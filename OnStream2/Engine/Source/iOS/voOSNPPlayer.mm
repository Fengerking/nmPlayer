/*******************************************************************************
 *	File:		voOSNPPlayer.cpp
 *	
 *	Contains:	voOSNPPlayer class file
 *	
 *	Written by:	Jeff huang
 *	
 *	Change History (most recent first):
 *	2012-03-20		Jeff			Create file
 *	
 ******************************************************************************/

#include "voOSNPPlayer.h"

#include "voOSFunc.h"
#include "voLog.h"
#include "voIVCommon.h"

#include "voOnStreamType.h"


#include "voCCManager.h"
#include "voLoadLibControl.h"

#include "voVideoRender.h"

voOSNPPlayer::voOSNPPlayer(voOSPlayerData *pData)
:COMXALPlayer()
,m_pData(pData)
,m_pCCRender(nil)
,m_pPlayerViewOld(nil)
,m_pPlayerView(nil)
,m_bPaused(false)
{
}

voOSNPPlayer::~voOSNPPlayer ()
{
	Uninit();
    
    if (m_pPlayerViewOld) {
        m_cVideoRender.SetSecondView(NULL);
        [m_pPlayerViewOld removeFromSuperview];
        [m_pPlayerViewOld release];
        m_pPlayerViewOld = nil;
    }
    
    if (m_pPlayerView) {
        
        [m_pPlayerView removeFromSuperview];
        [m_pPlayerView release];
        m_pPlayerView = nil;
    }
}

int voOSNPPlayer::Init()
{
    if (NULL == m_pData) {
        return VOOSMP_ERR_Pointer;
    }
    
    int nRC = COMXALPlayer::Init();
	
	if(nRC != VOOSMP_ERR_None) 
		return nRC;
    
	VO_TCHAR szID[256];
	memset(szID, 0, sizeof(szID));
	voOS_GetApplicationID(szID, sizeof(szID));
	VOLOGI("Application ID is %s", szID);
    
    CreateCCRender();
    
    return VOOSMP_ERR_None;
}

int voOSNPPlayer::Uninit()
{
    Stop();
    
    return COMXALPlayer::Uninit();
}

int voOSNPPlayer::SetView(void* pView)
{
    if (m_pData->m_hWnd == pView) {
        m_cVideoRender.SetDispRect(&(m_pData->m_cDraw));
        return VOOSMP_ERR_None;
    }
    
    if ([NSRunLoop mainRunLoop] != [NSRunLoop currentRunLoop]) {
        return VOOSMP_ERR_Status;
    }
    
    m_pData->m_hWnd = pView;
    
    if (m_pPlayerView) {
        [m_pPlayerView removeFromSuperview];
        [m_pPlayerView release];
        m_pPlayerView = nil;
    }
    
    CGRect cRect = ((UIView *)m_pData->m_hWnd).bounds;
//    if (UIDeviceOrientationIsLandscape([[UIDevice currentDevice] orientation])) {
//        cRect.size.width = ((UIView *)m_pData->m_hWnd).frame.size.height;
//        cRect.size.height = ((UIView *)m_pData->m_hWnd).frame.size.width;
//    }
    
    m_pPlayerView = [[voPlayerView alloc] initWithFrame:cRect];
    [m_pPlayerView setAutoresizingMask:(UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight)];
    [((UIView *)m_pData->m_hWnd) insertSubview:m_pPlayerView atIndex:0];
    
    m_cVideoRender.SetView(m_pPlayerView);
    m_cVideoRender.SetDispRect(&(m_pData->m_cDraw));
    
    if (m_pCCRender != nil)
	{
        [(voCCManager *)m_pCCRender setSurface:m_pData->m_hWnd];
	}
    
    UpdateCCRect();
    
    COMXALPlayer::SetView(m_pPlayerView);
    
    return VOOSMP_ERR_None;
}

int voOSNPPlayer::SetDataSource (void * pSource, int nFlag)
{
	Stop();
    
    if ((NULL == m_pData->m_cSubTitleCB.pUserData) || (NULL == m_pData->m_cSubTitleCB.ReadSubtitle)) {
        VO_BOOL bCC = VO_TRUE;
        COMXALPlayer::SetParam(VOOSMP_PID_COMMON_CCPARSER, &bCC);
    }
    
    COMXALPlayer::SetView(m_pPlayerView);
	
	return COMXALPlayer::SetDataSource(pSource, nFlag);
}

int voOSNPPlayer::Close()
{
    m_pData->m_nZoomMode = VOOSMP_ZOOM_LETTERBOX;
    m_pData->m_nAspectRatio = VO_RATIO_00;
    m_pData->m_nAspectRatioSet = VOOSMP_RATIO_AUTO;
    
    m_pData->m_nEnalbeSubTitleSetting = 0;
    
    return COMXALPlayer::Close();
}

int voOSNPPlayer::Run (void)
{
	int nRet = COMXALPlayer::Run();

    CreateCCRender();

    if (nil != m_pCCRender && m_bPaused)
	{
        [(voCCManager *)m_pCCRender voStart];
	}
    
    m_bPaused = false;
    
    UpdateDispType();
    
	return nRet;
}

int voOSNPPlayer::Pause (void)
{
    if (m_pCCRender != nil)
	{
        [(voCCManager *)m_pCCRender voPause];
	}
    
    m_bPaused = true;
    
	return COMXALPlayer::Pause();
}

int voOSNPPlayer::Stop (void)
{
    {
        // Protect for cc only. The stop operation maybe still need send event
        voCAutoLock lock(&m_EventLock);
        
        if (nil != m_pCCRender) {
            [(voCCManager *)m_pCCRender voStop];
            [(voCCManager *)m_pCCRender release];
            m_pCCRender = nil;
        }
    }
    
    int nRet = COMXALPlayer::Stop();
    
    if (m_pPlayerViewOld) {
        m_cVideoRender.SetSecondView(NULL);
        
        [m_pPlayerViewOld removeFromSuperview];
        [m_pPlayerViewOld release];
        m_pPlayerViewOld = nil;
    }
    
    return nRet;
}

int voOSNPPlayer::SetPos(int nCurPos)
{
	int nRet = COMXALPlayer::SetPos(nCurPos);
    
    if (VOOSMP_ERR_None != nRet) {
        return -1;
    }

    if ([NSRunLoop mainRunLoop] != [NSRunLoop currentRunLoop]) {
        return VOOSMP_ERR_Status;
    }
    
    if (nil != m_pCCRender)
    {
        [(voCCManager *)m_pCCRender voFlush];
        
        // don't try to get subtitle on pause status now [(voCCManager *)m_pCCRender updateScreen];
    }
    
    // if seeking(old exist), don't do reset
    if ((NULL == m_pPlayerView) || (NULL != m_pPlayerViewOld)) {
        return nCurPos;
    }
    
    m_pPlayerViewOld = m_pPlayerView;
    
    CGRect cRect = m_pPlayerViewOld.frame;
    
    m_pPlayerView = [[voPlayerView alloc] initWithFrame:cRect];
    [m_pPlayerView setAutoresizingMask:(UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight)];
    [((UIView *)m_pData->m_hWnd) insertSubview:m_pPlayerView atIndex:0];
    
    m_cVideoRender.SetView(m_pPlayerView);
    m_cVideoRender.SetSecondView(m_pPlayerViewOld);
    COMXALPlayer::SetView(m_pPlayerView);

	return nCurPos;
}

int voOSNPPlayer::GetPos(int* pCurPos)
{
    if (NULL == pCurPos) {
        return VOOSMP_ERR_Pointer;
    }
    
    int nRet = COMXALPlayer::GetPos(pCurPos);
    
	if (VOOSMP_ERR_None != nRet) {
        return -1;
    }
	
	return *pCurPos;
}

int voOSNPPlayer::GetParam(int nParamID, void* pParam)
{
    if (VOOSMP_PID_DRAW_RECT == nParamID) {
        m_cVideoRender.GetParam(VO_VR_PMID_DrawRect, pParam);
        return VOOSMP_ERR_None;
    }
    
	return COMXALPlayer::GetParam(nParamID, pParam);
}

int	voOSNPPlayer::SetParam(int nParamID, void* pParam)
{
    if ((VOOSMP_PID_VIDEO_REDRAW != nParamID) 
        && (NULL == pParam)) {
        return COMXALPlayer::SetParam(nParamID, pParam);
    }
    
    int nRet = VOOSMP_ERR_None;
    
    switch (nParamID) {
        case VOOSMP_PID_DRAW_RECT:
        {
            if (NULL == pParam) {
                return VOOSMP_ERR_Pointer;
            }
            
            nRet = SetDrawRect((VOOSMP_RECT*)pParam);
        }
            break;
            
        case VOOSMP_PID_ZOOM_MODE:
        {
            if (NULL == pParam) {
                return VOOSMP_ERR_Pointer;
            }
            
            m_pData->m_nZoomMode = voOSPlayerData::ConvertZoomModeOS2IV((VOOSMP_ZOOM_MODE *)pParam);
            
            UpdateDispType();
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
            
            COMXALPlayer::SetParam(nParamID, pParam);
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
            
            COMXALPlayer::SetParam(nParamID, pParam);
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
            nRet = COMXALPlayer::SetParam(nParamID, pParam);
            break;
    }
	
	return nRet;
}

int voOSNPPlayer::SetDrawRect(VOOSMP_RECT* pRect)
{
    if (NULL == pRect) {
        return VOOSMP_ERR_Pointer;
    }
    
	memcpy(&(m_pData->m_cDraw), pRect, sizeof(VOOSMP_RECT));
    
    m_cVideoRender.SetDispRect(pRect);
	
    UpdateCCRect();
    
	return VOOSMP_ERR_None;
}

int voOSNPPlayer::UpdateDispType()
{
    if (VOOSMP_RATIO_AUTO != m_pData->m_nAspectRatioSet) {
        m_cVideoRender.SetDispType((VO_IV_ZOOM_MODE)m_pData->m_nZoomMode, (VO_IV_ASPECT_RATIO)m_pData->m_nAspectRatioSet);
    }
    else {
        m_cVideoRender.SetDispType((VO_IV_ZOOM_MODE)m_pData->m_nZoomMode, (VO_IV_ASPECT_RATIO)m_pData->m_nAspectRatio);
    }

    UpdateCCRect();
    
    return VOOSMP_ERR_None;
}

int voOSNPPlayer::UpdateCCRect()
{
    if (nil != m_pCCRender) {
        VOOSMP_RECT cRect;
        memset(&cRect, 0, sizeof(cRect));
        GetParam(VOOSMP_PID_DRAW_RECT, &cRect);
        
        [(voCCManager *)m_pCCRender setDrawRect:cRect.nLeft nTop:cRect.nTop nRight:cRect.nRight nBottom:cRect.nBottom];
    }
    
    return VOOSMP_ERR_None;
}

int voOSNPPlayer::CreateCCRender (void)
{
	if (m_pCCRender != nil)
		return VOOSMP_ERR_None;
	
#ifdef _IOS
    m_pCCRender = [[voCCManager alloc] init:this pQuery:ProcessCCRenderQuery];
#endif
	
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

int	voOSNPPlayer::ProcessCCRenderQuery(void * pUserData, int nQT, void * pParam1, void * pParam2)
{
    if (pUserData)
	{
		voOSNPPlayer* pPlayer = (voOSNPPlayer*)pUserData;
		int nRet =  pPlayer->doProcessCCRenderQuery(nQT, pParam1, pParam2);
		
		return nRet;
	}
	
	return 0;
}

int	voOSNPPlayer::doProcessCCRenderQuery(int nQT, void * pParam1, void * pParam2)
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

int	voOSNPPlayer::HandleEvent (int nID, void * pParam1, void * pParam2)
{
	if (NULL != m_pOSMPLListener && m_pOSMPLListener->pUserData != NULL && m_pOSMPLListener->pListener != NULL)
	{
        if (VONP_CB_PlayerReadyRun == nID) {
            
            if (m_pPlayerViewOld) {
                
                m_cVideoRender.SetSecondView(NULL);
                
                [m_pPlayerViewOld removeFromSuperview];
                [m_pPlayerViewOld release];
                m_pPlayerViewOld = nil;
            }
            
            if(m_pCCRender)
            {
                [(voCCManager*)m_pCCRender voStart];
            }

            return VONP_ERR_None;
        }
        
        switch (nID) {
            case VOOSMP_CB_VideoAspectRatio:
            {
                if (NULL == pParam1) {
                    return VOOSMP_ERR_Pointer;
                }
                
                VOLOGI("handle VOOSMP_CB_VideoAspectRatio:%d", *(int *)pParam1);
                
                m_pData->m_nAspectRatio = *(int *)pParam1;
                
                UpdateDispType();
            }
                break;
            case VOOSMP_CB_VideoSizeChanged:
            {
                if (NULL != pParam1 && NULL != pParam2) {
                    m_cVideoRender.SetVideoInfo(*((int *)pParam1), *((int *)pParam2));
                    UpdateCCRect();
                }
            }
                break;
                
            case VOOSMP_CB_Metadata_Arrive:
            {
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
                break;
            case VOOSMP_CB_MediaTypeChanged:
            {
                if ((NULL != pParam1) && (m_pCCRender != nil)) {
                    
                    if ((VOOSMP_AVAILABLE_AUDIOVIDEO == *(int *)pParam1)
                        || (VOOSMP_AVAILABLE_PUREVIDEO == *(int *)pParam1)) {
                        
                        [(voCCManager *)m_pCCRender voFlushID3];
                    }
                }
            }
                break;
            default:
                break;
        }
        
		m_pOSMPLListener->pListener(m_pOSMPLListener->pUserData, nID, pParam1, pParam2);
	}
    
    return COMXALPlayer::HandleEvent(nID, pParam1, pParam2);
}
