#ifndef __LOG_H__

#define __LOG_H__
#include "CDllLoad.h"
#include "PD_log_interface.h"
#include "voCMutex.h"

class log_client
{
public:
	log_client();
	~log_client();

	void log( LOG_LEVEL level , int line , const char * str_filename , const char * str_funcname , char * fmt , va_list args );

	void add_ref();
	void release();
	
private:
	CDllLoad m_loader;

	LOG_API m_log_api;

	LOG_HANDLE m_log_handle;
	
	voCMutex m_ref_lock;
	VO_S32 m_ref;
};

extern log_client * g_ptr_logger;

class log_print
{
public:
	log_print( int line , const char * str_funcname , const char * str_filename );

	void operator()( LOG_LEVEL level , char * fmt , ... );

private:
	int m_line;
	char * m_str_funcname;
	char * m_str_filename;
};

//#ifdef LOG
#define PRINT_LOG log_print( __LINE__ , __FUNCTION__ , __FILE__ )
//#else
//#define PRINT_LOG
//#endif


#endif