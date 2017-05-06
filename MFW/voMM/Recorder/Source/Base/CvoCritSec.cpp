#include "CvoCritSec.h"

CvoCritSec::CvoCritSec()
{
#ifdef _WIN32
	::InitializeCriticalSection(&m_CritSec);
#endif // _WIN32
}

CvoCritSec::~CvoCritSec()
{
#ifdef _WIN32
	::DeleteCriticalSection(&m_CritSec);
#endif // _WIN32
}

void CvoCritSec::Lock()
{
#ifdef _WIN32
	::EnterCriticalSection(&m_CritSec);
#endif // _WIN32
}

void CvoCritSec::Unlock()
{
#ifdef _WIN32
	::LeaveCriticalSection(&m_CritSec);
#endif // _WIN32
}

CvoAutoLock::CvoAutoLock(CvoCritSec* pCritSec)
	: m_pCritSec(pCritSec)
{
	if(m_pCritSec)
		m_pCritSec->Lock();
}

CvoAutoLock::~CvoAutoLock()
{
	if(m_pCritSec)
		m_pCritSec->Unlock();
}
