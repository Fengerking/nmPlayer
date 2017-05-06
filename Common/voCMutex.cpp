	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCMutex.cpp

	Contains:	voCMutex class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-02-16		JBF			Create file

*******************************************************************************/
//#include "stdafx.h"
#include "voString.h"

#include "voCMutex.h"

#define LOG_TAG "voCMutex"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voCMutex::voCMutex()
{
//	strcpy ((VO_PCHAR)m_szName, __FILE__);
#ifdef _METRO
	InitializeCriticalSectionEx(&m_CritSec , 2000 , 0);
#elif defined  _WIN32
    InitializeCriticalSection(&m_CritSec);
#elif defined NNJ
	pthread_mutexattr_t attr;
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init (&m_hMutex, &attr);
#elif defined _LINUX_ANDROID
	pthread_mutexattr_t attr = PTHREAD_MUTEX_RECURSIVE_NP;
	pthread_mutex_init (&m_hMutex, &attr);
#elif defined __SYMBIAN32__
	_LIT(KRMutexName,"vommMutex"); 
	if (KErrNone != m_cMutex.OpenGlobal(KRMutexName)) 
		m_cMutex.CreateGlobal(KRMutexName);
#elif defined(_IOS) || defined(_MAC_OS)
	int                   rc=0;
	pthread_mutexattr_t   mta;
	rc = pthread_mutexattr_init(&mta);
	rc = pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);
	
	rc = pthread_mutex_init (&m_hMutex, &mta);
	rc = pthread_mutexattr_destroy(&mta);
	
	//pthread_mutex_init (&m_hMutex, NULL);	
#endif // _WIN32

    m_nCurrentOwner = 0;
	m_nLockCount = 0;
}

voCMutex::~voCMutex()
{
#ifdef _WIN32
    DeleteCriticalSection(&m_CritSec);
#elif defined LINUX
	pthread_mutex_destroy (&m_hMutex);
#elif defined __SYMBIAN32__
	m_cMutex.Close();
#elif defined(_IOS) || defined(_MAC_OS)
	pthread_mutex_destroy (&m_hMutex);
#endif // _WIN32

}

void voCMutex::Lock()
{
	unsigned int nNewOwner = 0;
#ifdef _WIN32
    nNewOwner = GetCurrentThreadId();
    EnterCriticalSection(&m_CritSec);
#elif defined LINUX// _WIN32
	nNewOwner = pthread_self();
	pthread_mutex_lock (&m_hMutex);
#elif defined __SYMBIAN32__
	m_cMutex.Wait();
#elif defined(_IOS) || defined(_MAC_OS)
	//nNewOwner = (unsigned int)pthread_self();
	//VOLOGI("lock mutex = %d", m_hMutex);
	pthread_mutex_lock (&m_hMutex);	
#endif

    if (0 == m_nLockCount++)
        m_nCurrentOwner = nNewOwner;
}

void voCMutex::Unlock()
{
    if (0 == --m_nLockCount)
        m_nCurrentOwner = 0;

#ifdef _WIN32
    LeaveCriticalSection(&m_CritSec);
#elif defined LINUX
	pthread_mutex_unlock (&m_hMutex);
#elif defined __SYMBIAN32__
	m_cMutex.Signal();
#elif defined(_IOS) || defined(_MAC_OS)
	pthread_mutex_unlock (&m_hMutex);	
#endif // _WIN32
}
