#ifndef __RTCPPARSER_H__
#define __RTCPPARSER_H__

#include "utility.h"

// RTCP Packet Type:
const unsigned char RTCP_PACKET_SR      = 200;
const unsigned char RTCP_PACKET_RR      = 201;
const unsigned char RTCP_PACKET_SDES    = 202;
const unsigned char RTCP_PACKET_BYE     = 203;
const unsigned char RTCP_PACKET_APP     = 204;

// SDES:
const unsigned char RTCP_SDES_END		= 0;
const unsigned char RTCP_SDES_CNAME		= 1;
const unsigned char RTCP_SDES_NAME		= 2;
const unsigned char RTCP_SDES_EMAIL		= 3;
const unsigned char RTCP_SDES_PHONE		= 4;
const unsigned char RTCP_SDES_LOC		= 5;
const unsigned char RTCP_SDES_TOOL		= 6;
const unsigned char RTCP_SDES_NOTE		= 7;
const unsigned char RTCP_SDES_PRIV		= 8;


class CSDESItem
{
public:
	CSDESItem(unsigned char tag, unsigned char * value)
	{
		unsigned int length = (unsigned int)strlen((char *)value);
		if(length > 255)
			length = 255;

		m_SDESData[0] = tag;
		m_SDESData[1] = (unsigned char)length;
		memcpy(m_SDESData+2, value, length);

		// Pad the trailing bytes to a 4-byte boundary:
		while((length) % 4 > 0) 
			m_SDESData[2 + length++] = '\0';
	}

	CSDESItem(unsigned char * SDESItemData)
	{
		unsigned char SDESTag = SDESItemData[0];
		unsigned char SDESLength = SDESItemData[1];
		if(SDESLength > 255)
			SDESLength = 255;

		memset(m_SDESData, 0, 2+0xFF);
		m_SDESData[0] = SDESTag;
		m_SDESData[1] = SDESLength;
		memcpy(m_SDESData+2, SDESItemData+2, SDESLength);
	}


	~CSDESItem(){}

public:
	unsigned char SDESTag() { return m_SDESData[0]; }
	unsigned char * SDESTotalItem() { return m_SDESData; }
	unsigned char * SDESData() { return m_SDESData+2; }
	unsigned int SDESDataSize() { return (unsigned int)m_SDESData[1]; }
	unsigned int TotalSize() { return 2 + (unsigned int)m_SDESData[1]; }

private:
	unsigned char m_SDESData[2 + 0xFF];
};

class CMediaStream;
class CMediaStreamSocket;

class CRTPSenderStatus;
class CRTCPMemberTable;
class CRTCPReportPacket;


class CRTCPParser
{
public:
	CRTCPParser(CMediaStream * mediaStream, CMediaStreamSocket * rtcpStreamSock, unsigned int totalSessionBandwidth);
	virtual ~CRTCPParser();

public:
	static int  ReadRTCPStream(void * param);
	int         ReadRTCPStreamOverTCP(PIVOSocket streamSock, int streamDataSize);
	int         ReadRTCPStreamFromLocal(unsigned char * streamData, int streamDataSize);
	virtual int ParseRTCPPacket();
	void		UpdateSocket(CMediaStreamSocket * socket);
public:
	CMediaStreamSocket  * RTCPStreamSock() { return m_rtcpStreamSock; }

protected:
	CMediaStream         * m_mediaStream;
	CMediaStreamSocket   * m_rtcpStreamSock;

protected:
	int                    m_rtcpPacketSize;
	unsigned char        * m_rtcpReceivePacket;
	CRTCPReportPacket    * m_rtcpReportPacket;

public: // rtcp receiver report
	unsigned int NumMembers();

protected: 
	void AddReport();
    void AddSR();
    void AddRR();
    void PackCommonReportPrefix(unsigned char packetType, unsigned int SSRC, unsigned int numExtraWords = 0);
    void PackCommonReportSuffix();
    void PackReportBlock(CRTPSenderStatus * senderStatus);
    void AddSDES();
    void AddBYE();
#if SUPPORT_ADAPTATION
	void AddNADU_APP();
	int  m_NADUCount;
#endif//SUPPORT_ADAPTATION
	void SendReportPacket();
	void SendReportPacketOverUDP();
	void SendReportPacketOverTCP();

public:
	static void OnExpire(CRTCPParser * rtcpParser);
	void DoExpire();

	void OnReceive(int typeOfPacket, int totalPacketSize, unsigned int SSRC);

public:
	void Schedule(long nextTime);
	void Reschedule(long nextTime);
	void Unschedule();
	void SendReport();
	void SendBYE();
	int	 TypeOfEvent() { return m_typeOfEvent; }
	int	 SentPacketSize() { return m_lastSentSize; }
	int	 PacketType() { return m_typeOfPacket; }
	int	 ReceivedPacketSize() { return m_lastReceivedSize; }
	int	 CheckNewSSRC();
	void RemoveLastReceivedSSRC();
	void RemoveSSRC(unsigned int SSRC, bool alsoRemoveStatus);

private:
	unsigned int		m_totalSessionBandwidth;
	CSDESItem		  *	m_CNAME;
	CRTCPMemberTable  * m_knownMembers;
	unsigned int		m_outgoingReportCount;

	double		        m_aveRTCPSize;
	int					m_isInitial;
	double				m_prevReportTime;
	double				m_nextReportTime;
	int					m_prevNumMembers;

	int					m_lastSentSize;
	int					m_lastReceivedSize;
	unsigned int		m_lastReceivedSSRC;
	int					m_typeOfEvent;
	int					m_typeOfPacket;
	bool				m_haveJustSentPacket;
	unsigned int		m_lastPacketSentSize;

	bool                m_streamingOverTCP;
	PIVOSocket          m_streamTCPSock;

private:
	long                m_taskToken;
	
private:
	int                 m_nSenderReportNum;
};


#endif //__RTCPPARSER_H__