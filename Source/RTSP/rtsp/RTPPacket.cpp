#include "voLog.h"
#include "utility.h"
#include "MediaStreamSocket.h"

#include "RTPPacket.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

const int MAX_RTP_PACKET_SIZE = 1024 * 48;//in theory, the MTU is 5K,but we found some cases that are over 5k in TCP mode

CRTPPacket::CRTPPacket()
{
	m_rtpStreamBuf = new unsigned char[MAX_RTP_PACKET_SIZE];TRACE_NEW("rtcpp_m_m_rtpStreamBuf",m_rtpStreamBuf);
}

CRTPPacket::~CRTPPacket()
{
	SAFE_DELETE_ARRAY(m_rtpStreamBuf);
}

void CRTPPacket::Reset()
{
	m_rtpPacketSize = 0;
	m_parsePos = m_rtpStreamBuf;

	m_rtpMarker = false;
	m_rtpTimestamp = 0;
	m_rtpSSRC = 0;
	m_rtpPayloadType = 0;
	m_rtpSeqNum = 0;

	m_packetReceivedTime.tv_sec = m_packetReceivedTime.tv_usec = 0;
}

int CRTPPacket::ReadRTPPacket(CMediaStreamSocket * rtpStreamSock)
{
	Reset();

	m_rtpPacketSize = ReadNetData(rtpStreamSock->GetMediaStreamSock(), m_rtpStreamBuf, MAX_RTP_PACKET_SIZE);
	CLog::Log.MakeLog(LL_RTP_ERR,"flow.txt","$$$$$enter ReadRTPPacket\n");
	if(m_rtpPacketSize>MAX_RTP_PACKET_SIZE)
	{
		sprintf(CLog::formatString,"m_rtpPacketSize(%d)>MAX_RTP_PACKET_SIZE\n",m_rtpPacketSize);
		CLog::Log.MakeLog(LL_RTP_ERR,"error.txt",CLog::formatString);
	}
	gettimeofday(&m_packetReceivedTime);
	return m_rtpPacketSize;
}

int CRTPPacket::ReadRTPPacketOverTCP(PIVOSocket rtpStreamSock, int rtpStreamDataSize)
{
	Reset();

	m_rtpPacketSize = ReadNetDataExact(rtpStreamSock, m_rtpStreamBuf, rtpStreamDataSize);
	gettimeofday(&m_packetReceivedTime);
	return m_rtpPacketSize;
}

int CRTPPacket::ReadRTPPacketFromLocal(unsigned char * streamData, int streamDataSize)
{
	Reset();
	int sizeCheck = streamDataSize;
	if(sizeCheck>MAX_RTP_PACKET_SIZE)
	{
		sprintf(CLog::formatString,"sizeCheck(%d)>MAX_RTP_PACKET_SIZE\n",sizeCheck);
		CLog::Log.MakeLog(LL_RTP_ERR,"error.txt",CLog::formatString);
		return 0;
	}
		
	memcpy(m_rtpStreamBuf, streamData, streamDataSize);
	m_rtpPacketSize = streamDataSize;
	return m_rtpPacketSize;
}

bool CRTPPacket::ParseRTPPacketHeader()
{
	do
	{
		CUserOptions::UserOptions.m_skipPos = 0;
		int sizeCheck = RTPPacketSize();
		if(sizeCheck < 12||sizeCheck>MAX_RTP_PACKET_SIZE)
			break;
		unsigned int rtpHeader = ::NetToHostLong(*(unsigned int *)(RTPData()));
		Skip(4);

		if((rtpHeader & 0xC0000000) != 0x80000000) // version: 2 bits
			break;

		m_rtpMarker = ((rtpHeader & 0x00800000) >> 23) != 0;       // marker: 1 bit
		m_rtpPayloadType = (rtpHeader & 0x007F0000) >> 16;  // payload: 7 bits
		m_rtpSeqNum = (unsigned short)(rtpHeader & 0xFFFF); // sequence number: 16 bits

		if(rtpHeader & 0x20000000) // padding: 1 bit
		{
			unsigned int paddingBytes = (unsigned int)(RTPPacket())[RTPPacketSize() - 1];
			if(RTPPacketSize() < (int)paddingBytes)
				break;
			RemoveRTPPadding(paddingBytes);
		}    

		m_rtpTimestamp = ::NetToHostLong(*(unsigned int *)(RTPData()));
		Skip(4);

		m_rtpSSRC = ::NetToHostLong(*(unsigned int *)(RTPData()));
		Skip(4);

		unsigned int rtpCSRCCount = (rtpHeader >> 24) & 0xF;
		Skip(rtpCSRCCount * 4);

		CUserOptions::UserOptions.m_skipPos = 4+4+4+rtpCSRCCount*4;	//doncy 0217

		if(rtpHeader & 0x10000000) // extension: 1 bit
		{
			unsigned int extRtpHeader = ::NetToHostLong(*(unsigned int *)(RTPData()));
			Skip(4);
			unsigned int extRtpHeaderSize = (extRtpHeader & 0xFFFF) * 4;
			Skip(extRtpHeaderSize);

			CUserOptions::UserOptions.m_skipPos = CUserOptions::UserOptions.m_skipPos + 4+extRtpHeaderSize;
		}

#ifdef WIN_X86_1//_DEBUG//TRACE_FDD
		if((CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP))
		{
			int sizeFromUDP=0;
			sizeFromUDP+=RTPPacketSize()-12;
			if(m_rtpPayloadType==97)//&&m_rtpSeqNum>0x0588)
			{
				unsigned char *pos=RTPData();
				sprintf(CLog::formatString,"%d:seq=%d,ts=%d,s=%d,0x%X%X%X%X\n",m_rtpPayloadType,m_rtpSeqNum,m_rtpTimestamp,sizeFromUDP
					,pos[0]
					,pos[1]
					,pos[2]
					,pos[3]);
				CLog::Log.MakeLog(LL_RTP_ERR,"RTPTrack.txt", CLog::formatString);
			}
			
			//return false;
		}
#endif
		return true;

	}while(0);

	return false;
}

void CRTPPacket::Skip(int num)
{
	m_parsePos += num;
	if(m_parsePos > m_rtpStreamBuf + m_rtpPacketSize)
	{
		m_parsePos = m_rtpStreamBuf + m_rtpPacketSize;
	}
}

void CRTPPacket::RemoveRTPPadding(int num)
{
	if(num > m_rtpPacketSize)
		num = m_rtpPacketSize;

	m_rtpPacketSize -= num;
}


