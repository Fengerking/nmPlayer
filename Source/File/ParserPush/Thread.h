#pragma once
#include "voType.h"


class CThread
{
public:
	CThread(void);
	virtual ~CThread(void);

	VO_BOOL ThreadStart();

	virtual VO_VOID ThreadMain() = 0;

private:
	void* m_hThread;
};

