#include "sdp_headers.h"
#include "ipnetwork.h"
#include "StreamChannel.h"

#ifdef _RTPSRC_LOG
#include "logtool.h"
#endif //_RTPSRC_LOG

CStreamChannel::CStreamChannel(VO_RTPSRC_INIT_INFO * pInitInfo)
: m_pInitInfo(pInitInfo)
, m_channelID(0)
, m_pIPSocket(NULL)
, m_transPort(0)
, m_buffer(NULL)
, m_pSDPMedia(NULL)
{
}

CStreamChannel::~CStreamChannel()
{
}

VORC_RTPSRC CStreamChannel::Init(CSDPMedia * pSDPMedia)
{
	m_pSDPMedia = pSDPMedia;

	return VORC_RTPSRC_OK;
}

VORC_RTPSRC CStreamChannel::Start()
{
	return VORC_RTPSRC_OK;
}

VORC_RTPSRC CStreamChannel::Stop()
{
	return VORC_RTPSRC_OK;
}

VORC_RTPSRC CStreamChannel::OnPacketArrived(void * _data, int datasize)
{
#ifdef _RTPSRC_LOG
	DumpStreamPacket(m_channelID, _data, datasize);
	DumpStreamPacketInfo(m_channelID, datasize);
#endif //_RTPSRC_LOG

	VO_RTPSRC_DATA StreamData;
	StreamData.nChannelID = m_channelID;
	StreamData.pData = _data;
	StreamData.nDataSize = datasize;
	StreamData.pUserData = m_pInitInfo->pUserData;

	m_pInitInfo->pProc(&StreamData);

	return VORC_RTPSRC_OK;
}

int CStreamChannel::GetChannelSocket()
{
	if(m_pIPSocket == NULL)
		return INVALID_SOCKET;

	return (int)(m_pIPSocket->GetSocket());
}

int CStreamChannel::ParseStreamId()
{
	int streamID = 0;
	if(!m_pSDPMedia->ParseAttribute_mid(&streamID))
	{
		if(!m_pSDPMedia->ParseAttribute_mpeg4_esid(&streamID))
		{
			streamID = m_pSDPMedia->GetIndex();
		}
	}

	return streamID;
}


#ifdef _RTPSRC_LOG
void DumpStreamPacket(int channelID, void * _packet, int packetLength)
{
	CLogTool::LogTool.LogOutput(_T("Streams.rtp"), &channelID, sizeof(channelID));
	CLogTool::LogTool.LogOutput(_T("Streams.rtp"), &packetLength, sizeof(packetLength));
	CLogTool::LogTool.LogOutput(_T("Streams.rtp"), _packet, packetLength);
}

void DumpStreamPacketInfo(int channelID, int packetLength)
{
	static DWORD dwBaseTime = GetTickCount();
	DWORD dwCurrentTime = GetTickCount();
	CLogTool::LogTool.LogOutput(_T("packetInfo.txt"), "channelID = %u,    recvTime = %u    packetSize = %d  \r\n", channelID&0xffff, dwCurrentTime-dwBaseTime, packetLength);
}
#endif //_RTPSRC_LOG
