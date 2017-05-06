#pragma once
#include "vo_downloader.h"
#include "vo_thread.h"
#include "voCMutex.h"
#include "voCSemaphore.h"
#include "vo_http_authentication.h"
#include "voDLNA_Param.h"
#include "COpenSSL.h"
#include "vo_http_cookie.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define MAXGETSIZE (1024*2)
typedef struct 
{
	VO_U32	metacnt;
	VO_U32  metalen;
	VO_CHAR buffer2[MAXGETSIZE * 4];
	VO_CHAR tmpbuff[MAXGETSIZE * 4];
	VO_U32 left_pos;
	VO_U32 copycnt;
}ShoutCastMetadataExtraction;

class Persist_HTTP
{
public:
    Persist_HTTP();
    ~Persist_HTTP();

	VO_BOOL lock();
	VO_VOID unlock();
	VO_BOOL Init();
	VO_VOID	UnInit();

public:
	virtual VO_S32 Open( VO_CHAR * str_host , VO_CHAR * str_port );
	virtual VO_VOID Close( VO_BOOL bCloseSocket = VO_TRUE );
	virtual VO_S32 Read( VO_PBYTE buffer , VO_S32 size );
	virtual VO_S32 Write( VO_PBYTE buffer , VO_S32 size );

    VO_S32 socket;
    VO_U32 deadtime;
    VO_S32 usedtimes;
    VO_BOOL brefreshbyserver;
    VO_S32 maxusetime;
	VO_CHAR tunnelrequest[2048];
	VO_CHAR addr_buf[65];

	voCMutex uselock;
	//used for avoid call reset/read/write/close in the same time, to make sure socket is available when use it.
	voCMutex m_lock;
	VO_BOOL isusing;
	VO_S32 ownerid;

	//flag for whether to exit socket related session
	VO_BOOL m_bexit;
};

enum VO_SSL_METHOD
{
	VO_SSL_SSLv23,
	VO_SSL_SSLv3,
	VO_SSL_TLSv1,
	VO_SSL_UNKNOWN,
};

class Persist_HTTPS
	:public Persist_HTTP
{
public:
    Persist_HTTPS();
    ~Persist_HTTPS();

	virtual VO_S32 Open( VO_CHAR * str_host , VO_CHAR * str_port );
	virtual VO_VOID Close( VO_BOOL bCloseSocket = VO_TRUE );
	virtual VO_S32 Read( VO_PBYTE buffer , VO_S32 size );
	virtual VO_S32 Write( VO_PBYTE buffer , VO_S32 size );

	VO_VOID setsslapi(vosslapi ssl);
	VO_BOOL SSL_Init();
	VO_BOOL SetupSSLTunnel2Proxy();

	VO_PTR m_ptr_ssl;
	VO_PTR m_ptr_ctx;
	VO_PTR m_ptr_method;
	VO_SSL_METHOD m_ssl_method;
	VO_BOOL	m_bsslinit;
	vosslapi m_ssl;

private:
	VO_S32 OpenInternalLoop( VO_CHAR * str_host , VO_CHAR * str_port );
	VO_VOID CloseInternalLoop();
};

class vo_http_downloader :
	public vo_downloader,
	public vo_thread
{
public:
	vo_http_downloader(void);
	virtual ~vo_http_downloader(void);

	//vo_downloader
	virtual VO_BOOL set_url( VO_CHAR * str_url , VOPDInitParam * ptr_PD_param );
	virtual VO_BOOL start_download( VO_S64 download_pos = 0 , VO_S64 download_size = DOWNLOADTOEND , VO_BOOL is_async = VO_TRUE );
	virtual VO_VOID stop_download();
	virtual VO_BOOL is_downloading();
	virtual VO_VOID get_dowloadinfo( VO_DOWNLOAD_INFO * ptr_info );
	virtual VO_BOOL is_support_seek(){ return m_is_server_canseek; }
	virtual VO_BOOL start_download_inthread( VO_S64 download_pos = 0 , VO_S64 download_size = DOWNLOADTOEND , VO_BOOL is_async = VO_TRUE );
	//

	//vo_thread
	virtual VO_VOID thread_function();
	virtual VO_VOID stop_thread();
	//

	virtual VO_S32 get_net_error_type(){ return m_error_type;}
	virtual VO_VOID set_net_error_type(VO_S32 err_type)
	{
		m_error_type = err_type;

		if( m_error_type == E_PD_DOWNLOAD_FAILED )
		{
			m_ptr_callback->download_notify( DOWNLOAD_ERROR , (VO_PTR)&m_current_physical_pos );
		}
		else
		{
			m_ptr_callback->download_notify( DOWNLOAD_ERROR , 0 );
		}
	}

	virtual VO_U32 get_duration(){ return m_duration; }

	//set data pointer and len for check whether the server can seek or not
	virtual VO_VOID set_comparedata(VO_PBYTE pdata , VO_S32 size);

	virtual VO_VOID slow_down(VO_U32 slowdown);

	//try to pause / run download thread to reveive data
	virtual VO_VOID pause_connection(VO_BOOL recvpause);

	virtual VO_VOID set_dlna_param(VO_VOID* pobj);

	virtual VO_VOID set_connectretry_times(VO_S32 retrytimes);

	virtual VO_VOID set_workpath(VO_TCHAR *	strWorkPath);

	VO_VOID setsslapi( vosslapi ssl);
protected:
	virtual VO_BOOL resolve_url( VO_CHAR * url );
	VO_BOOL resolve_urlHTTP( VO_CHAR * url );
	VO_BOOL resolve_urlHTTPS( VO_CHAR * url );

	virtual VO_S32 perpare_request( VO_CHAR * ptr_request , VO_S64 download_pos , VO_S64 download_size );
	virtual VO_BOOL get_response();
	virtual VO_BOOL analyze_response();

	virtual VO_S32 normal_perpare_request( VO_CHAR * ptr_request , VO_S64 download_pos , VO_S64 download_size );
	virtual VO_BOOL normal_analyze_response();

	//for DLNA case ,we should do some special things on receive response and prepare request
	virtual VO_S32 dlna_perpare_request( VO_CHAR * ptr_request , VO_S64 download_pos , VO_S64 download_size );
	virtual VO_BOOL dlna_analyze_response();

	virtual VO_VOID download_normal();
	virtual VO_VOID download_chunked();
	virtual VO_VOID download_endless();

	virtual VO_VOID normal_download_normal();
	virtual VO_VOID normal_download_chunked();
	virtual VO_VOID normal_download_endless();

	virtual VO_VOID dlna_download_normal();
	virtual VO_VOID dlna_download_chunked();
	virtual VO_VOID dlna_download_endless();

	virtual VO_VOID analyze_proxy();
	virtual VO_VOID urlencode();
	inline unsigned char toHex(const unsigned char x);

	//for test
	//virtual VO_BOOL vo_socket_connect2( VO_S32 * ptr_socket , VO_CHAR * str_host , VO_CHAR * str_port );
	//

	static unsigned int downloadfunc( void * ptr_obj );
	VO_VOID downloadfunc_internal();

	void stop_download_thread();

	VO_VOID set_to_close(VO_BOOL is_to_exit);

	VO_BOOL resolve_host( VO_CHAR * host );

	VO_BOOL Is_digit(VO_CHAR * ptr_data );
	
	virtual VO_U32 generate_tunnel_request( VO_CHAR * ptr_request );
	VO_U32 generate_tunnel_requestHTTP( VO_CHAR * ptr_request );
	VO_U32 generate_tunnel_requestHTTPS( VO_CHAR * ptr_request );
private:
	VO_S32 StatusCode();
	VO_BOOL IsChunked();
	VO_BOOL IsValidContentLength();
	VO_VOID ContentRange();
	VO_VOID Relocation();
	VO_VOID Duration();
	VO_VOID ContentType();
	VO_VOID AcceptRange();
	VO_VOID MetadataInt();
	VO_VOID AvailableSeekRange();

	VO_BOOL ProcessShoutcastMetadata( VO_CHAR* buffer,VO_S32& ret );
	VO_BOOL CheckSeekFeature(VO_CHAR * buffer, VO_S32 ret);
	VO_S32 RemoveProperty(VO_CHAR* ptr_request ,VO_CHAR* properties  , VO_S32 size);
	VO_BOOL Is_url_https( const VO_CHAR * ptr_url );
	VO_BOOL vostr_is_digit(VO_CHAR * ptr_data );
	VO_VOID generate_cookie_str();
protected:

	VOPDInitParam * m_ptr_PD_param;

	VO_CHAR m_host[1024];
	VO_CHAR m_path[2048];
	VO_CHAR m_port[10];
	VO_CHAR m_url[4096];

	VO_BOOL m_use_proxy;
	VO_CHAR m_proxy_host[1024];
	VO_CHAR m_proxy_port[10];

	VO_CHAR m_reparseurl[4096];
	VO_CHAR m_cookies[4096];

	VO_S32 m_status_code;
	VO_S64 m_content_length;
	VO_CHAR m_mimetype[50];
	VO_BOOL m_is_chunked;

	//for stagefright compatible
	VO_BOOL m_ContentLengthValid;

	VO_BOOL m_is_server_canseek;
	VO_BOOL m_is_url_encoded;

	VO_CHAR m_response[4096];

	VO_BOOL m_stop_download;
	VO_BOOL m_is_downloading;

	VO_BOOL m_is_update_buffer;

	VO_S64 m_current_physical_pos;
	VO_S64 m_downloaded;
	VO_S64 m_start_download_pos;

	VO_U32 m_download_start_time;

	VO_U32 m_duration;

	VO_S32	m_error_type;

	//timeout response value
	VO_S64 m_timeout;
	
	//identify protocol version
	VO_S32 m_protocol_ver;

	//chech flag for range request support
	VO_BOOL m_check_seek_feature;

	//the data pointer and len for check range request support or not
	VO_PBYTE m_pCompdata;
	VO_S32	 m_nComplen;

	voCMutex m_download_lock;

	VO_S64	m_download_pos_thread;

	VO_BOOL m_is_thread_downloading;

	VO_S64	m_buffering_end_pos;

	VO_U32	m_download_used_time;

	VO_U32  m_metadata_interval;

	VO_BOOL m_is_to_exit;

	VO_U32 m_slow_down;

	voDLNA_Param*	m_dlna_param;

	VO_S64	m_limit_range_start;

	VO_S64	m_limit_range_end;

	VO_BOOL	m_is_pause_connection;

	vo_http_authentication	m_authorization;

	VO_CHAR m_authinfo[1024];
	VO_BOOL m_bauth;
	VO_CHAR m_username[100];
	VO_CHAR m_password[100];
	VO_CHAR m_metadata[4096];

	ShoutCastMetadataExtraction	m_shoutcastmeta;

	VO_S32  m_connectretry_times;
	Persist_HTTP * m_ptr_persist;
	VO_BOOL m_bhttps;
	VO_BOOL	m_is_ignore_cookie_setting;
	VO_TCHAR *	m_pstrWorkPath;
	COpenSSL m_sslload;
	vo_http_cookie m_http_cookie;
	VO_S32 m_nLoopCnt;
public:
	THREAD_HANDLE m_download_threadhandle;
	VO_BOOL m_download_thread_processing;
};

#ifdef _VONAMESPACE
}
#endif
