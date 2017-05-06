#include "TCPClientSocket_ssl.h"
#include "voLog.h"

#ifndef _WINDOWS
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>
#endif //_WINDOWS

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CTCPClientSocket_ssl::CTCPClientSocket_ssl(void)
{
	SSL_library_init();
	SSL_load_error_strings();
	m_pCTX = SSL_CTX_new( SSLv23_client_method() );

	if (!m_pCTX)
	{
		return;
	}

	/*output( "5" , __LINE__ );
	SSL_CTX_set_verify(m_pCTX, SSL_VERIFY_PEER, NULL);
	output( "6" , __LINE__ );

	if(!(SSL_CTX_load_verify_locations(m_pCTX, "/sdcard/ca.pem", NULL)))
	{
		output( "Can't read CA list!" , __LINE__ );
		return VO_FALSE;
	}*/

	m_pSSL = SSL_new(m_pCTX);
}

CTCPClientSocket_ssl::~CTCPClientSocket_ssl(void)
{
	if (m_pSSL)
		SSL_free(m_pSSL);

	if (m_pCTX)
		SSL_CTX_free(m_pCTX);
	
	m_pSSL = 0;
	m_pCTX = 0;
}

VO_S32 CTCPClientSocket_ssl::Close()
{
	if (m_pSSL)
		SSL_shutdown(m_pSSL);

	CTCPClientSocket::Close();
}

VO_S32 CTCPClientSocket_ssl::Read(VO_CHAR *toBuf, VO_S32 iMaxLen)
{
	VO_S32 iRet = SSL_read(m_pSSL, toBuf, iMaxLen);

	switch( SSL_get_error(m_pSSL, iRet) )
	{
	case SSL_ERROR_NONE:
	case SSL_ERROR_ZERO_RETURN:
		{
			return iRet;
		}
		break;

	default:
		{
			VOLOGE("!SSL_read");
			return E_TCP_Base;
		}
		break;
	}

	return iRet;
}

VO_S32 CTCPClientSocket_ssl::Write(const VO_CHAR *fromBuf, VO_S32 iLen)
{
	while ( !IsConnected() )
	{
		if (m_Socket)
		{
			if (m_pAICur->ai_next)
			{
				m_pAICur = m_pAICur->ai_next;
			}
			else
			{
				VOLOGE("!Cannot Connect!");
				
				return E_TCP_Base;
			}
		} 
		else
		{
			m_pAICur = m_pAIServ;
		}

		Close();

		if ( BeginConnect() )
			return E_TCP_Base;
	};

	VO_S32 ret = SSL_set_fd(m_pSSL, m_Socket);
	if (ret == 0)
	{
		VOLOGE("!SSL_set_fd");
		return E_TCP_Base;
	}

	ret = SSL_connect(m_pSSL);
	if (ret != 1)
	{
		ret = SSL_connect(m_pSSL);
		if (ret != 1)
		{
			VOLOGE("!SSL_connect");
			return E_TCP_Base;
		} 
	}

	if (0 == iLen)
		iLen = strlen(fromBuf);

	VO_S32 iWritten = 0;
	do 
	{
		VO_S32 iSent = SSL_write(m_pSSL, fromBuf + iWritten, iLen - iWritten);

		if (iSent <= 0)
		{
			VOLOGE("!SSL_write");
			return E_TCP_Base;
		}

		iWritten +=  iSent;
	} while (iWritten != iLen);

	return iLen;
}
