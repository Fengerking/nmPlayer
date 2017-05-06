	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		H264RawDataFileDataStruct.h

	Contains:	H264 raw data file data structure header file.

	Written by:	Rodney Zhang

	Change History (most recent first):
	2011-10-25		Rodney		Create file

*******************************************************************************/


#ifndef __H265RawData_File_Data_Struct
#define __H265RawData_File_Data_Struct

#include "voType.h"


#define H265_RAW_DATA_FRAME_DURATION		24	// 24 frame rate
#define LOWEST_FRAME_FREQUENCY_THRESHOLD	5		// It defines the present times of frame period.
#define H265_RAW_DATA_FRAME_RATE			24		// 24 frame rate


// H264 raw data file relevant data types
#define LEADING_ZERO_8BITS				0x00
#define ZERO_BYTE						0x00
//#define START_CODE_PREFIX_ONE_3BYTES	0x000001
//#define FOUR_BYTE_SEQUENCE				0x00000001
#define TRAILING_ZERO_8BITS				0x00 

#define BITS_24							24
#define BITS_32							32

//#define STREAM_BUFFER_SIZE				0x9000
//#define SAMPLE_ALIGNMENT_SIZE			0x9000

#define H265_STREAM_BUFFER_SIZE				0x9000
#define H265_SAMPLE_ALIGNMENT_SIZE			0x9000

typedef struct _H265NALUNIT {
	//VO_U64 bsNALstart;					// start position of each byte stream NAL unit
	//VO_U32 numBytesInBSNAL;				// number of bytes in each byte stream NAL unit
	VO_U64 NALstart;					// start position of each NAL unit
	VO_U32 numBytesInNAL;				// number of bytes in a NAL unit
} H265NALUNIT, *PH265NALUNIT;

#endif	// __H265RawData_File_Data_Struct