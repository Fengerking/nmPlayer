/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
#ifndef __VO_AMEDIACODEC__H
#define __VO_AMEDIACODEC__H

#include "voVideo.h"
#include "voAMediaCodecCommon.h"

const int VO_ACODEC_COLOR_TYPE    = (VO_COLOR_TYPE_MAX -2013);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get video decoder API interface
 * \param pDecHandle [IN/OUT] Return the video Decoder API handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetAMediaCodecAPI (VO_VIDEO_DECAPI * pDecHandle);


#ifdef __cplusplus
}
#endif	
#endif
