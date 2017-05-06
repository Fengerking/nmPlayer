/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2012				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCRWLock.cpp

	Contains:	voCRWLock class file

	description: it is a light weight readers-writer lock implementation, 
	disadvantage: it may lead to write-starvation if contention is high enough, so please care it.
	advantage:it is especially meaningful when the read process cost time, if you lock it by 
	mutex, it may block other thread to read the resouce.
	
	usage: 1,define the lock and used for protect resouce A,  voCRWLock  rwlock;
		   2,when you just need to access A for read purpose, call rwlock.reader_lock()
		   3,when you finish the access to A for read, call rwlock.reader_unlock()
		   4,when you need to access A for write purpose, call rwlock.writer_lock()
		   5,when you finish the access to A for write, call rwlock.writer_unlock()

	Written by:	Stony

	Change History (most recent first):
	2012-09-20		Stony			Create file
	2012-09-21		Stony			change the way writer lock

*******************************************************************************/
#if defined(_WIN32)
#  include <tchar.h>
#endif

#include "voString.h"
#include "voCRWLock.h"
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

voCRWLock::voCRWLock()
	: m_nReaders (0)
{
	strcpy ((VO_PCHAR)m_szName, __FILE__);

#ifdef _METRO
	InitializeCriticalSectionEx(&m_CritSec , 2000 , 0);
	m_hEvent = CreateEventEx(nullptr , nullptr , CREATE_EVENT_MANUAL_RESET , EVENT_ALL_ACCESS);
#elif defined  _WIN32
	InitializeCriticalSection(&m_CritSec);
	m_hEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
#elif defined _LINUX
  pthread_cond_init (&m_hCondition, NULL);
  pthread_mutexattr_t attr = PTHREAD_MUTEX_RECURSIVE_NP;
  pthread_mutex_init (&m_hMutex,  &attr);
#elif defined __SYMBIAN32__
	_LIT(KRRWlockName,"vommRWLock"); 
	if (KErrNone != m_cSemaphore.OpenGlobal(KRRWlockName)) 
		m_cSemaphore.CreateGlobal(KRRWlockName);
#elif defined(_IOS) || defined(_MAC_OS)
	pthread_cond_init (&m_hCondition, NULL);
	int                   rc=0;
	pthread_mutexattr_t   mta;
	rc = pthread_mutexattr_init(&mta);
	rc = pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);
	rc = pthread_mutex_init (&m_hMutex, &mta);
	rc = pthread_mutexattr_destroy(&mta);
#endif // _WIN32

}

voCRWLock::~voCRWLock()
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

VO_VOID voCRWLock::writer_lock()
{
#ifdef _WIN32

	while( 1 )
	{
		EnterCriticalSection(&m_CritSec);
		if(m_nReaders > 0)
		{
			LeaveCriticalSection(&m_CritSec);
#ifdef _METRO
			WaitForSingleObjectEx (m_hEvent, INFINITE , false);
#else //_METRO
			WaitForSingleObject (m_hEvent, INFINITE);
#endif //_METRO
		}
		else
		{
			break;
		}
	}
	
#elif defined _LINUX
	while( 1 )
	{
		pthread_mutex_lock (&m_hMutex);
		if(m_nReaders > 0)
		{
			pthread_mutex_unlock (&m_hMutex);
			pthread_cond_wait (&m_hCondition, &m_hMutex);
		}
		else
		{
			break;
		}
	}
	
#elif defined __SYMBIAN32__
	while(m_nReaders)
	{
		;
	}
	m_cSemaphore.Wait ();
#elif defined(_IOS) || defined(_MAC_OS)
	while( 1 )
	{
		pthread_mutex_lock (&m_hMutex);
		if(m_nReaders > 0)
		{
			pthread_mutex_unlock (&m_hMutex);
			pthread_cond_wait (&m_hCondition, &m_hMutex);
		}
		else
		{
			break;
		}
	}
	 
#endif // _WIN32
}

VO_VOID voCRWLock::writer_unlock()
{
#ifdef _WIN32
	LeaveCriticalSection(&m_CritSec);
#elif defined _LINUX
	pthread_mutex_unlock (&m_hMutex);
#elif defined __SYMBIAN32__
	m_cSemaphore.Signal ();
#elif defined(_IOS) || defined(_MAC_OS)
	pthread_mutex_unlock (&m_hMutex);
	
#endif // _WIN32
}

VO_VOID voCRWLock::reader_lock()
{
#ifdef _WIN32
	EnterCriticalSection(&m_CritSec);
	m_nReaders++;
	ResetEvent (m_hEvent);
	LeaveCriticalSection(&m_CritSec);
#elif defined _LINUX
	pthread_mutex_lock (&m_hMutex);
	m_nReaders++;
	pthread_mutex_unlock (&m_hMutex);
#elif defined __SYMBIAN32__
	m_nReaders++;
	m_cSemaphore.Wait ();
#elif defined(_IOS) || defined(_MAC_OS)
	pthread_mutex_lock (&m_hMutex);
	m_nReaders++;
	pthread_mutex_unlock (&m_hMutex);
	 
	 
#endif // _WIN32
}

VO_VOID voCRWLock::reader_unlock()
{
#ifdef _WIN32
	EnterCriticalSection(&m_CritSec);

	if(--m_nReaders == 0)
	{
		SetEvent (m_hEvent);
	}

	LeaveCriticalSection(&m_CritSec);
#elif defined _LINUX
	pthread_mutex_lock (&m_hMutex);

	if(--m_nReaders == 0)
	{
		pthread_cond_signal(&m_hCondition);
	}
	
	pthread_mutex_unlock (&m_hMutex);
#elif defined __SYMBIAN32__
	--m_nReaders;
	m_cSemaphore.Signal ();
#elif defined(_IOS) || defined(_MAC_OS)
	
	pthread_mutex_lock (&m_hMutex);

	if(--m_nReaders == 0)
	{
		pthread_cond_signal(&m_hCondition);
	}
	
	pthread_mutex_unlock (&m_hMutex);
	
	
#endif // _WIN32
}


