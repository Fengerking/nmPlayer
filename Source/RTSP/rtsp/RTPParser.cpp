#include "voLog.h"
#include <string.h>
#include "RTSPMediaStreamSyncEngine.h"
#include "MediaStream.h"
#include "utility.h"
#include "MediaStreamSocket.h"
#include "RealTimeStreamingEngine.h"

#include "RTPPacket.h"
#include "RTPPacketReorder.h"
#include "RTPSenderTable.h"

#include "RTPParser.h"
#include "RTCPParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

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
#ifdef _VONAMESPACE
}
#endif


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

static const int MAX_FRAME_SIZE = 1024 * 256;
static const int BUFFER_RESERVE_HEAD = 1024;

void		CRTPParser::UpdateSocket(CMediaStreamSocket * socket)
{
	m_rtpStreamSock=socket;
	//SLOG1(LL_RTP_ERR,"flow.txt","UpdateSocket_tcp=%d\n",CUserOptions::UserOptions.m_useTCP)

	if(CUserOptions::UserOptions.m_useTCP<=0||CUserOptions::UserOptions.m_URLType==UT_SDP2_LOCAL)
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
enum{
	MAX_ANTIJITTER_BUF_SIZE = 30,
	DEFAULT_ANTIJITTER_BUF_SIZE = 10,
	MIN_ANTIJITTER_BUF_SIZE = 5,
};
CRTPParser::CRTPParser(CMediaStream * mediaStream, CMediaStreamSocket * rtpStreamSock)
: m_mediaStream(mediaStream)
, m_rtpStreamSock(rtpStreamSock)
, m_frameData(NULL)
, m_frameSize(0)
, m_seqNum(0)
, m_curRtpSeqNum(0)
, m_lastReceivedSSRC(0)
, m_packetLossInMultiPacketFrame(false)
, m_firstPacketInMultiPacketFrame(true)
, m_lastPacketInMultiPacketFrame(true)
, m_antijitterBufferSize(DEFAULT_ANTIJITTER_BUF_SIZE)
{
	
	UpdateSocket(rtpStreamSock);

	m_rtpPacketReorder = new CRTPPacketReorder(this);TRACE_NEW("rtpp_m_rtpPacketReorder",m_rtpPacketReorder);
	SLOG2(LL_RTP_ERR,"flow.txt","construct RTPParser,%x,%x\n",(unsigned int)m_rtpPacketReorder,(unsigned int)this);
	m_frameData = new unsigned char[MAX_FRAME_SIZE]+BUFFER_RESERVE_HEAD;TRACE_NEW("rtpp_m_m_frameData",m_frameData-BUFFER_RESERVE_HEAD);//reserved the first BUFFER_RESERVE_HEAD bytes
	
	m_rtpSenderTable = new CRTPSenderTable();TRACE_NEW("rtpp_m_rtpSenderTable",m_rtpSenderTable);
	m_SSRC = Random32();
	m_lastRTPTimestamp = 0;
	m_lastRTPPacketReceivedTime.tv_sec = 0;
	m_lastRTPPacketReceivedTime.tv_usec = 0;
	memset(&m_frame,0,sizeof(FrameData));
	if (m_mediaStream->IsAudioStream())
	{
		m_antijitterBufferSize = MIN_ANTIJITTER_BUF_SIZE;
	}

}
long		CRTPParser::			GetAntiJitterBufTime()
{
	long ret = 0;
	ret = m_rtpPacketReorder->GetStoredPackTime(m_mediaStream->RTPTimestampFrequency());

	if(m_mediaStream->IsVideoStream())
	{
		if(ret > GetAntiJitterBufSize()*1000/30)
			ret = GetAntiJitterBufSize()*1000/30;
	}
	
	return ret;
}
void      CRTPParser::          EnlargeAntiJitterBuffer()
{
		m_antijitterBufferSize += 5;
		if(m_antijitterBufferSize>MAX_ANTIJITTER_BUF_SIZE)
			m_antijitterBufferSize = MAX_ANTIJITTER_BUF_SIZE;
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
	SLOG1(LL_RTP_ERR,"flow.txt","destruct RTPParser,%x\n",(unsigned int)m_rtpPacketReorder);

	SAFE_DELETE(m_rtpPacketReorder);
	m_frameData-=BUFFER_RESERVE_HEAD;
	SAFE_DELETE_ARRAY(m_frameData);
	SAFE_DELETE(m_rtpSenderTable);
}
bool	CRTPParser::IsValidPackBySeqNum(int seqNum)
{
	//WMV streaming is special,the seqNum does not work,the sync is based on the first frame
	if(CUserOptions::UserOptions.streamType==ST_ASF)
	{
		CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
		int seqNum2=pRTSPMediaStreamSyncEngine->GetCommonSeqNum();
		if(seqNum2==ASF_SEQ_RESET)
			return true;
		if((seqNum2-seqNum)>30000)//wrap around happens
		{
			sprintf(CLog::formatString,"#seq=%d,rtpInfo.seqNum %d\n",seqNum,seqNum2);
			CLog::Log.MakeLog(LL_RTP_ERR,"rtpStatus.txt",CLog::formatString);
			seqNum2=0;
			pRTSPMediaStreamSyncEngine->SetCommonSeqNum(seqNum2);
			
		}
		if(seqNum2!=ASF_SEQ_USE_INDIVIDUAL)
		{
			return (seqNum>=seqNum2);//||(seqNum2-seqNum)>30000);
		}
	}
	if(m_seqNum==0)//first time,and the rtpinfo has been got
		m_seqNum =m_mediaStream->m_rtpInfo.seqNum;
	if(m_seqNum>64000&&seqNum<300)//wrap around happens
	{
		m_mediaStream->m_rtpInfo.seqNum=0;
		m_seqNum=1;//set it as 1 to indicates it is not the first time
	}
	if(seqNum<m_mediaStream->m_rtpInfo.seqNum)//assume the delay of packets will not beyond 10
	{
		if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
		{
			sprintf(CLog::formatString,"###seq=%d,rtpInfo.seqNum %d,lastSeq=%d\n",seqNum,m_mediaStream->m_rtpInfo.seqNum,m_seqNum);
			CLog::Log.MakeLog(LL_RTP_ERR,"rtpStatus.txt",CLog::formatString);
		}
		return false;
	}
	if(m_seqNum&&seqNum)//make sure the m_seqNum is great than 0. 
		m_seqNum = seqNum;
	return true;

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
		{
			break;
		}
		
		int recvBytes = rtpPacket->ReadRTPPacket(rtpParser->m_rtpStreamSock);
		if(recvBytes<=0)
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtpStatus.txt","@@@@error the returned RTP data is not great than 0\n");
			rtpPacketReorder->ReleaseRTPPacket(rtpPacket);
			return -108;
		}
		
		unsigned char flag = 0;
		
		int rtpPacketSize = rtpPacket->RTPDataSize();
		if(rtpPacketSize <= 0)
			break;

		if(rtpParser->m_mediaStream->IsVideoStream())
		{
			flag = LOCAL_VIDEO_RTP;
		}
		else if(rtpParser->m_mediaStream->IsAudioStream())
		{
			flag = LOCAL_AUDIO_RTP;
		}
		if((CUserOptions::UserOptions.m_bMakeLog==LOG_DATA))
		{
			CLog::EnablePrintTime(false);
			
			CLog::Log.MakeLog(LL_AUDIO_DATA,"RTSPStream.data", &flag, 1);
			CLog::Log.MakeLog(LL_AUDIO_DATA,"RTSPStream.data",(unsigned char *)&rtpPacketSize, sizeof(int));
			CLog::Log.MakeLog(LL_AUDIO_DATA,"RTSPStream.data", rtpPacket->RTPData(), rtpPacketSize);
		}

		if(!rtpPacket->ParseRTPPacketHeader())
		{
			CLog::Log.MakeLog(LL_RTP_ERR,"rtpStatus.txt","@@@@error to parse the rtp packet head\n");
			break;
		}
		sprintf(CLog::formatString,"seq=%d,ts=%u,size=%d,marker=%d\n",rtpPacket->RTPSeqNum(),rtpPacket->RTPTimestamp(),rtpPacket->RTPDataSize(),rtpPacket->RTPMarker());
	
		/*if (mediaStream->IsAudioStream())
		{
			sprintf(CLog::formatString,"*******CRTPParser::ReadRTPStream********rtpPacket, 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x\n",
				rtpPacket->RTPData()[0],rtpPacket->RTPData()[1],rtpPacket->RTPData()[2],rtpPacket->RTPData()[3],rtpPacket->RTPData()[4],rtpPacket->RTPData()[5],rtpPacket->RTPData()[6],rtpPacket->RTPData()[7]);
			CLog::Log.MakeLog(LL_RTSP_ERR,"RTPAudioPacket.txt",CLog::formatString);
		}*/
		
		if(CUserOptions::UserOptions.m_pRTSPTransFunc)
		{
			VOLOGI("***************************************************************isTCP=%d, flag=%d,size=%d,rtpPacket=%x, RTPSeqNum=%d, RTPTimestamp = %d, CUserOptions::UserOptions.m_skipPos = %d\n",CUserOptions::UserOptions.m_useTCP,flag,rtpPacket->RTPPacketSize(),rtpPacket->RTPData(),rtpPacket->RTPSeqNum(),rtpPacket->RTPTimestamp(), CUserOptions::UserOptions.m_skipPos);
			CUserOptions::UserOptions.m_pRTSPTransFunc(CUserOptions::UserOptions.m_useTCP, flag, rtpPacket->RTPPacketSize(),rtpPacket->RTPData()-CUserOptions::UserOptions.m_skipPos, rtpPacket->RTPSeqNum(), rtpPacket->RTPTimestamp()); //-12
		}


		if(mediaStream->IsVideoStream())
		{
			CLog::Log.MakeLog(LL_RTP_ERR,"videoRTP.txt",CLog::formatString);	
		}
		else if (mediaStream->IsAudioStream())
		{
			CLog::Log.MakeLog(LL_RTP_ERR,"audioRTP.txt",CLog::formatString);	
		}

		if(rtpParser->IsValidPackBySeqNum(rtpPacket->RTPSeqNum())==false)
			break;
		rtpParser->UpdateRTCP(rtpPacket);
		rtpPacketReorder->ReorderRTPPacket(rtpPacket);
		
		if(rtpParser->IsReadyParse())
			rtpParser->ParseRTPPayload();
		else
		{
			if(CUserOptions::UserOptions.isBuffering)
			{
				CLog::Log.MakeLog(LL_RTP_ERR,"netdataStatus.txt","not ready\n");
			}
		}
		
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
/*
int CRTPParser::ReadRTPStreamOverTCP(PIVOSocket streamSock, int streamDataSize)
{
	CRTPPacket * rtpPacket = NULL;
	do
	{
		rtpPacket = m_rtpPacketReorder->GetEmptyRTPPacket();
		if(rtpPacket == NULL)
			break;

		int recvBytes=rtpPacket->ReadRTPPacketOverTCP(streamSock, streamDataSize);
		if(recvBytes<=0)
		{
			CLog::Log.MakeLog(LL_RTP_ERR,"rtpStatus.txt","@@@@error the returned rtp data is not great than 0\n");
			break;
		}
		if(!rtpPacket->ParseRTPPacketHeader())
		{
			CLog::Log.MakeLog(LL_RTP_ERR,"rtpStatus.txt","@@@@error to parse the rtp packet head\n");
			break;
		}
		if(IsValidPackBySeqNum(rtpPacket->RTPSeqNum())==false)
			break;
		//m_mediaStream->m_rtpInfo.seqNum = 0;

		m_rtpPacketReorder->ReorderRTPPacket(rtpPacket);
		if(IsReadyParse())
			ParseRTPPayload();

		return 1;

	}while(0);

	m_rtpPacketReorder->ReleaseRTPPacket(rtpPacket);
	return 0;
}
*/
int CRTPParser::ReadRTPStreamFromLocal(unsigned char * streamData, int streamDataSize)
{
	CRTPPacket * rtpPacket = NULL;
	do
	{
		
		//SLOG2(LL_RTP_ERR,"flow.txt","GetEmpPack,%x,%x\n",m_rtpPacketReorder,this);
		
		rtpPacket = m_rtpPacketReorder->GetEmptyRTPPacket();
		//CLog::Log.MakeLog(LL_RTP_ERR,"rtpStatus.txt","after get emptyPack\n");
		if(rtpPacket == NULL)
			break;

		rtpPacket->ReadRTPPacketFromLocal(streamData, streamDataSize);

		if(!rtpPacket->ParseRTPPacketHeader())
		{
			CLog::Log.MakeLog(LL_RTP_ERR,"rtpStatus.txt","@@@@error to parse te rhtp packet head\n");
			break;
		}

		sprintf(CLog::formatString,"seq=%d,ts=%d,size=%d,marker=%d\n",rtpPacket->RTPSeqNum(),rtpPacket->RTPTimestamp(),rtpPacket->RTPDataSize(),rtpPacket->RTPMarker());
		//sprintf(CLog::formatString,"seq=%d,ts=%d,size=%d,marker=%d,data=%x\n",rtpPacket->RTPSeqNum(),rtpPacket->RTPTimestamp(),rtpPacket->RTPDataSize(),rtpPacket->RTPMarker(),rtpPacket->RTPData());

		unsigned char flag=0;

		if(m_mediaStream->IsVideoStream())
		{
			flag = LOCAL_VIDEO_RTP;
			CLog::Log.MakeLog(LL_RTSP_ERR,"videoRTPLocal.txt",CLog::formatString);
		}
		else if (m_mediaStream->IsAudioStream())
		{
			flag = LOCAL_AUDIO_RTP;
			CLog::Log.MakeLog(LL_RTSP_ERR,"audioRTPLocal.txt",CLog::formatString);	
		}

		if(CUserOptions::UserOptions.m_pRTSPTransFunc)
		{
			//VOLOGI("***************************************************************RTPMaker = %d, RTPSeqNum=%d, RTPPayloadType = %d\n",rtpPacket->RTPMarker(), rtpPacket->RTPSeqNum(),rtpPacket->RTPPayloadType());
			VOLOGI("***************************************************************isTCP=%d, flag=%d,size=%d,rtpPacket=%x, RTPSeqNum=%d, RTPTimestamp = %d, CUserOptions::UserOptions.m_skipPos = %d\n",CUserOptions::UserOptions.m_useTCP,flag,rtpPacket->RTPPacketSize(),rtpPacket->RTPData(),rtpPacket->RTPSeqNum(),rtpPacket->RTPTimestamp(), CUserOptions::UserOptions.m_skipPos);
			CUserOptions::UserOptions.m_pRTSPTransFunc(CUserOptions::UserOptions.m_useTCP, flag, rtpPacket->RTPPacketSize(),rtpPacket->RTPData()-CUserOptions::UserOptions.m_skipPos, rtpPacket->RTPSeqNum(), rtpPacket->RTPTimestamp());	//-12
		}

		if(m_mediaStream->IsVideoStream())
		{
			static int previousSeq=0;
			int curPacket=rtpPacket->RTPSeqNum();
			if(abs(curPacket-previousSeq)>1)
			{
				//CLog::Log.MakeLog(LL_RTP_ERR,"seq.txt","packet-loss\n");
				sprintf(CLog::formatString,"curSeq=%d,pre=%d,lost=%d\n",curPacket,previousSeq,curPacket-previousSeq);
				CLog::Log.MakeLog(LL_RTP_ERR,"seq.txt",CLog::formatString);
			}
			previousSeq=curPacket;
		}
		
		m_rtpPacketReorder->ReorderRTPPacket(rtpPacket);

		//if(IsReadyParse()&&CUserOptions::UserOptions.m_URLType!=UT_SDP2_LOCAL)
		UpdateRTCP(rtpPacket);
		if(IsReadyParse())
			ParseRTPPayload();
		return 1;

	}while(0);
	if(rtpPacket)
		m_rtpPacketReorder->ReleaseRTPPacket(rtpPacket);
	return 0;
}
void CRTPParser::RetrieveAllPack(int packetCounts2)
{
	
	int packetCounts=m_rtpPacketReorder->GetStoredPacket();
	if(packetCounts>0)
	{
		//sprintf(CLog::formatString,"RetrieveAllPack,%d\n",packetCounts);
		//CLog::Log.MakeLog(LL_RTP_ERR,"rtsp.txt",CLog::formatString);
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

	do
	{
		bool packetLoss = true;
		rtpPacket = m_rtpPacketReorder->GetNextReorderedRTPPacket(packetLoss);
		if(packetLoss)
			m_mediaStream->SetPacketLoss(packetLoss);
		if(rtpPacket == NULL)
		{
			CLog::Log.MakeLog(LL_RTP_ERR,"nullPacke.txt","rtpPacket == NULL\n");	
			break;
		}
		if(!ParseRTPPayloadHeader(rtpPacket))
		{
			CLog::Log.MakeLog(LL_RTP_ERR,"packet_loss.txt","@@@@fail to parse the Payload  head\n");
			break;
		}

		if(m_lastRTPTimestamp == 0)
		{
			m_lastRTPTimestamp = rtpPacket->RTPTimestamp();
		}
		if(m_lastRTPPacketReceivedTime.tv_sec == 0 && m_lastRTPPacketReceivedTime.tv_usec == 0)
		{
			m_lastRTPPacketReceivedTime = rtpPacket->PacketReceivedTime();
		}
	
		m_lastRTPTimestamp = rtpPacket->RTPTimestamp();
		m_lastRTPPacketReceivedTime = rtpPacket->PacketReceivedTime();

		if(0)//packetLoss==false)
		{
			if(m_firstPacketInMultiPacketFrame==false&&m_lastPacketInMultiPacketFrame)
			{
				CLog::Log.MakeLog(LL_RTP_ERR,"rtpStatus.txt","@@@@packet-loss:no head but with end\n");
				packetLoss = true;
			}
		}
		
		if(m_firstPacketInMultiPacketFrame)
		{
			if(packetLoss || m_packetLossInMultiPacketFrame)
			{
				m_frameSize = 0;
			}

			m_packetLossInMultiPacketFrame = false;
		}
		else if(packetLoss)
		{
			m_packetLossInMultiPacketFrame = true;
		}

		if(m_packetLossInMultiPacketFrame)
		{
			m_frameSize = 0;
			break;
		}
#define MAX_AU_IN_ONE_PACKET 2000
		int count=0;//it is impossible that a packet has over 2000 AU
		do
		{
			if(!ParseRTPPayloadFrame(rtpPacket))
			{
				if(count)//Some h.264 pack only has 2 bytes head as delima pack,so if it is the first time, try to output 
					break;
			}

			if(m_lastPacketInMultiPacketFrame||rtpPacket->RTPMarker())
			{
				RenderFrame(rtpPacket->RTPTimestamp()+GetRelativeTS(),count,rtpPacket->RTPSeqNum(),IsKeyFrame());
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

bool	CRTPParser::IsReadyParse()
{
	int storedPacket = m_rtpPacketReorder->GetStoredPacket();
	if((unsigned int)storedPacket>=m_antijitterBufferSize)//||CUserOptions::UserOptions.streamEnd)//||CUserOptions::UserOptions.status==Session_Paused)//CUserOptions::UserOptions.streamEnd)//CUserOptions::UserOptions.status==Session_Paused||
		return true;
	else
		return false;
}
bool CRTPParser::ParseRTPPayloadFrame(CRTPPacket * rtpPacket) 
{
	if(rtpPacket->RTPDataSize() == 0)
	{
		CLog::Log.MakeLog(LL_RTP_ERR,"error.txt","$$$$$ParseRTPPayloadFrame fail--datasize==0,\n");
		return false;
	}
	int payloadFrameSize = rtpPacket->RTPDataSize();

	if((payloadFrameSize+m_frameSize)>MAX_FRAME_SIZE)
	{
		rtpPacket->Skip(payloadFrameSize);

		sprintf(CLog::formatString,"frameSize(%d)>MAX_FRAME_SIZE\n",payloadFrameSize+m_frameSize);
		CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt",CLog::formatString);
		return false;
	}

	memcpy(m_frameData + m_frameSize, rtpPacket->RTPData(), payloadFrameSize);
	m_frameSize += payloadFrameSize;
	rtpPacket->Skip(payloadFrameSize);

	return true;
}

void CRTPParser::RenderFrame(unsigned int rtpTimestamp,int subFrame,unsigned int seqNum,bool IsKeyFrame)
{
	//if(m_mediaStream->IsAudioAMRNB())//The adjust is removed to RecvMediaFrame
	//	rtpTimestamp+=subFrame*160;
	m_mediaStream->RecvMediaFrame(m_frameData, m_frameSize, rtpTimestamp,seqNum,IsKeyFrame);
	m_frameSize = 0;
}
void CRTPParser::CleanFrameBuf()
{
	memset(m_frameData,0,m_frameSize);
	m_frameSize = 0;
}
void CRTPParser::ReleaseReorderedRTPPackets()
{
	m_rtpPacketReorder->ReleaseAllRTPPackets();
	CleanFrameBuf();
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
