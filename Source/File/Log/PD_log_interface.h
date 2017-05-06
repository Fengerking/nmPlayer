
#ifndef __PD_LOG_INTERFACE_H__

#define __PD_LOG_INTERFACE_H__


#include <stdarg.h>

#if defined __cplusplus
extern "C" {
#endif

enum LOG_LEVEL
{
	LOG_LEVEL_CRITICAL = 0,
	LOG_LEVEL_IMPORTANT = 1,
	LOG_LEVEL_NORMAL = 2,
	LOG_LEVEL_DETAIL = 3,
	LOG_LEVEL_ALLINFO = 4,
};

enum LOG_PARAM
{
	LOG_PARAM_PREFIX,			//param will be char*, strlen should be less than 254
	LOG_PARAM_PRINT_LOGCAT,		//param will be bool*
	LOG_PARAM_LOGCAT_LEVEL,		//param will be LOG_LEVEL*
	LOG_PARAM_PRINT_FILELOG,	//param will be char*, it is the file path
	LOG_PARAM_FILELOG_LEVEL,	//param will be LOG_LEVEL*
	LOG_PARAM_CONFIGFILE,		//param will be char*, it is the config file name
};

typedef void * LOG_HANDLE;

typedef void (* ptr_init )( LOG_HANDLE * ptr_handle );
typedef void (* ptr_uninit )( LOG_HANDLE handle );
typedef void (* ptr_set_param )( LOG_HANDLE handle , int id , void * param );
typedef void (* ptr_print_log )( LOG_HANDLE handle , LOG_LEVEL level , int line , const char * str_filename , const char * str_funcname , char * fmt , va_list args );

struct LOG_API
{
	ptr_init init;
	ptr_uninit uninit;
	ptr_print_log print_log;
	ptr_set_param set_param;
};

extern void voGetLogClientAPI( LOG_API * ptr_api );

#if defined __cplusplus
}
#endif

#endif