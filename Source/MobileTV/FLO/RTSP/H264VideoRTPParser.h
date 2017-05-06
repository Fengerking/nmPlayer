#ifndef __H264VIDEORTPPARSER_H__
#define __H264VIDEORTPPARSER_H__

#include "RTPParser.h"

class CH264VideoRTPParser : public CRTPParser
{
public:
	CH264VideoRTPParser(CMediaStream * mediaStream, CMediaStreamSocket * rtpStreamSock);
	~CH264VideoRTPParser();

public:
	bool    ParseRTPPayloadHeader(CRTPPacket * rtpPacket);
	bool    ParseRTPPayloadFrame(CRTPPacket * rtpPacket);  

private:
	unsigned int m_packetNALUnitType;
};


#endif //__H264VIDEORTPPARSER_H__