

#include "vo_https_stream.h"
#include "voString.h"
#include "voOSFunc.h"
#include "fortest.h"
#include "vo_mem_stream.h"
#include "vo_drm_mem_stream.h"
#include "..\..\Common\NetWork\vo_socket.h"

#define MAXGETSIZE (1024*2)

vo_https_stream::vo_https_stream()
:m_ptr_ctx(0)
,m_ptr_ssl(0)
{
	;
}

vo_https_stream::~vo_https_stream()
{
	;
}

VO_BOOL vo_https_stream::init( VO_CHAR * url )
{
	uninit();

	output( "1" , __LINE__ );
	SSL_library_init();
	output( "2" , __LINE__ );
	SSL_load_error_strings();
	output( "3" , __LINE__ );
	m_ptr_ctx = SSL_CTX_new(SSLv23_client_method());
	output( "4" , __LINE__ );

	if ( !m_ptr_ctx )
	{
		return VO_FALSE;
	}

	/*output( "5" , __LINE__ );
	SSL_CTX_set_verify(m_ptr_ctx, SSL_VERIFY_PEER, NULL);
	output( "6" , __LINE__ );

	if(!(SSL_CTX_load_verify_locations(m_ptr_ctx, "/sdcard/ca.pem", NULL)))
	{
		output( "Can't read CA list!" , __LINE__ );
		return VO_FALSE;
	}*/

	output( "7" , __LINE__ );

	m_ptr_ssl = SSL_new( m_ptr_ctx );

	output( "8" , __LINE__ );

	if (m_ptr_ssl == NULL)
	{
		return VO_FALSE;
	}

	output( "9" , __LINE__ );
	if( VO_FALSE == resolve_url( url ) )
		return VO_FALSE;

	output( "10" , __LINE__ );
	output( m_host , __LINE__ );
	output( m_port , __LINE__ );

	if( !vo_socket_connect( &m_socket , m_host , m_port ) )
	{
		output( "Socket Connect Fail!" , __LINE__ );
		return VO_FALSE;
	}

	output( "11" , __LINE__ );

	VO_S32 ret = SSL_set_fd( m_ptr_ssl , m_socket);
	if (ret == 0)
	{
		return VO_FALSE;
	}

	output( "12" , __LINE__ );

	RAND_poll();
	while (RAND_status() == 0)
	{
		unsigned short rand_ret = rand() % 65536;
		RAND_seed(&rand_ret, sizeof(rand_ret));
	} 

	output( "13" , __LINE__ );

	ret = SSL_connect( m_ptr_ssl );
	if (ret != 1)
	{
		ret = SSL_connect( m_ptr_ssl );
		if (ret != 1)
		{
			return VO_FALSE;
		} 
	} 

	output( "14" , __LINE__ );

	/* check server certification */
	//check_cert( m_ptr_ssl , m_host );

	VO_CHAR request[2048];
	memset( request , 0 , sizeof(request) );
	VO_U32 request_size = perpare_http_request( request );

	output( request , __LINE__ );

	VO_S32 written = 0;

	while( written < request_size )
	{
		VO_S32 size = SSL_write( m_ptr_ssl , request + written , request_size - written );

		if( size == -1 )
			return VO_FALSE;

		written = written + size;
	}

	m_download_starttime = voOS_GetSysTime();

	output( "Start to get response!" , __LINE__ );

	if( VO_FALSE == get_response() )
	{
		output( "get_response fail!" , __LINE__ );
		output( m_response , __LINE__ );
		return VO_FALSE;
	}

	output( m_response , __LINE__ );

	if( m_status_code / 100 != 2 )
	{
		if( m_status_code == 303 )
		{
			return init( m_reparseurl );
		}
		else
		{
			output( request , __LINE__ );
			output( m_response , __LINE__ );
			return VO_FALSE;
		}
	}

	stop_download_thread();

	VO_U32 thread_id;
	voThreadCreate( &m_downloadthread , &thread_id , downloadthread , this , 0 );

	return VO_TRUE;
}

void vo_https_stream::uninit()
{
	if( m_ptr_ssl )
		SSL_shutdown(m_ptr_ssl);

	if( m_socket != -1 )
	{
		//output( "close socket!" , m_socket );
		vo_socket_close( m_socket );
	}

	if( m_ptr_ssl )
		SSL_free( m_ptr_ssl );

	if( m_ptr_ctx )
		SSL_CTX_free(m_ptr_ctx);

	m_ptr_ctx = 0;
	m_ptr_ssl = 0;
	m_socket = -1;
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
		strcpy( m_port , "443" );
	}

	return VO_TRUE;
}

void vo_https_stream::download_normal()
{
	VO_S64 downloaded = 0;

	fd_set fds;
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 20000;

	m_totaldownload = 0;


// 	FD_ZERO(&fds);
// 	FD_SET(m_socket,&fds);
// 
// 	VO_S32 select_ret = select(m_socket,&fds,NULL,NULL,&timeout);
// 
// 	while( select_ret <= 0 )
// 	{
// 		voOS_Sleep( 20 );
// 		FD_ZERO(&fds);
// 		FD_SET(m_socket,&fds);
// 		select_ret = select(m_socket,&fds,NULL,NULL,&timeout);
// 	}

	VO_BOOL is_error = VO_FALSE;
	VO_BOOL is_end = VO_FALSE;

	do
	{
		VO_S32 downloadsize = 16;
		VO_CHAR buffer[MAXGETSIZE];
		memset( buffer , 0 , MAXGETSIZE );

		VO_S32 ret = SSL_read( m_ptr_ssl , (VO_PBYTE)buffer , downloadsize );

		switch(SSL_get_error( m_ptr_ssl,ret ))
		{
            case SSL_ERROR_NONE:
				{
					output( "we have read:" , __LINE__ );
					output( (VO_CHAR*)buffer , __LINE__ );

					m_totaldownload = downloaded = downloaded + ret;
					m_download_endtime = voOS_GetSysTime();
					append( (VO_PBYTE)buffer , ret );
				}
				break;
			case SSL_ERROR_ZERO_RETURN:
				{
					is_end = VO_TRUE;
				}
            default:
				is_error = VO_TRUE;
				break;
        }

		if( is_end )
			break;

	}while( !m_stop_download /*&& SSL_pending( m_ptr_ssl ) */&& !is_error );

	if( m_stop_download )
		output( "force stop download!" , __LINE__ );

	m_content_length = m_totaldownload;

	m_download_endtime = voOS_GetSysTime();
}

void vo_https_stream::download_chunked()
{
	vo_http_stream::download_chunked();
}

VO_BOOL vo_https_stream::get_response()
{
	VO_U32 count = 0;
	memset( m_response , 0 , sizeof(m_response) );

	while( 1 )
	{
		VO_CHAR value;

		VO_S32 ret = SSL_read( m_ptr_ssl , (VO_PBYTE)&value , 1 );

		if( ret != 1 )
			return VO_FALSE;

		m_response[count] = value;

		if( count >= 3 && m_response[count] == '\n' && m_response[count - 1] == '\r' 
			&& m_response[count - 2] == '\n' && m_response[count - 3] == '\r' )
			break;

		count++;

		if( count >= sizeof(m_response) )
			return VO_FALSE;
	}

	output( "Start to analyze response!" , __LINE__ );
	analyze_response();
	output( "End analyze response!" , __LINE__ );

	return VO_TRUE;
}

VO_BOOL vo_https_stream::check_cert(SSL *ssl, VO_CHAR *host)
{
	X509 *peer;
	char peer_CN[256];
	char *str;

	if(SSL_get_verify_result(ssl) != X509_V_OK)
	{
		return VO_FALSE;
	}

	peer = SSL_get_peer_certificate(ssl);

	if( peer != NULL )
	{
		str = X509_NAME_oneline (X509_get_subject_name (peer), 0, 0);
		if( str == NULL )
		{
			output("X509_NAME_oneline error!" , __LINE__);
		}
		else
		{
			output( str , __LINE__ );
			OPENSSL_free (str);
		}

		str = X509_NAME_oneline (X509_get_issuer_name(peer), 0, 0);
		if( str == NULL )
		{
			output("X509_NAME_oneline error!",__LINE__);
		}
		else
		{
			output(str,__LINE__);
			OPENSSL_free (str);
		}
		X509_free (peer);
	} 
	else
	{
		return VO_FALSE;
	}

	/*Check the common name*/

	X509_NAME_get_text_by_NID (X509_get_subject_name(peer), NID_commonName, peer_CN, 256);

	if( strcmp(peer_CN, host) )
	{
		output( "Common name doesn't match host name" , __LINE__ );
		return VO_FALSE;
	} 

	return VO_TRUE;
}