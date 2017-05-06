	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		vompCLivePlayer.cpp

	Contains:	vompCLivePlayer class file

	Written by:	Jeff Huang

	Change History (most recent first):
	2011-08-30		Jeff		Create file

*******************************************************************************/

#include "vompCLivePlayer.h"
#include "voOSFunc.h"

vompCLivePlayer::vompCLivePlayer(VO_PLAYER_OPEN_PARAM* pParam)
	: vompCPlayer(pParam)
{	
}

vompCLivePlayer::~vompCLivePlayer ()
{
}

int vompCLivePlayer::SendBuffer (VO_BUFFER_INPUT_TYPE eInputType, VOMP_BUFFERTYPE * pBuffer)
{
	int nRet = VOMP_ERR_Retry;
	
	if(m_bStopPlay)
	{
		return nRet;
	}
	
	voCAutoLock lock(&m_mutexSendBuf);
	
	int retrynums = 10;
	
	while ((nRet != VOMP_ERR_None ) && (0 < retrynums))
	{
		nRet = vompCEngine::SendBuffer(eInputType, pBuffer);
		
		if (nRet != VOMP_ERR_None)
		{
			voOS_Sleep (500);
			retrynums--;
		}
	}
	
	return nRet;
}

int vompCLivePlayer::initVomp() {
	m_bUseLiveSourceModule = false;
	
	if (m_fInit == NULL)
		return -1;
	
	if (NULL != m_hPlay && NULL != m_fUninit) {
		m_fUninit(m_hPlay);
		m_hPlay = NULL;
	}
	// VOMP_PLAYER_LIVE VOMP_PLAYER_MEDIA
	m_fInit (&m_hPlay, VOMP_PLAYER_LIVE, OnListener, this);
	if (m_hPlay == NULL)
		return -1;
	
	return 1;
}

