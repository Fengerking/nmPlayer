
#include "EventHandler.h"
#include "voLog.h"
#include "voOSFunc.h"
#include "voToolUtility.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

inline const char * EventHandlerFlagCheck(VO_U32 nID ) 
{ 
	switch(nID) 
	{ 
	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_WARNING: 
		return "VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_WARNING";
	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_INFO: 
		return "VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_INFO";
	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_ERROR: 
		return "VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_ERROR";
	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CUSTOMTAG: 
		return "VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CUSTOMTAG";
	} 
	return "";
}

CEventHandler::CEventHandler(VO_SOURCE2_EVENTCALLBACK * pEventCB, VO_ADAPTIVESTREAMPARSER_STREAMTYPE nType, VO_PTR pReserved)
:m_pEventCB(pEventCB)
,m_nStreamType( nType)
,m_bStop(VO_FALSE)
,m_sProgramType(VO_SOURCE2_STREAM_TYPE_VOD)
,m_bSusPend(VO_FALSE)
,m_nErrorNum(0)
,m_nTolerantErrorNum(0)
{

}

CEventHandler::~CEventHandler()
{

}

VO_U32 CEventHandler::CheckEvent(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2, VO_PTR pReserved)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

#if 0
	VODS_VOLOGI("+ EventHandler SendEvent: %s, nParam1:%d, nParam2:%p", EventHandlerFlagCheck( nID ), nParam1, nParam2 );
	ret = m_pEventCB->SendEvent(m_pEventCB->pUserData, nID, nParam1, nParam2);
	VODS_VOLOGI("- EventHandler SendEvent: %s. Ret:0x%08x", EventHandlerFlagCheck( nID ), ret );

#else
	VO_U32 nIntervalTime = 0;
	//do not check stream type cause if open failed, errer event will be blocked here
	/*
	if(m_nStreamType != VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS
		&& m_nStreamType != VO_ADAPTIVESTREAMPARSER_STREAMTYPE_ISS
		&& m_nStreamType != VO_ADAPTIVESTREAMPARSER_STREAMTYPE_DASH)
		return ret;
	*/
	switch( nID )
	{
	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_WARNING:
		m_pEventCB->SendEvent(m_pEventCB->pUserData, nID, nParam1, nParam2);
		ret = CheckWarningEvent(nParam1, nParam2, &nIntervalTime);
		break;

	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_INFO:
		ret =  CheckInfoEvent( nParam1, nParam2, &nIntervalTime);
		break;
	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_ERROR:
		m_nErrorNum = m_nTolerantErrorNum +1; //If error occur,make it pop error to uplay directly
		ret =  CheckErrorEvent( nParam1, nParam2, &nIntervalTime);
		break;
	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CUSTOMTAG:
		ret =  CheckCustomEvent( nParam1, nParam2, &nIntervalTime);
		break;
	}

	ret = CheckRetry(ret, nIntervalTime);
#endif
	return ret;
}


VO_U32 CEventHandler::GetParameter( VO_U32 uID, VO_PTR pParam )
{
	return VO_RET_SOURCE2_NOIMPLEMENT;
}

VO_U32 CEventHandler::SetParameter( VO_U32 uID, VO_PTR pParam )
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	switch(uID)
	{
	case VO_PID_ADAPTIVESTREAMING_STREAMTYPE:
		m_nStreamType = *((VO_ADAPTIVESTREAMPARSER_STREAMTYPE *) pParam);
		break;
	case VO_PID_SOURCE2_PROGRAM_TYPE:
		m_sProgramType = *((VO_SOURCE2_PROGRAM_TYPE *)pParam);
		m_CommonError.SetParameter( uID, pParam);
		break;
	case VO_PID_SOURCE2_APPLICATION_SUSPEND:
		m_bSusPend = VO_TRUE;
		break;
	case VO_PID_SOURCE2_APPLICATION_RESUME:
		m_CommonError.Reset();
		m_bSusPend = VO_FALSE;
		break;
	case VO_PID_SOURCE2_TOLERANT_ERROR_NUMBER:
		//suppose it will be set before open
		m_nTolerantErrorNum = *((VO_U32*)pParam);
		VOLOGI("Tolerant error number will be set to %d",m_nTolerantErrorNum);
		break;
	case VO_DATASOURCE_PID_STREAMCOUNT:
		//suppose it will be set when open complete
		m_nStreamCount = *((VO_U32*)pParam);
		VOLOGI("Source stream count is %d",m_nStreamCount);
		m_nTolerantErrorNum = ((m_nTolerantErrorNum < (m_nStreamCount -1)) ? m_nTolerantErrorNum : m_nStreamCount);
		break;
	case VO_PID_SOURCE2_HTTP_RETRY_TIMEOUT:
		m_CommonError.SetParameter(uID, pParam);
		break;
	default:
		ret =  VO_RET_SOURCE2_NOIMPLEMENT;
		break;
	}
	return ret;
}

VO_U32 CEventHandler::CheckWarningEvent( VO_U32 nType, VO_U32 nParam, VO_U32 *pRetryIntervalTime)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	switch (nType)
	{
	case VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_DOWNLOADERROR:
	case VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_PLAYLIST_DOWNLOADERROR:
	case VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_FILEFORMATUNSUPPORTED:
	case VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_DRMERROR:
		ret = m_CommonError.CheckError(nType,nParam, pRetryIntervalTime);
		break;
	default:
		break;
	}

	/*the count of continuing download error is more than DEFINE-NUMBER
	* It shall send error*/
	if( ret == VO_RET_SOURCE2_ONELINKFAIL)
	{
		m_nErrorNum++;
		switch(nType)
		{
			case VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_DOWNLOADERROR:
			case VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_PLAYLIST_DOWNLOADERROR:
			{
				CheckErrorEvent( VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_STREAMING_DOWNLOADFAIL, 0, pRetryIntervalTime);
				break;
			}
			case VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_FILEFORMATUNSUPPORTED:
			{
				CheckErrorEvent(VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_STREAMING_UNSUPPORTED, 0, pRetryIntervalTime);
				break;
			}	
			case VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_DRMERROR:
			{
				CheckErrorEvent(VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_STREAMING_DRMLICENSEERROR, 0, pRetryIntervalTime);
				break;
			}
		}
		*pRetryIntervalTime = 0;
		if(nType != VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_PLAYLIST_DOWNLOADERROR)
		{
			VO_SOURCE2_ADAPTIVESTREAMING_CHUNKINFO *pInfo = (VO_SOURCE2_ADAPTIVESTREAMING_CHUNKINFO*) nParam;
			if(pInfo->Type != VO_SOURCE2_ADAPTIVESTREAMING_VIDEO &&
			pInfo->Type != VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO)
			{
				ret = VO_RET_SOURCE2_FAIL;  // do not run into select stream logic if it is not video stream error
			}
		}
	}
	else if(ret == VO_RET_SOURCE2_LINKFAIL)
	{
		m_nErrorNum = m_nTolerantErrorNum +1; //If link failed caused by network disable,make it pop error to uplay directly
		CheckErrorEvent( VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_STREAMING_DOWNLOADFAIL, 0, pRetryIntervalTime);
	}

	return ret;
}	

VO_U32 CEventHandler::CheckInfoEvent( VO_U32 nType, VO_U32 nParam, VO_U32 *pRetryIntervalTime)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	switch (nType)
	{
		case VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_PLAYLIST_DOWNLOADOK:
		case VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_CHUNK_DOWNLOADOK:
		case VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_FILEFORMATSUPPORTED:
		{
			m_pEventCB->SendEvent(m_pEventCB->pUserData, VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_INFO, nType, nParam);
			ret = m_CommonError.CheckError(nType,nParam, pRetryIntervalTime);
			break;
		}
		case VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_DRM_OK:
		{
			ret = m_CommonError.CheckError(nType,nParam, pRetryIntervalTime);
			break;	
		}
		default:
		{
			VOLOGI("EventHandling. pass info event through:%d", nType);
			ret = m_pEventCB->SendEvent(m_pEventCB->pUserData, VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_INFO, nType, nParam);
			break;
		}
	}

	return ret;
}	
VO_U32 CEventHandler::CheckErrorEvent( VO_U32 nType, VO_U32 nParam, VO_U32 *pRetryIntervalTime)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	if(!m_bSusPend && m_nErrorNum > m_nTolerantErrorNum)
	{
		VOLOGE("EventHandling. Send Error Event:%d", nType);
		m_pEventCB->SendEvent(m_pEventCB->pUserData, VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_ERROR, nType, nParam);
	}
	return ret;
}	

VO_U32 CEventHandler::CheckCustomEvent( VO_U32 nType, VO_U32 nParam, VO_U32 *pRetryIntervalTime)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	VOLOGI("EventHandling. Send CustomTag Event:%d", nType);

	m_pEventCB->SendEvent(m_pEventCB->pUserData, VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CUSTOMTAG, nType, nParam);

	return ret;
}

VO_U32 CEventHandler::CheckRetry( VO_U32 ret, VO_U32 nRetryIntervalTime)
{
	if(ret == VO_RET_SOURCE2_NEEDRETRY)
	{
		//if( m_sProgramType == VO_SOURCE2_STREAM_TYPE_VOD)
		VOLOGI("EventHandling. The retrun code is NeedRetry, so it will be held about %d ms", nRetryIntervalTime);
		VO_U32 st = voOS_GetSysTime();
		while(!m_bStop && voOS_GetSysTime() - st < nRetryIntervalTime)
			voOS_Sleep(10);
	}
	return ret;
}
