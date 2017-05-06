
/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
	File:		vome2CBaseRender.cpp

	Contains:	vome2CBaseRender class file

	Written by:	Tom Yu Wei 

	Change History (most recent first):
	2010-09-25		Bang			Create file

*******************************************************************************/

#include "voBaseRender.h"
#include "voThread.h"

#include "voLog.h"
#include "voOSFunc.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voBaseRender::voBaseRender(void* pUserData, RenderQueryCallback pQuery)
:m_pQuery (pQuery)
,m_pUserData(pUserData)
,m_bThreadStatus (VO_THREAD_INIT)
,m_bThreadExit (true)
,m_bPauseDone(false)
#ifdef _VOLOG_INFO
,m_nSample(0)
,m_nLastTime(0)
#endif
{
	strcpy (m_szThreadName, "voRender");
}

voBaseRender::~voBaseRender()
{
}

int	voBaseRender::Start (void)
{
    voCAutoLock cLock(&m_cMutex);
    
	VOLOGI("%s START", m_szThreadName);
	
	m_bThreadStatus = VO_THREAD_RUNNING;
	if (!m_bThreadExit)
		return 0;

#ifdef _VOLOG_INFO
    m_nSample = 0;
    m_nLastTime = 0;
#endif
    
	m_bThreadExit = false;
	VOLOGI("%s before create thread", m_szThreadName);
	
	voThreadHandle hHandle = NULL;
	VO_U32 nID = 0;
	voThreadCreate(&hHandle, &nID, (voThreadProc)RenderThreadProc, (void*)this, 0);

	return 0;
}

int voBaseRender::Pause ()
{
    voCAutoLock cLock(&m_cMutex);
	return Pause(true);
}

int voBaseRender::Stop ()
{
    voCAutoLock cLock(&m_cMutex);
	return Stop(true);
}

int voBaseRender::Pause (bool wait)
{
    voCAutoLock cLock(&m_cMutex);
    
    if (m_bThreadStatus == VO_THREAD_RUNNING) {
        
        m_bThreadStatus = VO_THREAD_PAUSED;
        m_bPauseDone = false;
        
        while (!m_bPauseDone && (m_bThreadStatus == VO_THREAD_RUNNING) && wait) {
            voOS_Sleep(2);
        }
    }
    
	return 0;
}

int voBaseRender::Stop (bool wait)
{
    voCAutoLock cLock(&m_cMutex);
	m_bThreadStatus = VO_THREAD_STOPPED;

	while (!m_bThreadExit && wait)
	{
		voOS_Sleep(1);
	}

	return 0;
}

int voBaseRender::RenderThreadProc (void * pParam)
{
	voBaseRender * pRender = (voBaseRender *)pParam;

	VOLOGI ("Start thread %s", pRender->m_szThreadName);

	pRender->RenderThreadStart ();

	while (pRender->m_bThreadStatus >= VO_THREAD_RUNNING)
	{
		if (pRender->m_bThreadStatus == VO_THREAD_PAUSED)
		{
            pRender->m_bPauseDone = true;
			voOS_Sleep(5);
			continue;
		}

		int nRet = pRender->RenderThreadLoop ();
        
#ifdef _VOLOG_INFO
        if (0 == pRender->m_nLastTime) {
            pRender->m_nLastTime = voOS_GetSysTime();
        }
        
        if (0 == nRet) {
            ++pRender->m_nSample;
        }
        
        if ((5 * 1000) < (voOS_GetSysTime() - pRender->m_nLastTime)) {
            VOLOGI("The:%s Loop success count:%d, current ret:%d", pRender->m_szThreadName, pRender->m_nSample, nRet);
            
            pRender->m_nSample = 0;
            pRender->m_nLastTime = voOS_GetSysTime();
        }
#endif
	}

	pRender->RenderThreadExit ();

	pRender->m_bThreadExit = true;
	VOLOGI ("Exit thread %s", pRender->m_szThreadName);

	return 0;
}

int voBaseRender::RenderThreadLoop (void)
{
	voOS_Sleep(100);
//	VOLOGW ("There is nother in the thread.");
	return 0;
}

void voBaseRender::RenderThreadStart (void)
{
}

void voBaseRender::RenderThreadExit (void)
{
}
