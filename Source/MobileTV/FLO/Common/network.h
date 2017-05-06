#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utility.h"

#include "voOSFunc.h"

#include "voRTSPCrossPlatform.h"

#define  CP_SOCKET 1 //cross platform socket


using namespace vo_cross_platform_streaming;


#define  PIVOSocket IVOSocket*


#define  VO_FD_SETSIZE 64

#define VO_FD_CLR(fd, set) do { \
	int __i; \
	for (__i = 0; __i < ((VOSocketGroup  *)(set))->fd_count ; __i++) { \
	if (((VOSocketGroup *)(set))->fd_array[__i] == fd) { \
	while (__i < ((VOSocketGroup  *)(set))->fd_count-1) { \
	((VOSocketGroup *)(set))->fd_array[__i] = \
	((VOSocketGroup *)(set))->fd_array[__i+1]; \
	__i++; \
	} \
	((VOSocketGroup *)(set))->fd_count--; \
	break; \
	} \
	} \
} while(0)

#define VO_FD_SET(fd, set) do { \
	int __i; \
	for (__i = 0; __i < ((VOSocketGroup *)(set))->fd_count; __i++) { \
	if (((VOSocketGroup *)(set))->fd_array[__i] == (fd)) { \
	break; \
	} \
	} \
	if (__i == ((VOSocketGroup  *)(set))->fd_count) { \
	if (((VOSocketGroup *)(set))->fd_count < VO_FD_SETSIZE) { \
	((VOSocketGroup *)(set))->fd_array[__i] = (fd); \
	((VOSocketGroup *)(set))->fd_count++; \
	} \
	} \
} while(0)

#define VO_FD_ZERO(set) (((VOSocketGroup *)(set))->fd_count=0)




bool StartupSocket();
void CleanupSocket();
int  GetSocketError();

unsigned long  NetToHostLong(unsigned long);
unsigned short NetToHostShort(unsigned short);
unsigned long  HostToNetLong(unsigned long);
unsigned short HostToNetShort(unsigned short);

int SendTCPData(PIVOSocket sock,const void* data,int size,int flag);
int GetLocalHostName(char* name,int namelen);

IVOSocket* DatagramSocket(int addressFamily, int protocol, unsigned short port = 0);
IVOSocket* StreamSocket(int addressFamily, int protocol, unsigned short port = 0);

int IsNetReadyToSend(IVOSocket* sock,int timeOutBySec);
int IsNetReadyToRead(IVOSocket* sock,int timeOutBySec);

int ReadNetData(IVOSocket* sock, unsigned char * buffer, unsigned int bufferSize, VOSocketAddress * fromAddr = NULL, long timeout = 0);
int ReadNetDataExact(IVOSocket* sock, unsigned char * buffer, unsigned int bufferSize, VOSocketAddress * fromAddr = NULL, long timeout = 0);
int BlockUntilReadableOrTimeout(IVOSocket* sock, long = NULL);

int WriteNetData(IVOSocket* sock, VOSocketAddress * toAddr, unsigned char * buffer, unsigned int bufferSize);

unsigned int GetSocketRecvBufSize(IVOSocket* sock);
unsigned int SetSocketRecvBufSize(IVOSocket* sock, unsigned int bufSize);

bool GetSocketLocalPort(IVOSocket* sock, unsigned short & port);

long Timeval2Int32MS(timeval * time);
long Timeval2Int32MSDiff(timeval * time1, timeval * time2);


void gettimeofday(timeval * time);


#endif //__NETWORK_H__