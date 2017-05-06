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
#ifdef _DDRAW_RENDER

#include "voOSFunc.h"
#include "CDDVideoRender.h"

#define LOG_TAG "CDDVideoRender"
#include "voLog.h"

#pragma message("linking with Microsoft's DirectDraw library ...") 
#pragma comment(lib, "ddraw.lib") 
#pragma comment(lib, "dxguid.lib")

CDDVideoRender::CDDVideoRender(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
	: CBaseVideoRender (hInst, hView, pMemOP)
	, m_hWnd((HWND)m_hView)
	, m_pDD(NULL)
	, m_pDDSPrimary(NULL)
	, m_pDDSOffScr(NULL)
	, m_fDDrawCallback(NULL)
	, m_pDDCUserData(NULL)
	, m_pDDSUser(NULL)
	, m_bSurfaceLost(VO_FALSE)
	, m_nStatus(0)
	, m_bRenderStarted(VO_FALSE)
{
	m_hWinDC = ::GetDC(m_hWnd);

	memset(&m_sOffScrSurfDesc, 0, sizeof(DDSURFACEDESC2));
	memset(&m_sUserSurfDesc, 0, sizeof(DDSURFACEDESC2));

	m_hBlackBrush = ::CreateSolidBrush(RGB(0, 0, 0));
}

CDDVideoRender::~CDDVideoRender()
{
	if(m_hWinDC)
	{
		::ReleaseDC(m_hWnd, m_hWinDC);
		m_hWinDC = NULL;
	}

	ReleaseDD();

	if(m_hBlackBrush)
	{
		::DeleteObject(m_hBlackBrush);
		m_hBlackBrush = NULL;
	}
}

VO_U32 CDDVideoRender::Start()
{
	m_nStatus = 1;
	EraseBackGround();

	return CBaseVideoRender::Start();
}

VO_U32 CDDVideoRender::Pause()
{
	m_nStatus = 2;

	return CBaseVideoRender::Pause();
}

VO_U32 CDDVideoRender::Stop()
{
	m_nStatus = 0;
	m_bRenderStarted = VO_FALSE;

	return CBaseVideoRender::Stop();
}

VO_U32 CDDVideoRender::SetDispRect (VO_PTR hView, VO_RECT * pDispRect, VO_IV_COLORTYPE nColor)
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

		UpdateSize();
	} else {

		// in windowed mode, the cordinate never changes when scrolling window, it's relative
		EraseBackGround();
	}

	return VO_ERR_NONE;
}

VO_U32 CDDVideoRender::Render(VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	voCAutoLock lock(&m_csDraw);

	if(m_fCallBack != NULL)
		return CBaseVideoRender::Render(pVideoBuffer, nStart, bWait);

	// david @ 2013-07-04
	// if paused and scrolled bar, background should be erased instantly
	// david @ 2013-07-22
	// actually background should be erased per frame 
	EraseBackGround();

	if (m_pDD == NULL || m_pDDSOffScr == NULL || (m_fDDrawCallback && m_pDDSUser == NULL))
		CreateDD();

	HRESULT hr = DD_OK;

	//////////////////////////////////////////////////////////////////////////
	if(m_bSurfaceLost && m_pDDSPrimary)
	{
		// try restore primary surface
		hr = m_pDDSPrimary->Restore();
		if(DD_OK != hr)
		{
			VOLOGI("failed to restore primary surface! 0x%08X", hr);
			if(DDERR_WRONGMODE == hr)
			{
				// checked dxerr.c, DXERROR(0x8876024b, "DDERR_WRONGMODE", "This surface can not be restored because it was created in a different mode.")
				// so we need re-create DirectDraw, East 20130322
				CreateDD();
				m_bSurfaceLost = VO_FALSE;
			}
			// marked off to continue restoring the next 2 possibly lost surfaces, offscreen & user surface
			/*else
				return 0;*/
		}
	}

	if (m_bSurfaceLost && m_pDDSOffScr)
	{
		// try restore off screen surface
		hr = m_pDDSOffScr->Restore();
		if(DD_OK != hr)
		{
			VOLOGI("failed to restore off screen surface! 0x%08X", hr);
			if(DDERR_WRONGMODE == hr)
			{
				CreateDD();
				m_bSurfaceLost = VO_FALSE;
			}
			// marked off to continue restoring user surface
			/*else
				return 0;*/
		} else {
			// David @ 2013/09/11
			// if no user surf, and restored all, need to restore this tag to ture right now to avoid rendering green screen
			if (m_pDDSUser == NULL)
				m_bSurfaceLost = VO_FALSE;
		}
	}

	// david 2013-07-11
	// if user serface lost, restore it
	if(m_bSurfaceLost && m_pDDSUser)
	{
		// try restore off screen surface
		hr = m_pDDSUser->Restore();
		if(DD_OK != hr)
		{
			VOLOGI("failed to restore off screen surface! 0x%08X", hr);
			if(DDERR_WRONGMODE == hr)
			{
				CreateDD();
				m_bSurfaceLost = VO_FALSE;
			}
			else
				return 0;
		} else {
			// David @ 2013/09/11
			// if restored all, need to restore this tag to ture right now to avoid rendering green screen
			m_bSurfaceLost = VO_FALSE;
		}
	}

	// even restore successfully, we can't lock off screen surface, it crashes in some devices
	//////////////////////////////////////////////////////////////////////////

	// sometimes create surface will fail, for example IE browser first page, East 20130318
	if(NULL == m_pDDSPrimary || NULL == m_pDDSOffScr)
		return 0;

	if(pVideoBuffer == NULL || pVideoBuffer->Buffer[0] == NULL || pVideoBuffer->Buffer[1] == NULL || pVideoBuffer->Buffer[2] == NULL)
		return 0;

	int nYOffset = m_nShowTop * pVideoBuffer->Stride[0] + m_nShowLeft;
	int nUVOffset = (m_nShowTop / 2) * pVideoBuffer->Stride[1] + m_nShowLeft / 2;

	m_inData.Buffer[0] = pVideoBuffer->Buffer[0]  + nYOffset;
	m_inData.Buffer[1] = pVideoBuffer->Buffer[1]  + nUVOffset;
	m_inData.Buffer[2] = pVideoBuffer->Buffer[2]  + nUVOffset;

	m_inData.Stride[0] = pVideoBuffer->Stride[0];
	m_inData.Stride[1] = pVideoBuffer->Stride[1];
	m_inData.Stride[2] = pVideoBuffer->Stride[2];

	RECT rctDest, rctSour;
	rctSour.left = 0;
	rctSour.top = 0;
	rctSour.right = m_nShowWidth;
	rctSour.bottom = m_nShowHeight;
	rctDest.left = m_nDrawLeft;
	rctDest.top =  m_nDrawTop;

	POINT ptLeftTop;
	ptLeftTop.x = rctDest.left;
	ptLeftTop.y = rctDest.top;
	::ClientToScreen(m_hWnd, &ptLeftTop);
	rctDest.left = ptLeftTop.x;
	rctDest.top = ptLeftTop.y;

	//VOLOGI("[dst rect] l = %d, t = %d, r = %d, b = %d", rctDest.left, rctDest.top, rctDest.right, rctDest.bottom);
	// modified by david @ 2013/05/06
	// down-right coordinates should also be adjusted if top-left changed in projector cases
	rctDest.right = rctDest.left + m_nDrawWidth;
	rctDest.bottom = rctDest.top + m_nDrawHeight;

	if(!m_bSurfaceLost)
	{
		while(true)
		{
			hr = m_pDDSOffScr->Lock(&rctSour, &m_sOffScrSurfDesc, DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL);
			if(DDERR_WASSTILLDRAWING == hr)
			{
				VOLOGI("lock off screen return DDERR_WASSTILLDRAWING");
				voOS_Sleep(2);
			}
			else
				break;
		}

		if(DD_OK != hr)
		{
			VOLOGI("failed to lock off screen 0x%08X", hr);

			if(DDERR_SURFACELOST == hr || DDERR_UNSUPPORTED == hr)
			{
				VOLOGI("here surface lost!");
				m_bSurfaceLost = VO_TRUE;
			}

			return 0;
		}

		LPBYTE lpSurf = (LPBYTE)m_sOffScrSurfDesc.lpSurface;
		VO_U32 i;
		if(lpSurf)
		{
			for (i = 0; i < m_nShowHeight; i++)
			{
				memcpy (lpSurf, m_inData.Buffer[0], m_nShowWidth);

				m_inData.Buffer[0] += m_inData.Stride[0];
				lpSurf += m_sOffScrSurfDesc.lPitch;
			}

			for (i = 0; i < m_nShowHeight / 2; i++)
			{
				memcpy(lpSurf, m_inData.Buffer[2], m_nShowWidth / 2);

				m_inData.Buffer[2] += m_inData.Stride[2];
				lpSurf += m_sOffScrSurfDesc.lPitch / 2;
			}
			for (i = 0; i < m_nShowHeight / 2; i++)
			{
				memcpy(lpSurf, m_inData.Buffer[1], m_nShowWidth / 2);

				m_inData.Buffer[1] += m_inData.Stride[1];
				lpSurf += m_sOffScrSurfDesc.lPitch / 2;
			}
		}
		hr = m_pDDSOffScr->Unlock(&rctSour);
		if(DD_OK != hr)
		{
			VOLOGI("failed to unlock off screen 0x%08X", hr);
		}
	}

	if(m_fDDrawCallback && m_pDDSUser)
	{
		RECT rctUser;
		memset(&rctUser, 0, sizeof(rctUser));
		rctUser.right = rctUser.left + m_nDrawWidth;
		rctUser.bottom = rctUser.top + m_nDrawHeight;

		// Blt off screen surface to user surface
		hr = m_pDDSUser->Blt(&rctUser, m_pDDSOffScr, &rctSour, DDBLT_WAIT, NULL);
		if(DD_OK != hr)
		{
			VOLOGI("failed to Blt user surface 0x%08X;  [rect] r = %d, b = %d", hr, rctUser.right, rctUser.bottom);
			if(DDERR_SURFACELOST == hr || DDERR_UNSUPPORTED == hr)
			{
				VOLOGI("here surface lost!");
				m_bSurfaceLost = VO_TRUE;
			}

			return 0;
		}

		// The GetDC method creates a GDI-compatible device context for the surface.
		// It uses an internal version of the Lock method to lock the surface, and the surface will remain locked until ReleaseDC is called.
		// So we must call GetDC and ReleaseDC every time instead of when CreateDD and ReleaseDD
		HDC hUserDC = NULL;
		hr = m_pDDSUser->GetDC(&hUserDC);
		if(DD_OK != hr)
		{
			VOLOGW("failed to GetDC for user surface 0x%08X", hr);
		}
		else
		{
			// callback to user
			// user want to get display rectangle so that they can draw something
			m_fDDrawCallback(m_pDDCUserData, hUserDC, &rctUser, nStart);
			hr = m_pDDSUser->ReleaseDC(hUserDC);
		}

		// Blt user surface to primary surface
		hr = m_pDDSPrimary->Blt(&rctDest, m_pDDSUser, &rctUser, DDBLT_WAIT, NULL);
	}
	else
	{
		// Blt off screen surface to primary surface
		hr = m_pDDSPrimary->Blt(&rctDest, m_pDDSOffScr, &rctSour, DDBLT_WAIT, NULL);
	}

	if(DD_OK != hr)
	{
		VOLOGI("failed to Blt primary surface 0x%08X", hr);
		if(DDERR_SURFACELOST == hr || DDERR_UNSUPPORTED == hr)
		{
			VOLOGI("here surface lost!");
			m_bSurfaceLost = VO_TRUE;
		}
		m_bRenderStarted = VO_FALSE;

		return 0;
	}
	else	// we can judge surface restore successfully only by Blt
		m_bSurfaceLost = VO_FALSE;

	m_bRenderStarted = VO_TRUE;
	if(m_bBlockEraseBackGround)
		m_bBlockEraseBackGround = VO_FALSE;

#ifdef _DRAW_RENDER_INFO
	VO_U32 nCurrentSysTime = voOS_GetSysTime();

	// begin paint
	PAINTSTRUCT sPaint;
	::BeginPaint(m_hWnd, &sPaint);

	// set text color settings
	COLORREF clrOldBk = ::SetBkColor(m_hWinDC, RGB(0, 255, 0));
	COLORREF clrOldText = ::SetTextColor(m_hWinDC, RGB(255, 0, 0));

	// draw something
	RECT rect;
	rect.left = m_rcDisplay.left;
	rect.top = m_rcDisplay.top;
	rect.right = rect.left + 300;
	rect.bottom = rect.top + 25;
	::DrawText(m_hWinDC, _T("Renderer Type: DirectDraw"), -1, &rect, DT_LEFT | DT_SINGLELINE);

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

	// end paint
	::EndPaint(m_hWnd, &sPaint);

	m_nLastRenderSysTime = nCurrentSysTime;
#endif	// _DRAW_RENDER_INFO

	return VO_ERR_NONE;
}

VO_U32 CDDVideoRender::Redraw()
{
	voCAutoLock lock(&m_csDraw);

	if(VO_FALSE == m_bRenderStarted)
		return -1;

	EraseBackGround();

	// sometimes create surface will fail, for example IE browser first page, East 20130318
	if(NULL == m_pDDSPrimary || NULL == m_pDDSOffScr)
		return 0;

	RECT rctSour;
	memset(&rctSour, 0, sizeof(rctSour));
	rctSour.right = m_nShowWidth;
	rctSour.bottom = m_nShowHeight;

	POINT ptLeftTop;
	ptLeftTop.x = m_nDrawLeft;
	ptLeftTop.y = m_nDrawTop;
	::ClientToScreen(m_hWnd, &ptLeftTop);

	RECT rctDest;
	rctDest.left = ptLeftTop.x;
	rctDest.top = ptLeftTop.y;
	rctDest.right = rctDest.left + m_nDrawWidth;
	rctDest.bottom = rctDest.top + m_nDrawHeight;

	HRESULT hr = DD_OK;
	if(m_fDDrawCallback && m_pDDSUser)
	{
		RECT rctUser;
		memset(&rctUser, 0, sizeof(rctUser));
		rctUser.right = rctUser.left + m_nDrawWidth;
		rctUser.bottom = rctUser.top + m_nDrawHeight;

		// Blt off screen surface to user surface
		hr = m_pDDSUser->Blt(&rctUser, m_pDDSOffScr, &rctSour, DDBLT_WAIT, NULL);
		if(DD_OK != hr)
			return 0;

		HDC hUserDC = NULL;
		hr = m_pDDSUser->GetDC(&hUserDC);
		if(DD_OK == hr)
		{
			m_fDDrawCallback(m_pDDCUserData, hUserDC, &rctUser, -1);
			hr = m_pDDSUser->ReleaseDC(hUserDC);
		}

		// Blt user surface to primary surface
		hr = m_pDDSPrimary->Blt(&rctDest, m_pDDSUser, &rctUser, DDBLT_WAIT, NULL);
	}
	else
	{
		// Blt off screen surface to primary surface
		hr = m_pDDSPrimary->Blt(&rctDest, m_pDDSOffScr, &rctSour, DDBLT_WAIT, NULL);
	}

	return VO_ERR_NONE;
}

VO_U32 CDDVideoRender::SetDDrawCallback(VODDRAWRENDERCALLBACK fCallback, VO_PTR pUserData)
{
	voCAutoLock lock(&m_csDraw);

	m_fDDrawCallback = fCallback;
	m_pDDCUserData = pUserData;

	return VO_ERR_NONE;
}

VO_U32 CDDVideoRender::UpdateSize()
{
#ifdef _WIN32
#ifndef _WIN32_WCE
	m_nScreenWidth = m_rcDisplay.right - m_rcDisplay.left;
	m_nScreenHeight = m_rcDisplay.bottom - m_rcDisplay.top;
#endif // _WIN32_WCE
#endif // _WIN32
	
	CBaseVideoRender::UpdateSize();

	// if paused, do not erase background temporarily until running again
	if(m_nStatus != 2)
		EraseBackGround();

	// Daivd @ 2013-07-12
	// modified to fix the bug: video cannot be displayed if switching full DD mode to projector with subtitle enabled
	// create RGB user surface
	if (m_sUserSurfDesc.dwWidth == m_rcDisplay.right - m_rcDisplay.left && m_sUserSurfDesc.dwHeight == m_rcDisplay.bottom - m_rcDisplay.top)
		return 0;

	if (m_fDDrawCallback)
	{
		voCAutoLock lock (&m_csDraw);
		// release user surface
		if(m_pDDSUser)
		{
			m_pDDSUser->Release();
			m_pDDSUser = NULL;
		}

		memset(&m_sUserSurfDesc, 0, sizeof(m_sUserSurfDesc));
		m_sUserSurfDesc.dwSize = sizeof(m_sUserSurfDesc);
		m_sUserSurfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		m_sUserSurfDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		m_sUserSurfDesc.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
		m_sUserSurfDesc.ddpfPixelFormat.dwRGBBitCount = 32;
		m_sUserSurfDesc.ddpfPixelFormat.dwRBitMask = 0x00ff0000;
		m_sUserSurfDesc.ddpfPixelFormat.dwGBitMask = 0x0000ff00;
		m_sUserSurfDesc.ddpfPixelFormat.dwBBitMask = 0x000000ff;
		m_sUserSurfDesc.ddpfPixelFormat.dwRGBAlphaBitMask = 0xff000000;

		// david 2013-07-11
		// this user RGB surface should hold the entire display rect, otherwise if aspect ratio changed, it will failed to blit due to invalid_rect
		m_sUserSurfDesc.dwWidth = m_rcDisplay.right - m_rcDisplay.left;
		m_sUserSurfDesc.dwHeight = m_rcDisplay.bottom - m_rcDisplay.top;
		VOLOGI("[user surface] w x h = %d x %d", m_sUserSurfDesc.dwWidth, m_sUserSurfDesc.dwHeight);
		HRESULT hr = 0;
		if (m_pDD)
			hr = m_pDD->CreateSurface(&m_sUserSurfDesc, &m_pDDSUser, NULL);
		if(DD_OK != hr)
		{
			VOLOGW("failed to CreateSurface for user surface 0x%08X", hr);
		}
	}

	return 0;
}

void CDDVideoRender::EraseBackGround()
{
	if(m_bBlockEraseBackGround)
		return;

	int wndWidth = m_rcDisplay.right - m_rcDisplay.left;
	int wndHeight = m_rcDisplay.bottom - m_rcDisplay.top;

	RECT drawRect;
	if(wndWidth > (int)m_nDrawWidth)
	{
		int nDrawWidth = (wndWidth - m_nDrawWidth) / 2 ; 
		nDrawWidth = (nDrawWidth + 1) / 2 * 2;
		if(nDrawWidth > 0)
		{
			drawRect.top = m_rcDisplay.top;
			drawRect.bottom = m_rcDisplay.bottom;
			drawRect.left = m_rcDisplay.left;
			drawRect.right = drawRect.left + nDrawWidth + 2;

			FillRect((HDC)m_hWinDC, &drawRect, m_hBlackBrush);

			drawRect.left = m_rcDisplay.right - nDrawWidth - 2;
			drawRect.right = m_rcDisplay.right;

			FillRect((HDC)m_hWinDC, &drawRect, m_hBlackBrush);
		}
	}

	if(wndHeight > (int)m_nDrawHeight)
	{
		int nDrawHeight = (wndHeight - m_nDrawHeight) / 2;
		nDrawHeight = (nDrawHeight + 1) / 2 * 2;
		if(nDrawHeight > 0)
		{
			drawRect.top = m_rcDisplay.top;
			drawRect.bottom =  (drawRect.top + nDrawHeight + 1) / 2 * 2;
			drawRect.left = m_rcDisplay.left;
			drawRect.right = m_rcDisplay.right;

			FillRect((HDC)m_hWinDC, &drawRect, m_hBlackBrush);

			drawRect.top = (m_rcDisplay.bottom -  nDrawHeight - 1 ) / 2 * 2 ;
			drawRect.bottom =  m_rcDisplay.bottom;

			//VOLOGI("[display rect] %d, %d, %d, %d; draw height: %d, [fill rect] %d, %d, %d, %d", m_rcDisplay.left, m_rcDisplay.top, m_rcDisplay.right, m_rcDisplay.bottom,
				//nDrawHeight, drawRect.left, drawRect.top, drawRect.right, drawRect.bottom);
			FillRect((HDC)m_hWinDC, &drawRect, m_hBlackBrush);
		}
	}
}



bool CDDVideoRender::CreateDD()
{
	ReleaseDD ();

	// david @ 2013/12/09
	// to do protect for those zero-size cases, otherwise some device will fail to create the non-zero-size surface subsequently.
	if(m_nDrawWidth * m_nDrawHeight == 0)
	{
		VOLOGW("failed since illegal draw resolution %dX%d", m_nDrawWidth, m_nDrawHeight);
		return false;
	}

	HRESULT hr = DirectDrawCreateEx(NULL, (VOID**)&m_pDD, IID_IDirectDraw7, NULL);
	if(DD_OK != hr)
	{
		VOLOGE("failed to DirectDrawCreateEx 0x%08X", hr);
		return false;
	}

	hr = m_pDD->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL | DDSCL_ALLOWREBOOT | DDSCL_MULTITHREADED);
	if(DD_OK != hr)
	{
		VOLOGE("failed to SetCooperativeLevel 0x%08X", hr);
		return false;
	}

	DDSURFACEDESC2 sPrimarySurfDesc;
	memset(&sPrimarySurfDesc, 0, sizeof(sPrimarySurfDesc));
	sPrimarySurfDesc.dwSize = sizeof(sPrimarySurfDesc);
	sPrimarySurfDesc.dwFlags = DDSD_CAPS;
	sPrimarySurfDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	hr = m_pDD->CreateSurface(&sPrimarySurfDesc, &m_pDDSPrimary, NULL);
	if(DD_OK != hr)
	{
		VOLOGE("failed to CreateSurface for primary surface 0x%08X", hr);
		return false;
	}

	LPDIRECTDRAWCLIPPER pcClipper;   // clipper
	hr = m_pDD->CreateClipper(0, &pcClipper, NULL);
	if(DD_OK != hr)
	{
		VOLOGE("failed to CreateClipper 0x%08X", hr);
		return false;
	}

	hr = pcClipper->SetHWnd(0, m_hWnd);
	if(DD_OK != hr)
	{
		VOLOGE("failed to SetHWnd 0x%08X", hr);
		pcClipper->Release();
		return false;
	}

	hr = m_pDDSPrimary->SetClipper(pcClipper);
	if(DD_OK != hr)
	{
		VOLOGE("failed to SetClipper 0x%08X", hr);
		pcClipper->Release();
		return false;
	}

	// done with clipper
	pcClipper->Release();

	// create YUV off screen surface
	memset(&m_sOffScrSurfDesc, 0, sizeof(m_sOffScrSurfDesc));
	m_sOffScrSurfDesc.dwSize = sizeof(m_sOffScrSurfDesc);
	m_sOffScrSurfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	m_sOffScrSurfDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	m_sOffScrSurfDesc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	m_sOffScrSurfDesc.ddpfPixelFormat.dwFlags  = DDPF_FOURCC | DDPF_YUV ;
	m_sOffScrSurfDesc.ddpfPixelFormat.dwFourCC = MAKEFOURCC('Y','V', '1', '2');
	m_sOffScrSurfDesc.ddpfPixelFormat.dwYUVBitCount = 8;
	m_sOffScrSurfDesc.dwWidth = m_nShowWidth;
	m_sOffScrSurfDesc.dwHeight = m_nShowHeight;
	hr = m_pDD->CreateSurface(&m_sOffScrSurfDesc, &m_pDDSOffScr, NULL);
	if (DD_OK != hr)
	{
		VOLOGE("failed to CreateSurface for offscreen surface 0x%08X  dwWidth x dwHeight = %d x %d", hr, m_nShowWidth, m_nShowHeight);
		return false;
	}
	// create RGB user surface
	if(m_fDDrawCallback)
	{
		memset(&m_sUserSurfDesc, 0, sizeof(m_sUserSurfDesc));
		m_sUserSurfDesc.dwSize = sizeof(m_sUserSurfDesc);
		m_sUserSurfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		m_sUserSurfDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		m_sUserSurfDesc.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
		m_sUserSurfDesc.ddpfPixelFormat.dwRGBBitCount = 32;
		m_sUserSurfDesc.ddpfPixelFormat.dwRBitMask = 0x00ff0000;
		m_sUserSurfDesc.ddpfPixelFormat.dwGBitMask = 0x0000ff00;
		m_sUserSurfDesc.ddpfPixelFormat.dwBBitMask = 0x000000ff;
		m_sUserSurfDesc.ddpfPixelFormat.dwRGBAlphaBitMask = 0xff000000;

		// david 2013-07-11
		// this user RGB surface should hold the entire display rect, otherwise if aspect ratio changed, it will failed to blit due to invalid_rect
		m_sUserSurfDesc.dwWidth = m_rcDisplay.right - m_rcDisplay.left;
		m_sUserSurfDesc.dwHeight = m_rcDisplay.bottom - m_rcDisplay.top;
		VOLOGI("[user surface] w x h = %d x %d", m_sUserSurfDesc.dwWidth, m_sUserSurfDesc.dwHeight);
		hr = m_pDD->CreateSurface(&m_sUserSurfDesc, &m_pDDSUser, NULL);
		if(DD_OK != hr)
		{
			VOLOGW("failed to CreateSurface for user surface 0x%08X", hr);
		}
	}

	return true;
}

bool CDDVideoRender::ReleaseDD()
{
	if(m_pDD != NULL)
	{
		// release user surface
		if(m_pDDSUser)
		{
			m_pDDSUser->Release();
			m_pDDSUser = NULL;
		}

		// release off screen surface
		if(m_pDDSOffScr)
		{
			m_pDDSOffScr->Release();
			m_pDDSOffScr = NULL;
		}

		// release primary surface
		if(m_pDDSPrimary)
		{
			m_pDDSPrimary->Release();
			m_pDDSPrimary = NULL;
		}

		m_pDD->Release();
		m_pDD = NULL;
	}

	m_bRenderStarted = VO_FALSE;

	return true;
}

#endif //_DDRAW_RENDER
