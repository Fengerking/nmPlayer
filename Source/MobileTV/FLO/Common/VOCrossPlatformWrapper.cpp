#include <winsock2.h>
#include <ws2tcpip.h>
#include <mmsystem.h>
#include <tchar.h>

#include "utility.h"

#include "voCrossPlatformWrapper.h"

#include "voOSFunc.h"

#ifdef _WIN32_WCE
#include <time.h>
#else
#include <sys/timeb.h>
#endif

CVOSocketWindows::CVOSocketWindows(VOSockType sockType,VOProtocolType proType,VOAddressFamily af)
:IVOSocket(sockType,proType,af)
{
	m_sockAddress.ipAddress	=	m_sockAddress.port = 0;
	m_sock=VEC_INVALID_SOCKET;
	SOCKET streamSock = VEC_INVALID_SOCKET;
	int ret=0;

	do
	{
		streamSock = socket(af, sockType, proType); // protocol

		if(streamSock!=VEC_INVALID_SOCKET)
		{
			sprintf(CLog::formatString,"create sock=%d\n",streamSock);
			CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt",CLog::formatString);
		}
		
		m_sockAddress.family	= af;
		m_sockAddress.protocol	= proType;
		m_sockAddress.sockType	= sockType;
		m_sock					= streamSock;
		int reuseAddrOpt = 1;
		bool bDontLinger = false;
		if (setsockopt(m_sock,SOL_SOCKET,SO_DONTLINGER,(const char*)&bDontLinger,sizeof(bool))!= VEC_SOCKET_ERROR) 
		{
			CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt","setsockopt SO_DONTLINGER\n");
		} 
		if(setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuseAddrOpt, sizeof(reuseAddrOpt)) != VEC_SOCKET_ERROR) 
		{
			CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt","setsockopt SO_REUSEADDR\n");
		}
	}
	while(0);
}

CVOSocketWindows::~CVOSocketWindows()
{
	
	Close();
}

/** 
*Create one socket

int CVOSocketWindows::Create(VOSockType sockType,VOProtocolType proType,VOAddressFamily af)
{
	SOCKET streamSock = VEC_INVALID_SOCKET;
	int ret=0;

	do
	{
		streamSock = socket(af, sockType, proType); // protocol
		if(streamSock==VEC_INVALID_SOCKET)
		{
			ret = WSAGetLastError();
			break;
		}
		
		m_sockAddress.family	= af;
		m_sockAddress.protocol	= proType;
		m_sockAddress.sockType	= sockType;
		m_sock					= streamSock;
		return 0;
	}
	while(0);
	
	closesocket(streamSock);
	return ret;

}*/

/** 
*Initializes the socket, sets the socket timeout and 
*establishes a connection to the TCP server at the given address.
*If the timeout=0,the connect mode is blocking,ohterwise non-blocking
*/
int CVOSocketWindows::Connect(const VOSocketAddress& address, unsigned long timeoutBySec)
{
	
	if(m_sock==VEC_INVALID_SOCKET)
		return -1;

	struct addrinfo peer={0};
	//peer.ai_family		= address.family;
	//peer.ai_protocol	= address.protocol;
	//peer.ai_socktype	= address.sockType;
	struct sockaddr_in  peerAddr={0};
	peerAddr.sin_addr.s_addr	= address.ipAddress;
	peerAddr.sin_port	= address.port;
	peerAddr.sin_family	= address.family;
	peer.ai_addr		=(struct sockaddr*)&peerAddr;
	peer.ai_addrlen		=sizeof(struct sockaddr);
	peer.ai_family		= address.family;
	peer.ai_socktype	= address.sockType;
	peer.ai_protocol	= address.protocol;
	peer.ai_flags		= address.userData1;
	

	int ret=0;
	struct   timeval   timeout   ;   
	fd_set   r;   	
	
	if(timeoutBySec)//todo,
	{
		//set the connect as non-blocking
		unsigned long ul = 1;
		ioctlsocket(m_sock, FIONBIO, (unsigned long*)&ul);
		FD_ZERO(&r);   
		FD_SET(m_sock,   &r);   
		timeout.tv_sec   =   1;    
		timeout.tv_usec   =0; 
		connect(m_sock, peer.ai_addr, peer.ai_addrlen);
		

		ret   =   select(0,   0,   &r,   0,   &timeout);   
		//resume to block
		ul = 0;
		ioctlsocket(m_sock, FIONBIO, (unsigned long*)&ul);

		if(ret<=0)
			return 1;//timeout or error
		else
			return 0;//ok
	}
	else
	{
		if(connect(m_sock, peer.ai_addr, peer.ai_addrlen) == VEC_SOCKET_ERROR) 
		{
#if 1
			printf("sock=%d,add=%X,port=%d,family=%d,type=%d,pro=%d\n",m_sock,address.ipAddress,address.port,address.family,address.sockType,address.protocol);
#endif
			ret = VEC_SOCKET_ERROR;
			closesocket(m_sock);
			m_sock = VEC_INVALID_SOCKET;
		}
	} 
	
	return ret;
}
/** 
*Bind a local address to the socket.
*If reuseAddress is true, sets the SO_REUSEADDR
*socket option.
*/
int CVOSocketWindows::Bind(const VOSocketAddress& address2, bool reuseAddress)
{
	int ret=0;
	if(m_sock==VEC_INVALID_SOCKET)
		return -1;
	if(reuseAddress)
	{
		int reuseAddrOpt = 1;
		if(setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuseAddrOpt, sizeof(reuseAddrOpt)) == VEC_SOCKET_ERROR) 
		{
			return ret = VEC_SOCKET_ERROR;
		}
	}
	
	struct sockaddr_in local;
	memset(&local, 0, sizeof(sockaddr_in));
	local.sin_family = PF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	/*
	local.sin_family = address2.family;
	if(address2.family==VAF_INET4)
	{
		//if(m_sockAddress.ipAddress==0)
		//	local.sin_addr.s_addr = htonl(INADDR_ANY);
		//else
			local.sin_addr.s_addr = address2.ipAddress;
	}
	*/
	local.sin_port = address2.port;
	if(bind(m_sock, (struct sockaddr *)&local, sizeof(sockaddr_in)) == VEC_SOCKET_ERROR) 
	{
		return ret = VEC_SOCKET_ERROR;
	}
	memcpy(&m_sockAddress,&address2,sizeof(VOSocketAddress));
	return ret;
}
/** 
*Puts the socket into listening state.
*The socket becomes a passive socket that
*can accept incoming connection requests.
*The backlog argument specifies the maximum
*number of connections that can be queued
*for this socket.
*/
int CVOSocketWindows::Listen(int backlog )
{
	if(m_sock==VEC_INVALID_SOCKET)
		return -1;
	int ret=0;
	if(listen(m_sock, backlog) == VEC_SOCKET_ERROR) 
	{
		ret = WSAGetLastError();
		closesocket(m_sock);
		m_sock = VEC_INVALID_SOCKET;
	}
	return ret;
}
bool CVOSocketWindows::IsValidSocket()
{
	return m_sock!=VEC_INVALID_SOCKET;
}
/** 
*Get the next completed connection from the socket's completed connection queue.
*/
IVOSocket* CVOSocketWindows::Accept()
{
	
	return 0;
}
/** 
*Close the socket.
*/
int CVOSocketWindows::Close()
{
	if(m_sock==VEC_INVALID_SOCKET)
		return -1;
	sprintf(CLog::formatString,"close socket %d----",m_sock);
	CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt",CLog::formatString);
	closesocket(m_sock);
	
	m_sock=VEC_INVALID_SOCKET;
	return 0;
}
/** 
* disables sends or receives or both on a socket
*/
int CVOSocketWindows::Shutdown(VOSockShutOpt opt)
{
	if(m_sock==VEC_INVALID_SOCKET)
		return -1;
	shutdown(m_sock,opt);
	return 0;
}
int IVOSocket::SelectSockGroup(VOSockEvent soEvent,VOSocketGroup* group,long timeout2)
{
	fd_set set;
	FD_ZERO(&set);
	for(int i=0;i<group->fd_count;i++)
	{
		IVOSocket* sock= group->fd_array[i];
		FD_SET(sock->GetSockID(),&set);
	}
	struct timeval timeout;
	if(timeout2>500)
	{
		timeout.tv_sec = (timeout2+500)/1000;
		timeout.tv_usec = 0;
	}
	else
	{
		timeout.tv_sec  = 0;//(timeout2+500)/1000;
		timeout.tv_usec = timeout2*1000;
	}
	int ret=0;
	switch(soEvent)
	{
	case VSE_FD_READ:
		ret = select(0,&set,NULL,NULL,&timeout);
		break;
	case VSE_FD_WRITE:
		ret = select(0,NULL,&set,NULL,&timeout);
		break;
	case VSE_FD_ERROR:
		ret = select(0,NULL,NULL,&set,&timeout);
		break;
	}
	if(ret>0)
	{
		for(int i=0;i<set.fd_count;i++)
		{
			int sockID = set.fd_array[i];
			for(int j=0;j<group->fd_count;j++)
			{
				IVOSocket* sock= group->fd_array[j];
				if(sock->GetSockID()==sockID)
				{
					set.fd_array[i] = (int)sock;
					break;
				}
			}
		}
		memcpy(group,&set,sizeof(VOSocketGroup));
	}
	return ret;

}
/** 
*determines the status of one or more sockets, waiting if necessary, to perform synchronous I/O.
*\param soEvent 
*\param timeout, if timeout=0,blocking the operation. 
*/
int CVOSocketWindows::Select(VOSockEvent soEvent, long timeout2)
{
	if(m_sock==VEC_INVALID_SOCKET)
		return -1;
	int ret=0;
	fd_set set;
	FD_ZERO(&set);
	FD_SET(m_sock,&set);
	struct timeval timeout;
	if(timeout2>500)
	{
		timeout.tv_sec = (timeout2+500)/1000;
		timeout.tv_usec = 0;
	}
	else
	{
		timeout.tv_sec  = 0;//(timeout2+500)/1000;
		timeout.tv_usec = timeout2*1000;
	}
	
	
	switch(soEvent)
	{
	case VSE_FD_READ:
		ret = select(0,&set,NULL,NULL,&timeout);
		break;
	case VSE_FD_WRITE:
		ret = select(0,NULL,&set,NULL,&timeout);
	    break;
	case VSE_FD_ERROR:
		ret = select(0,NULL,NULL,&set,&timeout);
	    break;
	}
	return ret;
}
/** 
*Sends the contents of the given buffer through the socket.Returns the number of bytes sent, which may be
*less than the number of bytes specified.
*/
int CVOSocketWindows::Send(const char* buffer, int length, VOSockMSGType flags)
{
	if(m_sock==VEC_INVALID_SOCKET)
		return -1;
	int ret=0;
	if(send(m_sock,buffer,length,flags)==VEC_SOCKET_ERROR)
	{
		ret = VEC_SOCKET_ERROR;
	}
	return ret;
}
/** 
*The recv function receives data from a connected or bound socket
*returns the number of bytes received.
*/
int CVOSocketWindows::Recv(char* buffer, int length, VOSockMSGType flags)
{
	if(m_sock==VEC_INVALID_SOCKET)
		return -1;
	int ret=0;
	if(recv(m_sock,buffer,length,flags)==VEC_SOCKET_ERROR)
	{
		ret = WSAGetLastError();
	}
	return ret;
}
/** 
*Sends the contents of the given buffer through the socket to the given address.
*Returns the number of bytes sent
*/
int CVOSocketWindows::SendTo(const void* buffer, int length, const VOSocketAddress& address, VOSockMSGType flags)
{
	if(m_sock==VEC_INVALID_SOCKET)
		return VEC_SOCKET_ERROR;
	int ret=0;
	struct sockaddr_in  toaddr={0};
	toaddr.sin_addr.s_addr	= address.ipAddress;
	toaddr.sin_family  = address.family;
	toaddr.sin_port	= address.port;
	int bytesSent = sendto(m_sock, (char *)buffer, length, 0, (struct sockaddr *)&toaddr, sizeof(sockaddr));
	
	if(bytesSent == VEC_SOCKET_ERROR) 
	{
		bytesSent = VEC_SOCKET_ERROR;//WSAGetLastError();
	}
	return bytesSent;
}
/** 
*Receives data from the socket and stores it in buffer
*Returns the number of bytes received.
*/
int CVOSocketWindows::ReceiveFrom(void* buffer, int length, VOSocketAddress& address, VOSockMSGType flags)
{
	if(m_sock==VEC_INVALID_SOCKET)
		return VEC_SOCKET_ERROR;
	//struct sockaddr_storage remote={0};
	sockaddr_in remote={0};
	int addrSize = sizeof(remote);
	int recvBytes = recvfrom(m_sock, (char*)buffer, length, 0, (SOCKADDR *)&remote, &addrSize);
	return recvBytes;
}
/** 
*SetSendBufferSize
*/
int CVOSocketWindows::SetSendBufferSize(int size)
{
	if(m_sock==VEC_INVALID_SOCKET)
		return VEC_SOCKET_ERROR;
	int sizeOpt = sizeof(size);
	int ret = 0;
	if(setsockopt(m_sock, SOL_SOCKET, SO_SNDBUF, (char *)&size, sizeOpt) == VEC_SOCKET_ERROR) 
	{
		ret = VEC_SOCKET_ERROR;
	}
	return ret;
}
/** 
*GetSendBufferSize
*/
int CVOSocketWindows::GetSendBufferSize()
{
	if(m_sock==VEC_INVALID_SOCKET)
		return VEC_SOCKET_ERROR;
	unsigned int sendBufOpt;
	int sizeOpt = sizeof(sendBufOpt);
	if(getsockopt(m_sock, SOL_SOCKET, SO_SNDBUF, (char *)&sendBufOpt, &sizeOpt) == VEC_SOCKET_ERROR) 
	{
		sendBufOpt = VEC_SOCKET_ERROR;
	}

	return sendBufOpt;
}
/** 
*SetRecvBufferSize
*/
int CVOSocketWindows::SetRecvBufferSize(int size)
{
	if(m_sock==VEC_INVALID_SOCKET)
		return VEC_SOCKET_ERROR;
	int sizeOpt = sizeof(size);
	int ret = 0;
	if(setsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, (char *)&size, sizeOpt) == VEC_SOCKET_ERROR) 
	{
		ret = VEC_SOCKET_ERROR;
	}
	return ret;
}
/** 
*GetRecvBufferSize
*/
int CVOSocketWindows::GetRecvBufferSize()
{
	if(m_sock==VEC_INVALID_SOCKET)
		return VEC_SOCKET_ERROR;
	unsigned int recvBufOpt;
	int sizeOpt = sizeof(recvBufOpt);
	if(getsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, (char *)&recvBufOpt, &sizeOpt) == VEC_SOCKET_ERROR) 
	{
		recvBufOpt = VEC_SOCKET_ERROR;
	}

	return recvBufOpt;
}
/** 
*retrieves the current value for a socket option 
*associated with a socket of any type, in any state, and stores the result in optval
*/
int CVOSocketWindows::SetSockOpt(int level,int optname,const char* optVal,int optLen)
{
	if(m_sock==VEC_INVALID_SOCKET)
		return VEC_SOCKET_ERROR;
	int ret=0;
	if(setsockopt(m_sock, level, optname, optVal, optLen) == VEC_SOCKET_ERROR) 
	{
		ret = VEC_SOCKET_ERROR;
	}
	return ret;
}
/** 
*retrieves the current value for a socket option 
*associated with a socket of any type, in any state, and stores the result in optval
*/
int CVOSocketWindows::GetSockOpt(int level,int optname,char* optVal,int* optLen)
{
	if(m_sock==VEC_INVALID_SOCKET)
		return VEC_SOCKET_ERROR;
	int ret = 0;
	if(getsockopt(m_sock, level, optname, optVal, optLen) == VEC_SOCKET_ERROR) 
	{
		ret = VEC_SOCKET_ERROR;
	}
	return ret;

}
int CVOSocketWindows::SetAddress(VOSocketAddress* adderss)
{
	if(adderss)
		memcpy(&m_sockAddress,adderss,sizeof(VOSocketAddress));
	return 0;
}
/** 
*Returns the IP address and port number of the socket.
*/
int CVOSocketWindows::GetAddress(VOSocketAddress* adderss)
{
	if(m_sock==VEC_INVALID_SOCKET)
		return -1;
	memcpy(adderss,&m_sockAddress,sizeof(VOSocketAddress));
	return 0;
}
int CVOSocketWindows::SetPeerAddress(VOSocketAddress* adderss)
{
	if(adderss)
		memcpy(&m_peerSockAddress,adderss,sizeof(VOSocketAddress));
	return 0;
}
/** 
*Returns the IP address and port number of the socket.
*/
int CVOSocketWindows::GetPeerAddress(VOSocketAddress* adderss)
{
	if(m_sock==VEC_INVALID_SOCKET)
		return -1;
	memcpy(adderss,&m_peerSockAddress,sizeof(VOSocketAddress));
	return 0;
}
/** 
*Returns the IP address and port number of the socket.

int CVOSocketWindows::GetAddressByURL(char* hostName,int hostNamelen,int port,VOSocketAddress* adderss)
{
	if(m_sock==0)
		return -1;
	memcpy(adderss,&m_peerSockAddress,sizeof(VOSocketAddress));
	return 0;
}
*/
/** 
*Returns the IP address and port number of the socket.
*/
int  IVOSocket::GetPeerAddressByURL(char* hostName,int hostNamelen,VOSocketAddress* address)
{
	struct addrinfo *m_serverAddrInfo=NULL;
	struct addrinfo *m_serverAddrInfoList=NULL;
	struct addrinfo hints={0};
	int port = ntohs(address->port);//the address->port is big endian
	int size1=sizeof(hints);
	//memset(&hints, 0,size1);
	hints.ai_family   = PF_UNSPEC;
	hints.ai_protocol = address->protocol;
	hints.ai_socktype = address->sockType;
	m_serverAddrInfoList  = NULL;
	int ret=VEC_SOCKET_ERROR;
	char ServerPort[16] = {0};
	sprintf(ServerPort, "%u", port);
	sprintf(CLog::formatString,"url=%s,port=%s,size=%d\n",hostName,ServerPort,size1);
	CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt",CLog::formatString);

	if(getaddrinfo(hostName, ServerPort, &hints, (struct addrinfo **)&m_serverAddrInfoList) != 0)
	{
		return ret;
	}
	struct addrinfo * AI = m_serverAddrInfoList;
	while(AI != NULL)
	{
		if(AI->ai_protocol == address->protocol && AI->ai_socktype == address->sockType)
		{
			/*
			m_peerSockAddress.family		= (VOAddressFamily)AI->ai_family;
			struct sockaddr_in* in_addr		=(struct sockaddr_in*)(AI->ai_addr->sa_data);
			m_peerSockAddress.ipAddress		= in_addr->sin_addr.s_addr;
			m_peerSockAddress.sockType		= (VOSockType)AI->ai_socktype;
			m_peerSockAddress.protocol		= (VOProtocolType)AI->ai_protocol;
			m_peerSockAddress.port			= htons(port);
			m_serverAddrInfo = AI;
			memcpy(adderss,&m_peerSockAddress,sizeof(VOSocketAddress));*/
			address->family		= (VOAddressFamily)AI->ai_family;
			struct sockaddr_in* in_addr		=(struct sockaddr_in*)(AI->ai_addr);
			address->ipAddress  = in_addr->sin_addr.s_addr;
			address->sockType	= (VOSockType)AI->ai_socktype;
			address->protocol	= (VOProtocolType)AI->ai_protocol;
			address->userData1	= AI->ai_flags;
			ret = 0;
			break;
		}
		AI = AI->ai_next;
	}
	if(m_serverAddrInfoList)
		freeaddrinfo(m_serverAddrInfoList);
	if(AI == NULL)
	{
		ret = VEC_SOCKET_ERROR;
	}

	return ret;
}
/** 
*Set the blocking mode
*/
int CVOSocketWindows::SetBlocking(bool isBlocking)
{
	return 0;
}
/** 
*Get the blocking mode
*/
bool CVOSocketWindows::GetBlocking()
{
	return 0;
}
/** 
*Get the blocking mode
*/
int IVOSocket::GetSockLastError()
{
	return ::WSAGetLastError();
}
/** 
*Init the socket library on the platform
*\param versionNum,the number of the library
*\param userData
*/
int IVOSocket::Init(long versionNum,void* userData)
{
	unsigned short versionRequested = 0x202;

	WSADATA wsaData;
	if(WSAStartup(versionRequested, &wsaData) != 0) 
		return -1;

	if(wsaData.wVersion != 0x202)
	{
		WSACleanup();
		return -1;
	}
	return 0;
}
/** 
*UnInit the socket library on the platform
*/
int IVOSocket::UnInit(void* userData)
{
	WSACleanup();
	return 0;
}
unsigned short IVOSocket::HostToNetworkShort(unsigned short hostData)
{
	return htons(hostData);
}
/** 
*convert the Host byte order to network byte order
*/
unsigned long IVOSocket::HostToNetworkLong(unsigned long hostData)
{
	return htonl(hostData);
}
/** 
*convert the network byte  order to Host byte order
*/
unsigned short IVOSocket::NetworkToHostShort(unsigned short netData)
{
	return ntohs(netData);
}
/** 
*convert the network byte  order to Host byte order
*/
unsigned long IVOSocket::NetworkToHostLong(unsigned long netData)
{
	return ntohl(netData);
}

int IVOSocket::GetHostName(char* name,int namelen)
{
	return gethostname(name,namelen);
}

void IVOSocket::GetTimeOfDay(timeval * time)
{
	time->tv_sec = time->tv_usec = 0;

#ifdef _WIN32_WCE

#if 0
	static unsigned __int64 epoch = 116444736000000000L;

	FILETIME       file_time;
	SYSTEMTIME     system_time;
	ULARGE_INTEGER ularge;
	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	ularge.LowPart = file_time.dwLowDateTime;
	ularge.HighPart = file_time.dwHighDateTime;

	time->tv_sec  = (long)((ularge.QuadPart - epoch) / 10000000L);
	time->tv_usec = (long)(system_time.wMilliseconds * 1000);
#else
	long time2=voOS_GetSysTime();
	time->tv_sec   = (long)time2/1000;
	time->tv_usec =(time2-time->tv_sec*1000)*1000;
#endif

#else

	struct _timeb tb;
	_ftime(&tb);
	time->tv_sec  = tb.time;
	time->tv_usec = tb.millitm * 1000;

#endif
}

int	 IVOSocket::AddOneSockToGroup(IVOSocket* sock,VOSocketGroup* group)
{
	//SOCKET sockWin=(SOCKET)sock;
	//VO_FD_SET(sockWin,group);
	return 0;
}
int	 IVOSocket::RemoveOneSockFromGroup(IVOSocket* sock,VOSocketGroup* group)
{
	//SOCKET sockWin=(SOCKET)sock;
	//FD_CLR(sockWin,group);
	return 0;
}
int	 IVOSocket::CleanSockGroup(VOSocketGroup* group)
{
	memset(group,0,sizeof(VOSocketGroup));
	return 0;
}
int	 IVOSocket::IsSockInGroup(IVOSocket* sock,VOSocketGroup* group)
{
	SOCKET sockWin=(SOCKET)sock;
	if(FD_ISSET(sockWin,group))
		return 1;
	else
		return 0;
}



IVOSocket* VOCPFactory::CreateOneSocket(VOSockType sockType,VOProtocolType proType,VOAddressFamily af)
{
	return new CVOSocketWindows(sockType,proType,af);
}

void  VOCPFactory::DeleteOneSocket(IVOSocket* sock)
{
	delete sock;
}