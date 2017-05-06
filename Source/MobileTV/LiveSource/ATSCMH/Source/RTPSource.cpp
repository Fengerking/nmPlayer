#include "sdp_headers.h"
#include "StreamChannel.h"
#include "StreamDataChannel.h"
#include "StreamControlChannel.h"
#include "SourceEngine.h"
#include "RTPSource.h"

#ifdef _RTPSRC_LOG
#include "logtool.h"
#endif //_RTPSRC_LOG

CRTPSource::CRTPSource()
: m_pSourceEngine(NULL)
, m_pSDPData(NULL)
, m_pSDPParse(NULL)
{
	memset(&m_InitInfo, 0, sizeof(VO_RTPSRC_INIT_INFO));
}

CRTPSource::~CRTPSource()
{
	Close();
}

VORC_RTPSRC CRTPSource::Open(char * pSDPData, VO_RTPSRC_INIT_INFO * pInitInfo)
{
#ifdef _RTPSRC_LOG
	CLogTool::LogTool.EnableLog(true);
#endif //_RTPSRC_LOG

	size_t nSDPSize = strlen(pSDPData);
	m_pSDPData = new char[(nSDPSize + 4) / 4 * 4];
	if(m_pSDPData == NULL)
		return VORC_RTPSRC_OUT_OF_MEMORY;

	strcpy(m_pSDPData, pSDPData);

	m_InitInfo = *pInitInfo;


	VORC_RTPSRC rc = VORC_RTPSRC_OK;

	m_pSourceEngine = new CSourceEngine(this);
	if(m_pSourceEngine == NULL)
		return VORC_RTPSRC_OUT_OF_MEMORY;

	m_pSDPParse = new CSDPParse();
	if(m_pSDPParse == NULL)
		return VORC_RTPSRC_OUT_OF_MEMORY;

	if(!m_pSDPParse->OpenSDP(m_pSDPData))
		return VORC_RTPSRC_INVALID_ARG;

	for(int index=0; index<m_pSDPParse->GetSDPMediaCount(); ++index)
	{
		CSDPMedia * pSDPMedia = m_pSDPParse->GetSDPMedia(index);

		CStreamDataChannel * pDataChannel = new CStreamDataChannel(&m_InitInfo);
		if(pDataChannel == NULL)
			return VORC_RTPSRC_OUT_OF_MEMORY;
		rc = pDataChannel->Init(pSDPMedia);
		if(rc != VORC_RTPSRC_OK)
		{
			SAFE_DELETE(pDataChannel);
			return rc;
		}
		m_pSourceEngine->AddStreamChannel(pDataChannel);

		CStreamControlChannel * pControlChannel = new CStreamControlChannel(&m_InitInfo);
		if(pControlChannel == NULL)
			return VORC_RTPSRC_OUT_OF_MEMORY;
		rc = pControlChannel->Init(pSDPMedia);
		if(rc != VORC_RTPSRC_OK)
		{
			SAFE_DELETE(pControlChannel);
			return rc;
		}
		m_pSourceEngine->AddStreamChannel(pControlChannel);
	}

	SendSDPData();

	return VORC_RTPSRC_OK;
}

VORC_RTPSRC CRTPSource::Close()
{
	SAFE_DELETE(m_pSourceEngine);
	SAFE_DELETE_ARRAY(m_pSDPData);
	SAFE_DELETE(m_pSDPParse);

	return VORC_RTPSRC_OK;
}

VORC_RTPSRC CRTPSource::Start()
{
#ifdef _RTPSRC_LOG
	DumpStreamPacket(PACKET_SDP, m_pSDPData, strlen(m_pSDPData));
#endif //_RTPSRC_LOG

	return m_pSourceEngine->Start();
}

VORC_RTPSRC CRTPSource::Stop()
{
	return m_pSourceEngine->Stop();
}

VORC_RTPSRC CRTPSource::SetParam(VO_U32 uID, VO_PTR pParam)
{
	return VORC_RTPSRC_NOT_IMPLEMENT;
}

VORC_RTPSRC CRTPSource::GetParam(VO_U32 uID, VO_PTR pParam)
{
	return VORC_RTPSRC_NOT_IMPLEMENT;
}

VORC_RTPSRC CRTPSource::SendData(VO_PBYTE pData, VO_U32 nSize)
{
	return m_pSourceEngine->SendData(pData, nSize);
}

VORC_RTPSRC CRTPSource::SendSDPData()
{
	VO_RTPSRC_DATA SDPData;
	SDPData.nChannelID = PACKET_SDP;
	SDPData.pData = m_pSDPData;
	SDPData.nDataSize = strlen(m_pSDPData);
	SDPData.pUserData = m_InitInfo.pUserData;

	m_InitInfo.pProc(&SDPData);

	return VORC_RTPSRC_OK;
}
