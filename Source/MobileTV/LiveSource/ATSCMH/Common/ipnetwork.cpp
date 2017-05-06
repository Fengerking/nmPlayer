
#include <stdio.h>

#include "ipnetwork.h"


#if defined(LINUX)
#   include <netdb.h>
#   include <sys/ioctl.h>
#   include <errno.h>
#endif

// class CIPSocket implementation
CIPSocket::CIPSocket()
: m_socket(INVALID_SOCKET)
{
}

CIPSocket::~CIPSocket()
{
	CloseSocket();
}

bool CIPSocket::CloseSocket()
{
	if(m_socket == INVALID_SOCKET)
		return true;

	if(::closesocket(m_socket) != SOCKET_ERROR)
	{
		m_socket = INVALID_SOCKET;
		return true;
	}

	return false;
}

bool CIPSocket::GetSockOpt(int level, int optname, char * _optval, int * _optlen)
{
	if(m_socket == INVALID_SOCKET)
		return false;
#if defined(_WIN32) || defined(_WIN32_WCE)|| defined(WIN32)
	return ::getsockopt(m_socket, level, optname, _optval, _optlen) != SOCKET_ERROR;
#elif defined(LINUX)
	return ::getsockopt(m_socket, level, optname, _optval, (socklen_t*)_optlen) != SOCKET_ERROR;
#endif
}

bool CIPSocket::SetSockOpt(int level, int optname, const char * _optval, int optlen)
{
	if(m_socket == INVALID_SOCKET)
		return false;

	return ::setsockopt(m_socket, level, optname, _optval, optlen) != SOCKET_ERROR;
}

bool CIPSocket::IOCtlSocket(long cmd, unsigned long * _argp)
{
	if(m_socket == INVALID_SOCKET)
		return false;

	return ::ioctlsocket(m_socket, cmd, _argp) != SOCKET_ERROR;
}

bool CIPSocket::SetRecvBufSize(unsigned int recvBufSize)
{
	int sizeOpt = sizeof(unsigned int);
	return SetSockOpt(SOL_SOCKET, SO_RCVBUF, (char *)&recvBufSize, sizeOpt);
}

bool CIPSocket::GetRecvBufSize(unsigned int * _recvBufSize)
{	
	int sizeOpt = sizeof(unsigned int);
	return GetSockOpt(SOL_SOCKET, SO_RCVBUF, (char *)_recvBufSize, &sizeOpt);
}

bool CIPSocket::EnableBlocking(bool bBlocking)
{
	if(m_socket == INVALID_SOCKET)
		return false;

	unsigned long argp = bBlocking ? 0 : 1;
	return IOCtlSocket(FIONBIO, &argp);
}

bool CIPSocket::Connect(const char * _address, unsigned short port,struct timeval * _timeout)
{
	if(m_socket == INVALID_SOCKET)
		return false;

	struct addrinfo hints;
	memset(&hints, 0, sizeof(addrinfo));
	hints.ai_family   = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	char _port[16];
	sprintf(_port, "%hu", port);

	struct addrinfo * pAddrInfo = NULL;
	if(getaddrinfo(_address, _port, &hints, (struct addrinfo **)&pAddrInfo) != 0)
		return false;
	struct addrinfo * pAI = pAddrInfo;
	while(pAI != NULL)
	{
		if(pAI->ai_socktype == SOCK_STREAM && pAI->ai_protocol == IPPROTO_TCP)
		{
			int ret = ::connect(m_socket, pAI->ai_addr, pAI->ai_addrlen);
			if(ret != SOCKET_ERROR)
			{
				ret = Select(SOCK_WRITABLE, _timeout);
				if(ret == 1)
					break;
			}
		}

		pAI = pAI->ai_next;
	}
	freeaddrinfo(pAddrInfo);
	if(pAI == NULL)
		return false;

	return true;
}

int CIPSocket::Select(int checkAvailable, struct timeval * _timeout)
{
	fd_set sockSet;
	FD_ZERO(&sockSet);
	FD_SET(m_socket, &sockSet);
	int ret = SOCKET_ERROR;
	if(checkAvailable & SOCK_READABLE)
	{
		ret = ::select(0, &sockSet, NULL, NULL,_timeout);
	}
	else if(checkAvailable & SOCK_WRITABLE)
	{
		ret = ::select(0, NULL, &sockSet, NULL, _timeout);
	}
	return ret;
}

int CIPSocket::Receive(void * _buffer, int bufLen, struct timeval * _timeout)
{
	if(m_socket == INVALID_SOCKET)
		return SOCKET_ERROR;

	if(Select(SOCK_READABLE, _timeout) == 0)
		return SOCKET_ERROR;

	return ::recv(m_socket, (char *)_buffer, bufLen, 0);
}

int CIPSocket::ReceiveFrom(void * _buffer, int bufLen, struct sockaddr_storage * pAddrFrom)
{
	if(m_socket == INVALID_SOCKET)
		return SOCKET_ERROR;

	struct sockaddr_storage remote;
	if(pAddrFrom == NULL)
		pAddrFrom = &remote;
	int addrSize = sizeof(sockaddr_storage);
#if defined(_WIN32) || defined(_WIN32_WCE)|| defined(WIN32)
/*
	WSABUF DataBuf;
	DataBuf.buf = (char *)_buffer;
	DataBuf.len = bufLen;
	DWORD BytesRecv = 0;
	DWORD flags = 0;
	WSARecvFrom(m_socket, &DataBuf, 1, &BytesRecv, &flags, NULL, NULL, NULL, NULL);
	return BytesRecv;
*/
	return ::recvfrom(m_socket, (char *)_buffer, bufLen, 0, (struct sockaddr *)pAddrFrom, &addrSize);
#elif defined(LINUX)
	return ::recvfrom(m_socket, (char *)_buffer, bufLen, 0, (struct sockaddr *)pAddrFrom, (socklen_t*)&addrSize);
#endif
}

int CIPSocket::Send(const void * _buffer, int bufLen, struct timeval * _timeout)
{
	if(m_socket == INVALID_SOCKET)
		return SOCKET_ERROR;

	if(Select(SOCK_WRITABLE, _timeout) == 0)
		return SOCKET_ERROR;

	return ::send(m_socket, (const char *)_buffer, bufLen, 0);
}

int CIPSocket::SendTo(const void * _buffer, int bufLen, struct sockaddr_storage * pAddrTo, int sockAddrLen)
{
	if(m_socket == INVALID_SOCKET)
		return SOCKET_ERROR;

	return ::sendto(m_socket, (const char *)_buffer, bufLen, 0, (struct sockaddr *)pAddrTo, sockAddrLen);
}


// class CIPv4Socket implementation
CIPv4Socket::CIPv4Socket()
: CIPSocket()
{
}

CIPv4Socket::~CIPv4Socket()
{
}

bool CIPv4Socket::CreateDGramSocket(const char* inIPAddr, unsigned short port)
{
	CloseSocket();

	//m_socket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, 0);
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

	if(!Bind(inIPAddr, port))
		return false;

	return true;
}

bool CIPv4Socket::CreateStreamSocket(unsigned short port)
{
	CloseSocket();

	//m_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	if(!Bind(NULL, port))
		return false;

	return true;
}

bool CIPv4Socket::Bind(const char* inIPAddr, unsigned short port)
{
	if(m_socket == INVALID_SOCKET)
		return false;

	int nRet;
#if defined(_WIN32) || defined(_WIN32_WCE) || defined(WIN32)
	int reuseAddrOpt = 1;
	if(!SetSockOpt(SOL_SOCKET, SO_REUSEADDR, (const char *)&reuseAddrOpt, sizeof(reuseAddrOpt))) 
		return false;
#elif defined(LINUX)
	int on = 1;
	nRet = setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
#endif
	struct sockaddr_in local;
	memset(&local, 0, sizeof(sockaddr_in));
	local.sin_family = AF_INET;
	//local.sin_addr.s_addr = htonl(INADDR_ANY);
	if(inIPAddr)
		local.sin_addr.s_addr = inet_addr(inIPAddr);
	else
		local.sin_addr.s_addr = htonl(INADDR_ANY);

	local.sin_port = htons(port);

	nRet = bind(m_socket, (struct sockaddr *)&local, sizeof(sockaddr_in));

	return (nRet==0);
}

bool CIPv4Socket::JoinMulticastGroup(const char * _multicastAddr, const char * _interface)
{
	if(m_socket == INVALID_SOCKET)
		return false;

	struct addrinfo hints;
	memset(&hints, 0, sizeof(addrinfo));
	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	struct addrinfo * pAddrInfo = NULL;
	if(getaddrinfo(_multicastAddr, NULL, &hints, (struct addrinfo **)&pAddrInfo) != 0)
		return false;
	struct addrinfo * pAI = pAddrInfo;
	while(pAI != NULL)
	{
		if(pAI->ai_socktype == SOCK_DGRAM && pAI->ai_protocol == IPPROTO_UDP)
		{
			struct sockaddr_storage    addrStorage;
			memset(&addrStorage, 0, sizeof(sockaddr_storage));
			memcpy(&addrStorage, pAI->ai_addr, pAI->ai_addrlen);

			struct ip_mreq    mreq;
			mreq.imr_multiaddr.s_addr = ((struct sockaddr_in *)&addrStorage)->sin_addr.s_addr;
			mreq.imr_interface.s_addr = (_interface != NULL) ? inet_addr(_interface) : htonl(INADDR_ANY);
			if(SetSockOpt(IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(ip_mreq)))
				break;
		}

		pAI = pAI->ai_next;
	}
	freeaddrinfo(pAddrInfo);
	if(pAI == NULL)
		return false;

	return true;
}

bool CIPv4Socket::LeaveMulticastGroup(const char * _multicastAddr, const char * _interface)
{
	if(m_socket == INVALID_SOCKET)
		return false;

	struct addrinfo hints;
	memset(&hints, 0, sizeof(addrinfo));
	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	struct addrinfo * pAddrInfo = NULL;
	if(getaddrinfo(_multicastAddr, NULL, &hints, (struct addrinfo **)&pAddrInfo) != 0)
		return false;
	struct addrinfo * pAI = pAddrInfo;
	while(pAI != NULL)
	{
		if(pAI->ai_socktype == SOCK_DGRAM && pAI->ai_protocol == IPPROTO_UDP)
		{
			struct sockaddr_storage    addrStorage;
			memset(&addrStorage, 0, sizeof(sockaddr_storage));
			memcpy(&addrStorage, pAI->ai_addr, pAI->ai_addrlen);

			struct ip_mreq    mreq;
			mreq.imr_multiaddr.s_addr = ((struct sockaddr_in *)&addrStorage)->sin_addr.s_addr;
			mreq.imr_interface.s_addr = (_interface != NULL) ? inet_addr(_interface) : htonl(INADDR_ANY);
			if(SetSockOpt(IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&mreq, sizeof(ip_mreq)))
				break;
		}

		pAI = pAI->ai_next;
	}
	freeaddrinfo(pAddrInfo);
	if(pAI == NULL)
		return false;

	return true;
}



// class CIPv6Socket implementation
CIPv6Socket::CIPv6Socket()
: CIPSocket()
{
}

CIPv6Socket::~CIPv6Socket()
{
}

bool CIPv6Socket::CreateDGramSocket(const char* inIPAddr, unsigned short port)
{
	CloseSocket();

	//m_socket = WSASocket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, 0);
	m_socket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_IP);

	if(!Bind(inIPAddr, port))
		return false;

	return true;
}

bool CIPv6Socket::CreateStreamSocket(unsigned short port)
{
	CloseSocket();

	//m_socket = WSASocket(AF_INET6, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	m_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_IP);

	if(!Bind(NULL, port))
		return false;

	return true;
}

bool CIPv6Socket::Bind(const char* inIPAddr, unsigned short port)
{
	if(m_socket == INVALID_SOCKET)
		return false;

	int reuseAddrOpt = 1;
	if(!SetSockOpt(SOL_SOCKET, SO_REUSEADDR, (const char *)&reuseAddrOpt, sizeof(reuseAddrOpt))) 
		return false;

	struct sockaddr_in6 local;
	memset(&local, 0, sizeof(sockaddr_in6));
	local.sin6_family = PF_INET6;
	local.sin6_port = htons(port);
	//local.sin6_addr = inet_aton(inIPAddr); 
	return ::bind(m_socket, (struct sockaddr *)&local, sizeof(sockaddr_in6)) != SOCKET_ERROR;
}

bool CIPv6Socket::JoinMulticastGroup(const char * _multicastAddr, const char * _interface)
{
	if(m_socket == INVALID_SOCKET)
		return false;

	struct addrinfo hints;
	memset(&hints, 0, sizeof(addrinfo));
	hints.ai_family   = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	struct addrinfo * pAddrInfo = NULL;
	if(getaddrinfo(_multicastAddr, NULL, &hints, (struct addrinfo **)&pAddrInfo) != 0)
		return false;
	struct addrinfo * pAI = pAddrInfo;
	while(pAI != NULL)
	{
		if(pAI->ai_socktype == SOCK_DGRAM && pAI->ai_protocol == IPPROTO_UDP)
		{
			struct sockaddr_storage    addrStorage;
			memset(&addrStorage, 0, sizeof(sockaddr_storage));
			memcpy(&addrStorage, pAI->ai_addr, pAI->ai_addrlen);

			struct ipv6_mreq    mreq6;
			memset(&mreq6, 0, sizeof(ipv6_mreq));
			memcpy(&(mreq6.ipv6mr_multiaddr), &(((struct sockaddr_in6 *)&addrStorage)->sin6_addr), sizeof(in6_addr));
			mreq6.ipv6mr_interface = 0;
			if(SetSockOpt(IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, (char *)&mreq6, sizeof(ipv6_mreq)))
				break;
		}

		pAI = pAI->ai_next;
	}
	freeaddrinfo(pAddrInfo);
	if(pAI == NULL)
		return false;

	return true;
}

bool CIPv6Socket::LeaveMulticastGroup(const char * _multicastAddr, const char * _interface)
{
	if(m_socket == INVALID_SOCKET)
		return false;

	struct addrinfo hints;
	memset(&hints, 0, sizeof(addrinfo));
	hints.ai_family   = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	struct addrinfo * pAddrInfo = NULL;
	if(getaddrinfo(_multicastAddr, NULL, &hints, (struct addrinfo **)&pAddrInfo) != 0)
		return false;
	struct addrinfo * pAI = pAddrInfo;
	while(pAI != NULL)
	{
		if(pAI->ai_socktype == SOCK_DGRAM && pAI->ai_protocol == IPPROTO_UDP)
		{
			struct sockaddr_storage    addrStorage;
			memset(&addrStorage, 0, sizeof(sockaddr_storage));
			memcpy(&addrStorage, pAI->ai_addr, pAI->ai_addrlen);

			struct ipv6_mreq    mreq6;
			memset(&mreq6, 0, sizeof(ipv6_mreq));
			memcpy(&(mreq6.ipv6mr_multiaddr), &(((struct sockaddr_in6 *)&addrStorage)->sin6_addr), sizeof(in6_addr));
			mreq6.ipv6mr_interface = 0;
			if(SetSockOpt(IPPROTO_IPV6, IPV6_DROP_MEMBERSHIP, (char *)&mreq6, sizeof(ipv6_mreq)))
				break;
		}

		pAI = pAI->ai_next;
	}
	freeaddrinfo(pAddrInfo);
	if(pAI == NULL)
		return false;

	return true;
}



//
bool InitWinsock()
{
#if defined(_WIN32) || defined(_WIN32_WCE) || defined(WIN32)
	unsigned short versionRequested = 0x202;

	WSADATA wsaData;
	if(WSAStartup(versionRequested, &wsaData) != 0) 
		return false;

	if(wsaData.wVersion != 0x202)
	{
		WSACleanup();
		return false;
	}
#endif

	return true;
}

void CleanupWinsock()
{
#if defined(_WIN32) || defined(_WIN32_WCE) || defined(WIN32)
	WSACleanup();
#endif

}
