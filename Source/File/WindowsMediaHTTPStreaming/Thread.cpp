#include "Thread.h"
#include "voThread.h"
#include "voLog.h"
#include "voOSFunc.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

VO_U32 CThread::ThreadFunc(VO_PTR params)
{
	CThread* pThread = (CThread*)(params);

	pThread->ThreadMain();

	pThread->m_hThread = NULL;

	return 0;
}


CThread::CThread(const char * pThreadName)
	: m_hThread(NULL)
{
	if (pThreadName != NULL)
		strcpy (m_szName, pThreadName);
	else
		strcpy (m_szName, "Unknow name");
}


CThread::~CThread(void)
{
}


VO_U32 CThread::ThreadStart()
{
	if (m_hThread != NULL)
		return -1;

	VO_U32 ID = 0;

	return voThreadCreate(&m_hThread, &ID, ThreadFunc, this, 0);
}

VO_U32 CThread::WaitingThreadExit()
{
	VO_S32 iTimes = 0;
	while (m_hThread != NULL)
	{
		voOS_Sleep(20);

		iTimes++;
		if (iTimes > 400)
		{
			VOLOGW ("The thread %s could not safe exit", m_szName);
			break;
		}
	}

	VOLOGI("consider thread %s exit", m_szName);

	return 0;
}
