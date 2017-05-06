#include "TCPClientSocket.h"

#ifdef _WINDOWS
#include <winsock2.h>
#include <WS2tcpip.h>

#ifdef WINCE
#pragma comment(lib, "Ws2")
#else
#pragma comment(lib , "Ws2_32.lib")
#endif

#else
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>
#endif //_WINDOWS

#ifdef _IOS
	#include <unistd.h>
#endif

#include <stdio.h>

#if defined _WINDOWS || defined __VO_NDK__ || defined _IOS
#include "voLog.h"
#else  //toolchain
#include "log.h"
#endif //_WINDOWS; __VO_NDK__; _IOS


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CTCPClientSocket::CTCPClientSocket(void)
: m_pAIServ(NULL)
, m_pAICur(NULL)
, m_Socket(0)
{
#ifdef _WINDOWS
	WSADATA wsaData = {0};

	if (WSAStartup(MAKEWORD(2,2), &wsaData) != NO_ERROR)
		return;
#endif

#if defined _WINDOWS || defined __VO_NDK__ || defined _IOS
#else  //toolchain
	if(!g_ptr_logger)
		g_ptr_logger = new log_client();

	g_ptr_logger->add_ref();
#endif //_WINDOWS; __VO_NDK__; _IOS
}

CTCPClientSocket::~CTCPClientSocket(void)
{
	Close();

	if (m_Socket)
	{
#ifdef _WINDOWS
		closesocket(m_Socket);
#else
		close(m_Socket);
#endif
	}

	if (m_pAIServ)
		freeaddrinfo(m_pAIServ);

#ifdef _WINDOWS
	WSACleanup();
#endif

#if defined _WINDOWS || defined __VO_NDK__ || defined _IOS
#else  //toolchain
	if( g_ptr_logger )
		g_ptr_logger->release();
#endif //_WINDOWS; __VO_NDK__; _IOS
}

VO_S32 CTCPClientSocket::Open(const VO_CHAR *szHost, VO_S32 iPort)
{
	VO_S32 iRet = GetAddrList(szHost, iPort);
	if (iRet) {
#if defined _WINDOWS || defined __VO_NDK__ || defined _IOS
		VOLOGE("!GetAddrList %d", iRet);
#else  //toolchain
		PRINT_LOG(LOG_LEVEL_CRITICAL, "!GetAddrList %d", iRet);		
#endif //_WINDOWS; __VO_NDK__; _IOS
		
		return E_TCP_Base;//ERR_ADDRINFO
	}

	return BeginConnect();
}

VO_S32 CTCPClientSocket::Write(const VO_CHAR *fromBuf, VO_S32 iLen)
{
	while ( !IsConnected() )
	{
		if (m_Socket)
		{
			if (m_pAICur->ai_next)
			{
				m_pAICur = m_pAICur->ai_next;
			}
			else
			{

#if defined _WINDOWS || defined __VO_NDK__ || defined _IOS
				VOLOGE("!Cannot Connect!");
#else  //toolchain
				PRINT_LOG(LOG_LEVEL_CRITICAL, "!Cannot Connect!");	
#endif //_WINDOWS; __VO_NDK__; _IOS

				return E_TCP_Base;
			}
		} 
		else
		{
			m_pAICur = m_pAIServ;
		}

		Close();

		if ( BeginConnect() )
			return E_TCP_Base;
	};

	if (0 == iLen)
		iLen = strlen(fromBuf);

	VO_S32 iWritten = 0;
	do 
	{
#ifdef _WINDOWS
		VO_S32 iSent = send(m_Socket, fromBuf + iWritten, iLen - iWritten, 0);
#else
		VO_S32 iSent = write(m_Socket, fromBuf + iWritten, iLen - iWritten);
#endif

		if (iSent <= 0)
			return E_TCP_Base;

		iWritten +=  iSent;
	} while (iWritten != iLen);

	return iLen;
}

VO_S32 CTCPClientSocket::Read(VO_CHAR *toBuf, VO_S32 iMaxLen)
{
#ifdef _WINDOWS
	VO_S32 iRet = recv(m_Socket, toBuf, iMaxLen, 0);
#else
	VO_S32 iRet = read(m_Socket, toBuf, iMaxLen);
#endif

	if (iRet <= 0) {
		Close();

#ifdef _WINDOWS
		VOLOGE("!recv %d WSAGetLastError %d", iRet, WSAGetLastError());
#elif defined __VO_NDK__ || defined _IOS
		VOLOGE("!read %d errno %d", iRet, errno);
#else  //toolchain
		PRINT_LOG(LOG_LEVEL_CRITICAL, "!recv %d errno %d", iRet, errno);		
#endif //_WINDOWS
	}

	return iRet < 0 ? E_TCP_Base : iRet;
}

VO_S32 CTCPClientSocket::Close()
{
	VO_S32 iRet = 0;

	if (m_Socket)
	{
#ifdef _WINDOWS
		iRet = shutdown(m_Socket, SD_SEND);
#else
		iRet = shutdown(m_Socket, SHUT_WR);
#endif

		m_Socket = 0;
	}

	return iRet != 0 ? E_TCP_Base : iRet;
}

VO_S32 CTCPClientSocket::GetAddrList(const VO_CHAR *szHost, VO_S32 iPort)
{
	VO_CHAR szPort[16] = {0};
	addrinfo AIhints = {0};

	//AIhints.ai_flags	= AI_CANONNAME;
	AIhints.ai_family	= AF_INET;
	AIhints.ai_socktype	= SOCK_STREAM;
	AIhints.ai_protocol	= 6;

	sprintf(szPort, "%ld", iPort);

#if defined _WINDOWS || defined __VO_NDK__ || defined _IOS
	VOLOGR("szHost %s, szPort %d", szHost, iPort);
#else //toolchain
	PRINT_LOG(LOG_LEVEL_NORMAL, "szHost %s, szPort %d", szHost, iPort);	
#endif //_WINDOWS; __VO_NDK__; _IOS

	VO_S32 iRet = getaddrinfo(szHost, szPort, &AIhints, &m_pAIServ);
	if (0 != iRet)
	{
#ifdef _WINDOWS
		VOLOGE("!getaddrinfo %d", WSAGetLastError());
#elif defined __VO_NDK__ || defined _IOS
		VOLOGE("!getaddrinfo %d", errno);
#else //toolchain
		PRINT_LOG(LOG_LEVEL_CRITICAL, "!getaddrinfo %d", errno);		
#endif //_WINDOWS
		return E_TCP_Base;
	}
	
	m_pAICur = m_pAIServ;

	return iRet;
}

VO_S32 CTCPClientSocket::BeginConnect()
{
	if (!m_pAICur)
		return E_TCP_Base;

	m_Socket = socket(m_pAICur->ai_family, m_pAICur->ai_socktype, m_pAICur->ai_protocol);
	if (m_Socket < 0)
	{
#ifdef _WINDOWS
		VOLOGE("!socket %d", WSAGetLastError());
#elif defined __VO_NDK__ || defined _IOS
		VOLOGE("!socket %d", errno);
#else //toolchain
		PRINT_LOG(LOG_LEVEL_CRITICAL, "!socket %d", errno);		
#endif //_WINDOWS

		return E_TCP_Base;
	}

#ifdef _WINDOWS
	unsigned long iMode = 1;
	if ( 0 != ioctlsocket(m_Socket, FIONBIO, &iMode) ) {
		VOLOGE("!ioctlsocket %d", WSAGetLastError());
		return E_TCP_Base;
	}
#else
	VO_S32 flags = fcntl(m_Socket, F_GETFL, 0);
	if (-1 == flags) {

#if defined  __VO_NDK__ || defined _IOS
	VOLOGE("!fcntl get");
#else //toolchain
	PRINT_LOG(LOG_LEVEL_CRITICAL, "!fcntl get");		
#endif //__VO_NDK__

		return E_TCP_Base;
	}

	if (fcntl(m_Socket, F_SETFL, flags | O_NONBLOCK))
	{
#if defined  __VO_NDK__ || defined _IOS
		VOLOGE("!fcntl set NONBLOCK");
#else //toolchain
		PRINT_LOG(LOG_LEVEL_CRITICAL, "!fcntl set NONBLOCK");		
#endif //__VO_NDK__
		
		return E_TCP_Base;
	}
#endif

	if( 0 != connect(m_Socket, m_pAICur->ai_addr, m_pAICur->ai_addrlen) )
	{
#ifdef _WINDOWS
		VO_S32 iErr = WSAGetLastError();
		if (iErr != WSAEWOULDBLOCK) {
			VOLOGE("!connect %d", WSAGetLastError());
			return E_TCP_Base;
		}
#else
		if (errno != EINPROGRESS) {
#if defined  __VO_NDK__ || defined _IOS
			VOLOGE("!connect %d", errno);
#else //toolchain
			PRINT_LOG(LOG_LEVEL_CRITICAL, "!connect %d", errno);		
#endif //__VO_NDK__

			return E_TCP_Base;
		}
#endif
	}

	return 0;
}

VO_BOOL CTCPClientSocket::IsConnected() const
{
	if (!m_Socket)
		return VO_FALSE;

	fd_set r;
	timeval tv = {0};

	FD_ZERO(&r);
	FD_SET(m_Socket, &r);
	tv.tv_sec	= 6;
	tv.tv_usec	= 0;

	VO_S32 iRet = select(m_Socket + 1, 0, &r, 0, &tv);
	if (iRet <= 0)
	{
#ifdef _WINDOWS
		VOLOGE("!select %d", WSAGetLastError());
#elif defined __VO_NDK__ || defined _IOS
		VOLOGE("!select %d", errno);
#else //toolchain
		PRINT_LOG(LOG_LEVEL_CRITICAL, "!select %d", errno);		
#endif //_WINDOWS

		return VO_FALSE;
	}


#ifdef _WINDOWS
	unsigned long iMode = 0;
	if ( 0 != ioctlsocket(m_Socket, FIONBIO, &iMode) ) {
		VOLOGE("!ioctlsocket %d", WSAGetLastError());
		return VO_FALSE;
	}
#else
	VO_S32 flags = fcntl(m_Socket, F_GETFL, 0);
	if (-1 == flags) {
#if defined  __VO_NDK__ || defined _IOS
		VOLOGE("!fcntl get");
#else //toolchain
		PRINT_LOG(LOG_LEVEL_CRITICAL, "!fcntl get");		
#endif //__VO_NDK__ || _IOS

		return VO_FALSE;
	}

	flags &= ~O_NONBLOCK;
	if ( fcntl(m_Socket, F_SETFL, flags) )
	{
#if defined  __VO_NDK__ || defined _IOS
		VOLOGE("!fcntl set BLOCK");
#else //toolchain
		PRINT_LOG(LOG_LEVEL_CRITICAL, "!fcntl set BLOCK");		
#endif //__VO_NDK__ || _IOS

		return VO_FALSE;
	}
#endif


#ifdef _WINDOWS
	DWORD dwTimeOut = 0;
	int iOptLen = sizeof(DWORD);

	if ( 0 != getsockopt(m_Socket, SOL_SOCKET, SO_RCVTIMEO, (VO_CHAR*)&dwTimeOut, &iOptLen) ) {
		VOLOGE("!getsockopt");
		return VO_FALSE;
	}

	if (0 == dwTimeOut)
	{
		dwTimeOut = 24000;
		if ( 0 != setsockopt(m_Socket, SOL_SOCKET, SO_RCVTIMEO, (VO_CHAR*)&dwTimeOut, iOptLen) ) {
			VOLOGE("!getsockopt");
			return VO_FALSE;
		}
	}
#else
#if _IOS
	socklen_t iOptLen = sizeof(timeval);
#else
	int iOptLen = sizeof(timeval);
#endif

	if ( 0 != getsockopt(m_Socket, SOL_SOCKET, SO_RCVTIMEO, (VO_CHAR*)&tv, &iOptLen) ) {
#if defined  __VO_NDK__ || defined _IOS
		VOLOGE("!getsockopt");
#else //toolchain
		PRINT_LOG(LOG_LEVEL_CRITICAL, "!getsockopt");		
#endif //__VO_NDK__ || _IOS
		return VO_FALSE;
	}

	if (0 == tv.tv_sec)
	{
		tv.tv_sec = 24;
		tv.tv_usec = 0;
		if ( 0 != setsockopt(m_Socket, SOL_SOCKET, SO_RCVTIMEO, (VO_CHAR*)&tv, iOptLen) ) {
#if defined  __VO_NDK__ || defined _IOS
			VOLOGE("!setsockopt");
#else //toolchain
			PRINT_LOG(LOG_LEVEL_CRITICAL, "!setsockopt");		
#endif //__VO_NDK__ || _IOS
			return VO_FALSE;
		}
	}
#endif

	return VO_TRUE;
}
