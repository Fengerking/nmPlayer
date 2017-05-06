
#include "global.h"
#include "block.h"
#include "mbuffer.h"
#include "mv_prediction.h"
#include "ifunctions.h"
#include "mb_prediction.h"
static const int BLOCK_STEP[8][2]=
{
  {0,0},{4,4},{4,2},{2,4},{2,2},{2,1},{1,2},{1,1}
};

#if 0
static void update_direct_mv_info_temporal(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32* mb_type)
{
  //VideoParameters *p_Vid = currMB->p_Vid;
  Slice *currSlice = pDecLocal->p_Slice;
  int j,k;
  int partmode        = 4 ;
  int step_h0         = BLOCK_STEP [partmode][0];
  int step_v0         = BLOCK_STEP [partmode][1];

  int i0, j0, j6;

  int j4, i4;
  StorablePicture *dec_picture = pDecGlobal->dec_picture;
  //PicMotionParams *mv_cache = dec_picture->mv_cache+pDecLocal->block_xy;
  int list_offset = pDecLocal->mb_field? (pDecLocal->mb.y&1)?4:2: 0;
  StorablePicture **list0 = currSlice->listX[LIST_0 + list_offset];
  StorablePicture **list1 = currSlice->listX[LIST_1 + list_offset];
  StorablePicture** ref_pic_list0 = &pDecLocal->pic_cache[0][0];
  StorablePicture** ref_pic_list1 = &pDecLocal->pic_cache[1][0];
  VO_S32* mv_cache0 = &pDecLocal->mv_cache[0][12];
  VO_S32* mv_cache1 = &pDecLocal->mv_cache[1][12];
  VO_S8* ref_cache0 = &pDecLocal->ref_cache[0][12];
  VO_S8* ref_cache1 = &pDecLocal->ref_cache[1][12];
  //VO_S8 *ref_idx_list0 = &pDecGlobal->dec_picture->ref_idx[0][pDecLocal->block_xy];
  //VO_S8 *ref_idx_list1 = &pDecGlobal->dec_picture->ref_idx[1][pDecLocal->block_xy];
  VO_S8 colocated_ref_list0,colocated_ref_list1;
  StorablePicture* colocated_ref_pic0,*colocated_ref_pic1;
  VO_S16 pmvl0[2], pmvl1[2];
  VO_S16* colocated_mv0,*colocated_mv1;
  Boolean has_direct = (pDecLocal->b8mode[0] == 0) | (pDecLocal->b8mode[1] == 0) | (pDecLocal->b8mode[2] == 0) | (pDecLocal->b8mode[3] == 0);
  VO_S32 block_xy = pDecLocal->mb_xy*16;
  if(list0[0] == NULL||list1[0] == NULL)
  {
    return ;
  }
  if(pDecLocal->mb_field||(!currSlice->mb_aff_frame_flag&&!currSlice->field_pic_flag && list1[0]->iCodingType!=FRAME_CODING))
  {
    int mb_y = pDecLocal->mb.y;
	int mb_x = pDecLocal->mb.x;
	block_xy = ((mb_y>>1)*pDecGlobal->PicWidthInMbs+mb_x)*16;
  }
  if (has_direct)
  {
    int mv_scale = 0;

#if USE_FRAME_THREAD
	if (pDecGlobal->nThdNum > 1&&!pDecGlobal->interlace)
	{
		//wait for list1[0],mv=(?,?) progress ready
		FrameThdInfo *pInfo = pDecGlobal->pThdInfo;
		const VO_S32 max_y = pDecLocal->mb.y*MB_BLOCK_SIZE + MB_BLOCK_SIZE-1;		//???interlace need fix
		//??????calc max_y2 of combine or split, and max_y = max(max_y, max_y2)

		VO_S32 errFlag;
#if CHECK_REF_WRONG
		errFlag = list1[0]->frame_num - currSlice->frame_num;
		if ((errFlag >= 0 && errFlag < pDecGlobal->MaxFrameNum/2) || (errFlag <= -pDecGlobal->MaxFrameNum/2))
		{
			errFlag = VO_H264_ERR_DUPLICATE_FRAMENUM;
		}
		else
#endif
		{
			errFlag = WaitForProgress(pInfo, list1[0], max_y, 0);
		}
		if(errFlag)
		{
			//ref is error
			pInfo->errFlag = errFlag;
			return;
		}

		//check if list1[0],mv=(?,?) need combine or split

	}
#endif

    for (k = 0; k < 4; ++k) // Scan all blocks
    {
      if (pDecLocal->b8mode[k] == 0)
      {
        pDecLocal->b8pdir[k] = 2;
        for(j0 = 2 * (k >> 1); j0 < 2 * (k >> 1) + 2; j0 += step_v0)
        {
          for(i0 = 2*(k & 0x01); i0 < 2 * (k & 0x01)+2; i0 += step_h0)
          {
            //PicMotionParams *colocated;
            int refList;
            int ref_idx;
			StorablePicture* ref_pic;
            int mapped_idx = -1, iref;
			int col_indexR = block_xy+RSD(j0)*4+RSD(i0);
			int col_index  = block_xy+j0*4+i0;
			if(currSlice->active_sps->direct_8x8_inference_flag)
			{
			  colocated_ref_list0 = list1[0]->ref_idx[0][col_indexR];
			  colocated_ref_list1 = list1[0]->ref_idx[1][col_indexR];
			  colocated_ref_pic0 = list1[0]->ref_pic[0][col_indexR];
			  colocated_ref_pic1 = list1[0]->ref_pic[1][col_indexR];
			}
			else
			{
			  colocated_ref_list0 = list1[0]->ref_idx[0][col_index];
			  colocated_ref_list1 = list1[0]->ref_idx[1][col_index];
			  colocated_ref_pic0 = list1[0]->ref_pic[0][col_index];
			  colocated_ref_pic1 = list1[0]->ref_pic[1][col_index];
			}
#if 1
            if(currSlice->mb_aff_frame_flag)
            {
              if(!pDecLocal->mb_field && ((currSlice->listX[LIST_1][0]->iCodingType==FRAME_MB_PAIR_CODING && IS_INTERLACED(currSlice->listX[LIST_1][0]->mb_type_buffer[pDecLocal->mb_xy])) ||
               (currSlice->listX[LIST_1][0]->iCodingType==FIELD_CODING)))
              {
                int mb_y = pDecLocal->mb.y;
			    int mb_x = pDecLocal->mb.x;
			    int block_xy = ((mb_y>>1)*pDecGlobal->PicWidthInMbs+mb_x)*16;
        	    col_indexR = (block_xy)+(RSD(j0)>>1)*4+RSD(i0);
			    col_index = (block_xy)+j0*2+i0;
				col_indexR += (pDecLocal->mb.y&1)*8;
			    col_index += (pDecLocal->mb.y&1)*8;
                if (iabs(pDecGlobal->dec_picture->poc - currSlice->listX[LIST_1+4][0]->poc)> iabs(pDecGlobal->dec_picture->poc -currSlice->listX[LIST_1+2][0]->poc) )
                {
                  if(currSlice->active_sps->direct_8x8_inference_flag)
			      {
      			    colocated_ref_list0 = currSlice->listX[LIST_1 + 2][0]->ref_idx[0][col_indexR];
      	  	        colocated_ref_list1 = currSlice->listX[LIST_1 + 2][0]->ref_idx[1][col_indexR];
      	  	        colocated_ref_pic0 = currSlice->listX[LIST_1 + 2][0]->ref_pic[0][col_indexR];
      	  	        colocated_ref_pic1 = currSlice->listX[LIST_1 + 2][0]->ref_pic[1][col_indexR];
                  }
				  else
				  {
				    colocated_ref_list0 = currSlice->listX[LIST_1 + 2][0]->ref_idx[0][col_index];
      	  	        colocated_ref_list1 = currSlice->listX[LIST_1 + 2][0]->ref_idx[1][col_index];
      	  	        colocated_ref_pic0 = currSlice->listX[LIST_1 + 2][0]->ref_pic[0][col_index];
      	  	        colocated_ref_pic1 = currSlice->listX[LIST_1 + 2][0]->ref_pic[1][col_index];
				  }
                }
                else
                {
                  if(currSlice->active_sps->direct_8x8_inference_flag)
			      {
                    colocated_ref_list0 = currSlice->listX[LIST_1 + 4][0]->ref_idx[0][col_indexR];
      	  	        colocated_ref_list1 = currSlice->listX[LIST_1 + 4][0]->ref_idx[1][col_indexR];
      	  	        colocated_ref_pic0 = currSlice->listX[LIST_1 + 4][0]->ref_pic[0][col_indexR];
      	  	        colocated_ref_pic1 = currSlice->listX[LIST_1 + 4][0]->ref_pic[1][col_indexR];
                  }
      	  	      else
      	  	      {
      	  	        colocated_ref_list0 = currSlice->listX[LIST_1 + 4][0]->ref_idx[0][col_index];
      	  	        colocated_ref_list1 = currSlice->listX[LIST_1 + 4][0]->ref_idx[1][col_index];
      	  	        colocated_ref_pic0 = currSlice->listX[LIST_1 + 4][0]->ref_pic[0][col_index];
      	  	        colocated_ref_pic1 = currSlice->listX[LIST_1 + 4][0]->ref_pic[1][col_index];
      	  	      }
                }
              }
            }
            else if(!currSlice->active_sps->frame_mbs_only_flag && !currSlice->field_pic_flag && currSlice->listX[LIST_1][0]->iCodingType != FRAME_CODING)
            {
              int mb_y = pDecLocal->mb.y;
			  int mb_x = pDecLocal->mb.x;
			  int block_xy = ((mb_y>>1)*pDecGlobal->PicWidthInMbs+mb_x)*16;
        	  col_indexR = (block_xy)+(RSD(j0)>>1)*4+RSD(i0);
			  col_index = (block_xy)+j0*2+i0;
			  col_indexR += (pDecLocal->mb.y&1)*8;
			  col_index += (pDecLocal->mb.y&1)*8;
              if (iabs(dec_picture->poc - list1[0]->bottom_field->poc)> iabs(dec_picture->poc -list1[0]->top_field->poc) )
              {
                //colocated = pDecGlobal->active_sps->direct_8x8_inference_flag ? 
                //  &list1[0]->top_field->mv_info[RSD(pDecLocal->block_y_aff + j0)>>1][RSD(i0)] : &list1[0]->top_field->mv_info[(pDecLocal->block_y_aff + j0)>>1][i0];
                if(currSlice->active_sps->direct_8x8_inference_flag)
			    {
      			  colocated_ref_list0 = list1[0]->top_field->ref_idx[0][col_indexR];
      			  colocated_ref_list1 = list1[0]->top_field->ref_idx[1][col_indexR];
      			  colocated_ref_pic0 = list1[0]->top_field->ref_pic[0][col_indexR];
      			  colocated_ref_pic1 = list1[0]->top_field->ref_pic[1][col_indexR];
      			}
      			else
      			{
      			  colocated_ref_list0 = list1[0]->top_field->ref_idx[0][col_index];
      			  colocated_ref_list1 = list1[0]->top_field->ref_idx[1][col_index];
      			  colocated_ref_pic0 = list1[0]->top_field->ref_pic[0][col_index];
      			  colocated_ref_pic1 = list1[0]->top_field->ref_pic[1][col_index];
      			}
              }
              else
              {
                //colocated = pDecGlobal->active_sps->direct_8x8_inference_flag ? 
                //  &list1[0]->bottom_field->mv_info[RSD(pDecLocal->block_y_aff + j0)>>1][RSD(i0)] : &list1[0]->bottom_field->mv_info[(pDecLocal->block_y_aff + j0)>>1][i0];
				if(currSlice->active_sps->direct_8x8_inference_flag)
			    {
      			  colocated_ref_list0 = list1[0]->bottom_field->ref_idx[0][col_indexR];
      			  colocated_ref_list1 = list1[0]->bottom_field->ref_idx[1][col_indexR];
      			  colocated_ref_pic0 = list1[0]->bottom_field->ref_pic[0][col_indexR];
      			  colocated_ref_pic1 = list1[0]->bottom_field->ref_pic[1][col_indexR];
      			}
      			else
      			{
      			  colocated_ref_list0 = list1[0]->bottom_field->ref_idx[0][col_index];
      			  colocated_ref_list1 = list1[0]->bottom_field->ref_idx[1][col_index];
      			  colocated_ref_pic0 = list1[0]->bottom_field->ref_pic[0][col_index];
      			  colocated_ref_pic1 = list1[0]->bottom_field->ref_pic[1][col_index];
      			}
              }
            }
            else if(!currSlice->active_sps->frame_mbs_only_flag && currSlice->field_pic_flag && currSlice->structure!=list1[0]->structure && list1[0]->coded_frame)
            {
              col_indexR += (pDecLocal->mb.y&1)*8;
			  col_index += (pDecLocal->mb.y&1)*8;
              if (currSlice->structure == TOP_FIELD)
              {
                //colocated = pDecGlobal->active_sps->direct_8x8_inference_flag ? 
                  //&list1[0]->frame->top_field->mv_info[RSD(pDecLocal->block_y_aff + j0)][RSD(i0)] : &list1[0]->frame->top_field->mv_info[pDecLocal->block_y_aff + j0][i0];
                if(currSlice->active_sps->direct_8x8_inference_flag)
			    {
      			  colocated_ref_list0 = list1[0]->top_field->ref_idx[0][col_indexR];
      			  colocated_ref_list1 = list1[0]->top_field->ref_idx[1][col_indexR];
      			  colocated_ref_pic0 = list1[0]->top_field->ref_pic[0][col_indexR];
      			  colocated_ref_pic1 = list1[0]->top_field->ref_pic[1][col_indexR];
      			}
      			else
      			{
      			  colocated_ref_list0 = list1[0]->top_field->ref_idx[0][col_index];
      			  colocated_ref_list1 = list1[0]->top_field->ref_idx[1][col_index];
      			  colocated_ref_pic0 = list1[0]->top_field->ref_pic[0][col_index];
      			  colocated_ref_pic1 = list1[0]->top_field->ref_pic[1][col_index];
      			}
              }
              else
              {
                //colocated = pDecGlobal->active_sps->direct_8x8_inference_flag ? 
                //  &list1[0]->frame->bottom_field->mv_info[RSD(pDecLocal->block_y_aff + j0)][RSD(i0)] : &list1[0]->frame->bottom_field->mv_info[pDecLocal->block_y_aff + j0][i0];
                if(currSlice->active_sps->direct_8x8_inference_flag)
			    {
      			  colocated_ref_list0 = list1[0]->bottom_field->ref_idx[0][col_indexR];
      			  colocated_ref_list1 = list1[0]->bottom_field->ref_idx[1][col_indexR];
      			  colocated_ref_pic0 = list1[0]->bottom_field->ref_pic[0][col_indexR];
      			  colocated_ref_pic1 = list1[0]->bottom_field->ref_pic[1][col_indexR];
      			}
      			else
      			{
      			  colocated_ref_list0 = list1[0]->bottom_field->ref_idx[0][col_index];
      			  colocated_ref_list1 = list1[0]->bottom_field->ref_idx[1][col_index];
      			  colocated_ref_pic0 = list1[0]->bottom_field->ref_pic[0][col_index];
      			  colocated_ref_pic1 = list1[0]->bottom_field->ref_pic[1][col_index];
      			}
              }
            }
#endif

            //refList = (colocated_ref_list0 == -1 ? LIST_1 : LIST_0);
            //ref_idx = colocated->ref_idx[refList];
			//ref_idx =  colocated_ref_list0 == -1? colocated_ref_list1: colocated_ref_list0;
			if(colocated_ref_list0 <= -1)
			{
			  refList = LIST_1;
			  ref_idx = colocated_ref_list1;
			  ref_pic = colocated_ref_pic1;
			}
			else
			{
			  refList = LIST_0;
			  ref_idx = colocated_ref_list0;
			  ref_pic = colocated_ref_pic0;
			}
            if (ref_idx <= -1)
            {
              for (j4 = j0; j4 <  j0 + step_v0; ++j4)
              {
                for (i4 = i0; i4 < i0 + step_h0; ++i4)
                {
                  //PicMotionParams *mv_info = &dec_picture->mv_info[j4][i4];
                  //mv_info->ref_pic[LIST_0] = list0[0];
                  //mv_info->ref_pic[LIST_1] = list1[0];
                  //mv_info->mv [LIST_0] = zero_mv;
                  //mv_info->mv [LIST_1] = zero_mv;
				  ref_pic_list0[(j4)*4+i4] = list0[0];
              	  ref_pic_list1[(j4)*4+i4] = list1[0];
              	  mv_cache0[(j4)*8+i4] = 0;
              	  mv_cache1[(j4)*8+i4] = 0;
                  //mv_info->ref_idx [LIST_0] = 0;
                  //mv_info->ref_idx [LIST_1] = 0;
				  ref_cache0[(j4)*8+i4] = 0;
				  ref_cache1[(j4)*8+i4] = 0;
                }
              }
            }
            else
            {
              if(ref_pic == NULL)
	          {
	            return;
	          }
              if( (currSlice->mb_aff_frame_flag && ( (pDecLocal->mb_field && ref_pic->structure==FRAME) || 
                (!pDecLocal->mb_field && ref_pic->structure!=FRAME))) ||
                (!currSlice->mb_aff_frame_flag && ((currSlice->field_pic_flag==0 && ref_pic->structure!=FRAME)||
                (currSlice->field_pic_flag==1 && ref_pic->structure==FRAME))) )
              {
                //! Frame with field co-located
                for (iref = 0; iref < imin(currSlice->num_ref_idx_active[LIST_0], currSlice->listXsize[LIST_0 + list_offset]); ++iref)
                {
                  if (currSlice->listX[LIST_0 + list_offset][iref]->top_field == ref_pic ||
                    currSlice->listX[LIST_0 + list_offset][iref]->bottom_field == ref_pic ||
                    currSlice->listX[LIST_0 + list_offset][iref]->frame == ref_pic ) 
                  {
                    mapped_idx=iref;
                    break;
                  }
                  else //! invalid index. Default to zero even though this case should not happen
                    mapped_idx=INVALIDINDEX;
                }
              }
              else
              {
                for (iref = 0; iref < imin(currSlice->num_ref_idx_active[LIST_0], currSlice->listXsize[LIST_0 + list_offset]); ++iref)
                {
                  if (currSlice->listX[LIST_0 + list_offset][iref] == ref_pic)
                  {
                    mapped_idx=iref;
                    break;
                  }
                  else //! invalid index. Default to zero even though this case should not happen
                    mapped_idx=INVALIDINDEX;
                }
              }

              if (mapped_idx != INVALIDINDEX)
              {
                for (j = j0; j < j0 + step_v0; ++j)
                {
                  j4 = j;
                  j6 = j;

                  for (i4 = i0; i4 < i0 + step_h0; ++i4)
                  {
                    //PicMotionParams *colocated = pDecGlobal->active_sps->direct_8x8_inference_flag ? 
                    //  &list1[0]->mv_info[RSD(j6)][RSD(i4)] : &list1[0]->mv_info[j6][i4];
                    int mv_y;
					int cmv_x,cmv_y;
                    int col4_indexR = block_xy+RSD(j)*4+RSD(i4);
			        int col4_index  = block_xy+j*4+i4;
					//PicMotionParams *colocated = pDecGlobal->active_sps->direct_8x8_inference_flag ? 
                    //  &list1[0]->mv_cache[pDecLocal->block_xy+RSD(j)*4+RSD(i4-pDecLocal->block_x)] : &list1[0]->mv_cache[pDecLocal->block_xy+j*4+i4-pDecLocal->block_x];
                    if(currSlice->active_sps->direct_8x8_inference_flag)
			        {
			          colocated_ref_pic0 = list1[0]->ref_pic[0][col4_indexR];
			          colocated_ref_pic1 = list1[0]->ref_pic[1][col4_indexR];
					  colocated_mv0 = (VO_S16*)&list1[0]->mv[0][col4_indexR];
					  colocated_mv1 = (VO_S16*)&list1[0]->mv[1][col4_indexR];
			        }
			        else
			        {
			          colocated_ref_pic0 = list1[0]->ref_pic[0][col4_index];
			          colocated_ref_pic1 = list1[0]->ref_pic[1][col4_index];
					  colocated_mv0 = (VO_S16*)&list1[0]->mv[0][col4_index];
					  colocated_mv1 = (VO_S16*)&list1[0]->mv[1][col4_index];
			        }
                    //PicMotionParams *mv_info = &dec_picture->mv_info[j4][i4];

#if 1
                    if(currSlice->mb_aff_frame_flag && (!currSlice->active_sps->frame_mbs_only_flag || currSlice->active_sps->direct_8x8_inference_flag))
                    {
					  if(!pDecLocal->mb_field && ((currSlice->listX[LIST_1][0]->iCodingType==FRAME_MB_PAIR_CODING && IS_INTERLACED(currSlice->listX[LIST_1][0]->mb_type_buffer[pDecLocal->mb_xy])) ||
						(currSlice->listX[LIST_1][0]->iCodingType==FIELD_CODING)))
					  {
						int mb_y = pDecLocal->mb.y;
						int mb_x = pDecLocal->mb.x;
						int block_xy = ((mb_y>>1)*pDecGlobal->PicWidthInMbs+mb_x)*16;
						col4_indexR = (block_xy)+(RSD(j)>>1)*4+RSD(i4);
						col4_index = (block_xy)+j*2+i4;
						col4_indexR += (pDecLocal->mb.y&1)*8;
			            col4_index += (pDecLocal->mb.y&1)*8;
                        if (iabs(pDecGlobal->dec_picture->poc - currSlice->listX[LIST_1+4][0]->poc)> iabs(pDecGlobal->dec_picture->poc -currSlice->listX[LIST_1+2][0]->poc) )
                        {
                          if(currSlice->active_sps->direct_8x8_inference_flag)
      			          {
            			    colocated_mv0 = (VO_S16*)&(currSlice->listX[LIST_1 + 2][0]->mv[0][col_indexR]);
                            colocated_mv1 = (VO_S16*)&(currSlice->listX[LIST_1 + 2][0]->mv[1][col_indexR]);
            	  	        colocated_ref_pic0 = currSlice->listX[LIST_1 + 2][0]->ref_pic[0][col_indexR];
            	  	        colocated_ref_pic1 = currSlice->listX[LIST_1 + 2][0]->ref_pic[1][col_indexR];
                          }
      				      else
      				      {
      				        colocated_mv0 = (VO_S16*)&(currSlice->listX[LIST_1 + 2][0]->mv[0][col_index]);
                            colocated_mv1 = (VO_S16*)&(currSlice->listX[LIST_1 + 2][0]->mv[1][col_index]);
            	  	        colocated_ref_pic0 = currSlice->listX[LIST_1 + 2][0]->ref_pic[0][col_index];
            	  	        colocated_ref_pic1 = currSlice->listX[LIST_1 + 2][0]->ref_pic[1][col_index];
      				      }
					    }
						else
						{
							if(currSlice->active_sps->direct_8x8_inference_flag)
							{
								colocated_mv0 = (VO_S16*)&(currSlice->listX[LIST_1 + 4][0]->mv[0][col_indexR]);
								colocated_mv1 = (VO_S16*)&(currSlice->listX[LIST_1 + 4][0]->mv[1][col_indexR]);
								colocated_ref_pic0 = currSlice->listX[LIST_1 + 4][0]->ref_pic[0][col_indexR];
								colocated_ref_pic1 = currSlice->listX[LIST_1 + 4][0]->ref_pic[1][col_indexR];
							}
							else
							{
								colocated_mv0 = (VO_S16*)&(currSlice->listX[LIST_1 + 4][0]->mv[0][col_index]);
								colocated_mv1 = (VO_S16*)&(currSlice->listX[LIST_1 + 4][0]->mv[1][col_index]);
								colocated_ref_pic0 = currSlice->listX[LIST_1 + 4][0]->ref_pic[0][col_index];
								colocated_ref_pic1 = currSlice->listX[LIST_1 + 4][0]->ref_pic[1][col_index];
							}
						}
                      }
                    }
                    else if(/*!currSlice->mb_aff_frame_flag &&*/ !currSlice->active_sps->frame_mbs_only_flag && !currSlice->field_pic_flag && currSlice->listX[LIST_1][0]->iCodingType!=FRAME_CODING)
                    {
                      int mb_y = pDecLocal->mb.y;
        			  int mb_x = pDecLocal->mb.x;
        			  int block_xy = ((mb_y>>1)*pDecGlobal->PicWidthInMbs+mb_x)*16;
                	  col4_indexR = (block_xy)+(RSD(j)>>1)*4+RSD(i4);
        			  col4_index = (block_xy)+j*2+i4;
					  col4_indexR += (pDecLocal->mb.y&1)*8;
					  col4_index += (pDecLocal->mb.y&1)*8;
                      if (iabs(dec_picture->poc - list1[0]->bottom_field->poc)> iabs(dec_picture->poc -list1[0]->top_field->poc) )
                      {
                        //colocated = pDecGlobal->active_sps->direct_8x8_inference_flag ? 
                        //  &list1[0]->top_field->mv_info[RSD(j6)>>1][RSD(i4)] : &list1[0]->top_field->mv_info[(j6)>>1][i4];
                        if(currSlice->active_sps->direct_8x8_inference_flag)
    			        {
    			          colocated_ref_pic0 = list1[0]->top_field->ref_pic[0][col4_indexR];
    			          colocated_ref_pic1 = list1[0]->top_field->ref_pic[1][col4_indexR];
    					  colocated_mv0 = (VO_S16*)&list1[0]->top_field->mv[0][col4_indexR];
    					  colocated_mv1 = (VO_S16*)&list1[0]->top_field->mv[1][col4_indexR];
    			        }
    			        else
    			        {
    			          colocated_ref_pic0 = list1[0]->top_field->ref_pic[0][col4_index];
    			          colocated_ref_pic1 = list1[0]->top_field->ref_pic[1][col4_index];
    					  colocated_mv0 = (VO_S16*)&list1[0]->top_field->mv[0][col4_index];
    					  colocated_mv1 = (VO_S16*)&list1[0]->top_field->mv[1][col4_index];
    			        }
                      }
                      else
                      {
                        //colocated = pDecGlobal->active_sps->direct_8x8_inference_flag ? 
                        //  &list1[0]->bottom_field->mv_info[RSD(j6)>>1][RSD(i4)] : &list1[0]->bottom_field->mv_info[(j6)>>1][i4];
						if(currSlice->active_sps->direct_8x8_inference_flag)
    			        {
    			          colocated_ref_pic0 = list1[0]->bottom_field->ref_pic[0][col4_indexR];
    			          colocated_ref_pic1 = list1[0]->bottom_field->ref_pic[1][col4_indexR];
    					  colocated_mv0 = (VO_S16*)&list1[0]->bottom_field->mv[0][col4_indexR];
    					  colocated_mv1 = (VO_S16*)&list1[0]->bottom_field->mv[1][col4_indexR];
    			        }
    			        else
    			        {
    			          colocated_ref_pic0 = list1[0]->bottom_field->ref_pic[0][col4_index];
    			          colocated_ref_pic1 = list1[0]->bottom_field->ref_pic[1][col4_index];
    					  colocated_mv0 = (VO_S16*)&list1[0]->bottom_field->mv[0][col4_index];
    					  colocated_mv1 = (VO_S16*)&list1[0]->bottom_field->mv[1][col4_index];
    			        }
                      }
                    }
                    else if(!currSlice->active_sps->frame_mbs_only_flag && currSlice->field_pic_flag && currSlice->structure!=list1[0]->structure && list1[0]->coded_frame)
                    {
                      col4_indexR += (pDecLocal->mb.y&1)*8;
			          col4_index += (pDecLocal->mb.y&1)*8;
                      if (currSlice->structure == TOP_FIELD)
                      {
                        //colocated = pDecGlobal->active_sps->direct_8x8_inference_flag ? 
                        //  &list1[0]->frame->top_field->mv_info[RSD(j6)][RSD(i4)] : &list1[0]->frame->top_field->mv_info[j6][i4];
                        if(currSlice->active_sps->direct_8x8_inference_flag)
    			        {
    			          colocated_ref_pic0 = list1[0]->top_field->ref_pic[0][col4_indexR];
    			          colocated_ref_pic1 = list1[0]->top_field->ref_pic[1][col4_indexR];
    					  colocated_mv0 = (VO_S16*)&list1[0]->top_field->mv[0][col4_indexR];
    					  colocated_mv1 = (VO_S16*)&list1[0]->top_field->mv[1][col4_indexR];
    			        }
    			        else
    			        {
    			          colocated_ref_pic0 = list1[0]->top_field->ref_pic[0][col4_index];
    			          colocated_ref_pic1 = list1[0]->top_field->ref_pic[1][col4_index];
    					  colocated_mv0 = (VO_S16*)&list1[0]->top_field->mv[0][col4_index];
    					  colocated_mv1 = (VO_S16*)&list1[0]->top_field->mv[1][col4_index];
    			        }
                      }
                      else
                      {
                        //colocated = pDecGlobal->active_sps->direct_8x8_inference_flag ? 
                        //  &list1[0]->frame->bottom_field->mv_info[RSD(j6)][RSD(i4)] : &list1[0]->frame->bottom_field->mv_info[j6][i4];
                        if(currSlice->active_sps->direct_8x8_inference_flag)
    			        {
    			          colocated_ref_pic0 = list1[0]->bottom_field->ref_pic[0][col4_indexR];
    			          colocated_ref_pic1 = list1[0]->bottom_field->ref_pic[1][col4_indexR];
    					  colocated_mv0 = (VO_S16*)&list1[0]->bottom_field->mv[0][col4_indexR];
    					  colocated_mv1 = (VO_S16*)&list1[0]->bottom_field->mv[1][col4_indexR];
    			        }
    			        else
    			        {
    			          colocated_ref_pic0 = list1[0]->bottom_field->ref_pic[0][col4_index];
    			          colocated_ref_pic1 = list1[0]->bottom_field->ref_pic[1][col4_index];
    					  colocated_mv0 = (VO_S16*)&list1[0]->bottom_field->mv[0][col4_index];
    					  colocated_mv1 = (VO_S16*)&list1[0]->bottom_field->mv[1][col4_index];
    			        }
                      }
                    }
#endif 
					if(colocated_ref_list0 <= -1)
			        {
			          ref_pic = colocated_ref_pic1;
					  cmv_x = colocated_mv1[0];
					  cmv_y = colocated_mv1[1];
			        }
			        else
			        {
			          ref_pic = colocated_ref_pic0;
					  cmv_x = colocated_mv0[0];
					  cmv_y = colocated_mv0[1];
			        }
					if(ref_pic == NULL)
	                {
	                  return;
	                }
                    mv_y = cmv_y; 
                    if((currSlice->mb_aff_frame_flag && !pDecLocal->mb_field && ref_pic->structure!=FRAME) ||
                      (!currSlice->mb_aff_frame_flag && currSlice->field_pic_flag==0 && ref_pic->structure!=FRAME))
                      mv_y *= 2;
                    else if((currSlice->mb_aff_frame_flag && pDecLocal->mb_field && ref_pic->structure==FRAME) ||
                      (!currSlice->mb_aff_frame_flag && currSlice->field_pic_flag==1 && ref_pic->structure==FRAME))
                      mv_y /= 2;

                    mv_scale = currSlice->mvscale[LIST_0 + list_offset][mapped_idx];

					ref_cache0[j*8+i4] = mapped_idx;
				    ref_cache1[j*8+i4] = 0;

                    //mv_info->ref_pic[LIST_0] = list0[mapped_idx];
                    //mv_info->ref_pic[LIST_1] = list1[0];
					ref_pic_list0[j*4+i4] = list0[mapped_idx];
              		ref_pic_list1[j*4+i4] = list1[0];
              

                    if (mv_scale == 9999 || currSlice->listX[LIST_0+list_offset][mapped_idx]->is_long_term)
                    {
                      //mv_info->mv[LIST_0].mv_x = colocated->mv[refList].mv_x;
                      //mv_info->mv[LIST_0].mv_y = (short) mv_y;
                      //mv_info->mv[LIST_1] = zero_mv;
					  pmvl0[0] = cmv_x;
					  pmvl0[1] = (short) mv_y;
					  mv_cache0[j*8+i4] = M32(pmvl0);
              		  mv_cache1[j*8+i4] = 0;
                    }
                    else
                    {
                      //mv_info->mv[LIST_0].mv_x = (short) ((mv_scale * colocated->mv[refList].mv_x + 128 ) >> 8);
                      //mv_info->mv[LIST_0].mv_y = (short) ((mv_scale * mv_y/*colocated->mv[refList].mv_y*/ + 128 ) >> 8);
                      //mv_info->mv[LIST_1].mv_x = (short) (mv_info->mv[LIST_0].mv_x - colocated->mv[refList].mv_x);
                      //mv_info->mv[LIST_1].mv_y = (short) (mv_info->mv[LIST_0].mv_y - mv_y/*colocated->mv[refList].mv_y*/);
					  pmvl0[0] = (short) ((mv_scale * cmv_x + 128 ) >> 8);
					  pmvl0[1] = (short) ((mv_scale * mv_y/*colocated->mv[refList].mv_y*/ + 128 ) >> 8);
              		  pmvl1[0] = (short) (pmvl0[0] - cmv_x);
					  pmvl1[1] = (short) (pmvl0[1] - mv_y/*colocated->mv[refList].mv_y*/);
					  mv_cache0[j*8+i4] = M32(pmvl0);
              		  mv_cache1[j*8+i4] = M32(pmvl1);
                    }
                  }
                }
              }
              else if (INVALIDINDEX == mapped_idx)
              {
                //error("temporal direct error: colocated block has ref that is unavailable",-1111);
                for (j4 = j0; j4 <  j0 + step_v0; ++j4)
                {
                  for (i4 = i0; i4 < i0 + step_h0; ++i4)
                  {
				    ref_pic_list0[(j4)*4+i4] = list0[0];
              	    ref_pic_list1[(j4)*4+i4] = list1[0];
              	    mv_cache0[(j4)*8+i4] = 0;
              	    mv_cache1[(j4)*8+i4] = 0;
				    ref_cache0[(j4)*8+i4] = 0;
				    ref_cache1[(j4)*8+i4] = 0;
                  }
                }
			  }
            }
          }
        }
      }
    }    
  }
}
#else
static const byte decode_block_scan[16] = {0, 1, 4, 5, 2, 3, 6, 7, 8, 9, 12, 13, 10, 11, 14, 15};

VO_S32 update_direct_mv_info_temporal(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32* mb_type)
{
  short ref_idx;
  int refList;
  StorablePicture* ref_pic;
  int i, j;
  int block8x8;   // needed for ABT
  int stride_4x4 = 4;
  int stride_8x8 = 4;
  Slice *currSlice = pDecLocal->p_Slice;
  //int list_offset = (pDecLocal->mb_field&&!currSlice->field_pic_flag)? (pDecLocal->mb.y&1)?4:2: 0;
  StorablePicture **list0 = currSlice->listX[currSlice->slice_number][LIST_0];
  StorablePicture **list1 = currSlice->listX[currSlice->slice_number][LIST_1];
  StorablePicture** ref_pic_list0 = pDecGlobal->dec_picture->ref_pic[0]+pDecLocal->mb_xy*16;
  StorablePicture** ref_pic_list1 = pDecGlobal->dec_picture->ref_pic[1]+pDecLocal->mb_xy*16;
  VO_S32* mv_cache0 = &pDecLocal->mv_cache[0][12];
  VO_S32* mv_cache1 = &pDecLocal->mv_cache[1][12];
  VO_S8* ref_cache0 = &pDecLocal->ref_cache[0][12];
  VO_S8* ref_cache1 = &pDecLocal->ref_cache[1][12];
  VO_S8 colocated_ref_list0,colocated_ref_list1;
  StorablePicture* colocated_ref_pic0,*colocated_ref_pic1;
  VO_S16 pmvl0[2], pmvl1[2];
  VO_S16* colocated_mv0,*colocated_mv1;
  VO_S32 colocated_type[2];
  VO_S32 is_8x8 = IS_8X8(*mb_type);
  VO_U32 sub_type;
  int cmv_x,cmv_y;
  VO_S32 mv0,mv1;
  VO_S32 ref0,ref1;
  VO_S32 block_xy = pDecLocal->mb_xy*16;
  VO_S32 mb_xy = pDecLocal->mb_xy;
  VO_S32 index_shift = 0;
  if(list0[0] == NULL||list1[0] == NULL)
  {
    return 0;
  }
#if USE_FRAME_THREAD
	if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
	{
		//wait for list1[0],mv=(?,?) progress ready
		FrameThdInfo *pInfo = pDecGlobal->pThdInfo;
		const VO_S32 max_y = pDecLocal->mb.y*MB_BLOCK_SIZE + MB_BLOCK_SIZE-1; 	  //???interlace need fix
        //const VO_S32 max_y = 10000;
		VO_S32 errFlag = WaitForProgress(pInfo, list1[0], max_y, 0);
		
		if(errFlag)
		{
			//ref is error
			pInfo->errFlag = errFlag;
			return errFlag;
		}
	}
#endif

  
                                      
      colocated_type[0] = colocated_type[1] = list1[0]->mb_type_buffer[mb_xy];

      sub_type = VO_16x16|VO_P0L0|VO_P0L1|VO_DIRECT; 
      if(!is_8x8 && (colocated_type[0] & VO_INTRA_OR_16X16))
	  {
         *mb_type   |= VO_16x16|VO_P0L0|VO_P0L1|VO_DIRECT;
      }
	  else if(!is_8x8 && (colocated_type[0] & (VO_16x8|VO_8x16)))
	  {
         *mb_type   |= VO_L0L1|VO_DIRECT | (colocated_type[0] & (VO_16x8|VO_8x16));
      }
	  else
	  {
         if(!currSlice->active_sps->direct_8x8_inference_flag)
		 {
           sub_type = VO_8x8|VO_P0L0|VO_P0L1|VO_DIRECT; 
         }
         *mb_type   |= VO_8x8|VO_L0L1;
      }
  
  
  if(IS_16X16(*mb_type))
  {
    
	M32(&ref_cache1[0]) = M32(&ref_cache1[8]) = M32(&ref_cache1[16]) = M32(&ref_cache1[24]) = 0;
	//ref_pic_list1[0] = ref_pic_list1[1] = ref_pic_list1[2] = ref_pic_list1[3] = 
	//ref_pic_list1[4] = ref_pic_list1[5] = ref_pic_list1[6] = ref_pic_list1[7] = 
	//ref_pic_list1[8] = ref_pic_list1[9] = ref_pic_list1[10] = ref_pic_list1[11] = 
	//ref_pic_list1[12] = ref_pic_list1[13] = ref_pic_list1[14] = ref_pic_list1[15] = list1[0];

	if(IS_INTRA0(colocated_type[0]))
	{
      ref0=mv0=mv1=0;
    }
	else
	{
	  VO_S32 mapped_idx=0;
      VO_S32 iref;
	  VO_S32 col_indexR = mb_xy*16;	  
	  colocated_ref_list0 = list1[0]->ref_idx[0][col_indexR];
	  colocated_ref_list1 = list1[0]->ref_idx[1][col_indexR];
	  //colocated_ref_pic0 = list1[0]->ref_pic[0][col_indexR];
	  //colocated_ref_pic1 = list1[0]->ref_pic[1][col_indexR];
	  colocated_ref_pic0 = list1[0]->listX[currSlice->slice_number][0][colocated_ref_list0];
	  colocated_ref_pic1 = list1[0]->listX[currSlice->slice_number][1][colocated_ref_list1];
	  colocated_mv0 = (VO_S16*)&list1[0]->mv[0][col_indexR];
	  colocated_mv1 = (VO_S16*)&list1[0]->mv[1][col_indexR];
	  if(colocated_ref_list0 <= -1)
	  {
		ref_pic = colocated_ref_pic1;
		cmv_x = colocated_mv1[0];
		cmv_y = colocated_mv1[1];
	  }
	  else
	  {
		ref_pic = colocated_ref_pic0;
		cmv_x = colocated_mv0[0];
		cmv_y = colocated_mv0[1];		
	  }
	  if(ref_pic == NULL)
	  {
	    return 0;
	  }
	  for (iref = 0; iref < imin(currSlice->num_ref_idx_active[LIST_0], currSlice->listXsize[currSlice->slice_number][LIST_0]);iref++)
      {
        if(currSlice->listX[currSlice->slice_number][LIST_0][iref] == ref_pic)
        {
          mapped_idx = iref;            
          break;
        }
        else //! invalid index. Default to zero even though this case should not happen
        {
          mapped_idx=INVALIDINDEX;
        }
      }
	  if(INVALIDINDEX == mapped_idx)
      {
        mv0 = 0;
        mv1 = 0;
		ref0 = 0;
      }
      else
      {
        int mv_scale = currSlice->mvscale[LIST_0][mapped_idx];

        if (mv_scale == 9999 || currSlice->listX[currSlice->slice_number][LIST_0][mapped_idx]->is_long_term)
        {
	      pmvl0[0] = cmv_x;
		  pmvl0[1] = cmv_y;
		  mv0 = M32(pmvl0);
          mv1 = 0;
        }
        else
        {
		  pmvl0[0] = (short) ((mv_scale * cmv_x + 128 ) >> 8);
		  pmvl0[1] = (short) ((mv_scale * cmv_y + 128 ) >> 8);
          pmvl1[0] = (short) (pmvl0[0] - cmv_x);
	      pmvl1[1] = (short) (pmvl0[1] - cmv_y);
		  mv0 = M32(pmvl0);
          mv1 = M32(pmvl1);
        }

		ref0 = (char) mapped_idx;
	  }
	}
	M32(&ref_cache0[0]) = M32(&ref_cache0[8]) = M32(&ref_cache0[16]) = M32(&ref_cache0[24]) = ref0*0x01010101;
	//ref_pic_list0[0] = ref_pic_list0[1] = ref_pic_list0[2] = ref_pic_list0[3] = 
	//ref_pic_list0[4] = ref_pic_list0[5] = ref_pic_list0[6] = ref_pic_list0[7] = 
	//ref_pic_list0[8] = ref_pic_list0[9] = ref_pic_list0[10] = ref_pic_list0[11] = 
	//ref_pic_list0[12] = ref_pic_list0[13] = ref_pic_list0[14] = ref_pic_list0[15] = list0[ref0];
	mv_cache0[0] = mv_cache0[1] = mv_cache0[2] = mv_cache0[3] = 	 
	mv_cache0[8] = mv_cache0[9] = mv_cache0[10] = mv_cache0[11] = 
	mv_cache0[16] = mv_cache0[17] = mv_cache0[18] = mv_cache0[19] =
	mv_cache0[24] = mv_cache0[25] = mv_cache0[26] = mv_cache0[27] = mv0;
	mv_cache1[0] = mv_cache1[1] = mv_cache1[2] = mv_cache1[3] = 	 
	mv_cache1[8] = mv_cache1[9] = mv_cache1[10] = mv_cache1[11] = 
	mv_cache1[16] = mv_cache1[17] = mv_cache1[18] = mv_cache1[19] =
	mv_cache1[24] = mv_cache1[25] = mv_cache1[26] = mv_cache1[27] = mv1;
  }
  else
  {
	block_xy = mb_xy*16;
    for (block8x8=0; block8x8<4; block8x8++)
    {      
      int k = (block8x8 << 2);
      int col_indexR;
	  VO_S32 mapped_idx=0;
      VO_S32 iref;
	  VO_S32 mv_scale;
	  VO_S16* colocated_mv;
      i =  (decode_block_scan[k] & 3);
      j = ((decode_block_scan[k] >> 2) & 3);
	  if(is_8x8 && !IS_DIRECT_VO(pDecLocal->b8type[block8x8]))
        continue;
      pDecLocal->b8type[block8x8] = sub_type;
	  ref_cache1[j*8+i]=ref_cache1[j*8+i+1]=ref_cache1[(j+1)*8+i]=ref_cache1[(j+1)*8+i+1]=0;
	  //ref_pic_list1[j*4+i]=ref_pic_list1[j*4+i+1]=ref_pic_list1[(j+1)*4+i]=ref_pic_list1[(j+1)*4+i+1]= list1[0];
	  if(IS_INTRA0(colocated_type[0]))
	  {
        //ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= list0[0];
	    mv_cache0[j*8+i]=mv_cache0[j*8+i+1]=mv_cache0[(j+1)*8+i]=mv_cache0[(j+1)*8+i+1]=0;
	    mv_cache1[j*8+i]=mv_cache1[j*8+i+1]=mv_cache1[(j+1)*8+i]=mv_cache1[(j+1)*8+i+1]=0;
	    ref_cache0[j*8+i]=ref_cache0[j*8+i+1]=ref_cache0[(j+1)*8+i]=ref_cache0[(j+1)*8+i+1]=0;
		continue;
      }
	  col_indexR = currSlice->active_sps->direct_8x8_inference_flag?block_xy+RSD(j)*4+RSD(i):block_xy+j*4+i;
	  colocated_ref_list0 = list1[0]->ref_idx[0][col_indexR];
	  colocated_ref_list1 = list1[0]->ref_idx[1][col_indexR];
	  //colocated_ref_pic0 = list1[0]->ref_pic[0][col_indexR];
	  //colocated_ref_pic1 = list1[0]->ref_pic[1][col_indexR];
	  colocated_ref_pic0 = list1[0]->listX[currSlice->slice_number][0][colocated_ref_list0];
	  colocated_ref_pic1 = list1[0]->listX[currSlice->slice_number][1][colocated_ref_list1];
	  colocated_mv0 = (VO_S16*)&list1[0]->mv[0][col_indexR];
	  colocated_mv1 = (VO_S16*)&list1[0]->mv[1][col_indexR];
	  if(colocated_ref_list0 <= -1)
	  {
		ref_pic = colocated_ref_pic1;
		colocated_mv = colocated_mv1;
	  }
	  else
	  {
		ref_pic = colocated_ref_pic0;
		colocated_mv = colocated_mv0;		
	  }
	  if(ref_pic == NULL)
	  {
	    return 0;
	  }
	  for (iref = 0; iref < imin(currSlice->num_ref_idx_active[LIST_0], currSlice->listXsize[currSlice->slice_number][LIST_0]);iref++)
      {
        if(currSlice->listX[currSlice->slice_number][LIST_0][iref] == ref_pic)
        {
          mapped_idx = iref;            
          break;
        }
        else //! invalid index. Default to zero even though this case should not happen
        {
          mapped_idx=INVALIDINDEX;
        }
      }
	  if(INVALIDINDEX == mapped_idx)
      {
        //ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= list0[0];
	    mv_cache0[j*8+i]=mv_cache0[j*8+i+1]=mv_cache0[(j+1)*8+i]=mv_cache0[(j+1)*8+i+1]=0;
	    mv_cache1[j*8+i]=mv_cache1[j*8+i+1]=mv_cache1[(j+1)*8+i]=mv_cache1[(j+1)*8+i+1]=0;
	    ref_cache0[j*8+i]=ref_cache0[j*8+i+1]=ref_cache0[(j+1)*8+i]=ref_cache0[(j+1)*8+i+1]=0;
		continue;
	  }
	  mv_scale = currSlice->mvscale[LIST_0][mapped_idx];
	  ref0 = (char) mapped_idx;
	  ref_cache0[j*8+i]=ref_cache0[j*8+i+1]=ref_cache0[(j+1)*8+i]=ref_cache0[(j+1)*8+i+1]=ref0;
	  //ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= list0[ref0];
	  if(IS_SUB_8X8(sub_type))
	  {
	    cmv_x = colocated_mv[0];
		cmv_y = colocated_mv[1];
	    if (mv_scale == 9999 || currSlice->listX[currSlice->slice_number][LIST_0][mapped_idx]->is_long_term)
        {
	      pmvl0[0] = cmv_x;
		  pmvl0[1] = cmv_y;
		  mv0 = M32(pmvl0);
          mv1 = 0;
        }
        else
        {
		  pmvl0[0] = (short) ((mv_scale * cmv_x + 128 ) >> 8);
		  pmvl0[1] = (short) ((mv_scale * cmv_y + 128 ) >> 8);
          pmvl1[0] = (short) (pmvl0[0] - cmv_x);
	      pmvl1[1] = (short) (pmvl0[1] - cmv_y);
		  mv0 = M32(pmvl0);
          mv1 = M32(pmvl1);
        }
		mv_cache0[j*8+i]=mv_cache0[j*8+i+1]=mv_cache0[(j+1)*8+i]=mv_cache0[(j+1)*8+i+1]=mv0;
	    mv_cache1[j*8+i]=mv_cache1[j*8+i+1]=mv_cache1[(j+1)*8+i]=mv_cache1[(j+1)*8+i+1]=mv1;
	  }
	  else
	  {
	    VO_S32 block4x4,index;
		for(block4x4=0; block4x4<4; block4x4++)
		{
		  index = (block4x4&2)*4+(block4x4&1);
		  cmv_x = colocated_mv[index+(block4x4&1)+0];
		  cmv_y = colocated_mv[index+(block4x4&1)+1];
		  if (mv_scale == 9999 || currSlice->listX[currSlice->slice_number][LIST_0][mapped_idx]->is_long_term)
          {
	        pmvl0[0] = cmv_x;
		    pmvl0[1] = cmv_y;
		    mv0 = M32(pmvl0);
            mv1 = 0;
          }
          else
          {
		    pmvl0[0] = (short) ((mv_scale * cmv_x + 128 ) >> 8);
		    pmvl0[1] = (short) ((mv_scale * cmv_y + 128 ) >> 8);
            pmvl1[0] = (short) (pmvl0[0] - cmv_x);
	        pmvl1[1] = (short) (pmvl0[1] - cmv_y);
		    mv0 = M32(pmvl0);
            mv1 = M32(pmvl1);
          }
		  mv_cache0[j*8+i+index]=mv0;
		  mv_cache1[j*8+i+index]=mv1;
		}
	  }
    }
  }
  return 1;
}


VO_S32 update_direct_mv_info_temporal_interlace(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32* mb_type)
{
	short ref_idx;
	int refList;
	StorablePicture* ref_pic;
	int i, j;
	int block8x8;   // needed for ABT
	int stride_4x4 = 4;
	int stride_8x8 = 4;
	Slice *currSlice = pDecLocal->p_Slice;
	int list_offset = (pDecLocal->mb_field&&!currSlice->field_pic_flag)? (pDecLocal->mb.y&1)?4:2: 0;
	StorablePicture **list0 = currSlice->listX[currSlice->slice_number][LIST_0 + list_offset];
	StorablePicture **list1 = currSlice->listX[currSlice->slice_number][LIST_1 + list_offset];
	StorablePicture** ref_pic_list0 = pDecGlobal->dec_picture->ref_pic[0]+pDecLocal->mb_xy*16;
	StorablePicture** ref_pic_list1 = pDecGlobal->dec_picture->ref_pic[1]+pDecLocal->mb_xy*16;
	VO_S32* mv_cache0 = &pDecLocal->mv_cache[0][12];
	VO_S32* mv_cache1 = &pDecLocal->mv_cache[1][12];
	VO_S8* ref_cache0 = &pDecLocal->ref_cache[0][12];
	VO_S8* ref_cache1 = &pDecLocal->ref_cache[1][12];
	VO_S8 colocated_ref_list0,colocated_ref_list1;
	StorablePicture* colocated_ref_pic0,*colocated_ref_pic1;
	VO_S16 pmvl0[2], pmvl1[2];
	VO_S16* colocated_mv0,*colocated_mv1;
	VO_S32 colocated_type[2];
	VO_S32 is_8x8 = IS_8X8(*mb_type);
	VO_U32 sub_type;
	int cmv_x,cmv_y;
	VO_S32 mv0,mv1;
	VO_S32 ref0,ref1;
	VO_S32 block_xy = pDecLocal->mb_xy*16;
	VO_S32 mb_xy = pDecLocal->mb_xy;
	VO_S32 index_shift = 0;
	if(list0[0] == NULL||list1[0] == NULL)
	{
		return 0;
	}
#if USE_FRAME_THREAD
	if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
	{
		//wait for list1[0],mv=(?,?) progress ready
		FrameThdInfo *pInfo = pDecGlobal->pThdInfo;
		const VO_S32 max_y = pDecLocal->mb.y*MB_BLOCK_SIZE + MB_BLOCK_SIZE-1+MB_BLOCK_SIZE*(!!IS_INTERLACED(*mb_type)); 	  //???interlace need fix
		//const VO_S32 max_y = 10000;
		VO_S32 errFlag = WaitForProgress(pInfo, list1[0], max_y, 0);

		if(errFlag)
		{
			//ref is error
			pInfo->errFlag = errFlag;
			return errFlag;
		}
	}
#endif

	if(IS_INTERLACED(list1[0]->mb_type_buffer[mb_xy]))
	{ 
		if(!IS_INTERLACED(*mb_type))
		{                   
			mb_xy= pDecLocal->mb.x + ((pDecLocal->mb.y&~1) + pDecGlobal->mbaff_dis)*pDecGlobal->PicWidthInMbs;
			stride_8x8 = 0;
		}
		else
		{
			mb_xy += pDecGlobal->field_offset; 
		}
		colocated_type[0] = colocated_type[1] = list1[0]->mb_type_buffer[mb_xy];

		sub_type = VO_16x16|VO_P0L0|VO_P0L1|VO_DIRECT; 
		if(!is_8x8 && (colocated_type[0] & VO_INTRA_OR_16X16))
		{
			*mb_type   |= VO_16x16|VO_P0L0|VO_P0L1|VO_DIRECT;
		}
		else if(!is_8x8 && (colocated_type[0] & (VO_16x8|VO_8x16)))
		{
			*mb_type   |= VO_L0L1|VO_DIRECT | (colocated_type[0] & (VO_16x8|VO_8x16));
		}
		else
		{
			if(!currSlice->active_sps->direct_8x8_inference_flag)
			{
				sub_type = VO_8x8|VO_P0L0|VO_P0L1|VO_DIRECT; 
			}
			*mb_type   |= VO_8x8|VO_L0L1;
		}
	}
	else
	{                                              
		if(IS_INTERLACED(*mb_type))
		{                    
			mb_xy= pDecLocal->mb.x + (pDecLocal->mb.y&~1)*pDecGlobal->PicWidthInMbs;
			colocated_type[0] = list1[0]->mb_type_buffer[mb_xy];
			colocated_type[1] = list1[0]->mb_type_buffer[mb_xy+pDecGlobal->PicWidthInMbs];
			stride_8x8 = 2+4*4;
			stride_4x4 = pDecGlobal->PicWidthInMbs*16+8;

			sub_type = VO_16x16|VO_P0L0|VO_P0L1|VO_DIRECT; 

			if((colocated_type[0] & VO_INTRA_OR_16X16)
				&& (colocated_type[1] & VO_INTRA_OR_16X16)&& !is_8x8)
			{
				*mb_type   |= VO_16x8 |VO_L0L1|VO_DIRECT;
			}
			else
			{
				*mb_type   |= VO_8x8|VO_L0L1;
			}
		}
		else
		{                                          
			colocated_type[0] = colocated_type[1] = list1[0]->mb_type_buffer[mb_xy];

			sub_type = VO_16x16|VO_P0L0|VO_P0L1|VO_DIRECT; 
			if(!is_8x8 && (colocated_type[0] & VO_INTRA_OR_16X16))
			{
				*mb_type   |= VO_16x16|VO_P0L0|VO_P0L1|VO_DIRECT;
			}
			else if(!is_8x8 && (colocated_type[0] & (VO_16x8|VO_8x16)))
			{
				*mb_type   |= VO_L0L1|VO_DIRECT | (colocated_type[0] & (VO_16x8|VO_8x16));
			}
			else
			{
				if(!currSlice->active_sps->direct_8x8_inference_flag)
				{
					sub_type = VO_8x8|VO_P0L0|VO_P0L1|VO_DIRECT; 
				}
				*mb_type   |= VO_8x8|VO_L0L1;
			}
		}
	}

	if(!stride_8x8)
	{
		if(pDecLocal->mb.y&1)
			index_shift = 2*stride_4x4;
	}

	if(IS_INTERLACED(*mb_type) != IS_INTERLACED(colocated_type[0]))
	{
		VO_S32 shift_y  = 2*!IS_INTERLACED(*mb_type);
		block_xy = mb_xy*16;
		for(block8x8=0; block8x8<4; block8x8++)
		{
			const int x8 = block8x8&1;
			const int y8 = block8x8>>1;
			int k = (block8x8 << 2);
			int col_indexR;
			VO_S32 mapped_idx=0;
			VO_S32 iref;
			VO_S32 mv_scale;
			VO_S16* colocated_mv;
			i =  (decode_block_scan[k] & 3);
			j = ((decode_block_scan[k] >> 2) & 3);
			if(is_8x8 && !IS_DIRECT_VO(pDecLocal->b8type[block8x8]))
				continue;
			pDecLocal->b8type[block8x8] = sub_type;
			ref_cache1[j*8+i]=ref_cache1[j*8+i+1]=ref_cache1[(j+1)*8+i]=ref_cache1[(j+1)*8+i+1]=0;
			ref_pic_list1[j*4+i]=ref_pic_list1[j*4+i+1]=ref_pic_list1[(j+1)*4+i]=ref_pic_list1[(j+1)*4+i+1]= list1[0];
			if(IS_INTRA0(colocated_type[y8]))
			{
				ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= list0[0];
				mv_cache0[j*8+i]=mv_cache0[j*8+i+1]=mv_cache0[(j+1)*8+i]=mv_cache0[(j+1)*8+i+1]=0;
				mv_cache1[j*8+i]=mv_cache1[j*8+i+1]=mv_cache1[(j+1)*8+i]=mv_cache1[(j+1)*8+i+1]=0;
				ref_cache0[j*8+i]=ref_cache0[j*8+i+1]=ref_cache0[(j+1)*8+i]=ref_cache0[(j+1)*8+i+1]=0;
				continue;
			}
			col_indexR = block_xy+x8*3+y8*stride_4x4+index_shift;
			colocated_ref_list0 = list1[0]->ref_idx[0][col_indexR];
			colocated_ref_list1 = list1[0]->ref_idx[1][col_indexR];
			colocated_ref_pic0 = list1[0]->ref_pic[0][col_indexR];
			colocated_ref_pic1 = list1[0]->ref_pic[1][col_indexR];
			//colocated_ref_pic0 = list1[0]->listX[currSlice->slice_number][0 + list_offset][colocated_ref_list0];
			//colocated_ref_pic1 = list1[0]->listX[currSlice->slice_number][1 + list_offset][colocated_ref_list1];
			colocated_mv0 = (VO_S16*)&list1[0]->mv[0][col_indexR];
			colocated_mv1 = (VO_S16*)&list1[0]->mv[1][col_indexR];
			if(colocated_ref_list0 <= -1)
			{
				ref_pic = colocated_ref_pic1;
				colocated_mv = colocated_mv1;
			}
			else
			{
				ref_pic = colocated_ref_pic0;
				colocated_mv = colocated_mv0;		
			}
			if(ref_pic == NULL)
			{
				return 0;
			}
			for (iref = 0; iref < imin(currSlice->num_ref_idx_active[LIST_0], currSlice->listXsize[currSlice->slice_number][LIST_0 + list_offset]);iref++)
			{
				if(currSlice->listX[currSlice->slice_number][LIST_0 + list_offset][iref]->top_field == ref_pic || 
					currSlice->listX[currSlice->slice_number][LIST_0 + list_offset][iref]->bottom_field == ref_pic ||
					currSlice->listX[currSlice->slice_number][LIST_0 + list_offset][iref]->frame == ref_pic)
				{
					mapped_idx = iref;            
					break;
				}
				else //! invalid index. Default to zero even though this case should not happen
				{
					mapped_idx=INVALIDINDEX;
				}
			}
			if(INVALIDINDEX == mapped_idx)
			{
				ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= list0[0];
				mv_cache0[j*8+i]=mv_cache0[j*8+i+1]=mv_cache0[(j+1)*8+i]=mv_cache0[(j+1)*8+i+1]=0;
				mv_cache1[j*8+i]=mv_cache1[j*8+i+1]=mv_cache1[(j+1)*8+i]=mv_cache1[(j+1)*8+i+1]=0;
				ref_cache0[j*8+i]=ref_cache0[j*8+i+1]=ref_cache0[(j+1)*8+i]=ref_cache0[(j+1)*8+i+1]=0;
				continue;
			}
			mv_scale = currSlice->mvscale[LIST_0 + list_offset][mapped_idx];
			ref0 = (char) mapped_idx;
			ref_cache0[j*8+i]=ref_cache0[j*8+i+1]=ref_cache0[(j+1)*8+i]=ref_cache0[(j+1)*8+i+1]=ref0;
			ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= list0[ref0];

			cmv_x = colocated_mv[0];
			cmv_y = (colocated_mv[1]<<shift_y)/2;
			if (mv_scale == 9999 || currSlice->listX[currSlice->slice_number][LIST_0 + list_offset][mapped_idx]->is_long_term)
			{
				pmvl0[0] = cmv_x;
				pmvl0[1] = cmv_y;
				mv0 = M32(pmvl0);
				mv1 = 0;
			}
			else
			{
				pmvl0[0] = (short) ((mv_scale * cmv_x + 128 ) >> 8);
				pmvl0[1] = (short) ((mv_scale * cmv_y + 128 ) >> 8);
				pmvl1[0] = (short) (pmvl0[0] - cmv_x);
				pmvl1[1] = (short) (pmvl0[1] - cmv_y);
				mv0 = M32(pmvl0);
				mv1 = M32(pmvl1);
			}
			mv_cache0[j*8+i]=mv_cache0[j*8+i+1]=mv_cache0[(j+1)*8+i]=mv_cache0[(j+1)*8+i+1]=mv0;
			mv_cache1[j*8+i]=mv_cache1[j*8+i+1]=mv_cache1[(j+1)*8+i]=mv_cache1[(j+1)*8+i+1]=mv1;
		}
		return 1;
	}

	if(IS_16X16(*mb_type))
	{

		M32(&ref_cache1[0]) = M32(&ref_cache1[8]) = M32(&ref_cache1[16]) = M32(&ref_cache1[24]) = 0;
		ref_pic_list1[0] = ref_pic_list1[1] = ref_pic_list1[2] = ref_pic_list1[3] = 
			ref_pic_list1[4] = ref_pic_list1[5] = ref_pic_list1[6] = ref_pic_list1[7] = 
			ref_pic_list1[8] = ref_pic_list1[9] = ref_pic_list1[10] = ref_pic_list1[11] = 
			ref_pic_list1[12] = ref_pic_list1[13] = ref_pic_list1[14] = ref_pic_list1[15] = list1[0];

		if(IS_INTRA0(colocated_type[0]))
		{
			ref0=mv0=mv1=0;
		}
		else
		{
			VO_S32 mapped_idx=0;
			VO_S32 iref;
			VO_S32 col_indexR = mb_xy*16;	  
			colocated_ref_list0 = list1[0]->ref_idx[0][col_indexR];
			colocated_ref_list1 = list1[0]->ref_idx[1][col_indexR];
			colocated_ref_pic0 = list1[0]->ref_pic[0][col_indexR];
			colocated_ref_pic1 = list1[0]->ref_pic[1][col_indexR];
			//colocated_ref_pic0 = list1[0]->listX[currSlice->slice_number][0 + list_offset][colocated_ref_list0];
			//colocated_ref_pic1 = list1[0]->listX[currSlice->slice_number][1 + list_offset][colocated_ref_list1];
			colocated_mv0 = (VO_S16*)&list1[0]->mv[0][col_indexR];
			colocated_mv1 = (VO_S16*)&list1[0]->mv[1][col_indexR];
			if(colocated_ref_list0 <= -1)
			{
				ref_pic = colocated_ref_pic1;
				cmv_x = colocated_mv1[0];
				cmv_y = colocated_mv1[1];
			}
			else
			{
				ref_pic = colocated_ref_pic0;
				cmv_x = colocated_mv0[0];
				cmv_y = colocated_mv0[1];		
			}
			if(ref_pic == NULL)
			{
				return 0;
			}
			for (iref = 0; iref < imin(currSlice->num_ref_idx_active[LIST_0], currSlice->listXsize[currSlice->slice_number][LIST_0 + list_offset]);iref++)
			{
				if(currSlice->listX[currSlice->slice_number][LIST_0 + list_offset][iref] == ref_pic)
				{
					mapped_idx = iref;            
					break;
				}
				else //! invalid index. Default to zero even though this case should not happen
				{
					mapped_idx=INVALIDINDEX;
				}
			}
			if(INVALIDINDEX == mapped_idx)
			{
				mv0 = 0;
				mv1 = 0;
				ref0 = 0;
			}
			else
			{
				int mv_scale = currSlice->mvscale[LIST_0 + list_offset][mapped_idx];

				if (mv_scale == 9999 || currSlice->listX[currSlice->slice_number][LIST_0 + list_offset][mapped_idx]->is_long_term)
				{
					pmvl0[0] = cmv_x;
					pmvl0[1] = cmv_y;
					mv0 = M32(pmvl0);
					mv1 = 0;
				}
				else
				{
					pmvl0[0] = (short) ((mv_scale * cmv_x + 128 ) >> 8);
					pmvl0[1] = (short) ((mv_scale * cmv_y + 128 ) >> 8);
					pmvl1[0] = (short) (pmvl0[0] - cmv_x);
					pmvl1[1] = (short) (pmvl0[1] - cmv_y);
					mv0 = M32(pmvl0);
					mv1 = M32(pmvl1);
				}

				ref0 = (char) mapped_idx;
			}
		}
		M32(&ref_cache0[0]) = M32(&ref_cache0[8]) = M32(&ref_cache0[16]) = M32(&ref_cache0[24]) = ref0*0x01010101;
		ref_pic_list0[0] = ref_pic_list0[1] = ref_pic_list0[2] = ref_pic_list0[3] = 
			ref_pic_list0[4] = ref_pic_list0[5] = ref_pic_list0[6] = ref_pic_list0[7] = 
			ref_pic_list0[8] = ref_pic_list0[9] = ref_pic_list0[10] = ref_pic_list0[11] = 
			ref_pic_list0[12] = ref_pic_list0[13] = ref_pic_list0[14] = ref_pic_list0[15] = list0[ref0];
		mv_cache0[0] = mv_cache0[1] = mv_cache0[2] = mv_cache0[3] = 	 
			mv_cache0[8] = mv_cache0[9] = mv_cache0[10] = mv_cache0[11] = 
			mv_cache0[16] = mv_cache0[17] = mv_cache0[18] = mv_cache0[19] =
			mv_cache0[24] = mv_cache0[25] = mv_cache0[26] = mv_cache0[27] = mv0;
		mv_cache1[0] = mv_cache1[1] = mv_cache1[2] = mv_cache1[3] = 	 
			mv_cache1[8] = mv_cache1[9] = mv_cache1[10] = mv_cache1[11] = 
			mv_cache1[16] = mv_cache1[17] = mv_cache1[18] = mv_cache1[19] =
			mv_cache1[24] = mv_cache1[25] = mv_cache1[26] = mv_cache1[27] = mv1;
	}
	else
	{
		block_xy = mb_xy*16;
		for (block8x8=0; block8x8<4; block8x8++)
		{      
			int k = (block8x8 << 2);
			int col_indexR;
			VO_S32 mapped_idx=0;
			VO_S32 iref;
			VO_S32 mv_scale;
			VO_S16* colocated_mv;
			i =  (decode_block_scan[k] & 3);
			j = ((decode_block_scan[k] >> 2) & 3);
			if(is_8x8 && !IS_DIRECT_VO(pDecLocal->b8type[block8x8]))
				continue;
			pDecLocal->b8type[block8x8] = sub_type;
			ref_cache1[j*8+i]=ref_cache1[j*8+i+1]=ref_cache1[(j+1)*8+i]=ref_cache1[(j+1)*8+i+1]=0;
			ref_pic_list1[j*4+i]=ref_pic_list1[j*4+i+1]=ref_pic_list1[(j+1)*4+i]=ref_pic_list1[(j+1)*4+i+1]= list1[0];
			if(IS_INTRA0(colocated_type[0]))
			{
				ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= list0[0];
				mv_cache0[j*8+i]=mv_cache0[j*8+i+1]=mv_cache0[(j+1)*8+i]=mv_cache0[(j+1)*8+i+1]=0;
				mv_cache1[j*8+i]=mv_cache1[j*8+i+1]=mv_cache1[(j+1)*8+i]=mv_cache1[(j+1)*8+i+1]=0;
				ref_cache0[j*8+i]=ref_cache0[j*8+i+1]=ref_cache0[(j+1)*8+i]=ref_cache0[(j+1)*8+i+1]=0;
				continue;
			}
			col_indexR = currSlice->active_sps->direct_8x8_inference_flag?block_xy+RSD(j)*4+RSD(i):block_xy+j*4+i;
			colocated_ref_list0 = list1[0]->ref_idx[0][col_indexR];
			colocated_ref_list1 = list1[0]->ref_idx[1][col_indexR];
			colocated_ref_pic0 = list1[0]->ref_pic[0][col_indexR];
			colocated_ref_pic1 = list1[0]->ref_pic[1][col_indexR];
			//colocated_ref_pic0 = list1[0]->listX[currSlice->slice_number][0 + list_offset][colocated_ref_list0];
			//colocated_ref_pic1 = list1[0]->listX[currSlice->slice_number][1 + list_offset][colocated_ref_list1];
			colocated_mv0 = (VO_S16*)&list1[0]->mv[0][col_indexR];
			colocated_mv1 = (VO_S16*)&list1[0]->mv[1][col_indexR];
			if(colocated_ref_list0 <= -1)
			{
				ref_pic = colocated_ref_pic1;
				colocated_mv = colocated_mv1;
			}
			else
			{
				ref_pic = colocated_ref_pic0;
				colocated_mv = colocated_mv0;		
			}
			if(ref_pic == NULL)
			{
				return 0;
			}
			for (iref = 0; iref < imin(currSlice->num_ref_idx_active[LIST_0], currSlice->listXsize[currSlice->slice_number][LIST_0 + list_offset]);iref++)
			{
				if(currSlice->listX[currSlice->slice_number][LIST_0 + list_offset][iref] == ref_pic)
				{
					mapped_idx = iref;            
					break;
				}
				else //! invalid index. Default to zero even though this case should not happen
				{
					mapped_idx=INVALIDINDEX;
				}
			}
			if(INVALIDINDEX == mapped_idx)
			{
				ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= list0[0];
				mv_cache0[j*8+i]=mv_cache0[j*8+i+1]=mv_cache0[(j+1)*8+i]=mv_cache0[(j+1)*8+i+1]=0;
				mv_cache1[j*8+i]=mv_cache1[j*8+i+1]=mv_cache1[(j+1)*8+i]=mv_cache1[(j+1)*8+i+1]=0;
				ref_cache0[j*8+i]=ref_cache0[j*8+i+1]=ref_cache0[(j+1)*8+i]=ref_cache0[(j+1)*8+i+1]=0;
				continue;
			}
			mv_scale = currSlice->mvscale[LIST_0 + list_offset][mapped_idx];
			ref0 = (char) mapped_idx;
			ref_cache0[j*8+i]=ref_cache0[j*8+i+1]=ref_cache0[(j+1)*8+i]=ref_cache0[(j+1)*8+i+1]=ref0;
			ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= list0[ref0];
			if(IS_SUB_8X8(sub_type))
			{
				cmv_x = colocated_mv[0];
				cmv_y = colocated_mv[1];
				if (mv_scale == 9999 || currSlice->listX[currSlice->slice_number][LIST_0 + list_offset][mapped_idx]->is_long_term)
				{
					pmvl0[0] = cmv_x;
					pmvl0[1] = cmv_y;
					mv0 = M32(pmvl0);
					mv1 = 0;
				}
				else
				{
					pmvl0[0] = (short) ((mv_scale * cmv_x + 128 ) >> 8);
					pmvl0[1] = (short) ((mv_scale * cmv_y + 128 ) >> 8);
					pmvl1[0] = (short) (pmvl0[0] - cmv_x);
					pmvl1[1] = (short) (pmvl0[1] - cmv_y);
					mv0 = M32(pmvl0);
					mv1 = M32(pmvl1);
				}
				mv_cache0[j*8+i]=mv_cache0[j*8+i+1]=mv_cache0[(j+1)*8+i]=mv_cache0[(j+1)*8+i+1]=mv0;
				mv_cache1[j*8+i]=mv_cache1[j*8+i+1]=mv_cache1[(j+1)*8+i]=mv_cache1[(j+1)*8+i+1]=mv1;
			}
			else
			{
				VO_S32 block4x4,index;
				for(block4x4=0; block4x4<4; block4x4++)
				{
					index = (block4x4&2)*4+(block4x4&1);
					cmv_x = colocated_mv[index+(block4x4&1)+0];
					cmv_y = colocated_mv[index+(block4x4&1)+1];
					if (mv_scale == 9999 || currSlice->listX[currSlice->slice_number][LIST_0 + list_offset][mapped_idx]->is_long_term)
					{
						pmvl0[0] = cmv_x;
						pmvl0[1] = cmv_y;
						mv0 = M32(pmvl0);
						mv1 = 0;
					}
					else
					{
						pmvl0[0] = (short) ((mv_scale * cmv_x + 128 ) >> 8);
						pmvl0[1] = (short) ((mv_scale * cmv_y + 128 ) >> 8);
						pmvl1[0] = (short) (pmvl0[0] - cmv_x);
						pmvl1[1] = (short) (pmvl0[1] - cmv_y);
						mv0 = M32(pmvl0);
						mv1 = M32(pmvl1);
					}
					mv_cache0[j*8+i+index]=mv0;
					mv_cache1[j*8+i+index]=mv1;
				}
			}
		}
	}
	return 1;
}

VO_S32 update_direct_mv_info_spatial(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32* mb_type)
{
  VO_S32 l_ref[2];
  VO_S32 pmv[2]={0};
  VO_S32 block8x8,list;
  int stride_4x4 = 4;
  int stride_8x8 = 4;
  Slice *currSlice = pDecLocal->p_Slice;
  //int list_offset = (pDecLocal->mb_field&&!currSlice->field_pic_flag)? (pDecLocal->mb.y&1)?4:2: 0;
  StorablePicture **list0 = currSlice->listX[currSlice->slice_number][LIST_0];
  StorablePicture **list1 = currSlice->listX[currSlice->slice_number][LIST_1];
  StorablePicture** ref_pic_list0 = pDecGlobal->dec_picture->ref_pic[0]+pDecLocal->mb_xy*16;
  StorablePicture** ref_pic_list1 = pDecGlobal->dec_picture->ref_pic[1]+pDecLocal->mb_xy*16;
  VO_S32* mv_cache0 = &pDecLocal->mv_cache[0][12];
  VO_S32* mv_cache1 = &pDecLocal->mv_cache[1][12];
  VO_S8* ref_cache0 = &pDecLocal->ref_cache[0][12];
  VO_S8* ref_cache1 = &pDecLocal->ref_cache[1][12];
  VO_S32 ref0,ref1,mv0, mv1;
  StorablePicture* ref_pic[2];
  int pred_dir = 0;
  VO_U16 mov_flag = 0;
  VO_S32 colocated_type[2];
  VO_S32 is_8x8 = IS_8X8(*mb_type);
  VO_U32 sub_type;
  VO_S32 mb_xy = pDecLocal->mb_xy;
  VO_S32 index_shift = 0;
  if(list0[0] == NULL||list1[0] == NULL)
  {
    return 0;
  }
  for(list=0; list<2; list++)
  {
    VO_S32 l0_refA, l0_refB, l0_refC;
    VO_S8* ref_cache0 = &pDecLocal->ref_cache[list][0];
    VO_S32 n = pDecLocal->mbAvailC ? 8 : 3;
    VO_S16 *C= (VO_S16 *)&pDecLocal->mv_cache[list][n];
    l0_refA  = ref_cache0[11];
    l0_refB = ref_cache0[4];
    l0_refC = ref_cache0[n];

    l_ref[list] =VOMIN3((unsigned) l0_refA, (unsigned) l0_refB, (unsigned) l0_refC);
    if(l_ref[list] >= 0)
    {
      VO_S16 * const A= (VO_S16 *)&pDecLocal->mv_cache[list][ 12 - 1 ];
      VO_S16 * const B= (VO_S16 *)&pDecLocal->mv_cache[list][ 12 - 8 ];

      int match_count= (l0_refA==l_ref[list]) + (l0_refB==l_ref[list]) + (l0_refC==l_ref[list]);
      if(match_count > 1)
	  {
	    VO_S16 *p = (VO_S16 *)&pmv[list];
        p[0]= imedian(A[0], B[0], C[0]);
		p[1]= imedian(A[1], B[1], C[1]);
      }
	  else 
	  {
        if(l0_refA==l_ref[list])
          pmv[list]= M32(A);
        else if(l0_refB==l_ref[list])
          pmv[list]= M32(B);
        else
          pmv[list]= M32(C);
      }
    }
	else
	{
      VO_S32 mask= ~(VO_L0 << (2*list));
      pmv[list] = 0;
      l_ref[list] = -1;
      if(!is_8x8)
        *mb_type &= mask;
      sub_type &= mask;
	  
    }
  }
#if USE_FRAME_THREAD
	if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
	{
		//wait for list1[0],mv=(?,?) progress ready
		FrameThdInfo *pInfo = pDecGlobal->pThdInfo;
		const VO_S32 max_y = pDecLocal->mb.y*MB_BLOCK_SIZE + MB_BLOCK_SIZE-1; 	  //???interlace need fix
        //const VO_S32 max_y = 10000;
		VO_S32 errFlag = WaitForProgress(pInfo, list1[0], max_y, 0);
		
		if(errFlag)
		{
			//ref is error
			pInfo->errFlag = errFlag;
			return errFlag;
		}
	}
#endif
  //if(!currSlice->active_sps->direct_8x8_inference_flag)
  /*{
    if (l_ref[0] < 0 && l_ref[1] < 0)
    {
	  //pred_dir=2;
	  ref_pic[0] = list0[0];
      ref_pic[1] = list1[0];
    }
	else if (l_ref[1] <= -1)
	{
	  //pred_dir=0;
	  ref_pic[0] = list0[l_ref[0]];
      ref_pic[1] = NULL;
	}
	else if (l_ref[0] <= -1)
	{
	  //pred_dir=1;
	  ref_pic[0] = NULL;
      ref_pic[1] = list1[l_ref[1]];
	}
	else
	{
	  //pred_dir=2;
	  ref_pic[0] = list0[l_ref[0]];
      ref_pic[1] = list1[l_ref[1]];
	}
    //pDecLocal->b8pdir[0] = pDecLocal->b8pdir[1] = pDecLocal->b8pdir[2] = pDecLocal->b8pdir[3] = pred_dir;
  }*/
  if(l_ref[0] < 0 && l_ref[1] < 0)
  {
    l_ref[0] = l_ref[1] = 0;
    if(!is_8x8)
      *mb_type |= VO_L0L1;
    sub_type |= VO_L0L1;
  }
  if(!(is_8x8|pmv[0]|pmv[1]))
  {
    M32(ref_cache0)=M32(ref_cache0+8)=M32(ref_cache0+16)=M32(ref_cache0+24)=((VO_U8)l_ref[0])*0x01010101;
	M32(ref_cache1)=M32(ref_cache1+8)=M32(ref_cache1+16)=M32(ref_cache1+24)=((VO_U8)l_ref[1])*0x01010101;
	//ref_pic_list0[0]=ref_pic_list0[1]=ref_pic_list0[2]=ref_pic_list0[3]=
	//ref_pic_list0[4]=ref_pic_list0[5]=ref_pic_list0[6]=ref_pic_list0[7]=
	//ref_pic_list0[8]=ref_pic_list0[9]=ref_pic_list0[10]=ref_pic_list0[11]=
	//ref_pic_list0[12]=ref_pic_list0[13]=ref_pic_list0[14]=ref_pic_list0[15]=ref_pic[0];
	//ref_pic_list1[0]=ref_pic_list1[1]=ref_pic_list1[2]=ref_pic_list1[3]=
	//ref_pic_list1[4]=ref_pic_list1[5]=ref_pic_list1[6]=ref_pic_list1[7]=
	//ref_pic_list1[8]=ref_pic_list1[9]=ref_pic_list1[10]=ref_pic_list1[11]=
	//ref_pic_list1[12]=ref_pic_list1[13]=ref_pic_list1[14]=ref_pic_list1[15]=ref_pic[1];
	mv_cache0[0] = mv_cache0[1] = mv_cache0[2] = mv_cache0[3] = 	 
	mv_cache0[8] = mv_cache0[9] = mv_cache0[10] = mv_cache0[11] = 
	mv_cache0[16] = mv_cache0[17] = mv_cache0[18] = mv_cache0[19] =
	mv_cache0[24] = mv_cache0[25] = mv_cache0[26] = mv_cache0[27] = pmv[0];
	mv_cache1[0] = mv_cache1[1] = mv_cache1[2] = mv_cache1[3] = 	 
	mv_cache1[8] = mv_cache1[9] = mv_cache1[10] = mv_cache1[11] = 
	mv_cache1[16] = mv_cache1[17] = mv_cache1[18] = mv_cache1[19] =
	mv_cache1[24] = mv_cache1[25] = mv_cache1[26] = mv_cache1[27] = pmv[1];
	//pDecLocal->direct_mode[0] = 16;
    *mb_type= (*mb_type & ~(VO_8x8|VO_16x8|VO_8x16|VO_P1L0|VO_P1L1))|VO_16x16|VO_DIRECT;
    return 1;
  }

                                      
      colocated_type[0] = colocated_type[1] = list1[0]->mb_type_buffer[mb_xy];

      sub_type = VO_16x16|VO_DIRECT; 
      if(!is_8x8 && (colocated_type[0] & VO_INTRA_OR_16X16))
	  {
         *mb_type   |= VO_16x16|VO_DIRECT;
      }
	  else if(!is_8x8 && (colocated_type[0] & (VO_16x8|VO_8x16)))
	  {
         *mb_type   |=VO_DIRECT | (colocated_type[0] & (VO_16x8|VO_8x16));
      }
	  else
	  {
         if(!currSlice->active_sps->direct_8x8_inference_flag)
		 {
           sub_type += (VO_8x8-VO_16x16); 
         }
         *mb_type   |= VO_8x8;
      }
    
  
  if(IS_16X16(*mb_type))
  {
	VO_S32 index = mb_xy*16;
	VO_S32 ref_idx_list0 = list1[0]->ref_idx[0][index];
	VO_S32 ref_idx_list1 = list1[0]->ref_idx[1][index];
	VO_S16* mv_list0 = (VO_S16*)&list1[0]->mv[0][index];
	VO_S16* mv_list1 = (VO_S16*)&list1[0]->mv[1][index];
    //ref_pic[0]=ref_pic[1]=NULL;
    ref0=ref1=-1;
	mv0=mv1=0;
	M32(ref_cache0)=M32(ref_cache0+8)=M32(ref_cache0+16)=M32(ref_cache0+24)=((VO_U8)l_ref[0])*0x01010101;
	M32(ref_cache1)=M32(ref_cache1+8)=M32(ref_cache1+16)=M32(ref_cache1+24)=((VO_U8)l_ref[1])*0x01010101;
	//ref_pic_list0[0] = ref_pic_list0[1] = ref_pic_list0[2] = ref_pic_list0[3] = 
	//ref_pic_list0[4] = ref_pic_list0[5] = ref_pic_list0[6] = ref_pic_list0[7] = 
	//ref_pic_list0[8] = ref_pic_list0[9] = ref_pic_list0[10] = ref_pic_list0[11] = 
	//ref_pic_list0[12] = ref_pic_list0[13] = ref_pic_list0[14] = ref_pic_list0[15] = ref_pic[0];
	//ref_pic_list1[0] = ref_pic_list1[1] = ref_pic_list1[2] = ref_pic_list1[3] = 
	//ref_pic_list1[4] = ref_pic_list1[5] = ref_pic_list1[6] = ref_pic_list1[7] = 
	//ref_pic_list1[8] = ref_pic_list1[9] = ref_pic_list1[10] = ref_pic_list1[11] = 
	//ref_pic_list1[12] = ref_pic_list1[13] = ref_pic_list1[14] = ref_pic_list1[15] = ref_pic[1];
    if(!IS_INTRA0(colocated_type[0]) && !list1[0]->is_long_term
       && ((ref_idx_list0 == 0 && iabs(mv_list0[0]) <= 1 && iabs(mv_list0[1]) <= 1)
       || (ref_idx_list0  < 0 && ref_idx_list1 == 0 && iabs(mv_list1[0]) <= 1 && iabs(mv_list1[1]) <= 1)))
    {     
      if(l_ref[0] > 0)
      {
        mv0= pmv[0];
      }
      if(l_ref[1] > 0)
      {
        mv1= pmv[1];
      }
    }
	else
	{
      mv0= pmv[0];
      mv1= pmv[1];
    }
    
	mv_cache0[0] = mv_cache0[1] = mv_cache0[2] = mv_cache0[3] = 	 
	mv_cache0[8] = mv_cache0[9] = mv_cache0[10] = mv_cache0[11] = 
	mv_cache0[16] = mv_cache0[17] = mv_cache0[18] = mv_cache0[19] =
	mv_cache0[24] = mv_cache0[25] = mv_cache0[26] = mv_cache0[27] = mv0;
	mv_cache1[0] = mv_cache1[1] = mv_cache1[2] = mv_cache1[3] = 	 
	mv_cache1[8] = mv_cache1[9] = mv_cache1[10] = mv_cache1[11] = 
	mv_cache1[16] = mv_cache1[17] = mv_cache1[18] = mv_cache1[19] =
	mv_cache1[24] = mv_cache1[25] = mv_cache1[26] = mv_cache1[27] = mv1;
	
  }
  else
  {
    VO_S32 n=0;
    for(block8x8=0; block8x8<4; block8x8++)
	{
	  VO_S32 k = (block8x8 << 2);
      VO_S32 i =  (decode_block_scan[k] & 3);
      VO_S32 j = ((decode_block_scan[k] >> 2) & 3);
      VO_S32 index = currSlice->active_sps->direct_8x8_inference_flag?mb_xy*16+RSD(j)*4+RSD(i):mb_xy*16+j*4+i;
	  VO_S32 ref_idx_list0 = list1[0]->ref_idx[0][index];
	  VO_S32 ref_idx_list1 = list1[0]->ref_idx[1][index];
	  VO_S16* mv_list0 = (VO_S16*)&list1[0]->mv[0][index];
	  VO_S16* mv_list1 = (VO_S16*)&list1[0]->mv[1][index];
      if(is_8x8 && !IS_DIRECT_VO(pDecLocal->b8type[block8x8]))
        continue;
      pDecLocal->b8type[block8x8] = sub_type;

      ref_cache0[j*8+i]=ref_cache0[j*8+i+1]=ref_cache0[(j+1)*8+i]=ref_cache0[(j+1)*8+i+1]=l_ref[0];
      ref_cache1[j*8+i]=ref_cache1[j*8+i+1]=ref_cache1[(j+1)*8+i]=ref_cache1[(j+1)*8+i+1]=l_ref[1];
	  //ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= ref_pic[0];
	  //ref_pic_list1[j*4+i]=ref_pic_list1[j*4+i+1]=ref_pic_list1[(j+1)*4+i]=ref_pic_list1[(j+1)*4+i+1]= ref_pic[1];
	  mv_cache0[j*8+i]=mv_cache0[j*8+i+1]=mv_cache0[(j+1)*8+i]=mv_cache0[(j+1)*8+i+1]=pmv[0];
	  mv_cache1[j*8+i]=mv_cache1[j*8+i+1]=mv_cache1[(j+1)*8+i]=mv_cache1[(j+1)*8+i+1]=pmv[1];

      if(!IS_INTRA0(colocated_type[0]) && !list1[0]->is_long_term && 
	  	( ref_idx_list0 == 0|| (ref_idx_list0 < 0 && ref_idx_list1 == 0)))
	  {
        const VO_S16 *mv8= ref_idx_list0 == 0 ? mv_list0 : mv_list1;
        if(IS_SUB_8X8(sub_type))
		{
          if(iabs(mv8[0]) <= 1 && iabs(mv8[1]) <= 1)
		  {
            if(l_ref[0] == 0)
            {
              mv_cache0[j*8+i]=mv_cache0[j*8+i+1]=mv_cache0[(j+1)*8+i]=mv_cache0[(j+1)*8+i+1]=0;
            }
            if(l_ref[1] == 0)
            {
              mv_cache1[j*8+i]=mv_cache1[j*8+i+1]=mv_cache1[(j+1)*8+i]=mv_cache1[(j+1)*8+i+1]=0;
            }
            n+=4;
          }
        }
		else
		{
          VO_S32 m=0;
		  VO_S32 block4x4,index;
		  VO_S16 *mv4;
          for(block4x4=0; block4x4<4; block4x4++)
		  {
		    index = (block4x4&2)*4+(block4x4&1);
            mv4 = &mv_list0[index+(block4x4&1)];
            if(iabs(mv4[0]) <= 1 && iabs(mv4[1]) <= 1)
			{
              if(l_ref[0] == 0)
                mv_cache0[j*8+i+index]=0;
              if(l_ref[1] == 0)
                mv_cache1[j*8+i+index]=0;
              m++;
            }
          }
          if(!(m&3))
             pDecLocal->b8type[block8x8]+= VO_16x16 - VO_8x8;
          n+=m;
        }
      }
    }
    if(!is_8x8 && !(n&15))
      *mb_type= (*mb_type & ~(VO_8x8|VO_16x8|VO_8x16|VO_P1L0|VO_P1L1))|VO_16x16|VO_DIRECT;
  }
  return 1;
}

VO_S32 update_direct_mv_info_spatial_interlace(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32* mb_type)
{
	VO_S32 l_ref[2];
	VO_S32 pmv[2]={0};
	VO_S32 block8x8,list;
	int stride_4x4 = 4;
	int stride_8x8 = 4;
	Slice *currSlice = pDecLocal->p_Slice;
	int list_offset = (pDecLocal->mb_field&&!currSlice->field_pic_flag)? (pDecLocal->mb.y&1)?4:2: 0;
	StorablePicture **list0 = currSlice->listX[currSlice->slice_number][LIST_0+list_offset];
	StorablePicture **list1 = currSlice->listX[currSlice->slice_number][LIST_1+list_offset];
	StorablePicture** ref_pic_list0 = pDecGlobal->dec_picture->ref_pic[0]+pDecLocal->mb_xy*16;
	StorablePicture** ref_pic_list1 = pDecGlobal->dec_picture->ref_pic[1]+pDecLocal->mb_xy*16;
	VO_S32* mv_cache0 = &pDecLocal->mv_cache[0][12];
	VO_S32* mv_cache1 = &pDecLocal->mv_cache[1][12];
	VO_S8* ref_cache0 = &pDecLocal->ref_cache[0][12];
	VO_S8* ref_cache1 = &pDecLocal->ref_cache[1][12];
	VO_S32 ref0,ref1,mv0, mv1;
	StorablePicture* ref_pic[2];
	int pred_dir = 0;
	VO_U16 mov_flag = 0;
	VO_S32 colocated_type[2];
	VO_S32 is_8x8 = IS_8X8(*mb_type);
	VO_U32 sub_type;
	VO_S32 mb_xy = pDecLocal->mb_xy;
	VO_S32 index_shift = 0;
	if(list0[0] == NULL||list1[0] == NULL)
	{
		return 0;
	}
	for(list=0; list<2; list++)
	{
		VO_S32 l0_refA, l0_refB, l0_refC;
		VO_S8* ref_cache0 = &pDecLocal->ref_cache[list][0];
		VO_S32 n = pDecLocal->mbAvailC ? 8 : 3;
		VO_S16 *C= (VO_S16 *)&pDecLocal->mv_cache[list][n];
		l0_refA  = ref_cache0[11];
		l0_refB = ref_cache0[4];
		l0_refC = ref_cache0[n];

		l_ref[list] =VOMIN3((unsigned) l0_refA, (unsigned) l0_refB, (unsigned) l0_refC);
		if(l_ref[list] >= 0)
		{
			VO_S16 * const A= (VO_S16 *)&pDecLocal->mv_cache[list][ 12 - 1 ];
			VO_S16 * const B= (VO_S16 *)&pDecLocal->mv_cache[list][ 12 - 8 ];

			int match_count= (l0_refA==l_ref[list]) + (l0_refB==l_ref[list]) + (l0_refC==l_ref[list]);
			if(match_count > 1)
			{
				VO_S16 *p = (VO_S16 *)&pmv[list];
				p[0]= imedian(A[0], B[0], C[0]);
				p[1]= imedian(A[1], B[1], C[1]);
			}
			else 
			{
				if(l0_refA==l_ref[list])
					pmv[list]= M32(A);
				else if(l0_refB==l_ref[list])
					pmv[list]= M32(B);
				else
					pmv[list]= M32(C);
			}
		}
		else
		{
			VO_S32 mask= ~(VO_L0 << (2*list));
			pmv[list] = 0;
			l_ref[list] = -1;
			if(!is_8x8)
				*mb_type &= mask;
			sub_type &= mask;

		}
	}
#if USE_FRAME_THREAD
	if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
	{
		//wait for list1[0],mv=(?,?) progress ready
		FrameThdInfo *pInfo = pDecGlobal->pThdInfo;
		const VO_S32 max_y = pDecLocal->mb.y*MB_BLOCK_SIZE + MB_BLOCK_SIZE-1+MB_BLOCK_SIZE*(!!IS_INTERLACED(*mb_type)); 	  //???interlace need fix
		//const VO_S32 max_y = 10000;
		VO_S32 errFlag = WaitForProgress(pInfo, list1[0], max_y, 0);

		if(errFlag)
		{
			//ref is error
			pInfo->errFlag = errFlag;
			return errFlag;
		}
	}
#endif
	//if(!currSlice->active_sps->direct_8x8_inference_flag)
	{
		if (l_ref[0] < 0 && l_ref[1] < 0)
		{
			//pred_dir=2;
			ref_pic[0] = list0[0];
			ref_pic[1] = list1[0];
		}
		else if (l_ref[1] <= -1)
		{
			//pred_dir=0;
			ref_pic[0] = list0[l_ref[0]];
			ref_pic[1] = NULL;
		}
		else if (l_ref[0] <= -1)
		{
			//pred_dir=1;
			ref_pic[0] = NULL;
			ref_pic[1] = list1[l_ref[1]];
		}
		else
		{
			//pred_dir=2;
			ref_pic[0] = list0[l_ref[0]];
			ref_pic[1] = list1[l_ref[1]];
		}
		//pDecLocal->b8pdir[0] = pDecLocal->b8pdir[1] = pDecLocal->b8pdir[2] = pDecLocal->b8pdir[3] = pred_dir;
	}
	if(l_ref[0] < 0 && l_ref[1] < 0)
	{
		l_ref[0] = l_ref[1] = 0;
		if(!is_8x8)
			*mb_type |= VO_L0L1;
		sub_type |= VO_L0L1;
	}
	if(!(is_8x8|pmv[0]|pmv[1]))
	{
		M32(ref_cache0)=M32(ref_cache0+8)=M32(ref_cache0+16)=M32(ref_cache0+24)=((VO_U8)l_ref[0])*0x01010101;
		M32(ref_cache1)=M32(ref_cache1+8)=M32(ref_cache1+16)=M32(ref_cache1+24)=((VO_U8)l_ref[1])*0x01010101;
		ref_pic_list0[0]=ref_pic_list0[1]=ref_pic_list0[2]=ref_pic_list0[3]=
			ref_pic_list0[4]=ref_pic_list0[5]=ref_pic_list0[6]=ref_pic_list0[7]=
			ref_pic_list0[8]=ref_pic_list0[9]=ref_pic_list0[10]=ref_pic_list0[11]=
			ref_pic_list0[12]=ref_pic_list0[13]=ref_pic_list0[14]=ref_pic_list0[15]=ref_pic[0];
		ref_pic_list1[0]=ref_pic_list1[1]=ref_pic_list1[2]=ref_pic_list1[3]=
			ref_pic_list1[4]=ref_pic_list1[5]=ref_pic_list1[6]=ref_pic_list1[7]=
			ref_pic_list1[8]=ref_pic_list1[9]=ref_pic_list1[10]=ref_pic_list1[11]=
			ref_pic_list1[12]=ref_pic_list1[13]=ref_pic_list1[14]=ref_pic_list1[15]=ref_pic[1];
		mv_cache0[0] = mv_cache0[1] = mv_cache0[2] = mv_cache0[3] = 	 
			mv_cache0[8] = mv_cache0[9] = mv_cache0[10] = mv_cache0[11] = 
			mv_cache0[16] = mv_cache0[17] = mv_cache0[18] = mv_cache0[19] =
			mv_cache0[24] = mv_cache0[25] = mv_cache0[26] = mv_cache0[27] = pmv[0];
		mv_cache1[0] = mv_cache1[1] = mv_cache1[2] = mv_cache1[3] = 	 
			mv_cache1[8] = mv_cache1[9] = mv_cache1[10] = mv_cache1[11] = 
			mv_cache1[16] = mv_cache1[17] = mv_cache1[18] = mv_cache1[19] =
			mv_cache1[24] = mv_cache1[25] = mv_cache1[26] = mv_cache1[27] = pmv[1];
		//pDecLocal->direct_mode[0] = 16;
		*mb_type= (*mb_type & ~(VO_8x8|VO_16x8|VO_8x16|VO_P1L0|VO_P1L1))|VO_16x16|VO_DIRECT;
		return 1;
	}


	if(IS_INTERLACED(list1[0]->mb_type_buffer[mb_xy]))
	{ 
		if(!IS_INTERLACED(*mb_type))
		{                   
			mb_xy= pDecLocal->mb.x + ((pDecLocal->mb.y&~1) + pDecGlobal->mbaff_dis)*pDecGlobal->PicWidthInMbs;
			stride_8x8 = 0;
		}
		else
		{
			mb_xy += pDecGlobal->field_offset; 
		}
		colocated_type[0] = colocated_type[1] = list1[0]->mb_type_buffer[mb_xy];

		sub_type = VO_16x16|VO_DIRECT; 
		if(!is_8x8 && (colocated_type[0] & VO_INTRA_OR_16X16))
		{
			*mb_type   |= VO_16x16|VO_DIRECT;
		}
		else if(!is_8x8 && (colocated_type[0] & (VO_16x8|VO_8x16)))
		{
			*mb_type   |=VO_DIRECT | (colocated_type[0] & (VO_16x8|VO_8x16));
		}
		else
		{
			if(!currSlice->active_sps->direct_8x8_inference_flag)
			{
				sub_type += (VO_8x8-VO_16x16); 
			}
			*mb_type   |= VO_8x8;
		}
	}
	else
	{                                              
		if(IS_INTERLACED(*mb_type))
		{                    
			mb_xy= pDecLocal->mb.x + (pDecLocal->mb.y&~1)*pDecGlobal->PicWidthInMbs;
			colocated_type[0] = list1[0]->mb_type_buffer[mb_xy];
			colocated_type[1] = list1[0]->mb_type_buffer[mb_xy+pDecGlobal->PicWidthInMbs];
			stride_8x8 = 2+4*4;
			stride_4x4 = pDecGlobal->PicWidthInMbs*16+8;

			sub_type = VO_16x16|VO_P0L0|VO_P0L1|VO_DIRECT; 

			if((colocated_type[0] & VO_INTRA_OR_16X16)
				&& (colocated_type[1] & VO_INTRA_OR_16X16)&& !is_8x8)
			{
				*mb_type   |= VO_16x8 |VO_L0L1|VO_DIRECT;
			}
			else
			{
				*mb_type   |= VO_8x8|VO_L0L1;
			}
		}
		else
		{                                          
			colocated_type[0] = colocated_type[1] = list1[0]->mb_type_buffer[mb_xy];

			sub_type = VO_16x16|VO_DIRECT; 
			if(!is_8x8 && (colocated_type[0] & VO_INTRA_OR_16X16))
			{
				*mb_type   |= VO_16x16|VO_DIRECT;
			}
			else if(!is_8x8 && (colocated_type[0] & (VO_16x8|VO_8x16)))
			{
				*mb_type   |=VO_DIRECT | (colocated_type[0] & (VO_16x8|VO_8x16));
			}
			else
			{
				if(!currSlice->active_sps->direct_8x8_inference_flag)
				{
					sub_type += (VO_8x8-VO_16x16); 
				}
				*mb_type   |= VO_8x8;
			}
		}
	}
	if(!stride_8x8)
	{
		if(pDecLocal->mb.y&1)
			index_shift = 2*stride_4x4;
	}
	if(IS_INTERLACED(*mb_type) != IS_INTERLACED(colocated_type[0]))
	{
		int n=0;
		for(block8x8=0; block8x8<4; block8x8++)
		{
			int x8 = block8x8&1;
			int y8 = block8x8>>1;
			VO_S32 k = (block8x8 << 2);
			VO_S32 i =  (decode_block_scan[k] & 3);
			VO_S32 j = ((decode_block_scan[k] >> 2) & 3);
			VO_S32 index = mb_xy*16+x8*3+y8*stride_4x4+index_shift;
			VO_S32 ref_idx_list0 = list1[0]->ref_idx[0][index];
			VO_S32 ref_idx_list1 = list1[0]->ref_idx[1][index];
			VO_S16* mv_list0 = (VO_S16*)&list1[0]->mv[0][index];
			VO_S16* mv_list1 = (VO_S16*)&list1[0]->mv[1][index];
			if(is_8x8 && !IS_DIRECT_VO(pDecLocal->b8type[block8x8]))
				continue;
			pDecLocal->b8type[block8x8] = sub_type;

			ref_cache0[j*8+i]=ref_cache0[j*8+i+1]=ref_cache0[(j+1)*8+i]=ref_cache0[(j+1)*8+i+1]=l_ref[0];
			ref_cache1[j*8+i]=ref_cache1[j*8+i+1]=ref_cache1[(j+1)*8+i]=ref_cache1[(j+1)*8+i+1]=l_ref[1];
			ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= ref_pic[0];
			ref_pic_list1[j*4+i]=ref_pic_list1[j*4+i+1]=ref_pic_list1[(j+1)*4+i]=ref_pic_list1[(j+1)*4+i+1]= ref_pic[1];
			if(!IS_INTRA0(colocated_type[y8]) && !list1[0]->is_long_term
				&& ((ref_idx_list0 == 0 && iabs(mv_list0[0]) <= 1 && iabs(mv_list0[1]) <= 1)
				|| (ref_idx_list0  < 0 && ref_idx_list1 == 0 && iabs(mv_list1[0]) <= 1 && iabs(mv_list1[1]) <= 1)))
			{   
				mv0=mv1=0;
				if(l_ref[0] > 0)
				{
					mv0= pmv[0];
				}
				if(l_ref[1] > 0)
				{
					mv1= pmv[1];
				}
				n++;
			}
			else
			{
				mv0= pmv[0];
				mv1= pmv[1];
			}

			mv_cache0[j*8+i]=mv_cache0[j*8+i+1]=mv_cache0[(j+1)*8+i]=mv_cache0[(j+1)*8+i+1]=mv0;
			mv_cache1[j*8+i]=mv_cache1[j*8+i+1]=mv_cache1[(j+1)*8+i]=mv_cache1[(j+1)*8+i+1]=mv1;
		}
		if(!is_8x8 && !(n&3))
			*mb_type= (*mb_type & ~(VO_8x8|VO_16x8|VO_8x16|VO_P1L0|VO_P1L1))|VO_16x16|VO_DIRECT;
	}
	else if(IS_16X16(*mb_type))
	{
		VO_S32 index = mb_xy*16;
		VO_S32 ref_idx_list0 = list1[0]->ref_idx[0][index];
		VO_S32 ref_idx_list1 = list1[0]->ref_idx[1][index];
		VO_S16* mv_list0 = (VO_S16*)&list1[0]->mv[0][index];
		VO_S16* mv_list1 = (VO_S16*)&list1[0]->mv[1][index];
		//ref_pic[0]=ref_pic[1]=NULL;
		ref0=ref1=-1;
		mv0=mv1=0;
		M32(ref_cache0)=M32(ref_cache0+8)=M32(ref_cache0+16)=M32(ref_cache0+24)=((VO_U8)l_ref[0])*0x01010101;
		M32(ref_cache1)=M32(ref_cache1+8)=M32(ref_cache1+16)=M32(ref_cache1+24)=((VO_U8)l_ref[1])*0x01010101;
		ref_pic_list0[0] = ref_pic_list0[1] = ref_pic_list0[2] = ref_pic_list0[3] = 
			ref_pic_list0[4] = ref_pic_list0[5] = ref_pic_list0[6] = ref_pic_list0[7] = 
			ref_pic_list0[8] = ref_pic_list0[9] = ref_pic_list0[10] = ref_pic_list0[11] = 
			ref_pic_list0[12] = ref_pic_list0[13] = ref_pic_list0[14] = ref_pic_list0[15] = ref_pic[0];
		ref_pic_list1[0] = ref_pic_list1[1] = ref_pic_list1[2] = ref_pic_list1[3] = 
			ref_pic_list1[4] = ref_pic_list1[5] = ref_pic_list1[6] = ref_pic_list1[7] = 
			ref_pic_list1[8] = ref_pic_list1[9] = ref_pic_list1[10] = ref_pic_list1[11] = 
			ref_pic_list1[12] = ref_pic_list1[13] = ref_pic_list1[14] = ref_pic_list1[15] = ref_pic[1];
		if(!IS_INTRA0(colocated_type[0]) && !list1[0]->is_long_term
			&& ((ref_idx_list0 == 0 && iabs(mv_list0[0]) <= 1 && iabs(mv_list0[1]) <= 1)
			|| (ref_idx_list0  < 0 && ref_idx_list1 == 0 && iabs(mv_list1[0]) <= 1 && iabs(mv_list1[1]) <= 1)))
		{     
			if(l_ref[0] > 0)
			{
				mv0= pmv[0];
			}
			if(l_ref[1] > 0)
			{
				mv1= pmv[1];
			}
		}
		else
		{
			mv0= pmv[0];
			mv1= pmv[1];
		}

		mv_cache0[0] = mv_cache0[1] = mv_cache0[2] = mv_cache0[3] = 	 
			mv_cache0[8] = mv_cache0[9] = mv_cache0[10] = mv_cache0[11] = 
			mv_cache0[16] = mv_cache0[17] = mv_cache0[18] = mv_cache0[19] =
			mv_cache0[24] = mv_cache0[25] = mv_cache0[26] = mv_cache0[27] = mv0;
		mv_cache1[0] = mv_cache1[1] = mv_cache1[2] = mv_cache1[3] = 	 
			mv_cache1[8] = mv_cache1[9] = mv_cache1[10] = mv_cache1[11] = 
			mv_cache1[16] = mv_cache1[17] = mv_cache1[18] = mv_cache1[19] =
			mv_cache1[24] = mv_cache1[25] = mv_cache1[26] = mv_cache1[27] = mv1;

	}
	else
	{
		VO_S32 n=0;
		for(block8x8=0; block8x8<4; block8x8++)
		{
			VO_S32 k = (block8x8 << 2);
			VO_S32 i =  (decode_block_scan[k] & 3);
			VO_S32 j = ((decode_block_scan[k] >> 2) & 3);
			VO_S32 index = currSlice->active_sps->direct_8x8_inference_flag?mb_xy*16+RSD(j)*4+RSD(i):mb_xy*16+j*4+i;
			VO_S32 ref_idx_list0 = list1[0]->ref_idx[0][index];
			VO_S32 ref_idx_list1 = list1[0]->ref_idx[1][index];
			VO_S16* mv_list0 = (VO_S16*)&list1[0]->mv[0][index];
			VO_S16* mv_list1 = (VO_S16*)&list1[0]->mv[1][index];
			if(is_8x8 && !IS_DIRECT_VO(pDecLocal->b8type[block8x8]))
				continue;
			pDecLocal->b8type[block8x8] = sub_type;

			ref_cache0[j*8+i]=ref_cache0[j*8+i+1]=ref_cache0[(j+1)*8+i]=ref_cache0[(j+1)*8+i+1]=l_ref[0];
			ref_cache1[j*8+i]=ref_cache1[j*8+i+1]=ref_cache1[(j+1)*8+i]=ref_cache1[(j+1)*8+i+1]=l_ref[1];
			ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= ref_pic[0];
			ref_pic_list1[j*4+i]=ref_pic_list1[j*4+i+1]=ref_pic_list1[(j+1)*4+i]=ref_pic_list1[(j+1)*4+i+1]= ref_pic[1];
			mv_cache0[j*8+i]=mv_cache0[j*8+i+1]=mv_cache0[(j+1)*8+i]=mv_cache0[(j+1)*8+i+1]=pmv[0];
			mv_cache1[j*8+i]=mv_cache1[j*8+i+1]=mv_cache1[(j+1)*8+i]=mv_cache1[(j+1)*8+i+1]=pmv[1];

			if(!IS_INTRA0(colocated_type[0]) && !list1[0]->is_long_term && 
				( ref_idx_list0 == 0|| (ref_idx_list0 < 0 && ref_idx_list1 == 0)))
			{
				const VO_S16 *mv8= ref_idx_list0 == 0 ? mv_list0 : mv_list1;
				if(IS_SUB_8X8(sub_type))
				{
					if(iabs(mv8[0]) <= 1 && iabs(mv8[1]) <= 1)
					{
						if(l_ref[0] == 0)
						{
							mv_cache0[j*8+i]=mv_cache0[j*8+i+1]=mv_cache0[(j+1)*8+i]=mv_cache0[(j+1)*8+i+1]=0;
						}
						if(l_ref[1] == 0)
						{
							mv_cache1[j*8+i]=mv_cache1[j*8+i+1]=mv_cache1[(j+1)*8+i]=mv_cache1[(j+1)*8+i+1]=0;
						}
						n+=4;
					}
				}
				else
				{
					VO_S32 m=0;
					VO_S32 block4x4,index;
					VO_S16 *mv4;
					for(block4x4=0; block4x4<4; block4x4++)
					{
						index = (block4x4&2)*4+(block4x4&1);
						mv4 = &mv_list0[index+(block4x4&1)];
						if(iabs(mv4[0]) <= 1 && iabs(mv4[1]) <= 1)
						{
							if(l_ref[0] == 0)
								mv_cache0[j*8+i+index]=0;
							if(l_ref[1] == 0)
								mv_cache1[j*8+i+index]=0;
							m++;
						}
					}
					if(!(m&3))
						pDecLocal->b8type[block8x8]+= VO_16x16 - VO_8x8;
					n+=m;
				}
			}
		}
		if(!is_8x8 && !(n&15))
			*mb_type= (*mb_type & ~(VO_8x8|VO_16x8|VO_8x16|VO_P1L0|VO_P1L1))|VO_16x16|VO_DIRECT;
	}
	return 1;
}



#endif

int get_colocated_info_4x4(StorablePicture *list1,int i1, int j1,H264DEC_L *pDecLocal)
{
  if (list1->is_long_term)
    return 1;
  else
  {
    //PicMotionParams *fs = &list1->mv_cache[pDecLocal->block_xy+j1*4+i1];
	VO_S32 block_xy = pDecLocal->mb_xy*16;
	VO_S8 ref_idx_list0 = list1->ref_idx[0][block_xy+j1*4+i1];
	VO_S8 ref_idx_list1 = list1->ref_idx[1][block_xy+j1*4+i1];
	VO_S16 *mv_list0 = (VO_S16*)&list1->mv[0][block_xy+j1*4+i1];
	VO_S16 *mv_list1 = (VO_S16*)&list1->mv[1][block_xy+j1*4+i1];
	
	int moving = !(((
          (ref_idx_list0 == 0)
      &&  (iabs(mv_list0[0])>>1 == 0)
      &&  (iabs(mv_list0[1])>>1 == 0)))
      || ((ref_idx_list0 <= -1)
      &&  (ref_idx_list1 == 0)
      &&  (iabs(mv_list1[0])>>1 == 0)
      &&  (iabs(mv_list1[1])>>1 == 0)));
    return moving;  
  }
}

int get_colocated_info_8x8(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, StorablePicture *list1,int i1, int j1)
{
  if (list1->is_long_term)
    return 1;
  else
  {
    Slice *currSlice = pDecLocal->p_Slice;
	
    if( (currSlice->mb_aff_frame_flag) ||
      (!currSlice->active_sps->frame_mbs_only_flag && ((currSlice->structure==FRAME && list1->iCodingType == FIELD_CODING)||(currSlice->structure!=list1->structure && list1->coded_frame))))
    {
      int moving;
	  int mb_x = pDecLocal->mb.x;
	  int mb_y = pDecLocal->mb.y;
	  int block_xy = pDecLocal->mb_field?((mb_y>>1)*pDecGlobal->PicWidthInMbs+mb_x)*16:(mb_y*pDecGlobal->PicWidthInMbs+mb_x)*16;
      VO_S32 index = block_xy+RSD(j1)*4+RSD(i1);
      VO_S32 ref_idx_list0 = list1->ref_idx[0][index];
	  VO_S32 ref_idx_list1 = list1->ref_idx[1][index];
	  VO_S16* mv_list0 = (VO_S16*)&list1->mv[0][index];
	  VO_S16* mv_list1 = (VO_S16*)&list1->mv[1][index];
      if(currSlice->field_pic_flag && currSlice->structure!=list1->structure && list1->coded_frame)
      {
         if(currSlice->structure == TOP_FIELD)
         {
           //fs = list1->top_field->mv_info[jj] + ii;
		   ref_idx_list0 = list1->top_field->ref_idx[0][index];
	  	   ref_idx_list1 = list1->top_field->ref_idx[1][index];
	       mv_list0 = (VO_S16*)&list1->top_field->mv[0][index];
	       mv_list1 = (VO_S16*)&list1->top_field->mv[1][index];
         }
         else
         {
           //fs = list1->bottom_field->mv_info[jj] + ii;
		   ref_idx_list0 = list1->bottom_field->ref_idx[0][index];
	  	   ref_idx_list1 = list1->bottom_field->ref_idx[1][index];
	       mv_list0 = (VO_S16*)&list1->bottom_field->mv[0][index];
	       mv_list1 = (VO_S16*)&list1->bottom_field->mv[1][index];
         }
      }
      else
      {
        if( (currSlice->mb_aff_frame_flag && ((!pDecLocal->mb_field && IS_INTERLACED(list1->mb_type_buffer[pDecLocal->mb_xy])) ||
          (!pDecLocal->mb_field && list1->iCodingType == FIELD_CODING))) 
          || (!currSlice->mb_aff_frame_flag))
        {
          int mb_x = pDecLocal->mb.x;
		  int mb_y = pDecLocal->mb.y;
		  int block_xy = ((mb_y>>1)*pDecGlobal->PicWidthInMbs+mb_x)*16;
          index = (block_xy)+(j1)*2+RSD(i1);
		  index += (mb_y&1)*8;
          if (iabs(pDecGlobal->dec_picture->poc - list1->bottom_field->poc)> iabs(pDecGlobal->dec_picture->poc -list1->top_field->poc) )
          {
            //fs = list1->top_field->mv_info[jdiv] + ii;
            ref_idx_list0 = list1->top_field->ref_idx[0][index];
	  	    ref_idx_list1 = list1->top_field->ref_idx[1][index];
	        mv_list0 = (VO_S16*)&list1->top_field->mv[0][index];
	        mv_list1 = (VO_S16*)&list1->top_field->mv[1][index];
          }
          else
          {
            //fs = list1->bottom_field->mv_info[jdiv] + ii;
            ref_idx_list0 = list1->bottom_field->ref_idx[0][index];
	  	    ref_idx_list1 = list1->bottom_field->ref_idx[1][index];
	        mv_list0 = (VO_S16*)&list1->bottom_field->mv[0][index];
	        mv_list1 = (VO_S16*)&list1->bottom_field->mv[1][index];
          }
        }
      }
	  moving= !((((ref_idx_list0 == 0)
        &&  (iabs(mv_list0[0])>>1 == 0)
        &&  (iabs(mv_list0[1])>>1 == 0)))
        || ((ref_idx_list0 <= -1)
        &&  (ref_idx_list1 == 0)
        &&  (iabs(mv_list1[0])>>1 == 0)
        &&  (iabs(mv_list1[1])>>1 == 0)));
      return moving;  
    }
	else
    {
      VO_S32 index = pDecLocal->mb_xy*16+RSD(j1)*4+RSD(i1);
	  VO_S32 ref_idx_list0 = list1->ref_idx[0][index];
	  VO_S32 ref_idx_list1 = list1->ref_idx[1][index];
	  VO_S16* mv_list0 = (VO_S16*)&list1->mv[0][index];
	  VO_S16* mv_list1 = (VO_S16*)&list1->mv[1][index];
	  int moving= !((((ref_idx_list0 == 0)
        &&  (iabs(mv_list0[0])>>1 == 0)
        &&  (iabs(mv_list0[1])>>1 == 0)))
        || ((ref_idx_list0 <= -1)
        &&  (ref_idx_list1 == 0)
        &&  (iabs(mv_list1[0])>>1 == 0)
        &&  (iabs(mv_list1[1])>>1 == 0)));
      return moving;  
    }
  }
}



void update_direct_types(H264DEC_G *pDecGlobal,Slice *currSlice)
{
  if(currSlice->direct_spatial_mv_pred_flag)
  {
    pDecGlobal->update_direct_mv_info = pDecGlobal->interlace?update_direct_mv_info_spatial_interlace:update_direct_mv_info_spatial;
	pDecGlobal->mb_pred_b = pDecGlobal->interlace?mb_pred_b_spatial_interlace:mb_pred_b_spatial;
  }
  else
  {
    pDecGlobal->update_direct_mv_info = pDecGlobal->interlace?update_direct_mv_info_temporal_interlace:update_direct_mv_info_temporal;
	pDecGlobal->mb_pred_b = pDecGlobal->interlace?mb_pred_b_temporal_interlace:mb_pred_b_temporal;
  }
}


