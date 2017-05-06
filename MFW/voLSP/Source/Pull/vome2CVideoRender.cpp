
/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
	File:		vome2CVideoRender.cpp

	Contains:	vome2CVideoRender class file

	Written by:	Tom Yu Wei 

	Change History (most recent first):
	2010-09-25		Bang			Create file

*******************************************************************************/

#include "vome2CVideoRender.h"
#include "CiOSVideoRender.h"
#include "voOSFunc.h"

#define LOG_TAG "vome2CVideoRender"
#include "voLog.h"

#define MEDIA_SET_VIDEO_SIZE 5


vome2CVideoRender::vome2CVideoRender(vompCEngine * pEngine)
: vome2CBaseRender (pEngine)
, m_pSurface (NULL)
, m_nColorType (VOMP_COLOR_RGB565_PACKED)
, m_bSetPos (false)
, m_bSurfaceNeedChange( true )
, m_pVideoRender(NULL)
{
	strcpy (m_szThreadName, "vome2VideoRender");
	memset (&m_sVideoFormat, 0, sizeof (VOMP_VIDEO_FORMAT));

	memset(&m_rcDraw, 0, sizeof(VOMP_RECT));
    
    if (NULL == m_pVideoRender)
	{
		m_pVideoRender = new CiOSVideoRender(NULL, m_pSurface, NULL);
	}
}

vome2CVideoRender::~vome2CVideoRender()
{
	if(m_pVideoRender)
	{
		m_pVideoRender->Stop();
		delete m_pVideoRender;
		m_pVideoRender = NULL;
	}
}

int vome2CVideoRender::SetSurface (void* hWnd)
{
	m_pSurface = hWnd;

    if (NULL != m_pVideoRender) {
        m_pVideoRender->SetDispRect (m_pSurface, (VO_RECT *)&m_rcDraw, (VO_IV_COLORTYPE)m_nColorType);
    }
    
	return 0;
}

int	vome2CVideoRender::Start (void)
{
	//Mutex::Autolock autoLock(m_mtStatus);
	
	if (NULL == m_pVideoRender)
	{
		m_pVideoRender = new CiOSVideoRender(NULL, m_pSurface, NULL);
	}
    
	m_pVideoRender->Start();
	VO_RECT r;
	r.left=m_rcDraw.left;r.top=m_rcDraw.top;r.right=m_rcDraw.right;r.bottom=m_rcDraw.bottom;
	m_pVideoRender->SetDispRect (m_pSurface, &r, (VO_IV_COLORTYPE)m_nColorType);
	m_pVideoRender->SetVideoInfo (m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
	m_pVideoRender->SetParam (VO_PID_COMMON_LIBOP, (VO_LIB_OPERATOR *)NULL);

	if (m_bSetPos)
		m_bSetPos = false;

	vome2CBaseRender::Start ();

	return 0;
}

int vome2CVideoRender::Stop (void)
{
    int iRet = vome2CBaseRender::Stop();
    
    if (NULL != m_pVideoRender) {
        m_pVideoRender->Stop();
    }
    
    memset (&m_sVideoFormat, 0, sizeof (VOMP_VIDEO_FORMAT));
    m_bSurfaceNeedChange = true;
    
    return iRet;
}

int vome2CVideoRender::SetPos (int nPos)
{
	//Mutex::Autolock autoLock(m_mtStatus);

	if (m_bThreadStatus == VOME2_THREAD_PAUSED)
	{
		m_bSetPos = true;
		m_bThreadStatus = VOME2_THREAD_RUNNING;
	}

	return 0;
}

int vome2CVideoRender::SetColorType (VOMP_COLORTYPE nColor)
{
	m_nColorType = nColor;
    if (NULL != m_pVideoRender) {
        m_pVideoRender->SetDispRect (m_pSurface, (VO_RECT *)&m_rcDraw, (VO_IV_COLORTYPE)m_nColorType);
    }
    
	return 0;
}

int vome2CVideoRender::GetParam (int nID, void * pValue)
{
    if (VOMP_PID_VIDEO_RENDER_CONTEXT == nID) {
        if (NULL != m_pVideoRender) {
            return m_pVideoRender->GetParam(VO_VR_PMID_RENDER_CONTEXT, pValue);
        }
    }
    
	return vome2CBaseRender::GetParam(nID, pValue);
}

int vome2CVideoRender::vome2RenderThreadLoop (void)
{
	if (m_pSurface == NULL)
	{
		voOS_Sleep(10);
		return -1;
	}

	VOMP_BUFFERTYPE *pVideoBuffer = NULL;
	int nRC = VOMP_ERR_Retry;
	while (nRC != VOMP_ERR_None)
	{
		pVideoBuffer = NULL;
		
		//VOLOGI("+Read video buffer...");
		nRC = m_pEngine->GetParam (VOMP_PID_VIDEO_SAMPLE, &pVideoBuffer);
		//VOLOGI("-Read video buffer...");
		
		if (nRC == VOMP_ERR_None)
		{
			break;
		}
		else if( nRC == VOMP_ERR_FormatChange )
		{
			m_bSurfaceNeedChange = true;
			break;
		}
		else if (nRC == VOMP_ERR_Retry)
		{
			voOS_Sleep(5);
		}
		else if (nRC == VOMP_ERR_WaitTime)
		{
		}
		else
		{
			voOS_Sleep(2);
		}
		
		if (m_bThreadStatus != VOME2_THREAD_RUNNING)
			break;
	}

    if ((m_sVideoFormat.Width == 0 && m_sVideoFormat.Height == 0) || m_bSurfaceNeedChange)
	{
		memset (&m_sVideoFormat, 0, sizeof (VOMP_VIDEO_FORMAT));
		nRC = m_pEngine->GetParam (VOMP_PID_VIDEO_FORMAT, &m_sVideoFormat);
//		m_sVideoFormat.Width = (m_sVideoFormat.Width>>5)<<5;
        
        if ((VOMP_ERR_None == nRC) && (m_pVideoRender != NULL)) {
            m_bSurfaceNeedChange = false;
			VO_RECT r;
			r.left=m_rcDraw.left;r.top=m_rcDraw.top;r.right=m_rcDraw.right;r.bottom=m_rcDraw.bottom;
			m_pVideoRender->SetDispRect (m_pSurface, &r, (VO_IV_COLORTYPE)m_nColorType);
			m_pVideoRender->SetVideoInfo (m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
        }
	}
    
    if ((m_pVideoRender != NULL)
		&& (VOME2_THREAD_RUNNING == m_bThreadStatus)
		&& (NULL != pVideoBuffer)
		&& (NULL != pVideoBuffer->pBuffer)
        && (VOMP_ERR_None == nRC))
	{
		VO_VIDEO_BUFFER* pBuf = (VO_VIDEO_BUFFER*)pVideoBuffer->pBuffer;
			
        if (pBuf)
            m_pVideoRender->Render (pBuf, pBuf->Time, VO_TRUE);
	}

	if (m_bSetPos)
	{
		m_bThreadStatus = VOME2_THREAD_PAUSED;
		m_bSetPos = false;
	}
	
	return 0;
}

void vome2CVideoRender::vome2RenderThreadStart (void)
{
}

void vome2CVideoRender::vome2RenderThreadExit (void)
{
	vome2CBaseRender::vome2RenderThreadExit ();
}

int vome2CVideoRender::SetDrawRect(int nLeft, int nTop, int nRight, int nBottom)
{
	m_rcDraw.left	= nLeft;
	m_rcDraw.top	= nTop;
	m_rcDraw.right	= nRight;
	m_rcDraw.bottom	= nBottom;
	
    if (NULL != m_pVideoRender) {
        m_pVideoRender->SetDispRect (m_pSurface, (VO_RECT *)&m_rcDraw, (VO_IV_COLORTYPE)m_nColorType);
    }
    
	return VO_ERR_PLAYER_OK;
}
