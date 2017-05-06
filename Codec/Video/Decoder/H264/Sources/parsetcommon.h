
#ifndef _PARSETCOMMON_H_
#define _PARSETCOMMON_H_

#include "defines.h"

#define MAXIMUMPARSETRBSPSIZE   1500
#define MAXIMUMPARSETNALUSIZE   1500
#define SIZEslice_group_id      (sizeof (int) * 60000)    // should be sufficient for HUGE pictures, need one int per MB in a picture

#if 0 //because ADS will take the  enum size as 16bit,rather than 32bit
//! avdBoolean Type
typedef enum {
  VOI_FALSE,
  VOI_TRUE,
} avdBoolean;
#else
#define avdBoolean avdUInt32
#define VOI_FALSE  0
#define VOI_TRUE   1
#endif
#define MAXIMUMVALUEOFcpb_cnt   32
typedef struct
{
	avdUInt8	bit_rate_scale;										// u(4)
	avdUInt8	cpb_size_scale;										// u(4)
	avdUInt8	initial_cpb_removal_delay_length_minus1;			// u(5)
	avdUInt8	cpb_removal_delay_length_minus1;					// u(5)
	avdUInt8	dpb_output_delay_length_minus1;						// u(5)
	avdUInt8	time_offset_length;									// u(5)
	avdUInt32	cpb_cnt_minus1;										// ue(v)
	avdUInt8	cbr_flag              [MAXIMUMVALUEOFcpb_cnt];		// u(1)
	avdUInt32	bit_rate_value_minus1 [MAXIMUMVALUEOFcpb_cnt];		// ue(v)
	avdUInt32	cpb_size_value_minus1 [MAXIMUMVALUEOFcpb_cnt];		// ue(v)
} hrd_parameters_t;


typedef struct
{
	avdUInt8			aspect_ratio_info_present_flag;                   // u(1)
	avdUInt8			aspect_ratio_idc;                               // u(8)
	avdUInt8			overscan_info_present_flag;                       // u(1)
	avdUInt8			overscan_appropriate_flag;                      // u(1)
	avdUInt8			video_signal_type_present_flag;                   // u(1)
	avdUInt8			video_format;                                   // u(3)
	avdUInt8			video_full_range_flag;                          // u(1)
	avdUInt8			colour_description_present_flag;                // u(1)
	avdUInt8			colour_primaries;                             // u(8)
	avdUInt8			transfer_characteristics;                     // u(8)
	avdUInt8			matrix_coefficients;                          // u(8)
	avdUInt8			chroma_location_info_present_flag;                // u(1)
	avdUInt8			timing_info_present_flag;                         // u(1)
	avdUInt8			fixed_frame_rate_flag;                          // u(1)
	avdUInt8			nal_hrd_parameters_present_flag;                  // u(1)
	avdUInt8			vcl_hrd_parameters_present_flag;                  // u(1)
	// if ((nal_hrd_parameters_present_flag || (vcl_hrd_parameters_present_flag))
	avdUInt8			low_delay_hrd_flag;                             // u(1)
	avdUInt8			pic_struct_present_flag;                        // u(1)
	avdUInt8			bitstream_restriction_flag;                       // u(1)
	avdUInt8			motion_vectors_over_pic_boundaries_flag;        // u(1)

	avdUInt16			sar_width;                                    // u(16)
	avdUInt16			sar_height;                                   // u(16)
	avdUInt32			num_units_in_tick;                              // u(32)
	avdUInt32			time_scale;                                     // u(32)
	avdUInt32			chroma_sample_loc_type_top_field;               // ue(v)
	avdUInt32			chroma_sample_loc_type_bottom_field;            // ue(v)
	avdUInt32			max_bytes_per_pic_denom;                        // ue(v)
	avdUInt32			max_bits_per_mb_denom;                          // ue(v)
	avdUInt32			log2_max_mv_length_vertical;                    // ue(v)
	avdUInt32			log2_max_mv_length_horizontal;                  // ue(v)
	avdUInt32			num_reorder_frames;                             // ue(v)
	avdUInt32			max_dec_frame_buffering;                        // ue(v)
	hrd_parameters_t	nal_hrd_parameters;                      // hrd_paramters_t
	hrd_parameters_t	vcl_hrd_parameters;                      // hrd_paramters_t
} vui_seq_parameters_t;


#define MAXnum_slice_groups_minus1  8
typedef struct avdPPS
{
	// if( slice_group_map_type = = 3 || 4 || 5
	avdBoolean   Valid;                  // indicates the parameter set is valid
	avdBoolean   entropy_coding_mode_flag;                         // u(1)
	// if( pic_order_cnt_type < 2 )  in the sequence parameter set
	avdBoolean   pic_order_present_flag;                           // u(1)
	avdBoolean   weighted_pred_flag;                               // u(1)
	avdBoolean   weighted_bipred_idc;                              // u(2)
	avdBoolean   deblocking_filter_control_present_flag;           // u(1)
	avdBoolean   constrained_intra_pred_flag;                      // u(1)
	avdBoolean   redundant_pic_cnt_present_flag;                   // u(1)
	avdBoolean   vui_pic_parameters_flag;                          // u(1)
	avdNativeUInt  *slice_group_id;                              // complete MBAmap u(v)
	avdNativeUInt  num_slice_groups_minus1;                          // ue(v)
	avdNativeInt   pic_init_qp_minus26;                              // se(v)
	avdNativeInt   pic_init_qs_minus26;                              // se(v)
	avdNativeInt   chroma_qp_index_offset;                           // se(v)
	int				second_chroma_qp_index_offset;                    // se(v)

	avdNativeUInt  pic_parameter_set_id;                             // ue(v)
	avdNativeUInt  seq_parameter_set_id;                             // ue(v)
	avdNativeUInt  num_ref_idx_l0_active_minus1;                     // ue(v)
	avdNativeUInt  num_ref_idx_l1_active_minus1;                     // ue(v)

#if FEATURE_MATRIXQUANT
	avdInt8   pic_scaling_matrix_present_flag;                     // u(1)
	avdInt8   pic_scaling_list_present_flag[12];                   // u(1)
	int       ScalingList4x4[6][16];                               // se(v)
	int       ScalingList8x8[6][64];                               // se(v)
	avdInt8   UseDefaultScalingMatrix4x4Flag[6];
	avdInt8   UseDefaultScalingMatrix8x8Flag[6];
#endif//FEATURE_MATRIXQUANT
} pic_parameter_set_rbsp_t;


#define MAXnum_ref_frames_in_pic_order_cnt_cycle  256
typedef struct avdSPS
{
	avdBoolean   Valid;                  // indicates the parameter set is valid
	avdBoolean   constrained_set0_flag;                            // u(1)
	avdBoolean   constrained_set1_flag;                            // u(1)
	avdBoolean   constrained_set2_flag;                            // u(1)
	// else if( pic_order_cnt_type == 1 )
	avdBoolean   delta_pic_order_always_zero_flag;               // u(1)
	avdBoolean   gaps_in_frame_num_value_allowed_flag;             // u(1)
	avdBoolean   frame_mbs_only_flag;                              // u(1)
	// if( !frame_mbs_only_flag ) 
	avdBoolean   mb_adaptive_frame_field_flag;                   // u(1)
	avdBoolean   direct_8x8_inference_flag;                        // u(1)
	avdBoolean   frame_cropping_flag;                              // u(1)
	avdBoolean   vui_parameters_present_flag;                      // u(1)
	avdNativeUInt  profile_idc;                                      // u(8)
	avdNativeUInt  level_idc;                                        // u(8)
	avdNativeUInt  chroma_format_idc;
	avdNativeUInt  seq_parameter_set_id;                             // ue(v)
	avdNativeUInt  log2_max_frame_num_minus4;                        // ue(v)
	avdNativeUInt pic_order_cnt_type;
	// if( pic_order_cnt_type == 0 ) 
	avdNativeUInt log2_max_pic_order_cnt_lsb_minus4;                 // ue(v)
	avdNativeUInt  num_ref_frames_in_pic_order_cnt_cycle;          // ue(v)
	avdNativeUInt  num_ref_frames;                                   // ue(v)
	avdNativeUInt  pic_width_in_mbs_minus1;                          // ue(v)
	avdNativeUInt  pic_height_in_map_units_minus1;                   // ue(v)
	avdNativeUInt  frame_cropping_rect_left_offset;                // ue(v)
	avdNativeUInt  frame_cropping_rect_right_offset;               // ue(v)
	avdNativeUInt  frame_cropping_rect_top_offset;                 // ue(v)
	avdNativeUInt  frame_cropping_rect_bottom_offset;              // ue(v)
	avdInt32       offset_for_non_ref_pic;                         // se(v)
	avdInt32       offset_for_top_to_bottom_field;                 // se(v)
	vui_seq_parameters_t *vui_seq_parameters;                  // vui_seq_parameters_t
	// for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
	// ToDo: reduce memory, this may be too many;
	
	avdInt8			*offset_for_ref_frame;   // se(v)
	
#if FEATURE_MATRIXQUANT
	avdInt8		  seq_scaling_matrix_present_flag;                   // u(1)
	avdInt8       seq_scaling_list_present_flag[8];                 // u(1)
	int			  ScalingList4x4[6][16];                             // se(v)
	int			  ScalingList8x8[6][64];                             // se(v)
	avdInt8		  UseDefaultScalingMatrix4x4Flag[6];
	avdInt8       UseDefaultScalingMatrix8x8Flag[6];
#endif
} seq_parameter_set_rbsp_t;

#endif
