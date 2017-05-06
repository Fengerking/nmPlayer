	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.cpp

	Contains:	CVideoWindow class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#if defined(LINUX)
#  include <linux/fb.h>
#  include <fcntl.h>
#endif

#if defined _IOS || defined _MAC_OS
#include "voOSFunc.h"
#endif

#include "vompType.h"
#include "CBaseVideoRender.h"

#define LOG_TAG "CBaseVideoRender"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CBaseVideoRender::CBaseVideoRender(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
	: m_hInst (hInst)
	, m_pMemOP (pMemOP)
	, m_hView (hView)
	, m_pConfig (NULL)
	, m_pCfgFile (NULL)
	, m_pCCRRR (NULL)
	, m_pSoftCCRRR (NULL)
	, m_bSoftForce (VO_FALSE)
	, m_nVideoWidth (0)
	, m_nVideoHeight (0)
	, m_nVideoColor (VO_COLOR_YUV_PLANAR420)
	, m_nBytesPixel (0)
	, m_bSetDispRect (VO_FALSE)
	, m_nZoomMode (VO_ZM_LETTERBOX)
	, m_nRatio (VO_RATIO_00)
	, m_nOutputColor (VO_COLOR_RGB565_PACKED)
	, m_fCallBack (NULL)
	, m_pUserData (NULL)
	, m_fEventCallBack (NULL)
	, m_pEventUserData (NULL)
    , m_nShowLeft (0)
	, m_nShowTop (0)
	, m_nShowWidth (0)
	, m_nShowHeight (0)
	, m_nDrawLeft (0)
	, m_nDrawTop (0)
	, m_nDrawWidth (0)
	, m_nDrawHeight (0)
	, m_nScreenWidth (0)
	, m_nScreenHeight (0)
	, m_bVideoUpsizeDown(VO_FALSE)
	, m_bRotate (VO_FALSE)
	, m_bFullScreen (VO_FALSE)
	, m_bRotateType (VO_RT_90R)
	, m_pUVData (NULL)
	, m_pOutMemOP (NULL)
	, m_pLibOP (NULL)
	, m_pWorkPath (NULL)
	, m_nCPUVer (6)
	, m_bCanRedraw (VO_FALSE)
	, m_bBlockEraseBackGround (VO_FALSE)
#ifdef _DRAW_RENDER_INFO
	, m_nTimeoutCount (0)
	, m_nLastRenderSysTime (-1)
#endif	// _DRAW_RENDER_INFO
{
	if (m_pMemOP == NULL)
	{
		cmnMemFillPointer (VO_INDEX_SNK_AUDIO);
		m_pMemOP = &g_memOP;
	}

	memset (&m_inData, 0, sizeof (VO_VIDEO_BUFFER));
	m_inData.ColorType = VO_COLOR_YUV_PLANAR420;

	memset (&m_bufRGB565, 0, sizeof (VO_VIDEO_BUFFER));
	m_bufRGB565.ColorType = VO_COLOR_RGB565_PACKED;

	memset (&m_bufRGBRevert, 0, sizeof (VO_VIDEO_BUFFER));
	m_bufRGBRevert.ColorType = VO_COLOR_RGB565_PACKED;

	memset (&m_bufOutput, 0, sizeof (VO_VIDEO_BUFFER));

	memset (&m_rcDisplay, 0, sizeof (VO_RECT));
	m_bSetDispRect = VO_FALSE;

}

CBaseVideoRender::~CBaseVideoRender ()
{
	if (m_pCCRRR != NULL)
		delete m_pCCRRR;
	m_pCCRRR = NULL;
	if (m_pSoftCCRRR != NULL)
		delete m_pSoftCCRRR;
	m_pSoftCCRRR = NULL;

	if (m_bufRGB565.Buffer[0] != NULL)
		m_pMemOP->Free (VO_INDEX_MFW_VOMMPLAY, m_bufRGB565.Buffer[0]);
	
	if(m_bufRGBRevert.Buffer[0] != NULL)
		m_pMemOP->Free (VO_INDEX_MFW_VOMMPLAY, m_bufRGBRevert.Buffer[0]);

	if (m_pUVData != NULL)
		delete []m_pUVData;

	if (m_pCfgFile != NULL)
		delete m_pCfgFile;
	m_pCfgFile = NULL;
}

VO_U32 CBaseVideoRender::SetVideoInfo (VO_U32 nWidth, VO_U32 nHeight, VO_IV_COLORTYPE nColor)
{
	if (m_nVideoWidth != nWidth || m_nVideoHeight != nHeight)
	{
		m_nVideoWidth = nWidth;
		m_nVideoHeight = nHeight;
		m_nVideoColor = nColor;

		m_nShowWidth = m_nVideoWidth;
		m_nShowHeight = m_nVideoHeight;

		m_nYUVWidth = (m_nVideoWidth + 15) & ~0X0F;
		m_nYUVHeight = (m_nVideoHeight + 15) & ~0X0F;

		UpdateSize ();
	}

	return VO_ERR_NONE;
}

VO_U32 CBaseVideoRender::SetDispRect (VO_PTR hView, VO_RECT * pDispRect, VO_IV_COLORTYPE nColor)
{
	voCAutoLock lock (&m_csDraw);
	if (hView != NULL && hView != m_hView)
		m_hView = hView;

	m_nOutputColor = nColor;

	if (nColor == VO_COLOR_RGB565_PACKED || nColor == VO_COLOR_RGB555_PACKED)
		m_nBytesPixel = 2;
	else if (nColor == VO_COLOR_RGB888_PACKED || nColor == VO_COLOR_RGB888_PLANAR)
		m_nBytesPixel = 3;
	else if (nColor == VO_COLOR_ARGB32_PACKED || nColor == VO_COLOR_RGB32_PACKED)
		m_nBytesPixel = 4;

	if (m_nBytesPixel > 0 && m_pCCRRR == NULL)
		CreateCCRRR ();

	if (m_pCCRRR != NULL)
		m_pCCRRR->SetColorType (m_nVideoColor, m_nOutputColor);

	if (pDispRect == NULL)
		return VO_ERR_NONE;

	if (pDispRect->bottom <= 0 || pDispRect->right <= 0)
		return VO_ERR_NONE;

	m_bSetDispRect = VO_TRUE;
	if (m_rcDisplay.left != pDispRect->left || m_rcDisplay.top != pDispRect->top ||
		m_rcDisplay.right != pDispRect->right || m_rcDisplay.bottom != pDispRect->bottom)
	{
		m_rcDisplay.left = pDispRect->left;
		m_rcDisplay.top = pDispRect->top;
		m_rcDisplay.right = pDispRect->right;
		m_rcDisplay.bottom = pDispRect->bottom;

		//VOLOGI ("m_rcDisplay is %d, %d, %d, %d", (int)m_rcDisplay.left, (int)m_rcDisplay.top, (int)m_rcDisplay.right, (int)m_rcDisplay.bottom);

		UpdateSize ();
	}

	return VO_ERR_NONE;
}

VO_U32 CBaseVideoRender::SetDispType (VO_IV_ZOOM_MODE nZoomMode, VO_IV_ASPECT_RATIO nRatio)
{
	if (m_nZoomMode != nZoomMode || m_nRatio != nRatio)
	{
		if(VO_ZM_MAX != nZoomMode)
			m_nZoomMode = nZoomMode;

		if(VO_RATIO_MAX != nRatio)
			m_nRatio = nRatio;

		UpdateSize ();
	}

	return VO_ERR_NONE;
}

VO_U32 CBaseVideoRender::Start (void)
{
	return VO_ERR_NONE;
}

VO_U32 CBaseVideoRender::Pause (void)
{
	return VO_ERR_NONE;
}

VO_U32 CBaseVideoRender::Stop (void)
{
	return VO_ERR_NONE;
}

VO_U32 CBaseVideoRender::Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	VOLOGR ("Start Time : %d", (int)nStart);

	if (m_fCallBack != NULL)
	{
		VO_U32		nRC = VO_ERR_NONE;

		m_csDraw.Lock ();
		m_fmtVideo.Width = m_nShowWidth;
		m_fmtVideo.Height = m_nShowHeight;
		nRC = m_fCallBack (m_pUserData, pVideoBuffer, &m_fmtVideo, (VO_S32)nStart);
		m_csDraw.Unlock ();

		if (nRC == VO_ERR_FINISH)
			return VO_ERR_NONE;

		if (m_nDrawWidth == 0 || m_nDrawHeight == 0)
			return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;

		if (m_pCCRRR == NULL)
		{
			CBaseVideoRender::CreateCCRRR ();
			UpdateSize ();
		}

		voCAutoLock lock (&m_csDraw);
		VO_VIDEO_BUFFER * pRGBBuffer = &m_bufOutput;
		if (pRGBBuffer->Buffer[0] != NULL)
		{
			if (m_bRotate)
				m_bufOutput.Stride[0] = m_nDrawHeight * 2;
			else
				m_bufOutput.Stride[0] = m_nDrawWidth * 2;
		}
		else
		{
			pRGBBuffer = &m_bufRGB565;
			if (m_bufRGB565.Buffer[0] == NULL)
			{
				VO_MEM_INFO memInfo;
				memInfo.Size = m_nDrawWidth * m_nDrawHeight * 2;
				m_pMemOP->Alloc (VO_INDEX_MFW_VOMMPLAY, &memInfo);

				m_bufRGB565.Buffer[0] = (VO_PBYTE) memInfo.VBuffer;

				if (m_bRotate)
					m_bufRGB565.Stride[0] = m_nDrawHeight * 2;
				else
					m_bufRGB565.Stride[0] = m_nDrawWidth * 2;

				if(m_bVideoUpsizeDown == VO_TRUE && m_bufRGBRevert.Buffer[0] == NULL)
				{
					m_pMemOP->Alloc (VO_INDEX_MFW_VOMMPLAY, &memInfo);
					m_bufRGBRevert.Buffer[0] = (VO_PBYTE) memInfo.VBuffer;
					m_bufRGBRevert.Stride[0] = m_bufRGB565.Stride[0];
				}
			}
		}

		if (!ConvertData (pVideoBuffer, pRGBBuffer, nStart, bWait))
		{
			VOLOGE ("ConvertData Error!");
			return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;
		}

		if (m_bRotate)
		{
			m_fmtVideo.Width = m_nDrawHeight;
			m_fmtVideo.Height = m_nDrawWidth;
		}
		else
		{
			m_fmtVideo.Width = m_nDrawWidth;
			m_fmtVideo.Height = m_nDrawHeight;
		}
		if(m_bVideoUpsizeDown == VO_TRUE&& m_bufRGBRevert.Buffer[0]!= NULL)
		{   //TODO: if color conversion can support revert, the following codes should be deprecated 
			VO_U32 i;
			VO_BYTE *pSrc, *pDst;
			pSrc = pRGBBuffer->Buffer[0]+ (m_fmtVideo.Height -1)*pRGBBuffer->Stride[0];
			pDst = m_bufRGBRevert.Buffer[0];

			for (i=0; i<(VO_U32)(m_fmtVideo.Height); i++, pSrc -= pRGBBuffer->Stride[0], pDst += pRGBBuffer->Stride[0])
				memcpy(pDst, pSrc, pRGBBuffer->Stride[0]);
			pRGBBuffer = &m_bufRGBRevert;
		}

		nRC = m_fCallBack (m_pUserData, pRGBBuffer, &m_fmtVideo, (VO_S32) nStart);		

		if (nRC == VO_ERR_FINISH)
			return VO_ERR_NONE;
	}

	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;
}

VO_U32	CBaseVideoRender::RenderRGBBuffer (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_BUFFER * pRGBBuffer)
{
	if (m_nDrawWidth == 0 || m_nDrawHeight == 0)
		return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;

	if (m_pCCRRR == NULL)
	{
		CBaseVideoRender::CreateCCRRR ();
		UpdateSize ();
	}

	voCAutoLock lock (&m_csDraw);
	if (pRGBBuffer->Buffer[0] != NULL)
	{
		int nDispWidth = m_rcDisplay.right - m_rcDisplay.left;
		int nDispHeight = m_rcDisplay.bottom - m_rcDisplay.top;

		if (pRGBBuffer->Stride[0] == 0)
		{
		//	if (m_bRotate)
		//		pRGBBuffer->Stride[0] = nDispHeight * m_nBytesPixel;
		//	else
				pRGBBuffer->Stride[0] = nDispWidth * m_nBytesPixel;
		}

		if (!m_bRotate)
		{
			pRGBBuffer->Buffer[0] += (nDispHeight - m_nDrawHeight) * nDispWidth * m_nBytesPixel / 2 + (nDispWidth - m_nDrawWidth) * m_nBytesPixel / 2;
		}
		else
		{
			int nOffset = (nDispHeight - m_nDrawWidth) * nDispWidth * m_nBytesPixel / 2;
			nOffset += (nDispWidth - m_nDrawHeight) * m_nBytesPixel / 2;
			pRGBBuffer->Buffer[0] += nOffset;
		}
	}
	else
	{
		if (m_bufRGB565.Buffer[0] == NULL)
		{
			VO_MEM_INFO memInfo;
			memInfo.Size = m_nDrawWidth * m_nDrawHeight * m_nBytesPixel;
			m_pMemOP->Alloc (VO_INDEX_MFW_VOMMPLAY, &memInfo);

			m_bufRGB565.Buffer[0] = (VO_PBYTE) memInfo.VBuffer;

			if (m_bRotate)
				pRGBBuffer->Stride[0] = m_nDrawHeight * m_nBytesPixel;
			else
				pRGBBuffer->Stride[0] = m_nDrawWidth * m_nBytesPixel;
		}

		pRGBBuffer->Buffer[0] = m_bufRGB565.Buffer[0];
		pRGBBuffer->Stride[1] = m_nDrawWidth * m_nDrawHeight * m_nBytesPixel;	
	}

	VO_VIDEO_BUFFER nRGBBuffer;
	memset(&nRGBBuffer, 0, sizeof(VO_VIDEO_BUFFER));

	nRGBBuffer.Buffer[0]  = pRGBBuffer->Buffer[0];
	nRGBBuffer.Stride[0] = pRGBBuffer->Stride[0];

	if(m_bVideoUpsizeDown == VO_TRUE && m_bufRGBRevert.Buffer[0] == NULL)
	{
		VO_MEM_INFO memInfo;
		memInfo.Size = m_nDrawWidth * m_nDrawHeight * m_nBytesPixel;
		m_pMemOP->Alloc (VO_INDEX_MFW_VOMMPLAY, &memInfo);
		m_bufRGBRevert.Buffer[0] = (VO_PBYTE) memInfo.VBuffer;
		m_bufRGBRevert.Stride[0] = m_nDrawWidth * m_nBytesPixel;	
	}

	if(m_bVideoUpsizeDown == VO_TRUE)
	{
		nRGBBuffer.Buffer[0]  = m_bufRGBRevert.Buffer[0];
		nRGBBuffer.Stride[0] = m_bufRGBRevert.Stride[0];
	}

	if (!ConvertData (pVideoBuffer, &nRGBBuffer, 0, VO_TRUE))
	{
		VOLOGE ("ConvertData Error!");
		return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;
	}

	if(m_bVideoUpsizeDown == VO_TRUE && m_bufRGBRevert.Buffer[0]!= NULL)
	{   //TODO: if color conversion can support revert, the following codes should be deprecated 
		VO_U32 i;
		VO_BYTE *pSrc, *pDst;
		pDst = pRGBBuffer->Buffer[0];
		pSrc = m_bufRGBRevert.Buffer[0] + (m_nDrawHeight -1)*m_bufRGBRevert.Stride[0];

		for (i=0; i<(VO_U32)m_nDrawHeight; i++, pSrc -= m_bufRGBRevert.Stride[0], pDst += pRGBBuffer->Stride[0])
			memcpy(pDst, pSrc, pRGBBuffer->Stride[0]);
	}

	return VO_ERR_NONE;
}

VO_U32 CBaseVideoRender::WaitDone (void)
{
	return VO_ERR_NONE;
}

VO_U32 CBaseVideoRender::Redraw (void)
{
	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;
}

VO_U32 CBaseVideoRender::SetCallBack (VOVIDEOCALLBACKPROC pCallBack, VO_PTR pUserData)
{
	m_fCallBack = pCallBack;
	m_pUserData = pUserData;

	return VO_ERR_NONE;
}

VO_U32 CBaseVideoRender::SetEventCallBack (VOVIDEO_EVENT_CALLBACKPROC pCallBack, VO_PTR pUserData)
{
    m_fEventCallBack = pCallBack;
	m_pEventUserData = pUserData;
    
	return VO_ERR_NONE;
}

VO_U32 CBaseVideoRender::GetVideoMemOP (VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP)
{
	if (m_pOutMemOP != NULL)
	{
		*ppVideoMemOP = m_pOutMemOP;
		return VO_ERR_NONE;
	}

	if (m_pCCRRR == NULL)
		CreateCCRRR ();

	if (m_pCCRRR != NULL)
		m_pCCRRR->GetVideoMemOP (ppVideoMemOP);

	return VO_ERR_NONE;
}

VO_U32 CBaseVideoRender::SetVideoMemOP (VO_MEM_VIDEO_OPERATOR * pVideoMemOP)
{
	m_pOutMemOP = pVideoMemOP;

	return VO_ERR_NONE;
}

VO_U32 CBaseVideoRender::SetParam (VO_U32 nID, VO_PTR pValue)
{
	if (nID == VO_PID_VIDEO_DATABUFFER)
	{
		//VOLOGI ("ID %d : %d", VO_PID_VIDEO_DATABUFFER, pValue);
		memcpy (&m_bufOutput, pValue, sizeof (VO_VIDEO_BUFFER));
	}
	else if (nID == VO_PID_VIDEO_VIDEOMEMOP)
	{
		m_pOutMemOP = (VO_MEM_VIDEO_OPERATOR *)pValue;
	}
	else if (nID == VO_PID_COMMON_LIBOP)
	{
		m_pLibOP = (VO_LIB_OPERATOR *)pValue;
	}
	else if (nID == VO_PID_VIDEO_UPSIDEDOWN)
	{
		m_bVideoUpsizeDown = *(VO_BOOL*)pValue;
		if(m_pCCRRR != NULL)
			m_pCCRRR->SetParam(VO_PID_VIDEO_UPSIDEDOWN , &m_bVideoUpsizeDown);
	}
	return VO_ERR_WRONG_PARAM_ID | VO_INDEX_SNK_VIDEO;
}

VO_U32 CBaseVideoRender::GetParam (VO_U32 nID, VO_PTR pValue)
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

VO_U32 CBaseVideoRender::UpdateSize (void)
{
	voCAutoLock lock (&m_csDraw);

	if (m_nVideoWidth == 0 || m_nVideoHeight == 0)
		return 0;
#ifdef _METRO

#elif defined _WIN32
	m_nScreenWidth = ::GetSystemMetrics (SM_CXSCREEN);
	m_nScreenHeight = ::GetSystemMetrics (SM_CYSCREEN);
	if (m_rcDisplay.right == 0 || m_rcDisplay.bottom == 0)
		return 0;

#ifndef _WIN32_WCE
	m_nScreenWidth = 0;//m_rcDisplay.right - m_rcDisplay.left;
	m_nScreenHeight = 0;//m_rcDisplay.bottom - m_rcDisplay.top;;
#endif // _WIN32_WCE

#elif defined __SYMBIAN32__
	m_nScreenWidth = m_rcDisplay.right - m_rcDisplay.left;
	m_nScreenHeight = m_rcDisplay.bottom - m_rcDisplay.top;;
#elif defined _IOS
	m_nScreenWidth = m_rcDisplay.right - m_rcDisplay.left;
	m_nScreenHeight = m_rcDisplay.bottom - m_rcDisplay.top;
#elif defined _MAC_OS
	m_nScreenWidth = m_rcDisplay.right - m_rcDisplay.left;
	m_nScreenHeight = m_rcDisplay.bottom - m_rcDisplay.top;
	if(m_nScreenWidth==0 || m_nScreenHeight==0)
		return 0;
#elif defined(LINUX)
	getScreenSize(m_nScreenWidth, m_nScreenHeight);
	if (!m_bSetDispRect)
	{
		m_rcDisplay.left = 0;
		m_rcDisplay.top = 0;
		m_rcDisplay.right = m_nVideoWidth;
		m_rcDisplay.bottom = m_nVideoHeight;
	}
#endif // _WIN32

	//m_nVideoWidth = m_nVideoWidth / 2;
	//m_nVideoHeight = m_nVideoHeight;
	//m_nRatio = VO_RATIO_00;
	//m_nZoomMode = VO_ZM_FITWINDOW;
	//m_nScreenWidth = m_rcDisplay.right - m_rcDisplay.left;
	//m_nScreenHeight = m_rcDisplay.bottom - m_rcDisplay.top;

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
	
	// tag: added by Lin Jun 20110302 to fix issue when draw width less than height
	//int  nOriginalDrawHeight = m_nDrawHeight;
	bool bDrawWidthLessThanDrawHeight = m_nDrawWidth < m_nDrawHeight;
	if(bDrawWidthLessThanDrawHeight)
		m_nDrawHeight = m_nDrawWidth;
	//end

	// detect it is full screen or not
	if (m_nDrawWidth == m_nScreenWidth && m_nDrawHeight == m_nScreenHeight)
	{
		m_bFullScreen = VO_TRUE;

		// detect it need to rotate or not
		if (m_nRatio == VO_RATIO_00)
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
		else
		{
			if (nWndWidth >= nWndHeight)
			{
				m_bRotate = VO_FALSE;
			}
			else
			{
				m_bRotate = VO_TRUE;
			}
		}
	}

	if (m_nZoomMode == VO_ZM_ZOOMIN)
	{
		m_nShowLeft = m_rcDisplay.left;
		m_nShowTop = m_rcDisplay.top;

		if(m_nDrawWidth < m_nShowWidth)
		{
			m_nShowWidth = m_nDrawWidth;				
		}

		if(m_nDrawHeight < m_nShowHeight)
		{
			m_nShowHeight = m_nDrawHeight;				
		}
	}

	if (m_nZoomMode == VO_ZM_FITWINDOW)
	{
		if (!m_bRotate)
		{
			m_nDrawWidth = m_rcDisplay.right - m_rcDisplay.left;
			m_nDrawHeight = m_rcDisplay.bottom - m_rcDisplay.top;
		}
		else
		{
			m_nDrawWidth = m_rcDisplay.bottom - m_rcDisplay.top;
			m_nDrawHeight = m_rcDisplay.right - m_rcDisplay.left;
		}
	}
	else
	{
		// calculate the max draw width and height
		if (!m_bRotate)
		{
			m_nDrawWidth = nWndWidth;
			m_nDrawHeight = nWndHeight;
		}
		else
		{
			m_nDrawWidth = nWndHeight;
			m_nDrawHeight = nWndWidth;
		}

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
		else if(m_nRatio != VO_RATIO_00)
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

	if (!m_bRotate)
	{
		nWndWidth = m_rcDisplay.right - m_rcDisplay.left;
		nWndHeight = m_rcDisplay.bottom - m_rcDisplay.top;
	}
	else
	{
		nWndHeight = m_rcDisplay.right - m_rcDisplay.left;
		nWndWidth = m_rcDisplay.bottom - m_rcDisplay.top;
	}

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
	if (!m_bRotate)
	{
		m_nShowLeft = m_nShowLeft & ~7;
		m_nShowTop = m_nShowTop & ~7;

		m_nShowWidth =  m_nShowWidth & ~1;
		m_nShowHeight = m_nShowHeight & ~1;
		m_nDrawWidth = m_nDrawWidth & ~1;
		m_nDrawHeight = m_nDrawHeight & ~1;

		m_nDrawLeft = (m_nDrawLeft + (nWndWidth - m_nDrawWidth) / 2) & ~1;
		m_nDrawTop = (m_nDrawTop + (nWndHeight - m_nDrawHeight) / 2) & ~1;
	}
	else
	{
		m_nShowLeft = m_nShowLeft & ~7;
		m_nShowTop = m_nShowTop & ~7;

		m_nShowWidth =  m_nShowWidth & ~1;
		m_nShowHeight = m_nShowHeight & ~1;
		m_nDrawWidth = m_nDrawWidth & ~1;
		m_nDrawHeight = m_nDrawHeight & ~1;

		m_nDrawTop = (m_nDrawTop + (nWndWidth - m_nDrawWidth) / 2) & ~1;
		m_nDrawLeft = (m_nDrawLeft + (nWndHeight - m_nDrawHeight) / 2) & ~1;
	}

	if (m_pCCRRR == NULL)
		CreateCCRRR ();

	if (m_pCCRRR != NULL)
		m_pCCRRR->SetColorType (m_nVideoColor, m_nOutputColor);

	if (m_bRotate)
	{
		if (m_pCCRRR != NULL)
			m_pCCRRR->SetCCRRSize (&m_nShowWidth, &m_nShowHeight, &m_nDrawWidth, &m_nDrawHeight, m_bRotateType);
		if (m_pSoftCCRRR != NULL)
			m_pSoftCCRRR->SetCCRRSize (&m_nShowWidth, &m_nShowHeight, &m_nDrawWidth, &m_nDrawHeight, m_bRotateType);

		VOLOGI ("SetCCRRSize %d, %d, %d, %d  %d", (int)m_nShowWidth, (int)m_nShowHeight, (int)m_nDrawWidth, (int)m_nDrawHeight, (int)m_bRotateType);
	}
	else
	{
		if (m_pCCRRR != NULL)
			m_pCCRRR->SetCCRRSize (&m_nShowWidth, &m_nShowHeight, &m_nDrawWidth, &m_nDrawHeight, VO_RT_DISABLE);
		if (m_pSoftCCRRR != NULL)
			m_pSoftCCRRR->SetCCRRSize (&m_nShowWidth, &m_nShowHeight, &m_nDrawWidth, &m_nDrawHeight, VO_RT_DISABLE);

		VOLOGI ("SetCCRRSize %d, %d, %d, %d  %d", (int)m_nShowWidth, (int)m_nShowHeight, (int)m_nDrawWidth, (int)m_nDrawHeight, 0);
	}


	if (m_fCallBack != NULL)
	{
		if (m_bufRGB565.Buffer[0] != NULL)
			m_pMemOP->Free (VO_INDEX_MFW_VOMMPLAY, m_bufRGB565.Buffer[0]);
		m_bufRGB565.Buffer[0] = NULL;
		if (m_bufRGBRevert.Buffer[0] != NULL)
			m_pMemOP->Free (VO_INDEX_MFW_VOMMPLAY, m_bufRGBRevert.Buffer[0]);
		m_bufRGBRevert.Buffer[0] = NULL;
	}

	return 0;
}

bool CBaseVideoRender::ConvertData (VO_VIDEO_BUFFER * pInBuffer, VO_VIDEO_BUFFER * pOutBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	int nYOffset = 0;
	int nUVOffset = 0;

	if (pInBuffer->ColorType == VO_COLOR_YUV_PLANAR420)
	{
		nYOffset = m_nShowTop * pInBuffer->Stride[0] + m_nShowLeft;
		nUVOffset = (m_nShowTop / 2) * pInBuffer->Stride[1] + m_nShowLeft / 2;

		m_inData.Buffer[0] = pInBuffer->Buffer[0]  + nYOffset;
		m_inData.Buffer[1] = pInBuffer->Buffer[1]  + nUVOffset;
		m_inData.Buffer[2] = pInBuffer->Buffer[2]  + nUVOffset;

		m_inData.Stride[0] = pInBuffer->Stride[0];
		m_inData.Stride[1] = pInBuffer->Stride[1];
		m_inData.Stride[2] = pInBuffer->Stride[2];
	}
	else if (pInBuffer->ColorType == VO_COLOR_YUV_420_PACK_2 || pInBuffer->ColorType == VO_COLOR_YUV_420_PACK)
	{
		if (m_pUVData == NULL)
			m_pUVData = new VO_BYTE[m_nYUVWidth * m_nYUVHeight / 2];

		VO_PBYTE pUV = pInBuffer->Buffer[1];
		VO_PBYTE pU = m_pUVData;
		VO_PBYTE pV = pU + m_nYUVWidth * m_nYUVHeight / 4;

		if (pInBuffer->ColorType == VO_COLOR_YUV_420_PACK_2)
		{
			for (int i = 0; i < (int)(m_nVideoHeight / 2); i++)
			{
				pUV = pInBuffer->Buffer[1] + pInBuffer->Stride[1] * i;
				pU = m_pUVData + (m_nYUVWidth / 2) * i;
				pV = pU + m_nYUVWidth * m_nYUVHeight / 4;

				for (int w = 0; w < (int)(m_nVideoWidth / 2); w++)
				{
					*pV++ = *pUV++;
					*pU++ = *pUV++;
				}
			}
		}
		else if (pInBuffer->ColorType == VO_COLOR_YUV_420_PACK)
		{
			for (int i = 0; i < (int)(m_nVideoHeight / 2); i++)
			{
				pUV = pInBuffer->Buffer[1] + pInBuffer->Stride[1] * i;
				pU = m_pUVData + (m_nYUVWidth / 2) * i;
				pV = pU + m_nYUVWidth * m_nYUVHeight / 4;

				for (int w = 0; w < (int)(m_nVideoWidth / 2); w++)
				{
					*pU++ = *pUV++;
					*pV++ = *pUV++;
				}
			}
		}

		nYOffset = m_nShowTop * pInBuffer->Stride[0] + m_nShowLeft;
		nUVOffset = (m_nShowTop / 2) * pInBuffer->Stride[1] / 2 + m_nShowLeft / 2;

		m_inData.Buffer[0] = pInBuffer->Buffer[0]  + nYOffset;
		m_inData.Buffer[1] = m_pUVData + nUVOffset;
		m_inData.Buffer[2] = m_pUVData + m_nYUVWidth * m_nYUVHeight / 4  + nUVOffset;

		m_inData.Stride[0] = pInBuffer->Stride[0];
		m_inData.Stride[1] = m_nYUVWidth / 2;
		m_inData.Stride[2] = m_nYUVWidth / 2;
	}

	if (!m_bSoftForce && m_pCCRRR != NULL)
	{
		m_pCCRRR->Process (&m_inData, pOutBuffer, nStart, bWait);
	}
	else if (m_bSoftForce && m_pSoftCCRRR != NULL)
	{
		m_pSoftCCRRR->Process (&m_inData, pOutBuffer, nStart, bWait);
	}
	else if (m_bSoftForce && m_pCCRRR != NULL)
	{
		m_pCCRRR->Process (&m_inData, pOutBuffer, nStart, bWait);
	}

	return true;
}

void CBaseVideoRender::SetCPUVersion (int nVer)
{
	m_nCPUVer = nVer;
}

bool CBaseVideoRender::CreateCCRRR (void)
{
	voCAutoLock lock (&m_mutexCCRR);
	VOLOGI("m_pCCRRR: %p", m_pCCRRR);
	if (m_pCCRRR != NULL)
		return true;

#ifndef __SYMBIAN32__
#ifndef _MAC_OS
	if (m_pConfig == NULL)
	{
		m_pCfgFile= new CBaseConfig ();
		if (m_pCfgFile != NULL)
		{
			TCHAR szFileName[1024];
			memset(szFileName , 0 ,  sizeof(TCHAR) * 1024);
			vostrcpy(szFileName , m_pWorkPath);
			vostrcat(szFileName , _T("vommcodec.cfg"));

			VO_BOOL  bValue = m_pCfgFile->Open (szFileName);
			if(bValue == VO_FALSE)
			{
				delete m_pCfgFile;
				m_pCfgFile = NULL;
			}

			m_pConfig = m_pCfgFile;
		}
	}
#endif // _MAC_OS
#endif // __SYMBIAN32__

	m_pCCRRR = new CCCRRRFunc (VO_FALSE, m_pConfig);
	if (m_pCCRRR != NULL)
	{
		m_pCCRRR->SetLibOperator (m_pLibOP);
		m_pCCRRR->SetWorkPath (m_pWorkPath);
		m_pCCRRR->SetCPUVersion (m_nCPUVer);

		if (m_pCCRRR->LoadLib (m_hInst) > 0)
		{
			m_pCCRRR->Init (m_hView, m_pMemOP, 0);
			m_pCCRRR->SetParam(VOMP_PID_PLAYER_PATH , m_pWorkPath);
			VO_CCRRR_PROPERTY * pProp = m_pCCRRR->GetProp ();
			if (pProp->nOverlay)
			{
				m_pSoftCCRRR = new CCCRRRFunc (VO_FALSE, m_pConfig);
				m_pSoftCCRRR->Init (m_hView, m_pMemOP, 0);
				m_pSoftCCRRR->SetWorkPath (m_pWorkPath);
				m_pSoftCCRRR->SetLibOperator (m_pLibOP);
				m_pSoftCCRRR->SetCPUVersion (m_nCPUVer);
				m_pSoftCCRRR->LoadLib (m_hInst);
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

#if defined(LINUX)
bool CBaseVideoRender::getScreenSize(VO_U32& inOutWidth, VO_U32& inOutHeight)
{
    const char* dev[] = {
	"/dev/fb0",
	"/dev/fb1",
	"/dev/graphics/fb0",
	"/dev/graphics/fb1",
    };
    int nFd = -1;
    for(int n=0; n < (int)(sizeof(dev)/sizeof(dev[0])); n++)
    {
       nFd = open(dev[n],  O_RDONLY);
       if(nFd > 0)
          break;
    }
	if(nFd != -1)
	{
		struct fb_var_screeninfo info;
		ioctl(nFd, FBIOGET_VSCREENINFO, &info);
		inOutWidth = info.xres;
		inOutHeight = info.yres;
		close(nFd);
		return true;
	}
	inOutWidth = inOutHeight = 0;
	return false;
}
#endif
