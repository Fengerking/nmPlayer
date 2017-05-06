	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOMXFile.c

	Contains:	memory operator implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-15		JBF			Create file

*******************************************************************************/
#include "voOMXFile.h"

#ifdef _WIN32
#include <windows.h>
#elif defined LINUX || defined _IOS
#  include <errno.h>
#  include <stdio.h>
#  include <unistd.h>
#  include <wchar.h>
#  include <string.h>
#endif // _WIN32

#include <stdlib.h>

OMX_PTR voOMXFileOpen (OMX_STRING pName, VOOMX_FILE_MODE uMode)
{
#ifdef _WIN32
	HANDLE hFile = NULL;

	if (uMode == VOOMX_FILE_READ_ONLY)
		hFile = CreateFile((TCHAR *)pName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, (DWORD) 0, NULL);
	else if (uMode == VOOMX_FILE_WRITE_ONLY)
		hFile = CreateFile((TCHAR *)pName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, (DWORD) 0, NULL);
	else
		hFile = CreateFile((TCHAR *)pName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, (DWORD) 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return 0;

	return hFile;

#elif defined LINUX || defined _IOS
	FILE* pf = NULL;
	char* filename = pName;
	if (uMode == VOOMX_FILE_READ_ONLY)
		pf = fopen(filename, "r");
	else if (uMode == VOOMX_FILE_WRITE_ONLY)
		pf = fopen(filename, "w+");
	else 
		pf = fopen(filename, "a+b");

	if (pf == NULL)
    {
		return 0;
    }

	return pf;
#endif // _WIN32

}

OMX_U32 voOMXFileRead (OMX_PTR pHandle, OMX_U8 * pBuffer, OMX_U32 uSize)
{
	OMX_U32 uRead = 0;

	if (pHandle == 0 || pBuffer == 0)
		return 0;

#ifdef _WIN32
	ReadFile (pHandle, pBuffer, uSize, &uRead, NULL);
#elif defined LINUX  || defined _IOS
	uRead = fread(pBuffer, 1, uSize, (FILE*)pHandle);
	if (uRead < uSize) {
		if (feof((FILE*)pHandle) == 0) {
			perror("fread");
			return -1;
		}
	}
#endif // _WIN32

	return uRead;
}


OMX_U32 voOMXFileWrite (OMX_PTR pHandle, OMX_U8 * pBuffer, OMX_U32 uSize)
{
	OMX_U32 uWrite = 0;

	if (pHandle == 0 || pBuffer == 0)
		return 0;

#ifdef _WIN32
	WriteFile (pHandle, pBuffer, uSize, &uWrite, NULL);
#elif defined LINUX || defined _IOS
	uWrite = fwrite(pBuffer,1, uSize, (FILE*)pHandle);
#endif // _WIN32

	return uWrite;
}

OMX_U32 voOMXFileFlush (OMX_PTR pHandle)
{
#ifdef _WIN32
	return FlushFileBuffers (pHandle);
#elif defined LINUX || defined _IOS
	return fflush((FILE*)pHandle);
#endif // _WIN32
}

OMX_U32 voOMXFileSeek (OMX_PTR pHandle, OMX_S64 nPos, VOOMX_FILE_POS uFlag)
{
#ifdef _WIN32
	OMX_U32 lPos = (long)nPos;
	OMX_U32 lHigh = (long)(nPos >> 32);

	OMX_U32 sMove = FILE_BEGIN;
	if (uFlag == VOOMX_FILE_BEGIN)
		sMove = FILE_BEGIN;
	else if (uFlag == VOOMX_FILE_CURRENT)
		sMove = FILE_CURRENT;
	else
		sMove = FILE_END;

	return SetFilePointer (pHandle, lPos, (PLONG)&lHigh, sMove);
#elif defined LINUX || defined _IOS
	OMX_U32 whence = 0;
	if (uFlag == VOOMX_FILE_BEGIN)
		whence = SEEK_SET;
	else if (uFlag == VOOMX_FILE_CURRENT)
		whence = SEEK_CUR;
	else 
		whence = SEEK_END;

	if (fseek((FILE*)pHandle, nPos, whence) < 0) {
		perror("fseek: ");	
		return -1;
	}

	long lpos = ftell((FILE*)pHandle);
	if (lpos < 0) {
		perror("ftell: ");	
		return -1;
	}

	return (int)lpos;
#endif // _WIN32
}

OMX_S64 voOMXFileSize (OMX_PTR pHandle)
{
#ifdef _WIN32
	OMX_U32 lSize = 0;
	OMX_U32 lHigh = 0;

	OMX_S64 sSize = 0;

	lSize = GetFileSize (pHandle, &lHigh);

	sSize = lHigh;
	sSize = (sSize << 32) + lSize;

	return sSize;
#elif defined LINUX || defined _IOS

	OMX_S32 r0 = 0, r = 0;
	//struct stat st;
	//r = fstat(fileno((FILE*)pHandle), &st);   // not be supported by some devices
	r0 = ftell((FILE*)pHandle);

	r = fseek((FILE*)pHandle, 0L, SEEK_END);
	if (-1 == r){
		printf("fseek on %p Error: %s\n", pHandle, strerror(errno));
		return -1;
  }

	r = ftell((FILE*)pHandle);
	if (-1 == r){
		printf("ftell on %p Error: %s\n", pHandle, strerror(errno));
		return -1;
  }

	fseek((FILE*)pHandle, r0, SEEK_SET);
	//return st.st_size;
	return r;
#endif // _WIN32
}

OMX_U32 voOMXFileClose (OMX_PTR pHandle)
{
#ifdef _WIN32
	CloseHandle (pHandle);
#elif defined LINUX || defined _IOS
	fclose((FILE*)pHandle);
#endif // _WIN32
	return 0;
}

