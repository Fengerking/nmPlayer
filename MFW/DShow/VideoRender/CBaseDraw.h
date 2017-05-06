#pragma once

#include "IFilterVideoRender.h"

class CCCRRRFunc;
class CVideoWindow;

class CBaseDraw
{
public:
	CBaseDraw(HWND hWnd , CVideoWindow *pVW);
	virtual ~CBaseDraw(void);

	virtual bool	SetInputSize(int nWidth , int nHeight);
	virtual bool	SetZoomMode(VO_IV_ZOOM_MODE zm);
	virtual bool	SetRotate(VO_IV_RTTYPE rt);
	virtual bool	SetInputColor(VO_IV_COLORTYPE ct);
	virtual bool	DrawImage(IMediaSample *pSample);
	virtual bool	RenderImage(HBITMAP hBmp);

	void			SizeChanged(bool bValue){mbUpdateSize = bValue;}

	HRESULT 		SetParam (VO_U32 nID, VO_PTR pValue);
	HRESULT			GetParam (VO_U32 nID, VO_PTR pValue);
protected:
	bool			UpdateSize();
	virtual bool	LoadCCRRR();

	bool			CreateBitmapInfo();
	void			SaveRawDataToFile(VO_VIDEO_BUFFER videoBuf , int nVW , int nVH , char *fileName);
protected:
	HWND	mhWnd;
	HDC		mhDC;
	HDC		mhMemoryDC;

	CVideoWindow *mpVideoWindow;

	int		mnInputHeight;
	int		mnInputWidth;
	int		mnInputVideoH;
	int		mnInputVideoW;
	int		mnOutHeight;
	int		mnOutWidth;

	int		mnScreenX;
	int		mnScreenY;

	RECT	mrcWnd;
	RECT	mrcDrawWnd;

	VO_IV_ZOOM_MODE mzmZoomMode;
	VO_IV_RTTYPE	mrtRotateType;
	VO_IV_COLORTYPE	mInputColor;
	VO_IV_COLORTYPE mOutputColor;

	bool			mbUpdateSize;

	CCCRRRFunc *	mpCCRRR;

	//GDI 
	BITMAPINFO *	mpBitmapInfo;
	BYTE *			mpBitmapBuffer;
	int				mnBitmapStride;
	HBITMAP			mhBitmap;

	VO_VIDEO_BUFFER mvbInBuf;
	VO_VIDEO_BUFFER	mvbOutBuf;
};
