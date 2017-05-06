	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseBitmap.h

	Contains:	CBaseBitmap header file

	Written by:	Bangfei Jin
	Modify by:   Leon Huang
	Change History (most recent first):
	2008-05-20		JBF			Create the file from CBitmapReader.h
	2011-02-14		Leon			Modify for Linux
*******************************************************************************/

#ifndef __CBaseBitmap_H__
#define __CBaseBitmap_H__

#define	SIZE_MASKS	12


//#ifdef _WIN32_WCE
//extern "C" {
//	WINGDIAPI HBITMAP WINAPI CreateBitmapFromPointer(CONST BITMAPINFO *	pbmi, int iStride, PVOID pvBits);
//}
//#endif //_WIN32_WCE
#include "videoEditorType.h"

#ifdef _WIN32
#include <Windows.h>
#else
#pragma pack (1)
typedef VO_U32   COLORREF;
typedef VO_U32   *LPCOLORREF;

typedef struct tagRGBQUAD {
	VO_BYTE    rgbBlue;
	VO_BYTE    rgbGreen;
	VO_BYTE    rgbRed;
	VO_BYTE    rgbReserved;
} RGBQUAD;


typedef struct tagBITMAPINFOHEADER{
	VO_U32      biSize;
	VO_S32       biWidth;
	VO_S32       biHeight;
	VO_U16       biPlanes;
	VO_U16       biBitCount;
	VO_U32      biCompression;
	VO_U32      biSizeImage;
	VO_S32       biXPelsPerMeter;
	VO_S32       biYPelsPerMeter;
	VO_U32      biClrUsed;
	VO_U32      biClrImportant;
} BITMAPINFOHEADER;
typedef struct tagBITMAPINFO {
	BITMAPINFOHEADER    bmiHeader;
	RGBQUAD             bmiColors[1];
} BITMAPINFO;

typedef struct tagBITMAPFILEHEADER {
	VO_U16    bfType;
	VO_U32   bfSize;
	VO_U16    bfReserved1;
	VO_U16    bfReserved2;
	VO_U32   bfOffBits;
} BITMAPFILEHEADER;

#define LOBYTE(w)           ((VO_BYTE )(((VO_U32)(w)) & 0xff))

#define GetRValue(rgb)      (LOBYTE(rgb))
#define GetGValue(rgb)      (LOBYTE(((VO_U16)(rgb)) >> 8))
#define GetBValue(rgb)      (LOBYTE((rgb)>>16))

#endif

class CBaseBitmap
{
public:
	CBaseBitmap(void);
	virtual ~CBaseBitmap(void);
	virtual int		CreateBitmap (TCHAR * inFileName);

	virtual VO_PBYTE	GetBitmapBuffer (void);

	virtual int		GetWidth (void) {return m_nWidth;}
	virtual int		GetHeight (void) {return abs (m_nHeight);}

	BITMAPINFO*		GetBmpInfo(){return m_pBmpInfo;};
	bool			IsWide(){return m_bWide;};
	void			SetWide(bool bWide){m_bWide = bWide;};
	void			SetBkColor(COLORREF clr);
	virtual void	Release (void);
	int				GetBmpDataWidth (void);
	void			Rotate(int angle, CBaseBitmap* outBitmap, COLORREF inBackColor);
	bool			m_bCloseHandleWhenRelease;

protected:
	

protected:
	int				m_nWidth;
	int				m_nHeight;
	int				m_nBits;
	
	VO_PBYTE	m_pData;

//	HBITMAP 		m_pHandle;

	BITMAPINFO *	m_pBmpInfo;
	int				m_nBmpSize;

	bool			m_bWide;
};

#endif // _H_CBaseBitmap_
