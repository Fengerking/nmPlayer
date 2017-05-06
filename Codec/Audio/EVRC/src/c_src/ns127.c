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
/* ns127.c */
/*****************************************************************
*
* EVRC Noise Suppression
*
* Input:  The input to the function is a Shortword pointer to the
*         array of data to be noise suppressed.
*
* Output: There is no return value.  The input array is replaced
*         with the noise suppressed values.
*
*
* Written by:                   Tenkasi V. Ramabadran
* Date:                         December 28, 1994
*
* Last Modified:		James Ashley
* Date:				November 7, 1996
*
* Version    Date      Description
*
*   1.0    12/01/95    Released to TIA TR45.5.1.1
*   1.1    02/14/96    Init Noise to first 4 frames      
*   1.2    02/19/96    Bug fix in frame_cnt declaration  
*          03/27/96    Revised for Fixed Point Calculations
*          05/28/96    Adjust thresholds for -6 dB input level,
*                      add block_norm/denorm around FFT/IFFT,
*                      general scaling cleanup.
*   1.3    08/09/96    Adjusted thresholds to 0dB input level, 
*                      Modified block_denormalization to provide 6 db adjustment.
*   1.4    10/15/96    Apply block_norm call to farray_ptr instead of data_buffer
*                      (fixes high level input problems). (JPA)
*   1.5    11/07/96    Fix bug in block_norm(). (JPA)
*****************************************************************/
#include <stdio.h>
#include <math.h>

/* Includes */
#include "typedefs.h"
#include "basic_op.h"
#include "math_ext32.h"
#include "math_adv.h"
#include "e_globs.h"
#include "ns127.h"
//#include "mathevrc.h"
//#include "mathdp31.h"
//#include "mathadv.h"


/* Local functions */

Shortword block_norm(Shortword * data, Shortword size, Shortword headroom)
{

	Shortword i, max, scnt, adata;

	max = (data[0] >=0)?data[0]:(-data[0]);
	for (i = 1; i < size; i++)
	{
		adata = (data[i] >= 0)?data[i]:(-data[i]);
		if (adata > max)
			max = adata;
	}
	scnt = norm_s(max) - headroom;
	for (i = 0; i < size; i++)
	{
		data[i] = shift_r(data[i], scnt);
	}
	return (scnt);

}

void block_denorm(Shortword * data, Shortword size, Shortword scnt)
{

	Shortword i;

	for (i = 0; i < size; i++)
	{
		data[i] = shift_r(data[i], (-scnt));
	}
	return;

}

/* The noise supression function */
/* Functions */
void r_fft(short *, short);

void noise_suprs(ERVC_ENC_OBJ* evrc_encobj, Shortword * farray_ptr)
{
/*
 * The channel table is defined below.  In this table, the
 * lower and higher frequency coefficients for each of the 16
 * channels are specified.  The table excludes the coefficients
 * with numbers 0 (DC), 1, and 64 (Foldover frequency).  For
 * these coefficients, the gain is always set at 1.0 (0 dB).
 */

	static Shortword ch_tbl[NUM_CHAN][2] =
	{

		{2, 3},
		{4, 5},
		{6, 7},
		{8, 9},
		{10, 11},
		{12, 13},
		{14, 16},
		{17, 19},
		{20, 22},
		{23, 26},
		{27, 30},
		{31, 35},
		{36, 41},
		{42, 48},
		{49, 55},
		{56, 63}

	};

	static Shortword ch_tbl_sh[NUM_CHAN][2] =
	{

		{TRUE, 1},
		{TRUE, 1},
		{TRUE, 1},
		{TRUE, 1},
		{TRUE, 1},
		{TRUE, 1},
		{FALSE, 10922},
		{FALSE, 10922},
		{FALSE, 10922},
		{TRUE, 2},
		{TRUE, 2},
		{FALSE, 6553},
		{FALSE, 5462},
		{FALSE, 4681},
		{FALSE, 4681},
		{TRUE, 3}

	};

static Shortword nTable_VMetric[90] = { 
		0x2,  
		0x2,  0x2,  0x2,  0x2,  0x2,  0x2,  0x2,  0x2,  
		0x2,  0x2,  0x3,  0x3,  0x3,  0x3,  0x3,  0x4,  
		0x4,  0x4,  0x5,  0x5,  0x5,  0x6,  0x6,  0x7,  
		0x7,  0x7,  0x8,  0x8,  0x9,  0x9,  0xa,  0xa,  
		0xb,  0xc,  0xc,  0xd,  0xd,  0xe,  0xf,  0xf,  
		0x10,  0x11,  0x11,  0x12,  0x13,  0x14,  0x14,  0x15,  
		0x16,  0x17,  0x18,  0x18,  0x19,  0x1a,  0x1b,  0x1c,  
		0x1c,  0x1d,  0x1e,  0x1f,  0x20,  0x21,  0x22,  0x23,  
		0x24,  0x25,  0x25,  0x26,  0x27,  0x28,  0x29,  0x2a,  
		0x2b,  0x2c,  0x2d,  0x2e,  0x2f,  0x30,  0x31,  0x32,  
		0x32,  0x32,  0x32,  0x32,  0x32,  0x32,  0x32,  0x32,  
		0x32
}; 

	static Shortword window[DELAY + FRM_LEN] =
	{

		35, 315, 869, 1690, 2761, 4066, 5581, 7281, 9137,
		11117, 13187, 15312, 17455, 19580, 21650, 23630,
		25486, 27186, 28701, 30006, 31077, 31898, 32452,
		32732, 32767, 32767, 32767, 32767, 32767, 32767,
		32767, 32767, 32767, 32767, 32767, 32767, 32767,
		32767, 32767, 32767, 32767, 32767, 32767, 32767,
		32767, 32767, 32767, 32767, 32767, 32767, 32767,
		32767, 32767, 32767, 32767, 32767, 32767, 32767,
		32767, 32767, 32767, 32767, 32767, 32767, 32767,
		32767, 32767, 32767, 32767, 32767, 32767, 32767,
		32767, 32767, 32767, 32767, 32767, 32767, 32767,
		32767, 32732, 32452, 31898, 31077, 30006, 28701,
		27186, 25486, 23630, 21650, 19580, 17455, 15312,
		13187, 11117, 9137, 7281, 5581, 4066, 2761, 1690,
		869, 315, 35

	};
	
	Shortword *overlap;
	Shortword *ch_gain;	/* scaled as 0,15 */
	Shortword *window_overlap;
	Shortword *ch_enrg_long_db;		/* scaled as 10,5  */

	Longword *ch_enrg;	/* scaled as 23,8 */
	Longword *ch_noise;		/* scaled as 15,16 (change to 23,8) */

	Longword enrg;				/* scaled as 30,1 */
	Longword tne;				/* scaled as 15,16 (change to 23,8) */
	Longword tce;				/* scaled as 23,8 */
	Longword gain;				/* scaled as 11,20 */

	Shortword data_buffer[FFT_LEN];
	Shortword ch_snr[NUM_CHAN];	/* scaled as 15,0 */
	Shortword ftmp2;			/* scaled as 0,15 */
	Shortword vm_sum;			/* scaled as 15,0 */
	Shortword ch_enrg_dev;		/* scaled as 10,5 */
	Shortword ch_enrg_db[NUM_CHAN];		/* scaled as 10,5 */
	Shortword alpha;			/* scaled as 0,15 */

	int i, j, j1, j2;
	int update_flag, modify_flag, index_cnt;

	Longword Ltmp, Ltmp1, Ltmp2, Ltmp3;
	int tmp, tmp1, tmp2, norm_shift, norm_shift1;

	int normb_shift;		/* block norm shift count */

///* Functions */
//	void r_fft(short *, short);

	overlap = evrc_encobj->ns_overlap;
	ch_gain = evrc_encobj->ns_ch_gain;
	window_overlap  = evrc_encobj->ns_window_overlap;
	ch_enrg_long_db = evrc_encobj->ns_ch_enrg_long_db;

	ch_enrg  = evrc_encobj->ns_ch_enrg;
	ch_noise = evrc_encobj->ns_ch_noise;

/* Increment frame counter */
	evrc_encobj->ns_frame_cnt++;

/* Block normalize the input */
	normb_shift = block_norm(farray_ptr, FRM_LEN, FFT_HEADROOM);

/*
 * Preemphasize the input data and store in the data buffer with
 * appropriate delay 
 */
	for (i = 0; i < DELAY; i++)
		data_buffer[i] = shift_r(window_overlap[i], (normb_shift - evrc_encobj->ns_last_normb_shift));

	evrc_encobj->ns_pre_emp_mem = shift_r(evrc_encobj->ns_pre_emp_mem,(normb_shift-evrc_encobj->ns_last_normb_shift));
	evrc_encobj->ns_last_normb_shift = normb_shift;

	data_buffer[DELAY] = (*farray_ptr) + (Shortword)((Longword)PRE_EMP_FAC * evrc_encobj->ns_pre_emp_mem >> 15);

	for (i = DELAY + 1, j = 1; i < DELAY + FRM_LEN; i++, j++)
	{
		data_buffer[i] = (*(farray_ptr + j)) + (Shortword)((Longword)PRE_EMP_FAC * (*(farray_ptr + j - 1)) >> 15);
	}

	evrc_encobj->ns_pre_emp_mem = *(farray_ptr + FRM_LEN - 1);

	for (i = DELAY + FRM_LEN; i < FFT_LEN; i++)
		data_buffer[i] = 0;

/* update window_overlap buffer */
	for (i = 0, j = FRM_LEN; i < DELAY; i++, j++)
		window_overlap[i] = data_buffer[j];

/* Apply window to frame prior to FFT */
	for (i = 0; i < FRM_LEN + DELAY; i++)
	{
		data_buffer[i] = (data_buffer[i] * window[i] + 0x4000) >> 15;
	}

/* Perform FFT on the data buffer */
	r_fft(data_buffer, +1);

/* Estimate the energy in each channel */
	for (i = LO_CHAN; i <= HI_CHAN; i++)
	{
		enrg = 0;
		j1 = ch_tbl[i][0];
		j2 = ch_tbl[i][1];

		for (j = j1; j <= j2; j++)
		{
			enrg += (Longword)data_buffer[2 * j]     * data_buffer[2 * j];
			enrg += (Longword)data_buffer[2 * j + 1] * data_buffer[2 * j + 1];
		}

		if (ch_tbl_sh[i][0] == TRUE)
		{
			enrg >>= ch_tbl_sh[i][1] - 1;
		}
		else
		{
			norm_shift = norm_l(enrg);
			tmp = (Shortword)((enrg << norm_shift) >> 16);
			enrg = tmp * ch_tbl_sh[i][1];
			enrg >>= norm_shift - 2;
		}

		if (evrc_encobj->ns_first == TRUE)
		{
		    //can't modified further, or bit-exact tests fail
			ch_enrg[i] = L_shl(enrg, (Shortword)(7 - (normb_shift << 1)));	/* rescaled from 30,1 to 23,8 (w/block denorm) */
		}
		else
		{
			norm_shift = norm_l(enrg);
			Ltmp1 = enrg << norm_shift;
			Ltmp1 = L_mpy_ls(CEE_SM_FAC, extract_h(Ltmp1));
			Ltmp1 >>= norm_shift;
			Ltmp2 = L_shl(Ltmp1, (Shortword)(7 - (normb_shift << 1)));	/* rescaled from 30,1 to 23,8 (w/block denorm) */

			norm_shift = norm_l(ch_enrg[i]);
			Ltmp1 = ch_enrg[i] << norm_shift;
			Ltmp3 = L_mpy_ls(ONE_MINUS_CEE_SM_FAC, extract_h(Ltmp1));
			Ltmp3 >>= norm_shift;

			//following L_add can't be modified, or bit-exact tests will fail
			if (((Ltmp3 ^ Ltmp2) & MIN_32) == 0)
		    {
		        Ltmp2 += Ltmp3;
		        if ((Ltmp3 ^ Ltmp2) & MIN_32)
	            {
	                ch_enrg[i] = (Ltmp3 > 0) ? MAX_32 : MIN_32;
	            }
	            else
                {
                    ch_enrg[i] = Ltmp2;
                }
		    }
		    else
	        {
                ch_enrg[i] = Ltmp2 + Ltmp3;
	        }
		}

		if (ch_enrg[i] < MIN_CHAN_ENRG)
			ch_enrg[i] = MIN_CHAN_ENRG;
	}

/* Initialize channel noise estimate to channel energy of first four frames */
	if (evrc_encobj->ns_frame_cnt <= 4)
	{
		for (i = LO_CHAN; i <= HI_CHAN; i++)
		{
			if (ch_enrg[i] < INE_CHAN)
				ch_noise[i] = INE_NOISE;
			else
				ch_noise[i] = ch_enrg[i];
		}
	}

/* Compute the channel SNR indices */
	for (i = LO_CHAN; i <= HI_CHAN; i++)
	{

		norm_shift = norm_l(ch_noise[i]);
		Ltmp = ch_noise[i] << norm_shift;

		norm_shift1 = norm_l(ch_enrg[i]);
		Ltmp3 = (ch_enrg[i] << norm_shift1) >> 1;

		Ltmp2 = L_divide(Ltmp3, Ltmp);
		//following L_shr can't be modified, or bit-exact tests will fail
		Ltmp2 = L_shr(Ltmp2, (Shortword)(27 - 1 + norm_shift1 - norm_shift));	/* scaled as 27,4 */

		if (Ltmp2 == 0)
			Ltmp2 = 1;

		Ltmp1 = fnLog10(Ltmp2);
		Ltmp3 = Ltmp1 + LOG_OFFSET - 80807124;	/* -round32(log10(2^4)*2^26 */
		Ltmp2 = (Longword)TEN_S5_10 * (Ltmp3 >> 16) << 1;
		if (Ltmp2 < 0)
			Ltmp2 = 0;
		/* 0.1875 scaled as 10,21 */
		Ltmp1 = Ltmp2 + CONST_0_1875_S10_21;
		/* tmp / 0.375  2.667 scaled as 5,10, Ltmp is scaled 15,16 */
		Ltmp = (Ltmp1 >> 16) * CONST_2_667_S5_10;
		ch_snr[i] = (Shortword)(Ltmp >> 15);
	}

/* Compute the sum of voice metrics */
	vm_sum = 0;
	for (i = LO_CHAN; i <= HI_CHAN; i++)
	{
		if (ch_snr[i] < 89)
			j = ch_snr[i];
		else
			j = 89;

		vm_sum += nTable_VMetric[j];
	}
/* Compute the total noise estimate (tne) and total channel energy estimate (tce) */
	tne = tce = 0;
	for (i = LO_CHAN; i <= HI_CHAN; i++)
	{
		tne += ch_noise[i];
		tce += ch_enrg[i];
	}

/* Calculate log spectral deviation */
	for (i = LO_CHAN; i <= HI_CHAN; i++)
	{
		Ltmp = ch_enrg[i];
		if (Ltmp == 0)
			Ltmp = 1;
		Ltmp1 = fnLog10(Ltmp);
		Ltmp2 = Ltmp1 + LOG_OFFSET - 161614248;	/* -round32(log10(2^8)*2^26) */
		ch_enrg_db[i] = (Shortword)((Longword)TEN_S5_10 * (Ltmp2 >> 16) >> 15);
	}

	if (evrc_encobj->ns_first == TRUE)
		for (i = LO_CHAN; i <= HI_CHAN; i++)
			ch_enrg_long_db[i] = ch_enrg_db[i];

	ch_enrg_dev = 0;
	for (i = LO_CHAN; i <= HI_CHAN; i++)
	{
		tmp = (ch_enrg_long_db[i] >= ch_enrg_db[i]) ? (ch_enrg_long_db[i] - ch_enrg_db[i]) : (ch_enrg_db[i] - ch_enrg_long_db[i]);
		ch_enrg_dev += tmp;
	}

/*
 * Calculate long term integration constant as a function of total channel energy (tce) 
 * (i.e., high tce (-40 dB) -> slow integration (alpha = 0.99),
 *         low tce (-60 dB) -> fast integration (alpha = 0.50)
 */

	Ltmp1 = fnLog10(tce);
	Ltmp2 = Ltmp1 + LOG_OFFSET - 161614248;	/* -round32(log10(2^8)*2^26) */
	tmp = (Shortword)((Longword)TEN_S5_10 * (Ltmp2 >> 16) >> 15);

	tmp2 = HIGH_TCE_DB - tmp;	/* HIGH_TCE_DB and tmp scaled as 10,5 */
	//following can't be modified, or bit-exact tests fail
	if (tmp2 > (short)0x03ff)
    {
        tmp2 = MAX_16;
    }
    else if (tmp2 < (short)0xfc00)
    {
        tmp2 = MIN_16;
    }
    else
    {
        tmp2 <<= 5;
    }
	tmp1 = (Shortword)((Longword)ALPHA_RAN_DIV_TCE_RAN * tmp2 >> 15);
	alpha = HIGH_ALPHA_S5_10 - tmp1;

	if (alpha > HIGH_ALPHA_S5_10)
	{
		alpha = HIGH_ALPHA;
	}
	else if (alpha < LOW_ALPHA_S5_10)
	{
		alpha = LOW_ALPHA;
	}
	else
	{
		alpha <<= 5;	/* rescale from 5,10 to 0,15 alpha is a fraction */
	}

/* Calc long term log spectral energy */
	tmp = SW_MAX - alpha;
	for (i = LO_CHAN; i <= HI_CHAN; i++)
	{
		Ltmp1 = (Longword)tmp * ch_enrg_db[i];
		Ltmp2 = (Longword)alpha * ch_enrg_long_db[i];

		if (((Ltmp1 ^ Ltmp2) & 0x40000000L) == 0)
	    {
	        Ltmp2 += Ltmp1;
	        if ((Ltmp1 ^ Ltmp2) & 0x40000000L)
            {
                ch_enrg_long_db[i] = (Ltmp1 > 0) ? MAX_16 : MIN_16;
            }
            else
            {
                ch_enrg_long_db[i] = (Shortword)(Ltmp2 >> 15);
            }
	    }
	    else
        {
            ch_enrg_long_db[i] = (Shortword)((Ltmp1 + Ltmp2) >> 15);
        }
	}

/* Set or reset the update flag */
	update_flag = FALSE;

	if (vm_sum <= UPDATE_THLD)
	{
		update_flag = TRUE;
		evrc_encobj->ns_update_cnt = 0;
	}
	else if (tce > NOISE_FLOOR_CHAN && ch_enrg_dev < DEV_THLD)
	{
		evrc_encobj->ns_update_cnt++;
		if (evrc_encobj->ns_update_cnt >= UPDATE_CNT_THLD)
			update_flag = TRUE;
	}

	if (evrc_encobj->ns_update_cnt == evrc_encobj->ns_last_update_cnt)
		evrc_encobj->ns_hyster_cnt++;
	else
		evrc_encobj->ns_hyster_cnt = 0;

	evrc_encobj->ns_last_update_cnt = evrc_encobj->ns_update_cnt;

	if (evrc_encobj->ns_hyster_cnt > HYSTER_CNT_THLD)
		evrc_encobj->ns_update_cnt = 0;

/* Set or reset modify flag */
	index_cnt = 0;

	for (i = MID_CHAN; i <= HI_CHAN; i++)
		if (ch_snr[i] >= INDEX_THLD)
			index_cnt++;

	modify_flag = (index_cnt < INDEX_CNT_THLD) ? TRUE : FALSE;

/* Modify the SNR indices */
	if (modify_flag == TRUE)
	{
		for (i = LO_CHAN; i <= HI_CHAN; i++)
			if ((vm_sum <= METRIC_THLD) || (ch_snr[i] <= SETBACK_THLD))
				ch_snr[i] = 1;
	}

/* Compute the channel gains */
	Ltmp1 = fnLog10(tne);
	Ltmp1 += LOG_OFFSET - 161614248;	/* -round32(log10(2^8)*2^26) */
	Ltmp1 = -Ltmp1;
	gain = L_mpy_ls(Ltmp1, TEN_S5_10);
	if (gain < MIN_GAIN)
		gain = MIN_GAIN;

	for (i = LO_CHAN; i <= HI_CHAN; i++)
	{
		if (ch_snr[i] <= SNR_THLD)
			ch_snr[i] = SNR_THLD;

		tmp = ch_snr[i] - SNR_THLD;
		Ltmp1 = (Longword)tmp * GAIN_SLOPE << 1;
		Ltmp2 = Ltmp1 << 5;	/* rescaled to 10,5 */
		Ltmp = Ltmp2 + gain;	/* gain scaled as 10,5 */
		if (Ltmp > 0)
			Ltmp = 0;

		Ltmp1 = L_mpy_ls(Ltmp, ONE_OVER_20);
		Ltmp1 <<= 5;	/* rescale Ltmp1 to 5,26 */
		if (Ltmp1 == 0)
			Ltmp1 = -1;
		Ltmp2 = fnExp10(Ltmp1);
		ftmp2 = (Shortword)(Ltmp2 >> 16);

		j1 = ch_tbl[i][0];
		j2 = ch_tbl[i][1];
		for (j = j1; j <= j2; j++)
		{
			ch_gain[j] = ftmp2;
		}
	}

/* Update the channel noise estimates */

	if (update_flag == TRUE)
	{
		for (i = LO_CHAN; i <= HI_CHAN; i++)
		{
			norm_shift = norm_l(ch_noise[i]);
			Ltmp = ch_noise[i] << norm_shift;
			Ltmp1 = (Ltmp >> 16) * (Longword)ONE_MINUS_CNE_SM_FAC << 1;
			Ltmp1 >>= norm_shift;

			norm_shift = norm_l(ch_enrg[i]);
			Ltmp = ch_enrg[i] << norm_shift;
			Ltmp2 = (Ltmp >> 16) * CNE_SM_FAC << 1;
			Ltmp2 >>= norm_shift;

			ch_noise[i] = Ltmp1 + Ltmp2;

			if (ch_noise[i] < MIN_NOISE_ENRG)
				ch_noise[i] = MIN_NOISE_ENRG;
		}
	}

/* Filter the input data in the frequency domain and perform IFFT */

//	for (i = 0; i < FFT_LEN / 2; i++)
	for (i = 0; i < (FFT_LEN >> 1); i++)
	{
		data_buffer[2 * i] = (Shortword)((Longword)data_buffer[2 * i] * ch_gain[i] >> 15);
		data_buffer[2 * i + 1] = (Shortword)((Longword)data_buffer[2 * i + 1] * ch_gain[i] >> 15);
	}

/* Block normalize data_buffer */
	norm_shift = block_norm(data_buffer, FFT_LEN, IFFT_HEADROOM);

/* Inverse FFT */
	r_fft(data_buffer, -1);

/* Block denormalize data_buffer */
/*	block_denorm(data_buffer, FFT_LEN, normb_shift + norm_shift); */
	block_denorm(data_buffer, FFT_LEN, (Shortword)(normb_shift + norm_shift + 1));

/* Overlap add the filtered data from previous block.
 * Save data from this block for the next. */

	for (i = 0; i < FFT_LEN - FRM_LEN; i++)
	{
		data_buffer[i] += overlap[i];
	}

	for (i = FRM_LEN; i < FFT_LEN; i++)
	{
		overlap[i - FRM_LEN] = data_buffer[i];
	}

/* Deemphasize the filtered speech and write it out to farray */
	tmp = (Shortword)((Longword)DE_EMP_FAC * evrc_encobj->ns_de_emp_mem >> 15);
	*farray_ptr = data_buffer[0] + tmp;

	for (i = 1; i < FRM_LEN; i++)
	{
		tmp = (Shortword)(((Longword)DE_EMP_FAC * (*(farray_ptr + i - 1)) + 0x4000) >> 15);
		*(farray_ptr + i) = data_buffer[i] + tmp;
	}

	evrc_encobj->ns_de_emp_mem = *(farray_ptr + FRM_LEN - 1);

	evrc_encobj->ns_first = FALSE;

}								/* end noise_suprs () */


