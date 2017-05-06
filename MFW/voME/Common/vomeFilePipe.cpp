	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		vomeFilePipe.cpp

	Contains:	memory operator implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-15		JBF			Create file

*******************************************************************************/
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#elif defined LINUX
#include <stdio.h>
#include <unistd.h>
#include <wchar.h>
#include <string.h>
#include "voLog.h"
#endif

#include "vomeFilePipe.h"

OMX_PTR vomeFilePipeOpen (OMX_VO_FILE_SOURCE * pSource)
{
	if (pSource == NULL)
		return NULL;

	vomeCBaseFileOP * pFileOP = new vomeCBaseFileOP ();
	if (pFileOP == NULL)
		return NULL;

	OMX_PTR pFile = pFileOP->Open (pSource);
	if (pFile == NULL)
	{
		delete pFileOP;
		pFileOP = NULL;
	}

	return pFileOP;
}

OMX_S32 vomeFilePipeRead (OMX_PTR pHandle, OMX_PTR pBuffer, OMX_U32 uSize)
{
	if (pHandle == 0 || pBuffer == 0)
		return 0;

	vomeCBaseFileOP * pFileOP = (vomeCBaseFileOP *)pHandle;

	return pFileOP->Read (pBuffer, uSize);
}

OMX_S32 vomeFilePipeWrite (OMX_PTR pHandle, OMX_PTR pBuffer, OMX_U32 uSize)
{
	if (pHandle == 0 || pBuffer == 0)
		return 0;

	vomeCBaseFileOP * pFileOP = (vomeCBaseFileOP *)pHandle;

	return pFileOP->Write (pBuffer, uSize);
}

OMX_S32 vomeFilePipeFlush (OMX_PTR pHandle)
{
	if (pHandle == 0)
		return 0;

	vomeCBaseFileOP * pFileOP = (vomeCBaseFileOP *)pHandle;

	return pFileOP->Flush ();
}

OMX_S64 vomeFilePipeSeek (OMX_PTR pHandle, OMX_S64 nPos, OMX_VO_FILE_POS uFlag)
{
	if (pHandle == 0)
		return 0;

	vomeCBaseFileOP * pFileOP = (vomeCBaseFileOP *)pHandle;

	return pFileOP->Seek (nPos, uFlag);
}

OMX_S64 vomeFilePipeSize (OMX_PTR pHandle)
{
	if (pHandle == 0)
		return 0;

	vomeCBaseFileOP * pFileOP = (vomeCBaseFileOP *)pHandle;

	return pFileOP->Size ();
}

OMX_S64 vomeFilePipeSave (OMX_PTR pHandle)
{
	if (pHandle == 0)
		return 0;

	vomeCBaseFileOP * pFileOP = (vomeCBaseFileOP *)pHandle;

	return pFileOP->Save ();
}

OMX_S32 vomeFilePipeClose (OMX_PTR pHandle)
{
	if (pHandle == 0)
		return 0;

	vomeCBaseFileOP * pFileOP = (vomeCBaseFileOP *)pHandle;

	pFileOP->Close ();
	delete pFileOP;

	return 0;
}

OMX_S32	vomeFilePipeFillPointer (OMX_VO_FILE_OPERATOR * pPipeOP)
{
	if (pPipeOP == NULL)
		return -1;

	pPipeOP->Open = vomeFilePipeOpen;
	pPipeOP->Read = vomeFilePipeRead;
	pPipeOP->Write = vomeFilePipeWrite;
	pPipeOP->Flush = vomeFilePipeFlush;
	pPipeOP->Seek = vomeFilePipeSeek;
	pPipeOP->Size = vomeFilePipeSize;
	pPipeOP->Save = vomeFilePipeSave;
	pPipeOP->Close = vomeFilePipeClose;

	return 0;
}


vomeCBaseFileOP::vomeCBaseFileOP(void)
{
	m_hHandle = NULL;
	m_hOpen = NULL;
	m_nFilePos = 0;
}

vomeCBaseFileOP::~vomeCBaseFileOP(void)
{
	Close ();
}

OMX_PTR vomeCBaseFileOP::Open (OMX_VO_FILE_SOURCE * pSource)
{
	if (pSource == NULL)
		return NULL;
	memcpy (&m_Source, pSource, sizeof (OMX_VO_FILE_SOURCE));

	if ((pSource->nFlag & 0XFF) != OMX_VO_FILE_PIPE_NAME)
	{
		m_hHandle = pSource->pSource;
		return m_hHandle;
	}

	OMX_STRING pName = (OMX_STRING) pSource->pSource;
#ifdef _WIN32
	if (pSource->nMode == OMX_VO_FILE_READ_ONLY || pSource->nMode == 0)
		m_hOpen = CreateFile((TCHAR *)pName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, (DWORD) 0, NULL);
	else if (pSource->nMode == OMX_VO_FILE_WRITE_ONLY)
		m_hOpen = CreateFile((TCHAR *)pName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, (DWORD) 0, NULL);
	else
		m_hOpen = CreateFile((TCHAR *)pName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, (DWORD) 0, NULL);

	if (m_hOpen == INVALID_HANDLE_VALUE)
		return NULL;

	m_hHandle = m_hOpen;

#elif defined LINUX
	FILE* pf = NULL;
	if (pSource->nMode == OMX_VO_FILE_READ_ONLY || pSource->nMode == 0)
		pf = fopen(pName, "rb");
	else if (pSource->nMode == OMX_VO_FILE_WRITE_ONLY)
		pf = fopen(pName, "wb");
	else
		pf = fopen(pName, "a+b");

	if (pf == NULL)
		return 0;

	m_hHandle = m_hOpen = pf;
#endif

	if (m_Source.nLength > 0)
	{
		if (m_Source.nLength > FileSize () - m_Source.nOffset)
			m_Source.nLength = FileSize () - m_Source.nOffset;
	}

	if (m_Source.nOffset > 0)
		Seek (0, OMX_VO_FILE_BEGIN);

	return m_hHandle;
}

OMX_S32 vomeCBaseFileOP::Read (OMX_PTR pBuffer, OMX_U32 uSize)
{
	if (m_hHandle == NULL)
		return -1;

	OMX_U32 uRead = 0;

#ifdef _WIN32
	ReadFile (m_hHandle, pBuffer, uSize, &uRead, NULL);
#elif defined LINUX
	uRead = fread(pBuffer, 1, uSize, (FILE*)m_hHandle);
	if (uRead < uSize)
	{
		if (feof((FILE*)m_hHandle) == 0)
			return -1;
	}
#endif

	if (m_Source.nLength > 0)
	{
		if (m_nFilePos + uRead > m_Source.nOffset + m_Source.nLength)
		{
			uRead = (OMX_U32)(m_Source.nOffset + m_Source.nLength - m_nFilePos);
		}
	}

	m_nFilePos = m_nFilePos + uRead;

	return uRead;
}

OMX_S32 vomeCBaseFileOP::Write (OMX_PTR pBuffer, OMX_U32 uSize)
{
	if (m_hHandle == NULL)
		return -1;

	OMX_U32 uWrite = 0;

#ifdef _WIN32
	WriteFile (m_hHandle, pBuffer, uSize, &uWrite, NULL);
#elif defined LINUX
	uWrite = fwrite(pBuffer,1, uSize, (FILE*)m_hHandle);
#endif

	return uWrite;
}

OMX_S32 vomeCBaseFileOP::Flush (void)
{
	if (m_hHandle == NULL)
		return -1;

#ifdef _WIN32
	return FlushFileBuffers (m_hHandle);
#elif defined LINUX
	return fflush((FILE*)m_hHandle);
#endif

	return 0;
}

OMX_S64 vomeCBaseFileOP::Seek (OMX_S64 nPos, OMX_VO_FILE_POS uFlag)
{
	if (m_hHandle == NULL)
		return -1;

	if (uFlag == OMX_VO_FILE_BEGIN)
		nPos = nPos + m_Source.nOffset;
	else if (uFlag == OMX_VO_FILE_CURRENT)
		nPos = nPos;
	else
	{
		if (m_Source.nLength > 0)
			nPos = FileSize () - (m_Source.nOffset + m_Source.nLength);
	}

	if (uFlag == OMX_VO_FILE_BEGIN)
		m_nFilePos = nPos;
	else if (uFlag == OMX_VO_FILE_CURRENT)
		m_nFilePos = m_nFilePos + nPos;
	else
		m_nFilePos = FileSize () - nPos;

#ifdef _WIN32
	OMX_S32 lPos = (long)nPos;
	OMX_S32 lHigh = (long)(nPos >> 32);

	OMX_S32 sMove = FILE_BEGIN;
	if (uFlag == OMX_VO_FILE_BEGIN)
		sMove = FILE_BEGIN;
	else if (uFlag == OMX_VO_FILE_CURRENT)
		sMove = FILE_CURRENT;
	else
		sMove = FILE_END;

	return SetFilePointer (m_hHandle, lPos, &lHigh, sMove);

#elif defined LINUX
	OMX_S32 whence = 0;
	if (uFlag == OMX_VO_FILE_BEGIN)
		whence = SEEK_SET;
	else if (uFlag == OMX_VO_FILE_CURRENT)
		whence = SEEK_CUR;
	else
		whence = SEEK_END;

	if (fseek((FILE*)m_hHandle, nPos, whence) < 0)
		return -1;

	long lpos = ftell((FILE*)m_hHandle);
	if (lpos < 0)
		return -1;

	return (int)lpos;
#endif
}

OMX_S64 vomeCBaseFileOP::Size (void)
{
	if (m_Source.nLength > 0)
		return m_Source.nLength;

	OMX_S64 nFileSize = FileSize ();

	return nFileSize - m_Source.nOffset;
}

OMX_S64 vomeCBaseFileOP::Save (void)
{
	OMX_S64 sSize = 0;

#ifdef _WIN32
	OMX_S32 lSize = 0;
	OMX_S32 lHigh = 0;

	lSize = GetFileSize (m_hHandle, (LPDWORD)&lHigh);

	sSize = lHigh;
	sSize = (sSize << 32) + lSize;
#endif // _WIN32

	return sSize;
}

OMX_S32 vomeCBaseFileOP::Close (void)
{
	if (m_hOpen != NULL)
	{
#ifdef _WIN32
		CloseHandle (m_hOpen);
#elif defined LINUX
		fclose((FILE*)m_hOpen);
#endif
	}

	m_hOpen = NULL;

	return 0;
}

OMX_S64 vomeCBaseFileOP::FileSize (void)
{
	if (m_hHandle == NULL)
		return -1;

#ifdef _WIN32
	OMX_U32 lSize = 0;
	OMX_U32 lHigh = 0;

	OMX_S64 sSize = 0;

	lSize = GetFileSize (m_hHandle, (LPDWORD)&lHigh);

	sSize = lHigh;
	sSize = (sSize << 32) + lSize;

	return sSize;
#elif defined LINUX

	OMX_S32 r0 = 0, r = 0;
	//struct stat st;
	//r = fstat(fileno((FILE*)m_hHandle), &st);   // not be supported by some devices
	r0 = ftell((FILE*)m_hHandle);

	r = fseek((FILE*)m_hHandle, 0L, SEEK_END);
	if (-1 == r)
	{
		return -1;
	}

	r = ftell((FILE*)m_hHandle);
	if (-1 == r)
	{
		return -1;
	}

	fseek((FILE*)m_hHandle, r0, SEEK_SET);
	//return st.st_size;
	return r;
#endif
}
