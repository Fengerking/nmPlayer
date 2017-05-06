#ifndef __RTPPACKET_H__
#define __RTPPACKET_H__

class CRTPPacket
{
public:
	CRTPPacket();
	virtual ~CRTPPacket();

public:
	void			Reset();
	bool            AssignRTPData(unsigned char * _rtpData, int rtpDataSize);
	bool            ParseRTPCommonHeader();

public:
	int             Read(void * _buffer, int numOfBytesToRead);
	int             Write(void * _buffer, int numOfBytesToWrite);
	int             Peek(void * _buffer, int numOfBytesToPeek);
	void            Skip(int num);
	void            RemoveRTPPadding(int num);

public:
	unsigned char * RTPPacketData();
	int				RTPPacketSize();
	void            SetRTPPacketSize(int rtpPacketSize);
public:
	unsigned char * ReadPointer();
	void            ResetReadPointer();
	int             AvailDataSize();

public:
	bool            Marker();
	unsigned int    Timestamp();
	unsigned int    SSRC();
	unsigned int    PayloadType();
	unsigned short  SeqNum();

protected:
	unsigned char * m_rtpPacketData;
	int             m_rtpPacketSize;
protected:
	unsigned char * m_readPointer;

protected:
	bool            m_rtpMarker;
	unsigned int    m_rtpTimestamp;
	unsigned int    m_rtpSSRC;
	unsigned int    m_rtpPayloadType;
	unsigned short  m_rtpSeqNum;
};


#endif //__RTPPACKET_H__