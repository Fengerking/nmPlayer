#ifndef __voSubTitleWndRender_H__
#define __voSubTitleWndRender_H__

#include "voSubTitleRenderBase.h"
class voSubTitleManager;

class voSubTitleWndRender: public voSubTitleRenderBase
{
public:
	virtual LRESULT	OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	voSubTitleWndRender(voSubTitleManager* manager);
	virtual ~voSubTitleWndRender(void);

	virtual HWND	GetWnd (void) {return m_hWnd;}
	virtual bool Update(bool bForce = false);

	virtual bool	ResizeWindow (void);
    virtual void Show(bool bShow);
	//virtual bool  CheckWindowSizeOrPosition();
	virtual void SetDisplayRect(VOOSMP_RECT& rectDisplay);

protected:
	virtual bool OnDraw(HDC dc, bool bOnlyBitblt = false);

protected:


};
#endif //__voSubTitleWndRender_H__