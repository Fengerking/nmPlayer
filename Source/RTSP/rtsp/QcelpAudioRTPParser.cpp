#include <memory.h>
#include <string.h>
#include "utility.h"
#include "MediaStream.h"
#include "MediaStreamSocket.h"
#include "RTPPacket.h"
#include "QcelpAudioRTPParser.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CQcelpAudioRTPParser::CQcelpAudioRTPParser(CMediaStream * mediaStream, 
					 CMediaStreamSocket * rtpStreamSock)
: CRTPParser(mediaStream, rtpStreamSock)
, m_interleaveL(0)
, m_interleaveN(0)
{

}

CQcelpAudioRTPParser::~CQcelpAudioRTPParser()
{

}

bool CQcelpAudioRTPParser::ParseRTPPayloadHeader(CRTPPacket * rtpPacket)
{
	do
	{
		int frameHeaderSize = 0;
		if(rtpPacket->RTPDataSize() < frameHeaderSize + 1)
			break;

		m_interleaveL = (rtpPacket->RTPData()[frameHeaderSize] & 0x38) >> 3;
		m_interleaveN = rtpPacket->RTPData()[frameHeaderSize] & 0x07;
		++frameHeaderSize;

		if(m_interleaveL > 5 || m_interleaveN > m_interleaveL)
			break;

		rtpPacket->Skip(frameHeaderSize);

		return true;
	}while(0);

	return false;
}

bool CQcelpAudioRTPParser::ParseRTPPayloadFrame(CRTPPacket * rtpPacket) 
{
	do
	{
		if(rtpPacket->RTPDataSize() == 0)
			break;

		int payloadFrameSize = 0;
		switch(rtpPacket->RTPData()[0])
		{
		case 0: { payloadFrameSize = 1; break; }
		case 1: { payloadFrameSize = 4; break; }
		case 2: { payloadFrameSize = 8; break; }
		case 3: { payloadFrameSize = 17; break; }
		case 4: { payloadFrameSize = 35; break; }
		default:{ payloadFrameSize = 0; break; }
		}

		if(payloadFrameSize > rtpPacket->RTPDataSize())
		{
			payloadFrameSize = rtpPacket->RTPDataSize();
		}
		memcpy(m_frameData + m_frameSize, rtpPacket->RTPData(), payloadFrameSize);
		m_frameSize += payloadFrameSize;
		rtpPacket->Skip(payloadFrameSize);

		return true;

	}while(1);

	return false;
}
