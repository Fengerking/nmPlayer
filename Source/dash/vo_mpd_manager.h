
#ifndef __VO_MPD_MANAGER_H_
#define __VO_MPD_MANAGER_H_

#include "vo_mpd_reader.h"
#include "voCMutex.h"
#define _USE_NEW_BITRATEADAPTATION

#ifdef _USE_NEW_BITRATEADAPTATION
#include "CBitrateMap.h"
#endif
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class vo_mpd_manager
{
public:
     vo_mpd_manager(void);
	~vo_mpd_manager(void);
	VO_BOOL SetManifest( vo_http_stream *ptr_fragment,VO_CHAR * url);
	VO_BOOL Parse_mpd(vo_http_stream *ptr_fragment);
	VO_VOID TooHightQualityLevel();
	VO_VOID close();
	VO_VOID replace(char *source,const char *sub,const char *rep );
	vo_mpd_reader m_reader;
    MpdStreamingMedia  m_manifest;
	Seg_item * GetFirstChunk();
	Seg_item * GetNextChunk(Seg_item *ptr_item);
	Seg_item * GetNextAudio_Chunk(Seg_item *ptr_item);
	Representation * get_qualityLevel(int stream_index , int index_item );
	VO_BOOL is_live(){ return m_reader.is_live_stream(); }
	VO_BOOL is_needupdate(){ return m_reader.is_need_update(); }
	//VO_VOID replace(char *source,const char *sub,const char *rep );
	VO_U64  GetTimeScaleMs() { return m_manifest.video_scale == 1? 1: m_manifest.video_scale/1000; }
	VO_U64  GetTimeScaleMsAudio() { return  m_manifest.audio_scale== 1? 1: m_manifest.audio_scale/1000; }
	//return 90;}
    VO_S64  get_audiostarttime();
	//VO_BOOL get_segmentItem(Representation *  rpe_index,Seg_item ** ptr_item,  VO_S64 start_time );
	VO_BOOL get_segmentItem(Seg_item ** ptr_item,  VO_U64 start_time );
	VO_BOOL get_segmentItem(Seg_item ** ptr_item,  int ini_chunk );
	VO_BOOL get_segmentItem(Seg_item ** ptr_item,  int ini_chunk,int type);
	VO_CHAR  * getRepresentationUrl(int stream_index,int representation_index);
	VO_S32  pop_fragment( Seg_item * ptr_item, VO_CHAR * ptr_path, VO_S64 last_network_bitrate,VO_S64 &last_fragment_duration);
	VO_S64  determine_bitrate( VO_S64 last_network_bitrate ,VO_S64 duration);
	VO_S64  get_cur_bitrate();
	VO_VOID make_requesturl(VO_CHAR *requestUrl, VO_S32 streamIndex,VO_S32 representation_index);
	VO_VOID reduce_QualityLevel();
	VO_U32  get_audioCodecType();
	VO_U32  get_videoCodecType();
	VO_S32  get_duration();
	VO_S64  set_pos( VO_S64 pos );
	//VO_CHAR * get_codecPrivateData(int representation_index);
	VO_S32  setLang(VO_CHAR * lang);
    VO_VOID destroy();
	VO_VOID hexstr2byte( VO_PBYTE ptr_hex , VO_CHAR * str_hex, VO_S32 length );
	VO_BOOL get_fragmentItem( Seg_item ** ptr_item,  VO_S64 start_time,Representation * pp );
	//void   ReleaseProgramInfo();
	//VO_BOOL CreateProgramInfo();
#ifdef _USE_NEW_BITRATEADAPTATION
/*add by leon*/
	void AddtimeStamp2Map(VO_U64 timeStamp, int nFlag = 0);
	int CheckDelayTimeStamp(int delayTime, VO_U64 timeStamp);
	VO_BOOL GetIsSmoothPlayed(int index);
	void FlushMap();
	VO_VOID CheckBetterQL();
#endif
	int GetSegmentCounts();
	int FindIndexByStartTime(VO_U64 startTime);
	VO_S64  GetLastStartTime(VO_U8 isVideo);
public :
	//VO_S32 streamCount;
	VO_S32 max_chunks;
	VO_S64 gbitrate[3];
	VO_S64 gduration[3];
	VO_S32 m_playlist_duration;
	int num;
	Representation  m_better_QualityLevel;
	VO_BOOL bitrate_changed_Video;
	VO_BOOL bitrate_changed_Audio;
	VO_S32 last_bitrate_index;

	VO_S32 m_playlist_item_count;
//	VO_BOOL m_stop_thread;
   // VO_S32 max_chunks;
	//VO_S64 m_nVideoChunk_id;
   //VO_S64 m_nAudioChunk_id;
public:	
	VO_CHAR m_url[1024];
	VO_S64 max_bitrate;	
	VO_S64 min_bitrate;
	VO_S32 m_video_index;
	VO_S32 m_audio_index;
	VO_S32 cur_bitrate_index;
	VO_S64 last_QL_bitrate;
	VO_BOOL m_bNeedReduceQualityLevel;
	VO_S32 m_nReduceNum;
	VO_CHAR request_video[1024];
	VO_CHAR request_audio[1024];
	VO_S64 m_nVideoChunk_id;
	VO_S64 m_nAudioChunk_id;
	bool m_bStartBitrate;

	VO_U32 SetBACAP(VO_SOURCE2_CAP_DATA* pCap);

#ifdef _USE_NEW_BITRATEADAPTATION
private:
	CBitrateMap m_BitrateMap[20];
	VO_S32 m_nCurBitrateCounts;

#endif
	
	

};
#ifdef _VONAMESPACE
}
#endif

#endif