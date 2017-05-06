#pragma once

#ifndef _DEBUG

#include "voSubTitleRenderBase.h"
#include "DDraw.h"


typedef HRESULT (* DIRECTDRAWCREATE) (GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter);

class COverlayRender :
	public voSubTitleRenderBase
{
public:
	COverlayRender(voSubTitleManager* manager);
	~COverlayRender(void);

	//bool Render(TCHAR *strSubtitle);

	virtual bool	CreateWnd (HWND hParent);
	virtual bool Update(bool bForce = false);
    virtual void Show(bool bShow);

protected:
	bool Init();
	bool UnInit();
	bool Erase();
	virtual bool OnDraw(HDC dc);

private:

	bool InitDDraw();

private:

	HMODULE						m_hDll;
	DIRECTDRAWCREATE			m_pCreate;

#ifdef _WIN32_WCE
	LPDIRECTDRAW				m_pDD;
	LPDIRECTDRAWSURFACE			m_pDDPrimary;
	LPDIRECTDRAWSURFACE			m_pDDOverlay;
#else
	LPDIRECTDRAW4				m_pDD;
	LPDIRECTDRAWSURFACE4		m_pDDPrimary;
	LPDIRECTDRAWSURFACE4		m_pDDOverlay;
#endif // _WIN32_WCE


	int							m_nSurfaceWidth;
	int							m_nSurfaceHeight;

	RECT						m_rcSurface;
	RECT						m_rcWin;


	HBRUSH						m_brBlack;

	HDC							mhOverlayDC;
};

#endif //_DEBUG