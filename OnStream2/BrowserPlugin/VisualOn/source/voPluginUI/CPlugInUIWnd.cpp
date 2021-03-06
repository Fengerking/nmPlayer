	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CPlugInUIWnd.cpp

	Contains:	CPlugInUIWnd class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2011-01-31		JBF			Create file

*******************************************************************************/
#include "windows.h"
#include <commctrl.h>

#include "voOSFunc.h"
#include "CPlugInUIWnd.h"
#include "voLog.h"
#include "resource.h"
#include "..\Common\voPluginCBType.h"

//#define _DEBUG_TEST_

extern HINSTANCE		g_hInst;

// david added @ 2013-05-16 for posting message
#define WM_USERDEFNOTIFY WM_USER + 101 //user notify message
#define WM_BUFFERING_TIMEOUT WM_USER +102 //when buffering for sometime, should automatically exit full screen
#define WM_UPDATE_SCREEN     WM_USER+103

#define	TIMER_UPDATE_CONTROL		101
#define	TIMER_HIDE_CONTROLS		  102 //sometimes need hide control, and when in fullscreen, hide the mouse at the same time


CPlugInUIWnd*   g_PluginUI = NULL;
BOOL            g_bTop = FALSE;
HHOOK           g_hHotKey = NULL;
BOOL Hotkey_Install(DWORD ThreadID);
BOOL Hotkey_UnInstall();
LRESULT CALLBACK Hotkey_Filter(int nCode, WPARAM wParam, LPARAM lParam);
voCMutex g_mutSysHook;

CPlugInUIWnd::CPlugInUIWnd (VOPUI_INIT_PARAM * pParam)
	: m_hWndView (NULL)
	, m_hWndParent (NULL)
	, m_hBrowserContextWnd(NULL)
	, m_pControlWindow(NULL)
	, m_nSkipTimes(0)
  , m_bForceUpdateTrackSelInfo(FALSE)
  , m_bFullScreen(FALSE)
  , m_bPopupMenu(FALSE)
  , m_bVideoRenderStart(FALSE)
  , m_bPureAudio(FALSE)
  , m_bCursorShow(TRUE)
  , m_bControlbarShow(TRUE)
  , m_dwTickLastMouseMove(0)
  , m_dwTickLastKeyEvent(0)
  , m_pFullscreenBtn(NULL)
  , m_bExit(FALSE)
  , m_hAnimationThd(NULL)
  , m_bMouseTracking(FALSE)
  , m_bEanblePlugin(TRUE)
  , m_bActive(TRUE)
  , m_dwLastClickLButtonTime(0)
  , m_hMonitorFullscreen(NULL)
  , m_bShowFullBtn(FALSE)
{
    g_PluginUI = this;

	memcpy (&m_initParam, pParam, sizeof (VOPUI_INIT_PARAM));
	m_hWndParent = (HWND) m_initParam.hView;
	memset(&m_rcView, 0, sizeof(RECT));

  VOLOGINIT(m_initParam.szWorkingPath);

  {
    TCHAR szIniFile[MAX_PATH] = _T("");
    _tcscpy(szIniFile,m_initParam.szWorkingPath);
    _tcscat(szIniFile,PLUGIN_SETTING_INI_FILE);
    TCHAR szTemp[10] = _T("");
    GetPrivateProfileString(_T("Timer"),_T("BufferingShow"), _T("120000"), szTemp, 10, szIniFile);
    m_dwWaitBufferingTime =  _ttoi(szTemp);
    _tcscpy(szTemp,_T(""));
    GetPrivateProfileString(_T("Timer"),_T("ButtonHide"), _T("2000"), szTemp, 10, szIniFile);
    m_dwHideButtonTime =  _ttoi(szTemp);
  }

	_tcscpy (m_szClassName, _T("voPlugInViewWidnow"));
	_tcscpy (m_szWindowName, _T("voPlugInViewWidnow"));

	m_brhBackground = (HBRUSH)::CreateSolidBrush (RGB_DEFAULT_BKG);
	m_bEnableLiveSeek = FALSE;
	m_image = NULL;
	m_hBackBitmap = NULL;
	m_pGDIPlusimage = NULL;
	m_imageFull = NULL;
	m_pGDIPlusimageFull = NULL;

    m_msgQueue.setCapability(MAX_MSG_COUNT);
    m_msgQueue.clearAll();

	CreateWnd ();
	InitWaitCursor();
}

CPlugInUIWnd::~CPlugInUIWnd ()
{
	{
		voCAutoLock l(&g_mutSysHook);
		Hotkey_UnInstall();
		g_PluginUI = NULL;
	}

  KillTimer(m_hWndView,TIMER_HIDE_CONTROLS);
  KillTimer(m_hWndView,TIMER_UPDATE_CONTROL);
  UnInitWaitCursor();

	if (m_brhBackground)
		::DeleteObject(m_brhBackground);
	if(m_pControlWindow)
		delete m_pControlWindow;
	m_pControlWindow = NULL;
    if(m_pFullscreenBtn)
        delete m_pFullscreenBtn;
    m_pFullscreenBtn = NULL;
	SendMessage (m_hWndView, WM_CLOSE, 0, 0);

	UnregisterClass (m_szClassName, NULL);

	VOLOGUNINIT();

	
	if(m_pGDIPlusimageFull)
		delete m_pGDIPlusimageFull;
	if(m_imageFull)
		delete m_imageFull;
	if(m_pGDIPlusimage)
		delete m_pGDIPlusimage;
	if(m_image)
		delete m_image;
	m_pGDIPlusimageFull = NULL;
	m_pGDIPlusimage = NULL;
	m_image = NULL;
	m_imageFull = NULL;

	m_bExit = TRUE;
	voOS_Sleep(100);
	voThreadClose(m_hAnimationThd, 0);
	//m_semAnimation.Up();
}

bool CPlugInUIWnd::InitWaitCursor()
{
  //memset(m_hBmpList, 0, sizeof(HBITMAP) * 12);
  m_nBmpWidth = 0;
  m_nBmpHeight = 0;

  m_iBMPIndex = 0;
  m_bBufferBegin = FALSE;
  m_bBuffering = FALSE;


  m_hdc = GetDC(m_hWndView);
  if (m_hdc == NULL) 
    return FALSE;

  m_hdcMem = CreateCompatibleDC(m_hdc);
  if (m_hdcMem == NULL)
    return FALSE;

  if(m_image==NULL){
	  m_image = new CImage();//to init gdi+
	  CFullscreenBtn::InitCImage(m_image, IDB_PNG_BUFFERING, &m_pGDIPlusimage, false);
	  m_imageFull = new CImage();//to init gdi+
	  CFullscreenBtn::InitCImage(m_imageFull, IDB_PNG_BUFFERING+1000, &m_pGDIPlusimageFull, false);
		//HMODULE hInstance = g_hInst;
		//HRSRC hRsrc = ::FindResource(hInstance, MAKEINTRESOURCE(IDB_PNG_BUFFERING), _T("PNG"));
		// ATLASSERT(hRsrc != NULL);
	 //
		//DWORD dwSize = ::SizeofResource(hInstance, hRsrc);
		// LPBYTE lpRsrc = (LPBYTE)::LoadResource(hInstance, hRsrc);
		// ATLASSERT(lpRsrc != NULL);
	 //
		//HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, dwSize);
		// LPBYTE pMem = (LPBYTE)::GlobalLock(hMem);
		// memcpy(pMem, lpRsrc, dwSize);
		// IStream* pStream = NULL;
		// ::CreateStreamOnHGlobal( hMem, FALSE, &pStream);
	 //
		// m_image->Load(pStream);
		// m_pGDIPlusimage = new Gdiplus::Bitmap(pStream);
		// pStream->Release();
		//::GlobalUnlock(hMem);
		// ::FreeResource(lpRsrc);
		// ::GlobalFree(hMem);
  }
  m_nBmpWidth = m_image->GetWidth();
  m_nBmpHeight = m_image->GetHeight();

  return TRUE;
}

void  CPlugInUIWnd::UnInitWaitCursor()
{
  for (int i = 0; i < 12; ++i) {
    if (m_hBmpList[i]) {
      ::DeleteObject(m_hBmpList[i]);
      m_hBmpList[i] = NULL;
    }
  }
  if(m_hBackBitmap)
	::DeleteObject(m_hBackBitmap);
  m_hBackBitmap = NULL;
  

  ::DeleteDC(m_hdcMem);
  m_hdcMem = NULL;

  ::ReleaseDC(m_hWndView,m_hdc);
  m_hdc = NULL;
}

void *	CPlugInUIWnd::GetView (void)
{
	return m_hWndView;
}

void CPlugInUIWnd::SetWndTopmostIfFullScreen()
{
  if (!IsFullScreen())
  {
    return;
  }

  VOLOGI("SetWndTopmostIfFullScreen 1");

  RECT rcView = GetFullScreenRect();
#ifdef _DEBUG_TEST_
  SetWindowPos (m_hWndView, HWND_TOP, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
#else
  SetWindowPos (m_hWndView, HWND_TOP, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
#endif

  VOLOGI("SetWndTopmostIfFullScreen 2");
}
bool CPlugInUIWnd::ShowFullScreen (void)
{
	VOLOGI("Begin");

	if (m_bEanblePlugin==FALSE)
	{
		return false;
	}

	m_hMonitorFullscreen = NULL;
	RECT rcView;

    m_bControlbarShow = FALSE;
    if (m_pFullscreenBtn) {
        m_pFullscreenBtn->showButton(1, false);
        m_pFullscreenBtn->showButton(2,false);
    }

	if (m_bBuffering)
	{
		if(m_hBackBitmap){
			::DeleteObject(m_hBackBitmap);
			m_hBackBitmap = NULL;
		}
	}

	if (m_bFullScreen)
	{
		m_bFullScreen = FALSE;
		if(m_pFullscreenBtn)
			m_pFullscreenBtn->SetFullScreen(false);

		SetParent (m_hWndView, m_hWndParent);
        ::SetFocus(m_hBrowserContextWnd);//when back to normal window, should set focus back to browser.

		GetClientRect (m_hWndParent, &rcView);
		
		SetWindowPos (m_hWndView, HWND_TOP, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
		ShowMouseCursor(TRUE);

		KillTimer(m_hWndView,TIMER_HIDE_CONTROLS);
		SetTimer (m_hWndView, TIMER_HIDE_CONTROLS, 2000, NULL);		
	}
	else
	{
		if (!m_bShowFullBtn)
			return false;

        if (m_bBuffering)
        {
            m_dwTickStartBuffering = GetTickCount();
        }

		m_bFullScreen = TRUE;
		if(m_pFullscreenBtn)
			m_pFullscreenBtn->SetFullScreen(true);

        SetFocus (m_hWndView);

		SetParent (m_hWndView, NULL);
		rcView = GetFullScreenRect();
		
#ifdef _DEBUG_TEST_
		SetWindowPos (m_hWndView, HWND_TOP, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
#else
		SetWindowPos (m_hWndView, HWND_TOP, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
#endif

		UpdateControlSlide();
		UpdateVolumeSlide();
		UpdateSeekEnableStatus();

		//::PostMessage(m_hWndView, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(1, 1));
		::PostMessage(m_hWndView, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(10, 10));
		VOLOGI("full over.");
	}

	UpdateCursorPosition();
	m_bPopupMenu = FALSE;
	return true;
}
bool	CPlugInUIWnd::IsControlsVisible (void)
{
  return m_bControlbarShow;//now, mouse move will check if bar is show, so use the member value to check.

	bool b = false;
	if(m_pControlWindow)
	{
		b = this->m_pControlWindow->IsWindowVisible();
	}
	return b;
}
int CPlugInUIWnd::ShowControls (bool bShow)
{
    //maybe the window is not ready.
    if ((m_rcView.right-m_rcView.left)<=10 || (m_rcView.bottom-m_rcView.top)<=10)
    {
        return 0;
    }

	if (m_bControlbarShow == bShow)
		return 0;

	m_bControlbarShow = bShow;

	if (m_hWndView == NULL)
		return -1;

	if(m_pControlWindow)
	{
		this->m_pControlWindow->ShowWindow(bShow);
		UpdatePlayPauseButton();
	}

	ShowMouseCursor(bShow);
	if (m_pFullscreenBtn )
	{
		if (m_bFullScreen) {
			m_pFullscreenBtn->showButton(2, bShow);
		} else {
			
			RECT r, cr;
			GetWindowRect(m_hBrowserContextWnd, &cr);
			GetWindowRect(m_hWndView, &r);
			if (r.right > cr.right || r.bottom > cr.bottom) 
				return 0;

			if (bShow && m_bShowFullBtn) {
				m_bControlbarShow = TRUE;
				m_pFullscreenBtn->showButton(1);
			} else {
				m_bControlbarShow = FALSE;			
				m_pFullscreenBtn->showButton(1, false);
			}
		}
	}

	VOLOGI("end");
	return 0;
}

int CPlugInUIWnd::SetParam (int nID, void * pParam)
{
  switch(nID)
  {
  case VOOSMP_PLUGIN_PLAYER_STATUS:
      {
          if(pParam == NULL)
              return -1;
          VOOSMP_STATUS status = (VOOSMP_STATUS) (*(int *)pParam);
          switch(status)
          {
          case VOOSMP_STATUS_INIT:
              {
                  //stop buffering
                  m_bVideoRenderStart = FALSE;
                  m_bPureAudio = FALSE;
                  m_bBuffering = FALSE;

                  ControlFullBtn(FALSE);
              }
              break;
          case VOOSMP_STATUS_STOPPED:
              {
                  m_bVideoRenderStart = FALSE;
                  m_bPureAudio = FALSE;
                  m_bBuffering = FALSE;

                  if(IsFullScreen())
                      ShowFullScreen();

                  EraseBackGround(m_hdc);

                  ControlFullBtn(FALSE);
              }
              break;
          }
      }
      break;

  case VOOSMP_PLUGIN_ENALBE_THIS_INSTANCE:
      m_bEanblePlugin = pParam!=NULL ? TRUE : FALSE;
      return 0;

  case VOOSMP_PLUGIN_UNIT_OSMP_PLAYER:
    {
      if (IsFullScreen())
      {
        ShowFullScreen();
      }
      m_initParam.pOSMPPlayer = NULL;
    }
    return 0; 
  case VOOSMP_PLUGIN_CB_EVENT_INFO:
    {
      if(pParam)
      {
        VOCBMSG cbEventInfo = *(VOCBMSG *)pParam;
		if (cbEventInfo.nID == VOOSMP_CB_Video_Render_Complete || cbEventInfo.nID == VOOSMP_CB_VR_USERCALLBACK)
	    {
			if(m_bBuffering == TRUE){
				VOOSMP_VR_USERCALLBACK_TYPE* pVr = (VOOSMP_VR_USERCALLBACK_TYPE*)cbEventInfo.nValue1;
				if(pVr){
					this->ShowWaitCursor(pVr);
				}
			}
			return 0;
	    }

        switch (cbEventInfo.nID)
        {
        case VOOSMP_CB_VideoStartBuff:
        case VOOSMP_CB_AudioStartBuff:
        case VOOSMP_CB_VideoStopBuff:
        case VOOSMP_CB_AudioStopBuff:
            cbHandleEvent(cbEventInfo);
            return 0;
        default:
            break;
        }

        {
            voCAutoLock lock (&m_mtMsgQueue);
            m_msgQueue.enqueue(cbEventInfo);
        }

        VOLOGI("post WM_USERDEFNOTIFY 0x%08x", cbEventInfo.nID);
        ::PostMessage(m_hWndView, WM_USERDEFNOTIFY, NULL, NULL);
      }
    }
    return 0;
  default:
    break;
  }
  VOLOGI("End");
	return -1;
}

int CPlugInUIWnd::GetParam (int nID, void * pParam)
{
	return -1;
}

LRESULT CALLBACK CPlugInUIWnd::ViewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CPlugInUIWnd * pViewWindow = (CPlugInUIWnd *)GetWindowLong (hwnd, GWL_USERDATA);
	if (pViewWindow == NULL)
		return(DefWindowProc(hwnd, uMsg, wParam, lParam));
	else
		return pViewWindow->OnReceiveMessage(hwnd,uMsg,wParam,lParam);
}

bool CPlugInUIWnd::CreateWnd (void)
{
  VOLOGI("Begin");

	if (m_hWndView != NULL)
		return true;

	m_hBrowserContextWnd = GetParent(GetParent(m_hWndParent));
	HINSTANCE hInst = (HINSTANCE)m_initParam.hInst;

	WNDCLASS wcex;
	wcex.style			= CS_HREDRAW | CS_VREDRAW ;
	wcex.lpfnWndProc	= (WNDPROC)ViewWindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= m_brhBackground;//
	wcex.lpszMenuName	= (LPCTSTR)NULL;
	wcex.lpszClassName	= m_szClassName;

	RegisterClass(&wcex);
	m_hWndView = CreateWindow(m_szClassName, m_szWindowName, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
							CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, m_hWndParent, NULL, hInst, NULL);
	if (m_hWndView == NULL)
		return false;

	RECT rcView;
	GetClientRect (m_hWndParent, &rcView);
	m_rcView = rcView;
	VOLOGI("[video window pos] l = %d, t = %d, r = %d, b = %d\n", rcView.left, rcView.top, rcView.right, rcView.bottom);
	// modified to avoid white screen on FF & Chrome by David @ 2013/03/26
	SetWindowPos (m_hWndView, HWND_TOPMOST, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
	SetFocus(m_hWndView);

	LONG lRC = SetWindowLong (m_hWndView, GWL_USERDATA, (LONG)this);

	if(m_pControlWindow)
		delete m_pControlWindow;
//  m_pControlWindow = new vo_main_view() ;
  if (m_pControlWindow)
  {	
    m_pControlWindow->SetParentObject(this);
    m_pControlWindow->CreateWnd(m_hWndView, hInst);
  }

  if(m_pFullscreenBtn)
    delete m_pFullscreenBtn;
  m_pFullscreenBtn = new CFullscreenBtn(m_hWndView);
  if(m_pFullscreenBtn)
    m_pFullscreenBtn->SetParentObject(this);

	if (m_initParam.nBrowserType)
		UpdateSize();

	if(NULL == m_hAnimationThd)
	{
		VO_U32 tID = 0;
		voThreadCreate(&m_hAnimationThd, &tID, (voThreadProc)msgHandlerProc, this, 0);
		VOLOGI("m_hMsgLoop: %p", m_hAnimationThd);
	}

    DWORD   dwThreadID   =   GetWindowThreadProcessId(m_hBrowserContextWnd, NULL);
    Hotkey_Install(dwThreadID);

	return true;
}

bool CPlugInUIWnd::IsFullScreen (void)
{
  return m_bFullScreen;
  
}
void CPlugInUIWnd::OnPosChanged(int nPos, bool bTimeSlider)
{
	VOLOGI("Begin");

	if(bTimeSlider)
	{
		if(m_nSkipTimes>0)//need this case, to avoid crashing
			return;

		//int nPos = SendMessage (m_hWndSldPos, TBM_GETPOS, 0, 0);
		int nDuration = GetDuration();
		if (nDuration > 0)
		{
			int nSetPos = nDuration * nPos / 1000;
			if (m_initParam.NotifyCommand != NULL)
			{
				int nID = VOUI_CMD_SETPOS;
				m_initParam.NotifyCommand (m_initParam.hUserData, VO_OSMP_CB_WINDOWED_UI_COMMAND,&nID, &nSetPos);
			}
			else
				m_initParam.pOSMPPlayer->SetPos(nSetPos);
			m_nSkipTimes = 4;
		}
	}
	else
	{
		if (m_initParam.pOSMPPlayer != NULL)
		{
			int nVolume = nPos;//100 - SendMessage (m_hWndSldAudio, TBM_GETPOS, 0, 0);
			if (nPos>95)//bug:	The volume can not change to 100 while full screen model
			{
				nVolume = 100;
			}
			if (m_initParam.NotifyCommand != NULL)
			{
				int nID = VOUI_CMD_SETVOLUME;
				m_initParam.NotifyCommand (m_initParam.hUserData,VO_OSMP_CB_WINDOWED_UI_COMMAND, &nID, &nVolume);
			}
			else
				m_initParam.pOSMPPlayer->SetParam(VOOSMP_PID_AUDIO_VOLUME, &nVolume);
			//need to skip update UI 2 second;
		}
	}
  VOLOGI("End");
}

void CPlugInUIWnd::OnShowMenuTrackInfo(int wmID)
{
  VOLOGI("Begin");


  CvoOnStreamMP * pOSMPPlayer = m_initParam.pOSMPPlayer;
  if (NULL == pOSMPPlayer)
  {
    return;
  }

  m_bPopupMenu = TRUE;

  POINT point;
  GetCursorPos(&point);

  switch(wmID)
  {
  case VOOSMP_SS_VIDEO:
    {
      HMENU hMenuSelVideo = CreatePopupMenu();
      if (hMenuSelVideo == NULL)
        return;

      for (int i=0; i< pOSMPPlayer->GetVideoCount(); ++i)
      {
        CHAR szMenuText[MAX_PATH] = "";

        GetMenuShowTextByIndex(szMenuText,VOOSMP_SS_VIDEO, i);

        UINT uFlags = pOSMPPlayer->IsVideoAvailable(i)==TRUE ? MF_ENABLED : MF_GRAYED;
        if (uFlags!=MF_ENABLED)//do not show the disable track
        {
          continue;
        }
        AppendMenuA(hMenuSelVideo, MF_STRING|uFlags,ID_MENU_SEL_VIDEO+i,szMenuText);
      }

      //add "auto select"
      AppendMenuA(hMenuSelVideo, MF_STRING,ID_MENU_SEL_VIDEO+pOSMPPlayer->GetVideoCount(),"Auto select");

      VOOSMP_SRC_CURR_TRACK_INDEX curIndex;
      m_initParam.pOSMPPlayer->GetCurrTrackSelection(&curIndex);
      if(-1 == curIndex.nCurrVideoIdx)
        curIndex.nCurrVideoIdx = pOSMPPlayer->GetVideoCount();
      CheckMenuItem(hMenuSelVideo,ID_MENU_SEL_VIDEO+curIndex.nCurrVideoIdx,MF_BYCOMMAND|MF_CHECKED);

      TrackPopupMenu(hMenuSelVideo,TPM_BOTTOMALIGN,point.x,GetSystemMetrics(SM_CYSCREEN)-40,0,m_hWndView,NULL);

      DestroyMenu(hMenuSelVideo);
    }
    break;
  case VOOSMP_SS_AUDIO:
    {
      HMENU hMenuSelAudio = CreatePopupMenu();
      if (hMenuSelAudio == NULL)
        return;

      for (int i=0; i< pOSMPPlayer->GetAudioCount(); ++i)
      {
        CHAR szMenuText[MAX_PATH] = "";
        GetMenuShowTextByIndex(szMenuText,VOOSMP_SS_AUDIO, i);

        UINT uFlags = pOSMPPlayer->IsAudioAvailable(i)==TRUE ? MF_ENABLED : MF_GRAYED;
        if (uFlags!=MF_ENABLED)//do not show the disable track
        {
          continue;
        }
        AppendMenuA(hMenuSelAudio, MF_STRING|uFlags,ID_MENU_SEL_AUDIO+i,szMenuText);
      }

      VOOSMP_SRC_CURR_TRACK_INDEX curIndex;
      m_initParam.pOSMPPlayer->GetCurrTrackSelection(&curIndex);
      CheckMenuItem(hMenuSelAudio,ID_MENU_SEL_AUDIO+curIndex.nCurrAudioIdx,MF_BYCOMMAND|MF_CHECKED);

      TrackPopupMenu(hMenuSelAudio,TPM_BOTTOMALIGN,point.x,GetSystemMetrics (SM_CYSCREEN)-40,0,m_hWndView,NULL);
      DestroyMenu(hMenuSelAudio);
    }
    break;
  case VOOSMP_SS_SUBTITLE:
    {
      if (m_initParam.pOSMPPlayer)
      {
        BOOL bEnable = FALSE;
        m_initParam.pOSMPPlayer->GetParam(VOOSMP_PID_COMMON_CCPARSER,&bEnable);
        if (FALSE == bEnable)
        {
          break;
        }
      }

      HMENU hMenuSelSubtitle = CreatePopupMenu();
      if (hMenuSelSubtitle == NULL)
        return;

      for (int i=0; i< pOSMPPlayer->GetSubtitleCount(); ++i)
      {
        CHAR szMenuText[MAX_PATH] = "";
        GetMenuShowTextByIndex(szMenuText,VOOSMP_SS_SUBTITLE, i);

        UINT uFlags = pOSMPPlayer->IsSubtitleAvailable(i)==TRUE ? MF_ENABLED : MF_GRAYED;
        if (uFlags!=MF_ENABLED)//do not show the disable track
        {
          continue;
        }
        AppendMenuA(hMenuSelSubtitle, MF_STRING|uFlags,ID_MENU_SEL_SUBTITLE+i,szMenuText);
      }

      VOOSMP_SRC_CURR_TRACK_INDEX curIndex;
      m_initParam.pOSMPPlayer->GetCurrTrackSelection(&curIndex);
      CheckMenuItem(hMenuSelSubtitle,ID_MENU_SEL_SUBTITLE+curIndex.nCurrSubtitleIdx,MF_BYCOMMAND|MF_CHECKED);

      TrackPopupMenu(hMenuSelSubtitle,TPM_BOTTOMALIGN,point.x,GetSystemMetrics (SM_CYSCREEN)-40,0,m_hWndView,NULL);
      DestroyMenu(hMenuSelSubtitle);
    }
    break;
  default:
    break;
  }

  VOLOGI("End");
}

void CPlugInUIWnd::OnSelectTrack(int wmId)
{
  if (NULL == m_initParam.pOSMPPlayer)
  {
    return;
  }

  VOLOGI("Begin");

  m_bPopupMenu = false;

  void * pValue1 = NULL;
  void * pValue2 = NULL;
  int    nIndex = 0;

  int nID = VOUI_CMD_PLAY;
  if (wmId >= ID_MENU_SEL_AUDIO && wmId<(ID_MENU_SEL_AUDIO+m_initParam.pOSMPPlayer->GetAudioCount()))
  {
    nIndex = wmId - ID_MENU_SEL_AUDIO;

    m_initParam.pOSMPPlayer->SelectAudio(nIndex);
  }
  else if (wmId >= ID_MENU_SEL_VIDEO && wmId<=(ID_MENU_SEL_VIDEO+m_initParam.pOSMPPlayer->GetVideoCount()))
  {
    nIndex = wmId - ID_MENU_SEL_VIDEO;
    if (nIndex==m_initParam.pOSMPPlayer->GetVideoCount())
    {
      nIndex = -1;
    }

    m_initParam.pOSMPPlayer->SelectVideo(nIndex);
  }
  else if (wmId >= ID_MENU_SEL_SUBTITLE && wmId<(ID_MENU_SEL_SUBTITLE+m_initParam.pOSMPPlayer->GetSubtitleCount()))
  {
    nIndex = wmId - ID_MENU_SEL_SUBTITLE;

    m_initParam.pOSMPPlayer->SelectSubtitle(nIndex);
  }

  VOLOGI("End");

  return;
}

void CPlugInUIWnd::OnCommand(int wmId)
{
  m_bPopupMenu = FALSE;

  int nID = VOUI_CMD_PLAY;
  switch(wmId)
  {
  case ID_BUTTON_PLAY:
	  {
		nID = VOUI_CMD_PLAY;
	  }
      break;
  case ID_BUTTON_PAUSE:
	  {
		nID = VOUI_CMD_PAUSE;
	  }
      break;
  case ID_BUTTON_STOP:
    {
      nID = VOUI_CMD_STOP;
      m_bEnableLiveSeek = FALSE;
      UpdateSeekEnableStatus();
    }
    break;
  case ID_BUTTON_FULLSCREEN:
    {
      ShowFullScreen();
      return;
      nID = VOUI_CMD_FULLSCREEN;
    }
    break;
  case ID_BUTTON_CLEAR_SELECTION:
    {
      if(m_initParam.pOSMPPlayer)
        m_initParam.pOSMPPlayer->ClearSelection();
      return;
    }
    break;
  case ID_BUTTON_COMMIT_SELECTION:
    {
      if(m_initParam.pOSMPPlayer)
        m_initParam.pOSMPPlayer->CommitSelection();
      return;
    }
    break;
  default:
    break;
  }

  if(NULL!=m_initParam.NotifyCommand)
  {
    m_initParam.NotifyCommand (m_initParam.hUserData, VO_OSMP_CB_WINDOWED_UI_COMMAND, &nID, NULL);
  }

  UpdatePlayPauseButton();
}

void CPlugInUIWnd::UpdatePlayPauseButton()
{
	if (IsControlsVisible())
	{
    if (m_initParam.pOSMPPlayer != NULL)
		{
      if(this->m_pControlWindow)
        m_pControlWindow->UpdatePlayButtonStatus(GetPlayerStatus() == VOOSMP_STATUS_RUNNING);
		}
	}
}

void CPlugInUIWnd::EraseBackGround(HDC hDC)
{
  RECT	r;
  GetClientRect(m_hWndView,&r);

  HDC hRealDC = hDC;
  if (m_initParam.nBrowserType == 1)
	  hRealDC = ::GetDC(m_hWndView);

  ::FillRect(hRealDC, &r, m_brhBackground);

  VOLOGI("browser: %d, input hDC: %p, realDC: %p", m_initParam.nBrowserType, hDC, hRealDC);
  if (m_initParam.nBrowserType == 1)
	::ReleaseDC(m_hWndView, hRealDC);
}
bool CPlugInUIWnd::UpdateBackground (HDC hDC)
{

	HWND hwnd = this->m_hWndView;
	if (hwnd == NULL)
		return false;

	bool bUpdated = false;
  if (m_initParam.pOSMPPlayer != NULL)
	{
		VOOSMP_VIDEO_FORMAT fmt;
		memset(&fmt, 0, sizeof(VOOSMP_VIDEO_FORMAT));
//    int n = m_initParam.pOSMPPlayer->GetParam(VOOSMP_PID_VIDEO_FORMAT,&fmt);
		RECT r;
		GetClientRect (hwnd, &r);
		if(fmt.Height>0 && fmt.Width>0 && r.right - r.left>0)
		{
			VOOSMP_RECT rt;
			memset(&rt, 0, sizeof(VOOSMP_RECT));
			//int nWidth;
			float fRate = 1.0f;
			if (fmt.Width)
				fRate = fmt.Height/(float)fmt.Width;

			int nWidth = r.right - r.left;
			int nHeight = r.bottom - r.top;
			int nWidthRight = nWidth-nWidth/4*4;
			int nHeightBott = (int)(fRate * nWidthRight);

			float fRate2 = 1.0f;
			if (nWidth)
				fRate2 = nHeight/((float)nWidth);

			if(fRate2>fRate)
			{
				rt.nTop = (nHeight - nWidth*fRate)/2+0.5;
				rt.nTop++;
				rt.nBottom = (nHeight - rt.nTop)-nHeightBott;
				rt.nRight = r.right;
			}
			else if(fRate2<fRate)
			{
				if (fRate != 0)
					rt.nLeft = (nWidth - (nHeight)/fRate)/2+0.5;
				rt.nLeft++;
				rt.nRight = (nWidth - rt.nLeft)-nWidthRight;
				rt.nBottom = r.bottom;
			}
			else
				return true;

			if(rt.nTop>0)
			{
				r.bottom = rt.nTop;
			}
			else
			{
				r.right = rt.nLeft;
			}
			::FillRect(hDC,&r,m_brhBackground);
			GetClientRect (hwnd, &r);
			if(rt.nTop>0)
			{
				r.top = rt.nBottom-1;
			}
			else
			{
				r.left = rt.nRight;
			}
			::FillRect(hDC,&r,m_brhBackground);

			if(nWidthRight>0)
			{
				GetClientRect (hwnd, &r);
				if(rt.nTop>0)
				{
					r.left = rt.nRight - nWidthRight ;
				}
				else
				{
					r.top = rt.nBottom - nHeightBott;
				}
				::FillRect(hDC,&r,m_brhBackground);
			}
			bUpdated = true;
		}
	}

	return bUpdated;
}

bool CPlugInUIWnd::UpdateSize ()
{
	HWND hwnd = this->m_hWndView;
	if (hwnd == NULL)
		return false;

	RECT rcView;
	if (IsFullScreen())
		rcView = GetFullScreenRect();
	else 
		GetClientRect (m_hWndParent, &rcView);

	int stat = GetPlayerStatus();
	if(m_rcView.right != rcView.right ||m_rcView.bottom != rcView.bottom)
	{
		m_rcView = rcView;
		SetWindowPos (hwnd, HWND_BOTTOM, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
		if (m_initParam.pOSMPPlayer != NULL)
		{
			m_initParam.pOSMPPlayer->SetView(m_hWndView);
			GetClientRect(m_hWndView,&m_rcView);
			m_initParam.pOSMPPlayer->updateViewRegion(m_rcView);
		}

		::InvalidateRect(hwnd,NULL,TRUE);

		return true;
	}
	else if(stat == VOOSMP_STATUS_PAUSED)
	{
		if (m_initParam.pOSMPPlayer)
		{
			m_initParam.pOSMPPlayer->Redraw();
		}

		if (!IsFullScreen())
			::PostMessage(m_hWndView, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(50, 50));

		return true;
	} else if (stat == VOOSMP_STATUS_RUNNING) {
	
		if (!IsFullScreen()/* && m_initParam.nBrowserType == 1*/)
			::PostMessage(m_hWndView, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(1, 1));
	}

	return false;
}

void CPlugInUIWnd::cbHandleEvent(VOCBMSG eventInfo)
{
  switch (eventInfo.nID)
  {
  case VOOSMP_SRC_CB_Adaptive_Streaming_Info:
    {
      if(eventInfo.nValue1 == VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_LIVESEEKABLE)
      {
        m_bEnableLiveSeek = TRUE;
        UpdateSeekEnableStatus();
      }
    }
    break;
  case VOOSMP_CB_VideoStartBuff:
  case VOOSMP_CB_AudioStartBuff:
    {
        VOLOGI("start buffering");
		m_bBuffering = TRUE;
		m_bBufferBegin = m_bBuffering ? FALSE : TRUE;
	}
    break;
  case VOOSMP_CB_VideoStopBuff:
  case VOOSMP_CB_AudioStopBuff:     
      {
          VOLOGI("end buffering");
          m_bBuffering = FALSE;
          if(m_bVideoRenderStart==FALSE)
              EraseBackGround(m_hdc);

          //m_semAnimation.Down();
      }
      break;	
  case VOOSMP_CB_PlayComplete:
      {
          {
              voCAutoLock lock (&m_mtMsgQueue);
              m_msgQueue.clearAll();
          }
          m_bVideoRenderStart = FALSE;
          if(IsFullScreen())
              ShowFullScreen();
      }
      break;
  case VOOSMP_CB_CodecNotSupport:
  case VOOSMP_CB_LicenseFailed:
  case VOOSMP_SRC_CB_Adaptive_Streaming_Error:
    {
        if(IsFullScreen())
            ShowFullScreen();
    }
    break;
  case VOOSMP_SRC_CB_Open_Finished:
    {
        {
            voCAutoLock lock (&m_mtMsgQueue);
            m_msgQueue.clearAll();
        }
      m_bVideoRenderStart = FALSE;
      m_bPureAudio = FALSE;
      m_bBuffering = FALSE;
    }
    break;
  case VOOSMP_CB_VideoRenderStart:
    {
        m_bVideoRenderStart = TRUE;
        VOLOGI("[Video] gonna to be rendered @ %d", voOS_GetSysTime());
        ControlFullBtn(TRUE);
        KillTimer(m_hWndView, TIMER_HIDE_CONTROLS);
        SetTimer (m_hWndView, TIMER_HIDE_CONTROLS, 2000, NULL);
    }
    break;
  case VOOSMP_CB_AudioRenderStart:
      {
          VOLOGI("[Audio] gonna to be rendered @ %d", voOS_GetSysTime());
          ControlFullBtn(TRUE);
      }
      break;
  case VOOSMP_CB_SubtitleRenderStart:
      {
          VOLOGI("[Subtitle] gonna to be rendered @ %d", voOS_GetSysTime());
          ControlFullBtn(TRUE);
      }
      break;
  case VOOSMP_CB_MediaTypeChanged:
    {
      VOOSMP_AVAILABLE_TRACK_TYPE trackType = (VOOSMP_AVAILABLE_TRACK_TYPE)(eventInfo.nValue1);
      if (trackType == VOOSMP_AVAILABLE_PUREAUDIO)
      {
        m_bPureAudio = TRUE;
        EraseBackGround(m_hdc);
        m_bVideoRenderStart = FALSE;
      }
      else
        m_bPureAudio = FALSE;
    }
    break;
  default:
    break;
  }

  return;
}

void CPlugInUIWnd::procOnDispalyChange(WPARAM wParam, LPARAM lParam)
{
  if(IsFullScreen())
  {
    //sometimes, change monitor IE will show on the top.
    SetWindowPos (m_hWndView, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
    SetWindowPos (m_hWndView, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);

    //m_bFullScreen = FALSE;
    ShowFullScreen();//call two times, make sure change display, fullscreen can show right.

    ShowFullScreen();
  }
}

void CPlugInUIWnd::procOnTimer(WPARAM wParam, LPARAM lParam)
{
	if (wParam == TIMER_UPDATE_CONTROL)
	{
		if (IsControlsVisible())
		{
			UpdateControlSlide();
			UpdateVolumeSlide();

			//no nessisary to update too fast
			static int iLoop = 0;
			if(iLoop%10==0)
			{
				UpdatePlayPauseButton();
			}
			iLoop++;
		}
	}
	else if (wParam == TIMER_HIDE_CONTROLS && false==m_bPopupMenu)
	{
		if ((GetTickCount()-m_dwTickLastMouseMove)>=m_dwHideButtonTime && (GetTickCount()-m_dwTickLastKeyEvent)>=m_dwHideButtonTime )
		{
			if (GetKeyState(VK_LBUTTON)<0)
			{
				return;
			}

			if (m_pFullscreenBtn) {
				m_pFullscreenBtn->showButton(1, false);
				m_pFullscreenBtn->showButton(2, false);
			}

	//		traceMouseMove();//do not track mouse here to fix bug: https://sh.visualon.com/node/35173

			if (m_bFullScreen) {
				if (/*m_bActive ||*/ IsWndTop()) {//fix bug: https://sh.visualon.com/node/33619
					ShowMouseCursor(FALSE);
				} else {
					ShowMouseCursor(TRUE);
				}
			}
		}
	}
}

void CPlugInUIWnd::procOnLButtonDown(WPARAM wParam, LPARAM lParam)
{
    VOLOGI("WM_LBUTTONDOWN");

    if(lParam != MAKELPARAM(10, 10))//if lParam is MAKELPARAM(10, 10), this message is send by plugin self.
    {
        m_dwTickLastMouseMove = GetTickCount();

        {
            //can not get WM_LBUTTONDBLCLK, but get two times of WM_LBUTTONDOWN
            DWORD dwNewClickTime = GetTickCount();
            if ( (dwNewClickTime-m_dwLastClickLButtonTime)<=GetDoubleClickTime())
            {
                m_dwLastClickLButtonTime = 0;
				::PostMessage(m_hWndView,WM_LBUTTONDBLCLK,0,0);
                return;
            }
            m_dwLastClickLButtonTime = dwNewClickTime;
        }
    }

	m_bPopupMenu = FALSE;

	//fix bug: https://sh.visualon.com/node/27193 (press "Windows" key, popup start bar, then press esc can not exit from fullscreen)
	//bool bConVisible = IsControlsVisible();
	SetWndTopmostIfFullScreen();

	if (!m_pFullscreenBtn)
		return;

	if (m_bFullScreen) {
		m_pFullscreenBtn->showButton(1, false);
		m_pFullscreenBtn->showButton(2);
		m_bControlbarShow = TRUE;

		KillTimer(m_hWndView, TIMER_HIDE_CONTROLS);
		SetTimer(m_hWndView, TIMER_HIDE_CONTROLS, 2000, NULL);
	} else {

		RECT cr;
		GetWindowRect(m_hBrowserContextWnd, &cr);
		VOLOGI("IE content rect: l = %d, t = %d, r = %d, b = %d, w x h = %d x %d", cr.left, cr.top, cr.right, cr.bottom, cr.right - cr.left, cr.bottom - cr.top);

		RECT r;
		GetWindowRect(m_hWndView, &r);
		RECT br = r;
		getNormalFullBtnRect(br);
		VOLOGI("cr: l = %d, t = %d, btn rect top = %d", cr.left, cr.top, br.top);
		if (r.right < cr.right && r.bottom < cr.bottom && cr.top < br.top - 10 && br.top > 0) {

			m_pFullscreenBtn->showButton(2, false);

			if (m_bShowFullBtn) {
				m_pFullscreenBtn->showButton(1);
				m_bControlbarShow = TRUE;

				traceMouseMove(); 
			}
		}
	}

	ShowMouseCursor(TRUE);
}

void CPlugInUIWnd::procOnMouseMove(WPARAM wParam, LPARAM lParam)
{
	//some pc, the mouse do not move, but the plugin can receive the mouse move message.
	{
		POINT pt;
		GetCursorPos(&pt);
		if (pt.x == m_ptLastMousePos.x && pt.y==m_ptLastMousePos.y)
		{
			return;
		}
		m_ptLastMousePos.x = pt.x;
		m_ptLastMousePos.y = pt.y;
	}

	if( (GetTickCount()-m_dwTickLastMouseMove)>1500)
	{
		m_dwTickLastMouseMove = GetTickCount();
		return;
	}

	if (m_bFullScreen && !IsWndTop()) {
		traceMouseMove();
		return;
	}

	ShowMouseCursor(TRUE);

	m_dwTickLastMouseMove = GetTickCount();

	if (m_bFullScreen) {
		if (m_pFullscreenBtn) {
			m_pFullscreenBtn->showButton(2);
			m_bControlbarShow = TRUE;
		}
		return;
	}

	// if hovering on full button, do nothing, either
	RECT vr, br;
	GetClientRect(m_hWndView, &vr);
	br = vr;
	getNormalFullBtnRect(br);

	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);

	POINT pt = {x, y};
	if (PtInRect(&br, pt)) {
		return;
	}

	// if the button's position is larger than browser window's right-bottom corner, do nothing
	RECT cr;
	GetWindowRect(m_hBrowserContextWnd, &cr);
	//VOLOGI("IE content rect: l = %d, t = %d, r = %d, b = %d, w x h = %d x %d", cr.left, cr.top, cr.right, cr.bottom, cr.right - cr.left, cr.bottom - cr.top);

	GetWindowRect(m_hWndView, &vr);
	br = vr;
	getNormalFullBtnRect(br);
	//VOLOGI("cr.top = %d, br.top = %d, distance: %d", cr.top, br.top, br.top - cr.top);
	/*VOLOGI("video rect: {%d %d %d %d}, cr: {%d %d %d %d}, br: { %d, %d, %d, %d}",
		vr.left, vr.top, vr.right, vr.bottom, cr.left,cr.top,cr.right,cr.bottom, br.left, br.top, br.right, br.bottom);*/
	//VOLOGI("cr.top = %d, br.top = %d", cr.top, br.top);
	if (vr.right > cr.right || vr.bottom > cr.bottom || br.top <= cr.top + 10 || br.top < 0)
		return;

	if (!m_bShowFullBtn)
		return;

	if (!m_bMouseTracking) {
		traceMouseMove();
	}

	if (m_pFullscreenBtn) {
		m_pFullscreenBtn->showButton(1);
		m_bControlbarShow = TRUE;
	}
}

void CPlugInUIWnd::procOnKeyDown(WPARAM wParam, LPARAM lParam)
{
  m_dwTickLastKeyEvent = GetTickCount();
  if(IsControlsVisible()==FALSE)
  {
    ShowControls(true);
    ShowMouseCursor(TRUE);
  }

  if (wParam == VK_ESCAPE )
  {
    if (IsFullScreen ())
      ShowFullScreen ();
  }
  else if(wParam == VK_LWIN || wParam == VK_RWIN)
  {
      if(m_initParam.pOSMPPlayer && m_initParam.pOSMPPlayer->IsWin8OS())
      {
          if (IsFullScreen ())
              ShowFullScreen ();
      }
  }
}

void CPlugInUIWnd::procOnSize(WPARAM wParam, LPARAM lParam)
{
  // david 2013-07-03 for distinguish IE from Chrome && FF
  //			if (!m_initParam.bIE) //if do not do this ,IE also has bugs: https://sh.visualon.com/node/29728 https://sh.visualon.com/node/29776
	VOLOGI("resizing.....................full: %d", m_bFullScreen);
	RECT rcView;
	if (IsFullScreen()) {
		rcView = GetFullScreenRect();				
	} else {
		GetClientRect(m_hWndParent, &rcView);
	}
	SetWindowPos (m_hWndView, HWND_TOP, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, SWP_SHOWWINDOW);
	if (m_initParam.pOSMPPlayer != NULL)
	{
		GetClientRect(m_hWndView, &m_rcView);
		m_initParam.pOSMPPlayer->updateViewRegion(m_rcView);
	}

	::InvalidateRect(m_hWndView,NULL,TRUE);

	int nWidth = LOWORD(lParam); 
	if (nWidth)
	{
		int nHeight = HIWORD(lParam);
		if (m_pControlWindow)
		{
			m_pControlWindow->resize(nWidth, nHeight);
		}  

		// this part should be processed by WM_PAINT, so mark off this part.
		//if (m_pFullscreenBtn)
		//{
		//	m_pFullscreenBtn->UpdatePosition();
		//}
	}
	VOLOGI("resized!!!!!!!!!!!!!!!!!");
}

void CPlugInUIWnd::procOnAvtivateApp(WPARAM wParam, LPARAM lParam)
{
	VOLOGI("WM_ACTIVATEAPP wParam %d, lParam %d", wParam, lParam);
	BOOL bActived = (BOOL)(wParam);
	if (m_bFullScreen)
	{
		if (bActived)//fix bug:https://sh.visualon.com/node/33619
		{
			VOLOGI("full be actived");
			m_bActive = TRUE;
			SetWndTopmostIfFullScreen();
			SetTimer (m_hWndView, TIMER_HIDE_CONTROLS, 2000, NULL);
			if (m_pFullscreenBtn)
				m_pFullscreenBtn->showButton(2);
		}
		else
		{
			m_bActive = FALSE;
			VOLOGI("full be deactived");
            m_bControlbarShow = FALSE;
			if (m_pFullscreenBtn)
				m_pFullscreenBtn->showButton(2, false);

			ShowMouseCursor(TRUE);
			KillTimer(m_hWndView,TIMER_HIDE_CONTROLS);
		}
	} 
}

LRESULT CPlugInUIWnd::OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (uMsg)
	{
	case WM_DESTROY:
	case WM_CLOSE:
		{
			if(IsFullScreen())
			{
				ShowFullScreen();
				return 0;
			}
		}
		break;
	case WM_USERDEFNOTIFY:
		{
            VOCBMSG msg;
            int ret = VOQU_OK;
            {
                voCAutoLock lock (&m_mtMsgQueue);
                ret = m_msgQueue.dequeue(&msg);
            }

            if(VOQU_ERR != ret)
            {
                VOLOGI("receive WM_USERDEFNOTIFY 0x%08x", msg.nID);
			    cbHandleEvent(msg);
            }
		}
		break;
	case WM_UPDATE_SCREEN:
		{
            if(m_bBuffering==FALSE)
                return 0;

			if(m_initParam.pOSMPPlayer)
            {
                int iRet = m_initParam.pOSMPPlayer->Redraw();
            }

            if (m_bVideoRenderStart == FALSE)
            {
                VOLOGI("redraw failed!");
                ShowWaitCursor(NULL);
            }
		}
		return 0;
	case WM_BUFFERING_TIMEOUT:
		{
			VOLOGI("Buffering time out, exit fullscreen");
			if (IsFullScreen())
			{
				ShowFullScreen();
			}
		}
		break;
	case WM_COMMAND:
		{
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			OnSelectTrack(wmId);
		}
		break;
	case WM_ERASEBKGND:
        {
            //VOLOGI("recevie WM_ERASEBKGND");
            if (m_bPureAudio || m_bVideoRenderStart == FALSE)
            {
                if(FALSE == m_bBuffering)
                    DefWindowProc(hwnd, uMsg, wParam, lParam);
            }
        }
		return 1;//make the system do not erase the background
	case WM_PAINT:
		{
            VOLOGI("recevie WM_PAINT");
            DefWindowProc(hwnd, uMsg, wParam, lParam);
            if (m_initParam.pOSMPPlayer)
            {
                int iRet = m_initParam.pOSMPPlayer->Redraw();
 //               m_bVideoRedrawSuccess = iRet==0;
            }

            if (!m_bFullScreen && m_pFullscreenBtn)
            {
                if (GetKeyState(VK_LBUTTON)<0)//https://sh.visualon.com/node/34054
                {
                    m_pFullscreenBtn->showButton(1, false);
                    m_bControlbarShow = FALSE;
                    return 0;
                }

				POINT pt;
				GetCursorPos(&pt);

				RECT vr;
				GetWindowRect(m_hWndView, &vr);
				RECT br = vr;
				getNormalFullBtnRect(br);

				RECT cr;
				GetWindowRect(m_hBrowserContextWnd, &cr);
				
				VOLOGI("vr {r %d, b %d}, cr {t %d, r %d, b %d}, br.top = %d", vr.right, vr.bottom, cr.top, cr.right, cr.bottom, br.top);
				if ( m_bShowFullBtn && PtInRect(&vr, pt) && vr.right < cr.right && vr.bottom < cr.bottom && cr.top < br.top - 10 && br.top > 0) {
					m_pFullscreenBtn->showButton(1);
					m_bControlbarShow = TRUE;
				} else {
					m_pFullscreenBtn->showButton(1, false);
					m_bControlbarShow = FALSE;
				}
            }

            if(m_bVideoRenderStart==FALSE && m_bBuffering)
            {
                ShowWaitCursor(NULL);
            }
			return 0;
		}
		break;
	case WM_DISPLAYCHANGE:
		{
            VOLOGI("WM_DISPLAYCHANGE");
			procOnDispalyChange(wParam,lParam);
		}
		break;
        break;
	case WM_TIMER:
		{
			procOnTimer(wParam,lParam);
		}
		break;
	case WM_LBUTTONDOWN:
		{
			procOnLButtonDown(wParam,lParam);
	
		}
		break;

	case WM_LBUTTONDBLCLK: 
		{
			VOLOGI("receive WM_LBUTTONDBLCLK. %lu", GetTickCount());
			//if (!m_bFullScreen && !m_bShowFullBtn)
			//	break;

			ShowFullScreen ();
		}
		break;

	case WM_ACTIVATEAPP: 
		{
			procOnAvtivateApp(wParam,lParam);
		}
		break;

	case WM_ACTIVATE:
		{
			VOLOGI("active: %d", LOWORD(wParam));
			ShowMouseCursor(TRUE);
		}
		break;
	case WM_MOUSEMOVE:
		{
			procOnMouseMove(wParam,lParam);
		}
		break;

	case WM_MOUSELEAVE:
		{
			if (m_bFullScreen)
				break;

			m_bMouseTracking = FALSE;
			VOLOGI("mouse left the video region.");
			POINT pt;
			GetCursorPos(&pt);
			RECT r;
			GetClientRect(m_hWndView, &r);
			getNormalFullBtnRect(r);

			ScreenToClient(m_hWndView, &pt);

			if (PtInRect(&r, pt)) {
				//VOLOGI("parent rect: l = %d, t = %d, r = %d, b = %d, point = (%d x %d)", r.left, r.top, r.right, r.bottom, pt.x, pt.y);
				break;
			}

			if (m_pFullscreenBtn) 
				m_pFullscreenBtn->showButton(1, false);

			m_bControlbarShow = FALSE;

            ::SetFocus(m_hBrowserContextWnd);
		}
		break;

	case WM_KEYDOWN:
		{
			procOnKeyDown(wParam,lParam);
		}
		break;

	case WM_SYSKEYDOWN:
		{
			VOLOGI("key: 0x%02x, 0x%02x", wParam, lParam);
            return 0;
		}
		break;
    case WM_SYSCOMMAND:
        {
            VOLOGI("key: 0x%02x, 0x%02x", wParam, lParam);
            switch(wParam)
            {
            case SC_CLOSE:
                if (IsFullScreen())
                {
                    ShowFullScreen();
                }
                VOLOGI("SC_CLOSE");
                return 0;
            case SC_SCREENSAVE://fix bug:https://sh.visualon.com/node/33908 [New windowed]Screen saver cannot work when pause in full screen mode
                break;
              default:
            return 0;
        }
        }
        break;

	case WM_SIZE:
		{
			procOnSize(wParam,lParam);
		}
		break;
	case WM_KILLFOCUS:
		{
			ShowMouseCursor(TRUE);
		}
		break;

	default:
		{
			//VOLOGI("unknow event: 0x%08x", uMsg);
		}
		break;
	}

	return	DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CPlugInUIWnd::UpdateControlSlide()
{
  if (m_initParam.pOSMPPlayer != NULL)
  {
    int nDuration = 0;
    int nCurPos = 0;
    int nPos = 0;
    nDuration = GetDuration ();
    nCurPos = GetPos ();
    if (nDuration > 0)
      nPos = nCurPos * 1000/ nDuration;

    if(this->m_pControlWindow)
    {
      if(m_nSkipTimes>0)
        m_nSkipTimes--;
      else
      {
        int nPos2 = 0;
        m_pControlWindow->GetSliderPos(0,nPos2);
        if(nPos2!=nPos)
          this->m_pControlWindow->SetSliderPos(0,nPos);
      }
    }
  }
}

void CPlugInUIWnd::UpdateVolumeSlide()
{
  if (m_initParam.pOSMPPlayer != NULL)
  {
    int nVolume = 0;
    m_initParam.pOSMPPlayer->GetParam(VOOSMP_PID_AUDIO_VOLUME, &nVolume);
    if(this->m_pControlWindow)
    {
      int nPos = 0;
      m_pControlWindow->GetSliderPos(1,nPos);
      if(nPos != nVolume)
        this->m_pControlWindow->SetSliderPos(1,nVolume);
    }
  }
}

int CPlugInUIWnd::GetMenuShowTextByIndex(OUT LPSTR szMenuText, IN VOOSMP_SOURCE_STREAMTYPE sType, IN int nIndex)
{
  VOLOGI("Begin");

  CvoOnStreamMP * pOSMPPlayer = m_initParam.pOSMPPlayer;
  if (NULL == pOSMPPlayer)
  {
    return VOOSMP_ERR_Pointer;
  }
  VOOSMP_SRC_TRACK_PROPERTY *pProperty = NULL;
  int nRC = VOOSMP_ERR_None;
  
  switch(sType)
  {
  case VOOSMP_SS_VIDEO:
    pOSMPPlayer->GetVideoProperty(nIndex,&pProperty);
    break;
  case VOOSMP_SS_AUDIO:
    pOSMPPlayer->GetAudioProperty(nIndex,&pProperty);
    break;
  case VOOSMP_SS_SUBTITLE:
    pOSMPPlayer->GetSubtitleProperty(nIndex,&pProperty);
    break;
  default:
    break;
  }  

  if (NULL == pProperty)
  {
    strcpy(szMenuText,"Property error!");
    return VOOSMP_ERR_Pointer;
  }

  for (int j=0; pProperty && j<pProperty->nPropertyCount; ++j)
  {
    VOOSMP_SRC_TRACK_ITEM_PROPERTY* pItemProperties = pProperty->ppItemProperties[j];

    char szProperty[MAX_PATH] = "";
    strcpy(szProperty,pItemProperties->pszProperty);
    if (strstr(pItemProperties->szKey, ("language")))
    {
      for (int m=0; m<strlen(pItemProperties->pszProperty); ++m)
      {
        if (pItemProperties->pszProperty[m] == '-')
        {
          szProperty[m] = '\0';
          break;
        }

        szProperty[m] = pItemProperties->pszProperty[m];
      }
    }

    strcat(szMenuText,szProperty);

    if (j<pProperty->nPropertyCount-1)
    {
      strcat(szMenuText," | ");
    }                                        
  }

  VOLOGI("End");

  return VOOSMP_ERR_None;
}

LONG CPlugInUIWnd::GetDuration()
{
  if (m_initParam.pOSMPPlayer==NULL)
  {
    return 0;
  }

  LONG lDuration = 0;
  if (m_initParam.pOSMPPlayer->IsLiveStreaming())
  {
    lDuration = m_initParam.pOSMPPlayer->GetMaxPosition() - m_initParam.pOSMPPlayer->GetMinPosition();
  }
  else
  {
    lDuration =  m_initParam.pOSMPPlayer->GetDuration();
  }

  return lDuration;
}

LONG CPlugInUIWnd::GetPos()
{
  if (m_initParam.pOSMPPlayer==NULL)
  {
    return 0;
  }

  LONG lPos = m_initParam.pOSMPPlayer->GetPos();
  if (m_initParam.pOSMPPlayer->IsLiveStreaming())
  {
    lPos = lPos - m_initParam.pOSMPPlayer->GetMinPosition();

    if (lPos<0)
    {
      VOLOGI("curPos %d is samller than minPos %d ", lPos, m_initParam.pOSMPPlayer->GetMinPosition());
      lPos = 0;
    }
  }

  return lPos;
}

void CPlugInUIWnd::ShowErrorMsg(VOCBMSG cbEventInfo)
{
    return;

  if (m_initParam.pOSMPPlayer)
  {
    m_initParam.pOSMPPlayer->Stop();
  }

  TCHAR szMsg[MAX_PATH] = _T("");
  switch(cbEventInfo.nID)
  {
  case VOOSMP_CB_CodecNotSupport:
    {
      _tcscpy(szMsg, _T("Cannot support the codec."));
    }
    break;
  case VOOSMP_SRC_CB_Adaptive_Stream_Warning:
    {
      if(cbEventInfo.nValue1 == VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_DOWNLOADERROR ||
        cbEventInfo.nValue1 == VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_PLAYLIST_DOWNLOADERROR )
      {
        _tcscpy(szMsg, _T("Streaming download failed."));
      }
      else if(cbEventInfo.nValue1 == VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_DRMERROR)
      {
        _tcscpy(szMsg, _T("DRM error."));
      }
    }    
    break;
  case VOOSMP_SRC_CB_Adaptive_Streaming_Error:
    {
      switch(cbEventInfo.nValue1)
      {
      case VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_PLAYLIST_PARSEFAIL:
        _tcscpy(szMsg, _T("Playlist parse failed."));
        break;
      case VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_PLAYLIST_UNSUPPORTED:
        _tcscpy(szMsg, _T("Playlist not supported"));
        break;
      case VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_UNSUPPORTED:
        _tcscpy(szMsg, _T("Stream not supported"));
        break;
      case VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_DOWNLOADFAIL:
        _tcscpy(szMsg, _T("Stream download failed"));
        break;
      case VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_DRMLICENSEERROR:
        _tcscpy(szMsg, _T("Stream DRM error"));
        break;
      default:
        _tcscpy(szMsg, _T("Streaming error."));
        break;
      }
    }
    break;
  case VOOSMP_CB_LicenseFailed:
    {
      _tcscpy(szMsg, _T("License check failed"));
    }
    break;
  default:
    {
      TCHAR szBuf[MAX_PATH] = _T("Error info is: 0X%x");
      _stprintf (szMsg,szBuf, cbEventInfo.nID);
    }
    break;
  }

  RECT rcView;
  GetClientRect (m_hWndView, &rcView);
  SetWindowPos (m_hWndView, HWND_NOTOPMOST, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
  MessageBox(m_hWndView, szMsg,_T("Warning"),MB_OK);
  SetWindowPos (m_hWndView, HWND_TOP, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, 0);

}

void CPlugInUIWnd::UpdateCursorPosition()
{
  RECT		  rect;
  if (IsFullScreen())
	  rect = GetFullScreenRect();
  else
	GetClientRect(m_hWndParent, &rect);
  m_BmpX = rect.left + (rect.right - rect.left)/2 - m_nBmpWidth / 2;
  m_BmpY = rect.top + (rect.bottom - rect.top)/2 - m_nBmpHeight / 2;
}
void CPlugInUIWnd::ShowWaitCursor(VOOSMP_VR_USERCALLBACK_TYPE* pVr)
{
  if (FALSE == m_bBuffering )
  {  
    return;
  }

  HBITMAP	hTempBitmap = NULL;
  HDC hdcTemp = NULL;
  HBITMAP hOld2 = NULL;
  if(pVr==NULL){
	  hTempBitmap = ::CreateCompatibleBitmap(m_hdc,m_rcView.right - m_rcView.left,m_rcView.bottom - m_rcView.top);
	  hdcTemp = ::CreateCompatibleDC(m_hdc);
	  hOld2 = (HBITMAP)SelectObject(hdcTemp, hTempBitmap);
      ::FillRect(hdcTemp, &m_rcView, m_brhBackground);
  }

  HDC hDC = pVr!=NULL ? (HDC)pVr->pDC : hdcTemp;
  RECT * r = pVr!=NULL ? (RECT *)pVr->pRect : &m_rcView;

  RECT rWin;
  GetClientRect(m_hWndView, &rWin);
  int nWidth = r->right - r->left;
  int nHeight = r->bottom - r->top;
  if(m_pGDIPlusimage)
  {//draw now
      Gdiplus::Graphics g(hDC);

      g.TranslateTransform(nWidth/2,nHeight/2);
      g.RotateTransform(m_iBMPIndex*30);
      g.SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQualityBilinear);
      int nW;
      int nH;
      if(m_bFullScreen)
      {
          nW = (int)m_pGDIPlusimageFull->GetWidth();
          if((nW&1)==0)
              nW++;
          nH = (int)m_pGDIPlusimageFull->GetHeight();
          if((nH&1)==0)
              nH++;
          g.DrawImage(m_pGDIPlusimageFull, -(nW)/2, -(nH)/2,nW,nH);
      }
      else
      {
          nW = (int)m_nBmpWidth;
          if((nW&1)==0)
              nW++;
          nH = (int)m_nBmpHeight;
          if((nH&1)==0)
              nH++;
          g.DrawImage(m_pGDIPlusimage, -(nW)/2, -(nH)/2,nW,nH);
      }

  }
  if(pVr==NULL){
	  ::BitBlt(m_hdc, 0,0,nWidth,nHeight,hdcTemp, 0, 0, SRCCOPY);

	  if(hdcTemp){
		  SelectObject(hdcTemp, hOld2);
		  ::DeleteDC(hdcTemp);
	  }
	  if(hTempBitmap)
		  ::DeleteObject(hTempBitmap);
  }
  //when scroll the webpage fast, will call this too often, so control the roll speed.
  if ( (GetTickCount() - m_dwTickCountLastShow) >= 50)
  {
      ++m_iBMPIndex;
      m_dwTickCountLastShow = GetTickCount();
  }
  return;
}


void CPlugInUIWnd::UpdateSeekEnableStatus()
{
  if (m_initParam.pOSMPPlayer==NULL)
  {
    return;
  }

  VOOSMP_SRC_PROGRAM_INFO *pProgramInfo = NULL;
  int nRC = m_initParam.pOSMPPlayer->GetProgramInfo(0, &pProgramInfo);
  if (0!=nRC || pProgramInfo==NULL)
  {
    return;
  }

  if (pProgramInfo->nProgramType == VOOSMP_SRC_PROGRAM_TYPE_LIVE)
  {
    if(m_pControlWindow)
      m_pControlWindow->SetCanSeek(m_bEnableLiveSeek);   
  }
  else
  {
    if(m_pControlWindow)
      m_pControlWindow->SetCanSeek(TRUE); 
  }

  int dur = GetDuration();
  if (m_pControlWindow)
	  m_pControlWindow->SetDuration(dur);
}

VOOSMP_STATUS CPlugInUIWnd::GetPlayerStatus()
{
  VOOSMP_STATUS status = VOOSMP_STATUS_MAX;

  if(m_initParam.pOSMPPlayer)
    m_initParam.pOSMPPlayer->GetParam(VOOSMP_PID_STATUS,&status);

  return status;
}

RECT CPlugInUIWnd::GetFullScreenRect()
{
    if (m_hMonitorFullscreen == NULL)
    {
        RECT rcWindow;
        ::GetWindowRect(m_hWndParent,&rcWindow);
        m_hMonitorFullscreen = MonitorFromRect(&rcWindow,MONITOR_DEFAULTTONULL);
    }

  MONITORINFO monitorInfo;
  memset(&monitorInfo, 0,sizeof(monitorInfo));
  monitorInfo.cbSize = sizeof(MONITORINFO);
  BOOL bRet = GetMonitorInfo(m_hMonitorFullscreen,&monitorInfo);

  // modified to avoid the issue that video window will disappear from taskbar 
  if (!bRet) {
	  monitorInfo.rcMonitor.right = ::GetSystemMetrics (SM_CXSCREEN);
	  monitorInfo.rcMonitor.bottom = ::GetSystemMetrics (SM_CYSCREEN);
  }
  return monitorInfo.rcMonitor;
}

void CPlugInUIWnd::ShowMouseCursor(BOOL bShow)
{
  if (!IsFullScreen())
  {
    bShow = TRUE;
  }

  if (m_bCursorShow == bShow)
  {
    return;
  }

  if (bShow)
  {
    m_bCursorShow = TRUE;
    ::ShowCursor(TRUE);
  }
  else
  {
    m_bCursorShow = FALSE;

    HWND hWnd = GetCapture();
    if (!(hWnd==m_hWndView))
      SetCapture(m_hWndView);
 
    ::ShowCursor(FALSE);

    if (!(hWnd==m_hWndView))
      SetCapture(hWnd);
  }
}

int	CPlugInUIWnd::msgHandlerProc(VO_PTR pParam) {

	if (pParam == NULL)
		return 0;

	CPlugInUIWnd* p = (CPlugInUIWnd*)pParam;
	p->msgHandlerLoop();

	return 0;
}

int	CPlugInUIWnd::msgHandlerLoop() 
{

	while (!m_bExit) {

		if (m_bExit)
			break;

		if (!m_bBuffering) {
            m_dwTickStartBuffering = 0;
			voOS_Sleep(100);
			continue;
		}

        if (m_dwTickStartBuffering==0)
        {
            m_dwTickStartBuffering = GetTickCount();
        }

        if(IsFullScreen())
        {
            if ((GetTickCount()-m_dwTickStartBuffering)>=m_dwWaitBufferingTime)
            {
                ::PostMessage(m_hWndView,WM_BUFFERING_TIMEOUT,0,0);
            }
        }

		//VOLOGI("post WM_UPDATE_SCREEN");
		PostMessage(m_hWndView, WM_UPDATE_SCREEN,0,0);
		voOS_Sleep(50);
	}

	VOLOGI("windowed buffering animation thread exit normally.");
	return 0;
}

void CPlugInUIWnd::getNormalFullBtnRect(RECT& r) {

	r.left = r.right - FULLBTN_NORMAL_OFFSET;
	r.right = r.left + FULLBTN_NORMAL_WIDTH;
	r.top = r.bottom - FULLBTN_NORMAL_OFFSET;
	r.bottom = r.top + FULLBTN_NORMAL_WIDTH;
}

void CPlugInUIWnd::traceMouseMove() {
	TRACKMOUSEEVENT tme;
	tme.cbSize=sizeof(TRACKMOUSEEVENT);
	tme.dwFlags=TME_LEAVE;   
	tme.hwndTrack = m_hWndView;

	if (::TrackMouseEvent (&tme))
		m_bMouseTracking = TRUE;
}

/*
return TRUE; --->discard the message, do nothing when receive this message 
*/
LRESULT CALLBACK Hotkey_Filter(int nCode, WPARAM wParam, LPARAM lParam)
{
	voCAutoLock l(&g_mutSysHook);
    if (g_PluginUI==NULL )
    {
        return 0;
    }

    if (HC_ACTION == nCode)
    {

        //fix bug: https://sh.visualon.com/node/33583
        //Fullscreen mode, press F11 key,will switch to the test page
        if (wParam == VK_F11)
        {
            VOLOGI("F11");
            if (g_PluginUI->IsWndTop())
            {
                VOLOGI("F11, fullscreen top window, return null");
                return TRUE;
            }

            //https://sh.visualon.com/node/34054
            if(FALSE==g_PluginUI->IsFullScreen())
            {
                g_PluginUI->ShowControls(false);
                return FALSE;
            }
        }

        //fix bug: https://sh.visualon.com/node/33468
        if ( (wParam == VK_RWIN || wParam == VK_LWIN) && lParam == 0X015B0001) //WIN+D
        {
            VOLOGI("WIN+D");

            if (g_PluginUI->IsFullScreen())
            {
                g_PluginUI->ShowFullScreen();
                return TRUE;
            }
        }
    }	

    return 0;
}

BOOL Hotkey_Install(DWORD ThreadID)
{
    Hotkey_UnInstall();

    g_hHotKey = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)Hotkey_Filter,
        GetModuleHandle(L"hotkey"), ThreadID);
    if (NULL == g_hHotKey)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL Hotkey_UnInstall()
{
    if(g_hHotKey)
    {
        UnhookWindowsHookEx(g_hHotKey);
    }
    return TRUE;
}

BOOL	CPlugInUIWnd::IsWndTop()
{
    if(!m_bFullScreen)
        return FALSE;

    HWND hForeWnd = GetForegroundWindow();
    HWND hActiveWnd = GetActiveWindow();
    HWND hBtnWnd = NULL;
    if (m_pControlWindow)
    {
        hBtnWnd = m_pControlWindow->GetWindow();
    }
    else if(m_pFullscreenBtn)
    {
        hBtnWnd = m_pFullscreenBtn->GetWindow();
    }

    if( hForeWnd==m_hWndView || hActiveWnd==m_hWndView 
        || hForeWnd==hBtnWnd || hActiveWnd==hBtnWnd )
        return TRUE;

    return FALSE;
}

void CPlugInUIWnd::ControlFullBtn(BOOL bShow)
{
    m_bShowFullBtn = bShow;

    ShowMouseCursor(m_bShowFullBtn);
    if (m_pFullscreenBtn && !m_bFullScreen) {

        RECT vr, cr;
        GetWindowRect(m_hBrowserContextWnd, &cr);
        GetWindowRect(m_hWndView, &vr);

        RECT br = vr;
        getNormalFullBtnRect(br);

        POINT pt;
        GetCursorPos(&pt);
        if (vr.right > cr.right || vr.bottom > cr.bottom || br.top <= cr.top + 10 || br.top < 0 || !PtInRect(&vr, pt)) 
            return;

        if (m_bShowFullBtn)
            m_pFullscreenBtn->showButton(1);
        else
            m_pFullscreenBtn->showButton(1, false);

        traceMouseMove(); 
        m_bControlbarShow = m_bShowFullBtn;
    }
}
