#ifndef __NETWORK_H__
#define __NETWORK_H__
#include "utility.h"
#include "stdio.h"
#include "voRTSPSDK.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

using namespace vo_cross_platform_streaming;


#define sockaddr_storage voSocketAddress
#define  PIVOSocket IVOSocket*
#define INVALID_SOCKET NULL
#define SOCKET_ERROR  VEC_SOCKET_ERROR
#define timeGetTime   voGetCurrentTime
#define ss_family family 
#define AF_INET	  VAF_INET4
#define DWORD	  long
#define HANDLE	  long
#define LPVOID	  void*
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#define fd_set VOSocketGroup

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

/*
#ifndef FD_SETSIZE
#define  FD_SETSIZE 64
#endif
*/
#ifndef VO_FD_CLR
#define VO_FD_CLR(fd, set) do { \
	int cnt; \
	for (cnt = 0; cnt < ((VOSocketGroup  *)(set))->fd_count ; cnt++) { \
	if (((VOSocketGroup *)(set))->fd_array[cnt] == fd) { \
	while (cnt < ((VOSocketGroup  *)(set))->fd_count-1) { \
	((VOSocketGroup *)(set))->fd_array[cnt] = \
	((VOSocketGroup *)(set))->fd_array[cnt+1]; \
	cnt++; \
	} \
	((VOSocketGroup *)(set))->fd_count--; \
	break; \
	} \
	} \
} while(0)
#endif

#ifndef VO_FD_SET
#define VO_FD_SET(fd, set) do { \
	int cnt; \
	for (cnt = 0; cnt < ((VOSocketGroup *)(set))->fd_count; cnt++) { \
	if (((VOSocketGroup *)(set))->fd_array[cnt] == (fd)) { \
	break; \
	} \
	} \
	if (cnt == ((VOSocketGroup  *)(set))->fd_count) { \
	if (((VOSocketGroup *)(set))->fd_count < 64) { \
	((VOSocketGroup *)(set))->fd_array[cnt] = (fd); \
	((VOSocketGroup *)(set))->fd_count++; \
	} \
	} \
} while(0)
#endif

#ifndef VO_FD_ZERO
#define VO_FD_ZERO(set) (((VOSocketGroup *)(set))->fd_count=0)
#endif

bool StartupSocket();
void CleanupSocket();
int  GetSocketError();
unsigned long  NetToHostLong(unsigned long);
unsigned short NetToHostShort(unsigned short);
unsigned long  HostToNetLong(unsigned long);
unsigned short HostToNetShort(unsigned short);
int SendTCPData(PIVOSocket sock,const void* data,int size,int flag);
int GetLocalHostName(char* name,int namelen);

int voGetCurrentTime();
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

long Timeval2Int32MS(VOTimeval * time);
long Timeval2Int32MSDiff(VOTimeval * time1,VOTimeval * time2);
void gettimeofday(VOTimeval * time);

#ifdef _VONAMESPACE
}
#endif

#endif //__NETWORK_H__
