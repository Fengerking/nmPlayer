#include "typedef.h"
#include "basic_op.h"
#include "count.h"

#include "amr_plus_fx.h"


void Init_decoder_amrwb_plus(
							 Decoder_State_Plus_fx *st,      /* (i/o) :  decoder memory state      */
							 Word16 num_chan,                /* (i)   :  Number of channel         */
							 Word16 fscale,                  /* (i)   ;  Frequency scaling         */
							 Word16 full_reset               /* (i)   : Do entire reset au decoder */
							 )
{

	/* initialize memories (stereo part) */

	st->left.wover_frac = 0;    
	st->right.wover_frac = 0;   

	if (fscale == 0) 
	{
		st->left.Mean_isf_hf = Mean_isf_hf_low_rate;   
		st->left.Dico1_isf_hf =Dico1_isf_hf_low_rate;  
		st->right.Mean_isf_hf = Mean_isf_hf_low_rate;  
		st->right.Dico1_isf_hf =Dico1_isf_hf_low_rate; 
	}
	else 
	{ 
		st->left.Mean_isf_hf = Mean_isf_hf_12k8;  
		st->left.Dico1_isf_hf =Dico1_isf_hf_12k8; 
		st->right.Mean_isf_hf = Mean_isf_hf_12k8; 
		st->right.Dico1_isf_hf =Dico1_isf_hf_12k8;
	}
	if (full_reset>0) 
	{
		Init_bass_postfilter(st);
		/* init filters memories */  
		Set_zero(st->left.wmem_oversamp, L_MEM_JOIN_OVER);
		Set_zero(st->right.wmem_oversamp, L_MEM_JOIN_OVER);
		Set_zero(st->left.wmem_oversamp_hf, 2*L_FILT);
		Set_zero(st->right.wmem_oversamp_hf, 2*L_FILT);

		Set_zero(st->left.wold_synth_hf, (D_BPF +L_SUBFR+ L_BSP + 2*D_NC + L_FDEL + 32*D_STEREO_TCX/5));
		Set_zero(st->right.wold_synth_hf, (D_BPF +L_SUBFR+ L_BSP + 2*D_NC + L_FDEL + 32*D_STEREO_TCX/5));

		Init_decoder_hf(&(st->left));
		Init_decoder_hf(&(st->right));

		Set_zero(st->wmem_sig_out,6);
		Set_zero(st->left.wmem_sig_out,6);
		Set_zero(st->right.wmem_sig_out,6);

		/* initialize memories (mono part) */
		st->last_mode = 0;             /* set last mode to acelp */
		st->wmem_deemph = 0;    

		Set_zero(st->h_fx,ECU_WIEN_ORD+1);

		Init_decoder_lf(st);
		if(num_chan == 2)
		{
			Init_decoder_stereo_x(st);
		}
		Set_zero(st->wold_xri,L_TCX);	

		Set_zero(st->wmem_gain_code, 4);
		Set_zero(st->wmem_lpc_hf + 1, MHF);
		st->wmem_lpc_hf[0] = 4096; 
		st->wmem_gain_hf = 0;      
		st->wramp_state = 64;      
		st->L_gc_thres = 0;        
		st->Old_bpf_scale = 0;     

	}
	return;
}



/*-----------------------------------------------------------------*
*                  Principal decoder routine                      *
*-----------------------------------------------------------------*/

#if (LARGE_VAR_OPT)
static  Word16 AqLF[(NB_SUBFR+1)*(M+1)];     /* LPC coefficients for band 0..6400Hz */
static  Word16 sig_left[L_FRAME_PLUS+2*L_FDEL+20]; /* Needed to extend sig_left in order to use it in decoder_stereo_x*/
static  Word16 synth_buf[L_BSP+L_TCX];

static  Word16 pit_gain_[NB_SUBFR];

static  Word16 pitch[NB_SUBFR];
static  Word16 nbits_AVQ[4];      
static  Word16 param[DEC_NPRM_DIV*NB_DIV];
static  Word16 prm_stereo[MAX_NPRM_STEREO_DIV*NB_DIV];   /* see cnst.h */
static  Word16 prm_hf_left[NPRM_BWE_DIV*NB_DIV];
static  Word16 prm_hf_right[NPRM_BWE_DIV*NB_DIV];
#endif

Word16 Decoder_amrwb_plus(    /* (o)  : number of sample processed */
						  Word16 codec_mode,          /* (i)  : AMR-WB+ mode (see cnst.h)  */
						  Word16 serial[],            /* (i)  : serial parameters (4x20ms) */
						  Word16 bad_frame[],         /* (i)  : bfi (bad_frame[4])         */
						  Word16 L_frame,             /* (i)  : frame size of synthesis    */
						  Word16 n_channel,           /* (i)  : 1 or 2 (mono/stereo)       */
						  Word16 channel_right[],     /* (o)  : used on mono and stereo    */
						  Word16 channel_left[],      /* (o)  : used on stereo only        */
						  Decoder_State_Plus_fx *st,  /* (i/o): decoder memory state       */
						  Word16 fscale,              /* (i)  : Frequency scaling          */
						  Word16 StbrMode,            /* (i)  : Stereo bitrate mode        */   
						  Word16 mono_dec_stereo,     /* (i)  : 1=Mono decoding with stereo bitstream */
						  Word16 upsamp_fscale        /* (i)  : Upsampling frequency scaling */
						  )
{
#if (!LARGE_VAR_OPT)
	Word16 AqLF[(NB_SUBFR+1)*(M+1)];     /* LPC coefficients for band 0..6400Hz */
	Word16 sig_left[L_FRAME_PLUS+2*L_FDEL+20]; /* Needed to extend sig_left in order to use it in decoder_stereo_x*/
	Word16 synth_buf[L_BSP+L_TCX];

	Word16 pit_gain_[NB_SUBFR];

	Word16 pitch[NB_SUBFR];
	Word16 nbits_AVQ[4];      
	Word16 param[DEC_NPRM_DIV*NB_DIV];
	Word16 prm_stereo[MAX_NPRM_STEREO_DIV*NB_DIV];   /* see cnst.h */
	Word16 prm_hf_left[NPRM_BWE_DIV*NB_DIV];
	Word16 prm_hf_right[NPRM_BWE_DIV*NB_DIV];
#endif

	/* Scalars */
	Word16 i, k;
	Word16 mod_buf[1+NB_DIV], *mod;
	Word16 nbits_pack;
	Word16 bad_frame_hf[4];
	Word16 n_loss, any_tcx80;
	Word16 nb_samp;

	Word16 *synth;

	synth = synth_buf + L_FILT_2k;

	/* for 20-ms packetization, divide by 4 the 80-ms bitstream */
	nbits_pack = ((NBITS_CORE_FX[codec_mode] + NBITS_BWE) >> 2);  

	mono_dec_stereo = 0; 
	if (StbrMode >= 0) 
	{
		if(n_channel == 1)
		{
			mono_dec_stereo = 1;
		}

		nbits_pack = (nbits_pack + ((StereoNbits_FX[StbrMode] + NBITS_BWE) >> 2));  
	}

	/*------------------------------------------------------------------------*
	* read modes (somes modes may be unreadable if the related frame is lost)*
	*------------------------------------------------------------------------*
	* mode 0 = ACELP 20ms   --> mode[] = 0,x,x,x  ..  x,x,x,0                *
	* mode 1 = TCX 20ms     --> mode[] = 1,x,x,x  ..  x,x,x,1                *
	* mode 2 = TCX 40ms     --> mode[] = 2,2,x,x  or  x,x,2,2                *
	* mode 3 = TCX 80ms     --> mode[] = 3,3,3,3                             *
	*------------------------------------------------------------------------*/

	mod = mod_buf+1;
	mod[-1] = st->last_mode;     /* previous mode */

	n_loss = 0;                 	

	for (k=0; k<NB_DIV; k++) 
	{ 
		if (bad_frame[k] == 0) 
		{
			mod[k] = Bin2int(2, &serial[k*nbits_pack]); 
		}
		else
		{
			mod[k] = -1;
		}
		n_loss += bad_frame[k];
	}

	for(i=0;i<4;i++)
	{
		bad_frame_hf[i] = bad_frame[i];
	}
	any_tcx80 = (mod[0] == 3); 

	for (i=1; i<4; i++) 
	{
		any_tcx80 |= (mod[i]==3);  
	}
	if ((n_loss > 2) && (any_tcx80)) 
	{
		for (i=0; i<4; i++) 
		{
			mod[i] = -1;                
			bad_frame[i] = 1;             
		}
	}

	/* extrapolate mode in case of packet erasures and fix bit errors */
	if ((mod[0] == 3)||(mod[1] == 3)||(mod[2] == 3)||(mod[3] == 3)) 
	{
		/* handle loss of one or several TCX-80 packets */ 
		for (k=0; k<NB_DIV; k++) 
		{
			mod[k] = 3; 
		}
	}
	else
	{
		/* handle loss of a TCX-40 packet */
		if ((mod[0] == 2)|| (mod[1] == 2)) 
		{
			for (k=0; k<2; k++) 
			{
				mod[k] = 2;
			}
		}
		if ((mod[2] == 2)|| (mod[3] == 2)) 
		{
			for (k=2; k<4; k++) 
			{
				mod[k] = 2;
			}
		}

		/* handle loss of an ACELP or TCX-20 packet */

		for (k=0; k<NB_DIV; k++) 
		{
			if (mod[k] < 0) 
			{
				/* repeat the previous mode:
				if previous mode = ACELP        -> ACELP
				if previous mode = TCX-20/40/80 -> TCX-20
				notes:
				- ACELP is not allowed after TCX (no pitch information to reuse)
				- TCX-40 is not allowed in the mode repetition to keep the logic simple */
				if (mod[k-1] == 0) 
				{
					mod[k] = 0; 
				}
				else
				{
					mod[k] = 1; 
				}
			}
		}
	}

	/*----------- DECODE BIT-STREAM ----------*/

	Dec_prm(mod, bad_frame, serial, nbits_pack, codec_mode, param, nbits_AVQ);                    

	if ((n_channel == 2) && (StbrMode >= 0))
	{
		Dec_prm_stereo_x(bad_frame_hf, serial, nbits_pack, NBITS_BWE, prm_stereo, StbrMode, st);    
	}
	Dec_prm_hf(mod, bad_frame, serial, nbits_pack, prm_hf_right);                                 
                                                                                             
	if ((n_channel == 2) || (mono_dec_stereo == 1)) 
	{
		if (StbrMode < 0)
		{
			voAMRWBPDecCopy(prm_hf_right, prm_hf_left, NPRM_BWE_DIV*NB_DIV);
		}
		else
		{
			Dec_prm_hf(mod, bad_frame, serial-NBITS_BWE/4, nbits_pack, prm_hf_left);
		}
	}

	Decoder_amrwb_plus_1(channel_right,channel_left,mod,param,prm_hf_right,prm_hf_left,nbits_AVQ,
		codec_mode,bad_frame,bad_frame_hf,AqLF,synth,pitch,pit_gain_,st,n_channel,
		L_frame, fscale,mono_dec_stereo);

	if (n_channel == 2)
	{
		Decoder_stereo_x(prm_stereo, bad_frame_hf, sig_left, synth, AqLF,StbrMode,fscale, st);  
		if (fscale == 0)
		{
			voAMRWBPDecHP50_12k8(sig_left, L_FRAME_PLUS, st->right.wmem_sig_out);                             
			voAMRWBPDecHP50_12k8(synth, L_FRAME_PLUS, st->left.wmem_sig_out);                                 

			voAMRWBPDecOversamp_12k8(sig_left, channel_left, L_frame, st->left.wmem_oversamp, 0,1);              
			voAMRWBPDecOversamp_12k8(synth, channel_right, L_frame, st->right.wmem_oversamp, 0,1);               
			nb_samp = L_frame; 
		}
		else 
		{
			/* band join and oversampling (HF into channel_left/right) */
			/*fac_fs = (L_FRAME48k*upsamp_fscale)/L_frame;*/
			nb_samp = Join_over_12k8(sig_left, channel_left, L_FRAME_PLUS, channel_left,L_frame,
				upsamp_fscale, st->left.wmem_oversamp, &(st->left.wover_frac));
			nb_samp = Join_over_12k8(synth, channel_right, L_FRAME_PLUS, channel_right,L_frame,
				upsamp_fscale, st->right.wmem_oversamp, &(st->right.wover_frac));
		}
	}
	else
	{
		if (fscale == 0)
		{
			voAMRWBPDecOversamp_12k8(synth, channel_right, L_frame, st->right.wmem_oversamp, 0,1);               
			nb_samp = L_frame; 
		}
		else 
		{
			/* band join and oversampling (HF into channel_left/right) */
			/*fac_fs = (L_FRAME48k*upsamp_fscale)/L_frame;*/

			nb_samp = Join_over_12k8(synth, channel_right, L_FRAME_PLUS, channel_right,L_frame,
				upsamp_fscale, st->right.wmem_oversamp, &(st->right.wover_frac));
		}

	}

	/* update for next superframe */
	st->last_mode = mod[NB_DIV-1];

	return(nb_samp);
}


#if (LARGE_VAR_OPT)
Word16 exc_buf[PIT_MAX_MAX+L_INTERPOL+L_TCX]; 
#endif
void Decoder_amrwb_plus_1(
						  Word16* chan_right,     /* (o)  : Channel right synthesis HF Q_syn +1 */
						  Word16* chan_left,      /* (o)  : Channel left synthesis  HF Q_syn +1 */  
						  Word16* mod,            /* (i)  : mode for each 20ms frame (mode[4])  */
						  Word16* param,          /* (i)  : parameter to decode                 */    
						  Word16* prm_hf_right,   /* (i)  : HF parameters channel right         */
						  Word16* prm_hf_left,    /* (i)  : HF parameters channel left          */    
						  Word16* nbits_AVQ,      /* (i)  : Number of bits used by AVQ_dec      */  
						  Word16  codec_mode,     /* (i)  : AMR-WB+ mode (see cnst.h)           */ 
						  Word16* bad_frame,      /* (i)  : for each frame (bad_frame[4])       */
						  Word16* bad_frame_hf,   /* (i)  : for each frame (bad_frame[4])       */  
						  Word16* AqLF,           /* (o)  : decoded coefficients (AdLF[16])     */  
						  Word16* synth,          /* (o)  : decoded synthesis              Q_syn*/
						  Word16* pitch,          /* (o)  : decoded pitch (pitch[16])           */
						  Word16* pit_gain,       /* (o)  : decoded pitch gain (pit_gain[16])Q14*/
						  Decoder_State_Plus_fx* st, /* (i/o) : coder memory state              */
						  Word16 n_channel,       /* (i)  : Number of channels                  */
						  Word16 L_frame,         /* (i)  : Frame length                        */
						  Word16 fscale,          /* (i)  : frequency scaling                   */    
						  Word16 mono_dec_stereo  /* (i)  : 1=Mono decoding with stereo bitstream */
						  )
{

#if (!LARGE_VAR_OPT)
	Word16 exc_buf[PIT_MAX_MAX+L_INTERPOL+L_TCX]; 
#endif
	Word16 *exc;

	exc = exc_buf + PIT_MAX_MAX + L_INTERPOL;
	voAMRWBPDecCopy(st->wold_exc, exc_buf, PIT_MAX_MAX + L_INTERPOL);

	Decoder_lf(mod, param, nbits_AVQ, codec_mode, bad_frame, AqLF, exc, synth, pitch, pit_gain,fscale, st );		

	Scale_mem2(synth, st->old_syn_q, st, 1);
	voAMRWBPDecCopy(&synth[L_FRAME_PLUS-M], st->cp_old_synth, M);        /* needed in cp state */

	voAMRWBPDecCopy(exc_buf+L_FRAME_PLUS, st->wold_exc, PIT_MAX_MAX+L_INTERPOL);

	exc = exc_buf;

	Scale_mem2(exc, st->old_subfr_q, st, 0);
	voAMRWBPDecDeemph(synth, PREEMPH_FAC_FX, L_FRAME_PLUS, &(st->wmem_deemph));
	Bass_postfilter(synth, pitch, pit_gain, synth, fscale, st);

	if (fscale == 0)
	{
		if(n_channel == 1) 
		{
			/* high pass filter only in the mono case to avoid phase mismatch */
			voAMRWBPDecHP50_12k8(synth, L_FRAME_PLUS, st->wmem_sig_out);
		}
	}

	if (L_frame > L_FRAME8k) 
	{
		Decoder_hf(mod, prm_hf_right, prm_hf_left, mono_dec_stereo, bad_frame_hf, AqLF, exc, chan_right, st->wmem_lpc_hf, &(st->wmem_gain_hf), &(st->wramp_state), &(st->right),  st->Old_Q_exc);
		if(n_channel == 1) 
		{
			if(fscale == 0) 
			{
				Delay(chan_right, L_FRAME_PLUS, DELAY_PF, st->right.wold_synth_hf);
			}
			else 
			{
				Delay(chan_right, L_FRAME_PLUS, DELAY_PF+L_SUBFR, st->right.wold_synth_hf);
			}
		}
		else  
		{
			if(fscale == 0) 
			{
				Delay(chan_right, L_FRAME_PLUS, (D_BPF+L_BSP+2*D_NC+L_FDEL+32*D_STEREO_TCX/5) , st->right.wold_synth_hf);	
			}
			else 
			{
				Delay(chan_right, L_FRAME_PLUS, (D_BPF+L_SUBFR+L_BSP+2*D_NC+L_FDEL+32*D_STEREO_TCX/5) , st->right.wold_synth_hf);		
			}
		}
		if(n_channel == 2) 
		{
			Decoder_hf(mod, prm_hf_left, prm_hf_right, mono_dec_stereo, bad_frame_hf, AqLF, exc, chan_left, st->wmem_lpc_hf, &(st->wmem_gain_hf), &(st->wramp_state), &(st->left),  st->Old_Q_exc);
			if(fscale == 0) 
			{
				Delay(chan_left, L_FRAME_PLUS,(D_BPF + L_BSP + 2*D_NC + L_FDEL + 32*D_STEREO_TCX/5), st->left.wold_synth_hf);
			}
			else 
			{
				Delay(chan_left, L_FRAME_PLUS,(D_BPF + L_SUBFR + L_BSP + 2*D_NC + L_FDEL + 32*D_STEREO_TCX/5), st->left.wold_synth_hf);
			}
		}

		if (fscale == 0)
		{
			voAMRWBPDecOversamp_12k8(chan_right, chan_right, L_frame, st->right.wmem_oversamp_hf, 1,0);    
			if (n_channel == 2) 
			{
				voAMRWBPDecOversamp_12k8(chan_left, chan_left, L_frame, st->left.wmem_oversamp_hf, 1,0);     
			}
		}
	}
	else
	{
		Set_zero( chan_right, L_frame );                                                          
		Set_zero( chan_left, L_frame );                                                           
	}

	return;
}

