
#include "amr_plus_fx.h"

#include "basic_op.h"
#include "count.h"
#include "oper_32b.h"
#include "log2.h"
#include "math_op.h"
//#include "stdlib.h"

void Init_tcx_stereo_decoder(Decoder_State_Plus_fx *st)
{
	st->mem_stereo_ovlp_size_fx = 0;      
	Set_zero(st->mem_stereo_ovlp_fx,L_OVLP_2k);
	st->mem_balance_fx = 0;

}

void Dtcx_stereo(
				 Word16 synth[],       /* Out     in/out: synth[-M..lg]           */
				 Word16 mono[],        /* In */
				 Word16 wovlp[],       /* i/o:    wovlp[0..127]           */
				 Word16 ovlp_size,     /* input:  0, 64 or 128 (0=acelp)  */
				 Word16 L_frame,       /* input:  frame length            */
				 Word16 prm[],
				 Word16 pre_echo,
				 Word16 bad_frame[],
				 Decoder_State_Plus_fx
				 *st)
{
	Word16 i, i_subfr,  lg;

	Word16 any_bad_frame;	

	Word16 xri[L_TCX_LB];
	Word16 *xnq=xri;
	Word16 wm_arr[L_TCX_LB+ECU_WIEN_ORD];
	Word16 window[32+32];

	Word16 *wm = &wm_arr[ECU_WIEN_ORD];
	Word16 wbalance;
	Word16 gain_shap[8];
	Word16 rmm_h[ECU_WIEN_ORD+1];
	Word16 rmm_l[ECU_WIEN_ORD+1];
	Word16 rms_h[ECU_WIEN_ORD+1];
	Word16 rms_l[ECU_WIEN_ORD+1];
	Word16 *h;
	Word16 ifft3_scl;
	Word32 Lgain;
	Word16 lext=32;
	Word16 n_pack = 4;
	Word16 inc2 = 0;
	Word16 rms_shift,rmm_shift;

	ifft3_scl = 5;

	/*------ set length of overlap (lext) and length of encoded frame (lg) -----*/


	for (i=0;i< ECU_WIEN_ORD;i++) 
	{
		wm_arr[i] = 0;
	}
	h = st->h_fx;

	switch (L_frame) 
	{
	case 40:
		lext = 8; 
		n_pack=1; 
		inc2 = 16;
		break;
	case 80:
		lext = 16; 
		n_pack =2; 
		inc2 = 8;  

		break;
	case 160:
		lext = 32; 
		n_pack= 4; 
		inc2 = 4;  

		break;
	default:
		//AMRWBPLUS_PRINT("Stereo frame length unknow\n");
		//exit(0);
		break;

	};
	lg = add(L_frame,lext);


	any_bad_frame = 0;  
	for (i=0; i<n_pack; i++) 
	{
		any_bad_frame |= bad_frame[i];    logic16();
	}

	/* built window for overlaps section */
	Cos_window(window, ovlp_size, lext, inc2);

	/* these are already concealed by the avq demux */
	for(i=0; i<lg; i++) 
	{
		xri[i] = prm[i+2];   
	}

	Scale_tcx_ifft(xri, lg, &ifft3_scl); /* Output max value scaled*/

	/* only in mode 0,1 */
	/* windowed mono  */
	for (i=0; i<lg; i++)
	{
		wm[i] = mono[i];  
	}
	/* windowing for TCX overlap and correlation */
	Windowing(ovlp_size, wm, window, 0,0); /* wm Qsyn*/
	Windowing(lext, wm, window, L_frame, ovlp_size);	/* wm Qsyn*/

	
	if(pre_echo > 0)
	{
		Comp_gain_shap(wm,gain_shap,lg,st->Old_Q_syn); /*gain_shap Q10 */ 
	}

	/* windowing for TCX overlap and correlation */
	/*-----------------------------------------------------------*
	* Compute inverse FFT for obtaining xnq[] without noise.    *
	* Coefficients (xri[]) order are                            *
	*    re[0], re[n/2], re[1], re[2], ... re[n/2-1], im[n/2-1] *
	* Note that last FFT element (re[n/2]) is zeroed.           *
	*-----------------------------------------------------------*/
	Adap_low_freq_deemph(xri,shl(lg,2)); 
	xri[1]=0;     

	Ifft_reorder(xri, lg);
	Ifft3(xri, xnq, lg);

	wbalance = Balance(bad_frame[0],st, prm[0]); /* gain_pan Q14 out*/

	
	if (pre_echo>0)
	{
		Apply_gain_shap(lg,xnq,gain_shap); /* gain_shap Q10 in, xnq same Q out as in */
	}


	Lgain = D_gain_tcx(prm[1], xnq, lg, any_bad_frame, &(st->side_rms_fx)); 


	
	if ((sub(L_frame,40) != 0) || (bad_frame[0]==0) )
	{
		Apply_xnq_gain2(lg,wbalance,Lgain,xnq,wm,st->Old_Q_syn);
	}
	else
	{
		Apply_wien_filt(lg,xnq,h,wm);
	}
	
	if (any_bad_frame == 0 )
	{
		/* Create rmm and rms */
		Crosscorr_2(wm,wm,rmm_h,rmm_l,lg,0,ECU_WIEN_ORD+1,&rmm_shift,0);
		Crosscorr_2(xnq,wm,rms_h,rms_l,lg,0,ECU_WIEN_ORD+1,&rms_shift,1);
		/* calculate Wiener filter reconstructing xnq in mse sense*/
		Glev_s(h,rmm_h,rmm_l,rms_h,rms_l,ECU_WIEN_ORD+1,sub(rmm_shift,rms_shift));  
	}

	/*-----------------------------------------------------------*
	* find and quantize gain, multiply xnq[] by gain.           *
	* windowing of xnq[] for TCX overlap.                       *
	*-----------------------------------------------------------*/

	/* adaptive windowing on overlap (beginning and end of frame) */
	Windowing(ovlp_size,xnq,window,0,0);

	Windowing(lext,xnq,window,L_frame,ovlp_size);

	for (i=add(L_frame,lext); i<lg; i++) 
	{
		xnq[i] = 0;   
	}

	/*-----------------------------------------------------------*
	* TCX overlap and add.  Update memory for next overlap.     *
	*-----------------------------------------------------------*/

	Apply_tcx_overlap(xnq,wovlp,lext,L_frame);


	/*-----------------------------------------------------------*
	* find excitation and synthesis                             *
	*-----------------------------------------------------------*/

	for (i_subfr=0; i_subfr<L_frame; i_subfr++)
	{
		synth[i_subfr] = xnq[i_subfr]; 
	}

	return;
}

/*-----------------------------------------------------------------*
* Funtion dec_tcx_stereo                                           *
* ~~~~~~~~~~~~~~~~~~~~                                            *
*-----------------------------------------------------------------*/

void Dec_tcx_stereo(
					Word16 synth_2k[],
					Word16 left_2k[],
					Word16 right_2k[],
					Word16 param[],
					Word16 bad_frame[],
					Decoder_State_Plus_fx *st)
{
	Word16 synth_side[L_FRAME_2k];

	/* Scalars */
	Word16 start,end;
	Word16 k, mod[4];
	Word16 bfi;
	Word16 *prm;
	Word16 alpha;

	bfi = (bad_frame[0] || bad_frame[1] || bad_frame[2] || bad_frame[3]);

	/* get the mode man*/
	for(k=0;k<4;k++) 
	{
		mod[k] = param[k];
	}
	/* tcx decoder loop */
	k = 0; 
	while(k < NB_DIV)
	{
		prm = param+ 4 + (k*NPRM_DIV_TCX_STEREO);
		if ((mod[k] == 1) || (mod[k] == 0)) 
		{ logic16();   
		Dtcx_stereo(
			&synth_side[k*L_DIV_2k],
			&synth_2k[k*L_DIV_2k],
			st->mem_stereo_ovlp_fx,
			st->mem_stereo_ovlp_size_fx,
			L_FRAME_2k/4,
			prm,
			!mod[k],
			&bad_frame[k],
			st);        

		st->mem_stereo_ovlp_size_fx = L_OVLP_2k/4;
		k ++;
		}
		else if(mod[k] == 2) 
		{
			Dtcx_stereo(
				&synth_side[k*L_DIV_2k],
				&synth_2k[k*L_DIV_2k],
				st->mem_stereo_ovlp_fx,
				st->mem_stereo_ovlp_size_fx,
				L_FRAME_2k/2,
				prm,0,
				bad_frame+k,
				st);

			st->mem_stereo_ovlp_size_fx = L_OVLP_2k/2;
			k +=2;
		}
		else if(mod[k] == 3) 
		{
			Dtcx_stereo(
				&synth_side[k*L_DIV_2k],
				&synth_2k[k*L_DIV_2k],
				st->mem_stereo_ovlp_fx,
				st->mem_stereo_ovlp_size_fx,
				L_FRAME_2k,
				prm,
				0,
				bad_frame+k,
				st);

			st->mem_stereo_ovlp_size_fx = L_OVLP_2k;
			k += 4;
		} 

	}
	k=0; 

	while(k < NB_DIV) 
	{
		alpha = 32767;     
		if((mod[k]==0) || (mod[k] == 1)) 
		{ 
			start = extract_l(L_mult(k,L_FRAME_2k/4/2));
			end = extract_l(L_shr(L_mac(L_FRAME_2k/2, k, L_FRAME_2k/4),1));
			Ch_sep(synth_side,alpha,synth_2k,left_2k,right_2k,start,end);
			k ++;
		}
		else if(mod[k] == 2) 
		{ 
			start = extract_l(L_mult(k,L_FRAME_2k/4/2));
			end = extract_l(L_mac(L_FRAME_2k/2, k, L_FRAME_2k/4/2));
			Ch_sep(synth_side,alpha,synth_2k,left_2k,right_2k,start,end);
			k +=2;
		} 
		else 
		{
			start = 0;
			end = L_FRAME_2k; 
			Ch_sep(synth_side,alpha,synth_2k,left_2k,right_2k,start,end);
			k += 4;
		}
		sub(k,NB_DIV); /* end of while */
	}
	return;
}


