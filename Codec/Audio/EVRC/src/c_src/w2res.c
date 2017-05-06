/**********************************************************************
Each of the companies; Lucent, Motorola, Nokia, and Qualcomm (hereinafter 
referred to individually as "Source" or collectively as "Sources") do 
hereby state:

To the extent to which the Source(s) may legally and freely do so, the 
Source(s), upon submission of a Contribution, grant(s) a free, 
irrevocable, non-exclusive, license to the Third Generation Partnership 
Project 2 (3GPP2) and its Organizational Partners: ARIB, CCSA, TIA, TTA, 
and TTC, under the Source's copyright or copyright license rights in the 
Contribution, to, in whole or in part, copy, make derivative works, 
perform, display and distribute the Contribution and derivative works 
thereof consistent with 3GPP2's and each Organizational Partner's 
policies and procedures, with the right to (i) sublicense the foregoing 
rights consistent with 3GPP2's and each Organizational Partner's  policies 
and procedures and (ii) copyright and sell, if applicable) in 3GPP2's name 
or each Organizational Partner's name any 3GPP2 or transposed Publication 
even though this Publication may contain the Contribution or a derivative 
work thereof.  The Contribution shall disclose any known limitations on 
the Source's rights to license as herein provided.

When a Contribution is submitted by the Source(s) to assist the 
formulating groups of 3GPP2 or any of its Organizational Partners, it 
is proposed to the Committee as a basis for discussion and is not to 
be construed as a binding proposal on the Source(s).  The Source(s) 
specifically reserve(s) the right to amend or modify the material 
contained in the Contribution. Nothing contained in the Contribution 
shall, except as herein expressly provided, be construed as conferring 
by implication, estoppel or otherwise, any license or right under (i) 
any existing or later issuing patent, whether or not the use of 
information in the document necessarily employs an invention of any 
existing or later issued patent, (ii) any copyright, (iii) any 
trademark, or (iv) any other intellectual property right.

With respect to the Software necessary for the practice of any or 
all Normative portions of the Enhanced Variable Rate Codec (EVRC) as 
it exists on the date of submittal of this form, should the EVRC be 
approved as a Specification or Report by 3GPP2, or as a transposed 
Standard by any of the 3GPP2's Organizational Partners, the Source(s) 
state(s) that a worldwide license to reproduce, use and distribute the 
Software, the license rights to which are held by the Source(s), will 
be made available to applicants under terms and conditions that are 
reasonable and non-discriminatory, which may include monetary compensation, 
and only to the extent necessary for the practice of any or all of the 
Normative portions of the EVRC or the field of use of practice of the 
EVRC Specification, Report, or Standard.  The statement contained above 
is irrevocable and shall be binding upon the Source(s).  In the event 
the rights of the Source(s) in and to copyright or copyright license 
rights subject to such commitment are assigned or transferred, the 
Source(s) shall notify the assignee or transferee of the existence of 
such commitments.
*******************************************************************/
 
/*======================================================================*/
/*     Enhanced Variable Rate Codec - Bit-Exact C Specification         */
/*     Copyright (C) 1997-1998 Telecommunications Industry Association. */
/*     All rights reserved.                                             */
/*----------------------------------------------------------------------*/
/* Note:  Reproduction and use of this software for the design and      */
/*     development of North American Wideband CDMA Digital              */
/*     Cellular Telephony Standards is authorized by the TIA.           */
/*     The TIA does not authorize the use of this software for any      */
/*     other purpose.                                                   */
/*                                                                      */
/*     The availability of this software does not provide any license   */
/*     by implication, estoppel, or otherwise under any patent rights   */
/*     of TIA member companies or others covering any use of the        */
/*     contents herein.                                                 */
/*                                                                      */
/*     Any copies of this software or derivative works must include     */
/*     this and all other proprietary notices.                          */
/*======================================================================*/
/*  Memory Usage:                                                       */
/*      ROM:                0                                           */
/*      Static/Global RAM:  0                                           */
/*      Stack/Local RAM:    22                                          */
/*----------------------------------------------------------------------*/

#include "macro.h"
#include <stdio.h>

#include "basic_op.h"
#include "proto.h"
//#include "mathevrc.h"
//#include "mathdp31.h"
//#include "mathadv.h"
#if 1  //C_OPT
const short GAMMA13[10] ={
    	29491,
	    26542,
    	23888,
    	21499,
	    19349,
	    17414,
	    15673,
	    14106,
	    12695,
	    11425
};
void Weight2Res(
				   short *output,
				   short *input,
				   short *coef_uq,
				   short *coef,
				   short gamma1,
				   short gamma2,
				   short order,
				   short length
)
{
	register int i;
	short memory[ORDER];	
	short wcoef[ORDER];
	int   sum;
	const short *sp, *sp1;
	/* A(Z) */
	for (i = 1; i < ORDER; i++)
	{
		memory[i] = 0;
	}
	memory[0] = input[0];
	output[0] = input[0];

	for (i = 1; i < length; i++)
	{
		sum  = memory[9] * coef[9];
		memory[9] = memory[8];
		sum += memory[8] * coef[8];
		memory[8] = memory[7];
		sum += memory[7] * coef[7];
		memory[7] = memory[6];
		sum += memory[6] * coef[6];
		memory[6] = memory[5];
		sum += memory[5] * coef[5];
		memory[5] = memory[4];
		sum += memory[4] * coef[4];
		memory[4] = memory[3];
		sum += memory[3] * coef[3];
		memory[3] = memory[2];
		sum += memory[2] * coef[2];
		memory[2] = memory[1];
		sum += memory[1] * coef[1];
		memory[1] = memory[0];

		sum += memory[0] * coef[0];
		sum = ((long)input[i] << 16) + (sum << 4);
		*memory = input[i];
		output[i] = (sum + 0x8000) >> 16;
	}
	sp = GAMMA13;
	sp1 = coef_uq;
	wcoef[0] = ((*sp++) * (*sp1++) + 0x4000) >> 15;
	for (i = 1; i < 10; i++)
	{
		memory[i] = 0;
        wcoef[i] = ((*sp++) * (*sp1++) + 0x4000) >> 15;
	}
	memory[0] = output[0];
	for (i = 1; i < length; i++)
	{
	    sum = (long)output[i] << 15;
		sum -= memory[9] * wcoef[9];
		memory[9] = memory[8];
		sum -= memory[8] * wcoef[8];
		memory[8] = memory[7];
		sum -= memory[7] * wcoef[7];
		memory[7] = memory[6];
		sum -= memory[6] * wcoef[6];
		memory[6] = memory[5];
		sum -= memory[5] * wcoef[5];
		memory[5] = memory[4];
		sum -= memory[4] * wcoef[4];
		memory[4] = memory[3];
		sum -= memory[3] * wcoef[3];
        memory[3] = memory[2];
		sum -= memory[2] * wcoef[2];
        memory[2] = memory[1];
		sum -= memory[1] * wcoef[1];
		memory[1] = memory[0];

		sum -= memory[0] * wcoef[0];
		*memory = (sum + 0x4000) >> 15;
		output[i] = (sum + 0x4000) >> 15;
	}

	/* Cascade with of A(Z/gamma2) */
	sp1 = coef_uq;
    wcoef[0] = (((*sp1++)<<14) + 0x4000) >> 15;
	for (i = 1; i < 10; i++)
	{
		memory[i] = 0;
        wcoef[i] = (((*sp1++)<<(14-i)) + 0x4000) >> 15;
	}
	memory[0] = output[0];
	for (i = 1; i < length; i++)
	{
	    sum = (long)output[i] << 15;
		sum += memory[9] * wcoef[9];
		memory[9] = memory[8];
		sum += memory[8] * wcoef[8];
		memory[8] = memory[7];
		sum += memory[7] * wcoef[7];
		memory[7] = memory[6];
		sum += memory[6] * wcoef[6];
		memory[6] = memory[5];
		sum += memory[5] * wcoef[5];
		memory[5] = memory[4];
		sum += memory[4] * wcoef[4];
		memory[4] = memory[3];
		sum += memory[3] * wcoef[3];
		memory[3] = memory[2];
		sum += memory[2] * wcoef[2];
		memory[2] = memory[1];
		sum += memory[1] * wcoef[1];
		memory[1] = memory[0];

		sum += memory[0] * wcoef[0];
		*memory = output[i];
		output[i] = (sum + 0x4000) >> 15;
	}
}
#else
void Weight2Res(
				   short *output,
				   short *input,
				   short *coef_uq,
				   short *coef,
				   short gamma1,
				   short gamma2,
				   short order,
				   short length
)
{
#if (FUNC_WEIGHT2RES_OPT)
	register int i;
	short memory[ORDER];	
	short wcoef[ORDER];
	int   SUM;
	/* A(Z) */
	for (i = 1; i < ORDER; i++)
	{
		memory[i] = 0;
	}
	memory[0] = input[0];
	output[0] = input[0];

	for (i = 1; i < length; i++)
	{
		/* for (j = order - 1, SUM = L_deposit_h(input[i]); j > 0; j--) */
		SUM  = memory[9] * coef[9];
		memory[9] = memory[8];
		SUM += memory[8] * coef[8];
		memory[8] = memory[7];
		SUM += memory[7] * coef[7];
		memory[7] = memory[6];
		SUM += memory[6] * coef[6];
		memory[6] = memory[5];
		SUM += memory[5] * coef[5];
		memory[5] = memory[4];
		SUM += memory[4] * coef[4];
		memory[4] = memory[3];
		SUM += memory[3] * coef[3];
		memory[3] = memory[2];
		SUM += memory[2] * coef[2];
		memory[2] = memory[1];
		SUM += memory[1] * coef[1];
		memory[1] = memory[0];

		//for (j = order - 1, SUM = 0; j > 0; j--)
		//{
		//	/* SUM = L_add(SUM, L_shl(L_mult(memory[j], coef[j]), 3)); */
		//	SUM += memory[j] * coef[j];
		//	memory[j] = memory[j - 1];
		//}
		/* SUM = L_add(SUM, L_shl(L_mult(memory[0], coef[0]), 3)); */
		SUM += memory[0] * coef[0];
		SUM = ((long)input[i] << 16) + (SUM << 4);
		*memory = input[i];
		output[i] = (SUM + 0x8000) >> 16;
	}

	/* Cascade with of 1/A(Z/gamma1) */
	weight(wcoef, coef_uq, gamma1, order);

	for (i = 1; i < ORDER; i++)
	{
		memory[i] = 0;
	}
	memory[0] = output[0];
	for (i = 1; i < length; i++)
	{
	    SUM = (long)output[i] << 15;
		SUM -= memory[9] * wcoef[9];
		memory[9] = memory[8];
		SUM -= memory[8] * wcoef[8];
		memory[8] = memory[7];
		SUM -= memory[7] * wcoef[7];
		memory[7] = memory[6];
		SUM -= memory[6] * wcoef[6];
		memory[6] = memory[5];
		SUM -= memory[5] * wcoef[5];
		memory[5] = memory[4];
		SUM -= memory[4] * wcoef[4];
		memory[4] = memory[3];
		SUM -= memory[3] * wcoef[3];
        memory[3] = memory[2];
		SUM -= memory[2] * wcoef[2];
        memory[2] = memory[1];
		SUM -= memory[1] * wcoef[1];
		memory[1] = memory[0];
		//for (j = order - 1; j > 0; j--)
		//{
		//	SUM -= memory[j] * wcoef[j];
		//	memory[j] = memory[j - 1];
		//}
		SUM -= memory[0] * wcoef[0];
		*memory = (SUM + 0x4000) >> 15;
		output[i] = (SUM + 0x4000) >> 15;
	}

	/* Cascade with of A(Z/gamma2) */
	weight(wcoef, coef_uq, gamma2, order);

	for (i = 1; i < ORDER; i++)
	{
		memory[i] = 0;
	}
	memory[0] = output[0];
	for (i = 1; i < length; i++)
	{
	    SUM = (long)output[i] << 15;
		SUM += memory[9] * wcoef[9];
		memory[9] = memory[8];
		SUM += memory[8] * wcoef[8];
		memory[8] = memory[7];
		SUM += memory[7] * wcoef[7];
		memory[7] = memory[6];
		SUM += memory[6] * wcoef[6];
		memory[6] = memory[5];
		SUM += memory[5] * wcoef[5];
		memory[5] = memory[4];
		SUM += memory[4] * wcoef[4];
		memory[4] = memory[3];
		SUM += memory[3] * wcoef[3];
		memory[3] = memory[2];
		SUM += memory[2] * wcoef[2];
		memory[2] = memory[1];
		SUM += memory[1] * wcoef[1];
		memory[1] = memory[0];
		//for (j = order - 1; j > 0; j--)
		//{
		//	SUM += memory[j] * wcoef[j];
		//	memory[j] = memory[j - 1];
		//}
		SUM += memory[0] * wcoef[0];
		*memory = output[i];
		output[i] = (SUM + 0x4000) >> 15;
	}
	
#else

	register short i, j;
	long SUM;
	short memory[ORDER];
	short wcoef[ORDER];

	/* A(Z) */
	for (i = 1; i < ORDER; i++)
		memory[i] = 0;
	memory[0] = input[0];
	output[0] = input[0];

	for (i = 1; i < length; i++)
	{
		/* for (j = order - 1, SUM = L_deposit_h(input[i]); j > 0; j--) */
		for (j = order - 1, SUM = 0; j > 0; j--)
		{
			/* SUM = L_add(SUM, L_shl(L_mult(memory[j], coef[j]), 3)); */
			SUM = L_mac(SUM, memory[j], coef[j]);	
			memory[j] = memory[j - 1];
		}
		/* SUM = L_add(SUM, L_shl(L_mult(memory[0], coef[0]), 3)); */
		SUM = L_mac(SUM, memory[0], coef[0]);
		SUM = L_add(L_deposit_h(input[i]),L_shl(SUM,3)); 
		*memory = input[i];
		output[i] = round32(SUM); 
	}

	/* Cascade with of 1/A(Z/gamma1) */
	weight(wcoef, coef_uq, gamma1, order);

	for (i = 1; i < ORDER; i++)
		memory[i] = 0;
	memory[0] = output[0];
	for (i = 1; i < length; i++)
	{
		for (j = order - 1, SUM = L_deposit_h(output[i]); j > 0; j--)
		{
			SUM = L_msu(SUM, memory[j], wcoef[j]);
			memory[j] = memory[j - 1];
		}
		SUM = L_msu(SUM, memory[0], wcoef[0]);
		*memory = round32(SUM);
		output[i] = round32(SUM);
	}

	/* Cascade with of A(Z/gamma2) */
	weight(wcoef, coef_uq, gamma2, order);

	for (i = 1; i < ORDER; i++)
		memory[i] = 0;
	memory[0] = output[0];
	for (i = 1; i < length; i++)
	{
		for (j = order - 1, SUM = L_deposit_h(output[i]); j > 0; j--)
		{
			SUM = L_mac(SUM, memory[j], wcoef[j]);
			memory[j] = memory[j - 1];
		}
		SUM = L_mac(SUM, memory[0], wcoef[0]);
		*memory = output[i];
		output[i] = round32(SUM);
	}
#endif
}
#endif
