#include "voPDPort.h"
#include <stdlib.h>
#include "httpget.h"
#include "CThroughputer.h"
#include "voStreaming.h"
#include "UFileReader.h"
#include "voString.h"
#define VO_HTTP_GET_RETRY_COUNT				1
#define VO_HTTP_GET_RETRY_WAIT_TIME			3000

extern DUMPLOGFUNC		g_pfDumpLogFunc;
extern CThroughputer*	g_Throughputer;

void DumpLog_NetGet(LPCSTR pLogInfo)
{
	if(g_pfDumpLogFunc)
		g_pfDumpLogFunc(pLogInfo);
}

void DumpLog_NetGet_Error(LPCSTR pLogInfo, HTTPGETRETURNVALUE rc)
{
	if(g_pfDumpLogFunc)
	{
		char szTmp[128];
		switch(rc)
		{
		case HGRC_UNKNOWN_ERROR:
			sprintf(szTmp, "%s%s\r\n", pLogInfo, "HGRC_UNKNOWN_ERROR");
			break;

		case HGRC_NETWORK_CLOSE:
			sprintf(szTmp, "%s%s\r\n", pLogInfo, "HGRC_NETWORK_CLOSE");
			break;

		case HGRC_NULL_POINTER:
			sprintf(szTmp, "%s%s\r\n", pLogInfo, "HGRC_NULL_POINTER");
		    break;

		case HGRC_USER_ABORT:
			sprintf(szTmp, "%s%s\r\n", pLogInfo, "HGRC_USER_ABORT");
		    break;

		case HGRC_SOCKET_ERROR:
			sprintf(szTmp, "%s%s: %d\r\n", pLogInfo, "HGRC_SOCKET_ERROR", GetSocketError());
			break;

		case HGRC_TIMEOUT:
			sprintf(szTmp, "%s%s\r\n", pLogInfo, "HGRC_TIMEOUT");
			break;

		default:
		    break;
		}
		g_pfDumpLogFunc(szTmp);
	}
}

CAutoForbidDumpLog::CAutoForbidDumpLog()
	: m_pfDumpLogFunc(g_pfDumpLogFunc)
{
	g_pfDumpLogFunc = NULL;
}

CAutoForbidDumpLog::~CAutoForbidDumpLog()
{
	g_pfDumpLogFunc = m_pfDumpLogFunc;
}

CHttpGet::CHttpGet()
	: CNetGet()
	, m_pHttpCmdHdl(NULL)
	, m_pbCancel(NULL)
	, m_dwRecvBytesOnce(MIN_PACKET_BUF_SIZE)
#ifdef _CONTROL_THROUGHPUT
	, m_dwMaxRecvBytesPerSec(MAXDWORD)
#endif	//_CONTROL_THROUGHPUT
	, m_bRedirect(false)
	, m_nHttpProtocol(1)
{
	memset(m_host, 0, sizeof(m_host));
	memset(m_object, 0, sizeof(m_object));
	//memset(&m_backupAI,0,sizeof(struct addrinfo));
	CVOLog::CreateVOLog();
}

CHttpGet::~CHttpGet()
{
	
}

VONETDOWNRETURNCODE CHttpGet::Open(LPCTSTR _url, LPCTSTR _proxy, VONETDOWNTYPE type)
{
	m_bRedirect = false;
	voLog(LL_DEBUG,"PDSource.txt","CHttpGet::open:%s\n",_url);
	VONETDOWNRETURNCODE rc = CNetGet::Open(_url, _proxy, type);
	if(rc != VORC_NETDOWN_OK)
		return rc;

	if(!ParseUrl())
		return VORC_NETDOWN_INVALID_URL;

	if(!CreateSocket())
		return VORC_NETDOWN_OUTOF_MEMORY;

	switch(type)
	{
	case VO_NETDOWN_TYPE_SHOUT_CAST:
		m_pHttpCmdHdl = new(MEM_CHECK) CSCHttpCmdHandle;
		break;
	case VO_NETDOWN_TYPE_WMS:
		m_pHttpCmdHdl = new(MEM_CHECK) CWMSHttpCmdHandle;
		break;
	default:
		m_pHttpCmdHdl = new(MEM_CHECK) CHttpCmdHandle;
	    break;
	}
	
		
	if(m_pHttpCmdHdl == NULL)
		return VORC_NETDOWN_OUTOF_MEMORY;

	if(!m_pHttpCmdHdl->Init())
		return VORC_NETDOWN_UNKNOWN_ERROR;

	return VORC_NETDOWN_OK;
}

HTTPGETRETURNVALUE CHttpGet::StartB()
{
	if(m_pbCancel && *m_pbCancel)
		return HGRC_USER_ABORT;

	HTTPGETRETURNVALUE rc = HGRC_OK;
	rc = ReConnect();
	if(HGRC_OK != rc)
	{
		DumpLog_NetGet("[CHttpGet::StartB]Connect fail.\r\n");
		return rc;
	}
	voLog(LL_DEBUG,"PDSource.txt","CHttpGet:before Action Get\n");
	rc = Action_GET();
	if(HGRC_OK != rc)
		DumpLog_NetGet("[CHttpGet::StartB]Action_GET fail.\r\n");

	return rc; 
}

VONETDOWNRETURNCODE CHttpGet::Start(bool* pbCancel)
{
	m_pbCancel = pbCancel;

	DWORD dwRetryTime = 0;
	while(true)
	{
		if(g_pfDumpLogFunc)
		{
			char szTmp[128];
			sprintf(szTmp, "[CHttpGet::Start]try count: %d\r\n", dwRetryTime);
			DumpLog_NetGet(szTmp);
		}

STARB_MEET_HGRC_NOSUPPORT_SEEK:
		HTTPGETRETURNVALUE rc = StartB();
		if(HGRC_OK == rc)
			return CNetGet::Start(pbCancel);
		else if(HGRC_USER_ABORT == rc)
			return VORC_NETDOWN_USER_ABORT;
		else if(HGRC_TIMEOUT == rc)
			return VORC_NETDOWN_NETWORK_TIMEOUT;
		else if( HGRC_NOSUPPORT_SEEK == rc && m_bIsSupportSeek )
		{
			m_bSupportSeek = false;
			UFileReader::g_nFlag != VOFR_DISABLE_SEEK;
			goto STARB_MEET_HGRC_NOSUPPORT_SEEK;
		}
		else	//retry
		{
			if(dwRetryTime >= VO_HTTP_GET_RETRY_COUNT)
				return VORC_NETDOWN_CONNECT_NET_FAIL;
			else
			{
				for(int i = 0; i < VO_HTTP_GET_RETRY_WAIT_TIME / 50; i++)
				{
					if (UFileReader::g_bCancel)
					{
						return VORC_NETDOWN_USER_ABORT;
					}
					IVOThread::Sleep(50);
				}
				dwRetryTime++;
			}
		}
	}
}

VONETDOWNRETURNCODE CHttpGet::Stop()
{
	return CNetGet::Stop();
}

VONETDOWNRETURNCODE CHttpGet::Close()
{
	SAFE_DELETE(m_pHttpCmdHdl);
	return CNetGet::Close();
}

VONETDOWNRETURNCODE CHttpGet::SetParam(long id, long param)
{
	if(VOID_NETDOWN_RECV_BYTES_ONCE == id)
	{
		m_dwRecvBytesOnce = param;
		return VORC_NETDOWN_OK;
	}
	if(VOID_NETDOWN_HTTP_PROTOCOL == id)
	{
		m_nHttpProtocol = param;
		return VORC_NETDOWN_OK;
	}
#ifdef _CONTROL_THROUGHPUT
	if(0xFFFFFFFC == id)
	{
		m_dwMaxRecvBytesPerSec = param;
		if(g_pfDumpLogFunc)
		{
			char szTmp[128];
			sprintf(szTmp, "[CHttpGet::SetParam 0xFFFFFFFC]%d\r\n", m_dwMaxRecvBytesPerSec);
			DumpLog_NetGet(szTmp);
		}
		return VORC_NETDOWN_OK;
	}
#endif	//_CONTROL_THROUGHPUT
	return CNetGet::SetParam(id, param);
}

VONETDOWNRETURNCODE CHttpGet::GetParam(long id, long * _param)
{
	switch(id)
	{
	case VOID_NETDOWN_STATE:
		{
			return VORC_NETDOWN_OK;
		}
		break;

	case VOID_NETDOWN_REDIRECT_URL:
		{
			if(!m_bRedirect)
				return VORC_NETDOWN_UNKNOWN_ERROR;

			TCHAR* szUrl = (TCHAR*)_param;
#ifdef _UNICODE
#ifdef _WIN32_WCE
			MultiByteToWideChar(CP_ACP, 0, m_url, -1, szUrl, sizeof(m_url) * sizeof(TCHAR));
#else//_WIN32_WCE
			mbtowc(szUrl,m_url,sizeof(m_url) * sizeof(TCHAR));
#endif//_WIN32_WCE
#else// _UNICODE
			_tcscpy(szUrl, m_url);
#endif //_UNICODE
			return VORC_NETDOWN_OK;
		}
		break;
	case VOID_NETDOWN_DOWNLOAD_TYPE:
		*_param=m_pHttpCmdHdl->GetNetDownType();
		return VORC_NETDOWN_OK;
	case VOID_NETDOWN_CONTENT_TYPE:
		*_param=(long)(m_pHttpCmdHdl->GetContentType());
	default:
		break;
	}

	return CNetGet::GetParam(id, _param);
}

bool CHttpGet::ParseUrl()
{
	if(strlen(m_proxy) > 0)
	{
		if(g_pfDumpLogFunc)
		{
			char szTmp[256];
			sprintf(szTmp, "[CHttpGet::proxy:ParseUrl]%s\r\n", m_proxy);
			DumpLog_NetGet(szTmp);
		}

		memset(m_host, 0, sizeof(m_host));
		strcpy(m_host, m_proxy);

		char * colon = strchr(m_host, ':');
		if(colon != NULL)
		{
			*colon++ = '\0';
			if(sscanf(colon, "%hu", &m_port) != 1)
				return false;
		}
		else
			return false;
	}
	else
	{
		char * p = m_url + 7;
		char * p1 = strchr(p, '/');
		//http://205.188.215.228:8016 is also valid
		if(p1)
		{
			memset(m_object, 0, sizeof(m_object));
			strcpy(m_object, p1);
		}
		else
		{
			strcpy(m_object,"/");
		}
		memset(m_host, 0, sizeof(m_host));
		if(sscanf(p, "%[^/]", m_host) != 1)
			return false;

		

		m_port = 80;
		char * colon = strchr(m_host, ':');
		if(colon != NULL)
		{
			*colon++ = '\0';
			if(sscanf(colon, "%hu", &m_port) != 1)
				return false;
		}

#ifdef _WIN32
		if( stricmp( m_host , "localhost"  ) == 0 )
		{
			strcpy( m_host , "127.0.0.1" );
		}
#else
		if( strncasecmp( m_host , "localhost" , strlen( "localhost" ) ) == 0 )
		{
			strcpy( m_host , "127.0.0.1" );
		}
#endif
	}

	return true;
}

bool CHttpGet::CreateSocket()
{
	// create IPv4 or IPv6 socket
	if(m_pCmdSocket)
		return true;

	CREATE_SOCKET(m_pCmdSocket,VST_SOCK_STREAM,VPT_TCP,VAF_INET4);// = VOCPFactory::CreateOneSocket(VST_SOCK_STREAM,VPT_TCP,VAF_INET4);
	return (NULL != m_pCmdSocket);
}

HTTPGETRETURNVALUE CHttpGet::Action_GET()
{
	char szRange[256];
	if(MAXDWORD == m_dwEndPos)
		sprintf(szRange, "bytes=%d-", m_dwStartPos);
	else
		sprintf(szRange, "bytes=%d-%d", m_dwStartPos, m_dwEndPos);
	int flag=0;
	int tryCount=0;
RETRY_AGAIN:
	if(strlen(m_proxy) > 0)
	{
		char szHost[MAX_PATH];

		char* p = m_url + 7;
		char* p1 = strchr(p, '/');
		if(!p1)
			return HGRC_UNKNOWN_ERROR;

		memset(szHost, 0, sizeof(szHost));
		if(sscanf(p, "%[^/]", szHost) != 1)
			return HGRC_UNKNOWN_ERROR;

		char* colon = strchr(szHost, ':');
		if(colon)
			*colon++ = '\0';

		if( !m_bSupportSeek )
		{
			flag = VOHTTP_REQ_NO_RANGE;
			UFileReader::g_nFlag|=VOFR_DISABLE_SEEK;
		}

		HTTPGETRETURNVALUE rc = m_pHttpCmdHdl->CMD_GET(this, szHost, m_url, m_szUserAgent, szRange, true,flag);
		if(HGRC_OK != rc)
		{
			//DumpLog_NetGet_Error("[CHttpGet::Action_GET]", rc);
			voLog(LL_DEBUG,"PDSource.txt","[CHttpGet::Action_GET] rc=%X,err=%d\n", rc,GetSocketError());
			return rc;
		}
	}
	else
	{
		char szHost[MAX_PATH];

		char* p = m_url + 7;
		char* p1 = strchr(p, '/');
		if(!p1)
			return HGRC_UNKNOWN_ERROR;

		memset(szHost, 0, sizeof(szHost));
		if(sscanf(p, "%[^/]", szHost) != 1)
			return HGRC_UNKNOWN_ERROR;

		char* colon = strchr(szHost, ':');
		if(colon)
			*colon++ = '\0';

		if( !m_bSupportSeek )
		{
			flag = VOHTTP_REQ_NO_RANGE;
			UFileReader::g_nFlag|=VOFR_DISABLE_SEEK;
		}

		HTTPGETRETURNVALUE rc = m_pHttpCmdHdl->CMD_GET(this, szHost, m_object, m_szUserAgent, szRange, false,flag);
		if(HGRC_OK != rc)
		{
			DumpLog_NetGet_Error("[CHttpGet::Action_GET]", rc);
			return rc;
		}
	}

	unsigned int statusCode = 200;
	if(!m_pHttpCmdHdl->ParseStatusCode(&statusCode))
	{
		DumpLog_NetGet("[CHttpGet::Action_GET]m_pHttpCmdHdl->ParseStatusCode fail.\r\n");
		return HGRC_UNKNOWN_ERROR;
	}
	if (statusCode==403&&tryCount==0)//forbiden
	{
		tryCount++;
		DumpLog_NetGet("[CHttpGet::Action_GET] 403 forbidden, try to request it without range.\r\n");
		if(HGRC_OK != ReConnect())
		{
			DumpLog_NetGet("[CHttpGet::Action_GET]ReConnect fail.\r\n");
			return HGRC_NETWORK_CLOSE;
		}
		flag=VOHTTP_REQ_NO_RANGE;
		goto RETRY_AGAIN;	
	}
	if(statusCode == 200 || statusCode == 206)
	{
		m_pHttpCmdHdl->ParseContentLength(&m_contentLength);

		if( m_contentLength == 0 && m_pHttpCmdHdl->ParseContentRange() )
			return HGRC_NOSUPPORT_SEEK;

		m_pHttpCmdHdl->ParseContentType();
	}
	else if(statusCode >= 300 && statusCode < 400)
	{
		return Action_REDIRECT();
	}
	else if( statusCode == 500 )
	{
		return HGRC_NOSUPPORT_SEEK;
	}
	else
	{
		if(g_pfDumpLogFunc)
		{
			char szTmp[128];
			sprintf(szTmp, "[statusCode error]%d\r\n", statusCode);
			DumpLog_NetGet(szTmp);
		}
		return HGRC_UNKNOWN_ERROR;
	}

	m_bSupportSeek = m_contentLength!=0&&statusCode == 206;
	if (m_contentLength==0)
	{
		m_contentLength = 0x7fffffff;
	}
	voLog(LL_DEBUG,"PDSource.txt","Action GetDone,length=%d\n",m_contentLength);
	
	if(flag==VOHTTP_REQ_NO_RANGE)
		UFileReader::g_nFlag|=VOFR_DISABLE_SEEK;
	return HGRC_OK;
}

HTTPGETRETURNVALUE CHttpGet::Action_REDIRECT()
{
	memset(m_url, 0, sizeof(m_url));
	if(!m_pHttpCmdHdl->ParseLocation(m_url, MAX_URL))
	{
		DumpLog_NetGet("[CHttpGet::Action_REDIRECT]m_pHttpCmdHdl->ParseLocation fail\r\n");
		return HGRC_UNKNOWN_ERROR;
	}

	m_bRedirect = true;

	if(!ParseUrl())
	{
		DumpLog_NetGet("[CHttpGet::Action_REDIRECT]ParseUrl fail.\r\n");
		return HGRC_UNKNOWN_ERROR;
	}

	if(strlen(m_proxy) <= 0)
	{
		//m_pCmdSocket->CreateStreamSocket();
		//m_pCmdSocket->EnableBlocking(false);
		m_pCmdSocket->SetBlocking(false);
		HTTPGETRETURNVALUE rc = Connect();
		if(HGRC_OK != rc)
		{
			DumpLog_NetGet("[CHttpGet::Action_REDIRECT]Connect fail.\r\n");
			return rc;
		}
	}

	return Action_GET();
}
bool CHttpGet::HandleASXBuf(char* asxBuf)
{
	m_pfCallback(VO_PD_EVENT_GET_ASX,(long)asxBuf,m_lCallbackUserData);
	return true;
}
DWORD CHttpGet::Getting()
{
	m_bCompleted = false;
	//this is for WMS PD,which needs to copy the head firstly
	int packeSize=m_dwRecvBytesOnce<MIN_PACKET_BUF_SIZE?MIN_PACKET_BUF_SIZE:m_dwRecvBytesOnce;
	if(!m_WriteThread.Create(this, packeSize, m_nThreadPriority))
	{
		DumpLog_NetGet("[CHttpGet::Getting]m_WriteThread.Create fail.\r\n");
		return 0;
	}
	
	m_downLength = 0;
	int nRecvBytes = 0;
	DWORD dwRecvBytesInBuffer = 0;
	bool bFirst = true;
	m_pbCancel = &m_stopGetting;

	DWORD dwStartTimeInThroughput = voGetCurrentTime();
	DWORD dwBytesInThroughput = 0;
	DWORD dwCurrTime = 0;

	PBYTE pBuffer = NULL;

	DumpLog_NetGet("CHttpGet::Getting thread start\r\n");
	voLog(LL_DEBUG,"PDSource.txt","CHttpGet::before Getting thread=%d\n",m_stopGetting);
	while(!m_stopGetting)
	{
		if (UFileReader::g_bCancel)
		{
			voLog(LL_DEBUG,"PDSource.txt","CHttpGet::Getting() exit by cancel\n");
			break;
		}
		if(bFirst)
		{
			if(m_pfCallback)
			{
				if(!m_pfCallback(NETDOWN_CREATE_FILE, m_contentLength, m_lCallbackUserData))
				{
					m_WriteThread.Destroy();
					voLog(LL_DEBUG,"PDSource.txt","CHttpGet:Set callback fail\n");
					break;
				}
			}
			bFirst = false;
		}
		int remained=0;
		if(dwRecvBytesInBuffer == 0)
		{
			if(!m_WriteThread.GetIdleBuffer(&pBuffer))
			{
				IVOThread::Sleep(1);
				continue;
			}
			remained=m_pHttpCmdHdl->GetRemainedDataInCMDBuf((char*)pBuffer);
			if(remained>0)//this is only available when in live wma mode
			{
				//m_WriteThread.SetBufferValid(true, pBuffer, remained);
				//continue;
				dwRecvBytesInBuffer=remained;
				m_WriteThread.SetBufferValid(true, pBuffer, dwRecvBytesInBuffer);
				dwBytesInThroughput += dwRecvBytesInBuffer;
				dwRecvBytesInBuffer=0;
				voLog(1,"WMSHead.txt","wma:head size=%d\n",remained);
				continue;
			}

		}
	

		HTTPGETRETURNVALUE rc = m_pHttpCmdHdl->Receive(this,pBuffer + dwRecvBytesInBuffer, (m_dwRecvBytesOnce - dwRecvBytesInBuffer), &nRecvBytes);
		
		if(HGRC_OK != rc)
		{
			m_WriteThread.Destroy();
			if(HGRC_USER_ABORT != rc)
			{
				if(m_pfCallback)
					m_pfCallback(NETDOWN_DISCONNECTED, 0, m_lCallbackUserData);
			}
			break;
		}
		//voLogData(LL_TRACE,"hbf.mp4",(char*)pBuffer + dwRecvBytesInBuffer,nRecvBytes);

		dwRecvBytesInBuffer += nRecvBytes;
		voLog(LL_TRACE,"netDataStatus.txt","rcv=%d,packSize=%d\n",nRecvBytes,dwRecvBytesInBuffer);

		if(dwRecvBytesInBuffer == m_dwRecvBytesOnce || m_downLength + dwRecvBytesInBuffer == m_contentLength)
		{
			m_WriteThread.SetBufferValid(true, pBuffer, dwRecvBytesInBuffer);

			dwBytesInThroughput += dwRecvBytesInBuffer;
			dwCurrTime = voGetCurrentTime();

#ifdef _CONTROL_THROUGHPUT
			if(LONGLONG(dwBytesInThroughput) * 1000 > LONGLONG(m_dwMaxRecvBytesPerSec) * (dwCurrTime - dwStartTimeInThroughput))
				Sleep(LONGLONG(dwBytesInThroughput) * 1000 / m_dwMaxRecvBytesPerSec - (dwCurrTime - dwStartTimeInThroughput));

			dwCurrTime = voGetCurrentTime();
#endif	//_CONTROL_THROUGHPUT
			if((!g_Throughputer->HasElem() && dwCurrTime - dwStartTimeInThroughput > 500) || 
				dwCurrTime - dwStartTimeInThroughput > 1000)
			{
				g_Throughputer->AddElem(dwBytesInThroughput, dwCurrTime - dwStartTimeInThroughput);
				dwStartTimeInThroughput = dwCurrTime;
				dwBytesInThroughput = 0;
			}
			m_downLength += dwRecvBytesInBuffer;
			dwRecvBytesInBuffer = 0;
		}

		if(m_downLength == m_contentLength&&m_contentLength!=LIVE_SIZE)	//download finish
		{
			m_WriteThread.Destroy();
			m_bCompleted = true;
			if(m_pfCallback)
				m_pfCallback(NETDOWN_COMPLETED, 0, m_lCallbackUserData);

			DumpLog_NetGet("CHttpGet::Getting thread complete\r\n");
			return 0;
		}
	}

	m_WriteThread.Destroy();
	//must close the socket, so the socket will not receive data from network!!
	//m_pCmdSocket->CloseSocket();
	m_pCmdSocket->Close();
	DumpLog_NetGet("CHttpGet::Getting thread stop\r\n");
	return 0;
}
HTTPGETRETURNVALUE		CHttpGet::ReConnect()
{
		//m_pCmdSocket->CreateStreamSocket();
		//m_pCmdSocket->EnableBlocking(false);
		DELETE_SOCKET(m_pCmdSocket);
		m_pCmdSocket=NULL;
		CreateSocket();
		HTTPGETRETURNVALUE rc = HGRC_OK;
		voLog(LL_DEBUG,"PDSource.txt","CHttpGet:begin ReConnect\n");
		return rc=Connect();
}
HTTPGETRETURNVALUE CHttpGet::Connect()
{
	if(!m_pCmdSocket)
		return HGRC_NULL_POINTER;

	VOSocketAddress addrinfo={0};
	VOSocketAddress* m_rtspServerAddr=&addrinfo;

	DumpLog_NetGet("CHttpGet::Connect getaddrinfo start\r\n");
	DumpLog_NetGet(m_host);
	m_rtspServerAddr->family = VAF_UNSPEC;
	m_rtspServerAddr->port   = IVOSocket::HostToNetworkShort(m_port);
	m_rtspServerAddr->protocol = VPT_TCP;
	m_rtspServerAddr->sockType = VST_SOCK_STREAM;

	if( strstr( m_host , "0.0.0.0" ) )
	{
		UFileReader::g_nFlag |= VOFR_SPECIAL_LINUXSERVER;
	}

	int nRet = IVOSocket::GetPeerAddressByURL(m_host,strlen(m_host),m_rtspServerAddr);
	if(0 != nRet)
	{
		if(g_pfDumpLogFunc)
		{
			char szTmp[128];
			sprintf(szTmp, "[CHttpGet::Connect]getaddrinfo fail: %d,error=%d\r\n", nRet,IVOSocket::GetSockLastError());
			DumpLog_NetGet(szTmp);
		}
		return HGRC_UNKNOWN_ERROR;
	}
	DumpLog_NetGet("CHttpGet::Connect getaddrinfo end\r\n");

	if(m_pbCancel && *m_pbCancel)
	{
		DumpLog_NetGet("CHttpGet::Connect user abort\r\n");
		return HGRC_USER_ABORT;
	}
	int ret=0;
#define CON_TIME_OUT 200
	int timeout=15000;//TODO;
	do 
	{

		ret=m_pCmdSocket->Connect(*m_rtspServerAddr,CON_TIME_OUT);
	
		if(ret== SOCKET_ERROR) 
		{
			int err = GetSocketError();
			if(g_pfDumpLogFunc)
			{
				char szTmp[64];
				sprintf(szTmp, "[connect SOCKET_ERROR]%d\r\n", err);
				DumpLog_NetGet(szTmp);
			}
			break;
		}
		if(UFileReader::g_bCancel)//m_pbCancel && *m_pbCancel)
		{
			voLog(LL_DEBUG, "PDSource.txt", "user stops the socket\n");
			timeout=0;
			break;//return false;
		}
		timeout-=CON_TIME_OUT;
	} while(timeout>0&&ret==1);

	if(timeout<=0||ret== SOCKET_ERROR)//timeout
	{
		DELETE_SOCKET(m_pCmdSocket);
		m_pCmdSocket=NULL;
		voLog(LL_DEBUG, "PDSource.txt", "connect timeout=%d,error=%d\n",timeout,ret);
		return  HGRC_TIMEOUT;
	}
	return HGRC_OK ;
}

HTTPGETRETURNVALUE CHttpGet::Send(const void* pBuffer, int nToSend, int* pnSended)
{
	if(!m_pCmdSocket)
		return HGRC_NULL_POINTER;

	HTTPGETRETURNVALUE rc = CheckSendAvailable();
	if(HGRC_OK != rc)
	{
		DumpLog_NetGet_Error("[CHttpGet::Send]", rc);
		return rc;
	}
	
	int nRet = m_pCmdSocket->Send((char*)pBuffer, nToSend);
	if(SOCKET_ERROR == nRet)
	{
		int nErr = GetSocketError();
		if(g_pfDumpLogFunc)
		{
			char szTmp[64];
			sprintf(szTmp, "[send SOCKET_ERROR]%d\r\n", nErr);
			DumpLog_NetGet(szTmp);
		}

		return HGRC_SOCKET_ERROR;
	}
	else
	{
		if(pnSended)
			*pnSended = nRet;
		return HGRC_OK;
	}
}

HTTPGETRETURNVALUE CHttpGet::Receive(void* pBuffer, int nToRecv, int* pnRecved)
{
	if(!m_pCmdSocket)
		return HGRC_NULL_POINTER;

	HTTPGETRETURNVALUE rc = CheckRecvAvailable();
	if(HGRC_OK != rc)
	{
		DumpLog_NetGet_Error("[CHttpGet::Receive]", rc);
		return rc;
	}

	int nRet = m_pCmdSocket->Recv((char*)pBuffer, nToRecv);
	if(SOCKET_ERROR == nRet)
	{
		int nErr = GetSocketError();
		if(g_pfDumpLogFunc)
		{
			char szTmp[64];
			sprintf(szTmp, "[recv SOCKET_ERROR]%d\r\n", nErr);
			DumpLog_NetGet(szTmp);
		}

		return HGRC_SOCKET_ERROR;
	}
	else if(0 == nRet)
	{
		DumpLog_NetGet("the network connection is gracefully closed\r\n");
		return HGRC_NETWORK_CLOSE;
	}
	else
	{
		if(pnRecved)
			*pnRecved = nRet;
		return HGRC_OK;
	}
}

HTTPGETRETURNVALUE CHttpGet::CheckConnectAvailable()
{
	return HGRC_OK;
}

HTTPGETRETURNVALUE CHttpGet::CheckSendAvailable()
{
	return HGRC_OK;
}

HTTPGETRETURNVALUE CHttpGet::CheckRecvAvailable()
{
	int timeout=m_dwRecvTimeout;
	int ret=0;
#define RECV_TIME_OUT 200
	do 
	{
		ret=m_pCmdSocket->Select(VSE_FD_READ,RECV_TIME_OUT);
		if (ret>0)
		{
			break;
		}
		if (UFileReader::g_bCancel||ret<0)
		{
			timeout=0;
			break;
		}

		timeout-=RECV_TIME_OUT;
	} while (timeout>0);
	if (timeout<=0)

	{
		voLog(LL_DEBUG,"PDSource.txt","CheckRecvAvailable:timeout=%d,ret=%d,cancel=%d\n",m_dwRecvTimeout,ret,UFileReader::g_bCancel);
		return HGRC_TIMEOUT;
	}
	return HGRC_OK;
}

bool CHttpGet::SinkFile(BYTE* pBuffer, DWORD dwLength)
{
	if(!m_pfCallback)
		return false;

	NDBuffer buf;
	buf.buffer = pBuffer;
	buf.size = dwLength;
	return m_pfCallback(NETDOWN_FILE_SINK, long(&buf), m_lCallbackUserData);
}