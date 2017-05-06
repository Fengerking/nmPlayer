#ifndef __IPNETWORK_H__
#define __IPNETWORK_H__

#include "netbase.h"


#define SOCK_READABLE    (0x00000001)
#define SOCK_WRITABLE    (0x00000010)

const int MTU_SIZE = 1500;

class CIPSocket
{
public:
	CIPSocket();
	virtual ~CIPSocket();

public:
	virtual bool CreateDGramSocket(const char* inIPAddr, unsigned short port = 0) = 0;
	virtual bool CreateStreamSocket(unsigned short port = 0) = 0;
	virtual bool CloseSocket();
public:
	virtual bool GetSockOpt(int level, int optname, char * _optval, int * _optlen);
	virtual bool SetSockOpt(int level, int optname, const char * _optval, int optlen);
	virtual bool IOCtlSocket(long cmd, unsigned long * _argp);
public:
	virtual bool SetRecvBufSize(unsigned int recvBufSize);
	virtual bool GetRecvBufSize(unsigned int * _recvBufSize);
	virtual bool EnableBlocking(bool bBlocking); 
public:
	virtual bool Bind(const char* ipAddress, unsigned short port) = 0;
	virtual bool Connect(const char * _address, unsigned short port, struct timeval * _timeout = NULL);
	virtual int Select(int checkAvailable, struct timeval * _timeout = NULL);
	virtual int Receive(void * _buffer, int bufLen, struct timeval * _timeout = NULL);
	virtual int ReceiveFrom(void * _buffer, int bufLen, struct sockaddr_storage * pAddrFrom = NULL);
	virtual int Send(const void * _buffer, int bufLen, struct timeval * _timeout = NULL);
	virtual int SendTo(const void * _buffer, int bufLen, struct sockaddr_storage * pAddrTo, int sockAddrLen);
public:
	virtual bool JoinMulticastGroup(const char * _multicastAddr, const char * _interface = NULL) = 0;
	virtual bool LeaveMulticastGroup(const char * _multicastAddr, const char * _interface = NULL) = 0;

public:
	SOCKET GetSocket() { return m_socket; }
protected:
	SOCKET m_socket;
};



class CIPv4Socket : public CIPSocket
{
public:
	CIPv4Socket();
	virtual ~CIPv4Socket();

public:
	virtual bool CreateDGramSocket(const char* inIPAddr, unsigned short port = 0);
	virtual bool CreateStreamSocket(unsigned short port = 0);
public:
	virtual bool Bind(const char* inIPAdd, unsigned short port);
public:
	virtual bool JoinMulticastGroup(const char * _multicastAddr, const char * _interface = NULL);
	virtual bool LeaveMulticastGroup(const char * _multicastAddr, const char * _interface = NULL);
};



class CIPv6Socket : public CIPSocket
{
public:
	CIPv6Socket();
	virtual ~CIPv6Socket();

public:
	virtual bool CreateDGramSocket(const char* inIPAddr, unsigned short port = 0);
	virtual bool CreateStreamSocket(unsigned short port = 0);
public:
	virtual bool Bind(const char* inIPAddr, unsigned short port);
public:
	virtual bool JoinMulticastGroup(const char * _multicastAddr, const char * _interface = NULL);
	virtual bool LeaveMulticastGroup(const char * _multicastAddr, const char * _interface = NULL);
};


bool InitWinsock();
void CleanupWinsock();


#endif //__IPNETWORK_H__
