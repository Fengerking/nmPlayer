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

#include "cioFile.h"

#include "voOSFunc.h"

#define LOG_TAG "cmnFile"
#include "voLog.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

VO_FILE_OPERATOR	g_fileIOOP;
VO_SOURCE2_IO_API	g_sourceIO;

class voCBaseFileIOOP
{
public:
    voCBaseFileIOOP(void);
    virtual ~voCBaseFileIOOP(void);

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
	VO_PTR			m_hHandle;
};

VO_PTR cioFileOpen (VO_FILE_SOURCE * pSource)
{
	if (pSource == NULL)
		return NULL;

	voCBaseFileIOOP * pFileOP = new voCBaseFileIOOP ();
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

VO_S32 cioFileRead (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize)
{
	if (pHandle == 0 || pBuffer == 0)
		return 0;

	voCBaseFileIOOP * pFileOP = (voCBaseFileIOOP *)pHandle;

	return pFileOP->Read (pBuffer, uSize);
}

VO_S32 cioFileWrite (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize)
{
	if (pHandle == 0 || pBuffer == 0)
		return 0;

	voCBaseFileIOOP * pFileOP = (voCBaseFileIOOP *)pHandle;

	return pFileOP->Write (pBuffer, uSize);
}

VO_S32 cioFileFlush (VO_PTR pHandle)
{
	if (pHandle == 0)
		return 0;

	voCBaseFileIOOP * pFileOP = (voCBaseFileIOOP *)pHandle;

	return pFileOP->Flush ();
}

VO_S64 cioFileSeek (VO_PTR pHandle, VO_S64 nPos, VO_FILE_POS uFlag)
{
	if (pHandle == 0)
		return 0;

	voCBaseFileIOOP * pFileOP = (voCBaseFileIOOP *)pHandle;

	return pFileOP->Seek (nPos, uFlag);
}

VO_S64 cioFileSize (VO_PTR pHandle)
{
	if (pHandle == 0)
		return 0;

	voCBaseFileIOOP * pFileOP = (voCBaseFileIOOP *)pHandle;

	return pFileOP->Size ();
}

VO_S64 cioFileSave (VO_PTR pHandle)
{
	if (pHandle == 0)
		return 0;

	voCBaseFileIOOP * pFileOP = (voCBaseFileIOOP *)pHandle;

	return pFileOP->Save ();
}

VO_S32 cioFileClose (VO_PTR pHandle)
{
	if (pHandle == 0)
		return 0;

	voCBaseFileIOOP * pFileOP = (voCBaseFileIOOP *)pHandle;

	pFileOP->Close ();
	delete pFileOP;

	return 0;
}

VO_S32	cioFileFillPointer (VO_SOURCE2_IO_API* pSourceIO)
{
	if(pSourceIO == NULL)
		return -1;
	memcpy(&g_sourceIO, pSourceIO, sizeof(VO_SOURCE2_IO_API));
	g_fileIOOP.Open = cioFileOpen;
	g_fileIOOP.Read = cioFileRead;
	g_fileIOOP.Write = cioFileWrite;
	g_fileIOOP.Flush = cioFileFlush;
	g_fileIOOP.Seek = cioFileSeek;
	g_fileIOOP.Size = cioFileSize;
	g_fileIOOP.Save = cioFileSave;
	g_fileIOOP.Close = cioFileClose;

	return 0;
}
    
#ifdef _VONAMESPACE
}
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voCBaseFileIOOP::voCBaseFileIOOP(void)
{
	m_hHandle = NULL;
}

voCBaseFileIOOP::~voCBaseFileIOOP(void)
{
	Close ();
}

VO_PTR voCBaseFileIOOP::Open (VO_FILE_SOURCE * pSource)
{
	if (pSource == NULL)
		return NULL;

	if(pSource->nFlag != VO_FILE_TYPE_NAME)
		return NULL;

	int nFlag = VO_SOURCE2_IO_FLAG_OPEN_LOCAL_FILE | VO_SOURCE2_IO_FLAG_OPEN_DRM;

	int nRC = g_sourceIO.Init(&m_hHandle, pSource->pSource, nFlag, NULL);

	nRC = g_sourceIO.Open(m_hHandle, VO_FALSE);
	
	if(nRC)
		return NULL;

	return pSource->pSource;
}

VO_S32 voCBaseFileIOOP::Read (VO_PTR pBuffer, VO_U32 uSize)
{
	if (m_hHandle == NULL)
		return -1;

	VO_U32 nRead = 0;

	int nRC = g_sourceIO.Read(m_hHandle, (VO_VOID *)pBuffer, uSize, &nRead);

	if(nRC) return nRC;

	return nRead;
}

VO_S32 voCBaseFileIOOP::Write (VO_PTR pBuffer, VO_U32 uSize)
{
	if (m_hHandle == NULL)
		return -1;

	VO_U32 nWrited = 0;

	int nRC = g_sourceIO.Write(m_hHandle, (VO_VOID *)pBuffer, uSize, &nWrited);

	if(nRC) return nRC;

	return nWrited;
}

VO_S32 voCBaseFileIOOP::Flush (void)
{
	if (m_hHandle == NULL)
		return -1;

	int nRC = g_sourceIO.Flush(m_hHandle);
	
	return nRC;
}

VO_S64 voCBaseFileIOOP::Seek (VO_S64 nPos, VO_FILE_POS uFlag)
{
	if (m_hHandle == NULL)
		return -1;

	VO_S64 llPos = 0;

	int nRC = g_sourceIO.SetPos(m_hHandle, nPos, (VO_SOURCE2_IO_POS)uFlag, &llPos);

	if(nRC) return nRC;

	return llPos;
}

VO_S64 voCBaseFileIOOP::Size (void)
{
	return 0;
}

VO_S64 voCBaseFileIOOP::Save (void)
{
	return 0;
}

VO_S32 voCBaseFileIOOP::Close (void)
{
	if (m_hHandle == NULL)
		return 0;

	int nRC = g_sourceIO.Close(m_hHandle);
	nRC = g_sourceIO.UnInit(m_hHandle);
	m_hHandle = NULL;

	return nRC;
}

VO_S64 voCBaseFileIOOP::FileSize (void)
{
	if (m_hHandle == NULL)
		return -1;

	VO_U64 llSize = 0;
	int nRC = g_sourceIO.GetSize(m_hHandle, &llSize);	

	if(nRC) return nRC;

	return llSize;
}
