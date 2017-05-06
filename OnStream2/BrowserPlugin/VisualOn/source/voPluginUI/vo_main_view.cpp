//#include "StdAfx.h"
#include "vo_main_view.h"
#include "RESOURCE.h"
#include "CPlugInUIWnd.h"
#include "voLog.h"
#define _TRANSPARENT_WIN_

vo_main_view::vo_main_view(void)
{
	_tcscpy (m_szClassName, _T("voPlugInControlWidnow"));
	_tcscpy (m_szWindowName, _T("voPlugInControlWidnow"));
	m_hWndParent = NULL;
	m_pParent = 0;
	m_hWndView = NULL;
}

vo_main_view::~vo_main_view(void)
{
	SendMessage (m_hWndView, WM_CLOSE, 0, 0);
	UnregisterClass (m_szClassName, NULL);
}
LRESULT CALLBACK vo_main_view::ViewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	vo_main_view * pViewWindow = (vo_main_view *)GetWindowLong (hwnd, GWL_USERDATA);
	if (pViewWindow == NULL)
		return(DefWindowProc(hwnd, uMsg, wParam, lParam));
	else
		return pViewWindow->OnReceiveMessage(hwnd,uMsg,wParam,lParam);
}
LRESULT vo_main_view::OnReceiveMessage (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	UINT message = uMsg;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_TIMER:
		break;

	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);
			g_playback_control.draw( hdc );
			EndPaint(hWnd, &ps);
		}
		break;

	case WM_MOUSEMOVE :
		{
			int xPos = LOWORD(lParam); 
			int yPos = HIWORD(lParam);

			g_playback_control.pre_mouse_move( xPos , yPos );
		}
		break;
	case WM_LBUTTONDOWN :
		{
			int xPos = LOWORD(lParam); 
			int yPos = HIWORD(lParam);

			g_playback_control.pre_left_button_down( xPos , yPos );
		}
		break;
	case WM_LBUTTONUP :
		{
			int xPos = LOWORD(lParam); 
			int yPos = HIWORD(lParam);

			g_playback_control.pre_left_button_up( xPos , yPos );
		}
		break;

	case WM_DESTROY:
		if(hWnd)
		{
			HRGN rgn = NULL;
			::GetWindowRgn(hWnd,rgn);//::GetRe
			if(rgn)
				::DeleteObject(rgn);
		}
		break;
	case WM_SIZE:
		{
			int width = LOWORD( lParam );
			int height = HIWORD( lParam );

			g_playback_control.resize( width , height );
		}
		break;
	case WM_KEYUP:
		{
			if (wParam == VK_ESCAPE )
			{
				if (m_pParent && m_pParent->IsFullScreen ())
					m_pParent->OnCommand(ID_BUTTON_FULLSCREEN);
			}
		}
		break;
 	case WM_ERASEBKGND:
 		break;

	case WM_PLAY_FULL_SCREEN:
		{
			if (m_pParent)
				m_pParent->OnCommand(ID_BUTTON_FULLSCREEN);
		}
		break;
	case WM_PLAY_PLAY:
		{
			if (m_pParent)
				m_pParent->OnCommand(ID_BUTTON_PLAY);
		}
		//::OutputDebugString(_T("WM_PLAY_PLAY"));
		break;
	case WM_PLAY_PAUSE:
		{

		if (m_pParent)
			m_pParent->OnCommand(ID_BUTTON_PAUSE);
		
		}//::OutputDebugString(_T("WM_PLAY_PAUSE"));
		break;
	case WM_AUDIO_SLIDER:
		{
			if (m_pParent)
				m_pParent->OnPosChanged((int)wParam, false);
		
		}break;
	case WM_PLAY_SEEK:
		{
		if (m_pParent)
			m_pParent->OnPosChanged((int)wParam, true);
		}
		break;
  case WM_SELECT_AUDIO:
	  {
      if (m_pParent)
		  m_pParent->OnShowMenuTrackInfo(VOOSMP_SS_AUDIO);
	  }break;
  case WM_SELECT_VIDEO:
	  {
		  if (m_pParent)
			  m_pParent->OnShowMenuTrackInfo(VOOSMP_SS_VIDEO);
	  }break;
  case WM_SELECT_SUBTITLE:
	  {
		  if (m_pParent)
			  m_pParent->OnShowMenuTrackInfo(VOOSMP_SS_SUBTITLE);
	  }break;
  case WM_CLEAR_SELECTION:
	  {
		  if (m_pParent)
			  m_pParent->OnCommand(ID_BUTTON_CLEAR_SELECTION);
	  }break;
  case WM_COMMIT_SELECTION:
	  {
		  if (m_pParent)
			  m_pParent->OnCommand(ID_BUTTON_COMMIT_SELECTION);
	  }break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

  return 0;
}

bool vo_main_view::CreateWnd (HWND hwndParent, HINSTANCE hInst)
{
	if(hwndParent)
		m_hWndParent = hwndParent;
	if (m_hWndView != NULL)
		return true;

	//hInst = (HINSTANCE)m_initParam.hInst;

	WNDCLASS wcex;
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)ViewWindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)::CreateSolidBrush (RGB (180, 180, 180));
	wcex.lpszMenuName	= (LPCTSTR)NULL;
	wcex.lpszClassName	= m_szClassName;

	RegisterClass(&wcex);
#ifdef _TRANSPARENT_WIN_
	m_hWndView = CreateWindowEx(WS_EX_LAYERED|(WS_EX_TOOLWINDOW)&(~WS_EX_APPWINDOW), m_szClassName, m_szWindowName, WS_POPUP  | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
							CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInst, NULL);
#else
	m_hWndView = CreateWindow(m_szClassName, m_szWindowName, WS_CHILD  | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
							CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, m_hWndParent, NULL, hInst, NULL);
#endif
	if (m_hWndView == NULL)
		return false;

#ifdef _TRANSPARENT_WIN_
	::SetLayeredWindowAttributes(m_hWndView, RGB(255,255,255), 200,LWA_ALPHA);
#endif

	RECT rcView;
	GetClientRect (m_hWndParent, &rcView);
	SetWindowPos (m_hWndView, NULL, rcView.right-2, rcView.bottom-2, 2,  2, 0);
	LONG lRC = SetWindowLong (m_hWndView, GWL_USERDATA, (LONG)this);

	//SetLayeredWindowAttributes(
	//int nWidth = 42;
	//int nHeight = 28;
	//int nLeft = 0;
	//int nTop = 0;
	////m_hBtnPlay   = CreateWindowEx(0, L"Button", L"Play", WS_CHILD|WS_VISIBLE, nLeft, nTop, nWidth, nHeight, m_hWndView,(HMENU)IDM_EXIT, hInst, NULL);

	RECT rc;
	GetClientRect( m_hWndView , &rc );
	rc.top = 0;
	vo_rect rc_local(rc);
	g_playback_control.set_rect( &rc_local );
	g_playback_control.init(m_hWndView);

	return true;
}
void vo_main_view::resize(int cx, int cy)
{
  RECT rcView;
  if (m_pParent && m_pParent->IsFullScreen())
  {
    rcView = m_pParent->GetFullScreenRect();
  }
  else
  {
#ifdef _TRANSPARENT_WIN_
    GetWindowRect(m_hWndParent, &rcView);
#else
    if (cx==0 || cy==0)
    {
      GetClientRect(m_hWndParent, &rcView);
    }
    else
    {
      rcView.right = cx;
      rcView.bottom = cy;
    }
#endif
  }

//	SetWindowPos (m_hWndView, NULL, rcView.left, rcView.bottom-250, rcView.right - rcView.left,  250, 0);

  //now only one fullscreen butn
  SetWindowPos (m_hWndView, NULL, rcView.right-44, rcView.bottom-44, 44,  44, 0);


	//LONG lRC = SetWindowLong (m_hWndView, GWL_USERDATA, (LONG)this);

	{
		RECT rc;
		GetClientRect( m_hWndView , &rc );
		vo_rect rc_local(rc);
		g_playback_control.set_rect( &rc_local );
		g_playback_control.calculate_control_rect(rc.bottom);
		g_playback_control.reset_parent_window_region();
	}
}
void vo_main_view::ShowWindow(bool bShow)
{
	::ShowWindow(m_hWndView, bShow?SW_SHOW:SW_HIDE);

	// david 2013-04-25, even though hidden, still make it active to receive the input key up/down event
	if (bShow)
		SetActiveWindow(m_hWndView);
	else
		SetActiveWindow(m_hWndParent);

#ifdef _TRANSPARENT_WIN_
	if(bShow)
	{
		resize(0, 0);//fix bug: https://sh.visualon.com/node/28237 Change monitor when full screen mode, the controllor bar display a little upper place
		SetWindowPos (m_hWndView, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
	} 
#endif
}

bool vo_main_view::IsWindowVisible()
{
	return ::IsWindowVisible(m_hWndView);
}

bool vo_main_view::GetSliderPos(int nControl, int& nPos)
{
	{
		vo_slider * pSlider = g_playback_control.m_ptr_time_slider;
		if(nControl == 1)
			pSlider = g_playback_control.m_ptr_audio_slider;
		if(pSlider)
		{
			__int64 nPos2 = 0;
			pSlider->get_current_pos(nPos2);
			nPos = nPos2;
		}
	}
	return true;
}
bool vo_main_view::SetSliderPos(int nControl, int nPos)
{
	{
		vo_slider * pSlider = g_playback_control.m_ptr_time_slider;
		if(nControl == 1)
			pSlider = g_playback_control.m_ptr_audio_slider;
		if(pSlider)
		{
			__int64 nPos2 = nPos;
			pSlider->set_current_pos(nPos2);
			vo_rect rt;
			pSlider->get_rect(&rt);
			g_playback_control.children_need_repaint_callback(&rt);
			//::InvalidateRect(this->m_hWndView,NULL,FALSE);
		}
	}
	return true;
}
void  vo_main_view::UpdatePlayButtonStatus(bool bRunning)
{
	if(g_playback_control.m_ptr_play_button)
	{
		bool b = g_playback_control.m_ptr_play_button->get_checked();
		if(bRunning!=b)
		{
			g_playback_control.m_ptr_play_button->set_checked(bRunning);
			g_playback_control.children_need_repaint_callback(NULL);
		}
	}
}

void vo_main_view::SetCanSeek(bool bEnable)
{
  vo_slider * pSlider = g_playback_control.m_ptr_time_slider;
  if(pSlider)
  {
    pSlider->SetEnable(bEnable);
  }
}

void vo_main_view::SetDuration(const int dur) 
{

	g_playback_control.set_duration(dur);
}