
#ifndef __VO_MPD_H__

#define __VO_MPD_H__
#include "voParser.h"
#include "voString.h"
#include "voType.h"
#include "vompType.h"
class vo_MPD
{
public:
	vo_MPD(void);
	~vo_MPD(void);
private:
	VO_CHAR	baseurl[32];
	VO_S64  mediaPresentationDuration;
	VO_BOOL m_is_live;
	VO_BOOL m_is_need_update;
	VO_S32  peroid_index;
    VO_S32  group_count;
	VO_S64  minBufferTime;
	VO_S64  m_start_time;
public:
	VO_S64  get_mpd_duration();
	VO_BOOL is_live();
	VO_BOOL is_update();
	VO_VOID set_mpd_duration(VO_CHAR *attr);
	VO_VOID set_is_live(VO_CHAR *attr);
	VO_VOID set_is_update(VO_CHAR *attr);

};
#endif