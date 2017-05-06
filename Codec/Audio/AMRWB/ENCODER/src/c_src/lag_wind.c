/***********************************************************************
*                                                                      *
*         VisualOn, Inc. Confidential and Proprietary, 2003-2010       *
*                                                                      *
************************************************************************/
/***********************************************************************
*      File: lag_wind.c                                                *
*                                                                      *
*	   Description: Lag_windows on autocorrelations                    *
*	                r[i] *= lag_wind[i]                                *
*                                                                      *
************************************************************************/

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "lag_wind.tab"


void Lag_window(
		Word16 r_h[],                         /* (i/o)   : Autocorrelations  (msb)          */
		Word16 r_l[]                          /* (i/o)   : Autocorrelations  (lsb)          */
	       )
{
	Word32 i;
	Word32 x;

	for (i = 1; i <= M; i++)
	{
		x = voMpy_32(r_h[i], r_l[i], lag_h[i - 1], lag_l[i - 1]);
		r_h[i] = x >> 16;
		r_l[i] = (x & 0xffff)>>1;
	}
	return;
}



