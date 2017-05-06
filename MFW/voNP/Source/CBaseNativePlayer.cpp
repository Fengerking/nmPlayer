/*
 *  COMXALMediaPlayer.cpp
 *
 *  Created by Lin Jun on 10/09/12.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#include "CBaseNativePlayer.h"
#include "voOSFunc.h"
#include <string.h>
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CBaseNativePlayer::CBaseNativePlayer()
:m_pView(NULL)
,m_nCurrPos(0)
{
    memset(&m_NotifyEventInfo, 0, sizeof(VONP_LISTENERINFO));
}

CBaseNativePlayer::~CBaseNativePlayer(void)
{
	Uninit();
}

int CBaseNativePlayer::Init()
{
    return VONP_ERR_None;
}

int CBaseNativePlayer::Uninit()
{
    return VONP_ERR_None;
}

int CBaseNativePlayer::SetView(void* pView)
{
    m_pView = pView;
    return VONP_ERR_None;
}

int CBaseNativePlayer::Open(void* pSource, int nFlag)
{
    return VONP_ERR_None;
}

int CBaseNativePlayer::Close()
{
    return VONP_ERR_None;
}

int CBaseNativePlayer::Run()
{
    return VONP_ERR_None;
}

int CBaseNativePlayer::Pause()
{
    return VONP_ERR_None;
}

int CBaseNativePlayer::Stop()
{
    m_nCurrPos = 0;
    return VONP_ERR_None;
}

int CBaseNativePlayer::GetPos()
{
    return m_nCurrPos;
}

int CBaseNativePlayer::SetPos(int nPos)
{
    return VONP_ERR_None;
}

int CBaseNativePlayer::GetParam(int nParamID, void* pValue)
{
    return VONP_ERR_None;
}

int CBaseNativePlayer::SetParam(int nParamID, void* pValue)
{
    if(VONP_PID_LISTENER == nParamID)
	{
		VONP_LISTENERINFO* pNotifyEvent = (VONP_LISTENERINFO*)pValue;
        m_NotifyEventInfo.pUserData = pNotifyEvent->pUserData;
        m_NotifyEventInfo.pListener = pNotifyEvent->pListener;
        return VONP_ERR_None;
	}

    return VONP_ERR_None;
}

int CBaseNativePlayer::SendEvent(int nEventID, void* pParam1, void*pParam2)
{
	int nRet = VONP_ERR_Pointer;
	
	if(m_NotifyEventInfo.pListener && m_NotifyEventInfo.pUserData)
		nRet = m_NotifyEventInfo.pListener(m_NotifyEventInfo.pUserData, nEventID, pParam1, pParam2);
	
	return nRet;
}

int CBaseNativePlayer::GetBufferTime()
{
    return 0;
}

int CBaseNativePlayer::ReplaceURL(unsigned char* pszNewURL)
{
    return VONP_ERR_Implement;
}

int CBaseNativePlayer::EnableTrack(bool bAudio, bool bEnable)
{
    return VONP_ERR_Implement;
}