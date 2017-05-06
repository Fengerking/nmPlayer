#include "voLog.h"
#include "utility.h"
#include <memory.h>
#include <string.h>

#include "MediaStream.h"
#include "MediaStreamSocket.h"
#include "RTPPacket.h"
#include "G711AudioRTPParser.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CG711AudioRTPParser::CG711AudioRTPParser(CMediaStream * mediaStream, 
									   CMediaStreamSocket * rtpStreamSock)
: CRTPParser(mediaStream, rtpStreamSock)
{

}

CG711AudioRTPParser::~CG711AudioRTPParser()
{

}

bool CG711AudioRTPParser::ParseRTPPayloadHeader(CRTPPacket * rtpPacket)
{
	return true;
}

bool CG711AudioRTPParser::ParseRTPPayloadFrame(CRTPPacket * rtpPacket) 
{
	return CRTPParser::ParseRTPPayloadFrame(rtpPacket);
}
