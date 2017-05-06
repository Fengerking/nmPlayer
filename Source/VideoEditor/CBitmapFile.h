	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBitmapFile.h

	Contains:	CBitmapFile header file

	Written by:	Bangfei Jin
	Modify by:	Leon Huang

	Change History (most recent first):
	2008-05-20		JBF			Create the file from CBitmapReader.h
	2011-01-02		Leon			Mofiy for Linux
*******************************************************************************/

#ifndef __CBitmapFile_H__
#define __CBitmapFile_H__

#include "CBaseBitmap.h"

class CBitmapFile : public CBaseBitmap
{
public:
	CBitmapFile(void);
	virtual ~CBitmapFile(void);

	virtual int		CreateBitmap (VO_CHAR * inFileName);
	//static bool ResampleBiLinear(BITMAPINFOHEADER * headSource, BYTE* pByteSource, BITMAPINFOHEADER * headDest, BYTE * pByteDest);

};

#endif // _H_CBitmapFile_