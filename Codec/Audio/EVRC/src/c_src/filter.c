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
/*      Stack/Local RAM:    0                                           */
/*----------------------------------------------------------------------*/
#include "macro.h"
#include <stdio.h>
#include "basic_op.h"

/*****************************************************************************
** FUNCTION NAME: iir
** 
** CLASSIFICATION: FOR OFFICIAL USE ONLY
**
** DESCRIPTION:
**
** PARAMETERS-
**   INPUT:
**        input - ptr to long array, input data in the following form:
**              (scaled down by 32767) 
**              <------------ 32 bits ----------->
**              +---------------+----------------+
**              |S INTEGER VALUE|FRACTIONAL VALUE| 
**              +---------------+----------------+
**           MSB--^  RADIX PT.--^           LSB--^
**
**        coef - ptr to long array of coefficeints in long word fractional form.
**
**        IIRmemory - ptr to long array, data in the following form:
**              (scaled down by 32767) 
**              <------------ 32 bits ----------->
**              +---------------+----------------+
**              |S INTEGER VALUE|FRACTIONAL VALUE| 
**              +---------------+----------------+
**           MSB--^  RADIX PT.--^           LSB--^
**
**      order - 
**
**      length - subframe size.
**
**   OUTPUT:
**        output - ptr to long array, output data in the following form:
**              (scaled down by 32767) 
**              <------------ 32 bits ----------->
**              +---------------+----------------+
**              |S INTEGER VALUE|FRACTIONAL VALUE| 
**              +---------------+----------------+
**           MSB--^  RADIX PT.--^           LSB--^
**
**
** CHANGE HISTORY:
** DATE        ENGINEER 	COMMENT
** --------------------------------------------------------------------------
** 12-04-95    V. Maan   	Creation
**
**
****************************************************************************/
void iir(
			short *output,
			short *input,
			short *coef,
			short *IIRmemory,
			short order,
			short length
)
{
#if (FUNC_IIR_OPTIMIZE)
	int i,sum;
	short vout;

	for (i = 0; i < length; i++)
	{
	    sum = input[i] << 12;
		sum -= coef[9] * IIRmemory[9];
		IIRmemory[9] = IIRmemory[8];
		sum -= coef[8] * IIRmemory[8];
		IIRmemory[8] = IIRmemory[7];
		sum -= coef[7] * IIRmemory[7];
		IIRmemory[7] = IIRmemory[6];
		sum -= coef[6] * IIRmemory[6];
		IIRmemory[6] = IIRmemory[5];
        sum -= coef[5] * IIRmemory[5];
		IIRmemory[5] = IIRmemory[4];
		sum -= coef[4] * IIRmemory[4];
		IIRmemory[4] = IIRmemory[3];
		sum -= coef[3] * IIRmemory[3];
		IIRmemory[3] = IIRmemory[2];
		sum -= coef[2] * IIRmemory[2];
		IIRmemory[2] = IIRmemory[1];
		sum -= coef[1] * IIRmemory[1];
		IIRmemory[1] = IIRmemory[0];
		sum -= coef[0] * IIRmemory[0];

        vout = (short)(((sum << 4) + 0x00008000L) >> 16);

		IIRmemory[0] = vout;
		output[i] = vout;		
	}
#else
/*===========================================================================
                         LOCAL VARIABLE DECLARATIONS
===========================================================================*/

	register short i, j;
	long SUM;

/*===========================================================================
                         START C CODE 
===========================================================================*/
	for (i = 0; i < length; i++)
	{
		SUM = L_shr(L_deposit_h(input[i]), 3);
		for (j = order - 1; j > 0; j--)
		{
			SUM = L_msu(SUM, coef[j], IIRmemory[j]);
			IIRmemory[j] = IIRmemory[j - 1];
		}
		SUM = L_msu(SUM, coef[0], IIRmemory[0]);
		SUM = L_shl(SUM, 3);
		IIRmemory[0] = round32(SUM);
		output[i] = round32(SUM);
	}
#endif
}								/* END MODULE iir */

/*****************************************************************************
** FUNCTION NAME: fir
** 
** CLASSIFICATION: FOR OFFICIAL USE ONLY
**
** DESCRIPTION:
**
** PARAMETERS-
**   INPUT:
**        input - ptr to long array, input data in the following form:
**              (scaled down by 32767) 
**              <------------ 32 bits ----------->
**              +---------------+----------------+
**              |S INTEGER VALUE|FRACTIONAL VALUE| 
**              +---------------+----------------+
**           MSB--^  RADIX PT.--^           LSB--^
**
**        coef - ptr to long array of coefficeints in long word fractional form.
**
**        FIRmemory - ptr to long array, data in the following form:
**              (scaled down by 32767) 
**              <------------ 32 bits ----------->
**              +---------------+----------------+
**              |S INTEGER VALUE|FRACTIONAL VALUE| 
**              +---------------+----------------+
**           MSB--^  RADIX PT.--^           LSB--^
**
**      order - 
**
**      length - subframe size.
**
**   OUTPUT:
**        output - ptr to long array, output data in the following form:
**              (scaled down by 32767) 
**              <------------ 32 bits ----------->
**              +---------------+----------------+
**              |S INTEGER VALUE|FRACTIONAL VALUE| 
**              +---------------+----------------+
**           MSB--^  RADIX PT.--^           LSB--^
**
**
** CHANGE HISTORY:
** DATE        ENGINEER 	COMMENT
** --------------------------------------------------------------------------
** 12-04-95    V. Maan   	Creation
**
**
****************************************************************************/
void fir(
			short *output,		/* int.frac */
			short *input,		/* int.frac */
			short *coef,		/* frac */
			short *FIRmemory,	/* int */
			short order,
			short length
)
{
#if (FUNC_FIR_OPTIMIZE)
	int i;
	int sum;

	for (i = 0; i < length; i++)
	{
		sum = input[i] << 12;
		sum += coef[9] * FIRmemory[9];
		FIRmemory[9] = FIRmemory[8];
		sum += coef[8] * FIRmemory[8];
		FIRmemory[8] = FIRmemory[7];
		sum += coef[7] * FIRmemory[7];
		FIRmemory[7] = FIRmemory[6];
		sum += coef[6] * FIRmemory[6];
		FIRmemory[6] = FIRmemory[5];
		sum += coef[5] * FIRmemory[5];
		FIRmemory[5] = FIRmemory[4];
		sum += coef[4] * FIRmemory[4];
		FIRmemory[4] = FIRmemory[3];
		sum += coef[3] * FIRmemory[3];
		FIRmemory[3] = FIRmemory[2];
		sum += coef[2] * FIRmemory[2];
		FIRmemory[2] = FIRmemory[1];
		sum += coef[1] * FIRmemory[1];
		FIRmemory[1] = FIRmemory[0];
		sum += coef[0] * FIRmemory[0];
		FIRmemory[0] = input[i];
		sum <<= 4;
		output[i] = (short)((sum + 0x00008000) >> 16);
	}

#else

	register short i, j;
	long SUM;

	for (i = 0; i < length; i++)
	{
		SUM = L_shr(L_deposit_h(input[i]), 3);
		for (j = order - 1; j > 0; j--)
		{
			SUM = L_mac(SUM, coef[j], FIRmemory[j]);
			FIRmemory[j] = FIRmemory[j - 1];
		}
		SUM = L_mac(SUM, coef[0], FIRmemory[0]);
		FIRmemory[0] = input[i];
		output[i] = round32(L_shl(SUM, 3));
	}
#endif
}
