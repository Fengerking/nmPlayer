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
/*  Module:     impulser.c                                              */
/*  Memory Usage:                                                       */
/*      ROM:                0                                           */
/*      Static/Global RAM:  0                                           */
/*      Stack/Local RAM:    23                                          */
/*----------------------------------------------------------------------*/

/************************************************************************
* Routine name: ImpulseR                                                *
* Function: Calculates impulse response.                                *
************************************************************************/
#include "macro.h"
#include "basic_op.h"
#include "proto.h"

const short GAMMA11[10] ={
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
#if 1 //C_OPT
void ImpulseRzp1(
				   short *output,
				   short *coef_uq,
				   short *coef,
				   short length
)
{
	register int i;
	int LSUM, tmp;
	short memory[ORDER];
	short wcoef[ORDER];
	int L_temp;
	short s_temp;
	const short *sp, *sp1;

	/* ImpulseR of 1/A(Z) */
	output[0] = 0x0800;
    memory[0] = 0x0800;
	for (i = 1; i < 10; i++)
	{
		memory[i] = 0;
	}
	for (i = 1; i < length; i++)
	{
		LSUM  = (memory[9] * coef[9]);
		memory[9] = memory[8];
		LSUM += memory[8] * coef[8];
		memory[8] = memory[7];
		LSUM += memory[7] * coef[7];
		memory[7] = memory[6];
		LSUM += memory[6] * coef[6];
		memory[6] = memory[5];
		LSUM += memory[5] * coef[5];
		memory[5] = memory[4];
		LSUM += memory[4] * coef[4];
		memory[4] = memory[3];
		LSUM += memory[3] * coef[3];
		memory[3] = memory[2];
		LSUM += memory[2] * coef[2];
		memory[2] = memory[1];
		LSUM += memory[1] * coef[1];
		memory[1] = memory[0];
		LSUM += memory[0] * coef[0];
		L_temp = (-LSUM) << 4;
		*memory = (L_temp + 0x8000) >> 16;		
		output[i] = (L_temp + 0x8000) >> 16;
	}    
	sp = GAMMA11;
	sp1 = coef_uq;
	memory[0] = 0x0800;
	wcoef[0] = ((*sp++) * (*sp1++) + 0x4000) >> 15;
	for (i = 1; i < 10; i++)
	{
		memory[i] = 0;
        wcoef[i] = ((*sp++) * (*sp1++) + 0x4000) >> 15;
	}
	for (i = 1; i < length; i++)
	{
		LSUM = output[i] << 12;
		LSUM  += memory[9] * wcoef[9];
		memory[9] = memory[8];
		LSUM  += memory[8] * wcoef[8];
		memory[8] = memory[7];
		LSUM  += memory[7] * wcoef[7];
		memory[7] = memory[6];
		LSUM  += memory[6] * wcoef[6];
		memory[6] = memory[5];
		LSUM  += memory[5] * wcoef[5];
		memory[5] = memory[4];
		LSUM  += memory[4] * wcoef[4];
		memory[4] = memory[3];
		LSUM  += memory[3] * wcoef[3];
		memory[3] = memory[2];
		LSUM  += memory[2] * wcoef[2];
		memory[2] = memory[1];
		LSUM  += memory[1] * wcoef[1];
		memory[1] = memory[0];

		LSUM += memory[0] * wcoef[0];
		*memory = output[i];
		L_temp = LSUM << 4;
		output[i] = (L_temp + 0x8000) >> 16;
	}

	sp1 = coef_uq;
	memory[0] = 0x0800;
    wcoef[0] = (((*sp1++)<<14) + 0x4000) >> 15;
	for (i = 1; i < 10; i++)
	{
		memory[i] = 0;
        wcoef[i] = (((*sp1++)<<(14-i)) + 0x4000) >> 15;
	}
	for (i = 1; i < length; i++)
	{
		LSUM = output[i] << 12;
		tmp = memory[9] * wcoef[9];
		memory[9] = memory[8];
		tmp += memory[8] * wcoef[8];
		memory[8] = memory[7];
		tmp += memory[7] * wcoef[7];
		memory[7] = memory[6];
		tmp += memory[6] * wcoef[6];
		memory[6] = memory[5];
		tmp += memory[5] * wcoef[5];
		memory[5] = memory[4];
		tmp += memory[4] * wcoef[4];
		memory[4] = memory[3];
		tmp += memory[3] * wcoef[3];
		memory[3] = memory[2];
		tmp += memory[2] * wcoef[2];
		memory[2] = memory[1];
		tmp += memory[1] * wcoef[1];
		memory[1] = memory[0];
		tmp += memory[0] * wcoef[0];
		LSUM -= tmp;
		L_temp = LSUM << 4;
		s_temp = (L_temp + 0x8000) >> 16;
		*memory   = s_temp;
        output[i] = s_temp;
	}
}

void ImpulseRzp2(
				   short *output,
				   short *coef_uq,
				   short *coef,
				   short length
)
{
	register int i;
	int LSUM, tmp;
	short memory[ORDER];
	int L_temp;
	short s_temp;

	/* ImpulseR of 1/A(Z) */
	output[0] = 0x0800;
    memory[0] = 0x0800;
	for (i = 1; i < 10; i++)
	{
		memory[i] = 0;
	}
	for (i = 1; i < length; i++)
	{
		LSUM  = (memory[9] * coef[9]);
		memory[9] = memory[8];
		LSUM += memory[8] * coef[8];
		memory[8] = memory[7];
		LSUM += memory[7] * coef[7];
		memory[7] = memory[6];
		LSUM += memory[6] * coef[6];
		memory[6] = memory[5];
		LSUM += memory[5] * coef[5];
		memory[5] = memory[4];
		LSUM += memory[4] * coef[4];
		memory[4] = memory[3];
		LSUM += memory[3] * coef[3];
		memory[3] = memory[2];
		LSUM += memory[2] * coef[2];
		memory[2] = memory[1];
		LSUM += memory[1] * coef[1];
		memory[1] = memory[0];
		LSUM += memory[0] * coef[0];
		L_temp = (-LSUM) << 4;
		output[i] = (L_temp + 0x8000) >> 16;
		*memory = output[i];	

	}    

	memory[0] = 0x0800;
	for (i = 1; i < 10; i++)
	{
		memory[i] = 0;
	}
	for (i = 1; i < length; i++)
	{
		LSUM = output[i] << 12;
		LSUM  += memory[9] * coef_uq[9];
		memory[9] = memory[8];
		LSUM  += memory[8] * coef_uq[8];
		memory[8] = memory[7];
		LSUM  += memory[7] * coef_uq[7];
		memory[7] = memory[6];
		LSUM  += memory[6] * coef_uq[6];
		memory[6] = memory[5];
		LSUM  += memory[5] * coef_uq[5];
		memory[5] = memory[4];
		LSUM  += memory[4] * coef_uq[4];
		memory[4] = memory[3];
		LSUM  += memory[3] * coef_uq[3];
		memory[3] = memory[2];
		LSUM  += memory[2] * coef_uq[2];
		memory[2] = memory[1];
		LSUM  += memory[1] * coef_uq[1];
		memory[1] = memory[0];

		LSUM += memory[0] * coef_uq[0];
		*memory = output[i];
		L_temp = LSUM << 4;
		output[i] = (L_temp + 0x8000) >> 16;
	}

	memory[0] = 0x0800;
	for (i = 1; i < 10; i++)
	{
		memory[i] = 0;
	}
	for (i = 1; i < length; i++)
	{
		LSUM = output[i] << 12;
		tmp = memory[9] * coef_uq[9];
		memory[9] = memory[8];
		tmp += memory[8] * coef_uq[8];
		memory[8] = memory[7];
		tmp += memory[7] * coef_uq[7];
		memory[7] = memory[6];
		tmp += memory[6] * coef_uq[6];
		memory[6] = memory[5];
		tmp += memory[5] * coef_uq[5];
		memory[5] = memory[4];
		tmp += memory[4] * coef_uq[4];
		memory[4] = memory[3];
		tmp += memory[3] * coef_uq[3];
		memory[3] = memory[2];
		tmp += memory[2] * coef_uq[2];
		memory[2] = memory[1];
		tmp += memory[1] * coef_uq[1];
		memory[1] = memory[0];
		tmp += memory[0] * coef_uq[0];
		LSUM -= tmp;
		L_temp = LSUM << 4;
		s_temp = (L_temp + 0x8000) >> 16;
		*memory   = s_temp;
        output[i] = s_temp;
	}
}
#endif 

void ImpulseRzp(
				   short *output,
				   short *coef_uq,
				   short *coef,
				   short gamma1,
				   short gamma2,
				   short order,
				   short length
)
{
#if (FUNC_IMPULSERZP_OPTIMIZE)
	register int i;
	int LSUM, tmp;
	short memory[ORDER];
	short wcoef[ORDER];
	int L_temp;
	short s_temp;

	/* ImpulseR of 1/A(Z) */
	output[0] = 0x0800;
    memory[0] = 0x0800;
	for (i = 1; i < order; i++)
	{
		memory[i] = 0;
	}
	for (i = 1; i < length; i++)
	{
		LSUM  = (memory[9] * coef[9]);
		memory[9] = memory[8];
		LSUM += memory[8] * coef[8];
		memory[8] = memory[7];
		LSUM += memory[7] * coef[7];
		memory[7] = memory[6];
		LSUM += memory[6] * coef[6];
		memory[6] = memory[5];
		LSUM += memory[5] * coef[5];
		memory[5] = memory[4];
		LSUM += memory[4] * coef[4];
		memory[4] = memory[3];
		LSUM += memory[3] * coef[3];
		memory[3] = memory[2];
		LSUM += memory[2] * coef[2];
		memory[2] = memory[1];
		LSUM += memory[1] * coef[1];
		memory[1] = memory[0];
		LSUM += memory[0] * coef[0];
		L_temp = (-LSUM) << 4;
		*memory = (L_temp + 0x8000) >> 16;
		
		output[i] = (short)((L_shl2((-LSUM), 4) + 0x8000) >> 16);
	}

	/* Cascade with ImpulseR of A(Z/gamma1) */
	weight(wcoef, coef_uq, gamma1, order);

	memory[0] = 0x0800;
	for (i = 1; i < order; i++)
		memory[i] = 0;
	for (i = 1; i < length; i++)
	{
		LSUM = output[i] << 12;
		LSUM  += memory[9] * wcoef[9];
		memory[9] = memory[8];
		LSUM  += memory[8] * wcoef[8];
		memory[8] = memory[7];
		LSUM  += memory[7] * wcoef[7];
		memory[7] = memory[6];
		LSUM  += memory[6] * wcoef[6];
		memory[6] = memory[5];
		LSUM  += memory[5] * wcoef[5];
		memory[5] = memory[4];
		LSUM  += memory[4] * wcoef[4];
		memory[4] = memory[3];
		LSUM  += memory[3] * wcoef[3];
		memory[3] = memory[2];
		LSUM  += memory[2] * wcoef[2];
		memory[2] = memory[1];
		LSUM  += memory[1] * wcoef[1];
		memory[1] = memory[0];

		LSUM += memory[0] * wcoef[0];
		*memory = output[i];
		L_temp = LSUM << 4;
		output[i] = (L_temp + 0x8000) >> 16;
	}

	/* Cascade with ImpulseR of 1/A(Z/gamma2) */
	weight(wcoef, coef_uq, gamma2, order);

	memory[0] = 0x0800;
	for (i = 1; i < order; i++)
		memory[i] = 0;
	for (i = 1; i < length; i++)
	{
		LSUM = output[i] << 12;
		tmp = memory[9] * wcoef[9];
		memory[9] = memory[8];
		tmp += memory[8] * wcoef[8];
		memory[8] = memory[7];
		tmp += memory[7] * wcoef[7];
		memory[7] = memory[6];
		tmp += memory[6] * wcoef[6];
		memory[6] = memory[5];
		tmp += memory[5] * wcoef[5];
		memory[5] = memory[4];
		tmp += memory[4] * wcoef[4];
		memory[4] = memory[3];
		tmp += memory[3] * wcoef[3];
		memory[3] = memory[2];
		tmp += memory[2] * wcoef[2];
		memory[2] = memory[1];
		tmp += memory[1] * wcoef[1];
		memory[1] = memory[0];
		tmp += memory[0] * wcoef[0];
		LSUM -= tmp;
		L_temp = LSUM << 4;
		s_temp = (L_temp + 0x8000) >> 16;
		*memory   = s_temp;
        output[i] = s_temp;
	}

#else

	register short i, j;
	long LSUM;
	short memory[ORDER];
	short wcoef[ORDER];

	/* ImpulseR of 1/A(Z) */
	output[0] = 0x0800;
    memory[0] = 0x0800;
	for (i = 1; i < order; i++)
		memory[i] = 0;
	for (i = 1; i < length; i++)
	{
		for (j = order - 1, LSUM = 0; j > 0; j--)
		{
			LSUM = L_msu(LSUM, memory[j], coef[j]);
			memory[j] = memory[j - 1];
		}
		LSUM = L_msu(LSUM, memory[0], coef[0]);
		*memory = round32(L_shl(LSUM, 3));
		output[i] = round32(L_shl(LSUM, 3));
	}

	/* Cascade with ImpulseR of A(Z/gamma1) */
	weight(wcoef, coef_uq, gamma1, order);

	memory[0] = 0x0800;
	for (i = 1; i < order; i++)
		memory[i] = 0;
	for (i = 1; i < length; i++)
	{
		LSUM = L_deposit_h(output[i]);
		LSUM = L_shr(LSUM, 3);
		for (j = order - 1; j > 0; j--)
		{
			LSUM = L_mac(LSUM, memory[j], wcoef[j]);
			memory[j] = memory[j - 1];
		}
		LSUM = L_mac(LSUM, memory[0], wcoef[0]);
		*memory = output[i];
		output[i] = round32(L_shl(LSUM, 3));
	}

	/* Cascade with ImpulseR of 1/A(Z/gamma2) */
	weight(wcoef, coef_uq, gamma2, order);

	memory[0] = 0x0800;
	for (i = 1; i < order; i++)
		memory[i] = 0;
	for (i = 1; i < length; i++)
	{
		LSUM = L_deposit_h(output[i]);
		LSUM = L_shr(LSUM, 3);
		for (j = order - 1; j > 0; j--)
		{
			LSUM = L_msu(LSUM, memory[j], wcoef[j]);
			memory[j] = memory[j - 1];
		}
		LSUM = L_msu(LSUM, memory[0], wcoef[0]);
		*memory = round32(L_shl(LSUM, 3));
        output[i] = round32(L_shl(LSUM, 3));
	}
#endif
}
