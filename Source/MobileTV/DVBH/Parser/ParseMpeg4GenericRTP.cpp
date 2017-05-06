#include "commonheader.h"
#include "sdp_headers.h"
#include "MediaStream.h"
#include "ReorderRTPPacket.h"
#include "RTPPacket.h"
#include "SynchronizeStreams.h"
#include "ParseMpeg4GenericAudioMode.h"
#include "ParseMpeg4GenericRTP.h"

CParseMpeg4GenericRTP::CParseMpeg4GenericRTP(CMediaStream * pMediaStream)
: CParseRTP(pMediaStream)
, m_pParseMode(NULL)
{
}

CParseMpeg4GenericRTP::~CParseMpeg4GenericRTP()
{
	SAFE_DELETE(m_pParseMode);
}

VO_U32 CParseMpeg4GenericRTP::Init()
{
	VO_U32 rc = CParseRTP::Init();
	if(rc != VO_ERR_PARSER_OK)
		return rc;

	CSDPMedia * pSDPMedia = m_pMediaStream->GetSDPMediaDescription();

	char _fmtp_mode[16] = "generic";
	pSDPMedia->ParseAttribute_fmtp_mode(_fmtp_mode, 16);
	if(_stricmp(_fmtp_mode, "AAC-hbr") == 0)
	{
		m_pParseMode = new CParseAAChbrMode(this);
	}
	else if(_stricmp(_fmtp_mode, "AAC-lbr") == 0)
	{
		m_pParseMode = new CParseAAClbrMode(this);
	}
	else if(_stricmp(_fmtp_mode, "CELP-vbr") == 0)
	{
		m_pParseMode = new CParseCELPvbrMode(this);
	}
	else if(_stricmp(_fmtp_mode, "CELP-cbr") == 0)
	{
		m_pParseMode = new CParseCELPcbrMode(this);
	}
	else if(_stricmp(_fmtp_mode, "generic") == 0)
	{
		m_pParseMode = new CParseGenericMode(this);
	}
	else
	{
		m_pParseMode = NULL;
		return VO_ERR_PARSER_NOT_IMPLEMENT;
	}

	if(m_pParseMode == NULL)
		return VO_ERR_PARSER_OUT_OF_MEMORY;
	rc = m_pParseMode->Init();
	if(rc != VO_ERR_PARSER_OK)
		return rc;

	return VO_ERR_PARSER_OK;
}

VO_U32 CParseMpeg4GenericRTP::ParseRTPPayload()
{
	VO_U32 rc = VO_ERR_PARSER_OK;

	bool packetLoss = true;
	CRTPPacket * pRTPPacket = m_pReorderRTPPacket->GetNextReorderedRTPPacket(packetLoss);
	if(pRTPPacket == NULL)
		return VO_ERR_PARSER_OUT_OF_MEMORY;

	rc = m_pParseMode->ParseRTPPayload(pRTPPacket, packetLoss);

	m_pReorderRTPPacket->ReleaseRTPPacket(pRTPPacket);

	return rc;
}
