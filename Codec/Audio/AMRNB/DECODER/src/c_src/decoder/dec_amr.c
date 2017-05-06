/*************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
*****************************************************************************
*
*      File             : dec_amr.c
*      Purpose          : Decoding of one speech frame using given codec mode
*
*****************************************************************************
*/

/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "dec_amr.h"
const char dec_amr_id[] = "@(#)$Id $" dec_amr_h;

#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "cnst.h"
#include "copy.h"
#include "set_zero.h"
#include "syn_filt.h"
#include "d_plsf.h"
#include "agc.h"
#include "int_lpc.h"
#include "dec_gain.h"
#include "dec_lag3.h"
#include "dec_lag6.h"
#include "d2_9pf.h"
#include "d2_11pf.h"
#include "d3_14pf.h"
#include "d4_17pf.h"
#include "d8_31pf.h"
#include "d1035pf.h"
#include "pred_lt.h"
#include "d_gain_p.h"
#include "d_gain_c.h"
#include "dec_gain.h"
#include "ec_gains.h"
#include "ph_disp.h"
#include "c_g_aver.h"
#include "int_lsf.h"
#include "lsp_lsf.h"
#include "lsp_avg.h"
#include "bgnscd.h"
#include "ex_ctrl.h"
#include "sqrt_l.h"
#include "frame.h"
#include "lsp.tab"
#include "bitno.tab"
#include "b_cn_cod.h"
/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/
/*-----------------------------------------------------------------*
*   Decoder constant parameters (defined in "cnst.h")             *
*-----------------------------------------------------------------*
*   L_FRAME       : Frame size.                                   *
*   L_FRAME_BY2   : Half the frame size.                          *
*   L_SUBFR       : Sub-frame size.                               *
*   M             : LPC order.                                    *
*   MP1           : LPC order+1                                   *
*   PIT_MIN       : Minimum pitch lag.                            *
*   PIT_MIN_MR122 : Minimum pitch lag for the MR122 mode.         *
*   PIT_MAX       : Maximum pitch lag.                            *
*   L_INTERPOL    : Length of filter for interpolation            *
*   PRM_SIZE      : size of vector containing analysis parameters *
*-----------------------------------------------------------------*/

/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
/*
**************************************************************************
*
*  Function    : Decoder_amr_init
*  Purpose     : Allocates and initializes state memory
*
**************************************************************************
*/
void ex_ltp1(Word16 exc_enhanced[],
			Word16 code[], 
			Word16 *ptr,
			Word16 pitch_fac, 
			Word16 gain_code,
			Word16 tmp_shift)
{
	int i;
	Word32 L_temp;    
	for (i = 0; i < L_SUBFR; i++)
	{
		exc_enhanced[i] = *(ptr+i);                        
		/* st->exc[i] = gain_pit*st->exc[i] + gain_code*code[i]; */
		L_temp = L_mult (*(ptr+i), pitch_fac);
		L_temp = L_mac (L_temp, code[i], gain_code);
		L_temp = L_shl (L_temp, tmp_shift);                   
		*(ptr+i) = (L_temp + 0x8000)>>16;                        
	}
}

int Decoder_amr_init (Decoder_amrState **state, VO_MEM_OPERATOR *pMemOP)
{
	Decoder_amrState* s;
	Word16 i;
	if (state == (Decoder_amrState **) NULL){
		return -1;
	}
	*state = NULL;
	/* allocate memory */
	if ((s= (Decoder_amrState *) voAMRNBDecmem_malloc(pMemOP, sizeof(Decoder_amrState), 32)) == NULL){
		return -1;
	}

	s->T0_lagBuff = 40;
	s->inBackgroundNoise = 0;
	s->voicedHangover = 0;
	for (i = 0; i < 9; i++)
		s->ltpGainHistory[i] = 0;

	s->lsfState = NULL;
	s->ec_gain_p_st = NULL;
	s->ec_gain_c_st = NULL;  
	s->pred_state = NULL;
	s->ph_disp_st = NULL;
	s->dtxDecoderState = NULL;

	if (D_plsf_init(&s->lsfState, pMemOP) || ec_gain_pitch_init(&s->ec_gain_p_st, pMemOP) ||
		ec_gain_code_init(&s->ec_gain_c_st, pMemOP) || gc_pred_init(&s->pred_state, pMemOP) ||
		Cb_gain_average_init(&s->Cb_gain_averState, pMemOP) || lsp_avg_init(&s->lsp_avg_st, pMemOP) ||      
		Bgn_scd_init(&s->background_state, pMemOP) || ph_disp_init(&s->ph_disp_st, pMemOP) || 
		voAMRNBDecDtx_dec_init(&s->dtxDecoderState, pMemOP)) {
			Decoder_amr_exit(&s, pMemOP);
			return -1;
	}

	Decoder_amr_reset(s, (enum Mode)0);
	*state = s;

	return 0;
}

/*
**************************************************************************
*
*  Function    : Decoder_amr_reset
*  Purpose     : Resets state memory
*
**************************************************************************
*/
int Decoder_amr_reset (Decoder_amrState *state, enum Mode mode)
{
	Word16 i;
	if (state == (Decoder_amrState *) NULL){
		return -1;
	}
    /* Initialize static pointer */
	state->exc = state->old_exc + PIT_MAX + L_INTERPOL;
	/* Static vectors to zero */
	voAMRNBDecSetZero (state->old_exc, PIT_MAX + L_INTERPOL);
	if (mode != MRDTX)
		voAMRNBDecSetZero (state->mem_syn, M);
	/* initialize pitch sharpening */
	state->sharp = SHARPMIN;
	state->old_T0 = 40;
	/* Initialize state->lsp_old [] */ 
	if (mode != MRDTX) {
		voAMRNBDecCopy(voAMRNBDeclsp_init_data, &state->lsp_old[0], M);
	}
	/* Initialize memories of bad frame handling */
	state->prev_bf = 0;
	state->prev_pdf = 0;
	state->state = 0;
	state->T0_lagBuff = 40;
	state->inBackgroundNoise = 0;
	state->voicedHangover = 0;
	if (mode != MRDTX) {
		for (i=0;i<9;i++)
			state->excEnergyHist[i] = 0;
	}
	for (i = 0; i < 9; i++)
		state->ltpGainHistory[i] = 0;

	Cb_gain_average_reset(state->Cb_gain_averState);
	if (mode != MRDTX)
		lsp_avg_reset(state->lsp_avg_st);
	D_plsf_reset(state->lsfState);
	ec_gain_pitch_reset(state->ec_gain_p_st);
	ec_gain_code_reset(state->ec_gain_c_st);

	if (mode != MRDTX)
		gc_pred_reset(state->pred_state);

	Bgn_scd_reset(state->background_state);
	state->nodataSeed = 21845;
	ph_disp_reset(state->ph_disp_st);
	if (mode != MRDTX)
		voAMRNBDecDtx_dec_reset(state->dtxDecoderState);

	return 0;
}

/*
**************************************************************************
*
*  Function    : Decoder_amr_exit
*  Purpose     : The memory used for state memory is freed
*
**************************************************************************
*/
void Decoder_amr_exit (Decoder_amrState **state, VO_MEM_OPERATOR *pMemOP)
{
	if (state == NULL || *state == NULL)
		return;

	D_plsf_exit(&(*state)->lsfState, pMemOP);
	ec_gain_pitch_exit(&(*state)->ec_gain_p_st, pMemOP);
	ec_gain_code_exit(&(*state)->ec_gain_c_st, pMemOP);
	gc_pred_exit(&(*state)->pred_state, pMemOP);
	Bgn_scd_exit(&(*state)->background_state, pMemOP);
	ph_disp_exit(&(*state)->ph_disp_st, pMemOP);
	Cb_gain_average_exit(&(*state)->Cb_gain_averState, pMemOP);
	lsp_avg_exit(&(*state)->lsp_avg_st, pMemOP);
	voAMRNBDecDtx_dec_exit(&(*state)->dtxDecoderState, pMemOP);

	/* deallocate memory */
	voAMRNBDecmem_free(pMemOP, *state);
	*state = NULL;
	return;
}

/*
**************************************************************************
*
*  Function    : Decoder_amr
*  Purpose     : Speech decoder routine.
*
**************************************************************************
*/
int Decoder_amr (
				 Decoder_amrState *st,			/* i/o : State variables                   */
				 enum Mode mode,				/* i   : AMR mode                          */
				 Word16 parm[],					/* i   : vector of synthesis parameters (PRM_SIZE) */
				 enum RXFrameType frame_type,   /* i   : received frame type             */
				 Word16 synth[],			    /* o   : synthesis speech (L_FRAME)        */
				 Word16 A_t[]                   /* o   : decoded LP filter in 4 subframes (AZ_SIZE)*/
)
{
	/* LPC coefficients */
	Word16 *Az;                /* Pointer on A_t */
	/* LSPs */
	Word16 lsp_new[M];
	Word16 lsp_mid[M];
	/* LSFs */
	Word16 prev_lsf[M];
	Word16 lsf_i[M];    
	/* Algebraic codevector */
	Word16 code[L_SUBFR];
	/* excitation */
	Word16 excp[L_SUBFR];
	Word16 exc_enhanced[L_SUBFR];
	/* Scalars */
	Word16 i, i_subfr;
	Word16 T0, T0_frac, index, index_mr475 = 0;
	Word16 gain_pit, gain_code, gain_code_mix, pit_sharp = 0, pit_flag, pitch_fac;
	Word16 t0_min, t0_max;
	Word16 delta_frc_low, delta_frc_range;
	Word16 tmp_shift;
	Word16 temp;
	Word32 L_temp;
	Word16 flag4;
	Word16 carefulFlag;
	Word16 excEnergy;
	Word16 subfrNr; 
	Word16 evenSubfr = 0;

	Word16 bfi = 0;   /* bad frame indication flag                          */
	Word16 pdfi = 0;  /* potential degraded bad frame flag                  */

	enum DTXStateType newDTXState;  /* SPEECH , DTX, DTX_MUTE */

	/* find the new  DTX state  SPEECH OR DTX */

	newDTXState = voAMRNBDecRx_dtx_handler(st->dtxDecoderState, frame_type);

	/* DTX actions */
	if(newDTXState != SPEECH)
	{
		Decoder_amr_reset (st, MRDTX);
		voAMRNBDecDtx_dec(st->dtxDecoderState, 
			st->mem_syn, 
			st->lsfState, 
			st->pred_state,
			st->Cb_gain_averState,
			newDTXState,
			mode, 
			parm, synth, A_t);
		/* update average lsp */

		Lsf_lsp(st->lsfState->past_lsf_q, st->lsp_old, M);
		lsp_avg(st->lsp_avg_st, st->lsfState->past_lsf_q);

		goto the_end;
	}
	/* SPEECH action state machine  */
	if ((frame_type == RX_SPEECH_BAD) || (frame_type == RX_NO_DATA) || (frame_type == RX_ONSET)) 
	{
		bfi = 1; 
		if ((frame_type == RX_NO_DATA) || (frame_type == RX_ONSET))
		{
			build_CN_param(&st->nodataSeed,voAMRNBDecprmno[mode],voAMRNBDecbitno[mode],parm);
		}       
	}
	else if(frame_type == RX_SPEECH_DEGRADED)
	{
		pdfi = 1; 
	}
	if (bfi != 0)
	{
		st->state = (st->state +  1);
	}
	else if (st->state == 6)
	{
		st->state = 5;                                   
	}
	else
	{
		st->state = 0;                                   
	}
	if(st->state > 6)
	{
		st->state = 6;                                   
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
	if(st->dtxDecoderState->dtxGlobalState == DTX)
	{
		st->state = 5;                                    
		st->prev_bf = 0;                                  
	}
	else if(st->dtxDecoderState->dtxGlobalState == DTX_MUTE)
	{
		st->state = 5;                                    
		st->prev_bf = 1;                                  
	}
	/* save old LSFs for CB gain smoothing */
	voAMRNBDecCopy (st->lsfState->past_lsf_q, prev_lsf, M);

	/* decode LSF parameters and generate interpolated lpc coefficients
	for the 4 subframes */
	if(mode != MR122)
	{
		D_plsf_3(st->lsfState, mode, bfi, parm, lsp_new);
		/* Advance synthesis parameters pointer */
		parm += 3;                 
		Int_lpc_1to3(st->lsp_old, lsp_new, A_t);
		/* update the LSPs for the next frame */
		for (i = 0; i < M; i++)
		{
			st->lsp_old[i] = lsp_new[i];      
		}
	}
	else
	{
		D_plsf_5 (st->lsfState, bfi, parm, lsp_mid, lsp_new);
		/* Advance synthesis parameters pointer */
		parm += 5;              
		Int_lpc_1and3 (st->lsp_old, lsp_mid, lsp_new, A_t);
	}
	/*------------------------------------------------------------------------*
	*          Loop for every subframe in the analysis frame                 *
	*------------------------------------------------------------------------*
	* The subframe size is L_SUBFR and the loop is repeated L_FRAME/L_SUBFR  *
	*  times                                                                 *
	*     - decode the pitch delay                                           *
	*     - decode algebraic code                                            *
	*     - decode pitch and codebook gains                                  *
	*     - find the excitation and compute synthesis speech                 *
	*------------------------------------------------------------------------*/

	/* pointer to interpolated LPC parameters */
	Az = A_t;                                                       
	evenSubfr = 0;                                                  
	subfrNr = -1;                                                   
	for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
	{
		subfrNr = subfrNr + 1 ;                                     
		evenSubfr = 1- evenSubfr;    /* flag for first and 3th subframe */
		pit_flag = i_subfr;             
		if (i_subfr == L_FRAME_BY2)
		{
			if (mode > MR515) 
			{
				pit_flag = 0;             
			}
		}
		/* pitch index */
		index = *parm++;               
		/*-------------------------------------------------------*
		* - decode pitch lag and find adaptive codebook vector. *
		*-------------------------------------------------------*/
		if (mode != MR122)
		{
			/* flag4 indicates encoding with 4 bit resolution;     */
			/* this is needed for mode MR475, MR515, MR59 and MR67 */

			//flag4 = (mode -MR67) <= 0;    //G1 platform have bug
            flag4 = (mode<=MR67);			
			/*-------------------------------------------------------*
			* - get ranges for the t0_min and t0_max                *
			* - only needed in delta decoding                       *
			*-------------------------------------------------------*/
			delta_frc_low = 5;                      
			delta_frc_range = 9;                    
			if (mode == MR795)
			{
				delta_frc_low = 10;                 
				delta_frc_range = 19;               
			}
			t0_min = (st->old_T0 - delta_frc_low);
			if(t0_min < PIT_MIN)
			{
				t0_min = PIT_MIN;                   
			}
			t0_max = (t0_min + delta_frc_range);
			if(t0_max > PIT_MAX)
			{
				t0_max = PIT_MAX;                   
				t0_min = (t0_max -  delta_frc_range);
			}
			Dec_lag3 (index, t0_min, t0_max, pit_flag, st->old_T0, &T0, &T0_frac, flag4);
			st->T0_lagBuff = T0;        

			if (bfi != 0)
			{

				/* Avoid violate the patent, so comment the code out, which has no big influence decoder 
				 * output wav files
				 */

				//if(st->old_T0 < PIT_MAX)     
				//{                                        
				//	st->old_T0 += 1;                  
				//}

				T0 = st->old_T0;                     
				T0_frac = 0;                         

				if ((mode <= MR59)&&st->inBackgroundNoise != 0 && (st->voicedHangover > 4))
				{
					T0 = st->T0_lagBuff;                  
				}
			}
			Pred_lt_3or6 (st->exc, T0, T0_frac, L_SUBFR, 1);
		}
		else
		{
			Dec_lag6 (index, PIT_MIN_MR122, PIT_MAX, pit_flag, &T0, &T0_frac);
			if ( bfi != 0 || (pit_flag != 0 &&  (index >= 61) ))
			{
				st->T0_lagBuff = T0;                 
				T0 = st->old_T0;                      
				T0_frac = 0;                          
			}                
			Pred_lt_3or6 (st->exc, T0, T0_frac, L_SUBFR, 0);
		}
		/*-------------------------------------------------------*
		* - (MR122 only: Decode pitch gain.)                    *
		* - Decode innovative codebook.                         *
		* - set pitch sharpening factor                         *
		*-------------------------------------------------------*/
		switch(mode)
		{
		case MR122:
			{  /* MR122 */
				index = *parm++;                                
				if (bfi != 0)
				{
					ec_gain_pitch (st->ec_gain_p_st, st->state, &gain_pit);
				}
				else
				{
					gain_pit = d_gain_pitch (mode, index);        
				}
				ec_gain_pitch_update (st->ec_gain_p_st, bfi, st->prev_bf, &gain_pit);

				dec_10i40_35bits (parm, code);
				parm += 10;                                      
				/* pit_sharp = gain_pit;                   */
				/* if (pit_sharp > 1.0) pit_sharp = 1.0;   */
				pit_sharp = shl(gain_pit, 1);
			}
			break;
		case MR475:
		case MR515:
			{
				index = *parm++;        /* index of position */ 
				i = *parm++;            /* signs             */ 
				decode_2i40_9bits (subfrNr, i, index, code);
				pit_sharp = shl(st->sharp, 1);
			}
			break;
		case MR59:
			{   /* MR59 */
				index = *parm++;        /* index of position */ 
				i = *parm++;            /* signs             */ 
				decode_2i40_11bits (i, index, code);
				pit_sharp = shl(st->sharp, 1);
			}
			break;
		case MR67:
			{   /* MR67 */
				index = *parm++;        /* index of position */ 
				i = *parm++;            /* signs             */ 
				decode_3i40_14bits (i, index, code);
				pit_sharp = shl(st->sharp, 1);
			}
			break;
		case MR74:
		case MR795:
			{   /* MR74, MR795 */
				index = *parm++;        /* index of position */ 
				i = *parm++;            /* signs             */ 
				decode_4i40_17bits (i, index, code);
				pit_sharp = shl(st->sharp, 1);
			}
			break;
		case MR102:
			{  /* MR102 */
				dec_8i40_31bits(parm, code);
				parm += 7;                                       
				pit_sharp = shl(st->sharp, 1);
			}
			break;
		default:
			break;
		}
		/*-------------------------------------------------------*
		* - Add the pitch contribution to code[].               *
		*-------------------------------------------------------*/
		// c(n) = c(n) + coeff*c(n-T)    
		// c(n) -- fixed code vector
		for (i = T0; i < L_SUBFR; i++)
		{
			temp = (code[i - T0] * pit_sharp)>>15;
			code[i] = (code[i] + temp);
		}
		/*------------------------------------------------------------*
		* - Decode codebook gain (MR122) or both pitch               *
		*   gain and codebook gain (all others)                      *
		* - Update pitch sharpening "sharp" with quantized gain_pit  *
		*------------------------------------------------------------*/
		switch(mode)
		{
		case MR122:
			{ /* MR122 */
				index = *parm++;
				if (bfi == 0)
				{
					d_gain_code (st->pred_state, mode, index, code, &gain_code);
				}
				else
				{
					ec_gain_code (st->ec_gain_c_st, st->pred_state, st->state, &gain_code);
				}
				ec_gain_code_update (st->ec_gain_c_st, bfi, st->prev_bf, &gain_code);
				pit_sharp = gain_pit;                                
			}
			break;
		case MR475:
			{
				if (evenSubfr != 0) 
				{
					index_mr475 = *parm++;        
				}
				if (bfi == 0)
				{
					voAMRNBDec_DecGain(st->pred_state, mode, index_mr475, code, evenSubfr, &gain_pit, &gain_code); 
				}
				else
				{
					ec_gain_pitch (st->ec_gain_p_st, st->state, &gain_pit);
					ec_gain_code (st->ec_gain_c_st, st->pred_state, st->state, &gain_code);
				}
				ec_gain_pitch_update (st->ec_gain_p_st, bfi, st->prev_bf, &gain_pit);
				ec_gain_code_update (st->ec_gain_c_st, bfi, st->prev_bf, &gain_code);

				pit_sharp = gain_pit;                                
				if(pit_sharp > SHARPMAX) 
				{
					pit_sharp = SHARPMAX;                            
				}
			}
			break;
		case MR515:
		case MR59:
		case MR67:
		case MR74:
		case MR102:
			{
				/* read and decode pitch and code gain */
				index = *parm++;                                       /* index of gain(s) */
				if (bfi == 0)
				{
					voAMRNBDec_DecGain(st->pred_state, mode, index, code, evenSubfr, &gain_pit, &gain_code);
				}
				else
				{
					ec_gain_pitch (st->ec_gain_p_st, st->state, &gain_pit);
					ec_gain_code (st->ec_gain_c_st, st->pred_state, st->state, &gain_code);
				}
				ec_gain_pitch_update (st->ec_gain_p_st, bfi, st->prev_bf, &gain_pit);
				ec_gain_code_update (st->ec_gain_c_st, bfi, st->prev_bf, &gain_code);
				pit_sharp = gain_pit;                               
				if(pit_sharp > SHARPMAX) 
				{
					pit_sharp = SHARPMAX;                          
				}

				if(mode == MR102)
				{
					if(st->old_T0 > (L_SUBFR + 5))
					{
						pit_sharp = (pit_sharp >> 2);
					}
				}
			}
			break;
		case MR795:
			{
				index = *parm++;
				/* decode pitch gain */
				if (bfi != 0)
				{
					ec_gain_pitch (st->ec_gain_p_st, st->state, &gain_pit);
				}
				else
				{
					gain_pit = d_gain_pitch (mode, index);       
				}
				ec_gain_pitch_update (st->ec_gain_p_st, bfi, st->prev_bf, &gain_pit);
				/* read and decode code gain */
				index = *parm++;                               
				if (bfi == 0)
				{
					d_gain_code (st->pred_state, mode, index, code, &gain_code);
				}
				else
				{
					ec_gain_code (st->ec_gain_c_st, st->pred_state, st->state, &gain_code);
				}
				ec_gain_code_update (st->ec_gain_c_st, bfi, st->prev_bf, &gain_code);

				pit_sharp = gain_pit;                               
				if(pit_sharp > SHARPMAX)
				{
					pit_sharp = SHARPMAX;                           
				}
			}
			break;
		default:
			break;
		}
		/* store pitch sharpening for next subframe          */
		/* (for modes which use the previous pitch gain for 
		pitch sharpening in the search phase)             */
		/* do not update sharpening in even subframes for MR475 */
		if((mode != MR475)|| evenSubfr == 0)
		{
			st->sharp = gain_pit;                                   
			if(st->sharp > SHARPMAX)
			{
				st->sharp = SHARPMAX;                                
			}
		}
		pit_sharp = shl2(pit_sharp, 1);
		if ((pit_sharp > 16384) &&(mode ==MR122))
		{
#ifdef ARMv7_opt 
			excp_fun(st->exc, excp, pit_sharp, gain_pit);
#else
			for (i = 0; i < L_SUBFR; i++)
			{
				temp = mult(st->exc[i], pit_sharp);
				L_temp = L_mult (temp, gain_pit);
				L_temp =  L_shr2(L_temp, 1);
				excp[i] = vo_round (L_temp);                        
			}
#endif
		}
		if((pit_sharp > 16384) && (mode != MR122))
		{
			for (i = 0; i < L_SUBFR; i++)
			{
				temp = mult(st->exc[i], pit_sharp);
				L_temp = L_mult(temp, gain_pit);
				excp[i] = vo_round (L_temp);                        
			}
		}
		/*-------------------------------------------------------*
		* - Store list of LTP gains needed in the source        *
		*   characteristic detector (SCD)                       *
		*-------------------------------------------------------*/
		if ( bfi == 0 )
		{
			for (i = 0; i < 8; i++)
			{
				st->ltpGainHistory[i] = st->ltpGainHistory[i+1];  
			}
			st->ltpGainHistory[8] = gain_pit;                     
		}
		/*-------------------------------------------------------*
		* - Limit gain_pit if in background noise and BFI       *
		*   for MR475, MR515, MR59                              *
		*-------------------------------------------------------*/
		if ( (mode<=MR59)&&(st->prev_bf != 0 || bfi != 0) && st->inBackgroundNoise != 0)
		{
			if(gain_pit > 12288)                       /* if (gain_pit > 0.75) in Q14*/
				gain_pit = add((sub(gain_pit, 12288) >> 1), 12288);
			/* gain_pit = (gain_pit-0.75)/2.0 + 0.75; */
			if(gain_pit > 14745)    /* if (gain_pit > 0.90) in Q14*/
			{
				gain_pit = 14745;
			}
		}

		/*-------------------------------------------------------*
		*  Calculate CB mixed gain                              *
		*-------------------------------------------------------*/
		Int_lsf(prev_lsf, st->lsfState->past_lsf_q, i_subfr, lsf_i); 
		gain_code_mix = Cb_gain_average(st->Cb_gain_averState, mode, gain_code, 
			lsf_i, st->lsp_avg_st->lsp_meanSave, bfi,st->prev_bf, pdfi, st->prev_pdf,  
			st->inBackgroundNoise, st->voicedHangover);
		/* make sure that MR74, MR795, MR122 have original code_gain*/

		if((mode > MR67) && (mode != MR102))  /* MR74, MR795, MR122 */
		{
			gain_code_mix = gain_code; 
		}

		/*-------------------------------------------------------*
		* - Find the total excitation.                          *
		* - Find synthesis speech corresponding to st->exc[].   *
		*-------------------------------------------------------*/
		if(mode <= MR102)                     /* MR475, MR515, MR59, MR67, MR74, MR795, MR102*/
		{
			pitch_fac = gain_pit;                        
			tmp_shift = 1;                               
		}
		else       /* MR122 */
		{
			pitch_fac = gain_pit >> 1;                   
			tmp_shift = 2;                               
		}
		/* copy unscaled LTP excitation to exc_enhanced (used in phase
		* dispersion below) and compute total excitation for LTP feedback*/
#ifdef ARMv7_opt  //ex_ltp for ARMv7_opt, and ex_ltp1 for C
		ex_ltp(exc_enhanced, code, st->exc, pitch_fac, gain_code, tmp_shift); 
#else
		ex_ltp1(exc_enhanced, code, st->exc, pitch_fac, gain_code, tmp_shift);
		//for (i = 0; i < L_SUBFR; i++)
		//{
		//	exc_enhanced[i] = st->exc[i];  

		//	/* st->exc[i] = gain_pit*st->exc[i] + gain_code*code[i]; */
		//	L_temp = L_mult (st->exc[i], pitch_fac);
		//	/* 12.2: Q0 * Q13 */
		//	/*  7.4: Q0 * Q14 */
		//	L_temp = L_mac (L_temp, code[i], gain_code);
		//	/* 12.2: Q12 * Q1 */
		//	/*  7.4: Q13 * Q1 */
		//	L_temp = L_shl (L_temp, tmp_shift);                   /* Q16 */
		//	st->exc[i] = vo_round (L_temp); 
		//}
#endif
		/*-------------------------------------------------------*
		* - Adaptive phase dispersion                           *
		*-------------------------------------------------------*/
		ph_disp_release(st->ph_disp_st); /* free phase dispersion adaption */
		if ((mode <= MR59)&&(st->voicedHangover > 3) && st->inBackgroundNoise != 0 && bfi != 0 )
		{
			ph_disp_lock(st->ph_disp_st); /* Always Use full Phase Disp. */
		}                                /* if error in bg noise       */
		/* apply phase dispersion to innovation (if enabled) and
		compute total excitation for synthesis part           */
		ph_disp(st->ph_disp_st, mode, exc_enhanced, gain_code_mix, gain_pit, code, pitch_fac, tmp_shift);       
		/*-------------------------------------------------------*
		* - The Excitation control module are active during BFI.*
		* - Conceal drops in signal energy if in bg noise.      *
		*-------------------------------------------------------*/
		L_temp = 0;  
#ifdef  ARMv7_opt
        L_temp = voAMRNBDecsum_armv7_xx(L_temp, exc_enhanced, exc_enhanced, L_SUBFR);
#else
		for (i = 0; i < L_SUBFR; i++)
		{
			L_temp = L_mac(L_temp, exc_enhanced[i], exc_enhanced[i] );
		}
#endif
        L_temp = L_temp >> 1;
		L_temp = sqrt_l_exp(L_temp, &temp);              /* function result */
		L_temp = L_shr2(L_temp, ((temp >> 1) + 15));
		L_temp = L_temp >> 2;                            /* To cope with 16-bit and  */
		excEnergy = extract_l(L_temp);                   /* scaling in ex_ctrl()     */
		if((mode <= MR59) &&(st->voicedHangover > 5)&&st->inBackgroundNoise != 0 &&
			(st->state < 4) &&((pdfi != 0 && st->prev_pdf != 0) || bfi != 0 || st->prev_bf != 0) )
		{
			carefulFlag = 0;                          
			if ( pdfi != 0 && bfi == 0 )       
			{
				carefulFlag = 1;                      
			}

			Ex_ctrl(exc_enhanced,excEnergy,st->excEnergyHist,st->voicedHangover,
				st->prev_bf,carefulFlag);
		}
		if ( st->inBackgroundNoise != 0 && (bfi != 0 || st->prev_bf != 0 ) && (st->state < 4))
		{ 
			; /* do nothing! */
		}
		else
		{
			/* Update energy history for all modes */
			for (i = 0; i < 8; i++)
			{
				st->excEnergyHist[i] = st->excEnergyHist[i+1];
			}
			st->excEnergyHist[8] = excEnergy;
		}
		/*-------------------------------------------------------*
		* Excitation control module end.                        *
		*-------------------------------------------------------*/
		if(pit_sharp > 16384)
		{
#if 0 //def ARMv7_opt // Neon opt will crush
            agc2_asm(exc_enhanced, excp, L_SUBFR);
#else
			agc2 (exc_enhanced, excp, L_SUBFR);
#endif
			Overflow = 0;                 //move16 ();
#if 0 //def  ASM_OPT
            voAMRNBDecSyn_filt_0 (Az, excp, &synth[i_subfr], st->mem_syn);
#else
			Syn_filt (Az, excp, &synth[i_subfr], L_SUBFR, st->mem_syn, 0);
#endif
		}
		else
		{
			Overflow = 0;                 //move16 ();
#ifdef  ASM_OPT
            voAMRNBDecSyn_filt_0 (Az, exc_enhanced, &synth[i_subfr],st->mem_syn);
#else
			Syn_filt (Az, exc_enhanced, &synth[i_subfr], L_SUBFR, st->mem_syn, 0);
#endif
		}
		if (Overflow != 0)    /* Test for overflow */
		{
			for (i = 0; i < PIT_MAX + L_INTERPOL + L_SUBFR; i++)
			{
				st->old_exc[i] = shr(st->old_exc[i], 2);       
			}
			for (i = 0; i < L_SUBFR; i++)
			{
				exc_enhanced[i] = shr(exc_enhanced[i], 2);     
			}
			Syn_filt(Az, exc_enhanced, &synth[i_subfr], L_SUBFR, st->mem_syn, 1);
		}
		else
		{
			voAMRNBDecCopy(&synth[i_subfr+L_SUBFR-M], st->mem_syn, M);
		}

		/*--------------------------------------------------*
		* Update signal for next frame.                    *
		* -> shift to the left by L_SUBFR  st->exc[]       *
		*--------------------------------------------------*/
		voAMRNBDecCopy (&st->old_exc[L_SUBFR], &st->old_exc[0], PIT_MAX + L_INTERPOL);
		/* interpolated LPC parameters for next subframe */
		Az += MP1;                     
		st->old_T0 = T0;                /* store T0 for next subframe */ 
	}

	/*-------------------------------------------------------*
	* Call the Source Characteristic Detector which updates *
	* st->inBackgroundNoise and st->voicedHangover.         *
	*-------------------------------------------------------*/
	st->inBackgroundNoise = Bgn_scd(st->background_state,
		&(st->ltpGainHistory[0]),&(synth[0]),&(st->voicedHangover) );
	voAMRNBDecDtx_dec_activity_update(st->dtxDecoderState, st->lsfState->past_lsf_q, synth);
	/* store bfi for next subframe */
	st->prev_bf = bfi;                   
	st->prev_pdf = pdfi;                 

	/*--------------------------------------------------*
	* Calculate the LSF averages on the eight          *
	* previous frames                                  *
	*--------------------------------------------------*/
	lsp_avg(st->lsp_avg_st, st->lsfState->past_lsf_q);

the_end:
	st->dtxDecoderState->dtxGlobalState = newDTXState;  
	return 0;
}
