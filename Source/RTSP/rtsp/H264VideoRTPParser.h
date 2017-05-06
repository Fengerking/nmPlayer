#ifndef __H264VIDEORTPPARSER_H__
#define __H264VIDEORTPPARSER_H__

#include "RTPParser.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
/**
\brief H264Video RTP Parser
*/
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

#ifdef _VONAMESPACE
}
#endif

#endif //__H264VIDEORTPPARSER_H__