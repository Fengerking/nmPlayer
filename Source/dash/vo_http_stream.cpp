#include "vo_http_stream.h"
#include "voString.h"
#include "voOSFunc.h"
#include "fortest.h"
#include "vo_mem_stream.h"
#include "vo_socket.h"

#define MAXGETSIZE (1024*2)

#define LOG_TAG "vo_http_stream"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
#ifdef _IOS
using namespace _DASH;
#endif

vo_http_stream::vo_http_stream()
: m_socket(-1)
, m_downloadthread(0)
,m_ptr_buffer_stream(0)
,m_download_mode(DOWNLOAD2NONE)
,m_stop_download(VO_FALSE)
,m_download_starttime(-1)
,m_download_endtime(-1)
,m_is_inited(VO_FALSE)
,timecost(0)
,m_http_Request_type(VO_HTTP_GET)
{

	m_pChallenge = NULL;
	m_nChallenge = 0;
	m_pcUrl = NULL;
	m_nUrl = 0;
	m_pcHeader = NULL;
	m_nHeader = 0;

	vo_socket_init();
}

vo_http_stream::~vo_http_stream(void)
{
	close();
	vo_socket_uninit();
}

VO_BOOL vo_http_stream::open( VO_CHAR * url , DownloadMode mode )
{

	//m_download_starttime = voOS_GetSysTime();

	close();

	m_download_mode = mode;

	output( url , __LINE__ );
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

	//output( url , __LINE__ );
	
	VO_S32 trycounts = 0;
	VO_BOOL ret;

	while( !(ret = init( url )) && trycounts < 3 )
	{
		trycounts++;
		char c[100];
		//sprintf(c, "url init fail trycounts:%d", trycounts);
		output(c, __LINE__);
		voOS_Sleep( 1000 );
	}

	return ret;
}

VO_BOOL vo_http_stream::open( VO_CHAR * url , DownloadMode mode , VO_PBYTE ptr_key , VO_PBYTE ptr_iv , ProtectionType drm_type , DRM_Callback * ptr_drm_engine , void * drm_handle )
{
/*
	close();

	m_download_mode = mode;

	switch( m_download_mode )
	{
	case DOWNLOAD2MEM:
		m_ptr_buffer_stream = new vo_drm_mem_stream( ( VO_CHAR * )ptr_key , ( VO_CHAR * )ptr_iv , drm_type , ptr_drm_engine , drm_handle );
		((vo_mem_stream*)m_ptr_buffer_stream)->open();
		break;
	case DOWNLOAD2FILE:
		break;
	}

	VO_S32 trycounts = 0;
	VO_BOOL ret;

	output( url , __LINE__ );

	while( !(ret = init( url )) && trycounts < 3 )
	{
		trycounts++;
		voOS_Sleep( 1000 );
	}

	return ret;*/
	return VO_FALSE;
}

VO_VOID vo_http_stream::close()
{

	//stop_download_thread();

	//switch( m_download_mode )
	//{
	//case DOWNLOAD2MEM:
	//	((vo_mem_stream*)m_ptr_buffer_stream)->close();
	//	break;
	//case DOWNLOAD2FILE:
	//	break;
	//}

	//m_is_download_ok = VO_FALSE;
	//if( m_ptr_buffer_stream )
	//	delete m_ptr_buffer_stream;
	//m_ptr_buffer_stream = 0;
	//m_download_mode = DOWNLOAD2NONE;

	closedownload();

	uninit();
	
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

	if( VO_FALSE == resolve_url( url ) )
		return VO_FALSE;

	if( !vo_socket_connect( &m_socket , m_host , m_port ) )
	{
		output( "Socket Connect Fail!" , __LINE__ );
		return VO_FALSE;
	}

	//output( "socket created!" , m_socket );

	//m_download_endtime = voOS_GetSysTime();

	//VOLOGR( "/////////////////////////////////////////////////////////////////init socket time ////// : %I64d ", m_download_endtime - m_download_starttime);

	m_download_starttime = voOS_GetSysTime();

	VO_CHAR request[2048];
	memset( request , 0 , sizeof(request) );
	VO_U32 request_size = perpare_http_request( request );

	vo_socket_send( m_socket , (VO_PBYTE)request , request_size );

	m_download_starttime = voOS_GetSysTime();

	output( request , __LINE__ );


	//add timeout for recv by hc
#ifdef WIN32
	int   timeout=2000;	
	setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(int)); 
#else
	struct timeval timeout = {3,0};
	setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(timeval)); 
#endif
	//add timeout for recv by hc

	if( VO_FALSE == get_response() )
	{
		output( "get_response fail!" , __LINE__ );
	//	output( m_response , __LINE__ );
		return VO_FALSE;
	}
#ifdef WIN32
	int   timeout2=0;	
	setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout2,sizeof(int)); 
#else
	struct timeval timeout2 = {0,0};
	setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout2,sizeof(timeval)); 
#endif

	//output( m_response , __LINE__ );

	if( m_status_code / 100 != 2 )
	{
		if( m_status_code / 100 == 3 )
		{
			return init( m_reparseurl );
		}
		else
		{
			output( request , __LINE__ );
			//output( m_response , __LINE__ );
           
			return VO_FALSE;
		}
	}

	//m_download_endtime = voOS_GetSysTime();

	//VOLOGR( "/////////////////////////////////////////////////////////////////init socket time ////// : %I64d ", m_download_endtime - m_download_starttime);

	//m_download_starttime = voOS_GetSysTime();
	vo_thread::begin();
	//VO_U32 thread_id;
//	voThreadHandle handlethread;
	//voThreadCreate( &handlethread , &thread_id , downloadthread , this , 0 );
	//voThreadCreate( &m_downloadthread , &thread_id , downloadthread , this , 0 );
	//output( "downloadthread created! thread id:" , (VO_S32)m_downloadthread );

	return VO_TRUE;
}
void vo_http_stream::thread_function()
{
	downloadthread(this);
}
void vo_http_stream::uninit()
{
	if( m_socket != -1 )
	{
		//output( "close socket!" , m_socket );
		vo_socket_close( m_socket );
		m_socket = -1;
	}

	stop_download_thread();

	m_downloadthread = 0;
	m_stop_download = VO_FALSE;
}

VO_BOOL vo_http_stream::resolve_url( VO_CHAR * url )
{
	if(strlen(url) == 0)
		return VO_FALSE;
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

VO_U32 vo_http_stream::downloadthread( VO_PTR pParam )
{
	vo_http_stream * ptr_stream = ( vo_http_stream * )pParam;
	ptr_stream->m_downloadthread = (voThreadHandle)1;
	ptr_stream->download();
	//output( "+ptr_stream->m_downloadthread = 0" , __LINE__ );
	ptr_stream->m_downloadthread = 0;
	//output( "-ptr_stream->m_downloadthread = 0" , __LINE__ );
	return 0;
}

void vo_http_stream::download()
{
	if( m_content_length <= 0 )
		return;

	VO_U32 start = voOS_GetSysTime();

	if( m_is_chunked )
	{
		output("It is Chunked Mode Download!",__LINE__);
		download_chunked();
	}
	else
	{
		if ( m_content_length <= 0 )
		{
			output( "It is endless download!" , __LINE__ );
			//download_endless();
		}
		else
		{
			output( "It is normal download!" , __LINE__ );
			download_normal();
		}
	}

	
	VO_U32 end = voOS_GetSysTime();

	//VOLOGR("http Download Ok!");

	char debug[250];
	sprintf( debug , "Download OK! Download %f KB, Cost %f Seconds!\n" , m_content_length/1024. , ( end - start ) / 1000. );
	output( debug , __LINE__ );

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

	while( downloaded != m_content_length && !m_stop_download )
	{
		FD_ZERO(&fds);
		FD_SET(m_socket,&fds);

		VO_S32 select_ret = select( m_socket + 1 ,&fds,NULL,NULL,&timeout);

		/*if( -1 == select_ret )
		{
			voOS_Sleep( 20 );
			continue;
		}
		else if( -1 == select_ret )
		{
			return;
		}*/

		if( -1 == select_ret )
		{
			output( "Socket Error" , __LINE__ );
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
		output( "force stop download!" , __LINE__ );


	m_download_endtime = voOS_GetSysTime();
}

void vo_http_stream::download_chunked()
{
	;
}

VO_U32 vo_http_stream::perpare_http_request( VO_CHAR * ptr_request )
{
	switch(m_http_Request_type)
	{
	/*case VO_HTTP_GET:
		if( strcmp( m_port , "80" ) )
		{
			const char * request_template = "GET %s HTTP/1.1\r\nHost: %s:%s\r\n\r\n";
			return sprintf( ptr_request , request_template , m_path , m_host , m_port );
		}
		else
		{
			 const char * request_template = "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n";
			return sprintf( ptr_request , request_template , m_path , m_host );
		}*/
//
	case VO_HTTP_GET:
	if( strcmp( m_port , "80" ) )
  {
   const char * request_template = "GET %s HTTP/1.1\r\nHost: %s:%s\r\nConnection: keep-alive\r\n\r\n";
   return sprintf( ptr_request , request_template , m_path , m_host , m_port );
  }
  else
  {
    const char * request_template = "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\n\r\n";
   return sprintf( ptr_request , request_template , m_path , m_host );
  }
		break;
	case VO_HTTP_POST:
		{
		/*POST /pr/svc/rightsmanager.asmx HTTP/1.1
		Host: playready.directtaps.net
	    Content-Type: text/xml; charset=utf-8
	    Content-Length: length
		SOAPAction: "http://schemas.microsoft.com/DRM/2007/03/protocols/AcquireLicense"

		challege*/
		output("ppp",__LINE__);
		const char * request_template = "POST %s HTTP/1.1\r\nHost: %s\r\nContent-Type: text/xml; charset=utf-8\r\nContent-Length:%d\r\nSOAPAction: \"%s\"\r\n\r\n%s";
		return sprintf( ptr_request , request_template , m_path , m_host , m_nChallenge, m_pcHeader,m_pChallenge );
		}
		break;
	default:
		return 0;
		break;
	}
	
	
}

VO_BOOL vo_http_stream::get_response()
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

		if( count >= 3 && m_response[count] == '\n' && m_response[count - 1] == '\r' 
			&& m_response[count - 2] == '\n' && m_response[count - 3] == '\r' )
			break;

		count++;

		if( count >= sizeof(m_response) )
			return VO_FALSE;
	}


	return 	analyze_response();

}

VO_BOOL vo_http_stream::analyze_response()
{
	output("analyze_response",__LINE__);
	m_status_code = -1;
	 int   iGetValue = 0;
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


	output( "HTTP Status Code: " , m_status_code,2 );
// 	if(m_status_code == -1)
// 		output( m_response, m_status_code,2 );
	if(m_status_code == 404 || m_status_code == -1)
	{
		char c[1024];
		sprintf(c, "HTTP. Response Code:%d", m_status_code);
		output( c, __LINE__, 1 );
		return VO_FALSE;
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

		m_content_length = 0;

		if(ptr)
		{
			sscanf( ptr , "Content-Length: %d\r\n" , &m_content_length );
		}
		else
		{
			ptr = strstr( m_response , "-Length:" );
			m_content_length = 0;
			if(ptr)
				sscanf( ptr , "-Length: %d\r\n" , &m_content_length );
		}
	}

	ptr = strstr( m_response , "Content-Type:" );

	memset( m_mimetype , 0 , sizeof( m_mimetype ) );
	
//   	char cc[]="Content-Type: text/xml; charset=utf-8";
//  	sscanf( cc , "%*s %[^;]\r\n" , m_mimetype );
// 		char cc[100];
//  	memset(cc,0,sizeof(cc));
//  	sprintf(cc,"Content-type:%s",ptr);
//  	output(cc,0);
	if( ptr )
	{
		sscanf( ptr , "%*s %[^;\r\n]" , m_mimetype );
	}
//  	memset(cc,0,sizeof(cc));
//  	sprintf(cc,"MimeType:%s",m_mimetype);
//  	output(cc,0);
	if( m_status_code == 303 )
	{
		ptr = strstr( m_response , "Location:" );
		ptr = ptr + strlen( "Location:" );

		while( *ptr != 'h' )
			ptr++;

		VO_CHAR * ptr_temp = strchr( ptr , '\r' );

		memset( m_reparseurl , 0 , sizeof( m_reparseurl ) );
		strncpy( m_reparseurl , ptr , ptr_temp - ptr );
	}
	output("analyze_response",__LINE__);

	return VO_TRUE;
}

VO_VOID vo_http_stream::stop_download_thread()
{
	output("+stop_download_thread",__LINE__ );
	m_stop_download = VO_TRUE;
	vo_thread::stop();

	m_stop_download = VO_FALSE;
	output("-stop_download_thread",__LINE__ );

}

VO_S64 vo_http_stream::get_download_bitrate()
{
//	if( m_totaldownload == 0 || m_download_endtime == -1 || m_download_starttime == -1 )
	//	return -1;

	timecost = m_download_endtime - m_download_starttime;

	//if( timecost == 0 )
	//	return -1;

	return m_totaldownload * 8. * 1000. / timecost;
}

VO_BOOL vo_http_stream::startdownload( VO_CHAR * url, DownloadMode mode,VO_HTTP_REQUEST_TYPE type /*= VO_HTTP_GET */)
{
	if( !m_is_inited )
		close();

	m_download_mode = mode;
	m_http_Request_type = type;
	switch( m_download_mode )
	{
	case DOWNLOAD2MEM:
		m_ptr_buffer_stream = new vo_mem_stream();
		((vo_mem_stream*)m_ptr_buffer_stream)->open();
		break;
	case DOWNLOAD2FILE:
		break;
	default:
		break;
	}

	output( url , __LINE__ );

	VO_S32 trycounts = 0;
	VO_BOOL ret;

	while( !(ret = process( url )) && trycounts < 3 )
	{
		m_is_inited = VO_FALSE;
		if(m_status_code == 404)
		{	
			close();
			break;
		}
		trycounts++;
		voOS_Sleep( 1000 );
	}

	return ret;
}
VO_BOOL vo_http_stream::startdownload( VO_PBYTE pChallenge, VO_U32 nChallenge, VO_PCHAR pcUrl, VO_U32 nUrl, VO_PCHAR pcHttpHeader, VO_U32 nHttpHeader, DownloadMode mode,VO_HTTP_REQUEST_TYPE type /*= VO_HTTP_GET*/)
{

	m_pChallenge = pChallenge;
	m_nChallenge = nChallenge;
	m_pcUrl = pcUrl;
	m_nUrl = nUrl;
	m_pcHeader = pcHttpHeader;
	m_nHeader = nHttpHeader;
	return startdownload( pcUrl , mode,type );
}

VO_BOOL vo_http_stream::process( VO_CHAR * url)
{
	output("in process",__LINE__);
	if( VO_FALSE == resolve_url( url ) )
		return VO_FALSE;
	output("in process",__LINE__);
	if( !m_is_inited )
	{
		if( !vo_socket_connect( &m_socket , m_host , m_port ) )
		{
			output( "Socket Connect Fail!" , __LINE__ );
			return VO_FALSE;
		}

		m_is_inited = VO_TRUE;
	}
	output("in process",__LINE__);
	//output( "socket created!" , m_socket );

	//m_download_endtime = voOS_GetSysTime();

	//VOLOGR( "/////////////////////////////////////////////////////////////////init socket time ////// : %I64d ", m_download_endtime - m_download_starttime);
	
	VO_CHAR *request = new VO_CHAR[1024*20];
	memset( request , 0 , sizeof(request) );
	VO_U32 request_size = perpare_http_request( request );
	//output("request_size %d",request_size);
	output( request , __LINE__ );
if(m_http_Request_type == VO_HTTP_POST)
{
// #ifndef _WIN32
// 	FILE *fff =NULL;
// 	fff = fopen("data/local/ff/post","wb");
// 	if(fff) fwrite(request,1,request_size,fff);
// 	fclose(fff);
// #endif
}
// 	FILE *fff =NULL;
// 	fff = fopen("d:/connect_str","wb");
// 	fwrite(request,1,request_size,fff);
// 	fclose(fff);
	vo_socket_send( m_socket , (VO_PBYTE)request , request_size );
	delete []request;
	request = NULL;
	m_download_starttime = voOS_GetSysTime();

	if( VO_FALSE == get_response() )
	{
		output( "get_response fail!" , __LINE__ );
	//	output( m_response , __LINE__ );
		return VO_FALSE;
	}

	//output( m_response , __LINE__ );

	if( m_status_code / 100 != 2 )
	{
		if( m_status_code == 303 )
		{
			return init( m_reparseurl );
		}
		else
		{
			output( request , __LINE__ );
			//output( m_response , __LINE__ );
			return VO_FALSE;
		}
	}

	//thread_function();
	vo_thread::begin();

// 	VO_U32 thread_id;
// 	voThreadHandle handlethread;
// 	voThreadCreate( &handlethread , &thread_id , downloadthread , this , 0 );
	//voThreadCreate( &m_downloadthread , &thread_id , downloadthread , this , 0 );
	//output( "downloadthread created! thread id:" , (VO_S32)m_downloadthread );

	return VO_TRUE;
}

VO_VOID vo_http_stream::closedownload( )
{
	output("in closedownload",1);
	stop_download_thread();

	switch( m_download_mode )
	{
	case DOWNLOAD2MEM:
		((vo_mem_stream*)m_ptr_buffer_stream)->close();
		break;
	case DOWNLOAD2FILE:
		break;
	default:
		break;
	}
output("in closedownload1",1);
	m_is_download_ok = VO_FALSE;
	if( m_ptr_buffer_stream )
		delete m_ptr_buffer_stream;
	output("in closedownload2",1);
	m_ptr_buffer_stream = 0;
	m_download_mode = DOWNLOAD2NONE;

}

VO_BOOL vo_http_stream::GetUTCTime(time_t &tt)
{
	VO_BOOL ret = VO_FALSE;
	strcpy( m_port , "13" );
	strcpy( m_host , "time.nist.gov" );//time.nist.gov UTC
	output(m_host,__LINE__,5);
	output(m_port,__LINE__,5);
	if( !vo_socket_connect( &m_socket , m_host , m_port ) )
	{
		output( "Socket Connect Fail!" , __LINE__ );
		ret = VO_FALSE;
	}
	
	VO_CHAR NtpTime[100];
	memset(NtpTime, 0x00, sizeof(NtpTime));
	VO_CHAR szTemp[100]="";   
	voOS_Sleep(1);
	VO_S32 size = vo_socket_recv( m_socket , (VO_PBYTE)&NtpTime , 100 );
	//56080 12-06-02 07:03:08 50 0 0 363.7 UTC(NIST) * 
	if( size < 20 )
	{	
		output("ret < 20", __LINE__, 5);
		ret = VO_FALSE;
	}
	else
	{
		
		output(NtpTime +1,__LINE__,5);
		char *pdest;   
		pdest = strchr( NtpTime, ' ' );   
		strcpy(szTemp, pdest+1);   

		struct tm tm1;
		//time_t time1;
		sscanf(szTemp, "%2d-%2d-%2d %2d:%2d:%2d",    
			&tm1.tm_year,
			&tm1.tm_mon,
			&tm1.tm_mday,
			&tm1.tm_hour,
			&tm1.tm_min,
			&tm1.tm_sec);
		tm1.tm_year += (2000 - 1900);
		tm1.tm_mon --;
		tm1.tm_isdst=-1;
		tt = mktime(&tm1);
		ret = VO_TRUE;
	}
	if(m_socket)
		vo_socket_close(m_socket);
	return ret;
}
