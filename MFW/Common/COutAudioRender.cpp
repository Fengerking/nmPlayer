	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.cpp

	Contains:	CVideoWindow class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include "COutAudioRender.h"

#include "voOSFunc.h"
#include "voLog.h"

COutAudioRender::COutAudioRender (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP)
	: CAudioRender (hInst, pMemOP)
	, m_nMediaTime (0)
	, m_nClockTime (0)
	, m_bWallClock (VO_FALSE)
	, m_fCallBack (NULL)
	, m_pUserData (NULL)
{
	m_fmtAudio.SampleRate = 44100;
	m_fmtAudio.Channels = 2;
	m_fmtAudio.SampleBits = 16;
}

COutAudioRender::~COutAudioRender ()
{
	Uninit ();
}

VO_U32 COutAudioRender::Init (VO_AUDIO_FORMAT * pFormat)
{
	if (pFormat != NULL)
	{
		m_fmtAudio.SampleRate = pFormat->SampleRate;
		m_fmtAudio.Channels = pFormat->Channels;
		m_fmtAudio.SampleBits = pFormat->SampleBits;
	}

	return VO_ERR_NONE;
}

VO_U32 COutAudioRender::Uninit (void)
{
	return 0;
}

VO_U32 COutAudioRender::SetFormat(VO_AUDIO_FORMAT * pFormat)
{
	if (pFormat != NULL)
	{
		m_fmtAudio.SampleRate = pFormat->SampleRate;
		m_fmtAudio.Channels = pFormat->Channels;
		m_fmtAudio.SampleBits = pFormat->SampleBits;
	}

	return VO_ERR_NONE;
}

VO_U32 COutAudioRender::Start(void)
{
	m_nClockTime = 0;

	return VO_ERR_NONE;
}

VO_U32 COutAudioRender::Pause(void)
{
	m_bWallClock = VO_FALSE;
	m_nMediaTime = 0;

	return VO_ERR_NONE;
}

VO_U32 COutAudioRender::Stop(void)
{
	m_nMediaTime = 0;
	m_nClockTime = 0;

	return VO_ERR_NONE;
}

VO_U32 COutAudioRender::Render(VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart)
{
	if (m_fCallBack != NULL)
	{
		m_Buffer.Buffer = pBuffer;
		m_Buffer.Length = nSize;

		m_fCallBack (m_pUserData, &m_Buffer, &m_fmtAudio, (VO_S32)nStart);
	}

	voCAutoLock lock (&m_mutex);
	m_nMediaTime = nStart;
	m_nClockTime = voOS_GetSysTime ();

	if (!m_bWallClock)
	{
		m_bWallClock = VO_TRUE;
	}

	return VO_ERR_NONE;
}

VO_U32 COutAudioRender::Flush(void)
{
	m_nMediaTime = 0;
	m_nClockTime = 0;

	return VO_ERR_NONE;
}

VO_S64 COutAudioRender::GetPlayingTime(void)
{
	if (m_nMediaTime == 0)
		return 0;

	if (!m_bWallClock)
		return m_nMediaTime;

	if (m_nClockTime == 0)
		m_nClockTime = voOS_GetSysTime ();

	VO_S64 nPlayingTime = m_nMediaTime + (voOS_GetSysTime () - m_nClockTime);

	return nPlayingTime;
}

VO_S32 COutAudioRender::GetBufferTime(void)
{
	return 0;
}

VO_U32 COutAudioRender::SetCallBack(VOAUDIOCALLBACKPROC pCallBack, VO_PTR pUserData)
{
	m_fCallBack = pCallBack;
	m_pUserData = pUserData;

	return VO_ERR_NONE;
}

VO_U32 COutAudioRender::SetParam(VO_U32 nID, VO_PTR pValue)
{
	return VO_ERR_NONE;
}

VO_U32 COutAudioRender::GetParam(VO_U32 nID, VO_PTR pValue)
{
	return VO_ERR_NONE;
}

VO_U32 COutAudioRender::LoadLib (VO_HANDLE hInst)
{
	return 1;
}
