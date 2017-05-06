#ifndef __VORTSPCROSSPLATFORM_H__
#define __VORTSPCROSSPLATFORM_H__

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

namespace vo_cross_platform_streaming{
	class IVOCommand
	{
	public:
		virtual ~IVOCommand(){};
		virtual  void Execute()=0;
	};

	class IVOMutex
	{
	public:
		virtual ~IVOMutex(){};
		virtual void Lock()=0;
		virtual void UnLock()=0;
	};

	class CAutoLock
	{
	public:
		CAutoLock(IVOMutex* pMutex)
		{
			m_pMutex = pMutex;
			if(m_pMutex)
			m_pMutex->Lock();
		}

		~CAutoLock()
		{
			if(m_pMutex)
				m_pMutex->UnLock();
		}

	protected:
		IVOMutex*			  m_pMutex;
	};
	typedef enum 
	{
		PRIO_LOWEST			=0,
		PRIO_LOW			=1,
		PRIO_NORMAL			=2,
		PRIO_HIGH			=3,
		PRIO_HIGHEST		=4
	}VOThreadPriority;

	typedef long VOThreadID;
	class IVOThread
	{
	public:
		virtual	~IVOThread(){};
		///Create the thread implementation and return the ID of the thread
		///the cmd is the ThreadFunc provided by caller
		///If the IsHangup==0,run the thread main function immediately,otherwise call
		///Start() later
		virtual VOThreadID Create(IVOCommand* cmd,long IsHangUp=0)=0;
		///if the created thread is hangup,call start() to make it run
		virtual	long Start()=0;
		///return true if the thread is running,otherwise false
		virtual long IsRunning()=0;
		///return the thread ID,on windows,it can be the Handle of the Thread
		virtual long GetThreadID()=0;
		/// Waits for at most the given interval for the thread
		/// to complete. Returns true if the thread has finished,
		/// false otherwise.
		virtual long TryJoin(long milliseconds=0x7fffffff)=0;
		/// Returns the thread's priority.
		virtual VOThreadPriority GetPriority()=0;
		/// Set the thread's priority.
		virtual void SetPriority(VOThreadPriority)=0;
		/// Suspends the current thread for the specified
		/// amount of time.
		static void Sleep(long milliseconds);
		/// Yields cpu to other threads.
		static void YieldCpu();

	};

	typedef enum
	{
		VST_SOCK_STREAM	=1,
		VST_SOCK_DGRAM	=2
	}VOSockType;
	typedef enum
	{
		VSE_FD_READ		= 1,	/*!< Want to receive notification of readiness for reading */
		VSE_FD_WRITE	= 1<<1, /*!< Want to receive notification of readiness for writing */
		VSE_FD_OOB		= 1<<2, /*!< Want to receive notification of the arrival of out-of-band data */
		VSE_FD_ACCEPT	= 1<<3, /*!< Want to receive notification of incoming connections */
		VSE_FD_CONNECT	= 1<<4, /*!< Want to receive notification of completed connection */
		VSE_FD_CLOSE	= 1<<5,  /*!< Want to receive notification of socket closure */
		VSE_FD_ERROR	= 1<<6,	/*!< Want to receive notification of socket error */
	}VOSockEvent;
	typedef enum
	{
		VPT_TCP		=6,		/*!< The Transmission Control Protocol (TCP).  */
		VPT_UDP		=17,	/*!< The User Datagram Protocol (UDP).  */
		VPT_RM		=113,	/*!< The PGM protocol for reliable multicast */
	}VOProtocolType;
	typedef enum
	{
		VAF_UNSPEC	=0,		/*!< TThe address family is unspecified  */
		VAF_INET4	=2,		/*!< The Internet Protocol version 4 (IPv4) address family  */
		VAF_INET6	=23,	/*!< The Internet Protocol version 6 (IPv6) address family */	
	}VOAddressFamily;
	typedef struct  voSocketAddress
	{
		unsigned long	ipAddress;	/*!< the byte order must be big endian(network byte order)*/
		unsigned short  port;		/*!< the byte order must be big endian(network byte order)*/
		VOAddressFamily family;
		VOProtocolType	protocol;
		VOSockType		sockType;
		int				userData1;	/*!< reserved for user data*/
		int				userData2;	/*!< reserved for user data*/
	}VOSocketAddress;
	typedef enum
	{
		VSD_READ	= 0,
		VSD_SEND	= 1,
		VSD_BOTH	= 2,
	}VOSockShutOpt;
	typedef enum
	{
		VSMT_NONE			= 0,
		VSMT_OOB			= 1,				/*!< process out-of-band data */
		VSMT_PEEK			= 1<<1,				/*!< peek at incoming message */
		VSMT_DONTROUTE		= 1<<2,            /*!< send without using routing tables */
		VSMT_WAITALL		= 1<<3,           /*!< do not complete until packet is completely filled */
		VSMT_PARTIAL		= 0x8000,         /*!< partial send or recv for message*/

	}VOSockMSGType;
	typedef enum
	{
		VEC_SOCKET_ERROR	= -1,
		VEC_INVALID_SOCKET	= (~0),
	}VOErrCode;
#ifndef LINUX//somehow, there is also a votimeval in system
#endif//LINUX
	typedef struct votimeval{
		long    tv_sec;         /* seconds */
		long    tv_usec;        /* and microseconds */
	}VOTimeval;

	class IVOSocket;
	typedef struct  
	{
		int			  fd_count;
		IVOSocket*	  fd_array[64];   /* an array of SOCKETs */
	}VOSocketGroup;
	/** 
	*
	*/
	class IVOSocket
	{
	public:
		IVOSocket(VOSockType sockType,VOProtocolType proType,VOAddressFamily af=VAF_UNSPEC){};
		virtual ~IVOSocket(){};
		/** 
		*Init the socket library on the platform
		*\param versionNum,the number of the library
		*\param userData
		*\retVal 0:success,others fail
		*/
		static int Init(long versionNum=2,void* userData=0);
		/** 
		*UnInit the socket library on the platform
		*\retVal 0:success,others fail
		*/
		static int UnInit(void* userData=0);
		/** 
		*Get the blocking mode
		*/
		static int GetSockLastError();
		/** 
		*convert the Host byte order to network byte order
		*/
		static unsigned short HostToNetworkShort(unsigned short hostData);
		/** 
		*convert the Host byte order to network byte order
		*/
		static unsigned long HostToNetworkLong(unsigned long hostData);
		/** 
		*convert the network byte  order to Host byte order
		*/
		static unsigned short NetworkToHostShort(unsigned short netData);
		/** 
		*convert the network byte  order to Host byte order
		*/
		static unsigned long NetworkToHostLong(unsigned long netData);
		static int GetHostName(char* name,int namelen);
		static int GetCurrTime();
		static void GetTimeOfDay(VOTimeval * time);
		static int SelectSockGroup(VOSockEvent soEvent,VOSocketGroup* group,long timeout);

		//voSockGroup update
		static int	 AddOneSockToGroup(IVOSocket* sock,VOSocketGroup* group);
		static int	 RemoveOneSockFromGroup(IVOSocket* sock,VOSocketGroup* group);
		static int	 CleanSockGroup(VOSocketGroup* group);
		static int	 IsSockInGroup(IVOSocket* sock,VOSocketGroup* group);
		/** 
		*Returns the IP address of the peer.
		*\Param hostName[IN]:the name of peer,say, www.yahoo.com
		*\Param port[IN]:the port of the service
		*\Param address[IN\OUT]:family,sockType,protocol,port(optional)should be input params,
		*the ipField is the output
		*\Retval 0:ok,others fails
		*/
		static int GetPeerAddressByURL(char* hostName,int hostNamelen,VOSocketAddress* adderss);
		/** 
		*Create one socket
		*/
		//virtual int Create(VOSockType sockType,VOProtocolType proType,VOAddressFamily af=VAF_UNSPEC)=0;

		/** 
		*Initializes the socket, sets the socket timeout and 
		*establishes a connection to the TCP server at the given address.
		*If the timeout=0,the connect mode is blocking,ohterwise non-blocking
		*/
		virtual int Connect(const VOSocketAddress& address, unsigned long timeout=0)=0;
		/** 
		*Bind a local address to the socket.
		*If reuseAddress is true, sets the SO_REUSEADDR
		*socket option.
		*/
		virtual int Bind(const VOSocketAddress& address, bool reuseAddress = true)=0;
		/** 
		*Puts the socket into listening state.
		*The socket becomes a passive socket that
		*can accept incoming connection requests.
		*The backlog argument specifies the maximum
		*number of connections that can be queued
		*for this socket.
		*/
		virtual int Listen(int backlog = 64)=0;
		/** 
		*Get the next completed connection from the socket's completed connection queue.
		*the new socket will save the peerAddress. 
		*/
		virtual IVOSocket* Accept()=0;
		/** 
		*Close the socket.
		*/
		virtual int Close()=0;
		/** 
		* disables sends or receives or both on a socket
		*/
		virtual int Shutdown(VOSockShutOpt opt= VSD_BOTH)=0;
		/** 
		*determines the status of one or more sockets, waiting if necessary, to perform synchronous I/O.
		*\param soEvent 
		*\param timeout, if timeout=0,blocking the operation. 
		*/
		virtual int Select(VOSockEvent soEvent, long timeout=0)=0;
		/** 
		*Sends the contents of the given buffer through the socket.Returns the number of bytes sent, which may be
		*less than the number of bytes specified.
		*/
		virtual int Send(const char* buffer, int length, VOSockMSGType flags = VSMT_NONE)=0;
		/** 
		*The recv function receives data from a connected or bound socket
		*returns the number of bytes received.
		*/
		virtual int Recv(char* buffer, int length, VOSockMSGType flags = VSMT_NONE)=0;
		/** 
		*Sends the contents of the given buffer through the socket to the given address.
		*Returns the number of bytes sent
		*/
		virtual int SendTo(const void* buffer, int length, const VOSocketAddress& address, VOSockMSGType flags = VSMT_NONE)=0;
		/** 
		*Receives data from the socket and stores it in buffer
		*Returns the number of bytes received.
		*/
		virtual int ReceiveFrom(void* buffer, int length, VOSocketAddress& address, VOSockMSGType flags = VSMT_NONE)=0;
		/** 
		*SetSendBufferSize
		*/
		virtual int SetSendBufferSize(int size)=0;
		/** 
		*GetSendBufferSize
		*/
		virtual int GetSendBufferSize()=0;
		/** 
		*SetRecvBufferSize
		*/
		virtual int SetRecvBufferSize(int size)=0;
		/** 
		*GetRecvBufferSize
		*/
		virtual int GetRecvBufferSize()=0;
		/** 
		*retrieves the current value for a socket option 
		*associated with a socket of any type, in any state, and stores the result in optval
		*/
		virtual int SetSockOpt(int level,int optname,const char* optVal,int optLen)=0;
		/** 
		*retrieves the current value for a socket option 
		*associated with a socket of any type, in any state, and stores the result in optval
		*/
		virtual int GetSockOpt(int level,int optname,char* optVal,int* optLen)=0;
		/** 
		*Set IP address and port number.
		*/
		virtual int SetAddress(VOSocketAddress* adderss)=0;
		/** 
		*Returns the IP address and port number of the socket.
		*/
		virtual int GetAddress(VOSocketAddress* adderss)=0;
		/** 
		*Set IP address and port number. of the peersocket.
		*/
		virtual int SetPeerAddress(VOSocketAddress* adderss)=0;
		/** 
		*Returns the IP address and port number of the peersocket.
		*/
		virtual int GetPeerAddress(VOSocketAddress* adderss)=0;
		/** 
		*Returns the IP address and port number of the socket.
		*/
		//virtual int GetAddressByURL(char* hostName,int hostNamelen,int port,VOSocketAddress* adderss)=0;


		/** 
		*Set the blocking mode
		*/
		virtual int SetBlocking(bool isBlocking=true)=0;
		/** 
		*Get the blocking mode
		*/
		virtual bool GetBlocking()=0;

		/** 
		*check the sockt is valid or not
		*/
		virtual bool IsValidSocket()=0;

		virtual unsigned long GetSockID()=0;
	};
	class VOCPFactory
	{
	public:
		static IVOMutex*	CreateOneMutex();
		static void DeleteOneMutex(IVOMutex* mutex);
		static IVOThread*	CreateOneThread();
		static void DeleteOneThread(IVOThread* thread)	;
		static IVOSocket*	CreateOneSocket(VOSockType sockType,VOProtocolType proType,VOAddressFamily af=VAF_UNSPEC);
		static void DeleteOneSocket(IVOSocket* sock)	;
	};
	
}

#ifdef _VONAMESPACE
}
#endif

#endif//__VORTSPCROSSPLATFORM_H__