
#include "MediaStreamSocket.h"
#include "utility.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CMediaStreamSocket::CMediaStreamSocket(int addrFamily, int protocol, unsigned short port)
: m_streamSock(NULL)
{
	m_streamSock = DatagramSocket(addrFamily, protocol, port);
	if(m_streamSock&&m_streamSock->IsValidSocket())
	{
		m_port = port;
	}
	else
	{
		m_streamSock = NULL;
		m_port = 0;
	}
}

CMediaStreamSocket::~CMediaStreamSocket()
{
	if(m_streamSock)
	{
		VOCPFactory::DeleteOneSocket(m_streamSock);TRACE_DELETE(m_streamSock);
		m_streamSock = NULL;
	}
	
}

unsigned short CMediaStreamSocket::GetMediaStreamSockLocalPort()
{
	return m_port;
}

int CMediaStreamSocket::SetSockRecvBufSize(unsigned int bufSize)
{
	unsigned int resultBufSize = ::SetSocketRecvBufSize(m_streamSock, bufSize);
	unsigned int actualSize    = ::GetSocketRecvBufSize(m_streamSock);
	if(actualSize!=bufSize)
	{
		sprintf(CLog::formatString,"SetSockRecvBufSize:set=%d,actual=%d\n",bufSize,actualSize);
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt",CLog::formatString);
	}
	resultBufSize=actualSize;
	return resultBufSize;
}
