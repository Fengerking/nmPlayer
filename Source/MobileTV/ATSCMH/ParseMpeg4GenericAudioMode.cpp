#include "commonheader.h"
#include "netbase.h"
#include "BitVector.h"
#include "RTPPacket.h"
#include "ParseMpeg4GenericRTP.h"
#include "ParseMpeg4GenericAudioMode.h"

using namespace Mpeg4GenericMode;

// mode=CELP-cbr implement
CParseCELPcbrMode::CParseCELPcbrMode(CParseMpeg4GenericRTP * pParseMpeg4GenericRTP)
: CParseMode(pParseMpeg4GenericRTP)
{
}

CParseCELPcbrMode::~CParseCELPcbrMode()
{
}

VO_U32 CParseCELPcbrMode::Init()
{
	return VO_ERR_PARSER_OK;
}

VO_U32 CParseCELPcbrMode::ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss)
{
	return VO_ERR_PARSER_OK;
}



// mode=CELP-vbr implement
CParseCELPvbrMode::CParseCELPvbrMode(CParseMpeg4GenericRTP * pParseMpeg4GenericRTP)
: CParseMode(pParseMpeg4GenericRTP)
{
}

CParseCELPvbrMode::~CParseCELPvbrMode()
{
}

VO_U32 CParseCELPvbrMode::Init()
{
	return VO_ERR_PARSER_OK;
}

VO_U32 CParseCELPvbrMode::ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss)
{
	return VO_ERR_PARSER_OK;
}



// mode=AAC-lbr implement
CParseAAClbrMode::CParseAAClbrMode(CParseMpeg4GenericRTP * pParseMpeg4GenericRTP)
: CParseMode(pParseMpeg4GenericRTP)
{
}

CParseAAClbrMode::~CParseAAClbrMode()
{
}

VO_U32 CParseAAClbrMode::Init()
{
	return VO_ERR_PARSER_OK;
}

VO_U32 CParseAAClbrMode::ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss)
{
	return VO_ERR_PARSER_OK;
}



// mode=AAC-hbr implement
CParseAAChbrMode::CParseAAChbrMode(CParseMpeg4GenericRTP * pParseMpeg4GenericRTP)
: CParseMode(pParseMpeg4GenericRTP)
{
}

CParseAAChbrMode::~CParseAAChbrMode()
{
}

VO_U32 CParseAAChbrMode::Init()
{
	VO_U32 rc = CParseMode::Init();
	if(rc != VO_ERR_PARSER_OK)
		return rc;

	m_maxAUHeadersLength = 32;
	m_AUHeaders = new unsigned char[m_maxAUHeadersLength];
	if(m_AUHeaders == NULL)
		return VO_ERR_PARSER_OUT_OF_MEMORY;

	return VO_ERR_PARSER_OK;
}

VO_U32 CParseAAChbrMode::ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss)
{
	do
	{
		if(m_sizeLength == 0)
			break;

		unsigned short AUHeadersLength_bits = 0; 
		pRTPPacket->Read(&AUHeadersLength_bits, 2);
		AUHeadersLength_bits = ntohs(AUHeadersLength_bits);
		unsigned short AUHeadersLength_bytes = (AUHeadersLength_bits + 7) / 8;
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
				return VO_ERR_PARSER_FAIL;
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
