#ifndef __RTPSENDERTABLE_H__
#define __RTPSENDERTABLE_H__

#include "HashTable.h"

#define TRACE_RR	1

class CRTPSenderStatus;

class CRTPSenderTable
{
public:
	CRTPSenderTable();
	virtual ~CRTPSenderTable();

public:
	void DeleteCurStatus();
	void Reset();
	unsigned int ActiveSenderCountSinceLastReset() const { return  1; }
	
public:
	void NotifyReceivedRTPPacketFromSender(unsigned int SSRC, unsigned short seqNum, unsigned int rtpTimestamp, unsigned int timestampFrequency, unsigned int packetSize);
	void NotifyReceivedSRPacketFromSender(unsigned int SSRC, unsigned int ntpTimestampMSW, unsigned int ntpTimestampLSW, unsigned int rtpTimestamp);

	//Lookup(unsigned int SSRC) const;
	void RemoveRecord(unsigned int SSRC);
	void RemoveAllRecords();

//protected:
	//void AddRTPSender(unsigned int SSRC, CRTPSenderStatus * rtpSenderStatus);
	CRTPSenderStatus* GetSenderStatus(){return m_status;};
protected:
	CRTPSenderStatus* m_status;
	//CHashTable * m_rtpSenderStatusTable;
	//unsigned int m_activeSendersCountSinceLastReset;
};

//-----------------        CRTPSenderStatus          ---------------------

class CRTPSenderStatus
{
public:
	CRTPSenderStatus(unsigned int SSRC);
	CRTPSenderStatus(unsigned int SSRC, unsigned short seqNum);
	virtual ~CRTPSenderStatus();

public:
	void InitRTPSenderStatus(unsigned int SSRC);
	void InitRTPSenderSeqNum(unsigned short initialSeqNum);
	void Reset();

public:
	void NotifyReceivedRTPPacketFromSender(unsigned short seqNum, unsigned int rtpTimestamp, unsigned int timestampFrequency, unsigned int packetSize);
	void NotifyReceivedSRPacketFromSender(unsigned int ntpTimestampMSW, unsigned int ntpTimestampLSW, unsigned int rtpTimestamp);

public:
	unsigned int SSRC() { return m_SSRC; }
	unsigned int PacketsReceivedSinceLastReset() { return m_packetsReceivedSinceLastReset; }
	unsigned int TotalPacketsReceived() { return m_totalPacketsReceived; }
	unsigned int TotalPacketsLost() { return m_totalPacketsLost; }
	void		 UpdatePacketsLost(int lost){m_totalPacketsLost+=lost;}
	void		 UpdatePacketError(int num);
	unsigned int TotalNumPacketsExpected() { return m_highestExtSeqNumReceived - m_baseExtSeqNumReceived; }

	unsigned int BaseExtSeqNumReceived() { return m_baseExtSeqNumReceived; }
	unsigned int LastResetExtSeqNumReceived() { return m_lastResetExtSeqNumReceived; }
	unsigned int HighestExtSeqNumReceived() { return m_highestExtSeqNumReceived; }

	unsigned int Jitter() { return (unsigned int)m_jitter; }

	unsigned int LastReceivedSR_NTPmsw() { return m_lastReceivedSR_NTPmsw; }
	unsigned int LastReceivedSR_NTPlsw() { return m_lastReceivedSR_NTPlsw; }
	struct timeval & LastReceivedSR_time() { return m_lastReceivedSR_time;}

	unsigned int MinInterPacketGapUS() { return m_minInterPacketGapUS; }
	unsigned int MaxInterPacketGapUS() { return m_maxInterPacketGapUS; }
	struct timeval & TotalInterPacketGaps() { return m_totalInterPacketGaps;}

protected:
	unsigned int		m_SSRC;
	unsigned int        m_packetsReceivedSinceLastReset;
	unsigned int		m_totalPacketsReceived;
	unsigned int		m_totalPacketsLost;
	bool 			    m_haveSeenInitialSequenceNumber;
	unsigned int		m_baseExtSeqNumReceived;
	unsigned int 		m_lastResetExtSeqNumReceived;
	unsigned int 		m_highestExtSeqNumReceived;
	int				    m_lastTransit;
	unsigned int		m_previousPacketRTPTimestamp;
	double		    	m_jitter;
	unsigned int		m_lastReceivedSR_NTPmsw; 
	unsigned int		m_lastReceivedSR_NTPlsw;
	
	struct timeval      m_lastReceivedSR_time;
	struct timeval      m_lastPacketReceptionTime;
	unsigned int	    m_minInterPacketGapUS, m_maxInterPacketGapUS;
	struct timeval	    m_totalInterPacketGaps;

	bool				m_hasBeenSynchronizedByRTCP;
	unsigned int		m_syncTimestamp;
	struct timeval		m_syncTime;
	int					m_packErrNumInThisReport;
public:
	void		 UpdatePacketErrorNum(int num){m_packErrNumInThisReport+=num;}
	int			 GetPacketErrNum(){return m_packErrNumInThisReport;}
	void		 ResetPacketErrNum(){m_packErrNumInThisReport=0;}
};

#endif //__RTPSENDERTABLE_H__

