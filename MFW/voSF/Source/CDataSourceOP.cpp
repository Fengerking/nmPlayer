	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voFileOP.c

	Contains:	memory operator implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-14		JBF			Create file

*******************************************************************************/
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifdef _LINUX_ANDROID
#include <unistd.h>
#include <wchar.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif // _LINUX_ANDROID

#include "voSF.h"
#include "CDataSourceOP.h"

#include "voLog.h"

VOSF_FILE_OPERATOR	g_vosfFileOP;

class CDataSourceOP
{
public:
    CDataSourceOP(void);
    virtual ~CDataSourceOP(void);

	virtual void *		Open (vosfFileOpenSource * pSource);
	virtual int 		Read (void * pBuffer, int uSize);
	virtual int 		Write (void * pBuffer, int uSize);
	virtual int 		Flush (void);
	virtual long long 	Seek (long long nPos, VOSF_FILE_POS uFlag);
	virtual long long 	Size (void);
	virtual long long 	Save (void);
	virtual int 		Close (void);

protected:
	virtual long long	FileSize (void);

protected:
	vosfFileOpenSource		m_Source;
	long long				m_nFilePos;

	VOSF_DATASOURCETYPE *	m_pDataSource;
};

void * voFileOPOpen (vosfFileOpenSource * pSource)
{
//	LOGW ("@@@voFileOPOpen Open %d", pSource);

	if (pSource == NULL)
		return NULL;

	CDataSourceOP * pFileOP = new CDataSourceOP ();
	if (pFileOP == NULL)
		return NULL;

	pFileOP->Open (pSource);

	return pFileOP;
}

int voFileOPRead (void * pHandle, void * pBuffer, int uSize)
{
//	LOGW ("voFileOPRead read handle %d, size %d", pHandle, uSize);

	if (pHandle == NULL || pBuffer == 0)
		return 0;

	CDataSourceOP * pFileOP = (CDataSourceOP *)pHandle;

	return pFileOP->Read (pBuffer, uSize);
}

int voFileOPWrite (void * pHandle, void * pBuffer, int uSize)
{
	if (pHandle == NULL || pBuffer == 0)
		return 0;

	CDataSourceOP * pFileOP = (CDataSourceOP *)pHandle;

	return pFileOP->Write (pBuffer, uSize);
}

int voFileOPFlush (void * pHandle)
{
	if (pHandle == NULL)
		return 0;

	CDataSourceOP * pFileOP = (CDataSourceOP *)pHandle;

	return pFileOP->Flush ();
}

long long voFileOPSeek (void * pHandle, long long nPos, VOSF_FILE_POS uFlag)
{
	if (pHandle == NULL)
		return 0;

	CDataSourceOP * pFileOP = (CDataSourceOP *)pHandle;

	return pFileOP->Seek (nPos, uFlag);
}

long long voFileOPSize (void * pHandle)
{
	if (pHandle == NULL)
		return 0;

	CDataSourceOP * pFileOP = (CDataSourceOP *)pHandle;

	return pFileOP->Size ();
}

long long voFileOPSave (void * pHandle)
{
	if (pHandle == NULL)
		return 0;

	CDataSourceOP * pFileOP = (CDataSourceOP *)pHandle;

	return pFileOP->Save ();
}

int voFileOPClose (void * pHandle)
{
	if (pHandle == NULL)
		return 0;

	CDataSourceOP * pFileOP = (CDataSourceOP *)pHandle;

	pFileOP->Close ();
	delete pFileOP;

	return 0;
}

int	voFileFillPointer (void)
{
	g_vosfFileOP.Open = voFileOPOpen;
	g_vosfFileOP.Read = voFileOPRead;
	g_vosfFileOP.Write = voFileOPWrite;
	g_vosfFileOP.Flush = voFileOPFlush;
	g_vosfFileOP.Seek = voFileOPSeek;
	g_vosfFileOP.Size = voFileOPSize;
	g_vosfFileOP.Save = voFileOPSave;
	g_vosfFileOP.Close = voFileOPClose;

	return 0;
}

CDataSourceOP::CDataSourceOP(void)
{
	m_nFilePos = 0;
	m_pDataSource = NULL;
}

CDataSourceOP::~CDataSourceOP(void)
{
	Close ();
}

void * CDataSourceOP::Open (vosfFileOpenSource * pSource)
{
	if (pSource == NULL)
		return NULL;

	memcpy (&m_Source, pSource, sizeof (vosfFileOpenSource));
	m_pDataSource = (VOSF_DATASOURCETYPE *)m_Source.pSource;

	return m_Source.pSource;
}

int CDataSourceOP::Read (void * pBuffer, int uSize)
{
	int uRead = 0;

	if (m_pDataSource == NULL)
		return -0;

//	VOLOGI ("Data %p, read %p, buff %p, size %d, pos %d", m_pDataSource->pDataSource, m_pDataSource->readAt, pBuffer, uSize, (int)m_nFilePos);

	uRead = m_pDataSource->readAt (m_pDataSource->pDataSource, m_nFilePos, pBuffer, uSize);

//	VOLOGI ("Read Size %d", uRead);

	m_nFilePos = m_nFilePos + uSize;

	return uRead;
}

int CDataSourceOP::Write (void * pBuffer, int uSize)
{
	return 0;
}

int CDataSourceOP::Flush (void)
{
	return 0;
}

long long CDataSourceOP::Seek (long long nPos, VOSF_FILE_POS uFlag)
{
//	VOLOGI ("Pos is %lld, flag is %d", nPos, uFlag);

	if (uFlag == VOSF_FILE_BEGIN)
		m_nFilePos = nPos;
	else if (uFlag == VOSF_FILE_CURRENT)
		m_nFilePos = m_nFilePos + nPos;
	else
		m_nFilePos = FileSize () - nPos;

//	VOLOGI ("m_nFilePos is %lld", m_nFilePos);

	return m_nFilePos;
}

long long CDataSourceOP::Size (void)
{
	return FileSize ();
}

long long CDataSourceOP::Save (void)
{
	return 0;
}

int CDataSourceOP::Close (void)
{
	return 0;
}

long long CDataSourceOP::FileSize (void)
{
	long long llSize = 0;

	if (m_pDataSource == NULL)
		return 0;

	m_pDataSource->getSize (m_pDataSource->pDataSource, &llSize);

	return llSize;
}

