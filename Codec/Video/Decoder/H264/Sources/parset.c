
 #include "global.h"
#if !BUILD_WITHOUT_C_LIB
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#endif

#include "parsetcommon.h"
#include "parset.h"
#include "nalu.h"
#include "memalloc.h"
#include "fmo.h"
#include "vlc.h"
#include "mbuffer.h"

#if TRACE
#define SYMTRACESTRING(s) strncpy(sym->tracestring,s,TRACESTRING_SIZE)
#else
#define SYMTRACESTRING(s) // do nothing
#endif

avdNativeInt ReadHRDParameters(ImageParameters *img,Bitstream *s, hrd_parameters_t *hrd)
{
	avdNativeUInt SchedSelIdx;
	hrd->cpb_cnt_minus1                                      = avd_ue_v(s);
	hrd->bit_rate_scale                                      = avd_u_v(4, s);
	hrd->cpb_size_scale                                      = avd_u_v(4, s);
	if(hrd->cpb_cnt_minus1>=MAXIMUMVALUEOFcpb_cnt)
		AVD_ERROR_CHECK(img,"ReadHRDParameters:cpb_cnt_minus1>=MAXIMUMVALUEOFcpb_cnt",-100);
	for( SchedSelIdx = 0; SchedSelIdx <= hrd->cpb_cnt_minus1; SchedSelIdx++ ) 
	{
		hrd->bit_rate_value_minus1[ SchedSelIdx ]             = avd_ue_v(s);
		hrd->cpb_size_value_minus1[ SchedSelIdx ]             = avd_ue_v(s);
		hrd->cbr_flag[ SchedSelIdx ]                          = avd_u_1(s);
	}

	hrd->initial_cpb_removal_delay_length_minus1            = avd_u_v(5, s);
	hrd->cpb_removal_delay_length_minus1                    = avd_u_v(5, s);
	hrd->dpb_output_delay_length_minus1                     = avd_u_v(5, s);
	hrd->time_offset_length                                 = avd_u_v(5, s);

	return 0;
}


avdNativeInt ReadVUI(ImageParameters *img,Bitstream *s, seq_parameter_set_rbsp_t *sps)
{
	if (!sps->vui_seq_parameters)
		sps->vui_seq_parameters = (vui_seq_parameters_t *)voH264AlignedMalloc(img,93, sizeof (vui_seq_parameters_t));
	CHECK_ERR_RET_INT
    sps->vui_seq_parameters->matrix_coefficients = 2; // set to unknown;
	sps->vui_seq_parameters->aspect_ratio_info_present_flag = avd_u_1(s);
	if (sps->vui_seq_parameters->aspect_ratio_info_present_flag)
	{
		sps->vui_seq_parameters->aspect_ratio_idc             = avd_u_v(8, s);
		if (255==sps->vui_seq_parameters->aspect_ratio_idc)
		{
			sps->vui_seq_parameters->sar_width                  = avd_u_v(16, s);
			sps->vui_seq_parameters->sar_height                 = avd_u_v(16, s);
		}
	}

	sps->vui_seq_parameters->overscan_info_present_flag     = avd_u_1(s);
	if (sps->vui_seq_parameters->overscan_info_present_flag)
	{
		sps->vui_seq_parameters->overscan_appropriate_flag    = avd_u_1(s);
	}

	sps->vui_seq_parameters->video_signal_type_present_flag = avd_u_1(s);
	if (sps->vui_seq_parameters->video_signal_type_present_flag)
	{
		sps->vui_seq_parameters->video_format                    = avd_u_v(3, s);
		sps->vui_seq_parameters->video_full_range_flag           = avd_u_1(s);
		sps->vui_seq_parameters->colour_description_present_flag = avd_u_1(s);
		if(sps->vui_seq_parameters->colour_description_present_flag)
		{
			sps->vui_seq_parameters->colour_primaries              = avd_u_v(8, s);
			sps->vui_seq_parameters->transfer_characteristics      = avd_u_v(8, s);
			sps->vui_seq_parameters->matrix_coefficients           = avd_u_v(8, s);
		}
	}
	sps->vui_seq_parameters->chroma_location_info_present_flag = avd_u_1( s);
	if(sps->vui_seq_parameters->chroma_location_info_present_flag)
	{
		sps->vui_seq_parameters->chroma_sample_loc_type_top_field     = avd_ue_v(s);
		sps->vui_seq_parameters->chroma_sample_loc_type_bottom_field  = avd_ue_v(s);
	}
	sps->vui_seq_parameters->timing_info_present_flag          = avd_u_1(s);
	if (sps->vui_seq_parameters->timing_info_present_flag)
	{
		//img->seiTimeInfo.num_units_in_tick = sps->vui_seq_parameters->num_units_in_tick               = avd_u_v(32, s);
		//img->seiTimeInfo.time_scale = sps->vui_seq_parameters->time_scale										 = avd_u_v(32, s);
		//img->seiTimeInfo.fixed_frame_rate_flag =  sps->vui_seq_parameters->fixed_frame_rate_flag    = avd_u_1(s);
	}
	sps->vui_seq_parameters->nal_hrd_parameters_present_flag   = avd_u_1(s);
	if (sps->vui_seq_parameters->nal_hrd_parameters_present_flag)
	{
		ReadHRDParameters(img,s, &(sps->vui_seq_parameters->nal_hrd_parameters));CHECK_ERR_RET_INT
	}
	sps->vui_seq_parameters->vcl_hrd_parameters_present_flag   = avd_u_1(s);
	if (sps->vui_seq_parameters->vcl_hrd_parameters_present_flag)
	{
		ReadHRDParameters(img,s, &(sps->vui_seq_parameters->vcl_hrd_parameters));CHECK_ERR_RET_INT
	}
	if (sps->vui_seq_parameters->nal_hrd_parameters_present_flag || sps->vui_seq_parameters->vcl_hrd_parameters_present_flag)
	{
		sps->vui_seq_parameters->low_delay_hrd_flag             =  avd_u_1(s);
	}
	sps->vui_seq_parameters->pic_struct_present_flag          =  avd_u_1(s);
	sps->vui_seq_parameters->bitstream_restriction_flag       =  avd_u_1(s);
	if (sps->vui_seq_parameters->bitstream_restriction_flag)
	{
		sps->vui_seq_parameters->motion_vectors_over_pic_boundaries_flag =  avd_u_1( s);
		sps->vui_seq_parameters->max_bytes_per_pic_denom                 =  avd_ue_v(s);
		sps->vui_seq_parameters->max_bits_per_mb_denom                   =  avd_ue_v(s);
		sps->vui_seq_parameters->log2_max_mv_length_horizontal           =  avd_ue_v(s);
		sps->vui_seq_parameters->log2_max_mv_length_vertical             =  avd_ue_v(s);
		sps->vui_seq_parameters->num_reorder_frames                      =  avd_ue_v(s);
		sps->vui_seq_parameters->max_dec_frame_buffering                 =  avd_ue_v(s);
	}

	return 0;
}
#define AVD_ERROR_CHECKSPS(a,b,c)	\
{\
	avd_error(a,b,c);\
	if(img->error<0) goto ERROR_SPS ;\
}
//static seq_parameter_set_rbsp_t tmpSP={0};
seq_parameter_set_rbsp_t *InterpretSPS (ImageParameters *img,DataPartition *p)
{
	seq_parameter_set_rbsp_t *sps;
	seq_parameter_set_rbsp_t *active_sps;
	avdNativeUInt i, index,idAlreadyExist,profile, constrainedFlag0, constrainedFlag1, constrainedFlag2, level_idc;
	avdNativeInt reserved_zero;
	Bitstream *s = p->bitstream;
	avdUInt32	code;
	H264VdLibParam* params = img->vdLibPar;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	if(params->enableReuseValidSPS && img->active_sps)
		return img->active_sps;
	//assert (p != NULL);
	//assert (p->bitstream != NULL);
	//assert (p->bitstream->streamBuffer != 0);

	if(p==NULL||p->bitstream==NULL||p->bitstream->streamBuffer==NULL)
		AVD_ERROR_CHECK2(img,"InterpretSPS,p==NULL||p->bitstream==NULL||p->bitstream->streamBuffer==NULL",ERROR_MEMExit);

	code						= ShowBits(s, 24);
	profile                     = (code>>16); //(avdNativeUInt)avd_u_v  (8, s);
	constrainedFlag0            = (code>>15) & 0x1; //avd_u_1  (s);
	constrainedFlag1            = (code>>14) & 0x1; //avd_u_1  (s);
	constrainedFlag2            = (code>>13) & 0x1; //avd_u_1  (s);
	reserved_zero               = (code>>8)  & 0x1f; //avd_u_v  (5, s);
	level_idc                   = (code & 0xff); //avd_u_v  (8, s);
	FlushBits(s, 24);

	if(profile!=PRO_BASELINE&&profile!=PRO_MAIN&&profile!=PRO_HIGH)//it should be baseline\main\extend
		AVD_ERROR_CHECK2(img,"InterpretSPS,this profile should be baseline main and high",100);//ERROR_NotSupportProfile)

	index							= avd_ue_v (s);
	//assert (reserved_zero==0);
	//printf("sps index=%d\n",index);
	if(index<0||index>=MAXSPS)
	{
		AVD_ERROR_CHECK2(img,"InterpretSPS,SPS ID is too large",100);
		index = 0;
	}
	if(reserved_zero!=0)
		AVD_ERROR_CHECK2(img,"InterpretSPS,reserved_zero!=0",100);
	if (!img->SeqParSet[index]||!img->SeqParSet[index]->Valid){
		if(!img->SeqParSet[index])
			img->SeqParSet[index] = (seq_parameter_set_rbsp_t *)voH264AlignedMalloc(img,91, sizeof (seq_parameter_set_rbsp_t));CHECK_ERR_RET_NULL
		AVD_ALIGNED32_MEMSET(img->SeqParSet[index], 0, sizeof(seq_parameter_set_rbsp_t) / sizeof (avdInt32));
		sps = img->SeqParSet[index];
	}
	else
	{
		//AvdLog2(LL_INFO,"warning: the sps id has existed, so ignore it, which may cause the non-consistent issue@SPS\n");
		if(params->enableReuseValidSPS==0)
			sps = img->SeqParSet[index];
		else
			return img->SeqParSet[index];
	}
	
	sps->seq_parameter_set_id                   = index;

	sps->profile_idc                            = profile;
	sps->constrained_set0_flag                  = constrainedFlag0;
	sps->constrained_set1_flag                  = constrainedFlag1;
	sps->constrained_set2_flag                  = constrainedFlag2;
	sps->level_idc                              = level_idc;
	sps->chroma_format_idc						= 1;
	AvdLog2(LL_INFO,"profile_idc=%d\n",profile);
	if (profile==PRO_HIGH)
	{
		sps->chroma_format_idc = avd_ue_v (s);
		
		if(sps->chroma_format_idc==3)
		{
			int separate_colour_plane_flag =avd_u_1(s);
			avd_error(img,"chroma_format_idc==3!!",128);
		}
		{
			int bit_depth_luma_minus8			=avd_ue_v (s);
			int bit_depth_chroma_minus8		=avd_ue_v (s);
			int qpprime_y_zero_transform_bypass_flag=avd_u_1(s);
			int seq_scaling_matrix_present_flag = avd_u_1(s);
#if FEATURE_MATRIXQUANT
			sps->seq_scaling_matrix_present_flag = seq_scaling_matrix_present_flag;
			
			if(sps->seq_scaling_matrix_present_flag)
			{
				for(i=0; i<8; i++)
				{
					sps->seq_scaling_list_present_flag[i]   = avd_u_1(s);
					if(sps->seq_scaling_list_present_flag[i])
					{
						if(i<6)
							Scaling_List(sps->ScalingList4x4[i], 16, &sps->UseDefaultScalingMatrix4x4Flag[i], s);
						else
							Scaling_List(sps->ScalingList8x8[i-6], 64, &sps->UseDefaultScalingMatrix8x8Flag[i-6], s);
					}
				}
			}
#else//FEATURE_MATRIXQUANT
			if (qpprime_y_zero_transform_bypass_flag||seq_scaling_matrix_present_flag)
			{
				AVD_ERROR_CHECKSPS(img,"qpprime_y_zero_transform_bypass_flag||seq_scaling_matrix_present_flag",ERROR_InvalidPPS);
			}
#endif//FEATURE_MATRIXQUANT
		}


	}
	sps->log2_max_frame_num_minus4              = avd_ue_v (s);
	sps->pic_order_cnt_type                     = avd_ue_v (s);
	
	

	if (sps->pic_order_cnt_type == 0)
		sps->log2_max_pic_order_cnt_lsb_minus4 = (avdNativeUInt)avd_ue_v (s);
	else if (sps->pic_order_cnt_type == 1)
	{
		sps->delta_pic_order_always_zero_flag      = avd_u_1  (s);
		sps->offset_for_non_ref_pic                = avd_se_v (s);
		sps->offset_for_top_to_bottom_field        = avd_se_v (s);
		sps->num_ref_frames_in_pic_order_cnt_cycle = avd_ue_v (s);
		if(sps->offset_for_ref_frame==NULL)
			sps->offset_for_ref_frame = voH264AlignedMalloc(img,2009,sps->num_ref_frames_in_pic_order_cnt_cycle);
		for(i=0; i<sps->num_ref_frames_in_pic_order_cnt_cycle; i++)
			sps->offset_for_ref_frame[i]               = avd_se_v (s);
	}
	sps->num_ref_frames                        = avd_ue_v (s);
	sps->gaps_in_frame_num_value_allowed_flag  = avd_u_1  (s);
	sps->pic_width_in_mbs_minus1               = avd_ue_v (s);
	sps->pic_height_in_map_units_minus1        = avd_ue_v (s);
	
	i = 3;
#if ENABLE_OTHER_ALLOC
	if(params->customMem2&&params->customMem2->SetFrameCount)
	{
#ifdef H264D_IPP_DEBLOCK_PATCH
		memset(&(params->customMem2),0,sizeof(VOH264MEMOP));
#else//H264D_IPP_DEBLOCK_PATCH
		int frameNum=1;
		int ret;
		frameNum+=sps->num_ref_frames;
#if FEATURE_INTERLACE
		//frameNum+=pps->num_ref_idx_l1_active_minus1+1;
#endif//
		ret=params->customMem2->SetFrameCount(sizeInfo->width,sizeInfo->height,frameNum);
		if(ret<0)
			memset(&(params->customMem2),0,sizeof(VOH264MEMOP));
		params->customMem2->SetFrameCount=NULL;
#endif//H264D_IPP_DEBLOCK_PATCH
	}
#endif//ENABLE_OTHER_ALLOC

#if FEATURE_INTERLACE
	code									   = ShowBits(s, 4);
	sps->frame_mbs_only_flag                   = (code>>3); //avd_u_1  (s);
	if (!sps->frame_mbs_only_flag)
	{
		sps->mb_adaptive_frame_field_flag      = (code>>2) & 0x1; //avd_u_1  (s);
		AvdLog2(LL_INFO,"\n@@sps->mb_adaptive_frame_field_flag=%d\n",sps->mb_adaptive_frame_field_flag );
		i = 4;
	}
	else
		code >>= 1;
#else //VOI_H264D_NON_BASELINE
	code															 = ShowBits(s, 3);
	sps->frame_mbs_only_flag						 = (code>>2); //avd_u_1  (s);
	if(sps->frame_mbs_only_flag==0)
	{
		AVD_ERROR_CHECKSPS(img,"frame_mbs_only_flag==0",ERROR_InvalidBitstream);
	}
#endif //VOI_H264D_NON_BASELINE
	//error check
	if(sps->num_ref_frames <0||sps->num_ref_frames>16)
	{
		AVD_ERROR_CHECKSPS(img,"sps->num_ref_frames <0||sps->num_ref_frames>16",ERROR_InvalidBitstream);
	}
#if 1//def ARM//TBD,here assume the range is 
#define MAX_SIZE_INMB 128
#define MIN_SIZE_INMB 3
#else
#define MAX_SIZE_INMB 128
#define MIN_SIZE_INMB 8
#endif//ARM	
	if(img->ioInfo->outNumber>5&&sizeInfo->width>0&&sizeInfo->height>0
		&&img->active_sps->seq_parameter_set_id==sps->seq_parameter_set_id)
	{
		if((sps->pic_width_in_mbs_minus1+1)!=sizeInfo->PicWidthInMbs ||(sps->pic_height_in_map_units_minus1+1)!=sizeInfo->PicHeightInMapUnits)
			//||sps->pic_height_in_map_units_minus1 <=2||sps->pic_height_in_map_units_minus1 >128)
		{

			AVD_ERROR_CHECKSPS(img,"(sps->pic_width_in_mbs_minus1+1)!=sizeInfo->PicWidthInMbs",ERROR_InvalidBitstream);

		}
	}//
	else if (sps->pic_height_in_map_units_minus1 <MIN_SIZE_INMB||sps->pic_height_in_map_units_minus1 >MAX_SIZE_INMB
		||sps->pic_width_in_mbs_minus1<MIN_SIZE_INMB||sps->pic_width_in_mbs_minus1>MAX_SIZE_INMB)
	{

		AVD_ERROR_CHECKSPS(img,"sps->pic_width_in_mbs_minus1<0||sps->pic_width_in_mbs_minus1>10240",ERROR_InvalidBitstream);
	}

	sps->direct_8x8_inference_flag             = (code>>1) & 0x1; //avd_u_1  (s);
	sps->frame_cropping_flag                   = code & 0x1; //avd_u_1  (s);
	//assert(sps->frame_mbs_only_flag || sps->direct_8x8_inference_flag);
	//if(sps->frame_mbs_only_flag==0&&sps->direct_8x8_inference_flag==0)
	//	AVD_ERROR_CHECK(img,"InterpretSPS:sps->frame_mbs_only_flag==0&&sps->direct_8x8_inference_flag==0",ERROR_InvalidBitstream);
	FlushBits(s, i);

	if (sps->frame_cropping_flag)
	{
		
		int crop_left,crop_right,crop_top,crop_bottom;
		sps->frame_cropping_rect_left_offset      = avd_ue_v (s);
		sps->frame_cropping_rect_right_offset     = avd_ue_v (s);
		sps->frame_cropping_rect_top_offset       = avd_ue_v (s);//TODO:
		sps->frame_cropping_rect_bottom_offset    = avd_ue_v (s);
		crop_left   = SubWidthC[sps->chroma_format_idc] * sps->frame_cropping_rect_left_offset;
		crop_right  = SubWidthC[sps->chroma_format_idc] * sps->frame_cropping_rect_right_offset;
		crop_top    = SubHeightC[sps->chroma_format_idc]* ( 2 - sps->frame_mbs_only_flag )*sps->frame_cropping_rect_top_offset;
		crop_bottom = SubHeightC[sps->chroma_format_idc]* ( 2 - sps->frame_mbs_only_flag )*sps->frame_cropping_rect_bottom_offset;
		
		img->vdLibPar->sizeInfo.frameWidth  = (sps->pic_width_in_mbs_minus1+1)*16-
			crop_left-
			crop_right;
		img->vdLibPar->sizeInfo.frameHeight = (sps->pic_height_in_map_units_minus1+1)*16*(2-sps->frame_mbs_only_flag)-
			crop_top-
			crop_bottom;

	}
	else
	{
		img->vdLibPar->sizeInfo.frameWidth = (sps->pic_width_in_mbs_minus1+1)*16;
		img->vdLibPar->sizeInfo.frameHeight  = (sps->pic_height_in_map_units_minus1+1)*16*(2-sps->frame_mbs_only_flag);//sps->frame_mbs_only_flag? (sps->pic_height_in_map_units_minus1+1)*16 :(sps->pic_height_in_map_units_minus1+1)*8 ;
		
	}
	AvdLog2(LL_INFO,"@@width=%d,height=%d\n",img->vdLibPar->sizeInfo.frameWidth,img->vdLibPar->sizeInfo.frameHeight);
	sps->vui_parameters_present_flag            = avd_u_1  (s);
	if (sps->vui_parameters_present_flag)
		ReadVUI(img,s, sps);
ERROR_SPS:
	if(img->error<0)
	{
		if (sps->vui_seq_parameters)
		{
			SafevoH264AlignedFree(img,sps->vui_seq_parameters);
		}
		if(sps->offset_for_ref_frame)
		{
			SafevoH264AlignedFree(img,sps->offset_for_ref_frame);
		}
		SafevoH264AlignedFree(img,img->SeqParSet[index]);
		return NULL;
	}
	sps->Valid = VOI_TRUE;
#if TEST_TIME
	img->profile.profile = sps->profile_idc;
	img->profile.refNum  = sps->num_ref_frames;
	img->profile.interlace		 = sps->frame_mbs_only_flag==0;
	img->profile.mbaff  = sps->mb_adaptive_frame_field_flag;
	img->profile.width  = img->vdLibPar->sizeInfo.frameWidth;
	img->profile.height = img->vdLibPar->sizeInfo.frameHeight;
		
#endif
#if (DUMP_VERSION & DUMP_SEQUENCE)
	AvdLog(DUMP_SEQUENCE,0, "\n********************* NALU_TYPE_SPS(::InterpretSPS) *********************************");
	AvdLog(DUMP_SEQUENCE,0, "\nprofile_idc                              = %d", sps->profile_idc);
	AvdLog(DUMP_SEQUENCE,0, "\nconstrained_set0_flag                    = %d", sps->constrained_set0_flag);
	AvdLog(DUMP_SEQUENCE,0, "\nconstrained_set1_flag                    = %d", sps->constrained_set1_flag);
	AvdLog(DUMP_SEQUENCE,0, "\nconstrained_set2_flag                    = %d", sps->constrained_set2_flag);
	AvdLog(DUMP_SEQUENCE,0, "\nseq_parameter_set_id                     = %d", sps->seq_parameter_set_id);
	AvdLog(DUMP_SEQUENCE,0, "\nlog2_max_frame_num_minus4                = %d", sps->log2_max_frame_num_minus4);
	AvdLog(DUMP_SEQUENCE,0, "\npic_order_cnt_type                       = %d", sps->pic_order_cnt_type);
	if (sps->pic_order_cnt_type == 0)
		AvdLog(DUMP_SEQUENCE,0, "\nlog2_max_pic_order_cnt_lsb_minus4        = %d", sps->log2_max_pic_order_cnt_lsb_minus4);
	else if (sps->pic_order_cnt_type == 1)
	{
		AvdLog(DUMP_SEQUENCE,0, "\ndelta_pic_order_always_zero_flag         = %d", sps->delta_pic_order_always_zero_flag);
		AvdLog(DUMP_SEQUENCE,0, "\noffset_for_non_ref_pic                   = %d", sps->offset_for_non_ref_pic);
		AvdLog(DUMP_SEQUENCE,0, "\noffset_for_top_to_bottom_field           = %d", sps->offset_for_top_to_bottom_field);
		AvdLog(DUMP_SEQUENCE,0, "\nnum_ref_frames_in_pic_order_cnt_cycle    = %d", sps->num_ref_frames_in_pic_order_cnt_cycle);
		for(i=0; i<sps->num_ref_frames_in_pic_order_cnt_cycle; i++)
			AvdLog(DUMP_SEQUENCE,0, "\noffset_for_ref_frame(%d)                  = %d", i, sps->offset_for_ref_frame[i]);
	}
	AvdLog(DUMP_SEQUENCE,0, "\nnum_ref_frames                           = %d", sps->num_ref_frames);
	AvdLog(DUMP_SEQUENCE,0, "\ngaps_in_frame_num_value_allowed_flag     = %d", sps->gaps_in_frame_num_value_allowed_flag);
	AvdLog(DUMP_SEQUENCE,0, "\npic_width_in_mbs_minus1                  = %d", sps->pic_width_in_mbs_minus1);
	AvdLog(DUMP_SEQUENCE,0, "\npic_height_in_map_units_minus1           = %d", sps->pic_height_in_map_units_minus1);
	AvdLog(DUMP_SEQUENCE,0, "\nframe_mbs_only_flag                      = %d", sps->frame_mbs_only_flag);
	if (!sps->frame_mbs_only_flag)
	{
		AvdLog(DUMP_SEQUENCE,0, "\nmb_adaptive_frame_field_flag             = %d", sps->mb_adaptive_frame_field_flag);
	}
	AvdLog(DUMP_SEQUENCE,0, "\ndirect_8x8_inference_flag                = %d", sps->direct_8x8_inference_flag);
	AvdLog(DUMP_SEQUENCE,0, "\nframe_cropping_flag                      = %d", sps->frame_cropping_flag);
	if (sps->frame_cropping_flag)
	{
		AvdLog(DUMP_SEQUENCE,0, "\nframe_cropping_rect_left_offset          = %d", sps->frame_cropping_rect_left_offset);
		AvdLog(DUMP_SEQUENCE,0, "\nframe_cropping_rect_right_offset         = %d", sps->frame_cropping_rect_right_offset);
		AvdLog(DUMP_SEQUENCE,0, "\nframe_cropping_rect_top_offset           = %d", sps->frame_cropping_rect_top_offset);
		AvdLog(DUMP_SEQUENCE,0, "\nframe_cropping_rect_bottom_offset        = %d", sps->frame_cropping_rect_bottom_offset);
	}
	AvdLog(DUMP_SEQUENCE,0, "\nvui_parameters_present_flag              = %d", sps->vui_parameters_present_flag);	
	AvdLog(DUMP_SEQUENCE,0, "\n\n  ...end SPS...");
//	fflush(_dmpSeqFp);
#endif //(DUMP_VERSION & DUMP_SEQUENCE)
	//
  return img->SeqParSet[index];
}


pic_parameter_set_rbsp_t *InterpretPPS (ImageParameters *img,DataPartition *p)
{
	Bitstream *s = p->bitstream;
	pic_parameter_set_rbsp_t *pps;
	avdNativeUInt i, id;
	avdNativeUInt NumberBitsPerSliceGroupId;
	avdUInt32	code;
	if(img->vdLibPar->enableReuseValidSPS && img->active_pps)
		return img->active_pps;
	if(p==NULL||p->bitstream==NULL||p->bitstream->streamBuffer==NULL)
		AVD_ERROR_CHECK2(img,"InterpretPPS,p==NULL||p->bitstream==NULL||p->bitstream->streamBuffer==NULL",ERROR_MEMExit);
	
	// read id first;
	id					                  = avd_ue_v (s);
	if(id<0||id>(MAXPPS-1))
	{
		AVD_ERROR_CHECK2(img,"InterpretPPS,PPS ID is too large",100);
		id = 0;
	}
	if (!img->PicParSet[id]||!img->PicParSet[id]->Valid){
		if(!img->PicParSet[id])
			img->PicParSet[id] = (pic_parameter_set_rbsp_t *) voH264AlignedMalloc(img,92, sizeof (pic_parameter_set_rbsp_t));
		AVD_ALIGNED32_MEMSET(img->PicParSet[id], 0, sizeof(pic_parameter_set_rbsp_t) / sizeof (avdInt32));
		pps = img->PicParSet[id];
	}
	else {
		//AvdLog2(LL_INFO,"warning: the pps id has existed, so ignore it, which may cause the non-consistent issue\n");
		if(img->vdLibPar->enableReuseValidSPS==0)
			pps = img->PicParSet[id];
		else
			return img->PicParSet[id];
	}
	
	pps->pic_parameter_set_id                  = id;

	pps->seq_parameter_set_id                  = avd_ue_v (s);
	code = ShowBits(s, 2);
	pps->entropy_coding_mode_flag              = (code>>1); //avd_u_1  (s);
	//! Note: as per JVT-F078 the following bit is unconditional.  If F078 is not accepted, then
	//! one has to fetch the correct SPS to check whether the bit is present (hopefully there is
	//! no consistency problem :-(
	//! The current encoder code handles this in the same way.  When you change this, don't forget
	//! the encoder!  StW, 12/8/02
	pps->pic_order_present_flag                = (code & 1);  //avd_u_1  (s);
	FlushBits(s, 2);
	pps->num_slice_groups_minus1               = avd_ue_v (s);


	if(pps->num_slice_groups_minus1 != 0)
		AVD_ERROR_CHECK2(img,"InterpretPPS,pps->num_slice_groups_minus1 != 0",ERROR_NotSupportFMO);


	// End of FMO stuff

	pps->num_ref_idx_l0_active_minus1          = avd_ue_v (s);
	if(pps->num_ref_idx_l0_active_minus1<0||pps->num_ref_idx_l0_active_minus1>=MAX_REFERENCE_FRAMES)
	{
		AVD_ERROR_CHECK2(img,"pps->num_ref_idx_l0_active_minus1 is not valid",ERROR_InvalidBitstream);
	}
	pps->num_ref_idx_l1_active_minus1          = avd_ue_v (s);
	if(pps->num_ref_idx_l1_active_minus1<0||pps->num_ref_idx_l1_active_minus1>=MAX_REFERENCE_FRAMES)
	{
		AVD_ERROR_CHECK2(img,"pps->num_ref_idx_l1_active_minus1 is not valid",ERROR_InvalidBitstream);
	}
	code = ShowBits(s, 3);
	pps->weighted_pred_flag                    = (code>>2); //avd_u_1  (s);
	pps->weighted_bipred_idc                   = (code&0x3); //avd_u_v  ( 2, s);
	FlushBits(s, 3);
	pps->pic_init_qp_minus26                   = (avdNativeInt)avd_se_v (s);
	pps->pic_init_qs_minus26                   = (avdNativeInt)avd_se_v (s);
	pps->chroma_qp_index_offset                = (avdNativeInt)avd_se_v (s);
	//error check
	if(pps->pic_init_qp_minus26<-26||pps->pic_init_qp_minus26>=26||pps->pic_init_qs_minus26<-26||pps->pic_init_qs_minus26>=26)
	{
		AVD_ERROR_CHECK2(img,"pps->pic_init_qp_minus26 is not valid",ERROR_InvalidBitstream);
	}
	code = ShowBits(s, 3);
	pps->deblocking_filter_control_present_flag = (code>>2); //avd_u_1 (s);
	pps->constrained_intra_pred_flag            = (code>>1) & 0x1; //avd_u_1  (s);
	pps->redundant_pic_cnt_present_flag         = code & 0x1; //avd_u_1  (s);
	FlushBits(s, 3);
	if(pps->seq_parameter_set_id<0||pps->seq_parameter_set_id>=MAXSPS)
	{
		AVD_ERROR_CHECK2(img,"pps->seq_parameter_set_id is not valid",100);
		pps->seq_parameter_set_id = 0;
	}
	if (img->SeqParSet[pps->seq_parameter_set_id]==NULL)
	{
		AVD_ERROR_CHECK2(img,"InterpretPPS,pps->seq_parameter_set_id does not exist",ERROR_SPSIsNULL);
	}
#if FEATURE_BFRAME
	if (img->SeqParSet[pps->seq_parameter_set_id]->profile_idc==PRO_HIGH&&more_rbsp_data(s->streamBuffer, s->frame_bitoffset,s->code_len))
	{
		//TMBBitStream	*mbBits = GetMBBits(currMB);
		code = ShowBits(s, 2);
		FlushBits(s, 2);
		img->Transform8x8Mode=(code>>1) & 0x1; 
		{
			int pic_scaling_matrix_present_flag=code & 0x1;
#if FEATURE_MATRIXQUANT
			pps->pic_scaling_matrix_present_flag   =  pic_scaling_matrix_present_flag;

			if(pps->pic_scaling_matrix_present_flag)
			{
				int n_ScalingList = 6 + 2 * img->Transform8x8Mode;
				for(i=0; i<n_ScalingList; i++)
				{
					pps->pic_scaling_list_present_flag[i]= avd_u_1  (s);

					if(pps->pic_scaling_list_present_flag[i])
					{
						if(i<6)
							Scaling_List(pps->ScalingList4x4[i], 16, &pps->UseDefaultScalingMatrix4x4Flag[i], s);
						else
							Scaling_List(pps->ScalingList8x8[i-6], 64, &pps->UseDefaultScalingMatrix8x8Flag[i-6], s);
					}
				}
			}
			
#else//FEATURE_MATRIXQUANT
			if (pic_scaling_matrix_present_flag)
			{
				AVD_ERROR_CHECK2(img,"pic_scaling_matrix_present_flag!!",100);//ERROR_InvalidPPS);
			}
#endif//FEATURE_MATRIXQUANT
		}
		//pps->second_chroma_qp_index_offset      = (avdNativeInt)avd_se_v (s);
	}
#endif//#if FEATURE_BFRAME	
	if(img->active_pps&&img->ioInfo->outNumber>10&&(img->active_pps->pic_parameter_set_id==pps->pic_parameter_set_id))
	{
		if(img->active_pps->num_ref_idx_l0_active_minus1!=pps->num_ref_idx_l0_active_minus1)
		{
			AVD_ERROR_CHECK2(img,"InterpretPPS,the same PPS changed",ERROR_InvalidPPS);
		}
	}
	pps->Valid = VOI_TRUE;
#if TEST_TIME
	img->profile.cabac					= pps->entropy_coding_mode_flag;
	img->profile.weight					= pps->weighted_pred_flag||pps->weighted_bipred_idc;
	img->profile.deblock_filter		= pps->deblocking_filter_control_present_flag;
					
#endif	
#if (DUMP_VERSION & DUMP_SEQUENCE)
	// dump to log file;
	AvdLog(DUMP_SEQUENCE,0, "\n\n********************* NALU_TYPE_PPS(::InterpretPPS) *********************************");
	AvdLog(DUMP_SEQUENCE,0, "\npic_parameter_set_id              = %d", pps->pic_parameter_set_id);
	AvdLog(DUMP_SEQUENCE,0, "\nseq_parameter_set_id              = %d", pps->seq_parameter_set_id);
	AvdLog(DUMP_SEQUENCE,0, "\nentropy_coding_mode_flag          = %s", pps->entropy_coding_mode_flag ? "CABAC" : "UVLC");
	AvdLog(DUMP_SEQUENCE,0, "\npic_order_present_flag            = %d", pps->pic_order_present_flag);
	AvdLog(DUMP_SEQUENCE,0, "\nnum_slice_groups_minus1           = %d", pps->num_slice_groups_minus1);
#ifndef VOI_H264D_BLOCK_FMO
	if (pps->num_slice_groups_minus1 > 0)
	{
		AvdLog(DUMP_SEQUENCE,0, "\nslice_group_map_type              = %d", pps->slice_group_map_type);
		if (pps->slice_group_map_type == 0)
		{
			for (i=0; i<=pps->num_slice_groups_minus1; i++)
				AvdLog(DUMP_SEQUENCE,0, "\nrun_length_minus1(%d)              = %d", i, pps->run_length_minus1[i]);
		}
		else if (pps->slice_group_map_type == 2)
		{
			for (i=0; i<pps->num_slice_groups_minus1; i++)
			{
				AvdLog(DUMP_SEQUENCE,0, "\ntop_left(%d)                       = %d", i, pps->top_left[i]);
				AvdLog(DUMP_SEQUENCE,0, "\nbottom_right(%d)                   = %d", i, pps->bottom_right[i]);
			}
		}
		else if (pps->slice_group_map_type == 3 ||
			 pps->slice_group_map_type == 4 ||
			 pps->slice_group_map_type == 5)
		{
			AvdLog(DUMP_SEQUENCE,0, "\nslice_group_change_direction_flag = %d", pps->slice_group_change_direction_flag);
			AvdLog(DUMP_SEQUENCE,0, "\nslice_group_change_rate_minus1    = %d", pps->slice_group_change_rate_minus1);
		}
		else if (pps->slice_group_map_type == 6)
		{
			AvdLog(DUMP_SEQUENCE,0, "\nnum_slice_group_map_units_minus1  = %d", pps->num_slice_group_map_units_minus1);
			for (i=0; i<=pps->num_slice_group_map_units_minus1; i++)
				AvdLog(DUMP_SEQUENCE,0, "\nslice_group_id(%d)                 = %d", i, pps->slice_group_id[i]);
		}
	}
#endif //VOI_H264D_BLOCK_FMO

	AvdLog(DUMP_SEQUENCE,0, "\nnum_ref_idx_l0_active_minus1      = %d", pps->num_ref_idx_l0_active_minus1);
	AvdLog(DUMP_SEQUENCE,0, "\nnum_ref_idx_l1_active_minus1      = %d", pps->num_ref_idx_l1_active_minus1);
	AvdLog(DUMP_SEQUENCE,0, "\nweighted_pred_flag                = %d", pps->weighted_pred_flag);
	AvdLog(DUMP_SEQUENCE,0, "\nweighted_bipred_idc               = %d", pps->weighted_bipred_idc);
	AvdLog(DUMP_SEQUENCE,0, "\npic_init_qp_minus26               = %d", pps->pic_init_qp_minus26);
	AvdLog(DUMP_SEQUENCE,0, "\npic_init_qs_minus26               = %d", pps->pic_init_qs_minus26);
	AvdLog(DUMP_SEQUENCE,0, "\nchroma_qp_index_offset            = %d", pps->chroma_qp_index_offset);
	AvdLog(DUMP_SEQUENCE,0, "\ndeblocking_filter_control_present_flag = %d", pps->deblocking_filter_control_present_flag);
	AvdLog(DUMP_SEQUENCE,0, "\nconstrained_intra_pred_flag       = %d", pps->constrained_intra_pred_flag);
	AvdLog(DUMP_SEQUENCE,0, "\nredundant_pic_cnt_present_flag    = %d", pps->redundant_pic_cnt_present_flag);
	AvdLog(DUMP_SEQUENCE,0, "\n\n  ...end PPS...");
//	fflush(_dmpSeqFp);
#endif //(DUMP_VERSION & DUMP_SEQUENCE)
	return pps;
}

void UpdateSize(ImageParameters *img,seq_parameter_set_rbsp_t *sps)
{
#if 1
	TSizeInfo	*sizeInfo = img->sizeInfo;
	if(sps==NULL)
		AVD_ERROR_CHECK2(img,"UpdateSize,sps==NULL",ERROR_SPSIsNULL);
	sizeInfo->PicWidthInMbs = (sps->pic_width_in_mbs_minus1 +1);
	sizeInfo->PicHeightInMapUnits = (sps->pic_height_in_map_units_minus1 +1);
	sizeInfo->FrameHeightInMbs = ( 2 - sps->frame_mbs_only_flag ) * sizeInfo->PicHeightInMapUnits;
	sizeInfo->FrameSizeInMbs = sizeInfo->PicWidthInMbs * sizeInfo->FrameHeightInMbs;

	sizeInfo->width = sizeInfo->PicWidthInMbs <<4;
	sizeInfo->width_cr = sizeInfo->PicWidthInMbs<<3;//sizeInfo->width >>1;
	sizeInfo->height = sizeInfo->FrameHeightInMbs <<4;
	sizeInfo->height_cr = sizeInfo->FrameHeightInMbs<<3;//sizeInfo->height >>1;
	
#else
	// not supported yet;
    img->bitdepth_chroma = 0;
    sizeInfo->width_cr        = 0;
    sizeInfo->height_cr       = 0;

    // Fidelity Range Extensions stuff (part 1)
    img->bitdepth_luma   = sps->bit_depth_luma_minus8 + 8;
    if (sps->chroma_format_idc != YUV400)
      img->bitdepth_chroma = sps->bit_depth_chroma_minus8 + 8;  

    specialInfo->MaxFrameNum = 1<<(sps->log2_max_frame_num_minus4+4);
    sizeInfo->PicWidthInMbs = (sps->pic_width_in_mbs_minus1 +1);
    sizeInfo->PicHeightInMapUnits = (sps->pic_height_in_map_units_minus1 +1);
    sizeInfo->FrameHeightInMbs = ( 2 - sps->frame_mbs_only_flag ) * sizeInfo->PicHeightInMapUnits;
    sizeInfo->FrameSizeInMbs = sizeInfo->PicWidthInMbs * sizeInfo->FrameHeightInMbs;
    
    img->yuv_format=sps->chroma_format_idc;

    sizeInfo->width = sizeInfo->PicWidthInMbs * MB_BLOCK_SIZE;
    sizeInfo->height = sizeInfo->FrameHeightInMbs * MB_BLOCK_SIZE;
    if (sps->chroma_format_idc == YUV420)
    {
      sizeInfo->width_cr = sizeInfo->width /2;
      sizeInfo->height_cr = sizeInfo->height / 2;
    }

    else if (sps->chroma_format_idc == YUV422)
    {
      sizeInfo->width_cr = sizeInfo->width /2;
      sizeInfo->height_cr = sizeInfo->height;
    }
    else if (sps->chroma_format_idc == YUV444)
    {
      //YUV444
      sizeInfo->width_cr = sizeInfo->width;
      sizeInfo->height_cr = sizeInfo->height;
    }
#endif	
}

void PPSConsistencyCheck (ImageParameters *img,pic_parameter_set_rbsp_t *pps)
{
  printf ("Consistency checking a picture parset, to be implemented\n");
//  if (pps->seq_parameter_set_id invalid then do something)
}

void SPSConsistencyCheck (ImageParameters *img,seq_parameter_set_rbsp_t *sps)
{
  printf ("Consistency checking a sequence parset, to be implemented\n");
}

void ProcessSPS (ImageParameters *img,NALU_t *nalu)
{
	
	DataPartition *dp = img->currentSlice->partArr;// AllocPartition(1);
	seq_parameter_set_rbsp_t *sps;
	H264VdLibParam* params = img->vdLibPar;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	InitBitStream(dp->bitstream, nalu->buf, nalu->len);
	sps = InterpretSPS (img,dp);
	
	CHECK_ERR_RET_VOID

	if (img->active_sps) {
		if (sps->seq_parameter_set_id == img->active_sps->seq_parameter_set_id) {
			if (img->dec_picture)
			{
				// this may only happen on slice loss
				//Number Huang: 20100526, it may happen that there is repeated sps
				//uninit_frame(img);
				AVD_ERROR_CHECK2(img,"new sequence head is identical with the active_sps but dec_picture is NULL",100);
			}	
	
		}
	}
	else
	{

		img->active_sps = sps;
		UpdateSize(img,sps);CHECK_ERR_RET_VOID
	}
	if(img->dpb->init_done==0)
	{

		init_global_buffers(img);CHECK_ERR_RET_VOID
		init_dpb(img);CHECK_ERR_RET_VOID
	}
	if(params->sharedMem&&params->sharedMem->Init)
	{
		int frameNum=2;
		int ret;
		frameNum+=sps->num_ref_frames;
		params->sharedMemInfo->Stride			  =sizeInfo->width;
		params->sharedMemInfo->Height		  =sizeInfo->height;
		params->sharedMemInfo->FrameCount=frameNum;
		params->sharedMemInfo->ColorType	  =VOYUV_PLANAR420;	
		ret=params->sharedMem->Init(0,params->sharedMemInfo);
		if(ret!=0)
			params->sharedMem->GetBufByIndex=NULL;
		params->sharedMem->Init=NULL;
	}
#if (DUMP_VERSION & DUMP_BITS)
	_dmpTotBits += GetSliceBitsRead();
	AvdLog(DUMP_BITS, "\nSPS bits/total = %d/%d", GetSliceBitsRead(), _dmpTotBits);
#endif //(DUMP_VERSION & DUMP_BITS)
	//FreePartition (dp, 1);
}
void ProcessPPS (ImageParameters *img,NALU_t *nalu)
{
	DataPartition *dp;
	pic_parameter_set_rbsp_t *pps;
	H264VdLibParam* params = img->vdLibPar;
	dp = img->currentSlice->partArr;//AllocPartition(1);
	InitBitStream(dp->bitstream, nalu->buf, nalu->len);
	pps = InterpretPPS (img,dp);

	CHECK_ERR_RET_VOID

#if (DUMP_VERSION & DUMP_BITS)
	_dmpTotBits += GetSliceBitsRead();
	AvdLog(DUMP_BITS, "\nPPS bits/total = %d/%d", GetSliceBitsRead(), _dmpTotBits);
#endif //(DUMP_VERSION & DUMP_BITS)
	//FreePartition (dp, 1);
}

void UpdateSizeNActivateSPS(ImageParameters *img,seq_parameter_set_rbsp_t *sps)
{
	
	UpdateSize(img,sps);CHECK_ERR_RET_VOID
	if (img->active_sps != sps){

		if (img->dec_picture)
		{
		  // this may only happen on slice loss
		  AVD_ERROR_CHECK2(img,"UpdateSizeNActivateSPS dec_picture!=0",ERRPR_DecPictureIsNotNULL);
		  //uninit_frame2(img);
		}
		img->active_sps = sps;
				
	}
	
}

//return:1:all right,-1:bitstream error
avdNativeInt UseParameterSet (ImageParameters *img,avdNativeInt PicParsetId)
{

  pic_parameter_set_rbsp_t *pps; 
  seq_parameter_set_rbsp_t *sps;
  TSpecitialInfo	*specialInfo = img->specialInfo;
  if(PicParsetId>=MAXPPS)
	  AVD_ERROR_CHECK(img,"UseParameterSet PicParsetId>=MAXPPS",ERROR_PPSIsNULL);
  pps= img->PicParSet[PicParsetId]; 
  if(pps!=0)
  {
	 if(pps->seq_parameter_set_id>=MAXSPS)
		  AVD_ERROR_CHECK(img,"pps->seq_parameter_set_id>=MAXSPS",ERROR_SPSIsNULL);
  	sps = img->SeqParSet[pps->seq_parameter_set_id];
  }
  else
  {
 	 AVD_ERROR_CHECK(img,"PPS is NULL",ERROR_PPSIsNULL) ;
  }


  if (pps->Valid != VOI_TRUE)
    AVD_ERROR_CHECK(img,"Trying to use an invalid (uninitialized) Picture Parameter Set",ERROR_InvalidPPS);
  if (!sps||sps->Valid!= VOI_TRUE)
    AVD_ERROR_CHECK(img,"SPS is NULL", ERROR_SPSIsNULL);


  specialInfo->MaxFrameNum = 1<<(sps->log2_max_frame_num_minus4+4);

  
  // POC200301
  if (sps->pic_order_cnt_type > 2)  // != 1
  {
    sps->pic_order_cnt_type = 1;
    AVD_ERROR_CHECK(img,"sps->pic_order_cnt_type > 2", ERROR_InvalidPOCType);
    //AVD_ERROR_CHECK(img,"pic_order_cnt_type != 1", -1000);
  }

  if (sps->pic_order_cnt_type == 1)
  {
    if(sps->num_ref_frames_in_pic_order_cnt_cycle >= MAXnum_ref_frames_in_pic_order_cnt_cycle)
    {
      sps->num_ref_frames_in_pic_order_cnt_cycle = 16; 		
      AVD_ERROR_CHECK(img,"num_ref_frames_in_pic_order_cnt_cycle too large",ERROR_InvalidNumRefFrame);
    }
  }
 
  
  if (img->active_pps != pps){
    //it is not necessary to re-update the pps if they are the same and the previous one works well(img->number<0)
    //hbfTODO:

	if(img->active_pps)
	{
		int isEqual = img->active_pps->pic_parameter_set_id==pps->pic_parameter_set_id;

		if (!isEqual)
		{
			img->active_pps = pps;
			AvdLog2(LL_INFO,"@!!update the pps\n");
		}		
	}
	else
		img->active_pps = pps;
	
	
  }

  UpdateSizeNActivateSPS(img,sps);CHECK_ERR_RET_INT

  // currSlice->dp_mode is set by read_new_slice (NALU first byte available there)
  if (IsVLCCoding(pps))
  {
    nal_startcode_follows = uvlc_startcode_follows;
  }
#if FEATURE_CABAC
  else
  {
    nal_startcode_follows = cabac_startcode_follows;
  }
#endif
	return 1;
}
