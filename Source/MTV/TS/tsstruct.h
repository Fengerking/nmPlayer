#ifndef _TS_STRUCT_H_
#define _TS_STRUCT_H_

#include "voYYDef_TS.h"
#include "voString.h"
#include "vobstype.h"
#include "tssi.spec.h"
#include "tsdesc.spec.h"

#if defined(LINUX)
#   define __cdecl 	
#endif 


#ifdef _DEBUG
#define DBG_MSG(_x) OutputDebugString(_T("[voPasTS] ") _x _T("\r\n"))
#else //_DEBUG
#define DBG_MSG(_x)
#endif //_DEBUG


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


namespace TS{



struct PIDInfo
{
	bit13 PID;
	uint8 continuity_counter;
	uint32 count;
	uint32 countDuplicate; //duplicate packet count

	PIDInfo();
	void Reset();

#ifdef _IOS
	static int  compareByPID(const void *arg1, const void *arg2);
	static int  compareByCount(const void *arg1, const void *arg2);
#else
	static int __cdecl compareByPID(const void *arg1, const void *arg2);
	static int __cdecl compareByCount(const void *arg1, const void *arg2);
#endif
};

const int MaxTotalPID = 512;   //32

class PIDsInfo
{
public:
	PIDsInfo();
	PIDInfo* GetPIDInfo(bit13 PID);
	PIDInfo* AddPIDInfo(bit13 PID,bit4 continuity_counter);
	PIDInfo* GetAddPIDInfo(bit13 PID,bit4 continuity_counter);

	void Reset();

protected:
	PIDInfo _PIDInfo[MaxTotalPID];
	int _countPID;

public:
	int GetCount() const { return _countPID; }
	PIDInfo& Item(int i) { return _PIDInfo[i]; }
	int GetTotalCount() const;

	void SortByPID();
	void SortByCount();

};


struct RawPacket
{
public:
	uint32 index;

	bit1 transport_error_indicator;
	bit1 payload_unit_start_indicator;
	bit1 transport_priority;
	bit13 PID;
	bit2 transport_scrambling_control;
	bit2 adaptation_field_control;
	bit4 continuity_counter;

	uint8* head;
	uint8* data;
	uint32 datasize;
};

struct PESPacket
{
	uint8 stream_id;
	uint16 PES_packet_length;

	bit2 PES_scrambling_control;
	bit1 PES_priority;
	bit1 data_alignment_indicator;
	bit1 copyright;
	bit1 original_or_copy;
	bit2 PTS_DTS_flags;
	bit1 ESCR_flag;
	bit1 ES_rate_flag;
	bit1 DSM_trick_mode_flag;
	bit1 additional_copy_info_flag;
	bit1 PES_CRC_flag;
	bit1 PES_extension_flag;
	uint8 PES_header_data_length;

	bit33 PTS;
	bit33 DTS;
	bit33 ESCR_base;
	bit10 ESCR_extension;

	bit1 PES_private_data_flag;
	bit1 pack_header_field_flag;
	bit1 program_packet_sequence_counter_flag;
	bit1 PSTD_buffer_flag;
	bit1 PES_extension_flag_2;

	uint8* head;
	uint8* data;
	uint16 datasize; //current valid data size in TS packet
	uint16 payloadsize; //whole PES payload


	bool Load(uint8* pData, uint32 cbData);
};

struct PSI
{
	uint8 table_id;
	bit12 section_length;
	uint8* section_data;	
};


class PAT : public program_association_section
{
public:
	int GetProgramNumber(bit13 PID); //return -1 if not found
	int GetProgramPID(int pn); //return -1 if not found
	int GetNetworkPID() { return GetProgramPID(0); }
	int GetProgramCount();

	// construct PAT, for ISDB-T
	bool AddProgram(bit16 pn, bit13 PID); // return false if exists
};

class MGT : public master_guide_section
{
public:
    int   GetEITNumber(bit13 PID);
    int   GetEITPID(int iEitNumber);
	int   GetEITCount();
    bool  AddEIT(bit16 en, bit13  PID);
};


//typedef TS_program_map_item ESConfig;

class PMT : public TS_program_map_section
{
public:
	TS_program_map_item* GetProgramMapItem(bit13 PID);
};

class SDT : public service_description_section
{
};

class NIT : public network_information_section
{
};

class EIT : public event_information_section
{
};

class EIT_ATSC : public event_information_section_ATSC
{
};

class TDT : public time_date_section
{

};

struct EventItem
{
	bit16 event_id;
	bit40 start_time;
	bit24 duration;
	char* event_name;
	char* text;

	EventItem();
	~EventItem();

	bool Import(event_information_item* item);
	void ExportEventName(VO_TCHAR* buffer, int size);
	void ExportEventDescription(VO_TCHAR* buffer, int size);
};

struct EventInfo
{
	int count;
	EventItem* items;

	EventInfo();
	~EventInfo();

	int Import(EIT* eit);

	bool ImportedEIT() { return refEIT > 0; }

private:
	int refEIT;
};

struct ProgramInfo;

class ElementExtension
{
public:
	virtual ~ElementExtension() { }
	virtual void Reset(){};
};

struct ElementInfo
{
	bit8 stream_type;
	bit13 elementary_PID;

	descriptor* descriptors;
	ElementExtension* extension; //element specific data or class
    char   chLanguage[16];


	ElementInfo();
	~ElementInfo();
	bool Import(TS_program_map_item* item);

	TS::descriptor* GetDescriptor(bit8 tag) { return descriptors ? descriptors->FindFirst(tag) : NULL; }

	SL_descriptor* GetSLD() { return (SL_descriptor*)GetDescriptor(30); }
	FMC_descriptor* GetFMC(){ return (FMC_descriptor*)GetDescriptor(31); }

	MP4::ESDescriptor* GetESD() { return ESD; }
	MP4::DecoderConfigDescriptor* GetDCD() { return DCD; }
	MP4::SLConfigDescriptor* GetSLCD() { return SLCD; }
	MP4::DecoderSpecificInfo* GetDSI();

	void SetProgram(struct ProgramInfo* program);
	void SetRootDescriptor(MP4::Descriptor* d);
	void SetESD(MP4::ESDescriptor* esd);

	bool IsPlaybackable();

private:
	//struct ProgramInfo* program;
	MP4::Descriptor* root_descriptor; //should release it
	MP4::ESDescriptor* ESD; //it is a reference, need not release it
	MP4::DecoderConfigDescriptor* DCD;
	MP4::SLConfigDescriptor* SLCD;

};

typedef ElementInfo ESConfig;

struct ProgramInfo
{
	bit16 program_number;
	bit13 program_map_PID;
	descriptor* descriptors;

	int element_count;
	ElementInfo* elements;

	int event_count;
	EventInfo* events;

	char* service_provider_name;
	char* service_name;

	ProgramInfo();
	~ProgramInfo();

	bool ImportedPMT() const { return refPMT > 0; }
	bool ImportedSDT() const { return service_name || service_provider_name; }
	bool ImportedEIT();

	int Import(PMT* pmt);
	int Import(service_description_item* item);
	int Import(EIT* eit);

	int ExportElementPIDs(uint32* pids, int size);
	int ExportElementPIDs(bit13* pids, int size);
    int ExportVideoElementPIDs(uint32* pids, int size);
	void ExportServiceName(VO_TCHAR* buffer, int size);

	int GetEventTotal();
	ElementInfo* GetElementInfoByPID(bit13 pid);
	ElementInfo* GetElementInfoByESID(uint16 esid);

	MP4::InitialObjectDescriptor* GetIOD();

	int GetPlaybackableElementCount();

	int GetRefPMT() const { return refPMT; }
	void IncRefPMT() { ++refPMT; }
	int GetRefSDT() const { return refSDT; }
	void IncRefSDT() { ++refSDT; }

private:
	uint8 refPMT;
	uint8 refSDT;
	uint8 refEIT;
};

struct TransportStreamInfo
{
	bit16 transport_stream_id;
	int program_count;
	ProgramInfo* programs;

	ProgramInfo* FindProgram(bit16 id);

	TransportStreamInfo();
	~TransportStreamInfo();
	
	bool ImportedPMT();
	bool ImportedPMT2();
	bool ImportedSDT();
	bool ImportedSDT2();
	bool ImportedEIT();
	bool ImportedEIT2();

	bool Import(PAT* pat);
	int Import(PMT* pmt);
	int Import(SDT* sdt);
	int Import(EIT* eit);

	void Reset(); 
	void Reset2(); 

	int ExportProgramMapPIDs(int* pids, int size);
};



} //namespace TS

#ifdef _VONAMESPACE
}
#endif

#endif //_TS_STRUCT_H_
