	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCMutex.h

	Contains:	voCMutex header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-04		JBF			Create file

*******************************************************************************/
#ifndef __voCMutex_H__
#define __voCMutex_H__

#ifdef _WIN32
#include <windows.h>
#elif defined LINUX
#include <pthread.h>
#elif defined __SYMBIAN32__
#include <e32std.h>
#elif defined(_IOS) || defined(_MAC_OS) 
#include <pthread.h>
#endif // _WIN32

#include "voYYDef_Common.h"

#include "voType.h"
#include "CvoBaseObject.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

// wrapper for whatever critical section we have
class voCMutex : public CvoBaseObject
{
public:
    voCMutex(void);
    virtual ~voCMutex(void);

    void Lock();
    void Unlock();

#ifdef _WIN32
    CRITICAL_SECTION	m_CritSec;
#elif defined LINUX
	pthread_mutex_t		m_hMutex;
#elif defined __SYMBIAN32__
	RMutex				m_cMutex;
#elif defined(_IOS) || defined(_MAC_OS)
	pthread_cond_t		m_hCondition;
	pthread_mutex_t		m_hMutex;
#endif // _WIN32

public:
	VO_U32				m_nCurrentOwner;
	VO_U32				m_nLockCount;
};

// locks a critical section, and unlocks it automatically
// when the lock goes out of scope
class voCAutoLock
{
protected:
    voCMutex * m_pLock;

public:
    voCAutoLock(voCMutex * plock)
    {
        m_pLock = plock;
        if (m_pLock) {
            m_pLock->Lock();
        }
    };

    ~voCAutoLock()
	{
        if (m_pLock) {
            m_pLock->Unlock();
        }
    };
};
    
#ifdef _VONAMESPACE
}
#endif

#endif //__voCMutex_H__
