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
#include "cmnMemory.h"
#include "CBaseAudioRender.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
    
CBaseAudioRender::CBaseAudioRender(VO_PTR hInst, VO_MEM_OPERATOR * pMemOP)
	: m_hInst (hInst)
	, m_pMemOP (pMemOP)
	, m_fCallBack (NULL)
	, m_pUserData (NULL)
	, m_pLibOP (NULL)
	, m_hCheck (NULL)
{
	if (m_pMemOP == NULL)
	{
		cmnMemFillPointer (VO_INDEX_SNK_AUDIO);
		m_pMemOP = &g_memOP;
	}
}

CBaseAudioRender::~CBaseAudioRender ()
{
}

VO_U32 CBaseAudioRender::SetInputFormat(int nFormat)
{
	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_AUDIO;
}

VO_U32 CBaseAudioRender::SetFormat (VO_AUDIO_FORMAT * pFormat)
{
	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_AUDIO;
}

VO_U32 CBaseAudioRender::Start (void)
{
	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_AUDIO;
}

VO_U32 CBaseAudioRender::Pause (void)
{
	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_AUDIO;
}

VO_U32 CBaseAudioRender::Stop (void)
{
	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_AUDIO;
}

VO_U32 CBaseAudioRender::Render (VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart, VO_BOOL bWait)
{
	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_AUDIO;
}

VO_U32 CBaseAudioRender::Flush (void)
{
	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_AUDIO;
}

VO_U32 CBaseAudioRender::GetPlayingTime (VO_S64	* pPlayingTime)
{
	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_AUDIO;
}

VO_U32 CBaseAudioRender::GetBufferTime (VO_S32	* pBufferTime)
{
	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_AUDIO;
}

VO_U32 CBaseAudioRender::SetVolume(float leftVolume, float rightVolume)
{
	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_AUDIO;
}

VO_U32 CBaseAudioRender::AudioInterruption(VO_U32 inInterruptionState)
{
	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_AUDIO;
}

VO_U32 CBaseAudioRender::SetCallBack (VOAUDIOCALLBACKPROC pCallBack, VO_PTR pUserData)
{
	m_fCallBack = pCallBack;
	m_pUserData = pUserData;

	return VO_ERR_NONE;
}

VO_U32 CBaseAudioRender::SetParam (VO_U32 nID, VO_PTR pValue)
{
	if (nID == VO_PID_COMMON_LIBOP)
	{
		m_pLibOP = (VO_LIB_OPERATOR *)pValue;

		return VO_ERR_NONE;
	}
	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_AUDIO;
}

VO_U32 CBaseAudioRender::GetParam (VO_U32 nID, VO_PTR pValue)
{
	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_AUDIO;
}

