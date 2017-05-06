

#include <math.h>
#include <limits.h>

#include "global.h"
#include "image.h"
#include "nalu.h"
#include "parset.h"
#include "header.h"

#include "sei.h"
#include "output.h"
#include "mb_access.h"
#include "memalloc.h"
#include "macroblock.h"

#include "loopfilter.h"
#include "cabac.h"
#include "vlc.h"
#include "quant.h"
//#include "mbuffer_mvc.h"
#include "fast_memory.h"
#include "voH264Readbits.h"
#include "biaridecod.h"
#include "ifunctions.h"

extern int testEndian(void);
VO_S32 reorder_lists(H264DEC_G *pDecGlobal,Slice *currSlice);


VO_S32 init_picture(H264DEC_G *pDecGlobal, Slice *currSlice)
{
  StorablePicture *dec_picture = NULL;
  seq_parameter_set_rbsp_t *active_sps = currSlice->active_sps;
  VO_S32 ret;
  //p_Vid->bFrameInit = 1;
  if (pDecGlobal->dec_picture&&!pDecGlobal->direct_set) // && p_Vid->num_dec_mb == p_Vid->PicSizeInMbs)
  {
    // this may only happen on slice loss
		//ret = exit_picture(pDecGlobal, &pDecGlobal->dec_picture);
		
		//if(pDecGlobal->dec_picture&&ret)
		//{
		//  int i = pDecGlobal->dec_picture->cur_buf_index;
		//  free_storable_picture(pDecGlobal,pDecGlobal->dec_picture);
		//  FrameBufCtl(&pDecGlobal->vFrameIndexFIFO, i, FIFO_WRITE);
		  
		//}
#if USE_FRAME_THREAD
		if (pDecGlobal->nThdNum > 1/* && !pDecGlobal->interlace*/)
		{
			pthread_mutex_lock(pDecGlobal->pThdInfo->progressMutex);
			pDecGlobal->dec_picture->errFlag = VO_H264_ERR_PIC_STRUCT;
//printf("%d %d errFlag! %x\n", pDecGlobal->pThdInfo->thread_id, pDecGlobal->dec_picture->cur_buf_index, pDecGlobal->dec_picture->errFlag);
			pthread_cond_broadcast(pDecGlobal->pThdInfo->progressCond);
#if USE_WIN32_BROADCAST
#if defined(VOWINCE) || defined(WIN32)
			{
				VO_U32 i;
				for (i = pDecGlobal->nThdNum-1; i > 0; --i)
				{
					pthread_cond_signal(pDecGlobal->pThdInfo->progressCond);
				}
			}
#endif
#endif
			pthread_mutex_unlock(pDecGlobal->pThdInfo->progressMutex);
		}
		else
#endif
		{
			VO_U32 i;
			pDecGlobal->dec_picture->errFlag = VO_H264_ERR_PIC_STRUCT;

				
				//delete from fs
				for (i = 0; i < pDecGlobal->p_Dpb->used_size; ++i)
				{
					if (pDecGlobal->p_Dpb->fs[i]->frame&&pDecGlobal->p_Dpb->fs[i]->frame->cur_buf_index == pDecGlobal->dec_picture->cur_buf_index)
					{
						break;
					}
					if (pDecGlobal->p_Dpb->fs[i]->top_field&&pDecGlobal->p_Dpb->fs[i]->top_field->cur_buf_index == pDecGlobal->dec_picture->cur_buf_index)
					{
						break;
					}
					if (pDecGlobal->p_Dpb->fs[i]->bottom_field&&pDecGlobal->p_Dpb->fs[i]->bottom_field->cur_buf_index == pDecGlobal->dec_picture->cur_buf_index)
					{
						break;
					}
				}
				if (i < pDecGlobal->p_Dpb->used_size)
				{
					FrameStore *tmp = pDecGlobal->p_Dpb->fs[i];

					for (; i<pDecGlobal->p_Dpb->used_size-1;++i)
					{
						pDecGlobal->p_Dpb->fs[i] = pDecGlobal->p_Dpb->fs[i+1];
					}
					pDecGlobal->p_Dpb->fs[pDecGlobal->p_Dpb->used_size-1] = tmp;
					pDecGlobal->p_Dpb->used_size--;
				}
				update_ref_list(pDecGlobal->p_Dpb);
				update_ltref_list(pDecGlobal->p_Dpb);
				//add to remove list
				for (i = 0; i < pDecGlobal->p_Dpb->remove_size; ++i)
				{
					if (pDecGlobal->p_Dpb->p_remove[i]->cur_buf_index == pDecGlobal->dec_picture->cur_buf_index)
					{
						pDecGlobal->p_Dpb->p_remove[i]->bFinishOutput = 1;
						pDecGlobal->p_Dpb->p_remove[i]->removable =1;
						break;
					}
				}
				if (i == pDecGlobal->p_Dpb->remove_size)
				{
					pDecGlobal->p_Dpb->p_remove[i] = pDecGlobal->dec_picture;
					pDecGlobal->p_Dpb->p_remove[i]->bFinishOutput = 1;
					pDecGlobal->p_Dpb->p_remove[i]->removable =1;
					pDecGlobal->p_Dpb->remove_size++;
				}

				//delete from output list
				for (i = 0; i < pDecGlobal->output_size; ++i)
				{
					if (pDecGlobal->p_output[i]->cur_buf_index == pDecGlobal->dec_picture->cur_buf_index)
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

		}
		pDecGlobal->dec_picture = NULL;
  }
  if (pDecGlobal->recovery_point)
    pDecGlobal->recovery_frame_num = (currSlice->frame_num + pDecGlobal->recovery_frame_cnt) % pDecGlobal->MaxFrameNum;

  if (currSlice->idr_flag)
    pDecGlobal->recovery_frame_num = currSlice->frame_num;

  if(currSlice->nal_reference_idc)
  {
    pDecGlobal->pre_frame_num = currSlice->frame_num;
  }

  //calculate POC
  currSlice->toppoc = currSlice->bottompoc = currSlice->framepoc = 0;
  
  ret = decode_poc(pDecGlobal, currSlice);
  if(ret)
  	return ret;
  //printf("poc:%d\n",currSlice->framepoc);
  //if (currSlice->ThisPOC==6)
  //{
	//  ret = ret;
  //}
  if (pDecGlobal->recovery_frame_num == (int) currSlice->frame_num && pDecGlobal->recovery_poc == 0x7fffffff)
    pDecGlobal->recovery_poc = currSlice->framepoc;
  if(pDecGlobal->p_Dpb->last_picture&&((currSlice->structure%3)&pDecGlobal->p_Dpb->last_picture->is_used)==1)
  {
    if (pDecGlobal->nThdNum > 1&&!pDecGlobal->interlace)
      VOH264ERROR(VO_H264_ERR_NO_STRUCTURE);
    pDecGlobal->p_Dpb->last_picture->is_output=1;
	  pDecGlobal->p_Dpb->last_picture=NULL;
  }
  if(currSlice->structure!=FRAME&&(currSlice->active_sps->frame_mbs_only_flag||!pDecGlobal->interlace))
  {
    VOH264ERROR(VO_H264_ERR_NO_STRUCTURE);
  }
  dec_picture = pDecGlobal->dec_picture = alloc_storable_picture (pDecGlobal, currSlice->structure);
  if(dec_picture==NULL)
  {
    //LOGI("dec_picture NULL\r\n");
  	VOH264ERROR(VO_H264_ERR_NO_STRUCTURE);
  }
  //printf("dec_picture:%x alloc %d\n",dec_picture, dec_picture->cur_buf_index);
  dec_picture->top_poc=currSlice->toppoc;
  dec_picture->bottom_poc=currSlice->bottompoc;
  dec_picture->frame_poc=currSlice->framepoc;
  
  dec_picture->iCodingType = currSlice->structure==FRAME? (currSlice->mb_aff_frame_flag? FRAME_MB_PAIR_CODING:FRAME_CODING): FIELD_CODING; //currSlice->slice_type;
#if (MVC_EXTENSION_ENABLE)
  dec_picture->view_id         = currSlice->view_id;
  dec_picture->inter_view_flag = currSlice->inter_view_flag;
  dec_picture->anchor_pic_flag = currSlice->anchor_pic_flag;
#endif
  //memset(pDecGlobal->pic_cache,0,32*sizeof(StorablePicture*));
  //memset(pDecGlobal->mv_cache,0,80*sizeof(int));


  switch (currSlice->structure )
  {
  case TOP_FIELD:
    {
      dec_picture->poc = currSlice->toppoc;
      pDecGlobal->number *= 2;
      break;
    }
  case BOTTOM_FIELD:
    {
      dec_picture->poc = currSlice->bottompoc;
      pDecGlobal->number = pDecGlobal->number * 2 + 1;
      break;
    }
  case FRAME:
    {
      dec_picture->poc = currSlice->framepoc;
      break;
    }
  default:
    VOH264ERROR(VO_H264_ERR_NO_STRUCTURE);
  }

  //p_Vid->current_slice_nr=0;

  if (pDecGlobal->type > SI_SLICE)
  {
    pDecGlobal->type = P_SLICE;  // concealed element
  }
 

  //dec_picture->slice_type = pDecGlobal->type;
  dec_picture->used_for_reference = (currSlice->nal_reference_idc != 0);
  pDecGlobal->idr_flag = currSlice->idr_flag;
  pDecGlobal->no_output_of_prior_pics_flag = currSlice->no_output_of_prior_pics_flag;
  pDecGlobal->long_term_reference_flag     = currSlice->long_term_reference_flag;
  dec_picture->adaptive_ref_pic_buffering_flag = currSlice->adaptive_ref_pic_buffering_flag;

  dec_picture->dec_ref_pic_marking_buffer = pDecGlobal->dec_ref_pic_marking_buffer;
  //currSlice->dec_ref_pic_marking_buffer   = NULL;

  dec_picture->pic_num   = currSlice->frame_num;
  dec_picture->frame_num = currSlice->frame_num;

  pDecGlobal->recovery_frame = (unsigned int) ((int) currSlice->frame_num == pDecGlobal->recovery_frame_num);

  dec_picture->coded_frame = (currSlice->structure==FRAME);

  pDecGlobal->frame_cropping_flag = active_sps->frame_cropping_flag;

  if (pDecGlobal->frame_cropping_flag)
  {
    pDecGlobal->frame_cropping_rect_left_offset   = active_sps->frame_cropping_rect_left_offset;
    pDecGlobal->frame_cropping_rect_right_offset  = active_sps->frame_cropping_rect_right_offset;
    pDecGlobal->frame_cropping_rect_top_offset    = active_sps->frame_cropping_rect_top_offset;
    pDecGlobal->frame_cropping_rect_bottom_offset = active_sps->frame_cropping_rect_bottom_offset;
  }
  pDecGlobal->mb_type0_frame_buffer = dec_picture->mb_type_buffer;
  currSlice->slice_number = -1;
  return 0;
}
static void fill_wp_params(H264DEC_G *pDecGlobal,Slice *currSlice)
{ 
  VO_S32 slice_number = currSlice->slice_number;
  if (currSlice->slice_type == B_SLICE)
  {
    int i, j, k;
    int comp;
    int log_weight_denom;
    int tb, td;  
    int tx,DistScaleFactor;

    int max_l0_ref = currSlice->num_ref_idx_active[LIST_0];
    int max_l1_ref = currSlice->num_ref_idx_active[LIST_1];
	

    if (currSlice->active_pps->weighted_bipred_idc == 2)
    {
      currSlice->luma_log2_weight_denom = 5;
      currSlice->chroma_log2_weight_denom = 5;
      currSlice->wp_round_luma   = 16;
      currSlice->wp_round_chroma = 16;

      for (i=0; i<MAX_REFERENCE_PICTURES; ++i)
      {
        for (comp=0; comp<3; ++comp)
        {
          log_weight_denom = (comp == 0) ? currSlice->luma_log2_weight_denom : currSlice->chroma_log2_weight_denom;
          currSlice->wp_weight[0][i][comp] = 1 << log_weight_denom;
          currSlice->wp_weight[1][i][comp] = 1 << log_weight_denom;
          currSlice->wp_offset[0][i][comp] = 0;
          currSlice->wp_offset[1][i][comp] = 0;
        }
      }
    }

    for (i=0; i<max_l0_ref; ++i)
    {
      for (j=0; j<max_l1_ref; ++j)
      {
        for (comp = 0; comp<3; ++comp)
        {
          log_weight_denom = (comp == 0) ? currSlice->luma_log2_weight_denom : currSlice->chroma_log2_weight_denom;
          if (currSlice->active_pps->weighted_bipred_idc == 1)
          {
            currSlice->wbp_weight[0][i][j][comp] =  currSlice->wp_weight[0][i][comp];
            currSlice->wbp_weight[1][i][j][comp] =  currSlice->wp_weight[1][j][comp];
          }
          else if (currSlice->active_pps->weighted_bipred_idc == 2)
          {
            td = iClip3(-128,127,currSlice->listX[slice_number][LIST_1][j]->poc - currSlice->listX[slice_number][LIST_0][i]->poc);
            if (td == 0 || currSlice->listX[slice_number][LIST_1][j]->is_long_term || currSlice->listX[slice_number][LIST_0][i]->is_long_term)
            {
              currSlice->wbp_weight[0][i][j][comp] = 32;
              currSlice->wbp_weight[1][i][j][comp] = 32;
            }
            else
            {
              tb = iClip3(-128,127,currSlice->ThisPOC - currSlice->listX[slice_number][LIST_0][i]->poc);

              tx = (16384 + iabs(td/2))/td;
              DistScaleFactor = iClip3(-1024, 1023, (tx*tb + 32 )>>6);

              currSlice->wbp_weight[1][i][j][comp] = DistScaleFactor >> 2;
              currSlice->wbp_weight[0][i][j][comp] = 64 - currSlice->wbp_weight[1][i][j][comp];
              if (currSlice->wbp_weight[1][i][j][comp] < -64 || currSlice->wbp_weight[1][i][j][comp] > 128)
              {
                currSlice->wbp_weight[0][i][j][comp] = 32;
                currSlice->wbp_weight[1][i][j][comp] = 32;
                currSlice->wp_offset[0][i][comp] = 0;
                currSlice->wp_offset[1][j][comp] = 0;
              }
            }
          }
        }
      }
    }
#if 1
    if (currSlice->mb_aff_frame_flag)
    {
      for (i=0; i<2*max_l0_ref; ++i)
      {
        for (j=0; j<2*max_l1_ref; ++j)
        {
          for (comp = 0; comp<3; ++comp)
          {
            for (k=2; k<6; k+=2)
            {
              currSlice->wp_offset[k+0][i][comp] = currSlice->wp_offset[0][i>>1][comp];
              currSlice->wp_offset[k+1][j][comp] = currSlice->wp_offset[1][j>>1][comp];

              log_weight_denom = (comp == 0) ? currSlice->luma_log2_weight_denom : currSlice->chroma_log2_weight_denom;
              if (currSlice->active_pps->weighted_bipred_idc == 1)
              {
                currSlice->wbp_weight[k+0][i][j][comp] =  currSlice->wp_weight[0][i>>1][comp];
                currSlice->wbp_weight[k+1][i][j][comp] =  currSlice->wp_weight[1][j>>1][comp];
              }
              else if (currSlice->active_pps->weighted_bipred_idc == 2)
              {
			    if (!currSlice->listX[slice_number][k+LIST_1][j]||!currSlice->listX[slice_number][k+LIST_0][i])
			      td = 0;
				else
                  td = iClip3(-128, 127, currSlice->listX[slice_number][k+LIST_1][j]->poc - currSlice->listX[slice_number][k+LIST_0][i]->poc);
                if (td == 0 || currSlice->listX[slice_number][k+LIST_1][j]->is_long_term || currSlice->listX[slice_number][k+LIST_0][i]->is_long_term)
                {
                  currSlice->wbp_weight[k+0][i][j][comp] =   32;
                  currSlice->wbp_weight[k+1][i][j][comp] =   32;
                }
                else
                {
                  tb = iClip3(-128,127,((k==2)?currSlice->toppoc:currSlice->bottompoc) - currSlice->listX[slice_number][k+LIST_0][i]->poc);

                  tx = (16384 + iabs(td/2))/td;
                  DistScaleFactor = iClip3(-1024, 1023, (tx*tb + 32 )>>6);

                  currSlice->wbp_weight[k+1][i][j][comp] = DistScaleFactor >> 2;
                  currSlice->wbp_weight[k+0][i][j][comp] = 64 - currSlice->wbp_weight[k+1][i][j][comp];
                  if (currSlice->wbp_weight[k+1][i][j][comp] < -64 || currSlice->wbp_weight[k+1][i][j][comp] > 128)
                  {
                    currSlice->wbp_weight[k+1][i][j][comp] = 32;
                    currSlice->wbp_weight[k+0][i][j][comp] = 32;
                    currSlice->wp_offset[k+0][i][comp] = 0;
                    currSlice->wp_offset[k+1][j][comp] = 0;
                  }
                }
              }
            }
          }
        }
      }
    }
#endif
  }
}



int init_picture_decoding(H264DEC_G *pDecGlobal,Slice *currSlice)
{
  int PicParsetId = currSlice->pic_parameter_set_id;  

  
  if(currSlice->idr_flag)
    pDecGlobal->number=0;

  pDecGlobal->PicHeightInMbs = pDecGlobal->FrameHeightInMbs / ( 1 + currSlice->field_pic_flag );//mask by Really Yang for field 20110425
  pDecGlobal->PicSizeInMbs   = pDecGlobal->PicWidthInMbs * pDecGlobal->PicHeightInMbs;
  pDecGlobal->FrameSizeInMbs = pDecGlobal->PicWidthInMbs * pDecGlobal->FrameHeightInMbs;
  pDecGlobal->structure = currSlice->structure;
  currSlice->current_mb_nr = 0;

  update_pic_num(pDecGlobal,currSlice);//mask by Really Yang for field 20110425

  return 0;
}

VO_S32 init_slice(H264DEC_G *pDecGlobal, Slice *currSlice)
{
  int ret = 0;
  VO_S32 slice_number = currSlice->slice_number;
  StorablePicture *dec_picture = pDecGlobal->dec_picture;
  VO_S32 i,j;
  if(slice_number>=32)
  	VOH264ERROR(VO_H264_ERR_SLICE_HEADER);
  pDecGlobal->init_lists (pDecGlobal,currSlice);
  
  ret = reorder_lists (pDecGlobal,currSlice);
  if(ret)
  	return ret;
  
#if USE_FRAME_THREAD
  if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
  {
	  
	  for (i = 0; i < 2; ++i)
	  {
		  for (j = 0; j < currSlice->listXsize[slice_number][i]; ++j)
		  {
			  if (currSlice->listX[slice_number][i][j])
			  {
				  currSlice->listX[slice_number][i][j] = currSlice->listX[slice_number][i][j]->frame_real;
			  }
			  if (!currSlice->listX[slice_number][i][j])
			  {
				  return VO_H264_ERR_REORDER_LIST;
			  }
			  if (currSlice->listX[slice_number][i][j] == dec_picture)
			  {
            	  return VO_H264_ERR_REORDER_LIST;
			  }
			  if (currSlice->listX[slice_number][i][j]->errFlag)
			  {
				  return VO_H264_ERR_REORDER_LIST;
			  }
#if CHECK_REF_WRONG
			  {
				  VO_S32 df;
				  df = currSlice->listX[slice_number][i][j]->frame_num - currSlice->frame_num;
				  if ((df >= 0 && df < pDecGlobal->MaxFrameNum/2) || (df < -pDecGlobal->MaxFrameNum/2))
				  {
					  return VO_H264_ERR_DUPLICATE_FRAMENUM;
				  }
			  }
#endif
		  }
	  }
    
  }
  else
#endif
  {
	  for (i = 0; i < 2; ++i)
	  {
		  for (j = 0; j < currSlice->listXsize[slice_number][i]; ++j)
		  {
			  if (currSlice->listX[slice_number][i][j] == dec_picture)
			  {
				  VO_U32 size = pDecGlobal->p_Dpb->size + 1 + pDecGlobal->extra_dpb;
        			VO_U32 i;
            	    for (i = 0; i < pDecGlobal->p_Dpb->ref_frames_in_buffer; i++)
                      pDecGlobal->p_Dpb->fs_ref[i]=NULL;
                    for (i=0; i<pDecGlobal->p_Dpb->ltref_frames_in_buffer; i++)
                      pDecGlobal->p_Dpb->fs_ltref[i]=NULL;
        
        		    pDecGlobal->vFrameIndexFIFO.r_idx = pDecGlobal->vFrameIndexFIFO.w_idx = 0;
        
        		    for(i = 0; i < size; i++)
        		    {
        			  FrameBufCtl(&pDecGlobal->vFrameIndexFIFO, i, FIFO_WRITE);
        		    }
                    pDecGlobal->p_Dpb->used_size=0;
//printf("init_slice remove all\n");
        		    pDecGlobal->p_Dpb->remove_size=0;
            	  update_ref_list(pDecGlobal->p_Dpb);
                  update_ltref_list(pDecGlobal->p_Dpb);
                  pDecGlobal->p_Dpb->last_output_poc = INT_MIN;
            	  return VO_H264_ERR_REORDER_LIST;
			  }
		  }
	  }
  }

  ret = init_mbaff_lists(pDecGlobal, currSlice);
  if(ret)
	return ret;
  if(!pDecGlobal->interlace&&currSlice->active_sps->profile_idc!=BASELINE)
  {
    for(i = 0;i <2;i++)
    {
	  if(currSlice->listXsize[slice_number][i])
	  {
	    dec_picture->listXsize[slice_number][i] =  currSlice->listXsize[slice_number][i];
	    for(j=0; j<dec_picture->listXsize[slice_number][i]; j++)
		  dec_picture->listX[slice_number][i][j] = currSlice->listX[slice_number][i][j];
	  }
    }
  }
  pDecGlobal->iDeblockMode = (pDecGlobal->DFDisableIdc != 1&&!pDecGlobal->disableDeblock);
  return 0;
}




VO_S32 decode_slice(H264DEC_G *pDecGlobal,Slice *currSlice)
{
  //VideoParameters *p_Vid = currSlice->p_Vid;
  //int iScale = (1+currSlice->mb_aff_frame_flag);//YU deblock
  
  VO_S32 ret;
  H264DEC_L pDecLocal = {0};
  pDecLocal.cof_yuv  = pDecGlobal->cof_yuv_row;
  if (pDecGlobal->dec_picture==NULL||pDecGlobal->dec_picture->structure!=currSlice->structure)
  {
    VOH264ERROR(VO_H264_ERR_PIC_STRUCT);
  }
  //YU: CABAC different slices in a pic can use different pps??
  currSlice->slice_number++;
    ret = init_slice(pDecGlobal, currSlice); 
    if(ret)
  	  return ret;
  
#if(CAL_SLICE)
	  {
		TIME_T start,end;
			  gettime (&start); 	   // start time
#endif 

	if (currSlice->active_pps->entropy_coding_mode_flag)
	{    
		currSlice->last_dquant=0;
		vo_h264_init_cabac_states(currSlice);
	}
  //printf("slice type:%d\n",currSlice->slice_type);
  //YU: weighted_bipred_idc and weighted_pred_flag difference?
  if ( (currSlice->active_pps->weighted_bipred_idc > 0  && (currSlice->slice_type == B_SLICE)) || (currSlice->active_pps->weighted_pred_flag && currSlice->slice_type !=I_SLICE))
    fill_wp_params(pDecGlobal,currSlice);//mask by Really Yang for field 20110425
#if(CAL_SLICE)
	gettime (&end); 		  // end time
	pDecGlobal->tot_slice += timediff(&start, &end);
	}
#endif
  //printf("frame picture %d %d %d\n",currSlice->structure,currSlice->ThisPOC,currSlice->direct_spatial_mv_pred_flag);
  
  // decode main slice information
  //if (currSlice->ei_flag == 0)
    
    ret = decode_one_slice(pDecGlobal,&pDecLocal,currSlice);
    if(ret)
	  return ret;
	return 0;
}


VO_S32 backup_slice_ref(H264DEC_G *pDecGlobal,Slice *currSlice)
{
  VO_U32 i;
  VO_U32 sp_used = 0;
  currSlice->ref_frames_in_buffer = pDecGlobal->p_Dpb->ref_frames_in_buffer;
  currSlice->ltref_frames_in_buffer = pDecGlobal->p_Dpb->ltref_frames_in_buffer;
  {
	  FrameThdInfo *pInfo = pDecGlobal->pThdInfo;
// 	  pthread_mutex_lock(pInfo->progressMutex);

	  for(i = 0;i < currSlice->ref_frames_in_buffer;i++)
	  {
		  currSlice->fs_ref[i] = *pDecGlobal->p_Dpb->fs_ref[i];
		  if (pDecGlobal->nThdNum > 1&&!pDecGlobal->interlace)
		  {
			  currSlice->cur_storable_pic[sp_used] = *(currSlice->fs_ref[i].frame);
			  currSlice->cur_storable_pic[sp_used].frame_real = currSlice->fs_ref[i].frame;
			  currSlice->fs_ref[i].frame = currSlice->cur_storable_pic + sp_used;
			  sp_used++;
		  }
		  else if(pDecGlobal->nThdNum > 1)
		  {
		    if(currSlice->fs_ref[i].frame)
		    {
		      struct storable_picture *top_field = currSlice->fs_ref[i].frame->top_field;
			  struct storable_picture *bottom_field = currSlice->fs_ref[i].frame->bottom_field;
		      currSlice->cur_storable_pic[sp_used] = *(currSlice->fs_ref[i].frame);
			  currSlice->cur_storable_pic[sp_used].frame_real = currSlice->fs_ref[i].frame;
			  currSlice->fs_ref[i].frame = currSlice->cur_storable_pic + sp_used;
			  sp_used++;
			  if (top_field)
			  {
				currSlice->cur_storable_pic[sp_used] = *(top_field);
				currSlice->cur_storable_pic[sp_used].frame_real = top_field;
				currSlice->fs_ref[i].top_field = currSlice->cur_storable_pic + sp_used;
				sp_used++;
			  }
			  if (bottom_field)
			  {
				currSlice->cur_storable_pic[sp_used] = *(bottom_field);
				currSlice->cur_storable_pic[sp_used].frame_real = bottom_field;
				currSlice->fs_ref[i].bottom_field = currSlice->cur_storable_pic + sp_used;
				sp_used++;
			  }
			  
		    }
			else if(currSlice->fs_ref[i].top_field)
			{
			  currSlice->cur_storable_pic[sp_used] = *(currSlice->fs_ref[i].top_field);
			  currSlice->cur_storable_pic[sp_used].frame_real = currSlice->fs_ref[i].top_field;
			  currSlice->fs_ref[i].top_field= currSlice->cur_storable_pic + sp_used;
			  //currSlice->fs_ref[i].frame->top_field= currSlice->cur_storable_pic + sp_used;
			  sp_used++;
			}
			else
			{
			  currSlice->cur_storable_pic[sp_used] = *(currSlice->fs_ref[i].bottom_field);
			  currSlice->cur_storable_pic[sp_used].frame_real = currSlice->fs_ref[i].bottom_field;
			  currSlice->fs_ref[i].bottom_field= currSlice->cur_storable_pic + sp_used;
			  //currSlice->fs_ref[i].frame->bottom_field= currSlice->cur_storable_pic + sp_used;
			  sp_used++;
			}
		  }
	  }
	  for(i = 0;i < currSlice->ltref_frames_in_buffer;i++)
	  {
		  currSlice->fs_ltref[i] = *pDecGlobal->p_Dpb->fs_ltref[i];
		  if (pDecGlobal->nThdNum > 1&&!pDecGlobal->interlace)
		  {
			  currSlice->cur_storable_pic[sp_used] = *(currSlice->fs_ltref[i].frame);
			  currSlice->cur_storable_pic[sp_used].frame_real = currSlice->fs_ltref[i].frame;
			  currSlice->fs_ltref[i].frame = currSlice->cur_storable_pic + sp_used;
			  sp_used++;
		  }
		  else if(pDecGlobal->nThdNum > 1)
		  {
		    if(currSlice->fs_ltref[i].frame)
		    {
		      currSlice->cur_storable_pic[sp_used] = *(currSlice->fs_ltref[i].frame);
			  currSlice->cur_storable_pic[sp_used].frame_real = currSlice->fs_ltref[i].frame;
			  currSlice->fs_ltref[i].frame = currSlice->cur_storable_pic + sp_used;
			  sp_used++;
		    }
			else if(currSlice->fs_ltref[i].top_field)
			{
			  currSlice->cur_storable_pic[sp_used] = *(currSlice->fs_ltref[i].top_field);
			  currSlice->cur_storable_pic[sp_used].frame_real = currSlice->fs_ltref[i].top_field;
			  currSlice->fs_ltref[i].top_field= currSlice->cur_storable_pic + sp_used;
			  sp_used++;
			}
			else
			{
			  currSlice->cur_storable_pic[sp_used] = *(currSlice->fs_ltref[i].bottom_field);
			  currSlice->cur_storable_pic[sp_used].frame_real = currSlice->fs_ltref[i].bottom_field;
			  currSlice->fs_ltref[i].bottom_field= currSlice->cur_storable_pic + sp_used;
			  sp_used++;
			}
		  }
	  }
// 	  pthread_mutex_unlock(pInfo->progressMutex);
  }
  return 0;
}


VO_S32 reorder_lists(H264DEC_G *pDecGlobal,Slice *currSlice)
{
  //VideoParameters *p_Vid = currSlice->p_Vid;
  int ret;
  VO_S32 i;
  VO_S32 slice_number = currSlice->slice_number;
  if ((currSlice->slice_type != I_SLICE)&&(currSlice->slice_type != SI_SLICE))
  {
    if (currSlice->ref_pic_list_reordering_flag[LIST_0])
    {
      //printf("reordering!!!!!!\r\n");
      ret = reorder_ref_pic_list(pDecGlobal,currSlice, LIST_0);
	  if(ret)
	  	return ret;
    }
	//printf("number:%d\r\n",currSlice->frame_num);
	
    if (NULL == currSlice->listX[slice_number][0][currSlice->num_ref_idx_active[LIST_0] - 1])
    {      
      //printf("num_ref_idx_active:%d \r\n",currSlice->num_ref_idx_active[LIST_0]);
	  //exit(0);
	  VOH264ERROR(VO_H264_ERR_REORDER_LIST);      
    }
    // that's a definition
    currSlice->listXsize[slice_number][0] = (char) currSlice->num_ref_idx_active[LIST_0];
  }
  if (currSlice->slice_type == B_SLICE)
  {
    if (currSlice->ref_pic_list_reordering_flag[LIST_1])
    {
      ret = reorder_ref_pic_list(pDecGlobal,currSlice, LIST_1);
	  if(ret)
	  	return ret;
    }
    if (NULL == currSlice->listX[slice_number][1][currSlice->num_ref_idx_active[LIST_1]-1])
    {     
	  VOH264ERROR(VO_H264_ERR_REORDER_LIST);      
    }
    // that's a definition
    currSlice->listXsize[slice_number][1] = (char) currSlice->num_ref_idx_active[LIST_1];
  }

  for (i=currSlice->listXsize[slice_number][0]; i< (MAX_LIST_SIZE) ; i++)
  {
	  currSlice->listX[slice_number][0][i] = NULL;
  }
  for (i=currSlice->listXsize[slice_number][1]; i< (MAX_LIST_SIZE) ; i++)
  {
	  currSlice->listX[slice_number][1][i] = NULL;
  }

  return 0;
}

extern int InterpretSPS (H264DEC_G *pDecGlobal, DataPartition *p, seq_parameter_set_rbsp_t *sps);
int process_first_sps(H264DEC_G *pDecGlobal)
{
  int ret=0,len; 
  NALU_t *nalu = pDecGlobal->nalu;
  VO_U8* buffer = voMalloc(pDecGlobal,nalu->len);
  DataPartition *dp = AllocPartition(pDecGlobal, 1);
  seq_parameter_set_rbsp_t *sps = AllocSPS(pDecGlobal);
  memcpy(buffer,nalu->buf,nalu->len);
  len = EBSPtoRBSP(buffer, nalu->len, 1);

  //memcpy (dp->bitstream->streamBuffer, &nalu->buf[1], nalu->len-1);
  dp->bitstream->streamBuffer = &buffer[1];
  dp->bitstream->code_len = dp->bitstream->bitstream_length = RBSPtoSODB (dp->bitstream->streamBuffer, nalu->len-1);
  InitBitStream(pDecGlobal,dp->bitstream->streamBuffer, dp->bitstream->code_len);
  //dp->bitstream->ei_flag = 0;
  dp->bitstream->read_len = dp->bitstream->frame_bitoffset = 0;
  ret = InterpretSPS (pDecGlobal, dp, sps);
  if(!ret)
    pDecGlobal->interlace = !sps->frame_mbs_only_flag;
  FreePartition (pDecGlobal,dp, 1);
  FreeSPS (pDecGlobal,sps);
  voFree(pDecGlobal,buffer);
  return ret;
}
int process_one_nalu(H264DEC_G *pDecGlobal,Slice *currSlice)
{

  NALU_t *nalu = pDecGlobal->nalu; 
  Bitstream *currStream = NULL;
  int ret; 
#if(CAL_HEADER)
	{
	  TIME_T start,end;
			gettime (&start);		 // start time
#endif 

  ret = nalu->len = EBSPtoRBSP(nalu->buf, nalu->len, 1);
 
  //if (ret < 0)
  //  VOH264ERROR(VO_H264_ERR_RBSP);
  /*if(pDecGlobal->last_output)
  {
	StorablePicture *pPic = pDecGlobal->last_output;
	if(!pPic->used_for_reference)
	{
	  VO_VIDEO_BUFFER pOutput;
	  VO_VIDEO_OUTPUTINFO pOutInfo;
	  pOutput.ColorType=VO_COLOR_YUV_PLANAR420;
	  pOutput.Buffer[0]=pPic->pY;
	  pOutput.Buffer[1]=pPic->pU;
	  pOutput.Buffer[2]=pPic->pV;
	  pOutput.Stride[0]=pPic->iYBufStride;
	  pOutput.Stride[1]=pPic->iUVBufStride;
	  pOutput.Stride[2]=pPic->iUVBufStride;
	  pOutInfo.Format.Width  =  pPic->iWidth;
	  pOutInfo.Format.Height =  pPic->iHeight;
	  pDecGlobal->licenseCheck->checkVideo(pDecGlobal->licenseCheck->hCheck,&pOutput,&pOutInfo.Format);
	}
	pDecGlobal->last_output = NULL;
  }*/
  if (nalu->forbidden_bit)
    VOH264ERROR(VO_H264_ERR_FORBIDDEN_BIT);
#if(CAL_HEADER)
	gettime (&end); 		  // end time
	pDecGlobal->tot_header += timediff(&start, &end);
	}
#endif 
  switch (nalu->nal_unit_type)
  {
  case NALU_TYPE_SLICE:
  case NALU_TYPE_IDR:
  {

	  //YU SEI recovery point radom access
    if (pDecGlobal->recovery_point || nalu->nal_unit_type == NALU_TYPE_IDR)
    {
#if USE_FRAME_THREAD
		if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
		{
			assert(pDecGlobal->recovery_point_found == 1);
		}
		else
#endif
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
    }

    
    currSlice->is_reset_coeff = FALSE;
    currSlice->idr_flag = (nalu->nal_unit_type == NALU_TYPE_IDR);
    currSlice->nal_reference_idc = nalu->nal_reference_idc;
    currStream = currSlice->partArr[0].bitstream;
    //currStream->ei_flag = 0;
    currStream->frame_bitoffset = currStream->read_len = 0;
    //memcpy (currStream->streamBuffer, &nalu->buf[1], nalu->len-1);
    currStream->streamBuffer = &nalu->buf[1];
    currStream->code_len = currStream->bitstream_length = RBSPtoSODB(currStream->streamBuffer, nalu->len-1);
	//if (currStream->code_len<=10)
	//{
	//  VOH264ERROR(VO_H264_ERR_NALU_SMALL);
	//}
    InitBitStream(pDecGlobal,currStream->streamBuffer, currStream->code_len);
	  
	
    ret = FirstPartOfSliceHeader(pDecGlobal,currSlice);
	if(ret)
	  return ret;
	if (pDecGlobal->recovery_point_found == 0)
	{
       if (currSlice->slice_type!=I_SLICE)		   
		   break;
       pDecGlobal->non_conforming_stream = 1;
	   //printf("Warning: Decoding does not start with an IDR picture.\n");
       pDecGlobal->recovery_point_found = 1;
	}
		
#if USE_FRAME_THREAD
	if (pDecGlobal->nThdNum <= 1/*||pDecGlobal->interlace*/)
#endif
	{
		pic_parameter_set_rbsp_t *pps;
		seq_parameter_set_rbsp_t *sps;
		pps = &pDecGlobal->PicParSet[currSlice->pic_parameter_set_id];
		sps = &pDecGlobal->SeqParSet[pps->seq_parameter_set_id];
		if (pps->Valid != TRUE)
			VOH264ERROR(VO_H264_ERR_PPSIsNULL);
		if (sps->Valid != TRUE)
			VOH264ERROR(VO_H264_ERR_SPSIsNULL);
		activate_sps(pDecGlobal, sps);
		activate_pps(pDecGlobal, pps);
	}
	if(currSlice->start_mb_nr>=(VO_S32)pDecGlobal->FrameSizeInMbs)
      VOH264ERROR(VO_H264_ERR_FIRST_MB_OVERFLOW);
    pDecGlobal->nal_startcode_follows = (currSlice->active_pps->entropy_coding_mode_flag == CAVLC)?
		                              uvlc_startcode_follows:cabac_startcode_follows;
    currSlice->Transform8x8Mode = currSlice->active_pps->transform_8x8_mode_flag;
	currSlice->is_not_independent = 0;
    ret = RestOfSliceHeader (pDecGlobal,currSlice);
	if(ret)
	  return ret;

#if USE_FRAME_THREAD
	if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
	{
		assert(currSlice->start_mb_nr!=0);
	}
	else
#endif
	{
		if(currSlice->start_mb_nr==0)
		{
		  pDecGlobal->num_dec_mb = 0;

		  ret = init_picture(pDecGlobal, currSlice);
		  if(ret)
			return ret;
		  pDecGlobal->dec_picture->Time = nalu->Time;
		  pDecGlobal->dec_picture->UserData = nalu->UserData;
		  if(pDecGlobal->p_Dpb->last_picture==NULL)
			pDecGlobal->dec_picture->type = currSlice->slice_type;
		  ret = init_picture_decoding(pDecGlobal, currSlice);
		  if(ret)
			return ret;  
		  //check zero_byte if it is also the first NAL unit in the access unit
		  CheckZeroByteVCL(pDecGlobal, nalu);

		  backup_slice_ref(pDecGlobal,currSlice);

		}
	}
	//assign_quant_params (pDecGlobal,currSlice);
	setup_slice_methods(pDecGlobal,currSlice);
	if(pDecGlobal->old_pps_id != currSlice->pic_parameter_set_id)
	{
// #if USE_FRAME_THREAD
// 		printf("pps id change in frame\n");
// 		return VO_ERR_BASE;
// #else
		assign_quant_params (pDecGlobal,currSlice);
		pDecGlobal->old_pps_id = currSlice->pic_parameter_set_id;
// #endif
	}
    if(currSlice->current_mb_nr < currSlice->start_mb_nr)
      currSlice->current_mb_nr = currSlice->start_mb_nr;
	currSlice->start_mb_nr_mbaff = currSlice->current_mb_nr;

    if (currSlice->active_pps->entropy_coding_mode_flag)
    {
      bytealign(pDecGlobal);
      //arideco_start_decoding (&currSlice->partArr[0].de_cabac, currStream->streamBuffer, bytepos(pDecGlobal)-currStream->streamBuffer, &currStream->read_len);
	  vo_start_cabac(&currSlice->partArr[0].c,bytepos(pDecGlobal),currStream->code_len-(bytepos(pDecGlobal)-currStream->streamBuffer));
	  if(!pDecGlobal->cabac_init_done)
	  {
#if USE_FRAME_THREAD
		  if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
		  {
			  //printf("cabac change in frame\n");
			  return VO_ERR_BASE;
		  }
		  else
#endif
		  {
			  vo_init_cabac_states();
			  pDecGlobal->cabac_init_done = 1;
		  }
	  }
    }
#if USE_FRAME_THREAD
	if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
	{
		assert(pDecGlobal->recovery_point == 0);
	}
	else
#endif
	{
		pDecGlobal->recovery_point = 0;
	}

	ret = decode_slice(pDecGlobal,currSlice);

	if(ret)
	{
#if USE_FRAME_THREAD
		if (pDecGlobal->nThdNum > 1 /*&& !pDecGlobal->interlace*/)
		{
// 			pDecGlobal->pThdInfo->errFlag = ret;
		}
		else
#endif
		{
			if (pDecGlobal->dec_picture)
			{
			    pDecGlobal->dec_picture->errFlag= ret;
                exit_picture(pDecGlobal, &pDecGlobal->dec_picture);
			}
		}
	  return ret;
	}

#if USE_FRAME_THREAD
	if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
	{
	}
	else
#endif
	{
		if(pDecGlobal->num_dec_mb >= pDecGlobal->PicSizeInMbs)
		{
			ret = exit_picture(pDecGlobal, &pDecGlobal->dec_picture);

			pDecGlobal->previous_frame_num = pDecGlobal->pCurSlice->frame_num;

			pDecGlobal->dec_picture->removable = 1;

			//if(!pDecGlobal->direct_set)
				pDecGlobal->dec_picture=NULL;
			return ret;
		}
	}
    return 0;
    break;
  }
  case NALU_TYPE_SEI:
    //printf ("read_new_slice: Found NALU_TYPE_SEI, len %d\n", nalu->len);
#if(CAL_HEADER)
		{
		  TIME_T start,end;
				gettime (&start);		 // start time
#endif
    if(pDecGlobal->enbaleSEI)
      return InterpretSEIMessage(pDecGlobal,nalu->buf,nalu->len, currSlice);
	else
	  return 0;
#if(CAL_HEADER)
		gettime (&end); 		  // end time
		pDecGlobal->tot_header += timediff(&start, &end);
		}
#endif 

    break;
  case NALU_TYPE_PPS:
    //printf ("Found NALU_TYPE_PPS\n");
#if(CAL_HEADER)
		{
		  TIME_T start,end;
				gettime (&start);		 // start time
 

#endif

    return ProcessPPS(pDecGlobal, nalu);
#if(CAL_HEADER)
						gettime (&end); 		  // end time
						pDecGlobal->tot_header += timediff(&start, &end);
						}
#endif

    break;
  case NALU_TYPE_SPS:
    //printf ("Found NALU_TYPE_SPS\n");
#if(CAL_HEADER)
		{
		  TIME_T start,end;
				gettime (&start);		 // start time
#endif
#if USE_FRAME_THREAD
		if (pDecGlobal->nThdNum > 1 /*&& !pDecGlobal->interlace*/)
		{
			if (pDecGlobal->p_Dpb->init_done)
			{
				//in frame thread mode, not support multi sps
				return VO_H264_ERR_InvalidSPS;
			}
		}
#endif
    return ProcessSPS(pDecGlobal, nalu);
#if(CAL_HEADER)
		gettime (&end); 		  // end time
		pDecGlobal->tot_header += timediff(&start, &end);
		}
#endif 

    break;
  case NALU_TYPE_AUD:
    break;
  case NALU_TYPE_EOSEQ:
    break;
  case NALU_TYPE_EOSTREAM:
    break;
  case NALU_TYPE_FILL:
    break;
  default:
    VOH264ERROR(VO_H264_ERR_NALU_TYPE); 
    break;
  }
  
  return 0;
}

VO_S32 exit_picture(H264DEC_G *pDecGlobal, StorablePicture **dec_picture)
{
  VO_S32 ret;
  // return if the last picture has already been finished
  if (*dec_picture==NULL )
  {
    return 0;
  }
#if 0
  if ((*dec_picture)->mb_aff_frame_flag)
    MbAffPostProc(p_Vid);
#endif



  ret = store_picture_in_dpb(pDecGlobal,pDecGlobal->p_Dpb, *dec_picture);

  if(ret)
  	return ret;
  if (pDecGlobal->last_has_mmco_5)
  {
    pDecGlobal->pre_frame_num = 0;
  }
  return 0;
}


// #if USE_MULTI_THREAD
static void update_intra_buffer(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,int mbxy)
{
	const VO_U8 *src_y;
	const VO_U8 *src_u;
	const VO_U8 *src_v;
	const VO_S32 iLumaStride = pDecGlobal->iLumaStride<<pDecGlobal->pCurSlice->field_pic_flag;
	const VO_S32 iChromaStride = pDecGlobal->iChromaStride<<pDecGlobal->pCurSlice->field_pic_flag;
	RESTRICT VO_U64 *intra_buffer;

	if (!pDecGlobal->pCurSlice->mb_aff_frame_flag)
	{
#if ENABLE_DEBLOCK_MB
#if USE_FRAME_THREAD
		//if (!pDecGlobal->interlace||pDecGlobal->nThdNum <= 1)
#else
		if (pDecGlobal->nThdNum <= 1)
#endif
		{
			src_y = pDecGlobal->dec_picture->imgY+pDecLocal->pix_y*pDecGlobal->iLumaStride+15*iLumaStride+pDecLocal->pix_x;
			src_u = pDecGlobal->dec_picture->imgUV[0]+pDecLocal->pix_c_y*pDecGlobal->iChromaStride+7*iChromaStride+pDecLocal->pix_c_x;
			src_v = pDecGlobal->dec_picture->imgUV[1]+pDecLocal->pix_c_y*pDecGlobal->iChromaStride+7*iChromaStride+pDecLocal->pix_c_x;
		}
		/*else
		{*/
#endif
		/*if (pDecLocal->mb.x != pDecGlobal->PicWidthInMbs - 1)
		{
			VO_U32 i;
			VO_U64 tmp[2];
			RESTRICT VO_U8 *p;

			src_y = pDecGlobal->dec_picture->imgY+pDecLocal->pix_y*iLumaStride+pDecLocal->pix_x + 15;
			src_u = pDecGlobal->dec_picture->imgUV[0]+pDecLocal->pix_c_y*iChromaStride+pDecLocal->pix_c_x + 7;
			src_v = pDecGlobal->dec_picture->imgUV[1]+pDecLocal->pix_c_y*iChromaStride+pDecLocal->pix_c_x + 7;
			intra_buffer = (VO_U64 *)pDecLocal->left_intra;

			p = (VO_U8 *)tmp;
			i = 3;
			do 
			{
				*p++ = *src_y;
				src_y += iLumaStride;
				*p++ = *src_y;
				src_y += iLumaStride;
				*p++ = *src_y;
				src_y += iLumaStride;
				*p++ = *src_y;
				src_y += iLumaStride;
			} while (--i);
			*p++ = *src_y;
			src_y += iLumaStride;
			*p++ = *src_y;
			src_y += iLumaStride;
			*p++ = *src_y;
			src_y += iLumaStride;
			*p = *src_y;
			src_y -= 15;
			*(intra_buffer++) = tmp[0];
			*(intra_buffer++) = tmp[1];

			p = (VO_U8 *)tmp;
			i = 3;
			do 
			{
				*p++ = *src_u;
				src_u += iChromaStride;
				*p++ = *src_u;
				src_u += iChromaStride;
			} while (--i);
			*p++ = *src_u;
			src_u += iChromaStride;
			*p++ = *src_u;
			src_u -= 7;

			i = 3;
			do 
			{
				*p++ = *src_v;
				src_v += iChromaStride;
				*p++ = *src_v;
				src_v += iChromaStride;
			} while (--i);
			*p++ = *src_v;
			src_v += iChromaStride;
			*p = *src_v;
			src_v -= 7;
			*(intra_buffer++) = tmp[0];
			*intra_buffer = tmp[1];
		}
		else
		{
			src_y = pDecGlobal->dec_picture->imgY+(pDecLocal->pix_y+15)*iLumaStride+pDecLocal->pix_x;
			src_u = pDecGlobal->dec_picture->imgUV[0]+(pDecLocal->pix_c_y+7)*iChromaStride+pDecLocal->pix_c_x;
			src_v = pDecGlobal->dec_picture->imgUV[1]+(pDecLocal->pix_c_y+7)*iChromaStride+pDecLocal->pix_c_x;
		}*/
#if ENABLE_DEBLOCK_MB
		/*}*/
#endif
		if (pDecLocal->mb.y != pDecGlobal->FrameHeightInMbs- 1)
		{
			const VO_U64 *src_y_64 = (VO_U64 *)src_y;
			intra_buffer = (VO_U64 *)(pDecGlobal->intra_pix_buffer+mbxy*32);
			*(intra_buffer++) = *(src_y_64++);
			*(intra_buffer++) = *src_y_64;
			*(intra_buffer++) = M64(src_u);
			*intra_buffer = M64(src_v);
		}
	}
	else
	{
#if ENABLE_DEBLOCK_MB
#if USE_FRAME_THREAD
		//if (!pDecGlobal->interlace||pDecGlobal->nThdNum <= 1)
#else
		if (pDecGlobal->nThdNum <= 1)
#endif
		{
			src_y = pDecGlobal->dec_picture->imgY+(pDecLocal->pix_y+30)*iLumaStride+pDecLocal->pix_x;
			src_u = pDecGlobal->dec_picture->imgUV[0]+(pDecLocal->pix_c_y+14)*iChromaStride+pDecLocal->pix_c_x;
			src_v = pDecGlobal->dec_picture->imgUV[1]+(pDecLocal->pix_c_y+14)*iChromaStride+pDecLocal->pix_c_x;
		}
		/*else
		{*/
#endif
		/*if (pDecLocal->mb.x != pDecGlobal->PicWidthInMbs - 1)
		{
			VO_U32 i;
			VO_U64 tmp[4];
			RESTRICT VO_U8 *p;
			const VO_U64 *tmp_p = tmp;

			src_y = pDecGlobal->dec_picture->imgY+pDecLocal->pix_y*iLumaStride+pDecLocal->pix_x + 15;
			src_u = pDecGlobal->dec_picture->imgUV[0]+pDecLocal->pix_c_y*iChromaStride+pDecLocal->pix_c_x + 7;
			src_v = pDecGlobal->dec_picture->imgUV[1]+pDecLocal->pix_c_y*iChromaStride+pDecLocal->pix_c_x + 7;
			intra_buffer = (VO_U64 *)pDecLocal->left_intra;

			p = (VO_U8 *)tmp;
			i = 7;
			do 
			{
				*p++ = *src_y;
				src_y += iLumaStride;
				*p++ = *src_y;
				src_y += iLumaStride;
				*p++ = *src_y;
				src_y += iLumaStride;
				*p++ = *src_y;
				src_y += iLumaStride;
			} while (--i);
			*p++ = *src_y;
			src_y += iLumaStride;
			*p++ = *src_y;
			src_y += iLumaStride;
			*p++ = *src_y;
			src_y += iLumaStride;
			*p = *src_y;
			src_y += -(15 + iLumaStride);
			*(intra_buffer++) = *(tmp_p++);
			*(intra_buffer++) = *(tmp_p++);
			*(intra_buffer++) = *(tmp_p++);
			*(intra_buffer++) = *tmp_p;

			tmp_p = tmp;
			p = (VO_U8 *)tmp;
			i = 7;
			do 
			{
				*p++ = *src_u;
				src_u += iChromaStride;
				*p++ = *src_u;
				src_u += iChromaStride;
			} while (--i);
			*p++ = *src_u;
			src_u += iChromaStride;
			*p++ = *src_u;
			src_u += -(7+iChromaStride);

			i = 7;
			do 
			{
				*p++ = *src_v;
				src_v += iChromaStride;
				*p++ = *src_v;
				src_v += iChromaStride;
			} while (--i);
			*p++ = *src_v;
			src_v += iChromaStride;
			*p = *src_v;
			src_v += -(7+iChromaStride);
			*(intra_buffer++) = *(tmp_p++);
			*(intra_buffer++) = *(tmp_p++);
			*(intra_buffer++) = *(tmp_p++);
			*intra_buffer = *tmp_p;
		}
		else
		{
			src_y = pDecGlobal->dec_picture->imgY+(pDecLocal->pix_y+30)*iLumaStride+pDecLocal->pix_x;
			src_u = pDecGlobal->dec_picture->imgUV[0]+(pDecLocal->pix_c_y+14)*iChromaStride+pDecLocal->pix_c_x;
			src_v = pDecGlobal->dec_picture->imgUV[1]+(pDecLocal->pix_c_y+14)*iChromaStride+pDecLocal->pix_c_x;
		}*/
#if ENABLE_DEBLOCK_MB
		/*}*/
#endif

		if (pDecLocal->mb.y != pDecGlobal->PicHeightInMbs - 1)
		{
			const VO_U64 *src_64 = (VO_U64 *)src_y;
			intra_buffer = (VO_U64 *)(pDecGlobal->intra_pix_buffer+mbxy*64);
			*(intra_buffer++) = *(src_64++);
			*(intra_buffer++) = *src_64;
			src_64 += iLumaStride/8-1;
			*(intra_buffer++) = *(src_64++);
			*(intra_buffer++) = *src_64;
			src_64 = (VO_U64 *)src_u;
			*(intra_buffer++) = *src_64;
			src_64 += iChromaStride/8;
			*(intra_buffer++) = *src_64;
			src_64 = (VO_U64 *)src_v;
			*(intra_buffer++) = *src_64;
			src_64 += iChromaStride/8;
			*intra_buffer = *src_64;			
		}
	}
}

VO_S32 decode_one_slice(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,Slice *currSlice)
{
  //VideoParameters *p_Vid = currSlice->p_Vid;
  Boolean end_of_slice = FALSE;
  VO_S32 ret;
// #if USE_MULTI_THREAD
  VO_S32 num_dec_mb = pDecGlobal->num_dec_mb;
  VO_S32 b_plus = currSlice->structure!=FRAME?2:1;
// #endif
  //Macroblock *currMB = NULL;
  pDecGlobal->mb_x = currSlice->current_mb_nr % pDecGlobal->PicWidthInMbs;
  pDecGlobal->mb_y = (currSlice->current_mb_nr / pDecGlobal->PicWidthInMbs) << (currSlice->field_pic_flag);
  if (currSlice->structure == BOTTOM_FIELD)
    pDecGlobal->mb_y = pDecGlobal->mb_y + 1;
  currSlice->cod_counter=-1;
  currSlice->start_mb_nr_mbaff=currSlice->start_mb_nr = pDecGlobal->mb_y*pDecGlobal->PicWidthInMbs+pDecGlobal->mb_x;
  pDecLocal->p_Slice = currSlice;
  
  //YU B slice mvscale
  if (currSlice->slice_type == B_SLICE)
  {
    ret = compute_colocated(pDecGlobal,currSlice, currSlice->listX[currSlice->slice_number]);//mask by Really Yang for field 20110425
	if (ret)
	{
		return ret;
	}
  }

  {

//  #else
  //reset_ec_flags(p_Vid);
//   while (end_of_slice == FALSE) // loop over macroblocks
  do
  {

    if (currSlice->current_mb_nr >= pDecGlobal->PicSizeInMbs )
	{
	  currSlice->current_mb_nr = 0;
	  pDecGlobal->num_dec_mb = num_dec_mb;
	  VOH264ERROR(VO_H264_ERR_MB_NUM);
	}

#if TRACE
    fprintf(p_Dec->p_trace,"\n*********** POC: %i (I/P) MB: %i Slice: %i Type %d **********\n", currSlice->ThisPOC, currSlice->current_mb_nr, currSlice->current_slice_nr, currSlice->slice_type);
#endif

#if(CAL_PARSER)
	{
		TIME_T start,end;
		gettime (&start);		   // start time
#endif 

    start_macroblock(pDecGlobal,pDecLocal,currSlice);
	WaitAnyKey("start mb end");
    if(pDecLocal->mb_xy>=pDecGlobal->FrameSizeInMbs)
	  VOH264ERROR(VO_H264_ERR_MB_NUM); 
	//if (pDecLocal->mbAddrX == 4)
	//{
    //	end_of_slice = end_of_slice;
	//}
	//printf("mbAddr:%d\n",pDecLocal->mbAddrX);
    // Get the syntax elements from the NAL
      //printf("mb: %d\n",pDecLocal->mbAddrX);
    ret = pDecGlobal->read_one_macroblock(pDecGlobal,pDecLocal);
	if(ret)
	{
	  pDecGlobal->num_dec_mb = num_dec_mb;
	  return ret;
	}
	WaitAnyKey("read mb end");
    
    exit_macroblock(pDecGlobal,pDecLocal,currSlice);
	decode_one_macroblock(pDecGlobal,pDecLocal, pDecGlobal->dec_picture);
#if 0
			  {
				FILE *fp = fopen("d://new_data.txt", "a+");
				int stride = (currSlice->field_pic_flag||pDecLocal->mb_field)?pDecGlobal->iLumaStride<<1:pDecGlobal->iLumaStride;
				int i,j;
				VO_U8* src = pDecGlobal->dec_picture->imgY+pDecLocal->pix_y*stride+pDecLocal->pix_x;
				if(pDecLocal->mb_field)
				{
				  src = pDecGlobal->dec_picture->imgY+(pDecLocal->pix_y)*pDecGlobal->iLumaStride+pDecLocal->pix_x;
				  //if(pDecLocal->mb.y&1)
				  //  src -= 15*pDecGlobal->iLumaStride;
				}
				
				//if(currSlice->structure==BOTTOM_FIELD)
				//{
				//	src +=	pDecGlobal->iLumaStride;
				//}
				
							  
				fprintf(fp,"nb_num:%d\n\r", pDecLocal->mbAddrX );
				for(j = 0;j < 16;j++)
				{
				  for(i = 0;i < 16;i++)
				  {
					fprintf(fp,"%d ", src[i] );
				  }
				  fprintf(fp,"\n\r");
				  src+=stride;
				}
				stride = (currSlice->field_pic_flag||pDecLocal->mb_field)?pDecGlobal->iChromaStride<<1:pDecGlobal->iChromaStride;
				src = pDecGlobal->dec_picture->imgUV[0]+pDecLocal->pix_c_y*stride+pDecLocal->pix_c_x;
				if(pDecLocal->mb_field)
				{
				  src = pDecGlobal->dec_picture->imgUV[0]+(pDecLocal->pix_c_y)*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
				  //if(pDecLocal->mb.y&1)
				  //  src -= 7*pDecGlobal->iChromaStride;
				}
				//if(currSlice->structure==BOTTOM_FIELD)
				//{
				//	src +=	pDecGlobal->iChromaStride;
				//}
				for(j = 0;j < 8;j++)
				{
				  for(i = 0;i < 8;i++)
				  {
					fprintf(fp,"%d ", src[i] );
				  }
				  fprintf(fp,"\n\r");
				  src+=stride;
				}
				src = pDecGlobal->dec_picture->imgUV[1]+pDecLocal->pix_c_y*stride+pDecLocal->pix_c_x;
				if(pDecLocal->mb_field)
				{
				  src = pDecGlobal->dec_picture->imgUV[1]+(pDecLocal->pix_c_y)*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
				  //if(pDecLocal->mb.y&1)
				  //  src -= 7*pDecGlobal->iChromaStride;
				}
				//if(currSlice->structure==BOTTOM_FIELD)
				//{
				//	src +=	pDecGlobal->iChromaStride;
				//}
				for(j = 0;j < 8;j++)
				{
				  for(i = 0;i < 8;i++)
				  {
					fprintf(fp,"%d ", src[i] );
				  }
				  fprintf(fp,"\n\r");
				  src+=stride;
				}
					  
				fclose(fp);
			  }
#endif

    ++num_dec_mb;
	if(currSlice->mb_aff_frame_flag)
	{
	  currSlice->current_mb_nr=num_dec_mb;
	  pDecGlobal->mb_y++;
	  start_macroblock(pDecGlobal,pDecLocal,currSlice);
	  //if (pDecLocal->mbAddrX == 39)
	  //{
		//  end_of_slice = end_of_slice;
	  //}
	  if(pDecLocal->mb_xy>=pDecGlobal->FrameSizeInMbs)
		  VOH264ERROR(VO_H264_ERR_MB_NUM); 
	  ret = pDecGlobal->read_one_macroblock(pDecGlobal,pDecLocal);	
	  if(ret)
	  {
	    pDecGlobal->num_dec_mb = num_dec_mb;
	    return ret;
	  }
	  exit_macroblock(pDecGlobal,pDecLocal,currSlice);
	  decode_one_macroblock(pDecGlobal,pDecLocal, pDecGlobal->dec_picture);
#if 0
				{
				  FILE *fp = fopen("d://new_data.txt", "a+");
				  int stride = (currSlice->field_pic_flag||pDecLocal->mb_field)?pDecGlobal->iLumaStride<<1:pDecGlobal->iLumaStride;
				  int i,j;
				  VO_U8* src = pDecGlobal->dec_picture->imgY+pDecLocal->pix_y*stride+pDecLocal->pix_x;
				  if(pDecLocal->mb_field)
				  {
					src = pDecGlobal->dec_picture->imgY+(pDecLocal->pix_y)*pDecGlobal->iLumaStride+pDecLocal->pix_x;
					//if(pDecLocal->mb.y&1)
					//	src -= 15*pDecGlobal->iLumaStride;
				  }
				  
				  //if(currSlice->structure==BOTTOM_FIELD)
				  //{
				  //  src +=  pDecGlobal->iLumaStride;
				  //}
				  
								
				  fprintf(fp,"nb_num:%d\n\r", pDecLocal->mbAddrX );
				  for(j = 0;j < 16;j++)
				  {
					for(i = 0;i < 16;i++)
					{
					  fprintf(fp,"%d ", src[i] );
					}
					fprintf(fp,"\n\r");
					src+=stride;
				  }
				  stride = (currSlice->field_pic_flag||pDecLocal->mb_field)?pDecGlobal->iChromaStride<<1:pDecGlobal->iChromaStride;
				  src = pDecGlobal->dec_picture->imgUV[0]+pDecLocal->pix_c_y*stride+pDecLocal->pix_c_x;
				  if(pDecLocal->mb_field)
				  {
					src = pDecGlobal->dec_picture->imgUV[0]+(pDecLocal->pix_c_y)*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
					//if(pDecLocal->mb.y&1)
					//	src -= 7*pDecGlobal->iChromaStride;
				  }
				  //if(currSlice->structure==BOTTOM_FIELD)
				  //{
				  //  src +=  pDecGlobal->iChromaStride;
				  //}
				  for(j = 0;j < 8;j++)
				  {
					for(i = 0;i < 8;i++)
					{
					  fprintf(fp,"%d ", src[i] );
					}
					fprintf(fp,"\n\r");
					src+=stride;
				  }
				  src = pDecGlobal->dec_picture->imgUV[1]+pDecLocal->pix_c_y*stride+pDecLocal->pix_c_x;
				  if(pDecLocal->mb_field)
				  {
					src = pDecGlobal->dec_picture->imgUV[1]+(pDecLocal->pix_c_y)*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
					//if(pDecLocal->mb.y&1)
					//	src -= 7*pDecGlobal->iChromaStride;
				  }
				  //if(currSlice->structure==BOTTOM_FIELD)
				  //{
				  //  src +=  pDecGlobal->iChromaStride;
				  //}
				  for(j = 0;j < 8;j++)
				  {
					for(i = 0;i < 8;i++)
					{
					  fprintf(fp,"%d ", src[i] );
					}
					fprintf(fp,"\n\r");
					src+=stride;
				  }
						
				  fclose(fp);
				}
#endif

	  pDecGlobal->mb_y--;
	  ++num_dec_mb;
	}
	if(num_dec_mb == pDecGlobal->PicSizeInMbs) //if (p_Vid->num_dec_mb == p_Vid->PicSizeInMbs)
	{
		end_of_slice =  TRUE;
	}
	else
	{
		VO_S32 eos_bit = (!currSlice->mb_aff_frame_flag|| currSlice->current_mb_nr%2);
// 		currSlice->current_mb_nr++;// = FmoGetNextMBNr (pDecGlobal, currSlice->current_mb_nr);

		if (currSlice->current_mb_nr == -2)     // End of Slice group, MUST be end of slice
		{
			assert (pDecGlobal->nal_startcode_follows (pDecGlobal,currSlice, eos_bit) == TRUE);
			end_of_slice = TRUE;
		}
		else if(pDecGlobal->nal_startcode_follows(pDecGlobal,currSlice, eos_bit) == FALSE)
			end_of_slice = FALSE;
		else if(currSlice->slice_type == I_SLICE  || currSlice->slice_type == SI_SLICE || currSlice->active_pps->entropy_coding_mode_flag == CABAC)
			end_of_slice = TRUE;
		else if(currSlice->cod_counter <= 0)
			end_of_slice = TRUE;
		else
			end_of_slice = FALSE;
	}
	WaitAnyKey("parser end");



#if(CAL_PARSER)
		gettime (&end); 			// end time
		pDecGlobal->tot_parser += timediff(&start, &end);
	}
#endif 

#if(CAL_MC)
		{
			TIME_T start,end;
			gettime (&start);		   // start time
#endif 


	  
      
	  WaitAnyKey("decoder end");


#if 0
	  if(currSlice->mb_aff_frame_flag && currMB->mb_field)
	  {
		  currSlice->num_ref_idx_active[LIST_0] >>= 1;
		  currSlice->num_ref_idx_active[LIST_1] >>= 1;
	  }
#endif
#if(CAL_MC)
	  gettime (&end); 			// end time
	  pDecGlobal->tot_mc += timediff(&start, &end);
  }
#endif 


#if(ENABLE_DEBLOCK_MB)
#if USE_FRAME_THREAD
  if (pDecGlobal->iDeblockMode/* && (!pDecGlobal->interlace||pDecGlobal->nThdNum <= 1)*/)
#else
  if (pDecGlobal->nThdNum <= 1 && pDecGlobal->iDeblockMode)
#endif
  {
#if(CAL_DEBLOCK)
	  TIME_T start,end;

	  gettime (&start); 			 // start time
#endif 
	  if (!currSlice->mb_aff_frame_flag)
	  {
		  if ( ++pDecGlobal->mb_x >= pDecGlobal->PicWidthInMbs || end_of_slice)
		  {
			  VO_S32 mb_xy = pDecGlobal->mb_y*pDecGlobal->PicWidthInMbs;
			  VO_S32 mb_x = 0;
			  pDecLocal->pix_y = pDecGlobal->mb_y*MB_BLOCK_SIZE;
			  pDecLocal->pix_c_y = pDecGlobal->mb_y*8;
			  if(pDecLocal->mb_field&&pDecGlobal->mb_y&1)
              {
                pDecLocal->pix_y -= 15;
	            pDecLocal->pix_c_y -= 7;
              }
			  if (mb_xy < currSlice->start_mb_nr_mbaff)
			  {
				  mb_xy = currSlice->start_mb_nr_mbaff;
				  mb_x = mb_xy%pDecGlobal->PicWidthInMbs;
				  pDecLocal->pix_c_x = (pDecLocal->pix_x = mb_x*MB_BLOCK_SIZE)/2;			  
			  }
			  else
			  {
				  pDecLocal->pix_x = pDecLocal->pix_c_x = 0;
			  }
			  for(;mb_x<pDecGlobal->mb_x;mb_x++)
			  {
				  update_intra_buffer(pDecGlobal,pDecLocal,mb_xy); 
				  if(!update_deblock_cache(pDecGlobal,pDecLocal,mb_xy++,mb_x))
				  {
					  DeblockMb( pDecGlobal,pDecLocal, pDecGlobal->dec_picture );
#if 0
					  {
						  FILE *fp = fopen("d://new_data.txt", "a+");
						  int stride = (currSlice->field_pic_flag||pDecLocal->mb_field)?pDecGlobal->iLumaStride<<1:pDecGlobal->iLumaStride;
						  int i,j;
						  VO_U8* src = pDecGlobal->dec_picture->imgY+pDecLocal->pix_y*stride+pDecLocal->pix_x;
						  if(pDecLocal->mb_field)
						  {
							  src = pDecGlobal->dec_picture->imgY+(pDecLocal->pix_y)*pDecGlobal->iLumaStride+pDecLocal->pix_x;
							  //if(pDecLocal->mb.y&1)
							  //  src -= 15*pDecGlobal->iLumaStride;
						  }

						  //if(currSlice->structure==BOTTOM_FIELD)
						  //{
						  //	src +=	pDecGlobal->iLumaStride;
						  //}


						  //fprintf(fp,"nb_num:%d\n\r", mb_xy-1 );
						  for(j = 0;j < 16;j++)
						  {
							  for(i = 0;i < 16;i++)
							  {
								  fprintf(fp,"%d ", src[i] );
							  }
							  fprintf(fp,"\n\r");
							  src+=stride;
						  }
						  stride = (currSlice->field_pic_flag||pDecLocal->mb_field)?pDecGlobal->iChromaStride<<1:pDecGlobal->iChromaStride;
						  src = pDecGlobal->dec_picture->imgUV[0]+pDecLocal->pix_c_y*stride+pDecLocal->pix_c_x;
						  if(pDecLocal->mb_field)
						  {
							  src = pDecGlobal->dec_picture->imgUV[0]+(pDecLocal->pix_c_y)*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
							  //if(pDecLocal->mb.y&1)
							  //  src -= 7*pDecGlobal->iChromaStride;
						  }
						  //if(currSlice->structure==BOTTOM_FIELD)
						  //{
						  //	src +=	pDecGlobal->iChromaStride;
						  //}
						  for(j = 0;j < 8;j++)
						  {
							  for(i = 0;i < 8;i++)
							  {
								  fprintf(fp,"%d ", src[i] );
							  }
							  fprintf(fp,"\n\r");
							  src+=stride;
						  }
						  src = pDecGlobal->dec_picture->imgUV[1]+pDecLocal->pix_c_y*stride+pDecLocal->pix_c_x;
						  if(pDecLocal->mb_field)
						  {
							  src = pDecGlobal->dec_picture->imgUV[1]+(pDecLocal->pix_c_y)*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
							  //if(pDecLocal->mb.y&1)
							  //  src -= 7*pDecGlobal->iChromaStride;
						  }
						  //if(currSlice->structure==BOTTOM_FIELD)
						  //{
						  //	src +=	pDecGlobal->iChromaStride;
						  //}
						  for(j = 0;j < 8;j++)
						  {
							  for(i = 0;i < 8;i++)
							  {
								  fprintf(fp,"%d ", src[i] );
							  }
							  fprintf(fp,"\n\r");
							  src+=stride;
						  }

						  fclose(fp);
					  }
#endif
				  }
				  pDecLocal->pix_x += MB_BLOCK_SIZE;
				  pDecLocal->pix_c_x += 8;
			  }
			  pDecGlobal->mb_x=0;
#if USE_FRAME_THREAD
			  if (pDecGlobal->nThdNum > 1)
			  {
				  FrameThdInfo *pInfo = pDecGlobal->pThdInfo;
				  if (pInfo->errFlag)	//check if any error happened
				  {
				      pDecGlobal->num_dec_mb = num_dec_mb;
					  return pInfo->errFlag;
				  }
				  if (pDecLocal->mb.x+1 >= pDecGlobal->PicWidthInMbs/* && pDecLocal->mb.y > 0*/)
				  {
					  VO_S32 p_y;
					  //printf("%d mb.y:%d\r\n",pDecGlobal->dec_picture->poc,pDecLocal->mb.y);
					  if (pDecLocal->mb.y + b_plus < pDecGlobal->FrameHeightInMbs)
					  {
						  p_y = pDecLocal->mb.y*MB_BLOCK_SIZE + 11;
					  }
					  else
					  {
						  p_y = INT_MAX;
					  }
					  if (p_y > 0)
					  {
						  pthread_mutex_lock(pInfo->progressMutex);
						  pDecGlobal->dec_picture->nProgress[0] = p_y;
						  if(pDecGlobal->interlace)
						  {
						    if(currSlice->structure==FRAME)
						    {
						      if(pDecGlobal->dec_picture->top_set)
						        pDecGlobal->dec_picture->top_field->nProgress[0] = p_y;
							  if(pDecGlobal->dec_picture->bottom_set)
							    pDecGlobal->dec_picture->bottom_field->nProgress[0] = p_y;
						    }
							else if(currSlice->structure==TOP_FIELD)
							{
							  if(pDecGlobal->dec_picture->bottom_set)
							    pDecGlobal->dec_picture->frame->nProgress[0] = imin(p_y,pDecGlobal->dec_picture->bottom_field->nProgress[0]);
							  else if(pDecGlobal->dec_picture->frame_set)
							  	pDecGlobal->dec_picture->frame->nProgress[0] = p_y;
							  
							}
							else
							{
							  if(pDecGlobal->dec_picture->top_set)
							    pDecGlobal->dec_picture->frame->nProgress[0] = imin(p_y,pDecGlobal->dec_picture->top_field->nProgress[0]);
							  else if(pDecGlobal->dec_picture->frame_set)
							  	pDecGlobal->dec_picture->frame->nProgress[0] = p_y;
							}
						  }
//printf("%d progress %d\n", pDecGlobal->dec_picture->poc, pDecGlobal->dec_picture->nProgress[0]);
						  pthread_cond_broadcast(pInfo->progressCond);
#if USE_WIN32_BROADCAST
#if defined(VOWINCE) || defined(WIN32)
						  {
							  VO_U32 i;
							  for (i = pDecGlobal->nThdNum-1; i > 0; --i)
							  {
								  pthread_cond_signal(pInfo->progressCond);
							  }
						  }
#endif
#endif
						  pthread_mutex_unlock(pInfo->progressMutex);
					  }
				  }
			  }
#endif      
            pDecGlobal->mb_y++;
            if(currSlice->field_pic_flag)
			  pDecGlobal->mb_y++;
		  }
	  }
	  else
	  {
			if ((++pDecGlobal->mb_x >= pDecGlobal->PicWidthInMbs || end_of_slice))
			{
				VO_S32 mb_xy = pDecGlobal->mb_y*pDecGlobal->PicWidthInMbs;
				VO_S32 mb_x = 0;
				
				pDecLocal->pix_y += 15*pDecLocal->mb_field - MB_BLOCK_SIZE;
				pDecLocal->pix_c_y += 7*pDecLocal->mb_field - 8;
				if (mb_xy < currSlice->start_mb_nr_mbaff)
				{
					mb_xy = currSlice->start_mb_nr_mbaff;
					mb_x = pDecGlobal->PicPos[mb_xy>>1].x;
					pDecLocal->pix_c_x = (pDecLocal->pix_x = mb_x*MB_BLOCK_SIZE)/2;
				}
				else
				{
					pDecLocal->pix_x = pDecLocal->pix_c_x = 0;
				}
				//pDecLocal->pix_x = pDecLocal->pix_c_x = 0;
				for(;mb_x<pDecGlobal->mb_x;mb_x++)
				{
					update_intra_buffer(pDecGlobal,pDecLocal,mb_xy>>1); 
					if(!update_deblock_cache(pDecGlobal,pDecLocal,mb_xy++,mb_x))
					{
						DeblockMb_MBAFF( pDecGlobal,pDecLocal, pDecGlobal->dec_picture ) ; 
					}
					if(!update_deblock_cache(pDecGlobal,pDecLocal,mb_xy++,mb_x))
					{
						DeblockMb_MBAFF( pDecGlobal,pDecLocal, pDecGlobal->dec_picture ) ; 
					}
					pDecLocal->pix_x += MB_BLOCK_SIZE;
					pDecLocal->pix_c_x += 8;
				}
#if USE_FRAME_THREAD
				if (pDecGlobal->nThdNum > 1)
				{
					FrameThdInfo *pInfo = pDecGlobal->pThdInfo;
					if (pInfo->errFlag)	//check if any error happened
					{
					    pDecGlobal->num_dec_mb = num_dec_mb;
						return pInfo->errFlag;
					}
					if (pDecLocal->mb.x+1 >= pDecGlobal->PicWidthInMbs/* && pDecLocal->mb.y > 2*/)
					{
						VO_S32 p_y;
						if (pDecLocal->mb.y + 1 < pDecGlobal->PicHeightInMbs)
						{
							p_y = pDecGlobal->mb_y*MB_BLOCK_SIZE+11;
						}
						else
						{
							p_y = INT_MAX;
						}
						if (p_y > 0)
						{
							pthread_mutex_lock(pInfo->progressMutex);
							pDecGlobal->dec_picture->nProgress[0] = p_y;
							if(pDecGlobal->dec_picture->top_set)
			                  pDecGlobal->dec_picture->top_field->nProgress[0] = p_y;
							if(pDecGlobal->dec_picture->bottom_set)
			                  pDecGlobal->dec_picture->bottom_field->nProgress[0] = p_y;
 //printf("%d progress %d\n", pDecGlobal->dec_picture->poc, p_y);
							pthread_cond_broadcast(pInfo->progressCond);
#if USE_WIN32_BROADCAST
#if defined(VOWINCE) || defined(WIN32)
							{
								VO_U32 i;
								for (i = pDecGlobal->nThdNum-1; i > 0; --i)
								{
									pthread_cond_signal(pInfo->progressCond);
								}
							}
#endif
#endif
							pthread_mutex_unlock(pInfo->progressMutex);
						}
					}
				}
#endif
              pDecGlobal->mb_x = 0;
              pDecGlobal->mb_y+=2;
			}
	  }
#if(CAL_DEBLOCK)
	  gettime (&end);			  // end time
	  pDecGlobal->tot_deblock+= timediff(&start, &end);

#endif 
  }
#if USE_FRAME_THREAD
  else if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
  {
	  if (++pDecGlobal->mb_x >= pDecGlobal->PicWidthInMbs && 
		  (!currSlice->mb_aff_frame_flag || (currSlice->mb_aff_frame_flag && (pDecLocal->mb.y&1))))
	  {
		  FrameThdInfo *pInfo = pDecGlobal->pThdInfo;
		  VO_S32 p_y;
		  pDecGlobal->mb_x = 0;
		  if (pDecLocal->mb.y + b_plus < pDecGlobal->FrameHeightInMbs)
		  {
			  p_y = pDecLocal->mb.y*MB_BLOCK_SIZE + 15;
		  }
		  else
		  {
			  p_y = INT_MAX;
		  }

		  pthread_mutex_lock(pInfo->progressMutex);
		  pDecGlobal->dec_picture->nProgress[0] = p_y;
		  if(currSlice->mb_aff_frame_flag)
		  {
		    if(pDecGlobal->dec_picture->top_set)
		      pDecGlobal->dec_picture->top_field->nProgress[0] = p_y;
			if(pDecGlobal->dec_picture->bottom_set)
			  pDecGlobal->dec_picture->bottom_field->nProgress[0] = p_y;
		  }
		  else if(pDecGlobal->interlace)
		  {
		    if(currSlice->structure==FRAME)
			{
			  if(pDecGlobal->dec_picture->top_set)
			    pDecGlobal->dec_picture->top_field->nProgress[0] = p_y;
			  if(pDecGlobal->dec_picture->bottom_set)
			    pDecGlobal->dec_picture->bottom_field->nProgress[0] = p_y;
			}
			else if(currSlice->structure==TOP_FIELD)
			{
			  if(pDecGlobal->dec_picture->bottom_set)
			    pDecGlobal->dec_picture->frame->nProgress[0] = imin(p_y,pDecGlobal->dec_picture->bottom_field->nProgress[0]);
			  else if(pDecGlobal->dec_picture->frame_set)
			  	pDecGlobal->dec_picture->frame->nProgress[0] = p_y;
			}
			else
			{
			  if(pDecGlobal->dec_picture->top_set)
			    pDecGlobal->dec_picture->frame->nProgress[0] = imin(p_y,pDecGlobal->dec_picture->top_field->nProgress[0]);
			  else if(pDecGlobal->dec_picture->frame_set)
			  	pDecGlobal->dec_picture->frame->nProgress[0] = p_y;
			}
		  }
// printf("%d progress %d\n", pDecGlobal->dec_picture->poc, p_y);
		  pthread_cond_broadcast(pInfo->progressCond);
#if USE_WIN32_BROADCAST
#if defined(VOWINCE) || defined(WIN32)
		  {
			  VO_U32 i;
			  for (i = pDecGlobal->nThdNum-1; i > 0; --i)
			  {
				  pthread_cond_signal(pInfo->progressCond);
			  }
		  }
#endif
#endif
		  pthread_mutex_unlock(pInfo->progressMutex);
          pDecGlobal->mb_y++;
		  if(currSlice->field_pic_flag||currSlice->mb_aff_frame_flag)
			  pDecGlobal->mb_y++;
	  }
  }
  else
  {
    if (++pDecGlobal->mb_x >= pDecGlobal->PicWidthInMbs)
    {
	  pDecGlobal->mb_x = 0;
	  pDecGlobal->mb_y++;
	  if(currSlice->field_pic_flag||currSlice->mb_aff_frame_flag)
		pDecGlobal->mb_y++;
    }
  }
#endif
#endif

	currSlice->current_mb_nr = num_dec_mb;
    


    } while (end_of_slice == FALSE);
  }
// #endif
  //reset_ec_flags(p_Vid);
  pDecGlobal->num_dec_mb = num_dec_mb;
  return 0;
}


