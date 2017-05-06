#include <string.h>
#include "utility.h"
#include "RTPPacket.h"
#include "RTPPacketReorder.h"
#include "mylog.h"
#include "voLog.h"

CRTPPacketReorder::CRTPPacketReorder()
: m_haveSeenInitialSeqNum(false)
, m_nextExpectedSeqNum(0)
, m_actualPackets(0)
{
}

CRTPPacketReorder::~CRTPPacketReorder()
{
	Reset();
}

void CRTPPacketReorder::Reset()
{
	list_T<CRTPPacket *, allocator<CRTPPacket *, MAX_RTP_PACKETS_IN_BUFFER> >::iterator iter;
	
	ReleaseAllRTPPackets();
	int size = m_listEmptyRTPPacket.size();
	if(size != m_actualPackets)
	{
		sprintf(CLog::formatString, "size(%d)!=m_actualPackets(%d)\n", size, m_actualPackets);
		CLog::Log.MakeLog(LL_RTP_ERR, "memory.txt", CLog::formatString);
	}
	for(iter=m_listEmptyRTPPacket.begin(); iter!=m_listEmptyRTPPacket.end(); ++iter)
	{
		SAFE_DELETE(*iter);
	}
	
	m_actualPackets = 0;
	m_listEmptyRTPPacket.clear();
}

CRTPPacket * CRTPPacketReorder::GetEmptyRTPPacket()
{
	CRTPPacket * rtpPacket = NULL;
	do
	{
		if(!m_listEmptyRTPPacket.empty())
		{
			rtpPacket = m_listEmptyRTPPacket.front();
			m_listEmptyRTPPacket.pop_front();
			break;
		}

		rtpPacket = new CRTPPacket();
		m_actualPackets++;

	}while(0);

	return rtpPacket;
}

void CRTPPacketReorder::ReorderRTPPacket(CRTPPacket * rtpPacket)
{
	if(!m_haveSeenInitialSeqNum)
	{
		m_haveSeenInitialSeqNum = true;
		m_nextExpectedSeqNum = rtpPacket->RTPSeqNum();
	}

	//if(SeqNumLT(rtpPacket->RTPSeqNum(), m_nextExpectedSeqNum))
	//	return;


	list_T<CRTPPacket *, allocator<CRTPPacket *, MAX_RTP_PACKETS_IN_BUFFER> >::iterator iter;
	for(iter=m_listReorderedRTPPacket.begin(); iter!=m_listReorderedRTPPacket.end(); ++iter)
	{
		if(SeqNumLT(rtpPacket->RTPSeqNum(), (*iter)->RTPSeqNum()))
			break;
		if(rtpPacket->RTPSeqNum() == (*iter)->RTPSeqNum())
		{
			ReleaseRTPPacket(rtpPacket);
			return;
		}
	}
	m_listReorderedRTPPacket.insert(iter, rtpPacket);
}

CRTPPacket * CRTPPacketReorder::GetNextReorderedRTPPacket(bool & packetLoss)
{
	if(m_listReorderedRTPPacket.empty())
		return NULL;

	CRTPPacket * rtpPacket = m_listReorderedRTPPacket.front();

	if(rtpPacket->RTPSeqNum() == m_nextExpectedSeqNum)
	{
		packetLoss = false;

	}
	else
	{
		int lostNum=rtpPacket->RTPSeqNum()-m_nextExpectedSeqNum;
		CUserOptions::UserOptions.nLostPacks+=lostNum;
#if 1
		if(1)//CUserOptions::UserOptions.m_bMakeLog)
		{
			sprintf(CLog::formatString,"%d,  loss = %d (%d - %d)\n", rtpPacket->RTPPayloadType(), lostNum, rtpPacket->RTPSeqNum(), m_nextExpectedSeqNum);
			CLog::Log.MakeLog(LL_RTP_ERR,"packet_loss.txt",CLog::formatString);
			VOLOGE("%s",CLog::formatString);
		}
#endif
		m_nextExpectedSeqNum = rtpPacket->RTPSeqNum();

		packetLoss = true;
	}
	m_listReorderedRTPPacket.pop_front();
	ReleaseRTPPacket(rtpPacket);
	return rtpPacket;
}

void CRTPPacketReorder::ReleaseRTPPacket(CRTPPacket * rtpPacket)
{
	if(rtpPacket == NULL)
		return;

	++m_nextExpectedSeqNum;
	m_listEmptyRTPPacket.push_back(rtpPacket);
}

void CRTPPacketReorder::ReleaseAllRTPPackets()
{
	m_haveSeenInitialSeqNum = false;
	m_nextExpectedSeqNum = 0;

	list_T<CRTPPacket *, allocator<CRTPPacket *, MAX_RTP_PACKETS_IN_BUFFER> >::iterator iter;
	for(iter=m_listReorderedRTPPacket.begin(); iter!=m_listReorderedRTPPacket.end(); ++iter)
	{
		m_listEmptyRTPPacket.push_back(*iter);
	}
	m_listReorderedRTPPacket.clear();
}
