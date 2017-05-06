	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCSemaphore.cpp

	Contains:	voCSemaphore class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-02-16		JBF			Create file

*******************************************************************************/
#if defined(_WIN32)
#  include <tchar.h>
#endif

#include "voString.h"
#include "voCSemaphore.h"
#ifndef WINCE
#include "errno.h"
#endif //WINCE
#include "string.h"

#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
#include "sys/time.h"
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voCSemaphore::voCSemaphore()
	: m_nSemCount (0)
	, m_bWaiting (VO_FALSE)
{
//	strcpy ((VO_PCHAR)m_szName, __FILE__);

#ifdef _METRO
	InitializeCriticalSectionEx(&m_CritSec , 2000 , 0);
	m_hEvent = CreateEventEx(nullptr , nullptr , CREATE_EVENT_MANUAL_RESET , EVENT_ALL_ACCESS);
#elif defined  _WIN32
	InitializeCriticalSection(&m_CritSec);
	m_hEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
#elif defined _LINUX
  pthread_cond_init (&m_hCondition, NULL);
  pthread_mutex_init (&m_hMutex, NULL);
#elif defined __SYMBIAN32__
	_LIT(KRSemaphoreName,"vommSemaphore"); 
	if (KErrNone != m_cSemaphore.OpenGlobal(KRSemaphoreName)) 
		m_cSemaphore.CreateGlobal(KRSemaphoreName, 1);
#elif defined(_IOS) || defined(_MAC_OS)
	pthread_cond_init (&m_hCondition, NULL);
	pthread_mutex_init (&m_hMutex, NULL);
#endif // _WIN32

}

voCSemaphore::~voCSemaphore()
{
#ifdef _WIN32
	CloseHandle (m_hEvent);
	DeleteCriticalSection(&m_CritSec);
#elif defined _LINUX
	pthread_cond_destroy (&m_hCondition);
	pthread_mutex_destroy (&m_hMutex);
#elif defined __SYMBIAN32__
	m_cSemaphore.Close();
#elif defined(_IOS) || defined(_MAC_OS)
	pthread_cond_destroy (&m_hCondition);
	pthread_mutex_destroy (&m_hMutex);	
	 
#endif // _WIN32
}

VO_U32 voCSemaphore::Down (VO_U32 nWaitTime)
{
	m_bWaiting = VO_TRUE;

#ifdef _WIN32
	VO_U32 uRC = 0;
	if (m_nSemCount == 0)
	{
#ifdef _METRO
		uRC = WaitForSingleObjectEx (m_hEvent, nWaitTime , false);
#else //_METRO
		uRC = WaitForSingleObject (m_hEvent, nWaitTime);
#endif //_METRO
		if (uRC == WAIT_TIMEOUT)
			return VOVO_SEM_TIMEOUT;
	}

	EnterCriticalSection(&m_CritSec);

	ResetEvent (m_hEvent);

	if (m_nSemCount > 0)
		m_nSemCount--;
	
	LeaveCriticalSection(&m_CritSec);
#elif defined _LINUX
	pthread_mutex_lock (&m_hMutex);
	while (m_nSemCount == 0)
	{
		//pthread_cond_wait (&m_hCondition, &m_hMutex);
	  struct timespec t;
		// david modified to support waiting infinitely 2011-05-12, nWaitTime must be ms
	  timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
	  t.tv_sec = ts.tv_sec + nWaitTime / 1000;
	  t.tv_nsec = ts.tv_nsec + nWaitTime % 1000 * 1000000;
		t.tv_sec += t.tv_nsec / 1000000000;
		t.tv_nsec %= 1000000000;
	  int err = pthread_cond_timedwait (&m_hCondition, &m_hMutex, &t);
	  if(0 != err)
	  {
	  	pthread_mutex_unlock (&m_hMutex);
		  return VOVO_SEM_TIMEOUT;
	  }
	}
	m_nSemCount--;
	pthread_mutex_unlock (&m_hMutex);
#elif defined __SYMBIAN32__
	m_nSemCount--;
	m_cSemaphore.Wait ();
#elif defined(_IOS) || defined(_MAC_OS)
	pthread_mutex_lock (&m_hMutex);
	while (m_nSemCount == 0)
	{
		struct timeval tv;
        struct timespec ts;
        gettimeofday(&tv, NULL);
        ts.tv_sec = tv.tv_sec + nWaitTime / 1000;
        ts.tv_nsec = tv.tv_usec * 1000 + (nWaitTime % 1000 * 1000000);
		ts.tv_sec = ts.tv_sec + ts.tv_nsec / 1000000000;
		ts.tv_nsec = ts.tv_nsec % 1000000000;
		
		int err = pthread_cond_timedwait (&m_hCondition, &m_hMutex, &ts);
        if(0 != err)
        {
            pthread_mutex_unlock (&m_hMutex);
            return	VOVO_SEM_TIMEOUT;
        }
	}
	m_nSemCount--;
	pthread_mutex_unlock (&m_hMutex);
	 
	 
#endif // _WIN32

	m_bWaiting = VO_FALSE;

	return 0;
}

VO_U32 voCSemaphore::Up (void)
{
 #ifdef _WIN32
	EnterCriticalSection(&m_CritSec);

	m_nSemCount++;
	SetEvent (m_hEvent);

	LeaveCriticalSection(&m_CritSec);
#elif defined _LINUX
	pthread_mutex_lock (&m_hMutex);
	m_nSemCount++;
	pthread_cond_signal(&m_hCondition);
	pthread_mutex_unlock (&m_hMutex);
#elif defined __SYMBIAN32__
	m_nSemCount++;
	m_cSemaphore.Signal ();
#elif defined(_IOS) || defined(_MAC_OS)
	
	pthread_mutex_lock (&m_hMutex);
	m_nSemCount++;
	pthread_cond_signal(&m_hCondition);
	pthread_mutex_unlock (&m_hMutex);
	
	
#endif // _WIN32

	return 0;
}

VO_U32 voCSemaphore::Reset(void)
{
#ifdef _WIN32
	EnterCriticalSection(&m_CritSec);

	m_nSemCount = 0;
	ResetEvent (m_hEvent);

	LeaveCriticalSection(&m_CritSec);
#elif defined _LINUX
  pthread_mutex_lock (&m_hMutex);
  m_nSemCount = 0;
  pthread_mutex_unlock (&m_hMutex);
#elif defined(_IOS) || defined(_MAC_OS)
	
	pthread_mutex_lock (&m_hMutex);
	m_nSemCount = 0;
	pthread_mutex_unlock (&m_hMutex);
	 
#endif // _WIN32

	return 0;
}

VO_U32 voCSemaphore::Wait (VO_U32 nWaitTime)
{
	VO_U32 ret = VOVO_SEM_OK;

#ifdef _WIN32
#ifdef _METRO
	if (WaitForSingleObjectEx (m_hEvent, nWaitTime , false) == WAIT_TIMEOUT)
#else //_METRO
	if (WaitForSingleObject(m_hEvent, nWaitTime) == WAIT_TIMEOUT)
#endif //_METRO
		return VOVO_SEM_TIMEOUT;

	ResetEvent (m_hEvent);
#elif defined _LINUX
  pthread_mutex_lock (&m_hMutex);

  // modified by Lin Jun 20090630
  //pthread_cond_wait (&m_hCondition, &m_hMutex);

  struct timespec t;
	// david modified to support waiting infinitely 2011-05-12, nWaitTime must be ms
  timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
  t.tv_sec = ts.tv_sec + nWaitTime / 1000;
  t.tv_nsec = ts.tv_nsec + nWaitTime % 1000 * 1000000;
	t.tv_sec += t.tv_nsec / 1000000000;
	t.tv_nsec %= 1000000000;
  int err = pthread_cond_timedwait (&m_hCondition, &m_hMutex, &t);
  if(0 != err)
  {
	  ret =	VOVO_SEM_TIMEOUT;
  }

  pthread_mutex_unlock (&m_hMutex);
#elif defined(_IOS) || defined(_MAC_OS)
	
	pthread_mutex_lock (&m_hMutex);
	
	struct timeval tv;
	struct timespec ts;
	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec + nWaitTime / 1000;
	ts.tv_nsec = tv.tv_usec * 1000 + (nWaitTime % 1000 * 1000000);
	ts.tv_sec = ts.tv_sec + ts.tv_nsec / 1000000000;
	ts.tv_nsec = ts.tv_nsec % 1000000000;
	
	int err = pthread_cond_timedwait (&m_hCondition, &m_hMutex, &ts);
	if(0 != err)
		ret =	VOVO_SEM_TIMEOUT;
	
	pthread_mutex_unlock (&m_hMutex);	

#endif // _WIN32

	return ret;
}

VO_U32 voCSemaphore::Signal (void)
{
#ifdef _WIN32
	SetEvent (m_hEvent);
#elif defined _LINUX
  pthread_mutex_lock (&m_hMutex);
  pthread_cond_signal (&m_hCondition);
  pthread_mutex_unlock (&m_hMutex);
#elif defined(_IOS) || defined(_MAC_OS)
	
	pthread_mutex_lock (&m_hMutex);
	pthread_cond_signal (&m_hCondition);
	pthread_mutex_unlock (&m_hMutex);
	 
#endif // _WIN32

	return 0;
}

VO_U32 voCSemaphore::Count (void)
{
	return m_nSemCount;
}

VO_BOOL voCSemaphore::Waiting (void)
{
	return (VO_BOOL)(m_bWaiting&&m_nSemCount<=0);
}
