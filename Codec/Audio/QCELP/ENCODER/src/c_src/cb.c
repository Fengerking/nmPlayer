/* cb.c - perform closed-loop codebook search                         */

#include "basic_op.h"
#include "celp.h"
#include "cb.h"
int compute_cb(
			   short                  mode,
			   short                  *target,
               struct ENCODER_MEM     *e_mem,
			   short                  *lpc,
               struct CBPARAMS        cb_params[MAXCBPERPIT][MAXNUMCB],
	           short                  cbsf)
{
	int     Exy, Eyy, err, min_err;
	int		G, G2, MinG;
	int     Eyyopt[MAXNUMCB];
	short	*indata1, *indata2, *outdata;
	short   lpc_ir[LENGTH_OF_IMPULSE_RESPONSE];
	short   cb_out[3*FSIZE]; 
	short   *cb_out_shifted;
	short   y[FSIZE];
//	short   subtarget[FSIZE];
	short   n_shiftcnt, d_shiftcnt; 
	short   s_Exy, S_Eyy;
	short   i,j, fsiszecb;
	short   cbnum, s_G, tmp_G;	
	
	fsiszecb = FSIZE_DIV_CBSF[mode];

//	indata1 = target;
//	outdata = subtarget;
//	for (i=fsiszecb; i; i--) {
//		*outdata++ = *indata1++;
//	}
	
	indata1 = lpc;
	outdata = e_mem->dec.wght_syn_filt.pole_coeff;
	for (i = LPCORDER; i; i--) {
		*outdata++ = *indata1++;
	}	

	/* get LPC impulse response */
	get_impulse_response_pole(lpc_ir, LENGTH_OF_IMPULSE_RESPONSE,
		&(e_mem->dec.wght_syn_filt));
	
	for (cbnum=0; cbnum<NUMCB[mode]; cbnum++) {
		MinG = 0;
		if (mode == FULLRATE_VOICED ) { /* Normal search */
			min_err=1000000000;
			
			/* construct c(n) */
			cb_out_shifted= &cb_out[CBLENGTH];
			
//			for (i= -CBLENGTH+1; i<fsiszecb; i++) {
//				cb_out_shifted[i] = CODEBOOK[(i-1+CBLENGTH)&(CBLENGTH - 1)];
//			}                             /* complete c(n) in memory */
			
			indata1 = CODEBOOK;
			outdata = cb_out_shifted - CBLENGTH + 1;
			for (i= CBLENGTH; i; i--) {
				*outdata++ = *indata1++;
			}
			indata1 = CODEBOOK;
			for(i = fsiszecb; i; i--)
			{
				*outdata++ = *indata1++;
			}
			
			initial_recursive_conv(&cb_out_shifted[1], fsiszecb - 1, lpc_ir);	  
			
			for (i=1; i<CBLENGTH; i+=CB_STEP) {
				recursive_conv_10(&cb_out_shifted[-i], lpc_ir, fsiszecb);
				Exy=0;
				Eyy=0;
							
				indata1 = target;
				indata2 = cb_out_shifted - i;
				for (j=fsiszecb; j; j--) {
					Exy += (int)(int)(*indata1)*(int)(*indata2);
					Eyy += ((int)(*indata2)*(int)(*indata2) + 1024) >> 11;
					indata1++;
					indata2++;
				}

				if ((ABS(Exy) < 21)||(Eyy < 21)) {
					Exy=205;
					Eyy=2048;
					G = 104858;   /* Q20 */
					err = 0;
				}
				else
				{
//					if(Exy >= 0)
//					{
//						n_shiftcnt = (norm_l(Exy) - 1);
//						d_shiftcnt = norm_l(Eyy);
//						G = L_shr(L_divide((Exy << n_shiftcnt), 
//							(Eyy << d_shiftcnt)), (11 - (d_shiftcnt - n_shiftcnt)));
//						
//					}else{
//						n_shiftcnt = (norm_l(-Exy) - 1);
//						d_shiftcnt = norm_l(Eyy);
//						G = -L_shr(L_divide(((-Exy) << n_shiftcnt), 
//							(Eyy << d_shiftcnt)), (11 - (d_shiftcnt - n_shiftcnt)));
//					}
//
//					s_G = (short)((G + 0x8000) >> 16);
//					err = L_mpy_ls(L_shl(L_mpy_ls((Eyy << d_shiftcnt), s_G), 11 - d_shiftcnt)
//									- (Exy << 1), s_G);
					
					if(Exy >= 0)
					{
						n_shiftcnt = (norm_l(Exy) - 1);
						d_shiftcnt = norm_l(Eyy);
						s_Exy = extract_h(Exy << n_shiftcnt);
						S_Eyy = extract_h(Eyy << d_shiftcnt);
						
						s_G = divide_s(s_Exy, S_Eyy);
						G2 = 5 +  (d_shiftcnt - n_shiftcnt);
						if(G2 >= 0)
							G = s_G << G2;
						else
							G = s_G << (-G2);
						//G = L_shl(s_G, 5 +  (d_shiftcnt - n_shiftcnt));
					}else{
						n_shiftcnt = (norm_l(-Exy) - 1);
						d_shiftcnt = norm_l(Eyy);
						s_Exy = extract_h((-Exy) << n_shiftcnt);
						S_Eyy = extract_h(Eyy << d_shiftcnt);
						s_G = divide_s(s_Exy, S_Eyy);
						
						G2 = 5 +  (d_shiftcnt - n_shiftcnt);
						if(G2 >= 0)
							G = -(s_G << G2);
						else
							G = -(s_G << (-G2));
						//G = -L_shl(s_G, 5 +  (d_shiftcnt - n_shiftcnt));
					}

					s_G = (short)((G + 0x8000) >> 16);
					err = (s_Exy * s_G) >> n_shiftcnt;

//					n_shiftcnt = norm_l(G);
//					G2 =  L_mpy_ll((G << n_shiftcnt), (G << n_shiftcnt));
//					err = L_shl(L_mpy_ll((Eyy << d_shiftcnt), G2), 15 - d_shiftcnt - 2*n_shiftcnt)
//						- L_shl(L_mpy_ll(Exy, (G << n_shiftcnt)), 5 - n_shiftcnt);				
				}				
				
				if (err<min_err) {
					min_err = err;
					MinG = G;
					cb_params[cbsf][cbnum].i = (i+1)&(CBLENGTH - 1);
					Eyyopt[cbnum] = Eyy;
				}
			}
			
			quantize_G(mode, MinG, 
				&(cb_params[cbsf][cbnum].G), 
				&(cb_params[cbsf][cbnum].qcode_G), 
				&(cb_params[cbsf][cbnum].qcode_Gsign),
				e_mem->dec.lastG, cbsf, e_mem->dec.G_pred);
			
			quantize_i(&(cb_params[cbsf][cbnum].i), 
				&(cb_params[cbsf][cbnum].qcode_i));

			tmp_G = cb_params[cbsf][cbnum].G;
			n_shiftcnt = CBLENGTH - cb_params[cbsf][cbnum].i;
			d_shiftcnt = CBLENGTH - n_shiftcnt;
			if(d_shiftcnt >= fsiszecb)
			{
//				for (j=0; j<fsiszecb; j++) {
//					y[j]= mult(CODEBOOK[(n_shiftcnt+j)&(CBLENGTH - 1)], tmp_G);
//				}
				indata1 = CODEBOOK + n_shiftcnt;
				outdata = y;
				for (j=fsiszecb; j; j--) {
					*outdata = mult(*indata1, tmp_G);
					outdata++;
					indata1++;
				}
			}
			else
			{
				indata1 = CODEBOOK + n_shiftcnt;
				outdata = y;

				for (j=d_shiftcnt; j; j--) {
					*outdata = mult(*indata1, tmp_G);
					outdata++;
					indata1++;
 				}
				
				indata1 = CODEBOOK;
				for(j = fsiszecb - d_shiftcnt; j; j--) {
					*outdata = mult(*indata1 , tmp_G);
					outdata++;
					indata1++;
				}
			}
			
		}
		else if (mode == HALFRATE_VOICED) /* Normal search */
		{ 
			min_err=1000000000;
			
			/* construct c(n) */
			cb_out_shifted= &cb_out[CBLENGTH];
			
//			for (i= -CBLENGTH+1; i<fsiszecb; i++) {
//				cb_out_shifted[i] = CODEBOOK_HALF[(i-1+CBLENGTH)&(CBLENGTH - 1)];
//			}	/* complete c(n) in memory */

			indata1 = CODEBOOK_HALF;
			outdata = cb_out_shifted - CBLENGTH + 1;
			for (i= CBLENGTH; i; i--) {
				*outdata++ = *indata1++;
			}
			indata1 = CODEBOOK_HALF;
			for(i = fsiszecb; i; i--)
			{
				*outdata++ = *indata1++;
			}
			
			initial_recursive_conv(&cb_out_shifted[1], fsiszecb - 1, lpc_ir);	  
			
			for (i=0; i<CBLENGTH; i+=CB_STEP) {
				recursive_conv_10(&cb_out_shifted[-i], lpc_ir, fsiszecb);
				Exy=0;
				Eyy=0;
				
				indata1 = target;
				indata2 = cb_out_shifted - i;
				for (j=fsiszecb; j; j--) {
					Exy += (int)(int)(*indata1)*(int)(*indata2);
					Eyy += ((int)(*indata2)*(int)(*indata2) + 1024) >> 11;
					indata1++;
					indata2++;
				}			

				if ((ABS(Exy)<21)||(Eyy<21)) {
					Exy=205;
					Eyy=2048;
					G = 104858;   /* Q20 */
					err = 0;					
				}
				else
				{
					if(Exy >= 0)
					{
						n_shiftcnt = (norm_l(Exy) - 1);
						d_shiftcnt = norm_l(Eyy);
						s_Exy = extract_h(Exy << n_shiftcnt);
						S_Eyy = extract_h(Eyy << d_shiftcnt);
						s_G = divide_s(s_Exy, S_Eyy);
						//G = L_shl(s_G, 5 +  (d_shiftcnt - n_shiftcnt));
						G2 = 5 +  (d_shiftcnt - n_shiftcnt);
						if(G2 >= 0)
							G = s_G << G2;
						else
							G = s_G << (-G2);

					}else{
						n_shiftcnt = (norm_l(-Exy) - 1);
						d_shiftcnt = norm_l(Eyy);
						s_Exy = extract_h((-Exy) << n_shiftcnt);
						S_Eyy = extract_h(Eyy << d_shiftcnt);
						s_G = divide_s(s_Exy, S_Eyy);
						//G = -L_shl(s_G, 5 +  (d_shiftcnt - n_shiftcnt));
						G2 = 5 +  (d_shiftcnt - n_shiftcnt);
						if(G2 >= 0)
							G = -(s_G << G2);
						else
							G = -(s_G << (-G2));
					}

					s_G = (short)((G + 0x8000) >> 16);
					err = (s_Exy * s_G) >> n_shiftcnt;
	
//					if(Exy >= 0)
//					{
//						n_shiftcnt = (norm_l(Exy) - 1);
//						d_shiftcnt = norm_l(Eyy);
//						G = L_shr(L_divide((Exy << n_shiftcnt), 
//							(Eyy << d_shiftcnt)), (11 - (d_shiftcnt - n_shiftcnt)));
//					}else{
//						n_shiftcnt = (norm_l(-Exy) - 1);
//						d_shiftcnt = norm_l(Eyy);
//						G = -L_shr(L_divide(((-Exy) << n_shiftcnt), 
//							(Eyy << d_shiftcnt)), (11 - (d_shiftcnt - n_shiftcnt)));
//					}
//					
//					err = L_mpy_ls(L_shl(L_mpy_ls((Eyy << d_shiftcnt), s_G), 11 - d_shiftcnt)
//									- (Exy << 1), s_G);
					
//					n_shiftcnt = norm_l(G);
//					G2 =  L_mpy_ll((G << n_shiftcnt), (G << n_shiftcnt));
//					err = L_shl(L_mpy_ll((Eyy << d_shiftcnt), G2), 15 - d_shiftcnt - 2*n_shiftcnt)
//						- L_shl(L_mpy_ll(Exy, (G << n_shiftcnt)), 5 - n_shiftcnt);
				}

				if (err<min_err) {
					min_err=err;
					MinG=G;
					cb_params[cbsf][cbnum].i=(i+1)&(CBLENGTH - 1);
					Eyyopt[cbnum] = Eyy;
				}
			}    
			
			quantize_G(mode, MinG, 
				&(cb_params[cbsf][cbnum].G), 
				&(cb_params[cbsf][cbnum].qcode_G), 
				&(cb_params[cbsf][cbnum].qcode_Gsign),
				e_mem->dec.lastG, cbsf, e_mem->dec.G_pred);
			
			quantize_i(&(cb_params[cbsf][cbnum].i), 
				&(cb_params[cbsf][cbnum].qcode_i));

			tmp_G = cb_params[cbsf][cbnum].G;
			n_shiftcnt = CBLENGTH - cb_params[cbsf][cbnum].i;
			d_shiftcnt = CBLENGTH - n_shiftcnt;
			if(d_shiftcnt >= fsiszecb)
			{
//				for (j=0; j<fsiszecb; j++) {
//					y[j]= mult(CODEBOOK_HALF[(n_shiftcnt+j)&(CBLENGTH - 1)], tmp_G);
//				}
				indata1 = CODEBOOK_HALF + n_shiftcnt;
				outdata = y;
				for (j=fsiszecb; j; j--) {
					*outdata = mult(*indata1, tmp_G);
					outdata++;
					indata1++;
				}
			}
			else
			{
				indata1 = CODEBOOK_HALF + n_shiftcnt;
				outdata = y;

				for (j=d_shiftcnt; j; j--) {
					*outdata = mult(*indata1, tmp_G);
					outdata++;
					indata1++;
 				}
				
				indata1 = CODEBOOK_HALF;
				for(j = fsiszecb - d_shiftcnt; j; j--) {
					*outdata = mult(*indata1 , tmp_G);
					outdata++;
					indata1++;
				}
			}
		}
  }  
  
  return 0;
  
}/* end of compute_cb() */


/***********************************************************************
*
* compute_cb_gain()
*
************************************************************************/
/* For QUARTERRATE_UNVOICED */
int compute_cb_gain(
					short                  mode,
					struct ENCODER_MEM     *e_mem,
					struct CBPARAMS        cb_params[MAXCBPERPIT][MAXNUMCB],
					short                  *speech,
					short                  cbsf)
{
	int     Eyy, j;
	int     factor;
	short	*indata;
	int     n_shiftcnt, d_shiftcnt;
	short   sEyy, cbnum;
	for (cbnum=0; cbnum<NUMCB[mode]; cbnum++) 
	{
		Eyy=0;		
		/* get the gain from the speech itself and not the prediction residual */
		/* and scale it down by the prediction gain from the entire frame      */		
		indata = speech;
		for (j=FSIZE_DIV_CBSF[mode]; j; j-=8) 
		{
			Eyy = L_add(Eyy, (int)(*indata) * (*indata)); 	indata++;
			Eyy = L_add(Eyy, (int)(*indata) * (*indata)); 	indata++;
			Eyy = L_add(Eyy, (int)(*indata) * (*indata)); 	indata++;
			Eyy = L_add(Eyy, (int)(*indata) * (*indata)); 	indata++;

			Eyy = L_add(Eyy, (int)(*indata) * (*indata)); 	indata++;
			Eyy = L_add(Eyy, (int)(*indata) * (*indata)); 	indata++;
			Eyy = L_add(Eyy, (int)(*indata) * (*indata)); 	indata++;
			Eyy = L_add(Eyy, (int)(*indata) * (*indata)); 	indata++;
		} 

		factor = e_mem->features.log_pred_gain * FSIZE_DIV_CBSF[mode];		
		n_shiftcnt = (norm_l(Eyy) - 1);
		d_shiftcnt = norm_l(factor);
		Eyy = L_shr(L_divide((Eyy << n_shiftcnt), 
					(factor << d_shiftcnt)), (3 - (d_shiftcnt - n_shiftcnt)));		
		n_shiftcnt = norm_l(Eyy);
		if(n_shiftcnt & 1) n_shiftcnt--;
		Eyy <<= n_shiftcnt;
		sEyy = sqroot(Eyy);
		sEyy = mult(sEyy, 23170);
		n_shiftcnt >>= 1;
		sEyy = sEyy >> (n_shiftcnt + 3);
		Eyy = sEyy * G_FACTOR_16;
				
		quantize_G(mode, Eyy, &(cb_params[cbsf][cbnum].G), 
			&(cb_params[cbsf][cbnum].qcode_G), 
			&(cb_params[cbsf][cbnum].qcode_Gsign),
			e_mem->dec.lastG, cbsf, e_mem->dec.G_pred);
	}
	return 0;
}/* end of compute_cb_gain() */
