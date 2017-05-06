#pragma once
#include "vo_stream.h"
#include "voThread.h"
#include "DRM_API.h"
#include "vo_thread.h"
#include "voSource2_IO.h"

#ifdef WIN32

#ifdef WINCE
#include <Winsock2.h>
#pragma comment( lib , "Ws2.lib" )
#endif
#endif

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
//typedef VO_U32 (VO_API * IO_HTTP_VERIFYCALLBACK_FUNC)( VO_PTR hHandle , VO_U32 uID , VO_PTR pUserData );
#define  _USE_VERIFYCALLBACK_


enum VO_HTTP_REQUEST_TYPE
{
	VO_HTTP_GET,
	VO_HTTP_POST,
};
enum DownloadMode
{
	DOWNLOAD2NONE = 0,
	DOWNLOAD2MEM = 1,
	DOWNLOAD2FILE = 2,
	DONWLOADENCRYPT = 32768,
};

class vo_http_stream :
	public vo_stream,vo_thread
{
public:
	vo_http_stream();
	virtual ~vo_http_stream(void);

	virtual VO_BOOL open( VO_CHAR * url , DownloadMode mode );
	virtual VO_BOOL open( VO_CHAR * url , DownloadMode mode , VO_PBYTE ptr_key , VO_PBYTE ptr_iv , ProtectionType drm_type , DRM_Callback * ptr_drm_engine , void * drm_handle );
	virtual VO_VOID close();

	virtual VO_S64 read( VO_PBYTE ptr_buffer , VO_U64 buffer_size );
	virtual VO_S64 write( VO_PBYTE ptr_buffer , VO_U64 buffer_size );
	virtual VO_BOOL seek( VO_S64 pos , vo_stream_pos stream_pos  );
	virtual VO_S64 append( VO_PBYTE ptr_buffer , VO_U64 buffer_size );

	VO_S64 get_content_length(){ return m_content_length; }
	VO_CHAR * get_mime_type(){ return m_mimetype; }

	virtual VO_S64 get_download_bitrate();

	virtual VO_VOID stop(){ stop_download_thread(); }
	VO_BOOL startdownload( VO_PBYTE pChallenge, VO_U32 nChallenge, VO_PCHAR pcUrl, VO_U32 nUrl, VO_PCHAR pcHttpHeader, VO_U32 nHttpHeader, DownloadMode mode,VO_HTTP_REQUEST_TYPE type = VO_HTTP_GET);
	VO_BOOL startdownload( VO_CHAR * url, DownloadMode mode,VO_HTTP_REQUEST_TYPE type = VO_HTTP_GET );

	VO_VOID closedownload( );

	VO_S64 timecost;
	
	virtual void thread_function();
#ifdef _USE_VERIFYCALLBACK_
	VO_BOOL SendRequest(VO_CHAR* request, VO_U32 request_size);
	static VO_U32 DoVerifyCallBack(VO_CHAR* request, VO_U32 request_size,VO_PTR pUserData);
	VO_SOURCE2_IO_HTTP_VERIFYCALLBACK *m_pVerifyCallBack;
	void    SetVerifyCallBack(VO_PTR pCallback){m_pVerifyCallBack = (VO_SOURCE2_IO_HTTP_VERIFYCALLBACK*)pCallback;}
#endif

protected:
	virtual VO_BOOL init( VO_CHAR * url );
	VO_BOOL process( VO_CHAR * url );
	virtual void uninit();

	virtual VO_BOOL resolve_url( VO_CHAR * url );

	VO_U32 downloadthread( VO_PTR pParam );
	virtual void download();

	virtual void download_normal();
	virtual void download_chunked();

	virtual VO_U32 perpare_http_request( VO_CHAR * ptr_request);

	virtual VO_BOOL get_response();

	virtual VO_BOOL analyze_response();

	virtual VO_VOID stop_download_thread();

protected:
	DownloadMode m_download_mode;

	VO_CHAR m_host[256];
	VO_CHAR m_path[1024];
	VO_CHAR m_port[10];

	VO_S32	m_socket;

	voThreadHandle m_downloadthread;

	VO_CHAR m_response[1024];
	VO_CHAR m_reparseurl[1024];

	int m_status_code;
	int m_content_length;
	VO_CHAR m_mimetype[50];
	VO_BOOL m_is_chunked;
	VO_S64 m_totaldownload;

	vo_stream * m_ptr_buffer_stream;

	VO_BOOL m_is_inited;

	VO_BOOL m_is_download_ok;

	VO_BOOL m_stop_download;

	VO_S64 m_download_starttime;
	VO_S64 m_download_endtime;


	VO_HTTP_REQUEST_TYPE m_http_Request_type;
	VO_PBYTE m_pChallenge;
	VO_U32 m_nChallenge;
	VO_PCHAR m_pcUrl;
	VO_U32 m_nUrl;
	VO_PCHAR m_pcHeader;
	VO_U32 m_nHeader;
};


#ifdef _VONAMESPACE
}
#endif

