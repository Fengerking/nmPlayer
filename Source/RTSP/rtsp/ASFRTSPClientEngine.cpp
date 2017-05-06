#include "network.h"
#include "ASFRTSPClientEngine.h"
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include "RTSPMediaStreamSyncEngine.h"
#include "utility.h"
#include "network.h"
#include "RTSPSession.h"
#include "RealTimeStreamingEngine.h"
#include "TaskSchedulerEngine.h"
#include "RTSPClientEngine.h"
#include "MediaStreamSocket.h"
#include "MediaStream.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CASFRTSPClientEngine::CASFRTSPClientEngine(void)
{
}

CASFRTSPClientEngine::~CASFRTSPClientEngine(void)
{
}
bool CASFRTSPClientEngine::PauseCmd()
{
	return CRTSPClientEngine::PauseCmd();
}
bool CASFRTSPClientEngine::DescribeCmd(char * rtspURL)
{
	return CRTSPClientEngine::DescribeCmd(rtspURL);
}
CMediaStream* CASFRTSPClientEngine::CreateOneMediaStream(CRTSPSession* session)
{
	CMediaStream* stream= new CMediaStream(session);TRACE_NEW("CreateOneMediaStream",stream);
	
	return stream;
}

bool CASFRTSPClientEngine::OptionsCmd(char * rtspURL)
{
	return CRTSPClientEngine::OptionsCmd(rtspURL);
}

bool CASFRTSPClientEngine::Set_Parameter_Cmd(int paramType,void* value)
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
bool CASFRTSPClientEngine::SetupCmd(bool streamingOverTCP)
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
bool CASFRTSPClientEngine::SetupCmd_MediaStream(int streamIndex)
{
	return CRTSPClientEngine::SetupCmd_MediaStream(streamIndex);	
}
bool    CASFRTSPClientEngine::ParseSetupCMDResponse_Transport(char * sdpParam, int streamIndex)
{
	
	return true;
}
bool CASFRTSPClientEngine::PlayCmd(float startTime,float endTime)
{
	return CRTSPClientEngine::PlayCmd(startTime,endTime);
}

