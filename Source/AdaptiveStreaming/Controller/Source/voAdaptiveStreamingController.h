#ifndef __VOADAPTIVESTREAMINGCONTROLLER_H__

#define __VOADAPTIVESTREAMINGCONTROLLER_H__

#include "voAdaptiveStreamParserWrapper.h"
#include "voAdaptiveStreamingControllerInfo.h"
#include "voChannelItemThread.h"

#ifdef _USE_BUFFERCOMMANDER
#include "voBufferCommander.h"
#else
#include "voSourceBufferManager_AI.h"
#endif

#include "voBitrateAdaptationImp.h"
#include "voStreamingDRM.h"
#include "vo_thread.h"
#include "vo_singlelink_list.hpp"

#include "voEVENTHandler.h"

#include "vo_timestamp_recalculator.h"

#include "voProgramInfoOp.h"

#include "vo_tracksample_filter.h"

//#define DUMP_MANIFEST


//#define _USE_SHARELIB
#ifdef _USE_SHARELIB
#include "voShareLib.h"
#endif

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

//#define _HTTPHEADER_TEST

enum CONTROLLER_STATUS
{
	CONTROLLER_STATUS_UNKNOWN,
	CONTROLLER_STATUS_RUNNING,
	CONTROLLER_STATUS_STOP,
};

class voAdaptiveStreamingController
	: public vo_thread
{
public:
	voAdaptiveStreamingController(void);
	virtual ~voAdaptiveStreamingController(void);

	virtual VO_U32 Init( VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pInitParam );
	virtual VO_U32 Uninit();
	virtual VO_U32 Open();
	virtual VO_U32 Close();
	virtual VO_U32 Start();
	virtual VO_U32 Pause();
	virtual VO_U32 Stop();
	virtual VO_U32 Seek( VO_U64* pTimeStamp);
	virtual VO_U32 GetDuration( VO_U64 * pDuration);
	virtual VO_U32 GetSample( VO_SOURCE2_TRACK_TYPE nTrackType , VO_PTR pSample );
	virtual VO_U32 GetProgramCount( VO_U32 *pProgramCount);
	virtual VO_U32 GetProgramInfo( VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **ppProgramInfo);
	virtual VO_U32 GetCurTrackInfo(  VO_SOURCE2_TRACK_TYPE nTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo );
	virtual VO_U32 SelectProgram( VO_U32 nProgram);
	virtual VO_U32 SelectStream( VO_U32 nStream);
	virtual VO_U32 SelectTrack( VO_U32 nTrack);
	virtual VO_U32 GetDRMInfo( VO_SOURCE2_DRM_INFO **ppDRMInfo);
	virtual VO_U32 SendBuffer( const VO_SOURCE2_SAMPLE& buffer );
	virtual VO_U32 GetParam( VO_U32 nParamID, VO_PTR pParam);
	virtual VO_U32 SetParam( VO_U32 nParamID, VO_PTR pParam);

	virtual VO_U32 ToSelectTrack( VO_U32 nTrack );
	virtual VO_U32 ToSelectStream( VO_U32 nStream );
	virtual VO_BOOL CheckIsOpenComplete();
protected:
	virtual VO_U32 ToOpenInternal();
	/*.............OpenInternalII instead of
	virtual VO_U32 OpenInternal();
	............*/
	virtual VO_U32 OpenInternalII();
	virtual VO_U32 SeekAsync(VO_U64 * pTimeStamp);
	virtual VO_U32 UpdateUrl();

	virtual VO_U32 CheckStreamingType( VO_CHAR * ptr_buffer , VO_U32 size );
	virtual VO_U32 CheckStreamingType( VO_CHAR * ptr_buffer , VO_U32 size,VO_ADAPTIVESTREAMPARSER_STREAMTYPE* stream_type  );
#ifndef _USE_BUFFERCOMMANDER
	virtual VO_U32 SetBufferTime(voSourceBufferManager_AI * pSBM, VO_BUFFER_SETTINGS_TYPE type, VO_U32 nTime, VO_BOOL &hasSet);
#endif
	static VO_S32 OnEvent(VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);
	static VO_S32 OnSample(VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData);

	VO_VOID	OnChunkFirstSample(VO_U64 uChunkID, VO_U64 uTimeStamp);
	
	virtual VO_U32 OpenComplete();
	//VO_VOID AddProgramInfo2Buffer(_PROGRAM_INFO *pInfo);

	//virtual VO_U32 ParseItem( VO_ADAPTIVESTREAM_PLAYLISTDATA * pUrl );

	
	/*pSize, indicate the *ppBuffer's length.*/
	/*.............DownloadItem instead of
	virtual VO_U32 DownloadItem(VO_SOURCE2_IO_API *pIoApi, VO_SOURCE2_VERIFICATIONINFO * pVerificationInfo, VO_ADAPTIVESTREAM_PLAYLISTDATA * pData, VO_CHAR **ppBuffer, VO_U32 *pSize, VO_BOOL * pCancel );
	................*/

	virtual VO_VOID SetStopFlag( VO_BOOL bStop = VO_TRUE );

	virtual VO_U64 DownloadEndColletor( CHUNKINFO * pChunkInfo );
	virtual VO_U32 DownloadStartColletor( CHUNKINFO * pChunkInfo );
	virtual VO_U32 ChunkBegin( CHUNKINFO * pChunkInfo, VO_BOOL bFormatChanged );
	virtual VO_U32 ChunkComplete( CHUNKINFO * pChunkInfo );

	virtual VO_U32 ToChangeBitrate();
	virtual VO_U32 ChangeBitrate();
	virtual VO_U32 ToChangeBitrateWhenError();
	virtual VO_U32 ChangeBitrateWhenError();

	virtual void thread_function();

	virtual VO_U32 NotifyEvent( VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2 );

	virtual VO_U32 ChangeTrack(VO_BOOL * pbChanged = NULL);

	virtual VO_S32 SendEvent( VO_U32 eventType, VO_U32 eventInfo, VO_ADAPTIVESTREAM_PLAYLISTDATA* pData );

	virtual VO_U32 PreProcessURL(VO_ADAPTIVESTREAM_PLAYLISTDATA * pData);
	VO_VOID InitForDRM(VO_StreamingDRM_API *pDRM);

	//for async seek 
	static unsigned int AsyncSeekThread(void * pParam );
	VO_U32 BeginThread_Seek( );
	VO_VOID StopThread_Seek( );

	//for async update url
	static unsigned int AsyncUpdateUrlThread(void* pParam);
	VO_U32 BeginThread_UpdateUrl();
	VO_VOID StopThread_UpdateUrl();

	// get current play list start time for live (DVR window start time)
	virtual VO_U32	GetCurrentStartTime(VO_U64 * puStartTime);

	// update DVR information
	virtual VO_U32	UpdateDVRInfo();
#ifdef _USE_BUFFERCOMMANDER
	VO_VOID CheckBufferReady();
	VO_U32 GetMinBufferDuration();
#endif
#ifdef _HTTPHEADER_TEST
	VO_SOURCE2_IO_HTTPCALLBACK httpCB;
	static VO_U32 IO_HTTP_CALLBACK_FUNC_TEST( VO_PTR hHandle , VO_U32 uID , VO_PTR pParam1 , VO_PTR pParam2  );
#endif
protected:
	voAdaptiveStreamParserWrapper * m_ptr_parser;

	VO_CHAR m_url[MAXURLLEN];
	VO_CHAR m_update_url[MAXURLLEN];
	VO_CHAR m_tmp_update_url[MAXURLLEN];
	VO_U32 m_nFlag;

	VO_ADAPTIVESTREAMPARSER_STREAMTYPE	m_streaming_type;

	CDllLoad		  m_ioloader;
	VO_SOURCE2_IO_API m_internalio;

	VO_SOURCE2_IO_API * m_ptr_io;

	VO_BITRATE_ADAPTATION_API * m_ptr_ba;
	VO_BITRATE_ADAPTATION_API m_internalba;


	VO_SOURCEDRM_CALLBACK2 * m_ptr_drmcallback;
	VO_StreamingDRM_API m_drm_api;

	voChannelItemThread * m_ptr_thread_audio;
	VO_StreamingDRM_API m_drm_api_audio;

	voChannelItemThread * m_ptr_thread_video;
	VO_StreamingDRM_API m_drm_api_video;

	voChannelItemThread * m_ptr_thread_subtitle;
	VO_StreamingDRM_API m_drm_api_subtitle;

#ifndef _USE_BUFFERCOMMANDER
	voSourceBufferManager_AI * m_ptr_samplebuffer;
#endif
	VO_SOURCE2_EVENTCALLBACK m_EventCallback;
	VO_SOURCE2_SAMPLECALLBACK m_SampleCallback;
	VO_SOURCEIO_EVENTCALLBACK m_IOEventCallback;
	VO_BOOL m_is_stop;

	VO_BOOL m_is_pause;

	VO_BOOL m_is_start;

	//for playlist data cache with VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_NEEDPARSEITEM
	VO_CHAR * m_pPlaylistData;
	VO_U32 m_PlaylistDataSize;
	VO_U32 m_PlaylistDataUsedSize;
	
	VO_CHAR * m_pUpdateUrlPlaylistData;
	VO_U32 m_UpdateUrlPlaylistDataSize;
	VO_U32 m_UpdateUrlPlaylistDataUsedSize;

	VO_U32 m_CurBitrate;

	VO_BOOL m_audio_thread_eos;
	VO_BOOL m_video_thread_eos;

	VO_BOOL m_enableBA;
	VO_BOOL m_DisableCpuBA;

	VO_U32 m_SelBitrate;

	VO_BOOL m_bStopSeek;
	VO_BOOL m_bStopUpdateUrl;


	VO_StreamingEVENTHandler_API m_eventHandler_api;
	VO_SOURCE2_EVENTCALLBACK * m_ptr_eventcallback;

	VO_BOOL m_contain_video;

	//VO_SOURCE2_PROGRAM_TYPE m_programtype;

	VO_BOOL m_b_offset;
	VO_U64 m_timestamp_offset;

	voAdaptiveStreamingControllerInfo m_info;
	VO_SOURCE2_BITRATE_INFO bitrate_info;

	VO_SOURCE2_BA_THRESHOLD m_ba_threshold;

	VO_U32 m_TolerantErrorCount;
	VO_U32 m_RetryTimeOut;

	vo_singlelink_list< VO_U32 > m_SelTrackList;
	voCMutex m_SelTrackLock;

	VO_BOOL m_hasSetBufferingTime;
	VO_BOOL m_hasSetStartBufferTime;
#ifndef _USE_BUFFERCOMMANDER
	VO_U32 m_nBufferingTime;
	VO_U32 m_nStartBufferTime;
	VO_U32 m_nMaxBufferTime;
#endif
	VO_U64 m_nLiveDelayTime;
	VO_U32 m_nBufferDuration;

	/*For DVR*/
	VO_SOURCE2_SEEKRANGE_INFO m_DVRInfo;

	/*timestamp recalculator*/
	vo_timestamp_recalculator m_TimeStampRecalculator;



	//for log
	VO_U64 m_uLastAudio;
	VO_U64 m_uLastVideo;
	VO_U64 m_uLastSubtitle;
	VO_LOG_PRINT_CB * m_pVologCB;

	VO_U32 m_uCheckedGapTime_forAudio;
	VO_U32 m_uCheckedGapTime_AudioCounts;
	VO_U32 m_uCheckedGapTime_forVideo;
	VO_U32 m_uCheckedGapTime_VideoCounts;
	VO_U32 m_uCheckedGapTime_forSubtitle;
	VO_U32 m_uCheckedGapTime_SubtitleCounts;

	CIntervalPrintf m_intervalPrintf_Audio;
	CIntervalPrintf m_intervalPrintf_Video;
	CIntervalPrintf m_intervalPrintf_Subtitle;


#ifdef _USE_BUFFERCOMMANDER
	VO_BOOL m_bIsFirstChunk;
#endif

	//programinfo operation
	voProgramInfoOp *m_pProgramInfoOP;
	VO_DATASOURCE_CALLBACK m_ds_callback;
	//
	vo_tracksample_filter *m_pTrackSampleFilter;

	VO_SOURCE2_TRACK_INFO *m_pTrackInfoA;
	VO_SOURCE2_TRACK_INFO *m_pTrackInfoV;
	VO_SOURCE2_TRACK_INFO *m_pTrackInfoS;

	VO_U32  m_nCheckTime_forSeek;
	VO_U32	m_nCheckTime;
	//for http callback, cookie
	VO_SOURCE2_HTTPHEADER * m_pIOHttpHeader;
	//for http poxy setting
	VO_SOURCE2_HTTPPROXY * m_pIOHttpProxy;
	VO_BOOL m_bOpenComplete;

	voCMutex		m_lockSeek;
	THREAD_HANDLE	m_threadHandleSeek;
	VO_U64			m_nPositionSeek;
	VO_BOOL			m_bSeeking;
	VO_BOOL			m_bSeekingBySelectTrack;

	THREAD_HANDLE 	m_threadHandleUpdateUrl;
	
	VO_BOOL			m_bNeedSwitchAudioThread;
	VO_BOOL			m_bDashInitDataReady;
#ifdef _USE_SHARELIB
	VO_ShareLib_API m_sharelibApi;
#endif

	//Status of Controller
	CONTROLLER_STATUS m_status;

	voCMutex		m_mtxLastChunk;
	VO_U64			m_uLastChunkID;
	VO_U64			m_uLastChunkTimeStamp;

	VO_BOOL		m_bAudioUnavailableBlock;
	VO_BOOL		m_bVideoUnavailableBlock;
	VO_BOOL		m_bSubtitleUnavailableBlock;

	voCMutex	m_lockSeekAndUpdate;
	voCMutex       m_lockURL;
	VO_BOOL		m_bUpdatingURL;
#ifdef DUMP_MANIFEST
	VO_BOOL		m_bFirstSubManifest;
#endif
	VO_BOOL		m_bOutSideSourceIO;
	VO_U64		m_uDuration;
#ifdef _USE_BUFFERCOMMANDER
private:
	voBufferCommander m_BufferCommander;
#endif
};

#ifdef _VONAMESPACE
}
#endif
#endif
