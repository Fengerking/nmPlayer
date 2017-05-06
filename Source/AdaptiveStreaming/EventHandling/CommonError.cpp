
#include "CommonError.h"
#include "voLog.h"
#include "voSource2_IO.h"
#include "voOSFunc.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


#define IOERROR_INTERVALTIME_LAST_ERRORNONE				0
#define IOERROR_INTERVALTIME_CONNECTION_ERROR			2000	
#define IOERROR_INTERVALTIME_RESPONSE_TOOLARGE			200
#define IOERROR_INTERVALTIME_GET_RESPONSE_TIMEOUT		200	
#define IOERROR_INTERVALTIME_AUTHENTICATION_FAIL		200
#define IOERROR_INTERVALTIME_DEFAULT					200
#define IOERROR_INTERVALTIME_CLIENT_SIDE_ERROR			200
#define IOERROR_INTERVALTIME_SERVER_SIDE_ERROR			200
#define IOERROR_INTERVALTIME_REDIRECTION_ERROR			200
#define FILEERROR_INTERVALTIME_UNSUPPORTED				200
#define DRMERROR_INTERVALTIME_DRMCOMMON				200

CommonError::CommonError()
:m_sProgramType( VO_SOURCE2_STREAM_TYPE_VOD)
,m_uDrmErrorCount(0)
,m_uUnSupportCount(0)
,m_uInitDataFailCount(0)
,m_uRetryTimeout(1000 * 60 * 2)
{
	memset( &m_stAudio, 0, sizeof( TOLERATE_ST));
	memset( &m_stVideo, 0, sizeof( TOLERATE_ST));
	memset( &m_stSubtitle, 0, sizeof( TOLERATE_ST));
	memset( &m_stPlaylist, 0, sizeof( TOLERATE_ST));
	m_stAudio.nLastErrCode = VO_SOURCE2_IO_HTTP_LAST_ERRORNONE;
	m_stVideo.nLastErrCode = VO_SOURCE2_IO_HTTP_LAST_ERRORNONE;
	m_stSubtitle.nLastErrCode = VO_SOURCE2_IO_HTTP_LAST_ERRORNONE;
	m_stPlaylist.nLastErrCode = VO_SOURCE2_IO_HTTP_LAST_ERRORNONE;

}

CommonError::~CommonError()
{

}
VO_U32 CommonError::CheckError(VO_U32 nType, VO_U32 nParam2, VO_U32 *pRetryIntervalTime)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	switch( nType )
	{
		/*info*/

	case VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_CHUNK_DOWNLOADOK:
		ret = ForChunk( nParam2, pRetryIntervalTime, VO_FALSE);
		break;
	case VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_PLAYLIST_DOWNLOADOK:
		ret = ForPlaylist( pRetryIntervalTime, VO_FALSE);
		break;
		/*warning*/
	case VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_DOWNLOADERROR:
		ret = ForChunk( nParam2, pRetryIntervalTime, VO_TRUE); 
		break;
	case VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_PLAYLIST_DOWNLOADERROR:
		ret = ForPlaylist( pRetryIntervalTime, VO_TRUE); 
		break;
	case VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_FILEFORMATSUPPORTED:
		ret = ForFileFormat(nParam2, pRetryIntervalTime, VO_FALSE);
		break;
	case VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_FILEFORMATUNSUPPORTED:
		ret = ForFileFormat(nParam2, pRetryIntervalTime, VO_TRUE);
		break;
	case VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_DRM_OK:
		ret = ForDrm(nParam2, pRetryIntervalTime, VO_FALSE);
		break;
	case VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_DRMERROR:
		ret = ForDrm(nParam2, pRetryIntervalTime, VO_TRUE);
		break;
	default:
		break;
	}
	
	
	return ret;

}
VO_U32 CommonError::ForChunk( VO_U32 nParam2, VO_U32 *pRetryIntervalTime, VO_BOOL isError )
{
	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKINFO *pInfo = (VO_SOURCE2_ADAPTIVESTREAMING_CHUNKINFO*) nParam2;
	
	TOLERATE_ST *pTolerate;
	if(pInfo->Type == VO_SOURCE2_ADAPTIVESTREAMING_AUDIO)
		pTolerate = &m_stAudio;
	else if(pInfo->Type == VO_SOURCE2_ADAPTIVESTREAMING_VIDEO || pInfo->Type == VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO)
		pTolerate = &m_stVideo;
	else if(pInfo->Type == VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE)
		pTolerate = &m_stSubtitle;
	else
	{
		if(isError)
		{
			VOLOGW("Init data in event handle");
			m_uInitDataFailCount++;
			if(m_uInitDataFailCount > INIT_DATA_TOLERATE_COUNTS)
			{
				VOLOGE("INIT DATA ERROR reach warning handler upper limit");
				return VO_RET_SOURCE2_ONELINKFAIL;
			}
			else
			{
				*pRetryIntervalTime = GetRetryIntervalTime(0);
				return VO_RET_SOURCE2_NEEDRETRY;
			}
		}
		else
		{
			m_uInitDataFailCount = 0;
			return VO_RET_SOURCE2_OK;
		}
	}

	VO_U32 nErrCode = pInfo->uReserved1;

	VO_U32 ret = ToCheckError( pTolerate, isError, nErrCode );
	if(ret == VO_RET_SOURCE2_NEEDRETRY)
		*pRetryIntervalTime = GetRetryIntervalTime(nErrCode);
	return ret;
}

VO_U32 CommonError::ForPlaylist(VO_U32 *pRetryIntervalTime, VO_BOOL isError  )
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	TOLERATE_ST *pTolerate;
	pTolerate = &m_stPlaylist;

	VO_U32 nErrCode = 0;//pInfo->uReserved1;
	ret = ToCheckError( pTolerate, isError, nErrCode );
	if(ret == VO_RET_SOURCE2_NEEDRETRY)
		*pRetryIntervalTime = GetRetryIntervalTime(nErrCode);
	return ret;
}

VO_U32 CommonError::ForDrm(VO_U32 nParam2, VO_U32 *pRetryIntervalTime, VO_BOOL isError)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	if(isError)
	{
		//do not need check vod or live for drm error
		m_uDrmErrorCount++;
		if(m_uDrmErrorCount >= DRM_FAIL_TOLERATE_COUNTS)
		{
			VOLOGE("DRM ERROR reach warning handler upper limit");
			ret = VO_RET_SOURCE2_ONELINKFAIL;
		}
		else
		{
			VOLOGW("DRM ERROR one chunk failed");
			ret =  VO_RET_SOURCE2_NEEDRETRY;
		}	
	}
	else
	{
		m_uDrmErrorCount = 0;
	}
	if(ret == VO_RET_SOURCE2_NEEDRETRY)
		*pRetryIntervalTime = DRMERROR_INTERVALTIME_DRMCOMMON;
	return ret;
}

VO_U32 CommonError::ForFileFormat(VO_U32 nParam2, VO_U32 *pRetryIntervalTime , VO_BOOL isError)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	if(isError)
	{
		//do not need check vod or live for file format unsupport error
		m_uUnSupportCount++;
		if(m_uUnSupportCount >= FILEUNSUPPORT_FAIL_TOLERATE_COUNTS)
		{
			VOLOGE("FILE FORMAT UNSUPPORT reach warning handler upper limit");
			ret = VO_RET_SOURCE2_ONELINKFAIL;
		}
		else
		{	
			VOLOGW("FILE FORMAT UNSUPPORT one chunk failed");
			ret =  VO_RET_SOURCE2_ONECHUNKFAIL;
		}	
	}
	else
	{
		m_uUnSupportCount= 0;
	}
	if(ret == VO_RET_SOURCE2_NEEDRETRY)
		*pRetryIntervalTime = FILEERROR_INTERVALTIME_UNSUPPORTED;
	return ret;
}


VO_U32 CommonError::ToCheckError(TOLERATE_ST *pTolerate, VO_BOOL isError, VO_U32 nErrCode)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	if( isError)
	{
		VOLOGI(" IOERROR. ErrCode = %x.", nErrCode);

		if( m_sProgramType == VO_SOURCE2_STREAM_TYPE_VOD)
			ret = CheckVodError( pTolerate, nErrCode);
		else if( m_sProgramType == VO_SOURCE2_STREAM_TYPE_LIVE)
			ret = CheckLiveError( pTolerate, nErrCode);
	}
	else
	{
		pTolerate->nFailTolerateNumbers = 0;
		pTolerate->nContinueTolerateNumbers = 0;
		pTolerate->nLastErrCode = VO_SOURCE2_IO_HTTP_LAST_ERRORNONE;
		pTolerate->nStartTime = 0;
	}
	return ret;	
}

VO_U32 CommonError::CheckVodError(TOLERATE_ST *pTolerate, VO_U32 nErrCode)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	/*
	VOD
	1. For Connect Fail
		We will continuously retry this chunk, the interval will be 2 seconds
		If we continuously fail for about 2 mins, we will return error
	2. For Other Download Fail
		We will retry download one chunk for 3 times, the interval will be 200ms
		If we continuously fail for 10 chunks, we will return error

	*/
	if( pTolerate->nLastErrCode != nErrCode  )
		pTolerate->nStartTime = voOS_GetSysTime();
	pTolerate->nLastErrCode = nErrCode;

	pTolerate->nContinueTolerateNumbers ++;

	if( nErrCode == VO_SOURCE2_IO_HTTP_CONNECTION_ERROR )
	{
		if( voOS_GetSysTime() - pTolerate->nStartTime >= m_uRetryTimeout )
		{
			VOLOGW("IOERROR. CONNECTION_ERROR. The time of continuous fail is more than m_uRetryTimeout( %d )", m_uRetryTimeout);
			ret = VO_RET_SOURCE2_LINKFAIL;
		}
		else
		{
			VOLOGW("IOERROR. CONNECTION_ERROR. It need retry.");
			ret = VO_RET_SOURCE2_NEEDRETRY;
		}
	}
	else
	{
		if( pTolerate->nContinueTolerateNumbers >= IO_CONTINUE_TOLERATE_COUNTS)
		{
			pTolerate->nContinueTolerateNumbers = 0;
			pTolerate->nFailTolerateNumbers ++;
			VOLOGW("IOERROR. The failure count of one CHUNK is more than Continue-Tolerate( %d )", IO_CONTINUE_TOLERATE_COUNTS);
			ret = VO_RET_SOURCE2_ONECHUNKFAIL;
		}
		else
		{
			ret = VO_RET_SOURCE2_NEEDRETRY;
		}

		if(pTolerate->nFailTolerateNumbers >= IO_FAIL_TOLERATE_COUNTS)
		{
			pTolerate->nFailTolerateNumbers = IO_FAIL_TOLERATE_COUNTS;
			VOLOGE("IOERROR. The failure count of one LINK is more than Fail-Tolerate( %d )", IO_FAIL_TOLERATE_COUNTS);
			ret = VO_RET_SOURCE2_ONELINKFAIL;
		}
	}

	return ret;
}

VO_U32 CommonError::CheckLiveError(TOLERATE_ST *pTolerate, VO_U32 nErrCode)
{
	/*Live
		Each Chunk will at most retry download for 3 times
		Retry Interval: ASAP
		Update thread continuously got download fail for about 2 mins we will return error
		Other download fail we will never return error
	*/
	VO_U32 ret = VO_RET_SOURCE2_OK;
	pTolerate->nContinueTolerateNumbers ++;

	if( pTolerate->nContinueTolerateNumbers >= IO_CONTINUE_TOLERATE_COUNTS)
	{
		pTolerate->nContinueTolerateNumbers = 0;
		pTolerate->nFailTolerateNumbers ++;
		VOLOGW("IOERROR. The failure count of one CHUNK is more than Continue-Tolerate( %d )", IO_CONTINUE_TOLERATE_COUNTS);
		ret = VO_RET_SOURCE2_ONECHUNKFAIL;
	}
	else
	{
		ret = VO_RET_SOURCE2_NEEDRETRY;
	}

	if( pTolerate->nStartTime == 0)
		pTolerate->nStartTime = voOS_GetSysTime();
	if( voOS_GetSysTime() - pTolerate->nStartTime >= m_uRetryTimeout )
		ret = VO_RET_SOURCE2_LINKFAIL;

	return ret;
}

VO_U32 CommonError::GetRetryIntervalTime( VO_U32 nErrCode)
{
	VO_U32 nIntervalTime = 0;
	switch( nErrCode )
	{
	case VO_SOURCE2_IO_HTTP_LAST_ERRORNONE:
		nIntervalTime = IOERROR_INTERVALTIME_LAST_ERRORNONE;
		break;
	case VO_SOURCE2_IO_HTTP_CONNECTION_ERROR:
		nIntervalTime = IOERROR_INTERVALTIME_CONNECTION_ERROR;
		break;
	case VO_SOURCE2_IO_HTTP_SERVER_SIDE_ERROR:
		nIntervalTime = IOERROR_INTERVALTIME_SERVER_SIDE_ERROR;
		break;
	case VO_SOURCE2_IO_HTTP_RESPONSE_TOOLARGE:
		nIntervalTime = IOERROR_INTERVALTIME_RESPONSE_TOOLARGE;
		break;
	case VO_SOURCE2_IO_HTTP_GET_RESPONSE_TIMEOUT:
		nIntervalTime = IOERROR_INTERVALTIME_GET_RESPONSE_TIMEOUT;
		break;
	case VO_SOURCE2_IO_HTTP_AUTHENTICATION_FAIL:
		nIntervalTime = IOERROR_INTERVALTIME_AUTHENTICATION_FAIL;
		break;
	case VO_SOURCE2_IO_HTTP_CLIENT_SIDE_ERROR:
		nIntervalTime = IOERROR_INTERVALTIME_CLIENT_SIDE_ERROR;
		break;
	case VO_SOURCE2_IO_HTTP_REDIRECTION_ERROR:
		nIntervalTime = IOERROR_INTERVALTIME_REDIRECTION_ERROR;
		break;
	default:
		nIntervalTime = IOERROR_INTERVALTIME_DEFAULT;
		break;
	}
	return nIntervalTime;
}

VO_U32 CommonError::SetParameter( VO_U32 uID, VO_PTR pParam)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	switch( uID)
	{
	case VO_PID_SOURCE2_PROGRAM_TYPE:
		m_sProgramType = *((VO_SOURCE2_PROGRAM_TYPE *)pParam);
		VOLOGI( "IOERROR. Set ProgramType: %d", m_sProgramType);
		break;
	case VO_PID_SOURCE2_HTTP_RETRY_TIMEOUT:
		{
			VO_U32 tmp = *(VO_U32*)pParam;
			if(tmp)
				m_uRetryTimeout = tmp * 1000;    //ms
			VOLOGI("Set Retry Timeout is 0x%08x",m_uRetryTimeout);
		}
		break;
	default:
		break;
	}

	return ret;
}

VO_VOID CommonError::Reset()
{
	m_stAudio.nFailTolerateNumbers = 0;
	m_stAudio.nContinueTolerateNumbers = 0;
	m_stAudio.nLastErrCode = VO_SOURCE2_IO_HTTP_LAST_ERRORNONE;
	m_stAudio.nStartTime = 0;

	m_stVideo.nFailTolerateNumbers = 0;
	m_stVideo.nContinueTolerateNumbers = 0;
	m_stVideo.nLastErrCode = VO_SOURCE2_IO_HTTP_LAST_ERRORNONE;
	m_stVideo.nStartTime = 0;

	m_stSubtitle.nFailTolerateNumbers = 0;
	m_stSubtitle.nContinueTolerateNumbers = 0;
	m_stSubtitle.nLastErrCode = VO_SOURCE2_IO_HTTP_LAST_ERRORNONE;
	m_stSubtitle.nStartTime = 0;

	m_stPlaylist.nFailTolerateNumbers = 0;
	m_stPlaylist.nContinueTolerateNumbers = 0;
	m_stPlaylist.nLastErrCode = VO_SOURCE2_IO_HTTP_LAST_ERRORNONE;
	m_stPlaylist.nStartTime = 0;
}