#ifndef __DIMSRTPPARSER_H__
#define __DIMSRTPPARSER_H__

#include "RTPParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

enum{
	DIMS_PCK_TYPE_AGGREGATION		= 0,
	DIMS_PCK_TYPE_FRAGMENT_START	= 1,
	DIMS_PCK_TYPE_FRAGMENT_MIDDLE	= 2,
	DIMS_PCK_TYPE_FRAGMENT_END		= 3,

};
class CDIMSRTPParser : public CRTPParser
{
public:
	CDIMSRTPParser(CMediaStream * mediaStream, CMediaStreamSocket * rtpStreamSock);
	~CDIMSRTPParser();

public:
	bool    ParseRTPPayloadHeader(CRTPPacket * rtpPacket);
	bool    ParseRTPPayloadFrame(CRTPPacket * rtpPacket);    

private:
	bool	m_bHeadIsReady;
	int	    m_nRAP;
	int		m_nType;
	int		m_nCTR;

};














#ifdef _VONAMESPACE
}
#endif


#endif //__DIMSRTPPARSER_H__
