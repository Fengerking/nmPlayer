#include "tssi.spec.h"
#ifdef DMEMLEAK
#include "dmemleak.h"
#endif //DMEMLEAK

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

using namespace TS;


// program_association_item
//=============================================================================

program_association_item::program_association_item()
{
}

program_association_item::~program_association_item()
{
}

bool program_association_item::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(16, program_number);
	is.SkipBits(3); //reserved
	is.ReadBits(13, program_map_PID);
	return true;
}

#ifdef ENABLE_LOG
void program_association_item::Dump(Logger& os)
{
	os.StartBlock("program_association_item");
	os.WriteBits("program_number", program_number);
	os.WriteBits("program_map_PID", program_map_PID);
	os.EndBlock("program_association_item");
}
#endif //ENABLE_LOG


// program_association_section
//=============================================================================

program_association_section::program_association_section()
{
	program_association_items = NULL;
}

program_association_section::~program_association_section()
{
	if (program_association_items)
	{
		delete program_association_items;
		program_association_items = NULL;
	}
}

bool program_association_section::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(16, transport_stream_id);
	is.SkipBits(2); //reserved
	is.ReadBits(5, version_number);
	is.ReadBits(1, current_next_indicator);
	is.ReadBits(8, section_number);
	is.ReadBits(8, last_section_number);
	if (program_association_items) delete program_association_items; //Debug
	program_association_items = program_association_item::LoadAll(is, pEnd);
	return true;
}

#ifdef ENABLE_LOG
void program_association_section::Dump(Logger& os)
{
	os.StartBlock("program_association_section");
	os.WriteBits("transport_stream_id", transport_stream_id);
	os.WriteBits("version_number", version_number);
	os.WriteBits("current_next_indicator", current_next_indicator);
	os.WriteBits("section_number", section_number);
	os.WriteBits("last_section_number", last_section_number);
	if (program_association_items) program_association_items->DumpAll(os); 
	os.EndBlock("program_association_section");
}
#endif //ENABLE_LOG

// master_guide_item
//=============================================================================

master_guide_item::master_guide_item()
{
}

master_guide_item::~master_guide_item()
{
}

bool master_guide_item::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(16, table_type);
	is.SkipBits(3); //reserved
	is.ReadBits(13, table_type_PID);
    is.ReadBits(3, reserved2);
    is.ReadBits(5, table_type_version_number);
    is.ReadBits(32, number_bytes);
	is.ReadBits(4, reserved3);
    is.ReadBits(12, table_type_descriptors_length);

	is.SkipBytes(table_type_descriptors_length);
	return true;
}

// master_guide_section
//=============================================================================

master_guide_section::master_guide_section()
{
	m_pMaster_guide_items = NULL;
}

master_guide_section::~master_guide_section()
{
	if (m_pMaster_guide_items)
	{
		delete m_pMaster_guide_items;
		m_pMaster_guide_items = NULL;
	}
}

bool master_guide_section::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(16, table_id_extension);
	is.SkipBits(2); //reserved
	is.ReadBits(5, version_number);
	is.ReadBits(1, current_next_indicator);
	is.ReadBits(8, section_number);
	is.ReadBits(8, last_section_number);
    is.ReadBits(8, protocol_version);
	is.ReadBits(16, tables_defined);
	if (m_pMaster_guide_items) delete m_pMaster_guide_items; //Debug
	m_pMaster_guide_items = master_guide_item::LoadAll(is, pEnd);
	return true;
}

// CA_section
//=============================================================================

CA_section::CA_section()
{
	descriptors = NULL;
}

CA_section::~CA_section()
{
	if (descriptors)
	{
		delete descriptors;
		descriptors = NULL;
	}
}

bool CA_section::Load(BitStream& is, void* pEnd)
{
	is.SkipBits(18); //reserved
	is.ReadBits(5, version_number);
	is.ReadBits(1, current_next_indicator);
	is.ReadBits(8, section_number);
	is.ReadBits(8, last_section_number);
	if (descriptors) delete descriptors; //Debug
	descriptors = descriptor::LoadAll(is, pEnd);
	return true;
}

#ifdef ENABLE_LOG
void CA_section::Dump(Logger& os)
{
	os.StartBlock("CA_section");
	os.WriteBits("version_number", version_number);
	os.WriteBits("current_next_indicator", current_next_indicator);
	os.WriteBits("section_number", section_number);
	os.WriteBits("last_section_number", last_section_number);
	if (descriptors) descriptors->DumpAll(os); 
	os.EndBlock("CA_section");
}
#endif //ENABLE_LOG


// TS_program_map_item
//=============================================================================

TS_program_map_item::TS_program_map_item()
{
	descriptors = NULL;
}

TS_program_map_item::~TS_program_map_item()
{
	if (descriptors)
	{
		delete descriptors;
		descriptors = NULL;
	}
}

bool TS_program_map_item::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(8, stream_type);
	is.SkipBits(3); //reserved
	is.ReadBits(13, elementary_PID);
	is.SkipBits(4); //reserved
	is.ReadBits(12, ES_info_length);
	if (is.Position() + ES_info_length > pEnd) return false;
	if (descriptors) delete descriptors; //Debug
	descriptors = descriptor::LoadAll(is, is.Position() + ES_info_length);
	return true;
}

#ifdef ENABLE_LOG
void TS_program_map_item::Dump(Logger& os)
{
	os.StartBlock("TS_program_map_item");
	os.WriteBits("stream_type", stream_type);
	os.WriteBits("elementary_PID", elementary_PID);
	os.WriteBits("ES_info_length", ES_info_length);
	if (descriptors) descriptors->DumpAll(os); 
	os.EndBlock("TS_program_map_item");
}
#endif //ENABLE_LOG


// TS_program_map_section
//=============================================================================

TS_program_map_section::TS_program_map_section()
{
	descriptors = NULL;
	TS_program_map_items = NULL;
}

TS_program_map_section::~TS_program_map_section()
{
	if (descriptors)
	{
		delete descriptors;
		descriptors = NULL;
	}
	if (TS_program_map_items)
	{
		delete TS_program_map_items;
		TS_program_map_items = NULL;
	}
}

bool TS_program_map_section::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(16, program_number);
	is.SkipBits(2); //reserved
	is.ReadBits(5, version_number);
	is.ReadBits(1, current_next_indicator);
	is.ReadBits(8, section_number);
	is.ReadBits(8, last_section_number);
	is.SkipBits(3); //reserved
	is.ReadBits(13, PCR_PID);
	is.SkipBits(4); //reserved
	is.ReadBits(12, program_info_length);
	if (is.Position() + program_info_length > pEnd) return false;
	if (descriptors) delete descriptors; //Debug
	descriptors = descriptor::LoadAll(is, is.Position() + program_info_length);
	if (TS_program_map_items) delete TS_program_map_items; //Debug
	TS_program_map_items = TS_program_map_item::LoadAll(is, pEnd);
	return true;
}

#ifdef ENABLE_LOG
void TS_program_map_section::Dump(Logger& os)
{
	os.StartBlock("TS_program_map_section");
	os.WriteBits("program_number", program_number);
	os.WriteBits("version_number", version_number);
	os.WriteBits("current_next_indicator", current_next_indicator);
	os.WriteBits("section_number", section_number);
	os.WriteBits("last_section_number", last_section_number);
	os.WriteBits("PCR_PID", PCR_PID);
	os.WriteBits("program_info_length", program_info_length);
	if (descriptors) descriptors->DumpAll(os); 
	if (TS_program_map_items) TS_program_map_items->DumpAll(os); 
	os.EndBlock("TS_program_map_section");
}
#endif //ENABLE_LOG


// service_description_item
//=============================================================================

service_description_item::service_description_item()
{
	descriptors = NULL;
}

service_description_item::~service_description_item()
{
	if (descriptors)
	{
		delete descriptors;
		descriptors = NULL;
	}
}

bool service_description_item::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(16, service_id);
	is.SkipBits(6); //reserved_future_use
	is.ReadBits(1, EIT_schedule_flag);
	is.ReadBits(1, EIT_present_following_flag);
	is.ReadBits(3, running_status);
	is.ReadBits(1, free_CA_mode);
	is.ReadBits(12, descriptors_loop_length);
	if (is.Position() + descriptors_loop_length > pEnd) return false;
	if (descriptors) delete descriptors; //Debug
	descriptors = descriptor::LoadAll(is, is.Position() + descriptors_loop_length);
	return true;
}

#ifdef ENABLE_LOG
void service_description_item::Dump(Logger& os)
{
	os.StartBlock("service_description_item");
	os.WriteBits("service_id", service_id);
	os.WriteBits("EIT_schedule_flag", EIT_schedule_flag);
	os.WriteBits("EIT_present_following_flag", EIT_present_following_flag);
	os.WriteBits("running_status", running_status);
	os.WriteBits("free_CA_mode", free_CA_mode);
	os.WriteBits("descriptors_loop_length", descriptors_loop_length);
	if (descriptors) descriptors->DumpAll(os); 
	os.EndBlock("service_description_item");
}
#endif //ENABLE_LOG


// service_description_section
//=============================================================================

service_description_section::service_description_section()
{
	service_description_items = NULL;
}

service_description_section::~service_description_section()
{
	if (service_description_items)
	{
		delete service_description_items;
		service_description_items = NULL;
	}
}

bool service_description_section::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(16, transport_stream_id);
	is.SkipBits(2); //reserved
	is.ReadBits(5, version_number);
	is.ReadBits(1, current_next_indicator);
	is.ReadBits(8, section_number);
	is.ReadBits(8, last_section_number);
	is.ReadBits(16, original_network_id);
	is.SkipBits(8); //reserved_future_use
	if (service_description_items) delete service_description_items; //Debug
	service_description_items = service_description_item::LoadAll(is, pEnd);
	return true;
}

#ifdef ENABLE_LOG
void service_description_section::Dump(Logger& os)
{
	os.StartBlock("service_description_section");
	os.WriteBits("transport_stream_id", transport_stream_id);
	os.WriteBits("version_number", version_number);
	os.WriteBits("current_next_indicator", current_next_indicator);
	os.WriteBits("section_number", section_number);
	os.WriteBits("last_section_number", last_section_number);
	os.WriteBits("original_network_id", original_network_id);
	if (service_description_items) service_description_items->DumpAll(os); 
	os.EndBlock("service_description_section");
}
#endif //ENABLE_LOG


// network_information_item
//=============================================================================

network_information_item::network_information_item()
{
	descriptors = NULL;
}

network_information_item::~network_information_item()
{
	if (descriptors)
	{
		delete descriptors;
		descriptors = NULL;
	}
}

bool network_information_item::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(16, transport_stream_id);
	is.ReadBits(16, original_network_id);
	is.SkipBits(4); //reserved_future_use
	is.ReadBits(12, transport_descriptors_length);
	if (is.Position() + transport_descriptors_length > pEnd) return false;
	if (descriptors) delete descriptors; //Debug
	descriptors = descriptor::LoadAll(is, is.Position() + transport_descriptors_length);
	return true;
}

#ifdef ENABLE_LOG
void network_information_item::Dump(Logger& os)
{
	os.StartBlock("network_information_item");
	os.WriteBits("transport_stream_id", transport_stream_id);
	os.WriteBits("original_network_id", original_network_id);
	os.WriteBits("transport_descriptors_length", transport_descriptors_length);
	if (descriptors) descriptors->DumpAll(os); 
	os.EndBlock("network_information_item");
}
#endif //ENABLE_LOG


// network_information_section
//=============================================================================

network_information_section::network_information_section()
{
	descriptors = NULL;
	network_information_items = NULL;
}

network_information_section::~network_information_section()
{
	if (descriptors)
	{
		delete descriptors;
		descriptors = NULL;
	}
	if (network_information_items)
	{
		delete network_information_items;
		network_information_items = NULL;
	}
}

bool network_information_section::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(16, network_id);
	is.SkipBits(2); //reserved
	is.ReadBits(5, version_number);
	is.ReadBits(1, current_next_indicator);
	is.ReadBits(8, section_number);
	is.ReadBits(8, last_section_number);
	is.SkipBits(4); //reserved_future_use
	is.ReadBits(12, network_descriptors_length);
	if (is.Position() + network_descriptors_length > pEnd) return false;
	if (descriptors) delete descriptors; //Debug
	descriptors = descriptor::LoadAll(is, is.Position() + network_descriptors_length);
	is.SkipBits(4); //reserved_future_use
	is.ReadBits(12, transport_stream_loop_length);
	if (is.Position() + transport_stream_loop_length > pEnd) return false;
	if (network_information_items) delete network_information_items; //Debug
	network_information_items = network_information_item::LoadAll(is, is.Position() + transport_stream_loop_length);
	return true;
}

#ifdef ENABLE_LOG
void network_information_section::Dump(Logger& os)
{
	os.StartBlock("network_information_section");
	os.WriteBits("network_id", network_id);
	os.WriteBits("version_number", version_number);
	os.WriteBits("current_next_indicator", current_next_indicator);
	os.WriteBits("section_number", section_number);
	os.WriteBits("last_section_number", last_section_number);
	os.WriteBits("network_descriptors_length", network_descriptors_length);
	if (descriptors) descriptors->DumpAll(os); 
	os.WriteBits("transport_stream_loop_length", transport_stream_loop_length);
	if (network_information_items) network_information_items->DumpAll(os); 
	os.EndBlock("network_information_section");
}
#endif //ENABLE_LOG


// event_information_item
//=============================================================================

event_information_item::event_information_item()
{
	descriptors = NULL;
}

event_information_item::~event_information_item()
{
	if (descriptors)
	{
		delete descriptors;
		descriptors = NULL;
	}
}

bool event_information_item::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(16, event_id);
	is.ReadBits(40, start_time);
	is.ReadBits(24, duration);
	is.ReadBits(3, running_status);
	is.ReadBits(1, free_CA_mode);
	is.ReadBits(12, descriptors_loop_length);

	// lin: 20091230
	//if (is.Position() + descriptors_loop_length > pEnd) return false;
	if (is.Position() + descriptors_loop_length > pEnd)
	{
		uint8* curr = is.Position();
		uint8* end = (uint8*)pEnd;

		if(end <= curr)
			return false;

		descriptors_loop_length = end - curr;
		//descriptors_loop_length -= ((uint8*)pEnd - (is.Position() + descriptors_loop_length));
	}
	//end


	if (descriptors) delete descriptors; //Debug
	descriptors = descriptor::LoadAll(is, is.Position() + descriptors_loop_length);
	
	// 20100118
	return descriptors!=NULL;
	
	return true;
}

#ifdef ENABLE_LOG
void event_information_item::Dump(Logger& os)
{
	os.StartBlock("event_information_item");
	os.WriteBits("event_id", event_id);
	os.WriteBits("start_time", start_time);
	os.WriteBits("duration", duration);
	os.WriteBits("running_status", running_status);
	os.WriteBits("free_CA_mode", free_CA_mode);
	os.WriteBits("descriptors_loop_length", descriptors_loop_length);
	if (descriptors) descriptors->DumpAll(os); 
	os.EndBlock("event_information_item");
}
#endif //ENABLE_LOG


// event_information_section
//=============================================================================

event_information_section::event_information_section()
{
	event_information_items = NULL;
}

event_information_section::~event_information_section()
{
	if (event_information_items)
	{
		delete event_information_items;
		event_information_items = NULL;
	}
}

bool event_information_section::Load(BitStream& is, void* pEnd)
{
	// 20100118
// 	uint8* curr = is.Position();
// 	uint8* end = (uint8*)pEnd;
// 	// 下面的头信息长度为11字节
// 	if (curr+11 >= end)
// 	{
// 		return false;
// 	}
	// end

	is.ReadBits(16, service_id);
	is.SkipBits(2); //reserved
	is.ReadBits(5, version_number);
	is.ReadBits(1, current_next_indicator);
	is.ReadBits(8, section_number);
	is.ReadBits(8, last_section_number);
	is.ReadBits(16, transport_stream_id);
	is.ReadBits(16, original_network_id);
	is.ReadBits(8, segment_last_section_number);
	is.ReadBits(8, last_table_id);
	if (event_information_items) delete event_information_items; //Debug
	event_information_items = event_information_item::LoadAll(is, pEnd);
	return true;
}

#ifdef ENABLE_LOG
void event_information_section::Dump(Logger& os)
{
	os.StartBlock("event_information_section");
	os.WriteBits("service_id", service_id);
	os.WriteBits("version_number", version_number);
	os.WriteBits("current_next_indicator", current_next_indicator);
	os.WriteBits("section_number", section_number);
	os.WriteBits("last_section_number", last_section_number);
	os.WriteBits("transport_stream_id", transport_stream_id);
	os.WriteBits("original_network_id", original_network_id);
	os.WriteBits("segment_last_section_number", segment_last_section_number);
	os.WriteBits("last_table_id", last_table_id);
	if (event_information_items) event_information_items->DumpAll(os); 
	os.EndBlock("event_information_section");
}
#endif //ENABLE_LOG

// event_information_item_ATSC
//=============================================================================
event_information_item_ATSC::event_information_item_ATSC()
{
	memset(title_text, 0, 256);
	descriptors = NULL;
}

event_information_item_ATSC::~event_information_item_ATSC()
{
	if (descriptors)
	{
		delete descriptors;
		descriptors = NULL;
	}
}

bool event_information_item_ATSC::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(2, reserved1);
	is.ReadBits(14, event_id);
	is.ReadBits(32, start_time);
	is.ReadBits(2, reserved2);
	is.ReadBits(2, ETM_location);
    is.ReadBits(20, length_in_seconds);
	is.ReadBits(8, title_length);
	is.ReadData(title_length, title_text);
    is.ReadBits(4, reserved3);
    is.ReadBits(12, descriptors_loop_length);

	if (is.Position() + descriptors_loop_length > pEnd)
	{
		uint8* curr = is.Position();
		uint8* end = (uint8*)pEnd;

		if(end <= curr)
			return false;

		descriptors_loop_length = end - curr;
	}


	if (descriptors)
	{
		delete descriptors;
		descriptors = NULL;
	}

	descriptors = descriptor::LoadAll(is, is.Position() + descriptors_loop_length);
	return descriptors!=NULL;
}

// event_information_section_ATSC
//=============================================================================

event_information_section_ATSC::event_information_section_ATSC()
{
	event_information_items = NULL;
}

event_information_section_ATSC::~event_information_section_ATSC()
{
	if (event_information_items)
	{
		delete event_information_items;
		event_information_items = NULL;
	}
}

bool event_information_section_ATSC::Load(BitStream& is, void* pEnd)
{
	// 20100118
	// 	uint8* curr = is.Position();
	// 	uint8* end = (uint8*)pEnd;
	// 	// 下面的头信息长度为11字节
	// 	if (curr+11 >= end)
	// 	{
	// 		return false;
	// 	}
	// end

	is.ReadBits(16, source_id);
	is.ReadBits(2, reserved1); //reserved
	is.ReadBits(5, version_number);
	is.ReadBits(1, current_next_indicator);
	is.ReadBits(8, section_number);
	is.ReadBits(8, last_section_number);
	is.ReadBits(8, protocol_version);
	is.ReadBits(8, num_events_in_section);

	if (event_information_items) delete event_information_items; //Debug
	event_information_items = event_information_item_ATSC::LoadAll(is, pEnd);
	return true;
}

// time date section
time_date_section::time_date_section()
{
	UTC_time = 0;
}

time_date_section::~time_date_section()
{

}

bool time_date_section::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(40, UTC_time);

	return true;
}

