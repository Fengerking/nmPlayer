#include "filebase.h"
#include "utility.h"
#include "network.h"
#include "macro.h"
#include "RTSPSource.h"
#include "RTSPTrack.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
/**
Create the Session
/param [IN/OUT] ppRTSPSession: the pointer to a session handle
*/
VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionCreate(HVOFILEREAD * ppRTSPSession)
{
	CRTSPSourceEX * pRTSPSource = new CRTSPSourceEX();TRACE_NEW("rtss_pRTSPSource",pRTSPSource);
	if(pRTSPSource == NULL)
		return VORC_FILEREAD_NULL_POINTER;
	*ppRTSPSession = pRTSPSource;
	return VORC_FILEREAD_OK;
}
/**
Open the URL
/param [IN] pRTSPSession: the  session handle
/param [IN] pRTSPLink: rtstp/mms URL
/param [IN] pFileOp: reservered,currently can be set to NULL
*/
VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionOpenURL(HVOFILEREAD pRTSPSession, const char * pRTSPLink, VOFILEOP * pFileOp)
{
	if(pRTSPSession==NULL)
		return VORC_FILEREAD_NULL_POINTER;
	CRTSPSourceEX * pRTSPSource = (CRTSPSourceEX *)pRTSPSession;
	if(pRTSPSource->Open(pRTSPLink))
	{
		return VORC_FILEREAD_OK;
	}
	else
	{
		return VORC_FILEREAD_OPEN_FAILED;
	}
}
VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionOpen(HVOFILEREAD * ppRTSPSession, const char * pRTSPLink, VOFILEOP * pFileOp)
{
	CRTSPSourceEX * pRTSPSource = new CRTSPSourceEX();TRACE_NEW("rtss_pRTSPSource2",pRTSPSource);
	if(pRTSPSource == NULL)
		return VORC_FILEREAD_NULL_POINTER;
	if(pRTSPSource->Open(pRTSPLink))
	{
		*ppRTSPSession = pRTSPSource;
		return VORC_FILEREAD_OK;
	}
	else
	{
		SAFE_DELETE(pRTSPSource);
		return VORC_FILEREAD_OPEN_FAILED;
	}
}
/**
Close the session
/param [IN] pRTSPSession: the  session handle
*/
VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionClose(HVOFILEREAD pRTSPSession)
{
	if(pRTSPSession == NULL)
		return VORC_FILEREAD_NULL_POINTER;

    CRTSPSourceEX * pRTSPSource = (CRTSPSourceEX *)pRTSPSession;
	pRTSPSource->SetParameter(VOID_STREAMING_FORCE_STOP,1);
    SAFE_DELETE(pRTSPSource);
	return VORC_FILEREAD_OK;
}

VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionSetParameter(HVOFILEREAD pRTSPSession, LONG lID, LONG lValue) 
{
	if(pRTSPSession == NULL)
		return VORC_FILEREAD_NULL_POINTER;

    CRTSPSourceEX * pRTSPSource = (CRTSPSourceEX *)pRTSPSession;
	return pRTSPSource->SetParameter(lID, lValue);
}

VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionGetParameter(HVOFILEREAD pRTSPSession, LONG lID, LONG * plValue) 
{
	if(pRTSPSession == NULL)
		return VORC_FILEREAD_NULL_POINTER;

    CRTSPSourceEX * pRTSPSource = (CRTSPSourceEX *)pRTSPSession;
	return pRTSPSource->GetParameter(lID, plValue);
}

VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackOpen(HVOFILETRACK * ppRTSPTrack, HVOFILEREAD pRTSPSession, int nIndex) 
{
	if(pRTSPSession == NULL)
		return VORC_FILEREAD_NULL_POINTER;

    CRTSPSourceEX * pRTSPSource = (CRTSPSourceEX *)pRTSPSession;
	return pRTSPSource->OpenRTSPTrack(nIndex, (CRTSPTrack **)ppRTSPTrack);
}

VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackClose(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack) 
{
	if(pRTSPSession == NULL)
		return VORC_FILEREAD_NULL_POINTER;

    CRTSPSourceEX * pRTSPSource = (CRTSPSourceEX *)pRTSPSession;
	return pRTSPSource->CloseRTSPTrack((CRTSPTrack *)pRTSPTrack);
}

VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackSetParameter(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, LONG lID, LONG lValue) 
{
	if(pRTSPSession == NULL || pRTSPTrack == NULL)
		return VORC_FILEREAD_NULL_POINTER;
	return ((CRTSPTrack *)pRTSPTrack)->SetParameter(lID, lValue);
}

VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackGetParameter(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, LONG lID, LONG * plValue) 
{
	if(pRTSPSession == NULL || pRTSPTrack == NULL)
		return VORC_FILEREAD_NULL_POINTER;

	return ((CRTSPTrack *)pRTSPTrack)->GetParameter(lID, plValue);
}

VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackInfo(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, VOTRACKINFO * pTrackInfo) 
{
	if(pRTSPSession == NULL || pRTSPTrack == NULL || pTrackInfo == NULL)
		return VORC_FILEREAD_NULL_POINTER;

	return ((CRTSPTrack *)pRTSPTrack)->GetInfo(pTrackInfo);
}



VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackGetSampleByIndex(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, VOSAMPLEINFO * pSampleInfo) 
{
	if(pRTSPSession == NULL || pRTSPTrack == NULL || pSampleInfo == NULL)
		return VORC_FILEREAD_NULL_POINTER;

	CRTSPSourceEX * pRTSPSource = (CRTSPSourceEX *)pRTSPSession;
	VOFILEREADRETURNCODE ret;
	ret= pRTSPSource->GetRTSPTrackSampleByIndex((CRTSPTrack *)pRTSPTrack, pSampleInfo);
	return ret;
}


int VOFILEAPI voRTSPTrackGetNextKeyFrame(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, int nIndex, int nDirectionFlag)
{
	if(pRTSPSession == NULL || pRTSPTrack == NULL)
		return VORC_FILEREAD_NULL_POINTER;

	return ((CRTSPTrack *)pRTSPTrack)->GetNextKeyFrame(nIndex, nDirectionFlag);
}

#ifdef _VONAMESPACE
}
#endif