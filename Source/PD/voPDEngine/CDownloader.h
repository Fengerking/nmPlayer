#pragma once

#include "netdownbase.h"
#include "voPDPort.h"
#ifdef _HTC
#include "common_sdk_PD.h"
typedef PDStreamInitParam	SPDInitParam;
#else	//_HTC
#include "voStreaming.h"
typedef	VOPDInitParam		SPDInitParam;
#endif	//_HTC

typedef VONETDOWNRETURNCODE (VONDAPI * VONETDOWNINIT)();
typedef VONETDOWNRETURNCODE (VONDAPI * VONETDOWNCLEANUP)();
typedef VONETDOWNRETURNCODE (VONDAPI * VONETDOWNOPEN)(HVONETDOWN* ppNetDown, const char* szUrl, const char* szProxyName, VONETDOWNTYPE type);
typedef VONETDOWNRETURNCODE (VONDAPI * VONETDOWNSTART)(HVONETDOWN pNetDown, bool* pbCancel);
typedef VONETDOWNRETURNCODE (VONDAPI * VONETDOWNSTOP)(HVONETDOWN pNetDown);
typedef VONETDOWNRETURNCODE (VONDAPI * VONETDOWNCLOSE)(HVONETDOWN pNetDown);
typedef VONETDOWNRETURNCODE (VONDAPI * VONETDOWNSETPARAM)(HVONETDOWN pNetDown, long lID, long lValue);
typedef VONETDOWNRETURNCODE (VONDAPI * VONETDOWNGETPARAM)(HVONETDOWN pNetDown, long lID, long* plValue);

class CNetGet2;
class CDownloader:public IVOCommand,MEM_MANAGER
{
public:
	void	Execute();

public:
	CDownloader(CNetGet2* pNetGet=NULL);
	virtual ~CDownloader();

public:
	bool			SetUrl(LPCTSTR szUrl);

	bool			StartDownload(DWORD dwStart, DWORD dwLen, bool bUserThread);
	void			StopDownload();

	bool			SetParam(long lID, long lValue);
	bool			GetParam(long lID, long* plValue);

	bool			SetThreadPriority(int nPriority);

	SPDInitParam*	GetInitParam();
	VONETDOWNTYPE	GetDownType(TCHAR* url);
	void			SetDownLoadType(VONETDOWNTYPE type){m_type=type;}
	char*			GetContentType();
protected:
	DWORD			StartDownloadProcB();

	bool			StartDownloadB();
	void			StopDownloadB();

protected:
	bool			InitDll();
public:
	bool			Open();
	void			Close();

	bool			Start();
	void			Stop();

protected:
	CNetGet2*			m_pNetGet;

	VONETDOWNINIT		m_fInit;
	VONETDOWNCLEANUP	m_fCleanup;
	VONETDOWNOPEN		m_fOpen;
	VONETDOWNSTART		m_fStart;
	VONETDOWNSTOP		m_fStop;
	VONETDOWNCLOSE		m_fClose;
	VONETDOWNSETPARAM	m_fSetParam;
	VONETDOWNGETPARAM	m_fGetParam;

	HVONETDOWN			m_hNetDown;

	IVOThread*				m_hThreadStartDownload;

	TCHAR				m_szUrl[MAX_URL];

	DWORD				m_dwStart;
	DWORD				m_dwStop;
	int					m_nThreadPriority;
	VONETDOWNTYPE		m_type;
	char*				m_contentType;
};
class IPDSource
{
public:
	~IPDSource(){};
	virtual long	GetCurrentTrackType()=0;
	virtual	DWORD	GetPDHeaderRemainSize(bool bDF)=0;
	virtual	SPDInitParam*	GetInitParam()=0;
	virtual	void NotifyEvent(int id,void* param)=0;
};