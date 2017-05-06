/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		LinuxTextBlend.cpp

Contains:	LinuxTextBlend class file

Written by:	Leon Huang

Change History (most recent first):
2011-08-02		Leon			Create file
*******************************************************************************/
#ifndef _WIN32
#include "LinuxTextBlend.h"
#include "CLinuxBMPBuffer.h"

#define SCALEBITS            8
#define ONE_HALF             (1 << (SCALEBITS - 1))
#define FIX(x)               ((int) ((x) * (1L<<SCALEBITS) + 0.5))
typedef unsigned char        uint8_t;
void rgb24_to_yuv420p(uint8_t *lum, uint8_t *cb, uint8_t *cr, uint8_t *src, int width, int height)
{
	int wrap, wrap3, x, y;
	int r, g, b, r1, g1, b1;
	uint8_t *p;
	wrap = width;
	wrap3 = width * 3;
	p = src;
	for (y = 0; y < height; y += 2)
	{
		for (x = 0; x < width; x += 2)
		{
			b = p[0];
			g = p[1];
			r = p[2];
			r1 = r;
			g1 = g;
			b1 = b;
			lum[0] = (FIX(0.29900) * r + FIX(0.58700) * g +FIX(0.11400) * b + ONE_HALF) >> SCALEBITS;
			r = p[3];
			g = p[4];
			b = p[5];
			r1 += r;
			g1 += g;
			b1 += b;
			lum[1] = (FIX(0.29900) * r + FIX(0.58700) * g +FIX(0.11400) * b + ONE_HALF) >> SCALEBITS;
			p += wrap3;
			lum += wrap;
			r = p[0];
			g = p[1];
			b = p[2];
			r1 += r;
			g1 += g;
			b1 += b;
			lum[0] = (FIX(0.29900) * r + FIX(0.58700) * g +FIX(0.11400) * b + ONE_HALF) >> SCALEBITS;
			r = p[3];
			g = p[4];
			b = p[5];
			r1 += r;
			g1 += g;
			b1 += b;
			lum[1] = (FIX(0.29900) * r + FIX(0.58700) * g +FIX(0.11400) * b + ONE_HALF) >> SCALEBITS;

			cb[0] = (((- FIX(0.16874) * r1 - FIX(0.33126) * g1 +
				FIX(0.50000) * b1 + 4 * ONE_HALF - 1) >> (SCALEBITS + 2)) + 128);
			cr[0] = (((FIX(0.50000) * r1 - FIX(0.41869) * g1 -
				FIX(0.08131) * b1 + 4 * ONE_HALF - 1) >> (SCALEBITS + 2)) + 128);
			cb++;
			cr++;
			p += -wrap3+2 * 3;
			lum += -wrap + 2;
		}
		p += wrap3;
		lum += wrap;
	}
} 

CLinuxTextBlend::CLinuxTextBlend(void)
:CBaseTextBlend()
,m_hWinDC(NULL)
,m_hMemDC(NULL)
,m_hOldBmp(NULL)
,m_pMemBmp(NULL)
{
	//m_pTextBuffer.Buffer[0] = NULL;
//	targetVFormat.nLeft = 100;
//	targetVFormat.nTop = 400;

}

CLinuxTextBlend::~CLinuxTextBlend(void)
{
	DeleteDC (m_hMemDC);
	ReleaseDC ((HWND)m_hView, m_hWinDC);
	if(m_pTextBuffer ) delete []m_pTextBuffer;
	if(m_pMemBmp) DeleteObject(m_pMemBmp);
}
VO_S32 CLinuxTextBlend::SetFont(VOEDT_TEXT_FORMAT *textFormat)
{
	memcpy(&m_sTextFormat, textFormat, sizeof(VOEDT_TEXT_FORMAT));
	int nRc;
	if(m_sTextFormat.nSize<=0 ) return nRc;

	SetBkMode(m_hMemDC,TRANSPARENT);
	SetTextColor(m_hMemDC,RGB(255, 0, 0));

	int nHeight=-MulDiv(m_sTextFormat.nSize,       GetDeviceCaps(m_hMemDC,       LOGPIXELSY),72);
	HFONT newfont;   

	VO_TCHAR value[255];
	MultiByteToWideChar (CP_ACP, 0, m_sTextFormat.cFont, -1, value, sizeof (value));
	newfont = CreateFont(nHeight,
		0,
		0,
		0,
		FW_NORMAL,
		FALSE,
		FALSE,
		0,
		DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE,
		value);//m_sTextFormat.cFont);//m_strFontNameÎªTTF×ÖÌåÃû
	SelectObject (m_hMemDC, newfont);
	DeleteObject(newfont);

	return VO_ERR_NONE;
}

VO_S32 CLinuxTextBlend::Init (VO_PTR hView)
{

	if (hView != NULL && hView != m_hView)
	{
		if (m_hOldBmp != NULL)
			SelectObject (m_hMemDC, m_hOldBmp);

		if (m_hWinDC != NULL)
		{
			DeleteDC (m_hMemDC);
			ReleaseDC ((HWND)m_hView, m_hWinDC);
		}

		m_hView = hView;

		m_hWinDC = GetDC ((HWND) m_hView);
		m_hMemDC = ::CreateCompatibleDC (m_hWinDC);

	}
	return VO_ERR_NONE;
}
VO_S32 CLinuxTextBlend::BlendText(VO_PBYTE buffer,VO_VIDEO_FORMAT vf,VO_U32 nLeft,VO_U32 nTop)
{
		VO_VIDEO_BUFFER *pImage1 = (VO_VIDEO_BUFFER*)buffer;
		VO_VIDEO_BUFFER *pImage2 = (VO_VIDEO_BUFFER*)&m_pTextBuffer;
		VO_PBYTE ppp = m_pTextBuffer;
		if(!ppp ) return VO_ERR_FAILED;

		VO_S32 left = nLeft;
		VO_S32 top = nTop;

		VO_S32 Y = left + top * vf.Width ;
		VO_S32 UV = left /2+ top/2 * vf.Width/2 ;

		VO_S32 nHeight = m_nHeight;
		VO_S32 nWidth = m_nWidth;

		VO_U32 x = 0;
		VO_U32 xxx =0;
		VO_U32 z =  nWidth  >>1;
		VO_U32 volumn = 0;
		VO_BOOL bmask = VO_FALSE;

		PBYTE pp10 = pImage1->Buffer[0]  + Y;

		PBYTE pp11,pp21,pp12,pp22;
		pp11 = pImage1->Buffer[1] + UV ;
		pp12 = pImage1->Buffer[2] + UV ;

		
		for (VO_U32 nH = 0 ; nH < nHeight; ++ nH)
		{
			int i = 0;
			volumn = nH %2;		
			if(volumn)
				pp11 = pImage1->Buffer[1] + left /2+ (top + nH)/2 * vf.Width/2;
			else	
				pp12 = pImage1->Buffer[2] + left /2+ (top + nH)/2 * vf.Width/2 ;
		
			pp10 = pImage1->Buffer[0]  + Y + nH * vf.Width;
		
			for (VO_U32 nW = 0; nW < nWidth; nW ++)
			{
				bmask= VO_FALSE;

				if(*(ppp++) == 1)
					bmask = VO_TRUE;
				if(bmask)
				{
					pp10[i]  =235 ;
					if(volumn)
						pp11[i>>1]= 128;
					else
						pp12[i>>1]= 0;
				}
				i++;				
			}
		}
	return VO_ERR_NONE;
}
VO_S32 CLinuxTextBlend::CreateTextBuffer(VO_CHAR* text_x, VO_VIDEO_BUFFER *ppData)
{
	VO_S32 nRc = VO_ERR_FAILED;

	if(m_sTextFormat.nSize<=0 ) return nRc;
	VO_TCHAR text[255];
	MultiByteToWideChar (CP_ACP, 0, text_x, -1, text, sizeof (text));
	VO_S32 len = _tcslen(text);
	SIZE fontSize;
	GetTextExtentPoint32(m_hMemDC,text,len,&fontSize);

	int bmpWidth = fontSize.cx ;
	int bmpHeight = fontSize.cy;

	if(m_pMemBmp) DeleteObject(m_pMemBmp);
	m_pMemBmp = CreateCompatibleBitmap(m_hWinDC, bmpWidth, bmpHeight);
	m_hOldBmp = (HBITMAP)SelectObject (m_hMemDC, m_pMemBmp);

	TextOut(m_hMemDC, 0, 0, text, len);
	m_nHeight = bmpHeight;
	m_nWidth = bmpWidth;
	if(m_pTextBuffer ) delete m_pTextBuffer;
	m_pTextBuffer = new BYTE[bmpWidth*bmpHeight];
	unsigned char *pPointer = m_pTextBuffer;
	for (int i=0;i<bmpHeight;i++)
	{
		for (int j=0;j<bmpWidth;j++)
		{
			COLORREF PixelColor=GetPixel(m_hMemDC,j,i);
			if ((PixelColor!=-1) && PixelColor==RGB(0,0,0))
			{
				*(pPointer+0) = 0;
			}
			else
			{
				*pPointer= 1;
			}	
			pPointer ++;
		}
	}
	return VO_ERR_NONE;
}

#endif