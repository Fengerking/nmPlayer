#include "RTSPMediaStreamSyncEngine.h"
#include "MediaStream.h"
#include "MediaStreamSocket.h"
#include "RealTimeStreamingEngine.h"

#include "network.h"
#include "utility.h"
#include "TaskSchedulerEngine.h"

#include "HashTable.h"
#include "RTCPReportPacket.h"
#include "RRUtility.h"

#include "mswitch.h"
#include "RTPParser.h"
#include "RTPSenderTable.h"
#include "RTCPParser.h"

#include "voLog.h"
#define LOG_TAG "FLOEngine_RTCPParser"

#include "mylog.h"

// ------------------------- CRTCPMemberTable  --------------------
class CRTCPMemberTable
{
public:
	CRTCPMemberTable(CRTCPParser * rtcpParser)
	: m_rtcpParser(rtcpParser)
	, m_numMembers(1)
	{
		m_memberTable = new CHashTable(HASH_KEY_WORD);TRACE_NEW("rtcpp_m_memberTable",m_memberTable);
	}

	virtual ~CRTCPMemberTable() 
	{
		if(m_memberTable != NULL)
		{
			SAFE_DELETE(m_memberTable);
			m_memberTable = NULL;
		}
	}

public:
	bool IsMember(unsigned int SSRC) 
	{
		return m_memberTable->Lookup((char *)(long)SSRC) != NULL;
	}

	bool NoteMembership(unsigned int SSRC, unsigned int curTimeCount)
	{
		bool isNew = !IsMember(SSRC);

		if(isNew) 
		{
			++m_numMembers;
		}

		m_memberTable->Add((char *)(long)SSRC, (void *)(long)curTimeCount);

		return isNew;
	}

	bool Remove(unsigned int SSRC) 
	{
		bool hasPresented = m_memberTable->Remove((char *)(long)SSRC);
		if(hasPresented) 
		{
			--m_numMembers;
		}
		return hasPresented;
	}

	unsigned int NumMembers()
	{
		return m_numMembers;
	}

	void ReapOldMembers(unsigned int threshold)
	{
		bool foundOldMember;
		unsigned int oldSSRC = 0;

		do 
		{
			foundOldMember = false;

			CHashTable::Iterator * iter = new CHashTable::Iterator(*m_memberTable);TRACE_NEW("rtcpp_iter",iter);
			unsigned long timeCount;
			const char * key;
			while((timeCount = (unsigned long)(iter->Next(key))) != 0)
			{
				if(timeCount < (unsigned long)threshold) 
				{ 
					unsigned long SSRC = (unsigned long)key;
					oldSSRC = (unsigned int)SSRC;
					foundOldMember = true;
				}
			}
			SAFE_DELETE(iter);

			if(foundOldMember) 
			{
				m_rtcpParser->RemoveSSRC(oldSSRC, true);
			}
		} while(foundOldMember);
	}

private:
	CRTCPParser   * m_rtcpParser;
	unsigned int    m_numMembers;
	CHashTable    * m_memberTable;
};

//-------------------  CRTCPParser --------------------------------------

double NowTime2Double() 
{
	struct timeval nowTime;
	gettimeofday(&nowTime);
	return (double)(nowTime.tv_sec + nowTime.tv_usec / 1000000.0);
}

const int IP_UDP_HEADER_SIZE = 28;
const int MAX_RTCP_PACKET_SIZE = 1460;
void		CRTCPParser::UpdateSocket(CMediaStreamSocket * socket)
{
	m_rtcpStreamSock=socket;
	if(CUserOptions::UserOptions.m_useTCP<=0)
	{
		int flag;
		if(m_mediaStream->IsVideoStream())
			flag = VOS_STREAM_VIDEO_RTCP;
		else if(m_mediaStream->IsAudioStream())
			flag = VOS_STREAM_AUDIO_RTCP;
		else
			flag = VOS_STREAM_ALL;
		CRealTimeStreamingEngine * streamingEngine = CRealTimeStreamingEngine::CreateRealTimeStreamingEngine();
		streamingEngine->AddStreamReader(m_rtcpStreamSock->GetMediaStreamSock(), ReadRTCPStream, this,flag);

	}
	else
	{
		m_streamingOverTCP = true;
	}
}
CRTCPParser::CRTCPParser(CMediaStream * mediaStream, CMediaStreamSocket * rtcpStreamSock, unsigned int totalSessionBandwidth)
: m_mediaStream(mediaStream)
, m_rtcpStreamSock(rtcpStreamSock)
, m_rtcpReceivePacket(NULL)
, m_totalSessionBandwidth(totalSessionBandwidth)
, m_outgoingReportCount(1)
, m_aveRTCPSize(0)
, m_isInitial(1)
, m_prevNumMembers(0)
, m_lastSentSize(0)
, m_lastReceivedSize(0)
, m_lastReceivedSSRC(0)
, m_typeOfEvent(0)
, m_typeOfPacket(0)
, m_haveJustSentPacket(false)
, m_lastPacketSentSize(0)
, m_streamingOverTCP(false)
, m_streamTCPSock(NULL)
, m_taskToken(0)
, m_nSenderReportNum(0)
#if SUPPORT_ADAPTATION
,m_NADUCount(0)
#endif//SUPPORT_ADAPTATION
{
	m_prevReportTime = m_nextReportTime = NowTime2Double();

	UpdateSocket(rtcpStreamSock);
	
	m_rtcpPacketSize = 0;
	m_rtcpReceivePacket = new unsigned char [MAX_RTCP_PACKET_SIZE];TRACE_NEW("rtcpp_m_rtcpReceivePacket",m_rtcpReceivePacket);
	m_rtcpReportPacket = new CRTCPReportPacket(MAX_RTCP_PACKET_SIZE);TRACE_NEW("rtcpp_m_rtcpReportPacket",m_rtcpReportPacket);
	m_knownMembers = new CRTCPMemberTable(this);TRACE_NEW("rtcpp_m_knownMembers",m_knownMembers);

	//char CNAME[256];
	//memset(CNAME, 0, 256);
	//GetLocalHostName((char *)CNAME, 256);
	char* CNAME="VisualOnStreaming";
	m_CNAME = new CSDESItem(RTCP_SDES_CNAME, (unsigned char *)CNAME);TRACE_NEW("rtcpp_m_CNAME",m_CNAME);


	// Send our first report.
//	OnExpire(this);
}

CRTCPParser::~CRTCPParser()
{
	CRealTimeStreamingEngine * streamingEngine = CRealTimeStreamingEngine::CreateRealTimeStreamingEngine();
	if(m_rtcpStreamSock)
	{
		streamingEngine->RemoveStreamReader(m_rtcpStreamSock->GetMediaStreamSock());
		SAFE_DELETE(m_rtcpStreamSock);
		CLog::Log.MakeLog(LL_RTP_ERR,"flow.txt","release one rtcp socket\n");
	}
	

	SAFE_DELETE_ARRAY(m_rtcpReceivePacket);
	SAFE_DELETE(m_rtcpReportPacket);
	SAFE_DELETE(m_knownMembers);
	SAFE_DELETE(m_CNAME);
}

int CRTCPParser::ReadRTCPStream(void * param)
{
	CRTCPParser * rtcpParser = (CRTCPParser *)param;
	CMediaStream * mediaStream = rtcpParser->m_mediaStream;
	
	rtcpParser->m_streamTCPSock = NULL;
	rtcpParser->m_streamingOverTCP = false;
	rtcpParser->m_rtcpPacketSize = ReadNetData(rtcpParser->m_rtcpStreamSock->GetMediaStreamSock(), rtcpParser->m_rtcpReceivePacket, MAX_RTCP_PACKET_SIZE);
	
	if(rtcpParser->m_rtcpPacketSize<=0)
	{
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtpStatus.txt","@@@@error the returned rtcp data is not great than 0\n");
		return -108;
	}


	if((CUserOptions::UserOptions.m_bMakeLog>=LOG_DATA))
	{
		CLog::EnablePrintTime(false);
		int rtcpPacketSize = rtcpParser->m_rtcpPacketSize;
		if(rtcpPacketSize <= 0)
			return 0;

		unsigned char flag = 0;
		if(rtcpParser->m_mediaStream->IsVideoStream())
		{
			flag = LOCAL_VIDEO_RTCP;
		}
		else if(rtcpParser->m_mediaStream->IsAudioStream())
		{
			flag = LOCAL_AUDIO_RTCP;
		}

		CLog::Log.MakeLog(LL_AUDIO_DATA,"RTSPStream.data", &flag, 1);
		CLog::Log.MakeLog(LL_AUDIO_DATA,"RTSPStream.data", (unsigned char *)&rtcpPacketSize, sizeof(int));
		CLog::Log.MakeLog(LL_AUDIO_DATA,"RTSPStream.data", rtcpParser->m_rtcpReceivePacket, rtcpPacketSize);
	}

	
#if 0 //Jason _TEST_OCC_TIME
	COCCTimeInfo * pOCCTimeInfo = COCCTimeInfo::CreateInstance();
	if(pOCCTimeInfo->nOCCStartTime != 0 && pOCCTimeInfo->nOCCStopTime == 0)
		return 0;
	
	if(mediaStream->IsVideoStream())
	{
		if(pOCCTimeInfo->nOCCStopTime != 0 && pOCCTimeInfo->nOCCFirstVideoRTPReceivedTime == 0)
		{
			return 0;
		}
	}
	else if(mediaStream->IsAudioStream())
	{
		if(pOCCTimeInfo->nOCCStopTime != 0 && pOCCTimeInfo->nOCCFirstAudioRTPReceivedTime == 0)
		{
			return 0;
		}
	}
#endif //_TEST_OCC_TIME
	
	//SLOG2(LL_RTP_ERR,"netPoll.txt","rtcp_sock=%d,size=%d\n",rtcpParser->m_rtcpStreamSock->GetMediaStreamSock()->GetSockID(),rtcpParser->m_rtcpPacketSize);
	int result = rtcpParser->ParseRTCPPacket();
	if(result>=0)
		return PACKET_RTCP;
	
	return 0;
}

int CRTCPParser::ReadRTCPStreamOverTCP(PIVOSocket streamSock, int streamDataSize)
{
	m_streamTCPSock = streamSock;
	m_streamingOverTCP = true;
	m_rtcpPacketSize = ReadNetDataExact(streamSock, m_rtcpReceivePacket, streamDataSize);
	return ParseRTCPPacket();
}

int CRTCPParser::ReadRTCPStreamFromLocal(unsigned char * streamData, int streamDataSize)
{
	memcpy(m_rtcpReceivePacket, streamData, streamDataSize);
	m_rtcpPacketSize = streamDataSize;
	return ParseRTCPPacket();
}

#define JUMP(n) { rtcpPacket += (n); rtcpPacketSize -= (n); }

int CRTCPParser::ParseRTCPPacket()
{
	int typeOfPacket = PACKET_UNKNOWN_TYPE;
	do
	{
		unsigned char * rtcpPacket = m_rtcpReceivePacket;
		int rtcpPacketSize = m_rtcpPacketSize;
		if(rtcpPacketSize < 4)
		{
			CLog::Log.MakeLog(LL_RTP_ERR,"rtcpStatus.txt","@@@@error:the rtcp packet size is too small\n");
			break;
		}

		int totalPacketSize = IP_UDP_HEADER_SIZE + rtcpPacketSize;
		unsigned int rtcpHeader = ::NetToHostLong(*(unsigned int *)rtcpPacket);
		if((rtcpHeader & 0xE0FE0000) != (0x80000000 | (RTCP_PACKET_SR << 16)))
		{
			CLog::Log.MakeLog(LL_RTP_ERR,"rtcpStatus.txt","@@@@error to parse the rtcp packet head\n");
			break;
		}

		unsigned int senderSSRC = 0;
		while(1)
		{
			unsigned int rtcpHeader_RC = (rtcpHeader >> 24) & 0x1F;
			unsigned int rtcpHeader_PT = (rtcpHeader >> 16) & 0xFF;
			unsigned int rtcpHeader_length = (rtcpHeader & 0xFFFF) * 4;
			JUMP(4)
			if(rtcpHeader_length > (unsigned int)rtcpPacketSize)
			{
				SLOG1(LL_SOCKET_ERR,"netPoll.txt","rtcpHeader_length is too big=%d\n",rtcpHeader_length);
				break;
			}
			switch(rtcpHeader_PT)
			{
			case RTCP_PACKET_SR:
				{
					if(rtcpHeader_length < 4)
						break;
					rtcpHeader_length -= 4;

					senderSSRC = ::NetToHostLong(*(unsigned int *)rtcpPacket); 
					JUMP(4)

					if(rtcpHeader_length < 20)
						break; 
					rtcpHeader_length -= 20;

					unsigned int ntpTimestampMSW = ::NetToHostLong(*(unsigned int *)rtcpPacket); 
					JUMP(4)
					unsigned int ntpTimestampLSW = ::NetToHostLong(*(unsigned int *)rtcpPacket); 
					JUMP(4)
					unsigned int rtpTimestamp = ::NetToHostLong(*(unsigned int *)rtcpPacket);
					JUMP(4)
					
					//if(ntpTimestampMSW == 0 || ntpTimestampLSW == 0 || rtpTimestamp == 0)
					//	break;
					
					//++m_nSenderReportNum;
					//if(m_nSenderReportNum < 3)//TBD: why? who made the logic?
					//	break;

					
					CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
					pRTSPMediaStreamSyncEngine->SyncMediaStream(m_mediaStream, ntpTimestampMSW, ntpTimestampLSW, rtpTimestamp);

					m_mediaStream->RTPParser()->RTPSenderTable()->NotifyReceivedSRPacketFromSender(senderSSRC, ntpTimestampMSW, ntpTimestampLSW, rtpTimestamp);
					JUMP(8)
				}
			case RTCP_PACKET_RR:
				{
					unsigned int reportBlocksSize = rtcpHeader_RC * (6 * 4);
					if(rtcpHeader_length < reportBlocksSize)
						break;
					rtcpHeader_length -= reportBlocksSize;

					JUMP(reportBlocksSize)

					break;
				}
			case RTCP_PACKET_SDES:
				{
					for(unsigned int i=0; i<rtcpHeader_RC; ++i)
					{
						if(rtcpHeader_length < 4)
							break;
						rtcpHeader_length -= 4;

						senderSSRC = ::NetToHostLong(*(unsigned int *)rtcpPacket); 
						JUMP(4)

						CSDESItem SDESItem(rtcpPacket);
						unsigned int SDESItemSize = SDESItem.TotalSize();
						while((SDESItemSize % 4) > 0) 
							++SDESItemSize;


						rtcpHeader_length -= SDESItemSize;
						JUMP(SDESItemSize)
					}
					break;
				}
			case RTCP_PACKET_BYE:
				{
					unsigned int ssrc_csrc_identifiers_size = rtcpHeader_RC * 4;
					if(rtcpHeader_length < ssrc_csrc_identifiers_size)
						break;
					rtcpHeader_length -= ssrc_csrc_identifiers_size;

					JUMP(ssrc_csrc_identifiers_size)

					break;
				}
			default:

				break;
			}

			JUMP(rtcpHeader_length)

			if(rtcpPacketSize < 4)
			{
				break;
			}

			rtcpHeader = ::NetToHostLong(*(unsigned int *)rtcpPacket);
			if((rtcpHeader & 0xC0000000) != 0x80000000) 
			{
				break;
			}
		}

		OnReceive(0, totalPacketSize, senderSSRC);
#if TRACE_CRASH
		CLog::Log.MakeLog(LL_RTP_ERR,"readRTCP.txt","!!!leave RTCP parser\n");
#endif//
	}while(0);

	return 0;
}

unsigned int CRTCPParser::NumMembers()
{
	return m_knownMembers->NumMembers();
}

void CRTCPParser::OnReceive(int typeOfPacket, int totalPacketSize, unsigned int SSRC)
{
	m_typeOfPacket = typeOfPacket;
	m_lastReceivedSize = totalPacketSize;
	m_lastReceivedSSRC = SSRC;
}

void CRTCPParser::SendReport() 
{
	CRTPSenderTable * rtpSenderTable = m_mediaStream->RTPParser()->RTPSenderTable();
	CRTPSenderStatus * senders = rtpSenderTable->GetSenderStatus();
	if(senders == NULL)
		return;

	AddReport();

	AddSDES();

#if SUPPORT_ADAPTATION
	//if(++m_NADUCount==m_mediaStream->GetReportFreqOf3gppAdaptation())
	{
		AddNADU_APP();
		m_NADUCount=0;
	}
	//CRTPSenderTable * rtpSenderTable = m_mediaStream->RTPParser()->RTPSenderTable();
	rtpSenderTable->Reset();
#endif//SUPPORT_ADAPTATION

	SendReportPacket();

#if ENABLE_LOG
 //if (CDumper::GetMask() & LOG_RTCP_REPORT)
 // VOLOGI("Send RTCP Receiver report at %lu", voOS_GetSysTime());
#endif //ENABLE_LOG

	const unsigned int membershipReapPeriod = 5;
	if((++m_outgoingReportCount) % membershipReapPeriod == 0)
	{
		unsigned int threshold = m_outgoingReportCount - membershipReapPeriod;
		m_knownMembers->ReapOldMembers(threshold);
	}
}

void CRTCPParser::SendBYE()
{
	AddReport();
	AddBYE();

	SendReportPacket();
}

void CRTCPParser::SendReportPacket() 
{
	if(!m_streamingOverTCP)
		SendReportPacketOverUDP();
	else
		SendReportPacketOverTCP();
}

void CRTCPParser::SendReportPacketOverUDP()
{
	struct voSocketAddress   rtcpServerAddr;
	memset(&rtcpServerAddr, 0, sizeof(voSocketAddress));
	memcpy(&rtcpServerAddr, m_mediaStream->MediaAddr(), sizeof(voSocketAddress));

	rtcpServerAddr.port = HostToNetShort(m_mediaStream->ServerRTCPPort());

	unsigned int reportPacketSize = m_rtcpReportPacket->PacketDataSize();
	WriteNetData(m_rtcpStreamSock->GetMediaStreamSock(), &rtcpServerAddr, m_rtcpReportPacket->PacketData(), reportPacketSize);
	m_rtcpReportPacket->Reset();

	m_lastSentSize = IP_UDP_HEADER_SIZE + reportPacketSize;
	m_haveJustSentPacket = true;
	m_lastPacketSentSize = reportPacketSize;
}

void CRTCPParser::SendReportPacketOverTCP()
{
	do
	{
		if(m_streamTCPSock == NULL)
			break;

		const char dollar = '$';
		if(::SendTCPData(m_streamTCPSock, &dollar, 1, 0) != 1) 
		{
			break;
		}

		unsigned char streamChannelID = m_mediaStream->RTCPChannelID();
		if(::SendTCPData(m_streamTCPSock, (char *)&streamChannelID, 1, 0) != 1)
			break;

		unsigned int reportPacketSize = m_rtcpReportPacket->PacketDataSize();
		char reportPacketSizeNetOrder[2];
		reportPacketSizeNetOrder[0] = (char)((reportPacketSize & 0xFF00) >> 8 );
		reportPacketSizeNetOrder[1] = (char)(reportPacketSize & 0xFF);
		if(::SendTCPData(m_streamTCPSock, reportPacketSizeNetOrder, 2, 0) != 2) 
			break;

		if(::SendTCPData(m_streamTCPSock, (char *)m_rtcpReportPacket->PacketData(), reportPacketSize, 0) != (int)reportPacketSize)
		{
			CLog::Log.MakeLog(LL_RTP_ERR,"rtcpStatus.txt","@@@@error to send the rtcp packet by TCP\n");
			break;
		}

		m_rtcpReportPacket->Reset();
		m_lastSentSize = IP_UDP_HEADER_SIZE + reportPacketSize;
		m_haveJustSentPacket = true;
		m_lastPacketSentSize = reportPacketSize;

	} while (0);
}

int CRTCPParser::CheckNewSSRC()
{
	return m_knownMembers->NoteMembership(m_lastReceivedSSRC, m_outgoingReportCount);
}

void CRTCPParser::RemoveLastReceivedSSRC()
{
	RemoveSSRC(m_lastReceivedSSRC, false);
}

void CRTCPParser::RemoveSSRC(unsigned int SSRC, bool alsoRemoveStatus) 
{
	m_knownMembers->Remove(SSRC);

	if(alsoRemoveStatus) 
	{
		m_mediaStream->RTPParser()->RTPSenderTable()->RemoveRecord(SSRC);
	}
}

void CRTCPParser::OnExpire(CRTCPParser * rtcpParser) 
{
	rtcpParser->DoExpire();
}

void CRTCPParser::AddReport()
{
	AddRR();
}

void CRTCPParser::AddSR()
{

}

void CRTCPParser::AddRR() 
{
	PackCommonReportPrefix(RTCP_PACKET_RR, m_mediaStream->RTPParser()->SSRC());
	PackCommonReportSuffix();
}

void CRTCPParser::PackCommonReportPrefix(unsigned char packetType, unsigned int SSRC, unsigned int numExtraWords) 
{
	unsigned int numReportingSources = 0;
	numReportingSources = m_mediaStream->RTPParser()->RTPSenderTable()->ActiveSenderCountSinceLastReset();

	if(numReportingSources >= 32) 
	{ 
		numReportingSources = 32; 
	}

	unsigned int rtcpHeader = 0x80000000;
	rtcpHeader |= (numReportingSources << 24);
	rtcpHeader |= (packetType << 16);
	rtcpHeader |= (1 + numExtraWords + 6*numReportingSources);
	
	m_rtcpReportPacket->PackWord(rtcpHeader);
	m_rtcpReportPacket->PackWord(SSRC);
}

void CRTCPParser::PackCommonReportSuffix()
{
	CRTPSenderTable * rtpSenderTable = m_mediaStream->RTPParser()->RTPSenderTable();
	CRTPSenderStatus* senders=rtpSenderTable->GetSenderStatus();
	if(senders)
		PackReportBlock(senders);

}

void CRTCPParser::PackReportBlock(CRTPSenderStatus * senderStatus)
{
	m_rtcpReportPacket->PackWord(senderStatus->SSRC());

	unsigned int highestExtSeqNumReceived = senderStatus->HighestExtSeqNumReceived();
	unsigned int totalNumExpected = highestExtSeqNumReceived - senderStatus->BaseExtSeqNumReceived();
	int totalNumLost;
#if 0
	totalNumLost=totalNumExpected - senderStatus->TotalPacketsReceived();
	// totalNumLost number is a 24-bit signed value:
	if(totalNumLost > 0x007FFFFF)
	{
		totalNumLost = 0x007FFFFF;
	} 
	else if(totalNumLost < 0) 
	{
		if(totalNumLost < -0x00800000) 
			totalNumLost = 0x00800000;

		totalNumLost &= 0x00FFFFFF;
		if(totalNumLost==0x00FFFFFF)
			totalNumLost=0;
	}
#endif
	unsigned int numExpectedSinceLastReset = highestExtSeqNumReceived - senderStatus->LastResetExtSeqNumReceived();
	int errPackNum = senderStatus->GetPacketErrNum();
	int numLostSinceLastReset = numExpectedSinceLastReset - senderStatus->PacketsReceivedSinceLastReset()+errPackNum; 
	if(numLostSinceLastReset<0)
		numLostSinceLastReset=0;
	senderStatus->UpdatePacketsLost(numLostSinceLastReset);
	unsigned char lossFraction;
	if(numExpectedSinceLastReset == 0 || numLostSinceLastReset < 0) 
	{
		lossFraction = 0;
	} 
	else 
	{
		lossFraction = (unsigned char)((numLostSinceLastReset << 8) / numExpectedSinceLastReset);
	}
	totalNumLost=senderStatus->TotalPacketsLost();
	if(CUserOptions::UserOptions.m_bMakeLog==2)
	{
		sprintf(CLog::formatString,"fraction=%d(%d/%d),tatolLost=%d,err=%d,Seq(%d-%d)jitter=%d\n",
			lossFraction,numLostSinceLastReset,numExpectedSinceLastReset,totalNumLost,errPackNum,
			senderStatus->HighestExtSeqNumReceived(),
			senderStatus->BaseExtSeqNumReceived(),
			senderStatus->Jitter());
		if(m_mediaStream->IsVideoStream())
		{
			CLog::Log.MakeLog(LL_RTP_ERR,"RTCPvideo.txt",CLog::formatString);
		}
		else
		{
			CLog::Log.MakeLog(LL_RTP_ERR,"RTCPaudio.txt",CLog::formatString);
		}
	}
	m_rtcpReportPacket->PackWord((lossFraction << 24) | totalNumLost);
	m_rtcpReportPacket->PackWord(highestExtSeqNumReceived);

	m_rtcpReportPacket->PackWord(senderStatus->Jitter());

	unsigned int NTPmsw = senderStatus->LastReceivedSR_NTPmsw();
	unsigned int NTPlsw = senderStatus->LastReceivedSR_NTPlsw();
	unsigned int LSR = ((NTPmsw & 0xFFFF) << 16) | (NTPlsw >> 16);
	m_rtcpReportPacket->PackWord(LSR);

	struct timeval & LSRtime = senderStatus->LastReceivedSR_time();
	struct timeval timeNow, timeSinceLSR;
	gettimeofday(&timeNow);
	if(timeNow.tv_usec < LSRtime.tv_usec)
	{
		timeNow.tv_usec += 1000000;
		timeNow.tv_sec -= 1;
	}

	timeSinceLSR.tv_sec = timeNow.tv_sec - LSRtime.tv_sec;
	timeSinceLSR.tv_usec = timeNow.tv_usec - LSRtime.tv_usec;

	unsigned int DLSR;
	if(LSR == 0) 
	{
		DLSR = 0;
	} 
	else 
	{
		DLSR = (timeSinceLSR.tv_sec << 16) | ((((timeSinceLSR.tv_usec << 11) + 15625) / 31250) & 0xFFFF);
	}

#if 0//TRACE_RR//defined(R_TEST)||defined(WMP_PLUGIN)
	if(CUserOptions::UserOptions.m_bMakeLog==2)
	{
		sprintf(CLog::formatString,"LSRT=(%d,%d),CURT=(%d,%d),diff=(%d,%d),DSLR=%d(%f)\n",
			LSRtime.tv_sec,LSRtime.tv_usec,
			timeNow.tv_sec,timeNow.tv_usec,
			timeSinceLSR.tv_sec,timeSinceLSR.tv_usec,
			DLSR,
			DLSR*1.0/65536);
		if(m_mediaStream->IsVideoStream())
		{
			CLog::Log.MakeLog(LL_RTP_ERR,"RTCPvideo.txt",CLog::formatString);
		}
		else
		{
			CLog::Log.MakeLog(LL_RTP_ERR,"RTCPaudio.txt",CLog::formatString);
		}
	}
#endif//WMP_PLUING
	m_rtcpReportPacket->PackWord(DLSR);
}

#if SUPPORT_ADAPTATION
void CRTCPParser::AddNADU_APP()
{
//#define PSS0 0x50535330
//#define NUM_OF_NADU_BLOCK 1
//	if(CUserOptions::UserOptions.m_n3GPPAdaptive==0)
//		return;
//	CRTPSenderTable * rtpSenderTable = m_mediaStream->RTPParser()->RTPSenderTable();
//	CRTPSenderStatus* senders=rtpSenderTable->GetSenderStatus();
//	unsigned long ssrc=0;
//	
//	if(senders)
//	{
//		ssrc=senders->SSRC();
//	}
//		
//	
//	if(ssrc==0)//there is no sender report now
//		return;
//
//	unsigned int num4ByteWords = 2+3*NUM_OF_NADU_BLOCK;
//	unsigned int rtcpHeader = 0x80000000;
//	rtcpHeader |= (RTCP_PACKET_APP << 16);
//	rtcpHeader |= num4ByteWords;
//
//	m_rtcpReportPacket->PackWord(rtcpHeader);
//	m_rtcpReportPacket->PackWord(m_mediaStream->RTPParser()->SSRC());	
//
//	m_rtcpReportPacket->PackWord(PSS0);
//	m_rtcpReportPacket->PackWord(ssrc);
//	unsigned int playOutAndNsn;
//	/*
//	unsigned int playOutAndNsn = m_mediaStream->GetFirstFrameSeqNumInBuffer();
//	if(playOutAndNsn==0)
//	{
//		playOutAndNsn = 1;//(rtpSenderStatus->HighestExtSeqNumReceived()&0x0000ffff)+1;
//	}*/
//	playOutAndNsn=0xffff0000;
//	m_rtcpReportPacket->PackWord(playOutAndNsn);//
//	unsigned int FreeBufSpace = CUserOptions::UserOptions.m_nMediaBuffTime*CUserOptions::UserOptions.outInfo.clip_bitrate*2;//1024/512,
//	FreeBufSpace&=0x0000ffff;
//	m_rtcpReportPacket->PackWord(FreeBufSpace);
//#ifndef NDEBUG
//	sprintf(CLog::formatString,"ssrc=%u,playOutAndNsn=%d\n",ssrc,playOutAndNsn&0x0000ffff);
//	CLog::Log.MakeLog(LL_RTP_ERR,"testAdaptation.txt",CLog::formatString);
//#endif//
}
#endif//SUPPORT_ADAPTATION

void CRTCPParser::AddSDES() 
{
	unsigned int numBytes = 4;
	numBytes += m_CNAME->TotalSize();
	numBytes += 1;

	unsigned int num4ByteWords = (numBytes + 3) / 4;
	unsigned int rtcpHeader = 0x81000000;
	rtcpHeader |= (RTCP_PACKET_SDES << 16);
	rtcpHeader |= num4ByteWords;

	m_rtcpReportPacket->PackWord(rtcpHeader);
	m_rtcpReportPacket->PackWord(m_mediaStream->RTPParser()->SSRC());	
	m_rtcpReportPacket->Pack(m_CNAME->SDESTotalItem(), m_CNAME->TotalSize());

	unsigned int numPaddingBytesNeeded = 4 - (m_rtcpReportPacket->PacketDataSize() % 4);
	unsigned char zero = '\0';
	while(numPaddingBytesNeeded-- > 0) 
		m_rtcpReportPacket->Pack(&zero, 1);
}

void CRTCPParser::AddBYE() 
{
	unsigned int rtcpHeader = 0x81000000;
	rtcpHeader |= (RTCP_PACKET_BYE << 16);
	rtcpHeader |= 1; 
	m_rtcpReportPacket->PackWord(rtcpHeader);
	m_rtcpReportPacket->PackWord(m_mediaStream->RTPParser()->SSRC());
}

void CRTCPParser::Schedule(long nextTime) 
{
	m_taskToken = CTaskSchedulerEngine::CreateTaskSchedulerEngine()->ScheduleTask(nextTime, (TaskFunction *)CRTCPParser::OnExpire, this,m_taskToken);
}

void CRTCPParser::Reschedule(long nextTime) 
{
	CTaskSchedulerEngine::CreateTaskSchedulerEngine()->UnscheduleTask(m_taskToken);
	Schedule(nextTime);
}

void CRTCPParser::Unschedule()
{
	CTaskSchedulerEngine::CreateTaskSchedulerEngine()->UnscheduleTask(m_taskToken);

}

void CRTCPParser::DoExpire()
{
	SendReport();
	Schedule(CUserOptions::UserOptions.m_nRTCPInterval);
}

