#pragma once

#include "voYYDef_TS.h"
#include "voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
const VO_U8 TRANSPORT_PACKET_SYNC_BYTE = 0x47;

class CCheckTsPacketSize
{
public:
	CCheckTsPacketSize(void);
	~CCheckTsPacketSize(void);

	VO_U16 Check(VO_BYTE* pBuf, VO_U32 nLen);

private:
	VO_U8* FindPacketHeader(VO_BYTE* pData, VO_U32 cbData, VO_U8 packetSize);
	VO_U8* FindPacketHeader2(VO_BYTE* pData, VO_U32 cbData, VO_U8 packetSize);

};
#ifdef _VONAMESPACE
}
#endif