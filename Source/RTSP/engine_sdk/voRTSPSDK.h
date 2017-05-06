/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2007				*
*																		*
************************************************************************/
/*******************************************************************************
File:		voRTSPSDK.h

Contains:	rtsp filter sdk.h

Written by:	Number Huang

Change History (most recent first):
2007-07-25		JBF			Create file

*******************************************************************************/
#ifndef __VORTSPSDK_H__
#define __VORTSPSDK_H__

#include "filebase.h"
#include "voStreaming.h"
#include "voRTSPCrossPlatform.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionCreate(HVOFILEREAD * ppRTSPSession);
VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionOpenURL(HVOFILEREAD pRTSPSession, const char * pRTSPLink, VOFILEOP * pFileOp);
VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionOpen(HVOFILEREAD * ppRTSPSession, const char * pRTSPLink, VOFILEOP * pFileOp);
VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionClose(HVOFILEREAD pRTSPSession);
VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionSetParameter(HVOFILEREAD pRTSPSession, LONG lID, LONG lValue) ;
VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionGetParameter(HVOFILEREAD pRTSPSession, LONG lID, LONG * plValue) ;
VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackOpen(HVOFILETRACK * ppRTSPTrack, HVOFILEREAD pRTSPSession, int nIndex) ;
VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackClose(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack) ;
VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackSetParameter(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, LONG lID, LONG lValue); 
VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackGetParameter(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, LONG lID, LONG * plValue);
VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackInfo(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, VOTRACKINFO * pTrackInfo);
VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackGetSampleByIndex(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, VOSAMPLEINFO * pSampleInfo);
int VOFILEAPI voRTSPTrackGetNextKeyFrame(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, int nIndex, int nDirectionFlag);

#ifdef _VONAMESPACE
}
#endif

#endif//__VORTSPSDK_H__;