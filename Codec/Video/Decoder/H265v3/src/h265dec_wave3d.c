#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "h265_decoder.h"
#include "h265dec_dequant.h"
#include "h265dec_slice.h"
#include "h265dec_list.h"

#if USE_3D_WAVE_THREAD

static VO_S32 InitTaskQ(TaskQ *pQ)
{
// 	if (pthread_mutex_init(&pQ->mutex, NULL))
// 	{
// 		return VO_ERR_FAILED;
// 	}
	pQ->size = 0;
	pQ->head = pQ->tail = 0;

	return VO_ERR_NONE;
}

VO_S32 AllocParserTaskQ(const H265_DEC* const p_dec, TaskQ *pQ, VO_U32 size)
{
	pQ->TaskPool = (void**)AlignMalloc(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, sizeof(Parser_T*)*size, CACHE_LINE);
	if (!pQ->TaskPool)
	{
		return VO_ERR_OUTOF_MEMORY;
	}
	pQ->szPool = size;
	SetMem(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, pQ->TaskPool, 0, sizeof(Parser_T*)*size);

	return InitTaskQ(pQ);
}

VO_S32 AllocReconTaskQ(const H265_DEC* const p_dec, TaskQ *pQ, VO_U32 size)
{
	pQ->TaskPool = (void**)AlignMalloc(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, sizeof(TReconTask*)*size, CACHE_LINE);
	if (!pQ->TaskPool)
	{
		return VO_ERR_OUTOF_MEMORY;
	}
	pQ->szPool = size;
	SetMem(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, pQ->TaskPool, 0, sizeof(TReconTask*)*size);

	return InitTaskQ(pQ);
}

void freeTaskQ(const H265_DEC* const p_dec, TaskQ *pQ)
{
// 	pthread_mutex_destroy(&pQ->mutex);

	AlignFree(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, pQ->TaskPool);
}

static VOINLINE VO_BOOL IsFullTaskQ(TaskQ *pQ)
{
	return pQ->size >= pQ->szPool;
}

VO_S32 PushTaskQ(TaskQ *pQ, void *task)
{
// 	pthread_mutex_lock(&pQ->mutex);
	if (!IsFullTaskQ(pQ))
	{
		pQ->TaskPool[pQ->tail++] = task;
		if (pQ->tail == pQ->szPool)
		{
			pQ->tail = 0;
		}
		++pQ->size;
	}
	else
	{
//printf("warning: task Q is full!\n");
		return 1;
	}
// 	pthread_mutex_unlock(&pQ->mutex);

	return VO_ERR_NONE;
}

void * PopTaskQ(TaskQ *pQ)
{
	void *t = NULL;
// 	pthread_mutex_lock(&pQ->mutex);
	if (pQ->size > 0)
	{
		t = pQ->TaskPool[pQ->head++];
		if (pQ->head == pQ->szPool)
		{
			pQ->head = 0;
		}
		--pQ->size;
	}
// 	pthread_mutex_unlock(&pQ->mutex);

	return t;
}

inline VO_U32 CheckPicFinished(h265_wave3DThdInfo * const selfInfo, const TComPic * const p)
{
	VO_U32 bFinishDecode;

	pthread_mutex_lock(selfInfo->outputMutex);
	bFinishDecode = p->bFinishDecode;
	pthread_mutex_unlock(selfInfo->outputMutex);

	return bFinishDecode;
}


TReconTask * GetRefLCU_rcTask(const H265_DEC_SLICE *const p_slice, /*const TComPic * const refPic,*/ VO_S32 max_y, VO_S32 max_x)
{
	const VO_U32 log2_luma_coding_block_size = p_slice->p_sps->log2_luma_coding_block_size;

	max_y += 10;  //due to SAO & deblock
	max_y >>= log2_luma_coding_block_size;
	max_y = Clip3(0, p_slice->PicHeightInCtbsY - 1, max_y);


	max_x >>= log2_luma_coding_block_size;
  max_x = Clip3(0, p_slice->PicWidthInCtbsY - 2, max_x);
	++max_x;  //due to SAO


	return p_slice->rc_task + (max_y*p_slice->PicWidthInCtbsY + max_x);
}

VO_BOOL CheckLCUFinished(TComPic * const p, TReconTask * const block, TReconTask * const reconTask)
{
	if (!block->bFinish)
	{
		pthread_mutex_lock(&p->unBlockMutex);
		if (!block->bFinish)
		{
			//add to the end of blocked list
			TReconTask *head = block->blocked;
			//if(reconTask->p_slice&&block->p_slice)
			//   printf("+++++++POC %d rs %d blocked by POC %d rs %d\n", reconTask->p_slice->m_iPOC, reconTask->ctb_addr_in_rs, block->p_slice->m_iPOC, block->ctb_addr_in_rs);
			//else
			//   printf("+++++++POC %d rs %d blocked by POC %d rs %d\n", 0, reconTask->ctb_addr_in_rs, 1, block->ctb_addr_in_rs);
			if (head == NULL)
			{
				block->blocked = reconTask;
			}
			else
			{
				while (head->next_blocked != NULL)
				{
					head = head->next_blocked;
				}
				head->next_blocked = reconTask;
			}
			pthread_mutex_unlock(&p->unBlockMutex);
			return VO_FALSE;
		}
		pthread_mutex_unlock(&p->unBlockMutex);
	}

	return VO_TRUE;
}

//main-thread
VO_S32 DecodeFrameMain_wave3D(H265_DEC *pDecGlobal, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutInfo, VO_S32 newInput)
{
	VO_S32 ret;
	h265_wave3DThdPool *pThdPool = pDecGlobal->pWave3DThdPool;
// 	Parser_T *parserTask;
	H265_DEC_SLICE *p_slice_cur;

	switch (newInput)
	{
	case 0:
		//no new input data, just check if output
		GetDisPic(pDecGlobal, pOutput, pOutInfo);
		return VO_ERR_NONE;
		break;
	case 1:
		//new input data
#if USE_CODEC_NEW_API
		p_slice_cur = (H265_DEC_SLICE *)pOutput;
#else
	case 3:
		//only decode frame, but don't output anything
		//used for combined frame before flush command
// 		UpdateContextFromSub(pDecGlobal, pSubContext);
#endif
		break;
	case 2:
		//flush command from API
		{
			//just wait for all thread done
			pthread_mutex_lock(&pThdPool->freeFrameMutex);
			while (pThdPool->freeFrameList.size < pThdPool->freeFrameList.szPool)
			{
// LOGI("free frame size %d, wait\n", pThdPool->freeFrameList.size);
				pthread_cond_wait(&pThdPool->freeFrameCond, &pThdPool->freeFrameMutex);
			}
LOGI("free frame size %lu, wait end\n", pThdPool->freeFrameList.size);
			pthread_mutex_unlock(&pThdPool->freeFrameMutex);

#if USE_CODEC_NEW_API
			xWriteOutput(pDecGlobal, &pDecGlobal->dpb_list, -1 );//flush dis pics
#else
			xWriteOutput(pDecGlobal, &pDecGlobal->dpb_list, 0 );//flush dis pics
#endif
		}
		return VO_ERR_NONE;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

#if !USE_CODEC_NEW_API

	//M1.wait for freedom frame room
	pthread_mutex_lock(&pThdPool->freeFrameMutex);
	while ((p_slice_cur = (Parser_T *)PopTaskQ(&pThdPool->freeFrameList)) == NULL)
	{
		pthread_cond_wait(&pThdPool->freeFrameCond, &pThdPool->freeFrameMutex);
	}
	pthread_mutex_unlock(&pThdPool->freeFrameMutex);
    //printf("get slice:%x\n",p_slice_cur);
	assert(p_slice_cur != NULL);
// 	p_slice_cur = parserTask->pSelfSlice;
#endif

	//M2.prepare new frame
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
LOGI("out of memory\n");
				return VO_ERR_OUTOF_MEMORY;
			}
			p_slice_cur->frameDataSize = size;
		}
	}
	CopyMem(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, p_slice_cur->pFrameData, pDecGlobal->in_data, pDecGlobal->data_sz);
	p_slice_cur->in_data = p_slice_cur->pFrameData;
	p_slice_cur->data_sz = pDecGlobal->data_sz;
	p_slice_cur->fast_mode = pDecGlobal->fast_mode;
	p_slice_cur->timestamp = pDecGlobal->timestamp;
// 	p_slice_cur->pThdInfo = pThdInfo;

// LOGI("start prepare\n");
	if ((ret = PrepareDecodeFrame(pDecGlobal, p_slice_cur, &p_slice_cur->in_data, &p_slice_cur->data_sz)) != VO_ERR_NONE)
	{
LOGI("error %lx, free slice:%p\n", (VO_U32)ret, p_slice_cur);
	    pthread_mutex_lock(&pThdPool->freeFrameMutex);
		PushTaskQ(&pThdPool->freeFrameList, p_slice_cur);
		pthread_mutex_unlock(&pThdPool->freeFrameMutex);
		return ret;
	}
// LOGI("finish prepare\n");
	//M3. Add to parser task queue, signal
	pthread_mutex_lock(&pThdPool->quaryTaskMutex);
	PushTaskQ(&pThdPool->parserTaskQ, p_slice_cur);
	pthread_mutex_unlock(&pThdPool->quaryTaskMutex);
	pthread_cond_signal(&pThdPool->quaryTaskCond);

#if !USE_CODEC_NEW_API
	if (newInput == 1)
	{
		GetDisPic(pDecGlobal, pOutput, pOutInfo);
	}
#endif

	return VO_ERR_NONE;
}

//sub-thread
static THREAD_FUNCTION H265DecThreadFunc_wave3D(void *pArg)
{
	THREAD_FUNCTION_RETURN exitCode = 0;

	voH265Wave3DThreadParam *param = (voH265Wave3DThreadParam *)pArg;
	const H265_DEC * const pDecGlobal = param->pDecGlobal;
	h265_wave3DThdInfo *selfInfo = param->selfInfo;
	VO_U32 nThdIdx = selfInfo - pDecGlobal->pWave3DThdPool->thdInfo;
#if USE_CODEC_NEW_API
	pthread_cond_t * const disListCond = &param->pDecGlobal->disListCond;
	pthread_mutex_t * const disListMutex = &param->pDecGlobal->disListMutex;
#endif
	H265_DEC_SLICE * p_slice;

	selfInfo->bRunning = 1;

	while (selfInfo->runCMD)
	{
		//S1.wait for task
// 		Parser_T *parserTask;
		TReconTask *reconTask = NULL;
		pthread_mutex_lock(selfInfo->quaryTaskMutex);
		while ((p_slice = (Parser_T *)PopTaskQ(selfInfo->parserTaskQ)) == NULL
			&& (reconTask = (TReconTask *)PopTaskQ(selfInfo->unblockTaskQ)) == NULL
			&& (reconTask = (TReconTask *)PopTaskQ(selfInfo->reconTaskQ)) == NULL)	//priority control: parser>unblock>recon
		{
			pthread_cond_wait(selfInfo->quaryTaskCond, selfInfo->quaryTaskMutex);
		}
		pthread_mutex_unlock(selfInfo->quaryTaskMutex);

		if (!selfInfo->runCMD)
		{
			//put back task
			pthread_mutex_lock(selfInfo->quaryTaskMutex);
			if (p_slice != NULL)
			{
				PushTaskQ(selfInfo->parserTaskQ, p_slice);
			}
			else
			{
				PushTaskQ(selfInfo->reconTaskQ, reconTask);
			}
			pthread_mutex_unlock(selfInfo->quaryTaskMutex);
			pthread_cond_signal(selfInfo->quaryTaskCond);
			break;
		}

		if (p_slice != NULL)
		{
			p_slice->pThdInfo = selfInfo;
			//parse one frame
LOGI("POC %ld start parser slice:%p\n", p_slice->cur_pic->m_iPOC,p_slice);
			if ((selfInfo->errFlag = DoDecodeFrame(pDecGlobal, p_slice, p_slice->in_data, p_slice->data_sz)) != VO_ERR_NONE)
			{
			    TReconTask *reconT = NULL;
				VO_U32 i;
LOGI("error in parser poc %ld, pic: %p,free slice:%p ERR_ID: %lx\n",p_slice->cur_pic->m_iPOC,p_slice->cur_pic,p_slice, (VO_U32)selfInfo->errFlag);
                p_slice->cur_pic->errFlag = selfInfo->errFlag;
				pthread_mutex_lock(selfInfo->quaryTaskMutex);
                for(i = 0;i < p_slice->PicSizeInCtbsY;i++)
                {
                  reconT = &p_slice->rc_task[i];
				  if (reconT->blocked)
				  {
					//cur task blocked some other task, need to unblock them
					TReconTask *blocked = reconT->blocked;
					reconT->blocked = NULL;					
					do 
					{
						TReconTask *t;
						PushTaskQ(selfInfo->unblockTaskQ, blocked);
						t = blocked;
						blocked = t->next_blocked;
						t->next_blocked = NULL;
					} while (blocked != NULL);										
				  }
                }
				SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, p_slice->rc_task, 0, p_slice->PicSizeInCtbsY*sizeof(TReconTask));
				pthread_mutex_unlock(selfInfo->quaryTaskMutex);
// 				pthread_cond_signal(selfInfo->quaryTaskCond);
				pthread_cond_broadcast(selfInfo->quaryTaskCond);
				WIN32_BROADCAST_OTHERS(selfInfo->quaryTaskCond, p_slice->nThdNum);

				pthread_mutex_lock(selfInfo->freeFrameMutex);
				PushTaskQ(selfInfo->freeFrameList, p_slice);
				pthread_mutex_unlock(selfInfo->freeFrameMutex);
				pthread_cond_signal(selfInfo->freeFrameCond);

#if USE_CODEC_NEW_API
				pthread_mutex_lock(disListMutex);
				pthread_cond_signal(disListCond);
				pthread_mutex_unlock(disListMutex);
				pthread_cond_signal(pDecGlobal->getPicCond);
#endif
				goto wave3DThdEndOfDecoding;
			}
            
            //set recon task
 			{
				const VO_U32 PicWidthInCtbsY = p_slice->PicWidthInCtbsY;
				TReconTask* rctask = p_slice->rc_task;
				TReconTask* block = rctask - PicWidthInCtbsY + 1;

				VO_U32 i;
				for (i = p_slice->PicHeightInCtbsY-1; i > 0; --i)
				{
					rctask += PicWidthInCtbsY;
					block += PicWidthInCtbsY;
					CheckLCUFinished(p_slice->cur_pic, block, rctask);
				}
				pthread_mutex_lock(selfInfo->quaryTaskMutex);
				PushTaskQ(selfInfo->reconTaskQ, p_slice->rc_task);
				pthread_mutex_unlock(selfInfo->quaryTaskMutex);
				pthread_cond_signal(selfInfo->quaryTaskCond);

			}
//LOGI("POC %d end set recon task slice:%x\n", p_slice->cur_pic->m_iPOC,p_slice);
// 			pthread_mutex_lock(selfInfo->quaryTaskMutex);
// 			for(nRow = 0;nRow < p_slice->PicHeightInCtbsY;nRow++) 
// 			{
// 				TReconTask* rctask = p_slice->rc_task + nRow*p_slice->PicWidthInCtbsY;
// 				PushTaskQ(selfInfo->reconTaskQ, rctask);
// 			}
// 			pthread_mutex_unlock(selfInfo->quaryTaskMutex);
// 			pthread_cond_signal(selfInfo->quaryTaskCond);

// 			p_slice->cur_pic->nProgress_mv = VO_MAX_INT;

			///////////only for test///////////////////////////////////

// 			//frame finished
// 			pthread_mutex_lock(selfInfo->outputMutex);
// 			p_slice->cur_pic->bFinishDecode = 1;
// 			pthread_mutex_unlock(selfInfo->outputMutex);
// 
// 			pthread_mutex_lock(selfInfo->freeFrameMutex);
// 			PushTaskQ(selfInfo->freeFrameList, p_slice);
// 			pthread_mutex_unlock(selfInfo->freeFrameMutex);
// 			pthread_cond_signal(selfInfo->freeFrameCond);
			////////////////////////////////////////////////////////////
		}
		else
		{
			VO_U32 ctb_addr_in_rs;
			VO_U32 nRow, nColumn;
			//VO_BOOL sao_flag = VO_FALSE;

			//recon one LCU row
			assert(reconTask!=NULL);
			p_slice = reconTask->p_slice;
			ctb_addr_in_rs = reconTask->ctb_addr_in_rs;
			nRow = reconTask->nRow;
			nColumn = reconTask->nColumn;
//printf("--------------start recon task POC %d row %d rs %d slice:%x\n", p_slice->m_iPOC, nRow, ctb_addr_in_rs,p_slice);

// 			p_slice->pThdInfo = selfInfo;

			//if (p_slice->p_pps->transquant_bypass_enabled_flag != 1 && p_slice->p_sps->sample_adaptive_offset_enabled_flag && (p_slice->fast_mode & VO_FM_SAO)==0){
				//if(p_slice->slice_sao_luma_flag||p_slice->slice_sao_chroma_flag){			
					//sao_flag = VO_TRUE;
				//}
			//}

			do 
			{
				//reconTask->ctb_addr_in_rs = ctb_addr_in_rs;
				if(!reconTask->p_slice&&(reconTask->blocked))
				{	
					TReconTask *blocked = reconTask->blocked;
					H265_DEC_SLICE * p_slice_in = reconTask->blocked->p_slice;
					if(p_slice_in)
					{
					pthread_mutex_lock(&p_slice_in->cur_pic->unBlockMutex);
				    reconTask->bFinish = 1;
					reconTask->blocked = NULL;
					pthread_mutex_lock(selfInfo->quaryTaskMutex);
					do 
					{
						TReconTask *t;
 //printf("=======POC %d rs %d unblocked\n", blocked->p_slice->m_iPOC, blocked->ctb_addr_in_rs);
						PushTaskQ(selfInfo->unblockTaskQ, blocked);

						t = blocked;
						blocked = t->next_blocked;
						t->next_blocked = NULL;
					} while (blocked != NULL);
					pthread_mutex_unlock(selfInfo->quaryTaskMutex);
// 					pthread_cond_signal(selfInfo->quaryTaskCond);
					pthread_cond_broadcast(selfInfo->quaryTaskCond);
					WIN32_BROADCAST_OTHERS(selfInfo->quaryTaskCond, p_slice->nThdNum);
					pthread_mutex_unlock(&p_slice_in->cur_pic->unBlockMutex);
					}
				}
				if(reconTask->p_slice)
				{
				reconTask->pThdInfo = selfInfo;
				
				//check the in-frame ref
				if (nRow > 0)
				{
					TReconTask *block;
					if (nColumn+1 < p_slice->PicHeightInCtbsY)	//cur LCU is not the last one of LCU row
					{
						block = reconTask - p_slice->PicHeightInCtbsY + 1;
						if (!CheckLCUFinished(p_slice->cur_pic, block, reconTask))
						{
							break;
						}
					}
					else if (p_slice->PicHeightInCtbsY == 1) //only for one col LCU clip
					{
						block = reconTask - 1;
						if (!CheckLCUFinished(p_slice->cur_pic, block, reconTask))
						{
							break;
						}
					}
				}


				if (ReconTask(reconTask, nRow, nColumn))
				{
					//not finished

					break;
				}

				//all finished, unblock which blocked
// LOGI("POC %d rs %d finished\n", p_slice->m_iPOC, ctb_addr_in_rs);
				pthread_mutex_lock(&p_slice->cur_pic->unBlockMutex);
				reconTask->bFinish = 1;
				if (reconTask->blocked)
				{
					//cur task blocked some other task, need to unblock them
					TReconTask *blocked = reconTask->blocked;
					reconTask->blocked = NULL;
					pthread_mutex_lock(selfInfo->quaryTaskMutex);
					do 
					{
						TReconTask *t;
// LOGI("=======POC %d rs %d unblocked\n", blocked->p_slice->m_iPOC, blocked->ctb_addr_in_rs);
						PushTaskQ(selfInfo->unblockTaskQ, blocked);

						t = blocked;
						blocked = t->next_blocked;
						t->next_blocked = NULL;
					} while (blocked != NULL);
					pthread_mutex_unlock(selfInfo->quaryTaskMutex);
// 					pthread_cond_signal(selfInfo->quaryTaskCond);
					pthread_cond_broadcast(selfInfo->quaryTaskCond);
					WIN32_BROADCAST_OTHERS(selfInfo->quaryTaskCond, p_slice->nThdNum);
				}
				pthread_mutex_unlock(&p_slice->cur_pic->unBlockMutex);
				}
				++reconTask;
				++ctb_addr_in_rs;
			} while (++nColumn!=p_slice->PicWidthInCtbsY);
			//printf("thread: %d poc:%d ctb_addr_in_rs: %d\n",selfInfo->thread_id,p_slice->m_iPOC,ctb_addr_in_rs);
			if (ctb_addr_in_rs == p_slice->PicSizeInCtbsY)
			{

				//frame finished
				pthread_mutex_lock(selfInfo->outputMutex);
				if (!p_slice->errFlag)
				{
					p_slice->cur_pic->bFinishDecode = 1;
				}
				else
				{
LOGI("error in recon poc %ld, pic: %p,free slice:%p ERR_ID: %lx\n",p_slice->cur_pic->m_iPOC,p_slice->cur_pic,p_slice, (VO_U32)p_slice->errFlag);
					p_slice->cur_pic->errFlag = p_slice->errFlag;
				}
LOGI("finish POC %ld free slice:%p\n", p_slice->cur_pic->m_iPOC,p_slice);
				pthread_mutex_unlock(selfInfo->outputMutex);
#if USE_CODEC_NEW_API
				pthread_mutex_lock(disListMutex);
				pthread_cond_signal(disListCond);
				pthread_mutex_unlock(disListMutex);
				pthread_cond_signal(pDecGlobal->getPicCond);
#endif

				pthread_mutex_lock(selfInfo->freeFrameMutex);
				PushTaskQ(selfInfo->freeFrameList, p_slice);
				pthread_mutex_unlock(selfInfo->freeFrameMutex);
				pthread_cond_signal(selfInfo->freeFrameCond);
			}
		}
wave3DThdEndOfDecoding:
		//reset the errFlag
		selfInfo->errFlag = 0;
		;
	}

	selfInfo->bRunning = 0;
	return exitCode;
}


VO_U32 InitH265Threads_wave3D(H265_DEC *pDecGlobal)
{
	VO_S32 i;
	voH265Wave3DThreadParam param;
	h265_wave3DThdPool *pThdPool;
	VO_U8 *p_priv_mem;

	assert(pDecGlobal->nThdNum>=1 && pDecGlobal->nThdNum<=MAX_THREAD_NUM2);

	pDecGlobal->pWave3DThdPool = pThdPool = (h265_wave3DThdPool *)AlignMalloc(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, sizeof(h265_wave3DThdPool), CACHE_LINE);
	if (pThdPool == NULL)
	{
		return VO_ERR_OUTOF_MEMORY;
	}
	SetMem(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, pThdPool, 0, sizeof(h265_wave3DThdPool));

	if (pthread_mutex_init(&pThdPool->freeFrameMutex, NULL))
	{
		return VO_ERR_FAILED;
	}

	if (pthread_mutex_init(&pThdPool->quaryTaskMutex, NULL))
	{
		return VO_ERR_FAILED;
	}

	if (pthread_mutex_init(&pThdPool->outputMutex, NULL))
	{
		return VO_ERR_FAILED;
	}

	if (pthread_cond_init(&pThdPool->freeFrameCond, NULL))
	{
		return VO_ERR_FAILED;
	}

	if (pthread_cond_init(&pThdPool->quaryTaskCond, NULL))
	{
		return VO_ERR_FAILED;
	}

	//init slice memory
	{
		VO_S32 private_mem_size = 0;

		//---------- temp edge 
		private_mem_size += ( sizeof( VO_U8 )*( MAX_CU_SIZE + 7 ) * ( MAX_CU_SIZE + 7 ) + CACHE_LINE );//edge_emu_buff0
		private_mem_size += ( sizeof( VO_U8 )*( MAX_CU_SIZE + 7 ) * ( MAX_CU_SIZE + 7 ) + CACHE_LINE );//edge_emu_buff1
		//----------MC Buffer
		private_mem_size +=  ( sizeof( VO_S16 )*( MAX_CU_SIZE + 7 ) * ( MAX_CU_SIZE + 7 ) + CACHE_LINE ); // p_slice->asm_mc0_buff
		private_mem_size +=  ( sizeof( VO_S16 )*( MAX_CU_SIZE + 7 ) * ( MAX_CU_SIZE + 7 ) + CACHE_LINE ); // p_slice->asm_mc1_buff

		pThdPool->pThdContext = AlignMalloc(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id,  pDecGlobal->nThdNum*private_mem_size, CACHE_LINE);
		if (pThdPool->pThdContext == NULL)
		{
			return VO_ERR_OUTOF_MEMORY;
		}
// 	 	SetMem(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, pThdPool->pThdContext, 0, pDecGlobal->nThdNum*sizeof(H265_DEC_SLICE));
		p_priv_mem = (VO_U8 *)pThdPool->pThdContext;
	}

	//init free frame list, parserTaskQ and reconTaskQ
	pDecGlobal->nDecodingFrame = pDecGlobal->nThdNum; // + 1;

	{
		VO_S32 ret;
		ret = AllocParserTaskQ(pDecGlobal, &pThdPool->freeFrameList, pDecGlobal->nDecodingFrame+1);
		if (ret)
		{
			return ret;
		}
		ret = AllocParserTaskQ(pDecGlobal, &pThdPool->parserTaskQ, pDecGlobal->nDecodingFrame+1);
		if (ret)
		{
			return ret;
		}
		ret = AllocReconTaskQ(pDecGlobal, &pThdPool->reconTaskQ, (pDecGlobal->nDecodingFrame+1)*160); //160 LCU rows for height 2560 in 16x16 LCU size
		if (ret)
		{
			return ret;
		}
		ret = AllocReconTaskQ(pDecGlobal, &pThdPool->unblockTaskQ, (pDecGlobal->nDecodingFrame+1)*160);
		if (ret)
		{
			return ret;
		}
	}

	//init frame data buffer for main thd
	pDecGlobal->slice.pFrameData = (VO_U8*)AlignMalloc(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, INPUT_SIZE_INC_STEP, CACHE_LINE);
	if (pDecGlobal->slice.pFrameData == NULL)
	{
		pDecGlobal->slice.frameDataSize = 0;
		return VO_ERR_OUTOF_MEMORY;
	}
	pDecGlobal->slice.frameDataSize = 1;
	pDecGlobal->slice.nThdNum = pDecGlobal->nThdNum;

	//init all slice
	pDecGlobal->slicePool = (H265_DEC_SLICE *)AlignMalloc(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, sizeof(H265_DEC_SLICE)*pDecGlobal->nDecodingFrame, CACHE_LINE);
	if (pDecGlobal->slicePool == NULL)
	{
		return VO_ERR_OUTOF_MEMORY;
	}
	SetMem(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, pDecGlobal->slicePool, 0, sizeof(H265_DEC_SLICE)*pDecGlobal->nDecodingFrame);
	PushTaskQ(&pThdPool->freeFrameList, &pDecGlobal->slice);
	for (i = 0; i < pDecGlobal->nDecodingFrame; ++i)
	{
		H265_DEC_SLICE *pSlice = pDecGlobal->slicePool+i;
		pSlice->p_user_op_all = &(pDecGlobal->user_op_all);
		//Harry: Initialize the state of pps&sps
		pSlice->slice_pic_parameter_set_id = (VO_U32)(-1); //None pps set
		pSlice->slice_seq_parameter_set_id = (VO_U32)(-1); //None sps set

		//init frame data buffer for sub thd
		pSlice->pFrameData = (VO_U8*)AlignMalloc(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, INPUT_SIZE_INC_STEP, CACHE_LINE);
		if (pSlice->pFrameData == NULL)
		{
			pSlice->frameDataSize = 0;
			return VO_ERR_OUTOF_MEMORY;
		}
		pSlice->frameDataSize = 1;
		pSlice->nThdNum = pDecGlobal->nThdNum;
        pSlice->voCabacInitTab = pDecGlobal->voCabacInitTab;
		PushTaskQ(&pThdPool->freeFrameList, pSlice);
	}

	param.pDecGlobal = pDecGlobal;
	for (i = 0; i < pDecGlobal->nThdNum; i++)
	{
		h265_wave3DThdInfo *pInfo = pThdPool->thdInfo+i;
		param.selfInfo = pInfo;
		pInfo->freeFrameMutex = &pThdPool->freeFrameMutex;
		pInfo->quaryTaskMutex = &pThdPool->quaryTaskMutex;
		pInfo->outputMutex = &pThdPool->outputMutex;
		pInfo->freeFrameCond = &pThdPool->freeFrameCond;
		pInfo->quaryTaskCond = &pThdPool->quaryTaskCond;
		pInfo->freeFrameList = &pThdPool->freeFrameList;
		pInfo->parserTaskQ = &pThdPool->parserTaskQ;
		pInfo->reconTaskQ = &pThdPool->reconTaskQ;
		pInfo->unblockTaskQ = &pThdPool->unblockTaskQ;
		pInfo->pDecGlobal = pDecGlobal;

		ALIGN_MEM(p_priv_mem);
		pInfo->edge_emu_buff0 = ( VO_U8* )p_priv_mem;
		p_priv_mem += ( sizeof( VO_U8 )*( MAX_CU_SIZE + 7 ) * ( MAX_CU_SIZE + 7 ) );

		ALIGN_MEM(p_priv_mem);
		pInfo->edge_emu_buff1 = ( VO_U8* )p_priv_mem;
		p_priv_mem += ( sizeof( VO_U8 )*( MAX_CU_SIZE + 7 ) * ( MAX_CU_SIZE + 7 ) );

		ALIGN_MEM(p_priv_mem);
		pInfo->asm_mc0_buff = ( VO_S16* )p_priv_mem;
		p_priv_mem += ( sizeof( VO_S16 )*( MAX_CU_SIZE + 7 ) * ( MAX_CU_SIZE + 7 ) );

		ALIGN_MEM(p_priv_mem);
		pInfo->asm_mc1_buff = ( VO_S16* )p_priv_mem;
		p_priv_mem += ( sizeof( VO_S16 )*( MAX_CU_SIZE + 7 ) * ( MAX_CU_SIZE + 7 ) );
#if ENABLE_THD_INFO
        pInfo->mc_sync = 0;
        pInfo->block_count = 0;

#endif
// 		pInfo->state = FRAME_STATE_READY;
		pInfo->runCMD = 1;
		pInfo->bRunning = 0;
		if (pthread_create((pthread_t *)(&(pInfo->thread_id)), NULL, H265DecThreadFunc_wave3D, &param))
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

VO_U32 ReleaseH265Threads_wave3D(H265_DEC *pDecGlobal)
{
	h265_wave3DThdPool *pThdPool = pDecGlobal->pWave3DThdPool;
	h265_wave3DThdInfo *pInfo = pThdPool->thdInfo;
// 	H265_DEC_SLICE *pSlice = (H265_DEC_SLICE *)pThdPool->pThdContext;
	VO_U32 nSubThd = pDecGlobal->nThdNum;
	VO_U32 voRC = VO_ERR_NONE;
	VO_S32 i;
	TReconTask reconTask;  //only to wake up blocked thread
#if ENABLE_THD_INFO
	VO_U32 mc_sync = 0;
    VO_U32 block_count = 0;

#endif

	assert(pDecGlobal->nThdNum >= 0);

	for (i = nSubThd; i != 0; --i,++pInfo)
	{
		pInfo->runCMD = 0;
#if ENABLE_THD_INFO
		mc_sync += pInfo->mc_sync;
        block_count += pInfo->block_count;

#endif

	}
#if ENABLE_THD_INFO
		printf("MC SYNC count:%d\n",mc_sync);
        printf("MC block_count:%d\n",block_count);

#endif

	//signal
	pthread_mutex_lock(&pThdPool->quaryTaskMutex);
	PushTaskQ(&pThdPool->reconTaskQ, &reconTask);
	pthread_mutex_unlock(&pThdPool->quaryTaskMutex);
	pthread_cond_signal(&pThdPool->quaryTaskCond);

	for (i = nSubThd, pInfo = pThdPool->thdInfo; i != 0; --i,++pInfo)
	{
LOGI("try ending sub-thd %lu\n", nSubThd-i);
		while(pInfo->bRunning)
		{
			thread_sleep(1);
		}
LOGI("sub-thd %lu is end\n", nSubThd-i);
		pthread_join(pInfo->thread_id, NULL);
		pInfo->thread_id = (pthread_t)NULL;
	}

	for (i = 0; i < pDecGlobal->nDecodingFrame; ++i)
	{
		H265_DEC_SLICE *pSlice = pDecGlobal->slicePool+i;
		VO_U32 j;
		if (pSlice->pFrameData && pSlice->frameDataSize > 0)
		{
			AlignFree(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, pSlice->pFrameData);
		}

// 		if(pSlice->p_sps->scaling_list_enabled_flag){ //i==0, because only one instance in slice
			free_mMulLevelScale(pSlice);
// 		}
	
		/* Free decoder internal (private) memory*/
		FreeSliceBuf( pSlice, &pDecGlobal->user_op_all );
	}
	if (pDecGlobal->slice.pFrameData && pDecGlobal->slice.frameDataSize > 0)
	{
		AlignFree(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, pDecGlobal->slice.pFrameData);
		pDecGlobal->slice.pFrameData = NULL;
		pDecGlobal->slice.frameDataSize = 0;
	}

	freeTaskQ(pDecGlobal, &pThdPool->freeFrameList);
	freeTaskQ(pDecGlobal, &pThdPool->parserTaskQ);
	freeTaskQ(pDecGlobal, &pThdPool->reconTaskQ);
	freeTaskQ(pDecGlobal, &pThdPool->unblockTaskQ);

	if (pDecGlobal->slicePool)
	{
		AlignFree(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, pDecGlobal->slicePool);
	}

	if (pThdPool->pThdContext)
	{
		AlignFree(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, pThdPool->pThdContext);
	}

	if (pthread_cond_destroy(&pThdPool->quaryTaskCond))
	{
		voRC |= VO_ERR_FAILED;
	}
	if (pthread_cond_destroy(&pThdPool->freeFrameCond))
	{
		voRC |= VO_ERR_FAILED;
	}
	if (pthread_mutex_destroy(&pThdPool->outputMutex))
	{
		voRC |= VO_ERR_FAILED;
	}
	if (pthread_mutex_destroy(&pThdPool->quaryTaskMutex))
	{
		voRC |= VO_ERR_FAILED;
	}
	if (pthread_mutex_destroy(&pThdPool->freeFrameMutex))
	{
		voRC |= VO_ERR_FAILED;
	}

	if (pThdPool)
	{
		AlignFree(pDecGlobal->user_op_all.p_user_op, pDecGlobal->user_op_all.codec_id, pThdPool);
		pThdPool = NULL;
	}

	return voRC;

}

#endif
