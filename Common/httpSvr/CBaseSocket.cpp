	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2011				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseSocket.cpp

	Contains:	CBaseSocket class file

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
#include "CBaseSocket.h"

#include "vo_socket.h"

#define LOG_TAG "CBaseSocket"
#include "voLog.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CBaseSocket::CBaseSocket(void)
	: m_hSocket (-1)
	, m_hThread (NULL)
	, m_bStop (true)
	, m_pBuffRead (NULL)
	, m_nSizeRead (0)
	, m_pBuffWrite (NULL)
	, m_nSizeWrite (0)
{
	m_tmout.tv_sec = 0;
	m_tmout.tv_usec = 20000;

	m_pBuffRead = new unsigned char[SO_BUFF_SIZE];
  	m_pBuffWrite = new unsigned char[SO_BUFF_SIZE];
    
    memset(&m_EventCallback, 0, sizeof(SOCKET_EVENT_CALLBACK_INFO));
}

CBaseSocket::~CBaseSocket ()
{
	Close ();

    if(m_pBuffRead)
    {
        delete []m_pBuffRead;
        m_pBuffRead = NULL;
    }
    
    if(m_pBuffWrite)
    {
        delete []m_pBuffWrite;
        m_pBuffWrite = NULL;        
    }
}

int CBaseSocket::Create (void)
{
	if (m_hSocket != -1)
		return 0;

	int nSocket = -1;
	if((nSocket = socket (AF_INET, SOCK_STREAM, 0)) == -1)  
		return -1;
    
    int flag    = 1;
    int len     =sizeof(int);
    setsockopt(nSocket, SOL_SOCKET, SO_REUSEADDR, &flag, len);

    VOLOGI("Create socket ok!");
	return SetSocket (nSocket);
}

int CBaseSocket::SetSocket (int nSocket)
{
	if (m_hSocket != -1)
		return 0;

	m_hSocket = nSocket;

	return 0;
}

int CBaseSocket::Start (void)
{
	if (m_hThread != NULL)
		return 0;

	VO_U32	nID = 0;
	m_bStop = false;
	voThreadCreate (&m_hThread, &nID, (voThreadProc)SocketThreadProc, this, 0);

	return 0;
}

int CBaseSocket::Stop (void)
{
	m_bStop = true;
	while (m_hThread != NULL)
		voOS_Sleep (2);

	return 0;
}

int CBaseSocket::Close (void)
{
	Stop ();

	if (m_hSocket != -1)
	{
#ifdef _WIN32
//		shutdown (m_hSocket, 2);
		closesocket (m_hSocket);
#else
		close (m_hSocket);
#endif // _WIN32
		m_hSocket = -1;
	}

	return 0;
}

int CBaseSocket::WriteBuff (unsigned char * pBuffer, int nBuffSize)
{
    if(!pBuffer || nBuffSize<=0)
        return -1;
    
	return vo_socket_send_safe (m_hSocket, (VO_PBYTE) pBuffer, nBuffSize);
}

int CBaseSocket::SocketThreadProc (VO_PTR pParam)
{
	CBaseSocket * pSocket = (CBaseSocket *)pParam;

	pSocket->CheckEventLoop ();

	return 0;
}

int CBaseSocket::CheckEventLoop (void)
{
	VO_S32	nRC = 0;
	//int		nError = 0;
    
    VOLOGI("Enter socket thread");

	while (!m_bStop)
	{
        if(m_hSocket == -1)
        {
            voOS_Sleep(2);
            continue;
        }
        
		FD_ZERO(&m_fdsRead);
		FD_SET(m_hSocket, &m_fdsRead);

		FD_ZERO(&m_fdsWrite);
		FD_SET(m_hSocket, &m_fdsWrite);

		FD_ZERO(&m_fdsExcept);
		FD_SET(m_hSocket, &m_fdsExcept);

		nRC = select (m_hSocket + 1 ,&m_fdsRead, &m_fdsWrite, &m_fdsExcept, &m_tmout);
		if (nRC == -1)
		{
			VOLOGI ("It return error when call select function!");
			// Socket Error
#ifdef _WIN32
			nError = WSAGetLastError ();
#endif // _WIN32
			break;
		}
		else if(nRC == 0)
		{
			voOS_Sleep( 20 );
			continue;
		}
		else
		{
			if (FD_ISSET (m_hSocket, &m_fdsRead))
			{
				int nRet = OnRead ();
                
                if(nRet == -1)
                    voOS_Sleep(2);
			}

			if (FD_ISSET (m_hSocket, &m_fdsWrite))
			{
				DoneWrite ();
			}

			if (FD_ISSET (m_hSocket, &m_fdsExcept))
			{
				OnExcept ();
			}
		}
	}

    VOLOGI("Exit socket thread");
	m_hThread = NULL;

	return 0;
}

int CBaseSocket::OnRead (void)
{
    if(m_hSocket <= -1 || !m_pBuffRead)
        return -1;
    
	m_nSizeRead = SO_BUFF_SIZE;

	while (m_nSizeRead == SO_BUFF_SIZE)
	{
		m_nSizeRead = vo_socket_recv (m_hSocket, (VO_PBYTE)m_pBuffRead, SO_BUFF_SIZE);

		if (m_nSizeRead > 0)
			DoneRead ();
	}

	return 0;
}

int CBaseSocket::OnWrite (void)
{

	return 0;
}

int CBaseSocket::OnExcept (void)
{
    VOLOGW("[NPW]OnExcept");
	return 0;
}

int CBaseSocket::DoneRead (void)
{
	return 0;
}

int CBaseSocket::DoneWrite (void)
{
	return 0;
}

void CBaseSocket::SetEventCallback(SOCKET_EVENT_CALLBACK_INFO* pInfo)
{
    if(!pInfo)
        return;
    
    m_EventCallback.pUserData   = pInfo->pUserData;
    m_EventCallback.pListener   = pInfo->pListener;
}


int CBaseSocket::SendEvent(int nID, void * pParam1, void * pParam2)
{
    if(m_EventCallback.pUserData && m_EventCallback.pListener)
        return m_EventCallback.pListener(m_EventCallback.pUserData, nID, pParam1, pParam2);
    
    return 0;
}





