
#ifndef __VO_THREAD_H__

#define __VO_THREAD_H__

#include "voYYDef_SrcCmn.h"
#include "string.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

typedef unsigned int (* ptr_thread_func) (void * pParam);
typedef void * THREAD_HANDLE;

unsigned int create_thread( THREAD_HANDLE * ptr_handle, unsigned int * ptr_id, ptr_thread_func ptr_func, void * pParam, unsigned int uFlag);
void exit_thread();
void wait_thread_exit( THREAD_HANDLE thread_handle );
void set_threadname( const char * ptr_name );
int get_current_threadid (void);

class vo_thread
{
public:
	vo_thread()
		:m_thread_handle(0)
		,m_is_thread_running( false )
	{
		memset( m_thread_name , 0 , sizeof( m_thread_name ) );
	}

	virtual ~vo_thread()
	{
		stop();
	}

protected:

	virtual void begin( char * ptr_thread_name = 0 )
	{
		unsigned int thread_id;

		stop();

		if( ptr_thread_name )
			strcpy( m_thread_name , ptr_thread_name );
		else
			memset( m_thread_name , 0 , sizeof( m_thread_name ) );

		create_thread( &m_thread_handle , &thread_id , threadfunc , this , 0 );
	}

	static unsigned int threadfunc( void * pParam )
	{
		vo_thread * ptr_thread = (vo_thread *)pParam;

		ptr_thread->m_is_thread_running = true;
		
		if( strlen( ptr_thread->m_thread_name ) > 0 )
			set_threadname( ptr_thread->m_thread_name );

		ptr_thread->thread_function();

		ptr_thread->m_is_thread_running = false;

		exit_thread();

		return 0;
	}

	virtual void thread_function() = 0;

	bool is_thread_running()
	{
		return m_is_thread_running;
	}

	virtual void stop()
	{
		wait_thread_exit( m_thread_handle );
		m_thread_handle = 0;
	}

protected:
	THREAD_HANDLE m_thread_handle;
	char		  m_thread_name[256];
	bool		  m_is_thread_running;
};

// End define
#ifdef _VONAMESPACE
}
#else
#endif /* _VONAMESPACE */

#endif