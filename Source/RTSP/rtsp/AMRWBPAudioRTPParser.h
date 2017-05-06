#ifndef __AMRWBPAUDIORTPPARSER_H__
#define __AMRWBPAUDIORTPPARSER_H__

#include "RTPParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

/**
\brief AMR-WB and AMR-WBP RTP Parser
*/
class CAMRWBPAudioRTPParser : public CRTPParser
{
public:
	CAMRWBPAudioRTPParser(CMediaStream * mediaStream,
						  CMediaStreamSocket * rtpStreamSock,
						  unsigned int audioChannels,
						  unsigned int fmtpOctetAlign,
						  unsigned int fmtpInterleaving);
	~CAMRWBPAudioRTPParser();

public:
	bool    ParseRTPPayloadHeader(CRTPPacket * rtpPacket);
	bool    ParseRTPPayloadFrame(CRTPPacket * rtpPacket); 

protected:
	unsigned int          m_audioChannels;
	unsigned int          m_fmtpOctetAlign;
	unsigned int          m_fmtpInterleaving;

protected: // The Payload Header
	unsigned char         m_PayloadHeader;
	unsigned char         m_ISF;
	unsigned char         m_TFI;
	unsigned char         m_L;

protected: // The Payload Table Of Contents
	unsigned char       * m_TocEntry;
	unsigned int          m_TocEntryCount;
	unsigned int          m_TocEntryIndex;
	unsigned int          m_TocEntryFrameIndex;

};

#ifdef _VONAMESPACE
}
#endif

#endif //__AMRWBPAUDIORTPPARSER_H__
