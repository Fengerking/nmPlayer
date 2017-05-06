#pragma once
#include "httpcmdhdl.h"

class CSCHttpCmdHandle :
	public CHttpCmdHandle
{
	
public:
	CSCHttpCmdHandle();
	virtual ~CSCHttpCmdHandle();
	
public:
	virtual HTTPGETRETURNVALUE	CMD_GET(CHttpGet* pGet, const char* szHost, const char* szObject, const char* szUserAgent, const char* szRange, bool bUseProxy,int flag=0);

	virtual HTTPGETRETURNVALUE	RecvResponse(CHttpGet* pGet);
	virtual bool	ParseContentLength(unsigned int* _contentLength);
	virtual VONETDOWNTYPE	GetNetDownType(){return m_netDownType;};
};
