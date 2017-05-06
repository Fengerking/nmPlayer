	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBitmapReader.cpp

	Contains:	CBitmapReader class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-02		JBF			Create file

*******************************************************************************/

//#include "stdafx.h"
#include "CBitmapReader.h"
extern HINSTANCE		g_hInst;

//#include "CYUV2RGB24.h"

#ifdef _WIN32_WCE
#define _FUNC _T
#else 
#define _FUNC
#endif // _WIN32_WCE

#define iMASK_COLORS 3
#define SIZE_MASKS (iMASK_COLORS * sizeof(DWORD))

const DWORD bits565[3] = {0X0000F800, 0X000007E0, 0X0000001F,};

CBitmapReader::CBitmapReader(void)
	: m_nBitmaps (1)
	, m_lWidth (0)
	, m_lHeight (0)
	, m_pFileBmpInfo (NULL)
	, m_nBmpInfoSize (0)
	, m_uImageSize (0)
{
	//_tcscpy (m_szFileName, _T(""));

	for (int i = 0; i < MAX_BITMAPS; i++)
	{
		m_pData[i] = NULL;
		m_hBitmap[i] = NULL;
	}
}

CBitmapReader::~CBitmapReader(void)
{
	ReleaseData ();
}

void CBitmapReader::ReleaseData (void)
{
	for (int i = 0; i < MAX_BITMAPS; i++)
	{
		if (m_hBitmap[i] != NULL)
		{
			DeleteObject (m_hBitmap[i]);
			m_hBitmap[i] = NULL;
		}
	}
	ReleaseRawData();

	if (m_pFileBmpInfo != NULL)
	{
		delete [](LPBYTE)m_pFileBmpInfo;
		m_pFileBmpInfo = NULL;
	}
}
void CBitmapReader::ReleaseRawData (void)
{
	for (int i = 0; i < MAX_BITMAPS; i++)
	{
		if (m_pData[i] != NULL)
		{
			delete []m_pData[i];
			m_pData[i] = NULL;
		}
	}
}

bool CBitmapReader::LoadResourceFromID(int nResourceID, int nImageWidth, int nBitmapCount)
{
	ReleaseData ();

	HBITMAP hbitmap = LoadBitmap( g_hInst , MAKEINTRESOURCE(nResourceID) );
	if (hbitmap == NULL)
		return false;

	BITMAP	bmpSize;
	memset (&bmpSize, 0, sizeof (BITMAP));
	::GetObject (hbitmap, sizeof (BITMAP), &bmpSize);

	m_lWidth = nImageWidth;
	m_nBitmaps = nBitmapCount;
	int nBmpWidth = bmpSize.bmWidth;
	int nBmpHeight = bmpSize.bmHeight;

	if (m_lWidth > 0)
	{
		m_nBitmaps = nBmpWidth / m_lWidth;
	}
	else
	{
		m_lWidth = nBmpWidth / m_nBitmaps;
	}
	m_lHeight = nBmpHeight;

	if(m_nBitmaps==1)
	{
		m_hBitmap[0] = hbitmap;
		return true;
	}

	HDC hdc = ::GetDC(::GetDesktopWindow());
	if (hdc == NULL)
		return false;

	for (int j = 0; j < m_nBitmaps; j++)
	{
		m_hBitmap[j] = ::CreateCompatibleBitmap(hdc,m_lWidth,m_lHeight);
		HDC hMemDC1 = ::CreateCompatibleDC(hdc);
		if (hMemDC1 == NULL)
			continue;

		HDC hMemDC2 = ::CreateCompatibleDC(hdc);
		if (hMemDC2 == NULL) {
			::DeleteDC(hMemDC1);
			hMemDC1 = NULL;
			continue;
		}
		HGDIOBJ hOld1 = ::SelectObject(hMemDC1,m_hBitmap[j] );
		HGDIOBJ hOld2 = ::SelectObject(hMemDC2,hbitmap);
		::BitBlt(hMemDC1,0,0,m_lWidth, m_lHeight,hMemDC2,j*m_lWidth,0,SRCCOPY);

		::SelectObject(hMemDC1,hOld1);
		::SelectObject(hMemDC2,hOld2);
		::DeleteDC(hMemDC1);
		hMemDC1 = NULL;
		::DeleteDC(hMemDC2);
		hMemDC2 = NULL;
		if (m_hBitmap[j] == NULL)
		{
			break;
		}
	}
	::ReleaseDC(::GetDesktopWindow(), hdc);
	DeleteObject (hbitmap);
	hbitmap = NULL;

	return true;
}

//bool CBitmapReader::SetFileName(const TCHAR * inFileName, int nImageWidth, int nBitmapCount)
//{
//	if (_tcscmp(m_szFileName, inFileName))
//	{
//		_tcscpy(m_szFileName, inFileName);
//
//		m_lWidth = nImageWidth;
//		m_nBitmaps = nBitmapCount;
//
//		int nLen = _tcslen (m_szFileName);
//		TCHAR * pExt = m_szFileName + nLen - 1;
//		while (*pExt != TEXT ('.'))
//		{
//			pExt--;
//		}
//		pExt++;
//		_tcsupr (pExt);
//
//		if (!_tcscmp (pExt, TEXT("BMP")))
//		{
//			return ReadBitmap();
//		}
//	}
//
//	return true;
//}

//void CBitmapReader::GetFileName(TCHAR * outFileName)
//{
//	_tcscpy(outFileName, m_szFileName);
//}

LPBYTE CBitmapReader::GetBuffer (int nIndex)
{
	if (nIndex < 0 || nIndex > MAX_BITMAPS)
		return NULL;

	return m_pData[nIndex];
}

HBITMAP	CBitmapReader::GetBitmapHandle (int nIndex)
{
	if (nIndex < 0 || nIndex > MAX_BITMAPS)
		return NULL;

	return m_hBitmap[nIndex];
}

unsigned long CBitmapReader::GetWidth(void)
{
	return m_lWidth;
}

unsigned long CBitmapReader::GetHeight(void)
{
	return m_lHeight;
}

unsigned long CBitmapReader::GetBufferSize(void)
{
	return m_uImageSize;
}

unsigned long CBitmapReader::GetBitmapBytePerPixel(void)
{
	if (m_pFileBmpInfo == NULL)
		return 0;

	switch (m_pFileBmpInfo->bmiHeader.biBitCount)
	{
	case 8:
		return 1;

	case 16:
		return 2;

	case 24:
		return 3;

	case 32:
		return 4;
	}

	return 4;
}

//unsigned long CBitmapReader::GetBitmapFileLineSize(void)
//{
//	unsigned long lineSize = GetBitmapBytePerPixel() * m_pFileBmpInfo->bmiHeader.biWidth;
//
//	lineSize = (lineSize + 3) & ~3;
//
//	return lineSize;
//}

unsigned long CBitmapReader::GetBitmapLineDataLen (void)
{
	int nDataLen = GetBitmapBytePerPixel() * m_lWidth;

//#ifdef _WIN32_WCE
//	nDataLen = (nDataLen + 1) & ~1;
//#else
	nDataLen = (nDataLen + 3) & ~3;
//#endif

	return nDataLen;
}

//bool CBitmapReader::ReadBitmap(void)
//{
//	ReleaseData ();
//
//	HANDLE		hFile = NULL;
//	DWORD		dwRead = 0;
//
//	hFile = CreateFile (m_szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
//						OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//	if (hFile == INVALID_HANDLE_VALUE)
//		return false;
//
//	int nFileSize = GetFileSize (hFile, NULL);
//	LPBYTE pFileData = new BYTE[nFileSize];
//	if (pFileData == NULL)
//	{
//		CloseHandle (hFile);
//		return false;
//	}
//
//	ReadFile (hFile, pFileData, nFileSize, &dwRead, NULL);
//	CloseHandle (hFile);
//	if (dwRead != nFileSize)
//		return false;
//
//	LPBYTE pFileBuffer = pFileData;
//	BITMAPFILEHEADER bmpFileInfo;
//	memcpy (&bmpFileInfo, pFileBuffer, sizeof (BITMAPFILEHEADER));
//	pFileBuffer += sizeof (BITMAPFILEHEADER);
//
//	BITMAPINFOHEADER	bmpHeader;
//	memcpy (&bmpHeader, pFileBuffer, sizeof (BITMAPINFOHEADER));
//	pFileBuffer += sizeof (BITMAPINFOHEADER);
//
//	int nBmpWidth = bmpHeader.biWidth;
//	int nBmpHeight = bmpHeader.biHeight;
//
//	// If bmiHeader.biClrUsed is zero we have to infer the number
//	// of colors from the number of bits used to specify it.
//	int nColors = bmpHeader.biClrUsed ? bmpHeader.biClrUsed : 1 << bmpHeader.biBitCount;
//	if (bmpHeader.biBitCount >= 24)
//		nColors = 0;
//	else if (bmpHeader.biBitCount == 16)
//		nColors = 3;
//
//	m_nBmpInfoSize = sizeof (BITMAPINFOHEADER) + sizeof (RGBQUAD) * nColors;
//	if (m_pFileBmpInfo != NULL)
//		delete [](LPBYTE)m_pFileBmpInfo;
//	m_pFileBmpInfo = (BITMAPINFO *) new BYTE[m_nBmpInfoSize];
//	memcpy (m_pFileBmpInfo, &bmpHeader, sizeof (BITMAPINFOHEADER));
//	if (nColors > 0)
//	{
//		memcpy (m_pFileBmpInfo->bmiColors, pFileBuffer, sizeof (RGBQUAD) * nColors);
//	}
//
//	if (m_lWidth > 0)
//	{
//		m_nBitmaps = nBmpWidth / m_lWidth;
//	}
//	else
//	{
//		m_lWidth = nBmpWidth / m_nBitmaps;
//	}
//	m_lHeight = nBmpHeight;
//
//	int	nBmpLineLen = GetBitmapBytePerPixel () * m_lWidth;
//	m_uImageSize = GetBitmapLineDataLen () * m_lHeight;
//
//	for (int i = 0; i < m_nBitmaps; i++)
//	{
//		m_pData[i] = new BYTE[m_uImageSize];
//	}
//
//	for (int nHeight = 0; nHeight < m_lHeight; nHeight++)
//	{
//		pFileBuffer = pFileData + bmpFileInfo.bfOffBits + GetBitmapFileLineSize () * nHeight;
//		for (int i = 0; i < m_nBitmaps; i++)
//		{
//			memcpy (m_pData[i] + (m_lHeight - nHeight - 1) * GetBitmapLineDataLen (), pFileBuffer, nBmpLineLen);
//			pFileBuffer += nBmpLineLen;
//		}
//	}
//
//	if (pFileData != NULL)
//		delete []pFileData;
//
//	m_pFileBmpInfo->bmiHeader.biWidth = m_lWidth;
//	m_pFileBmpInfo->bmiHeader.biHeight = -m_lHeight;
//	m_pFileBmpInfo->bmiHeader.biSizeImage = m_uImageSize;
//
//	for (int j = 0; j < m_nBitmaps; j++)
//	{
//#ifdef _WIN32_WCE
//		BYTE * pData = NULL;
//		HDC hdc = GetDC(NULL);
//		//m_hBitmap[j] = CreateBitmap( m_lWidth,abs( m_lHeight) ,1,GetDeviceCaps(hdc, BITSPIXEL),NULL);
//		m_hBitmap[j] =CreateDIBSection (hdc, m_pFileBmpInfo, DIB_RGB_COLORS, (void **)&pData, NULL, 0);
//		memcpy(pData,m_pData[j],m_pFileBmpInfo->bmiHeader.biSizeImage);
////		m_hBitmap[j] = CreateCompatibleBitmap(hdc,m_lWidth,m_lHeight);
//		//CopyBitmap(m_hBitmap[j], hOld, hdc,m_lWidth,m_lHeight);
//		//::DeleteObject(hOld);
//		::ReleaseDC(NULL,hdc);
//		//m_hBitmap[j] = CreateBitmap (m_lWidth, abs(m_lHeight), 1, bmpHeader.biBitCount, m_pData[j]);
//		//m_hBitmap[j] = CreateBitmapFromPointer(m_pFileBmpInfo, GetBitmapLineDataLen (), (PVOID)m_pData[j]);
//#else
//		HDC hdc = CreateDC (_T("DISPLAY"), NULL, NULL, NULL);
//		m_hBitmap[j] = CreateDIBitmap (hdc, &m_pFileBmpInfo->bmiHeader, CBM_INIT, m_pData[j], m_pFileBmpInfo, DIB_RGB_COLORS);
//		DeleteDC (hdc);
//#endif //_WIN32_WCE
//
//		if (m_hBitmap[j] == NULL)
//		{
//			//MessageBox (NULL, _T("Create the bitmap handle failed!"), _T("Error"), MB_OK);
//			return false;
//		}
//	}
//
//	return true;
//}
//
//HRGN CBitmapReader::CreateRgn (COLORREF clrMask)
//{
//	if (m_pData[0] == NULL)
//		return false;
//
//	HRGN		hRgnBmp = NULL;
//
//	LPBYTE		pData =	GetBuffer (0);
//	int			nLine = GetBitmapLineDataLen ();
//	int			nPixel = GetBitmapBytePerPixel ();
//	int *		pColorTable = (int *)((LPBYTE)m_pFileBmpInfo + sizeof (BITMAPINFOHEADER));
//
//	COLORREF	clrMasked;
//	UINT		nHeight;
//	UINT		nWidth;
//
//	for (nHeight = 0; nHeight < GetHeight (); nHeight++)
//	{
//		pData = GetBuffer (0) + nHeight * nLine;
//		int	 nStart = -1;
//
//		for (nWidth = 0; nWidth < GetWidth (); nWidth++)
//		{
//			clrMasked = 0;
//			if (nPixel >= 3)
//			{
//				clrMasked = *(pData + nWidth * nPixel)<<16;
//				clrMasked += *(pData + nWidth * nPixel + 1) << 8;
//				clrMasked += *(pData + nWidth * nPixel + 2);
//			}
//			else if (nPixel == 1)
//			{
//				clrMasked = pColorTable[*(pData + nWidth)];
//			}
//
//			if (clrMasked != clrMask)
//			{
//				if (nStart == -1)
//				{
//					nStart = nWidth; 
//				}
//			}
//			else
//			{
//				if (nStart >= 0)
//				{
//					if (hRgnBmp == NULL)
//						hRgnBmp = CreateRectRgn (nStart, nHeight, nWidth, nHeight + 1);
//					else
//					{
//						HRGN	rgnPixel = CreateRectRgn (nStart, nHeight, nWidth, nHeight + 1);
//						CombineRgn (hRgnBmp, hRgnBmp, rgnPixel, RGN_OR);
//						DeleteObject (rgnPixel);
//					}
//				}
//
//				nStart = -1;
//			}
//		}
//
//		if (nStart >= 0)
//		{
//			if (hRgnBmp == NULL)
//				hRgnBmp = CreateRectRgn (nStart, nHeight, nWidth, nHeight + 1);
//			else
//			{
//				HRGN	rgnPixel = CreateRectRgn (nStart, nHeight, nWidth, nHeight + 1);
//				CombineRgn (hRgnBmp, hRgnBmp, rgnPixel, RGN_OR);
//				DeleteObject (rgnPixel);
//			}
//		}
//	}
//
//	return hRgnBmp;
//}
//
//void CBitmapReader::SaveToBitmapFile(HBITMAP hBitmap,  const TCHAR* strFile )
//{
//#ifdef _WIN32_WCE

	//HDC hdc = ::GetDC (NULL);

	//BITMAP	bmpSize;
	//memset (&bmpSize, 0, sizeof (BITMAP));
	//::GetObject (hBitmap, sizeof (BITMAP), &bmpSize);

	//int size = sizeof(BITMAPINFOHEADER);
	//size += SIZE_MASKS; // for RGB bitMask;

	//LPBYTE pBmpInfoBuf = new BYTE[size];
	//memset (pBmpInfoBuf, 0, size);
	//BITMAPINFO * pBmpInfo = (BITMAPINFO *)pBmpInfoBuf;

	//pBmpInfo->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER) + SIZE_MASKS;
	//pBmpInfo->bmiHeader.biWidth			= bmpSize.bmWidth;
	//pBmpInfo->bmiHeader.biHeight		= bmpSize.bmHeight;

	//pBmpInfo->bmiHeader.biBitCount		= 16; 
	//pBmpInfo->bmiHeader.biCompression	= BI_BITFIELDS;
	//pBmpInfo->bmiHeader.biXPelsPerMeter	= 0;
	//pBmpInfo->bmiHeader.biYPelsPerMeter	= 0;

	//DWORD *	pBmiColors = (DWORD *)((LPBYTE)pBmpInfo + sizeof(BITMAPINFOHEADER));
	//for (int i = 0; i < 3; i++)
	//	*(pBmiColors + i) = bits565[i];

	//pBmpInfo->bmiHeader.biPlanes		= 1;
	//pBmpInfo->bmiHeader.biSizeImage		= bmpSize.bmWidthBytes * bmpSize.bmHeight;

	//LPBYTE	pBmpBuffer = NULL;//new BYTE[bmpSize.bmWidthBytes * bmpSize.bmHeight];
	////HBITMAP	pNewBmp = CreateBitmapFromPointer(pBmpInfo, bmpSize.bmWidthBytes, (PVOID)pBmpBuffer);
	//HBITMAP	pNewBmp = CreateDIBSection (hdc, pBmpInfo, DIB_RGB_COLORS, (void **)&pBmpBuffer, NULL, 0);

	//HDC hMemDC =  CreateCompatibleDC (hdc);
	//HDC hMemDC2 =  CreateCompatibleDC (hdc);

	//HBITMAP hOldBmp = (HBITMAP) SelectObject (hMemDC2, hBitmap);
	//HBITMAP hOldMemBmp = (HBITMAP) SelectObject (hMemDC, pNewBmp);

	//BitBlt (hMemDC, 0, 0, bmpSize.bmWidth, bmpSize.bmHeight, hMemDC2, 0, 0, SRCCOPY);

	//SelectObject (hMemDC2, hOldBmp);
	//SelectObject (hMemDC, hOldMemBmp);

	//BITMAPFILEHEADER bmpFileInfo;
	//memset (&bmpFileInfo, 0, sizeof (BITMAPFILEHEADER));
	//bmpFileInfo.bfType = 0x4D42;
	//bmpFileInfo.bfSize = sizeof (BITMAPFILEHEADER) + size + pBmpInfo->bmiHeader.biSizeImage;
	//bmpFileInfo.bfOffBits = sizeof (BITMAPFILEHEADER) + size;

	//pBmpInfo->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);

	//HANDLE hFile = CreateFile (strFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	//DWORD dwWrite = 0;
	//WriteFile (hFile, &bmpFileInfo, sizeof (BITMAPFILEHEADER), &dwWrite, NULL);
	//WriteFile (hFile, pBmpInfoBuf, size, &dwWrite, NULL);
	//WriteFile (hFile, pBmpBuffer, pBmpInfo->bmiHeader.biSizeImage, &dwWrite, NULL);
	//CloseHandle (hFile);

	//delete []pBmpInfoBuf;
	////delete []pBmpBuffer;
	//DeleteObject (pNewBmp);
	//DeleteDC (hMemDC);
	//DeleteDC (hMemDC2);
	//::ReleaseDC (NULL, hdc);
//#endif // _WIN32_WCE
//}
