/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2007				*
*																		*
************************************************************************
*******************************************************************************
File:		voPDSDK.h

Contains:	PD filter sdk.h

Written by:	Number Huang

Change History (most recent first):
2007-07-25		JBF			Create file

*******************************************************************************/
#ifndef __VOPDSDK_H__
#define __VOPDSDK_H__

/** 
 * \file voPDSDK2.h
 * \brief VisualOn PD Engine SDK
 * \version 0.1
 * \date 10/27/2006 created
 * \author VisualOn
 */
#include "filebase.h"


// PD Source Function
typedef long (VOFILEAPI * VOFILEOPEN)				(HVOFILEREAD *ppPDSource, const char *pSourceFile, VOFILEOP *pFileOp);
typedef long (VOFILEAPI * VOFILECLOSE)				(HVOFILEREAD pPDSource);
typedef long (VOFILEAPI * VOFILESETPARAMETER)		(HVOFILEREAD pPDSource, LONG nID, LONG lValue);
typedef long (VOFILEAPI * VOFILEGETPARAMETER)		(HVOFILEREAD pPDSource, LONG nID, LONG *plValue);
typedef long (VOFILEAPI * VOFILEFP2MT)				(HVOFILEREAD pPDSource, int nFilePos, int* pnMediaTime, bool bStart);
typedef long (VOFILEAPI * VOFILEMT2FP)				(HVOFILEREAD pPDSource, int nMediaTime, int* pnFilePos, bool bStart);

// PD Track Function
typedef long (VOFILEAPI * VOTRACKOPEN)				(HVOFILETRACK * ppPDTrack, HVOFILEREAD pPDSource, int nIndex);
typedef long (VOFILEAPI * VOTRACKCLOSE)			(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack);
typedef long (VOFILEAPI * VOTRACKSETPARAMETER)		(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, LONG nID, LONG lValue);
typedef long (VOFILEAPI * VOTRACKGETPARAMETER)		(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, LONG nID, LONG *plValue);
typedef long (VOFILEAPI * VOTRACKGETINFO)			(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, VOTRACKINFO *pTrackInfo);
typedef long (VOFILEAPI * VOTRACKGETSAMPLEBYINDEX) (HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, VOSAMPLEINFO *pSampleInfo);
typedef long (VOFILEAPI * VOTRACKGETSAMPLEBYTIME)	(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, VOSAMPLEINFO *pSampleInfo);
typedef long (VOFILEAPI * VOTRACKGETNEXTKEYFRAME)	(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, int nIndex, int nDirectionFlag);
typedef long (VOFILEAPI * VOTRACKGETFPBYINDEX)		(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, int nIndex);
typedef struct  
{
	VOFILEOPEN												open;
	VOFILECLOSE												close;
	VOFILESETPARAMETER							setParam;
	VOFILEGETPARAMETER							getParam;
	VOFILEFP2MT												pos2time;
	VOFILEMT2FP												time2pos;																				
	VOTRACKOPEN											openTrack;
	VOTRACKCLOSE										closeTrack;
	VOTRACKSETPARAMETER						setTrackParam;
	 VOTRACKGETPARAMETER						getTrackParam;
	VOTRACKGETINFO									getTrackInfo;
	VOTRACKGETSAMPLEBYINDEX				getSampleByIndex;
	VOTRACKGETSAMPLEBYTIME				getSampleByTime;
	VOTRACKGETNEXTKEYFRAME				getNextKeyFrame;
	VOTRACKGETFPBYINDEX						getPosByIndex;


}TFileReaderAPI;
/*!
Create the Session
\param [IN/OUT] ppPDSession: the pointer to a session handle
\return VORC_OK if SUCCEEDED
*/
VOFILEREADRETURNCODE VOFILEAPI voPDSessionCreate(HVOFILEREAD * ppPDSession,void* userData);
/*!
Open the URL
\param [IN] pPDSession: the  session handle
\param [IN] pPDLink: rtstp/mms URL
\param [IN] pFileReader: the reader API for the URL type,it may be mp4/3gp reader,audio reader,real reader,asf reader,flv reader etc.
\return VORC_OK if SUCCEEDED
*/
VOFILEREADRETURNCODE VOFILEAPI voPDSessionOpenURL(HVOFILEREAD pPDSession, TCHAR * pPDLink, TFileReaderAPI* pFileReader);
/*!
Close the session
\param [IN] pPDSession: the  session handle
\return VORC_OK if SUCCEEDED
*/
VOFILEREADRETURNCODE VOFILEAPI voPDSessionClose(HVOFILEREAD pPDSession);
/*!
Set Parameter of  the session
\param [IN] pPDSession: the  session handle
\param [IN] IID:param ID
\param [IN] lValue
\return VORC_OK if SUCCEEDED
*/
VOFILEREADRETURNCODE VOFILEAPI voPDSessionSetParameter(HVOFILEREAD pPDSession, LONG lID, LONG lValue) ;
/*!
Get Parameter of  the session
\param [IN] pPDSession: the  session handle
\param [IN] IID:param ID
\param [OUT]  plValue
\return VORC_OK if SUCCEEDED
*/
VOFILEREADRETURNCODE VOFILEAPI voPDSessionGetParameter(HVOFILEREAD pPDSession, LONG lID, LONG * plValue) ;
/*!
Create one track
\param [IN/OUT] ppPDTrack: the pointer to a track handle
\param [IN]  pPDSession:the  session handle
\param [IN] index: the index of the track
\return VORC_OK if SUCCEEDED
*/
VOFILEREADRETURNCODE VOFILEAPI voPDTrackOpen(HVOFILETRACK * ppPDTrack, HVOFILEREAD pPDSession, int nIndex) ;
/*!
Close one track
\param [IN]  pPDTrack: the  track handle
\param [IN]  pPDSession:the  session handle
\param [IN] index: the index of the track
\return VORC_OK if SUCCEEDED
*/
VOFILEREADRETURNCODE VOFILEAPI voPDTrackClose(HVOFILEREAD pPDSession, HVOFILETRACK pPDTrack) ;
/*!
Set Parameter of  the track
\\param [IN]  pPDTrack: the  track handle
\param [IN]  pPDSession:the  session handle
\param [IN] IID:param ID
\param [IN]  lValue
\return VORC_OK if SUCCEEDED
*/
VOFILEREADRETURNCODE VOFILEAPI voPDTrackSetParameter(HVOFILEREAD pPDSession, HVOFILETRACK pPDTrack, LONG lID, LONG lValue); 
/*!
Get Parameter of  the track
\param [IN]  pPDTrack: the  track handle
\param [IN]  pPDSession:the  session handle
\param [IN] IID:param ID
\param [OUT]  plValue
\return VORC_OK if SUCCEEDED
*/
VOFILEREADRETURNCODE VOFILEAPI voPDTrackGetParameter(HVOFILEREAD pPDSession, HVOFILETRACK pPDTrack, LONG lID, LONG * plValue);
/*!
Get the meta info of  the track
\param [IN]  pPDTrack: the  track handle
\param [IN]  pPDSession:the  session handle
\param [OUT] VOTRACKINFO
\return VORC_OK if SUCCEEDED
*/
VOFILEREADRETURNCODE VOFILEAPI voPDTrackInfo(HVOFILEREAD pPDSession, HVOFILETRACK pPDTrack, VOTRACKINFO * pTrackInfo);
/*!
Get the media sample of  the track 
\param [IN]  pPDTrack: the  track handle
\param [IN]  pPDSession:the  session handle
\param [OUT] VOSAMPLEINFO:
\return VORC_OK if SUCCEEDED
*/
VOFILEREADRETURNCODE VOFILEAPI voPDTrackGetSampleByIndex(HVOFILEREAD pPDSession, HVOFILETRACK pPDTrack, VOSAMPLEINFO * pSampleInfo);
/*!
Get the key media sample (Intra video frame) of  the track 
\param [IN]  pPDTrack: the  track handle
\param [IN]  pPDSession:the  session handle
\param [IN]  nIndex:reserved,currently==0
\param [IN]  nDirectionFlag:reserved,currently==0
\return the index of the key sample
*/
int VOFILEAPI voPDTrackGetNextKeyFrame(HVOFILEREAD pPDSession, HVOFILETRACK pPDTrack, int nIndex, int nDirectionFlag);

int VOFILEAPI  voPDTrackGetFPByIndex(HVOFILEREAD pPDSession, HVOFILETRACK pPDTrack, int nIndex);
int VOFILEAPI	voPDTrackGetSampleByTime(HVOFILEREAD pPDSource, HVOFILETRACK pPDTrack, VOSAMPLEINFO *pSampleInfo);

#endif//__VOPDSDK_H__;