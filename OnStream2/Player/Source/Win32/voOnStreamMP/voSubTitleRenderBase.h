#pragma once
#include "voSubtitleType.h"
#include <windows.h>
#include <tchar.h>
#include "COnStreamMPImpl.h"
#include "CGUID.h"
#include "atlconv.h"
//#include "Gdiplusheaders.h"
#include "atlimage.h"
#ifdef _WIN32_WCE
#else
#include "GdiPlusGraphics.h"
#endif // _WIN32_WCE
#include "voCMutex.h"

class CBaseBitmap;

class voSubTitleManager;

class voGraphics
{
public:
	voGraphics();
	~voGraphics();
protected:
#ifndef _WIN32_WCE
	Gdiplus::GdiplusStartupInput m_gdiplusStartupInput; 
	ULONG_PTR m_gdiplusToken;
#endif // _WIN32_WCE
}
;

class voSubTitleRenderBase
{
public:
	voSubTitleRenderBase(voSubTitleManager* manager);
	virtual ~voSubTitleRenderBase(void);
	virtual bool Update(bool bForce = false)=0;
    virtual void Show(bool bShow) = 0;
	int toRealX(int x, bool bComputeScale = false,  bool bAddValidLeft = true);
	int toRealY(int y, bool bComputeScale = false,  bool bAddValidTop = true);
	void reverseString(TCHAR *ch);
	int setTextViewTextInfo(pvoSubtitleTextRowInfo rowInfo, pvoSubtitleTextInfoEntry textInfo, HDC textPaint, HDC strokePaint);
	float getFontSize(pvoSubtitleTextInfoEntry textInfo);
	virtual HWND GetRenderWindow(){return m_hWnd;};
	virtual HWND GetMessageWindow();
	static LRESULT CALLBACK ViewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual bool	CreateWnd (HWND hParent);
	virtual LRESULT	OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual int Start();
	virtual int Stop();
	void		ComputeValidRectOfXYRatio(bool bRedraw);
	bool IsRunning(){return m_bRunning;};
	void GetTextSize(HDC hMemDC, TCHAR* sUse, int nLenStrUse,SIZE *sz);

	virtual void Draw(HDC hdc,void* pParam){};
	/**
	 * Set display region for windowless mode
	 * 
	 * @param rectDisplay the current time
	 */
	virtual void SetDisplayRect(VOOSMP_RECT& rectDisplay){
		if(m_rectDisplay==NULL)
			m_rectDisplay = new RECT;
		m_rectDisplay->left = rectDisplay.nLeft;
		m_rectDisplay->right = rectDisplay.nRight;
		m_rectDisplay->top = rectDisplay.nTop;
		m_rectDisplay->bottom = rectDisplay.nBottom;
	};

protected:
	virtual bool OnDraw(HDC dc, bool bOnlyBitblt = false)=0;
	virtual bool OnDrawToDC(HDC dc,RECT rcClient, bool bOnlyBitblt = false);
	virtual void UpdateBack (HDC dc, RECT rcDisplay, COLORREF clr=RGB(0,0,0), BYTE nTransparent=0);
	HRGN CombineDrawedRgn(HRGN hSrc, int x, int y, int r, int b);
	void SetWidthHeight(int width, int height);
	void ResetetTransparentColor();
	virtual bool OnDrawImage(HDC dc, HDC hMemDC, pvoSubtitleImageInfo imageInfo, HRGN& combine_rgn);
	void DeleteBitmapBK();
	virtual void DrawString(HDC hdc, TCHAR* lpchText, int cchText, LPRECT lprc);
	virtual void BitbltToDC(HDC dc, HDC hMemDC, bool bRecreateBitmap);
	void CheckRectNeedUpdate(RECT* rtIn);
	void ResetRectNeedUpdate();
	void TransferRectByAspect(int& leftTarget, int& topTarget, int& rightTarget, int& bottomTarget, float fXYRate);


protected:
	voSubTitleManager* m_pManager;
	int					m_width;
	int					m_height;
	HFONT				m_font;
	HWND				m_hMessageWnd;
	HWND				m_hWnd;
	TCHAR				m_szClassName[64];
	TCHAR				m_szWindowName[64];
	COLORREF			m_clrBack;
	bool				m_bShow;
	bool				m_bUseDDrawKeyColor;
	bool				m_bEnableCreatRegion;
	bool				m_bWin8;
	bool				m_bRunning;

	RECT				m_rectVaild;
	RECT*				m_rectDisplay;
	RECT*				m_rectNeedUpdated;
	CBaseBitmap*		m_bitmapBK;

	voGraphics			m_gp;
	bool				m_bForceRedrawAll;
	BYTE*				m_pBufferIn;
	bool				m_bBltAfterUpdated;
	bool				m_bHandleSubtitleData;
};
