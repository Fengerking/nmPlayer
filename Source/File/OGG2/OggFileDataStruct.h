/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		OggFileDataStruct.h

Contains:	Data Struct Of OGG File

Written by:	East

Reference:	Vorbis_I_spec.pdf; www.xiph.org/ogg/doc/rfc3533.txt

Change History (most recent first):
2007-09-04		East			Create file
*******************************************************************************/
#ifndef __OGG_File_Data_Struct_
#define __OGG_File_Data_Struct_

#include "voType.h"

#define OGG_PAGE_FLAG_SIZE					4
#define OGG_PAGE_HEAD_SIZE					27
const VO_BYTE ogg_page_flag[] = {'O', 'g', 'g', 'S'};

typedef struct tagOggPageHead 
{
	VO_U32		capture_pattern;			//signify the beginning of page, must be "OggS"
	VO_BYTE		stream_structure_version;	//the version number of the OGG file format used in this stream
	/*
	BYTE: 0XABCDEFGH
	H: 0 - page contains a fresh packet; 1 - pages contains data of a packet continued from the previous page.
	G: 0 - not bos; 1 - bos(first page of a logical bitstream).
	E: 0 - not eos; 1 - eos(last page of a logical bitstream).
	*/
	VO_BYTE		header_type_flag;			//specific type of this page
	VO_U64		granule_position;			//position information
	VO_U32		bitstream_serial_number;	//the unique serial number of the logical bitstream
	VO_U32		page_sequence_number;		//the sequence number of the page, identify page loss
	VO_U32		CRC_checksum;				//CRC checksum of the page
	VO_BYTE		number_page_segments;		//the number of segment entries encoded in the segment table
} OggPageHead, *POggPageHead;

#define OGG_HEAD_PACKET_FLAG_SIZE			6
const VO_BYTE OGG_HEAD_PACKET_FLAG[] = {'v', 'o', 'r', 'b', 'i', 's'};

const VO_BYTE THEORA_HEAD_PACKET_FLAG[] = {'t', 'h', 'e', 'o', 'r', 'a'};
typedef struct tagOggHeadPacket 
{
	VO_BYTE		packet_type;
	VO_BYTE		packet_flag[OGG_HEAD_PACKET_FLAG_SIZE];			//signify this is head packet, must be "vorbis"
} OggHeadPacket, *POggHeadPacket;

typedef struct tagOggIdentificationHeader 
{
	VO_U32		vorbis_version;
	VO_BYTE		audio_channel;
	VO_U32		audio_sample_rate;
	VO_U32		bitrate_maximum;
	VO_U32		bitrate_nominal;
	VO_U32		bitrate_minimum;
	/*
	BYTE: 0xABCDEFGH
	ABCD: blocksize_0
	EFGH: blocksize_1
	*/
	VO_BYTE		block_size;
	/*
	BYTE: 0xABCDEFGH
	H: value
	*/
	VO_BYTE		framing_flag;
} OggIdentificationHeader, *POggIdentificationHeader;

typedef struct tagOggCommentHeader 
{
	VO_U32		vendor_length;
	VO_PBYTE	vendor_string;	//UTF-8 string
	VO_U32		user_comment_list_length;
	VO_BYTE		framing_bit;	//if unset or end-of-packet, ERROR
} OggCommentHeader, *POggCommentHeader;

struct VORBISFORMAT2 {
	VO_U32 Channels;
	VO_U32 SamplesPerSec;
	VO_U32 BitsPerSample;
	VO_U32 HeaderSize[3]; // Sizes of three init packets
};

#endif	//__OGG_File_Data_Struct_
