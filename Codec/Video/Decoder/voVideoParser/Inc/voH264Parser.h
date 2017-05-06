/************************************************************************
*									     *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009	     *
*									     *
************************************************************************/
/*******************************************************************************
	File:		voH264Parser.h

	Contains:	 H264 Parser Header File

	Written by:	Hanyue Yang

	Change History (most recent first):
	2012-02-17		DTT			Create file

*******************************************************************************/
#ifndef  __VO_H264_PARSER_H__
#define  __VO_H264_PARSER_H__
#include "voVideoParser.h"
#include "voVideoParserDecID.h"

typedef enum {
	FREXT_CAVLC444 = 44,       //!< YUV 4:4:4/14 "CAVLC 4:4:4"
	BASELINE       = 66,       //!< YUV 4:2:0/8  "Baseline"
	MAIN           = 77,       //!< YUV 4:2:0/8  "Main"
	EXTENDED       = 88,       //!< YUV 4:2:0/8  "Extended"
	FREXT_HP       = 100,      //!< YUV 4:2:0/8  "High"
	FREXT_Hi10P    = 110,      //!< YUV 4:2:0/10 "High 10"
	FREXT_Hi422    = 122,      //!< YUV 4:2:2/10 "High 4:2:2"
	FREXT_Hi444    = 244,      //!< YUV 4:4:4/14 "High 4:4:4"
	MVC_HIGH       = 118,      //!< YUV 4:2:0/8  "Multiview High"
	STEREO_HIGH    = 128       //!< YUV 4:2:0/8  "Stereo High"
} ParserH264ProfileIDC;

typedef struct
{
	unsigned int cpb_cnt_minus1;                                   // ue(v)
	unsigned int bit_rate_scale;                                   // u(4)
	unsigned int cpb_size_scale;                                   // u(4)
	unsigned int bit_rate_value_minus1 [32];    // ue(v)
	unsigned int cpb_size_value_minus1 [32];    // ue(v)
	unsigned int cbr_flag              [32];    // u(1)
	unsigned int initial_cpb_removal_delay_length_minus1;          // u(5)
	unsigned int cpb_removal_delay_length_minus1;                  // u(5)
	unsigned int dpb_output_delay_length_minus1;                   // u(5)
	unsigned int time_offset_length;                               // u(5)
} hrd_parameters_t;

typedef struct
{
	int      aspect_ratio_info_present_flag;                   // u(1)
	unsigned int aspect_ratio_idc;                                 // u(8)
	unsigned short sar_width;                                        // u(16)
	unsigned short sar_height;                                       // u(16)
	int      overscan_info_present_flag;                       // u(1)
	int      overscan_appropriate_flag;                        // u(1)
	int      video_signal_type_present_flag;                   // u(1)
	unsigned int video_format;                                     // u(3)
	int      video_full_range_flag;                            // u(1)
	int      colour_description_present_flag;                  // u(1)
	unsigned int colour_primaries;                                 // u(8)
	unsigned int transfer_characteristics;                         // u(8)
	unsigned int matrix_coefficients;                              // u(8)
	int      chroma_location_info_present_flag;                // u(1)
	unsigned int  chroma_sample_loc_type_top_field;                // ue(v)
	unsigned int  chroma_sample_loc_type_bottom_field;             // ue(v)
	int      timing_info_present_flag;                         // u(1)
	unsigned int num_units_in_tick;                                // u(32)
	unsigned int time_scale;                                       // u(32)
	int      fixed_frame_rate_flag;                            // u(1)
	int      nal_hrd_parameters_present_flag;                  // u(1)
	hrd_parameters_t nal_hrd_parameters;                           // hrd_paramters_t
	int      vcl_hrd_parameters_present_flag;                  // u(1)
	hrd_parameters_t vcl_hrd_parameters;                           // hrd_paramters_t
	// if ((nal_hrd_parameters_present_flag || (vcl_hrd_parameters_present_flag))
	int      low_delay_hrd_flag;                               // u(1)
	int      pic_struct_present_flag;                          // u(1)
	int      bitstream_restriction_flag;                       // u(1)
	int      motion_vectors_over_pic_boundaries_flag;          // u(1)
	unsigned int max_bytes_per_pic_denom;                          // ue(v)
	unsigned int max_bits_per_mb_denom;                            // ue(v)
	unsigned int log2_max_mv_length_vertical;                      // ue(v)
	unsigned int log2_max_mv_length_horizontal;                    // ue(v)
	unsigned int num_reorder_frames;                               // ue(v)
	unsigned int max_dec_frame_buffering;                          // ue(v)
} vui_seq_parameters_t;

typedef struct
{
	int   Valid;                  // indicates the parameter set is valid

	unsigned int profile_idc;                                       // u(8)
	int   constrained_set0_flag;                                // u(1)
	int   constrained_set1_flag;                                // u(1)
	int   constrained_set2_flag;                                // u(1)
	int   constrained_set3_flag;                                // u(1)
	unsigned  int level_idc;                                        // u(8)
	unsigned  int seq_parameter_set_id;                             // ue(v)
	unsigned  int chroma_format_idc;                                // ue(v)

	int   seq_scaling_matrix_present_flag;                   // u(1)
	int       seq_scaling_list_present_flag[12];                 // u(1)
	int       ScalingList4x4[6][16];                             // se(v)
	int       ScalingList8x8[6][64];                             // se(v)
	int   UseDefaultScalingMatrix4x4Flag[6];
	int   UseDefaultScalingMatrix8x8Flag[6];

	unsigned int bit_depth_luma_minus8;                            // ue(v)
	unsigned int bit_depth_chroma_minus8;                          // ue(v)
	unsigned int log2_max_frame_num_minus4;                        // ue(v)
	unsigned int pic_order_cnt_type;
	// if( pic_order_cnt_type == 0 )
	unsigned int log2_max_pic_order_cnt_lsb_minus4;                 // ue(v)
	// else if( pic_order_cnt_type == 1 )
	int delta_pic_order_always_zero_flag;               // u(1)
	int     offset_for_non_ref_pic;                         // se(v)
	int     offset_for_top_to_bottom_field;                 // se(v)
	unsigned int num_ref_frames_in_pic_order_cnt_cycle;          // ue(v)
	// for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
	//int   offset_for_ref_frame[256];   // se(v)
	unsigned int num_ref_frames;                                   // ue(v)
	int   gaps_in_frame_num_value_allowed_flag;             // u(1)
	unsigned int pic_width_in_mbs_minus1;                          // ue(v)
	unsigned int pic_height_in_map_units_minus1;                   // ue(v)
	int   frame_mbs_only_flag;                              // u(1)
	// if( !frame_mbs_only_flag )
	int   mb_adaptive_frame_field_flag;                   // u(1)
	int   direct_8x8_inference_flag;                        // u(1)
	int   frame_cropping_flag;                              // u(1)
	unsigned int frame_cropping_rect_left_offset;                // ue(v)
	unsigned int frame_cropping_rect_right_offset;               // ue(v)
	unsigned int frame_cropping_rect_top_offset;                 // ue(v)
	unsigned int frame_cropping_rect_bottom_offset;              // ue(v)
	int   vui_parameters_present_flag;                      // u(1)
	vui_seq_parameters_t vui_seq_parameters;                  // vui_seq_parameters_t
	unsigned  separate_colour_plane_flag;                       // u(1)
} parser_sps_t;

typedef enum {
	SEI_BUFFERING_PERIOD = 0,
	SEI_PIC_TIMING,
	SEI_PAN_SCAN_RECT,
	SEI_FILLER_PAYLOAD,
	SEI_USER_DATA_REGISTERED_ITU_T_T35,
	SEI_USER_DATA_UNREGISTERED,
	SEI_RECOVERY_POINT,
	SEI_DEC_REF_PIC_MARKING_REPETITION,
	SEI_SPARE_PIC,
	SEI_SCENE_INFO,
	SEI_SUB_SEQ_INFO,
	SEI_SUB_SEQ_LAYER_CHARACTERISTICS,
	SEI_SUB_SEQ_CHARACTERISTICS,
	SEI_FULL_FRAME_FREEZE,
	SEI_FULL_FRAME_FREEZE_RELEASE,
	SEI_FULL_FRAME_SNAPSHOT,
	SEI_PROGRESSIVE_REFINEMENT_SEGMENT_START,
	SEI_PROGRESSIVE_REFINEMENT_SEGMENT_END,
	SEI_MOTION_CONSTRAINED_SLICE_GROUP_SET,
	SEI_FILM_GRAIN_CHARACTERISTICS,
	SEI_DEBLOCKING_FILTER_DISPLAY_PREFERENCE,
	SEI_STEREO_VIDEO_INFO,
	SEI_POST_FILTER_HINTS,
	SEI_TONE_MAPPING,
	SEI_SCALABILITY_INFO,
	SEI_SUB_PIC_SCALABLE_LAYER,
	SEI_NON_REQUIRED_LAYER_REP,
	SEI_PRIORITY_LAYER_INFO,
	SEI_LAYERS_NOT_PRESENT,
	SEI_LAYER_DEPENDENCY_CHANGE,
	SEI_SCALABLE_NESTING,
	SEI_BASE_LAYER_TEMPORAL_HRD,
	SEI_QUALITY_LAYER_INTEGRITY_CHECK,
	SEI_REDUNDANT_PIC_PROPERTY,
	SEI_TL0_DEP_REP_INDEX,
	SEI_TL_SWITCHING_POINT,
	SEI_PARALLEL_DECODING_INFO,
	SEI_MVC_SCALABLE_NESTING,
	SEI_VIEW_SCALABILITY_INFO,
	SEI_MULTIVIEW_SCENE_INFO,
	SEI_MULTIVIEW_ACQUISITION_INFO,
	SEI_NON_REQUIRED_VIEW_COMPONENT,
	SEI_VIEW_DEPENDENCY_CHANGE,
	SEI_OPERATION_POINTS_NOT_PRESENT,
	SEI_BASE_VIEW_TEMPORAL_HRD,
	SEI_FRAME_PACKING_ARRANGEMENT,

	SEI_MAX_ELEMENTS  //!< number of maximum syntax elements
} Parser_SEI_Type;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

VO_S32 voH264ParserInit(VO_HANDLE *pParHandle);
VO_S32 voH264ParserSetParam(VO_HANDLE pParHandle, VO_S32 uParamID, VO_PTR pData);
VO_S32 voH264ParserGetFrame(VO_HANDLE pParHandle,VO_CODECBUFFER *pInData);
VO_S32 voH264ParserProcess(VO_HANDLE pParHandle,VO_CODECBUFFER *pInData);
VO_S32 voH264ParserGetInfo(VO_HANDLE pParHandle,VO_S32 nID,VO_PTR pValue);
VO_S32 voH264ParserUninit(VO_HANDLE pParHandle);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


#endif   //__VO_H264_PARSER_H__