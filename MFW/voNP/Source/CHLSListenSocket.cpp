//
//  CListenSocketHLS.cpp
//  
//
//  Created by Jim Lin on 9/11/12.
//
//

#include "CHLSListenSocket.h"
#include "voLog.h"
#include "CHLSHttpSocket.h"


#ifdef _WIN32
#include <iostream>
#include <io.h>
#include <winsock2.h>
#include <string>
#elif defined (_IOS) || defined (_MAC_OS)
#include <arpa/inet.h>
#else
#include "unistd.h"
#include "netdb.h"
#include "fcntl.h"
#include "linux/in.h"
#include "sys/endian.h"
#include "setjmp.h"
#include "arpa/inet.h"
#endif // _WIN32

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CHLSListenSocket::CHLSListenSocket(void * hUserData, int nPortNum)
:CListenSocket(hUserData, nPortNum)
{
}

CHLSListenSocket::~CHLSListenSocket()
{
    
}

int CHLSListenSocket::Open(void * pSource, int nType)
{
    //VOLOGI("[NPW]Listener socket open %x", (unsigned int)this);
    return 0;
}


int CHLSListenSocket::OnRead (void)
{
	VOLOGI ("Call On Read!");
	
	//VO_S32				nRC = 0;
	int					nNewSocket = -1;
	int					sin_size = 0;
    struct sockaddr_in	client_addr;
    
    
    sin_size=sizeof(struct sockaddr_in);
    
#if defined (_IOS) || defined (_MAC_OS)
    if((nNewSocket = accept(m_hSocket, (struct sockaddr *)(&client_addr), (socklen_t*)&sin_size))==-1)
#else
    if((nNewSocket = accept(m_hSocket, (struct sockaddr *)(&client_addr), &sin_size))==-1)
#endif
    {
        VOLOGI ("[NPW]It accept client socket error!");
        SendEvent(SOCKET_EVENT_SOCKET_ACCEPT_FAILED, NULL, NULL);
        return -1;
    }
    
	VOLOGI ("[NPW]SYY It accepted one client socket from %s Connect num %d, sock %d", inet_ntoa(client_addr.sin_addr), m_nConnectClientNum+1, nNewSocket);
    
    // remove useless socket resource
    if(m_nConnectClientNum >= 2)
    {
        voCAutoLock lock(&m_cClientLock);
 
        //remove from second socket,first is m3u8 stream
        for (int i = 1; i < m_nMaxClient; i++)
        {
            CHttpSocket* pSocket = (CHttpSocket*)m_aClientSocket[i];
            
            if(pSocket)
            {
                if (pSocket->IsStreamSendComplete())
                {
                    VOLOGI("[NPW]SYY Remove socket idx: %d", i);
                    pSocket->Stop();
                    pSocket->Close();
                    delete  pSocket;
                    m_aClientSocket[i] = NULL;
                    m_nConnectClientNum--;
                    break;
                }
            }
        }

//        int nIdx = m_nConnectClientNum - 1;
//        
//        CBaseSocket* pSocket = m_aClientSocket[nIdx];
//        
//        if (pSocket)
//        {
//            VOLOGI("[NPW]SYY Remove socket idx: %d", nIdx);
//            pSocket->Stop();
//            pSocket->Close();
//            delete  pSocket;
//            m_aClientSocket[nIdx] = NULL;
//            m_nConnectClientNum--;
//        }
//        else
//        {
//            VOLOGI("[NPW]SYY Can't find socket idx: %d", nIdx);
//        }

    }
    
	for (int i = 0; i < m_nMaxClient; i++)
	{
        voCAutoLock lock(&m_cClientLock);
        
		if (m_aClientSocket[i] == NULL)
		{
            m_aClientSocket[i] = new CHLSHttpSocket (NULL);
			m_aClientSocket[i]->SetSocket (nNewSocket);
            m_aClientSocket[i]->SetEventCallback(&m_EventCallback);
			m_aClientSocket[i]->Start ();
            VOLOGI("[NPW]SYY New socket created %x, idx %d", (unsigned int)m_aClientSocket[i], i);
			break;
		}
	}
    
	m_nConnectClientNum++;
    
    if(m_nConnectClientNum > m_nMaxClient)
    {
        VOLOGE("[NPW]Client count large than max count");
    }
	
	return 0;
}

