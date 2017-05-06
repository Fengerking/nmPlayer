	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		ID3v2DataStruct.h

Contains:	Data Structure Of ID3v2

Written by:	East

Reference:	http://www.id3.org/

Change History (most recent first):
2009-06-19		East			Create file

*******************************************************************************/
#ifndef __ID3V2_Data_Struct
#define __ID3V2_Data_Struct

#include "voType.h"

#define ID3V2_HEADER_LEN		10
typedef struct tagID3v2Header
{
	VO_CHAR		szTag[3];		//ID3
	VO_U8		btVersion[2];
	VO_U8		btFlags;
	VO_U8		btSize[4];
} ID3v2Header, *PID3v2Header;

typedef struct tagID3v2ExtHeader
{
	VO_U8		btSize[4];
	VO_U8		btFlags[2];
	VO_U8		btPaddingSize[4];
} ID3v2ExtHeader, *PID3v2ExtHeader;

#define VO_ID3V2_FRAME_HEADER_LEN_20BEFORE		6
#define VO_ID3V2_FRAME_HEADER_LEN				10
typedef struct tagID3v2FrameHeader
{
	VO_U8		btID[4];		//version 2.0 and before only use first 3 bytes
	VO_U8		btSize[4];		//version 2.0 and before only use last 3 bytes

	/*version 2.0 and before no flag
	0abc0000 0h00kmnp
	XXXXXXXX XXXXXXXX
	a - Tag alter preservation
	b - File alter preservation
	c - Read only
	h - Grouping identity
	k - Compression
	m - Encryption
	n - unsynchronization
	p - Data length indicator(0: There is no Data Length Indicator; 1: A data length Indicator has been added to the frame.*/
	VO_U8		btFlags[2];
} ID3v2FrameHeader, *PID3v2FrameHeader;

typedef struct tagID3v2Frame
{
	ID3v2FrameHeader	Header;
	VO_PBYTE			pData;
	tagID3v2Frame*		pNext;
} ID3v2Frame, *PID3v2Frame;

//Text encoding[Ref Doc: http://www.id3.org/id3v2.4.0-structure]
//$00   ISO-8859-1 [ISO-8859-1]. Terminated with $00.
//$01   UTF-16 [UTF-16] encoded Unicode [UNICODE] with BOM. All strings in the same frame SHALL have the same byteorder. Terminated with $00 00.
//$02   UTF-16BE [UTF-16] encoded Unicode [UNICODE] without BOM. Terminated with $00 00.
//$03   UTF-8 [UTF-8] encoded Unicode [UNICODE]. Terminated with $00.
//BOM: FF FE or FE FF
#define TEXT_ENCODING_ANSI							0
#define TEXT_ENCODING_UTF16_WITH_BOM				1		// UTF-16 stream with "FF FE" or "FE FF" as start bytes
#define TEXT_ENCODING_UTF16_WITHOUT_BOM				2		// UTF-16 stream without BOM(Byte Order Marks)
#define TEXT_ENCODING_UTF8							3
#define TEXTENCODING_IS_UNICODE(text_encoding)		(0 != text_encoding)
#define TEXTENCODING_IS_BOMINHEAD(text_encoding)	(1 == text_encoding)
#define TEXTENCODING_IS_MULTIBYTE(text_encoding)	(0 == text_encoding || 3 == text_encoding)
#define TEXTENCODING_IS_WIDECHAR(text_encoding)		(1 == text_encoding || 2 == text_encoding)
#define TEXTENCODING_IS_UTF8(text_encoding)			(3 == text_encoding)

#define CALCULATE_SIZE_SYNCHSAFE(size_array)		((size_array[0] << 21) | (size_array[1] << 14) | (size_array[2] << 7) | size_array[3])
#define CALCULATE_SIZE(size_array)					((size_array[0] << 24) | (size_array[1] << 16) | (size_array[2] << 8) | size_array[3])

#define FOURCC_TITLE								MAKEFOURCC('T', 'I', 'T', '2')
#define FOURCC_TITLE_20BEFORE						MAKEFOURCC('T', 'T', '2', '\0')
#define FOURCC_ARTIST								MAKEFOURCC('T', 'P', 'E', '1')
#define FOURCC_ARTIST_20BEFORE						MAKEFOURCC('T', 'P', '1', '\0')
#define FOURCC_ALBUM								MAKEFOURCC('T', 'A', 'L', 'B')
#define FOURCC_ALBUM_20BEFORE						MAKEFOURCC('T', 'A', 'L', '\0')
#define FOURCC_GENRE								MAKEFOURCC('T', 'C', 'O', 'N')
#define FOURCC_GENRE_20BEFORE						MAKEFOURCC('T', 'C', 'O', '\0')
#define FOURCC_COMPOSER								MAKEFOURCC('T', 'C', 'O', 'M')
#define FOURCC_COMPOSER_20BEFORE					MAKEFOURCC('T', 'C', 'M', '\0')
#define FOURCC_TRACK								MAKEFOURCC('T', 'R', 'C', 'K')
#define FOURCC_TRACK_20BEFORE						MAKEFOURCC('T', 'R', 'K', '\0')
#define FOURCC_PICTURE								MAKEFOURCC('A', 'P', 'I', 'C')
#define FOURCC_PICTURE_20BEFORE						MAKEFOURCC('P', 'I', 'C', '\0')
#define FOURCC_FILEID								MAKEFOURCC('U', 'F', 'I', 'D')
#define FOURCC_FILEID_20BEFORE						MAKEFOURCC('U', 'F', 'I', '\0')
#define FOURCC_COMMENT								MAKEFOURCC('C', 'O', 'M', 'M')
#define FOURCC_COMMENT_20BEFORE						MAKEFOURCC('C', 'O', 'M', '\0')
#define FOURCC_DISK									MAKEFOURCC('T', 'P', 'O', 'S')
#define FOURCC_DISK_20BEFORE						MAKEFOURCC('T', 'P', 'A', '\0')
#define FOURCC_PUBLISHER							MAKEFOURCC('T', 'P', 'U', 'B')
#define FOURCC_PUBLISHER_20BEFORE					MAKEFOURCC('T', 'P', 'B', '\0')
#define FOURCC_ISRC									MAKEFOURCC('T', 'S', 'R', 'C')
#define FOURCC_ISRC_20BEFORE						MAKEFOURCC('T', 'R', 'C', '\0')
#define FOURCC_YEAR									MAKEFOURCC('T', 'Y', 'E', 'R')
#define FOURCC_YEAR_20BEFORE						MAKEFOURCC('T', 'Y', 'E', '\0')
#define FOURCC_BAND									MAKEFOURCC('T', 'P', 'E', '2')
#define FOURCC_BAND_20BEFORE						MAKEFOURCC('T', 'P', '2', '\0')
#define FOURCC_CONDUCTOR							MAKEFOURCC('T', 'P', 'E', '3')
#define FOURCC_CONDUCTOR_20BEFORE					MAKEFOURCC('T', 'P', '3', '\0')
#define FOURCC_REMIXER								MAKEFOURCC('T', 'P', 'E', '4')
#define FOURCC_REMIXER_20BEFORE						MAKEFOURCC('T', 'P', '4', '\0')
#define FOURCC_LYRICIST								MAKEFOURCC('T', 'E', 'X', 'T')
#define FOURCC_LYRICIST_20BEFORE					MAKEFOURCC('T', 'X', 'T', '\0')
#define FOURCC_RADIOSTATION							MAKEFOURCC('T', 'R', 'S', 'N')
#define FOURCC_ORIGRELYEAR							MAKEFOURCC('T', 'D', 'O', 'R')
#define FOURCC_ORIGRELYEAR_30						MAKEFOURCC('T', 'O', 'R', 'Y')
#define FOURCC_ORIGRELYEAR_20BEFORE					MAKEFOURCC('T', 'O', 'R', '\0')
#define FOURCC_OWNER								MAKEFOURCC('T', 'O', 'W', 'N')
#define FOURCC_ENCODER								MAKEFOURCC('T', 'E', 'N', 'C')
#define FOURCC_ENCODER_20BEFORE						MAKEFOURCC('T', 'E', 'N', '\0')
#define FOURCC_ENCSETTING							MAKEFOURCC('T', 'S', 'S', 'E')
#define FOURCC_ENCSETTING_20BEFORE					MAKEFOURCC('T', 'S', 'S', '\0')
#define FOURCC_COPYRIGHT							MAKEFOURCC('T', 'C', 'O', 'P')
#define FOURCC_COPYRIGHT_20BEFORE					MAKEFOURCC('T', 'C', 'R', '\0')
#define FOURCC_ORIGARTIST							MAKEFOURCC('T', 'O', 'P', 'E')
#define FOURCC_ORIGARTIST_20BEFORE					MAKEFOURCC('T', 'O', 'A', '\0')
#define FOURCC_ORIGALBUM							MAKEFOURCC('T', 'O', 'A', 'L')
#define FOURCC_ORIGALBUM_20BEFORE					MAKEFOURCC('T', 'O', 'T', '\0')
#define FOURCC_COMPILATION							MAKEFOURCC('T', 'C', 'M', 'P')

#endif	//__ID3V2_Data_Struct