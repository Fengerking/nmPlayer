#pragma once

#include "vo_manifest_reader.h"
#include "voCMutex.h"
#include "voLiveSourcePlus.h"
#include "CBitrateMap.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#ifdef _IOS
#ifdef _ISS_SOURCE_
namespace _ISS{
#endif
#endif


class vo_manifest_manager
{
public:
	vo_manifest_manager(void);
	~vo_manifest_manager(void);
	
	
	VO_U64 GetTimeScaleMs() { return m_manifest.timeScale == 0? 1: m_manifest.timeScale/1000; }
	VO_BOOL SetManifest( vo_http_stream *ptr_fragment, VO_CHAR * url );
#ifdef _Amazon
	VO_BOOL SetManifest( VO_PBYTE pBuffer, VO_U32 uSize);
#endif
	QualityLevel * get_qualityLevel( VO_S32 index_item );

	VO_S64 get_cur_bitrate();

	VO_S64 get_audiostarttime();

	VO_S32 pop_fragment( FragmentItem * ptr_item, VO_CHAR * ptr_path, VO_S64 last_network_bitrate,VO_S64 &last_fragment_duration);
	FragmentItem *GetNextChunk(FragmentItem *ptr_item);
	FragmentItem *GetFirstChunk();

	VO_BOOL parse_manifest(vo_http_stream *ptr_fragment );
#ifdef _Amazon
	VO_BOOL parse_manifest( VO_PBYTE pBuffer, VO_U32 uSize );
#endif
	VO_VOID TooHightQualityLevel();
	void AddtimeStamp2Map(VO_U64 timeStamp, int nFlag = 0);
	int CheckDelayTimeStamp(int delayTime, VO_U64 timeStamp);
	VO_BOOL GetIsSmoothPlayed(int index);
	void FlushMap();

	VO_VOID replace(char *source,const char *sub,const char *rep );

	VO_VOID close();

	VO_BOOL is_live(){ return m_reader.is_live_stream(); }

	VO_S32 get_duration();

	VO_S64 set_pos( VO_S64 pos );

	SmoothStreamingMedia m_manifest;
	vo_manifest_reader m_reader;

	QualityLevel m_better_QualityLevel;
	VO_S64 last_QL_bitrate;
	VO_BOOL bitrate_changed_Video;
	VO_BOOL bitrate_changed_Audio;
	VO_S32 max_chunks;
	VO_S32 cur_bitrate_index;
	VO_S32 last_bitrate_index;
	VO_S32 m_video_index;
	VO_S32 m_audio_index;

protected:
	VO_VOID CheckBetterQL();
	VO_VOID reduce_QualityLevel();
	VO_VOID make_requesturl(VO_CHAR *requestUrl, VO_S32 streamIndex);

	VO_VOID destroy();

	VO_VOID get_absolute_path( VO_CHAR * str_absolute_path , VO_CHAR * str_filename , VO_CHAR * str_relative_path );

	VO_S64 determine_bitrate(VO_S64 last_network_bitrate ,VO_S64 duration);

	VO_BOOL CreateProgramInfo();
	void	ReleaseProgramInfo();
private:

	VO_S64 max_bitrate;
	VO_S64 min_bitrate;
	VO_S32 m_nReduceNum;
	VO_CHAR m_url[1024];

	VO_CHAR request_video[1024];
	VO_CHAR request_audio[1024];

	VO_S32 m_playlist_duration;

	//QualityLevel * ptr_item;

	VO_BOOL m_bNeedReduceQualityLevel;
	VO_S64 m_nVideoChunk_id;
	VO_S64 m_nAudioChunk_id;

	
	double gbitrate[3];
	double gduration[3];
	int num;
	int maxbitrate ;

	bool m_bStartBitrate;

public:
	VO_U32 GetStreamCount();
	VO_U32 GetStreamInfo  (VO_U32 nStreamID, VO_SOURCE2_STREAM_INFO **ppStreamInfo);
	VO_U32 GetProgramInfo (VO_SOURCE2_PROGRAM_INFO **ppProgramInfo){ *ppProgramInfo = m_pPlusProgramInfo; return 0;}

	VO_U32 GetDrmInfo ( VO_SOURCE2_DRM_INFO **ppDrmInfo);
	VO_U32 SetBACAP(VO_SOURCE2_CAP_DATA* pCap);
	VO_U32 SetDrmCustomerType(VOSMTH_DRM_TYPE type);
private:
	VO_U32 m_nStreamCount;
	VO_SOURCE2_STREAM_INFO **m_ppPlusSample;
	VO_SOURCE2_DRM_INFO *m_pDRMInfo;
 
	VO_SOURCE2_PROGRAM_INFO *m_pPlusProgramInfo;

	CBitrateMap m_BitrateMap[20];
};

#ifdef _IOS
#ifdef _ISS_SOURCE_
}	// end of _ISS namespace
#endif
#ifdef _VONAMESPACE
}
#endif

#endif
