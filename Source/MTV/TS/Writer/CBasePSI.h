#pragma once
#include "voType.h"
#include "vobstype.h"
#include "fMacros.h"
#include "CTsPacket.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

enum Pmt_Descriptor_Type
{
	Pmt_Descriptor_Type_Program = 0,
	Pmt_Descriptor_Type_Stream
};

struct CTsPacketHeader
{
	bit8		sync_byte;
	bit1		transport_error_indicator;
	bit1		payload_unit_start_indicator;
	bit1		transport_priority;
	bit13		PID;
	bit2		transport_scarambling_control;
	bit2		adaptation_field_control;
	bit4		continuity_counter;

	uint16		GetHeaderSize(){return 4;};
};

struct CTableHeader
{
	bit8		table_id;
	bit1		section_synctax_indicator;
	bit1		unknown;
	bit2		reserved1;
	bit12		section_length;
	union
	{
		bit16		transport_stream_id;	// PAT:transport_stream_id, 
		bit16		program_number;	// PMT:program_number
	};
	
										
	bit2		reserved2;
	bit5		version_number;
	bit1		current_next_indicator;
	bit8		section_number;
	bit8		last_section_number;

	uint16		GetHeaderSize(){return 8;};
};


struct CPat : public CTableHeader
{
	VO_U8	program_count;

	bit16	program_number[32];		// max 32 program
	bit13	program_map_PID[32];
	bit3	reserved;
	bit13	network_id;

	bit32	crc_32;

	uint16	GetSectionSize(){return 4;};
	uint16	GetCrcSize(){return 4;};
};

class CDescriptor
{
public:
	CDescriptor();
	virtual ~CDescriptor();
public:
	virtual VO_U32 GetLength(){return 2;}
	virtual VO_U32 AssembleDescriptor(CTsPacket* pTsPacket)
	{
		pTsPacket->WriteBits(8, descriptor_tag);
		pTsPacket->WriteBits(8, descriptor_length);
		return 2;
	}
public:
	bit8				descriptor_tag;
	bit8				descriptor_length;
	VO_BOOL				bUsed;


	class CDescriptor * pNext;
};

class CRegistration_Descriptor :public CDescriptor
{
public:
	CRegistration_Descriptor();
	virtual ~CRegistration_Descriptor();
public:
	virtual VO_U32 GetLength(){return CDescriptor::GetLength()+ 4 + additional_info_len;}
	virtual VO_U32 AssembleDescriptor(CTsPacket* pTsPacket)
	{
		CDescriptor::AssembleDescriptor(pTsPacket);
		pTsPacket->WriteBits(32, format_identifier);
		VO_U32 nSize = 0;
		while(nSize < additional_info_len)
		{
			pTsPacket->WriteBits(8, additional_identification_info[nSize]);
			nSize ++;
		}
		return CDescriptor::GetLength() + 4 + additional_info_len;;
	}
public:
	bit32	format_identifier;///<32 bit
	VO_U32	additional_info_len;
	bit8 *	additional_identification_info;
};

#define MAX_STREAM_TYPE_COUNT 8
struct CPmt : public CTableHeader
{
	VO_U8	stream_type_count;
	
//	bit16	program_number;			// replace transport_stream_id

	bit3	reserved1;
	bit13	PCR_PID;
	bit4	reserved2;
	bit12	program_info_length;

	// descriptor
	CDescriptor * pProgramdescriptorHead;

	bit8	stream_type[MAX_STREAM_TYPE_COUNT];	// set max 8
	bit3	reserved3[MAX_STREAM_TYPE_COUNT];
	bit13	elementary_PID[MAX_STREAM_TYPE_COUNT];
	bit4	reserved4[MAX_STREAM_TYPE_COUNT];
	bit12	ES_info_length[MAX_STREAM_TYPE_COUNT];
	// descriptor
	CDescriptor * pStreamdescriptorHead[MAX_STREAM_TYPE_COUNT];
	bit32	crc_32;

	uint16	GetSectionSize(){return 4;};
	uint16	GetCrcSize(){return 4;};
	void    AddDescriptor(VO_U32 nStreamID,Pmt_Descriptor_Type eType,CDescriptor * pdescriptor);
	void	RemoveDescriptor(Pmt_Descriptor_Type eType);
	VO_VOID	Uninit(){RemoveDescriptor(Pmt_Descriptor_Type_Program);RemoveDescriptor(Pmt_Descriptor_Type_Stream);}	
};

struct CPESPacket
{
	bit24	packet_start_code_prefix;
	bit8	stream_id;
	bit16	PES_packet_length;

	bit2	PES_scrambling_control;
	bit1	PES_priority;
	bit1	data_alignment_indicator;
	bit1	copyright;
	bit1	original_or_copy;
	bit2	PTS_DTS_flags;
	bit1	ESCR_flag;
	bit1	ES_rate_flag;
	bit1	DSM_trick_mode_flag;
	bit1	additional_copy_info_flag;
	bit1	PES_CRC_flag;
	bit1	PES_extension_flag;
	bit8	PES_header_data_length;

	bit33	PTS;
	bit33	DTS;
	bit33	ESCR_base;
	bit10	ESCR_extension;

	bit1	PES_private_data_flag;
	bit1	pack_header_field_flag;
	bit1	program_packet_sequence_counter_flag;
	bit1	PSTD_buffer_flag;
	bit1	PES_extension_flag_2;

// 	uint8*	head;
// 	uint8*	data;
// 	uint16	datasize; //current valid data size in TS packet
// 	uint16	payloadsize; //whole PES payload
};

/***********************************************
************************************************
*				CRC check                  *
************************************************
************************************************/
void BuildCrcTable();
unsigned char CalcCRC8(const VO_PBYTE pbuf, VO_U32 len) ;
unsigned long CalcCRC32(const VO_PBYTE inData, VO_U32 inLen);
bool CheckCRC32(const VO_PBYTE lpIn, VO_U32 len);


#ifdef _VONAMESPACE
}
#endif
