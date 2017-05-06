/*
 *  CiOSVideoRender.cpp
 *  vompEngn
 *
 *  Created by Lin Jun on 11/19/10.
 *  Copyright 2010 VisualOn. All rights reserved.
 *
 */

#include "CiOSVideoRender.h"
#include "voLog.h"
#include "voOSFunc.h"
#include "voGLRenderFactory.h"
#include "voSystemStatus.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CiOSVideoRender::CiOSVideoRender (VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
:CBaseVideoRender(hInst, hView, pMemOP)
{
    if (!voSystemStatus::Instance()->IsAPPActive())
    {
        m_pRender = NULL;
    }
    else {
        m_pRender = voGLRenderFactory::Instance()->CreateGLRender();
    }
    
#ifdef _DUMP_YUV
	char szTmp[256];
	voOS_GetAppFolder(szTmp, 256);
	strcat(szTmp, "yuv420.yuv");
	m_hYUV = fopen(szTmp, "wb");
#endif
	
	
#ifdef _DUMP_RGB
	char szRGB[256];
	voOS_GetAppFolder(szRGB, 256);
	strcat(szRGB, "rgb565.raw");
	m_hRGB = fopen(szRGB, "wb");
#endif	
}

CiOSVideoRender::~CiOSVideoRender()
{
	if (m_pRender)
	{
        voGLRenderFactory::Instance()->ReclaimGLRender(m_pRender);
	}
	
#ifdef _DUMP_YUV	
	if(m_hYUV)
		fclose(m_hYUV);
	m_hYUV = NULL;
#endif
	
#ifdef _DUMP_RGB	
	if(m_hRGB)
		fclose(m_hRGB);
	m_hRGB = NULL;
#endif
}

VO_U32 	CiOSVideoRender::SetDispRect (VO_PTR hView, VO_RECT * pDispRect, VO_IV_COLORTYPE nColor)
{
	voCAutoLock lock (&m_csDraw);
	
    if (!voSystemStatus::Instance()->IsAPPActive())
    {
        return VO_ERR_NONE;
    }
    
	m_hView = hView;
    
    if (NULL == m_pRender) {
        return VO_ERR_WRONG_STATUS;
    }
    
    bool bNeedUpdateSize = false;
    
    if (nColor != m_nOutputColor) {
        if (m_pCCRRR != NULL)
            delete m_pCCRRR;
        m_pCCRRR = NULL;
        if (m_pSoftCCRRR != NULL)
            delete m_pSoftCCRRR;
        m_pSoftCCRRR = NULL;
        
        bNeedUpdateSize = true;
    }
    
    m_nOutputColor = nColor;
    int nRenderColoder = 0;
    if (VO_COLOR_RGB565_PACKED == nColor) {
        nRenderColoder = VO_GL_COLOR_RGB565;
    }
    else {
        nRenderColoder = VO_GL_COLOR_RGBA8;
    }

    m_pRender->SetView((UIView *)m_hView);
    m_pRender->SetRGBType(nRenderColoder);
    
	if ((pDispRect != NULL) && (pDispRect->bottom > 0) && (pDispRect->right > 0)) {
        
        if (m_rcDisplay.left != pDispRect->left || m_rcDisplay.top != pDispRect->top ||
            m_rcDisplay.right != pDispRect->right || m_rcDisplay.bottom != pDispRect->bottom)
            
        {
            m_rcDisplay.left = pDispRect->left;
            m_rcDisplay.top = pDispRect->top;
            m_rcDisplay.right = pDispRect->right;
            m_rcDisplay.bottom = pDispRect->bottom;
            
            VOLOGI ("m_rcDisplay is %ld, %ld, %ld, %ld", m_rcDisplay.left, m_rcDisplay.top, m_rcDisplay.right, m_rcDisplay.bottom);
            
            m_bSetDispRect = VO_TRUE;
            bNeedUpdateSize = true;
        }
    }

    if (NULL != m_hView) {
        int width = ((UIView *)m_hView).bounds.size.width * [[UIScreen mainScreen] scale];
        int height = ((UIView *)m_hView).bounds.size.height * [[UIScreen mainScreen] scale];
        
        if (VO_FALSE == m_bSetDispRect && m_rcDisplay.right != width && m_rcDisplay.bottom != height) {
            m_rcDisplay.right = width;
            m_rcDisplay.bottom = height;
            
            bNeedUpdateSize = true;
        }
    }
    
    if (bNeedUpdateSize) {
        UpdateSize ();
    }
	
	return VO_ERR_NONE;
}

VO_U32 	CiOSVideoRender::SetVideoInfo (VO_U32 nWidth, VO_U32 nHeight, VO_IV_COLORTYPE nColor)
{
	voCAutoLock lock (&m_csDraw);
    
	if (!voSystemStatus::Instance()->IsAPPActive())
    {
        return VO_ERR_NONE;
    }
    
	CBaseVideoRender::SetVideoInfo(nWidth, nHeight, nColor);
	
    if (m_pRender) {
        m_pRender->SetTexture(m_nVideoWidth, m_nVideoHeight);
        m_pRender->SetOutputRect(m_nDrawLeft, m_nDrawTop, m_nDrawWidth, m_nDrawHeight);
    }

	return 0;
}

VO_U32 	CiOSVideoRender::Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	voCAutoLock lock (&m_csDraw);
    
    if (!voSystemStatus::Instance()->IsAPPActive())
    {
        return VO_ERR_NONE;
    }
    
    if (NULL == m_pRender) {
        return VO_ERR_FAILED;
    }
    
#ifdef _DUMP_YUV
	if(m_hYUV)
	{
#ifdef _YUV_PLAYER
		int nWidthUV = m_nVideoWidth / 2;
		int nHeightUV = m_nVideoHeight / 2;
		{
			int i = 0;
			for (i = 0; i < m_nVideoHeight; i++)
				fwrite (pVideoBuffer->Buffer[0] + pVideoBuffer->Stride[0] * i, m_nVideoWidth, 1, m_hYUV);
			for (i = 0; i < nHeightUV; i++)
				fwrite (pVideoBuffer->Buffer[1] + pVideoBuffer->Stride[1] * i, nWidthUV, 1, m_hYUV);
			for (i = 0; i < nHeightUV; i++)
				fwrite (pVideoBuffer->Buffer[2] + pVideoBuffer->Stride[2] * i, nWidthUV, 1, m_hYUV);
		}
#else	
		for (int n=0; n<3; n++)
		{
			if (pVideoBuffer->Buffer[0] && pVideoBuffer->Stride[0]>0)
			{
				fwrite(pVideoBuffer->Buffer[n], pVideoBuffer->Stride[n]*m_nVideoHeight, 1, m_hYUV);
			}
		}
#endif //_YUV_PLAYER
	}
#endif //_DUMP_YUV
	
	if (m_fCallBack != NULL)
		return CBaseVideoRender::Render (pVideoBuffer, nStart, bWait);
	
    int nRet = m_pRender->RenderYUV(pVideoBuffer);
    if (VO_GL_RET_OK == nRet) {
        return VO_ERR_NONE;
    }
    else if (VO_GL_RET_IMPLEMENT == nRet) {
        //continue with color convert
    }
    else if (VO_GL_RET_RETRY == nRet) {
        return VO_ERR_RETRY;
    }
    else {
        return VO_ERR_FAILED;
    }
    
	VO_VIDEO_BUFFER buf;
	
	// getFrameData will lock the buffer, we need unlock it when unuse.
    buf.Buffer[0] = (VO_BYTE*)(m_pRender->GetFrameData());
    
    if (NULL == buf.Buffer[0]) {
        VOLOGW("LockFrameData LockFrameData fail fail\n");
        // Don't need unlock if you havn't got any buffer.
        return VO_ERR_RETRY;
    }
    
	if (m_nOutputColor == VO_COLOR_RGB565_PACKED) 
	{
		buf.Stride[0] = m_pRender->GetTextureWidth() * 2;
	}
	else if(m_nOutputColor == VO_COLOR_ARGB32_PACKED)
		buf.Stride[0] = m_pRender->GetTextureWidth() * 4;
	
	buf.Stride[1] = 0;
	buf.Stride[2] = 0;
	
	if (!ConvertData (pVideoBuffer, &buf, nStart, bWait)) {
		return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;
    }
	
#ifdef _DUMP_RGB
	if(m_hRGB)
	{
		fwrite(buf.Buffer[0], 1, buf.Stride[0] * m_pRender->getTextureHeight(), m_hRGB);
	}
#endif
	
	m_pRender->RenderFrameData();
    
	return VO_ERR_NONE;
}

VO_U32 CiOSVideoRender::UpdateSize (void)
{
	voCAutoLock lock (&m_csDraw);
    
    if (!voSystemStatus::Instance()->IsAPPActive())
    {
        return VO_ERR_NONE;
    }
    
	if (m_nVideoWidth == 0 || m_nVideoHeight == 0)
		return 0;
	
	if((m_rcDisplay.right - m_rcDisplay.left) == 0)
		return 0;
	
	
	
	/*
	 CGPoint videoBottomLeft;
	m_nScreenWidth = m_rcDisplay.right - m_rcDisplay.left;
	m_nScreenHeight = m_rcDisplay.bottom - m_rcDisplay.top;
	
	m_nShowLeft = 0;
	m_nShowTop = 0;
	m_nShowWidth = m_nVideoWidth;
	m_nShowHeight = m_nVideoHeight;
	
	m_nDrawLeft = m_rcDisplay.left;
	m_nDrawTop = m_rcDisplay.top;
	m_nDrawWidth = m_rcDisplay.right - m_rcDisplay.left;
	m_nDrawHeight = m_rcDisplay.bottom - m_rcDisplay.top;
	
	
	videoBottomLeft.x = m_rcDisplay.left;
	videoBottomLeft.y = m_rcDisplay.top;
	VO_S32 nWndWidth = m_rcDisplay.right - m_rcDisplay.left;
	VO_S32 nWndHeight = m_rcDisplay.bottom - m_rcDisplay.top;
	
	m_bRotate = VO_FALSE;
	m_bFullScreen = VO_FALSE;
	
	// landscape mode
	if(m_nScreenWidth > m_nScreenHeight)
	{
		m_nDrawHeight	= m_nScreenHeight;
		m_nDrawWidth	= (m_nDrawHeight*m_nVideoWidth)/m_nVideoHeight;
		
		if(m_nDrawWidth > m_nScreenWidth)
		{
			m_nDrawWidth	= m_nScreenWidth;
			m_nDrawHeight	= (m_nDrawWidth*m_nVideoHeight)/m_nVideoWidth;
		}
		
		m_nDrawWidth	= (m_nDrawWidth / 4) * 4;
		
	}
	else // portrait mode
	{
		m_nDrawWidth	= m_nScreenWidth;
		m_nDrawHeight	= (m_nDrawWidth*m_nVideoHeight)/m_nVideoWidth;
		m_nDrawHeight	= (m_nDrawHeight / 4) * 4;
	}
	 
	 videoBottomLeft.x = (m_nScreenWidth - m_nDrawWidth) / 2;
	 videoBottomLeft.y = (m_nScreenHeight - m_nDrawHeight) / 2;
	 
	 
	 m_nDrawLeft		= videoBottomLeft.x;
	 m_nDrawTop		= videoBottomLeft.y;
	 VOLOGI("m_nDrawLeft＝%d, m_nDrawTop=%d, m_nDrawWidth=%d, m_nDrawHeight=%d, m_nScreenWidth=%d, m_nScreenHeight=%d)", 
			m_nDrawLeft, m_nDrawTop, m_nDrawWidth, m_nDrawHeight, m_nScreenWidth, m_nScreenHeight);
	 */
	
	
	/*********************************************************
	 Base class code*/	
	/*
	m_nVideoWidth = m_nVideoWidth;
	m_nVideoHeight = m_nVideoHeight;
	m_nRatio = VO_RATIO_00;
	m_nZoomMode = VO_ZM_LETTERBOX;
	 */

	
	m_nScreenWidth = m_rcDisplay.right - m_rcDisplay.left;
	m_nScreenHeight = m_rcDisplay.bottom - m_rcDisplay.top;

	m_nShowLeft = 0;
	m_nShowTop = 0;
	m_nShowWidth = m_nVideoWidth;
	m_nShowHeight = m_nVideoHeight;
	
	m_nDrawLeft = m_rcDisplay.left;
	m_nDrawTop = m_rcDisplay.top;
	m_nDrawWidth = m_rcDisplay.right - m_rcDisplay.left;
	m_nDrawHeight = m_rcDisplay.bottom - m_rcDisplay.top;
	
	VO_S32 nWndWidth = m_nDrawWidth;
	VO_S32 nWndHeight = m_nDrawHeight;
	
	m_bRotate = VO_FALSE;
	m_bFullScreen = VO_FALSE;
	
	// detect it is full screen or not
	if (m_nDrawWidth == m_nScreenWidth && m_nDrawHeight == m_nScreenHeight)
	{
		m_bFullScreen = VO_TRUE;
		
		// detect it need to rotate or not
		/*if (m_nRatio == VO_RATIO_11)
		{
			if (((m_nShowWidth >= m_nShowHeight && nWndWidth >= nWndHeight) ||
				 (m_nShowWidth <= m_nShowHeight && nWndWidth <= nWndHeight)))
			{
				m_bRotate = VO_FALSE;
			}
			else
			{
				m_bRotate = VO_TRUE;
			}
		}
		else*/
		{
			if (nWndWidth <= nWndHeight)
			{
				m_bRotate = VO_FALSE;
			}
			else
			{
				m_bRotate = VO_TRUE;
			}
		}
	}
	
	VOLOGI("rotate = %s, ratio = %d, mode = %d\n", m_bRotate?"true":"false", m_nRatio, m_nZoomMode);
	
	
	if (m_nZoomMode == VO_ZM_FITWINDOW)
	{
		//if (!m_bRotate)
		{
			m_nDrawWidth = m_rcDisplay.right - m_rcDisplay.left;
			m_nDrawHeight = m_rcDisplay.bottom - m_rcDisplay.top;
		}
		/*else
		{
			m_nDrawWidth = m_rcDisplay.bottom - m_rcDisplay.top;
			m_nDrawHeight = m_rcDisplay.right - m_rcDisplay.left;
		}*/
	}
	else
	{
		// calculate the max draw width and height
		//if (!m_bRotate)
		{
			m_nDrawWidth = nWndWidth;
			m_nDrawHeight = nWndHeight;
		}
		/*else
		{
			m_nDrawWidth = nWndHeight;
			m_nDrawHeight = nWndWidth;
		}
		 */
		
		VO_U32	nW = m_nShowWidth;
		VO_U32	nH = m_nShowHeight;
        
		if(m_nRatio == VO_RATIO_11)
		{
			nW = 1;
			nH = 1;
		}
		else if (m_nRatio == VO_RATIO_43)
		{
			nW = 4;
			nH = 3;
		}
		else if (m_nRatio == VO_RATIO_169)
		{
			nW = 16;
			nH = 9;
		}
		else if (m_nRatio == VO_RATIO_21)
		{
			nW = 2;
			nH = 1;
		}
        else if (m_nRatio == VO_RATIO_2331)
		{
			nW = 233;
			nH = 100;
		}
        else
        {
            nW = m_nRatio >> 16;
            nH = m_nRatio & 0xffff;
        }
		
		// if ratio is VO_RATIO_11 calculate the draw depend on the video size.
		if (m_nRatio == VO_RATIO_00)
		{
			if (m_nZoomMode == VO_ZM_PANSCAN)
			{
				if (m_nShowWidth * m_nDrawHeight >= m_nShowHeight * m_nDrawWidth)
					m_nDrawWidth = m_nDrawHeight * m_nShowWidth / m_nShowHeight;
				else if (m_nShowWidth * m_nDrawHeight < m_nShowHeight * m_nDrawWidth)
					m_nDrawHeight = m_nDrawWidth * m_nShowHeight / m_nShowWidth;
			}
			else
			{
				if (m_nShowWidth * m_nDrawHeight >= m_nShowHeight * m_nDrawWidth)
					m_nDrawHeight = m_nDrawWidth * m_nShowHeight / m_nShowWidth;
				else if (m_nShowWidth * m_nDrawHeight < m_nShowHeight * m_nDrawWidth)
					m_nDrawWidth = m_nDrawHeight * m_nShowWidth / m_nShowHeight;
			}
		}
		// if ratio is others calculate the draw depend on wnd size.
		else
		{
			if (m_nZoomMode == VO_ZM_PANSCAN)
			{
				if (m_nDrawWidth * nH > m_nDrawHeight * nW)
					m_nDrawHeight = m_nDrawWidth * nH / nW;
				else
					m_nDrawWidth = m_nDrawHeight * nW / nH;
			}
			else
			{
				if (m_nDrawWidth * nH > m_nDrawHeight * nW)
					m_nDrawWidth = m_nDrawHeight * nW / nH;
				else
					m_nDrawHeight = m_nDrawWidth * nH / nW;
			}
		}
		
		if (m_nZoomMode == VO_ZM_ORIGINAL)
		{
			VO_U32	nShowWidth = m_nShowWidth;
			VO_U32	nShowHeight = m_nShowHeight;
			if (m_bRotate)
			{
				nShowWidth = m_nShowHeight;
				nShowHeight = m_nShowWidth;
			}
			
			if (m_nDrawWidth > nShowWidth || m_nDrawHeight > nShowHeight)
			{
				if (m_nDrawWidth * nShowHeight >= m_nDrawHeight * nShowWidth)
				{
					m_nDrawWidth = m_nDrawWidth * nShowHeight / m_nDrawHeight;
					m_nDrawHeight = nShowHeight;
				}
				else
				{
					m_nDrawHeight = m_nDrawHeight * m_nShowWidth / m_nDrawWidth;
					m_nDrawWidth = nShowWidth;
				}
			}
		}
	}
	
	//if (!m_bRotate)
	{
		nWndWidth = m_rcDisplay.right - m_rcDisplay.left;
		nWndHeight = m_rcDisplay.bottom - m_rcDisplay.top;
	}
	/*else
	{
		nWndHeight = m_rcDisplay.right - m_rcDisplay.left;
		nWndWidth = m_rcDisplay.bottom - m_rcDisplay.top;
	}*/
	
	// offset the video buffer for panscan mode.
	if (m_nDrawWidth > (VO_U32)nWndWidth)
	{
		m_nShowLeft = m_nShowWidth * (m_nDrawWidth - nWndWidth) / m_nDrawWidth / 2;
		m_nShowWidth = m_nShowWidth - m_nShowLeft * 2;
		m_nDrawWidth = nWndWidth;
	}
	else if (m_nDrawHeight > (VO_U32)nWndHeight)
	{
		m_nShowTop = m_nShowHeight * (m_nDrawHeight - nWndHeight) / m_nDrawHeight / 2;
		m_nShowHeight = m_nShowHeight - m_nShowTop * 2;
		m_nDrawHeight = nWndHeight;
	}
	
	// adjust the size
	//if (m_bRotate)
	{
		m_nShowLeft = m_nShowLeft & ~15;
		m_nShowTop = m_nShowTop & ~15;
		
		m_nShowWidth =  m_nShowWidth & ~3;
		m_nShowHeight = m_nShowHeight & ~1;
        
        if (VO_GL_SUPPORT_RGB == m_pRender->GetSupportType()) {
            m_nDrawWidth = m_nDrawWidth & ~3;
            m_nDrawHeight = m_nDrawHeight & ~1;
            m_nDrawLeft = (m_nDrawLeft + (nWndWidth - m_nDrawWidth) / 2) & ~1;
            m_nDrawTop = (m_nDrawTop + (nWndHeight - m_nDrawHeight) / 2) & ~1;
		}
        else {
            m_nDrawLeft = (m_nDrawLeft + (nWndWidth - m_nDrawWidth) / 2);
            m_nDrawTop = (m_nDrawTop + (nWndHeight - m_nDrawHeight) / 2);
        }
	}
	/*else
	{
		m_nShowLeft = m_nShowLeft & ~15;
		m_nShowTop = m_nShowTop & ~15;
		
		m_nShowWidth =  m_nShowWidth & ~3;
		m_nShowHeight = m_nShowHeight & ~3;
		m_nDrawWidth = m_nDrawWidth & ~3;
		m_nDrawHeight = m_nDrawHeight & ~3;
		
		m_nDrawTop = (m_nDrawTop + (nWndWidth - m_nDrawWidth) / 2) & ~1;
		m_nDrawLeft = (m_nDrawLeft + (nWndHeight - m_nDrawHeight) / 2) & ~1;
	}
	 */
	
	// add
	//m_nDrawLeft = (m_nScreenWidth - m_nDrawWidth) / 2;
	//m_nDrawTop = (m_nScreenHeight - m_nDrawHeight) / 2;
	
	
	
	
	/*******************************************************
	 */
	 
		
	VOLOGI("m_nDrawLeft＝%ld, m_nDrawTop=%ld, m_nDrawWidth=%ld, m_nDrawHeight=%ld, m_nScreenWidth=%ld, m_nScreenHeight=%ld)", 
		   m_nDrawLeft, m_nDrawTop, m_nDrawWidth, m_nDrawHeight, m_nScreenWidth, m_nScreenHeight);
	
	if (m_pCCRRR == NULL)
		CreateCCRRR ();
	
	if (m_pCCRRR != NULL)
		m_pCCRRR->SetColorType (VO_COLOR_YUV_PLANAR420, m_nOutputColor);

	if (m_pCCRRR != NULL)
		m_pCCRRR->SetCCRRSize (&m_nVideoWidth, &m_nVideoHeight, &m_nVideoWidth, &m_nVideoHeight, VO_RT_DISABLE);
	if (m_pSoftCCRRR != NULL)
		m_pSoftCCRRR->SetCCRRSize (&m_nVideoWidth, &m_nVideoHeight, &m_nVideoWidth, &m_nVideoHeight, VO_RT_DISABLE);

	if (m_fCallBack != NULL)
	{
		if (m_bufRGB565.Buffer[0] != NULL)
			m_pMemOP->Free (VO_INDEX_MFW_VOMMPLAY, m_bufRGB565.Buffer[0]);
		m_bufRGB565.Buffer[0] = NULL;
	}
	
    if (m_pRender) {
        m_pRender->SetOutputRect(m_nDrawLeft, m_nDrawTop, m_nDrawWidth, m_nDrawHeight);
        m_pRender->Redraw();
    }
    
	return 0;
}

bool CiOSVideoRender::CreateCCRRR (void)
{
    if (m_pRender && (VO_GL_SUPPORT_RGB == m_pRender->GetSupportType())) {
        return CBaseVideoRender::CreateCCRRR();
    }
    
    return false;
}

VO_U32 CiOSVideoRender::GetParam (VO_U32 nID, VO_PTR pValue)
{
	if (nID == VO_VR_PMID_RENDER_CONTEXT)
	{
		*(EAGLContext **)pValue = m_pRender->GetGLContext();
        return VO_ERR_NONE;
	}
    else if (VO_VR_PMID_CAPTURE_VIDEO_IMAGE == nID) {
        if (NULL == m_pRender) {
            return VO_ERR_RETRY;
        }
        
        VO_IMAGE_DATA *pData = (VO_IMAGE_DATA *)pValue;
        return m_pRender->GetLastFrame(pData);
    }
	
	return CBaseVideoRender::GetParam(nID, pValue);
}

VO_U32 CiOSVideoRender::SetParam (VO_U32 nID, VO_PTR pValue)
{
	if (nID == VO_PID_VIDEO_UPSIDEDOWN)
	{
		m_bVideoUpsizeDown = *(VO_BOOL*)pValue;
        
        if (m_pRender)
        {
            if (VO_TRUE == m_bVideoUpsizeDown) {
                m_pRender->SetRotation(VO_GL_ROTATION_180FLIP);
            }
            else {
                m_pRender->SetRotation(VO_GL_ROTATION_0);
            }
        }
	}
    
	return VO_ERR_WRONG_PARAM_ID | VO_INDEX_SNK_VIDEO;
}

VO_U32 CiOSVideoRender::Start (void)
{
	return VO_ERR_NONE;
}

VO_U32 CiOSVideoRender::Stop (void)
{
    if (m_pCCRRR != NULL)
        delete m_pCCRRR;
    m_pCCRRR = NULL;
    if (m_pSoftCCRRR != NULL)
        delete m_pSoftCCRRR;
    m_pSoftCCRRR = NULL;
    
    memset (&m_rcDisplay, 0, sizeof(m_rcDisplay));
	
    if (m_pRender && voSystemStatus::Instance()->IsAPPActive()) {
        m_pRender->ClearGL();
    }
    
	return CBaseVideoRender::Stop();
}
