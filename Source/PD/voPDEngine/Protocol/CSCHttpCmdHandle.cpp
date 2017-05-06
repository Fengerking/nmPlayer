#include "httpget.h"
#include "CSCHttpCmdHandle.h"

CSCHttpCmdHandle::CSCHttpCmdHandle()
	: CHttpCmdHandle()
{
	m_netDownType=VO_NETDOWN_TYPE_SHOUT_CAST;
}

CSCHttpCmdHandle::~CSCHttpCmdHandle()
{
}

HTTPGETRETURNVALUE CSCHttpCmdHandle::CMD_GET(CHttpGet* pGet, const char* szHost, const char* szObject, const char* szUserAgent, const char* szRange, bool bUseProxy,int flag)
{
	char * _cmdFmt = "GET %s HTTP/%s\r\n"
		"Host: %s\r\n"
		"Accept: */*\r\n"
		//"Icy-MetaData:1\r\n"
		"Pragma: no-cache\r\n"
		"Cache-Control: no-cache\r\n"
		"User-Agent: %s\r\n"
#if 0//test for 
		"X-Accept-Authentication: Negotiate, NTLM, Digest, Basic\r\n"
		"Pragma: no-cache,rate=1.000,stream-time=0,stream-offset=0:0,packet-num=4294967295,max-duration=0\r\n"
		"Pragma: version11-enabled=1\r\n"
		"Pragma: packet-pair-experiment=1\r\n"
		"Pragma: pipeline-experiment=1\r\n"
		"Pragma: xClientGUID={11223344-1122-1122-1122-AABBCCDDEEFF}\r\n"
#endif
		"Connection: close\r\n"
		"\r\n";

	sprintf(m_cmdBuffer, _cmdFmt, szObject, (0 == pGet->m_nHttpProtocol) ? "1.0" : "1.1", szHost, szUserAgent);

	HTTPGETRETURNVALUE rc = SendRequest(pGet);
	if(HGRC_OK != rc)
		return rc;

	rc=RecvResponse(pGet);
	//
	return rc;
}

HTTPGETRETURNVALUE CSCHttpCmdHandle::RecvResponse(CHttpGet* pGet)
{
//	CAutoForbidDumpLog afdl;
	
	memset(m_responseBuffer, 0, MAX_RESPONSE_BUFFER);

	int nReaded = 0;
	char* pPos = m_responseBuffer;
	char* p = NULL;
	while(true)
	{
		//receive byte one by one
		HTTPGETRETURNVALUE rc = pGet->Receive(pPos, 1, &nReaded);
		if(HGRC_OK != rc)
			return rc;

		if(nReaded != 1)
			return HGRC_UNKNOWN_ERROR;

		pPos++;
		p = pPos - 3;
		if(p < m_responseBuffer) 
			continue;

		if(*p == '\r' && *(p+1) == '\n' && *(p+2) == '\r')
		{
			*pPos = '\0';
			m_netDownType=VO_NETDOWN_TYPE_SHOUT_CAST;//default
			if (strstr(m_responseBuffer,"x-ms-")||strstr(m_responseBuffer,"x-mms-")||strstr(m_responseBuffer,"x-wms"))//
			{
				m_netDownType = VO_NETDOWN_TYPE_WMS;
				
			}
			else if(strstr(m_responseBuffer,"ICY 200 OK")==NULL&&strstr(m_responseBuffer,"audio/mpeg")==NULL)
			{
				if (strstr(m_responseBuffer,"video/x-flv"))
				{
					m_netDownType = VO_NETDOWN_TYPE_FLV;
				}
				else
				{
					m_netDownType = VO_NETDOWN_TYPE_NORMAL;
				}
				voLog(LL_TRACE,"http.txt","This is not ICY or WMS\n");
			}
			
			voLog(LL_TRACE,"http.txt",m_responseBuffer);
			break;
		}

		if((pPos - m_responseBuffer) >= MAX_RESPONSE_BUFFER)
			return HGRC_UNKNOWN_ERROR;
	}
	voLog(LL_DEBUG,"PDSource.txt","CHttpGet:Action Get\n");
	return HGRC_OK;
}

bool CSCHttpCmdHandle::ParseContentLength(unsigned int* _contentLength)
{
	*_contentLength = LIVE_SIZE;
	return true;
}