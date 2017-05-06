#ifdef WINCE
#include <winsock2.h>
#endif

#include "vo_http_stream.h"
#include "vo_http_utils.h"
#include "voString.h"
#include "voOSFunc.h"
#include "NetWork/vo_socket.h"
#include "voLog.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define MAXGETSIZE (1024*10)
#define IOBUFFERINGSIZE (2)
#define MAXU64 0xffffffffffffffffll
#define DEFAULTMAXDOWNLOADSPEED (10*1024*1024)
#define DEFAULTMINDOWNLOADSPEED (1*1024*1024)

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

	VOLOGI("persist sock instance :0x%08x locked +++++" , this );

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

	VOLOGI("persist sock instance :0x%08x unlocked -----" , this );
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

	VOLOGI( "++++++open_socket:%d" , socket );

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
		VOLOGI( "------close_socket:%d" , socket );
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
		VOLOGI( "------close_socket:%d" , socket );
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

	VOLOGI( "++++++open_socket:%d" , socket );

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

	VOLOGI("connecting....");
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

	VOLOGI("connected");
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
	VOLOGI("....");

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


vo_http_stream::vo_http_stream()
:m_status_code(-1)
,m_content_length(-1)
,m_protocol_ver(11)
,m_range_offset(0)
,m_range_length( MAXU64 )
,m_max_download_speed(DEFAULTMAXDOWNLOADSPEED)
,m_min_download_speed(DEFAULTMINDOWNLOADSPEED)
,m_bslow_down( VO_FALSE )
,m_ptr_persist(0)
,m_is_ignore_cookie_setting(VO_FALSE)
,m_is_cache_redirecturl(VO_FALSE)
,m_bauth(VO_FALSE)
,m_is_url_encoded(VO_FALSE)
,m_nspeed(0)
,m_bclose(VO_FALSE)
,m_reuseoldsocket(VO_FALSE)
,m_downloadcontent_wanted( VO_TRUE )
,m_bAsyncMode(VO_FALSE)
,m_bProxySet(VO_FALSE)
,m_bUserAgentSet(VO_FALSE)
,m_bDnsIpCached(VO_FALSE)
,m_bsslinit(VO_FALSE)
,m_bhttps(VO_FALSE)
{
	memset( m_reparseurl , 0 , sizeof( m_reparseurl ) );
	memset( m_cookies , 0 , sizeof( m_cookies ) );
	memset( m_mimetype , 0 , sizeof( m_mimetype ) );
	memset( m_response , 0 , sizeof(m_response) );
	memset( m_authinfo , 0 , sizeof(m_authinfo) );
	memset( m_username , 0 , sizeof(m_username) );
	memset( m_password , 0 , sizeof(m_password) );
	memset( m_proxyhost , 0 , sizeof( m_proxyhost ) );
	memset( m_proxyport , 0 , sizeof( m_proxyport ) );
	memset( m_useragent , 0 , sizeof( m_useragent ) );
	memset( m_dnscachedip , 0 , sizeof( m_dnscachedip ) );
	m_download2bufferCB.HttpStreamCBFunc = 0;
	m_download2bufferCB.handle = 0;
	m_httpiocb.IO_Callback = 0;
	m_httpiocb.hHandle = 0 ;
	Initdownloadinfo();
	memset( &m_ssl , 0 , sizeof( m_ssl ) );
}

vo_http_stream::~vo_http_stream(void)
{
}

VO_VOID vo_http_stream::initial( VO_CHAR *ptr_url )
{
	memset( m_url , 0 , sizeof(m_url) );

	if( strlen(ptr_url) > sizeof(m_url) )
	{
		VOLOGE("the url length is too long");
	}
	else
	{
		strcpy( m_url , ptr_url );
	}
}


VO_BOOL vo_http_stream::open_internal()
{
	VO_BOOL ret = VO_FALSE;

	ret = run_agent( m_url );

	if( !ret )
	{
		if( m_bclose )
		{
			Notify( 0 , 0 , VO_CONTENT_OPEN_FAILED );
			return VO_FALSE;
		}

		//1,when cookie update and error happen in client side,or
		//2,we use old socket to send request but failed to get response
		//we will retry
		if( ( m_status_code != -1 && m_status_code / 100 == HTTP_STATUS_CLIENT_ERROR && Is_cookie_update() )
			|| ( m_reuseoldsocket && m_status_code == -1 && !m_bclose)
			)
		{
			ret = run_agent( m_url );
			if( !ret )
			{
				Notify( 0 , 0 , VO_CONTENT_OPEN_FAILED );
				return VO_FALSE;
			}
		}
		else
		{
			Notify( 0 , 0 , VO_CONTENT_OPEN_FAILED );
			return VO_FALSE;
		}
	}

	return VO_TRUE;
}

VO_BOOL vo_http_stream::open( VO_BOOL bIsAsyncOpen )
{
	m_bAsyncMode = bIsAsyncOpen;
	m_bclose = VO_FALSE;

	if( m_bAsyncMode )
	{
		//start a new thread to download data.
		begin();
		return VO_TRUE;
	}
	else
	{
		return open_internal();
	}
}

VO_VOID vo_http_stream::close()
{
	VOLOGI("++++http_close");

	m_bclose = VO_TRUE;
	if( m_ptr_persist )
	{
		m_ptr_persist->Close( VO_FALSE );
	}
	stop_download_thread();

	VOLOGI("----http_close");
}

VO_VOID vo_http_stream::setHandle( Persist_HTTP * hanle )
{ 
	//close and free previous persist socket instance, 
	//otherwise the socket will be locked and not be used for ever
	if( m_ptr_persist )
	{
		VOLOGI(" release old persist sock first");
		m_ptr_persist->Close( VO_FALSE );
		m_ptr_persist->unlock();
	}

	m_ptr_persist = hanle;
}

VO_S32 vo_http_stream::IONotify( VO_U32 uID ,  VO_PTR pParam1, VO_PTR pParam2 )
{
	if( m_httpiocb.IO_Callback )
	{
		return (VO_S32)m_httpiocb.IO_Callback( m_httpiocb.hHandle , uID , pParam1 , pParam2 );
	}
	else
	{
		VOLOGE("IO CallBack function pointer is not set yet.");
		return 0;
	}
}

VO_S32 vo_http_stream::Notify( VO_PBYTE param , VO_U64 size , IO_HTTPDATA2BUFFER_DATA_TYPE type )
{
	voCAutoLock lock(&m_lock_w2buff);
	if( m_download2bufferCB.HttpStreamCBFunc )
	{
		return (VO_S32)m_download2bufferCB.HttpStreamCBFunc( m_download2bufferCB.handle , param , size , type );
	}
	return 0;
}

VO_S64 vo_http_stream::append( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	voCAutoLock lock(&m_lock_w2buff);
	if( m_download2bufferCB.HttpStreamCBFunc )
	{
		VO_S64 ret = m_download2bufferCB.HttpStreamCBFunc( m_download2bufferCB.handle , ptr_buffer , buffer_size , VO_CONTENT_DATA );
		return ret;
	}
	return -1;
}

VO_BOOL vo_http_stream::IsPersistSockAvailableHTTP()
{
	if( m_ptr_persist && m_ptr_persist->socket > 0 
		&& m_ptr_persist->usedtimes <  m_ptr_persist->maxusetime
		&& m_ptr_persist->deadtime >  voOS_GetSysTime())
	{
		fd_set fdw;
		fd_set fde;
		timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 20000;

		FD_ZERO(&fdw);
		FD_SET(m_ptr_persist->socket,&fdw);
		FD_ZERO(&fde);
		FD_SET(m_ptr_persist->socket,&fde);

		VO_S32 select_ret = select( m_ptr_persist->socket + 1 ,NULL, &fdw,&fde,&timeout);

		if(FD_ISSET(m_ptr_persist->socket , &fde))
		{
			VOLOGE( "socket exception happen when check persist connection" );
			return VO_FALSE;
		}

		if( select_ret == -1 || select_ret == 0 )
		{
			return VO_FALSE;
		}
		else
		{
			m_ptr_persist->usedtimes++;
			return VO_TRUE;
		}
	}
	else
	{
		return VO_FALSE;
	}
}

VO_BOOL vo_http_stream::IsPersistSockAvailableHTTPS()
{
	if( m_ptr_persist && m_ptr_persist->socket > 0 
		&& NULL != (( Persist_HTTPS* )m_ptr_persist)->m_ptr_ssl 
		&& m_ptr_persist->usedtimes <  m_ptr_persist->maxusetime
		&& m_ptr_persist->deadtime >  voOS_GetSysTime())
	{
		fd_set fdw;
		fd_set fde;
		timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 20000;

		FD_ZERO(&fdw);
		FD_SET(m_ptr_persist->socket,&fdw);
		FD_ZERO(&fde);
		FD_SET(m_ptr_persist->socket,&fde);

		VO_S32 select_ret = select( m_ptr_persist->socket + 1 ,NULL, &fdw,&fde,&timeout);

		if(FD_ISSET(m_ptr_persist->socket , &fde))
		{
			VOLOGE( "socket exception happen when check persist connection" );
			return VO_FALSE;
		}

		//if socket error happen or not writable , we should reset it.
		if( select_ret == -1 || select_ret == 0 )
		{
			return VO_FALSE;
		}
		else
		{
			m_ptr_persist->usedtimes++;
			return VO_TRUE;
		}
	}
	else
	{
		return VO_FALSE;
	}
}

VO_BOOL vo_http_stream::IsPersistSockAvailable()
{
	if( m_bhttps )
	{
		return IsPersistSockAvailableHTTPS();
	}
	else
	{
		return IsPersistSockAvailableHTTP();
	}
}

VO_BOOL vo_http_stream::persist_connect()
{
	m_http_downloadinfo.download_status = VO_CONNECTING;

	if( !IsPersistSockAvailable() || m_reuseoldsocket )
	{
		m_reuseoldsocket = VO_FALSE;
		//old socket is invalid
		VO_S32 ret = 0;
		if( m_ptr_persist )
		{
			m_ptr_persist->Close();
			if( m_bProxySet )
			{
				generate_tunnel_request(m_ptr_persist->tunnelrequest);
				ret = m_ptr_persist->Open( m_proxyhost , m_proxyport );
			}
			else
			{
				//query cached host--ip pair
				Notify( (VO_PBYTE)m_host , 0 , VO_CONTENT_DNSINFO_QUERY );
				if( m_bDnsIpCached && strlen(m_dnscachedip) > 0 )
				{
					ret = m_ptr_persist->Open( m_dnscachedip , m_port );
					//the cached dns ip may invalid for further session, for example, 3xx redirection, so clear it before next session.
					m_bDnsIpCached = VO_FALSE;
					memset( m_dnscachedip , 0 , sizeof( m_dnscachedip ) );
				}
				else
				{
					ret = m_ptr_persist->Open( m_host , m_port );
				}
			}
			if( ret <= 0 )
			{
				if( ret < 0 )
				{
					m_http_downloadinfo.error_code = VO_SOURCE2_IO_HTTP_CONNECTION_ERROR;
				}
				return VO_FALSE;
			}
			else
			{
				m_ptr_persist->deadtime = voOS_GetSysTime() + MAXSOCKETKEEPALIVETIME;
				m_ptr_persist->usedtimes++;
				//update DNS record to Cache 
				Notify( 0 , 0 , VO_CONTENT_DNSINFO_UPDATED );
			}
		}
		else
		{
			VOLOGE("set param is null");
			return VO_FALSE;
		}
	}
	else
	{
		m_reuseoldsocket = VO_TRUE;
	}

	return VO_TRUE;
}

VO_VOID vo_http_stream::Initdownloadinfo()
{
	m_http_downloadinfo.download_start_time = -1;
	m_http_downloadinfo.download_end_time = -1;
	m_http_downloadinfo.error_code = VO_SOURCE2_IO_HTTP_LAST_ERRORNONE;
	m_http_downloadinfo.total_download = 0;
	m_http_downloadinfo.download_mode = 0;
	m_http_downloadinfo.download_status = 0;
	m_http_downloadinfo.download_speed = 0;
	m_http_downloadinfo.get_response_time = -1;

	return;
}

VO_BOOL vo_http_stream::send_request()
{
	if( !persist_connect() )
	{
		return VO_FALSE;
	}

	if( !m_ptr_persist )
	{
		return VO_FALSE;
	}


	VO_CHAR request[4096];
	memset( request , 0 , sizeof(request) );
	VO_U32 request_size = generate_request( request );
	VOLOGI( "request: %s  socket: %d " , request , m_ptr_persist->socket );

	if( -1 == m_ptr_persist->Write( (VO_PBYTE)request , request_size ) )
	{
		m_http_downloadinfo.error_code = VO_SOURCE2_IO_HTTP_CONNECTION_ERROR;
		
		return VO_FALSE;
	}

	VOLOGI( "request send finished with socket: %d " , m_ptr_persist->socket );

	m_http_downloadinfo.download_start_time = voOS_GetSysTime();
	m_http_downloadinfo.download_status = VO_GOTRESPONSE;
	
	return VO_TRUE;
}

VO_VOID vo_http_stream::setsslapi( vosslapi ssl)
{ 
	if( m_bhttps )
	{
		m_ssl = ssl; 
		m_bsslinit = VO_TRUE;
	}
	else
	{
		;
	}
}

VO_BOOL vo_http_stream::run_agent( VO_CHAR * url )
{
	VO_BOOL ret = VO_FALSE;

	m_bhttps = Is_url_https( url );

	if( !resolve_url( url ) )
	{
		return VO_FALSE;
	}

	Initdownloadinfo();

	//prepare download setting info 
	if( !Notify( ( VO_PBYTE )url , 0 , VO_CONTENT_DOWNLOADSETTING_UPDATED ) )
	{
		VOLOGE( "prepare download setting info failed" );
		return VO_FALSE;
	}

	//for https link, we should set related API function pointer
	if( m_bhttps )
	{
		if(m_ptr_persist && m_bsslinit )
		{
			((Persist_HTTPS*)m_ptr_persist)->setsslapi(m_ssl);
		}
		else
		{
			VOLOGE("ssl api not set yet");
			return VO_FALSE;
		}
	}

	while( !m_bclose )
	{
		if( !send_request() )
		{
			VOLOGI( "send request failed!" );
			m_ptr_persist->Close();
			return VO_FALSE;
		}

		//ignore status code 100 and 101 of response string
		do
		{
			//try to get and analyze response from server 
			if( !get_response() )
			{
				VOLOGE("Fail to get response!");
				m_ptr_persist->Close();
				return VO_FALSE;
			}
		}while(m_status_code / 100 == 1 && m_status_code != -1 );

		//	VOLOGI("request:%s",request);
		//	VOLOGI("response:%s",m_response);

		ret = status_transfer();

		if( !ret && m_is_url_encoded )
		{
			//if the url path had been encoded, we should try once more.
			continue;
		}
		else
		{
			break;
		}
	}

	return ret;
}

VO_BOOL vo_http_stream::status_transfer()
{
	switch( m_status_code / 100 )
	{
	case HTTP_STATUS_SUCCESSFUL:
		{
			m_downloadcontent_wanted = VO_TRUE;
			Process_VCase();
		}
		break;
	case HTTP_STATUS_REDIRECTION:
		{
			m_ptr_persist->Close();
			return Process_Redirection();
		}
		break;
	case HTTP_STATUS_CLIENT_ERROR:
		{
			m_ptr_persist->Close();
			return Process_client_error();
		}
		break;
	case HTTP_STATUS_SERVER_ERROR:
		{
			m_ptr_persist->Close();
			Process_server_error();
			return VO_FALSE;
		}
		break;
	}

	return VO_TRUE;
}

VO_VOID vo_http_stream::Process_VCase()
{
	Notify( 0 , 0 , VO_CONTENT_OPEN_SUCCESS );
	m_http_downloadinfo.error_code = VO_SOURCE2_IO_HTTP_LAST_ERRORNONE;
	if(m_bAsyncMode)
	{
		download();
	}
	else
	{
		//start a new thread to download data.
		begin();
	}
}

VO_BOOL vo_http_stream::Process_client_error()
{
	//notify player side the download failed status
	VO_SOURCE2_IO_FAILEDREASON_DESCRIPTION description;
	description.reason = VO_SOURCE2_IO_HTTP_CLIENT_ERROR;
	description.szResponse = m_response;
	description.uResponseSize = strlen( m_response );
	IONotify( VO_SOURCE2_IO_HTTP_DOWNLOADFAILED , m_url , &description );

	//set error code when get the status code
	m_http_downloadinfo.error_code = VO_SOURCE2_IO_HTTP_CLIENT_SIDE_ERROR;

	switch(m_status_code)
	{
	case HTTP_STATUS_UNAUTHORIZED:
	case HTTP_STATUS_PROXY_UNAUTHORIZED:
		{
			return Process_Authentication();
		}
		break;
	case HTTP_STATUS_BAD_REQUEST:
	case HTTP_STATUS_NOT_FOUND:
		{
			if( !m_is_url_encoded )
			{
				if( urlencode() )
				{
					m_is_url_encoded = VO_TRUE;
				}
			}
			else
			{
				m_is_url_encoded = VO_FALSE;
			}
			return VO_FALSE;
		}
		break;
	default:
		{
			VOLOGE("http client error, status: %d" , m_status_code );
		}
		break;
	}

	//we should reset url encode flag, else it may cause dead loop
	if( m_is_url_encoded )
	{
		m_is_url_encoded = VO_FALSE;
	}

	return VO_FALSE;
}

VO_BOOL vo_http_stream::Process_Authentication()
{
	if( strlen(m_username) > 0 && strlen(m_password) > 0 )
	{
		m_authorization.set_username(m_username);
		m_authorization.set_password(m_password);

		//authentication challenge
		if( m_authorization.generate_authorizationinfo() )
		{
			memset( m_authinfo , 0 , sizeof(m_authinfo) );
			strcpy( m_authinfo , m_authorization.get_authorizationinfo() );
			m_bauth = VO_TRUE;
			return run_agent( m_url );
		}
		else
		{
			m_http_downloadinfo.error_code = VO_SOURCE2_IO_HTTP_AUTHENTICATION_FAIL;
		}
	}
	else
	{
		m_http_downloadinfo.error_code = VO_SOURCE2_IO_HTTP_AUTHENTICATION_FAIL;
	}

	return VO_FALSE;
}

VO_BOOL vo_http_stream::Process_Redirection()
{
	m_http_downloadinfo.error_code = VO_SOURCE2_IO_HTTP_REDIRECTION_ERROR;

	switch(m_status_code)
	{
	case HTTP_STATUS_MULTIPLE_CHOICES:
		{
			VOLOGI( "300 Multiple Choices" );
		}
		break;
	case HTTP_STATUS_MOVED_PERMANENTLY:
		{
			VOLOGI( "301 Moved Permanently" );
		}
		break;
	case HTTP_STATUS_MOVED_TEMPORARILY:
		{
			VOLOGI( "302 Found" );
		}
		break;
	case HTTP_STATUS_SEE_OTHER:
		{
			VOLOGI( "303 See Other" );
		}
		break;
	case HTTP_STATUS_NOT_MODIFIED:
		{
			VOLOGI( "304 Not Modified" );
		}
		break;
	case HTTP_STATUS_USE_PROXY:
		{
			VOLOGI( "305 Use Proxy" );
		}
		break;
	case HTTP_STATUS_UNUSED_1:
		{
			VOLOGI( "306 (Unused)" );
		}
		break;
	case HTTP_STATUS_TEMPORARY_REDIRECT:
		{
			VOLOGI( "307 Temporary Redirect" );
		}
		break;
	}

	if( strlen( m_reparseurl ) > 0 )
	{
		return run_agent( m_reparseurl );
	}
	else
	{
		return VO_FALSE;
	}
}

VO_VOID	vo_http_stream::Process_server_error()
{
	//notify player side the download failed status
	VO_SOURCE2_IO_FAILEDREASON_DESCRIPTION description;
	description.reason = VO_SOURCE2_IO_HTTP_SERVER_ERROR;
	description.szResponse = m_response;
	description.uResponseSize = strlen( m_response );
	IONotify( VO_SOURCE2_IO_HTTP_DOWNLOADFAILED , m_url , &description );

	switch(m_status_code)
	{
	case HTTP_STATUS_INTERNAL:
		{
			VOLOGE( "500 Internal Server Error" );
		}
		break;
	case HTTP_STATUS_NOT_IMPLEMENTED:
		{
			VOLOGE( "501 Not Implemented" );
		}
		break;
	case HTTP_STATUS_BAD_GATEWAY:
		{
			VOLOGE( "502 Bad Gateway" );
		}
		break;
	case HTTP_STATUS_UNAVAILABLE:
		{
			VOLOGE( "503 Service Unavailable" );
		}
		break;
	case HTTP_STATUS_GATEWAY_TIMEOUT:
		{
			VOLOGE( "504 Gateway Timeout" );
		}
		break;
	case HTTP_STATUS_VERSIONNOT_SUPPORT:
		{
			VOLOGE( "505 HTTP Version Not Supported" );
		}
		break;
	}

	m_http_downloadinfo.error_code = VO_SOURCE2_IO_HTTP_SERVER_SIDE_ERROR;

	return;
}

//http://admin:iaf49dfc16b@10.2.64.44
VO_BOOL vo_http_stream::resolve_host( VO_CHAR * host )
{
	memset( m_username , 0 , sizeof(m_username) );
	memset( m_password , 0 , sizeof(m_password) );

	VO_CHAR * ptr_begin = host;
	VO_CHAR * ptr_div = strchr( ptr_begin , ':' );
	if( ptr_div && ptr_div - ptr_begin > 0 )
	{
		if( ptr_div - ptr_begin >= sizeof(m_username) )
		{
			VOLOGE("avoid stack corruption, so exit");
			return VO_FALSE;
		}
		memcpy( m_username , ptr_begin , ptr_div - ptr_begin );
		ptr_begin = ptr_div;
		VO_CHAR * ptr_div = strchr( ptr_begin , '@' );
		if( ptr_div && ptr_div - ptr_begin > 0 )
		{
			if( ptr_div - ptr_begin - 1 > sizeof(m_password) )
			{
				VOLOGE("avoid stack corruption, so exit");
				return VO_FALSE;
			}
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

VO_BOOL vo_http_stream::resolve_urlHTTP( VO_CHAR * url )
{
	memset( m_host , 0 , sizeof(m_host) );
	memset( m_path , 0 , sizeof(m_path) );
	memset( m_port , 0 , sizeof(m_port) );

	//http://m.tvpot.daum.net 
	//we should deal with such case

	VO_CHAR * ptr = url;

	if( voiostrnicmp( ptr , "http://" ,strlen("http://")) == 0 )
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


VO_BOOL vo_http_stream::resolve_urlHTTPS( VO_CHAR * url )
{
	memset( m_host , 0 , sizeof(m_host) );
	memset( m_path , 0 , sizeof(m_path) );
	memset( m_port , 0 , sizeof(m_port) );
	VO_CHAR * ptr = url;

	if( voiostrnicmp( ptr , "https://" ,strlen("https://")) == 0 )
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
VO_BOOL vo_http_stream::resolve_url( VO_CHAR * url )
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

void vo_http_stream::thread_function()
{
	set_threadname( (char *)"HTTP Download" );

	if( m_bAsyncMode )
	{
		VOLOGI("AsyncMode");
		open_internal();
	}
	else
	{
		download();
	}

	VOLOGI("download exit");
}

void vo_http_stream::download()
{
	m_http_downloadinfo.download_status = VO_DOWNLOAD_CONTENT;
	if( VO_DOWNLOAD_CHUNK == m_http_downloadinfo.download_mode )
	{
		VOLOGI( "Chunk Mode!" );
		download_chunked();
		VOLOGI("chunk mode download-cost time:%d" , m_http_downloadinfo.download_end_time - m_http_downloadinfo.download_start_time);
	}
	else
	if( VO_DOWNLOAD_ENDLESS == m_http_downloadinfo.download_mode )
	{
		VOLOGI( "Endless Mode!" );
		download_endless();
		VOLOGI("endless mode download-cost time:%d" , m_http_downloadinfo.download_end_time - m_http_downloadinfo.download_start_time);
	}
	else
	if( VO_DOWNLOAD_NORMAL == m_http_downloadinfo.download_mode )
	{
		VOLOGI( "Normal Mode!" );
		download_normal();
		VOLOGI("Normal mode download-cost time:%d" , m_http_downloadinfo.download_end_time - m_http_downloadinfo.download_start_time);
	}
	else
	{
		Notify( 0 , 0 , VO_CONTENT_OPEN_FAILED );
		VOLOGE(" download mode is invalid");
	}
}

void vo_http_stream::download_normal()
{
	if( !m_ptr_persist )
	{
		return;
	}

	VO_S64 downloaded = 0;
	m_http_downloadinfo.total_download = 0;

	//in order to calculate download speed.
	VO_U32 start_time = voOS_GetSysTime();
	VO_S64 last_download = 0;

	VO_BOOL bNormalExit = VO_TRUE;
	VO_BOOL bNotified = VO_FALSE;
	while( ( downloaded < m_content_length ) && !m_bclose )
	{
		VO_S32 downloadsize = (VO_S32)( ( m_content_length - downloaded ) < MAXGETSIZE ? ( m_content_length - downloaded ) : MAXGETSIZE);
		VO_CHAR buffer[MAXGETSIZE];

		VO_S32 ret = m_ptr_persist->Read( (VO_PBYTE)buffer , downloadsize );
		if( ret == -1 )
		{
			VOLOGE("failed to recv data, socket error");
			bNormalExit = VO_FALSE;
			m_ptr_persist->Close();
			break;
		}
		else if( ret == 0 )
		{
			VOLOGI("exit download as required");
			m_ptr_persist->Close();
			bNormalExit = VO_FALSE;
			break;
		}
		else
		{
			downloaded = downloaded + ret;
			m_http_downloadinfo.total_download = downloaded;
		}
		
		//calculate download speed of recently.
		m_http_downloadinfo.download_end_time = voOS_GetSysTime();
		VO_U32 timecost = m_http_downloadinfo.download_end_time - start_time;
		if( m_http_downloadinfo.download_end_time > start_time &&  timecost  > 100 && downloaded > last_download )
		{
			m_http_downloadinfo.download_speed = (VO_U32)(( downloaded - last_download ) * 1000 / (timecost * 1024));
			last_download = downloaded;
			start_time = voOS_GetSysTime();
		}

		if( m_downloadcontent_wanted )
		{
			VO_S64 appended = append( (VO_PBYTE)buffer , ret );

			if( appended == -1 )
			{
				bNormalExit = VO_FALSE;
				VOLOGE( "Failed to write data to mem" );
				m_ptr_persist->Close();
				break;
			}

			if( downloaded >= IOBUFFERINGSIZE && !bNotified )
			{
				bNotified = VO_TRUE;
				Notify( 0 , 0 , VO_CONTENT_READAVAILABLE );
			}
			
			check_download_speed();
		}
		else
		{
			VOLOGI( "the content download is discard" );
		}
	}

	if( m_downloadcontent_wanted )
	{
		if( !bNotified )
		{
			bNotified = VO_TRUE;
			Notify( 0 , 0 , VO_CONTENT_READAVAILABLE );
		}
		
		m_http_downloadinfo.download_end_time = voOS_GetSysTime();
		if( m_bclose )
		{
			VOLOGI( "interrupted and downloaded size: %lld " , downloaded );
			m_http_downloadinfo.download_status = VO_DOWNLOAD_INTERRUPT;
		}
		else
		{
			if( !bNormalExit || downloaded != m_content_length )
			{
				VOLOGI( "download partially and exit,downloaded size: %lld " , downloaded );
				m_http_downloadinfo.download_status = VO_DOWNLOAD_PARTIALLY;
			}
			else
			{
				VOLOGI( "normal exit and downloaded size: %lld " , downloaded );
				m_http_downloadinfo.download_status = VO_DOWNLOAD_NORMAL_EXIT;
			}
		}
	}

	if( m_bclose )
	{
		m_ptr_persist->Close();
	}
}

void vo_http_stream::download_chunked()
{
	if( !m_ptr_persist )
	{
		return;
	}

	VO_S32 chunk_size = 0;
	VO_S64 filesize = 0;
	VO_S64 appended = 0;
	VO_S32 ret = 0;

	VO_BOOL bNormalExit = VO_TRUE;
	VO_BOOL bNotified = VO_FALSE;
	do
	{
		VO_BYTE buffer[MAXGETSIZE];
		memset( buffer , 0 , MAXGETSIZE );
		VO_S32 pos = 0;

		while( 1 && pos < MAXGETSIZE )
		{
			ret = m_ptr_persist->Read( buffer + pos , 1 );
			if( ret == -1 )
			{
				VOLOGE("failed to recv data, socket error");
				m_ptr_persist->Close();
				bNormalExit = VO_FALSE;
				break;
			}
			else if( ret == 0 )
			{
				VOLOGI("exit download as required");
				m_ptr_persist->Close();
				bNormalExit = VO_FALSE;
				break;
			}

			if( pos >= 1 && buffer[pos] == '\n' && buffer[pos - 1] == '\r' )
			{
				buffer[pos - 1] = '\0';
				break;
			}

			pos++;
		}

		if( !bNormalExit )
		{
			break;
		}

		sscanf( (VO_CHAR*)buffer , "%x" , (unsigned int*)&chunk_size );

		filesize = filesize + chunk_size;

		VO_S32 buffertoread = chunk_size;

		while( buffertoread && !m_bclose )
		{
			VO_S32 toread = buffertoread < MAXGETSIZE ? buffertoread : MAXGETSIZE;

			ret = m_ptr_persist->Read( (VO_PBYTE)buffer , toread );
			if( ret == -1 )
			{
				VOLOGE("failed to recv data, socket error");
				bNormalExit = VO_FALSE;
				m_ptr_persist->Close();
				break;
			}
			else if( ret == 0 )
			{
				VOLOGI("exit download as required");
				m_ptr_persist->Close();
				bNormalExit = VO_FALSE;
				break;
			}
			else
			{
				buffertoread = buffertoread - ret;
			}

			appended = append( (VO_PBYTE)buffer , ret );

			if( appended == -1 )
			{
				m_ptr_persist->Close();
				VOLOGE( "Failed to write data to mem" );
				bNormalExit = VO_FALSE;
				break;
			}

			if( (filesize - chunk_size + appended) >= IOBUFFERINGSIZE && !bNotified )
			{
				bNotified = VO_TRUE;
				Notify( 0 , 0 , VO_CONTENT_READAVAILABLE );
			}

			check_download_speed();
		}

		if( appended == -1 )
		{
			m_ptr_persist->Close();
			VOLOGE( "Failed to write data to mem" );
			bNormalExit = VO_FALSE;
			break;
		}

		if( !m_bclose )
		{
			ret = m_ptr_persist->Read( buffer , 2 );
			if( ret == -1 )
			{
				VOLOGE("failed to recv data, socket error");
				bNormalExit = VO_FALSE;
				m_ptr_persist->Close();
				break;
			}
			else if( ret == 0 )
			{
				VOLOGI("exit download as required");
				m_ptr_persist->Close();
				bNormalExit = VO_FALSE;
				break;
			}
		}

	} while ( chunk_size && !m_bclose );

	if( !m_bclose )
		m_content_length = filesize;

	if( !bNotified )
	{
		bNotified = VO_TRUE;
		Notify( 0 , 0 , VO_CONTENT_READAVAILABLE );
	}
	
	m_http_downloadinfo.download_end_time = voOS_GetSysTime();
	
	if( m_bclose  )
	{
		m_http_downloadinfo.download_status = VO_DOWNLOAD_INTERRUPT;
	}
	else
	{
		if( !bNormalExit )
		{
			VOLOGI( "download partially and exit,downloaded size: %lld " , m_content_length );
			m_http_downloadinfo.download_status = VO_DOWNLOAD_PARTIALLY;
		}
		else
		{
			VOLOGI( "normal exit and downloaded size: %lld " , m_content_length );
			m_http_downloadinfo.download_status = VO_DOWNLOAD_NORMAL_EXIT;
		}
	}
	
	if( m_bclose )
	{
		m_ptr_persist->Close();
	}
}

void vo_http_stream::download_endless()
{
	if( !m_ptr_persist )
	{
		return;
	}

	VO_S64 filesize = 0;

	VO_BOOL bNotified = VO_FALSE;
	while( !m_bclose )
	{
		VO_CHAR buffer[MAXGETSIZE];
		VO_S32 ret = m_ptr_persist->Read( (VO_PBYTE)buffer , MAXGETSIZE );
		if( ret == -1 )
		{
			VOLOGI("failed to recv data");
			m_ptr_persist->Close();
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
			filesize = filesize + ret;
		}

		VO_S64 appended = append( (VO_PBYTE)buffer , ret );

		if( appended == -1 )
		{
			m_ptr_persist->Close();
			VOLOGE( "Failed to write data to mem" );
			break;
		}

		if( ( filesize ) >= IOBUFFERINGSIZE && !bNotified )
		{
			bNotified = VO_TRUE;
			Notify( 0 , 0 , VO_CONTENT_READAVAILABLE );
		}
		check_download_speed();
	}

	if( !m_bclose )
		m_content_length = filesize;

	if( !bNotified )
	{
		bNotified = VO_TRUE;
		Notify( 0 , 0 , VO_CONTENT_READAVAILABLE );
	}
	
	m_http_downloadinfo.download_end_time = voOS_GetSysTime();
	
	if( m_bclose )
	{
		m_http_downloadinfo.download_status = VO_DOWNLOAD_INTERRUPT;
	}
	else
	{
		m_http_downloadinfo.download_status = VO_DOWNLOAD_NORMAL_EXIT;
	}
	
	if( m_bclose )
	{
		m_ptr_persist->Close();
	}
}

VO_BOOL vo_http_stream::IsDefaultPortHTTP()
{
	if( !strcmp( m_port , "80" ) )
	{
		return VO_TRUE;
	}

	return VO_FALSE;
}

VO_BOOL vo_http_stream::IsDefaultPortHTTPS()
{
	if( !strcmp( m_port , "443" ) )
	{
		return VO_TRUE;
	}

	return VO_FALSE;
}

VO_BOOL vo_http_stream::IsDefaultPort()
{
	if( m_bhttps )
	{
		return IsDefaultPortHTTPS();
	}
	else
	{
		return IsDefaultPortHTTP();
	}
}

VO_U32 vo_http_stream::generate_request( VO_CHAR * ptr_request )
{
	VO_U32 size = 0;
	if( !IsDefaultPort() )
	{
		char * request_template;
		if( m_ptr_persist )
		{
			request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s:%s\r\nConnection: keep-alive\r\n";
		}
		else
		{
			request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s:%s\r\nConnection: close\r\n";
		}
		size = sprintf( ptr_request , request_template , m_path , m_host , m_port );
	}
	else
	{
		char * request_template;
		if( m_ptr_persist )
		{
			request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\n";
		}
		else
		{
			request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n";
		}
		size = sprintf( ptr_request , request_template , m_path , m_host );
	}

	if( m_range_offset == 0 && m_range_length == MAXU64 )
	{
		;
	}
	else
	{
		char temp[256];
		memset( temp , 0 , 256 );

		VO_U32 sublen = 0;
		if( m_range_length == MAXU64 )
		{
			sublen = sprintf( temp , "Range: bytes=%lld-\r\n" , m_range_offset );
		}
		else
		{
			sublen = sprintf( temp , "Range: bytes=%lld-%lld\r\n" , m_range_offset , m_range_offset + m_range_length - 1 );
		}

		strcat( ptr_request , temp );
		size = size + sublen;
	}

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

	VO_S32 nagentlen = strlen( m_useragent );
	if( m_bUserAgentSet && nagentlen > 0 )
	{
		strcat( ptr_request ,(char *) "User-Agent: " );
		size = size + strlen( "User-Agent: " );
		strcat( ptr_request , m_useragent );
		size = size + strlen( m_useragent );
		if( m_useragent[nagentlen-1] != '\n' ||  m_useragent[nagentlen-2] != '\r' )
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

	VOLOGI("request size: %d " , size );

	return size;
}

VO_BOOL vo_http_stream::get_response()
{
	if( !m_ptr_persist )
	{
		return VO_FALSE;
	}

	VO_U32 count = 0;
	memset( m_response , 0 , sizeof(m_response) );

	VO_U32 get_response_start = voOS_GetSysTime();

	while( !m_bclose )
	{
		//check timeout 5s ,if so, then exit get response from server
		VO_U32 get_response_cost = voOS_GetSysTime() - get_response_start;
		if( get_response_cost > VODEFAULTTIMEOUT )
		{
			VOLOGE( "Response timeout!" );
			m_http_downloadinfo.error_code = VO_SOURCE2_IO_HTTP_GET_RESPONSE_TIMEOUT;
			return VO_FALSE;
		}

		VO_CHAR value;
		VO_S32 ret = 0;
		ret = m_ptr_persist->Read( (VO_PBYTE)&value , 1 );
		if( ret <= 0 )
		{
			if( ret < 0 )
			{
				m_http_downloadinfo.error_code = VO_SOURCE2_IO_HTTP_CONNECTION_ERROR;
				VOLOGE( "Response connection error!" );
			}
			else
			{
				m_ptr_persist->Close();
				VOLOGE( "exit called when get response" );
			}
			return VO_FALSE;
		}

		m_response[count] = value;

		//the response string is terminated with "\r\n\r\n"
		if( count >= 3 && m_response[count] == '\n' && m_response[count - 1] == '\r' 
			&& m_response[count - 2] == '\n' && m_response[count - 3] == '\r' )
			break;

		count++;

		if( count >= sizeof(m_response) )
		{
			VOLOGE( "Response too large!" );
			m_http_downloadinfo.error_code = VO_SOURCE2_IO_HTTP_RESPONSE_TOOLARGE;
			//the response is invalid, it may caused by previous remained A/V data, so reset socket.
			return VO_FALSE;
		}
	}

	m_http_downloadinfo.get_response_time = voOS_GetSysTime();
	VOLOGI("got-response cost time:%d" , m_http_downloadinfo.get_response_time - m_http_downloadinfo.download_start_time);

	if( m_bclose )
	{
		VOLOGI("get_response process is interrupted" );
		return VO_FALSE;
	}

	VOLOGI("%s  socket :%d" , m_response , m_ptr_persist->socket);
	if( !analyze_response() )
	{
		VOLOGE( "the response can't be recognized" );
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_S32 vo_http_stream::StatusCode()
{
	if( strncstr( m_response ,(char *)  "<html>" ) ==  m_response )
	{
		//the response is in webpage format
		if( strncstr( m_response , (char *)"HTTP/1.1" ) )
		{
			sscanf( m_response , "HTTP/1.1 %d " , (int *)&m_status_code );
			m_protocol_ver = 11;
		}
		else if( strncstr( m_response ,(char *) "HTTP/1.0" ) )
		{
			sscanf( m_response , "HTTP/1.0 %d " , (int *)&m_status_code );
			m_protocol_ver = 10;
		}
		else if( strncstr( m_response , (char *)"ICY" ) )
		{                         
			sscanf( m_response , "ICY %d OK" , (int *)&m_status_code );
			m_protocol_ver = 11;
			VOLOGR( "the link maybe is a shoutcast radio" );
		}
		else
		{
			m_status_code = -1;
			m_protocol_ver = 10;
		}
	}
	else
	{
		if( strncstr( m_response ,(char *) "HTTP/1.1" ) == m_response )
		{
			sscanf( m_response , "HTTP/1.1 %d " , (int *)&m_status_code );
			m_protocol_ver = 11;
		}
		else if( strncstr( m_response , (char *)"HTTP/1.0" ) == m_response )
		{
			sscanf( m_response , "HTTP/1.0 %d " , (int *)&m_status_code );
			m_protocol_ver = 10;
		}
		else if( strncstr( m_response ,(char *) "ICY" ) == m_response )
		{                         
			sscanf( m_response , "ICY %d OK" , (int *)&m_status_code );
			m_protocol_ver = 11;
			VOLOGR( "the link maybe is a shoutcast radio" );
		}
		else
		{
			m_status_code = -1;
			m_protocol_ver = 10;
		}
	}
	

	return m_status_code;
}

VO_BOOL vo_http_stream::IsChunked()
{
	VO_CHAR * ptr = strstr( m_response , "Transfer-Encoding:" );
	if( ptr )
	{
		ptr = ptr + strlen("Transfer-Encoding:");
		while( *ptr == ' '  )
			ptr++;

		if( strstr( ptr , "chunked" ) == ptr )
		{
			m_http_downloadinfo.download_mode = VO_DOWNLOAD_CHUNK;
			return VO_TRUE;	
		}
	}

	return VO_FALSE;
}

VO_BOOL vo_http_stream::ContentLength()
{
	VO_CHAR *ptr = strstr( m_response , "Content-Length:" );
	if( ptr )
	{
		ptr += strlen("Content-Length:");
		while( *ptr == ' ')
		{
			//remove leading WSP characters value
			ptr++;
		}

		//we need to assure that the value of Content-Length is successfully sscanf in.
		m_content_length = -1;
		VO_S32 ret = sscanf( ptr , "%lld\r\n" , &m_content_length );
		if(ret != 1)
		{
			VOLOGE( "the value of Content-Length  is expected but failed to sscanf" );
		}
		else
		{
			Notify( 0 , m_content_length , VO_CONTENT_LEN );
			m_http_downloadinfo.download_mode = VO_DOWNLOAD_NORMAL;
			return VO_TRUE;
		}
	}

	m_http_downloadinfo.download_mode = VO_DOWNLOAD_ENDLESS;
	
	return VO_FALSE;
}

VO_VOID vo_http_stream::ContentType()
{
	VO_CHAR *ptr = strstr( m_response , "Content-Type:" );

	memset( m_mimetype , 0 , sizeof( m_mimetype ) );

	if( ptr )
	{
		ptr = ptr + strlen( "Content-Type:" );
		while( *ptr == ' ')
		{
			//remove leading WSP characters value
			ptr++;
		}

		sscanf( ptr , "%s\r\n" , m_mimetype );
	}
}

VO_VOID vo_http_stream::Persist_keepalive()
{
	if( m_ptr_persist && strncstr( m_response , (VO_CHAR*)"Connection: close" ))
	{
		m_ptr_persist->deadtime = voOS_GetSysTime();
		m_ptr_persist->maxusetime = 1;
		return;
	}

	if( m_ptr_persist && !m_ptr_persist->brefreshbyserver 
		&& strncstr( m_response , (VO_CHAR*)"Connection: keep-alive" )
		)
	{
		m_ptr_persist->brefreshbyserver = VO_TRUE;
		char temp[1024];
		char value[100];
		memset( temp , 0 , 1024 );
		memset( value , 0 , 100 );

		VO_CHAR *ptr = 0;
		VO_CHAR *ptr_end = 0;
		ptr = strncstr( m_response , (VO_CHAR*)"Keep-Alive:" );
		if( ptr )
		{
			//find the line end
			ptr_end = ptr;
			while( *ptr_end && ( *ptr_end != '\r' && *( ptr_end+1 ) != '\n') )
			{
				ptr_end++;
			}

			//remember remove trailing WSP character of value
			while( *(ptr_end-1) == ' ')
			{
				ptr_end--;
			}

			//get the target data
			if( ptr_end - ptr > 0 &&  ptr_end - ptr < sizeof(temp) )
			{
				strncpy( temp , ptr , ptr_end - ptr );
			}
			else
			{
				return;
			}

			//process timeout field
			ptr = strncstr( temp , (VO_CHAR*)"timeout" );

			if( ptr )
			{
				ptr = ptr + strlen( "timeout" );
				while( *ptr == ' ' || *ptr == '=' )
				{
					ptr++;
				}

				if( strlen(ptr) < sizeof(value) )
				{
					strcpy( value , ptr );
				}
				else
				{
					return;
				}

				ptr = strchr( value , ',' );

				if( ptr )
					*ptr = '\0';

				if( vostr_is_digit(value) )
				{
					VO_U32 lasttime = (VO_U32)( atoi(value) * 1000 );

					if( lasttime >= MAXSOCKETKEEPALIVETIME )
						lasttime = MAXSOCKETKEEPALIVETIME;

					m_ptr_persist->deadtime = voOS_GetSysTime() + lasttime;
				}
			}

			//process max field
			memset( value , 0 , 100 );
			ptr = strncstr( temp , (VO_CHAR*)"max" );

			if( ptr )
			{
				ptr = ptr + strlen( "max" );
				while( *ptr == ' ' || *ptr == '=' )
				{
					ptr++;
				}

				if( strlen(ptr) < sizeof(value) )
				{
					strcpy( value , ptr );
				}

				ptr = strchr( value , ',' );

				if( ptr )
					*ptr = '\0';

				if( vostr_is_digit(value) )
				{
					VO_S32 maxtimes = atoi(value);

					if( maxtimes >= MAXSOCKETREUSETIMES )
						maxtimes = MAXSOCKETREUSETIMES;

					m_ptr_persist->maxusetime = maxtimes;
				}
			}
		}
	}
}
		

cookie_storage_node* vo_http_stream::get_cooki_node()
{
	return m_http_cookie.get_cooki_node();
}

VO_BOOL vo_http_stream::analyze_response()
{
	m_status_code = StatusCode();
	if( m_status_code == -1 )
	{
		return VO_FALSE;
	}

	if( m_status_code == HTTP_STATUS_UNAUTHORIZED || m_status_code == HTTP_STATUS_PROXY_UNAUTHORIZED )
	{
		//we should process authentication info here
		m_authorization.process_challenge_info( m_response );
	}

	if( !IsChunked() )
	{
		ContentLength();
	}

	ContentType();

	Persist_keepalive();

	if( !m_is_ignore_cookie_setting )
	{
		m_http_cookie.Reset();
		m_http_cookie.set_urlinfo( m_host , m_path , m_port );
		//we should update cookie info and use the new cookie info in time
		if( m_http_cookie.process_cookie( m_response ) )
		{
			Notify( 0 , 0 , VO_CONTENT_COOKIE_UPDATED );
		}
	}

	if( m_status_code / 100 == HTTP_STATUS_REDIRECTION )
	{
		analyze_url_redirection();
	}

	return VO_TRUE;
}

VO_VOID vo_http_stream::process_redirect_url()
{
	VO_CHAR * ptr = strncstr( m_response , (char *) "Location:" );

	if( ptr )
	{
		ptr = ptr + strlen( "Location:" );

		VO_CHAR *ptr_url = strncstr( ptr ,(char *)  "http://" );
		if( !ptr_url )
		{
			ptr_url = strncstr( ptr ,(char *)  "https://" );
		}

		if( !ptr_url )
		{
			memset( m_reparseurl , 0 , sizeof( m_reparseurl ) );
		}
		else
		{
			VO_CHAR * ptr_temp = strchr( ptr_url , '\r' );

			memset( m_reparseurl , 0 , sizeof( m_reparseurl ) );
			strncpy( m_reparseurl , ptr_url , ptr_temp - ptr_url );
		}
	}
	else
	{
		memset( m_reparseurl , 0 , sizeof( m_reparseurl ) );
	}
}

VO_VOID vo_http_stream::analyze_url_redirection()
{
	switch ( m_status_code )
	{
	case HTTP_STATUS_MULTIPLE_CHOICES:
		{
			//Multiple Choices
			process_redirect_url();
			m_is_cache_redirecturl = VO_FALSE;
		}
		break;
	case HTTP_STATUS_MOVED_PERMANENTLY:
		{
			//Moved Permanently
			process_redirect_url();
			m_is_cache_redirecturl = VO_TRUE;
		}
		break;
	case HTTP_STATUS_MOVED_TEMPORARILY:
		{
			//The requested resource resides temporarily under a different URI
			process_redirect_url();
			m_is_cache_redirecturl = VO_TRUE;
		}
		break;
	case HTTP_STATUS_SEE_OTHER:
		{
			//The new URI is not a substitute reference for the originally requested resource
			//The 303 response MUST NOT be cached
			process_redirect_url();
			m_is_cache_redirecturl = VO_FALSE;
		}
		break;
	case HTTP_STATUS_NOT_MODIFIED:
		{
			//Not Modified
			//The 304 response MUST NOT contain a message-body
		}
		break;
	case HTTP_STATUS_USE_PROXY:
		{
			//Use Proxy
			process_redirect_url();
			m_is_cache_redirecturl = VO_FALSE;
		}
		break;
	case HTTP_STATUS_UNUSED_1:
		{
			//The 306 status code was used in a previous version (http 1.0 ) of the specification, 
			//is no longer used, and the code is reserved
			process_redirect_url();
			m_is_cache_redirecturl = VO_FALSE;
		}
		break;
	case HTTP_STATUS_TEMPORARY_REDIRECT:
		{
			//Temporary Redirect
			process_redirect_url();
			m_is_cache_redirecturl = VO_TRUE;
		}
		break;	
	default:
		{
			m_is_cache_redirecturl = VO_FALSE;
		}
	}

	return;
}



VO_VOID vo_http_stream::stop_download_thread()
{
	vo_thread::stop();
}

//KBps
VO_S32 vo_http_stream::get_download_speed()
{
	if( m_http_downloadinfo.total_download == 0 ||
		m_http_downloadinfo.download_start_time == -1 ||
		m_http_downloadinfo.download_end_time == -1 )
	{
		return -1;
	}

	if( m_http_downloadinfo.download_speed > 0 )
	{
		return (VO_S32)m_http_downloadinfo.download_speed;
	}
	else
	{
		//the download speed calculate in this way is not so accurate.
		VO_U32 timecost = m_http_downloadinfo.download_end_time - m_http_downloadinfo.download_start_time;

		if( timecost == 0 )
			return -1;

		return (VO_S32)(( m_http_downloadinfo.total_download * 1000.) / ( timecost * 1024 ));
	}
}

VO_U32 vo_http_stream::get_download_time()
{
	if( m_http_downloadinfo.download_end_time == -1 || m_http_downloadinfo.get_response_time == -1 )
	{
		return 0;
	}
	else
	{
		return m_http_downloadinfo.download_end_time - m_http_downloadinfo.get_response_time;
	}
}


char * vo_http_stream::get_redirecturl()
{
	if( strlen( m_reparseurl ) == 0 )
		return 0;

	return m_reparseurl;
}


VO_VOID vo_http_stream::set_cookies(char * cookies )
{
	if( strlen(cookies) < sizeof( m_cookies ) )
	{
		strcpy( m_cookies , cookies );
	}
}

//username:password
VO_VOID vo_http_stream::set_userpass(VO_CHAR * userpass , VO_U32 uplen )
{
	VO_CHAR namepass[200];
	memset( namepass , 0 , sizeof(namepass) );
	memset( m_username , 0 , sizeof(m_username) );
	memset( m_password , 0 , sizeof(m_password) );

	if( uplen > 0 )
	{
		strncpy( namepass , userpass , uplen );

		VO_CHAR * ptr_begin = namepass;
		VO_CHAR * ptr_div = strchr( ptr_begin , ':' );
		if( ptr_div && ptr_div - ptr_begin > 0 )
		{
			if( ptr_div - ptr_begin >= sizeof(m_username) )
			{
				VOLOGE("avoid stack corruption, so exit");
				return;
			}
			memcpy( m_username , ptr_begin , ptr_div - ptr_begin );
			if( *(ptr_div + 1) )
			{
				strcpy( m_password , ptr_div + 1 );
			}
		}
	}
}

VO_VOID vo_http_stream::set_download2buff_callback( VO_PTR pParam )
{
	voCAutoLock lock(&m_lock_w2buff);
	VO_HTTP_DOWNLOAD2BUFFERCALLBACK* ptr = ( VO_HTTP_DOWNLOAD2BUFFERCALLBACK* )pParam;
	m_download2bufferCB.HttpStreamCBFunc = ptr->HttpStreamCBFunc;
	m_download2bufferCB.handle = ptr->handle;
}

VO_VOID vo_http_stream::set_IO_Callback( VO_PTR pParam )
{
	VO_SOURCE2_IO_HTTPCALLBACK* ptr = ( VO_SOURCE2_IO_HTTPCALLBACK* )pParam;
	if( !ptr )
	{
		VOLOGE("the parameter setin is null");
		return;
	}
	m_httpiocb.hHandle = ptr->hHandle;
	m_httpiocb.IO_Callback = ptr->IO_Callback;
}

VO_BOOL vo_http_stream::Is_cookie_update()
{
	return m_http_cookie.Is_cookie_update();
}

VO_BOOL vo_http_stream::Is_auth()
{
	return m_bauth;
}

VO_CHAR * vo_http_stream::get_authinfo()
{
	return m_authinfo;
}

VO_VOID	vo_http_stream::set_authinfo( VO_CHAR* str_auth )
{
	strcpy( m_authinfo , str_auth );
	m_bauth = VO_TRUE;
}

VO_VOID	vo_http_stream::setProxyHost( VO_CHAR* strProxyHost )
{
	strcpy( m_proxyhost , strProxyHost );
	m_bProxySet = VO_TRUE;
}

VO_VOID	vo_http_stream::setProxyPort( VO_CHAR* strProxyPort )
{
	strcpy( m_proxyport , strProxyPort );
	m_bProxySet = VO_TRUE;
}

VO_VOID	vo_http_stream::setUserAgent( VO_CHAR* strUserAgent )
{
	strcpy( m_useragent , strUserAgent );
	m_bUserAgentSet = VO_TRUE;
}

VO_BOOL vo_http_stream::IsDownloadComplete()
{
	if( m_http_downloadinfo.download_status == VO_DOWNLOAD_NORMAL_EXIT )
	{
		return VO_TRUE;
	}

	return VO_FALSE;
}

VO_BOOL vo_http_stream::IsDownloadFinished()
{
	if( m_http_downloadinfo.download_status == VO_DOWNLOAD_NORMAL_EXIT || 
		m_http_downloadinfo.download_status == VO_DOWNLOAD_INTERRUPT ||
		m_http_downloadinfo.download_status == VO_DOWNLOAD_PARTIALLY)
	{
		return VO_TRUE;
	}

	return VO_FALSE;
}

VO_VOID vo_http_stream::set_maxspeed( VO_U32 speed)
{ 
	//VOLOGR("max download speed : %d" , speed );
	m_max_download_speed = speed; 
}

VO_VOID vo_http_stream::check_download_speed()
{
	if( !m_bslow_down && get_download_speed() > (VO_S32)(m_max_download_speed / 1024) )
	{
		m_bslow_down = VO_TRUE;
	}

	if( m_bslow_down  && get_download_speed() < (VO_S32)(m_min_download_speed / 1024) )
	{
		m_bslow_down = VO_FALSE;
	}

	if( m_bslow_down )
	{
		voOS_Sleep(10);
	}

	if( m_nspeed )
	{
		voOS_Sleep( m_nspeed );
	}
}

VO_U32 vo_http_stream::GetLastError()
{
	//when user call get last error, we should print the info of download for debug
	print_downloadInfo();
	return m_http_downloadinfo.error_code;
}

VO_VOID vo_http_stream::print_downloadInfo()
{
	//print the current download info for debug purpose
	if( m_http_downloadinfo.download_status == VO_CONNECTING )
	{
		if( m_http_downloadinfo.error_code == VO_SOURCE2_IO_HTTP_CONNECTION_ERROR )
		{
			VOLOGE( "connect failed" );
		}
		else
		{
			VOLOGI( "connecting......" );
		}
	}

	if( m_http_downloadinfo.download_status == VO_GOTRESPONSE )
	{
		if( m_http_downloadinfo.error_code == VO_SOURCE2_IO_HTTP_CONNECTION_ERROR )
		{
			VOLOGE( "when get response , socket failed" );
		}
		else
			if( m_http_downloadinfo.error_code == VO_SOURCE2_IO_HTTP_GET_RESPONSE_TIMEOUT )
			{
				VOLOGE( "when get response , timeout" );
			}
			else
				if( m_http_downloadinfo.error_code == VO_SOURCE2_IO_HTTP_RESPONSE_TOOLARGE )
				{
					VOLOGE( "when get response , response too large" );
				}
				else
					if( m_http_downloadinfo.error_code == VO_SOURCE2_IO_HTTP_SERVER_SIDE_ERROR )
					{
						VOLOGE( "when get response , the server tells error on server side" );
					}
					else
					{
						VOLOGI(" get response of server ......");
					}
	}

	if( m_http_downloadinfo.download_status == VO_DOWNLOAD_CONTENT )
	{
		if( m_http_downloadinfo.download_mode == VO_DOWNLOAD_NORMAL )
		{
			VOLOGI( "normal downloading , the Download_Speed : %d KBps , time is: %d ms" , get_download_speed() , get_download_time() );
		}
		else
			if( m_http_downloadinfo.download_mode == VO_DOWNLOAD_CHUNK )
			{
				VOLOGI( "chunk downloading....." );
			}
			else
				if( m_http_downloadinfo.download_mode == VO_DOWNLOAD_CHUNK )
				{
					VOLOGI( "endless downloading....." );
				}

				if( m_http_downloadinfo.error_code == VO_SOURCE2_IO_HTTP_CONNECTION_ERROR )
				{
					VOLOGE( "socket failed when downloading....." );
				}
	}

	if( m_http_downloadinfo.download_status == VO_DOWNLOAD_NORMAL_EXIT )
	{
		VOLOGE( "socket closed and exit downloading normally....." );
	}

	VOLOGI("startime: %d ms,endtime: %d ms ,total download bytes: %lld == %lld KBps		Error code: 0x%08x " , m_http_downloadinfo.download_start_time ,
		   m_http_downloadinfo.download_end_time , m_http_downloadinfo.total_download , m_http_downloadinfo.total_download / 1024 ,m_http_downloadinfo.error_code );
}

VO_BOOL vo_http_stream::isxnumalpha( char x )
{
	if(  ('0' <= x && x <= '9') ||//0-9
		 ('a' <= x && x <= 'z') ||//abc...xyz
		 ('A' <= x && x <= 'Z')   //ABC...XYZ
	  )
	{
		return VO_TRUE;
	}
	else
	{
		return VO_FALSE;
	}
}

VO_BOOL vo_http_stream::isxhex( char x )
{
	if(  ('0' <= x && x <= '9') ||//0-9
		 ('a' <= x && x <= 'f') ||//abcdef
		 ('A' <= x && x <= 'F')   //ABCDEF
	  )
	{
		return VO_TRUE;
	}
	else
	{
		return VO_FALSE;
	}
}


VO_BOOL vo_http_stream::urlencode() 
{
	VO_BOOL encodeurl = VO_FALSE;
	char *pstr = m_path;
	char *buf = (char *)malloc(strlen(pstr) * 3 + 1);
	char *pbuf = buf;
	while (*pstr) 
	{
		if ( isxnumalpha(*pstr) || *pstr =='/' || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~') 
		{
			*pbuf++ = *pstr;
		}
		else if (*pstr == '%' && isxhex(*(pstr+1)) && isxhex(*(pstr+2)) ) 
		{
			//the url may contain url encoded char, %20 for example, we should not encode it again.
			*pbuf++ = *pstr++;
			*pbuf++ = *pstr++;
			*pbuf++ = *pstr;
		}
		else 
		{
			encodeurl = VO_TRUE;
			*pbuf++ = '%'; 
			*pbuf++ = toHex(*pstr >> 4); 
			*pbuf++ = toHex(*pstr & 15);
		}

		pstr++;
	}
	*pbuf = '\0';

	//if it is same as before encode
	if( !strcmp( m_path , buf) )
	{
		encodeurl = VO_FALSE;
	}

	memset( m_path , 0 , sizeof(m_path) );
	strcpy( m_path , buf );
	free(buf);

	return encodeurl;
}

/* Converts x to its hex character*/
unsigned char vo_http_stream::toHex(const unsigned char x) 
{
	static char desthex[] = "0123456789abcdef";
	return desthex[x & 15];
}


VO_VOID vo_http_stream::control_speed( VO_S32 speed )
{
	VOLOGI("slow Download_Speed : %d" , speed );
	m_nspeed = speed;
}

VO_U32 vo_http_stream::generate_tunnel_requestHTTP( VO_CHAR * ptr_request )
{
	return 0;
}

VO_U32 vo_http_stream::generate_tunnel_requestHTTPS( VO_CHAR * ptr_request )
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

	VO_S32 nagentlen = strlen( m_useragent );
	if( m_bUserAgentSet && nagentlen > 0 )
	{
		strcat( ptr_request ,(char *) "User-Agent: " );
		size = size + strlen( "User-Agent: " );
		strcat( ptr_request , m_useragent );
		size = size + strlen( m_useragent );
		if( m_useragent[nagentlen-1] != '\n' ||  m_useragent[nagentlen-2] != '\r' )
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

VO_U32 vo_http_stream::generate_tunnel_request( VO_CHAR * ptr_request )
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

VO_CHAR * vo_http_stream::getdnscacheip()
{
	if( m_ptr_persist )
	{
		return m_ptr_persist->addr_buf;
	}
	else
	{
		return 0;
	}
}

VO_VOID	vo_http_stream::setCachedDnsIP( VO_CHAR* strDNSCacheIP )
{
	if( strDNSCacheIP && strlen( strDNSCacheIP ) > 0 )
	{
		strcpy( m_dnscachedip , strDNSCacheIP );
		m_bDnsIpCached = VO_TRUE;
	}
}


