
#ifndef __VO_MPD_STREAMING_H_
#define __VO_MPD_STREAMING_H_
#include "voSource2.h"

//#include "OMX_Core.h"
//#include "voOMX_Index.h"
#include  "vo_mpd_manager.h"
#include "voDashBase.h"
#include "voParser.h"
#include "CDllLoad.h"
#include "DRM_API.h"
#include "vo_network_judgment.h"
//#include "bitrate_adaptation_buffer.h"
#include "vo_thread.h"
#include "CPtrList.h"
#include "voXMLLoad.h"
#include "voLiveSource.h"
//#include "voLiveSrcDataBuffer.h"





#ifdef _DASH_SOURCE_
struct Event_Dash
{
	VO_U32 nID; 
	VO_U32 nParam1;
	VO_U32 nParam2;
};
typedef VO_VOID ( *pDataCallback)( VO_VOID * ptr_obj , VO_LIVESRC_SAMPLE * ptr_buffer );
typedef VO_VOID	( *pStatusCallback)(VO_VOID * ptr_obj, VO_U32 id, VO_VOID * pBuffer);


#else
typedef VO_VOID	( *pStatusCallback)(VO_VOID * ptr_obj, VO_U32 id, VO_VOID * pBuffer);
typedef VO_VOID ( *pDataCallback)( VO_VOID * ptr_obj , OMX_BUFFERHEADERTYPE * ptr_buffer );
#endif
typedef VO_VOID ( *pEventCallback_dash)( VO_VOID * ptr_obj , Event_Dash * ptr_buffer );



//VO_VOID * m_ptr_statuscallbackobj;
//pStatusCallback m_statuscallback_func;

typedef struct  
{
	VO_DASH_FRAME_BUFFER * ptr_buffer;
	VO_U32 index ;
	VO_BOOL newfile;
}DATA_BUFFER_ST_DASH;

typedef struct  
{
	VO_VOID* ptr_buffer;
	VO_U32 id ;
}STATUS_BUFFER_ST_DASH;
typedef struct
{
	VO_VOID* buffer;
	VO_U32 type;
}CALLBACK_BUFFER_ST_DASH;

class vo_mpd_streaming: public vo_thread
{
public:
	 vo_mpd_streaming(void);
	~vo_mpd_streaming(void);
	VO_BOOL  SetDataSource( VO_CHAR * ptr_url , VO_BOOL is_ansyc = VO_FALSE );
	VO_VOID  start_dashstream();
	VO_VOID  stop_dashstream( VO_BOOL isflush );
	VO_BOOL  is_live(){ return m_manager.is_live(); }
	VO_BOOL  is_update(){ return m_manager.is_needupdate(); }
	VO_VOID  close();
	VO_S64   set_pos(VO_S32 pos);
	VO_S32   setLang(VO_CHAR * lang);
	VO_VOID  set_VideoDelayTime(VO_PTR pValue,VO_U64 timeStamp =0);
	void     SetLibOp(VO_LIB_OPERATOR* pValue);
	VO_VOID  pause();
	VO_S32   GetDuration(){ return m_manager.get_duration(); }
    VO_PBYTE GetMediacodecItem( VO_CHAR * ptr_item);
	VO_S64   GetMediaItem( VO_CHAR * ptr_item, VO_S32 index_stream );
	int      GetTrackNumber();
	VO_U32   SendTrackInfo(int isVideo,int frame_type);
	VO_VOID  UpdateTrackInfoData(VO_U32 nID);
    void     GetTrackInfo(VO_BOOL isvideo,VO_S32 stream_index,VO_S32 represen_index);
//	VO_S32 GetDuration(){ return m_manager.get_duration(); }
	VO_VOID  DownloadTrackInfo(VO_BOOL is_video,VO_CHAR * ptr_url,VO_S32 stream_index,VO_S32 represen_index);
	VO_U32   SetDrmCallBack(VO_PTR pValue);
	static void VO_API ParserProc(VO_PARSER_OUTPUT_BUFFER* pData);
	VO_S64  AddSendDataList(VO_SOURCE_SAMPLE * ptr_buffer , VO_U32 index ,VO_S8 frame_type,VO_BOOL newfile);
	VO_VOID audio_data_arrive( VO_SOURCE_SAMPLE * ptr_buffer, int nFlag = 0);
	VO_VOID video_data_arrive( VO_SOURCE_SAMPLE * ptr_buffer );
	VO_S64  send_status_data(STATUS_BUFFER_ST_DASH *pBuffer);
	VO_S64  SendBufferFromList();
	VO_VOID add_media_data( VO_DASH_FRAME_BUFFER * ptr_buffer , VO_U32 index , VO_BOOL newfile = VO_FALSE );
	VO_VOID set_datacallback( VO_VOID * ptr_obj , pDataCallback datacallback_func){ m_ptr_callbackobj = ptr_obj; m_datacallback_func = datacallback_func; }
	VO_VOID set_statuscallback( VO_VOID * ptr_obj , pStatusCallback statuscallback_func ){ m_ptr_statuscallbackobj = ptr_obj; m_statuscallback_func = statuscallback_func; }
	VO_VOID set_eventcallback( VO_VOID * ptr_obj , pEventCallback_dash eventcallback_func ){ m_ptr_eventcallbackobj = ptr_obj; m_eventcallback_func = eventcallback_func; }
//	static  VO_U32 threadfunc( VO_PTR pParam );
	void	setWorkPath (const char * pWorkPath) {strcpy (m_szWorkPath, pWorkPath); strcat (m_szWorkPath, "/lib/");}
	void    SetTrackInfo(VO_LIVESRC_TRACK_INFOEX * info,int index);
	VO_LIVESRC_SAMPLE   GetParam(int type);
	VO_VOID NotifyData();
    VO_VOID run();
	VO_VOID send_eos();
	VO_VOID load_DASH_parser();
	VO_VOID free_DASH_parser();
	VO_VOID need_flush();
	virtual void thread_function();
	//static VO_U32 threadfunc( VO_PTR pParam );
	VO_U32 GetProgramInfo( VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **pProgramInfo );
	VO_U32 SetParam(VO_U32 nParamID, VO_PTR pParam);
	VO_VOID FulshAudioSample(VO_S64 time);
#ifdef WIN32
	time_t SystemTimeToTime_t(const SYSTEMTIME& st);
#endif

public:
	char				m_szWorkPath[256];
	CDllLoad			m_dlEngine;
	CObjectList<DATA_BUFFER_ST_DASH>	m_List_buffer;
	voCMutex            g_voMemMutex;
	VO_PARSER_API		m_dashhparser_api;
	vo_http_stream m_fragment;
	vo_mpd_manager m_manager;
//	THREAD_HANDLE  m_thread;
	VO_BOOL             m_bNeedFlushBuffer;
    VO_PTR	  m_dashparser_handle;
	VO_U32    m_audioCodecType;
	VO_U32    m_videoCodecType;
	int	     repre_index;
	Representation * cur_pre;	
	pEventCallback_dash      m_eventcallback_func;
	pDataCallback       m_datacallback_func;
	VO_VOID * m_ptr_callbackobj;
	VO_VOID * m_ptr_statuscallbackobj;
	
	pStatusCallback m_statuscallback_func;
	vo_network_judgment m_judgementor;
	VO_PBYTE m_fragment_buffer_codec;
	VO_BOOL m_is_first;
	VO_VOID * m_ptr_eventcallbackobj;
	Representation_Audio * cur_audio_ql;
	Representation * cur_video_ql;
	VO_BOOL  audio_new_chunk;
	VO_SOURCE2_PROGRAM_INFO * m_pPlusProgramInfo;
	VO_S32 m_nStreamCount;
protected:
	 voThreadHandle m_thread;
	  VO_S64    seek_time;
	  VO_S64    start_time;
	  VO_S64    duration;
	  VO_S64   cur_duration;
	  VO_BOOL m_is_flush;
	  VO_BOOL m_is_seek;
	  VO_BOOL m_is_pause;
	  VO_BOOL m_bMediaStop;
	  VO_S64 download_bitrate;
	  VO_S64 m_lastFregmentDuration;
	  VO_BOOL m_recoverfrompause;
	  VO_S64  start_time_tamp;
	  VO_S64  sample_number;
	  VO_S64 m_nSyncTimeStamp;
	  VO_BOOL is_set;
	  int  try_counts;
private:
	  VO_U64 m_nCurTimeStamp_Audio;
	  VO_U64 m_nCurTimeStamp_Video;
	  VO_S64 m_nLastTimeStamp_Audio;
	  VO_S64 m_nLastTimeStamp_Video;
	  VO_LIVESRC_TRACK_INFOEX *m_pTrackInfoEx_Audio;
	  VO_LIVESRC_TRACK_INFOEX *m_pTrackInfoEx_Video;
	  VO_VOID CreateVideoTrackInfoBuffer(VO_DASH_FRAME_BUFFER *pBuffer);
	  VO_VOID CreateAudioTrackInfoBuffer(VO_DASH_FRAME_BUFFER *pBuffer);
	  void ReleaseProgramInfo();
	  VO_BOOL CreateProgramInfo();
	  VO_LOG_PRINT_CB * pVologCB;

#ifdef _USE_NEW_BITRATEADAPTATION
/*add by leon, for bitrate adaptation*/
	int m_nCountsInOneChunk_video;
#endif
private:
	vo_http_stream m_httpUTC;
	VO_VOID GetCurUTCTime(time_t &httpUTC, VO_BOOL enFlesh = VO_FALSE);
	VO_U32 m_nStartUTCMark;
	time_t m_nCurUTCTime;
	VO_BOOL m_bGetUTC2NTP;
};


#endif