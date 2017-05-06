#include "HTTPs_Transaction.h"
#include "TCPClientSocket_ssl.h"
#include "voLog.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

CHTTPs_Transaction::CHTTPs_Transaction(void)
{
	m_pTCPClient = new CTCPClientSocket_ssl();
}

CHTTPs_Transaction::~CHTTPs_Transaction(void)
{
}

VO_S32 CHTTPs_Transaction::Start(RequestInfo *pRequestInfo)
{
	VO_CHAR szRequestMessage[1024 * 10];

	switch (pRequestInfo->method)
	{
	case HTTP_POST:
		{
			strcpy(szRequestMessage, "POST ");
		}
		break;

	case HTTP_OPTIONS:
		{
			strcpy(szRequestMessage, "OPTIONS *");
		}
		break;

	default:
		{
			strcpy(szRequestMessage, "GET ");
		}
		break;
	}
	//---
	VO_S32 iLen = strlen(pRequestInfo->szURL);
	VO_S32 iBegin = 0, iEnd = iLen;
	while (iBegin < iLen && pRequestInfo->szURL[iBegin] <= ' ')
		iBegin++;
	while(iEnd > 0 && pRequestInfo->szURL[iEnd - 1] <= ' ')
		iEnd--;

	memset(m_szURL, 0, 1024);
	strncpy(m_szURL, pRequestInfo->szURL + iBegin, iEnd - iBegin);

	VO_CHAR *pOffset = strstr(m_szURL , "https://");
	if (pOffset)
		pOffset += strlen("https://");
	else
		pOffset = m_szURL;

	VO_CHAR szHost[1024] = {0};
	int iPort = 0;
	VO_CHAR *p = strchr(pOffset, '/');
	if (p)
	{
		strncpy(szHost, pOffset, p - pOffset);
		memset(m_szPath, 0, 1024);
		strcpy(m_szPath, p);

		p = strchr(szHost, ':');
		if (p)
		{
			*p = '\0';
			p++;

			sscanf(p, "%d", &iPort);
		}
		else
			iPort = 443;
	} 
	else
	{
		strcpy(szHost, pOffset);
	}

	if (0 != strcmp(m_szHost, szHost) || m_iPort !=iPort)
	{
		memset(m_szHost, 0, 1024);
		strcpy(m_szHost, szHost);

		if (iPort)
			m_iPort = iPort;
		else
			m_iPort = 443;

		VO_S32 iRet = m_pTCPClient->Open(m_szHost, m_iPort);
		if (iRet)
		{
			VOLOGE("!m_TCPClient.Open");
			return -1;
		}

		m_bKeepAlive = VO_TRUE;
		m_bChunked = VO_FALSE;
		m_llSize = -1;
	}

	if (m_szPath && HTTP_OPTIONS != pRequestInfo->method)
		strcat(szRequestMessage, m_szPath);

	strcat(szRequestMessage, " HTTP/1.1\r\nHost: ");

	strcat(szRequestMessage, m_szHost);

	if (m_bKeepAlive)
		strcat(szRequestMessage, "\r\nConnection: Keep-Alive\r\n");
	else
		strcat(szRequestMessage, "\r\n");

	if (pRequestInfo->szExtendHeaders)
		strcat(szRequestMessage, pRequestInfo->szExtendHeaders);

	strcat(szRequestMessage, "\r\n");

	if (pRequestInfo->szBody)
		strcat(szRequestMessage, pRequestInfo->szBody);

	if ( m_pTCPClient->Write(szRequestMessage) > 0)
	{
		szRequestMessage[1500] = '\0';
		//PRINT_LOG(LOG_LEVEL_CRITICAL, "SENT: %s", szRequestMessage);
	}
	else
	{
		return -1;
	}

	if (m_pDelegate)
		CThread::ThreadStart();

	return 0;
}
