
/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2011				*
*																		*
************************************************************************/
/*******************************************************************************
	File:		CVideoRenderManager.cpp

	Contains:	CVideoRenderManager class file

	Written by:	Jim Lin 

	Change History (most recent first):
	2011-11-16		Jim			Create file

*******************************************************************************/

#include "CVideoRenderManager.h"

#ifdef _IOS
#include "CiOSVideoRender.h"
#else
#include "CMacOSVideoRender.h"
#endif

#include "voSystemStatus.h"
#include "voOSFunc.h"

#define LOG_TAG "CVideoRenderManager"
#include "voLog.h"


CVideoRenderManager::CVideoRenderManager(void* pUserData, RenderQueryCallback pQuery)
:voBaseRender (pUserData, pQuery)
,m_nColorType (VOMP_COLOR_RGB565_PACKED)
,m_nBytesPixel (0)
,m_nStride (0)
,m_nZoomMode(VO_ZM_LETTERBOX)
,m_nAspectRatio(VO_RATIO_00)
,m_bSetPos (false)
,m_bSurfaceNeedChange( true )
,m_pVideoRender(NULL)
,m_pSurface(NULL)
{
	strcpy (m_szThreadName, "CVideoRenderManager");
	memset (&m_sVideoFormat, 0, sizeof (VOMP_VIDEO_FORMAT));
	m_sVideoFormat.Width	= 480;
	m_sVideoFormat.Height	= 320;

	memset(&m_rcDraw, 0, sizeof(VOMP_RECT));
}

CVideoRenderManager::~CVideoRenderManager()
{
	if(m_pVideoRender)
	{
		m_pVideoRender->Stop();
		delete m_pVideoRender;
		m_pVideoRender = NULL;
	}
}

int CVideoRenderManager::SetSurface (void* hWnd)
{
	m_pSurface = hWnd;
    
	if (NULL != m_pVideoRender) {
		m_pVideoRender->SetDispRect (m_pSurface, (VO_RECT *)&m_rcDraw, (VO_IV_COLORTYPE)m_nColorType);
	}

	return 0;
}

int	CVideoRenderManager::Start (void)
{
	if (m_bSetPos)
		m_bSetPos = false;

    if (NULL != m_pVideoRender) {
        m_pVideoRender->Start();
    }
	
    voBaseRender::Start ();
    
	return 0;
}

void CVideoRenderManager::UpdateRect()
{
	if (NULL != m_pVideoRender) {
		VO_RECT r;
		r.left=m_rcDraw.left;r.top=m_rcDraw.top;r.right=m_rcDraw.right;r.bottom=m_rcDraw.bottom;
		m_pVideoRender->SetDispRect (m_pSurface, &r, (VO_IV_COLORTYPE)m_nColorType);
		m_pVideoRender->SetVideoInfo (m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
        m_pVideoRender->SetDispType((VO_IV_ZOOM_MODE)m_nZoomMode, (VO_IV_ASPECT_RATIO)m_nAspectRatio);
        m_pVideoRender->Start();
        
		VOLOGI("%ld, %ld, Draw context %x, output color %d, width %d, height %d", r.right, r.bottom, (int)m_pSurface, m_nColorType, m_sVideoFormat.Width, m_sVideoFormat.Height);	
	}
}

int CVideoRenderManager::Stop ()
{
    int nStatus = m_bThreadStatus;
    
	voBaseRender::Stop (true);
    
	if (m_pVideoRender && (VO_THREAD_STOPPED != nStatus))
	{
		m_pVideoRender->Stop();
	}
	
    memset (&m_sVideoFormat, 0, sizeof (VOMP_VIDEO_FORMAT));
    m_bSurfaceNeedChange = true;
    
	return 0;
}

int CVideoRenderManager::SetPos (int nPos)
{
	if (m_bThreadStatus == VO_THREAD_PAUSED)
	{
		m_bSetPos = true;
		m_bThreadStatus = VO_THREAD_RUNNING;
	}

	return 0;
}

int CVideoRenderManager::SetColorType (VOMP_COLORTYPE nColor)
{
    m_nColorType = nColor;
    
    if (NULL != m_pVideoRender) {
		m_pVideoRender->SetDispRect (m_pSurface, (VO_RECT *)&m_rcDraw, (VO_IV_COLORTYPE)m_nColorType);
	}
    
	return 0;
}

VO_S32 CVideoRenderManager::OnListener (void * pUserData, int nID, void * pParam1, void * pParam2)
{
	CVideoRenderManager * pPlayer = (CVideoRenderManager *)pUserData;
    
    if (NULL == pPlayer) {
        return VO_ERR_FAILED;
    }
    
	return pPlayer->HandleEvent (nID, pParam1, pParam2);
}

int	CVideoRenderManager::HandleEvent (int nID, void * pParam1, void * pParam2)
{
    if (NULL == m_pQuery) {
        return VO_ERR_FAILED;
    }
    
    if (VO_VR_EVENT_VIDEO_DISPLAY_RECT_CHANGED == nID) {
        m_pQuery(m_pUserData, QT_NOTIFY_VIDEO_DISPLAY_CHANGE , pParam1, NULL);
    }
    
    return VO_ERR_NONE;
}

int CVideoRenderManager::RenderThreadLoop (void)
{
    if (!voSystemStatus::Instance()->IsAPPActive())
    {
        voOS_Sleep(5);
        return VOMP_ERR_Status;
    }
    
	if (NULL == m_pVideoRender)
	{
#ifdef _IOS
		m_pVideoRender = new CiOSVideoRender(NULL, m_pSurface, NULL);
#else
		m_pVideoRender = new CMacOSVideoRender(NULL, m_pSurface, NULL);
#endif
        m_pVideoRender->SetWorkPath(m_pWorkPath);
        m_pVideoRender->SetEventCallBack(OnListener, this);
        
        UpdateRect();
	}
 
    if (m_pSurface == NULL)
	{
		voOS_Sleep(5);
		return VOMP_ERR_Retry;
	}
    
	int nRC = VOMP_ERR_None;

	if((m_sVideoFormat.Width == 0 && m_sVideoFormat.Height == 0) || m_bSurfaceNeedChange)
	{
		memset (&m_sVideoFormat, 0, sizeof (VOMP_VIDEO_FORMAT));
		if(m_pQuery)
			nRC = m_pQuery(m_pUserData, QT_GET_VIDEO_FMT, &m_sVideoFormat, NULL);
		
		if (nRC != VOMP_ERR_None)
		{
			voOS_Sleep(5);
			return nRC;
		}
		
		//m_sVideoFormat.Width = (m_sVideoFormat.Width>>5)<<5;
	}

	VOMP_BUFFERTYPE *pVideoBuffer = NULL;
	
	nRC = VOMP_ERR_Retry;
    
	while (nRC != VOMP_ERR_None)
	{
		pVideoBuffer = NULL;
		
		//VOLOGI("+Read video buffer...");
		if(m_pQuery)
			nRC = m_pQuery(m_pUserData, QT_GET_VIDEO_BUFFER, &pVideoBuffer, NULL);
		//VOLOGI("-Read video buffer...");
		
		if (nRC == VOMP_ERR_None)
		{
			break;
		}
		else if( nRC == VOMP_ERR_FormatChange )
		{
			VOLOGI("Read video buffer VOMP_ERR_FormatChange...");
			if(m_pQuery)
				nRC = m_pQuery(m_pUserData, QT_GET_VIDEO_FMT , &m_sVideoFormat, NULL);
			
			if (nRC != VOMP_ERR_None)
			{
                voOS_Sleep(5);
				return nRC;
			}
			m_bSurfaceNeedChange = true;
			break;
		}
		else if (nRC == VOMP_ERR_Retry)
		{
			voOS_Sleep(5);
		}
		else if (nRC == VOMP_ERR_WaitTime)
		{
            voOS_Sleep(2);
		}
		else
		{
			voOS_Sleep(5);
		}
		
		if (m_bThreadStatus != VO_THREAD_RUNNING)
			break;
	}

    int rendered = 0;
	if ((m_pVideoRender != NULL)
		&& (VO_THREAD_RUNNING == m_bThreadStatus))
	{
		if (m_bSurfaceNeedChange)
		{
			m_bSurfaceNeedChange = false;
			UpdateRect();
            if (m_pQuery) {
                m_pQuery(m_pUserData, QT_NOTIFY_VIDEO_FORMAT_CHANGE , NULL, NULL);
            }
		}
		if ((NULL != pVideoBuffer) && (NULL != pVideoBuffer->pBuffer))
		{
			VO_VIDEO_BUFFER* pBuf = (VO_VIDEO_BUFFER*)pVideoBuffer->pBuffer;
			
			if(pBuf)
			{
				int ret = m_pVideoRender->Render (pBuf, pBuf->Time, VO_TRUE);
                
                while ((VO_ERR_RETRY == ret) && (m_bThreadStatus == VO_THREAD_RUNNING)) {
                    voOS_Sleep(5);
                    ret = m_pVideoRender->Render (pBuf, pBuf->Time, VO_TRUE);
                    if (VO_ERR_NONE == ret) {
                        rendered = 1;
                    }
                }
				
				if (m_bSetPos)
				{
					m_bThreadStatus = VO_THREAD_PAUSED;
					m_bSetPos = false;
				}
			}
		}
	}
    
    if (pVideoBuffer) {
        VOMP_SIGNALBUFFERRETURNED br;
        br.nRendered = rendered;
        br.nTrackType = VOMP_SS_Video;
        br.pBuffer = pVideoBuffer;
        
        if (m_pQuery) {
            nRC = m_pQuery(m_pUserData, QT_RETURN_VIDEO_BUFFER , &br, NULL);
        }
    }

	return nRC;
}

void CVideoRenderManager::VideoSizeChanged()
{
}

void CVideoRenderManager::DrawBlack( VOMP_BUFFERTYPE * pVideoBuffer )
{
    if (NULL == pVideoBuffer) {
        return;
    }
    
	memset( pVideoBuffer->pBuffer , 0 , pVideoBuffer->nSize );
}

int CVideoRenderManager::SetDrawRect(int nLeft, int nTop, int nRight, int nBottom)
{
	m_rcDraw.left	= nLeft;
	m_rcDraw.top	= nTop;
	m_rcDraw.right	= nRight;
	m_rcDraw.bottom	= nBottom;
	
	UpdateRect();
	
	return 0;
}

int CVideoRenderManager::GetDrawRect(VO_RECT *pValue)
{
    if (NULL != m_pVideoRender) {
		return m_pVideoRender->GetParam(VO_VR_PMID_DrawRect, pValue);
	}
    
    return VO_ERR_WRONG_STATUS;
}

int CVideoRenderManager::SetDispType (VO_IV_ZOOM_MODE nZoomMode, VO_IV_ASPECT_RATIO nRatio)
{
    m_nZoomMode = nZoomMode;
    m_nAspectRatio = nRatio;
    
    if (NULL != m_pVideoRender) {
		return m_pVideoRender->SetDispType((VO_IV_ZOOM_MODE)m_nZoomMode, (VO_IV_ASPECT_RATIO)m_nAspectRatio);
	}
    
	return VO_ERR_WRONG_STATUS;
}

VO_U32 CVideoRenderManager::SetParam (VO_U32 nID, VO_PTR pValue)
{
    if (NULL != m_pVideoRender) {
		return m_pVideoRender->SetParam (nID, pValue);
	}
    
	return VO_ERR_WRONG_STATUS;
}

VO_U32 CVideoRenderManager::GetParam (VO_U32 nID, VO_PTR pValue)
{
    if (NULL != m_pVideoRender) {
		return m_pVideoRender->GetParam (nID, pValue);
	}
    
	return VO_ERR_WRONG_STATUS;
}
