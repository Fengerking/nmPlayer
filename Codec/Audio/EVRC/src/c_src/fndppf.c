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
/*  Memory Usage:                           				*/
/*      ROM:                            				*/
/*      Static/Global RAM:                      			*/
/*      Stack/Local RAM:                    				*/
/*----------------------------------------------------------------------*/
/************************************************************************
* Routine name: fndppf                                                  *
* Function: forward pitch predictor.		                        *
* Inputs:   buf    - data buffer.                                       *
*           dmin   - minimum delay value.                               *
*           dmax   - maximum delay value.                               *
*           prevdelay - previous frame delay value.                     *
*           length - size of pitch window.                              *
* Outputs:  delay  - predicted delay.                                   *
*           beta   - gain value.                                        *
*                                                                       *
************************************************************************/
#include "e_globs.h"
#include "macro.h"
#include "typedefs.h"
#include "basic_op.h"
#include "math_ext32.h"
#include "math_adv.h"
//#include "mathevrc.h"
//#include "mathdp31.h"
//#include "mathadv.h"

void fndppf(ERVC_ENC_OBJ *evrc_encobj, short *delay, short *beta, short *buf, short dmin, short dmax, short length)
{
#if (FUNC_FNDPPF_35BITS_OPT)
	static short b = -10224;	/* rom storage */

	static short a[3] =
	{-18739, 16024, -4882};		/* a[] scaled down by 4 */

	long Lcorrmax, Lcmax, Ltmp;
	long Lsum;
	long Ltempf, Ltemp2;
	register int m, i, n;
	short dnew = 0;
	short sum;
	short *DECbuf;
	short *memory;
	short lastgoodpitch;
	short lastbeta;
	
//	static short DECbuf[FrameSize / 4];
	short tap1;
	short M1, M2, dnewtmp = 0;
	short Lsum_scale;
	int shift, Lcorr_scale, Lcmax_scale;
	int n1, n2, nq, nq1;
	short tmp_len;

	/* init static variables (should be in init routine for implementation) */
	DECbuf = evrc_encobj->fnd_DECbuf;
	memory = evrc_encobj->fnd_memory;
	lastbeta = evrc_encobj->fnd_lastbeta;
	lastgoodpitch = evrc_encobj->fnd_lastgoodpitch;

	/* Shift  memory of DECbuf */
	tmp_len = (length >> 3);
	for (i = 0; i < tmp_len; i++)
	{
		DECbuf[i] = DECbuf[i + tmp_len];
	}
	/* filter signal and decimate */
	for (i = 0, n = tmp_len; i < 80; i++)
	{
		Ltempf = (long)buf[i + 80] << 11;
		Ltempf -= memory[0] * a[0];
		Ltempf -= memory[1] * a[1];
		Ltempf -= memory[2] * a[2];
		Ltempf <<= 2;

		shift = 0;
		if (((i + 1) & 3) == 0)
		{
			Lsum = Ltempf + ((long)memory[2] << 15);
			Lsum += (memory[0] + memory[1]) * b;

            DECbuf[n++] = (short)((Lsum + 0x2000) >> 14);
		}
		memory[2] = memory[1];
		memory[1] = memory[0];
		memory[0] = (short)((Ltempf + 0x4000) >> 15);
	}

	/* perform first search for best delay value in decimated domain */
	Lcorrmax = (LW_MIN);
    Lcorr_scale = 1;

	for (m = 5; m <= 30; m++)
	{
        n1 = 1;
		for (i = 0, Lsum = 0; i < (40 - m); i++)
		{
			Ltempf = DECbuf[i] * DECbuf[i + m];
			Ltempf >>= n1 - 1;
			Lsum += Ltempf;
			if ((Lsum>0?Lsum:-Lsum) >= 0x40000000)
			{
				Lsum >>= 1;
				n1++;
			}
		}
		if (((Lcorr_scale >= n1) && ((Lsum >> (Lcorr_scale - n1)) > Lcorrmax))
		 || ((Lcorr_scale < n1) && (Lsum > (Lcorrmax >> (n1 - Lcorr_scale)))))
		{
			Lcorrmax = Lsum;
			Lcorr_scale = n1;
			dnew = m;
		}

	}

	/* Compare against lastgoodpitch */
	if (lastgoodpitch != 0 && (((lastgoodpitch - (dnew << 2))>0?(lastgoodpitch - (dnew << 2)):-(lastgoodpitch - (dnew << 2))) > 2))
	{
		M1 = (lastgoodpitch >> 2) - 2;
		if (M1 < (dmin >> 2)) M1 = dmin >> 2;
		M2 = M1 + 4;
		if (M2 > (dmax >> 2)) M2 = dmax >> 2;

		Lcmax = LW_MIN;
        Lcmax_scale = 1;
		for (m = M1; m <= M2; m++)
		{
            n1 = 1;
			for (i = 0, Lsum = 0; i < (40 - m); i++)
			{
				Ltempf = DECbuf[i] * DECbuf[i + m];
				Ltempf >>= n1 - 1;
				Lsum += Ltempf;
				if ((Lsum>0?Lsum:-Lsum)>= 0x40000000)
				{
					Lsum >>= 1;
					n1++;
				}
			}

			if (((Lcmax_scale >= n1) && ((Lsum >> (Lcmax_scale - n1)) > Lcmax))
			 || ((Lcmax_scale < n1) && (Lsum > (Lcmax >> (n1 - Lcmax_scale)))))
			{					/* Gives some bias to low delays */
				Lcmax = Lsum;
				Lcmax_scale = n1;
				dnewtmp = m;
			}
		}
		Lsum = L_mpy_ls(Lcorrmax, 27361);
		if (((Lcmax_scale >= Lcorr_scale) && ((Lsum >> (Lcmax_scale - Lcorr_scale)) < Lcmax))
		 || ((Lcmax_scale < Lcorr_scale) && (Lsum < (Lcmax >> (Lcorr_scale - Lcmax_scale)))))
		{
			dnew = dnewtmp;
		}
	}

	/* perform first search for best delay value in non-decimated buffer */
	M1 = Max(((dnew << 2) - 3), dmin);
	if (M1 < dmin) M1 = dmin;
	M2 = Min(((dnew << 2) + 3), dmax);
	if (M2 > dmax) M2 = dmax;
	Lcorrmax = LW_MIN;
    Lcorr_scale = 1;
	for (m = M1; m <= M2; m++)
	{
        n1 = 1;
		for (i = 0, Lsum = 0; i < (length - m); i++)
		{
			Ltempf = buf[i] * buf[i + m] << 1;

			Ltempf >>= n1;
			Lsum += Ltempf;
			if ((Lsum>0?Lsum:-Lsum) >= 0x40000000)
			{
				Lsum >>= 1;
				n1++;
			}
		}

		if (((Lcorr_scale >= n1) && ((Lsum >> (Lcorr_scale - n1)) > Lcorrmax))
		 || ((Lcorr_scale < n1) && (Lsum > (Lcorrmax >> (n1 - Lcorr_scale)))))
		{
			Lcorrmax = Lsum;
			Lcorr_scale = n1;
			dnew = m;
		}
	}

    Lsum_scale = 1;
	Lcmax_scale = 1;
	for (i = 0, Lsum = 0, Lcmax = 0; i < (length - dnew); i++)
	{
		Ltemp2 = buf[i] * buf[i];
		Ltempf = buf[i + dnew] * buf[i + dnew];
		Ltemp2 >>= (Lcmax_scale - 1);
		Ltempf >>= (Lsum_scale - 1);
		Lsum += Ltempf;
		Lcmax += Ltemp2;
		if ((Lsum>0?Lsum:-Lsum) >= 0x40000000)
		{
			Lsum >>= 1;
			Lsum_scale++;
		}
		if((Lcmax>0?Lcmax:-Lcmax) >= 0x40000000)
		{
			Lcmax >>= 1;
			Lcmax_scale++;
		}
	}

	nq = norm_l(Lsum);
	Lsum <<= nq;
	nq1 = norm_l(Lcmax);
	Lcmax <<= nq1;
	Lsum = L_mpy_ll(Lsum, Lcmax);
	n1 = norm_l(Lsum);
	Lsum <<= n1;
	sum = sqroot(Lsum);
	n1 += nq + nq1;
	n1 -= Lcmax_scale + Lsum_scale;
	n2 = n1 >> 1;

	if (n1 & 1)
	{
		Lsum = (long)sum * 23170 << 1;
	}
	else
	{
		Lsum = (long)(sum << 16);
	}

	n2 += Lcorr_scale;
	Lcorrmax <<= n2;

	if ((Lsum == 0) || (Lcorrmax <= 0))
		*beta = 0;
	else if (Lcorrmax > Lsum)
		*beta = 0x7fff;
	else
		*beta = (short)((L_divide(Lcorrmax, Lsum) + 0x8000) >> 16);

	/* perform search for best delay value in around old pitch delay */
	if (lastgoodpitch != 0)
	{
		M1 = lastgoodpitch - 6;
		M2 = lastgoodpitch + 6;

		if (M1 < dmin)
			M1 = dmin;
		if (M2 > dmax)
			M2 = dmax;

		if (dnew > M2 || dnew < M1)
		{
			Lcmax = LW_MIN;
            Lcmax_scale = 1;
			for (m = M1; m <= M2; m++)
			{
                n1 = 1;
				for (i = 0, Lsum = 0; i < length - m; i++)
				{
					Ltempf = buf[i] * buf[i + m];
					Ltempf >>= n1 - 1;
					Lsum += Ltempf;
					if ((Lsum>0?Lsum:-Lsum) >= 0x40000000)
					{
						Lsum >>= 1;
						n1++;
					}
				}

				if (((Lcmax_scale >= n1) && ((Lsum >> (Lcmax_scale - n1)) > Lcmax))
				 || ((Lcmax_scale < n1) && (Lsum > (Lcmax >> (n1 - Lcmax_scale)))))
				{
					Lcmax = Lsum;
					dnewtmp = m;
					Lcmax_scale = n1;
				}
			}
            Lcorr_scale = 1;
			Lsum_scale  = 1;
			for (i = 0, Ltmp = 0, Lsum = 0; i < length - dnewtmp; i++)
			{
				Ltemp2 = buf[i] * buf[i];
				Ltempf = buf[i + dnewtmp] * buf[i + dnewtmp];
				Ltemp2 >>= Lsum_scale - 1;
				Ltempf >>= Lcorr_scale - 1;
                Lsum += Ltemp2;
				Ltmp += Ltempf;
				if((Lsum >0 ? Lsum:-Lsum) >= 0x40000000)
				{
					Lsum >>= 1;
					Lsum_scale++;
				}
				if ((Ltmp >0 ? Ltmp:-Ltmp) >= 0x40000000)
				{
					Ltmp >>= 1;
					Lcorr_scale++;
				}

			}
			nq = norm_l(Ltmp);
			Ltmp <<= nq;
			nq1 = norm_l(Lsum);
			Lsum <<= nq1;
			Ltmp = L_mpy_ll(Ltmp, Lsum);
			n1 = norm_l(Ltmp);
			Ltmp <<= n1;
			sum = sqroot(Ltmp);

			n1 += nq + nq1;
			n1 -= Lsum_scale + Lcorr_scale;
			n2 = n1 >> 1;

			if (n1 & 1)
            {
				Ltmp = sum * 23170 << 1;
			}
			else
            {
				Ltmp = sum << 16;
			}

			n2 += Lcmax_scale;
			Lcmax <<= n2;

			if ((Ltmp == 0) || (Lcmax <= 0))
				tap1 = 0;
			else if (Lcmax >= Ltmp)
				tap1 = 0x7fff;
			else
				tap1 = (short)((L_divide(Lcmax, Ltmp) + 0x8000) >> 16);

			/* Replace dnew with dnewtmp if tap1 is large enough */
			if ((dnew > M2 && ((tap1 >> 1) > mult_r(9830, *beta))) ||
				(dnew < M1 && ((tap1 >> 1) > mult_r(19661, *beta))))
			{
				dnew = dnewtmp;
				*beta = (tap1);
			}
		}
	}

	*delay = dnew;
	if (*beta > 13107)
	{
		evrc_encobj->fnd_lastgoodpitch = dnew;
		evrc_encobj->fnd_lastbeta = *beta;
	}
	else
	{
		evrc_encobj->fnd_lastbeta = mult_r(24576, lastbeta);
		if (evrc_encobj->fnd_lastbeta < 9830)
			evrc_encobj->fnd_lastgoodpitch = 0;
	}
	
#else

	static short b = -10224;	/* rom storage */

	static short a[3] =
	{-18739, 16024, -4882};		/* a[] scaled down by 4 */

	short dnew = 0;
	short sum;
	long Lsum;
	register short m, i, n;
	static short DECbuf[FrameSize / 4];
	long Lcorrmax, Lcmax, Ltmp;
	short tap1;
	short M1, M2, dnewtmp = 0;
	static short lastgoodpitch = 0;
	static short lastbeta = 0;
	static short memory[3];
	static int FirstTime = 1;
	short Lsum_scale;
	short shift, Lcorr_scale, Lcmax_scale;
	short n1, n2, nq, nq1;
	long Ltempf;
	/* init static variables (should be in init routine for implementation) */
	if (FirstTime)
	{
		FirstTime = 0;
		n1 = (shr(FrameSize, 2));
		for (i = 0; i < n1; i++)
			DECbuf[i] = 0;
		memory[0] = memory[1] = memory[2] = 0;
	}

	/* Shift  memory of DECbuf */
	for (i = 0; i < shr(length, 3); i++)
	{
		DECbuf[i] = DECbuf[i + shr(length, 3)];
	}
	/* filter signal and decimate */
	for (i = 0, n = shr(length, 3); i < shr(length, 1); i++)
	{
		Ltempf = L_shr(L_deposit_h(buf[i + shr(length, 1)]), 4);
		Ltempf = L_msu(Ltempf, memory[0], a[0]);
		Ltempf = L_msu(Ltempf, memory[1], a[1]);
		Ltempf = L_msu(Ltempf, memory[2], a[2]);
		Ltempf = L_shl(Ltempf, 2);

		shift = 0;
		if ((i + 1) % 4 == 0)
		{
			Lsum = L_add(Ltempf, L_deposit_h(memory[2]));
			Lsum = L_mac(Lsum, memory[0], b);
			Lsum = L_mac(Lsum, memory[1], b);

			DECbuf[n++] = round32(L_shl(Lsum, 1));
		}
		memory[2] = memory[1];
		memory[1] = memory[0];
		memory[0] = round32(Ltempf);
	}

	/* perform first search for best delay value in decimated domain */
	Lcorrmax = (LW_MIN);
        Lcorr_scale = 1;

	for (m = shr(dmin, 2); m <= shr(dmax, 2); m++)
	{
                n1 = 1;
		for (i = 0, Lsum = 0; i < sub(shr(length, 2), m); i++)
		{
			Ltempf = L_mult(DECbuf[i], DECbuf[i + m]);
			Ltempf = L_shr(Ltempf, n1);
			Lsum = L_add(Lsum, Ltempf);
			if (L_abs(Lsum) >= 0x40000000)
			{
				Lsum = L_shr(Lsum, 1);
				n1++;
			}
		}
		if (
		    ((Lcorr_scale >= n1) && (L_shr(Lsum, sub(Lcorr_scale, n1)) > Lcorrmax))
		 || ((Lcorr_scale < n1) && (Lsum > L_shr(Lcorrmax, sub(n1, Lcorr_scale))))
		 ) 
		{
			Lcorrmax = Lsum;
			Lcorr_scale = n1;
			dnew = m;
		}

	}

	/* Compare against lastgoodpitch */
	if (lastgoodpitch != 0 && (abs_s(sub(lastgoodpitch, shl(dnew, 2))) > 2))
	{
		M1 = sub(shr(lastgoodpitch, 2), 2);
		if (M1 < shr(dmin, 2))
			M1 = shr(dmin, 2);
		M2 = add(M1, 4);
		if (M2 > shr(dmax, 2))
			M2 = shr(dmax, 2);

		Lcmax = LW_MIN;
                Lcmax_scale = 1;
		for (m = M1; m <= M2; m++)
		{
                        n1 = 1;
			for (i = 0, Lsum = 0; i < sub(shr(length, 2), m); i++)
			{
				Ltempf = L_mult(DECbuf[i], DECbuf[i + m]);
				Ltempf = L_shr(Ltempf, n1);
				Lsum = L_add(Lsum, Ltempf);
				if (L_abs(Lsum) >= 0x40000000)
				{
					Lsum = L_shr(Lsum, 1);
					n1++;
				}
			}

			if (
			    ((Lcmax_scale >= n1) && (L_shr(Lsum, sub(Lcmax_scale, n1)) > Lcmax))
			 || ((Lcmax_scale < n1) && (Lsum > L_shr(Lcmax, sub(n1, Lcmax_scale))))
			   ) 
			{					/* Gives some bias to low delays */
				Lcmax = Lsum;
				Lcmax_scale = n1;
				dnewtmp = m;
			}
		}
		Lsum = L_mpy_ls(Lcorrmax, 27361);
		if (
		    ((Lcmax_scale >= Lcorr_scale) && (L_shr(Lsum, sub(Lcmax_scale, Lcorr_scale)) < Lcmax))
		 || ((Lcmax_scale < Lcorr_scale) && (Lsum < L_shr(Lcmax, sub(Lcorr_scale, Lcmax_scale))))
		   ) 
		{
			dnew = dnewtmp;
		}
	}

	/* perform first search for best delay value in non-decimated buffer */
	M1 = Max(sub(shl(dnew, 2), 3), dmin);
	if (M1 < dmin)
		M1 = dmin;
	M2 = Min(add(shl(dnew, 2), 3), dmax);
	if (M2 > dmax)
		M2 = dmax;
	Lcorrmax = LW_MIN;
        Lcorr_scale = 1;
	for (m = M1; m <= M2; m++)
	{
                n1 = 1;
		for (i = 0, Lsum = 0; i < sub(length, m); i++)
		{
			Ltempf = L_mult(buf[i], buf[i + m]);

			Ltempf = L_shr(Ltempf, n1);
			Lsum = L_add(Lsum, Ltempf);
			if (L_abs(Lsum) >= 0x40000000)
			{
				Lsum = L_shr(Lsum, 1);
				n1++;
			}
		}

		if (
		    ((Lcorr_scale >= n1) && (L_shr(Lsum, sub(Lcorr_scale, n1)) > Lcorrmax))
		 || ((Lcorr_scale < n1) && (Lsum > L_shr(Lcorrmax, sub(n1, Lcorr_scale))))
		   ) 
		{
			Lcorrmax = Lsum;
			Lcorr_scale = n1;
			dnew = m;
		}
	}
        Lsum_scale = 1;
	for (i = 0, Lsum = 0; i < sub(length, dnew); i++)
	{
		Ltempf = L_mult(buf[i + dnew], buf[i + dnew]);
		Ltempf = L_shr(Ltempf, Lsum_scale);
		Lsum = L_add(Lsum, Ltempf);
		if (L_abs(Lsum) >= 0x40000000)
		{
			Lsum = L_shr(Lsum, 1);
			Lsum_scale++;
		}
	}

        Lcmax_scale = 1;
	for (i = 0, Lcmax = 0; i < length - dnew; i++)
	{
		Ltempf = L_mult(buf[i], buf[i]);
		Ltempf = L_shr(Ltempf, Lcmax_scale);
		Lcmax = L_add(Lcmax, Ltempf);
		if (L_abs(Lcmax) >= 0x40000000)
		{
			Lcmax = L_shr(Lcmax, 1);
			Lcmax_scale++;
		}
	}
	nq = norm_l(Lsum);
	Lsum = L_shl(Lsum, nq);
	nq1 = norm_l(Lcmax);
	Lcmax = L_shl(Lcmax, nq1);
	Lsum = L_mpy_ll(Lsum, Lcmax);
	n1 = norm_l(Lsum);
	Lsum = L_shl(Lsum, n1);
	sum = sqroot(Lsum);
	n1 = add(add(n1, nq), nq1);
	n1 = sub(sub(n1, Lcmax_scale), Lsum_scale);
	n2 = shr(n1, 1);

	if (n1 & 1)
		Lsum = L_mult(sum, 23170);
	else
		Lsum = L_deposit_h(sum);

	n2 = add(n2, Lcorr_scale);
	Lcorrmax = L_shl(Lcorrmax, n2);

	if ((Lsum == 0) || (Lcorrmax <= 0))
		*beta = 0;
	else if (Lcorrmax > Lsum)
		*beta = 0x7fff;
	else
		*beta = round32(L_divide(Lcorrmax, Lsum));

	/* perform search for best delay value in around old pitch delay */
	if (lastgoodpitch != 0)
	{
		M1 = lastgoodpitch - 6;
		M2 = lastgoodpitch + 6;

		if (M1 < dmin)
			M1 = dmin;
		if (M2 > dmax)
			M2 = dmax;

		if (dnew > M2 || dnew < M1)
		{
			Lcmax = LW_MIN;
                        Lcmax_scale = 1;
			for (m = M1; m <= M2; m++)
			{
                                n1 = 1;
				for (i = 0, Lsum = 0; i < length - m; i++)
				{
					Ltempf = L_mult(buf[i], buf[i + m]);
					Ltempf = L_shr(Ltempf, n1);
					Lsum = L_add(Lsum, Ltempf);
					if (L_abs(Lsum) >= 0x40000000)
					{
						Lsum = L_shr(Lsum, 1);
						n1++;
					}
				}

				if (
				    ((Lcmax_scale >= n1) && (L_shr(Lsum, sub(Lcmax_scale, n1)) > Lcmax))
				 || ((Lcmax_scale < n1) && (Lsum > L_shr(Lcmax, sub(n1, Lcmax_scale))))
				   ) 
				{
					Lcmax = Lsum;
					dnewtmp = m;
					Lcmax_scale = n1;
				}
			}
                        Lcorr_scale = 1;
			for (i = 0, Ltmp = 0; i < length - dnewtmp; i++)
			{
				Ltempf = L_mult(buf[i + dnewtmp], buf[i + dnewtmp]);
				Ltempf = L_shr(Ltempf, Lcorr_scale);
				Ltmp = L_add(Ltmp, Ltempf);
				if (L_abs(Ltmp) >= 0x40000000)
				{
					Ltmp = L_shr(Ltmp, 1);
					Lcorr_scale++;
				}

			}
                        Lsum_scale = 1;
			for (i = 0, Lsum = 0; i < length - dnewtmp; i++)
			{

				Ltempf = L_mult(buf[i], buf[i]);
				Ltempf = L_shr(Ltempf, Lsum_scale);
				Lsum = L_add(Lsum, Ltempf);
				if (L_abs(Lsum) >= 0x40000000)
				{
					Lsum = L_shr(Lsum, 1);
					Lsum_scale++;
				}
			}

			nq = norm_l(Ltmp);
			Ltmp = L_shl(Ltmp, nq);
			nq1 = norm_l(Lsum);
			Lsum = L_shl(Lsum, nq1);
			Ltmp = L_mpy_ll(Ltmp, Lsum);
			n1 = norm_l(Ltmp);
			Ltmp = L_shl(Ltmp, n1);
			sum = sqroot(Ltmp);

			n1 = add(add(n1, nq), nq1);
			n1 = sub(sub(n1, Lsum_scale), Lcorr_scale);
			n2 = shr(n1, 1);

			if (n1 & 1)
				Ltmp = L_mult(sum, 23170);
			else
				Ltmp = L_deposit_h(sum);

			n2 = add(n2, Lcmax_scale);
			Lcmax = L_shl(Lcmax, n2);

			if ((Ltmp == 0) || (Lcmax <= 0))
				tap1 = 0;
			else if (Lcmax >= Ltmp)
				tap1 = 0x7fff;
			else
				tap1 = round32(L_divide(Lcmax, Ltmp));

			/* Replace dnew with dnewtmp if tap1 is large enough */
			if ((dnew > M2 && (shr(tap1, 1) > mult_r(9830, *beta))) ||
				(dnew < M1 && (shr(tap1, 1) > mult_r(19661, *beta))))
			{
				dnew = dnewtmp;
				*beta = (tap1);
			}
		}
	}

	*delay = dnew;
	if (*beta > 13107)
	{
		lastgoodpitch = dnew;
		lastbeta = *beta;
	}
	else
	{
		lastbeta = mult_r(24576, lastbeta);
		if (lastbeta < 9830)
			lastgoodpitch = 0;
	}
#endif
}

