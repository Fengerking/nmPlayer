#include "CTsPacket.h"
#include "cmnMemory.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CTsPacket::CTsPacket(void)
:BitsStream(VO_NULL)
,m_pPacket(VO_NULL)
,m_nPacketSize(DEFAULT_TS_PACKET_LEN)
,m_nPacketHeadSize(0)
{
}

CTsPacket::~CTsPacket(void)
{
	if (m_pPacket)
	{
		cmnMemFree(0, m_pPacket);
		m_pPacket = VO_NULL;
	}
}

VO_VOID	CTsPacket::Init(VO_BYTE nOnePacketLen/*=DEFAULT_TS_PACKET_LEN*/)
{
	VO_MEM_INFO vmi = {0};
	vmi.Size		= nOnePacketLen;
	cmnMemAlloc(0, &vmi);
	m_pPacket		= (VO_BYTE*)vmi.VBuffer;
	m_nPacketSize	= nOnePacketLen;

	Reset();

	BitsStream::Init(m_pPacket);
}

VO_VOID	CTsPacket::Reset()
{
	if (m_pPacket)
	{
		cmnMemSet(0, m_pPacket, 0xFF, m_nPacketSize);
	}

	BitsStream::Init(m_pPacket);
}

VO_BYTE* CTsPacket::GetPacket()
{
	return m_pPacket;
}

VO_BYTE	CTsPacket::GetPacketSize()
{
	return m_nPacketSize;
}

VO_BYTE	CTsPacket::GetPacketHeadSize()
{
	return 0;
}



/////////////////////////////////////////
VO_VOID	CTsPacket::Set_sync_byte(uint8 val)
{

}

VO_VOID CTsPacket::Set_transport_error_indicator(bit1 val)
{

}

VO_VOID CTsPacket::Set_payload_unit_start_indicator(bit1 val)
{

}

VO_VOID CTsPacket::Set_transport_priority(bit1 val)
{

}

VO_VOID CTsPacket::Set_PID(bit13 val)
{

}

VO_VOID CTsPacket::Set_transport_scarambling_control(bit2 val)
{

}

VO_VOID CTsPacket::Set_adaptation_field_control(bit2 val)
{

}

VO_VOID CTsPacket::Set_continuity_counter(bit4 val)
{

}




