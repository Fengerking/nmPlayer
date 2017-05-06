#include "commonheader.h"
#include "sdp_headers.h"
#include "MediaStream.h"
#include "ReorderRTPPacket.h"
#include "RTPPacket.h"
#include "ParseMpeg4GenericAudioMode.h"
#include "ParseMpeg4GenericAudioRTP.h"


CParseMpeg4GenericAudioRTP::CParseMpeg4GenericAudioRTP(CMediaStream * pMediaStream)
: CParseRTP(pMediaStream)
, m_pBaseParseMode(NULL)
{
}

CParseMpeg4GenericAudioRTP::~CParseMpeg4GenericAudioRTP()
{
	SAFE_DELETE(m_pBaseParseMode);
}

VO_U32 CParseMpeg4GenericAudioRTP::Init()
{
	VO_U32 rc = CParseRTP::Init();
	if(rc != VO_ERR_PARSER_OK)
		return rc;

	CSDPMedia * pSDPMedia = m_pMediaStream->GetSDPMediaDescription();

	char _fmtp_mode[16];
	if(!pSDPMedia->ParseAttribute_fmtp_mode(_fmtp_mode, 16))
	{
		strcpy(_fmtp_mode, "generic");
	}

	if(_stricmp(_fmtp_mode, "AAC-hbr") == 0)
	{
		m_pBaseParseMode = new CParseAAChbrMode(this);
	}
	else if(_stricmp(_fmtp_mode, "AAC-lbr") == 0)
	{
		m_pBaseParseMode = new CParseAAClbrMode(this);
	}
	else if(_stricmp(_fmtp_mode, "CELP-vbr") == 0)
	{
		m_pBaseParseMode = new CParseCELPvbrMode(this);
	}
	else if(_stricmp(_fmtp_mode, "CELP-cbr") == 0)
	{
		m_pBaseParseMode = new CParseCELPcbrMode(this);
	}
	else if(_stricmp(_fmtp_mode, "generic") == 0)
	{
		m_pBaseParseMode = new CParseGenericMode(this);
	}
	else
	{
		m_pBaseParseMode = NULL;
	}

	if(m_pBaseParseMode == NULL)
		return VO_ERR_PARSER_ERROR;

	rc = m_pBaseParseMode->Init();
	if(rc != VO_ERR_PARSER_OK)
		return rc;

	return VO_ERR_PARSER_OK;
}

VO_U32 CParseMpeg4GenericAudioRTP::ParseRTPPayload()
{
	VO_U32 rc = VO_ERR_PARSER_ERROR;

	bool packetLoss = true;
	CRTPPacket * pRTPPacket = m_pReorderRTPPacket->GetNextReorderedRTPPacket(packetLoss);
	if(pRTPPacket == NULL)
		return VO_ERR_PARSER_OUT_OF_MEMORY;

	rc = m_pBaseParseMode->ParseRTPPayload(pRTPPacket, packetLoss);

	m_pReorderRTPPacket->ReleaseRTPPacket(pRTPPacket);

	return rc;
}
