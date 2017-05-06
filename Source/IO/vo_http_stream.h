#pragma once
#include "voYYDef_SourceIO.h"
#include "voSSL.h"
#include "Buffer/vo_singlelink_list.hpp"
#include "vo_thread.h"
#include "voCMutex.h"
#include "vo_http_struct.h"
#include "vo_http_cookie.h"
#include "vo_http_authentication.h"
#include "voSource2_IO.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#define MAXSOCKETREUSETIMES		30
#define MAXSOCKETKEEPALIVETIME	60000
#define MAXCONNECTRETRY	   2
#define VODEFAULTTIMEOUT	10000

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

class vo_http_stream :
	public vo_thread
{
public:
	vo_http_stream();
	virtual ~vo_http_stream(void);

	virtual VO_VOID initial( VO_CHAR *ptr_url );

    virtual VO_BOOL open( VO_BOOL bIsAsyncOpen = VO_FALSE );
	virtual VO_VOID close();

	virtual VO_VOID stop(){ stop_download_thread(); }

	virtual VO_VOID setsslapi( vosslapi ssl);

	VO_VOID set_maxspeed( VO_U32 speed);
	VO_VOID set_range( VO_U64 offset , VO_U64 size ){ m_range_offset = offset ; m_range_length = size; }
	VO_VOID set_cookies(char * cookies );
	VO_VOID set_download2buff_callback( VO_PTR pParam );
	VO_VOID set_IO_Callback( VO_PTR pParam );
	//username:password
	VO_VOID set_userpass(VO_CHAR * userpass  , VO_U32 uplen );

	VO_U32 GetLastError();
	VO_S32 get_download_speed();		//KBps
	VO_U32 get_download_time();			//ms
	Persist_HTTP * get_persisthttp(){ return m_ptr_persist; }
	VO_S64 get_content_length(){ return m_content_length; }
	VO_CHAR * get_mime_type(){ return m_mimetype; }
	VO_CHAR * get_url(){ return m_url; }
	VO_CHAR * get_host(){ return m_host; }
	VO_CHAR * get_redirecturl();
	VO_CHAR * getdnscacheip();
	cookie_storage_node* get_cooki_node();
	VO_CHAR * get_authinfo();
	VO_VOID	  set_authinfo( VO_CHAR* str_auth );
	VO_VOID	setProxyHost( VO_CHAR* strProxyHost );
	VO_VOID	setProxyPort( VO_CHAR* strProxyPort );
	VO_VOID	setUserAgent( VO_CHAR* strUserAgent );
	VO_VOID	setCachedDnsIP( VO_CHAR* strDNSCacheIP );
	VO_VOID setHandle( Persist_HTTP * hanle );


	VO_BOOL Is_cache_redirect_url(){ return m_is_cache_redirecturl; }
	VO_BOOL Is_cookie_update();
	VO_BOOL IsDownloadFinished();
	VO_BOOL IsDownloadComplete();
	VO_BOOL Is_auth();
	
	VO_S64 append( VO_PBYTE ptr_buffer , VO_U64 buffer_size );
	VO_S32 Notify( VO_PBYTE param , VO_U64 size , IO_HTTPDATA2BUFFER_DATA_TYPE type );
	VO_VOID print_downloadInfo();
	VO_VOID control_speed( VO_S32 speed );
	VO_S32  IONotify( VO_U32 uID , VO_PTR pParam1, VO_PTR pParam2 );

protected:
	virtual VO_BOOL run_agent( VO_CHAR * url );
	virtual VO_VOID stop_download_thread();

	virtual void thread_function();
	virtual void download();

	virtual void download_normal();
	virtual void download_chunked();
	virtual void download_endless();

	virtual VO_BOOL send_request();
	virtual VO_BOOL get_response();

	virtual VO_VOID Initdownloadinfo();
	virtual VO_BOOL resolve_url( VO_CHAR * url );
	VO_BOOL resolve_urlHTTP( VO_CHAR * url );
	VO_BOOL resolve_urlHTTPS( VO_CHAR * url );

	virtual VO_BOOL urlencode();
	inline unsigned char toHex(const unsigned char x);
	VO_BOOL isxnumalpha( char x );
	VO_BOOL isxhex( char x );

	virtual VO_BOOL persist_connect();
	virtual VO_BOOL IsPersistSockAvailable();
	VO_BOOL IsPersistSockAvailableHTTP();
	VO_BOOL IsPersistSockAvailableHTTPS();
	virtual VO_BOOL IsDefaultPort();
	VO_BOOL IsDefaultPortHTTP();
	VO_BOOL IsDefaultPortHTTPS();
	virtual VO_U32 generate_tunnel_request( VO_CHAR * ptr_request );
	VO_U32 generate_tunnel_requestHTTP( VO_CHAR * ptr_request );
	VO_U32 generate_tunnel_requestHTTPS( VO_CHAR * ptr_request );
	VO_BOOL analyze_response();
	VO_U32  generate_request( VO_CHAR * ptr_request );

	VO_S32 StatusCode();
	VO_BOOL IsChunked();
	VO_BOOL ContentLength();
	VO_VOID ContentType();
	VO_VOID Persist_keepalive();

	VO_VOID analyze_url_redirection();
	VO_VOID process_redirect_url();
	VO_BOOL status_transfer();

	VO_VOID check_download_speed();

	VO_BOOL resolve_host( VO_CHAR * host );

	VO_BOOL open_internal();
private:
	//process 5xx
	VO_VOID	Process_server_error(); 
	//process 4xx
	VO_BOOL Process_client_error();
	//process 3xx
	VO_BOOL Process_Redirection();
	//process 2xx
	VO_VOID Process_VCase();

	//process 401 and 407
	VO_BOOL Process_Authentication();
protected:
	VO_CHAR m_url[2048];
	VO_CHAR m_host[256];
	VO_CHAR m_path[1024];
	VO_CHAR m_port[10];
	VO_CHAR m_response[8192];
	VO_CHAR m_reparseurl[2048];
	VO_CHAR m_cookies[2048];
	VO_CHAR m_mimetype[256];

	VO_S32 m_status_code;
	VO_S64 m_content_length;
	VO_S32 m_protocol_ver;
	VO_U64 m_range_offset;
	VO_U64 m_range_length;
	VO_U32 m_max_download_speed;
	VO_U32 m_min_download_speed;
	VO_BOOL m_bslow_down;

    Persist_HTTP * m_ptr_persist;

	vo_http_cookie m_http_cookie;

	vo_http_download_info  m_http_downloadinfo;

	vo_http_authentication	m_authorization;

	//we allow change the data output position dynamic 
	voCMutex m_lock_w2buff;
	VO_HTTP_DOWNLOAD2BUFFERCALLBACK  m_download2bufferCB;
	VO_SOURCE2_IO_HTTPCALLBACK		 m_httpiocb;
	VO_BOOL	m_is_ignore_cookie_setting;
	VO_BOOL m_is_cache_redirecturl;
		
	VO_CHAR m_authinfo[1024];
	VO_BOOL m_bauth;
	VO_CHAR m_username[100];
	VO_CHAR m_password[100];
	VO_BOOL m_is_url_encoded;
	VO_S32	m_nspeed;
	VO_BOOL m_bclose;
	VO_BOOL m_reuseoldsocket;
	VO_BOOL m_downloadcontent_wanted;
	VO_BOOL m_bAsyncMode;
	VO_BOOL m_bProxySet;
	VO_BOOL m_bUserAgentSet;
	VO_BOOL m_bDnsIpCached;
	VO_CHAR m_proxyhost[512];
	VO_CHAR m_proxyport[64];
	VO_CHAR m_useragent[512];
	VO_CHAR m_dnscachedip[65];
	vosslapi m_ssl;
	VO_BOOL	m_bsslinit;
	VO_BOOL m_bhttps;
};

#ifdef _VONAMESPACE
}
#endif