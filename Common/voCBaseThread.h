	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCBaseThread.h

	Contains:	voCBaseThread header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-18		JBF			Create file

*******************************************************************************/
#ifndef __voCBaseThread_H__
#define __voCBaseThread_H__

#include "voYYDef_Common.h"
#include "voIndex.h"
#include "voThread.h"
#include "voCMutex.h"
#include "voCSemaphore.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

/**
*the status of the thread	
*/
typedef enum{
	VOTD_STATUS_INIT				= 0,	/*!<The status is running */
	VOTD_STATUS_RUN					= 1,	/*!<The status is running */
	VOTD_STATUS_PAUSE				= 2,	/*!<The status is paused */
	VOTD_STATUS_STOP				= 3		/*!<The status is stopped*/
}VO_THREAD_STATUS;

class voCBaseEventItem
{
public:
	voCBaseEventItem (int nType, int nMsg = 0, int nV1 = 0, int nV2 = 0)
	{
		m_nType = nType;
		m_nID = 0;
		m_nTime = -1;
		m_nMsg = nMsg;
		m_nV1 = nV1;
		m_nV2 = nV2;

		m_pNext = NULL;
	}
	virtual ~voCBaseEventItem (void)
	{

	}

public:
	virtual void		Fire (void) = 0;
	void				SetEventType (int nType) {m_nType = nType;}
	int					GetEventType (void) {return m_nType;}
	void				SetEventID (int nID) {m_nID = nID;}
	int					GetEventID (void) {return m_nID;}
	int					GetEventMsg (void) {return m_nMsg;}
	int					GetTime (void) {return m_nTime;}
	void				SetTime (int nTime) {m_nTime = nTime;}
	void				SetEventMsg (int nMsg, int nV1 = 0, int nV2 = 0) {m_nMsg = nMsg; m_nV1 = nV1; m_nV2 = nV2;}

protected:
	int					m_nType;
	int					m_nID;
	int					m_nTime;

	int					m_nMsg;
	int					m_nV1;
	int					m_nV2;


public:
	voCBaseEventItem *	m_pNext;
};

class voCBaseThread
{
public:
	voCBaseThread(const char * pThreadName);
	virtual ~voCBaseThread(void);

	virtual	int					Start (VOTDThreadCreate fThreadCreate = NULL);
	virtual int					Pause (void);
	virtual	int					Stop (void);

	virtual VO_THREAD_STATUS	GetStatus (void);

	// tag: 20101209
	virtual int			PostEventWithRealTime (voCBaseEventItem * pEvent, int nRealTime);
	virtual int			PostEventWithDelayTime (voCBaseEventItem * pEvent, int nDelayTime);

	virtual voCBaseEventItem *	CancelEvent (voCBaseEventItem * pEvent);
	virtual voCBaseEventItem *	CancelEventByID (int nID);
	virtual voCBaseEventItem *	CancelEventByType (int nType);
	virtual voCBaseEventItem *	CancelEventByMsg (int nMsg);

	virtual int					CancelAllEvent (void);
	
	virtual int					FreeAllEvent (void);

	virtual int					GetFullEventNum (int nType);
	virtual voCBaseEventItem *	GetEventByType (int nType);

protected:
	virtual voCBaseEventItem *	GetLastEvent (voCBaseEventItem * pFirst);
	virtual void 				AddFreeEvent (voCBaseEventItem * pEvent, voCBaseEventItem * pFree);

protected:
	voThreadHandle		m_hThread;
	VO_U32				m_nThreadID;

protected:
	voCMutex			m_mtEvent;
	voCMutex			m_mtStatus;
	voCSemaphore		m_csEvent;
	int					m_nNextEventID;
	char				m_szName[512];
	VO_THREAD_STATUS	m_nStatus;
	bool				m_bSleeping;

	voCBaseEventItem *	m_pFirstFull;
	voCBaseEventItem *	m_pFirstFree;

public:
	static	int		voBaseThreadProc (VO_PTR pParam);
	virtual int		voBaseThreadLoop (void);
};
    
#ifdef _VONAMESPACE
}
#endif

#endif // __voCBaseThread_H__
