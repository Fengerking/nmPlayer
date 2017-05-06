#include "tsdesc.spec.h"
#ifdef DMEMLEAK
#include "dmemleak.h"
#endif //DMEMLEAK


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif



using namespace TS;


// video_stream_descriptor
//=============================================================================

video_stream_descriptor::video_stream_descriptor()
{
}

video_stream_descriptor::~video_stream_descriptor()
{
}

bool video_stream_descriptor::Load(BitStream& is, void* pEnd)
{
	if (length < 1)
	{
		return true;
	}
	is.ReadBits(1, multiple_frame_rate_flag);
	is.ReadBits(4, frame_rate_code);
	is.ReadBits(1, MPEG_1_only_flag);
	is.ReadBits(1, constrained_parameter_flag);
	is.ReadBits(1, still_picture_flag);
	if (MPEG_1_only_flag == 0 && length > 1)
	{
		is.ReadBits(8, profile_and_level_indication);
		is.ReadBits(2, chroma_format);
		is.ReadBits(1, frame_rate_extension_flag);
		is.SkipBits(5); //reserved
	}
	return true;
}

#ifdef ENABLE_LOG
void video_stream_descriptor::Dump(Logger& os)
{
	os.StartBlock("video_stream_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("multiple_frame_rate_flag", multiple_frame_rate_flag);
	os.WriteBits("frame_rate_code", frame_rate_code);
	os.WriteBits("MPEG_1_only_flag", MPEG_1_only_flag);
	os.WriteBits("constrained_parameter_flag", constrained_parameter_flag);
	os.WriteBits("still_picture_flag", still_picture_flag);
	if (MPEG_1_only_flag == 0)
	{
		os.WriteBits("profile_and_level_indication", profile_and_level_indication);
		os.WriteBits("chroma_format", chroma_format);
		os.WriteBits("frame_rate_extension_flag", frame_rate_extension_flag);
	}
	os.EndBlock("video_stream_descriptor");
}
#endif //ENABLE_LOG


// audio_stream_descriptor
//=============================================================================

audio_stream_descriptor::audio_stream_descriptor()
{
}

audio_stream_descriptor::~audio_stream_descriptor()
{
}

bool audio_stream_descriptor::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(1, free_format_flag);
	is.ReadBits(1, ID);
	is.ReadBits(2, layer);
	is.ReadBits(1, variable_rate_audio_indicator);
	is.SkipBits(3); //reserved
	return true;
}

#ifdef ENABLE_LOG
void audio_stream_descriptor::Dump(Logger& os)
{
	os.StartBlock("audio_stream_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("free_format_flag", free_format_flag);
	os.WriteBits("ID", ID);
	os.WriteBits("layer", layer);
	os.WriteBits("variable_rate_audio_indicator", variable_rate_audio_indicator);
	os.EndBlock("audio_stream_descriptor");
}
#endif //ENABLE_LOG


// hierarchy_descriptor
//=============================================================================

hierarchy_descriptor::hierarchy_descriptor()
{
}

hierarchy_descriptor::~hierarchy_descriptor()
{
}

bool hierarchy_descriptor::Load(BitStream& is, void* pEnd)
{
	is.SkipBits(4); //reserved
	is.ReadBits(4, hierarchy_type);
	is.SkipBits(2); //reserved
	is.ReadBits(6, hierarchy_layer_index);
	is.SkipBits(2); //reserved
	is.ReadBits(6, hierarchy_embedded_layer_index);
	is.SkipBits(2); //reserved
	is.ReadBits(6, hierarchy_channel);
	return true;
}

#ifdef ENABLE_LOG
void hierarchy_descriptor::Dump(Logger& os)
{
	os.StartBlock("hierarchy_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("hierarchy_type", hierarchy_type);
	os.WriteBits("hierarchy_layer_index", hierarchy_layer_index);
	os.WriteBits("hierarchy_embedded_layer_index", hierarchy_embedded_layer_index);
	os.WriteBits("hierarchy_channel", hierarchy_channel);
	os.EndBlock("hierarchy_descriptor");
}
#endif //ENABLE_LOG

// registration_descriptor
//=============================================================================
registration_descriptor::registration_descriptor()
{
	format_identifier = 0;
	pAdditional_identification_info = NULL;
}
registration_descriptor::~registration_descriptor()
{
	if (pAdditional_identification_info)
	{
		delete []pAdditional_identification_info;
		pAdditional_identification_info = NULL;
	}
}
bool registration_descriptor::Load(BitStream& is, void* pEnd)
{
	if (tag != 0x05 || length < 4 || is.Position()+length > pEnd)
	{
		return false;
	}
	uint8 tmp = 0;
	for (uint8 i = 1;i < 5;i++)
	{
		is.ReadData(1,&tmp);
		format_identifier|= tmp<< (8*(4-i));
	}
	if ((length - 4) > 0)
	{
		if (pAdditional_identification_info)
		{
			delete []pAdditional_identification_info;
			pAdditional_identification_info = NULL;
		}
		pAdditional_identification_info = new uint8[length - 4];
		is.ReadData(length - 4,pAdditional_identification_info);
	}
	return true;
}


// AC3_audio_stream_descriptor
//=============================================================================
AC3_audio_stream_descriptor::AC3_audio_stream_descriptor()
{
	text = NULL;
	pAdditional_info = NULL;
}

AC3_audio_stream_descriptor::~AC3_audio_stream_descriptor()
{
	if (text)
	{
		delete []text;
		text = NULL;
	}
	if (pAdditional_info)
	{
		delete []pAdditional_info;
		pAdditional_info = NULL;
	}
}

bool AC3_audio_stream_descriptor::Load(BitStream& is, void* pEnd)
{
	bit8 CurLen = length;
	if (is.Position()+length > pEnd)
	{
		is.SkipBytes((uint8*)pEnd - is.Position());
		return false;
	}
	if (length < 3)
	{
		is.SkipBytes(length);
		return false;
	}
	is.ReadBits(3,sample_rate_code);
	is.ReadBits(5,bsid);

	is.ReadBits(6,bit_rate_code);
	is.ReadBits(2,surround_mode);

	is.ReadBits(3,bsmod);
	is.ReadBits(4,num_channels);
	is.ReadBits(1,full_svc);
	if (!(CurLen -= 3))///<first termination point 
	{
		return true;
	}

	is.ReadBits(8,langcod);
	if (!(--CurLen))///<second termination point 
	{
		return true;
	}

	if (!num_channels)
	{
		is.ReadBits(8,langcod2);
		if (!(--CurLen))///<third termination point 
		{
			return true;
		}
	}
	
	if (bsmod < 2)
	{
		is.ReadBits(3,main_service_info.mainid);
		is.ReadBits(2,main_service_info.priority);
		is.ReadBits(3,main_service_info.reserved);
	}
	else
		is.ReadBits(8,asvcflags);
	
	if (!(--CurLen))///<forth termination point 
	{
		return true;
	}

	is.ReadBits(7,textlen);
	is.ReadBits(1,text_code);
	
	if (textlen > 0)
	{
		if (text)
		{
			delete []text;
			text = NULL;
		}
		text = new uint8[textlen];
		is.ReadData(textlen,text);
	}
	if (!(CurLen -= (textlen+1)))///<fifth termination point 
	{
		return true;
	}

	is.ReadBits(1,language_flag);
	is.ReadBits(1,language_flag_2);
	is.ReadBits(6,reserved);
	if (!(--CurLen))///<sixth termination point 
	{
		return true;
	}

	if (language_flag)
	{
		is.ReadData(3,language);
		if (!(CurLen -= 3))///<seventh termination point 
		{
			return true;
		}
	}
	if (language_flag_2)
	{
		is.ReadData(3,language2);
		if (!(CurLen -= 3))///<eighth termination point 
		{
			return true;
		}
	}
	if (CurLen > 0)
	{
		if (pAdditional_info)
		{
			delete []pAdditional_info;
			pAdditional_info = NULL;
		}
		pAdditional_info = new uint8[CurLen];
		is.ReadData(CurLen,pAdditional_info);
	}
	return true;
}




// data_stream_alignment_descriptor
//=============================================================================

data_stream_alignment_descriptor::data_stream_alignment_descriptor()
{
}

data_stream_alignment_descriptor::~data_stream_alignment_descriptor()
{
}

bool data_stream_alignment_descriptor::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(8, alignment_type);
	return true;
}

#ifdef ENABLE_LOG
void data_stream_alignment_descriptor::Dump(Logger& os)
{
	os.StartBlock("data_stream_alignment_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("alignment_type", alignment_type);
	os.EndBlock("data_stream_alignment_descriptor");
}
#endif //ENABLE_LOG


// target_background_grid_descriptor
//=============================================================================

target_background_grid_descriptor::target_background_grid_descriptor()
{
}

target_background_grid_descriptor::~target_background_grid_descriptor()
{
}

bool target_background_grid_descriptor::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(14, horizontal_size);
	is.ReadBits(14, vertical_size);
	is.ReadBits(4, aspect_ratio_information);
	return true;
}

#ifdef ENABLE_LOG
void target_background_grid_descriptor::Dump(Logger& os)
{
	os.StartBlock("target_background_grid_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("horizontal_size", horizontal_size);
	os.WriteBits("vertical_size", vertical_size);
	os.WriteBits("aspect_ratio_information", aspect_ratio_information);
	os.EndBlock("target_background_grid_descriptor");
}
#endif //ENABLE_LOG


// video_window_descriptor
//=============================================================================

video_window_descriptor::video_window_descriptor()
{
}

video_window_descriptor::~video_window_descriptor()
{
}

bool video_window_descriptor::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(14, horizontal_offset);
	is.ReadBits(14, vertical_offset);
	is.ReadBits(4, window_priority);
	return true;
}

#ifdef ENABLE_LOG
void video_window_descriptor::Dump(Logger& os)
{
	os.StartBlock("video_window_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("horizontal_offset", horizontal_offset);
	os.WriteBits("vertical_offset", vertical_offset);
	os.WriteBits("window_priority", window_priority);
	os.EndBlock("video_window_descriptor");
}
#endif //ENABLE_LOG


// ISO_639_language_item
//=============================================================================

ISO_639_language_item::ISO_639_language_item()
{
}

ISO_639_language_item::~ISO_639_language_item()
{
}

bool ISO_639_language_item::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(24, ISO_639_language_code);
	is.ReadBits(8, audio_type);
	return true;
}

#ifdef ENABLE_LOG
void ISO_639_language_item::Dump(Logger& os)
{
	os.StartBlock("ISO_639_language_item");
	os.WriteBits("ISO_639_language_code", ISO_639_language_code);
	os.WriteBits("audio_type", audio_type);
	os.EndBlock("ISO_639_language_item");
}
#endif //ENABLE_LOG


// ISO_639_language_descriptor
//=============================================================================

ISO_639_language_descriptor::ISO_639_language_descriptor()
{
	ISO_639_language_items = NULL;
}

ISO_639_language_descriptor::~ISO_639_language_descriptor()
{
	if (ISO_639_language_items)
	{
		delete ISO_639_language_items;
		ISO_639_language_items = NULL;
	}
}

bool ISO_639_language_descriptor::Load(BitStream& is, void* pEnd)
{
	if (is.Position() + length > pEnd) return false;
	if (ISO_639_language_items) delete ISO_639_language_items; //Debug
	ISO_639_language_items = ISO_639_language_item::LoadAll(is, is.Position() + length);
	return true;
}

#ifdef ENABLE_LOG
void ISO_639_language_descriptor::Dump(Logger& os)
{
	os.StartBlock("ISO_639_language_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	if (ISO_639_language_items) ISO_639_language_items->DumpAll(os); 
	os.EndBlock("ISO_639_language_descriptor");
}
#endif //ENABLE_LOG


// system_clock_descriptor
//=============================================================================

system_clock_descriptor::system_clock_descriptor()
{
}

system_clock_descriptor::~system_clock_descriptor()
{
}

bool system_clock_descriptor::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(1, external_clock_reference_indicator);
	is.SkipBits(1); //reserved
	is.ReadBits(6, clock_accuracy_integer);
	is.ReadBits(3, clock_accuracy_exponent);
	is.SkipBits(5); //reserved
	return true;
}

#ifdef ENABLE_LOG
void system_clock_descriptor::Dump(Logger& os)
{
	os.StartBlock("system_clock_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("external_clock_reference_indicator", external_clock_reference_indicator);
	os.WriteBits("clock_accuracy_integer", clock_accuracy_integer);
	os.WriteBits("clock_accuracy_exponent", clock_accuracy_exponent);
	os.EndBlock("system_clock_descriptor");
}
#endif //ENABLE_LOG


// multiplex_buffer_utilization_descriptor
//=============================================================================

multiplex_buffer_utilization_descriptor::multiplex_buffer_utilization_descriptor()
{
}

multiplex_buffer_utilization_descriptor::~multiplex_buffer_utilization_descriptor()
{
}

bool multiplex_buffer_utilization_descriptor::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(1, bound_valid_flag);
	is.ReadBits(15, LTW_offset_lower_bound);
	is.SkipBits(1); //reserved
	is.ReadBits(14, LTW_offset_upper_bound);
	return true;
}

#ifdef ENABLE_LOG
void multiplex_buffer_utilization_descriptor::Dump(Logger& os)
{
	os.StartBlock("multiplex_buffer_utilization_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("bound_valid_flag", bound_valid_flag);
	os.WriteBits("LTW_offset_lower_bound", LTW_offset_lower_bound);
	os.WriteBits("LTW_offset_upper_bound", LTW_offset_upper_bound);
	os.EndBlock("multiplex_buffer_utilization_descriptor");
}
#endif //ENABLE_LOG


// maximum_bitrate_descriptor
//=============================================================================

maximum_bitrate_descriptor::maximum_bitrate_descriptor()
{
}

maximum_bitrate_descriptor::~maximum_bitrate_descriptor()
{
}

bool maximum_bitrate_descriptor::Load(BitStream& is, void* pEnd)
{
	is.SkipBits(2); //reserved
	is.ReadBits(22, maximum_bitrate);
	return true;
}

#ifdef ENABLE_LOG
void maximum_bitrate_descriptor::Dump(Logger& os)
{
	os.StartBlock("maximum_bitrate_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("maximum_bitrate", maximum_bitrate);
	os.EndBlock("maximum_bitrate_descriptor");
}
#endif //ENABLE_LOG


// private_data_indicator_descriptor
//=============================================================================

private_data_indicator_descriptor::private_data_indicator_descriptor()
{
}

private_data_indicator_descriptor::~private_data_indicator_descriptor()
{
}

bool private_data_indicator_descriptor::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(32, private_data_indicator);
	return true;
}

#ifdef ENABLE_LOG
void private_data_indicator_descriptor::Dump(Logger& os)
{
	os.StartBlock("private_data_indicator_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("private_data_indicator", private_data_indicator);
	os.EndBlock("private_data_indicator_descriptor");
}
#endif //ENABLE_LOG


// smoothing_buffer_descriptor
//=============================================================================

smoothing_buffer_descriptor::smoothing_buffer_descriptor()
{
}

smoothing_buffer_descriptor::~smoothing_buffer_descriptor()
{
}

bool smoothing_buffer_descriptor::Load(BitStream& is, void* pEnd)
{
	is.SkipBits(2); //reserved
	is.ReadBits(22, sb_leak_rate);
	is.SkipBits(2); //reserved
	is.ReadBits(22, sb_size);
	return true;
}

#ifdef ENABLE_LOG
void smoothing_buffer_descriptor::Dump(Logger& os)
{
	os.StartBlock("smoothing_buffer_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("sb_leak_rate", sb_leak_rate);
	os.WriteBits("sb_size", sb_size);
	os.EndBlock("smoothing_buffer_descriptor");
}
#endif //ENABLE_LOG


// STD_descriptor
//=============================================================================

STD_descriptor::STD_descriptor()
{
}

STD_descriptor::~STD_descriptor()
{
}

bool STD_descriptor::Load(BitStream& is, void* pEnd)
{
	is.SkipBits(7); //reserved
	is.ReadBits(1, leak_valid_flag);
	return true;
}

#ifdef ENABLE_LOG
void STD_descriptor::Dump(Logger& os)
{
	os.StartBlock("STD_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("leak_valid_flag", leak_valid_flag);
	os.EndBlock("STD_descriptor");
}
#endif //ENABLE_LOG


// ibp_descriptor
//=============================================================================

ibp_descriptor::ibp_descriptor()
{
}

ibp_descriptor::~ibp_descriptor()
{
}

bool ibp_descriptor::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(1, closed_gop_flag);
	is.ReadBits(1, identical_gop_flag);
	is.ReadBits(14, max_gop_length);
	return true;
}

#ifdef ENABLE_LOG
void ibp_descriptor::Dump(Logger& os)
{
	os.StartBlock("ibp_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("closed_gop_flag", closed_gop_flag);
	os.WriteBits("identical_gop_flag", identical_gop_flag);
	os.WriteBits("max_gop_length", max_gop_length);
	os.EndBlock("ibp_descriptor");
}
#endif //ENABLE_LOG


// SL_descriptor
//=============================================================================

SL_descriptor::SL_descriptor()
{
}

SL_descriptor::~SL_descriptor()
{
}

bool SL_descriptor::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(16, ES_ID);
	return true;
}

#ifdef ENABLE_LOG
void SL_descriptor::Dump(Logger& os)
{
	os.StartBlock("SL_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("ES_ID", ES_ID);
	os.EndBlock("SL_descriptor");
}
#endif //ENABLE_LOG


//add by qichaoshen @ 2011-11-15
// FMC_descriptor
//=============================================================================

FMC_descriptor::FMC_descriptor()
{
	m_iFMCItemCount = 0;
    m_pES_IDs = NULL;
	m_pFMCNumbers = NULL;
}

FMC_descriptor::~FMC_descriptor()
{
	if(NULL != m_pES_IDs)
	{
		delete m_pES_IDs;
		m_pES_IDs = NULL;
	}

	if(NULL != m_pFMCNumbers)
	{
		delete m_pFMCNumbers;
        m_pFMCNumbers = NULL;
	}
}

bool FMC_descriptor::Load(BitStream& is, void* pEnd)
{
	uint8*  pCur = NULL;
    pCur = is.Position();

	uint8*  pEndForByte = pCur+length;
	int    iIndex = 0;
	bit16  ES_ID_value = 0;
	bit8   FMC_Number_value = 0;

	if(NULL != m_pES_IDs)
	{
		delete m_pES_IDs;
		m_pES_IDs = NULL;
	}

	if(NULL != m_pFMCNumbers)
	{
		delete m_pFMCNumbers;
        m_pFMCNumbers = NULL;
	}
	
	pCur = is.Position();
    m_iFMCItemCount = ((pEndForByte-pCur)+FMC_ITEM_LENGTH-1)/(FMC_ITEM_LENGTH);
	
	m_pES_IDs = new bit16[m_iFMCItemCount];
    m_pFMCNumbers = new bit8[m_iFMCItemCount];

	if((NULL == m_pES_IDs) || (NULL == m_pFMCNumbers))
	{
		return false;
	}
	
	while(pCur <= (pEndForByte-2))
	{
		is.ReadBits(16, ES_ID_value);
		m_pES_IDs[iIndex] = ES_ID_value;
		
		pCur = is.Position();
		if(pCur <=  (pEndForByte-1))
		{
			is.ReadBits(8, FMC_Number_value);
		    m_pFMCNumbers[iIndex] = FMC_Number_value;
		}
		else
		{
			break;
		}

		iIndex++;
		pCur = is.Position();
	}

	return true;
}
//add by qichaoshen @ 2011-11-15



//add by qichaoshen @ 2011-12-05
AVC_video_descriptor::AVC_video_descriptor()
{
	m_uSucceed = 0;
}

AVC_video_descriptor::~AVC_video_descriptor()
{

}

bool AVC_video_descriptor::Load(BitStream& is, void* pEnd)
{
	uint8*  pCur = NULL;

	pCur = is.Position();

	//13818-1 2007   Page 102
	if((pCur+4) <= pEnd)     
	{
		is.ReadBits(8, m_uProfile_idc);
		is.ReadBits(8, m_uAllFlags);
		is.ReadBits(8, m_uLevel_idc);
		is.ReadBits(8, m_uAVCStill_24Hour_Reserved);
		m_uSucceed = 1;
	}
	else
	{
		return false;
	}

	return true;
}

bool AVC_video_descriptor::GetProfileIdcValue(uint8* pReceiveField)
{
	if(m_uSucceed && pReceiveField != NULL)
	{
        *pReceiveField = m_uProfile_idc;
		return true;
	}

	return false;
}

bool AVC_video_descriptor::GetConstraintSets_AVCCompatible_FlagsValues(uint8* pReceiveField)
{
	if(m_uSucceed && pReceiveField != NULL)
	{
        *pReceiveField = m_uAllFlags;
		return true;
	}

	return false;
}

bool AVC_video_descriptor::GetLevelIdcValue(uint8* pReceiveField)
{
	if(m_uSucceed && pReceiveField != NULL)
	{
        *pReceiveField = m_uLevel_idc;
		return true;
	}

	return false;
}

bool AVC_video_descriptor::GetAVCStill_24Hour_ReservedValues(uint8* pReceiveField)
{
	if(m_uSucceed && pReceiveField != NULL)
	{
        *pReceiveField = m_uAVCStill_24Hour_Reserved;
		return true;
	}

	return false;
}
//add by qichaoshen @ 2011-12-05


// External_ES_ID_descriptor
//=============================================================================

External_ES_ID_descriptor::External_ES_ID_descriptor()
{
}

External_ES_ID_descriptor::~External_ES_ID_descriptor()
{
}

bool External_ES_ID_descriptor::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(16, External_ES_ID);
	return true;
}

#ifdef ENABLE_LOG
void External_ES_ID_descriptor::Dump(Logger& os)
{
	os.StartBlock("External_ES_ID_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("External_ES_ID", External_ES_ID);
	os.EndBlock("External_ES_ID_descriptor");
}
#endif //ENABLE_LOG


// service_descriptor
//=============================================================================

service_descriptor::service_descriptor()
{
	service_provider_name = NULL;
	service_name = NULL;
}

service_descriptor::~service_descriptor()
{
	if (service_provider_name)
	{
		delete service_provider_name;
		service_provider_name = NULL;
	}
	if (service_name)
	{
		delete service_name;
		service_name = NULL;
	}
}

bool service_descriptor::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(8, service_type);
	is.ReadBits(8, service_provider_name_length);
	service_provider_name = new bit8[service_provider_name_length + 1];
	is.ReadData(service_provider_name_length, service_provider_name);
	service_provider_name[service_provider_name_length] = 0;
	is.ReadBits(8, service_name_length);
	service_name = new bit8[service_name_length + 1];
	is.ReadData(service_name_length, service_name);
	service_name[service_name_length] = 0;
	return true;
}

#ifdef ENABLE_LOG
void service_descriptor::Dump(Logger& os)
{
	os.StartBlock("service_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("service_type", service_type);
	os.WriteBits("service_provider_name_length", service_provider_name_length);
	os.WriteData("service_provider_name", service_provider_name, service_provider_name_length);
	os.WriteBits("service_name_length", service_name_length);
	os.WriteData("service_name", service_name, service_name_length);
	os.EndBlock("service_descriptor");
}
#endif //ENABLE_LOG


// network_name_descriptor
//=============================================================================

network_name_descriptor::network_name_descriptor()
{
	network_name = NULL;
}

network_name_descriptor::~network_name_descriptor()
{
	if (network_name)
	{
		delete network_name;
		network_name = NULL;
	}
}

bool network_name_descriptor::Load(BitStream& is, void* pEnd)
{
	network_name = new bit8[length + 1];
	is.ReadData(length, network_name);
	network_name[length] = 0;
	return true;
}

#ifdef ENABLE_LOG
void network_name_descriptor::Dump(Logger& os)
{
	os.StartBlock("network_name_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteData("network_name", network_name, length);
	os.EndBlock("network_name_descriptor");
}
#endif //ENABLE_LOG


// service_list_item
//=============================================================================

service_list_item::service_list_item()
{
}

service_list_item::~service_list_item()
{
}

bool service_list_item::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(16, service_id);
	is.ReadBits(8, service_type);
	return true;
}

#ifdef ENABLE_LOG
void service_list_item::Dump(Logger& os)
{
	os.StartBlock("service_list_item");
	os.WriteBits("service_id", service_id);
	os.WriteBits("service_type", service_type);
	os.EndBlock("service_list_item");
}
#endif //ENABLE_LOG


// service_list_descriptor
//=============================================================================

service_list_descriptor::service_list_descriptor()
{
	service_list_items = NULL;
}

service_list_descriptor::~service_list_descriptor()
{
	if (service_list_items)
	{
		delete service_list_items;
		service_list_items = NULL;
	}
}

bool service_list_descriptor::Load(BitStream& is, void* pEnd)
{
	if (is.Position() + length > pEnd) return false;
	if (service_list_items) delete service_list_items; //Debug
	service_list_items = service_list_item::LoadAll(is, is.Position() + length);
	return true;
}

#ifdef ENABLE_LOG
void service_list_descriptor::Dump(Logger& os)
{
	os.StartBlock("service_list_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	if (service_list_items) service_list_items->DumpAll(os); 
	os.EndBlock("service_list_descriptor");
}
#endif //ENABLE_LOG


// satellite_delivery_system_descriptor
//=============================================================================

satellite_delivery_system_descriptor::satellite_delivery_system_descriptor()
{
}

satellite_delivery_system_descriptor::~satellite_delivery_system_descriptor()
{
}

bool satellite_delivery_system_descriptor::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(32, frequency);
	is.ReadBits(16, orbital_position);
	is.ReadBits(1, west_east_flag);
	is.ReadBits(2, polarization);
	if (modulation_system == 1)
	{
		is.ReadBits(2, roll_off);
	}
	else
	{
		is.SkipBits(2); //"00"
	}
	is.ReadBits(1, modulation_system);
	is.ReadBits(2, modulation_type);
	is.ReadBits(28, symbol_rate);
	is.ReadBits(4, FEC_inner);
	return true;
}

#ifdef ENABLE_LOG
void satellite_delivery_system_descriptor::Dump(Logger& os)
{
	os.StartBlock("satellite_delivery_system_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("frequency", frequency);
	os.WriteBits("orbital_position", orbital_position);
	os.WriteBits("west_east_flag", west_east_flag);
	os.WriteBits("polarization", polarization);
	if (modulation_system == 1)
	{
		os.WriteBits("roll_off", roll_off);
	}
	else
	{
	}
	os.WriteBits("modulation_system", modulation_system);
	os.WriteBits("modulation_type", modulation_type);
	os.WriteBits("symbol_rate", symbol_rate);
	os.WriteBits("FEC_inner", FEC_inner);
	os.EndBlock("satellite_delivery_system_descriptor");
}
#endif //ENABLE_LOG


// stream_identifier_descriptor
//=============================================================================

stream_identifier_descriptor::stream_identifier_descriptor()
{
}

stream_identifier_descriptor::~stream_identifier_descriptor()
{
}

bool stream_identifier_descriptor::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(8, component_tag);
	return true;
}

#ifdef ENABLE_LOG
void stream_identifier_descriptor::Dump(Logger& os)
{
	os.StartBlock("stream_identifier_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("component_tag", component_tag);
	os.EndBlock("stream_identifier_descriptor");
}
#endif //ENABLE_LOG


// multilingual_bouquet_name_item
//=============================================================================

multilingual_bouquet_name_item::multilingual_bouquet_name_item()
{
	bouquet_name = NULL;
}

multilingual_bouquet_name_item::~multilingual_bouquet_name_item()
{
	if (bouquet_name)
	{
		delete bouquet_name;
		bouquet_name = NULL;
	}
}

bool multilingual_bouquet_name_item::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(24, ISO_639_language_code);
	is.ReadBits(8, bouquet_name_length);
	bouquet_name = new bit8[bouquet_name_length + 1];
	is.ReadData(bouquet_name_length, bouquet_name);
	bouquet_name[bouquet_name_length] = 0;
	return true;
}

#ifdef ENABLE_LOG
void multilingual_bouquet_name_item::Dump(Logger& os)
{
	os.StartBlock("multilingual_bouquet_name_item");
	os.WriteBits("ISO_639_language_code", ISO_639_language_code);
	os.WriteBits("bouquet_name_length", bouquet_name_length);
	os.WriteData("bouquet_name", bouquet_name, bouquet_name_length);
	os.EndBlock("multilingual_bouquet_name_item");
}
#endif //ENABLE_LOG


// multilingual_bouquet_name_descriptor
//=============================================================================

multilingual_bouquet_name_descriptor::multilingual_bouquet_name_descriptor()
{
	multilingual_bouquet_name_items = NULL;
}

multilingual_bouquet_name_descriptor::~multilingual_bouquet_name_descriptor()
{
	if (multilingual_bouquet_name_items)
	{
		delete multilingual_bouquet_name_items;
		multilingual_bouquet_name_items = NULL;
	}
}

bool multilingual_bouquet_name_descriptor::Load(BitStream& is, void* pEnd)
{
	if (is.Position() + length > pEnd) return false;
	if (multilingual_bouquet_name_items) delete multilingual_bouquet_name_items; //Debug
	multilingual_bouquet_name_items = multilingual_bouquet_name_item::LoadAll(is, is.Position() + length);
	return true;
}

#ifdef ENABLE_LOG
void multilingual_bouquet_name_descriptor::Dump(Logger& os)
{
	os.StartBlock("multilingual_bouquet_name_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	if (multilingual_bouquet_name_items) multilingual_bouquet_name_items->DumpAll(os); 
	os.EndBlock("multilingual_bouquet_name_descriptor");
}
#endif //ENABLE_LOG


// short_event_descriptor
//=============================================================================

short_event_descriptor::short_event_descriptor()
{
	event_name = NULL;
	text = NULL;
}

short_event_descriptor::~short_event_descriptor()
{
	if (event_name)
	{
		delete event_name;
		event_name = NULL;
	}
	if (text)
	{
		delete text;
		text = NULL;
	}
}

bool short_event_descriptor::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(24, ISO_639_language_code);
	is.ReadBits(8, event_name_length);
	event_name = new bit8[event_name_length + 1];
	is.ReadData(event_name_length, event_name);
	event_name[event_name_length] = 0;
	is.ReadBits(8, text_length);
	text = new bit8[text_length + 1];
	is.ReadData(text_length, text);
	text[text_length] = 0;
	return true;
}

#ifdef ENABLE_LOG
void short_event_descriptor::Dump(Logger& os)
{
	os.StartBlock("short_event_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("ISO_639_language_code", ISO_639_language_code);
	os.WriteBits("event_name_length", event_name_length);
	os.WriteData("event_name", event_name, event_name_length);
	os.WriteBits("text_length", text_length);
	os.WriteData("text", text, text_length);
	os.EndBlock("short_event_descriptor");
}
#endif //ENABLE_LOG


// component_descriptor
//=============================================================================

component_descriptor::component_descriptor()
{
	text = NULL;
}

component_descriptor::~component_descriptor()
{
	if (text)
	{
		delete text;
		text = NULL;
	}
}

bool component_descriptor::Load(BitStream& is, void* pEnd)
{
	is.SkipBits(4); //reserved_future_use
	is.ReadBits(4, stream_content);
	is.ReadBits(8, component_type);
	is.ReadBits(8, component_tag);
	is.ReadBits(24, ISO_639_language_code);
	text = new bit8[length-6 + 1];
	is.ReadData(length-6, text);
	text[length-6] = 0;
	return true;
}

#ifdef ENABLE_LOG
void component_descriptor::Dump(Logger& os)
{
	os.StartBlock("component_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("stream_content", stream_content);
	os.WriteBits("component_type", component_type);
	os.WriteBits("component_tag", component_tag);
	os.WriteBits("ISO_639_language_code", ISO_639_language_code);
	os.WriteData("text", text, length-6);
	os.EndBlock("component_descriptor");
}
#endif //ENABLE_LOG


// content_item
//=============================================================================

content_item::content_item()
{
}

content_item::~content_item()
{
}

bool content_item::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(4, content_nibble_level_1);
	is.ReadBits(4, content_nibble_level_2);
	is.ReadBits(4, user_nibble_1);
	is.ReadBits(4, user_nibble_2);
	return true;
}

#ifdef ENABLE_LOG
void content_item::Dump(Logger& os)
{
	os.StartBlock("content_item");
	os.WriteBits("content_nibble_level_1", content_nibble_level_1);
	os.WriteBits("content_nibble_level_2", content_nibble_level_2);
	os.WriteBits("user_nibble_1", user_nibble_1);
	os.WriteBits("user_nibble_2", user_nibble_2);
	os.EndBlock("content_item");
}
#endif //ENABLE_LOG


// content_descriptor
//=============================================================================

content_descriptor::content_descriptor()
{
	content_items = NULL;
}

content_descriptor::~content_descriptor()
{
	if (content_items)
	{
		delete content_items;
		content_items = NULL;
	}
}

bool content_descriptor::Load(BitStream& is, void* pEnd)
{
	if (is.Position() + length > pEnd) return false;
	if (content_items) delete content_items; //Debug
	content_items = content_item::LoadAll(is, is.Position() + length);
	return true;
}

#ifdef ENABLE_LOG
void content_descriptor::Dump(Logger& os)
{
	os.StartBlock("content_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	if (content_items) content_items->DumpAll(os); 
	os.EndBlock("content_descriptor");
}
#endif //ENABLE_LOG


// parental_rating_item
//=============================================================================

parental_rating_item::parental_rating_item()
{
}

parental_rating_item::~parental_rating_item()
{
}

bool parental_rating_item::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(24, country_code);
	is.ReadBits(8, rating);
	return true;
}

#ifdef ENABLE_LOG
void parental_rating_item::Dump(Logger& os)
{
	os.StartBlock("parental_rating_item");
	os.WriteBits("country_code", country_code);
	os.WriteBits("rating", rating);
	os.EndBlock("parental_rating_item");
}
#endif //ENABLE_LOG


// parental_rating_descriptor
//=============================================================================

parental_rating_descriptor::parental_rating_descriptor()
{
	parental_rating_items = NULL;
}

parental_rating_descriptor::~parental_rating_descriptor()
{
	if (parental_rating_items)
	{
		delete parental_rating_items;
		parental_rating_items = NULL;
	}
}

bool parental_rating_descriptor::Load(BitStream& is, void* pEnd)
{
	if (is.Position() + length > pEnd) return false;
	if (parental_rating_items) delete parental_rating_items; //Debug
	parental_rating_items = parental_rating_item::LoadAll(is, is.Position() + length);
	return true;
}

#ifdef ENABLE_LOG
void parental_rating_descriptor::Dump(Logger& os)
{
	os.StartBlock("parental_rating_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	if (parental_rating_items) parental_rating_items->DumpAll(os); 
	os.EndBlock("parental_rating_descriptor");
}
#endif //ENABLE_LOG


// private_data_specifier_descriptor
//=============================================================================

private_data_specifier_descriptor::private_data_specifier_descriptor()
{
}

private_data_specifier_descriptor::~private_data_specifier_descriptor()
{
}

bool private_data_specifier_descriptor::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(32, private_data_specifier);
	return true;
}

#ifdef ENABLE_LOG
void private_data_specifier_descriptor::Dump(Logger& os)
{
	os.StartBlock("private_data_specifier_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("private_data_specifier", private_data_specifier);
	os.EndBlock("private_data_specifier_descriptor");
}
#endif //ENABLE_LOG


// teletext_descriptor_item
//=============================================================================

teletext_descriptor_item::teletext_descriptor_item()
{
}

teletext_descriptor_item::~teletext_descriptor_item()
{
}

bool teletext_descriptor_item::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(24, ISO_639_language_code);
	is.ReadBits(5, teletext_type);
	is.ReadBits(3, teletext_magazine_number);
	is.ReadBits(8, teletext_page_number);
	return true;
}

#ifdef ENABLE_LOG
void teletext_descriptor_item::Dump(Logger& os)
{
	os.StartBlock("teletext_descriptor_item");
	os.WriteBits("ISO_639_language_code", ISO_639_language_code);
	os.WriteBits("teletext_type", teletext_type);
	os.WriteBits("teletext_magazine_number", teletext_magazine_number);
	os.WriteBits("teletext_page_number", teletext_page_number);
	os.EndBlock("teletext_descriptor_item");
}
#endif //ENABLE_LOG


// teletext_descriptor
//=============================================================================

teletext_descriptor::teletext_descriptor()
{
	teletext_descriptor_items = NULL;
}

teletext_descriptor::~teletext_descriptor()
{
	if (teletext_descriptor_items)
	{
		delete teletext_descriptor_items;
		teletext_descriptor_items = NULL;
	}
}

bool teletext_descriptor::Load(BitStream& is, void* pEnd)
{
	if (is.Position() + length > pEnd) return false;
	if (teletext_descriptor_items) delete teletext_descriptor_items; //Debug
	teletext_descriptor_items = teletext_descriptor_item::LoadAll(is, is.Position() + length);
	return true;
}

#ifdef ENABLE_LOG
void teletext_descriptor::Dump(Logger& os)
{
	os.StartBlock("teletext_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	if (teletext_descriptor_items) teletext_descriptor_items->DumpAll(os); 
	os.EndBlock("teletext_descriptor");
}
#endif //ENABLE_LOG

// caption_service_descriptor
//=============================================================================
caption_service_descriptor::caption_service_descriptor()
{
    m_ulLength = 0;
	memset(m_descriptorData, 0, 256);
}

caption_service_descriptor::~caption_service_descriptor()
{
}

bool caption_service_descriptor::Load(BitStream& is, void* pEnd)
{
	m_ulLength = length;
	is.ReadData(length, m_descriptorData);
	return true;
}

uint8*        caption_service_descriptor::GetDescData()
{
    return (uint8*)m_descriptorData;
}

// metadata_descriptor
//=============================================================================
metadata_descriptor::metadata_descriptor()
{
    metadata_application_format = 0;
    metadata_application_format_identifier = 0;
    metadata_format = 0;
    metadata_format_identifier = 0;


    metadata_service_id = 0;
    decoder_config_flags = 0;
    DSM_CC_flag = 0;
    reserved = 0;
    service_identification_length = 0;
	memset(service_identification_record_byte, 0, 256);

    decoder_config_length = 0;
	memset(decoder_config_byte, 0, 256);

	dec_config_identification_record_length = 0;
	memset(dec_config_identification_record_byte, 0, 256);
	memset(m_descriptorData, 0, 256);
}

metadata_descriptor::~metadata_descriptor()
{
}

bool metadata_descriptor::Load(BitStream& is, void* pEnd)
{
	if (is.Position() + length > pEnd) 
	{
		return false;
	}

	is.ReadBits(16, metadata_application_format);
	if (metadata_application_format == 0xFFFF) 
	{
		is.ReadBits(32, metadata_application_format_identifier);
	}

	is.ReadBits(8, metadata_format);
    if (metadata_format== 0xFF)
	{
		is.ReadBits(32, metadata_format_identifier);
    }

	is.ReadBits(8, metadata_service_id);
    is.ReadBits(3, decoder_config_flags);
    is.ReadBits(1, DSM_CC_flag);
	is.ReadBits(4, reserved);

	if (DSM_CC_flag == 1)
	{
		is.ReadBits(8, service_identification_length);
		is.ReadData(service_identification_length, service_identification_record_byte);
	}

	if (decoder_config_flags == 0x1)
	{
		is.ReadBits(8, decoder_config_length);
		is.ReadData(decoder_config_length, decoder_config_byte);
	}


	if (decoder_config_flags == 0x3)
	{
		is.ReadBits(8, dec_config_identification_record_length);
		is.ReadData(dec_config_identification_record_length, dec_config_identification_record_byte);
	}

	//
	if (decoder_config_flags == 0x4)
	{
		is.ReadBits(8, decoder_config_metadata_service_id);
	}

	if (decoder_config_flags == 0x5 || decoder_config_flags == 0x6) 
	{
		is.ReadBits(8, reserved_data_length);
		is.ReadData(reserved_data_length, reserved_data);
	}

	return true;
}

uint8*        metadata_descriptor::GetDescData()
{
	return (uint8*)NULL;
}

uint32        metadata_descriptor::GetMetaDataFormatIdentifier()
{
	return metadata_application_format_identifier;
}

// subtitling_descriptor
//=============================================================================
subtitling_descriptor::subtitling_descriptor()
{
    m_uLanguageCount = 0;
	for(int i=0; i<MAX_SUBTITLING_ITEM_COUNT; i++)
	{
		m_aISOLanguage[i] = 0;
		m_aSubTitleType[i] = 0;
		m_aCompositionPageId[i] = 0;
		m_aAncillaryPageId[i] = 0;
	}
}

subtitling_descriptor::~subtitling_descriptor()
{
}

bool subtitling_descriptor::Load(BitStream& is, void* pEnd)
{
    uint8  uCount = 0;
	uint8*  pCur = NULL;
	uint8*  pEndForByte = NULL;

	pCur = is.Position();
	pEndForByte = pCur+length;
    if(pCur == NULL || pEnd == NULL)
	{
        return false;
	}
	
	uCount = (pEndForByte-pCur)/SUBTITLING_ITEM_LENGTH;
	m_uLanguageCount = uCount;
	for(int i=0; i<m_uLanguageCount; i++)
	{
		is.ReadBits(24, m_aISOLanguage[i]);
		is.ReadBits(8, m_aSubTitleType[i]);
		is.ReadBits(16, m_aCompositionPageId[i]);
		is.ReadBits(16, m_aAncillaryPageId[i]);
	}

	return true;
}

uint8  subtitling_descriptor::GetLanguageCount()
{
	return m_uLanguageCount;
}

bool   subtitling_descriptor::GetSubTitleItemInfoByIndx(uint32 ulIndex, uint32* pISOLanguage, uint8* pSubTitleType, uint16* pCompositionPageId, uint16* pAncillaryPageId)
{
    if(pSubTitleType == NULL || pISOLanguage == NULL || pCompositionPageId == NULL || pAncillaryPageId == NULL)
	{
	    return false;
	}
	if(ulIndex>=m_uLanguageCount)
	{
		return false;
	}

	*pISOLanguage = m_aISOLanguage[ulIndex];
	*pSubTitleType = m_aSubTitleType[ulIndex];
    *pCompositionPageId = m_aCompositionPageId[ulIndex];
	*pAncillaryPageId = m_aAncillaryPageId[ulIndex];
	return true;
}

