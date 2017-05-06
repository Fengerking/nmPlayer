#include "AudioStream.h"
#include "commonheader.h"
#include "sdp_headers.h"
#include "parseutil.h"
#include "ParseRTP.h"
#include "ParseRTCP.h"
#include "ParseMpeg4GenericRTP.h"
#include "ParseEncMpeg4GenericRTP.h"
#include "ParseMp4aLatmRTP.h"

CAudioStream::CAudioStream(VO_PARSER_INIT_INFO * pParserInitInfo)
: CMediaStream(pParserInitInfo)
{
}

CAudioStream::~CAudioStream()
{
}

VO_U32 CAudioStream::Init(CSDPMedia * pSDPMedia)
{
	VO_U32 rc = CMediaStream::Init(pSDPMedia);
	if(rc != VO_ERR_PARSER_OK)
		return rc;

	if(!pSDPMedia->ParseAttribute_rtpmap_clockRate(&m_clockRate))
		return VO_ERR_PARSER_INVALID_ARG;

	char _encodingName[32];
	if(!pSDPMedia->ParseAttribute_rtpmap_encodingName(_encodingName, 32))
		return VO_ERR_PARSER_INVALID_ARG;
	if(_stricmp(_encodingName, "mpeg4-generic") == 0)
	{
		rc = NewStreamMpeg4Generic();
		if(rc != VO_ERR_PARSER_OK)
			return rc;

		m_pParseRTP = new CParseMpeg4GenericRTP(this);
		m_pParseRTCP = new CParseRTCP(this);
	}
	else if(_stricmp(_encodingName, "enc-mpeg4-generic") == 0)
	{
		rc = NewStreamEncMpeg4Generic();
		if(rc != VO_ERR_PARSER_OK)
			return rc;

		m_pParseRTP = new CParseEncMpeg4GenericRTP(this);
		m_pParseRTCP = new CParseRTCP(this);
	}
	else if(_stricmp(_encodingName, "MP4A-LATM") == 0)
	{
		rc = NewStreamMp4aLatm();
		if(rc != VO_ERR_PARSER_OK)
			return rc;

		m_pParseRTP = new CParseMp4aLatmRTP(this);
		m_pParseRTCP = new CParseRTCP(this);
	}
	else
		return VO_ERR_PARSER_NOT_IMPLEMENT;

	if(m_pParseRTP == NULL || m_pParseRTCP == NULL)
		return VO_ERR_PARSER_OUT_OF_MEMORY;
	rc = m_pParseRTP->Init();
	if(rc != VO_ERR_PARSER_OK)
		return rc;
	rc = m_pParseRTCP->Init();
	if(rc != VO_ERR_PARSER_OK)
		return rc;

	return VO_ERR_PARSER_OK;
}

VO_U32 CAudioStream::NewStreamMpeg4Generic()
{
	m_streamInfo.nStreamId = ParseStreamId();
	m_streamInfo.nMediaType = VO_PARSER_OT_AUDIO;

	char _fmtp_mode[16] = "generic";
	m_pSDPMedia->ParseAttribute_fmtp_mode(_fmtp_mode, 16);
	if(_stricmp(_fmtp_mode, "AAC-hbr") == 0)
	{
		m_streamInfo.nCodecType = VO_AUDIO_CodingAAC;
		m_streamInfo.nMaxFrameSize = 10*1024;
	}
	else if(_stricmp(_fmtp_mode, "AAC-lbr") == 0)
	{
		m_streamInfo.nCodecType = VO_AUDIO_CodingAAC;
		m_streamInfo.nMaxFrameSize = 64;
	}
	else if(_stricmp(_fmtp_mode, "CELP-vbr") == 0)
	{
		return VO_ERR_PARSER_NOT_IMPLEMENT;
	}
	else if(_stricmp(_fmtp_mode, "CELP-cbr") == 0)
	{
		return VO_ERR_PARSER_NOT_IMPLEMENT;
	}
	else if(_stricmp(_fmtp_mode, "generic") == 0)
	{
		return VO_ERR_PARSER_NOT_IMPLEMENT;
	}
	else
	{
		return VO_ERR_PARSER_NOT_IMPLEMENT;
	}

	char _fmtp_config[32];
	if(!m_pSDPMedia->ParseAttribute_fmtp_config(_fmtp_config, 32))
		return VO_ERR_PARSER_INVALID_ARG;
	unsigned char * _configData = NULL;
	int configSize = 0;
	if(!HexStringToBinary(_fmtp_config, &_configData, &configSize))
		return VO_ERR_PARSER_ERROR;
	m_streamInfo.pExtraData = _configData;
	m_streamInfo.nExtraSize = configSize;

	return VO_ERR_PARSER_OK;
}

VO_U32 CAudioStream::NewStreamEncMpeg4Generic()
{
	m_streamInfo.nStreamId = ParseStreamId();
	m_streamInfo.nMediaType = VO_PARSER_OT_AUDIO;

	char _fmtp_mode[16] = "";
	m_pSDPMedia->ParseAttribute_fmtp_mode(_fmtp_mode, 16);
	if(_stricmp(_fmtp_mode, "AAC-hbr") == 0)
	{
		m_streamInfo.nCodecType = VO_AUDIO_CodingAAC;
		m_streamInfo.nMaxFrameSize = 10*1024;
	}
	else if(_stricmp(_fmtp_mode, "AAC-lbr") == 0)
	{
		m_streamInfo.nCodecType = VO_AUDIO_CodingAAC;
		m_streamInfo.nMaxFrameSize = 64;
	}
	else if(_stricmp(_fmtp_mode, "CELP-vbr") == 0)
	{
		return VO_ERR_PARSER_NOT_IMPLEMENT;
	}
	else if(_stricmp(_fmtp_mode, "CELP-cbr") == 0)
	{
		return VO_ERR_PARSER_NOT_IMPLEMENT;
	}
	else
	{
		return VO_ERR_PARSER_NOT_IMPLEMENT;
	}

	char _fmtp_config[32];
	if(!m_pSDPMedia->ParseAttribute_fmtp_config(_fmtp_config, 32))
		return VO_ERR_PARSER_INVALID_ARG;
	unsigned char * _configData = NULL;
	int configSize = 0;
	if(!HexStringToBinary(_fmtp_config, &_configData, &configSize))
		return VO_ERR_PARSER_ERROR;
	m_streamInfo.pExtraData = _configData;
	m_streamInfo.nExtraSize = configSize;

	return VO_ERR_PARSER_OK;
}

VO_U32 CAudioStream::NewStreamMp4aLatm()
{
	m_streamInfo.nStreamId = ParseStreamId();
	m_streamInfo.nMediaType = VO_PARSER_OT_AUDIO;
	m_streamInfo.nCodecType = VO_AUDIO_CodingAAC;
	m_streamInfo.nMaxFrameSize = 1500;

	char _fmtp_config[32];
	if(!m_pSDPMedia->ParseAttribute_fmtp_config(_fmtp_config, 32))
		return VO_ERR_PARSER_INVALID_ARG;
	unsigned char * _configData = NULL;
	int configSize = 0;
	if(!ParseMp4aLatmConfig(_fmtp_config, &_configData, &configSize))
		return VO_ERR_PARSER_ERROR;
	m_streamInfo.pExtraData = _configData;
	m_streamInfo.nExtraSize = configSize;

	return VO_ERR_PARSER_OK;
}
