	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		vompSourceOP.c

	Contains:	memory operator implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-14		JBF			Create file

*******************************************************************************/
#include <stdlib.h>
#ifndef WINCE
#include <errno.h>
#endif //WINCE
#include <stdio.h>
#include <string.h>

#ifdef _LINUX_ANDROID
#include <unistd.h>
#include <wchar.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif // _LINUX_ANDROID

#include "voCDataSourceOP.h"
#include "vompType.h"

#define LOG_TAG "voCDataSourceOP"
#include "voLog.h"

#ifdef _VONAMESPACE
//using namespace _VONAMESPACE;
#endif

VO_FILE_OPERATOR	g_vompSourceOP;

class voCDataSourceOP
{
public:
    voCDataSourceOP(void);
    virtual ~voCDataSourceOP(void);

	virtual void *		Open (VO_FILE_SOURCE * pSource);
	virtual int 		Read (void * pBuffer, int uSize);
	virtual int 		Write (void * pBuffer, int uSize);
	virtual int 		Flush (void);
	virtual long long 	Seek (long long nPos, VO_FILE_POS uFlag);
	virtual long long 	Size (void);
	virtual long long 	Save (void);
	virtual int 		Close (void);

protected:
	virtual long long	FileSize (void);

protected:
	VO_FILE_SOURCE			m_Source;
	long long				m_nFilePos;

	VOMP_DATASOURCE_FUNC *	m_pDataSource;
};

void * vompSourceOPOpen (VO_FILE_SOURCE * pSource)
{
	if (pSource == NULL)
		return NULL;

	voCDataSourceOP * pFileOP = new voCDataSourceOP ();
	if (pFileOP == NULL)
		return NULL;

	pFileOP->Open (pSource);

	return pFileOP;
}

VO_S32 vompSourceOPRead (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize)
{
	if (pHandle == NULL || pBuffer == 0)
		return 0;

	voCDataSourceOP * pFileOP = (voCDataSourceOP *)pHandle;

	return pFileOP->Read (pBuffer, uSize);
}

VO_S32 vompSourceOPWrite (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize)
{
	if (pHandle == NULL || pBuffer == 0)
		return 0;

	voCDataSourceOP * pFileOP = (voCDataSourceOP *)pHandle;

	return pFileOP->Write (pBuffer, uSize);
}

VO_S32 vompSourceOPFlush (VO_PTR pHandle)
{
	if (pHandle == NULL)
		return 0;

	voCDataSourceOP * pFileOP = (voCDataSourceOP *)pHandle;

	return pFileOP->Flush ();
}

long long vompSourceOPSeek (void * pHandle, long long nPos, VO_FILE_POS uFlag)
{
	if (pHandle == NULL)
		return 0;

	voCDataSourceOP * pFileOP = (voCDataSourceOP *)pHandle;

	return pFileOP->Seek (nPos, uFlag);
}

long long vompSourceOPSize (void * pHandle)
{
	if (pHandle == NULL)
		return 0;

	voCDataSourceOP * pFileOP = (voCDataSourceOP *)pHandle;

	return pFileOP->Size ();
}

long long vompSourceOPSave (void * pHandle)
{
	if (pHandle == NULL)
		return 0;

	voCDataSourceOP * pFileOP = (voCDataSourceOP *)pHandle;

	return pFileOP->Save ();
}

VO_S32 vompSourceOPClose (VO_PTR pHandle)
{
	if (pHandle == NULL)
		return 0;

	voCDataSourceOP * pFileOP = (voCDataSourceOP *)pHandle;

	pFileOP->Close ();
	delete pFileOP;

	return 0;
}

int	voSourceOPFillPointer (void)
{
	g_vompSourceOP.Open = vompSourceOPOpen;
	g_vompSourceOP.Read = vompSourceOPRead;
	g_vompSourceOP.Write = vompSourceOPWrite;
	g_vompSourceOP.Flush = vompSourceOPFlush;
	g_vompSourceOP.Seek = vompSourceOPSeek;
	g_vompSourceOP.Size = vompSourceOPSize;
	g_vompSourceOP.Save = vompSourceOPSave;
	g_vompSourceOP.Close = vompSourceOPClose;

	return 0;
}

voCDataSourceOP::voCDataSourceOP(void)
{
	m_nFilePos = 0;
	m_pDataSource = NULL;
}

voCDataSourceOP::~voCDataSourceOP(void)
{
	Close ();
}

void * voCDataSourceOP::Open (VO_FILE_SOURCE * pSource)
{
	if (pSource == NULL)
		return NULL;

	memcpy (&m_Source, pSource, sizeof (VO_FILE_SOURCE));
	m_pDataSource = (VOMP_DATASOURCE_FUNC *)m_Source.pSource;

	m_nFilePos = 0;

	return m_Source.pSource;
}

int voCDataSourceOP::Read (void * pBuffer, int uSize)
{
	int uRead = 0;

	if (m_pDataSource == NULL)
		return -0;

//	VOLOGI ("Data %p, read %p, buff %p, size %d, pos %d", m_pDataSource->pUserData, m_pDataSource->ReadAt, pBuffer, uSize, (int)m_nFilePos);

	uRead = m_pDataSource->ReadAt (m_pDataSource->pUserData, m_nFilePos, (unsigned char *)pBuffer, uSize);

	if (uRead != uSize)
		VOLOGW ("Request size %d, read %d", uSize, uRead);

/*
	VOLOGI ("Read Size %d", uRead);
	if (uRead > 8)
	{
		unsigned char * pData = (unsigned char *)pBuffer;
		VOLOGI ("Read buffer 0X%02X%02X%02X%02X%02X%02X%02X%02X", pData[0], pData[1], pData[2], pData[3], pData[4], pData[5], pData[6], pData[7]);
	}
*/

	m_nFilePos = m_nFilePos + uRead;

	return uRead;
}

int voCDataSourceOP::Write (void * pBuffer, int uSize)
{
	return 0;
}

int voCDataSourceOP::Flush (void)
{
	return 0;
}

long long voCDataSourceOP::Seek (long long nPos, VO_FILE_POS uFlag)
{
//	VOLOGI ("Pos is %d, flag is %d", (int)nPos, uFlag);

	if (uFlag == VO_FILE_BEGIN)
		m_nFilePos = nPos;
	else if (uFlag == VO_FILE_CURRENT)
		m_nFilePos = m_nFilePos + nPos;
	else
		m_nFilePos = FileSize () - nPos;

	if (m_nFilePos < 0)
		VOLOGE ("m_nFilePos error %d", (int)m_nFilePos);

//	VOLOGI ("m_nFilePos is %d", (int)m_nFilePos);

	return m_nFilePos;
}

long long voCDataSourceOP::Size (void)
{
	return FileSize ();
}

long long voCDataSourceOP::Save (void)
{
	return 0;
}

int voCDataSourceOP::Close (void)
{
	return 0;
}

long long voCDataSourceOP::FileSize (void)
{
	long long llSize = 0;

	if (m_pDataSource == NULL)
		return 0;

	m_pDataSource->GetSize (m_pDataSource->pUserData, &llSize);

//	VOLOGI ("FileSize is %d", (int)llSize);

	return llSize;
}

