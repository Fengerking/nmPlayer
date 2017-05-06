#include "tsstruct.h"
#include "readutil.h"
#include <memory.h>
#include <stdlib.h>
#include "tsconst.h"
#ifdef DMEMLEAK
#include "dmemleak.h"
#endif //DMEMLEAK
#include "CDumper.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

using namespace TS;



// PIDInfo
//==============================================================================

PIDInfo::PIDInfo()
:PID(0x1fff)
,continuity_counter(0)
,count(0)
,countDuplicate(0)
{
}

void PIDInfo::Reset()
{
	count = 0;
	countDuplicate = 0;
	continuity_counter = 0;
}

PIDsInfo::PIDsInfo()
: _countPID(0)
{
}

PIDInfo* PIDsInfo::GetPIDInfo(bit13 PID)
{
	PIDInfo* p = _PIDInfo;
	for (int i = 0; i < _countPID; i++)
	{
		if (PID == p->PID)
			return p;
		++p;
	}
	return 0;
}

void PIDsInfo::Reset()
{
	PIDInfo* p = _PIDInfo;
	for (int i = 0; i < _countPID; i++)
	{
		p->Reset();
		++p;
	}
}

PIDInfo* PIDsInfo::AddPIDInfo(bit13 PID,bit4 continuity_counter)
{
	if (_countPID == MaxTotalPID)
		return 0;
	PIDInfo* p = _PIDInfo + _countPID;
	++_countPID;
	p->Reset();
	p->PID = PID;
	p->continuity_counter = continuity_counter;
	return p;
}

PIDInfo* PIDsInfo::GetAddPIDInfo(bit13 PID,bit4 continuity_counter)
{
	PIDInfo* p = GetPIDInfo(PID);
	return p ? p : AddPIDInfo(PID,continuity_counter);
}

#ifndef _IOS
int __cdecl PIDInfo::compareByPID(const void *arg1, const void *arg2)
{
	return ((PIDInfo*)arg1)->PID - ((PIDInfo*)arg2)->PID;
}

int __cdecl PIDInfo::compareByCount(const void *arg1, const void *arg2)
{
	return ((PIDInfo*)arg1)->count - ((PIDInfo*)arg2)->count;
}
#else
int PIDInfo::compareByPID(const void *arg1, const void *arg2)
{
	return ((PIDInfo*)arg1)->PID - ((PIDInfo*)arg2)->PID;
}

int PIDInfo::compareByCount(const void *arg1, const void *arg2)
{
	return ((PIDInfo*)arg1)->count - ((PIDInfo*)arg2)->count;
}
#endif

void PIDsInfo::SortByPID()
{
	qsort(_PIDInfo, _countPID, sizeof(PIDInfo), PIDInfo::compareByPID);
}

void PIDsInfo::SortByCount()
{
	qsort(_PIDInfo, _countPID, sizeof(PIDInfo), PIDInfo::compareByCount);
}

int PIDsInfo::GetTotalCount() const
{
	int sum = 0;
	for (int i = 0; i < _countPID; i++)
		sum += _PIDInfo[i].count;
	return sum;
}

// PAT
//==============================================================================


int PAT::GetProgramNumber(bit13 PID)
{
	program_association_item* item = program_association_items;
	while (item)
	{
		if (item->program_map_PID == PID)
			return item->program_number;
		item = (program_association_item*) item->more;
	}
	return -1;
}

int PAT::GetProgramPID(int pn)
{
	program_association_item* item = program_association_items;
	while (item)
	{
		if (item->program_number == pn)
			return item->program_map_PID;
		item = (program_association_item*) item->more;
	}
	return -1;
}

int PAT::GetProgramCount()
{
	int c = program_association_items ? program_association_items->GetCount() : 0;
	if (c)
	{
		if (GetNetworkPID() > 0)
			c--;
	}
	return c;
}


bool PAT::AddProgram(bit16 pn, bit13 PID)
{
	program_association_item*& item = program_association_items;
	while (item)
	{
		if (item->program_map_PID == PID)
			return false; //already exist
		item = (program_association_item*)((item->more));
	}

	program_association_item* newItem = new program_association_item();
	newItem->program_number = pn;
	newItem->program_map_PID = PID;
	item = newItem;
	return true;
}


//MGT
//==============================================================================

int MGT::GetEITNumber(bit13 PID)
{
	master_guide_item* item = m_pMaster_guide_items;
	while (item)
	{
		if (item->table_type_PID == PID)
		{
            //0x0100-0x017F EIT-0 to EIT-127  ATSC
			if(item->table_type >= 0x100 && item->table_type <=0x17f)
			{
				return item->table_type;
			}
		}
		item = (master_guide_item*) item->more;
	}
	return -1;
}

int MGT::GetEITPID(int en)
{
	master_guide_item* item = m_pMaster_guide_items;
	while (item)
	{
		if (item->table_type == en)
		{
			return item->table_type_PID;
		}
		item = (master_guide_item*) item->more;
	}
	return -1;
}

int MGT::GetEITCount()
{
	int c = m_pMaster_guide_items ? m_pMaster_guide_items->GetCount() : 0;
	return c;
}


bool MGT::AddEIT(bit16 en, bit13 PID)
{
	master_guide_item*& item = m_pMaster_guide_items;
	while (item)
	{
		if (item->table_type_PID == PID)
			return false; //already exist
		item = (master_guide_item*)((item->more));
	}

	master_guide_item* newItem = new master_guide_item();
	newItem->table_type = en;
	newItem->table_type_PID = PID;
	item = newItem;
	return true;
}


// PMT
//==============================================================================

TS_program_map_item* PMT::GetProgramMapItem(bit13 PID)
{
	TS_program_map_item* item = TS_program_map_items;
	while (item)
	{
		if (item->elementary_PID == PID)
			return item;
		item = item->Next();
	}
	return NULL;
}


// PESPacket
//==============================================================================

bool PESPacket::Load(uint8* pData, uint32 cbData)
{
	uint8* p = pData;
	bit24 packet_start_code_prefix;
	R24B(p, packet_start_code_prefix);
	if (packet_start_code_prefix != PESPacketStartCodePrefix)
		return false;
	head = p;
	stream_id = *p++;
	R16B(p, PES_packet_length);

	//if (stream_id == 0xE0)
	//if (stream_id == 0xC0)
	{
// 		CDumper::WriteLog("PES_packet_length = %d", PES_packet_length);
// 		uint8* pp = pData + 19;
// 		CDumper::WriteLog("PES_packet = [%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x]",
// 			pp[0], pp[1], pp[2], pp[3], pp[4], pp[5], pp[6], pp[7], pp[8], pp[9], pp[10], pp[11]);
// 		CDumper::WriteLog("\r\n");
	}


	if (stream_id == SID_ProgramStreamMap ||
		stream_id == SID_PrivateStream2 ||
		stream_id == SID_ECMStream ||
		stream_id == SID_EMMStream ||
		stream_id == SID_ProgramStreamDirectory ||
		stream_id == SID_DSMCCStream ||
		stream_id == SID_H2221E)
	{
		data = p;
		datasize = (uint16)(cbData - (uint32)(p - pData));
#if 1 // for error tolerance 
		if (datasize > cbData)
			return false;
#endif 
		payloadsize = PES_packet_length;

		//set the Max Value of PTS, something wrong
        PTS =  0x1FFFFFFFFLL;
		DTS =  0x1FFFFFFFFLL;
		return true;
	}
	else if (stream_id == SID_PaddingStream)
	{
		data = p;
		datasize = (uint16)(cbData - (uint32)(p - pData));
#if 1 // for error tolerance 
		if (datasize > cbData)
			return false;
#endif
		payloadsize = 0;
		return true;
	}

	// 20100122 前面有'10'两位为调整字段
	PES_scrambling_control = R2B5(*p);
	// end

	PES_priority = R1B3(*p);
	data_alignment_indicator = R1B2(*p);
	copyright = R1B1(*p);
	original_or_copy = R1B0(*p++);
	PTS_DTS_flags = R2B7(*p);
	ESCR_flag = R1B5(*p);
	ES_rate_flag = R1B4(*p);
	DSM_trick_mode_flag = R1B3(*p);
	additional_copy_info_flag = R1B2(*p);
	PES_CRC_flag = R1B1(*p);
	PES_extension_flag = R1B0(*p++);
	PES_header_data_length = *p++;
	if (PES_header_data_length > cbData)
		return false;
	uint8* pmark = p; //bookmark

	if (PTS_DTS_flags & 0x02)
		R33B(p, PTS);
	if (PTS_DTS_flags & 0x01)
		R33B(p, DTS);
	if (ESCR_flag)
	{
		R33n10B(p, ESCR_base, ESCR_extension);
	}
	if (ES_rate_flag)
	{
		uint32 ES_rate;
		R22B(p, ES_rate);
	}
	if (DSM_trick_mode_flag)
	{
		p++; //TODO
	}
	if (additional_copy_info_flag)
	{
		p++; //TODO
	}
	if (PES_CRC_flag)
	{
		p += 2; //TODO
	}

	if (PES_extension_flag)
	{
		bit1 PES_private_data_flag = R1B7(*p);
		bit1 pack_header_field_flag = R1B6(*p);
		bit1 program_packet_sequence_counter_flag = R1B5(*p);
		bit1 PSTD_buffer_flag = R1B4(*p);
		bit1 PES_extension_flag_2 = R1B0(*p++);
		if (PES_private_data_flag)
		{
			p += 8; //TODO
		}
		if (pack_header_field_flag)
		{
			uint8 pack_field_length = *p++;
			p += pack_field_length; //TODO
		}
		if (program_packet_sequence_counter_flag)
		{
			p += 2; //TODO
		}
		if (PSTD_buffer_flag)
		{
			p+= 2; //TODO
		}
		if (PES_extension_flag_2)
		{
			uint8 PES_extension_field_length = R7B6(*p++);
			p += PES_extension_field_length;
		}
	}

	int stuffing_len = PES_header_data_length;
	stuffing_len -= (int)(p - pmark);
	if (stuffing_len < 0)
		return false;
	p += stuffing_len;

	data = p;
	datasize = (uint16)(cbData - (p - pData));
#if 1 // for error tolerance 
		if (datasize > cbData)
			return false;
#endif 
	if (PES_packet_length > 0)
		payloadsize = PES_packet_length - (uint16) (p - pData - 6);
	else
		payloadsize = 0; //???

#if 1

#endif

	return true;

}





//**********************************************************************************


#include "tssi.spec.h"
#include "tsdesc.spec.h"

void CodePageSTR2TSTR(const char* src, uint16 codepage, VO_TCHAR* dst, int size)
{
#ifdef LINUX
	memcpy(dst, src, size);
#elif defined(_IOS) || defined(_MAC_OS)
	memcpy(dst, src, size);
#else
	#ifdef _UNICODE
		MultiByteToWideChar(codepage, 0, src, -1, dst, size); 
	#else
		WCHAR wbuf[1024];
		int len = MultiByteToWideChar(codepage, 0, src, -1, wbuf, sizeof(wbuf)/sizeof(WCHAR)); 
		if (len > 0)
			WideCharToMultiByte(CP_ACP, 0, wbuf, len, dst, size, NULL, NULL);
		else
			strcpy(dst, src);
	#endif

#endif
}

inline void STR2TSTR(VO_TCHAR* dst, const char* src, int size)
{
#ifdef LINUX
	CodePageSTR2TSTR(src, 0, dst, size);
#elif defined(_IOS)|| defined(_MAC_OS)
	CodePageSTR2TSTR(src, 0, dst, size);
#else
	CodePageSTR2TSTR(src, CP_UTF8, dst, size);
#endif
}

// EventItem
//==============================================================================

EventItem::EventItem()
: event_name(NULL)
, text(NULL)
{
}

EventItem::~EventItem()
{
	if (event_name)
	{
		delete[] event_name;
		event_name = NULL;
	}
	if (text)
	{
		delete[] text;
		text = NULL;
	}
}

bool EventItem::Import(event_information_item* item)
{
	event_id = item->event_id;
	start_time = item->start_time;
	duration = item->duration;
	if (item->descriptors == NULL)
		return false;
	short_event_descriptor* desc = (short_event_descriptor*)item->descriptors->FindFirst(0x4D);
	if (desc == NULL)
		return false;
	if (desc->event_name_length)
	{
		event_name = new char[desc->event_name_length + 1];
		memcpy(event_name, desc->event_name, desc->event_name_length);
		event_name[desc->event_name_length] = 0;
	}
	if (desc->text_length)
	{
		text = new char[desc->text_length + 1];
		memcpy(text, desc->text, desc->text_length);
		text[desc->text_length] = 0;
	}
	return true;
}

void EventItem::ExportEventName(VO_TCHAR* buffer, int size)
{
	if (event_name)
		STR2TSTR(buffer, event_name, size);
	else
		_stprintf(buffer, _T("Event%d"), event_id);
}

void EventItem::ExportEventDescription(VO_TCHAR* buffer, int size)
{
	if (text)
		STR2TSTR(buffer, text, size);
	else
		_stprintf(buffer, _T("EventDescription%d"), event_id);
	
}

// EventInfo
//==============================================================================

EventInfo::EventInfo()
: count(0)
, items(NULL)
, refEIT(0)
{
}

EventInfo::~EventInfo()
{
	if (items)
	{
		delete[] items;
		items = NULL;
	}
}

int EventInfo::Import(EIT* eit)
{
	if (ImportedEIT())
		return ++refEIT;
	event_information_item* node = eit->event_information_items;
	if (node == NULL)
		return refEIT;
	count = node->GetCount();
	if (count > 0)
	{
		EventItem* p = items = new EventItem[count];
		while (node)
		{
			p->Import(node);
			node = node->Next();
			p++;
		}
	}
	return ++refEIT;
}


// ElementInfo
//==============================================================================

ElementInfo::ElementInfo()
: stream_type(0)
, elementary_PID(0x1fff)
, descriptors(NULL)
, extension(NULL)
, root_descriptor(NULL)
, ESD(NULL)
, DCD(NULL)
, SLCD(NULL)

{
    memset(chLanguage, 0, 16);
}

ElementInfo::~ElementInfo()
{
	SetRootDescriptor(NULL);
	if (descriptors)
	{
		delete descriptors;
		descriptors = NULL;
	}
	if (extension)
	{
		delete extension;
		extension = NULL;
#ifdef _DEBUG
		OutputDebugString(_T("ElementInfo.extension deleted\n"));
#endif //_DEBUG
	}
}

bool ElementInfo::Import(TS_program_map_item* item)
{
	stream_type = item->stream_type;
	elementary_PID = item->elementary_PID;
#ifdef _DEBUG
	if (descriptors) //should not happen
		return false;
#endif //_DEBUG
	if (descriptors)
		delete descriptors;
	descriptors = item->descriptors;
	item->descriptors = NULL; //!!! ElementInfo will release the memory, so modify it
	return true;
}

void ElementInfo::SetRootDescriptor(MP4::Descriptor* d) 
{ 
	if (root_descriptor)
		delete root_descriptor;
	root_descriptor = d;
}

void ElementInfo::SetESD(MP4::ESDescriptor* esd)
{
	ESD = esd;
	DCD = (MP4::DecoderConfigDescriptor*) esd->GetChildByTag(MP4::DecoderConfigDescrTag);
	SLCD = (MP4::SLConfigDescriptor*) esd->GetChildByTag(MP4::SLConfigDescrTag);
}

MP4::DecoderSpecificInfo* ElementInfo::GetDSI()
{
	MP4::DecoderConfigDescriptor* dcd = GetDCD();
	if (dcd)
		return (MP4::DecoderSpecificInfo*) dcd->GetChildByTag(MP4::DecSpecificInfoTag);
	return NULL;
}

void ElementInfo::SetProgram(struct ProgramInfo* program)
{
	bit16    ES_ID_value = 0;
	SL_descriptor* sld = GetSLD();
	FMC_descriptor* pFMC = NULL;

	if (sld == NULL )
	{
		//current only support one ES_ID
		pFMC = GetFMC();
		if((pFMC == NULL) || (pFMC->m_iFMCItemCount != 1))
		{
		   return;
		}
		ES_ID_value = pFMC->m_pES_IDs[0];
	}
	else
	{
		ES_ID_value = sld->ES_ID;
	}

	MP4::InitialObjectDescriptor* iod = program->GetIOD();
	if (iod == NULL)
		return;

	MP4::Descriptor* d = iod->FirstChild();
	while (d)
	{
		if (d->GetTag() == MP4::ES_DescrTag)
		{
			MP4::ESDescriptor* temp = (MP4::ESDescriptor*)d;
			if (temp->ES_ID == ES_ID_value)
			{
				SetESD(temp);
				break;
			}
		}			
		d = iod->NextChild(d);
	}
}


bool ElementInfo::IsPlaybackable()
{
	descriptor*    pDesc = NULL;
	ISO_639_language_descriptor*    pLanguageDesc = NULL;
	ISO_639_language_item*          pLanguageItem = NULL;

    //Add the Language Desc
	if(descriptors != NULL)
	{
		pDesc = descriptors->FindFirst(0x0A);
		if(pDesc != NULL)
		{
			pLanguageDesc = (ISO_639_language_descriptor*)pDesc;
			pLanguageItem = pLanguageDesc->ISO_639_language_items;
			if(pLanguageItem != NULL)
			{
				chLanguage[0] = *((char*)(&(pLanguageItem->ISO_639_language_code))+2);
                chLanguage[1] = *((char*)(&(pLanguageItem->ISO_639_language_code))+1);
                chLanguage[2] = *((char*)(&(pLanguageItem->ISO_639_language_code))+0);
			}
		}
	}



	MP4::DecoderConfigDescriptor* dcd = GetDCD();
	if (dcd)
	{
		switch (dcd->objectTypeIndication)
		{
		case 0x21: //AVC
		case 0x40: //AAC
			return true;

		default:
			break;
		}
	}

	switch (stream_type)
	{
	case ST_111722Video: //MPEG2
	case ST_H262Video: //MPEG2
	case ST_111723Audio: //MP3
	case ST_138183Audio: //MP3
	case ST_H264:  //H264
	case ST_HEVC:  //HEVC
    case ST_HEVC_IEC_23008_2:  //HEVC IEC_23008_2
	case ST_138187AudioWithADTS: //AAC
	case ST_144963AudioWithLATM: //LATM-AAC
	case ST_144961PES: //T-DMB
	case ST_144962Visual: // MPEG4
	case ST_14496Section:		//MPEG4
		return true;

	case ST_META_DATA_IN_PES:
		{
			if(descriptors && descriptors->FindFirst(0x26))
			{
				metadata_descriptor *meta_des = (metadata_descriptor *)descriptors;
				//the metadata_application_format_identifier is ID3
				if (meta_des->GetMetaDataFormatIdentifier() == 0x49443320) //
				{
					return true;
				}	
			}
		}
	case ST_H2220_138181_PESPackets:
		if (descriptors && descriptors->FindFirst(0x6A)) //AC3
		{	
		    return true;
		}

		if(descriptors && descriptors->FindFirst(0x59))
		{
			return true;
		}
		else
		{
            if(descriptors && descriptors->FindFirst(0x05))
			{
			    registration_descriptor *res_des = (registration_descriptor *)descriptors;
			    if (res_des->GetFormatType() == 0x44545332)
			    {
				    return true;
			    }	
			}

			if(descriptors && descriptors->FindFirst(0x7a))
			{
				//Find the EAC3 Desc
                return true;
			}
		}
		break;
	case ST_UserPrivateAC3:///<for system A(ATSC)use the 0x81 to uniquely identify the AC-3,the descriptor may be not exist.
		{
			return true;
			if (descriptors && descriptors->FindFirst(0x05))
			{
				registration_descriptor *res_des = (registration_descriptor *)descriptors;
				if (res_des->GetFormatType() == 0x41432D33)
				{
					return true;
				}	
			}

			if (descriptors && descriptors->FindFirst(0x6A))
			{
				return true;
			}
		}
		break;
	case ST_UserPrivateMin:
		if (descriptors &&descriptors->FindFirst(0x05))
		{
			registration_descriptor *res_des = (registration_descriptor *)descriptors;
			if (res_des->GetFormatType() == 0x48444d56)
			{
				return true;
			}	
		}
		break;
	//Add for DTS
	case ST_HDMV_DTS_Audio:
		{
			return true;
		}
	//Add for DTS
	case ST_AUDIO_DTS_HD:
		{
			return true;
		}
	case ST_UserPrivateEAC3:
		{
			return true;
		}
	default:
		break;
	}


	return false;
}


#if 0

MP4::ESDescriptor* ElementInfo::GetESD()
{
	SL_descriptor* sld = GetSLD();
	if (sld == NULL)
		return NULL;
	MP4::InitialObjectDescriptor* iod = program->GetIOD();
	if (iod == NULL)
		return NULL;

	MP4::Descriptor* d = iod->FirstChild();
	while (d)
	{
		if (d->GetTag() == MP4::ES_DescrTag)
		{
			MP4::ESDescriptor* temp = (MP4::ESDescriptor*)d;
			if (temp->ES_ID == sld->ES_ID)
				return temp;
		}			
		d = iod->NextChild(d);
	}
	return NULL;
}

MP4::DecoderConfigDescriptor* ElementInfo::GetDCD()
{
	MP4::ESDescriptor* esd = GetESD();
	if (esd)
		return (MP4::DecoderConfigDescriptor*) esd->GetChildByTag(MP4::DecoderConfigDescrTag);
	return NULL;
}

MP4::SLConfigDescriptor* ElementInfo::GetSLCD()
{
	MP4::ESDescriptor* esd = GetESD();
	if (esd)
		return (MP4::SLConfigDescriptor*) esd->GetChildByTag(MP4::SLConfigDescrTag);
	return NULL;
}


#endif

// ProgramInfo
//==============================================================================

ProgramInfo::ProgramInfo()
: program_number(0)
, program_map_PID(0)
, descriptors(NULL)
, element_count(0)
, elements(NULL)
, event_count(0)
, events(NULL)
, service_provider_name(NULL)
, service_name(NULL)
, refPMT(0)
, refSDT(0)
, refEIT(0)
{
}

ProgramInfo::~ProgramInfo()
{
	if (descriptors)
	{
		delete descriptors;
		descriptors = NULL;
	}
	if (events)
	{
		delete[] events;
		events = NULL;
	}
	if (elements)
	{
		delete[] elements;
		elements = NULL;
	}
	if (service_provider_name)
	{
		delete[] service_provider_name;
		service_provider_name = NULL;
	}
	if (service_name)
	{
		delete[] service_name;
		service_name = NULL;
	}
}

int ProgramInfo::Import(PMT* pmt)
{
	TS_program_map_item* item = pmt->TS_program_map_items;
	if (item == NULL)
	{
		CDumper::WriteLog((char *)"PMT error!");
		return refPMT; 
	}
	if (ImportedPMT())
		return ++refPMT;
	element_count = item->GetCount();
	if (element_count == 0)
	{
		CDumper::WriteLog((char *)"No element in program!");
		return ++refPMT;
	}
	if (descriptors)
		delete descriptors;
	descriptors = pmt->descriptors;
	pmt->descriptors = NULL;

	elements = new ElementInfo[element_count];
	ElementInfo* p = elements;
	while (item)
	{
		p->Import(item);
		item = item->Next();
		p->SetProgram(this);
		p++;
	}
	return ++refPMT;
}

int ProgramInfo::Import(service_description_item* item)
{
	if (item->descriptors == NULL)
		return refSDT;
	if (ImportedSDT())
		return ++refSDT;
	service_descriptor* desc = (service_descriptor*) item->descriptors->FindFirst(0x48);
	if (desc == NULL)
		return refSDT;
	if (desc->service_provider_name_length)
	{
		service_provider_name = new char[desc->service_provider_name_length + 1];
		memcpy(service_provider_name, desc->service_provider_name, desc->service_provider_name_length);
		service_provider_name[desc->service_provider_name_length] = 0;
	}
	if (desc->service_name_length)
	{
		service_name = new char[desc->service_name_length + 1];
		memcpy(service_name, desc->service_name, desc->service_name_length);
		service_name[desc->service_name_length] = 0;
	}
	return ++refSDT;
}

int ProgramInfo::Import(EIT* eit)
{
	if (event_count == 0)
	{
		event_count = eit->last_section_number + 1;
		//if (event_count)
			events = new EventInfo[event_count];
	}
	if (eit->section_number <= event_count)
		return events[eit->section_number].Import(eit);
	return 0;
}

bool ProgramInfo::ImportedEIT()
{
	if (event_count == 0)
		return false;
	for (int i = 0; i < event_count; i++)
	{
		if (!events[i].ImportedEIT())
			return false;
	}
	return true;
}

#if 0 //090520, some files has too many PIDs for one program

int ProgramInfo::ExportElementPIDs(int* pids, int size)
{
	int total = element_count;
	if (size < total)
		total = size;
	for (int i = 0; i < total; i++)
		pids[i] = elements[i].elementary_PID;
	return total;
}

#else

int ProgramInfo::ExportElementPIDs(uint32* pids, int size)
{
	int count = 0;
	for (int i = 0; i < element_count; i++)
		if (elements[i].IsPlaybackable())  //only send playbackable PID
			pids[count++] = elements[i].elementary_PID;
	return count;
}

#endif


int ProgramInfo::ExportVideoElementPIDs(uint32* pids, int size)
{
	int count = 0;
	for (int i = 0; i < element_count; i++)
		if (elements[i].IsPlaybackable() && 
			(elements[i].stream_type ==  ST_111722Video ||
			 elements[i].stream_type ==  ST_H262Video ||
			 elements[i].stream_type ==  ST_H264 ||
			 elements[i].stream_type ==  ST_144962Visual))
		    pids[count++] = elements[i].elementary_PID;
	return count;
}



int ProgramInfo::ExportElementPIDs(bit13* pids, int size)
{
	if (size < element_count)
		return 0;
	for (int i = 0; i < element_count; i++)
		pids[i] = elements[i].elementary_PID;
	return element_count;
}

void ProgramInfo::ExportServiceName(VO_TCHAR* buffer, int size)
{
	if (service_name)
		STR2TSTR(buffer, service_name, size);
	else
		_stprintf(buffer, _T("PGM%d"), program_number);
}

int ProgramInfo::GetEventTotal()
{
	int total = 0;
	for (int i = 0; i < event_count; i++)
		total += events[i].count;
	return total;
}

ElementInfo* ProgramInfo::GetElementInfoByPID(bit13 pid)
{
	ElementInfo* p = elements;
	for (int i = 0; i < element_count; i++)
	{
		if (pid == p->elementary_PID)
			return p;
		++p;
	}
	return NULL;
}

ElementInfo* ProgramInfo::GetElementInfoByESID(uint16 esid)
{
	ElementInfo* p = elements;
	for (int i = 0; i < element_count; i++)
	{
		SL_descriptor* sld = p->GetSLD();
		if (sld && (esid == sld->ES_ID))
			return p;
		++p;
	}
	return NULL;
}


MP4::InitialObjectDescriptor* ProgramInfo::GetIOD()
{
	if (descriptors == NULL)
		return NULL;
	TS::descriptor* desc = descriptors->FindFirst(29); //IOD_descriptor
	if (desc == NULL)
		return NULL;
	return ((IOD_descriptor*)desc)->IOD;
}

int ProgramInfo::GetPlaybackableElementCount()
{
	int count = 0;
	ElementInfo* p = elements;
	for (int i = 0; i < element_count; i++)
	{
		if (p && p->IsPlaybackable())
			++count;
		++p;
	}
	return count;
}


// TransportStreamInfo
//==============================================================================


TransportStreamInfo::TransportStreamInfo()
: program_count(0)
, programs(NULL)
{
}

TransportStreamInfo::~TransportStreamInfo()
{
	program_count = 0;

	if (programs)
	{
		delete[] programs;
		programs = NULL;
	}
}

void TransportStreamInfo::Reset()
{
	program_count = 0;

	if (programs)
	{
		delete[] programs;
		programs = NULL;
	}
}

void TransportStreamInfo::Reset2()
{
	ProgramInfo* info = programs;

	for (int n=0; n<program_count; n++)
	{
		if(info)
		{
			ElementInfo* e = info->elements;

			for (int m=0; m<info->element_count; m++)
			{
				if (e)
				{
					if(e->extension)
						e->extension->Reset();
				}

				e++;
			}
		}

		info++;
	}
}

ProgramInfo* TransportStreamInfo::FindProgram(bit16 id)
{
	//CDumper::WriteLog("Enter TransportStreamInfo::FindProgram, prgram count = %d", program_count);

// 	ProgramInfo* p = programs;
// 
// 	while (p)
// 	{
// 		CDumper::WriteLog("TransportStreamInfo::FindProgram 1");
// 		if(p)
// 		{
// 			CDumper::WriteLog("TransportStreamInfo::FindProgram 2");
// 			if (p->program_number == id)
// 			{
// 				CDumper::WriteLog("Leave TransportStreamInfo::FindProgram, Find it");
// 				return p;
// 			}
// 				
// 		}
// 
// 		CDumper::WriteLog("TransportStreamInfo::FindProgram 3");
// 		p++;
// 		CDumper::WriteLog("TransportStreamInfo::FindProgram 4");
// 	}
// 
// 	CDumper::WriteLog("Leave TransportStreamInfo::FindProgram");
// 	return NULL;

	for (int i = 0; i < program_count; i++)
	{
		if(programs)
		{
			if (programs[i].program_number == id)
			{
				//CDumper::WriteLog("Leave TransportStreamInfo::FindProgram, Find it");
				return programs + i;
			}
		}
	}

	//CDumper::WriteLog("Leave TransportStreamInfo::FindProgram");
	return NULL;
}

bool TransportStreamInfo::ImportedPMT()
{
	if (program_count == 0)
		return false;
	for (int i = 0; i < program_count; i++)
	{
		if (!programs[i].ImportedPMT())
			return false;
	}
	return true;
}

bool TransportStreamInfo::ImportedPMT2()
{
	if (program_count == 0)
		return false;
	for (int i = 0; i < program_count; i++)
	{
		if(programs[i].GetPlaybackableElementCount() == 0)
			continue;

		if (!programs[i].ImportedPMT())
			return false;
	}
	return true;
}

bool TransportStreamInfo::ImportedSDT()
{
	if (program_count == 0)
		return false;
	for (int i = 0; i < program_count; i++)
	{
		if (!programs[i].ImportedSDT())
			return false;
	}
	return true;
}

bool TransportStreamInfo::ImportedSDT2()
{
	if (program_count == 0)
		return false;
	for (int i = 0; i < program_count; i++)
	{
		if(programs[i].GetPlaybackableElementCount() == 0)
			continue;

		if (!programs[i].ImportedSDT())
			return false;
	}
	return true;
}

bool TransportStreamInfo::ImportedEIT()
{
	if (program_count == 0 || !programs)
		return false;
	for (int i = 0; i < program_count; i++)
	{
		if (!programs[i].ImportedEIT())
			return false;
	}
	return true;
}

bool TransportStreamInfo::ImportedEIT2()
{
	if (program_count == 0 || !programs)
		return false;
	for (int i = 0; i < program_count; i++)
	{
		if(programs[i].GetPlaybackableElementCount() == 0)
			continue;

		if (!programs[i].ImportedEIT())
			return false;
	}
	return true;
}

bool TransportStreamInfo::Import(PAT* pat)
{
	program_association_item* item = pat->program_association_items;
	if (item == NULL) // 5/21/2009
		return 0;

	if (program_count == 0) // SDT not ready
	{
		transport_stream_id = pat->transport_stream_id;
		program_count = pat->GetProgramCount();
		programs = new ProgramInfo[program_count];

		ProgramInfo* p = programs;
		while (item)
		{
			if (item->program_number) //not network PID
			{
				p->program_number = item->program_number;
				p->program_map_PID = item->program_map_PID;
				p++;
			}
			item = item->Next();
		}
	}
	else // SDT is ready
	{
		if (transport_stream_id != pat->transport_stream_id)
			return false;
		while (item)
		{
			if (item->program_number) //not network PID
			{
				ProgramInfo* p = FindProgram(item->program_number);
				if (p)
					p->program_map_PID = item->program_map_PID;
			}
			item = item->Next();
		}
	}
	return true;
}


int TransportStreamInfo::Import(PMT* pmt)
{
	for (int i = 0; i < program_count; i++)
	{
		// modified by lin 20091224
		if(programs)
		{
			if (programs[i].program_number == pmt->program_number)
				return programs[i].Import(pmt);
		}
	}
	return 0;
}


int TransportStreamInfo::Import(SDT* sdt)
{
	service_description_item* item = sdt->service_description_items;
	if (item == NULL) // 5/21/2009
		return 0;

	//CDumper::WriteLog("TransportStreamInfo::Import");

	int rc = 0;
	if (program_count == 0) //PAT not ready
	{
		//CDumper::WriteLog("TransportStreamInfo::Import - 1");

		transport_stream_id = sdt->transport_stream_id;
		program_count = item->GetCount();
		programs = new ProgramInfo[program_count];

		ProgramInfo* p = programs;
		while (item)
		{
			p->program_number = item->service_id;

			//CDumper::WriteLog("import program's number = %d", p->program_number);

			int temp = p->Import(item);
			
			//CDumper::WriteLog("finish import program info");


			if (rc < temp)
				rc = temp;
			item = item->Next();
			p++;
		}
	}
	else // PAT is ready
	{
		// tag: 20091223 removed
		// 对ISDBT,transport_stream_id被设置为1,可能不正确.
// 		if (transport_stream_id != sdt->transport_stream_id)
// 		{
// 			OutputDebugString(_T("Ignore this SDT\r\n"));
// 			return 0;
// 		}
		
		//CDumper::WriteLog("TransportStreamInfo::Import - 2");


		while (item)
		{
			// service id可以保证是这个progaram的sdt

			//CDumper::WriteLog("find program by service id = %d", item->service_id);

			ProgramInfo* p = FindProgram(item->service_id);

			//CDumper::WriteLog("finish find program");


			if (p)
			{
				//CDumper::WriteLog("import program's number = %d - 2", p->program_number);

				int temp = p->Import(item);

				//CDumper::WriteLog("finish import program info - 2");

				if (rc < temp)
					rc = temp;
			}
			item = item->Next();
		}
	}
	return rc;
}


int TransportStreamInfo::Import(EIT* eit)
{
	// tag: 20091224 removed
// 	if (transport_stream_id != eit->transport_stream_id)
// 		return 0;

	ProgramInfo* p = FindProgram(eit->service_id);
	if (p)
		return p->Import(eit);
	else
		CDumper::WriteLog((char *)"No program found so far");

	return 0;
}


int TransportStreamInfo::ExportProgramMapPIDs(int* pids, int size)
{
	if (size < program_count)
		return -1;
	for (int i = 0; i < program_count; i++)
		pids[i] = programs[i].program_map_PID;
	return program_count;
}




