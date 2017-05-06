#include "commonheader.h"
#include "netbase.h"
#include "MediaStream.h"
#include "SynchronizeStreams.h"
#include "ParseRTCP.h"

CParseRTCP::CParseRTCP(CMediaStream * pMediaStream)
: m_pMediaStream(pMediaStream)
{
}

CParseRTCP::~CParseRTCP()
{
}

VO_U32 CParseRTCP::Init()
{
	return VO_ERR_PARSER_OK;
}

#define SKIP(n) { _rtcpData += (n); rtcpDataSize -= (n); }

VO_U32 CParseRTCP::Process(const unsigned char * _rtcpData, int rtcpDataSize)
{
	do
	{
		if(rtcpDataSize < 4)
			break;

		unsigned int rtcpHeader = ntohl(*(unsigned int *)_rtcpData);
		if((rtcpHeader & 0xE0FE0000) != (0x80000000 | (RTCP_PACKET_SR << 16)))
			break;

		while(1)
		{
			unsigned int rtcpHeader_RC = (rtcpHeader >> 24) & 0x1F;
			unsigned int rtcpHeader_PT = (rtcpHeader >> 16) & 0xFF;
			unsigned int rtcpHeader_length = (rtcpHeader & 0xFFFF) * 4;
			SKIP(4)
			if(rtcpHeader_length > (unsigned int)rtcpDataSize)
				break;

			switch(rtcpHeader_PT)
			{
			case RTCP_PACKET_SR:
				{
					if(rtcpHeader_length < 4)
						break;
					rtcpHeader_length -= 4;

					unsigned int senderSSRC = ntohl(*(unsigned int *)_rtcpData); 
					SKIP(4)

					if(rtcpHeader_length < 20)
						break; 
					rtcpHeader_length -= 20;

					unsigned int ntpTimestampMSW = ntohl(*(unsigned int *)_rtcpData); 
					SKIP(4)
					unsigned int ntpTimestampLSW = ntohl(*(unsigned int *)_rtcpData); 
					SKIP(4)
					unsigned int rtpTimestamp = ntohl(*(unsigned int *)_rtcpData);
					SKIP(4)

					CSynchronizeStreams * pSyncStreams = CSynchronizeStreams::CreateInstance();
					pSyncStreams->SynchronizeStream(m_pMediaStream, ntpTimestampMSW, ntpTimestampLSW, rtpTimestamp);

					SKIP(8)
				}
			case RTCP_PACKET_RR:
				{
					unsigned int reportBlocksSize = rtcpHeader_RC * (6 * 4);
					if(rtcpHeader_length < reportBlocksSize)
						break;
					rtcpHeader_length -= reportBlocksSize;
					SKIP(reportBlocksSize)
					break;
				}
			case RTCP_PACKET_SDES:
				{
					for(unsigned int i=0; i<rtcpHeader_RC; ++i)
					{
						if(rtcpHeader_length < 4)
							break;
						rtcpHeader_length -= 4;

						unsigned int senderSSRC = ntohl(*(unsigned int *)_rtcpData); 
						SKIP(4)

						CSDESItem SDESItem(_rtcpData);
						unsigned int SDESItemSize = SDESItem.TotalSize();
						while((SDESItemSize % 4) > 0) 
							++SDESItemSize;

						rtcpHeader_length -= SDESItemSize;
						SKIP(SDESItemSize)
					}
					break;
				}
			case RTCP_PACKET_BYE:
				{
					unsigned int ssrc_csrc_identifiers_size = rtcpHeader_RC * 4;
					if(rtcpHeader_length < ssrc_csrc_identifiers_size)
						break;
					rtcpHeader_length -= ssrc_csrc_identifiers_size;
					SKIP(ssrc_csrc_identifiers_size)
					break;
				}
			default:
				break;
			}

			SKIP(rtcpHeader_length)


			if(rtcpDataSize < 4)
				break;

			rtcpHeader = ntohl(*(unsigned int *)_rtcpData);
			if((rtcpHeader & 0xC0000000) != 0x80000000) 
				break;
		}

		return VO_ERR_PARSER_OK;

	}while(0);

	return VO_ERR_PARSER_ERROR;
}



// class CSDESItem member functions

CSDESItem::CSDESItem(unsigned char tag, unsigned char * _value)
{
	size_t length = strlen((char *)_value);
	if(length > 255)
		length = 255;

	m_SDESData[0] = tag;
	m_SDESData[1] = (unsigned char)length;
	memcpy(m_SDESData+2, _value, length);

	// Pad the trailing bytes to a 4-byte boundary:
	while((length) % 4 > 0) 
		m_SDESData[2 + length++] = '\0';
}

CSDESItem::CSDESItem(const unsigned char * _SDESItemData)
{
	unsigned char SDESTag = _SDESItemData[0];
	unsigned char SDESLength = _SDESItemData[1];
	if(SDESLength > 255)
		SDESLength = 255;

	memset(m_SDESData, 0, 2+0xFF);
	m_SDESData[0] = SDESTag;
	m_SDESData[1] = SDESLength;
	memcpy(m_SDESData+2, _SDESItemData+2, SDESLength);
}

CSDESItem::~CSDESItem()
{
}

unsigned char CSDESItem::SDESTag() 
{ 
	return m_SDESData[0];
}

unsigned char * CSDESItem::SDESData()
{
	return m_SDESData+2;
}

unsigned int CSDESItem::SDESDataSize() 
{ 
	return (unsigned int)m_SDESData[1];
}

unsigned int CSDESItem::TotalSize() 
{ 
	return 2 + (unsigned int)m_SDESData[1]; 
}
