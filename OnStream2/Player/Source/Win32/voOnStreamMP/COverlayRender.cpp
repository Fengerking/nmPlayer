
#ifndef _DEBUG

#include "COverlayRender.h"
#include "tchar.h"

#pragma warning (disable : 4996)

#ifdef _DEBUG
#pragma message("linking with Microsoft's DirectDraw library ...") 
#pragma comment(lib, "ddraw.lib") 
#pragma comment(lib, "dxguid.lib")
#endif //_DEBUG


DDPIXELFORMAT g_ddpfOverlayFormats = {sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 16,  0xF800, 0x07e0, 0x001F, 0};      // 16-bit RGB 5:6:5

#define SurfaceHeight	20

COverlayRender::COverlayRender(voSubTitleManager* manager)
: voSubTitleRenderBase(manager)
, m_hDll (NULL)
, m_pCreate (NULL)
, m_pDD (NULL)
, m_pDDPrimary (NULL)
, m_pDDOverlay (NULL)
, mhOverlayDC(NULL)
{
}

bool COverlayRender::CreateWnd (HWND hParent)
{
	GetClientRect(hParent , &m_rcWin);

	m_nSurfaceWidth = m_rcWin.right - m_rcWin.left;
	m_nSurfaceHeight = SurfaceHeight;

	m_brBlack = ::CreateSolidBrush (RGB (0, 0, 0));

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

	RECT rcView;
	::GetClientRect(hParent, &rcView);

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


#ifndef WINCE
	m_hWnd = CreateWindow(m_szClassName, m_szWindowName, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
							CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hParent, NULL, hInst, NULL);
	SetWindowPos (m_hWnd, NULL, rcView.left, rcView.top,1,1,0);// rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
#else
	//SetWindowPos (m_hWnd, HWND_TOP, rcView.left, rcView.top + 24, rcView.right - rcView.left, rcView.bottom - rcView.top - 24, 0);
	m_hWnd = CreateWindow(m_szClassName, m_szWindowName, WS_CHILD  | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
							CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hParent, NULL, hInst, NULL);
	SetWindowPos (m_hWnd, HWND_TOP, rcView.left , rcView.top ,1,1,0);// rcView.right - rcView.left , rcView.bottom - rcView.top , 0);
#endif 
	

	if (m_hWnd == NULL)
		return false;

	LONG lRC = SetWindowLong (m_hWnd, GWL_USERDATA, (LONG)this);

	return Init();
}

COverlayRender::~COverlayRender(void)
{
	if (m_brBlack != NULL)
	{
		DeleteObject (m_brBlack);
		m_brBlack = NULL;
	}

	UnInit();
}

void COverlayRender::Show(bool bShow)
{
	m_bShow = bShow;
	if(!m_bShow)
		Erase();
}
bool COverlayRender::Update(bool bForce )
{
	if(!bForce)
	{//check window position or size
		RECT rcParent;
		GetClientRect(m_pManager->GetWindow(), &rcParent);
		if(rcParent.right- rcParent.left != m_width ||
			rcParent.bottom- rcParent.top != m_height)
		{
			return false;
		}
	}

	bool b = OnDraw(mhOverlayDC);
	return b;
}
bool COverlayRender::OnDraw(HDC dc)
{
	if(m_pManager==NULL ||m_pDDOverlay == NULL)
		return false;
	RECT rcClient;
	GetClientRect(m_pManager->GetWindow(), &rcClient);
	if(rcClient.right- rcClient.left != m_width ||
			rcClient.bottom- rcClient.top != m_height)
	{
		Init();
		//GetClientRect(m_pManager->GetWindow(), &rcClient);
	}
	OnDrawToDC( mhOverlayDC, rcClient);
	return true;
}

//bool COverlayRender::Render(TCHAR *strSubtitle)
//{
//	if(m_pDDOverlay == NULL)
//	{
//		Init();
//	}
//
//	if( mhOverlayDC != NULL && strSubtitle != NULL)
//	{
//		int nLen = _tcslen(strSubtitle);
//
//		DrawText (mhOverlayDC, strSubtitle, _tcslen(strSubtitle) - 2 , &m_rcSurface , DT_CENTER | DT_BOTTOM);
//	}
//	return true;
//}
//
bool COverlayRender::Erase()
{
	if(mhOverlayDC)
		FillRect (mhOverlayDC, &m_rcSurface, m_brBlack);

	return true;
}

bool COverlayRender::Init()
{
	if(m_pManager == NULL || m_pManager->GetWindow() == NULL)
		return false;
	UnInit();
	if(m_hDll == NULL)
	{
		if(!InitDDraw())
			return false;
	}

	//CreateFont();

	if(m_pDDOverlay == NULL)
		return false;

	HRESULT hr = m_pDDOverlay->GetDC (&mhOverlayDC);
	if (FAILED (hr))
		return false;
	//m_hOldFont = (HFONT) SelectObject (mhOverlayDC, m_hFont);

	
	int nColor = 0XFFFFFF;
	SetTextColor (mhOverlayDC, nColor);
	SetBkMode (mhOverlayDC, TRANSPARENT);

	DDOVERLAYFX      Overlayfx;	 
	memset(&Overlayfx,0,sizeof(Overlayfx));
	Overlayfx.dwSize								= sizeof(Overlayfx);
	Overlayfx.dckSrcColorkey.dwColorSpaceLowValue	= RGB (0, 0, 0);
	Overlayfx.dckSrcColorkey.dwColorSpaceHighValue	= 0;

#ifndef _WIN32_WCE
	Overlayfx.dwAlphaEdgeBlend						= 128;
	Overlayfx.dwAlphaEdgeBlendBitDepth				= 64;
#endif

	RECT rcPrimary;

	SetRect (&rcPrimary, 0, m_rcWin.bottom - SurfaceHeight , m_nSurfaceWidth,  m_rcWin.bottom);
	SetRect (&m_rcSurface, 0, 0, m_nSurfaceWidth,  SurfaceHeight);


	hr = m_pDDOverlay->UpdateOverlay(&m_rcSurface , m_pDDPrimary, &rcPrimary, DDOVER_SHOW | DDOVER_KEYSRCOVERRIDE, &Overlayfx);
	return true;
}

bool COverlayRender::InitDDraw()
{
	if(m_pManager == NULL || m_pManager->GetWindow() == NULL)
		return false;
	m_hDll = LoadLibrary (_T("ddraw.dll"));
	if (m_hDll == NULL)
		return false;

#ifdef _WIN32_WCE
	m_pCreate =(DIRECTDRAWCREATE) GetProcAddress (m_hDll, _T("DirectDrawCreate"));
#else
	m_pCreate =(DIRECTDRAWCREATE) GetProcAddress (m_hDll, "DirectDrawCreate");
#endif // _WIN32_WCE

	if (m_pCreate == NULL)
		return false;

#ifdef _WIN32_WCE
	HRESULT hr = m_pCreate (NULL, &m_pDD, NULL);
	LPDIRECTDRAW	pDD = NULL;
#else
	//Create DDraw Object
	LPDIRECTDRAW	pDD = NULL;
	HRESULT hr = m_pCreate (NULL, &pDD, NULL);
	if (pDD == NULL)
		return false;
	hr = pDD->QueryInterface (IID_IDirectDraw4, (void**)&m_pDD);
#endif

	if (m_pDD == NULL)
		return false;

	//hr = m_pDD->SetCooperativeLevel(NULL , DDSCL_NORMAL);///
	if(pDD != NULL)
		pDD->Release ();
	//if (FAILED (hr))
	//	return false;

	//Create Primary surface
#ifdef _WIN32_WCE
	DDSURFACEDESC	ddsd;
#else
	DDSURFACEDESC2	ddsd;
#endif

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	hr = m_pDD->CreateSurface (&ddsd, &m_pDDPrimary, NULL);
	if (FAILED (hr))
		return false;

	// See if we can support overlays.
	DDCAPS	ddcaps;
	memset(&ddcaps, 0, sizeof(ddcaps));
	ddcaps.dwSize = sizeof(ddcaps);
	hr = m_pDD->GetCaps (&ddcaps,NULL);
	if (FAILED (hr))
		return false;

#ifndef _WIN32_WCE
	if ((!ddcaps.dwCaps & DDCAPS_OVERLAY))
		return false;
	// Get alignment info to compute our overlay surface size.
	if ((ddcaps.dwCaps & DDCAPS_ALIGNSIZESRC) && ddcaps.dwAlignSizeSrc)
	{
	}
#endif // _WIN32_WCE

	// Create the overlay layer1 surface. 
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);

#ifdef _WIN32_WCE
	ddsd.ddsCaps.dwCaps = DDSCAPS_OVERLAY | DDSCAPS_FLIP |
		DDSCAPS_VIDEOMEMORY;
#else
	ddsd.ddsCaps.dwCaps = DDSCAPS_OVERLAY | DDSCAPS_COMPLEX | DDSCAPS_FLIP |
		DDSCAPS_VIDEOMEMORY;
#endif


	ddsd.dwFlags  = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_BACKBUFFERCOUNT | 
		DDSD_PIXELFORMAT;
	ddsd.dwWidth  = m_nSurfaceWidth;
	ddsd.dwHeight = m_nSurfaceHeight;
	ddsd.dwBackBufferCount = 1;
	ddsd.ddpfPixelFormat = g_ddpfOverlayFormats;

	hr = m_pDD->CreateSurface(&ddsd, &m_pDDOverlay, NULL);
	if (hr != DD_OK)
		return false;


	return true;
}

bool COverlayRender::UnInit()
{
	if (m_hDll == NULL)
		return true;

	if(mhOverlayDC != NULL)
	{
		//if(m_hOldFont != NULL)
		//{
		//	SelectObject(mhOverlayDC , m_hOldFont);
		//	m_hOldFont = NULL;
		//}

		if(m_pDDOverlay != NULL)
		{
			m_pDDOverlay->ReleaseDC(mhOverlayDC);
			mhOverlayDC = NULL;
		}
	}

	if (m_pDDOverlay != NULL)
		m_pDDOverlay->Release ();
	if (m_pDDPrimary != NULL)
		m_pDDPrimary->Release ();
	if (m_pDD != NULL)
		m_pDD->Release ();

	if (m_hDll != NULL)
		FreeLibrary (m_hDll);
	m_hDll = NULL;
	return true;
}

#endif //_DEBUG