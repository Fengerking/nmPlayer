#ifndef __MPEG4ESVIDEORTPPARSER_H__
#define __MPEG4ESVIDEORTPPARSER_H__

#include "RTPParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

/**
\brief  Mpeg4 Video RTP Parser
*/
class CMPEG4ESVideoRTPParser : public CRTPParser
{
public:
	CMPEG4ESVideoRTPParser(CMediaStream * mediaStream, CMediaStreamSocket * rtpStreamSock);
	~CMPEG4ESVideoRTPParser();

public:
	bool    ParseRTPPayloadHeader(CRTPPacket * rtpPacket);
	bool    ParseRTPPayloadFrame(CRTPPacket * rtpPacket);  

};

#ifdef _VONAMESPACE
}
#endif










#endif //__MPEG4ESVIDEORTPPARSER_H__