

#include "global.h"
#include "mbuffer.h"




void output_one_picture(H264DEC_G *pDecGlobal, StorablePicture *p)
{
  seq_parameter_set_rbsp_t *active_sps = pDecGlobal->pCurSlice->active_sps ? pDecGlobal->pCurSlice->active_sps : pDecGlobal->SeqParSet;
  int frame_mbs_only_flag = active_sps->frame_mbs_only_flag;
  int chroma_format_idc =  active_sps->chroma_format_idc;
  VO_U32 crop_left, crop_right, crop_top, crop_bottom;
  int iLumaSizeX, iLumaSizeY;
  int iChromaSizeX, iChromaSizeY;

  if (pDecGlobal->frame_cropping_flag)
  {
	crop_left   = 2 * pDecGlobal->frame_cropping_rect_left_offset;
	crop_right  = 2 * pDecGlobal->frame_cropping_rect_right_offset;
	crop_top	= 2 *( 2 - frame_mbs_only_flag ) * pDecGlobal->frame_cropping_rect_top_offset;
	crop_bottom = 2 *( 2 - frame_mbs_only_flag ) * pDecGlobal->frame_cropping_rect_bottom_offset;
  }
  else
  {
    crop_left = crop_right = crop_top = crop_bottom = 0;
  }
  iChromaSizeX = pDecGlobal->width_cr- pDecGlobal->frame_cropping_rect_left_offset -pDecGlobal->frame_cropping_rect_right_offset;
  iChromaSizeY = pDecGlobal->height_cr - ( 2 - frame_mbs_only_flag ) * pDecGlobal->frame_cropping_rect_top_offset -( 2 - frame_mbs_only_flag ) * pDecGlobal->frame_cropping_rect_bottom_offset;
  iLumaSizeX = pDecGlobal->width-crop_left-crop_right;
  iLumaSizeY = pDecGlobal->height-crop_top-crop_bottom;
  if(iChromaSizeX<0||iChromaSizeY<0||iLumaSizeX<0||iLumaSizeY <0)
  {
    p->errFlag = VO_H264_ERR_InvalidSPS;
	return;
  }
  //if(p->structure==BOTTOM_FIELD)
  //{
    //p->imgY-=pDecGlobal->iLumaStride;
	//p->imgUV[0]-=pDecGlobal->iChromaStride;
	//p->imgUV[1]-=pDecGlobal->iChromaStride;
  //}
  p->pY = p->imgY+crop_top*pDecGlobal->iLumaStride+crop_left;
  crop_left >>= 1;
  crop_top >>= 1;
  p->pU = p->imgUV[0]+crop_top*pDecGlobal->iChromaStride+crop_left;
  p->pV = p->imgUV[1]+crop_top*pDecGlobal->iChromaStride+crop_left;
  p->iYUVFormat = chroma_format_idc;
  p->iWidth = iLumaSizeX;
  p->iHeight = iLumaSizeY;
  p->iYBufStride = pDecGlobal->iLumaStride;
  p->iUVBufStride = pDecGlobal->iChromaStride;
  //if(p->structure==BOTTOM_FIELD)
  //{
	//  p->imgY+=pDecGlobal->iLumaStride;
	//  p->imgUV[0]+=pDecGlobal->iChromaStride;
	//  p->imgUV[1]+=pDecGlobal->iChromaStride;
  //}
  p->bFinishOutput = 0;
  pDecGlobal->p_output[pDecGlobal->output_size++] = p;
  return;
}

VO_S32 GetOutputPic(H264DEC_G *pDecGlobal, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutInfo)
{
	StorablePicture *pPic;
	VO_U32 i;
	VO_U32 bFinish;
	//printf("start GetOutputPic\r\n");
	if(pDecGlobal->output_size > 0)
	{
		pPic = pDecGlobal->p_output[0];

#if USE_FRAME_THREAD
		if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
		{
			pthread_mutex_lock(&pDecGlobal->pFrameThdPool->outputMutex);
			//check errFlag
			while (pPic->errFlag)
			{
// printf("clean error pic %d, %x\n", pPic->cur_buf_index, pPic->errFlag);
// 				pPic->bFinishOutput = 1;
				for(i = 0;i<pDecGlobal->output_size-1;i++)
					pDecGlobal->p_output[i]=pDecGlobal->p_output[i+1];
				pDecGlobal->output_size--;
				if (pDecGlobal->output_size > 0)
				{
					pPic = pDecGlobal->p_output[0];
				}
				else
				{
					pthread_mutex_unlock(&pDecGlobal->pFrameThdPool->outputMutex);
					goto noOutput;
				}
			}
			if(pPic->iCodingType == FIELD_CODING&&pPic->structure==FRAME)
			{
			  bFinish = pPic->top_field->removable && pPic->bottom_field->removable;
			  pPic->removable=1;
			}
			else if(pPic->structure==BOTTOM_FIELD)
			{
			  bFinish = pPic->removable && pPic->top_field->removable;
			}
			else if(pPic->structure==TOP_FIELD)
			{
			  bFinish = pPic->removable && pPic->bottom_field->removable;
			}
			else
			  bFinish = pPic->removable;
			//printf("bFinish done\r\n");
			pthread_mutex_unlock(&pDecGlobal->pFrameThdPool->outputMutex);
		}
		else
#endif
		{
			while (pPic->errFlag)
			{
//LOGI("clean error pic %d, %x\n", pPic->cur_buf_index, pPic->errFlag);
// 				pPic->bFinishOutput = 1;
				for(i = 0;i<pDecGlobal->output_size-1;i++)
					pDecGlobal->p_output[i]=pDecGlobal->p_output[i+1];
				pDecGlobal->output_size--;
				if (pDecGlobal->output_size > 0)
				{
					pPic = pDecGlobal->p_output[0];
				}
				else
				{
					goto noOutput;
				}
			}
			bFinish = pPic->removable;
		}
         
		if (bFinish)
		{
// printf("output %d \n", pPic->cur_buf_index);
			pOutput->ColorType=VO_COLOR_YUV_PLANAR420;
			pOutput->Buffer[0]=pPic->pY;
			pOutput->Buffer[1]=pPic->pU;
			pOutput->Buffer[2]=pPic->pV;
			pOutput->Stride[0]=pPic->iYBufStride;
			pOutput->Stride[1]=pPic->iUVBufStride;
			pOutput->Stride[2]=pPic->iUVBufStride;
			pOutInfo->Format.Width  =  pPic->iWidth;
			pOutInfo->Format.Height =  pPic->iHeight;
			if(pPic->type==I_SLICE)
				pOutInfo->Format.Type = VO_VIDEO_FRAME_I;
			else if(pPic->type==P_SLICE)
				pOutInfo->Format.Type = VO_VIDEO_FRAME_P;
			else if(pPic->type==B_SLICE)
				pOutInfo->Format.Type = VO_VIDEO_FRAME_B;
			pOutput->Time		= pPic->Time;
			pOutput->UserData	= pPic->UserData;
			//printf("checkVideo: pPic->pY:%x,width:%d,height:%d\r\n",pPic->pY,pPic->iWidth,pPic->iHeight);
			if(!pPic->used_for_reference)
			  pDecGlobal->licenseCheck->checkVideo(pDecGlobal->licenseCheck->hCheck,pOutput,&pOutInfo->Format);
			//printf("finish checkVideo\r\n");
			//pDecGlobal->licenseCheck->lastVideo=(VO_U8 *)pOutput;
			pPic->bFinishOutput = 1;
			for(i = 0;i<pDecGlobal->output_size-1;i++)
				pDecGlobal->p_output[i]=pDecGlobal->p_output[i+1];
			pDecGlobal->output_size--;

			if(pDecGlobal->output_size > 0)
			{
				pOutInfo->Flag = (VO_U32)pDecGlobal->output_size;
// #if USE_FRAME_THREAD
// 				pthread_mutex_lock(&pDecGlobal->pFrameThdPool->outputMutex);
// #endif
// 				pOutInfo->Flag = pDecGlobal->p_output[0]->removable;
// // 				if (pOutInfo->Flag)
// // 				{
// // 					printf("has next, size %d poc %d\n", pDecGlobal->output_size, pDecGlobal->p_output[0]->poc);
// // 				}
// // 				else
// // 				{
// // 					printf("no next\n");
// // 				}
// #if USE_FRAME_THREAD
// 				pthread_mutex_unlock(&pDecGlobal->pFrameThdPool->outputMutex);
// #endif
			}
			else
			{
// 				printf("no next\n");
				pOutInfo->Flag = 0;
			}
			return 0;
		}
	}
#if USE_FRAME_THREAD
noOutput:
#endif
	pOutput->Buffer[0]=
	pOutput->Buffer[1]=
	pOutput->Buffer[2]=0;

	pOutInfo->Format.Type =VO_VIDEO_FRAME_NULL;
	pOutInfo->Flag = (VO_U32)pDecGlobal->output_size;

	return 0;
}


void write_stored_frame( H264DEC_G *pDecGlobal, FrameStore *fs)
{
  if (/*((!pDecGlobal->non_conforming_stream) || fs->recovery_frame)&&*/fs->frame)
  {
	  if(pDecGlobal->bDirectOutputMode == 0)
		output_one_picture (pDecGlobal, fs->frame);
  }


  fs->is_output = 1;
}


void direct_output(H264DEC_G *pDecGlobal, StorablePicture *p)
{
  if (p->structure==FRAME)
  {
    // we have a frame (or complementary field pair)
    // so output it directly
    //flush_direct_output(pDecGlobal, p_out);
// printf("direct_output poc %d\n", p->poc);
    output_one_picture (pDecGlobal, p);
	//remove by Really Yang 20110425
    //if (p_Vid->p_ref != NULL && !p_Inp->silent)
    //  find_snr(p_Vid, p, p_Vid->p_ref);
    //end of remove
    free_storable_picture(pDecGlobal,p);
 	  //FrameBufCtl(&pDecGlobal->vFrameIndexFIFO, p->cur_buf_index, FIFO_WRITE);
    pDecGlobal->p_Dpb->p_remove[pDecGlobal->p_Dpb->remove_size++]=p;
	//p->removable = 1;
    return;
  }
  pDecGlobal->direct_set = 1;
  pDecGlobal->p_Dpb->direct_out = p;
  return;
#if 0
  if (p->structure == TOP_FIELD)
  {
    if (p_Vid->out_buffer->is_used &1)
      flush_direct_output(pDecGlobal,p_Vid, p_Vid->p_out);
    pDecGlobal->out_buffer->top_field = p;
    pDecGlobal->out_buffer->is_used |= 1;
  }

  if (p->structure == BOTTOM_FIELD)
  {
    if (p_Vid->out_buffer->is_used &2)
      flush_direct_output(pDecGlobal,p_Vid, p_Vid->p_out);
    pDecGlobal->out_buffer->bottom_field = p;
    pDecGlobal->out_buffer->is_used |= 2;
  }

  if (pDecGlobal->out_buffer->is_used == 3)
  {
    // we have both fields, so output them
    dpb_combine_field_yuv(pDecGlobal, pDecGlobal->out_buffer);
#if (MVC_EXTENSION_ENABLE)
    pDecGlobal->out_buffer->frame->view_id = pDecGlobal->out_buffer->view_id;
#endif
    write_picture (pDecGlobal, pDecGlobal->out_buffer->frame, pDecGlobal->p_out, FRAME);

    calculate_frame_no(pDecGlobal, p);
	//remove by Really Yang 20110425
    //if (p_Vid->p_ref != NULL && !p_Inp->silent)
    //  find_snr(p_Vid, p_Vid->out_buffer->frame, p_Vid->p_ref);
	//end of remove
    free_storable_picture(pDecGlobal,pDecGlobal->out_buffer->frame);
    pDecGlobal->out_buffer->frame = NULL;
    free_storable_picture(pDecGlobal,pDecGlobal->out_buffer->top_field);
    pDecGlobal->out_buffer->top_field = NULL;
    free_storable_picture(pDecGlobal,pDecGlobal->out_buffer->bottom_field);
    pDecGlobal->out_buffer->bottom_field = NULL;
    pDecGlobal->out_buffer->is_used = 0;
  }
#endif
}

