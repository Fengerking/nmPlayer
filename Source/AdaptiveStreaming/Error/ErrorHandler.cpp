
#include "ErrorHandler.h"
#include "voLog.h"
#include "voOSFunc.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CErrorHandler::CErrorHandler(VO_SOURCE2_EVENTCALLBACK * pEventCB, VO_ADAPTIVESTREAMPARSER_STREAMTYPE nType, VO_PTR pReserved)
:m_pEventCB(pEventCB)
,m_nStreamType( nType)
,m_bStop(VO_FALSE)
,m_sProgramType(VO_SOURCE2_STREAM_TYPE_VOD)
{

}

CErrorHandler::~CErrorHandler()
{

}

VO_U32 CErrorHandler::CheckEvent(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2, VO_PTR pReserved)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	VO_U32 nIntervalTime = 0;
	if( m_nStreamType != VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS
		&& m_nStreamType != VO_ADAPTIVESTREAMPARSER_STREAMTYPE_ISS
		&& m_nStreamType != VO_ADAPTIVESTREAMPARSER_STREAMTYPE_MAX )
		return ret;

	switch( nID )
	{
	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_WARNING:
		ret = CheckWarningEvent(nParam1, nParam2, &nIntervalTime);
		break;

	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_INFO:
		ret =  CheckInfoEvent( nParam1, nParam2, &nIntervalTime);
		break;
	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_ERROR:
		ret =  CheckErrorEvent( nParam1, nParam2, &nIntervalTime);
		break;
	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CUSTOMTAG:
		ret =  CheckCustomEvent( nParam1, nParam2, &nIntervalTime);
		break;
	}

	CheckRetry(ret, nIntervalTime);

	return ret;
}


VO_U32 CErrorHandler::GetParameter( VO_U32 uID, VO_PTR pParam )
{
	return VO_RET_SOURCE2_NOIMPLEMENT;
}

VO_U32 CErrorHandler::SetParameter( VO_U32 uID, VO_PTR pParam )
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	switch(uID)
	{
	case VO_PID_ADAPTIVESTREAMING_STREAMTYPE:
		m_nStreamType = *((VO_ADAPTIVESTREAMPARSER_STREAMTYPE *) pParam);
		break;
	case VO_PID_SOURCE2_PROGRAM_TYPE:
		m_sProgramType = *((VO_SOURCE2_PROGRAM_TYPE *)pParam);
		m_IOError.SetParameter( uID, pParam);
		break;
	default:
		ret =  VO_RET_SOURCE2_NOIMPLEMENT;
		break;
	}
	return ret;
}

VO_U32 CErrorHandler::CheckWarningEvent( VO_U32 nType, VO_U32 nParam, VO_U32 *pRetryIntervalTime)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	
	switch (nType)
	{
	case VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_DOWNLOADERROR:
	case VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_PLAYLIST_DOWNLOADERROR:
		ret = m_IOError.CheckError(nType,nParam, pRetryIntervalTime);
		break;
	default:
		break;
	}

	/*the count of continuing download error is more than DEFINE-NUMBER
	* It shall send error*/
	if( ret == VO_RET_SOURCE2_LINKFAIL)
	{
		CheckErrorEvent( VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_STREAMING_DOWNLOADFAIL, 0, pRetryIntervalTime);
		*pRetryIntervalTime = 0;
	}
// 	else
// 		m_pEventCB->SendEvent(m_pEventCB->pUserData, VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_WARNING, nType, nParam);


	return ret;
}	

VO_U32 CErrorHandler::CheckInfoEvent( VO_U32 nType, VO_U32 nParam, VO_U32 *pRetryIntervalTime)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	switch (nType)
	{
	case VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_PLAYLIST_DOWNLOADOK:
	case VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_CHUNK_DOWNLOADOK:
		ret = m_IOError.CheckError(nType,nParam, pRetryIntervalTime);
		break;
	
	case VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_BITRATE_CHANGE:
		VOLOGI("ErrorHandling. Send BAInfo Event:%d", nType);
		m_pEventCB->SendEvent(m_pEventCB->pUserData, VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_INFO, nType, nParam);
		break;
	default:
		ret = VO_RET_SOURCE2_NOIMPLEMENT;
		break;
	}

	return ret;
}	
VO_U32 CErrorHandler::CheckErrorEvent( VO_U32 nType, VO_U32 nParam, VO_U32 *pRetryIntervalTime)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	VOLOGE("ErrorHandling. Send Error Event:%d", nType);
	m_pEventCB->SendEvent(m_pEventCB->pUserData, VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_ERROR, nType, nParam);

	return ret;
}	

VO_U32 CErrorHandler::CheckCustomEvent( VO_U32 nType, VO_U32 nParam, VO_U32 *pRetryIntervalTime)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	VOLOGI("ErrorHandling. Send CustomTag Event:%d", nType);

	m_pEventCB->SendEvent(m_pEventCB->pUserData, VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CUSTOMTAG, nType, nParam);

	return ret;
}

VO_U32 CErrorHandler::CheckRetry( VO_U32 ret, VO_U32 nRetryIntervalTime)
{
	if(ret == VO_RET_SOURCE2_NEEDRETRY)
	{
		//if( m_sProgramType == VO_SOURCE2_STREAM_TYPE_VOD)
		VOLOGI("ErrorHandling. NeedRetry, so it will be held %d ms", nRetryIntervalTime);
		VO_U32 st = voOS_GetSysTime();
		while(!m_bStop && voOS_GetSysTime() - st < nRetryIntervalTime)
			voOS_Sleep(10);
	}
	return ret;
}