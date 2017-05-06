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
/* decode.c - main CELP decoder */

#include <stdio.h>
#include "basic_op.h"
#include "celp.h"
#include "quantize.h"
#include "cb.h"

void run_decoder(
    short                   mode,
    struct  DECODER_MEM     *d_mem,
    short                   lpc[2][LPCORDER],
    struct  PITCHPARAMS     *pitch_params,
    struct  CBPARAMS        cb_params[MAXCBPERPIT][MAXNUMCB],
    short                   *out_buffer,
    short                   length,
    short                   numcbsf,
    short                   numcb
)
{
  int   tmp_cbOut, tmp_cbpara;
  short *indata1, *outdata;
  int n_shiftcnt, d_shiftcnt;
  int j, k, l;
  short cb_out[FSIZE], pitch_out[FSIZE];
  short white_out[FSIZE];
  short stmp, lencbsf, tmpG;
  
  lencbsf = length >> (numcb - 1);
  for (k=0; k<numcbsf; k++) {
	  outdata = cb_out + k*lencbsf;
	  for (j=lencbsf; j; j--) {
		  *outdata++ =0;
	  }
	  
	  if (mode!=QUARTERRATE_UNVOICED && mode != EIGHTH) {
		  for (l = numcb-1; l >= 0; l--) {
			  tmpG = cb_params[k][l].G;
			  n_shiftcnt = CBLENGTH - cb_params[k][l].i;
			  d_shiftcnt = CBLENGTH - n_shiftcnt;
			  if(mode == FULLRATE_VOICED || mode == BLANK){				  
				  if(d_shiftcnt >= lencbsf)
				  {
					  indata1 = CODEBOOK + n_shiftcnt;
					  outdata = cb_out + k*lencbsf;
					  for (j=lencbsf; j; j--) {
						  *outdata = ((int)(*indata1) * tmpG + 16384) >> 15 ;
						  outdata++;
						  indata1++;
					  }
				  }
				  else
				  {
					  indata1 = CODEBOOK + n_shiftcnt;
					  outdata = cb_out + k*lencbsf;					  
					  for (j=d_shiftcnt; j; j--) {
						  *outdata = ((int)(*indata1) * tmpG + 16384) >> 15 ;
						  outdata++;
						  indata1++;
					  }
					  
					  indata1 = CODEBOOK;
					  for(j = lencbsf - d_shiftcnt; j; j--) {
						  *outdata = ((int)(*indata1) * tmpG + 16384) >> 15 ;
						  outdata++;
						  indata1++;
					  }
				  }
			  }
			  else{/* HALFRATE_VOICED */
				  if(d_shiftcnt >= lencbsf)
				  {
					  indata1 = CODEBOOK_HALF + n_shiftcnt;
					  outdata = cb_out + k*lencbsf;
					  for (j=lencbsf; j; j--) {
						  *outdata = ((int)(*indata1) * tmpG + 16384) >> 15 ;
						  outdata++;
						  indata1++;
					  }
				  }
				  else
				  {
					  indata1 = CODEBOOK_HALF + n_shiftcnt;
					  outdata = cb_out + k*lencbsf;					  
					  for (j=d_shiftcnt; j; j--) {
						  *outdata = ((int)(*indata1) * tmpG + 16384) >> 15 ;
						  outdata++;
						  indata1++;
					  }
					  
					  indata1 = CODEBOOK_HALF;
					  for(j = lencbsf - d_shiftcnt; j; j--) {
						  *outdata = ((int)(*indata1) * tmpG + 16384) >> 15 ;
						  outdata++;
						  indata1++;
					  }
				  }	
			  }
		  }
	  }
	  else if(mode == EIGHTH) {/* EIGHTH rate */
		  tmp_cbpara = (int)cb_params[0][0].sd;
		  tmpG = cb_params[0][0].G;
		  outdata = cb_out + k*lencbsf;

		  for (j=lencbsf; j; j--) {
			  tmp_cbpara=(521*tmp_cbpara+259)&(0xffff);
			  stmp = (((tmp_cbpara+0x7fff)&(0xffff))-0x7fff);
			  tmp_cbOut = stmp*22;
			  tmp_cbOut = L_mpy_ls(tmp_cbOut, tmpG);
			  *outdata++ = (short)((tmp_cbOut + 128) >> 8);
		  }

		  cb_params[0][0].sd = tmp_cbpara;
	  }
	  else if (mode == QUARTERRATE_UNVOICED){  /* QUARTERRATE_UNVOICED */
		  tmp_cbpara = (int)cb_params[0][0].sd;
		  outdata = white_out + k*lencbsf;
		  for (j=lencbsf; j; j--) {
			  tmp_cbpara = (521*tmp_cbpara+259)&(0xffff);
			  stmp = (((tmp_cbpara+0x7fff)&(0xffff))-0x7fff);
			  *outdata++ = stmp;
		  }
		  cb_params[0][0].sd = tmp_cbpara;
		  
  		  outdata = white_out + k*lencbsf;
		  indata1 = cb_out + k*lencbsf;
		  do_fir_linear_filter(outdata, indata1, lencbsf, &(d_mem->bpf_unv), UPDATE);

		  tmpG = cb_params[0][0].G;
		  for (j=lencbsf; j; j--) {
			  tmp_cbOut = (int)(*indata1) * 22;
			  tmp_cbOut = L_mpy_ls(tmp_cbOut, tmpG);
			  *indata1++ = (short)((tmp_cbOut + 128) >> 8);
		  }
	  }
  }  
  
  d_mem->pitch_filt.delay=pitch_params->lag;
  d_mem->pitch_filt.coeff=pitch_params->b;
  d_mem->pitch_filt.frac=pitch_params->frac;
 
#ifdef ASM_OPT // asm opt .lhp
  filter_1_tap_interp_asm(cb_out, pitch_out, length, &(d_mem->pitch_filt), UPDATE);
#else
  do_pole_filter_1_tap_interp(cb_out, pitch_out, length, &(d_mem->pitch_filt), UPDATE);
#endif
  
  if (d_mem->type==ENCODER) {
	  /* generate unweighted speech, if we want the encoder's */
	  /* synthesized speech */
	  indata1 = lpc[NOT_WGHTED];
	  outdata = d_mem->lpc_filt.pole_coeff;
	  //for (j=LPCORDER; j; j-= 10)
	  {
		  *outdata++ = *indata1++; *outdata++ = *indata1++;
		  *outdata++ = *indata1++; *outdata++ = *indata1++;
		  *outdata++ = *indata1++; *outdata++ = *indata1++;
		  *outdata++ = *indata1++; *outdata++ = *indata1++;
		  *outdata++ = *indata1++; *outdata++ = *indata1++;
	  }

//	  indata1 = lpc[WGHTED];
//	  outdata = d_mem->wght_syn_filt.pole_coeff;
//  	  //for (j=LPCORDER; j; j-= 10)
//	  {
//		  *outdata++ = *indata1++; *outdata++ = *indata1++;
//		  *outdata++ = *indata1++; *outdata++ = *indata1++;
//		  *outdata++ = *indata1++; *outdata++ = *indata1++;
//		  *outdata++ = *indata1++; *outdata++ = *indata1++;
//		  *outdata++ = *indata1++; *outdata++ = *indata1++;
//	  }
#ifdef  ASM_OPT  //asm opt
	  do_pole_filter_asm(pitch_out, out_buffer, length, 
		  &(d_mem->lpc_filt), UPDATE);
	  
	  do_pole_filter_asm(pitch_out, d_mem->pw_speech_out, length, 
		  &(d_mem->wght_syn_filt), UPDATE);
#else
	  do_pole_filter(pitch_out, out_buffer, length, 
		  &(d_mem->lpc_filt), UPDATE);
	  
	  do_pole_filter(pitch_out, d_mem->pw_speech_out, length, 
		  &(d_mem->wght_syn_filt), UPDATE);
#endif
  }
  
}/* end of run_decoder() */





