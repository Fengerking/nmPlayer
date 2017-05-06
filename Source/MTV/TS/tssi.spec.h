#ifndef _TSSI_SPEC_H_
#define _TSSI_SPEC_H_

#include "voYYDef_TS.h"
#include "tsbscls.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


namespace TS {

class program_association_item
: public Item
{
public:
	program_association_item();
	virtual ~program_association_item();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG
	inline program_association_item* Next() { return (program_association_item*)(more); }
	static Item* Create(BitStream& is, void* pEnd) { return (uint8*)pEnd - is.Position() < 1 ? NULL : new program_association_item(); }
	static program_association_item* LoadAll(BitStream& is, void* pEnd) { return (program_association_item*) Item::LoadAll(is, pEnd, Create); }

public:
	bit16 program_number;
	bit13 program_map_PID;
};

class program_association_section
{
public:
	program_association_section();
	virtual ~program_association_section();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit16 transport_stream_id;
	bit5 version_number;
	bit1 current_next_indicator;
	bit8 section_number;
	bit8 last_section_number;
	program_association_item* program_association_items;
};

class CA_section
{
public:
	CA_section();
	virtual ~CA_section();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit5 version_number;
	bit1 current_next_indicator;
	bit8 section_number;
	bit8 last_section_number;
	descriptor* descriptors;
};

class TS_program_map_item
: public Item
{
public:
	TS_program_map_item();
	virtual ~TS_program_map_item();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG
	inline TS_program_map_item* Next() { return (TS_program_map_item*)(more); }
	static Item* Create(BitStream& is, void* pEnd) { return (uint8*)pEnd - is.Position() < 1 ? NULL : new TS_program_map_item(); }
	static TS_program_map_item* LoadAll(BitStream& is, void* pEnd) { return (TS_program_map_item*) Item::LoadAll(is, pEnd, Create); }

public:
	bit8 stream_type;
	bit13 elementary_PID;
	bit12 ES_info_length;
	descriptor* descriptors;
};

class TS_program_map_section
{
public:
	TS_program_map_section();
	virtual ~TS_program_map_section();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit16 program_number;
	bit5 version_number;
	bit1 current_next_indicator;
	bit8 section_number;
	bit8 last_section_number;
	bit13 PCR_PID;
	bit12 program_info_length;
	descriptor* descriptors;
	TS_program_map_item* TS_program_map_items;
};

class service_description_item
: public Item
{
public:
	service_description_item();
	virtual ~service_description_item();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG
	inline service_description_item* Next() { return (service_description_item*)(more); }
	static Item* Create(BitStream& is, void* pEnd) { return (uint8*)pEnd - is.Position() < 1 ? NULL : new service_description_item(); }
	static service_description_item* LoadAll(BitStream& is, void* pEnd) { return (service_description_item*) Item::LoadAll(is, pEnd, Create); }

public:
	bit16 service_id;
	bit1 EIT_schedule_flag;
	bit1 EIT_present_following_flag;
	bit3 running_status;
	bit1 free_CA_mode;
	bit12 descriptors_loop_length;
	descriptor* descriptors;
};

class service_description_section
{
public:
	service_description_section();
	virtual ~service_description_section();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit16 transport_stream_id;
	bit5 version_number;
	bit1 current_next_indicator;
	bit8 section_number;
	bit8 last_section_number;
	bit16 original_network_id;
	service_description_item* service_description_items;
};

class network_information_item
: public Item
{
public:
	network_information_item();
	virtual ~network_information_item();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG
	inline network_information_item* Next() { return (network_information_item*)(more); }
	static Item* Create(BitStream& is, void* pEnd) { return (uint8*)pEnd - is.Position() < 1 ? NULL : new network_information_item(); }
	static network_information_item* LoadAll(BitStream& is, void* pEnd) { return (network_information_item*) Item::LoadAll(is, pEnd, Create); }

public:
	bit16 transport_stream_id;
	bit16 original_network_id;
	bit12 transport_descriptors_length;
	descriptor* descriptors;
};

class network_information_section
{
public:
	network_information_section();
	virtual ~network_information_section();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit16 network_id;
	bit5 version_number;
	bit1 current_next_indicator;
	bit8 section_number;
	bit8 last_section_number;
	bit12 network_descriptors_length;
	descriptor* descriptors;
	bit12 transport_stream_loop_length;
	network_information_item* network_information_items;
};

class event_information_item
: public Item
{
public:
	event_information_item();
	virtual ~event_information_item();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG
	inline event_information_item* Next() { return (event_information_item*)(more); }
	static Item* Create(BitStream& is, void* pEnd) { return (uint8*)pEnd - is.Position() < 1 ? NULL : new event_information_item(); }
	static event_information_item* LoadAll(BitStream& is, void* pEnd) { return (event_information_item*) Item::LoadAll(is, pEnd, Create); }

public:
	bit16 event_id;
	bit40 start_time;
	bit24 duration;
	bit3 running_status;
	bit1 free_CA_mode;
	bit12 descriptors_loop_length;
	descriptor* descriptors;
};

class event_information_section
{
public:
	event_information_section();
	virtual ~event_information_section();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit16 service_id;
	bit5 version_number;
	bit1 current_next_indicator;
	bit8 section_number;
	bit8 last_section_number;
	bit16 transport_stream_id;
	bit16 original_network_id;
	bit8 segment_last_section_number;
	bit8 last_table_id;
	event_information_item* event_information_items;
};


class event_information_item_ATSC
	: public Item
{
public:
	event_information_item_ATSC();
	virtual ~event_information_item_ATSC();
	virtual bool Load(BitStream& is, void* pEnd);

	inline event_information_item_ATSC* Next() { return (event_information_item_ATSC*)(more); }
	static Item* Create(BitStream& is, void* pEnd) { return (uint8*)pEnd - is.Position() < 1 ? NULL : new event_information_item_ATSC(); }
	static event_information_item_ATSC* LoadAll(BitStream& is, void* pEnd) { return (event_information_item_ATSC*) Item::LoadAll(is, pEnd, Create); }

public:
    bit2  reserved1;
	bit14 event_id;
	bit32 start_time;
	bit2  reserved2;
	bit2  ETM_location;
    bit20 length_in_seconds;
	bit8  title_length;
	uint8 title_text[256];
	bit4  reserved3;
	bit12 descriptors_loop_length;
	descriptor* descriptors;
};
class event_information_section_ATSC
{
public:
	event_information_section_ATSC();
	virtual ~event_information_section_ATSC();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit16 source_id;
	bit2  reserved1;
	bit5  version_number;
	bit1 current_next_indicator;
	bit8 section_number;
	bit8 last_section_number;
	bit8 protocol_version;
	bit8 num_events_in_section;
	bit8 segment_last_section_number;
	bit8 last_table_id;
	event_information_item_ATSC* event_information_items;
};

class time_date_section
{
public:
	time_date_section();
	virtual ~time_date_section();
	virtual bool Load(BitStream& is, void* pEnd);

public:
	bit40	UTC_time;
};

class master_guide_item
	: public Item
{
public:
	master_guide_item();
	virtual ~master_guide_item();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG
	inline master_guide_item* Next() { return (master_guide_item*)(more); }
	static Item* Create(BitStream& is, void* pEnd) { return (uint8*)pEnd - is.Position() < 1 ? NULL : new master_guide_item(); }
	static master_guide_item* LoadAll(BitStream& is, void* pEnd) { return (master_guide_item*) Item::LoadAll(is, pEnd, Create); }

public:
	bit16  table_type;
    bit3   reserved1;
	bit13  table_type_PID;
    bit3   reserved2;
    bit5   table_type_version_number;
	bit32  number_bytes;
    bit4   reserved3;
	bit12 table_type_descriptors_length;
	descriptor* descriptors;
};

class master_guide_section
{
public:
	master_guide_section();
	virtual ~master_guide_section();
	virtual bool Load(BitStream& is, void* pEnd);

public:

	bit16    table_id_extension;
    bit2     reserved1;
    bit5     version_number;
    bit1     current_next_indicator;
	bit8     section_number;
    bit8     last_section_number;
    bit8     protocol_version;
    bit16    tables_defined;
	master_guide_item* m_pMaster_guide_items;

};

} //namespace TS

#ifdef _VONAMESPACE
}
#endif

#endif //_TSSI_SPEC_H_
