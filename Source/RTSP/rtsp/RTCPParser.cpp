#include "voLog.h"
#include "RTSPMediaStreamSyncEngine.h"
#include "MediaStream.h"
#include "MediaStreamSocket.h"
#include "RealTimeStreamingEngine.h"

#include "network.h"
#include "utility.h"
#include "TaskSchedulerEngine.h"

#include "HashTable.h"
#include "RTCPReportPacket.h"

#include "RTPParser.h"
#include "RTPSenderTable.h"
#include "RTCPParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

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
	struct votimeval nowTime;
	gettimeofday(&nowTime);
	return (double)(nowTime.tv_sec + nowTime.tv_usec / 1000000.0);
}

const int IP_UDP_HEADER_SIZE = 28;
const int MAX_RTCP_PACKET_SIZE = 1460;

#ifdef _VONAMESPACE
}
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

void		CRTCPParser::UpdateSocket(CMediaStreamSocket * socket)
{
	m_rtcpStreamSock=socket;
	//SLOG1(LL_RTP_ERR,"flow.txt","UpdateSocket_tcp2=%d\n",CUserOptions::UserOptions.m_useTCP)
	if(CUserOptions::UserOptions.m_useTCP<=0||CUserOptions::UserOptions.m_URLType==UT_SDP2_LOCAL)
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
, m_streamTCPSock(INVALID_SOCKET)
, m_taskToken(0)
#if SUPPORT_ADAPTATION
,m_NADUCount(0)
#endif//SUPPORT_ADAPTATION
{
	m_latencyAvg = 0;
	m_latency = 0;
	m_latencyCnt = 1;
	m_prevReportTime = m_nextReportTime = NowTime2Double();

	UpdateSocket(rtcpStreamSock);
	SLOG1(LL_RTP_ERR,"flow.txt","construct RTCPParser,%x\n",(unsigned int)this);

	m_rtcpPacketSize = 0;
	m_rtcpReceivePacket = new unsigned char [MAX_RTCP_PACKET_SIZE];TRACE_NEW("rtcpp_m_rtcpReceivePacket",m_rtcpReceivePacket);
	m_rtcpReportPacket = new CRTCPReportPacket(MAX_RTCP_PACKET_SIZE);TRACE_NEW("rtcpp_m_rtcpReportPacket",m_rtcpReportPacket);
	m_knownMembers = new CRTCPMemberTable(this);TRACE_NEW("rtcpp_m_knownMembers",m_knownMembers);

	//char CNAME[256];
	//memset(CNAME, 0, 256);
	//GetLocalHostName((char *)CNAME, 256);
	const char* CNAME="VisualOnStreaming";
	m_CNAME = new CSDESItem(RTCP_SDES_CNAME, (unsigned char *)CNAME);TRACE_NEW("rtcpp_m_CNAME",m_CNAME);
	memset(&m_sRReport,0,sizeof(VORTCPReceiverReport));
	memset(&m_sSReport,0,sizeof(VORTCPSenderReport));

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

 	rtcpParser->m_streamTCPSock = INVALID_SOCKET;
	rtcpParser->m_streamingOverTCP = false;
	rtcpParser->m_rtcpPacketSize = ReadNetData(rtcpParser->m_rtcpStreamSock->GetMediaStreamSock(), rtcpParser->m_rtcpReceivePacket, MAX_RTCP_PACKET_SIZE);
	if(rtcpParser->m_rtcpPacketSize<=0)
	{
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtpStatus.txt","@@@@error the returned rtcp data is not great than 0\n");
		return -108;
	}

	unsigned char flag = 0;
			
	int rtcpPacketSize = rtcpParser->m_rtcpPacketSize;
		if(rtcpPacketSize <= 0)
			return 0;

	if(rtcpParser->m_mediaStream->IsVideoStream())
	{
		flag = LOCAL_VIDEO_RTCP;
	}
	else if(rtcpParser->m_mediaStream->IsAudioStream())
	{
		flag = LOCAL_AUDIO_RTCP;
	}

	if(CUserOptions::UserOptions.m_pRTSPTransFunc)
	{
		VOLOGI("******************************ReadRTCPStream*********************************isTCP=%d, flag=%d,rtcpPacket=%x,\n",CUserOptions::UserOptions.m_useTCP,flag,rtcpParser->m_rtcpReceivePacket);
		CUserOptions::UserOptions.m_pRTSPTransFunc(CUserOptions::UserOptions.m_useTCP, flag,rtcpPacketSize,rtcpParser->m_rtcpReceivePacket, 0,0);
	}

	if((CUserOptions::UserOptions.m_bMakeLog==LOG_DATA))
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
#ifdef DIMS_DATA
		else if(rtcpParser->m_mediaStream->IsDataStream())
		{
			flag = LOCAL_DIMS_RTCP;
		}
#endif//DIMS_DATA
		CLog::Log.MakeLog(LL_AUDIO_DATA,"RTSPStream.data", &flag, 1);
		CLog::Log.MakeLog(LL_AUDIO_DATA,"RTSPStream.data", (unsigned char *)&rtcpPacketSize, sizeof(int));
		CLog::Log.MakeLog(LL_AUDIO_DATA,"RTSPStream.data", rtcpParser->m_rtcpReceivePacket, rtcpPacketSize);
	}

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

	unsigned char flag = 0;
	if(m_mediaStream->IsVideoStream())
		flag = LOCAL_VIDEO_RTCP;
	else if(m_mediaStream->IsAudioStream())
		flag = LOCAL_AUDIO_RTCP;
	if(CUserOptions::UserOptions.m_pRTSPTransFunc)
	{
		//CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "!!!!!!!!!!!!@@@@!!!!!!!!!!!!!!!!!ReadRTCPStreamFromLocal!!!!!!!!!!!!!!!!!!!!!!!@@@@@!!!!!!!!!!!!\n");
		VOLOGI("******************************ReadRTCPStreamFromLocal*********************************isTCP=%d, flag=%d,rtcpPacket=%x,\n",CUserOptions::UserOptions.m_useTCP,flag,m_rtcpReceivePacket);
		CUserOptions::UserOptions.m_pRTSPTransFunc(CUserOptions::UserOptions.m_useTCP,flag,m_rtcpPacketSize,m_rtcpReceivePacket,0,0);
	}

	return ParseRTCPPacket();
}

#define JUMP(n) { rtcpPacket += (n); rtcpPacketSize -= (n); }

int CRTCPParser::ParseRTCPPacket()
{
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
				break;

			switch(rtcpHeader_PT)
			{
			case RTCP_PACKET_SR:
				{
					memset(&m_sSReport,0,sizeof(VORTCPSenderReport));

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

					m_sSReport.packCnt = ::NetToHostLong(*(unsigned int *)rtcpPacket);
					JUMP(4)

					m_sSReport.octetCnt = ::NetToHostLong(*(unsigned int *)rtcpPacket);
					JUMP(4)

					m_sSReport.fractionLost= ::NetToHostLong(*(unsigned int *)rtcpPacket);
					JUMP(4)

					m_sSReport.cumulativeLost= ::NetToHostLong(*(unsigned int *)rtcpPacket);
					JUMP(4)

					m_sSReport.extendedHighestSeqReceived= ::NetToHostLong(*(unsigned int *)rtcpPacket);
					JUMP(4)

					m_sSReport.interarrivalJitter= ::NetToHostLong(*(unsigned int *)rtcpPacket);
					JUMP(4)

					m_sSReport.lastSR= ::NetToHostLong(*(unsigned int *)rtcpPacket);
					JUMP(4)

					m_sSReport.delayLastSR= ::NetToHostLong(*(unsigned int *)rtcpPacket);
					JUMP(4)

#if TRACE_FLOW1
	CLog::Log.MakeLog(LL_RTP_ERR,"flow2.txt","!!!before SyncMediaStream\n");
#endif//
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
					CLog::Log.MakeLog(LL_BUF_STATUS,"error.txt","!!!Recieved BYE packet,exit\n");
					
					CUserOptions::UserOptions.endOfStreamFromServer = E_CONNECTION_GOODBYE;
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
	CRTPSenderStatus* senders=rtpSenderTable->GetSenderStatus();
	if(senders==NULL)
		return;
	AddReport();

	AddSDES();
#if SUPPORT_ADAPTATION
	//if(++m_NADUCount==m_mediaStream->GetReportFreqOf3gppAdaptation())
	{
		AddNADU_APP();
		m_NADUCount=0;
		AddRFC3611Packet();
	}
	//CRTPSenderTable * rtpSenderTable = m_mediaStream->RTPParser()->RTPSenderTable();
	rtpSenderTable->Reset();
#endif//SUPPORT_ADAPTATION
	SendReportPacket();

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
	struct sockaddr_storage   rtcpServerAddr;
	memset(&rtcpServerAddr, 0, sizeof(sockaddr_storage));
	memcpy(&rtcpServerAddr, m_mediaStream->MediaAddr(), sizeof(sockaddr_storage));

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
		if(m_streamTCPSock == INVALID_SOCKET)
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
	memset(&m_sRReport, 0, sizeof(m_sRReport));
	
	PackCommonReportPrefix(RTCP_PACKET_RR, m_mediaStream->RTPParser()->SSRC());
	PackCommonReportSuffix();
	
	NotifyEvent(VO_EVENT_RTCP_RR, (int)&m_sRReport);
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
	m_sRReport.senderSSRC = SSRC;
}

void CRTCPParser::PackCommonReportSuffix()
{
	CRTPSenderTable * rtpSenderTable = m_mediaStream->RTPParser()->RTPSenderTable();
	CRTPSenderStatus* senders=rtpSenderTable->GetSenderStatus();
	if(senders)
		PackReportBlock(senders);
	
#if !SUPPORT_ADAPTATION//the code is moved to SendRR when adaptive
	rtpSenderTable->Reset();
#endif//
}

void CRTCPParser::PackReportBlock(CRTPSenderStatus * senderStatus)
{
	m_rtcpReportPacket->PackWord(senderStatus->SSRC());

	m_sRReport.sourceSSRC = senderStatus->SSRC();

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
	int numLostSinceLastReset = numExpectedSinceLastReset - senderStatus->PacketsReceivedSinceLastReset(); 
	if(numLostSinceLastReset<0)
		numLostSinceLastReset=0;
	senderStatus->UpdatePacketsLost(numLostSinceLastReset);
	unsigned char lossFraction;
	int lostPercent = 0;
	if(numExpectedSinceLastReset == 0 || numLostSinceLastReset < 0) 
	{
		lossFraction = 0;
	} 
	else 
	{
		lostPercent  = int(numLostSinceLastReset*100.0/float(numExpectedSinceLastReset));
		lossFraction = (unsigned char)((numLostSinceLastReset << 8) / numExpectedSinceLastReset);
	}
#define THREASH_SEEK_I 30
	if(m_mediaStream->IsVideoStream())
	{
		int frameInBuf = m_mediaStream->HaveBufferedMediaFrameCount();
		int lastFrameTime = m_mediaStream->GetLastTimeInBuffer();
		SLOG5(LL_RTP_ERR,"seek2Iframe.txt","@lostPercent = %d(%d/%d) lastTime=%d,frameNum=%d\n",lostPercent,numLostSinceLastReset,numExpectedSinceLastReset
			,lastFrameTime,frameInBuf);

#if 0//test
		CUserOptions::UserOptions.m_nSeekToNextIFlag++;
		SLOG0(LL_RTP_ERR,"seek2Iframe.txt","this is for test,remove it in release\n");
#else
		if(lostPercent>THREASH_SEEK_I||numExpectedSinceLastReset==0||
			(frameInBuf<3&&lastFrameTime>20000)//this is to handle the case that the video is received too slowly
			)
		{
			CUserOptions::UserOptions.m_nSeekToNextIFlag++;
		}
		else
		{
			CUserOptions::UserOptions.m_nSeekToNextIFlag--;
		}
		if(CUserOptions::UserOptions.m_nSeekToNextIFlag<0)
			CUserOptions::UserOptions.m_nSeekToNextIFlag = 0;
#endif
	}
	
	
	totalNumLost=senderStatus->TotalPacketsLost();
	
	if(CUserOptions::UserOptions.m_bMakeLog==2)
	{
		sprintf(CLog::formatString,"fraction=%d(%d/%d),tatolLost=%d,Seq(%d-%d)jitter=%d\n",
			lossFraction,numLostSinceLastReset,numExpectedSinceLastReset,totalNumLost,
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
	m_sRReport.fractionLost = lossFraction;
	m_sRReport.cumulativeLost = totalNumLost;

	m_rtcpReportPacket->PackWord(highestExtSeqNumReceived);
	m_sRReport.extendedHighestSeqReceived = highestExtSeqNumReceived;
	
	m_rtcpReportPacket->PackWord(senderStatus->Jitter());
	m_sRReport.interarrivalJitter = senderStatus->Jitter();

	unsigned int NTPmsw = senderStatus->LastReceivedSR_NTPmsw();
	unsigned int NTPlsw = senderStatus->LastReceivedSR_NTPlsw();
	unsigned int LSR = ((NTPmsw & 0xFFFF) << 16) | (NTPlsw >> 16);
	m_rtcpReportPacket->PackWord(LSR);
	m_sRReport.lastSR = LSR;

	struct votimeval & LSRtime = senderStatus->LastReceivedSR_time();
	struct votimeval timeNow, timeSinceLSR;
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
	m_rtcpReportPacket->PackWord(DLSR);
	m_sRReport.delayLastSR = DLSR;
	
	m_latency += DLSR*1000/65536;
	m_latencyAvg = m_latency/m_latencyCnt;
#if TRACE_RR//defined(R_TEST)||defined(WMP_PLUGIN)
	if(CUserOptions::UserOptions.m_bMakeLog==2)
	{
		sprintf(CLog::formatString,"LSRT=(%d,%d),CURT=(%d,%d),diff=(%d,%d),DSLR=%d(%f),latency(%u,%u,%u)\n",
			LSRtime.tv_sec,LSRtime.tv_usec,
			timeNow.tv_sec,timeNow.tv_usec,
			timeSinceLSR.tv_sec,timeSinceLSR.tv_usec,
			DLSR,
			DLSR*1.0/65536,m_latencyCnt,m_latency,m_latencyAvg);
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
	m_latencyCnt++;

}

#if SUPPORT_ADAPTATION
void CRTCPParser::AddRFC3611Packet()
{
	
#if ENABLE_RFC3611
	CRTPSenderTable * rtpSenderTable = m_mediaStream->RTPParser()->RTPSenderTable();
	CRTPSenderStatus* senders=rtpSenderTable->GetSenderStatus();
	if(CUserOptions::UserOptions.m_n3GPPAdaptive==0||senders->LastResetExtSeqNumReceived()==0)
		return;
	if(senders)
	{
		TRleReport* rle=senders->CreateRFC3611Report();
		unsigned long ssrc=senders->SSRC();
		int blockNum=rle->index;
		if(blockNum&1)
			blockNum+=1;
		unsigned int num4ByteWords = 2+2+blockNum/2;
		unsigned int rtcpHeader = 0x80000000;
		rtcpHeader |= (RTCP_PACKET_RFC3611 << 16);
		rtcpHeader |= num4ByteWords;
		m_rtcpReportPacket->PackWord(rtcpHeader);
		m_rtcpReportPacket->PackWord(m_mediaStream->RTPParser()->SSRC());	
#define BLOCK_TYPE_RLE 1
		unsigned int blockHead=0x01000000;
		blockHead|=2+blockNum/2;
		m_rtcpReportPacket->PackWord(blockHead);
		m_rtcpReportPacket->PackWord(ssrc);
		int firstSeq=senders->LastResetExtSeqNumReceived();
		if(firstSeq)
			firstSeq+=1;//
		firstSeq<<=16;
		unsigned int seqParams=(senders->HighestExtSeqNumReceived()+1)|(firstSeq);
		m_rtcpReportPacket->PackWord(seqParams);
		unsigned char* block=(unsigned char*)(&(rle->rle[0]));
		while (blockNum-->0)
		{
				m_rtcpReportPacket->Pack(block+1,1);
				m_rtcpReportPacket->Pack(block,1);
				block+=2;
		}
	}
#endif//ENABLE_RFC3611
}
void CRTCPParser::AddNADU_APP()
{
#define PSS0 0x50535330
#define NUM_OF_NADU_BLOCK 1
	if(CUserOptions::UserOptions.m_n3GPPAdaptive==0)
		return;
	CRTPSenderTable * rtpSenderTable = m_mediaStream->RTPParser()->RTPSenderTable();
	CRTPSenderStatus* senders=rtpSenderTable->GetSenderStatus();
	unsigned long ssrc=0;
	
	if(senders)
	{
		ssrc=senders->SSRC();
	}
		
	
	if(ssrc==0)//there is no sender report now
		return;

	unsigned int num4ByteWords = 2+3*NUM_OF_NADU_BLOCK;
	unsigned int rtcpHeader = 0x80000000;
	rtcpHeader |= (RTCP_PACKET_APP << 16);
	rtcpHeader |= num4ByteWords;

	m_rtcpReportPacket->PackWord(rtcpHeader);
	m_rtcpReportPacket->PackWord(m_mediaStream->RTPParser()->SSRC());	

	m_rtcpReportPacket->PackWord(PSS0);
	m_rtcpReportPacket->PackWord(ssrc);
	unsigned int playOutAndNsn;
	/*
	unsigned int playOutAndNsn = m_mediaStream->GetFirstFrameSeqNumInBuffer();
	if(playOutAndNsn==0)
	{
		playOutAndNsn = 1;//(rtpSenderStatus->HighestExtSeqNumReceived()&0x0000ffff)+1;
	}*/
	playOutAndNsn=0xffff0000;
	if(m_mediaStream->IsVideoStream())
	{
		unsigned int NSN=m_mediaStream->GetFirstFrameSeqNumInBuffer();//(senders->HighestExtSeqNumReceived()&0x0000ffff)+1;
		playOutAndNsn=0x00140000|NSN;
	}
	m_rtcpReportPacket->PackWord(playOutAndNsn);//
	unsigned int FreeBufSpace; 
	int timeMS=CUserOptions::UserOptions.m_nMediaBuffTime*1000-m_mediaStream->HaveBufferedMediaStreamTime();
	if(timeMS>0)
	{
		FreeBufSpace=timeMS*4;
		if(FreeBufSpace>0x0000ffff)
			FreeBufSpace=0x00005050;
	}
	else
	{
		FreeBufSpace = CUserOptions::UserOptions.m_nMediaBuffTime*CUserOptions::UserOptions.outInfo.clip_bitrate*4;
	}
	FreeBufSpace&=0x0000ffff;
	m_rtcpReportPacket->PackWord(FreeBufSpace);
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


