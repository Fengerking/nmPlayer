/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		GDITextBlend.cpp

Contains:	GDITextBlend class file

Written by:	Leon Huang

Change History (most recent first):
2011-01-06		Leon			Create file
*******************************************************************************/
#include "GDITextBlend.h"

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

CGDITextBlend::CGDITextBlend(void)
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

CGDITextBlend::~CGDITextBlend(void)
{
	DeleteDC (m_hMemDC);
	ReleaseDC ((HWND)m_hView, m_hWinDC);
	if(m_pTextBuffer ) delete []m_pTextBuffer;
	if(m_pMemBmp) DeleteObject(m_pMemBmp);
}
VO_S32 CGDITextBlend::SetFont(VOEDT_TEXT_FORMAT *textFormat)
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
		value);//m_sTextFormat.cFont);//m_strFontName为TTF字体名
	SelectObject (m_hMemDC, newfont);
	DeleteObject(newfont);

	return VO_ERR_NONE;
}

VO_S32 CGDITextBlend::Init (VO_PTR hView)
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
VO_S32 CGDITextBlend::BlendText(VO_PBYTE buffer,VO_VIDEO_FORMAT vf,VO_U32 nLeft,VO_U32 nTop)
{
		
	return VO_ERR_NONE;
}
VO_S32 CGDITextBlend::CreateTextBuffer(VO_CHAR* text_x, VOEDT_IMG_BUFFER *ppData)
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

//VO_S32 CGDITextBlend::CreateTextBuffer(VO_TCHAR* text, VO_PBYTE *ppData)
//{
//	VO_S32 nRc = VO_ERR_FAILED;
//	VO_S32 len = sizeof(text);
//	if(m_sTextFormat.nSize<=0 && len <= 0) return nRc;
//
//	int nHeight=-MulDiv(m_sTextFormat.nSize,       GetDeviceCaps(m_hMemDC,       LOGPIXELSY),72);
//	int bmpWidth = m_sTextFormat.nSize * len;
//	int bmpHeight = abs(nHeight);
//	int biBitCount = 24;
//	int lineByte=(bmpWidth*biBitCount/8+3)/4*4;
//	BITMAPINFOHEADER head;
//	
//
//	if(m_pMemBmp) DeleteObject(m_pMemBmp);
//	m_pMemBmp = CreateCompatibleBitmap(m_hWinDC, bmpWidth, bmpHeight);
//	m_hOldBmp = (HBITMAP)SelectObject (m_hMemDC, m_pMemBmp);
//	HFONT newfont;   
//	newfont = CreateFont(nHeight,
//									0,
//									0,
//									0,
//									FW_NORMAL,
//									FALSE,
//									FALSE,
//									0,
//									DEFAULT_CHARSET,
//									OUT_CHARACTER_PRECIS,
//									CLIP_CHARACTER_PRECIS,
//									DEFAULT_QUALITY,
//									DEFAULT_PITCH|FF_DONTCARE,
//									L"微软雅黑");//m_sTextFormat.cFont);//m_strFontName为TTF字体名
//	SelectObject (m_hMemDC, newfont);
//	//把图像的数据绘制到兼容位图上
//	SetBkMode(m_hMemDC,TRANSPARENT);
//	SetTextColor(m_hMemDC,RGB(255, 0, 0));
////	TCHAR textx[ ] = L"人民广场";
//
//	TextOut(m_hMemDC, 0, 0, text, sizeof(text));
//
//	head.biSize   = sizeof(BITMAPINFOHEADER);
//	head.biWidth   = bmpWidth;
//	head.biHeight   = bmpHeight;
//	head.biPlanes   = 1;
//	head.biBitCount = biBitCount;
//	head.biCompression = BI_RGB;
//	head.biSizeImage = 0;
//	head.biXPelsPerMeter = 0;
//	head.biYPelsPerMeter = 0;
//	head.biClrImportant = 0;
//	head.biClrUsed = 0;
//
//	unsigned char *pTemp = new unsigned char[lineByte*bmpHeight];
//	GetDIBits(m_hMemDC, (HBITMAP)m_pMemBmp, 0, bmpHeight, 
//		(LPVOID)pTemp, (BITMAPINFO*)&head, DIB_RGB_COLORS);
//	
//	VO_VIDEO_BUFFER inbuf;
//	inbuf.Buffer[0] = pTemp;
//	inbuf.Stride[0] = lineByte * bmpHeight;
//	inbuf.ColorType = VO_COLOR_RGB888_PACKED;
//	VOEDT_CLIP_POSITION_FORMAT vfin;
//	vfin.nHeight = bmpHeight;
//	vfin.nWidth = bmpWidth;
//
//	VO_VIDEO_BUFFER *outbuf = &m_pTextBuffer;
//	if(outbuf->Buffer[0])
//	{
//		delete []outbuf->Buffer[0];
//		outbuf->Buffer[0] = NULL;
//	}
//	outbuf->Buffer[0]= NULL;
//	outbuf->ColorType = VO_COLOR_YUV_PLANAR420;
//	m_nHeight = bmpHeight;
//	m_nWidth = bmpWidth;
//	EX_INIT_BUFFER(m_nWidth,m_nHeight,outbuf);
//	outbuf->Stride[0] = m_nWidth;
//	outbuf->Stride[1] = m_nWidth/2;
//	outbuf->Stride[2] = m_nWidth/2;
//	rgb24_to_yuv420p(outbuf->Buffer[0],outbuf->Buffer[2],outbuf->Buffer[1],pTemp,m_nWidth,m_nHeight);
//	unsigned char *tmp = new unsigned char[outbuf->Stride[0]];
//	for(int i =0;i< m_nHeight /2 ;i++)
//	{
//		memcpy( tmp,(outbuf->Buffer[0] +i * outbuf->Stride[0]  ) , outbuf->Stride[0] ) ;
//		memcpy( (outbuf->Buffer[0] +i * outbuf->Stride[0]  ),(outbuf->Buffer[0] +( m_nHeight - i -1)* outbuf->Stride[0]  ) , outbuf->Stride[0] ) ;
//		memcpy( (outbuf->Buffer[0] +( m_nHeight - i-1 )* outbuf->Stride[0]  ) ,tmp, outbuf->Stride[0] ) ;
//	}
//	for(int i =0;i< m_nHeight /4 ;i++)
//	{
//		memcpy( tmp,(outbuf->Buffer[1] +i * outbuf->Stride[1]  ) , outbuf->Stride[1] ) ;
//		memcpy( (outbuf->Buffer[1] +i * outbuf->Stride[1]  ),(outbuf->Buffer[1] +( m_nHeight /2 - i -1)* outbuf->Stride[1]  ) , outbuf->Stride[1] ) ;
//		memcpy( (outbuf->Buffer[1] +( m_nHeight/2 - i-1 )* outbuf->Stride[1]  ) ,tmp, outbuf->Stride[1] ) ;
//	}
//	for(int i =0;i< m_nHeight /4 ;i++)
//	{
//		memcpy( tmp,(outbuf->Buffer[2] +i * outbuf->Stride[2]  ) , outbuf->Stride[2] ) ;
//		memcpy( (outbuf->Buffer[2] +i * outbuf->Stride[2]  ),(outbuf->Buffer[2] +( m_nHeight /2- i  -1)* outbuf->Stride[2]  ) , outbuf->Stride[2] ) ;
//		memcpy( (outbuf->Buffer[2] +( m_nHeight/2 - i -1 )* outbuf->Stride[2]  ) ,tmp, outbuf->Stride[2] ) ;
//	}
//	delete []tmp;
//	//VOEDT_CLIP_POSITION_FORMAT vfout;
//	//vfout.nHeight = m_nHeight;
//	//vfout.nWidth = m_nWidth;
//	//m_pSampleTransformer->DoTransformer(&inbuf, outbuf, vfin, vfout ,0);
//	
//	char bmpwrite[]="2.bmp";
//	FILE *fp=fopen(bmpwrite,"wb");
//
//	BITMAPFILEHEADER fileHead;
//	fileHead.bfType=0x4D42;
//	fileHead.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+lineByte*bmpHeight;
//	fileHead.bfReserved1=0;
//	fileHead.bfReserved2=0;
//	fileHead.bfOffBits=54;
//	fwrite(&fileHead,sizeof(BITMAPFILEHEADER),1,fp);
//
//	fwrite(&head,sizeof(BITMAPINFOHEADER),1,fp);
//	fwrite(pTemp,lineByte*bmpHeight,1,fp);
//	fclose(fp);
//
//
//	
//	return VO_ERR_NONE;
//}
