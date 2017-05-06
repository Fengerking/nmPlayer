/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		ISS_ManifestParser.h

	Contains:	ISS_ManifestParser class file

	Written by:	Aiven

	Change History (most recent first):
	2012-08-14		Aiven			Create file

*******************************************************************************/
#ifndef _ISS_MANIFESTPARSER_H_
#define _ISS_MANIFESTPARSER_H_

#include "voVideo.h"
#include "voAudio.h"
#include "list_T.h"
#include "voXMLLoad.h"
#include "voSource2.h"
#include "voLiveSourcePlus.h"
#include "voDSType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


#define BYTE_1_REP          0x80   
#define BYTE_2_REP          0x800 
#define BYTE_3_REP          0x10000 
#define BYTE_4_REP          0x200000 

#define SIGMASK_1_1         0x80
#define SIGMASK_2_1         0xe0
#define SIGMASK_3_1         0xf0
#define SIGMASK_4_1         0xf8

#if !defined __VO_WCHAR_T__
#define __VO_WCHAR_T__
typedef unsigned short vowchar_t;
#endif

// Smooth Streaming Manifest file Tag
#define TAG_SMOOTH_STREAMING_MEDIA	"SmoothStreamingMedia"
#define TAG_MAJOR_VERSION               		"MajorVersion"
#define TAG_MINOR_VERSION               		"MinorVersion"
#define TAG_DURATION					"Duration"
#define TAG_ISLIVE						"IsLive"
#define TAG_STREAM_INDEX				"StreamIndex"
#define TAG_TYPE	                    				"Type"
#define TAG_NAME	                    				"Name"
#define TAG_SUBTYPE						"Subtype"
#define TAG_CHUNKS						"Chunks"
#define TAG_URL							"Url"
#define TAG_LANGUAGE					"Language"
#define TAG_QUALITY_LEVEL				"QualityLevel"
#define TAG_BITRATE						"Bitrate"
#define TAG_FOUR_CC						"FourCC"
#define TAG_WIDTH_V1					"Width"
#define TAG_HEIGHT_V1					"Height"
#define TAG_WIDTH_V2					"MaxWidth"
#define TAG_HEIGHT_V2					"MaxHeight"
#define TAG_CODEC_PRIVATE_DATA			"CodecPrivateData"
#define TAG_WAVE_FORMAT_EX				"WaveFormatEx"
#define TAG_AUDIO_TAG					"AudioTag"
#define TAG_CHANNELS					"Channels"
#define TAG_SIMPLE_RATE					"SamplingRate"
#define TAG_BIT_PER_SAMPLE				"BitsPerSample"
#define TAG_PACKET_SIZE					"PacketSize"
#define TAG_TIME_SCALE					"TimeScale"

#define TAG_LOOK_AHEAD_FRAGMENT_COUNT	"LookAheadFragmentCount"
#define TAG_DVR_WINDOW_LENGTH			"DVRWindowLength"

#define TAG_C							"c"
#define TAG_T							"t"
#define TAG_N							"n"
#define TAG_D							"d"


//PIFF
#define TAG_PIFF_PROTECTION				"Protection"
#define TAG_PIFF_PROTECTIONHEADER	 	"ProtectionHeader"
#define TAG_PIFF_SYSTEMID				"SystemID"
// typedef struct
// {
// 	VO_BOOL		isUsePiff;
// 	VO_BYTE		systemID[16];
// 	VO_U32		dataSize;
// 	VO_BYTE		*data;
// }ProtectionHeader;
// Smooth Streaming

#define STREAM_NUMBER	2
#define VIDEO_DURATION	20000000
#define DEFAULT_TIME_SCALE 10000000


#define AUDIO_TAG_AAC	0x00ff
#define AUDIO_TAG_MP3	0x0085
#define AUDIO_TAG_WMA	0x0161

enum VOSMTH_DRM_TYPE
{
	VOSMTH_DRM_Discretix_PlayReady = 0,
	VOSMTH_DRM_COMMON_PlayReady =100
};

struct VO_AUDIO_FROMAT_EX
{
	VO_AUDIO_FORMAT audio_format;
	VO_S32	samplerate_index;
};


struct QualityLevel
{
	VO_S64	bitrate;	
	VO_CHAR	fourCC[8];
	VO_BYTE	codecPrivateData[256];
	VO_S32	length_CPD;

	union
	{
		VO_VIDEO_FORMAT video_info;
		VO_AUDIO_FROMAT_EX audio_info;
		VO_SOURCE2_SUBTITLE_INFO subtitle_info;
	};

	VO_S32	index_QL;
//	QualityLevel * ptr_next;
//	QualityLevel * ptr_pre;
};

struct QualityLevel_Audio
{
	VO_S64	bitrate;
	VO_CHAR	fourCC[8];
	VO_S32	samplerate_index;
	VO_BYTE	codecPrivateData[256];
	VO_S32	length_CPD;
	VO_AUDIO_FORMAT audio_info;
};

struct CustomAttributesElement
{
	VO_CHAR	Name[255];
	VO_CHAR	Value[255];	
};


struct StreamIndex
{
	VO_SOURCE2_TRACK_TYPE   nTrackType;
	VO_CHAR	type[8];	
	VO_CHAR	name[32];	
	VO_U32	nCodecType; // 
	VO_CHAR	subtype[8];
	VO_CHAR	url[512];
	VO_CHAR language[16];
	
//	VO_S32	chunksNumber;
//	VO_S32	QL_Number;
//	QualityLevel * m_ptr_QL_head;
//	QualityLevel * m_ptr_QL_tail;
	VO_U32	streamindex;
	list_T<QualityLevel*> QL_List;
	list_T<FragmentItem*> Chunk_List;
	// Audio quality level ( only one )
//	QualityLevel_Audio audio_QL[10];
	CustomAttributesElement CustomAttribute;
};

typedef struct
{
	VO_BOOL		isUsePiff;
	VO_BYTE		systemID[16];
	VO_U32		dataSize;
	VO_BYTE		*data;
}ProtectionHeader;

struct SmoothStreamingMedia
{
	VO_S32	majorVersion;	
	VO_S32	minorVersion;
	VO_S64	duration;
	VO_S64    timeScale;
	VO_BOOL islive;
	list_T<StreamIndex*> streamIndexlist;	//[STREAM_NUMBER];
//	VO_U32   streamCount;
	VO_S64	fragment_duration;
	VO_S32	lookaheadfragmentcount;
	VO_U64	dvrwindowlength;

	//piff
	ProtectionHeader piff;
};



class ISS_ManifestParser
{
public:
	ISS_ManifestParser();
	~ISS_ManifestParser();

	VO_U32	   Init();
	VO_U32	   Uninit();

	
	VO_U32 Parse( VO_PBYTE pBuffer,  VO_U32 uSize, SmoothStreamingMedia *manifest);
	VO_U32 ReleaseManifestInfo(SmoothStreamingMedia *manifest);
	VO_U32 SetLibOp(VO_LIB_OPERATOR* libop);
	VO_U32 SetWorkPath(VO_TCHAR * pWorkPath);	
protected:
	VO_VOID ConvertUnicodeToUTF8( VO_PBYTE pBuffer,  VO_U32 uSize);

	VO_VOID hexstr2byte( VO_PBYTE ptr_hex , VO_CHAR * str_hex , VO_S32 length);
	int vowcslen(const vowchar_t* str); 
	VO_S32 get_samplerate_index( VO_S32 samplerate );
	VO_VOID add_fragmentItem(StreamIndex *pStreamIndex, FragmentItem * ptr_item);
	VO_VOID add_qualityLevel( StreamIndex *pStreamIndex, QualityLevel * ptr_item, VO_U32 index);

	VO_BOOL ParseSmoothStreamingMedia();
	VO_BOOL ParseStreamIndex();
	VO_BOOL ParseQualityLevel_Audio();
	VO_BOOL ParseQualityLevel();
	VO_BOOL ParseChunk();
	int VO_UnicodeToUTF8(const vowchar_t *szSrc, const int nSrcLen, char *strDst, const int nDstLen);
	VO_U32 CheckTheManifest( VO_PBYTE* ppBuffer,  VO_U32* pSize);

protected:	
	CXMLLoad *m_pXmlLoad;

	VO_CHAR*  m_pBuffer;
	VO_U32 	  m_uSize;
};


#ifdef _VONAMESPACE
}
#endif


#endif//_ISS_MANIFESTPARSER_H_

