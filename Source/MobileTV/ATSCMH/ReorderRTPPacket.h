#ifndef __RTPPACKETREORDER_H__
#define __RTPPACKETREORDER_H__

#include "list_T.h"

class CRTPPacket;

class CReorderRTPPacket
{
public:
	CReorderRTPPacket();
	~CReorderRTPPacket();

public:
	void Reset();

public:
	CRTPPacket * GetEmptyRTPPacket();
	void         ReorderRTPPacket(CRTPPacket * pRTPPacket);
	CRTPPacket * GetNextReorderedRTPPacket(bool & packetLoss);
public:
	void         ReleaseRTPPacket(CRTPPacket * pRTPPacket);
	void         ReleaseAllRTPPackets();

private:
	list_T<CRTPPacket *, allocator<CRTPPacket *, 10> > m_listEmptyRTPPacket;
	list_T<CRTPPacket *, allocator<CRTPPacket *, 10> > m_listReorderedRTPPacket;

private:
	bool                 m_haveSeenInitialSeqNum;
	unsigned short       m_nextExpectedSeqNum;
};


bool SeqNumLessThan(unsigned short seqNum1, unsigned short seqNum2);


#endif //__RTPPACKETREORDER_H__