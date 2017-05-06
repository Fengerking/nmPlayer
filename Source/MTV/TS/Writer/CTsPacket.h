#pragma once
#include "fMacros.h"
#include "voType.h"
#include "bstype.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define DEFAULT_TS_PACKET_LEN	188

class CTsPacket : public BitsStream
{
public:
	CTsPacket(void);
	virtual ~CTsPacket(void);

public:
	VO_VOID		Init(VO_BYTE nOnePacketLen=DEFAULT_TS_PACKET_LEN);
	VO_VOID		Reset();
	VO_BYTE*	GetPacket();
	VO_BYTE		GetPacketSize();
	VO_BYTE		GetPacketHeadSize();


	VO_VOID	Set_sync_byte(uint8 val);
	VO_VOID Set_transport_error_indicator(bit1 val);
	VO_VOID Set_payload_unit_start_indicator(bit1 val);
	VO_VOID Set_transport_priority(bit1 val);
	VO_VOID Set_PID(bit13 val);
	VO_VOID Set_transport_scarambling_control(bit2 val);
	VO_VOID Set_adaptation_field_control(bit2 val);
	VO_VOID Set_continuity_counter(bit4 val);

private:
	VO_BYTE*	m_pPacket;
	VO_BYTE		m_nPacketSize;
	VO_BYTE		m_nPacketHeadSize;
};

#ifdef _VONAMESPACE
}
#endif