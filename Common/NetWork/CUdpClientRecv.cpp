/*
 *  CUdpClientRecv.cpp
 *
 *  Created by Lin Jun on 8/24/11.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#include "CUdpClientRecv.h"
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CUdpClientRecv::CUdpClientRecv()
:m_hSocket(-1)
,m_nMaxRecvBuffSize(128*1024)
{
}

CUdpClientRecv::~CUdpClientRecv(void)
{
	Uninit();
}


int CUdpClientRecv::Init(int nPort)
{
	if(m_hSocket > 0)
		return -1;

#ifdef _WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD( 2, 2 );
	err = WSAStartup( wVersionRequested, &wsaData );

	if( err != 0 )
		return -1;
#endif
	
	struct sockaddr_in  server;
	
    m_hSocket = socket( AF_INET, SOCK_DGRAM, 0 );
    if (m_hSocket < 0)
	{
		printf("socket error\n");
		return -1;
	}

    setsockopt(m_hSocket, SOL_SOCKET, SO_RCVBUF, (char *) &m_nMaxRecvBuffSize, sizeof(m_nMaxRecvBuffSize));
	
	struct timeval tv_timeout;
	tv_timeout.tv_sec = 0;
	tv_timeout.tv_usec = 500000;
	setsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, &tv_timeout, sizeof(tv_timeout));
    
    server.sin_family      = AF_INET;
    server.sin_port        = htons(nPort);
    server.sin_addr.s_addr = INADDR_ANY;
    
    if ( bind( ( int )m_hSocket, ( struct sockaddr* )&server, sizeof( server ) ) == -1 )
    {
        printf( "bind error\n" );
		return -1;
    }
	
	return 0;
}

int CUdpClientRecv::Uninit()
{
	int nRet = 0;
	if(m_hSocket > 0)
	{
		nRet = shutdown(m_hSocket, 2);
		m_hSocket = -1;
	}

#ifdef _WIN32
	WSACleanup();
#endif
	
	return nRet;
}


int CUdpClientRecv::Read(int nWantRead, int* outRealRead, unsigned char* outBuffer)
{
	if(m_hSocket < 0 || nWantRead > m_nMaxRecvBuffSize)
	{
		*outRealRead = 0;
		return -1;
	}
	
	int nRecv = recv(m_hSocket, (char*)outBuffer, nWantRead, 0);
	
	if(nRecv > 0)
	{
		*outRealRead = nRecv;
	}
	else
	{
		*outRealRead = 0;
		return -1;
	}
	
	return 0;
}

