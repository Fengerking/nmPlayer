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

#ifndef __VO_VC1_H__
#define __VO_VC1_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voVideo.h>
#include <viMem.h>

//#define DEG 1

#define VO_PID_DEC_VC1_BASE              (VO_PID_COMMON_BASE | VO_INDEX_DEC_VC1) 

#define VO_PID_DEC_VC1_GET_LASTVIDEOBUFFER   VO_PID_DEC_VC1_BASE | 0x0001 /*!<Get the last nFrame video buffer,the parameter is address of structure VO_VIDEO_BUFFER,(invalid)*/
#define VO_PID_DEC_VC1_GET_LASTVIDEOINFO       VO_PID_DEC_VC1_BASE | 0x0002/*!<Get the last nFrame video information,the parameter is address of structure VO_VIDEO_OUTPUTINFO,(invalid)*/
#define VO_PID_DEC_VC1_SET_LASTVIDEOFLAG        VO_PID_DEC_VC1_BASE | 0x0003/*!<Get the status of threads ,the parameter is address of structure VO_VIDEO_OUTPUTINFO,(invalid)*/
/**
 * Get video decoder API interface
 * \param pDecHandle [IN/OUT] Return the WMV9 Decoder API handle.
 * \param uFlag,reserved
 * \retval VO_ERR_NONE Succeeded.
 */
VO_S32 VO_API voGetVC1DecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag);

/**
 * Get video encoder API interface
 * \param pEncHandle  [IN/OUT] Return the WMV9 Encoder handle.
 * \retval VO_ERR_NONE Succeeded.
 */
VO_S32 VO_API voGetVC1EncAPI (VO_VIDEO_ENCAPI * pEncHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __VO_VC1_H__
