#pragma once

#include "vo_http_stream.h"
#include "DRM_API.h"
#include "voParser.h"
#include "voString.h"
#include "voType.h"
#include "voAudio.h"
#include "voSmthBase.h"
#include "voLiveSourcePlus.h"
#include "voVideo.h"
#include "voSource.h"
//#define __USE_TINYXML
#ifdef __USE_TINYXML
	#include "tinyxml.h"
#else
	#include "voXMLLoad.h"
#endif

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#ifdef _IOS
namespace _ISS{
#endif

// For UTF-16 convert to UTF-8
#if !defined __VO_UTF8_H__
#define __VO_UTF8_H__

#define BYTE_1_REP          0x80   
#define BYTE_2_REP          0x800 
#define BYTE_3_REP          0x10000 
#define BYTE_4_REP          0x200000 

#define SIGMASK_1_1         0x80
#define SIGMASK_2_1         0xe0
#define SIGMASK_3_1         0xf0
#define SIGMASK_4_1         0xf8

#endif

#if !defined __VO_WCHAR_T__
#define __VO_WCHAR_T__
typedef unsigned short vowchar_t;
#endif

// Smooth Streaming Manifest file Tag
#define TAG_SMOOTH_STREAMING_MEDIA		"SmoothStreamingMedia"
#define TAG_MAJOR_VERSION               "MajorVersion"
#define TAG_MINOR_VERSION               "MinorVersion"
#define TAG_DURATION					"Duration"
#define TAG_ISLIVE						"IsLive"
#define TAG_STREAM_INDEX				"StreamIndex"
#define TAG_TYPE	                    "Type"
#define TAG_NAME	                    "Name"
#define TAG_SUBTYPE						"Subtype"
#define TAG_CHUNKS						"Chunks"
#define TAG_URL							"Url"
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
#define TAG_TIME_SCALE						"TimeScale"

#define TAG_C							"c"
#define TAG_T							"t"
#define TAG_N							"n"
#define TAG_D							"d"


//PIFF
#define TAG_PIFF_PROTECTION				"Protection"
#define TAG_PIFF_PROTECTIONHEADER	 "ProtectionHeader"
#define TAG_PIFF_SYSTEMID					"SystemID"
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




struct QualityLevel
{
	VO_S64	bitrate;	
	VO_CHAR	fourCC[8];
	VO_BYTE	codecPrivateData[256];
	VO_S32	length_CPD;

	union
	{
		VO_VIDEO_FORMAT video_info;
		VO_AUDIO_FORMAT audio_info;
	};

	VO_S32	index_QL;	
	QualityLevel * ptr_next;
	QualityLevel * ptr_pre;

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

struct StreamIndex
{
	VO_SOURCE_TRACKTYPE   nTrackType;
	VO_CHAR	type[8];	
	VO_CHAR	name[32];	
	VO_U32	nCodecType; // 
	VO_CHAR	subtype[8];
	VO_CHAR	url[512];

	VO_S32	chunksNumber;
	VO_S32	QL_Number;
	QualityLevel * m_ptr_QL_head;
	QualityLevel * m_ptr_QL_tail;
	
	// Audio quality level ( only one )
	QualityLevel_Audio audio_QL[10];

};

struct SmoothStreamingMedia
{
	VO_S32	majorVersion;	
	VO_S32	minorVersion;
	VO_S64	duration;
	VO_S64    timeScale;
	VO_BOOL islive;
	StreamIndex	*streamIndex;//[STREAM_NUMBER];
	VO_U32   streamCount;
	VO_S64	fragment_duration;

	//piff
	ProtectionHeader piff;
};


class vo_manifest_reader
{
public:
	vo_manifest_reader(void);
	~vo_manifest_reader(void);

	VO_BOOL Parse( vo_http_stream *ptr_fragment,VO_CHAR * url, SmoothStreamingMedia *manifest);
	VO_BOOL Parse( VO_PBYTE pBuffer,  VO_U32 uSize ,SmoothStreamingMedia *manifest );

	VO_BOOL is_live_stream(){ return m_is_live; }

	VO_CHAR * get_relativedirectory(){ return m_relative_directory; }

	int  StrCompare(const char* p1, const char* p2);

	VO_BOOL get_fragmentItem( FragmentItem ** ptr_item,  VO_S64 start_time);
	VO_BOOL get_FirstfragmentItem(FragmentItem **ppItem );
	VO_S64 find_fragment_starttime( VO_S64 pos,int nIsVideo);

	int vowcslen(const vowchar_t* str); 
	int VO_UnicodeToUTF8(const vowchar_t *szSrc, const int nSrcLen, char *strDst, const int nDstLen);

	FragmentItem * m_ptr_FI_head;
 	FragmentItem * m_ptr_FI_tail;

#ifndef __USE_TINYXML
	void SetXmlOp(CXMLLoad *pxmlop) { m_pXmlLoad = pxmlop; }
#endif

protected:

	VO_VOID get_relative_directory( VO_CHAR * url );
	VO_BOOL get_manifest(vo_http_stream *pHttpStream, VO_CHAR * url );

	VO_VOID destroy();

	VO_S32 get_samplerate_index( VO_S32 samplerate );

	// Manifest parse
#ifdef __USE_TINYXML
	VO_VOID add_qualityLevel(  VO_S32 index_stream, QualityLevel * ptr_item );
#else
	VO_VOID add_qualityLevel( StreamIndex *pStreamIndex,VO_S32 index_stream, QualityLevel * ptr_item );
#endif
	VO_VOID add_fragmentItem( FragmentItem * ptr_item );
	FragmentItem  *add_fragmentItem( FragmentItem **head, FragmentItem **tail, FragmentItem * ptr_item, FragmentItem *pCurItem = NULL);
	VO_VOID MergeFragmentList(FragmentItem **head, FragmentItem **tail,FragmentItem *head1, FragmentItem *tail1);

	VO_BOOL parse_manifest();
	//new update
	VO_BOOL parse_manifest2();
#ifdef __USE_TINYXML
	bool IsTheSameTag(TiXmlNode* pNode, char* pszTag);
#endif
	VO_VOID hexstr2byte( VO_PBYTE ptr_hex , VO_CHAR * str_hex , VO_S32 length);



protected:
//	vo_http_stream m_stream;

	SmoothStreamingMedia *m_manifest;

#ifndef __USE_TINYXML
	CXMLLoad *m_pXmlLoad;
#endif
// 	CDllLoad			m_dlXmlEngine;
// 	VO_PTR				m_xmlparser_handle;
// 	VO_XML_PARSER_API		m_xmlparser_api;

	VO_CHAR * m_file_content;
	VO_S32 m_file_content_size;
	VO_CHAR m_relative_directory[1024];

	VO_S32 m_item_count;

	VO_U32 m_read_time;

	VO_BOOL m_is_live;

private:
	VO_U32 ParserAudioHeadData(VO_AUDIO_FORMAT &info);

public:
	VOSMTH_DRM_TYPE m_drmCustomerType;
	VO_U32 SetDrmCustomerType(VOSMTH_DRM_TYPE type){ m_drmCustomerType = type; return 0;}
};

#ifdef _IOS
#ifdef _ISS_SOURCE_
}	// end of _ISS namespace
#endif
#ifdef _VONAMESPACE
}
#endif

#endif
