	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CWndRender.cpp

	Contains:	CWndRender class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-02-15		JBF			Create file

*******************************************************************************/
#include "windows.h"
#include <tchar.h>

#include "CWndRender.h"


LRESULT CALLBACK CWndRender::ViewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CWndRender * pViewWindow = (CWndRender *)GetWindowLong (hwnd, GWL_USERDATA);
	if (pViewWindow == NULL)
		return(DefWindowProc(hwnd, uMsg, wParam, lParam));
	else
		return pViewWindow->OnReceiveMessage(hwnd,uMsg,wParam,lParam);
}

CWndRender::CWndRender(void)
	: m_hWnd (NULL)
{
	_tcscpy (m_szClassName, _T("vommRenderWidnow"));
	_tcscpy (m_szWindowName, _T("vommRenderWidnow"));
}

CWndRender::~CWndRender(void)
{
	DeleteObject (m_hBKPen);

	UnregisterClass (m_szClassName, NULL);
}

bool CWndRender::CreateWnd (HWND hParent, RECT rcView)
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
	wcex.hbrBackground	= (HBRUSH)::CreateSolidBrush (RGB (16, 0, 16));
	wcex.lpszMenuName	= (LPCTSTR)NULL;
	wcex.lpszClassName	= m_szClassName;


	RegisterClass(&wcex);
	m_hWnd = CreateWindow(m_szClassName, m_szWindowName, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
							CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hParent, NULL, hInst, NULL);
	if (m_hWnd == NULL)
		return false;

	SetWindowPos (m_hWnd, NULL, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
	LONG lRC = SetWindowLong (m_hWnd, GWL_USERDATA, (LONG)this);

	m_hBKPen = CreatePen (PS_SOLID, 4, RGB (0, 0, 0));

	return true;
}

LRESULT CWndRender::OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_ERASEBKGND) 
	{
		RECT rcClient;
		GetClientRect(hwnd, &rcClient);

		HBRUSH hBrush = CreateSolidBrush(RGB(16,0,16));
		FillRect((HDC)wParam, &rcClient, hBrush);
		DeleteObject(hBrush);

		return (LRESULT) 0;
	}
	else if (uMsg == WM_LBUTTONDOWN) 
	{
		return 0;
	}
	else if (uMsg == WM_SIZE) 
	{
		return	DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	else if (uMsg == WM_PAINT)
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		EndPaint(hwnd, &ps);
	}

	return	DefWindowProc(hwnd, uMsg, wParam, lParam);
}


