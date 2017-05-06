#pragma once
#include "interface_download_callback.h"
#include "interface_buffer_callback.h"
#include "voSource.h"
#include "voStreaming.h"
#include "voSource2.h"
#include "voSource2_IO.h"
#include "vo_download_manager.h"
#include "vo_buffer_manager.h"
#include "CDllLoad.h"
#include "vo_buffer_stream.h"
#include "voCMutex.h"
#include "vo_thread.h"
#include "voCSemaphore.h"
#include "vo_playlist_parser.h"
#include "voDLNA_Param.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


enum StreamType
{
	StreamType_Normal,
	StreamType_NoneStop,
};

/**
 * Enumeration of seek modes.
 */
typedef enum TIME_SEEKMODETYPE {
    TIME_SeekModeFast = 0, /**< Prefer seeking to an approximation
                                * of the requested seek position over
                                * the actual seek position if it
                                * results in a faster seek. */
    TIME_SeekModeAccurate, /**< Prefer seeking to the actual seek
                                * position over an approximation
                                * of the requested seek position even
                                * if it results in a slower seek. */
    TIME_SeekModeKhronosExtensions = 0x6F000000, /**< Reserved region for introducing Khronos Standard Extensions */
    TIME_SeekModeVendorStartUnused = 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
    TIME_SeekModeMax = 0x7FFFFFFF
} TIME_SEEKMODETYPE;


class vo_PD_manager :
	public interface_download_callback,
	public interface_buffer_callback,
	public vo_thread
{
public:
	vo_PD_manager(void);
	virtual ~vo_PD_manager(void);

	//download call back
	virtual CALLBACK_RET received_data( VO_S64 physical_pos , VO_PBYTE ptr_buffer , VO_S32 size );
	virtual CALLBACK_RET download_notify( DOWNLOAD_CALLBACK_NOTIFY_ID id , VO_PTR ptr_data );
	//

	//buffer call back
	virtual BUFFER_CALLBACK_RET buffer_notify( BUFFER_CALLBACK_NOTIFY_ID notify_id , VO_PTR ptr_data );
	//

	VO_BOOL open( VO_SOURCE_OPENPARAM * pParam );
	VO_U32 close();
	VO_U32 get_sourceinfo(VO_SOURCE_INFO * pSourceInfo);
	VO_U32 get_trackinfo(VO_U32 nTrack, VO_SOURCE_TRACKINFO * pTrackInfo);
	VO_U32 get_sample(VO_U32 nTrack, VO_SOURCE_SAMPLE * pSample);
	VO_U32 set_pos(VO_U32 nTrack, VO_S64 * pPos);
	VO_VOID moveto( VO_S64 pos );
	VO_U32 set_sourceparam(VO_U32 uID, VO_PTR pParam);
	VO_U32 get_sourceparam(VO_U32 uID, VO_PTR pParam);
	VO_U32 set_trackparam(VO_U32 nTrack, VO_U32 uID, VO_PTR pParam);
	VO_U32 get_trackparam(VO_U32 nTrack, VO_U32 uID, VO_PTR pParam);
	VO_BOOL perpare_parser();

	VO_VOID set_firstseektrack( VO_S32 track ){ m_firstseektrack = track; }
	VO_S32 get_firstseektrack(){ return m_firstseektrack; }

	vo_buffer_stream * get_stream(){ return m_buffer_stream; }

	VO_VOID set_source2flag(){ m_is_source2 = VO_TRUE; }
	VO_VOID setWorkpath( VO_TCHAR * ptrPath ){ 	m_pstrWorkPath = ptrPath; }

protected:
	VO_BOOL delay_open();
	VO_VOID arrange_headdata( headerdata_info * ptr_info );

	BUFFER_CALLBACK_RET buffer_notify_bufferlack( VO_BUFFER_NEED * ptr_need );
	BUFFER_CALLBACK_RET dlna_buffer_notify_bufferlack( VO_BUFFER_NEED * ptr_need );
	BUFFER_CALLBACK_RET normal_buffer_notify_bufferlack( VO_BUFFER_NEED * ptr_need );

	VO_VOID process_STREAMING_QUICKRESPONSE_COMMAND( VO_QUICKRESPONSE_COMMAND command );

	VO_VOID notify( long EventCode, long * EventParam );

	VO_VOID http_verify_callback( int id, void* pParam);

	VO_VOID http_event_callback( int id, VO_U32 pParam1 , VO_U32 pParam2);

	VO_U32 get_sample_internal(VO_U32 nTrack, VO_SOURCE_SAMPLE * pSample);

	virtual void thread_function();

	void stop_getsample_thread();
	void stop_seek_thread();
	void stop_startbuffering_thread();

	static unsigned int buffering_func(void * ptr_obj);
	VO_VOID dobuffering_internal();

	void do_startbuffering();

	static unsigned int seekto( void * ptr_obj );
	VO_VOID moveto_internal();
	
	// added by Lin Jun to support static library
	VO_BOOL load_static_parser_lib(int nStreamType, void** pParserApi);
	// end

private:
	VO_BOOL ProcessShoutCast();

	//when network adapter switch between wifi and 3G, we should retry download data.
	VO_VOID ProcessNetworkError();

	//download partial data and load related parser.
	VO_BOOL LoadParser();

private:
	VO_SOURCE_OPENPARAM m_source_param;
	VOPDInitParam m_PD_param;
	VO_CHAR m_url[4096];
	VO_CHAR m_dwango_cookies[1024];

	vo_download_manager m_downloader;
	vo_buffer_manager m_buffer;

	VO_SOURCE_READAPI m_parser_api;
	CDllLoad m_dllloader;

	vo_buffer_stream * m_buffer_stream;

	VO_FILE_OPERATOR m_opFile;
	VO_PTR m_fileparser;

	VO_BOOL m_is_init;
	VO_BOOL m_is_to_exit;

	VO_S32 m_bitrate;
	VO_U32 m_duration;

	VO_S32 m_firstseektrack;

	VO_BOOL m_is_pause;

	StreamingNotifyEventFunc m_notifier;
	VO_SOURCE2_EVENTCALLBACK m_notifier2;

	VO_HTTP_VERIFYCALLBACK m_http_verifycb;
	VO_SOURCE_EVENTCALLBACK m_http_eventcb;

	StreamType m_streamtype;

	VO_BOOL m_is_networkerror;

	VO_S32 m_tracks;

	voCSemaphore m_sem_seekcount;

	VO_SOURCE_SAMPLE * m_ptr_sample;
	VO_SOURCE_SAMPLE * m_ptr_bottom_sample;
	VO_PBYTE * m_ptr_buffer;
	VO_PBYTE * m_ptr_bottom_buffer;
	VO_U32	* m_getsample_ret;
	VO_U32  * m_bsourcend;
	VO_S64 * m_ptr_timestamp;
	VO_U32 * m_max_sampsize;
	VO_U32 * m_getsample_flag;
	VO_U32 * m_bdrop_frame;

    voCSemaphore m_sem_emptysample;

	VO_BOOL m_b_stop_getsample;

	voCMutex * m_getsamplelock;
	voCSemaphore * m_sem_getsample;

	THREAD_HANDLE m_seek_threadhandle;
	THREAD_HANDLE m_startbuffering_handle;
	VO_S64 m_seekpos;

	VO_S32 m_videotrack;
	VO_BOOL m_is_seeking;

	voCMutex m_seekpos_lock;
	VO_BOOL m_is_stopseek;

	//store those data to detect the file format and range request feature
	VO_PBYTE m_ptrpool;
	VO_S32 m_poolsize;

	//the seek mode current is using
	TIME_SEEKMODETYPE m_seekmode;

	VO_S32 m_cur_downl_percent; 

	VO_BOOL m_is_buffering_on_start;

	VO_BOOL m_is_shoutcast;

	vo_playlist_parser*	m_ptr_playlist_parser;

	VO_BOOL m_is_stopbuffering_thread;

	VO_S32	m_buffering_percent;

	VO_BOOL	m_is_buffering;

	VO_S64  m_filesize_byserver;

	VO_BOOL m_is_source2;

	VO_S64	m_retry_download_pos;

	voDLNA_Param  m_dlna_protocolinfo;

	VO_TCHAR *	m_pstrWorkPath;

	VO_S32  m_connectretry_times;

	VO_U32  m_last_download_retry_time;
};

#ifdef _VONAMESPACE
}
#endif
