#ifndef _TSDESC_SPEC_H_
#define _TSDESC_SPEC_H_

#include "voYYDef_TS.h"
#include "tsbscls.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


namespace TS {

class video_stream_descriptor
: public descriptor
{
public:
	video_stream_descriptor();
	virtual ~video_stream_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit1 multiple_frame_rate_flag;
	bit4 frame_rate_code;
	bit1 MPEG_1_only_flag;
	bit1 constrained_parameter_flag;
	bit1 still_picture_flag;
	bit8 profile_and_level_indication;
	bit2 chroma_format;
	bit1 frame_rate_extension_flag;
};

class audio_stream_descriptor
: public descriptor
{
public:
	audio_stream_descriptor();
	virtual ~audio_stream_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit1 free_format_flag;
	bit1 ID;
	bit2 layer;
	bit1 variable_rate_audio_indicator;
};

class hierarchy_descriptor
: public descriptor
{
public:
	hierarchy_descriptor();
	virtual ~hierarchy_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit4 hierarchy_type;
	bit6 hierarchy_layer_index;
	bit6 hierarchy_embedded_layer_index;
	bit6 hierarchy_channel;
};
///add registration_descriptor for AC-3
class registration_descriptor
:public descriptor
{
public:
	registration_descriptor();
	virtual ~registration_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
	virtual uint32 GetFormatType(){return  format_identifier;}
private:
	uint32 format_identifier;
	uint8 * pAdditional_identification_info;

};

///add AC3_audio_stream_descriptor for AC-3 detail info
class AC3_audio_stream_descriptor
:public descriptor
{
public:
	AC3_audio_stream_descriptor();
	virtual ~AC3_audio_stream_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
private:
	bit3 sample_rate_code;
	bit5 bsid;

	bit6 bit_rate_code;
	bit2 surround_mode;

	bit3 bsmod;
	bit4 num_channels;
	bit1 full_svc;
	 
	bit8 langcod;
	bit8 langcod2;
	union
	{
		struct  
		{
			bit3 mainid;
			bit2 priority;
			bit3 reserved;
		}main_service_info;

		bit8 asvcflags;
	};
	bit7 textlen;
	bit1 text_code;

	uint8 * text;
	
	bit1 language_flag;
	bit1 language_flag_2;
	bit6 reserved;

	uint8 language[3];
	uint8 language2[3];
	uint8 *pAdditional_info;
};

class data_stream_alignment_descriptor
: public descriptor
{
public:
	data_stream_alignment_descriptor();
	virtual ~data_stream_alignment_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit8 alignment_type;
};

class target_background_grid_descriptor
: public descriptor
{
public:
	target_background_grid_descriptor();
	virtual ~target_background_grid_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit14 horizontal_size;
	bit14 vertical_size;
	bit4 aspect_ratio_information;
};

class video_window_descriptor
: public descriptor
{
public:
	video_window_descriptor();
	virtual ~video_window_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit14 horizontal_offset;
	bit14 vertical_offset;
	bit4 window_priority;
};

class ISO_639_language_item
: public Item
{
public:
	ISO_639_language_item();
	virtual ~ISO_639_language_item();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG
	inline ISO_639_language_item* Next() { return (ISO_639_language_item*)(more); }
	static Item* Create(BitStream& is, void* pEnd) { return (uint8*)pEnd - is.Position() < 1 ? NULL : new ISO_639_language_item(); }
	static ISO_639_language_item* LoadAll(BitStream& is, void* pEnd) { return (ISO_639_language_item*) Item::LoadAll(is, pEnd, Create); }

public:
	bit24 ISO_639_language_code;
	bit8 audio_type;
};

class ISO_639_language_descriptor
: public descriptor
{
public:
	ISO_639_language_descriptor();
	virtual ~ISO_639_language_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	ISO_639_language_item* ISO_639_language_items;
};

class system_clock_descriptor
: public descriptor
{
public:
	system_clock_descriptor();
	virtual ~system_clock_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit1 external_clock_reference_indicator;
	bit6 clock_accuracy_integer;
	bit3 clock_accuracy_exponent;
};

class multiplex_buffer_utilization_descriptor
: public descriptor
{
public:
	multiplex_buffer_utilization_descriptor();
	virtual ~multiplex_buffer_utilization_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit1 bound_valid_flag;
	bit15 LTW_offset_lower_bound;
	bit14 LTW_offset_upper_bound;
};

class maximum_bitrate_descriptor
: public descriptor
{
public:
	maximum_bitrate_descriptor();
	virtual ~maximum_bitrate_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit22 maximum_bitrate;
};

class private_data_indicator_descriptor
: public descriptor
{
public:
	private_data_indicator_descriptor();
	virtual ~private_data_indicator_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit32 private_data_indicator;
};

class smoothing_buffer_descriptor
: public descriptor
{
public:
	smoothing_buffer_descriptor();
	virtual ~smoothing_buffer_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit22 sb_leak_rate;
	bit22 sb_size;
};

class STD_descriptor
: public descriptor
{
public:
	STD_descriptor();
	virtual ~STD_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit1 leak_valid_flag;
};

class ibp_descriptor
: public descriptor
{
public:
	ibp_descriptor();
	virtual ~ibp_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit1 closed_gop_flag;
	bit1 identical_gop_flag;
	bit14 max_gop_length;
};

class SL_descriptor
: public descriptor
{
public:
	SL_descriptor();
	virtual ~SL_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit16 ES_ID;
};

//add by qichaoshen @ 2011-11-15



#define   FMC_ITEM_LENGTH   3 

class FMC_descriptor
: public descriptor
{
public:
	FMC_descriptor();
	virtual ~FMC_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	int   m_iFMCItemCount;

	bit16*   m_pES_IDs;
	bit8*    m_pFMCNumbers;
};

class AVC_video_descriptor
	: public descriptor
{
public:
    AVC_video_descriptor();
	virtual ~AVC_video_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
    bool GetProfileIdcValue(uint8* pReceiveField);
	bool GetConstraintSets_AVCCompatible_FlagsValues(uint8* pReceiveField);
	bool GetLevelIdcValue(uint8* pReceiveField);
	bool GetAVCStill_24Hour_ReservedValues(uint8* pReceiveField);

private:
	uint8 m_uProfile_idc;
	uint8 m_uAllFlags;
	uint8 m_uLevel_idc;
	uint8 m_uAVCStill_24Hour_Reserved;
	uint8 m_uSucceed;
};

//add by qichaoshen @ 2011-11-15

class External_ES_ID_descriptor
: public descriptor
{
public:
	External_ES_ID_descriptor();
	virtual ~External_ES_ID_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit16 External_ES_ID;
};

class service_descriptor
: public descriptor
{
public:
	service_descriptor();
	virtual ~service_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit8 service_type;
	bit8 service_provider_name_length;
	bit8* service_provider_name;
	bit8 service_name_length;
	bit8* service_name;
};

class network_name_descriptor
: public descriptor
{
public:
	network_name_descriptor();
	virtual ~network_name_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit8* network_name;
};

class service_list_item
: public Item
{
public:
	service_list_item();
	virtual ~service_list_item();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG
	inline service_list_item* Next() { return (service_list_item*)(more); }
	static Item* Create(BitStream& is, void* pEnd) { return (uint8*)pEnd - is.Position() < 1 ? NULL : new service_list_item(); }
	static service_list_item* LoadAll(BitStream& is, void* pEnd) { return (service_list_item*) Item::LoadAll(is, pEnd, Create); }

public:
	bit16 service_id;
	bit8 service_type;
};

class service_list_descriptor
: public descriptor
{
public:
	service_list_descriptor();
	virtual ~service_list_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	service_list_item* service_list_items;
};

class satellite_delivery_system_descriptor
: public descriptor
{
public:
	satellite_delivery_system_descriptor();
	virtual ~satellite_delivery_system_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit32 frequency;
	bit16 orbital_position;
	bit1 west_east_flag;
	bit2 polarization;
	bit2 roll_off;
	bit1 modulation_system;
	bit2 modulation_type;
	bit28 symbol_rate;
	bit4 FEC_inner;
};

class stream_identifier_descriptor
: public descriptor
{
public:
	stream_identifier_descriptor();
	virtual ~stream_identifier_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit8 component_tag;
};

class multilingual_bouquet_name_item
: public Item
{
public:
	multilingual_bouquet_name_item();
	virtual ~multilingual_bouquet_name_item();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG
	inline multilingual_bouquet_name_item* Next() { return (multilingual_bouquet_name_item*)(more); }
	static Item* Create(BitStream& is, void* pEnd) { return (uint8*)pEnd - is.Position() < 1 ? NULL : new multilingual_bouquet_name_item(); }
	static multilingual_bouquet_name_item* LoadAll(BitStream& is, void* pEnd) { return (multilingual_bouquet_name_item*) Item::LoadAll(is, pEnd, Create); }

public:
	bit24 ISO_639_language_code;
	bit8 bouquet_name_length;
	bit8* bouquet_name;
};

class multilingual_bouquet_name_descriptor
: public descriptor
{
public:
	multilingual_bouquet_name_descriptor();
	virtual ~multilingual_bouquet_name_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	multilingual_bouquet_name_item* multilingual_bouquet_name_items;
};

class short_event_descriptor
: public descriptor
{
public:
	short_event_descriptor();
	virtual ~short_event_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit24 ISO_639_language_code;
	bit8 event_name_length;
	bit8* event_name;
	bit8 text_length;
	bit8* text;
};

class component_descriptor
: public descriptor
{
public:
	component_descriptor();
	virtual ~component_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit4 stream_content;
	bit8 component_type;
	bit8 component_tag;
	bit24 ISO_639_language_code;
	bit8* text;
};

class content_item
: public Item
{
public:
	content_item();
	virtual ~content_item();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG
	inline content_item* Next() { return (content_item*)(more); }
	static Item* Create(BitStream& is, void* pEnd) { return (uint8*)pEnd - is.Position() < 1 ? NULL : new content_item(); }
	static content_item* LoadAll(BitStream& is, void* pEnd) { return (content_item*) Item::LoadAll(is, pEnd, Create); }

public:
	bit4 content_nibble_level_1;
	bit4 content_nibble_level_2;
	bit4 user_nibble_1;
	bit4 user_nibble_2;
};

class content_descriptor
: public descriptor
{
public:
	content_descriptor();
	virtual ~content_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	content_item* content_items;
};

class parental_rating_item
: public Item
{
public:
	parental_rating_item();
	virtual ~parental_rating_item();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG
	inline parental_rating_item* Next() { return (parental_rating_item*)(more); }
	static Item* Create(BitStream& is, void* pEnd) { return (uint8*)pEnd - is.Position() < 1 ? NULL : new parental_rating_item(); }
	static parental_rating_item* LoadAll(BitStream& is, void* pEnd) { return (parental_rating_item*) Item::LoadAll(is, pEnd, Create); }

public:
	bit24 country_code;
	bit8 rating;
};

class parental_rating_descriptor
: public descriptor
{
public:
	parental_rating_descriptor();
	virtual ~parental_rating_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	parental_rating_item* parental_rating_items;
};

class private_data_specifier_descriptor
: public descriptor
{
public:
	private_data_specifier_descriptor();
	virtual ~private_data_specifier_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	bit32 private_data_specifier;
};

class teletext_descriptor_item
: public Item
{
public:
	teletext_descriptor_item();
	virtual ~teletext_descriptor_item();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG
	inline teletext_descriptor_item* Next() { return (teletext_descriptor_item*)(more); }
	static Item* Create(BitStream& is, void* pEnd) { return (uint8*)pEnd - is.Position() < 1 ? NULL : new teletext_descriptor_item(); }
	static teletext_descriptor_item* LoadAll(BitStream& is, void* pEnd) { return (teletext_descriptor_item*) Item::LoadAll(is, pEnd, Create); }

public:
	bit24 ISO_639_language_code;
	bit5 teletext_type;
	bit3 teletext_magazine_number;
	bit8 teletext_page_number;
};

class teletext_descriptor
: public descriptor
{
public:
	teletext_descriptor();
	virtual ~teletext_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	teletext_descriptor_item* teletext_descriptor_items;
};

class  caption_service_descriptor
: public descriptor
{
public:
	caption_service_descriptor();
    virtual ~caption_service_descriptor();
    virtual bool Load(BitStream& is, void* pEnd);
    uint8*        GetDescData();
private:
	uint8    m_ulLength;
    uint8    m_descriptorData[256];
};

class  metadata_descriptor
	: public descriptor
{
public:
	metadata_descriptor();
	virtual ~metadata_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
	uint8*        GetDescData();
    uint32        GetMetaDataFormatIdentifier();
public:

	bit16       metadata_application_format;
	bit32       metadata_application_format_identifier;
	bit8	    metadata_format;
	bit32	    metadata_format_identifier;


	bit8        metadata_service_id;
	bit3	    decoder_config_flags;
	bit1	    DSM_CC_flag;
	bit4	    reserved;
	
	bit8        service_identification_length;
	uint8       service_identification_record_byte[256];
	
	bit8        decoder_config_length;
	bit8        decoder_config_byte[256];

	bit8        dec_config_identification_record_length;
	uint8       dec_config_identification_record_byte[256];

    bit8        decoder_config_metadata_service_id;
	uint8       m_descriptorData[256];

	bit8        reserved_data_length;
	uint8       reserved_data[256];


};

#define   SUBTITLING_ITEM_LENGTH      8
#define   MAX_SUBTITLING_ITEM_COUNT   8




class subtitling_descriptor
	: public descriptor
{
public:
	subtitling_descriptor();
	virtual ~subtitling_descriptor();
	virtual bool Load(BitStream& is, void* pEnd);
#ifdef ENABLE_LOG
	virtual void Dump(Logger& os);
#endif //ENABLE_LOG

public:
	uint8  GetLanguageCount();
    bool   GetSubTitleItemInfoByIndx(uint32 ulIndex, uint32* pISOLanguage, uint8* pSubTitleType, uint16* pCompositionPageId, uint16* pAncillaryPageId);
private:
	uint8  m_uLanguageCount;
    uint32 m_aISOLanguage[MAX_SUBTITLING_ITEM_COUNT];
	uint8  m_aSubTitleType[MAX_SUBTITLING_ITEM_COUNT];
	uint16 m_aCompositionPageId[MAX_SUBTITLING_ITEM_COUNT];
	uint16 m_aAncillaryPageId[MAX_SUBTITLING_ITEM_COUNT];
};




} //namespace TS

#ifdef _VONAMESPACE
}
#endif

#endif //_TSDESC_SPEC_H_
