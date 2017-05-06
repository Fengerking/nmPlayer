#include "COSWinVideoRender.h"
#include "CGDIVideoRender.h"
#include "CDDVideoRender.h"
#include "CDCVideoRender.h"
#include "CCCRRRRender.h"

#define LOG_TAG "COSWinVideoRender"
#include "voLog.h"

COSWinVideoRender::COSWinVideoRender(COSWinVomePlayer * pEngine, HWND hWnd)
	: m_pEngine(pEngine)
	, m_hWnd(hWnd)
	, m_pVideoRender(NULL)
	, m_pRenderThread(NULL)
	, m_pWorkpath(NULL)
	, m_nColorType(VOOSMP_COLOR_RGB565_PACKED)
	, m_nStatus(0)
	, m_bSeeking(false)
	, m_nRenderNum(0)
	, mbUpsideDown(VO_FALSE)
	, m_sAspect(VO_RATIO_00)
	, m_nVideoRenderType(VOOSMP_RENDER_TYPE_DDRAW)
	, m_bVideoRenderCallback(VO_FALSE)
	, m_pVideoBuffer(NULL)
	, m_bWindowlessMode(false)
	, m_bDDrawFailed(false)
	, m_hWDC(NULL)
{
	memset(&m_sVideoFormat, 0, sizeof (VOOSMP_VIDEO_FORMAT));
	memset(&m_rcDraw, 0, sizeof(VOMP_RECT));
	memset(&m_VideoBuffer, 0, sizeof(VOMP_BUFFERTYPE));
}

COSWinVideoRender::~COSWinVideoRender()
{
	Stop();

	while (m_pRenderThread != NULL)
		Sleep (10);	

	if(m_pVideoRender)
		delete m_pVideoRender;
}

int	COSWinVideoRender::Start()
{
	if (m_bSeeking)
		m_bSeeking = false;

	if (m_nStatus == 1)
		return 0;

	m_nStatus = 1;
	m_nRenderNum = 0;

	if(m_pRenderThread == NULL)
	{
		VO_U32 ThdID;
	
		voThreadCreate (&m_pRenderThread, &ThdID, (voThreadProc)VideoRenderThreadProc, this, 0);
	}

	if (m_pVideoRender)
		m_pVideoRender->Start();

	return 0;
}

int COSWinVideoRender::Stop()
{
	m_nStatus = 0;

	{
		voCAutoLock lock( &m_RenderLock );
        voCAutoLock lockDraw( &m_RenderDrawLock );
		if(m_pVideoRender)
			m_pVideoRender->Stop();
	}

	// here we must unlock m_RenderLock, otherwise m_pRenderThread can't exit since VideoRenderThreadLoop, East 20130317
	while (m_pRenderThread != NULL)
		Sleep (10);

	{
		voCAutoLock lock( &m_RenderLock );
        voCAutoLock lockDraw( &m_RenderDrawLock );

		if(m_pVideoRender)
		{
			delete m_pVideoRender;
			m_pVideoRender = NULL;
		}
	}
	m_bDDrawFailed = false;

	return 	0;
}

int COSWinVideoRender::Pause()
{
	m_nStatus = 2;
	if (m_pVideoRender)
		m_pVideoRender->Pause();
	return 0;
}

int	COSWinVideoRender::SetRect(VOMP_RECT * pRect)
{
	voCAutoLock lock( &m_RenderLock );
	if(pRect == NULL)
	{
		return 0;
	}

	VOLOGI("SetRect L %d T %d R %d B %d", pRect->left, pRect->top, pRect->right, pRect->bottom);

	m_rcDraw.bottom = pRect->bottom;
	m_rcDraw.left = pRect->left;
	m_rcDraw.right = pRect->right;
	m_rcDraw.top = pRect->top;

	if(m_pVideoRender != NULL)
		m_pVideoRender->SetDispRect (m_hWnd, (VO_RECT *)&m_rcDraw);

	return 0;
}

int	COSWinVideoRender::SetPos(int nPos)
{
	if (m_nStatus == 2)
	{
		m_bSeeking = true;			
		m_nStatus = 1;
	}

	return 0;
}

int	 COSWinVideoRender::SetUpsideDown(VO_BOOL bValue)
{
	mbUpsideDown = bValue;
	voCAutoLock lock( &m_RenderLock );
	if(m_pVideoRender != NULL)
	{
		m_pVideoRender->SetParam(VO_PID_VIDEO_UPSIDEDOWN , &mbUpsideDown);
	}

	return 0;
}

int	 COSWinVideoRender::SetWorkPath(VO_TCHAR * pWorkpath)
{
    m_pWorkpath = pWorkpath;
    voCAutoLock lock( &m_RenderLock ); 
	if(m_pVideoRender != NULL)
    {
        m_pVideoRender->SetWorkPath(pWorkpath);
    }

    return 0;
}

int COSWinVideoRender::SetView(void * pView)
{
	if(pView == NULL)
		return 0;

	voCAutoLock lock( &m_RenderLock );
	voCAutoLock lockDraw( &m_RenderDrawLock );

	if(VO_FALSE == InitVideoRenderer())
	{
		UninitVideoRenderer();
		return VOOSMP_ERR_Unknown;
	}
	
	m_pVideoRender->SetDispRect(m_hWnd, (VO_RECT *)&m_rcDraw);
	m_pEngine->GetParam(VOOSMP_PID_VIDEO_FORMAT, &m_sVideoFormat);
	int nRC = m_pVideoRender->SetVideoInfo(m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
	if(nRC != VOOSMP_ERR_None)
		m_pEngine->HandleEvent(VOOSMP_CB_CodecNotSupport, NULL, NULL);

	m_pVideoRender->SetDispType(VO_ZM_MAX, (VO_IV_ASPECT_RATIO)m_sAspect);

	return nRC;
}

void  COSWinVideoRender::ShowOverlay(VO_BOOL bValue)
{
	voCAutoLock lock( &m_RenderLock );
	if(m_pVideoRender != NULL)
	{
		m_pVideoRender->ShowOverlay(bValue);
	}
}

int	COSWinVideoRender::SetAspectRatio(VOOSMP_ASPECT_RATIO * pAspect)
{
	voCAutoLock lock( &m_RenderLock );
	if(pAspect)
	{
		m_sAspect = *(VO_IV_ASPECT_RATIO  *)pAspect;

		if(m_pVideoRender != NULL)
			return m_pVideoRender->SetDispType(VO_ZM_MAX , *(VO_IV_ASPECT_RATIO  *)pAspect);
	}

	return VOOSMP_ERR_Implement;
}


int COSWinVideoRender::VideoRenderThreadLoop(int decoder_or_render)
{
	if(m_bDDrawFailed == true && m_nVideoRenderType == VOOSMP_RENDER_TYPE_DDRAW)
	{
		VOLOGW("DDraw can't work!!");
		voOS_Sleep(2);

		return VOOSMP_ERR_None;
	}

	if(TRY_TO_HOLD_VIDEO_BUF == decoder_or_render)
	{
		m_pVideoBuffer = NULL;
		int nRC = 0;
		{
			voCAutoLock lock( &m_RenderLock );

			if(m_pVideoRender == NULL)
			{
				voCAutoLock lockDraw( &m_RenderDrawLock );

				if(VO_FALSE == InitVideoRenderer())
				{
					UninitVideoRenderer();
					return VOOSMP_ERR_Unknown;
				}
				m_pEngine->GetParam (VOOSMP_PID_VIDEO_FORMAT, &m_sVideoFormat);
				m_pVideoRender->SetVideoInfo (m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
				m_pVideoRender->SetDispRect (m_hWnd, (VO_RECT *)&m_rcDraw);
			}
		}
    
		nRC = m_pEngine->GetVideoBuffer(&m_pVideoBuffer);
      
		if ((m_sVideoFormat.Width == 0 || m_sVideoFormat.Height == 0) && (nRC == VOOSMP_ERR_None || nRC == VOOSMP_ERR_FormatChange))
		{
			internalReturnVideoBuffer();
			m_pEngine->GetParam (VOOSMP_PID_VIDEO_FORMAT, &m_sVideoFormat);
			{
				voCAutoLock lock( &m_RenderLock );
				nRC = m_pVideoRender->SetVideoInfo (m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
			}
			if(nRC != VOOSMP_ERR_None)
				m_pEngine->HandleEvent(VOOSMP_CB_CodecNotSupport , NULL , NULL);

			voCAutoLock lock( &m_RenderLock );
			m_pVideoRender->SetDispRect (m_hWnd, (VO_RECT *)&m_rcDraw);		
			return VOOSMP_ERR_FormatChange;
		}
    
		if (nRC == VOOSMP_ERR_Retry)
		{
			voOS_Sleep(2);
			return 0;
		}
		else if (nRC == VOOSMP_ERR_FormatChange)
		{
			// we need add lock here, otherwise handle event will crash since we delete video renderer, East 20130315
			voCAutoLock lock( &m_RenderLock );
			voCAutoLock lockDraw( &m_RenderDrawLock );

			if(VO_FALSE == InitVideoRenderer())
			{
				UninitVideoRenderer();
				internalReturnVideoBuffer();
				return VOOSMP_ERR_Unknown;
			}
			
			m_pVideoRender->SetDispRect (m_hWnd, (VO_RECT *)&m_rcDraw);
			m_pEngine->GetParam (VOOSMP_PID_VIDEO_FORMAT, &m_sVideoFormat);
			nRC = m_pVideoRender->SetVideoInfo (m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
			if(nRC != VOOSMP_ERR_None)
				m_pEngine->HandleEvent(VOOSMP_CB_CodecNotSupport, NULL, NULL);
			m_pVideoRender->SetDispType(VO_ZM_MAX , (VO_IV_ASPECT_RATIO)m_sAspect);
          
			if(m_pVideoBuffer == NULL)
				return 0;
		}
		else if (VOOSMP_ERR_WaitTime == nRC || VOOSMP_ERR_Retry == nRC)
		{
			// for VOOSMP_ERR_WaitTime and VOOSMP_ERR_Retry, we needn't call Sleep since voME2 will call Sleep
			// duplicate Sleep will make big deviation since it is not accurate in Windows platform
			return 0;
		}
		else if (nRC != VOOSMP_ERR_None)
		{
			voOS_Sleep(2);
			return 0;
		}
		else if (m_pVideoBuffer == NULL)
		{
			voOS_Sleep(2);
			return 0;
		}

		if(nRC == VOOSMP_ERR_None)
		{
			if(m_nRenderNum == 0)
			{
				m_pEngine->GetParam (VOOSMP_PID_VIDEO_FORMAT, &m_sVideoFormat);
				voCAutoLock lock( &m_RenderLock );
				m_pVideoRender->SetVideoInfo (m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
				m_pVideoRender->SetDispRect (m_hWnd, (VO_RECT *)&m_rcDraw);
			}
        
			m_nRenderNum++;
		}

		if(VOOSMP_RENDER_TYPE_DC == m_nVideoRenderType && true == m_bWindowlessMode)
			m_pEngine->HandleEvent(VOMP_CB_VideoReadyToRender, NULL, NULL);

		voCAutoLock lock( &m_RenderLock );
		if (m_pVideoRender != NULL && m_nRenderNum > 1)
		{
			if (m_pVideoBuffer && m_pVideoBuffer->nSize == sizeof (VO_VIDEO_BUFFER)) {
				m_pVideoRender->Render ((VO_VIDEO_BUFFER *)m_pVideoBuffer->pBuffer, m_pVideoBuffer->llTime, VO_TRUE);
				internalReturnVideoBuffer(1);
			}
			if(VOOSMP_RENDER_TYPE_DDRAW != m_nVideoRenderType && false == m_bWindowlessMode && NULL != m_pVideoRender)
				m_pVideoRender->Redraw();
		} else {
			internalReturnVideoBuffer();
		}
			
	}

	return 0;
}

int	COSWinVideoRender::Redraw()
{
    voCAutoLock lock( &m_RenderDrawLock );

	int nRet = 0;
    if (NULL != m_pVideoRender)
	{
		nRet = m_pVideoRender->Redraw();

        if (VO_ERR_RETRY == nRet)
		{
			if(VOOSMP_RENDER_TYPE_DC == m_nVideoRenderType && true == m_bWindowlessMode)
            { 
                m_pEngine->HandleEvent(VOMP_CB_VideoReadyToRender , NULL , NULL); 
            }
        }
    }
	else
	{
		nRet = -1;
	}
	if(0 == m_nStatus)
		nRet = -1;

    return nRet;
}

int	COSWinVideoRender::VideoRenderThreadProc(void * pParam)
{
  if(NULL == pParam)
    return -1;
	COSWinVideoRender * pRender = (COSWinVideoRender*)pParam;

	while (pRender->m_nStatus == 1 || pRender->m_nStatus == 2)
	{
		if (pRender->m_nStatus == 2)
		{
			voOS_Sleep(2);
			continue;
		}

		pRender->VideoRenderThreadLoop (TRY_TO_HOLD_VIDEO_BUF);
	}

	pRender->m_pRenderThread = NULL;

	return 0;
}

int COSWinVideoRender::SetParam(int nID, void * pParam)
{
	if(NULL == pParam)
		return VOOSMP_ERR_Pointer;

	if(VOOSMP_PID_VIDEO_RENDER_TYPE == nID)
	{
		VOOSMP_RENDER_TYPE eNewRenderType = *(VOOSMP_RENDER_TYPE *)pParam;
		VOLOGI("VOOSMP_PID_VIDEO_RENDER_TYPE old %d new %d", m_nVideoRenderType, eNewRenderType);

		// if renderer type keep same, just return OK
		if(eNewRenderType == m_nVideoRenderType && (m_bWindowlessMode || (!m_bDDrawFailed && m_pVideoRender)))
		{
			VOLOGI("Render type same %d, windowless %d, ddraw failed %d, m_pVideoRender 0x%08X", m_nVideoRenderType, m_bWindowlessMode, m_bDDrawFailed, m_pVideoRender);
			return VOOSMP_ERR_None;
		}

		m_nVideoRenderType = eNewRenderType;
		bool bChangeDynamically = false;

		{
			// change video renderer
			voCAutoLock lock( &m_RenderLock );
			voCAutoLock lockDraw( &m_RenderDrawLock );

			VOLOGI("VOOSMP_PID_VIDEO_RENDER_TYPE m_pVideoRender 0x%08X", m_pVideoRender);

			if(m_pVideoRender || false == m_bWindowlessMode)
			{
				bChangeDynamically = true;
				if(VO_FALSE == InitVideoRenderer())
				{
					UninitVideoRenderer();
					return VOOSMP_ERR_Unknown;
				}

				m_pVideoRender->SetBlockEraseBackGround(VO_TRUE);

				m_pVideoRender->SetDispRect(m_hWnd, (VO_RECT *)&m_rcDraw);
				m_pEngine->GetParam(VOOSMP_PID_VIDEO_FORMAT, &m_sVideoFormat);
				int nRC = m_pVideoRender->SetVideoInfo(m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
				if(nRC != VOOSMP_ERR_None)
					m_pEngine->HandleEvent(VOOSMP_CB_CodecNotSupport, NULL, NULL);
				m_pVideoRender->SetDispType(VO_ZM_MAX, (VO_IV_ASPECT_RATIO)m_sAspect);
			}
		}

		if(VOOSMP_RENDER_TYPE_DC == eNewRenderType && bChangeDynamically)
		{
			// if DC renderer, we need wait data ready
			int i = 0;
			for(i = 0; i < 500; i++)
			{
				VOOSMP_STATUS eStatus;
				int nRC = m_pEngine->GetStatus((int *)&eStatus);
				if(VOOSMP_ERR_None == nRC && VOOSMP_STATUS_PAUSED == eStatus)
				{
					m_pEngine->SetParam(VOOSMP_PID_VIDEO_REDRAW, NULL);
				}
				
				if(VOOSMP_ERR_None != nRC || VOOSMP_STATUS_RUNNING != eStatus)
				{
					break;
				}

				if(VO_TRUE == m_pVideoRender->CanRedraw())
					break;

				voOS_Sleep(2);
			}
		}

		return 0;
	}
	else if(VOOSMP_PID_VIDEO_DC == nID)
	{
		m_hWDC = (HDC)pParam;

		voCAutoLock lockDraw( &m_RenderDrawLock );

		if(m_pVideoRender)
			return m_pVideoRender->SetParam(nID, pParam);
		else
		{
			VOLOGE("VOOSMP_PID_VIDEO_DC VOOSMP_ERR_Pointer");
			return VOOSMP_ERR_Pointer;
		}
	}
	else if(VOOSMP_PID_VR_USERCALLBACK == nID)
	{
		m_bVideoRenderCallback = *(VO_BOOL *)pParam;
		if(VOOSMP_RENDER_TYPE_DDRAW == m_nVideoRenderType && NULL != m_pVideoRender)
		{
			CDDVideoRender * pDDVideoRender = (CDDVideoRender *)m_pVideoRender;
			if(VO_TRUE == m_bVideoRenderCallback)
				pDDVideoRender->SetDDrawCallback(OnDDrawRenderCallback, this);
			else
				pDDVideoRender->SetDDrawCallback(NULL, NULL);
		}

		return 0;
	}
	else if(VOOSMP_PID_DRAW_VIDEO_DIRECTLY == nID)
	{
		if(NULL != pParam)
			m_bWindowlessMode = *(bool*)pParam;
	}

	return 0;
}

BOOL COSWinVideoRender::InitVideoRenderer()
{
	// uninit previous video renderer
	UninitVideoRenderer();

	if(VOOSMP_RENDER_TYPE_DDRAW == m_nVideoRenderType)
	{
		m_pVideoRender = new CDDVideoRender(NULL, m_hWnd, NULL);
		CDDVideoRender * pDDVideoRender = (CDDVideoRender *)m_pVideoRender;
		if(VO_TRUE == m_bVideoRenderCallback)
			pDDVideoRender->SetDDrawCallback(OnDDrawRenderCallback, this);
		else
			pDDVideoRender->SetDDrawCallback(NULL, NULL);

		if(NULL != pDDVideoRender)
		{
			m_pVideoRender->SetWorkPath(m_pWorkpath);
			if(NULL != m_pEngine)
				m_pEngine->GetParam (VOOSMP_PID_VIDEO_FORMAT, &m_sVideoFormat);

			int nRC = m_pVideoRender->SetVideoInfo(m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
			if(nRC != VOOSMP_ERR_None)
				m_pEngine->HandleEvent(VOOSMP_CB_CodecNotSupport, NULL, NULL);

			// David @ 2013/08/16
			// SetDispType should be done before SetDispRect, 
			// otherwise the paused video would be compressed when seeking
			m_pVideoRender->SetDispType(VO_ZM_MAX, (VO_IV_ASPECT_RATIO)m_sAspect);
			m_pVideoRender->SetDispRect (m_hWnd, (VO_RECT *)&m_rcDraw);
			if(false == pDDVideoRender->CreateDD())
			{
				// david @ 2013/12/11
				// notify this failure to App
				// if a valid region is input
				int w = m_rcDraw.right - m_rcDraw.left;
				int h = m_rcDraw.bottom - m_rcDraw.top;
				if (w * h > 0) 
					m_pEngine->HandleEvent(VOOSMP_CB_VideoRenderCreateFail, NULL, NULL);
				
				VOLOGI("Create DDraw failed");
				m_bDDrawFailed = true;
				return VO_FALSE;
			}
			else
			{
				m_bDDrawFailed = false;
			}
		}
		else
		{
			m_bDDrawFailed = false;
		}
	}
	else if(VOOSMP_RENDER_TYPE_GDI == m_nVideoRenderType)
#ifdef WINCE
		m_pVideoRender = new CCCRRRRender(NULL, m_hWnd, NULL);
#else
		m_pVideoRender = new CGDIVideoRender(NULL, m_hWnd, NULL);
#endif //WINCE
	else if(VOOSMP_RENDER_TYPE_DC == m_nVideoRenderType)
		m_pVideoRender = new CDCVideoRender(NULL, m_hWnd, NULL);

	if(!m_pVideoRender)
		return FALSE;

	if(false == m_bWindowlessMode && NULL != m_pVideoRender)
	{
		m_pVideoRender->SetParam(VOOSMP_PID_VIDEO_DC, (void*)m_hWDC);
	}
	m_pVideoRender->SetWorkPath(m_pWorkpath);
	return TRUE;
}

void COSWinVideoRender::UninitVideoRenderer()
{
	if(m_pVideoRender)
	{
		delete m_pVideoRender;
		m_pVideoRender = NULL;
	}
}

VO_S32 COSWinVideoRender::OnDDrawRenderCallback(VO_PTR pUserData, HDC hDC, LPRECT pRect, VO_S64 llVideoTime)
{
	COSWinVideoRender * pVR = (COSWinVideoRender *)pUserData;

	VOOSMP_VR_USERCALLBACK_TYPE sVRUserCallbackType;
	sVRUserCallbackType.pDC = (void *)hDC;
	sVRUserCallbackType.pRect = (void *)pRect;
	sVRUserCallbackType.llVideoTime = llVideoTime;
	return (VO_S32)pVR->m_pEngine->HandleEvent(VOOSMP_CB_VR_USERCALLBACK, (void *)&sVRUserCallbackType, NULL);
}

void COSWinVideoRender::internalReturnVideoBuffer(const int rendered) {

	if (m_pEngine == NULL)
		return;
	
	if (m_pVideoRender) {
		VOMP_SIGNALBUFFERRETURNED br;
		br.nRendered = rendered;
		br.nTrackType = VOMP_SS_Video;
		br.pBuffer = m_pVideoBuffer;
		m_pEngine->SetParam(VOMP_PID_SIGNAL_BUFFER_RETURNED, (void*)&br);
	}
}
