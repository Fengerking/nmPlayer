	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/

#ifndef __voIMediaSource_H__
#define __voIMediaSource_H__

#include "voOMX_IMediaSource.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


OMX_ERRORTYPE APINewTrack (OMX_HANDLETYPE hComponent, OMX_S32 nIndex, VO_SOURCE_TRACKINFO *pTrackInfo);
OMX_ERRORTYPE APISendData (OMX_HANDLETYPE hComponent, OMX_S32 nIndex, VO_SOURCE_SAMPLE *pSample);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__voIMediaSource_H__