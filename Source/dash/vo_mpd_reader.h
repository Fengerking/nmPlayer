
#ifndef __VO_MPD_READER_H_
#define __VO_MPD_READER_H_


#include "vo_http_stream.h"
#include "DRM_API.h"

#include "voParser.h"
#include "voString.h"
#include "voType.h"
#include "voAudio.h"

#include "voDashBase.h"

#include "vompType.h"
#include "voSource.h"

#include "vo_http_stream.h"
#include "tinyxml.h"
#include "time.h"

//#include "contentComponent_Tag.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
#ifdef _IOS
using namespace _DASH;
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

#define STREAM_NUMBER	2

#define TAG_DASH_STREAMING_MEDIA		"MPD"
#define TAG_AVAILABLE_TIME    "availabilityStartTime"

#define TAG_DASH_STREAMING_Period   "Period"
#define TAG_DASH_STREAMING_Representation    "Representation"
#define TAG_DASH_STREAMING_Group    "Group"
#define TAG_DASH_STREAMING_SegmentInfo    "SegmentInfo"

#define TAG_DASH_STREAMING_InitialisationSegmentURL   "InitialisationSegmentURL"
#define TAG_DASH_STREAMING_URL   "Url"
#define TAG_DASH_STREAMING_SOURCEURL "sourceURL"
#define TAG_DASH_STREAMING_TYPE         "type"
#define TAG_DASH_STREAMING_MPDURATION        "mediaPresentationDuration"
#define TAG_DASH_STREAMING_MINBUFFERTIMER     "minBufferTime"
#define TAG_DASH_STREAMING_segmentAlignmentFlag    "segmentAlignmentFlag"
#define TAG_DASH_STREAMING_MIMETYPE  "mimeType"
#define TAG_DASH_STREAMING_WIDTH "width"
#define TAG_DASH_STREAMING_HEIGHT  "height"
#define TAG_DASH_STREAMING_BIRATE "bandwidth"
#define TAG_DASH_STREAMING_DURATION   "duration"
#define TAG_DASH_STREAMING_startWithRAP   "startWithRAP"
//#define TAG_DASH_STREAMING_INIATALURL  "InitialisationSegmentURL"
#define TAG_SIMPLE_TIMESCALE "timescale"
#define TAG_SIMPLE_ADAPTIONSET  "AdaptationSet"
#define TAG_SIMPLE_RATE					"SamplingRate"
#define TAG_CODEC_PRIVATE_DATA			"CodecPrivateData"
#define TAG_DASH_SEGMENTBASE            "SegmentBase"
#define TAG_DASH_SEGMENTLIST            "SegmentList"
#define TAG_DASH_BASEURL                "Initialisation"
#define TAG_DASH_RANGE                   "range"
#define TAG_DASH_MEDIARANGE              "mediaRange"
#define TAG_DASH_BASEURL_2                "BaseURL"
#define TAG_DASH_SEGURL                   "SegmentURL"
#define TAG_DASH_MEDIABASEURL             "media"
#define TAG_DASH_TEMPLATE                 "SegmentTemplate"
#define TAG_DASH_TIMELINE                 "SegmentTimeline"
#define TAG_DASH_ContentComponent         "ContentComponent"
#define TAG_DASH_CONTENTYPE               "contentType"
#define TAG_DASH_BufferDepth              "timeShiftBufferDepth"
#define TAG_DASH_UPDATE_PERIOD             "minimumUpdatePeriod"  
#define TAG_DASH_START_NUMBER              "startNumber"
#define TAG_DASH_ACTURE                    "g2:actuate"
#define TAG_DASH_HOLDER                   "SegmentTemplatePlaceholder" 
#define TAG_DASH_STREAMING_MEDIA_G1	"g1:MPD"
#define TAG_DASH_STREAMING_Period_G1  "g1:Period"
#define TAG_DASH_STREAMING_Representation_G1    "g1:Representation"
#define TAG_DASH_STREAMING_MEDIA_ID	"id"
#define TAG_SIMPLE_ADAPTIONSET_G1  "g1:AdaptationSet"
#define TAG_DASH_STREAMING_Group_G1   "Group"
#define TAG_DASH_TEMPLATE_G1                "g1:SegmentTemplate"

#define TAG_D                              "d"  
#define TAG_T                              "t" 
#define TAG_R                              "r"  

#define TAG_DASH_BASEURL_3                 "initialization"
#define TAG_DASH_BASEURL_4                 "initialization"

#define TAG_CODECS                         "codecs"
#define TAG_LANG                           "lang"
#define TAG_DASH_TIME_SCALE               "timescale"
#define TAG_DASH_TIME_Duration               "duration"
#define TAG_START_NUMBER              "startNumber"
#define TAG_BIT_SWITCH             "bitstreamSwitching"


typedef struct Seg_item{
	VO_S32	 group_index;
	VO_S32   representation_index;
	VO_S32	 segment_index;
	VO_S64	 starttime;
    VO_CHAR  item_url[1024];
    VO_S64	 duration;
	VO_U8    nIsVideo;//0 audio ,1 video
    Seg_item * ptr_next;
	VO_CHAR range[512];
	VO_S64	 startnmber;
	VO_S64   nOrignalStartNumber;
	VO_BOOL  bDependForNextTimeLine;

}linklist;//media_item

typedef struct Representation_Audio{
    //VO_CHAR	lang[8];
	VO_S64	bitrate;
	VO_S32	samplerate_index;
	VO_BYTE	codecPrivateData[256];
	VO_S32	length_CPD;
	VO_AUDIO_FORMAT audio_info;

}Representation_Audio;

struct Representation{
	VO_S64	bitrate;
	VO_S32	length_CPD;
	VO_BYTE	codecPrivateData[256];
	VO_CHAR	initalUrl[1024];
	VO_CHAR mediaUrl[1024];
    VO_S32 rpe_item_index;
	VO_S32 segmentsNumber;
    VO_BOOL startWithRAP;
    VO_S64 minbufferTime;
	VO_S64 duration;
	VO_CHAR id[512];
	VO_CHAR ini_range[512];
	VO_S32 group_index;
	VO_S32	index_QL;	
	union{
     VO_VIDEO_FORMAT video_QL;
	 VO_AUDIO_FORMAT audio_QL;
	};
	VO_S32 track_count;


	Representation * ptr_next;
	Representation * ptr_pre;
    Seg_item * m_ptr_SG_head;
	Seg_item * m_ptr_SG_tail;
};



struct ApaptionSet{
	VO_BOOL bSegmentTimeline;
	VO_S32  group_index;
	VO_SOURCE_TRACKTYPE   nTrackType; 
	VO_CHAR	type[8];
	VO_BOOL segmentAlignmentFlag;
	VO_U32	nCodecType;
	VO_S32  rpe_number;
	Representation * m_ptr_QL_head;
	Representation * m_ptr_QL_tail;
	VO_CHAR id[512];
	VO_CHAR	lang[256];
	Representation_Audio  audio_QL;
};


struct MpdStreamingMedia{
	VO_BOOL islive;
	VO_S64	duration_video;
	VO_S64	duration_audio;
    VO_S64  mediaPresentationDuration;
	VO_S64  minBufferTime;
	VO_S32  peroid_index;
    VO_S32  group_count;
	VO_CHAR	type[8];
	VO_S64  timeScale;
    VO_S64  audiotimeScale;
	VO_S64  video_scale;
	VO_S64  audio_scale;
	//ProtectionHeader piff; //the protection drm
	VO_CHAR	baseurl[32];
	ApaptionSet * adaptionSet;
	VO_S64  m_start_time;
	VO_S64  m_start_time_audio;

};// the group or the adaptionset   can be divides into audio and the video type




class vo_mpd_reader
{
public:
	 vo_mpd_reader(void);
	~vo_mpd_reader(void);
	VO_S32  lastIndex(const char *s,const char *t);
	VO_S64  find_fragment_starttime(VO_S64 pos,int nIsVideo);
	VO_BOOL Parse(vo_http_stream *ptr_fragment,VO_CHAR * url, MpdStreamingMedia *manifest);
	VO_VOID addRepresentationLevel(ApaptionSet * pAdaptionSet,int group_index, Representation *pRepresentation);
	VO_VOID add_segmentItem(Representation * rpe,Seg_item * ptr_item);
	VO_VOID add_fragmentItem( Seg_item * ptr_item );
	VO_VOID replace(char *source,const char *sub,const char *rep );
	VO_BOOL get_segmentItem( Seg_item ** ptr_item,  VO_U64 start_time );
	VO_BOOL get_segmentItem(Seg_item ** ptr_item,  int ini_chunk );
	Seg_item* GetFirstChunk();
	VO_BOOL get_mpd( VO_CHAR * url );
	VO_VOID get_relative_directory( VO_CHAR * url );
	VO_S32  get_samplerate_index( VO_S32 samplerate ) ;
	int     StrCompare(const char* p1, const char* p2);
	int     StrCompare(char* p1, const char* p2,int index);
	VO_BOOL analyze_2(char * url);


#if 0
	VO_BOOL analyze_video(VO_S64 Duration_video[1024],VO_S64 Timeline_video[1024],void * pChildNode,int flag,int group_index);
	VO_BOOL analyze_audio(void * pChildNode);
	VO_BOOL analyze_audio(VO_S64 Timeline_video[1024],void * pChildNode,int flag,int group_index);
#endif
	VO_VOID analyze_video(VO_S64 Duration_video[1024],VO_S64 Timeline_video[1024],TiXmlNode * pChildNode,int flag,int group_index);
	VO_BOOL analyze_test(char * url);
	VO_BOOL is_live_stream(){ return m_is_live; }
	VO_BOOL is_need_update(){return m_is_need_update;}
	VO_CHAR *  getRepresentationUrl(int stream_index,int representation_index);
    VO_VOID hexstr2byte( VO_PBYTE ptr_hex , VO_CHAR * str_hex, VO_S32 length );
	VO_VOID destroy();
	Representation * get_qualityLevel( int stream_index,int index_item );
	int vowcslen(const vowchar_t* str); 
	int VO_UnicodeToUTF8(const vowchar_t *szSrc, const int nSrcLen, char *strDst, const int nDstLen);
	time_t  FormatTime2(const char * szTime);
	VO_BOOL get_segmentItem(Seg_item ** ptr_item,  int ini_chunk,int type); 


public:
    MpdStreamingMedia * m_manifest;
	VO_CHAR * m_file_content;	
	VO_BOOL m_is_live;
	VO_BOOL m_is_need_update;
	VO_BOOL m_seg_temlate;
	VO_U32  m_seg_count;
	VO_S64  m_file_content_size;
	VO_CHAR m_relative_directory[1024];
    time_t  m_read_time;
	VO_U32  m_representation_count;
    int m_count_video;
    int m_count_audio;
    Representation	*  p_StreamIndex;
	Seg_item * m_ptr_FI_head;
	Seg_item * m_ptr_FI_tail;
	int video_flag;
	int audio_flag;
	vo_http_stream m_stream;
	VO_S64 m_update_period;
	VO_U32  audio_index;
	VO_S64  video_scale;
	VO_S64  audio_scale;
	VO_CHAR m_url[1024];
	int group_number;
	VO_S64 real_count;
	int unknown_flag;
//	contentComponent_Tag * com_tag;


//add by leon
	void InitAVList();
	void DestroyAVList();
	void FlushAVList();
	void CreateAVList(int counts);
	VO_S64 * Timeline_video;
	VO_S64 * Timeline_audio;
	VO_S64 * Duration_video;
	VO_S64 * Duration_audio;

	VO_U64 GetStartTime();

};
#ifdef _VONAMESPACE
}
#endif

#endif
