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
#include "CAudioQueueService.h"
#include "voOSFunc.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CAudioQueueRender::CAudioQueueRender (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP)
:CBaseAudioRender(hInst, pMemOP)
,m_pRender(NULL)
{
#ifndef USE_HW_AUDIO_DEC
	m_pRender = new CWaveOutAudio;
#endif
	
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

VO_U32 CAudioQueueRender::SetInputFormat(int nFormat)
{
	if(!m_pRender)
		return VO_ERR_WRONG_STATUS;

	m_pRender->SetInputFormat(nFormat);
	
	return VO_ERR_NONE;
}

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
#ifdef USE_HW_AUDIO_DEC
	//voOS_Sleep(50);
	return VO_ERR_NONE;
#endif
	
#if 0
	static FILE* hPCM = NULL;
	if(!hPCM)
	{
		char szTmp[1024];
		voOS_GetAppFolder(szTmp, 1024);
		strcat(szTmp, "cisco.pcm");
		hPCM = fopen(szTmp, "wb");
	}
	
	if(hPCM)
	{
		fwrite(pBuffer, 1, nSize, hPCM);
		fflush(hPCM);
	}
#endif
	
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


