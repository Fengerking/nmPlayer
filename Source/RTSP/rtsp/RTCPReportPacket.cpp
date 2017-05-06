#include <string.h>
#include "utility.h"
#include "network.h"

#include "RTCPReportPacket.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CRTCPReportPacket::CRTCPReportPacket(unsigned int maxPacketSize)
: m_maxPacketSize(maxPacketSize)
{
	m_packetBuffer = new unsigned char[m_maxPacketSize];TRACE_NEW("rtcpp_m_packetBuffer",m_packetBuffer);
	Reset();
}

CRTCPReportPacket::~CRTCPReportPacket()
{
	if(m_packetBuffer != NULL)
	{
		SAFE_DELETE_ARRAY(m_packetBuffer);
		m_packetBuffer = NULL;
	}
}

void CRTCPReportPacket::Reset()
{
	m_curPos = 0;
}

void CRTCPReportPacket::Pack(unsigned char * data, unsigned int dataSize)
{
	if(dataSize > RemainingPacketSize()) 
	{
		dataSize = RemainingPacketSize();
	}

	memcpy(CurPos(), data, dataSize);
	Increment(dataSize);
}

void CRTCPReportPacket::PackWord(unsigned int word)
{
	unsigned int n = HostToNetLong(word);
	Pack((unsigned char *)&n, 4);
}
