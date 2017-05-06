#include <string.h>

#include "RTPPacket.h"
#include "MPEG4LATMAudioRTPParser.h"

CMPEG4LATMAudioRTPParser::CMPEG4LATMAudioRTPParser(CMediaStream * mediaStream, CMediaStreamSocket * rtpStreamSock)
: CRTPParser(mediaStream, rtpStreamSock),
m_actualFrameSize(0)
{

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
	if(rtpPacket->RTPDataSize() == 0)
		return false;

	int payloadFrameSize = 0;
	int latmDataLengthBytes = 0;
	unsigned char * data=rtpPacket->RTPData();
	int rtpSize=rtpPacket->RTPDataSize();
	if(rtpSize+m_frameSize<=m_actualFrameSize)
	{
		payloadFrameSize=rtpSize;
	}
	else
	{
		for(int i=0; i<rtpSize; ++i) 
		{
			payloadFrameSize += data[i];
			++latmDataLengthBytes;

			if(data[i] != 0xFF) 
				break;
		}   
		rtpPacket->Skip(latmDataLengthBytes);
		rtpSize=rtpPacket->RTPDataSize();
	}
	if(rtpPacket->RTPMarker())
		m_actualFrameSize=0;
	else if(payloadFrameSize > rtpSize)
	{
		m_actualFrameSize=payloadFrameSize; 
		payloadFrameSize = rtpSize;
	}
	
	
	

	memcpy(m_frameData + m_frameSize, rtpPacket->RTPData(), payloadFrameSize);
	m_frameSize += payloadFrameSize;
	rtpPacket->Skip(payloadFrameSize);

	return true;
}

