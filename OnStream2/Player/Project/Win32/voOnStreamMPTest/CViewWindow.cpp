	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CViewWindow.cpp

	Contains:	CViewWindow class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-02-15		JBF			Create file

*******************************************************************************/
#include "windows.h"
#include <tchar.h>

#include "CViewWindow.h"

//#include "OnStreamCPlayer.h"

#define	TIMER_UPDATE_VISUAL	123

LRESULT CALLBACK CViewWindow::ViewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CViewWindow * pViewWindow = (CViewWindow *)GetWindowLong (hwnd, GWL_USERDATA);
	if (pViewWindow == NULL)
		return(DefWindowProc(hwnd, uMsg, wParam, lParam));
	else
		return pViewWindow->OnReceiveMessage(hwnd,uMsg,wParam,lParam);
}

CViewWindow::CViewWindow(void)
	: m_hWnd (NULL)
	//, m_pPlayer (NULL)
	, mbIsFullScreen(false)
{
	_tcscpy (m_szClassName, _T("voViewWidnow"));
	_tcscpy (m_szWindowName, _T("voViewWidnow"));
}

CViewWindow::~CViewWindow(void)
{
	Reflush();
	UnregisterClass (m_szClassName, NULL);
	if(m_hWnd)
		::DestroyWindow(m_hWnd);
	m_hWnd = 0;
}

bool CViewWindow::CreateWnd (HWND hParent, RECT rcView)
{
	m_rcView = rcView;

	if (m_hWnd != NULL)
	{
		SetParent (m_hWnd, hParent);
		return true;
	}
#ifdef _WIN32_WCE
	HINSTANCE hInst = NULL;
#else
	HINSTANCE hInst = (HINSTANCE )GetWindowLong (hParent, GWL_HINSTANCE);
#endif //_WIN32_WCE

	WNDCLASS wcex;

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)ViewWindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)NULL;
	wcex.lpszMenuName	= (LPCTSTR)NULL;
	wcex.lpszClassName	= m_szClassName;

	RegisterClass(&wcex);

	m_hWnd = CreateWindow(m_szClassName, m_szWindowName, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
							CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hParent, NULL, hInst, NULL);

	if (m_hWnd == NULL)
		return false;
#ifndef WINCE
	SetWindowPos (m_hWnd, NULL, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
#else
	//SetWindowPos (m_hWnd, HWND_TOP, m_rcView.left, m_rcView.top + 24, m_rcView.right - m_rcView.left, m_rcView.bottom - m_rcView.top - 24, 0);
	SetWindowPos (m_hWnd, HWND_TOP, m_rcView.left + 150, m_rcView.top + 24, m_rcView.right - m_rcView.left - 300 , m_rcView.bottom - m_rcView.top - 24, 0);
#endif 
	

	

	LONG lRC = SetWindowLong (m_hWnd, GWL_USERDATA, (LONG)this);

	return true;
}


void CViewWindow::SetFullScreen (void)
{
	if (m_hWnd == NULL)
		return;

	if (!mbIsFullScreen)
	{
		SendMessage (GetParent (m_hWnd), WM_VIEW_FULLSCREEN, 1, 0);

		HWND hParent = GetParent (m_hWnd);
		if(hParent == NULL)
			return ;

		RECT rcParent;
		GetClientRect (hParent, &rcParent);

#ifdef WINCE
		SetWindowPos (m_hWnd, HWND_TOP, rcParent.left, rcParent.top, rcParent.right - rcParent.left, rcParent.bottom - rcParent.top, 0);
#else //WINCE
		SetWindowPos (m_hWnd, HWND_TOP, rcParent.left, rcParent.top, rcParent.right - rcParent.left, rcParent.bottom - rcParent.top - 24, 0);
#endif //WINCE;

		mbIsFullScreen = !mbIsFullScreen;
	}
	else
	{
		SendMessage (GetParent (m_hWnd), WM_VIEW_FULLSCREEN, 0, 0);

#ifdef WINCE
		SetWindowPos (m_hWnd, HWND_TOP, m_rcView.left + 150, m_rcView.top + 24, m_rcView.right - m_rcView.left - 300 , m_rcView.bottom - m_rcView.top - 24, 0);
		//SetWindowPos (m_hWnd, HWND_TOP, m_rcView.left, m_rcView.top + 24, m_rcView.right - m_rcView.left, m_rcView.bottom - m_rcView.top - 24, 0);
#else //WINCE
		SetWindowPos (m_hWnd, HWND_TOP, m_rcView.left, m_rcView.top, m_rcView.right - m_rcView.left, m_rcView.bottom - m_rcView.top, 0);
#endif //WINCE

		mbIsFullScreen = !mbIsFullScreen;
	}
	
	ResizeVideoWindow ();
}



bool CViewWindow::ResizeVideoWindow (void)
{
	if(m_hWnd == NULL)
		return false;

	HWND hChild = GetWindow (m_hWnd, GW_CHILD);
	if (hChild != NULL)
	{
		RECT rcView;

		GetClientRect (m_hWnd, &rcView);
		MoveWindow (hChild, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, TRUE);
	}

	return true;
}

bool	CViewWindow::Reflush()
{
	SendMessage(m_hWnd , WM_ERASEBKGND , 0 , 0);

	return true;
}


LRESULT CViewWindow::OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_ERASEBKGND && hwnd != NULL) 
	{
		RECT rcClient;
		GetClientRect(hwnd, &rcClient);

		HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
		FillRect((HDC)wParam, &rcClient, hBrush);
		DeleteObject(hBrush);
		
		return (LRESULT) 1;
	}
	else if (uMsg == WM_LBUTTONUP) 
	{
		SetFullScreen ();
	}
	else if (uMsg == WM_SIZE) 
	{
		return	DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	else if (uMsg == WM_TIMER)
	{
	
		return 0;
	}
	
	return	DefWindowProc(hwnd, uMsg, wParam, lParam);
}


