/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2012				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCRWLock.h

	Contains:	voCRWLock header file

	Written by:	Stony

	Change History (most recent first):
	2012-09-20		Stony			Create file

*******************************************************************************/
#ifndef __voCRWLock_H__
#define __voCRWLock_H__

#ifdef _WIN32
#include "voYYDef_Common.h"
#include <windows.h>
#elif defined _LINUX
#include <pthread.h>
#include <time.h>
#elif defined __SYMBIAN32__
#include <e32std.h>
#elif defined(_IOS) || defined(_MAC_OS)
#include <pthread.h>
#endif // _WIN32

#include "voType.h"
#include "CvoBaseObject.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

// wrapper for whatever critical section we have
class voCRWLock : public CvoBaseObject
{
public:
    voCRWLock(void);
    virtual ~voCRWLock(void);

	VO_VOID writer_lock();
	VO_VOID writer_unlock();
	VO_VOID reader_lock();
	VO_VOID reader_unlock();

#ifdef _WIN32
	CRITICAL_SECTION	m_CritSec;
	HANDLE				m_hEvent;
#elif defined _LINUX
  pthread_cond_t		m_hCondition;
  pthread_mutex_t		m_hMutex;
#elif defined __SYMBIAN32__
	RSemaphore			m_cSemaphore;
	RMutex				m_cMutex;
#elif defined(_IOS) || defined(_MAC_OS)
	pthread_cond_t		m_hCondition;
	pthread_mutex_t		m_hMutex;	
#endif // _WIN32

public:
	volatile VO_U32		m_nReaders;
};

// locks a critical section as Reader, and unlocks it automatically
// when the lock goes out of scope
class voCAutoReadLock
{
protected:
    voCRWLock * m_pLock;

public:
    voCAutoReadLock(voCRWLock * plock)
    {
        m_pLock = plock;
        if (m_pLock) {
            m_pLock->reader_lock();
        }
    };

    ~voCAutoReadLock()
	{
        if (m_pLock) {
            m_pLock->reader_unlock();
        }
    };
};

// locks a critical section as Writer, and unlocks it automatically
// when the lock goes out of scope
class voCAutoWriteLock
{
protected:
    voCRWLock * m_pLock;

public:
    voCAutoWriteLock(voCRWLock * plock)
    {
        m_pLock = plock;
        if (m_pLock) {
			m_pLock->writer_lock();
        }
    };

    ~voCAutoWriteLock()
	{
        if (m_pLock) {
			m_pLock->writer_unlock();
        }
    };
};

#ifdef _VONAMESPACE
}
#endif

#endif //__voCRWLock_H__