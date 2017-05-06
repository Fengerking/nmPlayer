	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/

#ifndef __voOMX_IMediaSource_H__
#define __voOMX_IMediaSource_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "OMX_Types.h"
#include "OMX_Core.h"
#include "OMX_Component.h"
#include "voSource.h"

/**
 * VisualOn Media Source interface
 */
typedef struct
{
	OMX_ERRORTYPE (OMX_APIENTRY * NewTrack) (OMX_HANDLETYPE hComponent, OMX_S32 nIndex, VO_SOURCE_TRACKINFO *pTrackInfo);
	OMX_ERRORTYPE (OMX_APIENTRY * SendData) (OMX_HANDLETYPE hComponent, OMX_S32 nIndex, VO_SOURCE_SAMPLE *pSample);

}VOOMX_IMEDIASOURCE;


#ifdef __cplusplus
}
#endif // __cplusplus

#endif //__voOMX_IMediaSource_H__