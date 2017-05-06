#include <stdlib.h>
#include <string.h>
#include "voH264Parser.h"
#include "voReadBits.h"

#define PARSER_IS_ANNEXB(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)==1))
#define PARSER_IS_ANNEXB2(p) ( !(*((p)+0)) && !(*((p)+1)) && !(*((p)+2))&& (*((p)+3)==1))

static VO_U8 ZZ_SCAN[16]  =
{  0,  1,  4,  8,  5,  2,  3,  6,  9, 12, 13, 10,  7, 11, 14, 15
};

static VO_U8 ZZ_SCAN8[64] =
{  0,  1,  8, 16,  9,  2,  3, 10, 17, 24, 32, 25, 18, 11,  4,  5,
12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13,  6,  7, 14, 21, 28,
35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};
static VO_U8* ParserGetNextFrame(VO_U8* currPos,int size)
{
	VO_U8* p = currPos;  
	VO_U8* endPos = currPos+size-4;
	for (; p < endPos; p++)
	{
		if (PARSER_IS_ANNEXB(p))
			return p+3;
		else if(PARSER_IS_ANNEXB2(p))
			return p+4;
	}
	if(p>=endPos)
		p = NULL;

	return p;
}
VO_S32 ParserCleanStream(VO_BYTE*	head, VO_S32 size)
{
	VO_BYTE *end = head + size;

	do {
		if (head[0]) {// faster if most of bytes are not zero;	
			head++;
			continue;
		}
		// find 00xx
		if (head[1]){
			head += 2; // skip two bytes;
			continue;
		}
		// find 0000xx

		if(head[2]!=3)
		{
			head+=3;
		}
		else
		{
			memmove(head+2,head+3,end-(head+3));
			end--;
			head+=2;
		}
	}
	while(head+3<end);

	return 0;
}


void Scaling_List_Parser(int *scalingList, int sizeOfScalingList, PARSER_BITSTREAM *bs)
{
	int j, scanj;
	int delta_scale, lastScale, nextScale;

	lastScale      = 8;
	nextScale      = 8;

	for(j=0; j<sizeOfScalingList; j++)
	{
		scanj = (sizeOfScalingList==16) ? ZZ_SCAN[j]:ZZ_SCAN8[j];

		if(nextScale!=0)
		{
			delta_scale = p_se_v (bs);
			nextScale = (lastScale + delta_scale + 256) % 256;
		}

		scalingList[scanj] = (nextScale==0) ? lastScale:nextScale;
		lastScale = scalingList[scanj];
	}
}

VO_S32 ParserProcessSPS(VO_BYTE* buf,VO_S32 size, parser_sps_t *sps)
{
	int return_val = 0;
	VO_S32 reserved_zero;
	PARSER_BITSTREAM bs = {0};
    unsigned char *pCopy = malloc(size);
	if(!pCopy)
	  return 1; 
    memcpy(pCopy, buf, size);
	memset(sps,0,sizeof(parser_sps_t));
	ParserCleanStream(pCopy,size);

	ParserInitBits(&bs,pCopy,size);

	sps->profile_idc							= p_u_v(8, &bs);
	if ((sps->profile_idc!=BASELINE       ) &&
		(sps->profile_idc!=MAIN           ) &&
		(sps->profile_idc!=EXTENDED       ) &&
		(sps->profile_idc!=FREXT_HP       ) &&
		(sps->profile_idc!=FREXT_Hi10P    ) &&
		(sps->profile_idc!=FREXT_Hi422    ) &&
		(sps->profile_idc!=FREXT_Hi444    ) &&
		(sps->profile_idc!=FREXT_CAVLC444 ) && 
		(sps->profile_idc!=MVC_HIGH       ) && 
		(sps->profile_idc!=STEREO_HIGH)
		)
	{
		free(pCopy);
		return 1;
	}
	sps->constrained_set0_flag					= p_u_v(1, &bs);
	sps->constrained_set1_flag					= p_u_v(1, &bs);
	sps->constrained_set2_flag					= p_u_v(1, &bs);
	sps->constrained_set3_flag					= p_u_v(1, &bs);
	reserved_zero						= p_u_v(4, &bs);
	if (reserved_zero)
	{
		free(pCopy);
		return 1;
	}

	sps->level_idc								= p_u_v(8, &bs);
	if( sps->level_idc != 10 &&
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
		free(pCopy);
		return 1;
	}

	sps->seq_parameter_set_id					= p_ue_v(&bs);
	if (sps->seq_parameter_set_id & 0xffffffe0)
	{
		free(pCopy);
		return 1;
	}
	sps->chroma_format_idc = 1;
	if((sps->profile_idc==FREXT_HP   ) ||
		(sps->profile_idc==FREXT_Hi10P) ||
		(sps->profile_idc==FREXT_Hi422) ||
		(sps->profile_idc==FREXT_Hi444) ||
		(sps->profile_idc==FREXT_CAVLC444)
		)
	{
		VO_S32 qpprime_y_zero_transform_bypass_flag;
		sps->chroma_format_idc = p_ue_v(&bs);
		if (sps->chroma_format_idc & 0xfffffffc)
		{
			free(pCopy);
			return 1;
		}

		if(sps->chroma_format_idc==3)
		{
		  p_u_v(1, &bs);
		}

		sps->bit_depth_luma_minus8 = p_ue_v(&bs);
		sps->bit_depth_chroma_minus8 = p_ue_v(&bs);
		qpprime_y_zero_transform_bypass_flag = p_u_v(1, &bs);
		sps->seq_scaling_matrix_present_flag = p_u_v(1, &bs);

		if(sps->seq_scaling_matrix_present_flag)
		{
			VO_S32 n_ScalingList = (sps->chroma_format_idc != 3) ? 8 : 12;
			VO_S32 i;
			for( i=0; i<n_ScalingList; i++)
			{
			  sps->seq_scaling_list_present_flag[i]=p_u_v(1, &bs);
			  if(sps->seq_scaling_list_present_flag[i])
			  {
				  if(i<6)
					  Scaling_List_Parser(sps->ScalingList4x4[i], 16, &bs);
				  else
					  Scaling_List_Parser(sps->ScalingList8x8[i-6], 64, &bs);
			  }
			}
		}
	}

	sps->log2_max_frame_num_minus4				= p_ue_v(&bs);// ("SPS: log2_max_frame_num_minus4"                , s);
	if (sps->log2_max_frame_num_minus4 < 0 || sps->log2_max_frame_num_minus4 > 12)
	{
		free(pCopy);
		return 1;
	}
	sps->pic_order_cnt_type						= p_ue_v(&bs);//"SPS: pic_order_cnt_type"                       , s);
	if (sps->pic_order_cnt_type < 0 || sps->pic_order_cnt_type > 2)
	{
		free(pCopy);
		return 1;
	}

	if (sps->pic_order_cnt_type == 0)
		sps->log2_max_pic_order_cnt_lsb_minus4	= p_ue_v(&bs);//"SPS: log2_max_pic_order_cnt_lsb_minus4"           , s)
	else if (sps->pic_order_cnt_type == 1)
	{
		VO_S32 tmp,count,i;
		tmp	= p_u_v(1, &bs);
		tmp	= p_se_v(&bs);
		tmp	= p_se_v(&bs);
		count = p_ue_v(&bs);

		for( i=0; i<count; i++)
			tmp = p_se_v(&bs);
	}											

	sps->num_ref_frames                        = p_ue_v(&bs);// ("SPS: num_ref_frames"                         , s);
	sps->gaps_in_frame_num_value_allowed_flag  = p_u_v(1, &bs);// ("SPS: gaps_in_frame_num_value_allowed_flag"   , s);
	sps->pic_width_in_mbs_minus1               = p_ue_v(&bs);//("SPS: pic_width_in_mbs_minus1"                , s);
	sps->pic_height_in_map_units_minus1        = p_ue_v(&bs);// ("SPS: pic_height_in_map_units_minus1"         , s);
	sps->frame_mbs_only_flag                   = p_u_v(1, &bs);//("SPS: frame_mbs_only_flag"                    , s);
	if(sps->frame_mbs_only_flag==0)
		sps->mb_adaptive_frame_field_flag		= p_u_v(1, &bs);//  ("SPS: mb_adaptive_frame_field_flag"           , s);
	sps->direct_8x8_inference_flag             = p_u_v(1, &bs); // (s);
	sps->frame_cropping_flag                   = p_u_v(1, &bs); // (s);
	if(sps->frame_cropping_flag)
	{
		sps->frame_cropping_rect_left_offset      = p_ue_v (&bs);
		sps->frame_cropping_rect_right_offset     = p_ue_v (&bs);
		sps->frame_cropping_rect_top_offset       = p_ue_v (&bs);//*(4-2*sps->frame_mbs_only_flag);//TODO:
		sps->frame_cropping_rect_bottom_offset    = p_ue_v (&bs);//*(4-2*sps->frame_mbs_only_flag);
	}
	sps->vui_parameters_present_flag           =  p_u_v (1, &bs);
	if (sps->vui_parameters_present_flag)
	{
		sps->vui_seq_parameters.aspect_ratio_info_present_flag = p_u_v (1, &bs);//"VUI: aspect_ratio_info_present_flag"
		if (sps->vui_seq_parameters.aspect_ratio_info_present_flag)
		{
			sps->vui_seq_parameters.aspect_ratio_idc             = p_u_v  ( 8,&bs);//"VUI: aspect_ratio_idc"
			if (255==sps->vui_seq_parameters.aspect_ratio_idc)
			{
				sps->vui_seq_parameters.sar_width                  = (unsigned short) p_u_v  (16,&bs);//"VUI: sar_width" 
				sps->vui_seq_parameters.sar_height                 = (unsigned short) p_u_v  (16,&bs);//"VUI: sar_height"
			}
		}
		sps->vui_seq_parameters.overscan_info_present_flag     = p_u_v (1, &bs);//"VUI: overscan_info_present_flag"
		if (sps->vui_seq_parameters.overscan_info_present_flag)
		{
			sps->vui_seq_parameters.overscan_appropriate_flag    = p_u_v (1, &bs);//"VUI: overscan_appropriate_flag"
		}

		sps->vui_seq_parameters.video_signal_type_present_flag = p_u_v (1, &bs);//"VUI: video_signal_type_present_flag"
		if (sps->vui_seq_parameters.video_signal_type_present_flag)
		{
			sps->vui_seq_parameters.video_format                    = p_u_v  ( 3,&bs);//"VUI: video_format"
			sps->vui_seq_parameters.video_full_range_flag           = p_u_v (1, &bs);//"VUI: video_full_range_flag"
			sps->vui_seq_parameters.colour_description_present_flag = p_u_v (1, &bs);//"VUI: color_description_present_flag"
			if(sps->vui_seq_parameters.colour_description_present_flag)
			{
				sps->vui_seq_parameters.colour_primaries              = p_u_v  ( 8,&bs);//"VUI: colour_primaries" 
				sps->vui_seq_parameters.transfer_characteristics      = p_u_v  ( 8,&bs);//"VUI: transfer_characteristics"
				sps->vui_seq_parameters.matrix_coefficients           = p_u_v  ( 8,&bs);//"VUI: matrix_coefficients"
			}
		}
		sps->vui_seq_parameters.chroma_location_info_present_flag = p_u_v (1, &bs);//"VUI: chroma_loc_info_present_flag"
		if(sps->vui_seq_parameters.chroma_location_info_present_flag)
		{
			sps->vui_seq_parameters.chroma_sample_loc_type_top_field     = p_ue_v  (&bs);//"VUI: chroma_sample_loc_type_top_field"
			sps->vui_seq_parameters.chroma_sample_loc_type_bottom_field  = p_ue_v  (&bs);//"VUI: chroma_sample_loc_type_bottom_field"
		}
		sps->vui_seq_parameters.timing_info_present_flag          = p_u_v (1, &bs);//"VUI: timing_info_present_flag"
		if (sps->vui_seq_parameters.timing_info_present_flag)
		{
			//sps->vui_seq_parameters.num_units_in_tick               = p_u_v(32,&bs);//u_v  (32,pDecGlobal);//"VUI: num_units_in_tick"
			//sps->vui_seq_parameters.time_scale                      = p_u_v(32,&bs);//u_v  (32,pDecGlobal);//"VUI: time_scale"
			//sps->vui_seq_parameters.fixed_frame_rate_flag           = p_u_v (1, &bs);//"VUI: fixed_frame_rate_flag"
			p_u_v(8,&bs);p_u_v(8,&bs);p_u_v(8,&bs);p_u_v(8,&bs);
			p_u_v(8,&bs);p_u_v(8,&bs);p_u_v(8,&bs);p_u_v(8,&bs);
			p_u_v(1,&bs);
		}
		sps->vui_seq_parameters.nal_hrd_parameters_present_flag   = p_u_v (1, &bs);//"VUI: nal_hrd_parameters_present_flag"
		if (sps->vui_seq_parameters.nal_hrd_parameters_present_flag)
		{
			hrd_parameters_t *hrd = &(sps->vui_seq_parameters.nal_hrd_parameters);
			unsigned int SchedSelIdx;

			hrd->cpb_cnt_minus1                                      = p_ue_v (&bs);//"VUI: cpb_cnt_minus1"
			if (hrd->cpb_cnt_minus1>=32)
			{
				free(pCopy);
				return return_val;
			}
			hrd->bit_rate_scale                                      = p_u_v  ( 4,&bs);//"VUI: bit_rate_scale"
			hrd->cpb_size_scale                                      = p_u_v  ( 4,&bs);//"VUI: cpb_size_scale"
			for( SchedSelIdx = 0; SchedSelIdx <= hrd->cpb_cnt_minus1; SchedSelIdx++ )
			{
				hrd->bit_rate_value_minus1[ SchedSelIdx ]             = p_ue_v  (&bs);//"VUI: bit_rate_value_minus1"
				hrd->cpb_size_value_minus1[ SchedSelIdx ]             = p_ue_v  (&bs);//"VUI: cpb_size_value_minus1"
				hrd->cbr_flag[ SchedSelIdx ]                          = p_u_v (1, &bs);//"VUI: cbr_flag"
			}

			hrd->initial_cpb_removal_delay_length_minus1            = p_u_v  ( 5,&bs);//"VUI: initial_cpb_removal_delay_length_minus1"
			hrd->cpb_removal_delay_length_minus1                    = p_u_v  ( 5,&bs);//"VUI: cpb_removal_delay_length_minus1"
			hrd->dpb_output_delay_length_minus1                     = p_u_v  ( 5,&bs);//"VUI: dpb_output_delay_length_minus1"
			hrd->time_offset_length                                 = p_u_v  ( 5,&bs);//"VUI: time_offset_length"
		}
		sps->vui_seq_parameters.vcl_hrd_parameters_present_flag   = p_u_v (1, &bs);//"VUI: vcl_hrd_parameters_present_flag"
		if (sps->vui_seq_parameters.vcl_hrd_parameters_present_flag)
		{
			hrd_parameters_t *hrd = &(sps->vui_seq_parameters.vcl_hrd_parameters);
			unsigned int SchedSelIdx;

			hrd->cpb_cnt_minus1                                      = p_ue_v (&bs);//"VUI: cpb_cnt_minus1"
			if (hrd->cpb_cnt_minus1>=32)
			{
				free(pCopy);
				return return_val;
			}
			hrd->bit_rate_scale                                      = p_u_v  ( 4,&bs);//"VUI: bit_rate_scale"
			hrd->cpb_size_scale                                      = p_u_v  ( 4,&bs);//"VUI: cpb_size_scale"
			for( SchedSelIdx = 0; SchedSelIdx <= hrd->cpb_cnt_minus1; SchedSelIdx++ )
			{
				hrd->bit_rate_value_minus1[ SchedSelIdx ]             = p_ue_v  (&bs);//"VUI: bit_rate_value_minus1"
				hrd->cpb_size_value_minus1[ SchedSelIdx ]             = p_ue_v  (&bs);//"VUI: cpb_size_value_minus1"
				hrd->cbr_flag[ SchedSelIdx ]                          = p_u_v (1, &bs);//"VUI: cbr_flag"
			}

			hrd->initial_cpb_removal_delay_length_minus1            = p_u_v  ( 5,&bs);//"VUI: initial_cpb_removal_delay_length_minus1"
			hrd->cpb_removal_delay_length_minus1                    = p_u_v  ( 5,&bs);//"VUI: cpb_removal_delay_length_minus1"
			hrd->dpb_output_delay_length_minus1                     = p_u_v  ( 5,&bs);//"VUI: dpb_output_delay_length_minus1"
			hrd->time_offset_length                                 = p_u_v  ( 5,&bs);//"VUI: time_offset_length"

		}
		if (sps->vui_seq_parameters.nal_hrd_parameters_present_flag || sps->vui_seq_parameters.vcl_hrd_parameters_present_flag)
		{
			sps->vui_seq_parameters.low_delay_hrd_flag             =  p_u_v (1, &bs);//"VUI: low_delay_hrd_flag"
		}
		sps->vui_seq_parameters.pic_struct_present_flag          =  p_u_v (1, &bs);//"VUI: pic_struct_present_flag   "
		sps->vui_seq_parameters.bitstream_restriction_flag       =  p_u_v (1, &bs);//"VUI: bitstream_restriction_flag"
		if (sps->vui_seq_parameters.bitstream_restriction_flag)
		{
			sps->vui_seq_parameters.motion_vectors_over_pic_boundaries_flag =  p_u_v (1, &bs);//"VUI: motion_vectors_over_pic_boundaries_flag"
			sps->vui_seq_parameters.max_bytes_per_pic_denom                 =  p_ue_v (&bs);//"VUI: max_bytes_per_pic_denom"
			sps->vui_seq_parameters.max_bits_per_mb_denom                   =  p_ue_v (&bs);//"VUI: max_bits_per_mb_denom"
			sps->vui_seq_parameters.log2_max_mv_length_horizontal           =  p_ue_v (&bs);//"VUI: log2_max_mv_length_horizontal"
			sps->vui_seq_parameters.log2_max_mv_length_vertical             =  p_ue_v (&bs);//"VUI: log2_max_mv_length_vertical"
			sps->vui_seq_parameters.num_reorder_frames                      =  p_ue_v (&bs);//"VUI: num_reorder_frames"
			sps->vui_seq_parameters.max_dec_frame_buffering                 =  p_ue_v (&bs);//"VUI: max_dec_frame_buffering"
		}
	}
    free(pCopy);
	return return_val;	
}

typedef enum {
	NALU_PARSER_SLICE    = 1,
	NALU_PARSER_DPA      = 2,
	NALU_PARSER_DPB      = 3,
	NALU_PARSER_DPC      = 4,
	NALU_PARSER_IDR      = 5,
	NALU_PARSER_SEI      = 6,
	NALU_PARSER_SPS      = 7,
	NALU_PARSER_PPS      = 8,
	NALU_PARSER_AUD      = 9,
	NALU_PARSER_EOSEQ    = 10,
	NALU_PARSER_EOSTREAM = 11,
	NALU_PARSER_FILL     = 12,
#if (MVC_EXTENSION_ENABLE)
	NALU_PARSER_PREFIX   = 14,
	NALU_PARSER_SUB_SPS  = 15,
	NALU_PARSER_SLC_EXT  = 20,
	NALU_PARSER_VDRD     = 24  // View and Dependency Representation Delimiter NAL Unit
#endif
} ParserNaluType;

typedef struct
{
	VO_VIDEO_CODINGTYPE nCodec;
	VO_U32 nProfile;
	VO_U32 nLevel;
	VO_VIDEO_FRAMETYPE nFrame_type;
	VO_U32 nWidth;
	VO_U32 nHeight;
	VO_U32 isInterlace;
	VO_U32 isRefFrame;
	VO_U32 isIDRFrame;
	VO_VOID* pCodecUser;
	VO_U32 seHeader;
	VO_U32 FrameHeader;
	VO_U32 HasS3D;
	VO_U32 nSize;
	VO_S3D_Params S3D;
	VO_H264_USERDATA_Params UserData;
	VO_U32 DPB_size;
	VO_U32 ref_number;
	VO_PARSER_ASPECTRATIO ratio;
	parser_out_sps_t sps;
	parser_sps_t in_sps;
	VO_PARSER_SEI_INFO sei_info;
	VO_U8* sps_buffer;
	VO_U32 sps_len;
	VO_U8* pps_buffer;
	VO_U32 pps_len;
	VO_CODECBUFFER header;
	VO_U32 process_id;
} VO_H264PARSER_PARAM;

VO_S32 ParserGetFrameType (VO_U8* pInData, VO_S32 nInSize)
{
	PARSER_BITSTREAM bs = {0}; 
	VO_U32 tmp;
	ParserInitBits(&bs,pInData,nInSize);
	if(p_ue_v(&bs))
		return -1;
	tmp = p_ue_v(&bs);
	if (tmp > 4)
		tmp -= 5;
	return tmp;
}


void interpret_picture_timing_info( unsigned char* payload, int size,VO_H264PARSER_PARAM *pVideoInfo,VO_SEI_PIC_TIMING_STRUCT* pic_time  )
{
  parser_sps_t *active_sps = &pVideoInfo->in_sps;

  int cpb_removal_delay=0, dpb_output_delay=0, picture_structure_present_flag=0, picture_structure=0;
  int clock_time_stamp_flag=0;
  int ct_type=0, nuit_field_based_flag=0, counting_type=0, full_timestamp_flag=0, discontinuity_flag=0, cnt_dropped_flag=0, nframes=0;
  int seconds_value=0, minutes_value=0, hours_value=0, seconds_flag=0, minutes_flag=0, hours_flag=0, time_offset=0;
  int NumClockTs = 0;
  int i;

  int cpb_removal_len = 24;
  int dpb_output_len  = 24;

  int CpbDpbDelaysPresentFlag;

  PARSER_BITSTREAM bs = {0};

  ParserInitBits(&bs,payload,size);

  CpbDpbDelaysPresentFlag =  (active_sps->vui_parameters_present_flag
                              && (   (active_sps->vui_seq_parameters.nal_hrd_parameters_present_flag != 0)
                                   ||(active_sps->vui_seq_parameters.vcl_hrd_parameters_present_flag != 0)));

  if (CpbDpbDelaysPresentFlag )
  {
    if (active_sps->vui_parameters_present_flag)
    {
      if (active_sps->vui_seq_parameters.nal_hrd_parameters_present_flag)
      {
        cpb_removal_len = active_sps->vui_seq_parameters.nal_hrd_parameters.cpb_removal_delay_length_minus1 + 1;
        dpb_output_len  = active_sps->vui_seq_parameters.nal_hrd_parameters.dpb_output_delay_length_minus1  + 1;
      }
      else if (active_sps->vui_seq_parameters.vcl_hrd_parameters_present_flag)
      {
        cpb_removal_len = active_sps->vui_seq_parameters.vcl_hrd_parameters.cpb_removal_delay_length_minus1 + 1;
        dpb_output_len  = active_sps->vui_seq_parameters.vcl_hrd_parameters.dpb_output_delay_length_minus1  + 1;
      }
    }

    if ((active_sps->vui_seq_parameters.nal_hrd_parameters_present_flag)||
      (active_sps->vui_seq_parameters.vcl_hrd_parameters_present_flag))
    {
      cpb_removal_delay = p_u_v(cpb_removal_len,&bs);//"SEI: cpb_removal_delay"
      dpb_output_delay  = p_u_v(dpb_output_len,&bs);//"SEI: dpb_output_delay"
    }
  }

  if (!active_sps->vui_parameters_present_flag)
  {
    picture_structure_present_flag = 0;
  }
  else
  {
    picture_structure_present_flag  =  active_sps->vui_seq_parameters.pic_struct_present_flag;
  }

  if (picture_structure_present_flag)
  {
    picture_structure = p_u_v(4,&bs);//"SEI: pic_struct"
    switch (picture_structure)
    {
    case 0:
    case 1:
    case 2:
      NumClockTs = 1;
      break;
    case 3:
    case 4:
    case 7:
      NumClockTs = 2;
      break;
    case 5:
    case 6:
    case 8:
      NumClockTs = 3;
      break;
    default:
      //error("reserved picture_structure used (can't determine NumClockTs)", 500);
      break;
    }
    for (i=0; i<NumClockTs; i++)
    {
      VO_CLOCK_TIME_STAMP_STRUCT* clock_struct = &pic_time->clock_sturct[i];
      clock_time_stamp_flag = p_u_v (1, &bs);//"SEI: clock_time_stamp_flag"
      if (clock_time_stamp_flag)
      {
        ct_type               = p_u_v(2,&bs);//"SEI: ct_type"
        nuit_field_based_flag = p_u_v (1, &bs);//"SEI: nuit_field_based_flag"
        counting_type         = p_u_v(5,&bs);//"SEI: counting_type"
        full_timestamp_flag   = p_u_v (1, &bs);//"SEI: full_timestamp_flag"
        discontinuity_flag    = p_u_v (1, &bs);//"SEI: discontinuity_flag"
        cnt_dropped_flag      = p_u_v (1, &bs);//"SEI: cnt_dropped_flag"
        nframes               = p_u_v(8,&bs);//"SEI: nframes"

        if (full_timestamp_flag)
        {
          seconds_value         = p_u_v(6,&bs);//"SEI: seconds_value"
          minutes_value         = p_u_v(6,&bs);//"SEI: minutes_value"
          hours_value           = p_u_v(5,&bs);//"SEI: hours_value"
        }
        else
        {
          seconds_flag          = p_u_v (1, &bs);//"SEI: seconds_flag"
          if (seconds_flag)
          {
            seconds_value         = p_u_v(6,&bs);//"SEI: seconds_value"
            minutes_flag          = p_u_v (1, &bs);//"SEI: minutes_flag"
            if(minutes_flag)
            {
              minutes_value         = p_u_v(6,&bs);//"SEI: minutes_value"
              hours_flag            = p_u_v (1, &bs);//"SEI: hours_flag"
              if(hours_flag)
              {
                hours_value           = p_u_v(5,&bs);//"SEI: hours_value"
              }
            }
          }
        }
        {
          int time_offset_length;
          if (active_sps->vui_seq_parameters.vcl_hrd_parameters_present_flag)
            time_offset_length = active_sps->vui_seq_parameters.vcl_hrd_parameters.time_offset_length;
          else if (active_sps->vui_seq_parameters.nal_hrd_parameters_present_flag)
            time_offset_length = active_sps->vui_seq_parameters.nal_hrd_parameters.time_offset_length;
          else
            time_offset_length = 24;
          if (time_offset_length)
            time_offset = p_i_v(time_offset_length,&bs);//"SEI: time_offset"
          else
            time_offset = 0;
        }
      }
	  clock_struct->clock_time_stamp_flag = clock_time_stamp_flag;
      clock_struct->cnt_dropped_flag = cnt_dropped_flag;
      clock_struct->counting_type = counting_type;
      clock_struct->ct_type = ct_type;
      clock_struct->discontinuity_flag = discontinuity_flag;
      clock_struct->full_timestamp_flag = full_timestamp_flag;
      clock_struct->hours_flag = hours_flag;
      clock_struct->hours_value = hours_value;
      clock_struct->minutes_flag = minutes_flag;
      clock_struct->minutes_value = minutes_value;
      clock_struct->nframes = nframes;
      clock_struct->nuit_field_based_flag = nuit_field_based_flag;
      clock_struct->seconds_flag = seconds_flag;
      clock_struct->seconds_value = seconds_value;
      clock_struct->time_offset = time_offset;
    }
  }
  pic_time->CpbDpbDelaysPresentFlag = CpbDpbDelaysPresentFlag;
  pic_time->cpb_removal_delay = cpb_removal_delay;
  pic_time->dpb_output_delay = dpb_output_delay;
  pic_time->NumClockTs = NumClockTs;
  pic_time->picture_structure = picture_structure;
  pic_time->picture_structure_present_flag = picture_structure_present_flag;
  return;
}


void ParserInterpretStereoVideoInfo( unsigned char* payload, int size, VO_S3D_Params *S3D)
{
	int field_views_flags;
	int top_field_is_left_view_flag, current_frame_is_left_view_flag, next_frame_is_second_view_flag;
	int left_view_self_contained_flag;
	int right_view_self_contained_flag;

	PARSER_BITSTREAM bs = {0};

	ParserInitBits(&bs,payload,size);

	field_views_flags = p_u_v(1, &bs);//"SEI: field_views_flags"
	if (field_views_flags)
	{
		top_field_is_left_view_flag         = p_u_v(1, &bs);//"SEI: top_field_is_left_view_flag"
		if(top_field_is_left_view_flag)
			S3D->order = S3D_ORDER_LF;
		else
			S3D->order = S3D_ORDER_RF;
	}
	else
	{
		current_frame_is_left_view_flag     = p_u_v(1, &bs);//"SEI: current_frame_is_left_view_flag"
		if(current_frame_is_left_view_flag)
			S3D->order = S3D_ORDER_LF;
		else
			S3D->order = S3D_ORDER_RF;
		next_frame_is_second_view_flag      = p_u_v(1, &bs);//"SEI: next_frame_is_second_view_flag"
	}

	left_view_self_contained_flag         = p_u_v(1, &bs);//"SEI: left_view_self_contained_flag"
	right_view_self_contained_flag        = p_u_v(1, &bs);//"SEI: right_view_self_contained_flag"
}

typedef enum {
	checkerboard = 0,
	column = 1,
	row = 2,
	side_by_side = 3,
	top_bottom = 4,
	temporal = 5,
}V0_SFRAME_PACKING_ARRANGEMENT_TYPE;

typedef struct
{
	unsigned int  frame_packing_arrangement_id;
	VO_S32       frame_packing_arrangement_cancel_flag;
	unsigned char frame_packing_arrangement_type;
	VO_S32       quincunx_sampling_flag;
	unsigned char content_interpretation_type;
	VO_S32       spatial_flipping_flag;
	VO_S32       frame0_flipped_flag;
	VO_S32       field_views_flag;
	VO_S32       current_frame_is_frame0_flag;
	VO_S32       frame0_self_contained_flag;
	VO_S32       frame1_self_contained_flag;
	unsigned char frame0_grid_position_x;
	unsigned char frame0_grid_position_y;
	unsigned char frame1_grid_position_x;
	unsigned char frame1_grid_position_y;
	unsigned char frame_packing_arrangement_reserved_byte;
	unsigned int  frame_packing_arrangement_repetition_period;
	VO_S32       frame_packing_arrangement_extension_flag;
} VO_FRAME_PACKING_ARRANGEMENT_INFORMATION_STRUCT;

void ParserInterpretFramePackingArrangementInfo( unsigned char* payload, int size, VO_S3D_Params *S3D  )
{
	VO_FRAME_PACKING_ARRANGEMENT_INFORMATION_STRUCT seiFramePackingArrangement;
	//Bitstream* buf;

	PARSER_BITSTREAM bs = {0};

	ParserInitBits(&bs,payload,size);

	S3D->active = 1;
	S3D->mode = S3D_MODE_ON;
	S3D->subsampling = S3D_SS_NONE;
	seiFramePackingArrangement.frame_packing_arrangement_id = (unsigned int)p_ue_v(&bs );//"SEI: frame_packing_arrangement_id"
	seiFramePackingArrangement.frame_packing_arrangement_cancel_flag = p_u_v(1, &bs);//"SEI: frame_packing_arrangement_cancel_flag"
	if ( seiFramePackingArrangement.frame_packing_arrangement_cancel_flag == 0 )
	{
		seiFramePackingArrangement.frame_packing_arrangement_type = (unsigned char)p_u_v(7, &bs);//"SEI: frame_packing_arrangement_type"
		switch(seiFramePackingArrangement.frame_packing_arrangement_type)
		{
		case checkerboard:
			{
				S3D->fmt = S3D_FORMAT_CHECKB;
				break;
			}
		case column:
			{
				S3D->fmt = S3D_FORMAT_COL_IL;
				break;
			}
		case row:
			{
				S3D->fmt = S3D_FORMAT_ROW_IL;
				break;
			}
		case side_by_side:
			{
				S3D->fmt = S3D_FORMAT_SIDEBYSIDE;
				S3D->subsampling = S3D_SS_HOR;
				break;
			}
		case top_bottom:
			{
				S3D->fmt = S3D_FORMAT_OVERUNDER;
				S3D->subsampling = S3D_SS_VERT;
				break;
			}
		case temporal:
			{
				S3D->fmt = S3D_FORMAT_FRM_SEQ;
				break;
			}
		default:
			{
				S3D->fmt = S3D_FORMAT_OVERUNDER;
				break;
			}
		}
		seiFramePackingArrangement.quincunx_sampling_flag         = p_u_v(1, &bs);//"SEI: quincunx_sampling_flag"
		seiFramePackingArrangement.content_interpretation_type    = (unsigned char)p_u_v(6, &bs);//"SEI: content_interpretation_type"
		if(seiFramePackingArrangement.content_interpretation_type)
			S3D->order = S3D_ORDER_LF;
		else
			S3D->order = S3D_ORDER_RF;
		seiFramePackingArrangement.spatial_flipping_flag          = p_u_v(1, &bs);//"SEI: spatial_flipping_flag"
		seiFramePackingArrangement.frame0_flipped_flag            = p_u_v(1, &bs);//"SEI: frame0_flipped_flag"
		seiFramePackingArrangement.field_views_flag               = p_u_v(1, &bs);//"SEI: field_views_flag"
		seiFramePackingArrangement.current_frame_is_frame0_flag   = p_u_v(1, &bs);//"SEI: current_frame_is_frame0_flag"
		seiFramePackingArrangement.frame0_self_contained_flag     = p_u_v(1, &bs);//"SEI: frame0_self_contained_flag"
		seiFramePackingArrangement.frame1_self_contained_flag     = p_u_v(1, &bs);//"SEI: frame1_self_contained_flag"
		if ( seiFramePackingArrangement.quincunx_sampling_flag == 0 && seiFramePackingArrangement.frame_packing_arrangement_type != 5 )
		{
			seiFramePackingArrangement.frame0_grid_position_x = (unsigned char)p_u_v(4, &bs);//"SEI: frame0_grid_position_x"
			seiFramePackingArrangement.frame0_grid_position_y = (unsigned char)p_u_v(4, &bs);//"SEI: frame0_grid_position_y"
			seiFramePackingArrangement.frame1_grid_position_x = (unsigned char)p_u_v(4, &bs);//"SEI: frame1_grid_position_x"
			seiFramePackingArrangement.frame1_grid_position_y = (unsigned char)p_u_v(4, &bs);//"SEI: frame1_grid_position_y"
		}
		seiFramePackingArrangement.frame_packing_arrangement_reserved_byte = (unsigned char)p_u_v(8, &bs);//"SEI: frame_packing_arrangement_reserved_byte"
		seiFramePackingArrangement.frame_packing_arrangement_repetition_period = (unsigned int)p_ue_v(&bs );//"SEI: frame_packing_arrangement_repetition_period"
	}
	seiFramePackingArrangement.frame_packing_arrangement_extension_flag = p_u_v(1, &bs);//"SEI: frame_packing_arrangement_extension_flag"

}

VO_S32 ParserProcessSEI(VO_BYTE* buf,VO_S32 size, VO_H264PARSER_PARAM *pVideoInfo)
{
	int payload_type = 0;
	int payload_size = 0;
	int offset = 1;
	VO_U8 tmp_byte;
	VO_U8 has_header = 1;
	VO_S3D_Params *S3D;
	VO_H264_USERDATA_Params* UserData;
	VO_PARSER_SEI_INFO* sei_info;
	PARSER_BITSTREAM bs = {0};
	VO_U8 *pCopy = malloc(size);
	if (pVideoInfo==NULL)
	  return has_header; 
	if(!pCopy)
		return has_header; 
    S3D = &pVideoInfo->S3D;
	UserData = &pVideoInfo->UserData;
	sei_info = &pVideoInfo->sei_info;
	memcpy(pCopy, buf, size);
	ParserCleanStream(pCopy,size);

	ParserInitBits(&bs,pCopy,size);

	do
	{
		payload_type = 0;
		tmp_byte = pCopy[offset++];
		while (tmp_byte == 0xFF&&offset<size)
		{
			payload_type += 255;
			tmp_byte = pCopy[offset++];
		}
		payload_type += tmp_byte;   // this is the last byte

		payload_size = 0;
		tmp_byte = pCopy[offset++];
		while (tmp_byte == 0xFF&&offset<size)
		{
			payload_size += 255;
			tmp_byte = pCopy[offset++];
		}
		payload_size += tmp_byte;   
		if (offset+payload_size>=size)
		{
			return has_header;
		}
		switch ( payload_type )     // sei_payload( type, size );
		{
		case SEI_STEREO_VIDEO_INFO:
			ParserInterpretStereoVideoInfo( pCopy+offset, payload_size, S3D );
			pVideoInfo->HasS3D = 1;
			has_header = 0;
			break;
		case SEI_FRAME_PACKING_ARRANGEMENT:
			ParserInterpretFramePackingArrangementInfo( pCopy+offset, payload_size, S3D );
			pVideoInfo->HasS3D = 1;
			has_header = 0;
			break;
		case SEI_PIC_TIMING:
		{
			if(!sei_info->buffercount)
			  sei_info->buffer = malloc(1024*1024);
			memcpy(sei_info->buffer+sei_info->buffersize,pCopy+offset,payload_size);
			sei_info->data[sei_info->buffercount].flag_group = 0;
			sei_info->data[sei_info->buffercount].flag = VO_SEI_PIC_TIMING;
			sei_info->data[sei_info->buffercount].has_struct = sizeof(VO_SEI_PIC_TIMING_STRUCT);
			sei_info->data[sei_info->buffercount++].buffer_lenth= payload_size;
			sei_info->buffersize+=payload_size+sizeof(VO_SEI_PIC_TIMING_STRUCT);
		    break;
		}
		case SEI_USER_DATA_REGISTERED_ITU_T_T35:
		case SEI_USER_DATA_UNREGISTERED:	
		{
			VO_U8* p_buf = UserData->buffer;
			if(!sei_info->buffersize)
			  sei_info->buffer = malloc(1024*1024);
			memcpy(sei_info->buffer+sei_info->buffersize,pCopy+offset,payload_size);
			sei_info->data[sei_info->buffercount].flag_group = 0;
			sei_info->data[sei_info->buffercount].flag = payload_type;
			sei_info->data[sei_info->buffercount++].buffer_lenth= payload_size;
			sei_info->buffersize+=payload_size;
			if (UserData->count == 0)
			{
				if (UserData->buffer)
				{
				  free(UserData->buffer);
                  UserData->buffer = NULL;
				}
				if (pVideoInfo->nSize<payload_size)
				{
					break;
				}
				UserData->buffer = malloc(pVideoInfo->nSize);
				if(!UserData->buffer)
				{
					free(pCopy);
					return has_header;
				}
				p_buf = UserData->buffer;
			}
			else
			{
				VO_U32 i;
				for (i = 0;i < UserData->count;i++)
				{
					p_buf+=UserData->size[i];
				}
			}
			memcpy(p_buf,pCopy+offset,payload_size);
			UserData->size[UserData->count++] = payload_size;
			has_header = 0;
			break;
		}
		default:
			break;   
		}
		offset += payload_size;
	}while( pCopy[offset] != 0x80 && offset  < size);
	free(pCopy);
	return has_header;
}

int getDpbSize(parser_sps_t *active_sps)
{
  int pic_size = (active_sps->pic_width_in_mbs_minus1 + 1) * (active_sps->pic_height_in_map_units_minus1 + 1) * (active_sps->frame_mbs_only_flag?1:2) * 384;

  int size = 0;

  switch (active_sps->level_idc)
  {
  case 9:
    size = 152064;
    break;
  case 10:
    size = 152064;
    break;
  case 11:
    if (!( active_sps->profile_idc>=FREXT_HP || active_sps->profile_idc == FREXT_CAVLC444 ) && (active_sps->constrained_set3_flag == 1))
      size = 152064;
    else
      size = 345600;
    break;
  case 12:
    size = 912384;
    break;
  case 13:
    size = 912384;
    break;
  case 20:
    size = 912384;
    break;
  case 21:
    size = 1824768;
    break;
  case 22:
    size = 3110400;
    break;
  case 30:
    size = 3110400;
    break;
  case 31:
    size = 6912000;
    break;
  case 32:
    size = 7864320;
    break;
  case 40:
    size = 12582912;
    break;
  case 41:
    size = 12582912;
    break;
  case 42:
    size = 13369344;
    break;
  case 50:
    size = 42393600;
    break;
  case 51:
    size = 70778880;
    break;
  default:
    break;
  }

  size /= pic_size;
  size = size < 16 ? size : 16;
  size = size > 1 ? size : 1;

  return size;
}

static const VO_U8 VO_UE_VAL[512]={
	31,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
	7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9,10,10,10,10,11,11,11,11,12,12,12,12,13,13,13,13,14,14,14,14,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const VO_U8 VO_BIG_LEN_TAB[256]={
	0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
};

static signed long vo_big_len(unsigned long val)
{
	int len = 0;
	if (val & 0xffff0000) {
		val >>= 16;
		len += 16;
	}
	if (val & 0xff00) {
		val >>= 8;
		len += 8;
	}
	len += VO_BIG_LEN_TAB[val];

	return len;
}

static int ParserGet264Frame(char* currPos,int size,VO_FRAME_Params *frame_info)
{
	char* endPos = currPos+size-4;

	char* p = currPos;  
	unsigned char * tmp_ptr;
	unsigned int bFind = 0;
	unsigned int value;
    frame_info->total_nal = 0;
	frame_info->frame_size = 0;
	do 
	{
		for (; p < endPos; p++)
		{
			if (PARSER_IS_ANNEXB(p))
			{
				tmp_ptr = (unsigned char *)(p + 3);
				frame_info->nal_start_offset[frame_info->total_nal] = p-currPos;
				frame_info->total_nal++;
				
				break;
			}
			if (PARSER_IS_ANNEXB2(p))
			{
				tmp_ptr = (unsigned char *)(p + 4);
				frame_info->nal_start_offset[frame_info->total_nal] = p-currPos;
				frame_info->total_nal++;
				break;
			}
		}
		if(p>=endPos || tmp_ptr>=(unsigned char *)endPos)	//need 5 bytes to parse
			return VO_ERR_VIDEOPARSER_SMALLBUFFER;
		value = (*tmp_ptr++)&0x1F;
		if (value == 1 || value == 5)	//slice or IDR
		{
			value = (((unsigned int)(*tmp_ptr))<<24) | ((unsigned int)(*(tmp_ptr+1))<<16) | ((unsigned int)(*(tmp_ptr+2))<<8) | ((unsigned int)*(tmp_ptr+3));
			if(value >=(1<<27))
			{
				value >>= 32-9;
				value = VO_UE_VAL[value];
			}
			else
			{
				value >>= 2*vo_big_len(value)-31;
				value--;
			}
			if (value == 0)	//start_mb_nr = 0, find a new pic
			{
				if (bFind)
				{
					frame_info->frame_size = p - currPos;
					break;
				}
				else
				{
					bFind = 1;
				}
			}
		}
		else if (bFind && value<10)
		{
			frame_info->frame_size = p - currPos;
			break;
		}
		p+=3;
	} while (1);
    if(bFind)
	{
        frame_info->total_nal--;
		return VO_ERR_NONE;
	}
	else
	    return VO_ERR_VIDEOPARSER_SMALLBUFFER;
}

int voH264ParserProcessNalu(VO_H264PARSER_PARAM *pVideoInfo,VO_U8* inBuffer,VO_S32 bufferSize,VO_U32* hasSPS,VO_U32* hasFrame)
{
	VO_S32 forbiddenBit;
	VO_S32 nalReferenceIdc;
	VO_S32 nalUnitType;
	forbiddenBit    = (*(inBuffer) >> 7) & 1;
	nalReferenceIdc = ((*(inBuffer) >> 5) & 3);
	nalUnitType     = (ParserNaluType) ((*(inBuffer)) & 0x1f);
    
	switch (nalUnitType)
	{
	case NALU_PARSER_SLICE:
	if(*hasFrame==0)
	{
		int ret = ParserGetFrameType(inBuffer+1,bufferSize-1);
		if ( ret != -1)
		{
			*hasFrame = 1;
			if(ret == 0)
			    pVideoInfo->nFrame_type = VO_VIDEO_FRAME_P;
			else if(ret == 1)
				pVideoInfo->nFrame_type = VO_VIDEO_FRAME_B;
			else if(ret == 2)
				pVideoInfo->nFrame_type = VO_VIDEO_FRAME_I;
			else
				pVideoInfo->nFrame_type = VO_VIDEO_FRAME_S;
			pVideoInfo->isRefFrame = nalReferenceIdc > 0;
			pVideoInfo->isIDRFrame = 0;
			pVideoInfo->FrameHeader = 1;
		}
		break;
	}
	break;
	case NALU_PARSER_IDR:
	if(*hasFrame==0)
	{
		*hasFrame = 1;
		pVideoInfo->isRefFrame = 1;
		pVideoInfo->isIDRFrame = 1;
		pVideoInfo->nFrame_type = VO_VIDEO_FRAME_I;
		pVideoInfo->FrameHeader = 1;
		break;
	}
	break;
	case NALU_PARSER_PPS:
	{
		if(pVideoInfo->pps_buffer)
		    free(pVideoInfo->pps_buffer);
		pVideoInfo->pps_len = 4+bufferSize;
		pVideoInfo->pps_buffer=calloc(1,pVideoInfo->pps_len);
		pVideoInfo->pps_buffer[3] = 0x01;
		memcpy(&pVideoInfo->pps_buffer[4],inBuffer,bufferSize);
	    break;
	}
	case NALU_PARSER_SPS:
	{
		//parser_sps_t *sps = calloc (1, sizeof (parser_sps_t));
        parser_sps_t *sps = &pVideoInfo->in_sps;
		if(pVideoInfo->sps_buffer)
		    free(pVideoInfo->sps_buffer);
		pVideoInfo->sps_len = 4+bufferSize;
		pVideoInfo->sps_buffer=calloc(1,pVideoInfo->sps_len);
		pVideoInfo->sps_buffer[3] = 0x01;
		memcpy(&pVideoInfo->sps_buffer[4],inBuffer,bufferSize);
		if(!ParserProcessSPS(inBuffer+1,bufferSize-1,sps))
		{
			parser_out_sps_t *sps_out = &pVideoInfo->sps;
			int crop_left, crop_right, crop_top, crop_bottom;
			sps_out->direct_8x8_inference_flag = sps->direct_8x8_inference_flag;
			sps_out->frame_cropping_flag = sps->frame_cropping_flag;
			sps_out->frame_cropping_rect_bottom_offset = sps->frame_cropping_rect_bottom_offset;
            sps_out->frame_cropping_rect_left_offset = sps->frame_cropping_rect_left_offset;
			sps_out->frame_cropping_rect_right_offset = sps->frame_cropping_rect_right_offset;
			sps_out->frame_cropping_rect_top_offset = sps->frame_cropping_rect_top_offset;
            sps_out->frame_mbs_only_flag = sps->frame_mbs_only_flag;
			sps_out->gaps_in_frame_num_value_allowed_flag = sps->gaps_in_frame_num_value_allowed_flag;
			sps_out->level_idc = sps->level_idc;
			sps_out->log2_max_frame_num_minus4 = sps->log2_max_frame_num_minus4;
			sps_out->mb_adaptive_frame_field_flag = sps->mb_adaptive_frame_field_flag;
			sps_out->num_ref_frames = sps->num_ref_frames;
			sps_out->pic_height_in_map_units_minus1 = sps->pic_height_in_map_units_minus1;
			sps_out->pic_order_cnt_type = sps->pic_order_cnt_type;
			sps_out->pic_width_in_mbs_minus1 = sps->pic_width_in_mbs_minus1;
			sps_out->profile_idc = sps->profile_idc;
			sps_out->seq_parameter_set_id = sps->seq_parameter_set_id;
			sps_out->buffer_lenth = pVideoInfo->sps_len;
			sps_out->buffer = pVideoInfo->sps_buffer;
			*hasSPS = 1;
			pVideoInfo->DPB_size = getDpbSize(sps);
			switch(sps->profile_idc)
			{
				case 66:
					pVideoInfo->nProfile = VO_VIDEO_AVCProfileBaseline;
				    break;
				case 77:
					pVideoInfo->nProfile = VO_VIDEO_AVCProfileMain;
					break;
				case 88:
					pVideoInfo->nProfile = VO_VIDEO_AVCProfileExtended;
					break;
				case 100:
					pVideoInfo->nProfile = VO_VIDEO_AVCProfileHigh;
					break;
				case 110:
					pVideoInfo->nProfile = VO_VIDEO_AVCProfileHigh10;
					break;
				case 122:
					pVideoInfo->nProfile = VO_VIDEO_AVCProfileHigh422;
					break;
				case 244:
					pVideoInfo->nProfile = VO_VIDEO_AVCProfileHigh444;
					break;
				default:
					pVideoInfo->nProfile = VO_VIDEO_PROFILETYPE_MAX;
					break;
			}
			switch (sps->level_idc)
			{
				case 10:
					pVideoInfo->nLevel = VO_VIDEO_AVCLevel1;
					break;
				case 11:
					if(sps->constrained_set3_flag)
						pVideoInfo->nLevel = VO_VIDEO_AVCLevel1b;
					else
						pVideoInfo->nLevel = VO_VIDEO_AVCLevel11;
					break;
				case 12:
					pVideoInfo->nLevel = VO_VIDEO_AVCLevel12;
					break;
				case 13:
					pVideoInfo->nLevel = VO_VIDEO_AVCLevel13;
					break;
				case 20:
					pVideoInfo->nLevel = VO_VIDEO_AVCLevel2;
					break;
				case 21:
					pVideoInfo->nLevel = VO_VIDEO_AVCLevel21;
					break;
				case 22:
					pVideoInfo->nLevel = VO_VIDEO_AVCLevel22;
					break;
				case 30:
					pVideoInfo->nLevel = VO_VIDEO_AVCLevel3;
					break;
				case 31:
					pVideoInfo->nLevel = VO_VIDEO_AVCLevel31;
					break;
				case 32:
					pVideoInfo->nLevel = VO_VIDEO_AVCLevel32;
					break;
				case 40:
					pVideoInfo->nLevel = VO_VIDEO_AVCLevel4;
					break;
				case 41:
					pVideoInfo->nLevel = VO_VIDEO_AVCLevel41;
					break;
				case 42:
					pVideoInfo->nLevel = VO_VIDEO_AVCLevel42;
					break;
				case 50:
					pVideoInfo->nLevel = VO_VIDEO_AVCLevel5;
					break;
				case 51:
					pVideoInfo->nLevel = VO_VIDEO_AVCLevel51;
					break;
				default:
					pVideoInfo->nLevel = VO_VIDEO_LEVELTYPE_MAX;
					break;
			}
			if (sps->frame_cropping_flag)
			{
				crop_left   = 2 * sps->frame_cropping_rect_left_offset;
				crop_right  = 2 * sps->frame_cropping_rect_right_offset;
				crop_top	= 2 *( 2 - sps->frame_mbs_only_flag ) * sps->frame_cropping_rect_top_offset;
				crop_bottom = 2 *( 2 - sps->frame_mbs_only_flag ) * sps->frame_cropping_rect_bottom_offset;
			}
			else
			{
				crop_left = crop_right = crop_top = crop_bottom = 0;
			}
			pVideoInfo->nWidth = (sps->pic_width_in_mbs_minus1+1)*16;
			pVideoInfo->nHeight = ( 2 - sps->frame_mbs_only_flag ) * (sps->pic_height_in_map_units_minus1+1)*16;
			pVideoInfo->nWidth = pVideoInfo->nWidth-crop_left-crop_right;
			pVideoInfo->nHeight = pVideoInfo->nHeight-crop_top-crop_bottom;
            pVideoInfo->ratio.mode = 0;
			{
				vui_seq_parameters_t *  vui=&sps->vui_seq_parameters;
				if(vui)
				{
#define MAX_ASPECT_INFO_SIZE	16
                    const static int aspectInfo[MAX_ASPECT_INFO_SIZE][2]=
                    {
					  {1,1},{12,11},{10,11},{16,11},{40,33},{24,11},{20,11},{32,11},
					  {80,33},{18,11},{15,11},{64,33},{160,99},{4,3},{3,2},{2,1},
					};
					int id=vui->aspect_ratio_idc;
					int ratio=0;
					int width=0,height=0;
					if(id==0||id==1)
					{
						pVideoInfo->ratio.mode = 0;
					}
					if(id>1&&id<=MAX_ASPECT_INFO_SIZE)
					{
						width = pVideoInfo->nWidth*aspectInfo[id-1][0];
						height = pVideoInfo->nHeight*aspectInfo[id-1][1];

					}
					else if (id==255)
					{
						width = vui->sar_width*pVideoInfo->nWidth;
						height = vui->sar_height*pVideoInfo->nHeight;
					}
					if(height > 0)
				      ratio = width*10/height;

			        pVideoInfo->ratio.mode = 0;
			        if( ratio == 0 )
				      pVideoInfo->ratio.mode = 0;
			        else if( ratio <= 10 )
				      pVideoInfo->ratio.mode = 1;
			        else if( ratio <= 42/3 )
				      pVideoInfo->ratio.mode = 2;
			        else if( ratio <= 168 / 9 )
				      pVideoInfo->ratio.mode = 3;
			        else if( ratio <= 21)
				      pVideoInfo->ratio.mode = 4;
					else
					{
						pVideoInfo->ratio.mode = VO_RATIO_MAX;
						pVideoInfo->ratio.width = width;
						pVideoInfo->ratio.height = height;
					}

				}

			}
			pVideoInfo->ref_number = sps->num_ref_frames;
			pVideoInfo->isInterlace = !sps->frame_mbs_only_flag;
			pVideoInfo->seHeader = 1;
		}		
		//free(sps);
		break;
	}
	case NALU_PARSER_SEI:
	{
		if(ParserProcessSEI(inBuffer,bufferSize,pVideoInfo)==0)
		{
			//pVideoInfo->seHeader = 1;
			//*hasSPS = 1;
		}
		break;
	}
	default:
		break;
	}

	return 1;
}

VO_S32 voH264ParserInit(VO_HANDLE *pParHandle)
{
	VO_H264PARSER_PARAM *pParser = (VO_H264PARSER_PARAM*)malloc(sizeof(VO_H264PARSER_PARAM));
	if(!pParser)
		return VO_ERR_VIDEOPARSER_MEMORY; 
	memset(pParser,0,sizeof(VO_H264PARSER_PARAM));
	*pParHandle = pParser;
	return VO_ERR_NONE;
}

VO_S32 voH264ParserSetParam(VO_HANDLE pParHandle, VO_S32 uParamID, VO_PTR pData)
{
    VO_H264PARSER_PARAM *pVideoInfo = (VO_H264PARSER_PARAM *)pParHandle;
    VO_U32 returnCode = VO_ERR_NONE;
    switch(uParamID)
    {
        case VO_PID_VIDEOPARSER_GET_FRAME:
	        pVideoInfo->process_id = VO_PID_VIDEOPARSER_GET_FRAME;
	    break;
		default:
	        returnCode = VO_ERR_WRONG_PARAM_ID;
    }
    return returnCode;
}

VO_S32 voH264ParserGetFrame(VO_HANDLE pParHandle,VO_CODECBUFFER *pInData)
{
    VO_U8* inBuffer= pInData->Buffer;
	VO_U32 BufferLen = pInData->Length; 
	VO_FRAME_Params *frame_info = (VO_FRAME_Params*)pInData->UserData;
    VO_U32 ret = VO_ERR_NONE;
	
	if (BufferLen <=4)
	{
		return VO_ERR_VIDEOPARSER_SMALLBUFFER;
	}

    ret = ParserGet264Frame(inBuffer,BufferLen,frame_info);
	
	return VO_ERR_NONE;
}

VO_S32 voH264ParserProcess(VO_HANDLE pParHandle,VO_CODECBUFFER *pInData)
{
	VO_H264PARSER_PARAM *pVideoInfo = (VO_H264PARSER_PARAM *)pParHandle;
	VO_U32 hasSPS=0,hasFrame=0;
	VO_U8* inBuffer= pInData->Buffer;
	VO_U32 BufferLen = pInData->Length; 
	VO_U8* nextFrame,*currFrame;
	VO_S32 leftSize=BufferLen,inSize;
	VO_PARSER_SEI_INFO* sei_info = &pVideoInfo->sei_info;
	if(pVideoInfo->process_id == VO_PID_VIDEOPARSER_GET_FRAME)
	{
	    VO_FRAME_Params *frame_info = (VO_FRAME_Params*)pInData->UserData;
        VO_U32 ret = VO_ERR_NONE;
	
	    if (BufferLen <=4)
	    {
		    return VO_ERR_VIDEOPARSER_SMALLBUFFER;
	    }

        ret = ParserGet264Frame(inBuffer,BufferLen,frame_info);
	
	    return ret;
	}
	
	pVideoInfo->UserData.count = 0;
	pVideoInfo->nSize = BufferLen;
	pVideoInfo->HasS3D = 0;
	if(sei_info->buffer)
	  free(sei_info->buffer);
	sei_info->buffer = NULL;
	if(pVideoInfo->sps_buffer)
	  free(pVideoInfo->sps_buffer);
	pVideoInfo->sps_buffer = NULL;
	pVideoInfo->sps_len = 0;
	if(pVideoInfo->pps_buffer)
	  free(pVideoInfo->pps_buffer);
	pVideoInfo->pps_buffer = NULL;
	pVideoInfo->pps_len = 0;
	memset(sei_info,0,sizeof(VO_PARSER_SEI_INFO));
    
	if (BufferLen <=4)
	{
		return VO_ERR_VIDEOPARSER_INPUTDAT;
	}
	nextFrame = currFrame = inBuffer;
	currFrame = ParserGetNextFrame(currFrame,leftSize);
	if (currFrame!=NULL)
	{
		leftSize-=(currFrame-inBuffer);
	}
	while (leftSize>4&&currFrame!=NULL)
	{
		nextFrame = ParserGetNextFrame(currFrame,leftSize);
		if(nextFrame)
		{
			inSize = nextFrame-currFrame;
		}
		else
		{
			inSize = leftSize;
		}
		if(inSize >=2)
		  voH264ParserProcessNalu(pVideoInfo,currFrame,inSize,&hasSPS,&hasFrame);
		leftSize-=inSize;
		currFrame=nextFrame;

	}
	
	if (hasSPS&&hasFrame)
		return VO_RETURN_SQFMHEADER;
	else if(hasSPS)
		return VO_RETURN_SQHEADER;
	else if(hasFrame)
	    return VO_RETURN_FMHEADER;
	else
		return VO_ERR_VIDEOPARSER_INPUTDAT;
}

VO_S32 voH264ParserGetInfo(VO_HANDLE pParHandle,VO_S32 nID,VO_PTR pValue)
{
	VO_H264PARSER_PARAM *pParser = (VO_H264PARSER_PARAM *)pParHandle;
	switch(nID)
	{
	case VO_PID_VIDEOPARSER_SEI_INFO:
	{
		VO_PARSER_SEI_INFO* sei_info = &pParser->sei_info;
		if(sei_info->buffercount)
		{
		    VO_U32 i=0;
			VO_U8 *buffer = sei_info->buffer;
			for(i=0;i<sei_info->buffercount;i++)
			{
			  if(!sei_info->data[i].flag_group&&sei_info->data[i].flag&VO_SEI_PIC_TIMING)
			  {
			    VO_SEI_PIC_TIMING_STRUCT *pic_time = (VO_SEI_PIC_TIMING_STRUCT *)(buffer+sei_info->data[i].buffer_lenth);
				memset(pic_time,0,sei_info->data[i].has_struct);
				interpret_picture_timing_info(buffer,sei_info->data[i].buffer_lenth,pParser,pic_time);
				buffer+=sei_info->data[i].buffer_lenth+sei_info->data[i].has_struct;
			  }
			  else
			  {
			    buffer+=sei_info->data[i].buffer_lenth+sei_info->data[i].has_struct;
			  }
			}
			*((VO_PARSER_SEI_INFO*)pValue) = pParser->sei_info;
		}
		else
			return VO_ERR_VIDEOPARSER_NOVALUE;
		break;
	}
    case VO_PID_VIDEOPARSER_HEADERDATA:
	{
	  	if(pParser->sps_len&&pParser->pps_len)
	  	{
	  	  if(pParser->header.Buffer)
		  	free(pParser->header.Buffer);
		  pParser->header.Length = pParser->sps_len+pParser->pps_len;
		  pParser->header.Buffer = malloc(pParser->header.Length);
		  if(pParser->header.Buffer==NULL)
		  	return VO_ERR_VIDEOPARSER_NOVALUE;
		  memcpy(pParser->header.Buffer,pParser->sps_buffer,pParser->sps_len);
		  memcpy(pParser->header.Buffer+pParser->sps_len,pParser->pps_buffer,pParser->pps_len);
		  *((VO_CODECBUFFER*)pValue) = pParser->header;
	  	}
		else
			return VO_ERR_VIDEOPARSER_NOVALUE;
	  	break;
	}
	case VO_PID_VIDEOPARSER_SPS:
		if(pParser->seHeader)
			*((parser_out_sps_t*)pValue) = pParser->sps;
		else
			return VO_ERR_VIDEOPARSER_NOVALUE;
		break;
	case VO_PID_VIDEOPARSER_RATIO:
		if(pParser->seHeader)
			*((VO_PARSER_ASPECTRATIO*)pValue) = pParser->ratio;
		else
			return VO_ERR_VIDEOPARSER_NOVALUE;
		break;
	case VO_PID_VIDEOPARSER_PROFILE:
		if(pParser->seHeader)
		    *((VO_U32*)pValue) = pParser->nProfile;
		else
			return VO_ERR_VIDEOPARSER_NOVALUE;
		break;
	case VO_PID_VIDEOPARSER_LEVEL:
		if(pParser->seHeader)
			*((VO_U32*)pValue) = pParser->nLevel;
		else
			return VO_ERR_VIDEOPARSER_NOVALUE;
		break;
	case VO_PID_VIDEOPARSER_FRAMETYPE:
		if(pParser->FrameHeader)
			*((VO_U32*)pValue) = pParser->nFrame_type;
		else
			return VO_ERR_VIDEOPARSER_NOVALUE;
		break;
	case VO_PID_VIDEOPARSER_WIDTH:
		if(pParser->seHeader)
			*((VO_U32*)pValue) = pParser->nWidth;
		else
			return VO_ERR_VIDEOPARSER_NOVALUE;
		break;
	case VO_PID_VIDEOPARSER_HEIGHT:
		if(pParser->seHeader)
			*((VO_U32*)pValue) = pParser->nHeight;
		else
			return VO_ERR_VIDEOPARSER_NOVALUE;
		break;
	case VO_PID_VIDEOPARSER_INTERLANCE:
		if(pParser->seHeader)
			*((VO_U32*)pValue) = pParser->isInterlace;
		else
			return VO_ERR_VIDEOPARSER_NOVALUE;
		break;
	case VO_PID_VIDEOPARSER_DPB_SIZE:
		if(pParser->seHeader)
			*((VO_U32*)pValue) = pParser->DPB_size;
		else
			return VO_ERR_VIDEOPARSER_NOVALUE;
		break;
	case VO_PID_VIDEOPARSER_REFFRAME:
		if(pParser->FrameHeader)
			*((VO_U32*)pValue) = pParser->isRefFrame;
		else
			return VO_ERR_VIDEOPARSER_NOVALUE;
		break;
	case VO_PID_VIDEOPARSER_ISIDRFRAME:
		if(pParser->FrameHeader)
			*((VO_U32*)pValue) = pParser->isIDRFrame;
		else
			return VO_ERR_VIDEOPARSER_NOVALUE;
		break;
	case VO_PID_VIDEOPARSER_S3D:
		if(pParser->HasS3D)
			*((VO_S3D_Params*)pValue) = pParser->S3D;
		else
			return VO_ERR_VIDEOPARSER_NOVALUE;
		break;
	case VO_PID_VIDEOPARSER_USERDATA:
		if (pParser->UserData.count)
		{
			VO_U32 i;
			((VO_H264_USERDATA_Params*)pValue)->count= pParser->UserData.count;
			for (i = 0;i < pParser->UserData.count;i++)
			{
				((VO_H264_USERDATA_Params*)pValue)->size[i] = pParser->UserData.size[i];
			}
			((VO_H264_USERDATA_Params*)pValue)->buffer= pParser->UserData.buffer;
		}
		else
		    return VO_ERR_VIDEOPARSER_NOVALUE;
		break;
	case VO_PID_VIDEOPARSER_REFNUM:
		if(pParser->seHeader)
			*((VO_U32*)pValue) = pParser->ref_number;
		else
			return VO_ERR_VIDEOPARSER_NOVALUE;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_S32 voH264ParserUninit(VO_HANDLE pParHandle)
{
	VO_H264PARSER_PARAM *pParser = (VO_H264PARSER_PARAM *)pParHandle;
	if(pParser)
	{
		if(pParser->UserData.buffer)
			free(pParser->UserData.buffer);
		if(pParser->sei_info.buffer)
			free(pParser->sei_info.buffer);
		if(pParser->sps_buffer)
	        free(pParser->sps_buffer);
		if(pParser->pps_buffer)
	        free(pParser->pps_buffer);
		if(pParser->header.Buffer)
		  	free(pParser->header.Buffer);
		free(pParser);
		pParser = NULL;
	}
	return 0;
}
