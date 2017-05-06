	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		Mp3FileDataStruct.h

Contains:	Data Structure Of MP3 file

Written by:	East

Reference:	1, CodeProject MPEG Audio Frame Header_ Free source code and programming help.mht
			2, MPEG AUDIO FRAME HEADER.mht

Change History (most recent first):
2009-06-22		East			Create file

*******************************************************************************/
#ifndef __Mp3_File_Data_Struct
#define __Mp3_File_Data_Struct

#include "voType.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define FOURCC_Xing					MAKEFOURCC('X', 'i', 'n', 'g')
#define FOURCC_Info					MAKEFOURCC('I', 'n', 'f', 'o')
#define FOURCC_VBRI					MAKEFOURCC('V', 'B', 'R', 'I')
#define FOURCC_LAME					MAKEFOURCC('L', 'A', 'M', 'E')
#define FOURCC_GOGO					MAKEFOURCC('G', 'O', 'G', 'O')

typedef enum
{
	VO_MP3VBRHEADER_NONE			= 0X00000000,   /*!< no VRB header */
	VO_MP3VBRHEADER_INFO			= 0X00000001,   /*!< XING VBR header('Info') */
	VO_MP3VBRHEADER_XING			= 0X00000002,   /*!< XING VBR header('Xing'') */
	VO_MP3VBRHEADER_VBRI			= 0x00000003,	/*!< VBRI VBR header('VBRI') */
	VO_MP3VBRHEADER_TYPE_MAX		= VO_MAX_ENUM_VALUE
} VO_MP3VBRHEADER_TYPE;

#define XING_FRAMES_FLAG			0x0001
#define XING_BYTES_FLAG				0x0002
#define XING_TOC_FLAG				0x0004
#define XING_QUALITY_INDICATOR_FLAG	0x0008
#define MPA_HEADER_SIZE				4	// MPEG-Audio Header Size 32bit

typedef enum
{
	MPA_VERSION_25 = 0, 
	MPA_VERSION_Reserved, 
	MPA_VERSION_2, 
	MPA_VERSION_1, 
	MPA_VERSION_MAX		= VO_MAX_ENUM_VALUE
} MPA_VERSION;

typedef enum
{
	MPA_LAYER_1 = 0, 
	MPA_LAYER_2, 
	MPA_LAYER_3, 
	MPA_LAYER_Reserved, 
	MPA_LAYER_MAX		= VO_MAX_ENUM_VALUE
} MPA_LAYER;

typedef enum
{
	MPA_CHANNEL_Stereo = 0, 
	MPA_CHANNEL_JointStereo, 
	MPA_CHANNEL_Dual, 
	MPA_CHANNEL_Single, 
	MPA_CHANNEL_MAX		= VO_MAX_ENUM_VALUE
} MPA_CHANNEL;

typedef struct _EncoderDelays {
	VO_U32 X;		// number of start encoder delay samples
	VO_U32 Y;		// number of end padding 0-samples
} EncoderDelays;

#define DECLARE_USE_MP3_GLOBAL_VARIABLE\
	static const VO_U32	s_dwMaxRange;\
	static const VO_U32	s_dwTolerance;\
	static const VO_U32	s_dwSamplingRates[4][3];\
	static const VO_U32	s_dwBitrates[2][3][15];\
	static const VO_U32	s_dwSamplesPerFrames[2][3];\
	static const VO_U32	s_dwCoefficients[2][3];\
	static const VO_U32	s_dwSideinfoSizes[2][3][2];\
	static const VO_U32	s_dwXINGOffsets[2][2];

// tolerance range, look at expected offset +/- m_dwTolerance for subsequent frames
// max. range where to look for frame sync
// sampling rates in hertz: 1. index = MPEG Version ID, 2. index = sampling rate index
// bitrates: 1. index = LSF, 2. index = Layer, 3. index = bitrate index
// Samples per Frame: 1. index = LSF, 2. index = Layer
// Samples per Frame / 8
// needed later for CRC check
// sideinformation size: 1.index = LSF, 2. index = Layer, 3. index = mono
// XING Header offset: 1. index = lsf, 2. index = mono
#define DEFINE_USE_MP3_GLOBAL_VARIABLE(cls)\
const VO_U32 cls::s_dwTolerance = 3;\
const VO_U32 cls::s_dwMaxRange = 0x4000;\
const VO_U32 cls::s_dwSamplingRates[4][3] = \
{\
	{11025, 12000, 8000,  },\
	{0,     0,     0,     },\
	{22050, 24000, 16000, },\
	{44100, 48000, 32000  }	\
};\
const VO_U32 cls::s_dwBitrates[2][3][15] = \
{\
	{\
		{0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},\
		{0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},\
		{0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,}\
	},\
	{\
		{0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},\
		{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},\
		{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,}\
	}\
};\
const VO_U32 cls::s_dwSamplesPerFrames[2][3] = \
{\
	{\
		384,\
		1152,\
		1152\
	},\
	{\
		384,\
		1152,\
		576\
	}\
};\
const VO_U32 cls::s_dwCoefficients[2][3] = \
{\
	{\
		48,\
		144,\
		144\
	},\
	{\
		48,\
		144,\
		72\
	}\
};\
const VO_U32 cls::s_dwSideinfoSizes[2][3][2] = \
{\
	{\
		{0,0},\
		{0,0},\
		{9,17}\
	},\
	{\
		{0,0},\
		{0,0},\
		{17,32}\
	}\
};\
const VO_U32 cls::s_dwXINGOffsets[2][2] = \
{\
	{32 + MPA_HEADER_SIZE,	17 + MPA_HEADER_SIZE},\
	{17 + MPA_HEADER_SIZE,	9 + MPA_HEADER_SIZE}\
};

#ifdef _VONAMESPACE
}
#endif

#endif	//__Mp3_File_Data_Struct
