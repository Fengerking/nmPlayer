/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : g_pitch.c
*      Purpose          : Compute the pitch (adaptive codebook) gain.
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "g_pitch.h"
const char g_pitch_id[] = "@(#)$Id $" g_pitch_h;

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "mode.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "cnst.h"

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  FUNCTION:  G_pitch
*
*  PURPOSE:  Compute the pitch (adaptive codebook) gain.
*            Result in Q14 (NOTE: 12.2 bit exact using Q12) 
*
*  DESCRIPTION:
*      The adaptive codebook gain is given by
*
*              g = <x[], y[]> / <y[], y[]>
*
*      where x[] is the target vector, y[] is the filtered adaptive
*      codevector, and <> denotes dot product.
*      The gain is limited to the range [0,1.2] (=0..19661 Q14)
*
*************************************************************************/
Word16 voAMRNBEnc_G_pitch(    /* o : Gain of pitch lag saturated to 1.2       */
					enum Mode mode,     /* i : AMR mode                                 */
					Word16 xn[],        /* i : Pitch target.                            */
					Word16 y1[],        /* i : Filtered adaptive codebook.              */
					Word16 g_coeff[],   /* i : Correlations need for gain quantization  */
					Word16 L_subfr      /* i : Length of subframe.                      */
					)
{
	Word32  i;
	Word32  xy, yy, exp_xy, exp_yy, gain;
	Word32  s, temp;

	/* Compute scalar product <xn[],y1[]> */             
	s = 1L;                         
	for (i = 0; i < L_SUBFR; i++)
	{
		temp = (xn[i] * y1[i])<<1;
		s = L_add(s , temp);      //s= s + xn[i]*y1[i]
	}
	{
		exp_xy = norm_l (s);
		xy = vo_round (s << exp_xy);
	}
	/* If (xy < 4) gain = 0 */
	if (xy < 4)
		return ((Word16) 0);

	s = 1L;      
	for (i = 0; i < L_SUBFR; i++)
	{
		temp = (y1[i] * y1[i])<<1;
		s = L_add(s, temp);
	}     
	{
		exp_yy = norm_l (s);
		yy = vo_round (s << exp_yy);
	}

	g_coeff[0] = yy;                
	g_coeff[1] = 15 - exp_yy;   
	g_coeff[2] = xy;                 
	g_coeff[3] = 15 - exp_xy;   

	/* compute gain = xy/yy */
	xy =  (xy>> 1);                  /* Be sure xy < yy */
	gain = div_s (xy, yy);
	i = (exp_xy - exp_yy);      /* Denormalization of division */        
	gain = shr (gain, i);

	/* if(gain >1.2) gain = 1.2 */
	if (gain > 19661)
	{
		gain = 19661;                  
	}
	if (mode == MR122)
	{
		/* clear 2 LSBits */
		gain = gain & 0xfffC;          
	}
	return (gain);   
}
