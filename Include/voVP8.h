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

#ifndef __VO_VP8_H__
#define __VO_VP8_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voVideo.h>

#define VO_ERR_DEC_VP8_BASE                 (VO_ERR_BASE | VO_INDEX_DEC_VP8) /*!< VP8 decoder error code base*/
#define VO_ERR_ENC_VP8_BASE                 (VO_ERR_BASE | VO_INDEX_ENC_VP8) /*!< VP8 encoder error code base*/	

/**
 * VP8 Decoder&Encoder specific return code 
 * \see common return code defined in voIndex.h
 */
typedef enum{
	/*!< Decoder*/
    VO_ERR_DEC_VP8_CODEC_ID             = VO_ERR_DEC_VP8_BASE | 0x0009,  /*!< Unsuppoted codec type*/
    VO_ERR_DEC_VP8_HEADER               = VO_ERR_DEC_VP8_BASE | 0x0001,  /*!< Decode header data error */
    VO_ERR_DEC_VP8_I_FRAME              = VO_ERR_DEC_VP8_BASE | 0x0002,  /*!< Decode I nFrame error */
    VO_ERR_DEC_VP8_P_FRAME              = VO_ERR_DEC_VP8_BASE | 0x0003,  /*!< Decode P nFrame error  */
    VO_ERR_DEC_VP8_B_FRAME              = VO_ERR_DEC_VP8_BASE | 0x0004,  /*!< Decode B nFrame error  */
    VO_ERR_DEC_VP8_S_FRAME              = VO_ERR_DEC_VP8_BASE | 0x0005,  /*!< Decode S nFrame error  */
    VO_ERR_DEC_VP8_INTRA_MB             = VO_ERR_DEC_VP8_BASE | 0x0006,  /*!< Decode Intra MB error  */
    VO_ERR_DEC_VP8_INTER_MB             = VO_ERR_DEC_VP8_BASE | 0x0007,  /*!< Decode Inter MB error */
    VO_ERR_DEC_VP8_HW_PF                = VO_ERR_DEC_VP8_BASE | 0x0008,  /*!< Error of hardware post filter */

	/*!< Encoder*/
    VO_ERR_ENC_VP8_CONFIG               = VO_ERR_ENC_VP8_BASE | 0x0011,  /*!< Encoder error of configuration */
    VO_ERR_ENC_VP8_VIDEOTYPE            = VO_ERR_ENC_VP8_BASE | 0x0012,  /*!< Encoder unsupport video type */
    VO_ERR_ENC_VP8_FRAME                = VO_ERR_ENC_VP8_BASE | 0x0013,  /*!< Error in nFrame */
    VO_ERR_ENC_STATUS_UNCHECKED         = VO_ERR_ENC_VP8_BASE | 0x0014,
    VO_ERR_ENC_VP8_NO_ENOUGH_PARA       = VO_ERR_ENC_VP8_BASE | 0x0015,  /*!< Encoder configure parameter is not enough */
    VO_ERR_ENC_VP8_PACKET_NOT_READY     = VO_ERR_ENC_VP8_BASE | 0x0016,

	VO_ERR_DEC_VP8_MAX				  = VO_MAX_ENUM_VALUE                /*!< Max value of current enum */
}VOVP8ERR;


/**
 * VP8 Encoder specific input parameter, these parameter will 
 * be used to specify some feature which influence the encoder
 * behavior.
*/

typedef enum 
{
    RealtimeQuality,
    GoodQuality,
    BestQuality
} VO_VP8_ENCODE_QUALITY;

typedef enum 
{
    EndUsageVBR,
    EndUsageCBR,
    EndUsageCQ
} VO_VP8_ENCODE_END_USAGE;

typedef enum 
{
    KeyframeModeDisabled,
    KeyframeModeAuto
} VO_VP8_ENCODE_KEYFRAME_MODE;

typedef enum
{
    RcOnePass,
    RcFirstPass,
    RcLastPass
} VO_VP8_ENCODE_PASS_MODE;

typedef enum 
{
    VP8EncStatusUnknown,     
    VP8EncStatusWait,
    VP8EncStatusPrepare,
    VP8EncStatusReady,
    VP8EncStatusEncode
} VO_VP8_ENCODER_STATUS;

#define VO_PID_DEC_VP8_BASE              (VO_PID_COMMON_BASE | VO_INDEX_DEC_VP8) /*!< VP8 decoder parameter ID base*/
#define VO_PID_ENC_VP8_BASE              (VO_PID_COMMON_BASE | VO_INDEX_ENC_VP8) /*!< VP8 encoder parameter ID base*/

/**
 * VP8 Decoder&Encoder specific parameter ID
 * \see common parameter defined in voIndex.h
*/
typedef enum
{
	/*!< Decoder*/
    VO_PID_DEC_VP8_GET_VIDEOFORMAT       = VO_PID_DEC_VP8_BASE | 0x0002,  /*!<Get Video format, the parameter is address of structure VO_VIDEO_OUTPUTINFO*/
    VO_PID_DEC_VP8_MB_SKIP               = VO_PID_DEC_VP8_BASE | 0x0004,  /*!<Get mb skip information, the parameter is address of VO_U8 point, (Invalid)*/
    VO_PID_DEC_VP8_GET_ERRNUM            = VO_PID_DEC_VP8_BASE | 0x000C,  /*!<Get error num of current nFrame, the parameter is address of VO_U32, (Invalid)*/
    VO_PID_DEC_VP8_GET_LASTVIDEOBUFFER   = VO_PID_DEC_VP8_BASE | 0x0005,  /*!<Get the last nFrame video buffer,the parameter is address of structure VO_VIDEO_BUFFER,(invalid)*/
    VO_PID_DEC_VP8_GET_LASTVIDEOINFO     = VO_PID_DEC_VP8_BASE | 0x0006,  /*!<Get the last nFrame video information,the parameter is address of structure VO_VIDEO_OUTPUTINFO,(invalid)*/
    VO_PID_DEC_VP8_PF                    = VO_PID_DEC_VP8_BASE | 0x0007,  /*!<Post filer switch, the parameter is address of VO_BOOL, 0-disable,1-software,2-hdware,(invalid)*/
    VO_PID_DEC_VP8_PF_MODE               = VO_PID_DEC_VP8_BASE | 0x0008,  /*!<Set post filter mode, (invalid)*/
    VO_PID_DEC_VP8_MEASURE_PERFORMANCE   = VO_PID_DEC_VP8_BASE | 0x0009,  /*!<Performance switch, the parameter is address of VO_U32, 0-disable, 1-microsecond, 2-cpu cycles*, (invalid)r*/
    VO_PID_DEC_VP8_GET_PERFORMANCE_INFO  = VO_PID_DEC_VP8_BASE | 0x000A,  /*!<Get performance information, (invalid)*/
    VO_PID_DEC_VP8_SET_CURRENTFRAME_GREY = VO_PID_DEC_VP8_BASE | 0x000B,  /*!<Reset current decoded error nFrame, the paramter is address of VO_U32, 1-grey, 2-the latest non-corrupted nFrame,(invalid)*/ 
    VO_PID_DEC_VP8_SET_QUERYMEM          = VO_PID_DEC_VP8_BASE | 0x000C,  /*!<Set query memory structure, the parameter is address of query memory structure, (invalid)*/           		
    VO_PID_DEC_VP8_GET_VIDEO_WIDTH       = VO_PID_DEC_VP8_BASE | 0x000D,  /*!<Get video with, the parameter is address of VO_U32,(invalid)*/    
    VO_PID_DEC_VP8_GET_VIDEO_HEIGHT      = VO_PID_DEC_VP8_BASE | 0x000E,  /*!<Get video nHeight, the parameter is address of VO_U32,(invalid)*/   
	VO_PID_DEC_VP8_THREADS               = VO_PID_DEC_VP8_BASE | 0x000F,
	VO_PID_DEC_VP8_DISABLEDEBLOCK        = VO_PID_DEC_VP8_BASE | 0x0010,
	VO_ID_VP8_FLUSH_PICS				 = VO_PID_DEC_VP8_BASE | 0x0011, /*!<flush out last frames for multi-core delayed output*/
	/*!< Encoder*/
    VO_PID_ENC_VP8_BITRATE               = VO_PID_ENC_VP8_BASE | 0x0004,  /*!<Set bit rate, the parameter is address of VO_S32*/
    VO_PID_ENC_VP8_FRAMERATE             = VO_PID_ENC_VP8_BASE | 0x0005,  /*!<Set nFrame rate, the parameter is address of float*/
    VO_PID_ENC_VP8_KEY_FRAME_INTERVAL    = VO_PID_ENC_VP8_BASE | 0x0006,  /*!<Set key nFrame interval, the parameter is address of VO_S32*/
    VO_PID_ENC_VP8_VIDEO_QUALITY         = VO_PID_ENC_VP8_BASE | 0x0007,  /*!<Set encoder video quality, the parameter is address of VOVIDEOENCQUALITY*/	
    VO_PID_ENC_VP8_FORCE_KEY_FRAME       = VO_PID_ENC_VP8_BASE | 0x0008,  /*!<Force currenct nFrame coded as key nFrame, parameter is address of VO_S32, 0-disable, 1-enable*/

    VO_PID_ENC_VP8_VOL_HEADER            = VO_PID_ENC_VP8_BASE | 0x000a,  /*!<Return VOL header data, the parameter is address of VO_VIDEO_BUFFER structure*/
    VO_PID_ENC_VP8_INPUT_ROTATION        = VO_PID_ENC_VP8_BASE | 0x000b,  /*!<Set input video rotation type, (invalid)*/
    VO_PID_ENC_VP8_VP_SIZE               = VO_PID_ENC_VP8_BASE | 0x000c,  /*!<Set video packet size, the paramter is address of VO_S32*/		
    VO_PID_ENC_VP8_WIDTH                 = VO_PID_ENC_VP8_BASE | 0x000e,  /*!<Set nWidth of video, the paramter is address of VO_S32*/
	VO_PID_ENC_VP8_HEIGHT                = VO_PID_ENC_VP8_BASE | 0x0010,  /*!<Set nHeight of video, the paramter is address of VO_S32*/
    VO_PID_ENC_VP8_PROFILE               = VO_PID_ENC_VP8_BASE | 0x0011,
    VO_PID_ENC_VP8_ERROR_RESILIENT       = VO_PID_ENC_VP8_BASE | 0x0012,  /*!<Set error resilient mode, the paramter is address of VO_S32*/
    VO_PID_ENC_VP8_DROP_FRAME_THRESHOLD  = VO_PID_ENC_VP8_BASE | 0x0013,  /*!<Set drop frame threshold, the paramter is address of VO_S32*/
    VO_PID_ENC_VP8_LAGS_IN_FRAME         = VO_PID_ENC_VP8_BASE | 0x0014,  /*!<Set encoder frame lags, the paramter is address of VO_S32*/
    VO_PID_ENC_VP8_TOKEN_PARTITIONS      = VO_PID_ENC_VP8_BASE | 0x0015,  /*!<Set encoder token partitions, the paramter is address of VO_S32*/
    VO_PID_ENC_VP8_MIN_QUANTIZER         = VO_PID_ENC_VP8_BASE | 0x0016,  /*!<*/
    VO_PID_ENC_VP8_MAX_QUANTIZER         = VO_PID_ENC_VP8_BASE | 0x0017,  /*!<*/
    VO_PID_ENC_VP8_UNDERSHOOT_PCT        = VO_PID_ENC_VP8_BASE | 0x0018,  /*!<*/
    VO_PID_ENC_VP8_OVERSHOOT_PCT         = VO_PID_ENC_VP8_BASE | 0x0019,  /*!<*/
    VO_PID_ENC_VP8_DPB_SIZE              = VO_PID_ENC_VP8_BASE | 0x001a,  /*!<Set decoder buffer size, the paramter is address of VO_S32*/
    VO_PID_ENC_VP8_DPB_INITIAL_SIZE      = VO_PID_ENC_VP8_BASE | 0x001b,  /*!<Set decoder buffer initial size, the paramter is address of VO_S32*/
    VO_PID_ENC_VP8_DPB_OPTIMAL_SIZE      = VO_PID_ENC_VP8_BASE | 0x001c,  /*!<Set decoder buffer optimal size, the paramter is address of VO_S32*/
    VO_PID_ENC_VP8_KEYFRAME_MODE         = VO_PID_ENC_VP8_BASE | 0x001d,  /*!<Set key frame size, the paramter is address of VO_S32*/
    VO_PID_ENC_VP8_KEYFRAME_MIN_INTERVAL = VO_PID_ENC_VP8_BASE | 0x001e,
    VO_PID_ENC_VP8_KEYFRAME_MAX_INTERVAL = VO_PID_ENC_VP8_BASE | 0x001f,
    VO_PID_ENC_VP8_PASS_MODE             = VO_PID_ENC_VP8_BASE | 0x0020,
    VO_PID_ENC_VP8_AUTO_ALT_REF          = VO_PID_ENC_VP8_BASE | 0x0021,
    VO_PID_ENC_VP8_ARNR_MAX_FRAMES       = VO_PID_ENC_VP8_BASE | 0x0022,
    VO_PID_ENC_VP8_ARNR_STRENGTH         = VO_PID_ENC_VP8_BASE | 0x0023,
    VO_PID_ENC_VP8_ARNR_TYPE             = VO_PID_ENC_VP8_BASE | 0x0024,
    VO_PID_ENC_VP8_CPU_USED              = VO_PID_ENC_VP8_BASE | 0x0025,
    VO_PID_ENC_VP8_STATIC_THRESHOLD      = VO_PID_ENC_VP8_BASE | 0x0026,

    VO_PID_ENC_VP8_ENCODER_STATUS        = VO_PID_ENC_VP8_BASE | 0x0040,
    VO_PID_ENC_VP8_OUTBUF_SIZE           = VO_PID_ENC_VP8_BASE | 0x0041,  /*!<Return the max out buffer size needed by encoder, the parameter is address of VO_S32 */

	VO_PID_DEC_VP8_MAX				   = VO_MAX_ENUM_VALUE                /*!< Max value of current enum */
}VOVP8PID;

/*!
 * Get video decoder API interface
 * \param pDecHandle [IN/OUT] Return the VP8 Decoder API handle.
 * \param uFlag,reserved
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetVP8DecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag);

/*!
 * Get video encoder API interface
 * \param pEncHandle  [IN/OUT] Return the VP8 Encoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetVP8EncAPI (VO_VIDEO_ENCAPI * pEncHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __VO_VP8_H__
