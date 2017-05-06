#include "utility.h"
#if ENABLE_REAL
#include "network.h"
#include "RDTMediaStream.h"
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "BitVector.h"
#include "network.h"
#include "RDTParser.h"
#include "RTSPSession.h"
#include "MediaFrameSink.h"
#include "MediaStreamSocket.h"
#include "RealTimeStreamingEngine.h"
#include "RTSPMediaStreamSyncEngine.h"
#include "rdtpck.h"
#include "ra_depack.h"
#include "rv_depack.h"
#include "RealCommonbase.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

bool SeqNumLT2(unsigned short seqNum1, unsigned short seqNum2)
{
	int diff = seqNum2 - seqNum1;
	if(diff > 0)
	{
		return (diff < 0xff00);
	}
	else if(diff < 0)
	{
		return (diff < -0xff00);
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

static const int  MAX_UDP_PACKET_SIZE= 1024 * 5;
static const int  MAX_FRAME_SIZE=1024*40;
unsigned int CRDTDataPacket::GetTimeStamp()
{
	return rmPacket->ulTime;
}
unsigned int CRDTDataPacket::GetDataSize()
{
	return rmPacket->usDataLen;
}
int CRDTDataPacket::CopyTNGDataPacket(struct TNGDataPacket *packet,int flag)
{
	rmPacket->usASMFlags = flag;//
	rmPacket->ucASMRule = packet->asm_rule_number;
	rmPacket->ucLost	= 0;
	rmPacket->ulTime	= packet->timestamp;
	rmPacket->usStream	= packet->stream_id;
	rmPacket->usDataLen = packet->data.len;
	seq_no				= packet->seq_no;
	memcpy(rmPacket->pData,packet->data.data,rmPacket->usDataLen);
	return 0;
}
void CRDTDataPacket::SetAsLostPck()
{
	rmPacket->ucLost = 1;
}
CRDTDataPacket::CRDTDataPacket()
{
	rmPacket = new rm_packet_struct();TRACE_NEW("xxrmPacket=",rmPacket);
	rmPacket->pData = new unsigned char[MAX_UDP_PACKET_SIZE];TRACE_NEW("xxrmPacket->pData=",rmPacket->pData);
}
CRDTDataPacket::~CRDTDataPacket()
{
	SAFE_DELETE_ARRAY(rmPacket->pData);
	SAFE_DELETE(rmPacket);
}
static const int MAX_PROPERTY_NUM=8;
CRDTMediaStream::CRDTMediaStream(CRTSPSession * rtspSession)
:CMediaStream(rtspSession),
m_streamID(-1),
m_asmRule2Stream(0),
m_haveSeenInitialSeqNum(false),
m_nextExpectedSeqNum(0),
m_rmff(NULL),
m_asmRuleBook(NULL),
m_nTimePerFrame(0),
m_previousSeq(-1),
m_lastSeq(0)
{
	m_streamHead = new rm_stream_header();TRACE_NEW("xxm_streamHead=",m_streamHead);
	m_streamHead->ulNumProperties	= 0;
	m_streamHead->pProperty = new rm_property[MAX_PROPERTY_NUM];TRACE_NEW("xxm_streamHead->pProperty=",m_streamHead->pProperty);
	for (int num=0;num<MAX_PROPERTY_NUM;num++)
	{
		m_streamHead->pProperty[num].pName = NULL;
	}
	m_codecInitParam = NULL;
	m_rdtBufferInfo = new struct RDTBufferInfo();TRACE_NEW("xxm_rdtBufferInfo=",m_rdtBufferInfo);
	m_ackPacket		= new struct TNGACKPacket();TRACE_NEW("xxm_ackPacket=",m_ackPacket);
	m_ackPacket->data.data = new INT8[256];TRACE_NEW("xxm_ackPacket->data.data=",m_ackPacket->data.data);
	//m_frameData = new unsigned char[MAX_FRAME_SIZE];
	
}
CRDTMediaStream::~CRDTMediaStream(void)
{
	for (int num=0;num<MAX_PROPERTY_NUM;num++)
	{
		SAFE_DELETE(m_streamHead->pProperty[num].pName);
	}
	SAFE_DELETE(m_rdtBufferInfo);	
	SAFE_DELETE(m_streamHead);
	SAFE_DELETE_ARRAY(m_ackPacket->data.data);
	SAFE_DELETE(m_ackPacket);
	//SAFE_DELETE_ARRAY(m_frameData);
	if(m_codecInitParam)
		free(m_codecInitParam);
	SAFE_DELETE(m_rmff);
	SAFE_DELETE(m_asmRuleBook);
	if(IsAudioStream())
	{
		ra_depack_destroy(&m_depack);
	}
	else if(IsVideoStream())
	{
		rv_depack_destroy(&m_depack);
	}
	Reset();
}


int	CRDTMediaStream:: SendACKPacket(  
						   int	bLost)
{
	UINT16 unBegSeqNum	= m_previousSeq;
	UINT16 unEndSeqNum	= m_lastSeq;
	UINT16 unBitCount  = unEndSeqNum-unBegSeqNum;
	UINT8  unByteCount = 0;
	UINT16 unDataLen   = 0;
	UINT8* pData       = NULL;
	UINT8* pOff        = NULL;
	UINT32 unStreamNum   = m_streamID;
	struct TNGACKPacket* pkt = m_ackPacket;
	if( unBitCount > 0 )
		unByteCount = unBitCount/8+1;


	/* Pack the Ack/Nak bit field with the sequence
	* number and stream number info */
	unDataLen = 7+unByteCount;
	pData = (UINT8*)(m_ackPacket->data.data);

	/* Setting this all to ones means we received all the
	* packets, all zeros means lost all packets.
	*/
	memset( pData, (UINT8)(bLost?0:0xFF), unDataLen );
	pOff = pData;

	//pOff = addshort(pOff, unStreamNum);
	*pOff++=(BYTE)(unStreamNum>>8);
	*pOff++=(BYTE)(unStreamNum&0xff);
	//pOff = addshort(pOff, unEndSeqNum);
	*pOff++=(BYTE)(unEndSeqNum>>8);
	*pOff++=(BYTE)(unEndSeqNum&0xff);
	//pOff = addshort(pOff, unBitCount);
	*pOff++=(BYTE)(unBitCount>>8);
	*pOff++=(BYTE)(unBitCount&0xff);
	//pOff = addbyte(pOff, unByteCount);
	*pOff++ =(BYTE)(unByteCount&0xff);

	


	/* Fill in the packet structure */
	pkt->length_included_flag = 0;
	pkt->dummy0               = 0;
	pkt->dummy1               = 0;
	pkt->packet_type = RDT_ACK_PKT;
	pkt->lost_high   = (UINT8)(bLost?1:0);
	pkt->data.len    = unDataLen;
	m_previousSeq = m_lastSeq+1;

	CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
	pRTSPMediaStreamSyncEngine->GetRDTParser()->SendACK(pkt);

	return 0;
}
void CRDTMediaStream::Reset()
{
	ReleaseAllPackets();

	list_T<CRDTDataPacket *, allocator<CRDTDataPacket *, 10> >::iterator iter;
	for(iter=m_listEmptyPacket.begin(); iter!=m_listEmptyPacket.end(); ++iter)
	{
		SAFE_DELETE(*iter);
	}
	m_listEmptyPacket.clear();
}
bool CRDTMediaStream::ParseSDPParam_m(char *sdpParam)
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
CRDTDataPacket * CRDTMediaStream::GetEmptyPacket()
{
	CRDTDataPacket * rdtPacket = NULL;
	do
	{
		if(!m_listEmptyPacket.empty())
		{
			rdtPacket = m_listEmptyPacket.front();
			m_listEmptyPacket.pop_front();
			break;
		}

		rdtPacket = new CRDTDataPacket();TRACE_NEW("xxrdtPacket=",rdtPacket);

	}while(0);

	return rdtPacket;
}

void CRDTMediaStream::ReorderPacket(CRDTDataPacket * rdtPacket)
{
	if(!m_haveSeenInitialSeqNum)
	{
		m_haveSeenInitialSeqNum = true;
		m_nextExpectedSeqNum = rdtPacket->seq_no;
	}

	//if(SeqNumLT(rdtPacket->RTPSeqNum(), m_nextExpectedSeqNum))
	//	return;


	list_T<CRDTDataPacket *, allocator<CRDTDataPacket *, 10> >::iterator iter;
	
	for(iter=m_listReorderedPacket.begin(); iter!=m_listReorderedPacket.end(); ++iter)
	{
		if(SeqNumLT2(rdtPacket->seq_no, (*iter)->seq_no))
			break;
		if(rdtPacket->seq_no == (*iter)->seq_no)
			return;
	}
	m_listReorderedPacket.insert(iter, rdtPacket);
	m_rdtBufferInfo->bytes_buffered = 0;
	m_rdtBufferInfo->stream_id = m_streamID;

	iter=m_listReorderedPacket.begin();
	m_rdtBufferInfo->lowest_timestamp = (*iter)->GetTimeStamp();
	if(m_previousSeq==-1)
		m_previousSeq  = (*iter)->seq_no;

	for(; iter!=m_listReorderedPacket.end(); ++iter)
	{
		m_rdtBufferInfo->bytes_buffered+=(*iter)->GetDataSize();

	}
	iter--;
	m_rdtBufferInfo->highest_timestamp = (*iter)->GetTimeStamp();
	m_lastSeq = (*iter)->seq_no;
	
}
void CRDTMediaStream::GetRDTBufInfo(struct RDTBufferInfo* info)
{
	if(info)
	{
		info->bytes_buffered	= m_rdtBufferInfo->bytes_buffered;
		info->lowest_timestamp	= m_rdtBufferInfo->lowest_timestamp;
		info->highest_timestamp	= m_rdtBufferInfo->highest_timestamp;
		info->stream_id			= m_rdtBufferInfo->stream_id;
	}
}

int		CRDTMediaStream:: GetOneFrame()
{
	if(GetStoredPacket()>10||CUserOptions::UserOptions.streamEnd||CUserOptions::UserOptions.status==Session_Paused)
	{
		bool packetLoss=false;
		CRDTDataPacket *rdtPacket=GetNextReorderedPacket(packetLoss);
		if(rdtPacket)
		{
			rm_packet* rmPck=rdtPacket->GetRMPacket();
			if(IsAudioStream())
			{
				if(ra_depack_add_packet(m_depack,rmPck))
				{
					CLog::Log.MakeLog(LL_RTP_ERR,"rdt.txt","audio ra_depack_add_packet fails\n");
				} 
			}
			else
			{
				if(rv_depack_add_packet(m_depack,rmPck))
				{
					CLog::Log.MakeLog(LL_RTP_ERR,"rdt.txt","video rv_depack_add_packet fails\n");
				}
			}
			ReleasePacket(rdtPacket);
		}

	}
	return 0;
}

CRDTDataPacket * CRDTMediaStream::GetNextReorderedPacket(bool & packetLoss)
{
	if(m_listReorderedPacket.empty())
		return NULL;

	CRDTDataPacket * rdtPacket = m_listReorderedPacket.front();

	if(rdtPacket->seq_no == m_nextExpectedSeqNum)
	{
		packetLoss = false;
		m_listReorderedPacket.pop_front();
		return rdtPacket;
	}

	//packet loss,add one empty packet
	if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
	{
		if(IsVideoStream())
		{
			sprintf(CLog::formatString,"seq=%d,curr=%d\n",m_nextExpectedSeqNum,rdtPacket->seq_no);
			CLog::Log.MakeLog(LL_RTP_ERR,"Video_packet-loss.txt",CLog::formatString);
		}
		else if(IsAudioStream())
		{
			sprintf(CLog::formatString,"seq=%d,curr=%d\n",m_nextExpectedSeqNum,rdtPacket->seq_no);
			CLog::Log.MakeLog(LL_RTP_ERR,"Audio_packet-loss.txt",CLog::formatString);
		}
	
	}

	CRDTDataPacket* emptyPck = GetEmptyPacket();
	emptyPck->SetAsLostPck();
	emptyPck->seq_no	= m_nextExpectedSeqNum;
	//ReorderPacket(emptyPck);
	packetLoss = true;
	return emptyPck;

	return NULL;
}

void CRDTMediaStream::ReleasePacket(CRDTDataPacket * rdtPacket)
{
	if(rdtPacket == NULL)
		return;

	++m_nextExpectedSeqNum;

	m_listEmptyPacket.push_back(rdtPacket);
}

void CRDTMediaStream::ReleaseAllPackets()
{
	m_haveSeenInitialSeqNum = false;
	m_nextExpectedSeqNum = 0;

	list_T<CRDTDataPacket *, allocator<CRDTDataPacket *, 10> >::iterator iter;
	for(iter=m_listReorderedPacket.begin(); iter!=m_listReorderedPacket.end(); ++iter)
	{
		m_listEmptyPacket.push_back(*iter);
	}
	m_listReorderedPacket.clear();
}

void  CRDTMediaStream::RecvMediaFrame(unsigned char * frameData, int frameSize, unsigned int rtpTimestamp,unsigned int seqNum,bool IsKeyFrame)
{
#define TIME_DIFF_MAX 200
#define FIRST_LOG_FRAME 16
	if(IsVideoStream())
	{
		rv_frame* frame = (rv_frame*)frameData;
		unsigned int ts = frame->ulTimestamp-m_streamStartTime;
		if(ts<0)
		{
			ts = 0;
		}
		else
		{
			UINT32 lastTime = m_mediaFrameSink->GetLastTimeInBuffer();
			if(ts < lastTime + m_nTimePerFrame&&ts>lastTime)
				ts = lastTime + m_nTimePerFrame;

		}
		
		frame->ulTimestamp = ts;
		frameSize = frame->ulDataLen+sizeof(rv_frame)+frame->ulNumSegments*sizeof(rv_segment);
		/*if(CUserOptions::UserOptions.m_bMakeLog>LOG_DATA)
		{
			GetMediaData(frameData,frameSize);
		}*/
		m_mediaFrameSink->SinkMediaFrame(frameData, frameSize, m_streamStartTime,0, 1);
	}
	else if(IsAudioStream())
	{
		ra_block* frame=(ra_block*)frameData;
		frameSize = frame->ulDataLen+sizeof(ra_block);
		/*if(CUserOptions::UserOptions.m_bMakeLog>LOG_DATA)
		{
			GetMediaData(frameData,frameSize);
		}*/
		int ts = frame->ulTimestamp-m_streamStartTime;
		if(ts<0)
			ts = 0;
		frame->ulTimestamp = ts;
		m_mediaFrameSink->SinkMediaFrame(frameData, frameSize, m_streamStartTime, 0,0);
	}
}
void  CRDTMediaStream::FlushMediaStream(bool flushBuffer)
{
	ReleaseAllPackets();
	m_mediaFrameSink->FlushMediaFrame(0);
	m_isNewStart = true;
}

int CRDTMediaStream::GetStreamID()
{
	if(m_streamID==-1)
	{
		char* t=strstr(m_controlAttr,"=");
		if(t&&sscanf(t,"=%d",&m_streamID)==1)
		{
			
		}
	}
	return m_streamID;
}
int CRDTMediaStream::ReceiveRDTDataPacket(struct TNGDataPacket* rdtPacket)
{
	if(IsAppStream())
		return 0;

	do
	{
		if(rdtPacket->seq_no < m_rtpInfo.seqNum)
		{
			if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
			{
				sprintf(CLog::formatString,"####the seq number %d is less than the rtpInfo.seqNum %d\n",rdtPacket->seq_no,m_rtpInfo.seqNum);
				CLog::Log.MakeLog(LL_RTP_ERR,"rdtStatus.txt",CLog::formatString);
			}
			break;
		}
		//m_rtpInfo.seqNum = 0;
		UINT16 flag= m_asmRule2Flag[rdtPacket->asm_rule_number];

		if(m_isNewStart)
		{
			if(IsAudioStream())
			{
#define HX_KEYFRAME_FLAG     0x0002
				if((flag&HX_KEYFRAME_FLAG)==0)
				{
					sprintf(CLog::formatString,"the first frame seq=%d ts=%lu,of audio is not a Keyframe, flag=0x%X\n",rdtPacket->seq_no,rdtPacket->timestamp,flag);
					CLog::Log.MakeLog(LL_RTP_ERR,"rdtStatus.txt",CLog::formatString);
					break;
				}
				ra_depack_seek(m_depack,m_streamStartTime);
			}
			else
			{
				rv_depack_seek(m_depack,m_streamStartTime);
			}
			m_isNewStart = 0;
		}
	
		CRDTDataPacket* rdtPacket2 = GetEmptyPacket();
		if (rdtPacket2)
		{
			rdtPacket2->CopyTNGDataPacket(rdtPacket,flag);
			ReorderPacket(rdtPacket2);
		}
		GetOneFrame();
		if(m_lastSeq-m_previousSeq>10)
		{
			//SendACKPacket(0);
		}
		return PACKET_RTP;

	}while(0);

	return 0;
}
bool  CRDTMediaStream::CreateRDTPayloadParser()
{

	return true;
}
bool CRDTMediaStream::ParseSDPAttribute_fmtp(char * sdpParam)
{
	if(strncmp(sdpParam, "a=fmtp:", 7) != 0)
		return false;

	sdpParam += 7;
	while(isdigit(*sdpParam)) ++sdpParam;

	while(*sdpParam != '\0' && *sdpParam != '\r' && *sdpParam != '\n')
	{
		for(char * c=sdpParam; *c!='='; ++c)
			*c = tolower(*c);

		if(sscanf(sdpParam, " config = %[^; \t\r\n]", m_fmtpConfig) == 1)
		{
		}
		else if(sscanf(sdpParam, " sprop-parameter-sets = %[^; \t\r\n]", m_fmtpSpropParameterSets) == 1)
		{
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
			++sdpParam;

		if(*sdpParam == ';') ++sdpParam; // Skip the ';'
	}



	if(IsVideoStream())
	{
		if(IsVideoRMVideo())
		{
			//CUserOptions::UserOptions.outInfo.codecType[0]=kCodecTypeH264BL;
		}
		else
		{
			CUserOptions::UserOptions.outInfo.codecType[0]=kCodecTypeUnknown;
			CLog::Log.MakeLog(LL_CODEC_ERR,"codecErr.txt","@@@@real:unknown video codec!\n");
			CUserOptions::UserOptions.errorID = E_UNSUPPORTED_CODEC;
			return false;
		}

	}
	else if(IsAudioStream())
	{
		if(IsAudioRMAudio())
		{
			//HexStrToBytes(m_fmtpConfig, &m_codecHeadData, &m_codecHeadDataLen);
		}
		else
		{
			CUserOptions::UserOptions.outInfo.codecType[1]=kCodecTypeUnknown;
			CLog::Log.MakeLog(LL_CODEC_ERR,"codecErr.txt","@@@@real:unknown audio codec!\n");
			CUserOptions::UserOptions.errorID = E_UNSUPPORTED_CODEC;
			return false;
		}
		
	}

	return true;
}

bool  CRDTMediaStream::ParseSDPAttribute_rtpmap(char * sdpParam)
{
	if(sscanf(sdpParam,     "a=rtpmap: %u %[^/]/%u/%u", &m_rtpPayloadType, m_codecName, &m_rtpTimestampFrequency, &m_audioChannels) == 4
		|| sscanf(sdpParam, "a=rtpmap: %u %[^/]/%u", &m_rtpPayloadType, m_codecName, &m_rtpTimestampFrequency) == 3
		|| sscanf(sdpParam, "a=rtpmap: %u %s", &m_rtpPayloadType, m_codecName) == 2)
	{
		if(IsAudioStream())//m_audioChannels == 0)
		{
			m_audioSampleRate = m_rtpTimestampFrequency;
			if(IsAudioRMAudio())
			{
				//m_framerate = 25;//m_audioSampleRate/1024;
				if(m_audioChannels == 0) 
					m_audioChannels = 2;
				//CUserOptions::UserOptions.outInfo.codecType[1]=kCodecTypeRealAudio;

			}
			else
			{
				m_framerate = 25;
				m_audioChannels = 2;
				CUserOptions::UserOptions.outInfo.codecType[1]=kCodecTypeUnknown;
				CLog::Log.MakeLog(LL_CODEC_ERR,"codecErr.txt","@@@@only real_audio is supported,the audio codec is not supported!\n");
				CUserOptions::UserOptions.errorID = E_UNSUPPORTED_CODEC;
				return false;
			}
		}


		return true;
	}
	
	return false;
}
HX_RESULT ra_block_avail_func(void* pAvail, UINT32 ulSubStream, ra_block* block)
{
	CRDTMediaStream* mediaStream =(CRDTMediaStream*) pAvail;
	mediaStream->RecvMediaFrame((unsigned char*)block,0,0);
	return 0;
}
HX_RESULT rv_frame_avail_func(void* pAvail, UINT32 ulSubStreamNum, rv_frame* frame)
{
	CRDTMediaStream* mediaStream =(CRDTMediaStream*) pAvail;
	mediaStream->RecvMediaFrame((unsigned char*)frame,0,0);
	return 0;
}
int CRDTMediaStream::GetCodecHeadData(unsigned char * codecHead)
{
	int headLen = 0;
	if(IsAudioStream())
	{
		VORA_INIT_PARAM* param1 = (VORA_INIT_PARAM*)m_codecInitParam;
		headLen = sizeof(VORA_INIT_PARAM)+sizeof(VORA_FORMAT_INFO)+param1->format->ulOpaqueDataSize;
		if(codecHead)
		{
			VORA_INIT_PARAM* pParam = (VORA_INIT_PARAM*)codecHead;
			pParam->ulVOFOURCC	= m_fcc;
			pParam->format	= (VORA_FORMAT_INFO*)(codecHead + sizeof(VORA_INIT_PARAM));
			memcpy(pParam->format,param1->format,sizeof(VORA_FORMAT_INFO));
			pParam->format->pOpaqueData = codecHead+ sizeof(VORA_INIT_PARAM) + sizeof(VORA_FORMAT_INFO);
			memcpy(pParam->format->pOpaqueData,param1->format->pOpaqueData,param1->format->ulOpaqueDataSize);
		}
	}
	else
	{
		VORV_INIT_PARAM* param1 = (VORV_INIT_PARAM*)m_codecInitParam;
		headLen = sizeof(VORV_FORMAT_INFO)+param1->format->ulOpaqueDataSize;
		if(codecHead)
		{
			VORV_FORMAT_INFO* pParam = (VORV_FORMAT_INFO*)codecHead;
			//pParam->format	= (VORV_FORMAT_INFO*)(codecHead + sizeof(VORV_INIT_PARAM));
			memcpy(pParam,param1->format,sizeof(VORV_FORMAT_INFO));
			//pParam->pOpaqueData = codecHead+ sizeof(VORV_INIT_PARAM) + sizeof(VORV_FORMAT_INFO);
			memcpy(pParam->pOpaqueData,param1->format->pOpaqueData,param1->format->ulOpaqueDataSize);
		}
	}
	
	return headLen;
}

bool CRDTMediaStream::ParseSDPAttribute_OpaqueData(char* sdpParam)
{
	if(strncmp(sdpParam, "a=OpaqueData:", 13) != 0)
		return false;
	char* t =strchr(sdpParam,'\"');//[^\"\t\r\n]
	if(t&&sscanf(t, "\"%[^\"]", m_fmtpSpropParameterSets) == 1)
	{
		
		//SpropParameterSetsData(m_fmtpSpropParameterSets, &m_codecHeadData, &m_codecHeadDataLen);
		m_codecHeadData = new BYTE[MAX_OPAQUE_SIZE];TRACE_NEW("xxm_codecHeadDataxx=",m_codecHeadData);
		//b64_decode(m_fmtpSpropParameterSets,(char*)m_codecHeadData,&m_codecHeadDataLen);
		m_codecHeadDataLen = MAX_OPAQUE_SIZE;
		voBaseSixFourDecodeEx((BYTE*)m_fmtpSpropParameterSets,m_codecHeadData,&m_codecHeadDataLen);
		if(m_codecHeadData==NULL)
		{
			return true;
		}
		BYTE* off=(BYTE*)m_codecHeadData;
		int actualSize=m_codecHeadDataLen;
		if(IsVideoStream())
		{
			int sizebytheInfo=(off[0]<<24)|(off[1]<<16)|(off[2]<<8)|(off[3]);
			if(sizebytheInfo!=0x4D4C5449&&actualSize<sizebytheInfo)
			{
				//memset(off+actualSize,0,sizebytheInfo-actualSize);
				sprintf(CLog::formatString,"@@@@OpaqueData:actualSize(%d)<sizebytheInfo(%d)\n",actualSize,sizebytheInfo);
				CLog::Log.MakeLog(LL_SDP_ERR,"sdp_warning.txt",CLog::formatString);
			}
			//actualSize = sizebytheInfo;
		}
		
		
		m_streamHead->pOpaqueData		= m_codecHeadData;//the first two bytes are size,ignore
		m_streamHead->ulOpaqueDataLen	= actualSize;
		m_streamHead->pProperty[m_streamHead->ulNumProperties].pName = (char*)malloc(16);//new char[16];

		if(!m_streamHead->pProperty[m_streamHead->ulNumProperties].pName)
			m_streamHead->pProperty[m_streamHead->ulNumProperties].pName = (char*)malloc(16);//new char[16];
		else
			memset(m_streamHead->pProperty[m_streamHead->ulNumProperties].pName,0,16);
		
		strcpy(m_streamHead->pProperty[m_streamHead->ulNumProperties].pName,"OpaqueData");
		//m_streamHead->pProperty[m_streamHead->ulNumProperties].pName = "OpaqueData";
		m_streamHead->pProperty[m_streamHead->ulNumProperties].pValue = m_streamHead->pOpaqueData;
		m_streamHead->pProperty[m_streamHead->ulNumProperties].ulValueLen =m_streamHead->ulOpaqueDataLen;
		m_streamHead->pProperty[m_streamHead->ulNumProperties].ulType = RM_PROPERTY_TYPE_BUFFER;
		m_streamHead->ulNumProperties++;

		return true;
	}
	return false;
}
void	CRDTMediaStream::SetASMRule2Stream()
{
#define MAX_TEMP 128
	char tmpBuf[MAX_TEMP];
	
	char* t = strchr(m_asmRuleBook,'#');
	if(t==NULL)
		return;
	int index=0;
	unsigned int bandwidth;
	char* next = strchr(t+1,'#');
	while(next)
	{
		int size = next-t;
		if(size>MAX_TEMP)
			size = MAX_TEMP;
		memcpy(tmpBuf,t,size);
		char* nextBW = strstr(tmpBuf,"AverageBandwidth");//[^\"\t\r\n]
		if(nextBW&&sscanf(nextBW, "AverageBandwidth=%d", &bandwidth) == 1)
		{
			if(bandwidth==m_streamHead->ulAvgBitRate)
			{
				m_asmRule2Stream = index; 
				break;
			}
		}
		//char* tmp = (char*)malloc(128);
		//free(tmp);
		t=next;
		next = strchr(t+1,'#');
		index++;
	
	}
	
	
	
}
bool CRDTMediaStream::ParseSDPAttribute_ASMRuleBook(char * sdpParam)
{
	if(strncmp(sdpParam, "a=ASMRuleBook", 13) != 0)
		return false;
	char* t =strchr(sdpParam,'\"');//[^\"\t\r\n]
	m_asmRuleBook = new char[MAX_RULEBOOK_SIZE];TRACE_NEW("xxm_asmRuleBook=",m_asmRuleBook);
	if(t&&sscanf(t, "\"%[^\r\n]", m_asmRuleBook) == 1)
	{
		UINT32 len= strlen(m_asmRuleBook);

		if(!m_streamHead->pProperty[m_streamHead->ulNumProperties].pName)
			m_streamHead->pProperty[m_streamHead->ulNumProperties].pName = (char*)malloc(16);//new char[16];
		else
			memset(m_streamHead->pProperty[m_streamHead->ulNumProperties].pName,0,16);
		
		strcpy(m_streamHead->pProperty[m_streamHead->ulNumProperties].pName,"ASMRuleBook");

		//m_streamHead->pProperty[m_streamHead->ulNumProperties].pName = "ASMRuleBook";
		m_streamHead->pProperty[m_streamHead->ulNumProperties].pValue = (BYTE*)m_asmRuleBook;//the first two bytes are size,ignore
		m_streamHead->pProperty[m_streamHead->ulNumProperties].ulValueLen = (UINT32)len;
		m_streamHead->pProperty[m_streamHead->ulNumProperties].ulType = RM_PROPERTY_TYPE_CSTRING;
		m_streamHead->ulNumProperties++;
		return true;
	}
	return false;
}
bool CRDTMediaStream::ParseSDPAttribute_RMFF(char * sdpParam)
{
	if(strncmp(sdpParam, "a=RMFF", 6) != 0)
		return false;
	char* t =strchr(sdpParam,'\"');//[^\"\t\r\n]
	if(t&&sscanf(t, "\"%[^\"]", m_fmtpSpropParameterSets) == 1)
	{
		int len;
		//SpropParameterSetsData(m_fmtpSpropParameterSets, (BYTE**)&m_rmff, &len);
		//char out[64];
		m_rmff = new char[256];TRACE_NEW("xxm_rmff=",m_rmff);
		len = 256;
		//b64_decode(m_fmtpSpropParameterSets,m_rmff,&len);
		voBaseSixFourDecodeEx((BYTE*)m_fmtpSpropParameterSets,(BYTE*)m_rmff,&len);

		BYTE* off=(BYTE*)m_rmff;
		int numRules=(off[0]<<8)|off[1];
		if(numRules>MAX_RULE_NUM)
		{
			numRules = 16;
			CLog::Log.MakeLog(LL_SDP_ERR,"sdp_warning.txt","@@@@ASM Flag:numRules>16\n");
		}
		int actualSize=len;
		int sizebytheInfo=(numRules+1)*2;
		if(actualSize<sizebytheInfo)
		{
			//memset(off+actualSize,0,sizebytheInfo-actualSize);
			BYTE* tmp=off+actualSize;
			tmp[0]=0;
			tmp[1]=2;
			sprintf(CLog::formatString,"@@@@ASM Flag:actualSize(%d)<sizebytheInfo(%d)\n",actualSize,sizebytheInfo);
			CLog::Log.MakeLog(LL_SDP_ERR,"sdp_warning.txt",CLog::formatString);
		}

		off+=2;
		for(int i=0;i<numRules;i++,off+=2)
		{
			m_asmRule2Flag[i]=(off[0]<<8)|off[1];
		}

		//m_streamHead->pProperty[m_streamHead->ulNumProperties].pName = new char[24];

		if(!m_streamHead->pProperty[m_streamHead->ulNumProperties].pName)
			m_streamHead->pProperty[m_streamHead->ulNumProperties].pName = (char*)malloc(16);//new char[16];
		else
			memset(m_streamHead->pProperty[m_streamHead->ulNumProperties].pName,0,16);
		
		strcpy(m_streamHead->pProperty[m_streamHead->ulNumProperties].pName,"RMFF 1.0 Flags");
		//m_streamHead->pProperty[m_streamHead->ulNumProperties].pName = "RMFF 1.0 Flags";
		m_streamHead->pProperty[m_streamHead->ulNumProperties].pValue = (BYTE*)m_rmff;//the first two bytes are size,ignore
		m_streamHead->pProperty[m_streamHead->ulNumProperties].ulValueLen = (UINT32)sizebytheInfo;
		m_streamHead->pProperty[m_streamHead->ulNumProperties].ulType = RM_PROPERTY_TYPE_BUFFER;
		m_streamHead->ulNumProperties++;
		return true;
	}
	return false;
}
bool CRDTMediaStream::ParseSDPAttribute_MaxBitRate(char * sdpParam)
{
	if(strncmp(sdpParam, "a=MaxBitRate", 12) != 0)
		return false;
	char* t =strchr(sdpParam,';');//[^\"\t\r\n]
	int maxBitRate;
	if(t&&sscanf(t, ";%d", &maxBitRate) == 1)
	{
		m_streamHead->ulMaxBitRate = maxBitRate;
		return true;
	}
	return false;
}
bool CRDTMediaStream::ParseSDPAttribute_AvgBitRate(char * sdpParam)
{
	if(strncmp(sdpParam, "a=AvgBitRate", 12) != 0)
		return false;
	char* t =strchr(sdpParam,';');//[^\"\t\r\n]
	int value;
	if(t&&sscanf(t, ";%d", &value) == 1)
	{
		m_streamHead->ulAvgBitRate = value;
		return true;
	}
	return false;
}
bool CRDTMediaStream::ParseSDPAttribute_MaxPacketSize(char * sdpParam)
{
	if(strncmp(sdpParam, "a=MaxPacketSize", 15) != 0)
		return false;
	char* t =strchr(sdpParam,';');//[^\"\t\r\n]
	int value;
	if(t&&sscanf(t, ";%d", &value) == 1)
	{
		m_streamHead->ulMaxPacketSize = value;
		return true;
	}
	return false;
}
bool CRDTMediaStream::ParseSDPAttribute_AvgPacketSize(char * sdpParam)
{
	if(strncmp(sdpParam, "a=AvgPacketSize", 15) != 0)
		return false;
	char* t =strchr(sdpParam,';');//[^\"\t\r\n]
	int value;
	if(t&&sscanf(t, ";%d", &value) == 1)
	{
		m_streamHead->ulAvgPacketSize = value;
		return true;
	}
	return false;
}
bool CRDTMediaStream::ParseSDPAttribute_Preroll(char * sdpParam)
{
	if(strncmp(sdpParam, "a=Preroll", 9) != 0)
		return false;
	char* t =strchr(sdpParam,';');//[^\"\t\r\n]
	int value;
	if(t&&sscanf(t, ";%d", &value) == 1)
	{
		m_streamHead->ulPreroll = value;
		return true;
	}
	return false;
}
bool CRDTMediaStream::ParseSDPAttribute_mimetype(char * sdpParam)
{
	if(strncmp(sdpParam, "a=mimetype", 10) != 0)
		return false;
	char* t =strchr(sdpParam,'\"');//[^\"\t\r\n]

	if(t&&sscanf(t, "\"%[^\"\t\r\n]", m_mimeType) == 1)
	{
		m_streamHead->pMimeType = m_mimeType;
		return true;
	}
	return false;
}
bool CRDTMediaStream::ParseSDPAttribute_control(char * sdpParam)
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
				int pos = CUserOptions::UserOptions.sdpURLLength;
				if(sscanf(pRTSPLink+pos, "/%s", m_controlAttr) != 1)
					break;
			}
		}
		else if(sscanf(sdpParam, "a=control: %s", m_controlAttr) != 1)
		{
			CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse a=control=2\n");
			CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt",m_controlAttr);
			break;
		}
		sprintf(CLog::formatString,"%s\n",m_controlAttr);
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt",CLog::formatString);
		return true;

	}while(0);
	//CLog::Log.MakeLog(LL_SDP_ERR,"flow.txt","@@@@fail to parse a=control=2\n");
	return false;
}

bool CRDTMediaStream::ParseSDPAttribute_StreamName(char * sdpParam)
{
	if(strncmp(sdpParam, "a=StreamName", 12) != 0)
		return false;
	char* t =strchr(sdpParam,'\"');//[^\"\t\r\n]

	if(t&&sscanf(t, "\"%[^\"\t\r\n]", m_streamName) == 1)
	{
		m_streamHead->pStreamName = m_streamName;
		return true;
	}
	return false;
}

bool CRDTMediaStream::Init(char * &sdpParam)
{
	m_mediaAddr = new struct sockaddr_storage;TRACE_NEW("xxm_mediaAddr=",m_mediaAddr);
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
		if(ParseSDPAttribute_RMFF(sdpParam))
			continue;
		if(ParseSDPAttribute_ASMRuleBook(sdpParam))
			continue;
		if(ParseSDPAttribute_OpaqueData(sdpParam))
			continue;
		if(ParseSDPAttribute_MaxBitRate(sdpParam))
			continue;
		if(ParseSDPAttribute_AvgBitRate(sdpParam))
			continue;
		if(ParseSDPAttribute_MaxPacketSize(sdpParam))
			continue;
		if(ParseSDPAttribute_AvgPacketSize(sdpParam))
			continue;
		if(ParseSDPAttribute_Preroll(sdpParam))
			continue;
		if(ParseSDPAttribute_mimetype(sdpParam))
			continue;
		if(ParseSDPAttribute_StreamName(sdpParam))
			continue;
	}
	UINT32 duration = m_mediaStreamEndTime;
	if(duration==0)//live
		duration = 72*3600;//72 hours

	int duration2 = m_mediaStreamEndTime*1000;
	m_streamHead->ulDuration  = duration2;//(UINT32)(duration*1000);
	m_streamHead->ulStartTime = 0;//(UINT32)m_mediaStreamStartTime;

	m_streamHead->ulStreamNumber = GetStreamID();
	SetASMRule2Stream();

	if(IsVideoStream())
	{	
		//m_videoFrame	= new struct rv_frame_struct();
		m_depack = rv_depack_create(this,
			rv_frame_avail_func,
			NULL,
			NULL);
		int ret = rv_depack_init(m_depack,m_streamHead);
		if(ret)
		{
			CLog::Log.MakeLog(LL_SDP_ERR,"rtsp.txt","@@@@video:rv_depack_init fail \n");
			return false;
		}
		m_codecInitParam = calloc(1,sizeof(VORV_INIT_PARAM));
		VORV_INIT_PARAM* param = (VORV_INIT_PARAM*)	m_codecInitParam;
	
		rv_format_info* format=NULL;
		m_fcc = rv_depack_get_codec_4cc(m_depack);
		if(rv_depack_get_codec_init_info(m_depack,&format))
		{
			CLog::Log.MakeLog(LL_SDP_ERR,"codec.txt","@@@@video:rv_depack_get_codec_init_info fails\n");
			return false;
		}
		m_framerate   = (format->ufFramesPerSecond)>>16;
		if(m_framerate>0&&m_framerate<35)
			m_nTimePerFrame = 1000/m_framerate; 
		param->format = (VORV_FORMAT_INFO*)format;
		m_videoWidth	= format->usWidth;
		m_videoHeight	= format->usHeight;
		CUserOptions::UserOptions.outInfo.clip_width	= m_videoWidth;
		CUserOptions::UserOptions.outInfo.clip_height	= m_videoHeight;
		sprintf(CLog::formatString,"video_fcc=0x%X\n",m_fcc);
		CLog::Log.MakeLog(LL_SDP_ERR,"rtsp.txt",CLog::formatString);


	}
	else if(IsAudioStream())
	{
		//m_audioFrame	= new struct ra_block_struct();
		m_depack = ra_depack_create(this,
			ra_block_avail_func,
			NULL,
			NULL);
		int ret = ra_depack_init(m_depack,m_streamHead);
		if(ret)
		{
			sprintf(CLog::formatString,"@@@@warning:ra_depack_init fail_%d \n",ret);
			CLog::Log.MakeLog(LL_SDP_ERR,"rtsp.txt",CLog::formatString);
			return false;
		}
		m_codecInitParam = calloc(1,sizeof(VORA_INIT_PARAM));
		VORA_INIT_PARAM* param = (VORA_INIT_PARAM*)	m_codecInitParam;
		ret=ra_depack_get_num_substreams(m_depack);
		if(ret>1)
		{
			CLog::Log.MakeLog(LL_SDP_ERR,"codec.txt","@@@@ra_depack_get_num_substreams(m_depack)>1\n");
		}
		param->ulVOFOURCC = m_fcc = ra_depack_get_codec_4cc(m_depack, 0);
		ra_format_info* format=NULL;
		if(ra_depack_get_codec_init_info(m_depack,0,&format))
		{
			CLog::Log.MakeLog(LL_SDP_ERR,"codec.txt","@@@@ra_depack_get_codec_init_info fails\n");
			return false;
		}
		param->format = (VORA_FORMAT_INFO*)format;
		m_audioBits = 16;
		m_audioSampleRate = format->ulSampleRate;
		m_audioChannels	  = format->usNumChannels;
		sprintf(CLog::formatString,"audio fcc=0x%X\n",m_fcc);
		CLog::Log.MakeLog(LL_SDP_ERR,"rtsp.txt",CLog::formatString);

	}

	do
	{

		CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
		if(pRTSPMediaStreamSyncEngine == NULL)
			break;
		//herein,the rtpClientPort is rdtClientPort actually,it makes no sense except that for the request of SETUP
		m_clientRTPPort = pRTSPMediaStreamSyncEngine->CreateRDTParser(this);
		if(m_clientRTPPort==0)
			break;
		pRTSPMediaStreamSyncEngine->AddMediaStream(this);

		m_mediaFrameSink = new CRDTMediaFrameSink(this);TRACE_NEW("xxm_mediaFrameSink=",m_mediaFrameSink);
		if(m_mediaFrameSink == NULL)
			break;

		m_streamSyncWallClockTime = new struct votimeval;TRACE_NEW("xxm_streamSyncWallClockTime=",m_streamSyncWallClockTime);
		if(m_streamSyncWallClockTime == NULL)
			break;

		m_streamSyncWallClockTime->tv_sec = m_streamSyncWallClockTime->tv_usec = 0;
		m_baseSyncWallClock = new struct votimeval;TRACE_NEW("xxm_baseSyncWallClock=",m_baseSyncWallClock);
		if(m_baseSyncWallClock == NULL)
			break;
		m_baseSyncWallClock->tv_sec = m_baseSyncWallClock->tv_usec = 0;
		m_streamWallClockTime = new struct votimeval;TRACE_NEW("xxm_streamWallClockTime=",m_streamWallClockTime);
		if(m_streamWallClockTime == NULL)
			break;
		m_streamWallClockTime->tv_sec = m_streamWallClockTime->tv_usec = 0;
		
		return true;

	}while(0);

	return false;
}
bool	CRDTMediaStream::IsAppStream()
{
	return _stricmp(m_mediaName, "application") == 0;
}
unsigned long CRDTMediaStream::HaveBufferedMediaStreamTime()
{
	return m_mediaFrameSink->HaveBufferedTime();
}
void CRDTMediaFrameSink::SinkMediaFrame(unsigned char * frameData, int frameSize, unsigned long startTime, unsigned long endTime,  bool isIntraFrame,int flag)
{
	CAutoCriticalSec autoLock(m_criticalSec);
	int alignment = (16-(frameSize&15));
	int actualSize = frameSize+alignment;
	do
	{
		CMemBlock * memBlock = NULL;
		_List_memBlock_T::iterator iter;
		for(iter=m_listMemBlock.begin(); iter!=m_listMemBlock.end(); ++iter)
		{
			if((*iter)->IsMemSufficient())
			{
				memBlock = *iter;
				break;
			}
		}

		if(memBlock == NULL)
		{
			memBlock = new CMemBlock();TRACE_NEW("xxm_memBlock=",memBlock);
			if(memBlock == NULL)
				break;

			if(!memBlock->Allocate(MEM_BLOCK_SIZE))
				break;

			m_listMemBlock.push_back(memBlock);
		}

		if((unsigned int)memBlock->AvailMemSize() < (sizeof(CMediaFrame) + actualSize))
		{
			memBlock->MemDeficient();
			continue;
		}
		if(isIntraFrame)//video
		{
			rv_frame* frame = (rv_frame*)frameData;
			//frameSize = frame->ulDataLen+sizeof(rv_frame)+frame->ulNumSegments*sizeof(rv_segment);

			isIntraFrame = frame->usFlags!=0;
			m_lastMediaFrame.frameSize = actualSize;
			m_lastMediaFrame.index = m_mediaFrameIndex++;
			m_lastMediaFrame.startTime = frame->ulTimestamp;
			m_lastMediaFrame.seqNum = 0;//frame->ulTimestamp+1;
			m_lastMediaFrame.isIntraFrame = isIntraFrame;
			int sizeHead = sizeof(CMediaFrame);
			memBlock->Write(&m_lastMediaFrame,sizeHead);

			unsigned char * pWrite = memBlock->WritePointer(); 
			memBlock->ForwardWritePointer(actualSize);

			memcpy(pWrite,frameData,sizeof(rv_frame));
			rv_frame* dstFrame = (rv_frame*)pWrite;
			dstFrame->pData			= pWrite+sizeof(rv_frame) + frame->ulNumSegments * sizeof(rv_segment);
			dstFrame->pSegment		= (rv_segment*)(pWrite+sizeof(rv_frame));
			//some rv_segment
			memcpy((unsigned char*)dstFrame->pSegment,frame->pSegment,frame->ulNumSegments * sizeof(rv_segment));
			//frame raw data
			memcpy(dstFrame->pData,frame->pData,frame->ulDataLen);
			if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
			{
				
				char* format = CLog::formatString;
				sprintf(format,"%ld,s=%d,no=%u,st=%lu+%lu,I=%d,all=%d\n",m_lastMediaFrame.index,
					frameSize,frame->usSequenceNum,frame->ulTimestamp,startTime,isIntraFrame,
					
					HaveSinkedMediaFrameCount());
				CLog::Log.MakeLog(LL_VIDEO_TIME,"videoTime.txt",format);
			}
		}
		else
		{
			ra_block* frame=(ra_block*)frameData;
			
			m_lastMediaFrame.frameSize = actualSize;
			m_lastMediaFrame.index = m_mediaFrameIndex++;
			m_lastMediaFrame.startTime = frame->ulTimestamp;
			m_lastMediaFrame.seqNum = 0;//frame->ulTimestamp+1;
			m_lastMediaFrame.isIntraFrame = 0;
			int sizeHead = sizeof(CMediaFrame);
			memBlock->Write(&m_lastMediaFrame, sizeHead);

			unsigned char * pWrite = memBlock->WritePointer(); 
			memBlock->ForwardWritePointer(actualSize);

			memcpy(pWrite,frameData,sizeof(ra_block));
			ra_block* dstFrame = (ra_block*)pWrite;
			dstFrame->pData = pWrite+sizeof(ra_block);
			memcpy(dstFrame->pData,frame->pData,frame->ulDataLen);
			if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
			{
				
				char* format = CLog::formatString;
				sprintf(format,"%ld,s=%d,st=%lu+%lu,all=%d\n",m_lastMediaFrame.index,
					frameSize,frame->ulTimestamp,startTime,
					HaveSinkedMediaFrameCount());
				CLog::Log.MakeLog(LL_AUDIO_TIME,"audioTime.txt",format);
			}
		}
		
		m_totalFrameSize+=m_lastMediaFrame.frameSize;
		++m_totalMediaFrameCount;
		++m_totalValidMediaFrameCount;

		return;

	}while(1);
}
#endif// ENABLE_REAL
