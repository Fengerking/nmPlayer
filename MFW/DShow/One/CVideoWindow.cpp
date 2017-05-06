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
	2008-12-02		JBF			Create file

*******************************************************************************/
#include <streams.h>
#include "CVideoWindow.h"

#include "CFilterOneRender.h"
#include "CVOWPlayer.h"

// Constructor
CVideoWindow::CVideoWindow(TCHAR *pName,
							LPUNKNOWN pUnk,
							HRESULT *phr,
							CCritSec *pInterfaceLock,
							CFilterOneRender *pRenderer)
	: CBaseControlVideo(pRenderer, pInterfaceLock, pName, pUnk, phr)
	, CBaseControlWindow(pRenderer, pInterfaceLock, pName, pUnk, phr)
	, m_pRenderer(pRenderer)
	, m_pVideoFormat (NULL)
	, m_hBGBrush (NULL)
	, m_lFullScreen (0)
	, m_bDefaultTarget (true)
	, m_bDefaultSource (true)
{
	PrepareWindow();
}

// Destructor
CVideoWindow::~CVideoWindow()
{
	DoneWithWindow();

	if (m_pVideoFormat != NULL)
	{
		delete m_pVideoFormat;
		m_pVideoFormat = NULL;
	}
}

// NonDelegatingQueryInterface
STDMETHODIMP CVideoWindow::NonDelegatingQueryInterface(REFIID riid,void **ppv)
{
	CheckPointer(ppv,E_POINTER);

	if (riid == IID_IVideoWindow) 
	{
		return CBaseVideoWindow::NonDelegatingQueryInterface(riid,ppv);
	} 
	else 
	{
		return CBaseBasicVideo::NonDelegatingQueryInterface(riid,ppv);
	}
}

// GetClassWindowStyles
LPTSTR CVideoWindow::GetClassWindowStyles(DWORD *pClassStyles, DWORD *pWindowStyles, DWORD *pWindowStylesEx)
{
	CheckPointer(pClassStyles, NULL);
	CheckPointer(pWindowStyles, NULL);
	CheckPointer(pWindowStylesEx, NULL);

	// Default window styles
#ifdef _WIN32_WCE
	*pClassStyles    = CS_HREDRAW | CS_VREDRAW;
	*pWindowStyles   = WS_CLIPCHILDREN; //WS_OVERLAPPED | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	*pWindowStylesEx = WS_EX_NOANIMATION; //(DWORD) 0;
#else
	*pClassStyles    = CS_HREDRAW | CS_VREDRAW;
	*pWindowStyles   = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
	*pWindowStylesEx = WS_EX_OVERLAPPEDWINDOW;
#endif

	return TEXT("MobileVideoRenderer\0");
} // GetClassWindowStyles

HRESULT CVideoWindow::DoneWithWindow()
{
	const HWND hwnd = m_hwnd;
	if (hwnd == NULL)
		return NOERROR;

	InactivateWindow();

	//  UnintialiseWindow sets m_hwnd to NULL so save a copy
	UninitialiseWindow();
	DestroyWindow(hwnd);

	// Reset our state so we can be prepared again
	m_pClassName = NULL;
	m_ClassStyles = 0;
	m_WindowStyles = 0;
	m_WindowStylesEx = 0;
	m_ShowStageMessage = 0;
	m_ShowStageTop = 0;

	if (m_hBGBrush != NULL)
		DeleteObject (m_hBGBrush);
	m_hBGBrush = NULL;

	return NOERROR;
}

// OnReceiveMessage
// This is the derived class window message handler methods
LRESULT CVideoWindow::OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
#ifndef WINUTIL_STUB
#ifdef WINCE
	case WM_SETTINGCHANGE:

		if (SETTINGCHANGE_RESET == wParam && 0 == lParam) {

			DeleteDC(m_MemoryDC);
			ReleaseDC(hwnd, m_hdc);

			m_hdc = GetDC(hwnd);
			m_MemoryDC = CreateCompatibleDC(m_hdc);

			//m_pRenderer->RenewalHDC();
		}

		return (LRESULT) 0;
#endif //
#endif // WINUTIL_STUB

	case WM_ERASEBKGND:
	{
		RECT rcView;
		GetClientRect (hwnd, &rcView);

		if (m_hBGBrush == NULL)
			m_hBGBrush = CreateSolidBrush(RGB (12, 0, 12));
		FillRect(m_hdc , &rcView, m_hBGBrush);
/*
		m_pPlayer = m_pRenderer->GetPlayer ();
		if (m_pPlayer != NULL)
			m_pPlayer->SetParam (VO_VOMMP_PMID_Redraw, NULL);
		else
			return CBaseWindow::OnReceiveMessage(hwnd,uMsg,wParam,lParam);
*/
		return (LRESULT) 0;
	}	
	
	case WM_CLOSE: 
		m_pRenderer->NotifyEvent(EC_USERABORT,0,0);
		DoShowWindow(SW_HIDE);
		return CBaseWindow::OnClose();

	case WM_SETCURSOR:
		if (IsCursorHidden() == TRUE) 
		{
			SetCursor(NULL);
			return (LRESULT) 1;
		}
		break;

	case WM_PAINT:
		m_pPlayer = m_pRenderer->GetPlayer ();
		if (m_pPlayer != NULL)
			m_pPlayer->SetParam (VO_VOMMP_PMID_Redraw, NULL);
		break;

	case WM_WINDOWPOSCHANGED:
	case WM_STYLECHANGED:
	case WM_MOVE:
		if (m_pRenderer != NULL)
			m_pRenderer->Resize ();
		break;

	default:
		break;
	}

	return CBaseWindow::OnReceiveMessage(hwnd,uMsg,wParam,lParam);
} // OnReceiveMessage


//******************************************************************************
// Handle the IBasicVideo Interface
//******************************************************************************
// IsDefaultTargetRect
// Return S_OK if using the default target otherwise S_FALSE
HRESULT CVideoWindow::IsDefaultTargetRect()
{
	if (m_bDefaultTarget)
		return S_OK;
	else
		return S_FALSE;
} // IsDefaultTargetRect

// SetDefaultTargetRect
// This is called when we reset the default target rectangle
HRESULT CVideoWindow::SetDefaultTargetRect()
{
	m_bDefaultTarget = true;

	return NOERROR;
} // SetDefaultTargetRect

// SetTargetRect
HRESULT CVideoWindow::SetTargetRect(RECT *pTargetRect)
{
	m_bDefaultTarget = false;
	memcpy (&m_rcTarget, pTargetRect, sizeof (RECT));

	return NOERROR;
} // SetTargetRect

// GetTargetRect
HRESULT CVideoWindow::GetTargetRect(RECT *pTargetRect)
{
	if (m_bDefaultTarget)
	{
		long	wndLeft, wndTop, wndWidth, wndHeight;
		HRESULT hr = GetWindowPosition(&wndLeft, &wndTop, &wndWidth, &wndHeight);
		if (FAILED (hr))
			return hr;

		pTargetRect->left = 0;
		pTargetRect->top = 0;
		pTargetRect->right = wndWidth;
		pTargetRect->bottom = wndHeight;
	}
	else
	{
		memcpy (pTargetRect, &m_rcTarget, sizeof (RECT));
	}

	return NOERROR;
} // GetTargetRect

// IsDefaultSourceRect
// Return S_OK if using the default source otherwise S_FALSE
HRESULT CVideoWindow::IsDefaultSourceRect()
{
	if (m_bDefaultSource)
		return S_OK;
	else
		return S_FALSE;
} // IsDefaultSourceRect

// SetDefaultSourceRect
HRESULT CVideoWindow::SetDefaultSourceRect()
{
	m_bDefaultTarget = true;

	return NOERROR;
} // SetDefaultSourceRect

// SetSourceRect
HRESULT CVideoWindow::SetSourceRect(RECT *pSourceRect)
{
	m_pPlayer = m_pRenderer->GetPlayer ();
	if (m_pPlayer == NULL)
		return E_NOTIMPL;

	VO_VIDEO_FORMAT fmtVideo;
	int nRC = m_pPlayer->GetParam (VO_VOMMP_PMID_VideoFormat, &fmtVideo);

	int nWidth =  fmtVideo.Width;
	int nHeight = fmtVideo.Height;

	if (pSourceRect->left % 16 != 0 || pSourceRect->top % 16 != 0)
		return ERROR_DS_PARAM_ERROR;

	if (pSourceRect->right > nWidth || pSourceRect->bottom > nHeight)
		return ERROR_DS_PARAM_ERROR;

	memcpy (&m_rcSource, pSourceRect, sizeof (RECT));
	m_bDefaultSource = false;

	return NOERROR;
} // SetSourceRect

// GetSourceRect
HRESULT CVideoWindow::GetSourceRect(RECT *pSourceRect)
{
	m_pPlayer = m_pRenderer->GetPlayer ();
	if (m_pPlayer == NULL)
		return E_NOTIMPL;

	if (m_bDefaultSource)
	{
		VO_VIDEO_FORMAT fmtVideo;
		int nRC = m_pPlayer->GetParam (VO_VOMMP_PMID_VideoFormat, &fmtVideo);

		SetRect (pSourceRect, 0, 0, fmtVideo.Width, fmtVideo.Height);
	}
	else
	{
		memcpy (pSourceRect, &m_rcSource, sizeof (RECT));
	}

	return NOERROR;
} // GetSourceRect

// GetStaticImage
// Return a copy of the current image in the video renderer
HRESULT CVideoWindow::GetStaticImage(long *pBufferSize, long *pDIBImage)
{
	CBasePin * pVideoPin = m_pRenderer->GetPin (0);
	if (!pVideoPin->IsConnected ())
		return VFW_E_NOT_CONNECTED;

	return E_NOTIMPL;
} // GetStaticImage


//******************************************************************************
// Handle IVideoWindow Interface
//******************************************************************************
STDMETHODIMP CVideoWindow::get_FullScreenMode (long *FullScreenMode)
{
	* FullScreenMode = m_lFullScreen;

	return S_OK;
}

STDMETHODIMP CVideoWindow::put_FullScreenMode(long FullScreenMode)
{	
	m_lFullScreen = FullScreenMode;

	if (m_lFullScreen == OATRUE)
	{
		SetFullScreen (true);
	}
	else
	{
		SetFullScreen (false);
	}

	return S_OK;
}

STDMETHODIMP CVideoWindow::GetMinIdealImageSize(long * pWidth, long * pHeight)
{
	HRESULT hr =  CBaseControlWindow::GetMinIdealImageSize (pWidth, pHeight);

	return hr;
}

STDMETHODIMP CVideoWindow::GetMaxIdealImageSize (long * pWidth, long * pHeight)
{
	HRESULT hr = CBaseControlWindow::GetMaxIdealImageSize (pWidth, pHeight);

	return hr;
}

STDMETHODIMP CVideoWindow::put_Left(long Left)
{
	return CBaseControlWindow::put_Left (Left);
}

STDMETHODIMP CVideoWindow::put_Width(long Width)
{
	return CBaseControlWindow::put_Width (Width);
}

STDMETHODIMP CVideoWindow::put_Top(long Top)
{
	return CBaseControlWindow::put_Top (Top);
}

STDMETHODIMP CVideoWindow::put_Height(long Height)
{
	return CBaseControlWindow::put_Height (Height);
}

STDMETHODIMP CVideoWindow::put_Owner(OAHWND Owner)
{
	return CBaseControlWindow::put_Owner (Owner);
}

STDMETHODIMP CVideoWindow::SetWindowForeground(long Focus)
{
	return CBaseControlWindow::SetWindowForeground (Focus);
}

STDMETHODIMP CVideoWindow::SetWindowPosition (long Left,long Top,long Width,long Height)
{
	HRESULT hr = S_OK;

	hr = CBaseControlWindow::SetWindowPosition (Left, Top, Width, Height);

	return hr;
}


bool CVideoWindow::SetFullScreen (bool bFullScreen)
{
	CAutoLock	lockFullScreen (&m_csFullScreen);

	if (bFullScreen)
	{
		// Save the old position of video window
		GetWindowPosition (&m_nOldLeft, &m_nOldTop, &m_nOldWidth, &m_nOldHeight);
		// Remove the ownership of video window
		get_Owner(&m_pOldParent);
		put_Owner(NULL);
		// Change the video window dimension to full screen 
		long cxScreen = GetSystemMetrics(SM_CXSCREEN);
		long cyScreen = GetSystemMetrics(SM_CYSCREEN);
		SetWindowPosition(0, 0, cxScreen, cyScreen);
		SetWindowForeground(OATRUE);

		SetWindowPos (m_hwnd, HWND_TOPMOST, 0, 0, cxScreen, cyScreen, 0);
//		SetWindowPos (m_hwnd, NULL, 0, 0, cxScreen, cyScreen, 0);
	}
	else
	{
		// Set the ownership of video window
		SetWindowPosition(m_nOldLeft, m_nOldTop, m_nOldWidth, m_nOldHeight);
//		::SetWindowPos (m_hwnd, HWND_NOTOPMOST, m_nOldLeft, m_nOldTop, m_nOldWidth, m_nOldHeight, 0);
		::SetWindowPos (m_hwnd, HWND_BOTTOM, m_nOldLeft, m_nOldTop, m_nOldWidth, m_nOldHeight, 0);

		put_Owner(m_pOldParent);
		SetWindowForeground(OAFALSE);
	}

	return true;
}

BOOL CVideoWindow::OnSize(LONG Width, LONG Height)
{	
	if (CBaseControlWindow::OnSize(Width, Height))
	{
		RECT newRect;
		SetRect(&newRect, 0, 0, Width, Height);

		SetTargetRect(&newRect);

		m_pRenderer->Resize ();

		return TRUE;
	}

	return FALSE;
}

// GetVideoFormat
VIDEOINFOHEADER * CVideoWindow::GetVideoFormat()
{
	m_pPlayer = m_pRenderer->GetPlayer ();
	if (m_pPlayer == NULL)
		return NULL;

	if (m_pVideoFormat == NULL)
	{
		int size = sizeof(VIDEOINFOHEADER) + SIZE_MASKS;
		m_pVideoFormat = (VIDEOINFOHEADER *) new BYTE[size];
		if (m_pVideoFormat == NULL)
			return NULL;
		memset (m_pVideoFormat, 0, size);

		memcpy (&m_pVideoFormat->rcSource, &m_rcSource, sizeof (RECT));
		memcpy (&m_pVideoFormat->rcTarget, &m_rcTarget, sizeof (RECT));

		m_pVideoFormat->AvgTimePerFrame = 30;
		m_pVideoFormat->dwBitErrorRate = 0;
		m_pVideoFormat->dwBitRate = 0;

		LPBITMAPINFOHEADER bmpHead = HEADER(m_pVideoFormat);

		bmpHead->biSize = sizeof(LPBITMAPINFOHEADER) + SIZE_MASKS;

		VO_VIDEO_FORMAT fmtVideo;
		int nRC = m_pPlayer->GetParam (VO_VOMMP_PMID_VideoFormat, &fmtVideo);
		bmpHead->biWidth = fmtVideo.Width;
		bmpHead->biHeight = fmtVideo.Height;

		bmpHead->biBitCount = 16; 
		bmpHead->biCompression = BI_BITFIELDS;

		VIDEOINFO *vInfo = (VIDEOINFO *)m_pVideoFormat;
		for (int i = 0; i < 3; i++)
			vInfo->dwBitMasks[i] = bits565[i];

		bmpHead->biPlanes			= 1;
		bmpHead->biSizeImage		= GetBitmapSize (bmpHead);
	}

	return m_pVideoFormat;
} // GetVideoFormat

HRESULT CVideoWindow::PrepareWindow()
{
    if (m_hwnd) return NOERROR;
    ASSERT(m_hwnd == NULL);
    ASSERT(m_hdc == NULL);

    // Get the derived object's window and class styles

    m_pClassName = GetClassWindowStyles(&m_ClassStyles,
                                        &m_WindowStyles,
                                        &m_WindowStylesEx);
    if (m_pClassName == NULL) {
        return E_FAIL;
    }

    // Register our special private messages

#ifndef WINUTIL_STUB
	// headless configs don't expose RegisterWindowMessage
    m_ShowStageMessage = RegisterWindowMessage(SHOWSTAGE);
    m_ShowStageTop = RegisterWindowMessage(SHOWSTAGETOP);
    m_RealizePalette = RegisterWindowMessage(REALIZEPALETTE);
#endif

    return DoCreateWindow();
}

HRESULT CVideoWindow::DoCreateWindow()
{
    WNDCLASS wndclass;                  // Used to register classes
    BOOL bRegistered;                   // Is this class registered
    HWND hwnd;                          // Handle to our window

    bRegistered = GetClassInfo(m_hInstance,   // Module instance
                               m_pClassName,  // Window class
                               &wndclass);                 // Info structure

    // if the window is to be used for drawing puposes and we are getting a DC
    // for the entire lifetime of the window then changes the class style to do
    // say so. If we don't set this flag then the DC comes from the cache and is
    // really bad.
    if (m_bDoGetDC)
    {
#ifndef UNDER_CE
        m_ClassStyles |= CS_OWNDC;
#endif // UNDER_CE
    }

    // NOTE: Registered class names are scoped by process, so if quartz gets
    //   un/reloaded into a different spot (in the lifetime of host process), we must un/reregister 
    //   the wndclass with new WndProc location.  Can't directly compare wndclass.lpfnWndProc and 
    //   WndProc, as prior is mapped into kernel addr space, so always try to UnregisterClass if class 
    //   is registered.
    if (!bRegistered || UnregisterClass(m_pClassName, m_hInstance)) {

        // Register the renderer window class

        wndclass.lpszClassName = m_pClassName;
        wndclass.style         = m_ClassStyles;
        wndclass.lpfnWndProc   = (WNDPROC) WndProc;
        wndclass.cbClsExtra    = 0;
        wndclass.cbWndExtra    = sizeof(CBaseWindow *);
        wndclass.hInstance     = m_hInstance;
        wndclass.hIcon         = NULL;
#ifdef WINUTIL_STUB
        wndclass.hCursor       = NULL;
#else
        wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
#endif
		wndclass.hbrBackground = (HBRUSH) ::CreateSolidBrush (RGB (16, 0, 16));;
        wndclass.lpszMenuName  = NULL;

        RegisterClass(&wndclass);
    }

    // Create the frame window.  Pass the pBaseWindow information in the
    // CreateStruct which allows our message handling loop to get hold of
    // the pBaseWindow pointer.

    CBaseWindow *pBaseWindow = this;           // The owner window object
    hwnd = CreateWindowEx(m_WindowStylesEx,  // Extended styles
                          m_pClassName,      // Registered name
                          TEXT("ActiveMovie Window"),     // Window title
                          m_WindowStyles,    // Window styles
                          CW_USEDEFAULT,                  // Start x position
                          CW_USEDEFAULT,                  // Start y position
                          DEFWIDTH,                       // Window width
                          DEFHEIGHT,                      // Window height
                          NULL,                           // Parent handle
                          NULL,                           // Menu handle
                          m_hInstance,       // Instance handle
                          &pBaseWindow);                  // Creation data

    // If we failed signal an error to the object constructor (based on the
    // last Win32 error on this thread) then signal the constructor thread
    // to continue, release the mutex to let others have a go and exit

    if (hwnd == NULL) {
        DWORD Error = GetLastError();
        return HRESULT_FROM_WIN32(Error);
    }

    // Check the window LONG is the object who created us
    ASSERT(GetWindowLong(hwnd, 0) == (LONG)this);

    // Initialise the window and then signal the constructor so that it can
    // continue and then finally unlock the object's critical section. The
    // window class is left registered even after we terminate the thread
    // as we don't know when the last window has been closed. So we allow
    // the operating system to free the class resources as appropriate

    InitialiseWindow(hwnd);

    DbgLog((LOG_TRACE, 2, TEXT("Created window class (%s) HWND(%8.8X)"),
            m_pClassName, hwnd));

    return S_OK;
}
