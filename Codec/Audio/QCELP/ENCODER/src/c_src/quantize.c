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
#define MATCH_1617  1

/* quantize.c - quantization routines */

#include "celp.h"
#include "basic_op.h"
#include "quantize.h"
#include "coderate.h"
#include "voType.h"

#define	PI_QC	12868  /*3.14159265358979323846*(1 << 12)*/
void compute_sns(
				 int   *R,
				 int   *sns,
				 short *lpc,
				 short *lsp)
{
    int    ftmp; // ftmp1, sns_hi, sns_lo;
	//int	   RtmpH, RtmpL;
	VO_S64 tempR;
	short  i, j, k, index;
    short  J[10];
    short  P[12], Q[12], Tmp[12], sinLSP[10], cosLSP[10];
	
    P[0] = 4096;
    P[11] = 4096;
    Q[0] = 4096;
    Q[11] = -4096;
    for (i=0; i<10; i++) {
		P[i+1]= -lpc[i]-lpc[9-i];
		Q[i+1]= -lpc[i]+lpc[9-i];
    }
	
    for (i=0; i<10; i++) {
		if ((i&1)==0) {
			for (j=0; j<=11; j++) {
				Tmp[j]=P[j];
			}
		}
		else {
			for (j=0; j<=11; j++) {
				Tmp[j]=Q[j];
			}
		}
		
		/* factor of PI not needed in real DSP, since only */
		/* care about relative sensitivities in quantizer */
		
		index = lsp[i] >> (15 - COSTABLE_SCALE);
		sinLSP[i] = mult(SinineTable_16[index], PI_QC);
		cosLSP[i] = CosineTable_16[index] >> 2;
		
		for (j=0; j<LPCORDER>>1; j++) {
			J[j]=Tmp[j];
			Tmp[j+1] += (short)(((int)cosLSP[i]*Tmp[j] + 2048) >> 12);
			Tmp[j+2] -= Tmp[j];			
			J[j] = ((int)J[j]*(int)sinLSP[i] + 2048) >> 12;
		}
		if ((i&1)==0) {
			for (j=0; j<LPCORDER>>1; j++) {
				J[9-j]=J[j];
			}
		}
		else {
			for (j=0; j<LPCORDER>>1; j++) {
				J[9-j]= -J[j];
			}
		}
		
		/* Compute ii element of J' R J */
		ftmp=0;
		for (j=0; j<10; j++) {
			ftmp += ((int)J[j] * (int)J[j] + 2048) >> 12;
		}

		tempR = (VO_S64)R[0]*(VO_S64)ftmp;

		//RtmpH = R[0] >> 16;
		//RtmpL = R[0] & 0xffff;
		//
		//ftmp1 = ((ftmp + 512) >> 10);
		//sns_hi = RtmpH * ftmp1;
		//sns_lo = RtmpL * ftmp1;
		//ftmp1 = ftmp - (ftmp1 << 10);
		//sns_hi += (RtmpH * ftmp1 + 512) >> 10;
		//sns_lo += (RtmpL * ftmp1 + RtmpL) >> 10;

		for (j=1; j<10; j++) {
			ftmp=0;
			for (k=j; k<10; k++) {
				ftmp += (J[k] * J[k-j] + 2048) >> 12;
			}

			ftmp = ftmp + ftmp;

			tempR += (VO_S64)R[j]*(VO_S64)ftmp;

			//if(R[j] >= 0)
			//{				
			//	RtmpH = R[i] >> 16;
			//	RtmpL = R[i] & 0xffff;

			//	ftmp1 =  ((ftmp + 512) >> 10);
			//	sns_hi += RtmpH * ftmp1;
			//	sns_lo += RtmpL * ftmp1;

			//	ftmp1 = ftmp - (ftmp1 << 10);			
			//	sns_hi += (RtmpH * ftmp1 + 512) >> 10;			
			//	sns_lo += (RtmpL * ftmp1 + 512) >> 10;
			//}
			//else
			//{
			//	RtmpH = (-R[i]) >> 16;
			//	RtmpL = (-R[i]) & 0xffff;
			//	
			//	ftmp1 =  ((ftmp + 512) >> 10);
			//	sns_hi -= RtmpH * ftmp1;
			//	sns_lo -= RtmpL * ftmp1;

			//	ftmp1 = ftmp - (ftmp1 << 10);			
			//	sns_hi -= (RtmpH * ftmp1 + 512) >> 10;			
			//	sns_lo -= (RtmpL * ftmp1 + 512) >> 10;
			//}
		}

		if(((tempR + 2048) >> 12) > MAX_32)
			sns[i] = MAX_32;
		else
			sns[i] = (int)((tempR + 2048) >> 12);

		if(sns[i] < 4096)
			sns[i] = 4096;

		//if(sns_hi > MAX_16)
		//{
		//	sns[i] = MAX_32;
		//}
		//else
		//{
		//	if(sns_hi < 0)
		//	{
		//		sns[i] = 4096;
		//	}
		//	else
		//	{
		//		sns[i] = (sns_hi << 16);
		//		sns[i] = L_add(sns_lo, sns[i]);
		//	}
		//}
    }
}

void quantize_lpc(
				  short               mode,
				  short               lpc[LPCORDER],
				  short               *lsp,
				  short               *qlsp,
				  int                 *R,
				  struct LPCPARAMS    *lpc_params,
				  struct ENCODER_MEM  *e_mem)
{
	int    sns[LPCORDER];
	int	   err, minerr=1000000000, suberr;
	int    i,j;
	int    minj = 0, err_s;
	short  dlsp[3];
	short  *VQ;

	if (LSP_QTYPE[mode]==VQT) 
	{
		/* Pairwise differential LSP VQ using optimal LSP sensitivities */
		compute_sns(R, sns, lpc, lsp);
		for (i=0; i<LPCORDER/2; i++) 
		{
			minerr=1000000000;
			if (i==0) {
				dlsp[0]=lsp[0];
			}
			else {
				dlsp[0]=lsp[2*i]-qlsp[2*i-1];
			}
			dlsp[1]=lsp[2*i+1]-lsp[2*i];

			switch (i) {
				case 0:
					VQ=&LSPVQ0[0][0];
					break;
				case 1:
					VQ=&LSPVQ1[0][0];
					break;
				case 2:
					VQ=&LSPVQ2[0][0];
					break;
				case 3:
					VQ=&LSPVQ3[0][0];
					break;
				case 4:
					VQ=&LSPVQ4[0][0];
					break;
			}
			for (j=0; j<LSPVQSIZE[i]; j++) 
			{
				suberr = (dlsp[0] - VQ[2*j]);
				err = L_mpy_ls(sns[2*i], mult(suberr,suberr));

				suberr += (dlsp[1] - VQ[2*j+1]);
				err += L_mpy_ls(sns[1+2*i], mult(suberr,suberr));

				if ((j==0)||(minerr>err)) {
					minerr=err;
					minj=j;
				}
			}
			lpc_params->qcode_lsp[i]=minj;
			if (i==0) {
				qlsp[0] = VQ[2*minj];
			}
			else {
				qlsp[2*i] = qlsp[2*i-1] + VQ[2*minj];
			}
			qlsp[2*i+1] = qlsp[2*i] + VQ[2*minj+1];
		}
		unquantize_lsp(mode, qlsp, &(e_mem->dec.last_qlsp[0]),
				lpc_params->qcode_lsp, &(e_mem->dec));
	}
	else 
	{
		for (i=0; i<LPCORDER; i++) {
			err_s=lsp[i] - initlsp[i]
				- mult(LSP_DPCM_DECAY[mode], (e_mem->dec.last_qlsp[i]- initlsp[i]));
			lin_quant(&(lpc_params->qcode_lsp[i]), 
					MIN_DELTA_LSP[mode][i], MAX_DELTA_LSP[mode][i],
					NUM_LSP_QLEVELS[mode][i], err_s);
		}
		unquantize_lsp(mode, qlsp, &(e_mem->dec.last_qlsp[0]),
				lpc_params->qcode_lsp, &(e_mem->dec));
	}
}/* end of quantize_lpc() */

void unquantize_lsp(
					short               mode,
					short               *qlsp,          /* output unquantized lsp's */
					short               *last_qlsp,     /* input - unquantized last lsp */
					short               *qcode,
					struct DECODER_MEM  *d_mem)
{
	int   err;
	short *VQ;
	int i;
	short smooth;
	short lsp_spread_factor;
	if (LSP_QTYPE[mode]==VQT) {
		for (i=0; i<LPCORDER/2; i++) {
			switch (i) {
			case 0:
				VQ=&LSPVQ0[0][0];
				break;
			case 1:
				VQ=&LSPVQ1[0][0];
				break;
			case 2:
				VQ=&LSPVQ2[0][0];
				break;
			case 3:
				VQ=&LSPVQ3[0][0];
				break;
			case 4:
				VQ=&LSPVQ4[0][0];
				break;
			}
			if (i==0) {
				qlsp[0] = VQ[2*qcode[i]];
			}
			else {
				qlsp[2*i] = qlsp[2*i-1] + VQ[2*qcode[i]];
			}
			
			qlsp[2*i+1] = qlsp[2*i] + VQ[2*qcode[i]+1];
		}
		for (i=0; i<LPCORDER; i++){/* subtract off bias */
			d_mem->pred_qlsp[i] = qlsp[i] - initlsp[i];
		}
	}
	else {/* EIGHTH rate */
		for (i=0; i<LPCORDER; i++) {
			if(qcode[i] == 1) 
				err = 655;       
			else
				err = -655; 			
			qlsp[i] = err + mult(LSP_DPCM_DECAY[mode], d_mem->pred_qlsp[i]);
			d_mem->pred_qlsp[i] = qlsp[i];	
			/* add in bias */
			qlsp[i] =add(qlsp[i], initlsp[i]);
		}
	}
	if(mode == EIGHTH){
		/* Check for stability and perform LSP spreading if necessary */
		lsp_spread_factor = 655;	
		if (qlsp[0]<lsp_spread_factor) qlsp[0]=lsp_spread_factor;
		for (i=1; i<LPCORDER; i++) {
			if (qlsp[i]-qlsp[i-1]<lsp_spread_factor) {
				qlsp[i]=qlsp[i-1]+lsp_spread_factor;
			}
		}
		if (qlsp[LPCORDER-1] > MAX_16 - lsp_spread_factor) {
			qlsp[LPCORDER-1] = MAX_16 - lsp_spread_factor;
		}
		for (i=LPCORDER-2; i>=0; i--) {
			if (qlsp[i+1] - qlsp[i] < lsp_spread_factor) {
				qlsp[i] = qlsp[i+1] - lsp_spread_factor;
			}
		}	
		if (d_mem->low_rate_cnt<10) {
			smooth = 4096;
		}
		else{
			smooth = 29491; 
		}
		for (i=0; i<LPCORDER; i++){			
			qlsp[i]=  add(mult((last_qlsp[i] - qlsp[i]), smooth), qlsp[i]);
		}
	}	
}/* end of unquantize_lsp() */
			 
void quantize_min_lag(
					  short  min_lag,
					  short  *qcode_min_lag)
{
	*qcode_min_lag= min_lag-MINLAG+1;
}
  
void unquantize_min_lag(
						short  *min_lag,
						short qcode_min_lag)
{
	*min_lag= qcode_min_lag+MINLAG-1;
}
  
void lin_quant(
			   short   *qcode,
			   short   min,
			   short   max,
			   short   num_levels,
			   short   input)
{
	int sum, maxmin;
	sum = ((int)(num_levels-1)*(input-min));
	maxmin = max - min;
	sum += maxmin >> 1;
	*qcode = sum/maxmin;
	if (*qcode>=num_levels) 
		*qcode=num_levels-1;
	if (*qcode<0) 
		*qcode=0;	
}
  
void lin_unquant(
				 short   *output,
				 short   min,
				 short   max,
				 short   num_levels,
				 short   qcode)
{
	*output=(max-min)*qcode/(num_levels-1) + min;
}
  
void unquantize_i(
				  short   *i,
				  short   *qcode_i)
{
	*i=*qcode_i;
}
  
void quantize_i(
				short   *i,
				short   *qcode_i)
{
	*qcode_i= *i;
	unquantize_i(i, qcode_i);
}

void unquantize_lag(
					short   *lag,
					short   *qcode_lag,
					short   *frac,
					short   *qcode_frac)
{	
	if(*qcode_lag == 0){
		*lag = MAXLAG;
		*frac = 0;
	}
	else if(*qcode_frac == 0){
		*lag=*qcode_lag + 16;
		*frac = 0;
	}
	else if(*qcode_frac == 1){
		*lag=*qcode_lag + 16;
		*frac = 2;
	}
}

/* quantize/encode pitch lag and pitch lag fraction bit */ 
void quantize_lag(
				  short   *lag,
				  short   *qcode_lag,
				  short   *frac,
				  short   *qcode_frac)
{
	if(*lag == 0){
		*qcode_lag = 0;
		*qcode_frac = 0;
	}
	else if(*lag > 16 && *frac == 0 ){
		*qcode_lag= *lag - 16;
		*qcode_frac = 0;
	}
	else if( (*lag > 16 && *lag < MAXLAG) && *frac == 2 ){
		*qcode_frac= 1;
		*qcode_lag= *lag - 16;
	}
	else{
		return;
	}	
	unquantize_lag(lag, qcode_lag, frac, qcode_frac);
}

void unquantize_b(
				  short   *q_b,
				  short   *qcode_b,
				  short   lag)
{
	
	if(*qcode_b == 0){
		if(lag == 0)
			*q_b = 0;
		else
			*q_b = 1024;
	}
	else{
		*q_b = (*qcode_b + 1) << 10;
	}	
}

void quantize_b(
				short  unq_b,
				short  *q_b,
				short  *qcode_b)
{
	short tmp;
	short lag;	
	tmp = (unq_b + 512) >> 10;
	if(tmp <= 0){
		*qcode_b = 0;
		lag = 0;
	}
	else{
		lag = 1;
		if(tmp >= 8){
			*qcode_b = 7;
		}
		else
			*qcode_b = tmp - 1;
	}
	unquantize_b(q_b, qcode_b, lag);
}

void unquantize_G(
				  short   mode,
				  short   *q_G,
				  short   *qcode_G,
				  short   *qcode_Gsign,
				  short   *lastG,
				  short   cbsf,
				  short   *G_pred)
{
	int i;
	short pred;	
	for (i=GPRED_ORDER-1; i>0; i--) {
		G_pred[i]=G_pred[i-1];
	}	
	if (G_QUANT_TYPE[mode]==LINEAR) {
		if ((mode!=FULLRATE_VOICED)||(((cbsf+1)&3) !=0)) {
			lin_unquant(q_G, MING[mode], MAXG[mode], NUMBER_OF_G_LEVELS[mode], *qcode_G);
			for (i=GORDER-1; i>= 1; i--) {
				lastG[i] = lastG[i-1];
			}
			lastG[0] = *q_G;
			G_pred[0] = *q_G;
			
			*q_G= GA[shr(*q_G, 4)+6];
			
			if (*qcode_Gsign==NEGATIVE) {
				*q_G = -(*q_G);
			}
		}
		else {
			pred=0;
			for (i=0; i<GORDER; i++) {
				pred+=lastG[i];
			}
			pred = pred/GORDER;  //(int) (pred/GORDER); /* truncation */
			
			/* limit the prediction value */
			if(pred > MAX_GPRED)
				pred = MAX_GPRED;
			else if(pred < MIN_GPRED)
				pred = MIN_GPRED;
			
			lin_unquant(q_G, RELMING, RELMAXG, 
				RELNUMBER_OF_G_LEVELS[mode], *qcode_G);
			*q_G+= pred;
			for (i=GORDER-1; i>= 1; i--) {
				lastG[i] = lastG[i-1];
			}
			lastG[0] =  *q_G;
			G_pred[0] = *q_G;
			
			*q_G= GA[shr(*q_G, 4)+6];
			
			if (*qcode_Gsign==NEGATIVE) {
				*q_G = -(*q_G);
			}
		}
	}
	if (G_QUANT_TYPE[mode]==DELTA) {
		pred=0;
		for (i=0; i<GORDER; i++) {
			pred+=lastG[i];
		}
		
		lin_unquant(q_G, MING[mode], MAXG[mode], NUMBER_OF_G_LEVELS[mode], *qcode_G);
		*q_G+=pred;
		
		for (i=GORDER-1; i>= 1; i--) {
			lastG[i] = lastG[i-1];
		}
		lastG[0] =  *q_G;
		G_pred[0] = *q_G;
		
		*q_G= GA[shr(*q_G, 4)+6];
		
		if (*qcode_Gsign==NEGATIVE) {
			*q_G = -(*q_G);
		}
	}
}/* end of unquantize_G() */
 

void quantize_G(
				short   mode,
				int     unq_G,
				short   *q_G,
				short   *qcode_G,
				short   *qcode_Gsign,
				short   *lastG, /* only needed for FULLRATE every fourth CB subframe */
				short   cbsf,
				short   *G_pred)
{
	int    pre_unqG;
	int    pred;
	int    i;		
	if (G_QUANT_TYPE[mode]==LINEAR) {
		if ((mode!=FULLRATE_VOICED)||(((cbsf+1) & 3) !=0)) {
			if (unq_G >= 10486) {
				//pre_unqG = L_sub(L_add(fnLog10(unq_G), 626255212), 404035621);
				pre_unqG = fnLog10(unq_G) + 222219591;
				unq_G = extract_h(((pre_unqG + 8) >> 4)*5);
				lin_quant(qcode_G, MING[mode], MAXG[mode], NUMBER_OF_G_LEVELS[mode], unq_G);				
				*qcode_Gsign=POSITIVE;
			}
			else if (unq_G <= - 10486){
				//pre_unqG = L_sub(L_add(fnLog10(-unq_G), 626255212), 404035621);
				pre_unqG = fnLog10(-unq_G) + 222219591;
				unq_G = extract_h(((pre_unqG + 8) >> 4)*5);
				lin_quant(qcode_G, MING[mode], MAXG[mode], NUMBER_OF_G_LEVELS[mode], unq_G);
				*qcode_Gsign=NEGATIVE;
			}
			else {
				lin_quant(qcode_G, MING[mode], MAXG[mode], NUMBER_OF_G_LEVELS[mode], -640);  
				*qcode_Gsign=NEGATIVE;	
				if(mode == EIGHTH || mode == QUARTERRATE_UNVOICED){
					*qcode_Gsign=POSITIVE;
				}
			}
		}
		else {
			pred=0;
			for (i=0; i<GORDER; i++) {
				pred+=lastG[i];
			}
			pred = pred/GORDER;   			
			/* limit the prediction value */
			if(pred > MAX_GPRED)
				pred = MAX_GPRED;
			else if(pred < MIN_GPRED)
				pred = MIN_GPRED;	
			if (unq_G>=10486) {
				//pre_unqG = L_sub(L_add(fnLog10(unq_G), 626255212), 404035621);
				pre_unqG = fnLog10(unq_G) + 222219591;
				unq_G = extract_h(((pre_unqG + 8) >> 4)*5);
				lin_quant(qcode_G, RELMING, RELMAXG, 
					RELNUMBER_OF_G_LEVELS[mode], unq_G - pred);

				*qcode_Gsign=POSITIVE;
			}
			else if (unq_G<= -10486){
				//pre_unqG = L_sub(L_add(fnLog10(-unq_G), 626255212), 404035621);
				pre_unqG = fnLog10(-unq_G) + 222219591;
				unq_G = extract_h(((pre_unqG + 8) >> 4)*5);
				lin_quant(qcode_G, RELMING, RELMAXG, 
					RELNUMBER_OF_G_LEVELS[mode], unq_G- pred);
				*qcode_Gsign=NEGATIVE;
			}
			else {
				lin_quant(qcode_G, RELMING, RELMAXG, 
					RELNUMBER_OF_G_LEVELS[mode], -640 - pred  );
				*qcode_Gsign=NEGATIVE;
			}
		}    
	}
	if (G_QUANT_TYPE[mode]==DELTA) {
		pred=0;
		for (i=0; i<GORDER; i++) {
			pred+=lastG[i];
		}
		if (unq_G>0) {
			//pre_unqG = L_sub(L_add(fnLog10(unq_G), 626255212), 404035621);
			pre_unqG = fnLog10(unq_G) + 222219591;
			unq_G = extract_h(((pre_unqG + 8) >> 4)*5);
			lin_quant(qcode_G, MING[mode], MAXG[mode], NUMBER_OF_G_LEVELS[mode], unq_G-pred );
			*qcode_Gsign=POSITIVE;
		}
		else {
			//pre_unqG = L_sub(L_add(fnLog10(-unq_G), 626255212), 404035621);
			pre_unqG = fnLog10(-unq_G) + 222219591;
			unq_G = extract_h(((pre_unqG + 8) >> 4)*5);
			lin_quant(qcode_G, MING[mode], MAXG[mode], NUMBER_OF_G_LEVELS[mode], unq_G-pred );
			*qcode_Gsign=NEGATIVE;
		}
	}	
	unquantize_G(mode, q_G, qcode_G, qcode_Gsign, lastG, cbsf, G_pred);	
}/* quantize_G() */
  
void unquantize_G_8th(
					  short   *q_G,          /* reconstructed G value */
					  short   *qcode_G,      /* quantization code for Gain */
					  short   *G_pred        /* predictor */
					  )
{
	int     i;
	short   pred;
	short   ind, G_dB;
	
	if(*qcode_G < 0 || *qcode_G > 3){
		return;
	}	
	pred=0;
	for (i=0; i<GPRED_ORDER; i++) {
		pred+=G_pred[i];
	}
	
	pred = pred/GPRED_ORDER;
	
	G_dB = pred;
	
	G_dB -= 16;
	if(G_dB < 64)
		G_dB = 64;
	else if(G_dB > 928)
		G_dB = 928;
	
	for (i=GPRED_ORDER-1; i>0; i--) {
		G_pred[i]=G_pred[i-1];
	}
	G_pred[0] = G_dB+QG8[*qcode_G];
	
	ind = G_dB+96;  /* 6 is offset required for GA[] */
	*q_G= GA[shr(ind+QG8[*qcode_G], 4)];	
}/* end of unquantize_G_8th() */

void quantize_G_8th(
					short     unq_G,            /* input Gain value before quantization  */
					short     *q_G,             /* recontructed CB Gain */
					short     *qcode_G,         /* quant. code for CB Gain */
					short     *G_pred           /* prediction for CB Gain */
					)
{
	int    i, j;
	short  ind, G_dB;
	short  pred;
	short  G;
	int    min_error, err;
	
	pred=0;
	for (i=0; i<GPRED_ORDER; i++) {
		pred+=G_pred[i];
	}

	G_dB = pred/GPRED_ORDER;
	
	G_dB -= 16;
	if(G_dB < 64)
		G_dB = 64;
	else if(G_dB > 928)
		G_dB= 928;
	ind = G_dB + 96;
	G=unq_G;
	
	min_error=16000000; /* SAB: changed from 1000000; */
	*qcode_G=0;
	for (i=0; i<4; i++) {
		j = shr(ind+QG8[i], 4);
		err= ((int)(G-GA[j])*(G-GA[j])) >> 4;
		if (min_error>err) {
			min_error=err;
			*qcode_G=i;
		}
	}
	unquantize_G_8th(q_G, qcode_G, G_pred);	
}/* end of quantize_G_8th() */



