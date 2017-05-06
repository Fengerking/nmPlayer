#include <memory.h>
#include <string.h>

#include "utility.h"
#include "MediaStream.h"
#include "MediaStreamSocket.h"
#include "RTPPacket.h"
#include "AMRWBPAudioRTPParser.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

const unsigned int ISF2DurationTSTicks[14] = 
{
	1440, 2880, 2560, 2304,
	2160, 1920, 1728, 1536,
	1440, 1280, 1152, 1080,
	1024, 960
};

const float ISF2DurationMS[14] = 
{
	20,    40,    35.55, 32,
	30,    26.67, 24,    21.33,
	20,    17.78, 16,    15,
	14.22, 13.33
};

const unsigned short FT2FrameSizeWBP[48] = 
{
	17, 23, 32, 36,
	40, 46, 50, 58,
	60, 5,  34, 45,
	60, 60, 0,  0,
	26, 30, 34, 38,
	42, 48, 52, 60,
	31, 32, 35, 36,
	38, 40, 41, 43,
	45, 46, 48, 50,
	51, 53, 56, 58,
	60, 64, 65, 67,
	72, 74, 75, 80
};

CAMRWBPAudioRTPParser::CAMRWBPAudioRTPParser(CMediaStream * mediaStream, 
									         CMediaStreamSocket * rtpStreamSock,
											 unsigned int audioChannels,
											 unsigned int fmtpOctetAlign,
											 unsigned int fmtpInterleaving)
: CRTPParser(mediaStream, rtpStreamSock)
, m_audioChannels(audioChannels)
, m_fmtpOctetAlign(fmtpOctetAlign)
, m_fmtpInterleaving(fmtpInterleaving)

{
	m_PayloadHeader = 0;
	m_ISF = 0;
	m_TFI = 0;
	m_L = 0;

	m_TocEntry = NULL;
	m_TocEntryCount = 0;
	m_TocEntryIndex = 0;
	m_TocEntryFrameIndex = 0;
}

CAMRWBPAudioRTPParser::~CAMRWBPAudioRTPParser()
{
	SAFE_DELETE_ARRAY(m_TocEntry);
}

bool CAMRWBPAudioRTPParser::ParseRTPPayloadHeader(CRTPPacket * rtpPacket)
{
	if(m_fmtpInterleaving > 0)  // now we only support basic mode 
		return false;

	do
	{
		int frameHeaderSize = 0;
		if(rtpPacket->RTPDataSize() < frameHeaderSize + 1)
			break;

		m_PayloadHeader = rtpPacket->RTPData()[frameHeaderSize];
		m_ISF = (rtpPacket->RTPData()[frameHeaderSize] & 0xF8) >> 3;
		m_TFI = (rtpPacket->RTPData()[frameHeaderSize] & 0x6) >> 1;
		m_L   = rtpPacket->RTPData()[frameHeaderSize] & 0x1;
		if(m_ISF > 13)
			break;
		++frameHeaderSize;

		unsigned char * TocEntryData = rtpPacket->RTPData() + frameHeaderSize;
		unsigned int TocEntryCount = 0;
		bool lastTocEntry = true;
		do
		{
			if(rtpPacket->RTPDataSize() < frameHeaderSize + 2)
				break;

			unsigned char & TocEntry = rtpPacket->RTPData()[frameHeaderSize];
			lastTocEntry = (TocEntry & 0x80) == 0;
			TocEntry &= 0x7F;
//			unsigned char FT = TocEntry & 0x7F;
	

			++TocEntryCount;
			frameHeaderSize +=2; // jump to next TocEntry

		}while(!lastTocEntry);

		if(TocEntryCount > m_TocEntryCount) 
		{
			SAFE_DELETE_ARRAY(m_TocEntry);
			m_TocEntry = new unsigned char[TocEntryCount*2];TRACE_NEW("CAMRWBPAudioRTPParser::ParseRTPPayloadHeader m_TocEntry",m_TocEntry);
		
			m_TocEntryCount = TocEntryCount;
		}
		memcpy(m_TocEntry, TocEntryData, m_TocEntryCount*2);
		m_TocEntryIndex = 0;
		m_TocEntryFrameIndex = 0;

		rtpPacket->Skip(frameHeaderSize);

		return true;

	}while(0);

	return false;
}

bool CAMRWBPAudioRTPParser::ParseRTPPayloadFrame(CRTPPacket * rtpPacket) 
{
	do
	{
		if(rtpPacket->RTPDataSize() == 0)
			break;

		if(m_TocEntryIndex >= m_TocEntryCount) 
			break;

		unsigned char FrameType = m_TocEntry[m_TocEntryIndex*2];
		unsigned char frameCount = m_TocEntry[m_TocEntryIndex*2 + 1];
		unsigned short payloadFrameSize = FT2FrameSizeWBP[FrameType];
		if(m_TocEntryFrameIndex == frameCount)
		{
			++m_TocEntryIndex;
			m_TocEntryFrameIndex = 0;
			continue;
		}

		memcpy(m_frameData + m_frameSize, &FrameType, 1); // copy the Frame Type
		m_frameSize += 1;
		memcpy(m_frameData + m_frameSize, &m_ISF, 1);     // copy the ISF
		m_frameSize += 1;
		for(int i=0; i<4; ++i) // A super frame made of four sub frames
		{
			if(payloadFrameSize > rtpPacket->RTPDataSize())
				return false;

			memcpy(m_frameData + m_frameSize, rtpPacket->RTPData(), payloadFrameSize);
			m_frameSize += payloadFrameSize;
			rtpPacket->Skip(payloadFrameSize);

			++m_TocEntryFrameIndex;
		}

		return true;

	}while(1);

	return false;
}

