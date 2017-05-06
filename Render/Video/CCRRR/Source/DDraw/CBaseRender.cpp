
#include "windows.h"
#include "tchar.h"
#include "voIVCommon.h"
#include "voCCRRR.h"
#include "voVideo.h"
#include "CBaseConfig.h"
#include "vompType.h"
#include "voLog.h"

#include "CBaseRender.h"

#pragma warning (disable : 4996)

extern VO_PTR g_hvoccrrInst;

CBaseRender::CBaseRender(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
	: m_pMemOP(pMemOP)
	, m_hWnd((HWND)hView)
	, m_hDDLib (NULL)
	, m_fDDCreate (NULL)

	, m_pDD (NULL)
	, m_pPrmSurface (NULL)
	, m_pDrwSurface (NULL)

	, m_ddVideoType (DDVIDEO_YV12)
	, m_bOverlay (false)
	, m_bOverride (false)
	, m_bOverlayUpdate (false)
	, m_bOverlayShow (false)
	, m_nSurfaceWidth (0)
	, m_nSurfaceHeight (0)

	, m_nRenderFrames (0)
	, mbUpsideDown(VO_FALSE)
	, mstrWorkPath(NULL)

	, m_nInWidth(0)
	, m_nInHeight(0)
	, m_nOutWidth(0)
	, m_nOutHeight(0)
{

	m_prop.nRender = 1;
	m_prop.nOverlay = 1;
	m_prop.nKeyColor = 0;
	m_prop.nRotate = VO_RT_DISABLE;
	m_prop.nOutBuffer = 0;
	m_prop.nFlag = 0;
	
	m_nScreenWidth = GetSystemMetrics (SM_CXSCREEN);
	m_nScreenHeight = GetSystemMetrics (SM_CYSCREEN);
	SetRect (&m_rcWnd, 0, 0, 0, 0);
	SetRect (&m_rcVideo, 0, 0, 0, 0);

	m_hDDLib = LoadLibrary (_T("DDraw.dll"));
	if (m_hDDLib != NULL)
#ifdef _WIN32_WCE
		m_fDDCreate = (DDCREATE)GetProcAddress(m_hDDLib, _T("DirectDrawCreate"));
#else
		m_fDDCreate = (DDCREATE)GetProcAddress(m_hDDLib, "DirectDrawCreate");
#endif // WIN32_WCE

	GetPropertyFromCfg();
	
	memset(&m_ddBltFX, 0, sizeof(DDBLTFX));
	m_ddBltFX.dwSize = sizeof(m_ddBltFX);
}

CBaseRender::~CBaseRender(void)
{
	Close();

	if (m_hDDLib != NULL)
	{
		::FreeLibrary (m_hDDLib);
		m_hDDLib = NULL;
	}

	if(mstrWorkPath != NULL)
	{
		delete [] mstrWorkPath;
		mstrWorkPath = NULL;
	}
}

VO_U32 CBaseRender::GetProperty (VO_CCRRR_PROPERTY * pProperty)
{
	if (pProperty == NULL)
		return VO_ERR_INVALID_ARG | VO_INDEX_SNK_CCRRR;

	memcpy (pProperty, &m_prop, sizeof (VO_CCRRR_PROPERTY));

	return VO_ERR_NONE;
}

VO_U32 CBaseRender::GetInputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex)
{
	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_CCRRR;
}

VO_U32 CBaseRender::GetOutputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex)
{
	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_CCRRR;
}

VO_U32 CBaseRender::SetColorType (VO_IV_COLORTYPE nInputColor, VO_IV_COLORTYPE nOutputColor)
{
	if (nInputColor != VO_COLOR_YUV_PLANAR420 && nInputColor != VO_COLOR_YUV_420_PACK && nInputColor != VO_COLOR_YUV_420_PACK_2)
		return VO_ERR_FAILED;
	
	m_nInputColor = nInputColor;
	m_nOutputColor = nOutputColor;

	return VO_ERR_NONE;
}


VO_U32 CBaseRender::SetCCRRSize (VO_U32 * pInWidth, VO_U32 * pInHeight, VO_U32 * pOutWidth, VO_U32 * pOutHeight, VO_IV_RTTYPE nRotate)
{
	if (m_nInWidth == * pInWidth && m_nInHeight == * pInHeight && m_nOutWidth == *pOutWidth && m_nOutHeight == *pOutHeight && m_nRotate == nRotate)
			return VO_ERR_NONE;

	

	if(m_pPrmSurface == NULL)
		Init();
	
	m_nRotate = nRotate;
	m_nInWidth = *pInWidth;
	m_nInHeight = *pInHeight;

	m_nInWidth = m_nInWidth / 8 * 8;

	if (m_pDrwSurface != NULL)
	{
		m_pDrwSurface->Release ();
		m_pDrwSurface = NULL;
	}

	m_nSurfaceWidth = (m_nInWidth + 7) / 8 * 8 ;
	m_nSurfaceHeight = m_nInHeight;

	SetRect (&m_rcVideo, 0, 0, m_nInWidth, m_nInHeight);
	m_bOverlayUpdate = false;

	if (m_nRotate == VO_RT_DISABLE || m_nRotate == VO_RT_180)
	{
		//m_nOutWidth = (*pOutWidth + 0xF) & ~0xF;
		//m_nOutHeight = (*pOutHeight + 0x3) & ~0x3;

		m_nOutWidth = *pOutWidth ;
		m_nOutHeight = *pOutHeight ;
	}
	else
	{
		//m_nOutWidth = (*pOutWidth + 0xF) & ~0xF;
		//m_nOutHeight = (*pOutHeight + 0x3) & ~0x3;

		m_nOutWidth = *pOutHeight;
		m_nOutHeight = *pOutWidth ;
	}

 	GetWindowRect (m_hWnd, &m_rcWnd);
	m_rcWnd.left = m_rcWnd.left + (m_rcWnd.right - m_rcWnd.left - m_nOutWidth) / 2;
	m_rcWnd.top =  m_rcWnd.top + (m_rcWnd.bottom - m_rcWnd.top - m_nOutHeight) / 2;

	m_rcWnd.left = m_rcWnd.left / mnAlignBoundaryDest * mnAlignBoundaryDest;
	m_rcWnd.top = m_rcWnd.top;


	m_rcWnd.right = m_rcWnd.left + m_nOutWidth;
	m_rcWnd.bottom = m_rcWnd.top + m_nOutHeight;

	if(m_rcWnd.left < 0)
		m_rcWnd.left = 0;

	if(m_rcWnd.top < 0)
		m_rcWnd.top = 0;

	if(m_rcWnd.right > m_nScreenWidth)
		m_rcWnd.right = m_nScreenWidth;

	if(m_rcWnd.bottom > m_nScreenHeight)
		m_rcWnd.bottom = m_nScreenHeight;

	m_bOverlayUpdate = false;

	if (m_bOverlay && m_nRenderFrames > 0 && m_bOverlayShow)
		return ShowOverlay (true);

	return 0;
}

VO_U32 CBaseRender::Process (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_BUFFER * pOutputBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	if(m_pDrwSurface == NULL)
	{
		if(m_pPrmSurface == NULL)
		{
			Init();
		}

		CreateDrawSurface();
	}

	if(m_pDrwSurface == NULL )
		return VO_ERR_NOT_IMPLEMENT;


	if (pOutputBuffer != NULL)
	{
		pOutputBuffer->Buffer[0] = NULL;
		pOutputBuffer->Stride[0] = 0;
	}

#ifdef _TEST_PERFORMANCE
	DWORD dwStart0 = GetThreadTime (NULL);
#endif // _TEST_PERFORMANCE

	HRESULT		hr = S_OK;

    memset(&m_DDSurfaceDesc, 0, sizeof(m_DDSurfaceDesc));
    m_DDSurfaceDesc.dwSize = sizeof(m_DDSurfaceDesc);

#ifdef _TEST_PERFORMANCE
	DWORD dwStart = GetThreadTime (NULL);
	TCHAR szInfo[32];
#endif // _TEST_PERFORMANCE

#ifdef _WIN32_WCE50
	hr = m_pDrwSurface->Lock( NULL, &m_DDSurfaceDesc, DDLOCK_WAIT, NULL);
#else
	hr = m_pDrwSurface->Lock( NULL, &m_DDSurfaceDesc, DDLOCK_WAITNOTBUSY, NULL);
#endif // 
	if (hr != DD_OK)
		return VO_ERR_FAILED;
	
#ifdef _TEST_PERFORMANCE
	_stprintf (szInfo, _T("Lock : %d\r\n"), GetThreadTime (NULL) - dwStart);
	OutputDebugString (szInfo);
	dwStart = GetThreadTime (NULL);
#endif // _TEST_PERFORMANCE

	int i = 0;
	LPBYTE	lpBuffer = (LPBYTE)m_DDSurfaceDesc.lpSurface;
	int nStride = m_nSurfaceWidth;
#ifndef _WIN32_WCE
	nStride = m_DDSurfaceDesc.lPitch;
#endif // m_nInWidth

	if(lpBuffer != NULL)
		FillMem (pVideoBuffer, lpBuffer, nStride);

	m_pDrwSurface->Unlock(NULL);

#ifdef _TEST_PERFORMANCE
	_stprintf (szInfo, _T("Copy : %d\r\n"), GetThreadTime (NULL) - dwStart);
	OutputDebugString (szInfo);
	dwStart = GetThreadTime (NULL);
#endif // _TEST_PERFORMANCE

	if (m_bOverlay)
	{
		if (!m_bOverlayUpdate)
		{
			ShowOverlay (true);
			m_bOverlayUpdate = true;
		}
	}
	else
	{
#ifdef _WIN32_WCE50
		hr = m_pDrwSurface->Blt(&m_rcWnd, m_pDrwSurface, &m_rcVideo, DDBLT_WAIT, &m_ddBltFX);
#else
		hr = m_pPrmSurface->Blt(&m_rcWnd, m_pDrwSurface, &m_rcVideo, DDBLT_WAITNOTBUSY | DDBLT_WAITVSYNC, &m_ddBltFX);
#endif //
	}
	
	if (hr!=DD_OK)
		return VO_ERR_FAILED;

#ifdef _TEST_PERFORMANCE
	_stprintf (szInfo, _T("Draw : %d\r\n"), GetThreadTime (NULL) - dwStart);
	OutputDebugString (szInfo);
#endif //_TEST_PERFORMANCE

#ifdef _TEST_PERFORMANCE
	_stprintf (szInfo, _T("All  : %d\r\n"), GetThreadTime (NULL) - dwStart0);
	OutputDebugString (szInfo);
#endif // _TEST_PERFORMANCE

	m_nRenderFrames++;

	return VO_ERR_NONE;

}

VO_U32 CBaseRender::WaitDone (void)
{
	return VO_ERR_NONE;
}

VO_U32 CBaseRender::SetCallBack (VOVIDEOCALLBACKPROC pCallBack, VO_PTR pUserData)
{
	m_fCallBack = pCallBack;
	m_pUserData = pUserData;

	return VO_ERR_NONE;
}

VO_U32 CBaseRender::GetVideoMemOP (VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP)
{
	*ppVideoMemOP = NULL;

	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_CCRRR;
}

VO_U32 CBaseRender::SetParam (VO_U32 nID, VO_PTR pValue)
{
	if(nID == VO_CCRRR_PMID_ShowOverlay)
	{
		int nValue = *((int *)pValue);
		if(nValue != 0)
			ShowOverlay(true);
		else
			ShowOverlay(false);

		return 0;
	}
	else if(nID == VO_PID_VIDEO_UPSIDEDOWN)
	{
		mbUpsideDown = *(VO_BOOL *)pValue;
		return 0;
	}
	else if(nID == VOMP_PID_PLAYER_PATH)
	{
		if(mstrWorkPath == NULL)
		{
			mstrWorkPath = new TCHAR[MAX_PATH];
		}

		memset(mstrWorkPath , 0 ,  sizeof(TCHAR) * MAX_PATH);
		_tcscpy(mstrWorkPath , (TCHAR *)pValue);

		GetPropertyFromCfg();
	}

	return 0;
}

VO_U32 CBaseRender::GetParam (VO_U32 nID, VO_PTR pValue)
{
	return 0;
}

VO_U32 CBaseRender::Close (void)
{
	HRESULT hr = S_OK;

	ShowOverlay (false);

	if (m_pDrwSurface != NULL)
	{
		m_pDrwSurface->Release ();
		m_pDrwSurface = NULL;
	}

	if (m_pPrmSurface != NULL)
	{
		m_pPrmSurface->Release ();
		m_pPrmSurface = NULL;
	}

	if (m_pDD != NULL)
	{
		m_pDD->Release ();
		m_pDD = NULL;
	}

	return VO_ERR_NONE;
}

VO_U32 CBaseRender::Init()
{
	if (  m_pPrmSurface != NULL)
		return VO_ERR_NONE;

	Close ();

	LPDIRECTDRAW	pDD = NULL;
	HRESULT hr = m_fDDCreate (NULL, &pDD, NULL);
	if (pDD == NULL)
		return VO_ERR_FAILED;

#ifdef _WIN32_WCE
#ifdef _WIN32_WCE50
	hr = pDD->QueryInterface (IID_IDirectDraw4, (void**)&m_pDD);
#else
	m_pDD = pDD;
	m_pDD->AddRef ();
#endif // _WIN32_WCE50
#else
	hr = pDD->QueryInterface (IID_IDirectDraw7, (void**)&m_pDD);
#endif // _WIN32_WCE
	pDD->Release ();
	if (hr != DD_OK)
		return VO_ERR_FAILED;

	memset (&m_DDCaps, 0, sizeof (DDCAPS));
	m_DDCaps.dwSize = sizeof (DDCAPS);
	hr = m_pDD->GetCaps (&m_DDCaps, NULL);
	if (hr != DD_OK)
		return VO_ERR_FAILED;

	mnAlignBoundarySrc	= m_DDCaps.dwAlignBoundarySrc;
	mnAlignSizeSrc		= m_DDCaps.dwAlignSizeSrc;

	mnAlignBoundaryDest = m_DDCaps.dwAlignBoundaryDest;
	mnAlignSizeDest		= m_DDCaps.dwAlignSizeDest;


	if(mnAlignBoundarySrc < 2)
		mnAlignBoundarySrc = 2;


	if(mnAlignSizeSrc < 2)
		mnAlignSizeSrc = 2;


	if(mnAlignBoundaryDest < 2)
		mnAlignBoundaryDest = 2;


	if(mnAlignSizeDest < 2)
		mnAlignSizeDest = 2;

	hr = m_pDD->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL);

    memset(&m_DDSurfaceDesc, 0, sizeof(m_DDSurfaceDesc));
    m_DDSurfaceDesc.dwSize = sizeof(m_DDSurfaceDesc);
    m_DDSurfaceDesc.dwFlags = DDSD_CAPS;
    m_DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    hr = m_pDD->CreateSurface(&m_DDSurfaceDesc, &m_pPrmSurface, NULL);
	if (hr != DD_OK)
		return VO_ERR_FAILED;

#ifndef _WIN32_WCE
	LPDIRECTDRAWCLIPPER  pcClipper;   // Cliper
	if( m_pDD->CreateClipper( 0, &pcClipper, NULL ) != DD_OK )
		return VO_ERR_FAILED;
	if( pcClipper->SetHWnd( 0, m_hWnd ) != DD_OK )
	{
		pcClipper->Release();
		return VO_ERR_FAILED;
	}
	if( m_pPrmSurface->SetClipper( pcClipper ) != DD_OK )
	{
		pcClipper->Release();
		return VO_ERR_FAILED;
	}
	pcClipper->Release();
#endif //_WIN32_WCE

	memset (&m_DDSurfaceDesc, 0, sizeof (DDSURFACEDESC));
	m_DDSurfaceDesc.dwSize = sizeof (DDSURFACEDESC);
	hr = m_pPrmSurface->GetSurfaceDesc (&m_DDSurfaceDesc);
	if (hr != DD_OK)
		return VO_ERR_FAILED;

	return VO_ERR_NONE;
}

int CBaseRender::CreateDrawSurface (void)
{
	if (m_pDrwSurface != NULL)
		return 0;

	memset(&m_DDSurfaceDesc, 0, sizeof(m_DDSurfaceDesc));
	m_DDSurfaceDesc.dwSize = sizeof(m_DDSurfaceDesc);

	if (m_bOverlay)
		m_DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_OVERLAY;
	else
#ifdef _WIN32_WCE
		m_DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY;
#else
		m_DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
#endif // _WIN32_WCE

	m_DDSurfaceDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	m_DDSurfaceDesc.dwWidth  = m_nSurfaceWidth;
	m_DDSurfaceDesc.dwHeight = m_nSurfaceHeight;

	m_DDSurfaceDesc.ddpfPixelFormat = YV12Formats;

	HRESULT hr = m_pDD->CreateSurface(&m_DDSurfaceDesc, &m_pDrwSurface, NULL);
	if(hr != DD_OK)
	{
		m_DDSurfaceDesc.ddpfPixelFormat = NV12Formats;
		hr = m_pDD->CreateSurface(&m_DDSurfaceDesc, &m_pDrwSurface, NULL);
		if(hr != DD_OK)
			return -1;
		m_ddVideoType = DDVIDEO_NV12;
	}

	//if(m_bOverride)
	//{
	//	hr = m_pDrwSurface->SetColorKey(DDCKEY_DESTOVERLAY , &(m_ddOverlayFX.dckSrcColorkey));
	//}

	return 0;
}

VO_U32 CBaseRender::ShowOverlay (bool bShow)
{
	if (m_bOverlay && m_pDrwSurface != NULL)
	{
		if (m_bOverlayUpdate && m_bOverlayShow == bShow)
			return VO_ERR_NONE;
		m_bOverlayShow = bShow;

		HRESULT hr = S_OK;

		if (bShow)
		{
			if (m_bOverride)
			{
				//RETAILMSG(1, (L"0 call UpdateOverlay  src  : %d %d %d %d  dst : %d %d %d %d \r\n" , m_rcVideo.left , m_rcVideo.top , m_rcVideo.right , m_rcVideo.bottom , m_rcWnd.left , m_rcWnd.top , m_rcWnd.right , m_rcWnd.bottom));
				hr = m_pDrwSurface->UpdateOverlay(&m_rcVideo, m_pPrmSurface, &m_rcWnd, DDOVER_SHOW | DDOVER_KEYDESTOVERRIDE | DDOVER_ALPHACONSTOVERRIDE, &m_ddOverlayFX);
			}
			else
			{
				//RETAILMSG(1, (L"1 call UpdateOverlay  src  : %d %d %d %d  dst : %d %d %d %d \r\n" , m_rcVideo.left , m_rcVideo.top , m_rcVideo.right , m_rcVideo.bottom , m_rcWnd.left , m_rcWnd.top , m_rcWnd.right , m_rcWnd.bottom));
				hr = m_pDrwSurface->UpdateOverlay(&m_rcVideo, m_pPrmSurface, &m_rcWnd, DDOVER_SHOW | DDOVER_ALPHACONSTOVERRIDE, &m_ddOverlayFX);
			}

			//OutputDebugString (_T("Show Overlay\r\n"));
		}
		else
		{
			hr = m_pDrwSurface->UpdateOverlay(&m_rcVideo, m_pPrmSurface, &m_rcWnd, DDOVER_HIDE | DDOVER_WAITNOTBUSY | DDOVER_WAITVSYNC, &m_ddOverlayFX);

			//OutputDebugString (_T("Hide Overlay\r\n"));
		}

		if(FAILED(hr))
			return VO_ERR_FAILED;
	}

	return VO_ERR_NONE;
}

bool CBaseRender::FillMem (VO_VIDEO_BUFFER * pInput, LPBYTE pOutBuffer, int nOutStride)
{
	if(mbUpsideDown == VO_FALSE)
	{
		LPBYTE	lpSurf = pOutBuffer;
		LPBYTE  lpDest = pInput->Buffer[0];
		int		i = 0;

		for (i = 0; i < m_nInHeight; i++)
		{
			memcpy (lpSurf, lpDest, m_nInWidth);
			lpSurf += nOutStride;
			lpDest += pInput->Stride[0];
		}

		if (m_ddVideoType == DDVIDEO_NV12)
		{
			lpSurf = pOutBuffer + m_nSurfaceWidth * m_nSurfaceHeight;
			PackUV (lpSurf,  pInput->Buffer[2],  pInput->Buffer[1], pInput->Stride[2], pInput->Stride[1], m_nInHeight / 2, m_nInWidth / 2);
		}
		else
		{
			int nLoopTime = m_nInHeight / 2;
			int nAddSize = nOutStride / 2;
			int nCopySize = m_nInWidth / 2;
			lpSurf = pOutBuffer + m_nSurfaceWidth * m_nSurfaceHeight;
			lpDest =  pInput->Buffer[2];
			for (i = 0; i <nLoopTime ; i++)
			{
				memcpy (lpSurf, lpDest, nCopySize);
				lpSurf += nAddSize;
				lpDest += pInput->Stride[2];
			}

			lpSurf = pOutBuffer + m_nSurfaceWidth * m_nSurfaceHeight * 5 / 4;
			lpDest =  pInput->Buffer[1];
			for (i = 0; i < nLoopTime; i++)
			{
				memcpy (lpSurf, lpDest, nCopySize);
				lpSurf += nAddSize;
				lpDest += pInput->Stride[1];
			}
		}
	}
	else
	{
		LPBYTE	lpSurf = pOutBuffer;
		LPBYTE  lpDest = pInput->Buffer[0] + pInput->Stride[0] * (m_nInHeight -1);
		int		i = 0;


		for (i = 0; i < m_nInHeight; i++)
		{
			memcpy (lpSurf, lpDest, m_nInWidth);
			lpSurf += nOutStride;
			lpDest -= pInput->Stride[0];
		}

		if (m_ddVideoType == DDVIDEO_NV12)
		{
			lpSurf = pOutBuffer + m_nSurfaceWidth * m_nSurfaceHeight;
			PackUV (lpSurf,  pInput->Buffer[2],  pInput->Buffer[1], pInput->Stride[2], pInput->Stride[1], m_nInHeight / 2, m_nInWidth / 2);
		}
		else
		{
			int nLoopTime = m_nInHeight / 2;
			int nAddSize = nOutStride / 2;
			int nCopySize = m_nInWidth / 2;
			lpSurf = pOutBuffer + m_nSurfaceWidth * m_nSurfaceHeight;
			lpDest =  pInput->Buffer[2] + pInput->Stride[2] * (nLoopTime - 1);
			for (i = 0; i <nLoopTime ; i++)
			{
				memcpy (lpSurf, lpDest, nCopySize);
				lpSurf += nAddSize;
				lpDest -= pInput->Stride[2];
			}

			lpSurf = pOutBuffer + m_nSurfaceWidth * m_nSurfaceHeight * 5 / 4;
			lpDest =  pInput->Buffer[1] + pInput->Stride[1] * (nLoopTime - 1);
			for (i = 0; i < nLoopTime; i++)
			{
				memcpy (lpSurf, lpDest, nCopySize);
				lpSurf += nAddSize;
				lpDest -= pInput->Stride[1];
			}
		}
	}
	

	return true;
}

bool CBaseRender::PackUV(void* dstUV, void* srcU, void* srcV, int strideU, int strideV, int rows, int width)
{
	if(mbUpsideDown == VO_FALSE)
	{
		BYTE* pUV = (BYTE*) dstUV;
		BYTE* pU = (BYTE*) srcU  ;
		BYTE* pV = (BYTE*) srcV;

		int offsetU = strideU - width;
		int offsetV = strideV - width;

		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < width; j++)
			{
				*pUV++ = *pV++;
				*pUV++ = *pU++;
			}
			pU += offsetU;
			pV += offsetV;
		}
	}
	else
	{
		BYTE* pUV = (BYTE*) dstUV;
		BYTE* pU = (BYTE*) srcU + (rows - 1) * width;
		BYTE* pV = (BYTE*) srcV + (rows - 1) * width;

		int offsetU = strideU - width;
		int offsetV = strideV - width;

		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < width; j++)
			{
				*pUV++ = *pV++;
				*pUV++ = *pU++;
			}
			pU += offsetU;
			pV += offsetV;

			pU -= 2 * width;
			pV -= 2 * width;
		}
	}

	return true;
}

int CBaseRender::GetThreadTime (HANDLE hThread)
{
	if (hThread == NULL)
		hThread = GetCurrentThread ();

	FILETIME ftCreationTime;
	FILETIME ftExitTime;
	FILETIME ftKernelTime;
	FILETIME ftUserTime;

	BOOL bRC = GetThreadTimes(hThread, &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime);
	if (!bRC)
		return -1;

	LONGLONG llKernelTime = ftKernelTime.dwHighDateTime;
	llKernelTime = llKernelTime << 32;
	llKernelTime += ftKernelTime.dwLowDateTime;

	LONGLONG llUserTime = ftUserTime.dwHighDateTime;
	llUserTime = llUserTime << 32;
	llUserTime += ftUserTime.dwLowDateTime;

	int nThreadTime = (int) (llKernelTime + llUserTime) / 10000;

	return nThreadTime;
}

void	CBaseRender::GetPropertyFromCfg()
{
	if(m_pDrwSurface != NULL)
	{
		return;
	}

	CBaseConfig nConfig;
	TCHAR fileName[MAX_PATH];
	ZeroMemory(fileName , sizeof(fileName));

	if(mstrWorkPath != NULL)
	{
		_tcscpy(fileName , mstrWorkPath);
	}
	else
	{
		GetModuleFileName ((HMODULE)g_hvoccrrInst , fileName, MAX_PATH);
		VO_PTCHAR pPos = vostrrchr(fileName, _T('/'));
		if (pPos == NULL)
			pPos = vostrrchr(fileName, _T('\\'));
		VO_S32 nPos = pPos - fileName;
		fileName[nPos+1] = _T('\0');
	}

	_tcscat(fileName , _T("DDraw.cfg"));

	
	nConfig.Open(fileName);

	m_bOverlay = nConfig.GetItemValue("DDraw" , "Overlay" , 0);
	m_bOverride = nConfig.GetItemValue(("DDraw") , ("Override") , 0);

	int bOverlay = m_bOverlay ? 1 : 0;
	int bOverride = m_bOverride ? 1 : 0;

	VOLOGI("Overlay : %d   Override : %d " , bOverlay , bOverride);

	memset(&m_ddOverlayFX,0,sizeof(m_ddOverlayFX));
	m_ddOverlayFX.dwSize = sizeof(m_ddOverlayFX);
	m_ddOverlayFX.dwAlphaConstBitDepth = 8;
	m_ddOverlayFX.dwAlphaConst =  nConfig.GetItemValue(("DDraw") , ("AlphaConst") , 0xFF);
	m_ddOverlayFX.dckSrcColorkey.dwColorSpaceLowValue	= nConfig.GetItemValue(("DDraw") , ("ColorkeyLow") , RGB (16, 0, 16));
	m_ddOverlayFX.dckSrcColorkey.dwColorSpaceHighValue	= nConfig.GetItemValue(("DDraw") , ("ColorKeyHeight") , RGB (16, 0, 16));
}


