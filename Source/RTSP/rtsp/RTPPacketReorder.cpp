#include <string.h>
#include "utility.h"
#include "RTPPacket.h"
#include "RTPPacketReorder.h"
#include "RTPSenderTable.h"
#include "RTPParser.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CRTPPacketReorder::CRTPPacketReorder(CRTPParser* rtpParser)
: m_haveSeenInitialSeqNum(false)
, m_nextExpectedSeqNum(0)
,m_actualPackets(0)
,m_rtpParser(rtpParser)
,m_duplicatePacketNum(0)
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
	int size=m_listEmptyRTPPacket.size();
	if(size!=m_actualPackets)
	{
		sprintf(CLog::formatString,"size(%d)!=m_actualPackets(%d)\n",size,m_actualPackets);
		CLog::Log.MakeLog(LL_RTP_ERR,"memory.txt",CLog::formatString);
	}
	for(iter=m_listEmptyRTPPacket.begin(); iter!=m_listEmptyRTPPacket.end(); ++iter)
	{
		SAFE_DELETE(*iter);
	}
	
	m_actualPackets = 0;
	m_listEmptyRTPPacket.clear();
}
long		CRTPPacketReorder::	GetStoredPackTime(unsigned int frequency)
{
	long ret = 0;
	if(m_listReorderedRTPPacket.size()>=2)
	{
		CRTPPacket * rtpPacket = m_listReorderedRTPPacket.front();
		if(rtpPacket)
		{
			unsigned int firstTime = rtpPacket->RTPTimestamp();
			rtpPacket = m_listReorderedRTPPacket.back();
			if (rtpPacket)
			{
				unsigned int lastTime = rtpPacket->RTPTimestamp();
				ret = (lastTime - firstTime)*1000/frequency;
				//SLOG3(LL_RTP_ERR,"mediasink.txt","Antijitter:%ld=%d-%d\n",ret, lastTime, firstTime);
			}
		}
	}
	
	return ret;
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

		rtpPacket = new CRTPPacket();TRACE_NEW("rtcpp_mrtpPacket",rtpPacket);
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
#define TEST 0
#if  !TEST
	list_T<CRTPPacket *, allocator<CRTPPacket *, MAX_RTP_PACKETS_IN_BUFFER> >::iterator iter;
	for(iter=m_listReorderedRTPPacket.begin(); iter!=m_listReorderedRTPPacket.end(); ++iter)
	{
		if(SeqNumLT(rtpPacket->RTPSeqNum(), (*iter)->RTPSeqNum()))
			break;
		if(rtpPacket->RTPSeqNum() == (*iter)->RTPSeqNum())
		{
			ReleaseRTPPacket(rtpPacket);
			m_duplicatePacketNum++;
			return;
		}
	}
	m_listReorderedRTPPacket.insert(iter, rtpPacket);
#else
	int size=31;
	int seqNum[31]={1,3,7,9,2,5,19,12,6,4,
						 13,18,15,16,11,17,30,21,22,23,
						 29,27,28,24,26,25,14,8,10,9,31};
	int index=0;
	do 
	{
		rtpPacket = GetEmptyRTPPacket();
		rtpPacket->SetRTPSeqNum(seqNum[index++]);
		list_T<CRTPPacket *, allocator<CRTPPacket *, MAX_RTP_PACKETS_IN_BUFFER> >::iterator iter;
		for(iter=m_listReorderedRTPPacket.begin(); iter!=m_listReorderedRTPPacket.end(); ++iter)
		{
			if(SeqNumLT(rtpPacket->RTPSeqNum(), (*iter)->RTPSeqNum()))
				break;
		}
		m_listReorderedRTPPacket.insert(iter, rtpPacket);
		int storedPacket = GetStoredPacket();
		if(storedPacket>=30)//||CUserOptions::UserOptions.status==Session_Paused)//CUserOptions::UserOptions.streamEnd)//CUserOptions::UserOptions.status==Session_Paused||
		{
			do 
			{
				bool lost;
				CRTPPacket * rtpPacket2=GetNextReorderedRTPPacket(lost);
				sprintf(CLog::formatString,"reorder %d\n",rtpPacket2->RTPSeqNum());
				CLog::Log.MakeLog(LL_RTP_ERR,"testSeqOrder.txt",CLog::formatString);	
			} while (--storedPacket);
		
			break;
		}
	} while (--size);
	
#endif//TEST
}

CRTPPacket * CRTPPacketReorder::GetNextReorderedRTPPacket(bool & packetLoss)
{
	if(m_listReorderedRTPPacket.empty())
		return NULL;

	CRTPPacket * rtpPacket = m_listReorderedRTPPacket.front();

	if(rtpPacket->RTPSeqNum() == m_nextExpectedSeqNum)
	{
		packetLoss = false;
		CUserOptions::UserOptions.nReceivdPacks++;
	}
	else if(rtpPacket->RTPSeqNum() > m_nextExpectedSeqNum)
	{
		int lostNum=rtpPacket->RTPSeqNum()-m_nextExpectedSeqNum;
		CUserOptions::UserOptions.nLostPacks+=lostNum;
		
		if(CUserOptions::UserOptions.m_bMakeLog)
		{
			sprintf(CLog::formatString,"%d,loss=%d(%d-%d)\n",rtpPacket->RTPPayloadType(),lostNum,rtpPacket->RTPSeqNum(),m_nextExpectedSeqNum);
			CLog::Log.MakeLog(LL_RTP_ERR,"packet_loss.txt",CLog::formatString);
		}
		NotifyEvent(VO_EVENT_PACKET_LOST,CUserOptions::UserOptions.nLostPacks);

		m_nextExpectedSeqNum = rtpPacket->RTPSeqNum();

		packetLoss = true;
	}
	else
	{
		packetLoss = false;
		CUserOptions::UserOptions.nReceivdPacks++;
		
		m_rtpParser->EnlargeAntiJitterBuffer();
		--m_nextExpectedSeqNum;
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
