	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		cmnFile.c

	Contains:	memory operator implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-15		JBF			Create file

*******************************************************************************/
//#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _METRO
#include <windows.h>
#include <Mfidl.h>
#elif defined  _WIN32
#ifndef _WIN32_WCE
#include "direct.h"
#endif //_WIN32_WCE
#elif defined LINUX
#include <errno.h>
#if !defined UCLINUX
#include <wchar.h>
#else
#define __USE_LARGEFILE64
#endif
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif

#include "voYYDef_Common.h"
#include "cmnFile.h"

#include "voOSFunc.h"

#define LOG_TAG "cmnFile"
#include "voLog.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

VO_FILE_OPERATOR	g_fileOP;

class voCBaseFileOP
{
public:
    voCBaseFileOP(void);
    virtual ~voCBaseFileOP(void);

	virtual VO_PTR	Open (VO_FILE_SOURCE * pSource);
	virtual VO_S32 	Read (VO_PTR pBuffer, VO_U32 uSize);
	virtual VO_S32 	Write (VO_PTR pBuffer, VO_U32 uSize);
	virtual VO_S32 	Flush (void);
	virtual VO_S64 	Seek (VO_S64 nPos, VO_FILE_POS uFlag);
	virtual VO_S64 	Size (void);
	virtual VO_S64 	Save (void);
	virtual VO_S32 	Close (void);

protected:
	virtual VO_S64 	FileSize (void);


protected:
	VO_FILE_SOURCE	m_Source;

	VO_PTR			m_hHandle;
	VO_S32			m_nFD;
	VO_PTR			m_hOpen;
	VO_S64			m_nFilePos;

	VO_PTR			m_hMemHandle;
	VO_PBYTE		m_pMemBuffer[32];
	VO_U32			m_nMemPos;
	VO_U32			m_nItemSize;

#ifdef _METRO
	IMFByteStream *		  mpMFByteStream;
#endif //_METRO 
};

VO_PTR cmnFileOpen (VO_FILE_SOURCE * pSource)
{
	if (pSource == NULL)
		return NULL;

	voCBaseFileOP * pFileOP = new voCBaseFileOP ();
	if (pFileOP == NULL)
		return NULL;

	VO_PTR pFile = pFileOP->Open (pSource);
	if (pFile == NULL)
	{
		delete pFileOP;
		pFileOP = NULL;
	}

	return pFileOP;
}

VO_S32 cmnFileRead (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize)
{
	if (pHandle == 0 || pBuffer == 0)
		return 0;

	voCBaseFileOP * pFileOP = (voCBaseFileOP *)pHandle;

	return pFileOP->Read (pBuffer, uSize);
}

VO_S32 cmnFileWrite (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize)
{
	if (pHandle == 0 || pBuffer == 0)
		return 0;

	voCBaseFileOP * pFileOP = (voCBaseFileOP *)pHandle;

	return pFileOP->Write (pBuffer, uSize);
}

VO_S32 cmnFileFlush (VO_PTR pHandle)
{
	if (pHandle == 0)
		return 0;

	voCBaseFileOP * pFileOP = (voCBaseFileOP *)pHandle;

	return pFileOP->Flush ();
}

VO_S64 cmnFileSeek (VO_PTR pHandle, VO_S64 nPos, VO_FILE_POS uFlag)
{
	if (pHandle == 0)
		return 0;

	voCBaseFileOP * pFileOP = (voCBaseFileOP *)pHandle;

	return pFileOP->Seek (nPos, uFlag);
}

VO_S64 cmnFileSize (VO_PTR pHandle)
{
	if (pHandle == 0)
		return 0;

	voCBaseFileOP * pFileOP = (voCBaseFileOP *)pHandle;

	return pFileOP->Size ();
}

VO_S64 cmnFileSave (VO_PTR pHandle)
{
	if (pHandle == 0)
		return 0;

	voCBaseFileOP * pFileOP = (voCBaseFileOP *)pHandle;

	return pFileOP->Save ();
}

VO_S32 cmnFileClose (VO_PTR pHandle)
{
	if (pHandle == 0)
		return 0;

	voCBaseFileOP * pFileOP = (voCBaseFileOP *)pHandle;

	pFileOP->Close ();
	delete pFileOP;

	return 0;
}

VO_S32	cmnFileFillPointer (void)
{
	g_fileOP.Open = cmnFileOpen;
	g_fileOP.Read = cmnFileRead;
	g_fileOP.Write = cmnFileWrite;
	g_fileOP.Flush = cmnFileFlush;
	g_fileOP.Seek = cmnFileSeek;
	g_fileOP.Size = cmnFileSize;
	g_fileOP.Save = cmnFileSave;
	g_fileOP.Close = cmnFileClose;

	return 0;
}

VO_S32	cmnFileDelete (VO_PTCHAR pFileName)
{
	VO_S32 nRC = 0;

#ifdef _WIN32
	nRC = DeleteFile (pFileName);
#elif defined _MAC_OS
	nRC = remove(pFileName);
#endif // _WIN32

	return nRC;
}


VO_S32	cmnFolderCreate (VO_PTCHAR pFolderName)
{
	VO_S32 nRC = 0;

#ifdef _WIN32
#ifndef _WIN32_WCE
	nRC = _tmkdir (pFolderName);
#endif // _WIN32_WCE
#endif // _WIN32

	return nRC;
}


VO_S32	cmnFolderDelete (VO_PTCHAR pFolderName)
{
	VO_S32 nRC = 0;

#ifdef _WIN32
#ifndef _WIN32_WCE
	nRC = _trmdir (pFolderName);
#endif // _WIN32_WCE
#endif // _WIN32

	return nRC;
}
    
#ifdef _VONAMESPACE
}
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voCBaseFileOP::voCBaseFileOP(void)
{
	m_hHandle = NULL;
	m_nFD = -1;
	m_hOpen = NULL;
	m_nFilePos = 0;

	m_hMemHandle = NULL;
	for (int i = 0; i < 32; i++)
		m_pMemBuffer[i] = NULL;
	m_nMemPos = 0;
	m_nItemSize = 1024 * 1024;

#ifdef _METRO
	mpMFByteStream = nullptr;
#endif //_METRO
}

voCBaseFileOP::~voCBaseFileOP(void)
{
	Close ();
}

VO_PTR voCBaseFileOP::Open (VO_FILE_SOURCE * pSource)
{
	if (pSource == NULL)
		return NULL;
	memcpy (&m_Source, pSource, sizeof (VO_FILE_SOURCE));

	/*
	if(pSource->nFlag == VO_FILE_TYPE_HANDLE)
	{
		VOLOGE ("Handle Flag %d, Handle : %08X", pSource->nFlag, (int)pSource->pSource);
	}
	else if (pSource->nFlag == VO_FILE_TYPE_ID)
	{
		VOLOGE ("ID Flag %d, ID : %d", pSource->nFlag, (int)pSource->pSource);
	}
	else
	{
		VOLOGE ("ID Flag %d, File : %s", pSource->nFlag, (VO_PTCHAR)pSource->pSource);
	}
	*/

	if (pSource->nFlag != VO_FILE_TYPE_NAME)
	{
		if(pSource->nFlag == VO_FILE_TYPE_HANDLE)
			m_hHandle = pSource->pSource;
#ifdef _METRO
		else if(pSource->nFlag == VO_FILE_TYPE_IMFBYTESTREAM)
		{
			mpMFByteStream = (IMFByteStream *)(pSource->pSource);
			if(mpMFByteStream != nullptr)
				mpMFByteStream->AddRef();
		}
#endif //_METRO
		else
		{
#ifdef _MAC_OS
			m_nFD = (signed long)pSource->pSource;
#else
			m_nFD = (VO_S32)pSource->pSource;
#endif
		}
			

		Seek (0, VO_FILE_BEGIN);

		return pSource->pSource;
	}

	VO_TCHAR szFile[2048];
	vostrcpy (szFile, (VO_PTCHAR) pSource->pSource);

#ifdef _METRO
	
	VOLOGI("%s" , szFile);
	//memset(szFile , 0 , sizeof(VO_TCHAR) * 2048);
	//::MultiByteToWideChar (CP_ACP, 0, (char *)pSource->pSource, -1, szFile, sizeof (szFile));
	if (pSource->nMode == VO_FILE_READ_ONLY || pSource->nMode == 0)
		m_hOpen = CreateFile2(szFile, GENERIC_READ, FILE_SHARE_READ,  OPEN_EXISTING, NULL);
	else if (pSource->nMode == VO_FILE_WRITE_ONLY)
		m_hOpen = CreateFile2(szFile, GENERIC_WRITE, FILE_SHARE_READ,  CREATE_ALWAYS,  NULL);
	else
		m_hOpen = CreateFile2(szFile, GENERIC_WRITE, FILE_SHARE_READ, OPEN_ALWAYS,  NULL);

	if (m_hOpen == INVALID_HANDLE_VALUE)
		return NULL;

	m_hHandle = m_hOpen;
#elif defined _WIN32
	if (pSource->nMode == VO_FILE_READ_ONLY || pSource->nMode == 0)
		m_hOpen = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, (DWORD) 0, NULL);
	else if (pSource->nMode == VO_FILE_WRITE_ONLY)
		m_hOpen = CreateFile(szFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, (DWORD) 0, NULL);
	else
		m_hOpen = CreateFile(szFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, (DWORD) 0, NULL);

	if (m_hOpen == INVALID_HANDLE_VALUE)
		return NULL;

	m_hHandle = m_hOpen;

#elif defined LINUX
	if (pSource->nMode == VO_FILE_READ_ONLY || pSource->nMode == 0)
	{
		int nFlag = 0;
		int nMode = 0640;
		if (pSource->nMode == VO_FILE_READ_ONLY || pSource->nMode == 0)
			nFlag = O_RDONLY;
		else if (pSource->nMode == VO_FILE_WRITE_ONLY)
			nFlag = O_RDWR | O_CREAT;
		else
			nFlag = O_RDWR | O_CREAT;

#ifdef _LINUX_X86
		m_nFD = open64(szFile, nFlag, nMode);
#else
		m_nFD = open(szFile, nFlag, nMode);
#endif		
		if (m_nFD > 0)
		{
			m_hOpen = (VO_PTR)m_nFD;
		}
		else
		{
			m_nFD = -1;
		}
	}
	else
	{
		FILE* pf = NULL;
		if (pSource->nMode == VO_FILE_READ_ONLY || pSource->nMode == 0)
			pf = fopen(szFile, "rb");
		else if (pSource->nMode == VO_FILE_WRITE_ONLY)
			pf = fopen(szFile, "wb");
		else
			pf = fopen(szFile, "a+b");

		m_hHandle = m_hOpen = pf;
	}
#elif defined __SYMBIAN32__ || defined(_IOS) || defined(_MAC_OS)
	FILE* pf = NULL;
	if (pSource->nMode == VO_FILE_READ_ONLY || pSource->nMode == 0)
		pf = fopen(szFile, "rb");
	else if (pSource->nMode == VO_FILE_WRITE_ONLY)
		pf = fopen(szFile, "wb");
	else
		pf = fopen(szFile, "a+b");

	m_hHandle = m_hOpen = pf;	
#endif

	if (m_hHandle == NULL && m_nFD < 0)
	{
		if (pSource->nMode == VO_FILE_WRITE_ONLY)
		{
			m_pMemBuffer[0] = new VO_BYTE[m_nItemSize];
			m_hMemHandle = m_pMemBuffer[0];
			m_nMemPos = 0;
			return m_hMemHandle;
		}
		else
		{
			return NULL;
		}
	}

	if (m_Source.nLength > 0)
	{
		if (m_Source.nLength > FileSize () - m_Source.nOffset)
			m_Source.nLength = FileSize () - m_Source.nOffset;
	}

	if (m_Source.nOffset > 0)
		Seek (0, VO_FILE_BEGIN);

	return pSource->pSource;
}

VO_S32 voCBaseFileOP::Read (VO_PTR pBuffer, VO_U32 uSize)
{
	if (m_hMemHandle != NULL)
	{
		int nIndex = m_nMemPos / m_nItemSize;
		if (nIndex >= 32)
			return -1;

		if (m_pMemBuffer[nIndex] == NULL)
			return -1;

		VO_S32 nRestSize = m_nItemSize - (m_nMemPos % m_nItemSize);
		if (nRestSize - uSize >= 0) //MCW_SIGNED_CMP if (nRestSize >= uSize)
		{
			memcpy (pBuffer, m_pMemBuffer[nIndex], uSize);
		}
		else
		{
			memcpy (pBuffer, m_pMemBuffer[nIndex], nRestSize);
			if (nIndex >= 31)
			{
				m_nMemPos += nRestSize;
				return nRestSize;
			}
			if (m_pMemBuffer[nIndex + 1] == NULL)
				return nRestSize;

			memcpy ((VO_PBYTE)pBuffer + nRestSize, m_pMemBuffer[nIndex+1], uSize - nRestSize);
		}

		m_nMemPos += uSize;

		return uSize;
	}

#ifdef _METRO
	if (m_hHandle == NULL && mpMFByteStream == nullptr)
		return -1;
#else //_METRO
	if (m_hHandle == NULL && m_nFD <= 0)
		return -1;
#endif //_METRO

	VO_S32 uRead = 0;

//	VOLOGI ("Before Read Size: %d  %d, 0X%08X, %d", uSize, uRead, m_hHandle, m_nFD);
#ifdef _METRO
	if(mpMFByteStream != nullptr)
	{
		HRESULT hr = mpMFByteStream->Read((BYTE *)pBuffer , uSize , (ULONG *)&uRead);
		if(FAILED(hr))
			return -1;
	}
#elif defined  _WIN32
	ReadFile (m_hHandle, pBuffer, uSize, (LPDWORD)&uRead, NULL);
#elif defined LINUX
	if (m_nFD > 0)
	{
		uRead = read (m_nFD, pBuffer, uSize);
		if((int)uRead == -1)
			return -1;
	}
	else
	{
		uRead = fread(pBuffer, 1, uSize, (FILE*)m_hHandle);
		if((int)uRead == -1)
			return -1;
		if (uRead < (VO_S32)uSize)
		{
			if (feof((FILE*)m_hHandle) == 0)
				return -1;
		}
	}
#elif defined __SYMBIAN32__ || defined(_IOS) || defined(_MAC_OS)
	uRead = fread(pBuffer, 1, uSize, (FILE*)m_hHandle);
	if (uRead < uSize)
	{
		if (feof((FILE*)m_hHandle) == 0)
			return -1;
	}
#endif

//	VOLOGI ("Read Size: %d  %d", uSize, uRead);

	if (m_Source.nLength > 0)
	{
		if (m_nFilePos + uRead > m_Source.nOffset + m_Source.nLength)
		{
			uRead = (VO_U32)(m_Source.nOffset + m_Source.nLength - m_nFilePos);
		}
	}

	m_nFilePos = m_nFilePos + uRead;
	return uRead;
}

VO_S32 voCBaseFileOP::Write (VO_PTR pBuffer, VO_U32 uSize)
{
	if (m_hMemHandle != NULL)
	{
		int nIndex = m_nMemPos / m_nItemSize;
		if (nIndex >= 32)
			return -1;

		if (m_pMemBuffer[nIndex] == NULL)
			m_pMemBuffer[nIndex] = new VO_BYTE[m_nItemSize];
		if (m_pMemBuffer[nIndex] == NULL)
			return -1;

		int nRestSize = m_nItemSize - (m_nMemPos % m_nItemSize);
		if (nRestSize - uSize >= 0)  //MCW_SIGNED_CMP if (nRestSize >= uSize)
		{
			memcpy (m_pMemBuffer[nIndex] + (m_nItemSize - nRestSize), pBuffer, uSize);
		}
		else
		{
			memcpy (m_pMemBuffer[nIndex] + (m_nItemSize - nRestSize), pBuffer, nRestSize);
			if (nIndex >= 31)
			{
				m_nMemPos += nRestSize;
				return nRestSize;
			}

			if (m_pMemBuffer[nIndex+1] == NULL)
				m_pMemBuffer[nIndex+1] = new VO_BYTE[m_nItemSize];
			if (m_pMemBuffer[nIndex+1] == NULL)
				return nRestSize;

			memcpy (m_pMemBuffer[nIndex+1], (VO_PBYTE)pBuffer + (uSize - nRestSize), uSize - nRestSize);
		}

		m_nMemPos += uSize;

		return uSize;
	}


#ifdef _METRO
	if (m_hHandle == NULL && mpMFByteStream == nullptr)
		return -1;
#else //_METRO
	if (m_hHandle == NULL && m_nFD <= 0)
		return -1;
#endif //_METRO

	VO_U32 uWrite = 0;

#ifdef _WIN32
	WriteFile (m_hHandle, pBuffer, uSize, &uWrite, NULL);
#elif defined LINUX
	if (m_nFD > 0)
		uWrite = write(m_nFD, pBuffer, uSize);
	else
		uWrite = fwrite(pBuffer,1, uSize, (FILE*)m_hHandle);
#elif defined __SYMBIAN32__ || defined(_IOS) || defined(_MAC_OS)
	uWrite = fwrite(pBuffer,1, uSize, (FILE*)m_hHandle);
#endif

//	VOLOGI ("Request Size: %d, Written Size %d ", uSize, uWrite);

	return uWrite;
}

VO_S32 voCBaseFileOP::Flush (void)
{
#ifdef _METRO
	if (m_hHandle == NULL && mpMFByteStream == nullptr)
		return -1;
#else //_METRO
	if (m_hHandle == NULL && m_nFD <= 0)
		return -1;
#endif //_METRO

#ifdef _METRO
	if(mpMFByteStream != nullptr)
	{
		HRESULT hr = mpMFByteStream->Flush();
		if(FAILED(hr))
			return -1;

		return 1;
	}

	return -1;
#elif defined  _WIN32
	return FlushFileBuffers (m_hHandle);
#elif defined LINUX
	if (m_nFD > 0)
	{
		return fsync(m_nFD);
	}
	else
	{
		return fflush((FILE*)m_hHandle);
	}
#elif defined __SYMBIAN32__ || defined(_IOS) || defined(_MAC_OS)
	return fflush((FILE*)m_hHandle);
#endif
}

VO_S64 voCBaseFileOP::Seek (VO_S64 nPos, VO_FILE_POS uFlag)
{
	if (m_hMemHandle != NULL)
	{
		int nIndex = (int)(nPos / m_nItemSize);
		if (nIndex >= 32)
			return -1;

		for (int i = 0; i <= nIndex; i++)
		{
			if (m_pMemBuffer[i] == NULL)
				m_pMemBuffer[i] = new VO_BYTE[m_nItemSize];
			if (m_pMemBuffer[i] == NULL)
				return -1;
		}

		m_nMemPos = (VO_U32)nPos;
		return nPos;
	}

#ifdef _METRO
	if (m_hHandle == NULL && mpMFByteStream == nullptr)
		return -1;
#else //_METRO
	if (m_hHandle == NULL && m_nFD <= 0)
		return -1;
#endif //_METRO


	if (uFlag == VO_FILE_BEGIN)
		nPos = nPos + m_Source.nOffset;
	else if (uFlag == VO_FILE_CURRENT)
		nPos = nPos;
	else
	{
		if (m_Source.nLength > 0)
			nPos = FileSize () - (m_Source.nOffset + m_Source.nLength);
	}

	if (uFlag == VO_FILE_BEGIN)
		m_nFilePos = nPos;
	else if (uFlag == VO_FILE_CURRENT)
		m_nFilePos = m_nFilePos + nPos;
	else
		m_nFilePos = FileSize () - nPos;
#ifdef _METRO
	if(mpMFByteStream != nullptr)
	{
		MFBYTESTREAM_SEEK_ORIGIN seekOrigin = msoBegin;
		if(uFlag == VO_FILE_CURRENT)
			seekOrigin = msoCurrent ;

		QWORD qSeekPos = 0;
		HRESULT hr = mpMFByteStream->Seek(seekOrigin , nPos , 0 , &qSeekPos);
		if(FAILED(hr))
			return -1;

		return qSeekPos;
	}
	else
	{
		LARGE_INTEGER llPos ;
		llPos.QuadPart = (VO_S64)nPos;
		LARGE_INTEGER llCurPos;
	

		VO_S32 sMove = FILE_BEGIN;
		if (uFlag == VO_FILE_BEGIN)
			sMove = FILE_BEGIN;
		else if (uFlag == VO_FILE_CURRENT)
			sMove = FILE_CURRENT;
		else
			sMove = FILE_END;

		if(INVALID_SET_FILE_POINTER == SetFilePointerEx (m_hHandle, llPos, &llCurPos, sMove))
			return -1;

		return llCurPos.QuadPart;
	}
#elif defined _WIN32
	VO_S32 lPos = (long)nPos;
	VO_S32 lHigh = (long)(nPos >> 32);

	VO_S32 sMove = FILE_BEGIN;
	if (uFlag == VO_FILE_BEGIN)
		sMove = FILE_BEGIN;
	else if (uFlag == VO_FILE_CURRENT)
		sMove = FILE_CURRENT;
	else
		sMove = FILE_END;

	VO_S32 rtn = 0;
	DWORD dw = SetFilePointer (m_hHandle, lPos, &lHigh, sMove);
	//modefied by Aiven,return the currect file pointer if finish the seek.
	if(INVALID_SET_FILE_POINTER == dw){
		return -1;
	}

	// when sMove is FILE_CURRENT or lHigh is not 0, we need reset nPos 
	rtn = lHigh;
	rtn <<= 32;
	if(rtn+dw != nPos)
	{
		nPos = rtn+dw ;
	}

	return nPos;
#elif defined LINUX
	VO_S32 whence = 0;
	if (uFlag == VO_FILE_BEGIN)
		whence = SEEK_SET;
	else if (uFlag == VO_FILE_CURRENT)
		whence = SEEK_CUR;
	else
		whence = SEEK_END;

	long long lpos = 0;

	if (m_nFD > 0)
	{
		if((lpos = lseek64(m_nFD, nPos, whence)) < 0)
		{
			return -1;
		}
	}
	else
	{
		if (fseeko((FILE*)m_hHandle, nPos, whence) < 0)
		{
			VOLOGE("fseeko to  : %lld failed", (long long) nPos);
			return -1;

		}
		lpos = ftello((FILE*)m_hHandle);
		if (lpos < 0)
		{
			VOLOGE("ftello the position failed");
			return -1;
		}
	}

	return lpos;
#elif defined __SYMBIAN32__ || defined(_IOS) || defined(_MAC_OS)
	VO_S32 whence = 0;
	if (uFlag == VO_FILE_BEGIN)
		whence = SEEK_SET;
	else if (uFlag == VO_FILE_CURRENT)
		whence = SEEK_CUR;
	else
		whence = SEEK_END;

	long long lpos = 0;

	if (fseeko ((FILE*)m_hHandle, nPos, whence) < 0)
	{                                                                                                                                                                                                                               
		VOLOGE("fseeko to  : %lld failed", (long long) nPos);
		return -1;

	}
	lpos = ftello ((FILE*)m_hHandle);
	if (lpos < 0)
	{
		VOLOGE("ftello the position failed");
		return -1;
	}
	return lpos;
#endif
}

VO_S64 voCBaseFileOP::Size (void)
{
	if (m_Source.nLength > 0)
		return m_Source.nLength;

	VO_S64 nFileSize = FileSize ();

	return nFileSize - m_Source.nOffset;
}

VO_S64 voCBaseFileOP::Save (void)
{
	VO_S64 sSize = 0;

#ifdef _METRO
	
#elif defined _WIN32
	VO_S32 lSize = 0;
	VO_S32 lHigh = 0;

	lSize = GetFileSize (m_hHandle, (LPDWORD)&lHigh);

	sSize = lHigh;
	sSize = (sSize << 32) + lSize;
#endif // _WIN32

	return sSize;
}

VO_S32 voCBaseFileOP::Close (void)
{
	if (m_hMemHandle != NULL)
	{
		for (int i = 0; i < 32; i++)
		{
			if (m_pMemBuffer[i] != NULL)
				delete []m_pMemBuffer[i];
			m_pMemBuffer[i] = NULL;
		}
		return 0;
	}

	if (m_hOpen != NULL)
	{
#ifdef _WIN32
		CloseHandle (m_hOpen);
#elif defined LINUX
		if (m_nFD > 0)
		{
			close((size_t)m_hOpen);
		}
		else
		{
			fclose((FILE*)m_hOpen);
		}
#elif defined __SYMBIAN32__ || defined(_IOS) || defined(_MAC_OS)
		fclose((FILE*)m_hOpen);
#endif
	}

#ifdef _METRO
	if(mpMFByteStream != nullptr)
	{
		mpMFByteStream->Release();
		mpMFByteStream = nullptr;
	}
#endif //_METRO

	m_hOpen = NULL;

	return 0;
}

VO_S64 voCBaseFileOP::FileSize (void)
{
#ifdef _METRO
	if (m_hHandle == NULL && mpMFByteStream == nullptr)
		return -1;
#else //_METRO
	if (m_hHandle == NULL && m_nFD <= 0)
		return -1;
#endif //_METRO

#ifdef _METRO

	if(mpMFByteStream != nullptr)
	{
		QWORD qLength = 0;
		HRESULT hr = mpMFByteStream->GetLength(&qLength);
		if(FAILED(hr))
			return -1;

		return qLength;
	}

	FILE_STANDARD_INFO fileInfo;
	GetFileInformationByHandleEx(m_hHandle , FileStandardInfo , &fileInfo , sizeof(fileInfo));

	return fileInfo.EndOfFile.QuadPart;
#elif defined _WIN32
	VO_S64 llSize = 0;
	VO_S32 lHigh = 0;

	VO_S64 sSize = 0;

	llSize = GetFileSize (m_hHandle, (LPDWORD)&lHigh);

	sSize = lHigh;
	sSize = (sSize << 32) + llSize;

	return sSize;
#elif defined LINUX

	if (m_nFD > 0)
	{
		struct stat st;
		memset(&st, 0, sizeof(struct stat));
	    fstat(m_nFD, &st); 

		return st.st_size;
	}
	else
	{
		VO_S64 r0 = 0, r = 0;
		r0 = ftello((FILE*)m_hHandle);

		r = fseeko((FILE*)m_hHandle, 0LL, SEEK_END);
		if (-1 == r)
		{
			return -1;
		}

		r = ftello((FILE*)m_hHandle);
		if (-1 == r)
		{
			return -1;
		}

		fseeko((FILE*)m_hHandle, r0, SEEK_SET);
		//return st.st_size;
		return r;
	}
#elif defined __SYMBIAN32__ || defined(_IOS) || defined(_MAC_OS)
		VO_S64 r0 = 0, r = 0;
		r0 = ftello ((FILE*)m_hHandle);

		r = fseeko ((FILE*)m_hHandle, 0LL, SEEK_END);
		if (-1 == r)
		{
			return -1;
		}

		r = ftello ((FILE*)m_hHandle);
		if (-1 == r)
		{
			return -1;
		}

		fseeko((FILE*)m_hHandle, r0, SEEK_SET);
		//return st.st_size;
		return r;
#endif
}
