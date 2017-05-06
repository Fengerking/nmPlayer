	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/

#include "voIMediaSource.h"
#include "voCOMXMediaSource.h"

#define CHECK_POINT if (hComponent == NULL)\
	return OMX_ErrorInvalidComponent;\
OMX_COMPONENTTYPE * pCompType = (OMX_COMPONENTTYPE *)hComponent;\
if (pCompType->pComponentPrivate == NULL)\
	return OMX_ErrorInvalidComponent;\

OMX_ERRORTYPE APINewTrack (OMX_HANDLETYPE hComponent, OMX_S32 nIndex, VO_SOURCE_TRACKINFO *pTrackInfo)
{
	CHECK_POINT

	voCOMXMediaSource * pComp = (voCOMXMediaSource *)pCompType->pComponentPrivate;
	return pComp->NewTrack (nIndex, pTrackInfo);
}

OMX_ERRORTYPE APISendData (OMX_HANDLETYPE hComponent, OMX_S32 nIndex, VO_SOURCE_SAMPLE *pSample)
{
	CHECK_POINT

	voCOMXMediaSource * pComp = (voCOMXMediaSource *)pCompType->pComponentPrivate;
	return pComp->SendData (nIndex, pSample);
}
