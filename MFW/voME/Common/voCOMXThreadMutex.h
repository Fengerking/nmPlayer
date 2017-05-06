	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXThreadMutex.h

	Contains:	voCOMXThreadMutex header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-04		JBF			Create file

*******************************************************************************/
#ifndef __voCOMXThreadMutex_H__
#define __voCOMXThreadMutex_H__

#include "OMX_Types.h"

#ifdef _WIN32
#include <windows.h>
#elif defined LINUX
#include <pthread.h>
#endif // _WIN32

#include "voCOMXBaseObject.h"

// wrapper for whatever critical section we have
class voCOMXThreadMutex
{
public:
    voCOMXThreadMutex(void);
    virtual ~voCOMXThreadMutex(void);

    void Lock();
    void Unlock();

#ifdef _WIN32
    CRITICAL_SECTION	m_CritSec;
#elif defined LINUX
  pthread_mutex_t		m_hMutex;
#endif // _WIN32

public:
	OMX_U32				m_nCurrentOwner;
	OMX_U32				m_nLockCount;
};

// locks a critical section, and unlocks it automatically
// when the lock goes out of scope
class voCOMXAutoLock
{
protected:
    voCOMXThreadMutex * m_pLock;

public:
    voCOMXAutoLock (voCOMXThreadMutex * plock)
    {
        m_pLock = plock;
        m_pLock->Lock();
    };

    ~voCOMXAutoLock ()
	{
        m_pLock->Unlock();
    };
};

#endif //__voCOMXThreadMutex_H__
