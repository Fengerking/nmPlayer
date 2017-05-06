#include <string.h>

#include "BVUtility.h"
#include "RTPPacket.h"
#include "MPEG4GenericAudioRTPParser.h"
#define LOG_TAG "FLOEngine_RTPParser"
#include "voLog.h"
CMPEG4GenericAudioRTPParser::CMPEG4GenericAudioRTPParser(CMediaStream * mediaStream, 
														 CMediaStreamSocket * rtpStreamSock,
														 unsigned int sizeLength, 
														 unsigned int indexLength, 
														 unsigned int indexDeltaLength)
: CRTPParser(mediaStream, rtpStreamSock)
, m_sizeLength(sizeLength) 
, m_indexLength(indexLength)
, m_indexDeltaLength(indexDeltaLength)
{
	m_auHeaderArray = NULL;
	m_auHeaderCount = 0;
	m_auHeaderIndex = 0;
}

CMPEG4GenericAudioRTPParser::~CMPEG4GenericAudioRTPParser()
{
	if(m_auHeaderArray != NULL)
	{
		SAFE_DELETE_ARRAY(m_auHeaderArray);
		m_auHeaderArray = NULL;
	}
}

bool CMPEG4GenericAudioRTPParser::ParseRTPPayloadHeader(CRTPPacket * rtpPacket)
{
	do
	{
		m_firstPacketInMultiPacketFrame = m_lastPacketInMultiPacketFrame;
		m_lastPacketInMultiPacketFrame = rtpPacket->RTPMarker();

		if(m_sizeLength <= 0)
			break;

		if(rtpPacket->RTPDataSize() < 2)
			break;
		unsigned char *data = rtpPacket->RTPData();
		unsigned int auHeadersLength_bits  = (data[0] << 8) | data[1];
		unsigned int auHeadersLength_bytes = (auHeadersLength_bits + 7) / 8;
		rtpPacket->Skip(2);
		if(rtpPacket->RTPDataSize() < (int)auHeadersLength_bytes)
			break;

		int bitsCount = auHeadersLength_bits - (m_sizeLength + m_indexLength);
		int auHeaderCount = 0;
		if(bitsCount >= 0 && (m_sizeLength + m_indexDeltaLength) > 0) 
		{
			auHeaderCount = 1 + bitsCount / (m_sizeLength + m_indexDeltaLength);
			if(auHeaderCount <= 0)
				break;
		}

		if(auHeaderCount > m_auHeaderCount)
		{
			SAFE_DELETE_ARRAY(m_auHeaderArray);
			m_auHeaderArray = NULL;
			
			m_auHeaderArray = new AUHeader[auHeaderCount];
			if(m_auHeaderArray == NULL)
				break;
		}
		m_auHeaderCount = auHeaderCount;
		m_auHeaderIndex = 0;


		CBVUtility bitVec(rtpPacket->RTPData(), 0, auHeadersLength_bits);
		m_auHeaderArray[0].size = bitVec.GetBits(m_sizeLength);
		m_auHeaderArray[0].index = bitVec.GetBits(m_indexLength);
		for(int i=1; i<m_auHeaderCount; ++i) 
		{
			m_auHeaderArray[i].size = bitVec.GetBits(m_sizeLength);
			m_auHeaderArray[i].index = bitVec.GetBits(m_indexDeltaLength);
		}

		rtpPacket->Skip(auHeadersLength_bytes);

		return true;

	}while(0);

	return false;
}

bool CMPEG4GenericAudioRTPParser::ParseRTPPayloadFrame(CRTPPacket * rtpPacket) 
{
	if(m_auHeaderArray == NULL || m_auHeaderIndex >= m_auHeaderCount || rtpPacket->RTPDataSize() == 0)
		return false;

	int payloadFrameSize = m_auHeaderArray[m_auHeaderIndex].size; 
	if(payloadFrameSize > rtpPacket->RTPDataSize())
	{
		payloadFrameSize = rtpPacket->RTPDataSize();
	}
	++m_auHeaderIndex;
	if(m_frameSize+payloadFrameSize>MAX_FRAME_SIZE)
	{
		VOLOGE("aac:m_frameSize(%d,%d)>MAX_FRAME_SIZE",m_frameSize,payloadFrameSize);
		SLOG2(LL_RTP_ERR,"packet_loss.txt","aac:m_frameSize(%d,%d)>MAX_FRAME_SIZE",m_frameSize,payloadFrameSize);
		m_frameSize = 0;
		return false;
	}
	memcpy(m_frameData + m_frameSize, rtpPacket->RTPData(), payloadFrameSize);
	m_frameSize += payloadFrameSize;
	rtpPacket->Skip(payloadFrameSize);

	return true;
}

