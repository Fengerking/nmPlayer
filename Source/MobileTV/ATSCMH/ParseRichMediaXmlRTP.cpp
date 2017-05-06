#include "commonheader.h"
#include "sdp_headers.h"
#include "MediaStream.h"
#include "ReorderRTPPacket.h"
#include "RTPPacket.h"
#include "SynchronizeStreams.h"
#include "ParseRichMediaXmlRTP.h"

CParseRichMediaXmlRTP::CParseRichMediaXmlRTP(CMediaStream * pMediaStream)
: CParseRTP(pMediaStream)
, m_VersionProfile(0)
, m_Level(0)
{
}

CParseRichMediaXmlRTP::~CParseRichMediaXmlRTP()
{
}

VO_U32 CParseRichMediaXmlRTP::Init()
{
	VO_U32 rc = CParseRTP::Init();
	if(rc != VO_ERR_PARSER_OK)
		return rc;

	CSDPMedia * pSDPMedia = m_pMediaStream->GetSDPMediaDescription();

	pSDPMedia->ParseAttribute_fmtp_Version_profile(&m_VersionProfile);
	pSDPMedia->ParseAttribute_fmtp_Level(&m_Level);

	return VO_ERR_PARSER_OK;
}

VO_U32 CParseRichMediaXmlRTP::ParseRTPPayload()
{
	VO_U32 rc = VO_ERR_PARSER_OK;

	bool packetLoss = true;
	CRTPPacket * pRTPPacket = m_pReorderRTPPacket->GetNextReorderedRTPPacket(packetLoss);
	if(pRTPPacket == NULL)
		return VO_ERR_PARSER_OUT_OF_MEMORY;

	OnFrameStart(pRTPPacket->Timestamp());
	OnFrameData(pRTPPacket->RTPPacketData(), pRTPPacket->RTPPacketSize());
	OnFrameEnd();

	m_pReorderRTPPacket->ReleaseRTPPacket(pRTPPacket);

	return rc;
}
