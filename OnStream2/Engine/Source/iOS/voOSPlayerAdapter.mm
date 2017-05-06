/*******************************************************************************
 *	File:		voOSPlayerAdapter.cpp
 *
 *	Contains:	voOSPlayerAdapter cpp file
 *
 *	Written by:	Jeff huang
 *
 *	Change History (most recent first):
 *	2012-10-04		Jeff			Create file
 *
 ******************************************************************************/

#include "voOSPlayerAdapter.h"
#include "voOSPlayer.h"

#ifdef _IOS
#include "voOSNPPlayer.h"
#include "voAudioRenderFactory.h"
#else
#import <Cocoa/Cocoa.h>
#endif

#include "voLog.h"
#include "voHalInfo.h"

//#define VO_AUTO_SWITCH

voOSPlayerAdapter::voOSPlayerAdapter()
:m_pSource(NULL)
,m_nFlag(0)
,m_pPlayer(NULL)
,m_pLogCB(NULL)
,m_nPlayerType(VOOSMP_VOME2_PLAYER)
{
    memset(m_szPathLib, 0, sizeof(m_szPathLib));
    memset(&m_cSendListener, 0, sizeof(m_cSendListener));
    
    m_cThisListener.pUserData = this;
	m_cThisListener.pListener = OnListener;
#ifdef _IOS
    voAudioRenderFactory::Instance();
#endif
}

voOSPlayerAdapter::~voOSPlayerAdapter()
{
    Uninit();
}

int voOSPlayerAdapter::Init(int nPlayerType)
{
    if (NULL != m_pPlayer)
	{
		Uninit();
	}
    
    m_nPlayerType = nPlayerType;
    
    if (m_nPlayerType == VOOSMP_VOME2_PLAYER)
    {
		m_pPlayer = new voOSPlayer(&m_cData);
    }
	else if (m_nPlayerType == VOOSMP_AV_PLAYER)
    {
#ifdef _IOS
		m_pPlayer = new voOSNPPlayer(&m_cData);
#else
        return VOOSMP_ERR_ParamID;
#endif
    }
    else {
        return VOOSMP_ERR_ParamID;
    }
    
	if (m_pPlayer == NULL)
	{
		return VOOSMP_ERR_OutMemory;
	}
    
    if (m_pLogCB) {
        m_pPlayer->SetParam(VOOSMP_PID_COMMON_LOGFUNC, m_pLogCB);
    }
    
    m_pPlayer->SetParam(VOOSMP_PID_PLAYER_PATH, m_szPathLib);
    
    int nRet = m_pPlayer->Init();
    
    if (VOOSMP_ERR_None != nRet) {
#ifdef VO_AUTO_SWITCH
        if (m_nPlayerType == VOOSMP_HLS_PLAYER) {
            return ChangeNativeToVoPlayer();
        }
        else {
            Uninit();
        }
#else
        return nRet;
#endif
    }
    else {
        m_pPlayer->SetParam(VOOSMP_PID_LISTENER, &m_cThisListener);
    }
    
    return nRet;
}

int voOSPlayerAdapter::Uninit()
{
	if (m_pPlayer == NULL)
	{
		return VOOSMP_ERR_None;
	}
    
    int nRet = m_pPlayer->Uninit();
    
    if (VOOSMP_ERR_None == nRet) {
        delete m_pPlayer;
        m_pPlayer = NULL;
    }
    else {
        VOLOGE("voOSPlayerAdapter::Uninit() failed:%d", nRet);
    }
    
    return nRet;
}

int voOSPlayerAdapter::SetDataSource (void * pSource, int nFlag)
{
    if (m_pPlayer == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}

    VO_CPU_Info info;
    memset(&info, 0, sizeof(info));
    get_cpu_info(&info);
    
#ifdef _IOS
    char szChar[20] = {0};
    voGetSysInfoStrByName("hw.machine", szChar, sizeof(szChar));
    
    VOLOGI("Model:%s systemVersion:%s cpu:%d", szChar, [[UIDevice currentDevice].systemVersion UTF8String], info.mMaxCpuSpeed);
#else
    char szChar[20] = {0};
    voGetSysInfoStrByName("hw.model", szChar, sizeof(szChar));
    
    SInt32 versionMajor=0, versionMinor=0, versionBugFix=0;
    Gestalt(gestaltSystemVersionMajor, &versionMajor);
    Gestalt(gestaltSystemVersionMinor, &versionMinor);
    Gestalt(gestaltSystemVersionBugFix, &versionBugFix);
    
    VOLOGI("Model:%s systemVersion:%ld.%ld.%ld cpu:%d", szChar, versionMajor, versionMinor, versionBugFix, info.mMaxCpuSpeed);
#endif
    
    m_pSource = pSource;
    m_nFlag = nFlag;
    
	int nRet = m_pPlayer->SetDataSource(m_pSource, m_nFlag);
    
    if (VOOSMP_ERR_None != nRet) {
#ifdef VO_AUTO_SWITCH
        if (m_nPlayerType == VOOSMP_HLS_PLAYER) {
            nRet = ChangeNativeToVoPlayer();
            
            if (VOOSMP_ERR_None == nRet) {
                nRet = m_pPlayer->SetDataSource(m_pSource, m_nFlag);
            }
        }
#else
        return nRet;
#endif
    }
    
    return nRet;
}

int voOSPlayerAdapter::Run (void)
{
    if (m_pPlayer == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
    
    int nRet = m_pPlayer->Run();
    
    if (VOOSMP_ERR_None != nRet) {
#ifdef VO_AUTO_SWITCH
        if (m_nPlayerType == VOOSMP_HLS_PLAYER) {
            nRet = ChangeNativeToVoPlayer();
            
            if (VOOSMP_ERR_None == nRet) {
                nRet = m_pPlayer->SetDataSource(m_pSource, m_nFlag);
            }
            
            if (VOOSMP_ERR_None == nRet) {
                nRet = m_pPlayer->Run();
            }
        }
#else
        return nRet;
#endif
    }
    
    return nRet;
}

int voOSPlayerAdapter::Pause (void)
{
    if (m_pPlayer == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
    
    return m_pPlayer->Pause();
}

int voOSPlayerAdapter::Stop (void)
{
    if (m_pPlayer == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
    
    return m_pPlayer->Stop();
}

int voOSPlayerAdapter::Close()
{
    if (m_pPlayer == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
    
    return m_pPlayer->Close();
}

int voOSPlayerAdapter::GetStatus(int* pStatus)
{
    if (m_pPlayer == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
    
    return m_pPlayer->GetStatus(pStatus);
}

int voOSPlayerAdapter::GetDuration(int* pDuration)
{
    if (m_pPlayer == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
    
    return m_pPlayer->GetDuration(pDuration);
}

int voOSPlayerAdapter::SetView(void* pView)
{
    if (m_pPlayer == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
    
    return m_pPlayer->SetView(pView);
}

int voOSPlayerAdapter::GetPos(int* pCurPos)
{
    if (m_pPlayer == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
    
    m_pPlayer->GetPos(pCurPos);
    return *pCurPos;
}

int voOSPlayerAdapter::SetPos(int nCurPos)
{
    if (m_pPlayer == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
    
    return m_pPlayer->SetPos(nCurPos);
}

int voOSPlayerAdapter::GetParam(int nParamID, void* pParam)
{
    if (nParamID == VOOSMP_PID_PLAYER_TYPE)
	{
		*((int*)pParam) = m_nPlayerType;
		return VOOSMP_ERR_None;
	}
    
    if (m_pPlayer == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
    
    return m_pPlayer->GetParam(nParamID, pParam);
}

int voOSPlayerAdapter::SetParam(int nParamID, void* pParam)
{
    if (m_pPlayer == NULL)
	{
        if (nParamID == VOOSMP_PID_COMMON_LOGFUNC)
        {
            m_pLogCB = pParam;
            return VOOSMP_ERR_None;
        }
        
        if ((nParamID == VOOSMP_PID_PLAYER_PATH) && (NULL != pParam)) {
            vostrcpy(m_szPathLib, (VO_TCHAR *)pParam);
            return VOOSMP_ERR_None;
        }
        
		return VOOSMP_ERR_Pointer;
	}
    
    if (VOOSMP_PID_LISTENER == nParamID) {
        VOOSMP_LISTENERINFO *pInfo = (VOOSMP_LISTENERINFO *)pParam;
        if (NULL == pInfo) {
            m_cSendListener.pUserData = NULL;
            m_cSendListener.pListener = NULL;
            return VOOSMP_ERR_None;
        }
        
        m_cSendListener.pUserData = pInfo->pUserData;
        m_cSendListener.pListener = pInfo->pListener;
        
        return VOOSMP_ERR_None;
    }
    
    return m_pPlayer->SetParam(nParamID, pParam);
}

int voOSPlayerAdapter::GetSubtileSample (voSubtitleInfo * pSample)
{
    if (m_pPlayer == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
    
    return m_pPlayer->GetSubtileSample(pSample);
}

int voOSPlayerAdapter::GetSubLangNum(int *pNum)
{
    if (m_pPlayer == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
    
    return m_pPlayer->GetSubLangNum(pNum);
}

int voOSPlayerAdapter::GetSubLangItem(int Index, VOOSMP_SUBTITLE_LANGUAGE** pSubLangItem)
{
    if (m_pPlayer == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
    
    return m_pPlayer->GetSubLangItem(Index, pSubLangItem);
}

int voOSPlayerAdapter::GetSubLangInfo(VOOSMP_SUBTITLE_LANGUAGE_INFO** pSubLangInfo)
{
    if (m_pPlayer == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
    
    return m_pPlayer->GetSubLangInfo(pSubLangInfo);
}

int voOSPlayerAdapter::SelectLanguage(int Index)
{
    if (m_pPlayer == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
    
    return m_pPlayer->SelectLanguage(Index);
}

int	voOSPlayerAdapter::GetSEISample (VOOSMP_SEI_INFO * pSample)
{
    if (m_pPlayer == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
    
    return m_pPlayer->GetSEISample(pSample);
}

int voOSPlayerAdapter::ChangeNativeToVoPlayer()
{
    Uninit();
    
    m_nPlayerType = VOOSMP_VOME2_PLAYER;
    
    m_cData.m_hWnd = NULL;
    m_pPlayer = new voOSPlayer(&m_cData);
    
    if (m_pPlayer == NULL)
	{
		return VOOSMP_ERR_OutMemory;
	}
    
    if (m_pLogCB) {
        m_pPlayer->SetParam(VOOSMP_PID_COMMON_LOGFUNC, m_pLogCB);
    }
    
    int nRet = m_pPlayer->Init();
    m_pPlayer->SetParam(VOOSMP_PID_LISTENER, &m_cThisListener);
    
    HandleEvent(VOOSMP_CB_HWDecoderStatus, 0, 0);
    
    return nRet;
}

int	voOSPlayerAdapter::OnListener (void * pUserData, int nID, void * pParam1, void * pParam2)
{
	voOSPlayerAdapter * pPlayer = (voOSPlayerAdapter *)pUserData;
    
    if (NULL == pPlayer) {
        return VOOSMP_ERR_Pointer;
    }
    
	return pPlayer->HandleEvent (nID, pParam1, pParam2);
}

int	voOSPlayerAdapter::HandleEvent (int nID, void * pParam1, void * pParam2)
{
    if ((NULL != m_cSendListener.pUserData) && (NULL != m_cSendListener.pListener)) {
        return m_cSendListener.pListener(m_cSendListener.pUserData, nID, pParam1, pParam2);
    }
    
    return VOOSMP_ERR_Pointer;
}