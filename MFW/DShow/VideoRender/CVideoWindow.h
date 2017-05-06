#pragma once
#include "streams.h"

class CBaseDraw;
class CFilterVideoRender;

class CVideoWindow :	public CBaseControlWindow , public CBaseControlVideo , public INonDelegatingUnknown
{
public:
	CVideoWindow(CFilterVideoRender *pFilter,	CCritSec *pInterfaceLock,TCHAR *pName,LPUNKNOWN pUnk,HRESULT *phr);
	virtual ~CVideoWindow(void);

	DECLARE_IUNKNOWN
	 STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
	 STDMETHODIMP_(ULONG) NonDelegatingAddRef();
	 STDMETHODIMP_(ULONG) NonDelegatingRelease();
	 LPUNKNOWN GetOwner() const { return m_pUnknown;};

	 virtual LRESULT OnReceiveMessage(HWND hwnd, UINT uMsg,WPARAM wParam,LPARAM lParam); 

	 //CBaseControlWindow
	 virtual LPTSTR GetClassWindowStyles( DWORD *pClassStyles, DWORD *pWindowStyles, DWORD *pWindowStylesEx);

	 //CBaseControlVideo
	 virtual HRESULT IsDefaultTargetRect() ;
	 virtual HRESULT SetDefaultTargetRect();
	 virtual HRESULT SetTargetRect(RECT *pTargetRect);
	 virtual HRESULT GetTargetRect(RECT *pTargetRect);
	 virtual HRESULT IsDefaultSourceRect() ;
	 virtual HRESULT SetDefaultSourceRect();
	 virtual HRESULT SetSourceRect(RECT *pSourceRect);
	 virtual HRESULT GetSourceRect(RECT *pSourceRect) ;
	 virtual HRESULT GetStaticImage(long *pBufferSize,long *pDIBImage) ;
	 virtual VIDEOINFOHEADER *GetVideoFormat() ;

	 bool	SetDraw(CBaseDraw *pDraw);
	 HRESULT		SetMediaType(const CMediaType *pMediaType);
protected:
	LONG				m_cRef;
	CFilterVideoRender* mpVRFilter; 
	CBaseDraw *			mpDraw;
private:
	const LPUNKNOWN m_pUnknown;

	VIDEOINFOHEADER	mVideoHeader;
};
