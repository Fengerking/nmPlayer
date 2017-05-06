//
//  CHLSHttpSocket.cpp
//  
//
//  Created by Jim Lin on 9/11/12.
//
//

#include "CHLSHttpSocket.h"
#include "CHLSHttpRequest.h"
#include "CHLSHttpRespond.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CHLSHttpSocket::CHLSHttpSocket(CHttpBaseStream * pStream)
:CHttpSocket(pStream)
{
}

CHLSHttpSocket::~CHLSHttpSocket()
{
}


int CHLSHttpSocket::DoneRead (void)
{
	if (m_pRequest == NULL)
		m_pRequest = new CHLSHttpRequest ();
	if (m_pRespond == NULL)
		m_pRespond = new CHLSHttpRespond (m_pStream);
    
	if (m_nSizeRead <= 0)
		return -1;
    
	m_pRequest->Reset ();
    //VOLOGI("[NPW]Socket %d read", m_hSocket);
	m_pRequest->Parse ((char *)m_pBuffRead, m_nSizeRead);
    
    SendEvent(SOCKET_EVENT_HTTP_REQUEST, m_pRequest, this);
    
    //Update stream length
    if(m_pStream)
    {
        m_pStream->SetStartPos(m_pRequest->m_lPosBegin);
        
        if(m_pRequest->m_lPosEnd > 0)
            m_pStream->SetEndPos(m_pRequest->m_lPosEnd);
        else
            m_pStream->SetEndPos(m_pStream->Size());
    }
    
    if(IsPartialContent())
    {
        VOLOGI("[NPW]IsPartialContent %d", m_hSocket);
        m_pRespond->UpdateErrCode(206);
    }
    else
    {
        m_pRespond->UpdateErrCode(200);
    }
    
	m_nSizeWrite =	SO_BUFF_SIZE;
	memset (m_pBuffWrite, 0, SO_BUFF_SIZE);
	if (m_pRespond->Create (m_pRequest, m_pBuffWrite, &m_nSizeWrite))
	{
		if (m_nSizeWrite > 0)
        {
            WriteBuff (m_pBuffWrite, m_nSizeWrite);
            //VOLOGI("[NPW]send HTTP response: %d", m_nSizeWrite);
        }
	}
    
	return 0;
}


