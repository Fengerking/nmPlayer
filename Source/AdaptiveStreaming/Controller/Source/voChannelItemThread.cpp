#include "voChannelItemThread.h"
#include "voOSFunc.h"
#include "CSourceIOUtility.h"
#include "voLog.h"
#include "voAdaptiveStreamingClassFactory.h"
#include "voSmartPointor.hpp"
#include "voToolUtility.h"
#include "voDSType.h"
#include "voDSByteRange.h"



#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifndef LOG_TAG
#define LOG_TAG "voChannelItemThread"
#endif


#define __ISStreamingType( A , B) \
{\
	switch( A ) \
{ \
	case FileFormat_MP4: \
	case FileFormat_SMTH: \
	case FileFormat_WEBVTT: \
	case FileFormat_AAC: \
	case FileFormat_MP3: \
	case FileFormat_Unknown: \
	B = VO_FALSE; \
	break; \
	case FileFormat_TS: \
	B = VO_TRUE; \
	break;\
}  \
}

voChannelItemThread::voChannelItemThread(void)
:m_is_stop(VO_FALSE)
,m_pFileParser(0)
,m_audio_status( PARSEITEMSTATUS_NORMAL )
,m_video_status( PARSEITEMSTATUS_NORMAL )
,m_ref_timestamp( 0 )
,m_chunkType( VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE_MAX )
,m_trackType( VO_SOURCE2_TT_MAX )
,m_lastTrackType(VO_SOURCE2_TT_MAX)
,m_LastStartTime(_VODS_INT64_MAX)
,m_bLastAudioBAFlag(VO_FALSE)
,m_bLastVideoBAFlag(VO_FALSE)
,m_bCanSendFirstSample(VO_FALSE)
,m_bSwitchAudioThread(VO_FALSE)
,m_uChunkID(_VODS_INT64_MAX)
#ifdef _USE_BUFFERCOMMANDER
,m_pDownloadAgent(NULL)
#endif
,m_nReadedSize(0)
{
	m_sample_callback.pUserData = this;
	m_sample_callback.SendData = OnSample;
}

voChannelItemThread::~voChannelItemThread(void)
{
	DestroyAdaptiveStreamingFileParser( m_pFileParser );
	m_pFileParser = NULL;
}

VO_VOID voChannelItemThread::Stop()
{
	THREADTYPE type =  m_ptr_info ? m_ptr_info->type : THREADTYPE_MEDIA_MAX;
	VODS_VOLOGI(" + Stop ChannelItemThread(%d).", type);
	VO_U32 start_time = voOS_GetSysTime();

	m_is_stop = VO_TRUE;
	voItemThread::Stop();
	m_is_stop = VO_FALSE;

	/*
	as little huihui's suggestion, reset it
	2012-12-03, issue #21327	
	*/
	m_ref_timestamp = 0;
	m_bCanSendFirstSample = VO_FALSE;
	m_LastStartTime = _VODS_INT64_MAX;
	ResetCustomerTagMap();
	if(m_ptr_info)
	{
		m_ptr_info->pInfo->SetUpdateUrlOn(type,VO_FALSE);
	}
	VODS_VOLOGI( " - Stop ChannelItemThread(%d). cost %d ms" ,type, voOS_GetSysTime() - start_time );
}

VO_VOID voChannelItemThread::Resume()
{
	voItemThread::Resume();
	m_bCanSendFirstSample = VO_TRUE;
}

void voChannelItemThread::thread_function()
{
	if( !m_ptr_info )
		return;

	if(m_ptr_info->type == THREADTYPE_MEDIA_AUDIO)
		set_threadname( "Get Chunk Audio" );
	else if(m_ptr_info->type == THREADTYPE_MEDIA_VIDEO)
		set_threadname( "Get Chunk Video" );
	else 
		set_threadname( "Get Chunk Subtitle" );

	VODS_VOLOGI( "+GetChannelItem Thread %d Start.ChunkType:%d!" , m_ptr_info->type, m_chunkType );

	CIntervalPrintf intervalPrintf;
	VO_U32 nCountRetry = 0;
	VO_U32 nCountNotAvalible = 0;
	VO_U32 nCountOK = 0;
	VO_U64 nChunkID = 0;
	VO_U32 ret = VO_RET_SOURCE2_OK;

	VO_BOOL filechunkreset = VO_FALSE;
	VO_U32  nLastSavedFlag = 0;
	VO_BOOL nDiscontinueInherit = VO_FALSE;
	m_bAudioChunkDropped = m_bVideoChunkDropped = m_bSubtitleChunkDropped = m_bFormatChange = VO_FALSE;
	m_bChunkForceOutput = VO_FALSE;
	m_nThreadID = get_current_threadid();


	while( ( ret != VO_RET_SOURCE2_END ) && !m_is_stop )
	{
		VO_U32 ret1 = VO_RET_SOURCE2_OK;
		/*use these two parameters to check that one sample has been pushed*/
		m_audio_count = m_video_count = m_subtitle_count = 0;
		m_bChunkBegin = VO_TRUE;
		m_bChunkEnd	 = VO_FALSE;
		m_bChunkDiscontinue = VO_FALSE;
		VO_ADAPTIVESTREAMPARSER_CHUNK * ptr_item = 0;
		if(m_ptr_info->pInfo->GetStreamingType() == VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS)
		{
			if(m_ptr_info->pInfo->IsUpdateUrlOn(m_ptr_info->type) == VO_TRUE)
			{
				VOLOGW("Update url before processChunk happen!");
				UpdateProcess(VO_TRUE);
			}
		}
		ret = m_ptr_info->pInfo->GetAdaptiveStreamingParserPtr()->GetChunk( m_chunkType , &ptr_item );
		switch( ret )
		{
		case VO_RET_SOURCE2_NEEDRETRY:
			{
				if( m_ptr_info->type == 0 )
				{
					//VODS_VOLOGW( "No Item for Download! Thread %d" , m_ptr_info->type );
				}
				nCountRetry ++;
				voOS_Sleep( 40 );
			}
			break;

		case VO_RET_SOURCE2_OUTPUTNOTAVALIBLE:
			{
				m_ptr_curitem = NULL;
				m_pDownloadAgent = NULL;
				//	SendChunkInfoEvent(VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CHUNKBEGIN, 0, 0, 0);
				if( (nChunkID == 0 || (VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE & nLastSavedFlag))
					&& (m_bVideoChunkDropped || m_bAudioChunkDropped))
				{
					TrackUnavailable(2);
					nLastSavedFlag = 0;
					nChunkID = 1;
				}
				else
					TrackUnavailable(1);
				nCountNotAvalible ++;
				//	SendChunkInfoEvent(VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CHUNKCOMPLETE, 0, 0, 0);
				voOS_Sleep( 40 );
			}
			break;

		case VO_RET_SOURCE2_OK:
			{
				if( !ptr_item )
				{
					VODS_VOLOGW(" chunk pointer error.Ret:%p Item:%p", ret, ptr_item );
					break;
				}
				VO_U32 ret_code = SendDownloadInfo(ptr_item);
				VOLOGI("SendDownloadInfo return is 0x%08x",ret_code);
				if(ret_code == VO_RET_SOURCE2_ADAPTIVESTREAMING_CHUNK_SKIP)
				{
					if(m_bFormatChange == VO_FALSE && (ptr_item->uFlag & VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE))
					{
						m_bFormatChange = VO_TRUE;
					}
					VOLOGW("SendDownloadInfo Warning for ad,skip this chunk,format change is %d",m_bFormatChange);
					ret1 = VO_RET_SOURCE2_CHUNKSKIPPED;
					break;	
				}
				if(m_bFormatChange == VO_TRUE)
				{
					ptr_item->uFlag |= VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
					m_bFormatChange = VO_FALSE;
				}
#ifdef _USE_BUFFERCOMMANDER
				ptr_item->uFlag &= ~VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_SMOOTH_ADAPTIONEX;
				ptr_item->uFlag &= ~VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_SMOOTH_ADAPTION;
#endif
				if((m_ptr_info->pInfo->GetStreamingType() == VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS ) ||
					(m_ptr_info->pInfo->GetStreamingType() == VO_ADAPTIVESTREAMPARSER_STREAMTYPE_ISS &&
					m_ptr_info->type == THREADTYPE_MEDIA_VIDEO) )
				{
					m_uChunkID = ptr_item->uChunkID;
				}
				else
				{
					m_uChunkID = _VODS_INT64_MAX;
				}

				//add for smooth streaming for possibile chunk dropped before
				if(VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_CHUNKDROPBEFORE & ptr_item->uFlag)
				{
					VODS_VOLOGI("some chunks before %s have been dropped. Type %d (%d, %d, %d)", ptr_item->szUrl, ptr_item->Type, m_video_count, m_audio_count, m_subtitle_count);

					if(ptr_item->Type == VO_SOURCE2_ADAPTIVESTREAMING_AUDIO ||
						ptr_item->Type == VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO)
						m_bAudioChunkDropped = VO_TRUE;
					if(ptr_item->Type == VO_SOURCE2_ADAPTIVESTREAMING_VIDEO ||
						ptr_item->Type == VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO)
						m_bVideoChunkDropped = VO_TRUE;
					if(ptr_item->Type == VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE)
						m_bSubtitleChunkDropped = VO_TRUE;
				}

				// currently, we just force output audio after chunk dropped detected
				if(ptr_item->Type == VO_SOURCE2_ADAPTIVESTREAMING_AUDIO && m_bAudioChunkDropped == VO_TRUE)
				{
					VODS_VOLOGI("audio chunk dropped, we need force output this chunk");
					m_bChunkForceOutput = VO_TRUE;
				}

				m_ptr_info->pInfo->GetTimeStampSyncObject().SetChunkDuration( ptr_item->ullDuration / ( ptr_item->ullTimeScale / 1000.) );
				/*all adaptive streams have the unified treatment*/
				ptr_item->uFlag |= nLastSavedFlag;
				if(nDiscontinueInherit)
					m_bChunkDiscontinue = VO_TRUE;
				
				m_chunk_info.uChunkID = ptr_item->uChunkID;
				m_chunk_info.uTrackID = ptr_item->sKeyID.uTrackID;
				ret1 = ToDealItem_II(ptr_item, &filechunkreset);
				if((ret1 == VO_RET_SOURCE2_OK) && nDiscontinueInherit)
				{
					nDiscontinueInherit = VO_FALSE;
				}
				if(m_bChunkForceOutput == VO_TRUE)
				{
					VODS_VOLOGI("reset chunk force output");
					m_bChunkForceOutput = VO_FALSE;
				}

				nCountOK ++;
			}
			break;

		default:
			voOS_Sleep(40);
			break;
		}

		nLastSavedFlag = 0;
		if( !m_is_stop && ret == VO_RET_SOURCE2_OK &&
			(ptr_item->Type == VO_SOURCE2_ADAPTIVESTREAMING_AUDIO ||
			ptr_item->Type == VO_SOURCE2_ADAPTIVESTREAMING_VIDEO ||
			ptr_item->Type == VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE ||
			ptr_item->Type == VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO))
		{	
			VO_BOOL X = VO_FALSE;

			if( m_pFileParser )
			{
				__ISStreamingType( m_pFileParser->GetFileFormat(), X );
			}

			/*if no audio or no video has been pushed, it indicates that the chunk has been dropped.So it should been reset.
			But like TS parser, it maybe actually no sample be pushed. So skip TS FileParser*/

			if (ret1 != VO_RET_SOURCE2_CHUNKSKIPPED)
			{
				if(ret1 == VO_RET_SOURCE2_CHUNKDROPPED || ret1 == VO_RET_SOURCE2_FORMATUNSUPPORT
					|| ( !X && ( m_video_count == 0 && m_audio_count == 0 && m_subtitle_count == 0 ) )	)
				{
					VODS_VOLOGI("%s has been dropped. Ret: 0x%08x (%d, %d, %d)", ptr_item->szUrl, ret1, m_video_count, m_audio_count, m_subtitle_count);

					m_bVideoChunkDropped = m_bAudioChunkDropped = m_bSubtitleChunkDropped = VO_TRUE;
					if(ptr_item->Type == VO_SOURCE2_ADAPTIVESTREAMING_AUDIO)
					{
						m_ptr_info->pInfo->GetTimeStampSyncObject().Reset(0, RESET_FIRSTAUDIOFRAME);
					}
					else if(ptr_item->Type == VO_SOURCE2_ADAPTIVESTREAMING_VIDEO)
					{
						m_ptr_info->pInfo->GetTimeStampSyncObject().Reset(0, RESET_FIRSTVIDEOFRAME);
					}
					else if(ptr_item->Type == VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO)
					{
						m_ptr_info->pInfo->GetTimeStampSyncObject().Reset(0, RESET_FIRSTAVFRAME);
					}
					if(ptr_item &&  ( VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE & ptr_item->uFlag ) )
						nLastSavedFlag |= VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
					if(ptr_item &&  ( VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_PROGRAMCHANGE & ptr_item->uFlag ) )
						nLastSavedFlag |= VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_PROGRAMCHANGE;
					DoDrop( nLastSavedFlag );
				}
			}
			if(ptr_item && (VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_DISCONTINUE & ptr_item->uFlag) && 
				m_video_count == 0 && m_audio_count == 0 && ret1 != VO_RET_SOURCE2_OK)
			{
				VOLOGI("save discontinue flag for next chunk");
				nDiscontinueInherit = VO_TRUE;
			}
		}

		bool b = intervalPrintf( "%s Thread. Summary = [OK:%d, Retry:%d, NotAvalible:%d]", ThreadType2String(m_ptr_info->type), nCountOK, nCountRetry, nCountNotAvalible  );
		if(b)
			nCountOK = nCountNotAvalible = nCountRetry = 0;
	}

	if( ret == VO_RET_SOURCE2_END )
	{
		ThreadEnding();
	}

	VODS_VOLOGI( "-GetChannelItem Thread %d End! Ret:0x%08x. Is Stop %s" , m_ptr_info->type , ret , m_is_stop ? "TRUE" : "FALSE" );
}

VO_U32 voChannelItemThread::SendIOWarning(VO_HANDLE hHandle,  VO_U32 ret, VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk)
{	
	if (ret == VO_RET_SOURCE2_OK || hHandle == NULL)
		return ret;

	VO_SOURCE2_IO_API *pIO = m_ptr_info->pInfo->GetIOPtr();
	VO_SOURCE2_EVENTCALLBACK *pEventCallback = m_ptr_info->pInfo->GetEventCallback();

	VO_U32 errCode = pIO->GetLastError(hHandle);

	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKINFO info;
	memset( &info , 0 , sizeof( VO_SOURCE2_ADAPTIVESTREAMING_CHUNKINFO ) );
	info.ullDuration = pChunk->ullDuration / ( pChunk->ullTimeScale / 1000. );
	info.ullTimeScale = pChunk->ullTimeScale;
	info.Type = ( VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE)pChunk->Type;
	memcpy( info.szRootUrl, pChunk->szRootUrl, sizeof(pChunk->szRootUrl) );
	memcpy( info.szUrl, pChunk->szUrl, sizeof(pChunk->szUrl) );
	info.ullStartTime = pChunk->ullStartTime;
	info.uReserved1 = errCode;

	return pEventCallback->SendEvent( pEventCallback->pUserData , VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_WARNING , VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_DOWNLOADERROR,(VO_U32)&info);

}

VO_U32 voChannelItemThread::SendIOWarning_II( VO_U32 nEventParam2, VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk, VO_U32 nErrCode)
{	
	if( nEventParam2 == VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_MAX || m_is_stop)
		return VO_RET_SOURCE2_OK;

	VO_SOURCE2_EVENTCALLBACK *pEventCallback = m_ptr_info->pInfo->GetEventCallback();

	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKINFO info;
	memset( &info , 0 , sizeof( VO_SOURCE2_ADAPTIVESTREAMING_CHUNKINFO ) );
	info.ullDuration = pChunk->ullDuration / ( pChunk->ullTimeScale / 1000. );
	info.ullTimeScale = pChunk->ullTimeScale;
	info.Type = ( VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE)pChunk->Type;
	memcpy( info.szRootUrl, pChunk->szRootUrl, sizeof(pChunk->szRootUrl) );
	memcpy( info.szUrl, pChunk->szUrl, sizeof(pChunk->szUrl) );
	info.ullStartTime = pChunk->ullStartTime;
	info.uReserved1 = nErrCode;

	return pEventCallback->SendEvent( pEventCallback->pUserData , VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_WARNING , nEventParam2,(VO_U32)&info);

}

VO_U32 voChannelItemThread::SendDownloadInfo(VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk)
{
	VO_SOURCE2_EVENTCALLBACK *pEventCallback = m_ptr_info->pInfo->GetEventCallback();
	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKINFO info;

	memset( &info , 0 , sizeof( VO_SOURCE2_ADAPTIVESTREAMING_CHUNKINFO ) );
	info.ullDuration = pChunk->ullDuration / ( pChunk->ullTimeScale / 1000. );
	info.ullTimeScale = pChunk->ullTimeScale;
	info.uPeriodSequenceNumber = pChunk->uPeriodSequenceNumber;
	info.Type = ( VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE)pChunk->Type;
	memcpy( info.szRootUrl, pChunk->szRootUrl, sizeof(pChunk->szRootUrl) );
	memcpy( info.szUrl, pChunk->szUrl, sizeof(pChunk->szUrl) );
	info.ullStartTime = pChunk->ullStartTime/ (pChunk->ullTimeScale / 1000. );
	info.uReserved1 = 0;
	info.uReserved2 = pChunk->uFlag;
	VOLOGI("Send DownloadInfo to up layer");
	return pEventCallback->SendEvent( pEventCallback->pUserData , VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_INFO , VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_CHUNK_BEGINDOWNLOAD,(VO_U32)&info);
}

VO_U32 voChannelItemThread::SendChunkSample(VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk, _SAMPLE * pSample)
{
	VO_SOURCE2_EVENTCALLBACK *pEventCallback = m_ptr_info->pInfo->GetEventCallback();
	VO_SOURCE2_CHUNK_SAMPLE sample;
	VO_U64 tmpTime = pSample->uTime;
	if(m_bCanSendFirstSample == VO_TRUE)
	{
		sample.nChunkFlag = pChunk->uFlag |VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_NEEDREF;
		m_bCanSendFirstSample = VO_FALSE;
	}
	else
	{
		sample.nChunkFlag = pChunk->uFlag;	
	}
	if(m_ptr_info->pInfo->GetStreamingType() == VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS)
	{
		VO_SOURCE2_PERIODTIMEINFO pParam;
		pParam.uPeriodSequenceNumber = pChunk->uPeriodSequenceNumber;
		m_ptr_info->pInfo->GetAdaptiveStreamingParserPtr()->SetParam(VO_PID_ADAPTIVESTREAMING_PERIOD2TIME, (VO_PTR)&pParam);
		sample.ullPeriodFirstChunkStartTime = pParam.ullTimeStamp;
	}
	else
		sample.ullPeriodFirstChunkStartTime = -1;
	sample.ullChunkStartTime = pChunk->ullStartTime/ (pChunk->ullTimeScale / 1000. );
	sample.uPeriodSequenceNumber = pChunk->uPeriodSequenceNumber;
	sample.pullRetTimeStamp = &tmpTime;
	sample.pReserve1 = NULL;
	sample.pReserve2 = NULL;
	VOLOGI("vo_ads_manager::Send Chunk first sample,nChunkFlag is 0x%08x,PeriodNumber is %d,PeriodStartTime is %lld ,time is %lld",sample.nChunkFlag,
		sample.uPeriodSequenceNumber,sample.ullPeriodFirstChunkStartTime,tmpTime);

	VO_U32 ret = pEventCallback->SendEvent( pEventCallback->pUserData , VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_INFO , VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_DISCONTINUESAMPLE,(VO_U32)&sample);
	if(ret == VO_RET_SOURCE2_ADAPTIVESTREAMING_FORCETIMESTAMP)
	{
		VOLOGI("Adjust time is %lld",*(sample.pullRetTimeStamp));
		ret = pEventCallback->SendEvent( pEventCallback->pUserData , VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CHUNKTIMEREVISE , (VO_U32)&sample , 0 );
	}
	return ret;
}

VO_U32 voChannelItemThread::SendIOInfo(VO_U32 nEventParam2, VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk)
{

	VO_SOURCE2_EVENTCALLBACK *pEventCallback = m_ptr_info->pInfo->GetEventCallback();

	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKINFO info;
	memset( &info , 0 , sizeof( VO_SOURCE2_ADAPTIVESTREAMING_CHUNKINFO ) );
	info.ullDuration = pChunk->ullDuration / ( pChunk->ullTimeScale / 1000.);
	info.ullTimeScale = pChunk->ullTimeScale;
	info.Type = ( VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE)pChunk->Type;
	memcpy( info.szRootUrl, pChunk->szRootUrl, sizeof(pChunk->szRootUrl) );
	memcpy( info.szUrl, pChunk->szUrl, sizeof(pChunk->szUrl) );
	info.ullStartTime = pChunk->ullStartTime;
	info.uReserved1 = 0;

	return pEventCallback->SendEvent( pEventCallback->pUserData , VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_INFO , nEventParam2,(VO_U32)&info);
}

VO_U32 voChannelItemThread::SendDrmWarning(VO_U32 nEventParam2, VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk, VO_U32 nErrCode)
{
	if( nEventParam2 == VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_MAX )
		return VO_RET_SOURCE2_OK;

	VO_SOURCE2_EVENTCALLBACK *pEventCallback = m_ptr_info->pInfo->GetEventCallback();

	//Chunk infomation may not be used, reserve first here
	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKINFO info;
	memset( &info , 0 , sizeof( VO_SOURCE2_ADAPTIVESTREAMING_CHUNKINFO ) );
	info.ullDuration = pChunk->ullDuration / ( pChunk->ullTimeScale / 1000. );
	info.ullTimeScale = pChunk->ullTimeScale;
	info.Type = ( VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE)pChunk->Type;
	memcpy( info.szRootUrl, pChunk->szRootUrl, sizeof(pChunk->szRootUrl) );
	memcpy( info.szUrl, pChunk->szUrl, sizeof(pChunk->szUrl) );
	info.ullStartTime = pChunk->ullStartTime;
	info.uReserved1 = nErrCode;

	return pEventCallback->SendEvent( pEventCallback->pUserData , VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_WARNING , nEventParam2,(VO_U32)&info);
}

VO_U32 voChannelItemThread::SendDrmError(VO_U32 nEventParam2, VO_U32 nErrCode)
{
	if(nEventParam2 == VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_MAX)
		return VO_RET_SOURCE2_OK;

	VO_SOURCE2_EVENTCALLBACK *pEventCallback = m_ptr_info->pInfo->GetEventCallback();
	VO_U32 errCode = nErrCode;
	return pEventCallback->SendEvent(pEventCallback->pUserData, VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_ERROR, nEventParam2, errCode);
}

VO_U32 voChannelItemThread::SendLicenseCheckError(VO_U32 nEventParam2, VO_U32 nErrCode)
{
	if( nEventParam2 == VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_MAX )
		return VO_RET_SOURCE2_OK;

	VO_SOURCE2_EVENTCALLBACK *pEventCallback = m_ptr_info->pInfo->GetEventCallback();
	VO_U32 errCode = nErrCode;
	return pEventCallback->SendEvent( pEventCallback->pUserData , VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_ERROR , nEventParam2,errCode);
}

VO_U32 voChannelItemThread::ToGetItem( VO_ADAPTIVESTREAMPARSER_CHUNK * ptr_item,VO_U32 &nErrCode )
{
	VO_BOOL bFlag = VO_BOOL(m_ptr_curitem->Type != VO_SOURCE2_ADAPTIVESTREAMING_HEADDATA &&
		m_ptr_curitem->Type != VO_SOURCE2_ADAPTIVESTREAMING_SEGMENTINDEX &&
		m_ptr_curitem->Type != VO_SOURCE2_ADAPTIVESTREAMING_INITDATA);
#ifdef _USE_BUFFERCOMMANDER
	return GetItem_IV( ptr_item, bFlag, nErrCode );
#else
	return GetItem_III( ptr_item, bFlag, nErrCode );
#endif
}
#ifndef _USE_BUFFERCOMMANDER
VO_U32 voChannelItemThread::GetItem_III( VO_ADAPTIVESTREAMPARSER_CHUNK * ptr_item, VO_BOOL bFlag, VO_U32 &nErrCode )
{
	VODS_VOLOGI("+Type %d Start to Download %s", m_ptr_info->type , ptr_item->szUrl);

	VO_HANDLE h = NULL;
	VO_U32 ret = 0;

	VO_CHAR url[MAXURLLEN] = {0};
	memset( url, 0x00, sizeof(url) * sizeof(VO_CHAR) );

	GetTheAbsolutePath( url , ptr_item->szUrl , ptr_item->szRootUrl );

	VO_SOURCE2_IO_API *pIO = m_ptr_info->pInfo->GetIOPtr();
	VO_SOURCE2_EVENTCALLBACK *pEventCallback = m_ptr_info->pInfo->GetEventCallback();
	voAutoIOInit init_obj( pIO , url , &h );

	if( init_obj.m_ret != VO_SOURCE2_IO_OK )
	{
		nErrCode = pIO->GetLastError(h);
		VODS_VOLOGW("-AutoIO Init failed( errCode:0x%08x )! %s",nErrCode, url);
		return VO_RET_SOURCE2_CHUNKDROPPED;
	}


	if( ptr_item->ullChunkOffset != INAVALIBLEU64 )
	{
		VO_SOURCE2_IO_HTTPRANGE range;
		range.ullOffset = ptr_item->ullChunkOffset;
		range.ullLength = ptr_item->ullChunkSize;
		pIO->SetParam( h , VO_SOURCE2_IO_PARAMID_HTTPRANGE , &range );

		VODS_VOLOGI( "Range: %lld , %lld" , range.ullOffset , range.ullLength );
	}

	if( m_ptr_info->pInfo->GetMaxDownloadBitrate() != _VODS_INT32_MAX )
	{
		VO_S32 maxbitrate = m_ptr_info->pInfo->GetMaxDownloadBitrate();
		pIO->SetParam( h , VO_SOURCE2_IO_PARAMID_HTTPMAXSPEED , &maxbitrate );
	}

	VO_SOURCE2_VERIFICATIONINFO * pInfo = m_ptr_info->pInfo->GetVerificationInfo();

	if( pInfo )
	{
		pIO->SetParam( h , VO_SOURCE2_IO_PARAMID_DOHTTPVERIFICATION , pInfo );
	}
	VO_SOURCE2_IO_HTTPCALLBACK *pHttpCb = m_ptr_info->pInfo->GetIOHttpCallback();
	if( pHttpCb )
	{
		pIO->SetParam(h, VO_SOURCE2_IO_PARAMID_HTTPIOCALLBACK, pHttpCb );
	}

	//	voAutoIOOpen open_obj( pIO , h , VO_FALSE );
	//using async open for sourceIO
	voAutoIOOpen open_obj( pIO , h , VO_TRUE );

	if( open_obj.m_ret != VO_SOURCE2_IO_OK )
	{
		nErrCode = pIO->GetLastError(h);
		VODS_VOLOGW("-AutoIO Open failed( errCode:%d )! %s",nErrCode, url);
		return VO_RET_SOURCE2_CHUNKDROPPED;
	}

	if( !m_is_stop && bFlag &&!( ptr_item->uFlag & VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_SMOOTH_ADAPTION ) )
	{
		CHUNKINFO info;
		memset( &info , 0 , sizeof( CHUNKINFO ) );
		info.duration = ptr_item->ullDuration / ( ptr_item->ullTimeScale / 1000. );
		info.size = 0;
		info.chunktype = ptr_item->Type;
		/*2012.10.17, the start_timestamp is not used for BA. BA only uses end_timestamp to check the chunk's relegation now.
		* but the code is not be deleted
		*/
		info.start_timestamp = m_ref_timestamp + 1;
		pEventCallback->SendEvent( pEventCallback->pUserData , VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CHUNKBEGIN , (VO_U32)&info , ptr_item->uFlag);
	}

	VO_U32 readsize = 0;
	VODS_VOLOGI("+ ToProcessChunk.(URL:%s)", ptr_item->szUrl);
	ret = ToProcessChunk(h, ptr_item, &readsize);
	if(ret == VO_DATASOURCE_RET_DOWNLOAD_SLOW)
	{
		VOLOGW("download slow abort!");
	}
	else
	{
		if( readsize == 0 )
		{
			ret = VO_RET_SOURCE2_CHUNKDROPPED;
			VODS_VOLOGW(" this Chunk size is Zero.%s", url);
		}
		if( ret == VO_RET_SOURCE2_CHUNKDROPPED )
		{
			nErrCode = pIO->GetLastError(h);
			VODS_VOLOGW("-ToProcessChunk failed( errCode:0x%08x )! %s",nErrCode, url);
		}
		if( ret == VO_RET_SOURCE2_FORMATUNSUPPORT )
		{
			VODS_VOLOGW("-ToProcessChunk failed( VO_RET_SOURCE2_FORMATUNSUPPORT )! %s", url);
		}
	}
	VODS_VOLOGI("- ToProcessChunk.(URL:%s, Ret:%08x)", ptr_item->szUrl, ret);

	if( ret == VO_RET_SOURCE2_OK ||ret == VO_DATASOURCE_RET_DOWNLOAD_SLOW)
	{
		VO_U64 downloadtime = 0;
		VO_U32 ret1 = pIO->GetParam( h , VO_SOURCE2_IO_PARAMID_HTTPDOWNLOADTIME , &downloadtime );
		if( ret1 != VO_SOURCE2_IO_OK || downloadtime == 0 )
		{
			downloadtime = 1;
		}
		//If download slow,close IO before send chunkcomplete cause
		//this data download will block next manifest`s download
		if(ret == VO_DATASOURCE_RET_DOWNLOAD_SLOW)
		{
			if(pIO && pIO->Close && h)
			{
				pIO->Close(h);
			}
		}

		if( !m_is_stop && bFlag && !( ptr_item->uFlag & VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_SMOOTH_ADAPTION ) )
		{
			CHUNKINFO info;
			memset( &info , 0 , sizeof( CHUNKINFO ) );
			info.duration = ptr_item->ullDuration / ( ptr_item->ullTimeScale / 1000. );
			info.size = (VO_U64) readsize;
			info.chunktype = ptr_item->Type;
			info.download_time = (VO_U32)downloadtime;
			info.end_timestamp = m_ref_timestamp;
			pEventCallback->SendEvent( pEventCallback->pUserData , VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CHUNKCOMPLETE , (VO_U32)&info , 0 );

			SendIOInfo( VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_CHUNK_DOWNLOADOK, ptr_item );
			SendIOInfo( VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_FILEFORMATSUPPORTED, ptr_item );

			//if(m_bChunkEnd)
			{
				InfoChunk( m_ref_timestamp, VO_FALSE );
				m_bChunkEnd = VO_FALSE;
			}
		}

	VODS_VOLOGI("- Download OK! Time Cost %d Chunk duration %d Size %d Speed %d url %s", (VO_U32)downloadtime , (VO_U32)(ptr_item->ullDuration / ( ptr_item->ullTimeScale / 1000. )), readsize,(VO_U32)(readsize * 8000. /downloadtime) , url );
	}
	else
	{

	}
	VODS_VOLOGI("-Type. Ret:0x%08x", ret);
	return ret;
}
#endif

VO_U32 voChannelItemThread::GetItem_IV( VO_ADAPTIVESTREAMPARSER_CHUNK * ptr_item, VO_BOOL bFlag, VO_U32 &nErrCode )
{

	VODS_VOLOGI("+Type %d Start to Download %s", m_ptr_info->type , ptr_item->szUrl);

	VO_U32 ret = 0;
#ifdef _USE_BUFFERCOMMANDER
	if( !m_is_stop && bFlag  )
		ret = SendChunkInfoEvent(VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CHUNKBEGIN,0,0, 0);

	VO_U64 uDownloadTime = 1;// 2013-09-04 Leon, 1 is for protecting division to 0
	VO_U32 uDownloadSize = 0;
	if( !m_is_stop )
		ret = Downloading(uDownloadTime, uDownloadSize,nErrCode);
	if(m_ptr_info->type == THREADTYPE_MEDIA_VIDEO && m_ptr_info->pInfo->IsBitrateChanging() && ret == VO_RET_SOURCE2_OK &&
		ptr_item->Type != VO_SOURCE2_ADAPTIVESTREAMING_HEADDATA && 
		ptr_item->Type != VO_SOURCE2_ADAPTIVESTREAMING_SEGMENTINDEX &&
		ptr_item->Type != VO_SOURCE2_ADAPTIVESTREAMING_INITDATA)
	{
		m_ptr_info->pInfo->SetBitrateChanging(VO_FALSE);
	}

	if( !m_is_stop && bFlag)
		VO_U32 tmpRet = SendChunkInfoEvent(VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CHUNKCOMPLETE, uDownloadTime, uDownloadSize, ret);

	if( !m_is_stop && bFlag && ret == VO_RET_SOURCE2_OK)
	{
		SendIOInfo( VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_CHUNK_DOWNLOADOK, ptr_item );
		SendIOInfo( VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_FILEFORMATSUPPORTED, ptr_item );
		SendIOInfo( VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_DRM_OK, ptr_item);
	}

	VODS_VOLOGI("-Type. Ret:0x%08x", ret);
#endif
	return ret;
}


VO_U32 voChannelItemThread::ParseData( VO_PBYTE pBuffer , VO_U32 size , VO_BOOL isEnd , VO_ADAPTIVESTREAMPARSER_CHUNK * ptr_item )
{
	VO_U32 ret = 0;

	if( !m_pFileParser )
		return VO_RET_SOURCE2_FORMATUNSUPPORT;

	VO_ADAPTIVESTREAMPARSER_BUFFER buf;
	memset( &buf , 0 , sizeof( VO_ADAPTIVESTREAMPARSER_BUFFER ) );

	buf.bIsEnd = isEnd;
	buf.pBuffer = pBuffer;
	buf.pPrivateData = ptr_item->pPrivateData;
	buf.pStartExtInfo = ptr_item->pStartExtInfo;
	buf.Type = ptr_item->Type;
	buf.ullStartTime = ptr_item->ullStartTime;
	buf.uSize = size;
	buf.ullTimeScale = ptr_item->ullTimeScale;

	ret = m_pFileParser->Process( &buf );

	return ret;
}
VO_S32 voChannelItemThread::OnSample(VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData)
{
	voChannelItemThread * ptr_obj = ( voChannelItemThread * )pUserData;
	VOLOGR("+OnSample. Type:%s, CHUNKType:%s", TrackType2String((VO_SOURCE2_TRACK_TYPE)nOutputType), ChunkType2String(ptr_obj->m_ptr_curitem->Type));

	/*...........if sample time is not v/a, it may send custom Event outside.
	now only HLS has support it. And it is not a true sample ,so skip it.*/
	if( VO_DATASOURCE_RET_SHOULDSKIP == ptr_obj->SendCustomEvent(nOutputType, pData))
		return VO_RET_SOURCE2_OK;

	if(nOutputType == VO_SOURCE2_TT_SEGMENTINDEX)
	{
		((VODS_SEG_INDEX_INFO*)pData)->uStreamID = ptr_obj->m_ptr_curitem->sKeyID.uStreamID;
		((VODS_SEG_INDEX_INFO*)pData)->uTrackID = ptr_obj->m_ptr_curitem->sKeyID.uTrackID;
		ptr_obj->m_ptr_info->pInfo->GetAdaptiveStreamingParserPtr()->SetParam(VO_PID_ADAPTIVESTREAMING_SEGMENTINDEX_INFO, pData);
		return VO_RET_SOURCE2_OK;
	}
	if(nOutputType == VO_SOURCE2_TT_INITDATA)
	{
		((VODS_INITDATA_INFO*)pData)->uStreamID = ptr_obj->m_ptr_curitem->sKeyID.uStreamID;
		((VODS_INITDATA_INFO*)pData)->uTrackID = ptr_obj->m_ptr_curitem->sKeyID.uTrackID;
		ptr_obj->m_ptr_info->pInfo->GetAdaptiveStreamingParserPtr()->SetParam(VO_PID_ADAPTIVESTREAMING_INITDATA_INFO,pData);
		return VO_RET_SOURCE2_OK;
	}

	/* if the sample type which needed conflicts with the actually sample type, skip it and send trackUnavailble.
	eg. video chunk but it is only has audio, vice verse.
	*/
		if( ptr_obj->m_pFileParser->GetMeidaType() == VO_SOURCE2_MT_PUREAUDIO 
			&& ptr_obj->m_ptr_curitem->Type == VO_SOURCE2_ADAPTIVESTREAMING_VIDEO )
		{
			if(!ptr_obj->m_bSwitchAudioThread)
			{
				ptr_obj->m_bSwitchAudioThread = VO_TRUE;
			}
			ptr_obj->TrackUnavailable_II(VO_SOURCE2_TT_VIDEO, 0, 0);
		}

	if( ptr_obj->m_pFileParser->GetMeidaType() == VO_SOURCE2_MT_PUREVIDEO 
		&& ptr_obj->m_ptr_curitem->Type == VO_SOURCE2_ADAPTIVESTREAMING_AUDIO )
		ptr_obj->TrackUnavailable_II(VO_SOURCE2_TT_AUDIO, 0, 0);

	/*.........*/
	if( pData )
	{
		_SAMPLE * pSample = ( _SAMPLE * )pData;
#ifdef _USE_BUFFERCOMMANDER
		pSample->pChunkAgent = ptr_obj->m_pDownloadAgent;
#endif
		pSample->uThreadID = ptr_obj->m_nThreadID;
		if( pSample->uMarkFlag != VO_DATASOURCE_MARKFLAG_RESET )
		{
			//as terry's hls spec, that if m3u8 has a independent audio track then skip the auido track in ts if it has. 
			//hls parser will be set the chunk type is VO_SOURCE2_ADAPTIVESTREAMING_VIDEO not VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO
			//so the audio sample should be filter at first.

			if( nOutputType == VO_SOURCE2_TT_AUDIO && ptr_obj->m_ptr_curitem->Type == VO_SOURCE2_ADAPTIVESTREAMING_VIDEO )
				return VO_RET_SOURCE2_OK;

			if( nOutputType == VO_SOURCE2_TT_VIDEO && ptr_obj->m_ptr_curitem->Type == VO_SOURCE2_ADAPTIVESTREAMING_AUDIO )
				return VO_RET_SOURCE2_OK;
		}

	}
	if( VO_DATASOURCE_RET_SHOULDSKIP == ptr_obj->MarkOPFilter(nOutputType, pData))
		return VO_RET_SOURCE2_OK;


	if( ptr_obj->m_ptr_curitem 
		&& ( ptr_obj->m_ptr_curitem->uFlag & VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE )
		&& ptr_obj->m_video_count == 0 
		&& ptr_obj->m_audio_count == 0 )
	{
		switch( ptr_obj->m_pFileParser->GetMeidaType() )
		{
		case VO_SOURCE2_MT_PUREVIDEO:
			{
				VOLOGR("VO_SOURCE2_MT_PUREVIDEO");
				ptr_obj->m_lastTrackType = VO_SOURCE2_TT_VIDEO;
			}
			break;
		case VO_SOURCE2_MT_PUREAUDIO:
			{
				VOLOGR("VO_SOURCE2_MT_PUREAUDIO");
				ptr_obj->m_lastTrackType = VO_SOURCE2_TT_AUDIO;
			}
			break;
		case VO_SOURCE2_MT_AUDIOVIDEO:
			{
				VOLOGR("VO_SOURCE2_MT_AUDIOVIDEO");
				ptr_obj->m_lastTrackType = VO_SOURCE2_TT_MAX;
			}
			break;
		}
	}

	/*.........one case that chunk is AUDIOVIDEO, but it plays only video, and not send formatchange. 
	So now check CHUNKTYPE to decide lastTrackType.
	...........*/
	if( ptr_obj->m_ptr_curitem->Type == VO_SOURCE2_ADAPTIVESTREAMING_VIDEO 
		&&  ptr_obj->m_pFileParser->GetMeidaType() == VO_SOURCE2_MT_AUDIOVIDEO)
		ptr_obj->m_lastTrackType = VO_SOURCE2_TT_VIDEO;

	else if( ptr_obj->m_ptr_curitem->Type == VO_SOURCE2_ADAPTIVESTREAMING_AUDIO 
		&&  ptr_obj->m_pFileParser->GetMeidaType() == VO_SOURCE2_MT_AUDIOVIDEO)
		ptr_obj->m_lastTrackType = VO_SOURCE2_TT_AUDIO;
	/*....................*/

	/*.........HeadData will be sent to OnSample to decide the lastTrackType.
	But it shall be skipped because it is not a real sample*/
	if( ptr_obj->m_ptr_curitem->Type == VO_SOURCE2_ADAPTIVESTREAMING_HEADDATA || 
		ptr_obj->m_ptr_curitem->Type == VO_SOURCE2_ADAPTIVESTREAMING_SEGMENTINDEX ||
		ptr_obj->m_ptr_curitem->Type == VO_SOURCE2_ADAPTIVESTREAMING_INITDATA)
	{
		ptr_obj->m_video_count = ptr_obj->m_audio_count = 1;
		return VO_RET_SOURCE2_OK;
	}
	/*.............*/

	VO_BOOL need_for_ref_timestamp = VO_FALSE;

	_SAMPLE * pSample = ( _SAMPLE * )pData;

	VO_U32 uFlag_sample = pSample->uFlag;
#ifdef _USE_BUFFERCOMMANDER
	VO_U64 uFlag_sample_DS = pSample->uDataSource_Flag;
#endif
	if( ptr_obj->m_pFileParser->GetMeidaType() == VO_SOURCE2_MT_PUREVIDEO )
	{
		if( nOutputType == VO_SOURCE2_TT_VIDEO )
		{
			need_for_ref_timestamp = VO_TRUE;
			//ptr_obj->m_ref_timestamp = pSample->uTime;
		}
	}
	else
	{
		if( ptr_obj->m_ptr_curitem->Type == VO_SOURCE2_ADAPTIVESTREAMING_VIDEO && ptr_obj->m_pFileParser->GetMeidaType() == VO_SOURCE2_MT_AUDIOVIDEO && nOutputType == VO_SOURCE2_TT_VIDEO )
		{
			need_for_ref_timestamp = VO_TRUE;
		}
		else
		{
			if( nOutputType == VO_SOURCE2_TT_AUDIO )
			{
				need_for_ref_timestamp = VO_TRUE;
				//ptr_obj->m_ref_timestamp = pSample->uTime;
			}
		}
	}

	if( ( ptr_obj->m_ptr_curitem->uFlag & VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_SMOOTH_ADAPTION || 
		ptr_obj->m_ptr_curitem->uFlag & VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_SMOOTH_ADAPTIONEX) 
		&& ptr_obj->m_video_count == 0 && ptr_obj->m_audio_count == 0)
	{
		switch( ptr_obj->m_pFileParser->GetMeidaType() )
		{
		case VO_SOURCE2_MT_PUREVIDEO:
			ptr_obj->m_video_status = PARSEITEMSTATUS_BASTART;
			break;
		case VO_SOURCE2_MT_PUREAUDIO:
			ptr_obj->m_audio_status = PARSEITEMSTATUS_BASTART;
			break;
		case VO_SOURCE2_MT_AUDIOVIDEO:
			{
				ptr_obj->m_video_status = PARSEITEMSTATUS_BASTART;
				ptr_obj->m_audio_status = PARSEITEMSTATUS_BASTART;
			}
			break;
		}
	}

	/*for track unavalible*/
	VO_SOURCE2_TRACK_TYPE unavalibleType = VO_SOURCE2_TT_MAX;
	VO_U32 uFlag_unavalible = 0;
	if(pSample->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT
		&& ptr_obj->m_ptr_curitem->Type == VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO 
		&& ptr_obj->m_video_count == 0 && ptr_obj->m_audio_count == 0 )
	{
		VO_SOURCE2_MEDIATYPE mediatype = ptr_obj->m_pFileParser->GetMeidaType();

		switch( mediatype )
		{
		case VO_SOURCE2_MT_PUREAUDIO:
			{
				if( ptr_obj->m_video_status == PARSEITEMSTATUS_BAEND )
				{
					VODS_VOLOGI( "Video BA End!" );
					uFlag_unavalible |= VO_SOURCE2_FLAG_SAMPLE_BAEND;
					ptr_obj->m_video_status = (ParseItemStatus)(ptr_obj->m_video_status + 1);
				}

				VODS_VOLOGI("Video InAvailiable!");
				unavalibleType = VO_SOURCE2_TT_VIDEO;
			}
			break;
		case VO_SOURCE2_MT_PUREVIDEO:
			{
				if( ptr_obj->m_audio_status == PARSEITEMSTATUS_BAEND )
				{
					VODS_VOLOGI( "Audio BA End!" );
					uFlag_unavalible |= VO_SOURCE2_FLAG_SAMPLE_BAEND;
					ptr_obj->m_audio_status = (ParseItemStatus)(ptr_obj->m_audio_status + 1);
				}

				VODS_VOLOGI("Audio InAvailiable!");
				unavalibleType = VO_SOURCE2_TT_AUDIO;
			}
			break;
		}
	}
	/*end of for track unavalible*/

	VO_U32 *pCount = NULL;
	ParseItemStatus *pStatus = NULL;
	VO_BOOL *pChunkDropped = NULL;
	switch( nOutputType )
	{
	case VO_SOURCE2_TT_VIDEO:
		{
			pCount = &ptr_obj->m_video_count;
			pStatus = &ptr_obj->m_video_status;
			pChunkDropped = &ptr_obj->m_bVideoChunkDropped;
		}
		break;
	case VO_SOURCE2_TT_AUDIO:
		{
			pCount = &ptr_obj->m_audio_count;
			pStatus = &ptr_obj->m_audio_status;
			pChunkDropped = &ptr_obj->m_bAudioChunkDropped;
		}
		break;
	case VO_SOURCE2_TT_SUBTITLE:
		{
			pCount = &ptr_obj->m_subtitle_count;
			pStatus = NULL;
			pChunkDropped = &ptr_obj->m_bSubtitleChunkDropped;
		}
		break;
	}//switch( nOutputType )


	if( pCount )
	{
		if (*pCount == 0 && pStatus)
		{
			if( *pStatus == PARSEITEMSTATUS_BASTART )
			{
				VODS_VOLOGI( " %s BA Start!", TrackType2String((VO_SOURCE2_TRACK_TYPE ) nOutputType ) );
				uFlag_sample |= VO_SOURCE2_FLAG_SAMPLE_BASTART;
				*pStatus = (ParseItemStatus)( *pStatus + 1);
			}
			else if( *pStatus == PARSEITEMSTATUS_BAEND )
			{
				VODS_VOLOGI( "%s BA End!",TrackType2String((VO_SOURCE2_TRACK_TYPE ) nOutputType ) );
				uFlag_sample |= VO_SOURCE2_FLAG_SAMPLE_BAEND;
				*pStatus = (ParseItemStatus)( *pStatus + 1);
			}
		}
		(*pCount) += 1;	
		VOLOGR( "Got %s(No.%d) FPTrackID:%d, ASTrackID:%d TimeStamp: %lld,Size:%d,(RootURL:%s,%s)" 
			, TrackType2String((VO_SOURCE2_TRACK_TYPE ) nOutputType ), *pCount, pSample->uFPTrackID, pSample->uASTrackID, pSample->uTime, pSample->uSize, ptr_obj->m_ptr_curitem->szRootUrl,ptr_obj->m_ptr_curitem->szUrl );
		SpecialFlagCheck((VO_SOURCE2_TRACK_TYPE )nOutputType, uFlag_sample, pSample->uTime, VO_RET_SOURCE2_OK);

	}
	if(ptr_obj->m_bLastAudioBAFlag && nOutputType == VO_SOURCE2_TT_AUDIO)
	{
		uFlag_sample |= VO_SOURCE2_FLAG_SAMPLE_BASTART;
		ptr_obj->m_bLastAudioBAFlag = VO_FALSE;
	}
	if(ptr_obj->m_bLastVideoBAFlag && nOutputType == VO_SOURCE2_TT_VIDEO)
	{
		uFlag_sample |= VO_SOURCE2_FLAG_SAMPLE_BASTART;
		ptr_obj->m_bLastVideoBAFlag = VO_FALSE;
	}
	if( pChunkDropped && *pChunkDropped )
	{
		uFlag_unavalible |= VO_SOURCE2_FLAG_SAMPLE_CHUNKDROPPED;
		uFlag_sample |= VO_SOURCE2_FLAG_SAMPLE_CHUNKDROPPED;

		*pChunkDropped = VO_FALSE;

	}

	if( ptr_obj->m_bChunkBegin)
	{
#ifdef _USE_BUFFERCOMMANDER
		uFlag_sample_DS |= VO_DATASOURCE_FLAG_CHUNK_BEGIN;
		pSample->uChunkID = ptr_obj->m_uChunkID;
#else
		ptr_obj->InfoChunk( pSample->uTime, VO_TRUE );
		uFlag_sample |= VO_DATASOURCE_FLAG_CHUNK_BEGIN;
		pSample->uChunkID = ptr_obj->m_uChunkID;
#endif
	}

	if(ptr_obj->m_bChunkForceOutput)
#ifdef _USE_BUFFERCOMMANDER
		uFlag_sample_DS |= VO_DATASOURCE_FLAG_FORCEOUTPUT;
#else
		uFlag_sample |= VO_DATASOURCE_FLAG_FORCEOUTPUT;
#endif
	if( unavalibleType != VO_SOURCE2_TT_MAX )
	{
		ptr_obj->TrackUnavailable_II(unavalibleType, uFlag_unavalible, pSample->uTime );
	}
	
	pSample->uFlag |= uFlag_sample;
#ifdef _USE_BUFFERCOMMANDER
	if(ptr_obj->m_bChunkDiscontinue)
	{
		VOLOGI("mark discontinue chunk first sample");
		uFlag_sample_DS |= VO_DATASOURCE_FLAG_DISCONTINUE;
		ptr_obj->m_bChunkDiscontinue = VO_FALSE;
	}
	pSample->uDataSource_Flag |= uFlag_sample_DS;
	if(ptr_obj->m_ptr_curitem->uFlag & VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_SMOOTH_ADAPTIONEX)
		pSample->uDataSource_Flag |= VO_DATASOURCE_FLAG_CHUNK_SWITCH;
	VOLOGR("pSample->uFlag :%08x, pSample->uDataSource_Flag:08x", pSample->uFlag, pSample->uDataSource_Flag);
#else
	if(ptr_obj->m_ptr_curitem->uFlag & VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_SMOOTH_ADAPTIONEX)
		pSample->uFlag |= VO_DATASOURCE_FLAG_CHUNK_SWITCH;
	VOLOGR("pSample->uFlag :%08x", pSample->uFlag);
#endif
	if(ptr_obj->m_bChunkBegin)
	{
		//send this sample to ads manager every chunk begin
		_SAMPLE * pSample = ( _SAMPLE * )pData;
		ptr_obj->SendChunkSample(ptr_obj->m_ptr_curitem,pSample);
		ptr_obj->m_bChunkBegin = VO_FALSE;
	}

	VO_S32 ret = ptr_obj->m_ptr_info->pInfo->GetSampleCallback()->SendData( ptr_obj->m_ptr_info->pInfo->GetSampleCallback()->pUserData , nOutputType , pData );
	if(ret == VO_RET_SOURCE2_TRACKNOTFOUND)
	{
		_SAMPLE *pSample = (_SAMPLE*)pData;
		if(pSample->uFlag & VO_SOURCE2_FLAG_SAMPLE_BASTART)
		{
			if(nOutputType == VO_SOURCE2_TT_AUDIO)
				ptr_obj->m_bLastAudioBAFlag = VO_TRUE;
			else if(nOutputType = VO_SOURCE2_TT_VIDEO)
				ptr_obj->m_bLastVideoBAFlag = VO_TRUE;
		}
	}
	if( need_for_ref_timestamp && !( ptr_obj->m_ptr_curitem->uFlag & VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_SMOOTH_ADAPTION ) )
	{
		_SAMPLE * pSample = ( _SAMPLE * )pData;
		ptr_obj->m_ref_timestamp = pSample->uTime;
	}

	return ret;
}

VO_U32 voChannelItemThread::BeginChunkDRM()
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	VO_StreamingDRM_API *pDRM = m_ptr_info->pInfo->GetDRMPtr( m_ptr_info->type );
	if( pDRM && pDRM->hHandle )
	{
		ret = pDRM->DataBegin( pDRM->hHandle , m_nThreadID ,m_ptr_curitem->pChunkDRMInfo );
	}
	if(ret == VO_ERR_DRM2_NO_DRM_API )
		ret = VO_RET_SOURCE2_OK;
	return ret;
}

VO_U32 voChannelItemThread::ProcessChunkDRM_II( VO_PBYTE ptr_buffer , VO_U32 uOffset,VO_U32 * ptr_size, VO_BOOL bChunkEnd )
{

	VO_U32 ret = VO_RET_SOURCE2_OK;

	VO_StreamingDRM_API *pDRM = m_ptr_info->pInfo->GetDRMPtr( m_ptr_info->type );

	if( pDRM && pDRM->hHandle )
	{
		ret = pDRM->DataProcess_Chunk( pDRM->hHandle , m_nThreadID ,uOffset, ptr_buffer , ptr_size?*ptr_size:0 , 0 , ptr_size ,bChunkEnd, m_ptr_curitem->pChunkDRMInfo );
	}
	if(ret == VO_ERR_DRM2_NO_DRM_API )
		ret = VO_RET_SOURCE2_OK;

	return ret;
}
VO_U32 voChannelItemThread::EndChunkDRM_II()
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	VO_StreamingDRM_API *pDRM = m_ptr_info->pInfo->GetDRMPtr( m_ptr_info->type );

	if( pDRM && pDRM->hHandle )
	{
		return pDRM->DataEnd( pDRM->hHandle , m_nThreadID, m_ptr_curitem->pChunkDRMInfo );
	}
	if(ret == VO_ERR_DRM2_NO_DRM_API)
		ret = VO_RET_SOURCE2_OK;
	return ret;
}
VO_U32 voChannelItemThread::ProcessSampleDRM(VO_PTR pUseData, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE *ppDesData, VO_U32 *pnDesSize, VO_DRM2_DATATYPE eDataType, VO_DRM2_DATAINFO_TYPE eDataInfoType,VO_PTR pAdditionalInfo)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	voChannelItemThread *pItemThread = (voChannelItemThread*)pUseData;

	VO_StreamingDRM_API *pDRM = pItemThread->m_ptr_info->pInfo->GetDRMPtr( pItemThread->m_ptr_info->type );

	if( pDRM && pDRM->hHandle )
	{
		switch(eDataType)
		{
			case VO_DRM2DATATYPE_MEDIASAMPLE:
			case VO_DRM2DATATYPE_SEQUENCEHEADER:
				return pDRM->DataProcess_FR( pDRM->hHandle , pSrcData, nSrcSize, ppDesData, pnDesSize, eDataType,eDataInfoType,pAdditionalInfo );
			case VO_DRM2DATATYPE_CENC_PSSH:
			{
				ret = pDRM->Info_FR(pDRM->hHandle, pAdditionalInfo, VO_DRM2_INFO_PROCECTION, NULL);
				if(ret != VO_RET_SOURCE2_OK)
					pItemThread->SendDrmError(VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_STREAMING_DRMLICENSEERROR, ret);
				return ret;
			}
			case VO_DRM2DATATYPE_CENC_TENC:
			{
				ret = pDRM->Info_FR(pDRM->hHandle, pAdditionalInfo, VO_DRM2_INFO_TRACKDECYPTION, NULL);
				if(ret != VO_RET_SOURCE2_OK)
					pItemThread->SendDrmError(VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_STREAMING_DRMLICENSEERROR, ret);
				return ret;
			}
			default:
				return pDRM->DataProcess_FR( pDRM->hHandle , pSrcData, nSrcSize, ppDesData, pnDesSize, eDataType,eDataInfoType,pAdditionalInfo );
		}

	}
	if(ret == VO_ERR_DRM2_NO_DRM_API)
		ret = VO_RET_SOURCE2_OK;
	return ret;
}
VO_U32 voChannelItemThread::EndChunkDRM( VO_PBYTE ptr_buffer , VO_U32 * ptr_size )
{
	VO_StreamingDRM_API *pDRM = m_ptr_info->pInfo->GetDRMPtr( m_ptr_info->type );

	if( pDRM && pDRM->hHandle )
	{
		return pDRM->DataEnd( pDRM->hHandle , m_nThreadID, m_ptr_curitem->pChunkDRMInfo );
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 voChannelItemThread::CreateFileParser_II( VO_ADAPTIVESTREAMPARSER_CHUNK * ptr_item , VO_PBYTE *ppBuffer , VO_U32 *pSize, VO_BOOL bStreaming )
{
	VO_U32 ret = 0;

	if( ptr_item->uFlag & VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE )
	{
		VODS_VOLOGI("%s meets VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE. ", ChunkType2String( ptr_item->Type ) );
		FileFormat ff = FileFormat_Unknown;
		FileFormat oldff = FileFormat_Unknown;

		if( m_pFileParser )
			oldff = m_pFileParser->GetFileFormat();

		/* VO_ADAPTIVESTREAMPARSER_STARTEX_FLAG_TIMESTAMPERECALCULATE has been insteaded by VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_TIMESTAMPRECALCULATE, so modify it.*/
		//bool needforcereset = ptr_item->pStartExtInfo && ptr_item->pStartExtInfo->uFlag == VO_ADAPTIVESTREAMPARSER_STARTEX_FLAG_TIMESTAMPERECALCULATE;
		bool needforcereset = VO_TRUE;//ptr_item->uFlag & VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_PROGRAMCHANGE;

		VO_FILE_FORMAT fFormat = VO_FILE_FFUNKNOWN;
		if(m_ptr_info->pInfo->GetStreamingType() != VO_ADAPTIVESTREAMPARSER_STREAMTYPE_ISS)
		{
			ret = m_FormatChecker.FormatCheckStart(ppBuffer, pSize, &fFormat);
		}
		//Streaming is true. like hls, streaming data input
		if( bStreaming == VO_TRUE )
		{
			if( ret == VO_RET_SOURCE2_INPUTDATASMALL  )
			{
				return VO_RET_SOURCE2_INPUTDATASMALL;
			}
			else if( ret == VO_RET_SOURCE2_FORMATUNSUPPORT )
			{
				return VO_RET_SOURCE2_FORMATUNSUPPORT;
			}
		}

		if(fFormat == VO_FILE_FFUNKNOWN && ptr_item->Type == VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE )
			fFormat = VO_FILE_FFSUBTITLE_TTML;

		ret = CreateAdaptiveStreamingFileParser( &m_pFileParser , m_ptr_info->pInfo->GetStreamingType() , fFormat, &m_sample_callback 
			, m_ptr_info->pInfo->GetLibOp() , m_ptr_info->pInfo->GetWorkPath(), needforcereset );
		if( ret != VO_RET_SOURCE2_OK )
		{
			VODS_VOLOGW( "Create File Parser Fail! %d" , ret );
			return ret;
		}

		VO_StreamingDRM_API *pDrmApi = m_ptr_info->pInfo->GetDRMPtr(m_ptr_info->type);

		m_dsdrmcb.pUserData = this;
		m_dsdrmcb.FRCB = ProcessSampleDRM;
		if(m_pFileParser->SetParam(VO_DATASOURCE_PID_DRM_FP_CB, &m_dsdrmcb)  != VO_RET_SOURCE2_OK)
		{
			VODS_VOLOGW( "Set DRM pointer to FileParser Fail! %d" , ret );
		}

		m_pFileParser->SetParam(VO_PID_COMMON_LOGFUNC, m_ptr_info->pInfo->GetLogFunc() );

		ff = m_pFileParser->GetFileFormat();
		m_pFileParser->SetASTrackID( ptr_item->sKeyID.uTrackID );
		if( ff != oldff || needforcereset )
		{

			if(ff == FileFormat_AAC || ff == FileFormat_MP3)
				m_pFileParser->SetTimeStampOffset(_VODS_INT64_MAX);

			if( needforcereset )
			{
				VODS_VOLOGI("meet ProgramReset. %s", ptr_item->szUrl);
				m_pFileParser->SetTrackReset(  );
			}
		}

		GetHeadData( ptr_item->Type);
	}

	return ret;
}

VO_VOID voChannelItemThread::Start( THREADINFO * ptr_info )
{
	switch( ptr_info->type )
	{
	case THREADTYPE_MEDIA_AUDIO:
		m_chunkType = VO_SOURCE2_ADAPTIVESTREAMING_AUDIO;
		m_trackType = VO_SOURCE2_TT_AUDIO;
		break;
	case THREADTYPE_MEDIA_VIDEO:
		m_chunkType = VO_SOURCE2_ADAPTIVESTREAMING_VIDEO;
		m_trackType = VO_SOURCE2_TT_VIDEO;
		break; 
	case THREADTYPE_MEDIA_SUBTITLE:
		m_chunkType = VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE;
		m_trackType = VO_SOURCE2_TT_SUBTITLE;
		break; 
	default:
		m_chunkType = VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE_MAX;
		m_trackType = VO_SOURCE2_TT_MAX;
		break;
	}

	voItemThread::Start( ptr_info );
}

VO_VOID voChannelItemThread::TrackUnavailable_II(VO_SOURCE2_TRACK_TYPE tracktype, VO_U32 nFlag, VO_U64 uTimeStamp )
{

	VODS_VOLOGI(" +TrackUnavailable_II");

	_SAMPLE sample;
	memset( &sample , 0 , sizeof(_SAMPLE) );
	sample.uTime = uTimeStamp;
	sample.uFlag = VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE | nFlag;
#ifdef _USE_BUFFERCOMMANDER
	sample.uDataSource_Flag = VO_DATASOURCE_FLAG_CHUNK_TRACKTYPE_UNAVAILABLE;
	sample.uFPTrackID = _VODS_INT32_MAX;
	sample.uASTrackID = _VODS_INT32_MAX;
	sample.pChunkAgent = m_pDownloadAgent;
#endif
	/* notify CheckAndWait not to wait*/
	m_ptr_info->pInfo->GetTimeStampSyncObject().CheckAndWait( tracktype , &sample , &m_is_stop );

	m_ptr_info->pInfo->GetSampleCallback()->SendData( m_ptr_info->pInfo->GetSampleCallback()->pUserData , tracktype , &sample );
	VODS_VOLOGI(" -TrackUnavailable_II");

}
VO_VOID voChannelItemThread::TrackUnavailable( VO_U32 uTimes )
{
	if( m_ptr_info->type == THREADTYPE_MEDIA_SUBTITLE)
		return ;

	VODS_VOLOGI(" +TrackUnavailable");

	_SAMPLE sample;
	memset( &sample , 0 , sizeof(_SAMPLE) );
	sample.uTime = m_ref_timestamp +1;

	sample.uFlag = VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE;
#ifdef _USE_BUFFERCOMMANDER
	sample.uDataSource_Flag = VO_DATASOURCE_FLAG_CHUNK_TRACKTYPE_UNAVAILABLE;
	sample.uFPTrackID = _VODS_INT32_MAX;
	sample.uASTrackID = _VODS_INT32_MAX;
	sample.pChunkAgent = m_pDownloadAgent;
	sample.pReserve1 = (VO_VOID*)uTimes;
#endif
	VO_U32 tracktype;

	if( m_lastTrackType == VO_SOURCE2_TT_MAX )
	{
		switch( m_ptr_info->type )
		{
		case THREADTYPE_MEDIA_AUDIO:
			tracktype = VO_SOURCE2_TT_AUDIO;
			break;
		case THREADTYPE_MEDIA_VIDEO:
			tracktype = VO_SOURCE2_TT_VIDEO;
			break;
		default:
			return;
		}
	}
	else
		tracktype = m_lastTrackType;

	/* notify CheckAndWait not to wait*/
	m_ptr_info->pInfo->GetTimeStampSyncObject().CheckAndWait( tracktype , &sample , &m_is_stop );

	m_ptr_info->pInfo->GetSampleCallback()->SendData( m_ptr_info->pInfo->GetSampleCallback()->pUserData , tracktype , &sample );
	VODS_VOLOGI(" -TrackUnavailable");

}
VO_U32 voChannelItemThread::SendEventDownloadStartColletor(VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk)
{
	CHUNKINFO info;
	memset( &info , 0 , sizeof( CHUNKINFO ) );
	info.chunktype = pChunk->Type;
	VO_SOURCE2_EVENTCALLBACK *pEventCallback = m_ptr_info->pInfo->GetEventCallback();
	return pEventCallback->SendEvent( pEventCallback->pUserData , VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_DOWNLOADSTARTCOLLECTOR , (VO_U32)&info , pChunk->uFlag);
}
VO_U32 voChannelItemThread::SendEventDownloadEndColletor(VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk, VO_U64 &uDownloadSize)
{
	CHUNKINFO info;
	memset( &info , 0 , sizeof( CHUNKINFO ) );
	info.chunktype = pChunk->Type;
	VO_SOURCE2_EVENTCALLBACK *pEventCallback = m_ptr_info->pInfo->GetEventCallback();
	pEventCallback->SendEvent( pEventCallback->pUserData , VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_DOWNLOADENDCOLLECTOR , (VO_U32)&info , (VO_U32)&uDownloadSize);
	VOLOGI("uDownloadSize = %lld", uDownloadSize);
	return VO_RET_SOURCE2_OK;
}
	VO_U32 voChannelItemThread::ToProcessChunk(VO_HANDLE hIO, VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk, VO_U32 *readsize)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	VO_PBYTE pTmpBuffer = NULL;
	VO_U32 nSize = 0;
	VO_U32 uOffset = 0;
	VO_U64 uDownloadSize = 0;
	VO_BOOL	min_bitrate_playback = m_ptr_info->pInfo->IsMinBitratePlaying();
	VO_BOOL ba_enable = m_ptr_info->pInfo->IsBAEnable();
	VOLOGI("min bitrate playback is %d,ba enable is %d",min_bitrate_playback,ba_enable);

	VOLOGI("Before BeginChunkDRM");
	ret = BeginChunkDRM();
	VOLOGI("After BeginChunkDRM");
	if( ret == VO_RET_SOURCE2_OK)
	{
		VO_U64 size = 0;
		VO_SOURCE2_IO_API *pIOApi = m_ptr_info->pInfo->GetIOPtr();
		VOLOGI("Before IO GetSize");
		while(true)
		{
			ret = pIOApi->GetSize( hIO, &size );
			//	VOLOGI("Getsize return 0x%08x, size is %d",ret,(int)size);
			if(VO_SOURCE2_IO_FAIL == ret)
				break;
			if(VO_SOURCE2_IO_OK == ret || m_is_stop)
				break;
			voOS_Sleep(20);
		}
		VOLOGI("After IO GetSize,size is %lld",size);
		if(VO_SOURCE2_IO_OK == ret)
		{
			voSmartPointor< VO_CHAR > buffer( new VO_CHAR[(VO_U32)(size+1)] );
			memset( buffer , 0 , ( size + 1 ) * sizeof( VO_CHAR ) );
			nSize = (VO_U32)size;
			pTmpBuffer = (VO_PBYTE) buffer;
			uDownloadSize = 0;
			SendEventDownloadStartColletor(pChunk);

			VOLOGI("Before ReadFixedSize");
			if(ba_enable && !min_bitrate_playback && (pChunk->Type == VO_SOURCE2_ADAPTIVESTREAMING_VIDEO || pChunk->Type == VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO)) 
			{
				ret = ReadFixedSize(m_ptr_info->pInfo->GetDownloadDataColletor(), pIOApi, hIO , pTmpBuffer , &nSize, &m_is_stop,( pChunk->ullDuration / ( pChunk->ullTimeScale / 1000. ) + 2000 ), m_ptr_info->pInfo->GetIOEventCallback());	
			}
			else
			{
				ret = ReadFixedSize(m_ptr_info->pInfo->GetDownloadDataColletor(), pIOApi, hIO , pTmpBuffer , &nSize , &m_is_stop);
			}
			VOLOGI("After ReadFixedSize,size is %d,ret is 0x%08x",nSize,ret);
			SendEventDownloadEndColletor(pChunk, uDownloadSize);
			if(uDownloadSize == 0)
				uDownloadSize = nSize;

			if( ret == VO_SOURCE2_IO_OK )
			{
				VO_U32 bufsize = nSize;
				VOLOGI("Before first ProcessChunkDRM_II");
				ret = ProcessChunkDRM_II( pTmpBuffer, uOffset, &bufsize, VO_FALSE );
				VOLOGI("After first ProcessChunkDRM_II");
				if(ret == VO_RET_SOURCE2_OK)
				{
					uOffset += bufsize;
					nSize = bufsize;
					bufsize = 0;
					VOLOGI("Before second ProcessChunkDRM_II");
					ret = ProcessChunkDRM_II( pTmpBuffer + bufsize ,uOffset, &bufsize, VO_TRUE );
					VOLOGI("After second ProcessChunkDRM_II");
					if( ret == VO_RET_SOURCE2_OK )
					{
						nSize = nSize + bufsize;
						VOLOGI("Before CreateFileParser_II");
						ret = CreateFileParser_II( pChunk , &pTmpBuffer , &nSize, VO_FALSE );
						VOLOGI("After CreateFileParser_II");
						//ret = CreateFileParser( pChunk , pTmpBuffer , nSize );
						if( ret == VO_RET_SOURCE2_OK )
						{
							VOLOGI("Before ParseData");
							ret = ParseData( pTmpBuffer, nSize , VO_TRUE , pChunk );
							VOLOGI("After ParseData");
						}
						else if( ret != VO_RET_SOURCE2_OK )
						{
							VODS_VOLOGW("-ParseData Fail!" );
							if((ret & VO_ERR_LICENSE_ERROR) == VO_ERR_LICENSE_ERROR)
							{
								VOLOGE("CreateFileParser license check error,ret = 0x%08x",ret);
								SendLicenseCheckError(VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_STREAMING_VOLIBLICENSEERROR, ret);
							}
							ret = VO_RET_SOURCE2_FORMATUNSUPPORT;
						}
					}
					else
					{
						VODS_VOLOGW("-ProcessChunkDrm_II failed!ret is 0x%x",ret);
						ret = VO_RET_SOURCE2_DRMERROR;
					//	SendDrmWarning(VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_DRMERROR,pChunk,ret);
					}
					
				}
				else
				{
					VODS_VOLOGW("-ProcessChunkDrm_II failed!ret is 0x%x",ret);
					ret = VO_RET_SOURCE2_DRMERROR;
					//SendDrmWarning(VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_DRMERROR,pChunk,ret);
				}
			}
			else
			{
				if(m_is_stop)
				{
					ret = VO_RET_SOURCE2_OK;	//it is user abort,so set return code to VO_RET_SOURCE2_OK
				}
				else
				{
					if(ret == VO_DATASOURCE_RET_DOWNLOAD_SLOW)
					{
						VODS_VOLOGW("Download speed is slow,so abort it");
					}
					else
					{
						VODS_VOLOGW("-Download Fail!");
						ret = VO_RET_SOURCE2_CHUNKDROPPED;
					}
				}
			}
		}
		else
		{
			VODS_VOLOGW("-GetSize Fail!");
		}
	}
	else
	{
		VODS_VOLOGW( "-Start DRM Fail! "  );
		ret = VO_RET_SOURCE2_DRMERROR;
		//SendDrmWarning(VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_DRMERROR,pChunk,ret);
	}

	EndChunkDRM_II();
	*readsize = uDownloadSize;
//	*readsize = nSize;
	return ret;	

}

VO_U32 voChannelItemThread::ThreadEnding()
{
	VODS_VOLOGI(" +ThreadEnding");
	if( m_lastTrackType != VO_SOURCE2_TT_MAX )
	{
		TrackUnavailable();
	}
	/*disable a/v sync operation*/
	if( m_ptr_info->type != THREADTYPE_MEDIA_SUBTITLE )
	{
		m_ptr_info->pInfo->GetTimeStampSyncObject().Disable();
	}
	m_ptr_info->pInfo->GetEventCallback()->SendEvent( m_ptr_info->pInfo->GetEventCallback()->pUserData , VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CHUNKEOS , (VO_U32)&m_ptr_info->type , 0 );
	VODS_VOLOGI(" -ThreadEnding");

	return VO_RET_SOURCE2_OK;
}
/* .........ToDealItem_II instead of
VO_U32 voChannelItemThread::ToDealItem(VO_ADAPTIVESTREAMPARSER_CHUNK *pChunk, VO_BOOL *pReset)
{
VO_U32 ret = VO_RET_SOURCE2_OK;

VOLOGR( "url:%s", pChunk->szUrl );
VO_U64 utc = voOS_GetSysTime() - m_ptr_info->pInfo->GetStartSysTime() + m_ptr_info->pInfo->GetStartUTC();

if( utc > pChunk->ullChunkDeadTime )
{
VODS_VOLOGW( "One Time Out! %s Chunk Dead Time: %lld Current Time: %lld" , pChunk->szUrl , pChunk->ullChunkDeadTime , utc );
*pReset = VO_TRUE;
return VO_RET_SOURCE2_OK;
}

m_audio_count = m_video_count = 0;

m_ptr_curitem = pChunk;

if( *pReset )//&& m_ptr_info->pInfo->GetStreamingType() == VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS )
{
*pReset = VO_FALSE;
pChunk->uFlag = pChunk->uFlag | VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
}

VO_U32 nRetry = 0;
while(!m_is_stop && (ret = ToGetItem( pChunk )) == VO_RET_SOURCE2_NEEDRETRY )
{
nRetry ++;
VODS_VOLOGW("ErrorHandler Need Retry:%d", nRetry );
voOS_Sleep(1);
}
if( ret != VO_RET_SOURCE2_OK )
*pReset = VO_TRUE;

return ret;
}
...........*/

VO_U32 voChannelItemThread::ToDealItem_II(VO_ADAPTIVESTREAMPARSER_CHUNK *pChunk, VO_BOOL *pReset)
{
	VOLOGR(" + ToDealItem_II");
	VO_U32 ret = VO_RET_SOURCE2_OK;

	VO_U64 utc = voOS_GetSysTime() - m_ptr_info->pInfo->GetStartSysTime() + m_ptr_info->pInfo->GetStartUTC();

	VO_CHAR time1[255] = {0};
	VO_CHAR time2[255] = {0};
	VO_CHAR time3[255] = {0};
	if( pChunk->ullChunkDeadTime != _VODS_INT64_MAX)
		timeToSting( pChunk->ullChunkDeadTime, time1);
	if( pChunk->ullChunkLiveTime != _VODS_INT64_MAX)
		timeToSting( pChunk->ullChunkLiveTime, time3);
	timeToSting( utc, time2);
	VODS_VOLOGI( "New %s Chunk:%s, (ASTrackID:%d, Flag:0x%08x, Live Time:%s(%lld), Dead Time: %s(%lld),StartTime:%lld) Current Time: %s, PreviousStartTime:%lld " 
		,ChunkType2String(pChunk->Type), pChunk->szUrl,  pChunk->sKeyID.uTrackID, pChunk->uFlag, time3,pChunk->ullChunkLiveTime, time1, pChunk->ullChunkDeadTime,pChunk->ullStartTime, time2, m_LastStartTime  );

	if(pChunk->Type != VO_SOURCE2_ADAPTIVESTREAMING_HEADDATA && 
		pChunk->Type != VO_SOURCE2_ADAPTIVESTREAMING_SEGMENTINDEX &&
		pChunk->Type != VO_SOURCE2_ADAPTIVESTREAMING_INITDATA)
	{
		if(m_ptr_info->type == THREADTYPE_MEDIA_VIDEO)
		{
			if(m_ptr_info->pInfo->IsBitrateChanging())
			{
				pChunk->uFlag |= VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_SMOOTH_ADAPTIONEX;
				VOLOGI("BA happen, add BA flag to chunk uFlag");
			}
		}
		if(m_ptr_info->pInfo->GetStreamingType() != VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS 
			&& m_LastStartTime >= pChunk->ullStartTime && m_LastStartTime != _VODS_INT64_MAX
			&& !(pChunk->uFlag & VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_SMOOTH_ADAPTIONEX))
		{
			VODS_VOLOGI( "Duplicated!Type[%d]. %lld %lld" , m_ptr_info->type, m_LastStartTime , pChunk->ullStartTime );
			return VO_RET_SOURCE2_CHUNKSKIPPED;//continue;
		}

		if( utc > pChunk->ullChunkDeadTime )
		{
			VODS_VOLOGW( "One Time Out! %s Chunk Dead Time: %lld Current Time: %lld" , pChunk->szUrl , pChunk->ullChunkDeadTime , utc );

			return VO_RET_SOURCE2_CHUNKDROPPED;
		}

		if( m_ptr_info->pInfo->GetProgramType() == VO_SOURCE2_STREAM_TYPE_LIVE )
		{
			if(utc < pChunk->ullChunkLiveTime)
			{
				if(pChunk->ullChunkLiveTime == _VODS_INT64_MAX)
				{
					VODS_VOLOGI("Chunk change from live to vod %lld", utc);
				}
				else
				{
					VODS_VOLOGI("Chunk is a future one. It will be hold about %lld", pChunk->ullChunkLiveTime - utc);
				}
			}

			// sleep until reach live time
			while((pChunk->ullChunkLiveTime != _VODS_INT64_MAX && utc < pChunk->ullChunkLiveTime) && !m_is_stop)
			{
				voOS_Sleep(5);
				utc = voOS_GetSysTime() - m_ptr_info->pInfo->GetStartSysTime() + m_ptr_info->pInfo->GetStartUTC();
			}
		}
	}

	m_ptr_curitem = pChunk;

	VO_U32 nRetry = 0;
#if 0
	while(!m_is_stop && (ret = ToGetItem( pChunk ) == VO_RET_SOURCE2_NEEDRETRY )
	{
		nRetry ++;
		VODS_VOLOGW("ErrorHandler Need Retry:%d", nRetry );
		voOS_Sleep(1);
	}
#else
	VO_BOOL bContinue = VO_TRUE;
	VO_U32 nErrCode = 0;

	while(!m_is_stop && bContinue )
	{
		ret = ToGetItem( pChunk, nErrCode);

		VO_U32 ret1 = VO_RET_SOURCE2_OK;
		switch ( ret )
		{
		case VO_RET_SOURCE2_CHUNKDROPPED:
		case VO_RET_SOURCE2_CHUNKPARTERROR:
			{   
				if( VO_RET_SOURCE2_CHUNKPARTERROR == ret )
				{
					ret = VO_RET_SOURCE2_CHUNKDROPPED;
					bContinue = VO_FALSE;
				}
				else
				{
					ret1 = SendIOWarning_II(VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_DOWNLOADERROR, pChunk,nErrCode );
					if( ret1 == VO_RET_SOURCE2_NEEDRETRY)
					{
						nRetry ++;
						VODS_VOLOGW("ErrorHandler Need Retry:%d", nRetry );
						voOS_Sleep(1);
						bContinue = VO_TRUE;
					}
					else
						bContinue = VO_FALSE;
				}
			}
			break;
		case VO_RET_SOURCE2_FORMATUNSUPPORT:
			{
				ret1 = SendIOWarning_II(VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_FILEFORMATUNSUPPORTED, pChunk,nErrCode );
				bContinue = VO_FALSE;
			}
			break;
		case VO_RET_SOURCE2_DRMERROR:
			{
				ret1 = SendDrmWarning(VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_DRMERROR,pChunk,ret);
				bContinue = VO_FALSE;
			}
			break;
		case VO_RET_SOURCE2_OK:
			bContinue = VO_FALSE;
			break;
		default:
			bContinue = VO_FALSE;
			ret = VO_RET_SOURCE2_CHUNKDROPPED;
			break;
		}
	}
#endif
	

	if( pChunk->Type != VO_SOURCE2_ADAPTIVESTREAMING_HEADDATA && 
		pChunk->Type != VO_SOURCE2_ADAPTIVESTREAMING_SEGMENTINDEX &&
		pChunk->Type != VO_SOURCE2_ADAPTIVESTREAMING_INITDATA && ret == VO_RET_SOURCE2_OK)
	{
		m_LastStartTime = pChunk->ullStartTime;
	}
	else if(pChunk->uFlag & VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_PROGRAMCHANGE)
	{
		m_LastStartTime = _VODS_INT64_MAX;
	}

	if( ret != VO_RET_SOURCE2_OK )
		*pReset = VO_TRUE;
	VOLOGR(" - ToDealItem_II.Ret:0x%08x", ret );
	return ret;
}
#ifndef _USE_BUFFERCOMMANDER
VO_U32 voChannelItemThread::InfoChunk( VO_U64 uTimestamp, VO_BOOL isChunkBegin )
{
	VOLOGR(" +InfoChunk");

	_SAMPLE sample;
	memset( &sample , 0 , sizeof(_SAMPLE) );
	sample.uTime = uTimestamp;
	sample.uFlag = isChunkBegin ? VO_DATASOURCE_FLAG_CHUNK_BEGIN:VO_DATASOURCE_FLAG_CHUNK_END;

	m_ptr_info->pInfo->GetSampleCallback()->SendData( m_ptr_info->pInfo->GetSampleCallback()->pUserData ,(VO_U16) VO_DATASOURCE_TT_CHUNKINFO , &sample );
	VOLOGR(" -InfoChunk");

	return VO_RET_SOURCE2_OK;
}
#endif

VO_U32 voChannelItemThread::MarkOPFilter(VO_U16 nOutputType, VO_PTR pData)
{
	VO_U32 ret = VO_DATASOURCE_RET_OK;
	if( !pData )
		return ret;
	VO_BOOL bSend = VO_FALSE;
	_SAMPLE *pSample = (_SAMPLE *)pData;
	/*trackinfo reset shall be passed down*/
	VOLOGR("MarkOPFilter.Flag:%d, OP:%d", pSample->uMarkFlag, pSample->uMarkOP);
	if( pSample->uMarkFlag & VO_DATASOURCE_MARKFLAG_RESET)
	{
		bSend = VO_TRUE;
		ret = VO_DATASOURCE_RET_SHOULDSKIP;
	}
	else if( pSample ->uMarkFlag & VO_DATASOURCE_MARKFLAG_ADD)
	{
		switch( pSample->uMarkOP )
		{
		case VO_DATASOURCE_MARKOP_CONSTRUCTPROGRAMINFO_ADD:
		case VO_DATASOURCE_MARKOP_CONSTRUCTPROGRAMINFO_ADDBEGIN:
		case VO_DATASOURCE_MARKOP_CONSTRUCTPROGRAMINFO_ADDEND:
			{
				bSend = VO_TRUE;
				ret = VO_DATASOURCE_RET_SHOULDSKIP;
			}
			break;
		default:
			{
				bSend = VO_FALSE;
				ret = VO_DATASOURCE_RET_OK;
			}
			break;
		}
	}
	else if( pSample->uMarkOP != 0)
	{
		bSend = VO_FALSE;
		ret = VO_DATASOURCE_RET_SHOULDSKIP;
	}

	if( bSend )
	{
		/* re Construct ProgramInfo by trackInfo*/

		_TRACK_INFO *pTrackInfo = (_TRACK_INFO *) pSample->pFlagData;
		if( pTrackInfo )
		{
			pTrackInfo->uASTrackID = m_ptr_curitem->sKeyID.uTrackID;
		}

		voProgramInfoOp *pOp = m_ptr_info->pInfo->GetProgramInfoOP();
		if( pOp && pTrackInfo)
			pOp->ConstructProgramInfo(pSample->uMarkFlag, pSample->uMarkOP, 0 , nOutputType, pTrackInfo, 1 );
		else if( pOp)
		{
			_TRACK_INFO ti ;
			ti.uASTrackID = m_ptr_curitem->sKeyID.uTrackID;
			pOp->ConstructProgramInfo(pSample->uMarkFlag, pSample->uMarkOP, 0, nOutputType, &ti, 1 );
		}
		/*.............*/
	}
	return ret;
}

VO_U32 voChannelItemThread::GetHeadData( VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE  Type )
{
	if( Type == VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE )
		return m_pFileParser->CreateTrackInfo2( VO_SOURCE2_TT_SUBTITLE , NULL);
	return VO_RET_SOURCE2_FAIL;
}

#ifdef _USE_BUFFERCOMMANDER
VO_U32 voChannelItemThread::Downloading( VO_U64 &uDownloadTime, VO_U32 &uReadSize, VO_U32 &nErrCode)
{

	VOLOGI("+ Downloading Start.")

	VO_U32 ret = VO_RET_SOURCE2_OK;

	VO_HANDLE h = NULL;

	VO_SOURCE2_IO_API *pIO = m_ptr_info->pInfo->GetIOPtr();

	VO_CHAR url[MAXURLLEN] = {0};
	memset( url, 0x00, sizeof(url) * sizeof(VO_CHAR) );
	GetTheAbsolutePath( url , m_ptr_curitem->szUrl , m_ptr_curitem->szRootUrl );

	voAutoIOInit init_obj( pIO , url , &h );
	if( init_obj.m_ret != VO_SOURCE2_IO_OK )
	{
		nErrCode = pIO->GetLastError(h);
		VODS_VOLOGW("-AutoIO Init failed( errCode:0x%08x )! %s",nErrCode, url);
		ret = VO_RET_SOURCE2_CHUNKDROPPED;
	}

	if( VO_RET_SOURCE2_OK == ret )
	{
		if( m_ptr_curitem->ullChunkOffset != INAVALIBLEU64 )
		{
			VO_SOURCE2_IO_HTTPRANGE range;
			range.ullOffset = m_ptr_curitem->ullChunkOffset;
			range.ullLength = m_ptr_curitem->ullChunkSize;
			pIO->SetParam( h , VO_SOURCE2_IO_PARAMID_HTTPRANGE , &range );

			VODS_VOLOGI( "Range: %lld - %lld(%lld)" , range.ullOffset, range.ullOffset + range.ullLength, range.ullLength );
		}

		if( m_ptr_info->pInfo->GetMaxDownloadBitrate() != _VODS_INT32_MAX )
		{
			VO_S32 maxbitrate = m_ptr_info->pInfo->GetMaxDownloadBitrate();
			pIO->SetParam( h , VO_SOURCE2_IO_PARAMID_HTTPMAXSPEED , &maxbitrate );
		}

		VO_SOURCE2_VERIFICATIONINFO * pInfo = m_ptr_info->pInfo->GetVerificationInfo();

		if( pInfo )
		{
			pIO->SetParam( h , VO_SOURCE2_IO_PARAMID_DOHTTPVERIFICATION , pInfo );
		}
		VO_SOURCE2_IO_HTTPCALLBACK *pHttpCb = m_ptr_info->pInfo->GetIOHttpCallback();
		if( pHttpCb )
		{
			pIO->SetParam(h, VO_SOURCE2_IO_PARAMID_HTTPIOCALLBACK, pHttpCb );
		}
	}
	
	if( VO_RET_SOURCE2_OK == ret )
	{	
		voAutoIOOpen open_obj( pIO , h , VO_TRUE );
	//	voAutoIOOpen open_obj( pIO , h , VO_FALSE );
		//using async open for sourceIO
		if( open_obj.m_ret != VO_SOURCE2_IO_OK )
		{
			nErrCode = pIO->GetLastError(h);
			VODS_VOLOGW("-AutoIO Open failed( errCode:%d )! %s",nErrCode, url);

			ret = VO_RET_SOURCE2_CHUNKDROPPED;
		}
	
		VODS_VOLOGI("+ ToProcessChunk.(URL:%s)", m_ptr_curitem->szUrl);
		ret = ToProcessChunk(h, m_ptr_curitem, &uReadSize);
		if(ret == VO_RET_SOURCE2_UPDATEURL)
		{
			if(pIO && pIO->Close && h)
			{
				pIO->Close(h);
			}
			UpdateProcess(VO_FALSE);
			VO_ADAPTIVESTREAMPARSER_CHUNK* pChunk = 0;
			ret = m_ptr_info->pInfo->GetAdaptiveStreamingParserPtr()->GetChunk(m_chunkType,&pChunk);
			if(ret == VO_RET_SOURCE2_OK)
			{
				ret = DownloadingForUpdateUrl(h,pChunk,uReadSize,uDownloadTime);
			}
		}
		else
		{
			if(ret == VO_DATASOURCE_RET_DOWNLOAD_SLOW)
			{
				VOLOGW("download slow abort!");
			}
			else
			{
				if(ret == VO_RET_SOURCE2_DRMERROR)
				{
					VODS_VOLOGW("this chunk meet drm error");
				}
				else 
				{
					if( uReadSize == 0 )
					{
						ret = VO_RET_SOURCE2_CHUNKDROPPED;
						VODS_VOLOGW(" this Chunk size is Zero.%s", url);
					}
					if( ret == VO_RET_SOURCE2_CHUNKDROPPED )
					{
						nErrCode = pIO->GetLastError(h);
						VODS_VOLOGW("( errCode:0x%08x )! %s",nErrCode, url);
					}
					if( ret == VO_RET_SOURCE2_FORMATUNSUPPORT )
					{
						VODS_VOLOGW("( VO_RET_SOURCE2_FORMATUNSUPPORT )! %s", url);
					}
				}
			}

			VO_U32 ret1 = pIO->GetParam( h , VO_SOURCE2_IO_PARAMID_HTTPDOWNLOADTIME , &uDownloadTime );
			if( VO_SOURCE2_IO_OK != ret1 )
				uDownloadTime = 0;
		}
		m_nReadedSize = 0;
		VODS_VOLOGI("- ToProcessChunk.(URL:%s, ISStop:%d, ItemFlag:%08x Ret:%08x)", m_ptr_curitem->szUrl,m_is_stop, m_ptr_curitem->uFlag, ret);
	}

	if( 0 == uDownloadTime )
		uDownloadTime = 1; // 2013-09-04 Leon, 1 is for protecting division to 0

	VODS_VOLOGI("- Downloading End.Ret:%08x! Time Cost %d Chunk duration %d Size %d Speed %d url %s", ret, (VO_U32)uDownloadTime , (VO_U32)(m_ptr_curitem->ullDuration / ( m_ptr_curitem->ullTimeScale / 1000. )), uReadSize,(VO_U32)(uReadSize * 8000. /uDownloadTime) , url );
	return ret;
}


VO_U32 voChannelItemThread::SendChunkInfoEvent(VO_U32 uEvent, VO_U64 uDownloadTime, VO_U32 uDownloadSize, VO_U32 uRet)
{

	VO_SOURCE2_EVENTCALLBACK *pEventCallback = m_ptr_info->pInfo->GetEventCallback();

	CHUNKINFO info;
	memset( &info , 0 , sizeof( CHUNKINFO ) );
	VO_U32 uFlag = 0;
	info.chunktype = VO_SOURCE2_ADAPTIVESTREAMING_UNKNOWN;
	if(m_ptr_curitem)
	{
		uFlag = m_ptr_curitem->uFlag;
		info.duration			= m_ptr_curitem->ullDuration / ( m_ptr_curitem->ullTimeScale / 1000. );
		info.size				= (VO_U64) uDownloadSize;
		info.download_time		= uDownloadTime;
		if(uRet == VO_RET_SOURCE2_OK || uRet == VO_DATASOURCE_RET_DOWNLOAD_SLOW)
		{
			info.chunktype			= m_ptr_curitem->Type;
		}
		info.start_timestamp	= m_ref_timestamp + 1;
		info.end_timestamp		= m_ref_timestamp;
		info.uASTrackID			= m_ptr_curitem->sKeyID.uTrackID;
	}

	info.ppChunkAgent		= &m_pDownloadAgent;
	if( VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CHUNKBEGIN == uEvent )
	{	
		m_pDownloadAgent = NULL;
		info.bNeedSmoothSwitch = (VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_SMOOTH_ADAPTIONEX & uFlag)? VO_TRUE : VO_FALSE;
	}
	else if( VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CHUNKCOMPLETE == uEvent )
	{
		info.bDownloadComplete = ( uRet == 0 ? VO_TRUE : VO_FALSE );
		info.bNeedSwitchAudioThread = m_bSwitchAudioThread;
		m_bSwitchAudioThread = VO_FALSE;
	}
	else
		return 0;

	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	if( !m_is_stop )
		ret = pEventCallback->SendEvent( pEventCallback->pUserData , uEvent, (VO_U32)&info , uFlag);
	if(VO_RET_SOURCE2_OK == ret && info.ppChunkAgent && VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CHUNKBEGIN == uEvent)
		m_pDownloadAgent = *(info.ppChunkAgent);
	if( VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CHUNKCOMPLETE == uEvent)
		m_pDownloadAgent = NULL;
	return ret;
}

VO_U32 voChannelItemThread::UpdateProcess(VO_BOOL isChunkBegin)
{
	if(isChunkBegin)
	{
		m_chunk_info.uChunkID += 1;
		m_ptr_info->pInfo->GetAdaptiveStreamingParserPtr()->SetParam(VO_PID_ADAPTIVESTREAMING_CHUNK_INFO,&m_chunk_info);
	}
	else
	{
		m_ptr_info->pInfo->GetAdaptiveStreamingParserPtr()->SetParam(VO_PID_ADAPTIVESTREAMING_CHUNK_INFO,&m_chunk_info);
		
	}
	m_ptr_info->pInfo->SetUpdateUrlOn(m_ptr_info->type,VO_FALSE);
	return VO_RET_SOURCE2_OK;
}	

VO_U32 voChannelItemThread::DownloadingForUpdateUrl(VO_HANDLE &h, VO_ADAPTIVESTREAMPARSER_CHUNK* pChunk, VO_U32 &uReadSize,VO_U64 &nDownloadTime)
{
	VO_SOURCE2_IO_API *pIO = m_ptr_info->pInfo->GetIOPtr();
	VO_U32 ret = VO_RET_SOURCE2_OK;
	
	VO_CHAR url[MAXURLLEN] = {0};
	memset( url, 0x00, sizeof(url) * sizeof(VO_CHAR) );
	GetTheAbsolutePath( url , pChunk->szUrl , pChunk->szRootUrl );

	voAutoIOInit init_obj( pIO , url , &h );
	if( init_obj.m_ret != VO_SOURCE2_IO_OK )
	{
		VODS_VOLOGW("-AutoIO Init failed! %s", url);
		ret = VO_RET_SOURCE2_CHUNKDROPPED;
	}

	if( VO_RET_SOURCE2_OK == ret )
	{
		if( pChunk->ullChunkOffset != INAVALIBLEU64 )
		{
			VO_SOURCE2_IO_HTTPRANGE range;
			range.ullOffset = pChunk->ullChunkOffset;
			range.ullLength = pChunk->ullChunkSize;
			pIO->SetParam( h , VO_SOURCE2_IO_PARAMID_HTTPRANGE , &range );

			VODS_VOLOGI( "Range: %lld - %lld(%lld)" , range.ullOffset, range.ullOffset + range.ullLength, range.ullLength );
		}

		if( m_ptr_info->pInfo->GetMaxDownloadBitrate() != _VODS_INT32_MAX )
		{
			VO_S32 maxbitrate = m_ptr_info->pInfo->GetMaxDownloadBitrate();
			pIO->SetParam( h , VO_SOURCE2_IO_PARAMID_HTTPMAXSPEED , &maxbitrate );
		}

		VO_SOURCE2_VERIFICATIONINFO * pInfo = m_ptr_info->pInfo->GetVerificationInfo();

		if( pInfo )
		{
			pIO->SetParam( h , VO_SOURCE2_IO_PARAMID_DOHTTPVERIFICATION , pInfo );
		}
		VO_SOURCE2_IO_HTTPCALLBACK *pHttpCb = m_ptr_info->pInfo->GetIOHttpCallback();
		if( pHttpCb )
		{
			pIO->SetParam(h, VO_SOURCE2_IO_PARAMID_HTTPIOCALLBACK, pHttpCb );
		}
	}
	
	if( VO_RET_SOURCE2_OK == ret )
	{	
		voAutoIOOpen open_obj( pIO , h , VO_TRUE );
		if( open_obj.m_ret != VO_SOURCE2_IO_OK )
		{
			VODS_VOLOGW("-AutoIO Open failed! %s", url);
			ret = VO_RET_SOURCE2_CHUNKDROPPED;
		}
	
		VODS_VOLOGI("+ ToProcessChunk.(URL:%s)", pChunk->szUrl);
		pChunk->uFlag = 0;
		ret = ToProcessChunk(h, pChunk, &uReadSize);
		VODS_VOLOGI("- ToProcessChunk.(URL:%s)",pChunk->szUrl);
	}
	return ret;
}

#endif