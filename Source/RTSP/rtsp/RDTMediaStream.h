#pragma once
#include "MediaStream.h"
#include "helix_types.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

struct TNGDataPacket;
struct ra_block_struct;
struct rv_frame_struct;
struct rm_stream_header_struct;
struct rm_packet_struct;
struct RDTBufferInfo;
struct TNGACKPacket;
class CRDTDataPacket
{
	struct rm_packet_struct* rmPacket;
public:
	int CopyTNGDataPacket(struct TNGDataPacket *data,int flag);
	struct rm_packet_struct* GetRMPacket(){return rmPacket;};
	void SetAsLostPck();
	unsigned int GetTimeStamp();
	unsigned int GetDataSize();
	int seq_no;

	CRDTDataPacket();
	~CRDTDataPacket();
};
#define MAX_RULE_NUM 32
#define MAX_RULEBOOK_SIZE MAX_RULE_NUM*128
/**
\brief real RDT media stream
*/
class CRDTMediaStream :public CMediaStream
{
public:
	CRDTMediaStream(CRTSPSession * rtspSession);
	virtual ~CRDTMediaStream(void);
	virtual bool Init(char *  &sdpParam);
	/**receive the RDT frame from RDT parser*/
	virtual void    RecvMediaFrame(unsigned char * frameData, int frameSize, unsigned int rtpTimestamp,unsigned int seqNum=0,bool IsKeyFrame=false);
	virtual void    FlushMediaStream(bool isReSync=true);
	int		ReceiveRDTDataPacket(struct TNGDataPacket* dataPck);
	int		GetStreamID();
	void	GetRDTBufInfo(struct RDTBufferInfo* info);
	bool	IsAppStream();
	int		GetASMRule2Stream(){return m_asmRule2Stream;}
	virtual long  	GetAntiJitterBufferTime(){return 0;}
	virtual long 	GetAntiJitterBufferSize(){return 0;}
protected:
	bool  CreateRDTPayloadParser();
	virtual int			 GetCodecHeadData(unsigned char * codecHead);
	virtual bool         ParseSDPAttribute_rtpmap(char * sdpParam);
	virtual bool		 ParseSDPAttribute_fmtp(char * sdpParam);
	virtual bool		 ParseSDPAttribute_OpaqueData(char* sdpParam);
	virtual bool		 ParseSDPAttribute_RMFF(char * sdpParam);
	virtual bool		 ParseSDPAttribute_ASMRuleBook(char * sdpParam);
	virtual bool		 ParseSDPAttribute_MaxBitRate(char * sdpParam);
	virtual bool		 ParseSDPAttribute_AvgBitRate(char * sdpParam);
	virtual bool		 ParseSDPAttribute_MaxPacketSize(char * sdpParam);
	virtual bool		 ParseSDPAttribute_AvgPacketSize(char * sdpParam);
	virtual bool		 ParseSDPAttribute_Preroll(char * sdpParam);
	virtual bool		 ParseSDPAttribute_mimetype(char * sdpParam);
	virtual bool		 ParseSDPAttribute_StreamName(char * sdpParam);
	virtual bool         ParseSDPParam_m(char * sdpParam);
	virtual bool         ParseSDPAttribute_control(char * sdpParam);
	virtual unsigned long HaveBufferedMediaStreamTime();

private:
	int m_streamID;
	unsigned char* m_frameData;
	int m_asmRule2Stream;
private://packet list
	list_T<CRDTDataPacket*, allocator<CRDTDataPacket*, 10> > m_listEmptyPacket;
	list_T<CRDTDataPacket*, allocator<CRDTDataPacket*, 10> > m_listReorderedPacket;
	bool                 m_haveSeenInitialSeqNum;
	unsigned short       m_nextExpectedSeqNum;
	int			GetStoredPacket(){return m_listReorderedPacket.size();};
	CRDTDataPacket* GetEmptyPacket();
	void         ReorderPacket(CRDTDataPacket* rdtPacket);
	CRDTDataPacket* GetNextReorderedPacket(bool & packetLoss);
	void         ReleasePacket(CRDTDataPacket* rdtPacket);
	void         ReleaseAllPackets();
	void		 Reset();
	int			 GetOneFrame();
	int			 SendACKPacket(int	isLost);
	void		 SetASMRule2Stream();
	//struct ra_block_struct* m_audioFrame;
	//struct rv_frame_struct* m_videoFrame;
	struct rm_stream_header_struct* m_streamHead;
	char*							m_rmff;
	char*							m_asmRuleBook;
	UINT16							m_asmRule2Flag[MAX_RULE_NUM];
	void*							m_depack;
	char					        m_mimeType[64];
	char							m_streamName[64];
	void							*m_codecInitParam;
	UINT32							m_nTimePerFrame;
	struct RDTBufferInfo			*m_rdtBufferInfo;
	struct TNGACKPacket				*m_ackPacket;
	INT32							m_previousSeq;
	INT32							m_lastSeq;

};
extern bool SeqNumLT2(unsigned short seqNum1, unsigned short seqNum2); 
/**
\brief the RDT media sample buffer manager
*/
class CRDTMediaFrameSink:public CMediaFrameSink
{
public:
	CRDTMediaFrameSink(CMediaStream* stream):CMediaFrameSink(stream){}
	~CRDTMediaFrameSink(){}
	virtual void SinkMediaFrame(unsigned char * frameData, int frameSize, unsigned long startTime, unsigned long endTime, bool isIntraFramem,int flag=0);

};

#ifdef _VONAMESPACE
}
#endif