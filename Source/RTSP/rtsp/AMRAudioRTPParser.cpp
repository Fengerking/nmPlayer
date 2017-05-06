#include "utility.h"
#include <memory.h>
#include <string.h>

#include "MediaStream.h"
#include "MediaStreamSocket.h"
#include "RTPPacket.h"
#include "AMRAudioRTPParser.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

// RFC 3267
#define		FT_SPEECH_LOST		14
#define		FT_NO_DATA			15
#define     FT_INVALID			65535

const unsigned short FT2FrameSize[16] = 
{
	12, 13, 15, 17,
	19, 20, 26, 31,
	5, FT_INVALID, FT_INVALID, FT_INVALID,
	FT_INVALID, FT_INVALID, FT_INVALID, 0
};

const unsigned short FT2FrameSizeWB[16] = 
{
	17, 23, 32, 36,
	40, 46, 50, 58,
	60, 5, FT_INVALID, FT_INVALID,
	FT_INVALID, FT_INVALID, 0, 0
};

CAMRAudioRTPParser::CAMRAudioRTPParser(CMediaStream * mediaStream, 
									   CMediaStreamSocket * rtpStreamSock,
									   bool         isAMRWideband,
									   unsigned int audioChannels,
									   unsigned int fmtpOctetAlign,
									   unsigned int fmtpInterleaving,
					                   unsigned int fmtpRobustSorting,
					                   unsigned int fmtpCRC)
: CRTPParser(mediaStream, rtpStreamSock)
, m_isAMRWideband(isAMRWideband)
, m_audioChannels(audioChannels)
, m_fmtpOctetAlign(fmtpOctetAlign)
, m_fmtpInterleaving(fmtpInterleaving)
, m_fmtpRobustSorting(fmtpRobustSorting)
, m_fmtpCRC(fmtpCRC)
{
	m_CMR = 0;
	m_ILL = 0;
	m_ILP = 0;
    m_TocEntry = NULL;
	m_TocEntrySize = 0;
	m_TocEntryIndex = 0;
}

CAMRAudioRTPParser::~CAMRAudioRTPParser()
{
	if(m_TocEntry != NULL)
	{
		SAFE_DELETE_ARRAY(m_TocEntry);
	}
}

bool CAMRAudioRTPParser::ParseRTPPayloadHeader(CRTPPacket * rtpPacket)
{
	if(m_fmtpRobustSorting > 0)
		return false;

	if(m_fmtpInterleaving > 0)
		return false;

	if(m_audioChannels > 1)
		return false;

	if(m_fmtpOctetAlign == 0)
	{
		return ParseBandwidthEfficientModePayload(rtpPacket);
	}
	else
	{
		return ParseOctetAlignedModePayload(rtpPacket);
	}
}

bool CAMRAudioRTPParser::ParseRTPPayloadFrame(CRTPPacket * rtpPacket) 
{
	do
	{
		if(rtpPacket->RTPDataSize() == 0)
			break;

		if(m_TocEntryIndex >= m_TocEntrySize) 
			break;

		unsigned char TocEntry = m_TocEntry[m_TocEntryIndex];
		unsigned char FT = (TocEntry & 0x78) >> 3;

		unsigned short payloadFrameSize = m_isAMRWideband ? FT2FrameSizeWB[FT] : FT2FrameSize[FT];
		++m_TocEntryIndex;

		if(payloadFrameSize == FT_INVALID)
		{
			continue; //
		}

		if(rtpPacket->RTPDataSize() < payloadFrameSize)
			break;

		memcpy(m_frameData + m_frameSize, &TocEntry, 1); // copy the amr payload header
		m_frameSize += 1;
		memcpy(m_frameData + m_frameSize, rtpPacket->RTPData(), payloadFrameSize);
		m_frameSize += payloadFrameSize;
		rtpPacket->Skip(payloadFrameSize);

		return true;

	}while(1);

	return false;
}

bool CAMRAudioRTPParser::ParseBandwidthEfficientModePayload(CRTPPacket * rtpPacket)
{
	return false;
}

bool CAMRAudioRTPParser::ParseOctetAlignedModePayload(CRTPPacket * rtpPacket)
{
	do
	{
		int frameHeaderSize = 0;
		if(rtpPacket->RTPDataSize() < frameHeaderSize + 1)
			break;
		m_CMR = rtpPacket->RTPData()[frameHeaderSize++] >> 4;

		if(m_fmtpInterleaving > 0)
		{
			if(rtpPacket->RTPDataSize() < frameHeaderSize + 1)
				break;

			m_ILL = (rtpPacket->RTPData()[frameHeaderSize] & 0xF0) >> 4;
			m_ILP =  rtpPacket->RTPData()[frameHeaderSize] & 0x0F;

			if(m_ILL < m_ILP) 
				break;

			++frameHeaderSize;
		}

		unsigned int firstTocEntryIndex = frameHeaderSize;
		unsigned int TocEntryCount = 0;
		unsigned int frameCRCSize = 0;
		bool lastTocEntry = true;
		do
		{
			if(rtpPacket->RTPDataSize() < frameHeaderSize + 1)
				return false;

			unsigned char TocEntry = rtpPacket->RTPData()[frameHeaderSize++];
			lastTocEntry = (TocEntry & 0x80) == 0;
			unsigned char FT = (TocEntry & 0x78) >> 3;

			if(FT != FT_SPEECH_LOST && FT != FT_NO_DATA) 
				++frameCRCSize;

			++TocEntryCount;

		}while(!lastTocEntry);

		if(TocEntryCount > m_TocEntrySize) 
		{
			SAFE_DELETE_ARRAY(m_TocEntry);
			m_TocEntry = new unsigned char[TocEntryCount];TRACE_NEW("CAMRAudioRTPParser::ParseOctetAlignedModePayload m_TocEntry",m_TocEntry);
		}

		m_TocEntryIndex = 0; //
		m_TocEntrySize = TocEntryCount;
		for(unsigned int i = 0; i<m_TocEntrySize; ++i) 
		{
			unsigned char TocEntry = rtpPacket->RTPData()[firstTocEntryIndex + i];
			m_TocEntry[i] = TocEntry & 0x7C;
		}

		if(m_fmtpCRC > 0) 
		{
			frameHeaderSize += frameCRCSize;
			if(rtpPacket->RTPDataSize() < frameHeaderSize)
				break;
		}

		rtpPacket->Skip(frameHeaderSize);

		return true;

	}while(0);

	return false;
}



