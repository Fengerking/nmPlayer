#ifndef __QCELPAUDIORTPPARSER_H__
#define __QCELPAUDIORTPPARSER_H__

#include "RTPParser.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
/**
\brief QCELP RTP Parser
*/
class CQcelpAudioRTPParser : public CRTPParser
{
public:
	CQcelpAudioRTPParser(CMediaStream * mediaStream, 
						 CMediaStreamSocket * rtpStreamSock);
	~CQcelpAudioRTPParser();

public:
	bool    ParseRTPPayloadHeader(CRTPPacket * rtpPacket);
	bool    ParseRTPPayloadFrame(CRTPPacket * rtpPacket);  	

protected:
	unsigned char m_interleaveL;
	unsigned char m_interleaveN;

};

#ifdef _VONAMESPACE
}
#endif

#endif //__QCELPAUDIORTPPARSER_H__