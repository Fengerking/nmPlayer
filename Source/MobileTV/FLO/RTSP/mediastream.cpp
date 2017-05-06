
#include <memory.h>
#include <stdio.h>
#include <string.h>

#include "voFLOEngine.h"

#include "ctype.h"
#include "utility.h"
#include "BVUtility.h"
#include "network.h"
#include "getmp4vol.h"
#include "ParseSPS.h"
#include "RealTimeStreamingEngine.h"
#include "RTSPMediaStreamSyncEngine.h"
#include "RTSPSession.h"
#include "MediaStreamSocket.h"

#include "RTPSenderTable.h"
#include "RTPParser.h"
#include "RTCPParser.h"
#include "MPEG4GenericAudioRTPParser.h"
#include "MPEG4LATMAudioRTPParser.h"
#include "H264VideoRTPParser.h"
#include "mylog.h"

#include "MediaStream.h"
#include "DxManager.h"
#define LOG_TAG "FLOEngine"
#include "voLog.h"
unsigned char CMediaStream::m_streamChannelIDGenerator = 0;

#define DEFAULT_FRAMERATE 15

CMediaStream::CMediaStream(CRTSPSession * rtspSession)
: m_controlAttr(NULL)
, m_mediaStreamStartTime(0)
, m_mediaStreamEndTime(0)
, m_rtpPayloadType(0)
, m_rtpTimestampFrequency(0)
, m_videoWidth(320)
, m_videoHeight(240)
, m_videoFPS(0)
, m_audioChannels(0)
, m_audioSampleRate(41000)
, m_audioBits(16)
, m_mediaAddr(NULL)
, m_clientRTPPort(0)
, m_clientRTCPPort(0)
, m_serverRTPPort(0)
, m_serverRTCPPort(0)
, m_rtspSession(rtspSession)
, m_rtpParser(NULL)
, m_rtcpParser(NULL)
//, m_mediaFrameSink(NULL)
, m_fmtpPacketizationMode(0)
, m_fmtpProfileLevelID(0)
, m_fmtpSizeLength(0)
, m_fmtpIndexLength(0)
, m_fmtpIndexDeltaLength(0)
, m_fmtpOctetAlign(1)
, m_fmtpInterleaving(0)
, m_fmtpRobustSorting(0)
, m_fmtpCRC(0)
, m_codecHeadData(NULL)
, m_codecHeadDataLen(0)
, m_mediaStreamBandwidthAS(0)
, m_hasBeenSynchronized(false)
, m_streamSyncTimestamp(-1)
, m_streamTimestamp(-1)
, m_streamRestartTime(0)
, m_streamCurTime(0)
, m_streamSyncNTPAfterSync(-1)
, m_nPausedTime(0)
, m_nCurPlayTime(0)
, m_streamTimeBeforeSync(0)
, m_framerate(DEFAULT_FRAMERATE)
, m_streamStartTime(0)
, m_dataSyncbySeqNum(true)
, m_firstTimeStamp(-1)
, m_previousPos(-1)
, m_adjustTimeBetweenPauseAndPlay(0)
, m_isVideo(true)
, m_rtpStreamSocket(NULL)
, m_rtcpStreamSocket(NULL)
, m_frameNum(0)
, m_previousFrameIsIntra(0)
, m_previousFrameTime(-1)

, m_streamSyncWallClockTime(NULL)
, m_baseSyncWallClock(NULL)
, m_streamWallClockTime(NULL)
, m_lengthSizeOfNALU(0)
#if SUPPORT_ADAPTATION
, m_reportFreq(0)
#endif//
, m_gTmpBuf(NULL)
, m_h264ConfigNotBase64(0)
, m_streamNum(-1)
, m_timePerFrame(0)
, m_packetLoss(false)
, m_scaleAACPlus(0)
{
	memset(m_mediaName, 0, 16);
	memset(m_codecName, 0, 32);
	memset(m_protocolName, 0, 16);
	memset(m_fmtpConfig, 0, 256);
	memset(m_fmtpSpropParameterSets, 0, 256);
	memset(m_fmtpMode, 0, 32);
	ResetRTPInfo();
	ResetPreviousRTCPData();
	
	 m_previousTimeStamp = 0;
	 m_frameCount = 0;
	 m_totalTime = 0;
	 m_done = false;
	 m_isNewStart = true;
	 m_delayTime = 0;
	 m_recalFrameNum = 0;
	
	m_nRTCPSyncNum = 0;
	m_errPackNum = 0;

#if _DUMP_AVTIMESTAMP
	m_hAudioTS = NULL;
	m_hVideoTS = NULL;
#endif //_DUMP_AVTIMESTAMP

#if _DUMP_H264FRAME
	m_hDumpStreams = NULL;
#endif //_DUMP_H264FRAME
}

CMediaStream::~CMediaStream(void)
{
	SAFE_DELETE_ARRAY(m_controlAttr);
	SAFE_DELETE(m_mediaAddr);
	SAFE_DELETE_ARRAY(m_codecHeadData);
	SAFE_DELETE(m_rtpParser);
	SAFE_DELETE(m_rtcpParser);
//	SAFE_DELETE(m_mediaFrameSink);
	SAFE_DELETE(m_streamSyncWallClockTime);
	SAFE_DELETE(m_baseSyncWallClock);
	SAFE_DELETE(m_streamWallClockTime);
	SAFE_DELETE(m_gTmpBuf);

#if _DUMP_AVTIMESTAMP
	if(m_hAudioTS != NULL)
	{
		fclose(m_hAudioTS);
		m_hAudioTS = NULL;
	}
	if(m_hVideoTS != NULL)
	{
		fclose(m_hVideoTS);
		m_hVideoTS = NULL;
	}
#endif //_DUMP_AVTIMESTAMP

#if _DUMP_H264FRAME
	if(m_hDumpStreams != NULL)
	{
		fclose(m_hDumpStreams);
		m_hDumpStreams = NULL;
	}
#endif //_DUMP_H264FRAME
}
void CMediaStream::NotifyErrPack(){
	m_rtspSession->ProcessErrPack();
}
void CMediaStream::retrieveAllPacketsFromRTPBuffer()
{
	//CLog::Log.MakeLog(LL_RTSP_ERR,"rtsp.txt","retrieveAllPacketsFromRTPBuffer()\n");
	if(m_rtpParser)
		return m_rtpParser->RetrieveAllPack();
}

void CMediaStream::UpdateSocket()
{
	if(m_rtpParser && m_rtpStreamSocket)
		m_rtpParser->UpdateSocket(m_rtpStreamSocket);
	else
		CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt", "m_rtpParser&&m_rtpStreamSocket fail\n");

	if(m_rtcpParser && m_rtcpStreamSocket)
		m_rtcpParser->UpdateSocket(m_rtcpStreamSocket);
	else
		CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt", "m_rtcpParser&&m_rtcpStreamSocket fail\n");
}

bool CMediaStream::SetTransportProtocol(bool streamingOverTCP)
{
	if(streamingOverTCP)
		return false;
	
	do
	{
		CMediaStreamSocket *rtpStreamSock=NULL;
		CMediaStreamSocket *rtcpStreamSock=NULL;
		int minPort = CUserOptions::UserOptions.m_nUDPPortFrom;
		int maxPort = CUserOptions::UserOptions.m_nUDPPortTo;
		minPort += (int)((maxPort-minPort)/4*CUserOptions::UserOptions.portScale);

		if(minPort<1000)
			minPort = CUserOptions::UserOptions.m_nUDPPortFrom;	
		while(minPort<3000)
			minPort+=500;
		if(minPort>maxPort)
			minPort =maxPort-4;
		if(minPort&1)
			minPort+=1;

		int max_getport = 16;//maxPort-minPort;
		if(IsAudioStream())
			minPort+=2;//(max_getport/4)&0xfffffffe;

		m_clientRTPPort=minPort;
		while(max_getport-->0)//it is safe to limit the count of fetching port
		{
			rtpStreamSock = new CMediaStreamSocket(m_mediaAddr->family, 0, m_clientRTPPort);
			//m_clientRTPPort = rtpStreamSock->GetMediaStreamSockLocalPort();

			if((m_clientRTPPort & 1) == 0&&(m_clientRTPPort>=minPort&&m_clientRTPPort<=maxPort))
			{
				rtcpStreamSock = new CMediaStreamSocket(m_mediaAddr->family, 0, m_clientRTPPort + 1);

				m_clientRTCPPort = rtcpStreamSock->GetMediaStreamSockLocalPort();
				break;
			}
			else//error!!
			{
				char* format=CLog::formatString;
				sprintf(format,"%d:socket warning:invalid RTP port=%d valid(%d,%d)\n",max_getport,m_clientRTPPort,minPort,maxPort);
				CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt", format);

				//error resilience if it still fails at last time 
				if(m_clientRTPPort>maxPort)
					m_clientRTPPort-=2;
				else if(m_clientRTPPort<minPort)
					m_clientRTPPort = minPort + 8;
				SAFE_DELETE(rtpStreamSock);
				rtpStreamSock = NULL;
			}
		}
	
#define  MAX_SOCKET_BUF (1024 * 500)
#define  STEP_SIZE (1024*16)
		if(m_clientRTPPort==0)
			break;
		int actualSize;
		int setSize=MAX_SOCKET_BUF;
		
		actualSize=rtpStreamSock->SetSockRecvBufSize(setSize);
			


		m_rtpStreamSocket  = rtpStreamSock;
		m_rtcpStreamSocket = rtcpStreamSock;
		
		return true;
	}while(0);
	
	return false;
}

bool CMediaStream::Init(char * &sdpParam)
{
	if(m_mediaAddr == NULL)
	{
		m_mediaAddr = new struct voSocketAddress;
		if(m_mediaAddr == NULL)
			return false;
	}
	memcpy(m_mediaAddr, m_rtspSession->SessionAddr(), sizeof(voSocketAddress));

	char * nextSDPLine = GetNextLine(sdpParam);

	if(!ParseSDPParam_m(sdpParam))
		return false;

	while(1)
	{
		sdpParam = nextSDPLine;
		if(sdpParam == NULL)
			break;

		if(sdpParam[0] == 'm')
			break;

		nextSDPLine = GetNextLine(sdpParam);

		if(ParseSDPParam_c(sdpParam))
			continue;
		if(ParseSDPAttribute_rtpmap(sdpParam))
			continue;
		if(ParseSDPAttribute_stream(sdpParam))
			continue;
		
		if(ParseSDPAttribute_control(sdpParam))
			continue;
		if(ParseSDPAttribute_range(sdpParam))
			continue;
		if(ParseSDPAttribute_fmtp(sdpParam))
			continue;
		if(ParseSDPAttribute_cliprect(sdpParam))
			continue;
		if(ParseSDPAttribute_framesize(sdpParam))
			continue;
		if(ParseSDPAttribute_framerate(sdpParam))
			continue;

#if SUPPORT_ADAPTATION
		if(ParseSDPAttribute_3gppAdapation(sdpParam))
			continue;
#endif//
	}
	
#if TRACE_FLOW
	CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "$$$CMediaStream::Init one mediastream parse done\r\n");
#endif//TRACE_FLOW
	if(IsVideoStream())
	{
		CUserOptions::UserOptions.outInfo.clip_width	= m_videoWidth;
		CUserOptions::UserOptions.outInfo.clip_height	= m_videoHeight;
	}
	
	do
	{
		CMediaStreamSocket * rtpStreamSock = NULL;
		CMediaStreamSocket * rtcpStreamSock = NULL;
		m_rtpStreamSocket  = rtpStreamSock;
		m_rtcpStreamSocket = rtcpStreamSock;


		if(CUserOptions::UserOptions.m_useTCP<=0)
		{
			//TODO20070515:check port range
			if(!SetTransportProtocol(false))
				break;
		}

		
		if(!CreateRTPPayloadParser(m_rtpStreamSocket, m_rtcpStreamSocket))
			break;
		
		//m_rtpStreamSocket  = rtpStreamSock;
		//m_rtcpStreamSocket = rtcpStreamSock;
		//m_mediaFrameSink = new CMediaFrameSink(this);TRACE_NEW("new CMediaFrameSink() m_mediaFrameSink",m_mediaFrameSink);
		//if(m_mediaFrameSink == NULL)
		//	break;

		m_streamSyncWallClockTime = new struct timeval;
		if(m_streamSyncWallClockTime == NULL)
			break;
		
		m_streamSyncWallClockTime->tv_sec = m_streamSyncWallClockTime->tv_usec = 0;
		m_baseSyncWallClock = new struct timeval;
		if(m_baseSyncWallClock == NULL)
			break;
		m_baseSyncWallClock->tv_sec = m_baseSyncWallClock->tv_usec = 0;
		
		m_streamWallClockTime = new struct timeval;
		if(m_streamWallClockTime == NULL)
			break;
		m_streamWallClockTime->tv_sec = m_streamWallClockTime->tv_usec = 0;

		CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
		if(pRTSPMediaStreamSyncEngine == NULL)
			break;
		m_rtpChannelID = m_streamChannelIDGenerator++;
		m_rtcpChannelID = m_streamChannelIDGenerator++;
		
#if TRACE_FLOW
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "$$$CMediaStream::Init before AddMediaStream\r\n");
#endif//TRACE_FLOW
		
		pRTSPMediaStreamSyncEngine->AddMediaStream(this);
		
#if TRACE_FLOW
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "$$$CMediaStream::Init all done\r\n");
#endif//TRACE_FLOW
		
		return true;

	}while(0);

	return false;
}

bool CMediaStream::IsVideoH264AVC()
{
	return _stricmp(m_codecName,"X-HX-AVC1")==0;//||m_h264ConfigNotBase64==1;
}
bool CMediaStream::IsVideoH264()
{
	return _stricmp(m_codecName, "H264") == 0||_stricmp(m_codecName,"X-HX-AVC1")==0;
}
bool CMediaStream::IsVideoMpeg4()
{
	return _stricmp(m_codecName, "MP4V-ES") == 0||_stricmp(m_codecName, "X-RN-MP4") == 0;
}
bool CMediaStream::IsVideoH263()
{
	return _stricmp(m_codecName, "H263-2000") == 0 || _stricmp(m_codecName, "H263-1998") == 0;
}
bool CMediaStream::IsVideoRMVideo()
{
	return _stricmp(m_codecName, "x-pn-realvideo") == 0 ;
}
bool CMediaStream::IsVideoWMV()
{
	return IsVideoStream()&&_stricmp(m_codecName, "x-asf-pf") == 0 ;
}
bool CMediaStream::IsAudioWMA()
{
	return IsAudioStream()&&_stricmp(m_codecName, "x-asf-pf") == 0 ;
}
bool CMediaStream::IsAudioAMRNB()
{
	return _stricmp(m_codecName, "AMR") == 0;
}
bool CMediaStream::IsAudioAMRWB()
{
	return _stricmp(m_codecName, "AMR-WB") == 0;
}
bool CMediaStream::IsAudioAMRWBP()
{
	return _stricmp(m_codecName, "AMR-WBP") == 0;
}
bool CMediaStream::IsAudioMP3()
{
	return _stricmp(m_codecName, "MPA-ROBUST") == 0;
}
bool CMediaStream::IsAudioAAC()
{
	return _stricmp(m_codecName, "MPEG4-GENERIC") == 0||_stricmp(m_codecName, "MP4A-LATM") == 0;
}
bool CMediaStream::IsAudioQCELP()
{
	return _stricmp(m_codecName, "QCELP") == 0;
}
bool CMediaStream::IsAudioRMAudio()
{
	return _stricmp(m_codecName, "x-pn-realaudio") == 0 ;
}
bool CMediaStream::CreateRTPPayloadParser(CMediaStreamSocket * rtpStreamSock, CMediaStreamSocket * rtcpStreamSock)
{
	if(IsVideoStream())
	{
		if(IsVideoH264())
		{
			m_rtpParser = new CH264VideoRTPParser(this, rtpStreamSock);TRACE_NEW("new CH264VideoRTPParser m_rtpParser",m_rtpParser);
		}
		
	}

	if(IsAudioStream())
	{
		if(_stricmp(m_codecName, "MPEG4-GENERIC") == 0)
		{
			m_rtpParser = new CMPEG4GenericAudioRTPParser(this, rtpStreamSock, m_fmtpSizeLength, m_fmtpIndexLength, m_fmtpIndexDeltaLength);TRACE_NEW("new CMPEG4GenericAudioRTPParser m_rtpParser",m_rtpParser);
		}
#ifdef WIN32
		else if(_stricmp(m_codecName, "MP4A-LATM") == 0)
		{
			m_rtpParser = new CMPEG4LATMAudioRTPParser(this, rtpStreamSock);TRACE_NEW("new CMPEG4LATMAudioRTPParser m_rtpParser",m_rtpParser);
		}
#endif
	}

	if(m_rtpParser != NULL)
	{
		unsigned int mediaStreamBandwidth = m_mediaStreamBandwidthAS;
		if(mediaStreamBandwidth == 0)
		{
//			mediaStreamBandwidth = m_rtspSession->SessionBandwidth();
		}
		if(mediaStreamBandwidth == 0)
		{
			mediaStreamBandwidth = 300;
		}

		m_rtcpParser = new CRTCPParser(this, rtcpStreamSock, mediaStreamBandwidth);TRACE_NEW("new CRTCPParser m_rtcpParser",m_rtcpParser);
	}
	
	int result = m_rtpParser != NULL && m_rtcpParser != NULL;
	if(!result)
	{
		CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to creat RTP/RTCP parser\n");
	}
	
	return result;
}

bool CMediaStream::ParseSDPParam_m(char *sdpParam)
{
	if(sscanf(sdpParam, "m=%s %hu RTP/AVP %u", m_mediaName, &m_clientRTPPort, &m_rtpPayloadType) == 3)
	{
		if(_stricmp(m_mediaName, "Video") != 0)
			m_isVideo = false;
		else
			m_isVideo = true;
		return true;
	}
	else
	{
		//CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse Param_m\n");
	
	}
	return false;
}

bool CMediaStream::ParseSDPParam_c(char * sdpParam)
{
	do
	{
		if(_strnicmp(sdpParam, "c=IN IP", 7) != 0)
		{
			//CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse c=IN IP\n");
			break;
		}

		char remoteIPString[64] = {0};
		if(sscanf(sdpParam, "c=IN IP4 %[^/ ]", remoteIPString) == 1)
		{// "c=IN IP4 *.*.*.*/ttl"
			if(_stricmp(remoteIPString, "224") < 0)
			{
				//CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse remoteIPString\n");
				break;
			}
		}
		else if(sscanf(sdpParam, "c=IN IP6 %s", remoteIPString) == 1)
		{// "c=IN IP6 *:*:*:*:*:*:*:*"

			if(_stricmp(remoteIPString, "FF") < 0)
			{
				//CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse remoteIPString2\n");
				break;
			}
		}

#if !CP_SOCKET
		struct addrinfo hints;
		memset(&hints, 0, sizeof(addrinfo));
		hints.ai_family   = PF_UNSPEC;
		hints.ai_protocol = IPPROTO_UDP;
		hints.ai_socktype = SOCK_DGRAM;
		struct addrinfo * AddrInfo = NULL;
		if(getaddrinfo(remoteIPString, NULL, &hints, (struct addrinfo **)&AddrInfo) != 0)
			break;

		struct addrinfo * AI = AddrInfo;
		while(AI != NULL)
		{
			if(AI->ai_protocol == IPPROTO_UDP && AI->ai_socktype == SOCK_DGRAM)
			{
				memset(m_mediaAddr, 0, sizeof(sockaddr_storage));
				memcpy(m_mediaAddr, AI->ai_addr, AI->ai_addrlen);
				break;
			}

			AI = AI->ai_next;
		}
		freeaddrinfo(AddrInfo);
		if(AI == NULL)
			break;
#else//CP_SOCKET
		m_mediaAddr->family		=  VAF_UNSPEC;
		m_mediaAddr->port		=  0;
		m_mediaAddr->protocol	=  VPT_UDP;
		m_mediaAddr->sockType	=  VST_SOCK_DGRAM;
		int ret =IVOSocket::GetPeerAddressByURL(remoteIPString,strlen(remoteIPString),m_mediaAddr);
		if(ret)
			break;
#endif//CP_SOCKET
		return true;

	}while(0);

	return false;
}


bool CMediaStream::ParseSDPAttribute_rtpmap(char * sdpParam)
{
	if(sscanf(sdpParam,     "a=rtpmap: %u %[^/]/%u/%u", &m_rtpPayloadType, m_codecName, &m_rtpTimestampFrequency, &m_audioChannels) == 4
		|| sscanf(sdpParam, "a=rtpmap: %u %[^/]/%u", &m_rtpPayloadType, m_codecName, &m_rtpTimestampFrequency) == 3
		|| sscanf(sdpParam, "a=rtpmap: %u %s", &m_rtpPayloadType, m_codecName) == 2)
	{
		if(IsAudioStream())//m_audioChannels == 0)
		{
			m_audioSampleRate = m_rtpTimestampFrequency;
			if(_stricmp(m_codecName, "MPEG4-GENERIC") == 0||_stricmp(m_codecName, "MP4A-LATM") == 0)
			{
				m_framerate = m_audioSampleRate/1024;
				if(m_audioChannels == 0) m_audioChannels = 2;
				CUserOptions::UserOptions.outInfo.codecType[1]=kCodecTypeMPEG4AAC;
				m_timePerFrame = (int)(1024000.0/m_audioSampleRate);
				m_scaleAACPlus = 1;

			}
			else if(_stricmp(m_codecName, "AMR-WBP") == 0)
			{
				CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse frame rate for AMR-WBP\n");
				m_framerate = 50;
				if(m_audioChannels == 0) m_audioChannels = 2;
				CUserOptions::UserOptions.outInfo.codecType[1]=kCodecTypeUnknown;
				m_timePerFrame = 20;
			}
			else if(_stricmp(m_codecName, "AMR-WB") == 0)
			{
				CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse frame rate for AMR-WB\n");
				m_framerate = 50;
				if(m_audioChannels == 0) m_audioChannels = 1;
				CUserOptions::UserOptions.outInfo.codecType[1]=kCodecTypeGSMAMRWB;
				m_timePerFrame = 20;
			}
			else if(_stricmp(m_codecName, "AMR") == 0)
			{
				m_framerate = 50;
				if(m_audioChannels == 0) m_audioChannels = 1;
				CUserOptions::UserOptions.outInfo.codecType[1]=kCodecTypeGSMAMRNB;
				m_timePerFrame = 20;

			}
			else if(_stricmp(m_codecName, "QCELP") == 0)
			{
				m_framerate = 50;
				if(m_audioChannels == 0) m_audioChannels = 1;
				CUserOptions::UserOptions.outInfo.codecType[1]=kCodecTypeQCELP;
				m_timePerFrame = 20;
			}
			else if(IsAudioWMA())
			{
				m_framerate = 50;
				if(m_audioChannels == 0) m_audioChannels = 1;
			}
			else
			{
				m_framerate = 25;
				m_audioChannels = 2;
				CUserOptions::UserOptions.outInfo.codecType[1]=kCodecTypeUnknown;
				CLog::Log.MakeLog(LL_CODEC_ERR,"codecErr.txt","@@@@the audio codec is not supported!\n");
				CUserOptions::UserOptions.errorID = E_UNSUPPORTED_CODEC;
				return false;
			}
			CUserOptions::UserOptions.outInfo.clip_audioChannels	= m_audioChannels;
			CUserOptions::UserOptions.outInfo.clip_audioSampleRate	= m_audioSampleRate;
		}
		else if(IsVideoStream())//
		{
			if(IsVideoMpeg4())
			{
				CUserOptions::UserOptions.outInfo.codecType[0]=kCodecTypeMPEG4SP;
			}
			else if(IsVideoH263())
			{
				CUserOptions::UserOptions.outInfo.codecType[0]=kCodecTypeH263BL;

			}
			else if(IsVideoH264())
			{
				CUserOptions::UserOptions.outInfo.codecType[0]=kCodecTypeH264BL;
			}
		}
#ifdef DIMS_DATA
		else if(IsDataStream())
		{
			if(_stricmp(m_codecName,"richmedia+xml")==0)
			{
				strcpy(m_mediaName,"Data");
			}
		}
#endif
		return true;
	}
	else
	{
		//CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse rtpmap\n");

	}
	return false;
}

bool CMediaStream::ParseSDPAttribute_range(char * sdpParam)
{
	if(sscanf(sdpParam, "a=range:npt=%g- %g", &m_mediaStreamStartTime, &m_mediaStreamEndTime) == 2)
	{
		return true;
	}
	else if(sscanf(sdpParam, "a=range:%g- %g", &m_mediaStreamStartTime, &m_mediaStreamEndTime) == 2)
	{
		return true;
	}
	else
	{
		//CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse media range\n");

	}
	return false;
}
bool CMediaStream::ParseSDPAttribute_stream(char * sdpParam)
{
	do 
	{
		if(_strnicmp(sdpParam, "a=stream:", 9) != 0)
		{
			//CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse a=control\n");
			break;
		}
		if(sscanf(sdpParam, "a=stream: %d",&m_streamNum)!=1)
		{
			CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse a=stream\n");
			break;
		}
	} while(0);
	return false;
}
bool CMediaStream::ParseSDPAttribute_control(char * sdpParam)
{
	do
	{
		if(_strnicmp(sdpParam, "a=control:", 10) != 0)
		{
			//CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse a=control\n");
			break;
		}

		m_controlAttr = StringDup(sdpParam+10);
		if(CUserOptions::UserOptions.m_URLType==UT_SDP_URL)
		{
			
			char * pRTSPLink = strstr(sdpParam, "rtsp://");
			if(pRTSPLink == NULL)
			{
				if(sscanf(sdpParam, "a=control: %s", m_controlAttr) != 1)
					break;
			}
			else//keep the control of 
			{
				//int pos = CUserOptions::UserOptions.sdpURLLength;
				//if(sscanf(pRTSPLink+pos, "/%s", m_controlAttr) != 1)
				//	break;
			}
		}
		else if(sscanf(sdpParam, "a=control: %s", m_controlAttr) != 1)
		{
			CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse a=control=2\n");
			CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt",m_controlAttr);
			break;
		}
		sprintf(CLog::formatString,"control:%s\n",m_controlAttr);
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt",CLog::formatString);
		return true;

	}while(0);
	//CLog::Log.MakeLog(LL_SDP_ERR,"flow.txt","@@@@fail to parse a=control=2\n");
	return false;
}

bool CMediaStream::ParseSDPAttribute_cliprect(char * sdpParam)
{
	unsigned int left = 0;
	unsigned int top = 0;
	if(sscanf(sdpParam, "a=cliprect:%u,%u,%u,%u", &left, &top, &m_videoHeight, &m_videoWidth) == 4)
	{
		return true;
	}
	else
	{
		//CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse cliprect\n");
	}
	return false;
}

bool CMediaStream::ParseSDPAttribute_framesize(char * sdpParam)
{
	unsigned int rtpPayload = 0;
	if(sscanf(sdpParam, "a=framesize:%u %u-%u", &rtpPayload, &m_videoWidth, &m_videoHeight) == 3)
	{
		
		return true;
	}
	else
	{
		//CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse framesize\n");
	}
	return false;
}

bool CMediaStream::ParseSDPAttribute_framerate(char * sdpParam)
{
	if(sscanf(sdpParam, "a=framerate:%f", &m_videoFPS) == 1)
	{
		//m_framerate = m_videoFPS;
		return true;
	}
	else
	{
		//CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse framerate\n");
	}
	return false;
}
#if 0
typedef struct {
	unsigned int size;
	unsigned int chunk_size;
} xbuffer_header_t;

#define XBUFFER_HEADER_SIZE sizeof (xbuffer_header_t)
void *xbuffer_ensure_size(void *buf, int size) {
	xbuffer_header_t *xbuf;
	int new_size;

	if (!buf) {
		return 0;
	}

	xbuf = ((xbuffer_header_t*)(((unsigned char*)buf)-XBUFFER_HEADER_SIZE));

	if (xbuf->size < size) {
		new_size = size + xbuf->chunk_size - (size % xbuf->chunk_size);
		xbuf->size = new_size;
		buf = ((unsigned char*)realloc(((unsigned char*)buf)-XBUFFER_HEADER_SIZE,
			new_size+XBUFFER_HEADER_SIZE)) + XBUFFER_HEADER_SIZE;
	}

	return buf;
}
static char *b64_decode(const char *in, char *out, int *size)
{
	char dtable[256];              /* Encode / decode table */
	int i,j,k;

	for (i = 0; i < 255; i++) {
		dtable[i] = 0x80;
	}
	for (i = 'A'; i <= 'Z'; i++) {
		dtable[i] = 0 + (i - 'A');
	}
	for (i = 'a'; i <= 'z'; i++) {
		dtable[i] = 26 + (i - 'a');
	}
	for (i = '0'; i <= '9'; i++) {
		dtable[i] = 52 + (i - '0');
	}
	dtable['+'] = 62;
	dtable['/'] = 63;
	dtable['='] = 0;

	k=0;

	/*CONSTANTCONDITION*/
	for (j=0; j<strlen(in); j+=4)
	{
		char a[4], b[4];

		for (i = 0; i < 4; i++) {
			int c = in[i+j];

			if (dtable[c] & 0x80) {
				printf("Illegal character '%c' in input.\n", c);
				//        exit(1);
				return NULL;
			}
			a[i] = (char) c;
			b[i] = (char) dtable[c];
		}
		out = (char*)xbuffer_ensure_size(out, k+3);
		out[k++] = (b[0] << 2) | (b[1] >> 4);
		out[k++] = (b[1] << 4) | (b[2] >> 2);
		out[k++] = (b[2] << 6) | b[3];
		i = a[2] == '=' ? 1 : (a[3] == '=' ? 2 : 3);
		if (i < 3) {
			out[k]=0;
			*size=k;
			return out;
		}
	}
	out[k]=0;
	*size=k;
	return out;
}
#endif

int CMediaStream::ParseAVCConfig(char* pData,char* dst)
{
	//refer 14496-15 5.2.4.1.1
	int i;
	int numOfPictureParameterSets;
	int configurationVersion	= pData[0];
	int AVCProfileIndication	= pData[1];
	int profile_compatibility	= pData[2];
	int AVCLevelIndication		= pData[3];
	int numOfSequenceParameterSets = pData[5]&0x1f;
	unsigned char * buffer = (unsigned char*)pData+6;
	int lengthSizeOfNALU			= (pData[4]&0x03)+1;
	m_lengthSizeOfNALU = lengthSizeOfNALU;
	char* oriDst=dst;
	//inPa->fileFormat=VOIH264FM_14496_15;
	for (i=0; i< numOfSequenceParameterSets; i++) {
		int sequenceParameterSetLength = (buffer[0]<<8)|buffer[1];
#ifdef H264_ANNEXB_FORMAT
		dst[0]=dst[1]=dst[2]=0;dst[3]=1;dst+=4;
		memcpy(dst,buffer+2,sequenceParameterSetLength);
		dst+=sequenceParameterSetLength;
#else//H264_ANNEXB_FORMAT
		memcpy(dst,buffer,sequenceParameterSetLength+2);
		dst+=sequenceParameterSetLength+2;
#endif//H264_ANNEXB_FORMAT
		
		buffer+=sequenceParameterSetLength+2;
	}
	numOfPictureParameterSets=*buffer++;
	for (i=0; i< numOfPictureParameterSets; i++)
	{
		int pictureParameterSetLength = (buffer[0]<<8)|buffer[1];
#ifdef H264_ANNEXB_FORMAT
		dst[0]=dst[1]=dst[2]=0;dst[3]=1;dst+=4;
		memcpy(dst,buffer+2,pictureParameterSetLength);
		dst+=pictureParameterSetLength;
#else//H264_ANNEXB_FORMAT
		memcpy(dst,buffer,pictureParameterSetLength+2);
		dst+=pictureParameterSetLength+2;
#endif//H264_ANNEXB_FORMAT
		buffer+=pictureParameterSetLength+2;
	}
	return dst-oriDst;
}

#if SUPPORT_ADAPTATION

unsigned int CMediaStream::GetFirstFrameSeqNumInBuffer()
{
	//if(m_mediaFrameSink)
	//{
	//	CMediaFrame* frame = m_mediaFrameSink->GetFirstFrameInBuffer();
	//	if(frame)
	//		return frame->seqNum;
	//}
	return 0;
}
bool	CMediaStream::ParseSDPAttribute_3gppAdapation(char* sdpParam)
{
	//if(_strnicmp(sdpParam, "a=3gpp-adaptation-support:", 26) != 0)
	//	return false;
	//if(sscanf(sdpParam+26,"%d",&m_reportFreq)==1)
	//{
	//	CUserOptions::UserOptions.m_n3GPPAdaptive=1;
	//	return true;
	//}
	return false;
}

#endif//SUPPORT_ADAPTATION



bool CMediaStream::ParseSDPAttribute_fmtp(char * sdpParam)
{
	if(strncmp(sdpParam, "a=fmtp:", 7) != 0)
		return false;

	sdpParam += 7;
	int length ;//= strlen(sdpParam);
	while(isdigit(*sdpParam)) ++sdpParam;
	int i=0;
	//for(char * c=sdpParam,i=0;i<length; ++c,i++)
	//	*c = tolower(*c);
	m_h264ConfigNotBase64=0;
	while(*sdpParam != '\0' && *sdpParam != '\r' && *sdpParam != '\n')
	{
		int foundEQ=0;
		length = strlen(sdpParam);
		for(char * c=sdpParam,i=0;i<length; ++c,i++)
		{
			if( *c=='=')
			{
				foundEQ = 1;
				break;
			}
			*c = tolower(*c);
		}
		if(!foundEQ)//There is no '=' in the string
		{
			break;
		}
		if(sscanf(sdpParam, " config = %[^; \t\r\n]", m_fmtpConfig) == 1)
		{
		}
		else if(sscanf(sdpParam, " sprop-parameter-sets = %[^; \t\r\n]", m_fmtpSpropParameterSets) == 1)
		{
		}
		else if(sscanf(sdpParam, " parameter-sets = %[^; \t\r\n]", m_fmtpSpropParameterSets) == 1)
		{
			m_h264ConfigNotBase64 = 1;
		}
		else if(sscanf(sdpParam, " mode = %[^; \t\r\n]", m_fmtpMode) == 1)
		{
		}
		else if(sscanf(sdpParam, " packetization-mode = %u", &m_fmtpPacketizationMode) == 1)
		{
		}
		else if(sscanf(sdpParam, " profile-level-id = %u", &m_fmtpProfileLevelID) == 1) 
		{
		}
		else if(sscanf(sdpParam, " sizelength = %u", &m_fmtpSizeLength) == 1)
		{
		}
		else if(sscanf(sdpParam, " indexlength = %u", &m_fmtpIndexLength) == 1)
		{
		}
		else if(sscanf(sdpParam, " indexdeltalength = %u", &m_fmtpIndexDeltaLength) == 1)
		{
		}
		else if(sscanf(sdpParam, " octet-align = %u", &m_fmtpOctetAlign) == 1)
		{
		}
		else if(sscanf(sdpParam, " interleaving = %u", &m_fmtpInterleaving) == 1)
		{
		}
		else if (sscanf(sdpParam, " robust-sorting = %u", &m_fmtpRobustSorting) == 1)
		{
		}
		else if (sscanf(sdpParam, " crc = %u", &m_fmtpCRC) == 1)
		{
		}
		else if (sscanf(sdpParam, " framesize=%u-%u", &m_videoWidth, &m_videoHeight) == 2)
		{
		
		}

		while(*sdpParam != '\0' && *sdpParam != '\r' && *sdpParam != '\n' && *sdpParam != ';')
		{
			++sdpParam;
		}

		if(*sdpParam == ';') 
		{
			++sdpParam; // Skip the ';'
		}
	}

	if(IsVideoStream())
	{
		int videoWidth = 0;
		int videoHeight = 0;
		if(IsVideoH264())
		{
			if(strlen(m_fmtpSpropParameterSets)>8)
			{
	#if 1
				if(!m_h264ConfigNotBase64)
					SpropParameterSetsData(m_fmtpSpropParameterSets, &m_codecHeadData, &m_codecHeadDataLen);
				else
				{
					//m_codecHeadDataLen = strlen(m_fmtpSpropParameterSets);
					//m_codecHeadData	   = new unsigned char[m_codecHeadDataLen];
					HexStrToBytes(m_fmtpSpropParameterSets, &m_codecHeadData, &m_codecHeadDataLen);	
					unsigned char* pData		   = m_codecHeadData;
					if(pData==NULL)
						return false;
					int numOfSequenceParameterSets = pData[0]&0x1f;
					unsigned char * buffer = (unsigned char*)pData+1;
					unsigned char* dst = (unsigned char*)m_fmtpSpropParameterSets;
					unsigned char* oridst = dst;
					int i;
					for (i=0; i< numOfSequenceParameterSets; i++) {
						int sequenceParameterSetLength = (buffer[0]<<8)|buffer[1];
#ifdef H264_ANNEXB_FORMAT
						dst[0]=dst[1]=dst[2]=0;dst[3]=1;dst+=4;
						memcpy(dst,buffer+2,sequenceParameterSetLength);
						dst+=sequenceParameterSetLength;
#else//H264_ANNEXB_FORMAT
						memcpy(dst,buffer,sequenceParameterSetLength+2);
						dst+=sequenceParameterSetLength+2;
#endif//H264_ANNEXB_FORMAT

						buffer+=sequenceParameterSetLength+2;
					}
					int numOfPictureParameterSets=*buffer++;
					for (i=0; i< numOfPictureParameterSets; i++)
					{
						int pictureParameterSetLength = (buffer[0]<<8)|buffer[1];
#ifdef H264_ANNEXB_FORMAT
						dst[0]=dst[1]=dst[2]=0;dst[3]=1;dst+=4;
						memcpy(dst,buffer+2,pictureParameterSetLength);
						dst+=pictureParameterSetLength;
#else//H264_ANNEXB_FORMAT
						memcpy(dst,buffer,pictureParameterSetLength+2);
						dst+=pictureParameterSetLength+2;
#endif//H264_ANNEXB_FORMAT
						buffer+=pictureParameterSetLength+2;
					}
					m_codecHeadDataLen = dst-oridst;
					memcpy(m_codecHeadData,oridst,m_codecHeadDataLen);
					m_codecHeadData[m_codecHeadDataLen-1]='\0';
					
				}
	#else
				m_codecHeadData = new unsigned char[MAX_OPAQUE_SIZE];TRACE_NEW("xxm_codecHeadData2 =",m_codecHeadData);
				if(m_codecHeadData==NULL)
				{
					return true;
				}
				m_codecHeadDataLen = MAX_OPAQUE_SIZE;
				base64_decode((unsigned char*)m_fmtpSpropParameterSets, m_codecHeadData, &m_codecHeadDataLen);
	#endif			
				if(GetSizeInfo(m_codecHeadData + 2, &videoWidth, &videoHeight) == 1)
				{
					m_videoWidth = videoWidth;
					m_videoHeight = videoHeight;
				}
				
				CUserOptions::UserOptions.outInfo.codecType[0]=kCodecTypeH264BL;
#ifdef H264_ANNEXB_FORMAT
				{
					int annexbHead=0x01000000;
					
					unsigned char* seqHead	= m_codecHeadData+2;
					int seqHeadSize	=(seqHead[-2]<<8)|(seqHead[-1]);
					unsigned char* picHead	= seqHead+seqHeadSize+2;
					int picHeadSize	=(picHead[-2]<<8)|(picHead[-1]);
					unsigned char* seqHead2	=(unsigned char*)m_fmtpSpropParameterSets;
					unsigned char* picHead2	=seqHead2+seqHeadSize;
					memcpy(seqHead2,seqHead,seqHeadSize);
					memcpy(picHead2,picHead,picHeadSize);
					SAFE_DELETE_ARRAY(m_codecHeadData);
					m_codecHeadDataLen=seqHeadSize+picHeadSize+8;
					m_codecHeadData = new unsigned char[m_codecHeadDataLen];TRACE_NEW("xxm_codecHeadData3 =",m_codecHeadData);
					seqHead =m_codecHeadData;
					picHead	=seqHead+seqHeadSize+4;
					memcpy(seqHead,&annexbHead,4);
					memcpy(seqHead+4,seqHead2,seqHeadSize);
					memcpy(picHead,&annexbHead,4);
					memcpy(picHead+4,picHead2,picHeadSize);

				}
#endif//H264_ANNEXB_FORMAT
#ifdef WIN32
				VO_FLOENGINE_FRAME frame;
				frame.pData = m_codecHeadData;
				frame.nSize = m_codecHeadDataLen;
				frame.nStartTime = 0;
				frame.nFrameType = VO_FLOENGINE_FRAMETYPE_VIDEO;
				CUserOptions::UserOptions.m_mfCallBackFuncSet.OnFrame(&frame);
#endif
			}
		}
		
	
	}
	else if(IsAudioStream())
	{
		if(_stricmp(m_codecName, "MPEG4-GENERIC") == 0)
		{
			HexStrToBytes(m_fmtpConfig, &m_codecHeadData, &m_codecHeadDataLen);
		}
		else if(_stricmp(m_codecName, "MP4A-LATM") == 0)
		{
			ParseMPEG4LatmAudioFmtpConfig(m_fmtpConfig, &m_codecHeadData, &m_codecHeadDataLen);	
		}
		sprintf(CLog::formatString,"The audio configdataLen=%d\n",m_codecHeadDataLen);
		CLog::Log.MakeLog(LL_CODEC_ERR,"rtsp.txt",CLog::formatString);
		//TheRTSPLogger.Dump("The audio configdataLen=%d" TXTLN, m_codecHeadDataLen);

	}

	return true;
}

int CMediaStream::GetCodecHeadData(unsigned char * codecHead)
{
	if(codecHead != NULL)
	{
		memcpy(codecHead, m_codecHeadData, m_codecHeadDataLen);
	}
	return m_codecHeadDataLen;
}

bool CMediaStream::ParseRTPInfoHeader(char * rtpInfoText)
{
	do
	{
		if(strstr(rtpInfoText, m_controlAttr) == NULL)//check the trackID is in the line
		{
#if TRACE_FLOW1
				sprintf(CLog::formatString,"@@@Target track is not the track(%s)\n",m_controlAttr);
				CLog::Log.MakeLog(LL_RTSP_ERR,"rtp-info.txt",CLog::formatString);
#endif
			break;
		}
#ifdef R_TEST
		//strcpy(rtpInfoText,"url=rtsp://172.31.50.7/alu_user/CTI_0_hd.3gp/trackID=100?CONF_ID=1225446045458542&UID=24CE1D014684B42BF7D3A45298060456.MCDP-mifeas01_data;seq=0;rtptime=1013749888");
#endif
		char * field = StringDup(rtpInfoText);
		unsigned int   seqNum=0;
		unsigned int   rtpTimestamp=0;
		//use local bool value to decide whether the field exist
		bool seqNumNotExist = true;
		bool rtpTimestampNotExist = true;
		while(sscanf(rtpInfoText, "%[^;]", field) == 1)
		{
			if(sscanf(field, "seq=%hu", &(seqNum)) == 1)
			{
				seqNumNotExist =m_rtpInfo.seqNumNotExist = false;
				m_rtpInfo.seqNum = seqNum;
				//sprintf(CLog::formatString,"the seqNum is=%d\n",seqNum);
				//CLog::Log.MakeLog(LL_RTSP_ERR,"rtp-info.txt",CLog::formatString);
			}
			else
			{
				//m_rtpInfo.seqNumNotExist = true;
			}

			if(sscanf(field, "rtptime=%u", &(rtpTimestamp)) == 1)
			{
				rtpTimestampNotExist = m_rtpInfo.rtpTimestampNotExist = false;
				m_rtpInfo.rtpTimestamp = rtpTimestamp;
				//m_streamSyncTimestamp = m_rtpInfo.rtpTimestamp;
				//m_streamTimestamp = m_rtpInfo.rtpTimestamp;
				//also adjust the SyncClock
				//sprintf(CLog::formatString,"the rtptime is=%u\n",m_streamTimestamp);
				//CLog::Log.MakeLog(LL_RTSP_ERR,"rtp-info.txt",CLog::formatString);
			}
			else
			{
				//m_rtpInfo.rtpTimestampNotExist = true;
				//calcualte the approximate rtpTimestamp
				//m_streamTimestamp = m_streamSyncTimestamp = m_streamStartTime*RTPTimestampFrequency()/1000+m_firstTimeStamp;
				//CLog::Log.MakeLog(LL_RTSP_ERR,"rtp-info.txt","the rtpTimestamp does not exist\n");
			}

			rtpInfoText += strlen(field);
			if(rtpInfoText[0] == '\0') 
				break;

			++rtpInfoText; // skip over the ';'
		}
		if(rtpTimestampNotExist)
		{
			m_rtpInfo.rtpTimestampNotExist = rtpTimestampNotExist;
			CLog::Log.MakeLog(LL_RTSP_ERR,"rtp-info.txt","the rtpTimestamp does not exist\n");
		}
		if(seqNumNotExist)
		{
			m_rtpInfo.seqNumNotExist = seqNumNotExist;
			CLog::Log.MakeLog(LL_RTSP_ERR,"rtp-info.txt","the seqNum does not exist\n");
		}

#if 0
		m_streamSyncTimestamp = 0;//m_rtpInfo.rtpTimestamp;
		m_streamTimestamp = 0;//m_rtpInfo.rtpTimestamp;
#else
		//if(m_rtpInfo.rtpTimestamp>0)
		{
			
		}
		
#endif

		SAFE_DELETE_ARRAY(field);

		return true;

	}while(0);

	return false;
}

bool CMediaStream::IsVideoStream()
{
	return _stricmp(m_mediaName, "Video") == 0;
}
bool CMediaStream::IsApplicationStream()
{
	return _stricmp(m_mediaName, "application") == 0;
}
bool CMediaStream::IsAudioStream() 
{
	return _stricmp(m_mediaName, "Audio") == 0;
}

//const int sampRateTab[12] = {
//	96000, 88200, 64000, 48000, 44100, 32000, 
//	24000, 22050, 16000, 12000, 11025,  8000
//};
void WriteADTSHead(unsigned char* buf,int sampleRate,int chs,int size)
{
	int Idx;
	for(Idx=0;Idx<12;Idx++)
	{
		if(sampRateTab[Idx]==sampleRate)
			break;
	}
	CBVUtility bitVec(buf, 0, 7*8);
	bitVec.PutBits( 0xFFFF, 12); /* 12 bit Syncword */
	bitVec.PutBits( 1, 1); /* ID == 0 for MPEG4 AAC, 1 for MPEG2 AAC */
	bitVec.PutBits( 0, 2); /* layer == 0 */
	bitVec.PutBits( 1, 1); /* protection absent */
	bitVec.PutBits( 1, 2); /* profile */
	bitVec.PutBits( Idx, 4); /* sampling rate */
	bitVec.PutBits( 0, 1); /* private bit */
	bitVec.PutBits( chs, 3); /* ch. config (must be > 0) */
	/* simply using numChannels only works for
	6 channels or less, else a channel
	configuration should be written */
	bitVec.PutBits( 0, 1); /* original/copy */
	bitVec.PutBits( 0, 1); /* home */


	/* Variable ADTS header */
	bitVec.PutBits( 0, 1); /* copyr. id. bit */
	bitVec.PutBits( 0, 1); /* copyr. id. start */
	bitVec.PutBits( size, 13);
	bitVec.PutBits( 0x7FF, 11); /* buffer fullness (0x7FF for VBR) */
	bitVec.PutBits( 0, 2); /* raw data blocks (0+1=1) */
}
void  CMediaStream::SetDelayTime(int time)
{
	//time=((time+500)/1000)*1000;

//	if(m_rtspTrack)
//		m_rtspTrack->SetDelayTime(time);
	m_delayTime = time;
	m_frameNum = 0;
}
int CMediaStream::removeFrameBefore(long upTime)
{
	if(upTime<0)
		upTime=0;
	return  0;//m_mediaFrameSink->removeFrameBefore(upTime);
}


void CMediaStream::AdjustSyncClockWithRTPInfo()
{
	
}
//TODO:Use AdjustSyncClock approximately
void CMediaStream::AdjustSyncClockWhenRTCP()
{
	if(m_dataSyncbySeqNum)
		return;
	
	if(m_previousRTCP.timeStamp == 0)//firstTime
	{
		m_previousRTCP.timeStamp = m_streamSyncTimestamp;
		m_previousRTCP.sec		 = m_streamSyncWallClockTime->tv_sec;
		m_previousRTCP.uSec		 = m_streamSyncWallClockTime->tv_usec;
	}
	else //if(m_adjustTimeBetweenPauseAndPlay>0)//check whether the time has jumped by pause/play
	{
#if 0
		m_streamSyncTimestamp = m_previousRTCP.timeStamp;
		m_streamSyncWallClockTime->tv_sec = m_previousRTCP.sec;
		m_streamSyncWallClockTime->tv_usec= m_previousRTCP.uSec;
#else
		int timeStampDiff = m_streamSyncTimestamp - m_previousRTCP.timeStamp;
		double timeDiff	  = (timeStampDiff/(double)RTPTimestampFrequency());
		int	   expectTimediff = int(timeDiff*million);
		int secDiff	  = (m_streamSyncWallClockTime->tv_sec-m_previousRTCP.sec)*million;
		int usecDiff  = (m_streamSyncWallClockTime->tv_usec-m_previousRTCP.uSec);
		int adjustTime = secDiff+usecDiff-expectTimediff;
		int absAdjustTime = abs(adjustTime);
		if(absAdjustTime>million/4)//if the difference is great than 500ms,then adjust it
		{
			int adjustSec  = absAdjustTime/million;
			int adjustUsec = absAdjustTime-adjustSec*million;
			if(adjustTime>0)
			{
				m_streamSyncWallClockTime->tv_sec-=adjustSec;
				m_streamSyncWallClockTime->tv_usec-=adjustUsec;
				if(m_streamSyncWallClockTime->tv_usec<0)
				{
					m_streamSyncWallClockTime->tv_sec--;
					m_streamSyncWallClockTime->tv_usec+=million;
				}
			}
			else
			{
				m_streamSyncWallClockTime->tv_sec+=adjustSec;
				m_streamSyncWallClockTime->tv_usec+=adjustUsec;
				if(m_streamSyncWallClockTime->tv_usec>million)
				{
					m_streamSyncWallClockTime->tv_sec++;
					m_streamSyncWallClockTime->tv_usec-=million;
				}
			}
			
			if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
			{
				sprintf(CLog::formatString,"*******adjustTime=%d,tv_sec=%d,tv_usec=%d\n",adjustTime/1000,m_streamSyncWallClockTime->tv_sec,m_streamSyncWallClockTime->tv_usec);
				if(IsVideoStream())
					CLog::Log.MakeLog(LL_RTP_ERR,"videoDataStatus.txt",CLog::formatString);
				else
					CLog::Log.MakeLog(LL_RTP_ERR,"audioDataStatus.txt",CLog::formatString);
			}
		}
#endif
	}
}

void CMediaStream::AdjustSyncClock(long adjustTime,float begin)
{
	if(adjustTime<0)
		return;
	m_adjustTimeBetweenPauseAndPlay += adjustTime;

#if 1
	//if(m_rtspTrack==NULL||m_dataSyncbySeqNum||m_rtpInfo.rtpTimestampNotExist)
	//	return;
	float startPos   = 0;//m_rtspTrack->GetStartTime()/1000.0;
	float actualTime = begin;
	begin=actualTime-startPos;//-m_streamTimeBeforeSync*1.0/million;
	if(begin>5.0)//this is only to handle the continous pause
	{
		int second   = int(begin);
		int uSecond  = (begin-second*1.0)*million-m_streamTimeBeforeSync/10;
		if(uSecond<0)
		{
			uSecond+=million;
			second-=1;
		}
		
		m_streamSyncWallClockTime->tv_sec=m_baseSyncWallClock->tv_sec+second;
		m_streamSyncWallClockTime->tv_usec=m_baseSyncWallClock->tv_usec+uSecond;
	
		if (m_streamSyncWallClockTime->tv_usec>million)
		{
			m_streamSyncWallClockTime->tv_usec-=million;
			m_streamSyncWallClockTime->tv_sec+=1;
		}
		if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
		{
			sprintf(CLog::formatString,"AdjustSyncClock:begin=%f(%f-%f),SYN_CLOCK(%d,%d),ts=%u\n",begin,actualTime,startPos,m_streamSyncWallClockTime->tv_sec,m_streamSyncWallClockTime->tv_usec,m_rtpInfo.rtpTimestamp);
			//CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt",CLog::formatString);
			if(IsVideoStream())
			{
				CLog::Log.MakeLog(LL_RTSP_ERR,"videoDataStatus.txt",CLog::formatString);
			}
			else
			{
				CLog::Log.MakeLog(LL_RTSP_ERR,"audioDataStatus.txt",CLog::formatString);
			}
		}
		
		//mediaStream->m_baseSyncWallClock->tv_usec = mediaStream->m_streamSyncWallClockTime->tv_usec;

	}
#endif
	//if(m_adjustTimeBetweenPauseAndPlay==0)
	//	return;

}



void CMediaStream::ResetTmpBuf()
{
	if(IsVideoH264()&&m_gTmpBuf)
	{
		m_gTmpBuf->isIntra			=0;
		m_gTmpBuf->IsNotFirstNalu	= 0;
		m_gTmpBuf->seqNum			= 0;
		m_gTmpBuf->time				= 0;
		m_gTmpBuf->size				= 0;
		m_gTmpBuf->flag				= 0;
		m_gTmpBuf->fragment			= 0;
	}
	m_previousFrameTime=-1;
	CLog::Log.MakeLog(LL_RTSP_ERR,"rtsp.txt","ResetTmpBuf()\n");
}


unsigned long CMediaStream::CalculateNPT(unsigned long rtpTimestamp,int seqNum)
{
	long frameStartTime = 0;

	unsigned long startTime=0; 
	long tmpTime ;
	CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
	long oriTime; 
	//after set position and there is no RTP-Info and RTCP-sync
	//if(CUserOptions::UserOptions.streamType==ST_ASF)
	//{
	//	pRTSPMediaStreamSyncEngine->CalcualteMediaFramePresentationTimeASF(this,rtpTimestamp,seqNum,&oriTime);
	//}
	/*else*/ if((IsNeedSyncByFirstFrame()&&m_streamStartTime>0))//||(CUserOptions::UserOptions.streamType==ST_ASF))
	{
//		pRTSPMediaStreamSyncEngine->CalculateMediaFramePresentationTimeBasedOnFirstFrame(this, rtpTimestamp, &oriTime);

	}
	else
	{
		int timeStamp=pRTSPMediaStreamSyncEngine->CalculateMediaFramePresentationTime(this, rtpTimestamp, &frameStartTime,seqNum);
		if(timeStamp==INVALID_TIMESTAMP)
			return INVALID_TIMESTAMP;
		oriTime = frameStartTime;

	}
	tmpTime = oriTime;
	if(tmpTime<0)
		tmpTime = 0;
	startTime = tmpTime;
	if(IsAudioAMRNB()||IsAudioAMRWB()||IsAudioAAC())//&&CUserOptions::UserOptions.m_nEnableNullAudFrameWhenPacketloss))
	{
		//for dshow,it is necessary to adjust the TS to make the sound smooth
		unsigned long lastTime = 0; //m_mediaFrameSink->GetLastTimeInBuffer();

#if SUPPORT_NULL_PACK
		//check if it is aac+
		if(m_scaleAACPlus)
		{
			if(startTime<CHECK_NULL_TIME-500)
			{
				if(m_scaleAACPlus==1)
				{
					
					if(startTime>1000&&m_packetLoss==false)
					{
						int actualPackets=m_mediaFrameSink->GetReceivedFrames();
						int expectPackets=lastTime/m_timePerFrame;
						int frameRate=1000/m_timePerFrame;
						int adjust=frameRate>20?frameRate/4:frameRate/2;
						if(adjust<5)
							adjust=5;
						if((actualPackets+adjust)<expectPackets)
						{
							m_scaleAACPlus = 2;
							m_timePerFrame*=2;
							sprintf(CLog::formatString,"The audio is AAC+,expect=%d,actual=%d,framerate=%d\n",expectPackets,actualPackets,frameRate);
							CLog::Log.MakeLog(LL_RTSP_ERR,"rtsp.txt",CLog::formatString);
						}
						
					}
				}
			}
			else

			{
				if(startTime+10 <lastTime+m_timePerFrame&&startTime>0)
					startTime = lastTime + m_timePerFrame;
			}
			
		}
		else//AMR
#endif//SUPPORT_NULL_PACK
		{
			if(startTime+10 <lastTime+m_timePerFrame&&startTime>0)
				startTime = lastTime + m_timePerFrame;
		}
	
		
	}
	return startTime;
}



void CMediaStream::RecvMediaFrame(unsigned char * frameData, int frameSize, unsigned int rtpTimestamp,unsigned int seqNum,bool IsKeyFrame)
{
	
	unsigned long startTime; 
	unsigned long endTime;
	char* format = CLog::formatString;
	long tmpTime ;


	if(m_firstTimeStamp==-1)
		m_firstTimeStamp = rtpTimestamp;

	CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
	long oriTime; 
	long flag=0;

	startTime = CalculateNPT(rtpTimestamp,seqNum);
	if(startTime==INVALID_TIMESTAMP)
	{
		return ;
	}
	if(startTime<0)
		startTime = 0;


	m_nCurPlayTime = startTime;
	
	bool isIntraFrame = false;
	bool isVideo;
	bool isTheSamePic=false;
	int totalSize=frameSize;
	bool isGoOnNextAVCNalu=false;

	do 
	{
		if(IsVideoStream())
		{
			if(IsVideoH264())
			{
				int actualSize=frameSize;
				unsigned char* buffer=frameData;
#define GET_NALUSIZE4(b) ((b[0]<<24)|(b[1]<<16)|(b[2]<<8)|(b[3]))
#define GET_NALUSIZE2(b) ((b[0]<<8)|(b[1]))
				if(!IsValidH264NALU(frameData,frameSize))
				{
					m_errPackNum++;
					NotifyErrPack();
					return;
				}
				if(m_errPackNum>30)
				{
					SLOG3(LL_SOCKET_ERR,"videoRTP.txt","%d/%d,too many invalid NALU,block it_%d\n",m_errPackNum,m_frameNum,seqNum);
					return;
				}
				isIntraFrame = IsH264IntraFrame(frameData, frameSize) ==1;
				//int naluType = frameData[0]&0x0f;
				
				isTheSamePic = (rtpTimestamp==(unsigned int)m_previousFrameTime);
				
				frameData-=4;
				frameSize+=4;
				frameData[0]=frameData[1]=frameData[2]=0;frameData[3]=1;
				if(isTheSamePic&&m_previousFrameIsIntra)
					isIntraFrame = false;

			}
			
			
			isVideo = true;

		}
		else
		{
			isVideo = false;

		}
		

		do 
		{
			//if(CUserOptions::UserOptions.m_bMakeLog<LOG_DATA||m_frameNum<60)//disbale the data receiving/decoding if it is dumping data,but enbale the first 20 frames out(DShow issue)
			//if(pRTSPMediaStreamSyncEngine->RejectFrameBeforeIFrame(this,startTime,isIntraFrame,isVideo)==false)
			{
				
				m_frameNum++;

				if(IsVideoH264())
				{
					if(m_gTmpBuf==NULL)
					{
						m_gTmpBuf = new GTmpBuf();TRACE_NEW("xxm_m_gTmpBuf =",m_gTmpBuf);
						ResetTmpBuf();
						m_previousFrameTime=-1;
					}
					if((unsigned long)(frameSize+m_gTmpBuf->size)>MAX_TMP_H264BUF)
					{
						sprintf(CLog::formatString,"frameSize(%d)||m_gTmpBuf->size(%d)>MAX_TMP_H264BUF\n",frameSize,m_gTmpBuf->size);
						CLog::Log.MakeLog(LL_SOCKET_ERR,"h264.txt",CLog::formatString);
						VOLOGE("%s",CLog::formatString);
						ResetTmpBuf();
						
						return;
					}
					if(isTheSamePic||m_previousFrameTime==-1)
					{
						
						if(m_previousFrameTime==-1)//first frame
						{
							m_gTmpBuf->isIntra = isIntraFrame;
							m_gTmpBuf->time	   = startTime;
							m_gTmpBuf->seqNum  = seqNum;
							m_gTmpBuf->flag	   = flag;
							m_gTmpBuf->size	   = 0;
						}
						if(m_gTmpBuf->isIntra==0)//it is possible the frame=I slice+SEI
							m_gTmpBuf->isIntra= isIntraFrame;
						memcpy(m_gTmpBuf->buffer+m_gTmpBuf->size,frameData,frameSize);
						m_gTmpBuf->size+=frameSize;
						m_gTmpBuf->fragment++;
						
					}
					else
					{
						if(m_gTmpBuf->size>0)
						{
							//it is possible that the frame in tmpbuffer is still the uncalculated
						
							//m_mediaFrameSink->SinkMediaFrame(m_gTmpBuf->buffer, m_gTmpBuf->size, m_gTmpBuf->time, m_gTmpBuf->seqNum, m_gTmpBuf->isIntra,m_gTmpBuf->flag);

							if(CUserOptions::UserOptions.m_mfCallBackFuncSet.OnFrame)
							{
								VO_FLOENGINE_FRAME frame;
								frame.pData = m_gTmpBuf->buffer;
								frame.nSize = m_gTmpBuf->size;
								frame.nStartTime = m_gTmpBuf->time;
								frame.nFrameType = VO_FLOENGINE_FRAMETYPE_VIDEO;
								CUserOptions::UserOptions.m_mfCallBackFuncSet.OnFrame(&frame);

						
							}






							//CLog::EnablePrintTime(false);
							//CLog::Log.MakeLog("VideoData.dat", m_gTmpBuf->buffer, m_gTmpBuf->size);
							m_gTmpBuf->size=0;
							m_gTmpBuf->fragment=0;
							memcpy(m_gTmpBuf->buffer,frameData,frameSize);
							m_gTmpBuf->size+=frameSize;
							m_gTmpBuf->isIntra = isIntraFrame;
							m_gTmpBuf->time	= startTime;
							m_gTmpBuf->seqNum  = seqNum;
							m_gTmpBuf->flag	   = flag;
						}
						
					}
				}
				else

				{

					unsigned char adtsdata[1024];
					unsigned int adtsdatalen = 1024;


					Config2ADTSHeader((unsigned char *)m_codecHeadData, m_codecHeadDataLen, frameSize, adtsdata, &adtsdatalen);
					memcpy(adtsdata + adtsdatalen, frameData, frameSize);


					VO_FLOENGINE_FRAME frame;
					frame.pData = adtsdata; //frameData;
					frame.nSize = adtsdatalen + frameSize; //frameSize;
					frame.nStartTime = startTime;
					frame.nFrameType = VO_FLOENGINE_FRAMETYPE_AUDIO;


#if _DUMP_AVTIMESTAMP
								if(frame.nFrameType == VO_FLOENGINE_FRAMETYPE_AUDIO)
								{
									if(m_hAudioTS == NULL)
									{
										const char * logDir = GetLogDir();
										char filePath[256];
										strcpy(filePath, logDir);
										strcat(filePath, "Audio_TS.txt");
										m_hAudioTS = fopen(filePath, "wb");	

										char szAudioTSHead[256];
										sprintf(szAudioTSHead, "SysT,         TS,      FrameSize\n");
										fwrite(szAudioTSHead, 1, strlen(szAudioTSHead), m_hAudioTS);
									}

									char szAudioTSInfo[256];
									sprintf(szAudioTSInfo, "%u,     %llu,      %u\n", voOS_GetSysTime(), frame.nStartTime, frame.nSize);
									fwrite(szAudioTSInfo, 1, strlen(szAudioTSInfo), m_hAudioTS);
								}
								else if(frame.nFrameType == VO_FLOENGINE_FRAMETYPE_VIDEO)
								{
									if(m_hVideoTS == NULL)
									{
										const char * logDir = GetLogDir();
										char filePath[256];
										strcpy(filePath, logDir);
										strcat(filePath, "Video_TS.txt");
										m_hVideoTS = fopen(filePath, "wb");	

										char szVideoTSHead[128];
										sprintf(szVideoTSHead, "SysT,         TS,      FrameSize\n");
										fwrite(szVideoTSHead, 1, strlen(szVideoTSHead), m_hVideoTS);
									}

									char szVideoTSInfo[256];
									sprintf(szVideoTSInfo, "%u,     %llu,      %u\n", voOS_GetSysTime(), frame.nStartTime, frame.nSize);
									fwrite(szVideoTSInfo, 1, strlen(szVideoTSInfo), m_hVideoTS);
								}

#endif

					CUserOptions::UserOptions.m_mfCallBackFuncSet.OnFrame(&frame);
				}


				m_previousFrameTime			= rtpTimestamp;//m_nCurPlayTime;
				m_previousFrameIsIntra		= isIntraFrame;
			}

		} while(0);
	
	
	} while(0);
	


}

unsigned char*	CMediaStream::GetCodecHeadBuffer(int size)
{
	SAFE_DELETE_ARRAY(m_codecHeadData);
	m_codecHeadData = new unsigned char[size];TRACE_NEW("xxm_m_codecHeadData =",m_codecHeadData);
	m_codecHeadDataLen = size;
	return m_codecHeadData;
}

bool	CMediaStream::IsNeedSyncByFirstFrame()
{
	CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();

	return !pRTSPMediaStreamSyncEngine->IsSyncEngineSyncbyRTCP()&&IsRTPInfoTimeStampNotExist();
}
void CMediaStream::CalculateNPTAfterPlayResponse()
{
	if(m_recalFrameNum)
	{
//		m_mediaFrameSink->CalculateNPTAfterPlayResponse();
		m_recalFrameNum = 0;
	}
}
void CMediaStream::ResetAllFramesAfterPos(int oldstart,int newstart)
{
//	m_mediaFrameSink->ResetAllFramesAfterPos(oldstart, newstart);
}

void CMediaStream::FlushMediaStream(int flushFlag)
{
	if(m_rtpParser)
		m_rtpParser->ReleaseReorderedRTPPackets();
	

	m_delayTime = 0;
	m_frameNum = 0;

	CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
	pRTSPMediaStreamSyncEngine->RestartMediaStream(this);


	if(IsVideoH264() && (flushFlag & FLUSH_H264TEMPBUFFER))
	{
		m_previousFrameTime = -1;
		ResetTmpBuf();
	}
}


void CMediaStream::ScheduleReceiverReportTask()
{
	CRTCPParser::OnExpire(m_rtcpParser);
}

void CMediaStream::UnscheduleReceiverReportTask()
{
	m_rtcpParser->Unschedule();
}
bool CMediaStream::	IsNeedRTCPSync()
{
	//return m_rtspSession->IsNeedRTCPSync();
	return true;
}

void CMediaStream::HandleH264BitError()
{
	CRTPSenderTable * rtpSenderTable = m_rtpParser->RTPSenderTable();
	if(rtpSenderTable)
	{
		CRTPSenderStatus * senderStatus = rtpSenderTable->GetSenderStatus();
		if(senderStatus)
			senderStatus->UpdatePacketErrorNum(1);
	}
	//NotifyErrPack();
}

void CMediaStream::FastChannelChange()
{
	m_rtpParser->FastChannelChange();
	m_errPackNum = 0;
	
}
