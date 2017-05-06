	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		UFileReader.cpp

	Contains:	UFileReader class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2006-11-22		JBF			Create file

*******************************************************************************/
#include <voPDPort.h>
#include <tchar.h>

#include "CHttpDownloadFile.h"
#include "UFileReader.h"

bool UFileReader::g_bCancel = false;
int UFileReader::g_nFlag=0;
UFileReader::UFileReader()
{
}

UFileReader::~UFileReader()
{
}

HVOFILEOPERATE __cdecl UFileReader::voOpenHttpFile(const char* pFileName, VOFILEOPMODE opMode)
{
	CHttpDownloadFile* pFile = new(MEM_CHECK) CHttpDownloadFile;
	if(!pFile)
		return NULL;

	if(!pFile->Open((LPCTSTR)pFileName, opMode))
	{
		delete pFile;
		return NULL;
	}

	return HVOFILEOPERATE(pFile);
}

int __cdecl UFileReader::voSeekFile(HVOFILEOPERATE hFile, long offset, VOFILESEEKPOS fsPos)
{
	if(!hFile || g_bCancel)
		return -1;

	CHttpDownloadFile* pFile = (CHttpDownloadFile*)hFile;
	return pFile->Seek(offset, fsPos);
}

int __cdecl UFileReader::voReadFile(HVOFILEOPERATE hFile, void* buffer, int nSize, int* pReadSize)
{
	if(!hFile || nSize <= 0 || g_bCancel)
		return -1;

	CHttpDownloadFile* pFile = (CHttpDownloadFile*)hFile;
	return pFile->Read((PBYTE)buffer, nSize, (DWORD*)pReadSize);
}

int __cdecl UFileReader::voGetFileSize(HVOFILEOPERATE hFile, int* pHighSize)
{
	if(!hFile)
		return -1;

	CHttpDownloadFile* pFile = (CHttpDownloadFile*)hFile;
	return pFile->Size((DWORD*)pHighSize);
}

int __cdecl UFileReader::voCloseFile(HVOFILEOPERATE hFile)
{
	if(!hFile)
		return -1;

	CHttpDownloadFile* pFile = (CHttpDownloadFile*)hFile;
	pFile->Close();
	delete pFile;

	return 0;
}