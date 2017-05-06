#include "VOUtility.h"
#include "cmdhdl.h"
#include "netget.h"
//#include "ipc.h"
void DumpLog_NetGet(LPCSTR);
using namespace VOUtility; 
CNetGet::CNetGet()
	: m_pCmdSocket(NULL)
	, m_contentLength(0)
	, m_stopGetting(true)
	, m_hThread(NULL)
	, m_downLength(0)
	, m_bSupportSeek(true)
	, m_dwStartPos(0)
	, m_dwEndPos(MAXDWORD)
	, m_bCompleted(false)
	, m_pfCallback(NULL)
	, m_lCallbackUserData(0)
	, m_dwConnectTimeout(10000)
	, m_dwRecvTimeout(10000)
	, m_dwSendTimeout(10000)
	, m_nThreadPriority(PRIO_NORMAL)
{
	strcpy(m_szUserAgent, "NSPlayer/10.0.0.3802");
	memset(m_proxy, 0, sizeof(m_proxy));
}

CNetGet::~CNetGet()
{
}

VONETDOWNRETURNCODE CNetGet::Open(LPCTSTR _url, LPCTSTR _proxy, VONETDOWNTYPE type)
{
	if(_url)
	{
#ifdef _UNICODE
		//WideCharToMultiByte(CP_ACP, 0, _url, -1, m_url, sizeof(m_url), NULL, NULL);
		wcstombs(m_url,_url,sizeof(m_url));
		voLog(LL_DEBUG,"PDSource.txt","url=%s\n",m_url);
#else
		strcpy(m_url, (char*)_url);
#endif //_UNICODE
	}
	else
		memset(m_url, 0, sizeof(m_url));

	if(_tcslen(_proxy)>5)
	{
#ifdef _UNICODE
		//WideCharToMultiByte(CP_ACP, 0, _proxy, -1, m_proxy, sizeof(m_proxy), NULL, NULL);
		wcstombs(m_proxy,_proxy,sizeof(m_proxy));
#else
		strcpy(m_proxy, _proxy);
#endif //_UNICODE
		voLog(LL_DEBUG,"PDSource.txt","proxyName=%s\n",m_proxy);
	}

	else
		memset(m_proxy, 0, sizeof(m_proxy));

	return VORC_NETDOWN_OK;
}

VONETDOWNRETURNCODE CNetGet::Start(bool* pbCancel)
{
	if(pbCancel && *pbCancel)
		return VORC_NETDOWN_USER_ABORT;

	StartGetting();
	return VORC_NETDOWN_OK;
}

VONETDOWNRETURNCODE CNetGet::Stop()
{
	StopGetting();
	return VORC_NETDOWN_OK;
}

VONETDOWNRETURNCODE CNetGet::Close()
{
	//SAFE_DELETE(m_pCmdSocket);
	DELETE_SOCKET(m_pCmdSocket);
	m_pCmdSocket=NULL;
	return VORC_NETDOWN_OK;
}

VONETDOWNRETURNCODE CNetGet::SetParam(long id, long param)
{
	switch(id)
	{
	case VOID_NETDOWN_EVENT_NOTIFY:
		{
			m_pfCallback = (NETDOWNCALLBACK)param;
			return VORC_NETDOWN_OK;
		}

	case VOID_NETDOWN_EN_USER_DATA:
		{
			m_lCallbackUserData = param;
			return VORC_NETDOWN_OK;
		}

	case VOID_NETDOWN_CONNECT_TIMEOUT:
		{
			m_dwConnectTimeout = param;
			return VORC_NETDOWN_OK;
		}

	case VOID_NETDOWN_SEND_TIMEOUT:
		{
			m_dwSendTimeout = param;
			return VORC_NETDOWN_OK;
		}

	case VOID_NETDOWN_USER_AGENT:
		{
			strcpy(m_szUserAgent, (char*)param);
			return VORC_NETDOWN_OK;
		}

	case VOID_NETDOWN_RECV_TIMEOUT:
		{
			m_dwRecvTimeout = param;
			return VORC_NETDOWN_OK;
		}

	case VOID_NETDOWN_START_POS:
		{
			m_dwStartPos = param;
			return VORC_NETDOWN_OK;
		}

	case VOID_NETDOWN_END_POS:
		{
			m_dwEndPos = param;
			return VORC_NETDOWN_OK;
		}

	case VOID_NETDOWN_THREAD_PRIORITY:
		{
			m_nThreadPriority = param;
			return VORC_NETDOWN_OK;
		}

	default:
		break;
	}

	return VORC_NETDOWN_NOT_IMPLEMENT;
}

VONETDOWNRETURNCODE CNetGet::GetParam(long id, long * _param)
{
	switch(id)
	{
	case VOID_NETDOWN_FILE_SIZE:
		{
			*((DWORD*)_param) = m_contentLength;
			return VORC_NETDOWN_OK;
		}

	case VOID_NETDOWN_DOWN_SIZE:
		{
			*((DWORD*)_param) = m_downLength;
			return VORC_NETDOWN_OK;
		}

	case VOID_NETDOWN_SUPPORT_SEEK:
		{
			*((bool*)_param) = m_bSupportSeek;
			return VORC_NETDOWN_OK;
		}

	default:
		break;
	}

	return VORC_NETDOWN_NOT_IMPLEMENT;
}

bool CNetGet::ParseUrl()
{
	return true;
}

bool CNetGet::CreateSocket()
{
	return true;
}

bool CNetGet::StartGetting()
{
    m_stopGetting = false;
	DWORD dwThreadID;
	CREATE_THREAD(m_hThread);// = VOCPFactory::CreateOneThread();//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GettingEntry, this, 0, &dwThreadID);
	if(m_hThread == NULL)
		return false;
	m_hThread->Create(this);
	//TODO_HBF
	//SetThreadPriority(m_hThread, m_nThreadPriority);
	m_hThread->SetPriority((VOThreadPriority)m_nThreadPriority);
	return true;
}
void CNetGet::Execute()
{
	Getting();
}
void CNetGet::StopGetting()
{
	m_stopGetting = true;
	if(!m_bCompleted)
	{
		if(m_hThread)
		{
			//WaitForSingleObject(m_hThread, INFINITE);
			m_hThread->TryJoin();
			//CloseHandle(m_hThread);
			DELETE_THREAD(m_hThread);
			m_hThread = NULL;
		}
	}
	m_bCompleted = false;
}

DWORD CNetGet::GettingEntry(void * pParam)
{
	CNetGet * pNetGet = (CNetGet *)pParam;
	return pNetGet->Getting();
}

