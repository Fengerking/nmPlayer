#include "httpget.h"
#include "httpcmdhdl.h"
#include "UFileReader.h"

CHttpCmdHandle::CHttpCmdHandle()
	: CCmdHandle()
{
	memset(m_contentType,0,sizeof(m_contentType));
}

CHttpCmdHandle::~CHttpCmdHandle()
{
}

bool CHttpCmdHandle::Init()
{
	m_cmdBuffer = (char*)allocate(MAX_CMD_BUFFER,MEM_CHECK);// new char[MAX_CMD_BUFFER];
	if(!m_cmdBuffer)
		return false;
	memset(m_cmdBuffer, 0, MAX_CMD_BUFFER);

	m_responseBuffer = (char*)allocate(MAX_RESPONSE_BUFFER,MEM_CHECK);//new char[MAX_RESPONSE_BUFFER];
	if(!m_responseBuffer)
		return false;
	memset(m_responseBuffer, 0, MAX_RESPONSE_BUFFER);

	return true;
}
HTTPGETRETURNVALUE	CHttpCmdHandle::Receive(CHttpGet* pGet,void* pBuffer, int nToRecv, int* pnRecved)
{
	HTTPGETRETURNVALUE rc= pGet->Receive(pBuffer,nToRecv,pnRecved);
	if(*pnRecved>0)
	{
		//voLogData(1,"pd.mp3",(char*)pBuffer,*pnRecved);
	}
	return rc;
}
static char _cmdFmt2[8192];
HTTPGETRETURNVALUE CHttpCmdHandle::CMD_GET(CHttpGet* pGet, const char* szHost, const char* szObject, const char* szUserAgent, const char* szRange, bool bUseProxy,int flag)
{
	
	char * _cmdFmt = "GET %s HTTP/%s\r\n"
		"Host: %s\r\n"
		"Accept: */*\r\n";
	int protocolVersion=pGet->m_nHttpProtocol;
	strcpy(_cmdFmt2,_cmdFmt);
	
	if( UFileReader::g_nFlag & VOFR_SPECIAL_LINUXSERVER )
	{
		protocolVersion = 0;
	}
	else
	{
		if (bUseProxy)
		{
			sprintf(m_cmdBuffer,"User-Agent: %s\r\nProxy-Connection: Keep-Alive\r\n", szUserAgent);
			strcat(_cmdFmt2,m_cmdBuffer);
		}
		if (flag!=VOHTTP_REQ_NO_RANGE)
		{
			sprintf(m_cmdBuffer,"Range: %s\r\n",szRange);
			strcat(_cmdFmt2,m_cmdBuffer);
			strcat(_cmdFmt2,"Accept-Encoding: identity\r\n");
			strcat(_cmdFmt2,"Cache-Control: no-cache\r\n");
			strcat(_cmdFmt2,"Connection: close\r\n");
		}

		if(flag == VOHTTP_REQ_NO_RANGE)
		{
			strcat(_cmdFmt2,"Accept-Encoding: identity\r\n");
			strcat(_cmdFmt2,"Cache-Control: no-cache\r\n");
			strcat(_cmdFmt2,"Connection: close\r\n");
		}
	}
	
	strcat(_cmdFmt2,"\r\n");
	sprintf(m_cmdBuffer, _cmdFmt2, szObject, (0 == protocolVersion) ? "1.0" : "1.1", szHost);
	

	HTTPGETRETURNVALUE rc = SendRequest(pGet);
	if(HGRC_OK != rc)
		return rc;

	return RecvResponse(pGet);
}

HTTPGETRETURNVALUE CHttpCmdHandle::SendRequest(CHttpGet* pGet)
{
	if(!pGet)
		return HGRC_NULL_POINTER;

	int sendBytes = 0;
	int cmdLength = (int)strlen(m_cmdBuffer);
	HTTPGETRETURNVALUE rc = pGet->Send(m_cmdBuffer, cmdLength, &sendBytes);
	if(HGRC_OK != rc)
		return rc;
	voLog(LL_TRACE,"http.txt",m_cmdBuffer);
	if(sendBytes < cmdLength)
		return HGRC_UNKNOWN_ERROR;

	return HGRC_OK;
}

#define TEST_RESPONSE 0 
#if TEST_RESPONSE
static char* testResponse="HTTP/1.0 200 OK\r\n"
										"Content-Type: audio/mp4\r\n"
										"Test:this is for test\r\n"
										"\r\n";
#endif
HTTPGETRETURNVALUE CHttpCmdHandle::RecvResponse(CHttpGet* pGet)
{
//	CAutoForbidDumpLog afdl;

	memset(m_responseBuffer, 0, MAX_RESPONSE_BUFFER);

	int nReaded = 0;

	char* pPos = m_responseBuffer;
	char* p = NULL;

	while(true)
	{
		//receive byte one by one
		HTTPGETRETURNVALUE rc = HGRC_OK;

		rc = pGet->Receive(pPos, 1, &nReaded);

		if(HGRC_OK != rc)
			return rc;
		

		if(nReaded != 1)
			return HGRC_UNKNOWN_ERROR;

		pPos++;

		if( UFileReader::g_nFlag & VOFR_SPECIAL_LINUXSERVER )
		{
			p = pPos - 2;
			if(p < m_responseBuffer) 
				continue;
			m_netDownType=VO_NETDOWN_TYPE_NORMAL;
			if( ( *p == '\r' && *(p+1) == '\r' ) || ( *p == '\n'&& *(p+1) == '\n' ) ) 
			{
				*pPos = '\0';
				if(strstr(m_responseBuffer,"icy-")&&strstr(m_responseBuffer,"audio/mpeg"))
				{
					m_netDownType=VO_NETDOWN_TYPE_SHOUT_CAST;
				}
				voLog(LL_TRACE,"http.txt",m_responseBuffer);
				break;
			}
		}

		p = pPos - 4;
		if(p < m_responseBuffer) 
			continue;
		m_netDownType=VO_NETDOWN_TYPE_NORMAL;
		if(*p == '\r' && *(p+1) == '\n' && *(p+2) == '\r'&& *(p+3) == '\n') 
		{
			*pPos = '\0';
			if(strstr(m_responseBuffer,"icy-")&&strstr(m_responseBuffer,"audio/mpeg"))
			{
				m_netDownType=VO_NETDOWN_TYPE_SHOUT_CAST;
			}
			voLog(LL_TRACE,"http.txt",m_responseBuffer);
			break;
		}

		if((pPos - m_responseBuffer) >= MAX_RESPONSE_BUFFER)
			return HGRC_UNKNOWN_ERROR;
	}
#if TEST_RESPONSE
	strcpy(m_responseBuffer,testResponse);
	voLog(LL_TRACE,"http.txt",m_responseBuffer);
#endif//TEST_RESPONSE
	return HGRC_OK;
}

bool CHttpCmdHandle::ParseStatusCode(unsigned int * _statusCode)
{
	char * _statusLine = m_responseBuffer;
	char _httpVer[32];
	if(sscanf(_statusLine, "%[^ \r\n]%u", _httpVer, _statusCode) != 2)
		return false;

	return true;
}
bool CHttpCmdHandle::ParseContentType()
{
	char* _attrLine = SearchAttributeLine("content-type");
	if(!_attrLine)
		return false;
	_attrLine += 12;
	if(1 != sscanf(_attrLine, ": %[^\r\n]", m_contentType))
		return false;

	return true;
}
bool CHttpCmdHandle::ParseContentLength(unsigned int * _contentLength)
{
	char* _attrLine = SearchAttributeLine("Content-Length");
	//*_contentLength=LIVE_SIZE;
	//return true;

	if(!_attrLine)
	{
		if(m_netDownType==VO_NETDOWN_TYPE_SHOUT_CAST)
		{
			*_contentLength=LIVE_SIZE;
			return true;
		}
		return false;
	}
	_attrLine += 14;
	if(1 != sscanf(_attrLine, ": %u", _contentLength))
		return false;

	return true;
}

bool CHttpCmdHandle::ParseContentRange()
{
	char* _attrLine = SearchAttributeLine("Content-Range");

	if( _attrLine )
		return true;
	else
		return false;
}

bool CHttpCmdHandle::ParseLocation(char * _location, size_t len)
{
	char* _attrLine = SearchAttributeLine("Location");
	if(!_attrLine)
		return false;

	_attrLine += 8;
	if(1 != sscanf(_attrLine, ": %[^\r\n]", _location))
		return false;

	return true;
}

char * CHttpCmdHandle::SearchAttributeLine(const char * _attrName)
{
	size_t attrNameLen = strlen(_attrName);
	char * _line = m_responseBuffer;
	while(_line != NULL)
	{
		if(!_strnicmp(_line, _attrName, attrNameLen))
			return _line;

		_line = GetNextResponseLine(_line);
	}
	return NULL;
}
