/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#include "voMpegReadbits.h"
//#include "voMpeg4Parser.h"
#include "voMpeg4ACDC.h"
#include "voMpegMC.h"
#include "voMpegBuf.h"
#include "voMpeg4Gmc.h"
#include "voMpeg4Frame.h"
#include "voMpeg4ACDC.h"
		
VOCONST VO_S8 DQtab[4] = {
	-1, -2, 1, 2
};
	
extern int frameNumber;

/**/
#if ENABLE_DATA_PARTITION


#define MOTION_MARKER 0x1F001
#define DC_MARKER     0x6B001


static void ResetDPBuf(VO_MPEG4_DEC *pDec, VO_S32 mbCount)
{
	pDec->DP_Buffer->nLastMBNum	= -1;

	SetMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, pDec->DP_Buffer->bACPred, 0,\
			mbCount*6 + mbCount*sizeof(BLOCK_32BIT) + 6*CACHE_LINE);
}

VO_U32 MPEG4DecDPIVOP( VO_MPEG4_DEC *pDec )
{
	VO_S32 mcbpc, nQuant, nMBPos, nMBPosA, nYMBPos, nXMBPos;
	VO_MPEG4_DEC_LOCAL vDecLocal;
	VO_S32 mbCount = pDec->nMBWidth*pDec->nMBHeight;

	DP_BUFFER* DP_Buffer = pDec->DP_Buffer;
	//VO_U32 voRC = VO_ERR_NONE;

	VO_S8 blockLen[6] = {0};
	VO_S8 blockscanType[6] = {0};
	idct_t blockptr[6*64] = {0};

	vDecLocal.bFieldDct = 0;
	vDecLocal.blockLen = blockLen;
	vDecLocal.scanType = blockscanType;
	vDecLocal.blockptr = blockptr;

	pDec->nLastRefFrame = pDec->nFrame;
	pDec->nMapOfs = pDec->nFrame;
	SetMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, pDec->frameMap, 0, pDec->nPosEnd); // set all block to current frame (and clear rescue flag)
	pDec->pCurFrameMB = pDec->pCurFrame;

	pDec->nResyncMarkerBits = RESYNC_MARK_BITS;
	ResetDPBuf(pDec, mbCount);

	nMBPosA = 0;
	nYMBPos = 0;
	nXMBPos = 0;


	do {
DPIVOPLOOP1:	
		for (nMBPos = nMBPosA; nMBPos < mbCount; nMBPos++) {
			UPDATE_CACHE(pDec);

			// skip stuffing
			while (SHOW_BITS(pDec,9) == 1){
				FLUSH_BITS(pDec,9);
				UPDATE_CACHE(pDec);
				if (EofBits(pDec))
					return VO_ERR_INPUT_BUFFER_SMALL;
			}

			/*stream: 24bits available*/		
			if(SHOW_BITS(pDec,19) == DC_MARKER){
				break;
			}
			mcbpc = GetMCBPC_I(pDec); // mcbpc

			nQuant = pDec->nQuant;

			if ((mcbpc & 7) == INTRA_Q){ //mb_type
				VO_S32 q = GetBits(pDec,2); //DQtab[dquant]
				q = nQuant + DQtab[q]; 
				pDec->nQuant = nQuant = q<1 ? 1: (q>31 ? 31:q);
			}

			if(nQuant < pDec->nIntraDCThreshold){
				GetDPDCDiff(pDec, nMBPos);
			}
			DP_Buffer->quant[nMBPos]	= (VO_U8)nQuant;
			DP_Buffer->mcbpc[nMBPos]	= (VO_S16)mcbpc;	

		}
		UPDATE_CACHE(pDec);
		if(GetBits(pDec,19) == DC_MARKER){
			DP_Buffer->nLastMBNum = nMBPos;
		}
		//else{
		//	if(nMBPos == mbCount){
		//		return VO_ERR_NONE;
		//	}
		//}


		/*After dc_marker*/
		UPDATE_CACHE(pDec);

		for (nMBPos= nMBPosA; nMBPos < DP_Buffer->nLastMBNum; nMBPos++){ 
			while (SHOW_BITS(pDec,9) == 1){
				FLUSH_BITS(pDec,9);
				UPDATE_CACHE(pDec);
				if (EofBits(pDec))
					return VO_ERR_INPUT_BUFFER_SMALL;
			}
			UPDATE_CACHE(pDec);	
			DP_Buffer->bACPred[nMBPos]	= (VO_U8)GetBits(pDec,1);
			DP_Buffer->cbpy[nMBPos]		= (VO_U8)GetCBPY(pDec);
		}
		nMBPos = nYMBPos * MB_X + nXMBPos;
		vDecLocal.pMBQuant = pDec->quantrow + nXMBPos;
		for (;nMBPos < pDec->nPosEnd; nMBPos += MB_X-pDec->nMBWidth) {
			for (;POSX(nMBPos) < pDec->nMBWidth; ++nMBPos) {
				nYMBPos = POSY(nMBPos);
				nXMBPos = POSX(nMBPos);

				UPDATE_CACHE(pDec);

				if(nMBPosA >= DP_Buffer->nLastMBNum && nMBPosA < mbCount){
					if(VO_ERR_NONE != Resync(pDec, &nMBPos))
						return VO_ERR_DEC_MPEG4_I_FRAME;


					nYMBPos = POSY(nMBPos);
					nXMBPos = POSX(nMBPos);
					nMBPosA = nYMBPos * pDec->nMBWidth + nXMBPos;

					if (DP_Buffer->nLastMBNum >= mbCount)
						return VO_ERR_NONE;
					goto DPIVOPLOOP1;
				}
// 				vDecLocal.pCurY = pDec->pCurFrame->y + (nYMBPos << 4) * pDec->nLumEXWidth + (nXMBPos << 4); 
// 				vDecLocal.pCurUV[0] = pDec->pCurFrame->u + (nYMBPos << 3) * pDec->nChrEXWidth + (nXMBPos << 3); 
// 				vDecLocal.pCurUV[1] = pDec->pCurFrame->v + (nYMBPos << 3) * pDec->nChrEXWidth + (nXMBPos << 3); 


				pDec->bACPred	= DP_Buffer->bACPred[nMBPosA];//GetBits(pDec,1);
				pDec->nCBP		= (DP_Buffer->cbpy[nMBPosA] << 2) | ((DP_Buffer->mcbpc[nMBPosA] >> 4) & 3);
				pDec->nQuant	= DP_Buffer->quant[nMBPosA];
// 				pDec->fpReconMB[nMBPosA] = ReconIntraMB;
				if(VO_ERR_NONE != MPEG4DecIntraMB(pDec, &vDecLocal,nMBPos, nMBPosA))
					return VO_ERR_DEC_MPEG4_INTRA_MB;
				ReconIntraMB(pDec, &vDecLocal, nMBPos);
// 				if (pDec->fpReconMB[nMBPosA])
// 				{
// //					pDec->fpReconMB[nMBPosA](pDec, nMBPos);
// 					pDec->fpReconMB[nMBPosA] = NULL;
// 				}
				nMBPosA++;	

#if ENABLE_DEBLOCK
				pDec->pCurFrame->pQuant[nYMBPos * pDec->nMBWidth + nXMBPos] = pDec->nQuant;
#endif
				*(vDecLocal.pMBQuant++) = pDec->nQuant;
			}
			vDecLocal.pMBQuant = pDec->quantrow;
		}	
	} while(nMBPosA < mbCount);
	return VO_ERR_NONE;
}


VO_U32 MPEG4DecDPPVOP( VO_MPEG4_DEC *pDec )
{
	//if no b-frames, we need only one row of mv vectors
	VO_S32 j,nMBPos,mbCount,nMBPosA,nYMBPos,nXMBPos,xpos,ypos,pos3;
	VO_MPEG4_DEC_LOCAL vDecLocal;
	VO_S32* mv;
	DP_BUFFER* DP_Buffer = pDec->DP_Buffer;
	//VO_U32 voRC = 0;
	VO_U8 mc_data[BLOCK_SIZE*BLOCK_SIZE] = {0};
	VO_S8 blockLen[6] = {0};
	VO_S8 blockscanType[6] = {0};
	idct_t blockptr[6*64] = {0};
	VO_S32 mvb[6] = {0};
	VO_S32 mvf[6] = {0};

	vDecLocal.bFieldDct = 0;
	vDecLocal.mc_data = mc_data;
	vDecLocal.blockLen = blockLen;
	vDecLocal.scanType = blockscanType;
	vDecLocal.blockptr = blockptr;
	vDecLocal.MVBack = mvb;
	vDecLocal.MVFwd = mvf;
// 	volatile FuncReconMB* fpReconMB;
// 	VO_S8 *gmcmb;

	pDec->nCurFrameMap = (pDec->nFrame - pDec->nMapOfs) << 1;
	pDec->nLastRefFrame = pDec->nFrame;
	pDec->nResyncMarkerBits = RESYNC_MARK_BITS - 1 + pDec->nFcode ;
	pDec->pCurFrameMB = pDec->pCurFrame;
	pDec->pRefFrameMB = pDec->pRefFrame;

	//Does DP support interlace??
	//if(pDec->bInterlaced)
	//	pDec->fpReconInterMB = ReconInterMBInterlace;
	//else
//	pDec->fpReconMB[0] = ReconInterMB;

	SetMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, (VO_U8*)pDec->mv_buf, 0, 4*sizeof(VO_S32)*pDec->nPosEnd);
	mbCount = pDec->nMBWidth*pDec->nMBHeight;
	ResetDPBuf(pDec, mbCount);

	nMBPosA = 0;
	nYMBPos = 0;
	nXMBPos = 0;

	do {
DPPVOPLOOP2:

		ypos = nYMBPos;
		xpos = nXMBPos;

		pos3 = ypos*MB_X+xpos;
		mv = &pDec->mv_buf[(pos3 & pDec->mv_bufmask)*4];

		for (nMBPos=nMBPosA;nMBPos<mbCount;nMBPos++,mv+=4,pos3++) {
			VO_S32 mcbpc,bNotCoded,mb_type;

			mcbpc = 0;
			if(POSX(pos3)>=pDec->nMBWidth){
				pos3 += MB_X-pDec->nMBWidth;
				mv = &pDec->mv_buf[(pos3 & pDec->mv_bufmask)*4];
			}
			UPDATE_CACHE(pDec);
			// skip stuffing
			while (SHOW_BITS(pDec,10) == 1){
				FLUSH_BITS(pDec,10);
				UPDATE_CACHE(pDec);
				if (EofBits(pDec))
					return VO_ERR_INPUT_BUFFER_SMALL;
			}

			if(SHOW_BITS(pDec,17) == MOTION_MARKER){
				break;
			}
			UPDATE_CACHE(pDec);
			if (CheckResyncMarker(pDec)){	
				VO_S32 tmp;
				if(VO_ERR_NONE != Resync(pDec, &tmp))
					return VO_ERR_DEC_MPEG4_P_FRAME;

				if(tmp != pos3){
					pos3 = tmp;
					nYMBPos = POSY(pos3);
					nXMBPos = POSX(pos3);
					nMBPos = nYMBPos * pDec->nMBWidth + nXMBPos;

					mv = &pDec->mv_buf[(pos3 & pDec->mv_bufmask)*4];
					mv += nXMBPos*4; 	
					nMBPosA = nMBPos;
				}
				UPDATE_CACHE(pDec);
			}

			bNotCoded = GetBits(pDec, 1);
			if(!bNotCoded){
				mcbpc = GetMCBPC_P(pDec); // mcbpc
				mb_type = mcbpc & 7;
				//Get Motion Vector
				switch(mb_type) {
				case INTER4V:
					for (j = 0; j < 4; j++){ 
						mv[j] = GetMV(pDec->nFcode,GetPMV(j,pos3,0,pDec),pDec, 0);
					}
					break;
				case INTER:
				case INTER_Q:
					mv[3] = mv[2] = mv[1] = mv[0] = 
						GetMV(pDec->nFcode,GetPMV(0,pos3,0,pDec),pDec, 0);
					break;
				}	
			}
			DP_Buffer->mcbpc[nMBPos]	 = (VO_S16)mcbpc;
			DP_Buffer->bNotCoded[nMBPos] = (VO_U8)bNotCoded;
		}

		//After MOTION_MARKER
		UPDATE_CACHE(pDec);

		if(GetBits(pDec,17) == MOTION_MARKER){
			DP_Buffer->nLastMBNum = nMBPos;
		}//else{
		//	if(nMBPos == mbCount){
		//		return VO_ERR_NONE;
		//	}
		//}

		for (nMBPos=nMBPosA;nMBPos<DP_Buffer->nLastMBNum;nMBPos++){ 	
			if(!DP_Buffer->bNotCoded[nMBPos]){
				VO_S32 mcbpc = DP_Buffer->mcbpc[nMBPos];
				VO_S32 mb_type = mcbpc&7;
				VO_S32 quant;

				UPDATE_CACHE(pDec);
				while (SHOW_BITS(pDec,10) == 1){
					FLUSH_BITS(pDec,10);
					UPDATE_CACHE(pDec);
					if (EofBits(pDec))
						return VO_ERR_INPUT_BUFFER_SMALL;
				}

				UPDATE_CACHE(pDec);			
				if(mb_type>=INTRA)	{
					DP_Buffer->bACPred[nMBPos] = (VO_U8)GetBits(pDec,1);
				}
				DP_Buffer->cbpy[nMBPos] = (VO_U8)GetCBPY(pDec);

				quant = pDec->nQuant;
				if(mb_type==INTER_Q||mb_type==INTRA_Q){//dequant
					VO_S32 q = GetBits(pDec,2);//
					q = quant + DQtab[q]; //DQtab[dquant]
					pDec->nQuant = quant = q<1 ? 1: (q>31 ? 31:q);
				}

				DP_Buffer->quant[nMBPos]	= (VO_U8)quant;

				if(mb_type>=INTRA&&(quant< pDec->nIntraDCThreshold)){//use_intra_dc_vlc
					GetDPDCDiff(pDec, nMBPos);
				}
			}
		}

		nMBPos = nYMBPos*MB_X + nXMBPos;
		vDecLocal.pMBQuant = pDec->quantrow + nXMBPos;
		for (;nMBPos<pDec->nPosEnd;nMBPos+=MB_X-pDec->nMBWidth) {

			VO_S32* mv = &pDec->mv_buf[(nMBPos & pDec->mv_bufmask)*4];
			for (;POSX(nMBPos)<pDec->nMBWidth;++nMBPos,mv+=4) {
// 				VO_S32 nOffset;
				nYMBPos = POSY(nMBPos);
				nXMBPos = POSX(nMBPos);	

				if(nMBPosA>=DP_Buffer->nLastMBNum&&nMBPosA<mbCount){
					if(VO_ERR_NONE != Resync(pDec, &nMBPos))
						return VO_ERR_DEC_MPEG4_P_FRAME;

					nYMBPos = POSY(nMBPos);
					nXMBPos = POSX(nMBPos);
					nMBPosA = nYMBPos * pDec->nMBWidth + nXMBPos;

					goto DPPVOPLOOP2;
				}
				if (EofBits(pDec))
					return VO_ERR_INPUT_BUFFER_SMALL;
// 				nOffset = nYMBPos * pDec->nMBWidth + nXMBPos;
// 				mvb = pDec->MVBack + 6*nOffset;
// 				mvf = pDec->MVFwd + 6*nOffset;
// 				fpReconMB = pDec->fpReconMB + nOffset;
// 				gmcmb = pDec->gmcmb + nOffset;
				/*TBD package the initial process*/
// 				nOffset = (nYMBPos << 4) * pDec->nLumEXWidth + (nXMBPos << 4);
// 				vDecLocal.pCurY = pDec->pCurFrame->y + nOffset ; 
// 				vDecLocal.pRefY = pDec->pRefFrame->y + nOffset; 
// 				nOffset = (nYMBPos << 3) * pDec->nChrEXWidth + (nXMBPos << 3);
// 				vDecLocal.pCurUV[0] = pDec->pCurFrame->u + nOffset; 
// 				vDecLocal.pCurUV[1] = pDec->pCurFrame->v + nOffset; 
// 				vDecLocal.pRefUV[0] = pDec->pRefFrame->u + nOffset; 
// 				vDecLocal.pRefUV[1] = pDec->pRefFrame->v + nOffset; 

				UPDATE_CACHE(pDec);
				//stream: 24bits available
				if (!DP_Buffer->bNotCoded[nMBPosA]) {         //if coded

					VO_S32 mcbpc = DP_Buffer->mcbpc[nMBPosA];//GetMCBPC_P(pDec); // mcbpc
					VO_S32 mb_type = mcbpc & 7;
					mcbpc = (mcbpc >> 4) & 3;

					if (mb_type >= INTRA){
						pDec->frameMap[nMBPos] = (VO_U8)pDec->nCurFrameMap;

						pDec->bACPred = DP_Buffer->bACPred[nMBPosA];//GetBits(pDec,1);
						pDec->nCBP = (DP_Buffer->cbpy[nMBPosA] << 2) | mcbpc;//(GetCBPY(pDec) << 2) | mcbpc;

						pDec->nQuant = DP_Buffer->quant[nMBPosA];
						//stream: 5bits available

						mv[3] = mv[2] = mv[1] = mv[0] = 0;
						RescuePredict(pDec, nMBPos);  //Restore AC_DC values
						if(VO_ERR_NONE != MPEG4DecIntraMB(pDec,&vDecLocal, nMBPos, nMBPosA))
							return VO_ERR_DEC_MPEG4_INTRA_MB;
						ReconIntraMB(pDec, &vDecLocal, nMBPos);

					}else{		
						pDec->frameMap[nMBPos] = (VO_U8)(pDec->nCurFrameMap|RESCUE); // set rescue needed flag

						pDec->nCBP = ((15-DP_Buffer->cbpy[nMBPosA]) << 2) | mcbpc;//((15-GetCBPY(pDec)) << 2) | mcbpc;

						//stream: 8bits available

						// we will use mv[4],mv[5] for temporary purposes
						// in the next macroblock it will be overwrite (mv incremented by 4)
						pDec->nQuant = DP_Buffer->quant[nMBPosA];
						mvb[0] = 1;
						mvf[0] = 0;
						switch (mb_type){ 
						case INTER4V:
							for (j = 0; j < 4; j++){ 
								mvb[j+1] = CorrectMV( mv[j], pDec, nXMBPos, nYMBPos);
							}
							mvb[5] = GetChromaMV4(mvb+1, pDec->qpel);
							break;
						default: //case INTER:
							mvb[4] = mvb[3] = mvb[2] = mvb[1] = CorrectMV(mv[0], pDec, nXMBPos, nYMBPos);
							mvb[5] =  GetChromaMV(mvb[1], pDec->qpel);
							break;
						}
// 						*gmcmb = 0;
// 						*fpReconMB = ReconInterMB;
						vDecLocal.gmcmb = 0;
						MPEG4DecInterMB(pDec, &vDecLocal,nMBPos);
						ReconInterMB(pDec, &vDecLocal,nMBPos);
					}
				}else {
					// not coded macroblock
					VO_S32 n = pDec->frameMap[nMBPos];
					pDec->frameMap[nMBPos] = (VO_U8)(n|RESCUE);

// 					*fpReconMB = ReconCopyMB;
//					(*fpReconMB)(pDec, &vDecLocal, nMBPos);
					ReconCopyMB(pDec, &vDecLocal, nMBPos);
// 					CopyBlock16x16(vDecLocal.pRefY, vDecLocal.pCurY, pDec->nLumEXWidth, pDec->nLumEXWidth,16);
// 					CopyBlock8x8(vDecLocal.pRefUV[0], vDecLocal.pCurUV[0], pDec->nChrEXWidth, pDec->nChrEXWidth,8);
// 					CopyBlock8x8(vDecLocal.pRefUV[1], vDecLocal.pCurUV[1], pDec->nChrEXWidth, pDec->nChrEXWidth,8);

					mv[3] = mv[2] = mv[1] = mv[0] = 0;
				}

// 				if (*fpReconMB)
// 				{
//					(*fpReconMB)(pDec, nMBPos);
// 					*fpReconMB = NULL;
// 				}
				nMBPosA++;

#if ENABLE_DEBLOCK
				pDec->pCurFrame->pQuant[nYMBPos * pDec->nMBWidth + nXMBPos] = pDec->nQuant;
#endif
				*(vDecLocal.pMBQuant++) = pDec->nQuant;
			}
			vDecLocal.pMBQuant = pDec->quantrow;
		}	
	} while(nMBPosA<mbCount);
	return VO_ERR_NONE;
}


VO_U32 MPEG4DecDPBVOP( VO_MPEG4_DEC *pDec )
{
	//not support
	return VO_ERR_NOT_IMPLEMENT;
}
#endif//ENABLE_DATA_PARTITION

#if 0
VO_U32 MPEG4DecIVOP( VO_MPEG4_DEC *pDec )
{
	VO_S32 mcbpc, nMBPos;
	VO_MPEG4_DEC_LOCAL vDecLocal;	
	VO_U32 voRC = VO_ERR_NONE;
	volatile FuncReconMB* fpReconMB;
	pDec->nLastRefFrame = pDec->nFrame;
	pDec->nMapOfs		= pDec->nFrame;

	/* set all block to current frame (and clear rescue flag)*/
	SetMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, pDec->frameMap, 0, pDec->nPosEnd);
	
	pDec->pCurFrameMB = pDec->pCurFrame;
	pDec->pRefFrameMB = pDec->pRefFrame;

	pDec->nResyncMarkerBits = RESYNC_MARK_BITS;

	for (nMBPos=0; nMBPos<pDec->nPosEnd; nMBPos+=MB_X-pDec->nMBWidth) {
		vDecLocal.pMBQuant = pDec->quantrow;
		for (;POSX(nMBPos)<pDec->nMBWidth; ++nMBPos) {
			VO_S32 nYMBPos = POSY(nMBPos), nXMBPos = POSX(nMBPos);
			VO_S32 nOffset;
			VO_S8 *bFieldDct;
			pDec->nMBError = 0;

			UPDATE_CACHE(pDec);

			/* skip stuffing*/
			while (SHOW_BITS(pDec, 9) == 1){
				FLUSH_BITS(pDec, 9);
				UPDATE_CACHE(pDec);
				if (EofBits(pDec))
					return VO_ERR_INPUT_BUFFER_SMALL;
			}
			
			if (EofBits(pDec))
				return VO_ERR_INPUT_BUFFER_SMALL;
			
			if (CheckResyncMarker(pDec)){
				if(VO_ERR_NONE != Resync(pDec, &nMBPos))
					return VO_ERR_DEC_MPEG4_I_FRAME;
				nYMBPos = POSY(nMBPos);
				nXMBPos = POSX(nMBPos);
			}
			nOffset = nYMBPos*pDec->nMBWidth + nXMBPos;
			fpReconMB = pDec->fpReconMB + nOffset;
			bFieldDct = pDec->bFieldDct + nOffset;
			*bFieldDct = 0;
// 			nOffset = (nYMBPos << 4) * pDec->nLumEXWidth + (nXMBPos << 4);
// 			vDecLocal.pCurY = pDec->pCurFrame->y + nOffset; 
// 			vDecLocal.pRefY = pDec->pRefFrame->y + nOffset; 
// 			nOffset = (nYMBPos << 3) * pDec->nChrEXWidth + (nXMBPos << 3);
// 			vDecLocal.pCurUV[0] = pDec->pCurFrame->u + nOffset; 
// 			vDecLocal.pCurUV[1] = pDec->pCurFrame->v + nOffset; 
// 			vDecLocal.pRefUV[0] = pDec->pRefFrame->u + nOffset; 
// 			vDecLocal.pRefUV[1] = pDec->pRefFrame->v + nOffset; 
	
			/*stream: 24bits available*/		
			do{
				UPDATE_CACHE(pDec);
				mcbpc = GetMCBPC_I(pDec); /* mcbpc*/
			}while(mcbpc < 0);
			
			/*stream: 15bits available*/
			pDec->bACPred = GetBits(pDec,1);
			pDec->nCBP = (GetCBPY(pDec) << 2) | ((mcbpc >> 4) & 3);

			if ((mcbpc & 7) == INTRA_Q){ //mb_type
				VO_S32 nQuant = pDec->nQuant + DQtab[GetBits(pDec,2)]; //DQtab[dquant]
				pDec->nQuant = nQuant<1 ? 1: (nQuant>31 ? 31:nQuant);
			}

			if (pDec->bInterlaced) {
				*bFieldDct = (VO_S8)GetBits(pDec, 1);
			}		
			*fpReconMB = ReconIntraMB;
			if(VO_ERR_NONE != MPEG4DecIntraMB(pDec, &vDecLocal, nMBPos, 0))//TBD merger nMBPos and dp_pos
				return VO_ERR_DEC_MPEG4_INTRA_MB;

#if ENABLE_ERROR_CONCEAL
			if((pDec->nMBError & CONCEAL_ERROR)){
				pDec->nFrameError++;
				ReconCopyMB(pDec, nMBPos);
// 				CopyBlock16x16(vDecLocal.pRefY, vDecLocal.pCurY, pDec->nLumEXWidth, pDec->nLumEXWidth,16);
// 				CopyBlock8x8(vDecLocal.pRefUV[0], vDecLocal.pCurUV[0], pDec->nChrEXWidth, pDec->nChrEXWidth,8);
// 				CopyBlock8x8(vDecLocal.pRefUV[1], vDecLocal.pCurUV[1], pDec->nChrEXWidth, pDec->nChrEXWidth,8);
			}
#endif

#if ENABLE_DEBLOCK
			pDec->pCurFrame->pQuant[nYMBPos * pDec->nMBWidth + nXMBPos] = pDec->nQuant;
#endif
			*(vDecLocal.pMBQuant++) = pDec->nQuant;
		}
	}
	return VO_ERR_NONE;
}



VO_U32 MPEG4DecPVOP( VO_MPEG4_DEC *pDec )
{
	//if no b-frames, we need only one row of mv vectors
	VO_S32 j,nMBPos, mcbpc,bNotCode;
	VO_MPEG4_DEC_LOCAL vDecLocal;
	VO_S32 *BckMV;
	VO_S32 *FwdMV;
	VO_U32 voRC = VO_ERR_NONE;
	volatile FuncReconMB* fpReconMB;
	FuncReconMB fpReconMB_i;


	pDec->nCurFrameMap = (pDec->nFrame - pDec->nMapOfs) << 1;
	pDec->nLastRefFrame = pDec->nFrame;
	pDec->nResyncMarkerBits = RESYNC_MARK_BITS - 1 + pDec->nFcode;
	pDec->pDstFrameMB = pDec->pCurFrameMB = pDec->pCurFrame;
	pDec->pRefFrameMB = pDec->pRefFrame;

	if(pDec->bInterlaced)
		fpReconMB_i = ReconInterMBInterlace;
	else
		fpReconMB_i = ReconInterMB;
	
	SetMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, (VO_U8*)pDec->mv_buf, 0, 4 * sizeof(VO_S32)*pDec->nPosEnd);

#ifdef ASP_GMC
	if(pDec->nVOPType == VO_VIDEO_FRAME_S){
		IniGMCParam(pDec->nSpriteWarpPoints,
			pDec->nSpriteWarpgmc_accuracy, pDec->warp_point,
			pDec->nWidth, pDec->nHeight, &(pDec->vGmcData));
	}
#endif//ASP_GMC


	for (nMBPos=0;nMBPos<pDec->nPosEnd;nMBPos+=MB_X-pDec->nMBWidth) {
		
		VO_S32* mv = &pDec->mv_buf[(nMBPos & pDec->mv_bufmask)*4];
		vDecLocal.pMBQuant = pDec->quantrow;		
		for (;POSX(nMBPos)<pDec->nMBWidth;++nMBPos,mv+=4) {
			VO_S32 nYMBPos = POSY(nMBPos), nXMBPos = POSX(nMBPos);	
			VO_S32 nOffset;
			VO_S8 *bFieldDct;
			VO_S8 *gmcmb;

			pDec->nMBError = 0;

			/*for interlace*/
			pDec->bFieldPred = 0; 
			pDec->bFieldForTop = 0;  
			pDec->bFieldForBottom = 0; 
			UPDATE_CACHE(pDec);

			/* skip stuffing*/
			while (SHOW_BITS(pDec,10) == 1){
				FLUSH_BITS(pDec,10);
				UPDATE_CACHE(pDec);
				if (EofBits(pDec))
					return VO_ERR_INPUT_BUFFER_SMALL;
			}
			
			if (EofBits(pDec))
				return VO_ERR_INPUT_BUFFER_SMALL;
			
			if (CheckResyncMarker(pDec)){
				if(VO_ERR_NONE != Resync(pDec, &nMBPos))
					return VO_ERR_DEC_MPEG4_P_FRAME;
				nYMBPos = POSY(nMBPos);
				nXMBPos = POSX(nMBPos);
			}
			nOffset = nYMBPos*pDec->nMBWidth + nXMBPos;
			bFieldDct = pDec->bFieldDct + nOffset;
			fpReconMB = pDec->fpReconMB + nOffset;
			gmcmb = pDec->gmcmb + nOffset;
			*bFieldDct = 0;
			BckMV = pDec->MVBack + 6*nOffset;
			FwdMV = pDec->MVFwd + 6*nOffset;

			//if(pDec->nFrame == 3031 && nXMBPos == 5 && nYMBPos == 1)
			//	pDec->nFrame = pDec->nFrame;
			
			/*TBD package the initial process*/
// 			nOffset = (nYMBPos << 4) * pDec->nLumEXWidth + (nXMBPos << 4);
// 			vDecLocal.pDstY = vDecLocal.pCurY = pDec->pCurFrame->y + nOffset; 
// 			vDecLocal.pRefY = pDec->pRefFrame->y + nOffset; 
// 			nOffset = (nYMBPos << 3) * pDec->nChrEXWidth + (nXMBPos << 3);
// 			vDecLocal.pDstUV[0] = vDecLocal.pCurUV[0] = pDec->pCurFrame->u + nOffset; 
// 			vDecLocal.pDstUV[1] = vDecLocal.pCurUV[1] = pDec->pCurFrame->v + nOffset; 
// 			vDecLocal.pRefUV[0] = pDec->pRefFrame->u + nOffset; 
// 			vDecLocal.pRefUV[1] = pDec->pRefFrame->v + nOffset; 
				
			do{/*stuffing mb type*/
				UPDATE_CACHE(pDec);
				if(bNotCode = GetBits(pDec, 1)){
					break;
				}
				mcbpc = GetMCBPC_P(pDec); /* mcbpc */
			}while(mcbpc < 0);
			/*stream: 24bits available*/

			if (!bNotCode) {         //if coded
				VO_S32 nMBType = mcbpc & 7;
				mcbpc = (mcbpc >> 4) & 3;
				
				//stream: 14bits available
				if (nMBType >= INTRA){
					pDec->frameMap[nMBPos] = (VO_U8)pDec->nCurFrameMap;
					pDec->bACPred = GetBits(pDec,1);
					pDec->nCBP = (GetCBPY(pDec) << 2) | mcbpc;

					//stream: 7bits available
					if (nMBType == INTRA_Q){
						VO_S32 q = pDec->nQuant + DQtab[GetBits(pDec,2)]; //DQtab[dquant]
						pDec->nQuant = q<1 ? 1: (q>31 ? 31:q);
					}
					
					if (pDec->bInterlaced){
						*bFieldDct = (VO_S8)GetBits(pDec, 1);
					}

					//stream: 5bits available
					mv[3] = mv[2] = mv[1] = mv[0] = 0;
					
					RescuePredict(pDec, nMBPos);  //Restore AC_DC values
					*fpReconMB = ReconIntraMB;
					if(VO_ERR_NONE != MPEG4DecIntraMB(pDec, &vDecLocal, nMBPos, 0))
						return VO_ERR_DEC_MPEG4_INTRA_MB;

				}else{
					VO_S32 bMCSel = 0;
					pDec->frameMap[nMBPos] = (VO_U8)(pDec->nCurFrameMap|RESCUE); // set rescue needed flag	
					if ((pDec->nVOPType == VO_VIDEO_FRAME_S) && (nMBType == INTER || nMBType == INTER_Q))
						bMCSel = GetBits(pDec, 1);
					
					pDec->nCBP = ((15-GetCBPY(pDec)) << 2) | mcbpc;

					if (nMBType == INTER_Q){
						VO_S32 q = pDec->nQuant + DQtab[GetBits(pDec,2)]; //DQtab[dquant]
						pDec->nQuant = q<1 ? 1: (q>31 ? 31:q);
					}

					if (pDec->bInterlaced) {
						*fpReconMB = ReconInterMBInterlace;
						if (pDec->nCBP) {
							*bFieldDct = (VO_S8)GetBits(pDec, 1);
						}

						if ((nMBType == INTER || nMBType == INTER_Q) && !bMCSel) {
							pDec->bFieldPred = (VO_S8)GetBits(pDec, 1);
							if (pDec->bFieldPred) {
								pDec->bFieldForTop = (VO_S8)GetBits(pDec, 1);
								pDec->bFieldForBottom = (VO_S8)GetBits(pDec, 1);
							}
						}
					}
					else
					{
						*fpReconMB = ReconInterMB;
					}
#ifdef ASP_GMC
					if (bMCSel) {
						MPEG4DecGmcMB(pDec, &vDecLocal, nMBPos, mv);
						continue;
					}
#endif//#ifdef ASP_GMC				
					//stream: 8bits available
					
					// we will use mv[4],mv[5] for temporary purposes
					// in the next macroblock it will be overwrite (mv incremented by 4)
					BckMV[0] = 1;
					FwdMV[0] = 0;
					switch (nMBType) 
					{
					case INTER4V:
						
						for (j = 0; j < 4; j++){ 
							mv[j] = GetMV(pDec->nFcode,GetPMV(j,nMBPos,0,pDec),pDec, nXMBPos, nYMBPos);
							BckMV[j+1] = CorrectMV(mv[j], pDec, nXMBPos, nYMBPos);
						}
						
						BckMV[5] = GetChromaMV4(BckMV+1, pDec->qpel);

						break;
//					case INTER_Q:
//						{
//							VO_S32 q = pDec->nQuant + DQtab[GetBits(pDec,2)]; //DQtab[dquant]
//							pDec->nQuant = q<1 ? 1: (q>31 ? 31:q);
//						}
					default: //case INTER:

						if (pDec->bInterlaced && pDec->bFieldPred) {
							mv[1] = mv[0] = 
								GetMV(pDec->nFcode,GetPMV(0,nMBPos,0,pDec),pDec, nXMBPos, nYMBPos);
							BckMV[2] = BckMV[1] = CorrectMV(mv[0], pDec, nXMBPos, nYMBPos);

							mv[3] = mv[2] = 
								GetMV(pDec->nFcode,GetPMV(0,nMBPos,0,pDec),pDec, nXMBPos, nYMBPos);
							BckMV[4] = BckMV[3] = CorrectMV(mv[2], pDec, nXMBPos, nYMBPos);

							BckMV[5] = GetChromaMV(BckMV[1], pDec->qpel);//TBD
						} else {
							mv[3] = mv[2] = mv[1] = mv[0] = 
								GetMV(pDec->nFcode,GetPMV(0,nMBPos,0,pDec),pDec, nXMBPos, nYMBPos);

							BckMV[4] = BckMV[3] = BckMV[2] = BckMV[1] = CorrectMV(mv[0], pDec, nXMBPos, nYMBPos);

							BckMV[5] = GetChromaMV(BckMV[1], pDec->qpel);
						}
						break;
					}
					*gmcmb = 0;
					MPEG4DecInterMB(pDec, &vDecLocal, nMBPos);
				}
			}
#ifdef ASP_GMC
			else if(pDec->nVOPType == VO_VIDEO_FRAME_S){
				pDec->frameMap[nMBPos] = (VO_U8)(pDec->nCurFrameMap|RESCUE); // set rescue needed flag	
				pDec->nCBP = 0;
				*fpReconMB = fpReconMB_i;
				MPEG4DecGmcMB(pDec, &vDecLocal, nMBPos, mv);
			}
#endif //#ifdef ASP_GMC
			else{
				// not coded macroblock
				VO_S32 n = pDec->frameMap[nMBPos];

				pDec->frameMap[nMBPos] = (VO_U8)(n|RESCUE);
				mv[3] = mv[2] = mv[1] = mv[0] = 0;
				*fpReconMB = ReconCopyMB;
				(*fpReconMB)(pDec, nMBPos);
// 				CopyBlock16x16(vDecLocal.pRefY, vDecLocal.pCurY, pDec->nLumEXWidth, pDec->nLumEXWidth,16);
// 				CopyBlock8x8(vDecLocal.pRefUV[0], vDecLocal.pCurUV[0], pDec->nChrEXWidth, pDec->nChrEXWidth,8);
// 				CopyBlock8x8(vDecLocal.pRefUV[1], vDecLocal.pCurUV[1], pDec->nChrEXWidth, pDec->nChrEXWidth,8);
							
			}
#if ENABLE_ERROR_CONCEAL
			if(pDec->nMBError){
				pDec->nFrameError++;
//				if(pDec->nMBError& CONCEAL_ERROR){
//					CopyBlock16x16(vDecLocal.pRefY, vDecLocal.pCurY, pDec->nLumEXWidth, pDec->nLumEXWidth,16);
//					CopyBlock8x8(vDecLocal.pRefUV[0], vDecLocal.pCurUV[0], pDec->nChrEXWidth, pDec->nChrEXWidth,8);
//					CopyBlock8x8(vDecLocal.pRefUV[1], vDecLocal.pCurUV[1], pDec->nChrEXWidth, pDec->nChrEXWidth,8);
//				}
			}
#endif
#if ENABLE_DEBLOCK
			pDec->pCurFrame->pQuant[nYMBPos * pDec->nMBWidth + nXMBPos] = pDec->nQuant;
#endif
			*(vDecLocal.pMBQuant++) = pDec->nQuant;
#if 0
			if(pDec->nFrame == 4008 && nYMBPos == 0&&nXMBPos == 17){
				memset(vDecLocal.pCurY, 0, 16);
				memset(vDecLocal.pCurY + pDec->nLumEXWidth, 0, 16);
				memset(vDecLocal.pCurY+ 2*pDec->nLumEXWidth,0, 16);
				memset(vDecLocal.pCurY+ 3*pDec->nLumEXWidth, 0, 16);
				memset(vDecLocal.pRefUV[0], 0, 8);
				memset(vDecLocal.pRefUV[0] + pDec->nChrEXWidth, 0, 8);
				memset(vDecLocal.pRefUV[1], 0, 8);
				memset(vDecLocal.pRefUV[1] + pDec->nChrEXWidth, 0, 8);
			}
#endif
		}
	}

	return VO_ERR_NONE;
}
#endif

#ifdef ASP_BVOP
#if 0
VO_U32 MPEG4DecBVOP( VO_MPEG4_DEC *pDec )
{
	VO_S32 *FwdMV;
	VO_S32 *BckMV;
	VO_S32 nMBPos;
	volatile FuncReconMB* fpReconMB;
	FuncReconMB fpReconMB_i;
	VO_MPEG4_DEC_LOCAL vDecLocal;

	if(pDec->bInterlaced)
		fpReconMB_i = ReconInterMBInterlace;
	else
		fpReconMB_i = ReconInterMB_B;

	pDec->nResyncMarkerBits = RESYNC_MARK_BITS +
		(pDec->nBcode > pDec->nFcode ? pDec->nBcode : pDec->nFcode);

	pDec->pCurFrameMB = pDec->pCurFrame;
	pDec->pRefFrameMB = pDec->pRefFrame;
	pDec->pDstFrameMB = pDec->pOutFrame;
	
	for (nMBPos=0;nMBPos<pDec->nPosEnd;nMBPos+=MB_X-pDec->nMBWidth) {
		
		VO_S32 n;
		VO_S32 fprev = 0, bprev = 0;
		vDecLocal.pMBQuant = pDec->quantrow;		
		for (;POSX(nMBPos)<pDec->nMBWidth;++nMBPos) {
			VO_S32 nYMBPos = POSY(nMBPos), nXMBPos = POSX(nMBPos);
			VO_S32 nOffset;
			VO_S8 *bFieldDct;
			VO_S8 *gmcmb;

			/*for interlace*/
			pDec->bFieldPred = 0; 
			pDec->bFieldForTop = 0;  
			pDec->bFieldForBottom = 0; 

			UPDATE_CACHE(pDec);

			if (EofBits(pDec)){
				return  VO_ERR_INPUT_BUFFER_SMALL;
			}
			
			if (CheckResyncMarker(pDec)){
				if(VO_ERR_NONE != Resync(pDec, &nMBPos))
					return VO_ERR_DEC_MPEG4_B_FRAME;
				nYMBPos = POSY(nMBPos);
				nXMBPos = POSX(nMBPos);

				fprev = bprev = 0;
			}
			nOffset = nYMBPos*pDec->nMBWidth + nXMBPos;
			fpReconMB = pDec->fpReconMB + nOffset;
			bFieldDct = pDec->bFieldDct + nOffset;
			gmcmb = pDec->gmcmb + nOffset;
			*bFieldDct = 0;
			BckMV = pDec->MVBack + 6*nOffset;
			FwdMV = pDec->MVFwd + 6*nOffset;

			n = pDec->nMapOfs+(pDec->frameMap[nMBPos] >> 1);

			//if(pDec->nFrame == 21 && nXMBPos == 26 && nYMBPos == 0)
			//	pDec->nFrame = pDec->nFrame;

// 			nOffset = (nYMBPos << 4) * pDec->nLumEXWidth + (nXMBPos << 4);
// 			vDecLocal.pCurY = pDec->pCurFrame->y + nOffset; 
// 			vDecLocal.pRefY = pDec->pRefFrame->y + nOffset; 
// 			vDecLocal.pDstY = pDec->pOutFrame->y + nOffset;
// 			nOffset = (nYMBPos << 3) * pDec->nChrEXWidth + (nXMBPos << 3);
// 			vDecLocal.pCurUV[0] = pDec->pCurFrame->u + nOffset; 
// 			vDecLocal.pCurUV[1] = pDec->pCurFrame->v + nOffset; 
// 			vDecLocal.pRefUV[0] = pDec->pRefFrame->u + nOffset; 
// 			vDecLocal.pRefUV[1] = pDec->pRefFrame->v + nOffset; 
// 			vDecLocal.pDstUV[0] = pDec->pOutFrame->u + nOffset;
// 			vDecLocal.pDstUV[1] = pDec->pOutFrame->v + nOffset;

			pDec->nMBError = 0;
			
			// did the last refframe touch this block?
			if (pDec->nLastRefFrame > n){
// 				vDecLocal.pRefY = vDecLocal.pCurY;
// 				vDecLocal.pRefUV[0] = vDecLocal.pCurUV[0];
// 				vDecLocal.pRefUV[1] = vDecLocal.pCurUV[1];
// 				vDecLocal.pCurY = vDecLocal.pDstY;
// 				vDecLocal.pCurUV[0] = vDecLocal.pDstUV[0];
// 				vDecLocal.pCurUV[1] = vDecLocal.pDstUV[1];

				pDec->pRefFrameMB = pDec->pCurFrame;
				pDec->pCurFrameMB = pDec->pOutFrame;
				*fpReconMB = ReconCopyMB;
				(*fpReconMB)(pDec, nMBPos);
// 				CopyBlock16x16(vDecLocal.pCurY,vDecLocal.pDstY , pDec->nLumEXWidth, pDec->nLumEXWidth,16);
// 				CopyBlock8x8(vDecLocal.pCurUV[0], vDecLocal.pDstUV[0], pDec->nChrEXWidth, pDec->nChrEXWidth,8);
// 				CopyBlock8x8(vDecLocal.pCurUV[1], vDecLocal.pDstUV[1], pDec->nChrEXWidth, pDec->nChrEXWidth,8);
			}else{
				*fpReconMB = fpReconMB_i;
				*gmcmb = 0;
				//stream: 24bits available
				if (!GetBits(pDec, 1)){
					VO_S32 mb_type;
					n = GetBits(pDec,1);
		
					for (mb_type=0; mb_type<=3; ++mb_type)
						if (GetBits(pDec, 1))
							break;
						
						//stream: 19bits available
						if (!n)
							pDec->nCBP = GetBits(pDec,6);
						else
							pDec->nCBP = 0;

						if (mb_type != DIRECT){
							if (pDec->nCBP && GetBits(pDec, 1)){
								VO_S32 q = pDec->nQuant + (GetBits(pDec, 1) ? 2:-2);
								pDec->nQuant = q<1 ? 1: (q>31 ? 31:q);
							}
						}
						if (pDec->bInterlaced) {
							if (pDec->nCBP) {
								*bFieldDct = (VO_S8)GetBits(pDec, 1);
							}

							if (mb_type) {
								pDec->bFieldPred = (VO_S8)GetBits(pDec, 1);

								if (pDec->bFieldPred) {
									pDec->bFieldForTop = (VO_S8)GetBits(pDec, 1);
									pDec->bFieldForBottom = (VO_S8)GetBits(pDec, 1);
								}
							}
						}		
				
						//stream: 13bits available
						if (mb_type != DIRECT){

							switch (mb_type){ 
							case BACKWARD:
								BckMV[0] = 1;
								FwdMV[0] = 0;
								bprev = GetMV( pDec->nBcode, bprev, pDec , nXMBPos, nYMBPos);
								BckMV[4]=BckMV[3]=BckMV[2]=BckMV[1] = CorrectMV( bprev, pDec , nXMBPos, nYMBPos);
								BckMV[5]=GetChromaMV(BckMV[1], pDec->qpel);

								break;
								
							case FORWARD:
								BckMV[0] = 0;
								FwdMV[0] = 1;
								fprev = GetMV( pDec->nFcode, fprev, pDec , nXMBPos, nYMBPos);
								FwdMV[4]=FwdMV[3]=FwdMV[2]=FwdMV[1] = CorrectMV(fprev, pDec , nXMBPos, nYMBPos);

								FwdMV[5]=GetChromaMV(FwdMV[1], pDec->qpel);

								break;
								
							default: //case INTERPOLATE:
								BckMV[0] = 1;
								FwdMV[0] = 1;
								fprev = GetMV( pDec->nFcode, fprev, pDec, nXMBPos, nYMBPos );
								FwdMV[4]=FwdMV[3]=FwdMV[2]=FwdMV[1] = CorrectMV(fprev, pDec , nXMBPos, nYMBPos);
								FwdMV[5]=GetChromaMV(FwdMV[1], pDec->qpel);
								bprev = GetMV( pDec->nBcode, bprev, pDec , nXMBPos, nYMBPos);
								BckMV[4]=BckMV[3]=BckMV[2]=BckMV[1] = CorrectMV(bprev, pDec , nXMBPos, nYMBPos);
								BckMV[5]=GetChromaMV(BckMV[1], pDec->qpel);

								break;
							}
							MPEG4DecInterMB(pDec, &vDecLocal, nMBPos);

						}else{
							MPEG4DecDirectMB(pDec,nMBPos,GetMV( 1, 0, pDec,nXMBPos, nYMBPos), &vDecLocal);
							//TBD
							MPEG4DecInterMB(pDec, &vDecLocal, nMBPos);

						}
				}else { 
					pDec->nCBP = 0;
					MPEG4DecDirectMB(pDec,nMBPos,0, &vDecLocal);
					//TBD
					MPEG4DecInterMB(pDec, &vDecLocal, nMBPos);
				}
#if ENABLE_ERROR_CONCEAL
				if(pDec->nMBError){
					pDec->nFrameError++;
					//				if(pDec->nMBError& CONCEAL_ERROR){
					//					CopyBlock16x16(vDecLocal.pRefY, vDecLocal.pCurY, pDec->nLumEXWidth, pDec->nLumEXWidth,16);
					//					CopyBlock8x8(vDecLocal.pRefUV[0], vDecLocal.pCurUV[0], pDec->nChrEXWidth, pDec->nChrEXWidth,8);
					//					CopyBlock8x8(vDecLocal.pRefUV[1], vDecLocal.pCurUV[1], pDec->nChrEXWidth, pDec->nChrEXWidth,8);
					//				}
				}
#endif
			}

#if ENABLE_DEBLOCK
			pDec->pOutFrame->pQuant[nYMBPos * pDec->nMBWidth + nXMBPos] = pDec->nQuant;
#endif
			*(vDecLocal.pMBQuant++) = pDec->nQuant;
		}
	}

	return VO_ERR_NONE;
}


#endif


VO_U32 MPEG4DecVOP( VO_MPEG4_DEC *pDec )
{
// 	VO_U32 voRC = VO_ERR_NONE;
	VO_MPEG4_DEC_LOCAL vDecLocal;

	vDecLocal.nMBPos = 0;
	vDecLocal.pMBQuant = pDec->quantrow;

	pDec->pDstFrameMB = pDec->pCurFrameMB = pDec->pCurFrame;
	pDec->pRefFrameMB = pDec->pRefFrame;
	switch(pDec->nVOPType)
	{
	case VO_VIDEO_FRAME_I:
		pDec->nLastRefFrame = pDec->nFrame;
		pDec->nMapOfs		= pDec->nFrame;
		/* set all block to current frame (and clear rescue flag)*/
		SetMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, pDec->frameMap, 0, pDec->nPosEnd);
		SetMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, (VO_U8*)pDec->mv_buf, 0, 4 * sizeof(VO_S32)*pDec->nPosEnd);
		pDec->nResyncMarkerBits = RESYNC_MARK_BITS;
		vDecLocal.voERR = VO_ERR_DEC_MPEG4_I_FRAME;
		break;
#ifdef ASP_GMC
	case VO_VIDEO_FRAME_S:
		IniGMCParam(pDec->nSpriteWarpPoints,
			pDec->nSpriteWarpgmc_accuracy, pDec->warp_point,
			pDec->nTrueWidth, pDec->nTrueHeight, &(pDec->vGmcData));
		//no break, go on
#endif//ASP_GMC
	case VO_VIDEO_FRAME_P:
		pDec->nCurFrameMap = (pDec->nFrame - pDec->nMapOfs) << 1;
		pDec->nLastRefFrame = pDec->nFrame;
		pDec->nResyncMarkerBits = RESYNC_MARK_BITS - 1 + pDec->nFcode;
		SetMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, (VO_U8*)pDec->mv_buf, 0, 4 * sizeof(VO_S32)*pDec->nPosEnd);
		vDecLocal.voERR = VO_ERR_DEC_MPEG4_P_FRAME;
		break;
	case VO_VIDEO_FRAME_B:
		pDec->nResyncMarkerBits = RESYNC_MARK_BITS +
			(pDec->nBcode > pDec->nFcode ? pDec->nBcode : pDec->nFcode);
		pDec->pDstFrameMB = pDec->pOutFrame;
		vDecLocal.fprev0 = 0;
		vDecLocal.fprev1 = 0;
		vDecLocal.bprev0 = 0;
		vDecLocal.bprev1 = 0;
		vDecLocal.voERR = VO_ERR_DEC_MPEG4_B_FRAME;
		break;
	default:
		return VO_ERR_DEC_MPEG4_HEADER;

	}

	if(pDec->bInterlaced)
	{
		if(pDec->nVOPType != VO_VIDEO_FRAME_B)
			SetMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, (VO_U8*)pDec->bFieldPred_buf, 0, sizeof(VO_S8)*pDec->nPosEnd);
		vDecLocal.fpReconMB_i = ReconInterMBInterlace;
	}
	else if(pDec->nVOPType != VO_VIDEO_FRAME_B)
		vDecLocal.fpReconMB_i = ReconInterMB;
	else
		vDecLocal.fpReconMB_i = ReconInterMB_B;

// #if ENABLE_MULTI_THREAD

		//start MB serials, init/reset all sub-threads if needed
// #if USE_WHILE_FORVER
// 	pDec->nParser = 1;
// #else
		pthread_mutex_lock(&(pDec->parserMutex));
		pDec->nParser = 1;
		pthread_mutex_unlock(&(pDec->parserMutex));
// #endif
		pDec->nParserErr = VO_ERR_NONE;
		SetMBThreadVOPFunc(pDec, MBrMPEG4VOP);

// #endif
	MPEG4DecMBRow(pDec, &vDecLocal, &pDec->vopCtxt);
		//wait all sub-threads finish
// 		printf("waiting\n");
#if USE_WHILE_FORVER
		WaitForSubThdFinish(pDec);
#else
		pthread_mutex_lock(&(pDec->finishMutex));
		pDec->bEndVop = 1;
		while (pDec->nFinish < (VO_S32)pDec->nMpeg4Thd-1)
		{
//			thread_sleep(0);
			pthread_cond_wait(&(pDec->finishCond), &(pDec->finishMutex));
		}
		pDec->nFinish = 0;
		pDec->bEndVop = 0;
		pthread_mutex_unlock(&(pDec->finishMutex));
#endif
//  	printf("end frame\n");
		return pDec->nParserErr;
// #endif
}

VO_U32 MPEG4DecVOP_s( VO_MPEG4_DEC *pDec )
{
//	VO_U8 mc_data[64];
	VO_S8 scanType[6];
// 	VO_S16 block[6*64] = {0};
	VO_S8 blockLen[6];
	VO_S32 BckMV[6];
	VO_S32 FwdMV[6];
	VO_MPEG4_DEC_LOCAL vDecLocal = 
	{
		pDec->vopCtxt.mc_data,
		0,
		0,
		scanType,
		pDec->vopCtxt.blockptr,
		blockLen,
		BckMV,
		FwdMV,
		NULL
	};
// 	VO_S32 *BckMV = vDecLocal.MVBack;
// 	VO_S32 *FwdMV = vDecLocal.MVFwd;

// 	VO_S32 *bFieldDct = &vDecLocal.bFieldDct;
	VO_S32 *gmcmb = &vDecLocal.gmcmb;
	FuncReconMB fpReconMB;
	VO_S32 j,nMBPos, mcbpc,bNotCode;
// 	VO_U32 voRC = VO_ERR_NONE;
	VO_U32 voERR;
	FuncReconMB fpReconMB_i;

// 	vDecLocal.MVBack = BckMV;
// 	vDecLocal.MVFwd = FwdMV;
// 	vDecLocal.scanType = scanType;
// 	vDecLocal.blockLen = blockLen;
// 	vDecLocal.mc_data = pDec->vopCtxt.mc_data;
// 	vDecLocal.blockptr = pDec->vopCtxt.blockptr;

	pDec->pDstFrameMB = pDec->pCurFrameMB = pDec->pCurFrame;
	pDec->pRefFrameMB = pDec->pRefFrame;
	switch(pDec->nVOPType)
	{
	case VO_VIDEO_FRAME_I:
		pDec->nLastRefFrame = pDec->nFrame;
		pDec->nMapOfs		= pDec->nFrame;
		/* set all block to current frame (and clear rescue flag)*/
		SetMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, pDec->frameMap, 0, pDec->nPosEnd);
		SetMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, (VO_U8*)pDec->mv_buf, 0, 4 * sizeof(VO_S32)*pDec->nPosEnd);
		pDec->nResyncMarkerBits = RESYNC_MARK_BITS;
		voERR = VO_ERR_DEC_MPEG4_I_FRAME;
		break;
#ifdef ASP_GMC
	case VO_VIDEO_FRAME_S:
		IniGMCParam(pDec->nSpriteWarpPoints,
			pDec->nSpriteWarpgmc_accuracy, pDec->warp_point,
			pDec->nTrueWidth, pDec->nTrueHeight, &(pDec->vGmcData));
		//no break, go on
#endif//ASP_GMC
	case VO_VIDEO_FRAME_P:
		pDec->nCurFrameMap = (pDec->nFrame - pDec->nMapOfs) << 1;
		pDec->nLastRefFrame = pDec->nFrame;
		pDec->nResyncMarkerBits = RESYNC_MARK_BITS - 1 + pDec->nFcode;
		SetMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, (VO_U8*)pDec->mv_buf, 0, 4 * sizeof(VO_S32)*pDec->nPosEnd);
		voERR = VO_ERR_DEC_MPEG4_P_FRAME;
		break;
	case VO_VIDEO_FRAME_B:
		pDec->nResyncMarkerBits = RESYNC_MARK_BITS  +
			(pDec->nBcode > pDec->nFcode ? pDec->nBcode : pDec->nFcode);
		pDec->pDstFrameMB = pDec->pOutFrame;
		voERR = VO_ERR_DEC_MPEG4_B_FRAME;
		break;

	default:
		return VO_ERR_DEC_MPEG4_HEADER;
	}

	if(pDec->bInterlaced)
	{
		if(pDec->nVOPType != VO_VIDEO_FRAME_B)
			SetMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, (VO_U8*)pDec->bFieldPred_buf, 0, sizeof(VO_S8)*pDec->nPosEnd);
		fpReconMB_i = ReconInterMBInterlace;
	}
	else if(pDec->nVOPType != VO_VIDEO_FRAME_B)
		fpReconMB_i = ReconInterMB;
	else
		fpReconMB_i = ReconInterMB_B;

	for (nMBPos=0;nMBPos<pDec->nPosEnd;nMBPos+=MB_X-pDec->nMBWidth) {

		VO_S32 n;
		VO_S32 fprev0 = 0, fprev1 = 0;
		VO_S32 bprev0 = 0, bprev1 = 0;
		VO_S32* mv = &pDec->mv_buf[(nMBPos & pDec->mv_bufmask)*4];
		vDecLocal.pMBQuant = pDec->quantrow;		
		for (;POSX(nMBPos)<pDec->nMBWidth;++nMBPos,mv+=4) {
			VO_S32 nYMBPos = POSY(nMBPos), nXMBPos = POSX(nMBPos);
			VO_S32 mb_type;
			pDec->nMBError = 0;

			/*for interlace*/
// 			pDec->bFieldPred_buf[nMBPos] = 0; 
			UPDATE_CACHE(pDec);

			/* skip stuffing*/
			switch(pDec->nVOPType)
			{
			case VO_VIDEO_FRAME_B:
				// did the last refframe touch this block?
				if (pDec->nLastRefFrame > pDec->nMapOfs+(pDec->frameMap[nMBPos] >> 1)){
					fpReconMB = ReconCopyMB_B;
					goto BframeMC;
				}
				break;
			case VO_VIDEO_FRAME_I:
				while (SHOW_BITS(pDec, 9) == 1){
					FLUSH_BITS(pDec, 9);
					UPDATE_CACHE(pDec);
					if (EofBits(pDec))
					{
						return VO_ERR_INPUT_BUFFER_SMALL;
					}
				}
				break;
			default://P & S frame
				while (SHOW_BITS(pDec,10) == 1){
					FLUSH_BITS(pDec,10);
					UPDATE_CACHE(pDec);
					if (EofBits(pDec))
					{
						return VO_ERR_INPUT_BUFFER_SMALL;
					}
				}
			}

			if (EofBits(pDec))
			{
				return VO_ERR_INPUT_BUFFER_SMALL;
// 				return VO_ERR_NONE;
			}

			if (CheckResyncMarker(pDec)){
				if(VO_ERR_NONE != Resync(pDec, &nMBPos))
				{
					return voERR;
				}
				nYMBPos = POSY(nMBPos);
				nXMBPos = POSX(nMBPos);

				if (pDec->nVOPType == VO_VIDEO_FRAME_B)
				{
					bprev1 = bprev0 = fprev1 = fprev0 = 0;
				}
				//mv & pMBQuant update, ----by harry, 2011,11,8
				mv = &pDec->mv_buf[(nMBPos & pDec->mv_bufmask)*4];
				vDecLocal.pMBQuant = pDec->quantrow + nXMBPos;
			}
			vDecLocal.bFieldDct = 0;
			/*TBD package the initial process*/

			switch(pDec->nVOPType)
			{
			case VO_VIDEO_FRAME_B:
				// did the last refframe touch this block?
				if (pDec->nLastRefFrame > pDec->nMapOfs+(pDec->frameMap[nMBPos] >> 1)){
					fpReconMB = ReconCopyMB_B;
					goto BframeMC;
				}
				fpReconMB = fpReconMB_i;
				//stream: 24bits available
				if (GetBits(pDec, 1)){
					pDec->nCBP = 0;
					MPEG4DecDirectMB(pDec,nMBPos,0, &vDecLocal);
					//TBD
					*gmcmb = 0;
					MPEG4DecInterMB(pDec, &vDecLocal, nMBPos);
					break;
				}
				n = GetBits(pDec,1);

				for (mb_type=0; mb_type<=3; ++mb_type)
					if (GetBits(pDec, 1))
						break;

				//stream: 19bits available
				if (!n)
					pDec->nCBP = GetBits(pDec,6);
				else
					pDec->nCBP = 0;

				if (mb_type != DIRECT){
					if (pDec->nCBP && GetBits(pDec, 1)){
						VO_S32 q = pDec->nQuant + (GetBits(pDec, 1) ? 2:-2);
						pDec->nQuant = q<1 ? 1: (q>31 ? 31:q);
					}
				}
				if (pDec->bInterlaced) {
					if (pDec->nCBP) {
						vDecLocal.bFieldDct = GetBits(pDec, 1);
					}

					if (mb_type) {

						if (GetBits(pDec, 1)) {
							pDec->bFieldPred_buf[nMBPos] = 1;
							if (mb_type != BACKWARD)
							{
								FwdMV[0] = GetBits(pDec, 2);
							}
							if (mb_type != FORWARD)
							{
								BckMV[0] = GetBits(pDec, 2);
							}
						}
					}
					else
					{
						//direct field info update
						//mb_type = mb_type;
					}
				}

				//stream: 13bits available	
				switch (mb_type){ 
				case DIRECT:
					MPEG4DecDirectMB(pDec,nMBPos,GetMV( 1, 0, pDec,0), &vDecLocal);
					break;
				case BACKWARD:
					BckMV[0] |= 1<<31;
					FwdMV[0] = 0;
					if (!pDec->bInterlaced || !pDec->bFieldPred_buf[nMBPos])
					{
						bprev1 = bprev0 = GetMV( pDec->nBcode, bprev0, pDec , 0);
						BckMV[4]=BckMV[3]=BckMV[2]=BckMV[1] = CorrectMV( bprev0, pDec , nXMBPos, nYMBPos);
						BckMV[5]=GetChromaMV(BckMV[1], pDec->qpel);
					}
					else
					{
						bprev0 = GetMV( pDec->nBcode, bprev0, pDec , 1);
						BckMV[2]=BckMV[1] = CorrectMV( bprev0, pDec , nXMBPos, nYMBPos);

						bprev1 = GetMV( pDec->nBcode, bprev1, pDec , 1);
						BckMV[4]=BckMV[3] = CorrectMV( bprev1, pDec , nXMBPos, nYMBPos);
						//calc chroma mv later(in mc)
					}

					break;

				case FORWARD:
					BckMV[0] = 0;
					FwdMV[0] |= 1<<31;
					if (!pDec->bInterlaced || !pDec->bFieldPred_buf[nMBPos])
					{
						fprev1 = fprev0 = GetMV( pDec->nFcode, fprev0, pDec , 0);
						FwdMV[4]=FwdMV[3]=FwdMV[2]=FwdMV[1] = CorrectMV(fprev0, pDec , nXMBPos, nYMBPos);

						FwdMV[5]=GetChromaMV(FwdMV[1], pDec->qpel);
					}
					else
					{
						fprev0 = GetMV( pDec->nBcode, fprev0, pDec , 1);
						FwdMV[2]=FwdMV[1] = CorrectMV( fprev0, pDec , nXMBPos, nYMBPos);

						fprev1 = GetMV( pDec->nBcode, fprev1, pDec , 1);
						FwdMV[4]=FwdMV[3] = CorrectMV( fprev1, pDec , nXMBPos, nYMBPos);
						//calc chroma mv later(in mc)
					}

					break;

				default: //case INTERPOLATE:
					BckMV[0] |= 1<<31;
					FwdMV[0] |= 1<<31;
					if (!pDec->bInterlaced || !pDec->bFieldPred_buf[nMBPos])
					{
						fprev1 = fprev0 = GetMV( pDec->nFcode, fprev0, pDec, 0 );
						FwdMV[4]=FwdMV[3]=FwdMV[2]=FwdMV[1] = CorrectMV(fprev0, pDec , nXMBPos, nYMBPos);
						FwdMV[5]=GetChromaMV(FwdMV[1], pDec->qpel);
						bprev1 = bprev0 = GetMV( pDec->nBcode, bprev0, pDec , 0);
						BckMV[4]=BckMV[3]=BckMV[2]=BckMV[1] = CorrectMV(bprev0, pDec , nXMBPos, nYMBPos);
						BckMV[5]=GetChromaMV(BckMV[1], pDec->qpel);
					}
					else
					{
						fprev0 = GetMV( pDec->nFcode, fprev0, pDec, 1 );
						FwdMV[2]=FwdMV[1] = CorrectMV(fprev0, pDec , nXMBPos, nYMBPos);
						fprev1 = GetMV( pDec->nFcode, fprev1, pDec, 1 );
						FwdMV[4]=FwdMV[3] = CorrectMV(fprev1, pDec , nXMBPos, nYMBPos);
						bprev0 = GetMV( pDec->nBcode, bprev0, pDec , 1);
						BckMV[2]=BckMV[1] = CorrectMV(bprev0, pDec , nXMBPos, nYMBPos);
						bprev1 = GetMV( pDec->nBcode, bprev1, pDec , 1);
						BckMV[4]=BckMV[3] = CorrectMV(bprev1, pDec , nXMBPos, nYMBPos);
					}
				}

				*gmcmb = 0;
				MPEG4DecInterMB(pDec, &vDecLocal, nMBPos);
				break;
			case VO_VIDEO_FRAME_I:
				/*stream: 24bits available*/		
				do{
					UPDATE_CACHE(pDec);
					mcbpc = GetMCBPC_I(pDec); /* mcbpc*/
				}while(mcbpc < 0);

				/*stream: 15bits available*/
				pDec->bACPred = GetBits(pDec,1);
				pDec->nCBP = (GetCBPY(pDec) << 2) | ((mcbpc >> 4) & 3);

				if ((mcbpc & 7) == INTRA_Q){ //mb_type
					VO_S32 nQuant = pDec->nQuant + DQtab[GetBits(pDec,2)]; //DQtab[dquant]
					pDec->nQuant = nQuant<1 ? 1: (nQuant>31 ? 31:nQuant);
				}

				if (pDec->bInterlaced) {
					vDecLocal.bFieldDct = GetBits(pDec, 1);
				}		
				fpReconMB = ReconIntraMB;
				MPEG4DecIntraMB(pDec, &vDecLocal, nMBPos, 0);//TBD merger nMBPos and dp_pos
				break;
			default://P & S frame
				do{/*stuffing mb type*/
					UPDATE_CACHE(pDec);
					if((bNotCode = (VO_S8)GetBits(pDec, 1))){
						mcbpc = 0; //for clean warning
						break;
					}
					mcbpc = GetMCBPC_P(pDec); /* mcbpc */
				}while(mcbpc < 0);
				/*stream: 24bits available*/
				if (!bNotCode) {         //if coded
					mb_type = mcbpc & 7;
					mcbpc = (mcbpc >> 4) & 3;

					//stream: 14bits available
					if (mb_type >= INTRA){
						pDec->frameMap[nMBPos] = (VO_U8)pDec->nCurFrameMap;
						pDec->bACPred = GetBits(pDec,1);
						pDec->nCBP = (GetCBPY(pDec) << 2) | mcbpc;

						//stream: 7bits available
						if (mb_type == INTRA_Q){
							VO_S32 q = pDec->nQuant + DQtab[GetBits(pDec,2)]; //DQtab[dquant]
							pDec->nQuant = q<1 ? 1: (q>31 ? 31:q);
						}

						if (pDec->bInterlaced){
							vDecLocal.bFieldDct = GetBits(pDec, 1);
						}

						//stream: 5bits available
						mv[3] = mv[2] = mv[1] = mv[0] = 0;

						RescuePredict(pDec, nMBPos);  //Restore AC_DC values

						fpReconMB = ReconIntraMB;
						MPEG4DecIntraMB(pDec, &vDecLocal, nMBPos, 0);

					}else{
#ifdef ASP_GMC
						VO_S32 bMCSel = 0;
						if ((pDec->nVOPType == VO_VIDEO_FRAME_S) && (mb_type == INTER || mb_type == INTER_Q))
							bMCSel = GetBits(pDec, 1);
#endif//#ifdef ASP_GMC				

						fpReconMB = fpReconMB_i;
						pDec->frameMap[nMBPos] = (VO_U8)(pDec->nCurFrameMap|RESCUE); // set rescue needed flag	
						pDec->nCBP = ((15-GetCBPY(pDec)) << 2) | mcbpc;

						if (mb_type == INTER_Q){
							VO_S32 q = pDec->nQuant + DQtab[GetBits(pDec,2)]; //DQtab[dquant]
							pDec->nQuant = q<1 ? 1: (q>31 ? 31:q);
						}

						if (pDec->bInterlaced) {
							if (pDec->nCBP) {
								vDecLocal.bFieldDct = GetBits(pDec, 1);
							}

							if ((mb_type == INTER || mb_type == INTER_Q)
#ifdef ASP_GMC
								&& !bMCSel
#endif//#ifdef ASP_GMC				
								)
							{
								if (GetBits(pDec, 1)) {
									pDec->bFieldPred_buf[nMBPos] = 1;
									BckMV[0] = GetBits(pDec, 2);
								}
							}
						}
#ifdef ASP_GMC
						if (bMCSel) {
							MPEG4DecGmcMB(pDec, &vDecLocal, nMBPos, mv);
							if(pDec->nCBP){
								BckMV[0] |= 1<<31;
								FwdMV[0] = 0;
								BckMV[4] = BckMV[3] = BckMV[2] = BckMV[1] = *(mv);
								*gmcmb = 1;
								MPEG4DecInterMB(pDec, &vDecLocal, nMBPos);
							}
							else
							{
#if ENABLE_DEBLOCK
								if (pDec->nVOPType == VO_VIDEO_FRAME_B)
									pDec->pOutFrame->pQuant[nYMBPos * pDec->nMBWidth + nXMBPos] = pDec->nQuant;
								else
									pDec->pCurFrame->pQuant[nYMBPos * pDec->nMBWidth + nXMBPos] = pDec->nQuant;

#endif
								*(vDecLocal.pMBQuant++) = pDec->nQuant;
								continue;					
							}
							break;
						}
#endif//#ifdef ASP_GMC				
						//stream: 8bits available

						// we will use mv[4],mv[5] for temporary purposes
						// in the next macroblock it will be overwrite (mv incremented by 4)
						BckMV[0] |= 1<<31;
						FwdMV[0] = 0;
// 						printf("frame:%d, mb:%d ", pDec->nFrame, nMBPos);
						switch (mb_type) 
						{
						case INTER4V:
							if (!pDec->bInterlaced)
							{
								for (j = 0; j < 4; j++){ 
									mv[j] = GetMV(pDec->nFcode,GetPMV(j,nMBPos,0,pDec),pDec, 0);
									BckMV[j+1] = CorrectMV(mv[j], pDec, nXMBPos, nYMBPos);
								}
							}
							else
							{
								for (j = 0; j < 4; j++){ 
									mv[j] = GetMV(pDec->nFcode,GetPMV_interlace(j,nMBPos,0,pDec),pDec, 0);
									BckMV[j+1] = CorrectMV(mv[j], pDec, nXMBPos, nYMBPos);
								}
							}

							BckMV[5] = GetChromaMV4(BckMV+1, pDec->qpel);

							break;
// 						case INTER_Q:
// 							{
// 								VO_S32 q = pDec->nQuant + DQtab[GetBits(pDec,2)]; //DQtab[dquant]
// 								pDec->nQuant = q<1 ? 1: (q>31 ? 31:q);
// 							}
						default: //case INTER:

							if (!pDec->bInterlaced) {
								mv[3] = mv[2] = mv[1] = mv[0] = 
									GetMV(pDec->nFcode,GetPMV(0,nMBPos,0,pDec),pDec, 0);

								BckMV[4] = BckMV[3] = BckMV[2] = BckMV[1] = CorrectMV(mv[0], pDec, nXMBPos, nYMBPos);

								BckMV[5] = GetChromaMV(BckMV[1], pDec->qpel);
							} else {
								//modified by Harry
								VO_S32 pmv = GetPMV_interlace(0,nMBPos,0,pDec);
								if (pDec->bFieldPred_buf[nMBPos])
								{
									mv[1] = mv[0] = 
										GetMV(pDec->nFcode,pmv,pDec, 1);
									BckMV[2] = BckMV[1] = CorrectMV(mv[0], pDec, nXMBPos, nYMBPos);

									mv[3] = mv[2] = 
										GetMV(pDec->nFcode,pmv,pDec, 1);
									BckMV[4] = BckMV[3] = CorrectMV(mv[2], pDec, nXMBPos, nYMBPos);
									//calc chroma mv later(in mc)
								}
								else
								{
									mv[3] = mv[2] = mv[1] = mv[0] = 
										GetMV(pDec->nFcode,pmv,pDec, 0);

									BckMV[4] = BckMV[3] = BckMV[2] = BckMV[1] = CorrectMV(mv[0], pDec, nXMBPos, nYMBPos);
									BckMV[5] = GetChromaMV(BckMV[1], pDec->qpel);//TBD
								}

							}

							break;
						}
						*gmcmb = 0;
						MPEG4DecInterMB(pDec, &vDecLocal, nMBPos);
					}
				}
#ifdef ASP_GMC
				else if(pDec->nVOPType == VO_VIDEO_FRAME_S){
					pDec->frameMap[nMBPos] = (VO_U8)(pDec->nCurFrameMap|RESCUE); // set rescue needed flag	
					pDec->nCBP = 0;
					fpReconMB = fpReconMB_i;
					MPEG4DecGmcMB(pDec, &vDecLocal, nMBPos, mv);
					if(pDec->nCBP){
						BckMV[0] |= 1<<31;
						FwdMV[0] = 0;
						BckMV[4] = BckMV[3] = BckMV[2] = BckMV[1] = *(mv);
						*gmcmb = 1;
						MPEG4DecInterMB(pDec, &vDecLocal, nMBPos);
					}
					else
					{
#if ENABLE_DEBLOCK
						if (pDec->nVOPType == VO_VIDEO_FRAME_B)
							pDec->pOutFrame->pQuant[nYMBPos * pDec->nMBWidth + nXMBPos] = pDec->nQuant;
						else
							pDec->pCurFrame->pQuant[nYMBPos * pDec->nMBWidth + nXMBPos] = pDec->nQuant;

#endif
						*(vDecLocal.pMBQuant++) = pDec->nQuant;
						continue;					
					}
				}
#endif //#ifdef ASP_GMC
				else{
					// not coded macroblock
					VO_S32 n = pDec->frameMap[nMBPos];

					mv[3] = mv[2] = mv[1] = mv[0] = 0;
					pDec->frameMap[nMBPos] = (VO_U8)(n|RESCUE);

					fpReconMB = ReconCopyMB;
				}
			}

#if ENABLE_DEBLOCK
			if (pDec->nVOPType == VO_VIDEO_FRAME_B)
				pDec->pOutFrame->pQuant[nYMBPos * pDec->nMBWidth + nXMBPos] = pDec->nQuant;
			else
				pDec->pCurFrame->pQuant[nYMBPos * pDec->nMBWidth + nXMBPos] = pDec->nQuant;

#endif
BframeMC:
			*(vDecLocal.pMBQuant++) = pDec->nQuant;

#if ENABLE_ERROR_CONCEAL
			if(pDec->nMBError){
				pDec->nFrameError++;
				if (pDec->nVOPType == VO_VIDEO_FRAME_I && (pDec->nMBError& CONCEAL_ERROR))
				{
					fpReconMB = ReconCopyMB;
				}
			}
#endif
			fpReconMB(pDec, &vDecLocal, nMBPos);
		}
	}

	return VO_ERR_NONE;
}


#endif//ASP_BVOP

///////////////////////////////////////////////////////////////////////////////////////////////
//H.263 related
//#ifdef H263_NEWSDK   //MPEG4


VO_U32 H263DecIVOP( VO_MPEG4_DEC *pDec )
{
	VO_MPEG4_VOP_CONTEXT vopCtxt = pDec->vopCtxt;
	VO_S32 mcbpc, nMBPos;
	//const VO_U32 nMBWidth = pDec->nMBWidth, nMBHeight = pDec->nMBHeight;
	FuncReconMB* fpReconMB = vopCtxt.fpReconMB;
	VO_S32 *pMBPos = vopCtxt.pMBPos;
	VO_MPEG4_DEC_LOCAL vDecLocal;

// 	vopCtxt.nMBCount = 0;
	vDecLocal.bFieldDct = 0;
	vDecLocal.scanType = vopCtxt.scanType;
	vDecLocal.blockptr = vopCtxt.blockptr;
	vDecLocal.blockLen = vopCtxt.blockLen;

	pDec->nLastRefFrame = pDec->nFrame;
	pDec->nMapOfs = pDec->nFrame;
	SetMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, (VO_U8*)pDec->frameMap, 0, pDec->nPosEnd); // set all block to current frame (and clear rescue flag)
	pDec->pCurFrameMB = pDec->pCurFrame;
	pDec->pRefFrameMB = pDec->pRefFrame;

// #if ENABLE_MULTI_THREAD
	if (pDec->nMpeg4Thd > 1)
	{
		//start MB serials, init/reset all sub-threads if needed
		pthread_mutex_lock(&(pDec->parserMutex));
		pDec->nParser = 1;
		pthread_mutex_unlock(&(pDec->parserMutex));
		pDec->nParserErr = VO_ERR_NONE;
		SetMBThreadVOPFunc(pDec, MBrH263IVOP);
	}
// #endif

	for (nMBPos=0;nMBPos<pDec->nPosEnd;nMBPos+=MB_X-pDec->nMBWidth) {
		vDecLocal.pMBQuant = pDec->quantrow;
		for (;POSX(nMBPos)<pDec->nMBWidth;++nMBPos) {
			VO_S32 nYMBPos = POSY(nMBPos), nXMBPos = POSX(nMBPos);
			UPDATE_CACHE(pDec);

			if (EofBits(pDec))
			{
// #if ENABLE_MULTI_THREAD
				if (pDec->nMpeg4Thd > 1)
				{
					pDec->nParserErr = VO_ERR_INPUT_BUFFER_SMALL;
					pthread_mutex_lock(&(pDec->parserMutex));
					pDec->nParser = 0;
					if (pDec->nWaiter)
					{
						pthread_mutex_unlock(&(pDec->parserMutex));
						pthread_cond_signal(&(pDec->parserCond));
					}
					else
					{
						pthread_mutex_unlock(&(pDec->parserMutex));
					}
					goto EndOfH263IVOP;
				}
				else
// #else
					return VO_ERR_INPUT_BUFFER_SMALL;
// #endif
			}

			if (SHOW_BITS(pDec,16) == 0){ 
				if(VO_ERR_NONE != Resync(pDec, &nMBPos))
				{
// #if ENABLE_MULTI_THREAD
					if (pDec->nMpeg4Thd > 1)
					{
						pDec->nParserErr = VO_ERR_DEC_MPEG4_I_FRAME;
						pthread_mutex_lock(&(pDec->parserMutex));
						pDec->nParser = 0;
						if (pDec->nWaiter)
						{
							pthread_mutex_unlock(&(pDec->parserMutex));
							pthread_cond_signal(&(pDec->parserCond));
						}
						else
						{
							pthread_mutex_unlock(&(pDec->parserMutex));
						}
						goto EndOfH263IVOP;
					}
					else
// #else
						return VO_ERR_DEC_MPEG4_I_FRAME;
// #endif
				}

				nYMBPos = POSY(nMBPos);
				nXMBPos = POSX(nMBPos);
				//pMBQuant update, ----by harry, 2011,11,8
				vDecLocal.pMBQuant = pDec->quantrow + nXMBPos;
			}
			*pMBPos = nMBPos;

			pDec->nMBError = 0;

			//stream: 24bits available
			do{
				UPDATE_CACHE(pDec);
				mcbpc = GetMCBPC_I(pDec); // mcbpc
			}while(mcbpc < 0);

			//stream: 15bits available
			
			if (pDec->aic){
				pDec->bACPred = GetBits(pDec, 1); //todo: aic
				if(pDec->bACPred){
					pDec->predict_dir = GetBits(pDec, 1);
				}
			}else{
				pDec->bACPred = 0;
			}

			pDec->nCBP = (GetCBPY(pDec) << 2) | ((mcbpc >> 4) & 3);
				
			if ((mcbpc & 7) == INTRA_Q){ //mb_type
				VO_S32 q = pDec->nQuant + DQtab[GetBits(pDec,2)]; //DQtab[dquant]
				pDec->nQuant = q<1 ? 1: (q>31 ? 31:q);
			}
			*fpReconMB = ReconIntraMB;
			H263DecIntraMB(pDec, nMBPos, &vDecLocal);
#if ENABLE_ERROR_CONCEAL
			if((pDec->nMBError & CONCEAL_ERROR)){
				pDec->nFrameError++;
//				*fpReconMB = ReconCopyMB;
			}
#endif

#if ENABLE_DEBLOCK
			pDec->pCurFrame->pQuant[nYMBPos * pDec->nMBWidth + nXMBPos] = pDec->nQuant;
#endif
			*(vDecLocal.pMBQuant++) = pDec->nQuant;

// #if ENABLE_MULTI_THREAD
			//condition of continue parser
			++vopCtxt.nMBCount;
			if ((vopCtxt.nMBCount < pDec->nMpeg4ThdMaxMB && pDec->nWaiter == 0) || (pDec->nMpeg4Thd > 1 && vopCtxt.nMBCount < pDec->nMpeg4ThdMinMB) )
// #else
// 			if (++vopCtxt.nMBCount < MAX_MB_TASK_NUM)
// #endif
			{
				//parser can go on
				++fpReconMB;
				++pMBPos;
				vDecLocal.scanType += 6;
				vDecLocal.blockptr += 6*64;
				vDecLocal.blockLen += 6;
			}
			else
			{
// #if ENABLE_MULTI_THREAD
				if (pDec->nMpeg4Thd > 1)
				{
					pDec->nParserErr = VO_ERR_NONE;
					pDec->nParserMBPos = nMBPos;
					pDec->pParserMBQuant = vDecLocal.pMBQuant;
					pthread_mutex_lock(&(pDec->parserMutex));
					pDec->nParser = 0;
					if (pDec->nWaiter)
					{
						pthread_mutex_unlock(&(pDec->parserMutex));
						pthread_cond_signal(&(pDec->parserCond));
					}
					else
					{
						pthread_mutex_unlock(&(pDec->parserMutex));
					}
				}
// #endif
				fpReconMB = vopCtxt.fpReconMB;
				pMBPos = vopCtxt.pMBPos;
				vDecLocal.scanType = vopCtxt.scanType;
				vDecLocal.blockptr = vopCtxt.blockptr;
				vDecLocal.blockLen = vopCtxt.blockLen;
				do 
				{
					(*fpReconMB++)(pDec, &vDecLocal, *pMBPos++);
					vDecLocal.scanType += 6;
					vDecLocal.blockptr += 6*64;
					vDecLocal.blockLen += 6;
				} while (--vopCtxt.nMBCount);

				fpReconMB = vopCtxt.fpReconMB;
				pMBPos = vopCtxt.pMBPos;
				vDecLocal.scanType = vopCtxt.scanType;
				vDecLocal.blockptr = vopCtxt.blockptr;
				vDecLocal.blockLen = vopCtxt.blockLen;

// #if ENABLE_MULTI_THREAD
				if (pDec->nMpeg4Thd > 1)
				{
					//wait & update (nMBPos, mv) pMBQuant?
					pthread_mutex_lock(&(pDec->parserMutex));
					if (pDec->nParser)
					{
						++pDec->nWaiter;
						do 
						{
							pthread_cond_wait(&(pDec->parserCond), &(pDec->parserMutex));
						} while (pDec->nParser);
						--pDec->nWaiter;
					}
					pDec->nParser = 1;
					pthread_mutex_unlock(&(pDec->parserMutex));

					if (pDec->nParserErr != VO_ERR_NONE)
					{
						pthread_mutex_lock(&(pDec->parserMutex));
						pDec->nParser = 0;
						if (pDec->nWaiter)
						{
							pthread_mutex_unlock(&(pDec->parserMutex));
							pthread_cond_signal(&(pDec->parserCond));
						}
						else
						{
							pthread_mutex_unlock(&(pDec->parserMutex));
						}
						goto EndOfH263IVOP;
					}

					nMBPos = pDec->nParserMBPos;
					vDecLocal.pMBQuant = pDec->pParserMBQuant;
				}
// #endif
			}
		}
	}

// #if ENABLE_MULTI_THREAD
	if (pDec->nMpeg4Thd > 1)
	{
		pDec->nParserErr = VO_ERR_NONE;
		pDec->nParserMBPos = nMBPos - MB_X + pDec->nMBWidth - 1;	//let other threads finish the MB scanning
		pthread_mutex_lock(&(pDec->parserMutex));
		pDec->nParser = 0;
		if (pDec->nWaiter)
		{
			pthread_mutex_unlock(&(pDec->parserMutex));
			pthread_cond_signal(&(pDec->parserCond));
		}
		else
		{
			pthread_mutex_unlock(&(pDec->parserMutex));
		}
	}
// #endif
	if (vopCtxt.nMBCount > 0)
	{
		fpReconMB = vopCtxt.fpReconMB;
		pMBPos = vopCtxt.pMBPos;
		vDecLocal.scanType = vopCtxt.scanType;
		vDecLocal.blockptr = vopCtxt.blockptr;
		vDecLocal.blockLen = vopCtxt.blockLen;
		do
		{
			(*fpReconMB++)(pDec, &vDecLocal, *pMBPos++);
			vDecLocal.scanType += 6;
			vDecLocal.blockptr += 6*64;
			vDecLocal.blockLen += 6;
		}while (--vopCtxt.nMBCount);
	}

// #if ENABLE_MULTI_THREAD
	if (pDec->nMpeg4Thd > 1)
	{
EndOfH263IVOP:
		//wait all sub-threads finish
#if USE_WHILE_FORVER
		WaitForSubThdFinish(pDec);
#else
		pthread_mutex_lock(&(pDec->finishMutex));
		pDec->bEndVop = 1;
		while (pDec->nFinish < (VO_S32)pDec->nMpeg4Thd-1)
		{
//			thread_sleep(0);
			pthread_cond_wait(&(pDec->finishCond), &(pDec->finishMutex));
		}
		pDec->nFinish = 0;
		pDec->bEndVop = 0;
		pthread_mutex_unlock(&(pDec->finishMutex));
#endif
//		printf("end frame : %d\n", pDec->nParserErr);
		return pDec->nParserErr;
	}
// #else
	return VO_ERR_NONE;
// #endif
}

VO_U32 H263DecPVOP( VO_MPEG4_DEC *pDec )
{
	//if no b-frames, we need only one row of mv vectors
	VO_MPEG4_VOP_CONTEXT vopCtxt = pDec->vopCtxt;
	VO_S32 *BckMV = vopCtxt.MVBack;
	VO_S32 *FwdMV = vopCtxt.MVFwd;
	FuncReconMB* fpReconMB = vopCtxt.fpReconMB;
	VO_S32 *pMBPos = vopCtxt.pMBPos;
	VO_S32 j, nMBPos=0, mcbpc, bNotCode;
	VO_MPEG4_DEC_LOCAL vDecLocal;

// 	vopCtxt.nMBCount = 0;
	vDecLocal.mc_data = vopCtxt.mc_data;
	vDecLocal.gmcmb = 0;
	vDecLocal.bFieldDct = 0;
	vDecLocal.scanType = vopCtxt.scanType;
	vDecLocal.blockptr = vopCtxt.blockptr;
	vDecLocal.blockLen = vopCtxt.blockLen;
	vDecLocal.MVBack = BckMV;
	vDecLocal.MVFwd = FwdMV;

	pDec->nCurFrameMap = (pDec->nFrame - pDec->nMapOfs) << 1;
	pDec->nLastRefFrame = pDec->nFrame;
	pDec->nResyncMarkerBits = RESYNC_MARK_BITS - 1 + pDec->nFcode;
	pDec->pCurFrameMB = pDec->pCurFrame;
	pDec->pRefFrameMB = pDec->pRefFrame;


// #if ENABLE_MULTI_THREAD
	if (pDec->nMpeg4Thd > 1)
	{
		//start MB serials, init/reset all sub-threads if needed
		pthread_mutex_lock(&(pDec->parserMutex));
		pDec->nParser = 1;
		pthread_mutex_unlock(&(pDec->parserMutex));
		pDec->nParserErr = VO_ERR_NONE;
		SetMBThreadVOPFunc(pDec, MBrH263PVOP);
	}
// #endif

	//DPRINTF(ff_debug, "P frame start@@@@@@@@@@@@@@@@@@@@@@@@@ \n");

	for (nMBPos=0;nMBPos<pDec->nPosEnd;nMBPos+=MB_X-pDec->nMBWidth) {
		VO_S32* mv = &pDec->mv_buf[(nMBPos & pDec->mv_bufmask)*4];
		vDecLocal.pMBQuant = pDec->quantrow;
		for (;POSX(nMBPos)<pDec->nMBWidth;++nMBPos,mv+=4) {
			VO_S32 nYMBPos = POSY(nMBPos), nXMBPos = POSX(nMBPos);
		
			UPDATE_CACHE(pDec);

			if (EofBits(pDec))
			{
// #if ENABLE_MULTI_THREAD
				if (pDec->nMpeg4Thd > 1)
				{
					pDec->nParserErr = VO_ERR_INPUT_BUFFER_SMALL;
					pthread_mutex_lock(&(pDec->parserMutex));
					pDec->nParser = 0;
					if (pDec->nWaiter)
					{
						pthread_mutex_unlock(&(pDec->parserMutex));
						pthread_cond_signal(&(pDec->parserCond));
					}
					else
					{
						pthread_mutex_unlock(&(pDec->parserMutex));
					}
					goto EndOfH263PVOP;
				}
				else
// #else
					return VO_ERR_INPUT_BUFFER_SMALL;
// #endif
			}

			if (SHOW_BITS(pDec,16) == 0){
				if(VO_ERR_NONE != Resync(pDec, &nMBPos))
				{
// #if ENABLE_MULTI_THREAD
					if (pDec->nMpeg4Thd > 1)
					{
						pDec->nParserErr = VO_ERR_DEC_MPEG4_P_FRAME;
						pthread_mutex_lock(&(pDec->parserMutex));
						pDec->nParser = 0;
						if (pDec->nWaiter)
						{
							pthread_mutex_unlock(&(pDec->parserMutex));
							pthread_cond_signal(&(pDec->parserCond));
						}
						else
						{
							pthread_mutex_unlock(&(pDec->parserMutex));
						}
						goto EndOfH263PVOP;
					}
					else
// #else
						return VO_ERR_DEC_MPEG4_P_FRAME;
// #endif
				}

				nYMBPos = POSY(nMBPos);
				nXMBPos = POSX(nMBPos);
				//mv & pMBQuant update, ----by harry, 2011,11,8
				mv = &pDec->mv_buf[(nMBPos & pDec->mv_bufmask)*4];
				vDecLocal.pMBQuant = pDec->quantrow + nXMBPos;
			}
			*pMBPos = nMBPos;

			/*TBD package the initial process*/

			pDec->nMBError = 0;

			//stream: 24bits available
			do{// stuffing mb type
				UPDATE_CACHE(pDec);
				if((bNotCode = GetBits(pDec, 1))){
					break;
				}
				mcbpc = GetMCBPC_P(pDec); // mcbpc
			}while(mcbpc < 0);

			if (!bNotCode) {         //if coded

//				VO_S32 mcbpc = GetMCBPC_P(pDec); // mcbpc
				VO_S32 mb_type = mcbpc & 7;
				mcbpc = (mcbpc >> 4) & 3;

				//stream: 14bits available

				if (mb_type >= INTRA)
				{
					pDec->frameMap[nMBPos] = (VO_U8)pDec->nCurFrameMap;

					if (pDec->aic)
						pDec->bACPred = GetBits(pDec,1); //todo: aic
					else
						pDec->bACPred = 0;

					pDec->nCBP = (GetCBPY(pDec) << 2) | mcbpc;

					//stream: 7bits available
					//DPRINTF(ff_debug, "CBP = %d \n",pDec->nCBP);

					if (mb_type == INTRA_Q){
						VO_S32 q = pDec->nQuant + DQtab[GetBits(pDec,2)]; //DQtab[dquant]
						pDec->nQuant = q<1 ? 1: (q>31 ? 31:q);
					}

					//stream: 5bits available

					mv[3] = mv[2] = mv[1] = mv[0] = 0;
					*fpReconMB = ReconIntraMB;
					H263DecIntraMB(pDec, nMBPos, &vDecLocal);
				}
				else
				{
					pDec->frameMap[nMBPos] = (VO_U8)(pDec->nCurFrameMap|RESCUE); // set rescue needed flag

					pDec->nCBP = ((15-GetCBPY(pDec)) << 2) | mcbpc;
					//DPRINTF(ff_debug, "CBP = %d \n",pDec->nCBP);

					//stream: 8bits available

					// we will use mv[4],mv[5] for temporary purposes
					// in the next macroblock it will be overwrite (mv incremented by 4)
					

					switch (mb_type) 
					{
					case INTER4V:
						BckMV[0] = 1;
						FwdMV[0] = 0;
						for (j = 0; j < 4; j++) {
							mv[j] = GetMV(pDec->nFcode,GetPMV(j,nMBPos,0,pDec),pDec, 0);
							BckMV[j+1] = CorrectMV(mv[j], pDec, nXMBPos, nYMBPos);

						}
						
						/*TBD Calculate chroma mv*/
						BckMV[5] = GetChromaMV4(BckMV+1, pDec->qpel);

						break;

					case INTER_Q:
						{
							VO_S32 q = pDec->nQuant + DQtab[GetBits(pDec,2)]; //DQtab[dquant]
							pDec->nQuant = q<1 ? 1: (q>31 ? 31:q);
						}

					default: //case INTER:

						BckMV[0] = 1;
						FwdMV[0] = 0;
						mv[3] = mv[2] = mv[1] = mv[0] = 
							GetMV(pDec->nFcode,GetPMV(0,nMBPos,0,pDec),pDec, 0);

						BckMV[4] = BckMV[3] = BckMV[2] = BckMV[1] = CorrectMV( mv[0], pDec, nXMBPos, nYMBPos);
						
						BckMV[5] = GetChromaMV(BckMV[1], pDec->qpel);

						break;
					}
					*fpReconMB = ReconInterMB;
					H263DecInterMB(pDec, nMBPos, &vDecLocal);
				}
#if ENABLE_ERROR_CONCEAL
				if(pDec->nMBError){
					pDec->nFrameError++;
//					if(pDec->nMBError& CONCEAL_ERROR){
// 						*fpReconMB = ReconCopyMB;					
//					}
				}
#endif
			}else {
				// not coded macroblock
				VO_S32 n = pDec->frameMap[nMBPos];
				pDec->frameMap[nMBPos] = (VO_U8)(n|RESCUE);
				mv[3] = mv[2] = mv[1] = mv[0] = 0;
				*fpReconMB = ReconCopyMB;
			}
#if ENABLE_DEBLOCK
			pDec->pCurFrame->pQuant[nYMBPos * pDec->nMBWidth + nXMBPos] = pDec->nQuant;
#endif
			*(vDecLocal.pMBQuant++) = pDec->nQuant;

// #if ENABLE_MULTI_THREAD
			//condition of continue parser
			++vopCtxt.nMBCount;
			if ((vopCtxt.nMBCount < pDec->nMpeg4ThdMaxMB && pDec->nWaiter == 0) || (pDec->nMpeg4Thd > 1 && vopCtxt.nMBCount < pDec->nMpeg4ThdMinMB) )
// #else
// 			if (++vopCtxt.nMBCount < MAX_MB_TASK_NUM)
// #endif
			{
				//parser can go on
				++fpReconMB;
				++pMBPos;
				vDecLocal.scanType += 6;
				vDecLocal.blockptr += 6*64;
				vDecLocal.blockLen += 6;
				vDecLocal.MVBack = BckMV += 6;
				vDecLocal.MVFwd  = FwdMV += 6;
			}
			else
			{
// #if ENABLE_MULTI_THREAD
				if (pDec->nMpeg4Thd > 1)
				{
					pDec->nParserErr = VO_ERR_NONE;
					pDec->nParserMBPos = nMBPos;
					pDec->pParserMBQuant = vDecLocal.pMBQuant;
					pthread_mutex_lock(&(pDec->parserMutex));
					pDec->nParser = 0;
					if (pDec->nWaiter)
					{
						pthread_mutex_unlock(&(pDec->parserMutex));
						pthread_cond_signal(&(pDec->parserCond));
					}
					else
					{
						pthread_mutex_unlock(&(pDec->parserMutex));
					}
				}
// #endif

				fpReconMB = vopCtxt.fpReconMB;
				pMBPos = vopCtxt.pMBPos;
				vDecLocal.scanType = vopCtxt.scanType;
				vDecLocal.blockptr = vopCtxt.blockptr;
				vDecLocal.blockLen = vopCtxt.blockLen;
				vDecLocal.MVBack = vopCtxt.MVBack;
				vDecLocal.MVFwd = vopCtxt.MVFwd;
				do 
				{
					(*fpReconMB++)(pDec, &vDecLocal, *pMBPos++);
					vDecLocal.scanType += 6;
					vDecLocal.blockptr += 6*64;
					vDecLocal.blockLen += 6;
					vDecLocal.MVBack += 6;
					vDecLocal.MVFwd  += 6;
				} while (--vopCtxt.nMBCount);

				fpReconMB = vopCtxt.fpReconMB;
				pMBPos = vopCtxt.pMBPos;
				vDecLocal.scanType = vopCtxt.scanType;
				vDecLocal.blockptr = vopCtxt.blockptr;
				vDecLocal.blockLen = vopCtxt.blockLen;
				BckMV = vDecLocal.MVBack = vopCtxt.MVBack;
				FwdMV = vDecLocal.MVFwd = vopCtxt.MVFwd;

// #if ENABLE_MULTI_THREAD
				if (pDec->nMpeg4Thd > 1)
				{
					//wait & update (nMBPos, mv) pMBQuant?
					pthread_mutex_lock(&(pDec->parserMutex));
					if (pDec->nParser)
					{
						++pDec->nWaiter;
						do 
						{
							pthread_cond_wait(&(pDec->parserCond), &(pDec->parserMutex));
						} while (pDec->nParser);
						--pDec->nWaiter;
					}
					pDec->nParser = 1;
					pthread_mutex_unlock(&(pDec->parserMutex));

					if (pDec->nParserErr != VO_ERR_NONE)
					{
						pthread_mutex_lock(&(pDec->parserMutex));
						pDec->nParser = 0;
						if (pDec->nWaiter)
						{
							pthread_mutex_unlock(&(pDec->parserMutex));
							pthread_cond_signal(&(pDec->parserCond));
						}
						else
						{
							pthread_mutex_unlock(&(pDec->parserMutex));
						}
						goto EndOfH263PVOP;
					}

					nMBPos = pDec->nParserMBPos;
					vDecLocal.pMBQuant = pDec->pParserMBQuant;
					mv = &pDec->mv_buf[(nMBPos & pDec->mv_bufmask)*4];
				}
// #endif
			}
		}
	}

// #if ENABLE_MULTI_THREAD
	if (pDec->nMpeg4Thd > 1)
	{
		pDec->nParserErr = VO_ERR_NONE;
		pDec->nParserMBPos = nMBPos - MB_X + pDec->nMBWidth - 1;	//let other threads finish the MB scanning
		pthread_mutex_lock(&(pDec->parserMutex));
		pDec->nParser = 0;
		if (pDec->nWaiter)
		{
			pthread_mutex_unlock(&(pDec->parserMutex));
			pthread_cond_signal(&(pDec->parserCond));
		}
		else
		{
			pthread_mutex_unlock(&(pDec->parserMutex));
		}
	}
// #endif
	if (vopCtxt.nMBCount > 0)
	{
		fpReconMB = vopCtxt.fpReconMB;
		pMBPos = vopCtxt.pMBPos;
		vDecLocal.scanType = vopCtxt.scanType;
		vDecLocal.blockptr = vopCtxt.blockptr;
		vDecLocal.blockLen = vopCtxt.blockLen;
		vDecLocal.MVBack = vopCtxt.MVBack;
		vDecLocal.MVFwd = vopCtxt.MVFwd;
		do
		{
			(*fpReconMB++)(pDec, &vDecLocal, *pMBPos++);
			vDecLocal.scanType += 6;
			vDecLocal.blockptr += 6*64;
			vDecLocal.blockLen += 6;
			vDecLocal.MVBack += 6;
			vDecLocal.MVFwd  += 6;
		}while (--vopCtxt.nMBCount);
	}

// #if ENABLE_MULTI_THREAD
	if (pDec->nMpeg4Thd > 1)
	{
EndOfH263PVOP:
		//wait all sub-threads finish
#if USE_WHILE_FORVER
		WaitForSubThdFinish(pDec);
#else
		pthread_mutex_lock(&(pDec->finishMutex));
		pDec->bEndVop = 1;
		while (pDec->nFinish < (VO_S32)pDec->nMpeg4Thd - 1)
		{
//			thread_sleep(0);
			pthread_cond_wait(&(pDec->finishCond), &(pDec->finishMutex));
		}
		pDec->nFinish = 0;
		pDec->bEndVop = 0;
		pthread_mutex_unlock(&(pDec->finishMutex));
#endif
// 		printf("end frame : %d\n", pDec->nParserErr);
		return pDec->nParserErr;
	}
// #else
	return VO_ERR_NONE;
// #endif
}

VO_U32 H263DecBVOP( VO_MPEG4_DEC *pDec )
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 MPEG4DecMBRow(VO_MPEG4_DEC *pDec, VO_MPEG4_DEC_LOCAL *vDecLocal, VO_MPEG4_VOP_CONTEXT *vopCtxt)
{
	VO_S32 nMBPos = vDecLocal->nMBPos;
	VO_S32 fprev0, fprev1;
	VO_S32 bprev0, bprev1;
	VO_S32 j,mcbpc,bNotCode;
	VO_S32* mv;
	FuncReconMB* fpReconMB;
	VO_S32 *pMBPos;
	VO_S8 *gmcmb;
	VO_S8 *bFieldDct;
	VO_S32 *BckMV;
	VO_S32 *FwdMV;

	vopCtxt->nMBCount = 0;
	fpReconMB = vopCtxt->fpReconMB;
	pMBPos = vopCtxt->pMBPos;
	gmcmb = vopCtxt->gmcmb;
	bFieldDct = vopCtxt->bFieldDct;

	vDecLocal->mc_data = vopCtxt->mc_data;
	vDecLocal->scanType = vopCtxt->scanType;
	vDecLocal->blockptr = vopCtxt->blockptr;
	vDecLocal->blockLen = vopCtxt->blockLen;
	vDecLocal->MVBack = BckMV = vopCtxt->MVBack;
	vDecLocal->MVFwd = FwdMV = vopCtxt->MVFwd;
	if (pDec->nVOPType == VO_VIDEO_FRAME_B)
	{
		fprev0 = vDecLocal->fprev0;
		fprev1 = vDecLocal->fprev1;
		bprev0 = vDecLocal->bprev0;
		bprev1 = vDecLocal->bprev1;
	}
	else
	{
		fprev0 = 0;
		fprev1 = 0;
		bprev0 = 0;
		bprev1 = 0;
	}
	for (;nMBPos<pDec->nPosEnd;nMBPos+=MB_X-pDec->nMBWidth) 
	{
		VO_S32 n;
		mv = &pDec->mv_buf[(nMBPos & pDec->mv_bufmask)*4];
		for (;POSX(nMBPos)<pDec->nMBWidth;++nMBPos,mv+=4) {
			VO_S32 nYMBPos = POSY(nMBPos), nXMBPos = POSX(nMBPos);
			VO_S32 mb_type;
			pDec->nMBError = 0;

			/*for interlace*/
// 			pDec->bFieldPred_buf[nMBPos] = 0; 
			UPDATE_CACHE(pDec);

			/* skip stuffing*/
			switch(pDec->nVOPType)
			{
			case VO_VIDEO_FRAME_B:
				// did the last refframe touch this block?
				if (pDec->nLastRefFrame > pDec->nMapOfs+(pDec->frameMap[nMBPos] >> 1)){
					*fpReconMB = ReconCopyMB_B;
					*pMBPos = nMBPos;
					*bFieldDct = 0;
					goto BframeMC_MT;
				}
				break;
			case VO_VIDEO_FRAME_I:
				while (SHOW_BITS(pDec, 9) == 1){
					FLUSH_BITS(pDec, 9);
					UPDATE_CACHE(pDec);
					if (EofBits(pDec))
					{
// #if ENABLE_MULTI_THREAD
							pDec->nParserErr = VO_ERR_INPUT_BUFFER_SMALL;
#if USE_WHILE_FORVER
							pthread_mutex_lock(&(pDec->parserMutex));
							pDec->nParser = 0;
							pthread_mutex_unlock(&(pDec->parserMutex));
#else
							pthread_mutex_lock(&(pDec->parserMutex));
							pDec->nParser = 0;
							if (pDec->nWaiter)
							{
								pthread_mutex_unlock(&(pDec->parserMutex));
								pthread_cond_signal(&(pDec->parserCond));
							}
							else
							{
								pthread_mutex_unlock(&(pDec->parserMutex));
							}
#endif
							return pDec->nParserErr;

// #endif
					}
				}
				break;
			default://P & S frame
				while (SHOW_BITS(pDec,10) == 1){
					FLUSH_BITS(pDec,10);
					UPDATE_CACHE(pDec);
					if (EofBits(pDec))
					{
// #if ENABLE_MULTI_THREAD
							pDec->nParserErr = VO_ERR_INPUT_BUFFER_SMALL;
#if USE_WHILE_FORVER
							pthread_mutex_lock(&(pDec->parserMutex));
							pDec->nParser = 0;
							pthread_mutex_unlock(&(pDec->parserMutex));
#else
							pthread_mutex_lock(&(pDec->parserMutex));
							pDec->nParser = 0;
							if (pDec->nWaiter)
							{
								pthread_mutex_unlock(&(pDec->parserMutex));
								pthread_cond_signal(&(pDec->parserCond));
							}
							else
							{
								pthread_mutex_unlock(&(pDec->parserMutex));
							}
#endif
							return pDec->nParserErr;
// #endif
					}
				}
			}

			if (EofBits(pDec))
			{
// #if ENABLE_MULTI_THREAD
					pDec->nParserErr = VO_ERR_INPUT_BUFFER_SMALL;
#if USE_WHILE_FORVER
					pthread_mutex_lock(&(pDec->parserMutex));
					pDec->nParser = 0;
					pthread_mutex_unlock(&(pDec->parserMutex));
#else
					pthread_mutex_lock(&(pDec->parserMutex));
					pDec->nParser = 0;
					if (pDec->nWaiter)
					{
						pthread_mutex_unlock(&(pDec->parserMutex));
						pthread_cond_signal(&(pDec->parserCond));
					}
					else
					{
						pthread_mutex_unlock(&(pDec->parserMutex));
					}
#endif
					return pDec->nParserErr;
// #endif
			}

			if (CheckResyncMarker(pDec)){
				if(VO_ERR_NONE != Resync(pDec, &nMBPos))
				{
// #if ENABLE_MULTI_THREAD
						pDec->nParserErr = vDecLocal->voERR;
#if USE_WHILE_FORVER
						pthread_mutex_lock(&(pDec->parserMutex));
						pDec->nParser = 0;
						pthread_mutex_unlock(&(pDec->parserMutex));
#else
						pthread_mutex_lock(&(pDec->parserMutex));
						pDec->nParser = 0;
						if (pDec->nWaiter)
						{
							pthread_mutex_unlock(&(pDec->parserMutex));
							pthread_cond_signal(&(pDec->parserCond));
						}
						else
						{
							pthread_mutex_unlock(&(pDec->parserMutex));
						}
#endif
						return pDec->nParserErr;
// #endif
				}

				nYMBPos = POSY(nMBPos);
				nXMBPos = POSX(nMBPos);

				if (pDec->nVOPType == VO_VIDEO_FRAME_B)
				{
					fprev1 = fprev0 = bprev1 = bprev0 = 0;
				}
				//mv & pMBQuant update, ----by harry, 2011,11,8
				mv = &pDec->mv_buf[(nMBPos & pDec->mv_bufmask)*4];
				vDecLocal->pMBQuant = pDec->quantrow + nXMBPos;
			}
			*pMBPos = nMBPos;
			*bFieldDct = 0;
			/*TBD package the initial process*/

			switch(pDec->nVOPType)
			{
			case VO_VIDEO_FRAME_B:
				// did the last refframe touch this block?
				if (pDec->nLastRefFrame > pDec->nMapOfs+(pDec->frameMap[nMBPos] >> 1)){
					*fpReconMB = ReconCopyMB_B;
					goto BframeMC_MT;
				}
				*fpReconMB = vDecLocal->fpReconMB_i;
				//stream: 24bits available
				if (GetBits(pDec, 1)){
					pDec->nCBP = 0;
					MPEG4DecDirectMB(pDec,nMBPos,0, vDecLocal);
					//TBD
					*gmcmb = 0;
					MPEG4DecInterMB(pDec, vDecLocal, nMBPos);
					break;
				}
				n = GetBits(pDec,1);

				for (mb_type=0; mb_type<=3; ++mb_type)
					if (GetBits(pDec, 1))
						break;

				//stream: 19bits available
				if (!n)
					pDec->nCBP = GetBits(pDec,6);
				else
					pDec->nCBP = 0;

				if (mb_type != DIRECT){
					if (pDec->nCBP && GetBits(pDec, 1)){
						VO_S32 q = pDec->nQuant + (GetBits(pDec, 1) ? 2:-2);
						pDec->nQuant = q<1 ? 1: (q>31 ? 31:q);
					}
				}
				if (pDec->bInterlaced) {
					if (pDec->nCBP) {
						*bFieldDct = (VO_S8)GetBits(pDec, 1);
					}

					if (mb_type) {

						if (GetBits(pDec, 1)) {
							pDec->bFieldPred_buf[nMBPos] = 1;
							if (mb_type != BACKWARD)
							{
								FwdMV[0] = GetBits(pDec, 2);
							}
							if (mb_type != FORWARD)
							{
								BckMV[0] = GetBits(pDec, 2);
							}
						}
					}
				}

				//stream: 13bits available	
				switch (mb_type){ 
				case DIRECT:
					MPEG4DecDirectMB(pDec,nMBPos,GetMV( 1, 0, pDec,0), vDecLocal);
					break;
				case BACKWARD:
					BckMV[0] |= 1<<31;
					FwdMV[0] = 0;
					if (!pDec->bInterlaced || !pDec->bFieldPred_buf[nMBPos])
					{
						bprev1 = bprev0 = GetMV( pDec->nBcode, bprev0, pDec , 0);
						BckMV[4]=BckMV[3]=BckMV[2]=BckMV[1] = CorrectMV( bprev0, pDec , nXMBPos, nYMBPos);
						BckMV[5]=GetChromaMV(BckMV[1], pDec->qpel);
					}
					else
					{
						bprev0 = GetMV( pDec->nBcode, bprev0, pDec , 1);
						BckMV[2]=BckMV[1] = CorrectMV( bprev0, pDec , nXMBPos, nYMBPos);

						bprev1 = GetMV( pDec->nBcode, bprev1, pDec , 1);
						BckMV[4]=BckMV[3] = CorrectMV( bprev1, pDec , nXMBPos, nYMBPos);
						//calc chroma mv later(in mc)
					}

					break;

				case FORWARD:
					BckMV[0] = 0;
					FwdMV[0] |= 1<<31;
					if (!pDec->bInterlaced || !pDec->bFieldPred_buf[nMBPos])
					{
						fprev1 = fprev0 = GetMV( pDec->nFcode, fprev0, pDec , 0);
						FwdMV[4]=FwdMV[3]=FwdMV[2]=FwdMV[1] = CorrectMV(fprev0, pDec , nXMBPos, nYMBPos);

						FwdMV[5]=GetChromaMV(FwdMV[1], pDec->qpel);
					}
					else
					{
						fprev0 = GetMV( pDec->nBcode, fprev0, pDec , 1);
						FwdMV[2]=FwdMV[1] = CorrectMV( fprev0, pDec , nXMBPos, nYMBPos);

						fprev1 = GetMV( pDec->nBcode, fprev1, pDec , 1);
						FwdMV[4]=FwdMV[3] = CorrectMV( fprev1, pDec , nXMBPos, nYMBPos);
						//calc chroma mv later(in mc)
					}

					break;

				default: //case INTERPOLATE:
					BckMV[0] |= 1<<31;
					FwdMV[0] |= 1<<31;
					if (!pDec->bInterlaced || !pDec->bFieldPred_buf[nMBPos])
					{
						fprev1 = fprev0 = GetMV( pDec->nFcode, fprev0, pDec, 0 );
						FwdMV[4]=FwdMV[3]=FwdMV[2]=FwdMV[1] = CorrectMV(fprev0, pDec , nXMBPos, nYMBPos);
						FwdMV[5]=GetChromaMV(FwdMV[1], pDec->qpel);
						bprev1 = bprev0 = GetMV( pDec->nBcode, bprev0, pDec , 0);
						BckMV[4]=BckMV[3]=BckMV[2]=BckMV[1] = CorrectMV(bprev0, pDec , nXMBPos, nYMBPos);
						BckMV[5]=GetChromaMV(BckMV[1], pDec->qpel);
					}
					else
					{
						fprev0 = GetMV( pDec->nFcode, fprev0, pDec, 1 );
						FwdMV[2]=FwdMV[1] = CorrectMV(fprev0, pDec , nXMBPos, nYMBPos);
						fprev1 = GetMV( pDec->nFcode, fprev1, pDec, 1 );
						FwdMV[4]=FwdMV[3] = CorrectMV(fprev1, pDec , nXMBPos, nYMBPos);
						bprev0 = GetMV( pDec->nBcode, bprev0, pDec , 1);
						BckMV[2]=BckMV[1] = CorrectMV(bprev0, pDec , nXMBPos, nYMBPos);
						bprev1 = GetMV( pDec->nBcode, bprev1, pDec , 1);
						BckMV[4]=BckMV[3] = CorrectMV(bprev1, pDec , nXMBPos, nYMBPos);
					}
				}

				*gmcmb = 0;
				MPEG4DecInterMB(pDec, vDecLocal, nMBPos);
				break;
			case VO_VIDEO_FRAME_I:
				/*stream: 24bits available*/		
				do{
					UPDATE_CACHE(pDec);
					mcbpc = GetMCBPC_I(pDec); /* mcbpc*/
				}while(mcbpc < 0);

				/*stream: 15bits available*/
				pDec->bACPred = GetBits(pDec,1);
				pDec->nCBP = (GetCBPY(pDec) << 2) | ((mcbpc >> 4) & 3);

				if ((mcbpc & 7) == INTRA_Q){ //mb_type
					VO_S32 nQuant = pDec->nQuant + DQtab[GetBits(pDec,2)]; //DQtab[dquant]
					pDec->nQuant = nQuant<1 ? 1: (nQuant>31 ? 31:nQuant);
				}

				if (pDec->bInterlaced) {
					*bFieldDct = (VO_S8)GetBits(pDec, 1);
				}		
				*fpReconMB = ReconIntraMB;
				MPEG4DecIntraMB(pDec, vDecLocal, nMBPos, 0);//TBD merger nMBPos and dp_pos
// if (frameNumber == 246 && nYMBPos == 24 && nXMBPos == 1)
// {
// 	*fpReconMB = ReconCopyMB_B;
// }
				break;
			default://P & S frame
				do{/*stuffing mb type*/
					UPDATE_CACHE(pDec);
					if((bNotCode = (VO_S8)GetBits(pDec, 1))){
						mcbpc = 0;	//for clean warning
						break;
					}
					mcbpc = GetMCBPC_P(pDec); /* mcbpc */
				}while(mcbpc < 0);
				/*stream: 24bits available*/
				if (!bNotCode) {         //if coded
					mb_type = mcbpc & 7;
					mcbpc = (mcbpc >> 4) & 3;

					//stream: 14bits available
					if (mb_type >= INTRA){
						pDec->frameMap[nMBPos] = (VO_U8)pDec->nCurFrameMap;
						pDec->bACPred = GetBits(pDec,1);
						pDec->nCBP = (GetCBPY(pDec) << 2) | mcbpc;

						//stream: 7bits available
						if (mb_type == INTRA_Q){
							VO_S32 q = pDec->nQuant + DQtab[GetBits(pDec,2)]; //DQtab[dquant]
							pDec->nQuant = q<1 ? 1: (q>31 ? 31:q);
						}

						if (pDec->bInterlaced){
							*bFieldDct = (VO_S8)GetBits(pDec, 1);
						}

						//stream: 5bits available
						mv[3] = mv[2] = mv[1] = mv[0] = 0;

						RescuePredict(pDec, nMBPos);  //Restore AC_DC values

						*fpReconMB = ReconIntraMB;
						MPEG4DecIntraMB(pDec, vDecLocal, nMBPos, 0);

					}else{
#ifdef ASP_GMC
						VO_S32 bMCSel = 0;
						if ((pDec->nVOPType == VO_VIDEO_FRAME_S) && (mb_type == INTER || mb_type == INTER_Q))
							bMCSel = GetBits(pDec, 1);
#endif//#ifdef ASP_GMC				

						*fpReconMB = vDecLocal->fpReconMB_i;
						pDec->frameMap[nMBPos] = (VO_U8)(pDec->nCurFrameMap|RESCUE); // set rescue needed flag	
						pDec->nCBP = ((15-GetCBPY(pDec)) << 2) | mcbpc;

						if (mb_type == INTER_Q){
							VO_S32 q = pDec->nQuant + DQtab[GetBits(pDec,2)]; //DQtab[dquant]
							pDec->nQuant = q<1 ? 1: (q>31 ? 31:q);
						}

						if (pDec->bInterlaced) {
							if (pDec->nCBP) {
								*bFieldDct = (VO_S8)GetBits(pDec, 1);
							}

							if ((mb_type == INTER || mb_type == INTER_Q)
#ifdef ASP_GMC
								&& !bMCSel
#endif//#ifdef ASP_GMC				
								)
							{
								if (GetBits(pDec, 1)) {
									pDec->bFieldPred_buf[nMBPos] = 1;
									BckMV[0] = GetBits(pDec, 2);
								}
							}
						}
#ifdef ASP_GMC
						if (bMCSel) {
							MPEG4DecGmcMB(pDec, vDecLocal, nMBPos, mv);
							if(pDec->nCBP){
								BckMV[0] |= 1<<31;
								FwdMV[0] = 0;
								BckMV[4] = BckMV[3] = BckMV[2] = BckMV[1] = *(mv);
								*gmcmb = 1;
								MPEG4DecInterMB(pDec, vDecLocal, nMBPos);
							}
							else
							{
#if ENABLE_DEBLOCK
								if (pDec->nVOPType == VO_VIDEO_FRAME_B)
									pDec->pOutFrame->pQuant[nYMBPos * pDec->nMBWidth + nXMBPos] = pDec->nQuant;
								else
									pDec->pCurFrame->pQuant[nYMBPos * pDec->nMBWidth + nXMBPos] = pDec->nQuant;

#endif
								*(vDecLocal->pMBQuant++) = pDec->nQuant;
								continue;					
							}
 							break;
						}
#endif//#ifdef ASP_GMC				
						//stream: 8bits available

						// we will use mv[4],mv[5] for temporary purposes
						// in the next macroblock it will be overwrite (mv incremented by 4)
						BckMV[0] |= 1<<31;
						FwdMV[0] = 0;
						switch (mb_type) 
						{
						case INTER4V:

							if (!pDec->bInterlaced)
							{
								for (j = 0; j < 4; j++){ 
									mv[j] = GetMV(pDec->nFcode,GetPMV(j,nMBPos,0,pDec),pDec, 0);
									BckMV[j+1] = CorrectMV(mv[j], pDec, nXMBPos, nYMBPos);
								}
							}
							else
							{
								for (j = 0; j < 4; j++){ 
									mv[j] = GetMV(pDec->nFcode,GetPMV_interlace(j,nMBPos,0,pDec),pDec, 0);
									BckMV[j+1] = CorrectMV(mv[j], pDec, nXMBPos, nYMBPos);
								}
							}

							BckMV[5] = GetChromaMV4(BckMV+1, pDec->qpel);

							break;
// 						case INTER_Q:
// 							{
// 								VO_S32 q = pDec->nQuant + DQtab[GetBits(pDec,2)]; //DQtab[dquant]
// 								pDec->nQuant = q<1 ? 1: (q>31 ? 31:q);
// 							}
						default: //case INTER:

							if (!pDec->bInterlaced) {
								mv[3] = mv[2] = mv[1] = mv[0] = 
									GetMV(pDec->nFcode,GetPMV(0,nMBPos,0,pDec),pDec, 0);

								BckMV[4] = BckMV[3] = BckMV[2] = BckMV[1] = CorrectMV(mv[0], pDec, nXMBPos, nYMBPos);

								BckMV[5] = GetChromaMV(BckMV[1], pDec->qpel);
							} else {
								//modified by Harry
								VO_S32 pmv = GetPMV_interlace(0,nMBPos,0,pDec);
								if (pDec->bFieldPred_buf[nMBPos])
								{
									mv[1] = mv[0] = 
										GetMV(pDec->nFcode,pmv,pDec, 1);
									BckMV[2] = BckMV[1] = CorrectMV(mv[0], pDec, nXMBPos, nYMBPos);

									mv[3] = mv[2] = 
										GetMV(pDec->nFcode,pmv,pDec, 1);
									BckMV[4] = BckMV[3] = CorrectMV(mv[2], pDec, nXMBPos, nYMBPos);
									//calc chroma mv later(in mc)
								}
								else
								{
									mv[3] = mv[2] = mv[1] = mv[0] = 
										GetMV(pDec->nFcode,pmv,pDec, 0);

									BckMV[4] = BckMV[3] = BckMV[2] = BckMV[1] = CorrectMV(mv[0], pDec, nXMBPos, nYMBPos);
									BckMV[5] = GetChromaMV(BckMV[1], pDec->qpel);//TBD
								}

							}

							break;
						}
						*gmcmb = 0;
						MPEG4DecInterMB(pDec, vDecLocal, nMBPos);
					}
				}
#ifdef ASP_GMC
				else if(pDec->nVOPType == VO_VIDEO_FRAME_S){
					pDec->frameMap[nMBPos] = (VO_U8)(pDec->nCurFrameMap|RESCUE); // set rescue needed flag	
					pDec->nCBP = 0;
					*fpReconMB = vDecLocal->fpReconMB_i;
					MPEG4DecGmcMB(pDec, vDecLocal, nMBPos, mv);
					if(pDec->nCBP){
						BckMV[0] |= 1<<31;
						FwdMV[0] = 0;
						BckMV[4] = BckMV[3] = BckMV[2] = BckMV[1] = *(mv);
						*gmcmb = 1;
						MPEG4DecInterMB(pDec, vDecLocal, nMBPos);
					}
					else
					{
#if ENABLE_DEBLOCK
						if (pDec->nVOPType == VO_VIDEO_FRAME_B)
							pDec->pOutFrame->pQuant[nYMBPos * pDec->nMBWidth + nXMBPos] = pDec->nQuant;
						else
							pDec->pCurFrame->pQuant[nYMBPos * pDec->nMBWidth + nXMBPos] = pDec->nQuant;

#endif
						*(vDecLocal->pMBQuant++) = pDec->nQuant;
						continue;					
					}
				}
#endif //#ifdef ASP_GMC
				else{
					// not coded macroblock
					VO_S32 n = pDec->frameMap[nMBPos];

					mv[3] = mv[2] = mv[1] = mv[0] = 0;
					pDec->frameMap[nMBPos] = (VO_U8)(n|RESCUE);

					*fpReconMB = ReconCopyMB;
				}
			}
BframeMC_MT:
#if ENABLE_DEBLOCK
			if (pDec->nVOPType == VO_VIDEO_FRAME_B)
				pDec->pOutFrame->pQuant[nYMBPos * pDec->nMBWidth + nXMBPos] = pDec->nQuant;
			else
				pDec->pCurFrame->pQuant[nYMBPos * pDec->nMBWidth + nXMBPos] = pDec->nQuant;

#endif
			*(vDecLocal->pMBQuant++) = pDec->nQuant;

#if ENABLE_ERROR_CONCEAL
			if(pDec->nMBError){
				pDec->nFrameError++;
				if (pDec->nVOPType == VO_VIDEO_FRAME_I && (pDec->nMBError& CONCEAL_ERROR))
				{
					*fpReconMB = ReconCopyMB;
				}
			}
#endif

// #if ENABLE_MULTI_THREAD
			//condition of continue parser
			++vopCtxt->nMBCount;
			if ((vopCtxt->nMBCount < pDec->nMpeg4ThdMaxMB && pDec->nWaiter == 0) || vopCtxt->nMBCount < pDec->nMpeg4ThdMinMB )
			{
				//parser can go on
				++fpReconMB;
				++gmcmb;
				++bFieldDct;
				++pMBPos;
				vDecLocal->scanType += 6;
				vDecLocal->blockptr += 6*64;
				vDecLocal->blockLen += 6;
				vDecLocal->MVBack = BckMV += 6;
				vDecLocal->MVFwd  = FwdMV += 6;
			}
			else
			{
// #if ENABLE_MULTI_THREAD
					pDec->nParserErr = VO_ERR_NONE;
					pDec->nParserMBPos = nMBPos;
					pDec->pParserMBQuant = vDecLocal->pMBQuant;
					if (pDec->nVOPType == VO_VIDEO_FRAME_B)
					{
						pDec->vfprev0 = fprev0;
						pDec->vfprev1 = fprev1;
						pDec->vbprev0 = bprev0;
						pDec->vbprev1 = bprev1;
					}
#if USE_WHILE_FORVER
					pthread_mutex_lock(&(pDec->parserMutex));
					pDec->nParser = 0;
					pthread_mutex_unlock(&(pDec->parserMutex));
#else
					pthread_mutex_lock(&(pDec->parserMutex));
					pDec->nParser = 0;
					if (pDec->nWaiter)
					{
						pthread_mutex_unlock(&(pDec->parserMutex));
						pthread_cond_signal(&(pDec->parserCond));
					}
					else
					{
						pthread_mutex_unlock(&(pDec->parserMutex));
					}
#endif
// #endif
// printf("%d\n",vopCtxt.nMBCount);

				fpReconMB = vopCtxt->fpReconMB;
				pMBPos = vopCtxt->pMBPos;
				gmcmb = vopCtxt->gmcmb;
				bFieldDct = vopCtxt->bFieldDct;
				vDecLocal->scanType = vopCtxt->scanType;
				vDecLocal->blockptr = vopCtxt->blockptr;
				vDecLocal->blockLen = vopCtxt->blockLen;
				vDecLocal->MVBack = vopCtxt->MVBack;
				vDecLocal->MVFwd = vopCtxt->MVFwd;
				do 
				{
					vDecLocal->gmcmb = *gmcmb++;
					vDecLocal->bFieldDct = *bFieldDct++;
					(*fpReconMB++)(pDec, vDecLocal, *pMBPos++);
					vDecLocal->scanType += 6;
					vDecLocal->blockptr += 6*64;
					vDecLocal->blockLen += 6;
					vDecLocal->MVBack += 6;
					vDecLocal->MVFwd  += 6;
				} while (--vopCtxt->nMBCount);

				fpReconMB = vopCtxt->fpReconMB;
				pMBPos = vopCtxt->pMBPos;
				gmcmb = vopCtxt->gmcmb;
				bFieldDct = vopCtxt->bFieldDct;
				vDecLocal->scanType = vopCtxt->scanType;
				vDecLocal->blockptr = vopCtxt->blockptr;
				vDecLocal->blockLen = vopCtxt->blockLen;
				BckMV = vDecLocal->MVBack = vopCtxt->MVBack;
				FwdMV = vDecLocal->MVFwd = vopCtxt->MVFwd;

// #if ENABLE_MULTI_THREAD
					//wait & update (nMBPos, mv) pMBQuant?
#if USE_WHILE_FORVER
				WaitForParser(pDec);
#else
				pthread_mutex_lock(&(pDec->parserMutex));
				if (pDec->nParser)
				{
					++pDec->nWaiter;
					do 
					{
						pthread_cond_wait(&(pDec->parserCond), &(pDec->parserMutex));
					} while (pDec->nParser);
					--pDec->nWaiter;
				}
				pDec->nParser = 1;
				pthread_mutex_unlock(&(pDec->parserMutex));
#endif

					if (pDec->nParserErr != VO_ERR_NONE)
					{
#if USE_WHILE_FORVER
						pthread_mutex_lock(&(pDec->parserMutex));
						pDec->nParser = 0;
						pthread_mutex_unlock(&(pDec->parserMutex));
#else
						pthread_mutex_lock(&(pDec->parserMutex));
						pDec->nParser = 0;
						if (pDec->nWaiter)
						{
							pthread_mutex_unlock(&(pDec->parserMutex));
							pthread_cond_signal(&(pDec->parserCond));
						}
						else
						{
							pthread_mutex_unlock(&(pDec->parserMutex));
						}
#endif
						return pDec->nParserErr;
					}

					nMBPos = pDec->nParserMBPos;
					vDecLocal->pMBQuant = pDec->pParserMBQuant;
					if (pDec->nVOPType == VO_VIDEO_FRAME_B)
					{
						fprev0 = pDec->vfprev0;
						fprev1 = pDec->vfprev1;
						bprev0 = pDec->vbprev0;
						bprev1 = pDec->vbprev1;
					}
					mv = &pDec->mv_buf[(nMBPos & pDec->mv_bufmask)*4];
// #endif
			}
		}
		if (pDec->nVOPType == VO_VIDEO_FRAME_B)
		{
			fprev1 = fprev0 = 0;
			bprev1 = bprev0 = 0;
		}
		vDecLocal->pMBQuant = pDec->quantrow;
	}

// #if ENABLE_MULTI_THREAD
		pDec->nParserErr = VO_ERR_NONE;
		pDec->nParserMBPos = nMBPos - MB_X + pDec->nMBWidth - 1;	//let other threads finish the MB scanning
#if USE_WHILE_FORVER
		pthread_mutex_lock(&(pDec->parserMutex));
		pDec->nParser = 0;
		pthread_mutex_unlock(&(pDec->parserMutex));
#else
		pthread_mutex_lock(&(pDec->parserMutex));
		pDec->nParser = 0;
		if (pDec->nWaiter)
		{
			pthread_mutex_unlock(&(pDec->parserMutex));
			pthread_cond_signal(&(pDec->parserCond));
		}
		else
		{
			pthread_mutex_unlock(&(pDec->parserMutex));
		}
#endif
// #endif
	if (vopCtxt->nMBCount > 0)
	{
		fpReconMB = vopCtxt->fpReconMB;
		pMBPos = vopCtxt->pMBPos;
		gmcmb = vopCtxt->gmcmb;
		bFieldDct = vopCtxt->bFieldDct;
		vDecLocal->scanType = vopCtxt->scanType;
		vDecLocal->blockptr = vopCtxt->blockptr;
		vDecLocal->blockLen = vopCtxt->blockLen;
		vDecLocal->MVBack = vopCtxt->MVBack;
		vDecLocal->MVFwd = vopCtxt->MVFwd;
		do
		{
			vDecLocal->gmcmb = *gmcmb++;
			vDecLocal->bFieldDct = *bFieldDct++;
			(*fpReconMB++)(pDec, vDecLocal, *pMBPos++);
			vDecLocal->scanType += 6;
			vDecLocal->blockptr += 6*64;
			vDecLocal->blockLen += 6;
			vDecLocal->MVBack += 6;
			vDecLocal->MVFwd  += 6;
		}while (--vopCtxt->nMBCount);
	}
	return VO_ERR_NONE;
}

