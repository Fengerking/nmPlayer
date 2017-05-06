	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2011				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CHttpStream.cpp

	Contains:	CHttpStream class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2011-12-21		JBF			Create file

*******************************************************************************/
#include <stdio.h>

#ifdef _WIN32
#include <io.h>
#include <string>
#else
#include <ctype.h>
#endif // _WIN32

#include "voOSFunc.h"
#include "cmnFile.h"
#include "CHttpStream.h"

#define LOG_TAG "CHttpStream"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CHttpFileStream::CHttpFileStream(void * hUserData)
	: CHttpBaseStream (hUserData)
	, m_hFile (NULL)
{
	memset (&m_sSource, 0, sizeof (m_sSource));

#ifdef _WIN32
	vostrcpy (m_szFileName, _T("c:\\01.mp4"));
#else
	vostrcpy (m_szFileName, _T("/sdcard/adobe/kobe.mp4"));
#endif // _WIN32
	
}

CHttpFileStream::~CHttpFileStream ()
{
   Close ();
}

bool CHttpFileStream::Open (void * pSource, int nFlag)
{
	if (g_fileOP.Open == NULL)
		cmnFileFillPointer ();

    CHttpBaseStream::Open(pSource, nFlag);

    //tag:
	//if (nFlag == VOHSS_SOURCE_URL && pSource != NULL)
	if(pSource != NULL)
		vostrcpy (m_szFileName, (VO_TCHAR *)pSource);

	m_sSource.pSource = m_szFileName;
	m_sSource.nFlag = VO_FILE_TYPE_NAME;
	m_sSource.nMode = VO_FILE_READ_ONLY;

	m_hFile = g_fileOP.Open (&m_sSource);
	if (m_hFile != NULL && m_llEndPos == 0)
		m_llEndPos = Size ();
    
    if(!m_hFile)
        m_bStreamAvailable = false;
    else
    {
        m_bStreamAvailable = true;
    }
    

	return m_hFile != NULL;
}

bool CHttpFileStream::Close (void)
{
	if (m_hFile != NULL)
		g_fileOP.Close (m_hFile);
	m_hFile = NULL;

	return true;
}

int CHttpFileStream::Read (unsigned char * pBuff, int nSize)
{
	if (m_hFile == NULL)
		return 0;
    
	return g_fileOP.Read (m_hFile, pBuff, nSize);
}

bool CHttpFileStream::Seek (long long llPos, int nFlag)
{
	if (m_hFile == NULL)
		return false;

	return g_fileOP.Seek (m_hFile, llPos, (VO_FILE_POS) nFlag);
}

long long CHttpFileStream::Size (void)
{
	if (m_hFile == NULL)
		return 0;

	return g_fileOP.Size (m_hFile);
}

// Sink source stream
CHttpSinkStream::CHttpSinkStream(void * hUserData)
	: CHttpBaseStream (hUserData)
	, m_pBuffer (NULL)
    , m_nCurrPos(0)
    , m_nSize(0)
    , m_nMemSize(0)
{
}

CHttpSinkStream::~CHttpSinkStream ()
{
    Close();
}

bool CHttpSinkStream::Open (void * pSource, int nFlag)
{
    Close();
    
    CHttpBaseStream::Open(pSource, nFlag);
    
    return true;
}

bool CHttpSinkStream::Close (void)
{
    CHttpBaseStream::Close();
    
    //VOLOGI("[NPW]%x release %x, %lld", this, m_pBuffer, m_nSize);
    
    m_nCurrPos = 0;
    m_nSize    = 0;
    m_llEndPos = 0;
    m_nMemSize = 0;

    if (m_pBuffer != NULL)
    {
        delete []m_pBuffer;
        m_pBuffer = NULL;
    }
    
    return true;
}

int CHttpSinkStream::Read (unsigned char * pBuff, int nSize)
{
	if (m_pBuffer == NULL)
    {
        return 0;
    }

    if(m_nCurrPos >= m_nSize)
    {
        return 0;
    }

    if(m_nCurrPos >= m_llEndPos)
    {
        return 0;
    }
            
    int nLeft = m_llEndPos - m_nCurrPos;
    
    if(nLeft >= nSize)
    {
        memcpy(pBuff, m_pBuffer+m_nCurrPos, nSize);
        m_nCurrPos += nSize;
        return nSize;
    }
    else
    {
        memcpy(pBuff, m_pBuffer+m_nCurrPos, nLeft);
        m_nCurrPos += nLeft;
        return nLeft;
    }
    
    return 0;
}

bool CHttpSinkStream::Seek (long long llPos, int nFlag)
{
    //VOLOGI("[NPW]Stream seek to %lld, curr pos %lld, size %lld", llPos, m_nCurrPos, m_nSize);
    
    long long nWant = m_nCurrPos + llPos;
    
    if(nWant > m_nSize)
    {
        VOLOGE("[NPW]Seek failed!!!");
        return false;
    }
            
    m_nCurrPos = llPos;

    return true;
}

long long CHttpSinkStream::Size (void)
{
	if (m_pBuffer == NULL)
		return 0;
    
	return m_nSize;
}

void CHttpSinkStream::UpdateStream(unsigned char* pBuf, int nSize)
{
    //VOLOGI("[NPW]Stream update %d", nSize);
    
    //Close();
    
    if(m_pBuffer != pBuf)
    {
        VOLOGE("[NPW]Update stream error!!!");
    }
    
    m_nCurrPos  = 0;
    m_pBuffer   = pBuf;
    m_nSize     = nSize;
    
    SetAvailable(true);
    
    m_llEndPos = nSize;
}

unsigned char* CHttpSinkStream::AllocBuffer(int nSize)
{    
    if(!m_pBuffer || m_nMemSize<nSize)
    {
        if(m_pBuffer)
            delete []m_pBuffer;
        
        m_nMemSize = nSize;
        m_pBuffer = new unsigned char[m_nMemSize];
        //VOLOGI("[NPW]%x alloc %x", this, m_pBuffer);
    }
    
    return m_pBuffer;
}

bool CHttpSinkStream::IsEOS()
{
    return m_nCurrPos>=m_nSize;
}


unsigned char* CHttpSinkStream::GetBuffer()
{
    return m_pBuffer;
}

