#ifndef __RTPPACKETREORDER_H__
#define __RTPPACKETREORDER_H__

#include "list_T.h"

#define MAX_RTP_PACKETS_IN_BUFFER 50

class CRTPPacket;

class CRTPPacketReorder
{
public:
	CRTPPacketReorder();
	~CRTPPacketReorder();

	void Reset();

public:
	CRTPPacket * GetEmptyRTPPacket();
	void         ReorderRTPPacket(CRTPPacket * rtpPacket);
	CRTPPacket * GetNextReorderedRTPPacket(bool & packetLoss);
	void         ReleaseRTPPacket(CRTPPacket * rtpPacket);
	void         ReleaseAllRTPPackets();

private:
	list_T<CRTPPacket *, allocator<CRTPPacket *, MAX_RTP_PACKETS_IN_BUFFER> > m_listEmptyRTPPacket;
	list_T<CRTPPacket *, allocator<CRTPPacket *, MAX_RTP_PACKETS_IN_BUFFER> > m_listReorderedRTPPacket;

private:
	bool                 m_haveSeenInitialSeqNum;
	unsigned short       m_nextExpectedSeqNum;
	int					 m_actualPackets;

public:
	int			GetStoredPacket(){return m_listReorderedRTPPacket.size();};
};

extern bool SeqNumLT(unsigned short seqNum1, unsigned short seqNum2); 


#endif //__RTPPACKETREORDER_H__

