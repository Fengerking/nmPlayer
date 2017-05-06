#include <string.h>

#include "RTPPacket.h"
#include "BitVector.h"
#include "MPEG4GenericAudioRTPParser.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

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

		if(m_auHeaderArray != NULL)
		{
			SAFE_DELETE_ARRAY(m_auHeaderArray);
			m_auHeaderArray = NULL;
		}
		m_auHeaderCount = 0;
		m_auHeaderIndex = 0;

		if(m_sizeLength <= 0)
			break;

		if(rtpPacket->RTPDataSize() < 2)
			break;

		unsigned int auHeadersLength_bits  = (rtpPacket->RTPData()[0] << 8) | rtpPacket->RTPData()[1];
		unsigned int auHeadersLength_bytes = (auHeadersLength_bits + 7) / 8;
		rtpPacket->Skip(2);
		if(rtpPacket->RTPDataSize() < (int)auHeadersLength_bytes)
			break;

		int bitsCount = auHeadersLength_bits - (m_sizeLength + m_indexLength);
		if(bitsCount >= 0 && (m_sizeLength + m_indexDeltaLength) > 0) 
		{
			m_auHeaderCount = 1 + bitsCount / (m_sizeLength + m_indexDeltaLength);
		}

		if(m_auHeaderCount <= 0)
			break;

		m_auHeaderArray = new AUHeader[m_auHeaderCount];TRACE_NEW("xxm_auHeaderArray =",m_auHeaderArray);
		if(m_auHeaderCount == 0)
			break;
#if USE_REF_DEBUG_STREAM
		CBitVector bitVec(rtpPacket->RTPData(), 0, auHeadersLength_bits);
		m_auHeaderArray[0].size = bitVec.GetBits(m_sizeLength);
		m_auHeaderArray[0].index = bitVec.GetBits(m_indexLength);
		for (unsigned i = 1; i < m_auHeaderCount; ++i) 
		{
			m_auHeaderArray[i].size = bitVec.GetBits(m_sizeLength);
			m_auHeaderArray[i].index = bitVec.GetBits(m_indexDeltaLength);
		}
#else
		VORTSPBitstream bitVec;
		VORTSPInitBitStream(&bitVec,rtpPacket->RTPData(), auHeadersLength_bits);
		m_auHeaderArray[0].size = VORTSPGetBits(&bitVec,m_sizeLength);
		m_auHeaderArray[0].index = VORTSPGetBits(&bitVec,m_indexLength);
		for (unsigned i = 1; i < m_auHeaderCount; ++i) 
		{
			m_auHeaderArray[i].size = VORTSPGetBits(&bitVec,m_sizeLength);
			m_auHeaderArray[i].index = VORTSPGetBits(&bitVec,m_indexDeltaLength);
		}
#endif
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

	memcpy(m_frameData + m_frameSize, rtpPacket->RTPData(), payloadFrameSize);
	m_frameSize += payloadFrameSize;
	rtpPacket->Skip(payloadFrameSize);

	return true;
}

