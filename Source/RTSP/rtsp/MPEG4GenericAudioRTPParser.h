#ifndef __MPEG4GENERICAUDIORTPPARSER_H__
#define __MPEG4GENERICAUDIORTPPARSER_H__

#include "RTPParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class AUHeader 
{
public:
	AUHeader()
	{
		size = 0;
		index = 0;
	}

public:
	unsigned int size;
	unsigned int index;
};
/**
\brief Mpeg4 aac generic format RTP Parser
*/
class CMPEG4GenericAudioRTPParser : public CRTPParser
{
public:
	CMPEG4GenericAudioRTPParser(CMediaStream * mediaStream, 
							    CMediaStreamSocket * rtpStreamSock,
								unsigned int sizeLength, 
								unsigned int indexLength, 
								unsigned int indexDeltaLength);

	~CMPEG4GenericAudioRTPParser();

public:
	bool    ParseRTPPayloadHeader(CRTPPacket * rtpPacket);
	bool    ParseRTPPayloadFrame(CRTPPacket * rtpPacket); 

protected:
	unsigned int m_sizeLength; 
	unsigned int m_indexLength; 
	unsigned int m_indexDeltaLength;

private:
	AUHeader   * m_auHeaderArray;
	unsigned int m_auHeaderCount;
	unsigned int m_auHeaderIndex;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__MPEG4GENERICAUDIORTPPARSER_H__

