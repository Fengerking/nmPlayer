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
#pragma once

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voIndex.h"
#include "voMem.h"
#include "voString.h"
#include "voIVCommon.h"
#include "voAudio.h"
#include "voVideo.h"
#include "voFile.h"
#include "voStreaming.h"

typedef struct
{
	VO_U32 (VO_API * Init) (VO_HANDLE * phGetThumbnail);
	VO_U32 (VO_API * Uninit) (VO_HANDLE hGetThumbnail);
	VO_U32 (VO_API * GetThumbnail) (VO_HANDLE hGetThumbnail, VO_PTR pSource, VO_U32 nWidth, VO_U32 nHeight, VO_PTR pBitmapHandle);
} VO_THUMBNAILAPI;
/**
 * Get Thumbnail Play API interface
 * \param hGetThumbnail [IN/OUT] Return the API handle.
 * \retval VO_ERR_None Succeeded.
 */
VO_S32 VO_API voGetThumbnailAPI (VO_THUMBNAILAPI * pGetThumbnail);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
