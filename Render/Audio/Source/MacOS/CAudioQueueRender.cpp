/*
 *  CAudioQueueRender.cpp
 *  vompEngn
 *
 *  Created by Lin Jun on 11/18/10.
 *  Copyright 2010 VisualOn. All rights reserved.
 *
 */

#include "CAudioQueueRender.h"
#include "CWaveOutAudio.h"
#include "voOSFunc.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CAudioQueueRender::CAudioQueueRender (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP)
:CBaseAudioRender(hInst, pMemOP)
,m_pRender(NULL)
{
	m_pRender = new CWaveOutAudio;
}

CAudioQueueRender::~CAudioQueueRender (void)
{
	if (m_pRender)
	{
		m_pRender->CloseDevice();
		delete m_pRender;
		m_pRender = NULL;
	}
	
}

/*
VO_U32 CAudioQueueRender::GetBufferTime (VO_S32	* pBufferTime)
{
	if (m_pRender)
	{
		*pBuffTime = m_pRender->GetBufferTime();
	}
	return VO_ERR_NONE;
}
 */

VO_U32 CAudioQueueRender::SetFormat (VO_AUDIO_FORMAT * pFormat)
{
	if(!m_pRender)
		return VO_ERR_WRONG_STATUS;
	
	m_pRender->SetAudioFormat(pFormat->SampleRate, pFormat->Channels, pFormat->SampleBits);
	m_pRender->InitDevice();

	return VO_ERR_NONE;
}

VO_U32 CAudioQueueRender::Start (void)
{
	if(!m_pRender)
		return VO_ERR_WRONG_STATUS;
	m_pRender->Start();
	
	return VO_ERR_NONE;
}

VO_U32 CAudioQueueRender::Pause (void)
{
	if(!m_pRender)
		return VO_ERR_WRONG_STATUS;
	m_pRender->Pause();
	
	return VO_ERR_NONE;
}


VO_U32 CAudioQueueRender::Stop (void)
{
	if(!m_pRender)
		return VO_ERR_WRONG_STATUS;
	m_pRender->Stop();
	
	return VO_ERR_NONE;
}

VO_U32 CAudioQueueRender::Render (VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart, VO_BOOL bWait)
{
	if(!m_pRender)
		return VO_ERR_WRONG_STATUS;
	m_pRender->Render(pBuffer, nSize, nStart, 0, bWait);
	
	return VO_ERR_NONE;
}

VO_U32 CAudioQueueRender::Flush (void)
{
	if(!m_pRender)
		return VO_ERR_WRONG_STATUS;

	m_pRender->Flush();
	
	return VO_ERR_NONE;
}

VO_U32 CAudioQueueRender::GetPlayingTime (VO_S64	* pPlayingTime)
{
	if(!m_pRender)
		return VO_ERR_WRONG_STATUS;
	
	int time = m_pRender->GetPlayingTime();
	*pPlayingTime = time;

	return VO_ERR_NONE;
}

VO_U32 CAudioQueueRender::GetBufferTime (VO_S32	* pBufferTime)
{
	if(!m_pRender)
		return VO_ERR_WRONG_STATUS;
	
	int time = m_pRender->GetBufferTime();
	*pBufferTime = time;
	
	return VO_ERR_NONE;
}


