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
/* target.c - create and update the "desired" waveform for the searches */

#include <stdio.h>
//#include <malloc.h>
#include "basic_op.h"
#include "celp.h"

static short zir[FSIZE];

void create_target_speech(
						  short  mode,
						  short  *input,
						  short  *resid,
						  short  *target,
						  short  lpc[2][MAXSF][2][LPCORDER],
                          struct ZERO_FILTER *err_filt,
                          struct POLE_FILTER *wght_filt)
{
	short	*indata1, *indata2;
	short	*outdata1, *outdata2;
	int		i,j;
	int		sf_ptr = 0, FDP;	
	FDP = FSIZE_DIV_PITCHSF[mode];
	for (i=0; i<PITCHSF[mode]; i++)
	{
		indata1 = lpc[PITCH][i][NOT_WGHTED];
		indata2 = lpc[PITCH][i][WGHTED];
		outdata1 = err_filt->zero_coeff;
		outdata2 = wght_filt->pole_coeff;
		for (j = LPCORDER; j; j--) {
			*outdata1++ = - (*indata1++); 
			*outdata2++ = *indata2++; 
		}

#ifdef  ASM_OPT // the asm branch 		
		do_zero_filter_asm(&input[sf_ptr], &resid[sf_ptr], FDP,err_filt, UPDATE);
#else
		do_zero_filter(&input[sf_ptr], &resid[sf_ptr], FDP, err_filt, UPDATE);
#endif

#ifdef  ASM_OPT  //the asm branch have some bug ,lhp
		do_pole_filter_asm(&resid[sf_ptr], &target[sf_ptr], FDP, wght_filt, UPDATE);	
#else
		do_pole_filter(&resid[sf_ptr], &target[sf_ptr], FDP, wght_filt, UPDATE);	
#endif	
		sf_ptr += FDP;		
	}
}


void update_target_pitch(
						 short              mode,
						 short              *pw_speech,
						 short              *target,
						 short              *lpc,
                         struct DECODER_MEM *d_mem)
{
    short zir[FSIZE];
	short *indata1, *indata2, *outdata;
	short   i;
    
	indata1 = lpc;
	outdata = d_mem->wght_syn_filt.pole_coeff;
	for (i=LPCORDER; i; i--) {
		*outdata++ = *indata1++;
    }

    get_zero_input_response_pole(zir, FSIZE_DIV_PITCHSF[mode], 
		&(d_mem->wght_syn_filt));
	
	indata1 = pw_speech;
	indata2 = zir;
	outdata = target;
	for (i=FSIZE_DIV_PITCHSF[mode]; i; i-=4) {
		*outdata++ = (*indata1++) - (*indata2++);
		*outdata++ = (*indata1++) - (*indata2++);
		*outdata++ = (*indata1++) - (*indata2++);
		*outdata++ = (*indata1++) - (*indata2++);
	}
}

void update_target_cb(
					  short               mode,
					  short               *pw_speech,
					  short               *target,
					  short               *lpc,
                      struct PITCHPARAMS  *pitch_params,
                      struct DECODER_MEM  *d_mem)
{
	static short pitch_est[FSIZE];
	static short tmpmemory[20];
	short  *indata, *indata1, *outdata;
	int   i;
	d_mem->pitch_filt.delay = pitch_params->lag;
	d_mem->pitch_filt.coeff = pitch_params->b;
	d_mem->pitch_filt.frac = pitch_params->frac;

	get_zero_input_response_pole_1_tap_interp(zir, FSIZE_DIV_CBSF[mode],&(d_mem->pitch_filt)); 

	indata = d_mem->wght_syn_filt.memory;
	outdata = tmpmemory;
	for (i=d_mem->wght_syn_filt.order*2; i; i-= 10) {
		*outdata++ = *indata++; *outdata++ = *indata++;
		*outdata++ = *indata++; *outdata++ = *indata++;
		*outdata++ = *indata++; *outdata++ = *indata++;
		*outdata++ = *indata++; *outdata++ = *indata++;
		*outdata++ = *indata++; *outdata++ = *indata++;
	}
#ifdef ASM_OPT  // asm opt
	do_pole_filter_asm(zir, pitch_est, FSIZE_DIV_CBSF[mode],&(d_mem->wght_syn_filt), NO_UPDATE);
#else
	do_pole_filter(zir, pitch_est, FSIZE_DIV_CBSF[mode], &(d_mem->wght_syn_filt), NO_UPDATE);
#endif
	indata = tmpmemory;
	outdata = d_mem->wght_syn_filt.memory;
	for (i=d_mem->wght_syn_filt.order*2; i; i-= 10) {
		*outdata++ = *indata++; *outdata++ = *indata++;
		*outdata++ = *indata++; *outdata++ = *indata++;
		*outdata++ = *indata++; *outdata++ = *indata++;
		*outdata++ = *indata++; *outdata++ = *indata++;
		*outdata++ = *indata++; *outdata++ = *indata++;
	}	
	indata = pw_speech;
	indata1 = pitch_est;
	outdata = target;
	for (i=FSIZE_DIV_CBSF[mode]; i; i-=2) {
		*outdata++ = (*indata++) - (*indata1++);
		*outdata++ = (*indata++) - (*indata1++);
	}	
}/* end of update_target_cb() */

void update_form_resid_mems(
							short               *input,
                            struct ZERO_FILTER  *err_filt)
{
	short *indata, *outdata;
	indata = input + FSIZE - 1;
	outdata = err_filt->memory;
	*outdata++ = *indata--; *outdata++ = *indata--;
	*outdata++ = *indata--; *outdata++ = *indata--;
	*outdata++ = *indata--; *outdata++ = *indata--;
	*outdata++ = *indata--; *outdata++ = *indata--;
	*outdata++ = *indata--; *outdata++ = *indata--;
}


