#include <memory.h>

#include "MediaStream.h"
#include "MediaStreamSocket.h"
#include "RTPPacket.h"
#include "H263VideoRTPParser.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

// RFC2190

CH263VideoRTPParser::CH263VideoRTPParser(CMediaStream * mediaStream, CMediaStreamSocket * rtpStreamSock)
: CRTPParser(mediaStream, rtpStreamSock)
{
	m_bHeadIsReady = false;	
}

CH263VideoRTPParser::~CH263VideoRTPParser()
{

}

bool CH263VideoRTPParser::ParseRTPPayloadHeader(CRTPPacket * rtpPacket)
{
	do
	{
		int frameHeaderSize = 2;
		int packetSize = rtpPacket->RTPDataSize();
		if( packetSize< frameHeaderSize)
			break;

		bool H263PayloadHeader_P = (rtpPacket->RTPData()[0] & 0x4) != 0;
		bool H263PayloadHeader_V = (rtpPacket->RTPData()[0] & 0x2) != 0;
		unsigned int H263PayloadHeader_PLEN = ((rtpPacket->RTPData()[0] & 0x1) << 5) | (rtpPacket->RTPData()[1] >> 3);

		if(H263PayloadHeader_V) 
		{
			++frameHeaderSize;
			if(rtpPacket->RTPDataSize() < frameHeaderSize)
				break;
		}

		if(H263PayloadHeader_PLEN > 0) 
		{
			frameHeaderSize += H263PayloadHeader_PLEN;
			if(rtpPacket->RTPDataSize() < frameHeaderSize)
				break;
		}

		m_firstPacketInMultiPacketFrame = H263PayloadHeader_P;

		if(m_firstPacketInMultiPacketFrame) 
		{
			m_bHeadIsReady   = true;
			frameHeaderSize -= 2;
			rtpPacket->RTPData()[frameHeaderSize] = 0;
			rtpPacket->RTPData()[frameHeaderSize + 1] = 0;
		}

		m_lastPacketInMultiPacketFrame = rtpPacket->RTPMarker();
		rtpPacket->Skip(frameHeaderSize);

		if(m_lastPacketInMultiPacketFrame&&m_bHeadIsReady==false)
		{
			
			//return false;
		}

		if(m_lastPacketInMultiPacketFrame&&m_bHeadIsReady)
			m_bHeadIsReady = false;//seek next frame head

		return true;

	}while(0);

	return false;
}

bool CH263VideoRTPParser::ParseRTPPayloadFrame(CRTPPacket * rtpPacket) 
{
	return CRTPParser::ParseRTPPayloadFrame(rtpPacket);
}



