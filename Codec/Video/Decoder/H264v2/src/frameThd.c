#include "global.h"
#include "memalloc.h"
#include "vlc.h"
#include "cabac.h"
#include "nalu.h"
#include "voH264Readbits.h"
#include "header.h"
#include "parset.h"
#include "macroblock.h"
#include "quant.h"
#include "biaridecod.h"
#include "image.h"
#include "output.h"
//#include "frameThd.h"

#if USE_FRAME_THREAD

#define H264_IS_ANNEXB(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)==1))
#define H264_IS_ANNEXB2(p) ( !(*((p)+0)) && !(*((p)+1)) && !(*((p)+2))&& (*((p)+3)==1))
static VO_U8* GetNextNalu(VO_U8* currPos,int size)
{
	VO_U8* p = currPos;  
	VO_U8* endPos = currPos+size-4;
	for (; p < endPos; p++)
	{
		if (H264_IS_ANNEXB(p))
			return p+3;
		else if(H264_IS_ANNEXB2(p))
			return p+4;
	}
	if(p>=endPos)
		p = NULL;

	return p;
}
#undef H264_IS_ANNEXB
#undef H264_IS_ANNEXB2

static void UpdateContextFromSub(H264DEC_G *pDecGlobal, H264DEC_G *subContext)
{
	//to be fix
	pDecGlobal->number = subContext->number;
	pDecGlobal->pre_frame_num = subContext->pre_frame_num;
	pDecGlobal->previous_frame_num = subContext->previous_frame_num;
	pDecGlobal->MaxFrameNum = subContext->MaxFrameNum;
	pDecGlobal->non_conforming_stream = subContext->non_conforming_stream;
	pDecGlobal->width = subContext->width;
	pDecGlobal->height = subContext->height;
	pDecGlobal->width_cr = subContext->width_cr;
	pDecGlobal->height_cr = subContext->height_cr;
	pDecGlobal->direct_set = subContext->direct_set;
	pDecGlobal->vFrameIndexFIFO = subContext->vFrameIndexFIFO;
	pDecGlobal->pic_head = subContext->pic_head;
	pDecGlobal->pic_end = subContext->pic_end;
	pDecGlobal->PrevPicOrderCntMsb = subContext->PrevPicOrderCntMsb;
	pDecGlobal->PrevPicOrderCntLsb = subContext->PrevPicOrderCntLsb;
	pDecGlobal->PreviousFrameNum = subContext->PreviousFrameNum;
	pDecGlobal->FrameNumOffset = subContext->FrameNumOffset;
	pDecGlobal->ExpectedDeltaPerPicOrderCntCycle = subContext->ExpectedDeltaPerPicOrderCntCycle;
	pDecGlobal->PreviousFrameNumOffset = subContext->PreviousFrameNumOffset;
	pDecGlobal->PicWidthInMbs = subContext->PicWidthInMbs;
	pDecGlobal->FrameHeightInMbs = subContext->FrameHeightInMbs;
	pDecGlobal->no_output_of_prior_pics_flag = subContext->no_output_of_prior_pics_flag;
	pDecGlobal->last_has_mmco_5 = subContext->last_has_mmco_5;
	pDecGlobal->last_pic_bottom_field = subContext->last_pic_bottom_field;
	pDecGlobal->recovery_point = subContext->recovery_point;
	pDecGlobal->recovery_point_found = subContext->recovery_point_found;
	pDecGlobal->recovery_frame_cnt = subContext->recovery_frame_cnt;
	pDecGlobal->recovery_frame_num = subContext->recovery_frame_num;
	pDecGlobal->recovery_poc = subContext->recovery_poc;
// 	pDecGlobal->LastAccessUnitExists = subContext->LastAccessUnitExists;
// 	pDecGlobal->NALUCount = subContext->NALUCount;
// 	pDecGlobal->global_init_done = subContext->global_init_done;
// 	pDecGlobal->old_pps_id2 = subContext->old_pps_id2;
// 	pDecGlobal->PicPos = subContext->PicPos;
	pDecGlobal->output_size = subContext->output_size;
	pDecGlobal->spsNum = subContext->spsNum;
	pDecGlobal->cabac_init_done = subContext->cabac_init_done;
}

static void CopyContextToSub(H264DEC_G *pDecGlobal, H264DEC_G *pSubContext)
{
	//to be fix
	pSubContext->SeqParSet = pDecGlobal->SeqParSet;
	pSubContext->PicParSet = pDecGlobal->PicParSet;
	pSubContext->dec_ref_pic_marking_buffer = pDecGlobal->dec_ref_pic_marking_buffer;
	pSubContext->p_Dpb = pDecGlobal->p_Dpb;
	pSubContext->customMem2 = pDecGlobal->customMem2;
	pSubContext->p_output = pDecGlobal->p_output;
	pSubContext->interlace= pDecGlobal->interlace;
	pSubContext->first_sps=pDecGlobal->first_sps;

	UpdateContextFromSub(pSubContext, pDecGlobal);
}

inline VO_S32 WaitForProgress(FrameThdInfo * const pThdInfo, const StorablePicture * const p, const VO_S32 y, const VO_U32 field)
{
// 	if(p->frame_real != NULL)	//to be fix
// 	{
// 		return VO_H264_ERR_LIST_REORDERING;
// 	}
//	printf("%d wait for %d %x,process:%d cur_y:%d\n", pThdInfo->thread_id, p->poc,p,p->nProgress[field],y);
	THD_COND_WAIT(pThdInfo->progressCond, pThdInfo->progressMutex, p->nProgress[field] < y && !p->errFlag);
 //printf("%d end wait for %d, result %x\n", pThdInfo->thread_id, p->poc, p->errFlag);

// printf("%d wait for %d\n", pThdInfo->thread_id, p->cur_buf_index);
// 	pthread_mutex_lock(pThdInfo->progressMutex);
// 	if (p->nProgress[field] < y && !p->errFlag)
// 	{
// 		do
// 		{
// 			pthread_cond_wait(pThdInfo->progressCond, pThdInfo->progressMutex);
// 		}while (p->nProgress[field] < y && !p->errFlag);
// 	}
// 	pthread_mutex_unlock(pThdInfo->progressMutex);
// printf("%d end wait for %d, result %x\n", pThdInfo->thread_id, p->cur_buf_index, p->errFlag);

	return p->errFlag;
}

//main-thread
VO_S32 DecodeFrameMain(H264DEC_G *pDecGlobal, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutInfo, VO_S32 newInput)
{
	VO_U32 thdIdx = pDecGlobal->preThdIdx;
	FrameThdInfo *pThdInfo = pDecGlobal->pFrameThdPool->thdInfo + thdIdx;
	H264DEC_G *pSubContext = pThdInfo->pSelfGlobal;
    VO_S32 ret = VO_ERR_NONE;
	//M1.wait for pre-frame finish setting
	THD_COND_WAIT(pThdInfo->readyCond, pThdInfo->stateMutex, pThdInfo->state == FRAME_STATE_SETTING);
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
		pDecGlobal->output_size = pSubContext->output_size;
		GetOutputPic(pDecGlobal, pOutput, pOutInfo);
		pSubContext->output_size = pDecGlobal->output_size;
		return VO_ERR_NONE;
		break;
	case 1:
		//new input data
		UpdateContextFromSub(pDecGlobal, pSubContext);
		GetOutputPic(pDecGlobal, pOutput, pOutInfo);
		break;
	case 2:
		//flush command from API, must use sub's context to do flush
		flush_dpb(pSubContext, pSubContext->p_Dpb);
		update_ref_list(pSubContext->p_Dpb);
		update_ltref_list(pSubContext->p_Dpb);
		return VO_ERR_NONE;
		break;
#if COMBINE_FRAME
	case 3:
		//only decode frame, but don't output anything
		//used for combine frame before flush command
		UpdateContextFromSub(pDecGlobal, pSubContext);
		break;
#endif
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
			FrameThdInfo *pThdInfo_t = pDecGlobal->pFrameThdPool->thdInfo+i;
			THD_COND_WAIT(pThdInfo_t->readyCond, pThdInfo_t->stateMutex, pThdInfo_t->state != FRAME_STATE_READY);
		}
// printf("end reset state\n");

		//clean fs, output, remove
		{
			DecodedPictureBuffer *p_Dpb = pDecGlobal->p_Dpb;
			if (pSubContext->dec_picture != NULL)
			{
				VO_U32 i;

				//delete from fs
				for (i = 0; i < p_Dpb->used_size; ++i)
				{
					if (p_Dpb->fs[i]->frame&&p_Dpb->fs[i]->frame->cur_buf_index == pSubContext->dec_picture->cur_buf_index)
					{
						break;
					}
					if (p_Dpb->fs[i]->top_field&&p_Dpb->fs[i]->top_field->cur_buf_index == pSubContext->dec_picture->cur_buf_index)
					{
						break;
					}
					if (p_Dpb->fs[i]->bottom_field&&p_Dpb->fs[i]->bottom_field->cur_buf_index == pSubContext->dec_picture->cur_buf_index)
					{
						break;
					}
				}
				if (i < p_Dpb->used_size)
				{
					FrameStore *tmp = p_Dpb->fs[i];

					for (; i<p_Dpb->used_size-1;++i)
					{
						p_Dpb->fs[i] = p_Dpb->fs[i+1];
					}
					p_Dpb->fs[p_Dpb->used_size-1] = tmp;
					p_Dpb->used_size--;
				}
				update_ref_list(p_Dpb);
				update_ltref_list(p_Dpb);
				//add to remove list
				for (i = 0; i < p_Dpb->remove_size; ++i)
				{
					if (p_Dpb->p_remove[i]->cur_buf_index == pSubContext->dec_picture->cur_buf_index)
					{
						p_Dpb->p_remove[i]->bFinishOutput = 1;
						p_Dpb->p_remove[i]->removable =1;
						break;
					}
				}
				if (i == p_Dpb->remove_size)
				{
					p_Dpb->p_remove[i] = pSubContext->dec_picture;
					p_Dpb->p_remove[i]->bFinishOutput = 1;
					p_Dpb->p_remove[i]->removable =1;
					p_Dpb->remove_size++;
				}

				//delete from output list
				for (i = 0; i < pDecGlobal->output_size; ++i)
				{
					if (pDecGlobal->p_output[i]->cur_buf_index == pSubContext->dec_picture->cur_buf_index)
					{
						break;
					}
				}
				if (i < pDecGlobal->output_size)
				{
					for (; i < pDecGlobal->output_size-1; ++i)
					{
						pDecGlobal->p_output[i] = pDecGlobal->p_output[i+1];
					}
					pDecGlobal->output_size--;
				}
				
//printf("clean pic %d, %x\n", pSubContext->dec_picture->cur_buf_index, pThdInfo->errFlag);
				pSubContext->dec_picture = NULL;
			}
			if (newInput == 1)
			{
				while (pOutput->Buffer[0] == NULL && pOutInfo->Flag > 0)
				{
					GetOutputPic(pDecGlobal, pOutput, pOutInfo);
				}
			}
			//if(pOutput!=NULL&&pOutput->Buffer[0] == NULL)
			//{
			//  VO_S32 err = pThdInfo->errFlag;
            //  pThdInfo->errFlag = 0;
			//  return err;
			//}
			ret = pThdInfo->errFlag;
		}
	}

	//M4.copy global-context to sub-context
// printf("copy %d\n", thdIdx);
	CopyContextToSub(pDecGlobal, pSubContext);
	{
		VO_U32 size = pDecGlobal->nalu->len/INPUT_SIZE_INC_STEP;
		if (size >= pThdInfo->dataSize)
		{
			if (pThdInfo->data)
			{
				voFree(pDecGlobal, pThdInfo->data);
			}
			pThdInfo->data = (VO_U8 *)voMalloc(pDecGlobal, (++size)*INPUT_SIZE_INC_STEP);
			if (pThdInfo->data == NULL)
			{
				pThdInfo->dataSize = 0;
				return VO_ERR_OUTOF_MEMORY;
			}
			pThdInfo->dataSize = size;
//printf("increase thd %d input buf to %d\n", thdIdx, size);
		}
	}
	memcpy(pThdInfo->data, pDecGlobal->nalu->buf, pDecGlobal->nalu->len);
	pSubContext->nalu->buf = pThdInfo->data;
	pSubContext->nalu->len = pDecGlobal->nalu->len;
	pSubContext->nalu->Time = pDecGlobal->nalu->Time;
	pSubContext->nalu->UserData = pDecGlobal->nalu->UserData;
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

	return ret;
}

extern int process_one_nalu(H264DEC_G *pDecGlobal,Slice *currSlice);
extern VO_S32 init_picture(H264DEC_G *pDecGlobal, Slice *currSlice);
extern int init_picture_decoding(H264DEC_G *pDecGlobal,Slice *currSlice);
extern VO_S32 backup_slice_ref(H264DEC_G *pDecGlobal,Slice *currSlice);
extern void alloc_private_params( H264DEC_G *pDecGlobal);
extern void free_private_param(H264DEC_G *pDecGlobal);

//sub-thread
static THREAD_FUNCTION H264DecThreadFunc2(void *pArg)
{
	THREAD_FUNCTION_RETURN exitCode = 0;
	voH264ThreadParam2 *param = (voH264ThreadParam2 *)pArg;
	const H264DEC_G *pDecGlobal_m = param->pDecGlobal;
	FrameThdInfo *selfInfo = param->selfInfo;
	VO_U32 nThdIdx = selfInfo - pDecGlobal_m->pFrameThdPool->thdInfo;
	H264DEC_G *pDecGlobal = selfInfo->pSelfGlobal;
	H264DEC_L DecLocal={0};
	VO_S32 ret;
	//to backup all slice info
	VO_U32 sliceNum;
	VO_U8 *slicePos[MAX_NUM_SLICES];
	VO_U32 sliceLen[MAX_NUM_SLICES];
	pic_parameter_set_rbsp_t slice_pps[MAX_NUM_SLICES];
// 	seq_parameter_set_rbsp_t slice_sps[MAX_NUM_SLICES];
// 	VO_U32 sliceListSize[MAX_NUM_SLICES][6];
// 	struct storable_picture *sliceList[MAX_NUM_SLICES][6][MAX_LIST_SIZE];

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
			//process nalus
			Slice *currSlice = pDecGlobal->pCurSlice;
			NALU_t *nalu = pDecGlobal->nalu;
			VO_U8* const inBuffer= nalu->buf;
			VO_U32 const BufferLen = nalu->len; 
			VO_U8* nextFrame,*currFrame;
			VO_S32 leftSize=BufferLen,inSize;
			VO_U32 bFindFirstSlice;
			Bitstream *currStream = NULL;
// printf("thd %d: input %d\n", nThdIdx, BufferLen);
			if (BufferLen <=4)
			{
				selfInfo->errFlag = VO_ERR_INPUT_BUFFER_SMALL;
				goto frameThdEndOfSetting;
			}
			nextFrame = currFrame = inBuffer;
			currFrame = GetNextNalu(currFrame,leftSize);
			if (currFrame!=NULL)
			{
				leftSize-=(currFrame-inBuffer);
			}
			//S2.setting
			//S2.1 parse nalu until find a slice (sps/pps/sei)
frameThdStartSetting:
			bFindFirstSlice = 0;
			while (leftSize>=3&&currFrame!=NULL)
			{
				nextFrame = GetNextNalu(currFrame,leftSize);
				if(nextFrame)
				{
					inSize = nextFrame-currFrame;
				}
				else
				{
					inSize = leftSize;
				}
				if(inSize >=2)
				{
					nalu->buf = currFrame;
					nalu->len = inSize;
					CheckZeroByteNonVCL(pDecGlobal, nalu);
					nalu->forbidden_bit     = (*(nalu->buf) >> 7) & 1;
					nalu->nal_reference_idc = (NalRefIdc) ((*(nalu->buf) >> 5) & 3);
					nalu->nal_unit_type     = (NaluType) ((*(nalu->buf)) & 0x1f);

					if (nalu->nal_unit_type != NALU_TYPE_SLICE && nalu->nal_unit_type != NALU_TYPE_IDR)
					{
						ret = process_one_nalu(pDecGlobal,currSlice);
// 						if (iRet)
// 						{
// 
// 						}
					}
					else
					{
						bFindFirstSlice = 1;
						break;
					}
				}
				leftSize-=inSize;
				currFrame=nextFrame;
			}

			//S2.2 parse all slice head (update ref_list, poc_list)
			if (bFindFirstSlice)
			{
				//parse first slice head
				pic_parameter_set_rbsp_t *pps = slice_pps;
				seq_parameter_set_rbsp_t *sps;

// printf("thd %d: Find First Slice at %d\n", nThdIdx, nalu->buf - inBuffer);

				nalu->len = EBSPtoRBSP(nalu->buf, nalu->len, 1);
				if (nalu->forbidden_bit)
				{
					//ignore this nalu
					leftSize-=inSize;
					currFrame=nextFrame;
					goto frameThdStartSetting;
				}

				//YU SEI recovery point radom access
				if (pDecGlobal->recovery_point || nalu->nal_unit_type == NALU_TYPE_IDR)
				{
					if (pDecGlobal->recovery_point_found == 0)
					{
						if (nalu->nal_unit_type != NALU_TYPE_IDR)
						{
							//printf("Warning: Decoding does not start with an IDR picture.\n");
							pDecGlobal->non_conforming_stream = 1;
						}
						else
							pDecGlobal->non_conforming_stream = 0;
					}
					pDecGlobal->recovery_point_found = 1;
				}

				currSlice->is_reset_coeff = FALSE;
				currSlice->idr_flag = (nalu->nal_unit_type == NALU_TYPE_IDR);
				currSlice->nal_reference_idc = nalu->nal_reference_idc;
				currStream = currSlice->partArr[0].bitstream;
				currStream->frame_bitoffset = currStream->read_len = 0;
				currStream->streamBuffer = &nalu->buf[1];
				currStream->code_len = currStream->bitstream_length = RBSPtoSODB(currStream->streamBuffer, nalu->len-1);
				//if (currStream->code_len<=10)
				//{
				//  VOH264ERROR(VO_H264_ERR_NALU_SMALL);
				//}
				InitBitStream(pDecGlobal,currStream->streamBuffer, currStream->code_len);


				ret = FirstPartOfSliceHeader(pDecGlobal,currSlice);
				if(ret)
				{
				    leftSize-=inSize;
					currFrame=nextFrame;
					goto frameThdStartSetting;
					//selfInfo->errFlag = ret;
					//selfInfo->usedBytes = bytepos(pDecGlobal) - inBuffer;
					//goto frameThdEndOfSetting;
				}
				if (pDecGlobal->recovery_point_found == 0)
				{
					if (currSlice->slice_type!=I_SLICE)		   
					{	
						//ignore this nalu
						leftSize-=inSize;
					    currFrame=nextFrame;
					    goto frameThdStartSetting;
					}
					pDecGlobal->non_conforming_stream = 1;
					//printf("Warning: Decoding does not start with an IDR picture.\n");
					pDecGlobal->recovery_point_found = 1;					
				}
				if(currSlice->start_mb_nr!=0)
				{
					selfInfo->errFlag = VO_H264_ERR_FIRST_MB_OVERFLOW;
					selfInfo->usedBytes = bytepos(pDecGlobal) - inBuffer;
					goto frameThdEndOfSetting;
				}
				*pps = pDecGlobal->PicParSet[currSlice->pic_parameter_set_id];
				if (pps->Valid != TRUE)
				{
					selfInfo->errFlag = VO_H264_ERR_PPSIsNULL;
					selfInfo->usedBytes = bytepos(pDecGlobal) - inBuffer;
					goto frameThdEndOfSetting;
				}
				sps = &pDecGlobal->SeqParSet[pps->seq_parameter_set_id];
				if (sps->Valid != TRUE)
				{
					selfInfo->errFlag = VO_H264_ERR_SPSIsNULL;
					selfInfo->usedBytes = bytepos(pDecGlobal) - inBuffer;
					goto frameThdEndOfSetting;
				}
				
				activate_sps(pDecGlobal, sps);
				activate_pps(pDecGlobal, pps);
				pDecGlobal->spsNum = pps->seq_parameter_set_id;
				pDecGlobal->nal_startcode_follows = (pps->entropy_coding_mode_flag == CAVLC)?
					uvlc_startcode_follows:cabac_startcode_follows;
				currSlice->Transform8x8Mode = pps->transform_8x8_mode_flag;
				currSlice->is_not_independent = 0;
				ret = RestOfSliceHeader (pDecGlobal,currSlice);
				if(ret)
				{
					selfInfo->errFlag = ret;
					selfInfo->usedBytes = bytepos(pDecGlobal) - inBuffer;
					goto frameThdEndOfSetting;
				}
				pDecGlobal->num_dec_mb = 0;

				ret = init_picture(pDecGlobal, currSlice);
				if(ret)
				{
					selfInfo->errFlag = ret;
					selfInfo->usedBytes = bytepos(pDecGlobal) - inBuffer;
					goto frameThdEndOfSetting;
				}
				pDecGlobal->dec_picture->Time = nalu->Time;
				pDecGlobal->dec_picture->UserData = nalu->UserData;
				if(pDecGlobal->p_Dpb->last_picture==NULL)
					pDecGlobal->dec_picture->type = currSlice->slice_type;
				ret = init_picture_decoding(pDecGlobal, currSlice);
				if(ret)
				{
					selfInfo->errFlag = ret;
					selfInfo->usedBytes = bytepos(pDecGlobal) - inBuffer;
					goto frameThdEndOfSetting;
				}
				//check zero_byte if it is also the first NAL unit in the access unit
				CheckZeroByteVCL(pDecGlobal, nalu);

				backup_slice_ref(pDecGlobal,currSlice);

				ret = exit_picture(pDecGlobal, &pDecGlobal->dec_picture);
				if(ret)
				{
					selfInfo->errFlag = ret;
					selfInfo->usedBytes = bytepos(pDecGlobal) - inBuffer;
					goto frameThdEndOfSetting;
				}
				pDecGlobal->previous_frame_num = pDecGlobal->pCurSlice->frame_num;
				if(currSlice->active_pps->entropy_coding_mode_flag && !pDecGlobal->cabac_init_done)
				{
					vo_init_cabac_states();
					pDecGlobal->cabac_init_done = 1;
				}
				pDecGlobal->recovery_point = 0;

				//find other slices follow
				sliceNum = 0;
				leftSize-=inSize;
				currFrame=nextFrame;
				while (leftSize>=3&&currFrame!=NULL)
				{
					nextFrame = GetNextNalu(currFrame,leftSize);
					if(nextFrame)
					{
						inSize = nextFrame-currFrame;
					}
					else
					{
						inSize = leftSize;
					}
					if(inSize >=2)
					{
						const VO_U8 *tmp_ptr = currFrame;
						VO_U32 next_value;
						VO_U32 len;
						VO_U32 value = (*tmp_ptr++)&0x1F;
						// printf("find nalu, type %d\r\n", value);
						if (value != 1 && value != 5)
						{
							break;
						}
						if (tmp_ptr+3 < currFrame+inSize)
						{
							//slice or IDR
							next_value = value = (((VO_U32)(*tmp_ptr))<<24) | ((VO_U32)(*(tmp_ptr+1))<<16) | ((VO_U32)(*(tmp_ptr+2))<<8) | ((VO_U32)*(tmp_ptr+3));
							tmp_ptr += 4;
						}
						else
						{
							value = 0;
							for (len = 0; len < 4; ++len)
							{
								next_value = tmp_ptr < currFrame+inSize ? *(tmp_ptr++) : 0;
								value = (value << 8) | next_value;
							}
							next_value = value;
						}
						// printf("value %08X\r\n", value);
						if(value >=(1<<27))
						{
							value >>= 32-9;
							len = VO_VLC_LEN[value];
							value = VO_UE_VAL[value];
						}
						else
						{
							len = 2*vo_big_len(value)-31;
							value >>= len;
							value--;
							len = 32 - len;
						}
						if (value == 0)	
						{
							break;
						}
						//flush len
						while (len > 8)
						{
							VO_U32 tmp = tmp_ptr < currFrame+inSize ? *(tmp_ptr++) : 0;
							next_value = (next_value<<8)|tmp;
							len -= 8;
						}
						if (tmp_ptr < currFrame+inSize)
						{
							VO_U32 i;
							value = (next_value<<len)|((*tmp_ptr)>>(8-len));
							next_value = *(tmp_ptr++);
							for (i = 0; i < 3; ++i)
							{
								next_value <<= 8;
								next_value |= tmp_ptr < currFrame+inSize ? *(tmp_ptr++) : 0;
							}
							if (tmp_ptr < currFrame+inSize)
							{
								next_value = (next_value<<len)|((*tmp_ptr)>>(8-len));
							}
							else
							{
								next_value <<= len;
							}
						}
						else
						{
							value = next_value<<len;
							next_value = 0;
						}
						//parse slice type len
						if(value >=(1<<27))
						{
// 							value >>= 32-9;
							len = VO_VLC_LEN[value>>(32-9)];
// 							value = VO_UE_VAL[value];
						}
						else
						{
							len = 2*vo_big_len(value)-31;
// 							value >>= len;
// 							value--;
							len = 32 - len;
						}
						//flush len
						//assert(len <= 32);
						if (len > 32)
						{
							selfInfo->errFlag = VO_H264_ERR_TOTALZEROS_CAVLC;
							selfInfo->usedBytes = BufferLen;
							goto frameThdEndOfSetting;
						}else if (len < 32)
						{
							next_value = (value<<len)|(next_value>>(32-len));
						}
						//parse pps id
						if(next_value >=(1<<27))
						{
							next_value >>= 32-9;
							next_value = VO_UE_VAL[next_value];
						}
						else
						{
							next_value >>= 2*vo_big_len(next_value)-31;
							next_value--;
						}
						if (next_value >= MAXPPS)
						{
							selfInfo->errFlag = VO_H264_ERR_InvalidPPS;
							selfInfo->usedBytes = BufferLen;
							goto frameThdEndOfSetting;
						}

						//find next slice, save the offset
						if (sliceNum >= MAX_NUM_SLICES-1)
						{
							selfInfo->errFlag = VO_H264_ERR_MAX_SLICE_NUM;
							selfInfo->usedBytes = BufferLen;
							goto frameThdEndOfSetting;
						}

						slicePos[sliceNum] = currFrame;
						sliceLen[sliceNum] = inSize;
						sliceNum++;
// printf("new pps id %d\n", next_value);
						//backup the pps, it may be modified in other threads' setting state in the future
						slice_pps[sliceNum] = pDecGlobal->PicParSet[next_value];
// 						slice_sps[sliceNum] = pDecGlobal->SeqParSet[slice_pps[sliceNum].seq_parameter_set_id];
					}
					leftSize-=inSize;
					currFrame=nextFrame;
				}
				if (currFrame == NULL)
				{
					selfInfo->usedBytes = BufferLen;
				}
				else
				{
					selfInfo->usedBytes = currFrame - inBuffer;
				}
			}
			else
			{
				selfInfo->errFlag = VO_ERR_BASE;
				selfInfo->usedBytes = BufferLen;
			}
frameThdEndOfSetting:
			if (selfInfo->errFlag)
			{
				if (pDecGlobal->dec_picture)
				{
					pthread_mutex_lock(selfInfo->progressMutex);
					pDecGlobal->dec_picture->errFlag = selfInfo->errFlag;
					if (pDecGlobal->dec_picture->frame)
				    {
				      pDecGlobal->dec_picture->frame->errFlag = selfInfo->errFlag;
				    }
					if (pDecGlobal->dec_picture->top_field)
				    {
				      pDecGlobal->dec_picture->top_field->errFlag = selfInfo->errFlag;
				    }
				    if (pDecGlobal->dec_picture->bottom_field)
				    {
					  pDecGlobal->dec_picture->bottom_field->errFlag = selfInfo->errFlag;
				    }
// printf("%d %d errFlag! %x\n", selfInfo->thread_id, pDecGlobal->dec_picture->cur_buf_index, selfInfo->errFlag);
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
// 					pDecGlobal->dec_picture = NULL;
				}
				//skip the decoding state
				pthread_mutex_lock(selfInfo->stateMutex);
				selfInfo->state = FRAME_STATE_READY;
				pthread_mutex_unlock(selfInfo->stateMutex);
// 				pthread_cond_signal(selfInfo->setupCond);
				pthread_cond_signal(selfInfo->readyCond);
				continue;
			}
// printf("thd %d: end setting, used %d\n", nThdIdx, selfInfo->usedBytes);

			//S2.3 switch to decoding state, signal to M1
			pthread_mutex_lock(selfInfo->stateMutex);
			selfInfo->state = FRAME_STATE_DECODING;
			pthread_mutex_unlock(selfInfo->stateMutex);
			pthread_cond_signal(selfInfo->readyCond);
			
			//S3.decoding
				//S3.1 for every MB: parse mv&cof, wait for ref progress, do MC&idct
				//S3.2 end of row: deblock, update progress, broadcast
				//S3.3 end of slice: ref_list, (deblock, update progress, broadcast)

			//first slice header is parsed, only need to decode
			setup_slice_methods(pDecGlobal,currSlice);
			if(pDecGlobal->old_pps_id != currSlice->pic_parameter_set_id)
			{
				assign_quant_params (pDecGlobal,currSlice);
				pDecGlobal->old_pps_id = currSlice->pic_parameter_set_id;
			}
			if(currSlice->current_mb_nr < currSlice->start_mb_nr)
				currSlice->current_mb_nr = currSlice->start_mb_nr;
			currSlice->start_mb_nr_mbaff = currSlice->current_mb_nr;

			if (currSlice->active_pps->entropy_coding_mode_flag)
			{
				bytealign(pDecGlobal);
				vo_start_cabac(&currSlice->partArr[0].c,bytepos(pDecGlobal),currStream->code_len-(bytepos(pDecGlobal)-currStream->streamBuffer));
			}

			ret = decode_slice(pDecGlobal,currSlice);
			if(ret)
			{
				selfInfo->errFlag = ret;
				goto frameThdEndOfDecoding;
			}

			{
				//parse and decode other slices
				VO_U32 i;
				for (i = 0; i < sliceNum; ++i)
				{
					pic_parameter_set_rbsp_t *pps = slice_pps + i+1;
					seq_parameter_set_rbsp_t *sps = &pDecGlobal->SeqParSet[pps->seq_parameter_set_id];

					if (pps->Valid != TRUE)
					{
						selfInfo->errFlag = VO_H264_ERR_PPSIsNULL;
						goto frameThdEndOfDecoding;
					}
					if (sps->Valid != TRUE)
					{
						selfInfo->errFlag = VO_H264_ERR_SPSIsNULL;
						goto frameThdEndOfDecoding;
					}
					activate_sps(pDecGlobal, sps);
					if (pps->pic_parameter_set_id != pDecGlobal->old_pps_id)
					{
						activate_pps(pDecGlobal, pps);
					}

// 					pDecGlobal->oldSliceType = currSlice->slice_type;
					nalu->buf = slicePos[i];
					nalu->len = sliceLen[i];

					CheckZeroByteNonVCL(pDecGlobal, nalu);
					nalu->forbidden_bit     = (*(nalu->buf) >> 7) & 1;
					nalu->nal_reference_idc = (NalRefIdc) ((*(nalu->buf) >> 5) & 3);
					nalu->nal_unit_type     = (NaluType) ((*(nalu->buf)) & 0x1f);

					ret = process_one_nalu(pDecGlobal,currSlice);
					if (ret)
					{
						selfInfo->errFlag = ret;
						goto frameThdEndOfDecoding;
					}
				}
			}	
frameThdEndOfDecoding:
			;
		}
		

		//S3.4 end of pic: update errFlag/finish_flag in poc_list, signal to M6
		pthread_mutex_lock(selfInfo->outputMutex);
		if (selfInfo->errFlag)
		{
			//update errFlag
			if (pDecGlobal->dec_picture)
			{
				pthread_mutex_lock(selfInfo->progressMutex);
				pDecGlobal->dec_picture->errFlag = selfInfo->errFlag;
				if (pDecGlobal->dec_picture->frame)
				{
			      pDecGlobal->dec_picture->frame->errFlag = selfInfo->errFlag;
			    }
				if (pDecGlobal->dec_picture->top_field)
				{
				  pDecGlobal->dec_picture->top_field->errFlag = selfInfo->errFlag;
				}
				if (pDecGlobal->dec_picture->bottom_field)
				{
					pDecGlobal->dec_picture->bottom_field->errFlag = selfInfo->errFlag;
				}
// printf("%d %d errFlag! %x\n", selfInfo->thread_id, pDecGlobal->dec_picture->cur_buf_index, selfInfo->errFlag);
// printf("broadcast!\n");
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
// 				pDecGlobal->dec_picture=NULL;
			}
		}
		else
		{
			//update finish_flag
			if(pDecGlobal->num_dec_mb >= pDecGlobal->PicSizeInMbs)
			{
				pDecGlobal->dec_picture->removable = 1;
				//if(!pDecGlobal->direct_set)
					pDecGlobal->dec_picture=NULL;
			}
			else
			{
				//incomplete frame
				selfInfo->errFlag = VO_H264_ERR_NALU_SMALL;
				pthread_mutex_lock(selfInfo->progressMutex);
				pDecGlobal->dec_picture->errFlag = selfInfo->errFlag;
				if (pDecGlobal->dec_picture->frame)
				{
				  pDecGlobal->dec_picture->frame->errFlag = selfInfo->errFlag;
				}
				if (pDecGlobal->dec_picture->top_field)
				{
				  pDecGlobal->dec_picture->top_field->errFlag = selfInfo->errFlag;
				}
				if (pDecGlobal->dec_picture->bottom_field)
				{
					pDecGlobal->dec_picture->bottom_field->errFlag = selfInfo->errFlag;
				}
// printf("%d %d errFlag! %x\n", selfInfo->thread_id, pDecGlobal->dec_picture->cur_buf_index, selfInfo->errFlag);
// printf("broadcast!\n");
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
// 				pDecGlobal->dec_picture=NULL;
			}
		}
		pthread_mutex_unlock(selfInfo->outputMutex);
// 		pthread_cond_signal(selfInfo->outputCond);


		//S3.5 switch to ready state, signal to M3
		pthread_mutex_lock(selfInfo->stateMutex);
		selfInfo->state = FRAME_STATE_READY;
		pthread_mutex_unlock(selfInfo->stateMutex);
		pthread_cond_signal(selfInfo->readyCond);

	}

	free_global_buffers(pDecGlobal);
// 	free_private_param(pDecGlobal);
	selfInfo->bRunning = 0;
	return exitCode;
}

VO_U32 InitH264Threads2(H264DEC_G *pDecGlobal)
{
	VO_U32 i;
	voH264ThreadParam2 param;
	FrameThdPool *pThdPool;
	assert(pDecGlobal->nThdNum>=1 && pDecGlobal->nThdNum<=MAX_THREAD_NUM2);
    //VOLOGI("init thread %d\r\n",pDecGlobal->nThdNum);
	pDecGlobal->pFrameThdPool = pThdPool = (FrameThdPool *)voMalloc(pDecGlobal, sizeof(FrameThdPool));
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

	pThdPool->pThdContext = voMalloc(pDecGlobal, pDecGlobal->nThdNum*sizeof(H264DEC_G));
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
		FrameThdInfo *pInfo = pThdPool->thdInfo+i;
		param.selfInfo = pInfo;
		pInfo->stateMutex = pThdPool->stateMutex+i;
		pInfo->progressMutex = &pThdPool->progressMutex;
		pInfo->outputMutex = &pThdPool->outputMutex;
		pInfo->setupCond = pThdPool->setupCond+i;
		pInfo->readyCond = pThdPool->readyCond+i;
		pInfo->progressCond = &pThdPool->progressCond;
// 		pInfo->outputCond = &pThdPool->outputCond;
		pInfo->pDecGlobal = pDecGlobal;
		pInfo->pSelfGlobal = (H264DEC_G *)pThdPool->pThdContext + i;
// 		pInfo->data = pThdPool->pThdPriv + i*MAX_INPUT_SIZE;
		pInfo->data = (VO_U8*)voMalloc(pDecGlobal, INPUT_SIZE_INC_STEP);
		if (pInfo->data == NULL)
		{
			pInfo->dataSize = 0;
			return VO_ERR_FAILED;
		}
		pInfo->dataSize = 1;
		//update global & slice
		*(pInfo->pSelfGlobal) = *(pDecGlobal);
		alloc_private_params(pInfo->pSelfGlobal);

		pInfo->state = FRAME_STATE_READY;
		pInfo->runCMD = 1;
		pInfo->bRunning = 0;

#if defined(VOANDROID)
		if (1)
		{
			pthread_attr_t attr;
			struct sched_param sparam;
			int newprio=50; 
			pthread_attr_init(&attr); 
			pthread_attr_getschedparam(&attr, &sparam);  
			sparam.sched_priority=newprio;
			pthread_attr_setschedparam(&attr, &sparam);
			if (pthread_create((pthread_t *)(&(pInfo->thread_id)), &attr, H264DecThreadFunc2, &param))
			{
				return VO_ERR_FAILED;
			}
		}else
#endif
		{
			if (pthread_create((pthread_t *)(&(pInfo->thread_id)), NULL, H264DecThreadFunc2, &param))
			{
				return VO_ERR_FAILED;
			}
		}
		while (!pInfo->bRunning)
		{
			;
		}
	}

	return VO_ERR_NONE;
}

VO_U32 ReleaseH264Threads2(H264DEC_G *pDecGlobal)
{
	FrameThdPool *pThdPool = pDecGlobal->pFrameThdPool;
	FrameThdInfo *pInfo = pThdPool->thdInfo;
	VO_U32 nSubThd = pDecGlobal->nThdNum;
	VO_U32 voRC = VO_ERR_NONE;
	VO_U32 i;

//	for (i = nSubThd; i != 0; --i,++pInfo)
//	{
//		pInfo->runCMD = 0;
//	}
	//broadcast all cond

	for (i = nSubThd, pInfo = pThdPool->thdInfo; i != 0; --i,++pInfo)
	{
//VOLOGI("try ending sub-thd %d\r\n", nSubThd-i);
		THD_COND_WAIT(pInfo->readyCond, pInfo->stateMutex, pInfo->state != FRAME_STATE_READY);
		pInfo->runCMD = 0;

		//signal
		pthread_mutex_lock(pInfo->stateMutex);
		pInfo->state = FRAME_STATE_SETTING;
		pthread_mutex_unlock(pInfo->stateMutex);
		pthread_cond_signal(pInfo->setupCond);
		

		while (pInfo->bRunning)
		{
		  thread_sleep(1);
		}
//VOLOGI("sub-thd %d is end\r\n", nSubThd-i);

	    pthread_join(pInfo->thread_id, NULL);
		
		pInfo->thread_id = (pthread_t)NULL;
		free_private_param(pInfo->pSelfGlobal);
		if (pInfo->data && pInfo->dataSize > 0)
		{
			voFree(pDecGlobal, pInfo->data);
		}
	}

// 	if (pThdPool->pThdPriv)
// 	{
// 		voFree(pDecGlobal, pThdPool->pThdPriv);
// 	}

	if (pThdPool->pThdContext)
	{
		voFree(pDecGlobal, pThdPool->pThdContext);
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
		voFree(pDecGlobal, pThdPool);
		pThdPool = NULL;
	}

	return voRC;
}
#endif
