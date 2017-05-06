#ifndef __RTPPARSER_H__
#define __RTPPARSER_H__

#include "network.h"
#include "RTPPacketReorder.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
class CMediaStream;
class CMediaStreamSocket;
class CRTPPacket;
class CRTPPacketReorder;
class CRTPSenderTable;
typedef struct  
{
	BYTE*		data;
	int			len;
	unsigned long	timestamp;
	unsigned short  seqNum;
	bool		isKeyFrame;
	bool		isFragment;
}FrameData;
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
	//int ReadRTPStreamOverTCP(PIVOSocket streamSock, int streamDataSize);
	int ReadRTPStreamFromLocal(unsigned char * streamData, int streamDataSize);
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
	virtual void            RenderFrame(unsigned int rtpTimestamp,int subFrame,unsigned int seqNum=0,bool IsKeyFrame=false);

	void            ReleaseReorderedRTPPackets();

public:
	unsigned int		 SSRC() { return m_SSRC; }
	CRTPSenderTable	   * RTPSenderTable() { return m_rtpSenderTable; }
	CMediaStreamSocket * RTPStreamSock() { return m_rtpStreamSock; }
	int					GetLastSeqNum(){return m_seqNum;}
	void				SetLastSeqNum(int seqNum){m_seqNum=seqNum;};
	unsigned int 		GetDuplicatePacketNum(){return m_rtpPacketReorder->m_duplicatePacketNum;};
	void				CleanFrameBuf();

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

private:
	unsigned int           	m_SSRC;
	unsigned int           	m_lastRTPTimestamp;
	unsigned int			m_antijitterBufferSize;
	VOTimeval         		m_lastRTPPacketReceivedTime;
	FrameData			    m_frame;
	
public:
	bool				   IsReadyParse();
	void SetFrameData(FrameData* frame);
	FrameData* GetFrameData();
	bool	IsValidPackBySeqNum(int seqNum);
	long					GetAntiJitterBufTime();
	long					GetAntiJitterBufSize(){return m_antijitterBufferSize;}
	void                EnlargeAntiJitterBuffer();
};



bool SeqNumLT(unsigned short seqNum1, unsigned short seqNum2);


#ifdef _VONAMESPACE
}
#endif


#endif //__RTPPARSER_H__
