
#include "utility.h"
#include "RTPPacket.h"
#include "ASFRTPParser.h"
#include "MediaStream.h"
#include "RTSPMediaStreamSyncEngine.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CASFRTPParser::CASFRTPParser(CMediaStream * mediaStream, CMediaStreamSocket * rtpStreamSock)
: CRTPParser(mediaStream, rtpStreamSock),
m_bIsKeyFrame(false),
m_bIsFragment(false),
m_length(0),
m_offset(0),
m_relativeTS(0),
m_duration(0),
m_locationID(0),
m_head(NULL),
m_actualPayloadSize(0)
{
	
}

CASFRTPParser::~CASFRTPParser()
{

}
bool CASFRTPParser::ReadMediaDataIndex(CRTPPacket * rtpPacket) {
	BYTE bRead;
	m_head = rtpPacket->RTPData();

	read_byte(bRead);
	if(bRead & 128) {	//0x10000000, start with error correction data
		//if the data packet start with error correction data, we have not processed it.

		skip(bRead & 15);			//0x00001111
		read_byte(bRead);
	}

	//process "length type flags"
	bool bMultiPayload = bRead & 1;
	BYTE bSeqType = BYTE(bRead << 5) >> 6;
	BYTE bPaddingLenType = BYTE(bRead << 3) >> 6;
	BYTE bPacketLenType = BYTE(bRead << 1) >> 6;

	//process "property flags"
	read_byte(bRead);
	BYTE bRepDataLenType = BYTE(bRead << 6) >> 6;
	BYTE bOimoLenType = BYTE(bRead << 4) >> 6;
	BYTE bMonLenType = BYTE(bRead << 2) >> 6;

	//process "packet length"
	DWORD dwPacketLen = 0;
	read_by_type(dwPacketLen, bPacketLenType);

	//process "sequence", skip
	skip_by_type(bSeqType);

	//process "padding length", skip
	DWORD dwPaddingLen = 0;
	read_by_type(dwPaddingLen, bPaddingLenType);
	sprintf(CLog::formatString,"packLen=%ld,bMultiPayload=%d,pad=%ld,ts=%d\n",dwPacketLen,bMultiPayload,dwPaddingLen,rtpPacket->RTPTimestamp());
	CLog::Log.MakeLog(LL_VIDEO_TIME,"wmv.txt",CLog::formatString);
	//process "send time", the send time of this packet, in millisecond units
	skip(sizeof(DWORD) + sizeof(WORD));
	if(bMultiPayload)
	{
		BYTE bFlags;
		read_byte(bFlags);
		BYTE bPayloadNum = BYTE(bFlags << 2) >> 2;
		BYTE bPayloadLenType = bFlags >> 6;
		for(BYTE i = 0; i < bPayloadNum; i++)
		{
			if(!ReadMediaDataIndex_Payload(rtpPacket,bMonLenType, bOimoLenType, bRepDataLenType, dwPacketLen, dwPaddingLen, bPayloadLenType)) {
				return false;
			}
		}

		//payload data padding data
		//DWORD dwDist = 0;
		//m_chunk.FGetDistanceFromRecordPoint(&dwDist);
		//if(m_dwPacketSize && m_dwPacketSize > dwDist + dwPaddingLen) {
		//	skip(m_dwPacketSize - dwDist - dwPaddingLen);
		//}
	}
	else
	{
		if(!ReadMediaDataIndex_Payload(rtpPacket,bMonLenType, bOimoLenType, bRepDataLenType, dwPacketLen, dwPaddingLen, 0)) {
			return false;
		}
	}

	//padding data
	if(dwPaddingLen > 0) {
		skip(dwPaddingLen);
	}

	return true;
}

bool CASFRTPParser::ReadMediaDataIndex_Payload(CRTPPacket* rtpPacket,BYTE bMonLenType, BYTE bOimoLenType, BYTE bRepDataLenType, 
											DWORD dwPacketLen, DWORD dwPaddingLen, BYTE bPayloadLenType) 
{
	//rtpPacket->Skip(m_head-rtpPacket->RTPData());
	//stream number
	BYTE bStreamNum;
	int  isKey;
	read_byte(bStreamNum);
	if(0)//bStreamNum>5)
	{
		CLog::Log.MakeLog(LL_RTP_ERR,"wmv.txt","error:bStreamNum>5\n");
		return false;
	}
	isKey = (bStreamNum&0x000000ff)>>7;
	bStreamNum=bStreamNum & 127;
	
	//media object number
	DWORD dwMediaObjNo = 0;
	read_by_type(dwMediaObjNo, bMonLenType);

	//offset into media object or presentation time
	DWORD dwMediaObjOffset = 0;
	read_by_type(dwMediaObjOffset, bOimoLenType);

	//replicated data
	DWORD dwRepDataLen = 0;
	read_by_type(dwRepDataLen, bRepDataLenType);
	DWORD dwMediaObjSize = 0;
	DWORD dwMediaObjTime = 0;
	if(dwRepDataLen == 1) {	//compressed
		int bPreTimeDelta = 0;
		read_byte(bPreTimeDelta);
		bPreTimeDelta=bPreTimeDelta*m_mediaStream->RTPTimestampFrequency()/1000;
		DWORD dwPayloadLen = 0;
		DWORD dwInternalPaddingLen = 0;
		if(bPayloadLenType == 0) {
			//dwInternalPaddingLen = dwPacketLen ? m_dwPacketSize - dwPacketLen : 0;
			//DWORD dwDist = 0;
			//m_chunk.FGetDistanceFromRecordPoint(&dwDist);
			//dwPayloadLen = (m_dwPacketSize ? m_dwPacketSize : dwPacketLen) - 
			//	(dwDist + dwPaddingLen + dwInternalPaddingLen);
			m_actualPayloadSize = m_length-4-(m_head-rtpPacket->RTPData());;
		}
		else {
			read_by_type(m_actualPayloadSize, bPayloadLenType);
		}

		//get media data at this position
		DWORD dwPacketRead = 0;
		BYTE bLen = 0;
		int nIdx = 0;
		dwPayloadLen=m_actualPayloadSize;
		int timeStamp=dwMediaObjOffset;//rtpPacket->RTPTimestamp();
		while(dwPacketRead < dwPayloadLen) {
			read_byte(bLen);
			dwPacketRead += (bLen + 1);
			FrameData frame;
			frame.data=m_head;
			frame.len = bLen;
			frame.isKeyFrame = isKey;//IsKeyFrame();
			frame.seqNum	 = rtpPacket->RTPSeqNum();
			frame.timestamp	 = timeStamp;
			frame.isFragment = 0;//dwMediaObjOffset>0;
			CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
			pRTSPMediaStreamSyncEngine->RecvMediaFrame(bStreamNum,&frame);
			
			sprintf(CLog::formatString,"%d_compress:isFragment=%d,seq=%d,ts=%lu,TimeDelta=%d\n",nIdx,frame.isFragment,frame.seqNum,frame.timestamp,bPreTimeDelta);
			CLog::Log.MakeLog(LL_VIDEO_TIME,"wmv.txt",CLog::formatString);
			
			skip(bLen);
			timeStamp+=bPreTimeDelta;
			nIdx++;
		}

		if(dwInternalPaddingLen > 0) {
			skip(dwInternalPaddingLen);
		}
		
	}
	else {
		
		read_dword(dwMediaObjSize);
		
		read_dword(dwMediaObjTime);
		if(dwRepDataLen > 8) {
			skip(dwRepDataLen - 8);
		}

		if(bPayloadLenType == 0) {
			/*
			dwInternalPaddingLen = dwPacketLen ? m_dwPacketSize - dwPacketLen : 0;
			DWORD dwDist = 0;
			m_chunk.FGetDistanceFromRecordPoint(&dwDist);
			dwPayloadLen = (m_dwPacketSize ? m_dwPacketSize : dwPacketLen) - 
				(dwDist + dwPaddingLen + dwInternalPaddingLen);
			*/
			m_actualPayloadSize = m_length-4-(m_head-rtpPacket->RTPData());
		}
		else {
			read_by_type(m_actualPayloadSize, bPayloadLenType);
		}

		FrameData frame;
		frame.data=m_head;
		frame.len = m_actualPayloadSize;
		frame.isKeyFrame = isKey;//IsKeyFrame();
		frame.seqNum	 = rtpPacket->RTPSeqNum();
		frame.timestamp	 = dwMediaObjTime;//rtpPacket->RTPTimestamp();
		frame.isFragment = dwMediaObjOffset>0;
		CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
		pRTSPMediaStreamSyncEngine->RecvMediaFrame(bStreamNum,&frame);
		sprintf(CLog::formatString,"isFragment=%d,seq=%d,ts=%lu,",frame.isFragment,frame.seqNum,frame.timestamp);
		CLog::Log.MakeLog(LL_VIDEO_TIME,"wmv.txt",CLog::formatString);
		skip(m_actualPayloadSize);			
	}
	
	int offset=m_head-rtpPacket->RTPData();
	rtpPacket->Skip(offset);
	
	sprintf(CLog::formatString,"bStreamNum=%d,dwMediaObjNo=%ld,dwMediaObjOffset=%ld,dwRepDataLen=%ld,dwMediaObjSize=%ld,dwMediaObjTime=%ld,actual=%d\n",
		bStreamNum,dwMediaObjNo,dwMediaObjOffset,dwRepDataLen,dwMediaObjSize,dwMediaObjTime,m_actualPayloadSize);
	CLog::Log.MakeLog(LL_VIDEO_TIME,"wmv.txt",CLog::formatString);
	
	return true;
}

bool CASFRTPParser::ParseRTPPayloadHeader(CRTPPacket * rtpPacket)
{
	int frameHeaderSize = 0;
	unsigned char *head=rtpPacket->RTPData();
	m_bIsKeyFrame = head[0]&0x80;
	m_bIsFragment = (head[0]&0x40)==0;
	bool hasRT	  = head[0]&0x20;
	bool hasDuration	= head[0]&0x10;
	bool hasLocationID	= head[0]&0x08;

	head++;
	
	if(m_bIsFragment)
	{
		m_offset = BIG2L_3(head);
		m_length = 0;
	}
	else
	{
		m_length = BIG2L_3(head);
		m_offset = 0;
	}
	frameHeaderSize+=4;
	head+=3;
	if(hasRT)
	{
		m_relativeTS = BIG2L_4(head);
		head+=4;
		frameHeaderSize+=4;
	}
	else
	{
		m_relativeTS = 0;
	}
	if(hasDuration)
	{
		m_duration = BIG2L_4(head);
		head+=4;
		frameHeaderSize+=4;
	}
	else
	{
		m_duration = 0;
	}
	if(hasLocationID)
	{
		m_locationID = BIG2L_4(head);
		head+=4;
		frameHeaderSize+=4;
	}
	else
	{
		m_locationID = 0;
	}
	rtpPacket->Skip(frameHeaderSize);

	return true;
}

bool CASFRTPParser::ParseRTPPayloadFrame(CRTPPacket * rtpPacket) 
{
	int rtpPackSize=rtpPacket->RTPDataSize();
	
	if(rtpPackSize<=0)
		return false;
	int payloadFrameSize;
	if(!m_bIsFragment)
	{
		ReadMediaDataIndex(rtpPacket);
		m_lastPacketInMultiPacketFrame = false;
		if(rtpPacket->RTPDataSize()>4)//there are other packets
		{
			//ParseRTPPayloadHeader(rtpPacket);
			//if(rtpPacket->RTPDataSize()>4)//there are other packets
			{
				sprintf(CLog::formatString,"!!Warn:The RTP packet is combound,left=%d\n",rtpPacket->RTPDataSize());
				CLog::Log.MakeLog(LL_VIDEO_TIME,"wmv.txt",CLog::formatString);
				ParseRTPPayloadHeader(rtpPacket);
			}
		}
	}
	else
	{
		payloadFrameSize = rtpPackSize;
		memcpy(m_frameData + m_frameSize, rtpPacket->RTPData(), payloadFrameSize);
		m_frameSize += payloadFrameSize;
		rtpPacket->Skip(payloadFrameSize);
	}
	return true;
}

void CASFRTPParser::RenderFrame(unsigned int rtpTimestamp,int subFrame,unsigned int seqNum,bool IsKeyFrame)
{
	return;
}

