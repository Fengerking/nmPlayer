#include "commonheader.h"
#include "netbase.h"
#include "BitVector.h"
#include "RTPPacket.h"
#include "ParseEncMpeg4GenericRTP.h"
#include "ParseEncMpeg4GenericAudioMode.h"

using namespace Mpeg4GenericMode;

// mode=CELP-cbr implement
CParseEncCELPcbrMode::CParseEncCELPcbrMode(CParseEncMpeg4GenericRTP * pParseEncMpeg4GenericRTP)
: CParseEncMode(pParseEncMpeg4GenericRTP)
{
}

CParseEncCELPcbrMode::~CParseEncCELPcbrMode()
{
}

VO_U32 CParseEncCELPcbrMode::Init()
{
	return VO_ERR_PARSER_ERROR;
}

VO_U32 CParseEncCELPcbrMode::ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss)
{
	return VO_ERR_PARSER_ERROR;
}



// mode=CELP-vbr implement
CParseEncCELPvbrMode::CParseEncCELPvbrMode(CParseEncMpeg4GenericRTP * pParseEncMpeg4GenericRTP)
: CParseEncMode(pParseEncMpeg4GenericRTP)
{
}

CParseEncCELPvbrMode::~CParseEncCELPvbrMode()
{
}

VO_U32 CParseEncCELPvbrMode::Init()
{
	return VO_ERR_PARSER_ERROR;
}

VO_U32 CParseEncCELPvbrMode::ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss)
{
	return VO_ERR_PARSER_ERROR;
}



// mode=AAC-lbr implement
CParseEncAAClbrMode::CParseEncAAClbrMode(CParseEncMpeg4GenericRTP * pParseEncMpeg4GenericRTP)
: CParseEncMode(pParseEncMpeg4GenericRTP)
{
}

CParseEncAAClbrMode::~CParseEncAAClbrMode()
{
}

VO_U32 CParseEncAAClbrMode::Init()
{
	return VO_ERR_PARSER_ERROR;
}

VO_U32 CParseEncAAClbrMode::ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss)
{
	return VO_ERR_PARSER_ERROR;
}



// mode=AAC-hbr implement
CParseEncAAChbrMode::CParseEncAAChbrMode(CParseEncMpeg4GenericRTP * pParseEncMpeg4GenericRTP)
: CParseEncMode(pParseEncMpeg4GenericRTP)
{
}

CParseEncAAChbrMode::~CParseEncAAChbrMode()
{
}

VO_U32 CParseEncAAChbrMode::Init()
{
	VO_U32 rc = CParseEncMode::Init();
	if(rc != VO_ERR_PARSER_OK)
		return rc;

	m_maxAUHeadersLength = 32;
	m_AUHeaders = new unsigned char[m_maxAUHeadersLength];
	if(m_AUHeaders == NULL)
		return VO_ERR_PARSER_OUT_OF_MEMORY;

	return VO_ERR_PARSER_OK;
}

VO_U32 CParseEncAAChbrMode::ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss)
{
	do
	{
		if(m_sizeLength == 0)
			break;

		unsigned short AUHeadersLength_bits = 0; 
		pRTPPacket->Read(&AUHeadersLength_bits, 2);
		AUHeadersLength_bits = ntohs(AUHeadersLength_bits);
		unsigned short AUHeadersLength_bytes = (AUHeadersLength_bits + 7) / 8;
/*
	Handle ISMACryp header

*/
		if(m_maxAUHeadersLength < AUHeadersLength_bytes)
		{
			SAFE_DELETE_ARRAY(m_AUHeaders);
			m_maxAUHeadersLength = AUHeadersLength_bytes * 2;
			m_AUHeaders = new unsigned char[m_maxAUHeadersLength];
			if(m_AUHeaders == NULL)
				break;
		}
		m_AUHeadersLength = AUHeadersLength_bytes;
		if((unsigned int)pRTPPacket->AvailDataSize() < m_AUHeadersLength)
			break;
		pRTPPacket->Read(m_AUHeaders, m_AUHeadersLength);

		unsigned int AU_size = 0;
		unsigned int AU_Index = 0;
		unsigned int AU_Index_delta = 0;
		CBitVector bitVector(m_AUHeaders, 0, AUHeadersLength_bits);

		m_endOfFragmentedAccessUnit = pRTPPacket->Marker();

		void * _frameData = NULL;
		int frameSize = 0;

		// parse the first Access Unit
		AU_size = bitVector.GetBits(m_sizeLength);
		AU_Index = bitVector.GetBits(m_indexLength);
		if((unsigned int)pRTPPacket->AvailDataSize() < AU_size)
			break;
		_frameData = pRTPPacket->ReadPointer();
		frameSize = AU_size;
		m_pParseMpeg4GenericRTP->OnFrameStart(pRTPPacket->Timestamp());
		m_pParseMpeg4GenericRTP->OnFrameData(_frameData, frameSize);
		if(m_endOfFragmentedAccessUnit)
		{
			m_pParseMpeg4GenericRTP->OnFrameEnd();
		}
		pRTPPacket->Skip(frameSize);

		// parse non-first Access Unit
		while(pRTPPacket->AvailDataSize() > 0)
		{
			AU_size = bitVector.GetBits(m_sizeLength);
			AU_Index_delta = bitVector.GetBits(m_indexDeltaLength);
			if((unsigned int)pRTPPacket->AvailDataSize() < AU_size)
				return VO_ERR_PARSER_ERROR;
			_frameData = pRTPPacket->ReadPointer();
			frameSize = AU_size;
			m_pParseMpeg4GenericRTP->OnFrameStart(pRTPPacket->Timestamp());
			m_pParseMpeg4GenericRTP->OnFrameData(_frameData, frameSize);
			if(m_endOfFragmentedAccessUnit)
			{
				m_pParseMpeg4GenericRTP->OnFrameEnd();
			}

			pRTPPacket->Skip(frameSize);
		}

		return VO_ERR_PARSER_OK;

	}while(0);

	return VO_ERR_PARSER_ERROR;
}
