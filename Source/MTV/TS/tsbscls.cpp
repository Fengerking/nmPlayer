#include "tsbscls.h"
#include "tsdesc.spec.h"
#include "tsstruct.h" //debug
#include "CDumper.h"
#ifdef DMEMLEAK
#include "dmemleak.h"
#endif //DMEMLEAK

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

using namespace TS;



// Item
//=============================================================================

Item::Item()
: more(NULL)
{
}

Item::~Item()
{
	if (more)
	{
		delete more;
		more = NULL;
	}
}

int Item::GetCount() const
{
	int c = 1;
	Item* node = more;
	while (node)
	{
		c++;
		node = node->more;
	}
	return c;
}

Item* Item::LoadAll(BitStream& is, void* pEnd, CREATOR creat)
{
	Item* head = NULL;
	Item* prev = NULL;
	Item* item = head = creat(is, pEnd);
	while (item)
	{
		bool b = item->Load(is, pEnd);
		if (!b)
		{
#ifdef _DEBUG
			OutputDebugString(_T("Descriptor load failed (usually data error).\n"));
			if (is.Position() > pEnd)
			{
				CDumper::WriteLog("Error: is.Position() > pEnd (A)!");
			}
#endif //_DEBUG

			// 20091230
			//return head;
			// 20100118
			if (item != head)
				delete head;
			delete item;
			return NULL;
		}
		if (prev)
			prev->more = item;
		prev = item;
		if (is.Position() >= pEnd)
			break;
		item = creat(is, pEnd);
	}
#if 0
	if (is.Position() > pEnd)
	{
		DBG_MSG(_T("Error: is.Position() > pEnd (B)!"));
	}
#endif
	return head;
}

#ifdef ENABLE_LOG
void Item::DumpAll(Logger& os)
{
	Dump(os);
	if (more)
		more->DumpAll(os);
}
#endif //ENABLE_LOG

// descriptor
//=============================================================================

descriptor* descriptor::CreateDescriptorByTag(bit8 tag)
{
	switch(tag)
	{
	// P63, ISO/IEC 13818-1: 2000(E)
	//------------------------------------------------------------
	case  2: return new video_stream_descriptor;
	case  3: return new audio_stream_descriptor;
	//case  4: return new hierarchy_descriptor();
	case  5: return new registration_descriptor();
	//case  6: return new data_stream_alignment_descriptor();
	case  7: return new target_background_grid_descriptor();
	//case  8: return new Video_window_descriptor();
	//case  9: return new CA_descriptor();
	case 10: return new ISO_639_language_descriptor();
	//case 11: return new System_clock_descriptor();
	//case 12: return new Multiplex_buffer_utilization_descriptor();
	//case 13: return new Copyright_descriptor();
	//case 14: return new Maximum_bitrate_descriptor();
	//case 15: return new Private_data_indicator_descriptor();
	//case 16: return new Smoothing_buffer_descriptor();
	case 17: return new STD_descriptor();
	//case 18: return new IBP_descriptor();
	//19-26: Defined in ISO/IEC 13818-6
	//case 27: return new MPEG-4_video_descriptor();
	//case 28: return new MPEG-4_audio_descriptor();
	case 29: return new IOD_descriptor();
	case 30: return new SL_descriptor();
	case 31: return new FMC_descriptor();
	//case 32: return new External_ES_ID_descriptor();
	//case 33: return new MuxCode_descriptor();
	//case 34: return new FmxBufferSize_descriptor();
	//case 35: return new MultiplexBuffer_descriptor();
	
	case 38: return new metadata_descriptor();
	case 40: return new AVC_video_descriptor();
	//P29, ETSI EN 300 468 V1.8.1 (2008-07)
	//------------------------------------------------------------
	case 0x40: return new network_name_descriptor(); 
	case 0x41: return new service_list_descriptor(); 
	//case 0x42: return new stuffing_descriptor(); 
	case 0x43: return new satellite_delivery_system_descriptor(); 
	//case 0x44: return new cable_delivery_system_descriptor(); 
	//case 0x45: return new VBI_data_descriptor(); 
	//case 0x46: return new VBI_teletext_descriptor(); 
	//case 0x47: return new bouquet_name_descriptor(); 
	case 0x48: return new service_descriptor(); 
	//case 0x49: return new country_availability_descriptor(); 
	//case 0x4A: return new linkage_descriptor(); 
	//case 0x4B: return new NVOD_reference_descriptor(); 
	//case 0x4C: return new time_shifted_service_descriptor(); 
	case 0x4D: return new short_event_descriptor(); 
	//case 0x4E: return new extended_event_descriptor(); 
	//case 0x4F: return new time_shifted_event_descriptor(); 
	case 0x50: return new component_descriptor(); 
	//case 0x51: return new mosaic_descriptor(); 
	case 0x52: return new stream_identifier_descriptor(); 
	//case 0x53: return new CA_identifier_descriptor(); 
	case 0x54: return new content_descriptor(); 
	case 0x55: return new parental_rating_descriptor(); 
	case 0x56: return new teletext_descriptor(); 
	//case 0x57: return new telephone_descriptor(); 
	//case 0x58: return new local_time_offset_descriptor(); 
	case 0x59: return new subtitling_descriptor(); 
	//case 0x5A: return new terrestrial_delivery_system_descriptor(); 
	//case 0x5B: return new multilingual_network_name_descriptor(); 
	case 0x5C: return new multilingual_bouquet_name_descriptor(); 
	//case 0x5D: return new multilingual_service_name_descriptor(); 
	//case 0x5E: return new multilingual_component_descriptor(); 
	case 0x5F: return new  private_data_specifier_descriptor(); 
	//case 0x60: return new service_move_descriptor(); 
	//case 0x61: return new short_smoothing_buffer_descriptor(); 
	//case 0x62: return new frequency_list_descriptor(); 
	//case 0x63: return new partial_transport_stream_descriptor(); 
	//case 0x64: return new data_broadcast_descriptor(); 
	//case 0x65: return new scrambling_descriptor(); 
	//case 0x66: return new data_broadcast_id_descriptor(); 
	//case 0x67: return new transport_stream_descriptor(); 
	//case 0x68: return new DSNG_descriptor(); 
	//case 0x69: return new PDC_descriptor(); 
	case 0x6A: return new AC3_descriptor();
	case 0x81: return new AC3_audio_stream_descriptor();
	case 0x86: return new caption_service_descriptor();
	//case 0x6B: return new ancillary_data_descriptor(); 
	//case 0x6C: return new cell_list_descriptor(); 
	//case 0x6D: return new cell_frequency_link_descriptor(); 
	//case 0x6E: return new announcement_support_descriptor(); 
	//case 0x6F: return new application_signalling_descriptor(); 
	//case 0x70: return new adaptation_field_data_descriptor(); 
	//case 0x71: return new service_identifier_descriptor(); 
	//case 0x72: return new service_availability_descriptor(); 
	//case 0x73: return new default_authority_descriptor(); 
	//case 0x74: return new related_content_descriptor(); 
	//case 0x75: return new TVA_id_descriptor(); 
	//case 0x76: return new content_identifier_descriptor(); 
	//case 0x77: return new time_slice_fec_identifier_descriptor(); 
	//case 0x78: return new ECM_repetition_rate_descriptor(); 
	//case 0x79: return new S2_satellite_delivery_system_descriptor(); 
	//case 0x7A: return new enhanced_AC-3_descriptor(); 
	//case 0x7B: return new  DTS_descriptor(); 
	//case 0x7C: return new  AAC_descriptor(); 
	//case 0x7D: reserved for future use 
	//case 0x7E: reserved for future use 
	//case 0x7F: extension descriptor
    
	//case 0xFF: forbidden
 
	default: return new unknown_descriptor(); //for debug
	}
	return NULL;
}

Item* descriptor::Create(BitStream& is, void* pEnd)
{
	bit8 tag;
	bit8 length;
	if ((uint8*)pEnd - is.Position() < 2)
		return NULL;

	is.ReadBits(8, tag);
	is.ReadBits(8, length);

	descriptor* d = CreateDescriptorByTag(tag);
	if (d)
	{
		d->tag = tag;
		d->length = length;
	}
	else
		is.SkipBytes(length); //make correct pointer
	return d;
}


descriptor* descriptor::FindFirst(bit8 tag)
{
	descriptor* node = this;
	do
	{
		if (node->tag == tag)
			return node;
		node = (descriptor*) node->more;
	}
	while (node);
	return NULL;
}



// unknown_descriptor
//=============================================================================

unknown_descriptor::unknown_descriptor()
{
	body = NULL;
}

unknown_descriptor::~unknown_descriptor()
{
	if (body)
	{
		delete body;
		body = NULL;
	}
}

bool unknown_descriptor::Load(BitStream& is, void* pEnd)
{
	if (length > 0)
	{
		if (is.Position() + length > pEnd)
			return false;
		body = new uint8[length];
		is.ReadData(length, body);
	}
	return true;
}

#ifdef ENABLE_LOG
void unknown_descriptor::Dump(Logger& os)
{
	os.StartBlock("unknown_descriptor");
	os.WriteBits("tag", tag);
	//os.WriteBits("length", length);
	os.WriteData("body", body, length);
	os.EndBlock("unknown_descriptor");
}
#endif //ENABLE_LOG


// SLPacketHeader
//=============================================================================

SLPacketHeader::SLPacketHeader()
:
	accessUnitStartFlag(1),
	accessUnitEndFlag(1),
	OCRflag(0),
	idleFlag(0),
	paddingFlag(0),
	paddingBits(0),
	packetSequenceNumber(0),
	DegPrioflag(0),
	degradationPriority(0),
	objectClockReference(0),
	randomAccessPointFlag(0),
	AU_sequenceNumber(0),
	decodingTimeStampFlag(0),
	compositionTimeStampFlag(0),
	instantBitrateFlag(0),
	decodingTimeStamp(0),
	compositionTimeStamp(0),
	accessUnitLength(0),
	instantBitrate(0)
{
}


uint8* SLPacketHeader::Parse(uint8* data, uint32 size, MP4::SLConfigDescriptor* SLCD)
{
	ISOM::MemStream ms(data, size);
	ISOM::ReaderMSB r(&ms);

	long p1 = ms.Position();

	if (SLCD->useAccessUnitStartFlag())
		r.ReadBits(&accessUnitStartFlag, 1);
	if (SLCD->useAccessUnitEndFlag())
		r.ReadBits(&accessUnitEndFlag, 1);
	if (SLCD->OCRLength > 0)
		r.ReadBits(&OCRflag, 1);
	if (SLCD->useIdleFlag())
		r.ReadBits(&idleFlag, 1);
	if (SLCD->usePaddingFlag())
		r.ReadBits(&paddingFlag, 1);
	if (paddingFlag)
		r.ReadBits(&paddingBits, 3);

	if (!idleFlag && (!paddingFlag || paddingBits != 0)) 
	{
		if (SLCD->Packet_SeqNumLength > 0)
			r.ReadBits(&packetSequenceNumber, SLCD->Packet_SeqNumLength);
		if (SLCD->degradationPriorityLength > 0)
			r.ReadBits(&DegPrioflag, 1);
		if (DegPrioflag)
			r.ReadBits(&degradationPriority, SLCD->degradationPriorityLength);
		if (OCRflag)
			r.ReadBits(&objectClockReference, SLCD->OCRLength);
		if (accessUnitStartFlag) 
		{
			if (SLCD->useRandomAccessPointFlag())
				r.ReadBits(&randomAccessPointFlag, 1);
			if (SLCD->AU_seqNumLength > 0)
				r.ReadBits(&AU_sequenceNumber, SLCD->AU_seqNumLength);
			if (SLCD->useTimeStampsFlag()) 
			{
				r.ReadBits(&decodingTimeStampFlag, 1);
				r.ReadBits(&compositionTimeStampFlag, 1);
			}
			if (SLCD->instantBitrateLength > 0)
				r.ReadBits(&instantBitrateFlag, 1);
			if (decodingTimeStampFlag)
				r.ReadBits(&decodingTimeStamp, SLCD->timeStampLength);
			if (compositionTimeStampFlag)
				r.ReadBits(&compositionTimeStamp, SLCD->timeStampLength);
			if (SLCD->AU_Length > 0)
				r.ReadBits(&accessUnitLength, SLCD->AU_Length);
			if (instantBitrateFlag)
				r.ReadBits(&instantBitrate, SLCD->instantBitrateLength);
		}
	}

	r.Align();
	long p2 = ms.Position();
	return data + (p2 - p1);
}



// IOD_descriptor
//=============================================================================

IOD_descriptor::IOD_descriptor()
{
	IOD = NULL;
}

IOD_descriptor::~IOD_descriptor()
{
	if (IOD)
	{
		delete IOD;
		IOD = NULL;
	}
}

bool IOD_descriptor::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(8, Scope_of_IOD_label);
	is.ReadBits(8, IOD_label);
	//IOD = InitialObjectDescriptor::LoadAll(is, pEnd);
	IOD = (MP4::InitialObjectDescriptor *) MP4::Descriptor::Load(is.Position(), (uint8*)pEnd - is.Position());
	is.SetPosition((uint8*)pEnd);
	return true;
}

#ifdef ENABLE_LOG
void IOD_descriptor::Dump(Logger& os)
{
	os.StartBlock("IOD_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("Scope_of_IOD_label", Scope_of_IOD_label);
	os.WriteBits("IOD_label", IOD_label);
	//TODO if (IOD) IOD->DumpAll(os); 
	os.EndBlock("IOD_descriptor");
}
#endif //ENABLE_LOG


// AC3_descriptor
//=============================================================================

AC3_descriptor::AC3_descriptor()
{
	additional_info = NULL;
}

AC3_descriptor::~AC3_descriptor()
{
	if (additional_info)
	{
		delete additional_info;
		additional_info = NULL;
	}
}

bool AC3_descriptor::Load(BitStream& is, void* pEnd)
{
	is.ReadBits(1, component_type_flag);
	is.ReadBits(1, bsid_flag);
	is.ReadBits(1, mainid_flag);
	is.ReadBits(1, asvc_flag);
	is.SkipBits(4); //reserved_flags
	additional_info_len = length -1;
	if (component_type_flag == 1)
	{
		is.ReadBits(8, component_type);
		additional_info_len--;
	}
	if (bsid_flag == 1)
	{
		is.ReadBits(8, bsid);
		additional_info_len--;
	}
	if (mainid_flag == 1)
	{
		is.ReadBits(8, mainid);
		additional_info_len--;
	}
	if (asvc_flag == 1)
	{
		is.ReadBits(8, asvc);
		additional_info_len--;
	}
	if (additional_info_len > 0)
	{
		additional_info = new bit8[additional_info_len];
		is.ReadData(additional_info_len, additional_info);
	}
	return true;
}

#ifdef ENABLE_LOG
void AC3_descriptor::Dump(Logger& os)
{
	os.StartBlock("AC3_descriptor");
	os.WriteBits("tag", tag);
	os.WriteBits("length", length);
	os.WriteBits("component_type_flag", component_type_flag);
	os.WriteBits("bsid_flag", bsid_flag);
	os.WriteBits("mainid_flag", mainid_flag);
	os.WriteBits("asvc_flag", asvc_flag);
	if (component_type_flag == 1)
	{
		os.WriteBits("component_type", component_type);
	}
	if (bsid_flag == 1)
	{
		os.WriteBits("bsid", bsid);
	}
	if (mainid_flag == 1)
	{
		os.WriteBits("mainid", mainid);
	}
	if (asvc_flag == 1)
	{
		os.WriteBits("asvc", asvc);
	}
	os.WriteData("additional_info", additional_info, additional_info_len);
	os.EndBlock("AC3_descriptor");
}
#endif //ENABLE_LOG

