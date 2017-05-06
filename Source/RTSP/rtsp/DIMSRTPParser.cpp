#include <memory.h>

#include "MediaStream.h"
#include "MediaStreamSocket.h"
#include "RTPPacket.h"
#include "DIMSRTPParser.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

// 3gpp TS 26.142 V7.1.0

CDIMSRTPParser::CDIMSRTPParser(CMediaStream * mediaStream, CMediaStreamSocket * rtpStreamSock)
: CRTPParser(mediaStream, rtpStreamSock)
{
	m_bHeadIsReady = false;	
	m_nRAP	= 0;
	m_nType	= 0;
	m_nCTR	= 0;
}

CDIMSRTPParser::~CDIMSRTPParser()
{

}
#define  DUMP_DIMS_RTP_DATA 1
bool CDIMSRTPParser::ParseRTPPayloadHeader(CRTPPacket * rtpPacket)
{
	do
	{
		int frameHeaderSize = 1;
		int packetSize = rtpPacket->RTPDataSize();
		if( packetSize< frameHeaderSize)
			break;
		unsigned char* data=rtpPacket->RTPData();
		int head=data[0]&0x000000ff;
		m_nRAP	= (head>>6)&0x01;
		m_nType	= (head>>3)&0x07;
		m_nCTR	= head&0x07;
		m_firstPacketInMultiPacketFrame = false;
		m_lastPacketInMultiPacketFrame  = false;
#if DUMP_DIMS_RTP_DATA
		if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
		{
			char* format = CLog::formatString;
			sprintf(format,"rap=%d,type=%d,ctr=%d\n",m_nRAP,m_nType,m_nCTR);
			CLog::Log.MakeLog(LL_AUDIO_TIME,"dimsRTP.txt",format);
		}
#endif
		switch(m_nType)
		{
		case DIMS_PCK_TYPE_AGGREGATION:
			m_firstPacketInMultiPacketFrame = true;
			m_lastPacketInMultiPacketFrame  = true;
 			break;
		case DIMS_PCK_TYPE_FRAGMENT_START:
			m_firstPacketInMultiPacketFrame = true;
			break;
		case DIMS_PCK_TYPE_FRAGMENT_MIDDLE:
		    break;
		case DIMS_PCK_TYPE_FRAGMENT_END:
			m_lastPacketInMultiPacketFrame  = true;
		    break;
		default:
		    break;
		}
		rtpPacket->Skip(frameHeaderSize);
		return true;

	}while(0);

	return false;
}

bool CDIMSRTPParser::ParseRTPPayloadFrame(CRTPPacket * rtpPacket) 
{
	do
	{
		unsigned char* data=rtpPacket->RTPData();
		int headSize = 0;
		int unitsize = 0;
		switch(m_nType)
		{
		case DIMS_PCK_TYPE_AGGREGATION:

			headSize = 2;
			unitsize = (data[0]<<8)|data[1];
			break;
		case DIMS_PCK_TYPE_FRAGMENT_START:
			headSize = 1;
			unitsize = rtpPacket->RTPDataSize()-1;
			break;
		case DIMS_PCK_TYPE_FRAGMENT_MIDDLE:
		case DIMS_PCK_TYPE_FRAGMENT_END:
			headSize = 0;
			unitsize = rtpPacket->RTPDataSize();
			break;
		default:
			break;
		}
		if(rtpPacket->RTPDataSize() <unitsize)
			break;
#if DUMP_DIMS_RTP_DATA
		if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
		{
			char* format = CLog::formatString;
			sprintf(format,"datasize=%d,headsize=%d,packetSize=%d\n",unitsize,headSize,rtpPacket->RTPDataSize());
			CLog::Log.MakeLog(LL_AUDIO_TIME,"dimsRTP.txt",format);
		}
#endif//DUMP_DIMS_RTP_DATA
		memcpy(m_frameData + m_frameSize, data+headSize, unitsize);
		m_frameSize += unitsize;
		rtpPacket->Skip(unitsize+headSize);

		return true;

	}while(1);

	return false;
}



