#include "vo_https_stream.h"
#include "voString.h"
#include "voOSFunc.h"
#include "fortest.h"
#include "vo_mem_stream.h"
#include "../../Common/NetWork/vo_socket.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


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

	SSL_library_init();
	SSL_load_error_strings();
	m_ptr_ctx = SSL_CTX_new(SSLv23_client_method());

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

	m_ptr_ssl = SSL_new( m_ptr_ctx );

	if (m_ptr_ssl == NULL)
	{
		return VO_FALSE;
	}

	if( VO_FALSE == resolve_url( url ) )
		return VO_FALSE;

	if( !vo_socket_connect( &m_socket , m_host , m_port ) )
	{
		output( "Socket Connect Fail!" , __LINE__ );
		return VO_FALSE;
	}

	VO_S32 ret = SSL_set_fd( m_ptr_ssl , m_socket);
	if (ret == 0)
	{
		return VO_FALSE;
	}

	RAND_poll();
	while (RAND_status() == 0)
	{
		unsigned short rand_ret = rand() % 65536;
		RAND_seed(&rand_ret, sizeof(rand_ret));
	} 

	ret = SSL_connect( m_ptr_ssl );
	if (ret != 1)
	{
		ret = SSL_connect( m_ptr_ssl );
		if (ret != 1)
		{
			return VO_FALSE;
		} 
	}

	/* check server certification */
	//check_cert( m_ptr_ssl , m_host );

	VO_CHAR request[2048];
	memset( request , 0 , sizeof(request) );
	VO_U32 request_size = perpare_http_request( request );

	VO_U32 written = 0;

	while( written < request_size )
	{
		VO_S32 size = SSL_write( m_ptr_ssl , request + written , request_size - written );

		if( size == -1 )
			return VO_FALSE;

		written = written + size;
	}

	m_download_starttime = voOS_GetSysTime();

	if( VO_FALSE == get_response() )
	{
		output( "get_response fail!" , __LINE__ );
		output( m_response , __LINE__ );
		return VO_FALSE;
	}

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

	stop();

	begin();

	return VO_TRUE;
}

void vo_https_stream::uninit()
{
	if( m_ptr_ssl )
		SSL_shutdown(m_ptr_ssl);

	vo_http_stream::uninit();

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

	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 20000;

	m_totaldownload = 0;

	VO_BOOL is_error = VO_FALSE;
	VO_BOOL is_end = VO_FALSE;

	output( m_path , m_content_length );

	do
	{
		VO_S32 downloadsize = MAXGETSIZE;
		VO_CHAR buffer[MAXGETSIZE];
		memset( buffer , 0 , MAXGETSIZE );

		VO_S32 ret = SSL_read( m_ptr_ssl , (VO_PBYTE)buffer , downloadsize );

		switch(SSL_get_error( m_ptr_ssl,ret ))
		{
            case SSL_ERROR_NONE:
				{
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

		//output( "Total Download: " , (VO_S32)m_totaldownload );

	}while( !m_stop_download /*&& SSL_pending( m_ptr_ssl ) */&& !is_error && m_totaldownload < m_content_length );

	if( m_stop_download )
		output( "force stop download!" , __LINE__ );

	m_content_length = m_totaldownload;

	m_download_endtime = voOS_GetSysTime();

	output( "Download Stopped!" , __LINE__ );
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
