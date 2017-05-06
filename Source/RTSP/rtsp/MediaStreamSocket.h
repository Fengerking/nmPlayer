#ifndef __MEDIASTREAMSOCKET_H__
#define __MEDIASTREAMSOCKET_H__
#include "network.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CMediaStreamSocket
{
public:
	CMediaStreamSocket(int addrFamily, int protocol, unsigned short port = 0);
	virtual ~CMediaStreamSocket(void);

public:
	IVOSocket* GetMediaStreamSock() { return m_streamSock; }
	unsigned short GetMediaStreamSockLocalPort();

	int SetSockRecvBufSize(unsigned int bufSize);

private:
	IVOSocket*			m_streamSock;
	unsigned short		m_port;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__MEDIASTREAMSOCKET_H__