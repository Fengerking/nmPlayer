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
#include "voXMLParser.h"
#include "voXMLLoad.h"
#include "voDSByteRange.h"
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
#define LIVE_NUMBER  0
#define DEAD_LENGTH  20000
	/*for the time-line problem*/
#define _USETIMELINE 
#define _NOTUSETIMELINE
#define _DISABLEPUREAUDIO 1
#define INDEX_RANGE_OFFSET 1
#define INDEX_RANGE__END_OFFSET 0
#define WORKROUND_ELEMENTAL 0
	/*end*/
#define  ADAPTIONSET_MAXNUM  10
#define  PERIOD_MAXNUM 10
#define TAG_DASH_STREAMING_MEDIA		"MPD"
#define TAG_DASH_STREAMING_Period   "Period"
#define TAG_SIMPLE_ADAPTIONSET  "AdaptationSet"
#define TAG_DASH_STREAMING_Group    "Group"
#define TAG_DASH_STREAMING_Representation    "Representation"
#define TAG_DASH_STREAMING_SegmentInfo    "SegmentInfo"
#define TAG_DASH_REPRESENTATIONINDEX    "RepresentationIndex"
#define TAG_DASH_STREAMING_URL   "Url"
#define TAG_DASH_STREAMING_SOURCEURL "sourceURL"
#define TAG_DASH_STREAMING_SOURCEURL_1 "SegmentURL"
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
#define TAG_DASH_BASEURL_BACK                 "initialisation"
#define TAG_DASH_BASEURL                "Initialisation"
#define TAG_DASH_MEDIARANGE              "mediaRange"
#define TAG_DASH_RANGE                   "range"
#define TAG_D                              "d"  
#define TAG_T                              "t" 
#define TAG_R                              "r"  
#define MPD_LANG_DEFAULT				"en"



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



	typedef struct Seg_item{
		VO_U32  period_index;
		VO_S32	 group_index;
		VO_S32   representation_index;
		VO_S32	 segment_index;
		VO_U64	 starttime;
		VO_S64   live_time;
		VO_CHAR  item_url[1024];
		VO_S64	 duration;
		VO_U8    nIsVideo;//0 audio ,1 video,2 subtitle
		Seg_item * ptr_next;
		VO_CHAR range[512];
		VO_U64	 startnmber;
		VO_U64   nOrignalStartNumber;
		VO_BOOL  bDependForNextTimeLine;
		VO_CHAR  chunk_lang[512];
		VO_U64   first_offset;
		VO_S64   size;
		VO_BOOL  is_sigx_index_chunk;
		VO_BOOL  is_index_parsed;
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
		VO_CHAR index_range[512];
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
		VO_U32  byterange_chunk_count;
		VO_S64 chunk_range_offset;
		VO_BOOL m_index_range_exact;
		VO_S32  rpe_track_id;
		VO_U32  current_sigx_offset_time;
		VO_U64  head_range_end;
		VO_BOOL is_init_data;

	};



	struct ApaptionSet{
		VO_U32  period_index;
		VO_S32  group_index;
		VO_S32  track_id;
		VO_S32 track_count;
		VO_U32	nCodecType;
		VO_S32  rpe_number;	 
		VO_CHAR	type[8];
		VO_CHAR id[512];
		VO_CHAR	lang[16];
		VO_BOOL segmentAlignmentFlag;
		VO_BOOL bSegmentTimeline;
		Representation * m_ptr_QL_head;
		Representation * m_ptr_QL_tail;
		Representation_Audio  audio_QL;
		VO_SOURCE_TRACKTYPE   nTrackType;

	};

	struct Period{
		VO_S64  peroid_index;
		VO_S64  period_start;
		VO_S64  period_init_duration;
		VO_S64  period_fact_duration; 
		VO_S64  period_id;
		VO_S64	duration_video;
		VO_S64	duration_audio;
		VO_S64  duration_subtitle;
		VO_S64  period_atimescale;
		VO_S64  period_vtimescale;
		VO_S64  period_subtitle_timescale;
		VO_BOOL is_muxed;
	    VO_CHAR period_base_url[1024];
	};

	struct MpdStreamingMedia{
		VO_BOOL islive;
		VO_S64	duration_video;
		VO_S64	duration_audio;
		VO_S64  duration_subtitle;
		VO_S64  mediaPresentationDuration;
		VO_S64  minBufferTime;
		VO_U32  group_count;
		VO_U32  period_count;
		VO_CHAR	type[8];
		VO_S64  timeScale;
		VO_S64  audiotimeScale;
		VO_S64  video_scale;
		VO_S64  audio_scale;
		VO_CHAR	baseurl[32];
		Period  *m_period;
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
		VO_BOOL Parse(VO_PBYTE pBuffer, VO_U32 uSize ,MpdStreamingMedia *manifest,VO_CHAR* pMpdPath);
		VO_BOOL parse_manifest2(VO_CHAR* pMpdPath);
		VO_VOID analyze_video(VO_S64 Duration_video[1024],VO_S64 Timeline_video[1024],CXMLLoad *m_pXmlLoad,void * pChildNode,int flag,int group_index,VO_U32 period_index);
		VO_S64  find_fragment_starttime(VO_U64 pos,int nIsVideo,VO_U32 period_index);
		VO_S64  find_fragment_starttime_index(VO_U64 *pos, int nIsVideo,VO_U32 period_index );
		VO_VOID addRepresentationLevel(ApaptionSet * pAdaptionSet,int group_index, Representation *pRepresentation);
		VO_VOID add_segmentItem(Representation * rpe,Seg_item * ptr_item);
		VO_VOID add_segmentItem_subtile(Representation * rpe,Seg_item * ptr_item);
		VO_VOID add_fragmentItem( Seg_item * ptr_item );
		VO_VOID add_subtitleItem( Seg_item * ptr_item );
	public:

		VO_BOOL get_segmentItem(Seg_item ** ptr_item,  VO_U64 ini_chunk );
		VO_BOOL get_segmentItem(Seg_item ** ptr_item,Representation * rpe,  VO_U64 index );
		VO_BOOL get_segmentItem_subtitle(Seg_item ** ptr_item,  VO_U64 ini_chunk,VO_U32 period_index);
		VO_VOID get_relative_directory( VO_CHAR * url );
		VO_S32  get_samplerate_index( VO_S32 samplerate ) ;
		VO_CHAR *  getRepresentationUrl(int stream_index,int representation_index);
		VO_CHAR *  GetMPDUrl();
		Representation * get_qualityLevel( int stream_index,int index_item );	
		VO_BOOL get_segmentItem(Seg_item ** ptr_item,  VO_S64 ini_chunk,int type,VO_U32 period_index); 
		VO_S32  get_lang_group(VO_CHAR * lang,VO_U32 period_index);
		VO_S32 get_rpe_index(VO_S32 group_id,VO_S32 track_id);
#if 0
		VO_U32 DoumpTheRawData(VO_BYTE* pBuffer, VO_U32 nLen);
#endif
	public:

		VO_BOOL Is_Dynamic();
		VO_BOOL Is_Live();
		VO_BOOL is_dash_ts(){return m_is_ts;}
		VO_BOOL Is_dash_pure_video(){return m_is_pure_video;}
		VO_BOOL Is_dash_pure_audio(){return m_is_pure_audio;}
		VO_BOOL Is_dash_muxed(VO_U32 period_index){return  m_manifest->m_period[period_index].is_muxed;}
		VO_S64  Get_Update_Period(){return m_update_period;};
		VO_S64  Get_Video_Scale(){return video_scale;}
		VO_S64  Get_Audio_Scale(){return audio_scale;}
		VO_S64  Get_Audio_Scale(VO_U32 period_index){return m_manifest->m_period[period_index].period_atimescale;}
		VO_S64  Get_Video_Scale(VO_U32 period_index){return m_manifest->m_period[period_index].period_vtimescale;}
		Seg_item* GetFirstChunk(){return  m_ptr_FI_head;}
		Seg_item* GetLastChunk(){return  m_ptr_FI_tail;}
		VO_S32  GetAudioCount(){return m_audioCount;}
		VO_S32  GetVideoCount(){return m_videoCount;}
		VO_S32  GetAudioBitrateCount(){return m_audio_bitrate_count;}
		VO_S32  GetSubtitleCount(){return m_subtitleCount;}
		int  GetSegCount_Video(){return m_count_video;}
		int   GetSegCount_Audio(){return m_count_audio;}
		VO_S32  GetGroupCount(){return m_group_count;}
		VO_S32  GetPeriodCount(){return m_period_count;}
		VO_S64  GetAvailbilityTime(){return m_read_time;}
		VO_S64  GetLastAvailblityTime(){return last_read_time;}
		VO_S32  GetStreamCount(){return m_stream_count;}
		VO_S32  GetStreamCount(VO_S32 period_index);
		VO_VOID Init_MPD(VO_U32 period_index);
		VO_S64  GetDuration(){return m_duration;};
		VO_BOOL  Is_Template(){return m_seg_temlate;}
		VO_BOOL  Is_IndexRange_video(){return m_is_index_range_video;}
		VO_BOOL  Is_IndexRange_audio(){return m_is_index_range_audio;}
		VO_BOOL  Is_IndexRange_subtile(){return m_is_index_range_subtitle;}
		VO_VOID  Set_IndexRange(VO_BOOL is_index_range,int type){if(type==1){m_is_index_range_video = is_index_range;}
		else{
			m_is_index_range_audio = is_index_range;
		}}
		VO_U64   GetStartTime();
		VO_S32   get_lang_track(VO_S32 track_id);
		VO_BOOL  IsDenpedForNext(){return m_is_depend;}
		VO_U64   Compute_video_duration(VO_U64 chunk_index);
		VO_U64   Compute_audio_duration(VO_U64 chunk_index);
		VO_S64   GetTimeLineTime(){return m_time_line_time;}
		VO_U32   SetLibOp(VO_LIB_OPERATOR* libop);
		VO_U32   SetWorkPath(VO_TCHAR * pWorkPath);
		VO_U32   InitParser();
		VO_U32   UninitParser();
		VO_VOID  Set_rpe_index(int index,int type);
	public:
		void InitAVList();
		void DestroyAVList();
		void FlushAVList();
		void FlushTAGList();
		void CreateAVList(VO_U64 counts);
	public:
		int vowcslen(const vowchar_t* str); 
		int VO_UnicodeToUTF8(const vowchar_t *szSrc, const int nSrcLen, char *strDst, const int nDstLen);
		VO_VOID hexstr2byte( VO_PBYTE ptr_hex , VO_CHAR * str_hex, VO_S32 length );
		int     StrCompare(const char* p1, const char* p2);
		int     StrCompare(char* p1, const char* p2,int index);
		VO_VOID replace(char *source,const char *sub,const char *rep );
		VO_VOID  GetChunkRange(char *Range_URL,VO_U32 *ini_range,VO_U32 *end_range,VO_U32 *size);
		VO_S64   GetCurrent_index_offset(){return m_index_first_offset;}
		VO_VOID  SetCurrent_index_offset(VO_S64 offset){m_index_first_offset =offset; }
	public:
		CXMLLoad *m_pXmlLoad;
		MpdStreamingMedia * m_manifest;
		VO_U32 m_period_count;
		VO_BOOL m_seg_temlate;
		VO_U32  m_seg_count;
		VO_CHAR * m_file_content;
		VO_CHAR   mpd_base_url[256];
		VO_S64  m_file_content_size;
		VO_CHAR m_relative_directory[1024];
		Representation	*  p_StreamIndex;
		Seg_item * m_ptr_FI_head;
		Seg_item * m_ptr_FI_tail;
		Seg_item * m_ptr_sub_FI_head;
		Seg_item * m_ptr_sub_FI_tail;
		int video_flag;
		int audio_flag;
		int subtitle_flag;
		VO_U32  audio_index;
		VO_CHAR m_url[1024];
		int group_number;
		VO_S64 * Timeline_video;
		VO_S64 * Timeline_audio;
		VO_S64 * Timeline_subtile;
		VO_S64 * Duration_video;
		VO_S64 * Duration_audio;
		VO_S64 * Duration_subtitle;
		VO_BOOL  m_is_depend;
		VO_U64   m_time_line_time;
		VO_BOOL  m_is_ttml;

	private:
		VO_S64  m_timeline_time;
		VO_S64  m_time_offset;
		VO_BOOL m_is_ts;
		VO_BOOL m_is_muxed;
		VO_BOOL m_is_pure_video;
		VO_BOOL m_is_pure_audio;
		VO_BOOL m_is_index_range_video;
		VO_BOOL m_is_index_range_audio;
	public:
		VO_BOOL m_is_index_range_subtitle;
	//	Common_Tag * m_comtag;
	private:
		Mpd_tag  mpd_tag;
		BaseUrl  m_base_url_el;
		Group_Tag   m_grouptag;
		Period_Tag  m_period_tag;
		ProgramInformation  m_program;
		Metrics  m_metrics;
		contentComponent_Tag com_tag;
		SegmentTemplate_Tag m_template_tag;
		SegmentBase_Tag m_seg_base;
		Repre_Tag  m_rpe_tag;
		SegInfo_Tag m_seg_info;
	private:
		VO_S64 m_duration_time_line_video;
		VO_S64 m_duration_time_line_audio;
		VO_S64 m_duration_time_line_subtitle;
		int m_videoCount ;
		int m_audioCount;
		int m_subtitleCount;
		int m_group_count;
		int m_audio_bitrate_count;
		time_t  m_read_time;
		time_t  last_read_time;
		VO_U32  m_stream_count;
		int m_count_video;
		int m_count_audio;
		int m_count_subtitle;
		VO_S64   m_duration;
		VO_S64   m_update_period;
		VO_S64   video_scale;
		VO_S64   audio_scale;
		VO_S64   subtile_scale;
		VO_U32 current_video_rpe;
		VO_U32 current_audio_rpe;
		VO_S64  m_index_first_offset; 
	};
#ifdef _VONAMESPACE
}
#endif
#endif
