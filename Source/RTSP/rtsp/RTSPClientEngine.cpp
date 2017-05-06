#include <memory.h>
#include <stdio.h>
#include <string.h>
#include "voLog.h"
#include "RTSPMediaStreamSyncEngine.h"
#include "utility.h"
#include "network.h"
#include "RTSPSession.h"
#include "RealTimeStreamingEngine.h"

#include "MediaStream.h"
#include "RTPParser.h"
#include "RTCPParser.h"
#include "TaskSchedulerEngine.h"
#include "RTSPClientEngine.h"
#include "MediaStreamSocket.h"
#include "vostream_digest_auth.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define ENABLE_COPY_SDPFILESTRING 0

char *strSpace(char *str)
{
	while(*str != 0x0d)
	{
		if(*str == 0)
			*str = 0x20;
		str++;
	}

	return NULL;
}
void CRTSPResponseHandler::SerHandlerParam(CRTSPClientEngine* engine,RR_ID id)
{
	m_engine = engine;
	m_id	 = id;
}
void CRTSPResponseHandler::HandleResponse(char* response)
{
#if SYNC_REQUEST
	if(m_nextTask>0)
	{
		CTaskSchedulerEngine::CreateTaskSchedulerEngine()->UpdateTask(m_nextTask,0);
		sprintf(CLog::formatString,"HandleResponse_UpdateTask_%d\n",m_nextTask);
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
		m_nextTask=0;
	}
#endif//
	
	switch(m_id)
	{
	case RR_ID_PLAY:
		m_engine->SetLastResponse(m_id);
		m_engine->HandlePlayResponse(response);
		break;
	case RR_ID_PAUSE:
		m_engine->SetLastResponse(m_id);
		m_engine->HandlePauseResponse(response);
		break;
	case RR_ID_KEEPALIVE:
		m_engine->HandleKeepAliveResponse(response);
		break;
	default:
	    break;
	}
}
#ifdef HTC_PLAYLIST
#include "PlayListInterface_Vo.h"
#else
typedef enum RTSPField {
	CUST_DESCRIBE= 0x01, 
	CUST_SETUP = 0x02, 
	CUST_PLAY = 0x04, 
	CUST_PLAYLIST_PLAY = 0x08,
	CUST_OPTION = 0x10,
	CUST_PAUSE = 0x20,
	CUST_ALL = 0xFF
} RTSP_METHOD ;
#endif //HTC_PLAYLIST
#include "RTSPSource.h"
CRTSPClientEngine::CRTSPClientEngine(void)
: m_URLofRequest(NULL)
, m_rtspServerPort(0)
, m_lastResponse(RR_ID_UNKNOWN)
, m_bSyncPlayResponse(false)
, m_taskToken(0)
, m_failConnectNum(0)
, m_bReconnect(false)
, m_rtspURL(NULL)
, m_streamingOverTCP(false)
, m_rtspCSeq(0)
, m_publicAttr(NULL)
, m_rtspSessionID(NULL)
, m_contentBase(NULL)
, m_serverTimeoutValue(60)//RFC2326 12.37 default value=60
, m_rtspSock(INVALID_SOCKET)
, m_rtspSock2(INVALID_SOCKET)
, m_rtspServerAddr(NULL)
, m_rtspServerAddrOverHttpTunnel(NULL)
, m_recvBuf(NULL)
, m_rtspCMDString(NULL)
, m_userAgentAttr(NULL)
, m_UAProfile(NULL)
, m_rtspSession(NULL)
, m_hConnectThread(NULL)
, m_streamLength(0)
, m_streamPos(0)
,m_errorCount(0)
,m_range(0.0)
,m_pauseTime(-1)
,m_playTime(-1)
,m_sdpFileString(NULL)
#ifdef HTC_PLAYLIST
, m_pICustomNotify(NULL)
, m_pRTSPSource(NULL)
#endif //HTC_PLAYLIST
, m_bIfUAgent(0)
, m_bIfBandWidth(false)
, m_BIfWapProfile(false)
{
	StartupSocket();
	m_rtspSession = new CRTSPSession(this);TRACE_NEW("rspce_m_rtspSession",m_rtspSession);
	m_responsHandler[RR_ID_PLAY].SerHandlerParam(this,RR_ID_PLAY);
	m_responsHandler[RR_ID_PAUSE].SerHandlerParam(this,RR_ID_PAUSE);
	m_responsHandler[RR_ID_KEEPALIVE].SerHandlerParam(this,RR_ID_KEEPALIVE);
	CMediaStream::m_streamChannelIDGenerator = 0;
#if SUPPORT_NPT_FEEDBACK
	memset(&m_rangeInfo,0,sizeof(RangeInfo));
#endif//SUPPORT_NPT_FEEDBACK

	m_digestAuth = NULL;
	memset(m_digestResp,0,33);
	memset(m_digestRealm,0,128);
	memset(m_digestNonce,0,128);

	memset(m_hostPath, 0, MAX_PATH);
	memset(m_fSessionCookie, 0, 33);
}

CRTSPClientEngine::~CRTSPClientEngine(void)
{
	Reset();
	CTaskSchedulerEngine::CreateTaskSchedulerEngine()->DestroyTaskSchedulerEngine();
	CleanupSocket();
	SAFE_DELETE_ARRAY(m_digestAuth);
	SAFE_DELETE_ARRAY(m_recvBuf);
	SAFE_DELETE_ARRAY(m_rtspCMDString);
#if ENABLE_COPY_SDPFILESTRING
	if(m_sdpFileString)
		SAFE_DELETE_ARRAY(m_sdpFileString);
#endif
	//SAFE_DELETE_ARRAY(m_rtspSessionID);
}
CMediaStream* CRTSPClientEngine::CreateOneMediaStream(CRTSPSession* session)
{
	CMediaStream*  stream= new CMediaStream(session);TRACE_NEW("CRTSPClientEngine::CreateOneMediaStream",stream);
	
	return stream;
}
bool CRTSPClientEngine::Init()
{
	m_errorCount = 0;
	m_failConnectNum=0;

	if(m_rtspServerAddr)//inited
		return true;

	if(m_rtspServerAddrOverHttpTunnel)//inited
		return true;

	do
	{
		if(m_rtspServerAddr == NULL)
		{
			m_rtspServerAddr = new struct sockaddr_storage;TRACE_NEW("rspce_m_rtspServerAddr",m_rtspServerAddr);
			if(m_rtspServerAddr == NULL)
				break;
		}

		if(m_rtspServerAddrOverHttpTunnel == NULL)
		{
			m_rtspServerAddrOverHttpTunnel = new struct sockaddr_storage;TRACE_NEW("rspce_m_rtspServerAddr",m_rtspServerAddrOverHttpTunnel);
			if(m_rtspServerAddrOverHttpTunnel == NULL)
				break;
		}

		if(m_recvBuf == NULL)
		{
			m_recvBuf = new char[RECVBUFSIZE];TRACE_NEW("rspce_m_recvBuf",m_recvBuf);
			if(m_recvBuf == NULL)
				break;
		}

		if(m_rtspCMDString == NULL)
		{
			m_rtspCMDString = new char[RECVBUFSIZE];TRACE_NEW("rspce_m_rtspCMDString",m_rtspCMDString);
			if(m_rtspCMDString == NULL)
				break;
		}

		if(CTaskSchedulerEngine::CreateTaskSchedulerEngine() == NULL)
			break;

		if(CRealTimeStreamingEngine::CreateRealTimeStreamingEngine() == NULL)
			break;
		
		if(CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine() == NULL)
			break;

		if(m_userAgentAttr == NULL)
		{
			char* userAgent=CUserOptions::UserOptions.m_sUserAgent;
			

			m_userAgentAttr = new char[strlen(userAgent)+1];TRACE_NEW("rspce_m_m_userAgentAttr",m_userAgentAttr);
			if(m_userAgentAttr == NULL)
				break;

			//char OEMInfo[128] = {0};
			strcpy(m_userAgentAttr, userAgent);
//			strcpy(m_userAgentAttr, "VisualOn RTP Streaming Media V1.0; Device=");
//			strcat(m_userAgentAttr, OEMInfo);
//			strcat(m_userAgentAttr, ";");
		}
		if(m_UAProfile==NULL)
		{
			char* uaProfile=CUserOptions::UserOptions.m_sUAProfile;
			int len=strlen(uaProfile)+1;
			m_UAProfile = new char[len];TRACE_NEW("rspce_m_UAProfile",m_UAProfile);//];TRACE_NEW("rspce_m_UAProfile",m_UAProfile);
			if(m_UAProfile == NULL)
				break;
			strcpy(m_UAProfile, uaProfile);
		}
		return true;

	}while(0);

	return false;
}

void CRTSPClientEngine::Reset()
{
	CTaskSchedulerEngine::CreateTaskSchedulerEngine()->UnscheduleAllTask();
	if(m_rtspSock!=INVALID_SOCKET)
	{
		VOCPFactory::DeleteOneSocket(m_rtspSock);TRACE_DELETE(m_rtspSock);
		m_rtspSock = INVALID_SOCKET;
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt","release rtspSock\n");
	}

	if(m_rtspSock2!=INVALID_SOCKET)
	{
		VOCPFactory::DeleteOneSocket(m_rtspSock2);TRACE_DELETE(m_rtspSock2);
		m_rtspSock2 = INVALID_SOCKET;
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt","release rtspSock2\n");
	}

	SAFE_DELETE(m_rtspServerAddr);
	SAFE_DELETE(m_rtspServerAddrOverHttpTunnel);
	SAFE_DELETE(m_rtspSession);
	SAFE_DELETE_ARRAY(m_rtspURL);
	SAFE_DELETE_ARRAY(m_publicAttr);
	SAFE_DELETE_ARRAY(m_rtspSessionID);
	SAFE_DELETE_ARRAY(m_contentBase);
	SAFE_DELETE_ARRAY(m_userAgentAttr);
	SAFE_DELETE_ARRAY(m_UAProfile);
	SAFE_DELETE(m_digestAuth);

	m_rtspCSeq = 0;
	m_streamLength = 0;
	m_streamPos = 0;
	m_taskToken = 0;
	CRealTimeStreamingEngine::DestroyRealTimeStreamingEngine();
	CRTSPMediaStreamSyncEngine::DestroyRTSPMediaStreamSyncEngine();
	CMediaStream::m_streamChannelIDGenerator = 0;
	//reset the handler
	for(int id=RR_ID_PLAY;id<RR_ID_NUMBER;id++)
	{
		m_responsHandler[id].Reset();
	}
}

void CRTSPClientEngine::AddRTSPFields(unsigned int nRTSPMethod)
{

	if(0)//!CUserOptions::UserOptions.m_bEnablePlaylistSupport)
		return;
	size_t cmdLen = strlen(m_rtspCMDString);
	if(cmdLen < 4)
		return;
	if(m_rtspCMDString[cmdLen-4] == '\r' &&
		m_rtspCMDString[cmdLen-3] == '\n' &&
		m_rtspCMDString[cmdLen-2] == '\r' &&
		m_rtspCMDString[cmdLen-1] == '\n')
	{
		m_rtspCMDString[cmdLen-4] = '\0';
	}

	list_T<RTSPFieldParam *>::iterator iter;
	for(iter=m_pRTSPSource->m_listRTSPFieldParam.begin(); iter!=m_pRTSPSource->m_listRTSPFieldParam.end(); ++iter)
	{
		RTSPFieldParam * pRTSPFieldParam = (RTSPFieldParam *)(*iter);
		if(pRTSPFieldParam->uMethods & nRTSPMethod)
		{
			strcat(m_rtspCMDString, pRTSPFieldParam->_field);
			int length=strlen(pRTSPFieldParam->_field);
			if(!(pRTSPFieldParam->_field[length-2] == '\r' &&
				pRTSPFieldParam->_field[length-1] == '\n'))
			{
				strcat(m_rtspCMDString, "\r\n");
			}
		}
	}
	//strcat(m_rtspCMDString, "\r\n");
}
#ifdef HTC_PLAYLIST
void CRTSPClientEngine::SetCustomNotifyInterface(ICustomNotifyInterface * pICustomNotify)
{
	m_pICustomNotify = pICustomNotify;
}


#endif //HTC_PLAYLIST
void CRTSPClientEngine::SetRTSPSource(CRTSPSourceEX * pRTSPSource)
{
	m_pRTSPSource = pRTSPSource;
}
bool CRTSPClientEngine::OptionsCmd(char * rtspURL)
{
	if(CUserOptions::UserOptions.m_URLType==UT_SDP2_LOCAL)
		return true;
	do
	{
		if(!Init())
			break;

		if(!ConnectToStreamingServer(rtspURL))
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "Option:Can not connect streaming server! \r\n");
			break;
		}
		strcpy(cmdFormatString,"OPTIONS %s RTSP/1.0\r\n"
								 "CSeq: %d\r\n"
								 "User-Agent: %s\r\n");
								 
		if((CUserOptions::UserOptions.digestIsSet) && (m_digestAuth))
		{
			char method[33] = {0};
			strcpy(method,"OPTIONS");

			m_digestAuth->SetMethod(method,strlen(method));
			m_digestAuth->GetResponse(m_digestResp, 32);
			
			sprintf(m_rtspCMDString, "Authorization: Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\"\r\n",
									CUserOptions::UserOptions.digestAuth.user,
									m_digestRealm,m_digestNonce,m_rtspURL,m_digestResp);
			strcat(cmdFormatString,m_rtspCMDString);
		}
		if(CUserOptions::UserOptions.m_bOpenUAProEachRequest&&strlen(m_UAProfile)>2)
		{
			sprintf(m_rtspCMDString,"x-wap-profile: %s\r\n",m_UAProfile);
			strcat(cmdFormatString,m_rtspCMDString);
		}

		sprintf(m_rtspCMDString, cmdFormatString, rtspURL, ++m_rtspCSeq, m_userAgentAttr);
		AddRTSPFields(CUST_OPTION);
		strcat(m_rtspCMDString,"\r\n");

		if(CUserOptions::UserOptions.m_isOverHttpTunnel)
		{
			char* FormatString = new char[MAX_TMPLEN];TRACE_NEW("FormatString",FormatString);

			VOLOGI("_____________OPTIONSCMD____________before encoder=%s", m_rtspCMDString);
			EncodeBase64(m_rtspCMDString, FormatString);
			VOLOGI("_____________OPTIONSCMD____________after encoder=%s", FormatString);

			if(!SendRTSPCommandToStreamingServer(FormatString))
				break;
			unsigned int responseBytes;
			if(!RecvResponseFromStreamingServerOverHttpTunnel(responseBytes,VOS_CMD_OPTION)) 
			{
				VOLOGI("__________RecvResponseFromStreamingServer____________OPTIONS____FAIL");
				break;
			}
		}
		else
		{
			if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
				break;
			unsigned int responseBytes;
			if(!RecvResponseFromStreamingServer(responseBytes,VOS_CMD_OPTION)) 
				break;
		}

		char * responseLine = m_recvBuf;
		char * nextResponseLine = GetNextLine(responseLine);
		unsigned int responseCode = 0;
		if(!ParseResponseCode(responseLine, responseCode))
			break;

		if(responseCode != 200)
		{
			CUserOptions::UserOptions.errorID = E_USER_ERROR_BASE-responseCode;
			break;
		}
		while(1) 
		{
			responseLine = nextResponseLine;
			if(responseLine == NULL)
				break;

			nextResponseLine = GetNextLine(responseLine);

			if(ParsePublicAttribute(responseLine))
				continue;
		}

		return true;

	}while(0);
	m_failConnectNum+=5;
	NotifyEvent(VO_EVENT_OPTION_FAIL,CUserOptions::UserOptions.errorID);
	CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt", "@@@@Parse option fail!!\n");
	return false;
}
int CRTSPClientEngine::CleanURL(char* str2)
{
	int i;
	int len = strlen(str2);
	//char str2[12];
	//memcpy(str2,str,len);
	while(str2[len-1]=='\n'||str2[len-1]=='\r'||str2[len-1]==0x20)
	{
		str2[len-1]=0;
		len--;
	}
#if 1//damn China Telecom
	char* frm=strstr(str2,"&frm=");
	if(frm)
	{
		char* plt=strstr(frm,"&plt=");
		if(plt)
		{
			plt[0]=0;
			len=plt-str2;
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "@@@the url has the &plt,damn CT\n");
		}
	}
	
#endif
	//check whether there is a space
	for(i=0;i<len;i++)
	{
		if(str2[i]==0x20)
		{
			memmove(str2+i+3,str2+i+1,len-i);
			str2[i]='%';
			str2[i+1]='2';
			str2[i+2]='0';
			//i+=3;
			len+=3;
			str2[len-1]=0;
		}
	}
	return true;
}
int RemoveInnerRN(char* str2)
{
	int len = strlen(str2);
	//char str2[12];
	//memcpy(str2,str,len);
	while(str2[len-1]=='\n'||str2[len-1]=='\r')
	{
		str2[len-1]=0;
		len--;
	}
	return true;
}
bool CRTSPClientEngine::ParseSDP(const char * sdpBuf)
{
	//return m_rtspSession->ParseSDP(sdpBuf);
	return true;
}
bool CRTSPClientEngine::ParseSDP2(const char * rtspURL)
{
	char localFile[256]={0};
	const char* postfix="data";
	CUserOptions::UserOptions.m_URLType = UT_SDP2_LOCAL;
	CRealTimeStreamingEngine * streamingEngine = CRealTimeStreamingEngine::CreateRealTimeStreamingEngine();
	int urlLen = strlen(rtspURL);
	strcpy(localFile,rtspURL);
	memcpy(localFile+urlLen-4,postfix,4);
	bool dataFileExist = streamingEngine->SetLocalStream(localFile);
	sprintf(CLog::formatString,"begin loading data  %s\n",localFile);
	CLog::Log.MakeLog(LL_RTP_ERR,"flow.txt", CLog::formatString);
	if(!dataFileExist)
	{
		CLog::Log.MakeLog(LL_RTSP_ERR,"sdp2.txt", "ParseSDP2:can not open the stream file \r\n");
		return false;
	}
	FILE* sdpFile=fopen(rtspURL,"rb");
	CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "begin open sdp2 \r\n");
	if(sdpFile)
	{
		char* sdpBuf = new char[2048];TRACE_NEW("rspce_sdpBuf",sdpBuf);
		fread(sdpBuf,1,2048,sdpFile);
		bool ok=m_rtspSession->ParseSDP(sdpBuf);
		fclose(sdpFile);
		SAFE_DELETE_ARRAY(sdpBuf);
		sdpBuf  = NULL;
		sdpFile = NULL;
		return ok;
	}
	else
	{
		CLog::Log.MakeLog(LL_RTSP_ERR,"sdp2.txt", "ParseSDP:can not open the sdp file \r\n");
		return false;
	}
	return false;
}

void CRTSPClientEngine::parseExtraFiled()
{
	char *extra = CUserOptions::UserOptions.m_extraFields;
	
	if(extra&&strlen(extra)>5)
	{
		if(strstr(extra, "User-Agent"))
		{
			m_bIfUAgent = 1;
			
			int len=strlen(extra);

			char *head = strstr(extra, "User-Agent")+12;
			char *end = strstr(head, "\r\n");

			if(!end)
			{
				end = extra+len;
			}

			delete [] m_userAgentAttr;
			m_userAgentAttr=new char[end-head+1];
			memcpy(m_userAgentAttr,head,end-head);
			m_userAgentAttr[end-head]='\0';
		}
		
		if(strstr(extra, "Bandwidth"))
			m_bIfBandWidth= true;

		if(strstr(extra, "x-wap-profile"))
			m_BIfWapProfile= true;

	}
}

bool CRTSPClientEngine::GetCmd()
{
	do 
	{
		strcpy(cmdFormatString,"GET %s HTTP/1.0\r\n"
			"CSeq: %d\r\n"
			"x-sessioncookie: %s\r\n"
			"Accept: application/x-rtsp-tunnelled\r\n"
			"Host: 10.2.68.93\r\n"
			"Pragma: no-cache\r\n"
			"Cache-Control: no-cache\r\n");

		if(!m_bIfUAgent)
		{
			sprintf(m_rtspCMDString,"User-Agent: %s\r\n", m_userAgentAttr);
			strcat(cmdFormatString,m_rtspCMDString);
		}

		sprintf(m_rtspCMDString, cmdFormatString, m_hostPath, ++m_rtspCSeq, m_fSessionCookie);
		strcat(m_rtspCMDString,"\r\n");

		if(!SendRTSPCommandToStreamingServerOverHttpTunnel(m_rtspCMDString))
			break;

		unsigned int responseBytes;

		if(!RecvResponseFromStreamingServerOverHttpTunnel(responseBytes,VOS_CMD_GET)) 
			break;
	} while (0);
	return true;
}

bool CRTSPClientEngine::PostCmd()
{
	strcpy(cmdFormatString,"POST %s HTTP/1.0\r\n"
		"CSeq: %d\r\n"
		"x-sessioncookie: %s\r\n"
		"Content-Type: application/x-rtsp-tunnelled\r\n"
		"Pragma: no-cache\r\n"
		"Cache-Control: no-cache\r\n"
		"Content-Length: 32767\r\n"
		"Expires: Sun, 9 Jan 1972 00:00:00 GMT\r\n");

		if(!m_bIfUAgent)
		{
			sprintf(m_rtspCMDString,"User-Agent: %s\r\n", m_userAgentAttr);
			strcat(cmdFormatString,m_rtspCMDString);
		}

		sprintf(m_rtspCMDString, cmdFormatString, m_hostPath, m_rtspCSeq, m_fSessionCookie);
		strcat(m_rtspCMDString,"\r\n");

		if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
			VOLOGI("Send POST CMD fail");

	return true;
}

bool CRTSPClientEngine::DescribeCmd(char * rtspURL)
{
	
	if(CUserOptions::UserOptions.m_URLType==UT_SDP2_LOCAL)
		return true;

	if (CUserOptions::UserOptions.m_isOverHttpTunnel)
	{
		if(!Init())
			return false;
		CleanURL(rtspURL);//check the URL,if there is new line  symbol,remove it,if there is a space,add a %
		CUserOptions::UserOptions.m_currURL=rtspURL;
		if (!ConnectToStreamingServerOverHttpTunnel(rtspURL))
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "GetCmd:Can not connect streaming server! \r\n");
			return false;
		}

		GenerateSessionCookie();	//generate session cookie for http tunnel.


		strcpy(m_hostPath, strstr((rtspURL+8), "/"));

		VOLOGI("_____________describe m_hostPath = %s", m_hostPath);

		GetCmd();
	}

	do
	{
		if(!Init())
			break;
		CleanURL(rtspURL);//check the URL,if there is new line  symbol,remove it,if there is a space,add a %
		CUserOptions::UserOptions.m_currURL=rtspURL;
 
		if(!ConnectToStreamingServer(rtspURL)) 
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "Describe:Can not connect streaming server! \r\n");
			return false;
		}
		
		char* sdpBuf = m_recvBuf;	

		parseExtraFiled();

		if (CUserOptions::UserOptions.m_isOverHttpTunnel)
		{
			if(!ConnectToStreamingServer(rtspURL)) 
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "POST:Can not connect streaming server! \r\n");
				return false;
			}
			PostCmd();

			OptionsCmd(rtspURL);
		}

		if(CUserOptions::UserOptions.m_URLType==UT_SDP_URL&&m_sdpFileString)
		{
			//int lenofContentBase = strlen(m_contentBase);
			//if(CUserOptions::UserOptions.sdpURLLength!=lenofContentBase)
			///	CUserOptions::UserOptions.sdpURLLength=lenofContentBase;
			//sprintf(CLog::formatString,"")
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "!!!get the sdp file\n");
			sdpBuf = m_sdpFileString;

		}
		else
		{

			//const char * acceptAttr = "application/sdp";
			strcpy(cmdFormatString,"DESCRIBE %s RTSP/1.0\r\n"
									 "CSeq: %d\r\n"
									 "Accept: application/sdp\r\n"
									 "Accept-Encoding: identity\r\n"
									 "Content-Encoding: identity\r\n"
									 "Content-Language: en-US\r\n");

			if(!m_bIfUAgent)
			{
				sprintf(m_rtspCMDString,"User-Agent: %s\r\n", m_userAgentAttr);
				strcat(cmdFormatString,m_rtspCMDString);
			}
			
			if((CUserOptions::UserOptions.m_nBandWidth!=0) && (!m_bIfBandWidth))
			{
				sprintf(m_rtspCMDString,"Bandwidth:%u\r\n",CUserOptions::UserOptions.m_nBandWidth);
				strcat(cmdFormatString,m_rtspCMDString);
			}
			
			if((strlen(m_UAProfile)>2)&&(!m_BIfWapProfile))
			{
				sprintf(m_rtspCMDString,"x-wap-profile: %s\r\n",m_UAProfile);
				strcat(cmdFormatString,m_rtspCMDString);
			}
									 
			sprintf(m_rtspCMDString, cmdFormatString, rtspURL, ++m_rtspCSeq);

			AddRTSPFields(CUST_DESCRIBE);
			if(CUserOptions::UserOptions.m_extraFields&&strlen(CUserOptions::UserOptions.m_extraFields)>5)
			{
				strcat(m_rtspCMDString,CUserOptions::UserOptions.m_extraFields);
				int len=strlen(CUserOptions::UserOptions.m_extraFields);
				if(CUserOptions::UserOptions.m_extraFields[len-2]!='\r'||CUserOptions::UserOptions.m_extraFields[len-1]!='\n')
						strcat(m_rtspCMDString,"\r\n");
			}
			
			strcat(m_rtspCMDString,"\r\n");

			if (CUserOptions::UserOptions.m_isOverHttpTunnel)
			{
				char* FormatString = new char[MAX_TMPLEN];TRACE_NEW("FormatString",FormatString);

				VOLOGI("_____________DescribeCmd____________before encoder=%s", m_rtspCMDString);
				EncodeBase64(m_rtspCMDString, FormatString);
				VOLOGI("_____________DescribeCmd____________after encoder=%s", FormatString);


				if(!SendRTSPCommandToStreamingServer(FormatString))
					break;

				unsigned int responseBytes;
				if(!RecvResponseFromStreamingServerOverHttpTunnel(responseBytes,VOS_CMD_DESCRIBE)) 
					break;

				sprintf(CLog::formatString,"______________m_recvBuf = %s\n", m_recvBuf);
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
			}
			else
			{
				if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
				break;

				unsigned int responseBytes;

				if(!RecvResponseFromStreamingServer(responseBytes,VOS_CMD_DESCRIBE)) 
					break;
			}

			char * responseLine = m_recvBuf;
			char * nextResponseLine = GetNextLine(responseLine);
			unsigned int responseCode = 0;
			bool	  bIsOK200=false;

			if(bIsOK200==false)
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
				if(!ParseResponseCode(responseLine, responseCode))
				{
					CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
					break;
				}
			}
			
			bool needRedirection = false;
			char * redirectionURL = NULL;
			if(responseCode == 301 || responseCode == 302) 
			{ // redirection 
				needRedirection = true;
				redirectionURL = new char[MAX_PATH*2];TRACE_NEW("rspce_redirectionURL",redirectionURL);//MAX_PATH is small to handle some unregular URL
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "$$$Describe:Processing redirection_1! \r\n");
			} 
			else if(responseCode == 401)
			{
				if(!CUserOptions::UserOptions.digestIsSet)
					break;

				/*Try to use "user" "user" to authenticate first*/
				char method[33] = {0};
				strcpy(method,"DESCRIBE");

				m_digestAuth = new voStreamDigestAuth;

				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "@@@@DESCRIBE get not authenticate,try to authenticate\n");

				char * currentLine = nextResponseLine;

				while(currentLine)
				{
					if(_strnicmp(currentLine, "WWW-Authenticate:",17) == 0)
					{
						CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "@@@@Got WWW-Authenticate!\n");
						char *buf = currentLine;
						char *temp = NULL;

						temp=strstr(buf,"realm");
						if(temp)
						{
							sscanf(temp+7,"%[^\"]",m_digestRealm);
						}

						temp=strstr(buf,"nonce");
						if(temp)
						{
							sscanf(temp+7,"%[^\"]",m_digestNonce);
						}

						break;
					}
					currentLine = GetNextLine(currentLine);
				}

				if((strlen(m_digestRealm)!=0) && (strlen(m_digestNonce) != 0))
				{
					m_digestAuth->SetMethod(method,strlen(method));
					m_digestAuth->SetNonce(m_digestNonce,strlen(m_digestNonce));
					m_digestAuth->SetRealm(m_digestRealm,strlen(m_digestRealm));
					m_digestAuth->SetUserInfo(CUserOptions::UserOptions.digestAuth.user, 
						CUserOptions::UserOptions.digestAuth.userLen,
						CUserOptions::UserOptions.digestAuth.passwd,
						CUserOptions::UserOptions.digestAuth.passwdLen);
					m_digestAuth->SetURI(rtspURL, strlen(rtspURL));

					if(!m_digestAuth->GetResponse(m_digestResp, 32))
					{
						CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "@@@@Can not get authen response code!\n");
						CUserOptions::UserOptions.errorID = E_USER_ERROR_BASE-responseCode;
						break;
					}

					strcpy(cmdFormatString,"DESCRIBE %s RTSP/1.0\r\n"
						"CSeq: %d\r\n"
						"Accept: application/sdp\r\n"
						"Authorization: Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\"\r\n");

					if(!m_bIfUAgent)
					{
						sprintf(m_rtspCMDString,"User-Agent: %s\r\n", m_userAgentAttr);
						strcat(cmdFormatString,m_rtspCMDString);
					}

					sprintf(m_rtspCMDString, cmdFormatString, rtspURL, ++m_rtspCSeq,CUserOptions::UserOptions.digestAuth.user,
						m_digestRealm,m_digestNonce,rtspURL,m_digestResp);

					strcat(m_rtspCMDString,"\r\n");
				}

				if (CUserOptions::UserOptions.m_isOverHttpTunnel)
				{
					char* FormatString = new char[MAX_TMPLEN];TRACE_NEW("FormatString",FormatString);

					VOLOGI("_____________DescribeCmd____________before encoder=%s", m_rtspCMDString);
					EncodeBase64(m_rtspCMDString, FormatString);
					VOLOGI("_____________DescribeCmd____________after encoder=%s", FormatString);


					if(!SendRTSPCommandToStreamingServer(FormatString))
						break;

					unsigned int responseBytes;
					if(!RecvResponseFromStreamingServerOverHttpTunnel(responseBytes,VOS_CMD_DESCRIBE)) 
						break;

					sprintf(CLog::formatString,"______________m_recvBuf = %s\n", m_recvBuf);
					CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
				}
				else
				{
					if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
					{
						CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "@@@@send SET_PARAMETER response fail2\n");
						break;
					}

					unsigned int responseBytes;
					if(!RecvResponseFromStreamingServer(responseBytes,VOS_CMD_DESCRIBE)) 
						break;
					//damn 
				}

				responseLine = m_recvBuf;
				nextResponseLine = GetNextLine(responseLine);
				responseCode = 0;
				if(ParseResponseCode(responseLine, responseCode))
				{
					bIsOK200=responseCode==200;
					if(bIsOK200)
					{
						CUserOptions::UserOptions.digestIsSet = true;
					}
					else
						break;
				}
			}
			else if(responseCode != 200)
			{
				CUserOptions::UserOptions.errorID = E_USER_ERROR_BASE-responseCode;
				break;
			}

			int contentLength = -1;
			int line=300;
			while(line-->0) 
			{
				responseLine = nextResponseLine;
				if(responseLine == NULL)
					break;
				nextResponseLine = GetNextLine(responseLine);

				if(needRedirection)
				{
					CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "$$$Describe:Processing redirection_2! \r\n");

					if(sscanf(responseLine, "Location: %s", redirectionURL) == 1) 
					{
						CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "$$$Describe:Processing redirection_3! \r\n");
						Reset();
						bool result = DescribeCmd(redirectionURL);
						SAFE_DELETE_ARRAY(redirectionURL);
						return result;
					}
				}
				//CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "$$$Describe::dddd \n");
				if(sscanf(responseLine, "Content-Length: %d", &contentLength) == 1 || sscanf(responseLine, "Content-length: %d", &contentLength) == 1) 
					continue;
				if(ParseContentBase(responseLine))
					continue;
			}
			SAFE_DELETE_ARRAY(redirectionURL);

			if(contentLength > 0)
			{
				if (CUserOptions::UserOptions.m_isOverHttpTunnel)
				{
					if(ReadNetDataExact(m_rtspSock2, (unsigned char *)m_recvBuf, contentLength) != contentLength)
						break;
				}
				else
				{
					if(ReadNetDataExact(m_rtspSock, (unsigned char *)m_recvBuf, contentLength) != contentLength)
						break;
				}
				m_recvBuf[contentLength] = '\0';
				if(CUserOptions::UserOptions.m_bMakeLog==LOG_DATA)
				{
					CLog::EnablePrintTime(false);
					CLog::Log.MakeLog(LL_AUDIO_DATA,"RTSPStream.sdp2", m_recvBuf);
				}
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", m_recvBuf);
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "\r\n");
			}		
		}
		if (m_rtspSession ==NULL)
		{
			m_rtspSession = new CRTSPSession(this);TRACE_NEW("rspce_m_rtspSession",m_rtspSession);
		}

		m_bSyncPlayResponse = strstr(sdpBuf,"o=CarbonStreamingServer")!=NULL;
		if(!m_rtspSession->Init(sdpBuf, m_rtspServerAddr))
		{
			CUserOptions::UserOptions.errorID = E_UNSUPPORTED_FORMAT;
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "@@@@fail to init the rtspsession!!\n");
			break;
		}
		if(CUserOptions::UserOptions.m_exit)
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","After describe, user cancel it\n");
			break;
		}
		return true;

	}while(0);
	NotifyEvent(VO_EVENT_DESCRIBE_FAIL,CUserOptions::UserOptions.errorID);
	CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt", "@@@@Parse describe fail!!\n");
	return false;
}
bool CRTSPClientEngine::ReSetupCmd(bool streamingOverTCP)
{
	if(CUserOptions::UserOptions.m_URLType==UT_SDP2_LOCAL)
		return true;

	int streamCount = MediaStreamCount();
	for(int streamIndex=0; streamIndex<streamCount; ++streamIndex)
	{
		CMediaStream * pMediaStream = MediaStream(streamIndex);
		pMediaStream->SetTransportProtocol(streamingOverTCP);
		pMediaStream->UpdateSocket();
	}
	return SetupCmd(streamingOverTCP);
}
bool CRTSPClientEngine::SwitchChannel(char* newUrl)
{
	do 
	{
		if(SetupCmd())//compatible Codec params
		{
			//Flush current channel first
			FlushRTSPStreams();
			CLog::Log.MakeLog(LL_RTSP_ERR,"mediaFlo.txt","@@@@SwitchChannel setup OK\n");
		}
		else//Incompatible Codec params
		{
			FlushRTSPStreams();
			m_pRTSPSource->SetSessionStatus(Session_SwitchChannel);
			if(!DescribeCmd(newUrl))
				break;
			if(!SetupCmd())
				break;
			if(!PlayCmd(0))
				break;
		}
		return true;
	} while(0);
	
	CLog::Log.MakeLog(LL_RTSP_ERR,"mediaFlo.txt","@@@@SwitchChannel fail\n");
	return false;
}
bool CRTSPClientEngine::SetupCmd(bool streamingOverTCP)
{
	if(CUserOptions::UserOptions.m_URLType==UT_SDP2_LOCAL)
		return true;
	
	m_failConnectNum = 0;
	m_streamingOverTCP = streamingOverTCP;
	int streamCount = MediaStreamCount();
	for(int streamIndex=0; streamIndex<streamCount; ++streamIndex)
	{
		if(!SetupCmd_MediaStream(streamIndex))
		{
			
			CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt","@@@@setup fail!\n");
			return false;
		}
		if(0)//CUserOptions::UserOptions.m_useTCP<=0)
		{
			SendExtraRTPPacket();
			SendExtraRTCPPacket();
		}

	}

	return true;
}

bool CRTSPClientEngine::SetupCmd_MediaStream(int streamIndex)
{
	CMediaStream * pMediaStream = MediaStream(streamIndex);
	int loopCount=0;
	do
	{
		const char* constrol=pMediaStream->ControlAttr();
		if(strstr(constrol,"RTSP://")==NULL&&strstr(constrol,"rtsp://")==NULL)
		{
			if(m_URLofRequest[strlen(m_URLofRequest)-1]!='/')
				sprintf(m_rtspCMDString,"SETUP %s/%s RTSP/1.0\r\n",m_URLofRequest, pMediaStream->ControlAttr());
			else
				sprintf(m_rtspCMDString,"SETUP %s%s RTSP/1.0\r\n",m_URLofRequest, pMediaStream->ControlAttr());
		}
		else
		{
			sprintf(m_rtspCMDString,"SETUP %s RTSP/1.0\r\n",pMediaStream->ControlAttr());
		}
		
		sprintf(cmdFormatString,"CSeq: %d\r\nUser-Agent: %s\r\n",++m_rtspCSeq,m_userAgentAttr);
		strcat(m_rtspCMDString,cmdFormatString);

		if((CUserOptions::UserOptions.digestIsSet) && (m_digestAuth))
		{
			char method[33] = {0};
			strcpy(method,"SETUP");

			m_digestAuth->SetMethod(method,strlen(method));
			m_digestAuth->GetResponse(m_digestResp, 32);
			
			sprintf(cmdFormatString, "Authorization: Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\"\r\n",
									CUserOptions::UserOptions.digestAuth.user,
									m_digestRealm,m_digestNonce,m_rtspURL,m_digestResp);
			strcat(m_rtspCMDString,cmdFormatString);
		}
		
		if(m_rtspSessionID != NULL)
		{
			sprintf(cmdFormatString, "Session: %s\r\n", m_rtspSessionID);
			strcat(m_rtspCMDString,cmdFormatString);
		}
	
		if(m_rtspSession->SessionEntityTag() != NULL)
		{
//			sprintf(m_rtspCMDString, "If-Match: %s\r\n", m_rtspSession->SessionEntityTag());
		}

		if(m_streamingOverTCP)
		{
			sprintf(cmdFormatString, "Transport: RTP/AVP/TCP;unicast;interleaved=%d-%d\r\n", pMediaStream->RTPChannelID(), pMediaStream->RTCPChannelID());
			strcat(m_rtspCMDString,cmdFormatString);
		}
		else
		{
			sprintf(cmdFormatString, "Transport: RTP/AVP/UDP;unicast;client_port=%d-%d\r\n", pMediaStream->ClientRTPPort(), pMediaStream->ClientRTCPPort());
			strcat(m_rtspCMDString,cmdFormatString);
		}
		
		if(CUserOptions::UserOptions.m_bOpenUAProEachRequest&&strlen(m_UAProfile)>2)
		{
			sprintf(cmdFormatString,"x-wap-profile: %s\r\n",m_UAProfile);
			strcat(m_rtspCMDString,cmdFormatString);
		}

		AddRTSPFields(CUST_SETUP);
#if SUPPORT_ADAPTATION
		//int reportFreq=pMediaStream->GetReportFreqOf3gppAdaptation();
		//if(streamIndex==0&&reportFreq>0)
		if(CUserOptions::UserOptions.m_n3GPPAdaptive)
		{
			strcat(m_rtspCMDString,"3GPP-Adaptation:url=\"");
			strcat(m_rtspCMDString,m_URLofRequest);
			sprintf(cmdFormatString,"\";size=%d;target-time=%d\r\n",
				CUserOptions::UserOptions.m_nMediaBuffTime*CUserOptions::UserOptions.outInfo.clip_bitrate*128,
				CUserOptions::UserOptions.m_nMediaBuffTime*1000);
			strcat(m_rtspCMDString,cmdFormatString);
		}
#endif//SUPPORT_ADAPTATION
		strcat(m_rtspCMDString,"\r\n");

		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt",m_URLofRequest);

		const char * ControlAttr = pMediaStream->ControlAttr();
		if(ControlAttr)
			CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", ControlAttr);

		if (CUserOptions::UserOptions.m_isOverHttpTunnel)
		{
			char* FormatString = new char[MAX_TMPLEN];TRACE_NEW("FormatString",FormatString);

			VOLOGI("_____________SETUPCMD____________before encoder=%s", m_rtspCMDString);
			EncodeBase64(m_rtspCMDString, FormatString);
			VOLOGI("_____________SETUPCMD____________after encoder=%s", FormatString);

			if(!SendRTSPCommandToStreamingServer(FormatString))
			{
				break;
			}

			unsigned int responseBytes;
			if(!RecvResponseFromStreamingServerOverHttpTunnel(responseBytes,VOS_CMD_SETUP)) 
				break;
		}
		else
		{
			if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
			{
				break;
			}

			unsigned int responseBytes;
			if(!RecvResponseFromStreamingServer(responseBytes,VOS_CMD_SETUP)) 
				break;
		}

		char * responseLine = m_recvBuf;
		char * nextResponseLine = GetNextLine(responseLine);
		unsigned int responseCode = 0;
		//check whether this is a SET_PARAMETER
		if(_strnicmp(responseLine, "SET_PARAMETER", 13) == 0) 
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "@@@@SET_PARAMETER,reconnect\n");
			const char * cmdFormatString = "RTSP/1.0 501 Not Implemented\r\n"
								 "CSeq: %d\r\n"
								 "%s"						//Session attribute
								 "\r\n";
			//get the CSeq
			unsigned int seq=0;
			int count = 30;
			char setupSessionAttr[64];
			while(count--)
			{
				responseLine = nextResponseLine;
				if(responseLine == NULL)
					break;
				if(sscanf(responseLine, "CSeq: %u",&seq)==1)
					break;
				nextResponseLine = GetNextLine(responseLine);
			}
			sprintf(m_rtspCMDString, cmdFormatString,seq, setupSessionAttr);

			if (CUserOptions::UserOptions.m_isOverHttpTunnel)
			{
				char* FormatString = new char[MAX_TMPLEN];TRACE_NEW("FormatString",FormatString);

				VOLOGI("_____________SETUPCMD____________before encoder=%s", m_rtspCMDString);
				EncodeBase64(m_rtspCMDString, FormatString);
				VOLOGI("_____________SETUPCMD____________after encoder=%s", FormatString);

				if(!SendRTSPCommandToStreamingServer(FormatString))
				{
					break;
				}

				unsigned int responseBytes;
				if(!RecvResponseFromStreamingServerOverHttpTunnel(responseBytes,VOS_CMD_SETUP)) 
					break;
			}
			else
			{
				if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
				{
					CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "@@@@send SET_PARAMETER response fail\n");
					break;
				}
				unsigned int responseBytes;
				if(!RecvResponseFromStreamingServer(responseBytes,VOS_CMD_SETUP)) 
					break;
			}

			responseLine = m_recvBuf;
			nextResponseLine = GetNextLine(responseLine);
			responseCode = 0;
		}

		if(!ParseResponseCode(responseLine, responseCode))
			break;

		if(responseCode != 200)
		{
			if (0)//responseCode==451)//param not understood,sometimes it is a server issue,resend it
			{
				
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "try RTSP Setup again\n");
				if(loopCount++<50)
					continue;
			}
			if(responseCode==E_USER_ERROR_BASE-E_UNSUPPORTED_TRANSPORT)
			{
				bool IsCurUseTCP=CUserOptions::UserOptions.m_useTCP==1;
				m_streamingOverTCP=!IsCurUseTCP;
				m_pRTSPSource->SetTransportProtocol(!IsCurUseTCP);
				if(IsCurUseTCP)
				{
					CUserOptions::UserOptions.m_useTCP=-1;
					CUserOptions::UserOptions.portScale=0.5;
				}
				else
				{
					CUserOptions::UserOptions.m_useTCP=1;
					CUserOptions::UserOptions.portScale=1;
				}
				
			}
			
			CUserOptions::UserOptions.errorID = E_USER_ERROR_BASE-responseCode;
			break;
			
		
		}

		while(1) 
		{
			responseLine = nextResponseLine;
			if(responseLine == NULL)
				break;

			nextResponseLine = GetNextLine(responseLine);

			if(ParseSetupCMDResponse_Session(responseLine))
				continue;

			if(ParseSetupCMDResponse_Transport(responseLine, streamIndex))
				continue;
		}
		if(CUserOptions::UserOptions.m_exit)
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","After setup, user cancel it\n");
			break;
		}
		return true;

	}while(1);
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "@@@@Parse setup fail!!\n");

	return false;
}
int CRTSPClientEngine::SendExtraRTPPacket()
{
#define RTP_HEAD_SIZE 128
	unsigned char nullRTPPacket[RTP_HEAD_SIZE]={0};
	nullRTPPacket[0]=0x80;
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","SendExtraRTPPacket()!\n");
	for(int i=0;i<MediaStreamCount();i++)
	{
		CMediaStream* mediaStream = MediaStream(i);
		CMediaStreamSocket* rtpStreamSocket = mediaStream->GetRTPStreamSocket();
		if(rtpStreamSocket)
		{
			struct sockaddr_storage   rtpServerAddr;
			memset(&rtpServerAddr, 0, sizeof(sockaddr_storage));
			memcpy(&rtpServerAddr, mediaStream->MediaAddr(), sizeof(sockaddr_storage));

			rtpServerAddr.port = HostToNetShort(mediaStream->ServerRTPPort()) ;
			WriteNetData(rtpStreamSocket->GetMediaStreamSock(), &rtpServerAddr, nullRTPPacket, RTP_HEAD_SIZE);

		}
	}


	return 1;
}
int CRTSPClientEngine::SendExtraRTCPPacket()
{
#define RTCP_HEAD_SIZE 128
	unsigned char nullRTCPPacket[RTCP_HEAD_SIZE]={0};
	nullRTCPPacket[0]=0x80;
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","SendExtraRTCPPacket()!\n");
	for(int i=0;i<MediaStreamCount();i++)
	{
		CMediaStream* mediaStream = MediaStream(i);
		CMediaStreamSocket* rtcpStreamSocket = mediaStream->GetRTCPStreamSocket();
		if(rtcpStreamSocket)
		{
			struct sockaddr_storage   rtpServerAddr;
			memset(&rtpServerAddr, 0, sizeof(sockaddr_storage));
			memcpy(&rtpServerAddr, mediaStream->MediaAddr(), sizeof(sockaddr_storage));
			rtpServerAddr.port = HostToNetShort(mediaStream->ServerRTCPPort()); 
			WriteNetData(rtcpStreamSocket->GetMediaStreamSock(), &rtpServerAddr, nullRTCPPacket, RTCP_HEAD_SIZE);

		}
	}


	return 1;
}
int	CRTSPClientEngine::IsWaitingResponse()
{
	for(int id=RR_ID_PLAY;id<RR_ID_NUMBER;id++)
	{
		int cseq=m_responsHandler[id].GetCSEQ();
		if(cseq)
		{
			sprintf(CLog::formatString,"IsWaitingResponse()%d\n",cseq);
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
			return id;
		}
	}
	return RR_ID_UNKNOWN;
}
bool	CRTSPClientEngine::IsRTSPResponseTimeOut()
{
  	for(int id=RR_ID_PLAY;id<RR_ID_NUMBER;id++)
	{
		int cseq=m_responsHandler[id].GetCSEQ();
		if(cseq)
		{
			long curTime  = timeGetTime();
			int  sendTime = m_responsHandler[id].GetSendTime();
			if(curTime-sendTime>CUserOptions::UserOptions.m_nRTSPTimeOut*1000)
			{
				sprintf(CLog::formatString,"ResponseTimeOut(%d)%ld-%d\n",id,curTime,sendTime);
				CLog::Log.MakeLog(LL_RTSP_ERR,"buffering.txt",CLog::formatString);
				m_responsHandler[id].Reset();
				return true; 				
			}
		}
	}

  	return false;
}
bool CRTSPClientEngine::IsKeepAcitveResponse()
{
#if KEEPALIVE_VIA_SET_PARAM
	char * responseLine = m_recvBuf;
	unsigned int responseCode = 0;
	ParseResponseCode(responseLine, responseCode);
	if(responseCode>400)//SET_PARAMETER return 451
	{
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","GetResponse of KeepAliveWith_SET_PARAM\n");
		return true;
	}
#endif//KEEPALIVE_VIA_SET_PARAM
	return false;
}
int CRTSPClientEngine::GetTheSeqNum(char* response,int len)
{
	int	  seqNum=-1;
	char* seq=strstr(response,"CSeq:");
	if(seq)
	{
		sscanf(seq,"CSeq:%d",&seqNum);
	}
	else
	{
		seq=strstr(response,"Cseq:");
		if(seq)
		{
			sscanf(seq,"Cseq:%d",&seqNum);
		}
		else
		{
			seq = strstr(response,"cseq:");
			if(seq)
			{
				sscanf(seq,"cseq:%d",&seqNum);
			}
		}
	}
	return seqNum;
}
void CRTSPClientEngine::SetNextTaskOfResponse(int id,int task)
{
	
	int playTask=m_pRTSPSource->GetPlayTask();
	int pauseTask=m_pRTSPSource->GetPauseTask();
	switch(id)
	{
	case RR_ID_PLAY:
		if(1)//task==pauseTask)
		{
			m_responsHandler[id].SetNextTask(task);
		}
		break;
	case RR_ID_PAUSE:
		if(task!=pauseTask)
		{
			m_responsHandler[id].SetNextTask(task);
		}
		break;
	case RR_ID_KEEPALIVE:
		{
			int existTask=m_responsHandler[RR_ID_KEEPALIVE].GetNextTask();
			
			if(existTask==0||(existTask==playTask&&task==pauseTask)||(task==playTask&&existTask==pauseTask))
			{
				m_responsHandler[id].SetNextTask(task);
			}
		}
		break;
	default:
		break;
	}
	
};
bool CRTSPClientEngine::PlayCmd(float startTime,float endTime)
{
	if(CUserOptions::UserOptions.m_URLType==UT_SDP2_LOCAL)
		return true;

	if(CUserOptions::UserOptions.m_useTCP<=0
#if ENABLE_EXTRA_RTP
		&&CUserOptions::UserOptions.m_nPortOpener
#endif
		)
	{
		SendExtraRTPPacket();
		SendExtraRTCPPacket();
	}
	unsigned int responseCode = 0;
	CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
	pRTSPMediaStreamSyncEngine->SetPlayRangeBegin(0.0);

	m_pRTSPSource->SetBeginTime(timeGetTime());
	do
	{
		if(m_rtspSessionID == NULL)
			break;

		CRealTimeStreamingEngine * streamingEngine = CRealTimeStreamingEngine::CreateRealTimeStreamingEngine();
		if(streamingEngine == NULL)
			break;
		streamingEngine->RemoveStreamReader(m_rtspSock);

		strcpy(cmdFormatString,"PLAY %s RTSP/1.0\r\n"
			"CSeq: %d\r\n"
			"Session: %s\r\n"
			"User-Agent: %s\r\n");

		if((CUserOptions::UserOptions.digestIsSet) && (m_digestAuth))
		{
			char method[33] = {0};
			strcpy(method,"PLAY");

			m_digestAuth->SetMethod(method,strlen(method));
			m_digestAuth->GetResponse(m_digestResp, 32);
			
			sprintf(m_rtspCMDString, "Authorization: Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\"\r\n",
									CUserOptions::UserOptions.digestAuth.user,
									m_digestRealm,m_digestNonce,m_rtspURL,m_digestResp);
			strcat(cmdFormatString,m_rtspCMDString);
		}
		
		if(startTime >= 0)
		{
			char* range=m_rtspCMDString;
			if(endTime>0)
			{
				sprintf(range,"Range: npt=%.3f-%.3f\r\n",startTime,endTime);
				strcat(cmdFormatString,range);
			}
			else
			{
				sprintf(range,"Range: npt=%.3f-\r\n",startTime);
				strcat(cmdFormatString,range);
			}
		}
		if(CUserOptions::UserOptions.m_nBandWidth!=0)
		{
			sprintf(m_rtspCMDString,"Bandwidth:%u\r\n",CUserOptions::UserOptions.m_nBandWidth);
			strcat(cmdFormatString,m_rtspCMDString);
		}


		if(CUserOptions::UserOptions.m_bOpenUAProEachRequest&&strlen(m_UAProfile)>2)
		{
			sprintf(m_rtspCMDString,"x-wap-profile: %s\r\n",m_UAProfile);
			strcat(cmdFormatString,m_rtspCMDString);
		}
		sprintf(m_rtspCMDString, cmdFormatString, m_URLofRequest, ++m_rtspCSeq, m_rtspSessionID, m_userAgentAttr);
		AddRTSPFields(CUST_PLAY);
#if SUPPORT_ADAPTATION
		/*
		sprintf(m_rtspCMDString,"3GPP-LinkChar: url=\"%s\"; GBW=%d; MBW=%d; MTD=%d\r\n",m_URLofRequest,
		CUserOptions::UserOptions.m_3GPPLinkChart.mGBR,
		CUserOptions::UserOptions.m_3GPPLinkChart.mMBR,
		CUserOptions::UserOptions.m_3GPPLinkChart.mMaxDelay);*/
		if(CUserOptions::UserOptions.m_n3GPPAdaptive)
		{
			strcat(m_rtspCMDString,"3GPP-LinkChar: url=\"");
			strcat(m_rtspCMDString,m_URLofRequest);
			strcat(m_rtspCMDString,"\"\r\n");
			sprintf(cmdFormatString,"SetDeliveryBandwidth: Bandwidth=%d;BackOff=0\r\n",
				CUserOptions::UserOptions.outInfo.clip_bitrate*1000);
			strcat(m_rtspCMDString,cmdFormatString);
		}
#endif//SUPPORT_ADAPTATION
		strcat(m_rtspCMDString,"\r\n");
		
		m_responsHandler[RR_ID_PLAY].SetCSEQ(m_rtspCSeq);
		m_responsHandler[RR_ID_PLAY].SetSendTime(timeGetTime());
		//retrieve all the packets first,because the RTPinfo will change when the new play response is back
		for(int streamIndex=0; streamIndex<MediaStreamCount(); ++streamIndex)
		{
			MediaStream(streamIndex)->retrieveAllPacketsFromRTPBuffer();
		}
		pRTSPMediaStreamSyncEngine->SetPlayResponse(false);//this is must!!
		if(CUserOptions::UserOptions.streamType==ST_ASF)
			pRTSPMediaStreamSyncEngine->ResetASFCOMSEQ();

		if (CUserOptions::UserOptions.m_isOverHttpTunnel)
		{
			char* FormatString = new char[MAX_TMPLEN];TRACE_NEW("FormatString",FormatString);

			VOLOGI("_____________PLAYCMD____________before encoder=%s", m_rtspCMDString);
			EncodeBase64(m_rtspCMDString, FormatString);
			VOLOGI("_____________PLAYCMD____________after encoder=%s", FormatString);


			if(!SendRTSPCommandToStreamingServer(FormatString))
			{
				break;
			}

			unsigned int responseBytes;
			if(CUserOptions::UserOptions.m_useTCP>0)
			{
				//m_streamLength = 0;
				//m_streamPos = 0;
				SetSocketRecvBufSize(m_rtspSock2, 100*1024);
				streamingEngine->AddStreamReader(m_rtspSock2, ReadStreamsOverTCP, this);
#if !ASYNC_TCP
				int count = 20;
				bool IsPlayResponse=false;
				while (count-->0)
				{
					IsPlayResponse = false;
					responseBytes = HandleResponseViaTCP();
					int seq_num=GetTheSeqNum(m_rtspCMDString,responseBytes);

					if(seq_num==expectCSEQ)
					{
						IsPlayResponse = true;
						//pRTSPMediaStreamSyncEngine->SetPlayResponse(true);//this is must!!
						HandlePlayResponse(m_rtspCMDString);
						break;
					}
				}
				if(IsPlayResponse==false)
				{
					CLog::Log.MakeLog(LL_RTSP_ERR,"tcp_error.txt","can not get the play response\n");
				}
				responseLine  = m_rtspCMDString;
#endif//ASYNC_TCP
			}
			else
			{	
				if(m_bSyncPlayResponse)//only workaround for Server: CarbonStreamingServer
				{
					if(RecvResponseFromStreamingServerOverHttpTunnel(responseBytes,VOS_CMD_PLAY)) 
					{
						SLOG1(LL_SOCKET_ERR,"rtsp.txt","workaround for %s\n","Server: CarbonStreamingServer");

						HandlePlayResponse(m_recvBuf);
					}
				}
				else
					WaitAndProcessResponse(VOS_CMD_PLAY);
			}
		}
		else
		{
			if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
				break;

			unsigned int responseBytes;
			if(CUserOptions::UserOptions.m_useTCP>0)
			{
				//m_streamLength = 0;
				//m_streamPos = 0;
				SetSocketRecvBufSize(m_rtspSock, 100*1024);
				streamingEngine->AddStreamReader(m_rtspSock, ReadStreamsOverTCP, this);
#if !ASYNC_TCP
				int count = 20;
				bool IsPlayResponse=false;
				while (count-->0)
				{
					IsPlayResponse = false;
					responseBytes = HandleResponseViaTCP();
					int seq_num=GetTheSeqNum(m_rtspCMDString,responseBytes);

					if(seq_num==expectCSEQ)
					{
						IsPlayResponse = true;
						//pRTSPMediaStreamSyncEngine->SetPlayResponse(true);//this is must!!
						HandlePlayResponse(m_rtspCMDString);
						break;
					}
				}
				if(IsPlayResponse==false)
				{
					CLog::Log.MakeLog(LL_RTSP_ERR,"tcp_error.txt","can not get the play response\n");
				}
				responseLine  = m_rtspCMDString;
#endif//ASYNC_TCP
			}
			else
			{	
				if(m_bSyncPlayResponse)//only workaround for Server: CarbonStreamingServer
				{
					if(RecvResponseFromStreamingServer(responseBytes,VOS_CMD_PLAY)) 
					{
						SLOG1(LL_SOCKET_ERR,"rtsp.txt","workaround for %s\n","Server: CarbonStreamingServer");

						HandlePlayResponse(m_recvBuf);
					}
				}
				else
					WaitAndProcessResponse(VOS_CMD_PLAY);
			}
		}

		return true;

	}while(0);

	m_failConnectNum+=5;
	if(responseCode>400)
	{
		m_failConnectNum+=10;//it is an serious error
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "@@@@playcmd fail¡Á¡Á¡ÁresponseCode>400\n");
	}
	if(CUserOptions::UserOptions.m_useTCP>0)
	{
		sprintf(CLog::formatString,"@@@@play fail via TCP %d\n",::GetSocketError());
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", CLog::formatString);
		return true;
	}

	NotifyEvent(VO_EVENT_PLAY_FAIL,CUserOptions::UserOptions.errorID);
	CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt", "@@@@Parse play fail!!\n");
	return false;
}
void CRTSPClientEngine::	WaitAndProcessResponse(int command)
{
	CRealTimeStreamingEngine * streamingEngine = CRealTimeStreamingEngine::CreateRealTimeStreamingEngine();
	streamingEngine->AddStreamReader(m_rtspSock, HandleRTSPServerRequest, this,VOS_STREAM_RTSP);
}
bool CRTSPClientEngine::PauseLiveStream()
{
	return m_rtspSession->PauseLiveStream();
}
bool CRTSPClientEngine::ResumeLiveStream()
{
	return m_rtspSession->ResumeLiveStream();
}
void CRTSPClientEngine::ResetPauseTime()
{
	m_rtspSession->ResetPauseTime();
}
bool CRTSPClientEngine::PauseCmd(int waitResponseBySec)
{
	if(CUserOptions::UserOptions.m_URLType==UT_SDP2_LOCAL)
		return true;

	do
	{
		if(m_rtspSessionID == NULL)
			break;

		CRealTimeStreamingEngine * streamingEngine = CRealTimeStreamingEngine::CreateRealTimeStreamingEngine();
		if(streamingEngine == NULL)
			break;
		streamingEngine->RemoveStreamReader(m_rtspSock);

		strcpy(cmdFormatString,"PAUSE %s RTSP/1.0\r\n"
								 "CSeq: %d\r\n"
								 "Session: %s\r\n"
								 "User-Agent: %s\r\n")
								 ;

		if((CUserOptions::UserOptions.digestIsSet) && (m_digestAuth))
		{
			char method[33] = {0};
			strcpy(method,"PAUSE");

			m_digestAuth->SetMethod(method,strlen(method));
			m_digestAuth->GetResponse(m_digestResp, 32);
			
			sprintf(cmdFormatString, "Authorization: Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\"\r\n",
									CUserOptions::UserOptions.digestAuth.user,
									m_digestRealm,m_digestNonce,m_rtspURL,m_digestResp);
			strcat(m_rtspCMDString,cmdFormatString);
		}
		
		if(CUserOptions::UserOptions.m_bOpenUAProEachRequest&&strlen(m_UAProfile)>2)
		{
			sprintf(m_rtspCMDString,"x-wap-profile: %s\r\n",m_UAProfile);
			strcat(cmdFormatString,m_rtspCMDString);
		}
		
		sprintf(m_rtspCMDString, cmdFormatString, m_URLofRequest, ++m_rtspCSeq, m_rtspSessionID, m_userAgentAttr);
		AddRTSPFields(CUST_PAUSE);
		strcat(m_rtspCMDString,"\r\n");
		m_responsHandler[RR_ID_PAUSE].SetCSEQ(m_rtspCSeq);
		m_responsHandler[RR_ID_PAUSE].SetSendTime(timeGetTime());

		if (CUserOptions::UserOptions.m_isOverHttpTunnel)
		{
			char* FormatString = new char[MAX_TMPLEN];TRACE_NEW("FormatString",FormatString);

			VOLOGI("_____________PAUSECMD____________before encoder=%s", m_rtspCMDString);
			EncodeBase64(m_rtspCMDString, FormatString);
			VOLOGI("_____________PAUSECMD____________after encoder=%s", FormatString);

			if(!SendRTSPCommandToStreamingServer(FormatString,2))
				break;
		}
		else
		{
			if(!SendRTSPCommandToStreamingServer(m_rtspCMDString,2))
				break;
		}

		CUserOptions::UserOptions.status=Session_Paused;
		for(int streamIndex=0; streamIndex<MediaStreamCount(); ++streamIndex)
		{
			//MediaStream(streamIndex)->UnscheduleReceiverReportTask();
			//MediaStream(streamIndex)->retrieveAllPacketsFromRTPBuffer();
		}
		if(m_streamingOverTCP)
		{
			VOLOGI("_________________ReadStreamsOverTCP_____________");
			if (CUserOptions::UserOptions.m_isOverHttpTunnel)
			{
				streamingEngine->AddStreamReader(m_rtspSock2, ReadStreamsOverTCP, this);
			}
			else
			{
				streamingEngine->AddStreamReader(m_rtspSock, ReadStreamsOverTCP, this);
			}
#if !ASYNC_TCP
			responseBytes = HandleResponseViaTCP();
			responseLine  = m_rtspCMDString;
#endif//ASYNC_TCP
		}
		else
		{
 			if(!waitResponseBySec)
				streamingEngine->AddStreamReader(m_rtspSock, HandleRTSPServerRequest, this,VOS_STREAM_RTSP);
			else
			{
				unsigned int responseBytes;
				if (CUserOptions::UserOptions.m_isOverHttpTunnel)
				{
					if(!RecvResponseFromStreamingServerOverHttpTunnel(responseBytes,VOS_CMD_PAUSE, waitResponseBySec)) 
						break;
				}
				else
				{
					if(!RecvResponseFromStreamingServer(responseBytes,VOS_CMD_PAUSE,waitResponseBySec)) 
						break;
				}

				char * responseLine = m_recvBuf;
				unsigned int responseCode = 0;
				if(!ParseResponseCode(responseLine, responseCode))
					break;

				if(responseCode != 200)
				{
					//CUserOptions::UserOptions.errorID = E_USER_ERROR_BASE-responseCode;
					break;
				}
			}
 		}
		return true;

	}while(0);
	
	
	if(CUserOptions::UserOptions.m_useTCP>0)//do not process the response if there is error in tcp enviroment
	{
		sprintf(CLog::formatString,"@@@@pause fail via TCP %d\n",GetSocketError());
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", CLog::formatString);
		return true;
	}
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "@@@@Parse pause fail!!\n");
	return false;
}
#ifdef HTC_PLAYLIST
bool CRTSPClientEngine::Playlist_PlayCmd(char* strPlayList)
{
	if(!CUserOptions::UserOptions.m_bEnablePlaylistSupport)
		return false;
	if(CUserOptions::UserOptions.m_URLType==UT_SDP2_LOCAL)
		return true;

	//if(CUserOptions::UserOptions.m_nSupportPause==0)
	//	return true;

	unsigned int responseCode = 0;
	//DoNotifyStreamingServerClientBeLiving();
	do
	{
		if(m_rtspSessionID == NULL)
			break;

		CRealTimeStreamingEngine * streamingEngine = CRealTimeStreamingEngine::CreateRealTimeStreamingEngine();
		if(streamingEngine == NULL)
			break;
		streamingEngine->RemoveStreamReader(m_rtspSock);

		char * cmdFormatString = "PLAYLIST_PLAY %s RTSP/1.0\r\n"
			"CSeq: %d\r\n"
			"Session: %s\r\n"
			"Range: %s\r\n"
			"\r\n";

		sprintf(m_rtspCMDString, cmdFormatString, m_URLofRequest, ++m_rtspCSeq, m_rtspSessionID, strPlayList);

#ifdef HTC_PLAYLIST
		AddRTSPFields(CUST_PLAYLIST_PLAY);
#endif //HTC_PLAYLIST

		if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
			break;

		unsigned int responseBytes;
		char * responseLine;
		if(CUserOptions::UserOptions.m_useTCP>0)
		{
			streamingEngine->AddStreamReader(m_rtspSock, ReadStreamsOverTCP, this);
			//CUserOptions::UserOptions.debugFlag = 111;
			responseBytes = HandleResponseViaTCP();
			responseLine  = m_rtspCMDString;
		}
		else
		{	
			if(!RecvResponseFromStreamingServer(responseBytes)) 
				break;
			responseLine = m_recvBuf;
		}
		if(CUserOptions::UserOptions.m_useTCP>0)
		{
			CUserOptions::UserOptions.debugFlag = 0;
		}
		char * nextResponseLine = GetNextLine(responseLine);

		if(!ParseResponseCode(responseLine, responseCode))
			break;

		if(m_pICustomNotify != NULL)
		{
			m_pICustomNotify->PlayListPlayResponseNotify(responseCode);
		}

		if(responseCode != 200)
		{
			CUserOptions::UserOptions.errorID = E_USER_ERROR_BASE-responseCode;
			break;
		}

		if(strstr(strPlayList, "time=now") != NULL)
		{
			FlushRTSPStreams();
		}

		//CUserOptions::UserOptions.m_nSupportPause = 1;
		if(m_streamingOverTCP)
		{
			//m_streamLength = 0;
			//m_streamPos = 0;

			streamingEngine->AddStreamReader(m_rtspSock, ReadStreamsOverTCP, this);
		}
		else
		{
			streamingEngine->AddStreamReader(m_rtspSock, HandleRTSPServerRequest, this,VOS_STREAM_RTSP);
		}

		for(int streamIndex=0; streamIndex<MediaStreamCount(); ++streamIndex)
		{
			MediaStream(streamIndex)->UnscheduleReceiverReportTask();
		}
		m_pauseTime = timeGetTime();
		return true;

	}while(0);
	m_failConnectNum+=5;
	if(responseCode>400)
	{
		NotifyEvent(VO_EVENT_PLAY_FAIL,CUserOptions::UserOptions.errorID);
		//NotifyEvent(VO_EVENT_SOCKET_ERR,CUserOptions::UserOptions.errorID);
		m_failConnectNum+=10;//it is an serious error

		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "@@@@Parse Playlist fail  responseCode>400\n");
	}

	if(CUserOptions::UserOptions.m_useTCP>0)//do not process the response if there is error in tcp enviroment
	{
		sprintf(CLog::formatString,"@@@@pause fail via TCP %d\n",WSAGetSocketError());
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", CLog::formatString);
		return true;
	}
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "@@@@Parse Playlist fail  !!\n");
	return false;
}
#endif //HTC_PLAYLIST
bool CRTSPClientEngine::TeardownCmd()
{
	if(CUserOptions::UserOptions.m_URLType==UT_SDP2_LOCAL)
		return true;
	do
	{
		if(m_rtspSessionID == NULL)
			break;

		strcpy(cmdFormatString, "TEARDOWN %s RTSP/1.0\r\n"
								 "CSeq: %d\r\n"
								 "Session: %s\r\n"
								 "User-Agent: %s\r\n")
								 ;

		if(CUserOptions::UserOptions.m_bOpenUAProEachRequest&&strlen(m_UAProfile)>2)
		{
			sprintf(m_rtspCMDString,"x-wap-profile: %s\r\n",m_UAProfile);
			strcat(cmdFormatString,m_rtspCMDString);
		}
		strcat(cmdFormatString,"\r\n");

		sprintf(m_rtspCMDString, cmdFormatString, m_URLofRequest, ++m_rtspCSeq, m_rtspSessionID, m_userAgentAttr);
		
		if(CUserOptions::UserOptions.m_isOverHttpTunnel)
		{
			char* FormatString = new char[MAX_TMPLEN];TRACE_NEW("FormatString",FormatString);

			VOLOGI("_____________PAUSECMD____________before encoder=%s", m_rtspCMDString);
			EncodeBase64(m_rtspCMDString, FormatString);
			VOLOGI("_____________PAUSECMD____________after encoder=%s", FormatString);

			if(!SendRTSPCommandToStreamingServer(FormatString,500))
				break;

			if(CUserOptions::UserOptions.m_nWaitForTeardown)
			{
				unsigned int responseBytes;
				if(!RecvResponseFromStreamingServerOverHttpTunnel(responseBytes,VOS_CMD_TEARDOWN)) 
					break;
			}
		}
		else
		{
			if(!SendRTSPCommandToStreamingServer(m_rtspCMDString,500))
				break;

			if(CUserOptions::UserOptions.m_nWaitForTeardown)
			{
				unsigned int responseBytes;
				if(!RecvResponseFromStreamingServer(responseBytes,VOS_CMD_TEARDOWN)) 
					break;
			}
		}

/*
		char * responseLine = m_recvBuf;
		char * nextResponseLine = GetNextLine(responseLine);
		unsigned int responseCode = 0;
		if(!ParseResponseCode(responseLine, responseCode))
			break;

		if(responseCode != 200)
			break;
*/
		//SAFE_DELETE_ARRAY(m_rtspSessionID);
		return true;

	}while(0);

	//SAFE_DELETE_ARRAY(m_rtspSessionID);
	//CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "@@@@Parse teardown fail!!\n");
	return false;
}

bool CRTSPClientEngine::Get_Parameter_Cmd()
{
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "@@@@Parse Get_Parameter fail!!\n");
	return false;
}

bool CRTSPClientEngine::Set_Parameter_Cmd(int paramType,void* value)
{
	if(CUserOptions::UserOptions.m_URLType==UT_SDP2_LOCAL)
		return true;
	char* rtspURL = CUserOptions::UserOptions.m_currURL;
	do
	{
		if(!Init())
			break;

		if(!ConnectToStreamingServer(rtspURL))
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "SET_PARAMETER:Can not connect streaming server! \r\n");
			break;
		}
		strcpy(cmdFormatString, "SET_PARAMETER %s RTSP/1.0\r\n"
			"CSeq: %d\r\n"
			"User-Agent: %s\r\n")
			;

		if(CUserOptions::UserOptions.m_bOpenUAProEachRequest&&strlen(m_UAProfile)>2)
		{
			sprintf(m_rtspCMDString,"x-wap-profile: %s\r\n",m_UAProfile);
			strcat(cmdFormatString,m_rtspCMDString);
		}
		strcat(cmdFormatString,"\r\n");

		sprintf(m_rtspCMDString, cmdFormatString, rtspURL, ++m_rtspCSeq, m_userAgentAttr);
		if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
			break;

		unsigned int responseBytes;
		if(!RecvResponseFromStreamingServer(responseBytes,VOS_CMD_SET_PARAMETER)) 
			break;

		char * responseLine = m_recvBuf;
		unsigned int responseCode = 0;
		if(!ParseResponseCode(responseLine, responseCode))
			break;

		if(responseCode != 200)
		{
			CUserOptions::UserOptions.errorID = E_USER_ERROR_BASE-responseCode;
			break;
		}
		return true;

	}while(0);
	//m_failConnectNum++;
	NotifyEvent(VO_EVENT_OPTION_FAIL,CUserOptions::UserOptions.errorID);
	CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt", "@@@@Parse SET_PARAMETER fail!!\n");
	return false;
}

bool CRTSPClientEngine::RTSPStreamsAvailable(unsigned int millisecond)
{
	CRealTimeStreamingEngine * realTimeStreamingEngine = CRealTimeStreamingEngine::CreateRealTimeStreamingEngine();
	return realTimeStreamingEngine->StreamsReachable(millisecond);
}

int CRTSPClientEngine::ReadRTSPStreams(int flag)
{
	CRealTimeStreamingEngine * realTimeStreamingEngine = CRealTimeStreamingEngine::CreateRealTimeStreamingEngine();
	return realTimeStreamingEngine->ReadRealTimeStreams(flag);
}

void CRTSPClientEngine::FlushRTSPStreams(int flag)
{
	if(m_rtspSession == NULL)
		return;

	for(int streamIndex=0; streamIndex<m_rtspSession->MediaStreamCount(); ++streamIndex)
	{
		m_rtspSession->MediaStream(streamIndex)->FlushMediaStream(flag);
	}
	
}

int	 CRTSPClientEngine::GetActualBitrate()
{
	int bitrate=0;
	for(int streamIndex=0; streamIndex<m_rtspSession->MediaStreamCount(); ++streamIndex)
	{
		bitrate+=m_rtspSession->MediaStream(streamIndex)->GetActualBitrate();
	}
	sprintf(CLog::formatString,"actual bitrate=%d\n",bitrate);
	CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", CLog::formatString);
	return bitrate;
}
unsigned long CRTSPClientEngine::HaveBufferedRTSPStreamDuration(int plusAntijitterTime)
{
	if(m_rtspSession == NULL)
		return 0;
	
	//get the buffered video time if there are a/v
	unsigned long dwHaveBufferedRtspStreamDuration = CUserOptions::UserOptions.GetMediaBufferTime()+5000;
	
	dwHaveBufferedRtspStreamDuration=0;
	for(int streamIndex=0; streamIndex<m_rtspSession->MediaStreamCount(); ++streamIndex)
	{
		CMediaStream * pMediaStream = MediaStream(streamIndex);
		if(pMediaStream->IsAudioStream())
		{
			if(pMediaStream->HaveBufferedMediaFrameCount()<(pMediaStream->GetFrameRate()/2))
				return 0;
			else
				return pMediaStream->HaveBufferedMediaStreamTime(plusAntijitterTime);
		}
		else
			dwHaveBufferedRtspStreamDuration = pMediaStream->HaveBufferedMediaStreamTime(plusAntijitterTime);
	}

	return dwHaveBufferedRtspStreamDuration;
}
DWORD	CRTSPClientEngine::ConnectThreadProc(void* pParam)
{
	CRTSPClientEngine * pEngine = (CRTSPClientEngine *)pParam;
	return pEngine->ConnectToServer();
}
//bool	CRTSPClientEngine::	ConnectToServer()
// {
//	char* rtspURL=m_oriURL;
//	m_rtspURL = StringDup(rtspURL);
//	CUserOptions::UserOptions.errorID = 0;
//	
//	do
//	{
//		const char * protocolName = "rtsp://";
//		unsigned protocolNameLen = 7;
//		if (_strnicmp(rtspURL, protocolName, protocolNameLen) != 0) 
//		{
//			CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "invalid url syntax\r\n");
//			CUserOptions::UserOptions.errorID = E_HOST_UNKNOWN;
//			break;
//		}
//
//		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "$$$ConnectToServer:check rtspURL1 \r\n");
//
//		char * p = rtspURL + protocolNameLen;
//		char * p1 = strstr(p, "/");
//		if(p1 == NULL )
//		{
//			CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "invalid url syntax2\r\n");
//			CUserOptions::UserOptions.errorID = E_HOST_UNKNOWN;
//			break;
//		}
//		
//		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "$$$ConnectToServer:check rtspURL2 \r\n");
//		// parse streaming server address
//		char rtspServerHostName[MAX_PATH] = {0};
//		char * to = rtspServerHostName;
//		while(p < p1)
//		{
//			if(*p == ':') 
//				break;
//
//			*to++ = *p++;
//		}
//		*to = '\0';
//
//		// parse streaming server port
//		unsigned short serverPort = 554;
//		int port;
//		if(*p == ':')                 
//		{
//			if(sscanf(++p, "%u", &port) != 1)
//			{
//				CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "can not parse port\r\n");
//				CUserOptions::UserOptions.errorID =E_PORT_ALLOC;
//				break;
//			}
//			if(port < 1 || port > 65535) 
//			{
//				CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "invalid port\r\n");
//				CUserOptions::UserOptions.errorID =E_PORT_ALLOC;
//				break;
//			}
//			serverPort = (unsigned short )port;
//		}
//
//		if(CUserOptions::UserOptions.m_bUseRTSPProxy
//			&&strstr(rtspURL,"://127.0.0.1")==NULL&&strstr(rtspURL,"://localhost")==NULL)
//		{
//		
//			
//			char* start=strstr(CUserOptions::UserOptions.m_szProxyIP,"://");
//			if(start==NULL)
//				start=CUserOptions::UserOptions.m_szProxyIP;
//			else
//				start+=3;
//			char* strPort=strstr(start,":");
//			serverPort=554;
//			if(strPort)
//			{
//				if(sscanf(strPort,":%u", &port)==1)
//				{
//						serverPort = port;
//				}
//				strPort[0]=0;
//			}
//			strcpy(rtspServerHostName,start);
//			sprintf(CLog::formatString,"!!!!this is Proxy:%s,port=%d\n",rtspServerHostName,serverPort);
//			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
//		}
//
//		m_rtspServerAddr->family = VAF_UNSPEC;
//		m_rtspServerAddr->port   = HostToNetShort(serverPort);
//		m_rtspServerAddr->protocol = VPT_TCP;
//		m_rtspServerAddr->sockType = VST_SOCK_STREAM;
//		int ret=IVOSocket::GetPeerAddressByURL(rtspServerHostName,strlen(rtspServerHostName),m_rtspServerAddr);
//		if(ret)
//		{
//			CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "getaddrinfo fail\r\n");
//			CUserOptions::UserOptions.errorID = E_HOST_UNKNOWN;
//			break;
//		}
//		CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", "GetPeerAddressByURL\n");
//		m_rtspSock = VOCPFactory::CreateOneSocket( VST_SOCK_STREAM,m_rtspServerAddr->protocol,m_rtspServerAddr->family);
//		TRACE_NEW("m_rtspSock",m_rtspSock);
//		if(m_rtspSock==NULL||m_rtspSock->IsValidSocket()==false)
//		{
//			sprintf(CLog::formatString," StreamSocket fail err=%d\n",GetSocketError());
//			CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", CLog::formatString);
//			break;
//		}
//		//CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", "CreateOneSocket\n");
//		int timeout=CUserOptions::UserOptions.m_nConnectTimeOut; 
//		if(CUserOptions::UserOptions.m_bMakeLog)
//		{
//			sprintf(CLog::formatString,"Address:%d,%ld,%d,%d,%d,%d,%d\n",
//											m_rtspServerAddr->family,
//											m_rtspServerAddr->ipAddress,
//											serverPort,//m_rtspServerAddr->port,
//											m_rtspServerAddr->protocol,
//											m_rtspServerAddr->sockType,
//											m_rtspServerAddr->userData1,
//											m_rtspServerAddr->userData2);
//			CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", CLog::formatString);
//		}
//		ret=0;
//		do 
//		{
//			CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", "Try to connect...\n");
//
//			ret=m_rtspSock->Connect(*m_rtspServerAddr,1000);
//
//			if(ret== SOCKET_ERROR) 
//			{
//				int err = GetSocketError();
//				char* format = CLog::formatString;
//				sprintf(format,"@@@@connect function fail err=%d\r\n",err);
//				CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", format);
//				CUserOptions::UserOptions.errorID = E_CONNECTION_RESET;
//				break;
//			}
//			if(CUserOptions::UserOptions.m_exit)//user stop it
//			{
//				CLog::Log.MakeLog(LL_SOCKET_ERR, "rtsp.txt", "user stops the socket\n");
//				timeout=0;
//				break;//return false;
//			}
//		} while(--timeout>0&&ret==1);
//		
//		if(timeout==0||ret== SOCKET_ERROR)//timeout
//		{
//			VOCPFactory::DeleteOneSocket(m_rtspSock);TRACE_DELETE(m_rtspSock);
//			m_rtspSock=INVALID_SOCKET;
//			char* format = CLog::formatString;
//			sprintf(format,"@@@@connect function timeout=%d,%d,ret=%d,err=%d\r\n",timeout,CUserOptions::UserOptions.m_nRTSPTimeOut,ret,GetSocketError());
//			CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", format);
//			break;
//		}
//		CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", "ConnectToServer Done\n");
//		return true;
//
//	}while(0);
//	m_rtspSock = INVALID_SOCKET;
//	CLog::Log.MakeLog(LL_SOCKET_ERR, "rtsp.txt", "@@@@@ConnectToServer() fail\r\n");
//	return false;
//}

bool	CRTSPClientEngine::	ConnectToServer()
{
	char* rtspURL=m_oriURL;
	m_rtspURL = StringDup(rtspURL);
	CUserOptions::UserOptions.errorID = 0;

//#define HTTPTUNNELPORT 32000

	do
	{
		const char * protocolName = "rtsp://";
		unsigned protocolNameLen = 7;
		if (_strnicmp(rtspURL, protocolName, protocolNameLen) != 0) 
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "invalid url syntax\r\n");
			CUserOptions::UserOptions.errorID = E_HOST_UNKNOWN;
			break;
		}

		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "$$$ConnectToServer:check rtspURL1 \r\n");

		char * p = rtspURL + protocolNameLen;
		char * p1 = strstr(p, "/");
		if(p1 == NULL )
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "invalid url syntax2\r\n");
			CUserOptions::UserOptions.errorID = E_HOST_UNKNOWN;
			break;
		}
		
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "$$$ConnectToServer:check rtspURL2 \r\n");
		// parse streaming server address
		char rtspServerHostName[MAX_PATH] = {0};
		char * to = rtspServerHostName;
		while(p < p1)
		{
			if(*p == ':') 
				break;

			*to++ = *p++;
		}
		*to = '\0';

		// parse streaming server port
		unsigned short serverPort;
		if(CUserOptions::UserOptions.m_isOverHttpTunnel)
		{
			serverPort = CUserOptions::UserOptions.m_rtspOverHttpConnectionPort;
		}
		else
		{
			serverPort = 554;
		}

		int port;
		if(*p == ':')                 
		{
			if(sscanf(++p, "%u", &port) != 1)
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "can not parse port\r\n");
				CUserOptions::UserOptions.errorID =E_PORT_ALLOC;
				break;
			}
			if(port < 1 || port > 65535) 
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "invalid port\r\n");
				CUserOptions::UserOptions.errorID =E_PORT_ALLOC;
				break;
			}
			serverPort = (unsigned short )port;
		}

		if(CUserOptions::UserOptions.m_bUseRTSPProxy
			&&strstr(rtspURL,"://127.0.0.1")==NULL&&strstr(rtspURL,"://localhost")==NULL)
		{


			char* start=strstr(CUserOptions::UserOptions.m_szProxyIP,"://");
			if(start==NULL)
				start=CUserOptions::UserOptions.m_szProxyIP;
			else
				start+=3;
			char* strPort=strstr(start,":");

			if(CUserOptions::UserOptions.m_isOverHttpTunnel)
			{
				serverPort=CUserOptions::UserOptions.m_rtspOverHttpConnectionPort;
			}
			else
			{
				serverPort=554;
			}

			if(strPort)
			{
				if(sscanf(strPort,":%u", &port)==1)
				{
					serverPort = port;
				}
				strPort[0]=0;
			}
			strcpy(rtspServerHostName,start);
			sprintf(CLog::formatString,"!!!!this is Proxy:%s,port=%d\n",rtspServerHostName,serverPort);
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
		}

		if(CUserOptions::UserOptions.m_isOverHttpTunnel)
		{
			serverPort = CUserOptions::UserOptions.m_rtspOverHttpConnectionPort;
		}

		m_rtspServerAddr->family = VAF_UNSPEC;
		m_rtspServerAddr->port   = HostToNetShort(serverPort);
		m_rtspServerAddr->protocol = VPT_TCP;
		m_rtspServerAddr->sockType = VST_SOCK_STREAM;
		int ret=IVOSocket::GetPeerAddressByURL(rtspServerHostName,strlen(rtspServerHostName),m_rtspServerAddr);
		if(ret)
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "getaddrinfo fail\r\n");
			CUserOptions::UserOptions.errorID = E_HOST_UNKNOWN;
			break;
		}
		CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", "GetPeerAddressByURL\n");
		m_rtspSock = VOCPFactory::CreateOneSocket( VST_SOCK_STREAM,m_rtspServerAddr->protocol,m_rtspServerAddr->family);
		TRACE_NEW("m_rtspSock",m_rtspSock);
		if(m_rtspSock==NULL||m_rtspSock->IsValidSocket()==false)
		{
			sprintf(CLog::formatString," StreamSocket fail err=%d\n",GetSocketError());
			CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", CLog::formatString);
			break;
		}
		//CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", "CreateOneSocket\n");
		int timeout=CUserOptions::UserOptions.m_nConnectTimeOut; 
		if(CUserOptions::UserOptions.m_bMakeLog)
		{
			sprintf(CLog::formatString,"Address:%d,%ld,%d,%d,%d,%d,%d\n",
				m_rtspServerAddr->family,
				m_rtspServerAddr->ipAddress,
				serverPort,//m_rtspServerAddr->port,
				m_rtspServerAddr->protocol,
				m_rtspServerAddr->sockType,
				m_rtspServerAddr->userData1,
				m_rtspServerAddr->userData2);
			CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", CLog::formatString);
		}
		ret=0;
		do 
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", "Try to connect...\n");

			ret=m_rtspSock->Connect(*m_rtspServerAddr,1000);

			if(ret== SOCKET_ERROR) 
			{
				int err = GetSocketError();
				char* format = CLog::formatString;
				sprintf(format,"@@@@connect function fail err=%d\r\n",err);
				CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", format);
				CUserOptions::UserOptions.errorID = E_CONNECTION_RESET;
				break;
			}
			if(CUserOptions::UserOptions.m_exit)//user stop it
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR, "rtsp.txt", "user stops the socket\n");
				timeout=0;
				break;//return false;
			}
		} while(--timeout>0&&ret==1);

		if(timeout==0||ret== SOCKET_ERROR)//timeout
		{
			VOCPFactory::DeleteOneSocket(m_rtspSock);TRACE_DELETE(m_rtspSock);
			m_rtspSock=INVALID_SOCKET;
			char* format = CLog::formatString;
			sprintf(format,"@@@@connect function timeout=%d,%d,ret=%d,err=%d\r\n",timeout,CUserOptions::UserOptions.m_nRTSPTimeOut,ret,GetSocketError());
			CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", format);
			break;
		}
		CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", "ConnectToServer Done\n");
		return true;

	}while(0);
	m_rtspSock = INVALID_SOCKET;
	CLog::Log.MakeLog(LL_SOCKET_ERR, "rtsp.txt", "@@@@@ConnectToServer() fail\r\n");
	return false;
}

bool CRTSPClientEngine::ConnectToStreamingServer(char * rtspURL)
{
	
	if(m_rtspSock != INVALID_SOCKET)
		return true;
	//strcpy(rtspURL,"rtsp://10.2.64.11/matrix12.mp4");
	m_URLofRequest=m_oriURL = rtspURL;
	m_bTimeOut = false;
	
	ConnectToServer();

	if(m_rtspSock == INVALID_SOCKET)
	{
		
		if(CUserOptions::UserOptions.errorID==0)
			CUserOptions::UserOptions.errorID = E_CONNECTION_FAILED;
		NotifyEvent(VO_EVENT_CONNECT_FAIL,CUserOptions::UserOptions.errorID);

	}

	return m_rtspSock != INVALID_SOCKET;
}

bool CRTSPClientEngine::ConnectToStreamingServerOverHttpTunnel(char * rtspURL)
{

	if(m_rtspSock2 != INVALID_SOCKET)
		return true;
	//strcpy(rtspURL,"rtsp://10.2.64.11/matrix12.mp4");
	m_URLofRequest=m_oriURL = rtspURL;
	m_bTimeOut = false;

	ConnectToServerOverHttpTunnel();

	if(m_rtspSock2 == INVALID_SOCKET)
	{

		if(CUserOptions::UserOptions.errorID==0)
			CUserOptions::UserOptions.errorID = E_CONNECTION_FAILED;
		NotifyEvent(VO_EVENT_CONNECT_FAIL,CUserOptions::UserOptions.errorID);

	}

	return m_rtspSock2 != INVALID_SOCKET;
}

bool CRTSPClientEngine::SendRTSPCommandToStreamingServer(const char * cmdString,int timeOutBySec)
{
	VOLOGI("_________________________SendRTSPCommandToStreamingServer=%s", cmdString);

#if 1
	int result=IsNetReadyToSend(m_rtspSock,timeOutBySec);//if it is disable to send data in 3 seconds,there is something wrong in the network
	if(result<=0)
	{
		CUserOptions::UserOptions.errorID = E_RTSP_SEND_FAILED;
		CLog::Log.MakeLog(LL_SOCKET_ERR,"socket_timeout.txt","@@@@time out to send the RTSP request\n");
		return false;
	}
#endif
	//if(::SendTCPData(m_rtspSock, cmdString, strlen(cmdString), 0) == VEC_SOCKET_ERROR) zxs 2012-6-21

#ifdef _LINUX_ANDROID
#define	FLAG  0x4000
#else
#define FLAG 0
#endif

	if(::SendTCPData(m_rtspSock, cmdString, strlen(cmdString), FLAG) == VEC_SOCKET_ERROR)
	{
		m_errorCount++;
		CUserOptions::UserOptions.errorID = E_RTSP_SEND_FAILED;

		int err = GetSocketError();
		char* format = CLog::formatString;
		sprintf(format,"@@@@@@@@@@@@fail to SendRTSPCommandToStreamingServer err=%d\r\n",err);
		CLog::Log.MakeLog(LL_SOCKET_ERR,"socketErr.txt",format);
		return false;
	}
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", cmdString);
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "\r\n");

	return true;
}


bool CRTSPClientEngine::SendRTSPCommandToStreamingServerOverHttpTunnel(const char * cmdString,int timeOutBySec)
{

#if 1
	int result=IsNetReadyToSend(m_rtspSock2,timeOutBySec);//if it is disable to send data in 3 seconds,there is something wrong in the network
	if(result<=0)
	{
		CUserOptions::UserOptions.errorID = E_RTSP_SEND_FAILED;
		CLog::Log.MakeLog(LL_SOCKET_ERR,"socket_timeout.txt","@@@@time out to send the RTSP request\n");
		return false;
	}
#endif
	//if(::SendTCPData(m_rtspSock, cmdString, strlen(cmdString), 0) == VEC_SOCKET_ERROR) zxs 2012-6-21

#ifdef _LINUX_ANDROID
#define	FLAG  0x4000
#else
#define FLAG 0
#endif

	if(::SendTCPData(m_rtspSock2, cmdString, strlen(cmdString), FLAG) == VEC_SOCKET_ERROR)
	{
		m_errorCount++;
		CUserOptions::UserOptions.errorID = E_RTSP_SEND_FAILED;

		int err = GetSocketError();
		char* format = CLog::formatString;
		sprintf(format,"@@@@@@@@@@@@fail to SendRTSPCommandToStreamingServerOverHttpTunnel err=%d\r\n",err);
		CLog::Log.MakeLog(LL_SOCKET_ERR,"socketErr.txt",format);
		return false;
	}
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", cmdString);
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "\r\n");

	return true;
}



int CRTSPClientEngine::HandleResponseViaTCP()
{
	CLog::Log.MakeLog(LL_SOCKET_ERR,"tcp.txt", "before handle\n");
	m_rtspCMDString[10]='\0';
	DoReadStreamsOverTCP(GET_RTSP_RESPONSE_VIATCP);
	int responseBytes  = strlen(m_rtspCMDString);
	char * p = m_rtspCMDString + responseBytes - 4;
	if(responseBytes>10&&*p == '\r' && *(p+1) == '\n' && *(p+2) == '\r' && *(p+3) == '\n')
	{
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "//Get the response from TCP channel\n");
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", m_rtspCMDString);
	}
	else
	{
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@HandleResponseViaTCP() error\n");
	}
	return responseBytes;
}
bool CRTSPClientEngine::RecvResponseFromStreamingServer(unsigned int & responseBytes,int cmd,int timeoutBySec)
{
	if (cmd==VOS_CMD_GET)
	{
		VOLOGI("________rtsp_____GET____________receive packets from server_________________________");
	}
	if (cmd==VOS_CMD_POST)
	{
		VOLOGI("________rtsp_____POST____________receive packets from server_________________________");
	}

	responseBytes = 0;
	m_recvBuf[0] = '\0';
	if(cmd!=VOS_CMD_UNKNOWN)
	{
		int count = timeoutBySec==0?CUserOptions::UserOptions.m_nRTSPTimeOut:timeoutBySec;//cmd==VOS_CMD_PLAY?CuserOptions::UserOptions.m_nRTSPTimeOut:CuserOptions::UserOptions.m_nRTSPTimeOut/4;
		if(cmd == VOS_CMD_TEARDOWN)
			count = 1;
		
		int result;
#define TRY_TIME_RTSP 1
		do 
		{
			VOLOGI("____________________m_rtspSock = %x, cmd  = %d", m_rtspSock, cmd);
			result=IsNetReadyToRead(m_rtspSock,TRY_TIME_RTSP);
			if(result!=0)//error happens(<0) or has got the result(>0)
				break;
			count-=TRY_TIME_RTSP;
			if((CUserOptions::UserOptions.m_exit) && (cmd != VOS_CMD_TEARDOWN))//user stop it
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR, "rtsp.txt", "user stops the RecvResponse\n");
				return false;
			}
		} while(count>0);
		//if there is no data coming in m_nRTSPTimeOut seconds,there is something wrong in the network
		if(result<=0)
		{
			VOLOGI("_______________RecvResponseFromStreamingServer______________________, result = %d", result);

			CUserOptions::UserOptions.errorID = E_NETWORK_TIMEOUT;
			sprintf(CLog::formatString,"@@@@time out to get the RTSP response,cmd=%d\n",cmd);
			CLog::Log.MakeLog(LL_SOCKET_ERR,"socket_timeout.txt",CLog::formatString);
			return false;
		}
	}

	while(responseBytes < RECVBUFSIZE)
	{
		
		int readBytes = ReadNetData(m_rtspSock, (unsigned char *)(m_recvBuf + responseBytes), 1);
		if(readBytes <= 0) 
			break;

		responseBytes += readBytes;
		//recv response over after recv "\r\n\r\n"
		int offset2 = responseBytes - 4;
		if(offset2 < 0) 
		{
			continue;
		}
		char * p = m_recvBuf + offset2;
		

		if(*p == '\r' && *(p+1) == '\n' && *(p+2) == '\r' && *(p+3) == '\n') 
		{
			m_recvBuf[responseBytes] = '\0';

			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", m_recvBuf);
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "\r\n");

			return true;
		}
	}

	return false;
}



bool CRTSPClientEngine::RecvResponseFromStreamingServerOverHttpTunnel(unsigned int & responseBytes,int cmd,int timeoutBySec)
{
	if (cmd==VOS_CMD_GET)
	{
		VOLOGI("________rtsp_____GET____________receive packets from server_________________________");
	}
	if (cmd==VOS_CMD_POST)
	{
		VOLOGI("________rtsp_____POST____________receive packets from server_________________________");
	}

	responseBytes = 0;
	m_recvBuf[0] = '\0';
	if(cmd!=VOS_CMD_UNKNOWN)
	{
		int count = timeoutBySec==0?CUserOptions::UserOptions.m_nRTSPTimeOut:timeoutBySec;//cmd==VOS_CMD_PLAY?CuserOptions::UserOptions.m_nRTSPTimeOut:CuserOptions::UserOptions.m_nRTSPTimeOut/4;
		if(cmd == VOS_CMD_TEARDOWN)
			count = 1;

		int result;
#define TRY_TIME_RTSP 1
		do 
		{
			VOLOGI("____________________m_rtspSock2 = %x, cmd  = %d", m_rtspSock, cmd);
			result=IsNetReadyToRead(m_rtspSock2,TRY_TIME_RTSP);
			if(result!=0)//error happens(<0) or has got the result(>0)
				break;
			count-=TRY_TIME_RTSP;
			if((CUserOptions::UserOptions.m_exit) && (cmd != VOS_CMD_TEARDOWN))//user stop it
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR, "rtsp.txt", "user stops the RecvResponse\n");
				return false;
			}
		} while(count>0);
		//if there is no data coming in m_nRTSPTimeOut seconds,there is something wrong in the network
		if(result<=0)
		{
			VOLOGI("_______________RecvResponseFromStreamingServer______________________, result = %d", result);

			CUserOptions::UserOptions.errorID = E_NETWORK_TIMEOUT;
			sprintf(CLog::formatString,"@@@@time out to get the RTSP response2,cmd=%d\n",cmd);
			CLog::Log.MakeLog(LL_SOCKET_ERR,"socket_timeout.txt",CLog::formatString);
			return false;
		}
	}


	VOLOGI("_______________RecvResponseFromStreamingServer2(responseBytes)______________________, responseBytes = %d", responseBytes);

	while(responseBytes < RECVBUFSIZE)
	{
		int readBytes = ReadNetData(m_rtspSock2, (unsigned char *)(m_recvBuf + responseBytes), 1);

		if(readBytes <= 0) 
			break;

		responseBytes += readBytes;
		//recv response over after recv "\r\n\r\n"
		int offset2 = responseBytes - 4;
		if(offset2 < 0) 
		{
			continue;
		}

		//sprintf(CLog::formatString,"readBytes = %d, m_recvBuf = %s\n", readBytes, m_recvBuf);
		//CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);

		char * p = m_recvBuf + offset2;

		if(*p == '\r' && *(p+1) == '\n' && *(p+2) == '\r' && *(p+3) == '\n') 
		{
			m_recvBuf[responseBytes] = '\0';

			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", m_recvBuf);
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "\r\n");

			return true;
		}
	}

	return false;
}



bool CRTSPClientEngine::ParseResponseCode(char * responseText, unsigned int & responseCode)
{
	responseCode = 0;
	return sscanf(responseText, "%*s%u", &responseCode) == 1;
}

bool CRTSPClientEngine::ParsePublicAttribute(char * responseText)
{
	do
	{
		if(_strnicmp(responseText, "Public:", 7) != 0)
			break;

		SAFE_DELETE_ARRAY(m_publicAttr);
		m_publicAttr = StringDup(responseText);
		if(strstr(m_publicAttr,"PAUSE"))
		{
			CUserOptions::UserOptions.m_nSupportPause = 1;
		}
		else
		{
			CUserOptions::UserOptions.m_nSupportPause = 0;
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@ParsePublicAttribute:it doest no support pause\n");
		}
		return true;

	}while(0);
	//CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@ParsePublicAttribute\n");
	return false;
}
//char* teststr="Content-Base: rtsp://10.1.1.4/ettvn.sdp/";//"Content-base: rtsp://139.7.147.67/bx/tx/rx/mobileTVl05.sdp?p=c&m=491732141069&t=1&s=c523tvntv&loginterval=0&iver=3&gtime=1188416249&tokenname=D&VOGUID=343998c03895402a863f080020f9913c&key=4EB9E9F1391039B534F57BE1AA3B1398/";
//char* teststr2="rtsp://10.1.1.4/ettvn.sdp?trans_id=m20070918665572&msisdn=0988252652";//"rtsp://139.7.147.67/bx/tx/rx/mobileTVl05.sdp?p=c&m=491732141069&t=1&s=c523tvntv&loginterval=0&iver=3&gtime=1188416249&tokenname=D&VOGUID=343998c03895402a863f080020f9913c&key=4EB9E9F1391039B534F57BE1AA3B1398";
//responseText = teststr;
//m_rtspURL	= teststr2;
//char*	testStr3 = "Content-Base: rtsp://10.2.64.11/matrix12.mp4?Autograph%20Book/";
bool CRTSPClientEngine::ParseContentBase(char * responseText)
{
	do
	{
		if(_strnicmp(responseText, "Content-Base:", 13) != 0)
			break;
		m_contentBase = StringDup(responseText);
		if(sscanf(responseText, "Content-Base: %s", m_contentBase) != 1 && sscanf(responseText, "Content-base: %s", m_contentBase) != 1)
			break;
		int contentBaseLen = strlen(m_contentBase);

		if(m_contentBase[contentBaseLen- 1] == '/')
			m_contentBase[contentBaseLen - 1] = '\0';

		m_URLofRequest = m_contentBase;//m_rtspURL;
		return true;
	}while(0);
	//CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt","shit\n");
	//CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt",m_contentBase);
	return false;
}

bool CRTSPClientEngine::ParseSetupCMDResponse_Session(char * sdpParam)
{
	do
	{
		if(_strnicmp(sdpParam, "Session: ", 9) != 0)
			break;

		if(m_rtspSessionID)
			SAFE_DELETE_ARRAY(m_rtspSessionID);

		m_rtspSessionID = StringDup(sdpParam);

		char* field = StringDup(sdpParam);
		while(sscanf(sdpParam, "%[^;]", field) == 1)
		{
			if(sscanf(field, "Session: %[^;]", m_rtspSessionID) == 1)
			{
			}
			if(sscanf(field, "timeout=%u", &m_serverTimeoutValue) == 1)
			{
			}

			sdpParam += strlen(field);
			if(sdpParam[0] == '\0') 
				break;

			++sdpParam; // skip over the ';'
		}

		SAFE_DELETE_ARRAY(field);

		return true;

	}while(0);
	//CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@ParseSetupCMDResponse_Sessionn");

	return false;
}

bool CRTSPClientEngine::ParseSetupCMDResponse_Transport(char * sdpParam, int streamIndex)
{
	CMediaStream * pMediaStream = MediaStream(streamIndex);

	do
	{
		if(_strnicmp(sdpParam, "Transport: ", 11) != 0) 
			break;

		sdpParam += 11;

		char transportAttrField[64];
		while(sscanf(sdpParam, "%[^;]", transportAttrField) == 1)
		{
			unsigned short serverRTPPort;
			unsigned short serverRTCPPort;
			if(sscanf(transportAttrField, "server_port=%hu-%hu", &serverRTPPort, &serverRTCPPort) == 2) 
			{
				pMediaStream->SetServerRTPPort(serverRTPPort);
				pMediaStream->SetServerRTCPPort(serverRTCPPort);
			}

			sdpParam += strlen(transportAttrField) + 1;
		}

		return true;

	}while(0);
	//CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@ParseSetupCMDResponse_Transport");
	return false;
}
bool  CRTSPClientEngine::ParsePlayRange(char * responseText,float* begin,float *end)
{
	float rangeBegin, rangeEnd;
	int hhBegin,mmBegin,hhEnd,mmEnd;

	if(sscanf(responseText, "Range: npt=%d:%d:%f - %d:%d:%f",&hhBegin,&mmBegin, &rangeBegin, &hhEnd,&mmEnd, &rangeEnd) == 6)// || sscanf(responseText, "Range: npt:%g - %*g", &rangeBegin) == 1)
	{
		CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();

		rangeBegin = hhBegin*3600+mmBegin*60+rangeBegin;
		rangeEnd = hhEnd*3600+mmEnd*60+rangeEnd;

		pRTSPMediaStreamSyncEngine->SetPlayRangeBegin(rangeBegin);
		if(0)//CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
		{
			sprintf(CLog::formatString,"the begin range time is %f\n",rangeBegin);
			CLog::Log.MakeLog(LL_BUF_STATUS,"flow2.txt",CLog::formatString);
		}

		*begin = rangeBegin;
		*end = rangeEnd;
		return true;
	}
	else if(sscanf(responseText, "Range: npt=%g - %g", &rangeBegin, &rangeEnd) ==2 || sscanf(responseText, "Range: npt:%g - %g", &rangeBegin, &rangeEnd) ==2)
	{
		//a=range:npt=0-  63.44533
		CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
		pRTSPMediaStreamSyncEngine->SetPlayRangeBegin(rangeBegin);

		*begin = rangeBegin;
		*end=rangeEnd;

		return true;
	}
	else if(sscanf(responseText, "Range: npt=%g - %g", &rangeBegin, &rangeEnd) == 1|| sscanf(responseText, "Range: npt:%g - %g", &rangeBegin, &rangeEnd) ==1)
	{
		CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
		pRTSPMediaStreamSyncEngine->SetPlayRangeBegin(rangeBegin);

		*begin = rangeBegin;

		return true;
	}
	else if(sscanf(responseText, "Range: npt=%g-%g", &rangeBegin, &rangeEnd) == 1|| sscanf(responseText, "Range: npt:%g-%g", &rangeBegin, &rangeEnd) ==1)
	{
		CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
		pRTSPMediaStreamSyncEngine->SetPlayRangeBegin(rangeBegin);

		*begin = rangeBegin;

		return true;
	}

	CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse session range\n");

	return false;
}
bool CRTSPClientEngine::ParseRTCPInterval(char * responseText)
{
	do
	{
		if(_strnicmp(responseText, "RTCP-Interval", 13) != 0) 
			break;
		responseText += 13;
		int interval;
		if(sscanf(responseText, ":%d", &interval) == 1)
		{
			CUserOptions::UserOptions.m_nRTCPInterval=interval;
		}
		return true;
	}while(0);
	//CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@ParseSetupCMDResponse_Transport");
	return false;
}
bool CRTSPClientEngine::ParseRTPInfoHeader(char * responseText)
{
	int rtpInfo=0;

	do
	{
		while(*responseText == ' ')
			++responseText;

		if(_strnicmp(responseText, "RTP-Info: ", 10) == 0) 
		{
			responseText += 10;
		}//check if the seq= exist,
		else if(strstr(responseText,"seq="))//(_strnicmp(responseText, "url=", 4) == 0)
		{
		}
		else
		{
			break;
		}

		char * rtpInfoText = responseText;
		char * nextRTPInfoText = NULL;
		while(rtpInfoText != NULL)
		{
			rtpInfoText = strstr(rtpInfoText,"url");
			strSpace(rtpInfoText);
			nextRTPInfoText = strstr(rtpInfoText+3,"url");

			if(nextRTPInfoText != NULL)
			{
				*(nextRTPInfoText-1) = '\0';

				while(*nextRTPInfoText != '\0')
				{
					if(_strnicmp(nextRTPInfoText, "url=", 4) == 0)
						break;

					++nextRTPInfoText;
				}
				if(*nextRTPInfoText == '\0')
					nextRTPInfoText = NULL;
			}
			
			sprintf(CLog::formatString,"@@@Parsing rtp-info...%s\n",rtpInfoText);
			CLog::Log.MakeLog(LL_RTSP_ERR,"rtp-info.txt",CLog::formatString);
			
			for(int streamIndex=0; streamIndex<m_rtspSession->MediaStreamCount(); ++streamIndex)
			{
				if(m_rtspSession->MediaStream(streamIndex)->ParseRTPInfoHeader(rtpInfoText))
				{
					if(m_rtspSession->MediaStream(streamIndex)->IsRTPInfoTimeStampNotExist()==false)
						rtpInfo++;
					break;
				}
			}

			rtpInfoText = nextRTPInfoText;
		}
		
		return rtpInfo>0;

	}while(0);
	
	return false;
}

float CRTSPClientEngine::StreamingDuration() 
{
	return m_rtspSession != NULL ? m_rtspSession->SessionDuration() : 0; 
}

CMediaStream * CRTSPClientEngine::MediaStream(int streamIndex) 
{ 
	return m_rtspSession != NULL ? m_rtspSession->MediaStream(streamIndex) : NULL; 
}

int	CRTSPClientEngine::MediaStreamCount() 
{ 
	return m_rtspSession != NULL ? m_rtspSession->MediaStreamCount() : 0; 
}

int CRTSPClientEngine::HandleRTSPServerRequest(void * param)
{
	CRTSPClientEngine * rtspClientEngine = (CRTSPClientEngine *)param;
	return rtspClientEngine->DoHandleRTSPServerRequest();
}
void	CRTSPClientEngine::HandlePlayResponse(char* responseLine)
{
	do 
	{
		unsigned int responseCode = 0;
		CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
		m_responsHandler[RR_ID_PLAY].Reset();
		char * nextResponseLine = GetNextLine(responseLine);
		//unsigned int responseCode = 0;
		if(!ParseResponseCode(responseLine, responseCode))
			break;
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","HandlePlayResponse\n");
		if(responseCode != 200)
		{
			CUserOptions::UserOptions.errorID = E_USER_ERROR_BASE-responseCode;
			break;
		}
		int maxlineNum=30;
		int hasRTP_info = 0;
		int hasRange = 0;
		float begin = 0.0, end=0.0;
#if TRACE_FLOW1
		CLog::Log.MakeLog(LL_RTSP_ERR,"rtp-info.txt","begin parsing...rtp-info\n");
#endif
		while(maxlineNum-->0) 
		{
			responseLine = nextResponseLine;
			if(responseLine == NULL)
				break;
#if TRACE_FLOW1
			CLog::Log.MakeLog(LL_RTSP_ERR,"rtp-info.txt",responseLine);
#endif

			nextResponseLine = GetNextLine(responseLine);

			if(ParsePlayRange(responseLine,&begin, &end))
			{
				float duration = end-begin;
				
#define INVALID_PLAY_RANGE(a)		(a<0||a>(24*60*60))
				if(!INVALID_PLAY_RANGE(duration) && (m_rtspSession->SessionDuration() == 0))
				{
					m_rtspSession->SetSessionDuration(duration);
					//NotifyEvent(0,0);
				}

				hasRange = 1;
				m_range = begin;
				
				continue;
			}
			if(ParseRTPInfoHeader(responseLine))
			{
				hasRTP_info = 1;
				continue;
			}

			if(ParseRTCPInterval(responseLine))
			{
				continue;
			}
		}
		if(!hasRTP_info||!hasRange)//TODO:LIVE
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","WARNNING:no RTP-info or range in this play response!!!\n");
			for(int streamIndex=0; streamIndex<m_rtspSession->MediaStreamCount(); ++streamIndex)
			{
				m_rtspSession->MediaStream(streamIndex)->ResetRTPInfo();
			}
			pRTSPMediaStreamSyncEngine->SetSyncByRTPInfo(false);
		}
		else
		{
			m_pRTSPSource->SetDiffTimeInPlayResp(1000*GetPlayRangeBegin());

			pRTSPMediaStreamSyncEngine->SetSyncByRTPInfo(true);
		}

		if(m_taskToken == 0)
		{
			m_taskToken = CTaskSchedulerEngine::CreateTaskSchedulerEngine()->ScheduleTask(10*1000, (TaskFunction *)CRTSPClientEngine::NotifyStreamingServerClientBeLiving, this);
		}
		/*
		m_playTime = timeGetTime();
		int intervalBetweenPauseAndPlay = 0;
		if(m_pauseTime>=0)
		{
			intervalBetweenPauseAndPlay = m_playTime-m_pauseTime+100;
			if(intervalBetweenPauseAndPlay<0)
				intervalBetweenPauseAndPlay = 0;
		}*/

		for(int streamIndex=0; streamIndex<MediaStreamCount(); ++streamIndex)
		{
			MediaStream(streamIndex)->ScheduleReceiverReportTask();
			MediaStream(streamIndex)->AdjustSyncClockWithRTPInfo();
			//MediaStream(streamIndex)->CalculateNPTAfterPlayResponse();
		}
		CalculateNPTAfterPlayResponse();
		pRTSPMediaStreamSyncEngine->SetPlayResponse(true);
		return;
	} while(0);
	if(CUserOptions::UserOptions.errorID)
		NotifyEvent(VO_EVENT_PLAY_FAIL,CUserOptions::UserOptions.errorID);
	CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt", "@@@@Parse play response fail!!\n");
}
void CRTSPClientEngine::CalculateNPTAfterPlayResponse()
{
	for(int streamIndex=0; streamIndex<MediaStreamCount(); ++streamIndex)
	{
		MediaStream(streamIndex)->CalculateNPTAfterPlayResponse();
	}
}
void CRTSPClientEngine:: SetSDPFileString(char* string){
#if ENABLE_COPY_SDPFILESTRING
	if(m_sdpFileString)
		free(m_sdpFileString);
	m_sdpFileString=strdup(string);
#else
	m_sdpFileString= string;
#endif
}
void    CRTSPClientEngine::HandleKeepAliveResponse(char* responseLine)
{
	m_responsHandler[RR_ID_KEEPALIVE].Reset();
}
void    CRTSPClientEngine::HandlePauseResponse(char* responseLine)
{
	unsigned int responseCode = 0;

	m_responsHandler[RR_ID_PAUSE].Reset();
	do 
	{
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","HandlePauseResponse\n");

		if(!ParseResponseCode(responseLine, responseCode))
			break;

		if(responseCode != 200)
		{
			CUserOptions::UserOptions.errorID = E_USER_ERROR_BASE-responseCode;
			break;
		}
		
		return;
	} while(0);
	m_failConnectNum+=5;
	if(responseCode>400)
	{
		NotifyEvent(VO_EVENT_PAUSE_FAIL,CUserOptions::UserOptions.errorID);
		m_failConnectNum+=10;//it is an serious error
		CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt", "@@@@Parse pause fail  responseCode>400\n");
	}
}
CRTSPResponseHandler* CRTSPClientEngine::FindTheResponseHandler(char* response)
{
	int cseq=GetTheSeqNum(response,strlen(response));
	for (int i=0;i<MAX_HANDLER;i++)
	{
		if(cseq==m_responsHandler[i].GetCSEQ())
			return &m_responsHandler[i];
	}
	return NULL;
}
int CRTSPClientEngine::DoHandleRTSPServerRequest()
{
	unsigned int requestBytes;
	
	if (CUserOptions::UserOptions.m_isOverHttpTunnel)
	{
		if(!RecvResponseFromStreamingServerOverHttpTunnel(requestBytes,VOS_CMD_UNKNOWN)) 
		{
			//Sleep(10);
			return 0;
		}
	}
	else
	{
		if(!RecvResponseFromStreamingServer(requestBytes,VOS_CMD_UNKNOWN)) 
		{
			//Sleep(10);
			return 0;
		}
	}


 	CRTSPResponseHandler* handler=FindTheResponseHandler(m_recvBuf);
	if(handler)
	{
		handler->HandleResponse(m_recvBuf);
		return PACKET_RTSP;
	}
 	HandleServerRequest_OPTION();
	if (CUserOptions::UserOptions.streamType==ST_ASF)
	{
		char _rtspMethod[32];
		if(sscanf(m_recvBuf, "%[^ \r\n]", _rtspMethod) != 1)
			return 0;

		if(_stricmp(_rtspMethod, "SET_PARAMETER") == 0)
		{
			if (strstr(m_recvBuf,"End-of-Stream Reached"))
			{
				NotifyEvent(VO_EVENT_PLAY_FAIL,5555);//ASF end
			}
		}
	}
	
	// handle rtsp server's request:
#if SUPPORT_ANNOUNCE
	HandleServerRequest_ANNOUNCE();
#endif//SUPPORT_ANNOUNCE
#ifdef HTC_PLAYLIST
	if(CUserOptions::UserOptions.m_bEnablePlaylistSupport)
	{
		char _rtspMethod[32];
		if(sscanf(m_recvBuf, "%[^ \r\n]", _rtspMethod) != 1)
			return 0;

		if(_stricmp(_rtspMethod, "SET_PARAMETER") == 0)
		{
			return HandleServerRequest_SET_PARAMETER();
		}
		else if(_stricmp(_rtspMethod, "END_OF_STREAM") == 0)
		{
			return HandleServerRequest_END_OF_STREAM();
		}
		else
		{
			return 0;
			//		return HandleNotSupportRequest();
		}
	}
#endif //HTC_PLAYLIST
	return PACKET_RTSP;;

	
}
int CRTSPClientEngine::HandleServerRequest_OPTION()
{
	char _rtspMethod[32];
	if(sscanf(m_recvBuf, "%[^ \r\n]", _rtspMethod) != 1)
		return 0;

	if(_stricmp(_rtspMethod, "OPTIONS") == 0)
	{
		int cseq=GetTheSeqNum(m_recvBuf,strlen(m_recvBuf));
		return DoNotifyStreamingServerClientBeLiving(0,cseq);
	}

	return 0;
}
int CRTSPClientEngine::HandleServerRequest_ANNOUNCE()
{
#if SUPPORT_ANNOUNCE
	char _rtspMethod[32];
	if(sscanf(m_recvBuf, "%[^ \r\n]", _rtspMethod) != 1)
		return 0;

	if(_stricmp(_rtspMethod, "ANNOUNCE") == 0)
	{
		int cseq=GetTheSeqNum(m_recvBuf,strlen(m_recvBuf));
		RTSPTV_EVENTTYPE eventID=RTSPTVEvent_InternalError;

	
		char* str=strstr(m_recvBuf,"a=Access_event:");
		if(str)
		{
			eventID=RTSPTVEvent_AccessGranted;
			if(strstr(str,"granted"))
				eventID=RTSPTVEvent_AccessGranted;
			else if(strstr(str,"blackout"))
				eventID=RTSPTVEvent_AccessDeniedBlackout;
			else if(strstr(str,"nopermession"))
				eventID=RTSPTVEvent_AccessDeniedNoPermission;
			else if(strstr(str,"unknown"))
				eventID=RTSPTVEvent_AccessDenied;
			if(CUserOptions::UserOptions.m_mfCallBackFuncSet.EventHandler)
				CUserOptions::UserOptions.m_mfCallBackFuncSet.EventHandler(eventID,0,0);
		}

		str=strstr(m_recvBuf,"a=Nw_event:");
		if(str)
		{
			eventID=RTSPTVEvent_NetworkStatusOK;
			if(strstr(str,"nwok"))
				eventID=RTSPTVEvent_NetworkStatusOK;
			else if(strstr(str,"incompatiblesys"))
				eventID=RTSPTVEvent_NetworkIncompatible;
			else if(strstr(str,"acquiring"))
				eventID=RTSPTVEvent_NetworkAcquiring;
			else if(strstr(str,"outofcoverage"))
				eventID=RTSPTVEvent_NetworkOutOfCoverage;
			else if(strstr(str,"interfacedown"))
				eventID=RTSPTVEvent_NetworkInterfaceDown;
				
			if(CUserOptions::UserOptions.m_mfCallBackFuncSet.EventHandler)
				CUserOptions::UserOptions.m_mfCallBackFuncSet.EventHandler(eventID,0,0);
		}

		str=strstr(m_recvBuf,"a=Stream_event:");
		if(str)
		{
			eventID=RTSPTVEvent_StreamStatusOK;
			if(strstr(str,"streamok"))
				eventID=RTSPTVEvent_StreamStatusOK;
			else if(strstr(str,"activationfailed"))
				eventID=RTSPTVEvent_StreamProgramActivationFailed;
			else if(strstr(str,"deactivated"))
				eventID=RTSPTVEvent_StreamProgramDeactivated;
		
			if(CUserOptions::UserOptions.m_mfCallBackFuncSet.EventHandler)
				CUserOptions::UserOptions.m_mfCallBackFuncSet.EventHandler(eventID,0,0);
		}
		
		str=strstr(m_recvBuf,"a=Nw_event:");
		strcpy(cmdFormatString, "RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Session: %s\r\n"
			"\r\n");
		sprintf(m_rtspCMDString, cmdFormatString, cseq, m_rtspSessionID);
	
		if(!SendRTSPCommandToStreamingServer(m_rtspCMDString,1))
		{
			//break;
		}
	}
#endif//SUPPORT_ANNOUNCE
	return 0;
}
#ifdef HTC_PLAYLIST
int CRTSPClientEngine::HandleServerRequest_SET_PARAMETER()
{
	ResponseToServer(200, "OK");
	if(!CUserOptions::UserOptions.m_bEnablePlaylistSupport)
		return 0;

	char * requestLine = m_recvBuf;
	char * nextRequestLine = GetNextLine(requestLine);
	int contentLength = 0;
	while(1) 
	{
		requestLine = nextRequestLine;
		if(requestLine == NULL)
			break;
		nextRequestLine = GetNextLine(requestLine);

		if(sscanf(requestLine, "Content-Length: %d", &contentLength) == 1 || sscanf(requestLine, "Content-length: %d", &contentLength) == 1) 
			continue;
	}
	if(contentLength > 0)
	{
		ReadNetDataExact(m_rtspSock, (unsigned char *)m_recvBuf, contentLength);
		m_recvBuf[contentLength] = '\0';

		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", m_recvBuf);
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "\r\n");


		char * playRange = NULL;
		float nptTime = 0;
		char * mediaName = NULL;

		char * field = strstr(m_recvBuf, ":");
		char fieldName[32] = {0};
		char * fieldValue = NULL;
		while(field != NULL)
		{
			++field; // skip ": ;"
			while(*field == ' ')
				++field;

			do
			{
				fieldName[0] = '\0';
				if(sscanf(field, "%[^=\r\n]", fieldName) != 1)
					break;

				fieldValue = strstr(field, "=");
				if(fieldValue == NULL)
					break;
				++fieldValue; //skip '='
				while(*fieldValue == ' ')
					++fieldValue;

				if(_stricmp(fieldName, "playlist_play_time") == 0)
				{
					playRange = new char[256];TRACE_NEW("rspce_playRange",playRange);
					if(playRange == NULL)
						break;

					if(sscanf(fieldValue, "%[^;\r\n]", playRange) != 1)
					{
						playRange[0] = '\0';
						break;
					}
				}
				else if(_stricmp(fieldName, "npt") == 0)
				{
					sscanf(fieldValue, "%f", &nptTime);
				}
				else if(_stricmp(fieldName, "mediaName") == 0)
				{
					mediaName = new char[256];TRACE_NEW("rspce_mediaName",mediaName);
					if(mediaName == NULL)
						break;

					if(sscanf(fieldValue, "%[^;\r\n]", mediaName) != 1)
					{
						mediaName[0] = '\0';
						break;
					}
				}
				else
				{
				}

			}while(0);

			field = strstr(field, ";");
		}

		m_pICustomNotify->PlayListSetParamNotify(playRange, nptTime, mediaName);

		if(playRange != NULL)
			SAFE_DELETE_ARRAY(playRange);
		if(mediaName != NULL)
			SAFE_DELETE_ARRAY(mediaName);
	}

	return PACKET_RTSP;
}

int CRTSPClientEngine::HandleServerRequest_END_OF_STREAM()
{
	ResponseToServer(200, "OK");

	m_pICustomNotify->EndOfStreamNotify();

	return PACKET_RTSP;
}
#endif //HTC_PLAYLIST

int CRTSPClientEngine::HandleNotSupportRequest(char * CSeqValue)
{
	ResponseToServer(405, "Method Not Allowed");
	return PACKET_RTSP;
}

void CRTSPClientEngine::ResponseToServer(int respCode, const char * _respText)
{
	sprintf(m_rtspCMDString, "RTSP/1.0 %d %s\r\n", respCode, _respText);

	char * pCSeq = strstr(m_recvBuf, "CSeq:");
	if(pCSeq == NULL)
		pCSeq = strstr(m_recvBuf, "Cseq: ");
	if(pCSeq != NULL)
	{
		char CSeqParam[64] = {0};
		sscanf(pCSeq, "%[^\r\n]", CSeqParam);
		strcat(m_rtspCMDString, CSeqParam);
		strcat(m_rtspCMDString, "\r\n");
	}

	char * pSession = strstr(m_recvBuf, "Session:");
	if(pSession != NULL)
	{
		char SessionParam[128];
		sscanf(pSession, "%[^\r\n]", SessionParam);
		strcat(m_rtspCMDString, SessionParam);
		strcat(m_rtspCMDString, "\r\n");
	}
	

#ifdef HTC_PLAYLIST
	AddRTSPFields(CUST_PLAY);
#endif //HTC_PLAYLIST
	strcat(m_rtspCMDString, "\r\n");

	SendRTSPCommandToStreamingServer(m_rtspCMDString);
}
int CRTSPClientEngine::ReadStreamsOverTCP(void * param)
{
	CRTSPClientEngine * rtspClientEngine = (CRTSPClientEngine *)param;
	return rtspClientEngine->DoReadStreamsOverTCP();
}
#if 1
static void _TraceReadStreamsOverTCP(int flag,const char* name,const char* info)
{
	if(flag)
	{
		CLog::Log.MakeLog(LL_SOCKET_ERR,name,info);
	}
}
#else
#define _TraceReadStreamsOverTCP(a,b)
#endif

unsigned int CRTSPClientEngine::IsReadyToParse(unsigned char * buf, unsigned int length)
{
	unsigned int ret = 0;

	if(!length)
		return 0;
		
	if((buf[0] == 'R') && (buf[1] == 'T') && (buf[2] == 'S') && (buf[3] == 'P'))
	{
		unsigned char* end=(unsigned char*)strstr((const char*)buf,"\r\n\r\n");

		if(!end)
		{
			return 0;
		}
		else
		{
			ret = end+4-buf;
			if(ret>length)
			{
				sprintf(CLog::formatString,"Weird RTSP packet buf length %u, packet len %u\n",
						buf[0],buf[1],buf[2],buf[3]);
				CLog::Log.MakeLog(LL_RTSP_ERR,"tcp.txt",CLog::formatString);
				return 0;
			}
		}
	}
	else if((buf[0] == '$'))
	{
		unsigned short size = 0;
		if(length<4)
		{
			sprintf(CLog::formatString,"No enough length (%u) for locating size of interleave packet!!\n",
					length);
			CLog::Log.MakeLog(LL_RTSP_ERR,"tcp.txt",CLog::formatString);
			return 0;
		}

		memcpy(&size, buf+2, 2); 
		size = NetToHostShort(size);
		
		if(size+4>length)
			ret = 0;
		else
			ret = size+4;
	}
	else
	{
		sprintf(CLog::formatString,"Unknown packets 0x%x 0x%x 0x%x 0x%x!!!!\n",
				buf[0],buf[1],buf[2],buf[3]);
		CLog::Log.MakeLog(LL_RTSP_ERR,"tcp.txt",CLog::formatString);
		ret = length;
	}

	return ret;
}

void CRTSPClientEngine::ParseOneRTSPResp(unsigned char * buf, unsigned int length)
{
	memcpy(m_rtspCMDString,buf,length);
	m_rtspCMDString[length]='\0';
	char* end=strstr(m_rtspCMDString,"\r\n\r\n");
	int len = end-m_rtspCMDString+4;

	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",m_rtspCMDString);
	CRTSPResponseHandler* handler=FindTheResponseHandler(m_rtspCMDString);
	
	if(handler)
	{
		handler->HandleResponse(m_rtspCMDString);
	}
	
	if(CUserOptions::UserOptions.streamType==ST_ASF)
	{
		if(strncmp(m_rtspCMDString, "SET_PARAMETER", 13) == 0)
		{
			if(strstr(m_rtspCMDString,"End-of-Stream Reached"))
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","WMV server End-of-Stream Reached\n");
				m_pRTSPSource->SetSourceEnd(true);
			}
		}
	}
}

int CRTSPClientEngine::ParseOneRTPInterleave(unsigned char * buf, unsigned int length)
{
	unsigned char streamChannelID = buf[1];
	int ret = -1;

	CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();

	if(!pRTSPMediaStreamSyncEngine->GetPlayResponseEx())
		return ret;
		
	for(int streamIndex=0; streamIndex<MediaStreamCount(); ++streamIndex)
	{
		if(MediaStream(streamIndex)->RTPChannelID() == streamChannelID)
		{
			MediaStream(streamIndex)->RTPParser()->ReadRTPStreamFromLocal(buf+4, length-4);
			ret = 1;
			if(CUserOptions::UserOptions.streamType==ST_ASF)
			{
				CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
				pRTSPMediaStreamSyncEngine->AssignRTPInfo(MediaStream(streamIndex));	
			}
			break;
		}					
		if(MediaStream(streamIndex)->RTCPChannelID() == streamChannelID)
		{
			MediaStream(streamIndex)->RTCPParser()->ReadRTCPStreamFromLocal(buf+4, length-4);
			ret = 2;
			if(CUserOptions::UserOptions.streamType==ST_ASF)
			{
				CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
				pRTSPMediaStreamSyncEngine->AssignRTPInfo(MediaStream(streamIndex));	
			}
			break;
		}
	}
		
	return ret;
}

int CRTSPClientEngine::DoReadStreamsOverTCP(int flag)
{
	unsigned char * streamData = (unsigned char *)m_recvBuf;
	int resultFlag=0;

	int result;
	if (CUserOptions::UserOptions.m_isOverHttpTunnel)
	{
		result=IsNetReadyToRead(m_rtspSock2,CUserOptions::UserOptions.m_nRTSPTimeOut);//if there is no data coming in m_nRTSPTimeOut seconds,there is something wrong in the network
	}
	else
	{
		result=IsNetReadyToRead(m_rtspSock,CUserOptions::UserOptions.m_nRTSPTimeOut);//if there is no data coming in m_nRTSPTimeOut seconds,there is something wrong in the network
	}
	if(result<=0)
	{
		CUserOptions::UserOptions.errorID = E_NETWORK_TIMEOUT;
		CLog::Log.MakeLog(LL_SOCKET_ERR,"buffering.txt","@@@@time out to get the RTSP response via TCP\n");
		return 1;
	}
	int readNum;
	if (CUserOptions::UserOptions.m_isOverHttpTunnel)
	{
		readNum = ReadNetData(m_rtspSock2, streamData+m_streamLength, RECVBUFSIZE-m_streamLength);
	}
	else
	{
		readNum = ReadNetData(m_rtspSock, streamData+m_streamLength, RECVBUFSIZE-m_streamLength);
	}
	if(readNum<=0)
	{
		CLog::Log.MakeLog(LL_RTSP_ERR,"tcp.txt","DoReadStreamsOverTCP:readNum<=0\n");
		return 0;
	}
	m_streamLength += readNum;
	unsigned int ret = 0;
	while(ret = IsReadyToParse(streamData,m_streamLength))
	{
		sprintf(CLog::formatString,"TCP length %u (%u), buf[0] 0x%x 0x%x 0x%x 0x%x\n",
			ret,m_streamLength,streamData[0],streamData[1],streamData[2],streamData[3]);
		CLog::Log.MakeLog(LL_RTSP_ERR,"tcpTrace.txt",CLog::formatString);
		if((streamData[0] == 'R') && (streamData[1] == 'T') && (streamData[2] == 'S')
			&& (streamData[3] == 'P'))
		{
			resultFlag |= PACKET_RTSP;
			ParseOneRTSPResp(streamData,ret);
		}
		else if((streamData[0] == '$'))
		{
			int retP = -1;
			retP = ParseOneRTPInterleave(streamData,ret);

			if(retP == 1)
			{
				resultFlag |= PACKET_RTP;
			}
			else if(retP == 2)
			{
				resultFlag |= PACKET_RTCP;
			}
			else if(retP == -1)
			{
				sprintf(CLog::formatString,"Seek_Throw\n");
				CLog::Log.MakeLog(LL_RTSP_ERR,"tcpTrace.txt",CLog::formatString);
			}
		}

		memmove(streamData,streamData+ret,m_streamLength-ret);
		m_streamLength -= ret;
	};
	return resultFlag;
}

int CRTSPClientEngine::NotifyStreamingServerClientBeLiving(void * param)
{
	CRTSPClientEngine * rtspClientEngine = (CRTSPClientEngine *)param;
	return rtspClientEngine->DoNotifyStreamingServerClientBeLiving();
}
void CRTSPClientEngine::UpdateKeepAlive(int timeBySec)
{
	int seconds = m_serverTimeoutValue*3/4;
	if(seconds>100)//it is safe to set the max time-interval as 30 when the timeout is not correct
		seconds = 50;
	if(seconds<0) 		
		seconds = 5;

	if(seconds<timeBySec)
		timeBySec = seconds;
	
	m_taskToken = CTaskSchedulerEngine::CreateTaskSchedulerEngine()->ScheduleTask(timeBySec*1000, (TaskFunction *)CRTSPClientEngine::NotifyStreamingServerClientBeLiving, this,m_taskToken);

}
int CRTSPClientEngine::DoNotifyStreamingServerClientBeLiving(int waitResponseTimeOut,int cseq)
{
	int seconds = m_serverTimeoutValue*2/3;/*-10*/;

	if(seconds>100)//it is safe to set the max time-interval as 30 when the timeout is not correct
		seconds = 50;
	if(seconds<0) 		
		
	seconds = 5;
	
	long nextNotifyLivenessTime = seconds * 1000;
	m_taskToken = CTaskSchedulerEngine::CreateTaskSchedulerEngine()->ScheduleTask(nextNotifyLivenessTime, (TaskFunction *)CRTSPClientEngine::NotifyStreamingServerClientBeLiving, this,m_taskToken);

	do
	{
		if(m_rtspSessionID == NULL)
			break;
#if !KEEPALIVE_VIA_SET_PARAM
		strcpy(cmdFormatString,"OPTIONS %s RTSP/1.0\r\n"
								 "CSeq: %d\r\n"
								 "Session: %s\r\n"
								 "User-Agent: %s\r\n")
								;
		if((CUserOptions::UserOptions.digestIsSet) && (m_digestAuth))
		{
			char method[33] = {0};
			strcpy(method,"OPTIONS");

			m_digestAuth->SetMethod(method,strlen(method));
			m_digestAuth->GetResponse(m_digestResp, 32);
			
			sprintf(m_rtspCMDString, "Authorization: Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\"\r\n",
									CUserOptions::UserOptions.digestAuth.user,
									m_digestRealm,m_digestNonce,m_rtspURL,m_digestResp);
			strcat(cmdFormatString,m_rtspCMDString);
		}
#else
		strcpy(cmdFormatString, "SET_PARAMETER %s RTSP/1.0\r\n"
			"CSeq: %d\r\n"
			"Session: %s\r\n"
			"User-Agent: %s\r\n")
			;
#endif

		if(CUserOptions::UserOptions.m_bOpenUAProEachRequest&&strlen(m_UAProfile)>2)
		{
			sprintf(m_rtspCMDString,"x-wap-profile: %s\r\n",m_UAProfile);
			strcat(cmdFormatString,m_rtspCMDString);
		}
		int seq=cseq?cseq:++m_rtspCSeq;
		sprintf(m_rtspCMDString, cmdFormatString, m_URLofRequest, seq, m_rtspSessionID, m_userAgentAttr);
		AddRTSPFields(CUST_OPTION);
		strcat(m_rtspCMDString,"\r\n");
		//CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","Send Keep Alive Message\n");

		if(CUserOptions::UserOptions.m_isOverHttpTunnel)
		{
			char* FormatString = new char[MAX_TMPLEN];TRACE_NEW("FormatString",FormatString);

			VOLOGI("_____________OPTIONSCMD____________before encoder=%s", m_rtspCMDString);
			EncodeBase64(m_rtspCMDString, FormatString);
			VOLOGI("_____________OPTIONSCMD____________after encoder=%s", FormatString);

			if(!SendRTSPCommandToStreamingServer(FormatString))
				break;
			m_responsHandler[RR_ID_KEEPALIVE].SetCSEQ(seq);
			m_responsHandler[RR_ID_KEEPALIVE].SetSendTime(timeGetTime());
			if(waitResponseTimeOut)
			{
				if(CUserOptions::UserOptions.m_useTCP<=0)
				{
					unsigned int responseBytes;
					int result=IsNetReadyToRead(m_rtspSock2,waitResponseTimeOut);
					if(result>0)
					{
						if(!RecvResponseFromStreamingServerOverHttpTunnel(responseBytes,VOS_CMD_KEEPALIVE)) 
							break;
					}
					else
					{
						CLog::Log.MakeLog(LL_SOCKET_ERR,"keep-alive.txt","@@@@time out to get the keep-alive response\n");
					}
				}
			}
		}
		else
		{
			if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
				break;
			m_responsHandler[RR_ID_KEEPALIVE].SetCSEQ(seq);
			m_responsHandler[RR_ID_KEEPALIVE].SetSendTime(timeGetTime());
			if(waitResponseTimeOut)
			{
				if(CUserOptions::UserOptions.m_useTCP<=0)
				{
					unsigned int responseBytes;
					int result=IsNetReadyToRead(m_rtspSock,waitResponseTimeOut);
					if(result>0)
					{
						if(!RecvResponseFromStreamingServer(responseBytes,VOS_CMD_KEEPALIVE)) 
							break;
					}
					else
					{
						CLog::Log.MakeLog(LL_SOCKET_ERR,"keep-alive.txt","@@@@time out to get the keep-alive response\n");
					}
				}
			}
		}

		return 1;

	}while(0);

	return 0;
}


bool	CRTSPClientEngine::ConnectToServerOverHttpTunnel()
{
	char* rtspURL=m_oriURL;
	m_rtspURL = StringDup(rtspURL);
	CUserOptions::UserOptions.errorID = 0;
//#define HTTPTUNNELPORT 32000

	do
	{
		const char * protocolName = "rtsp://";
		unsigned protocolNameLen = 7;
		if (_strnicmp(rtspURL, protocolName, protocolNameLen) != 0) 
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "invalid url syntax\r\n");
			CUserOptions::UserOptions.errorID = E_HOST_UNKNOWN;
			break;
		}

		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "$$$ConnectToServer:check rtspURL1 \r\n");

		char * p = rtspURL + protocolNameLen;
		char * p1 = strstr(p, "/");
		if(p1 == NULL )
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "invalid url syntax2\r\n");
			CUserOptions::UserOptions.errorID = E_HOST_UNKNOWN;
			break;
		}

		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "$$$ConnectToServer:check rtspURL2 \r\n");
		// parse streaming server address
		char rtspServerHostName[MAX_PATH] = {0};
		char * to = rtspServerHostName;
		while(p < p1)
		{
			if(*p == ':') 
				break;

			*to++ = *p++;
		}
		*to = '\0';

		// parse streaming server port
		//unsigned short serverPort = 554;
		//unsigned short serverPort = 80;
		//unsigned short serverPort = 32000;
		unsigned short serverPort = CUserOptions::UserOptions.m_rtspOverHttpConnectionPort;
		//unsigned short serverPort = 8000;
		int port;
		if(*p == ':')                 
		{
			if(sscanf(++p, "%u", &port) != 1)
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "can not parse port\r\n");
				CUserOptions::UserOptions.errorID =E_PORT_ALLOC;
				break;
			}
			if(port < 1 || port > 65535) 
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "invalid port\r\n");
				CUserOptions::UserOptions.errorID =E_PORT_ALLOC;
				break;
			}
			serverPort = (unsigned short )port;
		}

		if(CUserOptions::UserOptions.m_bUseRTSPProxy
			&&strstr(rtspURL,"://127.0.0.1")==NULL&&strstr(rtspURL,"://localhost")==NULL)
		{


			char* start=strstr(CUserOptions::UserOptions.m_szProxyIP,"://");
			if(start==NULL)
				start=CUserOptions::UserOptions.m_szProxyIP;
			else
				start+=3;
			char* strPort=strstr(start,":");
			//serverPort=554;	//doncy 0724
			//serverPort=80;
			//serverPort=32000;
			serverPort = CUserOptions::UserOptions.m_rtspOverHttpConnectionPort;
			//serverPort=8000;
			if(strPort)
			{
				if(sscanf(strPort,":%u", &port)==1)
				{
					serverPort = port;
				}
				strPort[0]=0;
			}
			strcpy(rtspServerHostName,start);
			sprintf(CLog::formatString,"!!!!this is Proxy:%s,port=%d\n",rtspServerHostName,serverPort);
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
		}

		//added by doncy 0724
		//if(CUserOptions::UserOptions.m_bUseRTSPProxy)
		//{
		//	serverPort = HTTPTUNNELPORT;
		//}
		//end

		m_rtspServerAddrOverHttpTunnel->family = VAF_UNSPEC;
		m_rtspServerAddrOverHttpTunnel->port   = HostToNetShort(serverPort);
		m_rtspServerAddrOverHttpTunnel->protocol = VPT_TCP;
		m_rtspServerAddrOverHttpTunnel->sockType = VST_SOCK_STREAM;
		int ret=IVOSocket::GetPeerAddressByURL(rtspServerHostName,strlen(rtspServerHostName),m_rtspServerAddrOverHttpTunnel);
		if(ret)
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "getaddrinfo fail\r\n");
			CUserOptions::UserOptions.errorID = E_HOST_UNKNOWN;
			break;
		}
		CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", "GetPeerAddressByURL\n");
		m_rtspSock2 = VOCPFactory::CreateOneSocket( VST_SOCK_STREAM,m_rtspServerAddrOverHttpTunnel->protocol,m_rtspServerAddrOverHttpTunnel->family);
		TRACE_NEW("m_rtspSock2",m_rtspSock2);
		if(m_rtspSock2==NULL||m_rtspSock2->IsValidSocket()==false)
		{
			sprintf(CLog::formatString," StreamSocket fail err=%d\n",GetSocketError());
			CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", CLog::formatString);
			break;
		}
		//CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", "CreateOneSocket\n");
		int timeout=CUserOptions::UserOptions.m_nConnectTimeOut; 
		if(CUserOptions::UserOptions.m_bMakeLog)
		{
			sprintf(CLog::formatString,"Address:%d,%ld,%d,%d,%d,%d,%d\n",
				m_rtspServerAddrOverHttpTunnel->family,
				m_rtspServerAddrOverHttpTunnel->ipAddress,
				serverPort,//m_rtspServerAddrOverHttpTunnel->port,
				m_rtspServerAddrOverHttpTunnel->protocol,
				m_rtspServerAddrOverHttpTunnel->sockType,
				m_rtspServerAddrOverHttpTunnel->userData1,
				m_rtspServerAddrOverHttpTunnel->userData2);
			CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", CLog::formatString);
		}
		ret=0;
		do 
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", "Try to connect...\n");

			ret=m_rtspSock2->Connect(*m_rtspServerAddrOverHttpTunnel,1000);

			if(ret== SOCKET_ERROR) 
			{
				int err = GetSocketError();
				char* format = CLog::formatString;
				sprintf(format,"@@@@connect function fail err=%d\r\n",err);
				CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", format);
				CUserOptions::UserOptions.errorID = E_CONNECTION_RESET;
				break;
			}
			if(CUserOptions::UserOptions.m_exit)//user stop it
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR, "rtsp.txt", "user stops the socket\n");
				timeout=0;
				break;//return false;
			}
		} while(--timeout>0&&ret==1);

		if(timeout==0||ret== SOCKET_ERROR)//timeout
		{
			VOCPFactory::DeleteOneSocket(m_rtspSock2);TRACE_DELETE(m_rtspSock2);
			m_rtspSock2=INVALID_SOCKET;
			char* format = CLog::formatString;
			sprintf(format,"@@@@connect function timeout=%d,%d,ret=%d,err=%d\r\n",timeout,CUserOptions::UserOptions.m_nRTSPTimeOut,ret,GetSocketError());
			CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", format);
			break;
		}
		CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", "ConnectToServer Done\n");
		return true;

	}while(0);
	m_rtspSock2 = INVALID_SOCKET;
	CLog::Log.MakeLog(LL_SOCKET_ERR, "rtsp.txt", "@@@@@ConnectToServer() fail\r\n");
	return false;
}

void	CRTSPClientEngine::GenerateSessionCookie()
{
	int i,n;
	for(i=0;i<22;i++)
	{
		n = (Random32()&0x000f);
		if (n > 9 )
			m_fSessionCookie[i] = n + 'a' - 10 ;
		else
			m_fSessionCookie[i] = n + '1' - 1;
	}
	m_fSessionCookie[23] = '\0';
	VOLOGI("@@@@@@@@@@________m_fSessionCookie = %s", m_fSessionCookie);
}