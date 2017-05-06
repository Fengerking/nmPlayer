	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		FlacFileDataStruct.h

Contains:	Data Structure Of FLAC file

Written by:	East

Reference:	FLAC - format.mht

Change History (most recent first):
2009-06-26		East			Create file

*******************************************************************************/
#ifndef __Flac_File_Data_Struct
#define __Flac_File_Data_Struct

#include "voType.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define FOURCC_fLaC					MAKEFOURCC('f', 'L', 'a', 'C')

/* BlockType
0 : STREAMINFO 
1 : PADDING 
2 : APPLICATION 
3 : SEEKTABLE 
4 : VORBIS_COMMENT 
5 : CUESHEET 
6 : PICTURE 
7-126 : reserved 
127 : invalid, to avoid confusion with a frame sync code */
#define FLAC_BLOCKTYPE_STREAMINFO		0x00
#define FLAC_BLOCKTYPE_PADDING			0x01
#define FLAC_BLOCKTYPE_APPLICATION		0x02
#define FLAC_BLOCKTYPE_SEEKTABLE		0x03
#define FLAC_BLOCKTYPE_VORBISCOMMENT	0x04
#define FLAC_BLOCKTYPE_CUESHEET			0x05
#define FLAC_BLOCKTYPE_PICTURE			0x06
#define FLAC_BLOCKTYPE_INVALID			0x7F
//others is reserved, you can skip them!!

#define FLAC_STREAM_INFO_BLOCK_SIZE		34

typedef struct FLAC_FRAME_HEADER {
	VO_BOOL	bBlockingStrategy;		// 0 : fixed-blocksize stream; frame header encodes the frame number; 1 : variable-blocksize stream; frame header encodes the sample number.
	VO_U32	blockSize;				// Block size in inter-channel samples.
	VO_U32	sampleRate;				// sample rate in Hz.
	VO_U16	channels;				// The number of channels (== number of subframes).
	VO_U16	bitsPerSample;			// The bits of a sample.
	union {
		VO_U32 frameNumber;		// frame number of each frame.
		VO_U64 sampleNumber;		// the first sample number in a frame
	} number;
} FLACFRAMEHEADER, * PFLACFRAMEHEADER;

#ifdef _VONAMESPACE
}
#endif
#endif	//__Flac_File_Data_Struct
