#include "CFilterVideoRender.h"
#include "initguid.h"
#include "voGuids.h"
#include "FilterVideoRender_Guids.h"
#include "CVideoWindow.h"
#include "CBaseDraw.h"
#include "dvdmedia.h"
#include "voLog.h"

CFilterVideoRender::CFilterVideoRender(TCHAR *pName, LPUNKNOWN pUnk ,REFCLSID RenderClass , HRESULT *phr)
: CBaseRenderer(RenderClass , pName , pUnk , phr)
, mpDraw(NULL)
, m_pQSink(NULL)
, mdwDurPerFrame(0)
{
	mpVideoWindow = new CVideoWindow(this , &mInterfaceLock , TEXT("vo Window") , pUnk , phr);
	mpVideoWindow->AddRef();
}

CFilterVideoRender::~CFilterVideoRender(void)
{
	if(mpVideoWindow != NULL)
	{
		mpVideoWindow->Release();
		mpVideoWindow = NULL;
	}
}

STDMETHODIMP CFilterVideoRender::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	if(riid == IID_IVideoWindow || riid == IID_IBasicVideo2 || riid == IID_IBasicVideo)
	{
		return mpVideoWindow->NonDelegatingQueryInterface(riid , ppv);	
	}
	else if(riid == IID_IFilterVideoRender)
	{
		return GetInterface((IFilterVideoRender *)this , ppv);
	}

	return CBaseRenderer::NonDelegatingQueryInterface(riid , ppv);
}

HRESULT CFilterVideoRender::CheckMediaType(const CMediaType *pMT)
{
	if(pMT->majortype != MEDIATYPE_Video)
		return E_FAIL;

	if(pMT->subtype == MEDIASUBTYPE_WYUV 
		|| pMT->subtype == MEDIASUBTYPE_YV12 
		|| pMT->subtype == MEDIASUBTYPE_NV12
		|| pMT->subtype == MEDIASUBTYPE_YUY2
		|| pMT->subtype == MEDIASUBTYPE_RGB565)
		return S_OK;

	return E_FAIL;
}

 HRESULT CFilterVideoRender::BreakConnect()
 {
	 if (m_pQSink) {
		 m_pQSink->Release();
		 m_pQSink = NULL;
	 }

	 return CBaseRenderer::BreakConnect();
 }

 HRESULT CFilterVideoRender::SetMediaType(const CMediaType *pmt)
 {
	 if(mpDraw == NULL)
	 {
		 HWND hWnd = mpVideoWindow->GetWindowHWND();
		 mpDraw = new CBaseDraw(hWnd , mpVideoWindow);
		 if(mpDraw == NULL)
			 return E_FAIL;
	 }

	 if(pmt->subtype == MEDIASUBTYPE_WYUV || pmt->subtype == MEDIASUBTYPE_YV12)
	 {
		 mpDraw->SetInputColor(VO_COLOR_YUV_PLANAR420);
	 }
	 else if(pmt->subtype == MEDIASUBTYPE_NV12)
	 {
		 mpDraw->SetInputColor(VO_COLOR_YUV_PLANAR420_NV12);
	 }
	 else if(pmt->subtype == MEDIASUBTYPE_YUY2)
	 {
		 mpDraw->SetInputColor(VO_COLOR_YUV_PLANAR422_12);
	 }
	 else if(pmt->subtype == MEDIASUBTYPE_RGB565)
	 {
		 mpDraw->SetInputColor(VO_COLOR_RGB565_PACKED);
	 }
	 else 
	 {
		 return E_FAIL;
	 }

	 if(pmt->formattype == FORMAT_VideoInfo)
	 {
		VIDEOINFOHEADER *pInfo = (VIDEOINFOHEADER *)pmt->Format();
		mpDraw->SetInputSize(pInfo->bmiHeader.biWidth , pInfo->bmiHeader.biHeight);
		mdwDurPerFrame = pInfo->AvgTimePerFrame / 10000;

		if(mdwDurPerFrame > 1000 || mdwDurPerFrame < 10)
			mdwDurPerFrame = 40;
	 }
	 else if(pmt->formattype == FORMAT_VideoInfo2)
	 {
		 VIDEOINFOHEADER2 *pInfo = (VIDEOINFOHEADER2 *)pmt->Format();
		 mpDraw->SetInputSize(pInfo->bmiHeader.biWidth , pInfo->bmiHeader.biHeight);
		 mdwDurPerFrame = pInfo->AvgTimePerFrame / 10000;
		 if(mdwDurPerFrame > 1000 || mdwDurPerFrame < 10)
			 mdwDurPerFrame = 40;
	 }

	 if(mdwDurPerFrame == 0)
	 {
		 mdwDurPerFrame = 40;
	 }

	 if(mpVideoWindow != NULL)
	 {
		 mpVideoWindow->SetMediaType(pmt);
	 }

	 return CBaseRenderer::SetMediaType(pmt);
 }

 HRESULT CFilterVideoRender::CompleteConnect(IPin *pReceivePin)
 {
	 mpVideoWindow->SetControlWindowPin (m_pInputPin);
	 mpVideoWindow->SetControlVideoPin(m_pInputPin);
	 return CBaseRenderer::CompleteConnect(pReceivePin);
 }

 STDMETHODIMP CFilterVideoRender::Stop()
 {
	 return CBaseRenderer::Stop();
 }

 STDMETHODIMP CFilterVideoRender::Pause()
 {
	 return CBaseRenderer::Pause();
 }

 STDMETHODIMP CFilterVideoRender::Run(REFERENCE_TIME StartTime)
 {
	 return CBaseRenderer::Run(StartTime);
 }

HRESULT CFilterVideoRender::Active()
{
	HWND hwnd = mpVideoWindow->GetWindowHWND();

	if(mpVideoWindow->IsAutoShowEnabled() == TRUE)
	{
		if(m_bAbort == FALSE)
		{
			if(IsWindowVisible(hwnd) == FALSE)
			{
				SetRepaintStatus(FALSE);

				mpVideoWindow->PerformanceAlignWindow();
				mpVideoWindow->DoShowWindow(SW_SHOWNORMAL);
			}
		}
	}

	return CBaseRenderer::Active();
}

HRESULT CFilterVideoRender::Inactive()
{
	return CBaseRenderer::Inactive();
}


 HRESULT CFilterVideoRender::PrepareReceive(IMediaSample *pMediaSample)
 {
	 return CBaseRenderer::PrepareReceive(pMediaSample);
 }

 HRESULT CFilterVideoRender::Receive(IMediaSample *pMediaSample)
 {
	 return CBaseRenderer::Receive(pMediaSample);
 }

 HRESULT CFilterVideoRender::Render(IMediaSample *pMediaSample)
 {
	 SendQuality(pMediaSample);
	 return DoRenderSample(pMediaSample);
 }

 HRESULT CFilterVideoRender::DoRenderSample(IMediaSample *pMediaSample) 
 {
	 if(mpDraw != NULL)
	 {
		 if( mpDraw->DrawImage(pMediaSample))
			 return S_OK;
	 }

	 return E_FAIL;
 }

 HRESULT	CFilterVideoRender::RedrawSample()
 {
	  CAutoLock cSampleLock(&m_RendererLock);
	 if(m_pMediaSample == NULL)
		 return E_FAIL;

	 m_pMediaSample->AddRef();
	 HRESULT hr = DoRenderSample(m_pMediaSample);

	 m_pMediaSample->Release();

	return hr;
 }

 STDMETHODIMP CFilterVideoRender::SetZoomMode(VO_IV_ZOOM_MODE nMode)
 {
	 if(mpDraw != NULL)
	 {
		return  mpDraw->SetZoomMode(nMode);
	 }

	 return E_FAIL;
 }

 STDMETHODIMP CFilterVideoRender::SetRotate( VO_IV_RTTYPE nRtType)
 {
	 if(mpDraw != NULL)
	 {
		 return mpDraw->SetRotate(nRtType);
	 }

	 return E_FAIL;
 }

 STDMETHODIMP CFilterVideoRender::SetParam( int nID, int nValue)
 {
	 return E_FAIL;
 }

 STDMETHODIMP CFilterVideoRender::GetParam( int nID, int * pValue)
 {
	 if(mpDraw != NULL)
	 {
		 return mpDraw->GetParam(nID , pValue);
	 }

	 return E_FAIL;
 }

 STDMETHODIMP CFilterVideoRender::SetSink( IQualityControl * piqc)
 {
	 m_pQSink = piqc;

	 return NOERROR;
 }

 STDMETHODIMP CFilterVideoRender::Notify( IBaseFilter * pSelf, Quality q)
 {
	 return S_OK;
 }

HRESULT CFilterVideoRender::SendQuality(IMediaSample *pSample)
{
	Quality q;

	LONGLONG llStart , llStop;
	HRESULT hr = pSample->GetTime(&llStart , &llStop);
	if(FAILED(hr))
		return hr;

	REFERENCE_TIME trRealStream;     // the real time now expressed as stream time.
	REFERENCE_TIME trLate;

	m_pClock->GetTime(&trRealStream);
	trRealStream -= m_tStart;        // convert to stream time (this is a reftime)

	trLate = trRealStream - llStart;
	q.TimeStamp = llStart;
	q.Late = -trLate;

	if(trLate >= 0)
	{
		q.Type = Flood;
		q.Proportion = 1000;
	}
	else
	{
		q.Type = Famine;
		DWORD late = abs(trLate) / 10000;

		q.Proportion = (mdwDurPerFrame + late) * 1000 / mdwDurPerFrame;
	}


	if (m_pQSink==NULL) {
		// Get our input pin's peer.  We send quality management messages
		// to any nominated receiver of these things (set in the IPin
		// interface), or else to our source filter.

		IQualityControl *pQC = NULL;
		IPin *pOutputPin = m_pInputPin->GetConnected();
		ASSERT(pOutputPin != NULL);

		// And get an AddRef'd quality control interface

		HRESULT hr = pOutputPin->QueryInterface(IID_IQualityControl,(void**) &pQC);
		if (SUCCEEDED(hr)) {
			m_pQSink = pQC;
		}
	}
	if (m_pQSink) {
		return m_pQSink->Notify(this,q);
	}

	return S_FALSE;
}