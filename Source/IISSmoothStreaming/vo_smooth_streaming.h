
#ifndef __VO_SMOOTH_STREAMING_H__

#define __VO_SMOOTH_STREAMING_H__

#include "vo_manifest_manager.h"
#include "voSmthBase.h"
#include "OMX_Core.h"
#include "voParser.h"
#include "voSmthBase.h"
#include "CDllLoad.h"
#include "voOMX_Index.h"
#include "DRM_API.h"
#include "voLog.h"
#include "vo_network_judgment.h"
#include "vo_thread.h"
#include "CPtrList.h"
#include "voSource2.h"
#ifdef _ISS_SOURCE_
#include "voLiveSource.h"
#include "voLiveSourcePlus.h"
#endif


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#ifdef _IOS
#ifdef _ISS_SOURCE_
namespace _ISS{
#endif
#endif


struct Event
{
	VO_U32 nID; 
	VO_U32 nParam1;
	VO_U32 nParam2;
};

#define ISS_PID_CALLBACK_STATUS 1
#define ISS_PID_CALLBACK_DATA 2
typedef struct  
{
	VO_SMTH_FRAME_BUFFER * ptr_buffer;
	VO_U32 index ;
	VO_BOOL newfile;
}DATA_BUFFER_ST;
typedef struct  
{
	VO_VOID* ptr_buffer;
	VO_U32 id ;
}STATUS_BUFFER_ST;
typedef struct
{
	VO_VOID* buffer;
	VO_U32 type;
}CALLBACK_BUFFER_ST;

#ifdef _ISS_SOURCE_
typedef VO_VOID ( *pDataCallback)( VO_VOID * ptr_obj , VO_LIVESRC_SAMPLE * ptr_buffer );
typedef VO_VOID	( *pStatusCallback)(VO_VOID * ptr_obj, VO_U32 id, VO_VOID * pBuffer);
#else
typedef VO_VOID	( *pStatusCallback)(VO_VOID * ptr_obj, VO_U32 id, VO_VOID * pBuffer);
typedef VO_VOID ( *pDataCallback)( VO_VOID * ptr_obj , OMX_BUFFERHEADERTYPE * ptr_buffer );
#endif
typedef VO_VOID ( *pEventCallback)( VO_VOID * ptr_obj , Event * ptr_buffer );

class vo_smooth_streaming:
	public vo_thread
{
public:
	vo_smooth_streaming(void);
	~vo_smooth_streaming(void);

	VO_BOOL SetDataSource( VO_CHAR * ptr_url , VO_BOOL is_ansyc = VO_FALSE );

	VO_U32 HeaderParser( VO_PBYTE  pBuffer , VO_U32 uSize  );
	VO_U32 ChunkParser (VO_SOURCE_TRACKTYPE trackType, VO_PBYTE pData, VO_U32 uSize);

	VO_U32 GetStreamCounts ( VO_U32 *pStreamCounts);
	VO_U32 GetStreamInfo (VO_U32 nStreamID, VO_SOURCE2_STREAM_INFO **ppStreamInfo);
	VO_U32 GetDRMInfo ( VO_SOURCE2_DRM_INFO **ppDrmInfo);
	VO_U32 GetPlayList(FragmentItem **ppPlayList);
	VO_VOID close();
	VO_VOID set_datacallback( VO_VOID * ptr_obj , pDataCallback datacallback_func ){ m_ptr_callbackobj = ptr_obj; m_datacallback_func = datacallback_func; }
	VO_VOID set_statuscallback( VO_VOID * ptr_obj , pStatusCallback statuscallback_func ){ m_ptr_statuscallbackobj = ptr_obj; m_statuscallback_func = statuscallback_func; }
	VO_VOID set_eventcallback( VO_VOID * ptr_obj , pEventCallback eventcallback_func ){ m_ptr_eventcallbackobj = ptr_obj; m_eventcallback_func = eventcallback_func; }
	VO_VOID set_VideoDelayTime(VO_PTR pValue, VO_U64 timeStamp =0);
	void	setWorkPath (const char * pWorkPath) {strcpy (m_szWorkPath, pWorkPath); strcat (m_szWorkPath, "/lib/");}

	VO_BOOL is_live(){ return m_manager.is_live(); }

	VO_S32 GetDuration(){ return m_manager.get_duration(); }
	
	VO_S32 set_pos( VO_S32 pos );
	void SetLibOp(VO_LIB_OPERATOR* pValue);
	VO_VOID start_after_seek();

	VO_VOID pause(){ m_is_pause = VO_TRUE; VOLOGI("++++++++++++++++++++++++++++++++++++++++++++++m_is_pause: %d",m_is_pause);}
	VO_VOID run();

	VO_U32 SetDrmCallBack(VO_PTR pValue);
	VO_U32 SetLiveSrcCallbackPlus(VO_LIVESRC_CALLBACK *pCallback);
	VO_U32 SetParam(VO_U32 nParamID, VO_PTR pParam);
	virtual void thread_function();

protected:

	static VO_U32 threadfunc( VO_PTR pParam );
	VO_VOID start_smoothstream();
	VO_VOID stop_smoothstream( VO_BOOL isflush = VO_FALSE );

	VO_U32 load_SMTH_parser();
	VO_VOID free_SMTH_parser();

	VO_S64 GetMediaItem( VO_CHAR * ptr_item, VO_S32 index_stream,VO_U32 nChunkDuration );
	//get protection license
	VO_S64 GetProtectionLicense( ProtectionHeader * piff );

	static void VO_API ParserProc(VO_PARSER_OUTPUT_BUFFER* pData);

	VO_VOID audio_data_arrive( VO_SMTH_FRAME_BUFFER * ptr_buffer );
	VO_VOID video_data_arrive( VO_SMTH_FRAME_BUFFER * ptr_buffer );

	VO_S32 GetFrameFormat (VO_BYTE* buffer, VO_U32 nBufSize);
	VO_S32 GetFrameType (VO_BYTE* pInData, VO_U32 nInSize);

	VO_VOID NotifyData();
	VO_VOID UpdateTrackInfoData(VO_U32 nID);
	VO_U32  SendTrackInfo(int isVideo);

	VO_S64 send_status_data(STATUS_BUFFER_ST *pBuffer);
	VO_VOID send_media_data( VO_SMTH_FRAME_BUFFER * ptr_buffer , VO_U32 index , VO_BOOL newfile = VO_FALSE );
	VO_VOID send_eos();

	VO_VOID need_flush();
	VO_VOID sendCodecInfo();


	VO_S64 AddSampleToList(VO_SMTH_FRAME_BUFFER * ptr_buffer , VO_SOURCE2_TRACK_TYPE  index);
	VO_S64 AddSendDataList(VO_SMTH_FRAME_BUFFER * ptr_buffer , VO_U32 index , VO_BOOL newfile= VO_FALSE);
	VO_S64 SendBufferFromList();
	CObjectList<DATA_BUFFER_ST>	m_List_buffer;
	voCMutex g_voMemMutex;


	VO_LIVESRC_CALLBACK	*m_pCallbackPlus;


protected:
	VO_BOOL	m_bHasSyncTimeStamp;
	VO_U64 m_nSyncTimeStamp;
	/*use to flush BufferLish*/
	VO_BOOL m_bNeedFlushBuffer;

	vo_manifest_manager m_manager;
	vo_http_stream m_fragment;

	VO_S64 download_bitrate;
	VO_S64 m_lastFregmentDuration;
	VO_S64 start_time;
	VO_S64 seek_time;

	VO_BOOL m_bMediaStop;
	VO_BOOL m_bRecoverFromPause;
	VO_BOOL m_is_pause;
	VO_BOOL m_is_seek;
	VO_BOOL m_seeked;
	VO_BOOL m_isPushMode;

	VO_U32    m_audioCodecType;
	VO_U32    m_videoCodecType;



	VO_BOOL m_is_H264;
	VO_BOOL m_is_first;

	VO_BOOL m_is_flush;
	voThreadHandle m_thread;

	CDllLoad			m_dlEngine;
	char				m_szWorkPath[256];

	VO_PTR		m_pDrmValue;
	VO_PTR				m_smthparser_handle;
	VO_PARSER_API		m_smthparser_api;


	vo_network_judgment m_judgementor;

	VO_VOID * m_ptr_callbackobj;
	pDataCallback m_datacallback_func;

	VO_VOID * m_ptr_eventcallbackobj;
	pEventCallback m_eventcallback_func;

	VO_VOID * m_ptr_statuscallbackobj;
	pStatusCallback m_statuscallback_func;


private:
	VO_LIVESRC_TRACK_INFOEX *m_pTrackInfoEx_Audio;
	VO_LIVESRC_TRACK_INFOEX *m_pTrackInfoEx_Video;
	VO_VOID CreateVideoTrackInfoBuffer(VO_SMTH_FRAME_BUFFER *pBuffer);
	VO_VOID CreateAudioTrackInfoBuffer(VO_SMTH_FRAME_BUFFER *pBuffer);
	VO_S64  m_nLastTimeStamp_Video;
	VO_S64 m_nLastTimeStamp_Audio;

	VO_U64 m_nCurTimeStamp_Audio;
	VO_U64 m_nCurTimeStamp_Video;

	voCMutex m_voMemMutex_forSeek;

	int m_nCountsInOneChunk_video;


	VO_LOG_PRINT_CB * m_pVologCB;
public:
	int GetTrackNumber();

	VO_U32 GetProgramInfo( VO_U32 nProgram,VO_SOURCE2_PROGRAM_INFO **ppProgramInfo);
#ifndef __USE_TINYXML
	CXMLLoad m_xmlLoad;
#endif

#ifdef _USE_VERIFYCALLBACK_
public:
	void    SetVerifyCallBack(VO_PTR pCallback){m_fragment.SetVerifyCallBack(pCallback);}
	VO_U32    DoVerifyCallBack(VO_PTR pParam);
	VO_U32 m_nResponseSize ;
	VO_CHAR* m_pResponseBuffer ;
	VO_VOID SendTrackUnavailable(VO_LIVESRC_OUTPUT_TYPE type);
#endif

private:
	VOSMTH_DRM_TYPE m_drmCustomerType;
};

#ifdef _IOS
#ifdef _ISS_SOURCE_
}	// end of _ISS namespace
#endif
#endif
#ifdef _VONAMESPACE
}
#endif

#endif
