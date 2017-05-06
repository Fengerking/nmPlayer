#if defined(WIN32)
	#include <winsock2.h>
#elif defined(_LINUX_ANDROID)
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <sys/time.h>
	#include <netdb.h>
	#include <sys/ioctl.h>
	#include <net/if.h>
	#include <sys/utsname.h>
	#include <netinet/in.h>
	#include <unistd.h>
	#include <errno.h>
	#include <semaphore.h>
	#include <malloc.h>
	#include <fcntl.h>
	#include <signal.h>
#endif
#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif

#include <stdio.h>
#include <stdlib.h>
#include "voClientContext.h"
#include "voServerAnsycSocket.h"
#include "voHttpSvrExport.h"

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

const int GPORT = 5080;

#ifdef WIN32
DllExport int InitSvrAndStart(int nPort)
#else
int InitSvrAndStart(int nPort)
#endif
{
#if defined(WIN32)
     // Initialize Winsock
     WSADATA wsaData;
     int nResult;
     nResult = WSAStartup(MAKEWORD(2,2), &wsaData);
     if (NO_ERROR != nResult)
     {
          printf("\nError occurred while executing WSAStartup().");
          return 1; //error
     }
     else
     {
          printf("\nWSAStartup() successful.");
     }
#endif
#if defined(WIN32)
     SOCKET ListenSocket;
#elif defined(_POSIX)||defined(_LINUX_ANDROID)
	int ListenSocket;
#endif
     int    nPortNo;

     struct sockaddr_in ServerAddress;

     //Create a socket
     ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

     if (INVALID_SOCKET == ListenSocket) 
     {
#ifdef WIN32
          printf("\nError occurred while opening socket: %ld.", WSAGetLastError());
		  goto error;
#endif
          
     }
     else
     {
          printf("\nsocket() successful.");
     }

     //Cleanup and Init with 0 the ServerAddress
     memset((char *)&ServerAddress, 0,sizeof(ServerAddress));

     //Port number will be supplied as a commandline argument
     nPortNo = GPORT;

     //Fill up the address structure
     ServerAddress.sin_family = AF_INET;
     ServerAddress.sin_addr.s_addr = INADDR_ANY; //WinSock will supply address
     ServerAddress.sin_port = htons(nPortNo);    //comes from commandline

     //Assign local address and port number
     if (SOCKET_ERROR == bind(ListenSocket, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress))) 
     {
		 
#if defined(WIN32)
          closesocket(ListenSocket);
		  printf("\nError occurred while binding.");
		   goto error;
#elif defined(_POSIX)||defined(_LINUX_ANDROID)
		 close(ListenSocket);
#endif
  
     }
     else
     {
          printf("\nbind() successful.");
     }

     //Make the socket a listening socket
     if (SOCKET_ERROR == listen(ListenSocket,SOMAXCONN))
     {
#if defined(WIN32)
		  printf("\nError occurred while listening.");
          closesocket(ListenSocket);
		   goto error;
#elif defined(_POSIX)||defined(_LINUX_ANDROID)
		  close(ListenSocket);
#endif
        
     }
     else
     {
          printf("\nlisten() successful.");
     }
     
	 voServerAnsycSocket *AnsycServer = new voServerAnsycSocket;
     //This function will take are of multiple clients using select()/accept()
     AnsycServer->AcceptConnections(ListenSocket);
	 delete AnsycServer;
#if defined(WIN32)
     //Close open sockets
     closesocket(ListenSocket);
#elif defined(_POSIX)||defined(_LINUX_ANDROID)
	 close(ListenSocket);
#endif
#ifdef WIN32
     //Cleanup Winsock
     WSACleanup();
#endif
     return 0; //success


#ifdef WIN32
error:
     // Cleanup Winsock
     WSACleanup();
#endif
     return 1; //error

}