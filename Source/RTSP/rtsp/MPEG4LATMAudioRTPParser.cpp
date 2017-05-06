#include <string.h>

#include "RTPPacket.h"
#include "MPEG4LATMAudioRTPParser.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CMPEG4LATMAudioRTPParser::CMPEG4LATMAudioRTPParser(CMediaStream * mediaStream, CMediaStreamSocket * rtpStreamSock)
: CRTPParser(mediaStream, rtpStreamSock),
m_actualFrameSize(0),
m_individualFrameSize(0),
m_handledSize(0),
m_framePos(m_frameDataEx)
{
	memset(m_frameDataEx, 0, 1024*256);
}
	
CMPEG4LATMAudioRTPParser::~CMPEG4LATMAudioRTPParser()
{

}

bool CMPEG4LATMAudioRTPParser::ParseRTPPayloadHeader(CRTPPacket * rtpPacket)
{
	m_firstPacketInMultiPacketFrame = m_lastPacketInMultiPacketFrame;
	m_lastPacketInMultiPacketFrame = rtpPacket->RTPMarker();
	return true;
}

bool CMPEG4LATMAudioRTPParser::ParseRTPPayloadFrame(CRTPPacket * rtpPacket) 
{
	if (!rtpPacket)
		return false;

	if((m_actualFrameSize == m_handledSize) && (m_actualFrameSize > 0))
	{
		m_actualFrameSize = 0;
		m_individualFrameSize= 0;
		m_handledSize = 0;
		m_framePos = m_frameDataEx;

		return 0;
	}	

	int payloadFrameSize = 0;
	int latmDataLengthBytes = 0;
	unsigned char * data=rtpPacket->RTPData();
	int rtpSize=rtpPacket->RTPDataSize();

	if ((m_actualFrameSize + rtpSize)>262144)
	{
		m_actualFrameSize = 0;
		return false;
	}

	if (rtpSize)
	{
		memcpy(m_frameDataEx + m_actualFrameSize, data, rtpSize);
		m_actualFrameSize += rtpSize;
		rtpPacket->Skip(rtpSize);
	}

	if(m_lastPacketInMultiPacketFrame || rtpPacket->RTPMarker())
	{
		for(int i=0; i<m_actualFrameSize; ++i) 
		{
			payloadFrameSize += m_framePos[i];
			++latmDataLengthBytes;

			if(m_framePos[i] != 0xFF) 
				break;
		}
		
		m_individualFrameSize = payloadFrameSize;		
		
		if(m_individualFrameSize > m_actualFrameSize)
		{
			m_individualFrameSize = m_actualFrameSize;
		}

		if (m_individualFrameSize)
		{
			memcpy(m_frameData,m_framePos+latmDataLengthBytes, m_individualFrameSize);
			m_frameSize = m_individualFrameSize;

			m_handledSize += latmDataLengthBytes;
			m_handledSize += m_individualFrameSize;
			m_framePos = m_frameDataEx + m_handledSize;

			m_individualFrameSize = 0;
		}

	}
	
	return true;
}

