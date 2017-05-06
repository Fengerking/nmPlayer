#include <windows.h>
#include <tchar.h>
#include "CBaseBitmap.h"

#pragma warning (disable : 4996)

CBaseBitmap::CBaseBitmap(void)
	: m_nWidth (0)
	, m_nHeight (0)
	, m_nBits (32)
	, m_pData (NULL)
	, m_pHandle (NULL)
	, m_pBmpInfo (NULL)
	, m_nBmpSize (0)
	, m_bWide (false)
	, m_bCloseHandleWhenRelease(true)
{
}

CBaseBitmap::~CBaseBitmap(void)
{
	Release ();
}

int CBaseBitmap::CreateBitmap (HDC hDC, int nBits, int nWidth, int nHeight)
{
	if (m_nWidth == nWidth && m_nHeight == nHeight && m_pData!= NULL)
		return 0;

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nBits = nBits;

	Release ();
	CreateBitmapInfo ();

	//int nWidth2 = m_nWidth * nBits/ 8;
	int nWidth2 = GetBmpDataWidth();

	m_pHandle = CreateDIBSection (hDC, m_pBmpInfo, DIB_RGB_COLORS, (void **)&m_pData, NULL, 0);

	if(m_pHandle ==NULL || m_pData == NULL)
		return -1;


	return 1;
}
int CBaseBitmap::CreateBitmapInfo (HDC hDC, int nBits, int nWidth, int nHeight)
{
	if (m_nWidth == nWidth && m_nHeight == nHeight && m_pData!= NULL)
		return 0;

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nBits = nBits;

	Release ();
	CreateBitmapInfo ();

	return 1;
}

LPBYTE CBaseBitmap::GetBitmapBuffer (void)
{
	return m_pData;
}

HBITMAP CBaseBitmap::GetBitmapHandle (HDC hDC)
{
	return m_pHandle;
}

bool CBaseBitmap::CreateBitmapInfo (void)
{
	if (m_pBmpInfo != NULL)
		return true;

	if (m_nBits <= 16) 
		m_nBmpSize = sizeof(BITMAPINFOHEADER) + SIZE_MASKS;
	else
		m_nBmpSize = sizeof(BITMAPINFOHEADER);

	m_pBmpInfo = (BITMAPINFO *) new BYTE[m_nBmpSize];
	if(m_pBmpInfo == NULL)
		return false;

	memset (m_pBmpInfo, 0, m_nBmpSize);

	m_pBmpInfo->bmiHeader.biSize		= m_nBmpSize;
	m_pBmpInfo->bmiHeader.biWidth		= m_nWidth;
	m_pBmpInfo->bmiHeader.biHeight		= -m_nHeight;

	m_pBmpInfo->bmiHeader.biBitCount		= m_nBits;
	if (m_nBits <= 16) 
		m_pBmpInfo->bmiHeader.biCompression		= BI_BITFIELDS;
	else
		m_pBmpInfo->bmiHeader.biCompression		= BI_RGB;

	m_pBmpInfo->bmiHeader.biXPelsPerMeter	= 0;
	m_pBmpInfo->bmiHeader.biYPelsPerMeter	= 0;

	if (m_nBits <= 16) 
	{
		DWORD *	pBmiColors = (DWORD *)((LPBYTE)m_pBmpInfo + sizeof(BITMAPINFOHEADER));
		for (int i = 0; i < 3; i++)
    		*(pBmiColors + i) = bits565[i];
	}

	m_pBmpInfo->bmiHeader.biPlanes			= 1;
	m_pBmpInfo->bmiHeader.biSizeImage		= GetBmpDataWidth() * m_nHeight;

	return true;
}
void CBaseBitmap::SetBkColor(COLORREF clr)
{
	if(m_pBmpInfo ==NULL || m_pData == NULL)
		return;

	WORD r,g,b;
	r = GetRValue(clr);
	g = GetGValue(clr);
	b = GetBValue(clr);
	if(m_nBits == 16)
	{
		WORD wdClr = (((WORD)( b>>3))) + (((g >> 2)<<5)) 
			+ (((r>>3)<<11));	
		for(unsigned int i = 0;i<m_pBmpInfo->bmiHeader.biSizeImage;i+=2)
		{
			((WORD*)(m_pData+i))[0] = wdClr;
		}
	}
	else
	{
		if(r == g && g == b)
			memset(m_pData,r,m_pBmpInfo->bmiHeader.biSizeImage);
		else
		{
			BYTE bt[3] = {(BYTE)b,(BYTE)g,(BYTE)r};
			for(int i = 0;i<abs(m_pBmpInfo->bmiHeader.biHeight);i++)
			{
				BYTE * pData = m_pData + this->GetBmpDataWidth() * i;
				if(i == 0)
				{
					for(int j = 0;j<m_pBmpInfo->bmiHeader.biWidth;j++)
					{
						memcpy(pData,bt,3);
						pData+=3;
					}
				}
				else
				{
					memcpy(pData,m_pData,this->GetBmpDataWidth());
				}
			}
		}
	}
}
void CBaseBitmap::Release (void)
{
	if (m_pBmpInfo != NULL)
	{
		delete [](LPBYTE)m_pBmpInfo;
		m_pBmpInfo = NULL;
	}

	if (m_pData != NULL)
	{
		m_pData = NULL;
	}

	if(m_bCloseHandleWhenRelease)
	{
		if (m_pHandle != NULL)
		{
			DeleteObject (m_pHandle);
			m_pHandle = NULL;
		}
	}
}

int CBaseBitmap::GetBmpDataWidth (void)
{
	if (m_pBmpInfo == NULL)
		return -1;

	int nWidth = m_pBmpInfo->bmiHeader.biWidth * m_pBmpInfo->bmiHeader.biBitCount / 8;
	nWidth = (nWidth + 3) & ~3;

	return nWidth;
}

bool CBaseBitmap::ResampleBiLinearGeneral(CBaseBitmap* pSrc, int heightDirect)
{
	//return false;
	BITMAPINFO * infoSource = pSrc->GetBmpInfo();
	BYTE* pByteSource = pSrc->GetBitmapBuffer();
	BITMAPINFO * infoDest = GetBmpInfo();
	BYTE * pByteDest = GetBitmapBuffer();
	BITMAPINFOHEADER headSource = infoSource->bmiHeader; 
	BITMAPINFOHEADER * headDest = &infoDest->bmiHeader;
	bool b = true;
	long newx = headDest->biWidth;
	long newy = abs(headDest->biHeight); 
	if (newx==0 || newy==0) return false;
	if(pByteDest == NULL || pByteSource == NULL)
		return false;
	{
		if(headDest->biBitCount!=32)
			return false;

		int nBytePixel = 4;

		//DWORD tick = GetTickCount();
		DWORD dwEffWidth = headSource.biWidth * headSource.biBitCount / 8;
		dwEffWidth = (dwEffWidth + 3) & ~3;
		DWORD dwEffWidthDest = headDest->biWidth * headDest->biBitCount / 8;
		dwEffWidthDest = (dwEffWidthDest + 3) & ~3;
		int nHeightSrc = abs(headSource.biHeight);

		if(headSource.biWidth == newx && nHeightSrc == newy)
		{
			memcpy(pByteDest,pByteSource,headDest->biSizeImage);
			return true;
		}

		int xScale = headSource.biWidth*1024  / newx;
		int yScale = nHeightSrc*1024 / newy;
		//if(xScale<=1024 || yScale<=1024)
		//	return false;


		long ifX, ifY, ifX1, ifY1, xmax, ymax;
		xmax = headSource.biWidth-1;
		ymax = nHeightSrc-1;

		long* pIfX = new long[newx];
		if(pIfX==NULL)
			return false;
		long* pIfX1 = new long[newx];
		if(pIfX1==NULL)
		{
			delete[] pIfX;
			return false;
		}
		int* pdxInt = new int[newx];
		if(pdxInt==NULL)
		{
			delete[] pIfX;
			delete[] pIfX1;
			return false;
		}
		for(long x=0; x<newx; x++)
		{
			int nTemp2 = x * xScale;
			pIfX[x] = nTemp2/1024;
			pIfX1[x] = min(xmax, pIfX[x]+1);
			pdxInt[x] =nTemp2%1024;
		}

		BYTE* pDataDest = (BYTE*) pByteDest;
		BYTE* pDataSrc = (BYTE*) pByteSource;
		{
			for(long y=0; y<newy; y++){
				int yTemp = y * yScale;
				ifY = yTemp/1024;
				ifY1 = min(ymax, ifY+1);

				int dyInt = yTemp%1024;

				BYTE* pYSource = ((BYTE*)pDataSrc + ifY*dwEffWidth);
				BYTE* pY1Source = ((BYTE*)pDataSrc + ifY1*dwEffWidth);

				int rgbInt[1][4];
				for(long x=0; x<newx; x++){
					ifX = pIfX[x];
					ifX1 = pIfX1[x];
					int dxInt =pdxInt[x];
					// Interpolate using the four nearest pixels in the source
					{
						BYTE* iDst = pYSource + ifX*4;
						BYTE* iDst1 = ((BYTE*)pYSource + ifX1*4);
						BYTE* iDst2 = ((BYTE*)pY1Source + ifX*4);
						BYTE* iDst3 = ((BYTE*)pY1Source + ifX1*4);
						BYTE* wd = ((BYTE*)pDataDest+x*4);
						int nZero = 0;
						if((*(int*)iDst) == 0) nZero++;
						if((*(int*)iDst1) == 0) nZero++;
						if((*(int*)iDst2) == 0) nZero++;
						if((*(int*)iDst3) == 0) nZero++;
						if(nZero>1)
						{
							*(int*)wd = 0;
						}
						else
						{

							//DWORD* pYsrc = pYSource;
							rgbInt[0][0] = iDst[0]   + ((((iDst2[0]   -iDst[0])   * dyInt) >>10));
							rgbInt[0][1] = iDst[1]   + ((((iDst2[1]   -iDst[1]) * dyInt) >>10));
							rgbInt[0][2] = iDst[2]   + ((((iDst2[2]   -iDst[2])  * dyInt) >>10));
							rgbInt[0][3] = iDst[3]   + ((((iDst2[3]   -iDst[3])  * dyInt)>>10));
							// Interpolate in y:
							wd[0] = (BYTE)(rgbInt[0][0] + (((iDst1[0]   + ((((iDst3[0]   -iDst1[0])   * dyInt)>>10))-rgbInt[0][0]) * dxInt)>>10));
							wd[1] = (BYTE)(rgbInt[0][1] + (((iDst1[1]   + ((((iDst3[1]   -iDst1[1]) * dyInt)>>10))-rgbInt[0][1])* dxInt)>>10));
							wd[2] = (BYTE)(rgbInt[0][2] + (((iDst1[2]   + ((((iDst3[2]   -iDst1[2])  * dyInt)>>10))-rgbInt[0][2]) * dxInt)>>10));
							wd[3] = (BYTE)(rgbInt[0][3] + (((iDst1[3]   + ((((iDst3[3]   -iDst1[3])  * dyInt)>>10))-rgbInt[0][3]) * dxInt)>>10));;
						}
					}
				}

				pDataDest+=dwEffWidthDest;
			}
		}

		delete [] pIfX;
		delete [] pIfX1;
		delete [] pdxInt;
		//tick = GetTickCount() - tick;
	//TCHAR  chLog[200];
	//swprintf(chLog,L"tick  = %d\r\n",tick);
	//OutputDebugString(chLog);


		return b;
	}
	return true;
}
HRGN CBaseBitmap::CreateRgn (int clrMask, int nLeft , int nTop)
{
	HRGN		hRgnBmp = NULL;

	LPBYTE		pData =	this->GetBitmapBuffer();
	int			nLine = this->GetBmpDataWidth();
	int			nPixel = 4;

	if(pData && pData[0]!=0)
		return NULL;

	COLORREF	clrMasked;
	int		nHeight;
	int		nWidth;

	for (nHeight = 0; nHeight < this->GetHeight(); nHeight++)
	{
		pData = GetBitmapBuffer() + nHeight * nLine;
		int	 nStart = -1;

		for (nWidth = 0; nWidth < GetWidth (); nWidth++)
		{
			clrMasked = 0;
			clrMasked = *((int*)(pData + nWidth * nPixel));

			if (clrMasked != clrMask)
			{
				if (nStart == -1)
				{
					nStart = nWidth; 
				}
			}
			else
			{
				if (nStart >= 0)
				{
					if (hRgnBmp == NULL)
						hRgnBmp = CreateRectRgn (nStart+nLeft, nHeight+nTop, nWidth+nLeft,  nHeight+nTop + 1);
					else
					{
						HRGN	rgnPixel = CreateRectRgn (nStart+nLeft,  nHeight+nTop, nWidth+nLeft,  nHeight+nTop + 1);
						CombineRgn (hRgnBmp, hRgnBmp, rgnPixel, RGN_OR);
						DeleteObject (rgnPixel);
					}
				}

				nStart = -1;
			}
		}

		if (nStart >= 0)
		{
			if (hRgnBmp == NULL)
				hRgnBmp = CreateRectRgn (nStart+nLeft,  nHeight+nTop, nWidth+nLeft,  nHeight+nTop + 1);
			else
			{
				HRGN	rgnPixel = CreateRectRgn (nStart+nLeft,  nHeight+nTop, nWidth+nLeft,  nHeight+nTop + 1);
				CombineRgn (hRgnBmp, hRgnBmp, rgnPixel, RGN_OR);
				DeleteObject (rgnPixel);
			}
		}
	}

	return hRgnBmp;
}
