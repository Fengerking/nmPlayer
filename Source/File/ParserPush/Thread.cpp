#include "Thread.h"
#include "voThread.h"


VO_U32 ThreadFunc(VO_PTR params) {
	CThread* pThread = static_cast<CThread*>(params);

	pThread->ThreadMain();

	return NULL;
}


CThread::CThread(void)
	: m_hThread(NULL)
{
}


CThread::~CThread(void)
{
}


VO_BOOL CThread::ThreadStart()
{
	VO_U32 ID = 0;

	if (voThreadCreate(&m_hThread, &ID, ThreadFunc, this, 0) != VO_ERR_NONE)
		return VO_FALSE;
	else
		return VO_TRUE;
}