/*
*  Copyright 2008 by Visualon software Incorporated.
*  All rights reserved. Property of Visualon software Incorporated.
*  Restricted rights to use, duplicate or disclose this code are
*  granted through contract.
*  
*/

/***************************** Change History**************************
;* 
;*    DD/MMM/YYYY     Code Ver         Description             Author
;*    -----------     --------     ------------------        -----------
;*    11-12-2008        1.0        File imported from        Huaping Liu
;*                                             
;**********************************************************************/
/*------------------------------------------------------------------------*
*                         amrwb_e_api.c                                     *
*------------------------------------------------------------------------*
* Performs the main encoder routine                                      *
*------------------------------------------------------------------------*/

/*___________________________________________________________________________
|                                                                           |
| Fixed-point C simulation of AMR WB ACELP coding algorithm with 20 ms      |
| speech frames for wideband speech signals.                                |
|___________________________________________________________________________|
*/

#include <stdio.h>
#include <stdlib.h>
#include "voCheck.h"
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "math_op.h"
#include "cnst.h"
#include "acelp.h"
#include "cod_main.h"
#include "bits.h"
#include "main.h"
#include "voAMRWB.h"
#include "mem_align.h"
#include "wb_ghr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define  VOAMRWBENC_SCRATCHMEM       ((2720*4))

#define	c_s_len		((8*L_SUBFR) + 186 + 1024)   /* scratch for search/synth functions  */

/* LPC coefficients */
#define	r_h			(pTemp)					     /* Autocorrelations of windowed speech  */
#define	r_l			(r_h + M + 2)				 /* Autocorrelations of windowed speech  */
#define	rc			(r_l + M + 2)				 /* Reflection coefficients.             */
#define	Ap			(rc + M)					 /* A(z) with spectral expansion         */
#define	ispnew		(Ap + M +2)					 /* immittance spectral pairs at 4nd sfr */
#define	ispnew_q	(ispnew + M)				 /* quantized ISPs at 4nd subframe       */
#define	isf			(ispnew_q + M)				 /* ISF (frequency domain) at 4nd sfr    */
#define	A			(isf + M)					 /* A(z) unquantized for the 4 subframes */
#define	Aq			(A + (NB_SUBFR * (M + 1)))	 /* A(z)   quantized for the 4 subframes */

/* Other Vector */
#define	xn			(Aq + (NB_SUBFR * (M + 1))) /* Target vector for pitch search     */
#define	xn2			(xn + L_SUBFR)				 /* Target vector for codebook search  */
#define	dn			(xn2 + L_SUBFR)				 /* Correlation between xn2 and h1     */
#define	cn			(dn + L_SUBFR)				 /* Target vector in residual domain   */
#define	h1			(cn + c_s_len)				 /* Impulse response vector            */
#define	h2			(h1 + L_SUBFR)				 /* Impulse response vector            */
#define	code		(h2 + L_SUBFR)				 /* Fixed codebook excitation          */
#define	y1			(code + L_SUBFR)			 /* Filtered adaptive excitation       */
#define	y2			(y1 + L_SUBFR)				 /* Filtered adaptive excitation       */
#define	error		(y2 + L_SUBFR+2)				 /* error of quantization          */
#define	synth		(error + M + L_SUBFR)		 /* 12.8kHz wbamre_synthesis vector           */
#define	exc2		(synth + L_SUBFR)			 /* excitation vector                  */
#define	buf			(exc2 + L_FRAME)			 /* VAD buffer                         */

#if ((L_FRAME + buf)%4)
#define	code2		(buf + L_FRAME + 2)				 /* Fixed codebook excitation   	   */
#else
#define 	code2		(buf + L_FRAME)				 /* Fixed codebook excitation   	   */
#endif

#define pSynFilt	 code2
#define pAutoCorr    error
#define	pSynthesis	 (cn + L_SUBFR)

/* LPC interpolation coef {0.45, 0.8, 0.96, 1.0}; in Q15 */
static Word16 interpol_frac[NB_SUBFR] = {14746, 26214, 31457, 32767};

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

/* High Band encoding */
static const Word16 HP_gain[16] =
{
	3624, 4673, 5597, 6479, 7425, 8378, 9324, 10264,
	11210, 12206, 13391, 14844, 16770, 19655, 24289, 32728
};

static Word16 synthesis(
			Word16 Aq1[],                          /* A(z)  : quantized Az               */
			Word16 exc[],                         /* (i)   : excitation at 12kHz        */
			Word16 Q_new,                         /* (i)   : scaling performed on exc   */
			Word16 synth16k[],                    /* (o)   : 16kHz synthesis signal     */
			Coder_State * st,                      /* (i/o) : State structure            */
			Word16 yy[]
			);

void Reset_encoder(void *st, Word16 reset_all)
{
	Word16 i;
	Coder_State *cod_state;
	cod_state = (Coder_State *) st;
	Set_zero(cod_state->old_exc, PIT_MAX + L_INTERPOL);
	Set_zero(cod_state->mem_syn, M);
	Set_zero(cod_state->past_isfq, M);
	cod_state->mem_w0 = 0;                 
	cod_state->tilt_code = 0;              
	cod_state->first_frame = 1;            
	Init_gp_clip(cod_state->gp_clip);
	cod_state->L_gc_thres = 0;             
	if (reset_all != 0)
	{
		/* Static vectors to zero */
		Set_zero(cod_state->old_speech, L_TOTAL - L_FRAME);
		Set_zero(cod_state->old_wsp, (PIT_MAX / OPL_DECIM));
		Set_zero(cod_state->mem_decim2, 3);
		/* routines initialization */
		Init_Decim_12k8(cod_state->mem_decim);
		Init_HP50_12k8(cod_state->mem_sig_in);
		Init_Levinson(cod_state->mem_levinson);
		Init_Q_gain2(cod_state->qua_gain);
		Init_Hp_wsp(cod_state->hp_wsp_mem);
		/* isp initialization */
		voAMRWBEnc_Copy(isp_init, cod_state->ispold, M);
		voAMRWBEnc_Copy(isp_init, cod_state->ispold_q, M);
		/* variable initialization */
		cod_state->mem_preemph = 0;        
		cod_state->mem_wsp = 0;            
		cod_state->Q_old = 15;             
		cod_state->Q_max[0] = 15;          
		cod_state->Q_max[1] = 15;          
		cod_state->old_wsp_max = 0;        
		cod_state->old_wsp_shift = 0;      
		/* pitch ol initialization */
		cod_state->old_T0_med = 40;        
		cod_state->ol_gain = 0;            
		cod_state->ada_w = 0;              
		cod_state->ol_wght_flg = 0;        
		for (i = 0; i < 5; i++)
		{
			cod_state->old_ol_lag[i] = 40; 
		}
		Set_zero(cod_state->old_hp_wsp, (L_FRAME / 2) / OPL_DECIM + (PIT_MAX / OPL_DECIM));
		Set_zero(cod_state->mem_syn_hf, M);
		Set_zero(cod_state->mem_syn_hi, M);
		Set_zero(cod_state->mem_syn_lo, M);
		Init_HP50_12k8(cod_state->mem_sig_out);
		Init_Filt_6k_7k(cod_state->mem_hf);
		Init_HP400_12k8(cod_state->mem_hp400);
		voAMRWBEnc_Copy(isf_init, cod_state->isfold, M);
		cod_state->mem_deemph = 0;         
		cod_state->seed2 = 21845;          
		Init_Filt_6k_7k(cod_state->mem_hf2);
		cod_state->gain_alpha = 32767;     
		cod_state->vad_hist = 0;
		wb_vad_reset(&(cod_state->vadSt));
		dtx_enc_reset(&(cod_state->dtx_encSt), isf_init);
	}
	return;
}



/*-----------------------------------------------------------------*
*   Funtion  coder                                                *
*            ~~~~~                                                *
*   ->Main coder routine.                                         *
*                                                                 *
*-----------------------------------------------------------------*/
void coder(
		   Word16 * mode,                        /* input :  used mode                             */
		   Word16 speech16k[],                   /* input :  320 new speech samples (at 16 kHz)    */
		   Word16 prms[],                        /* output:  output parameters                     */
		   Word16 * ser_size,                    /* output:  bit rate of the used mode             */
		   void *spe_state,                      /* i/o   :  State structure                       */
		   Word16 allow_dtx                      /* input :  DTX ON/OFF                            */
		   )
{
	/* Coder states */
	Coder_State *st;
	/* Speech vector */
	Word16 *pTemp;
	Word16 *old_speech, *old_wsp, *old_exc;
	Word16 *new_speech, *speech, *p_window;

	/* Weighted speech vector */
	/* Excitation vector */
	Word16 *wsp;
	Word16 *exc;

	Word16 *p_A, *p_Aq;                    /* ptr to A(z) for the 4 subframes      */

	/* Scalars */
	Word32 i, j, i_subfr, select, pit_flag, clip_gain, vad_flag;
	Word16 codec_mode;
	Word16 T_op, T_op2, T0, T0_min, T0_max, T0_frac, index;
	Word16 gain_pit, gain_code, g_coeff[4], g_coeff2[4];
	Word16 tmp, gain1, gain2, exp, Q_new, mu, shift, max;
	Word16 voice_fac;
	Word16 indice[8];

	Word32 L_tmp, L_gain_code, L_max, L_tmp1;
	Word16 stab_fac, fac, gain_code_lo;

	Word16 corr_gain;
	Word16 *vo_p0, *vo_p1, *vo_p2, *vo_p3;

	st = (Coder_State *) spe_state;
	pTemp = (Word16 *)st->pScratch;

#if code2%4
	old_speech = code2 + L_SUBFR + 1;			/* Speech vector */
#else
	old_speech = code2 + L_SUBFR;			/* Speech vector */
#endif

	old_wsp = old_speech + L_TOTAL+1;			/* Weighted speech vector */
	old_exc = old_wsp + (L_FRAME + ((PIT_MAX) / OPL_DECIM));	/* Excitation vector */

	*ser_size = nb_of_bits[*mode];         
	codec_mode = *mode;                    

	/*--------------------------------------------------------------------------*
	*          Initialize pointers to speech vector.                           *
	*                                                                          *
	*                                                                          *
	*                    |-------|-------|-------|-------|-------|-------|     *
	*                     past sp   sf1     sf2     sf3     sf4    L_NEXT      *
	*                    <-------  Total speech buffer (L_TOTAL)   ------>     *
	*              old_speech                                                  *
	*                    <-------  LPC analysis window (L_WINDOW)  ------>     *
	*                    |       <-- present frame (L_FRAME) ---->             *
	*                   p_window |       <----- new speech (L_FRAME) ---->     *
	*                            |       |                                     *
	*                          speech    |                                     *
	*                                 new_speech                               *
	*--------------------------------------------------------------------------*/

	new_speech = old_speech + L_TOTAL - L_FRAME - L_FILT;         /* New speech     */
	speech = old_speech + L_TOTAL - L_FRAME - L_NEXT;     /* Present frame  */
	p_window = old_speech + L_TOTAL - L_WINDOW; 

	exc = old_exc + PIT_MAX + L_INTERPOL;  
	wsp = old_wsp + (PIT_MAX / OPL_DECIM); 

	/* copy coder memory state into working space (internal memory for DSP) */

	voAMRWBEnc_Copy(st->old_speech, old_speech, L_TOTAL - L_FRAME);
	voAMRWBEnc_Copy(st->old_wsp, old_wsp, PIT_MAX / OPL_DECIM);
	voAMRWBEnc_Copy(st->old_exc, old_exc, PIT_MAX + L_INTERPOL);

	/*---------------------------------------------------------------*
	* Down sampling signal from 16kHz to 12.8kHz                    *
	* -> The signal is extended by L_FILT samples (padded to zero)  *
	* to avoid additional delay (L_FILT samples) in the coder.      *
	* The last L_FILT samples are approximated after decimation and *
	* are used (and windowed) only in autocorrelations.             *
	*---------------------------------------------------------------*/
#if 0 //def ARMV7
	decim_12k8_asm(speech16k, L_FRAME16k, new_speech, st->mem_decim);
#else
	Decim_12k8(speech16k, L_FRAME16k, new_speech, st->mem_decim);
#endif

	/* last L_FILT samples for autocorrelation window */
	voAMRWBEnc_Copy(st->mem_decim, code, 2 * L_FILT16k);
	Set_zero(error, L_FILT16k);            /* set next sample to zero */
#if 0 //def ARMV7
    decim_12k8_asm(error, L_FILT16k, new_speech + L_FRAME, code);
#else
	Decim_12k8(error, L_FILT16k, new_speech + L_FRAME, code);
#endif

	/*---------------------------------------------------------------*
	* Perform 50Hz HP filtering of input signal.                    *
	*---------------------------------------------------------------*/

	HP50_12k8(new_speech, L_FRAME, st->mem_sig_in);

	/* last L_FILT samples for autocorrelation window */
	voAMRWBEnc_Copy(st->mem_sig_in, code, 6);
	HP50_12k8(new_speech + L_FRAME, L_FILT, code);

	/*---------------------------------------------------------------*
	* Perform fixed preemphasis through 1 - g z^-1                  *
	* Scale signal to get maximum of precision in filtering         *
	*---------------------------------------------------------------*/

	mu = PREEMPH_FAC >> 1;              /* Q15 --> Q14 */

	/* get max of new preemphased samples (L_FRAME+L_FILT) */

	L_tmp = new_speech[0] << 15;
	L_tmp -= (st->mem_preemph * mu)<<1;
	L_max = L_abs(L_tmp);

	for (i = 1; i < L_FRAME + L_FILT; i++)
	{
		L_tmp = new_speech[i] << 15;
		L_tmp -= (new_speech[i - 1] * mu)<<1;
		L_tmp = L_abs(L_tmp);
		if(L_tmp > L_max)
		{
			L_max = L_tmp;                 
		}
	}

	/* get scaling factor for new and previous samples */
	/* limit scaling to Q_MAX to keep dynamic for ringing in low signal */
	/* limit scaling to Q_MAX also to avoid a[0]<1 in syn_filt_32 */
	tmp = extract_h(L_max);
	if (tmp == 0)
	{
		shift = Q_MAX;                     
	} else
	{
		shift = norm_s(tmp) - 1;
		if (shift < 0)
		{
			shift = 0;                     
		}
		if (shift > Q_MAX)
		{
			shift = Q_MAX;                 
		}
	}
	Q_new = shift;                         
	if (Q_new > st->Q_max[0])
	{
		Q_new = st->Q_max[0];              
	}
	if (Q_new > st->Q_max[1])
	{
		Q_new = st->Q_max[1];              
	}
	exp = (Q_new - st->Q_old);
	st->Q_old = Q_new;                     
	st->Q_max[1] = st->Q_max[0];           
	st->Q_max[0] = shift;                  

	/* preemphasis with scaling (L_FRAME+L_FILT) */

	tmp = new_speech[L_FRAME - 1];         

	for (i = L_FRAME + L_FILT - 1; i > 0; i--)
	{
		L_tmp = new_speech[i] << 15;
		L_tmp -= (new_speech[i - 1] * mu)<<1;
		L_tmp = (L_tmp << Q_new);
		new_speech[i] = vo_round(L_tmp);      
	}

	L_tmp = new_speech[0] << 15;
	L_tmp -= (st->mem_preemph * mu)<<1;
	L_tmp = (L_tmp << Q_new);
	new_speech[0] = vo_round(L_tmp);          

	st->mem_preemph = tmp;                 

	/* scale previous samples and memory */

#ifdef ASM_OPT
#ifdef ARMV7
	Scale_sig_opt_v6(old_speech, L_TOTAL - L_FRAME - L_FILT, exp);
	Scale_sig_opt_v6(old_exc, PIT_MAX + L_INTERPOL, exp);
	Scale_sig_opt_v6(st->mem_syn, M, exp);
	Scale_sig_opt_v6(st->mem_decim2, 3, exp);
	Scale_sig_opt_v6(&(st->mem_wsp), 1, exp);
	Scale_sig_opt_v6(&(st->mem_w0), 1, exp);
#else
	Scale_sig_opt(old_speech, L_TOTAL - L_FRAME - L_FILT, exp);
	Scale_sig_opt(old_exc, PIT_MAX + L_INTERPOL, exp);
	Scale_sig_opt(st->mem_syn, M, exp);
	Scale_sig_opt(st->mem_decim2, 3, exp);
	Scale_sig_opt(&(st->mem_wsp), 1, exp);
	Scale_sig_opt(&(st->mem_w0), 1, exp);
#endif 
#else
	Scale_sig(old_speech, L_TOTAL - L_FRAME - L_FILT, exp);
	Scale_sig(old_exc, PIT_MAX + L_INTERPOL, exp);
	Scale_sig(st->mem_syn, M, exp);
	Scale_sig(st->mem_decim2, 3, exp);
	Scale_sig(&(st->mem_wsp), 1, exp);
	Scale_sig(&(st->mem_w0), 1, exp);
#endif

	/*------------------------------------------------------------------------*
	*  Call VAD                                                              *
	*  Preemphesis scale down signal in low frequency and keep dynamic in HF.*
	*  Vad work slightly in futur (new_speech = speech + L_NEXT - L_FILT).   *
	*------------------------------------------------------------------------*/
	voAMRWBEnc_Copy(new_speech, buf, L_FRAME);
#ifdef ASM_OPT
	Scale_sig_opt(buf, L_FRAME, 1 - Q_new);
#else
	Scale_sig(buf, L_FRAME, 1 - Q_new);
#endif

	vad_flag = wb_vad(&(st->vadSt), buf);   //delete the function have one bit error.
	if (vad_flag == 0)
	{
		st->vad_hist = (st->vad_hist + 1);        
	} else
	{
		st->vad_hist = 0;             
	}

	/* DTX processing */
	if (allow_dtx != 0)
	{
		/* Note that mode may change here */
		tx_dtx_handler(&(st->dtx_encSt), vad_flag, mode);
		*ser_size = nb_of_bits[*mode]; 
	}

	if(*mode != MRDTX)
	{
		Parm_serial(vad_flag, 1, &prms);
	}
	/*------------------------------------------------------------------------*
	*  Perform LPC analysis                                                  *
	*  ~~~~~~~~~~~~~~~~~~~~                                                  *
	*   - autocorrelation + lag windowing                                    *
	*   - Levinson-durbin algorithm to find a[]                              *
	*   - convert a[] to isp[]                                               *
	*   - convert isp[] to isf[] for quantization                            *
	*   - quantize and code the isf[]                                        *
	*   - convert isf[] to isp[] for interpolation                           *
	*   - find the interpolated ISPs and convert to a[] for the 4 subframes  *
	*------------------------------------------------------------------------*/

	/* LP analysis centered at 4nd subframe */
	Autocorr(p_window, M, r_h, r_l, (Word32*)pAutoCorr);       /* Autocorrelations */
	Lag_window(r_h, r_l);                  /* Lag windowing    */
	Levinson(r_h, r_l, A, rc, st->mem_levinson);        /* Levinson Durbin  */
	Az_isp(A, ispnew, st->ispold);         /* From A(z) to ISP */

	/* Find the interpolated ISPs and convert to a[] for all subframes */
	Int_isp(st->ispold, ispnew, interpol_frac, A);

	/* update ispold[] for the next frame */
	voAMRWBEnc_Copy(ispnew, st->ispold, M);

	/* Convert ISPs to frequency domain 0..6400 */
	Isp_isf(ispnew, isf, M);

	/* check resonance for pitch clipping algorithm */
	Gp_clip_test_isf(isf, st->gp_clip);

	/*----------------------------------------------------------------------*
	*  Perform PITCH_OL analysis                                           *
	*  ~~~~~~~~~~~~~~~~~~~~~~~~~                                           *
	* - Find the residual res[] for the whole speech frame                 *
	* - Find the weighted input speech wsp[] for the whole speech frame    *
	* - scale wsp[] to avoid overflow in pitch estimation                  *
	* - Find open loop pitch lag for whole speech frame                    *
	*----------------------------------------------------------------------*/
	p_A = A;                             
	for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
	{
		voAMRWBEnc_Weight_a(p_A, Ap, GAMMA1, M);
#ifdef ASM_OPT
		Residu_opt(Ap, &speech[i_subfr], &wsp[i_subfr], L_SUBFR);
#else
		Residu(Ap, &speech[i_subfr], &wsp[i_subfr], L_SUBFR);
#endif

		p_A += (M + 1);                    
	}
	Deemph2(wsp, TILT_FAC, L_FRAME, &(st->mem_wsp));
	/* find maximum value on wsp[] for 12 bits scaling */
	max = 0;                              
	for (i = 0; i < L_FRAME; i++)
	{
		tmp = abs_s(wsp[i]);
		if(tmp > max)
		{
			max = tmp;                     
		}
	}
	tmp = st->old_wsp_max;                 
	if(max > tmp)
	{
		tmp = max;                         /* tmp = max(wsp_max, old_wsp_max) */
	}
	st->old_wsp_max = max;                

	shift = norm_s(tmp) - 3;
	if (shift > 0)
	{
		shift = 0;                         /* shift = 0..-3 */
	}
	/* decimation of wsp[] to search pitch in LF and to reduce complexity */
	LP_Decim2(wsp, L_FRAME, st->mem_decim2);

	/* scale wsp[] in 12 bits to avoid overflow */
#ifdef  ASM_OPT
	Scale_sig_opt(wsp, L_FRAME / OPL_DECIM, shift);
#else
	Scale_sig(wsp, L_FRAME / OPL_DECIM, shift);
#endif
	/* scale old_wsp (warning: exp must be Q_new-Q_old) */
	exp = exp + (shift - st->old_wsp_shift);
	st->old_wsp_shift = shift;

#ifdef  ASM_OPT
#ifdef  ARMV7
	Scale_sig_opt_v6(old_wsp, PIT_MAX / OPL_DECIM, exp);
	Scale_sig_opt_v6(st->old_hp_wsp, PIT_MAX / OPL_DECIM, exp);
#else	
	Scale_sig_opt(old_wsp, PIT_MAX / OPL_DECIM, exp);
	Scale_sig_opt(st->old_hp_wsp, PIT_MAX / OPL_DECIM, exp);
#endif
#else
	Scale_sig(old_wsp, PIT_MAX / OPL_DECIM, exp);
	Scale_sig(st->old_hp_wsp, PIT_MAX / OPL_DECIM, exp);
#endif
	scale_mem_Hp_wsp(st->hp_wsp_mem, exp);

	/* Find open loop pitch lag for whole speech frame */

	if(*ser_size == NBBITS_7k)
	{
		/* Find open loop pitch lag for whole speech frame */
		T_op = Pitch_med_ol(wsp, PIT_MIN / OPL_DECIM, PIT_MAX / OPL_DECIM,
			L_FRAME / OPL_DECIM, st->old_T0_med, &(st->ol_gain), st->hp_wsp_mem, st->old_hp_wsp, st->ol_wght_flg);
	} else
	{
		/* Find open loop pitch lag for first 1/2 frame */
		T_op = Pitch_med_ol(wsp, PIT_MIN / OPL_DECIM, PIT_MAX / OPL_DECIM,
			(L_FRAME / 2) / OPL_DECIM, st->old_T0_med, &(st->ol_gain), st->hp_wsp_mem, st->old_hp_wsp, st->ol_wght_flg);
	}

	if(st->ol_gain > 19661)       /* 0.6 in Q15 */
	{
		st->old_T0_med = Med_olag(T_op, st->old_ol_lag);       
		st->ada_w = 32767;                 
	} else
	{
		st->ada_w = vo_mult(st->ada_w, 29491);
	}

	if(st->ada_w < 26214)
		st->ol_wght_flg = 0;
	else
		st->ol_wght_flg = 1;

	wb_vad_tone_detection(&(st->vadSt), st->ol_gain);
	T_op *= OPL_DECIM;                     

	if(*ser_size != NBBITS_7k)
	{
		/* Find open loop pitch lag for second 1/2 frame */
		T_op2 = Pitch_med_ol(wsp + ((L_FRAME / 2) / OPL_DECIM), PIT_MIN / OPL_DECIM, PIT_MAX / OPL_DECIM,
			(L_FRAME / 2) / OPL_DECIM, st->old_T0_med, &(st->ol_gain), st->hp_wsp_mem, st->old_hp_wsp, st->ol_wght_flg);

		if(st->ol_gain > 19661)   /* 0.6 in Q15 */
		{
			st->old_T0_med = Med_olag(T_op2, st->old_ol_lag);  
			st->ada_w = 32767;             
		} else
		{
			st->ada_w = mult(st->ada_w, 29491); 
		}

		if(st->ada_w < 26214)
			st->ol_wght_flg = 0;
		else
			st->ol_wght_flg = 1;

		wb_vad_tone_detection(&(st->vadSt), st->ol_gain);

		T_op2 *= OPL_DECIM;                

	} else
	{
		T_op2 = T_op;                      
	}
	/*----------------------------------------------------------------------*
	*                              DTX-CNG                                 *
	*----------------------------------------------------------------------*/
	if(*mode == MRDTX)            /* CNG mode */
	{
		/* Buffer isf's and energy */
#ifdef ASM_OPT
		Residu_opt(&A[3 * (M + 1)], speech, exc, L_FRAME);
#else
		Residu(&A[3 * (M + 1)], speech, exc, L_FRAME);
#endif

		for (i = 0; i < L_FRAME; i++)
		{
			exc2[i] = shr(exc[i], Q_new);  
		}

		L_tmp = 0;                         
		for (i = 0; i < L_FRAME; i++)
			L_tmp += (exc2[i] * exc2[i])<<1;

		L_tmp >>= 1;

		dtx_buffer(&(st->dtx_encSt), isf, L_tmp, codec_mode);

		/* Quantize and code the ISFs */
		dtx_enc(&(st->dtx_encSt), isf, exc2, &prms);

		/* Convert ISFs to the cosine domain */
		Isf_isp(isf, ispnew_q, M);
		Isp_Az(ispnew_q, Aq, M, 0);

		for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
		{
			corr_gain = synthesis(Aq, &exc2[i_subfr], 0, &speech16k[i_subfr * 5 / 4], st, (Word16*)pSynthesis);
		}
		voAMRWBEnc_Copy(isf, st->isfold, M);

		/* reset speech coder memories */
		Reset_encoder(st, 0);

		/*--------------------------------------------------*
		* Update signal for next frame.                    *
		* -> save past of speech[] and wsp[].              *
		*--------------------------------------------------*/

		voAMRWBEnc_Copy(&old_speech[L_FRAME], st->old_speech, L_TOTAL - L_FRAME);
		voAMRWBEnc_Copy(&old_wsp[L_FRAME / OPL_DECIM], st->old_wsp, PIT_MAX / OPL_DECIM);

		return;
	}
	/*----------------------------------------------------------------------*
	*                               ACELP                                  *
	*----------------------------------------------------------------------*/

	/* Quantize and code the ISFs */

	if (*ser_size <= NBBITS_7k)
	{
		Qpisf_2s_36b(isf, isf, st->past_isfq, indice, 4);

		Parm_serial(indice[0], 8, &prms);
		Parm_serial(indice[1], 8, &prms);
		Parm_serial(indice[2], 7, &prms);
		Parm_serial(indice[3], 7, &prms);
		Parm_serial(indice[4], 6, &prms);
	} else
	{
		Qpisf_2s_46b(isf, isf, st->past_isfq, indice, 4);

		Parm_serial(indice[0], 8, &prms);
		Parm_serial(indice[1], 8, &prms);
		Parm_serial(indice[2], 6, &prms);
		Parm_serial(indice[3], 7, &prms);
		Parm_serial(indice[4], 7, &prms);
		Parm_serial(indice[5], 5, &prms);
		Parm_serial(indice[6], 5, &prms);
	}

	/* Check stability on isf : distance between old isf and current isf */

	L_tmp = 0;                           
	for (i = 0; i < M - 1; i++)
	{
		tmp = vo_sub(isf[i], st->isfold[i]);
		L_tmp += (tmp * tmp)<<1;
	}

	tmp = extract_h(L_shl2(L_tmp, 8));      /* saturation can occur here */

	tmp = vo_mult(tmp, 26214);                /* tmp = L_tmp*0.8/256 */
	tmp = vo_sub(20480, tmp);                 /* 1.25 - tmp (in Q14) */

	stab_fac = shl(tmp, 1);                /* saturation can occur here */

	if (stab_fac < 0)
	{
		stab_fac = 0;                      
	}
	voAMRWBEnc_Copy(isf, st->isfold, M);

	/* Convert ISFs to the cosine domain */
	Isf_isp(isf, ispnew_q, M);

	if (st->first_frame != 0)
	{
		st->first_frame = 0;              
		voAMRWBEnc_Copy(ispnew_q, st->ispold_q, M);
	}
	/* Find the interpolated ISPs and convert to a[] for all subframes */

	Int_isp(st->ispold_q, ispnew_q, interpol_frac, Aq);

	/* update ispold[] for the next frame */
	voAMRWBEnc_Copy(ispnew_q, st->ispold_q, M);

	p_Aq = Aq;
	for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
	{
#ifdef ASM_OPT
		Residu_opt(p_Aq, &speech[i_subfr], &exc[i_subfr], L_SUBFR);
#else
		Residu(p_Aq, &speech[i_subfr], &exc[i_subfr], L_SUBFR);
#endif
		p_Aq += (M + 1);                   
	}

	/* Buffer isf's and energy for dtx on non-speech frame */
	if (vad_flag == 0)
	{
		for (i = 0; i < L_FRAME; i++)
		{
			//exc2[i] = shr(exc[i], Q_new); 
			exc2[i] = exc[i] >> Q_new;
		}
		L_tmp = 0;                         
		for (i = 0; i < L_FRAME; i++)
			L_tmp += (exc2[i] * exc2[i])<<1;
		L_tmp >>= 1;

		dtx_buffer(&(st->dtx_encSt), isf, L_tmp, codec_mode);
	}
	/* range for closed loop pitch search in 1st subframe */

	T0_min = T_op - 8;
	if (T0_min < PIT_MIN)
	{
		T0_min = PIT_MIN;                  
	}
	T0_max = (T0_min + 15);

	if(T0_max > PIT_MAX)
	{
		T0_max = PIT_MAX;                  
		T0_min = T0_max - 15;          
	}
	/*------------------------------------------------------------------------*
	*          Loop for every subframe in the analysis frame                 *
	*------------------------------------------------------------------------*
	*  To find the pitch and innovation parameters. The subframe size is     *
	*  L_SUBFR and the loop is repeated L_FRAME/L_SUBFR times.               *
	*     - compute the target signal for pitch search                       *
	*     - compute impulse response of weighted synthesis filter (h1[])     *
	*     - find the closed-loop pitch parameters                            *
	*     - encode the pitch dealy                                           *
	*     - find 2 lt prediction (with / without LP filter for lt pred)      *
	*     - find 2 pitch gains and choose the best lt prediction.            *
	*     - find target vector for codebook search                           *
	*     - update the impulse response h1[] for codebook search             *
	*     - correlation between target vector and impulse response           *
	*     - codebook search and encoding                                     *
	*     - VQ of pitch and codebook gains                                   *
	*     - find voicing factor and tilt of code for next subframe.          *
	*     - update states of weighting filter                                *
	*     - find excitation and synthesis speech                             *
	*------------------------------------------------------------------------*/

	p_A = A;                               
	p_Aq = Aq;                             

	for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
	{
		pit_flag = i_subfr;                
		if ((i_subfr == 2 * L_SUBFR) && (*ser_size > NBBITS_7k))
		{
			pit_flag = 0;                 

			/* range for closed loop pitch search in 3rd subframe */

			T0_min = (T_op2 - 8);

			if (T0_min < PIT_MIN)
			{
				T0_min = PIT_MIN;          
			}
			T0_max = (T0_min + 15);
			if (T0_max > PIT_MAX)
			{
				T0_max = PIT_MAX;         
				T0_min = (T0_max - 15);
			}
		}
		/*-----------------------------------------------------------------------*
		*                                                                       *
		*        Find the target vector for pitch search:                       *
		*        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                        *
		*                                                                       *
		*             |------|  res[n]                                          *
		* speech[n]---| A(z) |--------                                          *
		*             |------|       |   |--------| error[n]  |------|          *
		*                   zero -- (-)--| 1/A(z) |-----------| W(z) |-- target *
		*                   exc          |--------|           |------|          *
		*                                                                       *
		* Instead of subtracting the zero-input response of filters from        *
		* the weighted input speech, the above configuration is used to         *
		* compute the target vector.                                            *
		*                                                                       *
		*-----------------------------------------------------------------------*/

		for (i = 0; i < M; i++)
		{
			error[i] = vo_sub(speech[i + i_subfr - M], st->mem_syn[i]);
		}

#ifdef ASM_OPT
		Residu_opt(p_Aq, &speech[i_subfr], &exc[i_subfr], L_SUBFR);
		Syn_filt_opt(p_Aq, &exc[i_subfr], L_SUBFR, error + M, error, (Word16 *)pSynFilt, 0);
#else
		Residu(p_Aq, &speech[i_subfr], &exc[i_subfr], L_SUBFR);
		Syn_filt(p_Aq, &exc[i_subfr], error + M, L_SUBFR, error, (Word16 *)pSynFilt, 0);
#endif
		voAMRWBEnc_Weight_a(p_A, Ap, GAMMA1, M);

#ifdef ASM_OPT
		Residu_opt(Ap, error + M, xn, L_SUBFR);
#else
		Residu(Ap, error + M, xn, L_SUBFR);
#endif

		Deemph2(xn, TILT_FAC, L_SUBFR, &(st->mem_w0));

		/*----------------------------------------------------------------------*
		* Find approx. target in residual domain "cn[]" for inovation search.  *
		*----------------------------------------------------------------------*/

		/* first half: xn[] --> cn[] */
		Set_zero(code, M);
		voAMRWBEnc_Copy(xn, code + M, L_SUBFR / 2);
		tmp = 0;                          
		Preemph2(code + M, TILT_FAC, L_SUBFR / 2, &tmp);
		voAMRWBEnc_Weight_a(p_A, Ap, GAMMA1, M);


#ifdef ASM_OPT
		Syn_filt_opt(Ap,code + M, L_SUBFR / 2, code + M, code, (Word16 *)pSynFilt,0);
		Residu_opt(p_Aq,code + M, cn, L_SUBFR / 2);
#else
		Syn_filt(Ap,code + M, code + M, L_SUBFR / 2, code, (Word16 *)pSynFilt,0);
		Residu(p_Aq,code + M, cn, L_SUBFR / 2);
#endif

		/* second half: res[] --> cn[] (approximated and faster) */
		voAMRWBEnc_Copy(&exc[i_subfr + (L_SUBFR / 2)], cn + (L_SUBFR / 2), L_SUBFR / 2);

		/*---------------------------------------------------------------*
		* Compute impulse response, h1[], of weighted synthesis filter  * 
		*---------------------------------------------------------------*/

		Set_zero(error, M + L_SUBFR);
		voAMRWBEnc_Weight_a(p_A, error + M, GAMMA1, M);

#if 1
		vo_p0 = error+M;
		vo_p3 = h1;
		for (i = 0; i < L_SUBFR; i++)
		{
			L_tmp = *vo_p0 << 14;        /* x4 (Q12 to Q14) */
			vo_p1 = p_Aq + 1;
			vo_p2 = vo_p0-1;
			for (j = 1; j <= M/4; j++)
			{
				L_tmp -= *vo_p1++ * *vo_p2--;
				L_tmp -= *vo_p1++ * *vo_p2--;
				L_tmp -= *vo_p1++ * *vo_p2--;
				L_tmp -= *vo_p1++ * *vo_p2--;
			}
			*vo_p3++ = *vo_p0++ = vo_round((L_tmp <<4));
		}
#else
		for (i = 0; i < L_SUBFR; i++)
		{
			L_tmp = error[i + M] << 15;        /* x4 (Q12 to Q14) */
			for (j = 1; j <= M; j++)
				L_tmp -= (p_Aq[j] * error[i + M - j])<<1;

			h1[i] = error[i + M] = extract_h(L_add(L_shl2(L_tmp, 3), 0x8000)); 
		}
#endif
		/* deemph without division by 2 -> Q14 to Q15 */
		tmp = 0; 
		Deemph2(h1, TILT_FAC, L_SUBFR, &tmp);   /* h1 in Q14 */

		/* h2 in Q12 for codebook search */
		voAMRWBEnc_Copy(h1, h2, L_SUBFR);

		/*---------------------------------------------------------------*
		* scale xn[] and h1[] to avoid overflow in dot_product12()      *
		*---------------------------------------------------------------*/
#ifdef  ASM_OPT
		Scale_sig_opt(h2, L_SUBFR, -2);
		Scale_sig_opt(xn, L_SUBFR, shift);     /* scaling of xn[] to limit dynamic at 12 bits */
		Scale_sig_opt(h1, L_SUBFR, 1 + shift);  /* set h1[] in Q15 with scaling for convolution */
#else
		Scale_sig(h2, L_SUBFR, -2);
		Scale_sig(xn, L_SUBFR, shift);     /* scaling of xn[] to limit dynamic at 12 bits */
		Scale_sig(h1, L_SUBFR, 1 + shift);  /* set h1[] in Q15 with scaling for convolution */
#endif
		/*----------------------------------------------------------------------*
		*                 Closed-loop fractional pitch search                  *
		*----------------------------------------------------------------------*/
		/* find closed loop fractional pitch  lag */

		if(*ser_size <= NBBITS_9k)
		{
			T0 = Pitch_fr4(&exc[i_subfr], xn, h1, T0_min, T0_max, &T0_frac,
				pit_flag, PIT_MIN, PIT_FR1_8b, L_SUBFR);

			/* encode pitch lag */

			if (pit_flag == 0)             /* if 1st/3rd subframe */
			{
				/*--------------------------------------------------------------*
				* The pitch range for the 1st/3rd subframe is encoded with     *
				* 8 bits and is divided as follows:                            *
				*   PIT_MIN to PIT_FR1-1  resolution 1/2 (frac = 0 or 2)       *
				*   PIT_FR1 to PIT_MAX    resolution 1   (frac = 0)            *
				*--------------------------------------------------------------*/

				if (T0 < PIT_FR1_8b)
				{
					//index = sub(add(shl(T0, 1), shr(T0_frac, 1)), (PIT_MIN * 2));
					index = ((T0 << 1) + (T0_frac >> 1) - (PIT_MIN<<1));
				} else
				{
					index = ((T0 - PIT_FR1_8b) + ((PIT_FR1_8b - PIT_MIN)*2));
				}

				Parm_serial(index, 8, &prms);

				/* find T0_min and T0_max for subframe 2 and 4 */

				T0_min = (T0 - 8);
				if (T0_min < PIT_MIN)
				{
					T0_min = PIT_MIN;
				}
				T0_max = T0_min + 15;
				if (T0_max > PIT_MAX)
				{
					T0_max = PIT_MAX;
					T0_min = (T0_max - 15);
				}
			} else
			{                              /* if subframe 2 or 4 */
				/*--------------------------------------------------------------*
				* The pitch range for subframe 2 or 4 is encoded with 5 bits:  *
				*   T0_min  to T0_max     resolution 1/2 (frac = 0 or 2)       *
				*--------------------------------------------------------------*/

				i = (T0 - T0_min);
				index = (i << 1) + (T0_frac >> 1);

				Parm_serial(index, 5, &prms);
			}
		} else
		{
			T0 = Pitch_fr4(&exc[i_subfr], xn, h1, T0_min, T0_max, &T0_frac,
				pit_flag, PIT_FR2, PIT_FR1_9b, L_SUBFR);

			/* encode pitch lag */

			if (pit_flag == 0)             /* if 1st/3rd subframe */
			{
				/*--------------------------------------------------------------*
				* The pitch range for the 1st/3rd subframe is encoded with     *
				* 9 bits and is divided as follows:                            *
				*   PIT_MIN to PIT_FR2-1  resolution 1/4 (frac = 0,1,2 or 3)   *
				*   PIT_FR2 to PIT_FR1-1  resolution 1/2 (frac = 0 or 1)       *
				*   PIT_FR1 to PIT_MAX    resolution 1   (frac = 0)            *
				*--------------------------------------------------------------*/

				if (T0 < PIT_FR2)
				{
					index = ((T0 << 2) + T0_frac) - (PIT_MIN << 2);
				} else if(T0 < PIT_FR1_9b)
				{
					index = ((((T0 << 1) + (T0_frac >> 1)) - (PIT_FR2<<1)) + ((PIT_FR2 - PIT_MIN)<<2));
				} else
				{
					index = (((T0 - PIT_FR1_9b) + ((PIT_FR2 - PIT_MIN)<<2)) + ((PIT_FR1_9b - PIT_FR2)<<1));
				}

				Parm_serial(index, 9, &prms);

				/* find T0_min and T0_max for subframe 2 and 4 */

				T0_min = (T0 - 8);
				if (T0_min < PIT_MIN)
				{
					T0_min = PIT_MIN; 
				}
				T0_max = T0_min + 15;

				if (T0_max > PIT_MAX)
				{
					T0_max = PIT_MAX;
					T0_min = (T0_max - 15);
				}
			} else
			{                              /* if subframe 2 or 4 */
				/*--------------------------------------------------------------*
				* The pitch range for subframe 2 or 4 is encoded with 6 bits:  *
				*   T0_min  to T0_max     resolution 1/4 (frac = 0,1,2 or 3)   *
				*--------------------------------------------------------------*/
				i = (T0 - T0_min);
				index = (i << 2) + T0_frac;
				Parm_serial(index, 6, &prms);
			}
		}

		/*-----------------------------------------------------------------*
		* Gain clipping test to avoid unstable synthesis on frame erasure *
		*-----------------------------------------------------------------*/

		clip_gain = 0;
		if((st->gp_clip[0] < 154) && (st->gp_clip[1] > 14746))
			clip_gain = 1;

		//clip_gain = Gp_clip(st->gp_clip);

		/*-----------------------------------------------------------------*
		* - find unity gain pitch excitation (adaptive codebook entry)    *
		*   with fractional interpolation.                                *
		* - find filtered pitch exc. y1[]=exc[] convolved with h1[])      *
		* - compute pitch gain1                                           *
		*-----------------------------------------------------------------*/

		/* find pitch exitation */
#ifdef ASM_OPT
        pred_lt4_asm(&exc[i_subfr], T0, T0_frac, L_SUBFR + 1);
#else
		Pred_lt4(&exc[i_subfr], T0, T0_frac, L_SUBFR + 1);
#endif
		if (*ser_size > NBBITS_9k)
		{
#ifdef ASM_OPT
			voAMRWBEncConvolve_asm(&exc[i_subfr], h1, y1, L_SUBFR);
#else
			Convolve(&exc[i_subfr], h1, y1, L_SUBFR);
#endif
			gain1 = G_pitch(xn, y1, g_coeff, L_SUBFR);

			/* clip gain if necessary to avoid problem at decoder */

			if ((clip_gain != 0) && (gain1 > GP_CLIP))
			{
				gain1 = GP_CLIP; 
			}
			/* find energy of new target xn2[] */
			Updt_tar(xn, dn, y1, gain1, L_SUBFR);       /* dn used temporary */
		} else
		{
			gain1 = 0; 
		}
		/*-----------------------------------------------------------------*
		* - find pitch excitation filtered by 1st order LP filter.        *
		* - find filtered pitch exc. y2[]=exc[] convolved with h1[])      *
		* - compute pitch gain2                                           *
		*-----------------------------------------------------------------*/
		/* find pitch excitation with lp filter */
#if 1
		vo_p0 = exc + i_subfr-1;
		vo_p1 = code;
		/* find pitch excitation with lp filter */
		for (i = 0; i < L_SUBFR/2; i++)
		{
			L_tmp = 5898 * *vo_p0++;
			L_tmp1 = 5898 * *vo_p0;
			L_tmp += 20972 * *vo_p0++;
			L_tmp1 += 20972 * *vo_p0++;
			L_tmp1 += 5898 * *vo_p0--;
			L_tmp += 5898 * *vo_p0;
			*vo_p1++ = (L_tmp + 0x4000)>>15;
			*vo_p1++ = (L_tmp1 + 0x4000)>>15;
		}
#else
		for (i = 0; i < L_SUBFR; i++)
		{
			L_tmp = L_mult(5898, exc[i - 1 + i_subfr]);
			L_tmp += (20972 * exc[i + i_subfr])<<1;
			L_tmp += (5898 * exc[i + 1 + i_subfr])<<1;
			code[i] = vo_round(L_tmp);
		}
#endif

#ifdef ASM_OPT
		voAMRWBEncConvolve_asm(code, h1, y2, L_SUBFR);
#else
		Convolve(code, h1, y2, L_SUBFR);
#endif
		gain2 = G_pitch(xn, y2, g_coeff2, L_SUBFR);

		/* clip gain if necessary to avoid problem at decoder */
		if ((clip_gain != 0) && (gain2 > GP_CLIP))
		{
			gain2 = GP_CLIP;
		}
		/* find energy of new target xn2[] */
		Updt_tar(xn, xn2, y2, gain2, L_SUBFR);
		/*-----------------------------------------------------------------*
		* use the best prediction (minimise quadratic error).             *
		*-----------------------------------------------------------------*/
		select = 0; 
		if(*ser_size > NBBITS_9k)
		{
			L_tmp = 0L;
			vo_p0 = dn;
			vo_p1 = xn2;
			for (i = 0; i < L_SUBFR/2; i++)
			{
				L_tmp += *vo_p0 * *vo_p0;
				vo_p0++;
				L_tmp -= *vo_p1 * *vo_p1;
				vo_p1++;
				L_tmp += *vo_p0 * *vo_p0;
				vo_p0++;
				L_tmp -= *vo_p1 * *vo_p1;
				vo_p1++;
			}

			if (L_tmp <= 0)
			{
				select = 1; 
			}
			Parm_serial(select, 1, &prms);
		}
		if (select == 0)
		{
			/* use the lp filter for pitch excitation prediction */
			gain_pit = gain2;
			voAMRWBEnc_Copy(code, &exc[i_subfr], L_SUBFR);
			voAMRWBEnc_Copy(y2, y1, L_SUBFR);
			voAMRWBEnc_Copy(g_coeff2, g_coeff, 4);
		} else
		{
			/* no filter used for pitch excitation prediction */
			gain_pit = gain1;
			voAMRWBEnc_Copy(dn, xn2, L_SUBFR);        /* target vector for codebook search */
		}
		/*-----------------------------------------------------------------*
		* - update cn[] for codebook search                               *
		*-----------------------------------------------------------------*/
		Updt_tar(cn, cn, &exc[i_subfr], gain_pit, L_SUBFR);

#ifdef  ASM_OPT
		Scale_sig_opt(cn, L_SUBFR, shift);     /* scaling of cn[] to limit dynamic at 12 bits */
#else
		Scale_sig(cn, L_SUBFR, shift);     /* scaling of cn[] to limit dynamic at 12 bits */
#endif
		/*-----------------------------------------------------------------*
		* - include fixed-gain pitch contribution into impulse resp. h1[] *
		*-----------------------------------------------------------------*/
		tmp = 0;
		Preemph(h2, st->tilt_code, L_SUBFR, &tmp);

		if (T0_frac > 2)
			T0 = (T0 + 1);
		Pit_shrp(h2, T0, PIT_SHARP, L_SUBFR);
		/*-----------------------------------------------------------------*
		* - Correlation between target xn2[] and impulse response h1[]    *
		* - Innovative codebook search                                    *
		*-----------------------------------------------------------------*/
		voAMRWBEnccor_h_x(h2, xn2, dn);
		if (*ser_size <= NBBITS_7k)
		{
			ACELP_2t64_fx(dn, cn, h2, code, y2, indice);

			Parm_serial(indice[0], 12, &prms);
		} else if(*ser_size <= NBBITS_9k)
		{
			ACELP_4t64_fx(dn, cn, h2, code, y2, 20, *ser_size, indice);

			Parm_serial(indice[0], 5, &prms);
			Parm_serial(indice[1], 5, &prms);
			Parm_serial(indice[2], 5, &prms);
			Parm_serial(indice[3], 5, &prms);
		} else if(*ser_size <= NBBITS_12k)
		{
			ACELP_4t64_fx(dn, cn, h2, code, y2, 36, *ser_size, indice);

			Parm_serial(indice[0], 9, &prms);
			Parm_serial(indice[1], 9, &prms);
			Parm_serial(indice[2], 9, &prms);
			Parm_serial(indice[3], 9, &prms);
		} else if(*ser_size <= NBBITS_14k)
		{
			ACELP_4t64_fx(dn, cn, h2, code, y2, 44, *ser_size, indice);

			Parm_serial(indice[0], 13, &prms);
			Parm_serial(indice[1], 13, &prms);
			Parm_serial(indice[2], 9, &prms);
			Parm_serial(indice[3], 9, &prms);
		} else if(*ser_size <= NBBITS_16k)
		{
			ACELP_4t64_fx(dn, cn, h2, code, y2, 52, *ser_size, indice);

			Parm_serial(indice[0], 13, &prms);
			Parm_serial(indice[1], 13, &prms);
			Parm_serial(indice[2], 13, &prms);
			Parm_serial(indice[3], 13, &prms);
		} else if(*ser_size <= NBBITS_18k)
		{
			ACELP_4t64_fx(dn, cn, h2, code, y2, 64, *ser_size, indice);

			Parm_serial(indice[0], 2, &prms);
			Parm_serial(indice[1], 2, &prms);
			Parm_serial(indice[2], 2, &prms);
			Parm_serial(indice[3], 2, &prms);
			Parm_serial(indice[4], 14, &prms);
			Parm_serial(indice[5], 14, &prms);
			Parm_serial(indice[6], 14, &prms);
			Parm_serial(indice[7], 14, &prms);
		} else if(*ser_size <= NBBITS_20k)
		{
			ACELP_4t64_fx(dn, cn, h2, code, y2, 72, *ser_size, indice);

			Parm_serial(indice[0], 10, &prms);
			Parm_serial(indice[1], 10, &prms);
			Parm_serial(indice[2], 2, &prms);
			Parm_serial(indice[3], 2, &prms);
			Parm_serial(indice[4], 10, &prms);
			Parm_serial(indice[5], 10, &prms);
			Parm_serial(indice[6], 14, &prms);
			Parm_serial(indice[7], 14, &prms);
		} else
		{
			ACELP_4t64_fx(dn, cn, h2, code, y2, 88, *ser_size, indice);

			Parm_serial(indice[0], 11, &prms);
			Parm_serial(indice[1], 11, &prms);
			Parm_serial(indice[2], 11, &prms);
			Parm_serial(indice[3], 11, &prms);
			Parm_serial(indice[4], 11, &prms);
			Parm_serial(indice[5], 11, &prms);
			Parm_serial(indice[6], 11, &prms);
			Parm_serial(indice[7], 11, &prms);
		}
		/*-------------------------------------------------------*
		* - Add the fixed-gain pitch contribution to code[].    *
		*-------------------------------------------------------*/
		tmp = 0; 
		Preemph(code, st->tilt_code, L_SUBFR, &tmp);
		Pit_shrp(code, T0, PIT_SHARP, L_SUBFR);
		/*----------------------------------------------------------*
		*  - Compute the fixed codebook gain                       *
		*  - quantize fixed codebook gain                          *
		*----------------------------------------------------------*/
		if(*ser_size <= NBBITS_9k)
		{
			index = Q_gain2(xn, y1, Q_new + shift, y2, code, g_coeff, L_SUBFR, 6,
				&gain_pit, &L_gain_code, clip_gain, st->qua_gain);
			Parm_serial(index, 6, &prms);
		} else
		{
			index = Q_gain2(xn, y1, Q_new + shift, y2, code, g_coeff, L_SUBFR, 7,
				&gain_pit, &L_gain_code, clip_gain, st->qua_gain);
			Parm_serial(index, 7, &prms);
		}
		/* test quantized gain of pitch for pitch clipping algorithm */
		Gp_clip_test_gain_pit(gain_pit, st->gp_clip);

		L_tmp = L_shl(L_gain_code, Q_new); /* saturation can occur here */
		gain_code = extract_h(L_add(L_tmp, 0x8000));          /* scaled gain_code with Qnew */

		/*----------------------------------------------------------*
		* Update parameters for the next subframe.                 *
		* - tilt of code: 0.0 (unvoiced) to 0.5 (voiced)           *
		*----------------------------------------------------------*/
		/* find voice factor in Q15 (1=voiced, -1=unvoiced) */
		voAMRWBEnc_Copy(&exc[i_subfr], exc2, L_SUBFR);
#ifdef ASM_OPT
		Scale_sig_opt(exc2, L_SUBFR, shift);
#else
		Scale_sig(exc2, L_SUBFR, shift);
#endif
		voice_fac = voice_factor(exc2, shift, gain_pit, code, gain_code, L_SUBFR);
		/* tilt of code for next subframe: 0.5=voiced, 0=unvoiced */
		st->tilt_code = ((voice_fac >> 2) + 8192);
		/*------------------------------------------------------*
		* - Update filter's memory "mem_w0" for finding the    *
		*   target vector in the next subframe.                *
		* - Find the total excitation                          *
		* - Find synthesis speech to update mem_syn[].         *
		*------------------------------------------------------*/

		/* y2 in Q9, gain_pit in Q14 */
		L_tmp = (gain_code * y2[L_SUBFR - 1])<<1;
		L_tmp = L_shl(L_tmp, (5 + shift));
		L_tmp = L_negate(L_tmp);
		L_tmp += (xn[L_SUBFR - 1] * 16384)<<1;
		L_tmp -= (y1[L_SUBFR - 1] * gain_pit)<<1;
		L_tmp = L_shl(L_tmp, (1 - shift));
		st->mem_w0 = extract_h(L_add(L_tmp, 0x8000));

		if (*ser_size >= NBBITS_24k)
			voAMRWBEnc_Copy(&exc[i_subfr], exc2, L_SUBFR);

		for (i = 0; i < L_SUBFR; i++)
		{
			/* code in Q9, gain_pit in Q14 */
			L_tmp = (gain_code * code[i])<<1;
			L_tmp = (L_tmp << 5);
			L_tmp += (exc[i + i_subfr] * gain_pit)<<1;
			L_tmp = L_shl2(L_tmp, 1);       /* saturation can occur here */
			exc[i + i_subfr] = extract_h(L_add(L_tmp, 0x8000));
		}
#ifdef ASM_OPT
		Syn_filt_opt(p_Aq,&exc[i_subfr], L_SUBFR, synth,  st->mem_syn, (Word16 *)pSynFilt,1);
#else
		Syn_filt(p_Aq,&exc[i_subfr], synth, L_SUBFR, st->mem_syn, (Word16 *)pSynFilt,1);
#endif

		if(*ser_size >= NBBITS_24k)
		{
			/*------------------------------------------------------------*
			* phase dispersion to enhance noise in low bit rate          *
			*------------------------------------------------------------*/

			/* L_gain_code in Q16 */
			gain_code = L_gain_code >> 16;
			gain_code_lo = (L_gain_code & 0xffff) >> 1;

			//L_Extract(L_gain_code, &gain_code, &gain_code_lo);

			/*------------------------------------------------------------*
			* noise enhancer                                             *
			* ~~~~~~~~~~~~~~                                             *
			* - Enhance excitation on noise. (modify gain of code)       *
			*   If signal is noisy and LPC filter is stable, move gain   *
			*   of code 1.5 dB toward gain of code threshold.            *
			*   This decrease by 3 dB noise energy variation.            *
			*------------------------------------------------------------*/

			tmp = (16384 - (voice_fac >> 1));        /* 1=unvoiced, 0=voiced */
			fac = vo_mult(stab_fac, tmp);
			L_tmp = L_gain_code; 
			if(L_tmp < st->L_gc_thres)
			{
				L_tmp = vo_L_add(L_tmp, voMpy_32_16(gain_code, gain_code_lo, 6226));
				if(L_tmp > st->L_gc_thres)
				{
					L_tmp = st->L_gc_thres;
				}
			} else
			{
				L_tmp = voMpy_32_16(gain_code, gain_code_lo, 27536);
				if(L_tmp < st->L_gc_thres)
				{
					L_tmp = st->L_gc_thres;
				}
			}
			st->L_gc_thres = L_tmp;

			L_gain_code = voMpy_32_16(gain_code, gain_code_lo, (32767 - fac));

			gain_code = L_tmp >> 16;
			gain_code_lo = (L_tmp & 0xffff)>>1;

			//L_Extract(L_tmp, &gain_code, &gain_code_lo);
			L_gain_code = vo_L_add(L_gain_code, voMpy_32_16(gain_code, gain_code_lo, fac));

			/*------------------------------------------------------------*
			* pitch enhancer                                             *
			* ~~~~~~~~~~~~~~                                             *
			* - Enhance excitation on voice. (HP filtering of code)      *
			*   On voiced signal, filtering of code by a smooth fir HP   *
			*   filter to decrease energy of code in low frequency.      *
			*------------------------------------------------------------*/

			tmp = ((voice_fac >> 3) + 4096); /* 0.25=voiced, 0=unvoiced */

			L_tmp = L_deposit_h(code[0]);
			L_tmp -= (code[1] * tmp)<<1;
			code2[0] = vo_round(L_tmp); 

			for (i = 1; i < L_SUBFR - 1; i++)
			{
				L_tmp = L_deposit_h(code[i]);
				L_tmp -= (code[i + 1] * tmp)<<1;
				L_tmp -= (code[i - 1] * tmp)<<1;
				code2[i] = vo_round(L_tmp); 
			}

			L_tmp = L_deposit_h(code[L_SUBFR - 1]);
			L_tmp -= (code[L_SUBFR - 2] * tmp)<<1;
			code2[L_SUBFR - 1] = vo_round(L_tmp); 

			/* build excitation */

			gain_code = vo_round(L_shl(L_gain_code, Q_new));

			for (i = 0; i < L_SUBFR; i++)
			{
				L_tmp = (code2[i] * gain_code)<<1;
				L_tmp = (L_tmp << 5);
				L_tmp += (exc2[i] * gain_pit)<<1;
				L_tmp = (L_tmp << 1);   /* saturation can occur here */
				exc2[i] = vo_round(L_tmp);
			}

			corr_gain = synthesis(p_Aq, exc2, Q_new, &speech16k[i_subfr * 5 / 4], st, (Word16*)pSynthesis);
			Parm_serial(corr_gain, 4, &prms);
		}
		p_A += (M + 1);
		p_Aq += (M + 1);
	}                                      /* end of subframe loop */
	/*--------------------------------------------------*
	* Update signal for next frame.                    *
	* -> save past of speech[], wsp[] and exc[].       *
	*--------------------------------------------------*/
	voAMRWBEnc_Copy(&old_speech[L_FRAME], st->old_speech, L_TOTAL - L_FRAME);
	voAMRWBEnc_Copy(&old_wsp[L_FRAME / OPL_DECIM], st->old_wsp, PIT_MAX / OPL_DECIM);
	voAMRWBEnc_Copy(&old_exc[L_FRAME], st->old_exc, PIT_MAX + L_INTERPOL);
	return;
}

/*-----------------------------------------------------*
* Function synthesis()                                *
*                                                     *
* Synthesis of signal at 16kHz with HF extension.     *
*                                                     *
*-----------------------------------------------------*/
#define	extra_store				(psyn)
#define	synth_hi				(extra_store+2)
#define synth_lo                (synth_hi + M + L_SUBFR)
#define synth1                  (synth_lo + M + L_SUBFR)
#define HF                      (synth1 + L_SUBFR)
#define Ap_temp                 (HF + L_SUBFR16k)
#define HF_SP                   (Ap_temp + M + 1)
#define pFilter                 (HF_SP + L_SUBFR16k)


static Word16 synthesis(
						Word16 Aq1[],                          /* A(z)  : quantized Az               */
						Word16 exc[],                         /* (i)   : excitation at 12kHz        */
						Word16 Q_new,                         /* (i)   : scaling performed on exc   */
						Word16 synth16k[],                    /* (o)   : 16kHz synthesis signal     */
						Coder_State * st,                      /* (i/o) : State structure            */
						Word16 *psyn
						)
{
	Word16 fac, tmp, exp;
	Word16 ener, exp_ener;
	Word32 L_tmp, i;
	Word16 HP_est_gain, HP_calc_gain, HP_corr_gain;
	Word16 dist_min, dist;
	Word16 HP_gain_ind = 0;
	Word16 gain1, gain2;
	Word16 weight1, weight2;

	/*------------------------------------------------------------*
	* speech synthesis                                           *
	* ~~~~~~~~~~~~~~~~                                           *
	* - Find synthesis speech corresponding to exc2[].           *
	* - Perform fixed deemphasis and hp 50hz filtering.          *
	* - Oversampling from 12.8kHz to 16kHz.                      *
	*------------------------------------------------------------*/
	voAMRWBEnc_Copy(st->mem_syn_hi, synth_hi, M);
	voAMRWBEnc_Copy(st->mem_syn_lo, synth_lo, M);

#ifdef ASM_OPT
    voAMRWBEncSyn_filt_32_asm(Ap_temp, M, exc, Q_new, synth_hi + M, synth_lo + M, L_SUBFR);
#else
	Syn_filt_32(Ap_temp, M, exc, Q_new, synth_hi + M, synth_lo + M, L_SUBFR);
#endif

	voAMRWBEnc_Copy(synth_hi + L_SUBFR, st->mem_syn_hi, M);
	voAMRWBEnc_Copy(synth_lo + L_SUBFR, st->mem_syn_lo, M);

#ifdef ASM_OPT
	Deemph_32_asm(synth_hi + M, synth_lo + M, synth1, &(st->mem_deemph));
#else
	Deemph_32(synth_hi + M, synth_lo + M, synth1, PREEMPH_FAC, L_SUBFR, &(st->mem_deemph));
#endif

	HP50_12k8(synth1, L_SUBFR, st->mem_sig_out);

	/* Original speech signal as reference for high band gain quantisation */
	for (i = 0; i < L_SUBFR16k; i++)
	{
		HF_SP[i] = synth16k[i]; 
	}

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
		HF[i] = Random(&(st->seed2))>>3;
		//HF[i] = shr(Random(&(st->seed2)), 3); 
	}

	/* energy of excitation */
#ifdef ASM_OPT
	Scale_sig_opt(exc, L_SUBFR, -3);
	Q_new = Q_new - 3;
	ener = extract_h(voAMRWBEncDot_product12_asm(exc, exc, L_SUBFR, &exp_ener));
#else
	Scale_sig(exc, L_SUBFR, -3);
	Q_new = Q_new - 3;
	ener = extract_h(Dot_product12(exc, exc, L_SUBFR, &exp_ener));
#endif

	exp_ener = exp_ener - (Q_new + Q_new);

	/* set energy of white noise to energy of excitation */
#ifdef ASM_OPT
	tmp = extract_h(voAMRWBEncDot_product12_asm(HF, HF, L_SUBFR16k, &exp));
#else
	tmp = extract_h(Dot_product12(HF, HF, L_SUBFR16k, &exp));
#endif

	if(tmp > ener)
	{
		tmp = (tmp >> 1);                 /* Be sure tmp < ener */
		exp = (exp + 1);
	}
	L_tmp = L_deposit_h(div_s(tmp, ener)); /* result is normalized */
	exp = (exp - exp_ener);
	Isqrt_n(&L_tmp, &exp);
	L_tmp = L_shl(L_tmp, (exp + 1));     /* L_tmp x 2, L_tmp in Q31 */
	tmp = extract_h(L_tmp);                /* tmp = 2 x sqrt(ener_exc/ener_hf) */

	for (i = 0; i < L_SUBFR16k; i++)
	{
		HF[i] = vo_mult(HF[i], tmp);
	}

	/* find tilt of synthesis speech (tilt: 1=voiced, -1=unvoiced) */

	HP400_12k8(synth1, L_SUBFR, st->mem_hp400);

	L_tmp = 1L;
	for (i = 0; i < L_SUBFR; i++)
		L_tmp += (synth1[i] * synth1[i])<<1;
	//L_tmp = L_mac(L_tmp, synth[i], synth[i]);

	exp = norm_l(L_tmp);
	ener = extract_h(L_tmp << exp);   /* ener = r[0] */

	L_tmp = 1L;
	for (i = 1; i < L_SUBFR; i++)
		L_tmp +=(synth1[i] * synth1[i - 1])<<1;
	//L_tmp = L_mac(L_tmp, synth[i], synth[i - 1]);

	tmp = extract_h(L_tmp << exp);    /* tmp = r[1] */

	if (tmp > 0)
	{
		fac = div_s(tmp, ener);
	} else
	{
		fac = 0; 
	}


	/* modify energy of white noise according to synthesis tilt */
	gain1 = 32767 - fac;
	gain2 = vo_mult(gain1, 20480);
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
	tmp = vo_mult(weight1, gain1);
	tmp = add1(tmp, vo_mult(weight2, gain2));

	if (tmp != 0)
	{
		tmp = (tmp + 1);
	}
	HP_est_gain = tmp;

	if(HP_est_gain < 3277)
	{
		HP_est_gain = 3277;                /* 0.1 in Q15 */
	}
	/* synthesis of noise: 4.8kHz..5.6kHz --> 6kHz..7kHz */
	voAMRWBEnc_Weight_a(Aq1, Ap_temp, 19661, M);            /* fac=0.6 */

#ifdef ASM_OPT
	voAMRWBEncSyn_filt_asm(Ap_temp, HF, HF, st->mem_syn_hf);
	/* noise High Pass filtering (1ms of delay) */
	voAMRWBEncFilt_6k_7k_asm(HF, L_SUBFR16k, st->mem_hf);
	/* filtering of the original signal */
	voAMRWBEncFilt_6k_7k_asm(HF_SP, L_SUBFR16k, st->mem_hf2);

	/* check the gain difference */
	Scale_sig_opt(HF_SP, L_SUBFR16k, -1);
	ener = extract_h(voAMRWBEncDot_product12_asm(HF_SP, HF_SP, L_SUBFR16k, &exp_ener));
	/* set energy of white noise to energy of excitation */
	tmp = extract_h(voAMRWBEncDot_product12_asm(HF, HF, L_SUBFR16k, &exp));
#else
	Syn_filt(Ap_temp, HF, HF, L_SUBFR16k, st->mem_syn_hf, (Word16 *)pFilter,1);
	/* noise High Pass filtering (1ms of delay) */
	Filt_6k_7k(HF, L_SUBFR16k, st->mem_hf);
	/* filtering of the original signal */
	Filt_6k_7k(HF_SP, L_SUBFR16k, st->mem_hf2);
	/* check the gain difference */
	Scale_sig(HF_SP, L_SUBFR16k, -1);
	ener = extract_h(Dot_product12(HF_SP, HF_SP, L_SUBFR16k, &exp_ener));
	/* set energy of white noise to energy of excitation */
	tmp = extract_h(Dot_product12(HF, HF, L_SUBFR16k, &exp));
#endif

	if (tmp > ener)
	{
		tmp = (tmp >> 1);                 /* Be sure tmp < ener */
		exp = (exp + 1);
	}
	L_tmp = L_deposit_h(div_s(tmp, ener)); /* result is normalized */
	exp = vo_sub(exp, exp_ener);
	Isqrt_n(&L_tmp, &exp);
	L_tmp = L_shl(L_tmp, exp);             /* L_tmp, L_tmp in Q31 */
	HP_calc_gain = extract_h(L_tmp);       /* tmp = sqrt(ener_input/ener_hf) */

	/* st->gain_alpha *= st->dtx_encSt->dtxHangoverCount/7 */
	L_tmp = (vo_L_mult(st->dtx_encSt.dtxHangoverCount, 4681) << 15);
	st->gain_alpha = vo_mult(st->gain_alpha, extract_h(L_tmp));

	if(st->dtx_encSt.dtxHangoverCount > 6)
		st->gain_alpha = 32767;
	HP_est_gain = HP_est_gain >> 1;     /* From Q15 to Q14 */
	HP_corr_gain = add1(vo_mult(HP_calc_gain, st->gain_alpha), vo_mult((32767 - st->gain_alpha), HP_est_gain));

	/* Quantise the correction gain */
	dist_min = 32767;
	for (i = 0; i < 16; i++)
	{
		dist = vo_mult((HP_corr_gain - HP_gain[i]), (HP_corr_gain - HP_gain[i]));
		if (dist_min > dist)
		{
			dist_min = dist;
			HP_gain_ind = i;
		}
	}
	HP_corr_gain = HP_gain[HP_gain_ind];
	/* return the quantised gain index when using the highest mode, otherwise zero */
	return (HP_gain_ind);
}

int AMRWB_Encode(HAMRENC hCodec)
{
	Word32 i;
	Coder_State *gData = (Coder_State*)hCodec;
	Word16 *signal;
	Word16 packed_size = 0;
	Word16 prms[NB_BITS_MAX];
	Word16 coding_mode = 0, nb_bits, allow_dtx, mode, reset_flag;
	mode = gData->mode;
	coding_mode = gData->mode;
	nb_bits = nb_of_bits[mode];
	signal = (Word16 *)gData->inputStream;
	allow_dtx = gData->allow_dtx;

	/* check for homing frame */
	reset_flag = encoder_homing_frame_test(signal);

	for (i = 0; i < L_FRAME16k; i++)   /* Delete the 2 LSBs (14-bit input) */
	{
		*(signal + i) = (Word16) (*(signal + i) & 0xfffC);
	}

	coder(&coding_mode, signal, prms, &nb_bits, gData, allow_dtx);
	packed_size = PackBits(prms, coding_mode, mode, gData);
	if (reset_flag != 0)
	{
		Reset_encoder(gData, 1);
	}
	return packed_size;
}

void *g_hAMRWBEncInst = NULL;

VO_U32 VO_API voAMRWBEnc_Init(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	Coder_State *st;
	//VO_MEM_OPERATOR voMemoprator;
	VO_MEM_OPERATOR *pMemOP;
  //int interMem = 0;

	if(pUserData == NULL || (pUserData->memflag & 0xF0) != VO_IMF_USERMEMOPERATOR || pUserData->memData == NULL )
	{
    return VO_ERR_INVALID_ARG;
	/*	voMemoprator.Alloc = cmnMemAlloc;
		voMemoprator.Copy = cmnMemCopy;
		voMemoprator.Free = cmnMemFree;
		voMemoprator.Set = cmnMemSet;
		voMemoprator.Check = cmnMemCheck;
		interMem = 1;
		pMemOP = &voMemoprator;*/
	}
	else
	{
		pMemOP = (VO_MEM_OPERATOR *)pUserData->memData;
	} 
	/*-------------------------------------------------------------------------*
	* Memory allocation for coder state.                                      *
	*-------------------------------------------------------------------------*/
	if ((st = (Coder_State *)voAMRWBEncMem_malloc(pMemOP, sizeof(Coder_State), 32)) == NULL)
	{
		return VO_ERR_OUTOF_MEMORY;
	}

	if ((st->pScratch = (Word32*)voAMRWBEncMem_malloc(pMemOP, VOAMRWBENC_SCRATCHMEM, 32)) == NULL)
	{
		return VO_ERR_OUTOF_MEMORY;
	}

	memset(st->pScratch, 0, VOAMRWBENC_SCRATCHMEM);
                 
	st->sid_update_counter = 3;
	st->sid_handover_debt = 0;
	st->prev_ft = TX_SPEECH;
	st->inputStream = NULL;
	st->inputSize = 0;

	/* Default setting */
	st->mode = VOAMRWB_MD2385;                        /* bit rate 23.85kbps */
	st->frameType = VOAMRWB_RFC3267;                  /* frame type: RFC3267 */
	st->allow_dtx = 0;                                /* disable DTX mode */

	st->outputStream = NULL;
	st->outputSize = 0;
    st->nSetLens = 0;

    wb_vad_reset(&(st->vadSt));
    dtx_enc_reset(&(st->dtx_encSt), isf_init);

	Reset_encoder((void *) st, 1);

	/*if(interMem)
	{
		st->voMemoprator.Alloc = cmnMemAlloc;
		st->voMemoprator.Copy = cmnMemCopy;
		st->voMemoprator.Free = cmnMemFree;
		st->voMemoprator.Set = cmnMemSet;
		st->voMemoprator.Check = cmnMemCheck;
		pMemOP = &st->voMemoprator;
	}*/

	st->pvoMemop = pMemOP;

	*phCodec = (void *) st;

#ifdef LCHECK
	if((pUserData->memflag & 0xF0) == 0x10  && pUserData->libOperator != NULL)
	{
	    voCheckLibInit(&(st->hCheck), VO_INDEX_ENC_AMRWB, 1, g_hAMRWBEncInst, pUserData->libOperator);
	}
	else
	{
		voCheckLibInit(&(st->hCheck), VO_INDEX_ENC_AMRWB, 1, g_hAMRWBEncInst, NULL);
	}
#endif

	return VO_ERR_NONE;
}

VO_U32 VO_API voAMRWBEnc_SetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	Coder_State  *gData;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	gData = (Coder_State *)hCodec;
	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
	{
		return VO_ERR_INVALID_ARG;
	}

	gData->inputStream = pInput->Buffer;
	gData->nSetLens    = pInput->Length;


	return VO_ERR_NONE;
}

VO_U32 VO_API voAMRWBEnc_GetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat)
{
	Coder_State* gData = (Coder_State*)hCodec;

	if (gData->nSetLens < 640)
	{
		return VO_ERR_INPUT_BUFFER_SMALL;
	}

	gData->outputStream = (unsigned short*)pOutput->Buffer;

	gData->outputSize = AMRWB_Encode(gData);

	pOutput->Length = gData->outputSize;
	gData->nSetLens -= 640;


#ifdef LCHECK
	voCheckLibCheckAudio(gData->hCheck, pOutput, &(pAudioFormat->Format));
#endif

	if(pAudioFormat)
	{
		pAudioFormat->Format.Channels = 1;
		pAudioFormat->Format.SampleRate = 8000;
		pAudioFormat->Format.SampleBits = 16;	
		pAudioFormat->InputUsed = 640; //stream->used_len;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voAMRWBEnc_SetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	Coder_State* gData = (Coder_State*)hCodec;
	int *lValue = (int*)pData;

	switch(uParamID)
	{
	case VO_PID_AMRWB_FRAMETYPE:
		if(*lValue < VOAMRWB_DEFAULT || *lValue > VOAMRWB_RFC3267)
			return VO_ERR_WRONG_PARAM_ID; 
		gData->frameType = *lValue;
		break;
	case VO_PID_AMRWB_MODE:
		{
			if(*lValue < VOAMRWB_MD66 || *lValue > VOAMRWB_MD2385)
				return VO_ERR_WRONG_PARAM_ID; 
			gData->mode = *lValue;
		}
		break;
	case VO_PID_AMRWB_DTX:
		gData->allow_dtx = (Word16)(*lValue);
		break;

	case VO_PID_COMMON_HEADDATA:
		break;
	case VO_PID_COMMON_FLUSH:
		break;

	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voAMRWBEnc_GetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	int    temp;
	Coder_State* gData = (Coder_State*)hCodec;

	if (gData==NULL) 
		return VO_ERR_INVALID_ARG;
	switch(uParamID)
	{
	case VO_PID_AMRWB_FORMAT:
		{
			VO_AUDIO_FORMAT* fmt = (VO_AUDIO_FORMAT*)pData;
			fmt->Channels   = 1;
			fmt->SampleRate = 16000;
			fmt->SampleBits = 16;
			break;
		}

	case VO_PID_AMRWB_CHANNELS:
			temp = 1;
			pData = (void *)(&temp);
			break;
	case VO_PID_AMRWB_SAMPLERATE:
			temp = 16000;
			pData = (void *)(&temp);
			break;
	case VO_PID_AMRWB_FRAMETYPE:
			temp = gData->frameType;
			pData = (void *)(&temp);
			break;
	case VO_PID_AMRWB_MODE:
			temp = gData->mode;
			pData = (void *)(&temp);
			break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voAMRWBEnc_Uninit(VO_HANDLE hCodec)
{
	Coder_State* gData = (Coder_State*)hCodec;
	VO_MEM_OPERATOR *pMemOP;
	pMemOP = gData->pvoMemop;

	if(hCodec)
	{
		voAMRWBEncMem_free(pMemOP,gData->pScratch);
		voAMRWBEncMem_free(pMemOP, hCodec);
		hCodec = NULL;
	}
#ifdef LCHECK
	voCheckLibUninit(gData->hCheck);
#endif
	return VO_ERR_NONE;
}

VO_S32 VO_API voGetAMRWBEncAPI(VO_AUDIO_CODECAPI * pEncHandle)
{
	if(NULL == pEncHandle)
		return VO_ERR_INVALID_ARG;
	pEncHandle->Init = voAMRWBEnc_Init;
	pEncHandle->SetInputData = voAMRWBEnc_SetInputData;
	pEncHandle->GetOutputData = voAMRWBEnc_GetOutputData;
	pEncHandle->SetParam = voAMRWBEnc_SetParam;
	pEncHandle->GetParam = voAMRWBEnc_GetParam;
	pEncHandle->Uninit = voAMRWBEnc_Uninit;

	return VO_ERR_NONE;
}

#ifdef __cplusplus
}
#endif
