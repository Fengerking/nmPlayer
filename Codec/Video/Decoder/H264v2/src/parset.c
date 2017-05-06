

#include "global.h"
#include "image.h"
#include "parsetcommon.h"
#include "parset.h"
#include "nalu.h"
#include "vlc.h"
#include "voH264Readbits.h"
#include "memalloc.h"
//#include "erc_api.h"


static const VO_U8 ZZ_SCAN[16]  =
{  
  0,  1,  4,  8,  5,  2,  3,  6,  
  9, 12, 13, 10,  7, 11, 14, 15
};

static const VO_U8 ZZ_SCAN8[64] =
{  
  0,  1,  8, 16,  9,  2,  3, 10, 
  17, 24, 32, 25, 18, 11,  4,  5,
  12, 19, 26, 33, 40, 48, 41, 34, 
  27, 20, 13,  6,  7, 14, 21, 28,
  35, 42, 49, 56, 57, 50, 43, 36, 
  29, 22, 15, 23, 30, 37, 44, 51,
  58, 59, 52, 45, 38, 31, 39, 46, 
  53, 60, 61, 54, 47, 55, 62, 63
};


// syntax for scaling list matrix values
void Scaling_List(H264DEC_G *pDecGlobal,int *scalingList, int sizeOfScalingList, Boolean *UseDefaultScalingMatrix)
{
  VO_S32 j, scanj;
  VO_S32 delta_scale, lastScale, nextScale;

  lastScale      = 8;
  nextScale      = 8;

  for(j=0; j<sizeOfScalingList; j++)
  {
    scanj = (sizeOfScalingList==16) ? ZZ_SCAN[j]:ZZ_SCAN8[j];

    if(nextScale!=0)
    {
      delta_scale = se_v (pDecGlobal);//"   : delta_sl   " 
      nextScale = (lastScale + delta_scale + 256) % 256;
      *UseDefaultScalingMatrix = (Boolean) (scanj==0 && nextScale==0);
    }

    scalingList[scanj] = (nextScale==0) ? lastScale:nextScale;
    lastScale = scalingList[scanj];
  }
}
// fill sps with content of p

int InterpretSPS (H264DEC_G *pDecGlobal, DataPartition *p, seq_parameter_set_rbsp_t *sps)
{
  unsigned i;
  unsigned n_ScalingList;
  int reserved_zero;
  VO_S32 ret;
  //p_Dec->UsedBits = 0;

  sps->profile_idc                            = u_v  (8,pDecGlobal);//"SPS: profile_idc"

  if ((sps->profile_idc!=BASELINE       ) &&
      (sps->profile_idc!=MAIN           ) &&
      (sps->profile_idc!=FREXT_HP       ) 
      )
  {
    VOH264ERROR(VO_H264_ERR_NotSupportProfile);
  }

  sps->constrained_set0_flag                  = u_v (1, pDecGlobal);//"SPS: constrained_set0_flag"
  sps->constrained_set1_flag                  = u_v (1, pDecGlobal);//"SPS: constrained_set1_flag"
  sps->constrained_set2_flag                  = u_v (1, pDecGlobal);//"SPS: constrained_set2_flag"
  sps->constrained_set3_flag                  = u_v (1, pDecGlobal);//"SPS: constrained_set3_flag"
#if (MVC_EXTENSION_ENABLE)
  sps->constrained_set4_flag                  = u_v (1, pDecGlobal);//"SPS: constrained_set4_flag"
  reserved_zero                               = u_v  (3,pDecGlobal);//"SPS: reserved_zero_3bits"
#else
  reserved_zero                               = u_v  (4,pDecGlobal);//"SPS: reserved_zero_4bits"
#endif
  if(reserved_zero)
  	VOH264ERROR(VO_H264_ERR_InvalidSPS);

  sps->level_idc                              = u_v  (8,pDecGlobal);//"SPS: level_idc"

  if( sps->level_idc != 9 &&
  	  sps->level_idc != 10 &&
	  sps->level_idc != 11 &&
	  sps->level_idc != 12 &&
	  sps->level_idc != 13 &&
	  sps->level_idc != 20 &&
	  sps->level_idc != 21 &&
	  sps->level_idc != 22 &&
	  sps->level_idc != 30 &&
	  sps->level_idc != 31 &&
	  sps->level_idc != 32 &&
	  sps->level_idc != 40 &&
	  sps->level_idc != 41 &&
	  sps->level_idc != 42 &&
	  sps->level_idc != 50 &&
	  sps->level_idc != 51)
	{
      VOH264ERROR(VO_H264_ERR_InvalidSPS);
	}

  sps->seq_parameter_set_id                   = ue_v (pDecGlobal);//"SPS: seq_parameter_set_id"

  if(sps->seq_parameter_set_id<0||sps->seq_parameter_set_id>=MAXSPS)
	VOH264ERROR(VO_H264_ERR_InvalidSPS);
  // Fidelity Range Extensions stuff
  sps->chroma_format_idc = 1;
  sps->bit_depth_luma_minus8   = 0;
  sps->bit_depth_chroma_minus8 = 0;

  sps->separate_colour_plane_flag = 0;

  if((sps->profile_idc==FREXT_HP   ) ||
     (sps->profile_idc==FREXT_Hi10P) ||
     (sps->profile_idc==FREXT_Hi422) ||
     (sps->profile_idc==FREXT_Hi444) ||
     (sps->profile_idc==FREXT_CAVLC444)
#if (MVC_EXTENSION_ENABLE)
     || (sps->profile_idc==MVC_HIGH)
     || (sps->profile_idc==STEREO_HIGH)
#endif
     )
  {
    sps->chroma_format_idc                      = ue_v ( pDecGlobal);//"SPS: chroma_format_idc"

    if(sps->chroma_format_idc == YUV444)
    {
      sps->separate_colour_plane_flag           = u_v (1, pDecGlobal);//"SPS: separate_colour_plane_flag"
    }

    sps->bit_depth_luma_minus8                  = ue_v (pDecGlobal);//"SPS: bit_depth_luma_minus8"
    sps->bit_depth_chroma_minus8                = ue_v (pDecGlobal);//"SPS: bit_depth_chroma_minus8"
    //checking;
    if((sps->bit_depth_luma_minus8+8 > sizeof(imgpel)*8) || (sps->bit_depth_chroma_minus8+8> sizeof(imgpel)*8))
      VOH264ERROR(VO_H264_ERR_InvalidSPS);
    u_v (1, pDecGlobal);//"SPS: lossless_qpprime_y_zero_flag"

    sps->seq_scaling_matrix_present_flag        = u_v (1, pDecGlobal);//"SPS: seq_scaling_matrix_present_flag"
    
    if(sps->seq_scaling_matrix_present_flag)
    {
      n_ScalingList = (sps->chroma_format_idc != YUV444) ? 8 : 12;
      for(i=0; i<n_ScalingList; i++)
      {
        sps->seq_scaling_list_present_flag[i]   = u_v (1, pDecGlobal);//"SPS: seq_scaling_list_present_flag"
        if(sps->seq_scaling_list_present_flag[i])
        {
          if(i<6)
            Scaling_List(pDecGlobal,sps->ScalingList4x4[i], 16, &sps->UseDefaultScalingMatrix4x4Flag[i]);
          else
            Scaling_List(pDecGlobal,sps->ScalingList8x8[i-6], 64, &sps->UseDefaultScalingMatrix8x8Flag[i-6]);
        }
      }
    }
  }

  sps->log2_max_frame_num_minus4              = ue_v (pDecGlobal);//"SPS: log2_max_frame_num_minus4"
  if (sps->log2_max_frame_num_minus4 < 0 || sps->log2_max_frame_num_minus4 > 12)
	VOH264ERROR(VO_H264_ERR_InvalidSPS);
  sps->pic_order_cnt_type                     = ue_v (pDecGlobal);//"SPS: pic_order_cnt_type"
  if (sps->pic_order_cnt_type < 0 || sps->pic_order_cnt_type > 2)
	VOH264ERROR(VO_H264_ERR_InvalidSPS);
  if (sps->pic_order_cnt_type == 0)
    sps->log2_max_pic_order_cnt_lsb_minus4 = ue_v (pDecGlobal);//"SPS: log2_max_pic_order_cnt_lsb_minus4"
   
  else if (sps->pic_order_cnt_type == 1)
  {
    sps->delta_pic_order_always_zero_flag      = u_v (1, pDecGlobal);//"SPS: delta_pic_order_always_zero_flag"
    sps->offset_for_non_ref_pic                = se_v (pDecGlobal);//"SPS: offset_for_non_ref_pic"
    sps->offset_for_top_to_bottom_field        = se_v (pDecGlobal);//"SPS: offset_for_top_to_bottom_field"
    sps->num_ref_frames_in_pic_order_cnt_cycle = ue_v (pDecGlobal);//"SPS: num_ref_frames_in_pic_order_cnt_cycle"
    if(sps->num_ref_frames_in_pic_order_cnt_cycle >= MAXnum_ref_frames_in_pic_order_cnt_cycle)
      VOH264ERROR(VO_H264_ERR_InvalidSPS);
    for(i=0; i<sps->num_ref_frames_in_pic_order_cnt_cycle; i++)
      sps->offset_for_ref_frame[i]               = se_v (pDecGlobal);//"SPS: offset_for_ref_frame[i]"
  }
  sps->num_ref_frames                        = ue_v (pDecGlobal);//"SPS: num_ref_frames"
  if(sps->num_ref_frames <=0||sps->num_ref_frames>16)
	VOH264ERROR(VO_H264_ERR_InvalidSPS);
  sps->gaps_in_frame_num_value_allowed_flag  = u_v (1, pDecGlobal);//"SPS: gaps_in_frame_num_value_allowed_flag"
  sps->pic_width_in_mbs_minus1               = ue_v (pDecGlobal);//"SPS: pic_width_in_mbs_minus1"
  if (sps->pic_width_in_mbs_minus1>127)
    VOH264ERROR(VO_H264_ERR_InvalidSPS);
  sps->pic_height_in_map_units_minus1        = ue_v (pDecGlobal);//"SPS: pic_height_in_map_units_minus1"
  if (sps->pic_height_in_map_units_minus1>=199)
	  VOH264ERROR(VO_H264_ERR_InvalidSPS);
  sps->frame_mbs_only_flag                   = u_v (1, pDecGlobal);//"SPS: frame_mbs_only_flag"
  
  if (!sps->frame_mbs_only_flag)
  {
    if (sps->pic_height_in_map_units_minus1>=99)
	  VOH264ERROR(VO_H264_ERR_InvalidSPS);
    sps->mb_adaptive_frame_field_flag        = u_v (1, pDecGlobal);//"SPS: mb_adaptive_frame_field_flag"
  }
  sps->direct_8x8_inference_flag             = u_v (1, pDecGlobal);//"SPS: direct_8x8_inference_flag"
  sps->frame_cropping_flag                   = u_v (1, pDecGlobal);//"SPS: frame_cropping_flag"

  if (sps->frame_cropping_flag)
  {
    sps->frame_cropping_rect_left_offset      = ue_v (pDecGlobal);//"SPS: frame_cropping_rect_left_offset"
	if (sps->frame_cropping_rect_left_offset>=sps->pic_width_in_mbs_minus1+1)
		VOH264ERROR(VO_H264_ERR_InvalidSPS);
    sps->frame_cropping_rect_right_offset     = ue_v (pDecGlobal);//"SPS: frame_cropping_rect_right_offset"
	if (sps->frame_cropping_rect_right_offset>=sps->pic_width_in_mbs_minus1+1)
		VOH264ERROR(VO_H264_ERR_InvalidSPS);
    sps->frame_cropping_rect_top_offset       = ue_v (pDecGlobal);//"SPS: frame_cropping_rect_top_offset"
	if (sps->frame_cropping_rect_top_offset>=sps->pic_height_in_map_units_minus1+1)
		VOH264ERROR(VO_H264_ERR_InvalidSPS);
    sps->frame_cropping_rect_bottom_offset    = ue_v (pDecGlobal);//"SPS: frame_cropping_rect_bottom_offset"
	if (sps->frame_cropping_rect_bottom_offset>=sps->pic_height_in_map_units_minus1+1)
		VOH264ERROR(VO_H264_ERR_InvalidSPS);
  }
  sps->vui_parameters_present_flag           = (Boolean) u_v (1, pDecGlobal);//"SPS: vui_parameters_present_flag"

  InitVUI(sps);
  ret = ReadVUI(pDecGlobal,p, sps);
  if(ret)
  	return ret;
  sps->Valid = TRUE;
  return 0;//p_Dec->UsedBits;
}

// fill subset_sps with content of p
#if (MVC_EXTENSION_ENABLE)
static int InterpretSubsetSPS (H264DEC_G *pDecGlobal, DataPartition *p, int *curr_seq_set_id)
{
  subset_seq_parameter_set_rbsp_t *subset_sps;
  unsigned int additional_extension2_flag;
  Bitstream *s = p->bitstream;
  seq_parameter_set_rbsp_t *sps = AllocSPS();

  assert (p != NULL);
  assert (p->bitstream != NULL);
  assert (p->bitstream->streamBuffer != 0);

  InterpretSPS (pDecGlobal, p, sps);

  *curr_seq_set_id = sps->seq_parameter_set_id;
  subset_sps = pDecGlobal->SubsetSeqParSet + sps->seq_parameter_set_id;
  if(subset_sps->Valid || subset_sps->num_views_minus1>=0)
  {
    reset_subset_sps(subset_sps);
  }
  memcpy (&subset_sps->sps, sps, sizeof (seq_parameter_set_rbsp_t));

  assert (subset_sps != NULL);
  subset_sps->Valid = FALSE;

  /*if(subset_sps->sps.profile_idc == SCALABLE_BASELINE_PROFILE || subset_sps->sps.profile_idc == SCALABLE_HIGH_PROFILE)
  {
	  printf("\nScalable profile is not supported yet!\n");
  }
  else*/
  if(subset_sps->sps.profile_idc == MVC_HIGH || subset_sps->sps.profile_idc == STEREO_HIGH)
  {
	  subset_sps->bit_equal_to_one = u_v (1, pDecGlobal);//"bit_equal_to_one"

	  if(subset_sps->bit_equal_to_one !=1 )
	  {
		  //printf("\nbit_equal_to_one is not equal to 1!\n");
		  return 1;//p_Dec->UsedBits;
	  }

	  seq_parameter_set_mvc_extension(pDecGlobal,subset_sps, s);
	
	  subset_sps->mvc_vui_parameters_present_flag = u_v (1, pDecGlobal);//"mvc_vui_parameters_present_flag"
	  if(subset_sps->mvc_vui_parameters_present_flag)
		  mvc_vui_parameters_extension(pDecGlobal,&(subset_sps->MVCVUIParams)										, s);
  }

  additional_extension2_flag = u_v (1, pDecGlobal);//"additional_extension2_flag"	
  if(additional_extension2_flag)
  {
	  while (more_rbsp_data(pDecGlobal))
		  additional_extension2_flag = u_v (1, pDecGlobal);//"additional_extension2_flag"	
  }

  if (subset_sps->sps.Valid)
	  subset_sps->Valid = TRUE;

  FreeSPS (sps);
  return 1;//p_Dec->UsedBits;

}
#endif

void InitVUI(seq_parameter_set_rbsp_t *sps)
{
  sps->vui_seq_parameters.matrix_coefficients = 2;
}


int ReadVUI(H264DEC_G *pDecGlobal,DataPartition *p, seq_parameter_set_rbsp_t *sps)
{
  VO_S32 ret;
  if (sps->vui_parameters_present_flag)
  {
    sps->vui_seq_parameters.aspect_ratio_info_present_flag = u_v (1, pDecGlobal);//"VUI: aspect_ratio_info_present_flag"
    if (sps->vui_seq_parameters.aspect_ratio_info_present_flag)
    {
      sps->vui_seq_parameters.aspect_ratio_idc             = u_v  ( 8,pDecGlobal);//"VUI: aspect_ratio_idc"
      if (255==sps->vui_seq_parameters.aspect_ratio_idc)
      {
        sps->vui_seq_parameters.sar_width                  = (unsigned short) u_v  (16,pDecGlobal);//"VUI: sar_width" 
        sps->vui_seq_parameters.sar_height                 = (unsigned short) u_v  (16,pDecGlobal);//"VUI: sar_height"
      }
    }

    sps->vui_seq_parameters.overscan_info_present_flag     = u_v (1, pDecGlobal);//"VUI: overscan_info_present_flag"
    if (sps->vui_seq_parameters.overscan_info_present_flag)
    {
      sps->vui_seq_parameters.overscan_appropriate_flag    = u_v (1, pDecGlobal);//"VUI: overscan_appropriate_flag"
    }

    sps->vui_seq_parameters.video_signal_type_present_flag = u_v (1, pDecGlobal);//"VUI: video_signal_type_present_flag"
    if (sps->vui_seq_parameters.video_signal_type_present_flag)
    {
      sps->vui_seq_parameters.video_format                    = u_v  ( 3,pDecGlobal);//"VUI: video_format"
      sps->vui_seq_parameters.video_full_range_flag           = u_v (1, pDecGlobal);//"VUI: video_full_range_flag"
      sps->vui_seq_parameters.colour_description_present_flag = u_v (1, pDecGlobal);//"VUI: color_description_present_flag"
      if(sps->vui_seq_parameters.colour_description_present_flag)
      {
        sps->vui_seq_parameters.colour_primaries              = u_v  ( 8,pDecGlobal);//"VUI: colour_primaries" 
        sps->vui_seq_parameters.transfer_characteristics      = u_v  ( 8,pDecGlobal);//"VUI: transfer_characteristics"
        sps->vui_seq_parameters.matrix_coefficients           = u_v  ( 8,pDecGlobal);//"VUI: matrix_coefficients"
      }
    }
    sps->vui_seq_parameters.chroma_location_info_present_flag = u_v (1, pDecGlobal);//"VUI: chroma_loc_info_present_flag"
    if(sps->vui_seq_parameters.chroma_location_info_present_flag)
    {
      sps->vui_seq_parameters.chroma_sample_loc_type_top_field     = ue_v  (pDecGlobal);//"VUI: chroma_sample_loc_type_top_field"
      sps->vui_seq_parameters.chroma_sample_loc_type_bottom_field  = ue_v  (pDecGlobal);//"VUI: chroma_sample_loc_type_bottom_field"
    }
    sps->vui_seq_parameters.timing_info_present_flag          = u_v (1, pDecGlobal);//"VUI: timing_info_present_flag"
    if (sps->vui_seq_parameters.timing_info_present_flag)
    {
      sps->vui_seq_parameters.num_units_in_tick               = GetBits32(pDecGlobal);//u_v  (32,pDecGlobal);//"VUI: num_units_in_tick"
      sps->vui_seq_parameters.time_scale                      = GetBits32(pDecGlobal);//u_v  (32,pDecGlobal);//"VUI: time_scale"
      sps->vui_seq_parameters.fixed_frame_rate_flag           = u_v (1, pDecGlobal);//"VUI: fixed_frame_rate_flag"
    }
    sps->vui_seq_parameters.nal_hrd_parameters_present_flag   = u_v (1, pDecGlobal);//"VUI: nal_hrd_parameters_present_flag"
    if (sps->vui_seq_parameters.nal_hrd_parameters_present_flag)
    {
      ret = ReadHRDParameters(pDecGlobal,p, &(sps->vui_seq_parameters.nal_hrd_parameters));
	  if(ret)
	  	return ret;
    }
    sps->vui_seq_parameters.vcl_hrd_parameters_present_flag   = u_v (1, pDecGlobal);//"VUI: vcl_hrd_parameters_present_flag"
    if (sps->vui_seq_parameters.vcl_hrd_parameters_present_flag)
    {
      ret = ReadHRDParameters(pDecGlobal,p, &(sps->vui_seq_parameters.vcl_hrd_parameters));
	  if(ret)
	  	return ret;
    }
    if (sps->vui_seq_parameters.nal_hrd_parameters_present_flag || sps->vui_seq_parameters.vcl_hrd_parameters_present_flag)
    {
      sps->vui_seq_parameters.low_delay_hrd_flag             =  u_v (1, pDecGlobal);//"VUI: low_delay_hrd_flag"
    }
    sps->vui_seq_parameters.pic_struct_present_flag          =  u_v (1, pDecGlobal);//"VUI: pic_struct_present_flag   "
    sps->vui_seq_parameters.bitstream_restriction_flag       =  u_v (1, pDecGlobal);//"VUI: bitstream_restriction_flag"
    if (sps->vui_seq_parameters.bitstream_restriction_flag)
    {
      sps->vui_seq_parameters.motion_vectors_over_pic_boundaries_flag =  u_v (1, pDecGlobal);//"VUI: motion_vectors_over_pic_boundaries_flag"
      sps->vui_seq_parameters.max_bytes_per_pic_denom                 =  ue_v (pDecGlobal);//"VUI: max_bytes_per_pic_denom"
      sps->vui_seq_parameters.max_bits_per_mb_denom                   =  ue_v (pDecGlobal);//"VUI: max_bits_per_mb_denom"
      sps->vui_seq_parameters.log2_max_mv_length_horizontal           =  ue_v (pDecGlobal);//"VUI: log2_max_mv_length_horizontal"
      sps->vui_seq_parameters.log2_max_mv_length_vertical             =  ue_v (pDecGlobal);//"VUI: log2_max_mv_length_vertical"
      sps->vui_seq_parameters.num_reorder_frames                      =  ue_v (pDecGlobal);//"VUI: num_reorder_frames"
      sps->vui_seq_parameters.max_dec_frame_buffering                 =  ue_v (pDecGlobal);//"VUI: max_dec_frame_buffering"
    }
  }

  return 0;
}


int ReadHRDParameters(H264DEC_G *pDecGlobal,DataPartition *p, hrd_parameters_t *hrd)
{
  unsigned int SchedSelIdx;

  hrd->cpb_cnt_minus1                                      = ue_v (pDecGlobal);//"VUI: cpb_cnt_minus1"
  hrd->bit_rate_scale                                      = u_v  ( 4,pDecGlobal);//"VUI: bit_rate_scale"
  hrd->cpb_size_scale                                      = u_v  ( 4,pDecGlobal);//"VUI: cpb_size_scale"
  if(hrd->cpb_cnt_minus1>=MAXIMUMVALUEOFcpb_cnt)
	VOH264ERROR(VO_H264_ERR_MAX_CPB_CNT);
  for( SchedSelIdx = 0; SchedSelIdx <= hrd->cpb_cnt_minus1; SchedSelIdx++ )
  {
    hrd->bit_rate_value_minus1[ SchedSelIdx ]             = ue_v  (pDecGlobal);//"VUI: bit_rate_value_minus1"
    hrd->cpb_size_value_minus1[ SchedSelIdx ]             = ue_v  (pDecGlobal);//"VUI: cpb_size_value_minus1"
    hrd->cbr_flag[ SchedSelIdx ]                          = u_v (1, pDecGlobal);//"VUI: cbr_flag"
  }

  hrd->initial_cpb_removal_delay_length_minus1            = u_v  ( 5,pDecGlobal);//"VUI: initial_cpb_removal_delay_length_minus1"
  hrd->cpb_removal_delay_length_minus1                    = u_v  ( 5,pDecGlobal);//"VUI: cpb_removal_delay_length_minus1"
  hrd->dpb_output_delay_length_minus1                     = u_v  ( 5,pDecGlobal);//"VUI: dpb_output_delay_length_minus1"
  hrd->time_offset_length                                 = u_v  ( 5,pDecGlobal);//"VUI: time_offset_length"

  return 0;
}


int InterpretPPS (H264DEC_G *pDecGlobal, DataPartition *p, pic_parameter_set_rbsp_t *pps)
{
  unsigned i;
  unsigned n_ScalingList;
  int chroma_format_idc;

  //p_Dec->UsedBits = 0;

  pps->pic_parameter_set_id                  = ue_v (pDecGlobal);//"PPS: pic_parameter_set_id"
  if(pps->pic_parameter_set_id<0||pps->pic_parameter_set_id>=MAXPPS)
	VOH264ERROR(VO_H264_ERR_InvalidPPS);
  pps->seq_parameter_set_id                  = ue_v (pDecGlobal);//"PPS: seq_parameter_set_id"
  pps->entropy_coding_mode_flag              = u_v (1, pDecGlobal);//"PPS: entropy_coding_mode_flag"


  pps->bottom_field_pic_order_in_frame_present_flag                = u_v (1, pDecGlobal);//"PPS: bottom_field_pic_order_in_frame_present_flag"

  pps->num_slice_groups_minus1               = ue_v (pDecGlobal);//"PPS: num_slice_groups_minus1"
  if(pps->num_slice_groups_minus1 != 0)
	VOH264ERROR(VO_H264_ERR_NotSupportFMO);
  // FMO stuff begins here
  /*if (pps->num_slice_groups_minus1 > 0)
  {
    pps->slice_group_map_type               = ue_v (pDecGlobal);//"PPS: slice_group_map_type"
    if (pps->slice_group_map_type == 0)
    {
      for (i=0; i<=pps->num_slice_groups_minus1; i++)
        pps->run_length_minus1 [i]                  = ue_v (pDecGlobal);//"PPS: run_length_minus1 [i]"
    }
    else if (pps->slice_group_map_type == 2)
    {
      for (i=0; i<pps->num_slice_groups_minus1; i++)
      {
        //! JVT-F078: avoid reference of SPS by using ue(v) instead of u(v)
        pps->top_left [i]                          = ue_v (pDecGlobal);//"PPS: top_left [i]"
        pps->bottom_right [i]                      = ue_v (pDecGlobal);//"PPS: bottom_right [i]"
      }
    }
    else if (pps->slice_group_map_type == 3 ||
             pps->slice_group_map_type == 4 ||
             pps->slice_group_map_type == 5)
    {
      pps->slice_group_change_direction_flag     = u_v (1, pDecGlobal);//"PPS: slice_group_change_direction_flag"
      pps->slice_group_change_rate_minus1        = ue_v (pDecGlobal);//"PPS: slice_group_change_rate_minus1"
    }
    else if (pps->slice_group_map_type == 6)
    {
      if (pps->num_slice_groups_minus1+1 >4)
        NumberBitsPerSliceGroupId = 3;
      else if (pps->num_slice_groups_minus1+1 > 2)
        NumberBitsPerSliceGroupId = 2;
      else
        NumberBitsPerSliceGroupId = 1;
      pps->pic_size_in_map_units_minus1      = ue_v (pDecGlobal);//"PPS: pic_size_in_map_units_minus1"
      if ((pps->slice_group_id = voMalloc(pDecGlobal,pps->pic_size_in_map_units_minus1+1)) == NULL)
        no_mem_exit ("InterpretPPS: slice_group_id");
      for (i=0; i<=pps->pic_size_in_map_units_minus1; i++)
        pps->slice_group_id[i] = (byte) u_v (NumberBitsPerSliceGroupId,pDecGlobal);//"slice_group_id[i]"
    }
  }*/

  // End of FMO stuff

  pps->num_ref_idx_l0_active_minus1          = ue_v (pDecGlobal);//"PPS: num_ref_idx_l0_active_minus1"
  if(pps->num_ref_idx_l0_active_minus1<0||pps->num_ref_idx_l0_active_minus1>=MAX_REFERENCE_FRAMES)
	VOH264ERROR(VO_H264_ERR_InvalidPPS);
  pps->num_ref_idx_l1_active_minus1          = ue_v (pDecGlobal);//"PPS: num_ref_idx_l1_active_minus1"
  if(pps->num_ref_idx_l1_active_minus1<0||pps->num_ref_idx_l1_active_minus1>=MAX_REFERENCE_FRAMES)
	VOH264ERROR(VO_H264_ERR_InvalidPPS);
  pps->weighted_pred_flag                    = u_v (1, pDecGlobal);//"PPS: weighted_pred_flag"
  pps->weighted_bipred_idc                   = u_v  ( 2,pDecGlobal);//"PPS: weighted_bipred_idc"
  pps->pic_init_qp_minus26                   = se_v (pDecGlobal);//"PPS: pic_init_qp_minus26"
  pps->pic_init_qs_minus26                   = se_v (pDecGlobal);//"PPS: pic_init_qs_minus26"
  if(pps->pic_init_qp_minus26<-26||pps->pic_init_qp_minus26>=26||pps->pic_init_qs_minus26<-26||pps->pic_init_qs_minus26>=26)
	VOH264ERROR(VO_H264_ERR_InvalidPPS);
  pps->chroma_qp_index_offset                = se_v (pDecGlobal);//"PPS: chroma_qp_index_offset"

  pps->deblocking_filter_control_present_flag = u_v (1, pDecGlobal);//"PPS: deblocking_filter_control_present_flag"
  pps->constrained_intra_pred_flag           = u_v (1, pDecGlobal);//"PPS: constrained_intra_pred_flag"
  pps->redundant_pic_cnt_present_flag        = u_v (1, pDecGlobal);//"PPS: redundant_pic_cnt_present_flag"
  if(pps->seq_parameter_set_id<0||pps->seq_parameter_set_id>=MAXSPS)
    pps->seq_parameter_set_id = 0;
  if (pDecGlobal->SeqParSet[pps->seq_parameter_set_id].Valid != TRUE)
	VOH264ERROR(VO_H264_ERR_SPSIsNULL);
  if(pDecGlobal->SeqParSet[pps->seq_parameter_set_id].profile_idc==FREXT_HP&&more_rbsp_data(pDecGlobal)) // more_data_in_rbsp()
  {
    //Fidelity Range Extensions Stuff
    pps->transform_8x8_mode_flag           = u_v (1, pDecGlobal);//"PPS: transform_8x8_mode_flag"
    pps->pic_scaling_matrix_present_flag   =  u_v (1, pDecGlobal);//"PPS: pic_scaling_matrix_present_flag"

    if(pps->pic_scaling_matrix_present_flag)
    {
      chroma_format_idc = pDecGlobal->SeqParSet[pps->seq_parameter_set_id].chroma_format_idc;
      n_ScalingList = 6 + ((chroma_format_idc != YUV444) ? 2 : 6) * pps->transform_8x8_mode_flag;
      for(i=0; i<n_ScalingList; i++)
      {
        pps->pic_scaling_list_present_flag[i]= u_v (1, pDecGlobal);//"PPS: pic_scaling_list_present_flag"

        if(pps->pic_scaling_list_present_flag[i])
        {
          if(i<6)
            Scaling_List(pDecGlobal,pps->ScalingList4x4[i], 16, &pps->UseDefaultScalingMatrix4x4Flag[i]);
          else
            Scaling_List(pDecGlobal,pps->ScalingList8x8[i-6], 64, &pps->UseDefaultScalingMatrix8x8Flag[i-6]);
        }
      }
    }
    pps->second_chroma_qp_index_offset      = se_v (pDecGlobal);//"PPS: second_chroma_qp_index_offset"
  }
  else
  {
    pps->second_chroma_qp_index_offset      = pps->chroma_qp_index_offset;
  }
  
  pps->Valid = TRUE;
  return 0;//p_Dec->UsedBits;
}


void PPSConsistencyCheck (pic_parameter_set_rbsp_t *pps)
{
  //printf ("Consistency checking a picture parset, to be implemented\n");
//  if (pps->seq_parameter_set_id invalid then do something)
}

void SPSConsistencyCheck (seq_parameter_set_rbsp_t *sps)
{
  //printf ("Consistency checking a sequence parset, to be implemented\n");
}

#if (MVC_EXTENSION_ENABLE)
void SubsetSPSConsistencyCheck (subset_seq_parameter_set_rbsp_t *subset_sps)
{
  //printf ("Consistency checking a subset sequence parset, to be implemented\n");
}
#endif

void MakePPSavailable (H264DEC_G *pDecGlobal, int id, pic_parameter_set_rbsp_t *pps)
{
  assert (pps->Valid == TRUE);

  if (pDecGlobal->PicParSet[id].Valid == TRUE && pDecGlobal->PicParSet[id].slice_group_id != NULL)
    voFree (pDecGlobal,pDecGlobal->PicParSet[id].slice_group_id);

  memcpy (&pDecGlobal->PicParSet[id], pps, sizeof (pic_parameter_set_rbsp_t));

  // we can simply use the memory provided with the pps. the PPS is destroyed after this function
  // call and will not try to voFree if pps->slice_group_id == NULL
  pDecGlobal->PicParSet[id].slice_group_id = pps->slice_group_id;
  pps->slice_group_id          = NULL;
}

void CleanUpPPS(H264DEC_G *pDecGlobal)
{
  int i;

  for (i=0; i<MAXPPS; i++)
  {
    if (pDecGlobal->PicParSet[i].Valid == TRUE && pDecGlobal->PicParSet[i].slice_group_id != NULL)
      voFree (pDecGlobal,pDecGlobal->PicParSet[i].slice_group_id);

    pDecGlobal->PicParSet[i].Valid = FALSE;
  }
}


void MakeSPSavailable (H264DEC_G *pDecGlobal, int id, seq_parameter_set_rbsp_t *sps)
{
  assert (sps->Valid == TRUE);
  memcpy (&pDecGlobal->SeqParSet[id], sps, sizeof (seq_parameter_set_rbsp_t));
}


int ProcessSPS (H264DEC_G *pDecGlobal, NALU_t *nalu)
{
  int ret;
  DataPartition *dp = AllocPartition(pDecGlobal, 1);
  seq_parameter_set_rbsp_t *sps = AllocSPS(pDecGlobal);

  //memcpy (dp->bitstream->streamBuffer, &nalu->buf[1], nalu->len-1);
  dp->bitstream->streamBuffer = &nalu->buf[1];
  dp->bitstream->code_len = dp->bitstream->bitstream_length = RBSPtoSODB (dp->bitstream->streamBuffer, nalu->len-1);
  InitBitStream(pDecGlobal,dp->bitstream->streamBuffer, dp->bitstream->code_len);
  //dp->bitstream->ei_flag = 0;
  dp->bitstream->read_len = dp->bitstream->frame_bitoffset = 0;
  ret = InterpretSPS (pDecGlobal, dp, sps);
  if(ret)
  {
    FreePartition (pDecGlobal,dp, 1);
    FreeSPS (pDecGlobal,sps);
  	return ret;
  }
#if (MVC_EXTENSION_ENABLE)
  get_max_dec_frame_buf_size(sps);
#endif

  if (sps->Valid&&!pDecGlobal->SeqParSet[sps->seq_parameter_set_id].Valid)
  {
    if (pDecGlobal->first_sps == 3 && !sps->frame_mbs_only_flag != pDecGlobal->interlace)
    {
     FreePartition (pDecGlobal,dp, 1);
     FreeSPS (pDecGlobal,sps);
     return VO_H264_ERR_InvalidSPS;
    }
    MakeSPSavailable (pDecGlobal, sps->seq_parameter_set_id, sps);
    pDecGlobal->profile_idc = sps->profile_idc;
  }

  if (pDecGlobal->first_sps != 3)
  {
	  //LOGI("spsdone\r\n");
	  pDecGlobal->interlace = !sps->frame_mbs_only_flag;
	  pDecGlobal->first_sps |= 1;
  }
  FreePartition (pDecGlobal,dp, 1);
  FreeSPS (pDecGlobal,sps);
  return 0;
}

#if (MVC_EXTENSION_ENABLE)
void ProcessSubsetSPS (H264DEC_G *pDecGlobal, NALU_t *nalu)
{
  DataPartition *dp = AllocPartition(pDecGlobal,1);
  subset_seq_parameter_set_rbsp_t *subset_sps;
  int curr_seq_set_id;

  //memcpy (dp->bitstream->streamBuffer, &nalu->buf[1], nalu->len-1);
  dp->bitstream->streamBuffer = &nalu->buf[1];
  dp->bitstream->code_len = dp->bitstream->bitstream_length = RBSPtoSODB (dp->bitstream->streamBuffer, nalu->len-1);
  InitBitStream(pDecGlobal,dp->bitstream->streamBuffer, dp->bitstream->code_len);
  //dp->bitstream->ei_flag = 0;
  dp->bitstream->read_len = dp->bitstream->frame_bitoffset = 0;
  InterpretSubsetSPS (pDecGlobal, dp, &curr_seq_set_id);

  subset_sps = pDecGlobal->SubsetSeqParSet + curr_seq_set_id;
  get_max_dec_frame_buf_size(&(subset_sps->sps));

  if (subset_sps->Valid)
  {
    // SubsetSPSConsistencyCheck (subset_sps);
    pDecGlobal->profile_idc = subset_sps->sps.profile_idc;
#if 0 //YU_TBD
    if( pDecGlobal->separate_colour_plane_flag )
    {
      p_Vid->ChromaArrayType = 0;
    }
    else
#endif
    {
     // p_Vid->ChromaArrayType = subset_sps->sps.chroma_format_idc;
    }
  }

//  FreeSubsetSPS (subset_sps);
  FreePartition (pDecGlobal,dp, 1);  
}
#endif

int ProcessPPS (H264DEC_G *pDecGlobal, NALU_t *nalu)
{
  DataPartition *dp = AllocPartition(pDecGlobal,1);
  pic_parameter_set_rbsp_t *pps = AllocPPS(pDecGlobal);
  int ret;
  //memcpy (dp->bitstream->streamBuffer, &nalu->buf[1], nalu->len-1);
  dp->bitstream->streamBuffer = &nalu->buf[1];
  dp->bitstream->code_len = dp->bitstream->bitstream_length = RBSPtoSODB (dp->bitstream->streamBuffer, nalu->len-1);
  //dp->bitstream->ei_flag = 0;
  dp->bitstream->read_len = dp->bitstream->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,dp->bitstream->streamBuffer, dp->bitstream->code_len);
  ret = InterpretPPS (pDecGlobal, dp, pps);
  if(ret)
  {
    FreePartition (pDecGlobal,dp, 1);
    FreePPS (pDecGlobal,pps);
  	return ret;
  }
  // PPSConsistencyCheck (pps);
  if (pDecGlobal->pCurSlice->active_pps)
  {
    if (pps->pic_parameter_set_id == pDecGlobal->pCurSlice->active_pps->pic_parameter_set_id)
    {
      if(!pps_is_equal(pps, pDecGlobal->pCurSlice->active_pps))
      {
        //copy to next PPS;
        {
          if (pDecGlobal->dec_picture) // && p_Vid->num_dec_mb == p_Vid->PicSizeInMbs)
          {
            // this may only happen on slice loss
            //ret = exit_picture(pDecGlobal, &pDecGlobal->dec_picture);
#if USE_FRAME_THREAD
			if (pDecGlobal->nThdNum > 1 /*&& !pDecGlobal->interlace*/)
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
				pDecGlobal->dec_picture->errFlag = VO_H264_ERR_PIC_STRUCT;
			}
			pDecGlobal->dec_picture = NULL;
          }
          pDecGlobal->pCurSlice->active_pps = NULL;
        }
      }
    }
  }
  MakePPSavailable (pDecGlobal, pps->pic_parameter_set_id, pps);
  FreePartition (pDecGlobal,dp, 1);
  FreePPS (pDecGlobal,pps);
  return 0;
}


/*!
 ************************************************************************
 * \brief
 *    Activate Sequence Parameter Sets
 *
 ************************************************************************
 */
void activate_sps (H264DEC_G *pDecGlobal, seq_parameter_set_rbsp_t *sps)
{ 

  if (pDecGlobal->pCurSlice->active_sps != sps)
  {
    if (pDecGlobal->dec_picture)
    {
      // this may only happen on slice loss
      //exit_picture(pDecGlobal, &pDecGlobal->dec_picture);
#if USE_FRAME_THREAD
	  if (pDecGlobal->nThdNum > 1 /*&& !pDecGlobal->interlace*/)
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
		  pDecGlobal->dec_picture->errFlag = VO_H264_ERR_PIC_STRUCT;
	  }
	  pDecGlobal->dec_picture = NULL;
    }
#if USE_FRAME_THREAD
	if (pDecGlobal->nThdNum > 1 /*&& !pDecGlobal->interlace*/)
	{
		if (pDecGlobal->pCurSlice->active_sps)
		{
			//in frame thread mode, not support multi sps
			pDecGlobal->pThdInfo->errFlag = VO_H264_ERR_InvalidSPS;
			return;
		}
	}
#endif
    pDecGlobal->pCurSlice->active_sps = sps;

    pDecGlobal->width_cr        = 0;
    pDecGlobal->height_cr       = 0;

    // Fidelity Range Extensions stuff (part 1)

    pDecGlobal->MaxFrameNum = 1<<(sps->log2_max_frame_num_minus4+4);
    pDecGlobal->PicWidthInMbs = (sps->pic_width_in_mbs_minus1 +1);
    pDecGlobal->PicHeightInMapUnits = (sps->pic_height_in_map_units_minus1 +1);
    pDecGlobal->FrameHeightInMbs = ( 2 - sps->frame_mbs_only_flag ) * pDecGlobal->PicHeightInMapUnits;
    pDecGlobal->FrameSizeInMbs = pDecGlobal->PicWidthInMbs * pDecGlobal->FrameHeightInMbs;

    pDecGlobal->height = pDecGlobal->FrameHeightInMbs * MB_BLOCK_SIZE;
	pDecGlobal->width = pDecGlobal->PicWidthInMbs * MB_BLOCK_SIZE;
	pDecGlobal->iLumaStride = pDecGlobal->width + pDecGlobal->iLumaPadX*2;
	pDecGlobal->iLumaPadY = sps->frame_mbs_only_flag? MCBUF_LUMA_PAD_Y : MCBUF_LUMA_PAD_Y*2;
	pDecGlobal->iChromaPadY = sps->frame_mbs_only_flag? MCBUF_CHROMA_PAD_Y : MCBUF_CHROMA_PAD_Y*2;
   
    //if (sps->chroma_format_idc == YUV420)
    //{
      pDecGlobal->width_cr  = (pDecGlobal->width  >> 1);
      pDecGlobal->height_cr = (pDecGlobal->height >> 1);
    //}
	pDecGlobal->iChromaStride = pDecGlobal->width_cr + pDecGlobal->iChromaPadX*2;
	
    init_global_buffers(pDecGlobal);

    if (!pDecGlobal->no_output_of_prior_pics_flag && (sps-pDecGlobal->SeqParSet) != pDecGlobal->spsNum)
    {
      flush_dpb(pDecGlobal,pDecGlobal->p_Dpb);
    }
    init_dpb(pDecGlobal, pDecGlobal->p_Dpb);
    //remove by Really Yang 20110420
    //ercInit(p_Vid, p_Vid->width, p_Vid->height, 1);
	//end of remove 
    //if(p_Vid->dec_picture)
    //{
      //remove by Really Yang 20110420
      //ercReset(p_Vid->erc_errorVar, p_Vid->PicSizeInMbs, p_Vid->PicSizeInMbs, p_Vid->dec_picture->size_x);      
      //p_Vid->erc_mvperMB = 0;
	  //end of remove
    //}
  }
  
  //reset_format_info(pDecGlobal,sps, &p_Inp->source, &p_Inp->output);

}

void activate_pps(H264DEC_G *pDecGlobal, pic_parameter_set_rbsp_t *pps)
{ 
	if (pDecGlobal->pCurSlice->active_pps != pps)
	{
#if USE_FRAME_THREAD
		if (pDecGlobal->nThdNum <= 1/*||pDecGlobal->interlace*/)
#endif
		{
			if (pDecGlobal->dec_picture) // && p_Vid->num_dec_mb == p_Vid->pi)
			{
			  // this may only happen on slice loss
			  //exit_picture(pDecGlobal, &pDecGlobal->dec_picture);
			  pDecGlobal->dec_picture->errFlag = VO_H264_ERR_PIC_STRUCT;
			  pDecGlobal->dec_picture = NULL;
			}
		}

		pDecGlobal->pCurSlice->active_pps = pps;
		pDecGlobal->chroma_qp_offset[0] = pps->chroma_qp_index_offset;
		pDecGlobal->chroma_qp_offset[1] = pps->second_chroma_qp_index_offset;
// 		assign_quant_params (pDecGlobal,pDecGlobal->pCurSlice);
	}
}

