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
#include "cmnMemory.h"
#include "CBaseVideoRender.h"

#if defined(LINUX)
#  include <unistd.h>
#  include <linux/fb.h>
#  include <sys/ioctl.h>
#  include <fcntl.h>
#endif

//#define _VOLOG_STATUS
//#define _VOLOG_RUN
//#define _VOLOG_FUNC

#include "voLog.h"

CBaseVideoRender::CBaseVideoRender(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
	: m_hInst (hInst)
	, m_pMemOP (pMemOP)
	, m_hView (hView)
	, m_pConfig (NULL)
	, m_pCCRRR (NULL)
	, m_pSoftCCRRR (NULL)
	, m_bSoftForce (VO_FALSE)
	, m_nVideoWidth (0)
	, m_nVideoHeight (0)
	, m_nVideoColor (VO_COLOR_YUV_PLANAR420)
	, m_bSetDispRect (VO_FALSE)
	, m_nZoomMode (VO_ZM_LETTERBOX)
	, m_nRatio (VO_RATIO_11)
	, m_fCallBack (NULL)
	, m_pUserData (NULL)
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
	, m_bRotate (VO_FALSE)
	, m_bFullScreen (VO_FALSE)
	, m_bRotateType (VO_RT_90L)
	, m_fPlayCallBack (NULL)
	, m_pUVData (NULL)
	, m_pOutMemOP (NULL)
	, m_pLibOP (NULL)
	, m_pWorkPath (NULL)
{
	if (m_pMemOP == NULL)
	{
		cmnMemFillPointer (VO_INDEX_SNK_AUDIO);
		m_pMemOP = &g_memOP;
	}

	memset (&m_bufRGB565, 0, sizeof (VO_VIDEO_BUFFER));
	m_bufRGB565.ColorType = VO_COLOR_RGB565_PACKED;

	memset (&m_bufOutput, 0, sizeof (VO_VIDEO_BUFFER));

	memset (&m_rcDisplay, 0, sizeof (VO_RECT));
	m_bSetDispRect = VO_FALSE;

	VOLOGF ();
}

CBaseVideoRender::~CBaseVideoRender ()
{
	VOLOGF ();

	if (m_pCCRRR != NULL)
		delete m_pCCRRR;
	m_pCCRRR = NULL;
	if (m_pSoftCCRRR != NULL)
		delete m_pSoftCCRRR;
	m_pSoftCCRRR = NULL;

	if (m_bufRGB565.Buffer[0] != NULL)
		m_pMemOP->Free (VO_INDEX_MFW_VOMMPLAY, m_bufRGB565.Buffer[0]);

	if (m_pUVData != NULL)
		delete []m_pUVData;

	if (m_pConfig != NULL)
		delete m_pConfig;
	m_pConfig = NULL;
}

VO_U32 CBaseVideoRender::SetVideoInfo (VO_U32 nWidth, VO_U32 nHeight, VO_IV_COLORTYPE nColor)
{
	VOLOGF ();

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

VO_U32 CBaseVideoRender::SetDispRect (VO_PTR hView, VO_RECT * pDispRect)
{
	VOLOGF ();

	m_hView = hView;

	if (pDispRect == NULL)
		return VO_ERR_NONE;

	if (pDispRect->bottom <= 0 || pDispRect->right <= 0)
		return VO_ERR_NONE;

	if (m_rcDisplay.left != pDispRect->left || m_rcDisplay.top != pDispRect->top ||
		m_rcDisplay.right != pDispRect->right || m_rcDisplay.bottom != pDispRect->bottom)
	{
		m_rcDisplay.left = pDispRect->left;
		m_rcDisplay.top = pDispRect->top;
		m_rcDisplay.right = pDispRect->right;
		m_rcDisplay.bottom = pDispRect->bottom;

		m_bSetDispRect = VO_TRUE;

		UpdateSize ();
	}

	return VO_ERR_NONE;
}

VO_U32 CBaseVideoRender::SetDispType (VO_IV_ZOOM_MODE nZoomMode, VO_IV_ASPECT_RATIO nRatio)
{
	VOLOGF ();

	if (m_nZoomMode != nZoomMode || m_nRatio != nRatio)
	{
		m_nZoomMode = nZoomMode;
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
		VO_U32		i = 0;

		m_csDraw.Lock ();
		m_fmtVideo.Width = m_nShowWidth;
		m_fmtVideo.Height = m_nShowHeight;
		nRC = m_fCallBack (m_pUserData, pVideoBuffer, &m_fmtVideo, (VO_S32)nStart);
		m_csDraw.Unlock ();

		//VOLOGI ("Draw Result : %d", nRC);

		if (nRC == VO_ERR_FINISH)
			return VO_ERR_NONE;

		//VOLOGI ("Draw W X H %d, %d", m_nDrawWidth, m_nDrawHeight);

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

		nRC = m_fCallBack (m_pUserData, pRGBBuffer, &m_fmtVideo, (VO_S32) nStart);

		if (nRC == VO_ERR_FINISH)
			return VO_ERR_NONE;
	}

	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;
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

VO_U32 CBaseVideoRender::GetVideoMemOP (VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP)
{
	VOLOGF ();

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
	VOLOGF ();

	if (nID == 1001)
	{
		m_fPlayCallBack = (VOMMPlayCallBack) pValue;
	}
	else if (nID == 1002)
	{
		m_pPlayUserData = pValue;
	}
	else if (nID == VO_PID_VIDEO_DATABUFFER)
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
	return VO_ERR_WRONG_PARAM_ID | VO_INDEX_SNK_VIDEO;
}

VO_U32 CBaseVideoRender::GetParam (VO_U32 nID, VO_PTR pValue)
{
	VOLOGF ();

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
	VOLOGF ();

	voCAutoLock lock (&m_csDraw);

	if (m_nVideoWidth == 0 || m_nVideoHeight == 0)
		return 0;

#ifdef _WIN32
	m_nScreenWidth = ::GetSystemMetrics (SM_CXSCREEN);
	m_nScreenHeight = ::GetSystemMetrics (SM_CYSCREEN);
	if (m_rcDisplay.right == 0 || m_rcDisplay.bottom == 0)
		return 0;

#ifndef _WIN32_WCE
	m_nScreenWidth = m_rcDisplay.right - m_rcDisplay.left;
	m_nScreenHeight = m_rcDisplay.bottom - m_rcDisplay.top;;
#endif // _WIN32_WCE

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
	//m_nRatio = VO_RATIO_11;
	//m_nZoomMode = VO_ZM_ORIGINAL;
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

	// detect it is full screen or not
	if (m_nDrawWidth == m_nScreenWidth && m_nDrawHeight == m_nScreenHeight)
	{
		m_bFullScreen = VO_TRUE;

		// detect it need to rotate or not
		if (m_nRatio == VO_RATIO_11)
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

		VO_U32	nW = 4;
		VO_U32	nH = 3;

		if (m_nRatio == VO_RATIO_43)
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

		// if ratio is VO_RATIO_11 calculate the draw depend on the video size.
		if (m_nRatio == VO_RATIO_11)
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
		m_nShowLeft = m_nShowLeft & ~15;
		m_nShowTop = m_nShowTop & ~15;

		m_nShowWidth =  m_nShowWidth & ~3;
		m_nShowHeight = m_nShowHeight & ~1;
		m_nDrawWidth = m_nDrawWidth & ~3;
		m_nDrawHeight = m_nDrawHeight & ~1;

		m_nDrawLeft = (m_nDrawLeft + (nWndWidth - m_nDrawWidth) / 2) & ~1;
		m_nDrawTop = (m_nDrawTop + (nWndHeight - m_nDrawHeight) / 2) & ~1;
	}
	else
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

	if (m_pCCRRR == NULL)
		CreateCCRRR ();

	if (m_bRotate)
	{
		if (m_pCCRRR != NULL)
			m_pCCRRR->SetCCRRSize (&m_nShowWidth, &m_nShowHeight, &m_nDrawWidth, &m_nDrawHeight, m_bRotateType);
		if (m_pSoftCCRRR != NULL)
			m_pSoftCCRRR->SetCCRRSize (&m_nShowWidth, &m_nShowHeight, &m_nDrawWidth, &m_nDrawHeight, m_bRotateType);
	}
	else
	{
		if (m_pCCRRR != NULL)
			m_pCCRRR->SetCCRRSize (&m_nShowWidth, &m_nShowHeight, &m_nDrawWidth, &m_nDrawHeight, VO_RT_DISABLE);
		if (m_pSoftCCRRR != NULL)
			m_pSoftCCRRR->SetCCRRSize (&m_nShowWidth, &m_nShowHeight, &m_nDrawWidth, &m_nDrawHeight, VO_RT_DISABLE);
	}

	if (m_fCallBack != NULL)
	{
		if (m_bufRGB565.Buffer[0] != NULL)
			m_pMemOP->Free (VO_INDEX_MFW_VOMMPLAY, m_bufRGB565.Buffer[0]);
		m_bufRGB565.Buffer[0] = NULL;
	}

	return 0;
}

bool CBaseVideoRender::ConvertData (VO_VIDEO_BUFFER * pInBuffer, VO_VIDEO_BUFFER * pOutBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	VOLOGR ();

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
	else if (pInBuffer->ColorType == VO_COLOR_YUV_420_PACK_2)
	{
		if (m_pUVData == NULL)
			m_pUVData = new VO_BYTE[m_nYUVWidth * m_nYUVHeight / 2];

		VO_PBYTE pUV = pInBuffer->Buffer[1];
		VO_PBYTE pU = m_pUVData;
		VO_PBYTE pV = pU + m_nYUVWidth * m_nYUVHeight / 4;

		for (int i = 0; i < m_nVideoHeight / 2; i++)
		{
			pUV = pInBuffer->Buffer[1] + pInBuffer->Stride[1] * i;
			pU = m_pUVData + (m_nYUVWidth / 2) * i;
			pV = pU + m_nYUVWidth * m_nYUVHeight / 4;

			for (int w = 0; w < m_nVideoWidth / 2; w++)
			{
				*pV++ = *pUV++;
				*pU++ = *pUV++;
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

bool CBaseVideoRender::CreateCCRRR (void)
{
	VOLOGF ();

	if (m_pCCRRR != NULL)
		return true;

	if (m_pConfig == NULL)
		m_pConfig= new CBaseConfig ();
	if (m_pConfig != NULL)
		m_pConfig->Open (_T("vommcodec.cfg"));

	m_pCCRRR = new CCCRRRFunc (VO_FALSE, m_pConfig);
	if (m_pCCRRR != NULL)
	{
		m_pCCRRR->SetLibOperator (m_pLibOP);
		//m_pCCRRR->SetWorkPath (m_pWorkPath);

		if (m_pCCRRR->LoadLib (m_hInst) > 0)
		{
			m_pCCRRR->Init (m_hView, m_pMemOP, 0);
			VO_CCRRR_PROPERTY * pProp = m_pCCRRR->GetProp ();
			if (pProp->nOverlay)
			{
				m_pSoftCCRRR = new CCCRRRFunc (VO_FALSE, m_pConfig);
				m_pSoftCCRRR->Init (m_hView, m_pMemOP, 0);
				//m_pSoftCCRRR->SetWorkPath (m_pWorkPath);
				m_pSoftCCRRR->SetLibOperator (m_pLibOP);
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
