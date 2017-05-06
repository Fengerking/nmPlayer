
#ifndef __VO_HTTP_STREAMING_NEW_H__

#define __VO_HTTP_STREAMING_NEW_H__

#include "OMX_Core.h"
#include "voOMX_Index.h"

#include "voSource2.h"

#include "vo_m3u_manager.h"
#include "voMTVBase.h"
#include "voParser.h"
#include "CDllLoad.h"
#include "DRM_API.h"
#include "vo_network_judgment.h"
#include "bitrate_adaptation_buffer.h"
#include "vo_thread.h"
#include "voSourceBufferManager.h"

#include "voAAC2.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif



struct Event
{
	VO_S32 id;
	VO_S32 param1;
	VO_S32 param2;
};

struct  S_HLS_USER_INFO
{
    VO_CHAR strUserName[256];
	VO_S32  ulstrUserNameLen;
    VO_CHAR strPasswd[256];
	VO_S32  ulstrPasswdLen;
};

struct S_FilterForAdvertisementLoacal
{
    VO_CHAR strFilterString[256];
	VO_S32  iFilterId;
};

typedef	VO_S32 ( *pDataCallback) (VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData);
typedef	VO_S32 ( *pEventCallback) (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);


class vo_http_live_streaming_new
	:public vo_thread
{
public:
	vo_http_live_streaming_new(void);
	~vo_http_live_streaming_new(void);

    //the new interface 
    VO_U32     Init_HLS(VO_PTR  pSource, VO_U32   nFlag, VO_SOURCE2_INITPARAM*  pInitParam);
    VO_U32     Uninit_HLS();
	VO_U32     Open_HLS();
	VO_U32     Close_HLS();
	
	VO_U32     Start_HLS();
	VO_U32     Pause_HLS();
	VO_U32     Stop_HLS();
	VO_U32     Seek_HLS(VO_U64*  pTimeStamp);
	VO_U32     GetDuration_HLS(VO_U64 * pDuration);
	VO_U32     GetSample_HLS(VO_U32  nSampleType, VO_PTR  pSample);	
	VO_U32     GetProgramCounts_HLS(VO_U32*  pProgramCounts);
	VO_U32     GetProgramInfo_HLS(VO_U32 nProgramId, VO_SOURCE2_PROGRAM_INFO **ppProgramInfo);
	VO_U32     GetCurSelTrackInfo_HLS(VO_U32 nOutPutType , VO_SOURCE2_TRACK_INFO **ppTrackInfo);
	VO_U32     SelectProgram_HLS(VO_U32 nProgramID);
	VO_U32     SelectStream_HLS(VO_U32 nStreamID);	
	VO_U32     SelectTrack_HLS(VO_U32 nTrackID);
	VO_U32     GetDRMInfo_HLS(VO_U32 nStreamId, VO_SOURCE2_DRM_INFO **ppDRMInfo);
	
    //the new interface 

	//add for new interface
	VO_VOID FlushDataBuffer();
	VO_VOID FlushBuffer();
    VO_S32  InitDataBuffer();
    VO_S32  UnInitDataBuffer();	

	VO_VOID ResetAllContext();
	VO_VOID SetTheCurrentSelStream(VO_U32 uBitrate);

	VO_VOID SendAudioTrackinfo(VO_U64   ullTimeStamp);
    VO_VOID SendVideoTrackinfo(VO_U64   ullTimeStamp);
	
	VO_VOID DoTransactionForID3(VO_PTR pData);
	VO_VOID SendEos();
	VO_VOID SendMediaTrackDisable(VO_U32   ulTrackType, VO_U64   ullTimeStamp);

	VO_VOID NotifyMediaPlayType(VO_U32   ulMediaPlayType);
	VO_VOID NotifyTheBandWidth();
    VO_VOID NotifyTheNetworkBroken();
	VO_U32  CheckFileFormat(VO_PBYTE  pData, VO_U32 ulLen);
	VO_VOID DoTransactionForChunkMediaTypeChanged();
	VO_U32  DoPrePareForTheChuckMediaParser(VO_U32   ulMediaPlayType);

    //add for new interface


    //add for the aac push parser
	VO_VOID LoadAACParser();
    VO_VOID FreeAACParser();	
	VO_VOID SetParamToParser();
	VO_VOID SetParamForLog(VO_VOID*  pParam);
	VO_VOID SetVerificationInfo(VO_VOID*  psVerificationInfo);
    //add for the aac push parser


	VO_BOOL open( VO_CHAR * ptr_url , VO_BOOL is_ansyc = VO_FALSE );
	VO_VOID close();
	VO_VOID set_datacallback( VO_VOID * ptr_obj , pDataCallback datacallback_func ){ m_ptr_callbackobj = ptr_obj; m_datacallback_func = datacallback_func; }
	VO_VOID set_eventcallback( VO_VOID * ptr_obj , pEventCallback eventcallback_func ){ m_ptr_eventcallbackobj = ptr_obj; m_eventcallback_func = eventcallback_func; }
    VO_VOID set_eventcallbackForManifest(VO_PTR ptrEventcallback){m_manager.SetEventCall(ptrEventcallback);}
	
	void	setWorkPath (const char * pWorkPath);

	VO_BOOL is_live(){ return m_manager.is_live(); }

	VO_S32 get_duration(){ return m_manager.get_duration(); }
	
	VO_S32 set_pos( VO_S32 pos );

	VO_VOID pause();
	VO_VOID run();

	VO_VOID set_DRM( void * ptr_drm );

	VO_VOID set_videodelay( int * videodelaytime,  VO_S64* pCurrentPlayTime );
	VO_VOID set_libop(void*   pLibOp);
	VO_VOID Add_AdFilterInfo(void*   pAdFilterInfo);
	VO_S32  FindFilterIndex(VO_CHAR*  pFilterString);
	VO_VOID DoNotifyForAD( media_item * ptr_item, VO_S32  iFilterIndex);
	VO_VOID DoNotifyForThumbnail();




	
	VO_VOID setCpuInfo(VO_VOID* pCoreInfo);
	VO_VOID setCapInfo(VO_VOID* pAbilityCapInfo);
	VO_VOID setStartCap( VO_VOID* pStartBitrate );
    VO_VOID setTheMaxDownloadFailTolerantCount(VO_PTR   pulMax);
    VO_VOID TransactionForDownloadResult(VO_S64 illDownloadResult);
    VO_VOID NotifyRecoverFromTheDownloadFail();
    VO_VOID NotifyBeginWaitingDownloadFailRecover();
    VO_VOID setCPUWorkMode(VO_PTR  pParam);



protected:

    VO_VOID CloseAllDump();
	VO_VOID Prepare_HLSDRM();
	VO_VOID Do_HLSDRM_Process(VO_PBYTE pData, VO_U32  *ptr_ulDataLen);
	VO_VOID Prepare_HLSDRM_Process( VO_BYTE * ptr_key , VO_BYTE * ptr_iv );	
	VO_VOID After_HLSDRM_Process( VO_BYTE * ptr_buf , VO_U32 * ptr_size );

	
	virtual void thread_function();
	VO_VOID start_livestream();
	VO_VOID stop_livestream( VO_BOOL isflush = VO_FALSE );

	VO_VOID load_ts_parser();
	VO_VOID free_ts_parser();

	VO_S64 GetMediaItem( media_item * ptr_item );
	VO_S64 GetItem( vo_webdownload_stream * ptr_stream , VO_S32 eReloadType , VO_BOOL is_quick_fetch = VO_FALSE );

	VO_S32 read_buffer( vo_webdownload_stream * ptr_stream , VO_PBYTE buffer , VO_S32 size );
	VO_S32 readbuffer_determinesize( VO_PBYTE * ppBuffer , vo_webdownload_stream * ptr_stream);

	static void VO_API ParserProc(VO_PARSER_OUTPUT_BUFFER* pData);
	static VO_S32 VO_API ParserProcAAC(VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData);

	static VO_VOID bufferframe_callback( VO_BOOL is_video , VO_MTV_FRAME_BUFFER * ptr_buffer , VO_PTR ptr_obj );

	VO_VOID audio_data_arrive_in_bufferlist( VO_MTV_FRAME_BUFFER * ptr_buffer );
	VO_VOID video_data_arrive_in_bufferlist( VO_MTV_FRAME_BUFFER * ptr_buffer );
    VO_VOID subtitle_data_arrive_in_bufferlist(VO_PTR  pData);

	

	VO_VOID audio_data_arrive( VO_MTV_FRAME_BUFFER * ptr_buffer );
	VO_VOID video_data_arrive( VO_MTV_FRAME_BUFFER * ptr_buffer );


	VO_VOID send_media_data( VO_MTV_FRAME_BUFFER * ptr_buffer , VO_U32 index , VO_BOOL newfile = VO_FALSE );

	VO_VOID send_eos();

	VO_VOID CheckProfileLevel (VO_PBYTE pHeadData, VO_U32 nHeadSize);
    VO_U32  DownlLoadKeyForAES(vo_webdownload_stream *ptr_stream, VO_PBYTE buffer, VO_U32 ulSize);

	VO_VOID perpare_drm();
	VO_VOID release_drm();

	VO_VOID start_after_seek();

	VO_VOID need_flush();

    VO_VOID send_audio_trackinfo();
    VO_VOID send_video_trackinfo();

	void    LoadWorkPathInfo();
	void    ResetAllFilters();
	void    ResetAllIDs();
	void    DumpAACFromHttp(VO_BYTE*   pData, VO_U32 ulDataLen, VO_U32 ulFlag);
	void    DumpTSFromHttp(VO_BYTE*   pData, VO_U32 ulDataLen, VO_U32 ulFlag);	
	void    DumpTSFromDrm(VO_BYTE*   pData, VO_U32 ulDataLen, VO_U32 ulFlag);		
    void    DumpInputVideo(VO_PTR  pSample);
    void    DumpInputAudio(VO_PTR  pSample);
    void    DumpInputID3(VO_PTR  pSample);
	void	DumpOutputVideo(VO_PTR  pSample);
    void	DumpOutputAudio(VO_PTR  pSample);
	void    DeleteAllProgramInfo();
	

protected:
	vo_network_judgment m_judgementor;
	vo_m3u_manager m_manager;
	VO_BOOL m_is_first_frame;
	VO_BOOL m_new_video_file;
	VO_BOOL m_new_audio_file;
	VO_BOOL m_bMediaStop;
	VO_BOOL m_is_pause;
	VO_BOOL m_recoverfrompause;

	VO_BOOL m_is_flush;

	VO_BOOL m_is_seek;
    VO_BOOL m_is_first_start;
    VO_BOOL m_is_first_getdata;
   
	CDllLoad			m_dlEngine;
	char				m_szWorkPath[256];
	char                m_strManifestURL[1024];

	VO_PTR				m_tsparser_handle;
	VO_PARSER_API		m_tsparser_api;

    VO_PTR				m_aacparser_handle;
    VO_SOURCE2_API		m_aacparser_api;
    VO_S32 m_iCurrentMediaType;


	VO_VOID * m_ptr_callbackobj;
	VO_VOID * m_ptr_eventcallbackobj;


	pDataCallback m_datacallback_func;

	pEventCallback m_eventcallback_func;

	VO_S32				m_keytag;

	VO_CHAR				m_last_keyurl[1024];

	DRM_Callback		m_drm_eng;
	void *				m_drm_eng_handle;


	VO_BOOL				m_is_video_delayhappen;
	VO_BOOL				m_is_bitrate_adaptation;

	bitrate_adaptation_buffer m_adaptationbuffer;

	VO_S64 m_audiocounter;
	VO_S64 m_videocounter;

	VO_S64 m_timestamp_offset;
	VO_S64 m_last_big_timestamp;
	//
	VO_BOOL m_bDumpRawData;
	VO_S32 m_iVideoDelayCount;

    VO_U32 m_ulTSDumpCount;
    VO_U32 m_ulTSFromDrm;
	VO_U32 m_ulAACDumpCount;
	VO_U32 m_iDumpVideoCount;
	VO_U32 m_iDumpAudioCount;
	VO_U32 m_iDumpVideoOutputCount;
	VO_U32 m_iDumpAudioOutputCount;

	VO_U32 m_ulForceSelect;
    VO_U32 m_ulSelectBandwidth;

	VO_U32 m_ulCheckTime;
	
    //
	VO_S64 m_seekpos;
	VO_BOOL m_is_afterseek;

	int m_mediatype;
	VO_BOOL m_is_mediatypedetermine;

    Persist_HTTP m_persist;

    VO_SOURCE2_TRACK_INFO * m_ptr_audioinfo;
    VO_SOURCE2_TRACK_INFO * m_ptr_videoinfo;

	//Test For Buffer Manager
    voSourceBufferManager*      m_pBufferManager;
	
    //voSource2DataBufferItemList*    m_pDataBufferListForVideo;
    //voSource2DataBufferItemList*    m_pDataBufferListForAudio;
    //voSource2DataBufferItemList*    m_pDataBufferListForSubTitle;

	//Test For Buffer Manager

    VO_S64 m_last_video_timestamp;
    VO_S64 m_last_audio_timestamp;

    VO_S64 m_illLastAudioOutputTime;	
    VO_S64 m_illLastVideoOutputTime;

    VO_S64 m_download_bitrate;
    VO_S64 m_rightbandwidth;

    VO_S32 m_brokencount;

	void*  m_pDrmCallback;
	VO_S32 m_iSeekResult;


    S_Thumbnail_Item*                 m_pThumbnailList;
    S_FilterForAdvertisementLoacal    m_aFilterForAdvertisement[8];
	VO_S32                            m_iFilterForAdvertisementCount;

	VO_S32                            m_iLastCharpterID;
	VO_S32                            m_iLastSequenceID;
	VO_BOOL                           m_bNeedUpdateTimeStamp;
	VO_BOOL                           m_bEosReach;
	VO_BOOL                           m_bNeedBuffering;
    VO_BOOL                           m_bDrmNeedWork;

    S_HLS_USER_INFO                   m_sHLSUserInfo;
	//add for new interface
	VO_SOURCE2_PROGRAM_INFO*           m_pProgramInfo;
	//add for new interface
    VO_S32 m_iDrmType;
    VO_BOOL m_bWorkModeForDiscretixPlayReady;
    VO_S32 m_iProcessSize;

	VO_S32 m_delaycount;
	VO_BOOL m_isdeterminenextbitrate;
	VO_VOID*   m_pLogParam;
	VO_BOOL    m_bChuckMediaTypeChanged;
	VO_U32     m_ulCurrentSequenceIDForTimeStamp;
    VO_U32     m_ulMaxDownloadFailTolerantCount;
    VO_U32     m_ulCurrentDownloadFailConsistentCount;
	

};

#ifdef _VONAMESPACE
}
#endif

#endif
