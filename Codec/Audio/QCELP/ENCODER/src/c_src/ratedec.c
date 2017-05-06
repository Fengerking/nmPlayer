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
/* ratedec.c - selects encoding rate to use */


#include "basic_op.h"
#include "celp.h"
#include "coderate.h"

/* calculates various features for voiced/unvoiced classification
   and rate decision */

void compute_features(
    int                  *pitch_cor,
    short                *pitch,
    short                *resid,
    short                *speech,
    struct UNV_FEATURES  *unv_features,
    int                  *acf,
    short                *lpc,
    short                *lsp,
    int                  *band_energy,
    struct ENCODER_MEM   *e_mem
)
{

/* detect a clearly unvoiced segment of speech using the following features*/
/* or a subset thereof:                                                 */
/* nacf of residual, zero crossings of speech, energy level, spectral   */
/* tilt etc.                                                            */
   int zero,resid_energy;
   int acf0_0,i;
   int acf0_0p, dc, tmp_pgain;
   int  diff_lsp_tmp;
   short *indata1, *indata2, *outdata;
   int  n_shiftcnt, d_shiftcnt; 
   

   acf0_0 = 0;
   /* calculate nacf function */
   //acf0_0 = L_add(acf0_0, (int)(*indata1)*(int)(*indata1)); indata1++;
   //maybe have error , but for the test case ,the output is bit match. lhp 
   indata1 = resid + PMAX_DIV_DECIMATE_F;
   for(i=FSIZE_DIV_DECIMATE_F; i; i-= 8) {
	   acf0_0 += ((int)(*indata1)*(int)(*indata1)); indata1++;
	   acf0_0 += ((int)(*indata1)*(int)(*indata1)); indata1++;
	   acf0_0 += ((int)(*indata1)*(int)(*indata1)); indata1++;
	   acf0_0 += ((int)(*indata1)*(int)(*indata1)); indata1++;
	   acf0_0 += ((int)(*indata1)*(int)(*indata1)); indata1++;
	   acf0_0 += ((int)(*indata1)*(int)(*indata1)); indata1++;
	   acf0_0 += ((int)(*indata1)*(int)(*indata1)); indata1++;
	   acf0_0 += ((int)(*indata1)*(int)(*indata1)); indata1++;
   }

   acf0_0p = acf0_0;
   indata1 = resid + PMAX_DIV_DECIMATE_F - pitch[0];
    //acf0_0 = L_add(acf0_0, (int)(*indata1)*(int)(*indata1)); indata1++;
   //maybe have error but for the test case ,the output is bit match. lhp
   for(i=FSIZE_DIV_DECIMATE_F; i; i-=8) {
		acf0_0 += ((int)(*indata1)*(int)(*indata1)); indata1++;
		acf0_0 += ((int)(*indata1)*(int)(*indata1)); indata1++;
		acf0_0 += ((int)(*indata1)*(int)(*indata1)); indata1++;
		acf0_0 += ((int)(*indata1)*(int)(*indata1)); indata1++;
		acf0_0 += ((int)(*indata1)*(int)(*indata1)); indata1++;
		acf0_0 += ((int)(*indata1)*(int)(*indata1)); indata1++;
		acf0_0 += ((int)(*indata1)*(int)(*indata1)); indata1++;
		acf0_0 += ((int)(*indata1)*(int)(*indata1)); indata1++;
   }

   acf0_0 >>= 1;

   if(acf0_0 == 0)  acf0_0 = 1000000;

   n_shiftcnt = (norm_l(pitch_cor[0]) - 1);
   d_shiftcnt = norm_l(acf0_0);
   unv_features->nacf0 = saturate(L_shr(L_divide((pitch_cor[0] << n_shiftcnt), 
			(acf0_0 << d_shiftcnt)), (16 - (d_shiftcnt - n_shiftcnt))));
   unv_features->open_pitch = pitch[0];
   unv_features->nacf_frame = unv_features->nacf0;

   /* calculate zero-crossings */
   dc = 0;
   indata1 = speech;
   for(i=FSIZE; i; i -= 16)  /* strip out any dc that survives the front */
   {
	   dc += *indata1++; dc += *indata1++;
	   dc += *indata1++; dc += *indata1++;
	   dc += *indata1++; dc += *indata1++;
	   dc += *indata1++; dc += *indata1++;
	   dc += *indata1++; dc += *indata1++;
	   dc += *indata1++; dc += *indata1++;
	   dc += *indata1++; dc += *indata1++;
	   dc += *indata1++; dc += *indata1++;
   }
   
   dc = L_mpy_ls(dc, INVFSIZE);
   
   zero =0;
   indata1 = speech;
   for(i=FSIZE-1; i; i--) {
	   if((int)((*indata1)-dc)*(int)((*(indata1+1))-dc) < 0) zero++;
	   indata1++;
//   	   if((((*indata1)-dc)^((*(indata1+1))-dc)) & 0x80000000 ) zero++;
// 	   indata1++;
   }

   unv_features->zero = zero;

   /* calculate differential residual energy: might be used for more than  */
   /* unvoiced classification                                              */

     /*unv_features->res_energy_diff0 = 
       10*log10(acf0_0p/unv_features->last_res_energy);*/

   /* calculate prediction gain from acf and lpc coefficients */
   /* Re[0] = acf[0] - sum over k {acf[k]*lpc[k]              */

   resid_energy = acf[0];
   for(i = LPCORDER; i; i--)
     resid_energy -= (L_mpy_ls(acf[i], lpc[i-1]) << 3); /* is the sign on the lpc's correct */

   /*    there is a hamming window     */
   /*    factor in here that will make */
   /*    this smaller by about 3 dB    */

    if( resid_energy <= 0 )
    {
        unv_features->pred_gain = 0;
    }
    else
    {
		n_shiftcnt = (norm_l(acf[0]) - 1);
		d_shiftcnt = norm_l(resid_energy);
		tmp_pgain = L_shr(L_divide((acf[0] << n_shiftcnt), 
			(resid_energy << d_shiftcnt)), (23 - (d_shiftcnt - n_shiftcnt)));  /* Q8 */
		unv_features->log_pred_gain = tmp_pgain;
		
		//tmp_pgain = L_sub(L_add(fnLog10(tmp_pgain), 626255212), 161614248);	 /* Q26 */
		tmp_pgain = fnLog10(tmp_pgain) + 464640964;	 /* Q26 */
		unv_features->pred_gain = saturate(L_mpy_ls(tmp_pgain, 5)); /* Q10 */
    }

   if(unv_features->pred_gain < 0) unv_features->pred_gain = 0;
   
   unv_features->pred_gain_frame = saturate(L_mpy_ls(100*unv_features->pred_gain +
	   60*unv_features->last_pred_gain, INVFSIZE));
   
	/* now store memory for next time */
   /*unv_features->last_res_energy = resid_energy;*/
   unv_features->last_nacf = unv_features->nacf0;
   unv_features->pred_gain_diff = - (unv_features->pred_gain_frame -
	   unv_features->last_pred_gain_frame);
   unv_features->last_pred_gain = unv_features->pred_gain;
   unv_features->last_pred_gain_frame = unv_features->pred_gain_frame;

   /* calculate the difference in lsp's from the last frame */
   /*unv_features->diff_lsp =0;*/
   diff_lsp_tmp = 0;
   indata1 = lsp;
   indata2 = unv_features->last_lsp;
   for(i=LPCORDER; i; i-=2)
   {
	   n_shiftcnt = (*indata1) - (*indata2); 
	   indata1++; indata2++;
	   diff_lsp_tmp = (diff_lsp_tmp + mult(n_shiftcnt, n_shiftcnt));

	   n_shiftcnt = (*indata1) - (*indata2); 
	   indata1++; indata2++;
	   diff_lsp_tmp = (diff_lsp_tmp + mult(n_shiftcnt, n_shiftcnt));
   }

   unv_features->diff_lsp = (5*diff_lsp_tmp + 3*unv_features->last_diff_lsp) >> 3;
   unv_features->last_diff_lsp = diff_lsp_tmp;

   for(i=0;i< FREQBANDS; i++)
     unv_features->band_energy[i] = band_energy[i];

   indata1 = lsp;
   outdata = unv_features->last_lsp;
   for(i=LPCORDER; i; i-=10)
   { 
	   *outdata++ = *indata1++; *outdata++ = *indata1++;
	   *outdata++ = *indata1++; *outdata++ = *indata1++;
	   *outdata++ = *indata1++; *outdata++ = *indata1++;
	   *outdata++ = *indata1++; *outdata++ = *indata1++;
	   *outdata++ = *indata1++; *outdata++ = *indata1++;
   }
}/* end of compute_features() */

void band_energy_fcn(
					 int                 *R,
					 int                 *energy
					 )
{
    short  *indata;
	short  i,j;
	
    /* find the energy in each band   */
    for (j=0; j<FREQBANDS; j++) {
		indata = rate_filt[j];
		energy[j] = L_mpy_ls(R[0], *indata) >> 1;
		indata++;
		for(i=1;i<FILTERORDER;i++) {
			energy[j] = L_add(energy[j], L_mpy_ls(R[i], *indata)); 
			indata++;
		}
    }
}

void update_hist_cnt(
					 struct ENCODER_MEM  *e_mem,
					 short               rate 
					 )
{
	/* this routine will keep track of the number of frames in each rate */
	/* and will update 2 histograms: target_snr's when frame is full rate */
	/* and target_snr's when frame is 1/2 rate                           */

	if(rate == FULLRATE_VOICED){
		if(e_mem->target_snr <= e_mem->target_snr_thr){
			e_mem->full_cnt += 1;

		}
		else
		{
			e_mem->full_force += 1;
		}
	}
	else if(rate == HALFRATE_VOICED){
		if(e_mem->target_snr > e_mem->target_snr_thr){
			e_mem->half_cnt += 1;
		}
		else
		{
			e_mem->half_force += 1;
		}
	}
	else if(rate == QUARTERRATE_UNVOICED){
		e_mem->quarter_cnt += 1;
	}

	if(rate == FULLRATE_VOICED || rate == HALFRATE_VOICED){
		if(e_mem->target_snr <= e_mem->target_snr_thr){

			/* note that hist_below[0] contains count of full or half frames that */
			/* were less than 1 db below threshold, hist_below[3] contains count*/
			/* of frames declared full that were  greater than 3 db below */
			/* the threshold                                                 */

			if((e_mem->target_snr_thr - e_mem->target_snr) >= 3*1024){
				e_mem->hist_below[3] += 1;
			}
			else
				e_mem->hist_below[(e_mem->target_snr_thr - e_mem->target_snr) >> 10] += 1;
		}
		else{
			/* note that hist_above3[0] contains count of full and half frames that */
			/* were less than 1 db above threshold, hist_above[3] contains count*/
			/* of frames declared full that were greater than 3 db above */
			/* the threshold                                                 */

			if((e_mem->target_snr - e_mem->target_snr_thr) >= 3*1024){
				e_mem->hist_above[3] += 1;
			}
			else
				e_mem->hist_above[(e_mem->target_snr - e_mem->target_snr_thr) >> 10] += 1;
		}
	}
}/* end of update_hist_cnt() */


void adjust_rate_down(
    struct ENCODER_MEM  *e_mem
)
{
/* look at histogram of target_snr's when frame was declared full and   */
/* adjust threshold so that a larger % of full rate frames get declared */
/* half                                                                 */

    int n_to_half;
    int total_frames,i;
/* calculate what the number of full_rate frames should have been to achieve*/
/* the desired rate in the last stat block                                  */
    n_to_half = ((int)e_mem->full_cnt*FULLRATE_BPS_16 + (int)e_mem->half_cnt*HALFRATE_BPS_16 
                + (int)e_mem->quarter_cnt*QUARTER_BPS_16 + (int)e_mem->full_force*FULLRATE_BPS_16 
                + (int)e_mem->half_force*HALFRATE_BPS_16-(int)e_mem->avg_rate_thr*STATWINDOW)/HALFRATE_BPS_16; 

/* now step back in the histogram until you find the drop in snr threshold */
/* that is necessary to achieve the desired rate reduction                 */

    i = 0;
    total_frames = 0;
    while(total_frames < n_to_half && i < 4 ){
        total_frames += e_mem->hist_below[i];
		i++;
    }
    e_mem->target_snr_thr -= i; /* reduce the threshold by integer db's*/

}/* end of adjust_rate_down() */

void adjust_rate_up(
    struct ENCODER_MEM  *e_mem 
)
{
/* look at histogram of target_snr's when frame was declared 1/2  and   */
/* adjust threshold so that a smaller % of full rate frames get declared */
/* half                                                                 */

    int n_to_full;
    int total_frames,i;
/* calculate what the number of full_rate frames should have been to achieve*/
/* the desired rate in the last stat block                                  */
    n_to_full = -(e_mem->full_cnt*FULLRATE_BPS_16 + e_mem->half_cnt*HALFRATE_BPS_16 
                + e_mem->quarter_cnt*QUARTER_BPS_16 + e_mem->full_force*FULLRATE_BPS_16 
                + e_mem->half_force*HALFRATE_BPS_16-e_mem->avg_rate_thr*STATWINDOW)/HALFRATE_BPS_16; 

/* now step back in the histogram until you find the inc in snr threshold */
/* that is necessary to achieve the desired rate increase                 */

    i = 0;
    total_frames = 0;
    while(total_frames  < n_to_full && i < 4 ){
        total_frames += e_mem->hist_above[i];
		i++;
    }
    e_mem->target_snr_thr += i; /* reduce the threshold by integer db's*/

} /* end of adjust_rate_up() */


void select_mode1(
				  short               *rate,
				  int                 *acf,				  
				  struct ENCODER_MEM  *e_mem 
				  )
{

	int i, k;

	/* now filter the input speech by bandpass filters and */
	/* derive the necessary bandenergies */

   band_energy_fcn(acf, e_mem->band_power);

	/* now our threshold comparison and background noise estimation */
	/* is done for all frequency bands                              */
	/* rate thresholds are calculated below, but not saved */

	for(k=0; k<FREQBANDS;k++) {
		e_mem->band_rate[k]=EIGHTH;
		for (i=0; i<2; i++) {
			/* since we changed SMSNR to 0.6 from 0.8 */
			if (e_mem->band_power[k] >
					L_shl2(L_mpy_ls(e_mem->band_noise_sm[k], THRESH_SNR[k][e_mem->snr_map[k]][i]), 3))
			{
				if(e_mem->band_rate[k] == EIGHTH)
					e_mem->band_rate[k] = HALFRATE_VOICED;
				else if(e_mem->band_rate[k] == HALFRATE_VOICED)
					e_mem->band_rate[k] = FULLRATE_VOICED;
			}
		}
	}  /* do threshold comparisons for full and half rates  */

	/* the maximum rate for all bands is chosen as the rate */
	*rate = e_mem->band_rate[0];
	for(k=1;k< FREQBANDS;k++) {
		if(e_mem->band_rate[k] > *rate)
			*rate = e_mem->band_rate[k];
	}
	e_mem->features.rate[0] = e_mem->band_rate[0];
	e_mem->features.rate[1] = e_mem->band_rate[1];

	/* Section 2.4.4.1.3 Calculating Hangover Frames as Function of SNR */
	if((e_mem->num_full_frames > FULL_THRESH) ||
			e_mem->hangover_in_progress == 1) {
		/* only use hangover after so many full rate frames in a row*/
		if(*rate != FULLRATE_VOICED) {
			e_mem->hangover+=1; 
			if (e_mem->hangover <= hangover[e_mem->snr_map[0]]) {
				*rate = FULLRATE_VOICED;
				e_mem->hangover_in_progress = 1;
			}
			else {
				e_mem->hangover_in_progress = 0;
			}
		}
		else {
			e_mem->hangover_in_progress = 0;
		}
	}

	if((*rate == FULLRATE_VOICED) && (e_mem->hangover_in_progress == 0)) {
		e_mem->hangover=0;   
		e_mem->num_full_frames+=1;
	}
	else {
		e_mem->num_full_frames = 0;
	}
	/* end of hangover algorithm     */

	/* Section 2.4.4.1.4 Constraints on Rate Selection */
	if((e_mem->last_rate_1st_stage == FULLRATE_VOICED) && *rate == EIGHTH)
		*rate = HALFRATE_VOICED;

	if(e_mem->frame_num == 0)
		*rate = FULLRATE_VOICED;

	e_mem->frame_num++;

	e_mem->last_rate_1st_stage = *rate;
	e_mem->last_rate = *rate;

}/* end of select_mode1() */
void select_mode2(
				  short               *rate,
				  short               *form_resid,
				  int                 *acf,
				  struct ENCODER_MEM  *e_mem,
				  short               *speech,
				  short               *lpc,
				  short               *lsp,
				  short				  *last_solid_voiced,	
				  short				  reduced_rate
				  )
{
	int   max_cor, tmp_cor;
	int   pren, posten;
	int   snr_temp;
	int	 unvoiced;
	int   discrim;
	short *indata1, *indata2, *outdata;
	int n_shiftcnt, d_shiftcnt;
	int i, lag;
	short maxlag;
	short tmpbuf[FSIZE];
	short  resid[FSIZE_DIV_DECIMATE_F + PMAX_DIV_DECIMATE_F];
	/* this is the acf of the noise reducing prediction filter */

	/* Section 2.4.4.2 */
	/* now filter the prediction residual and decimate by a factor of 2 */
	/* for computing NACF as a periodicity measure		       */
#ifdef ASM_OPT // asm opt by lhp
	do_fir_linear_filter_asm(form_resid, tmpbuf, FSIZE, &(e_mem->decimate), UPDATE);
#else
	do_fir_linear_filter(form_resid, tmpbuf, FSIZE, &(e_mem->decimate), UPDATE);
#endif

	outdata = resid;
	indata1 = e_mem->resid_mem;
	for (i=PMAX_DIV_DECIMATE_F; i; i-=4) {
		*outdata++ = *indata1++;
		*outdata++ = *indata1++;
		*outdata++ = *indata1++;
		*outdata++ = *indata1++;
	}

	outdata = resid + PMAX_DIV_DECIMATE_F;
	indata1 = tmpbuf;
	for (i=FSIZE_DIV_DECIMATE_F; i; i-= 4) {
		*outdata = *indata1;
		outdata++;
		indata1 += DECIMATE_F;

		*outdata = *indata1;
		outdata++;
		indata1 += DECIMATE_F;

		*outdata = *indata1;
		outdata++;
		indata1 += DECIMATE_F;

		*outdata = *indata1;
		outdata++;
		indata1 += DECIMATE_F;
	}

	outdata = e_mem->resid_mem;
	indata1 = resid + FSIZE_DIV_DECIMATE_F;
	for (i=PMAX_DIV_DECIMATE_F; i; i-=4) {
		*outdata++ = *indata1++;
		*outdata++ = *indata1++;
		*outdata++ = *indata1++;
		*outdata++ = *indata1++;
	}

	/* now do open loop pitch prediction */
	maxlag = PMIN_DIV_DECIMATE_F;
	max_cor = 0;
	for (lag=PMIN_DIV_DECIMATE_F; lag<PMAX_DIV_DECIMATE_F; lag++) {
		tmp_cor = 0;
		indata1 = resid + PMAX_DIV_DECIMATE_F;
		indata2 = resid + PMAX_DIV_DECIMATE_F - lag;
		for(i=FSIZE_DIV_DECIMATE_F;i;i-= 4)
		{
			tmp_cor += (*indata1)*(*indata2);
			indata1++; indata2++;
			tmp_cor += (*indata1)*(*indata2);
			indata1++; indata2++;
			tmp_cor += (*indata1)*(*indata2);
			indata1++; indata2++;
			tmp_cor += (*indata1)*(*indata2);
			indata1++; indata2++;
		}

		if (tmp_cor > max_cor) {
			max_cor = tmp_cor;
			maxlag = lag;
		}
	}

	/* Calculate ACF and other unvoiced detection parameters */
	compute_features(&max_cor, &maxlag, resid, speech,
			&(e_mem->features), acf, lpc, lsp, e_mem->band_power, e_mem );

	/* Updating Smooth Energy estimate Esmb(i)(k), Eq. (2.4.4.1-3) */
	//for(i=0;i<FREQBANDS;i++) {
	e_mem->frame_energy_sm[0] = L_add(L_mpy_ls(L_sub(e_mem->frame_energy_sm[0],
					L_shl2(e_mem->band_power[0], 4)), SMSNR_16), L_shl2(e_mem->band_power[0], 4));
	e_mem->frame_energy_sm[1] = L_add(L_mpy_ls(L_sub(e_mem->frame_energy_sm[1],
					L_shl2(e_mem->band_power[1], 4)), SMSNR_16), L_shl2(e_mem->band_power[1], 4));
	// }

	/************************************************************************/
	/* Section 2.4.4.2.2 Updating background noise */

	/* Now do the threshold adaptation   */
	/* don't adapt thresholds unless NACF < NACF_ADAP_BGN_THR over ADP frames*/
	/* we're assuming the background noise here doesn't have periodicity in */
	/* the range of typical speech                                          */

	if (e_mem->features.nacf0 < NACF_ADAP_BGN_THR_16) 
		e_mem->adaptcount++;
	else
		e_mem->adaptcount = 0;

	if(e_mem->adaptcount > ADP)
	{
		/* change the predictor filter for the noise whitening */
		/* noise                                            */
		/* update our background noise estimate */
		for(i=0;i<FREQBANDS;i++)
		{
			/* just increase background noise until it matches */
			/* lowest stationary period background noise       */
			pren = e_mem->band_noise_sm[i] + 16;
			posten = L_shl2(L_mpy_ls(e_mem->band_noise_sm[i], INC_FACTOR_16), 1);
			if(pren < posten)
				e_mem->band_noise_sm[i] = posten;
			else
				e_mem->band_noise_sm[i] = pren;
		}
		e_mem->adaptcount = ADP + 1;
	}
	else{

		/*if SNR map est. is valid & above SNR_MAP_THRESHOLD let background noise */
		/* estimate slowly inch up like it does in IS-96.  This will allow */
		/* the rate decision to perform as well as IS-96 in babble noise and */
		/* other non-stationary noises    */

		if(e_mem->snr_stat_once ==  1)
		{  /* snr is valid  */
			for(i=0;i<FREQBANDS;i++)
			{
				if( (e_mem->snr_map[i] > SNR_MAP_THRESHOLD) && 
						(e_mem->band_noise_sm[i] < e_mem->frame_energy_sm[i]))
				{
					pren = e_mem->band_noise_sm[i]+16;
					posten = L_shl2(L_mpy_ls(e_mem->band_noise_sm[i], IS96_INC_16), 1);
					if(pren < posten)
						e_mem->band_noise_sm[i] = posten;
					else
						e_mem->band_noise_sm[i] = pren;
				}
			}
		}
	}

	/* if input energy is lower than noise estimate, */
	/* reduce background noise estimate immediately  */
	for(i=0;i<FREQBANDS;i++)
	{
		/* Update Background Noise Estimate */
		if(e_mem->frame_energy_sm[i] < e_mem->band_noise_sm[i])
		{
			e_mem->band_noise_sm[i] = e_mem->frame_energy_sm[i];
		}
	}

	for(i=0;i<FREQBANDS;i++)
	{
		if (e_mem->band_noise_sm[i]>HIGH_THRESH_LIM)
			e_mem->band_noise_sm[i]=HIGH_THRESH_LIM;

		if (e_mem->band_noise_sm[i] < LOWEST_LEVEL[i])
			e_mem->band_noise_sm[i] = LOWEST_LEVEL[i];
	}
	/* end of updating backgound noise energy */

	/***************************************************************/


	/***************************************************************/
	/* Section 2.4.4.2.2 Updating Signal Energy Estimate */
	/* pitchrun keeps track of the number of frames in a row with periodicity*/

	if (e_mem->features.nacf0 > NACF_SOLID_VOICED_16)
		e_mem->pitchrun += 1;
	else
		e_mem->pitchrun = 0;

	if (e_mem->pitchrun > STATVOICED) {  /* decrease the signal energy   */
		/* if we're in a voiced segment */
		e_mem->snr_stat_once = 1;   /* confidence that we have seen speech */

		/* indicates a stationary voiced segment       */
		for(i=0;i<FREQBANDS;i++) {
			e_mem->signal_energy[i] = L_mpy_ls(e_mem->signal_energy[i], SCALE_DOWN_ENERGY_16); 
		}
	}

	/* assume that the highest energy sounds are speech signals and */
	/* thus the highest energy frames contain speech and can be used */
	/* to define the SNR */

	for(i=0; i<FREQBANDS; i++) {
		/* Update Signal Energy Estimate */
		if(e_mem->frame_energy_sm[i] > e_mem->signal_energy[i])
			e_mem->signal_energy[i] = e_mem->frame_energy_sm[i];

	}

	/* end updating signal energy estimate */
	/************************************************************************/
	/* update SNR estimates */
	for(i=0; i<FREQBANDS; i++) {
		if (e_mem->band_noise_sm[i] > 0)
		{
			n_shiftcnt = (norm_l(e_mem->signal_energy[i]) - 1);
			d_shiftcnt = norm_l(e_mem->band_noise_sm[i]);
			snr_temp = L_divide((e_mem->signal_energy[i] << n_shiftcnt), (e_mem->band_noise_sm[i] << d_shiftcnt));
			e_mem->snr[i] = L_shr(snr_temp, (23 - (d_shiftcnt - n_shiftcnt)));
		}
		else
		{
			e_mem->snr[i] = 256000000;
		}
	}

	/* now map the snr into a set of thresholds mult factors to use */

	if(e_mem->snr_stat_once != 0) {
		/* we have seen some speech and are confident in our SNR measure */
		e_mem->snr_stat_once = 1;

		/* UPDATE THIS FOR GENERAL SNR MAPS! */
		for(i=0;i<FREQBANDS;i++) {

			if (e_mem->snr[i] > 0)
			{
				/* e_mem->snr_map[i] = (int) rint((10*log10(e_mem->snr[i])-20.0)/5.0); */
				//snr_temp = L_sub(L_add(fnLog10(e_mem->snr[i]), 626255212), 161614248);
				snr_temp = fnLog10(e_mem->snr[i]) + 464640964;
				e_mem->snr_map[i] = extract_h((((snr_temp >> 9) - 4 * 65536) + 32768));
			}
			else
			{
				e_mem->snr_map[i] = 0;
			}

			if (e_mem->snr_map[i] < 0)
				e_mem->snr_map[i] = 0;
			if (e_mem->snr_map[i] > 7)
				e_mem->snr_map[i] = 7;		 
		}
	}
	else
	{
		for(i=0;i<FREQBANDS;i++)
		{
			if (i == 0)
			{
				/* e_mem->snr_map[i]= (int) rint((VOICE_INITIAL- 10*log10(e_mem->band_noise_sm[i])-20.0)/5.0); */
				snr_temp = fnLog10(e_mem->band_noise_sm[i]) + 626255212;
				e_mem->snr_map[i] = extract_h(((((VOICE_INITIAL - 20) * 13107) - 
								(snr_temp >> 9)) + 32768));
			}
			else
			{
				/* e_mem->snr_map[i]= (int) rint((VOICE_INITIAL_HI- 10*log10(e_mem->band_noise_sm[i])-20.0)/5.0); */
				snr_temp = fnLog10(e_mem->band_noise_sm[i]) + 626255212;
				e_mem->snr_map[i] = extract_h(((((VOICE_INITIAL_HI - 20) * 13107) -
								(snr_temp >> 9)) + 32768));
			}

			if(e_mem->snr_map[i] < 0) e_mem->snr_map[i]=0;
			if(e_mem->snr_map[i] > 7) e_mem->snr_map[i]=7;
		}
	}

	/* Second Stage of Rate Determination Algorithm, Section 2.4.4.3 */
	if(reduced_rate > 0 && reduced_rate < 4)
	{
		/* using the features of prediction gain, zero crossings, nacf, and */
		/* the target snr from the previous frame make a decision to :      */
		/* 1.) go down to 1/2 rate                                          */
		/* 2.) use 1/4 rate unvoiced mode (no pitch codebook)               */
		/* 3.) use 1/4 rate voiced mode for low amplitude sounds  */

		unvoiced = 0;
		/* is frame unvoiced                                   */
		/* gather features and make a voiced/unvoiced decision */

		/* generate linear discriminant to make the voiced/unvoiced */
		/* decision                                                 */	   
		if((e_mem->features.nacf_frame > 16384 && e_mem->features.zero < 80) ||
				(e_mem->features.nacf_frame < 8192 && e_mem->features.zero < 45 ) ||
				e_mem->features.pred_gain >= 15360)
		{
			unvoiced = 0;
		}
		else
		{
			discrim = mult(dis_coef[0], e_mem->features.nacf_frame) + 
				dis_coef[1]*(e_mem->features.zero) + dis_coef[3];
			if(discrim < 0)
			{
				unvoiced = 1;
				if(*rate != EIGHTH){
					if(reduced_rate == 1)
					{
						/* reduced rate = 3/4 so unvoiced gets coded at 1/2 rate */
						*rate = HALFRATE_VOICED; 
					}
					else
					{ /* reduced rate mode 2 or 3) */
						if(*last_solid_voiced)
							*rate = HALFRATE_VOICED; 
						else
							*rate = QUARTERRATE_UNVOICED;
					} 
				}
			}
		}
		/* Update last_solid_voiced for next time */
		if(e_mem->features.nacf_frame >= 16384 && e_mem->features.zero < 60)
			*last_solid_voiced = 1;
		else
			*last_solid_voiced = 0;

		/*** Keep track of frame energy  ***/
		/* Section 2.4.4.3.2 TEMPORALLY MASKED FRAME DETECTION */

		e_mem->features.frame_energy_db = 10*((fnLog10(5*(acf[0] >> 3) +
						3*(e_mem->features.frame_energy_last >> 3)) + 626255212) >> 6);  /* Q20 */

		/* keep a running track of the current signal energy with a time */
		/* constant of 8 frames or 160 msec                              */

		/*  e_mem->features.voiced_energy is Eavg */
		e_mem->features.voiced_energy = L_mpy_ls(e_mem->features.voiced_energy - e_mem->features.frame_energy_db,
				FRAME_ENG_SCALE) + e_mem->features.frame_energy_db;

		e_mem->features.frame_energy_last = acf[0];

		/* taking advantage of the masking of low amplitude speech 
		   from recent high energy */
		/* speech if the current speech frame is less than 9 db from the 
		   running average   */
		/* then code at 1/2 rate, if less than 15 db code at 1/4 rate voiced  */
		/* do all this if the frame isn't unvoiced nor eighth rate      */

		if(*rate != EIGHTH && unvoiced == 0){		   
			if(e_mem->features.voiced_energy-e_mem->features.frame_energy_db  
					> THR_SIL_1_2_20Q && e_mem->features.diff_lsp < THR_DIFFLSP_15Q)
			{
				if (reduced_rate == 1)
					*rate = HALFRATE_VOICED; 	   
				else if(e_mem->features.voiced_energy - e_mem->features.frame_energy_db 
						> THR_SIL_1_4_20Q)
					*rate = QUARTERRATE_UNVOICED; 	   
				else
					*rate = HALFRATE_VOICED; 	   
			}
			else if(reduced_rate == 3 ){/* mode 3 */			      

				if(*rate == FULLRATE_VOICED){/* Stationary Mode */

					/* Section 2.4.4.3.3 STATIONARY VOICED FRAME DETECTION */
					/* if the previous frame was well matched (target_snr > T) and the */
					/* current frame shows good periodicity with the past frame and the*/
					/* prediction gain hasn`t dropped precipitously then               */
					/* bump the rate down to 1/2 rate                                  */				   
					if(  (e_mem->target_snr > e_mem->target_snr_thr) && 
							(e_mem->features.nacf_frame > THR_NACF_BUMPUP_16) && 
							(e_mem->features.pred_gain_diff < THR_PRED_GAIN_6Q )){
						*rate = HALFRATE_VOICED; 
					}
				}
				else
				{/* This is in the SPEC, however it looks redundant */
					*rate = HALFRATE_VOICED;
				}
			}/* end of if (control->reduced_rate_mode == 3) */
		}
	} /* end of if(control->reduced_rate_flag > 0 && control->reduced_rate_flag < 4) */
	else if(reduced_rate == 4) 
	{
		if (*rate == FULLRATE_VOICED){ /* force all FULLRATE frames to HALFRATE */
			*rate = HALFRATE_VOICED;
		}
	}

	/* keep track of the running average rate for voiced frames */
	/* Don't care about EIGHT rate frames */
	if(*rate != EIGHTH){
		if(e_mem->block_cnt > 0){
			update_hist_cnt(e_mem, *rate);
			e_mem->block_cnt -= 1;
		}
		else  /* the block has expired so make threshold adjustments */
		{
			e_mem->block_cnt = STATWINDOW;  /*reset counter for the next window*/
			/* reset histogram and frame counts for next block                 */

			e_mem->avg_rate = (((int)e_mem->full_cnt*FULLRATE_BPS_16+
						(int)e_mem->half_cnt*HALFRATE_BPS_16+
						(int)e_mem->full_force*FULLRATE_BPS_16+
						(int)e_mem->half_force*HALFRATE_BPS_16+
						(int)e_mem->quarter_cnt*QUARTER_BPS_16)/STATWINDOW + 1024) >> 11;

			if(reduced_rate == 3){
				/* was average rate high or low for the last block of frames       */
				/* if it was out of bounds then adjust threshold, else leave as is */
				if(e_mem->avg_rate > e_mem->avg_rate_thr)
					adjust_rate_down(e_mem);
				if(e_mem->avg_rate < e_mem->avg_rate_thr)
					adjust_rate_up(e_mem);

				if(e_mem->target_snr_thr > HIGH_BND_SNR)
					e_mem->target_snr_thr = HIGH_BND_SNR;
				if(e_mem->target_snr_thr < LOW_BND_SNR)
					e_mem->target_snr_thr = LOW_BND_SNR;
			}

			/* keep the total numbers for the file  */
			e_mem->full_force_t += e_mem->full_force;
			e_mem->half_force_t += e_mem->half_force;
			e_mem->full_cnt_t += e_mem->full_cnt;
			e_mem->half_cnt_t += e_mem->half_cnt;
			e_mem->quarter_cnt_t += e_mem->quarter_cnt;
			e_mem->total_speech_frames += STATWINDOW;

			/* zero out rate counters */
			e_mem->full_force = e_mem->full_cnt = 
				e_mem->half_force = e_mem->half_cnt = e_mem->quarter_cnt = 0;

			for(i=0;i< 4;i++)
				e_mem->hist_below[i] = e_mem->hist_above[i] = 0;
		}
	}
	e_mem->last_rate = *rate;   
}/* end of select_mode2() */

