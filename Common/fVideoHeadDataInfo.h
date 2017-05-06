#ifndef __fVideoHeadDataInfo_H__
#define __fVideoHeadDataInfo_H__


#include "voYYDef_Common.h"
#include "voVideo.h"
#include "voMPEG4.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#define	voBoolean VO_S32

/*values for possible S3D modes*/
enum 
{
  S3D_MODE_OFF = 0,
  S3D_MODE_ON = 1,
  S3D_MODE_ANAGLYPH = 2,
};

/*values for possible S3D format types*/
enum 
{
  S3D_FORMAT_NONE = 0,
  S3D_FORMAT_OVERUNDER,
  S3D_FORMAT_SIDEBYSIDE,
  S3D_FORMAT_ROW_IL,
  S3D_FORMAT_COL_IL,
  S3D_FORMAT_PIX_IL,
  S3D_FORMAT_CHECKB,
  S3D_FORMAT_FRM_SEQ,
};

/*values for possible S3D order types*/
enum 
{
  S3D_ORDER_LF = 0,
  S3D_ORDER_RF,
};

/*values for possible S3D subsampling modes*/
enum 
{
  S3D_SS_NONE = 0,
  S3D_SS_HOR,
  S3D_SS_VERT,
};

typedef struct 
{
  voBoolean active;
  VO_U32 mode;
  VO_U32 fmt;
  VO_U32 order;
  VO_U32 subsampling;
}VO_S3D_params;

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
} SEI_type;

typedef struct
{
  unsigned int  frame_packing_arrangement_id;
  voBoolean       frame_packing_arrangement_cancel_flag;
  unsigned char frame_packing_arrangement_type;
  voBoolean       quincunx_sampling_flag;
  unsigned char content_interpretation_type;
  voBoolean       spatial_flipping_flag;
  voBoolean       frame0_flipped_flag;
  voBoolean       field_views_flag;
  voBoolean       current_frame_is_frame0_flag;
  voBoolean       frame0_self_contained_flag;
  voBoolean       frame1_self_contained_flag;
  unsigned char frame0_grid_position_x;
  unsigned char frame0_grid_position_y;
  unsigned char frame1_grid_position_x;
  unsigned char frame1_grid_position_y;
  unsigned char frame_packing_arrangement_reserved_byte;
  unsigned int  frame_packing_arrangement_repetition_period;
  voBoolean       frame_packing_arrangement_extension_flag;
} frame_packing_arrangement_information_struct;
typedef struct
{
	VO_S32					Width;		/*!< Width, VO_MAX_ENUM_VALUE mean not valid */
	VO_S32					Height;		/*!< Height, VO_MAX_ENUM_VALUE mean not valid */
	VO_VIDEO_PROFILETYPE	Profile;	/*!< Profile, VO_MAX_ENUM_VALUE mean not valid */
	VO_VIDEO_LEVELTYPE		Level;		/*!< Level, VO_MAX_ENUM_VALUE mean not valid */
	VO_U32					Reserved[4];/*!< Reserved */
} VO_VIDEO_HEADDATAINFO;


typedef enum VO_VIDEO_AVCLOOPFILTERTYPE {
	VO_VIDEO_AVCLoopFilterEnable = 0,
	VO_VIDEO_AVCLoopFilterDisable,
	VO_VIDEO_AVCLoopFilterDisableSliceBoundary,
	VO_VIDEO_AVCLoopFilterMax = 0x7FFFFFFF
} VO_VIDEO_AVCLOOPFILTERTYPE;

typedef struct VO_VIDEO_PARAM_AVCTYPE {
	VO_U32 nSliceHeaderSpacing;
	VO_U32 nPFrames;
	VO_U32 nBFrames;
	VO_BOOL bUseHadamard;
	VO_U32 nRefFrames;
	VO_U32 nRefIdx10ActiveMinus1;
	VO_U32 nRefIdx11ActiveMinus1;
	VO_BOOL bEnableUEP;
	VO_BOOL bEnableFMO;
	VO_BOOL bEnableASO;
	VO_BOOL bEnableRS;
	VO_VIDEO_PROFILETYPE eProfile;
	VO_VIDEO_LEVELTYPE eLevel;
	VO_U32 nAllowedPictureTypes;
	VO_BOOL bFrameMBsOnly;
	VO_BOOL bMBAFF;
	VO_BOOL bEntropyCodingCABAC;
	VO_BOOL bWeightedPPrediction;
	VO_U32 nWeightedBipredicitonMode;
	VO_BOOL bconstIpred ;
	VO_BOOL bDirect8x8Inference;
	VO_BOOL bDirectSpatialTemporal;
	VO_U32 nCabacInitIdc;
	VO_VIDEO_AVCLOOPFILTERTYPE eLoopFilterMode;
} VO_VIDEO_PARAM_AVCTYPE;


typedef struct
{
	voBoolean   Valid;                  // indicates the parameter set is valid

	unsigned int profile_idc;                                       // u(8)
	voBoolean   constrained_set0_flag;                                // u(1)
	voBoolean   constrained_set1_flag;                                // u(1)
	voBoolean   constrained_set2_flag;                                // u(1)
	voBoolean   constrained_set3_flag;                                // u(1)
	unsigned  int level_idc;                                        // u(8)
	unsigned  int seq_parameter_set_id;                             // ue(v)
	unsigned  int chroma_format_idc;                                // ue(v)

	voBoolean   seq_scaling_matrix_present_flag;                   // u(1)
	int       seq_scaling_list_present_flag[12];                 // u(1)
	int       ScalingList4x4[6][16];                             // se(v)
	int       ScalingList8x8[6][64];                             // se(v)
	voBoolean   UseDefaultScalingMatrix4x4Flag[6];
	voBoolean   UseDefaultScalingMatrix8x8Flag[6];

	unsigned int bit_depth_luma_minus8;                            // ue(v)
	unsigned int bit_depth_chroma_minus8;                          // ue(v)
	unsigned int log2_max_frame_num_minus4;                        // ue(v)
	unsigned int pic_order_cnt_type;
	// if( pic_order_cnt_type == 0 )
	unsigned int log2_max_pic_order_cnt_lsb_minus4;                 // ue(v)
	// else if( pic_order_cnt_type == 1 )
	voBoolean delta_pic_order_always_zero_flag;               // u(1)
	int     offset_for_non_ref_pic;                         // se(v)
	int     offset_for_top_to_bottom_field;                 // se(v)
	unsigned int num_ref_frames_in_pic_order_cnt_cycle;          // ue(v)
	// for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
	//int   offset_for_ref_frame[256];   // se(v)
	unsigned int num_ref_frames;                                   // ue(v)
	voBoolean   gaps_in_frame_num_value_allowed_flag;             // u(1)
	unsigned int pic_width_in_mbs_minus1;                          // ue(v)
	unsigned int pic_height_in_map_units_minus1;                   // ue(v)
	voBoolean   frame_mbs_only_flag;                              // u(1)
	// if( !frame_mbs_only_flag )
	voBoolean   mb_adaptive_frame_field_flag;                   // u(1)
	voBoolean   direct_8x8_inference_flag;                        // u(1)
	voBoolean   frame_cropping_flag;                              // u(1)
	unsigned int frame_cropping_rect_left_offset;                // ue(v)
	unsigned int frame_cropping_rect_right_offset;               // ue(v)
	unsigned int frame_cropping_rect_top_offset;                 // ue(v)
	unsigned int frame_cropping_rect_bottom_offset;              // ue(v)
	voBoolean   vui_parameters_present_flag;                      // u(1)
	//vui_seq_parameters_t vui_seq_parameters;                  // vui_seq_parameters_t
	//unsigned  separate_colour_plane_flag;                       // u(1)
} seq_parameter_set_rbsp_t;
    
#ifdef _VONAMESPACE
#else
#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */
#endif

	VO_U32 getResolution_H264(VO_CODECBUFFER *pCodecBuf, VO_VIDEO_HEADDATAINFO *pVideoHeadDataInfo);
	VO_U32 getProfileLevel_H264(VO_CODECBUFFER *pCodecBuf, VO_VIDEO_PROFILELEVEL *pVideoProfileLevel);
	VO_U32 getVIDEO_PARAM_AVCTYPE_H264(VO_CODECBUFFER *pCodecBuf, VO_VIDEO_PARAM_AVCTYPE* pParam);
	VO_U32 IsInterlace_H264(VO_CODECBUFFER *pCodecBuf, VO_BOOL *b);
	VO_BOOL IsKeyFrame_H264(VO_PBYTE pData, VO_U32 cData, VO_U32 IDR_flag);
	VO_S32 ProcessSPS_AVC(VO_BYTE* buf, VO_S32 size, seq_parameter_set_rbsp_t *sps);
	VO_S32 getS3D_H264(VO_CODECBUFFER *pCodecBuf, VO_S3D_params *S3D);
	//---
	VO_U32 getResolution_H263(VO_CODECBUFFER *pCodecBuf, VO_VIDEO_HEADDATAINFO *pVideoHeadDataInfo);

	VO_U32 getResolution_Mpeg4(VO_CODECBUFFER *pCodecBuf, VO_VIDEO_HEADDATAINFO *pVideoHeadDataInfo);
	VO_U32 getProfileLevel_Mpeg4(VO_CODECBUFFER *pCodecBuf, VO_VIDEO_PROFILELEVEL *pVideoProfileLevel);
	VO_U32 IsInterlace_Mpeg4(VO_CODECBUFFER *pCodecBuf, VO_BOOL *b);

	//add by leon :issue 7396
	VO_U32 Mpeg4VideoInfo(VO_CODECBUFFER *pCodecBuf, VO_VIDEO_HEADDATAINFO *pVideoHeadDataInfo);

	//add by Harry :issue 10340
	//To generate a valid mpeg4 header
	//[out]codec_buf: new header buffer
	//[in]width,height,nDuration: resolution and (1/fps) are needed
	//note: 
	//     if codec_buf->Buffer is NULL, 
	//     the header size will be set into codec_buf->Length and function will return ignore other input
	void GenerateMPEG4Header(VO_CODECBUFFER* codec_buf, VO_U32 width, VO_U32 height, VO_S64 nDuration);
	//add by Harry :issue 10335
	VO_BOOL IsCorrectSequenceHeader(VO_BYTE* buf, VO_S32 size, VO_U32 *pReserve);
	//add by Harry :issue 9305
	VO_U32 Mpeg2VideoInfo(VO_CODECBUFFER *pCodecBuf, VO_VIDEO_HEADDATAINFO *pVideoHeadDataInfo);

	//add by Harry :issue 11104
	//get a mpeg2 sequence header according to input buf
	//must call "ReleaseMpeg2SequenceHead" to release it after used
	VO_U32 GetMpeg2SequenceHead(VO_PBYTE pInputBuf,VO_U32 nInputSize,VO_PBYTE* pOutputBuf,VO_U32* nOutputSize);
	//release the header what "GetMpeg2SequenceHead" output
	VO_U32 ReleaseMpeg2SequenceHead(VO_PBYTE pHeadBuf);

	VO_BOOL voIsWMVCodec(VO_CODECBUFFER *pInData);
    
#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _VONAMESPACE */

#endif // __fVideoHeadDataInfo_H__
