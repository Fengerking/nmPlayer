
#ifndef _VO_THREAD__H

#define _VO_THREAD__H

#include "voThread.h"

class vo_thread
{
public:
	vo_thread():m_thread_handle(0){}
	virtual ~vo_thread(){}

	virtual VO_VOID start_thread()
	{
		VO_U32 thread_id;
		voThreadHandle thread_handle;
		voThreadCreate( &thread_handle , &thread_id , threadfunc , this , 0 );
	}

	static VO_U32 threadfunc( VO_PTR pParam )
	{
		vo_thread * ptr_thread = (vo_thread *)pParam;
		ptr_thread->m_thread_handle = (voThreadHandle)1;
		ptr_thread->thread_function();
		ptr_thread->m_thread_handle = 0;
		return 0;
	}

	virtual VO_VOID thread_function() = 0;

	virtual VO_VOID stop_thread() = 0;

protected:
	voThreadHandle m_thread_handle;
};



#endif