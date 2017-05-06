#include "utility.h"
#if ENABLE_REAL
#include "network.h"
#include "RealRTSPClientEngine.h"
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include "RTSPMediaStreamSyncEngine.h"


#include "network.h"
#include "RTSPSession.h"
#include "RealTimeStreamingEngine.h"

#include "RDTMediaStream.h"
#include "RDTParser.h"
#include "TaskSchedulerEngine.h"
#include "RTSPClientEngine.h"
#include "MediaStreamSocket.h"
#if defined(_UNIX)
#include <sys/time.h>
#elif defined(_WINDOWS)
//#include <time.h>
//#include <sys/timeb.h>
#endif
#include "challenge.h"
//static const char* RM_VOGUID = "00000000-0000-0000-0000-000000000000";

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CRealRTSPClientEngine::CRealRTSPClientEngine(void)
:m_bchallengeVerified(false)
{
}

CRealRTSPClientEngine::~CRealRTSPClientEngine(void)
{
}
bool CRealRTSPClientEngine::PauseCmd(int waitResponseBySec)
{
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

		const char * cmdFormatString = "PAUSE %s RTSP/1.0\r\n"
			"CSeq: %d\r\n"
			"Session: %s\r\n"
			"User-Agent: %s\r\n"
			"\r\n";

		sprintf(m_rtspCMDString, cmdFormatString, m_URLofRequest, ++m_rtspCSeq, m_rtspSessionID, m_userAgentAttr);

		if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
			break;

		unsigned int responseBytes;
		char * responseLine;
		if(CUserOptions::UserOptions.m_useTCP>0)
		{
			streamingEngine->AddStreamReader(m_rtspSock, ReadStreamsOverTCP, this);
			CUserOptions::UserOptions.debugFlag = 111;
			responseBytes = HandleResponseViaTCP();
			responseLine  = m_rtspCMDString;
		}
		else
		{	
			if(!RecvResponseFromStreamingServer(responseBytes,VOS_CMD_PAUSE)) 
				break;
			responseLine = m_recvBuf;
		}
		if(CUserOptions::UserOptions.m_useTCP>0)
		{
			CUserOptions::UserOptions.debugFlag = 0;
		}

		if(!ParseResponseCode(responseLine, responseCode))
			break;

		if(responseCode != 200)
		{
			CUserOptions::UserOptions.errorID = E_USER_ERROR_BASE-responseCode;
			break;
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
			streamingEngine->AddStreamReader(m_rtspSock, HandleRTSPServerRequest, this);
		}

		for(int streamIndex=0; streamIndex<MediaStreamCount(); ++streamIndex)
		{
			//MediaStream(streamIndex)->UnscheduleReceiverReportTask();
		}
		m_pauseTime = timeGetTime();
		return true;

	}while(0);
	m_failConnectNum+=5;
	if(responseCode>400)
	{
		NotifyEvent(VO_EVENT_PAUSE_FAIL,CUserOptions::UserOptions.errorID);
		//NotifyEvent(VO_EVENT_SOCKET_ERR,CUserOptions::UserOptions.errorID);
		m_failConnectNum+=10;//it is an serious error

		CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt", "@@@@Parse pause fail  responseCode>400\n");
	}

	if(CUserOptions::UserOptions.m_useTCP>0)//do not process the response if there is error in tcp enviroment
	{
		sprintf(CLog::formatString,"@@@@pause fail via TCP %d\n",IVOSocket::GetSockLastError());
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", CLog::formatString);
		return true;
	}
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "@@@@Parse pause fail!!\n");
	return false;
}
bool CRealRTSPClientEngine::DescribeCmd(char * rtspURL)
{
	if(CUserOptions::UserOptions.m_URLType==UT_SDP2_LOCAL)
		return true;
	m_bchallengeVerified = false;
	do
	{
		if(!Init())
			break;
		CleanURL(rtspURL);//check the URL,if there is new line  symbol,remove it,if there is a space,add a %
		CUserOptions::UserOptions.m_currURL=rtspURL;

		if(1)//strlen(m_UAProfile)>0)
		{
			if(!OptionsCmd(rtspURL))
				break;
		}


		if(!ConnectToStreamingServer(rtspURL)) 
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "Describe:Can not connect streaming server! \r\n");
			break;
		}
		//const char * acceptAttr = "application/sdp";
		strcpy(cmdFormatString,"DESCRIBE %s RTSP/1.0\r\n"
			"CSeq: %d\r\n"
			"Accept: application/sdp\r\n"
			"%s"
			"User-Agent: %s\r\n"
			"Require: com.real.retain-entity-for-setup\r\n"
			"SupportsMaximumASMBandwidth: 1\r\n");
		char setupSessionAttr[64] = {0};
		if(m_rtspSessionID != NULL)
		{
			sprintf(setupSessionAttr, "Session: %s\r\n", m_rtspSessionID);
		}
		if(CUserOptions::UserOptions.m_nBandWidth!=0)
		{
			strcat(cmdFormatString,"Bandwidth:%u\r\n");
		}
		else if(strlen(m_UAProfile)>2)
		{
			strcat(cmdFormatString,"x-wap-profile: %s\r\n");
		}
		strcat(cmdFormatString,"\r\n");						 

		sprintf(m_rtspCMDString, cmdFormatString, rtspURL, ++m_rtspCSeq,setupSessionAttr,m_userAgentAttr,CUserOptions::UserOptions.m_nBandWidth,m_UAProfile);
		if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
			break;

		unsigned int responseBytes;
		if(!RecvResponseFromStreamingServer(responseBytes,VOS_CMD_DESCRIBE)) 
			break;
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "$$$Describe:receive response done! \r\n");
		char * responseLine = m_recvBuf;
		char * nextResponseLine = GetNextLine(responseLine);
		unsigned int responseCode = 0;
		if(!ParseResponseCode(responseLine, responseCode))
			break;
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "$$$Describe:ParseResponseCode done! \r\n");
		bool needRedirection = false;
		char * redirectionURL = NULL;
		if(responseCode == 301 || responseCode == 302) 
		{ // redirection 
			needRedirection = true;
			redirectionURL = new char[MAX_PATH*2];TRACE_NEW("rts_redirectionURL",redirectionURL);//MAX_PATH is small to handle some unregular URL
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "$$$Describe:Processing redirection_1! \r\n");
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
			//CLog::Log.MakeLog("flow.txt", "$$$Describe::dddd \n");
			if(sscanf(responseLine, "Content-Length: %d", &contentLength) == 1 || sscanf(responseLine, "Content-length: %d", &contentLength) == 1) 
				continue;
			if(ParseContentBase(responseLine))
				continue;
		}
		SAFE_DELETE_ARRAY(redirectionURL);
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "$$$Describe:before ReadNetDataExact! \r\n");
		if(contentLength > 0)
		{
			if(ReadNetDataExact(m_rtspSock, (unsigned char *)m_recvBuf, contentLength) != contentLength)
				break;
			m_recvBuf[contentLength] = '\0';
			if(CUserOptions::UserOptions.m_bMakeLog==LOG_DATA)
			{
				CLog::EnablePrintTime(false);
				CLog::Log.MakeLog(LL_AUDIO_DATA,"RTSPStream.sdp2", m_recvBuf);
			}
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", m_recvBuf);
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "\r\n");
		}
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "$$$Describe:after ReadNetDataExact \r\n");
		if (m_rtspSession ==NULL)
		{
			m_rtspSession = new CRTSPSession(this);TRACE_NEW("rts_m_rtspSession",m_rtspSession);
		}
		char* sdpBuf = m_recvBuf;
		if(CUserOptions::UserOptions.m_URLType==UT_SDP_URL&&m_sdpFileString)
		{
			int lenofContentBase = strlen(m_contentBase);
			if(CUserOptions::UserOptions.sdpURLLength!=lenofContentBase)
				CUserOptions::UserOptions.sdpURLLength=lenofContentBase;
			//CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "!!!get the sdp file\n");
			sdpBuf = m_sdpFileString;

		}
		if(!m_rtspSession->Init(sdpBuf, m_rtspServerAddr))
		{
			CUserOptions::UserOptions.errorID = E_SDP_PARSE_FAILED;
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "@@@@fail to init the rtspsession!!\n");
			break;
		}
		return true;

	}while(0);
	NotifyEvent(VO_EVENT_DESCRIBE_FAIL,CUserOptions::UserOptions.errorID);
	CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt", "@@@@Parse describe fail!!\n");
	return false;
}
CMediaStream* CRealRTSPClientEngine::CreateOneMediaStream(CRTSPSession* session)
{
	CMediaStream* stream= new CRDTMediaStream(session);TRACE_NEW("new CRDTMediaStream",stream);
	return stream;
}

bool CRealRTSPClientEngine::OptionsCmd(char * rtspURL)
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

		const char * cmdFormatString = "OPTIONS %s RTSP/1.0\r\n"
			"CSeq: %d\r\n"
			"User-Agent: %s\r\n"
			"Supported: ABD-1.0\r\n"
			"ClientChallenge: 7185f850b12aeab0357810aa04c427ef\r\n"
			"ClientID: WinNT_6.1_15.0.0.198_RealPlayer_R71APH14_en_686\r\n"
			"CompanyID: JRWFxiZ1Wv8f14HhTZxIgw==\r\n"
			"PlayerStarttime: [13/01/2012:14:07:41 08:00]\r\n"
			"GUID: 00000000-0000-0000-0000-000000000000\r\n"
			"Pragma: initiate-session\r\n"
			"RegionData: 0\r\n"
			"\r\n";
		sprintf(m_rtspCMDString, cmdFormatString, rtspURL, ++m_rtspCSeq, m_userAgentAttr);
					
		if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
			break;
		unsigned int responseBytes;
		if(!RecvResponseFromStreamingServer(responseBytes,VOS_CMD_OPTION)) 
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
		while(1) 
		{
			responseLine = nextResponseLine;
			if(responseLine == NULL)
				break;

			nextResponseLine = GetNextLine(responseLine);

			if(ParsePublicAttribute(responseLine))
				continue;
			if(ParseChallenge1(responseLine))
				continue;
			if(ParseSetupCMDResponse_Session(responseLine))
				continue;
		}

		return true;

	}while(0);
	m_failConnectNum+=5;
	NotifyEvent(VO_EVENT_OPTION_FAIL,CUserOptions::UserOptions.errorID);
	CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt", "@@@@Parse option fail!!\n");
	return false;
}
bool CRealRTSPClientEngine::ParseChallenge1(char* responseText)
{
	do
	{
		if(_strnicmp(responseText, "RealChallenge1:", 15) != 0)
			break;

	
		if(sscanf(responseText,"RealChallenge1:%s",m_challenge1)==1)
		{
			//CUserOptions::UserOptions.m_nSupportPause = 1;
		}
		else
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@ParseChallenge1,fail\n");
			break;

		}
		return true;

	}while(0);
	return false;
}

bool CRealRTSPClientEngine::ParseChallenge3(char* responseText)
{
	do
	{
		if(_strnicmp(responseText, "RealChallenge3:", 15) != 0)
			break;


		if(sscanf(responseText,"RealChallenge3: %[^,]",m_challenge1)==1)
		{
			//CUserOptions::UserOptions.m_nSupportPause = 1;
			char* t =strchr(responseText,'=');//[^\"\t\r\n])
			if(!(t&&sscanf(t,"=%[^\r\n]",m_checksum)==1))
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@ParseChallenge3 _ sdr,fail\n");
				break;
			}
		}
		else
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@ParseChallenge3,fail\n");
			break;

		}
		return true;

	}while(0);
	return false;
}
void CRealRTSPClientEngine::real_calc_response_and_checksum()
{
	
}
bool CRealRTSPClientEngine::Set_Parameter_Cmd(int paramType,void* value)
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
		char setupSessionAttr[64] = {0};
		if(m_rtspSessionID != NULL)
		{
			sprintf(setupSessionAttr, "Session: %s\r\n", m_rtspSessionID);
		}

		m_recvBuf[0] = '\0';
		const char * cmdFormatString = "SET_PARAMETER %s RTSP/1.0\r\n"
			"CSeq: %d\r\n"
			"User-Agent: %s\r\n"
			"%s"						//params
			"%s"						//Session attribute
			"\r\n";
		switch(paramType)
		{
		case SPT_REAL_STREAM_RULE:
			{
				int id0,id1;
				int rule0,rule1;
				id0=((CRDTMediaStream*)MediaStream(0))->GetStreamID();
				rule0 = ((CRDTMediaStream*)MediaStream(0))->GetASMRule2Stream();
				if(MediaStreamCount()==2)
				{
					
					id1 = ((CRDTMediaStream*)MediaStream(1))->GetStreamID();
					rule1 = ((CRDTMediaStream*)MediaStream(1))->GetASMRule2Stream();
					sprintf(m_recvBuf,"subscribe: stream=%d;rule=%d,stream=%d;rule=%d,stream=%d;rule=%d,stream=%d;rule=%d\r\n",
						id0,rule0,id0,rule0+1,id1,rule1,id1,rule1+1);
				}
				else
				{
					sprintf(m_recvBuf,"subscribe: stream=%d;rule=%d,stream=%d;rule=%d\r\n",
						id0,rule0,id0,rule0+1);
				}
			}
			break;
		case SPT_REAL_STREAM_BANDWIDTH:
			{
				int bandwidth=320000;
				int backoff = 0;
				sprintf(m_recvBuf,"SetDeliveryBandwidth: Bandwidth=%d;BackOff=%d\r\n",bandwidth,backoff);
			}
		    break;
		default:
		    break;
		}
		sprintf(m_rtspCMDString, cmdFormatString, rtspURL, ++m_rtspCSeq, m_userAgentAttr,m_recvBuf,setupSessionAttr);
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
bool CRealRTSPClientEngine::SetupCmd(bool streamingOverTCP)
{
	if(CUserOptions::UserOptions.m_URLType==UT_SDP2_LOCAL)
		return true;
	m_failConnectNum = 0;
	m_streamingOverTCP = streamingOverTCP;
	for(int streamIndex=0; streamIndex<MediaStreamCount(); ++streamIndex)
	{
		if(!SetupCmd_MediaStream(streamIndex))
		{
			NotifyEvent(VO_EVENT_SETUP_FAIL,CUserOptions::UserOptions.errorID);
			CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt","@@@@setup fail!\n");
			return false;
		}
	}
	if(m_rtspSession->GetApplicationStream())
	{
		if(!SetupCmd_MediaStream(2))
		{
			NotifyEvent(VO_EVENT_SETUP_FAIL,CUserOptions::UserOptions.errorID);
			CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt","@@@@setup appliaction channel fail!\n");
			return false;
		}
	}

	return true;
}
bool CRealRTSPClientEngine::SetupCmd_MediaStream(int streamIndex)
{
	

	CMediaStream * pMediaStream;
	if(streamIndex>1)
		pMediaStream= m_rtspSession->GetApplicationStream();
	else
		pMediaStream= MediaStream(streamIndex);

	do
	{
		const char * cmdFormatString = "SETUP %s/%s RTSP/1.0\r\n"
			"CSeq: %d\r\n"
			"%s"						//Session attribute
			"%s"                       //ETag attribute
			"%s"						//Transport attribute
			"%s"						//RealChallenge2
			"User-Agent: %s\r\n"
			"RDTFeatureLevel: 2\r\n"
			"\r\n";

		char setupSessionAttr[64] = {0};
		if(m_rtspSessionID != NULL)
		{
			sprintf(setupSessionAttr, "Session: %s\r\n", m_rtspSessionID);
		}
		m_recvBuf[0]='\0';
		struct RealChallenge* pRCH                = NULL;
		if(!m_bchallengeVerified)
		{
			/* We just create a new RealChallange object and set the Response1
			* on it.
			*/
			pRCH = CreateRealChallenge();
			RealChallengeResponse1( (BYTE*)m_challenge1,
				(BYTE*)pRCMagic1,
				RC_MAGIC1,
				RC_MAGIC2,
				pRCH);
			/* We now have all we need to construct the RealChallenge2
			* header. Again, this is sent to the server in the first
			* RTSP SETUP request */
			sprintf(m_recvBuf, "RealChallenge2:%-.128s, sd=%-.128s\r\n", (char*)pRCH->response, (char*)pRCH->trap);
		}
		char entityTagAttr[128] = {0};
		if(m_rtspSession->SessionEntityTag() != NULL)
		{
			//			sprintf(entityTagAttr, "If-Match: %s\r\n", m_rtspSession->SessionEntityTag());
		}

		char setupTransportAttr[128] = {0};
		if(CUserOptions::UserOptions.m_useTCP>0)
		{
			sprintf(setupTransportAttr, "Transport:x-real-rdt/tcp;mode=play\r\n");
		}
		else
		{
			sprintf(setupTransportAttr, "Transport: x-real-rdt/udp;client_port=%d;mode=play,x-real-rdt/tcp;mode=play\r\n", pMediaStream->ClientRTPPort());
		}

		sprintf(m_rtspCMDString, cmdFormatString, m_URLofRequest, pMediaStream->ControlAttr(), ++m_rtspCSeq, setupSessionAttr, entityTagAttr, setupTransportAttr, m_recvBuf,m_userAgentAttr);
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt",m_URLofRequest);
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt",pMediaStream->ControlAttr());
		if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
		{

			break;
		}

		unsigned int responseBytes;
		if(!RecvResponseFromStreamingServer(responseBytes,VOS_CMD_SETUP)) 
			break;

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
			if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "@@@@send SET_PARAMETER response fail\n");
				break;
			}

			if(!RecvResponseFromStreamingServer(responseBytes,VOS_CMD_SETUP)) 
				break;

			responseLine = m_recvBuf;
			nextResponseLine = GetNextLine(responseLine);
			responseCode = 0;
		}

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

			

			if(ParseSetupCMDResponse_Transport(responseLine, streamIndex))
			{
				CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
				CRDTParser* rdtParser = pRTSPMediaStreamSyncEngine->GetRDTParser();
				if(rdtParser)
				{
					int serverPort = pMediaStream->ServerRTCPPort();
					if(serverPort)
					{
						rdtParser->CreateRDTServerSock(serverPort);
					}
					else
					{
						rdtParser->SetRTSPSock(m_rtspSock);
						//rdtParser->CreateRDTServerSock(m_rtspServerPort);
					}

				}
				continue;
			}
			if(!m_bchallengeVerified)
			{
				if(ParseChallenge3(responseLine))
				{
					/*
					* Now we can use the RealChallenge3 strings to verify that this is a
					* RealServer.
					*/

					/* Compute what the server should have sent us. */
					RealChallengeResponse2(pRCH->response, (BYTE*)pRCMagic2, RC_MAGIC3, RC_MAGIC4, pRCH);
					/* Compare it to what we got from the server */
					int challengeOK = !strcmp((const char*) &pRCH->response[0], m_challenge1);
					int sdrOK		= !strcmp((const char*) &pRCH->trap[0], m_checksum);
					if(  challengeOK&&sdrOK)
					{
						/* Validation successful. */
						m_bchallengeVerified = true;
						CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "$$$$chanllege3 Validation successful\n");
					}
					continue;
				}
			}
		}
		if(pRCH)
		{
			free(pRCH);
			pRCH = NULL;
		}

		return true;

	}while(0);
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "@@@@Parse setup fail!!\n");

	return false;
}
bool    CRealRTSPClientEngine::ParseSetupCMDResponse_Transport(char * sdpParam, int streamIndex)
{
	CMediaStream * pMediaStream;
	if(streamIndex>1)
		pMediaStream= m_rtspSession->GetApplicationStream();
	else
		pMediaStream= MediaStream(streamIndex);

	do
	{
		if(_strnicmp(sdpParam, "Transport: ", 11) != 0) 
			break;

		sdpParam += 11;
		if(CUserOptions::UserOptions.m_useTCP>0)
		{
			char transportAttrField[64];
			if(sscanf(sdpParam, "%[^;]", transportAttrField) == 1)
			{
				int channelID;
				if(sscanf(transportAttrField, "interleaved=%d", &channelID) == 1) 
				{
					pMediaStream->SetRTPChannelID(channelID);
				}

			}
		}
		else
		{
			char transportAttrField[64];
			while(sscanf(sdpParam, "%[^;]", transportAttrField) == 1)
			{
				unsigned short serverRTPPort;
				if(sscanf(transportAttrField, "server_port=%hu", &serverRTPPort) == 1) 
				{
					pMediaStream->SetServerRTPPort(serverRTPPort);
					pMediaStream->SetServerRTCPPort(serverRTPPort);
				}

				sdpParam += strlen(transportAttrField) + 1;
			}
		}
		

		return true;

	}while(0);
	return false;
}
bool CRealRTSPClientEngine::PlayCmd(float startTime,float endTime)
{
	if(CUserOptions::UserOptions.m_URLType==UT_SDP2_LOCAL)
		return true;
	if(!CUserOptions::UserOptions.m_useTCP&&CUserOptions::UserOptions.m_nPortOpener)
	{
		//SendExtraRTPPacket();
	}
	unsigned int responseCode = 0;
	CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
	pRTSPMediaStreamSyncEngine->GetRDTParser()->SendRTTRequest(3);
	pRTSPMediaStreamSyncEngine->SetPlayRangeBegin(0.0);
	do
	{
		if(m_rtspSessionID == NULL)
			break;

		CRealTimeStreamingEngine * streamingEngine = CRealTimeStreamingEngine::CreateRealTimeStreamingEngine();
		if(streamingEngine == NULL)
			break;
		streamingEngine->RemoveStreamReader(m_rtspSock);

		if(startTime >= 0)
		{

			if(endTime>0)
			{
				const char * cmdFormatString = "PLAY %s RTSP/1.0\r\n"
					"CSeq: %d\r\n"
					"Session: %s\r\n"
					"Range: npt=%.3f-%.3f\r\n"
					"\r\n";
				sprintf(m_rtspCMDString, cmdFormatString, m_URLofRequest, ++m_rtspCSeq, m_rtspSessionID, startTime,endTime);

			}
			else
			{
				const char *cmdFormatString = "PLAY %s RTSP/1.0\r\n"
					"CSeq: %d\r\n"
					"Session: %s\r\n"
					"Range: npt=%.3f-\r\n"
					"\r\n";
				sprintf(m_rtspCMDString, cmdFormatString, m_URLofRequest, ++m_rtspCSeq, m_rtspSessionID, startTime);
			}
		}
		else
		{
			const char * cmdFormatString = "PLAY %s RTSP/1.0\r\n"
				"CSeq: %d\r\n"
				"Session: %s\r\n"
				"\r\n";
			sprintf(m_rtspCMDString, cmdFormatString, m_URLofRequest, ++m_rtspCSeq, m_rtspSessionID);
		}
		if(!SendRTSPCommandToStreamingServer(m_rtspCMDString))
			break;

		unsigned int responseBytes;
		char * responseLine;
		if(CUserOptions::UserOptions.m_useTCP>0)
		{
			//m_streamLength = 0;
			//m_streamPos = 0;
			SetSocketRecvBufSize(m_rtspSock, 100*1024);
			streamingEngine->AddStreamReader(m_rtspSock, ReadStreamsOverTCP, this);
			int count = 5;
			bool IsPlayResponse=false;
			while (count-->0)
			{
				responseBytes = HandleResponseViaTCP();
				IsPlayResponse = (strstr(m_rtspCMDString,"Range")!=NULL)||(strstr(m_rtspCMDString,"range")!=NULL);
				if(IsPlayResponse)
				{
					break;
				}
				else
				{
					//Sleep(10);
				}
			}
			if(IsPlayResponse==false)
			{
				CLog::Log.MakeLog(LL_RTSP_ERR,"tcp_error.txt","can not get the play response\n");
			}
			responseLine  = m_rtspCMDString;
		}
		else
		{	
			if(!RecvResponseFromStreamingServer(responseBytes,VOS_CMD_PLAY)) 
				break;
			responseLine = m_recvBuf;
		}

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
		float begin = 0.0, end  = 0.0;
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
				//if the response range_begin is different from the startTime by over 15 seconds,that is an error
				if(startTime>0&&abs(begin-startTime)>15)
				{
					sprintf(CLog::formatString,"the response range_begin is different with the startTime by over 10 seconds(%.3f.%.3f),that is an error\n",begin,startTime);
					CLog::Log.MakeLog(LL_RTSP_ERR,"scan_unsupported.txt",CLog::formatString);
					//so far,endTime>0 means that the checking if scan is supported is ongoing
					if(endTime>0)
						return false;
				}

#define INVALID_PLAY_RANGE(a)		(a<0||a>(24*60*60))
				if(!INVALID_PLAY_RANGE(duration) && (m_rtspSession->SessionDuration() == 0))
				{
					m_rtspSession->SetSessionDuration(duration);
					//NotifyEvent(0,0);
				}
				continue;
			}
			if(ParseRTPInfoHeader(responseLine))
			{
				hasRTP_info = 1;
				continue;
			}
		}
		if(!hasRTP_info)
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","WARNNING:no RTP-info in this play response!!!\n");
			for(int streamIndex=0; streamIndex<m_rtspSession->MediaStreamCount(); ++streamIndex)
			{
				m_rtspSession->MediaStream(streamIndex)->ResetRTPInfo();
			}
		}

		if(m_streamingOverTCP)
		{

		}
		else
		{
			streamingEngine->AddStreamReader(m_rtspSock, HandleRTSPServerRequest, this);
			streamingEngine->AddStreamReader(pRTSPMediaStreamSyncEngine->GetRDTStreamSock()->GetMediaStreamSock(), CRDTParser::ReadRDTStream, pRTSPMediaStreamSyncEngine->GetRDTParser());
		}

		if(m_taskToken == 0)
		{
			m_taskToken = CTaskSchedulerEngine::CreateTaskSchedulerEngine()->ScheduleTask(10*1000, (TaskFunction *)CRTSPClientEngine::NotifyStreamingServerClientBeLiving, this);
		}
		m_playTime = timeGetTime();
		int intervalBetweenPauseAndPlay = 0;
		if(m_pauseTime>=0)
		{
			intervalBetweenPauseAndPlay = m_playTime-m_pauseTime+100;
			if(intervalBetweenPauseAndPlay<0)
				intervalBetweenPauseAndPlay = 0;
		}
		for(int streamIndex=0; streamIndex<MediaStreamCount(); ++streamIndex)
		{
			//MediaStream(streamIndex)->ScheduleReceiverReportTask();//TODO_REAL RDT_ACK...
			if(MediaStream(streamIndex)->IsNewStart())
			{
				MediaStream(streamIndex)->SetStreamStartTime((int)(begin*1000));
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

int CRealRTSPClientEngine::DoReadStreamsOverTCP(int flag)
{
	
	unsigned char streamChannelFlag = 0;
	unsigned char streamChannelID = 0;
	unsigned short streamChannelDataSize = 0;
	unsigned char * streamData = (unsigned char *)m_recvBuf;
	unsigned int curPos = 0;

	do
	{
		int result=IsNetReadyToRead(m_rtspSock,CUserOptions::UserOptions.m_nRTSPTimeOut);//if there is no data coming in m_nRTSPTimeOut seconds,there is something wrong in the network
		if(result<=0)
		{
			CUserOptions::UserOptions.errorID = E_NETWORK_TIMEOUT;
			CLog::Log.MakeLog(LL_SOCKET_ERR,"socket_timeout.txt","@@@@time out to get the RTSP response via TCP\n");
			return 1;
		}

		int readNum = ReadNetData(m_rtspSock, streamData+m_streamLength, RECVBUFSIZE-m_streamLength);
		if(readNum<=0)
		{
			CLog::Log.MakeLog(LL_RTSP_ERR,"tcp.txt","DoReadStreamsOverTCP:readNum<=0\n");
			break;
		}
		m_streamLength += readNum;

		do
		{ 
			curPos = m_streamPos;

			if(curPos >= m_streamLength) // rtsp channel flag '$' 1 Byte
				break;
			streamChannelFlag = streamData[curPos++];
			if(streamChannelFlag != '$')
				break;

			if(curPos >= m_streamLength) // rtsp channel ID  1 Byte
				break;
			streamChannelID = streamData[curPos++];

			if((curPos+1) >= m_streamLength) // rtsp channel data size  2 Byte
				break;
			memcpy(&streamChannelDataSize, streamData+curPos, 2); 
			curPos += 2;
			streamChannelDataSize = NetToHostShort(streamChannelDataSize);

			if((curPos+streamChannelDataSize) >= m_streamLength) // rtsp channel data
				break;
			m_streamPos = curPos + streamChannelDataSize;

			if(CUserOptions::UserOptions.isSeeking==false)//do not process the packet when seeking
			{
				CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
				pRTSPMediaStreamSyncEngine->GetRDTParser()->ReadRDTStreamFromLocal(streamChannelID,streamData+curPos, streamChannelDataSize);
			
			}
			else
			{
				//break;
			}
		}while(1);



		m_streamLength -= m_streamPos;
		memmove(streamData, streamData+m_streamPos, m_streamLength);			
		m_streamPos = 0;
		if(flag)
		{
			sprintf(CLog::formatString,"m_streamPos=%d,m_streamLength=%d,curPos=%d,streamData[0]=%c\n",m_streamPos,m_streamLength,curPos,streamData[0]);
		}
		if(m_streamLength > 0 && streamData[0] != '$') // rtsp command from server
		{

			int minlen = min(1024,m_streamLength);
			memcpy(m_rtspCMDString,streamData,minlen);
			m_rtspCMDString[1023]='\0';
			char* end=strstr(m_rtspCMDString,"\r\n\r\n");
			if(end)
			{
				int len = end-m_rtspCMDString+4;
				m_rtspCMDString[len]='\0';
				m_streamLength -= len;
				memmove(streamData, streamData+len, m_streamLength);

			}
			//no matter which response it is,exit
			break;
		}
		else
		{
			//sprintf(CLog::formatString,"somehow,exit,m_streamLength =%d,char=%c\n",m_streamLength,streamData[0]);
			//_TraceReadStreamsOverTCP(flag,"tcp.txt",CLog::formatString);
			if (flag!=GET_RTSP_RESPONSE_VIATCP)//if it is to get data,exit,otherwise go on seeking the response
				break;
		}


	}while(1);

	return 0;

}
#endif//#if ENABLE_REAL
