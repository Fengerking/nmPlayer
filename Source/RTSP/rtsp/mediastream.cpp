#include "voLog.h"
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include "ctype.h"
#include "utility.h"
#include "BitVector.h"
#include "network.h"
#include "getmp4vol.h"
#include "ParseSPS.h"
#include "RealTimeStreamingEngine.h"
#include "RTSPMediaStreamSyncEngine.h"
#include "RTSPSession.h"
#include "MediaFrameSink.h"
#include "MediaStreamSocket.h"

#include "RTPParser.h"
#include "RTCPParser.h"
#include "MPEG4GenericAudioRTPParser.h"
#include "MPEG4LATMAudioRTPParser.h"
#include "H264VideoRTPParser.h"
#include "MPEG4ESVideoRTPParser.h"
#include "H263VideoRTPParser.h"
#include "AMRAudioRTPParser.h"
#include "QcelpAudioRTPParser.h"
#include "AMRWBPAudioRTPParser.h"
#include "ASFRTPParser.h"
#include "DIMSRTPParser.h"

#include "MediaStream.h"
#include "RTSPTrack.h"
#include "G711AudioRTPParser.h"
#include "voADPCM.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

unsigned char CMediaStream::m_streamChannelIDGenerator = 0;
#define DEFAULT_FRAMERATE 15

void WriteADTSHead(int sampFreq, int chanNum,int framelen,unsigned char *adtsbuf);

CMediaStream::CMediaStream(CRTSPSession * rtspSession)
: m_isVideo(true)
, m_frameNum(0)
, m_gTmpBuf(NULL)
, m_h264ConfigNotBase64(0)
, m_controlAttr(NULL)
, m_mediaStreamStartTime(0)
, m_mediaStreamEndTime(0)
, m_rtpPayloadType(0)
, m_rtpTimestampFrequency(0)
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
, m_videoFPS(0)
, m_videoWidth(176)
, m_videoHeight(144)
, m_audioChannels(0)
, m_audioSampleRate(41000)
, m_audioBits(16)
, m_framerate(DEFAULT_FRAMERATE)
, m_mediaAddr(NULL)
, m_clientRTPPort(0)
, m_clientRTCPPort(0)
, m_serverRTPPort(0)
, m_serverRTCPPort(0)
, m_mediaStreamBandwidthAS(0)
, m_rtspSession(rtspSession)
, m_rtpParser(NULL)
, m_rtcpParser(NULL)
, m_mediaFrameSink(NULL)
, m_hasBeenSynchronized(false)
, m_baseSyncWallClock(NULL)
, m_streamSyncWallClockTime(NULL)
, m_streamSyncTimestamp(-1)
, m_streamSyncNTPAfterSync(-1)
, m_nPausedTime(0)
, m_nCurPlayTime(0)
, m_streamWallClockTime(NULL)
, m_streamTimestamp(-1)
, m_firstTimeStamp(-1)
, m_streamStartTime(0)
, m_previousPos(-1)
, m_dataSyncbySeqNum(true)
, m_streamRestartTime(0)
, m_streamCurTime(0)
, m_streamTimeBeforeSync(0)
,m_adjustTimeBetweenPauseAndPlay(0)
,m_rtpStreamSocket(NULL)
,m_rtcpStreamSocket(NULL)
,m_rtspTrack(NULL)
,m_previousFrameTime(-1)
,m_previousFrameIsIntra(0)
,m_lengthSizeOfNALU(0)
#if SUPPORT_ADAPTATION
,m_reportFreq(0)
#endif//
,m_streamNum(-1)
,m_timePerFrame(0)
,m_packetLoss(false)
,m_scaleAACPlus(0)

{
	memset(m_mediaName, 0, 16);
	memset(m_codecName, 0, 32);
	memset(m_protocolName, 0, 16);
	memset(m_fmtpConfig, 0, 256);
	memset(m_fmtpSpropParameterSets, 0, 256);
	memset(m_fmtpMode, 0, 32);
	memset(&m_WaveFormatEx, 0, sizeof(m_WaveFormatEx));
	ResetRTPInfo();
	ResetPreviousRTCPData();
	
	 m_previousTimeStamp = 0;
	 m_frameCount = 0;
	 m_totalTime = 0;
	 m_done = false;
	 m_isNewStart = true;
	 m_delayTime = 0;
	 m_recalFrameNum = 0;
}

CMediaStream::~CMediaStream(void)
{
	SAFE_DELETE_ARRAY(m_controlAttr);
	SAFE_DELETE(m_mediaAddr);
	SAFE_DELETE_ARRAY(m_codecHeadData);
	SAFE_DELETE(m_rtpParser);
	SAFE_DELETE(m_rtcpParser);
	SAFE_DELETE(m_mediaFrameSink);
	SAFE_DELETE(m_streamSyncWallClockTime);
	SAFE_DELETE(m_baseSyncWallClock);
	SAFE_DELETE(m_streamWallClockTime);
	SAFE_DELETE(m_gTmpBuf);
	
}
void CMediaStream::retrieveAllPacketsFromRTPBuffer()
{
	//CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","retrieveAllPacketsFromRTPBuffer()\n");
	if(m_rtpParser)
		return m_rtpParser->RetrieveAllPack();
}
void CMediaStream::UpdateSocket()
{
	if(m_rtpParser&&m_rtpStreamSocket)
		m_rtpParser->UpdateSocket(m_rtpStreamSocket);
	else
		CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt", "m_rtpParser&&m_rtpStreamSocket fail\n");

	if(m_rtcpParser&&m_rtcpStreamSocket)
		m_rtcpParser->UpdateSocket(m_rtcpStreamSocket);
	else
		CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt", "m_rtcpParser&&m_rtcpStreamSocket fail\n");
}
bool CMediaStream::SetTransportProtocol(bool streamingOverTCP)
{
	if(streamingOverTCP)
		return false;
	CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
	do
	{
		CMediaStreamSocket *rtpStreamSock=NULL;
		CMediaStreamSocket *rtcpStreamSock=NULL;

		int minPort;
		int maxPort;
    	int max_getport = 128;//maxPort-minPort;
		if (CUserOptions::UserOptions.m_videoConnectionPort||CUserOptions::UserOptions.m_audioConnectionPort)
		{
			if (IsAudioStream())
			{
				minPort = CUserOptions::UserOptions.m_audioConnectionPort;
			}
			else
			{
				minPort = CUserOptions::UserOptions.m_videoConnectionPort;
			}

			if(minPort&1)
				minPort+=1;
		}
		else
		{
			minPort = CUserOptions::UserOptions.m_nUDPPortFrom;
			maxPort = CUserOptions::UserOptions.m_nUDPPortTo;
			int halfRange=(maxPort-minPort)/2;
			minPort += halfRange;//4*CUserOptions::UserOptions.portScale);

			int random=pRTSPMediaStreamSyncEngine->GetRandomPort();
			if (random<0)
			{
				random=-random;
			}
			if (random>halfRange/2)
			{
				random&=0xff;
			}
			minPort+=random;
			if(minPort>maxPort)
				minPort =maxPort-4;
			
			if(minPort&1)
				minPort+=1;

			if(IsAudioStream())
				minPort+=2;//(max_getport/4)&0xfffffffe;

		}
		
		m_clientRTPPort=minPort;
		while(max_getport-->0)//it is safe to limit the count of fetching port
		{
			rtpStreamSock = new CMediaStreamSocket(m_mediaAddr->ss_family, 0, m_clientRTPPort);TRACE_NEW("CMediaStream::Init rtpStreamSock4",rtpStreamSock);
			//m_clientRTPPort = rtpStreamSock->GetMediaStreamSockLocalPort();

			bool portFlag;
			if (CUserOptions::UserOptions.m_videoConnectionPort||CUserOptions::UserOptions.m_audioConnectionPort)
			{
				portFlag = rtpStreamSock->GetMediaStreamSock()&&((m_clientRTPPort & 1) == 0);
			}
			else
				portFlag = rtpStreamSock->GetMediaStreamSock()&&(m_clientRTPPort & 1) == 0&&(m_clientRTPPort>=minPort&&m_clientRTPPort<=maxPort);
			if(portFlag)
			{
				rtcpStreamSock = new CMediaStreamSocket(m_mediaAddr->ss_family, 0, m_clientRTPPort + 1);TRACE_NEW("CMediaStream::Init rtcpStreamSock4",rtcpStreamSock);

				sprintf(CLog::formatString,"$$$CMediaStream::create one RTP(%lu)/RTCP(%lu) socket\n",rtpStreamSock->GetMediaStreamSock()->GetSockID(),rtcpStreamSock->GetMediaStreamSock()->GetSockID());
				CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", CLog::formatString);

				if(rtcpStreamSock->GetMediaStreamSock())
				{
					m_clientRTCPPort = rtcpStreamSock->GetMediaStreamSockLocalPort();
					break;
				}
				else
				{
					SAFE_DELETE(rtcpStreamSock);
					rtcpStreamSock = NULL;
				}
			}
			//else//error!!
			{
				char* format=CLog::formatString;
				sprintf(format,"%d:socket warning:invalid RTP port=%d valid(%d,%d)\n",max_getport,m_clientRTPPort,minPort,maxPort);
				CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt", format);
				CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "$$$CMediaStream::get port fails\r\n");
				//error resilience if it still fails at last time 
				if(m_clientRTPPort>maxPort)
					m_clientRTPPort-=2;
				else if(m_clientRTPPort<minPort)
					m_clientRTPPort = minPort + 8;
				SAFE_DELETE(rtpStreamSock);
				rtpStreamSock = NULL;
			}
		}
		if(rtpStreamSock)
			CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "$$$CMediaStream::Init before SetSockRecvBufSize\r\n");
		else
			break;
#define  MAX_SOCKET_BUF (1024 * 300)
#define  STEP_SIZE (1024*16)
		if(m_clientRTPPort==0)
			break;
		int actualSize;
		int setSize=MAX_SOCKET_BUF;
		do 
		{	
			actualSize=rtpStreamSock->SetSockRecvBufSize(setSize);
			if(actualSize==setSize)
				break;
			setSize-=STEP_SIZE;
		} while(0);//setSize>STEP_SIZE);

		sprintf(CLog::formatString,"$$$After SetSockRecvBufSize socksize=%d\r\n",actualSize);
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt",CLog::formatString);
		m_rtpStreamSocket  = rtpStreamSock;
		m_rtcpStreamSocket = rtcpStreamSock;
		return true;
	}while(0);
	return false;
}
bool CMediaStream::Init(char * &sdpParam)
{
	m_mediaAddr = new struct sockaddr_storage;TRACE_NEW("CMediaStream::Init: m_mediaAddr",m_mediaAddr);
	if(m_mediaAddr == NULL)
		return false;
	memcpy(m_mediaAddr, m_rtspSession->SessionAddr(), sizeof(sockaddr_storage));

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
		if(ParseSDPParam_b_AS(sdpParam))
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
		if(ParseSDPAttribute_OpaqueData(sdpParam))
			continue;
#if SUPPORT_ADAPTATION
		if(ParseSDPAttribute_3gppAdapation(sdpParam))
			continue;
#endif//
	}
	CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "$$$CMediaStream::Init one mediastream parse done\r\n");
	if(IsVideoStream())
	{
		CUserOptions::UserOptions.outInfo.clip_width	= m_videoWidth;
		CUserOptions::UserOptions.outInfo.clip_height	= m_videoHeight;
	}
#ifdef MEDIA_FLO
	if(CUserOptions::UserOptions.status==Session_SwitchChannel)
		return true;
#endif//MEDIA_FLO
	do
	{
		CMediaStreamSocket * rtpStreamSock = NULL;
		CMediaStreamSocket * rtcpStreamSock = NULL;
		m_rtpStreamSocket  = rtpStreamSock;
		m_rtcpStreamSocket = rtcpStreamSock;
		if(CUserOptions::UserOptions.m_URLType == UT_SDP2_LOCAL)
		{
			CRealTimeStreamingEngine * streamingEngine = CRealTimeStreamingEngine::CreateRealTimeStreamingEngine();
			if(IsVideoStream())
			{
				m_clientRTPPort = 5000;
				rtpStreamSock = new CMediaStreamSocket(m_mediaAddr->ss_family, 0, m_clientRTPPort);TRACE_NEW("CMediaStream::Init rtpStreamSock",rtpStreamSock);
				rtcpStreamSock = new CMediaStreamSocket(m_mediaAddr->ss_family, 0, m_clientRTPPort + 1);TRACE_NEW("CMediaStream::Init rtcpStreamSock",rtcpStreamSock);
				int rtpSock = (int)rtpStreamSock->GetMediaStreamSock();
				int rtcpSock=(int)rtcpStreamSock->GetMediaStreamSock();
				streamingEngine->SetLocalVideoPort(rtpSock,rtcpSock);
				SLOG2(LL_RTSP_ERR,"flow.txt","v_sock=(%X,%X)\n",rtcpSock,rtcpSock);
			}
			else if(IsAudioStream())
			{
				m_clientRTPPort = 5002;
				rtpStreamSock = new CMediaStreamSocket(m_mediaAddr->ss_family, 0, m_clientRTPPort);TRACE_NEW("CMediaStream::Init rtpStreamSock2",rtpStreamSock);
				rtcpStreamSock = new CMediaStreamSocket(m_mediaAddr->ss_family, 0, m_clientRTPPort + 1);TRACE_NEW("CMediaStream::Init rtcpStreamSock2",rtcpStreamSock);
				int rtpSock = (int)rtpStreamSock->GetMediaStreamSock();
				int rtcpSock=(int)rtcpStreamSock->GetMediaStreamSock();
				streamingEngine->SetLocalAudioPort(rtpSock,rtcpSock);
				SLOG2(LL_RTSP_ERR,"flow.txt","a_sock=(%X,%X)\n",rtcpSock,rtcpSock);
			}
#ifdef DIMS_DATA
			else if(IsDataStream())
			{
				m_clientRTPPort = 1004;
				rtpStreamSock = new CMediaStreamSocket(m_mediaAddr->ss_family, 0, m_clientRTPPort);TRACE_NEW("CMediaStream::Init rtpStreamSock3",rtcpStreamSock);
				rtcpStreamSock = new CMediaStreamSocket(m_mediaAddr->ss_family, 0, m_clientRTPPort + 1);TRACE_NEW("CMediaStream::Init rtcpStreamSock3",rtcpStreamSock);
				streamingEngine->SetLocalDIMSPort(rtpStreamSock->GetMediaStreamSock(),rtcpStreamSock->GetMediaStreamSock());
			}
#endif//DIMS_DATA
			m_rtpStreamSocket  = rtpStreamSock;
			m_rtcpStreamSocket = rtcpStreamSock;
		}
		else if(CUserOptions::UserOptions.m_useTCP<=0)
		{
			//TODO20070515:check port range
			if(!SetTransportProtocol(false))
				break;
		
		}
		if(!CreateRTPPayloadParser(m_rtpStreamSocket, m_rtcpStreamSocket))
			break;
		//m_rtpStreamSocket  = rtpStreamSock;
		//m_rtcpStreamSocket = rtcpStreamSock;
		m_mediaFrameSink = new CMediaFrameSink(this);TRACE_NEW("new CMediaFrameSink() m_mediaFrameSink",m_mediaFrameSink);
		if(m_mediaFrameSink == NULL)
			break;

		m_streamSyncWallClockTime = new struct votimeval;TRACE_NEW("new struct votimeval m_streamSyncWallClockTime",m_streamSyncWallClockTime);
		if(m_streamSyncWallClockTime == NULL)
			break;
		
		m_streamSyncWallClockTime->tv_sec = m_streamSyncWallClockTime->tv_usec = 0;
		m_baseSyncWallClock = new struct votimeval;TRACE_NEW("new struct votimeval m_baseSyncWallClock",m_baseSyncWallClock);
		if(m_baseSyncWallClock == NULL)
			break;
		m_baseSyncWallClock->tv_sec = m_baseSyncWallClock->tv_usec = 0;
		m_streamWallClockTime = new struct votimeval;TRACE_NEW("new struct votimeval m_streamWallClockTime",m_streamWallClockTime);
		if(m_streamWallClockTime == NULL)
			break;
		m_streamWallClockTime->tv_sec = m_streamWallClockTime->tv_usec = 0;

		CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
		if(pRTSPMediaStreamSyncEngine == NULL)
			break;
		m_rtpChannelID = m_streamChannelIDGenerator++;
		m_rtcpChannelID = m_streamChannelIDGenerator++;
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "$$$CMediaStream::Init before AddMediaStream\r\n");
		pRTSPMediaStreamSyncEngine->AddMediaStream(this);
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "$$$CMediaStream::Init all done\r\n");
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
	return IsVideoStream()&&(_stricmp(m_codecName, "x-asf-pf")==0||_stricmp(m_codecName, "x-wms-fec") == 0) ;
}
bool CMediaStream::IsAudioWMA()
{
	return IsAudioStream()&&(_stricmp(m_codecName, "x-asf-pf")==0||_stricmp(m_codecName, "x-wms-fec")  == 0) ;
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
	return _stricmp(m_codecName, "QCELP") == 0||_stricmp(m_codecName, "MP4A-QCELP") == 0;
}
bool CMediaStream::IsAudioRMAudio()
{
	return _stricmp(m_codecName, "x-pn-realaudio") == 0 ;
}
bool CMediaStream::IsAudioG711()
{
	return _stricmp(m_codecName, "PCMU")==0||_stricmp(m_codecName, "PCMA") == 0 ;
}
bool CMediaStream::CreateRTPPayloadParser(CMediaStreamSocket * rtpStreamSock, CMediaStreamSocket * rtcpStreamSock)
{
	if(IsVideoStream())
	{
		if(IsVideoH264())
		{
			m_rtpParser = new CH264VideoRTPParser(this, rtpStreamSock);TRACE_NEW("new CH264VideoRTPParser m_rtpParser",m_rtpParser);
		}
		else if(IsVideoMpeg4())
		{
			m_rtpParser = new CMPEG4ESVideoRTPParser(this, rtpStreamSock);TRACE_NEW("new CMPEG4ESVideoRTPParser m_rtpParser",m_rtpParser);
		}
		else if(IsVideoH263())
		{
			m_rtpParser = new CH263VideoRTPParser(this, rtpStreamSock);TRACE_NEW("new CH263VideoRTPParser m_rtpParser",m_rtpParser);
		}
		else if(IsVideoWMV())
		{
			m_rtpParser = new CASFRTPParser(this, rtpStreamSock);TRACE_NEW("new CASFRTPParser m_rtpParser",m_rtpParser);

		}
	}

	if(IsAudioStream())
	{
		if(_stricmp(m_codecName, "MPEG4-GENERIC") == 0)
		{
			m_rtpParser = new CMPEG4GenericAudioRTPParser(this, rtpStreamSock, m_fmtpSizeLength, m_fmtpIndexLength, m_fmtpIndexDeltaLength);TRACE_NEW("new CMPEG4GenericAudioRTPParser m_rtpParser",m_rtpParser);
		}
		else if(_stricmp(m_codecName, "MP4A-LATM") == 0)
		{
			m_rtpParser = new CMPEG4LATMAudioRTPParser(this, rtpStreamSock);TRACE_NEW("new CMPEG4LATMAudioRTPParser m_rtpParser",m_rtpParser);
		}
		else if(IsAudioAMRWBP())
		{
			m_rtpParser = new CAMRWBPAudioRTPParser(this, rtpStreamSock, m_audioChannels, m_fmtpOctetAlign, m_fmtpInterleaving);TRACE_NEW("new IsAudioAMRWBP() m_rtpParser",m_rtpParser);
		}
		else if(IsAudioAMRWB())
		{
			m_rtpParser = new CAMRAudioRTPParser(this, rtpStreamSock, true, m_audioChannels, m_fmtpOctetAlign, m_fmtpInterleaving, m_fmtpRobustSorting, m_fmtpCRC);TRACE_NEW("new CAMRAudioRTPParser",m_rtpParser);
		}
		else if(IsAudioAMRNB())
		{
			m_rtpParser = new CAMRAudioRTPParser(this, rtpStreamSock, false, m_audioChannels, m_fmtpOctetAlign, m_fmtpInterleaving, m_fmtpRobustSorting, m_fmtpCRC);TRACE_NEW("new CAMRAudioRTPParserNB",m_rtpParser);
		}
		else if(IsAudioQCELP())
		{
			m_rtpParser = new CQcelpAudioRTPParser(this, rtpStreamSock);TRACE_NEW("new IsAudioQCELP()",m_rtpParser);
		}
		else if(IsAudioWMA())
		{
			m_rtpParser = new CASFRTPParser(this, rtpStreamSock);TRACE_NEW("new CASFRTPParser m_rtpParser WMA",m_rtpParser);

		}
		else if(IsAudioG711())
		{
			m_rtpParser = new CG711AudioRTPParser(this, rtpStreamSock);TRACE_NEW("new CG711AudioRTPParser m_rtpParser G711",m_rtpParser);

		}
	}
#ifdef DIMS_DATA
	if(IsDataStream())
	{
		if(_stricmp(m_codecName, "richmedia+xml") == 0)
		{
			
			m_rtpParser = new CDIMSRTPParser(this, rtpStreamSock);TRACE_NEW("new CDIMSRTPParser m_rtcpParser",m_rtpParser);

		}
	}
#endif//
	if(m_rtpParser != NULL)
	{
		unsigned int mediaStreamBandwidth = m_mediaStreamBandwidthAS;
		if(mediaStreamBandwidth == 0)
		{
			mediaStreamBandwidth = m_rtspSession->SessionBandwidth();
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

		m_mediaAddr->family		=  VAF_UNSPEC;
		m_mediaAddr->port		=  0;
		m_mediaAddr->protocol	=  VPT_UDP;
		m_mediaAddr->sockType	=  VST_SOCK_DGRAM;
		int ret =IVOSocket::GetPeerAddressByURL(remoteIPString,strlen(remoteIPString),m_mediaAddr);
		if(ret)
			break;
		return true;

	}while(0);

	return false;
}

bool CMediaStream::ParseSDPParam_b_AS(char * sdpParam)
{
	if(sscanf(sdpParam, "b=AS:%u", &m_mediaStreamBandwidthAS) == 1)
	{
		return true;
	}
	else
	{
		//CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse Param_b_AS\n");

	}
	/*
	unsigned int bandwidthRR = 0;
	if(sscanf(sdpParam, "b=RR:%u", &bandwidthRR) == 1)
	{
		m_mediaStreamBandwidthAS = bandwidthRR * 26.67 / 1024;
		return true;
	}
	else
	{
		//CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse Param_b_RR\n");
	}*/
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
			if(IsAudioAAC())
			{
				m_framerate = m_audioSampleRate/1024;
				if(m_audioChannels == 0) m_audioChannels = 2;
				CUserOptions::UserOptions.outInfo.codecType[1]=kCodecTypeMPEG4AAC;
				m_timePerFrame = (int)(1024000.0/m_audioSampleRate);
				m_scaleAACPlus = 1;
				
				//Add ADTS head
				//WriteADTSHead(m_audioSampleRate,m_audioChannels,0,m_adtsHead);
			}
			else if(IsAudioAMRWBP())
			{
				CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse frame rate for AMR-WBP\n");
				m_framerate = 50;
				if(m_audioChannels == 0) m_audioChannels = 2;
				CUserOptions::UserOptions.outInfo.codecType[1]=kCodecTypeUnknown;
				m_timePerFrame = 20;
			}
			else if(IsAudioAMRWB())
			{
				CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse frame rate for AMR-WB\n");
				m_framerate = 50;
				if(m_audioChannels == 0) m_audioChannels = 1;
				CUserOptions::UserOptions.outInfo.codecType[1]=kCodecTypeGSMAMRWB;
				m_timePerFrame = 20;
			}
			else if(IsAudioAMRNB())
			{
				m_framerate = 50;
				if(m_audioChannels == 0) m_audioChannels = 1;
				CUserOptions::UserOptions.outInfo.codecType[1]=kCodecTypeGSMAMRNB;
				m_timePerFrame = 20;

			}
			else if(IsAudioQCELP())
			{
				m_framerate = 50;
				if(m_audioChannels == 0) m_audioChannels = 1;
				CUserOptions::UserOptions.outInfo.codecType[1]=kCodecTypeQCELP;
				m_timePerFrame = 20;
			}
			else if(IsAudioWMA())
			{
				m_framerate = 5;
				if(m_audioChannels == 0) m_audioChannels = 1;
			}
			else if(IsAudioG711())
			{
				m_framerate = 5;
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

int CMediaStream::ParseAVCConfig(char* pData,char* dst)
{
	//refer 14496-15 5.2.4.1.1
	int i;
	int numOfPictureParameterSets;
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
	if(m_mediaFrameSink)
	{
		CMediaFrame frame={0};
		m_mediaFrameSink->GetFirstFrameInBuffer(&frame);
		if(frame.seqNum)
			return frame.seqNum;
	}
	return 0;
}
bool	CMediaStream::ParseSDPAttribute_3gppAdapation(char* sdpParam)
{
	if(_strnicmp(sdpParam, "a=3gpp-adaptation-support:", 26) != 0)
		return false;
	if(sscanf(sdpParam+26,"%d",&m_reportFreq)==1)
	{
		CUserOptions::UserOptions.m_n3GPPAdaptive=1;
		return true;
	}
	return false;
}
#endif//SUPPORT_ADAPTATION
bool	CMediaStream::ParseSDPAttribute_OpaqueData(char* sdpParam)
{
	if(strncmp(sdpParam, "a=OpaqueData:", 13) != 0)
		return false;
	char* t =strchr(sdpParam,'\"');//[^\"\t\r\n]
	//int pos = strpos(sdpParam,'"');
	if(t&&sscanf(t, "\"%[^\"]", m_fmtpSpropParameterSets) == 1&&strlen(m_fmtpSpropParameterSets)>5&&m_codecHeadDataLen==0)
	{
		
		
	

		if(IsVideoStream())
		{
			m_codecHeadData = new unsigned char[MAX_OPAQUE_SIZE];TRACE_NEW("xxm_codecHeadData =",m_codecHeadData);
			m_codecHeadDataLen = MAX_OPAQUE_SIZE;
			voBaseSixFourDecodeEx((unsigned char*)m_fmtpSpropParameterSets, m_codecHeadData, &m_codecHeadDataLen);
			int videoWidth = 0;
			int videoHeight = 0;
			if(IsVideoH264())
			{
				
				char* dst=(char*)(m_codecHeadData+m_codecHeadDataLen+64);
				m_codecHeadDataLen=ParseAVCConfig((char*)(m_codecHeadData+8),dst);
				memcpy(m_codecHeadData,dst,m_codecHeadDataLen);
#ifdef H264_ANNEXB_FORMAT
				int offset1=4;
#else
				int offset1=2;
#endif//
				if(GetSizeInfo(m_codecHeadData + offset1,m_codecHeadDataLen, &videoWidth, &videoHeight) == 1)
				{
					m_videoWidth = videoWidth;
					m_videoHeight = videoHeight;
				}
				if(0)
				{
					CLog::Log.MakeLog(LL_CODEC_ERR,"codecErr.txt","@@@@The H.264 decoder doest not support the Profile!\n");
					CUserOptions::UserOptions.errorID = E_UNSUPPORTED_CODEC;
					NotifyEvent(VO_EVENT_DESCRIBE_FAIL,CUserOptions::UserOptions.errorID);
				}
				CUserOptions::UserOptions.outInfo.codecType[0]=kCodecTypeH264BL;

				
			}
			else if(IsVideoMpeg4())
			{
				//HexStrToBytes(m_fmtpConfig, &m_codecHeadData, &m_codecHeadDataLen);	
				if(m_codecHeadData==NULL)
				{
					return true;
				}
				if(GetMpeg4SequenceHr(m_codecHeadData, m_codecHeadDataLen, &videoWidth, &videoHeight) == 0)
				{
					m_videoWidth = videoWidth;
					m_videoHeight = videoHeight;
				}

				unsigned char *m_codecHeadDataTmp = m_codecHeadData;
				
				while((*m_codecHeadDataTmp!=0x0) || (*(m_codecHeadDataTmp+1)!=0x0)||
					(*(m_codecHeadDataTmp+2)!=0x01) || 
					(*(m_codecHeadDataTmp+3)!=0x20))
				{
					m_codecHeadDataTmp++;
				}

				m_codecHeadDataLen -= (m_codecHeadDataTmp-m_codecHeadData);
				memmove(m_codecHeadData,m_codecHeadDataTmp,m_codecHeadDataLen);
				
				CUserOptions::UserOptions.outInfo.codecType[0]=kCodecTypeMPEG4SP;
			}
			else
			{
				//SAFE_DELETE_ARRAY(m_codecHeadData);
				m_codecHeadDataLen = 0;
			}
		}
		
	}
	return true;
}

bool CMediaStream::ParseSDPAttribute_fmtp(char * sdpParam)
{
	if(strncmp(sdpParam, "a=fmtp:", 7) != 0)
		return false;

	sdpParam += 7;
	int length ;
	
	while(isdigit(*sdpParam)) ++sdpParam;
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
				voBaseSixFourDecodeEx((unsigned char*)m_fmtpSpropParameterSets, m_codecHeadData, &m_codecHeadDataLen);
	#endif			
				if(GetSizeInfo(m_codecHeadData + 2, m_codecHeadDataLen,&videoWidth, &videoHeight) == 1)
				{
					m_videoWidth = videoWidth;
					m_videoHeight = videoHeight;
				}
				if(0)
				{
					CLog::Log.MakeLog(LL_CODEC_ERR,"codecErr.txt","@@@@The H.264 decoder doest not support the Profile!\n");
					CUserOptions::UserOptions.errorID = E_UNSUPPORTED_CODEC;
					NotifyEvent(VO_EVENT_DESCRIBE_FAIL,CUserOptions::UserOptions.errorID);
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
			}
		}
		else if(IsVideoMpeg4())
		{
			HexStrToBytes(m_fmtpConfig, &m_codecHeadData, &m_codecHeadDataLen);	
			if(m_codecHeadData==NULL)
			{
				return true;
			}
			if(GetMpeg4SequenceHr(m_codecHeadData, m_codecHeadDataLen, &videoWidth, &videoHeight) == 0)
			{
				m_videoWidth = videoWidth;
				m_videoHeight = videoHeight;
			}
			CUserOptions::UserOptions.outInfo.codecType[0]=kCodecTypeMPEG4SP;
		}
		else if(IsVideoH263())
		{
			CUserOptions::UserOptions.outInfo.codecType[0]=kCodecTypeH263BL;
		}
		else if(IsVideoWMV())
		{

		}
		else
		{
			CUserOptions::UserOptions.outInfo.codecType[0]=kCodecTypeUnknown;
			CLog::Log.MakeLog(LL_CODEC_ERR,"codecErr.txt","@@@@unknown video codec!\n");
			CUserOptions::UserOptions.errorID = E_UNSUPPORTED_CODEC;
			return false;
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
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);

	}

	return true;
}

int CMediaStream::GetCodecHeadData(unsigned char * codecHead)
{
	if (!m_codecHeadDataLen && IsAudioG711())
	{
		VO_U32 codecCC = (0 ==_stricmp(m_codecName, "PCMA") ) ? WAVE_FORMAT_ALAW : WAVE_FORMAT_MULAW;
		GenerateWaveFormatEx(codecCC,m_audioChannels, m_rtpTimestampFrequency, 0, &m_WaveFormatEx);

		m_codecHeadDataLen = sizeof(m_WaveFormatEx);
		if (m_codecHeadData)
		{
			SAFE_DELETE(m_codecHeadData);
		}

		m_codecHeadData	= new unsigned char[m_codecHeadDataLen];
		memcpy(m_codecHeadData, &m_WaveFormatEx, m_codecHeadDataLen);

		sprintf(CLog::formatString,"After add head data, m_codecHeadData = %x, m_codecHeadDataLen = %d, m_codecName = %s \n", m_codecHeadData, m_codecHeadDataLen, m_codecName);
		CLog::Log.MakeLog(LL_SOCKET_ERR,"G711.txt",CLog::formatString);
	}

	if(codecHead != NULL)
	{
		memcpy(codecHead, m_codecHeadData, m_codecHeadDataLen);
	}
	return m_codecHeadDataLen;
}

bool CMediaStream::ParseRTPInfoHeader(char * rtpInfoText)
{
	char * field=NULL;
	do
	{
		//char tmpBuf[128];
		//strcpy(tmpBuf,m_controlAttr);
		//strcat(tmpBuf,";");
		char* trackID;
		trackID=strstr(rtpInfoText, m_controlAttr);
		if(trackID== NULL||trackID[strlen(m_controlAttr)]!=';')//check the trackID is in the line
		{
#if TRACE_FLOW1
				sprintf(CLog::formatString,"@@@Target track is not the track(%s)\n",m_controlAttr);
				CLog::Log.MakeLog(LL_RTSP_ERR,"rtp-info.txt",CLog::formatString);
#endif
			break;
		}
		
		field = StringDup(rtpInfoText);
		unsigned int   seqNum=0;
		unsigned int   rtpTimestamp=0;
		//use local bool value to decide whether the field exist
		bool seqNumNotExist = true;
		bool rtpTimestampNotExist = true;
		while(sscanf(rtpInfoText, "%[^;]", field) == 1)
		{
			if(sscanf(field, "seq=%u", &(seqNum)) == 1)
			{
				seqNumNotExist =m_rtpInfo.seqNumNotExist = false;
				m_rtpInfo.seqNum = seqNum;
				
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

		{
			sprintf(CLog::formatString,"RTP_info(%s),(%d,%d)seq=%d,ts=%d\n",m_controlAttr,
				m_rtpInfo.rtpTimestampNotExist,m_rtpInfo.seqNumNotExist,
				m_rtpInfo.seqNum,m_rtpInfo.rtpTimestamp);
			CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt",CLog::formatString);
		}
		
		SAFE_DELETE_ARRAY(field);

		return true;

	}while(0);
	SAFE_DELETE_ARRAY(field);
	return false;
}
#ifdef DIMS_DATA
bool CMediaStream::IsDataStream()
{
	return _stricmp(m_mediaName, "Data") == 0;
}
#endif//DIMS_DATA
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


void  CMediaStream::SetDelayTime(int time)
{
	//time=((time+500)/1000)*1000;

	if(m_rtspTrack)
		m_rtspTrack->SetDelayTime(time);
	m_delayTime = time;
	m_frameNum = 0;
}

void CMediaStream::calculateFPS(unsigned long timeStamp)
{
}
void CMediaStream::AdjustSyncClockWithRTPInfo()
{
	if(m_dataSyncbySeqNum)
		return;
	//The following is map NPT in range head to NTP in RTCP 
	CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
	long rangeBegin= pRTSPMediaStreamSyncEngine->GetPlayRangeBegin();
	if(m_rtpInfo.rtpTimestampNotExist==false)//rangeBegin.tv_sec>0&&
	{
		int timeStampDiff = m_rtpInfo.rtpTimestamp - m_streamSyncTimestamp;
		double timeDiff	  = (timeStampDiff/(double)RTPTimestampFrequency());
		unsigned int seconds, uSeconds;
		if(timeDiff >= 0.0)
		{
			seconds  = m_streamSyncWallClockTime->tv_sec + (unsigned int)(timeDiff)-m_baseSyncWallClock->tv_sec;
			uSeconds = m_streamSyncWallClockTime->tv_usec + (unsigned int)((timeDiff - (unsigned int)timeDiff) * million)-m_baseSyncWallClock->tv_usec;
			if((long)uSeconds >= million)
			{
				uSeconds -= million;
				++seconds;
			}
		}
		else
		{
			timeDiff = -timeDiff;
			seconds  = m_streamSyncWallClockTime->tv_sec - (unsigned int)(timeDiff)-m_baseSyncWallClock->tv_sec;
			uSeconds = m_streamSyncWallClockTime->tv_usec - (unsigned int)((timeDiff - (unsigned int)timeDiff) * million)-m_baseSyncWallClock->tv_usec;
			if((int)uSeconds < 0)
			{
				uSeconds += million;
				--seconds;
			}
		}
		int beforeSyncTime = (int)m_streamTimeBeforeSync/10;
		int expectNPT = seconds*million+uSeconds+beforeSyncTime;
		int startTime = m_rtspTrack->GetStartTime()*1000;
		int actualNPT = rangeBegin-startTime;
		if(actualNPT<0)
			actualNPT = 0;
		int adjustTime = expectNPT-actualNPT;
		int absAdjustTime = abs(adjustTime);
		int limit = million/4;
		if(expectNPT>0&&absAdjustTime>limit)
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
				if(m_streamSyncWallClockTime->tv_usec>=million)
				{
					m_streamSyncWallClockTime->tv_sec++;
					m_streamSyncWallClockTime->tv_usec-=million;
				}
			}

			if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
			{
				sprintf(CLog::formatString,"NTP-->NPT: adjust=%d(calTime=%d-actual=%d(start=%d),timeStamp(%d-%d)\n",
					adjustTime/1000,expectNPT/1000,actualNPT/1000,startTime/1000,m_rtpInfo.rtpTimestamp,m_streamSyncTimestamp);
				if(IsVideoStream())
					CLog::Log.MakeLog(LL_VIDEO_TIME,"videoTime.txt",CLog::formatString);
				else 
					CLog::Log.MakeLog(LL_AUDIO_TIME,"audioTime.txt",CLog::formatString);
			}
		}
	}
}
//TODO:Use AdjustSyncClock approximately
void CMediaStream::AdjustSyncClockWhenRTCP()
{
	if(m_dataSyncbySeqNum)
		return;
	if(m_previousRTCP.timeStamp ==0)//firstTime
	{
			m_previousRTCP.timeStamp = m_streamSyncTimestamp;
			m_previousRTCP.sec		 = m_streamSyncWallClockTime->tv_sec;
			m_previousRTCP.uSec		 = m_streamSyncWallClockTime->tv_usec;
	}
	else //if(m_adjustTimeBetweenPauseAndPlay>0)//check whether the time has jumped by pause/play
	{
#if 1
		m_streamSyncTimestamp = m_previousRTCP.timeStamp;
		m_streamSyncWallClockTime->tv_sec =m_previousRTCP.sec;
		m_streamSyncWallClockTime->tv_usec=m_previousRTCP.uSec;
#else
			int timeStampDiff = m_streamSyncTimestamp - m_previousRTCP.timeStamp;
			double timeDiff	  = (timeStampDiff/(double)RTPTimestampFrequency());
			int	   expectTimediff = int(timeDiff*million);
			int secDiff	  = (m_streamSyncWallClockTime->tv_sec-m_previousRTCP.sec)*million;
			int usecDiff  = (m_streamSyncWallClockTime->tv_usec-m_previousRTCP.uSec);
			int adjustTime = secDiff+usecDiff-expectTimediff;
			int absAdjustTime = abs(adjustTime);
			if(absAdjustTime>million/2)//if the difference is great than 500ms,then adjust it
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
						CLog::Log.MakeLog(LL_VIDEO_TIME,"videoTime.txt",CLog::formatString);
					else
						CLog::Log.MakeLog(LL_AUDIO_TIME,"audioTime.txt",CLog::formatString);
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
	if(m_rtspTrack==NULL||m_dataSyncbySeqNum||m_rtpInfo.rtpTimestampNotExist)
		return;
	float startPos   = m_rtspTrack->GetStartTime()/1000.0;
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
			sprintf(CLog::formatString,"AdjustSyncClock:begin=%f(%f-%f),SYN_CLOCK(%ld,%ld),ts=%u\n",begin,actualTime,startPos,m_streamSyncWallClockTime->tv_sec,m_streamSyncWallClockTime->tv_usec,m_rtpInfo.rtpTimestamp);
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
void CMediaStream:: StoreStreamPreviousPos(int previousStart)
{
	int curSectionPos = m_mediaFrameSink->GetLastTimeInBuffer();
	m_previousPos = curSectionPos+previousStart;
}
const int sampRateTab[12] = {
	96000, 88200, 64000, 48000, 44100, 32000, 
	24000, 22050, 16000, 12000, 11025,  8000
};

void WriteADTSHead(int sampFreq, int chanNum,int framelen,unsigned char *adtsbuf)
{
	int sampIdx = 0 ,object=1;//LC
	adtsbuf[0] = 0xFF; adtsbuf[1] = 0xF9;
	for (int idx = 0; idx < 12; idx++) {
		if (sampFreq == sampRateTab[idx]) {
			sampIdx = idx;
			break;
		}
	}
	adtsbuf[2] = (unsigned char)((object << 6) | (sampIdx << 2) | ((chanNum&4)>>2));

	framelen += 7;

	

	adtsbuf[3] = (chanNum << 6) | (framelen >> 11);
	adtsbuf[4] = (framelen & 0x7FF) >> 3;
#if 1
	adtsbuf[5] = ((framelen & 7) << 5) ;//| 0x1F;
	adtsbuf[6] = 0x00;	//0xFC
#else
	adtsbuf[5] = ((framelen & 7) << 5) | 0x1F;
	adtsbuf[6] = 0xFC;
#endif
}
int Config2ADTSHeader(/*in*/	unsigned char *config, 
					  /*in*/	unsigned int  conlen,
					  /*in*/	unsigned int  framelen,
					  /*in&out*/unsigned char *adtsbuf
					  )
{
	int object, sampIdx, sampFreq, chanNum;

	if(config == NULL || conlen < 2 || adtsbuf == NULL)
		return -1;

	object = config[0] >> 3;
	if(object > 5)
	{
		return -2;
	}

	if(object == 0) object += 1;

	object--;

	sampIdx = ((config[0] & 3) << 1) | (config[1] >> 7);
	if(sampIdx == 0x0f)
	{
		int idx;

		if(conlen < 5)
			return -3;

		sampFreq = ((config[1]&0x7f) << 17) | (config[2] << 9) | ((config[3] << 1)) | (config[4] >> 7);

		for (idx = 0; idx < 12; idx++) {
			if (sampFreq == sampRateTab[idx]) {
				sampIdx = idx;
				break;
			}
		}

		if (idx == 12)
			return -4;

		chanNum = (config[4]&0x78) >> 3;
	}
	else
	{
		chanNum = (config[1]&0x78) >> 3;
	}

	if(chanNum > 7)
		return -5;

	adtsbuf[0] = 0xFF; adtsbuf[1] = 0xF9;

	adtsbuf[2] = (unsigned char)((object << 6) | (sampIdx << 2) | ((chanNum&4)>>2));

	framelen += 7;

	if(framelen > 0x1FFF)
		return -6;

	adtsbuf[3] = (chanNum << 6) | (framelen >> 11);
	adtsbuf[4] = (framelen & 0x7FF) >> 3;
	adtsbuf[5] = ((framelen & 7) << 5);// | 0x1F;
	adtsbuf[6] = 0;//0xFC;	
	return 0;
}

void CMediaStream::GetMediaData(unsigned char * frameData, int frameSize)
{
#if 1
	// dump video data
	static bool hadCreateDumpFile = false;
	static bool hadCreateDumpFileAudio = false;
	//if((CUserOptions::UserOptions.m_bMakeLog>=LOG_DATA))
	{
		CLog::EnablePrintTime(false);
		int headOffset=0;
		if(IsVideoStream())
		{
			int IsH264 = IsVideoH264();
			unsigned char flag[4];
			CLog::EnablePrintTime(false);
			if(!hadCreateDumpFile)
			{
				hadCreateDumpFile = true;
				if(IsH264)
				{
#ifndef H264_ANNEXB_FORMAT
					unsigned char* buffer = m_codecHeadData;
					flag[0] = 0x00;
					flag[1] = 0x00;
					flag[2] = 0x00;
					flag[3] = 0x01;
					CLog::Log.MakeLog(LL_AUDIO_TIME,"VideoData.dat", flag, 4);

					int sisLen=(buffer[0]<<8)|(buffer[1]);
					CLog::Log.MakeLog(LL_AUDIO_TIME,"VideoData.dat", buffer + 2, sisLen);

					
					flag[0] = 0x00;
					flag[1] = 0x00;
					flag[2] = 0x00;
					flag[3] = 0x01;
					CLog::Log.MakeLog(LL_AUDIO_TIME,"VideoData.dat", flag, 4);

					
					buffer+=sisLen+2;
					int picLen=(buffer[0]<<8)|(buffer[1]);
					CLog::Log.MakeLog(LL_AUDIO_TIME,"VideoData.dat", buffer + 2, picLen);
#else
					unsigned char* buffer = m_codecHeadData;
					CLog::Log.MakeLog(LL_AUDIO_TIME,"VideoData.dat", buffer, m_codecHeadDataLen);
#endif
				}
				else if(IsVideoMpeg4())
				{
					int IsMPEG4 = _stricmp(m_codecName, "MP4V-ES") == 0;
					if(IsMPEG4)
					{

						CLog::Log.MakeLog(LL_AUDIO_TIME,"VideoData.dat", m_codecHeadData, m_codecHeadDataLen);
					}
				}


			}
#if 1//ndef H264_ANNEXB_FORMAT
			if(IsH264)
			{
				flag[0] = 0x00;
				flag[1] = 0x00;
				flag[2] = 0x00;
				flag[3] = 0x01;
				//flag=0x01000000;
				CLog::Log.MakeLog(LL_AUDIO_TIME,"VideoData.dat", flag, 4);
				headOffset=4;
			}
#endif
			CLog::Log.MakeLog(LL_AUDIO_TIME,"VideoData.dat", frameData, frameSize);

		}//video
		else//audio
		{
			if(!hadCreateDumpFileAudio)
			{
				hadCreateDumpFileAudio = true;
				if(IsAudioAMRNB())
				{				
					const char* amrHead= "#!AMR\n";
					CLog::EnablePrintTime(false);
					CLog::Log.MakeLog(LL_AUDIO_TIME,"AudioData.dat", (unsigned char*)amrHead, strlen(amrHead));
				}
				else if(IsAudioAMRWB())
				{
					const char* amrHead= "#!AMR-WB\n";
					CLog::EnablePrintTime(false);
					CLog::Log.MakeLog(LL_AUDIO_TIME,"AudioData.dat", (unsigned char*)amrHead, strlen(amrHead));
				}
			}
			if(IsAudioAAC())//AAC
				{
#ifndef FORCE_ADD_ADTS
					static unsigned char ADTSHead[7];
					//int ADTSLen=7;
					//Config2ADTSHeader(m_codecHeadData,m_codecHeadDataLen,frameSize,ADTSHead);//&ADTSLen);
					WriteADTSHead(m_audioSampleRate,m_audioChannels,frameSize,ADTSHead);
				CLog::EnablePrintTime(false);
					CLog::Log.MakeLog(LL_AUDIO_TIME,"AudioData.dat", (unsigned char*)ADTSHead, 7);
#endif
				}
			CLog::EnablePrintTime(false);
			CLog::Log.MakeLog(LL_AUDIO_TIME,"AudioData.dat", frameData, frameSize);
		}

	}
#endif
}
#ifdef DIMS_DATA
void CMediaStream::RecvMediaFrameDIMSData(unsigned char * frameData, int frameSize, unsigned int rtpTimestamp,unsigned int seqNum)
{
	__int64 frameStartTime = 0;
	__int64 frameEndTime = 0;
	if(m_firstTimeStamp==-1)
		m_firstTimeStamp = rtpTimestamp;
	CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
	long oriTime; 

	//after set position and there is no RTP-Info and RTCP-sync
	if(IsNeedMinusStartTime()&&m_streamStartTime>0)
	{
		pRTSPMediaStreamSyncEngine->CalculateMediaFramePresentationTimeBasedOnFirstFrame(this, rtpTimestamp, &oriTime);
	}
	else
	{
		pRTSPMediaStreamSyncEngine->CalculateMediaFramePresentationTime(this, rtpTimestamp, &frameStartTime, &frameEndTime);
		oriTime = frameStartTime / 10000+m_nPausedTime;//it is safe to judge if the time is less than 0 using 32bit calculation. 

	}
	char* format = CLog::formatString;
	long tmpTime = oriTime;
	if(tmpTime<0)
		tmpTime = 0;
	unsigned long startTime = tmpTime;
	unsigned long endTime   = startTime + 1;
	//TODO:handle the delay time
	m_mediaFrameSink->SinkMediaFrame(frameData, frameSize, startTime, startTime+1, 0);
}
#endif//DIMS_DATA

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
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","ResetTmpBuf()\n");
}
unsigned long CMediaStream::CalculateNPT(unsigned long rtpTimestamp,int seqNum)
{
	long frameStartTime = 0;

	unsigned long startTime=0; 
	long tmpTime ;
	CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
	long oriTime; 
	//after set position and there is no RTP-Info and RTCP-sync
	if(CUserOptions::UserOptions.streamType==ST_ASF)
	{
		pRTSPMediaStreamSyncEngine->CalcualteMediaFramePresentationTimeASF(this,rtpTimestamp,seqNum,&oriTime);
	}
	else if((IsNeedSyncByFirstFrame()&&m_streamStartTime>0))//||(CUserOptions::UserOptions.streamType==ST_ASF))
	{
		pRTSPMediaStreamSyncEngine->CalculateMediaFramePresentationTimeBasedOnFirstFrame(this, rtpTimestamp, &oriTime);

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
	if(IsAudioAMRNB()||IsAudioAMRWB()||IsAudioAAC())
	{
		//for dshow,it is necessary to adjust the TS to make the sound smooth
		unsigned long lastTime = m_mediaFrameSink->GetLastTimeInBuffer();
		if(startTime+10 <lastTime+m_timePerFrame&&startTime>0)
			startTime = lastTime + m_timePerFrame;		
	}
	return startTime;
}
void CMediaStream::RecvMediaFrame(unsigned char * frameData, int frameSize, unsigned int rtpTimestamp,unsigned int seqNum,bool IsKeyFrame)
{
	unsigned long startTime;
	
	sprintf(CLog::formatString,"RecvMediaFrame::framedata=%x, size=%d, timestamp=%u, seq=%d, key=%d\n",
		(unsigned int)frameData,frameSize,rtpTimestamp,seqNum,IsKeyFrame);
	if(IsVideoStream())
	{
		CLog::Log.MakeLog(LL_RTP_ERR,"videoFrame.txt",CLog::formatString);	
	}
	else if (IsAudioStream())
	{
		CLog::Log.MakeLog(LL_RTP_ERR,"audioFrame.txt",CLog::formatString);	
	}

#if TRACE_FLOW_RTPPARSER
	if(IsVideoStream())
		CLog::Log.MakeLog(LL_RTP_ERR,"WMV.txt","$$$$$enter ParseRTPPayload()\n");
#endif//
#ifdef DIMS_DATA
	if(IsDataStream())
		return RecvMediaFrameDIMSData(frameData,frameSize,rtpTimestamp);
#endif//DIMS_DATA
#if 1//def DUMP_MEDIA_DATA
	if(CUserOptions::UserOptions.m_bMakeLog>LOG_DATA)
	{
		GetMediaData(frameData,frameSize);
	}
#endif//WIN_X86
	if(m_firstTimeStamp==-1)
		m_firstTimeStamp = rtpTimestamp;
	CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
	long oriTime; 
	long flag=0;
	if(pRTSPMediaStreamSyncEngine->GetPlayResponse())
	{
		//make sure:
		if(pRTSPMediaStreamSyncEngine->GetPlayResponse()&&m_rtpInfo.seqNumNotExist==false)
		{
			if(IsValidFrameBySeqNum(seqNum)==false)
			{
				sprintf(CLog::formatString,"processing3::IsValidFrameBySeqNum(seqNum)==false\n");
				if(IsVideoStream())
				{
					CLog::Log.MakeLog(LL_RTP_ERR,"videoFrame.txt",CLog::formatString);	
				}
				else if (IsAudioStream())
				{
					CLog::Log.MakeLog(LL_RTP_ERR,"audioFrame.txt",CLog::formatString);	
				}
				return;
			}
		}
		startTime = CalculateNPT(rtpTimestamp,seqNum);
		if(startTime==INVALID_TIMESTAMP)
		{
			return ;
		}
		if(startTime<0)
			startTime = 0;

	}
	else
	{

		startTime = rtpTimestamp;
		oriTime = rtpTimestamp;//it needs to be calculated after the play response is done
		flag|=SINK_NEED_RECALCULATE_TS;
		if(m_recalFrameNum==0&&IsVideoH264())
		{
			ResetTmpBuf();
			m_previousFrameTime = -1;
		}
		m_recalFrameNum++;
		sprintf(CLog::formatString,"processing4::Recalcualte loop(response=%d,%d\n",pRTSPMediaStreamSyncEngine->GetPlayResponse(),pRTSPMediaStreamSyncEngine->IsSyncByRTPInfo());
		if(IsVideoStream())
		{
			CLog::Log.MakeLog(LL_RTP_ERR,"videoFrame.txt",CLog::formatString);	
		}
		else if (IsAudioStream())
		{
			CLog::Log.MakeLog(LL_RTP_ERR,"audioFrame.txt",CLog::formatString);	
		}
	}
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
				if(IsVideoH264AVC())
				{
					if(m_lengthSizeOfNALU==4)
						actualSize=GET_NALUSIZE4(buffer);
					else if(m_lengthSizeOfNALU==2)
					{
						actualSize=GET_NALUSIZE2(buffer);
						totalSize+=2;//because the lenthofNalu increase 2 bytes,so does totalSize
					}
					else
					{
						CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt","m_lengthSizeOfNALU is not 2 or 4\n");
					}
					if(actualSize>frameSize)//some error happens
					{
						sprintf(CLog::formatString,"AVC:actualSize(%X)>frameSize(%X)\n",actualSize,frameSize);
						CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt",CLog::formatString);
						return;

					}
					buffer+=m_lengthSizeOfNALU;
					frameData=buffer;
					frameSize=actualSize;
					startTime=m_nCurPlayTime;
				}
				
				int naluType = frameData[0]&0x0f;
				isIntraFrame = false;
				
				if(naluType==5)
				{
					isIntraFrame =	true;
				}
				else
				{
					isIntraFrame = IsH264IntraFrame(frameData, frameSize) ==1;
				}
				
				if(rtpTimestamp==(unsigned int)m_previousFrameTime)
				{
					isTheSamePic=true;
#if !H264_COMBINE_ALL_NALU
					if(m_previousFrameIsIntra)
						isIntraFrame = false;//this is for the DShow Notify to get the correct I slice,which makes no sense in COMBINE_NALU
#endif//#if H264_COMBINE_ALL_NALU
				}
				else
				{
					isTheSamePic=false;
				}
#ifdef H264_ANNEXB_FORMAT
				
				if(naluType==6)//ignore SEI
					return;
				frameData-=4;
				frameSize+=4;
				frameData[0]=frameData[1]=frameData[2]=0;frameData[3]=1;
				/*if(m_frameNum<2&&m_codecHeadDataLen>0)//set the seq and pic head in the first 2 frames
				{
					frameData-=m_codecHeadDataLen;
					frameSize+=m_codecHeadDataLen;
					memcpy(frameData,m_codecHeadData,m_codecHeadDataLen);
				}*/
//#endif//
//#ifdef INTACT_AVC_FRAME
#else//H264_ANNEXB_FORMAT
#define L2B_CONVERT_LONG(a) ((((a)>>24)&0x000000ff)|(((a)>>8)&0x0000ff00)|(((a)<<8)&0x00ff0000)|(((a)<<24)&0xff000000))
				frameData-=4;
				char* naluSize=(char*)&frameSize;
				//long *naluSize=(long*)frameData;
				//*naluSize=L2B_CONVERT_LONG(frameSize);
				frameData[0]=naluSize[3];
				frameData[1]=naluSize[2];
				frameData[2]=naluSize[1];
				frameData[3]=naluSize[0];
				frameSize+=4;
				
#endif//H264_ANNEXB_FORMAT
			}
			else if(IsVideoMpeg4())
			{
#if TRACE_FLOW1
				if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
				{
					sprintf(format,"frameSize=%d,frameData(%X,%X,%X,%X,%X,%X)IsIntra=%d\n",frameSize,frameData[0],
						frameData[1],frameData[2],frameData[3],frameData[4],frameData[5],(frameData[4]>>6)==0);
					CLog::Log.MakeLog(LL_VIDEO_TIME,"flow2.txt",format);
				}
#endif
				
				isIntraFrame = IsMPEG4IntraFrame(frameData, frameSize) != 0;
			}
			else if(IsVideoH263())
			{
				isIntraFrame = IsH263IntraFrame(frameData, frameSize) != 0;
			}
			else if(IsVideoWMV())
			{
				isIntraFrame = IsKeyFrame;
			}
			isVideo = true;
			calculateFPS(rtpTimestamp);
			
		}
		else
		{
			isVideo = false;
#ifdef FORCE_ADD_ADTS
			if (IsAudioAAC())
			{
				if (!(frameData[0]==0xFF&&(frameData[1]&0xF0) == 0xF0))
				{
					WriteADTSHead(m_audioSampleRate,m_audioChannels,frameSize,frameData-7);
					frameData-=7;
					frameSize+=7;
				}
			}
#endif//FORCE_ADD_ADTS			
		}
		
		do 
		{
#define FORCE_I_FRAME 0
				
#if H264_COMBINE_ALL_NALU
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
						CLog::Log.MakeLog(LL_RTSP_ERR,"h264.txt",CLog::formatString);
						ResetTmpBuf();
						
						return;
					}
					//SLOG5(LL_RTSP_ERR,"h264.txt","%d,seq=%d,ts=%d,intra1=%d,newIntra=%d\n",m_frameNum,seqNum,startTime,m_gTmpBuf->isIntra,isIntraFrame);
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
#if FORCE_I_FRAME
							if(!(m_frameNum==0&&m_gTmpBuf->isIntra==0))//guarantee the first frame is Intra
#else//FORCE_I_FRAME
							if(1)
#endif//FORCE_I_FRAME
							{
								m_mediaFrameSink->SinkMediaFrame(m_gTmpBuf->buffer, m_gTmpBuf->size, m_gTmpBuf->time, m_gTmpBuf->seqNum, m_gTmpBuf->isIntra,m_gTmpBuf->flag);
								m_frameNum++;
							}
							else
							{
								SIMPLE_LOG("videoDataStatus.txt","waiting for h.264 I frame\n");
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
#endif//H264_ANNEXB_FORMAT
				{
#if FORCE_I_FRAME
					if(!(m_frameNum==0&&isVideo&&!isIntraFrame))//guarantee the first frame is Intra
#else//FORCE_I_FRAME
					if(1)
#endif//FORCE_I_FRAME
					{
						m_mediaFrameSink->SinkMediaFrame(frameData, frameSize, startTime, seqNum, isIntraFrame,flag);
						m_frameNum++;
					}
					else
					{
						SIMPLE_LOG("videoDataStatus.txt","waiting for I frame\n");
					}
				
				}
			
				m_previousFrameTime			= rtpTimestamp;//m_nCurPlayTime;
				m_previousFrameIsIntra		= isIntraFrame;
		} while(0);
	
		//The following go on processing the AVC format
		totalSize-=frameSize;
		frameData+=frameSize;
		frameSize=totalSize;
		isGoOnNextAVCNalu=false;
		if(IsVideoH264AVC()&&frameSize>2)
		{
			isGoOnNextAVCNalu=true;
		}
	} while(isGoOnNextAVCNalu);
	


}

unsigned char*	CMediaStream::GetCodecHeadBuffer(int size)
{
	SAFE_DELETE_ARRAY(m_codecHeadData);
	m_codecHeadData = new unsigned char[size];TRACE_NEW("xxm_m_codecHeadData =",m_codecHeadData);
	m_codecHeadDataLen = size;
	return m_codecHeadData;
}
//int CMediaStream::GetMediaFrame(unsigned char * frameData, int * frameSize, unsigned long * startTime, unsigned long * endTime, bool & bSyncPoint)
//{
//	return m_mediaFrameSink->GetMediaFrame(frameData, frameSize, startTime, endTime, bSyncPoint);
//}
bool	CMediaStream::IsNeedSyncByFirstFrame()
{
	CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();

	return !pRTSPMediaStreamSyncEngine->IsSyncEngineSyncbyRTCP()&&IsRTPInfoTimeStampNotExist();
}
void	CMediaStream::	CalculateNPTAfterPlayResponse()
{
	if(m_recalFrameNum)
	{
		m_mediaFrameSink->CalculateNPTAfterPlayResponse();
		m_recalFrameNum = 0;
	}
}

void CMediaStream::FlushMediaStream(int flushFlag)
{	
	CUserOptions::UserOptions.m_nSinkFlushed=1;

	if(flushFlag&FLUSH_RTPBUFFER)
	{
		if(m_rtpParser)
			m_rtpParser->ReleaseReorderedRTPPackets();
	}

	if(flushFlag&FLUSH_MEDIABUFFER)
	{
		if(m_dataSyncbySeqNum)
		{
			CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
			pRTSPMediaStreamSyncEngine->RestartMediaStream(this);
		}
		else
		{
			m_isNewStart = true;
			m_streamTimeBeforeSync = 0;
		}
		m_delayTime = 0;
		m_frameNum = 0;
		m_mediaFrameSink->FlushMediaFrame(m_rtspTrack->GetStartTime());		
	}

	if(IsVideoH264()&&(flushFlag&FLUSH_H264TEMPBUFFER))
	{
		m_previousFrameTime = -1;
		ResetTmpBuf();
	}
}

unsigned long CMediaStream::HaveBufferedMediaStreamTime(bool flag)
{
#if TRACE_FLOW_REBUFFERING
	if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
	{
		sprintf(CLog::formatString,"buffered frame=.%d fps=%d\n",HaveBufferedMediaFrameCount(),m_framerate);
		CLog::Log.MakeLog(LL_RTSP_ERR,"buffering.txt",CLog::formatString);
	}

#endif//	
	if(IsAudioStream())
		return m_mediaFrameSink->HaveBufferedTime(flag);//return HaveBufferedMediaFrameCount()*1000/m_framerate;//audio's fps is stable
	else if (IsVideoStream())
		return m_mediaFrameSink->HaveBufferedTime(flag);//video's fps is not stable,do not use it
	else
		return 10*1000;//if there is other data stream,regard it as ok
}

int CMediaStream::HaveBufferedMediaFrameCount()
{
	return m_mediaFrameSink->HaveSinkedMediaFrameCount();
}
int  CMediaStream::GetActualBitrate()
{
	int frameSizeInBit = m_mediaFrameSink->HaveBufferdFrameSize()*8;
	int timeInMS	   = m_mediaFrameSink->BufferLocalTime();
	if(timeInMS==0)
		timeInMS=1000;
	return int(frameSizeInBit*1000.0/(timeInMS*1.0));
}
VOFILEREADRETURNCODE CMediaStream::GetMediaFrameByIndex(VOSAMPLEINFO * pSampleInfo)
{

	if(pSampleInfo->pBuffer == NULL)
		return m_mediaFrameSink->GetMediaFrameInfoByIndex(pSampleInfo);
	else //if(m_recalFrameNum==0)//if the ts in buffer needs to be calculated,do not get the frame
		return m_mediaFrameSink->GetMediaFrameByIndex(pSampleInfo);
	//else
	//	return VORC_FILEREAD_NEED_RETRY;
}

int CMediaStream::GetNextKeyFrame(int nIndex, int nDirectionFlag)
{
	return m_mediaFrameSink->GetNextKeyFrame(nIndex, nDirectionFlag);
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
	return m_rtspSession->IsNeedRTCPSync();
}
