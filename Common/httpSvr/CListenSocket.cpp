	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2011				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CListenSocket.cpp

	Contains:	CListenSocket class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2011-12-21		JBF			Create file

*******************************************************************************/
#include <stdio.h>

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

#include "voOSFunc.h"
#include "vo_socket.h"

#include "CListenSocket.h"

#include "CHttpSocket.h"

#define LOG_TAG "CListenSocket"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CListenSocket::CListenSocket(void * hUserData, int nPortNum)
	: CBaseSocket ()
	, m_hUserData (hUserData)
	, m_nPortNum (nPortNum)
	, m_nMaxClient (128)
	, m_nConnectClientNum (0)
	, m_pStream (NULL)
{
	m_aClientSocket = new CBaseSocket *[m_nMaxClient];
	memset (m_aClientSocket, 0, sizeof (CBaseSocket *) * m_nMaxClient);
}

CListenSocket::~CListenSocket ()
{
	Close ();

	for (int i = 0; i < m_nMaxClient; i++)
	{
		if (m_aClientSocket[i] != NULL)
			delete m_aClientSocket[i];
	}

	delete []m_aClientSocket;

}

int CListenSocket::Create (void)
{	    
	CBaseSocket::Create ();

    struct sockaddr_in server_addr;

    memset (&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port=htons(m_nPortNum);

	if (bind(m_hSocket, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr))==-1)
	{
		VOLOGE ("+++++++++It could not bind local IP! %s, %d, %d+++++++++", inet_ntoa(server_addr.sin_addr), m_nPortNum, m_hSocket);
		return -1;
	}
    else
    {
        VOLOGI("bind success!!! %s, %d, %d+++++++++", inet_ntoa(server_addr.sin_addr), m_nPortNum, m_hSocket);
    }

    if (listen(m_hSocket, m_nMaxClient)==-1)
	{
		VOLOGE ("It could not listen on the socket!");
		return -1;
	}

	VOLOGI ("It Created the listen socket successfully!");

	return 0;
}

int CListenSocket::Open (void * pSource, int nType)
{
//	if (m_pStream != NULL)
//	{
//		m_pStream->Close ();
//		delete m_pStream;
//		m_pStream = NULL;
//	}
//
//	m_pStream = new CHttpFileStream (m_hUserData);
//	bool bRC = m_pStream->Open (pSource, nType);

//	if (bRC)
//		return 0;
//	else
//		return -1;
    
    return 0;
}

int CListenSocket::Stop (void)
{
	if (m_hThread == NULL)
		return 0;
    
    VOLOGI ("Try to stop listen socket thread!");
	
	m_bStop = true;
	while (m_hThread != NULL)
		voOS_Sleep (2);

    
    VOLOGI ("Try to close listen socket!");
    
	if (m_hSocket != -1)
	{
#ifdef _WIN32
		closesocket (m_hSocket);
#else
		close (m_hSocket);
#endif // _WIN32
		m_hSocket = -1;
	}
    

	VOLOGI ("It stopped the listen socket!");

	for (int i = 0; i < m_nMaxClient; i++)
	{
        voCAutoLock lock(&m_cClientLock);
        
		if (m_aClientSocket[i] != NULL)
			m_aClientSocket[i]->Stop ();
	}

	VOLOGI ("It stopped all client socket!");

	return 0;
}

int CListenSocket::Close (void)
{
	if (m_hSocket != -1)
		Stop ();

	CBaseSocket::Close ();
    
	for (int i = 0; i < m_nMaxClient; i++)
	{
        voCAutoLock lock(&m_cClientLock);
        
		if (m_aClientSocket[i] != NULL)
        {
            m_aClientSocket[i]->Close ();
            delete m_aClientSocket[i];
            m_aClientSocket[i] = NULL;
        }
	}
    
    m_nConnectClientNum = 0;

	return 0;
}

int CListenSocket::CheckEventLoop (void)
{
	return CBaseSocket::CheckEventLoop ();
}

int CListenSocket::OnRead (void)
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
		VOLOGI ("It accept client socket error!");       	
		return -1;
    }

	VOLOGI ("It accepted one client socket from %s Connect num %d", inet_ntoa(client_addr.sin_addr), m_nConnectClientNum);

	for (int i = 0; i < m_nMaxClient; i++)
	{
        voCAutoLock lock(&m_cClientLock);
        
		if (m_aClientSocket[i] == NULL)
		{
			m_aClientSocket[i] = new CHttpSocket (m_pStream);
			m_aClientSocket[i]->SetSocket (nNewSocket);
			m_aClientSocket[i]->Start ();
			break;
		}
	}

	m_nConnectClientNum++;
	
	return 0;
}

int CListenSocket::DoneRead (void)
{		
	return 0;
}

int CListenSocket::SetParam(int nID, void * pParam)
{
//	if (nID == VOHSS_PID_NOTIFY_FUNC)
//		m_fNotify = (vohssNotify)pParam;

	return -1;
}

int CListenSocket::GetParam(int nID, void * pParam)
{
	return -1;
}



int CListenSocket::SetStream(CHttpBaseStream * pStream)
{
    m_pStream = pStream;
    
    return 0;
}

int CListenSocket::Flush()
{
	for (int i = 0; i < m_nMaxClient; i++)
	{
        voCAutoLock lock(&m_cClientLock);
        
		if (m_aClientSocket[i] != NULL)
        {
            m_aClientSocket[i]->Stop ();
            m_aClientSocket[i]->Close ();
            delete m_aClientSocket[i];
            m_aClientSocket[i] = NULL;
        }
	}
    
	m_nConnectClientNum = 0;
    
    return 0;
}




