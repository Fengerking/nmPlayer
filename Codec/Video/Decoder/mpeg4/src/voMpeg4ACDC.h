/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#ifndef _MP4_PREDICT_H_
#define _MP4_PREDICT_H_
#include "voMpeg4DecGlobal.h"
#include "voMpeg4Dec.h"
#include "voMpeg4Parser.h"

static INLINE idct_t* dc_recon(VO_MPEG4_DEC* pDec, VO_S32 BlkNum, VO_S32 pos, VO_S32 dc_scaler )
{
	VO_S32	Fa,Fb,Fc;
	idct_t *dc;

	Fb = 1024;
	Fc = 1024;

	if (BlkNum < 4) {
		dc = pDec->dc_lum;
		if (pos >= MB_X*2)
		{
			Fb=dc[(pos-MB_X*2) & DC_LUM_MASK];
			Fc=dc[(pos+1-MB_X*2) & DC_LUM_MASK];
		}
		dc += pos & DC_LUM_MASK;
	}
	else {
		dc = pDec->dc_chr[BlkNum & 1];
		if (pos >= MB_X)
		{
			Fb=dc[(pos-MB_X) & DC_CHR_MASK];
			Fc=dc[(pos+1-MB_X) & DC_CHR_MASK];
		}
		dc += pos & DC_CHR_MASK;
	}

	Fa=dc[0];

#ifdef VODIVX3
    Fa = (Fa + (dc_scaler >> 1)) / dc_scaler;
    Fb = (Fb + (dc_scaler >> 1)) / dc_scaler;
    Fc = (Fc + (dc_scaler >> 1)) / dc_scaler;

	if (abs(Fb - Fa) <= abs(Fb - Fc)) {
#else
	if (abs(Fb - Fa) < abs(Fb - Fc)) {
#endif
		pDec->predict_dir = TOP;
		Fa = Fc;
	}
	else {
		pDec->predict_dir = LEFT;
		//Fa = Fa;
	}

#ifndef VODIVX3
	if (Fa>0)
		Fa+=dc_scaler>>1;
	else
		Fa-=dc_scaler>>1;
	Fa /= dc_scaler;
#endif

	++dc;
	Fa *= dc_scaler;
	*dc = (idct_t)Fa;
	return dc;
}

#ifdef VODIVX3
static INLINE void ac_recon(VO_MPEG4_DEC* pDec, VO_MPEG4_DEC_LOCAL* vDecLocal, idct_t *block, VO_S32 BlkNum, VO_S32 pos)
{
	VO_S32 i;
	idct_t *ac_top;
	idct_t *ac_left;

	if (BlkNum < 4) {
		ac_top = pDec->ac_top_lum[pos & (MB_X*2-1)];
		ac_left = pDec->ac_left_lum[(pos >> (MB_X2+1)) & 1];
		i = MB_X*2-1; //stride-1
	}
	else {
		ac_top = pDec->ac_top_chr[pos & (MB_X-1)][BlkNum & 1];
		ac_left = pDec->ac_left_chr[BlkNum & 1];
		i = MB_X-1; //stride-1
	}

	if (pDec->bACPred)
	{
		if (pDec->predict_dir == TOP)
		{
			if (pos > i)
				for (i = 1; i < 8; i++) 
				{
					block[i] = (idct_t)(block[i] + ac_top[i]);
					//DEBUG_MSG2(DEBUG_VCODEC,T("predictor[%i] %i"), i, ac_top[i] / pDec->dc_scaler);
				}
		}
		else // left prediction
		{
			if (pos & i)
				for (i = 1; i < 8; i++)
				{
					block[i<<3] = (idct_t)(block[i<<3] + ac_left[i]);
					//DEBUG_MSG2(DEBUG_VCODEC,T("predictor[%i] %i"), i<<3, ac_left[i] / pDec->dc_scaler);
				}
		}
	}

	for (i = 1; i < 8; i++) 
	{
		ac_top[i] = block[i];
		ac_left[i] = block[i<<3];
	}
}
#else

#define DIV_DIV(a,b)    (((a)>0) ? ((a)+((b)>>1))/(b) : ((a)-((b)>>1))/(b))

#define RESCALE(a, pq, cq)      ((a != 0) ? DIV_DIV((a) * (pq),(cq)) : 0)
								  

static INLINE void ac_recon(VO_MPEG4_DEC* pDec,VO_MPEG4_DEC_LOCAL* vDecLocal, idct_t *block, VO_S32 BlkNum, VO_S32 pos)
{
	VO_S32 i, nPreQuant, nCurQuant = pDec->nQuant, bTop, bLeft;
	idct_t *ac_top;
	idct_t *ac_left;
	idct_t nTop[8], nLeft[8];

	VO_S32 q_scale = (nCurQuant) << 1;
	VO_S32 q_add = (nCurQuant-1)|1;

// 	nCurQuant = pDec->nQuant;

	if (!pDec->nQuantType) //is H263 quant type
	{
		//Quant back
		for (i = 1; i < 8; i++) {
			VO_S32 nLevel = block[i];

			
			if(nLevel>0){
				nLevel -= q_add;
				nLevel /= q_scale;
			}else if(nLevel <0){
				nLevel += q_add;
				nLevel /= q_scale;
			}
			nTop[i] = (idct_t)nLevel;

			nLevel = block[i<<3];

			if(nLevel>0){
				nLevel -= q_add;
				nLevel /= q_scale;
			}else if(nLevel <0){
				nLevel += q_add;
				nLevel /= q_scale;
			}
			nLeft[i] = (idct_t)nLevel;
		}
	}
	else	//mpeg quant type
	{
		//they havn't do dequant(see more in MpegGetBlockVld), so there's no need to do quant back here
		for (i = 1; i < 8; i++) {
			nTop[i] = block[i];
			nLeft[i] = block[i<<3];
		}	
	}

	if (BlkNum < 4) {
		ac_top = pDec->ac_top_lum[pos & (MB_X*2-1)];
		ac_left = pDec->ac_left_lum[(pos >> (MB_X2+1)) & 1];
		i = MB_X*2-1; //stride-1
		bTop = BlkNum<2;
		bLeft = !(BlkNum&1);
	}else {
		ac_top = pDec->ac_top_chr[pos & (MB_X-1)][BlkNum & 1];
		ac_left = pDec->ac_left_chr[BlkNum & 1];
		i = MB_X-1; //stride-1
		bTop = 1;
		bLeft = 1;
	}
	


	if (pDec->bACPred){
		if (pDec->predict_dir == TOP){
			if (pos > i){
				nPreQuant = *(vDecLocal->pMBQuant);

				if((nPreQuant != nCurQuant)&&bTop){
					for(i = 1; i < 8; i++){
						ac_top[i] = (idct_t)RESCALE(ac_top[i], nPreQuant, nCurQuant);
					}
				}
				
				if (!pDec->nQuantType)
				{
					for (i = 1; i < 8; i++){ 
						nTop[i] += ac_top[i];
						if(nTop[i]> 0){
							block[i] = (idct_t)(nTop[i] * q_scale + q_add);
						}else if(nTop[i]< 0){
							block[i] = (idct_t)(nTop[i] * q_scale - q_add);
						}else{
							block[i] = 0;
						}
					}
				}
				else	//mpeg quant type
				{
					for (i = 1; i < 8; i++)
					{
						nTop[i] += ac_top[i];	//do dequant later
					}
				}
			}
		}else{ // left prediction
		
			if (pos & i){
				nPreQuant = *(vDecLocal->pMBQuant - 1);

				if((nPreQuant != nCurQuant)&&bLeft){
					for(i = 1; i < 8; i++){
						ac_left[i] = (idct_t)RESCALE(ac_left[i], nPreQuant, nCurQuant);
					}
				}

				if (!pDec->nQuantType)
				{
					for (i = 1; i < 8; i++){ 
						nLeft[i] += ac_left[i];
						if(nLeft[i]> 0){
							block[i<<3] = (idct_t)(nLeft[i] * q_scale + q_add);
						}else if(nLeft[i]< 0){
							block[i<<3] = (idct_t)(nLeft[i] * q_scale - q_add);
						}else{
							block[i<<3] = 0;
						}
					}
				}
				else	//mpeg quant type
				{
					for (i = 1; i < 8; i++)
					{
						nLeft[i] += ac_left[i];	//do dequant later
					}
				}
			}
		}
	}

	for (i = 1; i < 8; i++){ 
		ac_top[i] = nTop[i];
		ac_left[i] = nLeft[i];
	}

	if (pDec->nQuantType)	//mpeg quant type
	{
		//do dequant for top & left
		for (i = 1; i < 8; i++)
		{ 
			if (nTop[i] > 0)
			{
				block[i] = (idct_t)((q_scale*nTop[i]*pDec->quant[0][i])>>4);
			}
			else if (nTop[i] < 0)
			{
				block[i] = -(idct_t)((q_scale*(-nTop[i])*pDec->quant[0][i])>>4);
			}
			else
			{
				block[i] = 0;
			}
			if (nLeft[i] > 0)
			{
				block[i<<3] = (idct_t)((q_scale*nLeft[i]*pDec->quant[0][i<<3])>>4);
			}
			else if (nLeft[i] < 0)
			{
				block[i<<3] = -(idct_t)((q_scale*(-nLeft[i])*pDec->quant[0][i<<3])>>4);
			}
			else
			{
				block[i<<3] = 0;
			}
		}
	}
}
#endif
extern void RescuePredict(VO_MPEG4_DEC*, VO_S32 pos);

//////////////////////////////////////////////////////////////////////////////////////////////
//Predict MV
extern VO_S32 GetPMV(VO_S32 Block_Num, VO_S32 pos, VO_S32 slice_pos, VO_MPEG4_DEC *pDec);
extern VO_S32 GetPMV_interlace(VO_S32 Block_Num, VO_S32 pos, VO_S32 slice_pos, VO_MPEG4_DEC *pDec);
extern VO_S32 GetMV(VO_S32 fcode, VO_S32 prev, VO_MPEG4_DEC* pDec, VO_S32 bField/*, VO_S32 nMBPos*/);
extern VO_S32 CorrectMV(VO_S32 MV, VO_MPEG4_DEC* pDec, VO_S32 nXMBPos, VO_S32 nYMBPos);

#endif

