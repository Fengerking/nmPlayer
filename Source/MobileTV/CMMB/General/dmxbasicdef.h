#ifndef _DMX_BASIC_DEF_H_
#define _DMX_BASIC_DEF_H_

// 20100226
#include "voString.h"
// end

#include "vocrstypes.h"


#define MF_START_CODE					"\x00\x00\x00\x01" 
#define MF_START_CODE_LEN				4
#define MODE2_FRAGMENT_START_CODE		0x55
#define TS0_MF_ID						0

#define MF_HEADER_CRC32_LEN				4
#define MSF_HEADER_CRC32_LEN			4
#define NIT_CRC32_LEN					4
#define CMCT_CRC32_LEN					4
#define CSCT_CRC32_LEN					4
#define EBDT_CRC32_LEN					4
#define EA_DESC_CRC32_LEN				4
#define EMEGENCY_BROAD_CRC32_LEN		4
#define MODE2_FRAGMENT_CRC8_LEN			1
#define VIDEO_SEGMENT_HEADER_CRC32_LEN	4
#define AUDIO_SEGMENT_HEADER_CRC32_LEN	4
#define DATA_SEGMENT_HEADER_CRC32_LEN	4

#define DATA_UNIT_PARAM_LEN				3
#define DATA_SEGMENT_HEADER_LEN			6

#define MAX_SUBFRAME_NUM				15
#define MAX_FREQ_NUM					15
#define MAX_NEIGHBOR_NET_NUM			15
#define MAX_MF_NUM						40
#define MAX_VIDEO_SEG_NUM				7
#define MAX_AUDIO_SEG_NUM				7
#define MAX_DATAUNIT_NUM				256
#define MAX_VIDEO_UNIT_NUM				100
#define MAX_AUDIO_UNIT_NUM				256
#define MAX_TIME_SLOT_NUM				40
#define MAX_ESG_SERVICE_DESC_COUNT		15
#define MAX_DATA_TYPE_DESC_COUNT		16
#define MAX_EA_DESC_COUNT				16
#define MAX_DATA_FRAGMENT_DESC_COUNT	128
#define MAX_PROGRAM_GUIDE_INFO			3
#define MAX_ESG_SEGMENT_PAYLOAD_LEN		65535


#define MIN_VIDEO_UNIT_PARAM_LEN		3

//复用块头最小长度
#define MIN_MODE2_FRAGMENT_HEADER_LEN	4

enum
{
	DEMUX_OK						= 0,
	DEMUX_MF_START_CODE_ERROR		= 1,
	DEMUX_MF_HEADER_FAILED			= 2,
	DEMUX_MSF_HEADER_FAILED			= 3,
	DEMUX_TS0_FAILED				= 4,
	DEMUX_FRAGMENT_STRAT_CODE_ERROR	= 5,
	DEMUX_VIDEO_FRAGMENT_ERROR		= 6,
	DEMUX_UNSUPORT_H264_NAL_TYPE	= 7,
	DEMUX_NOT_READY					= 8,

	DEMUX_NIT_CRC_ERROR				= 10,
	DEMUX_CMCT_CRC_ERROR			= 11,
	DEMUX_EBDT_CRC_ERROR			= 12,
	DEMUX_MF_HEADER_CRC_ERROR		= 13,
	DEMUX_MSF_HEADER_CRC_ERROR		= 14,
	DEMUX_FRAGMENT_HEADER_CRC_ERROR	= 15,
	DEMUX_VIDEO_SEG_HEADER_CRC_ERROR= 16,
	DEMUX_AUDIO_SEG_HEADER_CRC_ERROR= 17,
	DEMUX_DATA_SEG_HEADER_CRC_ERROR	= 18,
	DEMUX_PROGRAME_GUIDE_CRC_ERROR	= 19,
	DEMUX_ESG_SEG_HEADERCRC_ERROR	= 20,
	DEMUX_EA_DESC_CRC_ERROR			= 21,
	DEMUX_EMEGENCY_BROADCAST_CRC_ERROR	= 22,
	DEMUX_CSCT_CRC_ERROR			= 23,
	DEMUX_MF_LENTH_ERROR			= 24,
	DEMUX_MSF_LENTH_ERROR			= 25,
	DEMUX_XPE_ERROR					= 26,
	DEMUX_DESCRAMBLE_FAILED			= 27
};

typedef enum
{
	FRAME_WHOLE		= 0,
	FRAME_BEGIN		= 1,
	FRAME_MID		= 2,
	FRAME_END		= 3
}FRAMEPOS;

typedef enum
{
	ENCRYPT_CLEAR		= 0,
	ENCRYPT_DESCRAMBLE	= 1,
	ENCRYPT_SCRAMBLE	= 2,
	ENCRYPT_RESERVED	= 3
}EncryptFlag;

typedef enum
{
	ECM_DISABLE	= 0,
	ECM_ENABLE	= 1
}EcmFlag;


typedef struct
{
	BYTE    header_len;						// 8 bits
	
	WORD    version					:5;		// 5 bits
	WORD    min_version				:5;		// 5 bits 
	WORD    mf_id					:6;		// 6 bits

	BYTE    urgency_broadcast_flag	:2;		// 2 bits
	BYTE    next_frame_para_flag	:1;		// 1 bits
	BYTE	encrypt_flag			:2;		// 2 bits
	BYTE	ecm_flag				:1;		// 1 bits
	BYTE    control_table_update_flag :2;	// 2 bits

	BYTE    nit_update_sn	:4;				// 4 bits
	BYTE    cmct_update_sn	:4;				// 4 bits
	BYTE    csct_update_sn	:4;				// 4 bits
	BYTE    smct_update_sn	:4;				// 4 bits
	BYTE    ssct_update_sn	:4;				// 4 bits
	BYTE    esg_update_sn	:4;				// 4 bits

	BYTE	reserved2			:4;			// 4bits
	BYTE	multi_sub_frame_num	:4;			// 4bits

	DWORD	multi_sub_frame_len[MAX_SUBFRAME_NUM];	// 24bits*MAX_SUBFRAME_NUM

	struct NextMultiFrameParameter
	{
		BYTE     multi_frame_header_len;	
		BYTE     first_sub_frame_header_len;
		DWORD    first_sub_frame_len;
	}next_frame_parameter;					// 40 bits
}MultiplexFrameHeader;

typedef struct
{
	BYTE	header_len;			// 8 bits

	BYTE	start_time_flag		:1;
	BYTE	video_seg_flag		:1;
	BYTE	audio_seg_flag		:1;
	BYTE	data_seg_flag		:1;
	BYTE	extend_area_flag	:1;
	BYTE    encrypt_flag		:2;	
	BYTE    packet_mode			:1;

	DWORD	start_time_stamp;	// 32 bits

	DWORD	video_seg_len;		// 21 bits
	DWORD	audio_seg_len;		// 21 bits
	DWORD	data_seg_len ;		// 21 bits
	
	BYTE	video_stream_num	:3;
	BYTE	audio_stream_num	:3;
	BYTE	ecm_flag			:1;
	BYTE	reserved;			// 2 bits

	struct VideoStreamParam
	{
		BYTE algorithm_type		:3;	// 3 bits	
		BYTE bit_rate_flag		:1;	// 1 bits								
		BYTE video_display_flag	:1;	// 1 bits	                            
		BYTE resolution_flag	:1;	// 1 bits		                    	
		BYTE frame_freq_flag	:1;	// 1 bits :Hz
		BYTE reserved			:1; // 1 bits

		struct VSPExtent
		{
			__int64 bitrate		:16;		// 16 bits
			__int64 x			:6;			// 6 bits
			__int64 y			:6;			// 6 bits	
			__int64 display_priority :3;		// 3 bits
			__int64 reserved		:5;			// 5 bits
			__int64 x_resolution	:10;		// 10 bits
			__int64 y_resolution	:10;		// 10 bits
			__int64 frame_freq	:4;			// 4 bits
			__int64 reserved2	:4;			// 4 bits
		}vsp_extent;

	}video_stream_param[MAX_VIDEO_SEG_NUM];

	struct AudioStreamParam
	{
		BYTE algorithm_type		:4;
		BYTE bit_rate_flag		:1;
		BYTE sample_rate_flag	:1;	
		BYTE stream_desc_flag	:1;
		BYTE reserved			:1;

		struct ASPExtent
		{
			DWORD bitrate		:14;		
			DWORD reserved		:6;
			DWORD sample_rate	:4;
			DWORD stream_desc;				// 24 bits
		}asp_extent;
	}audio_stream_param[MAX_AUDIO_SEG_NUM];

}MultiSubFrameHeader;


typedef struct
{
	WORD	header_len;		// 12 bits, not include CRC_32 length
							// reserved 4 bits
	struct VideoUnitParam 
	{
		WORD unit_len;		// 16 bits
		BYTE frame_type		:3;		// I,B,P
		BYTE stream_sn		:3;
		BYTE frame_end_flag	:1;
		BYTE play_time_flag :1;
		WORD relative_play_time;	// 16 bits
	}video_unit_param[MAX_VIDEO_UNIT_NUM];

	//crc32	: 32 bits

	//aditional definition
	WORD    video_unit_count;
}VideoSegmentHeader;

typedef struct
{
	BYTE	unit_count;				// 8 bits

	struct AudioUnitParam
	{
		WORD unit_len;				// 16 bits
		WORD relative_play_time;	// 16 bits
		BYTE stream_sn :3;
		BYTE reserved  :5;
	}audio_unit_param[MAX_AUDIO_UNIT_NUM];

	//crc32							// 32 bits

	//aditional definition
	WORD    header_len;
}AudioSegmentHeader;

typedef struct
{
	BYTE unit_count;				// 8 bits

	struct DataUnitParam
	{
		BYTE unit_type;				// 8 bits,  '0'=ESG, '1'=Program Guide, '128-159' ECM
		WORD unit_len;				// 16 bits
		
	}data_unit_param[MAX_DATAUNIT_NUM];

	// crc32						// 32 bits

	//aditional definition
	WORD    header_len;
}DataSegmentHeader;


typedef struct 
{
	DWORD start_code	:8;
	DWORD start_flag	:1;
	DWORD end_flag		:1;
	DWORD fragment_type	:2;		// audio or video or data?
	DWORD payload_len	:12;
	DWORD data_unit_type :8;	// which ESG data?
	BYTE  crc8;					// 8 bits

	//aditional definition
	BYTE  header_len;			// not include crc8, in byte
}Mode2FragmentHeader;

typedef enum
{
	AUDIO_DRA		= 0,
	AUDIO_HE_AAC	= 1,
	AUDIO_AAC		= 2
}AudioAlgorithmType;

typedef enum
{
	VIDEO_AVS		= 0,
	VIDEO_H264		= 1,
}VideoAlgorithmType;

enum
{
	VIDEO_UNIT		= 0,	// 00
	AUDIO_UNIT		= 1,	// 01
	DATA_UNIT		= 2,	// 10
	RESERVED_UNIT	= 3,	// 11
	UNKNOWN_UNIT	= 255
};

// control table id
enum
{
	CTROL_TABLE_NIT			= 0x01,
	CTROL_TABLE_CMCT		= 0x02,
	CTROL_TABLE_CSCT		= 0x03,
	CTROL_TABLE_SMCT		= 0x04,
	CTROL_TABLE_SSCT		= 0x05,
	CTROL_TABLE_ESG			= 0x06,
	CTROL_TABLE_CA			= 0x07,
	CTROL_TABLE_EMERGENCY	= 0x10
};

enum
{
	PACKET_MODE_1	= 1,
	PACKET_MODE_2	= 0
};

typedef struct
{
	BYTE    table_id;			// 8 bits
	
	BYTE    nit_update_sn :4;	// 4 bits
	BYTE	reserved	  :4;	// 4 bits

	WORD	mjd_date;			// total 40 bits
	BYTE	hour;
	BYTE	minute;
	BYTE	second;

	BYTE	country_code[3];	// 24 bits	
	BYTE	net_level;			// 4 bits

	WORD    net_id;				// 12 bits
	BYTE    net_name_len;		// 8 bits
	char    net_name[256];

	BYTE    freq_sn;			// 8 bits
	DWORD   center_freq;		// 32 bits
	BYTE    band_width			:4;
	BYTE    net_other_freq_num	:4;

	struct NetOtherFreq
	{
		BYTE    freq_sn;		// 8 bits
		DWORD   center_freq;	// 32 bits
		BYTE    band_width;		// 4 bits
	}net_other_freq[MAX_FREQ_NUM];
	
	BYTE    near_net_num		:4;
	BYTE	reserved2			:4;

	struct NeighborNet
	{
		WORD   neighbor_net_level :4;
		WORD   neighbor_net_id	  :12;

		BYTE   neighbor_freq_sn;	// 8 bits
		DWORD  neighbor_center_freq;// 32 bits
		BYTE   neighbor_band_width :4;
		BYTE   reserved			   :4;
	}neighbor_net[MAX_NEIGHBOR_NET_NUM];

	//DWORD crc32;					// 32 bits
}NetworkInfoTable;

//


typedef struct
{
	BYTE			table_id;				// 8 bits	
	BYTE			freq;					// 8 bits

	WORD			multi_table_update_sn	:4;
	WORD			reserved				:6;
	WORD			mf_num					:6;

	struct MFInfo
	{
		BYTE	mf_id			:6;
		BYTE	rs				:2;

		WORD	byte_interlace	:2;
		WORD	ldpc			:2;
		WORD	mode			:2;
		WORD	reserved		:1;
		WORD	scramble_code	:3;
		WORD	ts_num			:6;

		BYTE	ts_sn[MAX_TIME_SLOT_NUM];

		BYTE	reserved2		:4;
		BYTE	msf_num			:4;

		struct MSFInfo
		{
			BYTE	msf_sn		:4;
			BYTE	reserved	:4;
			WORD	service_id;		// 16 bits
		}msf_info[MAX_SUBFRAME_NUM];
	}mf_info[MAX_MF_NUM];

}ContinueServiceMultiControlTable;

typedef struct _tagShortServiceMultiControlTable : public ContinueServiceMultiControlTable
{

}ShortServiceMultiControlTable;

typedef struct  
{
	DWORD			table_id			:8;		
	DWORD			esg_update_sn		:4;		
	DWORD			segment_len			:12;	
	DWORD			segment_sn			:4;
	DWORD			segment_count		:4;		
	DWORD			network_level		:4;		
	DWORD			network_id			:12;
	DWORD			reserved			:2;
	DWORD			local_time_offset	:6;		
	DWORD			char_type			:4;		
	DWORD			esg_service_count	:4;		

	struct EsgServiceDesc
	{
		BYTE		reserved;				
		BYTE		esg_index_id;			
		WORD		esg_service_id;				//important					
	}esg_service_desc[MAX_ESG_SERVICE_DESC_COUNT];

	BYTE			reserved2			:4;		//ESG Service Count ????
	BYTE			data_type_count		:4;

	struct EsgDataTypeDesc
	{
		BYTE data_type_id;						//data type id :4 bits
		BYTE fragment_count;					//data fragment count : 8 bits
		struct DataFragment						//data unit description
		{
			WORD fragment_id			:8;		//8 bits
			WORD fragment_version		:4;		//4 bits
			WORD esg_service_index_id	:4;		//4 bits
		}data_fragment[MAX_DATA_FRAGMENT_DESC_COUNT]; // count = 2^4, see SPEC
	}data_type_desc[MAX_DATA_TYPE_DESC_COUNT];	  // count = 2^4, see data_type_count's bits count

}ESgBasicDescription;

typedef struct 
{
	bool finished;
	BYTE curr_data_fragment_count;
	BYTE data_fragment_id[MAX_DATA_FRAGMENT_DESC_COUNT];
	BYTE max_fragment_id;
}EsgParseState;

typedef struct
{
	BYTE	table_id;			// 8 bits
	WORD	segment_len;		// 16 bits
	
	DWORD	reserved		:20;
	DWORD	ea_update_sn	:4;
	DWORD	segment_sn		:8;

	BYTE	segment_count;		// 8 bits

	struct EADataDesc
	{
		WORD	ca_id;				// 16 bits
		WORD	service_id;			// 16 bits
		BYTE	emm_data_type;		//  8 bits
		BYTE	ecm_data_type;		//  8 bits
		BYTE	ecm_transfer_type;	//  2 bits
		
		// reserved 32 bits
	}ea_data_desc[MAX_EA_DESC_COUNT];

	// crc 32 bits

	// additional definition
	BYTE*	table_data_ptr;
	WORD	table_data_len;

}EncryptionAuthorizationDescTable;

typedef struct
{
	BYTE	table_id;				// 8 bits
	WORD	segment_len;			// 16 bits
	BYTE	segment_sn;				// 8 bits
	BYTE	segment_count;			// 8 bits
	
	BYTE	service_update_sn	:4;
	BYTE	reserved			:4;

	WORD	service_count;			// 16 bits

	struct ServiceInfo
	{
		WORD	service_id;			// 16 bits
		BYTE	freq;				// 8 bits
	}service_info[16];

	// crc 32 bits
	
}ContinueServiceControlTable;

typedef struct _tagShortServiceControlTable : public ContinueServiceControlTable
{

}ShortServiceControlTable;

typedef struct
{
	BYTE	table_id;				// 8 bits

	BYTE	msg_count		:4;
	BYTE	reserved		:2;
	BYTE	broadcast_sn	:2;

	WORD	broadcast_data_len;		// 16 bits
	BYTE*	broadcast_data;

	struct EbDataSegment 
	{
		BYTE	version			:4;
		BYTE	mini_version	:4;
		
		WORD	net_level		:4;
		WORD	net_id			:12;

		WORD	msg_id;

		WORD	curr_segment_sn	:8;
		WORD	last_segment_sn	:8;

		WORD	reseved			:3;
		WORD	eb_data_len		:13;
		BYTE*	eb_data;

		BYTE	trigger_flag;			// 1 bit

		// trigger_flag == 0
		struct EbMsg0 
		{	
			BYTE	trigger_type;			// 7 bits
			BYTE	eb_level		:3;
			BYTE	eb_charset		:3;
			BYTE	reserved1		:2;

			__int64	eb_start_time	:40;
			__int64	eb_duration		:20;
			__int64	text_data_count	:4;		// language 

			struct TextData
			{
				DWORD	language_type;	// 24 bits
				
				WORD	organization_flag	:1;
				WORD	ref_service_flag	:1;
				WORD	reserved1			:2;
				WORD	text_data_len		:12;

				BYTE*	text_data_ptr;

				BYTE	organization_name_len;	// 8 bits
				BYTE*	organization_name_ptr;	// 0 ~ 255

				WORD	ref_service_id;			// 16 bits

				BYTE	reserved2	:4;
				BYTE	aux_data_idx:4;
			}text_data[15];

			//
			BYTE	reserved2		:4;
			BYTE	aux_data_count	:4;
			
			struct AuxData
			{
				BYTE	aux_data_type;	// 8 bits
				WORD	aux_data_len;	// 16 bits
				BYTE*	aux_data_ptr;
			}aux_data[15];
		}eb_msg0;

		// trigger_flag == 1
		struct EbMsg1
		{
			BYTE	trigger_type;			// 7 bits

			BYTE	trigger_msg_level	:3;
			BYTE	reserved1			:5;

			WORD	trigger_net_level	:4;
			WORD	trigger_network_sn	:12;

			BYTE	trigger_freq_point;
			DWORD	trigger_center_freq;

			BYTE	trigger_band_width	:4;
			BYTE	reserved2			:4;
		}eb_msg1;
		
	}eb_data_segment;


	// crc 32 bits
}EmegencyBroadcast;


typedef struct  
{
	NetworkInfoTable					nit;
	
	ContinueServiceMultiControlTable	cmct;
	ShortServiceMultiControlTable		smct;
	
	ContinueServiceControlTable			csct;
	ShortServiceControlTable			ssct;
	
	ESgBasicDescription					esg_desc;
	EncryptionAuthorizationDescTable	ea_desc;
	EmegencyBroadcast					emegency_broadcast;
}TS0Info;


typedef struct 
{
	MultiplexFrameHeader	frame_header;
	TS0Info					ts0_info;

	struct MultiplexSubFrame
	{
		MultiSubFrameHeader		sub_frame_header[MAX_SUBFRAME_NUM];
	}multiplex_sub_frame;

}MultiplexFrame;

typedef enum
{
	FRAME_I	= 0,
	FRAME_P = 1,
	FRAME_B = 2
	// 3 ~ 7 reserved
}FRAMETYPE;


//video data in one video fragment(复用块)
typedef struct
{
	BYTE buf_count;

	struct VideoFrameBuf
	{
		BYTE*		pData;
		DWORD		dwLen;
		DWORD		dwTimeStamp;
		FRAMEPOS	pos;
		FRAMETYPE	type; // key frame?
		BYTE		nCodecType;
	}frame[10];
}VideoUnitBuf;

//audio data in one audio fragment
typedef struct
{
	BYTE buf_count;

	struct AudioFrameBuf
	{
		BYTE*		pData;
		DWORD		dwLen;
		DWORD		dwTimeStamp;
		BYTE		nCodecType;
	}frame[25];
}AudioUnitBuf;

typedef enum
{
	SINGLE_NAL	= 23,
	STAP_A		= 24,
	STAP_B		= 25,
	MTAP_16		= 26,
	MTAP_24		= 27,
	FU_A		= 28,
	FU_B		= 29
}NALTYPE;

// data segment data type
typedef enum
{
	DATA_ESG			= 0,
	DATA_PROGRAM_GUIDE	= 1,
	DATA_ECM_1			= 128,	// CAS-1
	DATA_EMM_1			= 129,	
	DATA_ECM_2			= 130,	// CAS-2
	DATA_EMM_2			= 131,	
	DATA_ECM_3			= 132,	// CAS-3
	DATA_EMM_3			= 133,
	DATA_XPE			= 160,
	DATA_XPE_FEC		= 161,
	DATA_TEST			= 255

}DataUnitType;

typedef struct  
{
	BYTE	record_count	:2;
	// reserved	, 6bits

	struct RecordInfo 
	{
		WORD	date;			// 16 bits, MJ date
		DWORD	time;			// 24 bits, BCD
		WORD	hour;
		WORD	minute;
		WORD	second;
		WORD	duration;		// 16 bits

		DWORD	language_type	:24;
		DWORD	theme_len		:8;

		BYTE*	theme_ptr;		// 
	}record_info[MAX_PROGRAM_GUIDE_INFO];

	// additional definition
	WORD	info_len;			// not include CRC32

}ProgramGuideInfo;

typedef struct 
{
	BYTE	id;				// 8 bits
	BYTE	type		:4;	// see EsgDataType
	BYTE	reserved	:1;
	BYTE	encode		:3;	// see DataEncodeType
	BYTE	seg_sn;			// 8 bits
	BYTE	total;			// 8 bits
	WORD	payload_len;	// 16 bits
}EsgDataSegmentHeader;

//esg data segment
typedef struct  
{

	EsgDataSegmentHeader header;
	BYTE*	payload;

	void init()
	{
		payload	= NULL;
		memset(&header, 0, sizeof(EsgDataSegmentHeader));
		header.id		= -1;
		header.seg_sn	= 0;
		header.type		= -1;
	}

	//CRC32

	//additional definition
	//BYTE	esg_update_sn;
}EsgDataSegment;




//#define _OLD_ESG_PARSE

#ifdef _OLD_ESG_PARSE
//esg data container
#ifdef WIN32
class __declspec(dllexport) EsgDataContainer
#else
class EsgDataContainer
#endif
{
public:
	EsgDataContainer(void);
	virtual ~EsgDataContainer(void);

public:
	void init(EsgDataSegment* pSeg);
	void insert(EsgDataSegment* pSeg);
	void reset();

public:
	EsgDataSegment**	seg;
	BYTE				seg_count;
	BYTE				curr_seg_count;
	DWORD				total_payload_len;

	// 20090820
	BYTE				next_seg_sn;
};
#else

#define MAX_ESG_SEG_COUNT 32
// #ifdef WIN32
// class __declspec(dllexport) EsgDataContainer
// #else
class EsgDataContainer
/*#endif*/
{
public:
	EsgDataContainer(void);
	virtual ~EsgDataContainer(void);

public:
	void init(EsgDataSegment* pSeg);
	void insert(EsgDataSegment* pSeg);
	void reset();

public:
	EsgDataSegment		seg[MAX_ESG_SEG_COUNT];
	BYTE				seg_count;
	BYTE				curr_seg_count;
	DWORD				total_payload_len;

	// 20090820
	BYTE				next_seg_sn;
};

#endif

typedef enum
{
	SERVICE_INFO		= 1,
	SERVICE_AUX_INFO	= 2,
	SCHEDULE_INFO		= 3,
	CONTENT_INFO		= 4,
	SERVICE_PARAM_INFO	= 5,
	TOTAL_ESG_DATA_TYPE	= 5
}EsgDataStyle;

typedef enum
{
	NO_ENCODE	= 0,
	GZIP_ENCODE	= 1
}DataEncodeType;

typedef struct
{

}CADescription;

typedef struct
{
	BYTE		usage;				// 8 bits
	VO_TCHAR	content_class[32];
	void*		media_data;
	VO_TCHAR	media_uri[512];
}EsgMediaType;

typedef struct _tagEsgServiceInfo
{
	BYTE	service_class;		// 8 bits
	BYTE	service_genre;		// 8 bits
	WORD	service_param_id;	// 16 bits
	VO_TCHAR	service_name_str[256];
	VO_TCHAR	service_name_lang[32];
	bool	for_free;

	CADescription ca_desc;

	WORD	service_id;			// 16 bits

	_tagEsgServiceInfo()
	{
		memset(service_name_str, 0, 256);
		memset(service_name_lang, 0, 32);
	};
}EsgServiceInfo;

typedef struct _tagEsgServiceAuxInfo
{
	VO_TCHAR	service_lang_special[32];
	VO_TCHAR	service_desc[256];
	VO_TCHAR	service_provider[256];
	VO_TCHAR	service_lang[32];
	WORD	service_id;			// 16 bits

	EsgMediaType media_type;

	_tagEsgServiceAuxInfo()
	{
		memset(service_lang_special, 0, 32);
		memset(service_desc, 0, 256);
		memset(service_provider, 0, 256);
		memset(service_lang, 0, 32);
	};

}EsgServiceAuxInfo;

typedef struct _tagEsgScheduleInfo 
{
	WORD	service_id;			// 16 bits
	WORD	content_id;			// 32 bits
	VO_TCHAR	title[256];
	VO_TCHAR	lang[32];
	VO_TCHAR	date[32];
	VO_TCHAR	time[32];
	bool	for_free;
	bool	live;
	bool	repeat;
	CADescription ca_desc;
	WORD	schedule_id;		// 16 bits

	_tagEsgScheduleInfo()
	{
		memset(title, 0, 256);
		memset(lang, 0, 32);
		memset(date, 0, 32);
		memset(time, 0, 32);
	};

}EsgScheduleInfo;

typedef struct _tagEsgContentInfo
{
	VO_TCHAR	content_class[128];
	BYTE	content_genre;			// 8 bits
	WORD	duration;				// 16 bits
	VO_TCHAR	content_lang_special[64];
	VO_TCHAR	keyword[256];
	VO_TCHAR	digest_info[512];
	VO_TCHAR	title[128];
	VO_TCHAR	audio_lang[64];
	VO_TCHAR	subtitle_lang[64];
	WORD	service_para_id;		// 16 bits
	VO_TCHAR	extent_info[256];
	WORD	content_id;				// 16 bits

	_tagEsgContentInfo()
	{
		memset(content_class, 0, 128);
		memset(content_lang_special, 0, 64);
		memset(keyword, 0, 256);
		memset(digest_info, 0, 512);
		memset(title, 0, 128);
		memset(audio_lang, 0, 64);
		memset(subtitle_lang, 0, 64);
		memset(extent_info, 0, 256);
	};
}EsgContentInfo;

typedef struct 
{
	WORD	service_param_id;		// 16 bits
	void*	para;
}EsgServiceParamInfo;


typedef struct
{
	EsgServiceInfo		service_info;
	EsgServiceAuxInfo	service_aux_info;
	EsgScheduleInfo		schedule_info;
	EsgContentInfo		content_info;	
	EsgServiceParamInfo service_param_info;
}EsgDataBuf;

typedef struct
{
	BYTE	command_id;	// 8 bits
	DWORD	length;		// 32 bits
	BYTE*	payload;	// see TLV_MsfPayload or other payload type
}TLV_Packet;

typedef enum
{
	HTTP_GET			= 0x01,
	HTTP_POST			= 0x02,
	HTTP_HEAD			= 0x03,
	HTTP_PUT			= 0x04,
	HTTP_DELETE			= 0x05,
	HTTP_RESPONSE		= 0x06,

	HTTP_GET_TLS		= 0x07,
	HTTP_POST_TLS		= 0x08,
	HTTP_HEAD_TLS		= 0x09,
	HTTP_PUT_TLS		= 0x0a,
	HTTP_DELETE_TLS		= 0x0b,
	HTTP_RESPONSE_TLS	= 0x0c,
	TLS_TERMINAL_REQUEST= 0x0d,
	TLS_CAS_RESPONSE	= 0x0e,
	TLS_TERMINAL_FINISHED	= 0x0f,

	MUX_SUB_FRAME		= 0x10,
	MUX_SUB_FRAME_SAC	= 0x11,

	ACKNOWLEDGEMENT		= 0xfe,
	ERROR_CMD			= 0xff

}TLV_CommandID;

typedef struct
{
	WORD		service_id;	// 16 bits
	BYTE		frame_pos;	//	8 bits, see TLV_FRAME_POS
	BYTE*		payload;
}TLV_MsfPayload;

typedef enum
{
	TLV_FRM_WHOLE		= 0x00,
	TLV_FRM_BEGIN		= 0x01,
	TLV_FRM_MID			= 0x02,
	TLV_FRM_END			= 0x03
}TLV_FramePos;

//
typedef enum
{
	COMPLETE_MSF		= 0,
	MUX_SEGMENT_AUDIO,
	MUX_SEGMENT_VIDEO,
	MUX_SEGMENT_DATA,
	MUX_SEGMENT_ECM,
	MUX_SEGMENT_EMM
}DescramblingType;

//
typedef struct
{
	BYTE	type;				/*!< [in]Refer to DescramblingType */
	WORD	mf_id;				/*!< [in]MF id */
	BYTE*	encrypt_data_ptr;	/*!< [in]Encrypt data pointer */
	WORD	encrypt_data_len;	/*!< [in]Encrypt data length */
	BYTE	ecm_emm_type;		/*!< [in]ECM EMM type */

	BYTE*	decrypt_data_ptr;	/*!< [out]Decrypt data pointer */
	WORD	decrypt_data_len;	/*!< [out]Decrypt data length */
	bool	encrypt;			/*!< [out]Set true if describling successed, default is false */	
}CaPacket;

typedef enum
{
	DATA_SERVICE_STREAM_MODE	= 0,
	DATA_SERVICE_FILE_MODE		= 1
}DataServiceMode;

typedef enum
{
	TYPE_XPE		= 0,
	TYPE_XPE_FEC	= 1
}XpeType;

typedef struct
{
	BYTE*			data;
	WORD			data_len;
	DataServiceMode mode;
	XpeType			type;
}XpeBuf;

/*
static DWORD freq_list[]=
{
	0,   0,   0,   0, //  0 - 3
	0,   0,   0,   0, //  4 - 7
	0,   0,   0,   0, //  8 - 11
	0, 474, 482, 490, // 12 - 15
	498, 506, 514, 522, // 16 - 19
	530, 538, 546, 554, // 20 - 23
	562, 610, 618, 626, // 24 - 27
	634, 642, 650, 658, // 28 - 31
	666, 674, 682, 690, // 32 - 35
	698, 706, 714, 722, // 36 - 39
	730, 738, 746, 754, // 40 - 43
	762, 770, 778, 786, // 44 - 47
	794, 802,   0,   0, // 48 - 51
	0,   0,   0,   0,
	0,   0,   0,   0,
	0,   0,   0,   0,
	0,   0,   0,   0,
	0,   0,	0,   0,
	0,   0,   0,   0,
	0,   0,   0,   0,
	0,   0,   0,   0,
	0,   0,   0,   0,
	0,   0,	0,   0,
	0,   0,   0,   0,
	0,   0,   0,   0,
	2620,  2628,  2636 // 100 - 102
};
*/

/*
typedef enum
{
	STATE_ESG_SERVICE_INFO		= 0x0001,
	STATE_ESG_SERVICE_AUX_INFO	= 0x0002,
	STATE_ESG_SCHEDULE_INFO		= 0x0004,
	STATE_ESG_CONTENT_INFO		= 0x0008,
	STATE_ESG_SERVICE_PARA_INFO	= 0x0010
}EsgParseState;
*/

// ESG : General Tag
#define ESG_BASE_TAG			"esg:ESGDATA"
#define ESG_VALUE_TRUE			"true"
#define TAG_LANG				"lang"
#define TAG_SERVICE_PARA_ID		"ServiceParaID"


// ESG : Schedule Tag
#define TAG_SCHEDULE				"Schedule"
#define TAG_SCHEDULE_SERVICE_ID		"ServiceID"
#define TAG_SCHEDULE_CONTENT_ID		"ContentID"
#define TAG_TITLE_STR				"TitleStr"
#define TAG_DATE					"Date"
#define TAG_TIME					"Time"
#define TAG_FOR_FREE				"forFree"
#define TAG_CA_DESC					"CADescriptior"
#define TAG_LIVE					"Live"
#define TAG_REPEAT					"Repeat"
#define TAG_SCHEDULE_TITLE			"Title"
#define TAG_SCHEDULE_ID				"ScheduleID"
#define TAG_STARTTIME				"StartTime"

// ESG : Content Info Tag
#define TAG_CONTENT_CLASS			"ContentClass"
#define TAG_CONTENT_GENRE			"ContentGenre"
#define TAG_DURATION				"Duration"
#define TAG_DIGEST_INFO				"DigestInfo"
#define TAG_CONTENT_TITLE			"Title"
#define TAG_AUDIO_LANG				"AudioLang"
#define TAG_SUBTITLE_LANG			"SubtitleLang"
#define TAG_CONTENT_LANG_SPECIAL	"ContentLanguageSpecial"
#define TAG_CONTENT					"Content"
#define TAG_CONTENT_ID				"ContentID"
#define TAG_CONTENT_SERVICE_PARA_ID	"ServiceParaID"


// ESG : Service Aux Tag
#define TAG_SERVICE_AUX			"ServiceAux"
#define TAG_SERVICE_LANG_SPC	"ServiceAuxLanguageSpecial"
#define TAG_SERVICE_DESC		"ServiceDescription"
#define TAG_SERVICE_PROVIDER	"ServiceProvider"
#define TAG_SERVICE_LANGUAGE	"ServiceLanguage"
#define TAG_MEDIA				"Media"
#define TAG_MEDIA_USAGE			"Usage"
#define TAG_MEDIA_CONTENT_CLASS "ContentClass"
#define TAG_MEDIA_DATA			"MediaData"
#define TAG_MEDIA_URI			"MediaURI"
#define TAG_AUX_SERVICE_ID		"ServiceID"

// ESG : Service Info Tag
#define TAG_SERVICE_CLASS		"ServiceClass"
#define TAG_SERVICE_GENRE		"ServiceGenre"
#define TAG_SERVICE_NAME_STR	"ServiceNameStr"
#define TAG_SERVICE_FOR_FREE	"forFree"
#define TAG_SERVICE_NAME		"ServiceName"
#define TAG_SERVICE_SERVICE_ID	"ServiceID"
#define TAG_SERVICE				"Service"

// ESG : Service Param Tag
#define TAG_SERVICE_PARA		"ServicePara"
#define TAG_SERVICE_PARA_DATA	"Para"


class CDmxResultReciever
{
protected:
	CDmxResultReciever(){};
public:
	// MF & MSF header
	virtual void WhenParsedMultiFrameHeader(MultiplexFrameHeader* pHeader)=0;
	virtual void WhenParsedMultiSubFrameHeader(MultiSubFrameHeader* pHeader)=0;

	// TS0 - Control table info
	virtual void WhenParsedCMCT(ContinueServiceMultiControlTable* pCMCT)=0;
	virtual void WhenParsedSMCT(ShortServiceMultiControlTable* pSMCT)=0;
	virtual void WhenParsedCSCT(ContinueServiceControlTable* pCSCT)=0;
	virtual void WhenParsedSSCT(ShortServiceControlTable* pSSCT)=0;
	virtual void WhenParsedEB(EmegencyBroadcast* pEB)=0;

	virtual void WhenParsedNIT(NetworkInfoTable* pNIT)=0;
	virtual void WhenParsedEsgBasicDescription(ESgBasicDescription* pEsgDesc)=0;
	virtual void WhenParsedEADesc(EncryptionAuthorizationDescTable* pDesc)=0;
	virtual void WhenParsedTS0(TS0Info* pTS0)=0;

	// AV
	virtual void WhenParsedVideoFrame(VideoUnitBuf* pVideo)=0;
	virtual void WhenParsedAudioFrame(AudioUnitBuf* pVideo)=0;

	// ESG
	virtual void WhenParsedProgramGuide(ProgramGuideInfo* pInfo)=0;
	virtual void WhenParsedEsgServiceInfo(EsgServiceInfo* pInfo)=0;
	virtual void WhenParsedEsgServiceAuxInfo(EsgServiceAuxInfo* pInfo)=0;
	virtual void WhenParsedEsgScheduleInfo(EsgScheduleInfo* pInfo)=0;
	virtual void WhenParsedEsgServiceParamInfo(EsgServiceParamInfo* pInfo)=0;
	virtual void WhenParsedEsgContentInfo(EsgContentInfo* pInfo)=0;
	virtual void WhenParsedEsgFinished()=0;

	// CA
	virtual void WhenParsedEncryptData(CaPacket* pPacket)=0;

	// Error
	virtual void WhenParsedFail(int nErrorType, void* pParam)=0;

	// Parse whole MF Finished
	virtual void WhenParseWholeMFFinished(MultiplexFrame* pMF)=0;

	// Data Service
	virtual void WhenParsedXpe(XpeBuf* buf)=0;
};

#endif