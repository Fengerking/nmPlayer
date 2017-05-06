/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

/*******************************************************************************
File:		AviDataStruct.h

Contains:	Data Struct Of AVI File

Written by:	East

Reference:	OpenDML AVI File Format Extensions

Change History (most recent first):
2006-09-20		East			Create file

*******************************************************************************/
#ifndef __AVI_Data_Struct_H
#define __AVI_Data_Struct_H

#include "fMacros.h"

#ifndef FOURCC_RIFF
#define FOURCC_RIFF					MAKEFOURCC('R', 'I', 'F', 'F')
#endif	//FOURCC_RIFF
#define FOURCC_AVI					MAKEFOURCC('A', 'V', 'I', ' ')
#ifndef FOURCC_LIST
#define FOURCC_LIST					MAKEFOURCC('L', 'I', 'S', 'T')
#endif	//FOURCC_LIST
#define FOURCC_hdrl					MAKEFOURCC('h', 'd', 'r', 'l')		//AVI header LIST
#define FOURCC_avih					MAKEFOURCC('a', 'v', 'i', 'h')		//Main AVI Header
#define FOURCC_strl					MAKEFOURCC('s', 't', 'r', 'l')		//AVI Stream LIST
#define FOURCC_strh					MAKEFOURCC('s', 't', 'r', 'h')		//AVI Stream Header
#define FOURCC_strf					MAKEFOURCC('s', 't', 'r', 'f')		//AVI Stream Format
#define FOURCC_strd					MAKEFOURCC('s', 't', 'r', 'd')		//AVI Stream Decoder
#define FOURCC_strn					MAKEFOURCC('s', 't', 'r', 'n')		//AVI Stream Name
#define FOURCC_indx					MAKEFOURCC('i', 'n', 'd', 'x')		//'indx' index
#define FOURCC_idx1					MAKEFOURCC('i', 'd', 'x', '1')		//'idx1' index
#define FOURCC_JUNK					MAKEFOURCC('J', 'U', 'N', 'K')		//Padding
#define FOURCC_JUNQ					MAKEFOURCC('J', 'U', 'N', 'Q')		//Padding
#define FOURCC_movi					MAKEFOURCC('m', 'o', 'v', 'i')		//Media Data LIST
#define FOURCC_rec					MAKEFOURCC('r', 'e', 'c', ' ')		//Sub Media Data LIST
#define FOURCC_IDIT					MAKEFOURCC('I', 'D', 'I', 'T')		//Time and Date
#define FOURCC_INFO					MAKEFOURCC('I', 'N', 'F', 'O')		//Info LIST
#define FOURCC_odml					MAKEFOURCC('o', 'd', 'm', 'l')		//Extended AVI Header LIST
#define FOURCC_dmlh					MAKEFOURCC('d', 'm', 'l', 'h')		//Extended AVI Header
#define FOURCC_vprp					MAKEFOURCC('v', 'p', 'r', 'p')		//Video Properties Header

#define TWOCC_db					MAKETWOCC('d', 'b')					//Uncompress Video
#define TWOCC_dc					MAKETWOCC('d', 'c')					//Compress Video
#define TWOCC_dd					MAKETWOCC('d', 'd')					//DRM video
#define TWOCC_pc					MAKETWOCC('p', 'c')					//Palette
#define TWOCC_wb					MAKETWOCC('w', 'b')					//Audio
#define TWOCC_ix					MAKETWOCC('i', 'x')					//Index
//////////////////////////////////////////////////////////////////////////
//Media Data Type
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#define FOURCC_auds					MAKEFOURCC('a', 'u', 'd', 's')		//Audio Stream
#define FOURCC_vids					MAKEFOURCC('v', 'i', 'd', 's')		//Video Stream
#define FOURCC_mids					MAKEFOURCC('m', 'i', 'd', 's')		//Midi Stream
#define FOURCC_txts					MAKEFOURCC('t', 'x', 't', 's')		//Text Stream
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//Info Type
//////////////////////////////////////////////////////////////////////////
#define FOURCC_Info_IARL			MAKEFOURCC('I', 'A', 'R', 'L')
#define FOURCC_Info_IART			MAKEFOURCC('I', 'A', 'R', 'T')
#define FOURCC_Info_ICMS			MAKEFOURCC('I', 'C', 'M', 'S')
#define FOURCC_Info_ICMT			MAKEFOURCC('I', 'C', 'M', 'T')
#define FOURCC_Info_ICOP			MAKEFOURCC('I', 'C', 'O', 'P')
#define FOURCC_Info_ICRD			MAKEFOURCC('I', 'C', 'R', 'D')
#define FOURCC_Info_ICRP			MAKEFOURCC('I', 'C', 'R', 'P')
#define FOURCC_Info_IDIM			MAKEFOURCC('I', 'D', 'I', 'M')
#define FOURCC_Info_IDPI			MAKEFOURCC('I', 'D', 'P', 'I')
#define FOURCC_Info_IENG			MAKEFOURCC('I', 'E', 'N', 'G')
#define FOURCC_Info_IGNR			MAKEFOURCC('I', 'G', 'N', 'R')
#define FOURCC_Info_IKEY			MAKEFOURCC('I', 'K', 'E', 'Y')
#define FOURCC_Info_ILGT			MAKEFOURCC('I', 'L', 'G', 'T')
#define FOURCC_Info_IMED			MAKEFOURCC('I', 'M', 'E', 'D')
#define FOURCC_Info_INAM			MAKEFOURCC('I', 'N', 'A', 'M')
#define FOURCC_Info_IPLT			MAKEFOURCC('I', 'P', 'L', 'T')
#define FOURCC_Info_IPRD			MAKEFOURCC('I', 'P', 'R', 'D')
#define FOURCC_Info_ISBJ			MAKEFOURCC('I', 'S', 'B', 'J')
#define FOURCC_Info_ISFT			MAKEFOURCC('I', 'S', 'F', 'T')
#define FOURCC_Info_ISHP			MAKEFOURCC('I', 'S', 'H', 'P')
#define FOURCC_Info_ISRC			MAKEFOURCC('I', 'S', 'R', 'C')
#define FOURCC_Info_ISRF			MAKEFOURCC('I', 'S', 'R', 'F')
#define FOURCC_Info_ITCH			MAKEFOURCC('I', 'T', 'C', 'H')

//////////////////////////////////////////////////////////////////////////
//AVI Flag
//////////////////////////////////////////////////////////////////////////
#define AVI_Flag_HasIndex		0x00000010	//if has index at end of file
#define AVI_Flag_MustUseIndex	0x00000020
#define AVI_Flag_IsInterleaved	0x00000100
#define AVI_Flag_WasCaptureFile	0x00010000
#define AVI_Flag_CopyRighted	0x00020000

typedef struct tagAviObject {
	VO_U32	fcc;
	VO_U32	size;
} AviObject, *PAviObject;

typedef struct tagAviHeader {
	VO_U32	dwMicroSecPerFrame;			//video frame interval, mill second
	VO_U32	dwMaxBytesPerSec;			//max bytes per second
	VO_U32	dwPaddingGranularity;		//padding granularity
	VO_U32	dwFlags;					//global flag, AVI_Flag_XXX composition
	VO_U32	dwTotalFrames;				//total frames
	VO_U32	dwInitialFrames;			//initial frames of interleave mode, if not interleave mode, it should be 0
	VO_U32	dwStreams;					//number of streams
	VO_U32	dwSuggestedBufferSize;		//when read file, suggest using this buffer size, normally, it is max chunk data size
	VO_U32	dwWidth;					//width
	VO_U32	dwHeight;					//height
	VO_U32	dwReserved[4];
//	VO_U32	dwScale;
//	VO_U32	dwRate;
//	VO_U32	dwStart;
//	VO_U32	dwLength;
} AviHeader, *PAviHeader;

typedef struct tagAviStreamHeader {
	VO_U32	fccStreamType;				//stream type
	VO_U32	fccHandler;					//decoder's FOURCC
	VO_U32	dwFlags;					//flag, 1.if output; 2.if palette modify
	VO_U16	wPriority;					//Priority
	VO_U16	wLanguage;					//Language
	VO_U32	dwInitialFrames;			//initial frames of interleave mode, if not interleave mode, it should be 0
	VO_U32	dwScale;					//time scale
	VO_U32	dwRate;						//bit rate
	VO_U32	dwStart;					//start time
	VO_U32	dwLength;					//length, relative to dwScale and dwRate
	VO_U32	dwSuggestedBufferSize;		//suggest buffer size, same to max sample size
	VO_U32	dwQuality;					//quality, 0-10000
	VO_U32	dwSampleSize;				//sample size
/*	struct 
	{
		short int left;
		short int top;
		short int right;
		short int bottom;
	} rcFrame;							//the position that this stream show in main window
	*/
} AviStreamHeader, *PAviStreamHeader;

typedef struct tagAviPaletteChange {
	VO_U8			bFirstEntry;
	VO_U8			bNumEntries;
	VO_U16			wFlags;
	VO_U8			bNewPalette[4];
} AviPaletteChange, *PAviPaletteChange;

//'idx1'
typedef struct tagAviOriginalIndexEntry {
	VO_U32	dwChunkId;
	VO_U32	dwFlags;
	VO_U32	dwOffset;
	VO_U32	dwSize;
} AviOriginalIndexEntry, *PAviOriginalIndexEntry;

//////////////////////////////////////////////////////////////////////////
//IndexType codes
//////////////////////////////////////////////////////////////////////////
#define AVI_INDEX_OF_INDEXES	0x00	// when each entry in aIndex array points to an index chunk
#define AVI_INDEX_OF_CHUNKS		0x01	// when each entry in aIndex array   points to a chunk in the file
#define AVI_INDEX_IS_DATA		0x80	// when each entry is aIndex is really the data
//////////////////////////////////////////////////////////////////////////
//IndexSubtype codes for INDEX_OF_CHUNKS
//////////////////////////////////////////////////////////////////////////
#define AVI_INDEX_2FIELD		0x01	// when fields within frames are also indexed

//'indx'
typedef struct tagAviSuperIndexChunk {
	VO_U16	wLongsPerEntry;		//must be 4 (size of each entry in aIndex array)
	VO_U8	bIndexSubType;		//must be 0
	VO_U8	bIndexType;			//must be AVI_INDEX_OF_CHUNKS
	VO_U32	dwEntriesInUse;		//number of entries in aIndex array that are used
	VO_U32	dwChunkId;			//fcc of what is indexed
	VO_U32	dwReserved[3];		//must be 0
} AviSuperIndexChunk, *PAviSuperIndexChunk;

typedef struct tagAviSuperIndexEntry {
	VO_U64		ullFilePos;
	VO_U32		dwSize;
	VO_U32		dwDuration;
} AviSuperIndexEntry, *PAviSuperIndexEntry;

typedef struct tagAviStandardIndexChunk {
	VO_U16	wLongsPerEntry;		//must be 2 (size of each entry in aIndex array)
	VO_U8	bIndexSubType;		//must be 0 or AVI_INDEX_2FIELD
	VO_U8	bIndexType;			//must be AVI_INDEX_OF_INDEXES
	VO_U32	dwEntriesInUse;		//number of entries in aIndex array that are used
	VO_U32	dwChunkId;			//fcc of what is indexed
	VO_U64	ullBaseOffset;		//all dwOffsets in aIndex array are relative to this
	VO_U32	dwReserved;			//must be 0
} AviStandardIndexChunk, *PAviStandardIndexChunk;

typedef struct tagAviStandardIndexEntry {
	VO_U32	dwOffset;				// qwBaseOffset + this is absolute file offset
	//0xKDSSSSSS SSSSSSSS SSSSSSSS SSSSSSSS
	//K: set if NOT a key frame, specified by AVI spec
	//D: set if IS 'dd' frame, specified by VisualOn
	VO_U32	dwSize;
} AviStandardIndexEntry, *PAviStandardIndexEntry;

#endif	//__AVI_Data_Struct_H