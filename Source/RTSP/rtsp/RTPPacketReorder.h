#ifndef __RTPPACKETREORDER_H__
#define __RTPPACKETREORDER_H__

#include "list_T.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CRTPPacket;
#define MAX_RTP_PACKETS_IN_BUFFER 50
class CRTPParser;
class CRTPPacketReorder
{
public:
	CRTPPacketReorder(CRTPParser* rtpParser);
	~CRTPPacketReorder();

public:
	void Reset();

public:
	CRTPPacket * GetEmptyRTPPacket();
	void         ReorderRTPPacket(CRTPPacket * rtpPacket);
	CRTPPacket * GetNextReorderedRTPPacket(bool & packetLoss);
	void         ReleaseRTPPacket(CRTPPacket * rtpPacket);
	void         ReleaseAllRTPPackets();
	long			GetStoredPackTime(unsigned int frequency);
private:
	list_T<CRTPPacket *, allocator<CRTPPacket *, MAX_RTP_PACKETS_IN_BUFFER> > m_listEmptyRTPPacket;
	list_T<CRTPPacket *, allocator<CRTPPacket *, MAX_RTP_PACKETS_IN_BUFFER> > m_listReorderedRTPPacket;

private:
	bool                 m_haveSeenInitialSeqNum;
	unsigned short       m_nextExpectedSeqNum;
	int					 m_actualPackets;
	CRTPParser*		m_rtpParser;
public:
	int			GetStoredPacket(){return m_listReorderedRTPPacket.size();};
	unsigned int 		m_duplicatePacketNum;
};

extern bool SeqNumLT(unsigned short seqNum1, unsigned short seqNum2); 

#ifdef _VONAMESPACE
}
#endif

#endif //__RTPPACKETREORDER_H__

