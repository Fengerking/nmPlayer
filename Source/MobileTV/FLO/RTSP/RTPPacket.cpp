
#include "utility.h"
#include "MediaStreamSocket.h"

#include "RTPPacket.h"

#include "DxManager.h"

#define LOG_TAG "FLOEngine_RTPPacket"
#include "voLog.h"

const int MAX_RTP_PACKET_SIZE = 1024 * 16;//in theory, the MTU is 5K,but we found some cases that are over 5k in TCP mode

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
bool  CRTPPacket::			DecryptFromSRTP()
{
#if ENABLE_DX
	int status = TheDxManager.Decrypt(m_rtpStreamBuf, m_rtpPacketSize);
	if(status!=0)
	{
		VOLOGE("Decrypt fails");
		return false;
	}
#endif //ENABLE_DX
	return true;
}
int CRTPPacket::ReadRTPPacket(CMediaStreamSocket * rtpStreamSock)
{
	Reset();

	m_rtpPacketSize = ReadNetData(rtpStreamSock->GetMediaStreamSock(), m_rtpStreamBuf, MAX_RTP_PACKET_SIZE);
	//VOLOGI("ReadNetData... %d", m_rtpPacketSize);



	if(m_rtpPacketSize > MAX_RTP_PACKET_SIZE)
	{
		sprintf(CLog::formatString,"m_rtpPacketSize(%d)>MAX_RTP_PACKET_SIZE\n",m_rtpPacketSize);
		CLog::Log.MakeLog(LL_RTP_ERR,"error.txt",CLog::formatString);
		VOLOGE("m_rtpPacketSize(%d) > MAX_RTP_PACKET_SIZE", m_rtpPacketSize);
		m_rtpPacketSize = 0;
	}
	gettimeofday(&m_packetReceivedTime);
	return m_rtpPacketSize;
}

bool CRTPPacket::ParseRTPPacketHeader()
{
	do
	{
		int sizeCheck = RTPPacketSize();
		if(sizeCheck < 12||sizeCheck>MAX_RTP_PACKET_SIZE)
        {
            VOLOGE("Wrong sizeCheck: %d", sizeCheck);
			break;
        }
		
		unsigned int rtpHeader = ::NetToHostLong(*(unsigned int *)(RTPData()));
		Skip(4);

		if((rtpHeader & 0xC0000000) != 0x80000000) // version: 2 bits
        {
            VOLOGE("Wrong version, rtpHader: 0x%x", rtpHeader);
			break;
        }

		m_rtpMarker = ((rtpHeader & 0x00800000) >> 23) != 0;       // marker: 1 bit
		m_rtpPayloadType = (rtpHeader & 0x007F0000) >> 16;  // payload: 7 bits
		m_rtpSeqNum = (unsigned short)(rtpHeader & 0xFFFF); // sequence number: 16 bits

		if(rtpHeader & 0x20000000) // padding: 1 bit
		{
			unsigned int paddingBytes = (unsigned int)(RTPPacket())[RTPPacketSize() - 1];
			if(RTPPacketSize() < (int)paddingBytes)
            {
                VOLOGE("RTPPacketSize (%d) < paddingBytes %d", RTPPacketSize(), paddingBytes);
				break;
            }
			RemoveRTPPadding(paddingBytes);
		}    

		m_rtpTimestamp = ::NetToHostLong(*(unsigned int *)(RTPData()));
		Skip(4);

		m_rtpSSRC = ::NetToHostLong(*(unsigned int *)(RTPData()));
		Skip(4);

		unsigned int rtpCSRCCount = (rtpHeader >> 24) & 0xF;
		Skip(rtpCSRCCount * 4);

		if(rtpHeader & 0x10000000) // extension: 1 bit
		{
			unsigned int extRtpHeader = ::NetToHostLong(*(unsigned int *)(RTPData()));
			Skip(4);
			unsigned int extRtpHeaderSize = (extRtpHeader & 0xFFFF) * 4;
			Skip(extRtpHeaderSize);
		}

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
