#ifndef __voSubTitleWindowlessRender_H__
#define __voSubTitleWindowlessRender_H__

#include "voSubTitleRenderBase.h"
#include "atlimage.h"

class voSubTitleManager;

class voSubTitleWindowlessRender: public voSubTitleRenderBase
{
public:
	virtual LRESULT	OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	voSubTitleWindowlessRender(voSubTitleManager* manager);
	virtual ~voSubTitleWindowlessRender(void);

	virtual HWND	GetWnd (void) {return m_hWnd;}
	virtual bool	Update(bool bForce = false);

	virtual bool	ResizeWindow (void);
    virtual void	Show(bool bShow);
	virtual bool	CreateWnd (HWND hParent);
	virtual void	Draw(HDC hdc, void* pParam);
	virtual int		Start();
	virtual int		Stop();

protected:
	virtual bool OnDraw(HDC dc, bool bOnlyBitblt = false);
	virtual void UpdateBack (HDC dc, RECT rcDisplay, COLORREF clr=RGB(0,0,0), BYTE nTransparent=0);
	virtual void DrawString(HDC hdc, TCHAR* lpchText, int cchText, LPRECT lprc);
	virtual void BitbltToDC(HDC dc, HDC hMemDC, bool bRecreateBitmap);

protected:

	CImage m_img;



};
#endif //__voSubTitleWindowlessRender_H__