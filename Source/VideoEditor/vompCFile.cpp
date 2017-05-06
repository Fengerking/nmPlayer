	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		vompCFile.cpp

	Contains:	memory operator implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-15		JBF			Create file

*******************************************************************************/
//#include <windows.h>

#include "stdlib.h"
#include "stdio.h"
#include "wchar.h"
#include "string.h"
#include "fcntl.h"
#include "sys/stat.h"

//#include <tchar.h>

#include "voIndex.h"
#include "vompCFile.h"

#include "voLog.h"

int	vompReadAt (void * pUserData, long long llPos, unsigned char * pBuffer, int nSize)
{
	vompCBaseFileOP * pFileOP = (vompCBaseFileOP *)pUserData;
	if (pFileOP == NULL)
		return 0;

	if (llPos >= 0)
		pFileOP->Seek (llPos);

	return pFileOP->Read (pBuffer, nSize);
}

int vompWriteAt (void * pUserData, long long llPos, unsigned char * pBuffer, int nSize)
{
	vompCBaseFileOP * pFileOP = (vompCBaseFileOP *)pUserData;
	if (pFileOP == NULL)
		return 0;

	if (llPos >= 0)
		pFileOP->Seek (llPos);

	return pFileOP->Write (pBuffer, nSize);
}

int	vompGetSize (void * pUserData, long long  * pPos)
{
	vompCBaseFileOP * pFileOP = (vompCBaseFileOP *)pUserData;
	if (pFileOP == NULL)
		return 0;

	*pPos = pFileOP->Size ();

	return 0;
}

vompCBaseFileOP::vompCBaseFileOP(void)
{
	m_hFile = NULL;
}

vompCBaseFileOP::~vompCBaseFileOP(void)
{
	Close ();
}

int vompCBaseFileOP::Open (const char * pSource)
{
	Close ();

#ifdef _WIN32
	m_hFile = (FILE *)CreateFile((TCHAR *)pSource, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, (DWORD) 0, NULL);
#else
	m_hFile = fopen (pSource, "rb");
#endif // _WIN32

	if (m_hFile != NULL)
	{
		m_llSize = Size ();
		return 0;
	}
	else
	{
		return -1;
	}
}

int vompCBaseFileOP::Read (unsigned char * pBuffer, int uSize)
{
	if (m_hFile == NULL)
		return -1;

#ifdef _WIN32
	DWORD nRead = 0;
	ReadFile ((HANDLE)m_hFile, pBuffer, uSize, &nRead, NULL);
#else
	int nRead = fread(pBuffer, 1, uSize, m_hFile);
#endif // _WIN32

//	VOLOGI ("Need to Read %d, Get %d, Pos %d, Len %d", uSize, nRead, (int)m_llPos, (int)m_llSize);

	return nRead;
}

int vompCBaseFileOP::Write (unsigned char * pBuffer, int uSize)
{
	if (m_hFile == NULL)
		return -1;

	int uWrite = fwrite(pBuffer,1, uSize, m_hFile);

	return uWrite;
}

int vompCBaseFileOP::Flush (void)
{
	if (m_hFile == NULL)
		return -1;

	return 0;
}

long long vompCBaseFileOP::Seek (long long nPos)
{
	if (m_hFile == NULL)
		return -1;

	m_llPos = nPos;

#ifdef _WIN32
	VO_S32 lPos = (long)nPos;
	VO_S32 lHigh = (long)(nPos >> 32);

	return SetFilePointer ((HANDLE)m_hFile, lPos, &lHigh, FILE_BEGIN);
#else
	if (fseek(m_hFile, nPos, SEEK_SET) < 0)
		return -1;

	int lpos = ftell(m_hFile);
	if (lpos < 0)
		return -1;

	return (int)lpos;
#endif // _WIN32
}

long long vompCBaseFileOP::Size (void)
{
	return FileSize ();
}

int vompCBaseFileOP::Close (void)
{
	if (m_hFile != NULL)
	{
#ifdef _WIN32
		CloseHandle ((HANDLE)m_hFile);
#else
		fclose(m_hFile);
#endif //_WIN32
		m_hFile = NULL;
	}

	return 0;
}

long long vompCBaseFileOP::FileSize (void)
{
	if (m_hFile == NULL)
		return -1;

#ifdef _WIN32
	VO_S32 lSize = 0;
	VO_S32 lHigh = 0;

	VO_S64 sSize = 0;

	lSize = GetFileSize ((HANDLE)m_hFile, (LPDWORD)&lHigh);

	sSize = lHigh;
	sSize = (sSize << 32) + lSize;

	return sSize;
#else
	int r0 = 0, r = 0;
	r0 = ftell(m_hFile);

	r = fseek(m_hFile, 0L, SEEK_END);
	if (-1 == r)
	{
		return -1;
	}

	r = ftell(m_hFile);
	if (-1 == r)
	{
		return -1;
	}

	fseek(m_hFile, r0, SEEK_SET);

	return r;
#endif // _WIN32
}
