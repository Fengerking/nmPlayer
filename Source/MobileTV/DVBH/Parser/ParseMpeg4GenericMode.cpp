#include "commonheader.h"
#include "sdp_headers.h"
#include "RTPPacket.h"
#include "MediaStream.h"
#include "ParseMpeg4GenericRTP.h"
#include "ParseEncMpeg4GenericRTP.h"
#include "ParseMpeg4GenericMode.h"

#ifdef _RTPPSR_LOG
#include "logtool.h"
#endif //_RTPPSR_LOG

using namespace Mpeg4GenericMode;

CParseMode::CParseMode(CParseMpeg4GenericRTP * pParseMpeg4GenericRTP)
: m_pParseMpeg4GenericRTP(pParseMpeg4GenericRTP)
, m_startOfFragmentedAccessUnit(false)
, m_lossOfFragmentedAccessUnit(false)
, m_endOfFragmentedAccessUnit(false)
, m_maxAUHeadersLength(0)
, m_AUHeaders(NULL)
, m_AUHeadersLength(0)
, m_sizeLength(0)
, m_indexLength(0)
, m_indexDeltaLength(0)
, m_CTSDeltaLength(0)
, m_DTSDeltaLength(0)
, m_randomAccessIndication(0)
, m_streamStateIndication(0)
{
}

CParseMode::~CParseMode()
{
	SAFE_DELETE_ARRAY(m_AUHeaders);
}

VO_U32 CParseMode::Init()
{
	CMediaStream * pMediaStream = m_pParseMpeg4GenericRTP->GetMediaStream();
	CSDPMedia * pSDPMedia = pMediaStream->GetSDPMediaDescription();

	pSDPMedia->ParseAttribute_fmtp_sizeLength(&m_sizeLength);
	pSDPMedia->ParseAttribute_fmtp_indexLength(&m_indexLength);
	pSDPMedia->ParseAttribute_fmtp_indexDeltaLength(&m_indexDeltaLength);
	pSDPMedia->ParseAttribute_fmtp_CTSDeltaLength(&m_CTSDeltaLength);
	pSDPMedia->ParseAttribute_fmtp_DTSDeltaLength(&m_DTSDeltaLength);
	pSDPMedia->ParseAttribute_fmtp_randomAccessIndication(&m_randomAccessIndication);
	pSDPMedia->ParseAttribute_fmtp_streamStateIndication(&m_streamStateIndication);

	return VO_ERR_PARSER_OK;
}


CParseEncMode::CParseEncMode(CParseEncMpeg4GenericRTP * pParseEncMpeg4GenericRTP)
: CParseMode(pParseEncMpeg4GenericRTP)
, m_ISMACrypCryptoSuite(NULL)
, m_ISMACrypIVLength(4)
, m_ISMACrypDeltaIVLength(0)
, m_ISMACrypSelectiveEncryption(0)
, m_ISMACrypKeyIndicatorLength(0)
, m_ISMACrypKeyIndicatorPerAU(0)
, m_ISMACrypSalt(0)
, m_ISMACrypKey(NULL)
, m_ISMACrypKMSID(0)
, m_ISMACrypKMSVersion(0)
, m_ISMACrypKMSSpecificData(NULL)
{
}

CParseEncMode::~CParseEncMode()
{
	SAFE_DELETE_ARRAY(m_ISMACrypCryptoSuite);
	SAFE_DELETE_ARRAY(m_ISMACrypKey);
	SAFE_DELETE_ARRAY(m_ISMACrypKMSSpecificData);
}

VO_U32 CParseEncMode::Init()
{
	VO_U32 rc = CParseMode::Init();
	if(rc != VO_ERR_PARSER_OK)
		return rc;

	CMediaStream * pMediaStream = m_pParseMpeg4GenericRTP->GetMediaStream();
	CSDPMedia * pSDPMedia = pMediaStream->GetSDPMediaDescription();

	pSDPMedia->ParseAttribute_fmtp_ISMACrypIVLength(&m_ISMACrypIVLength);
	pSDPMedia->ParseAttribute_fmtp_ISMACrypDeltaIVLength(&m_ISMACrypDeltaIVLength);
	pSDPMedia->ParseAttribute_fmtp_ISMACrypSelectiveEncryption(&m_ISMACrypSelectiveEncryption);
	pSDPMedia->ParseAttribute_fmtp_ISMACrypKeyIndicatorLength(&m_ISMACrypKeyIndicatorLength);
	pSDPMedia->ParseAttribute_fmtp_ISMACrypKeyIndicatorPerAU(&m_ISMACrypKeyIndicatorPerAU);

	return VO_ERR_PARSER_OK;
}



// mode=generic implement
CParseGenericMode::CParseGenericMode(CParseMpeg4GenericRTP * pParseMpeg4GenericRTP)
: CParseMode(pParseMpeg4GenericRTP)
{
}

CParseGenericMode::~CParseGenericMode()
{
}

VO_U32 CParseGenericMode::Init()
{
	return VO_ERR_PARSER_OK;
}

VO_U32 CParseGenericMode::ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss)
{
	return VO_ERR_PARSER_OK;
}
