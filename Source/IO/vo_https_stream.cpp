
#ifdef WINCE
#include <winsock2.h>
#endif
#include "vo_https_stream.h"
#include "vo_http_utils.h"
#include "voString.h"
#include "voOSFunc.h"
#include "vo_mem_stream.h"
#include "NetWork/vo_socket.h"
#include "voSource2_IO.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define MAXGETSIZE (1024*2)
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
			VO_S32 ret = vo_socket_recv( socket , (VO_PBYTE)&value , 1 );

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
			sscanf( response , "HTTP/1.1 %d " , &status_code );
		}
		else if( strncstr( response , (char *)"HTTP/1.0" ) == response )
		{
			sscanf( response , "HTTP/1.0 %d " , &status_code );
		}
		else if( strncstr( response ,(char *) "ICY" ) == response )
		{                         
			sscanf( response , "ICY %d OK" , &status_code );
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


vo_https_stream::vo_https_stream()
: m_bsslinit(VO_FALSE)
{
	memset( &m_ssl , 0 , sizeof( m_ssl ) );
}

vo_https_stream::~vo_https_stream()
{	
}

VO_BOOL vo_https_stream::open( Persist_HTTP * ptr_persist , VO_BOOL bIsAsyncOpen )
{
	m_bAsyncMode = bIsAsyncOpen;
	m_bclose = VO_FALSE;

	m_ptr_persist = ptr_persist;

	if(m_ptr_persist && m_bsslinit)
	{
		((Persist_HTTPS*)m_ptr_persist)->setsslapi(m_ssl);
	}
	else
	{
		VOLOGE("ssl api not set yet");
		return VO_FALSE;
	}

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

VO_BOOL vo_https_stream::IsPersistSockAvailable()
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

VO_VOID vo_https_stream::setsslapi( vosslapi ssl)
{ 
	m_ssl = ssl; 
	m_bsslinit = VO_TRUE;
}


VO_BOOL vo_https_stream::resolve_url( VO_CHAR * url )
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

VO_BOOL vo_https_stream::IsDefaultPort()
{
	if( !strcmp( m_port , "443" ) )
	{
		return VO_TRUE;
	}

	return VO_FALSE;
}

VO_U32 vo_https_stream::generate_tunnel_request( VO_CHAR * ptr_request )
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