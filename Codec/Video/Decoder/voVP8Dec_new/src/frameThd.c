#include <assert.h>
#include "vpx_config.h"
#include "onyxc_int.h"
#include "onyxd_int.h"
#include "../vpx_mem/vpx_mem.h"
#include "alloccommon.h"
#include "decoderthreading.h"

#if USE_FRAME_THREAD

extern int get_free_fb (VP8D_COMP *pbi, VP8_COMMON *cm);
extern void init_frame(VP8D_COMP *pbi);
extern void setup_token_decoder(VP8D_COMP *pbi,
						 const unsigned char* token_part_sizes);
extern int get_delta_q(vp8_reader *bc, int prev, int *q_update);
extern void vp8cx_init_de_quantizer(VP8D_COMP *pbi);
extern void vp8_mb_init_dequantizer(VP8D_COMP *pbi, MACROBLOCKD *xd);
extern int swap_frame_buffers (VP8_COMMON *cm, int mul_thd);
extern void vp8_de_alloc_priv_buffer(VP8_COMMON *oci);


static VO_S32 GetOutputPic(VP8D_COMP *pDecGlobal, vpx_image_t * pOutput)
{
	YV12_BUFFER_CONFIG *pPic;
	VO_U32 i;
	VO_U32 bFinish;
	VP8_COMMON *cm = &pDecGlobal->common;
	if(cm->output_size > 0)
	{
		pPic = cm->p_output[0];

			pthread_mutex_lock(&pDecGlobal->pFrameThdPool->outputMutex);
			//check errFlag
			while (pPic->errFlag)
			{
printf("clean error pic %p, %x\n", pPic, pPic->errFlag);
// 				pPic->bFinishOutput = 1;
				for(i = 0;i<cm->output_size-1;i++)
					cm->p_output[i]=cm->p_output[i+1];
				cm->output_size--;
				if (cm->output_size > 0)
				{
					pPic = cm->p_output[0];
				}
				else
				{
					pthread_mutex_unlock(&pDecGlobal->pFrameThdPool->outputMutex);
					goto noOutput;
				}
			}
			bFinish = pPic->removable;
			pthread_mutex_unlock(&pDecGlobal->pFrameThdPool->outputMutex);

		if (bFinish)
		{
			// printf("output %d \n", pPic->cur_buf_index);
// 			pOutput->fmt=pPic->clrtype == REG_YUV ?
// 				VPX_IMG_FMT_I420 : VPX_IMG_FMT_VPXI420;
			pOutput->planes[VPX_PLANE_Y]=pPic->y_buffer;
			pOutput->planes[VPX_PLANE_U]=pPic->u_buffer;
			pOutput->planes[VPX_PLANE_V]=pPic->v_buffer;
			pOutput->stride[VPX_PLANE_Y]=pPic->y_stride;
			pOutput->stride[VPX_PLANE_U]=pPic->uv_stride;
			pOutput->stride[VPX_PLANE_V]=pPic->uv_stride;
// 			pOutput->d_w  =  pPic->y_width;
// 			pOutput->d_h =  pPic->y_height;
			pOutput->d_w  =  cm->Width;
			pOutput->d_h =  cm->Height;
// 			pOutput->w = pPic->y_stride;
// 			pOutput->h = (pPic->y_height + 2 * VP8BORDERINPIXELS + 15) & ~15;
			pOutput->type = pPic->type;
			pOutput->Time		= pPic->Time;
			//printf("checkVideo: pPic->pY:%x,width:%d,height:%d\r\n",pPic->pY,pPic->iWidth,pPic->iHeight);

// 			if(!pPic->used_for_reference)
// 				pDecGlobal->licenseCheck->checkVideo(pDecGlobal->licenseCheck->hCheck,pOutput,&pOutInfo->Format);
			//printf("finish checkVideo\r\n");
			//pDecGlobal->licenseCheck->lastVideo=(VO_U8 *)pOutput;
			pPic->bFinishOutput = 1;
			for(i = 0;i<cm->output_size-1;i++)
				cm->p_output[i]=cm->p_output[i+1];
			cm->output_size--;

			if(cm->output_size > 0)
			{
				pOutput->Flag = (VO_U32)cm->output_size;
			}
			else
			{
				pOutput->Flag = 0;
			}
			return 0;
		}
	}
noOutput:
	pOutput->planes[VPX_PLANE_Y]=
		pOutput->planes[VPX_PLANE_U]=
		pOutput->planes[VPX_PLANE_V]=0;

	pOutput->type =VO_VIDEO_FRAME_NULL;
	pOutput->Flag = (VO_U32)cm->output_size;

	return 0;
}

static void UpdateContextFromSub(VP8D_COMP *pDecGlobal, const VP8D_COMP *subContext)
{
	VP8_COMMON *cm_g = &pDecGlobal->common;
	const VP8_COMMON *cm_s = &subContext->common;
	//to be fix
	cm_g->Width = cm_s->Width;
	cm_g->Height = cm_s->Height;
	cm_g->global_init_done = cm_s->global_init_done;
	cm_g->clr_type = cm_s->clr_type;
	cm_g->frame_to_show = cm_s->frame_to_show;
	cm_g->yv12_fb = cm_s->yv12_fb;
	cm_g->pSegID_s = cm_s->pSegID_s;
	cm_g->fb_idx_ref_cnt = cm_s->fb_idx_ref_cnt;
	cm_g->lst_fb_idx = cm_s->lst_fb_idx;
	cm_g->gld_fb_idx = cm_s->gld_fb_idx;
	cm_g->alt_fb_idx = cm_s->alt_fb_idx;
	cm_g->imageBufSize = cm_s->imageBufSize;
	cm_g->vFrameIndexFIFO = cm_s->vFrameIndexFIFO;
	cm_g->show_frame = cm_s->show_frame;
	cm_g->mb_rows = cm_s->mb_rows;
	cm_g->mb_cols = cm_s->mb_cols;
	cm_g->mode_info_stride = cm_s->mode_info_stride;
	cm_g->use_bilinear_mc_filter = cm_s->use_bilinear_mc_filter;
	cm_g->full_pixel = cm_s->full_pixel;
	cm_g->nfc.mvc[0] = cm_s->nfc.mvc[0];
	cm_g->nfc.mvc[1] = cm_s->nfc.mvc[1];
	memcpy(cm_g->nfc.ymode_prob, cm_s->nfc.ymode_prob, sizeof(cm_g->nfc.ymode_prob));
	memcpy(cm_g->nfc.uv_mode_prob, cm_s->nfc.uv_mode_prob, sizeof(cm_g->nfc.uv_mode_prob));
	memcpy(cm_g->nfc.coef_probs, cm_s->nfc.coef_probs, sizeof(cm_g->nfc.coef_probs));


	cm_g->current_video_frame = cm_s->current_video_frame;
	cm_g->p_output = cm_s->p_output;
	cm_g->output_size = cm_s->output_size;
	cm_g->p_remove = cm_s->p_remove;
	cm_g->remove_size = cm_s->remove_size;

	pDecGlobal->mb.mb_segement_abs_delta = subContext->mb.mb_segement_abs_delta;
	memcpy(pDecGlobal->mb.mb_segment_tree_probs, subContext->mb.mb_segment_tree_probs, sizeof(pDecGlobal->mb.mb_segment_tree_probs));
	memcpy(pDecGlobal->mb.segment_feature_data, subContext->mb.segment_feature_data, sizeof(pDecGlobal->mb.segment_feature_data));
	memcpy(pDecGlobal->mb.ref_lf_deltas, subContext->mb.ref_lf_deltas, sizeof(pDecGlobal->mb.ref_lf_deltas));
	memcpy(pDecGlobal->mb.mode_lf_deltas, subContext->mb.mode_lf_deltas, sizeof(pDecGlobal->mb.mode_lf_deltas));
	pDecGlobal->ready_for_new_data = subContext->ready_for_new_data;
}

static void CopyContextToSub(const VP8D_COMP *pDecGlobal, VP8D_COMP *pSubContext)
{
	const VP8_COMMON *cm_g = &pDecGlobal->common;
	VP8_COMMON *cm_s = &pSubContext->common;
	//to be fix
	cm_s->new_fb_idx = cm_g->new_fb_idx;
	cm_s->no_lpf = cm_g->no_lpf;
	cm_s->processor_core_count = cm_g->processor_core_count;
	cm_s->cpu_caps = cm_g->cpu_caps;

	pSubContext->oxcf = pDecGlobal->oxcf;
	pSubContext->ec_enabled = pDecGlobal->ec_enabled;
	pSubContext->input_fragments = pDecGlobal->input_fragments;
	pSubContext->nThdNum = pDecGlobal->nThdNum;

	UpdateContextFromSub(pSubContext, pDecGlobal);
}

inline VO_S32 WaitForProgress(vp8FrameThdInfo * const pThdInfo, const YV12_BUFFER_CONFIG * const p, const VO_S32 y)
{
// printf("%d wait for %d\n", pThdInfo->thread_id, p->cur_buf_index);
	THD_COND_WAIT(pThdInfo->progressCond, pThdInfo->progressMutex, p->nProgress < y && !p->errFlag);
// printf("%d end wait for %d, result %x\n", pThdInfo->thread_id, p->cur_buf_index, p->errFlag);

// 	if (p->nProgress < y && !p->errFlag)
// 	{
// // 		printf("%d wait for %p\n", pThdInfo->thread_id, p);
// 		pthread_mutex_lock(pThdInfo->progressMutex);
// 		while(p->nProgress < y && !p->errFlag)
// 		{
// 			pthread_cond_wait(pThdInfo->progressCond, pThdInfo->progressMutex);
// 		}
// 		pthread_mutex_unlock(pThdInfo->progressMutex);
// // 		printf("%d end wait for %p, result %x\n", pThdInfo->thread_id, p, p->errFlag);
// 	}

	return p->errFlag;
}

//main-thread
VO_S32 DecodeFrameMain(VP8D_COMP *pDecGlobal, vpx_image_t * pOutput, unsigned int data_sz, const uint8_t * data, int64_t time_stamp, VO_S32 newInput)
{
	VO_U32 thdIdx = pDecGlobal->preThdIdx;
	vp8FrameThdInfo *pThdInfo = pDecGlobal->pFrameThdPool->thdInfo + thdIdx;
	VP8D_COMP *pSubContext = pThdInfo->pSelfGlobal;

	//M1.wait for pre-frame finish setting
	THD_COND_WAIT(pThdInfo->setupCond, pThdInfo->stateMutex, pThdInfo->state == FRAME_STATE_SETTING);
// 	if (pThdInfo->state == FRAME_STATE_SETTING)
// 	{
// 		pthread_mutex_lock(pThdInfo->stateMutex);
// 		while (pThdInfo->state == FRAME_STATE_SETTING)
// 		{
// 			pthread_cond_wait(pThdInfo->setupCond, pThdInfo->stateMutex);
// 		}
// 		pthread_mutex_unlock(pThdInfo->stateMutex);
// 	}

	//M2.update global-context from sub-context, get output frame idx
// 	printf("update %d\n", thdIdx);
	switch (newInput)
	{
	case 0:
		//no new input data, just check if output
		pDecGlobal->common.Width = pSubContext->common.Width;
		pDecGlobal->common.Height = pSubContext->common.Height;
		pDecGlobal->common.p_output = pSubContext->common.p_output;
		pDecGlobal->common.output_size = pSubContext->common.output_size;
		GetOutputPic(pDecGlobal, pOutput);
		pSubContext->common.output_size = pDecGlobal->common.output_size;
		return VO_ERR_NONE;
		break;
	case 1:
		//new input data
		UpdateContextFromSub(pDecGlobal, pSubContext);
		GetOutputPic(pDecGlobal, pOutput);
		break;
	case 2:
		//flush command from API
		{
			//just wait for all thread done
			VO_U32 i;
			for (i = 0; i < pDecGlobal->nThdNum; ++i)
			{
				vp8FrameThdInfo *pThdInfo_t = pDecGlobal->pFrameThdPool->thdInfo+i;
				THD_COND_WAIT(pThdInfo_t->readyCond, pThdInfo_t->stateMutex, pThdInfo_t->state != FRAME_STATE_READY);
			}
		}
		return VO_ERR_NONE;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	//M3.wait for current sub-thd ready
	if (thdIdx < pDecGlobal->nThdNum-1)
	{
		++thdIdx;
		++pThdInfo;
	}
	else
	{
		thdIdx = 0;
		pThdInfo = pDecGlobal->pFrameThdPool->thdInfo;
	}
	THD_COND_WAIT(pThdInfo->readyCond, pThdInfo->stateMutex, pThdInfo->state != FRAME_STATE_READY);
	pSubContext = pThdInfo->pSelfGlobal;

	if (pThdInfo->errFlag)
	{
		VO_U32 i;
// printf("error, start reset state\n");
		for (i = 0; i < pDecGlobal->nThdNum; ++i)
		{
			vp8FrameThdInfo *pThdInfo_t = pDecGlobal->pFrameThdPool->thdInfo+i;
			THD_COND_WAIT(pThdInfo_t->readyCond, pThdInfo_t->stateMutex, pThdInfo_t->state != FRAME_STATE_READY);
		}
// printf("end reset state\n");

		if (newInput == 1)
		{
			while (pOutput->type == VO_VIDEO_FRAME_NULL && pOutput->Flag > 0)
			{
				GetOutputPic(pDecGlobal, pOutput);
			}
		}
	}

	//M4.copy global-context to sub-context
// printf("copy %d\n", thdIdx);
	CopyContextToSub(pDecGlobal, pSubContext);
	{
		VO_U32 size = data_sz/INPUT_SIZE_INC_STEP;
		if (size >= pThdInfo->dataSize)
		{
			if (pThdInfo->data)
			{
				vpx_free(pThdInfo->data);
			}
			pThdInfo->data = (VO_U8 *)vpx_memalign(32, (++size)*INPUT_SIZE_INC_STEP);
			if (pThdInfo->data == NULL)
			{
				pThdInfo->dataSize = 0;
				return VO_ERR_OUTOF_MEMORY;
			}
			pThdInfo->dataSize = size;
printf("increase thd %d input buf to %d\n", thdIdx, size);
		}
	}
	memcpy(pThdInfo->data, data, data_sz);
	pSubContext->in_data = pThdInfo->data;
	pSubContext->data_sz = data_sz;
	pSubContext->timestamp = time_stamp;
// printf("nalu time %lld\n", pDecGlobal->nalu->Time);
	//M5.switch sub's state to setting, signal to S1.1
	pthread_mutex_lock(pThdInfo->stateMutex);
	pThdInfo->state = FRAME_STATE_SETTING;
	pthread_mutex_unlock(pThdInfo->stateMutex);
	pthread_cond_signal(pThdInfo->setupCond);
	pDecGlobal->preThdIdx = thdIdx;

//////////////////////////////////////////////////////////////////////////
//Only for sync output. Because we use async output instead, it will do nothing in M7&M6
	//M6.wait for output finish_flag


	//M7.output a finished frame data
//////////////////////////////////////////////////////////////////////////

	return VO_ERR_NONE;
}


//sub-thread
static THREAD_FUNCTION VP8DecThreadFunc2(void *pArg)
{
	THREAD_FUNCTION_RETURN exitCode = 0;
	voVP8ThreadParam2 *param = (voVP8ThreadParam2 *)pArg;
	const VP8D_COMP *pDecGlobal_m = param->pDecGlobal;
	vp8FrameThdInfo *selfInfo = param->selfInfo;
	VO_U32 nThdIdx = selfInfo - pDecGlobal_m->pFrameThdPool->thdInfo;
	VP8D_COMP *pDecGlobal = selfInfo->pSelfGlobal;
	VP8_COMMON *cm;

	selfInfo->bRunning = 1;

	pDecGlobal->pThdInfo = selfInfo;
	while (selfInfo->runCMD)
	{
		//S1.ready
		//S1.1 wait for main-thd submit task
		THD_COND_WAIT(selfInfo->setupCond, selfInfo->stateMutex, selfInfo->state != FRAME_STATE_SETTING);
// 		if (selfInfo->state != FRAME_STATE_SETTING)
// 		{
// 			pthread_mutex_lock(selfInfo->stateMutex);
// 			while (selfInfo->state != FRAME_STATE_SETTING)
// 			{
// 				pthread_cond_wait(selfInfo->setupCond, selfInfo->stateMutex);
// 			}
// 			pthread_mutex_unlock(selfInfo->stateMutex);
// 		}
		if (!selfInfo->runCMD)
		{
			break;
		}

		selfInfo->errFlag = 0;
		selfInfo->usedBytes = 0;
		{
			//S2.setting
			int retcode = 0;
frameThdStartSetting:

			cm = &pDecGlobal->common;
			cm->error.error_code = VPX_CODEC_OK;

			if (pDecGlobal->num_fragments == 0)
			{
				/* New frame, reset fragment pointers and sizes */
				vpx_memset((void *)pDecGlobal->fragments, 0, sizeof(pDecGlobal->fragments));
				vpx_memset(pDecGlobal->fragment_sizes, 0, sizeof(pDecGlobal->fragment_sizes));
			}
// 			if (pDecGlobal->input_fragments && !(pDecGlobal->in_data == NULL && pDecGlobal->data_sz == 0))
// 			{
// 				/* Store a pointer to this fragment and return. We haven't
// 				* received the complete frame yet, so we will wait with decoding.
// 				*/
// 				assert(pDecGlobal->num_fragments < MAX_PARTITIONS);
// 				pDecGlobal->fragments[pDecGlobal->num_fragments] = pDecGlobal->in_data;
// 				pDecGlobal->fragment_sizes[pDecGlobal->num_fragments] = pDecGlobal->data_sz;
// 				pDecGlobal->num_fragments++;
// 				if (pDecGlobal->num_fragments > (1 << EIGHT_PARTITION) + 1)
// 				{
// 					pDecGlobal->common.error.error_code = VPX_CODEC_UNSUP_BITSTREAM;
// 					pDecGlobal->common.error.setjmp = 0;
// 					pDecGlobal->num_fragments = 0;
// 					return -1;
// 				}
// 				return 0;
// 			}

			if (!pDecGlobal->input_fragments)
			{
				pDecGlobal->fragments[0] = pDecGlobal->in_data;
				pDecGlobal->fragment_sizes[0] = pDecGlobal->data_sz;
				pDecGlobal->num_fragments = 1;
			}
			assert(pDecGlobal->common.multi_token_partition <= EIGHT_PARTITION);
			if (pDecGlobal->num_fragments == 0)
			{
				pDecGlobal->num_fragments = 1;
				pDecGlobal->fragments[0] = NULL;
				pDecGlobal->fragment_sizes[0] = 0;
			}

// 			if (!pDecGlobal->ec_active &&
// 				pDecGlobal->num_fragments <= 1 && pDecGlobal->fragment_sizes[0] == 0)
// 			{
// 				/* If error concealment is disabled we won't signal missing frames
// 				* to the decoder.
// 				*/
// 				if (cm->fb_idx_ref_cnt[cm->lst_fb_idx] > 1)
// 				{
// 					/* The last reference shares buffer with another reference
// 					* buffer. Move it to its own buffer before setting it as
// 					* corrupt, otherwise we will make multiple buffers corrupt.
// 					*/
// 					const int prev_idx = cm->lst_fb_idx;
// 					cm->fb_idx_ref_cnt[prev_idx]--;
// 					cm->lst_fb_idx = get_free_fb(cm);
// 					vp8_yv12_copy_frame(&cm->yv12_fb[prev_idx],
// 						&cm->yv12_fb[cm->lst_fb_idx]);
// 				}
// 				/* This is used to signal that we are missing frames.
// 				* We do not know if the missing frame(s) was supposed to update
// 				* any of the reference buffers, but we act conservative and
// 				* mark only the last buffer as corrupted.
// 				*/
// 				cm->yv12_fb[cm->lst_fb_idx].corrupted = 1;
// 
// 				/* Signal that we have no frame to show. */
// 				cm->show_frame = 0;
// 
// 				pDecGlobal->num_fragments = 0;
// 
// 				/* Nothing more to do. */
// 				return 0;
// 			}
			pDecGlobal->common.error.setjmp = 1;

			if (selfInfo->errFlag = vp8_prepare_decode(pDecGlobal) < 0)
			{
				goto frameThdEndOfSetting;
			}

			cm->yv12_fb[cm->new_fb_idx].Time = pDecGlobal->timestamp;
			cm->yv12_fb[cm->new_fb_idx].type = cm->frame_type;
frameThdEndOfSetting:
			if (selfInfo->errFlag)
			{
				//skip the decoding state
				pthread_mutex_lock(selfInfo->stateMutex);
				selfInfo->state = FRAME_STATE_READY;
				pthread_mutex_unlock(selfInfo->stateMutex);
				pthread_cond_signal(selfInfo->setupCond);
				pthread_cond_signal(selfInfo->readyCond);
				continue;
			}
// printf("thd %d: end setting, used %d\n", nThdIdx, selfInfo->usedBytes);

			//S2.3 switch to decoding state, signal to M1
			pthread_mutex_lock(selfInfo->stateMutex);
			selfInfo->state = FRAME_STATE_DECODING;
			pthread_mutex_unlock(selfInfo->stateMutex);
			pthread_cond_signal(selfInfo->setupCond);
			
			//S3.decoding
				//S3.1 for every MB: parse mv&cof, wait for ref progress, do MC&idct
				//S3.2 end of row: deblock, update progress, broadcast
				//S3.3 end of slice: ref_list, (deblock, update progress, broadcast)

			retcode = vp8_decode_frame(pDecGlobal);
			if (retcode != 0)
			{
				selfInfo->errFlag = retcode;
				goto frameThdEndOfDecoding;
			}

//			pthread_mutex_lock(selfInfo->progressMutex);
//			cm->frame_to_show->nProgress = INT_MAX;
//			pthread_cond_broadcast(selfInfo->progressCond);
//#if USE_WIN32_BROADCAST
//#if defined(VOWINCE) || defined(WIN32)
//			{
//				VO_U32 i;
//				for (i = pDecGlobal->nThdNum-1; i > 0; --i)
//				{
//					pthread_cond_signal(selfInfo->progressCond);
//				}
//			}
//#endif
//#endif
//			pthread_mutex_unlock(selfInfo->progressMutex);

// 			if(cm->filter_level)
// 			{
// 				/* Apply the loop filter if appropriate. */
// 				vp8_loop_filter_frame(cm, &pDecGlobal->mb);
// 			}
// 			vp8_yv12_extend_frame_borders(cm->frame_to_show);			pDecGlobal->ready_for_new_data = 0;
			pDecGlobal->last_time_stamp = pDecGlobal->timestamp;
			pDecGlobal->num_fragments = 0;

			pDecGlobal->common.error.setjmp = 0;

frameThdEndOfDecoding:
			;
		}
		

		//S3.4 end of pic: update errFlag/finish_flag in poc_list, signal to M6
		pthread_mutex_lock(selfInfo->outputMutex);
		if (selfInfo->errFlag)
		{
printf("set error flag to pic %p\n", cm->frame_to_show);

			pthread_mutex_lock(selfInfo->progressMutex);
			cm->frame_to_show->errFlag = selfInfo->errFlag;
			pthread_cond_broadcast(selfInfo->progressCond);
#if USE_WIN32_BROADCAST
#if defined(VOWINCE) || defined(WIN32)
			{
				VO_U32 i;
				for (i = pDecGlobal->nThdNum-1; i > 0; --i)
				{
					pthread_cond_signal(selfInfo->progressCond);
				}
			}
#endif
#endif
			pthread_mutex_unlock(selfInfo->progressMutex);

		}
		else
		{
// 			pthread_mutex_lock(selfInfo->progressMutex);
// 			cm->frame_to_show->nProgress = INT_MAX;
//			pthread_cond_broadcast(selfInfo->progressCond);
// #if USE_WIN32_BROADCAST
// #if defined(VOWINCE) || defined(WIN32)
// 			{
// 				VO_U32 i;
// 				for (i = pDecGlobal->nThdNum-1; i > 0; --i)
// 				{
// 					pthread_cond_signal(selfInfo->progressCond);
// 				}
// 			}
// #endif
// #endif
// 			pthread_mutex_unlock(selfInfo->progressMutex);

			//update finish_flag
			pDecGlobal->common.frame_to_show->removable = 1;
// 			printf("%p done, type %d\n", pDecGlobal->common.frame_to_show, pDecGlobal->common.frame_to_show->type);
		}
		pthread_mutex_unlock(selfInfo->outputMutex);


		//S3.5 switch to ready state, signal to M3
		pthread_mutex_lock(selfInfo->stateMutex);
		selfInfo->state = FRAME_STATE_READY;
		pthread_mutex_unlock(selfInfo->stateMutex);
		pthread_cond_signal(selfInfo->readyCond);

	}

	vp8mt_de_alloc_temp_buffers(pDecGlobal, pDecGlobal->common.mb_rows);
	vp8_de_alloc_priv_buffer(&pDecGlobal->common);

	selfInfo->bRunning = 0;
	return exitCode;
}

VO_U32 InitVP8Threads2(VP8D_COMP *pDecGlobal)
{
	VO_U32 i;
	voVP8ThreadParam2 param;
	vp8FrameThdPool *pThdPool;
	assert(pDecGlobal->nThdNum>=1 && pDecGlobal->nThdNum<=MAX_THREAD_NUM2);

	pDecGlobal->pFrameThdPool = pThdPool = (vp8FrameThdPool *)vpx_memalign(32, sizeof(vp8FrameThdPool));
	if (pThdPool == NULL)
	{
		return VO_ERR_FAILED;
	}

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

	if (pthread_mutex_init(&pThdPool->progressMutex, NULL))
	{
		return VO_ERR_FAILED;
	}
	if (pthread_mutex_init(&pThdPool->outputMutex, NULL))
	{
		return VO_ERR_FAILED;
	}
	if (pthread_cond_init(&pThdPool->progressCond, NULL))
	{
		return VO_ERR_FAILED;
	}
// 	if (pthread_cond_init(&pThdPool->outputCond, NULL))
// 	{
// 		return VO_ERR_FAILED;
// 	}

	pThdPool->pThdContext = vpx_memalign(32, pDecGlobal->nThdNum*sizeof(VP8D_COMP));
	if (pThdPool->pThdContext == NULL)
	{
		return VO_ERR_FAILED;
	}

// 	pThdPool->pThdPriv = (VO_U8*)voMalloc(pDecGlobal, pDecGlobal->nThdNum*MAX_INPUT_SIZE);
// 	if (pThdPool->pThdPriv == NULL)
// 	{
// 		return VO_ERR_FAILED;
// 	}

	param.pDecGlobal = pDecGlobal;
	for (i = 0; i < pDecGlobal->nThdNum; i++)
	{
		vp8FrameThdInfo *pInfo = pThdPool->thdInfo+i;
		param.selfInfo = pInfo;
		pInfo->stateMutex = pThdPool->stateMutex+i;
		pInfo->progressMutex = &pThdPool->progressMutex;
		pInfo->outputMutex = &pThdPool->outputMutex;
		pInfo->setupCond = pThdPool->setupCond+i;
		pInfo->readyCond = pThdPool->readyCond+i;
		pInfo->progressCond = &pThdPool->progressCond;
// 		pInfo->outputCond = &pThdPool->outputCond;
		pInfo->pDecGlobal = pDecGlobal;
		pInfo->pSelfGlobal = (VP8D_COMP *)pThdPool->pThdContext + i;
// 		pInfo->data = pThdPool->pThdPriv + i*MAX_INPUT_SIZE;
		pInfo->data = (VO_U8*)vpx_memalign(32, INPUT_SIZE_INC_STEP);
		if (pInfo->data == NULL)
		{
			pInfo->dataSize = 0;
			return VO_ERR_FAILED;
		}
		pInfo->dataSize = 1;
		//update global & slice
		*(pInfo->pSelfGlobal) = *(pDecGlobal);
// 		alloc_private_params(pInfo->pSelfGlobal);

		pInfo->state = FRAME_STATE_READY;
		pInfo->runCMD = 1;
		pInfo->bRunning = 0;
		if (pthread_create((pthread_t *)(&(pInfo->thread_id)), NULL, VP8DecThreadFunc2, &param))
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

VO_U32 ReleaseVP8Threads2(VP8D_COMP *pDecGlobal)
{
	vp8FrameThdPool *pThdPool = pDecGlobal->pFrameThdPool;
	vp8FrameThdInfo *pInfo = pThdPool->thdInfo;
	VO_U32 nSubThd = pDecGlobal->nThdNum;
	VO_U32 voRC = VO_ERR_NONE;
	VO_U32 i;
	VO_S32 tryCount;
#define MAX_TRY_EXIT_COUNT 100

	for (i = nSubThd; i != 0; --i,++pInfo)
	{
		pInfo->runCMD = 0;
	}
	//broadcast all cond

	for (i = nSubThd, pInfo = pThdPool->thdInfo; i != 0; --i,++pInfo)
	{
printf("try ending sub-thd %d\n", nSubThd-i);
		tryCount = MAX_TRY_EXIT_COUNT;
#undef MAX_TRY_EXIT_COUNT
		while (tryCount != 0)
		{
			if (pInfo->bRunning)
			{
				//signal
				pthread_mutex_lock(pInfo->stateMutex);
				pInfo->state = FRAME_STATE_SETTING;
				pthread_mutex_unlock(pInfo->stateMutex);
				pthread_cond_signal(pInfo->setupCond);
				--tryCount;
				thread_sleep(1);
			}
			else
			{
printf("sub-thd %d is end\n", nSubThd-i);
				break;
			}
		}
		if (tryCount > 0)
		{
			pthread_join(pInfo->thread_id, NULL);
		}
		else
		{
#ifdef _IOS
			pthread_join(pInfo->thread_id, NULL);
#else
			pthread_cancel(pInfo->thread_id);	//Note: it has no effect in Android system
			pInfo->bRunning = 0;
			voRC |= VO_ERR_FAILED;
#endif
		}
		pInfo->thread_id = (pthread_t)NULL;
// 		free_private_param(pInfo->pSelfGlobal);
		if (pInfo->data && pInfo->dataSize > 0)
		{
			vpx_free(pInfo->data);
		}
	}

// 	if (pThdPool->pThdPriv)
// 	{
// 		voFree(pDecGlobal, pThdPool->pThdPriv);
// 	}

	if (pThdPool->pThdContext)
	{
		vpx_free(pThdPool->pThdContext);
	}

// 	if (pthread_cond_destroy(&pThdPool->outputCond))
// 	{
// 		voRC |= VO_ERR_FAILED;
// 	}
	if (pthread_cond_destroy(&pThdPool->progressCond))
	{
		voRC |= VO_ERR_FAILED;
	}
	if (pthread_mutex_destroy(&pThdPool->outputMutex))
	{
		voRC |= VO_ERR_FAILED;
	}
	if (pthread_mutex_destroy(&pThdPool->progressMutex))
	{
		voRC |= VO_ERR_FAILED;
	}

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
		vpx_free(pThdPool);
		pThdPool = NULL;
	}

	return voRC;
}
#endif