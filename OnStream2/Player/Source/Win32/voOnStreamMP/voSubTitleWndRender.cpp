#include "windows.h"
#include <tchar.h>

#include "voSubTitleWndRender.h"
#include "voSubtitleType.h"
#include "voSubTitleManager.h"

#define	TIMER_UPDATE_VISUAL	123


voSubTitleWndRender::voSubTitleWndRender(voSubTitleManager* manager)
	: voSubTitleRenderBase(manager)
{
	ResetetTransparentColor();
	//m_bEnableCreatRegion = false;
	m_bUseDDrawKeyColor = true;
	m_bEnableCreatRegion = true;
}

voSubTitleWndRender::~voSubTitleWndRender(void)
{
}
void voSubTitleWndRender::SetDisplayRect(VOOSMP_RECT& rectDisplay){
		//if(m_rectDisplay!=NULL)
		//	delete m_rectDisplay ;
		//m_rectDisplay = NULL;
	__super::SetDisplayRect(rectDisplay);
	//m_rectDisplay->left=300;
	//m_rectDisplay->top=300;
	//m_rectDisplay->right=800;
	//m_rectDisplay->bottom=600;
};

void voSubTitleWndRender::Show(bool bShow)
{
	m_bShow = bShow;
	if(this->GetRenderWindow())
	{
		::ShowWindow(m_hWnd, m_bShow?SW_SHOW:SW_HIDE);
	}
}

bool voSubTitleWndRender::Update(bool bForce)
{
	if(!bForce)
	{//check window position or size
		RECT rcParent;
		if(m_pManager != NULL)
		{
			GetClientRect(m_pManager->GetWindow(), &rcParent);
			//POINT ptParent;
			//memset(&ptParent,0,sizeof(ptParent));
			//::ClientToScreen(m_pManager->GetWindow(), &ptParent);
			RECT pt;
			::GetClientRect(m_hWnd,&pt);
			if(rcParent.right- rcParent.left != pt.right- pt.left ||
				rcParent.bottom- rcParent.top != pt.bottom- pt.top )
			{
				return false;
			}
		}
	}
	else
		m_bForceRedrawAll = true;

	HDC dc = ::GetDC(m_hWnd);
	bool b = OnDraw(dc);
	::ReleaseDC(m_hWnd, dc);
	return b;
}

bool voSubTitleWndRender::OnDraw(HDC dc, bool bOnlyBitblt)
{
	bOnlyBitblt = false;
	if(m_pManager==NULL || !::IsWindowVisible(m_hWnd))
		return false;
	RECT rcParent;
	GetClientRect(m_pManager->GetWindow(), &rcParent);
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	//POINT ptParent;
	//memset(&ptParent,0,sizeof(ptParent));
	//::ClientToScreen(m_pManager->GetWindow(), &ptParent);
	//RECT pt;

	bool bFullScreen = false;
	//::GetWindowRect(m_hWnd,&pt);
	if(rcParent.right- rcParent.left != rcClient.right- rcClient.left ||
			rcParent.bottom- rcParent.top != rcClient.bottom- rcClient.top)
	{
#ifndef WINCE
		//HWND_TOPMOST
		//int nW = ::GetSystemMetrics(SM_CXSCREEN);
		//int nH = ::GetSystemMetrics(SM_CYSCREEN);
		//if(nW<=rcParent.right - rcParent.left 
		//	&& nH<=rcParent.bottom - rcParent.top)
		//{
		//	bFullScreen = true;
		//	SetWindowPos (m_hWnd,HWND_TOPMOST, 0, 0, rcParent.right - rcParent.left, rcParent.bottom - rcParent.top, 0);
		//}
		//else
			SetWindowPos (m_hWnd, NULL,0, 0, rcParent.right - rcParent.left, rcParent.bottom - rcParent.top, 0);
#else
	//SetWindowPos (m_hWnd, HWND_TOP, rcParent.left, rcParent.top + 24, rcParent.right - rcParent.left, rcParent.bottom - rcParent.top - 24, 0);
		SetWindowPos (m_hWnd, HWND_TOP, 0, 0 , rcParent.right - rcParent.left , rcParent.bottom - rcParent.top , 0);
#endif 
		GetClientRect(m_hWnd, &rcClient);
	}
	//else
	//{
	//	int nW = ::GetSystemMetrics(SM_CXSCREEN);
	//	int nH = ::GetSystemMetrics(SM_CYSCREEN);
	//	if(nW<=rcParent.right - rcParent.left 
	//		&& nH<=rcParent.bottom - rcParent.top)
	//	{
	//		bFullScreen = true;
	//		SetWindowPos (m_hWnd,HWND_TOPMOST, 0, 0, rcParent.right - rcParent.left, rcParent.bottom - rcParent.top, 0);
	//	}
	//}

	//get valid rect to draw
	RECT r = rcClient;
	if(this->m_rectDisplay)
		r=*m_rectDisplay;

	OnDrawToDC( dc, r, bOnlyBitblt);
	return true;

}


bool voSubTitleWndRender::ResizeWindow (void)
{
	return true;
}

LRESULT voSubTitleWndRender::OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_ERASEBKGND) 
	{
		m_bForceRedrawAll = true;
		//if(!OnDraw((HDC)wParam))
		//{
		//	RECT rcClient;
		//	GetClientRect(hwnd, &rcClient);
		//	UpdateBack ((HDC)wParam, rcClient,RGB(1,1,1));

		//}
	
		return (LRESULT) 1;
	}
	else if (uMsg == WM_CLOSE) 
	{
		return 1;
	}
	else if (uMsg == WM_PAINT)
	{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			m_bForceRedrawAll = true;
			if(!OnDraw((HDC)hdc))
			{
				RECT rcClient;
				GetClientRect(hwnd, &rcClient);
				UpdateBack ((HDC)hdc, rcClient,RGB(1,1,1));

			}

			EndPaint(hwnd, &ps);
	}

	return	__super::OnReceiveMessage( hwnd, uMsg, wParam, lParam);//DefWindowProc(hwnd, uMsg, wParam, lParam);
}



