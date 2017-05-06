
#include "log.h"
#include "stdio.h"
#include "string.h"
#include "voString.h"
#include "voLog.h"

#ifdef _WIN32
#include "Windows.h"
#endif

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

typedef void (* ptr_voGetLogClientAPI)( LOG_API * ptr_api );

log_client * g_ptr_logger = NULL;

void voGetLogClientAPI( LOG_API * ptr_api )
{
}

log_client::log_client()
:m_log_handle(0)
,m_ref(0)
{
	vostrcpy( m_loader.m_szDllFile , _T("voLog") );
	vostrcpy( m_loader.m_szAPIName , _T("voGetLogClientAPI") );

#if defined _WIN32
	vostrcat(m_loader.m_szDllFile, _T(".Dll"));
#elif defined LINUX
	vostrcat(m_loader.m_szDllFile, _T(".so"));
#endif
  
#ifdef _IOS
	ptr_voGetLogClientAPI getapi = (ptr_voGetLogClientAPI)voGetLogClientAPI;
#else
	m_loader.LoadLib( NULL );
	ptr_voGetLogClientAPI getapi = (ptr_voGetLogClientAPI)m_loader.m_pAPIEntry;
#endif // _IOS
	
	if( getapi )
	{
		getapi( &m_log_api );
		m_log_api.init( &m_log_handle );

		if( m_log_handle )
		{
			m_log_api.set_param( m_log_handle , LOG_PARAM_PREFIX , (void *)"PDLog" );
			m_log_api.set_param( m_log_handle , LOG_PARAM_CONFIGFILE , (void *)"voPDLog.cfg" );
	//		m_log_api.set_param( m_log_handle , LOG_PARAM_PRINT_FILELOG , (void *)"c:/log" );
		}
	}
	else
	{
		memset( &m_log_api , 0 , sizeof(m_log_api) );
	}
}

log_client::~log_client()
{
	if( m_log_api.uninit && m_log_handle )
		m_log_api.uninit( m_log_handle );
}

void log_client::log( LOG_LEVEL level , int line , const char * str_filename , const char * str_funcname , char * fmt , va_list args )
{
	if( m_log_api.print_log && m_log_handle )
		m_log_api.print_log( m_log_handle , level , line , str_filename , str_funcname , fmt , args );
}

void log_client::add_ref()
{
	voCAutoLock lock( &m_ref_lock );
	m_ref++;
}

void log_client::release()
{
	m_ref--;

	if( m_ref == 0 )
	{
		delete this;
		g_ptr_logger = NULL;
	}
}

log_print::log_print( int line , const char * str_funcname , const char * str_filename )
:m_line(line)
{
	char * ptr = strrchr( (char*)str_filename , '\\' );
	if( ptr )
		m_str_filename = ptr + 1;
	else if( (ptr = strrchr( (char*)str_filename , '/' )) )
		m_str_filename = ptr + 1;
	else
		m_str_filename = (char *)str_filename;

	m_str_funcname = (char *)str_funcname;
}

void log_print::operator()( LOG_LEVEL level , char * fmt , ... )
{
	va_list list;
	va_start( list , fmt );
	if( g_ptr_logger )
		g_ptr_logger->log( level , m_line , m_str_filename , m_str_funcname , fmt , list );
	va_end( list );
}

#ifdef _VONAMESPACE
}
#endif