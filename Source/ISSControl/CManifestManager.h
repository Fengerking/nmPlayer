#ifndef		_VO_MAINFEST_MANGAGER_H
#define		_VO_MAINFEST_MANGAGER_H
#include "voType.h"
#include "voSource2.h"
#include "voLiveSourcePlus.h"

#define STREAM_NUMBER 2
#define DEFAULT_TIME_SCALE 10000000

class CManifestManager
{
public:
	CManifestManager(void);
	~CManifestManager(void);


	VO_U64 GetTimeScaleMs() { return DEFAULT_TIME_SCALE/1000; }

	VO_S64 get_cur_bitrate();

	VO_S64 GetAudioStartTime();

	VO_S32 pop_fragment( FragmentItem * ptr_item, VO_CHAR * ptr_path, VO_S64 last_network_bitrate,VO_S64 &last_fragment_duration);
	FragmentItem *GetNextChunk(FragmentItem *ptr_item);
	FragmentItem *GetFirstChunk();

	VO_VOID replace(char *source,const char *sub,const char *rep );
	VO_VOID close();
	VO_S32 get_duration();
	VO_S64 set_pos( VO_S64 pos );

	VO_S64 last_QL_bitrate;
	VO_BOOL bitrate_changed_Video;
	VO_BOOL bitrate_changed_Audio;
	VO_S32 max_chunks;
	VO_S32 cur_bitrate_index;
	VO_S32 last_bitrate_index;
	VO_S32 m_video_index;
	VO_S32 m_audio_index;

protected:
	VO_VOID make_requesturl(VO_CHAR *requestUrl, VO_SOURCE2_OUTPUT_TYPE  streamIndex);

	VO_VOID destroy();

	VO_VOID get_absolute_path( VO_CHAR * str_absolute_path , VO_CHAR * str_filename , VO_CHAR * str_relative_path );

	VO_S64 determine_bitrate(VO_S64 last_network_bitrate ,VO_S64 duration);

private:

	VO_S64 max_bitrate;
	VO_S64 min_bitrate;
	VO_S32 m_nReduceNum;
	VO_CHAR m_url[1024];

	VO_CHAR request_video[1024];
	VO_CHAR request_audio[1024];

	VO_S32 m_playlist_duration;

	VO_BOOL m_bNeedReduceQualityLevel;
	VO_S64 m_nVideoChunk_id;
	VO_S64 m_nAudioChunk_id;


	double gbitrate[3];
	double gduration[3];
	int num;
	int maxbitrate ;

	bool m_bStartBitrate;

	VO_SOURCE2_SUBSTREAM_INFO * GetSubStreamInfo( VO_U32 nSubStreamID );
public:
	VO_SOURCE2_SUBSTREAM_INFO * GetCurSubStreamInfo(VO_SOURCE2_OUTPUT_TYPE type);
	VO_BOOL GetFragmentItem( FragmentItem ** ptr_item,  VO_S64 start_time );
	VO_SOURCE2_SUBSTREAM_INFO *GetSubStreamInfoByBitrate( VO_S32 bitrate );
	void SetPlaylist(FragmentItem *item);
	void SetStreamInfo(VO_SOURCE2_OUTPUT_TYPE type,  VO_SOURCE2_STREAM_INFO *pStream);
	void SetStreamIndex(VO_SOURCE2_OUTPUT_TYPE type,  VO_U32 nIndex);
	void SetManifest( VO_CHAR * url );
private:


	VO_SOURCE2_STREAM_INFO *m_pSSInfo_video;
	VO_SOURCE2_SUBSTREAM_INFO *m_pCurSSSInfo_video;
	VO_SOURCE2_STREAM_INFO *m_pSSInfo_audio;
	VO_SOURCE2_SUBSTREAM_INFO *m_pCurSSSInfo_audio;

	FragmentItem *m_pPlaylist;

	VO_S64 FindFragmentStartTime( VO_S64 pos,int nIsVideo );
};

#endif