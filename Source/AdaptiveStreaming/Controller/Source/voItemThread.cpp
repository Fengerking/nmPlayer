#include "voItemThread.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voItemThread::voItemThread(void)
:m_ptr_info(0)
{
}

voItemThread::~voItemThread(void)
{
	if( m_ptr_info )
		delete m_ptr_info;
}

VO_VOID voItemThread::Start( THREADINFO * ptr_info )
{
	if( m_ptr_info )
		delete m_ptr_info;

	m_ptr_info = ptr_info;

	begin();
}

VO_VOID voItemThread::Resume()
{
	if( !m_ptr_info )
		return;

	begin();
}

VO_VOID voItemThread::Stop()
{
	stop();
}
