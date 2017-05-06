/*
********************************************************************************
*
*      GSM AMR speech codec   Version 7.1.0   July 26, 1999
*
********************************************************************************
*
*      File             : pitch_ol.c
*      Purpose          : Compute the open loop pitch lag.
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "pitch_ol.h"
const char pitch_ol_id[] = "@(#)$Id $" pitch_ol_h;
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "cnst.h"
#include "inv_sqrt.h"
#include "vad.h"
#include "calc_cor.h"
#include "hp_max.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#define THRESHOLD 27853
/*
********************************************************************************
*                         LOCAL PROGRAM CODE
********************************************************************************
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
#ifdef VAD2
static Word16 Lag_max ( /* o   : lag found                               */
					   Word32 corr[],      /* i   : correlation vector.                     */
					   Word16 scal_sig[],  /* i   : scaled signal.                          */    
					   Word16 scal_fac,    /* i   : scaled signal factor.                   */
					   Word16 scal_flag,   /* i   : if 1 use EFR compatible scaling         */
					   Word16 L_frame,     /* i   : length of frame to compute pitch        */
					   Word16 lag_max,     /* i   : maximum lag                             */
					   Word16 lag_min,     /* i   : minimum lag                             */
					   Word16 *cor_max,    /* o   : normalized correlation of selected lag  */
					   Word32 *rmax,       /* o   : max(<s[i]*s[j]>)                        */
					   Word32 *r0,         /* o   : residual energy                         */
					   Flag dtx            /* i   : dtx flag; use dtx=1, do not use dtx=0   */
					   )
#else
static Word16 Lag_max ( /* o   : lag found                               */
					   vadState *vadSt,    /* i/o : VAD state struct                        */
					   Word32 corr[],      /* i   : correlation vector.                     */
					   Word16 scal_sig[],  /* i   : scaled signal.                          */    
					   Word16 scal_fac,    /* i   : scaled signal factor.                   */
					   Word16 scal_flag,   /* i   : if 1 use EFR compatible scaling         */
					   Word16 L_frame,     /* i   : length of frame to compute pitch        */
					   Word16 lag_max,     /* i   : maximum lag                             */
					   Word16 lag_min,     /* i   : minimum lag                             */
					   Word16 *cor_max,    /* o   : normalized correlation of selected lag  */
					   Flag dtx            /* i   : dtx flag; use dtx=1, do not use dtx=0   */
					   )
#endif
{
	Word16 i, j;
	Word16 *p;
	Word32 max, t0;
	Word16 max_h, max_l, ener_h, ener_l;
	Word16 p_max = 0; /* initialization only needed to keep gcc silent */
	max = MIN_32;  
	p_max = lag_max;

	for (i = lag_max, j = (PIT_MAX-lag_max-1); i >= lag_min; i--, j--)  
	{
		if (corr[-i] >= max) 
		{ 
			max = corr[-i]; 
			p_max = i;        
		} 
	}   
	/* compute energy */
	t0 = 0;  
#ifdef ARMv7_OPT
	p = &scal_sig[-p_max];  	
	t0 = sum_armv7_xx(t0, p, p, L_frame);
#else
	p = &scal_sig[-p_max]; 
	for (i = 0; i < L_frame; i++, p++)
	{
		t0 += ((*p) * (*p))<<1;
	}
#endif
	/* 1/sqrt(energy) */
	if (dtx)
	{  /* no test() call since this if is only in simulation env */
#ifdef VAD2
		*rmax = max;	
		*r0 = t0;	
#else
		/* check tone */
		voAMRNBEnc_vad_tone_detection (vadSt, max, t0);
#endif
	}
	t0 = Inv_sqrt (t0); /* function result */
	if (scal_flag)
	{
		t0 = t0 << 1;
	}  
	/* max = max/sqrt(energy)  */
	max_h = max >> 16;
	max_l = (max - (max_h<<16))>>1;

	ener_h = t0 >> 16;
	ener_l = (t0 - (ener_h << 16))>>1;

	t0 = (max_h * ener_h << 1) + (((max_h * ener_l>>15) + (max_l * ener_h>>15))<<1);

	if (scal_flag)
	{
		t0 = L_shr2(t0, scal_fac);
		*cor_max = extract_h (L_shl2(t0, 15)); /* divide by 2 */
	}
	else
	{
		*cor_max = extract_l(t0);
	}
	return (p_max);
}

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  FUNCTION:  Pitch_ol
*
*  PURPOSE: Compute the open loop pitch lag.
*
*  DESCRIPTION:
*      The open-loop pitch lag is determined based on the perceptually
*      weighted speech signal. This is done in the following steps:
*        - find three maxima of the correlation <sw[n],sw[n-T]>,
*          dividing the search range into three parts:
*               pit_min ... 2*pit_min-1
*             2*pit_min ... 4*pit_min-1
*             4*pit_min ...   pit_max
*        - divide each maximum by <sw[n-t], sw[n-t]> where t is the delay at
*          that maximum correlation.
*        - select the delay of maximum normalized correlation (among the
*          three candidates) while favoring the lower delay ranges.
*
*************************************************************************/
Word16 voAMRNBEnc_Pitch_ol(      /* o   : open loop pitch lag                         */
				 vadState *vadSt,   /* i/o : VAD state struct                            */
				 enum Mode mode,    /* i   : coder mode                                  */
				 Word16 signal[],   /* i   : signal used to compute the open loop pitch  */
				 /*    signal[-pit_max] to signal[-1] should be known */
				 Word16 pit_min,    /* i   : minimum pitch lag                           */
				 Word16 pit_max,    /* i   : maximum pitch lag                           */
				 Word16 L_frame,    /* i   : length of frame to compute pitch            */
				 Word16 idx,        /* i   : frame index                                 */
				 Flag dtx           /* i   : dtx flag; use dtx=1, do not use dtx=0       */
				 )
{
	Word16 i, j;
	Word16 max1, max2, max3;
	Word16 p_max1, p_max2, p_max3;
	Word16 scal_flag = 0;
	Word32 t0, temp;
	Word32 corr[PIT_MAX+1], *corr_ptr;
	/* Scaled signal */
	Word16 scaled_signal[L_FRAME + PIT_MAX];
	Word16 *scal_sig, scal_fac;

	scal_sig = &scaled_signal[pit_max];
	t0 = 0L;                           
	for (i = - pit_max; i < L_frame; i++)
	{
		temp = (signal[i] * signal[i])<<1;
		t0 = L_add(t0, temp);
	}

	/*--------------------------------------------------------*
	* Scaling of input signal.                               *
	*                                                        *
	*   if Overflow        -> scal_sig[i] = signal[i]>>3     *
	*   else if t0 < 1^20  -> scal_sig[i] = signal[i]<<3     *
	*   else               -> scal_sig[i] = signal[i]        *
	*--------------------------------------------------------*/

	/*--------------------------------------------------------*
	*  Verification for risk of overflow.                    *
	*--------------------------------------------------------*/

	if (t0 == MAX_32)                                   /* Test for overflow */
	{
		for (i = - pit_max; i < L_frame; i++)
		{
			scal_sig[i] = signal[i] >> 3;  
		}
		scal_fac = 3;                       
	}
	else if (t0 < (Word32) 1048576L)
		/* if (t0 < 2^20) */
	{
		for (i = - pit_max; i < L_frame; i++)
		{
			scal_sig[i] = signal[i] << 3;
		}
		scal_fac = -3;     
	}
	else
	{
		for (i = - pit_max; i < L_frame; i++)
		{
			scal_sig[i] = signal[i];
		}
		scal_fac = 0; 
	}

	/* calculate all coreelations of scal_sig, from pit_min to pit_max */
	corr_ptr = &corr[pit_max]; 

#ifdef ARM
	comp_corr_asm(scal_sig, pit_max, pit_min, corr_ptr); 
#else
	voAMRNBEnc_comp_corr (scal_sig, L_frame, pit_max, pit_min, corr_ptr); 
#endif

	/*--------------------------------------------------------------------*
	*  The pitch lag search is divided in three sections.                *
	*  Each section cannot have a pitch multiple.                        *
	*  We find a maximum for each section.                               *
	*  We compare the maximum of each section by favoring small lags.    *
	*                                                                    *
	*  First section:  lag delay = pit_max     downto 4*pit_min          *
	*  Second section: lag delay = 4*pit_min-1 downto 2*pit_min          *
	*  Third section:  lag delay = 2*pit_min-1 downto pit_min            *
	*--------------------------------------------------------------------*/

	/* mode dependent scaling in Lag_max */
	if ((mode == MR122))
	{
		scal_flag = 1;                         
	}
	else
	{
		scal_flag = 0;                        
	} 
	j = pit_min << 2;

	p_max1 = Lag_max (vadSt, corr_ptr, scal_sig, scal_fac, scal_flag, L_frame,
		pit_max, j, &max1, dtx); // move16 (); /* function result */

	i = (j - 1);
	j = pit_min << 1;
	p_max2 = Lag_max (vadSt, corr_ptr, scal_sig, scal_fac, scal_flag, L_frame,
		i, j, &max2, dtx);        //move16 (); /* function result */

	i = (j - 1);
	p_max3 = Lag_max (vadSt, corr_ptr, scal_sig, scal_fac, scal_flag, L_frame,
		i, pit_min, &max3, dtx); // move16 (); /* function result */

	/*--------------------------------------------------------------------*
	* Compare the 3 sections maximum, and favor small lag.               *
	*--------------------------------------------------------------------*/

	if ((max1 * THRESHOLD>>15) < max2)
	{
		max1 = max2;                   
		p_max1 = p_max2;                  
	}

	if ((max1 * THRESHOLD>>15) < max3)
	{
		p_max1 = p_max3;                 
	}
	return (p_max1);
}

