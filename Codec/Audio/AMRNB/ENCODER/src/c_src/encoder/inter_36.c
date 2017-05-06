/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : inter_36.c
*      Purpose          : Interpolating the normalized correlation
*                       : with 1/3 or 1/6 resolution.
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "inter_36.h"
const char inter_36_id[] = "@(#)$Id $" inter_36_h;

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
#define UP_SAMP_MAX  6

#include "inter_36.tab"

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  FUNCTION:  Interpol_3or6()
*
*  PURPOSE:  Interpolating the normalized correlation with 1/3 or 1/6
*            resolution.
*
*************************************************************************/
Word16 voAMRNBEnc_Interpol_3or6 (  /* o : interpolated value                        */
					  Word16 *x,          /* i : input vector                              */
					  Word16 frac,        /* i : fraction  (-2..2 for 3*, -3..3 for 6*)    */
					  Word16 flag3        /* i : if set, upsampling rate = 3 (6 otherwise) */
					  )
{
	Word16       *x1, *x2;
	const Word16 *c1, *c2;
	Word32 s;

	if (flag3 != 0)
	{
		frac =  (frac<< 1);   /* inter_3[k] = inter_6[2*k] -> k' = 2*k */
	}
	if (frac < 0)
	{
		frac = (frac+ UP_SAMP_MAX);
		x--;
	}
	x1 = &x[0];                         
	x2 = &x[1];                         
	c1 = &inter_6[frac];                
	c2 = &inter_6[ (UP_SAMP_MAX- frac)]; 

	s = (x1[0]*c1[0] <<1);
	s = s + (x2[0]*c2[0] <<1);
	s = s + (x1[-1]*c1[6] <<1);
	s = s + (x2[-1]*c2[6] <<1);
	s = s + (x1[-2]*c1[12] <<1);
	s = s + (x2[-2]*c2[12] <<1);
	s = s + (x1[-3]*c1[18] <<1);
	s = s + (x2[-3]*c2[18] <<1);

	return (s+0x00008000)>>16;//round (s);
}
