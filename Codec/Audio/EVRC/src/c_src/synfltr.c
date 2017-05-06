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
/*  Module:     synfltr.c                                               */
/*  Memory Usage:                                                       */
/*      ROM:                0                                           */
/*      Static/Global RAM:  0                                           */
/*      Stack/Local RAM:    0                                           */
/*----------------------------------------------------------------------*/

/************************************************************************
* Routine name: SynthesisFilter                                         *
* Function: Synthesis filter.                                           *
* Inputs: input - input excitation (or residual signal).                *
*         coef  - interpolated LPC coefficients.                        *
*         memory - filter's memory.                                     *
*         pcQ   - Q value of predicition coefficients.                  *
*         order - filter order.                                         *
*         length - input/output buffers length.                         *
* Output: output - filter output.                                       *
************************************************************************/
#include "macro.h"
#include <stdio.h>

#include "basic_op.h"
//#include "mathevrc.h"
//#include "mathdp31.h"
//#include "mathadv.h"

void SynthesisFilter(
						short *output,
						short *input,
						short *coef,
						short *memory,
						short order,
						short length
)
{
#if (FUNC_SYNTHESISFILTER_OPT)
	long acc;	
	short chen_temp_s;

	/* iir filter for each subframe */
	while (length-- != 0)
	{
		acc = (*input++) << 12;
        acc -= memory[9] * coef[9];
		memory[9] = memory[8];
		acc -= memory[8] * coef[8];
		memory[8] = memory[7];
        acc -= memory[7] * coef[7];
		memory[7] = memory[6];
		acc -= memory[6] * coef[6];
		memory[6] = memory[5];
		acc -= memory[5] * coef[5];
		memory[5] = memory[4];
		acc -= memory[4] * coef[4];
		memory[4] = memory[3];
		acc -= memory[3] * coef[3];
		memory[3] = memory[2];
		acc -= memory[2] * coef[2];
		memory[2] = memory[1];
		acc -= memory[1] * coef[1];
		memory[1] = memory[0];

		//for (j = order - 1; j > 0; j--)
		//{
		//	acc -= memory[j] * coef[j];
		//	memory[j] = memory[j - 1];
		//}
		acc -= memory[0] * coef[0];
		if (acc > (long)0x07ffffffL)
	    {
	        chen_temp_s = MAX_16;
	    }
	    else if (acc < (long)0xf8000000L)
	    {
	        chen_temp_s = (short)((acc + 0x8000) >> 16);
	    }
	    else
	    {
	        chen_temp_s = (short)((acc + 0x800) >> 12);
	    }
		*output++ = chen_temp_s;
		memory[0] = chen_temp_s;
	}
	
#else

	register short i, j;
	long acc;

	/* iir filter for each subframe */
	for (i = 0; i < length; i++)
	{
		acc = L_deposit_h(*input++);
		acc = L_shr(acc, 3);

		for (j = order - 1; j > 0; j--)
		{
			/* acc = L_sub(acc, L_mult(memory[j], coef[j])); */
			acc = L_msu(acc, memory[j], coef[j]);
			memory[j] = memory[j - 1];
		}
		/* acc = L_sub(acc, L_mult(memory[0], coef[0])); */
		acc = L_msu(acc, memory[0], coef[0]);
		acc = L_shl(acc, 3);

		*output++ = round32(acc);
		memory[0] = round32(acc);
	}
#endif
}
