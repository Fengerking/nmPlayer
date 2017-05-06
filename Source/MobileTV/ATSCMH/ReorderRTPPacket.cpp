#include "RTPPacket.h"
#include "ReorderRTPPacket.h"

CReorderRTPPacket::CReorderRTPPacket()
: m_haveSeenInitialSeqNum(false)
, m_nextExpectedSeqNum(0)
{

}

CReorderRTPPacket::~CReorderRTPPacket()
{
	Reset();
}

void CReorderRTPPacket::Reset()
{
	ReleaseAllRTPPackets();

	list_T<CRTPPacket *, allocator<CRTPPacket *, 10> >::iterator iter;
	for(iter=m_listEmptyRTPPacket.begin(); iter!=m_listEmptyRTPPacket.end(); ++iter)
	{
		delete *iter;
	}
	m_listEmptyRTPPacket.clear();
}

CRTPPacket * CReorderRTPPacket::GetEmptyRTPPacket()
{
	CRTPPacket * pRTPPacket = NULL;
	do
	{
		if(!m_listEmptyRTPPacket.empty())
		{
			pRTPPacket = m_listEmptyRTPPacket.front();
			m_listEmptyRTPPacket.pop_front();
			break;
		}

		pRTPPacket = new CRTPPacket();

	}while(0);

	return pRTPPacket;
}

void CReorderRTPPacket::ReorderRTPPacket(CRTPPacket * pRTPPacket)
{
	if(!m_haveSeenInitialSeqNum)
	{
		m_haveSeenInitialSeqNum = true;
		m_nextExpectedSeqNum = pRTPPacket->SeqNum();
	}

	//if(SeqNumLT(pRTPPacket->SeqNum(), m_nextExpectedSeqNum))
	//	return;


	list_T<CRTPPacket *, allocator<CRTPPacket *, 10> >::iterator iter;
	for(iter=m_listReorderedRTPPacket.begin(); iter!=m_listReorderedRTPPacket.end(); ++iter)
	{
		if(SeqNumLessThan(pRTPPacket->SeqNum(), (*iter)->SeqNum()))
			break;
		if(pRTPPacket->SeqNum() == (*iter)->SeqNum())
		{
			m_listEmptyRTPPacket.push_back(pRTPPacket);
			return;
		}
	}
	m_listReorderedRTPPacket.insert(iter, pRTPPacket);
}

CRTPPacket * CReorderRTPPacket::GetNextReorderedRTPPacket(bool & packetLoss)
{
	if(m_listReorderedRTPPacket.empty())
		return NULL;

	CRTPPacket * pRTPPacket = m_listReorderedRTPPacket.front();

	if(pRTPPacket->SeqNum() == m_nextExpectedSeqNum)
	{
		packetLoss = false;
		m_listReorderedRTPPacket.pop_front();
		return pRTPPacket;
	}

//	int seqNumDiff = abs(rtpPacket->RTPSeqNum() - m_nextExpectedSeqNum);
//	int seqNumDiff = rtpPacket->RTPSeqNum() - m_nextExpectedSeqNum;
//	if(seqNumDiff > 1 || m_listReorderedRTPPacket.size() >= 10)
	{
		m_nextExpectedSeqNum = pRTPPacket->SeqNum();

		packetLoss = true;
		m_listReorderedRTPPacket.pop_front();
		return pRTPPacket;
	}

	return NULL;
}

void CReorderRTPPacket::ReleaseRTPPacket(CRTPPacket * pRTPPacket)
{
	if(pRTPPacket == NULL)
		return;

	++m_nextExpectedSeqNum;

	m_listEmptyRTPPacket.push_back(pRTPPacket);
}

void CReorderRTPPacket::ReleaseAllRTPPackets()
{
	m_haveSeenInitialSeqNum = false;
	m_nextExpectedSeqNum = 0;

	list_T<CRTPPacket *, allocator<CRTPPacket *, 10> >::iterator iter;
	for(iter=m_listReorderedRTPPacket.begin(); iter!=m_listReorderedRTPPacket.end(); ++iter)
	{
		m_listEmptyRTPPacket.push_back(*iter);
	}
	m_listReorderedRTPPacket.clear();
}




bool SeqNumLessThan(unsigned short seqNum1, unsigned short seqNum2)
{
	int diff = seqNum2 - seqNum1;
	if(diff > 0)
	{
		return (diff < 0x8000);
	}
	else if(diff < 0)
	{
		return (diff < -0x8000);
	}
	else
	{
		return false;
	}
}
