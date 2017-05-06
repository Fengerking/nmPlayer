#ifndef __CThread_H__
#define __CThread_H__

#include "voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class CThread
{
public:
	CThread(const char * pThreadName);
	virtual ~CThread(void);

	static VO_U32 ThreadFunc(VO_PTR params);
	VO_U32 ThreadStart();

	VO_U32 WaitingThreadExit();

	virtual VO_VOID ThreadMain() = 0;
private:
	void* m_hThread;
	char	m_szName[128];
};

#ifdef _VONAMESPACE
}
#endif

#endif //__CThread_H__