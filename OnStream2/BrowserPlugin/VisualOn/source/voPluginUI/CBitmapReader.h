	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBitmapReader.h

	Contains:	CBitmapReader header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-02		JBF			Create file

*******************************************************************************/

#ifndef __CBitmapReader_H__
#define __CBitmapReader_H__

#include "windows.h"
#include "tchar.h"
#include <commctrl.h>

#define MAX_BITMAPS 8


class  CBitmapReader
{
public:
	CBitmapReader(void);
	virtual ~CBitmapReader(void);

	virtual	void ReleaseData (void);
	virtual	void ReleaseRawData (void);

	//bool SetFileName(const TCHAR * inFileName, int nImageWidth = - 1, int nBitmapCount = 1);
	bool LoadResourceFromID(int nResourceID, int nImageWidth = - 1, int nBitmapCount = 1);

	//void GetFileName(TCHAR * outFileName);
	int	 GetBitmapCount (void) {return m_nBitmaps;}

	BITMAPINFOHEADER *	GetBitmapInfo (void) {return &m_pFileBmpInfo->bmiHeader;}
	LPBYTE				GetBuffer (int nIndex);
	HBITMAP				GetBitmapHandle (int nIndex);

	unsigned long GetWidth(void);
	unsigned long GetHeight(void);
	unsigned long GetBufferSize(void);

	unsigned long GetBitmapBytePerPixel(void);
	//unsigned long GetBitmapFileLineSize(void);
	unsigned long GetBitmapLineDataLen (void);
	//static void SaveToBitmapFile(HBITMAP hBitmap,  const TCHAR* strFile);

	//HRGN		  CreateRgn (COLORREF clrMask);

protected:
	//bool			ReadBitmap(void);


protected:
	//TCHAR				m_szFileName[MAX_PATH];
	int					m_nBitmaps;

	long				m_lWidth;
	long				m_lHeight;

	BITMAPINFO *		m_pFileBmpInfo;
	int					m_nBmpInfoSize;

	ULONG				m_uImageSize;
	LPBYTE				m_pData[MAX_BITMAPS];
	HBITMAP				m_hBitmap[MAX_BITMAPS];

};

#endif // _H_CBitmapReader_