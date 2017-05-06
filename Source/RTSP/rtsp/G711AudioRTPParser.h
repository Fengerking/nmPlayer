#ifndef __G711AUDIORTPPARSER_H__
#define __G711AUDIORTPPARSER_H__

#include "RTPParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

/**
\brief AMR-NB RTP Parser
*/
class CG711AudioRTPParser : public CRTPParser
{
public:
	CG711AudioRTPParser(CMediaStream * mediaStream, 
					   CMediaStreamSocket * rtpStreamSock);
	~CG711AudioRTPParser();

public:
	bool    ParseRTPPayloadHeader(CRTPPacket * rtpPacket);
	bool    ParseRTPPayloadFrame(CRTPPacket * rtpPacket); 

};


#ifdef _VONAMESPACE
}
#endif



#endif //__G711AUDIORTPPARSER_H__