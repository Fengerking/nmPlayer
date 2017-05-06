	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2011				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CHttpSocket.cpp

	Contains:	CHttpSocket class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2011-12-21		JBF			Create file

*******************************************************************************/
#include <stdio.h>

#ifdef _WIN32
#include <iostream>
#include <io.h>
#include <string>
#include <winsock2.h>
#else
#include "unistd.h"
#include "netdb.h"
#include "fcntl.h"
#endif // _WIN32

#include "voOSFunc.h"
#include "CHttpSocket.h"

#include "vo_socket.h"

#define LOG_TAG "CHttpSocket"
#include "voLog.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CHttpSocket::CHttpSocket(CHttpBaseStream * pStream)
	: CBaseSocket ()
	, m_pStream (pStream)
	, m_pRequest (NULL)
	, m_pRespond (NULL)
	, m_llSendSize (0)
    , m_bStreamEOS(false)
{
}

CHttpSocket::~CHttpSocket ()
{
	Close ();

	if (m_pRespond != NULL)
		delete m_pRespond;
	if (m_pRequest != NULL)
		delete m_pRequest;
}

int CHttpSocket::Close (void)
{
	int nRC = CBaseSocket::Close ();

	if (m_pRespond != NULL)
		m_pRespond->Close ();

	return nRC;
}

int CHttpSocket::DoneRead (void)
{
	if (m_pRequest == NULL)
		m_pRequest = new CHttpRequest ();
	if (m_pRespond == NULL)
		m_pRespond = new CHttpRespond (m_pStream);

	if (m_nSizeRead <= 0)
		return -1;

	m_pRequest->Reset ();
	m_pRequest->Parse ((char *)m_pBuffRead, m_nSizeRead);

    // maybe it will update m_pStream in this callback
    SendEvent(SOCKET_EVENT_HTTP_REQUEST, m_pRequest, this);
    
	m_nSizeWrite =	SO_BUFF_SIZE;
	memset (m_pBuffWrite, 0, SO_BUFF_SIZE);
	if (m_pRespond->Create (m_pRequest, m_pBuffWrite, &m_nSizeWrite))
	{
		if (m_nSizeWrite > 0)
			WriteBuff (m_pBuffWrite, m_nSizeWrite);
	}

	return 0;
}

int CHttpSocket::DoneWrite (void)
{
	if (m_pRespond == NULL)
		return 0;
    
    int nRet = -1;

	int nSize = m_pRespond->ReadStream (m_pBuffWrite, SO_BUFF_SIZE);
	if (nSize > 0)
 		nRet = WriteBuff (m_pBuffWrite, nSize);
    else
    {
    }
    
    if(-1 == nRet)
    {
        voOS_Sleep(5);
        return -1;
    }
    
	m_llSendSize += nSize;
    
    int nStreamSize = m_pStream->Size();
    int nTotalSize  = 0;
    
    if(IsPartialContent())
    {
        nTotalSize = m_pStream->GetEndPos()-m_pStream->GetStartPos();
        VOLOGI("[NPW]Total size %d", nTotalSize);
    }
    else
        nTotalSize = nStreamSize;
    
    if(m_llSendSize >= nTotalSize)
    {
        if(m_pStream)
            SendEvent(SOCKET_EVENT_STREAM_EOS, this, m_pStream);
        //VOLOGI("[NPW]%x, buffer send finished", this);
        //maybe need double check stream is EOS
        m_pStream = NULL;
        m_pRespond->UpdateStream(NULL);
        m_bStreamEOS = true;
    }

	if(nSize > 0 && nRet != -1)
	{
		//VOLOGI ("Socket %d Send Size  % 4d. Total Size % 8lld", m_hSocket, nSize, m_llSendSize);
        
        if(m_llSendSize > nStreamSize)
        {
            VOLOGE("Socket %d Send Size  % 4lld large than Stream Size % d", m_hSocket, m_llSendSize , nStreamSize);
        }
	}
	
	return 0;
}

int CHttpSocket::SetStream(CHttpBaseStream * pStream)
{
    //VOLOGI("[NPW]Set stream %s", pStream->GetSourceURL());
    m_pStream = pStream;
    
    if(m_pRespond)
    {
        //VOLOGI("[NPW]Response update stream %s", pStream->GetSourceURL());
        m_pRespond->UpdateStream(pStream);
    }
    
    m_bStreamEOS = false;
    m_llSendSize = 0;
    return 0;
}

bool CHttpSocket::IsPartialContent()
{
    if(!m_pRequest || !m_pStream)
        return false;
    
    long long nAvailableSize = m_pStream->GetEndPos() - m_pStream->GetStartPos();
    
    if(nAvailableSize < m_pStream->Size())
        return true;
    
    return false;
}

bool CHttpSocket::IsStreamSendComplete()
{
    return m_bStreamEOS;
}
