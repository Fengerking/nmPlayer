/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#include "CFullscreenBtn.h"
#include "Resource.h"
#include "CPlugInUIWnd.h"
#include "voLog.h"

extern HINSTANCE		g_hInst;

CFullscreenBtn::CFullscreenBtn(HWND hParent):
     m_hWndParent(hParent)
   , m_hWndNormalBtn(NULL)
   , m_hWndFullBtn(NULL)
   , m_bFullscreen(FALSE)
   , m_pParent(NULL)
   , m_imageEnter(NULL)
{
  CreateNormalBtnWnd();
  CreateFullBtnWnd();
  showButton(1,FALSE);
  showButton(2,FALSE);
}

CFullscreenBtn::~CFullscreenBtn()
{
	SendMessage(m_hWndFullBtn, WM_CLOSE, 0,0);
    DestroyWindow(m_hWndFullBtn);
    m_hWndFullBtn = NULL;
	SendMessage(m_hWndNormalBtn, WM_CLOSE, 0,0);
    DestroyWindow(m_hWndNormalBtn);
    m_hWndNormalBtn = NULL;

	m_imageExit.Destroy();
	if(m_imageEnter)
	{
		m_imageEnter->Destroy();
		delete m_imageEnter;
	}
}

void CFullscreenBtn::InitCImage(CImage* pImage, int nID, Gdiplus::Bitmap** pBitmap, bool bComputeData)
{
	if(pImage == NULL)
		return;
	TCHAR ch[2048];
	memset(ch,0,2048*sizeof(TCHAR));
	TCHAR * pch = NULL;
	if(g_hInst)
	{
		::GetModuleFileName(g_hInst,ch,2048);
		VO_PTCHAR pPos = vostrrchr(ch, _T('/'));
		if (pPos == NULL)
			pPos = vostrrchr(ch, _T('\\'));
		VO_S32 nPos = pPos - ch;
		ch[nPos+1] = _T('\0');
		_tcscat(ch,_T("resource\\"));
		switch(nID){
			case IDB_EXIT_FULLSCREEN:
				_tcscat(ch,_T("exit-fullscreen.png"));
				break;
			case IDB_ENTER_FULLSCREEN:
				_tcscat(ch,_T("enter-fullscreen.png"));
				break;
			case IDB_PNG_BUFFERING:
				_tcscat(ch,_T("buffering-inline.png"));
				break;
			case IDB_PNG_BUFFERING+1000:
				_tcscat(ch,_T("buffering-fullscreen.png"));
				break;
			default:
				_tcscpy(ch,_T(""));
				break;
		}
		HANDLE h = ::CreateFile(ch,GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, (DWORD) 0, NULL);
		if(h !=INVALID_HANDLE_VALUE){
			pch = (TCHAR*)ch;
			::CloseHandle(h);
		}
	}

	//if(pch == NULL){
	//	HMODULE hInstance = g_hInst;
	//	HRSRC hRsrc = ::FindResource(hInstance, MAKEINTRESOURCE(nID), _T("PNG"));//IDB_EXIT_FULLSCREEN
	//	 ATLASSERT(hRsrc != NULL);
	// 
	//	DWORD dwSize = ::SizeofResource(hInstance, hRsrc);
	//	 LPBYTE lpRsrc = (LPBYTE)::LoadResource(hInstance, hRsrc);
	//	 ATLASSERT(lpRsrc != NULL);
	// 
	//	HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, dwSize);
	//	 LPBYTE pMem = (LPBYTE)::GlobalLock(hMem);
	//	 memcpy(pMem, lpRsrc, dwSize);
	//	 IStream* pStream = NULL;
	//	 ::CreateStreamOnHGlobal( hMem, FALSE, &pStream);
	// 
	//	 pImage->Load(pStream);
	//	 if(pBitmap){
	//		 if(*pBitmap == NULL)
	//			*pBitmap = new  Gdiplus::Bitmap(pStream);
	//	 }
	//	 pStream->Release();
	//	 ::GlobalUnlock(hMem);
	//	 GlobalFree(hMem);
	//	 ::FreeResource(lpRsrc);
	//}
	//else
	if(pch)
	{
		pImage->Load(pch);
		 if(pBitmap){
			 if(*pBitmap == NULL)
				*pBitmap = new  Gdiplus::Bitmap(pch);
		 }
	}
	if(pImage->GetBPP() == 32 && bComputeData)
	{
		for(int i =0;i<pImage->GetHeight();i++){
			BYTE* p = (BYTE*)pImage->GetPixelAddress(0,i);
			for(int i =0;i<pImage->GetWidth();i++){
				if(p[3] == 0)
				{
					p[0] = 0;
					p[1] = 0;
					p[2] = 0;
				}
				else
				{
					p[0]= ((int)p[0])*p[3]/255;
					p[1]= ((int)p[1])*p[3]/255;
					p[2]= ((int)p[2])*p[3]/255;
				}
				p+=4;
			}
		}
	}
 
}
HWND CFullscreenBtn::GetWindow()
{
  return m_bFullscreen ? m_hWndFullBtn : m_hWndNormalBtn;
}

void CFullscreenBtn::Show(bool bShow)
{
  if (m_bFullscreen)
  {
      showButton(2,bShow);
  }
  else
  {
      showButton(1,bShow);
  }
}

void CFullscreenBtn::showButton(const int index, const bool bShow) {

   // VOLOGI("index: %d, bshow %d", index, bShow);

	if (index == 2)
	{
		if(bShow)
		{
			//::ShowWindow(m_hWndFullBtn, SW_SHOW);

			RECT rcView;
			GetWindowRect (m_hWndParent, &rcView);
			//VOLOGI("b = %d, r = %d", rcView.bottom, rcView.right);
			//SetWindowPos (m_hWndFullBtn, HWND_TOP, rcView.right-100, rcView.bottom-100, 65,  65, SWP_NOSIZE|SWP_NOMOVE);
			SetWindowPos (m_hWndFullBtn, NULL, rcView.right-100, rcView.bottom-100, 65,  65, 0);
			SetWindowPos (m_hWndFullBtn, HWND_TOP, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
		}
		else
		{
			SetWindowPos (m_hWndFullBtn, HWND_TOP, 0, 0, 0, 0, SWP_HIDEWINDOW|SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
		}

	}
	else if (index == 1)
	{
		RECT rcView;
		GetWindowRect (m_hWndParent, &rcView);
		//POINT pt = { rcView.right-FULLBTN_NORMAL_OFFSET, rcView.bottom-FULLBTN_NORMAL_OFFSET };

        int nCmd = bShow ? SWP_SHOWWINDOW : SWP_HIDEWINDOW;

		SetWindowPos (m_hWndNormalBtn, HWND_TOP, rcView.right-FULLBTN_NORMAL_OFFSET, rcView.bottom-FULLBTN_NORMAL_OFFSET, 
            FULLBTN_NORMAL_WIDTH,  FULLBTN_NORMAL_WIDTH, nCmd|SWP_NOACTIVATE);
	}
}

void CFullscreenBtn::SetFullScreen(bool bFullscreen)
{
  m_bFullscreen = bFullscreen;
}

bool CFullscreenBtn::CreateNormalBtnWnd()
{
	if (m_hWndNormalBtn != NULL)
		return true;

	TCHAR szClassName[MAX_PATH] = _T("voPluginEnterFullscreen");
	TCHAR szWindowName[MAX_PATH] = _T("voPluginEnterFullscreen");

	WNDCLASS wcex;
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)ViewWindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)::CreateSolidBrush (RGB (180, 180, 180));
	wcex.lpszMenuName	= (LPCTSTR)NULL;
	wcex.lpszClassName	= szClassName;

	RegisterClass(&wcex);

	//m_hWndNormalBtn = CreateWindowEx(0, szClassName, szWindowName, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, m_hWndParent, NULL, g_hInst, NULL);
	m_hWndNormalBtn = CreateWindowEx(WS_EX_LAYERED|(WS_EX_TOOLWINDOW)&(~WS_EX_APPWINDOW), szClassName, szWindowName, WS_POPUP  | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		 CW_USEDEFAULT, CW_USEDEFAULT, FULLBTN_NORMAL_WIDTH, FULLBTN_NORMAL_WIDTH, NULL, NULL, g_hInst, NULL);

	if(!m_hWndNormalBtn)
		return false;

	SetWindowLong(m_hWndNormalBtn, GWL_USERDATA, (LONG)this);

	HBITMAP hBmp = NULL;
	if(m_imageEnter == NULL)
	{
		m_imageEnter = new CImage();
		InitCImage(m_imageEnter, IDB_ENTER_FULLSCREEN);
	}

	if (m_imageEnter)
		hBmp = m_imageEnter->Detach();

	// get the bitmap dimensions
	BITMAP bmpInfo;
	memset (&bmpInfo, 0, sizeof (BITMAP));
	::GetObject (hBmp, sizeof (BITMAP), &bmpInfo);
	if (bmpInfo.bmBitsPixel != 32)
	{
		m_imageEnter->Attach(hBmp);
		return false;
	}

	LPBYTE pBitmapBits = NULL;

	HDC dcScreen = ::GetDC(NULL);
	HDC dcMemory = ::CreateCompatibleDC(dcScreen);

	HBITMAP pOldBitmap= (HBITMAP)SelectObject(dcMemory, hBmp);

	// get the window rectangle (we are only interested in the top left position)
	RECT r;
	GetWindowRect(m_hWndParent, &r);
	VOLOGI("parent rect: l = %d, t = %d, r = %d, b = %d, w x h = %d x %d", r.left, r.top, r.right, r.bottom, r.right - r.left, r.bottom - r.top);
	// calculate the new window position/size based on the bitmap size
	POINT pt;
	pt.x = (r.right - FULLBTN_NORMAL_OFFSET);
	pt.y = (r.bottom - FULLBTN_NORMAL_OFFSET);
	ClientToScreen(m_hWndParent, &pt);
	SIZE szWindow;
	szWindow.cx = bmpInfo.bmWidth;
	szWindow.cy = bmpInfo.bmHeight;

	// setup the blend function
	BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	POINT ptSrc; // start point of the copy from dcMemory to dcScreen
	ptSrc.x = 0;
	ptSrc.y = 0;


	UpdateLayeredWindow(m_hWndNormalBtn, dcScreen, &pt, &szWindow, dcMemory, &ptSrc, 0, &blendPixelFunction, ULW_ALPHA);

	::ReleaseDC(NULL,dcScreen);
	::DeleteDC(dcMemory);
	//::DeleteObject();
	SelectObject(dcMemory, pOldBitmap);
	if(pBitmapBits)
		delete [] pBitmapBits;

	m_imageEnter->Attach(hBmp);
	return true;
}

bool CFullscreenBtn::CreateFullBtnWnd()
{
  if (m_hWndFullBtn != NULL)
    return true;

  TCHAR szClassName[MAX_PATH] = _T("voPluginEixtFullscreen");
  TCHAR szWindowName[MAX_PATH] = _T("voPluginEixtFullscreen");

  WNDCLASS wcex;
  wcex.style			= CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc	= (WNDPROC)ViewWindowProc;
  wcex.cbClsExtra		= 0;
  wcex.cbWndExtra		= 0;
  wcex.hInstance		= g_hInst;
  wcex.hIcon			= NULL;
  wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground	= (HBRUSH)::CreateSolidBrush (RGB (180, 180, 180));
  wcex.lpszMenuName	= (LPCTSTR)NULL;
  wcex.lpszClassName	= szClassName;

  RegisterClass(&wcex);

  m_hWndFullBtn = CreateWindowEx(WS_EX_LAYERED|(WS_EX_TOOLWINDOW)&(~WS_EX_APPWINDOW), szClassName, szWindowName, WS_POPUP  | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
    CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, g_hInst, NULL);

  if (m_hWndFullBtn == NULL)
    return false;

  //::SetLayeredWindowAttributes(m_hWndFullBtn, RGB(255,255,255), 200,LWA_ALPHA);

  LONG lRC = SetWindowLong (m_hWndFullBtn, GWL_USERDATA, (LONG)this);

  HBITMAP hBmp = m_imageExit.Detach();
  if(hBmp==NULL){
	    InitCImage(&m_imageExit, IDB_EXIT_FULLSCREEN);
		//HMODULE hInstance = g_hInst;
		//HRSRC hRsrc = ::FindResource(hInstance, MAKEINTRESOURCE(IDB_EXIT_FULLSCREEN), _T("PNG"));
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
		// m_imageExit.Load(pStream);
		// pStream->Release();
		// ::GlobalUnlock(hMem);
		// GlobalFree(hMem);
		// ::FreeResource(lpRsrc);
		////m_image.Load(_T("d:\\seek.png"));
		// if(m_imageExit.GetBPP() == 32)
		//{
		//	for(int i =0;i<m_imageExit.GetHeight();i++){
		//		BYTE* p = (BYTE*)m_imageExit.GetPixelAddress(0,i);
		//		for(int i =0;i<m_imageExit.GetWidth();i++){
		//			if(p[3] == 0)
		//			{
		//				p[0] = 0;
		//				p[1] = 0;
		//				p[2] = 0;
		//			}
		//			else
		//			{
		//				p[0]= ((int)p[0])*p[3]/255;
		//				p[1]= ((int)p[1])*p[3]/255;
		//				p[2]= ((int)p[2])*p[3]/255;
		//			}
		//			p+=4;
		//		}
		//	}
		//}
 
		hBmp = m_imageExit.Detach();
  }

  // get the bitmap dimensions
  BITMAP bmpInfo;
  memset (&bmpInfo, 0, sizeof (BITMAP));
  ::GetObject (hBmp, sizeof (BITMAP), &bmpInfo);
  if (bmpInfo.bmBitsPixel != 32)
  {
    m_imageExit.Attach(hBmp);
    return false;
  }

  LPBYTE pBitmapBits = NULL;

  HDC dcScreen = ::GetDC(NULL);
  HDC dcMemory = ::CreateCompatibleDC(dcScreen);

  HBITMAP pOldBitmap= (HBITMAP)SelectObject(dcMemory, hBmp);

  // get the window rectangle (we are only interested in the top left position)
  RECT rectDlg;
  GetWindowRect(m_hWndFullBtn, &rectDlg);

  // calculate the new window position/size based on the bitmap size
  POINT ptWindowScreenPosition;
  ptWindowScreenPosition.x = (rectDlg.left);
  ptWindowScreenPosition.y = (rectDlg.top);
  SIZE szWindow;
  szWindow.cx = bmpInfo.bmWidth;
  szWindow.cy = bmpInfo.bmHeight;

  // setup the blend function
  BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
  POINT ptSrc; // start point of the copy from dcMemory to dcScreen
  ptSrc.x = 0;
  ptSrc.y = 0;

  // perform the alpha blend
  BOOL	bRet = TRUE;

  UpdateLayeredWindow(m_hWndFullBtn, dcScreen, &ptWindowScreenPosition, &szWindow, dcMemory,
    &ptSrc, 0, &blendPixelFunction, ULW_ALPHA);

  // clean up
  ::ReleaseDC(NULL,dcScreen);
  ::DeleteDC(dcMemory);
  //::DeleteObject();
  SelectObject(dcMemory, pOldBitmap);
  if(pBitmapBits)
    delete [] pBitmapBits;

  m_imageExit.Attach(hBmp);

  return true;
}

LRESULT CALLBACK CFullscreenBtn::ViewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  CFullscreenBtn * pViewWindow = (CFullscreenBtn *)GetWindowLong (hwnd, GWL_USERDATA);
  if (pViewWindow == NULL)
    return(DefWindowProc(hwnd, uMsg, wParam, lParam));
  else
    return pViewWindow->OnReceiveMessage(hwnd,uMsg,wParam,lParam);
}

LRESULT CFullscreenBtn::OnReceiveMessage (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (m_pParent==NULL)
    {
        return 0;
    }

	UINT message = uMsg;
	switch (message)
	{
	case WM_LBUTTONUP:
		if (m_pParent)
		{
			m_pParent->ShowFullScreen();
		}
		break;
    case WM_KEYDOWN:
        {
            switch(wParam)
            {
            case VK_ESCAPE:
                if (m_pParent->IsFullScreen())
                {
                    m_pParent->ShowFullScreen();
                }
                break;

            default:
                break;
            }
        }
        break;
    case WM_SYSKEYDOWN:
        {
            return 0;
        }
        break;
	case WM_USERMOVE:
		{
			VOLOGI("WM_MOVE");
			if (FALSE == m_bFullscreen)
			{
				UpdatePosition();
			}
			return 0;
		}
		break;
	case WM_ERASEBKGND:
		return (LRESULT) 1;

	case WM_PAINT:
		{
			if(hWnd == m_hWndNormalBtn)
			{
				//if(m_imageEnter == NULL)
				//{
				//	m_imageEnter = new CImage();
				//	InitCImage(m_imageEnter, IDB_ENTER_FULLSCREEN);
				//}

				if(m_imageEnter == NULL)
					return DefWindowProc(hWnd, message, wParam, lParam);
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);

				//m_imageEnter->Draw(hdc, 0, 0);

				EndPaint(hWnd, &ps);
				return TRUE;
			}
		}
		return DefWindowProc(hWnd, message, wParam, lParam);


	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void CFullscreenBtn::UpdatePosition()
{
    if (m_bFullscreen)
    {
        if(FALSE == IsWindowVisible(m_hWndFullBtn))
            return;

        showButton(2,true);
    }
    else
    {
        if(FALSE == IsWindowVisible(m_hWndNormalBtn))
            return;

        showButton(1,true);
    }
}