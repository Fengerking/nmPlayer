#ifndef __H263VIDEORTPPARSER_H__
#define __H263VIDEORTPPARSER_H__

#include "RTPParser.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
/**
\brief H263Video RTP Parser
*/
class CH263VideoRTPParser : public CRTPParser
{
public:
	CH263VideoRTPParser(CMediaStream * mediaStream, CMediaStreamSocket * rtpStreamSock);
	~CH263VideoRTPParser();

public:
	bool    ParseRTPPayloadHeader(CRTPPacket * rtpPacket);
	bool    ParseRTPPayloadFrame(CRTPPacket * rtpPacket);    

private:
	bool	m_bHeadIsReady;

};

#ifdef _VONAMESPACE
}
#endif

















#endif //__H263VIDEORTPPARSER_H__
