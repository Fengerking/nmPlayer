	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CDDVideoRender.h

	Contains:	CDDVideoRender header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/
#ifndef __CDDVideoRender_H__
#define __CDDVideoRender_H__

#ifdef _DDRAW_RENDER
#include <ddraw.h>
#include <mmsystem.h>

#include "..\CBaseVideoRender.h"

typedef VO_S32 (VO_API * VODDRAWRENDERCALLBACK)(VO_PTR pUserData, HDC hDC, LPRECT pRect, VO_S64 llVideoTime);

class CDDVideoRender : public CBaseVideoRender
{
public:
	CDDVideoRender(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP);
	virtual ~CDDVideoRender();

public:
	virtual VO_U32 	Start();
	virtual VO_U32 	Pause();
	virtual VO_U32 	Stop();
	virtual VO_U32	Render(VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait);
	virtual VO_U32	Redraw();
	virtual VO_U32	SetDDrawCallback(VODDRAWRENDERCALLBACK fCallback, VO_PTR pUserData);

	VO_U32 	SetDispRect (VO_PTR hView, VO_RECT * pDispRect, VO_IV_COLORTYPE nColor = VO_COLOR_RGB565_PACKED);
	virtual bool	CreateDD();
protected:
	virtual VO_U32	UpdateSize();
	virtual bool	ReleaseDD();

	virtual void	EraseBackGround();

protected:
	HWND					m_hWnd;
	HDC						m_hWinDC;

	LPDIRECTDRAW7           m_pDD;				// DirectDraw object
	LPDIRECTDRAWSURFACE7    m_pDDSPrimary;		// DirectDraw primary surface

	LPDIRECTDRAWSURFACE7    m_pDDSOffScr;		// DirectDraw YUV off screen surface
	DDSURFACEDESC2			m_sOffScrSurfDesc;	// DirectDraw surface description for YUV off screen surface

	VODDRAWRENDERCALLBACK	m_fDDrawCallback;	// user callback
	VO_PTR					m_pDDCUserData;		// callback user data
	LPDIRECTDRAWSURFACE7	m_pDDSUser;			// DirectDraw user surface, RGB, used for user want to add something like subtitle and so on...
	DDSURFACEDESC2			m_sUserSurfDesc;	// DirectDraw surface description for RGB user surface

	VO_BOOL					m_bSurfaceLost;		// VO_TRUE if DDERR_SURFACELOST, IDirectDrawSurface7::IsLost not accurate for every device

	int						m_nStatus;
	HBRUSH					m_hBlackBrush;
	VO_BOOL					m_bRenderStarted;
};

#endif //_DDRAW_RENDER

#endif // __CDDVideoRender_H__
