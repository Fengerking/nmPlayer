/*******************************************************************************
 *	File:		voOSPlayer.cpp
 *	
 *	Contains:	voOSPlayer class file
 *	
 *	Written by:	Jeff huang
 *	
 *	Change History (most recent first):
 *	2012-03-20		Jeff			Create file
 *	
 ******************************************************************************/

#include "voOSPlayer.h"

#include "voOSFunc.h"
#include "voLog.h"
#include "voIVCommon.h"

#include "COSVomePlayer.h"
#include "voOnStreamType.h"

#if defined(_IOS) || defined(_MAC_OS)
#include "CAudioRenderManager.h"
#include "CVideoRenderManager.h"
#include "voCCManager.h"
#include "voSystemStatus.h"
#endif

#ifdef _IOS
#include "voLoadLibControl.h"
#endif

voOSPlayer::voOSPlayer(voOSPlayerData *pData)
:COSVomePlayer()
,m_pData(pData)
,m_pAudioRender (NULL)
,m_pVideoRender (NULL)
,m_pCCRender(nil)
,m_nPlayMode (VOMP_PULL_MODE)
#if defined(_USE_OLD_AUDIO_RENDER)
,m_bEnableAudioRefClock(VO_FALSE)
#endif
,m_bPaused(false)
{
    m_sLibFunc.pUserData = this;
	m_sLibFunc.LoadLib = vomtLoadLib;
	m_sLibFunc.GetAddress = vomtGetAddress;
	m_sLibFunc.FreeLib = vomtFreeLib;
    
    m_pLibFunc = &m_sLibFunc;
}

voOSPlayer::~voOSPlayer ()
{
	Uninit();
}

int voOSPlayer::Init()
{
    if (NULL == m_pData) {
        return VOOSMP_ERR_Pointer;
    }
    
    int nRC = COSVomePlayer::Init();
	
	if(nRC != VOOSMP_ERR_None) 
		return nRC;
    
	VO_TCHAR szID[256];
	memset(szID, 0, sizeof(szID));
	voOS_GetApplicationID(szID, sizeof(szID));
	VOLOGI("Application ID is %s", szID);
    
    CreateCCRender();
    
    return VOOSMP_ERR_None;
}

int voOSPlayer::Uninit()
{
	if (m_pAudioRender != NULL)
	{
		m_pAudioRender->Stop ();
		delete m_pAudioRender;
		m_pAudioRender = NULL;
	}
	
	if (m_pVideoRender != NULL)
	{
        m_pVideoRender->Stop();
		delete m_pVideoRender;
		m_pVideoRender = NULL;
	}
    
    if (m_pCCRender != nil)
	{
        [(voCCManager *)m_pCCRender voStop];
		[(voCCManager *)m_pCCRender release];
		m_pCCRender = nil;
	}
    
    return COSVomePlayer::Uninit();
}

int voOSPlayer::SetView(void* pView)
{
    m_pData->m_hWnd = pView;
    
    if ((VOMP_PULL_MODE == m_nPlayMode) && (NULL != m_pVideoRender)) {
        m_pVideoRender->SetSurface(m_pData->m_hWnd);
    }
    
    if (m_pCCRender != nil)
	{
        [(voCCManager *)m_pCCRender setSurface:m_pData->m_hWnd];
	}
    
    UpdateCCRect();
    
    // Don't need set to vome2, vome2 will reinit decoder when set view, will lead to crash
    // COSVomeEngine::SetParam(VOMP_PID_DRAW_VIEW, m_pData->m_hWnd);
    
    return VOOSMP_ERR_None;
}

int voOSPlayer::SetDataSource (void * pSource, int nFlag)
{
	Stop();
    
    int nRC = VOMP_ERR_None;
    
    if ((NULL == m_pData->m_cSubTitleCB.pUserData) || (NULL == m_pData->m_cSubTitleCB.ReadSubtitle)) {
        VO_BOOL bCC = VO_TRUE;
        COSVomePlayer::SetParam(VOOSMP_PID_COMMON_CCPARSER, &bCC);
    }
    
    COSVomePlayer::SetParam(VOOSMP_PID_DRAW_RECT , &(m_pData->m_cDraw));
    
    if (0 != m_pData->m_nGetThumbnail) {
        int v = 1;
        COSVomeEngine::SetParam(VOMP_PID_VIDEO_ONLY, &v);
        
		m_nPlayMode = VOMP_PULL_MODE;
        COSVomePlayer::SetParam(VOMP_PID_VIDEO_PLAYMODE, &m_nPlayMode);
        
        int ct = VOOSMP_COLOR_RGB32_PACKED;
        COSVomePlayer::SetParam(VOOSMP_PID_DRAW_COLOR, &ct);
    }
	else 
	{
		int v = 0;
        COSVomeEngine::SetParam(VOMP_PID_VIDEO_ONLY, &v);
        
		COSVomeEngine::SetParam(VOMP_PID_VIDEO_PLAYMODE , &m_nPlayMode);
		COSVomeEngine::SetParam(VOMP_PID_AUDIO_PLAYMODE , &m_nPlayMode);
		
		if (m_nPlayMode == VOMP_PULL_MODE)
		{
			int nColorType = VOOSMP_COLOR_YUV_PLANAR420;
			COSVomePlayer::SetParam(VOOSMP_PID_DRAW_COLOR , &nColorType);
            int nRenderNum = 4;
            COSVomeEngine::SetParam(VOMP_PID_VIDEO_RNDNUM , &nRenderNum);
#if defined(_USE_OLD_AUDIO_RENDER) || defined (_MAC_OS)
#else
            int nAudioRndNum = 6;
            nRC = COSVomeEngine::SetParam (VOMP_PID_AUDIO_RNDNUM, &nAudioRndNum);
            if (nRC != VOMP_ERR_None) return ReturnCode(nRC);
#endif
		}
		else
		{
			COSVomePlayer::SetParam(VOOSMP_PID_DRAW_COLOR , &(m_pData->m_nColorType));
		}
	}
	
	COSVomeEngine::SetParam(VOMP_PID_GET_THUMBNAIL, &(m_pData->m_nGetThumbnail));
	
	if ((VOMP_PULL_MODE == m_nPlayMode) && (0 == m_pData->m_nGetThumbnail))
	{
		CreateAudioRender();
		CreateVideoRender();
	}
	
	return COSVomePlayer::SetDataSource(pSource, nFlag);
}

int voOSPlayer::Close()
{
    m_pData->m_nZoomMode = VOOSMP_ZOOM_LETTERBOX;
    m_pData->m_nAspectRatio = VO_RATIO_00;
    m_pData->m_nAspectRatioSet = VOOSMP_RATIO_AUTO;
    
    m_pData->m_nEnalbeSubTitleSetting = 0;
    
    return COSVomePlayer::Stop();
}

int voOSPlayer::Run (void)
{
	int nRet = COSVomePlayer::Run();
	
#if defined(_USE_OLD_AUDIO_RENDER)
    m_bEnableAudioRefClock = VO_TRUE;
    COSVomeEngine::SetParam(VOMP_PID_PAUSE_REFCOLOK, &m_bEnableAudioRefClock);
#endif

    if ((VOMP_PULL_MODE == m_nPlayMode) && (0 == m_pData->m_nGetThumbnail))
	{
        CreateAudioRender();
        CreateVideoRender();
	}
    
	if (m_pAudioRender != NULL)
		m_pAudioRender->Start ();
	
	if (m_pVideoRender != NULL) {
        m_pVideoRender->SetDrawRect(m_pData->m_cDraw.nLeft, m_pData->m_cDraw.nTop, m_pData->m_cDraw.nRight, m_pData->m_cDraw.nBottom);
        UpdateDispType();
		m_pVideoRender->Start ();
    }
    
    CreateCCRender();

    // Don't run cc render,just run while first video/audio frame rendered
    if (nil != m_pCCRender && m_bPaused)
	{
        [(voCCManager *)m_pCCRender voStart];
	}
    
    m_bPaused = false;
    
	return nRet;
}

int voOSPlayer::Pause (void)
{
	if (m_pAudioRender != NULL)
		m_pAudioRender->Pause ();
	
	if (m_pVideoRender != NULL)
		m_pVideoRender->Pause ();
	
    if (m_pCCRender != nil)
	{
        [(voCCManager *)m_pCCRender voPause];
	}
    
#if defined(_USE_OLD_AUDIO_RENDER)
    m_bEnableAudioRefClock = VO_TRUE;
    COSVomeEngine::SetParam(VOMP_PID_PAUSE_REFCOLOK, &m_bEnableAudioRefClock);
#endif
    
    m_bPaused = true;
    
	return COSVomePlayer::Pause();
}

int voOSPlayer::Stop (void)
{
    
    // Stop at first, destroy some resource asynchronous will cost some time, so delete it later.
    if (m_pAudioRender != NULL)
    {
        m_pAudioRender->Stop ();
    }
    
    if (m_pVideoRender != NULL)
    {
        m_pVideoRender->Stop ();
    }
    
    {
        voCAutoLock lock(&m_EventLock);
        
        if (nil != m_pCCRender) {
            [(voCCManager *)m_pCCRender voStop];
            [(voCCManager *)m_pCCRender release];
            m_pCCRender = nil;
        }
    }
    
	int nRet = COSVomePlayer::Stop();
    
    // delete audio render at last
//    if (m_pAudioRender != NULL)
//	{
//		delete m_pAudioRender;
//		m_pAudioRender = NULL;
//	}
    
    return nRet;
}

int voOSPlayer::SetPos(int nCurPos)
{
    if (m_pAudioRender != NULL)
	{
        m_pAudioRender->Flush();
	}
    
    if (m_pVideoRender != NULL)
	{
        m_pVideoRender->SetPos(nCurPos);
	}
    
    if (nil != m_pCCRender)
    {
        [(voCCManager *)m_pCCRender voFlush];
        
        // don't try to get subtitle on pause status now [(voCCManager *)m_pCCRender updateScreen];
    }
    
	int nRet = COSVomePlayer::SetPos(nCurPos);
    
    if (VOOSMP_ERR_None != nRet) {
        return -1;
    }

	return nCurPos;
}

int voOSPlayer::GetPos(int * pCurPos)
{
    if (NULL == pCurPos) {
        return VOOSMP_ERR_Pointer;
    }
    
    int nRet = COSVomePlayer::GetPos(pCurPos);
    
	if (VOOSMP_ERR_None != nRet) {
        return -1;
    }
	
	return *pCurPos;
}

int voOSPlayer::GetParam(int nParamID, void* pParam)
{
    if (VOOSMP_PID_DRAW_RECT == nParamID) {
        if (NULL != m_pVideoRender && NULL != pParam) {
            VO_RECT cValue;
            int nRet = m_pVideoRender->GetDrawRect(&cValue);
            if (VO_ERR_NONE != nRet) {
                return nRet;
            }
            
            VOOSMP_RECT* pRect = (VOOSMP_RECT*)pParam;
            pRect->nLeft = cValue.left;
            pRect->nRight = cValue.right;
            pRect->nTop = cValue.top;
            pRect->nBottom = cValue.bottom;
            return VOOSMP_ERR_None;
        } 
    }
    else if (VOOSMP_PID_CAPTURE_VIDEO_IMAGE == nParamID) {
        if (NULL == m_pVideoRender) {
            return VOOSMP_ERR_Retry;
        }
        
        return m_pVideoRender->GetParam(VO_VR_PMID_CAPTURE_VIDEO_IMAGE, pParam);
    }
    
	return COSVomePlayer::GetParam(nParamID, pParam);
}

int	voOSPlayer::SetParam(int nParamID, void* pParam)
{
    int nRet = VOOSMP_ERR_None;
    
    switch (nParamID) {
        case VOOSMP_PID_AUDIO_VOLUME:
        {
#if defined(_IOS)
            if (NULL == pParam) {
                return VOOSMP_ERR_Pointer;
            }
            
            float volume = (float)(*(int *)pParam) / 100;
            if (NULL != m_pAudioRender) {
                m_pAudioRender->SetVolume(volume, volume);
            }
            
            return VOOSMP_ERR_None;
#else
            nRet = COSVomePlayer::SetParam(nParamID, pParam);
#endif
        }
            break;
        case VOOSMP_PID_DRAW_RECT:
        {
            if (NULL == pParam) {
                return VOOSMP_ERR_Pointer;
            }
            
            nRet = SetDrawRect((VOOSMP_RECT*)pParam);
        }
            break;
        case VOOSMP_PID_APPLICATION_SUSPEND:
        {            
            voSystemStatus::Instance()->SetAPPActive(false);
            
            bool bPlayAudioBackground = *(bool*)pParam;
            
            if (bPlayAudioBackground)
            {                
                if (NULL != m_pVideoRender) {
                    m_pVideoRender->Pause();
                }
                
                if (NULL != m_pCCRender) {
                    [(voCCManager *)m_pCCRender voPause];
                }
                
                COSVomeEngine::SetParam(VOMP_PID_VIDEO_PLAY_BACK, 0);
            }
            else {
#if defined(_USE_OLD_AUDIO_RENDER)
                m_bEnableAudioRefClock = VO_TRUE;
                COSVomeEngine::SetParam(VOMP_PID_PAUSE_REFCOLOK, &m_bEnableAudioRefClock);
#endif
                //It should be controlled by high level module
                //Pause();
            }
            
            return VOOSMP_ERR_None;
        }
            break;
        case VOOSMP_PID_APPLICATION_RESUME:
        {
            voSystemStatus::Instance()->SetAPPActive(true);
            
            bool bPlayAudioBackground = *(bool*)pParam;
            
            if (bPlayAudioBackground)
            {
                if (NULL != m_pVideoRender) {
                    m_pVideoRender->Start();
                }
                
                if (NULL != m_pAudioRender) {
                    m_pAudioRender->Start();
                }
                
                if (NULL != m_pCCRender) {
                    [(voCCManager *)m_pCCRender voStart];
                }
                
                int video = 1;
                COSVomeEngine::SetParam(VOMP_PID_VIDEO_PLAY_BACK, &video);
            }
            else {
                //It should be controlled by high level module
                //Run();
            }
            
            return VOOSMP_ERR_None;
        }
            break;
            
        case VOOSMP_PID_DRAW_COLOR:
        {
            if (NULL == pParam) {
                return VOOSMP_ERR_Pointer;
            }
            
            int nColor = *((int*)pParam);
            m_pData->m_nColorType = ConvertColorOS2MP((VOOSMP_COLORTYPE *)pParam);
            
            if (m_nPlayMode == VOMP_PULL_MODE)
            {
                nColor = VOOSMP_COLOR_YUV_PLANAR420;
                
                if (NULL != m_pVideoRender) {
                    m_pVideoRender->SetColorType((VOMP_COLORTYPE)m_pData->m_nColorType);
                }
            }
			
            if (0 != m_pData->m_nGetThumbnail)
            {
                nColor = VOOSMP_COLOR_RGB32_PACKED;
            }
            
            nRet = COSVomePlayer::SetParam(VOOSMP_PID_DRAW_COLOR, &nColor);
        }
            break;
            
        case VOOSMP_PID_ZOOM_MODE:
        {
            if (NULL == pParam) {
                return VOOSMP_ERR_Pointer;
            }
            
            m_pData->m_nZoomMode = voOSPlayerData::ConvertZoomModeOS2IV((VOOSMP_ZOOM_MODE *)pParam);
            
            UpdateDispType();
			
            nRet = COSVomePlayer::SetParam(VOMP_PID_ZOOM_MODE, &m_pData->m_nZoomMode);
        }
            break;
            
        case VOOSMP_PID_VIDEO_ASPECT_RATIO:
        {
            if (NULL == pParam) {
                return VOOSMP_ERR_Pointer;
            }
            
            VOLOGI("SetParam VOOSMP_PID_VIDEO_ASPECT_RATIO:%d", *(int *)pParam);
            
            m_pData->m_nAspectRatioSet = voOSPlayerData::ConvertAspectRatioOS2IV((VOOSMP_ASPECT_RATIO *)pParam);
            
            UpdateDispType();
			
            nRet = VOOSMP_ERR_None;
        }
            break;
            
        case VOOSMP_PID_COMMON_CCPARSER:
        {
            if (NULL == pParam) {
                return VOOSMP_ERR_Pointer;
            }
            
            if (VO_TRUE == *((VO_BOOL *)pParam)) {
                m_pData->m_bSubtitleDisplay = true;
            }
            else {
                m_pData->m_bSubtitleDisplay = false;
            }
            
            if (m_pCCRender != nil)
            {
                [(voCCManager *)m_pCCRender setSubtitleShow:m_pData->m_bSubtitleDisplay];
            }
            
            nRet = VOOSMP_ERR_None;
        }
            break;
            
        case VOOSMP_PID_SUBTITLE_SETTINGS_ONOFF:
        {
            if (NULL == pParam) {
                return VOOSMP_ERR_Pointer;
            }
            
            COSVomePlayer::SetParam(nParamID, pParam);
            m_pData->m_nEnalbeSubTitleSetting = *(int *)pParam;
            if (m_pCCRender != nil)
            {
                [(voCCManager *)m_pCCRender setSettingsEnable:m_pData->m_nEnalbeSubTitleSetting];
            }
            
            nRet = VOOSMP_ERR_None;
        }
            break;
        
        case VOOSMP_PID_SUBTITLE_SETTINGS:
        {
            if (NULL == pParam) {
                return VOOSMP_ERR_Pointer;
            }
            
            if(!m_pCCRender)
            {
                m_pCCRender = [[voCCManager alloc] init:this pQuery:ProcessCCRenderQuery];
            }
            
            memcpy(&(m_pData->m_cSubTitleSetting), pParam, sizeof(m_pData->m_cSubTitleSetting));
            
            if (m_pCCRender != nil)
            {
                [(voCCManager *)m_pCCRender setSettingsInfo:&(m_pData->m_cSubTitleSetting)];
            }
            nRet = VOOSMP_ERR_None;
        }
            break;
            
        case VOOSMP_PID_READ_SUBTITLE_CALLBACK:
        {
            if (NULL != pParam) {
                memcpy(&(m_pData->m_cSubTitleCB), pParam, sizeof(m_pData->m_cSubTitleCB));
            }
            else {
                memset(&(m_pData->m_cSubTitleCB), 0, sizeof(m_pData->m_cSubTitleCB));
            }
            
            nRet = VOOSMP_ERR_None;
        }
            break;
        case VOOSMP_PID_PREVIEW_SUBTITLE:
        {
            if (m_pCCRender != nil)
            {
                [(voCCManager *)m_pCCRender previewSubtitle:(VOOSMP_PREVIEW_SUBTITLE_INFO*)pParam];
            }
            nRet = VOOSMP_ERR_None;
        }
            break;
        default:
            nRet = COSVomePlayer::SetParam(nParamID, pParam);
            break;
    }
	
	return nRet;
}

int voOSPlayer::SetDrawRect(VOOSMP_RECT* pRect)
{
    if (NULL == pRect) {
        return VOOSMP_ERR_Pointer;
    }
    
	memcpy(&(m_pData->m_cDraw), pRect, sizeof(VOOSMP_RECT));
    
	if (m_pVideoRender)
	{
		m_pVideoRender->SetDrawRect(m_pData->m_cDraw.nLeft, m_pData->m_cDraw.nTop, m_pData->m_cDraw.nRight, m_pData->m_cDraw.nBottom);
	}
	
    UpdateCCRect();
    
	return COSVomePlayer::SetParam(VOOSMP_PID_DRAW_RECT, pRect);
}

int voOSPlayer::UpdateDispType()
{
    if (NULL == m_pVideoRender) {
        return VOOSMP_ERR_Pointer;
    }
    
    if (VOOSMP_RATIO_AUTO != m_pData->m_nAspectRatioSet) {
        m_pVideoRender->SetDispType((VO_IV_ZOOM_MODE)m_pData->m_nZoomMode, (VO_IV_ASPECT_RATIO)m_pData->m_nAspectRatioSet);
    }
    else {
        m_pVideoRender->SetDispType((VO_IV_ZOOM_MODE)m_pData->m_nZoomMode, (VO_IV_ASPECT_RATIO)m_pData->m_nAspectRatio);
    }
    
    UpdateCCRect();
    
    return VOOSMP_ERR_None;
}

int voOSPlayer::UpdateCCRect()
{
    if (nil != m_pCCRender) {
        VOOSMP_RECT cRect;
        memset(&cRect, 0, sizeof(cRect));
        if (VO_ERR_NONE != GetParam(VOOSMP_PID_DRAW_RECT, &cRect)) {
            
            memcpy(&cRect, &(m_pData->m_cDraw), sizeof(cRect));
        }
        
        [(voCCManager *)m_pCCRender setDrawRect:cRect.nLeft nTop:cRect.nTop nRight:cRect.nRight nBottom:cRect.nBottom];
    }
    
    return VOOSMP_ERR_None;
}

int voOSPlayer::CreateAudioRender (void)
{
	if (m_pAudioRender != NULL)
		return VOOSMP_ERR_None;
	
	m_pAudioRender = new CAudioRenderManager(this, ProcessRenderQuery);
	
	return VOOSMP_ERR_None;
}

int voOSPlayer::CreateVideoRender (void)
{
	if (m_pVideoRender != NULL)
		return VOOSMP_ERR_None;
	
    if (!voSystemStatus::Instance()->IsAPPActive()) {
        return VOOSMP_ERR_Status;
    }
    
	m_pVideoRender = new CVideoRenderManager(this, ProcessRenderQuery);
	
	if (m_pVideoRender != NULL)
	{
		m_pVideoRender->SetColorType((VOMP_COLORTYPE)m_pData->m_nColorType);
		
		m_pVideoRender->SetSurface (m_pData->m_hWnd);
        
        m_pVideoRender->SetWorkPath((TCHAR *)m_szPathLib);
	}
	
	return VOOSMP_ERR_None;
}

int voOSPlayer::CreateCCRender (void)
{
	if (m_pCCRender != nil)
		return VOOSMP_ERR_None;

    m_pCCRender = [[voCCManager alloc] init:this pQuery:ProcessCCRenderQuery];
	
	if (m_pCCRender != nil)
	{
        [(voCCManager *)m_pCCRender setSurface:m_pData->m_hWnd];
        [(voCCManager *)m_pCCRender setSettingsEnable:m_pData->m_nEnalbeSubTitleSetting];
        [(voCCManager *)m_pCCRender setSettingsInfo:&(m_pData->m_cSubTitleSetting)];
        [(voCCManager *)m_pCCRender setSubtitleShow:m_pData->m_bSubtitleDisplay];
	}
    
    UpdateCCRect();
	
	return VOOSMP_ERR_None;
}

int	voOSPlayer::ProcessCCRenderQuery(void * pUserData, int nQT, void * pParam1, void * pParam2)
{
    if (pUserData)
	{
		voOSPlayer* pPlayer = (voOSPlayer*)pUserData;
		int nRet =  pPlayer->doProcessCCRenderQuery(nQT, pParam1, pParam2);
		
		return nRet;
	}
	
	return 0;
}

int	voOSPlayer::doProcessCCRenderQuery(int nQT, void * pParam1, void * pParam2)
{
    int nRet = VOOSMP_ERR_None;
	
	if (CC_QT_GET_BUFFER == nQT)
	{
        voSubtitleInfo *pSubTitle = (voSubtitleInfo *)pParam1;
        if (NULL == pSubTitle) {
            return VOOSMP_ERR_Pointer;
        }
        
        int nPos = 0;
        VO_U64 position = GetPos(&nPos);
        pSubTitle->nTimeStamp = position;
        
        
        if ((NULL != m_pData->m_cSubTitleCB.pUserData) && (NULL != m_pData->m_cSubTitleCB.ReadSubtitle)) {
            nRet = m_pData->m_cSubTitleCB.ReadSubtitle(m_pData->m_cSubTitleCB.pUserData, pSubTitle);
        }
        else {
            nRet = GetSubtileSample(pSubTitle);
        }
        
        if ((VOOSMP_ERR_None == nRet) && (NULL != pSubTitle->pSubtitleEntry)) {
            
            if ((pSubTitle->nTimeStamp + pSubTitle->pSubtitleEntry->nDuration) < position) {
                return VOOSMP_ERR_Retry;
            }
        }
	}
    else if (CC_QT_GET_TIMESTAMP == nQT)
    {
        int nPos = 0;
        return GetPos(&nPos);
    }
    
	return nRet;
}

int	voOSPlayer::ProcessRenderQuery(void * pUserData, QueryType nQT, void * pParam1, void * pParam2)
{
	if (pUserData)
	{
		voOSPlayer* pPlayer = (voOSPlayer*)pUserData;
		int nRet =  pPlayer->doProcessRenderQuery(nQT, pParam1, pParam2);
		
		//below is debug info
#if 0
		if(QT_GET_AUDIO_FMT == nQT)
		{
			VOMP_AUDIO_FORMAT* pFmt = (VOMP_AUDIO_FORMAT*)pParam1;
			VOLOGI("Render query audio format, %d %d %d", pFmt->SampleRate, pFmt->Channels, pFmt->SampleBits);
		}
		else if(QT_GET_VIDEO_FMT == nQT)
		{
			VOMP_VIDEO_FORMAT* pFmt = (VOMP_VIDEO_FORMAT*)pParam1;
			VOLOGI("Render query video format %d %d", pFmt->Width, pFmt->Height);
		}
		else if(QT_GET_ADUIO_BUFFER == nQT)
		{
		}
		else if(QT_GET_VIDEO_BUFFER == nQT)
		{
			if(nRet == VOMP_ERR_None)
			{
				VOMP_BUFFERTYPE* pBuffer = *(VOMP_BUFFERTYPE**)pParam1;
				if(pBuffer)
				{
					VOLOGI("Video render query buffer: %08lld, %08d", pBuffer->llTime, (pBuffer->nSize & 0X7FFFFFFF));
				}
			}
		}
		else if(QT_SET_AUDIO_RENDER_BUF_TIME == nQT)
		{
		}
#endif
		// end debug info
		
		return nRet;
	}
	
	return 0;
}

int voOSPlayer::doProcessRenderQuery(QueryType nQT, void * pParam1, void * pParam2)
{
	int nRet = VOMP_ERR_Retry;
	
	if(QT_GET_AUDIO_FMT == nQT)
	{
        if (NULL == pParam1) {
            return VOOSMP_ERR_Pointer;
        }
		VOMP_AUDIO_FORMAT* pFmt = (VOMP_AUDIO_FORMAT*)pParam1;
		nRet = GetAudioFormat(pFmt);
	}
	else if(QT_GET_VIDEO_FMT == nQT)
	{
        if (NULL == pParam1) {
            return VOOSMP_ERR_Pointer;
        }
		VOMP_VIDEO_FORMAT* pFmt = (VOMP_VIDEO_FORMAT*)pParam1;
		nRet = GetVideoFormat(pFmt);
	}
	else if(QT_GET_ADUIO_BUFFER == nQT)
	{
        if (NULL == pParam1) {
            return VOOSMP_ERR_Pointer;
        }
		nRet = GetAudioSample((VOMP_BUFFERTYPE**)pParam1);
        
        if(nRet == VOMP_ERR_None)
        {
            if(m_pCCRender)
            {
                [(voCCManager*)m_pCCRender voStart];
            }
        }
	}
	else if(QT_GET_VIDEO_BUFFER == nQT)
	{
        if (NULL == pParam1) {
            return VOOSMP_ERR_Pointer;
        }
		nRet = GetVideoSample((VOMP_BUFFERTYPE**)pParam1);
        
        if (VOMP_ERR_None == nRet) {
            VO_BOOL bUpside = VO_FALSE;
            
            int iRet = COSVomeEngine::GetParam(VOMP_PID_VIDEO_VIDEO_UPSIDE, &bUpside);
            
            if ((VOMP_ERR_None == iRet) && (NULL != m_pVideoRender))
            {
                m_pVideoRender->SetParam(VO_PID_VIDEO_UPSIDEDOWN, &bUpside);
            }
        }
	}
	else if(QT_SET_AUDIO_RENDER_BUF_TIME == nQT)
	{
		nRet = COSVomeEngine::SetParam(VOMP_PID_AUDIO_SINK_BUFFTIME, pParam1);
	}
    else if (QT_NOTIFY_VIDEO_FORMAT_CHANGE == nQT) {
        UpdateCCRect();
        
        VOMP_VIDEO_FORMAT cFmt;
        memset(&cFmt, 0, sizeof(cFmt));
        
		nRet = GetVideoFormat(&cFmt);
        
        if (VOMP_ERR_None == nRet && m_fListener.pUserData != NULL && m_fListener.pListener != NULL)
        {
            m_fListener.pListener(m_fListener.pUserData, VOOSMP_CB_VideoSizeChanged, &(cFmt.Width), &(cFmt.Height));
        }
    }
    else if (QT_RETURN_VIDEO_BUFFER == nQT) {
        COSVomeEngine::SetParam(VOMP_PID_SIGNAL_BUFFER_RETURNED, pParam1);
        nRet = VOMP_ERR_None;
    }
    else if (QT_NOTIFY_VIDEO_DISPLAY_CHANGE == nQT) {
        if ((nil != m_pCCRender) && (NULL != pParam1)) {
            VO_RECT *pRect = (VO_RECT *)pParam1;
            [(voCCManager *)m_pCCRender setDrawRect:pRect->left nTop:pRect->top nRight:pRect->right nBottom:pRect->bottom];
        }
    }
    else if(QT_NOTIFY_AUDIO_RENDER_READY == nQT)
    {
#if defined(_USE_OLD_AUDIO_RENDER)
        if(m_bEnableAudioRefClock && voSystemStatus::Instance()->IsAPPActive())
        {
            m_bEnableAudioRefClock = VO_FALSE;
            COSVomeEngine::SetParam(VOMP_PID_PAUSE_REFCOLOK, &m_bEnableAudioRefClock);
        }
#endif
    }
	
	return nRet;
}

int voOSPlayer::GetAudioFormat(VOMP_AUDIO_FORMAT* pFmt)
{
	int nRet = VOMP_ERR_Retry;
	
	nRet = COSVomeEngine::GetParam(VOMP_PID_AUDIO_FORMAT, pFmt);
	
	return nRet;
}

int	voOSPlayer::GetVideoFormat(VOMP_VIDEO_FORMAT* pFmt)
{
	int nRet = VOMP_ERR_Retry;
	
    nRet = COSVomeEngine::GetParam(VOMP_PID_VIDEO_FORMAT, pFmt);
	
	return nRet;
}

int voOSPlayer::GetAudioSample(VOMP_BUFFERTYPE** pOutBuffer)
{
    int nRet = COSVomeEngine::GetAudioBuffer(pOutBuffer);

	return nRet;
}

int voOSPlayer::GetVideoSample(VOMP_BUFFERTYPE** pOutBuffer)
{
    int nRet = COSVomeEngine::GetVideoBuffer(pOutBuffer);
    
	return nRet;
}

int	voOSPlayer::HandleEvent (int nID, void * pParam1, void * pParam2)
{
    if (VOOSMP_CB_VideoAspectRatio == nID) {
        
        if (NULL == pParam1) {
            return VOOSMP_ERR_Pointer;
        }
        
        VOLOGI("handle VOOSMP_CB_VideoAspectRatio:%d", *(int *)pParam1);
        
        if (NULL != m_pVideoRender) {
            m_pData->m_nAspectRatio = *(int *)pParam1;
            
            UpdateDispType();
        }
    }
    else if (VOOSMP_CB_Metadata_Arrive == nID) {
        if ((NULL != pParam1)
            && (VOOSMP_SS_IMAGE == *(int *)pParam1)) {
            
            VOOSMP_STATUS cStatus = VOOSMP_STATUS_INIT;
            GetParam(VOOSMP_PID_STATUS, &cStatus);
            
            // don't try to get subtitle on pause status now
            if ((VOOSMP_STATUS_RUNNING == cStatus) /*|| (VOOSMP_STATUS_PAUSED == cStatus)*/) {
                CreateCCRender();
                
                if (m_pCCRender != nil)
                {
                    voSubtitleInfo** pID3 = (voSubtitleInfo **)pParam2;
                    [(voCCManager *)m_pCCRender renderID3:*pID3];
                }
            }
        }
    }
    else if (VOOSMP_CB_MediaTypeChanged == nID) {
        
        if ((NULL != pParam1) && (m_pCCRender != nil)) {
            
            if ((VOOSMP_AVAILABLE_AUDIOVIDEO == *(int *)pParam1)
                || (VOOSMP_AVAILABLE_PUREVIDEO == *(int *)pParam1)) {
                
                [(voCCManager *)m_pCCRender voFlushID3];
            }
        }
    }
    else if(VOOSMP_CB_VideoRenderStart == nID)
    {
        if(m_pCCRender)
        {
            [(voCCManager*)m_pCCRender voStart];
        }
    }
    
	return COSVomePlayer::HandleEvent(nID, pParam1, pParam2);
}

void * voOSPlayer::vomtGetAddress (void * pUserData, void * hLib, char * pFuncName, int nFlag)
{
#ifdef _IOS
    voGetModuleRealAPI pFunction = (voGetModuleRealAPI)voGetModuleAdapterFunc(pFuncName);
    
    if (pFunction) {
        return pFunction();
    }
    
    return NULL;
#else
	
    if ((NULL == hLib) || (NULL == pFuncName))
		return NULL;
    
    return dlsym (hLib, pFuncName);
#endif
}

int voOSPlayer::vomtFreeLib (void * pUserData, void * hLib, int nFlag)
{
    if (hLib != NULL)
		dlclose (hLib);
    
	return 0;
}

void * voOSPlayer::vomtLoadLib (void * pUserData, char * pLibName, int nFlag)
{
    if (NULL == pUserData) {
        return NULL;
    }
    
    return ((voOSPlayer *)pUserData)->voOnLoadLib(pUserData, pLibName, nFlag);
}

void* voOSPlayer::voOnLoadLib (void * pUserData, char * pLibName, int nFlag)
{
#ifdef _IOS
    VOLOGI("Load lib name: %s", pLibName);
    // Return a no-zero value to continue load on iOS
    return (void *)1;
#elif _MAC_OS
    
    if (NULL == pLibName) {
        return NULL;
    }
    
    VO_TCHAR libname[256] = {0};
    VO_TCHAR PathAndName [1024] = {0};
    
    if (0 != strncasecmp (pLibName, "lib", 3))
    {
        vostrcat(libname, "lib");
    }
    vostrcat(libname, pLibName);
    if (vostrstr(libname, ".dylib") == 0)
    {
        vostrcat(libname, ".dylib");
    }
    
	bool bAPP = false;
	VOLOGI ("dllfile: %s, workpath: %s!", libname, m_szPathLib);
    
	if (m_hDll == NULL && strcmp(m_szPathLib, ""))
	{
		bAPP = true;
        
		vostrcpy(PathAndName, m_szPathLib);
        
        if (m_szPathLib[strlen(m_szPathLib) - 1] != '/') {
            vostrcat(PathAndName, "/");
        }
        
		vostrcat(PathAndName, libname);
        
		m_hDll = dlopen (PathAndName, RTLD_NOW);
	}
    
	VOLOGI ("bAPP: %d", bAPP);
    
    if (!bAPP && m_hDll == NULL)
	{
        voOS_GetAppFolder(PathAndName, sizeof(PathAndName));
        vostrcat(PathAndName, libname);
        m_hDll = dlopen (PathAndName, RTLD_NOW);
        if (NULL == m_hDll)
        {
            voOS_GetAppFolder(PathAndName, sizeof(PathAndName));
            vostrcat(PathAndName, "lib/");
            vostrcat(PathAndName, libname);
            m_hDll = dlopen (PathAndName, RTLD_NOW);
        }
        
        if (NULL == m_hDll)
        {
            voOS_GetPluginModuleFolder(PathAndName, sizeof(PathAndName));
            vostrcat(PathAndName, libname);
            m_hDll = dlopen (PathAndName, RTLD_NOW);
        }
        
        if (NULL == m_hDll)
        {
            vostrcpy(PathAndName, "/Library/Internet Plug-Ins/");
            vostrcat(PathAndName, libname);
            m_hDll = dlopen (PathAndName, RTLD_NOW);
        }
        
        if (NULL == m_hDll)
        {
            m_hDll = dlopen(libname, RTLD_NOW);
        }
        
        if (NULL == m_hDll)
        {
            if (strrchr(libname, '/') == NULL)
                vostrcpy(PathAndName, "/usr/lib/");
            
            vostrcat (PathAndName, libname);
            m_hDll = dlopen (PathAndName, RTLD_NOW);
        }
	}
    
	if (m_hDll)
	{
		VOLOGI("load: %s ok", PathAndName);
	}
    else {
        VOLOGI("load: %s error", libname);
    }
    
    return m_hDll;
#else
    return NULL;
#endif
}
