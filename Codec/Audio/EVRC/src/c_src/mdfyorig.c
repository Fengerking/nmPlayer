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
/*      Static/Global RAM:  24                                          */
/*      Stack/Local RAM:    141                                         */
/*----------------------------------------------------------------------*/
#include <stdio.h>
#include "e_globs.h"
#include "macro.h"
#include "basic_op.h"
#include "math_ext32.h"
#include "proto.h"
//#include "mathadv.h"
//#include "mathdp31.h"
//#include "mathevrc.h"

void modifyorig(ERVC_ENC_OBJ *evrc_encobj,
				short *residualm,
				short *accshift,
				short beta,
				short *dpm,
				short shiftrange,
				short resolution,
				short *TARGET,
				short *residual,
				short dp,
				short sfend)
{
	short best;
	short tmp;
	long ex0y[(RSHIFT * 2 + 2) * RRESOLUTION + 1];	/* Fraction sampled correlation function */
	long ex0y2[RSHIFT * 2 + 2];	/* Integer sampled correlation function */
	short Residual[2 * SubFrameSize + 2 * RSHIFT + 1];
	short *a1;
	short *a2;
	short *a3;
	register int i, j, k, n;
	short sfstart, length, shiftrangel, shiftranger;
	long e01, e02;
	int  shft_fctr1, shft_fctr2, shft_fctr3;
	long ltmp;
	long laccshift;
	long y;

	a1 = evrc_encobj->mdf_a1;
	a2 = evrc_encobj->mdf_a2;
	a3 = evrc_encobj->mdf_a3;

	/********************
	* CORRELATION MATCH *
	********************/
	length = (sfend - dp);
	sfstart = dp;

	/*laccshift = L_shl(L_deposit_h(*accshift), 8); */
	laccshift = L_deposit_h(*accshift);		/* accshift scaled by 8 */
	/* Perform before if
	 * * statement.
	 */
	if (shiftrange != 0)
	{
		/* Limit the search range to control accshift */
		shiftrangel = shiftranger = shiftrange;
		if (*accshift < 0)
			shiftrangel += 1;
		if (*accshift > 0)
			shiftranger += 1;

		tmp = abs_s(*accshift);
		/* For non-periodic signals */
		if ((beta < 6554 && tmp > 15 * 256) || (beta < 9830 && tmp > 30 * 256))
		{
			if (*accshift < 0)
				shiftranger = 1;
			else
				shiftrangel = 1;
		}

		if ((shiftrangel + ((*accshift) >> 8)) > 72)
		{
			shiftrangel = 72 - ((*accshift) >> 8);
		}
		if ((shiftranger - (*accshift >> 8)) > 72)
		{
			shiftranger = (72 + (*accshift >> 8));
		}

		/* Create a buffer of modify residual for match at low cut-off frequency */
		tmp = (length + shiftrangel);
		tmp = (tmp + shiftranger);

		ltmp = L_deposit_h((*accshift + (shiftrangel << 8)));
		for (i = 0; i <= tmp; i++)
		{
			/* POINTER ADDITION NOT CONVERTED -- NEED UNSIGNED ADDITION */
#ifdef ASM_OPT
			bl_intrp_asm(Residual + i, residual + dp + i, ltmp, 16384, 3);
#else
			bl_intrp(Residual + i, residual + dp + i, ltmp, 16384, 3);
#endif
		}

		tmp = shiftrangel + shiftranger;

		/* Search for all integer delays of residual */
		for (n = 0; n <= tmp; n++)
		{
			ex0y2[n] = 0;
			for (i = 0; i < length; i++)
			{
				ex0y2[n] += (Residual[n + i] * TARGET[sfstart + i])<<1;
			}
			ex0y2[n] = (ex0y2[n] >> 1);

		}

		/* Do quadratic interpolation of ex0y */
		for (n = 1, k = 0; n < shiftrangel + shiftranger; n++)
		{
			for (j = 0; j < resolution; j++)
			{
				ex0y[k] = L_mpy_ls(ex0y2[n - 1], a1[j]);
				ltmp = L_mpy_ls(ex0y2[n], a2[j]);
				ex0y[k] += ltmp;
				ltmp = L_mpy_ls(ex0y2[n + 1], a3[j]);
				ex0y[k] += ltmp;
				k++;
			}
		}

		/* Find maximum with positive correlation */
		y = 0;
		best = ((shiftrangel << 3) - 4);
		for (n = 0; n < k; n++)
		{
			if (ex0y[n] > y)
			{
				y = ex0y[n];
				best = n;
			}
		}
/* best value not very accurate since ex0y[] calculation not percise. To correct
 * Residual[] should have more percision. This error does not seem to affect the
 * final output using the test data. 
 */
		/* Calculate energy in selected shift index */
		e01 = e02 = 0;
		for (i = shiftrangel; i < length + shiftrangel; i++)
			e01 += (Residual[i] * Residual[i])<<1;

		for (i = 0; i < length; i++)
			e02 += (TARGET[i + sfstart] * TARGET[i + sfstart])<<1;

		if (e01 == 0 || e02 == 0)
			y = 0;
		else
		{
			shft_fctr1 = norm_l(y);
			y = (y << shft_fctr1);
			y = L_mpy_ll(y, y);
			/* TO RECOVER Y VALUE:
			 * y = L_shl(y, 31-2*shft_fctr1) */

			shft_fctr2 = norm_l(e01);
			e01 = e01 << shft_fctr2;

			shft_fctr3 = norm_l(e02);
			e02 = (e02 << shft_fctr3);
			ltmp = L_mpy_ll(e01, e02);
			ltmp = L_mpy_ls(ltmp, 16056);

			ltmp = L_shl(ltmp, (shft_fctr1 << 1) - ((2 + shft_fctr2) + shft_fctr3));

			if (y > ltmp)
			{
				/*tmp = shl(shr(*accshift,8) + shiftrangel, 3) - (best + 4); */
				tmp = *accshift + (shiftrangel << 8) - ((best + 4) << 5);
                *accshift = tmp;
				laccshift = L_deposit_h(tmp);
/**accshift = shift_r(tmp, -3);*/
				/*
				 * if (laccshift == -75497472)
				 * laccshift = -88080384;
				 */
			}
		}
	}

	for (k = 0; k < length; k++)
	{
#ifdef ASM_OPT
		bl_intrp_asm(residualm + dp + k, residual + dp + k, laccshift, BLFREQ, BLPRECISION);
#else
		bl_intrp(residualm + dp + k, residual + dp + k, laccshift, BLFREQ, BLPRECISION);
#endif
	}

	*dpm = (dp + length);

}
