
#include "IOError.h"
#include "voLog.h"
#include "voSource2_IO.h"
#include "voOSFunc.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


#define IOERROR_INTERVALTIME_LAST_ERRORNONE				0
#define IOERROR_INTERVALTIME_CONNECTION_ERROR			2000	
#define IOERROR_INTERVALTIME_CONTENT_NOTFOUND			200	
#define IOERROR_INTERVALTIME_RESPONSE_TOOLARGE			200
#define IOERROR_INTERVALTIME_GET_RESPONSE_TIMEOUT		200	
#define IOERROR_INTERVALTIME_AUTHENTICATION_FAIL		200
#define IOERROR_INTERVALTIME_DEFAULT					200

CIOError::CIOError()
:m_sProgramType( VO_SOURCE2_STREAM_TYPE_VOD)
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

CIOError::~CIOError()
{

}
VO_U32 CIOError::CheckError(VO_U32 nType, VO_U32 nParam2, VO_U32 *pRetryIntervalTime)
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
	default:
		break;
	}
	
	
	return ret;

}
VO_U32 CIOError::ForChunk( VO_U32 nParam2, VO_U32 *pRetryIntervalTime, VO_BOOL isError )
{
	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKINFO *pInfo = (VO_SOURCE2_ADAPTIVESTREAMING_CHUNKINFO*) nParam2;
	
	TOLERATE_ST *pTolerate;
	if(pInfo->Type == VO_SOURCE2_ADAPTIVESTREAMING_AUDIO)
		pTolerate = &m_stAudio;
	else if(pInfo->Type == VO_SOURCE2_ADAPTIVESTREAMING_VIDEO || pInfo->Type == VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO)
		pTolerate = &m_stVideo;
	else if(pInfo->Type == VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE)
		pTolerate = &m_stSubtitle;

	VO_U32 nErrCode = pInfo->uReserved1;

	VO_U32 ret = ToCheckError( pTolerate, isError, nErrCode );
	if(ret == VO_RET_SOURCE2_NEEDRETRY)
		*pRetryIntervalTime = GetRetryIntervalTime(nErrCode);
	return ret;
}

VO_U32 CIOError::ForPlaylist(VO_U32 *pRetryIntervalTime, VO_BOOL isError  )
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

VO_U32 CIOError::ToCheckError(TOLERATE_ST *pTolerate, VO_BOOL isError, VO_U32 nErrCode)
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

VO_U32 CIOError::CheckVodError(TOLERATE_ST *pTolerate, VO_U32 nErrCode)
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
		if( voOS_GetSysTime() - pTolerate->nStartTime >= IOERROR_FAIL_TOLERATE_TIME )
		{
			VOLOGW("IOERROR. CONNECTION_ERROR. The time of continuous fail is more than IOERROR_FAIL_TOLERATE_TIME( %d )", IO_CONTINUE_TOLERATE_COUNTS);
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
			ret = VO_RET_SOURCE2_LINKFAIL;
		}
	}

	return ret;
}

VO_U32 CIOError::CheckLiveError(TOLERATE_ST *pTolerate, VO_U32 nErrCode)
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
	if( voOS_GetSysTime() - pTolerate->nStartTime >= IOERROR_FAIL_TOLERATE_TIME )
		ret = VO_RET_SOURCE2_LINKFAIL;

	return ret;
}

VO_U32 CIOError::GetRetryIntervalTime( VO_U32 nErrCode)
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
	case VO_SOURCE2_IO_HTTP_CONTENT_NOTFOUND:
		nIntervalTime = IOERROR_INTERVALTIME_CONTENT_NOTFOUND;
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
	default:
		nIntervalTime = IOERROR_INTERVALTIME_DEFAULT;
		break;
	}
	return nIntervalTime;
}

VO_U32 CIOError::SetParameter( VO_U32 uID, VO_PTR pParam)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	switch( uID)
	{
	case VO_PID_SOURCE2_PROGRAM_TYPE:
		m_sProgramType = *((VO_SOURCE2_PROGRAM_TYPE *)pParam);
		VOLOGI( "IOERROR. Set ProgramType: %d", m_sProgramType);
		break;
	default:
		break;
	}

	return ret;
}