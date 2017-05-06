	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXThreadSemaphore.h

	Contains:	voCOMXThreadSemaphore header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-04		JBF			Create file

*******************************************************************************/
#ifndef __voCOMXThreadSemaphore_H__
#define __voCOMXThreadSemaphore_H__

#ifdef _WIN32
#include <windows.h>
#elif defined _LINUX
#include <pthread.h>
#endif // _WIN32

#include "OMX_Types.h"
#include "voCOMXBaseObject.h"

#define VOOMX_SEM_TIMEOUT	0X80000001
#define VOOMX_SEM_MAXTIME	0XFFFFFFFF

// wrapper for whatever semaphore we have
class voCOMXThreadSemaphore : public voCOMXBaseObject
{
public:
    voCOMXThreadSemaphore(void);
    virtual ~voCOMXThreadSemaphore(void);

    virtual OMX_U32		Down (OMX_U32 nWaitTime = VOOMX_SEM_MAXTIME);
    virtual OMX_U32		Up (void);
    virtual OMX_U32		Reset(void);
	virtual OMX_U32		Count (void);
	virtual OMX_BOOL	Waiting (void);

private:
#ifdef _WIN32
	CRITICAL_SECTION	m_CritSec;
	HANDLE				m_hEvent;
#elif defined _LINUX
	pthread_mutex_t		m_hMutex;
	pthread_cond_t		m_hCondition;
#endif // _WIN32
	OMX_S32				m_nSemCount;
	OMX_BOOL			m_bWaiting;
};

#endif //__voCOMXThreadSemaphore_H__
