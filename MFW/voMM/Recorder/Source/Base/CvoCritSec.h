#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32
class CvoCritSec
{
public:
	CvoCritSec();
	virtual ~CvoCritSec();

public:
	void	Lock();
	void	Unlock();

protected:
#ifdef _WIN32
	CRITICAL_SECTION	m_CritSec;
#endif // _WIN32
};

class CvoAutoLock
{
public:
	CvoAutoLock(CvoCritSec* pCritSec);
	virtual ~CvoAutoLock();

protected:
	CvoCritSec*			m_pCritSec;
};

