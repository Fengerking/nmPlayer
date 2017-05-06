#include "HTTP_Transaction.h"
#include <string.h>
#include "voLog.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

CHTTP_Transaction::CHTTP_Transaction(CHTTP_Transaction::Delegate *pDelegate)
	: CThread("HTTP Transaction")
	, m_pTCPClient(NULL)
	, m_iPort(0)
	, m_bKeepAlive(VO_TRUE)
	, m_bChunked(VO_FALSE)
	, m_llSize(-1)
	, m_szRespose(NULL)
	, m_pDelegate(pDelegate)
{
	memset(m_szHost, 0, 1024);

	m_szRespose = new VO_CHAR[1024 * 16];
}

CHTTP_Transaction::CHTTP_Transaction()
	: CThread("HTTP Transaction")
	, m_pTCPClient(NULL)
	, m_iPort(0)
	, m_bKeepAlive(VO_TRUE)
	, m_bChunked(VO_FALSE)
	, m_llSize(-1)
	, m_pDelegate(NULL)
{
	memset(m_szHost, 0, 1024);

	m_szRespose = new VO_CHAR[1024 * 16];
}


CHTTP_Transaction::~CHTTP_Transaction(void)
{
	WaitingThreadExit();

	if (m_szRespose)
		delete []m_szRespose;

	if (m_pTCPClient)
		delete m_pTCPClient;
}

VO_S32 CHTTP_Transaction::Reset()
{
	if (m_pTCPClient)
		m_pTCPClient->Close();

	m_bKeepAlive = VO_TRUE;
	m_bChunked = VO_FALSE;
	m_llSize = -1;

	return 0;
}


VO_S32 CHTTP_Transaction::Start(RequestInfo *pRequestInfo)
{
	VO_CHAR szRequestMessage[1024 * 10] = {0};

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

	VO_CHAR *pOffset = strstr(m_szURL , "http://");
	if(pOffset)
		pOffset += strlen("http://");
	else
		pOffset = m_szURL;

	if (NULL == m_pTCPClient)
		m_pTCPClient = new CTCPClientSocket();

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
			iPort = 80;
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
			m_iPort = 80;
		
		VO_S32 iRet = m_pTCPClient->Open(m_szHost, m_iPort);
		if (iRet) {
			VOLOGE("!TCP Open %d", iRet);
			return iRet;
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

	VO_S32 iRet = m_pTCPClient->Write(szRequestMessage);
	if (iRet <= 0) {
		VOLOGE("!TCP Write %d", iRet);
		return iRet;
	}
	
	if (m_pDelegate)
		CThread::ThreadStart();

	return 0;
}

VO_VOID CHTTP_Transaction::ThreadMain()
{
/*
	VO_CHAR Buf[1024 * 16] = {0};
	VO_S32 iRet = getResposeInfo(Buf, CacheSize);
	if (iRet <= 18)
		VOLOGE("!Status-Line");
	//---
	if ( AnalyseRespose(Buf) )
		VOLOGE("!AnalyseRespose");
	
	do 
	{
		if (m_bChunked)
			iRet = doReadChunked(Buf, 1024 * 16);
		else
			iRet = doReadNormal(Buf, 1024 * 16);

		if (iRet <= 0) {
			VOLOGE("!TCP Read");
			break;
		}

		m_pDelegate->Some(Buf, iRet);
	} while (m_llSize);
*/
}

VO_S32 CHTTP_Transaction::getResposeInfo(VO_PCHAR *ppBuf)
{
	VO_S32 iRet = doGetResposeInfo(m_szRespose, 1024 * 16);
	if (iRet <= 18) {
		VOLOGE("!Status-Line");
		return iRet;
	}
	//---
	if ( AnalyseRespose(m_szRespose) ) {
		VOLOGE("!AnalyseRespose");
		return -1;
	}
	
	*ppBuf = m_szRespose;

	return 0;
}

VO_S32 CHTTP_Transaction::ReadData(VO_CHAR *pBuf, VO_S32 iMax)
{
	//if (m_bNeedRespose)
	//{
	//	VO_CHAR Buf[1024 * 16] = {0};
	//	VO_S32 iRet = getResposeInfo(Buf, CacheSize);
	//	if (iRet <= 18)
	//		PRINT_LOG("!Status-Line");
	//	//---
	//	if ( AnalyseRespose(Buf) )
	//		PRINT_LOG("!AnalyseRespose");
	//}

	VO_S32 iRet = 0;

	if (m_bChunked)
	{
		iRet = doReadChunked(pBuf, iMax);
	} 
	else
	{
		iRet = doReadNormal(pBuf, iMax);
	}

	return iRet;
}

VO_S32 CHTTP_Transaction::doReadChunked(VO_CHAR *Buf, VO_S32 iMax)
{
	VO_CHAR tmp[8] = {0};
	VO_S32 ioffset = 0;

	while (0 == m_llSize && ioffset < 8)
	{
		VO_S32 iRet = m_pTCPClient->Read(tmp + ioffset, 1);
		if (1 != iRet) {
			VOLOGE("!TCP Read %d", iRet);
			return iRet;
		}

		if( ioffset >= 1 && tmp[ioffset] == '\n' && tmp[ioffset - 1] == '\r' )
		{
			tmp[ioffset - 1] = '\0';

			unsigned int i = 0;
			sscanf(tmp, "%x", &i);//avoid warning
			m_llSize = i;

			break;
		}

		ioffset++;
	}

	VO_S32 iRet = 0;
	if (m_llSize)
	{
		VO_S32 iLen = m_llSize < iMax ? static_cast<VO_S32>(m_llSize) : iMax;

		iRet = m_pTCPClient->Read(Buf, iLen);
		if (iRet <= 0) {
			VOLOGE("!TCP Read %d", iRet);
			return iRet;
		} 
		else
		{
			m_llSize -= iRet;
		}
	}

	if (0 == m_llSize)
	{
		VO_S32 i = m_pTCPClient->Read(tmp, 1);
		if (1 != i) {
			VOLOGE("!TCP Read %d", i);
			return i;
		}

		i = m_pTCPClient->Read(tmp, 1);
		if (1 != i) {
			VOLOGE("!TCP Read %d", i);
			return i;
		}
	}

	return iRet;
}

VO_S32 CHTTP_Transaction::doGetResposeInfo(VO_CHAR *pBuf, VO_S32 iMaxLen)
{
	VO_CHAR *pOffset = pBuf;
	VO_S32 iTotal = 0;
	VO_S32 iUnusedLen = 0;

	VO_S32 iStatu = 0;
	do {
		if (!iUnusedLen) {
			iUnusedLen = m_pTCPClient->Read(pOffset, 4 - iStatu < iMaxLen - iTotal ? 4 - iStatu : iMaxLen - iTotal);
			if (iUnusedLen <= 0) {
				VOLOGE("!TCP Read %d", iUnusedLen);
				return iUnusedLen;
			}

			iTotal += iUnusedLen;
		}

		VO_CHAR c = *pOffset;
		pOffset++;
		iUnusedLen--;

		switch (iStatu)
		{
		case 0:
			{
				if (c != '\r')
					continue;
				else
				{
					iStatu = 1;

					break;
				}
			}
		case 1:
			{
				if (c != '\n')
				{
					iStatu = 0;
					continue;
				}
				else
				{
					iStatu = 2;
					break;
				}
			}
		case 2:
			{
				if (c != '\r')
				{
					iStatu = 0;
					continue;
				}
				else
				{
					iStatu = 3;
					break;
				}
			}
		case 3:
			{
				if (c != '\n')
					continue;
				else
				{
					//VOLOGI("READ: %s", pBuf);
					return iTotal;
				}
			}
		}
	}
	while(iMaxLen > iTotal);

	return -1;
}

VO_S32 CHTTP_Transaction::doReadNormal(VO_CHAR *Buf, VO_S32 iMax)
{
	if (0 == m_llSize)
		return 0;

	VO_S32 iRet = m_pTCPClient->Read(Buf, iMax);
	if (m_llSize > 0)
	{
		if (iRet > 0)
		{
			m_llSize -= iRet;
		}
		else
		{
			VOLOGE("!TCP Read");
		}
	}

	return iRet;
}

VO_S32 CHTTP_Transaction::AnalyseRespose(VO_CHAR *pBuf)
{
	VO_CHAR *p = strstr(pBuf, "HTTP/");
	if (!p) {
		VOLOGE("!Status-Line");
		return -1;
	}

	if (strncmp( p, "HTTP/1.1", strlen("HTTP/1.1") ) != 0)
		m_bKeepAlive = VO_FALSE;

	int iStatuCode = 0;
	p = p + strlen("HTTP/1.1") + 1;
	while (' ' == *p)
		p++;

	sscanf(p, "%d", &iStatuCode);
	if (iStatuCode / 100 == 3 || iStatuCode == 201)
	{
		//VO_CHAR *p1 = strstr(p, "Location:");
	}
	else if (iStatuCode / 100 != 2) {
		VOLOGW("!StatuCode %d", iStatuCode);//
		return -1;
	}
	//-----
	p = strstr(p, "\r\n") + 2;

	if ( strstr(p, "Transfer-Encoding: chunked" ) )
	{
		m_bChunked = VO_TRUE;//
		m_llSize = 0;
	}
	else
	{
		m_bChunked = VO_FALSE;

		VO_CHAR *p1 = strstr(p , "Content-Length: ");

		if (p1)
		{
			p1 += strlen("Content-Length: ");

			while (*p1 == ' ')
				p1++;

			if (p1)
				sscanf(p1 , "%lld\r\n" , &m_llSize);
		}
	}

	if ( strstr(p, "Connection: Keep-Alive" ) )
		m_bKeepAlive = VO_TRUE;//
	else
		m_bKeepAlive = VO_FALSE;

	return 0;
}