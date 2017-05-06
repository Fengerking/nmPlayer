/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
#include"voMpeg4Dec.h"
#include "voMpeg4Parser.h"
#include "voMpeg4Frame.h"
#include "voMpeg4Haff.h"
#include "voMpegBuf.h"
#include "voMpeg4Decoder.h"
#include "voMpegMem.h"
#include "voMpegIdct.h"
#include "voMpegMC.h"
#include "voMpeg4Postprocess.h"
#include "voMpeg4Rvlc.h"
#include <stdio.h>
#ifdef VODIVX3
#include "voDivx3Tab.h"
#endif

#if ENABLE_TIME_ST

double start0 = 0,start1 = 0,start2 = 0,start3 = 0,end0,end1,end2,end3;
double tST0 = 0, tST1 = 0, tST2 = 0, tST3 = 0;

#endif

// VO_U32 pDec->nMpeg4Thd = 1;
// VO_S32 pDec->nMpeg4ThdMinMB = 20;
// VO_S32 pDec->nMpeg4ThdMaxMB = 4;

void GetInnerMemSize(VO_VIDEO_INNER_MEM * pInnerMem, VO_U32 nMemConfig, VO_U32 nFrameNum)
{
	VO_S32 nMemSize = 0;
	VO_S32 nWidth, nHeight, nMBXSize, nMBYSize, nMBEnd, nMBCount;

	pInnerMem->nNumRefFrames = nFrameNum;
	nWidth	= (pInnerMem->nPictureWidth+15)&0xfffffff0;
	nHeight = (pInnerMem->nPictureHeight+15)&0xfffffff0;

	nMBXSize = nWidth / 16;
	nMBYSize = nHeight / 16;	

	pInnerMem->nPictureEXWidth = nWidth + EDGE_SIZE*2; 
	pInnerMem->nPictureEXHeight = nHeight + EDGE_SIZE*2;

	nMBEnd = nMBYSize * MB_X;
	nMBCount = nMBXSize * nMBYSize;	


	if(nMemConfig & INNER_MEM_FRONT_HANDLE){
		nMemSize += sizeof(VO_MPEG4DEC_FRONT) + CACHE_LINE + SAFETY;
	}

	if(nMemConfig & INNER_MEM_DEC_HANDLE){
		nMemSize += sizeof(VO_MPEG4_DEC) + CACHE_LINE + SAFETY; 
	}
	/* mv buffer*/
//  #ifdef VODIVX3	
//  	/* row buffer*/
//  	nMemSize += sizeof(VO_S32) * (4 * (MB_X+1) + 32/sizeof(VO_S32)) + CACHE_LINE + SAFETY;
//  #else
	nMemSize += 4 * sizeof(VO_S32) * (nMBEnd + 1) + CACHE_LINE + SAFETY;
// #endif
	/* bFieldPred_buf*/
	nMemSize += sizeof(VO_S8) * nMBEnd + CACHE_LINE + SAFETY;


//////////////////////////////////////////////////////////////////////////ENABLE_MULTI_THREAD
// 	/* bFieldDct*/
// 	nMemSize += (MAX_MB_TASK_NUM * THREAD_NUM + CACHE_LINE) + SAFETY;
// 
// 	/* gmcmb*/
// 	nMemSize += (MAX_MB_TASK_NUM * THREAD_NUM + CACHE_LINE) + SAFETY;
// 
// 	/* MVBack*/
// 	nMemSize += (6 * MAX_MB_TASK_NUM * THREAD_NUM * sizeof(VO_S32) + CACHE_LINE) + SAFETY;
// 
// 	/* MVFwd*/
// 	nMemSize += (6 * MAX_MB_TASK_NUM * THREAD_NUM * sizeof(VO_S32) + CACHE_LINE) + SAFETY;
// 
// 	/* fpReconMB*/
// 	nMemSize += (MAX_MB_TASK_NUM * THREAD_NUM * sizeof(FuncReconMB *) + CACHE_LINE) + SAFETY;
// 
// 	/* scanType */
// 	nMemSize += (6 * MAX_MB_TASK_NUM * THREAD_NUM + CACHE_LINE) + SAFETY;
// 
// 	/* blockLen */
// 	nMemSize += (6 * MAX_MB_TASK_NUM * THREAD_NUM + CACHE_LINE) + SAFETY;
// 
// 	/* blockptr */
// 	nMemSize += (6 * 64 * sizeof(idct_t) * MAX_MB_TASK_NUM * THREAD_NUM + CACHE_LINE) + SAFETY;
// 
// 	/* pMBPos*/
// 	nMemSize += (MAX_MB_TASK_NUM * THREAD_NUM * sizeof(VO_S32) + CACHE_LINE) + SAFETY;
// 
// 	/* mc block*/
// 	nMemSize += BLOCK_SIZE * BLOCK_SIZE * THREAD_NUM * sizeof(VO_U8) + CACHE_LINE + SAFETY;

#if ENABLE_DEBLOCK
	/*quant_map*/
	nMemSize += nFrameNum*(sizeof(VO_S32) * nMBCount + CACHE_LINE + SAFETY);
#endif
#if  ENABLE_MBINFO
	/*dec->mbinfo[0]*/
	MemSize += sizeof(VO_S8)*(nMBWidth * nMBHeight) + CACHE_LINE + SAFETY;
	/*dec->mbinfo[1]*/
	MemSize += sizeof(VO_S8)*(nMBWidth * nMBHeight) + CACHE_LINE + SAFETY;
#endif

#ifndef VODIVX3
	/*data partition*/
	if(nMemConfig & INNER_MEM_DATA_PARTITION){
		/*dec->DP_Buffer*/
		nMemSize += sizeof(DP_BUFFER) + CACHE_LINE + SAFETY;
		/*dec->DP_Buffer->ac_pred_flag*/
		nMemSize += sizeof(VO_S8)*nMBCount + CACHE_LINE + SAFETY;
		/*dec->DP_Buffer->cbpy*/
		nMemSize += sizeof(VO_S8)*nMBCount + CACHE_LINE + SAFETY;
		/*dec->DP_Buffer->quant*/
		nMemSize += sizeof(VO_S8)*nMBCount + CACHE_LINE + SAFETY;
		/*dec->DP_Buffer->mcbpc*/
		nMemSize += sizeof(VO_S16)*nMBCount*2 + CACHE_LINE + SAFETY;
		/*dec->DP_Buffer->bNotCoded*/
		nMemSize += sizeof(VO_S8)*nMBCount + CACHE_LINE + SAFETY;
		/*dec->DP_Buffer->dct_dc_diff*/
		nMemSize += nMBCount*sizeof(BLOCK_32BIT) + CACHE_LINE + SAFETY;
	}
#endif

	pInnerMem->nNumRefFrames = nFrameNum;

	pInnerMem->nPrivateMemSize = nMemSize + SAFETY;
}



static VO_U32 AllocDecBuf(VO_MPEG4_DEC *pDec)
{
	VO_U32 i, nPrivateMemSize, nMemSize, nFrameNum, nFrameSize;
	VO_BOOL bValidFrameBuf = VO_FALSE;
	VO_VIDEO_INNER_MEM vInnerMem;


	vInnerMem.nPictureWidth = pDec->nWidth;
	vInnerMem.nPictureHeight = pDec->nHeight;
	GetInnerMemSize(&vInnerMem, pDec->nMemConfig, pDec->nFrameBufNum);	
	
	nMemSize = nPrivateMemSize = vInnerMem.nPrivateMemSize;
	nFrameNum = vInnerMem.nNumRefFrames;
	nFrameSize = vInnerMem.nPictureEXWidth * vInnerMem.nPictureEXHeight;

	/* frame memory*/
	if(pDec->vMemSever.pFrameMemOp){/* Shared frame memory*/
		VO_MEM_VIDEO_INFO vFrameMemInfo;
		VO_MEM_VIDEO_OPERATOR *pFrameMemOp = pDec->vMemSever.pFrameMemOp;

		bValidFrameBuf = VO_TRUE;

		vFrameMemInfo.ColorType = VO_COLOR_YUV_PLANAR420;
		vFrameMemInfo.FrameCount = nFrameNum;
		vFrameMemInfo.Stride = vInnerMem.nPictureEXWidth;
		vFrameMemInfo.Height = vInnerMem.nPictureEXHeight;


		if(pFrameMemOp->Init(pDec->vMemSever.nCodecIdx, &vFrameMemInfo)) 
			return VO_ERR_OUTOF_MEMORY;

		/* reset stride for shared frame buffer*/
		pDec->nLumEXWidth = vFrameMemInfo.VBuffer[0].Stride[0];
		pDec->nChrEXWidth = vFrameMemInfo.VBuffer[0].Stride[1];

		for(i = 0; i < nFrameNum; i++){
			if(!vFrameMemInfo.VBuffer[i].Buffer[0] || \
				!vFrameMemInfo.VBuffer[i].Buffer[1] || \
				!vFrameMemInfo.VBuffer[i].Buffer[2] )
				return VO_ERR_OUTOF_MEMORY;
			pDec->frameBuffer[i].y = vFrameMemInfo.VBuffer[i].Buffer[0];
			pDec->frameBuffer[i].u = vFrameMemInfo.VBuffer[i].Buffer[1];
			pDec->frameBuffer[i].v = vFrameMemInfo.VBuffer[i].Buffer[2];
		}
	}else if(pDec->vMemSever.pMemOp){/* pre-alloced memory*/
		VO_CODEC_INIT_USERDATA *pMemOp = pDec->vMemSever.pMemOp;

		if(pMemOp->memflag == VO_IMF_PREALLOCATEDBUFFER){
			VO_VIDEO_INNER_MEM *pInnerMem = (VO_VIDEO_INNER_MEM *)pMemOp->memData;

			//Added by Harry
			nFrameNum = pInnerMem->nNumRefFrames;
			if (nFrameNum == 0)
			{
				return VO_ERR_OUTOF_MEMORY;
			}
			if (nFrameNum > MAXFRAMES)
			{
				nFrameNum = MAXFRAMES;
			}
			pDec->nFrameBufNum = nFrameNum;
			//end of add

			bValidFrameBuf = VO_TRUE;

			for(i = 0; i < nFrameNum; i++){
				if(!pInnerMem->frameMem[i][0] || \
					!pInnerMem->frameMem[i][1] || \
					!pInnerMem->frameMem[i][2] )
					return VO_ERR_OUTOF_MEMORY;
				pDec->frameBuffer[i].y = pInnerMem->frameMem[i][0];
				pDec->frameBuffer[i].u = pInnerMem->frameMem[i][1];
				pDec->frameBuffer[i].v = pInnerMem->frameMem[i][2];
			}
		}
	}


	if(bValidFrameBuf == VO_FALSE)
		nMemSize = nPrivateMemSize + (nFrameNum * (nFrameSize * 3 / 2) + CACHE_LINE);


	pDec->pMemBase = (VO_U8 *)MallocMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, nMemSize, CACHE_LINE);

	if(!pDec->pMemBase)
		return VO_ERR_OUTOF_MEMORY;

	SetMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, pDec->pMemBase, 0, nPrivateMemSize);

	if(!bValidFrameBuf){
		VO_U8 *pFrameMem = pDec->pMemBase + nPrivateMemSize;

		SetMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, pFrameMem, 128, (nFrameNum * (nFrameSize * 3 / 2) + CACHE_LINE));

		ALIGN_MEM(pFrameMem);

		for(i = 0; i < nFrameNum; i++){
			pDec->frameBuffer[i].y = pFrameMem;
			pFrameMem += nFrameSize;
			pDec->frameBuffer[i].u = pFrameMem;
			pFrameMem += (nFrameSize/4);
			pDec->frameBuffer[i].v = pFrameMem;
			pFrameMem += (nFrameSize/4);
		}
	}
	return VO_ERR_NONE;
}

static VO_U32 FreeDecBuf(VO_MPEG4_DEC *pDec)
{

	/* frame memory*/
	if(pDec->vMemSever.pFrameMemOp){/* Shared frame memory*/

		VO_MEM_VIDEO_OPERATOR *pFrameMemOp = pDec->vMemSever.pFrameMemOp;
		if(pFrameMemOp->Uninit(pDec->vMemSever.nCodecIdx))
			return VO_ERR_INVALID_ARG;

	}
	
	FreeMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, pDec->pMemBase);

	pDec->pMemBase = NULL;


	return VO_ERR_NONE;
}

static void IniDecBuf(VO_MPEG4_DEC *pDec)
{
	/* initialize memory*/	
	VO_U8 *pMemBase = pDec->pMemBase;
	VO_U32 i;
	VO_S32 nLumOffset, nChrOffset;
	VO_S32 nMBCount = pDec->nMBWidth*pDec->nMBHeight;
#ifdef VODIVX3	
// 	ALIGN_MEM(pMemBase);
// 	/* row buffer*/
// 	pDec->mv_bufmask = MB_X-1; //only one row of mv buffer needed
// 	pDec->mv_buf = (VO_S32*)pMemBase;
// 	pMemBase +=	sizeof(VO_S32)*(4 * (MB_X+1) + 32/sizeof(VO_S32)) + SAFETY;
// 	ALIGN_MEM(pMemBase);
	pDec->rlmax = c_rlmax;
	pDec->rl_table[0] = vlctab_r0;
	pDec->rl_table[1] = vlctab_r1;
	pDec->rl_table[2] = vlctab_r2;
	pDec->rl_table[3] = vlctab_r3;
	pDec->rl_table[4] = vlctab_r4;
	pDec->rl_table[5] = vlctab_r5;
	pDec->mv_table[0] = vlctab_mv0;
	pDec->mv_table[1] = vlctab_mv1;
	pDec->dc_lum_table[0] = vlctab_lum0;
	pDec->dc_lum_table[1] = vlctab_lum1;
	pDec->dc_chr_table[0] = vlctab_chr0;
	pDec->dc_chr_table[1] = vlctab_chr1;
	pDec->mb_intra_vlc = vlctab_intra;
	pDec->mb_inter_vlc = vlctab_inter;
#endif
	ALIGN_MEM(pMemBase);
	pDec->mv_bufmask = ~0;
	pDec->mv_buf = (VO_S32*)pMemBase;
	pMemBase += (4*sizeof(VO_S32) * (pDec->nPosEnd+1))+ SAFETY;
	pDec->bFieldPred_buf = (VO_S8*)pMemBase;
	pMemBase += (sizeof(VO_S8) * pDec->nPosEnd)+ SAFETY;

	//////////////////////////////////////////////////////////////////////////ENABLE_MULTI_THREAD
// 	/* bFieldDct*/
// 	ALIGN_MEM(pMemBase);
// 	pDec->bFieldDct = (VO_S8*)pMemBase;
// 	pMemBase += (MAX_MB_TASK_NUM * THREAD_NUM) + SAFETY;
// 
// 	/* gmcmb*/
// 	ALIGN_MEM(pMemBase);
// 	pDec->gmcmb = (VO_S8*)pMemBase;
// 	pMemBase += (MAX_MB_TASK_NUM * THREAD_NUM) + SAFETY;
// 
// 	/* MVBack*/
// 	ALIGN_MEM(pMemBase);
// 	pDec->MVBack = (VO_S32*)pMemBase;
// 	pMemBase += (6*MAX_MB_TASK_NUM * THREAD_NUM*sizeof(VO_S32)) + SAFETY;
// 
// 	/* MVFwd*/
// 	ALIGN_MEM(pMemBase);
// 	pDec->MVFwd = (VO_S32*)pMemBase;
// 	pMemBase += (6*MAX_MB_TASK_NUM * THREAD_NUM*sizeof(VO_S32)) + SAFETY;
// 
// 	/* fpReconMB*/
// 	ALIGN_MEM(pMemBase);
// 	pDec->fpReconMB = (FuncReconMB*)pMemBase;
// 	pMemBase += (MAX_MB_TASK_NUM * THREAD_NUM * sizeof(FuncReconMB*)) + SAFETY;
// 
// 	/* scanType */
// 	ALIGN_MEM(pMemBase);
// 	pDec->scanType = (VO_S8*)pMemBase;
// 	pMemBase += (6*MAX_MB_TASK_NUM * THREAD_NUM) + SAFETY;
// 
// 	/* blockLen */
// 	ALIGN_MEM(pMemBase);
// 	pDec->blockLen = (VO_S8*)pMemBase;
// 	pMemBase += (6*MAX_MB_TASK_NUM * THREAD_NUM) + SAFETY;
// 
// 	/* idct block */
// 	ALIGN_MEM(pMemBase);
// 	pDec->blockptr = (idct_t*)pMemBase;
// 	pMemBase += (6*64*sizeof(idct_t)*MAX_MB_TASK_NUM * THREAD_NUM) + SAFETY;
// 
// 	/* pMBPos*/
// 	ALIGN_MEM(pMemBase);
// 	pDec->pMBPos = (VO_S32*)pMemBase;
// 	pMemBase += (MAX_MB_TASK_NUM * THREAD_NUM * sizeof(VO_S32)) + SAFETY;
// 
// 	/* mc block*/
// 	ALIGN_MEM(pMemBase);
// 	pDec->mc_data = (VO_U8*)pMemBase;
// 	pMemBase += (BLOCK_SIZE*BLOCK_SIZE*THREAD_NUM*sizeof(VO_U8)) + SAFETY;

// #if ENABLE_MULTI_THREAD
// 	//every sub-thread need update buffer point
// 	SetMBThreadBuff(pDec);
// #endif

#if ENABLE_DEBLOCK
	/*quant_map*/
	for(i = 0; i < pDec->nFrameBufNum; i++){
		ALIGN_MEM(pMemBase);
		pDec->frameBuffer[i].pQuant = (VO_S32*)pMemBase;
		pMemBase += (sizeof(VO_S32) * nMBCount + SAFETY);
	}
#endif


	if(pDec->nMemConfig & INNER_MEM_DATA_PARTITION){

		ALIGN_MEM(pMemBase);

		pDec->DP_Buffer = (DP_BUFFER*)pMemBase;
		pMemBase += sizeof(DP_BUFFER) + SAFETY;
		pDec->DP_Buffer->nLastMBNum = -1;//TBD

		ALIGN_MEM(pMemBase);
		pDec->DP_Buffer->bACPred = (VO_U8*)pMemBase;
		pMemBase += (sizeof(VO_S8)*nMBCount) + SAFETY;

		ALIGN_MEM(pMemBase);
		pDec->DP_Buffer->cbpy = (VO_U8*)pMemBase;
		pMemBase += (sizeof(VO_S8)*nMBCount) + SAFETY;

		ALIGN_MEM(pMemBase);
		pDec->DP_Buffer->quant = (VO_U8*)pMemBase;
		pMemBase += (sizeof(VO_S8)*nMBCount) + SAFETY;

		ALIGN_MEM(pMemBase);
		pDec->DP_Buffer->mcbpc = (VO_S16*)pMemBase;
		pMemBase += (sizeof(VO_S16)*nMBCount*2) + SAFETY;

		ALIGN_MEM(pMemBase);
		pDec->DP_Buffer->bNotCoded = (VO_U8*)pMemBase;
		pMemBase += (sizeof(VO_S8)*nMBCount) + SAFETY;

		ALIGN_MEM(pMemBase);
		pDec->DP_Buffer->nDctDCDiff = (VO_S32(*)[6])pMemBase;
		//dec->out_mem += mbCount*sizeof(BLOCK_32BIT);
		pMemBase += (nMBCount*sizeof(VO_S32)*6) + SAFETY;
	}

	nLumOffset = (EDGE_SIZE * pDec->nLumEXWidth + EDGE_SIZE);
	nChrOffset = (EDGE_SIZE2 * pDec->nChrEXWidth + EDGE_SIZE2);

	pDec->vFrameBufFIFO.r_idx = pDec->vFrameBufFIFO.w_idx = 0;
	/* frame buffers*/
	for(i = 0; i < pDec->nFrameBufNum; i++){//TBD
		pDec->frameBuffer[i].y += nLumOffset;
		pDec->frameBuffer[i].u += nChrOffset;
		pDec->frameBuffer[i].v +=  nChrOffset;
		FrameBufCtl(&pDec->vFrameBufFIFO, &pDec->frameBuffer[i], FIFO_WRITE);

	}

	/*defalt frame buffer*/
	pDec->pRefFrame = FrameBufCtl(&pDec->vFrameBufFIFO, NULL, FIFO_READ);
	pDec->pCurFrame = FrameBufCtl(&pDec->vFrameBufFIFO, NULL, FIFO_READ);
}

VO_S32 ConfigDecoder(VO_MPEG4_DEC* pDec,  VO_S32 nWidth, VO_S32 nHeight)
{
	VO_S32 vRC = VO_ERR_NONE;

	nWidth = (nWidth+15)&0xfffffff0;
	nHeight = (nHeight+15)&0xfffffff0;

	if(((nWidth>>4)>MB_X) || ((nHeight>>4)>MB_Y))
		return VO_ERR_FAILED;

	if (pDec->nWidth != nWidth || pDec->nHeight != nHeight || (pDec->nMemConfig&INNER_MEM_DATA_PARTITION)){

		pDec->nWidth = nWidth;
		pDec->nHeight = nHeight;

		pDec->nMBWidth = (nWidth + 15) / 16; 
		pDec->nMBHeight = (nHeight + 15) / 16;
		pDec->nMBBits = _log2(pDec->nMBWidth*pDec->nMBHeight);
		pDec->nPosEnd = pDec->nMBHeight * MB_X;

		/* default stride*/
		pDec->nLumEXWidth = pDec->nMBWidth*16 + EDGE_SIZE*2;
		pDec->nChrEXWidth = (pDec->nLumEXWidth >> 1);
		pDec->nLumEXHeight = pDec->nMBHeight*16 + EDGE_SIZE*2;
		pDec->nChrEXHeight = (pDec->nLumEXHeight >> 1);

#ifdef _IOS
		pDec->nMpeg4ThdMinMB = pDec->nMBWidth*3/4;
		if(pDec->nMpeg4ThdMinMB > pDec->nMpeg4ThdMaxMB)
			pDec->nMpeg4ThdMinMB = pDec->nMpeg4ThdMaxMB;
#else
		if (pDec->nMpeg4ThdMinMB < pDec->nMBWidth/2)
		{
			pDec->nMpeg4ThdMinMB = pDec->nMBWidth/2;
		}
#endif
		FreeDecBuf(pDec);
		AllocDecBuf(pDec);
		IniDecBuf(pDec); 
	}

	return vRC;
}

static VO_U32 DecVOP( VO_MPEG4_DEC *pDec)
{
	VO_U32	voRC = VO_ERR_NONE;
	VO_S64 TRB,TRD;

	pDec->nResyncPos = 0;

	//if(pDec->nFrame == 20)
	//	pDec->nFrame = pDec->nFrame;

	switch (pDec->nVOPType){ 
	case VO_VIDEO_FRAME_NULL:
		return VO_ERR_NONE;
		break;
	case VO_VIDEO_FRAME_S:
	case VO_VIDEO_FRAME_P:
	case VO_VIDEO_FRAME_I:

		if(pDec->pCurFrame)/*retire current buffer*/
			FrameBufCtl(&pDec->vFrameBufFIFO, pDec->pCurFrame, FIFO_WRITE);
		if(!(pDec->pCurFrame = FrameBufCtl(&pDec->vFrameBufFIFO, NULL, FIFO_READ)))
			return VO_ERR_WRONG_STATUS;
		if(pDec->vMemSever.pFrameMemOp){/*shared frame buffer*/
			if(pDec->vMemSever.pFrameMemOp->GetBufByIndex(pDec->vMemSever.nCodecIdx, GetFrameBufIdx(pDec->pCurFrame, pDec->frameBuffer)))
				return VO_MEMRC_NO_YUV_BUFFER;
		}

		pDec->pCurFrame->nVOPType = pDec->nVOPType;
		pDec->pCurFrame->nTime = pDec->nTime;
		pDec->pCurFrame->pUserData = pDec->pUserData;

		if (pDec->nVOPType == VO_VIDEO_FRAME_I){
			voRC = pDec->fpDecIVOP( pDec );
		}else{
			voRC = pDec->fpDecPVOP( pDec );
		}
		if(voRC != VO_ERR_NONE)
			return voRC;

		SetImageEdge(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, pDec->pCurFrame, 
			pDec->nLumEXWidth, pDec->nChrEXWidth, pDec->nLumEXHeight, pDec->nWidth, pDec->nHeight);

		SwapImage(&pDec->pCurFrame, &pDec->pRefFrame);

		if(!pDec->nOutMode){
			pDec->pOutFrame = pDec->pCurFrame;
		}else{
			pDec->pOutFrame = pDec->pRefFrame;
		}

		break;
#ifdef ASP_BVOP
	case VO_VIDEO_FRAME_B:

#if ENABLE_VT
		return VO_ERR_FAILED;
#endif//ENABLE_VT
		TRB = pDec->time_pp - pDec->time_bp;
		TRD = pDec->time_pp;

		if (TRD){
			pDec->TRB = (TRB << 32) / TRD + 1;				//by Harry, 2011.11.18
			pDec->TRB_TRD = ((TRD - TRB) << 32) / TRD + 1;	//to calc "*" in positive, by Harry, 2011.11.18
		}
		if(!(pDec->pOutFrame = FrameBufCtl(&pDec->vFrameBufFIFO, NULL, FIFO_READ)))
			return VO_ERR_WRONG_STATUS;

		pDec->pOutFrame->nVOPType = pDec->nVOPType;
		pDec->pOutFrame->nTime = pDec->nTime;
		pDec->pOutFrame->pUserData = pDec->pUserData;

		voRC = pDec->fpDecBVOP( pDec );

		FrameBufCtl(&pDec->vFrameBufFIFO, pDec->pOutFrame, FIFO_WRITE);

		if(voRC != VO_ERR_NONE){
			return voRC;
		}
		break;
#endif//ASP_BVOP
	default:
		return VO_ERR_FAILED;
	}

	pDec->nFrame++;
	
	// is there a possible (uint8) frameMap overflow?
	if ((pDec->nFrame - pDec->nMapOfs) >= 128){ 
		VO_S32 pos;
		for (pos=0;pos<pDec->nPosEnd;pos+=MB_X-pDec->nMBWidth)
			for (;POSX(pos)<pDec->nMBWidth;++pos)
			{
				VO_S32 i = pDec->frameMap[pos];
				if (i >= (120<<1))
					i -= 120<<1;
				else
					i &= 1;
				pDec->frameMap[pos] = (VO_U8)i;
			}

		pDec->nMapOfs += 120;
	}
	return VO_ERR_NONE;
}


VO_U32 voMPEGDecFrame(VO_MPEG4_DEC *pDec, VO_CODECBUFFER *pInData, VO_VIDEO_BUFFER *pOutData,
						  VO_VIDEO_OUTPUTINFO *pOutPutInfo, const VO_U32 nHeaderFlag)
{
	/*TBD to calculate the frame size*/
	const VO_U8* pInBufStart = pInData->Buffer;
	VO_U32 voRC = VO_ERR_NONE;
	
	/* initilization for every frame*/
	pDec->nFrameError = 0;

	/* initilize bit stream*/
	InitBitStream(pDec, pInData->Buffer, pInData->Length);

	if (nHeaderFlag){/*only decode header data*/
		voRC = pDec->fpGetHdr(pDec, nHeaderFlag);
		CHECK_ERROR(voRC);
	
		if(pOutPutInfo){
			pOutPutInfo->Format.Width = pDec->nTrueWidth;
			pOutPutInfo->Format.Height = pDec->nTrueHeight;
			pOutPutInfo->Format.Type = pDec->nVOPType;
		}

		return VO_ERR_NONE; 
	}
#ifdef VODIVX3
	if(pDec->nCodecId == MSMPEG4_ID){
		pDec->nTrueWidth = pOutPutInfo->Format.Width;
		pDec->nTrueHeight = pOutPutInfo->Format.Height;
		
		voRC = ConfigDecoder(pDec, pOutPutInfo->Format.Width, pOutPutInfo->Format.Height);
		CHECK_ERROR(voRC);
		pDec->validvol = 1;
	}
#endif //VODIVX3

	pOutData->Buffer[0] = NULL;
	pOutData->Buffer[1] = NULL;
	pOutData->Buffer[2] = NULL;
	pOutData->Stride[0]	= 0;
	pOutData->Stride[1]	= 0;
	pOutData->Stride[2]	= 0;

	voRC = pDec->fpGetHdr(pDec, nHeaderFlag);
	CHECK_ERROR(voRC);

	if(!pDec->validvol)
		return VO_ERR_WRONG_STATUS;/* invalid header or no vol information*/

	if ((pDec->nFrame == 0 && pDec->nVOPType != VO_VIDEO_FRAME_I) || 
		(pDec->nLastRefFrame < 1 && pDec->nVOPType == VO_VIDEO_FRAME_B))
	{
		return VO_ERR_WRONG_STATUS;
	}

	pDec->nTime = pInData->Time;/* set time stamp*/
	pDec->pUserData = pInData->UserData;/* set user data*/
#if ENABLE_TIME_ST
	 MARKTIME_INIT(start0);
#endif

	voRC = DecVOP(pDec);

#if ENABLE_TIME_ST
	 MARKTIME(end0);
	 tST0 += end0-start0;
#endif

	if(voRC == VO_ERR_NONE){
		if(EofBits(pDec))
			return VO_ERR_INPUT_BUFFER_SMALL;

		if(pDec->nFrame == 1&&!pDec->nOutMode){/*delay first frame*/
			pOutPutInfo->Format.Type = VO_VIDEO_FRAME_NULL;
			pOutData->ColorType = VO_COLOR_YUV_PLANAR420;
		}else{
			VO_IMGYUV	*pOutFrame = pDec->pOutFrame;

			if(pDec->nVOPType == VO_VIDEO_FRAME_NULL){
				pOutPutInfo->Format.Type = VO_VIDEO_FRAME_NULL;
				pOutData->ColorType = VO_COLOR_YUV_PLANAR420;
			}else{

#if ENABLE_DEBLOCK

				image_postproc(&pDec->postproc, pOutFrame, pDec->nLumEXWidth, \
					pOutFrame->pQuant, pDec->nMBWidth, pDec->nMBHeight, pDec->nMBWidth,\
					0, 3);//TBD
#endif
				pOutData->Buffer[0] = pOutFrame->y;
				pOutData->Buffer[1] = pOutFrame->u;
				pOutData->Buffer[2] = pOutFrame->v;
				pOutData->Stride[0]	= pDec->nLumEXWidth;
				pOutData->Stride[1]	= pDec->nChrEXWidth;
				pOutData->Stride[2]	= pDec->nChrEXWidth;
				pOutData->ColorType = VO_COLOR_YUV_PLANAR420;
				pOutData->Time = pOutFrame->nTime;
				pOutData->UserData = pOutFrame->pUserData;

				pOutPutInfo->Format.Width = pDec->nTrueWidth;
				pOutPutInfo->Format.Height = pDec->nTrueHeight;
				pOutPutInfo->Format.Type = pOutFrame->nVOPType;				/* 0-Iframe, 1-Pframe, 2-pOutFrame*/

#if 0	//for QA dump
				{
					unsigned char* out_src;
					int i, width,height;

					FILE *fp;

					fp = fopen("/sdcard/YUV/dump_output.yuv", "ab");
					if (fp != NULL)
					{
						width = pDec->nTrueWidth;
						height= pDec->nTrueHeight;
						/* output decoded YUV420 frame */
						/* Y */
						out_src = pOutFrame->y;
						for(i = 0;i < height; i++){
							fwrite(out_src, width, 1, fp);
							out_src += pDec->nLumEXWidth;
						}

						/* U */
						out_src = pOutFrame->u;
						for(i = 0; i< height/2; i++){
							fwrite(out_src, width/2, 1, fp);
							out_src += pDec->nChrEXWidth;
						}

						/* V */
						out_src = pOutFrame->v;
						for(i = 0; i < height/2; i++){
							fwrite(out_src, width/2, 1, fp);
							out_src += pDec->nChrEXWidth;
						}
						
						fclose(fp);
					}
				}
#endif

			}
		}


		/* calculate the frame size*/
//		pOutPutInfo->InputUsed = (VO_S32)(pDec->pBitPtr - pInBufStart) - 4 + ((pDec->nBitPos + 7)>>3);
		pOutPutInfo->InputUsed = (VO_S32)(bytepos(pDec) - pInBufStart);		//------------------yh------------

	}

	return voRC;
}


/*create back decoder structure
* and some initilization
*/
VO_U32 voMPEG4DecCreate(void **pHandle, VO_CODEC_INIT_USERDATA * pUserData, VO_U32 nCodecId)
{
	VO_MPEG4_DEC* pDec = NULL;
	VO_S32 i;

	/* malloc back decoder structure*/
	pDec = (VO_MPEG4_DEC*) MallocMem(pUserData, nCodecId, sizeof(VO_MPEG4_DEC), CACHE_LINE);
	if(!pDec)
		return VO_ERR_OUTOF_MEMORY;

	SetMem(pUserData, nCodecId, (VO_U8*)pDec, 0, sizeof(VO_MPEG4_DEC));

	/* memory operation*/
	pDec->vMemSever.pMemOp = pUserData;
	pDec->vMemSever.nCodecIdx = nCodecId;

	/* global initilization*/
	pDec->nFrameBufNum = FRAMEDEFAULT;
	/* initilize acdc*/
	for (i = 0; i < 4*MB_X*2; i++)
		pDec->dc_lum[i] = 1024;
	for (i = 0; i < 2*MB_X; i++){ 
		pDec->dc_chr[0][i] = 1024;
		pDec->dc_chr[1][i] = 1024;
	}

	if(nCodecId == VO_INDEX_DEC_MPEG4){
		pDec->nCodecId = MPEG4_ID;
		pDec->fpGetHdr = MPEG4GetHdr;
#ifdef ASP_BVOP
// 		if (pDec->nMpeg4Thd > 1)
// 		{
// 			pDec->fpDecIVOP = MPEG4DecVOP;
// 			pDec->fpDecPVOP = MPEG4DecVOP;
// 			pDec->fpDecBVOP = MPEG4DecVOP;
// 		}
// 		else
// 		{
			pDec->fpDecIVOP = MPEG4DecVOP_s;
			pDec->fpDecPVOP = MPEG4DecVOP_s;
			pDec->fpDecBVOP = MPEG4DecVOP_s;
// 		}
#else
		pDec->fpDecIVOP = MPEG4DecIVOP;
		pDec->fpDecPVOP = MPEG4DecPVOP;
#endif
		pDec->vld_block = Mpeg4GetBlockVld;
	}else if(nCodecId == VO_INDEX_DEC_H263){
		pDec->nCodecId = H263_ID;
		pDec->fpDecIVOP = H263DecIVOP;
		pDec->fpDecPVOP = H263DecPVOP;
		pDec->fpDecBVOP = H263DecBVOP;
		pDec->fpGetHdr = H263GetHdr;
		pDec->chroma_qscale_table = (VO_U8*)default_h263_chroma_qscale_table;
	}else if(nCodecId == VO_INDEX_DEC_S263){
		pDec->nCodecId = S263_ID;
		pDec->fpDecIVOP = H263DecIVOP;
		pDec->fpDecPVOP = H263DecPVOP;
		pDec->fpDecBVOP = H263DecBVOP;
		pDec->fpGetHdr = S263GetHdr;
		pDec->chroma_qscale_table = (VO_U8*)default_h263_chroma_qscale_table;
	}
#ifdef VODIVX3
	else if(nCodecId == VO_INDEX_DEC_DIVX3){
		pDec->nCodecId = MSMPEG4_ID;
		pDec->fpGetHdr = DIVX3GetHdr;
		pDec->fpDecIVOP = DIVX3DecIVOP;
		pDec->fpDecPVOP = DIVX3DecPVOP;
		pDec->fpDecBVOP = DIVX3DecBVOP;
	}
#endif
	else{
		return VO_ERR_INVALID_ARG;
	}
	
	*pHandle = pDec;/*return back decoder handle*/

	pDec->nParserMBPos = -1;
	pDec->nMpeg4Thd = 1;
	pDec->nMpeg4ThdMinMB = 20;
	pDec->nMpeg4ThdMaxMB = 4;

	//vop context size
	pDec->nVopCtxtSize = 0;

	//bFieldDct
	pDec->nVopCtxtSize += pDec->nMpeg4ThdMaxMB;

	//gmcmb
	pDec->nVopCtxtSize += pDec->nMpeg4ThdMaxMB;

	pDec->nVopCtxtSize = (pDec->nVopCtxtSize + 3)&(~3);

	//MVBack
	pDec->nVopCtxtSize += pDec->nMpeg4ThdMaxMB*6*sizeof(VO_S32);

	//MVFwd
	pDec->nVopCtxtSize += pDec->nMpeg4ThdMaxMB*6*sizeof(VO_S32);

	//fpReconMB
	pDec->nVopCtxtSize += pDec->nMpeg4ThdMaxMB*sizeof(FuncReconMB);

	//scanType
	pDec->nVopCtxtSize += pDec->nMpeg4ThdMaxMB*6;

	//blockLen
	pDec->nVopCtxtSize += pDec->nMpeg4ThdMaxMB*6;

	//blockptr
	pDec->nVopCtxtSize += pDec->nMpeg4ThdMaxMB*6*64*sizeof(VO_S16);

	//pMBPos
	pDec->nVopCtxtSize += pDec->nMpeg4ThdMaxMB*sizeof(VO_S32);

	//mc_data
	pDec->nVopCtxtSize += BLOCK_SIZE*BLOCK_SIZE;

	pDec->vopCtxt.bFieldDct = (VO_S8*) MallocMem(pUserData, nCodecId, pDec->nVopCtxtSize, CACHE_LINE);
	if(!pDec->vopCtxt.bFieldDct)
		return VO_ERR_OUTOF_MEMORY;

	SetMem(pUserData, nCodecId, (VO_U8*)pDec->vopCtxt.bFieldDct, 0, pDec->nVopCtxtSize);
	pDec->vopCtxt.gmcmb = pDec->vopCtxt.bFieldDct + pDec->nMpeg4ThdMaxMB;
	pDec->vopCtxt.MVBack = (VO_S32 *)(((VO_U32)(pDec->vopCtxt.gmcmb + pDec->nMpeg4ThdMaxMB + 3))&(~3));
	pDec->vopCtxt.MVFwd = pDec->vopCtxt.MVBack + pDec->nMpeg4ThdMaxMB*6;
	pDec->vopCtxt.fpReconMB = (FuncReconMB *)(pDec->vopCtxt.MVFwd + pDec->nMpeg4ThdMaxMB*6);
	pDec->vopCtxt.scanType = (VO_S8 *)(pDec->vopCtxt.fpReconMB + pDec->nMpeg4ThdMaxMB);
	pDec->vopCtxt.blockLen = pDec->vopCtxt.scanType + pDec->nMpeg4ThdMaxMB*6;
	pDec->vopCtxt.blockptr = (VO_S16 *)(pDec->vopCtxt.blockLen + pDec->nMpeg4ThdMaxMB*6);
	pDec->vopCtxt.pMBPos = (VO_S32 *)(pDec->vopCtxt.blockptr + pDec->nMpeg4ThdMaxMB*6*64);
	pDec->vopCtxt.mc_data = (VO_U8 *)(pDec->vopCtxt.pMBPos + pDec->nMpeg4ThdMaxMB);

// 	if (pDec->nMpeg4Thd > 1)
// 	{
// 		if (InitialMBThread(pDec))
// 		{
// 			return VO_ERR_INVALID_ARG;
// 		}
// 	}

// #if ENABLE_MULTI_THREAD
// 	if (InitialMBThread(pDec))
// 	{
// 		return VO_ERR_INVALID_ARG;
// 	}
// #endif

	return VO_ERR_NONE;
}


VO_U32 voMPEG4DecFinish(VO_MPEG4_DEC* pDec)
{
	if (pDec->nMpeg4Thd > 1)
	{
		ReleaseMBThread(pDec);
	}
// #if ENABLE_MULTI_THREAD
// 	ReleaseMBThread(pDec);
// #endif
	FreeMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, pDec->vopCtxt.bFieldDct);

	/* Free private memory*/
	FreeDecBuf(pDec);
	/* Free decoder handle*/
	FreeMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, pDec);

	return VO_ERR_NONE;
}



