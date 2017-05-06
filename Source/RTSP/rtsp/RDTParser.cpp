#include "utility.h"
#if ENABLE_REAL
#include "network.h"
#include "RDTParser.h"
#include <string.h>
#include "RTSPMediaStreamSyncEngine.h"
#include "RDTMediaStream.h"
#include "utility.h"
#include "MediaStreamSocket.h"
#include "RealTimeStreamingEngine.h"
#include "rdtpck.h"
#include "network.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

static const int  MAX_UDP_PACKET_SIZE= 1024 * 5;

CRDTParser::CRDTParser(CMediaStreamSocket *rdtStreamSock)
:m_rdtStreamSock(rdtStreamSock)
,m_rdtServerAddr(NULL)
,m_rdtServerSock(NULL)
{
	m_rdtStreamBuf = new unsigned char[MAX_UDP_PACKET_SIZE];TRACE_NEW("xxm_m_rdtStreamBuf=",m_rdtStreamBuf);
	m_mediaStreams[0]=m_mediaStreams[1]=NULL;
	m_nRDTPacketSize = 0;
	m_mediaCount = 0;
	m_socketID = m_rdtStreamSock->GetMediaStreamSock();
	m_dataPck = new struct TNGDataPacket();TRACE_NEW("xxm_m_dataPck=",m_dataPck);
	m_asmPck  = new struct TNGASMActionPacket();TRACE_NEW("xxm_m_dataPck",m_dataPck);
	m_bwReportPck	= new struct TNGBandwidthReportPacket();TRACE_NEW("xxm_m_bwReportPck",m_bwReportPck);
	m_reportPck		= new struct TNGReportPacket();TRACE_NEW("xxm_m_reportPck=",m_reportPck);
	//m_ackPck		= new struct TNGACKPacket();//					*m_ackPck;
	m_rttRequestPck	= new struct TNGRTTRequestPacket();TRACE_NEW("xxm_m_m_rttRequestPck",m_rttRequestPck);		
	m_rttResponsePck	= new struct TNGRTTResponsePacket();TRACE_NEW("xxm_m_rttResponsePck=",m_rttResponsePck);		
	m_congestionPck		= new struct TNGCongestionPacket();TRACE_NEW("xxm_m_congestionPck=",m_congestionPck);
	m_streamEndPck		= new struct TNGStreamEndPacket();TRACE_NEW("xxm_m_streamEndPck",m_streamEndPck);
	m_latencyReportPck	= new struct TNGLatencyReportPacket();TRACE_NEW("xxm_m_streamEndPck",m_streamEndPck);
	m_TIRequestPck		= new struct RDTTransportInfoRequestPacket();TRACE_NEW("xxm_m_TIRequestPck",m_TIRequestPck);
	//m_rdtBufferInfoPck	= new struct RDTBufferInfo();//					*m_rdtBufferInfoPck;
	m_TIResponsePck		= new struct RDTTransportInfoResponsePacket();TRACE_NEW("xxm_m_TIResponsePck",m_TIResponsePck);
	m_TIResponsePck->buffer_info = new struct RDTBufferInfo[2];TRACE_NEW("xxm_m_TIResponsePck->buffer_info=",m_TIResponsePck->buffer_info);
	m_bwProbingPck		= new struct TNGBWProbingPacket();TRACE_NEW("xxm_m_bwProbingPck=",m_bwProbingPck);
	
}

CRDTParser::~CRDTParser(void)
{
	SAFE_DELETE(m_rdtStreamSock);
	SAFE_DELETE_ARRAY(m_rdtStreamBuf);
	SAFE_DELETE(m_dataPck); //= new struct TNGDataPacket();
	SAFE_DELETE(m_asmPck); //  = new struct TNGASMActionPacket();
	SAFE_DELETE(m_bwReportPck); //	= new struct TNGBandwidthReportPacket();
	SAFE_DELETE(m_reportPck	); //	= new struct TNGReportPacket();
	//SAFE_DELETE(m_ackPck); //		= new struct TNGACKPacket();//					*m_ackPck;
	SAFE_DELETE(m_rttRequestPck); //	= new struct TNGRTTRequestPacket();//			*;
	SAFE_DELETE(m_rttResponsePck); //	= new struct TNGRTTResponsePacket();//			*;
	SAFE_DELETE(m_congestionPck); //		= new struct TNGCongestionPacket();//			*m_congestionPck;
	SAFE_DELETE(m_streamEndPck); //		= new struct TNGStreamEndPacket();//			*;
	SAFE_DELETE(m_latencyReportPck); //	= new struct TNGLatencyReportPacket();//		*m_latencyReportPck;
	SAFE_DELETE(m_TIRequestPck); //	= new struct RDTTransportInfoRequestPacket();//	*m_TIRequestPck;
	//SAFE_DELETE(m_rdtBufferInfoPck); //	= new struct RDTBufferInfo();//					*m_rdtBufferInfoPck;
	SAFE_DELETE_ARRAY(m_TIResponsePck->buffer_info);
	SAFE_DELETE(m_TIResponsePck); //		= new struct RDTTransportInfoResponsePacket();//	*m_TIResponsePck;
	SAFE_DELETE(m_bwProbingPck); //	= new struct TNGBWProbingPacket();//				*m_bwProbingPck;
	SAFE_DELETE(m_rdtServerAddr);
	SAFE_DELETE(m_rdtServerSock);
}
int CRDTParser::AddMediaStream(CRDTMediaStream* mediaStream)
{
	//Assume there is 2 streams at most
	if(m_mediaStreams[0]==NULL)
	{
		m_mediaStreams[0] = mediaStream;
		m_mediaCount++;
	}
	else
	{
		m_mediaStreams[1] = mediaStream;
		m_mediaCount++;
	}
	return 0;
}
int CRDTParser::CreateRDTServerSock(int serverPort)
{
	if(m_rdtServerAddr==NULL)
	{
		m_rdtServerAddr = new struct sockaddr_storage();TRACE_NEW("new m_rdtServerAddr",m_rdtServerAddr);
		memcpy(m_rdtServerAddr, m_mediaStreams[0]->MediaAddr(), sizeof(sockaddr_storage));
		m_rdtServerAddr->port = IVOSocket::HostToNetworkShort(serverPort);
	}
	
	return 0;
}
int CRDTParser::WriteRDTPacket(unsigned char* buf,int size)
{
	if(CUserOptions::UserOptions.m_useTCP>0)
	{

		if(::SendTCPData(m_socketID,(char*)buf,size,0) == SOCKET_ERROR)
		{
			CUserOptions::UserOptions.errorID = E_RTSP_SEND_FAILED;
			int err = GetSocketError();
			char* format = CLog::formatString;
			sprintf(format,"@@@@@@@@@@@@fail to WriteRDTPacket err=%d\r\n",err);
			CLog::Log.MakeLog(LL_SOCKET_ERR,"socketErr.txt",format);
		}

	}
	else
	{
		return WriteNetData(m_socketID, m_rdtServerAddr, buf, size);
	}
	return 0;
}
int CRDTParser:: SendACK(struct TNGACKPacket *ackPacket)
{
	UINT8* buf = (UINT8*)m_tmpbuf;
	UINT8* off = TNGACKPacket_pack(buf,3,ackPacket);
	WriteRDTPacket(buf,off-buf);
	return 0;
}
int CRDTParser::SendRTTRequest(int count)
{
	return 0;

	struct TNGRTTRequestPacket rttRequest={0};
	rttRequest.packet_type = RDT_RTT_REQUEST_PKT;
	UINT8 buf[3];
	while (count-->0)
	{
		TNGRTTRequestPacket_pack(buf,3,&rttRequest);
		WriteRDTPacket(buf,3);
		IVOThread::Sleep(5);
		ReadRDTPacket();
	}
	return 0;
}
int CRDTParser::ReadRDTStreamFromLocal(int channelID,unsigned char* data,int size)
{

	return HandleRDTPacket(data,size);
}

int CRDTParser::HandleRDTPacket(BYTE* data,int len)
{
	UINT16 unPacketType;
	int ret=0;
	unPacketType = GetRDTPacketType(data,len);
	switch( unPacketType )
	{
	case RDT_DATA_PACKET:
		ret =Handle_RDT_DATA_PACKET(data,len);
		break;
	case RDT_ASM_ACTION_PKT:
		ret =Handle_RDT_ASM_ACTION_PKT(data,len);
		break;
	case RDT_BW_REPORT_PKT:
		ret =Handle_RDT_BW_REPORT_PKT(data,len);
		break;
	case RDT_ACK_PKT:
		ret =Handle_ACK_PKT(data,len);
		break;
	case RDT_RTT_REQUEST_PKT:
		ret =Handle_RTT_REQUEST_PKT(data,len);
		break;
	case RDT_RTT_RESPONSE_PKT:
		ret =Handle_RTT_RESPONSE_PKT(data,len);
		break;
	case RDT_CONGESTION_PKT:
		ret =Handle_ACONGESTION_PKT(data,len);
		break;
	case RDT_STREAM_END_PKT:
		ret =Handle_STREAM_END_PKT(data,len);
		break;
	case RDT_REPORT_PKT:
		ret =Handle_REPORT_PKT(data,len);
		break;
	case RDT_LATENCY_REPORT_PKT:
		ret =Handle_LATENCY_REPORT_PKT(data,len);
		break;
	case RDT_TRANS_INFO_RQST_PKT:
		ret =Handle_TRANS_INFO_RQST_PKT(data,len);
		break;
	case RDT_TRANS_INFO_RESP_PKT:
		ret =Handle_TRANS_INFO_RESP_PKT(data,len);
		break;
	case RDT_BANDWIDTH_PROBE_PKT:
		ret =Handle_BANDWIDTH_PROBE_PKT(data,len);
		break;
	case RDT_UNKNOWN_TYPE:
		CLog::Log.MakeLog(LL_RTP_ERR,"rdt.txt","RDT_UNKNOWN_TYPE\n");
		break;
	default:
		CLog::Log.MakeLog(LL_RTP_ERR,"rdt.txt","WRONG TYPE,undefined\n");
		break;
	}
	return ret;
}
int CRDTParser::ReadRDTPacket()
{
	if(CUserOptions::UserOptions.m_useTCP>0)
		return 0;

	m_nRDTPacketSize = ReadNetData(m_socketID, m_rdtStreamBuf, MAX_UDP_PACKET_SIZE);
	int ret = 0;
	if(m_nRDTPacketSize>0)
	{
		ret = HandleRDTPacket(m_rdtStreamBuf, m_nRDTPacketSize);
	}
	return ret;
}
int CRDTParser::ReadRDTStream(void * param)
{
	CRDTParser   * rdtParser = (CRDTParser *)param;
	return rdtParser->ReadRDTPacket();
	
}
CRDTMediaStream* CRDTParser::GetTheMediaStreamByID(int ID)
{
	for(int i=0;i<m_mediaCount;i++)
	{
		if(m_mediaStreams[i]->GetStreamID()==ID)
			return m_mediaStreams[i];
	}
	return NULL;
}

int CRDTParser::Handle_RDT_DATA_PACKET(BYTE* data,int len)
{

	unsigned char* off = data;
	int leftSize = len;
	int result=0;
	if (len>0)
	{
		TNGDataPacket_unpack(off, leftSize, m_dataPck);
		if(m_dataPck->length_included_flag)
		{	
			TNGDataPacket_unpack(off, m_dataPck->_packlenwhendone, m_dataPck);
			off+=m_dataPck->_packlenwhendone;
			leftSize -= m_dataPck->_packlenwhendone;
			
		}
		else
		{
			leftSize = 0;
		}
		CRDTMediaStream* mediaStream = GetTheMediaStreamByID(m_dataPck->stream_id);
		if(mediaStream)
		{
			if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
			{
				sprintf(CLog::formatString,"seq=%d,s=%lu,r=%d,t=%lu\n",
					m_dataPck->seq_no,
					m_dataPck->data.len,
					m_dataPck->asm_rule_number,
					m_dataPck->timestamp);
				if(mediaStream->IsVideoStream())
				{
					CLog::Log.MakeLog(LL_RTP_ERR,"VideoRDTData.txt",CLog::formatString);
					//return 0;
				}
				else if(mediaStream->IsAudioStream())
				{
					CLog::Log.MakeLog(LL_RTP_ERR,"AudioRDTData.txt",CLog::formatString);
					//return 0;
				}
				else
				{
					CLog::Log.MakeLog(LL_RTP_ERR,"ApplicationRDTData.txt",CLog::formatString);

				}

			}
			result=mediaStream->ReceiveRDTDataPacket(m_dataPck);
		}
		if(leftSize)
		{
			return HandleRDTPacket(off,leftSize);
		}
	}
	
	return result;
}
int CRDTParser::Handle_RDT_ASM_ACTION_PKT(BYTE* data,int len)
{
	return 0;

}
int CRDTParser::Handle_RDT_BW_REPORT_PKT(BYTE* data,int len)
{
	return 0;

}
int CRDTParser::Handle_ACK_PKT(BYTE* data,int len)
{
	return 0;

}
int CRDTParser::Handle_RTT_REQUEST_PKT(BYTE* data,int len)
{
	unsigned char* off = data;
	int leftSize = len;
	if (len>0)
	{
		struct TNGRTTRequestPacket packet;
		BYTE* off1 =TNGRTTRequestPacket_pack(off, leftSize, &packet);
		int size = off1-off;

		if(leftSize>size)
		{	
			off+=size;
			leftSize -= size;

		}
		else
		{
			leftSize = 0;
		}

		if(leftSize)
		{
			return HandleRDTPacket(off,leftSize);
		}
	}

	return 0;

}
int CRDTParser::Handle_RTT_RESPONSE_PKT(BYTE* data,int len)
{
	return 0;
}
int CRDTParser::Handle_ACONGESTION_PKT(BYTE* data,int len)
{
	return 0;
}
int CRDTParser::Handle_STREAM_END_PKT(BYTE* data,int len)
{
	return 0;

}
int CRDTParser::Handle_REPORT_PKT(BYTE* data,int len)
{
	return 0;

}
int CRDTParser::Handle_LATENCY_REPORT_PKT(BYTE* data,int len)
{
	unsigned char* off = data;
	int leftSize = len;
	if (len>0)
	{
		TNGLatencyReportPacket_unpack(off, leftSize, m_latencyReportPck);
		if(m_latencyReportPck->length_included_flag)
		{	
			TNGLatencyReportPacket_unpack(off, m_latencyReportPck->length, m_latencyReportPck);
			off+=m_latencyReportPck->length;
			leftSize -= m_latencyReportPck->length;

		}
		else
		{
			leftSize = 0;
		}
	
		if(leftSize)
		{
			return HandleRDTPacket(off,leftSize);
		}
	}

	return 0;

}
int CRDTParser::Handle_TRANS_INFO_RQST_PKT(BYTE* data,int len)
{
	unsigned char* off = data;
	int leftSize = len;
	if (len>0)
	{
		BYTE* off1 = RDTTransportInfoRequestPacket_unpack(off, leftSize, m_TIRequestPck);
		int size = off1-off; 
		if(leftSize>size)
		{	
			off+=size;
			leftSize -= size;

		}
		else
		{
			leftSize = 0;
		}
		
		//response
		m_TIResponsePck->dummy0 = 0;
		m_TIResponsePck->dummy1 = 0;
		if(m_TIRequestPck->request_rtt_info)
		{
			m_TIResponsePck->has_rtt_info = 1;
			m_TIResponsePck->request_time_ms  = m_TIRequestPck->request_time_ms;
			m_TIResponsePck->response_time_ms = 50;
		}
		if(m_TIRequestPck->request_buffer_info)
		{
			m_TIResponsePck->has_buffer_info =1;
			m_TIResponsePck->buffer_info_count = m_mediaCount;
			for (int i=0;i<m_mediaCount;i++)
			{
				m_mediaStreams[i]->GetRDTBufInfo(&(m_TIResponsePck->buffer_info[i]));
			}
		}
		m_TIResponsePck->packet_type = RDT_TRANS_INFO_RESP_PKT;
		m_TIResponsePck->is_delayed	 = 0;

		if(leftSize)
		{
			return HandleRDTPacket(off,leftSize);
		}
	}

	return 0;

}
int CRDTParser::Handle_TRANS_INFO_RESP_PKT(BYTE* data,int len)
{
	unsigned char* off = data;
	int leftSize = len;
	if (len>0)
	{
		BYTE* off1 = RDTTransportInfoResponsePacket_unpack(off, leftSize, m_TIResponsePck);
		int size = off1-off; 
		if(leftSize>size)
		{	
			//TNGLatencyReportPacket_unpack(off, packet.length, &packet);
			off+=size;
			leftSize -= size;

		}
		else
		{
			leftSize = 0;
		}

		if(leftSize)
		{
			return HandleRDTPacket(off,leftSize);
		}
	}

	return 0;

}
int CRDTParser::Handle_BANDWIDTH_PROBE_PKT(unsigned char* data,int len)
{
	return 0;
}
#endif//#if ENABLE_REAL
