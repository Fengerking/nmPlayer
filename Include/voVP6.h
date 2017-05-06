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

#ifndef __VO_VP6_H__
#define __VO_VP6_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <voVideo.h>

#define VO_ERR_DEC_VP6_BASE                 VO_ERR_BASE | VO_INDEX_DEC_VP6
#define VO_ERR_ENC_VP6_BASE                 VO_ERR_BASE | VO_INDEX_ENC_VP6

/*!
 * Severity: bitstream errors that are resilient
 * For Decoder, 
 * For Encoder
 */
enum{

    VO_ERR_DEC_VP6_CODEC_ID             = VO_ERR_DEC_VP6_BASE | 0x0009,  /*!< Unsuppoted codec type*/
    VO_ERR_DEC_VP6_HEADER               = VO_ERR_DEC_VP6_BASE | 0x0001,  /*!< VP6 Decoder specific return code 1 */
    VO_ERR_DEC_VP6_I_FRAME              = VO_ERR_DEC_VP6_BASE | 0x0002,  /*!< VP6 Decoder specific return code 2 */
    VO_ERR_DEC_VP6_P_FRAME              = VO_ERR_DEC_VP6_BASE | 0x0003,  /*!< VP6 Decoder specific return code 3 */
    VO_ERR_DEC_VP6_B_FRAME              = VO_ERR_DEC_VP6_BASE | 0x0004,  /*!< VP6 Decoder specific return code 4 */
    VO_ERR_DEC_VP6_S_FRAME              = VO_ERR_DEC_VP6_BASE | 0x0005,  /*!< VP6 Decoder specific return code 5 */
    VO_ERR_DEC_VP6_INTRA_MB             = VO_ERR_DEC_VP6_BASE | 0x0006,  /*!< VP6 Decoder specific return code 6 */
    VO_ERR_DEC_VP6_INTER_MB             = VO_ERR_DEC_VP6_BASE | 0x0007,  /*!< VP6 Decoder specific return code 7 */
    VO_ERR_DEC_VP6_HW_PF                = VO_ERR_DEC_VP6_BASE | 0x0008,  /*!< VP6 Decoder specific return code 8 */

    VO_ERR_ENC_VP6_CONFIG               = VO_ERR_ENC_VP6_BASE | 0x0001,  /*!< VP6 Encoder error in configure */
    VO_ERR_ENC_VP6_VIDEOTYPE            = VO_ERR_ENC_VP6_BASE | 0x0002,  /*!< VP6 unsupport video type */
    VO_ERR_ENC_VP6_FRAME                = VO_ERR_ENC_VP6_BASE | 0x0003,  /*!< VP6 Encoder error in frame */
    VO_ERR_DEC_VP6_MAX				  = VO_MAX_ENUM_VALUE,
};

#define VO_PID_DEC_VP6_BASE              VO_PID_COMMON_BASE | VO_INDEX_DEC_VP6
#define VO_PID_ENC_VP6_BASE              VO_PID_COMMON_BASE | VO_INDEX_ENC_VP6

/*!
 * For Decoder, 
 * For Encoder
 */
enum
{
    VO_PID_DEC_VP6_GET_VIDEOFORMAT       = VO_PID_DEC_VP6_BASE | 0x0002,  /*!<Get Video format*/
    VO_PID_DEC_VP6_MB_SKIP               = VO_PID_DEC_VP6_BASE | 0x0004,  /*!<Get mb skip information, the parameter is point of unsighed char(size = mb_width * mb_height)*/
    VO_PID_DEC_VP6_GET_ERRNUM            = VO_PID_DEC_VP6_BASE | 0x000C,  /*!<Get error num of current frame*/
    VO_PID_DEC_VP6_GET_LASTVIDEOBUFFER   = VO_PID_DEC_VP6_BASE | 0x0005,  /*!<it will force the decoder to output the last frame video buffer immediately,the parameter is a Boolean(LONG integer)*/
    VO_PID_DEC_VP6_GET_LASTVIDEOINFO     = VO_PID_DEC_VP6_BASE | 0x0006,  /*!<it will force the decoder to output the last frame video info immediately,the parameter is a Boolean(LONG integer)*/
    VO_PID_DEC_VP6_PF                    = VO_PID_DEC_VP6_BASE | 0x0007,  /*!<Set to do post filer, the parameter is Boolean(LONG integer), 0-disable,1-software,2-hdware*/
    VO_PID_DEC_VP6_PF_MODE               = VO_PID_DEC_VP6_BASE | 0x0008,  /*!<Set post filter mode*/
    VO_PID_DEC_VP6_MEASURE_PERFORMANCE   = VO_PID_DEC_VP6_BASE | 0x0009,  /*!< 0-disable, 1-microsecond, 2-cpu cycles*, the parameter is long interger*/
    VO_PID_DEC_VP6_GET_PERFORMANCE_INFO  = VO_PID_DEC_VP6_BASE | 0x000A,  /*!<Get performance structure info, the parameter is point of structure VOVP6DECPERFORMANCE*/
    VO_PID_DEC_VP6_SET_CURRENTFRAME_GREY = VO_PID_DEC_VP6_BASE | 0x000B,  /*!<Reset current decoded error frame(which will be used for reference), 1-grey, 2-the latest non-corrupted frame*/ 
    VO_PID_DEC_VP6_SET_QUERYMEM          = VO_PID_DEC_VP6_BASE | 0x000C,  /*!<Set query memory structure, the parameter is point of query memory structure*/           		
    VO_PID_DEC_VP6_GET_VIDEO_WIDTH       = VO_PID_DEC_VP6_BASE | 0x000D,  /*!<Get video with*/    
    VO_PID_DEC_VP6_GET_VIDEO_HEIGHT      = VO_PID_DEC_VP6_BASE | 0x000E,  /*!<Get video height*/ 
    VO_PID_DEC_VP6_SET_VIDEO_WIDTH       = VO_PID_DEC_VP6_BASE | 0x000F,  /*!<Set video with*/    
    VO_PID_DEC_VP6_SET_VIDEO_HEIGHT      = VO_PID_DEC_VP6_BASE | 0x0010,  /*!<Set video height*/ 

    VO_PID_ENC_VP6_BITRATE               = VO_PID_ENC_VP6_BASE | 0x0004,  /*!<Set bit rate, the parameter is LONG integer*/
    VO_PID_ENC_VP6_FRAMERATE             = VO_PID_ENC_VP6_BASE | 0x0005,  /*!<Set frame rate, the parameter is float*/
    VO_PID_ENC_VP6_KEY_FRAME_INTERVAL    = VO_PID_ENC_VP6_BASE | 0x0006,  /*!<Set key frame interval, the parameter is LONG integer*/
    VO_PID_ENC_VP6_VIDEO_QUALITY         = VO_PID_ENC_VP6_BASE | 0x0007,  /*!<Set encoder video quality, the parameter is VOVIDEOENCQUALITY*/	
    VO_PID_ENC_VP6_FORCE_KEY_FRAME       = VO_PID_ENC_VP6_BASE | 0x0008,  /*!<Force currenct frame coded as key frame, parameter is Boolean(LONG integer)*/
    VO_PID_ENC_VP6_OUTBUF_SIZE           = VO_PID_DEC_VP6_BASE | 0x0009,  /*!<Return the max out buffer size needed by encoder, the parameter is a point of LONG integer */
    VO_PID_ENC_VP6_VOL_HEADER            = VO_PID_ENC_VP6_BASE | 0x000a,  /*!<Return VOL header data, the parameter is point of VO_CODECBUFFER structure*/
    VO_PID_ENC_VP6_INPUT_ROTATION        = VO_PID_ENC_VP6_BASE | 0x000b,  /*!<Set input video rotation type*/
    VO_PID_ENC_VP6_VP_SIZE               = VO_PID_ENC_VP6_BASE | 0x000c,  /*!<Set video packet size*/		
    VO_PID_ENC_VP6_WIDTH                 = VO_PID_ENC_VP6_BASE | 0x000e,  /*!<Set width of video*/
    VO_PID_ENC_VP6_HEIGHT                = VO_PID_ENC_VP6_BASE | 0x0010,  /*!<Set height of video*/
    VO_PID_DEC_VP6_MAX				   = VO_MAX_ENUM_VALUE
};

/**
 * Get video decoder API interface
 * \param pDecHandle [IN/OUT] Return the VP6 Decoder API handle.
 * \param uFlag,reserved
 * \retval VO_ERR_OK Succeeded.
 */
 VO_S32 VO_API voGetVP6DecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag);

/**
 * Get video encoder API interface
 * \param pEncHandle  [IN/OUT] Return the VP6 Encoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetVP6EncAPI (VO_VIDEO_ENCAPI * pEncHandle);

#ifdef __cplusplus
}
#endif

#endif
