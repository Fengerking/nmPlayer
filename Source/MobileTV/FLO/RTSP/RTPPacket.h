#ifndef __RTPPACKET_H__
#define __RTPPACKET_H__

#include "network.h"
#include "utility.h"

class CMediaStreamSocket;

class CRTPPacket
{
public:
	CRTPPacket();
	virtual ~CRTPPacket();

public:
	void			Reset();
	int				ReadRTPPacket(CMediaStreamSocket * rtpStreamSock);

	bool            ParseRTPPacketHeader();
	unsigned char * RTPPacket() { return m_rtpStreamBuf; }
	int				RTPPacketSize() { return m_rtpPacketSize; }
	unsigned char * RTPData() { return m_parsePos; }
	int             RTPDataSize() { return m_rtpStreamBuf + m_rtpPacketSize - m_parsePos; }
	void            Skip(int num);
	void            RemoveRTPPadding(int num);

	bool            RTPMarker() { return m_rtpMarker; }
	unsigned int    RTPTimestamp() { return m_rtpTimestamp; }
	unsigned int    RTPSSRC() { return m_rtpSSRC; }
	unsigned int    RTPPayloadType() { return m_rtpPayloadType; }
	unsigned short  RTPSeqNum() { return m_rtpSeqNum; }

	struct timeval & PacketReceivedTime() { return m_packetReceivedTime; }

	void            SetRTPTimestamp(unsigned int rtpTimestamp) { m_rtpTimestamp = rtpTimestamp; }
	bool			DecryptFromSRTP();
protected:
	unsigned char      * m_rtpStreamBuf;
	int                  m_rtpPacketSize;
	unsigned char      * m_parsePos;

protected:
	bool                 m_rtpMarker;
	unsigned int         m_rtpTimestamp;
	unsigned int         m_rtpSSRC;
	unsigned int         m_rtpPayloadType;
	unsigned short       m_rtpSeqNum;

protected:
	struct timeval       m_packetReceivedTime;
};


#endif //__RTPPACKET_H__
