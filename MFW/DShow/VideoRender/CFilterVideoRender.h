#pragma once

#include "streams.h"
#include "IFilterVideoRender.h"

class CVideoWindow;
class CBaseDraw;

class CFilterVideoRender :	public CBaseRenderer , public IFilterVideoRender , public IQualityControl
{
public:
	CFilterVideoRender( TCHAR *pName, LPUNKNOWN pUnk ,REFCLSID RenderClass , HRESULT *phr);
	virtual ~CFilterVideoRender(void);

	 DECLARE_IUNKNOWN
	 STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
	
	virtual HRESULT DoRenderSample(IMediaSample *pMediaSample) ;
	virtual HRESULT CheckMediaType(const CMediaType *pMT) ;

	STDMETHODIMP Stop();
	STDMETHODIMP Pause();
	STDMETHODIMP Run(REFERENCE_TIME StartTime);

	virtual HRESULT Active();
	virtual HRESULT Inactive();

	// Deal with connections and type changes

	virtual HRESULT BreakConnect();
	virtual HRESULT SetMediaType(const CMediaType *pmt);
	virtual HRESULT CompleteConnect(IPin *pReceivePin);

	virtual HRESULT PrepareReceive(IMediaSample *pMediaSample);
	virtual HRESULT Receive(IMediaSample *pMediaSample);
	virtual HRESULT Render(IMediaSample *pMediaSample);

	HRESULT	RedrawSample();

	//IFilterVideoRender
	STDMETHODIMP SetZoomMode(VO_IV_ZOOM_MODE nMode) ;
	STDMETHODIMP SetRotate( VO_IV_RTTYPE nRtType) ;
	STDMETHODIMP SetParam( int nID, int nValue) ;
	STDMETHODIMP GetParam( int nID, int * pValue);

	// IQualityControl methods - Notify allows audio-video throttling
	STDMETHODIMP SetSink( IQualityControl * piqc);
	STDMETHODIMP Notify( IBaseFilter * pSelf, Quality q);

	 virtual HRESULT SendQuality(IMediaSample *pSample);
protected:
	CVideoWindow *		mpVideoWindow;
	CCritSec			mInterfaceLock;

	CBaseDraw *			mpDraw;

	IQualityControl *	m_pQSink;
	DWORD				mdwDurPerFrame;
};
