/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : pred_lt.c
*      Purpose          : Compute the result of long term prediction
*
********************************************************************************
*/


/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "pred_lt.h"
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
#define L_INTER10    10//(L_INTERPOL-1)
#define FIR_SIZE     (UP_SAMP_MAX*L_INTER10+1)
#define HALF_L_INTER10  5
/* 1/6 resolution interpolation filter  (-3 dB at 3600 Hz) */
/* Note: the 1/3 resolution filter is simply a subsampled
*       version of the 1/6 resolution filter, i.e. it uses
*       every second coefficient:
*       
*          inter_3l[k] = inter_6[2*k], 0 <= k <= 3*L_INTER10
*/
Word16 inter_6[FIR_SIZE] =
{
	29443,
	28346, 25207, 20449, 14701, 8693, 3143,
	-1352, -4402, -5865, -5850, -4673, -2783,
	-672, 1211, 2536, 3130, 2991, 2259,
	1170, 0, -1001, -1652, -1868, -1666,
	-1147, -464, 218, 756, 1060, 1099,
	904, 550, 135, -245, -514, -634,
	-602, -451, -231, 0, 191, 308,
	340, 296, 198, 78, -36, -120,
	-163, -165, -132, -79, -19, 34,
	73, 91, 89, 70, 38, 0
};
Word16 inter_61[10] = {3143,-2783,2259,-1666,1099,-634,308,-120,34,0};
/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  FUNCTION:   Pred_lt_3or6()
*
*  PURPOSE:  Compute the result of long term prediction with fractional
*            interpolation of resolution 1/3 or 1/6. (Interpolated past
*            excitation).
*
*  DESCRIPTION:
*       The past excitation signal at the given delay is interpolated at
*       the given fraction to build the adaptive codebook excitation.
*       On return exc[0..L_subfr-1] contains the interpolated signal
*       (adaptive codebook excitation).
*
*************************************************************************/
void voAMRNBEnc_Pred_lt_3or6(
				   Word16 exc[],     /* in/out: excitation buffer                         */
				   Word16 T0,        /* input : integer pitch lag                         */
				   Word16 frac,      /* input : fraction of lag                           */
				   Word16 L_subfr,   /* input : subframe size                             */
				   Word16 flag3      /* input : if set, upsampling rate = 3 (6 otherwise) */
				   )
#ifndef C_OPT_N   // not bit match  
{
	Word32 j,frac2;
	Word16 *x0, *x1, *x2, *x3, *x4;
	const Word16 *c1, *c2;
	Word32 s, s1, s2, s3;

	x0 = &exc[-T0];            

	frac = -(frac);

	if (flag3 != 0)
	{
		frac =  (frac>> 1);   /* inter_3l[k] = inter_6[2*k] -> k' = 2*k */
	}
	if (frac < 0)
	{
		frac =  (frac + UP_SAMP_MAX);
		x0--;
	}
	frac2 = (UP_SAMP_MAX - frac);

	c1 = &inter_6[frac];
	c2 = &inter_6[frac2];

	for (j = L_SUBFR; j != 0; j-=4)
	{
		x1 = x0++;              //move16 (); 
		x2 = x0++;
		x3 = x0++;
		x4 = x0++;

		s  = (x1[0] * c1[0]);//L_mac (s, x1[-i], c1[k]);
		s1 = (x2[0] * c1[0]);//L_mac (s, x1[-i], c1[k]);
		s2 = (x3[0] * c1[0]);//L_mac (s, x1[-i], c1[k]);
		s3 = (x4[0] * c1[0]);//L_mac (s, x1[-i], c1[k]);

		s  += (x1[1] * c2[0]);//L_mac (s, x2[i], c2[k]);
		s1 += (x2[1] * c2[0]);//L_mac (s, x2[i], c2[k]);
		s2 += (x3[1] * c2[0]);//L_mac (s, x2[i], c2[k]);
		s3 += (x4[1] * c2[0]);//L_mac (s, x2[i], c2[k]);

		*exc++ = (s+0x00004000)>>15;//round (s); 
		*exc++ = (s1+0x00004000)>>15;//round (s); 
		*exc++ = (s2+0x00004000)>>15;//round (s); 
		*exc++ = (s3+0x00004000)>>15;//round (s); 
	}
	return;
}
#else
{
	Word16 j, k;
	Word16 *x0, *x1, *x2;
	const Word16 *c1, *c2;
	Word32 s;
	x0 = &exc[-T0];        
	frac = -(frac);
	if (flag3 != 0)
	{
		frac = (frac << 1);   /* inter_3l[k] = inter_6[2*k] -> k' = 2*k */
	}

	if (frac < 0)
	{
		frac = (frac + UP_SAMP_MAX);
		x0--;
	}
	c1 = &inter_6[frac];
	c2 = &inter_6[UP_SAMP_MAX - frac];
	for (j = 0; j < L_subfr; j++)
	{
		x1 = x0++;          
		x2 = x0;              
        k = 0;

        s  = ((x1[0] * c1[k]));
		s += ((x2[0]  *  c2[k]));
		k += UP_SAMP_MAX;

		s += ((x1[-1] * c1[k]));
		s += ((x2[1]  *  c2[k]));
		k += UP_SAMP_MAX;

		s += ((x1[-2] * c1[k]));
		s += ((x2[2]  *  c2[k]));
		k += UP_SAMP_MAX;

		s += ((x1[-3] * c1[k]));
		s += ((x2[3]  *  c2[k]));
		k += UP_SAMP_MAX;

		s += ((x1[-4] * c1[k]));
		s += ((x2[4]  *  c2[k]));
		k += UP_SAMP_MAX;

		s += ((x1[-5] * c1[k]));
		s += ((x2[5]  *  c2[k]));
		k += UP_SAMP_MAX;

		s += ((x1[-6] * c1[k]));
		s += ((x2[6]  *  c2[k]));
		k += UP_SAMP_MAX;

		s += ((x1[-7] * c1[k]));
		s += ((x2[7]  *  c2[k]));
		k += UP_SAMP_MAX;

		s += ((x1[-8] * c1[k]));
		s += ((x2[8]  *  c2[k]));
		k += UP_SAMP_MAX;

		s += ((x1[-9] * c1[k]));
		s += ((x2[9]  *  c2[k]));

		exc[j] = ((s<<1) + 0x8000)>>16;    
	}
	return;
}
#endif
