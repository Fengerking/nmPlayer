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
#ifndef __vo__CMutex_H__
#define __vo__CMutex_H__

#ifdef _WIN32
#include <windows.h>
#elif defined LINUX
#include <pthread.h>
#elif defined(_IOS) || defined(_MAC_OS) 
#include <pthread.h>
#endif // _WIN32

class osCMutex
{
public:
    osCMutex(void);
    virtual ~osCMutex(void);

    void Lock();
    void Unlock();

#ifdef _WIN32
    CRITICAL_SECTION	m_CritSec;
#elif defined _LINUX_ANDROID
	pthread_mutex_t		m_hMutex;
#elif defined(_IOS) || defined(_MAC_OS)
	pthread_cond_t		m_hCondition;
	pthread_mutex_t		m_hMutex;
#endif // _WIN32
};

class osCAutoLock
{
protected:
    osCMutex * m_pLock;

public:
    osCAutoLock(osCMutex * plock)
    {
        m_pLock = plock;
        m_pLock->Lock();
    };

    ~osCAutoLock()
	{
        m_pLock->Unlock();
    };
};

#endif //__vo__CMutex_H__
