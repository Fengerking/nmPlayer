#include<stdlib.h>

#include "vo_http_stream.h"
#include "voString.h"
#include "voOSFunc.h"
#include "fortest.h"
#include "vo_mem_stream.h"
#include "voLog.h"

#include "vostream_digest_auth.h"
#include "../../Common/NetWork/vo_socket.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


#define MAXGETSIZE (1024*10)

#define MAXSOCKETREUSETIME 7

VO_CHAR vo_http_stream::m_cookies[1024];
voCMutex vo_http_stream::m_cookieslock;
VO_U32  vo_http_stream::m_ulDigestFlag = 0;

VO_CHAR vo_http_stream::m_strUserName[128];
VO_U32  vo_http_stream::m_ulstrUserNameLen = 0;

VO_CHAR vo_http_stream::m_strPwd[128];
VO_U32  vo_http_stream::m_ulstrPwdLen = 0;

VO_CHAR vo_http_stream::m_strQop[128];
VO_U32  vo_http_stream::m_ulstrQopLen = 0;

VO_CHAR vo_http_stream::m_strNonce[128];
VO_U32  vo_http_stream::m_ulstrNonceLen = 0;

VO_CHAR vo_http_stream::m_strMethod[128];
VO_U32  vo_http_stream::m_ulstrMethodLen = 0;

VO_CHAR vo_http_stream::m_strRealm[128];
VO_U32  vo_http_stream::m_ulstrRealmLen = 0;

VO_CHAR vo_http_stream::m_strURI[1024];
VO_U32  vo_http_stream::m_ulstrURILen = 0;

VO_CHAR vo_http_stream::m_strOpaque[128];
VO_U32  vo_http_stream::m_ulstrOpaqueLen;

VO_U32  vo_http_stream::m_ulClientNonceCount = 0;
VO_U32  vo_http_stream::m_ulClientNonce1 = 0;
VO_U32  vo_http_stream::m_ulClientNonce2 = 0;


VO_CHAR vo_http_stream::m_strResponse[128];
VO_U32  vo_http_stream::m_ulstrResponseLen = 0;


Persist_HTTP::Persist_HTTP()
{
    socket = 0;
    usedtimes = 0;
    deadtime = 0;
    maxusetime = MAXSOCKETREUSETIME;
}

Persist_HTTP::~Persist_HTTP()
{
    if( socket )
        vo_socket_close( socket );
}

VO_VOID Persist_HTTP::reset()
{
    socket = 0;
    usedtimes = 0;
    deadtime = 0;
    brefreshbyserver = VO_FALSE;
    maxusetime = MAXSOCKETREUSETIME;
}

vo_http_stream::vo_http_stream()
{
    m_socket = -1;
	m_ptr_buffer_stream = 0;
	m_download_mode = DOWNLOAD2NONE;
	m_stop_download = VO_FALSE;
	m_download_starttime = -1;
	m_download_endtime = -1;
	m_content_length = -1;
	m_ptr_persist = 0;
	m_errorcode = 0;

	vo_socket_init();
}

vo_http_stream::~vo_http_stream(void)
{
	close();
	vo_socket_uninit();
}

VO_BOOL vo_http_stream::open( VO_CHAR * url , DownloadMode mode , VO_PBYTE ptr_key , VO_PBYTE ptr_iv , ProtectionType drm_type , DRM_Callback * ptr_drm_engine , void * drm_handle )
{
	close();

	m_download_mode = mode;




	VOLOGI("the input url:%s",  url);
    
	switch( m_download_mode )
	{
	case DOWNLOAD2MEM:
		m_ptr_buffer_stream = new vo_mem_stream();
		if( !((vo_mem_stream*)m_ptr_buffer_stream)->open() )
			return VO_FALSE;
		break;
	case DOWNLOAD2FILE:
		break;
	default:
		break;
	}

	VO_BOOL ret;

	VOLOGI("the input url:%s",  url);
    memset(m_reparseurl, 0, 1024);

	ret = init( url );

    if( !ret )
    {
        if( m_ptr_persist && m_ptr_persist->socket && m_ptr_persist->socket != -1 )
            vo_socket_close( m_ptr_persist->socket );

        if( m_ptr_persist )
            m_ptr_persist->reset();

        ret = init( url );
    }

// 	while( !(ret = init( url )) && trycounts < 3 )
// 	{
// 		trycounts++;
// 		voOS_Sleep( 500 );
// 	}

	return ret;
}

VO_BOOL vo_http_stream::persist_open( VO_CHAR * url , DownloadMode mode , Persist_HTTP * ptr_persist , VO_PBYTE ptr_key , VO_PBYTE ptr_iv , ProtectionType drm_type , DRM_Callback * ptr_drm_engine , void * drm_handle )
{
    m_ptr_persist = ptr_persist;
    
    return open( url , mode , ptr_key , ptr_iv , drm_type , ptr_drm_engine , drm_handle );
}

VO_VOID vo_http_stream::close()
{
	VOLOGI( "+stop_download_thread");
	stop_download_thread();
	VOLOGI( "-stop_download_thread");
	switch( m_download_mode )
	{
	case DOWNLOAD2MEM:
		if( m_ptr_buffer_stream )
			((vo_mem_stream*)m_ptr_buffer_stream)->close();
		break;
	case DOWNLOAD2FILE:
		break;
	default:
		break;
	}

    VOLOGI( "closed buffer stream");

	m_is_download_ok = VO_FALSE;
	if( m_ptr_buffer_stream )
		delete m_ptr_buffer_stream;
	m_ptr_buffer_stream = 0;
	m_download_mode = DOWNLOAD2NONE;
    m_errorcode = 0;

	VOLOGI( "+uninit");
	uninit();
	VOLOGI( "-uninit");
}

VO_S64 vo_http_stream::read( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	VO_BOOL isdownloadok = m_is_download_ok;

	VO_S64 ret = m_ptr_buffer_stream->read( ptr_buffer , buffer_size );

	if( ret == 0 && ( isdownloadok /*|| m_downloadthread == 0*/ ) )
		return -1;
	else if( ret == 0 && !isdownloadok )
		return -2;
	else
	{
		return ret;
	}
}

VO_S64 vo_http_stream::write( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	return m_ptr_buffer_stream->write( ptr_buffer , buffer_size );
}

VO_BOOL vo_http_stream::seek( VO_S64 pos , vo_stream_pos stream_pos  )
{
	return m_ptr_buffer_stream->seek( pos , stream_pos );
}

VO_S64 vo_http_stream::append( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	return m_ptr_buffer_stream->append( ptr_buffer , buffer_size );
}

VO_BOOL vo_http_stream::init( VO_CHAR * url )
{
    char   stroutput[256] = {0};
    char   strTemp[1024] = {0};
	uninit();

	m_stop_download = VO_FALSE;
	if( VO_FALSE == resolve_url( url ) )
		return VO_FALSE;

	if( m_ptr_persist && m_ptr_persist->socket && m_ptr_persist->deadtime > voOS_GetSysTime() && m_ptr_persist->usedtimes < m_ptr_persist->maxusetime )
    {
        m_socket = m_ptr_persist->socket;
        m_ptr_persist->usedtimes++;
    }
    else
    {
        VOLOGI( "Start Connect!");

        VO_S32 trycount = 0;
        while( !vo_socket_connect( &m_socket , m_host , m_port ) )
        {
            output( "Socket Connect Fail!" , __LINE__ );
            trycount++;

            if( trycount > 2 )
            {
                m_errorcode = CONNECTION_ERROR;
                return VO_FALSE;
            }
        }
        VOLOGI( "End Connect");


        if( m_ptr_persist )
        {
            if( m_ptr_persist->socket && m_ptr_persist->socket != -1 )
                vo_socket_close( m_ptr_persist->socket );

            m_ptr_persist->reset();

            m_ptr_persist->socket = m_socket;
            m_ptr_persist->deadtime = voOS_GetSysTime() + 7000;
            m_ptr_persist->usedtimes++;
        }
    }

	//output( "socket created!" , m_socket );


	VO_CHAR request[2048];
	memset( request , 0 , sizeof(request) );
	VO_U32 request_size = perpare_http_request( request );
	VOLOGI( "the request:%s", request);

	vo_socket_send( m_socket , (VO_PBYTE)request , request_size );

	m_download_starttime = voOS_GetSysTime();

	if( VO_FALSE == get_response() )
	{
		VOLOGI( "get_response fail!");
		VOLOGI( "the response:%s", m_response);
        if( m_ptr_persist )
            m_ptr_persist->maxusetime = 0;
		return VO_FALSE;
	}

	//output( m_response , __LINE__ );

	if( m_status_code / 100 != 2 )
	{
	    memset(stroutput, 0, 256);
		sprintf(stroutput, "status_code:%d", m_status_code);
        VOLOGI("the stroutput:%s", stroutput);
		
		if( m_status_code / 100 == 3 )
		{
            if( strlen( m_reparseurl ) > 0 )
			{
				if( m_ptr_persist )
                {            
					m_ptr_persist->maxusetime = 0;
                    m_ptr_persist->reset();
                }

                memset(strTemp, 0, 1024);
                strcpy(strTemp, m_reparseurl);

                if(m_status_code%100 != 1)
                {
                    memset(m_reparseurl, 0, 1024);
                }
                
			    return init( strTemp );
			}
            else
                return VO_FALSE;
		}
		else
		{
		    if(m_status_code / 100 == 4)
		    {
				if(m_status_code % 100 == 1)
				{
				   // goto RETRY;
				   return init( url );
				}
		    }
			VOLOGI("the request:%s", request);
			VOLOGI("the response:%s",  m_response);
            m_errorcode = m_status_code;
			return VO_FALSE;
		}
	}

	begin();

	return VO_TRUE;
}

void vo_http_stream::uninit()
{
	if( m_socket != -1 && !m_ptr_persist )
	{
		//output( "close socket!" , m_socket );
		vo_socket_close( m_socket );
		m_socket = -1;
	}

	stop_download_thread();
	//m_stop_download = VO_FALSE;
}

VO_BOOL vo_http_stream::resolve_url( VO_CHAR * url )
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

void vo_http_stream::thread_function()
{
	set_threadname((char *) "HTTP Download" );
	download();
}

void vo_http_stream::download()
{
	VO_U32 start = voOS_GetSysTime();

	if( m_is_chunked )
	{
		VOLOGI("It is Chunked Mode Download!");
		download_chunked();
	}
	else
	{
		if ( m_content_length <= 0 )
		{
			VOLOGI( "It is endless download!");
			download_endless();
		}
		else
		{
			VOLOGI( "It is normal download!");
			download_normal();
		}
	}

	write_eos();

	VO_U32 end = voOS_GetSysTime();

	//VOLOGI("http Download Ok!");

	char debug[250];
	sprintf( debug , "Download OK! Download %lld B, Cost %f Seconds!\n" , m_content_length , ( end - start ) / 1000. );
	VOLOGI("debug:%s", debug);

	//OutputDebugStringA( "Download OK!" );

	if( !m_stop_download )
	{
		m_is_download_ok = VO_TRUE;
	}

	//output( "+m_downloadthread = 0" , __LINE__ );
	//m_downloadthread = 0;
	//output( "-m_downloadthread = 0" , __LINE__ );
}

void vo_http_stream::download_normal()
{
	VO_S64 downloaded = 0;

	fd_set fds;
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 20000;

	m_totaldownload = 0;

	while( ( downloaded != m_content_length ) && !m_stop_download )
	{
		FD_ZERO(&fds);
		FD_SET(m_socket,&fds);

		VO_S32 select_ret = select( m_socket + 1 ,&fds,NULL,NULL,&timeout);

		if( -1 == select_ret )
		{
			VOLOGI( "Socket Error");
			break;
		}
		else if( 0 == select_ret )
		{
			voOS_Sleep( 20 );
			continue;
		}

		VO_S32 downloadsize = ( m_content_length - downloaded ) < MAXGETSIZE ? ( m_content_length - downloaded ) : MAXGETSIZE;
		VO_CHAR buffer[MAXGETSIZE];

		VO_S32 ret = vo_socket_recv( m_socket , (VO_PBYTE)buffer , downloadsize );//recv( m_socket , (VO_PBYTE)buffer , downloadsize , 0 );

		if( ret == -1 )
		{
			break;
		}
		else if( ret == 0 )
		{
			break;
		}
		else
		{
			m_totaldownload = downloaded = downloaded + ret;
		}

		m_download_endtime = voOS_GetSysTime();

		VO_S64 appended = append( (VO_PBYTE)buffer , ret );

		if( appended == -1 )
			break;
	}

	if( m_stop_download )
    {
        if( m_ptr_persist )
            m_ptr_persist->maxusetime = 0;
		VOLOGI( "force stop download!");
    }


	output( "Total Donwload: " , (VO_S32)m_totaldownload );
	m_download_endtime = voOS_GetSysTime();
}

void vo_http_stream::download_chunked()
{
	VO_S32 chunk_size = 0;
	fd_set fds;
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 20000;
	int   iChunkSize= 0;

	VO_S64 filesize = 0;

	VO_S64 appended = 0;

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

		sscanf( (VO_CHAR*)buffer , "%x" , &iChunkSize );
		chunk_size = iChunkSize;

		filesize = filesize + chunk_size;

		VO_S32 buffertoread = chunk_size;

		while( buffertoread && !m_stop_download )
		{
			VO_S32 toread = buffertoread < MAXGETSIZE ? buffertoread : MAXGETSIZE;

			FD_ZERO(&fds);
			FD_SET(m_socket,&fds);

			VO_S32 select_ret = select(m_socket+1,&fds,NULL,NULL,&timeout);

			if( -1 == select_ret )
			{
				break;
			}
			else if( 0 == select_ret )
			{
				continue;
			}

			VO_S32 ret = vo_socket_recv( m_socket , (VO_PBYTE)buffer , toread );

			if( ret == -1 )
			{
				break;
			}
			else if( ret == 0 )
			{
				break;
			}
			else
			{
				buffertoread = buffertoread - ret;
			}

			appended = append( (VO_PBYTE)buffer , ret );

			if( appended == -1 )
				break;
		}

		if( appended == -1 )
			break;

		if( !m_stop_download )
		{
			vo_socket_recv( m_socket , buffer , 2 );
		}

	} while ( chunk_size && !m_stop_download );

	if( !m_stop_download )
		m_content_length = filesize;

    if( m_stop_download )
    {
        if( m_ptr_persist )
            m_ptr_persist->maxusetime = 0;
    }

	m_download_endtime = voOS_GetSysTime();
}

void vo_http_stream::download_endless()
{
	fd_set fds;
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 20000;

	VO_S64 filesize = 0;

	while( !m_stop_download )
	{
		FD_ZERO(&fds);
		FD_SET(m_socket,&fds);

		VO_S32 select_ret = select(m_socket + 1,&fds,NULL,NULL,&timeout);

		if( -1 == select_ret )
		{
			break;
		}
		else if( 0 == select_ret )
		{
			continue;
		}

		VO_CHAR buffer[MAXGETSIZE];

		VO_S32 ret = vo_socket_recv( m_socket , (VO_PBYTE)buffer , MAXGETSIZE );

		if( ret == -1 )
		{
			break;
		}
		else if( ret == 0 )
		{
			break;
		}
		else
		{
			filesize = filesize + ret;
		}

		VO_S64 appended = append( (VO_PBYTE)buffer , ret );

		if( appended == -1 )
			break;
	}

	if( !m_stop_download )
		m_content_length = filesize;

    if( m_stop_download )
    {
        if( m_ptr_persist )
            m_ptr_persist->maxusetime = 0;
    }

	m_download_endtime = voOS_GetSysTime();
}

VO_U32 vo_http_stream::perpare_http_request( VO_CHAR * ptr_request )
{
    VO_U32 size = 0;
	VO_CHAR   strNumber[32] = {0};
	if( strcmp( m_port , "80" ) )
	{
        char * request_template;
        if( m_ptr_persist )
		    request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s:%s\r\nConnection: keep-alive\r\n";
        else
            request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s:%s\r\nConnection: close\r\n";
		size = sprintf( ptr_request , request_template , m_path , m_host , m_port );
	}
	else
	{
        char * request_template;
        if( m_ptr_persist )
            request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\n";
        else
            request_template = (char *)"GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n";
		size = sprintf( ptr_request , request_template , m_path , m_host );
	}

    if(m_ulDigestFlag != 0)
    {
		strcat( ptr_request , "Authorization: Digest " );
		size = size + strlen("Authorization: Digest " );

		strcat( ptr_request , "username=\"");
		strcat( ptr_request , m_strUserName);
		strcat( ptr_request , "\",");
		strcat( ptr_request , " " );

		size = size + strlen("username=\"") + strlen(m_strUserName) + strlen("\",") + strlen(" ");

		strcat( ptr_request , "realm=\"");
		strcat( ptr_request , m_strRealm);
		strcat( ptr_request , "\",");
        strcat( ptr_request , " " );

		size = size + strlen("realm=\"") + strlen(m_strRealm) + strlen("\",") + strlen(" ");

	    strcat( ptr_request , "nonce=\"");
	    strcat( ptr_request , m_strNonce);
	    strcat( ptr_request , "\",");
	    strcat( ptr_request , " " );
		size = size + strlen("nonce=\"") + strlen(m_strNonce) + strlen("\",") + strlen(" ");
		
		strcat( ptr_request , "uri=\"");
		strcat( ptr_request , m_strURI);
		strcat( ptr_request , "\",");
		strcat( ptr_request , " " );
		size = size + strlen("uri=\"") + strlen(m_strURI) + strlen("\",") + strlen(" ");


	    strcat( ptr_request , "response=\"");
	    strcat( ptr_request , m_strResponse);
 	    strcat( ptr_request , "\",");
	    strcat( ptr_request , " " );
		size = size + strlen("response=\"") + strlen(m_strResponse) + strlen("\",") + strlen(" ");		

		if(m_ulstrQopLen != 0)
		{
	        strcat( ptr_request , "qop=");
	        strcat( ptr_request , m_strQop);
	        strcat( ptr_request , ",");
	        strcat( ptr_request , " " );
			size = size + strlen("qop=") + strlen(m_strQop) + strlen(",") + strlen(" ");			
		}

        memset(strNumber, 0, 32);
		sprintf(strNumber, "%08x", m_ulClientNonceCount);
        strcat( ptr_request , "nc=");
        strcat( ptr_request , strNumber);
        strcat( ptr_request , ",");
        strcat( ptr_request , " " );
		size = size + strlen("nc=") + strlen(strNumber) + strlen(",") + strlen(" ");


        memset(strNumber, 0, 32);
		sprintf(strNumber, "%08x%08x", m_ulClientNonce1, m_ulClientNonce2);
        strcat( ptr_request , "cnonce=\"");
        strcat( ptr_request , strNumber);
        //strcat( ptr_request , "3aabe96f6032af9c");
        strcat( ptr_request , "\"\r\n" );		
		size = size + strlen("cnonce=\"") + strlen(strNumber) + strlen("\"\r\n");

	
		if(m_ulstrOpaqueLen != 0)
		{
	        strcat( ptr_request , "opaque=\"");
	        strcat( ptr_request , m_strOpaque);
 	        strcat( ptr_request , "\",");
	        strcat( ptr_request , "\r\n" );
			size = size + strlen("opaque=\"") + strlen(m_strOpaque) + strlen("\",") + strlen("\r\n");
		}
    }

    {
        voCAutoLock lock(&m_cookieslock);

        if( strlen( m_cookies ) )
        {
            strcat( ptr_request , "Cookie: " );
            strcat( ptr_request , m_cookies );
            strcat( ptr_request , "\r\n" );

            size = size + strlen( "Cookie: " ) + strlen( m_cookies ) + strlen( "\r\n" );
        }
    }

	strcat( ptr_request ,(char *) "User-Agent: " );
	strcat( ptr_request , (char *) "VisualOn PD Player(Linux;Android)" );
	strcat( ptr_request ,(char *)  "\r\n" );
	size = size + strlen( (char *) "User-Agent: " ) + strlen((char *)  "\r\n" ) + strlen( (char *) "VisualOn PD Player(Linux;Android)" ) ;


    strcat( ptr_request , "\r\n" );

    size = size + strlen( "\r\n" );

    return size;
}

VO_BOOL vo_http_stream::get_response()
{
	VO_U32 count = 0;
	memset( m_response , 0 , sizeof(m_response) );

    fd_set fds;
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 20000;

	VO_U32 get_response_start = voOS_GetSysTime();

	while( 1 )
	{
        VO_U32 get_response_cost = voOS_GetSysTime() - get_response_start;

        if( get_response_cost > 5000 )
        {
            m_errorcode = GET_RESPONSE_TIMEOUT;
            output( "Get Response Time Out!" , get_response_cost );
            return VO_FALSE;
        }

        FD_ZERO(&fds);
        FD_SET(m_socket,&fds);

        VO_S32 select_ret = select(m_socket + 1,&fds,NULL,NULL,&timeout);

        if( -1 == select_ret )
        {
            m_errorcode = CONNECTION_ERROR;
            output("socket error" , __LINE__);
            return VO_FALSE;
        }
        else if( 0 == select_ret )
        {
            continue;
        }

		VO_CHAR value;

		VO_S32 ret = vo_socket_recv( m_socket , (VO_PBYTE)&value , 1 );

		if( ret != 1 )
		{
            m_errorcode = CONNECTION_ERROR;
			output("socket error" , __LINE__);
			return VO_FALSE;
		}

		m_response[count] = value;

		if( count >= 3 && m_response[count] == '\n' && m_response[count - 1] == '\r' 
			&& m_response[count - 2] == '\n' && m_response[count - 3] == '\r' )
			break;

		count++;

		if( count >= sizeof(m_response) )
		{
            m_errorcode = RESPONSE_TOOLARGE;
			VOLOGI("response too large");
			return VO_FALSE;
		}
	}

    
	VOLOGI("the response:%s", m_response);
	analyze_response();

	return VO_TRUE;
}

VO_BOOL vo_http_stream::analyze_response()
{
    int   iGetValue = 0;
	m_status_code = -1;

	if( strstr( m_response , "HTTP/1.1" ) == m_response )
	{
		sscanf( m_response , "HTTP/1.1 %d " , &iGetValue );
		m_status_code = iGetValue;
	}
	else if( strstr( m_response , "HTTP/1.0" ) == m_response )
	{
		sscanf( m_response , "HTTP/1.0 %d " , &iGetValue );
		m_status_code = iGetValue;
	}

	VOLOGI( "HTTP Status Code: %d" , m_status_code );
    if(m_status_code == 401)
    {
        DoTransactionForDigest();
    }

    VO_CHAR * ptr = strstr( m_response , "Transfer-Encoding:" );

    if( ptr )
    {
        ptr = ptr + strlen("Transfer-Encoding:");
        while( *ptr == ' '  )
            ptr++;

        if( strstr( ptr , "chunked" ) != ptr )
            ptr = 0;
    }

	if( ptr )
	{
		m_is_chunked = VO_TRUE;
	}
	else
	{
		m_is_chunked = VO_FALSE;

		ptr = strstr( m_response , "Content-Length:" );

		if(ptr)
		{
			m_content_length = 0;
			sscanf( ptr , "Content-Length: %d\r\n" , &iGetValue );
			m_content_length = iGetValue;
            if(m_content_length == 0)
            {
                VOLOGI("the content length is 0, reset it to -1");
                m_content_length = -1;
            }
            else
            {
			    VOLOGI( "Content-Length:%d " , (VO_S32)m_content_length );
            }
		}
	}

	ptr = strstr( m_response , "Content-Type:" );

	memset( m_mimetype , 0 , sizeof( m_mimetype ) );

	if( ptr )
	{
		sscanf( ptr , "Content-Type: %s\r\n" , m_mimetype );
	}

    if( m_ptr_persist && !m_ptr_persist->brefreshbyserver )
    {
        m_ptr_persist->brefreshbyserver = VO_TRUE;

        ptr = strstr( m_response , "Keep-Alive:" );

        if( ptr )
        {
            char temp[1024];
            memset( temp , 0 , 1024 );

            sscanf( ptr , "Keep-Alive: %s\r\n" , temp );

            char value[100];
            memset( value , 0 , 100 );

            ptr = strstr( temp , "timeout=" );

            if( ptr )
            {
                ptr = ptr + strlen( "timeout=" );
                while( *ptr == ' ' )
                    ptr++;

                strcpy( value , ptr );

                ptr = strchr( value , ',' );

                if( ptr )
                    *ptr = '\0';

                VO_S32 lasttime = atoi(value) * 1000;

                if( lasttime >= 20000 )
                    lasttime = 20 * 1000;

                m_ptr_persist->deadtime = voOS_GetSysTime() + lasttime;
            }

            memset( value , 0 , 100 );
            ptr = strstr( temp , "max=" );

            if( ptr )
            {
                ptr = ptr + strlen( "max=" );
                while( *ptr == ' ' )
                    ptr++;

                strcpy( value , ptr );

                ptr = strchr( value , ',' );

                if( ptr )
                    *ptr = '\0';

                VO_S32 lasttime = atoi(value) * 1000;

                if( lasttime >= MAXSOCKETREUSETIME )
                    lasttime = MAXSOCKETREUSETIME;

                m_ptr_persist->maxusetime = lasttime;
            }
        }
    }

    if( m_ptr_persist )
    {
        ptr = strstr( m_response , "Connection: close" );
        if( ptr )
        {
            m_ptr_persist->deadtime = voOS_GetSysTime();
            m_ptr_persist->maxusetime = 1;
        }
    }

    VO_CHAR * ptr_cookietest = m_response;

    {
        voCAutoLock lock( &m_cookieslock );

        if( strstr( ptr_cookietest , "Set-Cookie: " ) )
            memset( m_cookies , 0 , sizeof( m_cookies ) );

        while( (ptr_cookietest = strstr( ptr_cookietest , "Set-Cookie: " )) )
        {
            ptr_cookietest = ptr_cookietest + strlen( "Set-Cookie: " );
			VO_CHAR * ptr_end = strstr( ptr_cookietest , "\r\n" );

            while( 1 )
            {
                if( memcmp( ptr_cookietest , "Expires" , strlen("Expires") ) == 0 ||
                    memcmp( ptr_cookietest , "Path" , strlen("Path") ) == 0 ||
                    memcmp( ptr_cookietest , "Domain" , strlen("Domain") ) == 0 ||
                    memcmp( ptr_cookietest , "\r\n" , strlen("\r\n") ) == 0 )
                    break;

                VO_CHAR * ptr_pos = strstr( ptr_cookietest , ";" );

                if( !ptr_pos || ptr_pos > ptr_end )
                    break;

                VO_CHAR temp[1024];
                memset( temp , 0 , sizeof(temp) );
                memcpy( temp , ptr_cookietest , ptr_pos - ptr_cookietest );

                if( m_cookies[0] )
                    strcat( m_cookies , "; " );

                strcat( m_cookies , temp );

                ptr_cookietest = ptr_pos + 1;
            }
        }
    }

	if( m_status_code / 100 == 3 )
	{
		ptr = strstr( m_response , "Location:" );

        if( ptr )
        {
            ptr = ptr + strlen( "Location:" );

            while( *ptr != 'h' )
                ptr++;

            VO_CHAR * ptr_temp = strchr( ptr , '\r' );

            memset( m_reparseurl , 0 , sizeof( m_reparseurl ) );
            strncpy( m_reparseurl , ptr , ptr_temp - ptr );
        }
        else
            memset( m_reparseurl , 0 , sizeof( m_reparseurl ) );
	}

	return VO_TRUE;
}

VO_VOID vo_http_stream::stop_download_thread()
{
	m_stop_download = VO_TRUE;

	vo_thread::stop();

	m_stop_download = VO_FALSE;
}

VO_S64 vo_http_stream::get_download_bitrate()
{
	if( m_totaldownload == 0 || m_download_endtime == -1 || m_download_starttime == -1 )
		return -1;

	VO_S64 timecost = m_download_endtime - m_download_starttime;

	if( timecost == 0 )
		return -1;

	return (VO_S64)(m_totaldownload * 8. * 1000. / timecost);
}

VO_S64 vo_http_stream::getdownloadtime()
{
	VO_S64 timecost = m_download_endtime - m_download_starttime;
    return timecost;
}


void vo_http_stream::DoTransactionForDigest()
{
/*
The Sampele string

WWW-Authenticate: Digest realm="testrealm@host.com",
                        qop="auth,auth-int",
                        nonce="dcd98b7102dd2f0e8b11d0f600bfb0c093",
                        opaque="5ccc069c403ebaf9f0171e9517f40e41"
*/
    VO_BOOL   bRet = VO_FALSE;
    voStreamDigestAuth  varDigestAuth;
	DigestQOPType   varDigestQopType = DIGEST_QOP_NONE;
	VO_CHAR         strData[32] = {0};


    if(GetDigestRealm() != 0)
    {
        VOLOGI("can't get the realm!");
        return;
    }

    if(GetDigestNonce() != 0)
    {
        VOLOGI("can't get the Nonce!");    
	    return;
    }

	if(GetDigestQop() == 0)
	{
	    if(memcmp(m_strQop, "auth", 4) == 0)
	    {
	        varDigestQopType = DIGEST_QOP_AUTH;
	    }

		if(memcmp(m_strQop, "auth-int", 8) == 0)
	    {
	        varDigestQopType = DIGEST_QOP_AUTHINT;
	    }
	}

    if(GetDigestOpaque() != 0)
    {
        VOLOGI("can't get the Opaque!");    
    }

	if(GetDigestMethod() != 0)
    {
        VOLOGI("can't get the Method!");    
	    return;        
    }

	if(GetDigestURI() != 0)
    {
        VOLOGI("can't get the URI!");    
	    return;        
    }

    if(GetDigestUserName() != 0)
    {
        VOLOGI("can't get the UserName!");    
	    return;        
    }   

    if(GetDigestPwd() != 0)
    {
        VOLOGI("can't get the Pwd!");    
	    return;        
    }

    //Add the rand of client
    m_ulClientNonce1 = rand();
	m_ulClientNonce2 = rand();
	m_ulClientNonceCount++;


	VOLOGI("user len:%d", m_ulstrUserNameLen);
	VOLOGI("pwd len:%d", m_ulstrPwdLen);	
	varDigestAuth.SetUserInfo((VO_CHAR*)m_strUserName, (VO_U16)m_ulstrUserNameLen, (VO_CHAR*)m_strPwd, (VO_U16)m_ulstrPwdLen);
	varDigestAuth.SetNonce((VO_CHAR*)m_strNonce,(VO_U16)m_ulstrNonceLen);
	varDigestAuth.SetMethod((VO_CHAR*)m_strMethod,(VO_U16)m_ulstrMethodLen);
	varDigestAuth.SetRealm((VO_CHAR*)m_strRealm, (VO_U16)m_ulstrRealmLen);
	varDigestAuth.SetURI((VO_CHAR*)m_strURI, (VO_U16)m_ulstrURILen);

	VOLOGI("m_strQop:%s", m_strQop);    	
	VOLOGI("the qop type:%d", (int)varDigestQopType);    
	
	varDigestAuth.SetQOP(varDigestQopType);

	if(m_ulstrQopLen != 0)
	{
	    varDigestAuth.SetQOPString((VO_CHAR*)m_strQop, m_ulstrQopLen);
	}
    
	sprintf(strData, "%08x%08x", m_ulClientNonce1, m_ulClientNonce2);
    varDigestAuth.SetClientNonce(strData, 16);
	//varDigestAuth.SetClientNonce("3aabe96f6032af9c", 16);
	
	sprintf(strData, "%08x", m_ulClientNonceCount);
    varDigestAuth.SetClientNonceCount(strData, 8);
	
		
	memset(m_strResponse, 0, 128);
	m_ulstrResponseLen = 0;
	bRet = varDigestAuth.GetResponse((VO_CHAR*)m_strResponse, 128);
	if(bRet == VO_FALSE)
    {
        VOLOGI("can't get Response!");
		return;
    }


    m_ulDigestFlag = 1;
	VOLOGI("Get Response!");
}



VO_U32    vo_http_stream::GetDigestRealm()
{
    // 0 is sucess
    VO_U32    ulRet = 1;
	
    ulRet = GetStringWithInStartEnd(m_response, (VO_CHAR*)"Digest realm=\"", (VO_CHAR*)"\"", m_strRealm, 128, &m_ulstrRealmLen);
    if(ulRet != 0)
	{
	    VOLOGI("get qop error!");
	}

	VOLOGI("m_strRealm:%s", m_strRealm);

    return ulRet;
}

VO_U32    vo_http_stream::GetDigestUserName()
{
    // 0 is sucess
	VOLOGI("m_strUserName:%s", m_strUserName);
	return 0;
}

VO_U32    vo_http_stream::GetDigestPwd()
{
    // 0 is sucess
    //constant value
	VOLOGI("m_strPwd:%s", m_strPwd);
	return 0;
}

VO_U32    vo_http_stream::GetDigestNonce()
{
    // 0 is sucess
    VO_U32    ulRet = 1;
	
    ulRet = GetStringWithInStartEnd(m_response, (VO_CHAR*)"nonce=\"", (VO_CHAR*)"\"", m_strNonce, 128, &m_ulstrNonceLen);
    if(ulRet != 0)
	{
	    VOLOGI("get nonce error!");
	}

	VOLOGI("m_strNonce:%s", m_strNonce);

    return ulRet;
}
	
VO_U32    vo_http_stream::GetDigestQop()
{
    // 0 is sucess
    VO_U32    ulRet = 1;
	
    ulRet = GetStringWithInStartEnd(m_response, (VO_CHAR*)"qop=\"", (VO_CHAR*)"\"", m_strQop, 128, &m_ulstrQopLen);
    if(ulRet != 0)
	{
	    VOLOGI("get qop error!");
	}
	
	VOLOGI("m_strQop:%s", m_strQop);
    return ulRet;
}

VO_U32    vo_http_stream::GetDigestMethod()
{
    VO_U32    ulRet = 0;
    memset(m_strMethod, 0, 128);
	memcpy(m_strMethod, "GET", strlen("GET"));
	m_ulstrMethodLen = strlen("GET");

	VOLOGI("m_strMethod:%s", m_strMethod);
	
    return ulRet;
}

VO_U32    vo_http_stream::GetDigestURI()
{
    VO_U32    ulRet = 0;
    memset(m_strURI, 0, 1024);
	m_ulstrURILen = strlen(m_path);
	memcpy(m_strURI, m_path, m_ulstrURILen);
	VOLOGI("the m_strURI:%s", m_strURI);
	
    return    ulRet;
}

VO_U32    vo_http_stream::GetDigestOpaque()
{
    // 0 is sucess
    VO_U32    ulRet = 1;
	
    ulRet = GetStringWithInStartEnd(m_response, (VO_CHAR*)"opaque=\"", (VO_CHAR*)"\"", m_strOpaque, 128, &m_ulstrOpaqueLen);
    if(ulRet != 0)
	{
	    VOLOGI("get qop Opaque!");
	}

    return ulRet;

}


VO_U32    vo_http_stream::GetStringWithInStartEnd(VO_CHAR* pStrSource, VO_CHAR* pStart, VO_CHAR* pEnd, VO_CHAR* pStrDesBuffer, VO_U32 ulBufferLen, VO_U32*  pLen)
{

    VO_U32    ulRet = 1;
	
    VO_CHAR * pFind = NULL;
    VO_CHAR * pValueStart = NULL;
	VO_U32    ulLen = 0;

    if((pStrSource == NULL) ||
	   (pStart == NULL) ||
	   (pEnd == NULL) ||
	   (pStrDesBuffer == NULL) ||
	   (pLen == NULL))
    {
        VOLOGI("some pointer is NULL");
        return ulRet;
    }

	
    pFind = strstr( pStrSource , pStart);
	if(pFind != NULL)
	{
	    //Find the Digest realm;
	    if(pFind == NULL)
		{
		    VOLOGI("can't find pStard String");
		    return ulRet;
		}

        //get the start
		pValueStart = pFind + strlen(pStart);
	    pFind = strstr( pValueStart , pEnd );
		if(pFind  == NULL)
		{
		    VOLOGI("can't find pEnd String");
			return ulRet;
		}

        ulLen = pFind-pValueStart;
		if(ulLen >= ulBufferLen)
		{
		    VOLOGI("buffer is too small");		
		    return ulRet;
		}
		
		memset(pStrDesBuffer, 0 , ulBufferLen);
		memcpy(pStrDesBuffer, pValueStart, ulLen);
		*pLen = ulLen;
		return 0;
	}

    return ulRet;
}


VO_VOID vo_http_stream::write_eos()
{
	if( m_ptr_buffer_stream )
		m_ptr_buffer_stream->write_eos();
}

VO_S32 vo_http_stream::get_lasterror()
{
    VO_S32 ret = 0;
	if( m_ptr_buffer_stream )
	{
		ret = m_ptr_buffer_stream->get_lasterror();
	}

    if( ret == 0 )
        ret = m_errorcode;

	return ret;
}


VO_U32 vo_http_stream::GetRedirectURL(VO_CHAR*  pReceiveBuffer, VO_U32  ulLen)
{
    if((pReceiveBuffer != NULL) && (ulLen > strlen(m_reparseurl)))
	{
        memcpy(pReceiveBuffer, m_reparseurl, strlen(m_reparseurl));
		return 0;
	}

	//only 0 is correct
	return 1;
}



VO_VOID    vo_http_stream::SetParamForHttp(VO_U32 uId, VO_VOID* pDataUser, VO_VOID*  pDataPwd)
{
    if(pDataUser == NULL)
    {
        return;
    }

    VOLOGI("set the user:%s and pwd:%s for http", (VO_CHAR*)pDataUser, (VO_CHAR*)pDataPwd);
	
	memset(m_strUserName, 0, 128);
	memset(m_strPwd, 0, 128);

	memcpy(m_strUserName, (VO_CHAR*)pDataUser, strlen((VO_CHAR*)pDataUser));
	m_ulstrUserNameLen = strlen((VO_CHAR*)pDataUser);
	memcpy(m_strPwd, (VO_CHAR*)pDataPwd, strlen((VO_CHAR*)pDataPwd));
	m_ulstrPwdLen = strlen((VO_CHAR*)pDataPwd);
}
