#ifndef __NETGET_H__
#define __NETGET_H__
#include "voPDPort.h"
#include "VOUtility.h"
#include "commonheader.h"
//#include "ipnetwork.h"
#include "netdownbase.h"

class CNetGet:public IVOCommand,MEM_MANAGER
{
public:
	CNetGet();
	virtual ~CNetGet();
	virtual void Execute();
public:
	virtual VONETDOWNRETURNCODE Open(LPCTSTR _url, LPCTSTR _proxy, VONETDOWNTYPE type);
	virtual VONETDOWNRETURNCODE Start(bool* pbCancel);
	virtual VONETDOWNRETURNCODE Stop();
	virtual VONETDOWNRETURNCODE Close();
	virtual VONETDOWNRETURNCODE SetParam(long id, long param);
	virtual VONETDOWNRETURNCODE GetParam(long id, long * _param);

	virtual bool	IsSupportSeek() {return m_bSupportSeek;}

protected:
	virtual bool ParseUrl();
	virtual bool CreateSocket();

	bool StartGetting();
	void StopGetting();

	static DWORD GettingEntry(void * pParam);
	virtual DWORD Getting() = 0;

protected:
	char		m_url[MAX_URL];
	IVOSocket*	m_pCmdSocket;

	NETDOWNCALLBACK m_pfCallback;
	long		m_lCallbackUserData;

	unsigned int m_contentLength;
	unsigned int m_downLength;

	//<MS>
	DWORD		m_dwConnectTimeout;
	DWORD		m_dwSendTimeout;
	DWORD		m_dwRecvTimeout;

	bool   m_stopGetting;
	IVOThread* m_hThread;

	bool	m_bSupportSeek;
	DWORD	m_dwStartPos;
	DWORD	m_dwEndPos;
	char	m_szUserAgent[MAX_PATH];
	char	m_proxy[MAX_PATH];			//"XXX.XXX.XXX.XXX:XXXX"
	bool	m_bCompleted;

	int		m_nThreadPriority;
};


#endif //__NETGET_H__