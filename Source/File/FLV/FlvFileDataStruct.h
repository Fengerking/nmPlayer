	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		FlashFileDataStruct.h

	Contains:	Data Struct Of Flash File Format(FLV/SWF)

	Written by:	East

	Reference:	SWF_FLV_Spec.pdf

	Change History (most recent first):
	2009-01-13		East			Create file

*******************************************************************************/
#ifndef _Flash_File_Data_Struct_H_
#define _Flash_File_Data_Struct_H_

//////////////////////////////////////////////////////////////////////////
//SWF not supported
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FLV
#define FLV_SCRIPT_DATA_STRING_LENGTH		1024
#define FLV_FLAG_AUDIO_PRESENT				0x4
#define FLV_FLAG_VIDEO_PRESENT				0x1

typedef struct tagFlvFileHeader 
{
	VO_U8		btSignature[3];		// Signature bytes, always 'FLV'
	VO_U8		btVersion;			// File version, for example, 0x01 for FLV version 1
	VO_U8		btTypeFlags;		// 0xRRRRRARV, R must be 0, A mean Audio present, V mean Video present
	VO_U32		dwDataOffset;		// Offset in bytes from start of file to start of body(size of header)
} FlvFileHeader, *PFlvFileHeader;

typedef enum
{
	FLV_TAGTYPE_UNKNOWN, 
	FLV_TAGTYPE_AUDIO, 
	FLV_TAGTYPE_VIDEO, 
	FLV_TAGTYPE_SCRIPT_DATA, 
} EFlvTagType;

typedef struct tagFlvTag 
{
	VO_U8		btTagType;			// 0x8 - audio; 0x9 - video; 0x12 - script data; others - reserved
	VO_U32		dwDataSize;			// 24bit, Length of the data
	VO_U32		dwTimeStamp;		// <MS>	
	VO_U32		dwStreamID;			// 24bit, always 0
	//...data
} FlvTag, *PFlvTag;

typedef struct tagScriptDataString 
{
	VO_U16		wLength;			//include zero_terminator, but actually it not include!!
	VO_PBYTE	pStringData;		//zero-terminated
} ScriptDataString, *PScriptDataString;

typedef struct tagScriptDataLongString 
{
	VO_U32		dwLength;			//include zero_terminator
	VO_PBYTE	pStringData;		//zero-terminated
} ScriptDataLongString, *PScriptDataLongString;

typedef struct tagFlvIndexEntry 
{
	VO_U32		dwTimeStamp;		//<MS>
	VO_U64		qwFilePos;			//file position
} FlvIndexEntry, *PFlvIndexEntry;

typedef struct tagFlvAudioInfo
{
	VO_U8		btCodecID;			//0 - uncompressed; 1 - ADPCM; 2 - MP3; 5 - Nellymoser 8kHz mono; 6 - Nellymoser.
	VO_U32		nSamplesPerSec;
	VO_U16		nChannels;
	VO_U16		wBitsPerSample;
	VO_U32		dwBytesPerSec;		//bit-rate

	VO_PBYTE	ptr_header_data;
	VO_U32		header_size;
} FlvAudioInfo, *PFlvAudioInfo;

typedef struct tagFlvVideoInfo
{
	VO_U8		btCodecID;			//2 - Sorenson H.263; 3 - Screen Video; 4 - On2 VP6; 5 - On2 VP6 with alpha channel; 6 - Screen video version 2.
	VO_S64		llAvgTimePerFrame;	//Average time per frame (100ns units)
	VO_U16		wHeight;
	VO_U16		wWidth;
	VO_U32		dwBytesPerSec;		//bit-rate

	VO_PBYTE	ptr_header_data;
	VO_U32		header_size;
} FlvVideoInfo, *PFlvVideoInfo;

//Sound Flags
//0xFFFFRRST
//F(Format): 0 - uncompressed; 1 - ADPCM; 2 - MP3; 5 - Nellymoser 8kHz mono; 6 - Nellymoser.
//R(Rate): 0 - 5.5kHz; 1 - 11kHz; 2 - 22kHz; 3 - 44kHz.
//S(Size): 0 - 8bit; 1 - 16bit.
//T(Type): 0 - Mono; 1 - Stereo.

//Video Flags
//0xFFFFCCCC
//F(Frame Type): 1 - key frame; 2 - inter frame; 3 - disposable inter frame(H263 only)
//C(Codec ID): 2 - Sorenson H.263; 3 - Screen Video; 4 - On2 VP6; 5 - On2 VP6 with alpha channel; 6 - Screen video version 2.
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//SCRIPTDATAOBJECT NAME
#define SDON_AUDIO_CODEC_ID 			"audiocodecid"
#define SDON_AUDIO_DATA_RATE 			"audiodatarate"
#define SDON_AUDIO_DELAY 				"audiodelay"
#define SDON_AUDIO_SAMPLE_RATE 			"audiosamplerate"
#define SDON_AUDIO_SAMPLE_SIZE 			"audiosamplesize"
#define SDON_AUDIO_SIZE 				"audiosize"

#define SDON_CAN_SEEK_TO_END 			"canSeekToEnd"
#define SDON_CUE_POINTS					"cuePoints"

#define SDON_DATA_SIZE 					"datasize"
#define SDON_DURATION 					"duration"

#define SDON_FRAME_RATE 				"framerate"
#define SDON_FILE_SIZE 					"filesize"
#define SDON_FILE_POSITIONS 			"filepositions"

#define SDON_HAS_AUDIO 					"hasAudio"
#define SDON_HAS_CUE_POINTS 			"hasCuePoints"
#define SDON_HAS_KEY_FRAMES				"hasKeyframes"
#define SDON_HAS_META_DATA 				"hasMetadata"
#define SDON_HAS_VIDEO 					"hasVideo"
#define SDON_HEIGHT 					"height"

#define SDON_KEY_FRAMES 				"keyframes"

#define SDON_LAST_KEY_FRAME_TIME_STAMP	"lastkeyframetimestamp"
#define SDON_LAST_TIME_STAMP 			"lasttimestamp"

#define SDON_META_DATA_CREATOR 			"metadatacreator"
#define SDON_META_DATA_DATE 			"metadatadate"

#define SDON_ON_LAST_SECOND 			"onLastSecond"
#define SDON_ON_META_DATA				"onMetaData"

#define SDON_STEREO 					"stereo"

#define SDON_TIMES 						"times"

#define SDON_VIDEO_CODEC_ID 			"videocodecid"
#define SDON_VIDEO_DATA_RATE 			"videodatarate"
#define SDON_VIDEO_SIZE 				"videosize"

#define SDON_WIDTH 						"width"

#define DECLARE_USE_FLV_GLOBAL_VARIABLE		static const VO_U32	s_nSamplesPerSec[4];
#define DEFINE_USE_FLV_GLOBAL_VARIABLE(cls)	const VO_U32 cls::s_nSamplesPerSec[4] = {5512, 11025, 22050, 44100};

//////////////////////////////////////////////////////////////////////////
#endif	//_Flash_File_Data_Struct_H_