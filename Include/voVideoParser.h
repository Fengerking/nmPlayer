/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
#ifndef  __VO_VIDEOPARSER_H__
#define  __VO_VIDEOPARSER_H__

#include  "voVideo.h"
#include "voType.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define  VO_VIDEO_PARSER_ERR_BASE      (VO_ERR_BASE | VO_INDEX_DEC_VIDEOPASER) /*!< VP8 decoder error code base*/  
#define	VO_RETURN_SQHEADER		   0   /*input has Sequence header only*/
#define	VO_RETURN_FMHEADER		   1   /*input has Frame header only*/
#define	VO_RETURN_SQFMHEADER	   2   /*input has Sequence header and Frame header*/

#define	VO_ERR_VIDEOPARSER_CODECID			 (VO_VIDEO_PARSER_ERR_BASE | 0x0001) /*!<Unsupport CODEC */
#define	VO_ERR_VIDEOPARSER_SQHEAD			 (VO_VIDEO_PARSER_ERR_BASE | 0x0002) /*!<Sepuence Head error*/
#define	VO_ERR_VIDEOPARSER_FMHEAD  		 (VO_VIDEO_PARSER_ERR_BASE | 0x0003) /*!<Frame Head error */
#define	VO_ERR_VIDEOPARSER_MEMORY			 (VO_VIDEO_PARSER_ERR_BASE | 0x0004) /*!<Memory error */
#define	VO_ERR_VIDEOPARSER_INPUTDAT		 (VO_VIDEO_PARSER_ERR_BASE | 0x0005) /*!<Input data no SQ header or FM header,or error*/
#define	VO_ERR_VIDEOPARSER_NOTSUPPORT    (VO_VIDEO_PARSER_ERR_BASE | 0x0006) /*!< This ID is Unupport*/
#define  VO_ERR_VIDEOPARSER_NOVALUE          (VO_VIDEO_PARSER_ERR_BASE | 0x0007) /*!< Can not gain this value current */
#define	VO_ERR_VIDEOPARSER_SMALLBUFFER		 (VO_VIDEO_PARSER_ERR_BASE | 0x0008) /*!<Input data is not enough for get one frame*/


typedef struct
{
	unsigned int profile_idc;                                       // u(8)
	unsigned  int level_idc;                                        // u(8)
	unsigned  int seq_parameter_set_id;                             // ue(v)
	unsigned int log2_max_frame_num_minus4;                        // ue(v)
	unsigned int pic_order_cnt_type;
	unsigned int num_ref_frames;                                   // ue(v)
	int   gaps_in_frame_num_value_allowed_flag;             // u(1)
	unsigned int pic_width_in_mbs_minus1;                          // ue(v)
	unsigned int pic_height_in_map_units_minus1;                   // ue(v)
	int   frame_mbs_only_flag;                              // u(1)
	int   mb_adaptive_frame_field_flag;                   // u(1)
	int   direct_8x8_inference_flag;                        // u(1)
	int   frame_cropping_flag;                              // u(1)
	unsigned int frame_cropping_rect_left_offset;                // ue(v)
	unsigned int frame_cropping_rect_right_offset;               // ue(v)
	unsigned int frame_cropping_rect_top_offset;                 // ue(v)
	unsigned int frame_cropping_rect_bottom_offset;              // ue(v)
	unsigned int buffer_lenth;
	unsigned char* buffer;
} parser_out_sps_t;

//for SEI flag0
#define VO_SEI_BUFFERING_PERIOD                      0x00000001
#define VO_SEI_PIC_TIMING                            0x00000002
#define VO_SEI_PAN_SCAN_RECT                         0x00000004
#define VO_SEI_FILLER_PAYLOAD                        0x00000008
#define VO_SEI_USER_DATA_REGISTERED_ITU_T_T35        0x00000010
#define VO_SEI_USER_DATA_UNREGISTERED                0x00000020
#define VO_SEI_RECOVERY_POINT                        0x00000040
#define VO_SEI_DEC_REF_PIC_MARKING_REPETITION        0x00000080
#define VO_SEI_SPARE_PIC                             0x00000100
#define VO_SEI_SCENE_INFO                            0x00000200
#define VO_SEI_SUB_SEQ_INFO                          0x00000400
#define VO_SEI_SUB_SEQ_LAYER_CHARACTERISTICS         0x00000800
#define VO_SEI_SUB_SEQ_CHARACTERISTICS               0x00001000
#define VO_SEI_FULL_FRAME_FREEZE                     0x00002000
#define VO_SEI_FULL_FRAME_FREEZE_RELEASE             0x00004000
#define VO_SEI_FULL_FRAME_SNAPSHOT                   0x00008000
#define VO_SEI_PROGRESSIVE_REFINEMENT_SEGMENT_START  0x00010000
#define VO_SEI_PROGRESSIVE_REFINEMENT_SEGMENT_END    0x00020000
#define VO_SEI_MOTION_CONSTRAINED_SLICE_GROUP_SET    0x00040000
#define VO_SEI_FILM_GRAIN_CHARACTERISTICS            0x00080000
#define VO_SEI_DEBLOCKING_FILTER_DISPLAY_PREFERENCE  0x00100000
#define VO_SEI_STEREO_VIDEO_INFO                     0x00200000
#define VO_SEI_POST_FILTER_HINTS                     0x00400000
#define VO_SEI_TONE_MAPPING                          0x00800000
#define VO_SEI_FRAME_PACKING_ARRANGEMENT             0x01000000
//for SEI flag1
#define VO_SEI_SCALABILITY_INFO                      0x00000001               
#define VO_SEI_SUB_PIC_SCALABLE_LAYER                0x00000002
#define VO_SEI_NON_REQUIRED_LAYER_REP                0x00000004
#define VO_SEI_PRIORITY_LAYER_INFO                   0x00000008
#define VO_SEI_LAYERS_NOT_PRESENT                    0x00000010
#define VO_SEI_LAYER_DEPENDENCY_CHANGE               0x00000020
#define VO_SEI_SCALABLE_NESTING                      0x00000040
#define VO_SEI_BASE_LAYER_TEMPORAL_HRD               0x00000080
#define VO_SEI_QUALITY_LAYER_INTEGRITY_CHECK         0x00000100
#define VO_SEI_REDUNDANT_PIC_PROPERTY                0x00000200
#define VO_SEI_TL0_DEP_REP_INDEX                     0x00000400
#define VO_SEI_TL_SWITCHING_POINT                    0x00000800
#define VO_SEI_PARALLEL_DECODING_INFO                0x00001000
#define VO_SEI_MVC_SCALABLE_NESTING                  0x00002000
#define VO_SEI_VIEW_SCALABILITY_INFO                 0x00004000
#define VO_SEI_MULTIVIEW_SCENE_INFO                  0x00008000
#define VO_SEI_MULTIVIEW_ACQUISITION_INFO            0x00010000
#define VO_SEI_NON_REQUIRED_VIEW_COMPONENT           0x00020000
#define VO_SEI_VIEW_DEPENDENCY_CHANGE                0x00040000
#define VO_SEI_OPERATION_POINTS_NOT_PRESENT          0x00080000
#define VO_SEI_BASE_VIEW_TEMPORAL_HRD                0x00100000

typedef struct 
{
	VO_U32 flag0;                  //current using flags
	VO_U32 flag1;                  //MVC flags or others
	VO_U32 flag2;                  //for future use
	VO_U32 flag3;                  //for future use 
}VO_PARSER_SEI_FLAGS;

typedef struct 
{
    VO_U32 flag_group;
	VO_U32 flag;
	VO_U32 buffer_lenth;
	VO_U32 has_struct;
}VO_PARSER_SEI_DATA;

typedef struct 
{
	VO_PARSER_SEI_FLAGS flags;
	VO_PARSER_SEI_DATA data[32];
	VO_S64 time;
	VO_U32 buffercount;
	VO_U32 buffersize;
	VO_U8* buffer;
}VO_PARSER_SEI_INFO;

/*struct of clock_struct of Picture timing ISO/IEC 14496-10:2005 (E) Annex D 2.2*/
typedef struct 
{
	VO_U32 clock_time_stamp_flag;
	VO_U32 ct_type;
	VO_U32 nuit_field_based_flag;
	VO_U32 counting_type;
	VO_U32 full_timestamp_flag;
	VO_U32 discontinuity_flag; 
	VO_U32 cnt_dropped_flag; 
	VO_U32 nframes;
	VO_U32 seconds_value; 
	VO_U32 minutes_value; 
	VO_U32 hours_value; 
	VO_U32 seconds_flag; 
	VO_U32 minutes_flag; 
	VO_U32 hours_flag; 
	VO_U32 time_offset;
}VO_CLOCK_TIME_STAMP_STRUCT;

/*struct of Picture timing SEI message ISO/IEC 14496-10:2005 (E) Annex D 2.2*/
typedef struct 
{
	VO_U32 CpbDpbDelaysPresentFlag;
	VO_U32 cpb_removal_delay;
	VO_U32 dpb_output_delay;
	VO_U32 picture_structure_present_flag;
	VO_U32 picture_structure;
	VO_U32 NumClockTs;
    VO_CLOCK_TIME_STAMP_STRUCT clock_sturct[3];
}VO_SEI_PIC_TIMING_STRUCT;

typedef struct
{
	VO_U32 width;		//[in]
	VO_U32 height;		//[in]
	VO_S64 nDuration;	//[in]
	VO_CODECBUFFER Header;	//[out]
}VO_PARSER_MPEG4_GENERATE_HEAD;

enum
{ 
	VO_PID_VIDEOPARSER_VERSION    		= VO_INDEX_DEC_VIDEOPASER | 0x0001, /*!<Get video version */
	VO_PID_VIDEOPARSER_PROFILE    		= VO_INDEX_DEC_VIDEOPASER | 0x0002, /*!<Get video profile */
	VO_PID_VIDEOPARSER_LEVEL      		= VO_INDEX_DEC_VIDEOPASER | 0x0003, /*!<Get video level */
	VO_PID_VIDEOPARSER_FRAMETYPE  		= VO_INDEX_DEC_VIDEOPASER | 0x0004, /*!<Get video frame type */
	VO_PID_VIDEOPARSER_WIDTH      		= VO_INDEX_DEC_VIDEOPASER | 0x0005, /*!<Get video with */
	VO_PID_VIDEOPARSER_HEIGHT     		= VO_INDEX_DEC_VIDEOPASER | 0x0006, /*!<Get video height */
	VO_PID_VIDEOPARSER_INTERLANCE       = VO_INDEX_DEC_VIDEOPASER | 0x0007, /*!<is interlance: 0 NO ; 1 YES */
	VO_PID_VIDEOPARSER_REFFRAME         = VO_INDEX_DEC_VIDEOPASER | 0x0008, /*!<is reference frame: 0 NO ; 1 YES*/
	VO_PID_VIDEOPARSER_ISVC1            = VO_INDEX_DEC_VIDEOPASER | 0x0009, /*!<is VC1: 0 NO ; 1 YES*/   
	VO_PID_VIDEOPARSER_ISIDRFRAME       = VO_INDEX_DEC_VIDEOPASER | 0x000A, /*!<is IDRframe: 0 NO ; 1 YES*/
	VO_PID_VIDEOPARSER_S3D              = VO_INDEX_DEC_VIDEOPASER | 0x000B, /*!<Get S3D INFO, VO_S3D_Params*/ 
	VO_PID_VIDEOPARSER_USERDATA         = VO_INDEX_DEC_VIDEOPASER | 0x000C, /*!<Get USERDATA INFO, VO_USERDATA_Params*/ 
	VO_PID_VIDEOPARSER_DPB_SIZE         = VO_INDEX_DEC_VIDEOPASER | 0x000D, /*!<Get DPB size */
	VO_PID_VIDEOPARSER_HEADERDATA		= VO_INDEX_DEC_VIDEOPASER | 0x000E,  /*!<Get video header data, VO_CODECBUFFER */
	VO_PID_VIDEOPARSER_REFNUM    		= VO_INDEX_DEC_VIDEOPASER | 0x000F,  /*!Get reference frame number */
	VO_PID_VIDEOPARSER_RATIO    		= VO_INDEX_DEC_VIDEOPASER | 0x0010,  /*!Get Ratio */
	VO_PID_VIDEOPARSER_SPS              = VO_INDEX_DEC_VIDEOPASER | 0x0011,  /*!Get SPS info */
	VO_PID_VIDEOPARSER_SEI_INFO         = VO_INDEX_DEC_VIDEOPASER | 0x0012,   /*!Get SEI info */
	VO_PID_VIDEOPARSER_GENERATE_HEAD    = VO_INDEX_DEC_VIDEOPASER | 0x0013,   /*!Get generate head data, VO_PARSER_MPEG4_GENERATE_HEAD */
	VO_PID_VIDEOPARSER_GET_FRAME        = VO_INDEX_DEC_VIDEOPASER | 0x0014, /*!Get one frame in the buffer */
	VO_PID_VIDEOPARSER_DATA_PARTITION   = VO_INDEX_DEC_VIDEOPASER | 0x0015  /*!<is data partition: 0 NO ; 1 YES */
};

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
	VO_U32 mode;
	VO_U32 width;
	VO_U32 height;
}VO_PARSER_ASPECTRATIO;

typedef struct 
{
	VO_U32 active;
	VO_U32 mode;
	VO_U32 fmt;
	VO_U32 order;
	VO_U32 subsampling;
}VO_S3D_Params;

typedef struct 
{
	VO_U32 count;
	VO_U32 size[255];
	VO_U8* buffer;
}VO_H264_USERDATA_Params, VO_USERDATA_Params;

typedef struct 
{
	VO_U32 frame_size;
	VO_U32 total_nal;
	VO_U32 nal_start_offset[512];
}VO_FRAME_Params;


typedef struct
{
	/**
	* Init Video Parser
	 * \ pParHandle [out] Return the Video Parser handle.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_S32 (VO_API *Init) (VO_HANDLE * pParHandle);

	/**
	 * Set param
	 * \uParamID:  the id.
	 * \retval VO_ERR_NONE Succeeded.
	 */	
	VO_S32 (VO_API *SetParam) (VO_HANDLE pParHandle, VO_S32 uParamID, VO_PTR pData);

	/**
        * Parser Video Header
	 * \pInData[in]: input data.
	 * \retval VO_RETURN_SQHEADER,or VO_RETURN_SQFMHEADER,VO_RETURN_FMHEADER Succeeded .
	 */
	VO_S32 (VO_API *Process) (VO_HANDLE pParHandle,VO_CODECBUFFER *pInData);
	/**
	 * Get Video Header info
	 * \Vlaue[out]:  the info.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_S32 (VO_API *GetParam) (VO_HANDLE pParHandle, VO_S32 nID,VO_PTR pValue);

	/**
	 * Uninit Video Header
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_S32 (VO_API *Uninit) (VO_HANDLE pParHandle);

} VO_VIDEO_PARSERAPI;

VO_S32 VO_API yyGetVPSRFunc (VO_VIDEO_PARSERAPI * pParserHandle, VO_VIDEO_CODINGTYPE vType);
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


#endif   //__VO_VIDEOPARSER_H__

