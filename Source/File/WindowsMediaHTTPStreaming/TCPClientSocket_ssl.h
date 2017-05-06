#ifndef __CTCPClientSocket_ssl_H__
#define __CTCPClientSocket_ssl_H__

#include "TCPClientSocket.h"

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CTCPClientSocket_ssl :
	public CTCPClientSocket
{
public:
	CTCPClientSocket_ssl(void);
	~CTCPClientSocket_ssl(void);

	virtual VO_S32 Read(VO_CHAR *, VO_S32);
	virtual VO_S32 Write(const VO_CHAR *, VO_S32 iLenBuf = 0);

	virtual VO_S32 Close();

private:
	SSL_CTX*	m_pCTX;
	SSL* m_pSSL;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__CTCPClientSocket_ssl_H__