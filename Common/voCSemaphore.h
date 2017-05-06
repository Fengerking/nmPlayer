	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCSemaphore.h

	Contains:	voCSemaphore header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-04		JBF			Create file

*******************************************************************************/
#ifndef __voCSemaphore_H__
#define __voCSemaphore_H__

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

#define VOVO_SEM_TIMEOUT	0X80000001
#define VOVO_SEM_MAXTIME	0XFFFFFFFF
#define VOVO_SEM_OK			0X00000000

// wrapper for whatever critical section we have
class voCSemaphore : public CvoBaseObject
{
public:
    voCSemaphore(void);
    virtual ~voCSemaphore(void);

    virtual VO_U32		Down (VO_U32 nWaitTime = VOVO_SEM_MAXTIME);
    virtual VO_U32		Up (void);
    virtual VO_U32		Reset(void);
    virtual VO_U32		Wait (VO_U32 nWaitTime = VOVO_SEM_MAXTIME);
    virtual VO_U32		Signal (void);
	virtual VO_U32		Count (void);
	virtual VO_BOOL		Waiting (void);

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
	VO_U32				m_nSemCount;
	VO_BOOL				m_bWaiting;
};
    
#ifdef _VONAMESPACE
}
#endif

#endif //__voCSemaphore_H__