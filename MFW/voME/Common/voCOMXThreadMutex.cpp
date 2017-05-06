	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXThreadMutex.cpp

	Contains:	voCOMXThreadMutex class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-02-16		JBF			Create file

*******************************************************************************/
#include "voCOMXThreadMutex.h"
#if defined LINUX
#include <string.h>
#endif

#define LOG_TAG "voCOMXThreadMutex"
#include "voLog.h"

voCOMXThreadMutex::voCOMXThreadMutex()
{
#ifdef _WIN32
    InitializeCriticalSection(&m_CritSec);
#elif defined _LINUX_ANDROID
    pthread_mutexattr_t attr = PTHREAD_MUTEX_RECURSIVE_NP;
	pthread_mutex_init (&m_hMutex, &attr);
#endif // _WIN32

    m_nCurrentOwner = 0;
	m_nLockCount = 0;
}

voCOMXThreadMutex::~voCOMXThreadMutex()
{
#ifdef _WIN32
    DeleteCriticalSection(&m_CritSec);
#elif defined LINUX
	pthread_mutex_destroy (&m_hMutex);
#endif // _WIN32
}

void voCOMXThreadMutex::Lock()
{
	unsigned int nNewOwner = 0;
#ifdef _WIN32
    nNewOwner = GetCurrentThreadId();
    EnterCriticalSection(&m_CritSec);
#elif defined LINUX// _WIN32
    nNewOwner = pthread_self();
//	if (nNewOwner != m_nCurrentOwner)
		pthread_mutex_lock (&m_hMutex);
#endif

	if (0 == m_nLockCount++)
        m_nCurrentOwner = nNewOwner;
}

void voCOMXThreadMutex::Unlock()
{
	unsigned int nNewOwner = 0;

    if (0 == --m_nLockCount)
        m_nCurrentOwner = 0;

#ifdef _WIN32
    LeaveCriticalSection(&m_CritSec);
#elif defined LINUX
    nNewOwner = pthread_self();
//	if (nNewOwner != m_nCurrentOwner)
		pthread_mutex_unlock (&m_hMutex);
#endif // _WIN32
}
