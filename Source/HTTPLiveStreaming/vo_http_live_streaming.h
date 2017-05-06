
#ifndef __VO_HTTP_STREAMING_H__

#define __VO_HTTP_STREAMING_H__

#include "OMX_Core.h"
#include "voOMX_Index.h"

#include "vo_m3u_manager.h"
#include "voMTVBase.h"
#include "voParser.h"
#include "CDllLoad.h"
#include "voHLSDRM.h"
#include "vo_network_judgment.h"
#include "bitrate_adaptation_buffer.h"
#include "vo_thread.h"

//
#include "voSource2.h"
#include "voAAC2.h"
//


#ifdef _HLS_SOURCE_
#include "voLiveSource.h"
#endif

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

struct Event
{
	VO_S32 id;
	VO_S32 param1;
	VO_S32 param2;
};


struct S_FilterForAdvertisementLoacal
{
    VO_CHAR strFilterString[256];
	VO_S32  iFilterId;
};

struct  S_HLS_USER_INFO
{
    VO_CHAR strUserName[256];
	VO_S32  ulstrUserNameLen;
    VO_CHAR strPasswd[256];
	VO_S32  ulstrPasswdLen;
};


#ifdef _HLS_SOURCE_
typedef VO_VOID ( *pDataCallback)( VO_VOID * ptr_obj , VO_LIVESRC_SAMPLE * ptr_buffer );
#else
typedef VO_VOID ( *pDataCallback)( VO_VOID * ptr_obj , OMX_BUFFERHEADERTYPE * ptr_buffer );
#endif
typedef VO_VOID ( *pEventCallback)( VO_VOID * ptr_obj , Event * ptr_buffer );

class vo_http_live_streaming
	:public vo_thread
{
public:
	vo_http_live_streaming(void);
	~vo_http_live_streaming(void);

	VO_BOOL open( VO_CHAR * ptr_url , VO_BOOL is_ansyc = VO_FALSE );
	VO_VOID close();
	VO_VOID set_datacallback( VO_VOID * ptr_obj , pDataCallback datacallback_func ){ m_ptr_callbackobj = ptr_obj; m_datacallback_func = datacallback_func; }
	VO_VOID set_eventcallback( VO_VOID * ptr_obj , pEventCallback eventcallback_func ){ m_ptr_eventcallbackobj = ptr_obj; m_eventcallback_func = eventcallback_func; }
	
	void	setWorkPath (const char * pWorkPath);

	VO_BOOL is_live(){ return m_manager.is_live(); }

	VO_S32 get_duration(){ return m_manager.get_duration(); }
	
	VO_S32 set_pos( VO_S32 pos );

	VO_VOID pause();
	VO_VOID run();

	VO_VOID set_DRM( void * ptr_drm );	
	VO_BOOL setDrmType(VO_S32*   pIDrmType);

	VO_VOID set_videodelay( int * videodelaytime );
	VO_VOID set_libop(void*   pLibOp);
	VO_VOID Add_AdFilterInfo(void*   pAdFilterInfo);
	VO_S32  FindFilterIndex(VO_CHAR*  pFilterString);
	VO_VOID DoNotifyForAD( media_item * ptr_item, VO_S32  iFilterIndex);
	VO_VOID DoNotifyForThumbnail();
	VO_VOID SetUserName(VO_VOID*  pStrUserName);
	VO_VOID SetUserPassWd(VO_VOID*  pStrPassWD);


	
	VO_VOID setCpuInfo(VO_VOID* pCpuInfo);
	VO_VOID setCapInfo(VO_VOID* pCapInfo);

	VO_VOID NotifyMediaPlayType(VO_U32   ulMediaPlayType);
	VO_VOID NotifyTheBandWidth();
	VO_VOID DoTransactionForID3(VO_PTR pData);
    VO_VOID DumpAACPureData(VO_BYTE*   pData, VO_U32 ulLen, VO_U32 ulFlag);

	



protected:
	virtual void thread_function();
	VO_VOID start_livestream();
	VO_VOID stop_livestream( VO_BOOL isflush = VO_FALSE );

	VO_VOID load_ts_parser();
	VO_VOID free_ts_parser();

	VO_VOID load_aac_parser();
    VO_VOID free_aac_parser();	

	VO_S64 GetMediaItem( media_item * ptr_item );
	VO_S64 GetItem( vo_webdownload_stream * ptr_stream , VO_S32 eReloadType , VO_BOOL is_quick_fetch = VO_FALSE );

	VO_S32 read_buffer( vo_webdownload_stream * ptr_stream , VO_PBYTE buffer , VO_S32 size );
	VO_S32 readbuffer_determinesize( VO_PBYTE * ppBuffer , vo_webdownload_stream * ptr_stream );

	static void VO_API ParserProc(VO_PARSER_OUTPUT_BUFFER* pData);
	static VO_S32 VO_API ParserProcAAC(VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData);
	
	static VO_VOID bufferframe_callback( VO_BOOL is_video , VO_MTV_FRAME_BUFFER * ptr_buffer , VO_PTR ptr_obj );

	VO_VOID audio_data_arrive( VO_MTV_FRAME_BUFFER * ptr_buffer );
	VO_VOID video_data_arrive( VO_MTV_FRAME_BUFFER * ptr_buffer );

	VO_VOID send_media_data( VO_MTV_FRAME_BUFFER * ptr_buffer , VO_U32 index , VO_BOOL newfile = VO_FALSE );
	VO_VOID send_eos();

	VO_VOID CheckProfileLevel (VO_PBYTE pHeadData, VO_U32 nHeadSize);

	VO_VOID perpare_drm();
	VO_VOID Prepare_HLSDRM();	
	VO_VOID Do_HLSDRM_Process(VO_PBYTE pData, VO_U32  ulDataLen);
	VO_VOID Prepare_HLSDRM_Process();
	
	VO_VOID After_HLSDRM_Process();
	VO_VOID release_drm();

	VO_VOID start_after_seek();

	VO_VOID need_flush();

    VO_VOID send_audio_trackinfo();
    VO_VOID send_video_trackinfo();

	void    LoadWorkPathInfo();
	void    ResetAllFilters();
	void    ResetAllIDs();

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

	CDllLoad			m_dlEngine;
	char				m_szWorkPath[256];

	VO_PTR				m_tsparser_handle;
	VO_PARSER_API		m_tsparser_api;

	VO_PTR				m_aacparser_handle;
	VO_SOURCE2_API		m_aacparser_api;
	VO_S32 m_iCurrentMediaType;


	VO_VOID * m_ptr_callbackobj;
	pDataCallback m_datacallback_func;

	VO_VOID * m_ptr_eventcallbackobj;
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
	VO_S32 m_iVideoDelayCount; 
    //
	VO_S64 m_seekpos;
	VO_BOOL m_is_afterseek;

	int m_mediatype;
	VO_BOOL m_is_mediatypedetermine;

    Persist_HTTP m_persist;

    VO_LIVESRC_TRACK_INFOEX * m_ptr_audioinfo;
    VO_LIVESRC_TRACK_INFOEX * m_ptr_videoinfo;

    VO_S64 m_last_video_timestamp;
    VO_S64 m_last_audio_timestamp;

    VO_S64 m_download_bitrate;
    VO_S64 m_rightbandwidth;

    VO_S32 m_brokencount;

	void*  m_pDrmCallback;
	VO_S32 m_iSeekResult;

    //
    VO_S32 m_iDrmType;
	VO_S32 m_iProcessSize;
    //

    S_HLS_USER_INFO                   m_sHLSUserInfo;
    S_Thumbnail_Item*                 m_pThumbnailList;
    S_FilterForAdvertisementLoacal    m_aFilterForAdvertisement[8];
	VO_S32                            m_iFilterForAdvertisementCount;

	VO_S32                            m_iLastCharpterID;
	VO_S32                            m_iLastSequenceID;
	VO_BOOL                           m_bNeedUpdateTimeStamp;
};

#ifdef _VONAMESPACE
}
#endif

#endif
