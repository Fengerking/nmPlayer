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
/* init.c - initialization of all coder stuff. defaults here.  */

#include <stdlib.h>
//#include <malloc.h>
#include "celp.h"
#include "coderate.h"
#include "quantize.h"
#include "basic_op.h"
#include "cb.h"
#include "mem_align.h"

void reset_encoder(struct ENCODER_MEM *e_mem)
{
	int  i, d_shiftcnt, n_shiftcnt;
    struct ZERO_FILTER       temp_unq_form_res_filt = e_mem->unq_form_res_filt;
	struct ZERO_FILTER       temp_form_res_filt = e_mem->form_res_filt;
	struct POLE_FILTER       temp_spch_wght_syn_filt = e_mem->spch_wght_syn_filt;
	struct POLE_FILTER       temp_target_syn_filter = e_mem->target_syn_filter;
	struct POLE_FILTER       temp_hipass_p = e_mem->hipass_p;
	struct ZERO_FILTER       temp_hipass_z = e_mem->hipass_z;
	struct ZERO_FILTER       temp_decimate = e_mem->decimate;

	//decimate
	for(i=0; i<DEC_ORDER; i++)
	{
		temp_decimate.zero_coeff[i] = decimate_filter[i];
		temp_decimate.memory[i] = 0; 
	}
	//hipass_p
    temp_hipass_p.memory[0] = 0;
	temp_hipass_p.memory[1] = 0;
	//hipass_z
	temp_hipass_z.memory[0] = 0;
	temp_hipass_z.memory[1] = 0;

	for(i=0; i<FIR_UNV_LEN; i++)
		e_mem->dec.bpf_unv.memory[i] = 0;
	//form_res_filt, spch_wght_syn_filt
	for(i=0; i<LPCORDER; i++)
	{
		temp_form_res_filt.memory[i] = 0;
		temp_spch_wght_syn_filt.memory[i] = 0;
		e_mem->dec.wght_syn_filt.memory[i] = 0;
		e_mem->dec.lpc_filt.memory[i] = 0;
		e_mem->dec.jt_gain_opt.memory[i] = 0;
		temp_target_syn_filter.memory[i] = 0;
		temp_unq_form_res_filt.memory[i] = 0;
	}
    for(i=0; i<PSMORDER; i++)
		e_mem->dec.pitch_sm.memory[i] = 0;

    for(i=0; i<MAXLAG+FR_INTERP_FILTER_LENGTH/2+3; i++)
		e_mem->dec.pitch_filt.memory[i] = 0;

    e_mem->dec.low_rate_cnt = 0; 
    e_mem->dec.last_G = 0; 
 
    for (i=0; i<LPCORDER; i++) {
		e_mem->dec.last_qlsp[i] = initlsp[i];
    }
    for (i=0; i<LPCORDER; i++) {
		e_mem->dec.pred_qlsp[i] = 0;
    }
	
    for (i=0; i<LPCORDER; i++) {
		e_mem->last_lsp[i] = initlsp[i];
     }
	
    e_mem->last_rate = EIGHTH; 
    e_mem->last_rate_1st_stage = EIGHTH; 

    /* init for average rate calc and thresholds */
    e_mem->avg_rate_thr = 9; //control->avg_rate; 
    e_mem->avg_rate = e_mem->avg_rate_thr; 

    /* start the target snr threshold at 10 db */
    e_mem->target_snr_thr = 10240; //control->target_snr_thr;

    e_mem->target_snr = 1024;

    e_mem->target_snr_var = 1;
    e_mem->target_snr_mean = 10; //control->target_snr_thr;  
    e_mem->features.pred_gain_mean = 10240; //control->target_snr_thr;  

    for (i=0; i<LPCORDER; i++) {  
		e_mem->features.last_lsp[i]= initlsp[i];
    } 
    e_mem->features.last_nacf = 0;
    e_mem->features.last_diff_lsp = 0;
    e_mem->features.last_pred_gain = 0;
    e_mem->features.pred_gain_mean = 0;
    e_mem->features.pred_gain_var = 0;

    e_mem->block_cnt = STATWINDOW;
    e_mem->full_force = e_mem->full_cnt =  e_mem->full_cnt_t = e_mem->full_force_t = 
    e_mem->half_force = e_mem->half_cnt = e_mem->half_cnt_t = e_mem->quarter_cnt = 0;
    e_mem->quarter_cnt_t = 0;
    e_mem->total_speech_frames = 0;
    e_mem->half_force_t = 0;
    for(i=0; i < 4; i++)
      e_mem->hist_above[i] = e_mem->hist_below[i] = 0;

    e_mem->num_full_frames=0;
    e_mem->hangover=10;
    e_mem->hangover_in_progress=0;

    for(i = 0; i < PMAX; i++)  
      e_mem->resid_mem[i] = 0;  

    e_mem->adaptcount=0;
    e_mem->pitchrun=0;

    e_mem->frame_energy_sm[0] = 3200000 * 16;
    e_mem->frame_energy_sm[1] = 320000 * 16;
	
    for (i=0; i<FREQBANDS; i++) {
		e_mem->band_power_last[i] = 0;
    }                               
	
	e_mem->signal_energy[0] = 3200000 * 16;
	e_mem->signal_energy[1] = 320000 * 16;	
	
    e_mem->snr_stat_once=0;
    e_mem->dec.G_pred[0] = e_mem->dec.G_pred[1] = 0;


    for (i=0; i<FREQBANDS; i++)
    {
        e_mem->band_noise_sm[i]= HIGH_THRESH_LIM;
    }
    for (i=0; i<FREQBANDS; i++)
    {
		n_shiftcnt = (norm_l(e_mem->signal_energy[i]) - 1);
		d_shiftcnt = norm_l(e_mem->band_noise_sm[i]);
		e_mem->snr[i] = L_shr(L_divide((e_mem->signal_energy[i] << n_shiftcnt), 
			(e_mem->band_noise_sm[i] << d_shiftcnt)), (23 - (d_shiftcnt - n_shiftcnt)));
		e_mem->snr_map[i] = 0;    
    }   

}

short initialize_encoder(struct  ENCODER_MEM *e_mem, VO_MEM_OPERATOR *pMemOP)
{
    int i, j;
	int d_shiftcnt, n_shiftcnt;

    /* initialize the encoder filters */
    e_mem->dec.type=ENCODER;
    initialize_zero_filter(&(e_mem->hipass_z), 2, pMemOP);
    e_mem->hipass_z.zero_coeff[0]= -8192; /* 2 * (1 << 12) */
    e_mem->hipass_z.zero_coeff[1]=  4096; /* 1 * (1 << 12) */
    initialize_pole_filter(&(e_mem->hipass_p), 2, pMemOP);
    e_mem->hipass_p.pole_coeff[0]=  30802; // //30802; // 7700; /* 1.88 * (1 << 12) */
    e_mem->hipass_p.pole_coeff[1]= -14477; // //-14477 // -3619; /* -0.94*0.94 * (1 << 12)*/

    initialize_zero_filter(&(e_mem->decimate), DEC_ORDER, pMemOP);

    for (i=0; i<DEC_ORDER; i++) {
		e_mem->decimate.zero_coeff[i] = decimate_filter[i];
    }

    /* initialize bpf for unvoiced speech for enc and dec */
    initialize_zero_filter(&(e_mem->dec.bpf_unv), FIR_UNV_LEN, pMemOP);
    for (i=0; i<FIR_UNV_LEN; i++) {
		e_mem->dec.bpf_unv.zero_coeff[i] = unv_filter[i];
    }

    initialize_zero_filter(&(e_mem->form_res_filt), LPCORDER, pMemOP);
    initialize_pole_filter(&(e_mem->spch_wght_syn_filt), LPCORDER, pMemOP);
    initialize_pole_filter(&(e_mem->dec.wght_syn_filt), LPCORDER, pMemOP);
    initialize_pole_1_tap_filter(&(e_mem->dec.pitch_filt),MAXLAG+FR_INTERP_FILTER_LENGTH/2+3, pMemOP);
    initialize_pole_filter(&(e_mem->dec.lpc_filt), LPCORDER, pMemOP);
    initialize_pole_filter(&(e_mem->dec.jt_gain_opt), LPCORDER, pMemOP);
    initialize_pole_filter(&(e_mem->target_syn_filter), LPCORDER, pMemOP);
    initialize_zero_filter(&(e_mem->dec.pitch_sm), PSMORDER, pMemOP);

    e_mem->dec.low_rate_cnt = 0; 
    e_mem->dec.last_G = 0; 
 
    for (i=0; i<LPCORDER; i++) {
		e_mem->dec.last_qlsp[i] = initlsp[i];
    }
    for (i=0; i<LPCORDER; i++) {
		e_mem->dec.pred_qlsp[i] = 0;
    }
	
    for (i=0; i<LPCORDER; i++) {
		e_mem->last_lsp[i] = initlsp[i];
     }
	
    e_mem->last_rate = EIGHTH; 
    e_mem->last_rate_1st_stage = EIGHTH; 

    /* init for average rate calc and thresholds */
    e_mem->avg_rate_thr = 9; //control->avg_rate; 
    e_mem->avg_rate = e_mem->avg_rate_thr; 

    /* start the target snr threshold at 10 db */
    e_mem->target_snr_thr = 10240; //control->target_snr_thr;

    e_mem->target_snr = 1024;

    e_mem->target_snr_var = 1;
    e_mem->target_snr_mean = 10; //control->target_snr_thr;  
    e_mem->features.pred_gain_mean = 10240; //control->target_snr_thr;  

    for (i=0; i<LPCORDER; i++) {  
		e_mem->features.last_lsp[i]= initlsp[i];
    } 
    e_mem->features.last_nacf = 0;
    e_mem->features.last_diff_lsp = 0;
    e_mem->features.last_pred_gain = 0;
    e_mem->features.pred_gain_mean = 0;
    e_mem->features.pred_gain_var = 0;

    e_mem->block_cnt = STATWINDOW;
    e_mem->full_force = e_mem->full_cnt =  e_mem->full_cnt_t = e_mem->full_force_t = 
    e_mem->half_force = e_mem->half_cnt = e_mem->half_cnt_t = e_mem->quarter_cnt = 0;
    e_mem->quarter_cnt_t = 0;
    e_mem->total_speech_frames = 0;
    e_mem->half_force_t = 0;
    for(i=0; i < 4; i++)
      e_mem->hist_above[i] = e_mem->hist_below[i] = 0;

    /* rate decision initialization */
    initialize_zero_filter(&(e_mem->unq_form_res_filt), LPCORDER, pMemOP);
    e_mem->num_full_frames=0;
    e_mem->hangover=10;
    e_mem->hangover_in_progress=0;

    for(i = 0; i < PMAX; i++)  
      e_mem->resid_mem[i] = 0;  

    e_mem->adaptcount=0;
    e_mem->pitchrun=0;

    e_mem->frame_energy_sm[0] = 3200000 * 16;
    e_mem->frame_energy_sm[1] = 320000 * 16;
	
    for (i=0; i<FREQBANDS; i++) {
		e_mem->band_power_last[i] = 0;
    }                               
	
	e_mem->signal_energy[0] = 3200000 * 16;
	e_mem->signal_energy[1] = 320000 * 16;	
	
    e_mem->snr_stat_once=0;
    e_mem->dec.G_pred[0] = e_mem->dec.G_pred[1] = 0;


    for (j=0; j<FREQBANDS; j++)
    {
        e_mem->band_noise_sm[j]= HIGH_THRESH_LIM;
    }
    for (j=0; j<FREQBANDS; j++)
    {
		n_shiftcnt = (norm_l(e_mem->signal_energy[j]) - 1);
		d_shiftcnt = norm_l(e_mem->band_noise_sm[j]);
		e_mem->snr[j] = L_shr(L_divide((e_mem->signal_energy[j] << n_shiftcnt), 
			(e_mem->band_noise_sm[j] << d_shiftcnt)), (23 - (d_shiftcnt - n_shiftcnt)));
		e_mem->snr_map[j] = 0;    
    }   

	return 0;
}/* end of initialize_encoder_and_decoder() */

void free_encoder(struct  ENCODER_MEM *e_mem, VO_MEM_OPERATOR *pMemOP)
{
    free_zero_filter(&(e_mem->hipass_z), pMemOP);
    free_zero_filter(&(e_mem->dec.bpf_unv), pMemOP);
    free_pole_filter(&(e_mem->hipass_p), pMemOP);
    free_zero_filter(&(e_mem->decimate), pMemOP);
    free_zero_filter(&(e_mem->form_res_filt), pMemOP);
    free_pole_filter(&(e_mem->spch_wght_syn_filt), pMemOP);
    free_pole_filter(&(e_mem->dec.wght_syn_filt), pMemOP);
    mem_free(pMemOP, (char *) e_mem->dec.pitch_filt.memory);
    free_pole_filter(&(e_mem->dec.lpc_filt), pMemOP);
    free_pole_filter(&(e_mem->dec.jt_gain_opt), pMemOP);
    free_pole_filter(&(e_mem->target_syn_filter), pMemOP);
    free_zero_filter(&(e_mem->dec.pitch_sm), pMemOP);

    /* free rate decision */
    free_zero_filter(&(e_mem->unq_form_res_filt), pMemOP);
    
}

