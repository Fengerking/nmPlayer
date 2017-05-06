
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

#include "vome2CBaseRender.h"
#include "voThread.h"

#define LOG_TAG "vome2CBaseRender"
#include "voLog.h"

#include "voOSFunc.h"


vome2CBaseRender::vome2CBaseRender(vompCEngine * pEngine)
	: m_pEngine (pEngine)
	, m_bThreadStatus (VOME2_THREAD_INIT)
	, m_bThreadExit (true)
{
	strcpy (m_szThreadName, "vome2Render");
}

vome2CBaseRender::~vome2CBaseRender()
{
}

int	vome2CBaseRender::Start (void)
{
	VOLOGI("%s START", m_szThreadName);
	
	m_bThreadStatus = VOME2_THREAD_RUNNING;
	if (!m_bThreadExit)
		return 0;

	m_bThreadExit = false;
	VOLOGI("%s before create thread", m_szThreadName);
	
	voThreadHandle hHandle = NULL;
	VO_U32 nID = 0;
	voThreadCreate(&hHandle, &nID, (voThreadProc)vome2RenderThreadProc, (void*)this, 0);
	//AndroidRuntime::createJavaThread(m_szThreadName, (void(*)(void*))vome2RenderThreadProc, (void*)this); 

	return 0;
}

int vome2CBaseRender::Pause (void)
{
	m_bThreadStatus = VOME2_THREAD_PAUSED;

	return 0;
}

int vome2CBaseRender::Stop (void)
{
	m_bThreadStatus = VOME2_THREAD_STOPPED;

	while (!m_bThreadExit)
	{
		voOS_Sleep(10);
	}

	return 0;
}

int vome2CBaseRender::Flush (void)
{
	return 0;
}

int vome2CBaseRender::SetPos (int nPos)
{
	return 0;
}

int vome2CBaseRender::GetParam (int nID, void * pValue)
{
	return 0;
}

int vome2CBaseRender::SetParam (int nID, void * pValue)
{
	return 0;
}

int vome2CBaseRender::vome2RenderThreadProc (void * pParam)
{
	vome2CBaseRender * pRender = (vome2CBaseRender *)pParam;

	VOLOGI ("Start thread %s", pRender->m_szThreadName);

	pRender->vome2RenderThreadStart ();

	while (pRender->m_bThreadStatus >= VOME2_THREAD_RUNNING)
	{
		if (pRender->m_bThreadStatus == VOME2_THREAD_PAUSED)
		{
			voOS_Sleep(1);
			continue;
		}

		pRender->vome2RenderThreadLoop ();
	}

	pRender->vome2RenderThreadExit ();

	pRender->m_bThreadExit = true;
	VOLOGI ("Exit thread %s", pRender->m_szThreadName);

	return 0;
}

int vome2CBaseRender::vome2RenderThreadLoop (void)
{
	voOS_Sleep(100);
//	VOLOGW ("There is nother in the thread.");
	return 0;
}

void vome2CBaseRender::vome2RenderThreadStart (void)
{
}

void vome2CBaseRender::vome2RenderThreadExit (void)
{
}
