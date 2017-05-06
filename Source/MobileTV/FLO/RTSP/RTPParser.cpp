
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "RTSPMediaStreamSyncEngine.h"
#include "MediaStream.h"
#include "utility.h"
#include "MediaStreamSocket.h"
#include "RealTimeStreamingEngine.h"

#include "RTPPacket.h"
#include "RTPPacketReorder.h"
#include "RTPSenderTable.h"

#include "RTPParser.h"
#include "DxManager.h"
#define LOG_TAG "FLOEngine_RTPParser"
#include "voLog.h"

#include "mylog.h"


const int BUFFER_RESERVE_HEAD = 1024;

#define MAX_STRORED_PACKETS 10//10
#define INVALID_SSRC  0
void CRTPParser::UpdateSocket(CMediaStreamSocket * socket)
{
	m_rtpStreamSock=socket;
	if(CUserOptions::UserOptions.m_useTCP<=0)
	{
		int flag;
		if(m_mediaStream->IsVideoStream())
			flag = VOS_STREAM_VIDEO;
		else if(m_mediaStream->IsAudioStream())
			flag = VOS_STREAM_AUDIO;
		else
			flag = VOS_STREAM_ALL;
		CRealTimeStreamingEngine * streamingEngine = CRealTimeStreamingEngine::CreateRealTimeStreamingEngine();
		streamingEngine->AddStreamReader(m_rtpStreamSock->GetMediaStreamSock(), ReadRTPStream, this,flag);
	}
}

CRTPParser::CRTPParser(CMediaStream * mediaStream, CMediaStreamSocket * rtpStreamSock)
: m_mediaStream(mediaStream)
, m_rtpStreamSock(rtpStreamSock)
, m_frameData(NULL)
, m_frameSize(0)
, m_curRtpSeqNum(0)
, m_lastReceivedSSRC(0)
, m_packetLossInMultiPacketFrame(true)
, m_firstPacketInMultiPacketFrame(true)
, m_lastPacketInMultiPacketFrame(true)
, m_seqNum(0)
,m_recvPackNum(0)
,m_firstChanSeqNum(-1)

{
	m_channelBeginTime = voOS_GetSysTime();
	UpdateSocket(rtpStreamSock);

	m_rtpPacketReorder = new CRTPPacketReorder();TRACE_NEW("rtpp_m_rtpPacketReorder",m_rtpPacketReorder);
	m_frameData = new unsigned char[MAX_FRAME_SIZE]+BUFFER_RESERVE_HEAD;TRACE_NEW("rtpp_m_m_frameData",m_frameData-BUFFER_RESERVE_HEAD);//reserved the first BUFFER_RESERVE_HEAD bytes

	m_rtpSenderTable = new CRTPSenderTable();TRACE_NEW("rtpp_m_rtpSenderTable",m_rtpSenderTable);
	m_SSRC = Random32();
	m_lastRTPTimestamp = 0;
	m_lastRTPPacketReceivedTime.tv_sec = 0;
	m_lastRTPPacketReceivedTime.tv_usec = 0;
	memset(&m_frame,0,sizeof(FrameData));
	
	m_SSRCChannelID = INVALID_SSRC;
	m_prevSSRCChannelID = INVALID_SSRC;
#if _DUMP_AV_RTP
	m_hVideoRTP = NULL;
	m_hAudioRTP = NULL;
#endif //_DUMP_AV_RTP	
}
void CRTPParser::	Reset()
{
	m_lastPacketInMultiPacketFrame = true;
	m_firstPacketInMultiPacketFrame = true;
	m_frameSize = 0;
}
bool CRTPParser::	FastChannelChange()
{
	m_recvPackNum = 0;
	m_channelBeginTime = voOS_GetSysTime();
	m_firstChanSeqNum = -1;
	m_lastRTPTimestamp= 0;
	Reset();
	if(m_rtpSenderTable)
	{
		m_rtpSenderTable->DeleteCurStatus();
		m_SSRC = Random32();
		m_lastRTPTimestamp = 0;
		m_lastRTPPacketReceivedTime.tv_sec = 0;
		m_lastRTPPacketReceivedTime.tv_usec = 0;
		memset(&m_frame,0,sizeof(FrameData));
		if(CUserOptions::UserOptions.m_bMakeLog)
		{
			if(m_mediaStream->IsVideoStream())
			{
				SLOG2(LL_RTP_ERR,"videoRTP.txt","changeCH:curSSRC=%d,prev=%d\n",m_SSRCChannelID,m_prevSSRCChannelID);
			}
			else
			{
				SLOG2(LL_RTP_ERR,"audioRTP.txt","changeCH:curSSRC=%d,prev=%d\n",m_SSRCChannelID,m_prevSSRCChannelID);

			}
		}
		//if one channel has the same SSRC with previous one,then m_SSRCChannelID will not be updated, and keep INVALID_SSRC
		//in this case, we do not update it
		if(INVALID_SSRC!=m_SSRCChannelID)
		{
			m_prevSSRCChannelID = m_SSRCChannelID;
			m_SSRCChannelID = INVALID_SSRC;

		}
	
	}
	return true;
}
CRTPParser::~CRTPParser()
{
	CRealTimeStreamingEngine * streamingEngine = CRealTimeStreamingEngine::CreateRealTimeStreamingEngine();
	if(m_rtpStreamSock)
	{
		streamingEngine->RemoveStreamReader(m_rtpStreamSock->GetMediaStreamSock());
		SAFE_DELETE(m_rtpStreamSock);
		CLog::Log.MakeLog(LL_RTP_ERR,"flow.txt","release one rtp socket\n");
	}
	SAFE_DELETE(m_rtpPacketReorder);
	m_frameData-=BUFFER_RESERVE_HEAD;
	SAFE_DELETE_ARRAY(m_frameData);
	SAFE_DELETE(m_rtpSenderTable);
	
#if _DUMP_AV_RTP
	if(m_hVideoRTP != NULL)
	{
		fclose(m_hVideoRTP);
		m_hVideoRTP = NULL;
	}
	if(m_hAudioRTP != NULL)
	{
		fclose(m_hAudioRTP);
		m_hAudioRTP = NULL;
	}
#endif //_DUMP_AV_RTP	
}
bool CRTPParser::	IsValidPackBySSRC(unsigned int ssrc,int seq)
{
#ifdef WIN32
	return true;
#endif
	bool result = true;
	m_recvPackNum++;
	if(m_SSRCChannelID==INVALID_SSRC)//waiting for new channel
	{
		
		if(m_prevSSRCChannelID==ssrc||ssrc>m_prevSSRCChannelID+10)
		{
			result = false;
		}
		else//this is new
			m_SSRCChannelID = ssrc;
		
	}
	else
	{
		result = ssrc>=m_SSRCChannelID&&ssrc<=m_SSRCChannelID+10;
	}
	if(result==false&&CUserOptions::UserOptions.m_bMakeLog)
	{
		if(m_mediaStream->IsVideoStream())
		{
			SLOG5(LL_RTP_ERR,"videoRTP.txt", "%d,InvalidPackBySSRC:ssrc=%d,(cur=%d,prev=%d)seq=%d\n",m_recvPackNum,ssrc,m_SSRCChannelID,m_prevSSRCChannelID,seq);
		}
		else
		{
			SLOG5(LL_RTP_ERR,"audioRTP.txt", "%d,InvalidPackBySSRC:ssrc=%d,(cur=%d,prev=%d)seq=%d\n",m_recvPackNum,ssrc,m_SSRCChannelID,m_prevSSRCChannelID,seq);

		}
		VOLOGE("InvalidPackBySSRC:ssrc=%d,(cur=%d,prev=%d)seq=%d\n",ssrc,m_SSRCChannelID,m_prevSSRCChannelID,seq);
	}
	
	if(result)
	{
		if(m_firstChanSeqNum==-1)
		{
#if ENABLE_DX
			TheDxManager.DumpContext("Dx:Get the first New Channel Data");
#endif ENABLE_DX	
			VOLOGI("ChannelSwitchTime=%u,curChan=%d,firstSeq=%d",voOS_GetSysTime()-m_channelBeginTime,ssrc,seq);
			m_firstChanSeqNum = seq;
		}
		
	}

	return result;
}
bool CRTPParser::IsValidPackBySeqNum(int seqNum)
{
	if(seqNum>65500)
		m_firstChanSeqNum = 0;//wrap around
	bool result = seqNum >= m_firstChanSeqNum;
	if(!result)
		SLOG2(LL_SOCKET_ERR,"rtpStatus.txt", "@@@@error InvalidPackBySeqNum(%d,%d)\n",seqNum,m_firstChanSeqNum);
	return result;
}

int CRTPParser::ReadRTPStream(void * param)
{
	CRTPParser   * rtpParser = (CRTPParser *)param;
	CMediaStream * mediaStream = rtpParser->m_mediaStream;
	
	CRTPPacketReorder * rtpPacketReorder = rtpParser->m_rtpPacketReorder;
	CRTPPacket * rtpPacket = NULL;
	
	do
	{
		rtpPacket = rtpPacketReorder->GetEmptyRTPPacket();
		if(rtpPacket == NULL)
			break;
		
        //VOLOGI("Jason Trace");
		int recvBytes = rtpPacket->ReadRTPPacket(rtpParser->m_rtpStreamSock);
		if(recvBytes <= 0)
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtpStatus.txt","@@@@error the returned RTP data is not great than 0\n");
			VOLOGE("returned RTP data is not great than 0");
			break;
		}

		if(!rtpPacket->ParseRTPPacketHeader())
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtpStatus.txt", "@@@@error to parse the rtp packet head\n");
			VOLOGE("Error to parse the rtp packet head");
			break;
		}
		
		
#if 1 //_DUMP_AV_RTP
#if ENABLE_LOG
		{
            //VOLOGI("RTPPayloadType: %d", rtpPacket->RTPPayloadType());
			if (mediaStream->IsVideoStream())
			{
				TheVideoRTPLogger.Dump("%u,%d,%u,%u,%d,%d\n",  
						GetAppTick(), rtpPacket->RTPPayloadType(), rtpPacket->RTPSSRC(), rtpPacket->RTPTimestamp(), rtpPacket->RTPSeqNum(), rtpPacket->RTPPacketSize());
			}
			else if(mediaStream->IsAudioStream())
			{
				TheAudioRTPLogger.Dump("%u,%d,%u,%u,%d,%d\n",  
						GetAppTick(), rtpPacket->RTPPayloadType(), rtpPacket->RTPSSRC(), rtpPacket->RTPTimestamp(), rtpPacket->RTPSeqNum(), rtpPacket->RTPPacketSize());
			}
		}
#endif //ENABLE_LOG
#endif //_DUMP_AV_RTP
		
		if(rtpParser->IsValidPackBySSRC(rtpPacket->RTPSSRC(),rtpPacket->RTPSeqNum() ) == false)
		{
			break;
		}
	
		if(!rtpPacket->DecryptFromSRTP())
			break;
		
		if(rtpParser->IsValidPackBySeqNum(rtpPacket->RTPSeqNum()) == false)
		{
			//Seq is not used any more because we can use the SSRC instead
			//break;
		}
		rtpParser->UpdateRTCP(rtpPacket);
			

		rtpPacketReorder->ReorderRTPPacket(rtpPacket);
		
		if(rtpParser->IsReadyParse())
			rtpParser->ParseRTPPayload();
		return PACKET_RTP;

	}while(0);

	rtpPacketReorder->ReleaseRTPPacket(rtpPacket);
	return 0;
}
bool		CRTPParser::UpdateRTCP(CRTPPacket * rtpPacket)
{
	m_rtpSenderTable->NotifyReceivedRTPPacketFromSender(rtpPacket->RTPSSRC(), rtpPacket->RTPSeqNum(), rtpPacket->RTPTimestamp(), m_mediaStream->RTPTimestampFrequency(), rtpPacket->RTPPacketSize());
	return true;
}
void CRTPParser::RetrieveAllPack(int packetCounts2)
{
	int packetCounts=m_rtpPacketReorder->GetStoredPacket();
	if(packetCounts>0)
	{
#if ENABLE_LOG
		//TheRTSPLogger.Dump("RetrieveAllPack,%d" TXTLN, packetCounts);
#else //ENABLE_LOG
		sprintf(CLog::formatString,"RetrieveAllPack,%d\n",packetCounts);
		CLog::Log.MakeLog(LL_RTSP_ERR,"rtsp.txt",CLog::formatString);
#endif //ENABLE_LOG
	}

	while(packetCounts>0)
	{
		ParseRTPPayload(1);
		packetCounts--;
	}
}

void CRTPParser::ParseRTPPayload(int flag)
{
	CRTPPacket * rtpPacket = NULL;

        //VOLOGI("Jason Trace +++");
	do
	{
		bool packetLoss = true;
		rtpPacket = m_rtpPacketReorder->GetNextReorderedRTPPacket(packetLoss);
		if(packetLoss)
			m_mediaStream->SetPacketLoss(packetLoss);
		if(rtpPacket == NULL)
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"nullPacke.txt","rtpPacket == NULL\n");	
			break;
		}
		if(m_mediaStream->IsVideoStream())//it can avoid to pad the new frame to the previous frame
		{
			int currentTimestamp = rtpPacket->RTPTimestamp();
			if(m_lastRTPTimestamp==0)
				m_lastRTPTimestamp = currentTimestamp;
			if(m_lastRTPTimestamp != currentTimestamp&&m_frameSize)//it is absolutely that pack lost
			{
				Reset();
				SLOG3(LL_SOCKET_ERR,"packet_loss.txt","last pack of prev frame lost(ts=%u),cur=(%d,ts=%u)\n",m_lastRTPTimestamp,currentTimestamp,rtpPacket->RTPSeqNum());
			}
			m_lastRTPTimestamp = currentTimestamp;
		}

		 if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
		 {
			int seqNum=rtpPacket->RTPSeqNum();
			sprintf(CLog::formatString,"ssrc=%d,seq=%d,ts=%d,size=%d,mark=%d\n",rtpPacket->RTPSSRC(),seqNum,rtpPacket->RTPTimestamp(),rtpPacket->RTPDataSize(),rtpPacket->RTPMarker());
			if(m_mediaStream->IsVideoStream())
			{
				CLog::Log.MakeLog(LL_RTP_ERR,"videoRTP.txt",CLog::formatString);	
			}
			else if (m_mediaStream->IsAudioStream())
			{
				CLog::Log.MakeLog(LL_RTP_ERR,"audioRTP.txt",CLog::formatString);	
			}
		 }

		if(!ParseRTPPayloadHeader(rtpPacket))
		{
			Reset();
			CLog::Log.MakeLog(LL_SOCKET_ERR,"packet_loss.txt","@@@@fail to parse the Payload  head\n");
			break;
		}
		
#define MAX_AU_IN_ONE_PACKET 200

		int count=0;//it is impossible that a packet has over 200 AU
		do
		{
            //VOLOGE("To ParseRTPPayloadFrame");
			if(!ParseRTPPayloadFrame(rtpPacket))		
            {
                //VOLOGE("ParseRTPPayloadFrame failed");
				break;
            }

			if(m_lastPacketInMultiPacketFrame||rtpPacket->RTPMarker())
			{
				RenderFrame(rtpPacket->RTPTimestamp()+GetRelativeTS(),count,rtpPacket->RTPSeqNum(),IsKeyFrame());
				//voOS_Sleep(1);
			}

		}while(++count<MAX_AU_IN_ONE_PACKET);


		return;

	}while(0);

	return;
}

bool CRTPParser::ParseRTPPayloadHeader(CRTPPacket * rtpPacket)
{
	return true;
}

bool CRTPParser::IsReadyParse()
{
//	return true;

#if 0
	return true;
#else
	int storedPacket = m_rtpPacketReorder->GetStoredPacket();
	if(storedPacket >= MAX_STRORED_PACKETS)
		return true;
	else
		return false;
#endif
}

bool CRTPParser::ParseRTPPayloadFrame(CRTPPacket * rtpPacket) 
{
	if(rtpPacket->RTPDataSize() == 0)
	{
		return false;
	}

	int payloadFrameSize = rtpPacket->RTPDataSize();
	memcpy(m_frameData + m_frameSize, rtpPacket->RTPData(), payloadFrameSize);
	m_frameSize += payloadFrameSize;
	rtpPacket->Skip(payloadFrameSize);

	return true;
}

void CRTPParser::RenderFrame(unsigned int rtpTimestamp,int subFrame,unsigned int seqNum,bool IsKeyFrame)
{
	
	m_mediaStream->RecvMediaFrame(m_frameData, m_frameSize, rtpTimestamp, seqNum, IsKeyFrame);
	Reset();
}

void CRTPParser::ReleaseReorderedRTPPackets()
{
	m_rtpPacketReorder->ReleaseAllRTPPackets();
}



bool SeqNumLT(unsigned short seqNum1, unsigned short seqNum2)
{
	int diff = seqNum2 - seqNum1;
	if(diff > 0)
	{
		return (diff < 0x8000);
	}
	else if(diff < 0)
	{
		return (diff < -0x8000);
	}
	else
	{
		return false;
	}
}


void CRTPParser::SetFrameData(FrameData* frame)
{
	if(!frame->isFragment)
		m_frameSize = 0;

	memcpy(m_frameData+m_frameSize,frame->data,frame->len);
	m_frameSize+=frame->len;
	m_frame.timestamp	=frame->timestamp;
	m_frame.isKeyFrame  =frame->isKeyFrame;
	m_frame.seqNum		=frame->seqNum;
	m_frame.data		=m_frameData;
	m_frame.len			=m_frameSize;
}

FrameData* CRTPParser::GetFrameData()
{
	return &m_frame;
}
