#include "COSWinVomePlayer.h"
#include "COSWinAudioRender.h"
#include "COSWinVideoRender.h"

#define LOG_TAG "COSWinVomePlayer"
#include "voLog.h"

COSWinVomePlayer::COSWinVomePlayer()
	: m_pAudioRender (NULL)
	, m_pVideoRender (NULL)
	, m_hWnd(NULL)
	, m_nAspectRatio(VOOSMP_RATIO_AUTO)
	, m_nVideoRenderType(VOOSMP_RENDER_TYPE_DDRAW)
	, m_bVideoRenderCallback(VO_FALSE)
	, m_nVideoAspectRatio(VOOSMP_RATIO_MAX)
	, m_bWindowlessMode(false)
{
	voOS_SetTimePeriod(VO_TRUE, 1);
}

COSWinVomePlayer::~COSWinVomePlayer ()
{
	Uninit();
	voOS_SetTimePeriod(VO_FALSE, 1);
}

int	COSWinVomePlayer::Init()
{
	int nRC = COSVomePlayer::Init();
	
	if(nRC != VOOSMP_ERR_None) 
		return nRC;

	int nRndNum = 6;
	nRC = COSVomeEngine::SetParam (VOMP_PID_VIDEO_RNDNUM, &nRndNum);
	if (nRC != VOMP_ERR_None) return ReturnCode(nRC);

	int nPlayMode = VOMP_PULL_MODE;
	nRC = COSVomeEngine::SetParam(VOMP_PID_AUDIO_PLAYMODE, &nPlayMode);
	if (nRC != VOOSMP_ERR_None) return ReturnCode(nRC);
	nRC = COSVomeEngine::SetParam(VOMP_PID_VIDEO_PLAYMODE, &nPlayMode);

	// for windows platform, we forbid multi-core decoder for CPU conflict issue, East 20131004
//	int CPUNum = voOS_GetCPUNum();
//	nRC = SetParam (VOOSMP_PID_CPU_NUMBER, &CPUNum);
	return nRC;
}

int	COSWinVomePlayer::Uninit()
{
	if( m_pAudioRender != NULL )
	{
		delete 	m_pAudioRender;
		m_pAudioRender = NULL;
	}

	if( m_pVideoRender != NULL)
	{
		delete m_pVideoRender;
		m_pVideoRender = NULL;
	}

	return COSVomePlayer::Uninit();
}

int COSWinVomePlayer::SetDataSource (void * pSource, int nFlag)
{
	{
		voCAutoLock lock(&m_VideoLock);
		if(m_pAudioRender != NULL)
		{
			delete m_pAudioRender;
			m_pAudioRender =NULL;
		}

		if(m_pVideoRender != NULL)
		{
			delete m_pVideoRender;
			m_pVideoRender = NULL;
		}

		m_pAudioRender = new COSWinAudioRender(this);		
		m_pVideoRender = new COSWinVideoRender(this, (HWND)m_hWnd);

		m_pVideoRender->SetWorkPath(m_szPathLib);
		m_pVideoRender->SetParam(VOOSMP_PID_VIDEO_RENDER_TYPE, &m_nVideoRenderType);

		m_pVideoRender->SetRect(&m_rcDraw);
	}

	return COSVomePlayer::SetDataSource(pSource, nFlag);
}

int COSWinVomePlayer::Run (void)
{
	{
		voCAutoLock lock(&m_VideoLock);
		if(m_pAudioRender)
			m_pAudioRender->Start();

		if(m_pVideoRender)
		{
			m_pVideoRender->SetParam(VOOSMP_PID_VIDEO_RENDER_TYPE, &m_nVideoRenderType);
			m_pVideoRender->SetParam(VOOSMP_PID_VR_USERCALLBACK, &m_bVideoRenderCallback);
			m_pVideoRender->Start();
		}
	}

	return COSVomePlayer::Run();
}

int COSWinVomePlayer::Pause (void)
{
	{
		voCAutoLock lock(&m_VideoLock);
		if(m_pAudioRender)
			m_pAudioRender->Pause();

		if(m_pVideoRender)
			m_pVideoRender->Pause();
	}

	return COSVomePlayer::Pause();
}

int COSWinVomePlayer::Stop (void)
{
	{
		voCAutoLock lock(&m_VideoLock);
		if(m_pAudioRender)
			m_pAudioRender->Stop();

		if(m_pVideoRender)
			m_pVideoRender->Stop();
	}

	return COSVomePlayer::Stop();
}

int COSWinVomePlayer::Close (void)
{
	int nRC = Stop();


	voCAutoLock lock(&m_VideoLock);

	if(m_pAudioRender != NULL)
	{
		delete m_pAudioRender;
		m_pAudioRender =NULL;
	}

	if(m_pVideoRender != NULL)
	{
		delete m_pVideoRender;
		m_pVideoRender = NULL;
	}

	return nRC;
}

int COSWinVomePlayer::Flush (void)
{
	if(m_pAudioRender)
		m_pAudioRender->Flush();

	return COSVomePlayer::Flush();
}

int COSWinVomePlayer::SetPos (int nCurPos)
{
	if(m_pAudioRender)
		m_pAudioRender->Flush();

	if(m_pVideoRender)
		m_pVideoRender->SetPos(nCurPos);

	return COSVomePlayer::SetPos(nCurPos);
}

int COSWinVomePlayer::SetView(void* pView)
{
	voCAutoLock lock(&m_VideoLock);
	m_hWnd = pView;
	if(pView && m_pVideoRender)
	{
		m_pVideoRender->SetView(pView);
	}

	VOMP_STATUS mpStatus = VOMP_STATUS_INIT;
	GetStatus((int *)&mpStatus);
	if(mpStatus == VOMP_STATUS_RUNNING)
	{
		COSVomeEngine::SetParam(VOMP_PID_VIDEO_PLAY_BACK, pView);		
	}
	
	return VOOSMP_ERR_None;
}

int COSWinVomePlayer::SetParam (int nID, void * pValue)
{
	if(nID == VOOSMP_PID_DRAW_RECT)
	{
		voCAutoLock lock( &m_VideoLock );

		VOOSMP_RECT	* pRect = (VOOSMP_RECT *)pValue;
		m_rcDraw.bottom = pRect->nBottom;
		m_rcDraw.right  = pRect->nRight;
		m_rcDraw.top	= pRect->nTop;
		m_rcDraw.left	= pRect->nLeft;

		if(m_pVideoRender)
			m_pVideoRender->SetRect(&m_rcDraw);
	}
	else if(nID == VOOSMP_PID_VIDEO_OVERLAY_CONTROL)
	{
		voCAutoLock lock( &m_VideoLock );

		if(m_pVideoRender != NULL)
		{
			if(*(int *)pValue == 0)
				m_pVideoRender->ShowOverlay(VO_FALSE);
			else if(*(int *)pValue == 1)
				m_pVideoRender->ShowOverlay(VO_TRUE);
		}

		return VOOSMP_ERR_None;
	}
	else if(nID == VOOSMP_PID_VIDEO_ASPECT_RATIO)
	{
		voCAutoLock lock( &m_VideoLock );

		m_nAspectRatio = *((VOOSMP_ASPECT_RATIO *) pValue);

		if(m_pVideoRender != NULL)
		{
			if(VOOSMP_RATIO_AUTO == *(VOOSMP_ASPECT_RATIO *) pValue)
			{
				if(VOOSMP_RATIO_MAX != m_nVideoAspectRatio) 
				{
					m_nAspectRatio = m_nVideoAspectRatio;
					return m_pVideoRender->SetAspectRatio(&m_nVideoAspectRatio);
				}
				else
				{
					VOOSMP_ASPECT_RATIO tempAspectRatio = VOOSMP_RATIO_00;
					m_nAspectRatio = VOOSMP_RATIO_00;
					return m_pVideoRender->SetAspectRatio(&tempAspectRatio);
				}
			}
			return m_pVideoRender->SetAspectRatio((VOOSMP_ASPECT_RATIO *) pValue);
		}

		return VOOSMP_ERR_Implement;
	}
  	else if(VOOSMP_PID_VIDEO_REDRAW == nID)
  	{
		voCAutoLock lock( &m_VideoLock );
		if(NULL == m_pVideoRender)
		{
			return VOOSMP_ERR_Pointer;
		}
		int nRC = VOOSMP_ERR_None;

		if(VOOSMP_RENDER_TYPE_DC != m_nVideoRenderType || NULL == pValue)
		{
			if(NULL != m_pVideoRender)
				nRC = m_pVideoRender->Redraw();
		}
		else
		{
			if(NULL != m_pVideoRender)
			{
                // Jeff: separate interface
				if(VO_TRUE == *(VO_BOOL*)pValue)
					nRC = m_pVideoRender->Redraw();
			}
		}

		return nRC;
	}
	else if(VOOSMP_PID_VIDEO_DC == nID)
	{
		voCAutoLock lock( &m_VideoLock );

		if(NULL != m_pVideoRender)
			m_pVideoRender->SetParam(nID, pValue);

		return VOOSMP_ERR_None;
	}
	else if(VOOSMP_PID_VIDEO_RENDER_TYPE == nID)
	{
		if(NULL != pValue)
		{
			int nRC = VOOSMP_ERR_None;
			voCAutoLock lock( &m_VideoLock );

			m_nVideoRenderType = *(VOOSMP_RENDER_TYPE*)pValue;

			if(NULL != m_pVideoRender)
				nRC = m_pVideoRender->SetParam(nID, pValue);

			return nRC;
		}

		return VOOSMP_ERR_Pointer;
	}
	else if(VOOSMP_PID_VR_USERCALLBACK == nID)
	{
		voCAutoLock lock( &m_VideoLock );

		m_bVideoRenderCallback = *(VO_BOOL *)pValue;
		VOLOGI("VOOSMP_PID_VR_USERCALLBACK %d", m_bVideoRenderCallback);

		if(m_pVideoRender)
			m_pVideoRender->SetParam(VOOSMP_PID_VR_USERCALLBACK, &m_bVideoRenderCallback);

		return VOOSMP_ERR_None;
	}
	else if(VOOSMP_PID_DRAW_VIDEO_DIRECTLY == nID)
	{
		if(NULL != pValue)
		{
			m_bWindowlessMode = *(bool*)pValue;
			if(m_pVideoRender)
				m_pVideoRender->SetParam(nID, pValue);
		}
		return VOOSMP_ERR_None;
	}

	return COSVomePlayer::SetParam(nID, pValue);
}

int	COSWinVomePlayer::HandleEvent (int nID, void * pParam1, void * pParam2)
{
	if((nID == VOOSMP_CB_MediaTypeChanged) && pParam1)
	{
		voCAutoLock lock( &m_VideoLock );
		if(*(int *)pParam1 == VOMP_AVAILABLE_PUREAUDIO)
		{
			if(m_pVideoRender != NULL)
			{
				m_pVideoRender->Stop();
				delete m_pVideoRender;
				m_pVideoRender = NULL;
			}

			if(m_pAudioRender == NULL)
			{
				m_pAudioRender = new COSWinAudioRender(this);

				if (m_pAudioRender == NULL)
					return VOOSMP_ERR_OutMemory;

				m_pAudioRender->Start();
			}
			else
			{
				m_pAudioRender->Start();
			}
		}
		else if(*(int *)pParam1 == VOMP_AVAILABLE_PUREVIDEO)
		{
			if(m_pAudioRender != NULL)
			{
				m_pAudioRender->Pause();
			}
			
			if(m_pVideoRender == NULL)
			{
				m_pVideoRender = new COSWinVideoRender(this, (HWND)m_hWnd);

				if (m_pVideoRender == NULL)
					return VOOSMP_ERR_OutMemory;

				m_pVideoRender->SetWorkPath(m_szPathLib);
				m_pVideoRender->SetRect(&m_rcDraw);
				m_pVideoRender->SetParam(VOOSMP_PID_VIDEO_RENDER_TYPE, &m_nVideoRenderType);
				m_pVideoRender->InitVideoRenderer();
				// David @ 2013/08/31
				// only if videoAspectRatio is valid
				if(VOOSMP_RATIO_MAX != m_nVideoAspectRatio)
					m_pVideoRender->SetAspectRatio((VOOSMP_ASPECT_RATIO *) &m_nVideoAspectRatio);
				else {

					// if no default video aspect been set, just use ratio_00, other wise video will disappear 
					if (m_nAspectRatio == VOOSMP_RATIO_AUTO)
						m_nAspectRatio = VOOSMP_RATIO_00;
					m_pVideoRender->SetAspectRatio((VOOSMP_ASPECT_RATIO *) &m_nAspectRatio);
				}
				m_pVideoRender->SetParam(VOOSMP_PID_DRAW_VIDEO_DIRECTLY, &m_bWindowlessMode);
				m_pVideoRender->SetParam(VOOSMP_PID_VR_USERCALLBACK, &m_bVideoRenderCallback);
				m_pVideoRender->Start();
			}
		}
		else if(*(int *)pParam1 == VOMP_AVAILABLE_AUDIOVIDEO)
		{
			if(m_pAudioRender == NULL)
			{
				m_pAudioRender = new COSWinAudioRender(this);

				if (m_pAudioRender == NULL)
					return VOOSMP_ERR_OutMemory;

				m_pAudioRender->Start();
			}
			else
			{
				m_pAudioRender->Start();
			}
			
			if(m_pVideoRender == NULL)
			{
				m_pVideoRender = new COSWinVideoRender(this, (HWND)m_hWnd);

				if (m_pVideoRender == NULL)
					return VOOSMP_ERR_OutMemory;

				m_pVideoRender->SetWorkPath(m_szPathLib);
				m_pVideoRender->SetRect(&m_rcDraw);
				m_pVideoRender->SetParam(VOOSMP_PID_VIDEO_RENDER_TYPE, &m_nVideoRenderType);
				m_pVideoRender->InitVideoRenderer();
				// David @ 2013/08/31
				// only if videoAspectRatio is valid
				if(VOOSMP_RATIO_MAX != m_nVideoAspectRatio)
					m_pVideoRender->SetAspectRatio((VOOSMP_ASPECT_RATIO *) &m_nVideoAspectRatio);
				else {

					// if no default video aspect been set, just use ratio_00, other wise video will disappear 
					if (m_nAspectRatio == VOOSMP_RATIO_AUTO)
						m_nAspectRatio = VOOSMP_RATIO_00;
					m_pVideoRender->SetAspectRatio((VOOSMP_ASPECT_RATIO *) &m_nAspectRatio);
				}
				m_pVideoRender->SetParam(VOOSMP_PID_DRAW_VIDEO_DIRECTLY, &m_bWindowlessMode);
				m_pVideoRender->SetParam(VOOSMP_PID_VR_USERCALLBACK, &m_bVideoRenderCallback);
				m_pVideoRender->Start();
			}
		}
	}
	else if(VOOSMP_CB_VideoRenderStart == nID)
	{
		int nUpsideDown = -1;
		int nRC = GetParam(VOMP_PID_VIDEO_VIDEO_UPSIDE , &nUpsideDown);
		
		voCAutoLock lock( &m_VideoLock );
		if(nUpsideDown == 1 && m_pVideoRender)
		{
			m_pVideoRender->SetUpsideDown(VO_TRUE);
		}
	}
	else if(nID == VOOSMP_CB_VideoAspectRatio)
	{
		voCAutoLock lock( &m_VideoLock );
		//m_nAspectRatio = *(VOOSMP_ASPECT_RATIO *) pParam1;
		m_nVideoAspectRatio = *(VOOSMP_ASPECT_RATIO *) pParam1;
		if(m_pVideoRender != NULL && VOOSMP_RATIO_AUTO == m_nAspectRatio)
		{
			m_pVideoRender->SetAspectRatio((VOOSMP_ASPECT_RATIO *) pParam1);
		}

		// because we need VOOSMP_CB_VideoAspectRatio event by upper layer, we do not return.
//		return VOOSMP_ERR_Implement;
	}

	return COSVomePlayer::HandleEvent(nID , pParam1 , pParam2);
}
