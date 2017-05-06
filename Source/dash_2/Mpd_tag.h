
#ifndef _Mpd_tag_H
#define _Mpd_tag_H

#include "voType.h"
#include "voSource2.h"
#include "tinyxml.h"
#include  "Common_Tag.h"




#define MPD_ID					"id"
#define MPD_PFOFILE				"profiles"
#define MPD_TYPE                 "type"
#define MPD_AVAIL_START_TIME     "availabilityStartTime"
#define MPD_AVAIL_END_TIME        "availabilityEndTime"
#define MPD_MEFIA_DURATION         "mediaPresentationDuration"
#define MPD_DASH_BufferDepth      "timeShiftBufferDepth"
#define MPD_DASH_UPDATE_PERIOD       "minimumUpdatePeriod" 
#define MPD_MIN_BUFFER_TIME          "minBufferTime"
#define MPD_MAX_SUGGEST_DURATION        "maxSegmentDuration"
#define MPD_MIN_SUGGEST_DURATION        "minSegmentDuration"



class Mpd_tag:public Common_Tag
	{
	public:
		Mpd_tag(void);
		~Mpd_tag(void);
	public:
		virtual VO_U32 Init(TiXmlNode* pNode);
	public:
		VO_U32 GetID(){return m_uID;}
		VO_S64 GetDuration(){return m_duration;}
		VO_S64 GetStartTime(){return m_avail_start_time;}
		VO_S64 GetEndTime(){return m_avail_end_time; }
		VO_S64 GetMaxDuration(){return max_seg_duration;}
		VO_S64 GetMinDuration(){return min_seg_duration;}
		VO_S64 GetUpdatePeriod(){return m_update_period;}
		VO_S64 GetBufferTime(){return m_buffer_time;}

		VO_U32 GetProfile(){ return m_is_live;}
		VO_BOOL GetLiveType(){ return m_is_need_update;}
		
	private:
		VO_U32	m_uID;
		VO_U32 m_is_live;
		VO_BOOL m_is_need_update;
		VO_S64  m_avail_start_time;
		VO_S64  m_avail_end_time;
		VO_S64  m_duration;
		VO_S64  m_update_period;
		VO_S64  m_buffer_time;
		VO_S64  max_seg_duration;
		VO_S64  min_seg_duration;
		Common_Tag * m_comtag;

};
#endif