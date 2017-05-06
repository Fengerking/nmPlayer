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

#ifndef __VO_MPEG4_H__
#define __VO_MPEG4_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voVideo.h>

#define VO_ERR_DEC_MPEG4_BASE                 (VO_ERR_BASE | VO_INDEX_DEC_MPEG4) /*!< MPEG4 decoder error code base*/
#define VO_ERR_ENC_MPEG4_BASE                 (VO_ERR_BASE | VO_INDEX_ENC_MPEG4) /*!< MPEG4 encoder error code base*/	

/**
 * MPEG4 Decoder&Encoder specific return code 
 * \see common return code defined in voIndex.h
 */
enum{
	/*!< Decoder*/
    VO_ERR_DEC_MPEG4_CODEC_ID             = VO_ERR_DEC_MPEG4_BASE | 0x0009,  /*!< Unsuppoted codec type*/
    VO_ERR_DEC_MPEG4_HEADER               = VO_ERR_DEC_MPEG4_BASE | 0x0001,  /*!< Decode header data error */
    VO_ERR_DEC_MPEG4_I_FRAME              = VO_ERR_DEC_MPEG4_BASE | 0x0002,  /*!< Decode I nFrame error */
    VO_ERR_DEC_MPEG4_P_FRAME              = VO_ERR_DEC_MPEG4_BASE | 0x0003,  /*!< Decode P nFrame error  */
    VO_ERR_DEC_MPEG4_B_FRAME              = VO_ERR_DEC_MPEG4_BASE | 0x0004,  /*!< Decode B nFrame error  */
    VO_ERR_DEC_MPEG4_S_FRAME              = VO_ERR_DEC_MPEG4_BASE | 0x0005,  /*!< Decode S nFrame error  */
    VO_ERR_DEC_MPEG4_INTRA_MB             = VO_ERR_DEC_MPEG4_BASE | 0x0006,  /*!< Decode Intra MB error  */
    VO_ERR_DEC_MPEG4_INTER_MB             = VO_ERR_DEC_MPEG4_BASE | 0x0007,  /*!< Decode Inter MB error */
    VO_ERR_DEC_MPEG4_HW_PF                = VO_ERR_DEC_MPEG4_BASE | 0x0008,  /*!< Error of hardware post filter */

	/*!< Encoder*/
    VO_ERR_ENC_MPEG4_CONFIG               = VO_ERR_ENC_MPEG4_BASE | 0x0001,  /*!< Encoder error of configuration */
    VO_ERR_ENC_MPEG4_VIDEOTYPE            = VO_ERR_ENC_MPEG4_BASE | 0x0002,  /*!< Encoder unsupport video type */
    VO_ERR_ENC_MPEG4_FRAME                = VO_ERR_ENC_MPEG4_BASE | 0x0003,  /*!< Error in nFrame */

	VO_ERR_DEC_MPEG4_MAX				  = VO_MAX_ENUM_VALUE                /*!< Max value of current enum */
};

#define VO_PID_DEC_MPEG4_BASE              (VO_PID_COMMON_BASE | VO_INDEX_DEC_MPEG4) /*!< MPEG4 decoder parameter ID base*/
#define VO_PID_ENC_MPEG4_BASE              (VO_PID_COMMON_BASE | VO_INDEX_ENC_MPEG4) /*!< MPEG4 encoder parameter ID base*/

/**
 * MPEG4 Decoder&Encoder specific parameter ID
 * \see common parameter defined in voIndex.h
*/
enum 
{
	/*!< Decoder*/
    VO_PID_DEC_MPEG4_GET_VIDEOFORMAT       = VO_PID_DEC_MPEG4_BASE | 0x0002,  /*!<Get Video format, the parameter is address of structure VO_VIDEO_OUTPUTINFO*/
    VO_PID_DEC_MPEG4_MB_SKIP               = VO_PID_DEC_MPEG4_BASE | 0x0004,  /*!<Get mb skip information, the parameter is address of VO_U8 point, (Invalid)*/
    VO_PID_DEC_MPEG4_GET_ERRNUM            = VO_PID_DEC_MPEG4_BASE | 0x000C,  /*!<Get error num of current nFrame, the parameter is address of VO_U32, (Invalid)*/
    VO_PID_DEC_MPEG4_GET_LASTVIDEOBUFFER   = VO_PID_DEC_MPEG4_BASE | 0x0005,  /*!<Get the last nFrame video buffer,the parameter is address of structure VO_VIDEO_BUFFER,(invalid)*/
    VO_PID_DEC_MPEG4_GET_LASTVIDEOINFO     = VO_PID_DEC_MPEG4_BASE | 0x0006,  /*!<Get the last nFrame video information,the parameter is address of structure VO_VIDEO_OUTPUTINFO,(invalid)*/
    VO_PID_DEC_MPEG4_PF                    = VO_PID_DEC_MPEG4_BASE | 0x0007,  /*!<Post filer switch, the parameter is address of VO_BOOL, 0-disable,1-software,2-hdware,(invalid)*/
    VO_PID_DEC_MPEG4_PF_MODE               = VO_PID_DEC_MPEG4_BASE | 0x0008,  /*!<Set post filter mode, (invalid)*/
    VO_PID_DEC_MPEG4_MEASURE_PERFORMANCE   = VO_PID_DEC_MPEG4_BASE | 0x0009,  /*!<Performance switch, the parameter is address of VO_U32, 0-disable, 1-microsecond, 2-cpu cycles*, (invalid)r*/
    VO_PID_DEC_MPEG4_GET_PERFORMANCE_INFO  = VO_PID_DEC_MPEG4_BASE | 0x000A,  /*!<Get performance information, (invalid)*/
    VO_PID_DEC_MPEG4_SET_CURRENTFRAME_GREY = VO_PID_DEC_MPEG4_BASE | 0x000B,  /*!<Reset current decoded error nFrame, the paramter is address of VO_U32, 1-grey, 2-the latest non-corrupted nFrame,(invalid)*/ 
    VO_PID_DEC_MPEG4_SET_QUERYMEM          = VO_PID_DEC_MPEG4_BASE | 0x000C,  /*!<Set query memory structure, the parameter is address of query memory structure, (invalid)*/           		
    VO_PID_DEC_MPEG4_GET_VIDEO_WIDTH       = VO_PID_DEC_MPEG4_BASE | 0x000D,  /*!<Get video with, the parameter is address of VO_U32,(invalid)*/    
    VO_PID_DEC_MPEG4_GET_VIDEO_HEIGHT      = VO_PID_DEC_MPEG4_BASE | 0x000E,  /*!<Get video nHeight, the parameter is address of VO_U32,(invalid)*/   
	VO_PID_DEC_MPEG4_SET_THREAD_NUM        = VO_PID_DEC_MPEG4_BASE | 0x000F,  /*!<Set thread number, the parameter is address of the thread number,*/

	/*!< Encoder*/
    VO_PID_ENC_MPEG4_BITRATE               = VO_PID_ENC_MPEG4_BASE | 0x0004,  /*!<Set bit rate, the parameter is address of VO_S32*/
    VO_PID_ENC_MPEG4_FRAMERATE             = VO_PID_ENC_MPEG4_BASE | 0x0005,  /*!<Set nFrame rate, the parameter is address of float*/
    VO_PID_ENC_MPEG4_KEY_FRAME_INTERVAL    = VO_PID_ENC_MPEG4_BASE | 0x0006,  /*!<Set key nFrame interval, the parameter is address of VO_S32*/
    VO_PID_ENC_MPEG4_VIDEO_QUALITY         = VO_PID_ENC_MPEG4_BASE | 0x0007,  /*!<Set encoder video quality, the parameter is address of VOVIDEOENCQUALITY*/	
    VO_PID_ENC_MPEG4_FORCE_KEY_FRAME       = VO_PID_ENC_MPEG4_BASE | 0x0008,  /*!<Force currenct nFrame coded as key nFrame, parameter is address of VO_S32, 0-disable, 1-enable*/
    VO_PID_ENC_MPEG4_OUTBUF_SIZE           = VO_PID_DEC_MPEG4_BASE | 0x0009,  /*!<Return the max out buffer size needed by encoder, the parameter is address of VO_S32 */
    VO_PID_ENC_MPEG4_VOL_HEADER            = VO_PID_ENC_MPEG4_BASE | 0x000a,  /*!<Return VOL header data, the parameter is address of VO_VIDEO_BUFFER structure*/
    VO_PID_ENC_MPEG4_INPUT_ROTATION        = VO_PID_ENC_MPEG4_BASE | 0x000b,  /*!<Set input video rotation type, (invalid)*/
    VO_PID_ENC_MPEG4_VP_SIZE               = VO_PID_ENC_MPEG4_BASE | 0x000c,  /*!<Set video packet size, the paramter is address of VO_S32*/		
    VO_PID_ENC_MPEG4_WIDTH                 = VO_PID_ENC_MPEG4_BASE | 0x000e,  /*!<Set nWidth of video, the paramter is address of VO_S32*/
	VO_PID_ENC_MPEG4_HEIGHT                = VO_PID_ENC_MPEG4_BASE | 0x0010,  /*!<Set nHeight of video, the paramter is address of VO_S32*/
	VO_PID_DEC_MPEG4_MAX				   = VO_MAX_ENUM_VALUE                /*!< Max value of current enum */
};

/*!
 * Get video decoder API interface
 * \param pDecHandle [IN/OUT] Return the MPEG4 Decoder API handle.
 * \param uFlag,reserved
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API yyGetMPEG4DecFunc (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag);

/*!
 * Get video encoder API interface
 * \param pEncHandle  [IN/OUT] Return the MPEG4 Encoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetMPEG4EncAPI (VO_VIDEO_ENCAPI * pEncHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __VO_MPEG4_H__
