#ifndef __RTCPREPORTPACKET_H__
#define __RTCPREPORTPACKET_H__

class CRTCPReportPacket
{
public:
	CRTCPReportPacket(unsigned int maxPacketSize);
	~CRTCPReportPacket();

public:
	unsigned char * CurPos() { return &(m_packetBuffer[m_curPos]); }
	unsigned int    MaxPacketSize() { return m_maxPacketSize; }
	unsigned int    RemainingPacketSize() { return m_maxPacketSize - m_curPos; }
	unsigned char * PacketData() { return m_packetBuffer; }
	unsigned int    PacketDataSize() { return m_curPos; }
	void            Increment(unsigned int numBytes) { m_curPos += numBytes; }

public:
	void            Reset();
	void            Pack(unsigned char * data, unsigned int dataSize);
	void            PackWord(unsigned int word);

private:
	unsigned int    m_maxPacketSize;
	unsigned int    m_curPos;
	unsigned char * m_packetBuffer;
};

#endif //__RTCPREPORTPACKET_H__