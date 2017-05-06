	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXThreadSemaphore.cpp

	Contains:	voCOMXThreadSemaphore class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-02-16		JBF			Create file

*******************************************************************************/
#include "voCOMXThreadSemaphore.h"
#ifdef _WIN32
#pragma warning(disable: 4996) //Use strxxx_s to replace strxxx
#endif
voCOMXThreadSemaphore::voCOMXThreadSemaphore()
	: m_nSemCount (0)
	, m_bWaiting (OMX_FALSE)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);

#ifdef _WIN32
	InitializeCriticalSection(&m_CritSec);
	m_hEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
#elif defined _LINUX
	pthread_mutex_init (&m_hMutex, NULL);
	pthread_cond_init (&m_hCondition, NULL);
#endif // _WIN32

}

voCOMXThreadSemaphore::~voCOMXThreadSemaphore()
{
#ifdef _WIN32
	CloseHandle (m_hEvent);
	DeleteCriticalSection(&m_CritSec);
#elif defined _LINUX
	pthread_cond_destroy (&m_hCondition);
	pthread_mutex_destroy (&m_hMutex);
#endif // _WIN32
}

OMX_U32 voCOMXThreadSemaphore::Down (OMX_U32 nWaitTime /* = VOOMX_SEM_MAXTIME */)
{
	m_bWaiting = OMX_TRUE;

#ifdef _WIN32
	if (m_nSemCount == 0)
	{
		if (WAIT_TIMEOUT == WaitForSingleObject (m_hEvent, nWaitTime))
			return VOOMX_SEM_TIMEOUT;
	}

	EnterCriticalSection(&m_CritSec);

	ResetEvent (m_hEvent);
	m_nSemCount--;

	LeaveCriticalSection(&m_CritSec);
#elif defined _LINUX
	pthread_mutex_lock (&m_hMutex);
	while (m_nSemCount == 0)
	{
		pthread_cond_wait (&m_hCondition, &m_hMutex);
	}
	m_nSemCount--;
	pthread_mutex_unlock (&m_hMutex);
#endif // _WIN32

	m_bWaiting = OMX_FALSE;

	return 0;
}

OMX_U32 voCOMXThreadSemaphore::Up (void)
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
#endif // _WIN32

	return 0;
}

OMX_U32 voCOMXThreadSemaphore::Reset(void)
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
#endif // _WIN32

	return 0;
}

OMX_U32 voCOMXThreadSemaphore::Count (void)
{
	return m_nSemCount;
}

OMX_BOOL voCOMXThreadSemaphore::Waiting (void)
{
	return (OMX_BOOL)(m_bWaiting&&m_nSemCount<=0);
}
