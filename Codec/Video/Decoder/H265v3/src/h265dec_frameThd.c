#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "h265_decoder.h"
#include "h265dec_dequant.h"


#if USE_FRAME_THREAD

extern VO_VOID xWriteOutput(H265_DEC * const p_dec, H265_DPB_LIST* pcListPic, VO_S32 reorder_num );


extern VO_S32 PrepareDecodeFrame(H265_DEC* const p_dec, 
	H265_DEC_SLICE * const p_slice, 
	VO_U8** pp_inbuf, 
	VO_S32 *buf_len);

extern VO_S32 DoDecodeFrame(const H265_DEC* const p_dec, 
	H265_DEC_SLICE * const p_slice, 
	VO_U8* p_inbuf, 
	VO_S32 buf_len);

extern VO_U32 FreeSliceBuf( H265_DEC_SLICE *p_slice, 
	USER_OP_ALL* p_user_op_all );



#define CONDITION_JUDGE ( p->nReady_y < y && (p->nReady_y < nReady_y || p->nProgress < x) ) && !p->errFlag
inline VO_S32 WaitForProgress(TComPic * const p, const VO_S32 y, const VO_S32 nReady_y, const VO_S32 x)
{
#if defined(NDEBUG)
	THD_COND_WAIT(&p->progressCond, &p->progressMutex, CONDITION_JUDGE);
#else
	if (CONDITION_JUDGE)
	{
// 		printf("%d wait for %d:%d\n", pThdInfo->thread_id, p->m_iPOC, y);
		pthread_mutex_lock(&p->progressMutex);
		while(CONDITION_JUDGE)
		{
			pthread_cond_wait(&p->progressCond, &p->progressMutex);
		}
		pthread_mutex_unlock(&p->progressMutex);
// 		printf("%d end wait for %d, result %x\n", pThdInfo->thread_id, p->m_iPOC, p->errFlag);
	}
#endif

	return p->errFlag;
}
#undef CONDITION_JUDGE

#define CONDITION_JUDGE_MV p->nProgress_mv < addr && !p->errFlag
inline VO_S32 WaitForProgress_mv(TComPic * const p, const VO_S32 addr)
{
#if defined(NDEBUG)
	THD_COND_WAIT(&p->progressCond, &p->progressMutex, CONDITION_JUDGE_MV);
#else
	if (CONDITION_JUDGE_MV)
	{
// 		printf("%d wait for mv %d:%d, now %d\n", pThdInfo->thread_id, p->m_iPOC, addr, p->nProgress_mv);
		pthread_mutex_lock(&p->progressMutex);
		while(CONDITION_JUDGE_MV)
		{
			pthread_cond_wait(&p->progressCond, &p->progressMutex);
		}
		pthread_mutex_unlock(&p->progressMutex);
// 		printf("%d end wait for mv %d, result %x\n", pThdInfo->thread_id, p->m_iPOC, p->errFlag);
	}
#endif
	return p->errFlag;
}
#undef CONDITION_JUDGE_MV

inline VO_VOID ReportReady(const H265_DEC_SLICE* const p_slice, TComPic * const p, const VO_S32 nReady_y)
{
	pthread_mutex_lock(&p->progressMutex);
	p->nProgress = -100;
	p->nReady_y = nReady_y;
	pthread_cond_broadcast(&p->progressCond);
	WIN32_BROADCAST_OTHERS(&p->progressCond, p_slice->nThdNum);
	pthread_mutex_unlock(&p->progressMutex);
}


inline VO_VOID ReportProgress(const H265_DEC_SLICE* const p_slice, TComPic * const p, const VO_S32 y)
{
	pthread_mutex_lock(&p->progressMutex);
	p->nProgress = y;
	// 	LOGI("POC %d report progress y:%d\n", p->m_iPOC, y);

	pthread_cond_broadcast(&p->progressCond);
	WIN32_BROADCAST_OTHERS(&p->progressCond, p_slice->nThdNum);
	pthread_mutex_unlock(&p->progressMutex);
}

inline VO_VOID ReportProgress_mv(const H265_DEC_SLICE* const p_slice, TComPic * const p, const VO_S32 addr)
{
	pthread_mutex_lock(&p->progressMutex);
	p->nProgress_mv = addr;
	// printf("%d progress_mv %d\n", p->m_iPOC, addr);
	pthread_cond_broadcast(&p->progressCond);
	WIN32_BROADCAST_OTHERS(&p->progressCond, p_slice->nThdNum);
	pthread_mutex_unlock(&p->progressMutex);
}

//main-thread
VO_S32 DecodeFrameMain(H265_DEC *pDecGlobal, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutInfo, VO_S32 newInput)
{
	VO_S32 thdIdx = pDecGlobal->nextThdIdx;
	h265FrameThdInfo *pThdInfo = pDecGlobal->pFrameThdPool->thdInfo + thdIdx;
	H265_DEC_SLICE *p_slice_cur = pDecGlobal->p_avalable_slice;

	VO_S32 ret;

	switch (newInput)
	{
	case 0:
		//no new input data, just check if output
		GetDisPic(pDecGlobal, pOutput, pOutInfo);
		return VO_ERR_NONE;
		break;
	case 1:
		//new input data
#if USE_NEW_INPUT
	case 3:
		//only decode frame, but don't output anything
		//used for combined frame before flush command
// 		UpdateContextFromSub(pDecGlobal, pSubContext);
		break;
#endif
	case 2:
		//flush command from API
		{
			//just wait for all thread done
			VO_S32 i;
			for (i = 0; i < pDecGlobal->nThdNum; ++i)
			{
				h265FrameThdInfo *pThdInfo_t = pDecGlobal->pFrameThdPool->thdInfo+i;
				THD_COND_WAIT(pThdInfo_t->readyCond, pThdInfo_t->stateMutex, pThdInfo_t->state != FRAME_STATE_READY);
			}
			xWriteOutput(pDecGlobal, &pDecGlobal->dpb_list, 0 );//flush dis pics
		}
		return VO_ERR_NONE;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	//M1.prepare new frame
	{
		VO_U32 size = pDecGlobal->data_sz/INPUT_SIZE_INC_STEP;
		if (size >= p_slice_cur->frameDataSize)
		{
			if (p_slice_cur->pFrameData)
			{
				AlignFree(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, p_slice_cur->pFrameData);
			}
			p_slice_cur->pFrameData = (VO_U8 *)AlignMalloc(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, (++size)*INPUT_SIZE_INC_STEP, CACHE_LINE);
			if (p_slice_cur->pFrameData == NULL)
			{
				p_slice_cur->frameDataSize = 0;
				return VO_ERR_OUTOF_MEMORY;
			}
			p_slice_cur->frameDataSize = size;
//printf("increase thd %d input buf to %d\n", thdIdx, size);
		}
	}
	CopyMem(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, p_slice_cur->pFrameData, pDecGlobal->in_data, pDecGlobal->data_sz);
	p_slice_cur->in_data = p_slice_cur->pFrameData;
	p_slice_cur->data_sz = pDecGlobal->data_sz;
	p_slice_cur->timestamp = pDecGlobal->timestamp;
	p_slice_cur->fast_mode = pDecGlobal->fast_mode;
	p_slice_cur->pThdInfo = pThdInfo;

	if ((ret = PrepareDecodeFrame(pDecGlobal, p_slice_cur, &p_slice_cur->in_data, &p_slice_cur->data_sz)) != VO_ERR_NONE)
 	{
		return ret;
	}

	//M2.wait for pre-frame finish decoding
	THD_COND_WAIT(pThdInfo->readyCond, pThdInfo->stateMutex, pThdInfo->state != FRAME_STATE_READY);

	//M3.exchange slice point
	pDecGlobal->p_avalable_slice = pThdInfo->pSelfSlice;
	pThdInfo->pSelfSlice = p_slice_cur;

	//M4.signal sub-thread, start decoding
	pthread_mutex_lock(pThdInfo->stateMutex);
	pThdInfo->state = FRAME_STATE_DECODING;
	pthread_mutex_unlock(pThdInfo->stateMutex);
	pthread_cond_signal(pThdInfo->setupCond);

	//calc next thdIdx
	if (thdIdx < pDecGlobal->nThdNum-1)
	{
		pDecGlobal->nextThdIdx = ++thdIdx;
	}
	else
	{
		pDecGlobal->nextThdIdx = 0;
	}

	//M5.output a finished frame data
	if (newInput == 1)
	{
		GetDisPic(pDecGlobal, pOutput, pOutInfo);
	}

	return VO_ERR_NONE;
}

//sub-thread
static THREAD_FUNCTION H265DecThreadFunc(void *pArg)
{
	THREAD_FUNCTION_RETURN exitCode = 0;
	voH265ThreadParam *param = (voH265ThreadParam *)pArg;
	const H265_DEC * const pDecGlobal = param->pDecGlobal;
	h265FrameThdInfo *selfInfo = param->selfInfo;
	//VO_U32 nThdIdx = selfInfo - pDecGlobal->pFrameThdPool->thdInfo;
	H265_DEC_SLICE * p_slice;

	selfInfo->bRunning = 1;

	while (selfInfo->runCMD)
	{
		//S1.ready
		//S1.1 wait for main-thd submit task
#if defined(NDEBUG)
		THD_COND_WAIT(selfInfo->setupCond, selfInfo->stateMutex, selfInfo->state != FRAME_STATE_DECODING);
#else
		if (selfInfo->state != FRAME_STATE_DECODING)
		{
			pthread_mutex_lock(selfInfo->stateMutex);
			while (selfInfo->state != FRAME_STATE_DECODING)
			{
				pthread_cond_wait(selfInfo->setupCond, selfInfo->stateMutex);
			}
			pthread_mutex_unlock(selfInfo->stateMutex);
		}
#endif
		if (!selfInfo->runCMD)
		{
			break;
		}

		selfInfo->errFlag = 0;
		selfInfo->usedBytes = 0;
		p_slice = selfInfo->pSelfSlice;
// 		p_slice->pThdInfo = selfInfo;

		if ((selfInfo->errFlag = DoDecodeFrame(pDecGlobal, p_slice, p_slice->in_data, p_slice->data_sz)) != VO_ERR_NONE)
		{
			goto frameThdEndOfDecoding;
		}

frameThdEndOfDecoding:
		;

		//S3.4 end of pic: update errFlag/finish_flag in poc_list, signal to M6
		pthread_mutex_lock(selfInfo->outputMutex);
		if (selfInfo->errFlag)
		{
// printf("set error flag to pic %p\n", cm->frame_to_show);
			TComPic* cur_pic = p_slice->cur_pic;

			pthread_mutex_lock(&cur_pic->progressMutex);
			cur_pic->errFlag = selfInfo->errFlag;
			pthread_cond_broadcast(&cur_pic->progressCond);
			WIN32_BROADCAST_OTHERS(&cur_pic->progressCond, p_slice->nThdNum);
			pthread_mutex_unlock(&cur_pic->progressMutex);
		}
		else
		{
			//update finish_flag
			p_slice->cur_pic->bFinishDecode = 1;
// 			printf("finish POC %d\n", p_slice->cur_pic->m_iPOC);
		}
		pthread_mutex_unlock(selfInfo->outputMutex);


		//S3.5 switch to ready state, signal to M3
		pthread_mutex_lock(selfInfo->stateMutex);
		selfInfo->state = FRAME_STATE_READY;
		pthread_mutex_unlock(selfInfo->stateMutex);
		pthread_cond_signal(selfInfo->readyCond);

	}


	selfInfo->bRunning = 0;
	return exitCode;
}

VO_U32 InitH265Threads(H265_DEC *pDecGlobal)
{
	VO_S32 i;
	voH265ThreadParam param;
	h265FrameThdPool *pThdPool;
	assert(pDecGlobal->nThdNum>=1 && pDecGlobal->nThdNum<=MAX_THREAD_NUM2);

	pDecGlobal->pFrameThdPool = pThdPool = (h265FrameThdPool *)AlignMalloc(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, sizeof(h265FrameThdPool), CACHE_LINE);
	if (pThdPool == NULL)
	{
		return VO_ERR_FAILED;
	}
	SetMem(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, pThdPool, 0, sizeof(h265FrameThdPool));

	for (i = 0; i < pDecGlobal->nThdNum; i++)
	{
		if (pthread_mutex_init(pThdPool->stateMutex+i, NULL))
		{
			return VO_ERR_FAILED;
		}
		if (pthread_cond_init(pThdPool->setupCond+i, NULL))
		{
			return VO_ERR_FAILED;
		}
		if (pthread_cond_init(pThdPool->readyCond+i, NULL))
		{
			return VO_ERR_FAILED;
		}
	}

// 	if (pthread_mutex_init(&pThdPool->progressMutex, NULL))
// 	{
// 		return VO_ERR_FAILED;
// 	}
	if (pthread_mutex_init(&pThdPool->outputMutex, NULL))
	{
		return VO_ERR_FAILED;
	}
// 	if (pthread_cond_init(&pThdPool->progressCond, NULL))
// 	{
// 		return VO_ERR_FAILED;
// 	}
// 	if (pthread_cond_init(&pThdPool->outputCond, NULL))
// 	{
// 		return VO_ERR_FAILED;
// 	}

	//init slice loop
	pThdPool->pThdContext = AlignMalloc(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id,  pDecGlobal->nThdNum*sizeof(H265_DEC_SLICE), CACHE_LINE);
	if (pThdPool->pThdContext == NULL)
	{
		return VO_ERR_FAILED;
	}
	SetMem(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, pThdPool->pThdContext, 0, pDecGlobal->nThdNum*sizeof(H265_DEC_SLICE));

	//init frame data buffer for main thd
	pDecGlobal->slice.pFrameData = (VO_U8*)AlignMalloc(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, INPUT_SIZE_INC_STEP, CACHE_LINE);
	if (pDecGlobal->slice.pFrameData == NULL)
	{
		pDecGlobal->slice.frameDataSize = 0;
		return VO_ERR_FAILED;
	}
	pDecGlobal->slice.frameDataSize = 1;
	pDecGlobal->slice.nThdNum = pDecGlobal->nThdNum;

	param.pDecGlobal = pDecGlobal;
	for (i = 0; i < pDecGlobal->nThdNum; i++)
	{
		h265FrameThdInfo *pInfo = pThdPool->thdInfo+i;
		param.selfInfo = pInfo;
		pInfo->stateMutex = pThdPool->stateMutex+i;
// 		pInfo->progressMutex = &pThdPool->progressMutex;
		pInfo->outputMutex = &pThdPool->outputMutex;
		pInfo->setupCond = pThdPool->setupCond+i;
		pInfo->readyCond = pThdPool->readyCond+i;
// 		pInfo->progressCond = &pThdPool->progressCond;
// 		pInfo->outputCond = &pThdPool->outputCond;
		pInfo->pDecGlobal = pDecGlobal;
		pInfo->pSelfSlice = (H265_DEC_SLICE *)pThdPool->pThdContext + i;

		pInfo->pSelfSlice->p_user_op_all = &(pDecGlobal->user_op_all);
		//Harry: Initialize the state of pps&sps
		pInfo->pSelfSlice->slice_pic_parameter_set_id = (VO_U32)(-1); //None pps set
		pInfo->pSelfSlice->slice_seq_parameter_set_id = (VO_U32)(-1); //None sps set

		//init frame data buffer for sub thd
		pInfo->pSelfSlice->pFrameData = (VO_U8*)AlignMalloc(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, INPUT_SIZE_INC_STEP, CACHE_LINE);
		if (pInfo->pSelfSlice->pFrameData == NULL)
		{
			pInfo->pSelfSlice->frameDataSize = 0;
			return VO_ERR_FAILED;
		}
		pInfo->pSelfSlice->frameDataSize = 1;
		pInfo->pSelfSlice->nThdNum = pDecGlobal->nThdNum;

		//update slice
// 		*(pInfo->pSelfSlice) = pDecGlobal->slice;
// 		alloc_private_params(pInfo->pSelfGlobal);

		pInfo->state = FRAME_STATE_READY;
		pInfo->runCMD = 1;
		pInfo->bRunning = 0;
		if (pthread_create((pthread_t *)(&(pInfo->thread_id)), NULL, H265DecThreadFunc, &param))
		{
			return VO_ERR_FAILED;
		}
		while (!pInfo->bRunning)
		{
			;
		}
// 		printf("thread %d created\n", i);
	}

	return VO_ERR_NONE;
}

VO_U32 ReleaseH265Threads(H265_DEC *pDecGlobal)
{
	h265FrameThdPool *pThdPool = pDecGlobal->pFrameThdPool;
	h265FrameThdInfo *pInfo = pThdPool->thdInfo;
	H265_DEC_SLICE *pSlice = (H265_DEC_SLICE *)pThdPool->pThdContext;
	VO_U32 nSubThd = pDecGlobal->nThdNum;
	VO_U32 voRC = VO_ERR_NONE;
	VO_S32 i;

	assert(pDecGlobal->nThdNum >= 0);

	//broadcast all cond

	for (i = nSubThd; i != 0; --i,++pInfo)
	{
//printf("try ending sub-thd %d\n", nSubThd-i);
		THD_COND_WAIT(pInfo->readyCond, pInfo->stateMutex, pInfo->state != FRAME_STATE_READY);
		pInfo->runCMD = 0;

		//signal
		pthread_mutex_lock(pInfo->stateMutex);
		pInfo->state = FRAME_STATE_DECODING;
		pthread_mutex_unlock(pInfo->stateMutex);
		pthread_cond_signal(pInfo->setupCond);
		while(pInfo->bRunning)
		{
			thread_sleep(1);
		}
//printf("sub-thd %d is end\n", nSubThd-i);
		pthread_join(pInfo->thread_id, NULL);
		pInfo->thread_id = (pthread_t)NULL;
		if (pInfo->pSelfSlice->pFrameData && pInfo->pSelfSlice->frameDataSize > 0)
		{
			AlignFree(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, pInfo->pSelfSlice->pFrameData);
		}
	}

	if (pDecGlobal->p_avalable_slice->pFrameData && pDecGlobal->p_avalable_slice->frameDataSize > 0)
	{
		AlignFree(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, pDecGlobal->p_avalable_slice->pFrameData);
		pDecGlobal->p_avalable_slice->pFrameData = NULL;
		pDecGlobal->p_avalable_slice->frameDataSize = 0;
	}

	//unit all slice
	for (i = nSubThd; i != 0; --i,++pSlice)
	{
	    VO_S32 j;
// 		if (pSlice->init_done)
// 		{
			//---Free ASO //YU_TBD: remove it
			//freeSaoParam(pDecGlobal, pSlice->m_saoParam );
			for(j = 0;j < pSlice->PicHeightInCtbsY;j++)
            {
		        freeSaoParam(pDecGlobal, pSlice->m_saoParam[j]);
			}
            //if(pSlice->p_sps->scaling_list_enabled_flag){ //pSilce->p_sps may be NULL
            free_mMulLevelScale(pSlice);
            //}
			/* Free decoder internal (private) memory*/
			FreeSliceBuf( pSlice, &pDecGlobal->user_op_all );
            
// 		}
	}
	if (pThdPool->pThdContext)
	{
		AlignFree(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, pThdPool->pThdContext);
	}

// 	if (pthread_cond_destroy(&pThdPool->outputCond))
// 	{
// 		voRC |= VO_ERR_FAILED;
// 	}
// 	if (pthread_cond_destroy(&pThdPool->progressCond))
// 	{
// 		voRC |= VO_ERR_FAILED;
// 	}
	if (pthread_mutex_destroy(&pThdPool->outputMutex))
	{
		voRC |= VO_ERR_FAILED;
	}
// 	if (pthread_mutex_destroy(&pThdPool->progressMutex))
// 	{
// 		voRC |= VO_ERR_FAILED;
// 	}

	for (i = 0; i < pDecGlobal->nThdNum; i++)
	{
		if (pthread_mutex_destroy(pThdPool->stateMutex+i))
		{
			voRC |= VO_ERR_FAILED;
		}
		if (pthread_cond_destroy(pThdPool->setupCond+i))
		{
			voRC |= VO_ERR_FAILED;
		}
		if (pthread_cond_destroy(pThdPool->readyCond+i))
		{
			voRC |= VO_ERR_FAILED;
		}
	}

	if (pThdPool)
	{
		AlignFree(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, pThdPool);
		pThdPool = NULL;
	}

	return voRC;
}
#endif