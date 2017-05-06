	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		VO_MPD_Parser.h

	Contains:	VO_MPD_Parser  header file

	Written by:	Danny_Wang

	Change History (most recent first):
	2012-06-28 				Create file

*******************************************************************************/
#ifndef __VO_MPD_Parser_H_
#define __VO_MPD_Parser_H_


//#include "DRM_API.h"

#include "voParser.h"
#include "voString.h"
#include "voType.h"
#include "voAudio.h"
#include "vompType.h"
#include "voSource.h"
#include "tinyxml.h"
#include "time.h"





#include "contentComponent_Tag.h"
#include "Mpd_tag.h"
#include "Group_Tag.h"
#include "SegmentTemplate_Tag.h"
#include "Repre_Tag.h"
#include "SegmentBase_Tag.h"
#include "SegInfo_Tag.h"
#include "ProgramInformation.h"
#include "BaseUrl.h"
#include "Metrics.h"
#include "Period_Tag.h"
#include "voToolUtility.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
#ifdef _IOS
	using namespace   _VONAMESPACE;
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
#define LIVE_NUMBER    10000
/*for the time-line problem*/
#define _USETIMELINE 
#define _NOTUSETIMELINE
/*end*/
#define TAG_DASH_STREAMING_MEDIA		"MPD"
#define TAG_DASH_STREAMING_Period   "Period"
#define TAG_SIMPLE_ADAPTIONSET  "AdaptationSet"
#define TAG_DASH_STREAMING_Group    "Group"
#define TAG_DASH_STREAMING_Representation    "Representation"
#define TAG_DASH_STREAMING_SegmentInfo    "SegmentInfo"
#define TAG_DASH_STREAMING_URL   "Url"
#define TAG_DASH_STREAMING_SOURCEURL "sourceURL"
#define TAG_DASH_STREAMING_MEDIA_G1	"g1:MPD"
#define TAG_DASH_STREAMING_Period_G1  "g1:Period"
#define TAG_DASH_STREAMING_Representation_G1    "g1:Representation"
#define TAG_DASH_STREAMING_MEDIA_ID	"id"
#define TAG_SIMPLE_ADAPTIONSET_G1  "g1:AdaptationSet"
#define TAG_DASH_STREAMING_Group_G1   "Group"
#define TAG_DASH_TEMPLATE_G1                "g1:SegmentTemplate"
#define TAG_DASH_BASEURL_2                "BaseURL"
#define TAG_DASH_TIMELINE                 "SegmentTimeline"
#define TAG_DASH_ContentComponent         "ContentComponent"
#define TAG_DASH_TEMPLATE                 "SegmentTemplate"
#define TAG_DASH_SEGURL                   "SegmentURL"
#define TAG_DASH_HOLDER                   "SegmentTemplatePlaceholder" 
#define TAG_DASH_STREAMING_InitialisationSegmentURL   "InitialisationSegmentURL"
#define TAG_DASH_SEGMENTBASE            "SegmentBase"
#define TAG_DASH_SEGMENTLIST            "SegmentList"
#define TAG_DASH_BASEURL_3                 "initialization"
#define TAG_DASH_BASEURL_5                 "initialization"
#define TAG_DASH_BASEURL                "Initialisation"
#define TAG_DASH_MEDIARANGE              "mediaRange"
#define TAG_DASH_RANGE                   "range"
#define TAG_D                              "d"  
#define TAG_T                              "t" 
#define TAG_R                              "r"  



#define DASH_ERR_NONE                               0
#define DASH_ERR_EMPTY_POINTER                      1
#define DASH_ERR_WRONG_MANIFEST_FORMAT              2
#define DASH_ERR_LACK_MEMORY                        3
#define DASH_UN_IMPLEMENT                           4
#define DASH_ERR_NOT_ENOUGH_BUFFER                  5
#define DASH_ERR_NOT_EXIST                          6
#define DASG_ERR_NOT_ENOUGH_PLAYLIST_PARSED         7
#define DASH_ERR_NEED_DOWNLOAD                      8
#define DASH_ERR_ALREADY_EXIST                      9
#define DASH_ERR_VOD_END                            10
#define DASH_PLAYLIST_END                           11
#define DASH_ERR_NOT_ENOUGH_PLAYLIST_PARSED         12


#define DASH_INVALID_SEQUENCE_VALUE                 0xffffffff

typedef struct Seg_item{
	VO_U32  period_index;
	VO_S32	 group_index;
	VO_S32   representation_index;
	VO_S32	 segment_index;
	VO_S64	 starttime;
	VO_S64   live_time;
    VO_CHAR  item_url[1024];
    VO_S64	 duration;
	VO_U8    nIsVideo;//0 audio ,1 video
    Seg_item * ptr_next;
	VO_CHAR range[512];
	VO_U64	 startnmber;
	VO_U64   nOrignalStartNumber;
	VO_BOOL  bDependForNextTimeLine;
	VO_CHAR  chunk_lang[512];
}linklist;//media_item


typedef struct Representation_Audio{
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
	VO_S64 m_time_offset;
	
};



struct ApaptionSet{
	VO_U32  period_index;
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
	VO_BOOL bSegmentTimeline;
	VO_S32 track_count;
	
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
	VO_CHAR	baseurl[32];
	ApaptionSet * adaptionSet;
	VO_U64  m_start_time;
	VO_S64  m_start_time_audio;

};// the group or the adaptionset   can be divides into audio and the video type




class VO_MPD_Parser
{
public:
	 VO_MPD_Parser(void);
	~VO_MPD_Parser(void);
	VO_VOID destroy();
	VO_BOOL Parse(VO_PBYTE pBuffer, VO_U32 uSize ,MpdStreamingMedia *manifest,VO_CHAR* pM3uPath);
	VO_BOOL parse_manifest2(VO_CHAR* pM3uPath);
	VO_VOID analyze_video(VO_S64 Duration_video[1024],VO_S64 Timeline_video[1024],TiXmlNode * pChildNode,int flag,int group_index);
	VO_S64  find_fragment_starttime(VO_S64 pos,int nIsVideo);
	VO_S64  find_fragment_starttime_index(VO_S64 pos, int nIsVideo );
	VO_VOID addRepresentationLevel(ApaptionSet * pAdaptionSet,int group_index, Representation *pRepresentation);
	VO_VOID add_segmentItem(Representation * rpe,Seg_item * ptr_item);
	VO_VOID add_fragmentItem( Seg_item * ptr_item );
public:
	
	VO_BOOL get_segmentItem(Seg_item ** ptr_item,  VO_U64 ini_chunk );
	VO_BOOL get_segmentItem(Seg_item ** ptr_item,Representation * rpe,  VO_U64 index );
	VO_VOID get_relative_directory( VO_CHAR * url );
	VO_S32  get_samplerate_index( VO_S32 samplerate ) ;
	VO_CHAR *  getRepresentationUrl(int stream_index,int representation_index);
	Representation * get_qualityLevel( int stream_index,int index_item );	
	VO_BOOL get_segmentItem(Seg_item ** ptr_item,  VO_U64 ini_chunk,int type); 
	VO_S32  get_lang_group(VO_CHAR * lang);
	
public:
	
	VO_BOOL Is_Dynamic(){return  m_is_need_update;}
	VO_BOOL Is_Live(){return m_is_live;}
	VO_BOOL is_dash_ts(){return m_is_ts;}
	VO_BOOL Is_dash_pure_video(){return m_is_pure_video;}
	VO_BOOL Is_dash_muxed(){return m_is_muxed;}
	VO_S64  Get_Update_Period(){return m_update_period;};
	VO_S64  Get_Video_Scale(){return video_scale;}
	VO_S64  Get_Audio_Scale(){return audio_scale;}
	Seg_item* GetFirstChunk(){return  m_ptr_FI_head;}
	Seg_item* GetLastChunk(){return  m_ptr_FI_tail;}
	VO_S32  GetAudioCount(){return m_audioCount;}
	VO_S32  GetVideoCount(){return m_videoCount;}
	int  GetSegCount_Video(){return m_count_video;}
	int   GetSegCount_Audio(){return m_count_audio;}
	VO_S32  GetGroupCount(){return m_group_count;}
	VO_S32  GetPeriodCount(){return m_period_count;}
	VO_S64  GetAvailbilityTime(){return m_read_time;}
	VO_S64  GetLastAvailblityTime(){return last_read_time;}
	VO_S32  GetStreamCount(){return m_stream_count;}
	VO_S32  GetStreamCount(VO_S32 period_index);
	VO_S64  GetDuration(){return m_duration;};
	VO_BOOL  Is_Template(){return m_seg_temlate;}
    VO_U64   GetStartTime();
	VO_BOOL  IsDenpedForNext(){return m_is_depend;}
	VO_U64   Compute_video_duration(VO_U64 chunk_index);
	VO_U64   Compute_audio_duration(VO_U64 chunk_index);
	VO_S64   GetTimeLineTime(){return m_time_line_time;}
public:
	void InitAVList();
	void DestroyAVList();
	void FlushAVList();
	void CreateAVList(VO_U64 counts);
public:
	int vowcslen(const vowchar_t* str); 
	int VO_UnicodeToUTF8(const vowchar_t *szSrc, const int nSrcLen, char *strDst, const int nDstLen);
	VO_VOID hexstr2byte( VO_PBYTE ptr_hex , VO_CHAR * str_hex, VO_S32 length );
	int     StrCompare(const char* p1, const char* p2);
	int     StrCompare(char* p1, const char* p2,int index);
	VO_VOID replace(char *source,const char *sub,const char *rep );
public:
    MpdStreamingMedia * m_manifest;
	VO_U32 m_period_count;
	VO_BOOL m_seg_temlate;
	VO_U32  m_seg_count;
	VO_CHAR * m_file_content;
	VO_S64  m_file_content_size;
	VO_CHAR m_relative_directory[1024];
    Representation	*  p_StreamIndex;
	Seg_item * m_ptr_FI_head;
	Seg_item * m_ptr_FI_tail;
	int video_flag;
	int audio_flag;
	VO_U32  audio_index;
	VO_CHAR m_url[1024];
	int group_number;
	VO_S64 * Timeline_video;
	VO_S64 * Timeline_audio;
	VO_S64 * Duration_video;
	VO_S64 * Duration_audio;
	VO_BOOL  m_is_depend;
	VO_U64   m_time_line_time;

private:
	VO_BOOL m_is_live;
	VO_BOOL m_is_need_update;
	VO_S64  m_timeline_time;
	VO_S64  m_time_offset;
	VO_BOOL m_is_ts;
	VO_BOOL m_is_muxed;
	VO_BOOL m_is_pure_video;
public:
   Common_Tag * m_comtag;
private:
	Mpd_tag  mpd_tag;
//	Common_Tag * m_comtag;
	contentComponent_Tag com_tag;
	Group_Tag   m_grouptag;
	Period_Tag  m_period_tag;
	SegmentTemplate_Tag m_template_tag;
	Repre_Tag  m_rpe_tag;
	SegmentBase_Tag m_seg_base;
    SegInfo_Tag m_seg_info;
	ProgramInformation  m_program;
	BaseUrl  m_base_url_el;
	Metrics  m_metrics;
	//Period_Tag * m_period_tag;
private:
	VO_S64 real_count;
	VO_S64 m_duration_time_line_video;
    VO_S64 m_duration_time_line_audio;
	int unknown_flag;
	int m_videoCount ;
	int m_audioCount;
	int m_group_count;
	time_t  m_read_time;
	time_t  last_read_time;
	VO_U32  m_stream_count;
	int m_count_video;
    int m_count_audio;
	VO_S64   m_duration;
	VO_S64   m_update_period;
	VO_S64   video_scale;
	VO_S64   audio_scale;

};
#ifdef _VONAMESPACE
}
#endif
#endif
