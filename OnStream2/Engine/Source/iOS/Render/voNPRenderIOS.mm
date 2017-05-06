/*
 *  voNPRenderIOS.cpp
 *  vompEngn
 *
 *  Created by Lin Jun on 11/19/10.
 *  Copyright 2010 VisualOn. All rights reserved.
 *
 */

#include "voNPRenderIOS.h"
#include "voLog.h"
#include "voOSFunc.h"
#include "voVideoRender.h"

#import <UIKit/UIKit.h>

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voNPRenderIOS::voNPRenderIOS()
: m_hView (NULL)
, m_hSecondView (NULL)
, m_nVideoWidth (480)
, m_nVideoHeight (320)
, m_bSetDispRect (VO_FALSE)
, m_nZoomMode (VO_ZM_LETTERBOX)
, m_nRatio (VO_RATIO_00)
, m_nOutputColor (VO_COLOR_RGB565_PACKED)
, m_nShowLeft (0)
, m_nShowTop (0)
, m_nShowWidth (0)
, m_nShowHeight (0)
, m_nDrawLeft (0)
, m_nDrawTop (0)
, m_nDrawWidth (0)
, m_nDrawHeight (0)
, m_nDrawLeftLast (0)
, m_nDrawTopLast (0)
, m_nDrawWidthLast (0)
, m_nDrawHeightLast (0)
, m_nScreenWidth (0)
, m_nScreenHeight (0)
, m_bVideoUpsizeDown(VO_FALSE)
, m_bRotate (VO_FALSE)
, m_bFullScreen (VO_FALSE)
, m_bRotateType (VO_RT_90R)
{
	memset (&m_rcDisplay, 0, sizeof (VO_RECT));
	m_bSetDispRect = VO_FALSE;
}

voNPRenderIOS::~voNPRenderIOS()
{
}

VO_U32 voNPRenderIOS::SetSecondView(VO_PTR hView)
{
    if (hView != m_hSecondView) {
        
        m_hSecondView = hView;
        
        if (NULL != m_hSecondView) {
            if ([NSRunLoop mainRunLoop] == [NSRunLoop currentRunLoop]) {
                RunningRequestOnMain(0, NULL, NULL);
            }
            else {
                PostRunOnMainRequest(false, 0, NULL, NULL);
            }
        }
    }
    
    return VO_ERR_NONE;
}

VO_U32 voNPRenderIOS::SetView(VO_PTR hView)
{
    if (hView != m_hView) {
        m_nDrawLeftLast = 0;
        m_nDrawTopLast = 0;
        m_nDrawWidthLast = 0;
        m_nDrawHeightLast = 0;
        m_hView = hView;
    }
    
    return VO_ERR_NONE;
}

VO_U32 voNPRenderIOS::SetDispRect(VOOSMP_RECT * pDispRect)
{
    bool bNeedUpdateSize = false;
    
    if ((pDispRect != NULL) && (pDispRect->nBottom > 0) && (pDispRect->nRight > 0)) {
        memcpy(&m_rcDisplay, pDispRect, sizeof(m_rcDisplay));
        m_bSetDispRect = VO_TRUE;
        
        bNeedUpdateSize = true;
    }
    
    if (NULL != m_hView) {
        int width = ((UIView *)m_hView).bounds.size.width * [[UIScreen mainScreen] scale];
        int height = ((UIView *)m_hView).bounds.size.height * [[UIScreen mainScreen] scale];
        
        if ((VO_FALSE == m_bSetDispRect) && (m_rcDisplay.nRight != width) && (m_rcDisplay.nBottom != height)) {
            m_rcDisplay.nRight = width;
            m_rcDisplay.nBottom = height;
            
            bNeedUpdateSize = true;
        }
    }
    
    if (bNeedUpdateSize) {
        UpdateSize ();
    }

	return VO_ERR_NONE;
}

VO_U32 voNPRenderIOS::SetDispType (VO_IV_ZOOM_MODE nZoomMode, VO_IV_ASPECT_RATIO nRatio)
{
	if (m_nZoomMode != nZoomMode || m_nRatio != nRatio)
	{
		m_nZoomMode = nZoomMode;
		m_nRatio = nRatio;
        
		UpdateSize ();
	}
    
	return VO_ERR_NONE;
}

VO_U32 	voNPRenderIOS::SetVideoInfo(VO_U32 nWidth, VO_U32 nHeight)
{
	if (m_nVideoWidth != nWidth || m_nVideoHeight != nHeight)
	{
		m_nVideoWidth = nWidth;
		m_nVideoHeight = nHeight;
        
		m_nShowWidth = m_nVideoWidth;
		m_nShowHeight = m_nVideoHeight;
        
		UpdateSize ();
	}

	return 0;
}

VO_U32 voNPRenderIOS::UpdateSize (void)
{
	if (m_nVideoWidth == 0 || m_nVideoHeight == 0)
		return 0;
	
	if((m_rcDisplay.nRight - m_rcDisplay.nLeft) == 0)
		return 0;
	
	m_nScreenWidth = m_rcDisplay.nRight - m_rcDisplay.nLeft;
	m_nScreenHeight = m_rcDisplay.nBottom - m_rcDisplay.nTop;

	m_nShowLeft = 0;
	m_nShowTop = 0;
	m_nShowWidth = m_nVideoWidth;
	m_nShowHeight = m_nVideoHeight;
	
	m_nDrawLeft = m_rcDisplay.nLeft;
	m_nDrawTop = m_rcDisplay.nTop;
	m_nDrawWidth = m_rcDisplay.nRight - m_rcDisplay.nLeft;
	m_nDrawHeight = m_rcDisplay.nBottom - m_rcDisplay.nTop;
	
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
			m_nDrawWidth = m_rcDisplay.nRight - m_rcDisplay.nLeft;
			m_nDrawHeight = m_rcDisplay.nBottom - m_rcDisplay.nTop;
		}
		/*else
		{
			m_nDrawWidth = m_rcDisplay.nBottom - m_rcDisplay.nTop;
			m_nDrawHeight = m_rcDisplay.nRight - m_rcDisplay.nLeft;
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
		nWndWidth = m_rcDisplay.nRight - m_rcDisplay.nLeft;
		nWndHeight = m_rcDisplay.nBottom - m_rcDisplay.nTop;
	}
	/*else
	{
		nWndHeight = m_rcDisplay.nRight - m_rcDisplay.nLeft;
		nWndWidth = m_rcDisplay.nBottom - m_rcDisplay.nTop;
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
		m_nDrawWidth = m_nDrawWidth & ~3;
		m_nDrawHeight = m_nDrawHeight & ~1;
		
		m_nDrawLeft = (m_nDrawLeft + (nWndWidth - m_nDrawWidth) / 2) & ~1;
		m_nDrawTop = (m_nDrawTop + (nWndHeight - m_nDrawHeight) / 2) & ~1;
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

    if (m_nDrawLeftLast != m_nDrawLeft
        || m_nDrawTopLast != m_nDrawTop
        || m_nDrawWidthLast != m_nDrawWidth
        || m_nDrawHeightLast != m_nDrawHeight) {
        
        m_nDrawLeftLast = m_nDrawLeft;
        m_nDrawTopLast = m_nDrawTop;
        m_nDrawWidthLast = m_nDrawWidth;
        m_nDrawHeightLast = m_nDrawHeight;
        
        if ([NSRunLoop mainRunLoop] == [NSRunLoop currentRunLoop]) {
            RunningRequestOnMain(0, NULL, NULL);
        }
        else {
            PostRunOnMainRequest(false, 0, NULL, NULL);
        }
    }

    
	return 0;
}

VO_U32 voNPRenderIOS::GetParam (VO_U32 nID, VO_PTR pValue)
{
	if (nID == VO_VR_PMID_DrawRect)
	{
		VO_RECT * pRect = (VO_RECT *)pValue;
		if (pRect != NULL)
		{
			pRect->left = m_nDrawLeft;
			pRect->top = m_nDrawTop;
			pRect->right = m_nDrawLeft + m_nDrawWidth;
			pRect->bottom = m_nDrawTop + m_nDrawHeight;
		}
        
		return VO_ERR_NONE;
	}
    
	return VO_ERR_WRONG_PARAM_ID | VO_INDEX_SNK_VIDEO;
}

void voNPRenderIOS::RunningRequestOnMain(int nID, void *pParam1, void *pParam2)
{
    if (NULL == m_hView) {
        return;
    }
    
    CGRect cRect;
    cRect.origin.x = m_nDrawLeft / [[UIScreen mainScreen] scale];
    cRect.origin.y = m_nDrawTop / [[UIScreen mainScreen] scale];
    cRect.size.width = m_nDrawWidth / [[UIScreen mainScreen] scale];
    cRect.size.height = m_nDrawHeight / [[UIScreen mainScreen] scale];

    if (m_hSecondView) {
        if ((cRect.origin.x != ((UIView *)m_hSecondView).frame.origin.x)
            || (cRect.origin.y != ((UIView *)m_hSecondView).frame.origin.y)
            || (cRect.size.width != ((UIView *)m_hSecondView).frame.size.width)
            || (cRect.size.height != ((UIView *)m_hSecondView).frame.size.height)
            ) {
            
            ((UIView *)m_hSecondView).frame = cRect;
        }
    }
    
    if ((cRect.origin.x != ((UIView *)m_hView).frame.origin.x)
        || (cRect.origin.y != ((UIView *)m_hView).frame.origin.y)
        || (cRect.size.width != ((UIView *)m_hView).frame.size.width)
        || (cRect.size.height != ((UIView *)m_hView).frame.size.height)
        ) {
        
        ((UIView *)m_hView).frame = cRect;
    }
}

VO_U32 voNPRenderIOS::SetParam (VO_U32 nID, VO_PTR pValue)
{
	if (nID == VO_PID_VIDEO_UPSIDEDOWN)
	{
		m_bVideoUpsizeDown = *(VO_BOOL*)pValue;
    }
    
	return VO_ERR_WRONG_PARAM_ID | VO_INDEX_SNK_VIDEO;
}

VO_U32 voNPRenderIOS::Start (void)
{
	return VO_ERR_NONE;
}

VO_U32 voNPRenderIOS::Stop (void)
{
	return VO_ERR_NONE;
}
