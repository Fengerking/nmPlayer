	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CWndRender.h

	Contains:	CWndRender header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-02-15		JBF			Create file

*******************************************************************************/
#ifndef __CWndRender_H__
#define __CWndRender_H__

class CWndRender
{
public:
	static LRESULT CALLBACK ViewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	CWndRender(void);
	virtual ~CWndRender(void);

	virtual bool	CreateWnd (HWND hParent, RECT rcView);
	virtual HWND	GetWnd (void) {return m_hWnd;}

protected:
	HWND			m_hWnd;
	RECT			m_rcView;

	TCHAR			m_szClassName[64];
	TCHAR			m_szWindowName[64];

	HPEN			m_hBKPen;
};
#endif //__CWndRender_H__