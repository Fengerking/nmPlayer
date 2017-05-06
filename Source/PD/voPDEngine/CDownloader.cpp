#include <voPDPort.h>
#include "VOUtility.h"
#include <stdio.h>
#include <tchar.h>

#include "CDownloader.h"
#include "CNetGet.h"
#include "UFileReader.h"

extern int g_nLog;

CDownloader::CDownloader(CNetGet2* pNetGet)
	: m_pNetGet(pNetGet)
	, m_hNetDown(NULL)
	, m_fInit(NULL)
	, m_fCleanup(NULL)
	, m_fOpen(NULL)
	, m_fStart(NULL)
	, m_fStop(NULL)
	, m_fClose(NULL)
	, m_fSetParam(NULL)
	, m_fGetParam(NULL)
	, m_hThreadStartDownload(NULL)
	, m_nThreadPriority(PRIO_NORMAL)
	, m_type(VO_NETDOWN_TYPE_NORMAL)
	,m_contentType(NULL)
{
	memset(m_szUrl, 0, sizeof(m_szUrl));
}

CDownloader::~CDownloader()
{
	StopDownload();
	if(m_fCleanup)
		m_fCleanup();
	if(m_contentType)
	{
		VO_SAFE_DELETE(m_contentType);
		m_contentType=NULL;
	}
}
char* CDownloader::GetContentType()
{
		return m_contentType;
}
VONETDOWNTYPE	CDownloader::GetDownType(TCHAR* url)
{
	do 
	{
		SetUrl(url);
		//try shout cast first
		SetDownLoadType(VO_NETDOWN_TYPE_SHOUT_CAST);
		//open
		if(_tcslen(m_szUrl) <= 0)
			break;

		if(!InitDll())
			break;
		char temp[256]={0};
		voLog(LL_DEBUG,"PDSource.txt","before GetDownType\n");
		if(VORC_NETDOWN_OK != m_fOpen(&m_hNetDown, (const char*)m_szUrl, (const char*)temp, m_type))
		{
			voLog(LL_DEBUG,"PDSource.txt","m_fOpen faile\n");
			break;
		}
		if(VORC_NETDOWN_OK != m_fStart(m_hNetDown, &UFileReader::g_bCancel))
		{
			voLog(LL_DEBUG,"PDSource.txt","m_fStart faile\n");
			break;
		}
		long type=0;
		GetParam(VOID_NETDOWN_DOWNLOAD_TYPE,(long*)&type);
		GetParam(VOID_NETDOWN_REDIRECT_URL,(long*)url);
		if(1)//type==VO_NETDOWN_TYPE_SHOUT_CAST)
		{
			char* conType=0;
			GetParam(VOID_NETDOWN_CONTENT_TYPE,(long*)&conType);
			m_contentType=(char*)allocate(strlen(conType)+1,MEM_CHECK);
			strcpy(m_contentType,(char*)conType);
		}
		
		Stop();
		Close();
		voLog(LL_DEBUG,"PDSource.txt","GetDownType end2,type=%d\n",type);
		return (VONETDOWNTYPE)type;
	} while(0);
	return VO_NETDOWN_TYPE_NORMAL;
}
bool CDownloader::Open()
{
	if(_tcslen(m_szUrl) <= 0)
		return false;

	if(!InitDll())
		return false;

	SPDInitParam* pInitParam = GetInitParam();
	if(VORC_NETDOWN_OK != m_fOpen(&m_hNetDown, (const char*)m_szUrl, (const char*)pInitParam->mProxyName, m_type))
	{
#ifdef _HTC
		m_pNetGet->NotifyEvent(HS_EVENT_CONNECT_FAIL, E_INVALID_URL);
#else	//_HTC
		m_pNetGet->NotifyEvent(VO_EVENT_CONNECT_FAIL, NULL);
#endif	//_HTC
		return false;
	}

	SetParam(VOID_NETDOWN_CONNECT_TIMEOUT, pInitParam->nHTTPDataTimeOut);
	SetParam(VOID_NETDOWN_SEND_TIMEOUT, pInitParam->nHTTPDataTimeOut);
	SetParam(VOID_NETDOWN_RECV_TIMEOUT, pInitParam->nHTTPDataTimeOut);

	SetParam(VOID_NETDOWN_USER_AGENT, (long)pInitParam->mUserAgent);
	SetParam(VOID_NETDOWN_RECV_BYTES_ONCE, pInitParam->nPacketLength * 1024);
	SetParam(VOID_NETDOWN_THREAD_PRIORITY, m_nThreadPriority);
	SetParam(VOID_NETDOWN_HTTP_PROTOCOL, pInitParam->nHttpProtocol);

	return true;
}

void CDownloader::Close()
{
	if(m_hNetDown)
	{
		m_fClose(m_hNetDown);
		m_hNetDown = NULL;
	}
}

bool CDownloader::Start()
{
	if(!m_hNetDown)
		return false;

	if(VORC_NETDOWN_OK != m_fStart(m_hNetDown, &UFileReader::g_bCancel))
	{
		DumpLog("HS_EVENT_CONNECT_FAIL E_CONNECTION_RESET\r\n");
#ifdef _HTC
		m_pNetGet->NotifyEvent(HS_EVENT_CONNECT_FAIL, E_CONNECTION_RESET);
#else	//_HTC
		m_pNetGet->NotifyEvent(VO_EVENT_CONNECT_FAIL, NULL);
#endif	//_HTC
		return false;
	}

	GetParam(VOID_NETDOWN_REDIRECT_URL, (long*)m_szUrl);

	return true;
}

void CDownloader::Stop()
{
	if(m_hNetDown)
		m_fStop(m_hNetDown);
}

bool CDownloader::SetParam(long lID, long lValue)
{
	if(!m_hNetDown)
		return false;

	return (VORC_NETDOWN_OK == m_fSetParam(m_hNetDown, lID, lValue));
}

bool CDownloader::GetParam(long lID, long* plValue)
{
	if(!m_fGetParam)
		return false;

	if(VOID_NETDOWN_BYTES_PER_SEC == lID)
		return (VORC_NETDOWN_OK == m_fGetParam(NULL, lID, plValue));

	if(!m_hNetDown)
		return false;

	return (VORC_NETDOWN_OK == m_fGetParam(m_hNetDown, lID, plValue));
}

bool CDownloader::InitDll()
{
	if(m_fInit && m_fCleanup && m_fOpen && m_fStart && m_fStop && m_fClose && m_fSetParam && m_fGetParam)
		return true;

	m_fInit = voNetDownInit;
	m_fCleanup = voNetDownCleanup;
	m_fOpen = voNetDownOpen;
	m_fStart = voNetDownStart;
	m_fStop = voNetDownStop;
	m_fClose = voNetDownClose;
	m_fSetParam = voNetDownSetParameter;
	m_fGetParam = voNetDownGetParameter;

	if(m_fInit && m_fCleanup && m_fOpen && m_fStart && m_fStop && m_fClose && m_fSetParam && m_fGetParam)
	{
		if(VORC_NETDOWN_OK != m_fInit())
			DumpLog("InitWinsock fail!!\r\n");

		if(g_nLog)
			m_fSetParam(NULL, VOID_NETDOWN_DUMP_LOG_FUNC, (long)DumpLog);
		return true;
	}

	return false;
}

void CDownloader::Execute()
{
	StartDownloadProcB();
}

DWORD CDownloader::StartDownloadProcB()
{
	StartDownloadB();
	return 0;
}

bool CDownloader::StartDownload(DWORD dwStart /* = 0 */, DWORD dwLen /* = MAXDWORD */, bool bUserThread)
{
	StopDownload();

	m_dwStart = dwStart;
	m_dwStop = (MAXDWORD == dwLen) ? MAXDWORD : (dwStart + dwLen);

	if(bUserThread)
	{
		CREATE_THREAD(m_hThreadStartDownload);// =VOCPFactory::CreateOneThread();
		if(m_hThreadStartDownload)
			m_hThreadStartDownload->Create(this);
		return (NULL != m_hThreadStartDownload);
	}
	else
		return StartDownloadB();
}

void CDownloader::StopDownload()
{
	//wait start download thread end
	if(m_hThreadStartDownload)
	{
		//WaitForSingleObject(m_hThreadStartDownload, INFINITE);
		//CloseHandle(m_hThreadStartDownload);
		m_hThreadStartDownload->TryJoin();
		//CloseHandle(m_hThread);
		DELETE_THREAD(m_hThreadStartDownload);
		m_hThreadStartDownload = NULL;
	}

	StopDownloadB();
}

bool CDownloader::StartDownloadB()
{
	if(!Open())
		return false;
	if (UFileReader::g_bCancel)
	{
		return false;
	}
	if(!SetParam(VOID_NETDOWN_EVENT_NOTIFY, long(CNetGet2::DownloadCallback)))
		return false;

	if(!SetParam(VOID_NETDOWN_EN_USER_DATA, long(m_pNetGet)))
		return false;

	if(!SetParam(VOID_NETDOWN_START_POS, m_dwStart))
		return false;

	if(!SetParam(VOID_NETDOWN_END_POS, m_dwStop))
		return false;

	if(!Start())
	{
		DumpLog("start downloading net error occur!\r\n");
		return false;
	}

	if(g_nLog)
	{
		char sz[256];
		sprintf(sz, "Base Start Download[start: %d, stop: %d]\r\n", m_dwStart, m_dwStop);
		DumpLog(sz);
	}

	m_pNetGet->OnStartDownloadOk();
	return true;
}

void CDownloader::StopDownloadB()
{
	Stop();
	Close();
}

bool CDownloader::SetUrl(LPCTSTR szUrl)
{
	_tcscpy(m_szUrl, szUrl);
	return true;
}

bool CDownloader::SetThreadPriority(int nPriority)
{
	m_nThreadPriority = nPriority;
	return true;
}

SPDInitParam* CDownloader::GetInitParam()
{
	return m_pNetGet ? m_pNetGet->GetInitParam() : NULL;
}