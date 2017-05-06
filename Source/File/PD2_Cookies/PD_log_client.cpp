// PD_log_client.cpp : Defines the exported functions for the DLL application.
//

//#include "stdafx.h"
#include "PD_log_interface.h"
#include "CBaseConfig.h"
#include "voCMutex.h"
#include "voOSFunc.h"

#ifdef _LINUX_ANDROID
#include <pthread.h>
#include <utils/Log.h>
#endif // _LINUX_ANDROID

#ifdef _LINUX_X86
#include <pthread.h>
#endif	//_LINUX_X86

#ifdef _WIN32
#include "windows.h"
#endif

#define LOG_TAG "PD_log_client"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#else
#ifdef __cplusplus
     extern "C" {
#endif /* __cplusplus */
#endif 

struct LOG_STRUCT
{
	char * prefix;

	FILE * log_fp;
	LOG_LEVEL filelog_level;

	bool is_logcat;
	LOG_LEVEL logcat_level;
};

void PDLOG_init( LOG_HANDLE * ptr_handle );
void PDLOG_uninit( LOG_HANDLE handle );
void PDLOG_print_log( LOG_HANDLE handle , LOG_LEVEL level , int line , const char * str_filename , const char * str_funcname , char * fmt , va_list args );
void PDLOG_set_param( LOG_HANDLE handle , int id , void * param );
static int get_current_threadid();
static void get_time( char * str );

voCMutex g_log_lock;

static const char * str_level[] = {"[*    ]",
							"[**   ]",
							"[***  ]",
							"[**** ]",
							"[*****]"};

void voGetLogClientAPI( LOG_API * ptr_api )
{
	ptr_api->init = PDLOG_init;
	ptr_api->uninit = PDLOG_uninit;
	ptr_api->print_log = PDLOG_print_log;
	ptr_api->set_param = PDLOG_set_param;
}

void PDLOG_init( LOG_HANDLE * ptr_handle )
{
	LOG_STRUCT * ptr_log = new LOG_STRUCT;

	memset( ptr_log , 0 , sizeof( LOG_STRUCT ) );

	ptr_log->is_logcat = true;
#ifdef _IOS
	ptr_log->logcat_level =  LOG_LEVEL_ALLINFO;
#else
	ptr_log->logcat_level =  LOG_LEVEL_CRITICAL;
#endif
	
	ptr_log->filelog_level = LOG_LEVEL_DETAIL;

	*ptr_handle = ptr_log;
}

void PDLOG_uninit( LOG_HANDLE handle )
{
	LOG_STRUCT * ptr_log = ( LOG_STRUCT * )handle;
	if( ptr_log->log_fp )
		fclose( ptr_log->log_fp );

	if( ptr_log->prefix )
		delete []ptr_log->prefix;

	delete ptr_log;
}

void PDLOG_print_log( LOG_HANDLE handle , LOG_LEVEL level , int line , const char * str_filename , const char * str_funcname , char * fmt , va_list args )
{
	LOG_STRUCT * ptr_log = ( LOG_STRUCT * )handle;
	if( level > ptr_log->logcat_level && level > ptr_log->filelog_level )
		return;

	voCAutoLock lock( &g_log_lock );

	char str[2048];
	char str_template[2048];
	char time[50];

	get_time( time );

	if( ptr_log->prefix )
		sprintf( str_template , "%s  %s%s Thread %d :  %s  %s  %d    %s\r\n" , time , ptr_log->prefix , str_level[level] , get_current_threadid() , str_filename , str_funcname , line , fmt );
	else
		sprintf( str_template , "%s  %s Thread %d :  %s  %s  %d    %s\r\n" , time , str_level[level] , get_current_threadid() , str_filename , str_funcname , line , fmt );
	int size = vsprintf( str , str_template , args );

	if( ptr_log->is_logcat && level <= ptr_log->logcat_level )
	{
#ifdef _WIN32
		OutputDebugStringA( str );
#elif defined _LINUX_ANDROID
		if( size < 1024 )
			LOGI(str);
#elif defined _LINUX_X86
		printf( str );
#elif defined _IOS
		printf( str );
#elif defined _MAC_OS
		printf( str );
#else
		;
#endif
	}

	if( ptr_log->log_fp && level <= ptr_log->filelog_level )
	{
		fwrite( str , size , 1 , ptr_log->log_fp );
		fflush( ptr_log->log_fp );
	}
}

void PDLOG_set_param( LOG_HANDLE handle , int id , void * param )
{
	LOG_STRUCT * ptr_log = ( LOG_STRUCT * )handle;

	switch( id )
	{
	case LOG_PARAM_PREFIX:			//param will be char*, strlen should be less than 254
		{
			char *ptr = (char *)param;

			if( ptr_log->prefix )
			{
				delete []ptr_log->prefix;
				ptr_log->prefix = 0;
			}

			ptr_log->prefix = new char[ strlen( ptr ) + 1 ];
			strcpy( ptr_log->prefix , ptr );
		}
		break;
	case LOG_PARAM_PRINT_LOGCAT:		//param will be bool*
		{
			bool * ptr = ( bool * )param;
			ptr_log->is_logcat = *ptr;
		}
		break;
	case LOG_PARAM_LOGCAT_LEVEL:		//param will be LOG_LEVEL
		{
			LOG_LEVEL * ptr = ( LOG_LEVEL * )param;
			ptr_log->logcat_level = *ptr;
		}
		break;
	case LOG_PARAM_PRINT_FILELOG:	//param will be char*, it is the file path
		{
			char * ptr = ( char * )param;

			if( ptr_log->log_fp )
				fclose( ptr_log->log_fp );

			ptr_log->log_fp = 0;

			ptr_log->log_fp = fopen( ptr , "wb+" );
		}
		break;
	case LOG_PARAM_FILELOG_LEVEL:	//param will be LOG_LEVEL
		{
			LOG_LEVEL * ptr = ( LOG_LEVEL * )param;
			ptr_log->filelog_level = *ptr;
		}
		break;
	case LOG_PARAM_CONFIGFILE:		//param will be char*, it is the config file name
		{
			char * ptr = ( char * )param;
			CBaseConfig cfg;

			cfg.Open( (TCHAR*)ptr );

			char * path = cfg.GetItemText( "FileLog" , "Path" );

			if( path )
			{
				if( ptr_log->log_fp )
					fclose( ptr_log->log_fp );

				ptr_log->log_fp = 0;
				ptr_log->log_fp = fopen( path , "wb+" );
			}

			ptr_log->is_logcat = cfg.GetItemValue( "Logcat" , "On" , ptr_log->is_logcat );

			ptr_log->filelog_level = (LOG_LEVEL)cfg.GetItemValue( "FileLog" , "LogLevel" , ptr_log->filelog_level );
			ptr_log->logcat_level = (LOG_LEVEL)cfg.GetItemValue( "Logcat" , "LogLevel" , ptr_log->logcat_level );
		}
		break;
	}
}

static int get_current_threadid()
{
#ifdef _WIN32
	return GetCurrentThreadId();
#elif defined _LINUX_ANDROID
	return (int)pthread_self();
#elif defined _LINUX_X86
	return (int)pthread_self();
#else
	return 0;
#endif
}

static void get_time( char * str )
{
	unsigned int systime = voOS_GetSysTime();

	int milisec = systime % 1000;
	systime = systime / 1000;
	int sec = systime % 60;
	systime = systime / 60;
	int min = systime % 60;
	int hour = systime / 60;

	sprintf( str , "%02d:%02d:%02d.%03d" , hour , min , sec , milisec );
}

#ifdef _VONAMESPACE
 }
#else
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _VONAMESPACE */