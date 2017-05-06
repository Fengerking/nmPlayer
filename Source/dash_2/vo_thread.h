
#ifndef __VO_THREAD_H__

#define __VO_THREAD_H__

typedef unsigned int (* ptr_thread_func) (void * pParam);
typedef void * THREAD_HANDLE;

unsigned int create_thread( THREAD_HANDLE * ptr_handle, unsigned int * ptr_id, ptr_thread_func ptr_func, void * pParam, unsigned int uFlag);
void exit_thread();
void wait_thread_exit( THREAD_HANDLE thread_handle );
void set_threadname( char * ptr_name );

class vo_thread
{
public:
	vo_thread():m_thread_handle(0){}
	virtual ~vo_thread(){}

protected:

	virtual void begin()
	{
		unsigned int thread_id;
		create_thread( &m_thread_handle , &thread_id , threadfunc , this , 0 );
	}

	static unsigned int threadfunc( void * pParam )
	{
		vo_thread * ptr_thread = (vo_thread *)pParam;
		ptr_thread->thread_function();
		exit_thread();
		return 0;
	}

	virtual void thread_function() = 0;

	virtual void stop()
	{
		wait_thread_exit( m_thread_handle );
		m_thread_handle = 0;
	}

protected:
	THREAD_HANDLE m_thread_handle;
};

#endif