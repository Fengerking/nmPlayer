#ifndef __VO_TIMESTAMP_RECALCULATOR_H__

#define __VO_TIMESTAMP_RECALCULATOR_H__


#include "voType.h"
#include "voCMutex.h"
#include "voSource2.h"
#include "voDSType.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class vo_mediatype_checker
{
public:
	vo_mediatype_checker();
	~vo_mediatype_checker();

	void add_sample( VO_U32 type , const _SAMPLE * const ptr_sample );

	VO_SOURCE2_MEDIATYPE get_mediatype();

private:
	VO_SOURCE2_MEDIATYPE m_mediatype;
};

class vo_timestamp_recalculator
{
public:
	vo_timestamp_recalculator();
	~vo_timestamp_recalculator();

	VO_VOID recalculate( VO_U32 type , _SAMPLE* ptr_sample );
	VO_VOID seek(VO_U64 uPos);
protected:
	VO_BOOL need_recalculate( VO_U64 last_timestamp , _SAMPLE* ptr_sample , VO_BOOL isforce );
	VO_VOID recalculate_offset(VO_BOOL bChunkBegin);
	
protected:
	VO_U64 m_last_audio;
	VO_U64 m_last_video;
	VO_S64 m_audio_offset;
	VO_S64 m_video_offset;
	VO_S64 m_subtitle_offset;
	VO_S64 m_customtag_offset;
	VO_U64 m_uStartOffset;

	VO_U64 m_last_calaudio;
	VO_U64 m_last_calvideo;

	VO_BOOL m_b_firstaudio;
	VO_BOOL m_b_firstvideo;
	VO_BOOL m_b_firstsubtitle;
	VO_BOOL m_b_firstcustomtag;

	VO_U64 m_audio_jumpcount;
	VO_U64 m_video_jumpcount;

	VO_U32 m_offset_notsync_count;

	vo_mediatype_checker m_mediatype;

	voCMutex m_Lock;
};

#ifdef _VONAMESPACE
}
#endif

#endif