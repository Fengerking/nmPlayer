#include "voAdaptiveStreamingController.h"
#include "vo_thread.h"
#include "CSourceIOUtility.h"
#include "voOSFunc.h"
#include "voLog.h"
#include "voAdaptiveStreamingClassFactory.h"
#include "voSmartPointor.hpp"
#include "voToolUtility.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

//#define _CONTROLLER_DUMPFILE
//#define _CHECK_DVR

#ifndef LOG_TAG
#define LOG_TAG "voAdaptiveStreamingController"
#endif

#ifdef WIN32
typedef VO_S32 ( VO_API * pvoGetDRMAPI)(VO_DRM2_API * pDRMHandle, VO_U32 uFlag);
#endif

typedef VO_VOID ( VO_API * pvoGetSourceIOAPI)( VO_SOURCE2_IO_API * ptr_api );


#define _CHECKEDGAPTIME 1000 * 5

#if defined VOLOGR && defined _CONTROLLER_DUMPFILE
FILE *pDumpFile_a = NULL;
FILE *pDumpFile_v = NULL;
FILE *pDumpFile_s = NULL;
#endif


#define __SAFEUNINITDRM(A) \
{	\
	if( A.hHandle && A.Init ) \
	{ \
		A.Uninit( A.hHandle ); \
		A.hHandle = NULL; \
	} \
}

VO_VOID BubbleSort(VO_SOURCE2_BITRATE_INFOITEM* a,VO_U32 n)
{
	VO_U32 i,j;
	VO_SOURCE2_BITRATE_INFOITEM tmp;
	for(i = 0;i<n-1;i++)
	{
		for(j=i+1;j<n;j++)
		{
			if(a[i].uBitrate>a[j].uBitrate)
			{
				tmp=a[i];
				a[i]=a[j];
				a[j]=tmp;
			}
		}
	}
}

voAdaptiveStreamingController::voAdaptiveStreamingController(void)
:m_ptr_parser(0)
,m_streaming_type(VO_ADAPTIVESTREAMPARSER_STREAMTYPE_UNKOWN)
,m_ptr_io(0)
,m_is_stop(VO_FALSE)
,m_pPlaylistData(0)
,m_PlaylistDataSize(0)
,m_PlaylistDataUsedSize(0)
,m_pUpdateUrlPlaylistData(0)
,m_UpdateUrlPlaylistDataSize(0)
,m_UpdateUrlPlaylistDataUsedSize(0)
,m_ptr_ba(0)
,m_is_pause(VO_FALSE)
,m_is_start(VO_FALSE)
,m_video_thread_eos(VO_FALSE)
,m_audio_thread_eos(VO_FALSE)
,m_SelBitrate( 0xffffffff )
,m_enableBA( VO_TRUE )
,m_DisableCpuBA(VO_FALSE)
,m_ptr_drmcallback(0)
,m_contain_video(VO_TRUE)
,m_ptr_thread_audio(0)
,m_ptr_thread_video(0)
,m_ptr_thread_subtitle(0)
,m_b_offset( VO_FALSE )
,m_ptr_eventcallback(0)
,m_CurBitrate(0)
,m_hasSetBufferingTime( VO_FALSE )
,m_hasSetStartBufferTime( VO_FALSE )
#ifndef _USE_BUFFERCOMMANDER
,m_nBufferingTime(5000)
,m_nStartBufferTime(2000)
,m_nMaxBufferTime(20000)
#endif
,m_nLiveDelayTime(-1)
,m_nBufferDuration(0)
,m_uLastAudio(0)
,m_uLastVideo(0)
,m_uLastSubtitle(0)
,m_TolerantErrorCount(0)
,m_RetryTimeOut(120)
,m_uCheckedGapTime_forSubtitle(0)
,m_uCheckedGapTime_SubtitleCounts(0)
,m_uCheckedGapTime_forVideo(0)
,m_uCheckedGapTime_VideoCounts(0)
,m_uCheckedGapTime_forAudio(0)
,m_uCheckedGapTime_AudioCounts(0)
,m_pVologCB(NULL)
, m_pTrackInfoV(NULL)
,m_pTrackInfoA(NULL)
,m_pTrackInfoS(NULL)
,m_nCheckTime(0)
,m_nCheckTime_forSeek(_VODS_INT32_MAX)
,m_pIOHttpHeader(NULL)
,m_pIOHttpProxy(NULL)
,m_bOpenComplete(VO_FALSE)
,m_threadHandleSeek(NULL)
,m_nPositionSeek(-1)
,m_bSeeking(VO_FALSE)
,m_bSeekingBySelectTrack(VO_FALSE)
,m_status(CONTROLLER_STATUS_UNKNOWN)
,m_bStopSeek( VO_FALSE )
,m_bStopUpdateUrl(VO_FALSE)
,m_uLastChunkID(_VODS_INT64_MAX)
,m_uLastChunkTimeStamp(-1)
,m_bAudioUnavailableBlock(VO_FALSE)
,m_bVideoUnavailableBlock(VO_FALSE)
,m_bSubtitleUnavailableBlock(VO_FALSE)
,m_bNeedSwitchAudioThread(VO_FALSE)
,m_bDashInitDataReady(VO_FALSE)
,m_bUpdatingURL(VO_FALSE)
#ifdef _USE_BUFFERCOMMANDER
,m_bIsFirstChunk(VO_TRUE)
#endif
#ifdef DUMP_MANIFEST
,m_bFirstSubManifest(VO_TRUE)
#endif
,m_bOutSideSourceIO(VO_FALSE)
,m_uDuration(0)
{
	memset( m_url , 0 , sizeof( m_url ) );

	m_EventCallback.pUserData = this;
	m_EventCallback.SendEvent = OnEvent;

	m_IOEventCallback.pUserData = this;
	m_IOEventCallback.NotifyEvent = OnEvent;

	m_SampleCallback.pUserData = this;
	m_SampleCallback.SendData = OnSample;


	m_info.SetSampleCallback( &m_SampleCallback );
	m_info.SetEventCallback( &m_EventCallback );
	m_info.SetIOEventCallback( &m_IOEventCallback );

#ifndef _USE_BUFFERCOMMANDER
	CreateAdaptiveStreamingBuffer( &m_ptr_samplebuffer , m_nBufferingTime , m_nMaxBufferTime , m_nStartBufferTime );
#else
	VO_U32 ret = m_BufferCommander.Init();
	if( VO_RET_SOURCE2_OK != ret)
	{
		VOLOGI("Buffer Init Fail.0x%08x", ret);
		//return ret;	
	}
#endif
	memset( &m_drm_api , 0 , sizeof( VO_StreamingDRM_API ) );
	voGetStreamingDRMAPI( &m_drm_api , 0 );
	memset( &m_drm_api_audio , 0 , sizeof( VO_StreamingDRM_API ) );
	voGetStreamingDRMAPI( &m_drm_api_audio , 0 );
	memset( &m_drm_api_video , 0 , sizeof( VO_StreamingDRM_API ) );
	voGetStreamingDRMAPI( &m_drm_api_video , 0 );
	memset( &m_drm_api_subtitle , 0 , sizeof( VO_StreamingDRM_API ) );
	voGetStreamingDRMAPI( &m_drm_api_subtitle , 0 );

	memset( &m_eventHandler_api , 0 , sizeof( VO_StreamingEVENTHandler_API ) );
	voGetStreamingEVENTHandlerAPI( &m_eventHandler_api , 0 );

#ifdef _USE_SHARELIB
	memset( &m_sharelibApi , 0 , sizeof( VO_ShareLib_API ) );
	voGetShareLibAPI( &m_sharelibApi , 0 );
	m_sharelibApi.Init(&m_sharelibApi.hHandle, NULL);
#endif

	memset( &m_internalba , 0 , sizeof( VO_BITRATE_ADAPTATION_API ) );
	if( !m_ptr_ba )
	{
		voGetBitrateAdaptationAPI( &m_internalba );
		m_ptr_ba = &m_internalba;
		m_ptr_ba->Init(&m_ptr_ba->hHandle);
	}
	
	memset( &m_internalio , 0 , sizeof( VO_SOURCE2_IO_API ) );

	m_ds_callback.pUserData = this;
	m_ds_callback.SendEvent = OnEvent;
	m_pProgramInfoOP = new voProgramInfoOp( &m_ds_callback );
	m_info.SetProgramInfoOP( m_pProgramInfoOP );
	m_pTrackSampleFilter = new vo_tracksample_filter( m_pProgramInfoOP );
	
	memset( &m_DVRInfo, 0x00, sizeof( VO_SOURCE2_SEEKRANGE_INFO ));

#ifdef WIN32
	//HMODULE h = LoadLibrary( _T("D:\\CheckOutSVN2\\Numen\\voRelease\\Win32\\Bin\\XP\\voDRM_VisualOn_AES128.dll") );
	//pvoGetDRMAPI getapi= (pvoGetDRMAPI)GetProcAddress( h , "voGetDRMAPI" );
	//VO_DRM2_API api;
	//getapi( &api , 0 );
	//VO_PTR hd;
	//api.Init( &hd , 0 );
	//api.GetInternalAPI( hd , (VO_PTR*)&m_ptr_drmcallback );
#endif
#ifdef _HTTPHEADER_TEST
	httpCB.hHandle = this;
	httpCB.IO_Callback = IO_HTTP_CALLBACK_FUNC_TEST;
	m_info.SetIOHttpCallback( &httpCB );
#endif

#if defined VOLOGR && defined _CONTROLLER_DUMPFILE
#ifdef _LINUX
	if( !pDumpFile_a)
		pDumpFile_a = fopen("/sdcard/audio_hc","wb");

	if( !pDumpFile_v)
		pDumpFile_v = fopen("/sdcard/video_hc","wb");
	if( !pDumpFile_s)
		pDumpFile_s = fopen("/sdcard/subtitle_hc","wb");
#elif defined WIN32
	if( !pDumpFile_a)
		pDumpFile_a = fopen("d:/audio_hc","wb");
	if( !pDumpFile_v)
		pDumpFile_v = fopen("d:/video_hc","wb");
	if( !pDumpFile_s)
		pDumpFile_s = fopen("d:/subtitle_hc","wb");

#endif
#endif
	bitrate_info.pItemList = NULL;
	m_ba_threshold.nUpper = 0xffffffff;
	m_ba_threshold.nLower = 0;
}

voAdaptiveStreamingController::~voAdaptiveStreamingController(void)
{
	Close();

	if( m_pPlaylistData )
	{
		delete []m_pPlaylistData;
		m_pPlaylistData = 0;
	}
	if(m_pUpdateUrlPlaylistData)
	{
		delete []m_pUpdateUrlPlaylistData;
		m_pUpdateUrlPlaylistData = 0;
	}
	if( m_pProgramInfoOP)
		delete m_pProgramInfoOP;
	m_pProgramInfoOP = NULL;

	if( m_pTrackSampleFilter)
		delete m_pTrackSampleFilter;
	m_pTrackSampleFilter = NULL;
#ifndef _USE_BUFFERCOMMANDER
	DestroyAdaptiveStreamingBuffer( m_ptr_samplebuffer );
#endif

	ReleaseTrackInfoOP_T(m_pTrackInfoA);
	ReleaseTrackInfoOP_T(m_pTrackInfoV);
	ReleaseTrackInfoOP_T(m_pTrackInfoS);
	
	VOLOGI(" + free sourceIO");
	if( m_ptr_io && !m_bOutSideSourceIO)
	{
		m_ptr_io->UnInit(NULL);
		VOLOGI("1. free sourceIO")
		m_ptr_io->SetParam(NULL, VO_SOURCE2_IO_PARAMID_DESTROY, NULL);
		VOLOGI("2. free sourceIO")
		VOLOGI("3. free sourceIO")
	}
	VOLOGI(" - free sourceIO")

	if(m_ptr_ba)
		m_ptr_ba->Uninit(m_ptr_ba->hHandle);
	
#if defined VOLOGR && defined _CONTROLLER_DUMPFILE
	if(pDumpFile_a)
		fclose(pDumpFile_a);
	if(pDumpFile_v)
		fclose(pDumpFile_v);
	if(pDumpFile_s)
		fclose(pDumpFile_s);
#endif
	VOLOGUNINIT()
}

VO_U32 voAdaptiveStreamingController::Init( VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pInitParam )
{
	if( !( nFlag & VO_SOURCE2_FLAG_OPEN_URL ) )
		return VO_RET_SOURCE2_FORMATUNSUPPORT;

	m_nFlag = nFlag;

	memset(m_url, 0x00, sizeof(m_url) );
#if defined _UNICODE || defined UNICODE
	int size = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pSource, -1, NULL, NULL, NULL, NULL );
	WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pSource, -1, m_url, size, NULL, NULL );
#else
	strcpy( m_url , (VO_CHAR*)pSource );
#endif
	VOLOGINIT(pInitParam->strWorkPath)
	return VO_RET_SOURCE2_OK;
}

VO_U32 voAdaptiveStreamingController::Uninit()
{
	return VO_RET_SOURCE2_OK;
}

VO_U32 voAdaptiveStreamingController::ToOpenInternal()
{
	VO_U32 ret = OpenInternalII();
	if(((ret & VO_ERR_LICENSE_ERROR) == VO_ERR_LICENSE_ERROR) && !m_is_stop)
	{
		VODS_VOLOGW("Open license check failed!");
		OnEvent( this, VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_ERROR, VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_STREAMING_VOLIBLICENSEERROR, ret);
	}
	else if( ret != VO_RET_SOURCE2_OK && !m_is_stop)
	{	
		VODS_VOLOGW("Open failed.Send VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_ERROR.ret is 0x%08x",ret);
		SendEvent( VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_ERROR, VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_STREAMING_DOWNLOADFAIL, NULL);
	}
	m_bOpenComplete = VO_TRUE;
	if(m_info.GetProgramType() == VO_SOURCE2_STREAM_TYPE_LIVE)
	{
		VO_U64 WindowLength;
		VO_U64 EndLength;
		if( m_ptr_parser->GetParam( VO_PID_ADAPTIVESTREAMING_DVR_WINDOWLENGTH, &WindowLength) == VO_RET_SOURCE2_OK &&
			m_ptr_parser->GetParam( VO_PID_ADAPTIVESTREAMING_DVR_ENDLENGTH, &EndLength) == VO_RET_SOURCE2_OK)
		{
			if(WindowLength > EndLength)
			{
				m_TimeStampRecalculator.seek(WindowLength - EndLength);
				VOLOGI("live start time is %lld",WindowLength - EndLength);
			}
			else
			{
				m_TimeStampRecalculator.seek(0);
				VOLOGW("WARNING:live start time is invalid, set to 0");
			}
			
		}
	}
	if(m_ptr_parser)
	{
		m_ptr_parser->GetDuration(&m_uDuration );
	}

	BeginThread_Seek();

	return ret;
}
VO_U32 voAdaptiveStreamingController::Open()
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	m_nCheckTime = 0;
	RecordCostTime( m_nCheckTime, true, NULL );
	RecordCostTime( m_nCheckTime, false, "Whole Open Start" );

	Close();

	m_info.SetBuffering(VO_TRUE);
#ifdef _USE_BUFFERCOMMANDER
	m_BufferCommander.Init();
#endif
	SetStopFlag(VO_FALSE);
	if( m_nFlag & VO_SOURCE2_FLAG_OPEN_ASYNC )
	{
		vo_thread::begin();
		ret = VO_RET_SOURCE2_OK;
	}
	else
	{
		ret = ToOpenInternal();
	}
	RecordCostTime( m_nCheckTime, false, "Whole Open End", 3 );

	return ret;
}


VO_U32 voAdaptiveStreamingController::OpenInternalII()
{
	VO_U32 nTimeCheck_S = 0;
	RecordCostTime( nTimeCheck_S, true, NULL );

// 	VO_TCHAR *pWorkPath = _T("/data/data/com.visualon.osmpDemoPlayer/libs/");
// 	m_info.SetWorkPath(pWorkPath);
	RecordCostTime( nTimeCheck_S, false, "Open Step1. Close() ",3);
	if( m_eventHandler_api.Init )
	{
		m_eventHandler_api.Init( &m_eventHandler_api.hHandle , m_ptr_eventcallback , VO_ADAPTIVESTREAMPARSER_STREAMTYPE_UNKOWN, 0 );
		m_eventHandler_api.SetParameter(m_eventHandler_api.hHandle, VO_PID_SOURCE2_TOLERANT_ERROR_NUMBER,&m_TolerantErrorCount);
		m_eventHandler_api.SetParameter(m_eventHandler_api.hHandle, VO_PID_SOURCE2_HTTP_RETRY_TIMEOUT,&m_RetryTimeOut);
	}
	RecordCostTime( nTimeCheck_S, false, "Open Step2. EventHandler_api.Init() ",3 );

	if( m_info.GetWorkPath() == NULL )
	{
		VOLOGE(" WorkPath is NULL. Please Check!!!!!! ");
	}
	if( !m_ptr_io && !m_bOutSideSourceIO)
	{
		VODS_VOLOGI("+Load SourceIO");
		m_ioloader.SetLibOperator( (VO_LIB_OPERATOR*)m_info.GetLibOp() );
		m_ioloader.SetWorkPath( m_info.GetWorkPath() );
		vostrcpy( m_ioloader.m_szDllFile , _T("voSourceIO") );
		vostrcpy( m_ioloader.m_szAPIName , _T("voGetSourceIOAPI") );

		if(m_ioloader.LoadLib(NULL) == 0)
		{
			VOLOGE ("Load IO fail");
			return VO_RET_SOURCE2_FAIL;
		}
		pvoGetSourceIOAPI getapi = (pvoGetSourceIOAPI)m_ioloader.m_pAPIEntry;
		if( !getapi )
		{
			VOLOGE( "Can not get IO API from IO Module!" );
			return VO_RET_SOURCE2_FAIL;
		}
		getapi( &m_internalio );
		m_ptr_io = &m_internalio;
		VODS_VOLOGI("-Load SourceIO");
	}
	if(m_ptr_io)
	{
		m_ptr_io->SetParam( m_ptr_io->hHandle, VO_PID_SOURCE2_WORKPATH, ( VO_PTR )m_info.GetWorkPath());
		m_ptr_io->SetParam( m_ptr_io->hHandle, VO_PID_COMMON_LOGFUNC,  m_pVologCB);
		m_ptr_io->SetParam( m_ptr_io->hHandle, VO_SOURCE2_IO_PARAMID_HTTPHEADINFO, (VO_PTR) m_pIOHttpHeader );
		m_ptr_io->SetParam( m_ptr_io->hHandle, VO_SOURCE2_IO_PARAMID_HTTPPROXYINFO, (VO_PTR) m_pIOHttpProxy );
	}
	RecordCostTime( nTimeCheck_S, false, "Open Step3. IO Load " ,3);
#ifdef _USE_SHARELIB
	m_sharelibApi.SetParameter(m_sharelibApi, VODS_PID_SHARELIB_IO_PTR, m_ptr_io );
#else
	m_info.SetIOPtr( m_ptr_io );
#endif

	InitForDRM(&m_drm_api);
	if( m_drm_api.Init && m_drm_api.hHandle)
	{
		/*exchange url if the url needs to decrypt or other operator*/
		VO_CHAR tmpURL[MAXURLLEN] = {0};
		VO_U32 ret1 = m_drm_api.PreprocessURL( m_drm_api.hHandle, m_url, tmpURL, NULL);
		if( ret1 == VO_RET_SOURCE2_OK )
		{
			memset( m_url, 0x00, sizeof( m_url ));
			strcpy( m_url, tmpURL );
		}
	}
	VOLOGI("URL:%s", m_url);
	VO_ADAPTIVESTREAM_PLAYLISTDATA data;
	memset( &data , 0 , sizeof( VO_ADAPTIVESTREAM_PLAYLISTDATA ) );
	memcpy( data.szUrl, m_url, sizeof(m_url));
	
	VO_U32 ret = DownloadItem_II(m_ptr_io, m_info.GetVerificationInfo(), m_info.GetIOHttpCallback(), &data, &m_pPlaylistData, &m_PlaylistDataSize, &m_PlaylistDataUsedSize, &m_is_stop, VO_FALSE );
	if( m_is_stop )
		return VO_RET_SOURCE2_OPENFAIL;

	RecordCostTime( nTimeCheck_S, false, "Open Step5. Download MainURL ",3 );

	if( ret != VO_SOURCE2_IO_OK || !m_pPlaylistData || 0 == data.uDataSize )
	{
		VOLOGE(" Open error. DataPtr:%p, Size:%d, Ret:%08x", m_pPlaylistData, data.uDataSize, ret);
		return VO_RET_SOURCE2_OPENFAIL;
	}

	VO_U32 readsize = data.uDataSize;
	ret = CheckStreamingType( m_pPlaylistData , readsize );

	RecordCostTime( nTimeCheck_S, false, "Open Step6. CheckStreamingType() " ,3);
	
	if(m_drm_api.Init && m_drm_api.hHandle)
	{
		m_drm_api.SetParameter(m_drm_api.hHandle, VO_PID_AS_DRM2_STREAMING_TYPE, &m_streaming_type);
		VO_U32 ret1 = m_drm_api.Info(m_drm_api.hHandle, data.szNewUrl, data.pData, data.uDataSize, 0);
		// if clear content, it will return ok or no-drm-api.
		if(ret1 != VO_RET_SOURCE2_OK && ret1 != VO_ERR_DRM2_NO_DRM_API)
		{
			OnEvent(this, VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_ERROR, VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_STREAMING_DRMLICENSEERROR, 0);
			VOLOGE("failed to get DRM license, 0x%08X", ret1);
			return VO_RET_SOURCE2_CONTENTENCRYPT;
		}
	}
	RecordCostTime( nTimeCheck_S, false, "Open Step7. DRM Init() ",3 );

	if( m_streaming_type != 0 )
	{
		m_ptr_parser = new voAdaptiveStreamParserWrapper( m_streaming_type , m_info.GetLibOp(), m_info.GetWorkPath(), m_info.GetLogFunc() );
		if( !m_ptr_parser )
			return VO_RET_SOURCE2_EMPTYPOINTOR;

		m_pProgramInfoOP->SetASParser( m_ptr_parser );

		m_eventHandler_api.SetParameter(m_eventHandler_api.hHandle, VO_PID_ADAPTIVESTREAMING_STREAMTYPE, &m_streaming_type);
	}
	VODS_VOLOGI("StreamingType:%d, StreamingParser:%p", m_streaming_type, m_ptr_parser);
	if(m_streaming_type == 0 || !m_ptr_parser )
	{
		return VO_RET_SOURCE2_FORMATUNSUPPORT;
	}

	RecordCostTime( nTimeCheck_S, false, "Open Step8. New voAdaptiveStreamParserWrapper ",3 );

	m_info.SetAdaptiveStreamingParserPtr( m_ptr_parser );

	ret = PreProcessURL( &data );
	ret = m_ptr_parser->Init( &data , &m_EventCallback );
#ifdef DUMP_MANIFEST
	FILE* fp = fopen("/sdcard/CBSi/main.m3u8","wb");
	if(fp)
	{
		fwrite(data.pData,1,data.uDataSize,fp);
		fclose(fp);
	}
#endif
	if( ret != VO_RET_SOURCE2_OK )
		return ret;
	ret = m_ptr_parser->SetParam( VO_PID_SOURCE2_LIBOP, m_info.GetLibOp() );
	if( ret != VO_RET_SOURCE2_OK )
	{
		VOLOGW(" Set LibOp to Parser failed! ");
	}
	ret = m_ptr_parser->SetParam( VO_PID_SOURCE2_WORKPATH, m_info.GetWorkPath() );
	if( ret != VO_RET_SOURCE2_OK )
	{
		VOLOGW(" Set WorkPath to Parser failed! ");
	}
	
	RecordCostTime( nTimeCheck_S, false, "Open Step9. voAdaptiveStreamParserWrapper Init " ,3);
	VO_U64 utc = voOS_GetUTC();
	utc = utc * 1000;
	m_info.SetStartUTC( utc );
	m_info.SetStartSysTime( voOS_GetSysTime() );

	VO_CHAR t[255];
	timeToSting( utc, t);
	VODS_VOLOGI( "UTC Time is: %s " , t );

	m_ptr_parser->SetParam( VO_PID_ADAPTIVESTREAMING_UTC , &utc );

	if( -1 != m_nLiveDelayTime)
	{
		m_ptr_parser->SetParam(VO_PID_ADAPTIVESTREAMING_LIVELATENCY, &m_nLiveDelayTime);
	}

	ret = m_ptr_parser->Open();

	if( ret != VO_RET_SOURCE2_OK )
		return ret;
	RecordCostTime( nTimeCheck_S, false, "Open Step10. voAdaptiveStreamParserWrapper Open " ,3);

	_PROGRAM_INFO *pProInfo = 0;
#ifdef _use_programinfo2
	m_ptr_parser->GetProgramInfo( 0, &pProInfo);
	VOLOGI(" It is a :%s Link", pProInfo->sProgramType == VO_SOURCE2_STREAM_TYPE_LIVE ? " Live" :" VOD" );
	m_eventHandler_api.SetParameter( m_eventHandler_api.hHandle, VO_PID_SOURCE2_PROGRAM_TYPE, &pProInfo->sProgramType);
#else
	m_pProgramInfoOP->GetProgramInfo( 0, &pProInfo);
	if(pProInfo )
	{
		VOLOGI(" It is a :%s Link", pProInfo->sProgramType == VO_SOURCE2_STREAM_TYPE_LIVE ? " Live" :" VOD" );
		m_eventHandler_api.SetParameter( m_eventHandler_api.hHandle, VO_PID_SOURCE2_PROGRAM_TYPE, &pProInfo->sProgramType);
	}
#endif

#ifndef _USE_BUFFERCOMMANDER
	//set buffer type
	VO_BUFFER_SETTING_SAMPLE setting;
	VO_BUFFERING_STYLE_TYPE type;
	setting.nType = VO_BUFFER_SETTING_BUFFERING_STYLE;
	if(pProInfo)
	{
		if(pProInfo->sProgramType == VO_SOURCE2_STREAM_TYPE_VOD)
		{
			 type = VO_BUFFERING_AV;
		}
		else
		{
			type = VO_BUFFERING_A;
		}
	}
	setting.pObj = &type;
	VOLOGI("set buffer type %d",type);
	m_ptr_samplebuffer->AddBuffer( VO_BUFFER_SETTINGS , &setting );
#else
	VO_SOURCE2_PROGRAM_TYPE nType = VO_SOURCE2_STREAM_TYPE_UNKNOWN;
	if(pProInfo )
		nType = pProInfo->sProgramType;
	m_BufferCommander.SetParam(VO_PID_SOURCE2_PROGRAM_TYPE, &nType);
#endif

	
	CreateChannelItemThread( m_streaming_type , &m_ptr_thread_audio );
	CreateChannelItemThread( m_streaming_type , &m_ptr_thread_video );
	CreateChannelItemThread( m_streaming_type , &m_ptr_thread_subtitle );

	RecordCostTime( nTimeCheck_S, false, "Open Step12. Create 2 thread " ,3);

	return VO_RET_SOURCE2_OK;
}

VO_U32 voAdaptiveStreamingController::Close()
{
	VODS_VOLOGI(" + Close");
	Stop();
	m_bOpenComplete = VO_FALSE;
	VO_U32 nTimew = 0;
	RecordCostTime(nTimew, true, NULL );

	VO_U32 nTime = 0;
	RecordCostTime(nTime, true, NULL );

	if(  m_ptr_parser )
	{
		m_ptr_parser->Stop();
		m_ptr_parser->Close();
		m_ptr_parser->UnInit();

		delete m_ptr_parser;
		m_ptr_parser = 0;
	}

	RecordCostTime( nTime, false, "Release Parser" );

	__SAFEUNINITDRM(m_drm_api);
	__SAFEUNINITDRM(m_drm_api_audio);
	__SAFEUNINITDRM(m_drm_api_video);
	__SAFEUNINITDRM(m_drm_api_subtitle);

	if( m_eventHandler_api.hHandle )
	{
		m_eventHandler_api.Uninit( m_eventHandler_api.hHandle );
		m_eventHandler_api.hHandle = NULL;
	}

	RecordCostTime( nTime, false, "Release DRM" );

	DestroyChannelItemThread( m_ptr_thread_audio );
	RecordCostTime( nTime, false, "Destroy Audio Thread" );
	DestroyChannelItemThread( m_ptr_thread_video );
	RecordCostTime( nTime, false, "Destroy Video Thread" );
	DestroyChannelItemThread( m_ptr_thread_subtitle );
	RecordCostTime( nTime, false, "Destroy Subtitle Thread" );

	m_ptr_thread_audio = 0;
	m_ptr_thread_video = 0;
	m_ptr_thread_subtitle = 0;

	

	if( m_ptr_ba )
	{
		m_ptr_ba->Close( m_ptr_ba->hHandle );
	}
	RecordCostTime( nTime, false, "Release BA" );
	if(bitrate_info.pItemList)
	{
		delete []bitrate_info.pItemList;
		bitrate_info.pItemList = NULL;
	}
	RecordCostTime( nTimew, false, "Whole Close", 3 );

	VODS_VOLOGI(" - Close");

	return VO_RET_SOURCE2_OK;
}

VO_U32 voAdaptiveStreamingController::Start()
{

	if( m_is_start && !m_is_pause)
	{
		VODS_VOLOGW("It has been started.Please check.");
		return VO_RET_SOURCE2_OK;
	}	

	if( m_is_pause )
	{
		m_is_pause = VO_FALSE;
		VOLOGI("Release Pause status.Running");
		return VO_RET_SOURCE2_OK;
	}	
	SetStopFlag( VO_FALSE );

	m_status = CONTROLLER_STATUS_RUNNING;

	VODS_VOLOGI( "+Start" );

	RecordCostTime( m_nCheckTime, false, "Start Thead", 3 );
	
	InitForDRM(&m_drm_api_audio);
	m_drm_api_audio.SetParameter(m_drm_api_audio.hHandle, VO_PID_AS_DRM2_STREAMING_TYPE, &m_streaming_type);
	m_info.SetDRMPtr(THREADTYPE_MEDIA_AUDIO, &m_drm_api_audio );

	InitForDRM(&m_drm_api_video);
	m_drm_api_video.SetParameter(m_drm_api_video.hHandle, VO_PID_AS_DRM2_STREAMING_TYPE, &m_streaming_type);
	m_info.SetDRMPtr(THREADTYPE_MEDIA_VIDEO, &m_drm_api_video );

	InitForDRM(&m_drm_api_subtitle);
	m_drm_api_subtitle.SetParameter(m_drm_api_subtitle.hHandle, VO_PID_AS_DRM2_STREAMING_TYPE, &m_streaming_type);
	m_info.SetDRMPtr(THREADTYPE_MEDIA_SUBTITLE, &m_drm_api_subtitle );
	
	THREADINFO * ptr_info = new THREADINFO;
	ptr_info->type = THREADTYPE_MEDIA_AUDIO;
	ptr_info->pInfo = &m_info;
	if( m_ptr_thread_audio )
		m_ptr_thread_audio->Start( ptr_info );

	ptr_info = new THREADINFO;
	ptr_info->type = THREADTYPE_MEDIA_VIDEO;
	ptr_info->pInfo = &m_info;
	if( m_ptr_thread_video )
		m_ptr_thread_video->Start( ptr_info );

	ptr_info = new THREADINFO;
	ptr_info->type = THREADTYPE_MEDIA_SUBTITLE;
	ptr_info->pInfo = &m_info;
	if( m_ptr_thread_subtitle )
		m_ptr_thread_subtitle->Start( ptr_info );
	m_is_start = VO_TRUE;
	VODS_VOLOGI( "-Start" );

	return VO_RET_SOURCE2_OK;
}

VO_U32 voAdaptiveStreamingController::Pause()
{
	VODS_VOLOGI( "+Pause" );
	if(m_is_start)
		m_is_pause = VO_TRUE;
	else
		m_is_pause = VO_FALSE;
	VODS_VOLOGI( "-Pause" );
	return VO_RET_SOURCE2_OK;
}

VO_U32 voAdaptiveStreamingController::Stop()
{
	m_status = CONTROLLER_STATUS_STOP;

	VODS_VOLOGI( "+Stop" );
	SetStopFlag();
#ifdef _USE_BUFFERCOMMANDER
	m_BufferCommander.Stop();
#endif
	VODS_VOLOGI( "+Stop Async Open Thread!" );
	vo_thread::stop();
	VODS_VOLOGI( "-Stop Async Open Thread!" );
	VODS_VOLOGI( "+Stop Async Seek Thread!" );
	StopThread_Seek();
	VODS_VOLOGI( "-Stop Async Seek Thread!" );
	VODS_VOLOGI("+Stop Async Update Url Thread");
	StopThread_UpdateUrl();
	VODS_VOLOGI("-Stop Async Update Url Thread");

	if( m_ptr_io )
	{
		m_ptr_io->Close(NULL);
	}

	VODS_VOLOGI( "+Stop Stop Audio Thread!" );
	if( m_ptr_thread_audio )
		m_ptr_thread_audio->Stop();
	VODS_VOLOGI( "-Stop Stop Audio Thread!" );

	VODS_VOLOGI( "+Stop Stop Video Thread!" );
	if( m_ptr_thread_video )
		m_ptr_thread_video->Stop();
	VODS_VOLOGI( "-Stop Stop Video Thread!" );

	VODS_VOLOGI( "+Stop Stop Subtitle Thread!" );
	if( m_ptr_thread_subtitle )
		m_ptr_thread_subtitle->Stop();
	VODS_VOLOGI( "-Stop Stop Subtitle Thread!" );
#ifdef _USE_BUFFERCOMMANDER
	m_BufferCommander.UnInit();
#endif

	m_is_start = VO_FALSE;
	m_is_pause = VO_FALSE;

	VODS_VOLOGI( "-Stop" );
	return VO_RET_SOURCE2_OK;
}

VO_U32 voAdaptiveStreamingController::SeekAsync(VO_U64 * pTimeStamp)
{
	VOLOGI("+SeekAsync %lld" , *pTimeStamp);

	if(m_is_stop)
	{
		VOLOGW("-SeekAsync m_is_stop %d", m_is_stop);
		return VO_RET_SOURCE2_OK;
	}

	VO_U32 ret = VO_RET_SOURCE2_OK;
	VO_U64 uStartTime = -1;

	while(m_info.GetStreamingType() == VO_ADAPTIVESTREAMPARSER_STREAMTYPE_DASH && !m_bDashInitDataReady && !m_is_stop)
	{
		VOLOGW("Dash data init not ready, wait for a while");
		voOS_SleepExitable(50, &m_is_stop);
	}
	if(m_info.GetProgramType() == VO_SOURCE2_STREAM_TYPE_LIVE)
	{
		// save DVR window start time here
		ret = GetCurrentStartTime(&uStartTime);
		VOLOGI("save DVR window position 0x%08X want: %lld start: %lld", ret, *pTimeStamp, uStartTime);
	}

	VO_BOOL bTrackChanged = VO_FALSE;
	ChangeTrack(&bTrackChanged);
	
	SetStopFlag();

#ifdef _USE_BUFFERCOMMANDER
	m_BufferCommander.Stop();
#endif

	if( m_ptr_thread_audio )
		m_ptr_thread_audio->Stop();
	if( m_ptr_thread_video )
		m_ptr_thread_video->Stop();
	if( m_ptr_thread_subtitle )
		m_ptr_thread_subtitle->Stop();

#ifdef _USE_BUFFERCOMMANDER
	m_BufferCommander.Flush();
#endif

	if( m_status != CONTROLLER_STATUS_STOP )
		SetStopFlag( VO_FALSE );
	else
	{
		VOLOGW("-SeekAsync m_status %d", m_status);
		return VO_RET_SOURCE2_OK;
	}

	/* check the cost of time for seeking. If the buffer is running, OnSample will printf "Source is ok" */
	RecordCostTime(m_nCheckTime_forSeek, true, NULL);

	m_info.SetBuffering(VO_TRUE);

	VO_U64 org = *pTimeStamp;
	VO_ADAPTIVESTREAMPARSER_SEEKMODE sMode = VO_ADAPTIVESTREAMPARSER_SEEKMODE_OBSOLUTE;
	if(m_info.GetProgramType() == VO_SOURCE2_STREAM_TYPE_LIVE)
	{
		if(bTrackChanged && -1 != uStartTime)
		{
			VOLOGI("use saved DVR window start time %lld for select track!", uStartTime);
			ret = VO_RET_SOURCE2_OK;
		}
		else
			ret = GetCurrentStartTime(&uStartTime);

		if(VO_RET_SOURCE2_OK == ret)
		{
			sMode = VO_ADAPTIVESTREAMPARSER_SEEKMODE_DVRWINDOWPOS;
			if(org > uStartTime)
				*pTimeStamp = org - uStartTime;
			else
				*pTimeStamp = 0;
		}
		else
		{
			if(org > m_DVRInfo.ullPlayingTime)
			{
				sMode = VO_ADAPTIVESTREAMPARSER_SEEKMODE_FORWARD;
				*pTimeStamp = org - m_DVRInfo.ullPlayingTime;
			}
			else
			{
				sMode = VO_ADAPTIVESTREAMPARSER_SEEKMODE_BACKWARD;
				*pTimeStamp = m_DVRInfo.ullPlayingTime - org;
			}
		}

		VOLOGI("LIVE seek mode %d step %lld", sMode, *pTimeStamp);
	}

	if(bTrackChanged)
		m_bSeekingBySelectTrack = VO_TRUE;

	if(m_ptr_parser)
		ret = m_ptr_parser->Seek(pTimeStamp, sMode);

	m_bSeekingBySelectTrack = VO_FALSE;

	VOLOGI("parser can seek to %lld", *pTimeStamp);
	
	if( m_ptr_ba )
		m_ptr_ba->SetParam( m_ptr_ba->hHandle , VO_PID_SOURCE2_BA_SEEK , &org );

	if( ret == VO_RET_SOURCE2_OK )
	{
		m_info.GetTimeStampSyncObject().Disable(VO_FALSE);

		if(m_info.GetProgramType() == VO_SOURCE2_STREAM_TYPE_LIVE)
		{
			if(VO_ADAPTIVESTREAMPARSER_SEEKMODE_DVRWINDOWPOS == sMode)
				m_TimeStampRecalculator.seek(*pTimeStamp + uStartTime);
			else
				m_TimeStampRecalculator.seek(org);
		}
		else
			m_TimeStampRecalculator.seek(*pTimeStamp);

#ifndef _USE_BUFFERCOMMANDER
		if( m_ptr_samplebuffer)
		{
			m_ptr_samplebuffer->Flush();
			m_ptr_samplebuffer->set_pos(org);
		}
#else
		m_BufferCommander.SetPosition(org);
#endif

		m_audio_thread_eos = VO_FALSE;
		m_video_thread_eos = VO_FALSE;
		if( m_info.GetProgramType() == VO_SOURCE2_STREAM_TYPE_LIVE )
			m_info.GetTimeStampSyncObject().Reset(org, 1);
		else
			m_info.GetTimeStampSyncObject().Reset(*pTimeStamp, 1);

		m_info.SetStartTime( *pTimeStamp );

		// we need update DVR playing time here for live
		if( m_info.GetProgramType() == VO_SOURCE2_STREAM_TYPE_LIVE )
			m_DVRInfo.ullPlayingTime = org;

		m_b_offset = VO_FALSE;
		if( m_ptr_thread_audio) 
			m_ptr_thread_audio->Resume();
		if( m_ptr_thread_video)
			m_ptr_thread_video->Resume();
		if( m_ptr_thread_subtitle)
			m_ptr_thread_subtitle->Resume();

		//reset unavailableblock here
		m_bAudioUnavailableBlock = m_bVideoUnavailableBlock = m_bSubtitleUnavailableBlock = VO_FALSE;
	}

	VOLOGI("-SeekAsync send VO_EVENTID_SOURCE2_SEEKCOMPLETE event.(position: %lld, ret:%d)", *pTimeStamp, ret);
	NotifyEvent( VO_EVENTID_SOURCE2_SEEKCOMPLETE, (VO_U32)pTimeStamp, (VO_U32)&ret);

	return ret;
}

VO_U32 voAdaptiveStreamingController::Seek( VO_U64* pTimeStamp)
{
	if( pTimeStamp && (*pTimeStamp != -1))
	{
		voCAutoLock lock(&m_lockSeek);

		// save DVR info since we can't get it during seeking
		if(!m_bSeeking)
			UpdateDVRInfo();

		VOLOGI("New Seek Position: %lld %lld %d", *pTimeStamp, m_nPositionSeek, m_bSeeking);
		m_nPositionSeek = *pTimeStamp;
		m_bSeeking = VO_TRUE;
	}

	return VO_RET_SOURCE2_OK;
}

VO_U32 voAdaptiveStreamingController::GetDuration( VO_U64 * pDuration)
{
/*
	if( !m_ptr_parser )
		return VO_RET_SOURCE2_NEEDRETRY;

	return m_ptr_parser->GetDuration( pDuration );
	*/
	*pDuration = m_uDuration;
	return VO_RET_SOURCE2_OK;
}

VO_U32 voAdaptiveStreamingController::GetSample( VO_SOURCE2_TRACK_TYPE nTrackType , VO_PTR pSample )
{
	{
		voCAutoLock lock(&m_lockSeek);
		if(m_bSeeking)
		{
			VOLOGW("seeking state, return need retry");
			return VO_RET_SOURCE2_NEEDRETRY;
		}
	}

	VO_SOURCE2_SAMPLE * ptr_sample = ( VO_SOURCE2_SAMPLE * )pSample;
	VO_DATASOURCE_SAMPLE sDSSample;
	sDSSample.uTime = ptr_sample->uTime;

	VO_S32 delay = 0;
	VO_U64 curtime = 0;
	VO_U64 uGetSampleTime = ptr_sample->uTime;
	VO_U32 *pGapTime = NULL;
	VO_U32 *pGapCounts = NULL;
	CIntervalPrintf *pIntervalPrintf = NULL;
	VO_SOURCE2_TRACK_INFO **ppTrackInfo = NULL;
	switch( nTrackType )
	{
	case VO_SOURCE2_TT_VIDEO:
		{
			delay = (VO_S32)ptr_sample->uDuration;
			curtime = ptr_sample->uTime;
			pGapTime = &m_uCheckedGapTime_forVideo;
			pGapCounts = &m_uCheckedGapTime_VideoCounts;
			ppTrackInfo = &m_pTrackInfoV;

			pIntervalPrintf = &m_intervalPrintf_Video;

			VOLOGR("Video wants get sample");
		}
		break;
	case VO_SOURCE2_TT_SUBTITLE:
		{
			ppTrackInfo = &m_pTrackInfoS;
			pGapTime = &m_uCheckedGapTime_forSubtitle;
			pGapCounts = &m_uCheckedGapTime_SubtitleCounts;

			pIntervalPrintf = &m_intervalPrintf_Subtitle;

			VOLOGR("Subtitle wants get sample");
		}
		break;
	case VO_SOURCE2_TT_AUDIO:
		{
			ppTrackInfo = &m_pTrackInfoA;
			pGapTime = &m_uCheckedGapTime_forAudio;
			pGapCounts = &m_uCheckedGapTime_AudioCounts;
			pIntervalPrintf = &m_intervalPrintf_Audio;

			VOLOGR("Audio wants get sample");
		}
		break;
	}
#ifndef _USE_BUFFERCOMMANDER
	VO_U32 ret = m_ptr_samplebuffer->GetBuffer( nTrackType , ( VO_PTR )&sDSSample );
#else
	VO_U32 ret = m_BufferCommander.GetSample( nTrackType , ( VO_PTR )&sDSSample );
#endif
	//getsample ok
	if( ret == VO_RET_SOURCE2_OK )
	{

		VO_U64 *pLastTimeStamp = NULL;
#if 0		
		if( sDSSample.uMarkFlag == VO_DATASOURCE_MARKFLAG_PROGRAMINFO )
		{
			_PROGRAM_INFO *pProg = (_PROGRAM_INFO*)sDSSample.pReserve2;
			m_pProgramInfoOP->SetOnTimeProgram( pProg );
			return VO_RET_SOURCE2_NEEDRETRY;
		}
#endif
	
		DSSample2Source2Sample( &sDSSample, &ptr_sample );
		if( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT )
		{
			if( *ppTrackInfo )
				ReleaseTrackInfoOP_T(*ppTrackInfo);
			*ppTrackInfo = NULL;
			CopyTrackInfoOP_EX( (_TRACK_INFO*) sDSSample.pFlagData, ppTrackInfo );
			ptr_sample->pFlagData = *ppTrackInfo;
		}
		switch (nTrackType)
		{
		case VO_SOURCE2_TT_AUDIO:
			{
				m_DVRInfo.ullPlayingTime = ptr_sample->uTime;
				pLastTimeStamp = &m_uLastAudio;
#if defined VOLOGR && defined _CHECK_DVR
				VO_SOURCE2_SEEKRANGE_INFO pDvrInfo;
				GetParam( VO_PID_SOURCE2_SEEKRANGEINFO, &pDvrInfo);
#endif	
			}
			break;
		case VO_SOURCE2_TT_VIDEO:
			{
				//if( m_b_offset )
					//curtime = curtime + m_timestamp_offset;

				if( delay > 140 )
				{
					VODS_VOLOGI( "BAINFO Video Delay: %d %lld[0x%llx]" , delay , One2Two(curtime) );
				}

				m_ptr_ba->AddVideoDelay( m_ptr_ba->hHandle , delay , curtime );

				pLastTimeStamp = &m_uLastVideo;
			}
			break;	
		case VO_SOURCE2_TT_SUBTITLE:
			{
				pLastTimeStamp = &m_uLastSubtitle;
			}
			break;
		}//endof switch
		
		if( pGapCounts )
			(*pGapCounts) ++;
		if( nTrackType == VO_SOURCE2_TT_SUBTITLE )
		{
			VOLOGI( "\t%s GetSample OK.(size:\t%d[\t0x%x], timestamp:\t%lld[\t0x%llx], LastTimeStamp:\t%lld[\t0x%llx], diff:\t%lld[\t0x%llx]) "
				, TrackType2String(nTrackType), One2Two(ptr_sample->uSize), One2Two(ptr_sample->uTime), One2Two(pLastTimeStamp ?*pLastTimeStamp:0),One2Two(pLastTimeStamp ? ptr_sample->uTime - *pLastTimeStamp:0) );
		}
		else
		{
			VOLOGR( "\t%s GetSample OK.(size:\t%d[\t0x%x], timestamp:\t%lld[\t0x%llx], LastTimeStamp:\t%lld[\t0x%llx], diff:\t%lld[\t0x%llx]) "
				, TrackType2String(nTrackType), One2Two(ptr_sample->uSize), One2Two(ptr_sample->uTime), One2Two(pLastTimeStamp ?*pLastTimeStamp:0),One2Two(pLastTimeStamp ? ptr_sample->uTime - *pLastTimeStamp:0) );
		}
		*pLastTimeStamp = ptr_sample->uTime;
		SpecialFlagCheck(nTrackType, ptr_sample->uFlag, ptr_sample->uTime, ret);
		
		//change VO_DATASOURCE_SAMPLE to VO_SOURCE2_SAMPLE 
	}//endof if( ret == VO_RET_SOURCE2_OK ) 
	

	if( pIntervalPrintf && pGapCounts)
	{
		bool b = (*pIntervalPrintf)(" %s GetSample Ret:0x%08x. (Wants timestamp:%lld[0x%llx], Result size:%d[0x%x], timestamp:%lld[0x%llx]). At this duration, %d samples has been gotten."
						, TrackType2String(nTrackType), ret, One2Two(uGetSampleTime), One2Two(ptr_sample->uSize), One2Two(ptr_sample->uTime), *pGapCounts );
		if( b )
			*pGapCounts = 0;
	}
/*	if( pGapTime && pGapCounts && voOS_GetSysTime() - *pGapTime > _CHECKEDGAPTIME )
	{
		VOLOGI(" %s GetSample Ret:0x%08x. (Wants timestamp:%lld[0x%llx], Result size:%d[0x%x], timestamp:%lld[0x%llx]). At this duration, %d samples has been gotten. It will be showed after %dms"
			, TrackType2String(nTrackType), ret, One2Two(uGetSampleTime), One2Two(ptr_sample->uSize), One2Two(ptr_sample->uTime), *pGapCounts, _CHECKEDGAPTIME );
		*pGapTime = voOS_GetSysTime();
		*pGapCounts = 0;
	}*/

#if defined VOLOGR && defined _CONTROLLER_DUMPFILE
	if(ret == VO_RET_SOURCE2_OK  )
	{
		switch( ptr_sample->uFlag )
		{
		case VO_SOURCE2_FLAG_SAMPLE_EOS:
		case VO_SOURCE2_FLAG_SAMPLE_FORCE_FLUSH:
		case VO_SOURCE2_FLAG_SAMPLE_FRAMEDROPPED:
		case VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE:
		case VO_SOURCE2_FLAG_SAMPLE_FRAMETYPEUNKNOWN:
		case VO_SOURCE2_FLAG_SAMPLE_FRAMESHOULDDROP:
		case VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE:
			return ret;
		case VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT:
			{
				if( nTrackType == VO_SOURCE2_TT_AUDIO )
					return ret;
			}
		}

		if(pDumpFile_v && nTrackType == VO_SOURCE2_TT_VIDEO )
		{
			if(ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT)
			{
				VO_SOURCE2_TRACK_INFO *pTrackInfo = ( VO_SOURCE2_TRACK_INFO* )ptr_sample->pFlagData;
				fwrite( pTrackInfo->pHeadData, 1, pTrackInfo->uHeadSize, pDumpFile_v);
			}
			else 
				fwrite( ptr_sample->pBuffer, 1, ptr_sample->uSize, pDumpFile_v);
		}
		else if(pDumpFile_a && nTrackType == VO_SOURCE2_TT_AUDIO )
		{
			fwrite( ptr_sample->pBuffer, 1, ptr_sample->uSize, pDumpFile_a);
		}
		else if(pDumpFile_a && nTrackType == VO_SOURCE2_TT_SUBTITLE )
		{
			fwrite( ptr_sample->pBuffer, 1, ptr_sample->uSize, pDumpFile_s);
		}
	}
#endif
	return ret;
}

VO_U32 voAdaptiveStreamingController::GetProgramCount( VO_U32 *pProgramCount)
{
/*
	if( !m_ptr_parser )
		return VO_RET_SOURCE2_NEEDRETRY;

	return m_ptr_parser->GetProgramCount( pProgramCount );
*/
	// now the program count is always be 1
	*pProgramCount = 1;
	return VO_RET_SOURCE2_OK;
}

VO_U32 voAdaptiveStreamingController::GetProgramInfo( VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **ppProgramInfo)
{
	if( !m_ptr_parser )
		return VO_RET_SOURCE2_NEEDRETRY;

	// m_ptr_parser->GetProgramInfo( nProgram , ppProgramInfo );
	// CopyProgramInfo(*ppProgramInfo, &m_pProgramInfo );
	
	VO_U32 ret = m_pProgramInfoOP->GetSource2ProgramInfo( 0, ppProgramInfo );
	if( ret == VO_RET_SOURCE2_FAIL )
		return VO_RET_SOURCE2_NEEDRETRY;
	else
		return VO_RET_SOURCE2_OK;
	//return m_ptr_parser->GetProgramInfo( nProgram , ppProgramInfo );
}

VO_U32 voAdaptiveStreamingController::GetCurTrackInfo(  VO_SOURCE2_TRACK_TYPE nTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo )
{
#ifndef _new_programinfo
	if( !m_ptr_parser )
		return VO_RET_SOURCE2_NEEDRETRY;

	return m_ptr_parser->GetCurTrackInfo( nTrackType , ppTrackInfo );
#else
	return VO_RET_SOURCE2_NOIMPLEMENT;
#endif
	
}

VO_U32 voAdaptiveStreamingController::SelectProgram( VO_U32 nProgram)
{
	if( !m_ptr_parser )
		return VO_RET_SOURCE2_NEEDRETRY;

	return m_ptr_parser->SelectProgram( nProgram );
}

VO_U32 voAdaptiveStreamingController::SelectStream( VO_U32 nStream)
{
	VODS_VOLOGI("+SelectStream. nStream: %d", nStream);
// 	if( !m_ptr_parser )
// 		return VO_RET_SOURCE2_NEEDRETRY;

	if( nStream == _VODS_INT32_MAX )
	{
		m_enableBA = VO_TRUE;

		return VO_RET_SOURCE2_OK;
	}

	VO_U32 changebitrate = _VODS_INT32_MAX;

	_PROGRAM_INFO * ptr_program_info = 0;
#ifdef _use_programinfo2
	m_ptr_parser->GetProgramInfo( 0 , &ptr_program_info );
#else
	m_pProgramInfoOP->GetProgramInfo( 0, &ptr_program_info);
#endif
	if(ptr_program_info && ptr_program_info->uStreamCount > 0 )
	{
		for( VO_U32 i = 0 ; i < ptr_program_info->uStreamCount ; i++ )
		{
			_STREAM_INFO * ptr_info = ptr_program_info->ppStreamInfo[i];

			if( ptr_info->uStreamID == nStream )
			{
				changebitrate = m_SelBitrate = ptr_info->uBitrate;
				m_enableBA = VO_FALSE;
				break;
			}
		}
	}

	if( changebitrate == _VODS_INT32_MAX )
		m_enableBA = VO_TRUE;

	VODS_VOLOGI("-SelectStream.EnableBA:%d.",m_enableBA);

	return VO_RET_SOURCE2_OK;
}

VO_U32 voAdaptiveStreamingController::SelectTrack( VO_U32 nTrack)
{
	VODS_VOLOGI("+ SelectTrack. nTrack: %d", nTrack);
	if( !m_ptr_parser )
		return VO_RET_SOURCE2_NEEDRETRY;

	//VO_U32 ret = m_ptr_parser->SelectTrack( nTrack );
	
	voCAutoLock lock( &m_SelTrackLock );

	_TRACK_INFO * pTrackInfo = NULL;

	VO_U32 ret = VO_RET_SOURCE2_OK;
#ifdef _new_programinfo
	VO_U32 ret1 = m_pProgramInfoOP->GetCurTrackInfo(VO_SOURCE2_TT_AUDIO, &pTrackInfo );
	if( ret1 == VO_RET_SOURCE2_OK && pTrackInfo && pTrackInfo->uOutSideTrackID == nTrack )
		ret = VO_RET_SOURCE2_NOIMPLEMENT;
	ret1 = m_pProgramInfoOP->GetCurTrackInfo(VO_SOURCE2_TT_VIDEO, &pTrackInfo );
	if( ret1 == VO_RET_SOURCE2_OK && pTrackInfo && pTrackInfo->uOutSideTrackID == nTrack )
		ret =  VO_RET_SOURCE2_NOIMPLEMENT;
	ret1 = m_pProgramInfoOP->GetCurTrackInfo(VO_SOURCE2_TT_SUBTITLE, &pTrackInfo );
	if( ret1 == VO_RET_SOURCE2_OK && pTrackInfo && pTrackInfo->uOutSideTrackID == nTrack )
		ret =  VO_RET_SOURCE2_NOIMPLEMENT;	
#else
	VO_U32 ret1 = m_ptr_parser->GetCurTrackInfo(VO_SOURCE2_TT_AUDIO, &pTrackInfo );
	if( ret1 == VO_RET_SOURCE2_OK && pTrackInfo && pTrackInfo->uTrackID == nTrack )
		ret = VO_RET_SOURCE2_NOIMPLEMENT;
	ret1 = m_ptr_parser->GetCurTrackInfo(VO_SOURCE2_TT_VIDEO, &pTrackInfo );
	if( ret1 == VO_RET_SOURCE2_OK && pTrackInfo && pTrackInfo->uTrackID == nTrack )
		ret =  VO_RET_SOURCE2_NOIMPLEMENT;
	ret1 = m_ptr_parser->GetCurTrackInfo(VO_SOURCE2_TT_SUBTITLE, &pTrackInfo );
	if( ret1 == VO_RET_SOURCE2_OK && pTrackInfo && pTrackInfo->uTrackID == nTrack )
		ret =  VO_RET_SOURCE2_NOIMPLEMENT;	
#endif

	if( ret ==  VO_RET_SOURCE2_OK )
		m_SelTrackList.push_back( nTrack );

	VODS_VOLOGI("- SelectTrack. nTrack: %d. Ret:0x%08x", nTrack, ret);

	return ret;
}

VO_U32 voAdaptiveStreamingController::GetDRMInfo( VO_SOURCE2_DRM_INFO **ppDRMInfo)
{
	VODS_VOLOGI("+GetDRMInfo. VO_SOURCE2_DRM_INFO: %p", *ppDRMInfo);
	if( !m_ptr_parser )
		return VO_RET_SOURCE2_NEEDRETRY;

	VO_U32 ret = m_ptr_parser->GetDRMInfo( ppDRMInfo );
	VODS_VOLOGI("-GetDRMInfo. VO_SOURCE2_DRM_INFO: %p", *ppDRMInfo);

	return ret;
}

VO_U32 voAdaptiveStreamingController::SendBuffer( const VO_SOURCE2_SAMPLE& buffer )
{
	return VO_RET_SOURCE2_NOIMPLEMENT;
}

VO_U32 voAdaptiveStreamingController::GetParam( VO_U32 nParamID, VO_PTR pParam)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	switch ( nParamID )
	{
	case VO_PID_SOURCE2_SEEKRANGEINFO:
		{
			// protect VO_PID_SOURCE2_SEEKRANGEINFO to avoid ANR, East 20130428
			if(!m_bOpenComplete)
			{
				VOLOGW("open not complete, VO_PID_SOURCE2_SEEKRANGEINFO return need retry");
				return VO_RET_SOURCE2_NEEDRETRY;
			}

			voCAutoLock lock(&m_lockSeek);
			if(m_bSeeking)
			{
				VOLOGI("seeking state, VO_PID_SOURCE2_SEEKRANGEINFO return saved DVR information");
				ret = VO_RET_SOURCE2_OK;
			}
			else
				ret = UpdateDVRInfo();
				
			if(VO_RET_SOURCE2_OK == ret)
			{
				VO_SOURCE2_SEEKRANGE_INFO * pDVRInfo = (VO_SOURCE2_SEEKRANGE_INFO *)pParam;
				if(pDVRInfo)
				{
					pDVRInfo->ullEndTime = m_DVRInfo.ullEndTime;
					pDVRInfo->ullStartTime = m_DVRInfo.ullStartTime;
					pDVRInfo->ullLiveTime = m_DVRInfo.ullLiveTime;
					pDVRInfo->ullPlayingTime = m_DVRInfo.ullPlayingTime;
					pDVRInfo->pReserve1 = m_DVRInfo.pReserve1;
					pDVRInfo->pReserve2 = m_DVRInfo.pReserve2;
				}
			}
		}
		break;

	case VO_PID_SOURCE2_BA_WORKMODE:
		{
			VO_SOURCE2_BAMODE *pBAMode = (VO_SOURCE2_BAMODE *)pParam;
			if( pBAMode )
				*pBAMode = m_enableBA ? VO_SOURCE2_ADAPTIVESTREAMING_BAMODE_AUTO : VO_SOURCE2_ADAPTIVESTREAMING_BAMODE_MANUAL ;
		}
		break;

	default:
		ret = VO_RET_SOURCE2_NOIMPLEMENT;
		break;
	}

	return ret;
}

VO_U32 voAdaptiveStreamingController::SetParam( VO_U32 nParamID, VO_PTR pParam)
{
	if( !pParam) 
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VODS_VOLOGI("+ SetParam: (ID:0x%08x, Param:%p) ", nParamID, pParam);
	VO_U32 ret = VO_RET_SOURCE2_OK;
	switch(nParamID)
	{
	case VO_PID_SOURCE2_WORKPATH:
		{
			VODS_VOLOGI("SetWork Path: %s ", (VO_TCHAR *)pParam );
			m_info.SetWorkPath( (VO_TCHAR*)pParam);
		}
		break;
	case VO_PID_SOURCE2_LIBOP:
		{
			VODS_VOLOGI("SetLibOp : %p ", pParam );
			m_info.SetLibOp( ( VO_SOURCE2_LIB_FUNC * )pParam );
		}
		break;
	case VO_PID_COMMON_LOGFUNC:
		{
			m_pVologCB = (VO_LOG_PRINT_CB *)pParam;
			//vologInit (m_pVologCB->pUserData, m_pVologCB->fCallBack);
			m_info.SetLogFunc( m_pVologCB );
		}
		break;
	case VO_PID_SOURCE2_DRMCALLBACK:
		{
			VODS_VOLOGI("Set DRMCallback %p", pParam);
			m_ptr_drmcallback = (VO_SOURCEDRM_CALLBACK2 *)pParam;
		}
		break;
	case VO_PID_SOURCE2_EVENTCALLBACK:
		{
			m_ptr_eventcallback = ( VO_SOURCE2_EVENTCALLBACK * )pParam;
		}
		break;
	case VO_PID_SOURCE2_BACAP:
		{
			VO_SOURCE2_CAP_DATA* pCapInfo = (VO_SOURCE2_CAP_DATA*)pParam;
			VO_U32 m_cap = pCapInfo->nBitRate;
			ret = m_ptr_ba->SetParam( m_ptr_ba->hHandle , VO_PID_SOURCE2_BA_CAP , &m_cap );
			VODS_VOLOGI( "Cap: %d" , m_cap );
		}
		break;
	case VO_PID_SOURCE2_BA_STARTCAP:
		{
			VO_SOURCE2_CAP_DATA* pCapInfo = (VO_SOURCE2_CAP_DATA*)pParam;
			VO_U32 m_nStartCap = pCapInfo->nBitRate;
			ret = m_ptr_ba->SetParam( m_ptr_ba->hHandle , VO_PID_SOURCE2_BA_START_CAP , &m_nStartCap );
			VODS_VOLOGI("StartCap:%d ", m_nStartCap );
		}
		break;
	case VO_PID_SOURCE2_DOHTTPVERIFICATION:
		{
			VODS_VOLOGI( "VO_PID_SOURCE2_DOHTTPVERIFICATION" );
			m_info.SetVerificationInfo( ( VO_SOURCE2_VERIFICATIONINFO * )pParam );
		}
		break;
	case VO_PID_SOURCE2_BUFFER_STARTBUFFERINGTIME:
		{
#ifndef _USE_BUFFERCOMMANDER
			m_nStartBufferTime = *((VO_U32 *) pParam);
			SetBufferTime(m_ptr_samplebuffer, VO_BUFFER_SETTING_STARTBUFFERTIME, m_nStartBufferTime, m_hasSetStartBufferTime);
#else
			m_BufferCommander.SetParam(VO_PID_SOURCE2_BUFFER_STARTBUFFERINGTIME, pParam);
			m_hasSetStartBufferTime = VO_TRUE;
#endif
		}
		break;
	case VO_PID_SOURCE2_BUFFER_BUFFERINGTIME:
		{
#ifndef _USE_BUFFERCOMMANDER
			m_nBufferingTime = *((VO_U32 *) pParam);
			SetBufferTime(m_ptr_samplebuffer, VO_BUFFER_SETTING_BUFFERTIME, m_nBufferingTime, m_hasSetBufferingTime);
#else
			m_BufferCommander.SetParam(VO_PID_SOURCE2_BUFFER_BUFFERINGTIME, pParam);
			m_hasSetBufferingTime = VO_TRUE;
#endif
		}
		break;
	case VO_PID_SOURCE2_BA_WORKMODE:
		{
			VO_SOURCE2_BAMODE *bamode = (VO_SOURCE2_BAMODE *)pParam;
			switch( *bamode)
			{
			case VO_SOURCE2_ADAPTIVESTREAMING_BAMODE_AUTO:
				m_enableBA = VO_TRUE;
				break;
			case VO_SOURCE2_ADAPTIVESTREAMING_BAMODE_MANUAL:
				m_enableBA = VO_FALSE;
				break;
			default: 
				m_enableBA = VO_TRUE;
				break;
			}
		}
		break;
	case VO_PID_SOURCE2_SUBTITLELANGUAGE:
		{
			if(m_pProgramInfoOP)
				m_pProgramInfoOP->SetLanguage(VO_SOURCE2_TT_SUBTITLE, (VO_CHAR*)pParam);
		}
		break;
	case VO_PID_SOURCE2_AUDIOLANGUAGE:
		{
			if(m_pProgramInfoOP)
				m_pProgramInfoOP->SetLanguage(VO_SOURCE2_TT_AUDIO, (VO_CHAR*)pParam);
		}
		break;
	case VO_PID_SOURCE2_BUFFER_MAXBUFFERSIZE:
		{
#ifndef _USE_BUFFERCOMMANDER
			if(m_ptr_samplebuffer)
				m_ptr_samplebuffer->SetParameter( VO_BUFFER_PID_MAX_SIZE, pParam);
#else
			m_BufferCommander.SetParam(VO_PID_SOURCE2_BUFFER_MAXBUFFERSIZE, pParam);
#endif
		}
		break;
	case VO_PID_SOURCE2_BA_CPUBADISABLE:
		{
			m_DisableCpuBA = *((VO_BOOL*)pParam);
			if( m_ptr_ba && m_ptr_ba->hHandle )
				m_ptr_ba->SetParam(m_ptr_ba->hHandle, VO_PID_SOURCE2_BA_DISABLECPUBA, pParam);
		}
		break;
	case VO_PID_SOURCE2_HTTPHEADER:
		{
			m_pIOHttpHeader = (VO_SOURCE2_HTTPHEADER *)pParam;
			if(m_ptr_io)
				m_ptr_io->SetParam(0, VO_SOURCE2_IO_PARAMID_HTTPHEADINFO,pParam );
		}
		break;
	case VO_PID_SOURCE2_HTTPPROXYINFO:
		{
			m_pIOHttpProxy = (VO_SOURCE2_HTTPPROXY *)pParam;
			if(m_ptr_io)
				m_ptr_io->SetParam(0, VO_SOURCE2_IO_PARAMID_HTTPPROXYINFO,pParam );
		}
		break;
	case VO_PID_SOURCE2_IO_CBFUNC:
		{
			m_info.SetIOHttpCallback( (VO_SOURCE2_IO_HTTPCALLBACK*)pParam);
		}
		break;
	case VO_PID_SOURCE2_LIVELATENCY:
		{
			m_nLiveDelayTime = *((VO_U64 *) pParam);
		}
		break;
	case VO_PID_SOURCE2_PERIOD2TIME:
		{
			if(m_ptr_parser)
			{
				m_ptr_parser->SetParam(VO_PID_ADAPTIVESTREAMING_PERIOD2TIME, pParam);
				VOLOGI("period 2 time,period is %d,time is %lld",((VO_SOURCE2_PERIODTIMEINFO*)pParam)->uPeriodSequenceNumber,((VO_SOURCE2_PERIODTIMEINFO*)pParam)->ullTimeStamp);
			}
		}
		break;
	case VO_PID_SOURCE2_APPLICATION_SUSPEND:
		{
			VOLOGI("VO_PID_SOURCE2_APPLICATION_SUSPEND in");
			if(m_eventHandler_api.SetParameter)
			{
				m_eventHandler_api.SetParameter(m_eventHandler_api.hHandle, VO_PID_SOURCE2_APPLICATION_SUSPEND,pParam);
			}
		}
		break;
	case VO_PID_SOURCE2_APPLICATION_RESUME:
		{
			VOLOGI("VO_PID_SOURCE2_APPLICATION_RESUME in");
			if(m_eventHandler_api.SetParameter)
			{
				m_eventHandler_api.SetParameter(m_eventHandler_api.hHandle, VO_PID_SOURCE2_APPLICATION_RESUME,pParam);
			}
		}
		break;
	case VO_PID_SOURCE2_PLAYBACK_SPEED:
		{
			if( m_ptr_ba && m_ptr_ba->hHandle )
				m_ptr_ba->SetParam(m_ptr_ba->hHandle, VO_PID_SOURCE2_BA_PLAYBACK_SPEED, pParam);
		}
		break;
	case VO_PID_SOURCE2_TOLERANT_ERROR_NUMBER:
		{	
			m_TolerantErrorCount = *(VO_U32*)pParam;
		}
		break;
	case VO_PID_SOURCE2_BITRATE_THRESHOLD:
		{
			VO_SOURCE2_BA_THRESHOLD* tmp = (VO_SOURCE2_BA_THRESHOLD*)pParam;
			VOLOGI("Bitrate threshold upper is %d, lower is %d",tmp->nUpper,tmp->nLower);
			if(tmp->nUpper > 0)
			{
				m_ba_threshold.nUpper = tmp->nUpper;
			}
			else
			{
				m_ba_threshold.nUpper = 0xffffffff;
			}
			if(tmp->nLower > 0)
			{
				m_ba_threshold.nLower = tmp->nLower;
			}
			else
			{
				m_ba_threshold.nLower = 0;
			}
			if(m_ba_threshold.nLower >= m_ba_threshold.nUpper)
			{
				VOLOGW("Bitrate Threshold set failed");
				m_ba_threshold.nUpper = 0xffffffff;
				m_ba_threshold.nLower = 0;
				ret = VO_RET_SOURCE2_INVALIDPARAM;
				break;
			}
			ret = m_ptr_ba->SetParam(m_ptr_ba->hHandle, VO_PID_SOURCE2_BA_THRESHOLD,&m_ba_threshold);
		}
		break;
	case VO_PID_SOURCE2_UPDATE_SOURCE_URL:
		{
			VOLOGI("Update url entering!");
			if(m_streaming_type != VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS)
			{
				VOLOGW("Update url only for hls now");
				return VO_RET_SOURCE2_NOIMPLEMENT;
			}
			if(!m_bOpenComplete)
			{
				VOLOGW("Update url should be called after open for now");
				return VO_RET_SOURCE2_NOIMPLEMENT;
			}
			if(m_bUpdatingURL)
			{
				VOLOGW("URL is under updating, ignore this call");
				return VO_RET_SOURCE2_NEEDRETRY;
			}
			VO_PTR pSource = pParam;
			{
				voCAutoLock lock(&m_lockURL);
				memset(m_tmp_update_url, 0x00, sizeof(m_tmp_update_url) );
#if defined _UNICODE || defined UNICODE
				int size = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pSource, -1, NULL, NULL, NULL, NULL );
				WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pSource, -1, m_tmp_update_url, size, NULL, NULL );
#else
				strcpy( m_tmp_update_url , (VO_CHAR*)pSource );
#endif
			}
			m_bUpdatingURL = VO_TRUE;
			ret = UpdateUrl();
		}
		break;
	case VO_PID_SOURCE2_HTTP_RETRY_TIMEOUT:
		{
			m_RetryTimeOut = *(VO_U32*)pParam;
		}
		break;
	case VO_PID_SOURCE2_SOURCE_IO_API:
		{
			VO_SOURCE2_IO_API* tmp = (VO_SOURCE2_IO_API*)pParam;
			if(tmp)
			{
				m_ptr_io = tmp;
				m_bOutSideSourceIO = VO_TRUE;
				VOLOGI("Set IO Func Pointer In");
			}
		}
	default:
		ret = VO_RET_SOURCE2_NOIMPLEMENT;
		break;
	}

	VODS_VOLOGI("- SetParam: (ID:0x%08x, Param:%p). Ret:0x%08x ", nParamID, pParam, ret);

	return ret;
}


VO_U32 voAdaptiveStreamingController::CheckStreamingType( VO_CHAR * ptr_buffer , VO_U32 size )
{
	// for UTF-8 text, 0xEF 0xBB 0xBF will at the head, East 20130409
	char * pFind = strstr(ptr_buffer, "#EXTM3U");
	if(pFind && ((pFind == ptr_buffer) || (pFind == ptr_buffer + 3 && (VO_BYTE)ptr_buffer[0] == 0xEF && (VO_BYTE)ptr_buffer[1] == 0xBB && (VO_BYTE)ptr_buffer[2] == 0xBF)))
		m_streaming_type = VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS;
	else if( CheckString( ptr_buffer , size, "<SmoothStreamingMedia" ) )
		m_streaming_type = VO_ADAPTIVESTREAMPARSER_STREAMTYPE_ISS;
	else if( CheckString( ptr_buffer , size, "<MPD" ) )
		m_streaming_type = VO_ADAPTIVESTREAMPARSER_STREAMTYPE_DASH;
	else 
		m_streaming_type = VO_ADAPTIVESTREAMPARSER_STREAMTYPE_UNKOWN;

	VODS_VOLOGI(" StreamType :%x", m_streaming_type );
	m_info.SetStreamingType( m_streaming_type );
	if( m_streaming_type == VO_ADAPTIVESTREAMPARSER_STREAMTYPE_UNKOWN )
		return VO_RET_SOURCE2_NOIMPLEMENT;
	return VO_RET_SOURCE2_OK;
}

VO_U32 voAdaptiveStreamingController::CheckStreamingType( VO_CHAR * ptr_buffer , VO_U32 size ,VO_ADAPTIVESTREAMPARSER_STREAMTYPE * stream_type)
{
	char * pFind = strstr(ptr_buffer, "#EXTM3U");
	if(pFind && ((pFind == ptr_buffer) || (pFind == ptr_buffer + 3 && (VO_BYTE)ptr_buffer[0] == 0xEF && (VO_BYTE)ptr_buffer[1] == 0xBB && (VO_BYTE)ptr_buffer[2] == 0xBF)))
		* stream_type = VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS;
	else if( CheckString( ptr_buffer , size, "<SmoothStreamingMedia" ) )
		* stream_type = VO_ADAPTIVESTREAMPARSER_STREAMTYPE_ISS;
	else if( CheckString( ptr_buffer , size, "<MPD" ) )
		* stream_type = VO_ADAPTIVESTREAMPARSER_STREAMTYPE_DASH;
	else 
		* stream_type = VO_ADAPTIVESTREAMPARSER_STREAMTYPE_UNKOWN;

	VODS_VOLOGI(" StreamType :%x", m_streaming_type );
	return VO_RET_SOURCE2_OK;
}

VO_S32 voAdaptiveStreamingController::SendEvent( VO_U32 eventType, VO_U32 eventInfo, VO_ADAPTIVESTREAM_PLAYLISTDATA* pData )
{
	VO_SOURCE2_ADAPTIVESTREAMING_PLAYLISTDATA playlist;
	memset( &playlist, 0x00, sizeof(VO_SOURCE2_ADAPTIVESTREAMING_PLAYLISTDATA) );
	if( pData )
	{
		memcpy( playlist.szUrl, pData->szUrl, MAXURLLEN);
		memcpy( playlist.szNewUrl, pData->szNewUrl, MAXURLLEN);
		memcpy( playlist.szRootUrl, pData->szRootUrl, MAXURLLEN);
		playlist.uDataSize = 0;
		playlist.pData = NULL;
	}
	else
	{
		memcpy( playlist.szUrl, m_url, MAXURLLEN);
	}
	if(m_ptr_io)
		playlist.uReserved1 = m_ptr_io->GetLastError( m_ptr_io->hHandle );
	return OnEvent(this, eventType, eventInfo, ( VO_U32 )&playlist);
}

VO_S32 voAdaptiveStreamingController::OnEvent(VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )pUserData;
	if(!ptr_obj )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VOLOGI("+ OnEvent. ShallBeStop:%d. pUserData:%p, nID:%d, nParam1:%p, nParam2:%p",ptr_obj->m_is_stop, pUserData, nID, nParam1, nParam2);

	VO_U32 ret = VO_RET_SOURCE2_OK;

	/*if STOP has been called, no event shall be callback*/
	if(ptr_obj->m_is_stop && CONTROLLER_STATUS_STOP == ptr_obj->m_status)
		return VO_RET_SOURCE2_OK;

	switch( nID )
	{
	case VO_EVENTID_SOURCE2_OPENCOMPLETE:
		{
			VODS_VOLOGI( " + Parser Open Complete!" );
			ret = ptr_obj->OpenComplete();

			VODS_VOLOGI( " - Parser Open Complete!.Ret: 0x%08x", ret);
		}
		break;

	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_NEEDPARSEITEM:
		{

			VO_ADAPTIVESTREAM_PLAYLISTDATA * pData = ( VO_ADAPTIVESTREAM_PLAYLISTDATA * )nParam1;
			if(pData )
			{
				VODS_VOLOGI( "+Begin Parse: %s" , pData->szUrl );

//				ret = DownloadItem_II(ptr_obj->m_ptr_io, ptr_obj->m_info.GetVerificationInfo(), ptr_obj->m_info.GetIOHttpCallback(),pData
//					, &ptr_obj->m_pPlaylistData, &ptr_obj->m_PlaylistDataSize, &ptr_obj->m_PlaylistDataUsedSize, &ptr_obj->m_is_stop);

				ret = DownloadItem_III(ptr_obj->m_ptr_io, ptr_obj->m_info.GetVerificationInfo(), ptr_obj->m_info.GetIOHttpCallback(),pData, &ptr_obj->m_is_stop);
				if( ret == VO_RET_SOURCE2_OK)
				{	
					ret = ptr_obj->PreProcessURL( pData );
#ifdef DUMP_MANIFEST
					if(ptr_obj->m_bFirstSubManifest)
					{
						FILE* fp = fopen("/sdcard/CBSi/sub.m3u8","wb");
						if(fp)
						{
							fwrite(pData->pData,1,pData->uDataSize,fp);
							fclose(fp);
							ptr_obj->m_bFirstSubManifest = VO_FALSE;
						}
					}
#endif			
					ptr_obj->SendEvent( VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_INFO, VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_PLAYLIST_DOWNLOADOK, pData);
				}
				else
				{
					ptr_obj->SendEvent( VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_WARNING, VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_PLAYLIST_DOWNLOADERROR, pData);
				}
			}
			VODS_VOLOGI( "-End Parser! 0x%08x" , ret );
		}
		break;
	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_DOWNLOADSTARTCOLLECTOR:
		{
			CHUNKINFO * pInfo = ( CHUNKINFO * )nParam1;
			if( pInfo )
				ret = ptr_obj->DownloadStartColletor( pInfo);
		}
		break;
	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_DOWNLOADENDCOLLECTOR:
		{
			CHUNKINFO * pInfo = ( CHUNKINFO * )nParam1;
			if( pInfo )
			{
				*((VO_U64*)nParam2) = ptr_obj->DownloadEndColletor( pInfo);
			}
		}
		break;
	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CHUNKBEGIN:
		{
			
			CHUNKINFO * pInfo = ( CHUNKINFO * )nParam1;
			if( pInfo )
			{
				//check chunktype, marked VO_SOURCE2_ADAPTIVESTREAMING_UNKNOWN to skip begin & complete
				if( pInfo->chunktype < VO_SOURCE2_ADAPTIVESTREAMING_UNKNOWN)
					ret = ptr_obj->ChunkBegin( pInfo, VO_BOOL(nParam2 & VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE));
#ifdef _USE_BUFFERCOMMANDER			
				ptr_obj->m_BufferCommander.ChunkAgentInit(pInfo->ppChunkAgent, pInfo->uASTrackID, pInfo->bNeedSmoothSwitch);
#endif
			}
		}
		break;

	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CHUNKCOMPLETE:
		{
			CHUNKINFO * pInfo = ( CHUNKINFO * )nParam1;
			if( pInfo )
			{
				VO_U32 ret1 = 0;
				CHUNK_STATUS nSwitch = SWITCH_MAX;
				if(pInfo->bNeedSwitchAudioThread)
				{
					ptr_obj->m_bNeedSwitchAudioThread = pInfo->bNeedSwitchAudioThread;
				}
				//check chunktype, marked VO_SOURCE2_ADAPTIVESTREAMING_UNKNOWN to skip begin & complete
				if( pInfo->chunktype < VO_SOURCE2_ADAPTIVESTREAMING_UNKNOWN)
					ret1 = ptr_obj->ChunkComplete( pInfo );	
				if( pInfo->chunktype < VO_SOURCE2_ADAPTIVESTREAMING_UNKNOWN)
				{
					if(pInfo->chunktype != VO_SOURCE2_ADAPTIVESTREAMING_AUDIO &&VO_RET_SOURCE2_BA_BITRATECHANGED == ret1)
						nSwitch = SWITCH_TRUE;
					else
						nSwitch = SWITCH_FALSE;
				}
				else
				{
					nSwitch = SWITCH_IGNORE;
				}
#ifdef _USE_BUFFERCOMMANDER		
				//2013/08/29 Leon. audio chunk should not do BA. pure audio ---> AV( at separated audio & video case)
				ptr_obj->m_BufferCommander.ChunkAgentUninit(*pInfo->ppChunkAgent, pInfo->uASTrackID, nSwitch);
				ret = VO_RET_SOURCE2_OK;
#endif
			}
		}
		break;

	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CHUNKEOS:
		{
			THREADTYPE * ptr = ( THREADTYPE * )nParam1;
			if( ptr )
			{
				switch( *ptr )
				{
				case THREADTYPE_MEDIA_AUDIO:
					ptr_obj->m_audio_thread_eos = VO_TRUE;
					break;
				case THREADTYPE_MEDIA_VIDEO:
					ptr_obj->m_video_thread_eos = VO_TRUE;
					break;
				}

				if( ptr_obj->m_audio_thread_eos && ptr_obj->m_video_thread_eos )
				{
					ptr_obj->m_contain_video = VO_TRUE;

					_SAMPLE eos;
					memset( &eos , 0 , sizeof( _SAMPLE ) );
					eos.uFlag = VO_SOURCE2_FLAG_SAMPLE_EOS;	
#ifdef _USE_BUFFERCOMMANDER
					eos.uDataSource_Flag = VO_DATASOURCE_FLAG_EOS;
#endif
#ifndef _USE_BUFFERCOMMANDER
					OnSample( pUserData , VO_SOURCE2_TT_VIDEO , &eos );
					OnSample( pUserData , VO_SOURCE2_TT_AUDIO , &eos );
#else
					eos.uFPTrackID = _VODS_INT32_MAX;
					eos.uASTrackID = _VODS_INT32_MAX;
					OnSample( pUserData,(VO_U16)VO_SOURCE2_TT_MAX, &eos);
#endif					
				}
			}
		}
		break;
	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CHUNKTIMEREVISE:
		{
			VO_SOURCE2_CHUNK_SAMPLE* pSample = (VO_SOURCE2_CHUNK_SAMPLE*)nParam1;
			if(pSample)
			{
				VO_U64 *pModifiedTime = pSample->pullRetTimeStamp;
				VOLOGI("Timestamp modification,Modified Time is %lld",*pModifiedTime);
				ptr_obj->m_TimeStampRecalculator.seek(*pModifiedTime);
			}
		}
		break;
	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_PROGRAMCHANGED:
		{
 			VODS_VOLOGI(" Receive Program Changed EVENT!");

 			VO_SOURCE2_PROGRAM_INFO *pProgInfo = ( VO_SOURCE2_PROGRAM_INFO* ) nParam1;
 			if( pProgInfo )
 			{
 				ptr_obj->m_pProgramInfoOP->SetProgramType( pProgInfo->sProgramType );
				ptr_obj->m_info.SetProgramType( pProgInfo->sProgramType );
			}
#ifndef _USE_BUFFERCOMMANDER
			if(pProgInfo)
			{
				VO_BUFFER_SETTING_SAMPLE setting;
				VO_BUFFERING_STYLE_TYPE type;
				setting.nType = VO_BUFFER_SETTING_BUFFERING_STYLE;
				if(pProgInfo->sProgramType == VO_SOURCE2_STREAM_TYPE_VOD)
				{
					 type = VO_BUFFERING_AV;
				}
				else
				{
					type = VO_BUFFERING_A;
				}
				setting.pObj = &type;
				VOLOGI("set buffer type %d",type);
				ptr_obj->m_ptr_samplebuffer->AddBuffer( VO_BUFFER_SETTINGS , &setting );
 			}
#else
			VO_SOURCE2_PROGRAM_TYPE nType = VO_SOURCE2_STREAM_TYPE_UNKNOWN;
			if(pProgInfo )
				nType = pProgInfo->sProgramType;
			ptr_obj->m_BufferCommander.SetParam(VO_PID_SOURCE2_PROGRAM_TYPE, &nType);
#endif
 			
 		//	ptr_obj->NotifyEvent( VO_EVENTID_SOURCE2_PROGRAMCHANGED, 0, 0);
		}
		break;

	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_PROGRAMRESET:
		{
			VODS_VOLOGI(" Receive Program Reset EVENT!");

			_PROGRAM_INFO *pProgInfo = ( _PROGRAM_INFO* ) nParam1;
			if( pProgInfo )
			{
				ptr_obj->m_pProgramInfoOP->ResetProgramInfo( pProgInfo );
				ret = ptr_obj->NotifyEvent( VO_EVENTID_SOURCE2_PROGRAMRESET, 0, 0);
			}
		}
		break;

	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_SEEK2LASTCHUNK:
		{
			VODS_VOLOGI(" Receive Seek To Last Chunk EVENT %d!", ptr_obj->m_bSeekingBySelectTrack);

			if(ptr_obj->m_bSeekingBySelectTrack)	// if this event is caused by Select Track, block it, East 20130618
				ret = VO_RET_SOURCE2_OK;
			else
				ret = ptr_obj->NotifyEvent(VO_EVENTID_SOURCE2_SEEK2LASTCHUNK, 0, 0);
		}
		break;

	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_WARNING:
	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_INFO:
	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_ERROR:
	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CUSTOMTAG:
		{
			ret = ptr_obj->m_eventHandler_api.CheckEvent( ptr_obj->m_eventHandler_api.hHandle, nID, nParam1, nParam2, NULL);
			if(nID == VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_WARNING && ret == VO_RET_SOURCE2_ONELINKFAIL)
			{
				VO_U32 ret1 = ptr_obj->ToChangeBitrateWhenError();
					if(ret1 == VO_RET_SOURCE2_BA_BITRATECHANGED)
						ptr_obj->m_info.SetBitrateChanging(VO_TRUE);
			}
				
		}
		break;

	case VO_DATASOURCE_EVENTID_POP_NEWPROGRAM:
		{
		//	ptr_obj->AddProgramInfo2Buffer( (_PROGRAM_INFO*)nParam1 );
			ret = VO_RET_SOURCE2_OK;
		}
		break;

	case VO_DATASOURCE_EVENTID_POP_PROGRAMINFOCHANGED:
		{
			VOLOGI("VO_EVENTID_SOURCE2_PROGRAMCHANGED");
			//this must a async event
			ret = ptr_obj->NotifyEvent( VO_EVENTID_SOURCE2_PROGRAMCHANGED, 0, 0 );
		}
		break;

	case VO_DATASOURCE_EVENTID_POP_PROGRAMINFORESET:
		{
			VOLOGI("VO_EVENTID_SOURCE2_PROGRAMRESET");
			//this must a async event
			ret = ptr_obj->NotifyEvent( VO_EVENTID_SOURCE2_PROGRAMRESET, 0, 0);
		}
		break;
	case VO_DATASOURCE_EVENTID_IO_DOWNLOADSLOW:
		{
			VOLOGI("VO_DATASOURCE_EVENTID_IO_DOWNLOADSLOW");
			ptr_obj->m_BufferCommander.GetParam(VO_BUFFER_PID_EFFECTIVE_DURATION,(VO_PTR)&(ptr_obj->m_nBufferDuration));
			ptr_obj->m_ptr_ba->SetParam( ptr_obj->m_ptr_ba->hHandle , VO_PID_SOURCE2_BA_BUFFER_DURATION , &ptr_obj->m_nBufferDuration );
			ret = ptr_obj->m_ptr_ba->CheckDownloadSlow(ptr_obj->m_ptr_ba->hHandle, nParam1);
		}
		break;
	default:
		ret = VO_RET_SOURCE2_NOIMPLEMENT;
		break;
	}

	VOLOGI("- OnEvent.[Result:0x%08x] pUserData:%p, nID:%d, nParam1:%p, nParam2:%p", ret,pUserData, nID, nParam1, nParam2);

	return ret;
}


#ifdef _USE_BUFFERCOMMANDER
VO_S32 voAdaptiveStreamingController::OnSample(VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData)
{

	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )pUserData;


	VO_U32 ret = VO_RET_SOURCE2_NEEDRETRY;

	_SAMPLE * pSample = ( _SAMPLE * )pData;
	
	if(nOutputType == VO_SOURCE2_TT_CUSTOMTIMEDTAG)
	{
		ptr_obj->m_TimeStampRecalculator.recalculate( (VO_U32)nOutputType , pSample );
		return VO_RET_SOURCE2_OK;
	}

	if(pSample->uFlag & VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE)
	{
		if(nOutputType == VO_SOURCE2_TT_AUDIO && ptr_obj->m_bAudioUnavailableBlock)
			return VO_RET_SOURCE2_OK;
		else if(nOutputType == VO_SOURCE2_TT_VIDEO && ptr_obj->m_bVideoUnavailableBlock)
			return VO_RET_SOURCE2_OK;
		else if(nOutputType == VO_SOURCE2_TT_SUBTITLE && ptr_obj->m_bSubtitleUnavailableBlock)
			return VO_RET_SOURCE2_OK;
	}
	
	if( nOutputType == VO_SOURCE2_TT_VIDEO && (pSample->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT) )
		ptr_obj->m_contain_video = VO_TRUE;
	else if( nOutputType == VO_SOURCE2_TT_VIDEO && (pSample->uFlag & VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE) )
		ptr_obj->m_contain_video = VO_FALSE;
	
	if( (0 == pSample->uSize) && ( VO_DATASOURCE_FLAG_EOS & pSample->uDataSource_Flag))
	{
		ptr_obj->m_BufferCommander.PushData(VO_DATASOURCE_BUFFER_TYPE_SAMPLE, VO_SOURCE2_TT_MAX, pSample);
	}
	else
	{
		VOLOGR(" + OnSample.StreamType:0x%08x, ProgramType:%d. FPTrackID:%d, ASTrackID:%d, MarkFlag:%d, MarkOP:%d, TimeStamp:%lld"
			,ptr_obj->m_streaming_type, ptr_obj->m_info.GetProgramType(), pSample->uFPTrackID, pSample->uASTrackID, pSample->uMarkFlag, pSample->uMarkOP, pSample->uTime);

		if( 0 == pSample->uMarkFlag )
		{
			if( ptr_obj->m_pTrackSampleFilter->check_sample( nOutputType, pSample ) == VO_RET_SOURCE2_TRACKNOTFOUND)
				return VO_RET_SOURCE2_TRACKNOTFOUND;;
		}

		pSample->uMarkFlag = 0;
		/*set timestamp offset*/
		switch ( ptr_obj->m_streaming_type ) 
		{
		case VO_ADAPTIVESTREAMPARSER_STREAMTYPE_DASH:
		case VO_ADAPTIVESTREAMPARSER_STREAMTYPE_ISS:
		case VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS:
			{
				if( nOutputType == VO_SOURCE2_TT_VIDEO || nOutputType == VO_SOURCE2_TT_AUDIO || nOutputType == VO_SOURCE2_TT_SUBTITLE )
				{
					//all sample must be timestamp recalculated, so the timestamp_offset is useless now.
					//as yi's idea, hls & livestreaming should change the timestamp.It is temporary modification.
					//it is for minervaNetworks
					//2013/08/14 Leon. now timestamp recalculate should be use for each sample. So remove some useless codes.
					ptr_obj->m_TimeStampRecalculator.recalculate( (VO_U32)nOutputType , pSample );
					if(pSample->uDataSource_Flag & VO_DATASOURCE_FLAG_CHUNK_BEGIN)
						ptr_obj->OnChunkFirstSample(pSample->uChunkID, pSample->uTime);

					// remove VO_DATASOURCE_FLAG_CHUNK_BEGIN flag (after used by timestamp re-calculator)
					pSample->uDataSource_Flag &= ~VO_DATASOURCE_FLAG_CHUNK_BEGIN;
					pSample->uDataSource_Flag &= ~VO_DATASOURCE_FLAG_CHUNK_SWITCH;
					ptr_obj->m_info.GetTimeStampSyncObject().CheckAndWait( nOutputType , pSample , &ptr_obj->m_is_stop );
				}
			}
			break;
		}
		

	if(  nOutputType == VO_SOURCE2_TT_VIDEO || nOutputType == VO_SOURCE2_TT_AUDIO ||nOutputType == VO_SOURCE2_TT_SUBTITLE   )
	{
		VOLOGR( "Add %s Sample to Buffer.(FPTrackID:%d, ASTrackID:%d timestamp:%lld[0x%llx], BufferPtr:0x%08x,Size: %d, FlagData:%p,  Flag:0x%08x )" 
			,TrackType2String( ( VO_SOURCE2_TRACK_TYPE ) nOutputType ) , pSample->uFPTrackID, pSample->uASTrackID, One2Two(pSample->uTime) , pSample->pBuffer,pSample->uSize ,pSample->pFlagData, pSample->uFlag );
	}

#ifdef VOLOGR //for test
 	VO_U64 nBufferTime = 0;
 	VO_U32 nPID = 0;
 	switch( nOutputType )
 	{
 	case VO_SOURCE2_TT_VIDEO:
 		nPID = VO_BUFFER_PID_DURATION_V;
 		break;
 	case VO_SOURCE2_TT_AUDIO:
 		nPID = VO_BUFFER_PID_DURATION_A;
 		break;
 	case VO_SOURCE2_TT_SUBTITLE:
 		nPID = VO_BUFFER_PID_DURATION_T;
 		break;
 	}

		ptr_obj->m_BufferCommander.GetDuration((VO_BUFFER_PID_TYPE )nPID , &nBufferTime);
// 		if( nBufferTime >= ( ptr_obj->m_nMaxBufferTime - 200 ) )
// 		{
// 			VODS_VOLOGI( "ATTENTION. %s BufferingTime(%lld[0x%llx]) is close to MaxBufferTime(%lld[0x%llx]). "
// 				,TrackType2String( ( VO_SOURCE2_TRACK_TYPE ) nOutputType ), One2Two(nBufferTime), One2Two(VO_U64(ptr_obj->m_nMaxBufferTime)) );
// 		}
	 
		if( pSample->uSize > 100 * 1024 )
		{
			VODS_VOLOGI( "ATTENTION. %s BufferingSize(%d[0x%x]) is a large value[ ref value is 100 * 1024 ]. "
				,TrackType2String( ( VO_SOURCE2_TRACK_TYPE ) nOutputType ), One2Two(pSample->uSize) );
		}
		
#endif
		/* 2013/8/13, each sample should be put into downloadbuffermgr first*/
		ret = ptr_obj->m_BufferCommander.PushData(VO_DATASOURCE_BUFFER_TYPE_SAMPLE, (VO_SOURCE2_TRACK_TYPE)nOutputType, pData);

	}

	ptr_obj->CheckBufferReady();
	VO_U32 minDuration = ptr_obj->GetMinBufferDuration();
	ptr_obj->m_info.SetMinBufferDuration(minDuration);

	if(pSample->uFlag & VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE)
	{
		if(nOutputType == VO_SOURCE2_TT_AUDIO && !ptr_obj->m_bAudioUnavailableBlock)
			ptr_obj->m_bAudioUnavailableBlock = VO_TRUE;
		else if(nOutputType == VO_SOURCE2_TT_VIDEO && !ptr_obj->m_bVideoUnavailableBlock)
			ptr_obj->m_bVideoUnavailableBlock = VO_TRUE;
		else if(nOutputType == VO_SOURCE2_TT_SUBTITLE && !ptr_obj->m_bSubtitleUnavailableBlock)
			ptr_obj->m_bSubtitleUnavailableBlock = VO_TRUE;
		
		VOLOGI("%s Open Track UnavailableBlock. %d,%d,%d", TrackType2String((VO_SOURCE2_TRACK_TYPE)nOutputType),ptr_obj->m_bAudioUnavailableBlock,ptr_obj->m_bVideoUnavailableBlock,ptr_obj->m_bSubtitleUnavailableBlock );

	}
	
	VOLOGR(" - OnSample.(timestamp:%lld[0x%llx])", One2Two(pSample->uTime) );
	return 0;
}


#else

VO_S32 voAdaptiveStreamingController::OnSample(VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData)
{
	// 	if(!(pUserData && pData && m_pTrackSampleFilter && m_ptr_samplebuffer))
	// 		return VO_RET_SOURCE2_FAIL;

	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )pUserData;

	VO_U32 ret = VO_RET_SOURCE2_NEEDRETRY;

	_SAMPLE * pSample = ( _SAMPLE * )pData;

	if(nOutputType == VO_SOURCE2_TT_CUSTOMTIMEDTAG)
	{
		ptr_obj->m_TimeStampRecalculator.recalculate( (VO_U32)nOutputType , pSample );
		return VO_RET_SOURCE2_OK;
	}

	if(pSample->uFlag & VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE)
	{
		if(nOutputType == VO_SOURCE2_TT_AUDIO && ptr_obj->m_bAudioUnavailableBlock)
			return VO_RET_SOURCE2_OK;
		else if(nOutputType == VO_SOURCE2_TT_VIDEO && ptr_obj->m_bVideoUnavailableBlock)
			return VO_RET_SOURCE2_OK;
		else if(nOutputType == VO_SOURCE2_TT_SUBTITLE && ptr_obj->m_bSubtitleUnavailableBlock)
			return VO_RET_SOURCE2_OK;
	}

	if( nOutputType == VO_SOURCE2_TT_VIDEO && (pSample->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT) )
		ptr_obj->m_contain_video = VO_TRUE;
	else if( nOutputType == VO_SOURCE2_TT_VIDEO && (pSample->uFlag & VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE) )
		ptr_obj->m_contain_video = VO_FALSE;

	if(pSample->uSize == 0)
	{
		SKIPSPECIALSAMPLE( pSample->uFlag );
	}

	VOLOGR(" + OnSample.StreamType:0x%08x, ProgramType:%d. FPTrackID:%d, ASTrackID:%d, MarkFlag:%d, MarkOP:%d, TimeStamp:%lld"
		,ptr_obj->m_streaming_type, ptr_obj->m_info.GetProgramType(), pSample->uFPTrackID, pSample->uASTrackID, pSample->uMarkFlag, pSample->uMarkOP, pSample->uTime);

	if( pSample->uMarkFlag == 0 )
	{
		if( ptr_obj->m_pTrackSampleFilter->check_sample( nOutputType, pSample ) == VO_RET_SOURCE2_TRACKNOTFOUND)
			return VO_RET_SOURCE2_TRACKNOTFOUND;
	}

	pSample->uMarkFlag = 0;
	/*set timestamp offset*/
	switch ( ptr_obj->m_streaming_type ) 
	{
	case VO_ADAPTIVESTREAMPARSER_STREAMTYPE_DASH:
	case VO_ADAPTIVESTREAMPARSER_STREAMTYPE_ISS:
	case VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS:
		{
			if(( nOutputType == VO_SOURCE2_TT_VIDEO || nOutputType == VO_SOURCE2_TT_AUDIO || nOutputType == VO_SOURCE2_TT_SUBTITLE) )
			{
				//all sample must be timestamp recalculated, so the timestamp_offset is useless now.
#ifndef _TIMERECALCULATE_LIVEandVOD
				if( !ptr_obj->m_b_offset && !(pSample->uFlag & VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE) )
				{
					ptr_obj->m_timestamp_offset = pSample->uTime;
					VODS_VOLOGI("TimeStamp offset: %lld[0x%llx]", One2Two(ptr_obj->m_timestamp_offset) );
					ptr_obj->m_b_offset = VO_TRUE;
				}
#endif
				//as yi's idea, hls & livestreaming should change the timestamp.It is temporary modification.
				//it is for minervaNetworks
#ifdef _TIMERECALCULATE_LIVEandVOD
				ptr_obj->m_TimeStampRecalculator.recalculate( (VO_U32)nOutputType , pSample );
#else
				if( (ptr_obj->m_streaming_type == VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS 
					&& ptr_obj->m_info.GetProgramType() == VO_SOURCE2_STREAM_TYPE_LIVE) 
#ifdef _TIMERECALCULATE_DASH
					|| ptr_obj->m_streaming_type == VO_ADAPTIVESTREAMPARSER_STREAMTYPE_DASH 
#endif
					)
					ptr_obj->m_TimeStampRecalculator.recalculate( (VO_U32)nOutputType , pSample );
				else if( !( ptr_obj->m_streaming_type == VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS 
					&& ptr_obj->m_info.GetProgramType() == VO_SOURCE2_STREAM_TYPE_VOD ) )
				{
					pSample->uTime = ptr_obj->m_info.GetStartTime() + ( pSample->uTime <= ptr_obj->m_timestamp_offset ? 0 : pSample->uTime - ptr_obj->m_timestamp_offset );
				}
#endif
				if(pSample->uFlag & VO_DATASOURCE_FLAG_CHUNK_BEGIN)
					ptr_obj->OnChunkFirstSample(pSample->uChunkID, pSample->uTime);

				// remove VO_DATASOURCE_FLAG_CHUNK_BEGIN flag (after used by timestamp re-calculator)
				pSample->uFlag &= ~VO_DATASOURCE_FLAG_CHUNK_BEGIN;
				pSample->uFlag &= ~VO_DATASOURCE_FLAG_CHUNK_SWITCH;
				ptr_obj->m_info.GetTimeStampSyncObject().CheckAndWait( nOutputType , pSample , &ptr_obj->m_is_stop );
			}
		}
		break;
	}


	if(  nOutputType == VO_SOURCE2_TT_VIDEO || nOutputType == VO_SOURCE2_TT_AUDIO ||nOutputType == VO_SOURCE2_TT_SUBTITLE   )
	{
		VOLOGR( "Add %s Sample to Buffer.(FPTrackID:%d, ASTrackID:%d timestamp:%lld[0x%llx], BufferPtr:0x%08x,Size: %d, FlagData:%p,  Flag:0x%08x )" 
			,TrackType2String( ( VO_SOURCE2_TRACK_TYPE ) nOutputType ) , pSample->uFPTrackID, pSample->uASTrackID, One2Two(pSample->uTime) , pSample->pBuffer,pSample->uSize ,pSample->pFlagData, pSample->uFlag );
	}

#ifdef VOLOGR //for test
	VO_U64 nBufferTime = 0;
	VO_U32 nPID = 0;
	switch( nOutputType )
	{
	case VO_SOURCE2_TT_VIDEO:
		nPID = VO_BUFFER_PID_DURATION_V;
		break;
	case VO_SOURCE2_TT_AUDIO:
		nPID = VO_BUFFER_PID_DURATION_A;
		break;
	case VO_SOURCE2_TT_SUBTITLE:
		nPID = VO_BUFFER_PID_DURATION_T;
		break;
	}

	ptr_obj->m_ptr_samplebuffer->GetParameter(nPID , &nBufferTime);
	if( nBufferTime >= ( ptr_obj->m_nMaxBufferTime - 200 ) )
	{
		VODS_VOLOGI( "ATTENTION. %s BufferingTime(%lld[0x%llx]) is close to MaxBufferTime(%lld[0x%llx]). "
			,TrackType2String( ( VO_SOURCE2_TRACK_TYPE ) nOutputType ), One2Two(nBufferTime), One2Two(VO_U64(ptr_obj->m_nMaxBufferTime)) );
	}

	if( pSample->uSize > 100 * 1024 )
	{
		VODS_VOLOGI( "ATTENTION. %s BufferingSize(%d[0x%x]) is a large value[ ref value is 100 * 1024 ]. "
			,TrackType2String( ( VO_SOURCE2_TRACK_TYPE ) nOutputType ), One2Two(pSample->uSize) );
	}

#endif

	while( ret == VO_RET_SOURCE2_NEEDRETRY && !ptr_obj->m_is_stop )
	{
		ret = ptr_obj->m_ptr_samplebuffer->AddBuffer( nOutputType , pData );

		if( ret == VO_RET_SOURCE2_NEEDRETRY )
			voOS_SleepExitable( 200, &ptr_obj->m_is_stop);

		if(VO_RET_SOURCE2_OK == ret &&(_VODS_INT32_MAX != ptr_obj->m_nCheckTime_forSeek || _VODS_INT32_MAX != ptr_obj->m_nCheckTime))
		{
			VO_BOOL is_buffer_ready = VO_FALSE;
			ptr_obj->m_ptr_samplebuffer->GetParameter(VO_BUFFER_PID_IS_RUN, (VO_PTR)&is_buffer_ready);
			if (is_buffer_ready)
			{
				if( ptr_obj->m_nCheckTime_forSeek != _VODS_INT32_MAX )
				{
					RecordCostTime( ptr_obj->m_nCheckTime_forSeek, false, "Seek/Switch. Buffer is ready, Source is ok!!", 3 );
					ptr_obj->m_nCheckTime_forSeek = _VODS_INT32_MAX;

					ptr_obj->m_info.SetBuffering(VO_FALSE);
				}
				if( ptr_obj->m_nCheckTime != _VODS_INT32_MAX )
				{
					RecordCostTime( ptr_obj->m_nCheckTime, false, "Open. Buffer is ready, Source is ok!!", 3 );
					ptr_obj->m_nCheckTime = _VODS_INT32_MAX;

					ptr_obj->m_info.SetBuffering(VO_FALSE);
				}
			}
		}
	}

	SpecialFlagCheck((VO_SOURCE2_TRACK_TYPE)nOutputType, pSample->uFlag, pSample->uTime, ret);
	if(pSample->uFlag & VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE)
	{
		if(nOutputType == VO_SOURCE2_TT_AUDIO && !ptr_obj->m_bAudioUnavailableBlock)
			ptr_obj->m_bAudioUnavailableBlock = VO_TRUE;
		else if(nOutputType == VO_SOURCE2_TT_VIDEO && !ptr_obj->m_bVideoUnavailableBlock)
			ptr_obj->m_bVideoUnavailableBlock = VO_TRUE;
		else if(nOutputType == VO_SOURCE2_TT_SUBTITLE && !ptr_obj->m_bSubtitleUnavailableBlock)
			ptr_obj->m_bSubtitleUnavailableBlock = VO_TRUE;
	}

	VOLOGR(" - OnSample.(timestamp:%lld[0x%llx])", One2Two(pSample->uTime) );
	return 0;
}
#endif
VO_VOID voAdaptiveStreamingController::OnChunkFirstSample(VO_U64 uChunkID, VO_U64 uTimeStamp)
{
	voCAutoLock lock(&m_mtxLastChunk);

	if(uChunkID != m_uLastChunkID && uChunkID != _VODS_INT64_MAX)
	{
		VOLOGI("Last chunk ID changed: old ID %lld, Time %lld, new ID %lld, Time %lld", m_uLastChunkID, m_uLastChunkTimeStamp, uChunkID, uTimeStamp);

		VO_BOOL bChanged2Seekable = VO_FALSE;
		if(_VODS_INT64_MAX == m_uLastChunkID)
		{
			VOLOGI("Now get first sample callback");

			bChanged2Seekable = VO_TRUE;
		}

		m_uLastChunkID = uChunkID;
		m_uLastChunkTimeStamp = uTimeStamp;

		if(VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS == m_streaming_type && VO_SOURCE2_STREAM_TYPE_LIVE == m_info.GetProgramType() && bChanged2Seekable)
		{
			VOLOGI("Now changed to seekable for LIVE");

			OnEvent(this, VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_INFO, VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_LIVESEEKABLE, 0);
		}
	}
	if(VO_ADAPTIVESTREAMPARSER_STREAMTYPE_DASH == m_streaming_type)
	{
		VOLOGI("Change dash to init data ready");
		m_bDashInitDataReady = VO_TRUE;
	}
}

VO_U32 voAdaptiveStreamingController::OpenComplete()
{
	VO_U32 program_count = 0;
	if( m_ptr_parser )
		m_ptr_parser->GetProgramCount( &program_count );

	if( program_count < 1 )
		return VO_RET_SOURCE2_OK;

	_PROGRAM_INFO * ptr_program_info = 0;
	VO_U32 ret = VO_RET_SOURCE2_OK;
#ifdef _use_programinfo2
	ret = m_ptr_parser->GetProgramInfo( 0 , &ptr_program_info );
#else
	ret = m_pProgramInfoOP->GetProgramInfo( 0 , &ptr_program_info );
#endif
	if( ret != VO_RET_SOURCE2_OK || !ptr_program_info)
		return VO_RET_SOURCE2_FAIL;


	if( ptr_program_info->uStreamCount > 0 )
	{
		bitrate_info.uItemCount = ptr_program_info->uStreamCount;
		bitrate_info.pItemList = new VO_SOURCE2_BITRATE_INFOITEM[bitrate_info.uItemCount];
		
		for( VO_U32 i = 0 ; i < ptr_program_info->uStreamCount ; i++ )
		{
			_STREAM_INFO * ptr_info = ptr_program_info->ppStreamInfo[i];
			bitrate_info.pItemList[i].uBitrate = ptr_info->uBitrate;
			bitrate_info.pItemList[i].uSelInfo = ptr_info->uSelInfo;
			if(ptr_info->uSelInfo == VO_SOURCE2_SELECT_SELECTED)
				m_CurBitrate = ptr_info->uBitrate;
		}

		m_ptr_ba->Open( m_ptr_ba->hHandle , &bitrate_info );

		VO_U32 maxdownloadspeed;
		m_ptr_ba->GetParam( m_ptr_ba->hHandle , VO_PID_SOURCE2_BA_MAXDOWNLOADBITRATE , &maxdownloadspeed );

		m_info.SetMaxDownloadBitrate( maxdownloadspeed );

		ret = ToChangeBitrate();
		
		if( ret == VO_RET_SOURCE2_BA_BITRATECHANGED || ret == VO_RET_SOURCE2_BA_BITRATEUNCHANGED )
		{
			VODS_VOLOGI( "BAINFO Use Bitrate %d to start playback" , m_CurBitrate );
			ret = VO_RET_SOURCE2_OK;
		}
		else 
		{
			VODS_VOLOGI( "Change Bitrate Failed" );
			ret = VO_RET_SOURCE2_FAIL;
		}

	}
	
	if( ret == VO_RET_SOURCE2_OK )
	{
		ptr_program_info = 0;
		//m_ptr_parser->GetProgramInfo( 0 , &ptr_program_info );
		m_pProgramInfoOP->GetProgramInfo(0, &ptr_program_info );
		VODS_VOLOGI("ProgramType:%d", ptr_program_info->sProgramType );
		m_info.SetProgramType( ptr_program_info->sProgramType );
		m_BufferCommander.SetParam(VO_DATASOURCE_PID_STREAMCOUNT, &ptr_program_info->uStreamCount);
		m_eventHandler_api.SetParameter(m_eventHandler_api.hHandle, VO_DATASOURCE_PID_STREAMCOUNT,&ptr_program_info->uStreamCount);

		if( m_ptr_parser )
			ret = m_ptr_parser->Start();
	}
	
	return ret;
}

VO_VOID voAdaptiveStreamingController::SetStopFlag( VO_BOOL bStop )
{
	m_is_stop = bStop;
	if( m_eventHandler_api.hHandle )
	{
		m_eventHandler_api.SetStop( m_eventHandler_api.hHandle, m_is_stop );
	}

	//ChangeTrack();
}
VO_U32 voAdaptiveStreamingController::DownloadStartColletor( CHUNKINFO * pChunkInfo )
{
	if( pChunkInfo->chunktype == VO_SOURCE2_ADAPTIVESTREAMING_AUDIO && m_contain_video )
	{
		return 0;
	}

	if( pChunkInfo->chunktype == VO_SOURCE2_ADAPTIVESTREAMING_AUDIO 
		|| pChunkInfo->chunktype == VO_SOURCE2_ADAPTIVESTREAMING_VIDEO 
		|| pChunkInfo->chunktype == VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO)
	{
		VOLOGI("%s DownloadDataColletor Reset.", ChunkType2String(pChunkInfo->chunktype));
		m_info.GetDownloadDataColletor()->Reset();
	}
	return VO_RET_SOURCE2_OK;
}

VO_U64 voAdaptiveStreamingController::DownloadEndColletor( CHUNKINFO * pChunkInfo )
{
	if( pChunkInfo->chunktype == VO_SOURCE2_ADAPTIVESTREAMING_AUDIO && m_contain_video )
	{
		return 0;
	}

	if( pChunkInfo->chunktype == VO_SOURCE2_ADAPTIVESTREAMING_AUDIO 
		|| pChunkInfo->chunktype == VO_SOURCE2_ADAPTIVESTREAMING_VIDEO 
		|| pChunkInfo->chunktype == VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO)
	{
		VOLOGI("%s DownloadDataColletor End.%lld", ChunkType2String(pChunkInfo->chunktype),m_info.GetDownloadDataColletor()->GetSize());
		return  m_info.GetDownloadDataColletor()->GetSize();
	}
	return 0;
}
VO_U32 voAdaptiveStreamingController::ChunkBegin( CHUNKINFO * pChunkInfo , VO_BOOL bFormatChanged)
{
	if( pChunkInfo->chunktype == VO_SOURCE2_ADAPTIVESTREAMING_AUDIO && m_contain_video )
	{
		return 0;
	}
	if(pChunkInfo->chunktype == VO_SOURCE2_ADAPTIVESTREAMING_VIDEO && !m_contain_video)
	{
		return 0;
	}
	if( pChunkInfo->chunktype != VO_SOURCE2_ADAPTIVESTREAMING_AUDIO 
		&& pChunkInfo->chunktype != VO_SOURCE2_ADAPTIVESTREAMING_VIDEO 
		&& pChunkInfo->chunktype != VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO)
		return 0;

	if( !m_hasSetBufferingTime )
	{
#ifndef _USE_BUFFERCOMMANDER
		SetBufferTime(m_ptr_samplebuffer, VO_BUFFER_SETTING_BUFFERTIME, (pChunkInfo->duration > 5000 ? pChunkInfo->duration : 5000), m_hasSetBufferingTime);
#else
		VO_U32 buffertime = pChunkInfo->duration > 5000 ? pChunkInfo->duration : 5000;
		m_BufferCommander.SetParam(VO_PID_SOURCE2_BUFFER_BUFFERINGTIME, &buffertime);
		m_hasSetBufferingTime = VO_TRUE;
#endif
	}
	/*
	if( !m_hasSetStartBufferTime )
	{
#ifndef _USE_BUFFERCOMMANDER
		SetBufferTime(m_ptr_samplebuffer, VO_BUFFER_SETTING_STARTBUFFERTIME, pChunkInfo->duration, m_hasSetStartBufferTime);
#else
		m_BufferCommander.SetParam(VO_PID_SOURCE2_BUFFER_STARTBUFFERINGTIME, &pChunkInfo->duration);
		m_hasSetStartBufferTime = VO_TRUE;
#endif
	}
	*/
	VODS_VOLOGI( "%s BAINFO Chunk Start! First TimeStamp %lld Bitrate %d isFormatChanged %s" ,ChunkType2String(pChunkInfo->chunktype), pChunkInfo->start_timestamp , m_CurBitrate , bFormatChanged?"true":"false" );

	m_ptr_ba->StreamChunkBegin( m_ptr_ba->hHandle , m_CurBitrate , pChunkInfo->start_timestamp, bFormatChanged );

	return 0;
}

VO_U32 voAdaptiveStreamingController::ChunkComplete( CHUNKINFO * pChunkInfo )
{

	if( pChunkInfo->chunktype == VO_SOURCE2_ADAPTIVESTREAMING_AUDIO && m_contain_video )
	{
		return 0;
	}
	if(pChunkInfo->chunktype == VO_SOURCE2_ADAPTIVESTREAMING_VIDEO && !m_contain_video)
	{
		return 0;
	}
	if( pChunkInfo->chunktype != VO_SOURCE2_ADAPTIVESTREAMING_AUDIO 
		&& pChunkInfo->chunktype != VO_SOURCE2_ADAPTIVESTREAMING_VIDEO 
		&& pChunkInfo->chunktype != VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO)
		return 0;

	VODS_VOLOGI( "%s BAINFO Chunk End!(ContainVideo:%d)  Last TimeStamp %lld Download Cost %lld Download Size %lld Download Speed %lld" , ChunkType2String( pChunkInfo->chunktype), m_contain_video,pChunkInfo->end_timestamp , pChunkInfo->download_time , pChunkInfo->size , pChunkInfo->size * 8000 / pChunkInfo->download_time );

	m_ptr_ba->StreamChunkEnd( m_ptr_ba->hHandle , pChunkInfo->download_time , ( VO_U32 )pChunkInfo->size , pChunkInfo->end_timestamp);

	//do not do ba for the first chunk, but add it into ba map to avoid lost first chunk`s big delay
#ifdef _USE_BUFFERCOMMANDER
	if( pChunkInfo->bDownloadComplete && m_bIsFirstChunk )
	{
		m_bIsFirstChunk = VO_FALSE;
		return 0;
	}
#endif
	m_BufferCommander.GetParam(VO_BUFFER_PID_EFFECTIVE_DURATION,(VO_PTR)&m_nBufferDuration);
	m_ptr_ba->SetParam(m_ptr_ba->hHandle,VO_PID_SOURCE2_BA_BUFFER_DURATION,&m_nBufferDuration);
	
	ChangeTrack();
	VO_U32 ret = ToChangeBitrate();
	if(ret == VO_RET_SOURCE2_BA_BITRATECHANGED)
		m_info.SetBitrateChanging(VO_TRUE);
	return ret;
}

/*this func will return VO_RET_SOURCE2_BA_BITRATEUNCHANGED / VO_RET_SOURCE2_BA_BITRATECHANGED/ VO_RET_SOURCE2_FAIL */
VO_U32 voAdaptiveStreamingController::ToChangeBitrate()
{
	VO_U32 ret = ChangeBitrate();
	if( ret == VO_RET_SOURCE2_BA_BITRATECHANGED )
	{
		OnEvent(this,VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_INFO,VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_BITRATE_CHANGE, m_CurBitrate );
		//reset unavailableblock here
		m_bAudioUnavailableBlock = m_bVideoUnavailableBlock = m_bSubtitleUnavailableBlock = VO_FALSE;
	}
	return ret;
}

VO_U32 voAdaptiveStreamingController::ChangeBitrate()
{
	//static VO_U32 times = 0;
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	VO_U32 next_bitrate = 0;
	if( !m_enableBA )
	{
		next_bitrate = m_SelBitrate;
	}
	else
	{
		VO_U32 ret = m_ptr_ba->GetNextBitrate( m_ptr_ba->hHandle , &next_bitrate );
		if( ret != VO_RET_SOURCE2_OK )
		{
			VODS_VOLOGW("GetNextBitrate Error");
			return VO_RET_SOURCE2_BA_BITRATEUNCHANGED;
		}
	}
	for(VO_U32 i = 0; i < bitrate_info.uItemCount; i++)
	{
		if(next_bitrate == bitrate_info.pItemList[i].uBitrate)
			break;
		if(i == bitrate_info.uItemCount - 1)
		{
			VOLOGW("bitrate %d has been  removed from bitrate list",next_bitrate);
			return VO_RET_SOURCE2_BA_BITRATEUNCHANGED;
		}
	}
	if(!m_enableBA)
	{
		m_ptr_ba->SetParam(m_ptr_ba->hHandle,VO_PID_SOURCE2_BA_SET_CURRENT_BITRATE,&m_SelBitrate);
	}
	VO_BOOL min_bitrate_play = VO_FALSE;
	m_ptr_ba->GetParam(m_ptr_ba->hHandle,VO_PID_SOURCE2_BA_MIN_BITRATE_PLAY,(VO_PTR)(&min_bitrate_play));
	m_info.SetMinBitratePlay(min_bitrate_play);
	m_info.SetBAEnable(m_enableBA);

	VODS_VOLOGI( "Decide Next Bitrate: %d" , next_bitrate );
	
	if( next_bitrate == m_CurBitrate )
	{

		VOLOGW("next and current bitrate are the same");
		return VO_RET_SOURCE2_BA_BITRATEUNCHANGED;
	}
	

	VO_U32 program_count = 0;
	if( m_ptr_parser )
		m_ptr_parser->GetProgramCount( &program_count );


	if( program_count < 1 )
	{

		VOLOGW("program_count is less than 1");
		return VO_RET_SOURCE2_BA_BITRATEUNCHANGED;
	}

	_PROGRAM_INFO * ptr_program_info = 0;
#ifdef _use_programinfo2
	m_ptr_parser->GetProgramInfo( 0 , &ptr_program_info );
#else
	if( m_pProgramInfoOP )
		m_pProgramInfoOP->GetProgramInfo( 0, &ptr_program_info);
#endif
	
	if( ptr_program_info && ptr_program_info->uStreamCount > 0 )
	{
			
		for( VO_U32 i = 0 ; i < ptr_program_info->uStreamCount ; i++ )
		{
			_STREAM_INFO * ptr_info = ptr_program_info->ppStreamInfo[i];
			
			if( ptr_info->uBitrate == next_bitrate )
			{
				VO_S32 retry_count = 0;
				while( !m_is_stop && retry_count <= 5 )
				{
					VODS_VOLOGI( "BAINFO Try to Change Bitrate to %d" , next_bitrate );
					if(m_bNeedSwitchAudioThread)
					{
						VOLOGI("select stream switch to audio thread");
						m_ptr_parser->SetParam(VO_PID_ADAPTIVESTREAMING_SWITCH_REF_ADUIO_THREAD, &m_bNeedSwitchAudioThread);
					}
					ret = ToSelectStream( ptr_info->uStreamID );
					//has some logic issue here,needretry will never be triggered
					if( ret != VO_RET_SOURCE2_OK )
					{
						VODS_VOLOGI( "BAINFO Change Bitrate Fail!" );
						return VO_RET_SOURCE2_FAIL;
					}
					else if( ret == VO_RET_SOURCE2_NEEDRETRY )
					{
						for( VO_S32 i = 0 ; i < 20 && !m_is_stop ; i++ )
							voOS_SleepExitable(100, &m_is_stop);

						retry_count++;
					}
					else
					{
						VODS_VOLOGI("BAINFO Change Bitrate to %d",next_bitrate);
						if(m_bNeedSwitchAudioThread)
						{
							m_bNeedSwitchAudioThread = VO_FALSE;
						}
						m_CurBitrate = next_bitrate;
						return VO_RET_SOURCE2_BA_BITRATECHANGED;
					}
				}
			}
		}
	}
	VOLOGW("cannot find suitable bitrate");
	return VO_RET_SOURCE2_BA_BITRATEUNCHANGED;
}

VO_U32 voAdaptiveStreamingController::ToChangeBitrateWhenError()
{
	VO_U32 ret = ChangeBitrateWhenError();
	if( ret == VO_RET_SOURCE2_BA_BITRATECHANGED )
	{
		OnEvent(this,VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_INFO,VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_BITRATE_CHANGE, m_CurBitrate );
		//reset unavailableblock here
		m_bAudioUnavailableBlock = m_bVideoUnavailableBlock = m_bSubtitleUnavailableBlock = VO_FALSE;
	}
	return ret;
}

VO_U32 voAdaptiveStreamingController::ChangeBitrateWhenError()
{
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	VO_U32 i = 0;
	VO_U32 next_bitrate = 0;
	for(i = 0; i < bitrate_info.uItemCount; i++)
	{
		if(bitrate_info.pItemList[i].uBitrate == m_CurBitrate)
		{
			bitrate_info.pItemList[i].uBitrate = 0;
			break;
		}
	}
	BubbleSort(bitrate_info.pItemList, bitrate_info.uItemCount);
	for(i = 0; i < bitrate_info.uItemCount; i++)
	{
		if(bitrate_info.pItemList[i].uBitrate < m_CurBitrate)
		{
			next_bitrate = bitrate_info.pItemList[i].uBitrate;
			continue;
		}
		else
		{
			break;
		}
	}
	if(next_bitrate == 0)
	{
		for(i = 0; i < bitrate_info.uItemCount; i++)
		{
			if(bitrate_info.pItemList[i].uBitrate <= m_CurBitrate)
			{
				continue;
			}
			else
			{
				next_bitrate = bitrate_info.pItemList[i].uBitrate;
				break;
			}
		}
	}
	if(next_bitrate == 0)
	{
		VOLOGW("Can not find suitable bitrate for error retry");
		return VO_RET_SOURCE2_FAIL;
	}
	_PROGRAM_INFO * ptr_program_info = 0;
#ifdef _use_programinfo2
	m_ptr_parser->GetProgramInfo( 0 , &ptr_program_info );
#else
	if( m_pProgramInfoOP )
		m_pProgramInfoOP->GetProgramInfo( 0, &ptr_program_info);
#endif
	if( ptr_program_info && ptr_program_info->uStreamCount > 0 )
	{
			
		for( VO_U32 i = 0 ; i < ptr_program_info->uStreamCount ; i++ )
		{
			_STREAM_INFO * ptr_info = ptr_program_info->ppStreamInfo[i];
			
			if( ptr_info->uBitrate == next_bitrate )
			{
				VO_S32 retry_count = 0;
				while( !m_is_stop && retry_count <= 5 )
				{
					VODS_VOLOGI( "BAINFO Try to Change Bitrate to %d" , next_bitrate );
					if(m_bNeedSwitchAudioThread)
					{
						VOLOGI("select stream switch to audio thread");
						m_ptr_parser->SetParam(VO_PID_ADAPTIVESTREAMING_SWITCH_REF_ADUIO_THREAD, &m_bNeedSwitchAudioThread);
					}
					ret = ToSelectStream( ptr_info->uStreamID );
					if( ret != VO_RET_SOURCE2_OK )
					{
						VODS_VOLOGI( "BAINFO Change Bitrate Fail!" );
						return VO_RET_SOURCE2_FAIL;
					}
					else
					{
						VODS_VOLOGI("BAINFO Change Bitrate to %d",next_bitrate);
						if(m_bNeedSwitchAudioThread)
						{
							m_bNeedSwitchAudioThread = VO_FALSE;
						}
						m_CurBitrate = next_bitrate;
						return VO_RET_SOURCE2_BA_BITRATECHANGED;
					}
				}
			}
		}
	}
	VOLOGW("cannot find suitable bitrate");
	return VO_RET_SOURCE2_BA_BITRATEUNCHANGED;
}

void voAdaptiveStreamingController::thread_function()
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	ret = ToOpenInternal();
	VODS_VOLOGI( "Send VO_EVENTID_SOURCE2_OPENCOMPLETE Event.(ret: %d)" , ret );
	NotifyEvent( VO_EVENTID_SOURCE2_OPENCOMPLETE , ( VO_U32 )&ret , 0 );

	if((VO_ADAPTIVESTREAMPARSER_STREAMTYPE_ISS == m_streaming_type || VO_ADAPTIVESTREAMPARSER_STREAMTYPE_DASH == m_streaming_type) && VO_SOURCE2_STREAM_TYPE_LIVE == m_info.GetProgramType())
	{
		VOLOGI("Now changed to seekable for LIVE");

		OnEvent(this, VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_INFO, VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_LIVESEEKABLE, 0);
	}
}

//notify event to outside.
VO_U32 voAdaptiveStreamingController::NotifyEvent( VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2 )
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	if( m_is_stop)
		return VO_RET_SOURCE2_OK;

	if( m_ptr_eventcallback )
		ret = m_ptr_eventcallback->SendEvent( m_ptr_eventcallback->pUserData , nID , nParam1 , nParam2 );
	else
	{
		VOLOGE( "Empty event callback!" );
		ret = VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	if(nID == VO_EVENTID_SOURCE2_OPENCOMPLETE)
		m_bOpenComplete = VO_TRUE;
	return ret;
}

VO_U32 voAdaptiveStreamingController::ChangeTrack(VO_BOOL * pbChanged /* = NULL */)
{
	vo_singlelink_list<VO_U32> listSelectTrackTmp;
	{
		// copy select track list to temp list to avoid lock to SelectTrack, East 20130521
		voCAutoLock lock(&m_SelTrackLock);

		if(m_SelTrackList.count() > 0)
		{
			vo_singlelink_list<VO_U32>::iterator iter = m_SelTrackList.begin();
			vo_singlelink_list<VO_U32>::iterator itere = m_SelTrackList.end();

			while(iter != itere)
			{
				listSelectTrackTmp.push_back(*iter);

				iter++;
			}
		}

		m_SelTrackList.reset();
	}

	// do actual select track
	if(listSelectTrackTmp.count() > 0)
	{
		VOLOGI("changing tracks...");

		vo_singlelink_list<VO_U32>::iterator iter = listSelectTrackTmp.begin();
		vo_singlelink_list<VO_U32>::iterator itere = listSelectTrackTmp.end();

		while(iter != itere)
		{
			ToSelectTrack(*iter);
			iter++;
		}

		if(pbChanged)
			*pbChanged = VO_TRUE;
	}
	else
	{
		if(pbChanged)
			*pbChanged = VO_FALSE;
	}

	return VO_RET_SOURCE2_OK;
}
#ifndef _USE_BUFFERCOMMANDER
VO_U32 voAdaptiveStreamingController::SetBufferTime(voSourceBufferManager_AI * pSBM, VO_BUFFER_SETTINGS_TYPE type, VO_U32 nTime, VO_BOOL &hasSet)
{
	VO_BUFFER_SETTING_SAMPLE sample;
	sample.nType = type;
	sample.pObj = (VO_S32*)&nTime;

	if(pSBM )
	{
		VODS_VOLOGI ("Set Buffer Time. Type:%d, Time:%d", type, nTime);
		pSBM->AddBuffer( VO_BUFFER_SETTINGS , &sample );
		hasSet = VO_TRUE;
	}
	else
		return VO_RET_SOURCE2_FAIL;
	return VO_RET_SOURCE2_OK;
}
#endif


VO_U32 voAdaptiveStreamingController::PreProcessURL(VO_ADAPTIVESTREAM_PLAYLISTDATA * pData)
{
	if( !m_drm_api.hHandle )
		return VO_RET_SOURCE2_FAIL;
	VO_U32 ret = VO_RET_SOURCE2_OK;
/*
	ret = m_drm_api.PreprocessPlaylist(m_drm_api.hHandle,(VO_PBYTE) m_pPlaylistData, m_PlaylistDataSize, &m_PlaylistDataUsedSize, NULL);
	if( ret ==  VO_RET_SOURCE2_OK || ret ==  VO_RET_SOURCE2_NOIMPLEMENT)
	{
		pData->pData = (VO_PBYTE)m_pPlaylistData;
		pData->uDataSize = m_PlaylistDataUsedSize;
	}
	else if( ret ==  VO_RET_SOURCE2_OUTPUTDATASMALL)
	{
		;
	}
*/
	ret = m_drm_api.PreprocessPlaylist(m_drm_api.hHandle,(VO_PBYTE) pData->pData, pData->uFullDataSize, &pData->uDataSize, NULL);

	return VO_RET_SOURCE2_OK;
}

VO_U32 voAdaptiveStreamingController::ToSelectTrack( VO_U32 nTrack )
{
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	
	if( m_pProgramInfoOP )
		ret = m_pProgramInfoOP->SelectTrack( nTrack );

	return ret;
}
VO_U32 voAdaptiveStreamingController::ToSelectStream( VO_U32 nStream )
{
	VO_U32 ret = VO_RET_SOURCE2_FAIL;

	if( m_pProgramInfoOP )
		ret = m_pProgramInfoOP->SelectStream( nStream );

	return ret;
}

//Useless code
/*
VO_VOID voAdaptiveStreamingController::AddProgramInfo2Buffer(_PROGRAM_INFO *pInfo)
{
	VO_U32 ret = VO_RET_SOURCE2_NEEDRETRY;
	_SAMPLE sample;
	memset( &sample, 0, sizeof(_SAMPLE) );
	sample.uMarkFlag = VO_DATASOURCE_MARKFLAG_PROGRAMINFO;
	sample.pReserve2 = pInfo;
	VOLOGR("ProgramInfoPlus_OT:%p", pInfo);
	sample.uTime = 0;
	while( ret == VO_RET_SOURCE2_NEEDRETRY && !m_is_stop )
	{
		ret = m_ptr_samplebuffer->AddBuffer( VO_SOURCE2_TT_VIDEO , &sample );

		if( ret == VO_RET_SOURCE2_NEEDRETRY )
			voOS_SleepExitable(200, &m_is_stop);
	}
}
*/
VO_VOID voAdaptiveStreamingController::InitForDRM(VO_StreamingDRM_API *pDRM)
{
	if( pDRM->Init )
	{
		VO_SOURCEDRM_CALLBACK2 * pCB = m_ptr_drmcallback;

		if( pDRM != &m_drm_api && m_ptr_drmcallback )
			m_drm_api.GetParameter(m_drm_api.hHandle, VO_PID_AS_DRM2_NEW_API,  &pCB );	
		pDRM->Init( &pDRM->hHandle , pCB , m_ptr_io , 0 );
		pDRM->SetParameter( pDRM->hHandle, VO_PID_COMMON_LOGFUNC,  m_pVologCB);
	}
}

#ifdef _HTTPHEADER_TEST
VO_U32 voAdaptiveStreamingController::IO_HTTP_CALLBACK_FUNC_TEST( VO_PTR pUserData , VO_U32 uID , VO_PTR pParam1 , VO_PTR pParam2  )
{
	voAdaptiveStreamingController *pCtrl = (voAdaptiveStreamingController*) pUserData;


	VO_SOURCE2_HTTPHEADER header;
	header.szHeaderName = "Set-Cookie";
	header.szHeaderValue = "IIWC_SID=IIWC983A78AED3A2633E4F553BAFF9BFD3F01613D3405D792; Path=/; Domain=digitallife.att.com; Expires = 1362993825;/r/n";
	pCtrl->SetParam(VO_PID_SOURCE2_HTTPHEADER, &header );
	return 0;
}
#endif


VO_BOOL voAdaptiveStreamingController::CheckIsOpenComplete()
{
	return m_bOpenComplete;
}

VO_U32 voAdaptiveStreamingController::BeginThread_Seek( )
{
	unsigned int thread_id;
	create_thread( &m_threadHandleSeek , &thread_id , AsyncSeekThread , this , 0 );
	return 0;
}
VO_VOID voAdaptiveStreamingController::StopThread_Seek( )
{
	m_bStopSeek = VO_TRUE;
	wait_thread_exit(m_threadHandleSeek);
	m_bStopSeek = VO_FALSE;
	m_threadHandleSeek = 0;
}

unsigned int voAdaptiveStreamingController::AsyncSeekThread( void * pParam )
{
	voAdaptiveStreamingController *ptr = (voAdaptiveStreamingController *)pParam;
	if( !ptr )
	{
		VOLOGE("Async Seek Thread Start Error.");
		return 0;
	}
	set_threadname("Async Seek Thread");

	while( !ptr->m_bStopSeek )
	{
		voOS_SleepExitable(50,&ptr->m_is_stop);

		VO_U64 nSeekPos = -1;
		{
			voCAutoLock lock( &ptr->m_lockSeek );
			nSeekPos = ptr->m_nPositionSeek;
			ptr->m_nPositionSeek = -1;
		}

		if(-1 != nSeekPos)
		{
			voCAutoLock lockseek(&ptr->m_lockSeekAndUpdate);
			VOLOGI("AsyncSeekThread. Seek position: %lld.", nSeekPos);
			VO_U32 ret = ptr->SeekAsync(&nSeekPos);
			{
				voCAutoLock lock( &ptr->m_lockSeek );
				if(ptr->m_nPositionSeek == -1)
				{
					VOLOGI("No new seeking enter, so reset seeking flag");
					ptr->m_bSeeking = VO_FALSE;
				}
				else
				{
					VOLOGI("new seeking enter, keep seeking flag");
				}
			}
			VOLOGI("AsyncSeekThread. Send VO_EVENTID_SOURCE2_SEEKCOMPLETE Event.(Can Seek Pos:%lld, ret: %d)", nSeekPos, ret);
		}
	}
	exit_thread();
	return 0;
}

VO_U32 voAdaptiveStreamingController::GetCurrentStartTime(VO_U64 * puStartTime)
{
	if(VO_SOURCE2_STREAM_TYPE_LIVE != m_info.GetProgramType())	// just for LIVE DVR
		return VO_RET_SOURCE2_FAIL;

	voCAutoLock lock(&m_mtxLastChunk);

	if(_VODS_INT64_MAX == m_uLastChunkID)
	{
		VOLOGI("still no reference chunk");
		return VO_RET_SOURCE2_FAIL;
	}

	VO_U32 ret = VO_RET_SOURCE2_NOIMPLEMENT;
	VO_ADAPTIVESTREAMPARSER_CHUNKWINDOWPOS sChunkWindowPosition;
	sChunkWindowPosition.uFlags = 0;
	sChunkWindowPosition.uChunkID = m_uLastChunkID;
	sChunkWindowPosition.ullWindowPosition = -1;
	if(m_ptr_parser)
		ret = m_ptr_parser->GetParam(VO_PID_ADAPTIVESTREAMING_DVR_CHUNKWINDOWPOS, &sChunkWindowPosition);

	if(VO_RET_SOURCE2_OK != ret)
		return ret;

	VOLOGI("last chunk ID %lld, Time %lld, window position %lld", m_uLastChunkID, m_uLastChunkTimeStamp, sChunkWindowPosition.ullWindowPosition);

	if(puStartTime)
	{
		if(m_uLastChunkTimeStamp < sChunkWindowPosition.ullWindowPosition)
			*puStartTime = 0;
		else
			*puStartTime = m_uLastChunkTimeStamp - sChunkWindowPosition.ullWindowPosition;
	}

	return VO_RET_SOURCE2_OK;
}

VO_U32 voAdaptiveStreamingController::UpdateDVRInfo()
{
	if(!m_ptr_parser)
		return VO_RET_SOURCE2_FAIL;

	VO_U32 ret = VO_RET_SOURCE2_OK;
	if(m_info.GetProgramType() == VO_SOURCE2_STREAM_TYPE_VOD)
	{
		// VOD
		VO_U64 mDuration = 0;
		ret = m_ptr_parser->GetDuration(&mDuration);
		if(VO_RET_SOURCE2_OK != ret)
		{
			VOLOGE("failed to m_ptr_parser->GetDuration 0x%08X", ret);
			return ret;
		}

		m_DVRInfo.ullStartTime = 0;
		m_DVRInfo.ullEndTime = m_DVRInfo.ullLiveTime = mDuration;
	}
	else
	{
		// LIVE
		VO_U64 uStartTime = -1;
		VO_U64 ullValue = 0;
		ret = GetCurrentStartTime(&uStartTime);
		if(VO_RET_SOURCE2_OK == ret)
		{
			m_DVRInfo.ullStartTime = uStartTime;

			ret = m_ptr_parser->GetParam(VO_PID_ADAPTIVESTREAMING_DVR_WINDOWLENGTH, &ullValue);
			if(VO_RET_SOURCE2_OK != ret)
			{
				VOLOGE("failed to VO_PID_ADAPTIVESTREAMING_DVR_WINDOWLENGTH 0x%08X", ret);
				return ret;
			}

			VOLOGI("DVR WINDOWLENGTH = %lld", ullValue);
			m_DVRInfo.ullLiveTime = m_DVRInfo.ullEndTime = uStartTime + ullValue;
		}
		else
		{
			ret = m_ptr_parser->GetParam(VO_PID_ADAPTIVESTREAMING_DVR_ENDLENGTH, &ullValue);
			if(VO_RET_SOURCE2_OK != ret)
			{
				VOLOGE("failed to VO_PID_ADAPTIVESTREAMING_DVR_ENDLENGTH 0x%08X", ret);
				return ret;
			}

			VOLOGI("DVR ENDLENGTH = %lld", ullValue);
			m_DVRInfo.ullEndTime = m_DVRInfo.ullPlayingTime + ullValue;

			ret = m_ptr_parser->GetParam(VO_PID_ADAPTIVESTREAMING_DVR_LIVELENGTH, &ullValue);
			if(VO_RET_SOURCE2_OK != ret)
			{
				VOLOGE("failed to VO_PID_ADAPTIVESTREAMING_DVR_LIVELENGTH 0x%08X", ret);
				return ret;
			}

			VOLOGI("DVR LIVELENGTH = %lld", ullValue);
			m_DVRInfo.ullLiveTime = m_DVRInfo.ullPlayingTime + ullValue;

			ret = m_ptr_parser->GetParam(VO_PID_ADAPTIVESTREAMING_DVR_WINDOWLENGTH, &ullValue);
			if(VO_RET_SOURCE2_OK != ret)
			{
				VOLOGE("failed to VO_PID_ADAPTIVESTREAMING_DVR_WINDOWLENGTH 0x%08X", ret);
				return ret;
			}

			VOLOGI("DVR WINDOWLENGTH = %lld", ullValue);
			if(m_DVRInfo.ullEndTime < ullValue)
				m_DVRInfo.ullStartTime = 0;
			else
				m_DVRInfo.ullStartTime = m_DVRInfo.ullEndTime - ullValue;
		}
	}

	VOLOGI("DVR PlayingTime \t= %lld", m_DVRInfo.ullPlayingTime + 10);
	VOLOGI("DVR LiveTime	\t= %lld", m_DVRInfo.ullLiveTime);
	VOLOGI("DVR StartTime	\t= %lld", m_DVRInfo.ullStartTime);
	VOLOGI("DVR EndTime		\t= %lld", m_DVRInfo.ullEndTime);

	return ret;
}
#ifdef _USE_BUFFERCOMMANDER
VO_VOID voAdaptiveStreamingController::CheckBufferReady()
{
	if(_VODS_INT32_MAX != m_nCheckTime_forSeek || _VODS_INT32_MAX != m_nCheckTime)
	{
		VO_BOOL is_buffer_ready = VO_FALSE;
#ifndef _USE_BUFFERCOMMANDER
		m_ptr_samplebuffer->GetParameter(VO_BUFFER_PID_IS_RUN, (VO_PTR)&is_buffer_ready);
#else
		m_BufferCommander.GetParam(VO_BUFFER_PID_IS_RUN, (VO_PTR)&is_buffer_ready);
#endif
		if (is_buffer_ready)
		{
			if( m_nCheckTime_forSeek != _VODS_INT32_MAX )
			{
				RecordCostTime( m_nCheckTime_forSeek, false, "Seek/Switch. Buffer is ready, Source is ok!!", 3 );
				m_nCheckTime_forSeek = _VODS_INT32_MAX;

				m_info.SetBuffering(VO_FALSE);
			}
			if( m_nCheckTime != _VODS_INT32_MAX )
			{
				RecordCostTime( m_nCheckTime, false, "Open. Buffer is ready, Source is ok!!", 3 );
				m_nCheckTime = _VODS_INT32_MAX;

				m_info.SetBuffering(VO_FALSE);
			}
		}
	}

}

VO_U32 voAdaptiveStreamingController::GetMinBufferDuration()
{
	VO_U32 uBufferDuration = 0;
	m_BufferCommander.GetParam(VO_BUFFER_PID_EFFECTIVE_DURATION,(VO_PTR)&uBufferDuration);
	//VOLOGI("Buffer Duration is %d",uBufferDuration);
	return uBufferDuration;
}

unsigned int voAdaptiveStreamingController::AsyncUpdateUrlThread( void * pParam )
{
	voAdaptiveStreamingController *ptr = (voAdaptiveStreamingController *)pParam;
	if( !ptr )
	{
		VOLOGE("Async Update Url Thread Start Error.");
		ptr->NotifyEvent(VO_EVENTID_SOURCE2_UPDATEURLCOMPLETE, VO_EVENTID_SOURCE2_ERR_DOWNLOADFAIL,0);
		return 0;
	}
	voCAutoLock lockupdate(&ptr->m_lockSeekAndUpdate);
	set_threadname("Async Update Url Thread");
	VO_U32 nTimeCheck_S = 0;
	RecordCostTime( nTimeCheck_S, true, NULL );	
	RecordCostTime(nTimeCheck_S,false,"Update Step1",3);
	{
		voCAutoLock lock(&ptr->m_lockURL);
		strcpy(ptr->m_update_url,ptr->m_tmp_update_url);
	}

	VO_BOOL bMaybeDRMModifyPlaylist = VO_FALSE;
	if(ptr->m_drm_api.Init && ptr->m_drm_api.hHandle)
	{
		/* exchange url if the url needs to decrypt*/
		VO_CHAR tmpURL[MAXURLLEN] = {0};
		VO_U32 ret1 = ptr->m_drm_api.PreprocessURL( ptr->m_drm_api.hHandle, ptr->m_update_url, tmpURL, NULL);
		if(ret1 == VO_RET_SOURCE2_OK)
		{
			if(strcmp(ptr->m_update_url, tmpURL))	// URL is not same after PreprocessURL
			{
				bMaybeDRMModifyPlaylist = VO_TRUE;

				memset(ptr->m_update_url, 0x00, sizeof(ptr->m_update_url));
				strcpy(ptr->m_update_url, tmpURL);
			}
			else
			{
				VOLOGI("URL is same after PreprocessURL.");
			}
		}
		else
		{
			VOLOGW("failed to PreprocessURL ret 0x%08X", ret1);
		}
	}
	VOLOGI("Update Url: %s", ptr->m_update_url);

	VO_ADAPTIVESTREAM_PLAYLISTDATA data;
	memset(&data, 0, sizeof(VO_ADAPTIVESTREAM_PLAYLISTDATA));
	memcpy(data.szUrl, ptr->m_update_url, sizeof(ptr->m_update_url));

	VO_U32 ret = DownloadItem_II(ptr->m_ptr_io, ptr->m_info.GetVerificationInfo(),ptr->m_info.GetIOHttpCallback(), &data, &ptr->m_pUpdateUrlPlaylistData,&ptr->m_UpdateUrlPlaylistDataSize, &ptr->m_UpdateUrlPlaylistDataUsedSize, &ptr->m_is_stop, VO_FALSE);
	if(ptr->m_is_stop)
	{
		ptr->NotifyEvent(VO_EVENTID_SOURCE2_UPDATEURLCOMPLETE, VO_EVENTID_SOURCE2_ERR_DOWNLOADFAIL,0);
		return VO_RET_SOURCE2_OPENFAIL;
	}
	RecordCostTime(nTimeCheck_S, false, "Update Step2. Download MainURL",3);
	
	if(ret != VO_SOURCE2_IO_OK || !ptr->m_pUpdateUrlPlaylistData || data.uDataSize == 0)
	{
		VOLOGE("Update error,DataPtr:%p,Size:%d,RetL0x%08x",ptr->m_pUpdateUrlPlaylistData,data.uDataSize,ret);
		ptr->NotifyEvent(VO_EVENTID_SOURCE2_UPDATEURLCOMPLETE, VO_EVENTID_SOURCE2_ERR_DOWNLOADFAIL,0);
		return VO_RET_SOURCE2_OPENFAIL;
	}

	// for DRM maybe modify content of playlist, we just let it go through if it is modified by DRM
	// maybe we need final solution for compare program info from streaming parser in future
	if(memcmp(ptr->m_pPlaylistData, ptr->m_pUpdateUrlPlaylistData, ptr->m_PlaylistDataSize) && VO_FALSE == bMaybeDRMModifyPlaylist)
	{
		VOLOGE("Update error. The Playlist should be the same. Old %d New %d", ptr->m_PlaylistDataSize, ptr->m_UpdateUrlPlaylistDataSize);
		ptr->NotifyEvent(VO_EVENTID_SOURCE2_UPDATEURLCOMPLETE,VO_EVENTID_SOURCE2_ERR_PLAYLISTPARSEERR,0);
		return VO_RET_SOURCE2_ERRORDATA;
	}
	
	VO_U32 readsize = data.uDataSize;
	VO_ADAPTIVESTREAMPARSER_STREAMTYPE stream_type;
	ptr->CheckStreamingType(ptr->m_pUpdateUrlPlaylistData, readsize, &stream_type);
	if(stream_type != VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS)
	{
		VOLOGE("Do not support update this kind of url for now");
		ptr->NotifyEvent( VO_EVENTID_SOURCE2_UPDATEURLCOMPLETE, VO_EVENTID_SOURCE2_ERR_DOWNLOADFAIL,0);
		return VO_RET_SOURCE2_NOIMPLEMENT;
	}
	ptr->m_streaming_type = stream_type;
	RecordCostTime( nTimeCheck_S, false, "Update Step3. CheckStreamingType() " ,3);

	if(ptr->m_drm_api.Init && ptr->m_drm_api.hHandle)
	{
		ptr->m_drm_api.SetParameter(ptr->m_drm_api.hHandle,VO_PID_AS_DRM2_STREAMING_TYPE, &ptr->m_streaming_type);
		VO_U32 ret1 = ptr->m_drm_api.Info(ptr->m_drm_api.hHandle,data.szNewUrl, data.pData, data.uDataSize,0);
		if(ret1 != VO_RET_SOURCE2_OK && ret1 != VO_ERR_DRM2_NO_DRM_API)
		{
			VOLOGE("failed to get DRM license, 0x%08x",ret1);
			ptr->NotifyEvent(VO_EVENTID_SOURCE2_UPDATEURLCOMPLETE, VO_EVENTID_SOURCE2_ERR_DRMFAIL,0);
			return VO_RET_SOURCE2_CONTENTENCRYPT;
		}
	}
	RecordCostTime(nTimeCheck_S, false, "Update Step4. DRM Init()", 3);

	ptr->PreProcessURL(&data);
	VOLOGI("root:%s\n,url:%s\n,newurl:%s\n,pData:%s",data.szRootUrl,data.szUrl,data.szNewUrl,data.pData);
	ret = ptr->m_ptr_parser->SetParam(VO_PID_ADAPTIVESTREAMING_UPDATEURL,&data);
	if(ret != VO_RET_SOURCE2_OK)
	{
		VOLOGE("Update URL setparam to streaming parser fail, 0x%08x",ret);
		ptr->NotifyEvent(VO_EVENTID_SOURCE2_UPDATEURLCOMPLETE, VO_EVENTID_SOURCE2_ERR_PLAYLISTPARSEERR,0);
		return VO_RET_SOURCE2_FAIL;
	}
	
	ptr->m_info.SetUpdateUrlOn(THREADTYPE_MEDIA_AUDIO,VO_TRUE);
	ptr->m_info.SetUpdateUrlOn(THREADTYPE_MEDIA_VIDEO, VO_TRUE);
//	ptr->m_info.SetUpdateUrlOn(THREADTYPE_MEDIA_SUBTITLE, VO_TRUE);

	RecordCostTime(nTimeCheck_S, false, "Update Step5. Updata URL",3);
	ptr->NotifyEvent(VO_EVENTID_SOURCE2_UPDATEURLCOMPLETE, 0,0);
	ptr->m_bUpdatingURL = VO_FALSE;
	return 0;
}

VO_U32 voAdaptiveStreamingController::BeginThread_UpdateUrl()
{
	unsigned int thread_id;
	create_thread( &m_threadHandleUpdateUrl , &thread_id , AsyncUpdateUrlThread, this , 0 );
	return 0;
}

VO_VOID voAdaptiveStreamingController::StopThread_UpdateUrl()
{
	m_bStopUpdateUrl = VO_TRUE;
	wait_thread_exit(m_threadHandleUpdateUrl);
	m_bStopUpdateUrl = VO_FALSE;
	m_threadHandleUpdateUrl = 0;
}

VO_U32 voAdaptiveStreamingController::UpdateUrl()
{
	BeginThread_UpdateUrl();
	return VO_RET_SOURCE2_OK;
}
#endif
