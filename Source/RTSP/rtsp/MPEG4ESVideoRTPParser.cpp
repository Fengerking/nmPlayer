#include <memory.h>
#include "utility.h"
#include "MediaStream.h"
#include "MediaStreamSocket.h"
#include "RTPPacket.h"

#include "MPEG4ESVideoRTPParser.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CMPEG4ESVideoRTPParser::CMPEG4ESVideoRTPParser(CMediaStream * mediaStream, CMediaStreamSocket * rtpStreamSock)
: CRTPParser(mediaStream, rtpStreamSock)
{

}
	
CMPEG4ESVideoRTPParser::~CMPEG4ESVideoRTPParser()
{

}

bool CMPEG4ESVideoRTPParser::ParseRTPPayloadHeader(CRTPPacket * rtpPacket)
{
	m_firstPacketInMultiPacketFrame = rtpPacket->RTPDataSize() >= 4 
									  && rtpPacket->RTPData()[0] == 0 
									  && rtpPacket->RTPData()[1] == 0 
									  && rtpPacket->RTPData()[2] == 1;

	m_lastPacketInMultiPacketFrame = rtpPacket->RTPMarker();

	return true;
}

bool CMPEG4ESVideoRTPParser::ParseRTPPayloadFrame(CRTPPacket * rtpPacket) 
{
	return CRTPParser::ParseRTPPayloadFrame(rtpPacket);
}



