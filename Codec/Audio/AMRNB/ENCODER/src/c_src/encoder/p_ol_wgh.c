/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
*****************************************************************************
*
*      File             : p_ol_wgh.c
*      Purpose          : Compute the open loop pitch lag with weighting      
*
*************************************************************************/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "p_ol_wgh.h"
const char p_ol_wgh_id[] = "@(#)$Id $" p_ol_wgh_h;

/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "cnst.h"
#include "corrwght.tab"
#include "gmed_n.h"
#include "inv_sqrt.h"
#include "vad.h"
#include "calc_cor.h"
#include "hp_max.h"

/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/
/*************************************************************************
*
*  FUNCTION:  Lag_max
*
*  PURPOSE: Find the lag that has maximum correlation of scal_sig[] in a
*           given delay range.
*
*  DESCRIPTION:
*      The correlation is given by
*           cor[t] = <scal_sig[n],scal_sig[n-t]>,  t=lag_min,...,lag_max
*      The functions outputs the maximum correlation after normalization
*      and the corresponding lag.
*
*************************************************************************/
static Word16 Lag_max ( /* o : lag found                               */
					   vadState *vadSt,    /* i/o : VAD state struct                      */
					   Word32 corr[],      /* i   : correlation vector.                   */
					   Word16 scal_sig[],  /* i : scaled signal.                          */
					   Word16 L_frame,     /* i : length of frame to compute pitch        */
					   Word16 lag_max,     /* i : maximum lag                             */
					   Word16 lag_min,     /* i : minimum lag                             */
					   Word16 old_lag,     /* i : old open-loop lag                       */
					   Word16 *cor_max,    /* o : normalized correlation of selected lag  */
					   Word16 wght_flg,    /* i : is weighting function used              */ 
					   Word16 *gain_flg,   /* o : open-loop flag                          */
					   Flag dtx            /* i   : dtx flag; use dtx=1, do not use dtx=0 */
					   )
{
	Word32 i, j;
	Word16 *p, *p1;
	Word32 max, t0;
	Word16 t0_h, t0_l;
	Word16 p_max;
	const Word16 *ww, *we;
	Word32 t1,tw;
	ww = &corrweight[250];                                
	we = &corrweight[123 + lag_max - old_lag];             
	max = MIN_32;                                         
	p_max = lag_max;                                       
	for (i = lag_max; i >= lag_min; i--)
	{
		t0 = corr[-i];                                        
		/* Weighting of the correlation function.   */
		t0_h = corr[-i] >> 16;
		t0_l = (corr[-i] & 0xffff) >> 1;

		//L_Extract (corr[-i], &t0_h, &t0_l);
		tw = *ww;
		t0 = Mpy_32_16 (t0_h, t0_l, tw);
		ww--;                                               
		if (wght_flg > 0) {
			/* Weight the neighbourhood of the old lag. */
			t0_h = t0 >> 16;
			t0_l = (t0 & 0xffff) >> 1;
			//L_Extract (t0, &t0_h, &t0_l);
			t0 = Mpy_32_16 (t0_h, t0_l, *we);
			we--;                                          
		}
		if (t0 >= max)
		{
			max = t0;                                      
			p_max = i;                                       
		}
	}
	p  = &scal_sig[0];                                    
	p1 = &scal_sig[-p_max];                                
	t0 = 0;                                               
	t1 = 0;                                               
	for (j = 0; j < L_frame; j++, p++, p1++)
	{
		t0 += (*p * *p1) <<1;               
		t1 += (*p1 * *p1) <<1;
	}
	if (dtx)
	{  /* no test() call since this if is only in simulation env */
#ifdef VAD2
		vadSt->L_Rmax = L_add(vadSt->L_Rmax, t0);   /* Save max correlation */
		vadSt->L_R0 =   L_add(vadSt->L_R0, t1);        /* Save max energy */
#else
		/* update and detect tone */
		voAMRNBEnc_vad_tone_detection_update (vadSt, 0);
		voAMRNBEnc_vad_tone_detection (vadSt, t0, t1);
#endif
	}
	/* gain flag is set according to the open_loop gain */
	/* is t2/t1 > 0.4 ? */    
	*gain_flg = vo_round(L_msu(t0, vo_round(t1), 13107));       
	*cor_max = 0;                                       
	return (p_max);
}

/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
/*************************************************************************
*
*  Function:   voAMRNBEnc_p_ol_wgh_init
*  Purpose:    Allocates state memory and initializes state memory
*
**************************************************************************
*/
int voAMRNBEnc_p_ol_wgh_init (pitchOLWghtState **state, VO_MEM_OPERATOR *pMemOP)
{
	pitchOLWghtState* s;
	if (state == (pitchOLWghtState **) NULL){
		return -1;
	}
	*state = NULL;
	/* allocate memory */
	if ((s= (pitchOLWghtState *)voAMRNBEnc_mem_malloc(pMemOP, sizeof(pitchOLWghtState), 32)) == NULL){
		return -1;
	}
	voAMRNBEnc_p_ol_wgh_reset(s);
	*state = s;
	return 0;
}

/*************************************************************************
*
*  Function:   voAMRNBEnc_p_ol_wgh_reset
*  Purpose:    Initializes state memory to zero
*
**************************************************************************
*/
int voAMRNBEnc_p_ol_wgh_reset (pitchOLWghtState *st)
{
	if (st == (pitchOLWghtState *) NULL){
		return -1;
	}
	/* Reset pitch search states */
	st->old_T0_med = 40;
	st->ada_w = 0;
	st->wght_flg = 0; 

	return 0;
}

/*************************************************************************
*
*  Function:   voAMRNBEnc_p_ol_wgh_exit
*  Purpose:    The memory used for state memory is freed
*
**************************************************************************
*/
void voAMRNBEnc_p_ol_wgh_exit (pitchOLWghtState **state, VO_MEM_OPERATOR *pMemOP)
{
	if (state == NULL || *state == NULL)
		return;
	/* deallocate memory */
	voAMRNBEnc_mem_free(pMemOP, *state);
	*state = NULL;

	return;
}

/*************************************************************************
*
*  Function:   p_ol_wgh
*  Purpose:    open-loop pitch search with weighting
*
**************************************************************************
*/

Word16 voAMRNBEnc_Pitch_ol_wgh (     /* o   : open loop pitch lag                            */
					 pitchOLWghtState *st, /* i/o : State struct                                   */
					 vadState *vadSt,      /* i/o : VAD state struct                               */
					 Word16 signal[],      /* i   : signal used to compute the open loop pitch     */
					 /*       signal[-pit_max] to signal[-1] should be known */
					 Word16 pit_min,       /* i   : minimum pitch lag                              */
					 Word16 pit_max,       /* i   : maximum pitch lag                              */
					 Word16 L_frame,       /* i   : length of frame to compute pitch               */
					 Word16 old_lags[],    /* i   : history with old stored Cl lags                */
					 Word16 ol_gain_flg[], /* i   : OL gain flag                                   */
					 Word16 idx,           /* i   : index                                          */
					 Flag dtx              /* i   : dtx flag; use dtx=1, do not use dtx=0          */
					 )
{
	Word32 i;
	Word16 max1;
	Word16 p_max1;
	Word32 t0;
#if !DISABLE_DTX
#ifndef VAD2
	Word16 corr_hp_max;
#endif
#endif
	Word32 *corr_ptr;    
	/* Scaled signal */
    Word32 corr[PIT_MAX+1];
    Word16 scaled_signal[PIT_MAX + L_FRAME];
	Word16 *scal_sig;
	scal_sig = &scaled_signal[pit_max]; 
	t0 = 0L;  
	for (i = -pit_max; i < L_frame; i++)
	{
		t0 = L_mac (t0, signal[i], signal[i]);
	}

	/*--------------------------------------------------------*
	* Scaling of input signal.                               *
	*                                                        *
	*   if Overflow        -> scal_sig[i] = signal[i]>>2     *
	*   else if t0 < 1^22  -> scal_sig[i] = signal[i]<<2     *
	*   else               -> scal_sig[i] = signal[i]        *
	*--------------------------------------------------------*/

	/*--------------------------------------------------------*
	*  Verification for risk of overflow.                    *
	*--------------------------------------------------------*/

	if ((t0 == MAX_32))               /* Test for overflow */
	{
		for (i = -pit_max; i < L_frame; i++)
		{
			scal_sig[i] = signal[i] >> 3; 
		}
	}
	else if(t0 < (Word32) 1048576L)
	{
		for (i = -pit_max; i < L_frame; i++)
		{
			scal_sig[i] = shl2 (signal[i], 3); 
		}
	}
	else
	{
		for (i = -pit_max; i < L_frame; i++)
		{
			scal_sig[i] = signal[i]; 
		}
	}
	/* calculate all coreelations of scal_sig, from pit_min to pit_max */
	corr_ptr = &corr[pit_max]; 
	voAMRNBEnc_comp_corr (scal_sig, L_frame, pit_max, pit_min, corr_ptr); 
	p_max1 = Lag_max (vadSt, corr_ptr, scal_sig, L_frame, pit_max, pit_min,
		st->old_T0_med, &max1, st->wght_flg, &ol_gain_flg[idx], dtx);
	if (ol_gain_flg[idx] > 0)
	{
		/* Calculate 5-point median of previous lags */
		for (i = 4; i > 0; i--) /* Shift buffer */
		{
			old_lags[i] = old_lags[i-1]; 
		}
		old_lags[0] = p_max1;  
		st->old_T0_med = voAMRNBEnc_gmed_n (old_lags, 5);
		st->ada_w = 32767;                                      /* Q15 = 1.0 */
	}	
	else
	{        
		st->old_T0_med = p_max1;                    
		st->ada_w = mult(st->ada_w, 29491);                    /* = ada_w = ada_w * 0.9 */
	}
	if(st->ada_w < 9830)  /* ada_w - 0.3 */
	{ 
		st->wght_flg = 0;                           
	} 
	else
	{
		st->wght_flg = 1;                           
	}
#if !DISABLE_DTX
#ifndef VAD2
	if (dtx)
	{  /* no test() call since this if is only in simulation env */
		if (sub(idx, 1) == 0)
		{
			/* calculate max high-passed filtered correlation of all lags */
			voAMRNBEnc_hp_max (corr_ptr, scal_sig, L_frame, pit_max, pit_min, &corr_hp_max); 
			/* update complex background detector */
			voAMRNBEnc_vad_complex_detection_update(vadSt, corr_hp_max); 
		}
	}
#endif
#endif    
	return (p_max1);
}

