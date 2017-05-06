#include "commonheader.h"
#include "sdp_headers.h"
#include "MediaStream.h"
#include "ReorderRTPPacket.h"
#include "RTPPacket.h"
#include "SynchronizeStreams.h"
#include "ParseMp4aLatmRTP.h"

CParseMp4aLatmRTP::CParseMp4aLatmRTP(CMediaStream * pMediaStream)
: CParseRTP(pMediaStream)
{
}

CParseMp4aLatmRTP::~CParseMp4aLatmRTP()
{
}

VO_U32 CParseMp4aLatmRTP::Init()
{
	VO_U32 rc = CParseRTP::Init();
	if(rc != VO_ERR_PARSER_OK)
		return rc;

	return VO_ERR_PARSER_OK;
}

VO_U32 CParseMp4aLatmRTP::ParseRTPPayload()
{
	VO_U32 rc = VO_ERR_PARSER_ERROR;

	bool packetLoss = true;
	CRTPPacket * pRTPPacket = m_pReorderRTPPacket->GetNextReorderedRTPPacket(packetLoss);
	if(pRTPPacket == NULL)
		return VO_ERR_PARSER_OUT_OF_MEMORY;

	while(pRTPPacket->AvailDataSize() > 0)
	{
		int frameSize = 0;
		int latmDataLength = 0;
		do
		{
			latmDataLength = 0;
			pRTPPacket->Read(&latmDataLength, 1);
			frameSize += latmDataLength;
		}while(latmDataLength == 0xFF);

		if(pRTPPacket->AvailDataSize() < frameSize)
			break;

		void * _frameData = pRTPPacket->ReadPointer();
		OnFrameStart(pRTPPacket->Timestamp());
		OnFrameData(_frameData, frameSize);
		OnFrameEnd();

		pRTPPacket->Skip(frameSize);
	}

	m_pReorderRTPPacket->ReleaseRTPPacket(pRTPPacket);

	return rc;
}
