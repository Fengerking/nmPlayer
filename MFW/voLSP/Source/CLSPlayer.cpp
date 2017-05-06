/*
 *  CLSPlayer.cpp
 *  voCTS
 *
 *  Created by Lin Jun on 4/21/11.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#include "CLSPlayer.h"
#include "voString.h"

#include "vompCPlayer.h"
#include "vompCLivePlayer.h"

CLSPlayer::CLSPlayer()
:m_pVome(NULL)
,m_bStopped(false)
{

}

CLSPlayer::~CLSPlayer(void)
{
	Close();
}

VO_U32 CLSPlayer::Open(VO_PLAYER_OPEN_PARAM* pOpenParam)
{
	Close();
	
	m_pVome = new vompCPlayer(pOpenParam);
//	m_pVome = new vompCLivePlayer(pOpenParam);
	m_pVome->init();
	
	m_pVome->SetParam(VOMP_PID_DRAW_VIEW, pOpenParam->hDrawWnd);
	if(strlen(pOpenParam->szCfgFilePath) > 0)
		m_pVome->SetParam(VOMP_PID_PLAY_CONFIG_FILE, pOpenParam->szCfgFilePath);
	
	return VO_ERR_PLAYER_OK;
}

VO_U32 CLSPlayer::Close()
{
	if(m_pVome)
	{
		m_pVome->Stop();
		delete m_pVome;
		m_pVome = NULL;
	}
		
	return VO_ERR_PLAYER_OK;
}

VO_U32 CLSPlayer::SetDrawArea(VO_U16 nLeft, VO_U16 nTop, VO_U16 nRight, VO_U16 nBottom)
{
	if(m_pVome)
	{
		VOMP_RECT r = {nLeft, nTop, nRight, nBottom};
		return m_pVome->SetDrawRect(&r);
	}
		
	return VO_ERR_PLAYER_OK;
}

VO_U32 CLSPlayer::SetDataSource(VO_PTR pSource)
{
	if(m_pVome)
		return m_pVome->SetDataSource(pSource, 0, 3);
	return VO_ERR_PLAYER_OK;
}

VO_U32 CLSPlayer::SendBuffer(VO_BUFFER_INPUT_TYPE eInputType, VOMP_BUFFERTYPE * pBuffer)
{
	if(m_pVome)
		return m_pVome->SendBuffer(eInputType, pBuffer);
	return VO_ERR_PLAYER_OK;	
}

VO_U32 CLSPlayer::Run()
{
	if(m_pVome)
	{
		if(IsPaused())
			return m_pVome->Run();
		else if(IsStopped() || m_bStopped)
		{
			m_bStopped = false;
			return m_pVome->Run();
		}
		else if(IsPlaying())
			return VO_ERR_PLAYER_OK;
		
		m_bStopped = false;

		m_pVome->Run();
	}
	
	return VO_ERR_PLAYER_OK;
}

VO_U32 CLSPlayer::Pause()
{
	if(m_pVome)
	{
		if(IsPlaying())
			return m_pVome->Pause();
	}
	
	return VO_ERR_PLAYER_OK;
}

VO_U32 CLSPlayer::Stop()
{
	m_bStopped = true;
	if(m_pVome)
	{
		m_pVome->Stop();
	}
	
	return VO_ERR_PLAYER_OK;
}

VO_U32 CLSPlayer::GetStatus(VOMP_STATUS* pStatus)
{
	if(m_pVome)
	{
		return m_pVome->GetStatus(pStatus);
	}
	
	return VOMP_STATUS_MAX;
}

VO_U32 CLSPlayer::GetParam(VO_U32 nParamID, VO_PTR pParam)
{
	if(m_pVome)
	{
		return m_pVome->GetParam(nParamID, pParam);
	}
	
	return VO_ERR_PLAYER_OK;
}

VO_U32 CLSPlayer::SetParam(VO_U32 nParamID, VO_PTR pParam)
{
	if(m_pVome)
	{
		return m_pVome->SetParam(nParamID, pParam);
	}
	
	return VO_ERR_PLAYER_OK;
}

VO_BOOL	CLSPlayer::IsPlaying()
{
	if(m_pVome)
	{
		VOMP_STATUS status;
		m_pVome->GetStatus(&status);
		return status==VOMP_STATUS_RUNNING?VO_TRUE:VO_FALSE;
	}
	
	return VO_FALSE;
}

VO_BOOL	CLSPlayer::IsStopped()
{
	if(m_pVome)
	{
		VOMP_STATUS status;
		m_pVome->GetStatus(&status);
		return status==VOMP_STATUS_STOPPED?VO_TRUE:VO_FALSE;
	}
	
	return VO_FALSE;
}

VO_BOOL	CLSPlayer::IsPaused()
{
	if(m_pVome)
	{
		VOMP_STATUS status;
		m_pVome->GetStatus(&status);
		return status==VOMP_STATUS_PAUSED?VO_TRUE:VO_FALSE;
	}
	
	return VO_FALSE;
}

