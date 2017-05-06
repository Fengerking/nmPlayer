#include "vo_http_downloader.h"
#include "../../../Common/NetWork/vo_socket.h"
#include "voString.h"
#include "voOSFunc.h"
#include "log.h"

#define MAXGETSIZE (1024*2)

char * strncchr( char * ptr_src , char chr )
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

char * strncstr( char * ptr_src , char * ptr_str )
{
	char * ptr = strncchr( ptr_src , ptr_str[0] );


	while( ptr )
	{
		if(
#ifdef WIN32
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

vo_http_downloader::vo_http_downloader(void):
	m_is_server_canseek(VO_TRUE)
	,m_socket(-1)
	,m_stop_download(VO_FALSE)
	,m_is_downloading(VO_FALSE)
	,m_content_length(-1)
	,m_duration(0)
	,m_is_url_encoded(VO_FALSE)
{
	vo_socket_init();
}

vo_http_downloader::~vo_http_downloader(void)
{
	if( m_socket != -1 )
	{
		vo_socket_close( m_socket );
		m_socket = -1;
	}
	vo_socket_uninit();
}

VO_BOOL vo_http_downloader::set_url( VO_CHAR * str_url , VOPDInitParam * ptr_PD_param )
{
	m_ptr_PD_param = ptr_PD_param;

	if( m_ptr_PD_param->nSeekUnsupported == 1 )
	{
		m_is_server_canseek = VO_FALSE;
	}

	memset( m_cookies , 0 , 1024 );
	if( m_ptr_PD_param->ptr_cookie )
	{
		if( sizeof(TCHAR) == 2 )
			wctomb( m_cookies , (wchar_t)m_ptr_PD_param->ptr_cookie );
		else
			strcpy( m_cookies , (VO_CHAR *)m_ptr_PD_param->ptr_cookie );
	}

	analyze_proxy();

	strcpy( m_url , str_url );
	strcpy( m_reparseurl , m_url );
	return resolve_url( str_url );
}

VO_BOOL vo_http_downloader::start_download( VO_S64 download_pos/* = 0 */, VO_S64 download_size/* = DOWNLOADTOEND*/ , VO_BOOL is_async/* = VO_TRUE*/ )
{
	PRINT_LOG( LOG_LEVEL_NORMAL , "+++++++++++download_pos: %lld" , download_pos );

	m_current_physical_pos = download_pos;
	m_downloaded = 0;

	if( m_socket != -1 )
	{
		vo_socket_close( m_socket );
		m_socket = -1;
	}

	while( 1 )
	{
		PRINT_LOG( LOG_LEVEL_ALLINFO , "+vo_socket_connect" );

		if( m_use_proxy )
		{
			PRINT_LOG( LOG_LEVEL_DETAIL , "Proxy host: %s Proxy Port: %s" , m_proxy_host , m_proxy_port );
			if( !vo_socket_connect( &m_socket , m_proxy_host , m_proxy_port ) )
			{
				PRINT_LOG( LOG_LEVEL_CRITICAL , "connect failed!" );
				return VO_FALSE;
			}
		}
		else
		{
			PRINT_LOG( LOG_LEVEL_DETAIL , "host: %s Port: %s" , m_host , m_port );
			if( !vo_socket_connect( &m_socket , m_host , m_port ) )
			{
				PRINT_LOG( LOG_LEVEL_CRITICAL , "connect failed!" );
				return VO_FALSE;
			}
		}

		PRINT_LOG( LOG_LEVEL_ALLINFO , "-vo_socket_connect" );

		char request[2048];
		memset( request , 0 , 2048 );

		VO_S32 request_size = perpare_request( request , download_pos , download_size );

		PRINT_LOG( LOG_LEVEL_IMPORTANT , request );

		if( -1 == vo_socket_send_safe( m_socket , (VO_PBYTE)request , request_size ) )
		{
			PRINT_LOG( LOG_LEVEL_CRITICAL , "Socket send failed!" );
			vo_socket_close( m_socket );
			m_socket  = -1;
			return VO_FALSE;
		}

		m_download_start_time = voOS_GetSysTime();

		if( !get_response() )
		{
			PRINT_LOG( LOG_LEVEL_CRITICAL , "Response Error!" );
			vo_socket_close( m_socket );
			m_socket  = -1;
			return VO_FALSE;
		}

		VO_BOOL ret = VO_FALSE;

		switch ( m_status_code / 100 )
		{
		case 2:
			{
				ret = VO_TRUE;
			}
			break;
		case 3:
			{
				if( !resolve_url( m_reparseurl ) )
					return VO_FALSE;

				VO_BOOL ret = start_download( download_pos , download_size );
				resolve_url( m_url );
				return ret;
			}
			break;
		case 4:
			{
				if( m_status_code == 416 )
					return VO_FALSE;
			}
		}

		if( ret )
		{
			m_stop_download = VO_FALSE;
			if( is_async )
			{
				m_is_downloading = VO_TRUE;
				begin();
			}
			else
			{
				m_is_downloading = VO_TRUE;
				thread_function();
			}
			return ret;
		}
		else
		{
			vo_socket_close( m_socket );
			m_socket = -1;

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
						return VO_FALSE;
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

VO_VOID vo_http_downloader::stop_download()
{
	stop();
}

VO_VOID vo_http_downloader::thread_function()
{
	m_ptr_callback->download_notify( DOWNLOAD_START , NULL );

	if( m_is_chunked )
	{
		PRINT_LOG( LOG_LEVEL_IMPORTANT , "Chunked!" );
		download_chunked();
	}
	else if( m_content_length <= 0 )
	{
		PRINT_LOG( LOG_LEVEL_IMPORTANT , "Endless Download!" );
		download_endless();
	}
	else
	{
		PRINT_LOG( LOG_LEVEL_IMPORTANT , "Normal!" );
		download_normal();
	}

	vo_socket_close( m_socket );
	m_socket = -1;

	m_ptr_callback->download_notify( DOWNLOAD_END , NULL );
	m_is_downloading = VO_FALSE;

	PRINT_LOG( LOG_LEVEL_NORMAL , "Download exit!" );
}

VO_VOID vo_http_downloader::stop()
{
	m_stop_download = VO_TRUE;

	vo_thread::stop();

	m_stop_download = VO_FALSE;
}

VO_BOOL vo_http_downloader::resolve_url( VO_CHAR * url )
{
	memset( m_host , 0 , sizeof(m_host) );
	memset( m_path , 0 , sizeof(m_path) );
	memset( m_port , 0 , sizeof(m_port) );

	VO_CHAR * ptr = strstr( url , "http://" );

	if( ptr )
	{
		ptr = ptr + strlen( "http://" );
	}
	else
	{
		ptr = url;
	}

	VO_CHAR * ptr_div = strchr( ptr , '/' );

	strncpy( m_host , ptr , ptr_div - ptr );
	strcpy( m_path , ptr_div );

	ptr_div = strchr( m_host , ':' );

	if( ptr_div )
	{
		ptr_div++;
		strcpy( m_port , ptr_div );

		ptr_div = strchr( m_host , ':' );
		*ptr_div = '\0';
	}
	else
	{
		strcpy( m_port , "80" );
	}

	return VO_TRUE;
}

VO_S32 vo_http_downloader::perpare_request( VO_CHAR * ptr_request , VO_S64 download_pos , VO_S64 download_size )
{
	VO_S32 size = 0;

	if( m_is_server_canseek && (!( download_pos == 0 && download_size == DOWNLOADTOEND )) )
	{
		VO_S64 download_start = download_pos;
		VO_S64 download_end = download_start + download_size - 1;

		if( strcmp( m_port , "80" ) )
		{
			if( download_size != DOWNLOADTOEND )
			{
				char * request_template = "GET %s HTTP/1.1\r\nHost: %s:%s\r\nRange: bytes=%lld-%lld\r\nUser-Agent: %s\r\nConnection: keep-alive\r\n";
				size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , m_port , download_start , download_end , m_ptr_PD_param->mUserAgent );
			}
			else
			{
				char * request_template = "GET %s HTTP/1.1\r\nHost: %s:%s\r\nRange: bytes=%lld-\r\nUser-Agent: %s\r\nConnection: keep-alive\r\n";
				size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path, m_host , m_port , download_start , m_ptr_PD_param->mUserAgent );
			}
		}
		else
		{
			if( download_size != DOWNLOADTOEND )
			{
				char * request_template = "GET %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%lld-%lld\r\nUser-Agent: %s\r\nConnection: keep-alive\r\n";
				size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , download_start , download_end , m_ptr_PD_param->mUserAgent );
			}
			else
			{
				char * request_template = "GET %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%lld-\r\nUser-Agent: %s\r\nConnection: keep-alive\r\n";
				size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , download_start , m_ptr_PD_param->mUserAgent );
			}
		}
	}
	else
	{
		if( strcmp( m_port , "80" ) )
		{
			char * request_template = "GET %s HTTP/1.1\r\nHost: %s:%s\r\nUser-Agent: %s\r\nConnection: keep-alive\r\n";
			size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , m_port , m_ptr_PD_param->mUserAgent );
		}
		else
		{
			char * request_template = "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nConnection: keep-alive\r\n";
			size = sprintf( ptr_request , request_template , m_use_proxy?m_reparseurl:m_path , m_host , m_ptr_PD_param->mUserAgent );
		}
	}

	if( strlen( m_cookies ) )
	{
		strcat( ptr_request , m_cookies );
		size = size + strlen( m_cookies );
	}

	strcat( ptr_request , "\r\n" );

	size = size + 2;

	return size;
}

VO_BOOL vo_http_downloader::get_response()
{
	VO_U32 count = 0;
	memset( m_response , 0 , sizeof(m_response) );

	while( 1 )
	{
		VO_CHAR value;

		VO_S32 ret = vo_socket_recv( m_socket , (VO_PBYTE)&value , 1 );

		if( ret != 1 )
			return VO_FALSE;

		m_response[count] = value;

		if( ( count >= 3 && m_response[count] == '\n' && m_response[count - 1] == '\r' 
			&& m_response[count - 2] == '\n' && m_response[count - 3] == '\r' ) ||
			( count >= 1 && m_response[count] == '\n' && m_response[count - 1] == '\n' ))
			break;

		count++;

		if( count >= sizeof(m_response) )
			return VO_FALSE;
	}

	analyze_response();

	return VO_TRUE;
}

VO_BOOL vo_http_downloader::analyze_response()
{
	PRINT_LOG( LOG_LEVEL_IMPORTANT , m_response );

	m_status_code = -1;

	if( strstr( m_response , "HTTP/1.1" ) == m_response )
	{
		sscanf( m_response , "HTTP/1.1 %d " , &m_status_code );
	}
	else if( strstr( m_response , "HTTP/1.0" ) == m_response )
	{
		sscanf( m_response , "HTTP/1.0 %d " , &m_status_code );
	}

	VO_CHAR * ptr = strncstr( m_response , "Transfer-Encoding: chunked" );

	if( ptr )
	{
		m_is_chunked = VO_TRUE;
	}
	else
	{
		m_is_chunked = VO_FALSE;

		ptr = strncstr( m_response , "Content-Length: " );

		if( ptr )
		{
			m_content_length = 0;

			ptr = ptr + strlen( "Content-Length: " );

			if(ptr)
			{
				sscanf( ptr , "%lld\r\n" , &m_content_length );
			}
		}

		m_filesize = m_content_length;
	}

	m_current_physical_pos = 0;

	ptr = strncstr( m_response , "Content-Type: " );

	memset( m_mimetype , 0 , sizeof( m_mimetype ) );

	if( ptr )
	{
		ptr = ptr + strlen( "Content-Type: " );
		sscanf( ptr , "%s\r\n" , m_mimetype );
	}

	ptr = strncstr( m_response , "X-AvailableSeekRange:" );

	if( ptr )
	{
		ptr = ptr + strlen( "X-AvailableSeekRange:" );
		VO_CHAR * ptr_end = strstr( ptr , "\r\n" );
		ptr = strchr( ptr , '-' );
		if( ptr && ptr < ptr_end )
		{
			ptr++;
			float duration;
			sscanf( ptr , "%f\r\n" , &duration );

			m_duration = duration * 1000;
		}
	}

	if( m_status_code / 100 == 3 )
	{
		ptr = strncstr( m_response , "Location:" );
		ptr = ptr + strlen( "Location:" );

		while( *ptr != 'h' )
			ptr++;

		VO_CHAR * ptr_temp = strchr( ptr , '\r' );

		memset( m_reparseurl , 0 , sizeof( m_reparseurl ) );
		strncpy( m_reparseurl , ptr , ptr_temp - ptr );
	}

	if( m_status_code == 206 )
	{
		ptr = strncstr( m_response , "Content-Range: bytes" );
		VO_S64 start_pos,end_pos,filesize;

		if( ptr )
		{
			ptr = ptr + strlen("Content-Range: ");
			sscanf( ptr , "bytes%lld-%lld/%lld" , &start_pos , &end_pos , &filesize );
			m_current_physical_pos = start_pos;
			m_filesize = filesize;
		}
	}

 	if( m_filesize <= 0 )
 	{
 		m_content_length = -1;
 		m_filesize = -1;
 	}


	if( m_status_code / 100 == 2 )
		m_ptr_callback->download_notify( DOWNLOAD_FILESIZE , &m_filesize );

	m_start_download_pos = m_current_physical_pos;

	return VO_TRUE;
}

VO_VOID vo_http_downloader::download_normal()
{
	m_downloaded = 0;

	fd_set fds;
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 20000;

	VO_U32 lastfile_time;

	while( m_downloaded != m_content_length && !m_stop_download )
	{
		FD_ZERO(&fds);
		FD_SET(m_socket,&fds);

		VO_S32 select_ret = select(m_socket + 1,&fds,NULL,NULL,&timeout);

		if( -1 == select_ret )
		{
			PRINT_LOG( LOG_LEVEL_CRITICAL , "Socket Error" );
			m_ptr_callback->download_notify( DOWNLOAD_ERROR , 0 );
			break;
		}
		else if( 0 == select_ret )
		{
			voOS_Sleep( 20 );
			continue;
		}

		VO_S32 downloadsize = ( m_content_length - m_downloaded ) < MAXGETSIZE ? ( m_content_length - m_downloaded ) : MAXGETSIZE;
		VO_CHAR buffer[MAXGETSIZE];

		VO_S32 ret = vo_socket_recv( m_socket , (VO_PBYTE)buffer , downloadsize );
		
		//if( m_downloaded > m_content_length )
			//ret = -1;

		if( ret == -1 )
		{
			PRINT_LOG( LOG_LEVEL_CRITICAL , "Socket May closed by server!" );
			m_ptr_callback->download_notify( DOWNLOAD_ERROR , 0 );
			break;
		}
		else if( ret == 0 )
		{
			break;
		}
		else
		{
			lastfile_time = voOS_GetSysTime();
			m_downloaded = m_downloaded + ret;
		}

		if( CALLBACK_BUFFER_FULL == m_ptr_callback->received_data( m_current_physical_pos , (VO_PBYTE)buffer , ret ) )
		{
			PRINT_LOG( LOG_LEVEL_IMPORTANT , "Buffer Full!" );
			break;
		}

		m_current_physical_pos += ret;

		//voOS_Sleep( 10 );
	}
}

VO_VOID vo_http_downloader::download_endless()
{
	m_downloaded = 0;

	fd_set fds;
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 20000;

	VO_U32 lastfile_time;

	while( !m_stop_download )
	{
		FD_ZERO(&fds);
		FD_SET(m_socket,&fds);

		VO_S32 select_ret = select(m_socket + 1,&fds,NULL,NULL,&timeout);

		if( -1 == select_ret )
		{
			PRINT_LOG( LOG_LEVEL_CRITICAL , "Socket Error" );
			m_ptr_callback->download_notify( DOWNLOAD_ERROR , 0 );
			break;
		}
		else if( 0 == select_ret )
		{
			voOS_Sleep( 20 );
			continue;
		}

		VO_CHAR buffer[MAXGETSIZE];

		VO_S32 ret = vo_socket_recv( m_socket , (VO_PBYTE)buffer , MAXGETSIZE );

		if( ret == -1 )
		{
			PRINT_LOG( LOG_LEVEL_CRITICAL , "Socket May closed by server!" );
			m_ptr_callback->download_notify( DOWNLOAD_ERROR , 0 );
			break;
		}
		else if( ret == 0 )
		{
			break;
		}
		else
		{
			lastfile_time = voOS_GetSysTime();
			m_downloaded = m_downloaded + ret;
		}

		if( CALLBACK_BUFFER_FULL == m_ptr_callback->received_data( m_current_physical_pos , (VO_PBYTE)buffer , ret ) )
		{
			PRINT_LOG( LOG_LEVEL_IMPORTANT , "Buffer Full!" );
			break;
		}

		m_current_physical_pos += ret;
	}

	if( !m_stop_download )
		m_ptr_callback->download_notify( DOWNLOAD_TO_FILEEND , (VO_PTR)&m_current_physical_pos );
}

VO_VOID vo_http_downloader::download_chunked()
{
	VO_S32 chunk_size = 0;
	fd_set fds;
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 20000;

	VO_U32 lastfile_time;
	VO_BOOL isbufferfull = VO_FALSE;

	do
	{
		VO_BYTE buffer[MAXGETSIZE];
		memset( buffer , 0 , MAXGETSIZE );
		VO_S32 pos = 0;

		while( 1 && pos < MAXGETSIZE )
		{
			VO_S32 ret = vo_socket_recv( m_socket , buffer + pos , 1 );

			if( ret == -1 )
				return;

			if( pos >= 1 && buffer[pos] == '\n' && buffer[pos - 1] == '\r' )
			{
				buffer[pos - 1] = '\0';
				break;
			}

			pos++;
		}

		sscanf( (VO_CHAR*)buffer , "%x" , &chunk_size );

		PRINT_LOG( LOG_LEVEL_IMPORTANT , "Chunked Size: %d" , chunk_size  );

		VO_S32 buffertoread = chunk_size;

		while( buffertoread && !m_stop_download )
		{
			VO_S32 toread = buffertoread < MAXGETSIZE ? buffertoread : MAXGETSIZE;

			FD_ZERO(&fds);
			FD_SET(m_socket,&fds);

			VO_S32 select_ret = select(m_socket+1,&fds,NULL,NULL,&timeout);

			if( -1 == select_ret )
			{
				PRINT_LOG( LOG_LEVEL_CRITICAL , "Socket Error" );
				m_ptr_callback->download_notify( DOWNLOAD_ERROR , 0 );
				break;
			}
			else if( 0 == select_ret )
			{
				voOS_Sleep( 20 );
				continue;
			}

			VO_S32 ret = vo_socket_recv( m_socket , (VO_PBYTE)buffer , toread );

			if( ret == -1 )
			{
				PRINT_LOG( LOG_LEVEL_CRITICAL , "Socket May closed by server!" );
				m_ptr_callback->download_notify( DOWNLOAD_ERROR , 0 );
				break;
			}
			else if( ret == 0 )
			{
				break;
			}
			else
			{
				lastfile_time = voOS_GetSysTime();
				buffertoread = buffertoread - ret;
			}

			if( CALLBACK_BUFFER_FULL == m_ptr_callback->received_data( m_current_physical_pos , (VO_PBYTE)buffer , ret ) )
			{
				isbufferfull = VO_TRUE;
				PRINT_LOG( LOG_LEVEL_IMPORTANT , "Buffer Full!" );
				return;
			}

			m_current_physical_pos += ret;
		}

		if( !m_stop_download )
		{
			vo_socket_recv( m_socket , buffer , 2 );
		}

	} while ( chunk_size && !m_stop_download );

	if( !m_stop_download )
		m_ptr_callback->download_notify( DOWNLOAD_TO_FILEEND , (VO_PTR)&m_current_physical_pos );
}

VO_BOOL vo_http_downloader::is_downloading()
{
	return m_is_downloading;
}

VO_VOID vo_http_downloader::get_dowloadinfo( VO_DOWNLOAD_INFO * ptr_info )
{
	VO_S64 timelost = voOS_GetSysTime() - m_download_start_time;
	if( timelost == 0 )
		timelost = 1;

	ptr_info->cur_download_pos = m_current_physical_pos;
	ptr_info->start_download_pos = m_start_download_pos;
	ptr_info->download_size = m_content_length;

	VO_S64 delta_pos = m_current_physical_pos - m_start_download_pos;

	if( timelost )
		ptr_info->average_speed = ( m_current_physical_pos - m_start_download_pos ) * 1000. / timelost;
	else
		ptr_info->average_speed = 0;

	if( delta_pos < 10240 || timelost < 1000 )
		ptr_info->is_speed_reliable = VO_FALSE;
	else
		ptr_info->is_speed_reliable = VO_TRUE;

	if( ptr_info->average_speed <= 5000 )
		ptr_info->is_speed_reliable = VO_FALSE;
}

VO_VOID vo_http_downloader::analyze_proxy()
{
	m_use_proxy = VO_FALSE;
	memset( m_proxy_port , 0 , sizeof( m_proxy_port ) );
	memset( m_proxy_host , 0 , sizeof( m_proxy_host ) );

	if( strlen( m_ptr_PD_param->mProxyName ) )
	{
		PRINT_LOG( LOG_LEVEL_NORMAL , "Using Proxy: %s" , m_ptr_PD_param->mProxyName );

		m_use_proxy = VO_TRUE;

		VO_CHAR proxy[1024];
		memset( proxy , 0 , 1024 );

		if( sizeof(TCHAR) == 2 )
			wctomb( proxy , (wchar_t)m_ptr_PD_param->mProxyName );
		else
			strcpy( proxy , (VO_CHAR*)m_ptr_PD_param->mProxyName );

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
	char temp[1024];

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