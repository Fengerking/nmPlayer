#ifndef __AMRAUDIORTPPARSER_H__
#define __AMRAUDIORTPPARSER_H__

#include "RTPParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

/**
\brief AMR-NB RTP Parser
*/
class CAMRAudioRTPParser : public CRTPParser
{
public:
	CAMRAudioRTPParser(CMediaStream * mediaStream, 
					   CMediaStreamSocket * rtpStreamSock, 
					   bool         isAMRWideband,
					   unsigned int audioChannels,
					   unsigned int fmtpOctetAlign,
					   unsigned int fmtpInterleaving,
					   unsigned int fmtpRobustSorting,
					   unsigned int fmtpCRC);
	~CAMRAudioRTPParser();

public:
	bool    ParseRTPPayloadHeader(CRTPPacket * rtpPacket);
	bool    ParseRTPPayloadFrame(CRTPPacket * rtpPacket); 	

protected:
	bool    ParseBandwidthEfficientModePayload(CRTPPacket * rtpPacket);
	bool    ParseOctetAlignedModePayload(CRTPPacket * rtpPacket);

protected:
	bool                  m_isAMRWideband;
	unsigned int          m_audioChannels;
	unsigned int          m_fmtpOctetAlign;
	unsigned int          m_fmtpInterleaving;
	unsigned int          m_fmtpRobustSorting;
	unsigned int          m_fmtpCRC;

	// The Payload Header
	unsigned char         m_CMR;
	unsigned char         m_ILL;
	unsigned char         m_ILP;
    // The Payload Table Of Contents
	unsigned char       * m_TocEntry;
	unsigned int          m_TocEntrySize;
	unsigned int          m_TocEntryIndex;

};


#ifdef _VONAMESPACE
}
#endif



#endif //__AMRAUDIORTPPARSER_H__