/***********************************************************************
*                                                                      *
*         VisualOn, Inc. Confidential and Proprietary, 2003-2010       *
*                                                                      *
************************************************************************/
/***********************************************************************
*      File: g_pitch.c                                                 *
*                                                                      *
*	   Description:Compute the gain of pitch. Result in Q12            *
*	               if(gain < 0) gain = 0                               *
*				   if(gain > 1.2) gain = 1.2                           *
************************************************************************/

#include "typedef.h"
#include "basic_op.h"
#include "math_op.h"

Word16 G_pitch(                            /* (o) Q14 : Gain of pitch lag saturated to 1.2   */
		Word16 xn[],                          /* (i)     : Pitch target.                        */
		Word16 y1[],                          /* (i)     : filtered adaptive codebook.          */
		Word16 g_coeff[],                     /* : Correlations need for gain quantization.     */
		Word16 L_subfr                        /* : Length of subframe.                          */
	      )
{
	Word32 i;
	Word16 xy, yy, exp_xy, exp_yy, gain;
	/* Compute scalar product <y1[],y1[]> */
#ifdef ASM_OPT                  /* asm optimization branch */
	/* Compute scalar product <xn[],y1[]> */
	xy = extract_h(voAMRWBEncDot_product12_asm(xn, y1, L_subfr, &exp_xy));
	yy = extract_h(voAMRWBEncDot_product12_asm(y1, y1, L_subfr, &exp_yy));

#else
	/* Compute scalar product <xn[],y1[]> */
	xy = extract_h(Dot_product12(xn, y1, L_subfr, &exp_xy));
	yy = extract_h(Dot_product12(y1, y1, L_subfr, &exp_yy));

#endif

	g_coeff[0] = yy;                       
	g_coeff[1] = exp_yy;                   
	g_coeff[2] = xy;                       
	g_coeff[3] = exp_xy;                   

	/* If (xy < 0) gain = 0 */
	if (xy < 0)
		return ((Word16) 0);

	/* compute gain = xy/yy */

	xy >>= 1;                       /* Be sure xy < yy */
	gain = div_s(xy, yy);

	i = exp_xy;
	i -= exp_yy;

	gain = shl(gain, i);                   

	/* if (gain > 1.2) gain = 1.2  in Q14 */
	if(gain > 19661)
	{
		gain = 19661;                      
	}
	return (gain);
}



