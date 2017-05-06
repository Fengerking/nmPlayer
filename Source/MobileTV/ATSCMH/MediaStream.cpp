#include "vortpbase.h"
#include "commonheader.h"
#include "sdp_headers.h"
#include "ParseRTP.h"
#include "ParseRTCP.h"
#include "MediaStream.h"

CMediaStream::CMediaStream(VO_PARSER_INIT_INFO * pParserInitInfo)
: m_pParserInitInfo(pParserInitInfo)
, m_pSDPMedia(NULL)
, m_pParseRTP(NULL)
, m_pParseRTCP(NULL)
, m_clockRate(0)
{
	memset(&m_streamInfo, 0, sizeof(STREAMINFO));

	m_syncCount = 0;
	memset(&m_syncWallClockTime, 0, sizeof(timeval));
	m_syncTimestamp = 0;
	memset(&m_wallClockTime, 0, sizeof(timeval));
	m_timestamp = 0;
}

CMediaStream::~CMediaStream()
{
	SAFE_DELETE_ARRAY(m_streamInfo.pExtraData);
	SAFE_DELETE(m_pParseRTP);
	SAFE_DELETE(m_pParseRTCP);
}

VO_U32 CMediaStream::Init(CSDPMedia * pSDPMedia)
{
	m_pSDPMedia = pSDPMedia;
	return VO_ERR_PARSER_OK;
}

VO_U32 CMediaStream::Process(VO_PARSER_INPUT_BUFFER * pBuffer)
{
	VO_U32 channel = pBuffer->nStreamID;
	VO_PBYTE data = pBuffer->pBuf;
	VO_U32 datasize = pBuffer->nBufLen;

	if(PACKET_CTRL & channel)
	{
		return m_pParseRTCP->Process(data, datasize);
	}
	else
	{
		return m_pParseRTP->Process(data, datasize);
	}

	return VO_ERR_PARSER_FAIL;
}

VO_PARSER_INIT_INFO * CMediaStream::GetParserInitInfo() 
{
	return m_pParserInitInfo;
}

STREAMINFO * CMediaStream::GetStreamInfo()
{
	return &m_streamInfo;
}

CSDPMedia * CMediaStream::GetSDPMediaDescription()
{
	return m_pSDPMedia;
}

int CMediaStream::GetStreamId()
{
	return m_streamInfo.nStreamId;
}

int CMediaStream::ParseStreamId()
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
