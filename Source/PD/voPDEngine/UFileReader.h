	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		UFileReader.h

	Contains:	UFileReader header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2006-11-22		JBF			Create file
	2008-04-11		East		Modify for PD

*******************************************************************************/

#ifndef __UFileReader_H__
#define __UFileReader_H__

#include "filebase.h"
enum{
	VOFR_DISABLE_SEEK=1,
	VOFR_SPECIAL_LINUXSERVER = 2,
};
class UFileReader
{
public:
	UFileReader();
	virtual ~UFileReader();

public:
	static 	bool			g_bCancel;
	static		int				g_nFlag;
public:
	static	HVOFILEOPERATE __cdecl voOpenHttpFile(const char* pFileName, VOFILEOPMODE opMode);
	static	int __cdecl voSeekFile(HVOFILEOPERATE hFile, long offset, VOFILESEEKPOS fsPos);
	static	int __cdecl voReadFile(HVOFILEOPERATE hFile, void* buffer, int nSize, int* pReadSize);
	static	int __cdecl voGetFileSize(HVOFILEOPERATE hFile, int* pHighSize);
	static	int __cdecl voCloseFile(HVOFILEOPERATE hFile);
};

#endif //__UFileReader_H__
