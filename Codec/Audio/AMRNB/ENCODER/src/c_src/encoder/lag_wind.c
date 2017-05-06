/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : lag_wind.c
*      Purpose          : Lag windowing of autocorrelations.
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "lag_wind.h"
const char lag_wind_id[] = "@(#)$Id $" lag_wind_h;

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "cnst.h" 
/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#include "lag_wind.tab"     /* Table for Lag_Window() */

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  FUNCTION:  Lag_window()
*
*  PURPOSE:  Lag windowing of autocorrelations.
*
*  DESCRIPTION:
*         r[i] = r[i]*lag_wind[i],   i=1,...,10
*
*     r[i] and lag_wind[i] are in special double precision format.
*     See "oper_32b.c" for the format.
*
*************************************************************************/
void voAMRNBEnc_Lag_window (
				 Word16 m,           /* (i)     : LPC order                        */
				 Word16 r_h[],       /* (i/o)   : Autocorrelations  (msb)          */
				 Word16 r_l[]        /* (i/o)   : Autocorrelations  (lsb)          */
)
{
	nativeInt  i;
	Word32 x;
	for (i = 1; i <= M; i++)
	{
		//Mpy_32 (r_h[i], r_l[i], lag_h[i - 1], lag_l[i - 1]);
		x = (r_h[i]*lag_h[i - 1]<<1) +(((r_h[i]*lag_l[i - 1]>>15) + (r_l[i]* lag_h[i - 1]>>15))<<1);

		r_h[i] = x>>16;
		r_l[i]  = (x - ((Word32)r_h[i]<<16))>>1;
	}
}
