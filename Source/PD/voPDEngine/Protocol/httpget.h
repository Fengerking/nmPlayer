#ifndef __HTTPGET_H__
#define __HTTPGET_H__
#include "VOUtility.h"
#include "netget.h"
#include "CWriteThread.h"
#include "CSCHttpCmdHandle.h"
#include "CWMSHttpCmdHandle.h"

#define  LIVE_SIZE 0x7fffffff 
#define MIN_PACKET_BUF_SIZE 10*1024
using namespace VOUtility;
class CAutoForbidDumpLog
{
public:
	CAutoForbidDumpLog();
	virtual ~CAutoForbidDumpLog();

private:
	DUMPLOGFUNC		m_pfDumpLogFunc;
};

class CHttpGet : public CNetGet
{
	friend class CWriteThread;
public:
	CHttpGet();
	virtual ~CHttpGet();

public:
	virtual VONETDOWNRETURNCODE Open(LPCTSTR _url, LPCTSTR _proxy, VONETDOWNTYPE type);
	virtual VONETDOWNRETURNCODE Start(bool* pbCancel);
	virtual VONETDOWNRETURNCODE Stop();
	virtual VONETDOWNRETURNCODE Close();
	virtual VONETDOWNRETURNCODE SetParam(long id, long param);
	virtual VONETDOWNRETURNCODE GetParam(long id, long * _param);

	bool SinkFile(BYTE* pBuffer, DWORD dwLength);
protected:
	virtual bool	ParseUrl();
	virtual bool	CreateSocket();

	virtual DWORD	Getting();

	virtual HTTPGETRETURNVALUE		StartB();

	virtual	HTTPGETRETURNVALUE		Action_GET();
	virtual HTTPGETRETURNVALUE		Action_REDIRECT();

public:
	virtual HTTPGETRETURNVALUE		Connect();
	virtual HTTPGETRETURNVALUE		ReConnect();
	virtual HTTPGETRETURNVALUE		Send(const void* pBuffer, int nToSend, int* pnSended);
	virtual HTTPGETRETURNVALUE		Receive(void* pBuffer, int nToRecv, int* pnRecved);

protected:
	virtual HTTPGETRETURNVALUE		CheckConnectAvailable();
	virtual HTTPGETRETURNVALUE		CheckSendAvailable();
	virtual HTTPGETRETURNVALUE		CheckRecvAvailable();

public:
	int		m_nHttpProtocol;

protected:
	CHttpCmdHandle * m_pHttpCmdHdl;

protected:
	char	m_host[MAX_PATH];
	char	m_object[MAX_URL];
	unsigned short	m_port;

	bool*	m_pbCancel;

	DWORD	m_dwRecvBytesOnce;

	CWriteThread	m_WriteThread;
	//struct addrinfo m_backupAI;
	
#ifdef _CONTROL_THROUGHPUT
	DWORD	m_dwMaxRecvBytesPerSec;
#endif	//_CONTROL_THROUGHPUT

	bool	m_bRedirect;
	bool    m_bIsSupportSeek;
public:
	bool HandleASXBuf(char* asxBuf);
};


#endif //__HTTPGET_H__