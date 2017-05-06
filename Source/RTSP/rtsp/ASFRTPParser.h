#ifndef __ASFRTPPARSER_H__
#define __ASFRTPPARSER_H__

#include "RTPParser.h"
#include "ASFHeadParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

/**
\brief the parser of ASF
*/
class CASFRTPParser : public CRTPParser
{
public:
	CASFRTPParser(CMediaStream * mediaStream, CMediaStreamSocket * rtpStreamSock);
	~CASFRTPParser();

public:
	bool    ParseRTPPayloadHeader(CRTPPacket * rtpPacket);
	bool    ParseRTPPayloadFrame(CRTPPacket * rtpPacket);  
	bool	IsKeyFrame(){return m_bIsKeyFrame;};
	int		GetRelativeTS(){return m_relativeTS;}
	void RenderFrame(unsigned int rtpTimestamp,int subFrame,unsigned int seqNum,bool IsKeyFrame);

private:
	bool m_bIsKeyFrame;
	bool m_bIsFragment;
	int	 m_length;
	int	 m_offset;
	int	 m_relativeTS;
	int  m_duration;
	int  m_locationID;
	bool ReadMediaDataIndex(CRTPPacket* rtpPacket);
	bool ReadMediaDataIndex_Payload(CRTPPacket* rtpPacket,BYTE bMonLenType, BYTE bOimoLenType, BYTE bRepDataLenType, 
		DWORD dwPacketLen, DWORD dwPaddingLen, BYTE bPayloadLenType) ;
	unsigned char* m_head;
	int m_actualPayloadSize;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__ASFRTPPARSER_H__