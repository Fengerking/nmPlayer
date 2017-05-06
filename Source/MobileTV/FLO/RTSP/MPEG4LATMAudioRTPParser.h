#ifndef __MPEG4LATMAUDIORTPPARSER_H__
#define __MPEG4LATMAUDIORTPPARSER_H__

#include "RTPParser.h"

class CMPEG4LATMAudioRTPParser : public CRTPParser
{
	int m_actualFrameSize;
public:
	CMPEG4LATMAudioRTPParser(CMediaStream * mediaStream, CMediaStreamSocket * rtpStreamSock);
	~CMPEG4LATMAudioRTPParser();

public:
	bool    ParseRTPPayloadHeader(CRTPPacket * rtpPacket);
	bool    ParseRTPPayloadFrame(CRTPPacket * rtpPacket); 
};

#endif //__MPEG4LATMAUDIORTPPARSER_H__