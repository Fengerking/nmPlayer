#ifdef WINCE
#include <winsock2.h>
#endif
#include "vo_http_downloader.h"
#include "../../../Common/NetWork/vo_socket.h"
#include "voString.h"
#include "voOSFunc.h"
#include "voThread.h"
#include "vo_thread.h"
#include "voLog.h"
#include "voLiveSource.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define MAXCONNECTRETRY	   2
#define MAXSOCKETREUSETIMES		30
#define MAXSOCKETKEEPALIVETIME	60000
#define VODEFAULTTIMEOUT	10000

#define HTTP_STATUS_OK                    200

#define SSL_ERROR_NONE			0
#define SSL_ERROR_SSL			1
#define SSL_ERROR_WANT_READ		2
#define SSL_ERROR_WANT_WRITE		3
#define SSL_ERROR_WANT_X509_LOOKUP	4
#define SSL_ERROR_SYSCALL		5 /* look at error stack/return value/errno */
#define SSL_ERROR_ZERO_RETURN		6
#define SSL_ERROR_WANT_CONNECT		7
#define SSL_ERROR_WANT_ACCEPT		8

#define VOSOCKETRECVTIMEOUTSSL		5
#define VOSOCKETSENDTIMEOUTSSL		5
#define VODOWNLOADRECURSIVECNT		2

static char * strncchr( char * ptr_src , char chr )
{
	char  * ptr = NULL;
	if( chr >= 'a' && chr <= 'z' )
	{
		char * ptr_low = NULL;
		char * ptr_up = NULL;

		ptr_low = strchr( ptr_src , chr );
		ptr_up = strchr( ptr_src , chr + 'A' - 'a' );

		ptr = ptr_low < ptr_up ? ptr_low : ptr_up;

		if( ptr == NULL )
			ptr = (unsigned int)ptr_low + ptr_up;
	}
	else if( chr >= 'A' && chr <= 'Z' )
	{
		char * ptr_low = NULL;
		char * ptr_up = NULL;

		ptr_up = strchr( ptr_src , chr );
		ptr_low = strchr( ptr_src , chr - 'A' + 'a' );

		ptr = ptr_low < ptr_up ? ptr_low : ptr_up;

		if( ptr == NULL )
			ptr = (unsigned int)ptr_low + ptr_up;
	}
	else
	{
		ptr = strchr( ptr_src , chr );
	}

	return ptr;
}

static char * strncstr( char * ptr_src , char * ptr_str )
{
	char * ptr = strncchr( ptr_src , ptr_str[0] );


	while( ptr )
	{
		if(
#ifdef WINCE 
			_strnicmp( ptr , ptr_str , strlen( ptr_str ) ) == 0
#elif WIN32
			strnicmp( ptr , ptr_str , strlen( ptr_str ) ) == 0
#else
			strncasecmp( ptr , ptr_str , strlen( ptr_str ) ) == 0
#endif
			)
			return ptr;
		else
			ptr = strncchr( ptr + 1 , ptr_str[0] );
	}

	return NULL;
}

Persist_HTTP::Persist_HTTP()
:socket(-1)
,usedtimes(0)
,deadtime(0)
,maxusetime(MAXSOCKETREUSETIMES)
,isusing(VO_FALSE)
,ownerid(0)
,m_bexit(VO_FALSE)
{
	memset( tunnelrequest , 0 , sizeof(tunnelrequest) );
	memset( addr_buf , 0 , sizeof(addr_buf) );
	Init();
}

Persist_HTTP::~Persist_HTTP()
{
	Close();
	UnInit();
}


VO_BOOL Persist_HTTP::lock()
{
	voCAutoLock lock(&uselock);

	if( isusing )
	{
		return VO_FALSE;
	}

	isusing = VO_TRUE;
	ownerid = get_current_threadid();
	m_bexit = VO_FALSE;
	memset( tunnelrequest , 0 , sizeof(tunnelrequest) );
	memset( addr_buf , 0 , sizeof(addr_buf) );

	return VO_TRUE;
}

VO_VOID Persist_HTTP::unlock()
{
	voCAutoLock lock(&uselock);

	isusing = VO_FALSE;
	ownerid = 0;
	m_bexit = VO_TRUE;
	memset( tunnelrequest , 0 , sizeof(tunnelrequest) );
	memset( addr_buf , 0 , sizeof(addr_buf) );
}

VO_BOOL Persist_HTTP::Init()
{
	return vo_socket_init();
}

VO_VOID	Persist_HTTP::UnInit()
{
	return vo_socket_uninit();
}

// 0, user notify exit; 1, success; -1 , connection fail.
VO_S32 Persist_HTTP::Open( VO_CHAR * str_host , VO_CHAR * str_port )
{
	voCAutoLock lock(&m_lock);
	m_bexit = VO_FALSE;
	VO_S32 trycount = 0;

#ifdef ENABLE_ASYNCDNS
	VO_BOOL bDNSFailed = VO_FALSE;
	while( !vo_socket_connect_asyncdns( &socket , str_host , str_port , &m_bexit , &bDNSFailed , addr_buf ) && !bDNSFailed )
#else
	while( !vo_socket_connect( &socket , str_host , str_port , &m_bexit ) )
#endif
	{
		trycount++;

		if( m_bexit )
		{
			VOLOGE( "user require to exit!" );
			return 0;
		}

		if( trycount >= MAXCONNECTRETRY )
		{
			VOLOGE( "Socket Connect Fail!" );
			return -1;
		}
	}

	if( !vo_socket_setTimeout( &socket ) )
	{
		VOLOGE( "set timeout failed" );
		return -1;
	}

	return 1;
}

VO_VOID Persist_HTTP::Close( VO_BOOL bCloseSocket )
{
	m_bexit = VO_TRUE;
	if( !bCloseSocket )
	{
		return;
	}

	voCAutoLock lock(&m_lock);
	if( socket > 0 )
	{
		vo_socket_close( socket );
	}
	socket = -1;
	usedtimes = 0;
	deadtime = 0;
	brefreshbyserver = VO_FALSE;
	maxusetime = MAXSOCKETREUSETIMES;
}

VO_S32 Persist_HTTP::Read( VO_PBYTE buffer , VO_S32 size )
{
	voCAutoLock lock(&m_lock);
	if( socket < 0 )
	{
		VOLOGE( "Socket not Init yet!" );
		return -1;
	}

	fd_set fdr;
	fd_set fde;
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 20000;

	VO_U32 read_data_start = voOS_GetSysTime();
	while( !m_bexit )
	{
		FD_ZERO(&fdr);
		FD_SET(socket,&fdr);
		FD_ZERO(&fde);
		FD_SET(socket,&fde);

		VO_S32 select_ret = select( socket + 1 ,&fdr,0,&fde,&timeout);
		if( -1 == select_ret )
		{
			//socket link error
			return -1;
		}

		if(FD_ISSET(socket , &fde))
		{
			VOLOGE( "socket exception happen when recv" );
			return -1;
		}
		
		if( 0 == select_ret )
		{
			VO_U32 read_data_cost = voOS_GetSysTime() - read_data_start;
			if( read_data_cost > VODEFAULTTIMEOUT )
			{
				VOLOGE( "read data timeout" );
				return -1;
			}
			else
			{
				voOS_Sleep(2);
				continue;
			}
		}

		VO_S32 ret = vo_socket_recv( socket , (VO_PBYTE)buffer , size );
		if( ret == 0 )
		{
			VOLOGI( "connection has been gracefully closed by server" );
			return -1;
		}
		else
		{
			return ret;
		}
	}

	return 0;
}

VO_S32 Persist_HTTP::Write( VO_PBYTE buffer , VO_S32 size )
{
	voCAutoLock lock(&m_lock);
	if( socket < 0 )
	{
		VOLOGE( "Socket not Init yet!" );
		return -1;
	}

	fd_set fdw;
	fd_set fde;
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 20000;
		
	VO_S32 total_send = 0;

	VO_S32 ret = 0;

	VO_U32 send_request_start = voOS_GetSysTime();
	do 
	{
		VO_U32 send_request_cost = voOS_GetSysTime() - send_request_start;
		if( send_request_cost > VODEFAULTTIMEOUT )
		{
			VOLOGE( "send request timeout!" );
			return -1;
		}

		FD_ZERO(&fdw);
		FD_SET(socket,&fdw);		
		FD_ZERO(&fde);
		FD_SET(socket,&fde);

		//check whether socket set for write is ready or not 
		VO_S32 select_ret = select( socket + 1 ,NULL, &fdw,&fde,&timeout);

		if( select_ret == -1 )
		{
			VOLOGE( "socket error happen when send" );
			return -1;
		}

		if(FD_ISSET(socket , &fde))
		{
			VOLOGE( "socket exception happen when send" );
			return -1;
		}

		if( 0 == select_ret )
		{
			voOS_Sleep(2);
			continue;
		}

		ret = vo_socket_send( socket , buffer + total_send , size - total_send );

		if( ret == -1 )
		{
			VOLOGE( "socket error happen when send" );
			return -1;
		}

		total_send = total_send + ret;

	} while ( total_send < size && !m_bexit );

	return total_send;
}

Persist_HTTPS::Persist_HTTPS()
:m_ptr_ssl(0)
,m_ptr_ctx(0)
,m_ptr_method(0)
,m_ssl_method(VO_SSL_SSLv23)
,m_bsslinit(VO_FALSE)
{
	Init();
	memset( &m_ssl , 0 , sizeof( m_ssl ) );
}

Persist_HTTPS::~Persist_HTTPS()
{
	Close();
	UnInit();
}

VO_VOID Persist_HTTPS::setsslapi( vosslapi ssl)
{ 
	m_ssl = ssl; 
	m_bsslinit = VO_TRUE;
}

VO_VOID Persist_HTTPS::Close( VO_BOOL bCloseSocket )
{
	m_bexit = VO_TRUE;
	if( !bCloseSocket )
	{
		return;
	}

	voCAutoLock lock(&m_lock);
	CloseInternalLoop();
}

VO_VOID Persist_HTTPS::CloseInternalLoop()
{
	if( socket > 0 )
	{
		vo_socket_close( socket );
		socket = -1;
	}

	if( m_ptr_ssl )
	{
		m_ssl.SSL_shutdown( m_ptr_ssl );
		m_ssl.SSL_free( m_ptr_ssl );
		m_ptr_ssl = 0;
	}

	if( m_ptr_ctx )
	{
		m_ssl.SSL_CTX_free(m_ptr_ctx);
		m_ptr_ctx = 0;
	}

	socket = -1;
	usedtimes = 0;
	deadtime = 0;
	brefreshbyserver = VO_FALSE;
	maxusetime = MAXSOCKETREUSETIMES;
}

VO_BOOL Persist_HTTPS::SetupSSLTunnel2Proxy()
{
	if( strlen( tunnelrequest ) > 0 )
	{
		VO_CHAR response[2048];
		VO_S32 status_code = -1;

		vo_socket_send_safe( socket , (VO_PBYTE)tunnelrequest , strlen(tunnelrequest) , &m_bexit );
		memset( response, 0 , sizeof(response) );

		VO_U32 count = 0;
		fd_set fdr;
		fd_set fde;
		timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 20000;
		//we should optimize it to avoid error.....
		VO_U32 get_response_start = voOS_GetSysTime();
		while( !m_bexit )
		{
			FD_ZERO(&fdr);
			FD_SET(socket,&fdr);
			FD_ZERO(&fde);
			FD_SET(socket,&fde);

			VO_S32 select_ret = select( socket + 1 ,&fdr,0,&fde,&timeout);
			if( -1 == select_ret )
			{
				//socket link error
				VOLOGE( "socket link error" );
				return VO_FALSE;
			}

			if(FD_ISSET(socket , &fde))
			{
				VOLOGE( "socket exception happen when recv" );
				return VO_FALSE;
			}

			if( 0 == select_ret )
			{
				VO_U32 read_response_cost = voOS_GetSysTime() - get_response_start;
				if( read_response_cost > VODEFAULTTIMEOUT )
				{
					VOLOGE( "read response timeout" );
					return VO_FALSE;
				}
				else
				{
					voOS_Sleep(2);
					continue;
				}
			}

			VO_BYTE value;
			vo_socket_recv( socket , (VO_PBYTE)&value , 1 );

			response[count] = value;

			//the response string is terminated with "\r\n\r\n"
			if( count >= 3 && response[count] == '\n' && response[count - 1] == '\r' 
				&& response[count - 2] == '\n' && response[count - 3] == '\r' )
				break;

			count++;

			if( count >= sizeof(response) )
			{
				VOLOGE( "Response too large!" );
				//the response is invalid, it may caused by previous remained A/V data, so reset socket.
				return VO_FALSE;
			}
		}

		VOLOGI("%s" , response );

		if( m_bexit )
		{
			VOLOGI("get_response process is interrupted" );
			return VO_FALSE;
		}

		if( strncstr( response ,(char *) "HTTP/1.1" ) == response )
		{
			sscanf( response , "HTTP/1.1 %d " , (int *)&status_code );
		}
		else if( strncstr( response , (char *)"HTTP/1.0" ) == response )
		{
			sscanf( response , "HTTP/1.0 %d " , (int *)&status_code );
		}
		else if( strncstr( response ,(char *) "ICY" ) == response )
		{                         
			sscanf( response , "ICY %d OK" , (int *)&status_code );
		}
		if( status_code != HTTP_STATUS_OK )
		{
			VOLOGE( "failed to CONNECT with ssl proxy" );
			return VO_FALSE;
		}
	}

	return VO_TRUE;
}

// 0, user notify exit; 1, success; -1 , connection fail.
VO_S32 Persist_HTTPS::OpenInternalLoop( VO_CHAR * str_host , VO_CHAR * str_port )
{
	VO_S32 ret = 0;
	VO_S32 trycount = 0;

#ifdef ENABLE_ASYNCDNS
	VO_BOOL bDNSFailed = VO_FALSE;
	while( !vo_socket_connect_asyncdns( &socket , str_host , str_port , &m_bexit , &bDNSFailed , addr_buf ) && !bDNSFailed )
#else
	while( !vo_socket_connect( &socket , str_host , str_port , &m_bexit ) )
#endif
	{
		trycount++;

		if( m_bexit )
		{
			VOLOGE( "user require to exit!" );
			return 0;
		}

		if( trycount >= MAXCONNECTRETRY )
		{
			VOLOGE( "Socket Connect Fail!" );
			return 0;
		}
	}

	//since ssl link may take more time to read/write data, so set the timeout value bigger than default 2
	if( !vo_socket_setTimeout( &socket , VOSOCKETRECVTIMEOUTSSL , VOSOCKETSENDTIMEOUTSSL ) )
	{
		VOLOGE( "set timeout failed" );
	}

	//setup http tunnel connection with proxy
	if( !SetupSSLTunnel2Proxy() )
	{
		return -1;
	}

	if( !m_bsslinit )
	{
		VOLOGE( "ssl api is not set yet." );
		return -1;
	}

	VO_BOOL bret = SSL_Init();
	if(!bret)
	{
		VOLOGE( "SSL_Init Failed" );
		return -1;
	}

	ret = m_ssl.SSL_set_fd( m_ptr_ssl , socket );
	if (ret == 0)
	{
		VOLOGE( "SSL_set_fd failed" );
		return -1;
	}

	m_ssl.RAND_poll();
	while (m_ssl.RAND_status() == 0)
	{
		unsigned short rand_ret = rand() % 65536;
		m_ssl.RAND_seed(&rand_ret, sizeof(rand_ret));
	} 

	VOLOGE("connecting....");
	ret = m_ssl.SSL_connect( m_ptr_ssl );
	if (ret != 1)
	{
		VOLOGE(  "SSL_connect failed ,ret : %d" , ret );
		VO_S32 err = m_ssl.SSL_get_error( m_ptr_ssl,ret );
		switch(err)
		{
		case SSL_ERROR_WANT_READ :
			{
				VOLOGE( "SSL_connect failed ,SSL_ERROR_WANT_READ " );
			}
			break;
		case SSL_ERROR_WANT_WRITE:
			{
				VOLOGE( "SSL_connect failed ,SSL_ERROR_WANT_WRITE " );
			}
			break;
		case SSL_ERROR_NONE:
			{
				VOLOGE( "SSL_connect failed ,SSL_ERROR_NONE " );
			}
			break;
		case SSL_ERROR_ZERO_RETURN:
			{
				VOLOGE( "SSL_connect failed ,SSL_ERROR_ZERO_RETURN " );
			}
			break;
		case SSL_ERROR_WANT_CONNECT:
			{
				VOLOGE( "SSL_connect failed ,SSL_ERROR_WANT_CONNECT " );
			}
			break;
		case SSL_ERROR_WANT_ACCEPT:
			{
				VOLOGE( "SSL_connect failed ,SSL_ERROR_WANT_ACCEPT " );
			}
			break;
		case SSL_ERROR_WANT_X509_LOOKUP:
			{
				VOLOGE( "SSL_connect failed ,SSL_ERROR_WANT_X509_LOOKUP " );
			}
			break;
		case SSL_ERROR_SYSCALL:
			{
				VOLOGE( "SSL_connect failed ,SSL_ERROR_SYSCALL " );
			}
			break;
		case SSL_ERROR_SSL:
			{
				VOLOGE( "SSL_connect failed ,SSL_ERROR_SSL " );
			}
			break;
		default:
			{
				VOLOGE( "SSL_connect failed ,SSL_ERROR_unknown " );
			}
			break;
		}

		return -1;
	}

	VOLOGE("connected");
	deadtime = voOS_GetSysTime() + MAXSOCKETKEEPALIVETIME;
	usedtimes++;

	return 1;
}

// 0, user notify exit; 1, success; -1 , connection fail.
VO_S32 Persist_HTTPS::Open( VO_CHAR * str_host , VO_CHAR * str_port )
{
	voCAutoLock lock(&m_lock);
	m_bexit = VO_FALSE;
	VO_S32 ret = 0;
	m_ssl_method = VO_SSL_SSLv23;
	do
	{
		ret = OpenInternalLoop( str_host, str_port );
		if( ret < 0 )
		{
			CloseInternalLoop();
			m_ssl_method = (VO_SSL_METHOD)(m_ssl_method + 1);
		}
	}while( ret < 0 && !m_bexit && m_ssl_method < VO_SSL_UNKNOWN );

	return ret;
}


VO_BOOL Persist_HTTPS::SSL_Init()
{
	VOLOGE("....");

	if( !m_bsslinit )
	{
		VOLOGE( "ssl api is not set yet." );
		return VO_FALSE;
	}

	if( VO_SSL_SSLv23 == m_ssl_method )
	{
		m_ptr_method = m_ssl.SSLv23_client_method();
	}
	else
	if( VO_SSL_SSLv3 == m_ssl_method )
	{
		m_ptr_method = m_ssl.SSLv3_client_method();
	}
	else
	if( VO_SSL_TLSv1 == m_ssl_method )
	{
		m_ptr_method = m_ssl.TLSv1_client_method();
	}
	
	if( !m_ptr_method )
	{
		return VO_FALSE;
	}
	m_ptr_ctx = m_ssl.SSL_CTX_new( m_ptr_method );

	if ( !m_ptr_ctx )
	{
		return VO_FALSE;
	}

	m_ptr_ssl = m_ssl.SSL_new( m_ptr_ctx );

	if (m_ptr_ssl == NULL)
	{
		return VO_FALSE;
	}

	return VO_TRUE;
}


VO_S32	Persist_HTTPS::Write( VO_PBYTE buffer , VO_S32 size )
{
	voCAutoLock lock(&m_lock);
	if( socket < 0 || NULL == m_ptr_ssl )
	{
		VOLOGE( "ssl Socket not Init yet!" );
		return -1;
	}

	VO_S32 total = 0;

	VO_U32 send_request_start = voOS_GetSysTime();
	while( total < size && !m_bexit )
	{
		VO_U32 send_request_cost = voOS_GetSysTime() - send_request_start;
		if( send_request_cost > VODEFAULTTIMEOUT )
		{
			VOLOGE( "send request timeout!" );
			return -1;
		}

		VO_S32 ret = m_ssl.SSL_write( m_ptr_ssl , buffer + total , size - total );

		if( ret == -1 )
			return -1;

		total = total + ret;
	}

	return total;
}

VO_S32 Persist_HTTPS::Read( VO_PBYTE buffer , VO_S32 size )
{
	voCAutoLock lock(&m_lock);
	if( NULL == m_ptr_ssl )
	{
		VOLOGE( "ssl Socket not Init yet!" );
		return -1;
	}

	VO_S32 ret = 0;
	VO_BOOL readfail = VO_FALSE;
	ret = m_ssl.SSL_read( m_ptr_ssl , (VO_PBYTE)buffer , size );
	switch(m_ssl.SSL_get_error( m_ptr_ssl,ret ))
	{
	case SSL_ERROR_NONE:
		{
			readfail = VO_FALSE;
		}
		break;
	case SSL_ERROR_ZERO_RETURN:
		{
			readfail = VO_TRUE;
			VOLOGE( "Socket has been gracefully closed!" );
		}
		break;
	default:
		{
			readfail = VO_TRUE;
			VOLOGE( "Socket May closed by server!" );
		}
		break;
	}

	if( readfail )
	{
		return -1;
	}

	return ret;
}

vo_http_downloader::vo_http_downloader(void):
	m_ptr_PD_param(0)
	,m_use_proxy(VO_FALSE)
	,m_status_code(0)
	,m_content_length(-1)
	,m_is_chunked(VO_FALSE)
	,m_ContentLengthValid(VO_FALSE)
	,m_is_server_canseek(VO_TRUE)
	,m_is_url_encoded(VO_FALSE)
	,m_stop_download(VO_FALSE)
	,m_is_downloading(VO_FALSE)
	,m_is_update_buffer(VO_FALSE)
	,m_current_physical_pos(0)
	,m_downloaded(0)
	,m_start_download_pos(0)
	,m_download_start_time(0)
	,m_duration(0)
	,m_timeout(30)
	,m_protocol_ver(10)
	,m_check_seek_feature(VO_FALSE)
	,m_pCompdata(0)
	,m_nComplen(0)
	,m_download_pos_thread(0)
	,m_is_thread_downloading(VO_FALSE)
	,m_buffering_end_pos(0)
	,m_download_used_time(0)
	,m_metadata_interval(0)
	,m_is_to_exit(VO_FALSE)
	,m_slow_down(0)
	,m_dlna_param(0)
	,m_limit_range_start(0)
	,m_limit_range_end(0)
	,m_is_pause_connection(VO_FALSE)
	,m_bauth(VO_FALSE)
	,m_connectretry_times(MAXCONNECTRETRY)
	,m_ptr_persist(0)
	,m_bhttps(VO_FALSE)
	,m_is_ignore_cookie_setting(VO_FALSE)
	,m_pstrWorkPath(0)
	,m_nLoopCnt(0)
	,m_download_threadhandle(0)
	,m_download_thread_processing(VO_FALSE)
{
	memset( m_reparseurl , 0 , sizeof( m_reparseurl ) );
	memset( m_cookies , 0 , sizeof( m_cookies ) );
	memset( m_mimetype , 0 , sizeof( m_mimetype ) );
	memset( m_response , 0 , sizeof(m_response) );
	memset( m_authinfo , 0 , sizeof(m_authinfo) );
	memset( m_username , 0 , sizeof(m_username) );
	memset( m_password , 0 , sizeof(m_password) );
}

vo_http_downloader::~vo_http_downloader(void)
{
	if( m_ptr_persist )
	{
		delete m_ptr_persist;
		m_ptr_persist = NULL;
	}
}

void vo_http_downloader::stop_download_thread()
{
	m_stop_download = VO_TRUE;
	VOLOGI( "+stop_download_thread" );	

	if(m_download_threadhandle)
	{
		wait_thread_exit(m_download_threadhandle);
	}
	m_download_threadhandle = 0;

	VOLOGI( "-stop_download_thread" );
	m_stop_download = VO_FALSE;
}

//set data pointer and len for check whether the server can seek or not
VO_VOID vo_http_downloader::set_comparedata(VO_PBYTE pdata , VO_S32 size)
{
	m_pCompdata = pdata;
	m_nComplen = size;
}

VO_BOOL vo_http_downloader::Is_url_https( const VO_CHAR * ptr_url )
{
	if( strncstr( (VO_CHAR *)ptr_url ,(VO_CHAR *)  "https://" ) ==  ptr_url )
	{
		return VO_TRUE;
	}
	else
	{
		return VO_FALSE;
	}
}

VO_BOOL vo_http_downloader::set_url( VO_CHAR * str_url , VOPDInitParam * ptr_PD_param )
{
	m_ptr_PD_param = ptr_PD_param;

	if( m_ptr_persist )
	{
		delete m_ptr_persist;
		m_ptr_persist = NULL;
	}

	m_bhttps = Is_url_https( str_url );
	if( m_bhttps )
	{
		m_ptr_persist = new Persist_HTTPS;
	}
	else
	{
		m_ptr_persist = new Persist_HTTP;
	}

	VOLOGI("IsSeekSupported value=%d", m_ptr_PD_param->nSeekUnsupported);
	if( m_ptr_PD_param->nSeekUnsupported == 1 )
	{
		m_is_server_canseek = VO_FALSE;
		m_check_seek_feature = VO_FALSE;
	}

	memset( m_cookies , 0 , sizeof(m_cookies) );
	if( m_ptr_PD_param->ptr_cookie )
		strcpy( m_cookies , m_ptr_PD_param->ptr_cookie );
	
	analyze_proxy();

	strcpy( m_url , str_url );
	strcpy( m_reparseurl , m_url );
	return resolve_url( str_url );
}

VO_VOID vo_http_downloader::slow_down(VO_U32 slowdown)
{
	m_slow_down = slowdown;
}

unsigned int vo_http_downloader::downloadfunc( void * ptr_obj)
{
	set_threadname( (char *) "PD_Download" );
	VOLOGE( "**********Thread_Created! name: PD_Download");
	vo_http_downloader* ptr = ( vo_http_downloader*)ptr_obj;
	ptr->m_download_thread_processing = VO_TRUE;
	ptr->downloadfunc_internal();
	VOLOGE("**********Thread_End! name:PD_Download");
	ptr->m_download_thread_processing = VO_FALSE;
	exit_thread();
	return 0;
}

VO_VOID vo_http_downloader::downloadfunc_internal()
{
	//avoid compete with start_download func
	voCAutoLock lock(&m_download_lock);

	VO_S64 download_pos = m_download_pos_thread ;
	VO_S64 download_size = DOWNLOADTOEND ;
	//VO_BOOL is_async = VO_FALSE;

	VOLOGI("[downloadfunc_internal]+++++++++++download_pos: %lld" , download_pos );

	m_start_download_pos = download_pos;
	m_current_physical_pos = download_pos;
	m_downloaded = 0;

	m_is_update_buffer = VO_FALSE;

	if( !m_ptr_persist )
	{
		VOLOGE("socket handler is not created yet");
		return;
	}
	else
	{
		m_ptr_persist->Close();
	}

	VO_S32 con_trytime = 0;

	while( !m_is_to_exit )
	{
		VOLOGI("[downloadfunc_internal]+vo_socket_connect" );
		
		VO_S32 nRes = 0;
		if( m_use_proxy )
		{
			generate_tunnel_request(m_ptr_persist->tunnelrequest);
			VOLOGI("[downloadfunc_internal]Proxy host: %s Proxy Port: %s" , m_proxy_host , m_proxy_port );
			nRes = m_ptr_persist->Open( m_proxy_host , m_proxy_port );
			if( nRes <= 0 )
			{
				if( nRes == 0 )
				{
					VOLOGE("user require to exit!" );
					return;
				}

				VOLOGE("[downloadfunc_internal]connect failed!" );
				if( con_trytime < m_connectretry_times && m_is_to_exit == VO_FALSE)
				{
					con_trytime++;
					voOS_Sleep( 200 );
					continue;
				}
				else
				{
					return;
				}
			}
		}
		else
		{
			VOLOGI("[downloadfunc_internal]host: %s Port: %s" , m_host , m_port );
			nRes = m_ptr_persist->Open( m_host , m_port );
			if( nRes <= 0 )
			{
				if( nRes == 0 )
				{
					VOLOGE("user require to exit!" );
					return;
				}
				
				VOLOGE("[downloadfunc_internal]connect failed!" );
				if( con_trytime < m_connectretry_times  && m_is_to_exit == VO_FALSE)
				{
					con_trytime++;
					voOS_Sleep( 200 );
					continue;
				}
				else
				{
					return;
				}
			}
		}

		VOLOGI("[downloadfunc_internal]-vo_socket_connect" );

		char request[4096];
		memset( request , 0 , 4096 );

		VO_S32 request_size = perpare_request( request , download_pos , download_size );
		VOLOGE("[downloadfunc_internal]%s , size: %d" , request ,request_size);
		if( -1 == m_ptr_persist->Write( (VO_PBYTE)request , request_size ) )
		{
			m_ptr_persist->Close();
			return;
		}

		m_download_start_time = voOS_GetSysTime();

		VO_BOOL ret = VO_FALSE;

		//ignore status code 100 and 101 of response string
		do
		{
			//try to get response from server 
			if( !get_response() )
			{
				if( !m_stop_download && !m_is_to_exit)
				{
					VOLOGE("Response Error!" );
				}
				m_ptr_persist->Close();
				return;
			}

		}while(m_status_code / 100 == 1);
	
		switch ( m_status_code / 100 )
		{
		case 2:
			{
				ret = VO_TRUE;
			}
			break;
		case 3:
			{
				if( m_status_code == 305 )
				{
					m_ptr_callback->download_notify( DOWNLOAD_USE_PROXY , 0 );
					return;
				}

				if( !resolve_url( m_reparseurl ) )
					return;

			//	VO_BOOL ret = start_download( download_pos , download_size , is_async);
				VOLOGE( "[downloadfunc_internal]Recursive download func all====!!!" );
				downloadfunc_internal();
				resolve_url( m_url );
				return;
			}
			break;
		case 4:
			{
				if( m_status_code == 401 || m_status_code == 407 )
				{
					//	m_authorization.set_username("admin");
					//	m_authorization.set_password("iaf49dfc16b");
					m_authorization.set_username(m_username);
					m_authorization.set_password(m_password);

					//authentication challenge
					if( m_authorization.generate_authorizationinfo() )
					{
						memset( m_authinfo , 0 , sizeof(m_authinfo) );
						strcpy( m_authinfo , m_authorization.get_authorizationinfo() );
						m_bauth = VO_TRUE;

						VOLOGE( "[downloadfunc_internal]Recursive download func call==40x!!!" );
						downloadfunc_internal();
						resolve_url( m_url );
						return;
					}
				}
				else
				if( m_status_code == 416 )
				{
					set_net_error_type(E_PD_SERVER_FILE_NOT_EXIST);
					return;
				}
				else if( m_status_code == 407 )
				{
					m_ptr_callback->download_notify( DOWNLOAD_NEED_PROXY , 0 );
					return;
				}
			}
		}
		

		if( ret )
		{
			//if response indicated that the server is ready for content data transmit
			m_stop_download = VO_FALSE;
			{
				m_is_thread_downloading = VO_TRUE;
				VOLOGE("[downloadfunc_internal]enter thread download data loop!" );
				thread_function();
				m_is_thread_downloading = VO_FALSE;
			}
			return;
		}
		else
		{
			//server has no response , or response with state code that it has not ready for content data transmit
			m_ptr_persist->Close();

			if( !m_is_server_canseek )
			{
				if( m_is_url_encoded )
				{
					resolve_url( m_url );
					m_is_url_encoded = VO_FALSE;

					if( m_use_proxy )
					{
						m_is_url_encoded = VO_FALSE;
						m_is_server_canseek = VO_TRUE;
						m_use_proxy = VO_FALSE;
					}
					else
					{
						m_ptr_callback->download_notify( DOWNLOAD_SERVER_RESPONSEERROR , 0 );
						return;
					}
				}
				else
				{
					m_is_server_canseek = VO_TRUE;
					urlencode();
					m_is_url_encoded = VO_TRUE;
				}
			}
			else
			{

				m_is_server_canseek = VO_FALSE;
			}
		}
	}
}



VO_BOOL vo_http_downloader::start_download_inthread( VO_S64 download_pos/* = 0 */, VO_S64 download_size/* = DOWNLOADTOEND*/ , VO_BOOL is_async/* = VO_TRUE*/ )
{
	voCAutoLock lock(&m_download_lock);

	m_slow_down = 0;

	if(m_download_thread_processing == VO_FALSE)
	{
		VOLOGI("start_download_inthread download start pos: %lld" , download_pos );

		m_download_pos_thread = download_pos;


		//VO_U32 threadid;
		//voThreadCreate( &m_download_threadhandle , &threadid , (voThreadProc)downloadfunc , this , 0 );
		unsigned int thread_id;
		create_thread( &m_download_threadhandle , &thread_id , downloadfunc , this , 0 );
	}

	return VO_TRUE;
}



VO_BOOL vo_http_downloader::start_download( VO_S64 download_pos/* = 0 */, VO_S64 download_size/* = DOWNLOADTOEND*/ , VO_BOOL is_async/* = VO_TRUE*/ )
{
	voCAutoLock lock(&m_download_lock);

	VOLOGI("+++++++++++download_pos: %lld" , download_pos );

	m_start_download_pos = download_pos;
	m_current_physical_pos = download_pos;
	m_downloaded = 0;

	m_is_update_buffer = VO_FALSE;
	m_slow_down = 0;

	if( !m_ptr_persist )
	{
		VOLOGE("socket handler is not created yet");
		return VO_FALSE;
	}
	else
	{
		m_ptr_persist->Close();
	}

	VO_S32 con_trytime = 0;

	while(!m_is_to_exit)
	{
		VOLOGI("+vo_socket_connect" );

		VO_S32 nRes = 0;
		if( m_use_proxy )
		{
			generate_tunnel_request(m_ptr_persist->tunnelrequest);
			VOLOGI("Proxy host: %s Proxy Port: %s" , m_proxy_host , m_proxy_port );
			nRes = m_ptr_persist->Open( m_proxy_host , m_proxy_port );
			if( nRes <= 0 )
			{
				if( nRes == 0 )
				{
					VOLOGE("user require to exit!" );
					return VO_FALSE;
				}

				VOLOGE("connect failed!" );
				if( con_trytime < MAXCONNECTRETRY  && m_is_to_exit == VO_FALSE)
				{
					con_trytime++;
					voOS_Sleep( 100 );
					continue;
				}
				else
				{
					return VO_FALSE;
				}
			}
		}
		else
		{
			VOLOGI("host: %s Port: %s" , m_host , m_port );
			nRes = m_ptr_persist->Open( m_host , m_port );
			if( nRes <= 0 )
			{
				if( nRes == 0 )
				{
					VOLOGE("user require to exit!" );
					return VO_FALSE;
				}

				VOLOGE( "connect failed!" );
				if( con_trytime < MAXCONNECTRETRY  && m_is_to_exit == VO_FALSE)
				{
					con_trytime++;
					voOS_Sleep( 100 );
					continue;
				}
				else
				{
					return VO_FALSE;
				}
			}
		}

		VOLOGI("-vo_socket_connect" );

		char request[4096];
		memset( request , 0 , 4096 );

		VO_S32 request_size = perpare_request( request , download_pos , download_size );
		VOLOGE("%s , size: %d" , request ,request_size);
		if( -1 == m_ptr_persist->Write( (VO_PBYTE)request , request_size ) )
		{
			m_ptr_persist->Close();
			return VO_FALSE;
		}

		VOLOGI( "request send finished with socket: %d " , m_ptr_persist->socket );

		m_download_start_time = voOS_GetSysTime();

		VO_BOOL ret = VO_FALSE;

		//ignore status code 100 and 101 of response string
		do
		{
			//try to get response from server 
			if( !get_response() )
			{
				if( !m_stop_download && !m_is_to_exit)
				{
					VOLOGE("Response Error!" );
				}
				m_ptr_persist->Close();
				return VO_FALSE;
			}
		}while(m_status_code / 100 == 1);
	
		switch ( m_status_code / 100 )
		{
		case 2:
			{
				//if the mimetype is not a/v , we should retry 
				if( strlen(m_mimetype) > 0 && strstr(m_mimetype , "text/html") )
				{
					m_nLoopCnt++;
					if( m_nLoopCnt < VODOWNLOADRECURSIVECNT )
					{
						ret = start_download( download_pos , download_size , is_async);
						resolve_url( m_url );
						return ret;
					}
					else
					{
						m_nLoopCnt = 0;
					}
				}
				ret = VO_TRUE;
			}
			break;
		case 3:
			{
				if( m_status_code == 305 )
				{
					m_ptr_callback->download_notify( DOWNLOAD_USE_PROXY , 0 );
					return VO_FALSE;
				}

				if( !resolve_url( m_reparseurl ) )
					return VO_FALSE;

				VO_BOOL ret = start_download( download_pos , download_size , is_async);
				resolve_url( m_url );
				return ret;
			}
			break;
		case 4:
			{
				if( m_status_code == 401 || m_status_code == 407 )
				{
					//	m_authorization.set_username("admin");
					//	m_authorization.set_password("iaf49dfc16b");
					m_authorization.set_username(m_username);
					m_authorization.set_password(m_password);

					//authentication challenge
					if( m_authorization.generate_authorizationinfo() )
					{
						memset( m_authinfo , 0 , sizeof(m_authinfo) );
						strcpy( m_authinfo , m_authorization.get_authorizationinfo() );
						m_bauth = VO_TRUE;
						VO_BOOL ret = start_download( download_pos , download_size , is_async);
						resolve_url( m_url );
						return ret;
					}
					return VO_FALSE;
				}
				else
				if( m_status_code == 416 )
				{
					set_net_error_type(E_PD_SERVER_FILE_NOT_EXIST);
					return VO_FALSE;
				}
				else if( m_status_code == 407 )
				{
					m_ptr_callback->download_notify( DOWNLOAD_NEED_PROXY , 0 );
					return VO_FALSE;
				}
			}
		}

		if( ret )
		{
			//if response indicated that the server is ready for content data transmit
			m_stop_download = VO_FALSE;
			if( is_async )
			{
				m_is_downloading = VO_TRUE;
				begin((char *)"PD_Download_m");
			}
			else
			{
				m_is_downloading = VO_TRUE;
				thread_function();
			}
			return VO_TRUE;
		}
		else
		{
			//server has no response , or response with state code that it has not ready for content data transmit
			m_ptr_persist->Close();

			if( !m_is_server_canseek )
			{
				if( m_is_url_encoded )
				{
					resolve_url( m_url );
					m_is_url_encoded = VO_FALSE;

					if( m_use_proxy )
					{
						m_is_url_encoded = VO_FALSE;
						m_is_server_canseek = VO_TRUE;
						m_use_proxy = VO_FALSE;
					}
					else
					{
						m_ptr_callback->download_notify( DOWNLOAD_SERVER_RESPONSEERROR , 0 );
						return VO_FALSE;
					}
				}
				else
				{
					m_is_server_canseek = VO_TRUE;
					urlencode();
					m_is_url_encoded = VO_TRUE;
				}
			}
			else
			{
				m_is_server_canseek = VO_FALSE;
			}
		}
	}

	return VO_FALSE;
}

VO_VOID vo_http_downloader::stop_download()
{
	m_is_stopped = VO_TRUE;
	stop_thread();

	stop_download_thread();
}

VO_VOID vo_http_downloader::thread_function()
{
	m_ptr_callback->download_notify( DOWNLOAD_START , NULL );
	VO_U32 begin = voOS_GetSysTime();

	if( m_is_chunked )
	{
		VOLOGE("Chunked!" );
		download_chunked();
	}
	else if( m_content_length <= 0 )
	{
		VOLOGE("Endless Download!" );
		download_endless();
	}
	else
	{
		VOLOGE( "Normal!" );
		download_normal();
	}

	if( m_ptr_persist )
	{
		m_ptr_persist->Close();
	}

	VO_U32 end = voOS_GetSysTime();

	VOLOGE("download_used_time: %d ms" , end - begin);

	m_download_used_time += (end - begin);

	VOLOGE("download_used_time total: %d ms" , m_download_used_time);

	m_ptr_callback->download_notify( DOWNLOAD_END , NULL );
	m_is_downloading = VO_FALSE;

	VOLOGI("Download exit!" );
}

VO_VOID vo_http_downloader::stop_thread()
{
	m_stop_download = VO_TRUE;

	VOLOGI("+thread stop" );
	stop();
	VOLOGI("-thread stop" );

	m_stop_download = VO_FALSE;
}

VO_BOOL vo_http_downloader::Is_digit(VO_CHAR * ptr_data )
{
	for( unsigned int i=0; i < strlen(ptr_data) ; i++ )
	{
		if( *(ptr_data + i) > '9' || *(ptr_data + i) < '0' )
		{
			return VO_FALSE;
		}
	}
	return VO_TRUE;
}

////http://User:b52340b4de4566b804c9880aa0b4af5f@broadway.pctv-demo.com
VO_BOOL vo_http_downloader::resolve_host( VO_CHAR * host )
{
	memset( m_username , 0 , sizeof(m_username) );
	memset( m_password , 0 , sizeof(m_password) );

	VO_CHAR * ptr_begin = host;
	VO_CHAR * ptr_div = strchr( ptr_begin , ':' );
	if( ptr_div && ptr_div - ptr_begin > 0 )
	{
		memcpy( m_username , ptr_begin , ptr_div - ptr_begin );
		ptr_begin = ptr_div;
		VO_CHAR * ptr_div = strchr( ptr_begin , '@' );
		if( ptr_div && ptr_div - ptr_begin > 0 )
		{
			memcpy( m_password , ptr_begin+1 , ptr_div - ptr_begin - 1 );

			VO_CHAR host_new[100];
			memset( host_new , 0 , sizeof(host_new) );
			strcpy( host_new , ptr_div + 1 );
			memset( m_host , 0 , sizeof(m_host) );
			strcpy( m_host , host_new);
			return VO_TRUE;
		}
		else
		{
			return VO_FALSE;
		}
	}
	else
	{
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_BOOL vo_http_downloader::vostr_is_digit(VO_CHAR * ptr_data )
{
	for( int i=0; i < (int)strlen(ptr_data) ; i++ )
	{
		if( *(ptr_data + i) > '9' || *(ptr_data + i) < '0' )
		{
			return VO_FALSE;
		}
	}
	return VO_TRUE;
}

VO_BOOL vo_http_downloader::resolve_urlHTTP( VO_CHAR * url )
{
	memset( m_host , 0 , sizeof(m_host) );
	memset( m_path , 0 , sizeof(m_path) );
	memset( m_port , 0 , sizeof(m_port) );

	//http://m.tvpot.daum.net 
	//we should deal with such case

	VO_CHAR * ptr = strstr( url , "http://" );

	if( ptr )
	{
		ptr = ptr + strlen( "http://" );
	}
	else
	{
	//	ptr = url;
		return VO_FALSE;
	}

	VO_CHAR * ptr_div = strchr( ptr , '/' );
	
	if(ptr_div == NULL)
	{
		strcpy( m_host , ptr);
		strcpy( m_path , "/" );
	}
	else
	{
		strncpy( m_host , ptr , ptr_div - ptr );
		strcpy( m_path , ptr_div );
	}

	ptr_div = strrchr( m_host , ':' );

	if( ptr_div )
	{
		ptr_div++;
		strcpy( m_port , ptr_div );
		if( !vostr_is_digit(m_port) )
		{
			strcpy( m_port , "80" );
		}
		else
		{
			ptr_div = strrchr( m_host , ':' );
			*ptr_div = '\0';
		}
	}
	else
	{
		strcpy( m_port , "80" );
	}

	if( strchr( m_host , ':' ) && strrchr( m_host , '@' ) )
	{
		return resolve_host( m_host ) ;
	}

	return VO_TRUE;
}


VO_BOOL vo_http_downloader::resolve_urlHTTPS( VO_CHAR * url )
{
	memset( m_host , 0 , sizeof(m_host) );
	memset( m_path , 0 , sizeof(m_path) );
	memset( m_port , 0 , sizeof(m_port) );
	VO_CHAR * ptr = strstr( url , "https://" );

	if( ptr )
	{
		ptr = ptr + strlen( "https://" );
	}
	else
	{
	//	ptr = url;
		return VO_FALSE;
	}

	VO_CHAR * ptr_div = strchr( ptr , '/' );
	
	if(ptr_div == NULL)
	{
		strcpy( m_host , ptr);
		strcpy( m_path , "/" );
	}
	else
	{
		strncpy( m_host , ptr , ptr_div - ptr );
		strcpy( m_path , ptr_div );
	}

	ptr_div = strrchr( m_host , ':' );

	if( ptr_div )
	{
		ptr_div++;
		strcpy( m_port , ptr_div );
		if( !vostr_is_digit(m_port) )
		{
			strcpy( m_port , "443" );
		}
		else
		{
			ptr_div = strrchr( m_host , ':' );
			*ptr_div = '\0';
		}
	}
	else
	{
		strcpy( m_port , "443" );
	}

	if( strchr( m_host , ':' ) && strrchr( m_host , '@' ) )
	{
		return resolve_host( m_host ) ;
	}

	return VO_TRUE;
}

//http://admin:iaf49dfc16b@10.2.64.44:8888/tmp/stream0.m3u8
VO_BOOL vo_http_downloader::resolve_url( VO_CHAR * url )
{
	if( m_bhttps )
	{
		return resolve_urlHTTPS( url );
	}
	else
	{
		return resolve_urlHTTP( url );
	}
}

VO_S32 vo_http_downloader::perpare_request( VO_CHAR * ptr_request , VO_S64 download_pos , VO_S64 download_size )
{
	if(m_dlna_param && m_dlna_param->is_dlna() )
	{
		return dlna_perpare_request(ptr_request , download_pos , download_size);
	}
	else
	{
		return normal_perpare_request(ptr_request , download_pos , download_size);
	}
}

VO_S32 vo_http_downloader::normal_perpare_request( VO_CHAR * ptr_request , VO_S64 download_pos , VO_S64 download_size )
{
	//char * test_request = "GET /videoplayback?ip=0.0.0.0&sparams=id%2Cexpire%2Cip%2Cipbits%2Citag%2Calgorithm%2Cburst%2Cfactor%2Coc%3AU0dWSVJQVF9FSkNNNl9KSlhB&fexp=900028%2C904711&algorithm=throttle-factor&itag=18&ipbits=0&burst=40&sver=3&expire=1274256000&key=yt1&signature=06E472CB53DF30443AFF322A2749DF9D46742F6C.2147FE106A5BDE0C502A4BE5FC7CF26FEB56BD06&factor=1.25&id=0ebcc61e0986df2e&redirect_counter=1 HTTP/1.1\r\nHost: v5.cache3.c.youtube.com\r\nConnection: keep-alive\r\nUser-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US) AppleWebKit/532.5 (KHTML, like Gecko) Chrome/4.1.249.1064 Safari/532.5\r\nAccept: application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5\r\nAccept-Encoding: gzip,deflate,sdch\r\nAccept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\nCookie: VISITOR_INFO1_LIVE=hRebMrJjw6k; dkv=d3b272c9187e19ed232bf5e85246946ae3QEAAAAdGxpcGnVa6xLMA==; __utmz=27069237.1272268319.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none); __utma=27069237.109834039.1272268319.1272268319.1272268319.1\r\n\r\n";
	//strcpy( ptr_request , test_request );

	//return strlen( test_request );

	VO_S32 size = 0;

	if( m_is_server_canseek && (!( download_pos == 0 && download_size == DOWNLOADTOEND )) )
	{
		VO_S64 download_start = download_pos;
		VO_S64 download_end = download_start + download_size - 1;

		//set check flag for range request support feature, moses
		if( download_pos != 0)
		{
			m_check_seek_feature = VO_TRUE;
			VOLOGI("the value of start download pos is not zero ,so set check flag for range request");
		}

		if( strcmp( m_port , "80" ) )
		{
			if( download_size != DOWNLOADTOEND )
			{
				const char * request_template = (const char *)"GET %s HTTP/1.1\r\nHost: %s:%s\r\nRange: bytes=%lld-%lld\r\nIcy-MetaData:1\r\nx-ms-version: 2011-08-18\r\nConnection: keep-alive\r\n";
				size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , m_port , download_start , download_end );
			}
			else
			{
				//we should try our best to provide accurate request string.
				//if(m_filesize > 0)
				//{
				//	char * request_template = "GET %s HTTP/1.1\r\nHost: %s:%s\r\nRange: bytes=%lld-%lld\r\nConnection: keep-alive\r\n";
				//	size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path, m_host , m_port , download_start, (m_filesize - 1));
				//}
				//else
				{
					const char * request_template = (const char *)"GET %s HTTP/1.1\r\nHost: %s:%s\r\nRange: bytes=%lld-\r\nIcy-MetaData:1\r\nx-ms-version: 2011-08-18\r\nConnection: keep-alive\r\n";
					size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path, m_host , m_port , download_start );
				}
				
			}
		}
		else
		{
			if( download_size != DOWNLOADTOEND )
			{
				const char * request_template = (const char *)"GET %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%lld-%lld\r\nIcy-MetaData:1\r\nx-ms-version: 2011-08-18\r\nConnection: keep-alive\r\n";
				size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , download_start , download_end );
			}
			else
			{
				//we should try our best to provide accurate request string.
				//if(m_filesize > 0)
				//{
				//	char * request_template = "GET %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%lld-%lld\r\nConnection: keep-alive\r\n";
				//	size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , download_start, (m_filesize - 1) );
				//}
				//else
				{
					const char * request_template = (const char *)"GET %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%lld-\r\nIcy-MetaData:1\r\nx-ms-version: 2011-08-18\r\nConnection: keep-alive\r\n";
					size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , download_start );
				}
			}
		}
	}
	//because Range: bytes=0- is not same with no-specify. in https://sh.visualon.com:8443/node/7410
	//if 0- specify, the content-len value will be right
	//if no specify, the content-len value will be right value + 1, so strange server!!
	//LG case is described as above;
	//but for kt case , the situation is just opposite,detail to: https://sh.visualon.com:8443/node/7941
	else if( m_is_server_canseek && ( download_pos == 0 && download_size == DOWNLOADTOEND ) )
	{
		//for lg
		//if( strcmp( m_port , "80" ) )
		//{
		//	const char * request_template = (const char *)"GET %s HTTP/1.1\r\nHost: %s:%s\r\nConnection: keep-alive\r\nRange: bytes=0-\r\nIcy-MetaData:1\r\nx-ms-version: 2011-08-18\r\n";
		//	size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , m_port );
		//}
		//else
		//{
		//	const char * request_template = (const char *)"GET %s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\nRange: bytes=0-\r\nIcy-MetaData:1\r\nx-ms-version: 2011-08-18\r\n";
		//	//char * request_template = "GET %s HTTP/1.1\r\nUser-Agent: HTC Streaming Player HTC / 1.0 / htc_ace / 2.2\r\nx-network-type: UMTS\r\nBandwidth: 1920000\r\nx-wap-profile: http://uaprof.vtext.com/pcd/adr63k/adr63k.xml\r\nHost: %s\r\n\r\n";
		//	//return sprintf( ptr_request , request_template , m_path , m_host );
		//	size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host );
		//}

		//for kt
		if( strcmp( m_port , "80" ) )
		{
			const char * request_template = "GET %s HTTP/1.1\r\nHost: %s:%s\r\nConnection: keep-alive\r\nIcy-MetaData:1\r\nx-ms-version: 2011-08-18\r\n";
			size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , m_port );
		}
		else
		{
			const char * request_template = "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\nIcy-MetaData:1\r\nx-ms-version: 2011-08-18\r\n";
			//char * request_template = "GET %s HTTP/1.1\r\nUser-Agent: HTC Streaming Player HTC / 1.0 / htc_ace / 2.2\r\nx-network-type: UMTS\r\nBandwidth: 1920000\r\nx-wap-profile: http://uaprof.vtext.com/pcd/adr63k/adr63k.xml\r\nHost: %s\r\n\r\n";
			//return sprintf( ptr_request , request_template , m_path , m_host );
			size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host );
		}
	}
	else
	{
		if( strcmp( m_port , "80" ) )
		{
			const char * request_template = (const char *)"GET %s HTTP/1.1\r\nHost: %s:%s\r\nIcy-MetaData:1\r\nx-ms-version: 2011-08-18\r\nConnection: keep-alive\r\n";
			size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , m_port );
		}
		else
		{
			const char * request_template = (const char *)"GET %s HTTP/1.1\r\nHost: %s\r\nIcy-MetaData:1\r\nx-ms-version: 2011-08-18\r\nConnection: keep-alive\r\n";
			//char * request_template = "GET %s HTTP/1.1\r\nUser-Agent: HTC Streaming Player HTC / 1.0 / htc_ace / 2.2\r\nx-network-type: UMTS\r\nBandwidth: 1920000\r\nx-wap-profile: http://uaprof.vtext.com/pcd/adr63k/adr63k.xml\r\nHost: %s\r\n\r\n";
			//return sprintf( ptr_request , request_template , m_path , m_host );
			size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host );
		}
	}

	if( strlen( m_cookies ) )
	{
		strcat( ptr_request , m_cookies );
		size = size + strlen( m_cookies );
	}

	if( m_bauth )
	{
		strcat( ptr_request , m_authinfo );
		strcat( ptr_request , (const char *)"\r\n" );
		size = size + strlen( m_authinfo ) + strlen( (const char *)"\r\n" );
	}

	if( strncstr( ptr_request ,(char *)  "User-Agent:" ) == NULL )
	{
		strcat( ptr_request ,(const char *) "User-Agent: " );
		if( strlen( (const char *)m_ptr_PD_param->mUserAgent ) )
		{
			strcat( ptr_request , (VO_CHAR*)m_ptr_PD_param->mUserAgent );
			strcat( ptr_request ,(const char *)  "\r\n" );
			size = size + strlen( (const char *) "User-Agent: " ) + strlen( (const char *) "\r\n" ) + strlen( (const char *)m_ptr_PD_param->mUserAgent ) ;
		}
		else
		{
			//defalut user agent info
#if defined (_IOS) || defined (_MAC_OS)
		strcat( ptr_request ,(char *) "VisualOn PD Player(iOS)\r\n" );
		size = size + strlen( (const char *) "User-Agent: " ) + strlen( (const char *)"VisualOn PD Player(iOS)\r\n" );
#elif defined (_LINUX_ANDROID) || defined(LINUX)
		strcat( ptr_request ,(char *) "VisualOn PD Player(Linux;Android)\r\n" );
		size = size + strlen( (const char *) "User-Agent: " ) + strlen( (const char *)"VisualOn PD Player(Linux;Android)\r\n" );
#elif defined (_WIN32)
		strcat( ptr_request ,(char *) "VisualOn PD Player(Windows)\r\n" );
		size = size + strlen( (const char *) "User-Agent: " ) + strlen( (const char *)"VisualOn PD Player(Windows)\r\n" );
#endif
		}
	}

	strcat( ptr_request ,(char *)  "\r\n" );

	size = size + 2;

	return size;
}

VO_S32 vo_http_downloader::dlna_perpare_request( VO_CHAR * ptr_request , VO_S64 download_pos , VO_S64 download_size )
{
	//char * test_request = "GET /videoplayback?ip=0.0.0.0&sparams=id%2Cexpire%2Cip%2Cipbits%2Citag%2Calgorithm%2Cburst%2Cfactor%2Coc%3AU0dWSVJQVF9FSkNNNl9KSlhB&fexp=900028%2C904711&algorithm=throttle-factor&itag=18&ipbits=0&burst=40&sver=3&expire=1274256000&key=yt1&signature=06E472CB53DF30443AFF322A2749DF9D46742F6C.2147FE106A5BDE0C502A4BE5FC7CF26FEB56BD06&factor=1.25&id=0ebcc61e0986df2e&redirect_counter=1 HTTP/1.1\r\nHost: v5.cache3.c.youtube.com\r\nConnection: keep-alive\r\nUser-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US) AppleWebKit/532.5 (KHTML, like Gecko) Chrome/4.1.249.1064 Safari/532.5\r\nAccept: application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5\r\nAccept-Encoding: gzip,deflate,sdch\r\nAccept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\nCookie: VISITOR_INFO1_LIVE=hRebMrJjw6k; dkv=d3b272c9187e19ed232bf5e85246946ae3QEAAAAdGxpcGnVa6xLMA==; __utmz=27069237.1272268319.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none); __utma=27069237.109834039.1272268319.1272268319.1272268319.1\r\n\r\n";
	//strcpy( ptr_request , test_request );

	//return strlen( test_request );

	VO_S32 size = 0;

	
	//if range is limited, the current download pos is out of range limitation, then just skip it.
	if(m_dlna_param && m_dlna_param->is_range_limit() && ( download_pos > m_limit_range_end || download_pos < m_limit_range_start))
	{
		ptr_request = NULL;
		return 0;
	}

	if( m_dlna_param && !m_dlna_param->is_byterange_support() )
	{
		char * request_template;
		m_is_server_canseek = VO_FALSE;
		if( strcmp( m_port , "80" ) )
		{
			request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s:%s\r\nConnection: keep-alive\r\n";
			size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , m_port );
		}
		else
		{
			request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\n";
			//char * request_template = "GET %s HTTP/1.1\r\nUser-Agent: HTC Streaming Player HTC / 1.0 / htc_ace / 2.2\r\nx-network-type: UMTS\r\nBandwidth: 1920000\r\nx-wap-profile: http://uaprof.vtext.com/pcd/adr63k/adr63k.xml\r\nHost: %s\r\n\r\n";
			//return sprintf( ptr_request , request_template , m_path , m_host );
			size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host );
		}
	}
	else
	{
		if( m_is_server_canseek && (!( download_pos == 0 && download_size == DOWNLOADTOEND )) )
		{
			VO_S64 download_start = download_pos;
			VO_S64 download_end = download_start + download_size - 1;

			if( m_dlna_param && m_dlna_param->is_range_limit() && (m_limit_range_start !=0 || m_limit_range_end != 0) )
			{
				VOLOGI("it is limited random access ,so check range");
				//start download position should not be changed, else it will cause problem when store the data to buffer
				//	download_start = download_start < m_limit_range_start ? m_limit_range_start : download_start;
				download_end = download_end > m_limit_range_end ? m_limit_range_end : download_end;
			}

			//set check flag for range request support feature, moses
			if( download_pos != 0)
			{
				m_check_seek_feature = VO_TRUE;
				VOLOGI("the value of start download pos is not zero ,so set check flag for range request");
			}

			if( strcmp( m_port , "80" ) )
			{
				if( download_size != DOWNLOADTOEND )
				{
					char * request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s:%s\r\nRange: bytes=%lld-%lld\r\nConnection: keep-alive\r\n";
					size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , m_port , download_start , download_end );
				}
				else
				{
					//we should try our best to provide accurate request string.
					if(m_filesize > 0)
					{
						VO_S64	end_pos = 0;
						end_pos = m_filesize - 1;
						if( m_dlna_param && m_dlna_param->is_range_limit() && (m_limit_range_start !=0 || m_limit_range_end != 0) )
						{
							end_pos = end_pos > m_limit_range_end ? m_limit_range_end : end_pos;
						}
						char * request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s:%s\r\nRange: bytes=%lld-%lld\r\nConnection: keep-alive\r\n";
						size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path, m_host , m_port , download_start, end_pos );
					}
					else
					{
						if( m_dlna_param && m_dlna_param->is_range_limit() )
						{
							char * request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s:%s\r\nRange: bytes=%lld-\r\nConnection: keep-alive\r\ngetAvailableSeekRange.dlna.org:1\r\n";
							size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path, m_host , m_port , download_start );
						}
						else
						{
							char * request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s:%s\r\nRange: bytes=%lld-\r\nConnection: keep-alive\r\n";
							size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path, m_host , m_port , download_start );
						}
					}

				}
			}
			else
			{
				if( download_size != DOWNLOADTOEND )
				{
					char * request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%lld-%lld\r\nConnection: keep-alive\r\n";
					size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , download_start , download_end );
				}
				else
				{
					//we should try our best to provide accurate request string.
					if(m_filesize > 0)
					{
						VO_S64	end_pos = 0;
						end_pos = m_filesize - 1;

						if( m_dlna_param && m_dlna_param->is_range_limit() && (m_limit_range_start !=0 || m_limit_range_end != 0) )
						{
							end_pos = end_pos > m_limit_range_end ? m_limit_range_end : end_pos;
						}

						char * request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%lld-%lld\r\nConnection: keep-alive\r\n";
						size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , download_start, end_pos );
					}
					else
					{
						if( m_dlna_param && m_dlna_param->is_range_limit() )
						{
							char * request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%lld-\r\nConnection: keep-alive\r\ngetAvailableSeekRange.dlna.org:1\r\n";
							size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , download_start );
						}
						else
						{
							char * request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%lld-\r\nConnection: keep-alive\r\n";
							size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , download_start );
						}
					}
				}
			}
		}
		else if( m_is_server_canseek && ( download_pos == 0 && download_size == DOWNLOADTOEND ) )
		{
			if( strcmp( m_port , "80" ) )
			{
				if( m_dlna_param && m_dlna_param->is_range_limit() )
				{
					char * request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s:%s\r\nConnection: keep-alive\r\nRange: bytes=0-\r\ngetAvailableSeekRange.dlna.org:1\r\n";
					size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , m_port );
				}
				else
				{
					char * request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s:%s\r\nConnection: keep-alive\r\nRange: bytes=0-\r\n";
					size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , m_port );
				}
			}
			else
			{
				if( m_dlna_param && m_dlna_param->is_range_limit() )
				{
					char * request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\nRange: bytes=0-\r\ngetAvailableSeekRange.dlna.org:1\r\n";
					size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host );
				}
				else
				{
					char * request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\nRange: bytes=0-\r\n";
					size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host );
				}
			}
		}
		else
		{
			if( strcmp( m_port , "80" ) )
			{
				if( m_dlna_param && m_dlna_param->is_range_limit() )
				{
					char * request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s:%s\r\nConnection: keep-alive\r\nRange: bytes=0-\r\ngetAvailableSeekRange.dlna.org:1\r\n";
					size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , m_port );
				}
				else
				{
					char * request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s:%s\r\nConnection: keep-alive\r\nRange: bytes=0-\r\n";
					size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , m_port );
				}
			}
			else
			{
				if( m_dlna_param && m_dlna_param->is_range_limit() )
				{
					char * request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\nRange: bytes=0-\r\ngetAvailableSeekRange.dlna.org:1\r\n";
					//char * request_template = "GET %s HTTP/1.1\r\nUser-Agent: HTC Streaming Player HTC / 1.0 / htc_ace / 2.2\r\nx-network-type: UMTS\r\nBandwidth: 1920000\r\nx-wap-profile: http://uaprof.vtext.com/pcd/adr63k/adr63k.xml\r\nHost: %s\r\n\r\n";
					//return sprintf( ptr_request , request_template , m_path , m_host );
					size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host );
				}
				else
				{
					char * request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\nRange: bytes=0-\r\n";
					//char * request_template = "GET %s HTTP/1.1\r\nUser-Agent: HTC Streaming Player HTC / 1.0 / htc_ace / 2.2\r\nx-network-type: UMTS\r\nBandwidth: 1920000\r\nx-wap-profile: http://uaprof.vtext.com/pcd/adr63k/adr63k.xml\r\nHost: %s\r\n\r\n";
					//return sprintf( ptr_request , request_template , m_path , m_host );
					size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host );
				}
			}
		}
	}

	if( strlen( m_cookies ) )
	{
		strcat( ptr_request , m_cookies );
		size = size + strlen( m_cookies );
	}

	//append up level http request string
	if(m_dlna_param && strlen (m_dlna_param->get_reqstr()) )
	{
		size = RemoveProperty( ptr_request , (VO_CHAR *)"User-Agent:" , size );

		strcat( ptr_request , m_dlna_param->get_reqstr() );
		size = size + strlen( m_dlna_param->get_reqstr() );
	}

	if( strncstr( ptr_request ,(char *) "User-Agent:" ) == NULL )
	{
		strcat( ptr_request ,(const char *) "User-Agent: " );
		strcat( ptr_request , (VO_CHAR*)m_ptr_PD_param->mUserAgent );
		strcat( ptr_request , (const char *)"\r\n" );
		size = size + strlen( (const char *) "User-Agent: " ) + strlen( (const char *)"\r\n" ) + strlen( (char *)m_ptr_PD_param->mUserAgent ) ;
	}

	strcat( ptr_request , (const char *)"\r\n" );

	size = size + 2;

	return size;
}

VO_S32 vo_http_downloader::RemoveProperty(VO_CHAR* ptr_request ,VO_CHAR* properties , VO_S32 size )
{
	if( strncstr( ptr_request , properties ) && strncstr( m_dlna_param->get_reqstr() , properties ) )
	{
		//remove old properties string
		VO_CHAR * pagent = NULL;
		pagent = strncstr( ptr_request , properties );
		if( pagent )
		{
			VO_CHAR* pbegin = pagent;
			while( *pbegin != '\r' && *(pbegin+1) != '\n')
			{
				pbegin++;
			}

			size = size - ( pbegin + 2 - pagent );
			if( *(pbegin+2) != '\0' )
			{
				strcpy( pagent , pbegin + 2 );
			}
			else
			{
				*pagent = '\0';
			}
		}
	}
	else
	{
		return size;
	}

	return size;
}

VO_BOOL vo_http_downloader::get_response()
{
	VOLOGI("Begin Get Response!" );
	VO_U32 count = 0;
	memset( m_response , 0 , sizeof(m_response) );

	//for 8k response string with padding data 
	VO_CHAR str_response[2048*4];
	memset( str_response , 0 , sizeof(str_response) );

	VO_U32 get_response_start = voOS_GetSysTime();
	while( !m_stop_download && !m_is_to_exit)
	{
		//check timeout 35s ,if so, then exit get response from server
		VO_U32 get_response_cost = voOS_GetSysTime() - get_response_start;
		if( get_response_cost > 35000 )
		{
			VOLOGE( "Response timeout!" );
			return VO_FALSE;
		}

		VO_CHAR value;
		VO_S32 ret = 0;
		ret = m_ptr_persist->Read( (VO_PBYTE)&value , 1 );
		if( ret <= 0 )
		{
			if( ret < 0 )
			{
				VOLOGE( "Response connection error!" );
			}
			else
			{
				VOLOGE( "exit called when get response" );
			}
			return VO_FALSE;
		}

		str_response[count] = value;

		if( ( count >= 3 && str_response[count] == '\n' && str_response[count - 1] == '\r' 
			&& str_response[count - 2] == '\n' && str_response[count - 3] == '\r' ) ||
			( count >= 1 && str_response[count] == '\n' && str_response[count - 1] == '\n' ))
			break;

		count++;

		if( count >= sizeof(str_response) )
		{
			VOLOGE("Response too large!");
			return VO_FALSE;
		}
	}

	//we should exit directly if required to stop download or exit
	if( m_stop_download || m_is_to_exit)
	{
		VOLOGI("get_response process is interrupted" );
		return VO_FALSE;
	}

	memcpy(m_response,str_response,(count  >= sizeof(m_response) ? sizeof(m_response) - 1 : count));

	if( m_dlna_param && m_dlna_param->is_dlna() )
	{
		//up level need to know the response string.
		m_dlna_param->set_server_response( m_response );
	}
	else
	{
		m_ptr_callback->download_notify( DOWNLOAD_SERVER_RESPONSE , &m_response );
	}

	//strcpy( m_response , "HTTP/1.1 401 Authorization Required\r\nDate: Thu, 19 Aug 2010 10:07:48 GMT\r\nServer: Apache/2.2.11 (Fedora)\r\nWWW-Authenticate: Basic realm=\"Your Password please. To get it ask your TM.\"\r\nContent-Length: 401\r\nKeep-Alive: timeout=15, max=100\r\nConnection: Keep-Alive\r\nContent-Type: text/html; charset=iso-8859-1\r\n\r\n" );
	analyze_response();

	return VO_TRUE;
}


VO_S32 vo_http_downloader::StatusCode()
{
	if( strncstr( m_response ,(char *)  "<html>" ) ==  m_response )
	{
		//the response is in webpage format
		if( strncstr( m_response , (char *) "HTTP/1.1" ) )
		{
			sscanf( m_response , "HTTP/1.1 %ld " , &m_status_code );
			m_protocol_ver = 11;
		}
		else if( strncstr( m_response , (char *) "HTTP/1.0" ) )
		{
			sscanf( m_response , "HTTP/1.0 %ld " , &m_status_code );
			m_protocol_ver = 10;
		}
		else if( strncstr( m_response ,(char *)  "ICY" ) )
		{                         
			sscanf( m_response , "ICY %ld OK" , &m_status_code );
			m_protocol_ver = 11;
			VOLOGE("the link maybe is a shoutcast radio" );
		}
		else
		{
			m_status_code = -1;
			m_protocol_ver = 10;
		}
	}
	else
	{
		if( strncstr( m_response , (char *) "HTTP/1.1" ) == m_response )
		{
			sscanf( m_response , "HTTP/1.1 %ld " , &m_status_code );
			m_protocol_ver = 11;
		}
		else if( strncstr( m_response , (char *) "HTTP/1.0" ) == m_response )
		{
			sscanf( m_response , "HTTP/1.0 %ld " , &m_status_code );
			m_protocol_ver = 10;
		}
		else if( strncstr( m_response ,(char *)  "ICY" ) == m_response )
		{                         
			sscanf( m_response , "ICY %ld OK" , &m_status_code );
			m_protocol_ver = 11;
			VOLOGE("the link maybe is a shoutcast radio" );
		}
		else
		{
			m_status_code = -1;
			m_protocol_ver = 10;
		}
	}
	

	return m_status_code;
}

VO_BOOL vo_http_downloader::IsChunked()
{
	VO_CHAR * ptr = strncstr( m_response ,(char *)  "Transfer-Encoding: chunked" );
	if( ptr )
	{
		m_is_chunked = VO_TRUE;
	}
	else
	{
		m_is_chunked = VO_FALSE;
	}

	return m_is_chunked;
}

VO_BOOL vo_http_downloader::IsValidContentLength()
{
	VO_CHAR ContentLenTag[20];
	m_ContentLengthValid = VO_FALSE;

	strcpy(ContentLenTag , "Content-Length: ");
	VO_CHAR *ptr = strncstr( m_response , ContentLenTag );
	if(!ptr)
	{
		strcpy(ContentLenTag , "Content-Length:");
		ptr = strncstr( m_response , ContentLenTag );
	}

	if( ptr )
	{
		m_content_length = 0;

		ptr = ptr + strlen( ContentLenTag );

		if(ptr)
		{
			//we need to assure that the value of Content-Length is successfully sscanf in.
			VO_S32 ret = sscanf( ptr , "%lld\r\n" , &m_content_length );
			if(ret != 1)
			{
				VOLOGE("the value of Content-Length  is expected but failed to sscanf" );
				m_ContentLengthValid = VO_FALSE;
			}
			else
			{
				//all kinds of shit server you can meet,for example: https://sh.visualon.com:8443/node/7410  , so be tolerant
			//	if(m_start_download_pos == 0 && m_status_code == 200)
				if(m_start_download_pos == 0 && m_status_code / 100 == 2)
				{
					//if the request pos is from 0, and status code is 2xx ,then  Content-Length is creditable
					VOLOGI("the value of Content-Length:%lld  is accept as file size", m_content_length);
					m_ContentLengthValid = VO_TRUE;
				}
			}
		}
	}

	return m_ContentLengthValid;
}

VO_VOID vo_http_downloader::ContentType()
{
	VO_CHAR *ptr = strncstr( m_response ,(char *)  "Content-Type: " );

	memset( m_mimetype , 0 , sizeof( m_mimetype ) );

	if( ptr )
	{
		ptr = ptr + strlen( "Content-Type: " );
		sscanf( ptr , "%s\r\n" , m_mimetype );
	}
}

VO_VOID vo_http_downloader::AcceptRange()
{
	VO_CHAR *ptr = strncstr( m_response ,(char *)  "Accept-Ranges: " );

	VO_CHAR acceptrange[50];
	memset( acceptrange , 0 , sizeof( acceptrange ) );

	if( ptr )
	{
		ptr = ptr + strlen( "Accept-Ranges: " );
		sscanf( ptr , "%s\r\n" , acceptrange );
		if(!strcmp(acceptrange,"none"))
		{
			//server tell us that it did not support range request
			m_is_server_canseek = VO_FALSE;

			//since we already know that it did not support range request, so we won't check it again.
			m_check_seek_feature = VO_FALSE;
		}
	}
}

VO_VOID vo_http_downloader::MetadataInt()
{
	VO_CHAR MetadataTag[20];
	strcpy(MetadataTag , "icy-metaint: ");
	VO_CHAR *ptr = strncstr( m_response , MetadataTag );
	if(!ptr)
	{
		strcpy(MetadataTag , "icy-metaint:");
		ptr = strncstr( m_response , MetadataTag );
	}

	if( ptr )
	{
		m_metadata_interval = 0;

		ptr = ptr + strlen( MetadataTag );

		if(ptr)
		{
			sscanf( ptr , "%d\r\n" , (int *)&m_metadata_interval );
		}
	}
	else
	{
		m_metadata_interval = 0;
	}

	return;
}

VO_VOID vo_http_downloader::Duration()
{
	VO_CHAR *ptr = strncstr( m_response ,(char *)  "X-AvailableSeekRange:" );

	if( ptr )
	{
		ptr = ptr + strlen( "X-AvailableSeekRange:" );
		VO_CHAR * ptr_end = strstr( ptr , "\r\n" );
		ptr = strchr( ptr , '-' );
		if( ptr && ptr < ptr_end )
		{
			ptr++;
			float duration;
			VO_S32 ret = sscanf( ptr , "%f\r\n" , &duration );
			if(ret != 1)
			{
				m_duration = 0;
			}
			else
			{
				m_duration = (VO_U32)duration * 1000;
			}
		}
	}
}

VO_VOID vo_http_downloader::AvailableSeekRange()
{
	VO_CHAR *ptr = strncstr( m_response , (char *)"availableSeekRange.dlna.org:" );

	if( ptr )
	{
		ptr = ptr + strlen((const char *)"availableSeekRange.dlna.org:");
		ptr = strncstr( ptr ,(char *) "bytes=" );
		if ( ptr )
		{
			//we need to assure that the value of start-end in availableSeekRange.dlna.org is successfully sscanf in.
			VO_S32 ret = sscanf( ptr + strlen((const char *)"bytes=") , "%lld-%lld" , &m_limit_range_start , &m_limit_range_end );

			if(ret != 2)
			{
				m_limit_range_start = 0;
				m_limit_range_end = 0;
				VOLOGE("the value of start-end in availableSeekRange.dlna.org is expected but failed to sscanf" );
			}
			else if(m_limit_range_end < m_limit_range_start)
			{
				m_limit_range_start = 0;
				m_limit_range_end = 0;
			}
			else
			{
				VOLOGI("the value of start-end in availableSeekRange.dlna.org %lld-%lld ", m_limit_range_start , m_limit_range_end );
			}
		}
	}
}

VO_VOID vo_http_downloader::Relocation()
{
	memset( m_reparseurl , 0 , sizeof( m_reparseurl ) );

	VO_CHAR *ptr = strncstr( m_response ,(char *)  "Location:" );
	if(!ptr)
	{
		VOLOGE( "Relocation tag not found" );
		return;
	}
	else
	{
		ptr = ptr + strlen( "Location:" );

		VO_S32 nlen = 0;
		while( *ptr != 'h' )
		{
			ptr++;
			if(nlen++ > 2048)
			{
				VOLOGE( "Relocation url not found" );
				return;
			}
		}

		
		VO_CHAR * ptr_temp = strchr( ptr , '\r' );
		if(!ptr_temp)
		{
			VO_CHAR * ptr_temp = strchr( ptr , '\n' );
			if(!ptr_temp)
			{
				VOLOGE("Relocation end tag not found" );
				return;
			}
		}

		nlen = ptr_temp - ptr;
		if(nlen < sizeof(m_reparseurl))
		{
			strncpy( m_reparseurl , ptr , nlen);
		}
		else
		{
			VOLOGE("Relocation url too long" );
		}
	}
}

VO_VOID vo_http_downloader::ContentRange()
{
	VO_CHAR *ptr = strncstr( m_response ,(char *)  "Content-Range: bytes" );
	VO_S64 filesize;

	if( ptr )
	{
		ptr = ptr + strlen("Content-Range: ");
		const VO_PCHAR slashPos = strchr(ptr, '/');
		if (slashPos != NULL)
		{
			ptr = slashPos ;
			//we need to assure that the value of filesize in Content-Range is successfully sscanf in.
			VO_S32 ret = sscanf( ptr , "/%lld" , &filesize );

			if(ret != 1)
			{
				VOLOGE("the value of filesize in Content-Range  is expected but failed to sscanf" );
			}
			else 
			{
				if(m_ContentLengthValid == VO_FALSE)
				{
					VOLOGI("the value of filesize in Content-Range:%lld  is accept as file size", filesize);
					//only when the  Content-Length is not creditable, we should not use this value for file-size refrence
					m_filesize = filesize;
				}
			}
		}
	}
}

VO_VOID vo_http_downloader::generate_cookie_str()
{
	memset( m_cookies , 0 , sizeof( m_cookies ) );

	cookie_storage_node* pCookie = m_http_cookie.get_cooki_node();
	if( pCookie )
	{
		strcat( m_cookies , "Cookie: ");
		strcat( m_cookies , pCookie->name );
		strcat( m_cookies , "=");
		strcat( m_cookies , pCookie->value );
		strcat( m_cookies , "\r\n" );
	}
}

VO_BOOL vo_http_downloader::analyze_response()
{
	if( m_dlna_param && m_dlna_param->is_dlna() )
	{
		return dlna_analyze_response();
	}
	else
	{
		return normal_analyze_response();
	}
}

VO_BOOL vo_http_downloader::normal_analyze_response()
{
	VOLOGE("%s" , m_response );

	m_status_code = StatusCode();

	if( m_status_code == 401 || m_status_code == 407 )
	{
		//we should process authentication info here
		m_authorization.process_challenge_info( m_response );
	}

	if( !IsChunked() )
	{
		if(IsValidContentLength())
		{
			m_filesize = m_content_length;
		}
	}

	//it is not reliable to check range base on the field
//	AcceptRange();

	MetadataInt();

	ContentType();

	Duration();

	if( !m_is_ignore_cookie_setting )
	{
		m_http_cookie.Reset();
		m_http_cookie.set_urlinfo( m_host , m_path , m_port );
		//we should update cookie info and use the new cookie info in time
		if( m_http_cookie.process_cookie( m_response ) )
		{
			generate_cookie_str();
		}
	}

	if( m_status_code == 301 || 
	    m_status_code == 302 ||
	    m_status_code == 303 ||
	    m_status_code == 307 )
	{
		Relocation();
	}

	ContentRange();

 	if( m_filesize <= 0 )
 	{
 		m_content_length = -1;
 		m_filesize = -1;
 	}


	if( m_status_code / 100 == 2 )
	{
		//if protocol is not http 1.1 ,then range request feature is not support.
		//if( m_protocol_ver != 11)
		//{
		//	m_is_server_canseek = VO_FALSE;
		//}

		//it is stupid when we request whole content and server feed back partial content
		if(m_start_download_pos == 0 && m_status_code == 206)
		{
			//we won't trust the content lenth provided by server
			m_content_length = m_filesize;
		}
		
		//when the mime is not a/v, we should not trust the size info
		//if( strlen(m_mimetype) > 0 && strstr(m_mimetype , "text/html") )
		//{
		//	m_content_length = -1;
		//	m_filesize = -1;
		//}

		m_ptr_callback->download_notify( DOWNLOAD_FILESIZE , &m_filesize );
	}

	//if not support range request, then current download physical pos should be 0.
	if(!m_is_server_canseek)
	{
		m_current_physical_pos = 0;
		m_content_length = m_filesize;
	}
	else
	{
		m_current_physical_pos  = m_start_download_pos;
	}

	if ( strncstr( m_response ,(char *) "com.microsoft.wm." ) )
	{
		VO_LIVESRC_FORMATTYPE livesrc_ft = VO_LIVESRC_FT_WMS; 
		m_ptr_callback->download_notify( DOWNLOAD_LIVESRC_TYPE, &livesrc_ft );
	}

	return VO_TRUE;
}

VO_BOOL vo_http_downloader::dlna_analyze_response()
{
	VOLOGI("%s" , m_response );

	m_status_code = StatusCode();

	if( !IsChunked() )
	{
		if(IsValidContentLength())
		{
			m_filesize = m_content_length;
		}
	}

//	AcceptRange();

	ContentType();

	Duration();

	if( m_status_code == 301 || 
	    m_status_code == 302 ||
	    m_status_code == 303 ||
	    m_status_code == 307 )
	{
		Relocation();
	}

	ContentRange();

	if( m_dlna_param && m_dlna_param->is_range_limit() )
	{
		AvailableSeekRange();
	}

 	if( m_filesize <= 0 )
 	{
 		m_content_length = -1;
 		m_filesize = -1;
 	}


	if( m_status_code / 100 == 2 )
	{
		//if protocol is not http 1.1 ,then range request feature is not support.
		//if( m_protocol_ver != 11)
		//{
		//	m_is_server_canseek = VO_FALSE;
		//}

		//it is stupid when we request whole content and server feed back partial content
		if(m_start_download_pos == 0 && m_status_code == 206)
		{
			//we won't trust the content lenth provided by server
			m_content_length = m_filesize;
		}
		
		m_ptr_callback->download_notify( DOWNLOAD_FILESIZE , &m_filesize );
	}

	//if not support range request, then current download physical pos should be 0.
	if(!m_is_server_canseek)
	{
		m_current_physical_pos = 0;
		m_content_length = m_filesize;
	}
	else
	{
		m_current_physical_pos  = m_start_download_pos;
	}

	return VO_TRUE;
}

VO_BOOL vo_http_downloader::CheckSeekFeature(VO_CHAR * buffer, VO_S32 ret)
{
	if(m_check_seek_feature && m_pCompdata != 0)
	{
		VOLOGI("check if server support range! m_nComplen: %d , ret: %d ", m_nComplen , ret );

		VO_S32 i=0;
		VO_S32 nlen = (m_nComplen >= ret ) ? ret : m_nComplen;
		do
		{
			if(m_pCompdata[i] != (VO_BYTE)buffer[i])
			{
				break;
			}
			i++;
		}while(i < nlen);

		VOLOGI("i : %d nlen: %d" , i , nlen);

		//if all compared data is same, then server not support range request
		if(i >= nlen && nlen >= 256)
		{
			VOLOGI( "Compare %d data and found server not support range! ", nlen );
			m_is_server_canseek = VO_FALSE;

			//server did not support range request, so initialize value to original
			m_current_physical_pos = 0;
			m_start_download_pos = 0;
			return VO_FALSE;
		}

		m_check_seek_feature = VO_FALSE;
	}
	return VO_TRUE;
}

VO_VOID vo_http_downloader::download_normal()
{
	if( m_dlna_param && m_dlna_param->is_dlna() )
	{
		return dlna_download_normal();
	}
	else
	{
		return normal_download_normal();
	}
}

VO_VOID vo_http_downloader::normal_download_normal()
{
	m_downloaded = 0;

	VO_U32 lastfile_time;
	VO_BOOL	bexit_download = VO_FALSE;

	//for shoutcast metadata processing
	memset( &m_shoutcastmeta , 0 , sizeof(m_shoutcastmeta) );

	while( m_downloaded != m_content_length && !m_stop_download && !m_is_to_exit)
	{
		VO_S32 downloadsize = (VO_S32)(( m_content_length - m_downloaded ) < MAXGETSIZE ? ( m_content_length - m_downloaded ) : MAXGETSIZE);
		VO_CHAR buffer[MAXGETSIZE];

		VO_S32 ret = m_ptr_persist->Read( (VO_PBYTE)buffer , downloadsize );
		if( ret == -1 )
		{
			VOLOGE("failed to recv data, socket error");
			m_ptr_persist->Close();
			set_net_error_type(E_PD_DOWNLOAD_FAILED);
			break;
		}
		else if( ret == 0 )
		{
			VOLOGI("exit download as required");
			m_ptr_persist->Close();
			break;
		}
		else
		{
			lastfile_time = voOS_GetSysTime();
			m_downloaded = m_downloaded + ret;

			if( m_metadata_interval )
			{
				if( !ProcessShoutcastMetadata(buffer , ret) )
				{
					//we need to read more data before sent it to memory.
					continue;
				}
			}
		}

		//check if server support range feature
		if(!CheckSeekFeature( buffer , ret ))
		{
			m_content_length = m_filesize;
		}


		
		if( m_filesize > 0 && ( m_current_physical_pos + ret > m_filesize ) )
		{
			//it means the file size info is wrong, so we need to protect from out of range memory access.
			VOLOGI("download out of range ,m_current_physical_pos : %lld , ret : %d" , m_current_physical_pos , ret);
			ret = (VO_S32)(m_filesize - m_current_physical_pos);
			bexit_download = VO_TRUE;
			if( ret < 0 )
			{
				break;
			}
		}

		if( ret <= MAXGETSIZE )
		{
			if( CALLBACK_BUFFER_FULL == m_ptr_callback->received_data( m_current_physical_pos , (VO_PBYTE)buffer , ret ) )
			{
				m_is_update_buffer = VO_TRUE;
				VOLOGE("Buffer Full!" );
				break;
			}
		}
		else
		{
			//the received data size maybe larger than MAXGETSIZE, since it may compose of several packets ,as a result of processing shoutcast metadata.
			VO_U32 rsize = ret;
			VO_BOOL bBufferful = VO_FALSE;
			while( rsize > 0 )
			{
				VO_U32 nsnd = rsize > MAXGETSIZE ? MAXGETSIZE : rsize;
				memcpy( buffer , m_shoutcastmeta.tmpbuff + ret - rsize, nsnd );
				if( CALLBACK_BUFFER_FULL == m_ptr_callback->received_data( m_current_physical_pos , (VO_PBYTE)buffer , nsnd ) )
				{
					bBufferful = VO_TRUE;
					m_is_update_buffer = VO_TRUE;
					VOLOGE( "Buffer Full!" );
					break;
				}
				rsize -= nsnd;
			}

			if(bBufferful)
			{
				break;
			}
		}
		
		m_is_update_buffer = VO_TRUE;

		m_current_physical_pos += ret;

		if( m_slow_down > 0 )
		{
			VOLOGE("slow_down download speed" );
			voOS_Sleep( m_slow_down );
		}
				
		if( bexit_download )
		{
			return;
		}
	}
}

VO_VOID vo_http_downloader::dlna_download_normal()
{
	m_downloaded = 0;

	VO_U32 lastfile_time;
	VO_BOOL	bexit_download = VO_FALSE;

	VO_U32	readtimeout_loop = 0;
	if(m_dlna_param && m_dlna_param->get_read_timeout() > 0 )
	{
		readtimeout_loop = m_dlna_param->get_read_timeout() / 20;
	}

	while( !m_stop_download && !m_is_to_exit)
	{
		while( m_dlna_param && m_dlna_param->is_connection_stalling_on_pause() && m_is_pause_connection && !m_stop_download && !m_is_to_exit)
		{
			VOLOGI( "connection stalling on pause................" );
			voOS_Sleep(20);
		}

		VO_S32 downloadsize = (VO_S32)(( m_content_length - m_downloaded ) < MAXGETSIZE ? ( m_content_length - m_downloaded ) : MAXGETSIZE);
		VO_CHAR buffer[MAXGETSIZE];

		VO_S32 ret = m_ptr_persist->Read( (VO_PBYTE)buffer , downloadsize );
		if( ret == -1 )
		{
			VOLOGE("failed to recv data, socket error");
			m_ptr_persist->Close();
			set_net_error_type(E_PD_DOWNLOAD_FAILED);
			break;
		}
		else if( ret == 0 )
		{
			VOLOGI("exit download as required");
			m_ptr_persist->Close();
			break;
		}
		else
		{
			lastfile_time = voOS_GetSysTime();
			m_downloaded = m_downloaded + ret;
		}

		//check if server support range feature
		if(!CheckSeekFeature( buffer , ret ))
		{
			m_content_length = m_filesize;
		}
		
		if( m_filesize > 0 && ( m_current_physical_pos + ret > m_filesize ) )
		{
			//it means the file size info is wrong, so we need to protect from out of range memory access.
			VOLOGI( "download out of range ,m_current_physical_pos : %lld , ret : %d" , m_current_physical_pos , ret);
			ret = (VO_S32)(m_filesize - m_current_physical_pos);
			bexit_download = VO_TRUE;
			if( ret < 0 )
			{
				break;
			}
		}

		if( CALLBACK_BUFFER_FULL == m_ptr_callback->received_data( m_current_physical_pos , (VO_PBYTE)buffer , ret ) )
		{
			m_is_update_buffer = VO_TRUE;
			VOLOGI( "Buffer Full!" );
			break;
		}
		

		m_is_update_buffer = VO_TRUE;

		m_current_physical_pos += ret;

		if( m_slow_down )
		{
			//avoid  buffer full in normal playback.
			if( m_slow_down == 100 && m_dlna_param && m_dlna_param->is_connection_stalling_on_play())
			{
				VOLOGI( "slow_down download speed :%d ms" , m_slow_down * 20 );
				voOS_Sleep( m_slow_down * 20 );
			}
			else
			{
				VOLOGI( "slow_down download speed :%d ms" , m_slow_down );
				voOS_Sleep( m_slow_down );
			}
		}

		//if we have finish download, then exit download thread.
		if(m_downloaded >= m_content_length)
		{
			while( m_dlna_param && m_dlna_param->is_connection_stalling_on_play() && !m_stop_download && !m_is_to_exit)
			{
				VOLOGI( "connection stalling on play................" );
				voOS_Sleep(20);
			}
			break;
		}

		if( bexit_download )
		{
			return;
		}
	}
}

//this function following the spec from here:
//http://www.smackfu.com/stuff/programming/shoutcast.html
//stage1: the metadata interval has been reached--->so we can get the metadata length.
//stage2: the total metadata is reached-->so we can extract the metadata, 
//and reorganize the media data buffer which will exclude the metadata.
VO_BOOL vo_http_downloader::ProcessShoutcastMetadata(VO_CHAR* buffer, VO_S32& ret )
{
	if( m_metadata_interval < (m_shoutcastmeta.metacnt + ret) )
	{
		//if the count has alread reached the interval of media data and metadata
		if( m_shoutcastmeta.metalen )
		{
			//if the metalen has already got , then we won't get it again from buffer.
			//it means we are buffering data to get metadata.
		}
		else
		{
			m_shoutcastmeta.metalen = ((VO_U32)buffer[m_metadata_interval - m_shoutcastmeta.metacnt]) * 16;
		}

		if( m_shoutcastmeta.metacnt + ret >= m_metadata_interval + m_shoutcastmeta.metalen + 1 )
		{
			//all metadata is included in the current buffer
			memset( m_shoutcastmeta.tmpbuff , 0 , sizeof(m_shoutcastmeta.tmpbuff) );

			if( m_shoutcastmeta.copycnt )
			{
				//it is composed by several packets
				memcpy( m_shoutcastmeta.buffer2 + m_shoutcastmeta.copycnt ,buffer , ret );
				m_shoutcastmeta.copycnt += ret;
				//1,copy the metadata
				memcpy( m_metadata , m_shoutcastmeta.buffer2 + ( m_shoutcastmeta.left_pos + 1 ) , m_shoutcastmeta.metalen );
				//2,copy media data and remove unnecessary metadata
				memcpy( m_shoutcastmeta.tmpbuff , m_shoutcastmeta.buffer2 , m_shoutcastmeta.left_pos );
				memcpy( m_shoutcastmeta.tmpbuff + m_shoutcastmeta.left_pos  , m_shoutcastmeta.buffer2 + m_shoutcastmeta.left_pos + m_shoutcastmeta.metalen + 1 , m_shoutcastmeta.copycnt - ( m_shoutcastmeta.left_pos + m_shoutcastmeta.metalen + 1 ));
				if( m_shoutcastmeta.copycnt  - ( m_shoutcastmeta.metalen + 1 ) <= MAXGETSIZE )
				{
					memset( buffer , 0 , MAXGETSIZE );
					memcpy( buffer , m_shoutcastmeta.tmpbuff , m_shoutcastmeta.copycnt  - ( m_shoutcastmeta.metalen + 1 ));
					ret = m_shoutcastmeta.copycnt  - ( m_shoutcastmeta.metalen + 1 );
				}
				else
				{
					ret = m_shoutcastmeta.copycnt  - ( m_shoutcastmeta.metalen + 1 );
				}
				//3,reset the media data count
				m_shoutcastmeta.metacnt = m_shoutcastmeta.copycnt - ( m_shoutcastmeta.left_pos + m_shoutcastmeta.metalen + 1 );
				//4,we will only send the media data to local media data buffer, so recalculate the size
				m_shoutcastmeta.copycnt = 0;
				m_shoutcastmeta.metalen = 0;
			}
			else
			{
				//it is composed only one packet
				//1,copy the metadata
				memcpy( m_metadata , buffer + ( m_metadata_interval - m_shoutcastmeta.metacnt + 1 ) , m_shoutcastmeta.metalen );
				//2,copy media data and remove unnecessary metadata
				memcpy( m_shoutcastmeta.tmpbuff , buffer , ( m_metadata_interval - m_shoutcastmeta.metacnt ) );
				memcpy( m_shoutcastmeta.tmpbuff + ( m_metadata_interval - m_shoutcastmeta.metacnt ) , buffer + ( m_metadata_interval - m_shoutcastmeta.metacnt ) + m_shoutcastmeta.metalen + 1 , ret - ( m_metadata_interval - m_shoutcastmeta.metacnt ) - ( m_shoutcastmeta.metalen + 1 ));
				memset( buffer , 0 , MAXGETSIZE );
				memcpy( buffer , m_shoutcastmeta.tmpbuff , ret  - ( m_shoutcastmeta.metalen + 1 ));
				//3,reset the media data count
				m_shoutcastmeta.metacnt = ret - ( m_metadata_interval - m_shoutcastmeta.metacnt + m_shoutcastmeta.metalen + 1);
				//4,we will only send the media data to local media data buffer, so recalculate the size
				ret = ret  - ( m_shoutcastmeta.metalen + 1 );
				m_shoutcastmeta.metalen = 0;
			}

			m_ptr_callback->download_notify( DOWNLOAD_SHOUTCAST_METADATA , &m_metadata );
		}
		else
		{
			//partial metadata is included in the current buffer
			if( m_shoutcastmeta.metacnt <= m_metadata_interval )
			{
				//record the left start position of metadata
				m_shoutcastmeta.left_pos = m_metadata_interval - m_shoutcastmeta.metacnt;
			}
			memcpy( m_shoutcastmeta.buffer2 + m_shoutcastmeta.copycnt ,buffer , ret );
			m_shoutcastmeta.copycnt += ret;
			m_shoutcastmeta.metacnt += ret;
			return VO_FALSE;
		}
	}
	else
	{
		m_shoutcastmeta.metacnt += ret; 
	}

	return VO_TRUE;
}

VO_VOID vo_http_downloader::download_endless()
{
	if( m_dlna_param && m_dlna_param->is_dlna() )
	{
		return dlna_download_endless();
	}
	else
	{
		return normal_download_endless();
	}
}

VO_VOID vo_http_downloader::normal_download_endless()
{
	m_downloaded = 0;

	VO_U32 lastfile_time;

	//for shoutcast metadata processing
	memset( &m_shoutcastmeta , 0 , sizeof(m_shoutcastmeta) );

	while( !m_stop_download  && !m_is_to_exit)
	{
		VO_CHAR buffer[MAXGETSIZE];
		memset( buffer , 0 , sizeof(buffer) );

		VO_S32 ret = m_ptr_persist->Read( (VO_PBYTE)buffer , MAXGETSIZE );
		if( ret == -1 )
		{
			VOLOGI("failed to recv data");
			m_ptr_persist->Close();
			set_net_error_type(E_PD_DOWNLOAD_FAILED);
			break;
		}
		else if( ret == 0 )
		{
			VOLOGI("exit download as required");
			m_ptr_persist->Close();
			break;
		}
		else
		{
			lastfile_time = voOS_GetSysTime();
			m_downloaded = m_downloaded + ret;
						
			if( m_metadata_interval )
			{
				if( !ProcessShoutcastMetadata(buffer , ret) )
				{
					continue;
				}
			}
		}

		//check if server support feature
		CheckSeekFeature(buffer,ret);

		if( ret <= MAXGETSIZE )
		{
			if( CALLBACK_BUFFER_FULL == m_ptr_callback->received_data( m_current_physical_pos , (VO_PBYTE)buffer , ret ) )
			{
				m_is_update_buffer = VO_TRUE;
				VOLOGE("Buffer Full!" );
				break;
			}
		}
		else
		{
			//the received data size maybe large than MAXGETSIZE, since it may compose of several packets ,as a result of processing shoutcast metadata.
			VO_U32 rsize = ret;
			VO_BOOL bBufferful = VO_FALSE;
			while( rsize > 0 )
			{
				VO_U32 nsnd = rsize > MAXGETSIZE ? MAXGETSIZE : rsize;
				memcpy( buffer , m_shoutcastmeta.tmpbuff + ret - rsize, nsnd );
				if( CALLBACK_BUFFER_FULL == m_ptr_callback->received_data( m_current_physical_pos , (VO_PBYTE)buffer , nsnd ) )
				{
					bBufferful = VO_TRUE;
					m_is_update_buffer = VO_TRUE;
					VOLOGE( "Buffer Full!" );
					break;
				}
				rsize -= nsnd;
			}

			if(bBufferful)
			{
				break;
			}
		}

		m_is_update_buffer = VO_TRUE;

		m_current_physical_pos += ret;

		if( m_slow_down > 0 )
		{
			VOLOGE( "slow_down download speed" );
			voOS_Sleep( m_slow_down );
		}
	}

	if( !m_stop_download )
		m_ptr_callback->download_notify( DOWNLOAD_TO_FILEEND , (VO_PTR)&m_current_physical_pos );
}

VO_VOID vo_http_downloader::dlna_download_endless()
{
	m_downloaded = 0;

	VO_U32	readtimeout_loop = 0;
	if(m_dlna_param && m_dlna_param->get_read_timeout() > 0 )
	{
		readtimeout_loop = m_dlna_param->get_read_timeout() / 20;
	}

	VO_U32 lastfile_time;

	while( !m_stop_download  && !m_is_to_exit)
	{
		while( m_dlna_param && m_dlna_param->is_connection_stalling_on_pause() && m_is_pause_connection && !m_stop_download && !m_is_to_exit)
		{
			VOLOGI( "connection stalling on pause................" );
			voOS_Sleep(20);
		}

		VO_CHAR buffer[MAXGETSIZE];

		VO_S32 ret = m_ptr_persist->Read( (VO_PBYTE)buffer , MAXGETSIZE );
		if( ret == -1 )
		{
			VOLOGI("failed to recv data");
			m_ptr_persist->Close();
			set_net_error_type(E_PD_DOWNLOAD_FAILED);
			break;
		}
		else if( ret == 0 )
		{
			VOLOGI("exit download as required");
			break;
		}
		else
		{
			lastfile_time = voOS_GetSysTime();
			m_downloaded = m_downloaded + ret;
		}

		//check if server support feature
		CheckSeekFeature( buffer , ret );

		if( CALLBACK_BUFFER_FULL == m_ptr_callback->received_data( m_current_physical_pos , (VO_PBYTE)buffer , ret ) )
		{
			m_is_update_buffer = VO_TRUE;
			VOLOGE( "Buffer Full!" );
			break;
		}

		m_is_update_buffer = VO_TRUE;

		m_current_physical_pos += ret;

		if( m_slow_down )
		{
			//PRINT_LOG( LOG_LEVEL_IMPORTANT , "slow_down download speed:%d" , m_slow_down );
			voOS_Sleep( m_slow_down );
		}
	}

	if( !m_stop_download )
		m_ptr_callback->download_notify( DOWNLOAD_TO_FILEEND , (VO_PTR)&m_current_physical_pos );
}

VO_VOID vo_http_downloader::download_chunked()
{
	if( m_dlna_param && m_dlna_param->is_dlna() )
	{
		return dlna_download_chunked();
	}
	else
	{
		return normal_download_chunked();
	}
}

VO_VOID vo_http_downloader::normal_download_chunked()
{
	VO_S32 chunk_size = 0;
	VO_S32 ret = 0;

	VO_U32 lastfile_time;
	VO_BOOL isbufferfull = VO_FALSE;

	do
	{
		VO_CHAR buffer[MAXGETSIZE];
		memset( buffer , 0 , MAXGETSIZE );
		VO_S32 pos = 0;

		while( 1 && pos < MAXGETSIZE )
		{
			ret = m_ptr_persist->Read( (VO_PBYTE)buffer + pos , 1 );
			if( ret == -1 )
			{
				VOLOGE("failed to recv data, socket error");
				m_ptr_persist->Close();
				set_net_error_type(E_PD_DOWNLOAD_FAILED);
				break;
			}
			else if( ret == 0 )
			{
				VOLOGI("exit download as required");
				m_ptr_persist->Close();
				break;
			}

			if( pos >= 1 && buffer[pos] == '\n' && buffer[pos - 1] == '\r' )
			{
				buffer[pos - 1] = '\0';
				break;
			}

			pos++;
		}

		sscanf( (VO_CHAR*)buffer , (char *)"%x" , (unsigned int*)&chunk_size );

		VOLOGE("Chunked Size: %ld" , chunk_size  );

		VO_S32 buffertoread = chunk_size;

		while( buffertoread && !m_stop_download && !m_is_to_exit )
		{
			VO_S32 toread = buffertoread < MAXGETSIZE ? buffertoread : MAXGETSIZE;

			ret = m_ptr_persist->Read( (VO_PBYTE)buffer , toread );
			if( ret == -1 )
			{
				VOLOGE("failed to recv data, socket error");
				m_ptr_persist->Close();
				set_net_error_type(E_PD_DOWNLOAD_FAILED);
				break;
			}
			else if( ret == 0 )
			{
				VOLOGI("exit download as required");
				m_ptr_persist->Close();
				break;
			}
			else
			{
				lastfile_time = voOS_GetSysTime();
				buffertoread = buffertoread - ret;
			}

			//check if server support feature
			CheckSeekFeature( buffer, ret );

			if( CALLBACK_BUFFER_FULL == m_ptr_callback->received_data( m_current_physical_pos , (VO_PBYTE)buffer , ret ) )
			{
				m_is_update_buffer = VO_TRUE;
				isbufferfull = VO_TRUE;
				VOLOGE( "Buffer Full!" );
				return;
			}

			m_is_update_buffer = VO_TRUE;

			m_current_physical_pos += ret;

			if( m_slow_down > 0 )
			{
				VOLOGE( "slow_down download speed" );
				voOS_Sleep( m_slow_down );
			}
		}

		if( !m_stop_download && !m_is_to_exit )
		{
			ret = m_ptr_persist->Read( (VO_PBYTE)buffer , 2 );
			if( ret == -1 )
			{
				VOLOGE("failed to recv data, socket error");
				m_ptr_persist->Close();
				set_net_error_type(E_PD_DOWNLOAD_FAILED);
				break;
			}
			else if( ret == 0 )
			{
				VOLOGI("exit download as required");
				m_ptr_persist->Close();
				break;
			}
		}

	} while ( chunk_size && !m_stop_download  && !m_is_to_exit);

	if( !m_stop_download )
		m_ptr_callback->download_notify( DOWNLOAD_TO_FILEEND , (VO_PTR)&m_current_physical_pos );
}

VO_VOID vo_http_downloader::dlna_download_chunked()
{
	VO_S32 chunk_size = 0;

	VO_U32 lastfile_time;
	VO_BOOL isbufferfull = VO_FALSE;

	VO_S32 ret = 0;
		
	VO_U32	readtimeout_loop = 0;
	if(m_dlna_param && m_dlna_param->get_read_timeout() > 0 )
	{
		readtimeout_loop = m_dlna_param->get_read_timeout() / 20;
	}

	do
	{
		VO_CHAR buffer[MAXGETSIZE];
		memset( buffer , 0 , MAXGETSIZE );
		VO_S32 pos = 0;

		while( 1 && pos < MAXGETSIZE )
		{
			ret = m_ptr_persist->Read( (VO_PBYTE)buffer + pos , 1 );
			if( ret == -1 )
			{
				VOLOGE("failed to recv data, socket error");
				m_ptr_persist->Close();
				set_net_error_type(E_PD_DOWNLOAD_FAILED);
				break;
			}
			else if( ret == 0 )
			{
				VOLOGI("exit download as required");
				break;
			}

			if( pos >= 1 && buffer[pos] == '\n' && buffer[pos - 1] == '\r' )
			{
				buffer[pos - 1] = '\0';
				break;
			}

			pos++;
		}

		sscanf( (VO_CHAR*)buffer , "%x" , (unsigned int *)&chunk_size );

		VOLOGI( "Chunked Size: %d" , chunk_size  );

		VO_S32 buffertoread = chunk_size;

		while( buffertoread && !m_stop_download && !m_is_to_exit )
		{
			while( m_dlna_param && m_dlna_param->is_connection_stalling_on_pause() && m_is_pause_connection && !m_stop_download && !m_is_to_exit)
			{
				VOLOGI( "connection stalling on pause................" );
				voOS_Sleep(20);
			}

			VO_S32 toread = buffertoread < MAXGETSIZE ? buffertoread : MAXGETSIZE;

			ret = m_ptr_persist->Read( (VO_PBYTE)buffer , toread );
			if( ret == -1 )
			{
				VOLOGE("failed to recv data, socket error");
				m_ptr_persist->Close();
				set_net_error_type(E_PD_DOWNLOAD_FAILED);
				break;
			}
			else if( ret == 0 )
			{
				VOLOGI("exit download as required");
				break;
			}
			else
			{
				lastfile_time = voOS_GetSysTime();
				buffertoread = buffertoread - ret;
			}

			//check if server support feature
			CheckSeekFeature( buffer , ret );

			if( CALLBACK_BUFFER_FULL == m_ptr_callback->received_data( m_current_physical_pos , (VO_PBYTE)buffer , ret ) )
			{
				m_is_update_buffer = VO_TRUE;
				isbufferfull = VO_TRUE;
				VOLOGE( "Buffer Full!" );
				return;
			}

			m_is_update_buffer = VO_TRUE;

			m_current_physical_pos += ret;

			if( m_slow_down )
			{
				//PRINT_LOG( LOG_LEVEL_IMPORTANT , "slow_down download speed:%d" , m_slow_down );
				voOS_Sleep( m_slow_down );
			}
		}

		if( !m_stop_download && !m_is_to_exit )
		{
			ret = m_ptr_persist->Read( (VO_PBYTE)buffer , 2 );
			if( ret == -1 )
			{
				VOLOGE("failed to recv data, socket error");
				m_ptr_persist->Close();
				set_net_error_type(E_PD_DOWNLOAD_FAILED);
				break;
			}
			else if( ret == 0 )
			{
				VOLOGI("exit download as required");
				break;
			}
		}

		//if we have finish download, then exit download thread.
		if( chunk_size == 0 )
		{
			while( m_dlna_param && m_dlna_param->is_connection_stalling_on_play() && !m_stop_download  && !m_is_to_exit)
			{
				VOLOGI( "connection stalling on play................" );
				voOS_Sleep(20);
			}
			break;
		}

	} while ( !m_stop_download  && !m_is_to_exit);

	if( !m_stop_download )
		m_ptr_callback->download_notify( DOWNLOAD_TO_FILEEND , (VO_PTR)&m_current_physical_pos );
}

VO_BOOL vo_http_downloader::is_downloading()
{
	if( !m_ptr_persist )
	{
		return VO_FALSE;
	}

	if( m_download_thread_processing )
	{
		return VO_TRUE;
	}

	return m_ptr_persist->socket == -1 ? VO_FALSE : VO_TRUE ;
}

VO_VOID vo_http_downloader::set_to_close(VO_BOOL is_to_exit)
{ 
	VOLOGE( "[moses] notify exit to downloader");
	m_is_to_exit = is_to_exit; 
}

VO_VOID vo_http_downloader::get_dowloadinfo( VO_DOWNLOAD_INFO * ptr_info )
{
	VO_U32 timelost = voOS_GetSysTime() - m_download_start_time;
	if( timelost == 0 )
		timelost = 1;

	ptr_info->cur_download_pos = m_current_physical_pos;
	ptr_info->start_download_pos = m_start_download_pos;
	ptr_info->download_size = m_content_length;

	VO_S64 delta_pos = m_current_physical_pos - m_start_download_pos;

	if( timelost )
		ptr_info->average_speed = (VO_S64)(( m_current_physical_pos - m_start_download_pos ) * 1000. / timelost);
	else
		ptr_info->average_speed = 0;

	if( delta_pos < 10240 || timelost < 1000 )
		ptr_info->is_speed_reliable = VO_FALSE;
	else
		ptr_info->is_speed_reliable = VO_TRUE;

	if( ptr_info->average_speed <= 5000 )
		ptr_info->is_speed_reliable = VO_FALSE;

	ptr_info->is_update_buffer = m_is_update_buffer;

	//show the download speed if it is reliable
	if(ptr_info->is_speed_reliable)
	{
		VOLOGE("Download_Speed: %d KBps" , (VO_S32)(ptr_info->average_speed/1024.));
	}
}

VO_VOID vo_http_downloader::analyze_proxy()
{
	m_use_proxy = VO_FALSE;
	memset( m_proxy_port , 0 , sizeof( m_proxy_port ) );
	memset( m_proxy_host , 0 , sizeof( m_proxy_host ) );

	if( strlen( (char *)m_ptr_PD_param->mProxyName ) )
	{
		VOLOGI("Using Proxy: %s" , m_ptr_PD_param->mProxyName );

		m_use_proxy = VO_TRUE;

		VO_CHAR proxy[1024];
		memset( proxy , 0 , 1024 );
		strcpy( proxy , (char *)m_ptr_PD_param->mProxyName );
		VO_CHAR * ptr_port = strchr( proxy , ':' );

		if( ptr_port )
		{
			*ptr_port = '\0';
			ptr_port++;

			strcpy( m_proxy_port , ptr_port );
		}
		else
		{
			strcpy( m_proxy_port , "80" );
		}

		strcpy( m_proxy_host , proxy );
	}
}

VO_VOID vo_http_downloader::urlencode()
{
	char * ptr_in = m_path;
	char temp[2048];

	memset( temp , 0 , sizeof( temp ) );

	char * ptr_out = temp;

	while (*ptr_in)
	{
		if ( ('0' <= *ptr_in && *ptr_in <= '9') ||//0-9
			('a' <= *ptr_in && *ptr_in <= 'z') ||//abc...xyz
			('A' <= *ptr_in && *ptr_in <= 'Z') || //ABC...XYZ
			( /**ptr_in =='~' || *ptr_in =='!' || *ptr_in =='*' || *ptr_in =='(' || *ptr_in ==')' || *ptr_in =='\'' ||*/ *ptr_in =='.' || *ptr_in =='/' /*|| *ptr_in =='_'*/ )
			)
		{
			*ptr_out++ = *ptr_in;
		}
		else
		{
			*ptr_out++ = '%';
			*ptr_out++ = toHex( ((*ptr_in)&0xF0)>>4 );
			*ptr_out++ = toHex( (*ptr_in)&0x0F );
		}

		ptr_in++;
	}
	*ptr_out = 0;

	memset( m_path , 0 , sizeof(m_path) );

	strcpy( m_path , temp );
}

unsigned char vo_http_downloader::toHex(const unsigned char x)
{
	unsigned char ret = x;
	if ( 0<= x && x<= 9) ret+=48;    //0,48inascii
	if (10<= x && x<=15) ret+=97-10; //a,97inascii

	if( ret >= 'a' && ret <= 'z'  )
		ret += 'A' - 'a';

	return ret;
}

VO_VOID vo_http_downloader::pause_connection(VO_BOOL recvpause)
{
	VOLOGI("pause_connection! %d" , recvpause);
	m_is_pause_connection = recvpause;
}

VO_VOID vo_http_downloader::set_workpath(VO_TCHAR *	strWorkPath)
{
	VOLOGI("set workpath! %s" , strWorkPath);
	m_pstrWorkPath = strWorkPath;

	if( m_bhttps && m_pstrWorkPath )
	{
		m_sslload.SetWorkpath( m_pstrWorkPath );
		if( !m_sslload.LoadSSL() )
		{
			VOLOGE( "load ssl lib Failed" );
			return;
		}
		else
		{
			setsslapi( m_sslload.m_sslapi);
		}
	}
}

VO_VOID vo_http_downloader::setsslapi( vosslapi ssl)
{ 
	if( m_bhttps )
	{
		if( m_ptr_persist )
		{
			((Persist_HTTPS*)m_ptr_persist)->setsslapi(ssl);
		}
		else
		{
			VOLOGI("set ssl api failed!");
		}
	}
	else
	{
		;
	}
}

VO_VOID vo_http_downloader::set_connectretry_times(VO_S32 retrytimes)
{
	VOLOGI("set connect retry times! %d" , retrytimes);
	if( retrytimes < 0 )
	{
		m_connectretry_times = 0x7fffffff;
	}
	else
	{
		m_connectretry_times = retrytimes;
	}
}

VO_VOID vo_http_downloader::set_dlna_param(VO_VOID* pobj)
{
	m_dlna_param = (voDLNA_Param*)pobj;
}

VO_U32 vo_http_downloader::generate_tunnel_requestHTTP( VO_CHAR * ptr_request )
{
	return 0;
}

VO_U32 vo_http_downloader::generate_tunnel_requestHTTPS( VO_CHAR * ptr_request )
{
	VO_U32 size = 0;
	char * request_template;
	if( m_ptr_persist )
	{
		request_template = (char *)"CONNECT %s:%s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\n";
	}
	else
	{
		request_template = (char *)"CONNECT %s:%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n";
	}
	size = sprintf( ptr_request , request_template , m_host , m_port , m_host);
	
	if( strlen( m_cookies ) )
	{
		strcat( ptr_request , m_cookies );
		size = size + strlen( m_cookies );
	}

	if( m_bauth )
	{
		strcat( ptr_request , m_authinfo );
		strcat( ptr_request , "\r\n" );
		size = size + strlen( m_authinfo ) + strlen( "\r\n" );
	}

	VO_S32 nagentlen = strlen( (const char *)m_ptr_PD_param->mUserAgent );
	if( nagentlen > 0 )
	{
		strcat( ptr_request ,(char *) "User-Agent: " );
		size = size + strlen( "User-Agent: " );
		strcat( ptr_request , (const char *)m_ptr_PD_param->mUserAgent );
		size = size + strlen( (const char *)m_ptr_PD_param->mUserAgent );
		if( m_ptr_PD_param->mUserAgent[nagentlen-1] != '\n' ||  m_ptr_PD_param->mUserAgent[nagentlen-2] != '\r' )
		{
			strcat( ptr_request , "\r\n" );
			size = size + strlen( "\r\n" );
		}
	}
	else
	{
		//defalut user agent info
#if defined (_IOS) || defined (_MAC_OS)
		strcat( ptr_request ,(char *) "User-Agent: VisualOn OSMP+ Player(iOS)\r\n" );
		size = size + strlen( "User-Agent: VisualOn OSMP+ Player(iOS)\r\n" );
#elif defined (_LINUX_ANDROID) || defined(LINUX)
		strcat( ptr_request ,(char *) "User-Agent: VisualOn OSMP+ Player(Linux;Android)\r\n" );
		size = size + strlen( "User-Agent: VisualOn OSMP+ Player(Linux;Android)\r\n" );
		//strcat( ptr_request ,(char *) "User-Agent: stagefright/1.2 (Linux;Android 4.1.1)\r\n" );
		//size = size + strlen( "User-Agent: stagefright/1.2 (Linux;Android 4.1.1)\r\n" );
#elif defined (_WIN32)
		strcat( ptr_request ,(char *) "User-Agent: VisualOn OSMP+ Player(Windows)\r\n" );
		size = size + strlen( "User-Agent: VisualOn OSMP+ Player(Windows)\r\n" );
#endif
	}

	strcat( ptr_request , "\r\n" );

	size = size + strlen( "\r\n" );

	return size;
}

VO_U32 vo_http_downloader::generate_tunnel_request( VO_CHAR * ptr_request )
{
	if( m_bhttps )
	{
		return generate_tunnel_requestHTTPS( ptr_request );
	}
	else
	{
		return generate_tunnel_requestHTTP( ptr_request );
	}
}