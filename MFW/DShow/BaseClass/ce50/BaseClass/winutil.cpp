//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of your Microsoft Windows CE
// Source Alliance Program license form.  If you did not accept the terms of
// such a license, you are not authorized to use this source code.
//
//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//
//--------------------------------------------------------------------------;

// Generic window handler base class, December 1995

#include <streams.h>
#include <limits.h>
#include <dvdmedia.h>

#ifdef HEADLESSDSHOW
#define WINUTIL_STUB 1
#endif


#define STUBNORET    \
    RETAILMSG(1, (TEXT("Unexpected stub call in DirectShow base class.\r\n")));   \
    DebugBreak()

#define STUBRET(hr)     \
    STUBNORET;              \
    return hr

#define STUBFAIL	STUBRET(E_FAIL)



static UINT MsgDestroy;

// Constructor

CBaseWindow::CBaseWindow(BOOL bDoGetDC) :
    m_hInstance(g_hInst),
    m_hwnd(NULL),
    m_bActivated(FALSE),
    m_pClassName(NULL),
    m_ClassStyles(0),
    m_WindowStyles(0),
    m_WindowStylesEx(0),
    m_hdc(NULL),
    m_ShowStageMessage(0),
    m_ShowStageTop(0),
    m_MemoryDC(NULL),
    m_hPalette(NULL),
    m_bBackground(FALSE),
#ifdef DEBUG
    m_bRealizing(FALSE),
#endif
    m_bNoRealize(FALSE)
{
    m_bDoGetDC = bDoGetDC;
}


// Prepare a window by spinning off a worker thread to do the creation and
// also poll the message input queue. We leave this to be called by derived
// classes because they might want to override methods like MessageLoop and
// InitialiseWindow, if we do this during construction they'll ALWAYS call
// this base class methods. We make the worker thread create the window so
// it owns it rather than the filter graph thread which is constructing us

HRESULT CBaseWindow::PrepareWindow()
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


// Destructor just a placeholder so that we know it becomes virtual
// Derived classes MUST call DoneWithWindow in their destructors so
// that no messages arrive after the derived class constructor ends

#ifdef DEBUG
CBaseWindow::~CBaseWindow()
{
    ASSERT(m_hwnd == NULL);
    ASSERT(m_hdc == NULL);
}
#endif


// We use the sync worker event to have the window destroyed. All we do is
// signal the event and wait on the window thread handle. Trying to send it
// messages causes too many problems, furthermore to be on the safe side we
// just wait on the thread handle while it returns WAIT_TIMEOUT or there is
// a sent message to process on this thread. If the constructor failed to
// create the thread in the first place then the loop will get terminated

HRESULT CBaseWindow::DoneWithWindow()
{
    if (GetWindowThreadProcessId(m_hwnd, NULL) != GetCurrentThreadId()) {
#ifdef WINUTIL_STUB
		// headless configs don't expose RegisterWindowMessage
		// but we still need to send a "destroy" message to the owner thread.
        MsgDestroy = 0x7000; 
#else
        MsgDestroy = RegisterWindowMessage(TEXT("AM_DESTROY"));
#endif // WINUTIL_STUB
        SendMessage(m_hwnd, MsgDestroy, 0, 0);
        return NOERROR;
    }
    const HWND hwnd = m_hwnd;
    if (hwnd == NULL) {
        return NOERROR;
    }

    InactivateWindow();
    NOTE("Inactivated");

#ifndef WINUTIL_STUB // SetWindowLong with GWL_STYLE will eventually cause an OnSize hit, which is no good in HLBASE
    // Reset the window styles before destruction
    SetWindowLong(hwnd,GWL_STYLE,m_WindowStyles);
    ASSERT(GetParent(hwnd) == NULL);
    NOTE1("Reset window styles %d",m_WindowStyles);
#endif

    //  UnintialiseWindow sets m_hwnd to NULL so save a copy
    UninitialiseWindow();
    DbgLog((LOG_TRACE, 2, TEXT("Destroying 0x%8.8X"), hwnd));
    if (!DestroyWindow(hwnd)) {
        DbgLog((LOG_TRACE, 0, TEXT("DestroyWindow %8.8X failed code %d"),
                hwnd, GetLastError()));
        DbgBreak("");
    }

    // Reset our state so we can be prepared again

    m_pClassName = NULL;
    m_ClassStyles = 0;
    m_WindowStyles = 0;
    m_WindowStylesEx = 0;
    m_ShowStageMessage = 0;
    m_ShowStageTop = 0;

    return NOERROR;
}


// Called at the end to put the window in an inactive state. The pending list
// will always have been cleared by this time so event if the worker thread
// gets has been signaled and gets in to render something it will find both
// the state has been changed and that there are no available sample images
// Since we wait on the window thread to complete we don't lock the object

HRESULT CBaseWindow::InactivateWindow()
{
    // Has the window been activated
    if (m_bActivated == FALSE) {
        return S_FALSE;
    }

    m_bActivated = FALSE;
    ShowWindow(m_hwnd,SW_HIDE);
    return NOERROR;
}


HRESULT CBaseWindow::CompleteConnect()
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    m_bActivated = FALSE;
    return NOERROR;
#endif // WINUTIL_STUB
}

// This displays a normal window. We ask the base window class for default
// sizes which unless overriden will return DEFWIDTH and DEFHEIGHT. We go
// through a couple of extra hoops to get the client area the right size
// as the object specifies which accounts for the AdjustWindowRectEx calls
// We also DWORD align the left and top coordinates of the window here to
// maximise the chance of being able to use DCI/DirectDraw primary surface

HRESULT CBaseWindow::ActivateWindow()
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    // Has the window been sized and positioned already

    if (m_bActivated == TRUE || GetParent(m_hwnd) != NULL) {

        SetWindowPos(m_hwnd,            // Our window handle
                     HWND_TOP,          // Put it at the top
                     0, 0, 0, 0,        // Leave in current position
                     SWP_NOMOVE |       // Don't change it's place
                     SWP_NOSIZE);       // Change Z-order only

        return S_FALSE;
    }

    // Calculate the desired client rectangle

    RECT WindowRect, ClientRect = GetDefaultRect();
    GetWindowRect(m_hwnd,&WindowRect);
    AdjustWindowRectEx(&ClientRect,GetWindowLong(m_hwnd,GWL_STYLE),
                       FALSE,GetWindowLong(m_hwnd,GWL_EXSTYLE));

    // Align left and top edges on DWORD boundaries

    UINT WindowFlags = (SWP_NOACTIVATE | SWP_FRAMECHANGED);
    WindowRect.left -= (WindowRect.left & 3);
    WindowRect.top -= (WindowRect.top & 3);

    SetWindowPos(m_hwnd,                // Window handle
                 HWND_TOP,              // Put it at the top
                 WindowRect.left,       // Align left edge
                 WindowRect.top,        // And also top place
                 WIDTH(&ClientRect),    // Horizontal size
                 HEIGHT(&ClientRect),   // Vertical size
                 WindowFlags);          // Don't show window

    m_bActivated = TRUE;
    return NOERROR;
#endif // WINUTIL_STUB
}


// This can be used to DWORD align the window for maximum performance

HRESULT CBaseWindow::PerformanceAlignWindow()
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    RECT ClientRect,WindowRect;
    GetWindowRect(m_hwnd,&WindowRect);
    ASSERT(m_bActivated == TRUE);

    // Don't do this if we're owned

    if (GetParent(m_hwnd)) {
        return NOERROR;
    }

    // Align left and top edges on DWORD boundaries

    GetClientRect(m_hwnd, &ClientRect);
    MapWindowPoints(m_hwnd, HWND_DESKTOP, (LPPOINT) &ClientRect, 2);
    WindowRect.left -= (ClientRect.left & 3);
    WindowRect.top  -= (ClientRect.top  & 3);
    UINT WindowFlags = (SWP_NOACTIVATE | SWP_NOSIZE);

    SetWindowPos(m_hwnd,                // Window handle
                 HWND_TOP,              // Put it at the top
                 WindowRect.left,       // Align left edge
                 WindowRect.top,        // And also top place
                 (int) 0,(int) 0,       // Ignore these sizes
                 WindowFlags);          // Don't show window

    return NOERROR;
#endif // WINUTIL_STUB
}



// Install a palette into the base window - we may be called by a different
// thread to the one that owns the window. We have to be careful how we do
// the palette realisation as we could be a different thread to the window
// which would cause an inter thread send message. Therefore we realise the
// palette by sending it a special message but without the window locked

HRESULT CBaseWindow::SetPalette(HPALETTE hPalette)
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    // We must own the window lock during the change
    {
        CAutoLock cWindowLock(&m_WindowLock);
        ASSERT(hPalette);
        m_hPalette = hPalette;
    }
    return SetPalette();
#endif WINUTIL_STUB
}

HRESULT CBaseWindow::SetPalette()
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    if (!m_bNoRealize) {
        SendMessage(m_hwnd, m_RealizePalette, 0, 0);
        // Make sure the device's palette is flushed
#ifndef UNDER_CE
        return (GdiFlush() == FALSE ? S_FALSE : S_OK);
#else
        return S_OK;
#endif // UNDER_CE
    } else {
        // Just select the palette
        ASSERT(m_hdc);
        ASSERT(m_MemoryDC);
        SelectPalette(m_hdc,m_hPalette,m_bBackground);
        SelectPalette(m_MemoryDC,m_hPalette,m_bBackground);
        return S_OK;
    }
#endif // WINUTIL_STUB
}

// Realise our palettes in the window and device contexts

HRESULT CBaseWindow::DoRealisePalette(BOOL bForceBackground)
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    //  If we grab a critical section here we can deadlock
    //  with the window thread because one of the side effects
    //  of RealizePalette is to send a WM_PALETTECHANGED message
    //  to every window in the system.  In our handling
    //  of WM_PALETTECHANGED we used to grab this CS too.
    //  The really bad case is when our renderer calls DoRealisePalette()
    //  while we're in the middle of processing a palette change
    //  for another window.
    //  So don't hold the critical section while actually realising
    //  the palette.  In any case USER is meant to manage palette
    //  handling - we shouldn't have to serialize everything as well
    if (m_hPalette == NULL) {

        return NOERROR;
    }

    // Realize the palette on the window thread
    ASSERT(m_hdc);
    ASSERT(m_MemoryDC);
    SelectPalette(m_hdc,m_hPalette,m_bBackground || bForceBackground);
    SelectPalette(m_MemoryDC,m_hPalette,m_bBackground);
#ifndef UNDER_CE
    EXECUTE_ASSERT(RealizePalette(m_hdc) != GDI_ERROR);
    EXECUTE_ASSERT(RealizePalette(m_MemoryDC) != GDI_ERROR);

    return (GdiFlush() == FALSE ? S_FALSE : S_OK);
#else
    // RealizePalette will fail on CE if our window/thread was not created by 
    // the current foreground window's process.  (i.e., we've been backgrounded)
    RealizePalette(m_hdc);
    RealizePalette(m_MemoryDC);

    return S_OK;
#endif
#endif // WINUTIL_STUB
}


// This is the global window procedure

LRESULT CALLBACK WndProc(HWND hwnd,         // Window handle
                         UINT uMsg,         // Message ID
                         WPARAM wParam,     // First parameter
                         LPARAM lParam)     // Other parameter
{

    // Get the window long that holds our window object pointer
    // If it is NULL then we are initialising the window in which
    // case the object pointer has been passed in the window creation
    // structure.  IF we get any messages before WM_NCCREATE we will
    // pass them to DefWindowProc.

    CBaseWindow *pBaseWindow = (CBaseWindow *)GetWindowLong(hwnd,0);
    if (pBaseWindow == NULL) {

	// Get the structure pointer from the create struct.
	// We can only do this for WM_NCCREATE which should be one of
	// the first messages we receive.  Anything before this will
	// have to be passed to DefWindowProc (i.e. WM_GETMINMAXINFO)

	// If the message is WM_NCCREATE we set our pBaseWindow pointer
	// and will then place it in the window structure

#ifndef UNDER_CE
	if ((uMsg != WM_NCCREATE)
	    || (NULL == (pBaseWindow = *(CBaseWindow**) ((LPCREATESTRUCT)lParam)->lpCreateParams)))
#else
    if ((uMsg != WM_CREATE)
        || (NULL == (pBaseWindow = *(CBaseWindow**) ((LPCREATESTRUCT)lParam)->lpCreateParams)))
#endif // UNDER_CE
	{
	    return(DefWindowProc(hwnd, uMsg, wParam, lParam));
	}

        // Set the window LONG to be the object who created us
#ifdef DEBUG
	SetLastError(0);  // because of the way SetWindowLong works
#endif
        BOOL rc = SetWindowLong(hwnd, (DWORD) 0, (LONG) pBaseWindow);
#ifdef DEBUG
	if (0 == rc) {
	    // SetWindowLong MIGHT have failed.  (Read the docs which admit
	    // that it is awkward to work out if you have had an error.)
	    LONG lasterror = GetLastError();
	    ASSERT(0 == lasterror);
	    // If this is not the case we have not set the pBaseWindow pointer
	    // into the window structure and we will blow up.
	}
#endif

    }
    // See if this is the packet of death
    if (uMsg == MsgDestroy && uMsg != 0) {
        pBaseWindow->DoneWithWindow();
        return 0;
    }
    return pBaseWindow->OnReceiveMessage(hwnd,uMsg,wParam,lParam);
}


// When the window size changes we adjust our member variables that
// contain the dimensions of the client rectangle for our window so
// that we come to render an image we will know whether to stretch

BOOL CBaseWindow::OnSize(LONG Width, LONG Height)
{
#ifdef WINUTIL_STUB
    STUBRET(FALSE);
#else
    m_Width = Width;
    m_Height = Height;
    return TRUE;
#endif // WINUTIL_STUB
}


// This function handles the WM_CLOSE message

BOOL CBaseWindow::OnClose()
{
#ifdef WINUTIL_STUB
    STUBRET(FALSE);
#else
    ShowWindow(m_hwnd,SW_HIDE);
    return TRUE;
#endif // WINUTIL_STUB
}


// This is called by the worker window thread when it receives a terminate
// message from the window object destructor to delete all the resources we
// allocated during initialisation. By the time the worker thread exits all
// processing will have been completed as the source filter disconnection
// flushes the image pending sample, therefore the GdiFlush should succeed

HRESULT CBaseWindow::UninitialiseWindow()
{
    // Have we already cleaned up

    if (m_hwnd == NULL) {
        ASSERT(m_hdc == NULL);
        ASSERT(m_MemoryDC == NULL);
        return NOERROR;
    }

    // Release the window resources

#ifndef UNDER_CE
    EXECUTE_ASSERT(GdiFlush());
#endif // UNDER_CE

    if (m_hdc)
    {
        EXECUTE_ASSERT(ReleaseDC(m_hwnd,m_hdc));
        m_hdc = NULL;
    }

#ifndef WINUTIL_STUB
    if (m_MemoryDC)
    {
        EXECUTE_ASSERT(DeleteDC(m_MemoryDC));
        m_MemoryDC = NULL;
    }
#endif // WINUTIL_STUB

    // Reset the window variables
    m_hwnd = NULL;

    return NOERROR;
}


// This is called by the worker window thread after it has created the main
// window and it wants to initialise the rest of the owner objects window
// variables such as the device contexts. We execute this function with the
// critical section still locked. Nothing in this function must generate any
// SendMessage calls to the window because this is executing on the window
// thread so the message will never be processed and we will deadlock

HRESULT CBaseWindow::InitialiseWindow(HWND hwnd)
{
    // Initialise the window variables

    ASSERT(IsWindow(hwnd));
    m_hwnd = hwnd;

    if (m_bDoGetDC)
    {
        EXECUTE_ASSERT(m_hdc = GetDC(hwnd));

#ifndef WINUTIL_STUB
        EXECUTE_ASSERT(SetStretchBltMode(m_hdc,COLORONCOLOR));
        EXECUTE_ASSERT(m_MemoryDC = CreateCompatibleDC(m_hdc));
        EXECUTE_ASSERT(SetStretchBltMode(m_MemoryDC,COLORONCOLOR));
#endif // WINUTIL_STUB
    }

    return NOERROR;
}

HRESULT CBaseWindow::DoCreateWindow()
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
        wndclass.hbrBackground = (HBRUSH) NULL;
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


// The base class provides some default handling and calls DefWindowProc

LRESULT CBaseWindow::OnReceiveMessage(HWND hwnd,         // Window handle
                                      UINT uMsg,         // Message ID
                                      WPARAM wParam,     // First parameter
                                      LPARAM lParam)     // Other parameter
{
    ASSERT(IsWindow(hwnd));

    if (PossiblyEatMessage(uMsg, wParam, lParam))
	return 0;

    // This is sent by the IVideoWindow SetWindowForeground method. If the
    // window is invisible we will show it and make it topmost without the
    // foreground focus. If the window is visible it will also be made the
    // topmost window without the foreground focus. If wParam is TRUE then
    // for both cases the window will be forced into the foreground focus

#ifndef WINUTIL_STUB
    if (uMsg == m_ShowStageMessage) {

        BOOL bVisible = IsWindowVisible(hwnd);
        SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW |
                     (bVisible ? SWP_NOACTIVATE : 0));

#ifdef BUGBUG
        // Should we bring the window to the foreground
        if (wParam == TRUE) {
            SetForegroundWindow(hwnd);
        }
#else
        SetForegroundWindow(hwnd);

        // Should we give the window focus
        if (wParam == TRUE) {
            SetFocus(hwnd);    
        }
#endif

        return (LRESULT) 1;
    }

    // When we go fullscreen we have to add the WS_EX_TOPMOST style to the
    // video window so that it comes out above any task bar (this is more
    // relevant to WindowsNT than Windows95). However the SetWindowPos call
    // must be on the same thread as that which created the window. The
    // wParam parameter can be TRUE or FALSE to set and reset the topmost

    if (uMsg == m_ShowStageTop) {
        HWND HwndTop = (wParam == TRUE ? HWND_TOPMOST : HWND_NOTOPMOST);
        BOOL bVisible = IsWindowVisible(hwnd);
        SetWindowPos(hwnd, HwndTop, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE |
                     (wParam == TRUE ? SWP_SHOWWINDOW : 0) |
                     (bVisible ? SWP_NOACTIVATE : 0));
        return (LRESULT) 1;
    }

    // New palette stuff
    if (uMsg == m_RealizePalette) {
        ASSERT(m_hwnd == hwnd);
        return OnPaletteChange(m_hwnd,WM_QUERYNEWPALETTE);
    }
#endif // WINUTIL_STUB

    switch (uMsg) {

#ifndef WINUTIL_STUB
        case WM_SETTINGCHANGE:
             if (SETTINGCHANGE_RESET == wParam && 0 == lParam) {

                 DeleteDC(m_MemoryDC);
                 ReleaseDC(hwnd, m_hdc);

                 m_hdc = GetDC(hwnd);
                 m_MemoryDC = CreateCompatibleDC(m_hdc);
             }

             return (LRESULT) 0;
#endif // WINUTIL_STUB

        // Repaint the window if the system colours change

        case WM_SYSCOLORCHANGE:

            InvalidateRect(hwnd,NULL,FALSE);
            return (LRESULT) 1;

        // Somebody has changed the palette
        case WM_PALETTECHANGED:

            OnPaletteChange((HWND)wParam,uMsg);
            return (LRESULT) 0;

        // We are about to receive the keyboard focus so we ask GDI to realise
        // our logical palette again and hopefully it will be fully installed
        // without any mapping having to be done during any picture rendering

	    case WM_QUERYNEWPALETTE:
	        ASSERT(m_hwnd == hwnd);
            return OnPaletteChange(m_hwnd,uMsg);

        // Store the width and height as useful base class members

        case WM_SIZE:

	        OnSize(LOWORD(lParam), HIWORD(lParam));
            return (LRESULT) 0;

        // Intercept the WM_CLOSE messages to hide the window

        case WM_CLOSE:

            OnClose();
            return (LRESULT) 0;
    }
    return DefWindowProc(hwnd,uMsg,wParam,lParam);
}


// This handles the Windows palette change messages - if we do realise our
// palette then we return TRUE otherwise we return FALSE. If our window is
// foreground application then we should get first choice of colours in the
// system palette entries. We get best performance when our logical palette
// includes the standard VGA colours (at the beginning and end) otherwise
// GDI may have to map from our palette to the device palette while drawing

LRESULT CBaseWindow::OnPaletteChange(HWND hwnd,UINT Message)
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    // First check we are not changing the palette during closedown

    if (m_hwnd == NULL || hwnd == NULL) {
        return (LRESULT) 0;
    }
    ASSERT(!m_bRealizing);

    // Should we realise our palette again

    if ((Message == WM_QUERYNEWPALETTE || hwnd != m_hwnd)) {
        //  It seems that even if we're invisible that we can get asked
        //  to realize our palette and this can cause really ugly side-effects
        if (!IsWindowVisible(m_hwnd)) {
            DbgLog((LOG_TRACE, 1, TEXT("Realizing when invisible!")));
            return (LRESULT) 0;
        }

        // Avoid recursion with multiple graphs in the same app
#ifdef DEBUG
        m_bRealizing = TRUE;
#endif
        DoRealisePalette(Message != WM_QUERYNEWPALETTE);
#ifdef DEBUG
        m_bRealizing = FALSE;
#endif

        // Should we redraw the window with the new palette
        if (Message == WM_PALETTECHANGED) {
            InvalidateRect(m_hwnd,NULL,FALSE);
        }
    }

    return (LRESULT) 1;
#endif // WINUTIL_STUB
}

// Return the default window rectangle

RECT CBaseWindow::GetDefaultRect()
{
#ifdef WINUTIL_STUB
    RECT DefaultRect = {0,0,640,480};
    STUBRET(DefaultRect);
#else
    RECT DefaultRect = {0,0,DEFWIDTH,DEFHEIGHT};
    ASSERT(m_hwnd);
    return DefaultRect;
#endif // WINUTIL_STUB
}


// Return the current window width

LONG CBaseWindow::GetWindowWidth()
{
#ifdef WINUTIL_STUB
    STUBRET(640);
#else
    ASSERT(m_hwnd);
    return m_Width;
#endif // WINUTIL_STUB
}


// Return the current window height

LONG CBaseWindow::GetWindowHeight()
{
#ifdef WINUTIL_STUB
    STUBRET(480);
#else
    ASSERT(m_hwnd);
    return m_Height;
#endif // WINUTIL_STUB
}


// Return the window handle

HWND CBaseWindow::GetWindowHWND()
{
    ASSERT(m_hwnd);
    return m_hwnd;
}


// Return the window drawing device context

HDC CBaseWindow::GetWindowHDC()
{
#ifdef WINUTIL_STUB
    STUBRET((HDC)NULL);
#else
    ASSERT(m_hwnd);
    ASSERT(m_hdc);
    return m_hdc;
#endif // WINUTIL_STUB
}


// Return the offscreen window drawing device context

HDC CBaseWindow::GetMemoryHDC()
{
#ifdef WINUTIL_STUB
    STUBRET((HDC)NULL);
#else
    ASSERT(m_hwnd);
    ASSERT(m_MemoryDC);
    return m_MemoryDC;
#endif // WINUTIL_STUB
}


// This is available to clients who want to change the window visiblity. It's
// little more than an indirection to the Win32 ShowWindow although these is
// some benefit in going through here as this function may change sometime

HRESULT CBaseWindow::DoShowWindow(LONG ShowCmd)
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    ShowWindow(m_hwnd,ShowCmd);
    return NOERROR;
#endif // WINUTIL_STUB
}


// Generate a WM_PAINT message for the video window

void CBaseWindow::PaintWindow(BOOL bErase)
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    InvalidateRect(m_hwnd,NULL,bErase);
#endif // WINUTIL_STUB
}


// Allow an application to have us set the video window in the foreground. We
// have this because it is difficult for one thread to do do this to a window
// owned by another thread. Rather than expose the message we use to execute
// the inter thread send message we provide the interface function. All we do
// is to SendMessage to the video window renderer thread with a WM_SHOWSTAGE

void CBaseWindow::DoSetWindowForeground(BOOL bFocus)
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    SendMessage(m_hwnd,m_ShowStageMessage,(WPARAM) bFocus,(LPARAM) 0);
#endif // WINUTIL_STUB
}


// Constructor initialises the owning object pointer. Since we are a worker
// class for the main window object we have relatively few state variables to
// look after. We are given device context handles to use later on as well as
// the source and destination rectangles (but reset them here just in case)

CDrawImage::CDrawImage(CBaseWindow *pBaseWindow)
#ifndef WINUTIL_STUB
    :
    m_pBaseWindow(pBaseWindow),
    m_hdc(NULL),
    m_MemoryDC(NULL),
    m_bStretch(FALSE),
    m_pMediaType(NULL),
    m_bUsingImageAllocator(FALSE)
#endif // WINUTIL_STUB
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    ASSERT(pBaseWindow);
    ResetPaletteVersion();
    SetRectEmpty(&m_TargetRect);
    SetRectEmpty(&m_SourceRect);

    m_perfidRenderTime = MSR_REGISTER("Single Blt time");
#endif // WINUTIL_STUB
}


// Overlay the image time stamps on the picture. Access to this method is
// serialised by the caller. We display the sample start and end times on
// top of the video using TextOut on the device context we are handed. If
// there isn't enough room in the window for the times we don't show them

void CDrawImage::DisplaySampleTimes(IMediaSample *pSample)
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    TCHAR szTimes[TIMELENGTH];      // Time stamp strings
    ASSERT(pSample);                // Quick sanity check
    RECT ClientRect;                // Client window size
    SIZE Size;                      // Size of text output

    // Get the time stamps and window size

    pSample->GetTime((REFERENCE_TIME*)&m_StartSample, (REFERENCE_TIME*)&m_EndSample);
    HWND hwnd = m_pBaseWindow->GetWindowHWND();
    EXECUTE_ASSERT(GetClientRect(hwnd,&ClientRect));

    // Format the sample time stamps

    wsprintf(szTimes,TEXT("%08d : %08d"),
             m_StartSample.Millisecs(),
             m_EndSample.Millisecs());

    ASSERT(lstrlen(szTimes) < TIMELENGTH);

    // Put the times in the middle at the bottom of the window

    GetTextExtentPoint32(m_hdc,szTimes,lstrlen(szTimes),&Size);
    INT XPos = ((ClientRect.right - ClientRect.left) - Size.cx) / 2;
    INT YPos = ((ClientRect.bottom - ClientRect.top) - Size.cy) * 4 / 5;

    // Check the window is big enough to have sample times displayed

    if ((XPos > 0) && (YPos > 0)) {
#ifndef UNDER_CE
        TextOut(m_hdc,XPos,YPos,szTimes,lstrlen(szTimes));
#else
        ExtTextOut(m_hdc,XPos, YPos,
                   ETO_CLIPPED | ETO_OPAQUE, // useless since no rect specified
                   NULL, szTimes, lstrlen(szTimes), NULL);
#endif // UNDER_CE
    }
#endif // WINUTIL_STUB
}


// This is called when the drawing code sees that the image has a down level
// palette cookie. We simply call the SetDIBColorTable Windows API with the
// palette that is found after the BITMAPINFOHEADER - we return no errors

void CDrawImage::UpdateColourTable(HDC hdc,BITMAPINFOHEADER *pbmi)
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    ASSERT(pbmi->biClrUsed);
    RGBQUAD *pColourTable = (RGBQUAD *)(pbmi+1);

    // Set the new palette in the device context

#ifndef UNDER_CE
    UINT uiReturn = SetDIBColorTable(hdc,(UINT) 0,
                                     pbmi->biClrUsed,
                                     pColourTable);

    // Should always succeed but check in debug builds
    ASSERT(uiReturn == pbmi->biClrUsed);
#else
    DbgOutString(TEXT("WinCE: SetDIBColorTable() NOT supported!\n"));
#endif // UNDER_CE
#endif // WINUTIL_STUB
}


// No source rectangle scaling is done by the base class

RECT CDrawImage::ScaleSourceRect(const RECT *pSource)
{
#ifdef WINUTIL_STUB
    STUBNORET;
#endif // WINUTIL_STUB
    ASSERT(pSource);
    return *pSource;
}


// This is called when the funky output pin uses our allocator. The samples we
// allocate are special because the memory is shared between us and GDI thus
// removing one copy when we ask for the image to be rendered. The source type
// information is in the main renderer m_mtIn field which is initialised when
// the media type is agreed in SetMediaType, the media type may be changed on
// the fly if, for example, the source filter needs to change the palette

void CDrawImage::FastRender(IMediaSample *pMediaSample)
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    BITMAPINFOHEADER *pbmi;     // Image format data
    DIBDATA *pDibData;          // Stores DIB information
    BYTE *pImage;               // Pointer to image data
    HBITMAP hOldBitmap;         // Store the old bitmap
    CImageSample *pSample;      // Pointer to C++ object

    ASSERT(m_pMediaType);

    // From the untyped source format block get the BITMAPINFOHEADER structure. 
	// We can cast the IMediaSample interface
    // to a CImageSample object so we can retrieve it's DIBSECTION details

	pbmi = GetBitmapInfoHeader (m_pMediaType);
	ASSERT (pbmi);
	if (!pbmi)
	{
		return;
	}
    pSample = (CImageSample *) pMediaSample;
    pDibData = pSample->GetDIBData();
    hOldBitmap = (HBITMAP) SelectObject(m_MemoryDC,pDibData->hBitmap);

    // Get a pointer to the real image data

    HRESULT hr = pMediaSample->GetPointer(&pImage);
    if (FAILED(hr)) {
        return;
    }

    // Do we need to update the colour table, we increment our palette cookie
    // each time we get a dynamic format change. The sample palette cookie is
    // stored in the DIBDATA structure so we try to keep the fields in sync
    // By the time we get to draw the images the format change will be done
    // so all we do is ask the renderer for what it's palette version is

    if (pDibData->PaletteVersion < GetPaletteVersion()) {
        ASSERT(pbmi->biBitCount <= iPALETTE);
        UpdateColourTable(m_MemoryDC,pbmi);
        pDibData->PaletteVersion = GetPaletteVersion();
    }

    // This allows derived classes to change the source rectangle that we do
    // the drawing with. For example a renderer may ask a codec to stretch
    // the video from 320x240 to 640x480, in which case the source we see in
    // here will still be 320x240, although the source we want to draw with
    // should be scaled up to 640x480. The base class implementation of this
    // method does nothing but return the same rectangle as we are passed in

    RECT SourceRect = ScaleSourceRect(&m_SourceRect);

    // Is the window the same size as the video

    if (m_bStretch == FALSE) {

        // Put the image straight into the window

        BitBlt(
            (HDC) m_hdc,                            // Target device HDC
            m_TargetRect.left,                      // X sink position
            m_TargetRect.top,                       // Y sink position
            m_TargetRect.right - m_TargetRect.left, // Destination width
            m_TargetRect.bottom - m_TargetRect.top, // Destination height
            m_MemoryDC,                             // Source device context
            SourceRect.left,                        // X source position
            SourceRect.top,                         // Y source position
            SRCCOPY);                               // Simple copy

    } else {

        // Stretch the image when copying to the window

        StretchBlt(
            (HDC) m_hdc,                            // Target device HDC
            m_TargetRect.left,                      // X sink position
            m_TargetRect.top,                       // Y sink position
            m_TargetRect.right - m_TargetRect.left, // Destination width
            m_TargetRect.bottom - m_TargetRect.top, // Destination height
            m_MemoryDC,                             // Source device HDC
            SourceRect.left,                        // X source position
            SourceRect.top,                         // Y source position
            SourceRect.right - SourceRect.left,     // Source width
            SourceRect.bottom - SourceRect.top,     // Source height
            SRCCOPY);                               // Simple copy
    }

    // This displays the sample times over the top of the image. This used to
    // draw the times into the offscreen device context however that actually
    // writes the text into the image data buffer which may not be writable

    #ifdef DEBUG
    DisplaySampleTimes(pMediaSample);
    #endif

    // Put the old bitmap back into the device context so we don't leak
    SelectObject(m_MemoryDC,hOldBitmap);
#endif // WINUTIL_STUB
}

#ifdef UNDER_CE
#ifndef WINUTIL_STUB
// Secret MGDI function prototype...
extern "C" {
WINGDIAPI HBITMAP WINAPI
CreateBitmapFromPointer(
    CONST BITMAPINFO *pbmi,
    int                   iStride,
    PVOID             pvBits
);
}
#endif // WINUTIL_STUB


WINGDIAPI int   WINAPI CeStretchDIBits(
	HDC hdc,        // handle of device context
    int XDest,  // x-coordinate of upper-left corner of dest. rect.
    int YDest,  // y-coordinate of upper-left corner of dest. rect.
    int nDestWidth,     // width of destination rectangle
    int nDestHeight,    // height of destination rectangle
    int XSrc,   // x-coordinate of upper-left corner of source rect.
    int YSrc,   // y-coordinate of upper-left corner of source rect.
    int nSrcWidth,      // width of source rectangle
    int nSrcHeight,     // height of source rectangle
    CONST VOID *lpBits, // address of bitmap bits
    CONST BITMAPINFO *lpBitsInfo,       // address of bitmap data
    UINT iUsage,        // usage
    DWORD dwRop         // raster operation code
)
{
#ifdef WINUTIL_STUB
    STUBRET(GDI_ERROR);
#else

    HBITMAP     hbm = NULL;
    HBITMAP     hbmOld;
    int         iStride;
    HDC         hdcSrc;
    int         iRet;
    PLOGPALETTE pPal;
    HPALETTE    hpal = NULL;
    HPALETTE    hpalOld;

    // Check input pointers
    if ((lpBitsInfo == NULL) || (lpBits == NULL))
	    return GDI_ERROR;

    // Stride is bitmap width in bytes, DWORD aligned
    iStride = ((lpBitsInfo->bmiHeader.biWidth * lpBitsInfo->bmiHeader.biBitCount / 8) + 3) & ~3;

    // Make a source DC to blt from
    hdcSrc = CreateCompatibleDC(hdc);
    if (hdcSrc == NULL)
    {
	    iRet = GDI_ERROR;
	    goto cleanup;
    }

    // Wrap a GDI BITMAP object around these bitmap bits, using "secret" MGDI function
    hbm = CreateBitmapFromPointer(lpBitsInfo, iStride, (PVOID)lpBits);
    if (hbm == NULL)
    {
	    iRet = GDI_ERROR;
	    goto cleanup;
    }
    hbmOld = (HBITMAP)SelectObject(hdcSrc, hbm);
    if (hbmOld == NULL)
    {
	    iRet = GDI_ERROR;
	    goto cleanup;
    }

    // Do we need a palette?
    if ((lpBitsInfo->bmiHeader.biCompression == BI_RGB) && (iUsage == DIB_RGB_COLORS) && (lpBitsInfo->bmiHeader.biBitCount <= 8))
    {
	    DWORD cColors = lpBitsInfo->bmiHeader.biClrUsed;
	
	    if (cColors == 0)
	    {
	        cColors = 1 << lpBitsInfo->bmiHeader.biBitCount;
	    }
	    pPal = (PLOGPALETTE)LocalAlloc(LMEM_FIXED, sizeof(LOGPALETTE) +
						   sizeof(PALETTEENTRY) * cColors);
	    if (pPal == NULL)
	    {
	        iRet = GDI_ERROR;
	        goto cleanup;
	    }
	    pPal->palVersion = 0x300;
	    pPal->palNumEntries = (WORD)cColors;
	    for (WORD idx = 0; idx < cColors; idx++)
	    {
	        pPal->palPalEntry[idx].peRed   = lpBitsInfo->bmiColors[idx].rgbRed;
	        pPal->palPalEntry[idx].peGreen = lpBitsInfo->bmiColors[idx].rgbGreen;
	        pPal->palPalEntry[idx].peBlue  = lpBitsInfo->bmiColors[idx].rgbBlue;
	        pPal->palPalEntry[idx].peFlags = 0;
	    }
	    hpal = CreatePalette(pPal);
	    if (hpal == NULL)
	    {
	        LocalFree(pPal);
	        iRet = GDI_ERROR;
	        goto cleanup;
	    }
	    hpalOld = SelectPalette(hdcSrc, hpal, FALSE);
	    LocalFree(pPal);
    }

    // Do the Blt

    if ( StretchBlt(hdc,    XDest,  YDest,  nDestWidth, nDestHeight,
		    hdcSrc, XSrc,   YSrc,   nSrcWidth,  nSrcHeight, dwRop))
    {
    	// REVIEW martsh: doesn't take into account clipping.  Should this be nDestHeight?
	    iRet = nSrcHeight > 0 ? nSrcHeight : -nSrcHeight;
    }
    else
    {
	    iRet = GDI_ERROR;
    }

cleanup:

    if (hbm)
    {
	    SelectObject(hdcSrc, hbmOld);
	    DeleteObject(hbm);
    }

    // Did we use a palette?
    if (hpal)
    {
	    SelectPalette(hdcSrc, hpalOld, FALSE);
	    DeleteObject(hpal);
    }
    if (hdcSrc)
    {
	    DeleteDC(hdcSrc);
    }

	return iRet;
#endif // WINUTIL_STUB
}
#endif // UNDER_CE


// This is called when there is a sample ready to be drawn, unfortunately the
// output pin was being rotten and didn't choose our super excellent shared
// memory DIB allocator so we have to do this slow render using boring old GDI
// SetDIBitsToDevice and StretchDIBits. The down side of using these GDI
// functions is that the image data has to be copied across from our address
// space into theirs before going to the screen (although in reality the cost
// is small because all they do is to map the buffer into their address space)

void CDrawImage::SlowRender(IMediaSample *pMediaSample)
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    // Get the BITMAPINFOHEADER for the connection

    ASSERT(m_pMediaType);
    BITMAPINFOHEADER *pbmi = GetBitmapInfoHeader (m_pMediaType);
    BYTE *pImage;

    // Get the image data buffer

    HRESULT hr = pMediaSample->GetPointer(&pImage);
    if (FAILED(hr)) {
        return;
    }

    // This allows derived classes to change the source rectangle that we do
    // the drawing with. For example a renderer may ask a codec to stretch
    // the video from 320x240 to 640x480, in which case the source we see in
    // here will still be 320x240, although the source we want to draw with
    // should be scaled up to 640x480. The base class implementation of this
    // method does nothing but return the same rectangle as we are passed in

    RECT SourceRect = ScaleSourceRect(&m_SourceRect);

    LONG lAdjustedSourceTop = SourceRect.top;
    // if the origin of bitmap is bottom-left, adjust soruce_rect_top
    // to be the bottom-left corner instead of the top-left.
    if (pbmi->biHeight > 0) {
       lAdjustedSourceTop = pbmi->biHeight - SourceRect.bottom;
    }
    // Is the window the same size as the video

#ifndef UNDER_CE
    if (m_bStretch == FALSE) {

        // Put the image straight into the window

        SetDIBitsToDevice(
            (HDC) m_hdc,                            // Target device HDC
            m_TargetRect.left,                      // X sink position
            m_TargetRect.top,                       // Y sink position
            m_TargetRect.right - m_TargetRect.left, // Destination width
            m_TargetRect.bottom - m_TargetRect.top, // Destination height
            SourceRect.left,                        // X source position
            lAdjustedSourceTop,                     // Adjusted Y source position
            (UINT) 0,                               // Start scan line
            pbmi->biHeight,                         // Scan lines present
            pImage,                                 // Image data
            (BITMAPINFO *) pbmi,                    // DIB header
            DIB_RGB_COLORS);                        // Type of palette

    } else {

        // Stretch the image when copying to the window

        StretchDIBits(
            (HDC) m_hdc,                            // Target device HDC
            m_TargetRect.left,                      // X sink position
            m_TargetRect.top,                       // Y sink position
            m_TargetRect.right - m_TargetRect.left, // Destination width
            m_TargetRect.bottom - m_TargetRect.top, // Destination height
            SourceRect.left,                        // X source position
            lAdjustedSourceTop,                     // Adjusted Y source position
            SourceRect.right - SourceRect.left,     // Source width
            SourceRect.bottom - SourceRect.top,     // Source height
            pImage,                                 // Image data
            (BITMAPINFO *) pbmi,                    // DIB header
            DIB_RGB_COLORS,                         // Type of palette
            SRCCOPY);                               // Simple image copy
    }
#else // !UNDER_CE
    CeStretchDIBits(
        (HDC) m_hdc,                            // Target device HDC
        m_TargetRect.left,                      // X sink position
        m_TargetRect.top,                       // Y sink position
        m_TargetRect.right - m_TargetRect.left, // Destination width
        m_TargetRect.bottom - m_TargetRect.top, // Destination height
        SourceRect.left,                        // X source position
        lAdjustedSourceTop,                     // Adjusted Y source position
        SourceRect.right - SourceRect.left,     // Source width
        SourceRect.bottom - SourceRect.top,     // Source height
        pImage,                                 // Image data
        (BITMAPINFO *) pbmi,                    // DIB header
        DIB_RGB_COLORS,                         // Type of palette
        SRCCOPY);                               // Simple image copy
#endif // !UNDER_CE

    // This shows the sample reference times over the top of the image which
    // looks a little flickery. I tried using GdiSetBatchLimit and GdiFlush to
    // control the screen updates but it doesn't quite work as expected and
    // only partially reduces the flicker. I also tried using a memory context
    // and combining the two in that before doing a final BitBlt operation to
    // the screen, unfortunately this has considerable performance penalties
    // and also means that this code is not executed when compiled retail

    #ifdef DEBUG
    DisplaySampleTimes(pMediaSample);
    #endif
#endif // WINUTIL_STUB
}


// This is called with an IMediaSample interface on the image to be drawn. We
// decide on the drawing mechanism based on who's allocator we are using. We
// may be called when the window wants an image painted by WM_PAINT messages
// We can't realise the palette here because we have the renderer lock, any
// call to realise may cause an interthread send message to the window thread
// which may in turn be waiting to get the renderer lock before servicing it

BOOL CDrawImage::DrawImage(IMediaSample *pMediaSample)
{
#ifdef WINUTIL_STUB
    STUBRET(FALSE);
#else
    SetDrawContext();
    ASSERT(m_hdc);
    ASSERT(m_MemoryDC);
    NotifyStartDraw();

    // If the output pin used our allocator then the samples passed are in
    // fact CVideoSample objects that contain CreateDIBSection data that we
    // use to do faster image rendering, they may optionally also contain a
    // DirectDraw surface pointer in which case we do not do the drawing

    if (m_bUsingImageAllocator == FALSE) {
        SlowRender(pMediaSample);
#ifndef UNDER_CE
        EXECUTE_ASSERT(GdiFlush());
#endif // UNDER_CE
        NotifyEndDraw();
        return TRUE;
    }

    // This is a DIBSECTION buffer

    FastRender(pMediaSample);
#ifndef UNDER_CE
    EXECUTE_ASSERT(GdiFlush());
#endif // UNDER_CE
    NotifyEndDraw();
    return TRUE;
#endif // WINUTIL_STUB
}


// This is called by the owning window object after it has created the window
// and it's drawing contexts. We are constructed with the base window we'll
// be drawing into so when given the notification we retrive the device HDCs
// to draw with. We cannot call these in our constructor as they are virtual

void CDrawImage::SetDrawContext()
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    m_MemoryDC = m_pBaseWindow->GetMemoryHDC();
    m_hdc = m_pBaseWindow->GetWindowHDC();
#endif // WINUTIL_STUB
}


// This is called to set the target rectangle in the video window, it will be
// called whenever a WM_SIZE message is retrieved from the message queue. We
// simply store the rectangle and use it later when we do the drawing calls

void CDrawImage::SetTargetRect(RECT *pTargetRect)
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    ASSERT(pTargetRect);
    m_TargetRect = *pTargetRect;
    SetStretchMode();
#endif // WINUTIL_STUB
}


// Return the current target rectangle

void CDrawImage::GetTargetRect(RECT *pTargetRect)
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    ASSERT(pTargetRect);
    *pTargetRect = m_TargetRect;
#endif // WINUTIL_STUB
}


// This is called when we want to change the section of the image to draw. We
// use this information in the drawing operation calls later on. We must also
// see if the source and destination rectangles have the same dimensions. If
// not we must stretch during the drawing rather than a direct pixel copy

void CDrawImage::SetSourceRect(RECT *pSourceRect)
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    ASSERT(pSourceRect);
    m_SourceRect = *pSourceRect;
    SetStretchMode();
#endif // WINUTIL_STUB
}


// Return the current source rectangle

void CDrawImage::GetSourceRect(RECT *pSourceRect)
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    ASSERT(pSourceRect);
    *pSourceRect = m_SourceRect;
#endif // WINUTIL_STUB
}


// This is called when either the source or destination rectanges change so we
// can update the stretch flag. If the rectangles don't match we stretch the
// video during the drawing otherwise we call the fast pixel copy functions
// NOTE the source and/or the destination rectangle may be completely empty

void CDrawImage::SetStretchMode()
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    // Calculate the overall rectangle dimensions

    LONG SourceWidth = m_SourceRect.right - m_SourceRect.left;
    LONG SinkWidth = m_TargetRect.right - m_TargetRect.left;
    LONG SourceHeight = m_SourceRect.bottom - m_SourceRect.top;
    LONG SinkHeight = m_TargetRect.bottom - m_TargetRect.top;

    m_bStretch = TRUE;
    if (SourceWidth == SinkWidth) {
        if (SourceHeight == SinkHeight) {
            m_bStretch = FALSE;
        }
    }
#endif // WINUTIL_STUB
}


// Tell us whose allocator we are using. This should be called with TRUE if
// the filter agrees to use an allocator based around the CImageAllocator
// SDK base class - whose image buffers are made through CreateDIBSection.
// Otherwise this should be called with FALSE and we will draw the images
// using SetDIBitsToDevice and StretchDIBitsToDevice. None of these calls
// can handle buffers which have non zero strides (like DirectDraw uses)

void CDrawImage::NotifyAllocator(BOOL bUsingImageAllocator)
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    m_bUsingImageAllocator = bUsingImageAllocator;
#endif // WINUTIL_STUB
}


// Are we using the image DIBSECTION allocator

BOOL CDrawImage::UsingImageAllocator()
{
#ifdef WINUTIL_STUB
    STUBRET(FALSE);
#else
    return m_bUsingImageAllocator;
#endif // WINUTIL_STUB
}


// We need the media type of the connection so that we can get the BITMAPINFO
// from it. We use that in the calls to draw the image such as StretchDIBits
// and also when updating the colour table held in shared memory DIBSECTIONs

void CDrawImage::NotifyMediaType(CMediaType *pMediaType)
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    m_pMediaType = pMediaType;
#endif // WINUTIL_STUB
}


// We store in this object a cookie maintaining the current palette version.
// Each time a palettised format is changed we increment this value so that
// when we come to draw the images we look at the colour table value they
// have and if less than the current we know to update it. This version is
// only needed and indeed used when working with shared memory DIBSECTIONs

LONG CDrawImage::GetPaletteVersion()
{
#ifdef WINUTIL_STUB
    STUBRET(0);
#else
    return m_PaletteVersion;
#endif // WINUTIL_STUB
}


// Resets the current palette version number

void CDrawImage::ResetPaletteVersion()
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    m_PaletteVersion = PALETTE_VERSION;
#endif // WINUTIL_STUB
}


// Increment the current palette version

void CDrawImage::IncrementPaletteVersion()
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    m_PaletteVersion++;
#endif // WINUTIL_STUB
}


// Constructor must initialise the base allocator. Each sample we create has a
// palette version cookie on board. When the source filter changes the palette
// during streaming the window object increments an internal cookie counter it
// keeps as well. When it comes to render the samples it looks at the cookie
// values and if they don't match then it knows to update the sample's colour
// table. However we always create samples with a cookie of PALETTE_VERSION
// If there have been multiple format changes and we disconnect and reconnect
// thereby causing the samples to be reallocated we will create them with a
// cookie much lower than the current version, this isn't a problem since it
// will be seen by the window object and the versions will then be updated

CImageAllocator::CImageAllocator(CBaseFilter *pFilter,
                                 TCHAR *pName,
                                 HRESULT *phr) :
    CBaseAllocator(pName,NULL,phr)                             
#ifndef WINUTIL_STUB
    , m_pFilter(pFilter)
#endif // WINUTIL_STUB
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    ASSERT(phr);
    ASSERT(pFilter);
#endif // WINUTIL_STUB
}


// Check our DIB buffers have been released

#ifdef DEBUG
CImageAllocator::~CImageAllocator()
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    ASSERT(m_bCommitted == FALSE);
#endif // WINUTIL_STUB
}
#endif


// Called from destructor and also from base class to free resources. We work
// our way through the list of media samples deleting the DIBSECTION created
// for each. All samples should be back in our list so there is no chance a
// filter is still using one to write on the display or hold on a pending list

void CImageAllocator::Free()
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    ASSERT(m_lAllocated == m_lFree.GetCount());
#ifndef UNDER_CE
    EXECUTE_ASSERT(GdiFlush());
#endif // UNDER_CE
    CImageSample *pSample;
    DIBDATA *pDibData;

    while (m_lFree.GetCount() != 0) {
        pSample = (CImageSample *) m_lFree.RemoveHead();
        pDibData = pSample->GetDIBData();
        EXECUTE_ASSERT(DeleteObject(pDibData->hBitmap));
#ifndef UNDER_CE
        EXECUTE_ASSERT(CloseHandle(pDibData->hMapping));
#endif
        delete pSample;
    }

    m_lAllocated = 0;
#endif // WINUTIL_STUB
}


// Prepare the allocator by checking all the input parameters

STDMETHODIMP CImageAllocator::CheckSizes(ALLOCATOR_PROPERTIES *pRequest)
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    // Check we have a valid connection

    if (m_pMediaType == NULL) {
        return VFW_E_NOT_CONNECTED;
    }

    // NOTE We always create a DIB section with the source format type which
    // may contain a source palette. When we do the BitBlt drawing operation
    // the target display device may contain a different palette (we may not
    // have the focus) in which case GDI will do after the palette mapping

	BITMAPINFOHEADER *pBmiHeader = GetBitmapInfoHeader (m_pMediaType);

    // When we call CreateDIBSection it implicitly maps only enough memory
    // for the image as defined by thee BITMAPINFOHEADER. If the user asks
    // for an image smaller than this then we reject the call, if they ask
    // for an image larger than this then we return what they can have

    if ((DWORD) pRequest->cbBuffer < pBmiHeader->biSizeImage) {
        return E_INVALIDARG;
    }

    // Reject buffer prefixes

    if (pRequest->cbPrefix > 0) {
        return E_INVALIDARG;
    }

    pRequest->cbBuffer = pBmiHeader->biSizeImage;
    return NOERROR;
#endif // WINUTIL_STUB
}


// Agree the number of media sample buffers and their sizes. The base class
// this allocator is derived from allows samples to be aligned only on byte
// boundaries NOTE the buffers are not allocated until the Commit call

STDMETHODIMP CImageAllocator::SetProperties(
    ALLOCATOR_PROPERTIES * pRequest,
    ALLOCATOR_PROPERTIES * pActual)
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    ALLOCATOR_PROPERTIES Adjusted = *pRequest;

    // Check the parameters fit with the current connection

    HRESULT hr = CheckSizes(&Adjusted);
    if (FAILED(hr)) {
        return hr;
    }
    return CBaseAllocator::SetProperties(&Adjusted, pActual);
#endif // WINUTIL_STUB
}


// Commit the memory by allocating the agreed number of media samples. For
// each sample we are committed to creating we have a CImageSample object
// that we use to manage it's resources. This is initialised with a DIBDATA
// structure that contains amongst other things the GDI DIBSECTION handle
// We will access the renderer media type during this so we must have locked
// (to prevent the format changing for example). The class overrides Commit
// and Decommit to do this locking (base class Commit in turn calls Alloc)

HRESULT CImageAllocator::Alloc(void)
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    ASSERT(m_pMediaType);
    CImageSample *pSample;
    DIBDATA DibData;

    // Check the base allocator says it's ok to continue

    HRESULT hr = CBaseAllocator::Alloc();
    if (FAILED(hr)) {
        return hr;
    }

    // We create a new memory mapped object although we don't map it into our
    // address space because GDI does that in CreateDIBSection. It is possible
    // that we run out of resources before creating all the samples in which
    // case the available sample list is left with those already created

    ASSERT(m_lAllocated == 0);
    while (m_lAllocated < m_lCount) {

        // Create and initialise a shared memory GDI buffer

        HRESULT hr = CreateDIB(m_lSize,DibData);
        if (FAILED(hr)) {
            return hr;
        }

        // Create the sample object and pass it the DIBDATA

        pSample = CreateImageSample(DibData.pBase,m_lSize);
        if (pSample == NULL) {
            EXECUTE_ASSERT(DeleteObject(DibData.hBitmap));
#ifndef UNDER_CE
            EXECUTE_ASSERT(CloseHandle(DibData.hMapping));
#endif
            return E_OUTOFMEMORY;
        }

        // Add the completed sample to the available list

        pSample->SetDIBData(&DibData);
        m_lFree.Add(pSample);
        m_lAllocated++;
    }
    return NOERROR;
#endif // WINUTIL_STUB
}


// We have a virtual method that allocates the samples so that a derived class
// may override it and allocate more specialised sample objects. So long as it
// derives its samples from CImageSample then all this code will still work ok

CImageSample *CImageAllocator::CreateImageSample(LPBYTE pData,LONG Length)
{
#ifdef WINUTIL_STUB
    STUBRET(NULL);
#else
    HRESULT hr = NOERROR;
    CImageSample *pSample;

    // Allocate the new sample and check the return codes

    pSample = new CImageSample((CBaseAllocator *) this,   // Base class
                               NAME("Video sample"),      // DEBUG name
                               (HRESULT *) &hr,           // Return code
                               (LPBYTE) pData,            // DIB address
                               (LONG) Length);            // Size of DIB

    if (pSample == NULL || FAILED(hr)) {
        delete pSample;
        return NULL;
    }
    return pSample;
#endif // WINUTIL_STUB
}


// This function allocates a shared memory block for use by the source filter
// generating DIBs for us to render. The memory block is created in shared
// memory so that GDI doesn't have to copy the memory when we do a BitBlt

HRESULT CImageAllocator::CreateDIB(LONG InSize,DIBDATA &DibData)
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    BITMAPINFO *pbmi;       // Format information for pin
    BYTE *pBase;            // Pointer to the actual image
    HANDLE hMapping;        // Handle to mapped object
    HBITMAP hBitmap;        // DIB section bitmap handle

    // Create a file mapping object and map into our address space
#ifndef UNDER_CE
    hMapping = CreateFileMapping(hMEMORY,         // Use system page file
                                 NULL,            // No security attributes
                                 PAGE_READWRITE,  // Full access to memory
                                 (DWORD) 0,       // Less than 4Gb in size
                                 InSize,          // Size of buffer
                                 NULL);           // No name to section
    if (hMapping == NULL) {
        DWORD Error = GetLastError();
        return HRESULT_FROM_WIN32(Error);
    }
#else
    hMapping = NULL;  // mapping object not supported in CreateDIBSection on CE
#endif

    // NOTE We always create a DIB section with the source format type which
    // may contain a source palette. When we do the BitBlt drawing operation
    // the target display device may contain a different palette (we may not
    // have the focus) in which case GDI will do after the palette mapping

    pbmi = (BITMAPINFO *) GetBitmapInfoHeader(m_pMediaType);
    if (m_pMediaType == NULL) {
        DbgBreak("Invalid media type");
    }

#ifdef UNDER_CE
    BITMAPINFO  *pBMI = NULL;
    // BUGBUG: we have to do a temp fix here as CE currently doesn't have
    // a way to get the BI_BITFIELD bitmasks for the screen from the system
    if (pbmi->bmiHeader.biBitCount == 16 || pbmi->bmiHeader.biBitCount == 32)
    {
        pBMI = (BITMAPINFO *) new BYTE[(sizeof(BITMAPINFOHEADER) + (3 * sizeof(DWORD)))];
        
        if (pBMI)
        {
            // copy the structure
            *pBMI = *pbmi;
            // fixup the struct
            pBMI->bmiHeader.biCompression = BI_BITFIELDS;
            
            // BUGBUG: angusg
            // This is a temp fix to alow video to work on both 555 and 565 devices
            // the pbmi structure above should be setup correctly with the bitfields
            // but on CE it doesn't seem to be, the real fix would be to make sure
            // that the format is setup correctly in the first place.
            // Thing may be confused by the fact that on the desktop (Win95/98/NT)
            // a bitmapinfo header with biBitCount == 16 and biCompression == BI_RGB
            // means a 555 bitmap on CE this is invalid.
            HDC hdcDisplay = GetDC(NULL);
            ASSERT(hdcDisplay);            
            
            HBITMAP hbm = CreateCompatibleBitmap(hdcDisplay,1,1);            
            if (hbm)
            {
                DIBSECTION  dib;
                GetObject(hbm, sizeof(DIBSECTION), &dib);
                // get the bitfields if necessary
                if (pBMI->bmiHeader.biCompression == BI_BITFIELDS)				
                {
                    DWORD   *pdw = (DWORD *) (&(pBMI->bmiColors));
                    
					// just copy the bit fields if the compatible bmp has the same bit depth... 
					// Otherwise, just use the ones that came with the mediaformat.	
					if (pBMI->bmiHeader.biBitCount == dib.dsBmih.biBitCount) { 
						*pdw++ = (DWORD) dib.dsBitfields[0];
						*pdw++ = (DWORD) dib.dsBitfields[1];
						*pdw++ = (DWORD) dib.dsBitfields[2];
					} else {
					    DWORD *pOrigBitFields = (DWORD *)(&(pbmi->bmiColors));
						*pdw++ = (DWORD) pOrigBitFields[0];
						*pdw++ = (DWORD) pOrigBitFields[1];
						*pdw++ = (DWORD) pOrigBitFields[2];
					}
                    // we have to do this on CE as it incorrectly returns 3
                    pBMI->bmiHeader.biClrUsed = 0;
                }
                DeleteObject(hbm);                    
            } 
            else
            {
                ASSERT(TRUE);
                
                // assume 565
                pBMI->bmiColors[2].rgbBlue     = 0x1F;
                pBMI->bmiColors[2].rgbGreen    = 0x00;
                pBMI->bmiColors[2].rgbRed      = 0x00;
                pBMI->bmiColors[2].rgbReserved = 0x00;
                pBMI->bmiColors[1].rgbBlue     = 0xE0;
                pBMI->bmiColors[1].rgbGreen    = 0x07;
                pBMI->bmiColors[1].rgbRed      = 0x00;
                pBMI->bmiColors[1].rgbReserved = 0x00;
                pBMI->bmiColors[0].rgbBlue     = 0x00;
                pBMI->bmiColors[0].rgbGreen    = 0xF8;
                pBMI->bmiColors[0].rgbRed      = 0x00;
                pBMI->bmiColors[0].rgbReserved = 0x00;
            }
                        
            ReleaseDC(NULL, hdcDisplay);
        }            
        pbmi = pBMI;
    }
#endif
    
    hBitmap = CreateDIBSection((HDC) NULL,          // NO device context
                               pbmi,                // Format information
                               DIB_RGB_COLORS,      // Use the palette
                               (VOID **) &pBase,    // Pointer to image data
                               hMapping,            // Mapped memory handle
                               (DWORD) 0);          // Offset into memory

#ifdef UNDER_CE
    if (pBMI)
    {
        delete [] pBMI;
    }
#endif

    if (hBitmap == NULL || pBase == NULL) {
#ifndef UNDER_CE
        EXECUTE_ASSERT(CloseHandle(hMapping));
#endif
        DWORD Error = GetLastError();
        return HRESULT_FROM_WIN32(Error);
    }

    // Initialise the DIB information structure

    DibData.hBitmap = hBitmap;
    DibData.hMapping = hMapping;
    DibData.pBase = pBase;
    DibData.PaletteVersion = PALETTE_VERSION;
    GetObject(hBitmap,sizeof(DIBSECTION),(VOID *)&DibData.DibSection);

    return NOERROR;
#endif // WINUTIL_STUB
}


// We use the media type during the DIBSECTION creation

void CImageAllocator::NotifyMediaType(CMediaType *pMediaType)
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    m_pMediaType = pMediaType;
#endif // WINUTIL_STUB
}


// Overriden to increment the owning object's reference count

STDMETHODIMP_(ULONG) CImageAllocator::NonDelegatingAddRef()
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    return m_pFilter->AddRef();
#endif // WINUTIL_STUB
}


// Overriden to decrement the owning object's reference count

STDMETHODIMP_(ULONG) CImageAllocator::NonDelegatingRelease()
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    return m_pFilter->Release();
#endif // WINUTIL_STUB
}


// If you derive a class from CMediaSample that has to transport specialised
// member variables and entry points then there are three alternate solutions
// The first is to create a memory buffer larger than actually required by the
// sample and store your information either at the beginning of it or at the
// end, the former being moderately safer allowing for misbehaving transform
// filters. You then adjust the buffer address when you create the base media
// sample. This has the disadvantage of breaking up the memory allocated to
// the samples into separate blocks. The second solution is to implement a
// class derived from CMediaSample and support additional interface(s) that
// convey your private data. This means defining a custom interface. The final
// alternative is to create a class that inherits from CMediaSample and adds
// the private data structures, when you get an IMediaSample in your Receive()
// call check to see if your allocator is being used, and if it is then cast
// the IMediaSample into one of your objects. Additional checks can be made
// to ensure the sample's this pointer is known to be one of your own objects

CImageSample::CImageSample(CBaseAllocator *pAllocator,
                           TCHAR *pName,
                           HRESULT *phr,
                           LPBYTE pBuffer,
                           LONG length) :
    CMediaSample(pName,pAllocator,phr,pBuffer,length)
#ifndef WINUTIL_STUB
    , m_bInit(FALSE)
#endif // WINUTIL_STUB
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    ASSERT(pAllocator);
    ASSERT(pBuffer);
    // angusg: zero the buffer so we never show a
    // garbage video frame when starting playback
    memset(pBuffer, 0, length);
#endif // WINUTIL_STUB
}


// Set the shared memory DIB information

void CImageSample::SetDIBData(DIBDATA *pDibData)
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    ASSERT(pDibData);
    m_DibData = *pDibData;
    m_bInit = TRUE;
#endif // WINUTIL_STUB
}


// Retrieve the shared memory DIB data

DIBDATA *CImageSample::GetDIBData()
{
#ifdef WINUTIL_STUB
    STUBRET(NULL);
#else
    ASSERT(m_bInit == TRUE);
    return &m_DibData;
#endif // WINUTIL_STUB
}


// This class handles the creation of a palette. It is fairly specialist and
// is intended to simplify palette management for video renderer filters. It
// is for this reason that the constructor requires three other objects with
// which it interacts, namely a base media filter, a base window and a base
// drawing object although the base window or the draw object may be NULL to
// ignore that part of us. We try not to create and install palettes unless
// absolutely necessary as they typically require WM_PALETTECHANGED messages
// to be sent to every window thread in the system which is very expensive

CImagePalette::CImagePalette(CBaseFilter *pBaseFilter,
                             CBaseWindow *pBaseWindow,
                             CDrawImage *pDrawImage)
#ifndef WINUTIL_STUB
    :
    m_pBaseWindow(pBaseWindow),
    m_pFilter(pBaseFilter),
    m_pDrawImage(pDrawImage),
    m_hPalette(NULL)
#endif // WINUTIL_STUB
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    ASSERT(m_pFilter);
#endif // WINUTIL_STUB
}


// Destructor

#ifdef DEBUG
CImagePalette::~CImagePalette()
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    ASSERT(m_hPalette == NULL);
#endif // WINUTIL_STUB
}
#endif


// We allow dynamic format changes of the palette but rather than change the
// palette every time we call this to work out whether an update is required.
// If the original type didn't use a palette and the new one does (or vica
// versa) then we return TRUE. If neither formats use a palette we'll return
// FALSE. If both formats use a palette we compare their colours and return
// FALSE if they match. This therefore short circuits palette creation unless
// absolutely necessary since installing palettes is an expensive operation

BOOL CImagePalette::ShouldUpdate(const BITMAPINFOHEADER *pNewBmiHeader,
                                 const BITMAPINFOHEADER *pOldBmiHeader,
								 const RGBQUAD *pNewPalette,
								 const RGBQUAD *pOldPalette)
{
#ifdef WINUTIL_STUB
    STUBRET(FALSE);
#else

	// we may not have a current format yet...
	if (!pOldBmiHeader  ||  !pOldPalette)
		return TRUE;

    // Do both formats not require a palette
    if (ContainsPalette(pNewBmiHeader) == FALSE) {
        if (ContainsPalette(pOldBmiHeader) == FALSE) {
            return FALSE;
        }
    }

    // Compare the colours to see if they match

    DWORD VideoEntries = pNewBmiHeader->biClrUsed;
    if (ContainsPalette(pNewBmiHeader) == TRUE)
        if (ContainsPalette(pOldBmiHeader) == TRUE)
            if (pOldBmiHeader->biClrUsed == VideoEntries)
                if (pOldBmiHeader->biClrUsed > 0)
                    if (memcmp((PVOID) pNewPalette,
                               (PVOID) pOldPalette,
                               VideoEntries * sizeof(RGBQUAD)) == 0) {

                        return FALSE;
                    }
    return TRUE;
#endif // WINUTIL_STUB
}


BOOL CImagePalette::ShouldUpdate(const VIDEOINFOHEADER *pNewInfo,
                                 const VIDEOINFOHEADER *pOldInfo)
{
#ifdef WINUTIL_STUB
    STUBRET(FALSE);
#else
    // We may not have a current format yet

    if (pOldInfo == NULL) {
        return TRUE;
    }
	return ShouldUpdate (&pNewInfo->bmiHeader, 
		                 &pOldInfo->bmiHeader, 
		                 GetBitmapPalette ((VIDEOINFO *)pNewInfo),
						 GetBitmapPalette ((VIDEOINFO *)pOldInfo));
#endif // WINUTIL_STUB
}


BOOL CImagePalette::ShouldUpdate(const VIDEOINFOHEADER2 *pNewInfo2,
                                 const VIDEOINFOHEADER2 *pOldInfo2)
{
#ifdef WINUTIL_STUB
    STUBRET(FALSE);
#else
    // We may not have a current format yet

    if (pOldInfo2 == NULL) {
        return TRUE;
    }
	return ShouldUpdate (&pNewInfo2->bmiHeader, 
		                 &pOldInfo2->bmiHeader,
						 GetBitmapPalette ((VIDEOINFO2 *)pNewInfo2),
						 GetBitmapPalette ((VIDEOINFO2 *)pOldInfo2));
#endif // WINUTIL_STUB
}


// This is normally called when the input pin type is set to install a palette
// We will typically be called from two different places. The first is when we
// have negotiated a palettised media type after connection, the other is when
// we receive a new type during processing with an updated palette in which
// case we must remove and release the resources held by the current palette

// We can be passed an optional device name if we wish to prepare a palette
// for a specific monitor on a multi monitor system

HRESULT CImagePalette::PreparePalette(const CMediaType *pmtNew,
                                      const CMediaType *pmtOld,
				      LPTSTR szDevice)
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    ASSERT(pmtNew->Format());
	const BITMAPINFOHEADER *pNewBmiHeader = GetBitmapInfoHeader (pmtNew);
	const BITMAPINFOHEADER *pOldBmiHeader = GetBitmapInfoHeader (pmtOld);

	if (!pmtNew  ||  !pmtOld)
		return S_FALSE;

    // This is an performance optimisation, when we get a media type we check
    // to see if the format requires a palette change. If either we need one
    // when previously we didn't or vica versa then this returns TRUE, if we
    // previously needed a palette and we do now it compares their colours

	RGBQUAD *pNewPalette = NULL, *pOldPalette = NULL;
	if (*pmtNew->FormatType() == FORMAT_VideoInfo)
	{
	    VIDEOINFO *pVideoInfo = (VIDEOINFO *) pmtNew->Format();
		if (pVideoInfo)
		{
			pNewPalette = GetBitmapPalette (pVideoInfo);
		}
	}
	else if (*pmtNew->FormatType() == FORMAT_VideoInfo2)
	{
	    VIDEOINFO2 *pVideoInfo2 = (VIDEOINFO2 *) pmtNew->Format();
		if (pVideoInfo2)
		{
			pNewPalette = GetBitmapPalette (pVideoInfo2);
		}
	}
	else
		return E_FAIL;

	if (*pmtOld->FormatType() == FORMAT_VideoInfo)
	{
	    VIDEOINFO *pVideoInfo = (VIDEOINFO *) pmtOld->Format();
		if (pVideoInfo)
		{
			pOldPalette = GetBitmapPalette (pVideoInfo);
		}
	}
	else if (*pmtOld->FormatType() == FORMAT_VideoInfo2)
	{
	    VIDEOINFO2 *pVideoInfo2 = (VIDEOINFO2 *) pmtOld->Format();
		if (pVideoInfo2)
		{
			pOldPalette = GetBitmapPalette (pVideoInfo2);
		}
	}
	else
		return E_FAIL;

    if (ShouldUpdate(pNewBmiHeader,pOldBmiHeader,pNewPalette,pOldPalette) == FALSE) {
        NOTE("No update needed");
        return S_FALSE;
    }

    // We must notify the filter graph that the application may have changed
    // the palette although in practice we don't bother checking to see if it
    // is really different. If it tries to get the palette either the window
    // or renderer lock will ensure it doesn't get in until we are finished

    RemovePalette();
    m_pFilter->NotifyEvent(EC_PALETTE_CHANGED,0,0);

    // Do we need a palette for the new format

    if (ContainsPalette(pNewBmiHeader) == FALSE) {
        NOTE("New has no palette");
        return S_FALSE;
    }

    // If we're changing the palette on the fly then we increment our palette
    // cookie which is compared against the cookie also stored in all of our
    // DIBSECTION media samples. If they don't match when we come to draw it
    // then we know the sample is out of date and we'll update it's palette

    NOTE("Making new colour palette");
    m_hPalette = MakePalette(pNewBmiHeader, pNewPalette, szDevice);
    ASSERT(m_hPalette != NULL);

    // The window in which the new palette is to be realised may be a NULL
    // pointer to signal that no window is in use, if so we don't call it
    // Some filters just want to use this object to create/manage palettes

    if (m_pBaseWindow) m_pBaseWindow->SetPalette(m_hPalette);

    // This is the only time where we need access to the draw object to say
    // to it that a new palette will be arriving on a sample real soon. The
    // constructor may take a NULL pointer in which case we don't call this

    if (m_pDrawImage) m_pDrawImage->IncrementPaletteVersion();
    return NOERROR;
#endif // WINUTIL_STUB
}


// Helper function to copy a palette out of any kind of VIDEOINFO (ie it may
// be YUV or true colour) into a palettised VIDEOINFO. We use this changing
// palettes on DirectDraw samples as a source filter can attach a palette to
// any buffer (eg YUV) and hand it back. We make a new palette out of that
// format and then copy the palette colours into the current connection type

HRESULT CImagePalette::CopyPalette(const CMediaType *pSrc,CMediaType *pDest)
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    // Reset the destination palette before starting

	BITMAPINFOHEADER *pDestBmiHeader = GetBitmapInfoHeader (pDest);
    pDestBmiHeader->biClrUsed = 0;
    pDestBmiHeader->biClrImportant = 0;

    // Does the destination have a palette

    if (!pDestBmiHeader  ||  BMI_PALETTISED(pDestBmiHeader) == FALSE) {
        NOTE("No destination palette");
        return S_FALSE;
    }

    // Does the source contain a palette

	const BITMAPINFOHEADER *pSrcBmiHeader = GetBitmapInfoHeader (pSrc);
    if (!pSrcBmiHeader  ||  ContainsPalette(pSrcBmiHeader) == FALSE) {
        NOTE("No source palette");
        return S_FALSE;
    }

    // The number of colours may be zero filled

    DWORD PaletteEntries = pSrcBmiHeader->biClrUsed;
    if (PaletteEntries == 0) {
        DWORD Maximum  = (1 << pSrcBmiHeader->biBitCount);
        NOTE1("Setting maximum colours (%d)",Maximum);
        PaletteEntries = Maximum;
    }

    // Make sure the destination has enough room for the palette

    ASSERT(pSrcBmiHeader->biClrUsed <= iPALETTE_COLORS);
    ASSERT(pSrcBmiHeader->biClrImportant <= PaletteEntries);

    pDestBmiHeader->biClrUsed = PaletteEntries;
    pDestBmiHeader->biClrImportant = pSrcBmiHeader->biClrImportant;
    ULONG BitmapSize = GetBitmapFormatSize(pSrcBmiHeader);

    if (pDest->FormatLength() < BitmapSize) {
        NOTE("Reallocating destination");
        pDest->ReallocFormatBuffer(BitmapSize);
    }

    // Now copy the palette colours across

	RGBQUAD *pDestColors;
	RGBQUAD *pSrcPalette;
	if (*pDest->FormatType() == FORMAT_VideoInfo)
	{
		pDestColors = COLORS((VIDEOINFO *)pDest->Format());

	}
	else if (*pDest->FormatType() == FORMAT_VideoInfo2)
	{
		pDestColors = COLORS((VIDEOINFO2 *)pDest->Format());
	}
	else
	{
		return E_FAIL;
	}

	if (*pSrc->FormatType() == FORMAT_VideoInfo)
	{
		pSrcPalette = GetBitmapPalette ((VIDEOINFO *)pSrc->Format());

	}
	else if (*pSrc->FormatType() == FORMAT_VideoInfo2)
	{
		pSrcPalette = GetBitmapPalette ((VIDEOINFO2 *)pSrc->Format());
	}
	else
	{
		return E_FAIL;
	}

    CopyMemory((PVOID) pDestColors,
               (PVOID) pSrcPalette,
               PaletteEntries * sizeof(RGBQUAD));

    return NOERROR;
#endif // WINUTIL_STUB
}


// This is normally called when the palette is changed (typically during a
// dynamic format change) to remove any palette we previously installed. We
// replace it (if necessary) in the video window with a standard VGA palette
// that should always be available even if this is a true colour display

HRESULT CImagePalette::RemovePalette()
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    // Do we have a palette to remove

    if (m_hPalette == NULL) {
        return NOERROR;
    }

    // Get a standard VGA colour palette

    HPALETTE hPalette = (HPALETTE) GetStockObject(DEFAULT_PALETTE);
    ASSERT(hPalette);
    const HPALETTE hPalOurs = m_hPalette;

    // Install the standard palette and delete ours. As in the previous method
    // we may not have been given a window in the constructor to use and if we
    // didn't then don't try to install the stock palette in it. This is used
    // by filters that have to create palettes but who do not draw using GDI

    if (m_pBaseWindow) {
        SelectPalette(m_pBaseWindow->GetWindowHDC(), hPalette, TRUE);
        SelectPalette(m_pBaseWindow->GetMemoryHDC(), hPalette, TRUE);
    }

    EXECUTE_ASSERT(DeleteObject(hPalOurs));
    m_hPalette = NULL;
    return NOERROR;
#endif // WINUTIL_STUB
}


// Called to create a palette for the object, the data structure used by GDI
// to describe a palette is a LOGPALETTE, this includes a variable number of
// PALETTEENTRY fields which are the colours, we have to convert the RGBQUAD
// colour fields we are handed in a BITMAPINFO from the media type into these
// This handles extraction of palettes from true colour and YUV media formats

// We can be passed an optional device name if we wish to prepare a palette
// for a specific monitor on a multi monitor system
HPALETTE CImagePalette::MakePalette(const BITMAPINFOHEADER *pHeader, const RGBQUAD *pColours, LPTSTR szDevice)
{
#ifdef WINUTIL_STUB
    STUBRET((HPALETTE)0);
#else
    ASSERT(ContainsPalette(pHeader) == TRUE);
    ASSERT(pHeader->biClrUsed >= 0);
    ASSERT(pHeader->biClrUsed <= iPALETTE_COLORS);

    LOGPALETTE *lp;                     // Used to create a palette
    HPALETTE hPalette;                  // Logical palette object

    lp = (LOGPALETTE *) new BYTE[sizeof(LOGPALETTE) + SIZE_PALETTE];
    if (lp == NULL) {
        return NULL;
    }

    // Unfortunately a GDI palette entry (a PALETTEENTRY structure) 
    // is different to a palette entry from a DIB
    // format (a RGBQUAD structure) so we have to do the field conversion
    // The VIDEOINFO containing the palette may be a true colour type so
    // we use GetBitmapPalette to skip over any bit fields if they exist

    lp->palVersion = PALVERSION;
    lp->palNumEntries = (USHORT) pHeader->biClrUsed;
    if (lp->palNumEntries == 0) lp->palNumEntries = (1 << pHeader->biBitCount);

    if (lp->palNumEntries > iPALETTE_COLORS) lp->palNumEntries = iPALETTE_COLORS;

    for (DWORD dwCount = 0;dwCount < lp->palNumEntries;dwCount++) {
        lp->palPalEntry[dwCount].peRed = pColours[dwCount].rgbRed;
        lp->palPalEntry[dwCount].peGreen = pColours[dwCount].rgbGreen;
        lp->palPalEntry[dwCount].peBlue = pColours[dwCount].rgbBlue;
        lp->palPalEntry[dwCount].peFlags = 0;
    }

    MakeIdentityPalette(lp->palPalEntry, lp->palNumEntries, szDevice);

    // Create a logical palette

    hPalette = CreatePalette(lp);
    ASSERT(hPalette != NULL);
    delete[] lp;
    return hPalette;
#endif // WINUTIL_STUB
}

HPALETTE CImagePalette::MakePalette(const VIDEOINFOHEADER *pVideoInfo, LPTSTR szDevice)
{
#ifdef WINUTIL_STUB
    STUBRET((HPALETTE)0);
#else
	return MakePalette (&pVideoInfo->bmiHeader, GetBitmapPalette ((VIDEOINFO *)pVideoInfo), szDevice);
#endif // WINUTIL_STUB
}

HPALETTE CImagePalette::MakePalette(const VIDEOINFOHEADER2 *pVideoInfo2, LPTSTR szDevice)
{
#ifdef WINUTIL_STUB
    STUBRET((HPALETTE)0);
#else
	return MakePalette (&pVideoInfo2->bmiHeader, GetBitmapPalette ((VIDEOINFO2 *)pVideoInfo2), szDevice);
#endif // WINUTIL_STUB
}


// GDI does a fair job of compressing the palette entries you give it, so for
// example if you have five entries with an RGB colour (0,0,0) it will remove
// all but one of them. When you subsequently draw an image it will map from
// your logical palette to the compressed device palette. This function looks
// to see if it is trying to be an identity palette and if so sets the flags
// field in the PALETTEENTRYs so they remain expanded to boost performance

// We can be passed an optional device name if we wish to prepare a palette
// for a specific monitor on a multi monitor system

HRESULT CImagePalette::MakeIdentityPalette(PALETTEENTRY *pEntry,INT iColours, LPTSTR szDevice)
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    PALETTEENTRY SystemEntries[10];         // System palette entries
    BOOL bIdentityPalette = TRUE;           // Is an identity palette
    ASSERT(iColours <= iPALETTE_COLORS);    // Should have a palette
    const int PalLoCount = 10;              // First ten reserved colours
    const int PalHiStart = 246;             // Last VGA palette entries

    // Does this have the full colour range

    if (iColours < 10) {
        return S_FALSE;
    }

    // Apparently some displays have odd numbers of system colours

    // Get a DC on the right monitor - it's ugly, but this is the way you have
    // to do it
    HDC hdc;
    if (szDevice == NULL || lstrcmpi(szDevice, TEXT("DISPLAY")) == 0)
        hdc = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
    else
        hdc = CreateDC(NULL, szDevice, NULL, NULL);
    ASSERT(hdc);
    INT Reserved = GetDeviceCaps(hdc,NUMRESERVED);
    if (Reserved != 20) {
        DeleteDC(hdc);
        return S_FALSE;
    }

    // Compare our palette against the first ten system entries. The reason I
    // don't do a memory compare between our two arrays of colours is because
    // I am not sure what will be in the flags fields for the system entries

    UINT Result = GetSystemPaletteEntries(hdc,0,PalLoCount,SystemEntries);
	UINT Count = 0;
    for (Count = 0;Count < Result;Count++) {
        if (SystemEntries[Count].peRed != pEntry[Count].peRed ||
                SystemEntries[Count].peGreen != pEntry[Count].peGreen ||
                    SystemEntries[Count].peBlue != pEntry[Count].peBlue) {
                        bIdentityPalette = FALSE;
        }
    }

    // And likewise compare against the last ten entries

    Result = GetSystemPaletteEntries(hdc,PalHiStart,PalLoCount,SystemEntries);
    for (Count = 0;Count < Result;Count++) {
        if (INT(Count) + PalHiStart < iColours) {
            if (SystemEntries[Count].peRed != pEntry[PalHiStart + Count].peRed ||
                    SystemEntries[Count].peGreen != pEntry[PalHiStart + Count].peGreen ||
                        SystemEntries[Count].peBlue != pEntry[PalHiStart + Count].peBlue) {
                            bIdentityPalette = FALSE;
            }
        }
    }

    // If not an identity palette then return S_FALSE

    DeleteDC(hdc);
    if (bIdentityPalette == FALSE) {
        return S_FALSE;
    }

    // Set the non VGA entries so that GDI doesn't map them

    for (Count = PalLoCount;INT(Count) < min(PalHiStart,iColours);Count++) {
#ifndef UNDER_CE
        pEntry[Count].peFlags = PC_NOCOLLAPSE;
#else
        pEntry[Count].peFlags = NULL;
#endif // UNDER_CE
    }
    return NOERROR;
#endif // WINUTIL_STUB
}


RGBQUAD *CImagePalette::GetBitmapPalette (const VIDEOINFO *pVideoInfo)
{
    if (pVideoInfo->bmiHeader.biCompression == BI_BITFIELDS) {
        return TRUECOLOR(pVideoInfo)->bmiColors;
    }
    return COLORS(pVideoInfo);
}


RGBQUAD *CImagePalette::GetBitmapPalette (const VIDEOINFO2 *pVideoInfo2)
{
    if (pVideoInfo2->bmiHeader.biCompression == BI_BITFIELDS) {
        return TRUECOLOR(pVideoInfo2)->bmiColors;
    }
    return COLORS(pVideoInfo2);
}


// Constructor initialises the VIDEOINFO we keep storing the current display
// format. The format can be changed at any time, to reset the format held
// by us call the RefreshDisplayType directly (it's a public method). Since
// more than one thread will typically call us (ie window threads resetting
// the type and source threads in the type checking methods) we have a lock

CImageDisplay::CImageDisplay()
{
#ifdef WINUTIL_STUB
    STUBNORET;
#else
    RefreshDisplayType(NULL);
#endif // WINUTIL_STUB
}



// This initialises the format we hold which contains the display device type
// We do a conversion on the display device type in here so that when we start
// type checking input formats we can assume that certain fields have been set
// correctly, an example is when we make the 16 bit mask fields explicit. This
// is normally called when we receive WM_DEVMODECHANGED device change messages

// The optional szDeviceName parameter tells us which monitor we are interested
// in for a multi monitor system

HRESULT CImageDisplay::RefreshDisplayType(LPTSTR szDeviceName)
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    CAutoLock cDisplayLock(this);

    // Set the preferred format type

    ZeroMemory((PVOID)&m_Display,sizeof(VIDEOINFOHEADER2)+sizeof(TRUECOLORINFO));
    m_Display.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_Display.bmiHeader.biBitCount = FALSE;

    // Get the bit depth of a device compatible bitmap

    // get caps of whichever monitor they are interested in (multi monitor)
    HDC hdcDisplay;
    // it's ugly, but this is the way you have to do it
    if (szDeviceName == NULL || lstrcmpi(szDeviceName, TEXT("DISPLAY")) == 0)
        hdcDisplay = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
    else
        hdcDisplay = CreateDC(NULL, szDeviceName, NULL, NULL);
    if (hdcDisplay == NULL) {
	ASSERT(FALSE);
	DbgLog((LOG_ERROR,1,TEXT("ACK! Can't get a DC for %s"),
				szDeviceName ? szDeviceName : TEXT("<NULL>")));
	return E_FAIL;
    } else {
	DbgLog((LOG_TRACE,3,TEXT("Created a DC for %s"),
				szDeviceName ? szDeviceName : TEXT("<NULL>")));
    }
    HBITMAP hbm = CreateCompatibleBitmap(hdcDisplay,1,1);

#ifndef UNDER_CE
    // this call gets the bitmapinfo data
    GetDIBits(hdcDisplay,hbm,0,1,NULL,(BITMAPINFO *)&m_Display.bmiHeader,DIB_RGB_COLORS);
    // This call will get the colour table or the proper bitfields
    GetDIBits(hdcDisplay,hbm,0,1,NULL,(BITMAPINFO *)&m_Display.bmiHeader,DIB_RGB_COLORS);
#else
    // angusg: we can't do the above on CE to get the bitfields for the display
    // device. We use GetObject() to return this information, this 'feature' of 
    // GetObject is CE specific and will not work on the desktop.
    DIBSECTION  dib;

    GetObject(hbm, sizeof(DIBSECTION), &dib);

    m_Display.bmiHeader.biSize          = dib.dsBmih.biSize;
    m_Display.bmiHeader.biWidth         = dib.dsBmih.biWidth;
    m_Display.bmiHeader.biHeight        = dib.dsBmih.biHeight;
    m_Display.bmiHeader.biPlanes        = dib.dsBmih.biPlanes;
    m_Display.bmiHeader.biBitCount      = dib.dsBmih.biBitCount;
    m_Display.bmiHeader.biCompression   = dib.dsBmih.biCompression;
    m_Display.bmiHeader.biSizeImage     = dib.dsBmih.biSizeImage;
    m_Display.bmiHeader.biXPelsPerMeter = dib.dsBmih.biXPelsPerMeter;
    m_Display.bmiHeader.biYPelsPerMeter = dib.dsBmih.biYPelsPerMeter;
    m_Display.bmiHeader.biClrUsed       = dib.dsBmih.biClrUsed;
    m_Display.bmiHeader.biClrImportant  = dib.dsBmih.biClrImportant;
    // get the bitfields if necessary
    if (m_Display.bmiHeader.biCompression == BI_BITFIELDS)
    {
        m_Display.dwBitMasks[0] = dib.dsBitfields[0];
        m_Display.dwBitMasks[1] = dib.dsBitfields[1];
        m_Display.dwBitMasks[2] = dib.dsBitfields[2];
        // we have to do this on CE as it incorrectly returns 3
        m_Display.bmiHeader.biClrUsed = 0;
    }
#endif // UNDER_CE
    DeleteObject(hbm);
    DeleteDC(hdcDisplay);

    // Complete the display type initialisation

    ASSERT(CheckHeaderValidity(&m_Display));
    UpdateFormat(&m_Display);
    DbgLog((LOG_TRACE,3,TEXT("New DISPLAY bit depth =%d"),
				m_Display.bmiHeader.biBitCount));
    return NOERROR;
#endif // WINUTIL_STUB
}


// We assume throughout this code that any bitfields masks are allowed no
// more than eight bits to store a colour component. This checks that the
// bit count assumption is enforced and also makes sure that all the bits
// set are contiguous. We return a boolean TRUE if the field checks out ok
BOOL CImageDisplay::CheckBitFields(const DWORD *pBitFields)
{
#ifdef WINUTIL_STUB
    STUBRET(FALSE);
#else
    for (INT iColour = iRED;iColour <= iBLUE;iColour++) {

        // First of all work out how many bits are set

        DWORD SetBits = CountSetBits(pBitFields[iColour]);
        if (SetBits > iMAXBITS || SetBits == 0) {
            NOTE1("Bit fields for component %d invalid",iColour);
            return FALSE;
        }

        // Next work out the number of zero bits prefix
        DWORD PrefixBits = CountPrefixBits(pBitFields[iColour]);

        // This is going to see if all the bits set are contiguous (as they
        // should be). We know how much to shift them right by from the
        // count of prefix bits. The number of bits set defines a mask, we
        // invert this (ones complement) and AND it with the shifted bit
        // fields. If the result is NON zero then there are bit(s) sticking
        // out the left hand end which means they are not contiguous

        DWORD TestField = pBitFields[iColour] >> PrefixBits;
        DWORD Mask = ULONG_MAX << SetBits;
        if (TestField & Mask) {
            NOTE1("Bit fields for component %d not contiguous",iColour);
            return FALSE;
        }
    }
    return TRUE;
#endif // WINUTIL_STUB
}


BOOL CImageDisplay::CheckBitFields(const VIDEOINFO *pInput)
{
#ifdef WINUTIL_STUB
    STUBRET(FALSE);
#else
	return CheckBitFields ((DWORD *)BITMASKS(pInput));
#endif // WINUTIL_STUB
}


BOOL CImageDisplay::CheckBitFields(const VIDEOINFO2 *pInput2)
{
#ifdef WINUTIL_STUB
    STUBRET(FALSE);
#else
	return CheckBitFields ((DWORD *)BITMASKS(pInput2));
#endif // WINUTIL_STUB
}


// This counts the number of bits set in the input field

DWORD CImageDisplay::CountSetBits(DWORD Field)
{
#ifdef WINUTIL_STUB
    STUBRET(0);
#else
    // This is a relatively well known bit counting algorithm

    DWORD Count = 0;
    DWORD init = Field;

    // Until the input is exhausted, count the number of bits

    while (init) {
        init = init & (init - 1);  // Turn off the bottommost bit
        Count++;
    }
    return Count;
#endif // WINUTIL_STUB
}


// This counts the number of zero bits upto the first one set NOTE the input
// field should have been previously checked to ensure there is at least one
// set although if we don't find one set we return the impossible value 32

DWORD CImageDisplay::CountPrefixBits(DWORD Field)
{
#ifdef WINUTIL_STUB
    STUBRET(0);
#else
    DWORD Mask = 1;
    DWORD Count = 0;

    while (TRUE) {
        if (Field & Mask) {
            return Count;
        }
        Count++;

        ASSERT(Mask != 0x80000000);
        if (Mask == 0x80000000) {
            return Count;
        }
        Mask <<= 1;
    }
#endif // WINUTIL_STUB
}


// This is called to check the BITMAPINFOHEADER for the input type. There are
// many implicit dependancies between the fields in a header structure which
// if we validate now make for easier manipulation in subsequent handling. We
// also check that the BITMAPINFOHEADER matches it's specification such that
// fields likes the number of planes is one, that it's structure size is set
// correctly and that the bitmap dimensions have not been set as negative
BOOL CImageDisplay::CheckHeaderValidity(const BITMAPINFOHEADER *pBmiHeader, const DWORD *pBitFields)
{
#ifdef WINUTIL_STUB
    STUBRET(FALSE);
#else
    // Check the bitmap width and height are not negative.

    if (pBmiHeader->biWidth <= 0 ||
		pBmiHeader->biHeight <= 0) {
        NOTE("Invalid bitmap dimensions");
        return FALSE;
    }

    // Check the compression is either BI_RGB or BI_BITFIELDS

    if (pBmiHeader->biCompression != BI_RGB) {
        if (pBmiHeader->biCompression != BI_BITFIELDS) {
            NOTE("Invalid compression format");
            return FALSE;
        }
    }

    // If BI_BITFIELDS compression format check the colour depth

    if (pBmiHeader->biCompression == BI_BITFIELDS) {
        if (pBmiHeader->biBitCount != 16) {
            if (pBmiHeader->biBitCount != 24) {
                if (pBmiHeader->biBitCount != 32) {
                    NOTE("BI_BITFIELDS not 16/24/32 bit depth");
                    return FALSE;
                }
            }
        }
    }

    // Check the assumptions about the layout of the bit fields

    if (pBmiHeader->biCompression == BI_BITFIELDS) {
        if (CheckBitFields(pBitFields) == FALSE) {
            NOTE("Bit fields are not valid");
            return FALSE;
        }
    }

    // Are the number of planes equal to one

    if (pBmiHeader->biPlanes != 1) {
        NOTE("Number of planes not one");
        return FALSE;
    }

    // Check the image size is consistent (it can be zero)

    if (pBmiHeader->biSizeImage != GetBitmapSize(pBmiHeader)) {
        if (pBmiHeader->biSizeImage) {
            NOTE("Image size incorrectly set");
            return FALSE;
        }
    }

    // Check the size of the structure

    if (pBmiHeader->biSize != sizeof(BITMAPINFOHEADER)) {
        NOTE("Size of BITMAPINFOHEADER wrong");
        return FALSE;
    }
    return CheckPaletteHeader(pBmiHeader);
#endif // WINUTIL_STUB
}

BOOL CImageDisplay::CheckHeaderValidity(const VIDEOINFO *pInput)
{
#ifdef WINUTIL_STUB
    STUBRET(FALSE);
#else
	return CheckHeaderValidity (&pInput->bmiHeader, (DWORD *)BITMASKS(pInput));
#endif // WINUTIL_STUB
}


BOOL CImageDisplay::CheckHeaderValidity(const VIDEOINFO2 *pInput2)
{
#ifdef WINUTIL_STUB
    STUBRET(FALSE);
#else
	return CheckHeaderValidity (&pInput2->bmiHeader, (DWORD *)BITMASKS(pInput2));
#endif // WINUTIL_STUB
}


// This runs a few simple tests against the palette fields in the input to
// see if it looks vaguely correct. The tests look at the number of palette
// colours present, the number considered important and the biCompression
// field which should always be BI_RGB as no other formats are meaningful

BOOL CImageDisplay::CheckPaletteHeader(const BITMAPINFOHEADER *pBmiHeader)
{
#ifdef WINUTIL_STUB
    STUBRET(FALSE);
#else
    // The checks here are for palettised videos only

    if (BMI_PALETTISED(pBmiHeader) == FALSE) {
        if (pBmiHeader->biClrUsed) {
            NOTE("Invalid palette entries");
            return FALSE;
        }
        return TRUE;
    }

    // Compression type of BI_BITFIELDS is meaningless for palette video

    if (pBmiHeader->biCompression != BI_RGB) {
        NOTE("Palettised video must be BI_RGB");
        return FALSE;
    }

    // Check the number of palette colours is correct

    if (pBmiHeader->biClrUsed > BMI_PALETTE_ENTRIES(pBmiHeader)) {
        NOTE("Too many colours in palette");
        return FALSE;
    }

    // The number of important colours shouldn't exceed the number used

    if (pBmiHeader->biClrImportant > pBmiHeader->biClrUsed) {
        NOTE("Too many important colours");
        return FALSE;
    }
    return TRUE;
#endif // WINUTIL_STUB
}



BOOL CImageDisplay::CheckPaletteHeader(const VIDEOINFO *pInput)
{
#ifdef WINUTIL_STUB
    STUBRET(FALSE);
#else
	return CheckPaletteHeader (&pInput->bmiHeader);
#endif // WINUTIL_STUB
}


BOOL CImageDisplay::CheckPaletteHeader(const VIDEOINFO2 *pInput2)
{
#ifdef WINUTIL_STUB
    STUBRET(FALSE);
#else
	return CheckPaletteHeader (&pInput2->bmiHeader);
#endif // WINUTIL_STUB
}


// Return the format of the video display

const VIDEOINFO2 *CImageDisplay::GetDisplayFormat()
{
#ifdef WINUTIL_STUB
    STUBRET(NULL);
#else
    return &m_Display;
#endif // WINUTIL_STUB
}


// Return TRUE if the display uses a palette

BOOL CImageDisplay::IsPalettised()
{
#ifdef WINUTIL_STUB
    STUBRET(FALSE);
#else
    return BMI_PALETTISED(&m_Display.bmiHeader);
#endif // WINUTIL_STUB
}


// Return the bit depth of the current display setting

WORD CImageDisplay::GetDisplayDepth()
{
#ifdef WINUTIL_STUB
    STUBRET(8);
#else
    return m_Display.bmiHeader.biBitCount;
#endif // WINUTIL_STUB
}


// Initialise the optional fields in a VIDEOINFO. These are mainly to do with
// the source and destination rectangles and palette information such as the
// number of colours present. It simplifies our code just a little if we don't
// have to keep checking for all the different valid permutations in a header
// every time we want to do anything with it (an example would be creating a
// palette). We set the base class media type before calling this function so
// that the media types between the pins match after a connection is made
HRESULT CImageDisplay::UpdateFormat(BITMAPINFOHEADER *pBmiHeader)
{
    if (BMI_PALETTISED(pBmiHeader)) {
        if (pBmiHeader->biClrUsed == 0) {
            pBmiHeader->biClrUsed = BMI_PALETTE_ENTRIES(pBmiHeader);
        }
    }

    // The number of important colours shouldn't exceed the number used, on
    // some displays the number of important colours is not initialised when
    // retrieving the display type so we set the colours used correctly

    if (pBmiHeader->biClrImportant > pBmiHeader->biClrUsed) {
        pBmiHeader->biClrImportant = BMI_PALETTE_ENTRIES(pBmiHeader);
    }

    // Change the image size field to be explicit

    if (pBmiHeader->biSizeImage == 0) {
        pBmiHeader->biSizeImage = GetBitmapSize(pBmiHeader);
    }
    return NOERROR;
}

HRESULT CImageDisplay::UpdateFormat(VIDEOINFO *pVideoInfo)
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    ASSERT(pVideoInfo);

    SetRectEmpty(&pVideoInfo->rcSource);
    SetRectEmpty(&pVideoInfo->rcTarget);
	return UpdateFormat (&pVideoInfo->bmiHeader);
#endif // WINUTIL_STUB
}


HRESULT CImageDisplay::UpdateFormat(VIDEOINFO2 *pVideoInfo2)
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    ASSERT(pVideoInfo2);

    SetRectEmpty(&pVideoInfo2->rcSource);
    SetRectEmpty(&pVideoInfo2->rcTarget);
	return UpdateFormat (&pVideoInfo2->bmiHeader);
#endif // WINUTIL_STUB
}

// Lots of video rendering filters want code to check proposed formats are ok
// This checks the VIDEOINFO we are passed as a media type. If the media type
// is a valid media type then we return NOERROR otherwise E_INVALIDARG. Note
// however we only accept formats that can be easily displayed in the display
// so if we are on a 16 bit device we will not accept 24 bit images. The one
// complexity is that most displays draw 8 bit palettised images efficiently
// Also if the input format is less colour bits per pixel then we also accept
HRESULT CImageDisplay::CheckVideoType(const BITMAPINFOHEADER *pBmiHeader, const DWORD *pInputMask)
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    // First of all check the BITMAPINFOHEADER looks correct

    // Virtually all devices support palettised images efficiently

    if (m_Display.bmiHeader.biBitCount == pBmiHeader->biBitCount) {
        if (BMI_PALETTISED(pBmiHeader) == TRUE) {
            ASSERT(PALETTISED(&m_Display) == TRUE);
            NOTE("(Video) Type connection ACCEPTED");
            return NOERROR;
        }
    }

    // Is the display depth greater than the input format

    if (m_Display.bmiHeader.biBitCount > pBmiHeader->biBitCount) {
        NOTE("(Video) Mismatch agreed");
        return NOERROR;
    }

    // Is the display depth less than the input format

    if (m_Display.bmiHeader.biBitCount < pBmiHeader->biBitCount) {
        NOTE("(Video) Format mismatch");
        return E_INVALIDARG;
    }

    // Both input and display formats are either BI_RGB or BI_BITFIELDS

    ASSERT(m_Display.bmiHeader.biBitCount == pBmiHeader->biBitCount);
    ASSERT(BMI_PALETTISED(pBmiHeader) == FALSE);
    ASSERT(PALETTISED(&m_Display) == FALSE);

    // BI_RGB 16 bit representation is implicitly RGB555, and likewise BI_RGB
    // 24 bit representation is RGB888. So we initialise a pointer to the bit
    // fields they really mean and check against the display device format
    // This is only going to be called when both formats are equal bits pixel

    const DWORD *pDisplayMask = GetBitMasks(&m_Display);

    if (pInputMask[iRED] != pDisplayMask[iRED] ||
            pInputMask[iGREEN] != pDisplayMask[iGREEN] ||
                pInputMask[iBLUE] != pDisplayMask[iBLUE]) {

        NOTE("(Video) Bit field mismatch");
        return E_INVALIDARG;
    }

    NOTE("(Video) Type connection ACCEPTED");
    return NOERROR;
#endif // WINUTIL_STUB
}

HRESULT CImageDisplay::CheckVideoType(const VIDEOINFO *pInput)
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    if (CheckHeaderValidity(pInput) == FALSE) {
        return E_INVALIDARG;
    }

	const DWORD *pBitMasks = GetBitMasks (pInput);
	return CheckVideoType (&pInput->bmiHeader, pBitMasks);
#endif // WINUTIL_STUB
}


HRESULT CImageDisplay::CheckVideoType(const VIDEOINFO2 *pInput2)
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    if (CheckHeaderValidity(pInput2) == FALSE) {
        return E_INVALIDARG;
    }

	const DWORD *pBitMasks = GetBitMasks (pInput2);
	return CheckVideoType (&pInput2->bmiHeader, pBitMasks);
#endif // WINUTIL_STUB
}


// Return the bit masks for the true colour VIDEOINFO provided
const DWORD *CImageDisplay::GetBitMasks(const VIDEOINFO *pVideoInfo)
{
#ifdef WINUTIL_STUB
    STUBRET(NULL);
#else
    if (pVideoInfo->bmiHeader.biCompression == BI_BITFIELDS) {
        return BITMASKS(pVideoInfo);
    }

    static const DWORD FailMasks[] = {0,0,0};

    ASSERT(pVideoInfo->bmiHeader.biCompression == BI_RGB);

    switch (pVideoInfo->bmiHeader.biBitCount) {
        case 16: return bits555;
        case 24: return bits888;
        case 32: return bits888;
        default: return FailMasks;
    }
#endif // WINUTIL_STUB
}


const DWORD *CImageDisplay::GetBitMasks(const VIDEOINFO2 *pVideoInfo2)
{
#ifdef WINUTIL_STUB
    STUBRET(NULL);
#else
    if (pVideoInfo2->bmiHeader.biCompression == BI_BITFIELDS) {
        return BITMASKS(pVideoInfo2);
    }

    static const DWORD FailMasks[] = {0,0,0};

    ASSERT(pVideoInfo2->bmiHeader.biCompression == BI_RGB);

    switch (pVideoInfo2->bmiHeader.biBitCount) {
        case 16: return bits555;
        case 24: return bits888;
        case 32: return bits888;
        default: return FailMasks;
    }
#endif // WINUTIL_STUB
}


// Check to see if we can support media type pmtIn as proposed by the output
// pin - We first check that the major media type is video and also identify
// the media sub type. Then we thoroughly check the VIDEOINFO type provided
// As well as the contained VIDEOINFO being correct the major type must be
// video, the subtype a recognised video format and the type GUID correct

HRESULT CImageDisplay::CheckMediaType(const CMediaType *pmtIn)
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    // Does this have a VIDEOINFOHEADER format block

    const GUID *pFormatType = pmtIn->FormatType();
    if (*pFormatType != FORMAT_VideoInfo  &&  *pFormatType != FORMAT_VideoInfo2) {
        NOTE("Format GUID not a VIDEOINFOHEADER/VIDEOINFOHEADER2");
        return E_INVALIDARG;
    }
    ASSERT(pmtIn->Format());

    // Check the format looks reasonably ok

    ULONG Length = pmtIn->FormatLength();
    if (Length < SIZE_VIDEOHEADER) {
        NOTE("Format smaller than a VIDEOHEADER");
        return E_FAIL;
    }

    // Check the major type is MEDIATYPE_Video

    const GUID *pMajorType = pmtIn->Type();
    if (*pMajorType != MEDIATYPE_Video) {
        NOTE("Major type not MEDIATYPE_Video");
        return E_INVALIDARG;
    }

    // Check we can identify the media subtype

    const GUID *pSubType = pmtIn->Subtype();
    if (GetBitCount(pSubType) == USHRT_MAX) {
        NOTE("Invalid video media subtype");
        return E_INVALIDARG;
    }

	if (*pFormatType == FORMAT_VideoInfo)
	{
	    VIDEOINFO *pVideoInfo = (VIDEOINFO *) pmtIn->Format();
		return CheckVideoType (pVideoInfo);
	}
	else
	{
	    VIDEOINFO2 *pVideoInfo2 = (VIDEOINFO2 *) pmtIn->Format();
		return CheckVideoType (pVideoInfo2);
	}
#endif // WINUTIL_STUB
}


// Given a video format described by a VIDEOINFO structure we return the mask
// that is used to obtain the range of acceptable colours for this type, for
// example, the mask for a 24 bit true colour format is 0xFF in all cases. A
// 16 bit 5:6:5 display format uses 0xF8, 0xFC and 0xF8, therefore given any
// RGB triplets we can AND them with these fields to find one that is valid

BOOL CImageDisplay::GetColourMask(DWORD *pMaskRed,
                                  DWORD *pMaskGreen,
                                  DWORD *pMaskBlue)
{
#ifdef WINUTIL_STUB
    STUBRET(FALSE);
#else
    CAutoLock cDisplayLock(this);
    *pMaskRed = 0xFF;
    *pMaskGreen = 0xFF;
    *pMaskBlue = 0xFF;

    // If this format is palettised then it doesn't have bit fields

    if (m_Display.bmiHeader.biBitCount < 16) {
        return FALSE;
    }

    // If this is a 24 bit true colour display then it can handle all the
    // possible colour component ranges described by a byte. It is never
    // allowed for a 24 bit colour depth image to have BI_BITFIELDS set

    if (m_Display.bmiHeader.biBitCount == 24) {
        ASSERT(m_Display.bmiHeader.biCompression == BI_RGB);
        return TRUE;
    }

    // Calculate the mask based on the format's bit fields

    const DWORD *pBitFields = (DWORD *) GetBitMasks((VIDEOINFO *)&m_Display);
    DWORD *pOutputMask[] = { pMaskRed, pMaskGreen, pMaskBlue };

    // We know from earlier testing that there are no more than iMAXBITS
    // bits set in the mask and that they are all contiguous. All that
    // therefore remains is to shift them into the correct position

    for (INT iColour = iRED;iColour <= iBLUE;iColour++) {

        // This works out how many bits there are and where they live

        DWORD PrefixBits = CountPrefixBits(pBitFields[iColour]);
        DWORD SetBits = CountSetBits(pBitFields[iColour]);

        // The first shift moves the bit field so that it is right justified
        // in the DWORD, after which we then shift it back left which then
        // puts the leading bit in the bytes most significant bit position

        *(pOutputMask[iColour]) = pBitFields[iColour] >> PrefixBits;
        *(pOutputMask[iColour]) <<= (iMAXBITS - SetBits);
    }
    return TRUE;
#endif // WINUTIL_STUB
}


/*  Helper to convert to VIDEOINFOHEADER2
*/
STDAPI ConvertVideoInfoToVideoInfo2(AM_MEDIA_TYPE *pmt)
{
#ifdef WINUTIL_STUB
    STUBFAIL;
#else
    ASSERT(pmt->formattype == FORMAT_VideoInfo);
    VIDEOINFO *pVideoInfo = (VIDEOINFO *)pmt->pbFormat;
    PVOID pvNew = CoTaskMemAlloc(pmt->cbFormat + sizeof(VIDEOINFOHEADER2) -
                                 sizeof(VIDEOINFOHEADER));
    if (pvNew == NULL) {
        return E_OUTOFMEMORY;
    }
    CopyMemory(pvNew, pmt->pbFormat, FIELD_OFFSET(VIDEOINFOHEADER, bmiHeader));
    ZeroMemory((PBYTE)pvNew + FIELD_OFFSET(VIDEOINFOHEADER, bmiHeader),
               sizeof(VIDEOINFOHEADER2) - sizeof(VIDEOINFOHEADER));
    CopyMemory((PBYTE)pvNew + FIELD_OFFSET(VIDEOINFOHEADER2, bmiHeader),
               pmt->pbFormat + FIELD_OFFSET(VIDEOINFOHEADER, bmiHeader),
               pmt->cbFormat - FIELD_OFFSET(VIDEOINFOHEADER, bmiHeader));
    VIDEOINFOHEADER2 *pVideoInfo2 = (VIDEOINFOHEADER2 *)pvNew;
    pVideoInfo2->dwPictAspectRatioX = (DWORD)pVideoInfo2->bmiHeader.biWidth;
    pVideoInfo2->dwPictAspectRatioY = (DWORD)pVideoInfo2->bmiHeader.biHeight;
    pmt->formattype = FORMAT_VideoInfo2;
    CoTaskMemFree(pmt->pbFormat);
    pmt->pbFormat = (PBYTE)pvNew;
    pmt->cbFormat += sizeof(VIDEOINFOHEADER2) - sizeof(VIDEOINFOHEADER);
    return S_OK;
#endif // WINUTIL_STUB
}

