#include "CVideoWindow.h"
#include "CBaseDraw.h"
#include "CFilterVideoRender.h"
#include "dvdmedia.h"

CVideoWindow::CVideoWindow(CFilterVideoRender *pFilter,	CCritSec *pInterfaceLock,TCHAR *pName,LPUNKNOWN pUnk,HRESULT *phr)
: CBaseControlWindow(pFilter , pInterfaceLock , pName , pUnk , phr)
, CBaseControlVideo(pFilter , pInterfaceLock , pName , pUnk , phr)
, m_pUnknown( pUnk != 0 ? pUnk : reinterpret_cast<LPUNKNOWN>( static_cast<PNDUNKNOWN>(this) ) )
, m_cRef(0)
, mpDraw(NULL)
, mpVRFilter(pFilter)
{
	ZeroMemory(&mVideoHeader , sizeof(mVideoHeader));

	PrepareWindow();
}

CVideoWindow::~CVideoWindow(void)	
{
}

STDMETHODIMP CVideoWindow::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	if (riid == IID_IVideoWindow) {
		return CBaseControlWindow::NonDelegatingQueryInterface(riid , ppv);
	}
	else if(riid == IID_IBasicVideo2 || riid == IID_IBasicVideo)
	{
		return CBaseControlVideo::NonDelegatingQueryInterface(riid , ppv);
	}
	else
	{
		return CUnknown::NonDelegatingQueryInterface(riid, ppv);
	}
}

STDMETHODIMP_(ULONG) CVideoWindow::NonDelegatingAddRef()
{
	LONG lRef = InterlockedIncrement( &m_cRef );
	return max(ULONG(m_cRef), 1ul);
}

STDMETHODIMP_(ULONG) CVideoWindow::NonDelegatingRelease()
{
	LONG lRef = InterlockedDecrement( &m_cRef );

	if (lRef == 0) {
		m_cRef++;

		delete this;
		return ULONG(0);
	} else {
		return max(ULONG(m_cRef), 1ul);
	}
}

LPTSTR CVideoWindow::GetClassWindowStyles( DWORD *pClassStyles, DWORD *pWindowStyles, DWORD *pWindowStylesEx)
{
	CheckPointer(pClassStyles, NULL);
	CheckPointer(pWindowStyles, NULL);
	CheckPointer(pWindowStylesEx, NULL);

	// Default window styles
#ifdef _WIN32_WCE
	*pClassStyles    = CS_HREDRAW | CS_VREDRAW;
	*pWindowStyles   = WS_CLIPCHILDREN; //WS_OVERLAPPED | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	*pWindowStylesEx = WS_EX_NOANIMATION; //(DWORD) 0;
#else
	*pClassStyles    = CS_HREDRAW | CS_VREDRAW;
	*pWindowStyles   =  WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
	*pWindowStylesEx = WS_EX_OVERLAPPEDWINDOW;
#endif

	return TEXT("MobileVideoRenderer\0");
}

bool	CVideoWindow::SetDraw(CBaseDraw *pDraw)
{
	mpDraw = pDraw;

	return true;
}

LRESULT CVideoWindow::OnReceiveMessage(HWND hwnd, UINT uMsg,WPARAM wParam,LPARAM lParam)
 {
	 switch (uMsg)
	 {
	 case WM_SIZE:
		 {
			 if(mpDraw != NULL)
			 {
				 mpDraw->SizeChanged(true);
				// mpVRFilter->RedrawSample();
		
			 }
		 }
	 case WM_ERASEBKGND:
		 {
			 COLORREF  clrBG = RGB(16,0, 16);
			 HBRUSH hBrush = CreateSolidBrush(clrBG);

			 RECT ClientRect;
			 GetClientRect(m_hwnd,&ClientRect);
			 FillRect(m_hdc , &ClientRect , hBrush);
			 DeleteObject(hBrush);

			 mpVRFilter->RedrawSample();
		 }
			 
	 }
	 return CBaseControlWindow::OnReceiveMessage(hwnd ,uMsg , wParam ,lParam );
 }

HRESULT CVideoWindow::IsDefaultTargetRect()
{
	//RECT SourceRect;

	//VIDEOINFO *pVideoInfo = (VIDEOINFO *) m_pRenderer->m_pInputPin->m_mtIn.Format();
	//BITMAPINFOHEADER *pHeader = HEADER(pVideoInfo);
	//m_pRenderer->m_DrawImage.GetSourceRect(&SourceRect);

	//// Check the coordinates that match the video dimensions.

	//if (SourceRect.left != 0 || SourceRect.top != 0 ||
	//	SourceRect.right != pHeader->biWidth ||
	//	SourceRect.bottom != pHeader->biHeight) {
			return S_FALSE;
	//}
	//return S_OK;

}

HRESULT CVideoWindow::SetDefaultTargetRect()
{
	return E_FAIL;
}

HRESULT CVideoWindow::SetTargetRect(RECT *pTargetRect)
{
	return E_FAIL;
}

HRESULT CVideoWindow::GetTargetRect(RECT *pTargetRect)
{
	return E_FAIL;
}

HRESULT CVideoWindow::IsDefaultSourceRect() 
{
	return E_FAIL;
}

HRESULT CVideoWindow::SetDefaultSourceRect()		
{
	return E_FAIL;
}

HRESULT CVideoWindow::SetSourceRect(RECT *pSourceRect)
{
	return E_FAIL;
}

HRESULT CVideoWindow::GetSourceRect(RECT *pSourceRect) 
{
	return E_FAIL;
}
HRESULT CVideoWindow::GetStaticImage(long *pBufferSize,long *pDIBImage) 
{
	return E_FAIL;
}

VIDEOINFOHEADER *CVideoWindow::GetVideoFormat() 
{
	return &mVideoHeader;
}

 HRESULT		CVideoWindow::SetMediaType(const CMediaType *pMediaType)
 {
	 if(pMediaType->formattype == FORMAT_VideoInfo)
	 {
		 VIDEOINFOHEADER *pInfo = (VIDEOINFOHEADER *)pMediaType->Format();

		 memcpy(&mVideoHeader , pInfo , sizeof(VIDEOINFOHEADER));
	 }
	 else if(pMediaType->formattype == FORMAT_VideoInfo2)
	 {
		 VIDEOINFOHEADER2 *pInfo = (VIDEOINFOHEADER2 *)pMediaType->Format();
		 CopyRect(&mVideoHeader.rcTarget , &pInfo->rcTarget);
		 CopyRect(&mVideoHeader.rcSource , &pInfo->rcSource);
		 mVideoHeader.AvgTimePerFrame = pInfo->AvgTimePerFrame;
		 mVideoHeader.dwBitErrorRate = pInfo->dwBitErrorRate;
		 mVideoHeader.dwBitRate = pInfo->dwBitRate;

		 memcpy(&mVideoHeader.bmiHeader , &pInfo->bmiHeader , sizeof(mVideoHeader.bmiHeader));
	 }
	 return S_OK;
 }