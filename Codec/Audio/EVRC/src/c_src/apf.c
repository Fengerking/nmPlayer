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
/*      ROM:                            */
/*      Static/Global RAM:                      */
/*      Stack/Local RAM:                    */
/*----------------------------------------------------------------------*/

/* Adaptive post filter */

//#include "mathevrc.h"
//#include "mathdp31.h"
//#include "mathadv.h"
#include "macro.h"
#include "basic_op.h"
#include "math_adv.h"
#include "math_ext32.h"
#include "proto.h"
#include "apf.h"


void    apf(ERVC_DEC_OBJ *pdobj, short *coeff, long delayi, short alpha,
	    short beta, short u, short agc, short ltgain, short order, short length, short br)
{
#if (FUNC_APF_OPT)

	short   wcoef1[ORDER];
	short   wcoef2[ORDER];
	short   scratch[SubFrameSize];
	short   temp[SubFrameSize];
	short   mem[ORDER];
	long	sum1, sum2;
	long    gamma, APFgain;
	long	Ltemp;
	short	*in, *out;
	short	*FIRmem, *IIRmem;
	short	*Residual;
	int   i, j, n, best;
	Word32  shift1, shift2;
	short	last, Stemp;

	in	 = pdobj->DECspeech;
	out	 = pdobj->DECspeechPF;
	last = pdobj->apf_last;
	FIRmem   = pdobj->apf_FIRmem;
	IIRmem   = pdobj->apf_IIRmem;
	Residual = pdobj->apf_Residual;

	/* Compute weighted LPC coefficients */
	weight(wcoef1, coeff, alpha, order);
	weight(wcoef2, coeff, beta, order);

	/* Tilt speech  */

	/*...no tilt in non-voiced regions...*/
	sum2 = 0;
	for (i = 0; i < length - 1; i++)
	{
		long chen_temp_l = ((long)in[i] * in[i + 1]) << 1;
		if (((sum2 ^ chen_temp_l) & MIN_32) == 0)
	    {
	        sum2 += chen_temp_l;
	        if ((sum2 ^ chen_temp_l) & MIN_32)
            {
                sum2 = (chen_temp_l > 0) ? MAX_32 : MIN_32;
            }
	    }
	    else
        {
            sum2 += chen_temp_l;
        }		
	}
    
	if (sum2 < 0)
	{
		u = 0;		/*...no tilt...*/
	}

	for (i = 0; i < length; i++)
	{
		scratch[i] = ((in[i] << 15) - (u * last) + 0x4000) >> 15;
		last = in[i];
	}

	/* Compute  residual */
#ifdef  ASM_OPT
	fir_asm(scratch, scratch, wcoef1, FIRmem, order, length);
#else
	fir(scratch, scratch, wcoef1, FIRmem, order, length);
#endif

	for (i = 0; i < SubFrameSize ; i++)
	  Residual[ACBMemSize+i] = scratch[i];

	/* long term filtering */
	/* Find best integer delay around delayi */
	j = (delayi + 0x8000) >> 16;
	sum1 = 0;
    shift1 = 1;
	best = j;
	for (i = Max(DMIN, j - 3); i <= Min(DMAX, j + 3); i++)
	{
        shift2 = 1;
		for (n = ACBMemSize, sum2 = 0; n < ACBMemSize + length; n++)
		{
			Ltemp = ((long)Residual[n] * Residual[n - i]) << 1;
			Ltemp >>= shift2;
			sum2 += Ltemp;
			if (sum2 >= 0x40000000)
			{
				sum2 >>= 1;
				shift2++;
			}
		}

        if (((shift1 >= shift2) && ((sum2 >> (shift1 - shift2)) > sum1))
          || ((shift1 < shift2) && (sum2 > (sum1 >> (shift2 - shift1)))))
		{
			sum1 = sum2;
			shift1 = shift2;
			best = i;
		}
	}

	/* Get beta for delayi */
	shift1 = 1;
	for (i = ACBMemSize, sum1 = 0; i < ACBMemSize + length; i++)
	{
		Ltemp = ((long)Residual[i - best] * Residual[i - best]) << 1;
		Ltemp >>= shift1;
		sum1 += Ltemp;
		if (sum1 >= 0x40000000)
		{
			sum1 >>= 1;
			shift1++;
		}
	}
	shift2 = 1;
	for (i = ACBMemSize, sum2 = 0; i < ACBMemSize + length; i++)
	{
		Ltemp = ((long)Residual[i] * Residual[i - best]) << 1;
		Ltemp >>= shift2;
		sum2 += Ltemp;
		if (sum2 >= 0x40000000)
		{
			sum2 >>= 1;
			shift2++;
		}
	}
	if (shift1 > shift2)
	{
		shift1 -= shift2;
		sum2 >>= shift1;
	}
	else if (shift1 < shift2)
	{
		shift2 -= shift1;
		sum1 >>= shift2;
	}

	if ((sum2 == 0) || (sum1 == 0) || (br == 1))
		for (i = 0; i < length; i++)
			temp[i] = Residual[i + ACBMemSize];
	else
	{
		if (sum2 >= sum1)
			gamma = 0x7fffffff;		/* Clip gamma at 1.0 */
		else if (sum2 < 0)
			gamma = 0;
		else
		{
			shift1 = norm_l(sum1);
			sum1 <<= shift1;
			sum2 <<= shift1;
			gamma = L_divide(sum2, sum1);
		}

		if (gamma < 0x40000000)
			for (i = 0; i < length; i++)
				temp[i] = Residual[i + ACBMemSize];
		else
		{
			/* Do actual filtering */
			for (i = 0; i < length; i++)
			{
				Ltemp = L_mpy_ls(gamma, ltgain);
				Ltemp = L_mpy_ls(Ltemp, Residual[ACBMemSize + i - best]);
				temp[i] = Residual[ACBMemSize + i] + (short)((Ltemp + 0x8000) >> 16);
			}
		}

	}
	/* iir short term filter - first run */
	for (i = 0; i < length; i++)
		scratch[i] = temp[i];
	for (i = 0; i < order; i++)
		mem[i] = IIRmem[i];

	iir(scratch, scratch, wcoef2, mem, order, length);

	/* Get filter gain */
	shift1 = 1;
	for (i = 0, sum1 = 0; i < length; i++)
	{
		Ltemp = ((long)in[i] * in[i]) << 1;
		Ltemp >>= shift1;
		sum1 += Ltemp;
		if (sum1 >= 0x40000000)
		{
			sum1 >>= 1;
			shift1++;
		}
	}
	shift2 = 1;
	for (i = 0, sum2 = 0; i < length; i++)
	{
		Ltemp = ((long)scratch[i] * scratch[i]) << 1;
		Ltemp >>= shift2;
		sum2 += Ltemp;
		if (sum2 >= 0x40000000)
		{
			sum2 = L_shr(sum2, 1);
			shift2++;
		}
	}
	if (shift1 > shift2)
	{
		shift1 -= shift2;
		sum2 >>= shift1;
	}
	else if (shift1 < shift2)
	{
		shift2 -= shift1;
		sum1 >>= shift2;
	}

	if (sum2 != 0)
	{
		shift1 = norm_l(sum2);
		sum2 <<= shift1;
		shift1 -= 2;	/* For (1. < APFgain < 2.) */
		if (shift1 > 0) sum1 <<= shift1;
		if (shift1 < 0) sum1 >>= (-shift1);
		Ltemp = L_divide(sum1, sum2);
		shift1 = norm_l(Ltemp);
		Ltemp <<= shift1;
		Stemp = sqroot(Ltemp);
		if (shift1 & 1)
		{
			APFgain = ((long)0x5a82 * Stemp) << 1;
		}
		else
		{
			APFgain = (long)Stemp << 16;
		}
		shift1 >>= 1;
		APFgain >>= shift1;

		/* Re-normalize the speech signal */
		for (i = 0; i < length; i++)
		{
			Ltemp = L_mpy_ls(APFgain, temp[i]);
			Ltemp <<= 1;  /* For (1. < APFgain < 2.) */
			temp[i] = (short)((Ltemp + 0x8000) >> 16);
		}
	}
	else
	{
		APFgain = 0x40000000;
	}

	/* iir short term filter - second run */
	iir(out, temp, wcoef2, IIRmem, order, length);

	/* Update residual buffer */
	for (i = 0; i < ACBMemSize; i++)
	{
		Residual[i] = Residual[i + length];
	}

	pdobj->apf_last = last;

#else

	static int FirstTime = 1;

	static short FIRmem[ORDER];	/* FIR filter memory */
	static short IIRmem[ORDER];	/* IIR filter memory */
	static short last;
	static short Residual[ACBMemSize + SubFrameSize];	/* local residual */

	short   wcoef1[ORDER];
	short   wcoef2[ORDER];
	short   scratch[SubFrameSize];
	short   temp[SubFrameSize];
	short   mem[ORDER];
	long	sum1, sum2;
	long    gamma, APFgain;
	short   i, j, n, best;
	short	Stemp, shift1, shift2;
	long	Ltemp;


	/* initialization -- should be done in init routine for implementation */
	if (FirstTime)
	{
		FirstTime = 0;
		for (i = 0; i < ORDER; i++)
			FIRmem[i] = 0;
		for (i = 0; i < ORDER; i++)
			IIRmem[i] = 0;
		for (i = 0; i < ACBMemSize; i++)
			Residual[i] = 0;
		last = 0;
	}

	/* Compute weighted LPC coefficients */
	weight(wcoef1, coeff, alpha, order);
	weight(wcoef2, coeff, beta, order);


	/* Tilt speech  */

	/*...no tilt in non-voiced regions...*/
	for (i = 0, sum2 = 0; i < length - 1; i++)
		sum2 = L_mac(sum2, in[i], in[i + 1]);
	if (sum2 < 0)
		u = 0;		/*...no tilt...*/

	for (i = 0; i < length; i++)
	{
		scratch[i] = msu_r(L_deposit_h(in[i]), u, last);
		last = in[i];
	}

	/* Compute  residual */
	fir(scratch, scratch, wcoef1, FIRmem, order, length);

	for (i = 0; i < SubFrameSize ; i++)
	  Residual[ACBMemSize+i] = scratch[i];

	/* long term filtering */
	/* Find best integer delay around delayi */
	j = extract_h(L_add(delayi, 32768));
	sum1 = 0;
        shift1 = 1;
	best = j;
	for (i = Max(DMIN, j - 3); i <= Min(DMAX, j + 3); i++)
	{
                shift2 = 1;
		for (n = ACBMemSize, sum2 = 0; n < ACBMemSize + length; n++)
		{
			Ltemp = L_mult(Residual[n], Residual[n - i]);
			Ltemp = L_shr(Ltemp, shift2);
			sum2 = L_add(sum2, Ltemp);
			if (sum2 >= 0x40000000)
			{
				sum2 = L_shr(sum2, 1);
				shift2++;
			}
		}

                if( ((shift1 >= shift2) && (L_shr(sum2,sub(shift1,shift2)) > sum1))
                   || ((shift1 < shift2) && (sum2 > L_shr(sum1,sub(shift2,shift1)))))
		{
			sum1 = sum2;
			shift1 = shift2;
			best = i;
		}
	}

	/* Get beta for delayi */
	shift1 = 1;
	for (i = ACBMemSize, sum1 = 0; i < ACBMemSize + length; i++)
	{
		Ltemp = L_mult(Residual[i - best], Residual[i - best]);
		Ltemp = L_shr(Ltemp, shift1);
		sum1 = L_add(sum1, Ltemp);
		if (sum1 >= 0x40000000)
		{
			sum1 = L_shr(sum1, 1);
			shift1++;
		}
	}
	shift2 = 1;
	for (i = ACBMemSize, sum2 = 0; i < ACBMemSize + length; i++)
	{
		Ltemp = L_mult(Residual[i], Residual[i - best]);
		Ltemp = L_shr(Ltemp, shift2);
		sum2 = L_add(sum2, Ltemp);
		if (sum2 >= 0x40000000)
		{
			sum2 = L_shr(sum2, 1);
			shift2++;
		}
	}
	if (shift1 > shift2)
	{
		shift1 = sub(shift1, shift2);
		sum2 = L_shr(sum2, shift1);
	}
	else if (shift1 < shift2)
	{
		shift2 = sub(shift2, shift1);
		sum1 = L_shr(sum1, shift2);
	}

	if ((sum2 == 0) || (sum1 == 0) || (br == 1))
		for (i = 0; i < length; i++)
			temp[i] = Residual[i + ACBMemSize];
	else
	{
		if (sum2 >= sum1)
			gamma = 0x7fffffff;		/* Clip gamma at 1.0 */
		else if (sum2 < 0)
			gamma = 0;
		else
		{
			shift1 = norm_l(sum1);
			sum1 = L_shl(sum1, shift1);
			sum2 = L_shl(sum2, shift1);
			gamma = L_divide(sum2, sum1);
		}

		if (gamma < 0x40000000)
			for (i = 0; i < length; i++)
				temp[i] = Residual[i + ACBMemSize];
		else
		{
			/* Do actual filtering */
			for (i = 0; i < length; i++)
			{
				Ltemp = L_mpy_ls(gamma, ltgain);
				Ltemp = L_mpy_ls(Ltemp, Residual[ACBMemSize + i - best]);
				temp[i] = add(Residual[ACBMemSize + i], round32(Ltemp));
			}
		}

	}


	/* iir short term filter - first run */
	for (i = 0; i < length; i++)
		scratch[i] = temp[i];
	for (i = 0; i < order; i++)
		mem[i] = IIRmem[i];
	iir(scratch, scratch, wcoef2, mem, order, length);


	/* Get filter gain */
	shift1 = 1;
	for (i = 0, sum1 = 0; i < length; i++)
	{
		Ltemp = L_mult(in[i], in[i]);
		Ltemp = L_shr(Ltemp, shift1);
		sum1 = L_add(sum1, Ltemp);
		if (sum1 >= 0x40000000)
		{
			sum1 = L_shr(sum1, 1);
			shift1++;
		}
	}
	shift2 = 1;
	for (i = 0, sum2 = 0; i < length; i++)
	{
		Ltemp = L_mult(scratch[i], scratch[i]);
		Ltemp = L_shr(Ltemp, shift2);
		sum2 = L_add(sum2, Ltemp);
		if (sum2 >= 0x40000000)
		{
			sum2 = L_shr(sum2, 1);
			shift2++;
		}
	}
	if (shift1 > shift2)
	{
		shift1 = sub(shift1, shift2);
		sum2 = L_shr(sum2, shift1);
	}
	else if (shift1 < shift2)
	{
		shift2 = sub(shift2, shift1);
		sum1 = L_shr(sum1, shift2);
	}

	if (sum2 != 0)
	{
		shift1 = norm_l(sum2);
		sum2 = L_shl(sum2, shift1);
		shift1 = sub(shift1, 2);	/* For (1. < APFgain < 2.) */
		sum1 = L_shl(sum1, shift1);
		Ltemp = L_divide(sum1, sum2);
		shift1 = norm_l(Ltemp);
		Ltemp = L_shl(Ltemp, shift1);
		Stemp = sqroot(Ltemp);
		if (shift1 & 1)
			APFgain = L_mult(0x5a82, Stemp);
		else
			APFgain = L_deposit_h(Stemp);
		shift1 = shr(shift1, 1);
		APFgain = L_shr(APFgain, shift1);

		/* Re-normalize the speech signal */
		for (i = 0; i < length; i++)
		{
			Ltemp = L_mpy_ls(APFgain, temp[i]);
			Ltemp = L_shl(Ltemp, 1);  /* For (1. < APFgain < 2.) */
			temp[i] = round32(Ltemp);
		}
	}
	else
		APFgain = 0x40000000;


	/* iir short term filter - second run */
	iir(out, temp, wcoef2, IIRmem, order, length);

	/* Update residual buffer */
	for (i = 0; i < ACBMemSize; i++)
		Residual[i] = Residual[i + length];
#endif
}
