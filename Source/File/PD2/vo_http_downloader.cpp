//#include "StdAfx.h"
#include "vo_http_downloader.h"
#include "..\..\..\Common\NetWork\vo_socket.h"
#include "voString.h"
#include "voOSFunc.h"


#define MAXGETSIZE (1024*2)

vo_http_downloader::vo_http_downloader(void):
	m_is_server_canseek(VO_TRUE)
	,m_socket(-1)
	,m_stop_download(VO_FALSE)
	,m_is_downloading(VO_FALSE)
	,m_content_length(-1)
	,m_is_url_encoded(VO_FALSE)
	,m_duration(0)
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

	strcpy( m_url , str_url );
	return resolve_url( str_url );
}

VO_BOOL vo_http_downloader::start_download( VO_S64 download_pos/* = 0 */, VO_S64 download_size/* = DOWNLOADTOEND*/ , VO_BOOL is_async/* = VO_TRUE*/ )
{
	char str[512];
	//sprintf( str , "download_pos: %I64d\n" , download_pos );
	sprintf( str , "+++++++++++download_pos: %d\n" , (VO_S32)download_pos );
	m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , str );
	//OutputDebugStringA( str );

	m_current_physical_pos = download_pos;
	m_downloaded = 0;

	if( m_socket != -1 )
	{
		vo_socket_close( m_socket );
		m_socket = -1;
	}

	while( 1 )
	{
		m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)"+vo_socket_connect" );
		if( !vo_socket_connect( &m_socket , m_host , m_port ) )
		{
			//fwrite( "connect failed\r\n" , strlen("connect failed\r\n") , 1 , m_fp );
			m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)"connect failed!" );
			return VO_FALSE;
		}
		m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)"-vo_socket_connect" );

		char request[2048];
		memset( request , 0 , 2048 );

		VO_S32 request_size = perpare_request( request , download_pos , download_size );

		m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , request );

		if( -1 == vo_socket_send_safe( m_socket , (VO_PBYTE)request , request_size ) )
		{
			vo_socket_close( m_socket );
			m_socket  = -1;
			return VO_FALSE;
		}

		//fwrite( request , request_size , 1 , m_fp );

		m_download_start_time = voOS_GetSysTime();

		if( !get_response() )
		{
			//fwrite( m_response , strlen( m_response ) , 1 , m_fp );

			vo_socket_close( m_socket );
			m_socket  = -1;
			return VO_FALSE;
		}

		//voOS_Sleep( 100 );
		//m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , m_response );

		//fwrite( m_response , strlen( m_response ) , 1 , m_fp );

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

				return start_download( download_pos , download_size );
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
				start_thread();
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
	stop_thread();
}

VO_VOID vo_http_downloader::thread_function()
{
	m_ptr_callback->download_notify( DOWNLOAD_START , NULL );

	if( m_is_chunked )
	{
		m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)"Chunked!" );
		download_chunked();
	}
	else if( m_content_length <= 0 )
	{
		m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)"Endless Download!" );
		download_endless();
	}
	else
	{
		m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)"Normal!" );
		download_normal();
	}

	vo_socket_close( m_socket );
	m_socket = -1;

	m_ptr_callback->download_notify( DOWNLOAD_END , NULL );
	m_is_downloading = VO_FALSE;


	m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)"Download exit!" );
}

VO_VOID vo_http_downloader::stop_thread()
{
	m_stop_download = VO_TRUE;

	while( m_thread_handle )
		voOS_Sleep( 500 );

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
	//char * test_request = "GET /videoplayback?ip=0.0.0.0&sparams=id%2Cexpire%2Cip%2Cipbits%2Citag%2Calgorithm%2Cburst%2Cfactor%2Coc%3AU0dWSVJQVF9FSkNNNl9KSlhB&fexp=900028%2C904711&algorithm=throttle-factor&itag=18&ipbits=0&burst=40&sver=3&expire=1274256000&key=yt1&signature=06E472CB53DF30443AFF322A2749DF9D46742F6C.2147FE106A5BDE0C502A4BE5FC7CF26FEB56BD06&factor=1.25&id=0ebcc61e0986df2e&redirect_counter=1 HTTP/1.1\r\nHost: v5.cache3.c.youtube.com\r\nConnection: keep-alive\r\nUser-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US) AppleWebKit/532.5 (KHTML, like Gecko) Chrome/4.1.249.1064 Safari/532.5\r\nAccept: application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5\r\nAccept-Encoding: gzip,deflate,sdch\r\nAccept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\nCookie: VISITOR_INFO1_LIVE=hRebMrJjw6k; dkv=d3b272c9187e19ed232bf5e85246946ae3QEAAAAdGxpcGnVa6xLMA==; __utmz=27069237.1272268319.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none); __utma=27069237.109834039.1272268319.1272268319.1272268319.1\r\n\r\n";
	//strcpy( ptr_request , test_request );

	//return strlen( test_request );

	if( m_is_server_canseek && (!( download_pos == 0 && download_size == DOWNLOADTOEND )) )
	{
		VO_S64 download_start = download_pos;
		VO_S64 download_end = download_start + download_size - 1;

		if( strcmp( m_port , "80" ) )
		{
			if( download_size != DOWNLOADTOEND )
			{
				char * request_template = "GET %s HTTP/1.1\r\nHost: %s:%s\r\nRange: bytes=%lld-%lld\r\nUser-Agent: %s\r\nConnection: keep-alive\r\n\r\n";
				return sprintf( ptr_request , request_template , m_path , m_host , m_port , download_start , download_end , m_ptr_PD_param->mUserAgent );
			}
			else
			{
				char * request_template = "GET %s HTTP/1.1\r\nHost: %s:%s\r\nRange: bytes=%lld-\r\nUser-Agent: %s\r\nConnection: keep-alive\r\n\r\n";
				return sprintf( ptr_request , request_template , m_path , m_host , m_port , download_start , m_ptr_PD_param->mUserAgent );
			}
		}
		else
		{
			if( download_size != DOWNLOADTOEND )
			{
				char * request_template = "GET %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%lld-%lld\r\nUser-Agent: %s\r\nConnection: keep-alive\r\n\r\n";
				return sprintf( ptr_request , request_template , m_path , m_host , download_start , download_end , m_ptr_PD_param->mUserAgent );
			}
			else
			{
				char * request_template = "GET %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%lld-\r\nUser-Agent: %s\r\nConnection: keep-alive\r\n\r\n";
				return sprintf( ptr_request , request_template , m_path , m_host , download_start , m_ptr_PD_param->mUserAgent );
			}
		}
	}
	else
	{
		if( m_is_server_canseek )
		{
			if( strcmp( m_port , "80" ) )
			{
				char * request_template = "GET %s HTTP/1.1\r\nHost: %s:%s\r\nRange: bytes=0-\r\nUser-Agent: %s\r\nConnection: keep-alive\r\n\r\n";
				return sprintf( ptr_request , request_template , m_path , m_host , m_port , m_ptr_PD_param->mUserAgent );
			}
			else
			{
				char * request_template = "GET %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=0-\r\nUser-Agent: %s\r\nConnection: keep-alive\r\n\r\n";
				//char * request_template = "GET %s HTTP/1.1\r\nUser-Agent: HTC Streaming Player HTC / 1.0 / htc_ace / 2.2\r\nx-network-type: UMTS\r\nBandwidth: 1920000\r\nx-wap-profile: http://uaprof.vtext.com/pcd/adr63k/adr63k.xml\r\nHost: %s\r\n\r\n";
				//return sprintf( ptr_request , request_template , m_path , m_host );
				return sprintf( ptr_request , request_template , m_path , m_host , m_ptr_PD_param->mUserAgent );
			}
		}
		else
		{
			if( strcmp( m_port , "80" ) )
			{
				char * request_template = "GET %s HTTP/1.1\r\nHost: %s:%s\r\nUser-Agent: %s\r\nConnection: keep-alive\r\n\r\n";
				return sprintf( ptr_request , request_template , m_path , m_host , m_port , m_ptr_PD_param->mUserAgent );
			}
			else
			{
				char * request_template = "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nConnection: keep-alive\r\n\r\n";
				//char * request_template = "GET %s HTTP/1.1\r\nUser-Agent: HTC Streaming Player HTC / 1.0 / htc_ace / 2.2\r\nx-network-type: UMTS\r\nBandwidth: 1920000\r\nx-wap-profile: http://uaprof.vtext.com/pcd/adr63k/adr63k.xml\r\nHost: %s\r\n\r\n";
				//return sprintf( ptr_request , request_template , m_path , m_host );
				return sprintf( ptr_request , request_template , m_path , m_host , m_ptr_PD_param->mUserAgent );
			}
		}
	}

	return 0;
}

VO_BOOL vo_http_downloader::get_response()
{
	VO_U32 count = 0;
	memset( m_response , 0 , sizeof(m_response) );

	while( !m_stop_download )
	{
		VO_CHAR value;

		fd_set fds;
		timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 20000;

		FD_ZERO(&fds);
		FD_SET(m_socket,&fds);

		VO_S32 select_ret = select(m_socket + 1,&fds,NULL,NULL,&timeout);

		if( -1 == select_ret )
		{
			m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)"Socket Error");
			m_ptr_callback->download_notify( DOWNLOAD_ERROR , 0 );
			break;
		}
		else if( 0 == select_ret )
		{
			voOS_Sleep( 20 );
			continue;
		}

		////m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)"+vo_socket_recv");
		VO_S32 ret = vo_socket_recv( m_socket , (VO_PBYTE)&value , 1 );
		////m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)"-vo_socket_recv");

		if( ret != 1 )
			return VO_FALSE;

		m_response[count] = value;

		//fwrite( &value , 1 , 1 , m_fp );
		//VO_CHAR debug[2];
		//debug[0] = value;
		//debug[1] = '\0';
		////m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)debug);

		if( count >= 3 && m_response[count] == '\n' && m_response[count - 1] == '\r' 
			&& m_response[count - 2] == '\n' && m_response[count - 3] == '\r' )
			break;

		count++;

		if( count >= sizeof(m_response) )
			return VO_FALSE;
	}

	if( m_stop_download )
		return VO_FALSE;

	//strcpy( m_response , "HTTP/1.1 401 Authorization Required\r\nDate: Thu, 19 Aug 2010 10:07:48 GMT\r\nServer: Apache/2.2.11 (Fedora)\r\nWWW-Authenticate: Basic realm=\"Your Password please. To get it ask your TM.\"\r\nContent-Length: 401\r\nKeep-Alive: timeout=15, max=100\r\nConnection: Keep-Alive\r\nContent-Type: text/html; charset=iso-8859-1\r\n\r\n" );
	analyze_response();

	return VO_TRUE;
}

VO_BOOL vo_http_downloader::analyze_response()
{
	m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)m_response );
	m_status_code = -1;
	
	if( strstr( m_response , "HTTP/1.1" ) == m_response )
	{
		sscanf( m_response , "HTTP/1.1 %d " , &m_status_code );
	}
	else if( strstr( m_response , "HTTP/1.0" ) == m_response )
	{
		sscanf( m_response , "HTTP/1.0 %d " , &m_status_code );
	}

	VO_CHAR * ptr = strstr( m_response , "Transfer-Encoding: chunked" );

	if( ptr )
	{
		m_is_chunked = VO_TRUE;
	}
	else
	{
		m_is_chunked = VO_FALSE;

		ptr = strstr( m_response , "Content-Length:" );

		if( !ptr )
		{
			ptr = strstr( m_response , "CONTENT-LENGTH:" );

			m_content_length = 0;

			if(ptr)
			{
				sscanf( ptr , "CONTENT-LENGTH: %lld\r\n" , &m_content_length );
			}
		}
		else
		{
			m_content_length = 0;

			if(ptr)
			{
				sscanf( ptr , "Content-Length: %lld\r\n" , &m_content_length );
			}
		}

		m_filesize = m_content_length;
	}

	m_current_physical_pos = 0;

	ptr = strstr( m_response , "Content-Type:" );

	memset( m_mimetype , 0 , sizeof( m_mimetype ) );

	if( ptr )
	{
		sscanf( ptr , "Content-Type: %s\r\n" , m_mimetype );
	}

	ptr = strstr( m_response , "X-AvailableSeekRange:" );

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
		ptr = strstr( m_response , "Location:" );
		ptr = ptr + strlen( "Location:" );

		while( *ptr != 'h' )
			ptr++;

		VO_CHAR * ptr_temp = strchr( ptr , '\r' );

		memset( m_reparseurl , 0 , sizeof( m_reparseurl ) );
		strncpy( m_reparseurl , ptr , ptr_temp - ptr );
	}

	if( m_status_code == 206 )
	{
		ptr = strstr( m_response , "Content-Range: bytes" );
		VO_S64 start_pos,end_pos,filesize;

		if( ptr )
		{
			sscanf( ptr , "Content-Range: bytes%lld-%lld/%lld" , &start_pos , &end_pos , &filesize );
			m_current_physical_pos = start_pos;
			m_filesize = filesize;
		}
		else
		{
			ptr = strstr( m_response , "CONTENT-RANGE: bytes" );

			if( ptr )
			{
				sscanf( ptr , "CONTENT-RANGE: bytes%lld-%lld/%lld" , &start_pos , &end_pos , &filesize );
				m_current_physical_pos = start_pos;
				m_filesize = filesize;
			}
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
			m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)"Socket Error");
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

		if( ret == -1 )
		{
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
			m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)"Buffer Full!" );
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
			m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)"Socket Error");
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
			m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)"Buffer Full!" );
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

		//char str[512];
		//memset( str , 0 , 512 );
		//sprintf( str , "Chunked Size: %d" , chunk_size );
		////m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)str );

		VO_S32 buffertoread = chunk_size;

		while( buffertoread && !m_stop_download )
		{
			VO_S32 toread = buffertoread < MAXGETSIZE ? buffertoread : MAXGETSIZE;

			FD_ZERO(&fds);
			FD_SET(m_socket,&fds);

			VO_S32 select_ret = select(m_socket+1,&fds,NULL,NULL,&timeout);

			if( -1 == select_ret )
			{
				m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)"Socket Error");
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
				m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)"Buffer Full!" );
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
		else if( *ptr_in == ' ' )
			*ptr_out++ = '+';
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

// VO_BOOL vo_http_downloader::vo_socket_connect2( VO_S32 * ptr_socket , VO_CHAR * str_host , VO_CHAR * str_port )
// {
// 	addrinfo info;
// 	memset(&info, 0, sizeof(info));
// 	info.ai_family = AF_INET;
// 	info.ai_socktype = SOCK_STREAM;
// 	info.ai_protocol = 6;
// 
// 	addrinfo * ptr_ret;
// 
// 	//m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)"+getaddrinfo" );
// 	VO_S32 ret = getaddrinfo( str_host , str_port , &info , &ptr_ret );
// 	//m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)"-getaddrinfo" );
// 
// 	if( ret )
// 		return VO_FALSE;
// 
// 	addrinfo * ptr_entry = ptr_ret;
// 	*ptr_socket = -1;
// 	while( ptr_entry )
// 	{
// 		*ptr_socket = socket( ptr_entry->ai_family, ptr_entry->ai_socktype, ptr_entry->ai_protocol );
// 
// 		if( *ptr_socket == -1 )
// 		{
// 			ptr_entry = ptr_entry->ai_next;
// 			continue;
// 		}
// 
// 		//m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)"+connect" );
// 		if( ::vo_socket_connect_ansyc( *ptr_socket, ptr_entry ) )
// 		{
// 			//m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)"-connect" );
// 			break;
// 		}
// 		//m_ptr_callback->download_notify( DOWNLOAD_DEBUG_STRING , (VO_PTR)"--connect" );
// 
// 		vo_socket_close( *ptr_socket );
// 
// 		*ptr_socket = -1;
// 
// 		ptr_entry = ptr_entry->ai_next;
// 	}
// 
// 	freeaddrinfo( ptr_ret );
// 
// 	if( *ptr_socket == -1 )
// 		return VO_FALSE;
// 
// 	return VO_TRUE;
// }
