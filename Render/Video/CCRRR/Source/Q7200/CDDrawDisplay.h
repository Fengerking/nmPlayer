#pragma once

#include "Ddraw.h"

class CDDrawDisplay
{
public:
	CDDrawDisplay(void);
	virtual ~CDDrawDisplay(void);

	virtual HRESULT	Init();
	virtual HRESULT	Uninit();

	virtual HRESULT	CreatePrimarySurface();
	virtual HRESULT CreateOverlaySurface(int nWidth , int nHeight);
	virtual HRESULT	UpdateOverlay(int nLeft , int nTop , int nRight , int nBotton , BOOL bShow);
	virtual void *	LockSurface();
	virtual HRESULT	UnlockSurface();
	virtual BYTE *	GetsurfaceVirAddress(){ return mpSurfaceVir;}

	virtual bool	IsRotateSurface(){return mbRotateSurface;}
	virtual int		GetSurfaceWidth(){ return mnSurfaceWidth;}
	virtual int		GetSurfaceHeight() { return mnSurfaceHeight;}

protected:
	
protected:
	IDirectDraw *			mpDDraw;
	IDirectDrawSurface *	mpPrimarySurface;
	IDirectDrawSurface *	mpOverlaySurface;

	BYTE *					mpOverlayVir;
	BYTE *					mpOverlayPhy;

	BYTE *					mpSurfaceVir;
	BYTE *					mpSurfacePhy;

	bool					mbRotateSurface;
	int						mnSurfaceWidth;
	int						mnSurfaceHeight;

	int						mnVideoWidth;
	int						mnVideoHeight;
};
