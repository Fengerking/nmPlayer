
#include "vo_socket.h"
#include "stdlib.h"

#if defined(_IOS) || defined (_MAC_OS) || defined(_LINUX_ANDROID)
#include "string.h"
#include "voOSFunc.h"
#include <errno.h>
#endif //_IOS

#if defined(_IOS) || defined (_MAC_OS) || defined(_LINUX_ANDROID)
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#include "Ws2tcpip.h"
#ifdef ENABLE_ASYNCDNS
#include "ws2ipdef.h"
#endif //ENABLE_ASYNCDNS
#ifdef WINCE
#pragma comment( lib , "Ws2" )
#else
#pragma comment( lib , "Ws2_32.lib" )
#endif //WINCE
#endif //_WIN32

#include "voLog.h"
#ifdef ENABLE_ASYNCDNS
#include "ares.h"
#endif

#define IOC_IN 0x80000000 
#define IOCPARM_MASK 0x7f 
#define _IOW(x,y,t) (IOC_IN|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))
#define FIONBIO     _IOW('f', 126, u_long) /* set/clear non-blocking i/o */
#define VODNSRESOLVETIMEOUTDEFAULT 30*1000000

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

VO_S32 vo_socket_send( VO_S32 s , VO_PBYTE buffer , VO_S32 size )
{
#ifdef _WIN32
	return send( s , (char*)buffer , size , 0 );
#else
	int iRet = write( s , buffer , size );
#if defined (_IOS) || defined (_MAC_OS) || defined (_LINUX_ANDROID)
    if (errno == EPIPE) {
        iRet = -1;
    }
#endif
    return iRet;
#endif
}

VO_S32 vo_socket_recv( VO_S32 s , VO_PBYTE buffer , VO_S32 size )
{
#ifdef _WIN32
	return recv( s , (char*)buffer , size , 0 );
#else
	return recv( s , (char*)buffer , size , 0 );
#endif
}

VO_S32 vo_socket_send_safe( VO_S32 s , VO_PBYTE buffer , VO_S32 size , VO_BOOL * bexit )
{
	VO_S32 total_send = 0;

	VO_S32 ret = 0;

	do 
	{
		if( bexit && *bexit )
		{
			VOLOGI( "require exit when socket send data" );
			return -1;
		}

		ret = vo_socket_send( s , buffer + total_send , size - total_send );

		if( ret == -1 )
			return -1;

		total_send = total_send + ret;

	} while ( total_send < size );

	return total_send;
}

VO_S32 vo_socket_close( VO_S32 s )
{
#ifdef _WIN32
	return closesocket( s );
#else
	return close( s );
#endif
}

VO_BOOL vo_socket_init()
{
#ifdef _WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD( 2, 2 );
	err = WSAStartup( wVersionRequested, &wsaData );

	if( err != 0 )
		return VO_FALSE;
#elif defined (_IOS) || defined (_MAC_OS) || defined (_LINUX_ANDROID)
	signal(SIGPIPE, SIG_IGN);
#endif
	return VO_TRUE;
}


VO_VOID vo_socket_uninit()
{
#ifdef _WIN32
	WSACleanup();
#endif
}

VO_BOOL vo_socket_connect_ansyc( VO_S32 s , addrinfo* ptr_addr )
{
	//check validity of parameter 
	if( s < 0 || !ptr_addr )
	{
		return VO_FALSE;
	}

	unsigned long ul = 1;
#ifdef _WIN32
	ioctlsocket(s, FIONBIO, (unsigned long*)&ul);
#else
	int oldflag = fcntl(s, F_GETFL, 0);
	fcntl(s, F_SETFL, oldflag | O_NONBLOCK );
#endif
	VO_S32 ret;
	timeval timeout; 
	fd_set   fdw;
	fd_set   fde;
	FD_ZERO(&fdw);   
	FD_SET(s,   &fdw);   
	FD_ZERO(&fde);   
	FD_SET(s,   &fde); 
	timeout.tv_sec = 2;    
	timeout.tv_usec =0; 
	connect(s, ptr_addr->ai_addr, ptr_addr->ai_addrlen );

	VOLOGI( "connect +++++" );
	ret = select( s + 1 , 0 , &fdw , &fde , &timeout );   
	VOLOGI( "connect -----" );

	if(FD_ISSET(s , &fde))
	{
		VOLOGE( "socket exception happen when connect" );
		ret = -1;
	}
	ul = 0;
#ifdef _WIN32
	ioctlsocket(s, FIONBIO, (unsigned long*)&ul);
#else
	fcntl(s, F_SETFL, oldflag);
#endif

	if(ret<=0)
	{
		return VO_FALSE;
	}
	else
	{
		return VO_TRUE;
	}
}

VO_BOOL vo_socket_connect( VO_S32 * ptr_socket , VO_CHAR * str_host , VO_CHAR * str_port , VO_BOOL * bexit )
{
	VO_S32 ret = 0;
	//int getaddrinfo(const char *node, const char *service,const struct addrinfo *hints,struct addrinfo **res);
	//If AI_NUMERICSERV is specified in hints.ai_flags and service is not NULL, then service must point to a string containing 
	//a numeric port number. This flag is used to inhibit the invocation of a name resolution service in cases where it is known 
	//not to be required. see http://linux.die.net/man/3/getaddrinfo   for detail.
	addrinfo info;
	memset(&info, 0x00, sizeof(info));
	info.ai_family = AF_UNSPEC;
	info.ai_socktype = SOCK_STREAM;
	info.ai_protocol = IPPROTO_TCP;

	VO_BOOL bsuppresses = VO_TRUE;
#ifdef _WIN32
    #include <Windows.h>
	OSVERSIONINFO osvi;
    int bIsWindowsVistaorLater;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx(&osvi);
	bIsWindowsVistaorLater = osvi.dwMajorVersion >= 6;
	if( !bIsWindowsVistaorLater )
	{
		bsuppresses = VO_FALSE;
	}
#endif

	// inet_pton does't support in XP. Disable it.
/*
	if( bsuppresses )
	{
#ifndef WINCE
		info.ai_flags = AI_NUMERICSERV;

		//Check if we were provided the address of the server, like :"ddd.ddd.ddd.ddd"     
		//inet_pton convert the text form of the address to binary form;
		//If it is numeric then we want to prevent getaddrinfo()     
		//from doing any name resolution.                                  
		struct in6_addr svraddr; 
		memset(&svraddr, 0x00, sizeof(svraddr));
		ret = inet_pton(AF_INET, str_host, &svraddr);
		if ( ret == 1 )    
		{
			VOLOGI( "it is ipv4, and numberic address is provied" );
			info.ai_family = AF_INET;
			// The AI_NUMERICHOST flag suppresses any potentially lengthy network host address lookups.
			info.ai_flags |= AI_NUMERICHOST;
		}
		else
		{
			ret = inet_pton(AF_INET6, str_host, &svraddr);
			if (ret == 1) 
			{
				VOLOGI( "it is ipv6, and numberic address is provied" );
				info.ai_family = AF_INET6;
				// The AI_NUMERICHOST flag suppresses any potentially lengthy network host address lookups.
				info.ai_flags |= AI_NUMERICHOST;
			}
		}
#endif
	}
*/
	if( bexit && *bexit )
	{
		VOLOGI( "require exit before DNS Lookup" );
		return VO_FALSE;
	}

	addrinfo * ptr_ret = 0;
	VOLOGI( "DNS Lookup ++++");
	ret = getaddrinfo( str_host , str_port , &info , &ptr_ret );
	VOLOGI( "DNS Lookup ----");
	if( ret )
	{
		//Memory allocated by a successful call to getaddrinfo must be released with a subsequent call to freeaddrinfo.
		//BUT not include failed case, so disable it, else it may cause problem.
	//	freeaddrinfo( ptr_ret );
		VOLOGI( "DNS Look up failed , ret : %d" , ret );
		return VO_FALSE;
	}

	addrinfo * ptr_entry = ptr_ret;
	*ptr_socket = -1;
	while( ptr_entry )
	{
		if( bexit && *bexit )
		{
			VOLOGI( "require exit when try the address list to connect" );
			break;
		}

		*ptr_socket = socket( ptr_entry->ai_family, ptr_entry->ai_socktype, ptr_entry->ai_protocol );

		if( *ptr_socket == -1 )
		{
			ptr_entry = ptr_entry->ai_next;
			continue;
		}

		if( vo_socket_connect_ansyc( *ptr_socket , ptr_entry ) )
		{
			break;
		}

		vo_socket_close( *ptr_socket );

		*ptr_socket = -1;

		ptr_entry = ptr_entry->ai_next;
	}
	
	freeaddrinfo( ptr_ret );

	if( *ptr_socket == -1 )
	{
		VOLOGI( "failed to connect to server" );
		return VO_FALSE;
	}

 	linger lg;
 	lg.l_linger = 0;
 	lg.l_onoff = 1;
 	setsockopt( *ptr_socket ,SOL_SOCKET ,SO_LINGER,(const char*)&lg,sizeof(linger));

#ifndef _WIN32
#ifndef _IOS
#ifndef _MAC_OS
 	VO_S32 enable = 1;
 	if( 0 == setsockopt( *ptr_socket , SOL_SOCKET , SO_KEEPALIVE , &enable , sizeof( VO_S32 ) ) )
 	{
 		VO_S32 value = 2;	
 		setsockopt(*ptr_socket, SOL_TCP, 6/*TCP_KEEPCNT*/, &value , sizeof(VO_S32));
 
 		value = 10;
 		setsockopt(*ptr_socket, SOL_TCP, 4/*TCP_KEEPIDLE*/, &value , sizeof(VO_S32));
 		setsockopt(*ptr_socket, SOL_TCP, 5/*TCP_KEEPINTVL*/, &value , sizeof(VO_S32));
 	}

	//set Receive timeout value default 30 senconds
	vo_socket_setReceiveTimeout(ptr_socket, 30);

#endif // _MAC_OS
#endif // _IOS
#endif

	return VO_TRUE;
}

#ifdef ENABLE_ASYNCDNS
//async dns related information transfered between requester and callback routine
struct AsyncDNSInfo
{
	VO_S32 * ptr_socket;	// socket handle
	int		 portnum;		//port number
	VO_BOOL  *bexit;		//exit flag set from outside
	VO_BOOL  *bconnected;	//connect result flag, callback function set it to true when connected successfully.
	VO_BOOL  *bDNSFailed;	//set the flag when DNS failed, we should not retry connect when DNS failed
	VO_CHAR	 *ptraddr;		//resolved dns ip addr
};

//refer http://c-ares.haxx.se/ares_gethostbyname.html for more details
static void AsyncDNSCallback(void *arg, int status, int timeouts, struct hostent *host)
{
	VOLOGI("async dns query timed out times: %d",timeouts );
	AsyncDNSInfo* pdnsinfo = (AsyncDNSInfo*)arg;
	if( !pdnsinfo )
	{
		VOLOGE("null pointer when async dns");
		return;
	}

	if (status != ARES_SUCCESS)
	{
		*(pdnsinfo->bDNSFailed) = VO_TRUE;
		VOLOGE("async dns failed: %s",ares_strerror(status));
		return;
	}
	else
	{
		*(pdnsinfo->bDNSFailed) = VO_FALSE;
	}

	char **p;
	for (p = host->h_addr_list; *p; p++)
	{
		char addr_buf[INET6_ADDRSTRLEN];
		memset(addr_buf , 0 , sizeof(addr_buf));
		ares_inet_ntop(host->h_addrtype, *p, addr_buf, sizeof(addr_buf));
		VOLOGI("%s \t %s", host->h_name, addr_buf);

		if( (*(pdnsinfo->bexit)) )
		{
			VOLOGI( "require exit when try the address list to connect" );
			break;
		}

		*(pdnsinfo->ptr_socket) = socket( host->h_addrtype, SOCK_STREAM, IPPROTO_TCP );

		if( *(pdnsinfo->ptr_socket) == -1 )
		{
			continue;
		}

		struct addrinfo adinfo;
		memset( &adinfo , 0 , sizeof(adinfo) );
		if( AF_INET == host->h_addrtype )
		{
			struct sockaddr_in socketInfo;
			memset(&socketInfo, 0, sizeof(socketInfo));
			socketInfo.sin_family            = AF_INET;
			socketInfo.sin_addr.s_addr       = ((in_addr *)(*p))->s_addr;
			socketInfo.sin_port              = htons( pdnsinfo->portnum );


			adinfo.ai_family = AF_INET;
			adinfo.ai_flags = 0;//??
			adinfo.ai_next = 0;
			adinfo.ai_protocol = IPPROTO_TCP;
			adinfo.ai_socktype = SOCK_STREAM;
			adinfo.ai_addr = (sockaddr *)&socketInfo;
			adinfo.ai_addrlen = sizeof(socketInfo);

			if( vo_socket_connect_ansyc( *(pdnsinfo->ptr_socket) , &adinfo ) )
			{
				VOLOGI( "ipv4 connected..." );
				*(pdnsinfo->bconnected) = VO_TRUE;
				strcpy( pdnsinfo->ptraddr , addr_buf );
				break;
			}
		}
		else
		if( AF_INET6 == host->h_addrtype )
		{
			VOLOGI( "ipv6 address detected..." );
			struct sockaddr_in6 socketInfo6;
			memset(&socketInfo6, 0, sizeof(socketInfo6));
			socketInfo6.sin6_family = AF_INET6;
			memcpy( socketInfo6.sin6_addr.s6_addr , ((in6_addr *)(*p))->s6_addr , sizeof( socketInfo6.sin6_addr));
			socketInfo6.sin6_port = htons( pdnsinfo->portnum );

			adinfo.ai_family = AF_INET6;
			adinfo.ai_flags = 0;
			adinfo.ai_next = 0;
			adinfo.ai_protocol = IPPROTO_TCP;
			adinfo.ai_socktype = SOCK_STREAM;
			adinfo.ai_addr = (sockaddr *)&socketInfo6;
			adinfo.ai_addrlen = sizeof(socketInfo6);

			if( vo_socket_connect_ansyc( *(pdnsinfo->ptr_socket) , &adinfo ) )
			{
				VOLOGI( "ipv6 connected..." );
				*(pdnsinfo->bconnected) = VO_TRUE;
				strcpy( pdnsinfo->ptraddr , addr_buf );
				break;
			}
		}
		else
		{
			continue;
		}

		vo_socket_close( *(pdnsinfo->ptr_socket));

		*(pdnsinfo->ptr_socket) = -1;

		*(pdnsinfo->bconnected) = VO_FALSE;
	}

	if( *(pdnsinfo->ptr_socket) == -1 || VO_FALSE == *(pdnsinfo->bconnected) )
	{
		VOLOGI( "failed to connect to server" );
		return;
	}

 	linger lg;
 	lg.l_linger = 0;
 	lg.l_onoff = 1;
 	setsockopt( *(pdnsinfo->ptr_socket) ,SOL_SOCKET ,SO_LINGER,(const char*)&lg,sizeof(linger));

#ifndef _WIN32
#ifndef _IOS
#ifndef _MAC_OS
 	VO_S32 enable = 1;
 	if( 0 == setsockopt( *(pdnsinfo->ptr_socket) , SOL_SOCKET , SO_KEEPALIVE , &enable , sizeof( VO_S32 ) ) )
 	{
 		VO_S32 value = 2;	
 		setsockopt(*(pdnsinfo->ptr_socket), SOL_TCP, 6/*TCP_KEEPCNT*/, &value , sizeof(VO_S32));
 
 		value = 10;
 		setsockopt(*(pdnsinfo->ptr_socket), SOL_TCP, 4/*TCP_KEEPIDLE*/, &value , sizeof(VO_S32));
 		setsockopt(*(pdnsinfo->ptr_socket), SOL_TCP, 5/*TCP_KEEPINTVL*/, &value , sizeof(VO_S32));
 	}

	//set Receive timeout value default 30 senconds
	vo_socket_setReceiveTimeout( pdnsinfo->ptr_socket , 30);

#endif // _MAC_OS
#endif // _IOS
#endif
}

//Wait for all queries to complete.
void wait_asyncdns2finish(ares_channel channel , VO_BOOL *bexit , VO_BOOL * bconnected)
{
	int nfds = 0;
	fd_set read_fds;
	fd_set write_fds;
	fd_set fde;
	struct timeval *tvp, tv;
	struct timeval maxtv;
	VO_U32 sum_timeout = 0;
	while( !(*bexit) && !(*bconnected) )
	{
		FD_ZERO(&read_fds);
		FD_ZERO(&write_fds);
		FD_ZERO(&fde);
		nfds = ares_fds(channel, &read_fds, &write_fds);
		if (nfds == 0)
		{
			VOLOGE( "DNS no queries are active");
			break;
		}
		
		//http://c-ares.haxx.se/ares_timeout.html
		maxtv.tv_sec = 2;
		maxtv.tv_usec = 0;
		tvp = ares_timeout(channel, &maxtv, &tv);
		VOLOGI( "DNS maximum time to wait: %d s, %d ms" , tvp->tv_sec , tvp->tv_usec );
		sum_timeout += (VO_U32)(tvp->tv_sec * 1000000 + tvp->tv_usec);
		if( sum_timeout > VODNSRESOLVETIMEOUTDEFAULT )
		{
			VOLOGE( "DNS Resolve timeout happen" );
			break;
		}

		VO_S32 select_ret = select(nfds, &read_fds, &write_fds, NULL, tvp);
		if( select_ret == -1 )
		{
			VOLOGE( "DNS socket error happen" );
			break;
		}

		if(FD_ISSET(nfds , &fde))
		{
			VOLOGE( "DNS socket exception happen" );
			break;
		}

		ares_process(channel, &read_fds, &write_fds);
	}
}

/* ==========================================================================================
** parameters set in:
* ptr_socket , socket handle
* str_host, the destination host name
* str_port, the destination host port
* bexit, the exit flag which tells whether interrupt the routine and exit directly
* bDNSFailed, the dns resolve result flag which tell whether dns resolve successfully or not
* ptr_addr, dns resolve ip address which is corresponde to the host
============================================================================================*/
VO_BOOL vo_socket_connect_asyncdns( VO_S32 * ptr_socket , VO_CHAR * str_host , VO_CHAR * str_port , VO_BOOL * bexit , VO_BOOL * bDNSFailed  , VO_CHAR *ptr_addr)
{
	ares_channel channel;
	int status = 0;
	int sa_family = AF_INET;

	if( bexit && *bexit )
	{
		VOLOGI( "require exit before DNS Lookup" );
		return VO_FALSE;
	}

	VO_BOOL bconnected = VO_FALSE;
	AsyncDNSInfo dnsinfo;
	dnsinfo.bexit = bexit;
	dnsinfo.bDNSFailed = bDNSFailed;
	dnsinfo.ptr_socket = ptr_socket;
	dnsinfo.bconnected = &bconnected;
	dnsinfo.portnum = atoi( str_port );
	dnsinfo.ptraddr = ptr_addr;

	do
	{
		*ptr_socket = -1;
		status = ares_init(&channel);
		if (status != ARES_SUCCESS)
		{
			VOLOGE( "Init ares channel failed" );
			return VO_FALSE;
		}
		
		if( AF_INET6 == sa_family )
		{
			VOLOGI( "DNS Lookup ipv6 started....." );
		}
		else
		{
			VOLOGI( "DNS Lookup ipv4 started....." );
		}
		ares_gethostbyname(channel, str_host , sa_family , AsyncDNSCallback, (void*)&dnsinfo );
		wait_asyncdns2finish( channel , bexit , &bconnected );
		ares_destroy(channel);

		if( sa_family != AF_INET6 && (*bDNSFailed) )
		{
			sa_family = AF_INET6;
		}
		else if( ( VO_FALSE == *(dnsinfo.bconnected) ) ||  *ptr_socket == -1 )
		{
			return VO_FALSE;
		}
	}while( !(*bexit) && (( VO_FALSE == *(dnsinfo.bconnected) ) ||  *ptr_socket == -1) );

	if( *bexit )
	{
		VOLOGI( "user require exit when dns look up" );
		return VO_FALSE;
	}

	return VO_TRUE;
}
#endif //ENABLE_ASYNCDNS

VO_BOOL vo_socket_setTimeout(VO_S32 * ptr_socket, VO_S32 recv , VO_S32 send )
{
	if( recv < DEFAULTVOSOCKETRECVTIMEOUT )
	{
		recv = DEFAULTVOSOCKETRECVTIMEOUT;
	}

#ifdef _WIN32
	VO_S32 timeout = recv * 1000;
#else
	timeval timeout; 
	timeout.tv_sec = recv;    
	timeout.tv_usec =0; 
#endif

	if (setsockopt ( *ptr_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
	{
		VOLOGE( "set socket recv timeout failed" );
		return VO_FALSE;
	}

	if( send < DEFAULTVOSOCKETSENDTIMEOUT )
	{
		send = DEFAULTVOSOCKETSENDTIMEOUT;
	}
#ifdef _WIN32
	timeout = send * 1000;
#else
	timeout.tv_sec = send; 
#endif
	if (setsockopt ( *ptr_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
	{
		VOLOGE( "set socket send timeout failed" );
		return VO_FALSE;
	}

	return VO_TRUE;
}


//set receive time out value acoording http server response field "X-SocketTimeout"
VO_S32 vo_socket_setReceiveTimeout(VO_S32 * ptr_socket, VO_S32 seconds) 
{
	if (seconds < 0) 
	{
		// Disable the Receive timeout.
		seconds = 0;
	}

	VO_S32 ret = 0;
#ifndef _WIN32
#ifndef _IOS
#ifndef _MAC_OS
	struct timeval tv;
	tv.tv_usec = 0;
	tv.tv_sec = seconds;

	ret = setsockopt( *ptr_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
#endif // _MAC_OS
#endif // _IOS
#endif

	return ret;
}
    
#ifdef _VONAMESPACE
}
#endif
