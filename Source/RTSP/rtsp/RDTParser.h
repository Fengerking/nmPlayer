#ifndef _RDTPARSER_H_
#define _RDTPARSER_H_

#include "network.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

	class CMediaStreamSocket;
	class CRDTMediaStream;
	struct TNGDataPacket;
	struct TNGASMActionPacket;
	struct TNGBandwidthReportPacket;
	struct TNGReportPacket;
	struct TNGACKPacket;
	struct TNGRTTRequestPacket;
	struct TNGRTTResponsePacket;
	struct TNGCongestionPacket;
	struct TNGStreamEndPacket;
	struct TNGLatencyReportPacket;
	struct RDTTransportInfoRequestPacket;
	struct RDTBufferInfo;
	struct RDTTransportInfoResponsePacket;
	struct TNGBWProbingPacket;
/**
\brief RDT Parser
*/
class CRDTParser
{
public:
	CRDTParser(CMediaStreamSocket *rdtStreamSock);
	virtual ~CRDTParser(void);
public:
	int AddMediaStream(CRDTMediaStream* mediaStream);
	int CreateRDTServerSock(int serverPort);
	int SendRTTRequest(int count);
	int SendACK(struct TNGACKPacket	*m_ackPck);
	int ReadRDTStreamFromLocal(int channelID,unsigned char* data,int size);
	static int ReadRDTStream(void * param);
	void SetRTSPSock(IVOSocket* socketID){m_socketID = socketID;};
protected:
	int ReadRDTPacket();
	int HandleRDTPacket(unsigned char* data,int len);
	int Handle_RDT_DATA_PACKET(unsigned char* data,int len);
	int Handle_RDT_ASM_ACTION_PKT(unsigned char* data,int len);
	int Handle_RDT_BW_REPORT_PKT(unsigned char* data,int len);
	int Handle_ACK_PKT(unsigned char* data,int len);
	int Handle_RTT_REQUEST_PKT(unsigned char* data,int len);
	int Handle_RTT_RESPONSE_PKT(unsigned char* data,int len);
	int Handle_ACONGESTION_PKT(unsigned char* data,int len);
	int Handle_STREAM_END_PKT(unsigned char* data,int len);
	int Handle_REPORT_PKT(unsigned char* data,int len);
	int Handle_LATENCY_REPORT_PKT(unsigned char* data,int len);
	int Handle_TRANS_INFO_RQST_PKT(unsigned char* data,int len);
	int Handle_TRANS_INFO_RESP_PKT(unsigned char* data,int len);
	int Handle_BANDWIDTH_PROBE_PKT(unsigned char* data,int len);
	CRDTMediaStream* GetTheMediaStreamByID(int ID);
	int WriteRDTPacket(unsigned char* buf,int size);
	
private:
	unsigned char* m_rdtStreamBuf;
	CMediaStreamSocket* m_rdtStreamSock;
	CRDTMediaStream* m_mediaStreams[2];
	char	m_tmpbuf[512];
	int	m_mediaCount;
	int	m_nRDTPacketSize;
	struct TNGDataPacket				*m_dataPck;
	struct TNGASMActionPacket			*m_asmPck;
	struct TNGBandwidthReportPacket		*m_bwReportPck;
	struct TNGReportPacket				*m_reportPck;
	//struct TNGACKPacket					*m_ackPck;
	struct TNGRTTRequestPacket			*m_rttRequestPck;
	struct TNGRTTResponsePacket			*m_rttResponsePck;
	struct TNGCongestionPacket			*m_congestionPck;
	struct TNGStreamEndPacket			*m_streamEndPck;
	struct TNGLatencyReportPacket		*m_latencyReportPck;
	struct RDTTransportInfoRequestPacket	*m_TIRequestPck;
	//struct RDTBufferInfo					*m_rdtBufferInfoPck;
	struct RDTTransportInfoResponsePacket	*m_TIResponsePck;
	struct TNGBWProbingPacket				*m_bwProbingPck;
	struct sockaddr_storage					*m_rdtServerAddr;
	CMediaStreamSocket* m_rdtServerSock;
	IVOSocket* m_socketID;
};

#ifdef _VONAMESPACE
}
#endif

#endif//_RDTPARSER_H_