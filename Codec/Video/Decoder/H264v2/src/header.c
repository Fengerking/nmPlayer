#include "global.h"
#include "fmo.h"
#include "vlc.h"
#include "mbuffer.h"
#include "header.h"

#include "ctx_tables.h"


static VO_S32 ref_pic_list_reordering(H264DEC_G *pDecGlobal,Slice *currSlice);
static void pred_weight_table(H264DEC_G *pDecGlobal,Slice *currSlice);
#if (MVC_EXTENSION_ENABLE)
static VO_S32 ref_pic_list_mvc_modification(H264DEC_G *pDecGlobal,Slice *currSlice);
#endif

#if(FAST_FILTER)
#define VOMAX(a,b) ((a) > (b) ? (a) : (b))
#define VOMAX3(a,b,c) VOMAX(VOMAX(a,b),c)
#define VOMIN(a,b) ((a) > (b) ? (b) : (a))
#define VOMIN3(a,b,c) VOMIN(VOMIN(a,b),c)
#endif

unsigned CeilLog2( unsigned uiVal)
{
  unsigned uiTmp = uiVal-1;
  unsigned uiRet = 0;

  while( uiTmp != 0 )
  {
    uiTmp >>= 1;
    uiRet++;
  }
  return uiRet;
}

unsigned CeilLog2_sf( unsigned uiVal)
{
  unsigned uiTmp = uiVal-1;
  unsigned uiRet = 0;

  while( uiTmp > 0 )
  {
    uiTmp >>= 1;
    uiRet++;
  }
  return uiRet;
}

int FirstPartOfSliceHeader(H264DEC_G *pDecGlobal,Slice *currSlice)
{
  //VideoParameters *p_Vid = currSlice->p_Vid;
  //byte dP_nr = assignSE2partition[currSlice->dp_mode][SE_HEADER];
  //DataPartition *partition = &(currSlice->partArr[dP_nr]);
  //Bitstream *currStream = partition->bitstream;
  int tmp;

  //p_Dec->UsedBits= partition->bitstream->frame_bitoffset; // was hardcoded to 31 for previous start-code. This is better.

  // Get first_mb_in_slice
  currSlice->start_mb_nr = ue_v (pDecGlobal);//"SH: first_mb_in_slice"
  

  tmp = ue_v (pDecGlobal);//"SH: slice_type"

  if (tmp > 4) tmp -= 5;

  if(tmp > 2)
  	VOH264ERROR(VO_H264_ERR_SLICE_TYPE);

  pDecGlobal->type = currSlice->slice_type = (SliceType) tmp;

  currSlice->pic_parameter_set_id = ue_v (pDecGlobal);//"SH: pic_parameter_set_id"
  if(currSlice->pic_parameter_set_id>255||currSlice->pic_parameter_set_id<0)
  {
    currSlice->pic_parameter_set_id = 0;
  	VOH264ERROR(VO_H264_ERR_SLICE_HEADER);
  }

    currSlice->colour_plane_id = PLANE_Y;

  return 0;//p_Dec->UsedBits;
}

int RestOfSliceHeader(H264DEC_G *pDecGlobal,Slice *currSlice)
{
  //VideoParameters *p_Vid = currSlice->p_Vid;
  seq_parameter_set_rbsp_t *active_sps = currSlice->active_sps;

  int val, len,ret;

  currSlice->frame_num = u_v (active_sps->log2_max_frame_num_minus4 + 4, pDecGlobal);//"SH: frame_num"

  /* Tian Dong: frame_num gap processing, if found */
  if(currSlice->idr_flag) //if (p_Vid->idr_flag)
  {
    pDecGlobal->pre_frame_num = currSlice->frame_num;
    // picture error concealment
    //p_Vid->last_ref_pic_poc = 0;
    if(currSlice->frame_num!=0)
	  VOH264ERROR(VO_H264_ERR_SLICE_HEADER);
  }

  if (active_sps->frame_mbs_only_flag)
  {
    pDecGlobal->structure = FRAME;
    currSlice->field_pic_flag=0;
  }
  else
  {
    // field_pic_flag   u(1)
    currSlice->field_pic_flag = u_v (1, pDecGlobal);//"SH: field_pic_flag"
    if (currSlice->field_pic_flag)
    {
      // bottom_field_flag  u(1)
      currSlice->bottom_field_flag = (byte) u_v (1, pDecGlobal);//"SH: bottom_field_flag"
      pDecGlobal->structure = currSlice->bottom_field_flag ? BOTTOM_FIELD : TOP_FIELD;
    }
    else
    {
      pDecGlobal->structure = FRAME;
      currSlice->bottom_field_flag = FALSE;
    }
  }

  currSlice->structure = (PictureStructure) pDecGlobal->structure;

  currSlice->mb_aff_frame_flag = (active_sps->mb_adaptive_frame_field_flag && (currSlice->field_pic_flag==0));
  //currSlice->mb_aff_frame_flag = p_Vid->mb_aff_frame_flag;

  if (pDecGlobal->structure == FRAME &&  currSlice->field_pic_flag != 0    ) 
    VOH264ERROR(VO_H264_ERR_PIC_STRUCT);
  if (pDecGlobal->structure == TOP_FIELD &&(currSlice->field_pic_flag != 1||currSlice->bottom_field_flag != FALSE )  ) 
    VOH264ERROR(VO_H264_ERR_PIC_STRUCT);
  if (pDecGlobal->structure == BOTTOM_FIELD&&(currSlice->field_pic_flag != 1||currSlice->bottom_field_flag == FALSE)) 
    VOH264ERROR(VO_H264_ERR_PIC_STRUCT);

  if (currSlice->idr_flag)
  {
    currSlice->idr_pic_id = ue_v(pDecGlobal);//"SH: idr_pic_id"
    if(currSlice->idr_pic_id<0)
	  VOH264ERROR(VO_H264_ERR_SLICE_HEADER); 
  }

  if (active_sps->pic_order_cnt_type == 0)
  {
    currSlice->pic_order_cnt_lsb = u_v(active_sps->log2_max_pic_order_cnt_lsb_minus4 + 4, pDecGlobal);//"SH: pic_order_cnt_lsb"
    if( currSlice->active_pps->bottom_field_pic_order_in_frame_present_flag  ==  1 &&  !currSlice->field_pic_flag )
      currSlice->delta_pic_order_cnt_bottom = se_v(pDecGlobal);//"SH: delta_pic_order_cnt_bottom"
    else
      currSlice->delta_pic_order_cnt_bottom = 0;
  }
  
  if( active_sps->pic_order_cnt_type == 1)
  {
    if ( !active_sps->delta_pic_order_always_zero_flag )
    {
      currSlice->delta_pic_order_cnt[ 0 ] = se_v(pDecGlobal);//"SH: delta_pic_order_cnt[0]"
      if( currSlice->active_pps->bottom_field_pic_order_in_frame_present_flag  ==  1  &&  !currSlice->field_pic_flag )
        currSlice->delta_pic_order_cnt[ 1 ] = se_v(pDecGlobal);//"SH: delta_pic_order_cnt[1]"
      else
        currSlice->delta_pic_order_cnt[ 1 ] = 0;  // set to zero if not in stream
    }
    else
    {
      currSlice->delta_pic_order_cnt[ 0 ] = 0;
      currSlice->delta_pic_order_cnt[ 1 ] = 0;
    }
  }

  //! redundant_pic_cnt is missing here
  if (currSlice->active_pps->redundant_pic_cnt_present_flag)
  {
    currSlice->redundant_pic_cnt = ue_v (pDecGlobal);//"SH: redundant_pic_cnt"
    if(currSlice->redundant_pic_cnt<0)
	  VOH264ERROR(VO_H264_ERR_SLICE_HEADER); 
  }

  if(currSlice->slice_type == B_SLICE)
  {
    currSlice->direct_spatial_mv_pred_flag = u_v (1, pDecGlobal);//"SH: direct_spatial_mv_pred_flag"
  }

  currSlice->num_ref_idx_active[LIST_0] = currSlice->active_pps->num_ref_idx_l0_active_minus1 + 1;
  currSlice->num_ref_idx_active[LIST_1] = currSlice->active_pps->num_ref_idx_l1_active_minus1 + 1;
  if(currSlice->num_ref_idx_active[LIST_0]<1||currSlice->num_ref_idx_active[LIST_0]>MAX_REFERENCE_FRAMES)
	VOH264ERROR(VO_H264_ERR_NUMREFFRAME);
  if(currSlice->num_ref_idx_active[LIST_1]<1||currSlice->num_ref_idx_active[LIST_1]>MAX_REFERENCE_FRAMES)
	VOH264ERROR(VO_H264_ERR_NUMREFFRAME);
  if(pDecGlobal->type==P_SLICE || pDecGlobal->type == SP_SLICE || pDecGlobal->type==B_SLICE)
  {
    val = u_v (1, pDecGlobal);//"SH: num_ref_idx_override_flag"
    if (val)
    {
      currSlice->num_ref_idx_active[LIST_0] = 1 + ue_v (pDecGlobal);//"SH: num_ref_idx_l0_active_minus1"
      if(currSlice->num_ref_idx_active[LIST_0]<1||currSlice->num_ref_idx_active[LIST_0]>MAX_REFERENCE_FRAMES)
	    VOH264ERROR(VO_H264_ERR_NUMREFFRAME);
      if(pDecGlobal->type==B_SLICE)
      {
        currSlice->num_ref_idx_active[LIST_1] = 1 + ue_v (pDecGlobal);//"SH: num_ref_idx_l1_active_minus1"
        if(currSlice->num_ref_idx_active[LIST_1]<1||currSlice->num_ref_idx_active[LIST_1]>MAX_REFERENCE_FRAMES)
	     VOH264ERROR(VO_H264_ERR_NUMREFFRAME);
      }
    }
  }
  if (currSlice->slice_type!=B_SLICE)
  {
    currSlice->num_ref_idx_active[LIST_1] = 0;
  }

#if (MVC_EXTENSION_ENABLE)
  if (currSlice->svc_extension_flag == 0 || currSlice->svc_extension_flag == 1)
  {
    ret = ref_pic_list_mvc_modification(pDecGlobal,currSlice);
	if(ret)
	 return ret;
  }
  else
  {
    ret = ref_pic_list_reordering(pDecGlobal,currSlice);
	if(ret)
	 return ret;
  }
#else
  ret = ref_pic_list_reordering(pDecGlobal,currSlice);
  if(ret)
	 return ret;
#endif

  currSlice->weighted_pred_flag = (unsigned short) ((currSlice->slice_type == P_SLICE || currSlice->slice_type == SP_SLICE) 
    ? currSlice->active_pps->weighted_pred_flag 
    : (currSlice->slice_type == B_SLICE && currSlice->active_pps->weighted_bipred_idc == 1));
  currSlice->weighted_bipred_idc = (unsigned short) (currSlice->slice_type == B_SLICE && currSlice->active_pps->weighted_bipred_idc > 0);

  if ((currSlice->active_pps->weighted_pred_flag&&(pDecGlobal->type==P_SLICE|| pDecGlobal->type == SP_SLICE))||
      (currSlice->active_pps->weighted_bipred_idc==1 && (pDecGlobal->type==B_SLICE)))
  {
    pred_weight_table(pDecGlobal,currSlice);
  }

  if (currSlice->nal_reference_idc)
  {
    ret = dec_ref_pic_marking(pDecGlobal, currSlice);
	if(ret)
	  return ret;
  }

  if (currSlice->active_pps->entropy_coding_mode_flag && pDecGlobal->type!=I_SLICE && pDecGlobal->type!=SI_SLICE)
  {
    currSlice->model_number = ue_v(pDecGlobal);//"SH: cabac_init_idc"
    if(currSlice->model_number>=3||currSlice->model_number<0)
	  VOH264ERROR(VO_H264_ERR_SLICE_HEADER);
  }
  else
  {
    currSlice->model_number = 0;
  }
  
  currSlice->slice_qp_delta = val = se_v(pDecGlobal);//"SH: slice_qp_delta"
  //currSlice->qp = p_Vid->qp = 26 + p_Vid->active_pps->pic_init_qp_minus26 + val;
  currSlice->qp = 26 + currSlice->active_pps->pic_init_qp_minus26 + val;

  if ((currSlice->qp < 0) || (currSlice->qp > 51))
	currSlice->qp = 26;//warning dtt

  if(pDecGlobal->type==SP_SLICE || pDecGlobal->type == SI_SLICE)
  {
    if(pDecGlobal->type==SP_SLICE)
    {
      currSlice->sp_switch = u_v (1, pDecGlobal);//"SH: sp_for_switch_flag"
    }
    currSlice->slice_qs_delta = val = se_v(pDecGlobal);//"SH: slice_qs_delta"
    currSlice->qs = 26 + currSlice->active_pps->pic_init_qs_minus26 + val;    
    if ((currSlice->qs < 0) || (currSlice->qs > 51))
      currSlice->qs = 26;//warning dtt
  }

  if ( !HI_INTRA_ONLY_PROFILE)
  //then read flags and parameters from bistream
  {
    if (currSlice->active_pps->deblocking_filter_control_present_flag)
    {
      pDecGlobal->DFDisableIdc = (short) ue_v (pDecGlobal);//"SH: disable_deblocking_filter_idc"
      if(pDecGlobal->DFDisableIdc>2||pDecGlobal->DFDisableIdc<0)
        VOH264ERROR(VO_H264_ERR_SLICE_HEADER);
      if (pDecGlobal->DFDisableIdc!=1)
      {
        pDecGlobal->DFAlphaC0Offset = (short) (2 * se_v(pDecGlobal));//"SH: slice_alpha_c0_offset_div2"
        pDecGlobal->DFBetaOffset    = (short) (2 * se_v(pDecGlobal));//"SH: slice_beta_offset_div2"
        if(pDecGlobal->DFAlphaC0Offset>52||pDecGlobal->DFBetaOffset>52)
		  VOH264ERROR(VO_H264_ERR_SLICE_HEADER);
      }
      else
      {
        pDecGlobal->DFAlphaC0Offset = pDecGlobal->DFBetaOffset = 0;
      }
    }
    else
    {
      pDecGlobal->DFDisableIdc = pDecGlobal->DFAlphaC0Offset = pDecGlobal->DFBetaOffset = 0;
    }
  }
  else //By default the Loop Filter is Off
  { //444_TEMP_NOTE: change made below. 08/07/07
    //still need to parse the SEs (read flags and parameters from bistream) but will ignore
    if (currSlice->active_pps->deblocking_filter_control_present_flag)
    {
      pDecGlobal->DFDisableIdc = (short) ue_v (pDecGlobal);//"SH: disable_deblocking_filter_idc"
      if(pDecGlobal->DFDisableIdc>2||pDecGlobal->DFDisableIdc<0)
        VOH264ERROR(VO_H264_ERR_SLICE_HEADER);
      if (pDecGlobal->DFDisableIdc!=1)
      {
        pDecGlobal->DFAlphaC0Offset = (short) (2 * se_v(pDecGlobal));//"SH: slice_alpha_c0_offset_div2"
        pDecGlobal->DFBetaOffset    = (short) (2 * se_v(pDecGlobal));//"SH: slice_beta_offset_div2"
        if(pDecGlobal->DFAlphaC0Offset>52||pDecGlobal->DFBetaOffset>52)
		  VOH264ERROR(VO_H264_ERR_SLICE_HEADER);
      }
    }//444_TEMP_NOTE. the end of change. 08/07/07
    //Ignore the SEs, by default the Loop Filter is Off
    pDecGlobal->DFDisableIdc =1;
    pDecGlobal->DFAlphaC0Offset = pDecGlobal->DFBetaOffset = 0;
  }
#if(FAST_FILTER)
	pDecGlobal->DFQPThresh = 15  - VOMIN(pDecGlobal->DFAlphaC0Offset,  pDecGlobal->DFBetaOffset) -
							 VOMAX3(0, currSlice->active_pps->chroma_qp_index_offset, 
							 currSlice->active_pps->second_chroma_qp_index_offset);
#endif


  if (currSlice->active_pps->num_slice_groups_minus1>0 && currSlice->active_pps->slice_group_map_type>=3 &&
      currSlice->active_pps->slice_group_map_type<=5)
  {
    len = (active_sps->pic_height_in_map_units_minus1+1)*(active_sps->pic_width_in_mbs_minus1+1)/
          (currSlice->active_pps->slice_group_change_rate_minus1+1);
    if (((active_sps->pic_height_in_map_units_minus1+1)*(active_sps->pic_width_in_mbs_minus1+1))%
          (currSlice->active_pps->slice_group_change_rate_minus1+1))
          len +=1;

    len = CeilLog2(len+1);

    currSlice->slice_group_change_cycle = u_v (len, pDecGlobal);//"SH: slice_group_change_cycle"
  }
  pDecGlobal->PicHeightInMbs = pDecGlobal->FrameHeightInMbs / ( 1 + currSlice->field_pic_flag );
  pDecGlobal->PicSizeInMbs   = pDecGlobal->PicWidthInMbs * pDecGlobal->PicHeightInMbs;
  //pDecGlobal->FrameSizeInMbs = pDecGlobal->PicWidthInMbs * pDecGlobal->FrameHeightInMbs;

  return 0;//p_Dec->UsedBits;
}


static VO_S32 ref_pic_list_reordering(H264DEC_G *pDecGlobal,Slice *currSlice)
{

  int i, val;


  if (pDecGlobal->type!=I_SLICE && pDecGlobal->type!=SI_SLICE)
  {
    val = currSlice->ref_pic_list_reordering_flag[LIST_0] = u_v (1, pDecGlobal);//"SH: ref_pic_list_reordering_flag_l0"

    if (val)
    {
      i=0;
      do
      {
        val = currSlice->reordering_of_pic_nums_idc[LIST_0][i] = ue_v(pDecGlobal);//"SH: reordering_of_pic_nums_idc_l0"
        if (val==0 || val==1)
        {
          currSlice->abs_diff_pic_num_minus1[LIST_0][i] = ue_v(pDecGlobal);//"SH: abs_diff_pic_num_minus1_l0"
        }
        else
        {
          if (val==2)
          {
            currSlice->long_term_pic_idx[LIST_0][i] = ue_v(pDecGlobal);//"SH: long_term_pic_idx_l0"
          }
        }
        i++;
		if((val>>2)||i>17)
		  VOH264ERROR(VO_H264_ERR_LIST_REORDERING);
      } while (val != 3);
    }
  }

  if (pDecGlobal->type==B_SLICE)
  {
    val = currSlice->ref_pic_list_reordering_flag[LIST_1] = u_v (1, pDecGlobal);//"SH: ref_pic_list_reordering_flag_l1"

    if (val)
    {
      i=0;
      do
      {
        val = currSlice->reordering_of_pic_nums_idc[LIST_1][i] = ue_v(pDecGlobal);//"SH: reordering_of_pic_nums_idc_l1"
        if (val==0 || val==1)
        {
          currSlice->abs_diff_pic_num_minus1[LIST_1][i] = ue_v(pDecGlobal);//"SH: abs_diff_pic_num_minus1_l1"
        }
        else
        {
          if (val==2)
          {
            currSlice->long_term_pic_idx[LIST_1][i] = ue_v(pDecGlobal);//"SH: long_term_pic_idx_l1"
          }
        }
        i++;
		if((val>>2)||i>17)
		  VOH264ERROR(VO_H264_ERR_LIST_REORDERING);
        // assert (i>currSlice->num_ref_idx_active[LIST_1]);
      } while (val != 3);
    }
  }

  // set reference index of redundant slices.
  if(currSlice->redundant_pic_cnt && (pDecGlobal->type != I_SLICE) )
  {
    currSlice->redundant_slice_ref_idx = currSlice->abs_diff_pic_num_minus1[LIST_0][0] + 1;
  }
  
  return 0;
}


static void reset_wp_params(Slice *currSlice)
{
  int i,comp;
  int log_weight_denom;

  for (i=0; i<MAX_REFERENCE_PICTURES; i++)
  {
    for (comp=0; comp<3; comp++)
    {
      log_weight_denom = (comp == 0) ? currSlice->luma_log2_weight_denom : currSlice->chroma_log2_weight_denom;
      currSlice->wp_weight[0][i][comp] = 1 << log_weight_denom;
      currSlice->wp_weight[1][i][comp] = 1 << log_weight_denom;
    }
  }
}

/*!
 ************************************************************************
 * \brief
 *    read the weighted prediction tables
 ************************************************************************
 */
static void pred_weight_table(H264DEC_G *pDecGlobal,Slice *currSlice)
{
  seq_parameter_set_rbsp_t *active_sps = currSlice->active_sps;
  //byte dP_nr = assignSE2partition[currSlice->dp_mode][SE_HEADER];
  int luma_weight_flag_l0, luma_weight_flag_l1, chroma_weight_flag_l0, chroma_weight_flag_l1;
  int i,j;

  currSlice->luma_log2_weight_denom = (unsigned short) ue_v (pDecGlobal);//"SH: luma_log2_weight_denom"
  currSlice->wp_round_luma = currSlice->luma_log2_weight_denom ? 1<<(currSlice->luma_log2_weight_denom - 1): 0;

  if ( 0 != active_sps->chroma_format_idc)
  {
    currSlice->chroma_log2_weight_denom = (unsigned short) ue_v (pDecGlobal);//"SH: chroma_log2_weight_denom"
    currSlice->wp_round_chroma = currSlice->chroma_log2_weight_denom ? 1<<(currSlice->chroma_log2_weight_denom - 1): 0;
  }

  reset_wp_params(currSlice);

  for (i=0; i<currSlice->num_ref_idx_active[LIST_0]; i++)
  {
    luma_weight_flag_l0 = u_v (1, pDecGlobal);//"SH: luma_weight_flag_l0"

    if (luma_weight_flag_l0)
    {
      currSlice->wp_weight[0][i][0] = se_v (pDecGlobal);//"SH: luma_weight_l0"
      currSlice->wp_offset[0][i][0] = se_v (pDecGlobal);//"SH: luma_offset_l0"
	  currSlice->weighted_flag[0][i][0] = 1;
	  //printf("wp_weight:%d wp_offset%d denom:%d\n",currSlice->wp_weight[0][i][0],currSlice->wp_offset[0][i][0],currSlice->luma_log2_weight_denom);
    }
    else
    {
      currSlice->wp_weight[0][i][0] = 1 << currSlice->luma_log2_weight_denom;
      currSlice->wp_offset[0][i][0] = 0;
    }

    if (active_sps->chroma_format_idc != 0)
    {
      chroma_weight_flag_l0 = u_v (1, pDecGlobal);//"SH: chroma_weight_flag_l0"

      for (j=1; j<3; j++)
      {
        if (chroma_weight_flag_l0)
        {
          currSlice->wp_weight[0][i][j] = se_v(pDecGlobal);//"SH: chroma_weight_l0"
          currSlice->wp_offset[0][i][j] = se_v(pDecGlobal);//"SH: chroma_offset_l0"
		  currSlice->weighted_flag[0][i][j]= 1;
		  //printf("wp_weight:%d wp_offset%d denom:%d\n",currSlice->wp_weight[0][i][j],currSlice->wp_offset[0][i][j],currSlice->chroma_log2_weight_denom);
        }
        else
        {
          currSlice->wp_weight[0][i][j] = 1<<currSlice->chroma_log2_weight_denom;
          currSlice->wp_offset[0][i][j] = 0;
        }
      }
    }
  }
  if ((pDecGlobal->type == B_SLICE) && currSlice->active_pps->weighted_bipred_idc == 1)
  {
    for (i=0; i<currSlice->num_ref_idx_active[LIST_1]; i++)
    {
      luma_weight_flag_l1 = u_v (1, pDecGlobal);//"SH: luma_weight_flag_l1"

      if (luma_weight_flag_l1)
      {
        currSlice->wp_weight[1][i][0] = se_v (pDecGlobal);//"SH: luma_weight_l1"
        currSlice->wp_offset[1][i][0] = se_v (pDecGlobal);//"SH: luma_offset_l1"
		currSlice->weighted_flag[1][i][0]= 1;
      }
      else
      {
        currSlice->wp_weight[1][i][0] = 1<<currSlice->luma_log2_weight_denom;
        currSlice->wp_offset[1][i][0] = 0;
      }

      if (active_sps->chroma_format_idc != 0)
      {
        chroma_weight_flag_l1 = u_v (1, pDecGlobal);//"SH: chroma_weight_flag_l1"

        for (j=1; j<3; j++)
        {
          if (chroma_weight_flag_l1)
          {
            currSlice->wp_weight[1][i][j] = se_v(pDecGlobal);//"SH: chroma_weight_l1"
            currSlice->wp_offset[1][i][j] = se_v(pDecGlobal);//"SH: chroma_offset_l1"
			currSlice->weighted_flag[1][i][j]= 1;
		  }
          else
          {
            currSlice->wp_weight[1][i][j] = 1<<currSlice->chroma_log2_weight_denom;
            currSlice->wp_offset[1][i][j] = 0;
          }
        }
      }
    }
  }
}

VO_S32 dec_ref_pic_marking(H264DEC_G *pDecGlobal, Slice *pSlice)
{
  int val;

  DecRefPicMarking_t *tmp_drpm;
  VO_U32 newBuf = 0;
  // voFree old buffer content
  tmp_drpm=pDecGlobal->dec_ref_pic_marking_buffer;
  memset(tmp_drpm,0,sizeof(DecRefPicMarking_t));

  if (pSlice->idr_flag)
  {
    pSlice->no_output_of_prior_pics_flag = u_v (1, pDecGlobal);//"SH: no_output_of_prior_pics_flag"
    pDecGlobal->no_output_of_prior_pics_flag = pSlice->no_output_of_prior_pics_flag;
    pSlice->long_term_reference_flag = u_v (1, pDecGlobal);//"SH: long_term_reference_flag"
  }
  else
  {
    pSlice->adaptive_ref_pic_buffering_flag = u_v (1, pDecGlobal);//"SH: adaptive_ref_pic_buffering_flag"
    if (pSlice->adaptive_ref_pic_buffering_flag)
    {
      // read Memory Management Control Operation
      do
      {
        //tmp_drpm=(DecRefPicMarking_t*)voMalloc(pDecGlobal,sizeof (DecRefPicMarking_t));
        //tmp_drpm->Next=NULL;

        val = tmp_drpm->memory_management_control_operation = ue_v(pDecGlobal);//"SH: memory_management_control_operation"
		if(val==0)
			break;
        if(val<0||val>6)
		  VOH264ERROR(VO_H264_ERR_INVALIDMCO);
        if ((val==1)||(val==3))
        {
          tmp_drpm->difference_of_pic_nums_minus1 = ue_v(pDecGlobal);//"SH: difference_of_pic_nums_minus1"
        }
        if (val==2)
        {
          tmp_drpm->long_term_pic_num = ue_v(pDecGlobal);//"SH: long_term_pic_num"
        }

        if ((val==3)||(val==6))
        {
          tmp_drpm->long_term_frame_idx = ue_v(pDecGlobal);//"SH: long_term_frame_idx"
        }
        if (val==4)
        {
          tmp_drpm->max_long_term_frame_idx_plus1 = ue_v(pDecGlobal);//"SH: max_long_term_pic_idx_plus1"
        }

        tmp_drpm->Next = tmp_drpm+1;
   		tmp_drpm++;
		if(++newBuf>=MAX_REFERENCE_FRAMES)
		{
		  VOH264ERROR(VO_H264_ERR_INVALIDMCO);
		}
		memset(tmp_drpm,0,sizeof(DecRefPicMarking_t));   

      }
      while (1);
    }
  } 
  return 0;
}

VO_S32 decode_poc(H264DEC_G *pDecGlobal, Slice *pSlice)
{
  seq_parameter_set_rbsp_t *active_sps = pSlice->active_sps;
  int i;
  // for POC mode 0:
  unsigned int MaxPicOrderCntLsb = (1<<(active_sps->log2_max_pic_order_cnt_lsb_minus4+4));

  switch ( active_sps->pic_order_cnt_type )
  {
  case 0: // POC MODE 0
    // 1st
    if(pSlice->idr_flag)
    {
      pDecGlobal->PrevPicOrderCntMsb = 0;
      pDecGlobal->PrevPicOrderCntLsb = 0;
    }
    else
    {
      if (pDecGlobal->last_has_mmco_5)
      {
        if (pDecGlobal->last_pic_bottom_field)
        {
          pDecGlobal->PrevPicOrderCntMsb = 0;
          pDecGlobal->PrevPicOrderCntLsb = 0;
        }
        else
        {
          pDecGlobal->PrevPicOrderCntMsb = 0;
          pDecGlobal->PrevPicOrderCntLsb = pSlice->toppoc;
        }
      }
    }
    // Calculate the MSBs of current picture
    if( pSlice->pic_order_cnt_lsb  <  pDecGlobal->PrevPicOrderCntLsb  &&
      ( pDecGlobal->PrevPicOrderCntLsb - pSlice->pic_order_cnt_lsb )  >=  ( MaxPicOrderCntLsb / 2 ) )
      pSlice->PicOrderCntMsb = pDecGlobal->PrevPicOrderCntMsb + MaxPicOrderCntLsb;
    else if ( pSlice->pic_order_cnt_lsb  >  pDecGlobal->PrevPicOrderCntLsb  &&
      ( pSlice->pic_order_cnt_lsb - pDecGlobal->PrevPicOrderCntLsb )  >  ( MaxPicOrderCntLsb / 2 ) )
      pSlice->PicOrderCntMsb = pDecGlobal->PrevPicOrderCntMsb - MaxPicOrderCntLsb;
    else
      pSlice->PicOrderCntMsb = pDecGlobal->PrevPicOrderCntMsb;

    // 2nd

    if(pSlice->field_pic_flag==0)
    {           //frame pix
      pSlice->toppoc = pSlice->PicOrderCntMsb + pSlice->pic_order_cnt_lsb;
      pSlice->bottompoc = pSlice->toppoc + pSlice->delta_pic_order_cnt_bottom;
      pSlice->ThisPOC = pSlice->framepoc = (pSlice->toppoc < pSlice->bottompoc)? pSlice->toppoc : pSlice->bottompoc; // POC200301
    }
    else if (pSlice->bottom_field_flag == FALSE)
    {  //top field
      pSlice->ThisPOC= pSlice->toppoc = pSlice->PicOrderCntMsb + pSlice->pic_order_cnt_lsb;
    }
    else
    {  //bottom field
      pSlice->ThisPOC= pSlice->bottompoc = pSlice->PicOrderCntMsb + pSlice->pic_order_cnt_lsb;
    }
    pSlice->framepoc = pSlice->ThisPOC;

    //pDecGlobal->ThisPOC = pSlice->ThisPOC;

    if ( pSlice->frame_num != pDecGlobal->PreviousFrameNum)
      pDecGlobal->PreviousFrameNum = pSlice->frame_num;

    if(pSlice->nal_reference_idc)
    {
      pDecGlobal->PrevPicOrderCntLsb = pSlice->pic_order_cnt_lsb;
      pDecGlobal->PrevPicOrderCntMsb = pSlice->PicOrderCntMsb;
    }

    break;

  case 1: // POC MODE 1
    // 1st
    if(pSlice->idr_flag)
    {
      pDecGlobal->FrameNumOffset=0;     //  first pix of IDRGOP,
      if(pSlice->frame_num)
        VOH264ERROR(VO_H264_ERR_POC);
    }
    else
    {
      if (pDecGlobal->last_has_mmco_5)
      {
        pDecGlobal->PreviousFrameNumOffset = 0;
        pDecGlobal->PreviousFrameNum = 0;
      }
      if (pSlice->frame_num<pDecGlobal->PreviousFrameNum)
      {             //not first pix of IDRGOP
        pDecGlobal->FrameNumOffset = pDecGlobal->PreviousFrameNumOffset + pDecGlobal->MaxFrameNum;
      }
      else
      {
        pDecGlobal->FrameNumOffset = pDecGlobal->PreviousFrameNumOffset;
      }
    }

    // 2nd
    if(active_sps->num_ref_frames_in_pic_order_cnt_cycle)
      pSlice->AbsFrameNum = pDecGlobal->FrameNumOffset+pSlice->frame_num;
    else
      pSlice->AbsFrameNum=0;
    if( (!pSlice->nal_reference_idc) && pSlice->AbsFrameNum > 0)
      pSlice->AbsFrameNum--;

    // 3rd
    pDecGlobal->ExpectedDeltaPerPicOrderCntCycle=0;

    if(active_sps->num_ref_frames_in_pic_order_cnt_cycle)
    for(i=0;i<(int) active_sps->num_ref_frames_in_pic_order_cnt_cycle;i++)
      pDecGlobal->ExpectedDeltaPerPicOrderCntCycle += active_sps->offset_for_ref_frame[i];

    if(pSlice->AbsFrameNum)
    {
      pDecGlobal->PicOrderCntCycleCnt = (pSlice->AbsFrameNum-1)/active_sps->num_ref_frames_in_pic_order_cnt_cycle;
      pDecGlobal->FrameNumInPicOrderCntCycle = (pSlice->AbsFrameNum-1)%active_sps->num_ref_frames_in_pic_order_cnt_cycle;
      pDecGlobal->ExpectedPicOrderCnt = pDecGlobal->PicOrderCntCycleCnt*pDecGlobal->ExpectedDeltaPerPicOrderCntCycle;
      for(i=0;i<=(int)pDecGlobal->FrameNumInPicOrderCntCycle;i++)
        pDecGlobal->ExpectedPicOrderCnt += active_sps->offset_for_ref_frame[i];
    }
    else
      pDecGlobal->ExpectedPicOrderCnt=0;

    if(!pSlice->nal_reference_idc)
      pDecGlobal->ExpectedPicOrderCnt += active_sps->offset_for_non_ref_pic;

    if(pSlice->field_pic_flag==0)
    {           //frame pix
      pSlice->toppoc = pDecGlobal->ExpectedPicOrderCnt + pSlice->delta_pic_order_cnt[0];
      pSlice->bottompoc = pSlice->toppoc + active_sps->offset_for_top_to_bottom_field + pSlice->delta_pic_order_cnt[1];
      pSlice->ThisPOC = pSlice->framepoc = (pSlice->toppoc < pSlice->bottompoc)? pSlice->toppoc : pSlice->bottompoc; // POC200301
    }
    else if (pSlice->bottom_field_flag == FALSE)
    {  //top field
      pSlice->ThisPOC = pSlice->toppoc = pDecGlobal->ExpectedPicOrderCnt + pSlice->delta_pic_order_cnt[0];
    }
    else
    {  //bottom field
      pSlice->ThisPOC = pSlice->bottompoc = pDecGlobal->ExpectedPicOrderCnt + active_sps->offset_for_top_to_bottom_field + pSlice->delta_pic_order_cnt[0];
    }
    pSlice->framepoc=pSlice->ThisPOC;

    pDecGlobal->PreviousFrameNum=pSlice->frame_num;
    pDecGlobal->PreviousFrameNumOffset=pDecGlobal->FrameNumOffset;

    break;


  case 2: // POC MODE 2
    if(pSlice->idr_flag) // IDR picture
    {
      pDecGlobal->FrameNumOffset=0;     //  first pix of IDRGOP,
      pSlice->ThisPOC = pSlice->framepoc = pSlice->toppoc = pSlice->bottompoc = 0;
      if(pSlice->frame_num)
        VOH264ERROR(VO_H264_ERR_POC);
    }
    else
    {
      if (pDecGlobal->last_has_mmco_5)
      {
        pDecGlobal->PreviousFrameNum = 0;
        pDecGlobal->PreviousFrameNumOffset = 0;
      }
      if (pSlice->frame_num<pDecGlobal->PreviousFrameNum)
        pDecGlobal->FrameNumOffset = pDecGlobal->PreviousFrameNumOffset + pDecGlobal->MaxFrameNum;
      else
        pDecGlobal->FrameNumOffset = pDecGlobal->PreviousFrameNumOffset;


      pSlice->AbsFrameNum = pDecGlobal->FrameNumOffset+pSlice->frame_num;
      if(!pSlice->nal_reference_idc)
        pSlice->ThisPOC = (2*pSlice->AbsFrameNum - 1);
      else
        pSlice->ThisPOC = (2*pSlice->AbsFrameNum);

      if (pSlice->field_pic_flag==0)
        pSlice->toppoc = pSlice->bottompoc = pSlice->framepoc = pSlice->ThisPOC;
      else if (pSlice->bottom_field_flag == FALSE)
         pSlice->toppoc = pSlice->framepoc = pSlice->ThisPOC;
      else 
        pSlice->bottompoc = pSlice->framepoc = pSlice->ThisPOC;
    }

    pDecGlobal->PreviousFrameNum=pSlice->frame_num;
    pDecGlobal->PreviousFrameNumOffset=pDecGlobal->FrameNumOffset;
    break;


  default:
    VOH264ERROR(VO_H264_ERR_POC);
    break;
  }
  return 0;
}

int picture_order( Slice *pSlice )
{
  if (pSlice->field_pic_flag==0) // is a frame
    return pSlice->framepoc;
  else if (pSlice->bottom_field_flag == FALSE) // top field
    return pSlice->toppoc;
  else // bottom field
    return pSlice->bottompoc;
}

