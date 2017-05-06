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
#ifndef __voVideoRender_H__
#define __voVideoRender_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voVideo.h"
#include "voMem.h"
#include "viMem.h"

//!< Get, the display rect VO_RECT *	) >
#define VO_VR_PMID_DrawRect			(VO_INDEX_SNK_VIDEO | 0x0001)
#define VO_VR_PMID_RENDER_CONTEXT	(VO_INDEX_SNK_VIDEO | 0x0002)
#define VO_VR_PMID_CAPTURE_VIDEO_IMAGE	(VO_INDEX_SNK_VIDEO | 0X0003)  /*!< Get, capture current video image, refer to VO_IMAGE_DATA*. */

/**
 * Event ID of VOVIDEO_RENDER_EVENT
 */
#define VO_VR_EVENT_VIDEO_DISPLAY_RECT_CHANGED     0X02000001 /*!<Video display rect be change, param1 will be VO_RECT*. */

/**
 * Video Render event CallBack function.
 * \param eventID [IN] The event id.
 * \param param1  [IN] The video buffer info.
 * \param param2  [IN] The video size.
 *
 * \retval VO_ERR_NONE Succeeded.
 */
typedef VO_S32 (VO_API * VOVIDEO_EVENT_CALLBACKPROC) (VO_PTR pUserData, int eventID, void * param1, void * param2);

typedef struct
{
	VO_U32 (VO_API * Init) (VO_HANDLE * phRender, VO_PTR hView, VO_MEM_OPERATOR * pMemOP, VO_U32 nFlag);
	VO_U32 (VO_API * Uninit) (VO_HANDLE hRender);
	VO_U32 (VO_API * SetVideoInfo) (VO_HANDLE hRender, VO_U32 nWidth, VO_U32 nHeight, VO_IV_COLORTYPE nColor);
	VO_U32 (VO_API * SetDispRect) (VO_HANDLE hRender, VO_PTR hView, VO_RECT * pDispRect);
	VO_U32 (VO_API * SetDispType) (VO_HANDLE hRender, VO_IV_ZOOM_MODE nZoomMode, VO_IV_ASPECT_RATIO nRatio);
	VO_U32 (VO_API * Start) (VO_HANDLE hRender);
	VO_U32 (VO_API * Pause) (VO_HANDLE hRender);
	VO_U32 (VO_API * Stop) (VO_HANDLE hRender);
	VO_U32 (VO_API * Render) (VO_HANDLE hRender, VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait);
	VO_U32 (VO_API * WaitDone) (VO_HANDLE hRender);
	VO_U32 (VO_API * Redraw) (VO_HANDLE hRender);
	VO_U32 (VO_API * SetCallBack) (VO_HANDLE hRender, VOVIDEOCALLBACKPROC pCallBack, VO_PTR pUserData);
	VO_U32 (VO_API * GetVideoMemOP) (VO_HANDLE hRender, VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP);
	VO_U32 (VO_API * SetParam) (VO_HANDLE hRender, VO_U32 nID, VO_PTR pValue);
	VO_U32 (VO_API * GetParam) (VO_HANDLE hRender, VO_U32 nID, VO_PTR pValue);
} VO_VIDEO_RENDERAPI;

/**
 * Get video render API interface
 * \param pRender [IN/OUT] Return the video render API handle.
 * \param uFlag,reserved
 * \retval VO_ERR_None Succeeded.
 */
VO_S32 VO_API voGetVideoRenderAPI (VO_VIDEO_RENDERAPI * pRender, VO_U32 uFlag);


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // __voVideoRender_H__

