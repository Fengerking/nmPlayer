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
/*  Memory Usage:                           */
/*      ROM:                1           */
/*      Static/Global RAM:  0               */
/*      Stack/Local RAM:    527             */
/*----------------------------------------------------------------------*/
/*********************************************************************************
*            *****   LSP matrix quantization  ************                       *
*                                                                                *
* Input Arguments                                                                *
* ***************                                                                *
* x		Input LSP matrix  x(ndimx,kdim), i.e., kdim rows of              *
*		ndimx elements.                                                  *
*		LSP vectors row by row.                                          *
* ndim		Actual vector dimension (LSP order).  ndim <= ndimx              *
* kdim		Number of LSP vectors, i.e., number of rows in the main matrix.  *
* many		Number of sub-matrices.                                          *
* nsub		Array containing "many" sub-matrices sizes. nsub(i) is the size  *
*		of the rows in the ith sub-matrix. Sum(i)[ nsub(i) ] = ndim      *
* book		(1D). Contains "many" codebooks in series. The ith codebook      *
*		contains matices of size kdim rows by nsub(i) columns.           *
*		i.e., kdim vectors of size nsub(i) in series.                    *
* alp		Weight adjustment factor.  alp=0  no weight                      *
*                                          alp >0 input is weighted              *
* nsiz		Contain "many" size values. nsiz(i) is the number of matrices    *
*		in codebook i.                                                   *
*                                                                                *
* Output arguments                                                               *
* ****************                                                               *
* y		Output matrix. The quantized version of x (same size).           *
* index		Contains "many" indices of the best matrix from each codebook    *
* d		Aux. array, at least the size of max( nsiz(i) )                  *
*                                                                                *
*********************************************************************************/
/*===========================================================================
                        INCLUDES
===========================================================================*/
#include <stdio.h>
#include "basic_op.h"
#include "macro.h"
#include "rom.h"

/*===========================================================================
                        GLOBAL VARIABLES
===========================================================================*/

/*===========================================================================
                        FUNCTIONS
===========================================================================*/
void lspmaq1(short *x, short many, short *nsub, short *nsiz, short *y, short *index, short *lsptab)
{
	register int i, j, m, im, jp, jm, jpm, jj;
	short dm;
	short d[512];
	short shft_fctr[ORDER];
	short temp, temp1;
	int ltemp1;
	int ltemp2;
	short stemp1;
	jj = 0;
	/* Find weights */
	y[0] = x[1] - x[0];
    shft_fctr[0] = norm_s(y[0]);
	y[0] <<= shft_fctr[0];
	y[0] = (divide_s(16384, y[0]) * 5215 >> 15) + (0x4000 >> (shft_fctr[0] - 1));

	y[9] = x[9] - x[8];
    shft_fctr[9] = norm_s(y[9]);
	y[9] <<= shft_fctr[9];
	y[9] = (divide_s(16384, y[9]) * 5215 >> 15) + (0x4000 >> (shft_fctr[9] - 1));

	for (i = 1; i < 9; i++)
	{
		y[i] = Min((x[i] - x[i - 1]), (x[i + 1] - x[i]));
		shft_fctr[i] = norm_s(y[i]);
		y[i] <<= shft_fctr[i];
		y[i] = (divide_s(16384, y[i]) * 5215 >> 15) + (0x4000 >> (shft_fctr[i] - 1));
	}

	/* Code all sub-matrices */
	im = 0;
	jp = 0;
	for (m = 0; m < many; m++)
	{
		temp = nsub[m];
		temp1 = nsiz[m];
		for (j = 0; j < temp1; j++)
		{
			/* jm = km * j; */
            jm = temp * j;         
			ltemp2 = 0;
			jpm = jp + jm;
			for (i = 0; i < temp; i++)
			{
				/* Rounding instead of extracting the high word
				 * in the following statements causes an error on 
				 * input frame 141 of enc_lspmaq.in (female).*/
				stemp1 = x[i + im] - lsptab[jpm + i];
				ltemp1 = y[i + im] * stemp1;
				ltemp1 = ltemp1 << (shft_fctr[i + im] - 2);
                if (ltemp1 > (Word32)0x0fffffffL)
                {
                    ltemp1 = MAX_16;
                }
                else if (ltemp1 < (Word32)0xf0000000L)
                {
                    ltemp1 = MIN_16;
                }
                else
                {
                    ltemp1 >>= 13;
                }
				ltemp2 += (ltemp1 * stemp1 << 1);
			}

			d[j] = (ltemp2 + 0x8000) >> 16;
		}
		if (im > 0)
		{
			for (j = 0; j < temp1; j++)
			{
				jm = temp* j;
				jpm = jp + jm;

				if (lsptab[jpm] <= (y[im - 1] + 261))
				{
					d[j] = 0x7FFF;
				}
			}
		}
		dm = 0x7FFF;
		for (j = 0; j < temp1; j++)
		{
			if (d[j] <= dm)
			{
				dm = d[j];
				jj = j;
			}
		}
		index[m] = jj;
		jpm = jp + temp * index[m];
		for (i = 0; i < temp; i++)
		{
			y[im + i] = lsptab[jpm + i];
		}
		im += temp;
		jp += temp * temp1;
	}
}

void lspmaq(short *x, short ndim, short kdim, short many, short *nsub,
		   short *nsiz, short alp, short *y, short *index, short br, short *lsptab)
{
#if (FUNC_LSPMAQ_OPTIMIZE)
#define SCALED_OneDivPI2 5215
    static short df = MIN_LSP_SEP;
	register int i, j, m, im, jp, jm, jpm, jj;
	short dm;
	short d[512];
	short shft_fctr[ORDER];
	int ltemp1;
	int ltemp2;
	short stemp1;
	jj = 0;
	/* Find weights */
	y[0] = x[1] - x[0];
	y[ndim - 1] = x[ndim - 1] - x[ndim - 2];

	for (i = 1; i < ndim - 1; i++)
	{
		y[i] = Min((x[i] - x[i - 1]), (x[i + 1] - x[i]));
	}

	for (i = 0; i < ndim; i++)
	{
		if (y[i] != 0)
		{
			shft_fctr[i] = norm_s(y[i]);
			y[i] <<= shft_fctr[i];
			y[i] = (divide_s(alp, y[i]) * SCALED_OneDivPI2 >> 15) + (0x4000 >> (shft_fctr[i] - 1));
		}
		else
		{
			y[i] = 18346;
			shft_fctr[i] = 4;
		}
	}
	/* Code all sub-matrices */
	im = 0;
	jp = 0;
	for (m = 0; m < many; m++)
	{
		for (j = 0; j < nsiz[m]; j++)
		{
			/* jm = km * j; */
            jm = nsub[m] * j;         
			ltemp2 = 0;
			jpm = jp + jm;
			for (i = 0; i < nsub[m]; i++)
			{
				/* Rounding instead of extracting the high word
				 * in the following statements causes an error on 
				 * input frame 141 of enc_lspmaq.in (female).*/
				stemp1 = x[i + im] - lsptab[jpm + i];
				ltemp1 = y[i + im] * stemp1;
				ltemp1 = ltemp1 << (shft_fctr[i + im] - 2);
                if (ltemp1 > (Word32)0x0fffffffL)
                {
                    ltemp1 = MAX_16;
                }
                else if (ltemp1 < (Word32)0xf0000000L)
                {
                    ltemp1 = MIN_16;
                }
                else
                {
                    ltemp1 >>= 13;
                }
				ltemp2 += (ltemp1 * stemp1 << 1);
			}

			d[j] = (ltemp2 + 0x8000) >> 16;
		}
		if (im > 0)
		{
			for (j = 0; j < nsiz[m]; j++)
			{
				jm = nsub[m] * j;
				jpm = jp + jm;

				if (lsptab[jpm] <= (y[im - 1] + df))
				{
					d[j] = 0x7FFF;
				}
			}
		}
		dm = 0x7FFF;
		for (j = 0; j < nsiz[m]; j++)
		{
			if (d[j] <= dm)
			{
				dm = d[j];
				jj = j;
			}
		}
		index[m] = jj;
		jpm = jp + nsub[m] * index[m];
		for (i = 0; i < nsub[m]; i++)
		{
			y[im + i] = lsptab[jpm + i];
		}
		im += nsub[m];
		jp += nsub[m] * nsiz[m];
	}
#else

/*===========================================================================
                         LOCAL VARIABLE DECLARATIONS
===========================================================================*/
#define SCALED_OneDivPI2 5215
        static short df = MIN_LSP_SEP;
	register short i, j, m, im, jp, jm, jpm, jj;
	short dm;
	short d[2048];
	short shft_fctr[ORDER];
	long ltemp1;
	long ltemp2;
	short stemp1;
/*===========================================================================
                         START C CODE
===========================================================================*/
	jj = 0;

	/* Find weights */
	y[0] = sub(x[1], x[0]);

	y[ndim - 1] = sub(x[ndim - 1], x[ndim - 2]);

	for (i = 1; i < ndim - 1; i++)
		y[i] = Min(sub(x[i], x[i - 1]), sub(x[i + 1], x[i]));

	for (i = 0; i < ndim; i = add(i, 1))
	{
		if (y[i] != 0)
		{
			shft_fctr[i] = norm_s(y[i]);
			y[i] = shl(y[i], shft_fctr[i]);
			y[i] = add(mult(divide_s(alp, y[i]), SCALED_OneDivPI2),
					   shr(0x4000, shft_fctr[i] - 1));
		}
		else
		{
			y[i] = 18346;
			shft_fctr[i] = 4;
		}
	}
	/* Code all sub-matrices */
	im = 0;
	jp = 0;
	for (m = 0; m < many; m = add(m, 1))
	{
		for (j = 0; j < nsiz[m]; j = add(j, 1))
			d[j] = 0;

		for (j = 0; j < nsiz[m]; j = add(j, 1))
		{
			/* jm = km * j; */
			jm = shr(extract_l(L_mult(nsub[m], j)), 1);

			ltemp2 = 0;
			jpm = add(jp, jm);

			for (i = 0; i < nsub[m]; i++)
			{
				/* Rounding instead of extracting the high word
				 * in the following statements causes an error on 
				 * input frame 141 of enc_lspmaq.in (female).
				 */
				stemp1 = sub(x[i + im], lsptab[jpm + i]);
				ltemp1 = L_shl(L_mult(y[i + im], stemp1),
							   shft_fctr[i + im]);
				ltemp2 = L_add(ltemp2,
							   L_mult(extract_h(ltemp1),
									  stemp1));
			}

			d[j] = round32(ltemp2);
		}

		if (im > 0)
		{
			for (j = 0; j < nsiz[m]; j = add(j, 1))
			{
				jm = shr(extract_l(L_mult(nsub[m], j)), 1);
				jpm = add(jp, jm);

				if (lsptab[jpm] <= add(y[im - 1], df))
				{
					d[j] = 0x7FFF;
				}
			}
		}

		dm = 0x7FFF;

		for (j = 0; j < nsiz[m]; j = add(j, 1))
		{
			if (d[j] <= dm)
			{
				dm = d[j];
				jj = j;
			}
		}

		index[m] = jj;

		jpm = add(jp, shr(extract_l(L_mult(nsub[m], index[m])), 1));

		for (i = 0; i < nsub[m]; i = add(i, 1))
		{
			y[im + i] = lsptab[jpm + i];
		}

		im = add(im, nsub[m]);

		jp = add(jp, shr(extract_l(L_mult(nsub[m], nsiz[m])), 1));
	}
#endif
}								/* END MODULE lspmaq */

/***************************************************************************
* Routine name: lspmaq_dec                                                 *
* Function: VQ of line spectral frequencies - Decoder part.                *
* Inputs:                                                                  *
*         ndim	- Actual vector dimension (LSP order).                     *
*         kdim	- Number of LSP vectors, i.e., number of rows in the main  *
*                matrix.                                                   *
*         many	- Number of sub-matrices.                                  *
*         nsub	- Array containing "many" sub-matrices sizes. nsub(i) is   *
*                the size of the rows in the ith sub-matrix.               *
*                Sum(i)[ nsub(i) ] = ndim.                                 *
*         book	- (1D). Contains "many" codebooks in series. The ith       *
*                codebook contains matices of size kdim rows by nsub(i)    *
*                columns. i.e., kdim vectors of size nsub(i) in series.    *
*         alp	- Weight adjustment factor. alp=0 no weight. alp >0 input  *
*                is weighted.                                              *
*         nsiz	- Contain "many" size values. nsiz(i) is the number of     *
*                matrices in codebook i.                                   *
*                                                                          *
* Outputs: y	- Output matrix. The quantized version of x (same size).   *
*          index - Contains "many" indices of the best matrix from each    *
*                  codebook.                                               *
*                                                                          *
*                                                                          *
*  Program written by Dror Nahumi, Dep. 45370H                             *
***************************************************************************/
void lspmaq_dec(short ndim, short kdim, short many, short *nsub,
				short *nsiz, short *y, short *index, short br, short *lsptab)
{
//	register int i, m, im, jp, jpm;
	int i;
	int m;
	short im;
	short jp;
	short jpm;

	/* Code all sub-matrices */
	im = 0;
	jp = 0;
	for (m = 0; m < many; m++)
	{
		jpm = add(jp, extract_l(L_shr(L_mult(nsub[m], index[m]), 1)));
		for (i = 0; i < nsub[m]; i++)
			y[im + i] = lsptab[jpm + i];
		im = add(im, nsub[m]);
		jp = add(jp, extract_l(L_shr(L_mult(nsub[m], nsiz[m]), 1)));
	}
}

