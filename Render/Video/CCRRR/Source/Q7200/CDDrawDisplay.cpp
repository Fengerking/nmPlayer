#include "CDDrawDisplay.h"
#include "..\myceddk.h"

#define PAGE_SIZE 0x1000
#define PAGE_MASK 0xFFF

//#define _SAME_COORDINATE_

CDDrawDisplay::CDDrawDisplay(void)
: mpDDraw(NULL)
, mpPrimarySurface(NULL)
, mbRotateSurface(false)
, mpOverlaySurface(NULL)
, mnVideoWidth(0)
, mnVideoHeight(0)
{
	Init();
}

CDDrawDisplay::~CDDrawDisplay(void)
{
	Uninit();
}

HRESULT CDDrawDisplay::Init()
{
	Uninit();

	HRESULT hr = DirectDrawCreate(NULL , &mpDDraw , NULL);
	if(hr != S_OK)
		return hr;

	hr = mpDDraw->SetCooperativeLevel( NULL , DDSCL_NORMAL);
	if(hr != S_OK)
		return hr;

	DDCAPS ddcaps ; 
	ZeroMemory(&ddcaps , sizeof(DDCAPS));
	ddcaps.dwSize = sizeof(DDCAPS);
	hr = mpDDraw->GetCaps(&ddcaps , NULL);
	hr = CreatePrimarySurface();

	return hr;
}

HRESULT CDDrawDisplay::Uninit()
{
	if(mpOverlaySurface != NULL)
	{
		int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		RECT rs = { 0, 0, nScreenWidth, nScreenHeight };
		RECT rd = { 0, 0, nScreenWidth, nScreenHeight };

		HRESULT hr = mpOverlaySurface->UpdateOverlay(&rs, mpPrimarySurface, &rd, 
			DDOVER_HIDE, NULL);


		mpOverlaySurface->Release();
		mpOverlaySurface = NULL;
	}

	if(mpPrimarySurface != NULL)
	{
		mpPrimarySurface->Release();
		mpPrimarySurface = NULL;
	}

	if(mpDDraw != NULL)
	{
		mpDDraw->Release();
		mpDDraw = NULL;
	}

	return S_OK;
}

HRESULT CDDrawDisplay::CreatePrimarySurface()
{
	if(mpDDraw == NULL)
		return E_FAIL;

	if(mpPrimarySurface != NULL)
	{
		mpPrimarySurface->Release();
		mpPrimarySurface = NULL;
	}

	 DDSURFACEDESC ddsc;
	 ZeroMemory(&ddsc , sizeof(ddsc));

	 ddsc.dwSize = sizeof(ddsc);
	 ddsc.dwFlags = DDSD_CAPS;
	 ddsc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	 HRESULT hr = mpDDraw->CreateSurface(&ddsc , &mpPrimarySurface , NULL);
	if(hr != S_OK)
	{
		hr = mpDDraw->GetGDISurface(&mpPrimarySurface);
		if(hr != S_OK)
			return hr;
	}

	ZeroMemory(&ddsc , sizeof(ddsc));
	ddsc.dwSize = sizeof(ddsc);
	DWORD flags = DDLOCK_WAITNOTBUSY;

	hr = mpPrimarySurface->Lock(NULL , &ddsc , false , NULL);
	if(hr != S_OK)
		return hr;

	if(ddsc.lPitch == ddsc.dwWidth * 2)
	{
		mbRotateSurface = false;
	}
	else
	{
		mbRotateSurface = true;
	}

#ifdef _SAME_COORDINATE_
	mbRotateSurface = false;
#endif // _SAME_COORDINATE_

	mnSurfaceHeight = ddsc.dwHeight;
	mnSurfaceWidth  = ddsc.dwWidth;

	mpSurfaceVir = (BYTE *)ddsc.lpSurface;

	DWORD pfn[1];
	BOOL b = LockPages(mpSurfaceVir, 1, pfn, LOCKFLAG_QUERY_ONLY);
	if (!b)
		return 0;
	DWORD phys = pfn[0] << UserKInfo[KINX_PFN_SHIFT];
	phys |= ((DWORD)mpSurfaceVir & PAGE_MASK);

	mpSurfacePhy = (BYTE *)phys;
	if(ddsc.lXPitch < 0)
	{
		mpSurfacePhy -= (abs(ddsc.lXPitch) * (mnSurfaceWidth - 1));
	}

	UnlockPages(mpSurfaceVir , 1);

	mpPrimarySurface->Unlock(NULL);
	return hr;	
}

HRESULT CDDrawDisplay::CreateOverlaySurface(int nWidth , int nHeight)
{
	if(mnVideoHeight == nWidth && mnVideoHeight == nHeight)
		return S_OK;

	if(mpOverlaySurface != NULL)
	{
		mpOverlaySurface->Release();
		mpOverlaySurface = NULL;
	}

	mnVideoWidth = nWidth;
	mnVideoHeight = nHeight;

	const DDPIXELFORMAT OverlayFormat =
#if 1
	{ sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 16,  0xF800, 0x07e0, 0x001F, 0 }; //RGB565
#else
	{
		sizeof(DDPIXELFORMAT), 
			DDPF_FOURCC, 
			MAKEFOURCC('Y','V','1','2'),
			0, 0, 0, 0, 0
	};  // YV12, YUV420
#endif

	DDSURFACEDESC ddsd;
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.ddsCaps.dwCaps =  DDSCAPS_OVERLAY | DDSCAPS_FLIP;
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_BACKBUFFERCOUNT | DDSD_PIXELFORMAT;
	ddsd.dwBackBufferCount = 1;
	ddsd.ddpfPixelFormat = OverlayFormat;
	ddsd.dwWidth = nWidth; 
	ddsd.dwHeight = nHeight;
	HRESULT hr = mpDDraw->CreateSurface(&ddsd, &mpOverlaySurface, NULL);
	if (FAILED(hr))
	{
		printf("Create overlay surface return %x \r\n" , hr);
		return hr;
	}

	ZeroMemory(&ddsd , sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	DWORD flags = DDLOCK_WAITNOTBUSY;

	hr = mpOverlaySurface->Lock(NULL , &ddsd , false , NULL);
	if(hr != S_OK)
		return hr;

	mpOverlayVir = (BYTE *)ddsd.lpSurface;

	ZeroMemory(mpOverlayVir , nWidth * nHeight * 2);

	DWORD pfn[1];
	BOOL b = LockPages(mpOverlayVir, 1, pfn, LOCKFLAG_QUERY_ONLY);
	if (!b)
		return 0;
	DWORD phys = pfn[0] << UserKInfo[KINX_PFN_SHIFT];
	phys |= ((DWORD)mpOverlayVir & PAGE_MASK);

	mpOverlayPhy = (BYTE *)phys;

	UnlockPages(mpOverlayVir , 1);

	mpOverlaySurface->Unlock(NULL);	

	return hr;
}

HRESULT	CDDrawDisplay::UpdateOverlay(int nLeft , int nTop , int nRight , int nBotton , BOOL bShow)
{
	if(mpOverlaySurface == NULL || mpPrimarySurface == NULL)
		return E_NOTIMPL;

	RECT rs = { 0, 0, mnVideoWidth , mnVideoHeight };
	RECT rd = { nLeft , nTop , nRight , nBotton };
	
	HRESULT hr = S_OK;
	if(bShow)
		hr = mpOverlaySurface->UpdateOverlay(&rs, mpPrimarySurface, &rd, DDOVER_SHOW, NULL);
	else
		hr = mpOverlaySurface->UpdateOverlay(&rs, mpPrimarySurface, &rd, DDOVER_HIDE, NULL);

#ifdef _DEBUG
	if(hr != S_OK)
	{
		//printf("UpdateOverlay failed hr = %x \r\n " , hr);
	}
#endif


	return hr;
}

void * CDDrawDisplay::LockSurface()
{
	HRESULT hr = E_FAIL;

	DDSURFACEDESC ddsd;
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	DWORD flags = DDLOCK_WAITNOTBUSY;

#ifdef _USE_OVERLAY
	if(mpOverlaySurface != NULL)
	{
		hr = mpOverlaySurface->Lock(NULL , &ddsd , flags , NULL);
		if(hr == S_OK)
		{
			return mpOverlayPhy;
		}
	}
#else  //_USE_OVERLAY
	if(mpPrimarySurface != NULL)
	{
		hr = mpPrimarySurface->Lock(NULL , &ddsd , flags , NULL);
		if(hr == DDERR_SURFACELOST)
		{
			CreatePrimarySurface();
			hr = mpPrimarySurface->Lock(NULL , &ddsd , flags , NULL);
		}

		if(hr == S_OK)
		{
			
			return mpSurfacePhy;
		}
	}
#endif //_USE_OVERLAY
	

	return NULL;
}

HRESULT CDDrawDisplay::UnlockSurface()
{
#ifdef _USE_OVERLAY
	if(mpOverlaySurface != NULL)
	{
		mpOverlaySurface->Unlock(NULL);
		return S_OK;
	}
#else  //_USE_OVERLAY
	if(mpPrimarySurface != NULL)
	{
		mpPrimarySurface->Unlock(NULL);
		return S_OK;
	}
#endif //_USE_OVERLAY


	return E_FAIL;
}
