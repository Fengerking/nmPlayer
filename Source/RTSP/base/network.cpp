#include "network.h"
#include "utility.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

int voGetCurrentTime()
{
	return IVOSocket::GetCurrTime();
}
void SocketErrorReport(const char* errMessage,int errID)
{
	sprintf(CLog::formatString,"%s,id=%d\n",errMessage,errID);
	CLog::Log.MakeLog(LL_SOCKET_ERR,"socketErr.txt",CLog::formatString);
	CUserOptions::UserOptions.errorID = errID;
	NotifyEvent(VO_EVENT_SOCKET_ERR,CUserOptions::UserOptions.errorID);
}
int IsNetReadyToRead(IVOSocket* sock,int timeOutBySec)
{
	
	int ret;

	if(timeOutBySec>30)
		timeOutBySec = 30;
	if(timeOutBySec<1)
		timeOutBySec = 1;

	
	CUserOptions::UserOptions.m_timeoutCounter = 0;
	do 
	{
		int ret = sock->Select(VSE_FD_READ,1000);// select(0,NULL,&fdwrite,NULL,&timeout);
		if(ret==VEC_SOCKET_ERROR)
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"socketErr.txt","@@@@fail to send rtsp request\n");
		}
		return ret;
		if(ret==0)
		{
			CUserOptions::UserOptions.m_timeoutCounter++;
			if(CUserOptions::UserOptions.m_exit)
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR,"socket_timeout.txt","IsNetReadyToRead:exit!!\n");
				ret = -1;
				break;
			}
			IVOThread::Sleep(10);
		}
		else
			break;

	} while(--timeOutBySec>0);
	CUserOptions::UserOptions.m_timeoutCounter = 0;

	return ret;
}
int IsNetReadyToSend(IVOSocket* sock,int timeOutBySec)
{
	
	int ret = sock->Select(VSE_FD_WRITE,timeOutBySec);// select(0,NULL,&fdwrite,NULL,&timeout);
	if(ret==VEC_SOCKET_ERROR)
	{
		CLog::Log.MakeLog(LL_SOCKET_ERR,"socketErr.txt","@@@@fail to send rtsp request\n");
	}
	return ret;
}
bool StartupSocket()
{
	bool ret = (IVOSocket::Init(0, NULL)==0);
	return ret;
}

void CleanupSocket()
{
	//WSACleanup();
	IVOSocket::UnInit();
}

static IVOSocket* CreateSocket(int addressFamily, int protocol,int sockType, unsigned short port)
{
	IVOSocket* streamSock = NULL;
	
	streamSock = VOCPFactory::CreateOneSocket((VOSockType)sockType, (VOProtocolType)protocol,(VOAddressFamily)addressFamily);
	TRACE_NEW("streamSock",streamSock);

	//socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // protocol
	if(streamSock->IsValidSocket()==false)
	{
		SocketErrorReport("@@@@fail to create socket",streamSock->GetSockLastError());
		VOCPFactory::DeleteOneSocket(streamSock);TRACE_DELETE(streamSock);
		return NULL;
	}

	VOSocketAddress sockAddress;
	sockAddress.family		=	(VOAddressFamily)addressFamily;
	sockAddress.protocol	=	(VOProtocolType)protocol;
	sockAddress.sockType	=	(VOSockType)sockType;
	sockAddress.port		=   streamSock->HostToNetworkShort(port);
	sockAddress.ipAddress	=   0;

	if(streamSock->Bind(sockAddress) == VEC_SOCKET_ERROR) 
	{
		//CLog::Log.MakeLog(LL_SOCKET_ERR,"socketErr.txt","@@@@fail to bind3...localAddress");
		SocketErrorReport("@@@@fail to bind3...localAddress",streamSock->GetSockLastError());
		VOCPFactory::DeleteOneSocket(streamSock);TRACE_DELETE(streamSock);
		return NULL;
	}
	return streamSock;
}

IVOSocket* DatagramSocket(int addressFamily, int protocol, unsigned short port)
{
	return CreateSocket(addressFamily,VPT_UDP,VST_SOCK_DGRAM,port);
}

IVOSocket* StreamSocket(int addressFamily, int protocol, unsigned short port)
{
	return CreateSocket(addressFamily,VPT_TCP,VST_SOCK_STREAM,port);	
}

int ReadNetData(IVOSocket* sock, unsigned char * buffer, unsigned int bufferSize, VOSocketAddress* fromAddr, long timeout)
{
#if TRACE_FLOW1
	sprintf(CLog::formatString,"$$$$$enter ReadNetData sock=%d\n",sock);
	CLog::Log.MakeLog(LL_RTP_ERR,"flow.txt",CLog::formatString);
#endif//
	int recvBytes = 0;
	do 
	{
		if(buffer==NULL||(int)bufferSize <= 0)    
			break;

		recvBytes = sock->ReceiveFrom(buffer,bufferSize,*fromAddr);

		if(recvBytes==0)
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"socketErr.txt","######nothing Received,the connection has been gracefully closed!\n");
			//CUserOptions::UserOptions.errorID = E_CONNECTION_RESET;
			//NotifyEvent(VO_EVENT_SOCKET_ERR	,CUserOptions::UserOptions.errorID);
		}
		if(recvBytes==VEC_SOCKET_ERROR)
		{
#define MAX_ERROR 50
			static int errCount=0;
			int err=sock->GetSockLastError();
			if(err!=10040)//message is too big for the
			{
				sprintf(CLog::formatString,"recvfrom data fails,sock=%ld,buf=%X,len=%d,err=%d,%s\n",sock->GetSockID(),(unsigned int)buffer,bufferSize,err,++errCount>MAX_ERROR?"exit":"try again");
				CLog::Log.MakeLog(LL_SOCKET_ERR,"socketErr.txt",CLog::formatString);
			}

			if(errCount>MAX_ERROR)
			{
				errCount=0;

				CUserOptions::UserOptions.errorID = E_CONNECTION_RESET;
				NotifyEvent(VO_EVENT_SOCKET_ERR	,CUserOptions::UserOptions.errorID);
			}
			else
			{
				break;
			}
			
		}
	}while(0);
	return recvBytes;
}

int ReadNetDataExact(IVOSocket* sock, unsigned char * buffer, unsigned int bufferSize, VOSocketAddress* fromAddr, long timeout)
{
	int recvBytes = 0;
	unsigned int totalRecvBytes = 0;
	while(totalRecvBytes < bufferSize)
	{
		recvBytes = ReadNetData(sock, buffer + totalRecvBytes, bufferSize - totalRecvBytes, fromAddr, timeout);
		if(recvBytes == VEC_SOCKET_ERROR) 
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"socketErr.txt","@@@@fail to ReadNetData\n");
			break;
		}

		totalRecvBytes += recvBytes;
	}

	return totalRecvBytes;
}

int BlockUntilReadableOrTimeout(IVOSocket* sock, long timeout)
{
	int ret = 1;
	ret = sock->Select(VSE_FD_READ,timeout);
	return ret;
}

int WriteNetData(IVOSocket* sock, VOSocketAddress* toAddr, unsigned char * buffer, unsigned int bufferSize)
{
	int bytesSent = sock->SendTo(buffer,bufferSize,*toAddr);
	//sendto(sock, (char *)buffer, bufferSize, 0, (struct sockaddr *)toAddr, sizeof(sockaddr));
	if(bytesSent != (int)bufferSize) 
	{
		sprintf(CLog::formatString,"sendto data not complete,plan=%d,actual=%d\n",bufferSize,bytesSent);
		CLog::Log.MakeLog(LL_SOCKET_ERR,"socketErr.txt",CLog::formatString);
	}
	if(bytesSent == VEC_SOCKET_ERROR) 
	{
		sprintf(CLog::formatString,"sendto data fails,err=%d\n",sock->GetSockLastError());
		CLog::Log.MakeLog(LL_SOCKET_ERR,"socketErr.txt",CLog::formatString);
	}
	return bytesSent;
}

unsigned int GetSocketRecvBufSize(IVOSocket* sock)
{
	int recvBufOpt;
	recvBufOpt=sock->GetRecvBufferSize();
	if(recvBufOpt==VEC_SOCKET_ERROR) //getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&recvBufOpt, &sizeOpt) == SOCKET_ERROR) 
	{
		CLog::Log.MakeLog(LL_SOCKET_ERR,"socketErr.txt","@@@@fail to GetSocketRecvBufSize\n");
		recvBufOpt = 0;
	}

	return recvBufOpt;
}

unsigned int SetSocketRecvBufSize(IVOSocket* sock, unsigned int bufSize)
{
	sock->SetRecvBufferSize(bufSize);
	return GetSocketRecvBufSize(sock);
}

bool GetSocketLocalPort(int sock, unsigned short & port)
{
	return true;
}

long Timeval2Int32MS(VOTimeval * time)
{

	return time->tv_sec*1000 + (time->tv_usec+500)/1000;
}
long Timeval2Int32MSDiff(VOTimeval * time1,VOTimeval * time2)
{
	long seconds  = time1->tv_sec - time2->tv_sec;
	long uSeconds = time1->tv_usec - time2->tv_usec;
	if((int)uSeconds < 0)
	{
		uSeconds += million;
		--seconds;
	}
	VOTimeval tmp;
	tmp.tv_sec	=seconds;
	tmp.tv_usec	=uSeconds;
	return Timeval2Int32MS(&tmp);
}

void gettimeofday(VOTimeval * time)
{
	IVOSocket::GetTimeOfDay(time);
}
int SendTCPData(PIVOSocket sock,const void* data,int size,int flag)
{
	return sock->Send((char*)data,size,(VOSockMSGType)flag);
}
int  GetSocketError()
{
	return IVOSocket::GetSockLastError();
}
unsigned long  NetToHostLong(unsigned long data)
{
	return IVOSocket::NetworkToHostLong (data);
}
unsigned short NetToHostShort(unsigned short data)
{
	return IVOSocket::NetworkToHostShort(data);
}
unsigned long  HostToNetLong(unsigned long data)
{
	return IVOSocket::HostToNetworkLong(data);
}
unsigned short HostToNetShort(unsigned short data)
{
	return IVOSocket::HostToNetworkShort(data);
}
int GetLocalHostName(char* name,int namelen)
{
	return IVOSocket::GetHostName(name,namelen);
}

#ifdef _VONAMESPACE
}
#endif