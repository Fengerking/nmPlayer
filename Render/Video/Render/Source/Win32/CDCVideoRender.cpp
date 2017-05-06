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
#include "voCCRRR.h"
#include "CDCVideoRender.h"

#define LOG_TAG "CDCVideoRender"
#include "voLog.h"

// disable it if you want to use VisualOn SW scaler
//#define VO_USE_SYSTEM_SCALER
// disable it if you want to get good quality for system scaler
#define VO_USE_SYSTEM_SCALER_FASTMODE

const DWORD bits565[3] = {0X0000F800, 0X000007E0, 0X0000001F,};

CDCVideoRender::CDCVideoRender(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
	: CBaseVideoRender (hInst, hView, pMemOP)
	, m_hWinDC(NULL)
	, m_hMemDC(NULL)
	, m_nGDIWidth (0)
	, m_nGDIHeight (0)
	, m_pInputBuffer (NULL)
	, m_pBmpInfo(NULL)
	, m_hOrigBitmap(NULL)
	, m_hPreBitmap(NULL)
	, m_nPixelBits (32)
	, m_bAspectRatioChanged(false)
	, m_hBlackBrush(NULL)
	, m_bRendering(false)
	, m_bIsPaused(VO_FALSE)
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

	::GetClientRect((HWND)m_hView, (LPRECT)&m_rcDisplay);
	m_hMemDC = ::CreateCompatibleDC(NULL);

	memset(&m_sVRData, 0, sizeof(m_sVRData));

    m_iLastOkBuffer = GL_INVAILD;
    m_iUsingBuffer = GL_INVAILD;
    m_bGetNewBuffer = false;
    m_bIsLastInWriting = false;

	memset(&m_sPausedVRData, 0, sizeof(m_sPausedVRData));
}

CDCVideoRender::~CDCVideoRender ()
{
	deletePausedVRData();
	deleteBuffer();

	::DeleteDC(m_hMemDC);

	if (m_hWinDC != NULL)
	{	
		// david @ 20130729
		// this destroyed DC should be erased with black, otherwise it will keep the last video frame, thus background will flash with this residual video
		::FillRect(m_hWinDC, (LPRECT)&m_rcDisplay, m_hBlackBrush);
	}

	if (m_hBlackBrush)
		::DeleteObject(m_hBlackBrush);
}

VO_BOOL CDCVideoRender::reInitVideoBuffer()
{
    voCAutoLock lock (&m_cBufferMutex);

    while (m_bIsLastInWriting)
        voOS_Sleep(1);

    deleteBuffer();

    if ((0 == m_nVideoWidth) || (0 == m_nVideoHeight))
        return VO_FALSE;

	CreateBitmapInfo();

	for(int i = 0; i < GL_BUFFER_COUNT; i++)
	{
		m_sVRData[i].hBitmap = ::CreateDIBSection(NULL, (BITMAPINFO *)m_pBmpInfo, DIB_RGB_COLORS, (void **)&m_sVRData[i].pBitmapBuffer, NULL, 0);
		if(!m_sVRData[i].hBitmap)
			return VO_FALSE;
	}

	return VO_TRUE;
}

void CDCVideoRender::deleteBuffer()
{
    voCAutoLock lock (&m_cBufferMutex);

    m_iLastOkBuffer = GL_INVAILD;
    m_iUsingBuffer = GL_INVAILD;
    m_bGetNewBuffer = false;

	{
		voCAutoLock lockDC (&m_cDCMutex);
		if(m_hOrigBitmap)
		{
			::SelectObject(m_hMemDC, m_hOrigBitmap);
			m_hOrigBitmap = NULL;
		}

		m_hPreBitmap = NULL;
	}

	for(int i = 0; i < GL_BUFFER_COUNT; i++)
	{
		if(m_sVRData[i].hBitmap)
		{
			::DeleteObject(m_sVRData[i].hBitmap);
			m_sVRData[i].hBitmap = NULL;
		}

		m_sVRData[i].pBitmapBuffer = NULL;
	}

	if(m_pBmpInfo)
	{
		delete [] m_pBmpInfo;
		m_pBmpInfo = NULL;
	}
}

unsigned char* CDCVideoRender::lockFrameData()
{
    voCAutoLock lock (&m_cBufferMutex);

    int iIndexToLock = 0;
    if (0 == m_iLastOkBuffer)
        iIndexToLock = 1;

    if (NULL == m_sVRData[iIndexToLock].pBitmapBuffer)
        return NULL;

    // Render is slower than buffer writer, return NULL if next buffer still using
    if ((GL_INVAILD != m_iUsingBuffer) && (m_iUsingBuffer == iIndexToLock))
        return NULL;

    m_bIsLastInWriting = true;
    return m_sVRData[iIndexToLock].pBitmapBuffer;
}

void CDCVideoRender::unlockFrameData(bool bWriteOk,unsigned char* unlockBuffer)
{
    if (NULL == unlockBuffer)
        return;

	if (bWriteOk)
	{
		switch (m_iLastOkBuffer)
		{
		case 0:
			if (unlockBuffer == m_sVRData[1].pBitmapBuffer)
				m_iLastOkBuffer = 1;
			break;

		case GL_INVAILD:
		case 1:
		default:
			if (unlockBuffer == m_sVRData[0].pBitmapBuffer)
				m_iLastOkBuffer = 0;
			break;
		}
	}

    m_bGetNewBuffer = true;
    m_bIsLastInWriting = false;
}

VO_BOOL CDCVideoRender::savePausedVRData(VO_U32 nWidth, VO_U32 nHeight)
{
	voCAutoLock lock (&m_cBufferMutex);

	if (GL_INVAILD == m_iLastOkBuffer)
	{
		// we can use old saved VR data
		if(m_sPausedVRData.hBitmap)
			return VO_TRUE;

		return VO_FALSE;
	}

	if(m_nPausedVideoWidth != nWidth || m_nPausedVideoHeight != nHeight)
	{
		deletePausedVRData();

		m_nPausedVideoWidth = nWidth;
		m_nPausedVideoHeight = nHeight;

		// here m_pBmpInfo should use old GDI width & height, otherwise we need be careful
		m_sPausedVRData.hBitmap = ::CreateDIBSection(NULL, (BITMAPINFO *)m_pBmpInfo, DIB_RGB_COLORS, (void **)&m_sPausedVRData.pBitmapBuffer, NULL, 0);
		if(!m_sPausedVRData.hBitmap)
			return VO_FALSE;
	}

	m_iUsingBuffer = m_iLastOkBuffer;

	memcpy(m_sPausedVRData.pBitmapBuffer, m_sVRData[m_iUsingBuffer].pBitmapBuffer, nWidth * nHeight * m_nPixelBits / 8);

	m_iUsingBuffer = GL_INVAILD;

	return VO_TRUE;
}

void CDCVideoRender::deletePausedVRData()
{
	voCAutoLock lock (&m_cBufferMutex);

	if(NULL == m_sPausedVRData.hBitmap)
		return;

	m_nPausedVideoWidth = 0;
	m_nPausedVideoHeight = 0;

	::DeleteObject(m_sPausedVRData.hBitmap);
	m_sPausedVRData.hBitmap = NULL;
	m_sPausedVRData.pBitmapBuffer = NULL;
}

VO_U32 CDCVideoRender::SetVideoInfo (VO_U32 nWidth, VO_U32 nHeight, VO_IV_COLORTYPE nColor)
{
    VO_U32 nRet = VO_ERR_NONE;
    if ((m_nVideoWidth != nWidth) || (m_nVideoHeight != nHeight) || (m_nVideoColor != nColor))
    {
        nRet = CBaseVideoRender::SetVideoInfo(nWidth, nHeight, nColor);
#ifdef VO_USE_SYSTEM_SCALER
		reInitVideoBuffer();
#endif	// VO_USE_SYSTEM_SCALER
    }
	
    return nRet;
}

VO_U32 CDCVideoRender::Start (void)
{
	m_bSoftForce = VO_FALSE;
	m_bIsPaused = VO_FALSE;
	ShowOverlay(VO_TRUE);

	return CBaseVideoRender::Start();
}

VO_U32 	CDCVideoRender::Pause (void)
{
	m_bSoftForce = VO_TRUE;
	m_bIsPaused = VO_TRUE;
	ShowOverlay(VO_FALSE);

	return CBaseVideoRender::Pause();
}

VO_U32 	CDCVideoRender::Stop (void)
{
	m_bSoftForce = VO_TRUE;
	m_bIsPaused = VO_FALSE;
	ShowOverlay(VO_FALSE);

	return CBaseVideoRender::Stop();
}

VO_U32 CDCVideoRender::SetDispRect (VO_PTR hView, VO_RECT * pDispRect, VO_IV_COLORTYPE nColor)
{
	voCAutoLock lock (&m_csDraw);
    voCAutoLock lockDC (&m_cDCMutex);

	if (m_hBlackBrush == NULL)
		m_hBlackBrush = ::CreateSolidBrush(RGB(0,0,0));

	return CBaseVideoRender::SetDispRect (hView, pDispRect, m_outBuffer.ColorType);
}

VO_U32 CDCVideoRender::Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	voCAutoLock lock (&m_csDraw);

	if (m_fCallBack != NULL)
		return CBaseVideoRender::Render (pVideoBuffer, nStart, bWait);

    if (pVideoBuffer != NULL)
      m_pInputBuffer = pVideoBuffer;
    else
      pVideoBuffer = m_pInputBuffer;
    if (m_pInputBuffer == NULL)
      return VO_ERR_NOT_IMPLEMENT;

	m_bRendering = true;

    VO_U32 nRC = 0;
    m_outBuffer.Buffer[0] = (VO_BYTE*)lockFrameData();
    int iTry = 0;
    while (NULL == m_outBuffer.Buffer[0])
	{
        ++iTry;
        if (iTry > 15)
		{
            VOLOGW("******************LockFrameData LockFrameData fail!!");
            // Don't need unlock if you havn't got any buffer.
			m_bRendering = false;
            return VO_ERR_NONE;
        }
        voOS_Sleep(2);
        m_outBuffer.Buffer[0] = (VO_BYTE*)lockFrameData();
    }

#ifdef VO_USE_SYSTEM_SCALER
	m_outBuffer.Stride[0] = m_nVideoWidth * m_nPixelBits / 8;
#else	// VO_USE_SYSTEM_SCALER
	m_outBuffer.Stride[0] = m_nGDIWidth * m_nPixelBits / 8;
#endif	// VO_USE_SYSTEM_SCALER

    if (!ConvertData (pVideoBuffer, &m_outBuffer, nStart, bWait))
	{
        unlockFrameData(false, m_outBuffer.Buffer[0]);
		m_bRendering = false;
        return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;
    }

    //fix bug:Stream's video is upside down (https://sh.visualon.com/node/5922)
    if(m_bVideoUpsizeDown == VO_TRUE&& m_outBuffer.Buffer[0]!= NULL)
    { 
		//TODO: if color conversion can support revert, the following codes should be deprecated
		int nStride = m_outBuffer.Stride[0];
#ifdef VO_USE_SYSTEM_SCALER
		VO_PBYTE pSrc = m_outBuffer.Buffer[0] + (m_nVideoHeight - 1) * nStride;
#else	// VO_USE_SYSTEM_SCALER
		VO_PBYTE pSrc = m_outBuffer.Buffer[0] + (m_nGDIHeight - 1) * nStride;
#endif	// VO_USE_SYSTEM_SCALER
		VO_PBYTE pDst = m_outBuffer.Buffer[0];

		VO_PBYTE pTemp = new VO_BYTE[nStride];
#ifdef VO_USE_SYSTEM_SCALER
		for (VO_U32 i = 0; i < (VO_U32)(m_nVideoHeight) / 2; i++, pSrc -= nStride, pDst += nStride)
#else	// VO_USE_SYSTEM_SCALER
		for (VO_U32 i = 0; i < (VO_U32)(m_nGDIHeight) / 2; i++, pSrc -= nStride, pDst += nStride)
#endif	// VO_USE_SYSTEM_SCALER
		{
			memcpy(pTemp, pDst, nStride);
			memcpy(pDst, pSrc, nStride);
			memcpy(pSrc, pTemp, nStride);
		}
		delete [] pTemp;
    }

// 	if (m_bSoftForce || (m_pCCRRR != NULL && m_pCCRRR->GetProp ()->nRender == 0))
// 	{
// 	}

    unlockFrameData(true, m_outBuffer.Buffer[0]);

	m_bCanRedraw = VO_TRUE;
	if(m_bBlockEraseBackGround)
		m_bBlockEraseBackGround = VO_FALSE;

	m_bRendering = false;
	return VO_ERR_NONE;
}

VO_U32 CDCVideoRender::Redraw (void)
{
	voCAutoLock lock (&m_cDCMutex);

    VO_U32 nRet = VO_ERR_NONE;

    if ((NULL == m_hWinDC) || (NULL == m_hMemDC))
        return VO_ERR_WRONG_STATUS;

	if (m_bAspectRatioChanged)
	{
		VOLOGI("here erasing background!!");
		EraseBackGround();
		m_bAspectRatioChanged = false;
	}

	// here we need wait for Render function if it is working
	while(!m_bGetNewBuffer && m_bRendering)
		voOS_Sleep(2);

	VO_BOOL	bUsePausedVRData = VO_FALSE;
	HBITMAP hCurBitmap = NULL;
    {
        voCAutoLock lock (&m_cBufferMutex);
        if (GL_INVAILD == m_iLastOkBuffer)
		{
			if(m_sPausedVRData.hBitmap)
			{
				bUsePausedVRData = VO_TRUE;
				hCurBitmap = m_sPausedVRData.hBitmap;
			}
			else
			{
				VOLOGW("no any bitmap can be drawn!!");

				return VO_ERR_WRONG_STATUS;
			}
		}
		else
		{
			// if we have new valid VR data, we can delete saved VR data
			deletePausedVRData();

			m_iUsingBuffer = m_iLastOkBuffer;
			m_bGetNewBuffer = false;

			hCurBitmap = m_sVRData[m_iUsingBuffer].hBitmap;
		}
    }

	if(hCurBitmap != m_hPreBitmap)
	{
		HBITMAP hOldBitmap = (HBITMAP)::SelectObject(m_hMemDC, hCurBitmap);
		m_hPreBitmap = hCurBitmap;
		if(NULL == m_hOrigBitmap)
			m_hOrigBitmap = hOldBitmap;
	}

#ifdef VO_USE_SYSTEM_SCALER
	::StretchBlt(m_hWinDC, m_nDrawLeft, m_nDrawTop, m_nGDIWidth, m_nGDIHeight, m_hMemDC, 0, 0, m_nVideoWidth, m_nVideoHeight, SRCCOPY);
#else	// VO_USE_SYSTEM_SCALER
	if(bUsePausedVRData)
		::StretchBlt(m_hWinDC, m_nDrawLeft, m_nDrawTop, m_nGDIWidth, m_nGDIHeight, m_hMemDC, 0, 0, m_nPausedVideoWidth, m_nPausedVideoHeight, SRCCOPY);
	else
		::BitBlt(m_hWinDC, m_nDrawLeft, m_nDrawTop, m_nGDIWidth, m_nGDIHeight, m_hMemDC, 0, 0, SRCCOPY);
#endif	// VO_USE_SYSTEM_SCALER

    m_iUsingBuffer = GL_INVAILD;

#ifdef _DRAW_RENDER_INFO
	VO_U32 nCurrentSysTime = voOS_GetSysTime();

	// set text color settings
	COLORREF clrOldBk = ::SetBkColor(m_hWinDC, RGB(0, 255, 0));
	COLORREF clrOldText = ::SetTextColor(m_hWinDC, RGB(255, 0, 0));

	// draw something
	RECT rect;
	rect.left = m_rcDisplay.left;
	rect.top = m_rcDisplay.top;
	rect.right = rect.left + 300;
	rect.bottom = rect.top + 25;
	::DrawText(m_hWinDC, _T("Renderer Type: DC"), -1, &rect, DT_LEFT | DT_SINGLELINE);

	if(-1 != m_nLastRenderSysTime)
	{
		if(nCurrentSysTime - m_nLastRenderSysTime > 50)
			m_nTimeoutCount++;

		rect.top = rect.bottom;
		rect.bottom = rect.top + 25;
		TCHAR szText[128];
		wsprintf(szText, _T("Timeout Count: %d, Time Interval %05d"), m_nTimeoutCount, nCurrentSysTime - m_nLastRenderSysTime);
		::DrawText(m_hWinDC, szText, -1, &rect, DT_LEFT | DT_SINGLELINE);
	}

	// restore text color settings
	::SetTextColor(m_hWinDC, clrOldText);
	::SetBkColor(m_hWinDC, clrOldBk);

	m_nLastRenderSysTime = nCurrentSysTime;
#endif	// _DRAW_RENDER_INFO

	return VO_ERR_NONE;
}

VO_U32 CDCVideoRender::SetParam(VO_U32 nID, VO_PTR pParam)
{
  if(VOOSMP_PID_VIDEO_DC == nID)
  {
    if(NULL != pParam)
    {
      m_hWinDC = (HDC)pParam;
#ifdef VO_USE_SYSTEM_SCALER
#ifdef VO_USE_SYSTEM_SCALER_FASTMODE
	  ::SetStretchBltMode(m_hWinDC, COLORONCOLOR);
#else	// VO_USE_SYSTEM_SCALER_FASTMODE
	  ::SetStretchBltMode(m_hWinDC, HALFTONE);
#endif	// VO_USE_SYSTEM_SCALER_FASTMODE
#else	// VO_USE_SYSTEM_SCALER
	  ::SetStretchBltMode(m_hWinDC, HALFTONE);
#endif	// VO_USE_SYSTEM_SCALER
    }
    else
      return -1;
    return VO_ERR_NONE;
  }

  return VO_ERR_NONE;
}

VO_U32 CDCVideoRender::SetDispType (VO_IV_ZOOM_MODE nZoomMode, VO_IV_ASPECT_RATIO nRatio)
{
	voCAutoLock lock (&m_cDCMutex);
	CBaseVideoRender::SetDispType(nZoomMode, nRatio);
	
	m_bAspectRatioChanged = true;
	return VO_ERR_NONE;
}

VO_U32 CDCVideoRender::UpdateSize (void)
{
#ifdef _WIN32
#ifndef _WIN32_WCE
	m_nScreenWidth = m_rcDisplay.right - m_rcDisplay.left;
	m_nScreenHeight = m_rcDisplay.bottom - m_rcDisplay.top;
#endif // _WIN32_WCE
#endif // _WIN32

	CBaseVideoRender::UpdateSize ();

	voCAutoLock lock (&m_csDraw);

	VO_U32 nOldGDIWidth = m_nGDIWidth, nOldGDIHeight = m_nGDIHeight;
	if (!m_bRotate)
	{
		m_nGDIWidth = m_nDrawWidth;
		m_nGDIHeight = m_nDrawHeight;

#ifdef VO_USE_SYSTEM_SCALER
		if (m_pCCRRR != NULL)
			m_pCCRRR->SetCCRRSize (&m_nShowWidth, &m_nShowHeight, &m_nShowWidth, &m_nShowHeight, VO_RT_DISABLE);
		if (m_pSoftCCRRR != NULL)
			m_pSoftCCRRR->SetCCRRSize (&m_nShowWidth, &m_nShowHeight, &m_nShowWidth, &m_nShowHeight, VO_RT_DISABLE);

		VOLOGI ("SetCCRRSize %d, %d, %d, %d  %d", (int)m_nShowWidth, (int)m_nShowHeight, (int)m_nShowWidth, (int)m_nShowHeight, 0);
#endif	// VO_USE_SYSTEM_SCALER
	}
	else
	{
		m_nGDIWidth = m_nDrawHeight;
		m_nGDIHeight = m_nDrawWidth;

#ifdef VO_USE_SYSTEM_SCALER
		if (m_pCCRRR != NULL)
			m_pCCRRR->SetCCRRSize (&m_nShowWidth, &m_nShowHeight, &m_nShowWidth, &m_nShowHeight, m_bRotateType);
		if (m_pSoftCCRRR != NULL)
			m_pSoftCCRRR->SetCCRRSize (&m_nShowWidth, &m_nShowHeight, &m_nShowWidth, &m_nShowHeight, m_bRotateType);

		VOLOGI ("SetCCRRSize %d, %d, %d, %d  %d", (int)m_nShowWidth, (int)m_nShowHeight, (int)m_nShowWidth, (int)m_nShowHeight, (int)m_bRotateType);
#endif	// VO_USE_SYSTEM_SCALER
	}

#ifndef VO_USE_SYSTEM_SCALER
	if(m_nGDIWidth != nOldGDIWidth || m_nGDIHeight != nOldGDIHeight)
	{
		voCAutoLock lock (&m_cBufferMutex);

		if(m_bIsPaused)
			savePausedVRData(nOldGDIWidth, nOldGDIHeight);

		reInitVideoBuffer();
	}
#endif	// VO_USE_SYSTEM_SCALER

	int nAlpha = 255;
	if (m_pCCRRR != NULL)
	{
		m_pCCRRR->SetColorType (VO_COLOR_YUV_PLANAR420, m_outBuffer.ColorType);
		m_pCCRRR->SetParam(VO_PID_CCRRR_ALPHAVALUE, &nAlpha);
	}
	if (m_pSoftCCRRR != NULL)
	{
		m_pSoftCCRRR->SetParam(VO_PID_CCRRR_ALPHAVALUE, &nAlpha);
		m_pSoftCCRRR->SetColorType (VO_COLOR_YUV_PLANAR420, m_outBuffer.ColorType);
	}

	EraseBackGround();
	return 0;
}

bool CDCVideoRender::CreateBitmapInfo (void )
{
	int nBmpSize = sizeof(BITMAPINFOHEADER);
	if (m_nPixelBits == 16)
		nBmpSize += SIZE_MASKS; // for RGB bitMask;

	if (m_pBmpInfo == NULL)
		m_pBmpInfo = new BYTE[nBmpSize];
	memset (m_pBmpInfo, 0, nBmpSize);

	BITMAPINFO * pBmpInfo = (BITMAPINFO *)m_pBmpInfo;

	pBmpInfo->bmiHeader.biSize			= nBmpSize;

#ifdef VO_USE_SYSTEM_SCALER
	pBmpInfo->bmiHeader.biWidth			= m_nVideoWidth;
	pBmpInfo->bmiHeader.biHeight		= 0 - m_nVideoHeight;
#else	// VO_USE_SYSTEM_SCALER
	pBmpInfo->bmiHeader.biWidth			= m_nGDIWidth;
	pBmpInfo->bmiHeader.biHeight		= 0 - m_nGDIHeight;
#endif	// VO_USE_SYSTEM_SCALER

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

#ifdef VO_USE_SYSTEM_SCALER
	pBmpInfo->bmiHeader.biSizeImage		= nStride * m_nVideoHeight;
#else	// VO_USE_SYSTEM_SCALER
	pBmpInfo->bmiHeader.biSizeImage		= nStride * m_nGDIHeight;
#endif	// VO_USE_SYSTEM_SCALER

	return true;
}

void CDCVideoRender::EraseBackGround()
{
	if(m_bBlockEraseBackGround)
		return;

	int wndWidth = m_rcDisplay.right - m_rcDisplay.left;
	int wndHeight = m_rcDisplay.bottom - m_rcDisplay.top;

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

			::FillRect((HDC)m_hWinDC, &drawRect, m_hBlackBrush);

			drawRect.left = m_rcDisplay.right - nDrawWidth - 2;
			drawRect.right = m_rcDisplay.right;

			::FillRect((HDC)m_hWinDC, &drawRect, m_hBlackBrush);
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

			::FillRect((HDC)m_hWinDC, &drawRect, m_hBlackBrush);

			drawRect.top =m_rcDisplay.bottom -  nDrawHeight -2 ;
			drawRect.bottom =  m_rcDisplay.bottom;

			::FillRect((HDC)m_hWinDC, &drawRect, m_hBlackBrush);
		}
	}
}

VO_U32 CDCVideoRender::ShowOverlay(VO_BOOL bShow)
{
	if(m_pCCRRR)
		return m_pCCRRR->SetParam(VO_CCRRR_PMID_ShowOverlay , (VO_PTR)&bShow);

	return VO_ERR_FAILED;
}

VO_U32			voDCVRMemInit (VO_S32 uID, VO_MEM_VIDEO_INFO * pVideoMem);
VO_U32			voDCVRMemGetBuf (VO_S32 uID, VO_S32 nIndex);
VO_U32			voDCVRMemUninit (VO_S32 uID);

VO_MEM_VIDEO_OPERATOR	g_vmDCVROP;
VO_MEM_VIDEO_INFO		g_vmDCVRInfo;
VO_U32					g_yDCOffset;

VO_U32 CDCVideoRender::GetVideoMemOP (VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP)
{
	return CBaseVideoRender::GetVideoMemOP (ppVideoMemOP);

	memset (&g_vmDCVRInfo, 0, sizeof (VO_MEM_VIDEO_INFO));

	g_vmDCVROP.Init = voDCVRMemInit;
	g_vmDCVROP.GetBufByIndex = voDCVRMemGetBuf;
	g_vmDCVROP.Uninit = voDCVRMemUninit;

	*ppVideoMemOP = &g_vmDCVROP;
	
	return VO_ERR_NONE;
}

VO_U32 voDCVRMemInit (VO_S32 uID, VO_MEM_VIDEO_INFO * pVideoMem)
{
	memcpy (&g_vmDCVRInfo, pVideoMem, sizeof (VO_MEM_VIDEO_INFO));

	g_vmDCVRInfo.PBuffer = NULL;

	VO_VIDEO_BUFFER * pBuffers =  new VO_VIDEO_BUFFER[g_vmDCVRInfo.FrameCount];

	for (int i = 0; i < g_vmDCVRInfo.FrameCount; i++)
	{
		int yStride = g_vmDCVRInfo.Stride;
		g_yDCOffset = 0;

		pBuffers[i].Buffer[0] = new BYTE[yStride * (g_vmDCVRInfo.Height)];
		pBuffers[i].Buffer[0] += g_yDCOffset;

		pBuffers[i].Buffer[1] = new BYTE[g_vmDCVRInfo.Stride * g_vmDCVRInfo.Height / 4];
		pBuffers[i].Buffer[2] = new BYTE[g_vmDCVRInfo.Stride * g_vmDCVRInfo.Height / 4];

		pBuffers[i].Stride[0] = yStride;
		pBuffers[i].Stride[1] = g_vmDCVRInfo.Stride/2;
		pBuffers[i].Stride[2] = g_vmDCVRInfo.Stride/2;
	}

	g_vmDCVRInfo.VBuffer = pBuffers;
	pVideoMem->VBuffer = pBuffers;

	return 0;
}

VO_U32 voDCVRMemGetBuf (VO_S32 uID, VO_S32 nIndex)
{
	if (nIndex < 0 || nIndex >= g_vmDCVRInfo.FrameCount)
		return VO_ERR_FAILED;

	return VO_ERR_NONE;
}

VO_U32 voDCVRMemUninit (VO_S32 uID)
{
	if (g_vmDCVRInfo.VBuffer == NULL)
		return 0;

	for (int i = 0; i < g_vmDCVRInfo.FrameCount; i++)
	{
		delete [](g_vmDCVRInfo.VBuffer[i].Buffer[0] - g_yDCOffset); 
		delete []g_vmDCVRInfo.VBuffer[i].Buffer[1];
		delete []g_vmDCVRInfo.VBuffer[i].Buffer[2];
	}

	delete []g_vmDCVRInfo.VBuffer;
	g_vmDCVRInfo.VBuffer = NULL;

	return 0;
}
