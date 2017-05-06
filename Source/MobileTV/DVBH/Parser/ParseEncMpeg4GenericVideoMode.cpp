#include "commonheader.h"
#include "netbase.h"
#include "BitVector.h"
#include "parseutil.h"
#include "RTPPacket.h"
#include "ParseEncMpeg4GenericRTP.h"
#include "ParseEncMpeg4GenericVideoMode.h"

using namespace Mpeg4GenericMode;

// mode=mpeg4-video implement
CParseMpeg4VideoMode::CParseMpeg4VideoMode(CParseEncMpeg4GenericRTP * pParseEncMpeg4GenericRTP)
: CParseEncMode(pParseEncMpeg4GenericRTP)
{
}

CParseMpeg4VideoMode::~CParseMpeg4VideoMode()
{
}

VO_U32 CParseMpeg4VideoMode::Init()
{
	VO_U32 rc = CParseEncMode::Init();
	if(rc != VO_ERR_PARSER_OK)
		return rc;

	return VO_ERR_PARSER_OK;
}

VO_U32 CParseMpeg4VideoMode::ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss)
{
	return VO_ERR_PARSER_ERROR;
}



// mode=avc-video implement
CParseAVCVideoMode::CParseAVCVideoMode(CParseEncMpeg4GenericRTP * pParseEncMpeg4GenericRTP)
: CParseEncMode(pParseEncMpeg4GenericRTP)
{
}

CParseAVCVideoMode::~CParseAVCVideoMode()
{
}

VO_U32 CParseAVCVideoMode::Init()
{
	VO_U32 rc = CParseEncMode::Init();
	if(rc != VO_ERR_PARSER_OK)
		return rc;

	m_maxAUHeadersLength = 4;
	m_AUHeaders = new unsigned char[m_maxAUHeadersLength];
	if(m_AUHeaders == NULL)
		return VO_ERR_PARSER_OUT_OF_MEMORY;

	return VO_ERR_PARSER_OK;
}

VO_U32 CParseAVCVideoMode::ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss)
{
	do
	{
		unsigned short AUHeadersLength_bits = 0; 
		pRTPPacket->Read(&AUHeadersLength_bits, 2);
		AUHeadersLength_bits = ntohs(AUHeadersLength_bits);
/*
		unsigned char AU_is_encrypted = 0;
		if(m_ISMACrypSelectiveEncryption == 1)
		{
			pRTPPacket->Read(&AU_is_encrypted, 1);
		}
		else
		{
			AU_is_encrypted = 1;
		}

		pRTPPacket->Skip(m_ISMACrypIVLength); // skip initial_IV
		pRTPPacket->Skip(m_ISMACrypKeyIndicatorLength); // skip key_indicator
		if(AU_is_encrypted == 1)
		{
			//decrypt access unit
		}
*/
		unsigned short AUHeadersLength_bytes = (AUHeadersLength_bits + 7) / 8;
		m_AUHeadersLength = AUHeadersLength_bytes;
		if((unsigned int)pRTPPacket->AvailDataSize() < m_AUHeadersLength)
			break;
		pRTPPacket->Read(m_AUHeaders, m_AUHeadersLength);

		unsigned int DTS_flag = 0;
		unsigned int DTS_delta = 0;
		unsigned int RAP_flag = 0;
		CBitVector bitVector(m_AUHeaders, 0, AUHeadersLength_bits);
		DTS_flag = bitVector.Get1Bit();
		if(DTS_flag)
		{
			DTS_delta = bitVector.GetBits(m_DTSDeltaLength);
		}
		RAP_flag = bitVector.Get1Bit();

		unsigned char _AUStartCode[4];
		pRTPPacket->Peek(_AUStartCode, 4);
		if(memcmp(_AUStartCode, _NALUnitStartCode, BytesOfStartCode) == 0)
		{
			m_startOfFragmentedAccessUnit = true;
		}
		m_endOfFragmentedAccessUnit = pRTPPacket->Marker();

		if(m_startOfFragmentedAccessUnit)
		{
			if(packetLoss || m_lossOfFragmentedAccessUnit)
			{
				m_pParseMpeg4GenericRTP->OnFrameError(0);
			}
			m_lossOfFragmentedAccessUnit = false;

			m_pParseMpeg4GenericRTP->OnFrameStart(pRTPPacket->Timestamp());
		}
		else if(packetLoss)
		{
			m_lossOfFragmentedAccessUnit = true;
		}

	    if(m_lossOfFragmentedAccessUnit)
		{
			m_pParseMpeg4GenericRTP->OnFrameError(0);
			return VO_ERR_PARSER_ERROR; //VORC_STREAMPARSE_PACKET_LOSS;
		}

		void * _frameData = pRTPPacket->ReadPointer();
		int frameSize = pRTPPacket->AvailDataSize();
		m_pParseMpeg4GenericRTP->OnFrameData(_frameData, frameSize);
		if(m_endOfFragmentedAccessUnit)
		{
			m_pParseMpeg4GenericRTP->OnFrameEnd();
		}

		return VO_ERR_PARSER_OK;

	}while(0);

	return VO_ERR_PARSER_ERROR;
}
