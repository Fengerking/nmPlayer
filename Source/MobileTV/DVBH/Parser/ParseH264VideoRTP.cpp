#include "commonheader.h"
#include "netbase.h"
#include "sdp_headers.h"
#include "parseutil.h"
#include "MediaStream.h"
#include "ReorderRTPPacket.h"
#include "RTPPacket.h"
#include "SynchronizeStreams.h"
#include "ParseH264VideoRTP.h"

using namespace H264VideoRTPPacketizationMode;

CParseH264VideoRTP::CParseH264VideoRTP(CMediaStream * pMediaStream)
: CParseRTP(pMediaStream)
, m_pParsePacketizationMode(NULL)
{
}

CParseH264VideoRTP::~CParseH264VideoRTP()
{
	SAFE_DELETE(m_pParsePacketizationMode);
}

VO_U32 CParseH264VideoRTP:: Init()
{
	VO_U32 rc = CParseRTP::Init();
	if(rc != VO_ERR_PARSER_OK)
		return rc;

	CSDPMedia * pSDPMedia = m_pMediaStream->GetSDPMediaDescription();

	int packetization_mode = 1;
	pSDPMedia->ParseAttribute_fmtp_packetization_mode(&packetization_mode);
	if(packetization_mode == 0)
	{
		m_pParsePacketizationMode = new CParseSingleNALUnitMode(this);
	}
	else if(packetization_mode == 1)
	{
		m_pParsePacketizationMode = new CParseNonInterleavedMode(this);
	}
	else if(packetization_mode == 2)
	{
		m_pParsePacketizationMode = new CParseInterleavedMode(this);
	}
	else
	{
		m_pParsePacketizationMode = NULL;
	}

	if(m_pParsePacketizationMode == NULL)
		return VO_ERR_PARSER_NOT_IMPLEMENT;
	rc = m_pParsePacketizationMode->Init();
	if(rc != VO_ERR_PARSER_OK)
		return rc;

	return VO_ERR_PARSER_OK;
}

VO_U32 CParseH264VideoRTP::ParseRTPPayload()
{
	VO_U32 rc = VO_ERR_PARSER_ERROR;

	bool packetLoss = true;
	CRTPPacket * pRTPPacket = m_pReorderRTPPacket->GetNextReorderedRTPPacket(packetLoss);
	if(pRTPPacket == NULL)
		return VO_ERR_PARSER_OUT_OF_MEMORY;

	rc = m_pParsePacketizationMode->ParseRTPPayload(pRTPPacket, packetLoss);

	m_pReorderRTPPacket->ReleaseRTPPacket(pRTPPacket);

	return rc;
}

void CParseH264VideoRTP::OnFrameStart(unsigned int rtpTimestamp)
{
	CParseRTP::OnFrameStart(rtpTimestamp);
	OnFrameData((void *)_NALUnitStartCode, 4);
}





// class CParsePacketizationMode implementation
CParsePacketizationMode::CParsePacketizationMode(CParseH264VideoRTP * pParseH264VideoRTP)
: m_pParseH264VideoRTP(pParseH264VideoRTP)
, m_startOfFragmentedNALUnit(false)
, m_lossOfFragmentedNALUnit(true)
, m_endOfFragmentedNALUnit(false)
{
}

CParsePacketizationMode::~CParsePacketizationMode()
{
}

VO_U32 CParsePacketizationMode::Init()
{
	return VO_ERR_PARSER_OK;
}


// class CParseSingleNALUnitMode implementation
CParseSingleNALUnitMode::CParseSingleNALUnitMode(CParseH264VideoRTP * pParseH264VideoRTP)
: CParsePacketizationMode(pParseH264VideoRTP)
{
}

CParseSingleNALUnitMode::~CParseSingleNALUnitMode()
{
}

VO_U32 CParseSingleNALUnitMode::ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss)
{
	unsigned char rtpPayloadHeader = 0;
	pRTPPacket->Peek(&rtpPayloadHeader, 1);

	unsigned char NALUnitType = rtpPayloadHeader & 0x1F;
	switch(NALUnitType) 
	{
	case 1: 
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
		{ // Single NAL Unit Mode
			void * _frameData = pRTPPacket->ReadPointer();
			int frameSize = pRTPPacket->AvailDataSize();
			m_pParseH264VideoRTP->OnFrameStart(pRTPPacket->Timestamp());
			m_pParseH264VideoRTP->OnFrameData(_frameData, frameSize);
			m_pParseH264VideoRTP->OnFrameEnd();
			break; 
		}
	default:
		{
			return VO_ERR_PARSER_ERROR;
		}
	}

	return VO_ERR_PARSER_OK;
}



// class CParseNonInterleavedMode implementation
CParseNonInterleavedMode::CParseNonInterleavedMode(CParseH264VideoRTP * pParseH264VideoRTP)
: CParsePacketizationMode(pParseH264VideoRTP)
{
}

CParseNonInterleavedMode::~CParseNonInterleavedMode()
{
}

VO_U32 CParseNonInterleavedMode::Init()
{
	return VO_ERR_PARSER_OK;
}

VO_U32 CParseNonInterleavedMode::ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss)
{
	unsigned char rtpPayloadHeader = 0;
	pRTPPacket->Peek(&rtpPayloadHeader, 1);

	unsigned char NALUnitType = rtpPayloadHeader & 0x1F;
	switch(NALUnitType) 
	{
	case 1: 
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
		{ // Single NAL Unit Mode
			void * _frameData = pRTPPacket->ReadPointer();
			int frameSize = pRTPPacket->AvailDataSize();
			m_pParseH264VideoRTP->OnFrameStart(pRTPPacket->Timestamp());
			m_pParseH264VideoRTP->OnFrameData(_frameData, frameSize);
			m_pParseH264VideoRTP->OnFrameEnd();
			break; 
		}
	case 24:
		{ // STAP-A
			pRTPPacket->Skip(1); // skip rtp payload header
			while(pRTPPacket->AvailDataSize() > 0)
			{
				unsigned short NALUSize = 0;
				pRTPPacket->Read(&NALUSize, 2);
				NALUSize = ntohs(NALUSize);
				if(pRTPPacket->AvailDataSize() < NALUSize)
					break;

				unsigned char NALUHeader = 0;
				pRTPPacket->Peek(&NALUHeader, 1);

				void * _frameData = pRTPPacket->ReadPointer();
				int frameSize = NALUSize;
				m_pParseH264VideoRTP->OnFrameStart(pRTPPacket->Timestamp());
				m_pParseH264VideoRTP->OnFrameData(_frameData, frameSize);
				m_pParseH264VideoRTP->OnFrameEnd();

				pRTPPacket->Skip(frameSize);
			}
			break;
		}
	case 28:
		{ // FU-A
			pRTPPacket->Skip(1); // skip rtp payload header
			unsigned char FUHeader = 0;
			pRTPPacket->Peek(&FUHeader, 1);
			m_startOfFragmentedNALUnit = (FUHeader & 0x80) != 0;
			m_endOfFragmentedNALUnit = (FUHeader & 0x40) != 0;
			if(m_startOfFragmentedNALUnit)
			{
				FUHeader = (rtpPayloadHeader & 0xE0) | (FUHeader  & 0x1F);
				pRTPPacket->Write((void *)&FUHeader, 1);
			}
			else
			{
				pRTPPacket->Skip(1);
			}

			if(m_startOfFragmentedNALUnit)
			{
				if(packetLoss || m_lossOfFragmentedNALUnit)
				{
					m_pParseH264VideoRTP->OnFrameError(0);
				}
				m_lossOfFragmentedNALUnit = false;

				m_pParseH264VideoRTP->OnFrameStart(pRTPPacket->Timestamp());
			}
			else if(packetLoss)
			{
				m_lossOfFragmentedNALUnit = true;
			}

		    if(m_lossOfFragmentedNALUnit)
			{
				m_pParseH264VideoRTP->OnFrameError(0);
				return VO_ERR_PARSER_FAIL;
			}

			void * _frameData = pRTPPacket->ReadPointer();
			int frameSize = pRTPPacket->AvailDataSize();
			m_pParseH264VideoRTP->OnFrameData(_frameData, frameSize);
			if(m_endOfFragmentedNALUnit)
			{
				m_pParseH264VideoRTP->OnFrameEnd();
			}

			break;
		}
	default:
		{
			return VO_ERR_PARSER_ERROR;
		}
	}

	return VO_ERR_PARSER_OK;
}



// class CParseInterleavedMode implementation
CParseInterleavedMode::CParseInterleavedMode(CParseH264VideoRTP * pParseH264VideoRTP)
: CParsePacketizationMode(pParseH264VideoRTP)
{
}
	
CParseInterleavedMode::~CParseInterleavedMode()
{
}

VO_U32 CParseInterleavedMode::Init()
{
	return VO_ERR_PARSER_OK;
}

VO_U32 CParseInterleavedMode::ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss)
{
	return VO_ERR_PARSER_ERROR;

	unsigned char rtpPayloadHeader = 0;
	pRTPPacket->Peek(&rtpPayloadHeader, 1);

	unsigned char NALUnitType = rtpPayloadHeader & 0x1F;
	switch(NALUnitType) 
	{
	case 25:
		{ // STAP-B
			break;
		}

	case 26:
		{ // MTAP16
			break;
		}

	case 27:
		{ // MTAP24
			break;
		}

	case 28:
		{ // FU-A
			break;
		}

	case 29:
		{ // FU-B
			break;
		}

	default:
		{
			return VO_ERR_PARSER_ERROR;
		}
	}

	return VO_ERR_PARSER_OK;
}
