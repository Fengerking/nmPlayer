#include "commonheader.h"
#include "sdp_headers.h"
#include "MediaStream.h"
#include "ParseEncMpeg4GenericAudioMode.h"
#include "ParseEncMpeg4GenericVideoMode.h"
#include "ParseMpeg4GenericRTP.h"
#include "ParseEncMpeg4GenericRTP.h"

CParseEncMpeg4GenericRTP::CParseEncMpeg4GenericRTP(CMediaStream * pMediaStream)
: CParseMpeg4GenericRTP(pMediaStream)
{
}

CParseEncMpeg4GenericRTP::~CParseEncMpeg4GenericRTP()
{
}

VO_U32 CParseEncMpeg4GenericRTP::Init()
{
	VO_U32 rc = CParseRTP::Init();
	if(rc != VO_ERR_PARSER_OK)
		return rc;

	CSDPMedia * pSDPMedia = m_pMediaStream->GetSDPMediaDescription();

	char _fmtp_mode[16] = "";
	pSDPMedia->ParseAttribute_fmtp_mode(_fmtp_mode, 16);
	if(_stricmp(_fmtp_mode, "AAC-hbr") == 0)
	{
		m_pParseMode = new CParseEncAAChbrMode(this);
	}
	else if(_stricmp(_fmtp_mode, "AAC-lbr") == 0)
	{
		m_pParseMode = new CParseEncAAClbrMode(this);
	}
	else if(_stricmp(_fmtp_mode, "CELP-vbr") == 0)
	{
		m_pParseMode = new CParseEncCELPvbrMode(this);
	}
	else if(_stricmp(_fmtp_mode, "CELP-cbr") == 0)
	{
		m_pParseMode = new CParseEncCELPcbrMode(this);
	}
	else if(_stricmp(_fmtp_mode, "mpeg4-video") == 0)
	{
		m_pParseMode = new CParseMpeg4VideoMode(this);
	}
	else if(_stricmp(_fmtp_mode, "avc-video") == 0)
	{
		m_pParseMode = new CParseAVCVideoMode(this);
	}
	else
	{
		m_pParseMode = NULL;
		return VO_ERR_PARSER_NOT_IMPLEMENT;
	}

	if(m_pParseMode == NULL)
		return VO_ERR_PARSER_NOT_IMPLEMENT;
	rc = m_pParseMode->Init();
	if(rc != VO_ERR_PARSER_OK)
		return rc;

	return VO_ERR_PARSER_OK;
}
