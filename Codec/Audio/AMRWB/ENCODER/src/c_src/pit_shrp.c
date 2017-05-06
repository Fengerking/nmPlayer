/***********************************************************************
*                                                                      *
*          VisualOn, Inc. Confidential and Proprietary, 2003-2010      *
*                                                                      *
************************************************************************/
/***********************************************************************
*      File: pit_shrp.c                                                *
*                                                                      *
*	   Description: Performs Pitch sharpening routine                  *
*                                                                      *
************************************************************************/

#include "typedef.h"
#include "basic_op.h"

void Pit_shrp(
		Word16 * x,                           /* in/out: impulse response (or algebraic code) */
		Word16 pit_lag,                       /* input : pitch lag                            */
		Word16 sharp,                         /* input : pitch sharpening factor (Q15)        */
		Word16 L_subfr                        /* input : subframe size                        */
	     )
{
	Word32 i;
	Word32 L_tmp;
	Word16 *x_ptr = x + pit_lag;

	for (i = pit_lag; i < L_subfr; i++)
	{
		L_tmp = (*x_ptr << 15);
		L_tmp += *x++ * sharp;
		*x_ptr++ = ((L_tmp + 0x4000)>>15);
	}

	return;
}



