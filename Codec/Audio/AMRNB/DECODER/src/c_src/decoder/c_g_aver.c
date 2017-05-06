/*************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : c_g_aver.c
*      Purpose          : 
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "c_g_aver.h"
const char c_g_aver_id[] = "@(#)$Id $" c_g_aver_h;
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "mode.h"
#include "basic_op.h"
#include "cnst.h"
#include "set_zero.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
/*-----------------------------------------------------------------*
*   Decoder constant parameters (defined in "cnst.h")             *
*-----------------------------------------------------------------*
*   L_FRAME       : Frame size.                                   *
*   L_SUBFR       : Sub-frame size.                               *
*-----------------------------------------------------------------*/

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*
**************************************************************************
*
*  Function    : Cb_gain_average_init
*  Purpose     : Allocates and initializes state memory
*
**************************************************************************
*/
Word16 Cb_gain_average_init (Cb_gain_averageState **state, VO_MEM_OPERATOR *pMemOP)
{
	Cb_gain_averageState* s;
	if (state == (Cb_gain_averageState **) NULL){
		return -1;
	}
	*state = NULL;
	/* allocate memory */
	if ((s= (Cb_gain_averageState *) voAMRNBDecmem_malloc(pMemOP, sizeof(Cb_gain_averageState), 32)) == NULL){
		return -1;
	}
	Cb_gain_average_reset(s);
	*state = s;
	return 0;
}

/*
**************************************************************************
*
*  Function    : Cb_gain_average_reset
*  Purpose     : Resets state memory
*
**************************************************************************
*/
Word16 Cb_gain_average_reset (Cb_gain_averageState *state)
{
	if (state == (Cb_gain_averageState *) NULL){
		return -1;
	}
	/* Static vectors to zero */
	voAMRNBDecSetZero (state->cbGainHistory, L_CBGAINHIST);
	/* Initialize hangover handling */
	state->hangVar = 0;
	state->hangCount= 0;
	return 0;
}

/*
**************************************************************************
*
*  Function    : Cb_gain_average_exit
*  Purpose     : The memory used for state memory is freed
*
**************************************************************************
*/
void Cb_gain_average_exit (Cb_gain_averageState **state, VO_MEM_OPERATOR *pMemOP) 
{
	if (state == NULL || *state == NULL)
		return;
	/* deallocate memory */
	voAMRNBDecmem_free(pMemOP, *state);
	*state = NULL;
	return;
}

/*
**************************************************************************
*
*  Function    : Cb_gain_average
*  Purpose     : 
*  Returns     : The mix cb gains for MR475, MR515, MR59, MR67, MR102; gain_code other modes 
*
**************************************************************************
*/
Word16 Cb_gain_average (    
						Cb_gain_averageState *st, /* i/o : State variables for CB gain avergeing   */
						enum Mode mode,           /* i   : AMR mode                                */
						Word16 gain_code,         /* i   : CB gain                              Q1 */
						Word16 lsp[],             /* i   : The LSP for the current frame       Q15 */
						Word16 lspAver[],         /* i   : The average of LSP for 8 frames     Q15 */
						Word16 bfi,               /* i   : bad frame indication flag               */
						Word16 prev_bf,           /* i   : previous bad frame indication flag      */
						Word16 pdfi,              /* i   : potential degraded bad frame ind flag   */
						Word16 prev_pdf,          /* i   : prev pot. degraded bad frame ind flag   */
						Word16 inBackgroundNoise, /* i   : background noise decision               */
						Word16 voicedHangover     /* i   : # of frames after last voiced frame     */
						)
{
	/*---------------------------------------------------------*
	* Compute mixed cb gain, used to make cb gain more        *
	* smooth in background noise for modes 5.15, 5.9 and 6.7  *
	* states that needs to be updated by all                  *
	*---------------------------------------------------------*/
	Word16 i;
	Word16 cbGainMix, diff, tmp_diff, bgMix, cbGainMean = 0;
	Word32 L_sum;
	Word16 tmp[M], tmp1, tmp2, shift1, shift2, shift;

	/* set correct cbGainMix for MR74, MR795, MR122 */
	cbGainMix = gain_code; 

	/*-------------------------------------------------------*
	*   Store list of CB gain needed in the CB gain         *
	*   averaging                                           *
	*-------------------------------------------------------*/
	for (i = 0; i < (L_CBGAINHIST-1); i++)
	{
		st->cbGainHistory[i] = st->cbGainHistory[i+1];    
	}
	st->cbGainHistory[L_CBGAINHIST-1] = gain_code;       

	/* compute lsp difference */
	for (i = 0; i < M; i++) {
		tmp1 = abs_s((lspAver[i] - lsp[i]));  /* Q15       */
		shift1 = (norm_s(tmp1) - 1);          /* Qn        */
		tmp1 <<= shift1;

		//tmp1 = shl(tmp1, shift1);               /* Q15+Qn    */
		shift2 = norm_s(lspAver[i]);            /* Qm        */
		tmp2 = lspAver[i] << shift2;

		//tmp2 = shl2(lspAver[i], shift2);         /* Q15+Qm    */
		tmp[i] = div_s(tmp1, tmp2);             /* Q15+(Q15+Qn)-(Q15+Qm) */

		shift = ((2 + shift1) - shift2);

		if (shift >= 0)
		{
			tmp[i] = (tmp[i]>>shift);           /* Q15+Qn-Qm-Qx=Q13 */
		}
		else
		{
			tmp[i] = shl2(tmp[i], -shift);      /* Q15+Qn-Qm-Qx=Q13 */
		}
	}

	diff = tmp[0];  
	for (i = 1; i < M; i++) { 
		diff = (diff + tmp[i]);       /* Q13 */
	}   
	/* Compute hangover */
	st->hangVar = 0; 
	if(diff > 5325)  /* 0.65 in Q11 */
	{
		st->hangVar += 1;
	}

	if(st->hangVar > 10)
	{
		st->hangCount = 0;  /* Speech period, reset hangover variable */ //move16 ();
	}

	/* Compute mix constant (bgMix) */   
	bgMix = 8192;    /* 1 in Q13 */  

	if((mode <= MR67)|| (mode == MR102))  /* MR475, MR515, MR59, MR67, MR102 */
	{
		/* if errors and presumed noise make smoothing probability stronger */
		if (((((pdfi != 0) && (prev_pdf != 0)) || (bfi != 0) || (prev_bf != 0)) && (voicedHangover > 1)&& (inBackgroundNoise != 0) && 
			(mode<=MR59)))
		{
			/* bgMix = min(0.25, max(0.0, diff-0.55)) / 0.25; */
			tmp_diff = sub(diff, 4506);   /* 0.55 in Q13 */
			/* max(0.0, diff-0.55) */
			if (tmp_diff > 0)
			{
				tmp1 = tmp_diff;       
			}
			else
			{
				tmp1 = 0;              
			}
			/* min(0.25, tmp1) */
			if (2048 < tmp1)
			{
				bgMix = 8192;          
			}
			else
			{
				bgMix = shl2(tmp1, 2);
			}
		}
		else
		{
			/* bgMix = min(0.25, max(0.0, diff-0.40)) / 0.25; */        
			tmp_diff = (diff - 3277); /* 0.4 in Q13 */
			/* max(0.0, diff-0.40) */
			if (tmp_diff > 0)
			{
				tmp1 = tmp_diff;
			}
			else
			{
				tmp1 = 0;
			}
			/* min(0.25, tmp1) */
			if (2048 < tmp1)
			{
				bgMix = 8192;
			}
			else
			{
				bgMix = shl2(tmp1, 2);
			}
		}

		if ((st->hangCount < 40) || (diff > 5325)) /* 0.65 in Q13 */
		{
			bgMix = 8192;  /* disable mix if too short time since */ //move16 ();
		}
		/* Smoothen the cb gain trajectory  */
		/* smoothing depends on mix constant bgMix */
		L_sum = L_mult(6554, st->cbGainHistory[2]);     /* 0.2 in Q15; L_sum in Q17 */   
		for (i = 3; i < L_CBGAINHIST; i++)
		{
			L_sum = L_mac(L_sum, 6554, st->cbGainHistory[i]);
		}
		cbGainMean = vo_round(L_sum);                      /* Q1 */

		/* more smoothing in error and bg noise (NB no DFI used  here) */
		if ((mode <= MR59)&&((bfi != 0) || (prev_bf != 0)) && (inBackgroundNoise != 0))
		{
			L_sum = L_mult(4681, st->cbGainHistory[0]);  /* 0.143 in Q15; L_sum in Q17 */     
			for (i = 1; i < L_CBGAINHIST; i++)
			{
				L_sum = L_mac(L_sum, 4681, st->cbGainHistory[i]);
			}
			cbGainMean = vo_round(L_sum);   
		}
		/* cbGainMix = bgMix*cbGainMix + (1-bgMix)*cbGainMean; */
		L_sum = L_mult(bgMix, cbGainMix);               /* L_sum in Q15 */
		L_sum = L_mac(L_sum, 8192, cbGainMean);         
		L_sum = L_msu(L_sum, bgMix, cbGainMean);
		cbGainMix = vo_round(L_shl2(L_sum, 2));             /* Q1 */
	}
	st->hangCount += 1;
	return cbGainMix;
}
