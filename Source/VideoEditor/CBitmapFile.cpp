/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CBitmapFile.cpp

Contains:	CBitmapFile class file

Written by:	Bangfei Jin
Modify by:	Leon Huang

Change History (most recent first):
2008-05-20		JBF			Create the file from CBitmapReader.h
2011-01-02		Leon			Mofiy for Linux
*******************************************************************************/
//#include <windows.h>
//#include <tchar.h>
#include <stdlib.h>

#include "CBitmapFile.h"
#include "cmnFile.h"
#pragma warning (disable : 4996)


CBitmapFile::CBitmapFile(void)
	: CBaseBitmap ()
{
}

CBitmapFile::~CBitmapFile(void)
{
	delete []m_pData;
}

int CBitmapFile::CreateBitmap (VO_CHAR * inFileName)
{

	VO_U32 dwRead = 0;
	VO_FILE_SOURCE pSource;
	pSource.nFlag = VO_FILE_TYPE_NAME;
	pSource.nLength = 0;
	pSource.nMode = VO_FILE_READ_ONLY;
	pSource.nOffset = 0;
	pSource.nReserve = 0;

	VO_TCHAR maskFile[255];
#ifdef _WIN32
	MultiByteToWideChar (CP_ACP, 0, inFileName, -1, maskFile, sizeof (maskFile));
#else
	memcpy(maskFile ,inFileName,sizeof(maskFile));
#endif
	VOLOGI("Open mask file: %s",maskFile);
	pSource.pSource = (VO_PTR)maskFile;
	VO_PTR hFile = cmnFileOpen(&pSource);
	if(hFile == NULL)
	{
		VOLOGE("Can not open target file: %s",maskFile);
		return -1;
	}
	VO_U32		nFileSize = cmnFileSize(hFile);
	VO_PBYTE pFileData = new VO_BYTE[nFileSize];
	if (pFileData == NULL)
	{
		cmnFileClose(hFile);
		return -1;
	}
	
	dwRead = cmnFileRead(hFile, pFileData, nFileSize);
	cmnFileClose(hFile);
	if (dwRead != nFileSize)
	{
		delete pFileData;
		return -1;
	}

	VO_PBYTE pFileBuffer = pFileData;
	BITMAPFILEHEADER bmpFileInfo;
	memcpy (&bmpFileInfo, pFileBuffer, sizeof (BITMAPFILEHEADER));
	pFileBuffer += sizeof (BITMAPFILEHEADER);

	BITMAPINFOHEADER	bmpHeader;
	memcpy (&bmpHeader, pFileBuffer, sizeof (BITMAPINFOHEADER));
	pFileBuffer += sizeof (BITMAPINFOHEADER);

	int nBmpWidth = bmpHeader.biWidth;
	int nBmpHeight = bmpHeader.biHeight;
	VOLOGI("bmpinfo (%d x %d) headersize:%d , infoheadersize:%d)",nBmpWidth,abs(nBmpHeight),sizeof (BITMAPFILEHEADER),sizeof (BITMAPINFOHEADER));

	// If bmiHeader.biClrUsed is zero we have to infer the number
	// of colors from the number of bits used to specify it.
	int nColors = bmpHeader.biClrUsed ? bmpHeader.biClrUsed : 1 << bmpHeader.biBitCount;
	if (bmpHeader.biBitCount >= 24)
		nColors = 0;
	else if (bmpHeader.biBitCount == 16)
		nColors = 3;
	m_nBits = bmpHeader.biBitCount;

	m_nBmpSize = sizeof (BITMAPINFOHEADER) + sizeof (RGBQUAD) * nColors;
	if (m_pBmpInfo != NULL)
		delete [](VO_PBYTE)m_pBmpInfo;
	m_pBmpInfo = (BITMAPINFO *) new VO_BYTE[m_nBmpSize];
	memcpy (m_pBmpInfo, &bmpHeader, sizeof (BITMAPINFOHEADER));
	if (nColors > 0)
		memcpy (m_pBmpInfo->bmiColors, pFileBuffer, sizeof (RGBQUAD) * nColors);

	m_nWidth = nBmpWidth;
	m_nHeight = nBmpHeight;

	int	nFileLineLen = ((m_pBmpInfo->bmiHeader.biBitCount / 8) * nBmpWidth + 3) & ~3;
	int	nBmpLineLen = nFileLineLen;
	VO_U32 dwImageSize = nFileLineLen * m_nHeight;

//	HDC hDC = GetDC (NULL);
//	m_pHandle = CreateDIBSection (hDC, m_pBmpInfo, DIB_RGB_COLORS, (void **)&, NULL, 0);
//	ReleaseDC (NULL, hDC);
	m_pData = new VO_BYTE[m_pBmpInfo->bmiHeader.biSizeImage];
	//memcpy(m_pData ,&pFileData[sizeof(BITMAPINFOHEADER) + sizeof(BITMAPINFO)],m_pBmpInfo->bmiHeader.biSize);
	for (int nHeight = 0; nHeight < m_nHeight; nHeight++)
	{
		pFileBuffer = pFileData + bmpFileInfo.bfOffBits + nFileLineLen * nHeight;
		memcpy (m_pData + (m_nHeight - nHeight - 1) * nBmpLineLen, pFileBuffer, nBmpLineLen);
	}

	if (pFileData != NULL)
		delete []pFileData;

	m_pBmpInfo->bmiHeader.biWidth = m_nWidth;
	m_pBmpInfo->bmiHeader.biHeight = -m_nHeight;
	m_pBmpInfo->bmiHeader.biSizeImage = nBmpLineLen * m_nHeight;
	return 1;
}

