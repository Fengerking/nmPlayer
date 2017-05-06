#include "CCheckTsPacketSize.h"

// ATSC maybe use 208 bytes
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CCheckTsPacketSize::CCheckTsPacketSize(void)
{
}

CCheckTsPacketSize::~CCheckTsPacketSize(void)
{
}

VO_U16 CCheckTsPacketSize::Check(VO_BYTE* pBuf, VO_U32 nLen)
{
	if (nLen <= 204) //only one packet
		return (VO_U16)nLen;

	if (nLen <= 408) //less than 2 packets
	{
		if (FindPacketHeader(pBuf, nLen, 188))
			return 188;
		if (FindPacketHeader(pBuf, nLen, 204))
			return 204;
		if (FindPacketHeader(pBuf, nLen, 192))
			return 192;
		return 0;
	}

	if (FindPacketHeader2(pBuf, nLen, 188))
		return 188;
	if (FindPacketHeader2(pBuf, nLen, 204))
		return 204;
	if (FindPacketHeader2(pBuf, nLen, 192))
		return 192;

	return 0;
}

VO_U8* CCheckTsPacketSize::FindPacketHeader(VO_BYTE* pData, VO_U32 cbData, VO_U8 packetSize)
{
	VO_U8* p = pData;
	VO_U8* p2 = pData + cbData - packetSize;
	while (p < p2)
	{
		if ( (*p == TRANSPORT_PACKET_SYNC_BYTE) && (*(p + packetSize) == TRANSPORT_PACKET_SYNC_BYTE) )
			return p;
		++p;
	}
	return 0;
}

VO_U8* CCheckTsPacketSize::FindPacketHeader2(VO_BYTE* pData, VO_U32 cbData, VO_U8 packetSize)
{
	VO_U8* p = FindPacketHeader(pData, cbData, packetSize);
	if (p)
	{
		VO_U8* p2 = p + packetSize * 2;
		if (p2 < pData + cbData)
			if (*p2 == TRANSPORT_PACKET_SYNC_BYTE)
				return p;
	}
	return 0;
}
