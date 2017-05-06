#ifndef __MPEG4LATMAUDIORTPPARSER_H__
#define __MPEG4LATMAUDIORTPPARSER_H__

#include "RTPParser.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
/**
\brief Mpeg4 aac LATM format RTP Parser
*/
class CMPEG4LATMAudioRTPParser : public CRTPParser
{
	int					m_actualFrameSize;
	int					m_individualFrameSize;
	int					m_handledSize;

	unsigned char      	m_frameDataEx[1024*256];
	unsigned char      *m_framePos;


public:
	CMPEG4LATMAudioRTPParser(CMediaStream * mediaStream, CMediaStreamSocket * rtpStreamSock);
	~CMPEG4LATMAudioRTPParser();

public:
	bool    ParseRTPPayloadHeader(CRTPPacket * rtpPacket);
	bool    ParseRTPPayloadFrame(CRTPPacket * rtpPacket); 
};

#ifdef _VONAMESPACE
}
#endif

#endif //__MPEG4LATMAUDIORTPPARSER_H__