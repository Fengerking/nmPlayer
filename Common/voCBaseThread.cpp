	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCBaseThread.cpp

	Contains:	voCBaseThread class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-18		JBF			Create file

*******************************************************************************/
#ifdef __SYMBIAN32__
#include <stdlib.h>
#elif defined _IOS || defined _MAC_OS
#include <stdlib.h>
#else
#include <malloc.h>
#endif // __SYMBIAN32__

#include <string.h>
#include <stdio.h>

#include "voOSFunc.h"

#include "voCBaseThread.h"

#define LOG_TAG "voCBaseThread"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voCBaseThread::voCBaseThread(const char * pThreadName)
	: m_hThread (NULL)
	, m_nThreadID (0)
	, m_nNextEventID (1)
	, m_nStatus (VOTD_STATUS_INIT)
	, m_bSleeping (true)
	, m_pFirstFull (NULL)
	, m_pFirstFree (NULL)
{
	if (pThreadName != NULL)
		strcpy (m_szName, pThreadName);
	else
		strcpy (m_szName, "");
}

voCBaseThread::~voCBaseThread(void)
{
	Stop ();

	FreeAllEvent ();
}

int voCBaseThread::Start (VOTDThreadCreate fThreadCreate)
{
	voCAutoLock lock (&m_mtStatus);

	m_nStatus = VOTD_STATUS_RUN;
	if (m_hThread != NULL)
		return 0;

	if (fThreadCreate != NULL)
		fThreadCreate (&m_hThread, (int *)&m_nThreadID, (void *)voBaseThreadProc, this);
	else
		voThreadCreate (&m_hThread, &m_nThreadID, (voThreadProc)voBaseThreadProc, this, 3);
	if (m_hThread == NULL)
		return -1;

	return 0;
}

int voCBaseThread::Pause (void)
{
	voCAutoLock lock (&m_mtStatus);

	m_nStatus = VOTD_STATUS_PAUSE;
	while (!m_bSleeping)
	{
		if (m_pFirstFull == NULL && m_csEvent.Waiting ())
			m_csEvent.Up ();
		voOS_Sleep (2);
	}

	return 0;
}

int voCBaseThread::Stop (void)
{
	voCAutoLock lock (&m_mtStatus);

	m_nStatus = VOTD_STATUS_STOP;
	if (m_csEvent.Waiting ())
		m_csEvent.Up ();

	int nTryTimes = 0;
	while (m_hThread != NULL)
	{
		if (m_csEvent.Waiting ())
			m_csEvent.Up ();
		voOS_Sleep (10);
		nTryTimes++;
		if (nTryTimes > 1000)
		{
			VOLOGI ("The thread %s could not safe exit", m_szName);
			break;
		}
	}

	CancelAllEvent ();

	VOLOGI ("The thread %s could safe exit", m_szName);

	return 0;
}

VO_THREAD_STATUS voCBaseThread::GetStatus (void)
{
	return m_nStatus;
}

int voCBaseThread::PostEventWithRealTime (voCBaseEventItem * pEvent, int nRealTime)
{
	voCAutoLock lock (&m_mtEvent);
	
	if (pEvent == NULL)
		return -1;

	pEvent->m_pNext = NULL;

	if (m_nStatus != VOTD_STATUS_RUN)
	{
		voCBaseEventItem * pLast = GetLastEvent (m_pFirstFree);
		if (pLast == NULL)
			m_pFirstFree = pEvent;
		else
			pLast->m_pNext = pEvent;

		return 0;
	}

	voCBaseEventItem * pLast = GetLastEvent (m_pFirstFull);
	if (pLast == NULL)
		m_pFirstFull = pEvent;
	else
		pLast->m_pNext = pEvent;

	pEvent->SetTime (nRealTime);
	pEvent->SetEventID (m_nNextEventID++);

	if (m_csEvent.Waiting ())
		m_csEvent.Up ();

	return 0;
}

int voCBaseThread::PostEventWithDelayTime (voCBaseEventItem * pEvent, int nDelayTime)
{
	int nTime = -1;
	if (nDelayTime > 0)
	{
		//ACW_UNUSED <nNow>: int nNow = voOS_GetSysTime ();
		nTime = voOS_GetSysTime () + nDelayTime;
	}

	return PostEventWithRealTime (pEvent, nTime);
}

voCBaseEventItem *	voCBaseThread::CancelEvent (voCBaseEventItem * pEvent)
{
	voCAutoLock lock (&m_mtEvent);
	
	if (pEvent == NULL)
		return NULL;

	voCBaseEventItem * pNext = m_pFirstFull;
	voCBaseEventItem * pPrev = NULL;
	while (pNext != NULL)
	{
		if (pNext == pEvent)
		{
			AddFreeEvent (pPrev, pNext);
			break;
		}

		pPrev = pNext;
		pNext = pNext->m_pNext;
	}

	return pNext;
}

voCBaseEventItem * voCBaseThread::CancelEventByID (int nID)
{
	voCAutoLock lock (&m_mtEvent);
	
	voCBaseEventItem * pNext = m_pFirstFull;
	voCBaseEventItem * pPrev = NULL;
	while (pNext != NULL)
	{
		if (pNext->GetEventID () == nID)
		{
			AddFreeEvent (pPrev, pNext);
			break;
		}

		pPrev = pNext;
		pNext = pNext->m_pNext;
	}

	return pNext;
}

voCBaseEventItem * voCBaseThread::CancelEventByType (int nType)
{
	voCAutoLock lock (&m_mtEvent);

	voCBaseEventItem * pNext = m_pFirstFull;
	voCBaseEventItem * pPrev = NULL;
	voCBaseEventItem * pTemp = NULL;
	while (pNext != NULL)
	{
		pTemp = pNext->m_pNext;
		if (pNext->GetEventType () == nType)
			AddFreeEvent (pPrev, pNext);
		else
			pPrev = pNext;

		pNext = pTemp;
	}

	return NULL;
}

voCBaseEventItem * voCBaseThread::CancelEventByMsg (int nMsg)
{
	voCAutoLock lock (&m_mtEvent);
	
	voCBaseEventItem * pNext = m_pFirstFull;
	voCBaseEventItem * pPrev = NULL;
	while (pNext != NULL)
	{
		if (pNext->GetEventMsg () == nMsg)
		{
			AddFreeEvent (pPrev, pNext);
			break;
		}

		pPrev = pNext;
		pNext = pNext->m_pNext;
	}

	return pNext;
}

int voCBaseThread::CancelAllEvent (void)
{
	voCAutoLock lock (&m_mtEvent);

	voCBaseEventItem * pNext = m_pFirstFull;
	voCBaseEventItem * pTemp = NULL;
	while (pNext != NULL)
	{
		pTemp = pNext->m_pNext;

		AddFreeEvent (NULL, pNext);

		pNext = pTemp;
	}

	m_pFirstFull = NULL;

	return 0;
}

int voCBaseThread::FreeAllEvent (void)
{
	voCAutoLock lock (&m_mtEvent);

	voCBaseEventItem * pNext = m_pFirstFull;
	voCBaseEventItem * pPrev = m_pFirstFull;
	while (pNext != NULL)
	{
		pPrev = pNext->m_pNext;

		delete pNext;

		pNext = pPrev;
	}

	pNext = m_pFirstFree;
	pPrev = m_pFirstFree;
	while (pNext != NULL)
	{
		pPrev = pNext->m_pNext;

		delete pNext;

		pNext = pPrev;
	}

	return 0;
}

int voCBaseThread::GetFullEventNum (int nType)
{
	voCAutoLock lock (&m_mtEvent);

	int nNum = 0;
	if (m_pFirstFull == NULL)
		return nNum;

	voCBaseEventItem * pNext = m_pFirstFull;
	while (pNext != NULL)
	{
		if (pNext->GetEventType () == nType)
			nNum++;
		pNext = pNext->m_pNext;
	}

	return nNum;
}

voCBaseEventItem * voCBaseThread::GetEventByType (int nType)
{
	voCAutoLock lock (&m_mtEvent);

	voCBaseEventItem * pNext = m_pFirstFree;
	voCBaseEventItem * pPrev = NULL;
	while (pNext != NULL)
	{
		if (pNext->GetEventType () == nType)
		{
			if (pPrev == NULL)
				m_pFirstFree = pNext->m_pNext;
			else
				pPrev->m_pNext = pNext->m_pNext;

			break;
		}

		pPrev = pNext;
		pNext = pNext->m_pNext;
	}

	if (pNext != NULL)
	{
		pNext->m_pNext = NULL;
	}

	return pNext;
}

voCBaseEventItem *	voCBaseThread::GetLastEvent (voCBaseEventItem * pFirst)
{
	voCAutoLock lock (&m_mtEvent);

	if (pFirst == NULL)
		return NULL;

	voCBaseEventItem * pLast = pFirst;
	while (pLast->m_pNext != NULL)
		pLast = pLast->m_pNext;

	return pLast;
}

void voCBaseThread::AddFreeEvent (voCBaseEventItem * pPrev, voCBaseEventItem * pFree)
{
	voCAutoLock lock (&m_mtEvent);

	if (pFree == NULL)
		return;

	if (pPrev == NULL)
		m_pFirstFull = pFree->m_pNext;
	else
		pPrev->m_pNext = pFree->m_pNext;

	pFree->m_pNext = NULL;
	voCBaseEventItem * pLast = GetLastEvent (m_pFirstFree);
	if (pLast == NULL)
		m_pFirstFree = pFree;
	else
		pLast->m_pNext = pFree;
}

int voCBaseThread::voBaseThreadProc (VO_PTR pParam)
{
	voCBaseThread * pThread = (voCBaseThread *)pParam;

	return pThread->voBaseThreadLoop ();
}
	
int voCBaseThread::voBaseThreadLoop (void)
{
	voThreadSetName (m_nThreadID, m_szName);

	voCBaseEventItem *	pTask = NULL;
	voCBaseEventItem *	pTemp = NULL;
	//ACW_UNUSED <pLast>: voCBaseEventItem *	pLast = NULL;
	
	int					nTime = 0;
	
	while (true)
	{
//		voCAutoLock lock (&m_mtStatus);
		if (m_nStatus == VOTD_STATUS_PAUSE)
		{
			m_bSleeping = true;
			voOS_Sleep (10);
			continue;
		}
		else if (m_nStatus != VOTD_STATUS_RUN)
		{
			break;
		}

		m_bSleeping = false;
		m_mtEvent.Lock ();

		nTime = 0X7FFFFFFF;
		pTemp = m_pFirstFull;
		pTask = NULL;
		while (pTemp != NULL)
		{
			if (pTemp->GetTime () <= 0)
			{
				pTask = pTemp;
				break;
			}
			else if (pTemp->GetTime () < nTime)
			{
				nTime = pTemp->GetTime ();
				pTask = pTemp;
			}

			pTemp = pTemp->m_pNext;
		}

		if (pTask != NULL && ((pTask->GetTime () <= 0) || (((VO_U32)pTask->GetTime ()) <= voOS_GetSysTime ())))
		{
			CancelEvent (pTask);
			m_mtEvent.Unlock ();
			pTask->Fire ();
		}
		else
		{
			m_mtEvent.Unlock ();
			voOS_Sleep (10);
		}

		if (m_pFirstFull == NULL)
		{
			m_csEvent.Reset ();
			m_csEvent.Down ();
		}
	}

	m_hThread = NULL;
	return 0;
}
