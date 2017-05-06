#ifndef __HTTPCMDHDL_H__
#define __HTTPCMDHDL_H__

#include "cmdhdl.h"

#define MAX_CMD_BUFFER			2048
#define MAX_RESPONSE_BUFFER		20480

enum{
VOHTTP_REQ_NO_RANGE = 1000
};
typedef enum _HTTPGETRETURNVALUE
{
	HGRC_NOSUPPORT_SEEK		= -5,   /*NOT SUPPORT SEEK*/
	HGRC_UNKNOWN_ERROR		= -128,	/*!< exception */
	HGRC_NETWORK_CLOSE		= -4,	/*!< the network connection is gracefully closed */
	HGRC_NULL_POINTER		= -3,	/*!< null pointer */
	HGRC_USER_ABORT			= -2,	/*!< user abort */
	HGRC_SOCKET_ERROR 		= -1,	/*!< SOCKET_ERROR */
	HGRC_TIMEOUT			= 0,	/*!< time out */
	HGRC_OK 				= 1,	/*!< successful */
} HTTPGETRETURNVALUE;

class CHttpGet;
class CHttpCmdHandle : public CCmdHandle
{
public:
	CHttpCmdHandle();
	virtual ~CHttpCmdHandle();

public:
	virtual bool	Init();

public:
	virtual HTTPGETRETURNVALUE	CMD_GET(CHttpGet* pGet, const char* szHost, const char* szObject, const char* szUserAgent, const char* szRange, bool bUseProxy,int flag=0);

	virtual HTTPGETRETURNVALUE	SendRequest(CHttpGet* pGet);
	virtual HTTPGETRETURNVALUE	RecvResponse(CHttpGet* pGet);
	
	// parse server's response
	virtual bool	ParseStatusCode(unsigned int* _statusCode);
	virtual bool	ParseContentLength(unsigned int* _contentLength);
	virtual bool	ParseContentRange();
	virtual bool	ParseLocation(char* _location, size_t len);
	virtual VONETDOWNTYPE	GetNetDownType(){return VO_NETDOWN_TYPE_NORMAL;}
	virtual bool	ParseContentType();
	virtual	char*	GetContentType(){return m_contentType;}
	virtual int		GetRemainedDataInCMDBuf(char* p){return 0;}
	virtual HTTPGETRETURNVALUE		Receive(CHttpGet* pGet,void* pBuffer, int nToRecv, int* pnRecved);
protected:
	char*			SearchAttributeLine(const char* _attrName);
#define MAX_CONTENT_TYPE_LEN 64
	char			m_contentType[MAX_CONTENT_TYPE_LEN];
	VONETDOWNTYPE m_netDownType;
};

#endif //__HTTPCMDHDL_H__