/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : residu.c
*      Purpose          : Computes the LP residual.
*      Description      : The LP residual is computed by filtering the input
*                       : speech through the LP inverse filter A(z).
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "residu.h"
const char residu_id[] = "@(#)$Id $" residu_h;
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "cnst.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
/*
*--------------------------------------*
* Constants (defined in cnst.h         *
*--------------------------------------*
*  M         : LPC order               *
*--------------------------------------*
*/

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
void voAMRNBEnc_Residu(
			 Word16 a[], /* (i)     : prediction coefficients                      */
			 Word16 x[], /* (i)     : speech signal                                */
			 Word16 y[], /* (o)     : residual signal                              */
			 Word16 lg   /* (i)     : size of filtering                            */
			 )
#ifdef C_OPT  //bit match .have about 40ms improvment
{
	int    i;
	Word32 s;
	Word16 *tmpA, *tmpX;
	for (i = 0; i < lg; i++)
	{
		tmpA = a;
		tmpX = x+i;
		s =  (*(tmpA++))* (*(tmpX--));
		s += (*(tmpA++))* (*(tmpX--));
		s += (*(tmpA++))* (*(tmpX--));
		s += (*(tmpA++))* (*(tmpX--));
		s += (*(tmpA++))* (*(tmpX--));
		s += (*(tmpA++))* (*(tmpX--));
		s += (*(tmpA++))* (*(tmpX--));
		s += (*(tmpA++))* (*(tmpX--));
		s += (*(tmpA++))* (*(tmpX--));
		s += (*(tmpA++))* (*(tmpX--));
		s += (*(tmpA))* (*(tmpX));
		s = L_shl2 (s, 4);
		y[i] = vo_round (s);
	}
	return;
}
#else
{
	nativeInt i, j;
	Word32 s;
	for (i = 0; i < lg; i++)
	{
		s = L_mult (x[i], a[0]);
		for (j = 1; j <= M; j++)
		{
			s = L_mac (s, a[j], x[i - j]);
		}
		s = L_shl2 (s, 3);
		y[i] = vo_round (s);
	}
	return;
}
#endif
