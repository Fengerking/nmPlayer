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
#include "CGDIVideoRender.h"

#include "voLogoData.h"

#define LOG_TAG "CGDIVideoRender"
#include "voLog.h"

const DWORD bits565[3] = {0X0000F800, 0X000007E0, 0X0000001F,};

CGDIVideoRender::CGDIVideoRender(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
	: CBaseVideoRender (hInst, hView, pMemOP)
	, m_hWinDC(NULL)
	, m_hMemDC(NULL)
	, m_pBitmapBuffer (NULL)
	, m_pBmpInfo (NULL)
	, m_hBitmap (NULL)
	, m_hOldBmp (NULL)
	, m_nGDIWidth (0)
	, m_nGDIHeight (0)
	, m_pInputBuffer (NULL)
#if !defined __VOPRJ_INST__
	, m_nPixelBits (32)
#else
	, m_nPixelBits (32)
#endif
{
	memset (&m_outBuffer, 0, sizeof (VO_VIDEO_BUFFER));
	m_outBuffer.ColorType = VO_COLOR_RGB565_PACKED;
	if (m_nPixelBits == 24)
	{
		m_outBuffer.ColorType = VO_COLOR_RGB888_PACKED;
		m_nOutputColor = VO_COLOR_RGB888_PACKED;
	}
	else if (m_nPixelBits == 32)
	{
		m_outBuffer.ColorType = VO_COLOR_RGB32_PACKED;
		m_nOutputColor = VO_COLOR_RGB32_PACKED;
	}

	GetClientRect ((HWND) m_hView, (LPRECT)&m_rcDisplay);
	m_hWinDC = GetDC ((HWND) m_hView);
	m_hMemDC = ::CreateCompatibleDC (m_hWinDC);
}

CGDIVideoRender::~CGDIVideoRender ()
{
	if (m_hOldBmp != NULL)
		SelectObject (m_hMemDC, m_hOldBmp);

	if (m_pBmpInfo != NULL)
		delete []m_pBmpInfo;

	if (m_hBitmap)
		DeleteObject (m_hBitmap);

	if (m_hWinDC != NULL)
	{
		DeleteDC (m_hMemDC);
		ReleaseDC ((HWND)m_hView, m_hWinDC);
	}
}

VO_U32 CGDIVideoRender::Start (void)
{
	m_bSoftForce = VO_FALSE;
	ShowOverlay(VO_TRUE);

	return CBaseVideoRender::Start();
}

VO_U32 	CGDIVideoRender::Pause (void)
{
	m_bSoftForce = VO_TRUE;
	ShowOverlay(VO_FALSE);

	return CBaseVideoRender::Pause();
}

VO_U32 	CGDIVideoRender::Stop (void)
{
	m_bSoftForce = VO_TRUE;
	ShowOverlay(VO_FALSE);

	return CBaseVideoRender::Stop();
}

VO_U32 CGDIVideoRender::SetDispRect (VO_PTR hView, VO_RECT * pDispRect, VO_IV_COLORTYPE nColor)
{
	voCAutoLock lock (&m_csDraw);

	if (hView != NULL && hView != m_hView)
	{
		if (m_hOldBmp != NULL)
			SelectObject (m_hMemDC, m_hOldBmp);

		if (m_hWinDC != NULL)
		{
			DeleteDC (m_hMemDC);
			ReleaseDC ((HWND)m_hView, m_hWinDC);
		}

		m_hView = hView;

		m_hWinDC = GetDC ((HWND) m_hView);
		m_hMemDC = ::CreateCompatibleDC (m_hWinDC);

		VO_RECT				rcDisplay;
		GetClientRect ((HWND) m_hView, (LPRECT)&rcDisplay);
		memset (&m_rcDisplay, 0, sizeof (VO_RECT));
		return CBaseVideoRender::SetDispRect (hView, &rcDisplay, m_outBuffer.ColorType);
	}

	return CBaseVideoRender::SetDispRect (hView, pDispRect, m_outBuffer.ColorType);
}

VO_U32 CGDIVideoRender::Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	VOLOGR ();

	voCAutoLock lock (&m_csDraw);

	if (pVideoBuffer != NULL)
		m_pInputBuffer = pVideoBuffer;
	else
		pVideoBuffer = m_pInputBuffer;
	if (m_pInputBuffer == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	if (m_fCallBack != NULL)
		return CBaseVideoRender::Render (pVideoBuffer, nStart, bWait);

	VO_U32 nRC = 0;
	if (m_hBitmap == NULL)
	{
		if (m_pBmpInfo == NULL)
			CreateBitmapInfo ();

		m_hBitmap = CreateDIBSection(m_hWinDC , (BITMAPINFO *)m_pBmpInfo , DIB_RGB_COLORS , (void **)&m_pBitmapBuffer , NULL , 0);
		m_hOldBmp = (HBITMAP)SelectObject (m_hMemDC, m_hBitmap);
	}
	if (m_hBitmap == NULL || m_pBitmapBuffer == NULL)
		return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;

	m_outBuffer.Buffer[0] = m_pBitmapBuffer;
	m_outBuffer.Stride[0] = m_nGDIWidth * m_nPixelBits / 8;

	if (!ConvertData (pVideoBuffer, &m_outBuffer, nStart, bWait))
		return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;

  //fix bug:Stream's video is upside down (https://sh.visualon.com/node/5922)
  if(m_bVideoUpsizeDown == VO_TRUE&& m_outBuffer.Buffer[0]!= NULL)
  {   //TODO: if color conversion can support revert, the following codes should be deprecated
      int nStride = m_outBuffer.Stride[0];
      VO_BYTE *pSrc, *pDst;
      pSrc = m_outBuffer.Buffer[0]+ (m_nGDIHeight -1)*nStride;
      pDst = m_outBuffer.Buffer[0];

      VO_BYTE *pTemp = new VO_BYTE[nStride];
      for (VO_U32 i=0; i<(VO_U32)(m_nGDIHeight)/2; i++, pSrc -= nStride, pDst += nStride)
      {
          memcpy(pTemp, pDst, nStride);
          memcpy(pDst, pSrc, nStride);
          memcpy(pSrc, pTemp, nStride);
      }
      delete []pTemp;
  }

	if (m_bSoftForce || (m_pCCRRR != NULL && m_pCCRRR->GetProp ()->nRender == 0))
		BitBlt(m_hWinDC, m_nDrawLeft, m_nDrawTop, m_nGDIWidth, m_nGDIHeight, m_hMemDC, 0, 0, SRCCOPY);

	return VO_ERR_NONE;
}

VO_U32 CGDIVideoRender::Redraw (void)
{
	voCAutoLock lock (&m_csDraw);

	if (m_hBitmap != NULL)
	{
		if (m_hMemDC != NULL)
			BitBlt(m_hWinDC, m_nDrawLeft, m_nDrawTop, m_nGDIWidth, m_nGDIHeight, m_hMemDC, 0, 0, SRCCOPY);
	}
	else
	{
		Render (NULL, 0, VO_FALSE);
	}

	return VO_ERR_NONE;
}

VO_U32 CGDIVideoRender::UpdateSize (void)
{
#ifdef _WIN32
#ifndef _WIN32_WCE
	m_nScreenWidth = m_rcDisplay.right - m_rcDisplay.left;
	m_nScreenHeight = m_rcDisplay.bottom - m_rcDisplay.top;
#endif // _WIN32_WCE
#endif // _WIN32

	CBaseVideoRender::UpdateSize ();

	voCAutoLock lock (&m_csDraw);
	if (!m_bRotate)
	{
		m_nGDIWidth = m_nDrawWidth;
		m_nGDIHeight = m_nDrawHeight;
	}
	else
	{
		m_nGDIWidth = m_nDrawHeight;
		m_nGDIHeight = m_nDrawWidth;
	}

	if (m_hBitmap != NULL)
	{
		SelectObject (m_hMemDC, m_hOldBmp);
		DeleteObject (m_hBitmap);

		m_hOldBmp = NULL;
		m_hBitmap = NULL;
	}

	if (m_pBmpInfo != NULL)
	{
		delete []m_pBmpInfo;
		m_pBmpInfo = NULL;
	}

	if (m_pCCRRR != NULL)
		m_pCCRRR->SetColorType (VO_COLOR_YUV_PLANAR420, m_outBuffer.ColorType);
	if (m_pSoftCCRRR != NULL)
		m_pSoftCCRRR->SetColorType (VO_COLOR_YUV_PLANAR420, m_outBuffer.ColorType);

	CreateBitmapInfo ();

	EraseBackGround();
	return 0;
}

bool CGDIVideoRender::CreateBitmapInfo (void )
{
	int nBmpSize = sizeof(BITMAPINFOHEADER);
	if (m_nPixelBits == 16)
		nBmpSize += SIZE_MASKS; // for RGB bitMask;

	if (m_pBmpInfo == NULL)
		m_pBmpInfo = new BYTE[nBmpSize];
	memset (m_pBmpInfo, 0, nBmpSize);

	BITMAPINFO * pBmpInfo = (BITMAPINFO *)m_pBmpInfo;

	pBmpInfo->bmiHeader.biSize			= nBmpSize;

	pBmpInfo->bmiHeader.biWidth			= m_nGDIWidth;//m_rcDraw.right - m_rcDraw.left;
	pBmpInfo->bmiHeader.biHeight		= 0-m_nGDIHeight;//-(m_rcDraw.bottom - m_rcDraw.top);

	pBmpInfo->bmiHeader.biBitCount		= (WORD)m_nPixelBits;
	if (m_nPixelBits == 16)
		pBmpInfo->bmiHeader.biCompression	= BI_BITFIELDS;
	else
		pBmpInfo->bmiHeader.biCompression	= BI_RGB;
	pBmpInfo->bmiHeader.biXPelsPerMeter	= 0;
	pBmpInfo->bmiHeader.biYPelsPerMeter	= 0;

	if (m_nPixelBits == 16)
	{
		DWORD *	pBmiColors = (DWORD *)((LPBYTE)pBmpInfo + sizeof(BITMAPINFOHEADER));
		for (int i = 0; i < 3; i++)
    		*(pBmiColors + i) = bits565[i];
	}

	pBmpInfo->bmiHeader.biPlanes			= 1;

	int nStride = ((pBmpInfo->bmiHeader.biWidth * pBmpInfo->bmiHeader.biBitCount / 8) + 3) & ~3;

	pBmpInfo->bmiHeader.biSizeImage		= nStride * m_nGDIHeight;

	return true;
}

void	CGDIVideoRender::EraseBackGround()
{
	int wndWidth = m_rcDisplay.right - m_rcDisplay.left;
	int wndHeight = m_rcDisplay.bottom - m_rcDisplay.top;

	HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));

	RECT drawRect;
	if(wndWidth > m_nDrawWidth)
	{
		int nDrawWidth = (wndWidth - m_nDrawWidth) / 2 ; 
		nDrawWidth = (nDrawWidth + 1) / 2 * 2;
		if(nDrawWidth > 0)
		{
			drawRect.top = m_rcDisplay.top;
			drawRect.bottom = m_rcDisplay.bottom;
			drawRect.left = m_rcDisplay.left;
			drawRect.right = drawRect.left + nDrawWidth + 2;

			FillRect((HDC)m_hWinDC, &drawRect, hBrush);

			drawRect.left = m_rcDisplay.right - nDrawWidth - 2;
			drawRect.right = m_rcDisplay.right;

			FillRect((HDC)m_hWinDC, &drawRect, hBrush);
		}
	}

	if(wndHeight > m_nDrawHeight)
	{
		int nDrawHeight = (wndHeight - m_nDrawHeight) / 2;
		nDrawHeight = (nDrawHeight + 1) / 2 * 2;
		if(nDrawHeight > 0)
		{
			drawRect.top = m_rcDisplay.top;
			drawRect.bottom =  drawRect.top + nDrawHeight + 2;
			drawRect.left = m_rcDisplay.left;
			drawRect.right = m_rcDisplay.right;

			FillRect((HDC)m_hWinDC, &drawRect, hBrush);

			drawRect.top =m_rcDisplay.bottom -  nDrawHeight -2 ;
			drawRect.bottom =  m_rcDisplay.bottom;

			FillRect((HDC)m_hWinDC, &drawRect, hBrush);
		}
	}

	DeleteObject(hBrush);
}

VO_U32 CGDIVideoRender::ShowOverlay(VO_BOOL bShow)
{
	if(m_pCCRRR)
	{
		return m_pCCRRR->SetParam(VO_CCRRR_PMID_ShowOverlay , (VO_PTR)&bShow);
	}

	return VO_ERR_FAILED;
}

VO_U32			voVRMemInit (VO_S32 uID, VO_MEM_VIDEO_INFO * pVideoMem);
VO_U32			voVRMemGetBuf (VO_S32 uID, VO_S32 nIndex);
VO_U32			voVRMemUninit (VO_S32 uID);

VO_MEM_VIDEO_OPERATOR	g_vmVROP;
VO_MEM_VIDEO_INFO		g_vmVRInfo;

VO_U32					g_yOffset;

VO_U32 CGDIVideoRender::GetVideoMemOP (VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP)
{
	return CBaseVideoRender::GetVideoMemOP (ppVideoMemOP);

	memset (&g_vmVRInfo, 0, sizeof (VO_MEM_VIDEO_INFO));

	g_vmVROP.Init = voVRMemInit;
	g_vmVROP.GetBufByIndex = voVRMemGetBuf;
	g_vmVROP.Uninit = voVRMemUninit;

	*ppVideoMemOP = &g_vmVROP;
	
	return VO_ERR_NONE;
}

VO_U32 voVRMemInit (VO_S32 uID, VO_MEM_VIDEO_INFO * pVideoMem)
{
	memcpy (&g_vmVRInfo, pVideoMem, sizeof (VO_MEM_VIDEO_INFO));

	g_vmVRInfo.PBuffer = NULL;

	VO_VIDEO_BUFFER * pBuffers =  new VO_VIDEO_BUFFER[g_vmVRInfo.FrameCount];

	for (int i = 0; i < g_vmVRInfo.FrameCount; i++)
	{
		int yStride = g_vmVRInfo.Stride;
		g_yOffset = 0;

		pBuffers[i].Buffer[0] = new BYTE[yStride * (g_vmVRInfo.Height)];
		pBuffers[i].Buffer[0] += g_yOffset;

		pBuffers[i].Buffer[1] = new BYTE[g_vmVRInfo.Stride * g_vmVRInfo.Height / 4];
		pBuffers[i].Buffer[2] = new BYTE[g_vmVRInfo.Stride * g_vmVRInfo.Height / 4];

		pBuffers[i].Stride[0] = yStride;
		pBuffers[i].Stride[1] = g_vmVRInfo.Stride/2;
		pBuffers[i].Stride[2] = g_vmVRInfo.Stride/2;
	}

	g_vmVRInfo.VBuffer = pBuffers;
	pVideoMem->VBuffer = pBuffers;

	return 0;
}

VO_U32 voVRMemGetBuf (VO_S32 uID, VO_S32 nIndex)
{
	if (nIndex < 0 || nIndex >= g_vmVRInfo.FrameCount)
		return VO_ERR_FAILED;

	return VO_ERR_NONE;
}

VO_U32 voVRMemUninit (VO_S32 uID)
{
	if (g_vmVRInfo.VBuffer == NULL)
		return 0;

	for (int i = 0; i < g_vmVRInfo.FrameCount; i++)
	{
		delete [](g_vmVRInfo.VBuffer[i].Buffer[0] - g_yOffset); 
		delete []g_vmVRInfo.VBuffer[i].Buffer[1];
		delete []g_vmVRInfo.VBuffer[i].Buffer[2];
	}

	delete []g_vmVRInfo.VBuffer;
	g_vmVRInfo.VBuffer = NULL;

	return 0;
}
