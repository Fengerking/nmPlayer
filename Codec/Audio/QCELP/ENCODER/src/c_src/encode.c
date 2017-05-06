/* encode.c - main CELP encoder */

#include <stdio.h>
#include "basic_op.h"
#include "celp.h"
#include "coderate.h"

//FILE *fix_point;
void encoder(QCP13_ENCOBJ* qcpenc)
{
	int form_ene; 
	int    Rs[FILTERORDER+1];
	short  i,j,index;
	short  lpc[LPCORDER], interp_lpc[2][MAXSF][2][LPCORDER];
	short  lsp[LPCORDER];
	short  qlsp[LPCORDER];
	short  *in_buffer;
	short  *out_buffer;
	//short  *indata = NULL, *outdata = NULL;
	short  *lpc_ir; 	 
	struct PACKET          *packet;
	struct ENCODER_MEM     *e_mem;
	struct LPCPARAMS   lpc_params;
	struct PITCHPARAMS pitch_params;
	struct CBPARAMS    cb_params[MAXCBPERPIT][MAXNUMCB];
	short  sf_pointer;
	short  mode;
	short  form_resid[FSIZE], target[FSIZE], pw_speech[FSIZE];
	short  before_search[FSIZE];
	short  target_after_save[FSIZE];
	short  wght_factor, form_ene_s;
	short  last_G, Gtmp[5];
	short  fsizepitch, fsizecb, fcbpitch;

	in_buffer = qcpenc->in_speech;
	out_buffer = qcpenc->out_speech;
	packet = qcpenc->packet;
	e_mem = qcpenc->encoder_memory;
	lpc_ir = qcpenc->lpc_ir;

	clear_packet_params(packet);
	pitch_params.frac = 0;


	front_end_filter(&in_buffer[LPCOFFSET],e_mem);

	compute_lpc(&in_buffer[LPCOFFSET], LPCSIZE, HAMMING, LPCORDER,
		lpc, Rs);

	lpc2lsp(lpc, lsp, e_mem->last_lsp, LPCORDER);

	set_lag_range(&pitch_params, &(packet->min_lag));  

	if(qcpenc->rda_flag)
	{
	    select_mode1(&mode, Rs, e_mem);
	    packet->mode=mode;
	}
	else
	{
		mode = 4;
		packet->mode = 4;
	}

	quantize_lpc(mode, lpc, lsp, qlsp, Rs, &lpc_params, e_mem);

	pack_lpc(mode, &lpc_params, packet); 

	interp_lpcs(mode, e_mem->dec.last_qlsp, qlsp, 
		interp_lpc, BOTH);

	create_target_speech(mode, in_buffer, form_resid, pw_speech, 
		interp_lpc, &(e_mem->form_res_filt), 
		&(e_mem->spch_wght_syn_filt));

	if(qcpenc->rda_flag)
	{
	    select_mode2(&mode, form_resid, Rs, e_mem, in_buffer,lpc, lsp,
		             &qcpenc->last_solid_voiced, qcpenc->reduced_rate_flag);
	    packet->mode = mode; 
	}
	else
	{
		mode = 4;
		packet->mode = 4;
	}
 
	fsizepitch = FSIZE_DIV_PITCHSF[mode];
	fsizecb = FSIZE_DIV_CBSF[mode];
	fcbpitch = CBSF_DIV_PITCHSF[mode];

	if(mode == BLANK)
	{
		e_mem->dec.last_G=cb_params[0][0].G=0;
		cb_params[0][0].i=0;
		pitch_params.b=e_mem->dec.last_b;
		pitch_params.frac=e_mem->dec.last_frac;
		if (pitch_params.b > 1) {
			pitch_params.b = 1;
		}
		pitch_params.lag=e_mem->dec.last_lag;
		for (i=0; i<LPCORDER; i++) {
			qlsp[i]=e_mem->dec.last_qlsp[i];
		}
		lsp2lpc(qlsp, interp_lpc[PITCH][0][NOT_WGHTED], LPCORDER);
		wght_factor=MAX_16;
		for (i=0; i<LPCORDER; i++) {
			wght_factor= mult(wght_factor, PERCEPT_WGHT_FACTOR_16);
			interp_lpc[PITCH][0][WGHTED][i]= mult(wght_factor, interp_lpc[PITCH][0][NOT_WGHTED][i]);
		}
		run_decoder(mode, &(e_mem->dec), interp_lpc[PITCH][0], &pitch_params, 
			cb_params, out_buffer, FSIZE, 1, 1);
		pack_frame(mode, packet);

		/* don't use formant residual here, but need to update the */
		/* memories for the following frames */	  
		update_form_resid_mems(in_buffer, &(e_mem->form_res_filt));

		e_mem->target_snr = 0;
	}
	else if(mode != EIGHTH && mode != QUARTERRATE_UNVOICED)
	{ 
		cb_params[0][0].sd = packet->sd_enc;
		sf_pointer=0;		

		if (PITCHSF[mode]>0) 
		{
			for(i=0; i<PITCHSF[mode]; i++) 
			{
				save_target(&pw_speech[sf_pointer],before_search,fsizepitch);
				
				update_target_pitch(mode, &pw_speech[sf_pointer], target, 
					interp_lpc[PITCH][i][WGHTED], &(e_mem->dec));
				
				compute_pitch(mode, target, e_mem,
					interp_lpc[PITCH][i][WGHTED], &pitch_params);			  
				
				target_reduction(target,before_search,
					&e_mem->pitch_target_energy[i],
					&e_mem->pitch_target_energy_after[i], fsizepitch);
				
				save_pitch(&pitch_params,&e_mem->pitch_lag[i],&e_mem->pitch_gain[i]);
				
				pack_pitch(&pitch_params, packet, i);
				
				save_target(target,before_search, fsizepitch);
				
				for(j=0; j<fcbpitch; j++) 
				{				  
					update_target_cb(mode, &pw_speech[sf_pointer],
						&target[j*fsizecb], interp_lpc[PITCH][i][WGHTED],
						&pitch_params, &(e_mem->dec));
					compute_cb(mode, &target[j*fsizecb], e_mem, 
						interp_lpc[PITCH][i][WGHTED], cb_params, j);
					
					pack_cb(mode, cb_params, packet, i, j); 					
					run_decoder(mode, &(e_mem->dec), interp_lpc[PITCH][i], 
						&pitch_params, cb_params+j, 
						&out_buffer[sf_pointer], fsizecb, 1, 
						NUMCB[mode]);
					
					for(index=0; index<fsizecb; index++)
						target[j*fsizecb+index] = 
						pw_speech[sf_pointer+index]-e_mem->
						dec.pw_speech_out[index];
					
					target_reduction(&target[j*fsizecb],
						&before_search[j*fsizecb],
						&e_mem->codebook_target_energy[j+i*fcbpitch],
						&e_mem->codebook_target_energy_after[j+i*fcbpitch],
						fsizecb);
					
					save_target(&target[j*fsizecb],&target_after_save
						[j*fsizecb+i*fsizepitch],fsizecb); 
					
					sf_pointer += fsizecb;
				}/* end of for(j = 0; j < CBSF[mode]/PITCHSF[mode]; j++) */
			}/* end of for(i = 0; i < PITCHSF[mode]; i++) */
			
			compute_target_snr(mode, e_mem);
		}
		pack_frame(mode, packet);

		e_mem->dec.last_G = abs_s(cb_params[fcbpitch-1][0].G);
		e_mem->dec.low_rate_cnt = 0;	  
	}
	else if(mode == QUARTERRATE_UNVOICED)
	{		
		pitch_params.b = 0;
		pitch_params.lag = 0;
		pitch_params.frac = 0;
		pitch_params.qcode_b = 0;
		pitch_params.qcode_lag = 0;
		pitch_params.qcode_frac = 0;

		sf_pointer = 0;
		/* need to compute 5 gain parameters which get interpolated */
		for(j = 0; j < CBSF[mode]; j++)
		{
			compute_cb_gain(mode, e_mem, cb_params, &in_buffer[sf_pointer], j);			
			pack_cb(mode, cb_params, packet, 0, j);

			Gtmp[j] = cb_params[j][0].G;
			sf_pointer += fsizecb;
		}

		pack_frame(mode, packet);

		i = ((short)packet->data[1] & ((short)packet->data[2] << 8));
		j = ((short)packet->data[3] & ((short)packet->data[4] << 8));

		cb_params[0][0].sd = ((j & 0xcff ) << 4) | ((i & 0x1f8) >> 3);
		/* produce 8 interpolated CB Gains from 5 CB Gains */

		for(i = 0; i < PITCHSF[mode]; i++)
		{
			switch(i)
			{
			case 0:
				cb_params[0][0].G = Gtmp[0];
				cb_params[1][0].G =  mult((Gtmp[0] - Gtmp[1]), INTERPRE06) + Gtmp[1];
				break;
			case 1:
				cb_params[0][0].G = Gtmp[1];
				cb_params[1][0].G = Gtmp[1] + mult((Gtmp[2] - Gtmp[1]), INTERPRE08);
				break;
			case 2:
				cb_params[0][0].G = mult((Gtmp[2] - Gtmp[3]), INTERPRE08) + Gtmp[3];
				cb_params[1][0].G = Gtmp[3];
				break;
			case 3:
				cb_params[0][0].G = Gtmp[3] + mult((Gtmp[4] - Gtmp[3]), INTERPRE06);
				cb_params[1][0].G = Gtmp[4];
				break;
			default:
				break;
			}

			run_decoder(mode, &(e_mem->dec), interp_lpc[PITCH][i], &pitch_params,
				cb_params, &out_buffer[i*fsizepitch], fsizepitch, 2, 1);
		}
		e_mem->dec.last_G = abs_s(Gtmp[4]);
		e_mem->dec.low_rate_cnt=0;
		e_mem->target_snr = 0;	  
	}
	else /* Lowest Rate */
	{ 
		pitch_params.b=0;
		pitch_params.lag=0;
		pitch_params.frac = 0;
		pitch_params.qcode_b = 0;
		pitch_params.qcode_lag = 0;
		pitch_params.qcode_frac = 0;

		pack_pitch(&pitch_params, packet, 0);

		form_ene=0;
		for (i=0; i<FSIZE; ) {
			form_ene = L_add(form_ene, (int)form_resid[i] * form_resid[i]); i++;
			form_ene = L_add(form_ene, (int)form_resid[i] * form_resid[i]); i++;
			form_ene = L_add(form_ene, (int)form_resid[i] * form_resid[i]); i++;
			form_ene = L_add(form_ene, (int)form_resid[i] * form_resid[i]); i++;
		}	

		form_ene = L_mpy_ls(form_ene, INVFSIZE);		
		i = norm_l(form_ene);
		if(i & 1) i--;
		form_ene <<= i;
		form_ene_s = sqroot(form_ene);
		form_ene_s = mult(form_ene_s, 23170);
		i >>= 1;
		form_ene_s = shr(form_ene_s, (short)(i - 5));

		/* correction factor to agree with DSP at decoder */
		form_ene_s = mult(form_ene_s, G_FACTOR_16);

		if(e_mem->snr_map[0] > 3){
			form_ene_s >>= 1;
			qcpenc->hysteresis = 1;
		}
		else if(e_mem->snr_map[0] < 2 )
			qcpenc->hysteresis = 0;
		else if(qcpenc->hysteresis == 1)
			form_ene_s >>= 1;

		quantize_G_8th(form_ene_s, &(cb_params[0][0].G),
			&(cb_params[0][0].qcode_G), e_mem->dec.G_pred);		
		pack_cb(mode, cb_params, packet, 0, 0);		
		packet->sd_enc=((521*(packet->sd_enc)+259)&0xffff);		
		pack_frame(mode, packet);	  

		/* check for null Traffic Channel data */
		while (packet->data[1]==0xff && packet->data[2]==0xff) {
			packet->sd_enc &= 0xfff7;
			pack_frame(mode, packet); 
		}
		cb_params[0][0].sd = packet->sd_enc;

		last_G=(cb_params[0][0].G+e_mem->dec.last_G) >> 1;

		for (i=0; i<PITCHSF8TH; i++) {
			cb_params[0][0].G= e_mem->dec.last_G + (((last_G - e_mem->dec.last_G) *(int)(i+1)) >> 3);

			run_decoder(mode, &(e_mem->dec), interp_lpc[PITCH][0], &pitch_params, 
				cb_params, &out_buffer[i*FSIZE8TH], FSIZE8TH, 1, 1);
		}
		e_mem->dec.last_G=cb_params[0][0].G;
		e_mem->dec.low_rate_cnt += 1;
		e_mem->target_snr = 0;
	}
	e_mem->dec.last_b=pitch_params.b;
	e_mem->dec.last_lag=pitch_params.lag;
	e_mem->dec.last_frac=pitch_params.frac;

	/* Need to put in case with pitch subframes and no cb subframes */  
	for (i=0; i<LPCORDER; i++) {
		e_mem->last_lsp[i]=lsp[i];
		e_mem->dec.last_qlsp[i]=qlsp[i];
	}

	//for (i=0; i<LPCORDER; i++) {
	//	e_mem->dec.last_qlsp[i]=qlsp[i];
	//}
}/* end of encoder() */
