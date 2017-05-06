/*********************************************************************
* Copyright 2003-2009 by VisualOn Software, Inc.
* All modifications are confidential and proprietary information
* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
**********************************************************************
***************************** Change History**************************
* 
*    DD/MMM/YYYY     Code Ver     Description             Author
*    -----------     --------     -----------             ------
*    17-12-2009        1.0        File imported from      Huaping Liu
*                                             
**********************************************************************/
#ifndef __VOSBR_GHDR_H__
#define __VOSBR_GHDR_H__

#include "voType.h"

#define FBITS_LPCOEFS	29	/* Q29 for range of (-4, 4) */
#define MAG_16			(16 * (1 << (32 - (2*(32-FBITS_LPCOEFS)))))		/* i.e. 16 in Q26 format */
#define RELAX_COEF		0x7ffff79c	/* 1.0 / (1.0 + 1e-6), Q31 */

#define  DSPint64  VO_S64

//Q31*Q31--->Q31
static __inline  int vo_Multi32(int var1, int var2)
{
	DSPint64   acc;
	acc = (DSPint64)var1 * (DSPint64)var2;
	acc >>= 32;
	acc <<= 1;

	return (int)acc;
}

static __inline int vo_norm(int var1)
{
	int  var_out;
	if(var1 == 0)
		return 31;
	else
	{
		if(var1 != 0xFFFFFFFF)
		{
			if(var1 < 0)
			{
				var1 = ~var1;
			}
		}
		for(var_out = 0; var1 < 0x40000000; var_out++)
			var1 <<= 1;
	}

	return var_out;
}

/* do y <<= n, clipping to range [-2^30, 2^30 - 1] (i.e. output has one guard bit) */

static __inline int L_shl(int var, int n)
{
	int sign = var >> 31;
	if(sign != (var >> (30 - n)))
		var = sign ^ 0x3fffffff;
	else
		var <<= n;

	return  var;
}

/* const float newBWTab[4][4] = {
*         {0.0,  0.6,  0.9,  0.98},
*         {0.6,  0.75, 0.9,  0.98},
*         {0.0,  0.75, 0.9,  0.98},
*         {0.0,  0.75, 0.9,  0.98},
*};
*/

//newBWTab[prev invfMode][curr invfMode], format = Q31 (table 4.156) 
static const int vo_newBWTab[4][4] = {
	{0x00000000, 0x4ccccccd, 0x73333333, 0x7d70a3d7},
	{0x4ccccccd, 0x60000000, 0x73333333, 0x7d70a3d7},
	{0x00000000, 0x60000000, 0x73333333, 0x7d70a3d7},
	{0x00000000, 0x60000000, 0x73333333, 0x7d70a3d7},
};

/* vo_invBandTab[i] = 1.0 / (i + 1), Q31 */
static const int vo_invBandTab[64] = {
	2147483647, 1073741824,  715827883,  536870912,  429496730,  357913941,  306783378,  268435456, 
	238609294,  214748365,  195225786,  178956971,  165191050,  153391689,  143165577,  134217728, 
	126322568,  119304647,  113025455,  107374182,  102261126,   97612893,   93368854,   89478485, 
	85899346,   82595525,   79536431,   76695845,   74051160,   71582788,   69273666,   67108864, 
	65075262,   63161284,   61356676,   59652324,   58040099,   56512728,   55063683,   53687091, 
	52377650,   51130563,   49941480,   48806447,   47721859,   46684427,   45691141,   44739243, 
	43826197,   42949673,   42107523,   41297762,   40518559,   39768216,   39045157,   38347922, 
	37675152,   37025580,   36398028,   35791394,   35204650,   34636833,   34087042,   33554432, 
};

#define SBR_GBOOST_MAX	0x2830afd3	/* Q28, 1.584893192 squared */
#define	SBR_ACC_SCALE	6

/* squared version of table in 4.6.18.7.5 */
static const int vo_limGainTab[4] = {0x20138ca7, 
                                     0x40000000, 
                                     0x7fb27dce, 
                                     0x80000000};	/* Q30 (0x80000000 = sentinel for GMAX) */

/* static const int vo_hSmoothCoef[5] = {
*              0.33333333333333
*              0.30150283239582
*              0.21816949906249
*              0.11516383427084
*              0.03183050093751
*};
* */

/* hSmooth table from 4.7.18.7.6, format = Q31 */
static const int vo_hSmoothCoef[MAX_NUM_SMOOTH_COEFS] = {
	0x2aaaaaab, 
	0x2697a512, 
	0x1becfa68, 
	0x0ebdb043, 
	0x04130598, 
};

#endif   //__VOSBR_GHDR_H__



