/**********************************************************************
Each of the companies; Qualcomm, and Lucent (hereinafter 
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
all Normative portions of the QCELP-13 Variable Rate Speech Codec as 
it exists on the date of submittal of this form, should the QCELP-13 be 
approved as a Specification or Report by 3GPP2, or as a transposed 
Standard by any of the 3GPP2's Organizational Partners, the Source(s) 
state(s) that a worldwide license to reproduce, use and distribute the 
Software, the license rights to which are held by the Source(s), will 
be made available to applicants under terms and conditions that are 
reasonable and non-discriminatory, which may include monetary compensation, 
and only to the extent necessary for the practice of any or all of the 
Normative portions of the QCELP-13 or the field of use of practice of the 
QCELP-13 Specification, Report, or Standard.  The statement contained above 
is irrevocable and shall be binding upon the Source(s).  In the event 
the rights of the Source(s) in and to copyright or copyright license 
rights subject to such commitment are assigned or transferred, the 
Source(s) shall notify the assignee or transferee of the existence of 
such commitments.
*******************************************************************/
/* snr.c - compute the snr values                               */

#include "basic_op.h"
#include "celp.h"

Word16 divide_s_cb(Word16 var1, Word16 var2)
{
	Word32 L_div;
	Word16 swOut;	
	if (var1 < 0 || var2 < 0 || var1 > var2)
	{
		return (0);
	}
	if (var1 == var2)
	{
		return (0x7fff);
	}
	L_div = ((0x00008000L * (Word32) var1) / (Word32) var2);
	swOut = saturate(L_div);
	return (swOut);
}
void target_reduction(
					  short  *after_search,
					  short  *before_search,
					  int    *energy_before,
					  int    *energy_after,
					  short  length)
{	
	int i;
	short  *indata1, *indata2;	
	*energy_after = 0;
	*energy_before = 0;
	indata1 = before_search;
	indata2 = after_search;

	for(i=length; i; i--){
		*energy_before = L_add(*energy_before, (int)(*indata1) * (int)(*indata1));
		*energy_after = L_add(*energy_after, (int)(*indata2) * (int)(*indata2));
		indata1++; indata2++;
	}	
}

void save_target(
				 short *target,
				 short *gt_target,
				 short length)
{
	int i;
	short *indata, *outdata;
	indata = target;
	outdata = gt_target;
	for(i=length; i; i--){
		*outdata++ = *indata++;
	}
}

void save_pitch(
				struct PITCHPARAMS  *pitch_params,
				short               *pitch_lag,
				short               *pitch_gain)
{
	*pitch_lag = pitch_params->lag + pitch_params->frac/MAX_FR_RESOLUTION;
	*pitch_gain = pitch_params->b;	
}

void compute_target_snr(
						short               mode,
						struct ENCODER_MEM  *e_mem)
{	
	int Ex=0, Ey=0;
	int i;
	int tmp_target_snr;
	int *indata1, *indata2;
	short n_shiftcnt, d_shiftcnt, nd;
	short s_Ex, s_Ey;
	indata1 = e_mem->codebook_target_energy;
	indata2 = e_mem->codebook_target_energy_after;

	for(i = CBSF[mode]; i; i--){
		Ex = L_add(Ex, *indata1);
		//Ey = L_add(Ey, *indata2);
		indata1++; 
		//indata2++;
	}

	for (i = CBSF[mode]; i; i--)
	{
		Ey = L_add(Ey, *indata2);
		indata2++;
	}

	n_shiftcnt = (norm_l(Ex) - 1);
	d_shiftcnt = norm_l(Ey);
	s_Ex = extract_h(Ex << n_shiftcnt);
	s_Ey = extract_h(Ey << d_shiftcnt);
	tmp_target_snr = divide_s(s_Ex, s_Ey);
	nd = 7 - (d_shiftcnt - n_shiftcnt);
	if(nd >=  0)
		tmp_target_snr = tmp_target_snr >> nd;
	else
		tmp_target_snr = tmp_target_snr << nd;

	tmp_target_snr = fnLog10(tmp_target_snr) + 464640964;	
	tmp_target_snr = L_mpy_ls(tmp_target_snr, 5);
	e_mem->target_snr = tmp_target_snr;
}
