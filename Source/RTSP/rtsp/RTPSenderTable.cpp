
#include <string.h>

#include "RTPParser.h"
#include "network.h"
#include "RTPSenderTable.h"
#include "utility.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

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

#if ENABLE_RFC3611
	int diffSeq=seqNum-m_highestExtSeqNumReceived;
	if(diffSeq>1&&diffSeq<100)
	{
		NotifyPackLost(m_highestExtSeqNumReceived,seqNum);
	}
#endif//ENABLE_RFC3611
	++m_packetsReceivedSinceLastReset;
	++m_totalPacketsReceived;

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
	int diff=newSeqNum - m_highestExtSeqNumReceived;
	if(diff>0)
	{
		m_highestExtSeqNumReceived = newSeqNum;
	}
	if(diff>1)
	{

	}

	struct votimeval timeNow;
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
#if ENABLE_RFC3611
	memset(&rle,0,sizeof(TRleReport));
	memset(bitVec.bitstream,0xff,sizeof(TBitVector));
	bitVec.curByte		= bitVec.bitstream;
	bitVec.bitOffset		= 0;
	bitVec.lastSeq		= m_lastResetExtSeqNumReceived==0?0:m_lastResetExtSeqNumReceived+1;
	if(CUserOptions::UserOptions.m_bMakeLog)
	{
		sprintf(CLog::formatString,"@@reset FIRST=%d\n ",m_lastResetExtSeqNumReceived);
		CLog::Log.MakeLog(LL_RTP_ERR,"rfc3611_Test.txt",CLog::formatString);
	}
#endif//ENABLE_RFC3611
}
#if ENABLE_RFC3611
void CRTPSenderStatus::shiftbits(int numberBits)
{
#define MAX_PACKS 128*5
	if(numberBits<=0||numberBits>MAX_PACKS)
		return;
	numberBits+=bitVec.bitOffset;
	int numbytes=numberBits/8;
	bitVec.curByte+=numbytes;
	bitVec.bitOffset=numberBits&7;
}
void CRTPSenderStatus::PadZerobits(int num)
{
	char* curByte=bitVec.curByte;
	int   bitOffset=bitVec.bitOffset;
	if(num<=0||num>MAX_PACKS)
		return;
	while(num-->0)
	{
		curByte[0]&=~(1<<(7-bitOffset));
		bitOffset++;
		if(bitOffset==8)
		{
			curByte++;
			bitOffset=0;
		}
	}
}
void CRTPSenderStatus::clearbits(int num)
{
	if(num<=0||num>MAX_PACKS)
		return;
	while(num-->0)
	{
		bitVec.curByte[0]&=~(1<<(7-bitVec.bitOffset));
		bitVec.bitOffset++;
		if(bitVec.bitOffset==8)
		{
			bitVec.curByte++;
			bitVec.bitOffset=0;
		}
	}
}

int CRTPSenderStatus::Get1Bit()
{
	int ret=(bitVec.curByte[0]>>(7-bitVec.bitOffset))&1;
	bitVec.bitOffset++;
	if(bitVec.bitOffset==8)
	{
		bitVec.curByte++;
		bitVec.bitOffset=0;
	}
	return ret;
}
int CRTPSenderStatus::GetBits(int num)
{
	if(num<=0||num>MAX_PACKS)
		return 0;
	int ret=0;
	while(num-->0)
	{
		ret|=Get1Bit();
		ret<<=1;
	} 
	return ret>>1;
}
int CRTPSenderStatus::TotolBits()
{
	return (bitVec.curByte-bitVec.bitstream)*8+bitVec.bitOffset;
}
void CRTPSenderStatus::NotifyPackLost(int lastSeq,int curSeq)
{
	if(bitVec.lastSeq==0)
		return;
	if(CUserOptions::UserOptions.m_bMakeLog)
	{
		sprintf(CLog::formatString,"%d-%d	[%d-%d] %d\n",bitVec.lastSeq,lastSeq,lastSeq+1,curSeq-1,curSeq);
		CLog::Log.MakeLog(LL_RTP_ERR,"rfc3611_Test.txt",CLog::formatString);
		
	}
	int inc=0;
	//if(bitVec.lastSeq==(m_lastResetExtSeqNumReceived+1))
		inc=1;
	shiftbits(lastSeq-bitVec.lastSeq+inc);
	clearbits(curSeq-lastSeq-1);
	shiftbits(1);//the curPack
	bitVec.lastSeq=curSeq;
}

TRleReport* CRTPSenderStatus::CreateRFC3611Report()
{
	int totalPacks=(bitVec.curByte-bitVec.bitstream)*8+bitVec.bitOffset;
	int expectedTotalPacks=m_highestExtSeqNumReceived-m_lastResetExtSeqNumReceived;
	int diff3=expectedTotalPacks-totalPacks;
	if(CUserOptions::UserOptions.m_bMakeLog)
	{
		sprintf(CLog::formatString,"begin=%d,end+1=%d,(%d) total packs=%d,shift=%d\n",m_lastResetExtSeqNumReceived+1,m_highestExtSeqNumReceived+1,expectedTotalPacks,totalPacks,diff3);
		CLog::Log.MakeLog(LL_RTP_ERR,"rfc3611_Test.txt",CLog::formatString);
	}
	
	
	if(totalPacks==0)//no loss
	{
		rle.rle[rle.index++]=0x4000|m_packetsReceivedSinceLastReset;
	}
	else
	{
		
		if(diff3>0)
			shiftbits(diff3);
		PadZerobits(16);
		if(CUserOptions::UserOptions.m_bMakeLog)
		{
			char* data=bitVec.bitstream;
			sprintf(CLog::formatString,"data:%X,%X,%X,%X,%X,%X,%X,%X\n",data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]);
			CLog::Log.MakeLog(LL_RTP_ERR,"rfc3611_Test.txt",CLog::formatString);
		}
		
		//prepare RLE
		bitVec.curByte=bitVec.bitstream;
		bitVec.bitOffset=0;
		int received=0;
		int hasGotPacks;
		int remaind;
		do 
		{
			int reciveOne=Get1Bit();
			
			if(reciveOne)
				received++;
			else
			{
#define MAX_RLE_LEN 14
#define MAX_BIT_LEN 15
				//handle rle first
				if (received>MAX_RLE_LEN)
				{
					rle.rle[rle.index++]=0x4000|received;
					//get the following 15 bits
					remaind=GetBits(MAX_BIT_LEN-1);//the first zero has been got
					if(remaind==0&&TotolBits()>expectedTotalPacks)
					{
						if(CUserOptions::UserOptions.m_bMakeLog)
						{
							sprintf(CLog::formatString,"case1_2:%X,con=%d\n",rle.rle[rle.index-1],TotolBits());
							CLog::Log.MakeLog(LL_RTP_ERR,"rfc3611_Test.txt",CLog::formatString);
						}
						break;
					}
					rle.rle[rle.index++]=0x8000|remaind;
					if(CUserOptions::UserOptions.m_bMakeLog)
					{
						sprintf(CLog::formatString,"case1:%X,%X con=%d\n",rle.rle[rle.index-2],rle.rle[rle.index-1],TotolBits());
						CLog::Log.MakeLog(LL_RTP_ERR,"rfc3611_Test.txt",CLog::formatString);
					}
				}
				else
				{
					remaind=GetBits(MAX_BIT_LEN-received-1);
					if(remaind==0&&TotolBits()>expectedTotalPacks)
					{
						rle.rle[rle.index++]=0x4000|received;
						if(CUserOptions::UserOptions.m_bMakeLog)
						{
							sprintf(CLog::formatString,"case2_2:%X,consumed=%d\n",rle.rle[rle.index-1],TotolBits());
							CLog::Log.MakeLog(LL_RTP_ERR,"rfc3611_Test.txt",CLog::formatString);
						}
						break;
					}
					rle.rle[rle.index++]=((remaind|(0xffff<<(MAX_BIT_LEN-received))))&0xffff;	
					if(CUserOptions::UserOptions.m_bMakeLog)
					{
						sprintf(CLog::formatString,"case2:%X,consumed=%d\n",rle.rle[rle.index-1],TotolBits());
						CLog::Log.MakeLog(LL_RTP_ERR,"rfc3611_Test.txt",CLog::formatString);
					}
				}

				received=0;
			}
			hasGotPacks=TotolBits();

		}while(hasGotPacks <=expectedTotalPacks);
	}
	rle.rle[rle.index]=0;//make the last one as zero for padding if needed
	if(CUserOptions::UserOptions.m_bMakeLog)
	{
		sprintf(CLog::formatString,"done:blockNum%d\n",rle.index);
		CLog::Log.MakeLog(LL_RTP_ERR,"rfc3611_Test.txt",CLog::formatString);
	}
	return &rle;
}
void CRTPSenderStatus::verify()
{
	int i=0;
	int packetNum=0;
	for (;i<rle.index;i++)
	{
		short rleVal=rle.rle[i];
		int type=0x8000&rleVal;

		if(type)//
		{
			int total=MAX_BIT_LEN;
			do 
			{
				int lost=((rleVal>>(total-1))&1)==0;
				if (lost)
				{
					sprintf(CLog::formatString,"[%d],",packetNum++);

				} 
				else
				{
					sprintf(CLog::formatString,"%d,",packetNum++);
				}
				CLog::Log.MakeLog(LL_RTP_ERR,"rfc3611_Test.txt",CLog::formatString);
			} while(--total>0);
			sprintf(CLog::formatString,"\n");
			CLog::Log.MakeLog(LL_RTP_ERR,"rfc3611_Test.txt",CLog::formatString);
		}
		else//rle
		{
			int isReceived=0x4000&rleVal;
			int packetsIntheRLE=rleVal&0x3fff;
			if (isReceived)
			{
				sprintf(CLog::formatString,"%d-%d\n",packetNum,packetNum+packetsIntheRLE-1);
				packetNum+=packetsIntheRLE;
			} 
			else
			{
				sprintf(CLog::formatString,"[%d-%d]\n",packetNum,packetNum+packetsIntheRLE-1);
				packetNum+=packetsIntheRLE;
			}
			CLog::Log.MakeLog(LL_RTP_ERR,"rfc3611_Test.txt",CLog::formatString);
		}
	}
}
#endif//ENABLE_RFC3611
