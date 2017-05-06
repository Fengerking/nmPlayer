#include "osCMutex.h"

#define LOG_TAG "osCMutex"

osCMutex::osCMutex()
{
#ifdef _METRO
	InitializeCriticalSectionEx(&m_CritSec , 2000 , 0);
#elif defined  _WIN32
    InitializeCriticalSection(&m_CritSec);
#elif defined _LINUX_ANDROID
	pthread_mutexattr_t attr = PTHREAD_MUTEX_RECURSIVE_NP;
	pthread_mutex_init (&m_hMutex, &attr);
#elif defined(_IOS) || defined(_MAC_OS)
	int                   rc=0;
	pthread_mutexattr_t   mta;
	rc = pthread_mutexattr_init(&mta);
	rc = pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);
	
	rc = pthread_mutex_init (&m_hMutex, &mta);
	rc = pthread_mutexattr_destroy(&mta);
	
	//pthread_mutex_init (&m_hMutex, NULL);	
#endif // _WIN32
}

osCMutex::~osCMutex()
{
#ifdef _WIN32
    DeleteCriticalSection(&m_CritSec);
#elif defined _LINUX_ANDROID
	pthread_mutex_destroy (&m_hMutex);
#elif defined(_IOS) || defined(_MAC_OS)
	pthread_mutex_destroy (&m_hMutex);
#endif // _WIN32

}

void osCMutex::Lock()
{
#ifdef _WIN32
    EnterCriticalSection(&m_CritSec);
#elif defined _LINUX_ANDROID// _WIN32
	pthread_mutex_lock (&m_hMutex);
#elif defined(_IOS) || defined(_MAC_OS)
	pthread_mutex_lock (&m_hMutex);	
#endif

}

void osCMutex::Unlock()
{
#ifdef _WIN32
    LeaveCriticalSection(&m_CritSec);
#elif defined _LINUX_ANDROID
	pthread_mutex_unlock (&m_hMutex);
#elif defined(_IOS) || defined(_MAC_OS)
	pthread_mutex_unlock (&m_hMutex);	
#endif // _WIN32
}
