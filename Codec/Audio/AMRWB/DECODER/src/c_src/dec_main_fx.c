/*------------------------------------------------------------------------*
*                         DEC_MAIN.C									  *
*------------------------------------------------------------------------*
* Performs the main decoder routine									  *
*------------------------------------------------------------------------*/

/*___________________________________________________________________________
|                                                                           |
| Fixed-point C simulation of AMR WB ACELP coding algorithm with 20 ms      |
| speech frames for wideband speech signals.                                |
|___________________________________________________________________________|
*/
#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "cnst_wb_fx.h"
#include "acelp_fx.h"
#include "dec_main_fx.h"
#include "bits_fx.h"
#include "math_op.h"
#include "main_fx.h"

#define L_WINDOW     384                   /* window size in LP analysis                 */
#define L_NEXT       64                    /* Overhead in LP analysis                    */
#define NB_SUBFR     4                     /* Number of subframe per frame               */

void voAMRWBDecSoft_exc_hf(Word16 *exc_hf, Word32 *mem);
void Smooth_ener_hf(Word16 *HF, Word32 *threshold);
void Oversamp_12k8(Word16 sig12k8[],     /* input: signal to oversampling */
				   Word16 sig_fs[],      /* output: oversampled signal */
				   Word16 lg,      /* input: length of output */
				   Word16 mem[], /* in/out: memory (2*L_FILT) */
				   Word16 band,    /* input: 0=0..6.4k, 1=6.4..10.8k */
				   Word16 add);
/* LPC interpolation coef {0.45, 0.8, 0.96, 1.0}; in Q15 */
static Word16 interpol_frac[NB_SUBFR] = {14746, 26214, 31457, 32767};

/* High Band encoding */
static const Word16 HP_gain[16] =
{
	3624, 4673, 5597, 6479, 7425, 8378, 9324, 10264,
	11210, 12206, 13391, 14844, 16770, 19655, 24289, 32728
};
/* isp tables for initialization */
static Word16 isp_init[M] =
{
	32138, 30274, 27246, 23170, 18205, 12540, 6393, 0,
	-6393, -12540, -18205, -23170, -27246, -30274, -32138, 1475
};
static Word16 isf_init[M] =
{
	1024, 2048, 3072, 4096, 5120, 6144, 7168, 8192,
	9216, 10240, 11264, 12288, 13312, 14336, 15360, 3840
};
static void synthesis(
					  Word16 *Aq,                          /* A(z)  : quantized Az               */
					  Word16 *exc,                         /* (i)   : excitation at 12kHz        */
					  Word16 Q_new,                         /* (i)   : scaling performed on exc   */
					  Word16 *synth16k,                    /* (o)   : 16kHz synthesis signal     */
					  Word16 prms,                          /* (i)   : parameter                  */
					  Word16 *HfIsf,
					  Word16 nb_bits,
					  Word16 newDTXState,
					  Decoder_State * st,                   /* (i/o) : State structure            */
					  Word16 bfi                            /* (i)   : bad frame indicator        */
					  );
/*-----------------------------------------------------------------*
*   Funtion  init_decoder	                                       *
*            ~~~~~~~~~~~~                                         *
*   ->Initialization of variables for the decoder section.        *
*-----------------------------------------------------------------*/
void Init_decoder(void **spd_state, VO_MEM_OPERATOR *pMemOP)
{
	/* Decoder states */
	Decoder_State *st;
	*spd_state = NULL;
	/*-------------------------------------------------------------------------*
	* Memory allocation for coder state.                                      *
	*-------------------------------------------------------------------------*/
	st = (Decoder_State *) voAMRWBDec_mem_malloc(pMemOP, sizeof(Decoder_State), 32);
	
	st->dtx_decSt = NULL;
	voAMRWBDecDtx_dec_init(&st->dtx_decSt, isf_init, pMemOP);
	Reset_decoder((void *) st, 1);
	*spd_state = (void *) st;
	return;
}

void Reset_decoder(void *st, Word16 reset_all)
{
	Word16 i;
	Decoder_State *dec_state;
	dec_state = (Decoder_State *) st;

	AMRWBDecSetZero(dec_state->old_exc, PIT_MAX + L_INTERPOL);
	AMRWBDecSetZero(dec_state->past_isfq, M);

	dec_state->old_T0_frac = 0;                       /* old pitch value = 64.0 */
	dec_state->old_T0 = 64; 
	dec_state->first_frame = 1;
	dec_state->L_gc_thres = 0;
	dec_state->tilt_code = 0;

	Init_Phase_dispersion(dec_state->disp_mem);

	/* scaling memories for excitation */
	dec_state->Q_old = Q_MAX; 
	dec_state->Qsubfr[3] = Q_MAX;
	dec_state->Qsubfr[2] = Q_MAX;
	dec_state->Qsubfr[1] = Q_MAX;
	dec_state->Qsubfr[0] = Q_MAX;

	if (reset_all != 0)
	{
		/* routines initialization */

		Init_D_gain2(dec_state->dec_gain);
		Init_Oversamp_16k(dec_state->mem_oversamp);
		Init_HP50_12k8(dec_state->mem_sig_out);
		Init_Filt_6k_7k(dec_state->mem_hf);
		Init_Filt_7k(dec_state->mem_hf3);
		Init_HP400_12k8(dec_state->mem_hp400);
		Init_Lagconc(dec_state->lag_hist);

		/* isp initialization */

		Copy(isp_init, dec_state->ispold, M);
		Copy(isf_init, dec_state->isfold, M);
		for (i = 0; i < L_MEANBUF; i++)
			Copy(isf_init, &dec_state->isf_buf[i * M], M);
		/* variable initialization */

		dec_state->mem_deemph = 0; 

		dec_state->seed = 21845;                             /* init random with 21845 */
		dec_state->seed2 = 21845;
		dec_state->seed3 = 21845;

		dec_state->state = 0; 
		dec_state->prev_bfi = 0; 

		/* Static vectors to zero */

		AMRWBDecSetZero(dec_state->mem_syn_hf, M16k);
		AMRWBDecSetZero(dec_state->mem_syn_hi, M);
		AMRWBDecSetZero(dec_state->mem_syn_lo, M);

		voAMRWBDecDtx_dec_reset(dec_state->dtx_decSt, isf_init);
		dec_state->vad_hist = 0;          

		AMRWBDecSetZero(dec_state->mem_syn_out, PIT_MAX + L_SUBFR );
		AMRWBDecSetZero(dec_state->mem_oversamp_hf_plus, (2 * L_FILT) );
		AMRWBDecSetZero(dec_state->mem_syn_hf_plus, 8);
		AMRWBDecSetZero(dec_state->lpc_hf_plus + 1, 8);
		dec_state->lpc_hf_plus[0] = 4096;   
		dec_state->gain_hf_plus = 0;        
		dec_state->threshold_hf = 0;        
		dec_state->lp_amp_hf = 0;           
		dec_state->ramp_state = 0;          
	}
	return;
}

void Close_decoder(void *spd_state, VO_MEM_OPERATOR  *pMemOP)
{
	voAMRWBDecDtx_dec_exit(&(((Decoder_State *) spd_state)->dtx_decSt), pMemOP);
	voAMRWBDec_mem_free(pMemOP, spd_state);
	return;
}

/*-----------------------------------------------------------------*
*   Funtion decoder		                                       *
*           ~~~~~~~	                                           *
*   ->Main decoder routine.                                       *
*                                                                 *
*-----------------------------------------------------------------*/
void voAMRWBDecMainProcess(
			 Word16 mode,                          /* input : used mode                     */
			 Word16 prms[],                        /* input : parameter vector              */
			 Word16 synth16k[],                    /* output: synthesis speech              */
			 Word16 * frame_length,                /* output:  lenght of the frame          */
			 void *spd_state,                      /* i/o   : State structure               */
			 Word16 frame_type                     /* input : received frame type           */
			 )
{
	/* Decoder states */
	Decoder_State *st;
	/* Excitation vector */
	Word16 old_exc[(L_FRAME + 1) + PIT_MAX + L_INTERPOL];
	Word16 *exc;
	/* LPC coefficients */
	Word16 *p_Aq;                          /* ptr to A(z) for the 4 subframes      */
	Word16 Aq[NB_SUBFR * (M + 1)];         /* A(z)   quantized for the 4 subframes */
	Word16 ispnew[M];                      /* immittance spectral pairs at 4nd sfr */
	Word16 isf[M];                         /* ISF (frequency domain) at 4nd sfr    */
	Word16 code[L_SUBFR];                  /* algebraic codevector                 */
	Word16 code2[L_SUBFR];                 /* algebraic codevector                 */
	Word16 exc2[L_FRAME];                  /* excitation vector                    */

	Word16 fac, stab_fac, voice_fac, Q_new = 0;
	Word32 L_tmp, L_gain_code;
	/* Scalars */
	Word16 i, j, i_subfr, index, ind[8], max, tmp;
	Word16 T0, T0_frac, pit_flag, T0_max, select, T0_min = 0;
	Word16 gain_pit, gain_code, gain_code_lo;
	Word16 newDTXState, bfi, unusable_frame, nb_bits;
	Word16 vad_flag;
	Word16 pit_sharp;
	Word16 excp[L_SUBFR];
	Word16 isf_tmp[M];
	Word16 HfIsf[M16k];

#if (!AWB_FUNC_DECODER_OPT)
	Word16 corr_gain = 0;
	st = (Decoder_State *) spd_state;
	/* mode verification */
	nb_bits = nb_of_bits[mode];            
	*frame_length = L_FRAME16k;            
	/* find the new  DTX state  SPEECH OR DTX */
	newDTXState = voAMRWBDecRx_dtx_handler(st->dtx_decSt, frame_type);
	if(newDTXState != SPEECH)
	{
		voAMRWBDecDtx_dec(st->dtx_decSt, exc2, newDTXState, isf, &prms);
	}
	/* SPEECH action state machine  */
	if ((frame_type == RX_SPEECH_BAD) || (frame_type == RX_SPEECH_PROBABLY_DEGRADED))
	{
		/* bfi for all index, bits are not usable */
		bfi = 1;                           
		unusable_frame = 0;                
	}  else if ((frame_type == RX_NO_DATA) || (frame_type == RX_SPEECH_LOST))
	{
		/* bfi only for lsf, gains and pitch period */
		bfi = 1;                           
		unusable_frame = 1;                
	} else
	{
		bfi = 0;                           
		unusable_frame = 0;                
	}
	if (bfi != 0)
	{
		st->state += 1;     
		if(st->state > 6)
		{
			st->state = 6;                 
		}
	} else
	{
		st->state >>=  1;     
	}
	/* If this frame is the first speech frame after CNI period,     */
	/* set the BFH state machine to an appropriate state depending   */
	/* on whether there was DTX muting before start of speech or not */
	/* If there was DTX muting, the first speech frame is muted.     */
	/* If there was no DTX muting, the first speech frame is not     */
	/* muted. The BFH state machine starts from state 5, however, to */
	/* keep the audible noise resulting from a SID frame which is    */
	/* erroneously interpreted as a good speech frame as small as    */
	/* possible (the decoder output in this case is quickly muted)   */
	if(st->dtx_decSt->dtxGlobalState == DTX)
	{
		st->state = 5;                     
		st->prev_bfi = 0;                 
	} else if(st->dtx_decSt->dtxGlobalState == DTX_MUTE)
	{
		st->state = 5;                     
		st->prev_bfi = 1;                 
	}
	if(newDTXState == SPEECH)
	{
		vad_flag = *prms++;
		if (bfi == 0)
		{
			if (vad_flag == 0)
			{
				st->vad_hist += 1; 
			} else
			{
				st->vad_hist = 0; 
			}
		}
	}
	/*----------------------------------------------------------------------*
	*                              DTX-CNG                                 *
	*----------------------------------------------------------------------*/
	if(newDTXState != SPEECH)          /* CNG mode */
	{
		/* increase slightly energy of noise below 200 Hz */

		/* Convert ISFs to the cosine domain */
		Isf_isp(isf, ispnew, M);
		Isp_Az(ispnew, Aq, M, 1);
		Copy(st->isfold, isf_tmp, M);
		for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
		{
			j = shr(i_subfr, 6);
			for (i = 0; i < M; i++)
			{
				L_tmp = L_mult(isf_tmp[i], sub(32767, interpol_frac[j]));
				L_tmp = L_mac(L_tmp, isf[i], interpol_frac[j]);
				HfIsf[i] = vo_round(L_tmp);
			}
			synthesis(Aq, &exc2[i_subfr], 0, &synth16k[i_subfr * 5 / 4], (short) 1, HfIsf, nb_bits, newDTXState, st, bfi);
		}
		/* reset speech coder memories */
		Reset_decoder(st, 0);
		Copy(isf, st->isfold, M);
		st->prev_bfi = bfi;               
		st->dtx_decSt->dtxGlobalState = newDTXState;
		return;
	}
	/*----------------------------------------------------------------------*
	*                               ACELP                                  *
	*----------------------------------------------------------------------*/
	/* copy coder memory state into working space (internal memory for DSP) */
	Copy(st->old_exc, old_exc, PIT_MAX + L_INTERPOL);
	exc = old_exc + PIT_MAX + L_INTERPOL;
	/* Decode the ISFs */
	if (sub(nb_bits, NBBITS_7k) <= 0)
	{
		ind[0] = *prms++;
		ind[1] = *prms++;
		ind[2] = *prms++;
		ind[3] = *prms++;
		ind[4] = *prms++;

		Dpisf_2s_36b(ind, isf, st->past_isfq, st->isfold, st->isf_buf, bfi, 1);
	} else
	{
		ind[0] = *prms++;
		ind[1] = *prms++;
		ind[2] = *prms++;
		ind[3] = *prms++;
		ind[4] = *prms++;
		ind[5] = *prms++;
		ind[6] = *prms++;

		Dpisf_2s_46b(ind, isf, st->past_isfq, st->isfold, st->isf_buf, bfi, 1);
	}

	/* Convert ISFs to the cosine domain */
	Isf_isp(isf, ispnew, M);
	if (st->first_frame != 0)
	{
		st->first_frame = 0; 
		Copy(ispnew, st->ispold, M);
	}
	/* Find the interpolated ISPs and convert to a[] for all subframes */
	Int_isp(st->ispold, ispnew, interpol_frac, Aq);

	/* update ispold[] for the next frame */
	Copy(ispnew, st->ispold, M);

	/* Check stability on isf : distance between old isf and current isf */
	L_tmp = 0; 
	for (i = 0; i < M - 1; i++)
	{
		tmp = sub(isf[i], st->isfold[i]);
		L_tmp = L_mac(L_tmp, tmp, tmp);
	}
	tmp = extract_h(L_shl(L_tmp, 8));
	tmp = mult(tmp, 26214);                /* tmp = L_tmp*0.8/256 */

	tmp = sub(20480, tmp);                 /* 1.25 - tmp */
	stab_fac = shl(tmp, 1);                /* Q14 -> Q15 with saturation */

	if (stab_fac < 0)
	{
		stab_fac = 0;
	}
	Copy(st->isfold, isf_tmp, M);
	Copy(isf, st->isfold, M);

	/*------------------------------------------------------------------------*
	*          Loop for every subframe in the analysis frame                 *
	*------------------------------------------------------------------------*
	* The subframe size is L_SUBFR and the loop is repeated L_FRAME/L_SUBFR  *
	*  times                                                                 *
	*     - decode the pitch delay and filter mode                           *
	*     - decode algebraic code                                            *
	*     - decode pitch and codebook gains                                  *
	*     - find voicing factor and tilt of code for next subframe.          *
	*     - find the excitation and compute synthesis speech                 *
	*------------------------------------------------------------------------*/
	p_Aq = Aq;                                      /* pointer to interpolated LPC parameters */

	for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
	{
		pit_flag = i_subfr; 

		if ((i_subfr == 2 * L_SUBFR) && (nb_bits > NBBITS_7k))
		{
			pit_flag = 0;
		}
		/*-------------------------------------------------*
		* - Decode pitch lag                              *
		* Lag indeces received also in case of BFI,       *
		* so that the parameter pointer stays in sync.    *
		*-------------------------------------------------*/
		if (pit_flag == 0)
		{
			if(nb_bits <= NBBITS_9k)
			{
				index = *prms++;
				if (index < (PIT_FR1_8b - PIT_MIN) * 2))
				{
					T0 = add(PIT_MIN, shr(index, 1));
					T0_frac = sub(index, shl(sub(T0, PIT_MIN), 1));
					T0_frac = shl(T0_frac, 1);
				} else
				{
					T0 = add(index, PIT_FR1_8b - ((PIT_FR1_8b - PIT_MIN) * 2));
					T0_frac = 0; 
				}
			} else
			{
				index = *prms++;
				if (sub(index, (PIT_FR2 - PIT_MIN) * 4) < 0)
				{
					T0 = add(PIT_MIN, shr(index, 2));
					T0_frac = sub(index, shl(sub(T0, PIT_MIN), 2));
				} else if (sub(index, (((PIT_FR2 - PIT_MIN) * 4) + ((PIT_FR1_9b - PIT_FR2) * 2))) < 0)
				{
					index = sub(index, (PIT_FR2 - PIT_MIN) * 4);
					T0 = add(PIT_FR2, shr(index, 1));
					T0_frac = sub(index, shl(sub(T0, PIT_FR2), 1));
					T0_frac = shl(T0_frac, 1);
				} else
				{
					T0 = add(index, (PIT_FR1_9b - ((PIT_FR2 - PIT_MIN) * 4) - ((PIT_FR1_9b - PIT_FR2) * 2)));
					T0_frac = 0; 
				}
			}

			/* find T0_min and T0_max for subframe 2 and 4 */
			T0_min = T0 - 8;
			if (T0_min < PIT_MIN)
			{
				T0_min = PIT_MIN; 
			}
			T0_max = add(T0_min, 15);

			if(T0_max > PIT_MAX)
			{
				T0_max = PIT_MAX;
				T0_min = sub(T0_max, 15);
			}
		} else
		{                                  /* if subframe 2 or 4 */
			if(nb_bits <= NBBITS_9k)
			{
				index = *prms++;
				T0 = add(T0_min, shr(index, 1));
				T0_frac = sub(index, shl(sub(T0, T0_min), 1));
				T0_frac = shl(T0_frac, 1);
			} else
			{
				index = *prms++;
				T0 = add(T0_min, shr(index, 2));
				T0_frac = sub(index, shl(sub(T0, T0_min), 2));
			}
		}

		/* check BFI after pitch lag decoding */
		if (bfi != 0)                      /* if frame erasure */
		{
			lagconc(&(st->dec_gain[17]), st->lag_hist, &T0, &(st->old_T0), &(st->seed3), unusable_frame);
			T0_frac = 0;
		}
		/*-------------------------------------------------*
		* - Find the pitch gain, the interpolation filter *
		*   and the adaptive codebook vector.             *
		*-------------------------------------------------*/
		Pred_lt4(&exc[i_subfr], T0, T0_frac, L_SUBFR + 1);
		if (unusable_frame)
		{
			select = 1; 
		} else
		{
			if(nb_bits <= NBBITS_9k)
			{
				select = 0; 
			} else
			{
				select = *prms++;
			}
		}

		if (select == 0)
		{
			/* find pitch excitation with lp filter */
			for (i = 0; i < L_SUBFR; i++)
			{
				L_tmp = L_mult(5898, exc[i - 1 + i_subfr]);
				L_tmp = L_mac(L_tmp, 20972, exc[i + i_subfr]);
				L_tmp = L_mac(L_tmp, 5898, exc[i + 1 + i_subfr]);
				code[i] = vo_round(L_tmp);
			}
			Copy(code, &exc[i_subfr], L_SUBFR);
		}
		/*-------------------------------------------------------*
		* - Decode innovative codebook.                         *
		* - Add the fixed-gain pitch contribution to code[].    *
		*-------------------------------------------------------*/
		if (unusable_frame != 0)
		{
			/* the innovative code doesn't need to be scaled (see Q_gain2) */
			for (i = 0; i < L_SUBFR; i++)
			{
				// @shanrong modified
#ifdef MSVC_AMRWBPLUS
				code[i] = shr(Random(&(st->seed)), 3); 
#elif EVC_AMRWBPLUS
				code[i] = shr(Random_evc(&(st->seed)), 3); 
#endif
				// end
			}
		} else if (sub(nb_bits, NBBITS_7k) <= 0)
		{
			ind[0] = *prms++;
			DEC_ACELP_2t64_fx(ind[0], code);
		} else if (sub(nb_bits, NBBITS_9k) <= 0)
		{
			for (i = 0; i < 4; i++)
			{
				ind[i] = *prms++;
			}
			DEC_ACELP_4t64_fx(ind, 20, code);
		} else if (sub(nb_bits, NBBITS_12k) <= 0)
		{
			for (i = 0; i < 4; i++)
			{
				ind[i] = *prms++;
			}
			DEC_ACELP_4t64_fx(ind, 36, code);
		} else if (sub(nb_bits, NBBITS_14k) <= 0)
		{
			ind[0] = *prms++;
			ind[1] = *prms++;
			ind[2] = *prms++;
			ind[3] = *prms++;
			DEC_ACELP_4t64_fx(ind, 44, code);
		} else if (sub(nb_bits, NBBITS_16k) <= 0)
		{
			for (i = 0; i < 4; i++)
			{
				ind[i] = *prms++;
			}
			DEC_ACELP_4t64_fx(ind, 52, code);
		} else if (sub(nb_bits, NBBITS_18k) <= 0)
		{
			for (i = 0; i < 4; i++)
			{
				ind[i] = *prms++;
			}
			for (i = 4; i < 8; i++)
			{
				ind[i] = *prms++;
			}
			DEC_ACELP_4t64_fx(ind, 64, code);
		} else if (sub(nb_bits, NBBITS_20k) <= 0)
		{
			ind[0] = *prms++;
			ind[1] = *prms++;
			ind[2] = *prms++;
			ind[3] = *prms++;
			ind[4] = *prms++;
			ind[5] = *prms++;
			ind[6] = *prms++;
			ind[7] = *prms++;
			DEC_ACELP_4t64_fx(ind, 72, code);
		} else
		{
			for (i = 0; i < 4; i++)
			{
				ind[i] = *prms++;
			}
			for (i = 4; i < 8; i++)
			{
				ind[i] = *prms++;
			}
			DEC_ACELP_4t64_fx(ind, 88, code);
		}

		tmp = 0;
		Preemph(code, st->tilt_code, L_SUBFR, &tmp);
		tmp = T0; 

		if(T0_frac > 2)
		{
			tmp += 1;
		}
		Pit_shrp(code, tmp, PIT_SHARP, L_SUBFR);

		/*-------------------------------------------------*
		* - Decode codebooks gains.                       *
		*-------------------------------------------------*/
		if(nb_bits <= NBBITS_9k)
		{
			index = *prms++;
			D_gain2(index, 6, code, L_SUBFR, &gain_pit, &L_gain_code, bfi, st->prev_bfi, st->state, unusable_frame, st->vad_hist, st->dec_gain);
		} else
		{
			index = *prms++;
			D_gain2(index, 7, code, L_SUBFR, &gain_pit, &L_gain_code, bfi, st->prev_bfi, st->state, unusable_frame, st->vad_hist, st->dec_gain);
		}

		/* find best scaling to perform on excitation (Q_new) */
		tmp = st->Qsubfr[0];
		for (i = 1; i < 4; i++)
		{
			if(st->Qsubfr[i] < tmp)
			{
				tmp = st->Qsubfr[i]; 
			}
		}
		/* limit scaling (Q_new) to Q_MAX: see cnst.h and syn_filt_32() */
		if(tmp > Q_MAX)
		{
			tmp = Q_MAX;                   
		}
		Q_new = 0;                         
		L_tmp = L_gain_code;                 /* L_gain_code in Q16 */

		while ((L_sub(L_tmp, 0x08000000L) < 0) && (sub(Q_new, tmp) < 0))
		{
			L_tmp = L_shl(L_tmp, 1);
			Q_new = add(Q_new, 1);
		}
		gain_code = vo_round(L_tmp);          /* scaled gain_code with Qnew */

		Scale_sig(exc + i_subfr - (PIT_MAX + L_INTERPOL), PIT_MAX + L_INTERPOL + L_SUBFR, sub(Q_new, st->Q_old));
		st->Q_old = Q_new; 

		/*----------------------------------------------------------*
		* Update parameters for the next subframe.                 *
		* - tilt of code: 0.0 (unvoiced) to 0.5 (voiced)           *
		*----------------------------------------------------------*/
		if (bfi == 0)
		{
			/* LTP-Lag history update */
			for (i = 4; i > 0; i--)
			{
				st->lag_hist[i] = st->lag_hist[i - 1]; 
			}
			st->lag_hist[0] = T0;          

			st->old_T0 = T0;               
			st->old_T0_frac = 0;             /* Remove fraction in case of BFI */
		}
		/* find voice factor in Q15 (1=voiced, -1=unvoiced) */
		Copy(&exc[i_subfr], exc2, L_SUBFR);
		Scale_sig(exc2, L_SUBFR, -3);
		/* post processing of excitation elements */

		if(nb_bits <= NBBITS_9k)
		{
			pit_sharp = shl(gain_pit, 1);
			if(pit_sharp > 16384)
			{
				for (i = 0; i < L_SUBFR; i++)
				{
					tmp = mult(exc2[i], pit_sharp);
					L_tmp = L_mult(tmp, gain_pit);
					L_tmp = L_shr(L_tmp, 1);
					excp[i] = vo_round(L_tmp);
				}
			}
		} else
		{
			pit_sharp = 0; 
		}
		voice_fac = voice_factor(exc2, -3, gain_pit, code, gain_code, L_SUBFR);
		/* tilt of code for next subframe: 0.5=voiced, 0=unvoiced */
		st->tilt_code = add(shr(voice_fac, 2), 8192); 

		/*-------------------------------------------------------*
		* - Find the total excitation.                          *
		* - Find synthesis speech corresponding to exc[].       *
		*-------------------------------------------------------*/
		Copy(&exc[i_subfr], exc2, L_SUBFR);
		for (i = 0; i < L_SUBFR; i++)
		{
			L_tmp = L_mult(code[i], gain_code);
			L_tmp = L_shl(L_tmp, 5);
			L_tmp = L_mac(L_tmp, exc[i + i_subfr], gain_pit);
			L_tmp = L_shl(L_tmp, 1);
			exc[i + i_subfr] = vo_round(L_tmp);
		}
		/* find maximum value of excitation for next scaling */
		max = 1;  
		for (i = 0; i < L_SUBFR; i++)
		{
			tmp = abs_s(exc[i + i_subfr]);
			if(tmp > max)
			{
				max = tmp; 
			}
		}

		/* tmp = scaling possible according to max value of excitation */
		tmp = sub(add(norm_s(max), Q_new), 1);

		st->Qsubfr[3] = st->Qsubfr[2];     
		st->Qsubfr[2] = st->Qsubfr[1];     
		st->Qsubfr[1] = st->Qsubfr[0];     
		st->Qsubfr[0] = tmp;               

		/*------------------------------------------------------------*
		* phase dispersion to enhance noise in low bit rate          *
		*------------------------------------------------------------*/
		/* L_gain_code in Q16 */
		L_Extract(L_gain_code, &gain_code, &gain_code_lo);
		if (sub(nb_bits, NBBITS_7k) <= 0)
			j = 0;                         /* high dispersion for rate <= 7.5 kbit/s */
		else if (sub(nb_bits, NBBITS_9k) <= 0)
			j = 1;                         /* low dispersion for rate <= 9.6 kbit/s */
		else
			j = 2;                         /* no dispersion for rate > 9.6 kbit/s */

		Phase_dispersion(gain_code, gain_pit, code, j, st->disp_mem);

		/*------------------------------------------------------------*
		* noise enhancer                                             *
		* ~~~~~~~~~~~~~~                                             *
		* - Enhance excitation on noise. (modify gain of code)       *
		*   If signal is noisy and LPC filter is stable, move gain   *
		*   of code 1.5 dB toward gain of code threshold.            *
		*   This decrease by 3 dB noise energy variation.            *
		*------------------------------------------------------------*/

		tmp = sub(16384, shr(voice_fac, 1));    /* 1=unvoiced, 0=voiced */
		fac = mult(stab_fac, tmp);
		L_tmp = L_gain_code; 
		if (L_sub(L_tmp, st->L_gc_thres) < 0)
		{
			L_tmp = L_add(L_tmp, Mpy_32_16(gain_code, gain_code_lo, 6226));
			if (L_sub(L_tmp, st->L_gc_thres) > 0)
			{
				L_tmp = st->L_gc_thres; 
			}
		} else
		{
			L_tmp = Mpy_32_16(gain_code, gain_code_lo, 27536);
			if (L_sub(L_tmp, st->L_gc_thres) < 0)
			{
				L_tmp = st->L_gc_thres;
			}
		}
		st->L_gc_thres = L_tmp;

		L_gain_code = Mpy_32_16(gain_code, gain_code_lo, sub(32767, fac));
		L_Extract(L_tmp, &gain_code, &gain_code_lo);
		L_gain_code = L_add(L_gain_code, Mpy_32_16(gain_code, gain_code_lo, fac));

		/*------------------------------------------------------------*
		* pitch enhancer                                             *
		* ~~~~~~~~~~~~~~                                             *
		* - Enhance excitation on voice. (HP filtering of code)      *
		*   On voiced signal, filtering of code by a smooth fir HP   *
		*   filter to decrease energy of code in low frequency.      *
		*------------------------------------------------------------*/
		tmp = add(shr(voice_fac, 3), 4096);/* 0.25=voiced, 0=unvoiced */
		L_tmp = L_deposit_h(code[0]);
		L_tmp = L_msu(L_tmp, code[1], tmp);
		code2[0] = vo_round(L_tmp);

		for (i = 1; i < L_SUBFR - 1; i++)
		{
			L_tmp = L_deposit_h(code[i]);
			L_tmp = L_msu(L_tmp, code[i + 1], tmp);
			L_tmp = L_msu(L_tmp, code[i - 1], tmp);
			code2[i] = vo_round(L_tmp);
		}

		L_tmp = L_deposit_h(code[L_SUBFR - 1]);
		L_tmp = L_msu(L_tmp, code[L_SUBFR - 2], tmp);
		code2[L_SUBFR - 1] = vo_round(L_tmp);

		/* build excitation */

		gain_code = vo_round(L_shl(L_gain_code, Q_new));

		for (i = 0; i < L_SUBFR; i++)
		{
			L_tmp = L_mult(code2[i], gain_code);
			L_tmp = L_shl(L_tmp, 5);
			L_tmp = L_mac(L_tmp, exc2[i], gain_pit);
			L_tmp = L_shl(L_tmp, 1);       /* saturation can occur here */
			exc2[i] = vo_round(L_tmp);
		}

		if (nb_bits <= NBBITS_9k)
		{
			if (pit_sharp > 16384)
			{
				for (i = 0; i < L_SUBFR; i++)
				{
					excp[i] = add(excp[i], exc2[i]);
				}
				voAMRWBDec_agc2(exc2, excp, L_SUBFR);
				Copy(excp, exc2, L_SUBFR);
			}
		}
		if(nb_bits <= NBBITS_7k)
		{
			j = shr(i_subfr, 6);
			for (i = 0; i < M; i++)
			{
				L_tmp = L_mult(isf_tmp[i], sub(32767, interpol_frac[j]));
				L_tmp = L_mac(L_tmp, isf[i], interpol_frac[j]);
				HfIsf[i] = vo_round(L_tmp);
			}
		} else
		{
			AMRWBDecSetZero(st->mem_syn_hf, M16k - M);
		}

		if(nb_bits >= NBBITS_24k)
		{
			corr_gain = *prms++;
			synthesis(p_Aq, exc2, Q_new, &synth16k[i_subfr * 5 / 4], corr_gain, HfIsf, nb_bits, newDTXState, st, bfi);
		} else
			synthesis(p_Aq, exc2, Q_new, &synth16k[i_subfr * 5 / 4], 0, HfIsf, nb_bits, newDTXState, st, bfi);

		p_Aq += (M + 1);                   /* interpolated LPC parameters for next subframe */
	}

	/*--------------------------------------------------*
	* Update signal for next frame.                    *
	* -> save past of exc[].                           *
	* -> save pitch parameters.                        *
	*--------------------------------------------------*/

	Copy(&old_exc[L_FRAME], st->old_exc, PIT_MAX + L_INTERPOL);

	Scale_sig(exc, L_FRAME, sub(0, Q_new));
	voAMRWBDecDtx_dec_activity_update(st->dtx_decSt, isf, exc);

	st->dtx_decSt->dtxGlobalState = newDTXState;

	st->prev_bfi = bfi; 

#else//OPT_DECODE
	Word16 corr_gain = 0;  
	st = (Decoder_State *) spd_state;
	/* mode verification */
	nb_bits = nb_of_bits[mode];

	*frame_length = L_FRAME16k;

	/* find the new  DTX state  SPEECH OR DTX */
	newDTXState = voAMRWBDecRx_dtx_handler(st->dtx_decSt, frame_type);

	if (newDTXState != SPEECH)
	{
		voAMRWBDecDtx_dec(st->dtx_decSt, exc2, newDTXState, isf, &prms);
	}
	/* SPEECH action state machine  */
	if ((frame_type == RX_SPEECH_BAD) || (frame_type == RX_SPEECH_PROBABLY_DEGRADED))
	{
		/* bfi for all index, bits are not usable */
		bfi = 1;
		unusable_frame = 0;
	}
	else if ((frame_type == RX_NO_DATA) || (frame_type == RX_SPEECH_LOST))
	{
		/* bfi only for lsf, gains and pitch period */
		bfi = 1;
		unusable_frame = 1;
	}
	else
	{
		bfi = 0;
		unusable_frame = 0;
	}
	if (bfi != 0)
	{
		st->state++;
		if (st->state > 6)
		{
			st->state = 6;
		}
	}
	else
	{
		st->state >>= 1;
	}

	/* If this frame is the first speech frame after CNI period,     */
	/* set the BFH state machine to an appropriate state depending   */
	/* on whether there was DTX muting before start of speech or not */
	/* If there was DTX muting, the first speech frame is muted.     */
	/* If there was no DTX muting, the first speech frame is not     */
	/* muted. The BFH state machine starts from state 5, however, to */
	/* keep the audible noise resulting from a SID frame which is    */
	/* erroneously interpreted as a good speech frame as small as    */
	/* possible (the decoder output in this case is quickly muted)   */
	if (st->dtx_decSt->dtxGlobalState == DTX)
	{
		st->state = 5;
		st->prev_bfi = 0;
	} 
	else if (st->dtx_decSt->dtxGlobalState == DTX_MUTE)
	{
		st->state = 5;
		st->prev_bfi = 1;
	}	
	if (newDTXState == SPEECH)
	{
		vad_flag = *prms++;
		if (bfi == 0)
		{
			if (vad_flag == 0)
			{
				st->vad_hist++;
			} else
			{
				st->vad_hist = 0;
			}
		}
	}
	/*----------------------------------------------------------------------*
	*                              DTX-CNG                                 *
	*----------------------------------------------------------------------*/
	if (newDTXState != SPEECH)     /* CNG mode */
	{
		/* increase slightly energy of noise below 200 Hz */

		/* Convert ISFs to the cosine domain */
		Isf_isp(isf, ispnew, M);
		Isp_Az(ispnew, Aq, M, 1);
		Copy(st->isfold, isf_tmp, M);
		for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
		{
			j = i_subfr >> 6;
			for (i = 0; i < M; i++)
			{
				L_tmp = isf_tmp[i] * (32767 - interpol_frac[j]);
				L_tmp += isf[i] * interpol_frac[j];
				HfIsf[i] = (L_tmp + 0x4000) >> 15;
			}
			synthesis(Aq, &exc2[i_subfr], 0, &synth16k[i_subfr * 5 / 4], (short) 1, HfIsf, nb_bits, newDTXState, st, bfi);
		}
		/* reset speech coder memories */
		Reset_decoder(st, 0);
		Copy(isf, st->isfold, M);
		st->prev_bfi = bfi;
		st->dtx_decSt->dtxGlobalState = newDTXState;
		return;
	}
	/*----------------------------------------------------------------------*
	*                               ACELP                                  *
	*----------------------------------------------------------------------*/
	/* copy coder memory state into working space (internal memory for DSP) */
	Copy(st->old_exc, old_exc, PIT_MAX + L_INTERPOL);
	exc = old_exc + PIT_MAX + L_INTERPOL;
	/* Decode the ISFs */
	if (nb_bits <= NBBITS_7k)
	{
		ind[0] = *prms++;
		ind[1] = *prms++;
		ind[2] = *prms++;
		ind[3] = *prms++;
		ind[4] = *prms++;
		Dpisf_2s_36b(ind, isf, st->past_isfq, st->isfold, st->isf_buf, bfi, 1);
	} else
	{
		ind[0] = *prms++;
		ind[1] = *prms++;
		ind[2] = *prms++;
		ind[3] = *prms++;
		ind[4] = *prms++;
		ind[5] = *prms++;
		ind[6] = *prms++;
		Dpisf_2s_46b(ind, isf, st->past_isfq, st->isfold, st->isf_buf, bfi, 1);
	}
	/* Convert ISFs to the cosine domain */
	Isf_isp(isf, ispnew, M);
	if (st->first_frame != 0)
	{
		st->first_frame = 0;
		Copy(ispnew, st->ispold, M);
	}
	/* Find the interpolated ISPs and convert to a[] for all subframes */
	Int_isp(st->ispold, ispnew, interpol_frac, Aq);
	/* update ispold[] for the next frame */
	Copy(ispnew, st->ispold, M);
	/* Check stability on isf : distance between old isf and current isf */
	L_tmp = 0;
	for (i = 0; i < M - 1; i++)
	{
		tmp = isf[i] - st->isfold[i];
		L_tmp += tmp * tmp;
	}
	tmp = extract_h(L_shl2(L_tmp, 9));   //can't be modified
	tmp = (tmp * 26214) >> 15;                /* tmp = L_tmp*0.8/256 */

	tmp = 20480 - tmp;                 /* 1.25 - tmp */
	stab_fac = shl(tmp, 1);                /* Q14 -> Q15 with saturation */ //can't be modified
	if (stab_fac < 0)
	{
		stab_fac = 0;
	}
	Copy(st->isfold, isf_tmp, M);
	Copy(isf, st->isfold, M);

	/*------------------------------------------------------------------------*
	*          Loop for every subframe in the analysis frame                 *
	*------------------------------------------------------------------------*
	* The subframe size is L_SUBFR and the loop is repeated L_FRAME/L_SUBFR  *
	*  times                                                                 *
	*     - decode the pitch delay and filter mode                           *
	*     - decode algebraic code                                            *
	*     - decode pitch and codebook gains                                  *
	*     - find voicing factor and tilt of code for next subframe.          *
	*     - find the excitation and compute synthesis speech                 *
	*------------------------------------------------------------------------*/
	p_Aq = Aq;/* pointer to interpolated LPC parameters */
	for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
	{
		pit_flag = i_subfr;

		if ((i_subfr == 2 * L_SUBFR) && (nb_bits > NBBITS_7k))
		{
			pit_flag = 0;
		}
		/*-------------------------------------------------*
		* - Decode pitch lag                              *
		* Lag indeces received also in case of BFI,       *
		* so that the parameter pointer stays in sync.    *
		*-------------------------------------------------*/
		if (pit_flag == 0)
		{
			if (nb_bits <= NBBITS_9k)
			{
				index = *prms++;
				if (index < (PIT_FR1_8b - PIT_MIN) * 2)
				{
					T0 = PIT_MIN + (index >> 1);
					T0_frac = index - ((T0 - PIT_MIN) << 1);
					T0_frac <<= 1;
				} else
				{
					T0 = (short)(index + PIT_FR1_8b - ((PIT_FR1_8b - PIT_MIN) << 1));
					T0_frac = 0;
				}
			} else
			{
				index = *prms++;
				if (index < ((PIT_FR2 - PIT_MIN) << 2))
				{
					T0 = PIT_MIN + (index >> 2);
					T0_frac = index - ((T0 - PIT_MIN) << 2);
				} else if (index < ((PIT_FR2 - PIT_MIN) << 2) + ((PIT_FR1_9b - PIT_FR2) << 1))
				{
					index -= (PIT_FR2 - PIT_MIN) << 2;
					T0 = PIT_FR2 + (index >> 1);
					T0_frac = index - ((T0 - PIT_FR2) << 1);
					T0_frac <<= 1;
				} else
				{
					T0 = (short)(index + PIT_FR1_9b - ((PIT_FR2 - PIT_MIN) << 2) - ((PIT_FR1_9b - PIT_FR2) << 1));
					T0_frac = 0;
				}
			}
			/* find T0_min and T0_max for subframe 2 and 4 */
			T0_min = T0 - 8;
			if (T0_min < PIT_MIN)
			{
				T0_min = PIT_MIN;
			}
			T0_max = T0_min + 15;
			if (T0_max > PIT_MAX)
			{
				T0_max = PIT_MAX;
				T0_min = T0_max - 15;
			}
		} else
		{                                  /* if subframe 2 or 4 */
			if (nb_bits <= NBBITS_9k)
			{
				index = *prms++;

				T0 = T0_min + (index >> 1);
				T0_frac = index - ((T0 - T0_min) << 1);
				T0_frac = T0_frac << 1;
			} else
			{
				index = *prms++;

				T0 = T0_min + (index >> 2);
				T0_frac = index - ((T0 - T0_min) << 2);
			}
		}
		/* check BFI after pitch lag decoding */
		if (bfi != 0)                      /* if frame erasure */
		{
			lagconc(&(st->dec_gain[17]), st->lag_hist, &T0, &(st->old_T0), &(st->seed3), unusable_frame);
			T0_frac = 0;
		}
		/*-------------------------------------------------*
		* - Find the pitch gain, the interpolation filter *
		*   and the adaptive codebook vector.             *
		*-------------------------------------------------*/
#ifdef ASM_OPT
		Pred_lt4_asm(&exc[i_subfr], T0, T0_frac, L_SUBFR + 1);
#else
		Pred_lt4(&exc[i_subfr], T0, T0_frac, L_SUBFR + 1);
#endif
		if (unusable_frame)
		{
			select = 1;
		} else
		{
			if (nb_bits <= NBBITS_9k)
			{
				select = 0;
			} else
			{
				select = *prms++;
			}
		}
		if (select == 0)
		{
			/* find pitch excitation with lp filter */
			for (i = 0; i < L_SUBFR; i++)
			{
				L_tmp = 5898 * exc[i - 1 + i_subfr];
				L_tmp += 20972 * exc[i + i_subfr];
				L_tmp += 5898 * exc[i + 1 + i_subfr];
				code[i] = (L_tmp + 0x4000) >> 15;
			}
			Copy(code, &exc[i_subfr], L_SUBFR);
		}
		/*-------------------------------------------------------*
		* - Decode innovative codebook.                         *
		* - Add the fixed-gain pitch contribution to code[].    *
		*-------------------------------------------------------*/
		if (unusable_frame != 0)
		{
			/* the innovative code doesn't need to be scaled (see Q_gain2) */
			for (i = 0; i < L_SUBFR; i++)
			{
				// @shanrong modified
#ifdef MSVC_AMRWBPLUS
				code[i] = Random(&(st->seed)) >> 3;
#elif EVC_AMRWBPLUS
				code[i] = Random_evc(&(st->seed)) >> 3;
#endif
				// end
			}
		} else if (nb_bits <= NBBITS_7k)
		{
			ind[0] = *prms++;
			DEC_ACELP_2t64_fx(ind[0], code);
		} else if (nb_bits <= NBBITS_9k)
		{
			ind[0] = *prms++;
			ind[1] = *prms++;
			ind[2] = *prms++;
			ind[3] = *prms++;
			DEC_ACELP_4t64_fx(ind, 20, code);
		} else if (nb_bits <= NBBITS_12k)
		{
			ind[0] = *prms++;
			ind[1] = *prms++;
			ind[2] = *prms++;
			ind[3] = *prms++;
			DEC_ACELP_4t64_fx(ind, 36, code);
		} else if (nb_bits <= NBBITS_14k)
		{
			ind[0] = *prms++;
			ind[1] = *prms++;
			ind[2] = *prms++;
			ind[3] = *prms++;
			DEC_ACELP_4t64_fx(ind, 44, code);
		} else if (nb_bits <= NBBITS_16k)
		{
			ind[0] = *prms++;
			ind[1] = *prms++;
			ind[2] = *prms++;
			ind[3] = *prms++;
			DEC_ACELP_4t64_fx(ind, 52, code);
		} else if (nb_bits <= NBBITS_18k)
		{
			ind[0] = *prms++;
			ind[1] = *prms++;
			ind[2] = *prms++;
			ind[3] = *prms++;
			ind[4] = *prms++;
			ind[5] = *prms++;
			ind[6] = *prms++;
			ind[7] = *prms++;
			DEC_ACELP_4t64_fx(ind, 64, code);
		} else if (nb_bits <= NBBITS_20k)
		{
			ind[0] = *prms++;
			ind[1] = *prms++;
			ind[2] = *prms++;
			ind[3] = *prms++;
			ind[4] = *prms++;
			ind[5] = *prms++;
			ind[6] = *prms++;
			ind[7] = *prms++;
			DEC_ACELP_4t64_fx(ind, 72, code);
		} else
		{
			ind[0] = *prms++;
			ind[1] = *prms++;
			ind[2] = *prms++;
			ind[3] = *prms++;
			ind[4] = *prms++;
			ind[5] = *prms++;
			ind[6] = *prms++;
			ind[7] = *prms++;
			DEC_ACELP_4t64_fx(ind, 88, code);
		}
#if AMR_DUMP
		{
			Dumploop2(AMR_DEBUG_DEC_ACELP,"after DEC_ACELP_2t64_fx",4,L_SUBFR/4,code,d16);
		}
#endif//AMR_DUMP
		tmp = 0;
		Preemph(code, st->tilt_code, L_SUBFR, &tmp);
		tmp = T0;
		if (T0_frac > 2)
		{
			tmp++;
		}
		Pit_shrp(code, tmp, PIT_SHARP, L_SUBFR);

		/*-------------------------------------------------*
		* - Decode codebooks gains.                       *
		*-------------------------------------------------*/
		if (nb_bits <= NBBITS_9k)
		{
			index = *prms++;
			D_gain2(index, 6, code, L_SUBFR, &gain_pit, &L_gain_code, bfi, st->prev_bfi, st->state, unusable_frame, st->vad_hist, st->dec_gain);
		} else
		{
			index = *prms++;
			D_gain2(index, 7, code, L_SUBFR, &gain_pit, &L_gain_code, bfi, st->prev_bfi, st->state, unusable_frame, st->vad_hist, st->dec_gain);
		}

		/* find best scaling to perform on excitation (Q_new) */

		tmp = st->Qsubfr[0];
		if (st->Qsubfr[1] < tmp) tmp = st->Qsubfr[1];
		if (st->Qsubfr[2] < tmp) tmp = st->Qsubfr[2];
		if (st->Qsubfr[3] < tmp) tmp = st->Qsubfr[3];

		/* limit scaling (Q_new) to Q_MAX: see cnst.h and syn_filt_32() */
		tmp = (tmp > Q_MAX) ? Q_MAX : tmp;
		Q_new = 0;
		L_tmp = L_gain_code;    /* L_gain_code in Q16 */

		while ((L_tmp < 0x08000000L) && (Q_new < tmp))
		{
			L_tmp <<= 1;
			Q_new++;
		}
		gain_code = vo_round(L_tmp);          /* scaled gain_code with Qnew */     //can't be modified
#ifdef ASM_OPT
		Scale_sig_asm(exc + i_subfr - (PIT_MAX + L_INTERPOL),
			PIT_MAX + L_INTERPOL + L_SUBFR, (Q_new - st->Q_old));
#else
		Scale_sig(exc + i_subfr - (PIT_MAX + L_INTERPOL),
			PIT_MAX + L_INTERPOL + L_SUBFR, (Q_new - st->Q_old));
#endif
		st->Q_old = Q_new;

		/*----------------------------------------------------------*
		* Update parameters for the next subframe.                 *
		* - tilt of code: 0.0 (unvoiced) to 0.5 (voiced)           *
		*----------------------------------------------------------*/
		if (bfi == 0)
		{
			/* LTP-Lag history update */
			for (i = 4; i > 0; i--)
			{
				st->lag_hist[i] = st->lag_hist[i - 1];
			}
			st->lag_hist[0] = T0;
			st->old_T0 = T0;
			st->old_T0_frac = 0;  /* Remove fraction in case of BFI */
		}
		/* find voice factor in Q15 (1=voiced, -1=unvoiced) */
		Copy(&exc[i_subfr], exc2, L_SUBFR);
#ifdef ASM_OPT
		Scale_sig_asm(exc2, L_SUBFR, -3);
#else
		Scale_sig(exc2, L_SUBFR, -3);
#endif

		/* post processing of excitation elements */
		if (nb_bits <= NBBITS_9k)
		{
			pit_sharp = shl(gain_pit, 1);
			if (pit_sharp > 16384)
			{
				for (i = 0; i < L_SUBFR; i++)
				{
					tmp = (exc2[i] * pit_sharp) >> 15;
					L_tmp = tmp * gain_pit;
					excp[i] = (L_tmp + 0x8000) >> 16;
				}
			}
		} else
		{
			pit_sharp = 0;
		}
		voice_fac = voice_factor(exc2, -3, gain_pit, code, gain_code, L_SUBFR);
		/* tilt of code for next subframe: 0.5=voiced, 0=unvoiced */
		st->tilt_code = (voice_fac >> 2) + 8192;

		/*-------------------------------------------------------*
		* - Find the total excitation.                          *
		* - Find synthesis speech corresponding to exc[].       *
		*-------------------------------------------------------*/
		Copy(&exc[i_subfr], exc2, L_SUBFR);
		for (i = 0; i < L_SUBFR; i++)
		{
			L_tmp = (code[i] * gain_code) << 5;
			L_tmp += exc[i + i_subfr] * gain_pit;
			L_tmp = L_shl2(L_tmp, 2);
			exc[i + i_subfr] = vo_round(L_tmp);
		}
		/* find maximum value of excitation for next scaling */
		max = 1;
		for (i = 0; i < L_SUBFR; i++)
		{
			tmp = abs_s(exc[i + i_subfr]);
			max = (tmp > max) ? tmp : max;
		}
		/* tmp = scaling possible according to max value of excitation */
		tmp = norm_s(max) + Q_new - 1;

		st->Qsubfr[3] = st->Qsubfr[2];
		st->Qsubfr[2] = st->Qsubfr[1];
		st->Qsubfr[1] = st->Qsubfr[0];
		st->Qsubfr[0] = tmp;

		/*------------------------------------------------------------*
		* phase dispersion to enhance noise in low bit rate          *
		*------------------------------------------------------------*/
		/* L_gain_code in Q16 */
		gain_code = L_gain_code >> 16;
		gain_code_lo = (L_gain_code & 0xffff)>>1;
		//L_Extract(L_gain_code, &gain_code, &gain_code_lo);

		if (nb_bits <= NBBITS_7k)
			j = 0;                         /* high dispersion for rate <= 7.5 kbit/s */
		else if (nb_bits <= NBBITS_9k)
			j = 1;                         /* low dispersion for rate <= 9.6 kbit/s */
		else
			j = 2;                         /* no dispersion for rate > 9.6 kbit/s */

		Phase_dispersion(gain_code, gain_pit, code, j, st->disp_mem);

		/*------------------------------------------------------------*
		* noise enhancer                                             *
		* ~~~~~~~~~~~~~~                                             *
		* - Enhance excitation on noise. (modify gain of code)       *
		*   If signal is noisy and LPC filter is stable, move gain   *
		*   of code 1.5 dB toward gain of code threshold.            *
		*   This decrease by 3 dB noise energy variation.            *
		*------------------------------------------------------------*/

		tmp = 16384 - (voice_fac >> 1);    /* 1=unvoiced, 0=voiced */
		fac = (stab_fac * tmp) >> 15;

		L_tmp = L_gain_code;

		if (L_tmp < st->L_gc_thres)
		{
			L_tmp += Mpy_32_16(gain_code, gain_code_lo, 6226);
			L_tmp = (L_tmp > st->L_gc_thres) ? st->L_gc_thres : L_tmp;
		} else
		{
			L_tmp = Mpy_32_16(gain_code, gain_code_lo, 27536);
			L_tmp = (L_tmp < st->L_gc_thres) ? st->L_gc_thres : L_tmp;
		}
		st->L_gc_thres = L_tmp;

		L_gain_code = Mpy_32_16(gain_code, gain_code_lo, sub(32767, fac));

        gain_code = L_tmp >> 16;
		gain_code_lo = (L_tmp & 0xffff)>>1;
		//L_Extract(L_tmp, &gain_code, &gain_code_lo);
		L_gain_code += Mpy_32_16(gain_code, gain_code_lo, fac);

		/*------------------------------------------------------------*
		* pitch enhancer                                             *
		* ~~~~~~~~~~~~~~                                             *
		* - Enhance excitation on voice. (HP filtering of code)      *
		*   On voiced signal, filtering of code by a smooth fir HP   *
		*   filter to decrease energy of code in low frequency.      *
		*------------------------------------------------------------*/

		tmp = (voice_fac >> 3) + 4096;/* 0.25=voiced, 0=unvoiced */

		L_tmp = L_deposit_h(code[0]);
		L_tmp -= (code[1] * tmp) << 1;
		code2[0] = (L_tmp + 0x8000) >> 16;

		for (i = 1; i < L_SUBFR - 1; i++)
		{
			L_tmp = code[i] << 15;
			L_tmp -= code[i + 1] * tmp;
			L_tmp -= code[i - 1] * tmp;
			code2[i] = (L_tmp + 0x4000) >> 15;
		}

		L_tmp = code[L_SUBFR - 1] << 15;
		L_tmp -= code[L_SUBFR - 2] * tmp;
		code2[L_SUBFR - 1] = (L_tmp + 0x4000) >> 15;

		/* build excitation */

		gain_code = vo_round(L_shl2(L_gain_code, Q_new));

		for (i = 0; i < L_SUBFR; i++)
		{
			L_tmp = (code2[i] * gain_code) << 5;
			L_tmp += exc2[i] * gain_pit;
			L_tmp = L_shl2(L_tmp, 2);       /* saturation can occur here */
			exc2[i] = vo_round(L_tmp);
		}

		//if (sub(nb_bits, NBBITS_9k) <= 0)
		if (nb_bits <= NBBITS_9k)
		{
			if (pit_sharp > 16384)
			{
				for (i = 0; i < L_SUBFR; i++)
				{
					excp[i] = add(excp[i], exc2[i]);        //can't be modified
				}
				voAMRWBDec_agc2(exc2, excp, L_SUBFR);
				Copy(excp, exc2, L_SUBFR);
			}
		}
		if (nb_bits <= NBBITS_7k)
		{
			j = i_subfr >> 6;
			for (i = 0; i < M; i++)
			{
				L_tmp = isf_tmp[i] * (32767 - interpol_frac[j]);
				L_tmp += isf[i] * interpol_frac[j];
				HfIsf[i] = (L_tmp + 0x4000) >> 15;
			}
		} else
		{
			AMRWBDecSetZero(st->mem_syn_hf, M16k - M);
		}

		if (nb_bits >= NBBITS_24k)
		{
			corr_gain = *prms++;
			synthesis(p_Aq, exc2, Q_new, &synth16k[i_subfr * 5 / 4], corr_gain, HfIsf, nb_bits, newDTXState, st, bfi);
		} else
			synthesis(p_Aq, exc2, Q_new, &synth16k[i_subfr * 5 / 4], 0, HfIsf, nb_bits, newDTXState, st, bfi);

		p_Aq += (M + 1);                   /* interpolated LPC parameters for next subframe */
	}

	/*--------------------------------------------------*
	* Update signal for next frame.                    *
	* -> save past of exc[].                           *
	* -> save pitch parameters.                        *
	*--------------------------------------------------*/

	Copy(&old_exc[L_FRAME], st->old_exc, PIT_MAX + L_INTERPOL);

#ifdef ASM_OPT
	Scale_sig_asm(exc, L_FRAME, -Q_new);
#else
	Scale_sig(exc, L_FRAME, -Q_new);
#endif
	voAMRWBDecDtx_dec_activity_update(st->dtx_decSt, isf, exc);

	st->dtx_decSt->dtxGlobalState = newDTXState;

	st->prev_bfi = bfi;
#endif

	return;
}



/*-----------------------------------------------------*
* Function synthesis()                                *
*                                                     *
* Synthesis of signal at 16kHz with HF extension.     *
*                                                     *
*-----------------------------------------------------*/
//FILE *log_file;

#if (AWB_FUNC_SYNTHESIS_OPT)
static Word16 synth_hi[M + L_SUBFR], synth_lo[M + L_SUBFR];
static Word16 synth[L_SUBFR];
static Word16 HF[L_SUBFR16k];                 /* High Frequency vector      */
static Word16 synth_buff[PIT_MAX + 2 * L_SUBFR];
//static Word16 synth_p[L_SUBFR];
#endif

static void synthesis(
					  Word16 *Aq,                          /* A(z)  : quantized Az               */
					  Word16 *exc,                         /* (i)   : excitation at 12kHz        */
					  Word16 Q_new,                         /* (i)   : scaling performed on exc   */
					  Word16 *synth16k,                    /* (o)   : 16kHz synthesis signal     */
					  Word16 prms,                          /* (i)   : parameter                  */
					  Word16 *HfIsf,
					  Word16 nb_bits,
					  Word16 newDTXState,
					  Decoder_State * st,                   /* (i/o) : State structure            */
					  Word16 bfi                            /* (i)   : bad frame indicator        */
					  )
{
	Word16 i, fac, tmp, exp;
	Word16 ener, exp_ener;
	Word32 L_tmp, L_tmp1;
	Word16 Ap[M16k + 1];
	Word16 HfA[M16k + 1];
	Word16 HF_corr_gain;
	Word16 HF_gain_ind;
	Word16 gain1, gain2;
	Word16 weight1, weight2;
	Word16 *synth_new, *synth_out;

	synth_new = synth_buff + PIT_MAX + L_SUBFR;                                    
	synth_out = synth_buff + PIT_MAX - L_FILT; 

	//memcpy(synth_buff, st->mem_syn_out, (PIT_MAX + L_SUBFR)<<1);

	Copy1(st->mem_syn_out, synth_buff, (PIT_MAX + L_SUBFR));     

	/*------------------------------------------------------------*
	* speech synthesis                                           *
	* ~~~~~~~~~~~~~~~~                                           *
	* - Find synthesis speech corresponding to exc2[].           *
	* - Perform fixed deemphasis and hp 50hz filtering.          *
	* - Oversampling from 12.8kHz to 16kHz.                      *
	*------------------------------------------------------------*/
	//memcpy(synth_hi, st->mem_syn_hi, M <<1);
	//memcpy(synth_lo, st->mem_syn_lo, M <<1);

	Copy(st->mem_syn_hi, synth_hi, M);
	Copy(st->mem_syn_lo, synth_lo, M);

	//expand syn_filt_32 to replace calling the function
#ifndef ASM_OPT //(MAX_OPT) 
	{
		Word16 i, a0;
		Word32 L_tmp;
		Word16 *sig_hi = synth_hi + M;
		Word16 *sig_lo = synth_lo + M;

		a0 = Aq[0] >>  (4 + Q_new);          /* input / 16 and >>Q_new */
		/* Do the filtering. */
		for (i = 0; i < L_SUBFR; i++)
		{
			L_tmp = -(sig_lo[i -  1] * Aq[ 1]
			+ sig_lo[i -  2] * Aq[ 2]
			+ sig_lo[i -  3] * Aq[ 3]
			+ sig_lo[i -  4] * Aq[ 4]
			+ sig_lo[i -  5] * Aq[ 5]
			+ sig_lo[i -  6] * Aq[ 6]
			+ sig_lo[i -  7] * Aq[ 7]
			+ sig_lo[i -  8] * Aq[ 8]
			+ sig_lo[i -  9] * Aq[ 9]
			+ sig_lo[i - 10] * Aq[10]
			+ sig_lo[i - 11] * Aq[11]
			+ sig_lo[i - 12] * Aq[12]
			+ sig_lo[i - 13] * Aq[13]
			+ sig_lo[i - 14] * Aq[14]
			+ sig_lo[i - 15] * Aq[15]
			+ sig_lo[i - 16] * Aq[16]);

			L_tmp >>= 11;      /* -4 : sig_lo[i] << 4 */

			L_tmp += (exc[i] * a0) << 1;

			L_tmp -= (sig_hi[i -  1] * Aq[ 1]
			+ sig_hi[i -  2] * Aq[ 2]
			+ sig_hi[i -  3] * Aq[ 3]
			+ sig_hi[i -  4] * Aq[ 4]
			+ sig_hi[i -  5] * Aq[ 5]
			+ sig_hi[i -  6] * Aq[ 6]
			+ sig_hi[i -  7] * Aq[ 7]
			+ sig_hi[i -  8] * Aq[ 8]
			+ sig_hi[i -  9] * Aq[ 9]
			+ sig_hi[i - 10] * Aq[10]
			+ sig_hi[i - 11] * Aq[11]
			+ sig_hi[i - 12] * Aq[12]
			+ sig_hi[i - 13] * Aq[13]
			+ sig_hi[i - 14] * Aq[14]
			+ sig_hi[i - 15] * Aq[15]
			+ sig_hi[i - 16] * Aq[16]) << 1;

			/* sig_hi = bit16 to bit31 of synthesis */
			//            L_tmp = L_shl(L_tmp, (Word16)(3 + s));           /* ai in Q12 */
			L_tmp <<= 3 ;           /* ai in Q12 */

			sig_hi[i] = L_tmp >> 16;

			/* sig_lo = bit4 to bit15 of synthesis */
			L_tmp >>= 4;                                      /* 4 : sig_lo[i] >> 4 */

			sig_lo[i] = L_tmp - (sig_hi[i] << 12);
		}
	}
#else
	Syn_filt_32_asm(Aq, M, exc, Q_new, synth_hi + M, synth_lo + M, L_SUBFR);
	//Syn_filt_32(Aq, M, exc, Q_new, synth_hi + M, synth_lo + M, L_SUBFR);
#endif

	//memcpy(st->mem_syn_hi, synth_hi + L_SUBFR, M <<1);
	//memcpy(st->mem_syn_lo, synth_lo + L_SUBFR, M <<1);
	Copy(synth_hi + L_SUBFR, st->mem_syn_hi, M);
	Copy(synth_lo + L_SUBFR, st->mem_syn_lo, M);


	Deemph_32(synth_hi + M, synth_lo + M, synth_new, PREEMPH_FAC, L_SUBFR, &(st->mem_deemph));
	HP50_12k8(synth_out, L_SUBFR, st->mem_sig_out);
	Oversamp_16k(synth_out, L_SUBFR, synth16k, st->mem_oversamp);

	Copy(synth_out, synth, L_SUBFR);

	//memcpy( synth, synth_out, L_SUBFR<<1);
	/*------------------------------------------------------*
	* HF noise synthesis                                   *
	* ~~~~~~~~~~~~~~~~~~                                   *
	* - Generate HF noise between 5.5 and 7.5 kHz.         *
	* - Set energy of noise according to synthesis tilt.   *
	*     tilt > 0.8 ==> - 14 dB (voiced)                  *
	*     tilt   0.5 ==> - 6 dB  (voiced or noise)         *
	*     tilt < 0.0 ==>   0 dB  (noise)                   *
	*------------------------------------------------------*/

	/* generate white noise vector */
	for (i = 0; i < L_SUBFR16k; i++)
	{
		// @shanrong modified
#ifdef MSVC_AMRWBPLUS
		HF[i] = Random(&(st->seed2)) >> 3;
#elif EVC_AMRWBPLUS
		HF[i] = Random_evc(&(st->seed2)) >> 3;
#endif
		// end
	}
	/* energy of excitation */
#ifdef ASM_OPT
	Scale_sig_asm(exc, L_SUBFR, -3);
#else
	Scale_sig(exc, L_SUBFR, -3);
#endif
	Q_new -= 3;

#ifdef ASM_OPT
	ener = extract_h(Dot_product12_asm(exc, exc, L_SUBFR, &exp_ener));
#else
	ener = extract_h(Dot_product12(exc, exc, L_SUBFR, &exp_ener));
#endif
	exp_ener -= Q_new << 1;
	/* set energy of white noise to energy of excitation */
#ifdef ASM_OPT
	tmp = extract_h(Dot_product12_asm(HF, HF, L_SUBFR16k, &exp));
#else
	tmp = extract_h(Dot_product12(HF, HF, L_SUBFR16k, &exp));
#endif
	if (tmp > ener)
	{
		tmp >>= 1;                 /* Be sure tmp < ener */
		exp++;
	}
	L_tmp = L_deposit_h(div_s(tmp, ener)); /* result is normalized */
	exp -= exp_ener;
	Isqrt_n(&L_tmp, &exp);
	L_tmp = L_shl(L_tmp, (Word16)(exp + 1));     /* L_tmp x 2, L_tmp in Q31 */
	tmp = extract_h(L_tmp);                /* tmp = 2 x sqrt(ener_exc/ener_hf) */

	for (i = 0; i < L_SUBFR16k; i++)
	{
		HF[i] = (HF[i] * tmp) >> 15;
	}

	/* find tilt of synthesis speech (tilt: 1=voiced, -1=unvoiced) */

	HP400_12k8(synth, L_SUBFR, st->mem_hp400);

	L_tmp  = 0;
	L_tmp1 = 0;

#if 1 // clean up warning, correct some optimization wrong
	for (i = 0; i < L_SUBFR; i++)
	{
		L_tmp += synth[i] * synth[i];
	}

	L_tmp1 = 1;
	for (i = 1; i < L_SUBFR; i++)
	{
		L_tmp1 += synth[i] * synth[i-1];
	}

#else
	for (i = 0; i < L_SUBFR; i++)
	{
		L_tmp += synth[i] * synth[i];
		L_tmp1 += synth[i] * synth[i-1];
	}
#endif

	L_tmp = (L_tmp << 1) + 1;
	L_tmp1 = (L_tmp1 << 1) + 1;
	exp = norm_l(L_tmp);

	ener = extract_h((L_tmp << exp));   /* ener = r[0] */
	tmp = extract_h((L_tmp1 << exp));    /* tmp = r[1] */

	if (tmp > 0)
	{
		fac = div_s(tmp, ener);
	}
	else
	{
		fac = 0;                          
	}
	/* modify energy of white noise according to synthesis tilt */
	gain1 = 32767 - fac;
	gain2 = (gain1 * 20480) >> 15;
	gain2 = shl(gain2, 1);

	if (st->vad_hist > 0)
	{
		weight1 = 0;                       
		weight2 = 32767;                   
	} else
	{
		weight1 = 32767;                  
		weight2 = 0;                       
	}
	tmp = (weight1 * gain1) >> 15;
	tmp += (weight2 * gain2) >> 15;

	if (tmp != 0)
	{
		tmp++;
	}
	if (tmp < 3277)
	{
		tmp = 3277;                        /* 0.1 in Q15 */
	}
	if ((nb_bits >= NBBITS_24k) && (bfi == 0))
	{
		/* HF correction gain */
		HF_gain_ind = prms;
		HF_corr_gain = HP_gain[HF_gain_ind];

		/* HF gain */
		for (i = 0; i < L_SUBFR16k; i++)
		{
			HF[i] = ((HF[i] * HF_corr_gain) >> 15) << 1;
		}
	} else
	{
		for (i = 0; i < L_SUBFR16k; i++)
		{
			HF[i] = (HF[i] * tmp) >> 15;
		}
	}

	if ((nb_bits <= NBBITS_7k) && (newDTXState == SPEECH))
	{
		Isf_Extrapolation(HfIsf);
		Isp_Az(HfIsf, HfA, M16k, 0);

		Weight_a(HfA, Ap, 29491, M16k);    /* fac=0.9 */
		voAMRWBDecSynFilt(Ap, M16k, HF, HF, L_SUBFR16k, st->mem_syn_hf, 1);
	} else
	{
		/* synthesis of noise: 4.8kHz..5.6kHz --> 6kHz..7kHz */
		Weight_a(Aq, Ap, 19661, M);        /* fac=0.6 */

#ifdef  ASM_OPT 
		Syn_filt_asm(Ap,HF, HF,st->mem_syn_hf + (M16k - M));
#else
		voAMRWBDecSynFilt(Ap, M, HF, HF, L_SUBFR16k, st->mem_syn_hf + (M16k - M), 1);
#endif
	}

	/* noise High Pass filtering (1ms of delay) */
#ifdef ASM_OPT
	Filt_6k_7k_asm(HF, L_SUBFR16k, st->mem_hf);
#else
	Filt_6k_7k(HF, L_SUBFR16k, st->mem_hf);
#endif
	if (nb_bits >= NBBITS_24k)
	{
		/* Low Pass filtering (7 kHz) */
#ifdef ASM_OPT
		Filt_7k_asm(HF, L_SUBFR16k, st->mem_hf3);
#else
		Filt_7k(HF, L_SUBFR16k, st->mem_hf3);
#endif
	}
	/* add filtered HF noise to speech synthesis */
	for (i = 0; i < L_SUBFR16k; i++)
	{
		synth16k[i] = add(synth16k[i], HF[i]);
	}

	Copy1(synth_buff + L_SUBFR, st->mem_syn_out, PIT_MAX + L_SUBFR);

	//memcpy(st->mem_syn_out, synth_buff + L_SUBFR, (PIT_MAX + L_SUBFR)<<1);
	return;
}
