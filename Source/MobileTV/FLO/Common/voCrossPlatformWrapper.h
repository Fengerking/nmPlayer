#ifndef vo_cross_platform_streaming_H
#define vo_cross_platform_streaming_H

#define WIN32_LEAN_AND_MEAN 1

#include <Windows.h>

#include "voRTSPCrossPlatform.h"

using namespace vo_cross_platform_streaming;

class CVOSocketWindows:public IVOSocket
{
public:
	CVOSocketWindows(VOSockType sockType,VOProtocolType proType,VOAddressFamily af=VAF_UNSPEC);
	~CVOSocketWindows();
	
	//virtual int UnInit(void* userData=NULL);
	/** 
	*Create one socket
	*/
	//virtual int Create(VOSockType sockType,VOProtocolType proType,VOAddressFamily af=VAF_UNSPEC);

	/** 
	*Initializes the socket, sets the socket timeout and 
	*establishes a connection to the TCP server at the given address.
	*If the timeout=0,the connect mode is blocking,ohterwise non-blocking
	*/
	virtual int Connect(const VOSocketAddress& address, unsigned long timeout=0);
	/** 
	*Bind a local address to the socket.
	*If reuseAddress is true, sets the SO_REUSEADDR
	*socket option.
	*/
	virtual int Bind(const VOSocketAddress& address, bool reuseAddress = true);
	/** 
	*Puts the socket into listening state.
	*The socket becomes a passive socket that
	*can accept incoming connection requests.
	*The backlog argument specifies the maximum
	*number of connections that can be queued
	*for this socket.
	*/
	virtual int Listen(int backlog = 64);
	/** 
	*Get the next completed connection from the socket's completed connection queue.
	*/
	virtual IVOSocket* Accept();
	/** 
	*Close the socket.
	*/
	virtual int Close();
	/** 
	* disables sends or receives or both on a socket
	*/
	virtual int Shutdown(VOSockShutOpt opt= VSD_BOTH);
	/** 
	*determines the status of one or more sockets, waiting if necessary, to perform synchronous I/O.
	*\param soEvent 
	*\param timeout, if timeout=0,blocking the operation. 
	*/
	virtual int Select(VOSockEvent soEvent, long timeout=0);
	/** 
	*Sends the contents of the given buffer through the socket.Returns the number of bytes sent, which may be
	*less than the number of bytes specified.
	*/
	virtual int Send(const char* buffer, int length, VOSockMSGType flags = VSMT_NONE);
	/** 
	*The recv function receives data from a connected or bound socket
	*returns the number of bytes received.
	*/
	virtual int Recv(char* buffer, int length, VOSockMSGType flags = VSMT_NONE);
	/** 
	*Sends the contents of the given buffer through the socket to the given address.
	*Returns the number of bytes sent
	*/
	virtual int SendTo(const void* buffer, int length, const VOSocketAddress& address, VOSockMSGType flags = VSMT_NONE);
	/** 
	*Receives data from the socket and stores it in buffer
	*Returns the number of bytes received.
	*/
	virtual int ReceiveFrom(void* buffer, int length, VOSocketAddress& address, VOSockMSGType flags = VSMT_NONE);
	/** 
	*SetSendBufferSize
	*/
	virtual int SetSendBufferSize(int size);
	/** 
	*GetSendBufferSize
	*/
	virtual int GetSendBufferSize();
	/** 
	*SetRecvBufferSize
	*/
	virtual int SetRecvBufferSize(int size);
	/** 
	*GetRecvBufferSize
	*/
	virtual int GetRecvBufferSize();
	/** 
	*retrieves the current value for a socket option 
	*associated with a socket of any type, in any state, and stores the result in optval
	*/
	virtual int SetSockOpt(int level,int optname,const char* optVal,int optLen);
	/** 
	*retrieves the current value for a socket option 
	*associated with a socket of any type, in any state, and stores the result in optval
	*/
	virtual int GetSockOpt(int level,int optname,char* optVal,int* optLen);
	
	/** 
	*Set the blocking mode
	*/
	virtual int SetBlocking(bool isBlocking=true);
	/** 
	*Get the blocking mode
	*/
	virtual bool GetBlocking();
	
	/** 
	*Set IP address and port number.
	*/
	virtual int SetAddress(VOSocketAddress* adderss);
	/** 
	*Returns the IP address and port number of the socket.
	*/
	virtual int GetAddress(VOSocketAddress* adderss);
	/** 
	*Set IP address and port number. of the peersocket.
	*/
	virtual int SetPeerAddress(VOSocketAddress* adderss);
	/** 
	*Returns the IP address and port number of the peersocket.
	*/
	virtual int GetPeerAddress(VOSocketAddress* adderss);
	/** 
	*Returns the IP address and port number of the socket.
	*/
	//virtual int GetAddressByURL(char* hostName,int hostNamelen,int port,VOSocketAddress* adderss);

	/** 
	*Returns the IP address and port number of the socket.
	*/
	//virtual int GetPeerAddressByURL(char* hostName,int hostNamelen,int port,VOSocketAddress* adderss);
	/** 
	*check the sockt is valid or not
	*/
	virtual bool IsValidSocket();
	//static int GetSockLastError();
	virtual unsigned long GetSockID(){return m_sock;};

private:
	VOSocketAddress	m_sockAddress;
	VOSocketAddress	m_peerSockAddress;
	unsigned long   m_sock;
};


#endif//vo_cross_platform_streaming