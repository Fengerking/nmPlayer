#ifndef __CTCPClientSocket_H__
#define __CTCPClientSocket_H__

#include "IClientSocket.h"

struct addrinfo;

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#define E_TCP_Base -0x8000

class CTCPClientSocket :
	public IClientSocket
{
public:
	CTCPClientSocket(void);
	~CTCPClientSocket(void);
	VO_S32 Open(const VO_CHAR *, VO_S32 );

	virtual VO_S32 Read(VO_CHAR *, VO_S32);
	virtual VO_S32 Write(const VO_CHAR *, VO_S32 iLenBuf = 0);

	virtual VO_S32 Close();

protected:
	VO_S32 GetAddrList(const VO_CHAR *, VO_S32);

	virtual VO_S32 BeginConnect();

	virtual VO_BOOL IsConnected() const;

	VO_S32 m_Socket;
	
	addrinfo *m_pAIServ;
	addrinfo *m_pAICur;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__CTCPClientSocket_H__