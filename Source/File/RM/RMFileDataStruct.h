/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		RMFileDataStruct.h

Contains:	Data Struct Of Real Media File Format(RMFF)

Written by:	East

Reference:	RMFormatSDK.pdf

Change History (most recent first):
2006-09-20		East			Create file

*******************************************************************************/
#ifndef __RM_File_Data_Struct_H
#define __RM_File_Data_Struct_H

#include "fMacros.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

/*
const value
*/
#define HX_KEYFRAME_FLAG			2
#define HX_KEYFRAME_RULE			0
#define HX_NONKEYFRAME_RULE			1
#define HX_THINNING_RULE			2
#define HX_ASM_SWITCH_ON			1
#define HX_ASM_SWITCH_OFF			2
#define RM_KEYFRAME_FLAG			0x80
#define RM_PACKET_HEADER_SIZE		12

/* Audio codec defines */
#define RA_INTERLEAVER_NONE			0x496E7430  /* 'Int0' (no interleaver) */
#define RA_INTERLEAVER_SIPR			0x73697072  /* interleaver used for SIPRO codec ("sipr") */
#define RA_INTERLEAVER_GENR			0x67656E72  /* interleaver used for ra8lbr and ra8hbr codecs ("genr") */
#define RA_INTERLEAVER_VBRS			0x76627273  /* Simple VBR interleaver ("vbrs") */
#define RA_INTERLEAVER_VBRF			0x76627266  /* Simple VBR interleaver (with possibly fragmenting) ("vbrf") */

/* Video codec defines */
#define HX_RVTRVIDEO_ID				0x52565452  /* 'RVTR' (for rv20 codec) */
#define HX_RVTR_RV30_ID				0x52565432  /* 'RVT2' (for rv30 codec) */
#define HX_RV20VIDEO_ID				0x52563230  /* 'RV20' */
#define HX_RV30VIDEO_ID				0x52563330  /* 'RV30' */
#define HX_RV40VIDEO_ID				0x52563430  /* 'RV40' */
#define HX_RVG2VIDEO_ID				0x52564732  /* 'RVG2' (raw TCK format) */
#define HX_RV89COMBO_ID				0x54524F4D  /* 'TROM' (raw TCK format) */

enum PROPERTY_TYPES
{
	MPT_TEXT = 1,	// The value is string data;
	MPT_TEXTLIST,	// The value is a separated list of strings, separator specified as sub-property/type descriptor;
	MPT_FLAG,		// The value is a boolean flag-either 1 byte or 4 bytes, check size value;
	MPT_ULONG,		// The value is a four-byte integer;
	MPT_BINARY,		// The value is a byte stream;
	MPT_URL,		// The value is string data;
	MPT_DATE,		// The value is a string representation of the date in the form: YYYYmmDDHHMMSS (m = month, M = minutes);
	MPT_FILENAME,	// The value is string data;
	MPT_GROUPING,	// This property has sub-properties, but its own value is empty;
	MPT_REFERENCE,	// The value is a large buffer of data, use sub-properties/type, descriptors to identify mime-type.

	MPT_MAX = VO_MAX_ENUM_VALUE
};

enum PROPERTY_FLAGS
{
	MPF_READONLY = 1,			// Read only, cannot be modified;
	MPF_PRIVATE = 2,			// Private, do not expose to users;
	MPF_TYPE_DESCRIPTOR = 4,	// Type descriptor used to further define type of value;

	MPF_MAX = VO_MAX_ENUM_VALUE
};

enum FRAMT_TYPES
{
	RM_FRAMETYPE_PARTIAL = 0,			//partial frame
	RM_FRAMETYPE_WHOLE = 1,				//whole frame
	RM_FRAMETYPE_LAST_PARTIAL = 2,		//last frame
	RM_FRAMETYPE_MULTIPLE = 3,			//multiple frames
	RM_FRAMETYPE_MAX = VO_MAX_ENUM_VALUE
};

/*
*Mime Type Define
*/
#define MIME_TYPE_RA						"audio/x-pn-realaudio"
#define MIME_TYPE_RA_ENCRYPTED				"audio/x-pn-realaudio-encrypted"
#define MIME_TYPE_RLA						"logical-audio/x-pn-multirate-realaudio"
#define MIME_TYPE_RLA_SUB					"audio/x-pn-multirate-realaudio"
#define MIME_TYPE_RV						"video/x-pn-realvideo"
#define MIME_TYPE_RV_ENCRYPTED				"video/x-pn-realvideo-encrypted"
#define MIME_TYPE_RLV						"logical-video/x-pn-multirate-realvideo"
#define MIME_TYPE_RLV_SUB					"video/x-pn-multirate-realvideo"
#define MIME_TYPE_RE						"application/x-pn-realevent"
#define MIME_TYPE_RE_ENCRYPTED				"application/x-pn-realevent-encrypted"
#define MIME_TYPE_RIM						"application/x-pn-imagemap"
#define MIME_TYPE_RIM_ENCRYPTED				"application/x-pn-imagemap-encrypted"
#define MIME_TYPE_RIM_VIDEO					"image_map/x-pn-realvideo"
#define MIME_TYPE_RIM_VIDEO_ENCRYPTED		"image_map/x-pn-realvideo-encrypted"
#define MIME_TYPE_RSMM						"syncMM/x-pn-realvideo"
#define MIME_TYPE_RSMM_ENCRYPTED			"syncMM/x-pn-realvideo-encrypted"

#define RMMT_IS_AUDIO(rmmt)		(rmmt & 0x1)
#define RMMT_IS_VIDEO(rmmt)		(!(rmmt & 0x1))
enum RM_MEDIA_MIME_TYPES
{
	RMMT_UNKNOWN			= 0, 
	RMMT_AUDIO				= 1, 
	RMMT_VIDEO				= 2, 
	RMMT_LOGICAL_AUDIO		= 3, 
	RMMT_LOGICAL_VIDEO		= 4, 
	RMMT_LOGICAL_AUDIO_SUB	= 5, 
	RMMT_LOGICAL_VIDEO_SUB	= 6, 
	RMMT_MAX				= VO_MAX_ENUM_VALUE
};

/*
*FOURCC Define
*/
#define FOURCC_RMF							MAKEFOURCC('.', 'R', 'M', 'F')	//RMF flag
#define FOURCC_PROP							MAKEFOURCC('P', 'R', 'O', 'P')	//Property
#define FOURCC_MDPR							MAKEFOURCC('M', 'D', 'P', 'R')	//Media Property
#define FOURCC_CONT							MAKEFOURCC('C', 'O', 'N', 'T')	//Content Description
#define FOURCC_DATA							MAKEFOURCC('D', 'A', 'T', 'A')	//Data
#define FOURCC_INDX							MAKEFOURCC('I', 'N', 'D', 'X')	//Index
#define FOURCC_RMMD							MAKEFOURCC('R', 'M', 'M', 'D')	//Meta data
#define FOURCC_RJMD							MAKEFOURCC('R', 'J', 'M', 'D')	//Meta data Tag
#define FOURCC_RMJE							MAKEFOURCC('R', 'M', 'J', 'E')	//Meta data Footer
#define FOURCC_MLTI							MAKEFOURCC('M', 'L', 'T', 'I')	//Multi Header
#define FOURCC_RAID							0xFD61722E						//RA Header

//base building block
typedef struct tagRMFFChunk 
{
	VO_U32		fcc;
	VO_U32		size;
} RMFFChunk, *PRMFFChunk;

/*
1. header section
2. data section
3. index section
*/

/************************************************************************/
/* header section                                                       */
/************************************************************************/
//FOURCC_RM_File
typedef struct tagRMFFFileHeader 
{
	VO_U16		object_version;
	VO_U32		file_version;		//the version of the RM file, 0 or 1
	VO_U32		num_headers;		//the number of headers
} RMFFFileHeader, *PRMFFFileHeader;

//FOURCC_RM_Properties
typedef struct tagRMFFProperties 
{
	VO_U16		object_version;		//The version of the RealMedia File Header object.
	VO_U32		max_bit_rate;		//The maximum bit rate required to deliver this file over a network
	VO_U32		avg_bit_rate;		//The average bit rate required to deliver this file over a network.
	VO_U32		max_packet_size;	//The largest packet size (in bytes) in the media data.
	VO_U32		avg_packet_size;	//The average packet size (in bytes) in the media data.
	VO_U32		num_packets;		//The number of packets in the media data.
	VO_U32		duration;			//<MS>The duration of the file in milliseconds.
	VO_U32		preroll;			//<MS>, prebuffer before starting playback, subtract from
	VO_U32		index_offset;		//the index header object file position,This value can be 0 (zero), which indicates that no index chunks are present in this file.
	VO_U32		data_offset;		//the data section file position
	VO_U16		num_streams;		//total number of media properties
	VO_U16		flags;				//0xABCDEFGH H-Save_Enabled; G-Perfect_Play; F-Live
									//H-Save_Enabled , If 1, clients are allowed to save this file to disk.
									//G-Perfect_Play , If 1, clients are instructed to use extra buffering.
									//F-Live , If 1, these streams are from a live broadcast.
} RMFFProperties, *PRMFFProperties;

//FOURCC_RM_Media_Properties
typedef struct tagRMFFMediaProperties 
{
	VO_U16		object_version;
	VO_U16		stream_number;		//unique value identifies a physical stream
	VO_U32		max_bit_rate;
	VO_U32		avg_bit_rate;
	VO_U32		max_packet_size;
	VO_U32		avg_packet_size;
	VO_U32		start_time;			//<MS>, add to the time stamp of each packet in a physical stream
	VO_U32		preroll;			//<MS>, subtract from the time stamp of each packet in a physical stream
	VO_U32		duration;			//<MS>
	VO_U8		stream_name_size;
	VO_PBYTE	stream_name;		//non-unique alias or name for the stream
	VO_U8		mime_type_size;
	VO_PBYTE	mime_type;			//non-unique MIME style type/subtype string for data associated with the stream
	VO_U32		type_specific_len;
	VO_PBYTE	type_specific_data;	//is used by the data type renderer to initialize itself in order to process the physical stream
} RMFFMediaProperties, *PRMFFMediaProperties;

typedef struct tagRMFFNameValueProperty 
{
	VO_U32		size;
	VO_U16		object_version;
	VO_U8		name_length;
	VO_PBYTE	name;
	VO_S32		type;				//0-UINT32; 1-buffer; 2-string
	VO_U16		value_length;
	VO_PBYTE	value_data;
} RMFFNameValueProperty, *PRMFFNameValueProperty;

//consist in type_specific_data of RMFFMediaProperties
//if the mime_type is "logical-XXX"
typedef struct tagRMFFLogicalStream 
{
	VO_U32		size;
	VO_U16		object_version;
	VO_U16		num_physical_streams;
	VO_U16*		physical_stream_numbers;
	VO_U32*		data_offsets;		//each stream's file position
	VO_U16		num_rules;
	VO_U16*		rule_to_physical_stream_number_map;
	VO_U16		num_properties;
	RMFFNameValueProperty*	properties;
} RMFFLogicalStream, *PRMFFLogicalStream;

//FOURCC_RM_Content_Description
typedef struct tagRMFFContentDescription 
{
	VO_U16		object_version;
	VO_U16		title_len;
	VO_PBYTE	title;				//title
	VO_U16		author_len;
	VO_PBYTE	author;				//author
	VO_U16		copyright_len;
	VO_PBYTE	copyright;			//copyright
	VO_U16		comment_len;
	VO_PBYTE	comment;			//comment
} RMFFContentDescription, *PRMFFContentDescription;

/************************************************************************/
/* data section                                                         */
/************************************************************************/
//FOURCC_RM_Data
typedef struct tagRMFFDataChunkHeader 
{
	VO_U16		object_version;
	VO_U32		num_packets;		//number of packets in the data chunk
	VO_U32		next_data_header;	//next data chunk's file position
} RMFFDataChunkHeader, *PRMFFDataChunkHeader;

typedef struct tagRMFFMediaPacketHeader 
{
	VO_U16		object_version;
	VO_U16		length;				//packet length
	VO_U16		stream_number;		//
	VO_U32		time_stamp;			//<MS>
	VO_U8		packet_group;
	VO_U8		flags;
	VO_U16		asm_rule;
	VO_U8		asm_flags;
} RMFFMediaPacketHeader, *PRMFFMediaPacketHeader;

/************************************************************************/
/* index section                                                        */
/************************************************************************/
//index section header
//FOURCC_RM_Index
typedef struct tagRMFFIndexSectionHeader 
{
	VO_U16		object_version;
	VO_U32		num_indices;		//number of index records in the index chunk
	VO_U16		stream_number;		//associate stream's number
	VO_U32		next_index_header;	//next index chunk's file position
} RMFFIndexSectionHeader, *PRMFFIndexSectionHeader;

typedef struct tagRealFileIndexInfo
{
	VO_U32		num_indices;		//number of index records in the index chunk
	VO_U16		stream_number;		//associate stream's number
	VO_U64		file_position;		//file position of this index chunk
	tagRealFileIndexInfo*	next;
} RealFileIndexInfo, *PRealFileIndexInfo;

typedef struct tagRMFFIndexRecord 
{
	VO_U16		object_version;
	VO_U32		time_stamp;			//<MS>
	VO_U32		offset;				//file position
	VO_U32		packet_count;		//the packet number of the packet
} RMFFIndexRecord, *PRMFFIndexRecord;

/************************************************************************/
/* metadata section                                                     */
/************************************************************************/
//FOURCC_RM_Meta_Data
typedef struct tagRMFFMetaDataSectionHeader 
{
} RMFFMetaDataSectionHeader, *PRMFFMetaDataSectionHeader;

typedef struct tagRMFFPropListEntry 
{
	VO_U32		offset;				//offset for this indexed sub-property
	VO_U32		num_props_for_name;	//number of sub-properties that share the same name
} RMFFPropListEntry, *PRMFFPropListEntry;

typedef struct tagRMFFMetaDataProperty 
{
	VO_U32		size;
	VO_U32		type;				//MPT_XXX
	VO_U32		flags;				//MPT_XXX
	VO_U32		value_offset;		//offset to the value_length
	VO_U32		sub_properties_offset;	//offset to the su_properties_list
	VO_U32		num_sub_properties;
	VO_U32		name_length;
	VO_PBYTE	name;				//name of the property, string
	VO_U32		value_length;
	VO_PBYTE	value;				//value of the property
	RMFFPropListEntry*	sub_properties_list;
	tagRMFFMetaDataProperty*	sub_properties;
} RMFFMetaDataProperty, *PRMFFMetaDataProperty;

typedef struct tagRMFFMetaDataTag 
{
	VO_U32		object_id;			//FOURCC_RM_Meta_Data_Tag
	VO_U32		object_version;
	RMFFMetaDataProperty*	properties;
} RMFFMetaDataTag, *PRMFFMetaDataTag;

typedef struct tagRMFFMetaSectionFooter 
{
	VO_U32		object_id;			//FOURCC_RM_Meta_Data_Footer
	VO_U32		object_version;
	VO_U32		size;
} RMFFMetaSectionFooter, *PRMFFMetaSectionFooter;

typedef struct tagRMFFAudiSubstreamHdr
{
	VO_U16		usRAFormatVersion;     /* 3, 4, or 5 */
	VO_U16		usRAFormatRevision;    /* should be 0 */
	VO_U16		usHeaderBytes;         /* size of raheader info */
	VO_U16		usFlavorIndex;         /* compression type */
	VO_U32		ulGranularity;         /* size of one block of encoded data */
	VO_U32		ulTotalBytes;          /* total bytes of ra data */
	VO_U32		ulBytesPerMin;         /* data rate of encoded and interleaved data */
	VO_U32		ulBytesPerMin2;        /* data rate of interleaved or non-interleaved data */
	VO_U16		ulInterleaveFactor;    /* number of blocks per superblock */
	VO_U16		ulInterleaveBlockSize; /* size of each interleave block */
	VO_U16		ulCodecFrameSize;      /* size of each audio frame */
	VO_U32		ulUserData;            /* extra field for user data */
	VO_U32		ulSampleRate;          /* sample rate of decoded audio */
	VO_U32		ulActualSampleRate;    /* sample rate of decoded audio */
	VO_U16		ulSampleSize;          /* bits per sample in decoded audio */
	VO_U16		ulChannels;            /* number of audio channels in decoded audio */
	VO_U32		ulInterleaverID;       /* interleaver 4cc */
	VO_U32		ulCodecID;             /* codec 4cc */
	VO_U8		bIsInterleaved;        /* 1 if file has been interleaved */
	VO_U8		bCopyByte;             /* copy enable byte, if 1 allow copies (SelectiveRecord) */
	VO_U8		ucStreamType;          /* i.e. LIVE_STREAM, FILE_STREAM */
	VO_U8		ucScatterType;         /* the interleave pattern type 0==cyclic,1==pattern */
	VO_U32		ulNumCodecFrames;      /* number of codec frames in a superblock */
	VO_U32*		pulInterleavePattern;  /* the pattern of interleave if not cyclic */
	VO_U32		ulOpaqueDataSize;      /* size of the codec specific data */
	VO_PBYTE	pOpaqueData;           /* codec specific data */
	double	    dBlockDuration;        /* Duration in ms of audio "block" */
	VO_U32		ulLastSentEndTime;     /* Ending time of last sent audio frame */
	VO_PBYTE	pFragBuffer;           /* Intermediate buffer for reconstructing VBR packets */
	VO_U32		ulFragBufferSize;      /* Size of intermediate buffer */
	VO_U32		ulFragBufferAUSize;    /* Size of AU being reconstructed */
	VO_U32		ulFragBufferOffset;    /* Current offset within AU */
	VO_U32		ulFragBufferTime;      /* Timestamp of AU being reconstructed */
	VO_U32		ulSuperBlockSize;      /* ulInterleaveBlockSize * ulInterleaveFactor */
	VO_U32		ulSuperBlockTime;      /* dBlockDuration * ulInterleaveFactor */
	VO_U32		ulKeyTime;             /* Timestamp of keyframe packet */
	VO_PBYTE	pIBuffer;              /* Buffer holding interleaved blocks    */
	VO_PBYTE	pDBuffer;              /* Buffer holding de-interleaved blocks */
	VO_U32*		pIPresentFlags;        /* number of UINT32s: ulInterleaveBlockSize */
	VO_U32*		pDPresentFlags;        /* number of UINT32s: ulInterleaveBlockSize */
	VO_U32		ulBlockCount;          /* number of blocks currently in superblock */
	VO_U32*		pulGENRPattern;        /* Interleave pattern for GENR interleaver */
	VO_U32*		pulGENRBlockNum;
	VO_U32*		pulGENRBlockOffset;
	VO_PTR		lastPacket;
	VO_U8		bIsVBR : 1;
	VO_U8		bSeeked : 1;
	VO_U8		bLossOccurred : 1;
	VO_U8		bHasKeyTime : 1;       /* Do we have a time for the key slot? */
	VO_U8		bHasFrag : 1;
	VO_U8		bAdjustTimestamps : 1;
	VO_U8		bKnowIfAdjustNeeded : 1;
	VO_U8		bHasLastPacket : 1;
} RMFFAudiSubstreamHdr, *PRMFFAudiSubstreamHdr;

typedef struct tagRMFFVideoFormatInfo
{
	VO_U32		ulLength;
	VO_U32		ulMOFTag;
	VO_U32		ulSubMOFTag;
	VO_U16		usWidth;
	VO_U16		usHeight;
	VO_U16		usBitCount;
	VO_U16		usPadWidth;
	VO_U16		usPadHeight;
	VO_U32		ufFramesPerSecond;
	VO_U32		ulOpaqueDataSize;
	VO_PBYTE	pOpaqueData;
} RMFFVideoFormatInfo, *PRMFFVideoFormatInfo;

typedef struct tagRealTrackInfo
{
	RM_MEDIA_MIME_TYPES	nType;
	VO_U16				wStreamNum;
	VO_U32				dwDuration;
	VO_U32				dwBitrate;			//<Byte/Sec>
	VO_U32				dwMaxPacketSize;
	VO_U32				dwDataOffset;
	VO_U32				dwPropBufferSize;
	VO_PBYTE			pPropBuffer;
	tagRealTrackInfo*	next;
} RealTrackInfo, *PRealTrackInfo;

typedef struct tagRealLogicalStream 
{
	VO_U16		num_physical_streams;	//The number of physical streams that make up this logical stream
	VO_U32		duration;
	VO_U16*		physical_stream_numbers;
	VO_U32*		data_offsets;		//each stream's file position
} RealLogicalStream, *PRealLogicalStream;

typedef struct tagRealSeekPoint 
{
	VO_U32		time_stamp;			//<MS>
	VO_U32		offset;				//file position
} RealSeekPoint, *PRealSeekPoint;

#ifdef _VONAMESPACE
}
#endif

#endif	//__RM_File_Data_Struct_H