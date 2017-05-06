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

#ifndef __VO_DIVX3_H__
#define __VO_DIVX3_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voVideo.h>

#define VO_ERR_DEC_DIVX3_BASE                 (VO_ERR_BASE | VO_INDEX_DEC_DIVX3)

enum{
    /*!<Severity1:bitstream errors that are resilient */
	/*!< Decoder*/
    VO_ERR_DEC_DIVX3_CODEC_ID             = VO_ERR_DEC_DIVX3_BASE | 0x0009,  /*!< Unsuppoted codec type*/
    VO_ERR_DEC_DIVX3_HEADER               = VO_ERR_DEC_DIVX3_BASE | 0x0001,  /*!< DIVX3 Decoder specific return code 1 */
    VO_ERR_DEC_DIVX3_I_FRAME              = VO_ERR_DEC_DIVX3_BASE | 0x0002,  /*!< DIVX3 Decoder specific return code 2 */
    VO_ERR_DEC_DIVX3_P_FRAME              = VO_ERR_DEC_DIVX3_BASE | 0x0003,  /*!< DIVX3 Decoder specific return code 3 */
    VO_ERR_DEC_DIVX3_B_FRAME              = VO_ERR_DEC_DIVX3_BASE | 0x0004,  /*!< DIVX3 Decoder specific return code 4 */
    VO_ERR_DEC_DIVX3_S_FRAME              = VO_ERR_DEC_DIVX3_BASE | 0x0005,  /*!< DIVX3 Decoder specific return code 5 */
    VO_ERR_DEC_DIVX3_INTRA_MB             = VO_ERR_DEC_DIVX3_BASE | 0x0006,  /*!< DIVX3 Decoder specific return code 6 */
    VO_ERR_DEC_DIVX3_INTER_MB             = VO_ERR_DEC_DIVX3_BASE | 0x0007,  /*!< DIVX3 Decoder specific return code 7 */
    VO_ERR_DEC_DIVX3_HW_PF                = VO_ERR_DEC_DIVX3_BASE | 0x0008,  /*!< DIVX3 Decoder specific return code 8 */

    VO_ERR_DEC_DIVX3_MAX	          = VO_MAX_ENUM_VALUE
};

#define VO_PID_DEC_DIVX3_BASE              (VO_PID_COMMON_BASE | VO_INDEX_DEC_DIVX3)
enum
{
	/*!< Decoder*/
    VO_PID_DEC_DIVX3_GET_VIDEOFORMAT       = VO_PID_DEC_DIVX3_BASE | 0x0002,  /*!<Get Video format*/
    VO_PID_DEC_DIVX3_GET_ERRNUM            = VO_PID_DEC_DIVX3_BASE | 0x000C,  /*!<Get error num of current frame*/
    VO_PID_DEC_DIVX3_GET_LASTVIDEOBUFFER   = VO_PID_DEC_DIVX3_BASE | 0x0005,  /*!<it will force the decoder to output the last frame video buffer immediately,the parameter is a Boolean(LONG integer)*/
    VO_PID_DEC_DIVX3_GET_LASTVIDEOINFO     = VO_PID_DEC_DIVX3_BASE | 0x0006,  /*!<it will force the decoder to output the last frame video info immediately,the parameter is a Boolean(LONG integer)*/         		
    VO_PID_DEC_DIVX3_VIDEO_WIDTH           = VO_PID_DEC_DIVX3_BASE | 0x000D,  /*!<Set video with*/    
    VO_PID_DEC_DIVX3_VIDEO_HEIGHT          = VO_PID_DEC_DIVX3_BASE | 0x000E,  /*!<Set video height*/   
	VO_PID_DEC_DIVX3_SET_THREAD_NUM        = VO_PID_DEC_DIVX3_BASE | 0x000F,  /*!<Set thread number*/

    VO_PID_DEC_DIVX3_MAX		    = VO_MAX_ENUM_VALUE
};
/**
 * Get video decoder API interface
 * \param pDecHandle [IN/OUT] Return the DIVX3 Decoder API handle.
 * \param uFlag,reserved
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetDIVX3DecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __VO_DIVX3_H__
