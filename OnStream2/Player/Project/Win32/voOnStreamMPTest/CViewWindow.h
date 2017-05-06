/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#ifndef __CViewWindow_H__
#define __CViewWindow_H__

#define WM_VIEW_FULLSCREEN		WM_USER+202

//class OnStreamCPlayer;

class CViewWindow
{
public:
	static LRESULT CALLBACK ViewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	CViewWindow(void);
	virtual ~CViewWindow(void);

	virtual bool	CreateWnd (HWND hParent, RECT rcView);
	virtual HWND	GetWnd (void) {return m_hWnd;}

	//virtual void	SetPlayer (OnStreamCPlayer * pPlayer) {m_pPlayer = pPlayer;}

	virtual void	SetFullScreen (void);

	virtual bool	Reflush();
protected:
	virtual bool	ResizeVideoWindow (void);
protected:
	HWND			m_hWnd;
	RECT			m_rcView;

	TCHAR			m_szClassName[64];
	TCHAR			m_szWindowName[64];

	bool			mbIsFullScreen;
};
#endif //__CViewWindow_H__