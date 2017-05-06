#include <memory.h>
#include <stdio.h>
#include <string.h>
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

#include "mswitch.h"

#include "DxManager.h"

#include "RTSPSource.h"
#include "voLog.h"
#include "mylog.h"
#define LOG_TAG "FLOEngine"
CRTSPClientEngine::CRTSPClientEngine(void)
: m_rtspURL(NULL)
, m_rtspCSeq(0)
, m_rtspSessionID(NULL)
, m_contentBase(NULL)
, m_serverTimeoutValue(60) //RFC2326 12.37 default value=60
, m_rtspSock(NULL)
, m_rtspServerAddr(NULL)
, m_rtspSession(NULL)
, m_recvBuf(NULL)
, m_rtspCMDString(NULL)
, m_userAgentAttr(NULL)

//, m_taskToken(0)

, m_URLofRequest(NULL)
, m_rtspServerPort(0)
, m_bInit(false)

{
	StartupSocket();

	CMediaStream::m_streamChannelIDGenerator = 0;

#if _DUMP_RTSP
	m_hLogRTSP = NULL;
#endif //_DUMP_RTSP
}

CRTSPClientEngine::~CRTSPClientEngine(void)
{
	Reset();

	CTaskSchedulerEngine::CreateTaskSchedulerEngine()->DestroyTaskSchedulerEngine();
	CleanupSocket();

	SAFE_DELETE_ARRAY(m_recvBuf);
	SAFE_DELETE_ARRAY(m_rtspCMDString);
	SAFE_DELETE_ARRAY(m_rtspSessionID);
}

bool CRTSPClientEngine::Init()
{
	if(m_bInit) //inited
		return true;
	
	do
	{
		if(m_rtspServerAddr == NULL)
		{
			m_rtspServerAddr = new struct voSocketAddress;
			if(m_rtspServerAddr == NULL)
				break;
		}

		if(m_recvBuf == NULL)
		{
			m_recvBuf = new char[RECVBUFSIZE];
			if(m_recvBuf == NULL)
				break;
		}

		if(m_rtspCMDString == NULL)
		{
			m_rtspCMDString = new char[1024];
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
			char * userAgent = CUserOptions::UserOptions.m_sUserAgent;
			m_userAgentAttr = new char[256]; 
			if(m_userAgentAttr == NULL)
				break;

			strcpy(m_userAgentAttr, userAgent);
		}

#if _DUMP_RTSP
	    if(m_hLogRTSP == NULL)
		{
			const char * logDir = GetLogDir();
			char filePath[256];
			strcpy(filePath, logDir);
			strcat(filePath, "flo_rtsp.txt");
			m_hLogRTSP = fopen(filePath, "wb");	
		}
#endif //_DUMP_RTSP
		
		CUserOptions::UserOptions.disableNotify = true;
		
		m_bInit = true;
		return true;

	}while(0);

	return false;
}

void CRTSPClientEngine::Reset()
{
	CTaskSchedulerEngine::CreateTaskSchedulerEngine()->UnscheduleAllTask();
	

	if(m_rtspSock != NULL)
	{
		VOCPFactory::DeleteOneSocket(m_rtspSock);
		m_rtspSock = NULL;
	}

	
	SAFE_DELETE(m_rtspServerAddr);
	SAFE_DELETE(m_rtspSession);
	SAFE_DELETE_ARRAY(m_rtspURL);
	SAFE_DELETE_ARRAY(m_rtspSessionID);
	SAFE_DELETE_ARRAY(m_contentBase);
	SAFE_DELETE_ARRAY(m_userAgentAttr);

	m_rtspCSeq = 0;

//	m_taskToken = 0;
	
	CRealTimeStreamingEngine::DestroyRealTimeStreamingEngine();
	CRTSPMediaStreamSyncEngine::DestroyRTSPMediaStreamSyncEngine();
	
	CMediaStream::m_streamChannelIDGenerator = 0;
	
	m_bInit = false;


#if _DUMP_RTSP
	if(m_hLogRTSP != NULL)
	{
		fclose(m_hLogRTSP);
		m_hLogRTSP = NULL;
	}
#endif //_DUMP_RTSP	
}

void CRTSPClientEngine::ResetForOCC()
{
	CTaskSchedulerEngine::CreateTaskSchedulerEngine()->UnscheduleAllTask();
	
	//SAFE_DELETE(m_rtspServerAddr);
	SAFE_DELETE(m_rtspSession);
	SAFE_DELETE_ARRAY(m_rtspURL);

	SAFE_DELETE_ARRAY(m_rtspSessionID);
	SAFE_DELETE_ARRAY(m_contentBase);
	SAFE_DELETE_ARRAY(m_userAgentAttr);

	//m_rtspCSeq = 0;

//	m_taskToken = 0;
	
	CRealTimeStreamingEngine::DestroyRealTimeStreamingEngine();
	CRTSPMediaStreamSyncEngine::DestroyRTSPMediaStreamSyncEngine();
	
	CMediaStream::m_streamChannelIDGenerator = 0;
	
	
	m_bInit = false;

#if _DUMP_RTSP
	if(m_hLogRTSP != NULL)
	{
		fclose(m_hLogRTSP);
		m_hLogRTSP = NULL;
	}
#endif //_DUMP_RTSP	
}

void CRTSPClientEngine::SetRTSPSource(CRTSPSource * pRTSPSource)
{
	m_pRTSPSource = pRTSPSource;
}


bool CRTSPClientEngine::ConnectServer(char * rtspURL)
{
	bool b = ConnectToStreamingServer(rtspURL);
	if (!b)
		return b;

#if ENABLE_DX
	EDxStatus status = TheDxManager.Open();
	if (EDX_SUCCESS != status)
		b = false;
#endif ENABLE_DX

	return b;
}


bool CRTSPClientEngine::OptionsCmd(char * rtspURL)
{	
	do
	{
		char cmdFormatString[256] = "OPTIONS %s RTSP/1.0\r\n"
									"CSeq: %d\r\n"
									"User-Agent: %s\r\n";


		sprintf(m_rtspCMDString, cmdFormatString, rtspURL, ++m_rtspCSeq, m_userAgentAttr);
		strcat(m_rtspCMDString, "\r\n");
		
		if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
			break;
		
		unsigned int responseBytes;
		if(!RecvResponseFromStreamingServer(responseBytes, VOS_CMD_OPTION)) 
			break;

		char * responseLine = m_recvBuf;
		char * nextResponseLine = GetNextLine(responseLine);
		unsigned int responseCode = 0;
		if(!ParseResponseCode(responseLine, responseCode))
			break;

		if(responseCode != 200)
		{
			CUserOptions::UserOptions.errorID = E_USER_ERROR_BASE - responseCode;
			break;
		}
		
		while(1) 
		{
			responseLine = nextResponseLine;
			if(responseLine == NULL)
				break;

			nextResponseLine = GetNextLine(responseLine);

		}

		return true;

	}while(0);
	
	NotifyEvent(HS_EVENT_OPTION_FAIL, CUserOptions::UserOptions.errorID);

	return false;
}

bool CRTSPClientEngine::DescribeCmd(char * rtspURL)
{
	do
	{
		char cmdFormatString[512] = "DESCRIBE %s RTSP/1.0\r\n"
									"CSeq: %d\r\n"
									"Accept: application/sdp\r\n"
									"Accept-Encoding: identity\r\n"
									"User-Agent: %s\r\n";
						
		if(m_rtspSessionID != NULL)
		{
			sprintf(m_rtspCMDString, "Session: %s\r\n", m_rtspSessionID);
			strcat(cmdFormatString, m_rtspCMDString);
		}

		sprintf(m_rtspCMDString, cmdFormatString, rtspURL, ++m_rtspCSeq, m_userAgentAttr);
		strcat(m_rtspCMDString,"\r\n");

		int targetCSEQ = m_rtspCSeq;
		if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
			break;

		unsigned int responseBytes;
		//if(!RecvResponseFromStreamingServer(responseBytes, VOS_CMD_DESCRIBE)) 
		//	break;

		int getResponse=0;
		do{
			if(!RecvResponseFromStreamingServer(responseBytes,VOS_CMD_DESCRIBE)) 
				break;

			if(targetCSEQ == GetTheSeqNum(m_recvBuf, strlen(m_recvBuf)))
			{
				getResponse=1;
				break;
			}
		}while(1);
		if(getResponse==0)
			break;

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

		int contentLength = -1;
		int line = 300;
		while(line-- > 0) 
		{
			responseLine = nextResponseLine;
			if(responseLine == NULL)
				break;
			nextResponseLine = GetNextLine(responseLine);


			if(sscanf(responseLine, "Content-Length: %d", &contentLength) == 1 || sscanf(responseLine, "Content-length: %d", &contentLength) == 1) 
				continue;
			if(ParseContentBase(responseLine))
				continue;
		}

			
			
		if(contentLength > 0)
		{
			if(ReadNetDataExact(m_rtspSock, (unsigned char *)m_recvBuf, contentLength) != contentLength)
				break;
			m_recvBuf[contentLength] = '\0';
			SLOG1(LL_RTSP_ERR,"rtsp.txt","%s",m_recvBuf);
#if ENABLE_LOG
			//TheRTSPLogger.Dump("Time: %u(ms)\r\n==============\r\n%s\r\n\r\n", GetAppTick(), m_recvBuf);
#endif //ENABLE_LOG

#if _DUMP_RTSP
			if(m_hLogRTSP != NULL)
			{
				fwrite(m_recvBuf, 1, strlen(m_recvBuf), m_hLogRTSP);
				fflush(m_hLogRTSP);
			}
#endif //_DUMP_RTSP
		}


		if(m_rtspSession == NULL)
		{
			m_rtspSession = new CRTSPSession(this);
		}
		
		char * sdpBuf = m_recvBuf;
		if(!m_rtspSession->Init(sdpBuf, m_rtspServerAddr))
		{
			CUserOptions::UserOptions.errorID = E_UNSUPPORTED_FORMAT;
#if ENABLE_LOG
		//TheRTSPLogger.Dump("Fail to init RTP session" TXTLN);
#else //ENABLE_LOG
		CLog::Log.MakeLog(LL_RTSP_ERR, "rtsp.txt", "@@@@fail to init the rtspsession!!\n");
#endif //ENABLE_LOG
			break;
		}
		
		return true;

	}while(0);
	
	NotifyEvent(HS_EVENT_DESCRIBE_FAIL, CUserOptions::UserOptions.errorID);
	CLog::Log.MakeLog(LL_RTSP_ERR, "error.txt", "@@@@Parse describe fail!!\n");

	return false;
}


bool CRTSPClientEngine::SetupCmd()
{
	int streamCount = MediaStreamCount();
	for(int i=0; i<streamCount; ++i)
	{
		if(!SetupCmd_MediaStream(i))
		{
			CLog::Log.MakeLog(LL_RTSP_ERR, "error.txt", "@@@@setup fail!\n");
			return false;
		}
	}

	return true;
}

bool CRTSPClientEngine::SetupCmd_MediaStream(int streamIndex)
{
	int getResponse=0;
	bool bRecvSucc=false;
	do
	{
		CMediaStream * pMediaStream = MediaStream(streamIndex);

		char cmdFormatString[1024];
		const char* constrol = pMediaStream->ControlAttr();
		if(strstr(constrol, "RTSP://") == NULL && strstr(constrol, "rtsp://") == NULL)
		{
			sprintf(m_rtspCMDString, "SETUP %s/%s RTSP/1.0\r\n", m_URLofRequest, pMediaStream->ControlAttr());
		}
		else
		{
			sprintf(m_rtspCMDString, "SETUP %s RTSP/1.0\r\n", pMediaStream->ControlAttr());
		}

		strcpy(cmdFormatString, m_rtspCMDString);
		strcat(cmdFormatString, "CSeq: %d\r\n");
		strcat(cmdFormatString, "User-Agent: %s\r\n");
		if(m_rtspSessionID != NULL)
		{
			sprintf(m_rtspCMDString, "Session: %s\r\n", m_rtspSessionID);
			strcat(cmdFormatString,m_rtspCMDString);
		}

	
		sprintf(m_rtspCMDString, "Transport: RTP/AVP/UDP;unicast;client_port=%d-%d\r\n", pMediaStream->ClientRTPPort(), pMediaStream->ClientRTCPPort());
		strcat(cmdFormatString,m_rtspCMDString);

		
		sprintf(m_rtspCMDString, cmdFormatString, ++m_rtspCSeq, m_userAgentAttr);
		strcat(m_rtspCMDString,"\r\n");
		

		const char * ControlAttr = pMediaStream->ControlAttr();
		if(ControlAttr)
			CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", ControlAttr);
		int targetCSEQ=m_rtspCSeq;
		
		if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
			break;

		unsigned int responseBytes;
		
		do{
			
			do{
				bRecvSucc = RecvResponseFromStreamingServer(responseBytes, VOS_CMD_SETUP);
				if(!bRecvSucc)
					break;

				if(_strnicmp(m_recvBuf, "ANNOUNCE", 8) == 0)
				{
					HandleServerRequest_ANNOUNCE();
				}
				else
				{
					break;
				}

			}while(1);

			if(!bRecvSucc)
				break;
			if(targetCSEQ==GetTheSeqNum(m_recvBuf, strlen(m_recvBuf)))
			{
			   getResponse=1;
			   break;
			}
		}while(1);
		if(getResponse==0)
			break;
		
		char * responseLine = m_recvBuf;
		char * nextResponseLine = GetNextLine(responseLine);
		unsigned int responseCode = 0;

		if(!ParseResponseCode(responseLine, responseCode))
			break;

		if(responseCode != 200)
		{
			CUserOptions::UserOptions.errorID = E_USER_ERROR_BASE - responseCode;
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

		return true;

	}while(0);
	
	//NotifyEvent(HS_EVENT_SETUP_FAIL, CUserOptions::UserOptions.errorID);
	SLOG2(LL_RTSP_ERR,"rtsp.txt", "@@@@Parse setup fail!!,%d,%d\n",bRecvSucc,getResponse);

	return false;
}


bool CRTSPClientEngine::PlayCmd()
{
	unsigned int responseCode = 0;
	CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
	pRTSPMediaStreamSyncEngine->SetPlayRangeBegin(0.0);

#ifdef WIN32
	SendExtraRTPPacket();
	SendExtraRTCPPacket();
#endif
	//m_pRTSPSource->SetBeginTime(voOS_GetSysTime());
	do
	{
		CRealTimeStreamingEngine * streamingEngine = CRealTimeStreamingEngine::CreateRealTimeStreamingEngine();
		if(streamingEngine == NULL)
			break;
		streamingEngine->RemoveStreamReader(m_rtspSock);

		char cmdFormatString[512] = "PLAY %s RTSP/1.0\r\n"
									"CSeq: %d\r\n"
									"Session: %s\r\n"
									"Range: npt=0.000-\r\n"
									"User-Agent: %s\r\n";
	
		sprintf(m_rtspCMDString, cmdFormatString, m_URLofRequest, ++m_rtspCSeq, m_rtspSessionID, m_userAgentAttr);
		strcat(m_rtspCMDString, "\r\n");
		

		
		int targetCSEQ = m_rtspCSeq;

		if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
			break;
		


		unsigned int responseBytes;
		int getResponse=0;
		do{
			if(!RecvResponseFromStreamingServer(responseBytes, VOS_CMD_PLAY)) 
				break;
			if(targetCSEQ==GetTheSeqNum(m_recvBuf, strlen(m_recvBuf)))
			{
			   getResponse=1;
			   break;
			}
		}while(1);
		if(getResponse==0)
			break;


		HandlePlayResponse(m_recvBuf);

		return true;

	}while(0);
	
	
	NotifyEvent(HS_EVENT_PLAY_FAIL, CUserOptions::UserOptions.errorID);
	CLog::Log.MakeLog(LL_RTSP_ERR, "error.txt", "@@@@Parse play fail!!\n");

	return false;
}


bool CRTSPClientEngine::TeardownCmd()
{
	if(m_rtspSock == NULL)
		return true;

#if ENABLE_DX
	TheDxManager.Close();
#endif ENABLE_DX

	do
	{
		char cmdFormatString[256] = "TEARDOWN %s RTSP/1.0\r\n"
									"CSeq: %d\r\n"
									"Session: %s\r\n"
									"User-Agent: %s\r\n";

		sprintf(m_rtspCMDString, cmdFormatString, m_URLofRequest, ++m_rtspCSeq, m_rtspSessionID, m_userAgentAttr);
		strcat(cmdFormatString, "\r\n");

        VOLOGI("Sending TEARDOWN");
		if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
		{
			break;
		}
		return true;

	}while(0);

	//SAFE_DELETE_ARRAY(m_rtspSessionID);
	CLog::Log.MakeLog(LL_RTSP_ERR,"rtsp.txt", "@@@@Processing teardown fail!!\n");
	return false;
}


bool CRTSPClientEngine::HandleOCC(const char * media_url2)
{	
	FlushRTSPStreams(); //Flush current channel data first
	
	do{
		m_URLofRequest = media_url2;
		sprintf(m_rtspCMDString, "SETUP %s RTSP/1.0\r\n", m_URLofRequest);
        VOLOGI("m_rtspCMDString: %s", m_rtspCMDString);
		
		char  cmdFormatString[1024];
		strcpy(cmdFormatString, m_rtspCMDString);
		strcat(cmdFormatString, "CSeq: %d\r\n");
		strcat(cmdFormatString, "User-Agent: %s\r\n");
		if(m_rtspSessionID != NULL)
		{
			sprintf(m_rtspCMDString, "Session: %s\r\n", m_rtspSessionID);
			strcat(cmdFormatString, m_rtspCMDString);
		}
		sprintf(m_rtspCMDString, cmdFormatString, ++m_rtspCSeq, m_userAgentAttr);
		strcat(m_rtspCMDString,"\r\n");
		
		CLog::Log.MakeLog(LL_RTSP_ERR, "flow.txt", m_URLofRequest);
		
		if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
			break;
		
		int targetCSEQ = m_rtspCSeq;
		unsigned int responseBytes = 0;
		int getResponse = 0;
		bool bRecvSucc = false;
		do{
			do{
				bRecvSucc = RecvResponseFromStreamingServer(responseBytes, VOS_CMD_SETUP);
				if(!bRecvSucc)
					break;
				
				if(_strnicmp(m_recvBuf, "ANNOUNCE", 8) == 0)
				{
					HandleServerRequest_ANNOUNCE();
				}
				else
				{
					break;
				}
				
			}while(1);
			
			if(!bRecvSucc)
				break;
			
			if(targetCSEQ == GetTheSeqNum(m_recvBuf, strlen(m_recvBuf)))
			{
				getResponse = 1;
				break;
			}
			
		}while(1);
		
		if(getResponse == 0)
		{
			CUserOptions::UserOptions.errorID = -9+bRecvSucc;
			break;
		}
		
		char * responseLine = m_recvBuf;
		//char * nextResponseLine = GetNextLine(responseLine);
		unsigned int responseCode = 0;		
		if(!ParseResponseCode(responseLine, responseCode))
		{
			CUserOptions::UserOptions.errorID = -10;
			break;
		}
		if(responseCode != 200)
		{
			CUserOptions::UserOptions.errorID = E_RTSP_BAD_RESPONSE;
			break;
		}
			
		//FlushRTSPStreams(); //Flush current channel data first
		for(int streamIndex=0; streamIndex<MediaStreamCount(); ++streamIndex)
		{
			CMediaStream* mediaStream = MediaStream(streamIndex);
			mediaStream->FlushMediaStream();
			mediaStream->FastChannelChange();
			mediaStream->ScheduleReceiverReportTask();					
		}	
		CLog::Log.MakeLog(LL_RTSP_ERR,"rtsp.txt","@@@@FCC --- SETUP command OK\n");
		return true;
		
	}while(0);
	
	// CR #39154, 8/20/2010, NotifyEvent(HS_EVENT_SETUP_FAIL, CUserOptions::UserOptions.errorID);
	SLOG1(LL_RTSP_ERR, "rtsp.txt", "@@@@ FCC --- SETUP command FAIL !!,%d\n",CUserOptions::UserOptions.errorID);
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

int CRTSPClientEngine::SendExtraRTPPacket()
{
#define RTP_HEAD_SIZE 128
	unsigned char nullRTPPacket[RTP_HEAD_SIZE]={0};
	nullRTPPacket[0]=0x80;
	CLog::Log.MakeLog(LL_RTSP_ERR,"rtsp.txt","SendExtraRTPPacket()!\n");
	for(int i=0;i<MediaStreamCount();i++)
	{
		CMediaStream* mediaStream = MediaStream(i);
		CMediaStreamSocket* rtpStreamSocket = mediaStream->GetRTPStreamSocket();
		if(rtpStreamSocket)
		{
			voSocketAddress   rtpServerAddr;
			memset(&rtpServerAddr, 0, sizeof(voSocketAddress));
			memcpy(&rtpServerAddr, mediaStream->MediaAddr(), sizeof(voSocketAddress));

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
	CLog::Log.MakeLog(LL_RTSP_ERR,"rtsp.txt","SendExtraRTCPPacket()!\n");
	for(int i=0;i<MediaStreamCount();i++)
	{
		CMediaStream* mediaStream = MediaStream(i);
		CMediaStreamSocket* rtcpStreamSocket = mediaStream->GetRTCPStreamSocket();
		if(rtcpStreamSocket)
		{
			voSocketAddress   rtpServerAddr;
			memset(&rtpServerAddr, 0, sizeof(voSocketAddress));
			memcpy(&rtpServerAddr, mediaStream->MediaAddr(), sizeof(voSocketAddress));

			rtpServerAddr.port = HostToNetShort(mediaStream->ServerRTCPPort()); 

			WriteNetData(rtcpStreamSocket->GetMediaStreamSock(), &rtpServerAddr, nullRTCPPacket, RTCP_HEAD_SIZE);

		}
	}


	return 1;
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


bool CRTSPClientEngine::ConnectToServer()
{
	const char* rtspURL=m_oriURL;
	m_rtspURL = StringDup(rtspURL);
	CUserOptions::UserOptions.errorID = 0;
	
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

		const char * p = rtspURL + protocolNameLen;
		const char * p1 = strstr(p, "/");
		if(p1 == NULL )
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "invalid url syntax2\r\n");
			CUserOptions::UserOptions.errorID = E_HOST_UNKNOWN;
			break;
		}

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
		unsigned short serverPort = 554;
		if(*p == ':')                 
		{
			int port;
#if 0
			char szPort[16];
			if(sscanf(++p, "%[^/]", szPort) != 1)
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "can not parse port\r\n");
				break;
			}
			port = atoi(szPort);
#else
			if(sscanf(++p, "%u", &port) != 1)
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "can not parse port\r\n");
				CUserOptions::UserOptions.errorID =E_PORT_ALLOC;
				break;
			}
#endif
			if(port < 1 || port > 65535) 
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "invalid port\r\n");
				CUserOptions::UserOptions.errorID =E_PORT_ALLOC;
				break;
			}
			serverPort = (unsigned short )port;
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
			sprintf(CLog::formatString,"Address:%d,%d,%d,%d,%d,%d,%d\n",
											m_rtspServerAddr->family,
											m_rtspServerAddr->ipAddress,
											m_rtspServerAddr->port,
											m_rtspServerAddr->protocol,
											m_rtspServerAddr->sockType,
											m_rtspServerAddr->userData1,
											m_rtspServerAddr->userData2);
			CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", CLog::formatString);
		}
		ret=0;
		do 
		{

#if 0//
			ret=m_rtspSock->Connect(*m_rtspServerAddr,0);
#else//
			ret=m_rtspSock->Connect(*m_rtspServerAddr,1000);
			//if(GetSocketError()==10048) ret=SOCKET_ERROR;//NOT_REUSE
#endif//
			//if(GetSocketError()==10048) ret=SOCKET_ERROR;//NOT_REUSE
				

			if(ret == VEC_SOCKET_ERROR) 
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
		
		if(timeout==0 || ret== VEC_SOCKET_ERROR)//timeout
		{
			VOCPFactory::DeleteOneSocket(m_rtspSock);
			m_rtspSock=NULL;
			char* format = CLog::formatString;
#ifdef LINUX
			sprintf(format,"@@@@connect function timeout=%d,%d,ret=%d,err=%s\r\n",timeout,CUserOptions::UserOptions.m_nConnectTimeOut,ret,strerror(GetSocketError()));
#else
			sprintf(format,"@@@@connect function timeout=%d,%d,ret=%d,err=%d\r\n",timeout,CUserOptions::UserOptions.m_nConnectTimeOut,ret,GetSocketError());
#endif
			CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", format);
			break;
		}

		CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", "ConnectToServer Done\n");

		return true;

	}while(0);

	m_rtspSock = NULL;
	CLog::Log.MakeLog(LL_SOCKET_ERR, "rtsp.txt", "@@@@@ConnectToServer() fail\r\n");
	return false;
}

bool CRTSPClientEngine::ConnectToStreamingServer(char * rtspURL)
{
	if(m_rtspSock != NULL)
		return true;
	
	m_URLofRequest = m_oriURL = rtspURL;
	m_bTimeOut = false;
	
	ConnectToServer();

	if(m_rtspSock == NULL)
	{
		if(CUserOptions::UserOptions.errorID==0)
			CUserOptions::UserOptions.errorID = E_CONNECTION_FAILED;
		
		NotifyEvent(HS_EVENT_CONNECT_FAIL,CUserOptions::UserOptions.errorID);
	}

	return m_rtspSock != NULL;
}

bool CRTSPClientEngine::SendRTSPCommandToStreamingServer(const char * cmdString,int timeOutBySec)
{
#if 0
	int result = IsNetReadyToSend(m_rtspSock, timeOutBySec);
	if(result <= 0)
	{
		CUserOptions::UserOptions.errorID = E_RTSP_SEND_FAILED;
		SLOG1(LL_SOCKET_ERR, "socket_timeout.txt", "@@@@time out to send the RTSP request,%d\n",result);
		VOLOGE("@@@@time out(%ds) to send the RTSP request,%d",timeOutBySec,result);
		return false;
	}
#endif
	
	if(::SendTCPData(m_rtspSock, cmdString, strlen(cmdString), 0) == VEC_SOCKET_ERROR)
	{

		CUserOptions::UserOptions.errorID = E_RTSP_SEND_FAILED;

		int err = GetSocketError();
#ifndef LINUX
		SLOG1(LL_SOCKET_ERR, "socketErr.txt","@@@@@@@@@@@@fail to SendRTSPCommandToStreamingServer err=%d\r\n", err);
#else
		SLOG1(LL_SOCKET_ERR, "socketErr.txt","@@@@@@@@@@@@fail to SendRTSPCommandToStreamingServer err=%s\r\n", strerror(err));

#endif
		return false;
	}
#if ENABLE_LOG
	TheRTSPLogger.Dump("%s" TXTLN, cmdString);

#endif //ENABLE_LOG

	if(CUserOptions::UserOptions.m_bMakeLog)
	{
		CLog::Log.MakeLog((LOG_Level)LL_RTSP_ERR, "rtsp.txt", cmdString);
		CLog::Log.MakeLog(LL_RTSP_ERR, "rtsp.txt", "\r\n");
	}

#if _DUMP_RTSP
	    if(m_hLogRTSP != NULL)
		{
			fwrite(cmdString, 1, strlen(cmdString), m_hLogRTSP);
			fflush(m_hLogRTSP);
		}
#endif //_DUMP_RTSP


	return true;
}


bool CRTSPClientEngine::RecvResponseFromStreamingServer(unsigned int & responseBytes, int cmd, int timeoutBySec)
{
	responseBytes = 0;
	m_recvBuf[0] = '\0';
	
	int nTimeoutValue = (timeoutBySec == 0) ? CUserOptions::UserOptions.m_nRTSPTimeOut : timeoutBySec;
	int result = 0;
	
	while(responseBytes < RECVBUFSIZE)
	{
		result = CheckRTSPTimeout(nTimeoutValue); //if there is no data coming in m_nRTSPTimeOut seconds, there is something wrong in the network
		if(result <= 0)
		{
			CUserOptions::UserOptions.errorID = E_NETWORK_TIMEOUT;
			sprintf(CLog::formatString, "@@@@time out to get the RTSP response, cmd=%d,RET=%d\n", cmd,result);
			CLog::Log.MakeLog(LL_SOCKET_ERR, "socket_timeout.txt", CLog::formatString);
			return false;
		}
		
		int readBytes = ReadNetData(m_rtspSock, (unsigned char *)(m_recvBuf + responseBytes), 1);
		if(readBytes <= 0) 
			break;

		responseBytes += readBytes;
		//recv response over after recv "\r\n\r\n"
		int offset2 = responseBytes - 4;
		if(offset2 < 0) 
			continue;

		char * p = m_recvBuf + offset2;
		if(*p == '\r' && *(p+1) == '\n' && *(p+2) == '\r' && *(p+3) == '\n') 
		{
			//_TraceTCPPause("pause.txt", "ok__end\n");
			m_recvBuf[responseBytes] = '\0';

			CLog::Log.MakeLog((LOG_Level)(LL_RTSP_ERR|LL_RTSP_MESSAGE), "rtsp.txt", m_recvBuf);
			CLog::Log.MakeLog(LL_RTSP_ERR, "rtsp.txt", "\r\n");
			//TheRTSPLogger.Dump("%s\r\n", m_recvBuf);

#if _DUMP_RTSP
	    if(m_hLogRTSP != NULL)
		{
			fwrite(m_recvBuf, 1, strlen(m_recvBuf), m_hLogRTSP);
			fflush(m_hLogRTSP);
		}
#endif //_DUMP_RTSP

			return true;
		}
	}
	
	return false;
}

int CRTSPClientEngine::CheckRTSPTimeout(int timeoutBySec)
{
	int count = timeoutBySec;
	int result = 0;
	
#define TRY_TIME_RTSP 1
	
	do 
	{
		result = IsNetReadyToRead(m_rtspSock, TRY_TIME_RTSP);
		if(result != 0) //error happens(<0) or has got the result(>0)
			break;
		
		count -= TRY_TIME_RTSP;
	} while(count > 0);
	
	return result;
}

bool CRTSPClientEngine::ParseResponseCode(char * responseText, unsigned int & responseCode)
{
	responseCode = 0;

	return sscanf(responseText, "%*s%u", &responseCode) == 1;
}


bool CRTSPClientEngine::ParseContentBase(char * responseText)
{
	do
	{
		if(_strnicmp(responseText, "Content-Base:", 13) != 0)
			break;
		
		SAFE_DELETE_ARRAY(m_contentBase);
		m_contentBase = StringDup(responseText);
		if(sscanf(responseText, "Content-Base: %s", m_contentBase) != 1 && sscanf(responseText, "Content-base: %s", m_contentBase) != 1)
			break;

		if(m_contentBase[strlen(m_contentBase) - 1] == '/')
			m_contentBase[strlen(m_contentBase) - 1] = '\0';

		m_URLofRequest = m_contentBase;//m_rtspURL;

		return true;

	}while(0);

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
	//CLog::Log.MakeLog(LL_RTSP_ERR,"rtsp.txt","@@@@ParseSetupCMDResponse_Sessionn");

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
	//CLog::Log.MakeLog(LL_RTSP_ERR,"rtsp.txt","@@@@ParseSetupCMDResponse_Transport");
	return false;
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

void CRTSPClientEngine::HandlePlayResponse(char* responseLine)
{
	do 
	{
		unsigned int responseCode = 0;
		CRealTimeStreamingEngine * streamingEngine = CRealTimeStreamingEngine::CreateRealTimeStreamingEngine();
		CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();

//		m_responsHandler[RR_ID_PLAY].Reset();

		char * nextResponseLine = GetNextLine(responseLine);
		//unsigned int responseCode = 0;
		if(!ParseResponseCode(responseLine, responseCode))
			break;

		if(responseCode != 200)
		{
			CUserOptions::UserOptions.errorID = E_USER_ERROR_BASE-responseCode;
			break;
		}

		int maxlineNum=30;
		int hasRTP_info = 0;
		int hasRange = 0;
		float begin = 0.0;

		while(maxlineNum-->0) 
		{
			responseLine = nextResponseLine;
			if(responseLine == NULL)
				break;

			nextResponseLine = GetNextLine(responseLine);
		}

		//if(m_taskToken == 0)
		//{
		//	m_taskToken = CTaskSchedulerEngine::CreateTaskSchedulerEngine()->ScheduleTask(10*1000, (TaskFunction *)CRTSPClientEngine::NotifyStreamingServerClientBeLiving, this);
		//}

		for(int streamIndex=0; streamIndex<MediaStreamCount(); ++streamIndex)
		{
			MediaStream(streamIndex)->ScheduleReceiverReportTask();
			//MediaStream(streamIndex)->AdjustSyncClockWithRTPInfo();
			//MediaStream(streamIndex)->CalculateNPTAfterPlayResponse();
		}

		streamingEngine->AddStreamReader(m_rtspSock, HandleRTSPServerRequest, this, VOS_STREAM_RTSP);


		return;

	} while(0);

	if(CUserOptions::UserOptions.errorID)
		NotifyEvent(HS_EVENT_PLAY_FAIL,CUserOptions::UserOptions.errorID);

	CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt", "@@@@Parse play response fail!!\n");
}




int CRTSPClientEngine::DoHandleRTSPServerRequest()
{
	unsigned int requestBytes;
	if(!RecvResponseFromStreamingServer(requestBytes, VOS_CMD_UNKNOWN)) 
	{
		//Sleep(10);
		return 0;
	}
	
//#if !WAIT_RESPONSE_SYNC
//	CRTSPResponseHandler* handler = FindTheResponseHandler(m_recvBuf);
//	if(handler)
//	{
//		handler->HandleResponse(m_recvBuf);
//		return PACKET_RTSP;
//	}
//#endif
	
	HandleServerRequest_OPTION();
	
	HandleServerRequest_ANNOUNCE();
	
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
//		return DoNotifyStreamingServerClientBeLiving(0,cseq);
	}
}



int CRTSPClientEngine::HandleServerRequest_ANNOUNCE()
{
	if(_strnicmp(m_recvBuf, "ANNOUNCE", 8) != 0)
		return 0;

	int rtspCSeq = GetTheSeqNum(m_recvBuf, strlen(m_recvBuf));
	///m_rtspCSeq = rtspCSeq;
	char *line = m_recvBuf;
	char *nextLine = GetNextLine(line);
	int contentLength = 0;
	while(1)
	{
		line = nextLine;
		if(line == NULL)
			break;
		nextLine = GetNextLine(line);
			
		if(sscanf(line, "Content-Length: %d", &contentLength) == 1 ||
			sscanf(line, "Content-length: %d", &contentLength) == 1)
			break;
	}

	SLOG1(LL_RTSP_ERR,"rtsp.txt","Announce_len=%d\n",contentLength);
	if(contentLength == 0)
		return 0;

	VO_U32 eventID = 0;
	VO_U32 eventReason = 0;
	
	if(ReadNetDataExact(m_rtspSock, (unsigned char*)m_recvBuf, contentLength) != contentLength)
	{
		eventID = 0;
		eventReason = 0;
		goto RESPONSE_OK;
	}
	m_recvBuf[contentLength] = '\0';
	SLOG1(LL_RTSP_ERR,"rtsp.txt","%s",m_recvBuf);		
#if _DUMP_RTSP
	    if(m_hLogRTSP != NULL)
		{
			fwrite(m_recvBuf, 1, contentLength, m_hLogRTSP);
			fflush(m_hLogRTSP);
		}
#endif //_DUMP_RTSP


//JPTV ANNOUNCE formated message
		char *msgAttr;
		msgAttr = strstr(m_recvBuf, "a=Access_event:");
		VOLOGI("Message: %s", msgAttr);
		if(msgAttr != NULL)
		{
			if(strstr(msgAttr, "granted"))
			{
				eventID = 100;
				eventReason = 100;
			}
			else if(strstr(msgAttr, "denied blackout"))
			{
				eventID = 101;
				eventReason = 2;
			}
			else if(strstr(msgAttr, "denied nopermission"))
			{
				eventID = 101;
				eventReason = 3;
			}
			else if(strstr(msgAttr, "denied unknown"))
			{
				eventID = 101;
				eventReason = 1;
			}

			goto RESPONSE_OK;
		}

		msgAttr = strstr(m_recvBuf, "a=Nw_event:");
		if(msgAttr != NULL)
		{
			if(strstr(msgAttr, "nwok"))
			{
				eventID = 200;
				eventReason = 0;
			}
			else if(strstr(msgAttr, "incompatiblesys"))
			{
				eventID = 204;
				eventReason = 0;
			}
			else if(strstr(msgAttr, "acquiring"))
			{
				eventID = 201;
				eventReason = 0;
			}
			else if(strstr(msgAttr, "outofcoverage"))
			{
				eventID = 202;
				eventReason = 0;
			}
			else if(strstr(msgAttr, "interfacedown"))
			{
				eventID = 203;
				eventReason = 0;
			}

			goto RESPONSE_OK;
		}

		msgAttr = strstr(m_recvBuf, "a=Stream_event:");
		if(msgAttr != NULL)
		{
			if(strstr(msgAttr, "streamok"))
			{
				eventID = 300;
				eventReason = 0;
			}
			else if(strstr(msgAttr, "activationfailed"))
			{
				eventID = 301;
				eventReason = 0;
			}
			else if(strstr(msgAttr, "deactivated"))
			{
				eventID = 302;
				eventReason = 0;
			}

			goto RESPONSE_OK;
		}


//Puck ANNOUNCE formated message
		line = nextLine = m_recvBuf;
		while(1)
		{
			line = nextLine;
			if(line == NULL)
				break;
			nextLine = GetNextLine(line);

			if(sscanf(line, "a=com.flotv.events.id:%d", &eventID) == 1)
				continue;

			if(sscanf(line, "a=com.flotv.events.reason:%d", &eventReason) == 1)
				continue;
		}



RESPONSE_OK:
		if(CUserOptions::UserOptions.m_mfCallBackFuncSet.OnEvent)
		{
			VOLOGI("Event id: %d, reason: %d", eventID, eventReason);
			CUserOptions::UserOptions.m_mfCallBackFuncSet.OnEvent(eventID, eventReason, NULL);
		}


		char cmdFormatString[256] = "RTSP/1.0 200 OK\r\n"
									"CSeq: %d\r\n"
									"Session: %s\r\n"
									"\r\n";
		sprintf(m_rtspCMDString, cmdFormatString, rtspCSeq, m_rtspSessionID);
		SendRTSPCommandToStreamingServer(m_rtspCMDString, 1);
	
	return 1;
}

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
	

	strcat(m_rtspCMDString, "\r\n");

	SendRTSPCommandToStreamingServer(m_rtspCMDString);
}
