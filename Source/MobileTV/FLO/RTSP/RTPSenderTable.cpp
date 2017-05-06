
#include <string.h>

#include "RTPParser.h"
#include "network.h"
#include "RTPSenderTable.h"
#include "utility.h";
CRTPSenderTable::CRTPSenderTable()
:m_status(NULL)
{
	Reset();
}

CRTPSenderTable::~CRTPSenderTable()
{
	RemoveAllRecords();
	SAFE_DELETE(m_status);
}

void CRTPSenderTable::Reset()
{
	if(m_status)
	{
		m_status->Reset();
	}
}
void CRTPSenderTable::DeleteCurStatus()
{
	SAFE_DELETE(m_status);	
}
void CRTPSenderTable::NotifyReceivedRTPPacketFromSender(unsigned int SSRC, unsigned short seqNum, unsigned int rtpTimestamp, unsigned int timestampFrequency, unsigned int packetSize)
{
	if(m_status == NULL)
	{
		m_status = new CRTPSenderStatus(SSRC, seqNum);TRACE_NEW("rtpst_m_senderStatus",m_status);
		if(m_status == NULL)
			return;

	}
	m_status->NotifyReceivedRTPPacketFromSender(seqNum, rtpTimestamp, timestampFrequency, packetSize);
}

void CRTPSenderTable::NotifyReceivedSRPacketFromSender(unsigned int SSRC, unsigned int ntpTimestampMSW, unsigned int ntpTimestampLSW, unsigned int rtpTimestamp)
{
	if(m_status == NULL)
	{
		m_status = new CRTPSenderStatus(SSRC);TRACE_NEW("rtpst_m_senderStatus2",m_status);
		if(m_status == NULL)
			return;

	}

	m_status->NotifyReceivedSRPacketFromSender(ntpTimestampMSW, ntpTimestampLSW, rtpTimestamp);
}

void CRTPSenderTable::RemoveRecord(unsigned int SSRC)
{
	
	//SAFE_DELETE(m_status);
	
}

void CRTPSenderTable::RemoveAllRecords()
{
	RemoveRecord(0);
}


//----------------        CRTPSenderStatus          -----------------------

CRTPSenderStatus::CRTPSenderStatus(unsigned int SSRC)
{
	InitRTPSenderStatus(SSRC);
}

CRTPSenderStatus::CRTPSenderStatus(unsigned int SSRC, unsigned short seqNum)
{
	//InitRTPSenderSeqNum(seqNum);
	InitRTPSenderStatus(SSRC);
	CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt","CRTPSenderStatus:InitRTPSenderSeqNum\n");

}

CRTPSenderStatus::~CRTPSenderStatus()
{
}

void CRTPSenderStatus::InitRTPSenderStatus(unsigned int SSRC)
{
	m_SSRC = SSRC;
	m_totalPacketsReceived = 0;
	m_totalPacketsLost = 0;
	m_haveSeenInitialSequenceNumber = false;
	m_lastTransit = ~0;
	m_previousPacketRTPTimestamp = 0;
	m_jitter = 0.0;
	m_lastReceivedSR_NTPmsw = m_lastReceivedSR_NTPlsw = 0;
	m_highestExtSeqNumReceived = 0;
	m_lastResetExtSeqNumReceived = 0;
	m_baseExtSeqNumReceived=0;
	m_lastReceivedSR_time.tv_sec = m_lastReceivedSR_time.tv_usec = 0;
	m_lastPacketReceptionTime.tv_sec = m_lastPacketReceptionTime.tv_usec = 0;
	m_minInterPacketGapUS = 0x7FFFFFFF;
	m_maxInterPacketGapUS = 0;
	m_totalInterPacketGaps.tv_sec = m_totalInterPacketGaps.tv_usec = 0;
	m_syncTimestamp = 0;
	m_hasBeenSynchronizedByRTCP = false;
	m_syncTime.tv_sec = m_syncTime.tv_usec = 0;
	Reset();
}

void CRTPSenderStatus::InitRTPSenderSeqNum(unsigned short initialSeqNum)
{
	
	m_baseExtSeqNumReceived    = initialSeqNum;
	m_highestExtSeqNumReceived = initialSeqNum;
	m_lastResetExtSeqNumReceived = m_highestExtSeqNumReceived;
	m_haveSeenInitialSequenceNumber = true;
	CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt","InitRTPSenderSeqNum\n");
}

#ifndef MILLION
#define MILLION 1000000
#endif

void CRTPSenderStatus::NotifyReceivedRTPPacketFromSender(unsigned short seqNum, unsigned int rtpTimestamp, unsigned int timestampFrequency, unsigned int packetSize)
{
	if(!m_haveSeenInitialSequenceNumber)
	{
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt","NotifyReceivedRTPPacketFromSender:InitRTPSenderSeqNum\n");
		InitRTPSenderSeqNum(seqNum);
	}

	++m_packetsReceivedSinceLastReset;
	++m_totalPacketsReceived;
#if 0//def WMP_PLUGIN 
	m_highestExtSeqNumReceived=seqNum;
#else
	// Check whether the sequence number has wrapped around:
	unsigned int seqNumCycle       = (m_highestExtSeqNumReceived & 0xFFFF0000);
	unsigned int oldSeqNum         = (m_highestExtSeqNumReceived & 0xFFFF);
	unsigned int seqNumDifference  = (unsigned int)((int)seqNum - (int)oldSeqNum);
	if(seqNumDifference >= 0x8000 && SeqNumLT((unsigned short)oldSeqNum, seqNum))
	{
		// sequence number wrapped around => start a new cycle:
		seqNumCycle += 0x10000;
	}

	unsigned int newSeqNum = seqNumCycle | seqNum;
	if(newSeqNum > m_highestExtSeqNumReceived)
	{
		m_highestExtSeqNumReceived = newSeqNum;
	}
#endif
	struct timeval timeNow;
	gettimeofday(&timeNow);
	if(m_lastPacketReceptionTime.tv_sec != 0 || m_lastPacketReceptionTime.tv_usec != 0)
	{
		unsigned int gap = (timeNow.tv_sec - m_lastPacketReceptionTime.tv_sec)*MILLION + (timeNow.tv_usec - m_lastPacketReceptionTime.tv_usec); 
		if(gap > m_maxInterPacketGapUS)
		{
			m_maxInterPacketGapUS = gap;
		}
		if(gap < m_minInterPacketGapUS)
		{
			m_minInterPacketGapUS = gap;
		}
		m_totalInterPacketGaps.tv_usec += gap;
		if(m_totalInterPacketGaps.tv_usec >= MILLION)//TODO:how about the gap is great than 2 seconds?
		{
			m_totalInterPacketGaps.tv_sec++;
			m_totalInterPacketGaps.tv_usec -= MILLION;
		}
	}

	m_lastPacketReceptionTime = timeNow;

	if(rtpTimestamp != m_previousPacketRTPTimestamp)
	{
		unsigned int arrival = (timestampFrequency * timeNow.tv_sec);
		arrival += (unsigned int)((2.0 * timestampFrequency * timeNow.tv_usec + 1000000.0)/2000000);
		// note: rounding
		int transit = arrival - rtpTimestamp;
		if(m_lastTransit == (~0))
		{
			m_lastTransit = transit;
		}
		int d = transit - m_lastTransit;
		m_lastTransit = transit;
		if (d < 0)
		{
			d = -d;
		}
		m_jitter += (1.0/16.0) * ((double)d - m_jitter);
	}

	m_previousPacketRTPTimestamp = rtpTimestamp;
}

void CRTPSenderStatus::NotifyReceivedSRPacketFromSender(unsigned int ntpTimestampMSW, unsigned int ntpTimestampLSW, unsigned int rtpTimestamp)
{
	m_lastReceivedSR_NTPmsw = ntpTimestampMSW;
	m_lastReceivedSR_NTPlsw = ntpTimestampLSW;

	gettimeofday(&m_lastReceivedSR_time);

#if 0//TRACE_RR//def R_TEST// TRACE_CRASH
	if(CUserOptions::UserOptions.m_bMakeLog==2)
	{
		sprintf(CLog::formatString,"$$$$$leave ReadRTCPStream,LSR=(%d,%d)\n",m_lastReceivedSR_time.tv_sec,
			m_lastReceivedSR_time.tv_usec);
		CLog::Log.MakeLog(LL_RTP_ERR,"RTCPSR.txt",CLog::formatString);//
	}
#endif//TRACE_FLOW

	m_syncTimestamp       = rtpTimestamp;
	m_syncTime.tv_sec     = ntpTimestampMSW - 0x83AA7E80;
	double microseconds   = (ntpTimestampLSW * 15625.0) / 0x04000000;
	m_syncTime.tv_usec    = (unsigned int)(microseconds + 0.5);
	m_hasBeenSynchronizedByRTCP = true;
}

void CRTPSenderStatus::Reset()
{
	m_packetsReceivedSinceLastReset = 0;
	m_lastResetExtSeqNumReceived = m_highestExtSeqNumReceived;
	ResetPacketErrNum();
}
