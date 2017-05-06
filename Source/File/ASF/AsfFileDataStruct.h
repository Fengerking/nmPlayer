/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		AsfFileDataStruct.h

Contains:	Data Struct Of ASF File

Written by:	East

Reference:	ASF Specification

Change History (most recent first):
2006-08-21		East			Create file

*******************************************************************************/
#ifndef __ASF_File_Data_Struct
#define __ASF_File_Data_Struct

//Base Data Struct
#define Len_Object						24
typedef struct tagAsfObject {
	VO_GUID		id;
	VO_U64		size;
} AsfObject, *PAsfObject;
//end of Base Data Struct

//ASF Header Object
#define Len_Header_Object				6
typedef struct tagAsfHeaderObject {
	VO_U32		header_objects_number;
	VO_U8		reserved1;	//must be set to 0x01
	VO_U8		reserved2;	//must be set to 0x02
} AsfHeaderObject, *PAsfHeaderObject;

#define Len_File_Prop_Object			80
typedef struct tagAsfFilePropObject {
	VO_GUID		file_id;
	VO_U64		file_size;
	VO_U64		creation_date;
	VO_U64		data_packets_count;
	VO_U64		play_duration;
	VO_U64		send_duration;
	VO_U64		preroll;
	VO_U32		flags;
	VO_U32		min_data_packet_size;
	VO_U32		max_data_packet_size;
	VO_U32		max_bitrate;
} AsfFilePropObject, *PAsfFilePropObject;

#define Len_Stream_Prop_Object			54
typedef struct tagAsfStreamPropObject {
	VO_GUID		stream_type;
	VO_GUID		error_correction_type;
	VO_U64		time_offset;
	VO_U32		type_specific_data_len;
	VO_U32		error_correction_data_len;
	VO_U16		flags;
	VO_U32		reserved;
	VO_PBYTE	type_specific_data;
	VO_PBYTE	error_correction_data;
} AsfStreamPropObject, *PAsfStreamPropObject;

#define Len_Header_Ext_Object			22
typedef struct tagAsfHeaderExtObject {
	VO_GUID		reserved_filed_1;
	VO_U16		reserved_filed_2;
	VO_U32		data_size;
	VO_PBYTE	data;
} AsfHeaderExtObject, *PAsfHeaderExtObject;

typedef struct tagAsfCodecEntries {
	VO_U16		type;
	VO_U16		name_len;
	VO_PTCHAR	name;
	VO_U16		descr_len;
	VO_PTCHAR	descr;
	VO_U16		info_len;
	VO_PBYTE	info;
} AsfCodecEntries, PAsfCodecEntries;

#define Len_Codec_List_Object			20
typedef struct tagAsfCodecListObject {
	VO_GUID		reserved;
	VO_U32		entries_count;
	PAsfCodecEntries	entries;
} AsfCodecListObject, *PAsfCodecListObject;

typedef struct tagAsfCommandType {
	VO_U16		name_len;
	VO_PTCHAR	name;
} AsfCommandType, *PAsfCommandType;

typedef struct tagAsfCommand {
	VO_U32		presentation_time;
	VO_U16		type_index;
	VO_U16		name_len;
	VO_PTCHAR	name;
} AsfCommand, *PAsfCommand;

typedef struct tagAsfScriptCmdObject {
	VO_GUID		reserved;
	VO_U16		commands_count;
	VO_U16		command_types_count;
	PAsfCommandType	command_types;
	PAsfCommand		commands;
} AsfScriptCmdObject, *PAsfScriptCmdObject;

typedef struct tagAsfMarket {
	VO_U64		offset;
	VO_U64		presentation_time;
	VO_U16		entry_len;
	VO_U32		send_time;
	VO_U32		flags;
	VO_U32		descr_len;
	VO_PTCHAR	descr;
} AsfMarket, *PAsfMarket;

typedef struct tagAsfMarketObject {
	VO_GUID		reserved;
	VO_U32		market_count;
	VO_U16		reserved1;
	VO_U16		name_len;
	VO_PTCHAR	name;
	PAsfMarket	markets;
} AsfMarketObject, *PAsfMarketObject;

#define Len_Bitrate_Mutex_Object		18
typedef struct tagAsfBitrateMutexObject {
	VO_GUID		mutex_type;
	VO_U16		stream_numbers_count;
	VO_U16*		stream_numbers;
} AsfBitrateMutexObject, *PAsfBitrateMutexObject;

typedef struct tagAsfErrCorrectObject {
	VO_GUID		type;
	VO_U32		data_len;
	VO_PBYTE	data;
} AsfErrCorrectObject, *PAsfErrCorrectObject;

#define Len_Content_Desc_Object			10
typedef struct tagAsfContentDescObject {
	VO_U16		title_len;
	VO_U16		author_len;
	VO_U16		copyright_len;
	VO_U16		descr_len;
	VO_U16		rating_len;
	VO_PTCHAR	title;
	VO_PTCHAR	author;
	VO_PTCHAR	copyright;
	VO_PTCHAR	descr;
	VO_PTCHAR	rating;
} AsfContentDescObject, *PAsfContentDescObject;

typedef struct tagAsfContentDesc {
	VO_U16		name_len;
	VO_PTCHAR	name;
	VO_U16		value_data_type;
	VO_U16		value_len;
	VO_PTR		value;
} AsfContentDesc, *PAsfContentDesc;

typedef struct tagAsfMetadata {
	VO_U16		reserved;	//"Language List Index" for ASF_Metadata_Library_Object
	VO_U16		stream_number;
	VO_U16		name_len;
	VO_U16		data_type;
	VO_U32		data_len;
	VO_PTCHAR	name;
	VO_PTR		data;
} AsfMetadata, *PAsfMetadata;

#define Len_Ext_Content_Desc_Object		2
typedef struct tagAsfExtContentDescObject {
	VO_U16		content_descs_count;
	PAsfContentDesc	content_descs;
} AsfExtContentDescObject, *PAsfExtContentDescObject;

#define Len_Bitrate_Record				6
typedef struct tagAsfBitrateRecord {
	VO_U16		flags;
	VO_U32		average_bitrate;
} AsfBitrateRecord, *PAsfBitrateRecord;

#define Len_Stream_Bitrate_Prop_Object	2
typedef struct tagAsfStreamBitratePropObject {
	VO_U16		records_count;
	PAsfBitrateRecord	records;
} AsfStreamBitratePropObject, *PAsfStreamBitratePropObject;

typedef struct tagAsfContentBrandObject {
	VO_U32		banner_image_type;
	VO_U32		banner_image_data_size;
	VO_PBYTE	banner_image_data;
	VO_U32		banner_image_url_len;
	VO_PCHAR	banner_image_url;
} AsfContentBrandObject, *PAsfContentBrandObject;

typedef struct tagAsfContentEncryptObject {
	VO_U32		secret_data_len;
	VO_PBYTE	secret_data;
	VO_U32		protection_type_len;
	VO_PCHAR	protection_type;
	VO_U32		key_id_len;
	VO_PCHAR	key_id;
	VO_U32		license_url_len;
	VO_PCHAR	license_url;
} AsfContentEncryptObject, *PAsfContentEncryptObject;

typedef struct tagAsfExtContentEncryptObject {
	VO_U32		data_size;
	VO_PBYTE	data;
} AsfExtContentEncryptObject, *PAsfExtContentEncryptObject;

typedef struct tagAsfDigiSignObject {
	VO_U32		sign_type;
	VO_U32		sign_data_len;
	VO_PBYTE	sign_data;
} AsfDigiSignObject, *PAsfDigiSignObject;

typedef struct tagAsfPaddingObject {
	VO_PBYTE	data;
} AsfPaddingObject, *PAsfPaddingObject;
//end of ASF Header Object

//ASF Header Extension Object
#define Len_Stream_Name				4
typedef struct tagAsfStreamName {
	VO_U16		lang_id_index;
	VO_U16		name_len;
	VO_PTCHAR	name;
} AsfStreamName, *PAsfStreamName;

#define Len_Payload_Ext_Sys			22
typedef struct tagAsfPayloadExtSys {
	VO_GUID		id;
	VO_U16		size;
	VO_U32		info_len;
	VO_PBYTE	info;
} AsfPayloadExtSys, *PAsfPayloadExtSys;

#define Len_Ext_Stream_Prop_Object		64
typedef struct tagAsfExtStreamPropObject {
	VO_U64		start_time;
	VO_U64		end_time;
	VO_U32		data_bitrate;
	VO_U32		buffer_size;
	VO_U32		init_buffer_fullness;
	VO_U32		alternate_data_bitrate;
	VO_U32		alternate_buffer_size;
	VO_U32		alternate_init_buffer_fullness;
	VO_U32		max_object_size;
	VO_U32		flags;
	VO_U16		stream_number;
	VO_U16		stream_lang_id_index;
	VO_U64		average_time_per_frame;
	VO_U16		stream_name_count;
	VO_U16		payload_ext_sys_count;
	PAsfStreamName			stream_names;
	PAsfPayloadExtSys		payload_ext_syss;
	PAsfStreamPropObject	stream_prop_objects;
} AsfExtStreamPropObject, *PAsfExtStreamPropObject;

typedef struct tagAsfAdvanceMutexObject {
	VO_GUID		mutex_type;
	VO_U16		stream_numbers_count;
	VO_U16*		stream_numbers;
} AsfAdvancedMutexObject, *PAsfAdvancedMutexObject;

typedef struct tagAsfRecord {
	VO_U16		stream_numbers_count;
	VO_U16*		stream_numbers;
} AsfRecord, *PAsfRecord;

typedef struct tagAsfGroupMutexObject {
	VO_GUID		mutex_type;
	VO_U16		records_count;
	PAsfRecord	records;
} AsfGroupMutexObject, *PAsfGroupMutexObject;

#define Len_Prior_Record				4
typedef struct tagAsfPriorRecord {
	VO_U16		stream_number;
	VO_U16		flags;
} AsfPriorRecord, *PAsfPriorRecord;

#define Len_Stream_Prior_Object			2
typedef struct tagAsfStreamPriorObject {
	VO_U16		records_count;
	PAsfPriorRecord	records;
} AsfStreamPriorObject, *PAsfStreamPriorObject;

typedef struct tagAsfBandwidthShareObject {
	VO_GUID		share_type;
	VO_U32		data_bitrate;
	VO_U32		buffer_size;
	VO_U16		stream_numbers_count;
	VO_U16*		stream_numbers;
} AsfBandwidthShareObject, *PAsfBandwidthShareObject;

typedef struct tagAsfLangIDRecord {
	VO_U8		lang_id_len;
	VO_PTCHAR	lang_id;
} AsfLangIDRecord, *PAsfLangIDRecord;

#define Len_Lang_List_Object			2
typedef struct tagAsfLangListObject {
	VO_U16		lang_id_records_count;
	PAsfLangIDRecord	lang_id_records;
} AsfLangListObject, *PAsfLangListObject;

#define Len_Desc_Record					12
typedef struct tagAsfDescRecord {
	VO_U16		reserved;	//if in tagAsfMetaDataLibObject, it is lang_list_index
	VO_U16		stream_number;
	VO_U16		name_len;
	VO_U16		data_type;
	VO_U32		date_len;
	VO_PTCHAR	name;
	VO_PTR		data;
} AsfDescRecord, *PAsfDescRecord;

#define Len_Meta_Data_Object			2
typedef struct tagAsfMetaDataObject {
	VO_U16		descr_records_count;
	PAsfDescRecord	descr_records;
} AsfMetaDataObject, *PAsfMetaDataObject;

typedef AsfMetaDataObject AsfMetaDataLibObject;
typedef PAsfMetaDataObject PAsfMetaDataLibObject;

#define Len_Index						4
typedef struct tagAsfIndex {
	VO_U16		stream_number;
	VO_U16		index_type;
} AsfIndex, *PAsfIndex;

typedef struct tagAsfIndexParamObject {
	VO_U32		index_entry_time_interval;
	VO_U16		indexs_count;
	PAsfIndex	indexs;
} AsfIndexParamObject, *PAsfIndexParamObject;

typedef AsfIndexParamObject AsfMediaObjIndexParamObject;
typedef PAsfIndexParamObject PAsfMediaObjIndexParamObject;
typedef AsfIndexParamObject AsfTimecodeIndexParamObject;
typedef PAsfIndexParamObject PAsfTimecodeIndexParamObject;

#define Len_Compatible_Object			2
typedef struct tagAsfCompatibleObject {
	VO_U8		profile;
	VO_U8		mode;
} AsfCompatibleObject, *PAsfCompatibleObject;

typedef struct tagAsfEncryptObjectRecord {
	VO_U16		id_type;
	VO_U16		id_len;
	VO_PTR		id;
} AsfEncryptObjectRecord, *PAsfEncryptObjectRecord;

typedef struct tagAsfContentEncryptRecord {
	VO_GUID		sys_id;
	VO_U32		sys_ver;
	VO_U16		encrypt_obj_records_count;
	PAsfEncryptObjectRecord	encrypt_obj_records;
	VO_U32		data_size;
	VO_PBYTE	data;
} AsfContentEncryptRecord, *PAsfContentEncryptRecord;

typedef struct tagAsfAdvanceContentEncryptObject {
	VO_U16		content_encrypt_records_count;
	PAsfContentEncryptRecord	content_encrypt_records;
} AsfAdvanceContentEncryptObject, *PAsfAdvanceContentEncryptObject;
//end of ASF Header Extension Object

//ASF Data Object
#define Len_Data_Object					26
typedef struct tagAsfDataObject {
	VO_GUID		file_id;
	VO_U64		total_data_packets;
	VO_U16		reserved;
	//need to be filled.
} AsfDataObject, *PAsfDataObject;
//end of ASF Data Object

//ASF Index Object
#define Len_Index_Entry					6
typedef struct tagAsfIndexEntry {
	VO_U32		packet_number;
	VO_U16		packet_count;
} AsfIndexEntry, *PAsfIndexEntry;

typedef struct tagAsfSimpleIndexObject {
	VO_GUID		file_id;
	VO_U64		index_entry_time_interval;
	VO_U32		max_packet_count;
	VO_U32		index_entries_count;
	PAsfIndexEntry	index_entries;
} AsfSimpleIndexObject, *PAsfSimpleIndexObject;

typedef struct tagAsfIndexBlock {
	VO_U32		index_entries_count;
	VO_U64*		block_positions;
	VO_U32*		index_entries;		//offset
} AsfIndexBlock, *PAsfIndexBlock;

typedef struct tagAsfIndexObject {
	VO_U32		index_entry_time_interval;	//in tagAsfTimecodeIndexObject, it is  reserved
	VO_U16		indexs_count;
	VO_U32		index_blocks_count;
	PAsfIndex		indexs;
	PAsfIndexBlock	index_blocks;
} AsfIndexObject, *PAsfIndexObject;

typedef AsfIndexObject	AsfMediaObjIndexObject;
typedef PAsfIndexObject	PAsfMediaObjIndexObject;
typedef AsfIndexObject	AsfTimecodeIndexObject;
typedef PAsfIndexObject	PAsfTimecodeIndexObject;
//end of ASF Index Object

//////////////////////////////////////////////////////////////////////////
//MediaType
//////////////////////////////////////////////////////////////////////////
#define Len_Audio_Media_Type			18
typedef struct tagAsfAudioMediaType {
	VO_U16		codec_id;
	VO_U16		channels;
	VO_U32		samples_per_second;
	VO_U32		average_bytes_per_second;
	VO_U16		block_alignment;
	VO_U16		bits_per_sample;
	VO_U16		codec_data_len;
	VO_PBYTE	codec_data;
} AsfAudioMediaType, *PAsfAudioMediaType;

#define Len_Video_Format				40
typedef struct tagAsfVideoFormat {
	VO_U32		size;
	VO_S32		image_width;
	VO_S32		image_height;
	VO_U16		reserved;
	VO_U16		bits_per_pixel;
	VO_U32		compression_id;
	VO_U32		image_size;
	VO_S32		horizontal_pixels_per_meter;
	VO_S32		vertical_pixels_per_meter;
	VO_U32		colors_used;
	VO_U32		important_colors;
	VO_PBYTE	codec_data;
} AsfVideoFormat, *PAsfVideoFormat;

#define Len_Video_Media_Type			51
typedef struct tagAsfVideoMediaType {
	VO_U32		image_width;
	VO_U32		image_height;
	VO_U8		reserved;
	VO_U16		format_data_len;
	AsfVideoFormat	format_data;
} AsfVideoMediaType, *PAsfVideoMediaType;

typedef struct tagAsfStreamExtInfo {
	VO_U8				btStreamNum;
	VO_U32				dwBitrate;			//bytes per seconds
	VO_U32				dwMaxSampleSize;
	VO_U32				dwFrameRate;		//frames per 100 seconds
	tagAsfStreamExtInfo	*next;
} AsfStreamExtInfo, *PAsfStreamExtInfo;

typedef struct tagAsfTrackInfo {
	VO_U8			btStreamNum;
	VO_BOOL			bVideo;
	VO_U32			dwPropBufferSize;
	VO_PBYTE		pPropBuffer;
	tagAsfTrackInfo	*next;
} AsfTrackInfo, *PAsfTrackInfo;

typedef struct tagAsfPriorInfo {
	VO_U8	btStreamNum;
	VO_U8	btPrior;
} AsfPriorInfo, *PAsfPriorInfo;

typedef struct tagAsfBitrateInfo {
	VO_U8	btStreamNum;
	VO_U32	dwBitrate;		//bits per second
} AsfBitrateInfo, *PAsfBitrateInfo;
#endif	//__ASF_File_Data_Struct