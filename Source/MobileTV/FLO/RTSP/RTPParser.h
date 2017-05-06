#ifndef __RTPPARSER_H__
#define __RTPPARSER_H__

#include "network.h"

#include "mswitch.h"

class CMediaStream;
class CMediaStreamSocket;
class CRTPPacket;
class CRTPPacketReorder;
class CRTPSenderTable;
typedef struct  
{
	unsigned char*		data;
	int			len;
	unsigned long	timestamp;
	unsigned short  seqNum;
	bool		isKeyFrame;
	bool		isFragment;
}FrameData;
const int MAX_FRAME_SIZE = 1024 * 32;
/**
\brief the RTP data parser

1,read the socket data
2,subclass parse the data 
*/
class CRTPParser
{
public:
	CRTPParser(CMediaStream * mediaStream, CMediaStreamSocket * rtpStreamSock);
	virtual ~CRTPParser();

public:
	/**read the rtp stream from socket buffer*/
	static int ReadRTPStream(void * param);

public:
	void		UpdateSocket(CMediaStreamSocket * socket);

public:
	/**parse the rtp payload */
	void     		ParseRTPPayload(int flag=0);
	void			RetrieveAllPack(int packetCounts=0x008000);
	virtual		bool		UpdateRTCP(CRTPPacket * rtpPacket);

	/**parse the rtp payload head,it is implemented by subClasss*/
	virtual bool    ParseRTPPayloadHeader(CRTPPacket * rtpPacket);
	/**parse the rtp payload frame,it is implemented by subClasss*/
	virtual bool    ParseRTPPayloadFrame(CRTPPacket * rtpPacket); 
	virtual bool	IsKeyFrame(){return false;}
	virtual int		GetRelativeTS(){return 0;}
	/**send the parsed data to mediastream*/
	void            RenderFrame(unsigned int rtpTimestamp,int subFrame,unsigned int seqNum=0,bool IsKeyFrame=false);

	void            ReleaseReorderedRTPPackets();

public:
	unsigned int		 SSRC() { return m_SSRC; }
	CRTPSenderTable	   * RTPSenderTable() { return m_rtpSenderTable; }
	CMediaStreamSocket * RTPStreamSock() { return m_rtpStreamSock; }
	int					GetLastSeqNum(){return m_seqNum;}
	void				SetLastSeqNum(int seqNum){m_seqNum=seqNum;};
protected:
	CMediaStream         * m_mediaStream;
	CMediaStreamSocket   * m_rtpStreamSock;
	CRTPPacketReorder    * m_rtpPacketReorder;
	CRTPSenderTable      * m_rtpSenderTable;

	unsigned char        * m_frameData;
	int                    m_frameSize;
	int					   m_seqNum;
protected:
	unsigned int           m_curRtpSeqNum;
	unsigned int           m_lastReceivedSSRC;

	bool                   m_packetLossInMultiPacketFrame;
	bool                   m_firstPacketInMultiPacketFrame;
	bool                   m_lastPacketInMultiPacketFrame;

	void Reset();
private:
	unsigned int           m_SSRC;
	unsigned int           m_lastRTPTimestamp;
#if CP_SOCKET
	struct timeval         m_lastRTPPacketReceivedTime;
#else
	struct timeval         m_lastRTPPacketReceivedTime;
#endif
	
	FrameData			   m_frame;
public:
	bool				   IsReadyParse();
	void SetFrameData(FrameData* frame);
	FrameData* GetFrameData();
	bool	IsValidPackBySeqNum(int seqNum);
	

private:
#if _DUMP_AV_RTP
	FILE    * m_hVideoRTP;
	FILE    * m_hAudioRTP;
#endif //_DUMP_AV_RTP
	
	unsigned int    m_SSRCChannelID;
	unsigned int	m_prevSSRCChannelID;
	bool	IsValidPackBySSRC(unsigned int ssrc,int seq);
	int				m_recvPackNum;
	unsigned int    m_channelBeginTime;
	int				m_firstChanSeqNum;
public:
	bool	FastChannelChange();
};



bool SeqNumLT(unsigned short seqNum1, unsigned short seqNum2);




#endif //__RTPPARSER_H__