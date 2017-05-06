#include "VideoStream.h"
#include "commonheader.h"
#include "sdp_headers.h"
#include "parseutil.h"
#include "ParseRTP.h"
#include "ParseRTCP.h"
#include "ParseH264VideoRTP.h"
#include "ParseEncMpeg4GenericRTP.h"
#include "ParseRichMediaXmlRTP.h"
#include "mp4cfg.h"

CVideoStream::CVideoStream(VO_PARSER_INIT_INFO * pParserInitInfo)
: CMediaStream(pParserInitInfo)
{
}

CVideoStream::~CVideoStream()
{
}

VO_U32 CVideoStream::Init(CSDPMedia * pSDPMedia)
{
	VO_U32 rc = CMediaStream::Init(pSDPMedia);
	if(rc != VO_ERR_PARSER_OK)
		return rc;

	if(!pSDPMedia->ParseAttribute_rtpmap_clockRate(&m_clockRate))
		return VO_ERR_PARSER_INVALID_ARG;

	char _encodingName[32];
	if(!pSDPMedia->ParseAttribute_rtpmap_encodingName(_encodingName, 32))
		return VO_ERR_PARSER_INVALID_ARG;
	if(_stricmp(_encodingName, "H264") == 0)
	{
		rc = NewStreamH264();
		if(rc != VO_ERR_PARSER_OK)
			return rc;

		m_pParseRTP = new CParseH264VideoRTP(this);
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
	else if(_stricmp(_encodingName, "richmedia+xml") == 0)
	{
		rc = NewStreamRichMediaXml();
		if(rc != VO_ERR_PARSER_OK)
			return rc;

		m_pParseRTP = new CParseRichMediaXmlRTP(this);
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

VO_U32 CVideoStream::NewStreamH264()
{
	m_streamInfo.nStreamId = ParseStreamId();
	m_streamInfo.nMediaType = VO_PARSER_OT_VIDEO;
	m_streamInfo.nCodecType = VO_VIDEO_CodingH264;
	m_streamInfo.nMaxFrameSize = 25*1024;

	char _fmtp_spropParameterSets[256];
	if(m_pSDPMedia->ParseAttribute_fmtp_sprop_parameter_sets(_fmtp_spropParameterSets, 256))
	{
		unsigned char * _paramSets = NULL;
		int paramSetsLength = 0;
		if(ParseSpropParameterSets(_fmtp_spropParameterSets, &_paramSets, &paramSetsLength))
		{
			m_streamInfo.pExtraData = _paramSets;
			m_streamInfo.nExtraSize = paramSetsLength;
		}
	}

	return VO_ERR_PARSER_OK;
}

VO_U32 CVideoStream::NewStreamEncMpeg4Generic()
{
	m_streamInfo.nStreamId = ParseStreamId();
	m_streamInfo.nMediaType = VO_PARSER_OT_VIDEO;
	m_streamInfo.nCodecType = VO_VIDEO_CodingH264;
	m_streamInfo.nMaxFrameSize = 25*1024;

	char _fmtp_config[256];
	if(!m_pSDPMedia->ParseAttribute_fmtp_config(_fmtp_config, 256))
		return VO_ERR_PARSER_INVALID_ARG;
	unsigned char * _configData = NULL;
	int configSize = 0;
	if(!HexStringToBinary(_fmtp_config, &_configData, &configSize))
		return VO_ERR_PARSER_ERROR;

	struct MP4::AVCDecoderConfigurationRecord AVCConfig;
	bool ret = AVCConfig.Load(_configData, configSize);
	SAFE_DELETE_ARRAY(_configData);
	if(!ret)
		return VO_ERR_PARSER_ERROR;

	m_streamInfo.pExtraData = new unsigned char[256];
	if(m_streamInfo.pExtraData == NULL)
		return VO_ERR_PARSER_OUT_OF_MEMORY;
	m_streamInfo.nExtraSize = AVCConfig.ToH264Header(m_streamInfo.pExtraData, 256);

	return VO_ERR_PARSER_OK;
}

VO_U32 CVideoStream::NewStreamRichMediaXml()
{
	//m_streamInfo.nStreamId = ParseStreamId();
	//m_streamInfo.nMediaType = VO_PARSER_OT_VIDEO;
	//m_streamInfo.nCodecType = VC_RICHMEDIAXML;
	//m_streamInfo.nMaxFrameSize = 1500;//MTU_SIZE

	return VO_ERR_PARSER_OK;
}
