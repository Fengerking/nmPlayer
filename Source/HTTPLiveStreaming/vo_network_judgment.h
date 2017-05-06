
#ifndef __VO_NETWORK_JUDGMENT_H__

#define __VO_NETWORK_JUDGMENT_H__

#include "voType.h"
#include "voSource2.h"

#define TIMEBITRATE_ARRAYSIZE 6
#define FUZZYMATRIXSIZE 30

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


struct TIME_BITRATE_ITEM
{
    VO_S64 start_time;
    VO_S64 end_time;
    VO_S64 bitrate;
    VO_S32 delaytimes;
    VO_S32 smalldelaytimes;
};

struct BITRATE_DELAYTIME_ITEM
{
    VO_S64 bitrate;
    VO_S32 delaytime;
    VO_S32 smalldelaytime;
};

class vo_network_judgment
{
public:
	vo_network_judgment( VO_S32 judge_size );
	~vo_network_judgment();

	VO_VOID add_item( VO_S64 speed );
    VO_VOID get_right_bandwidth( VO_S64 current_bandwidth , VO_S64* ptr_bandwidth );

    VO_VOID add_starttime( VO_S64 start_time );
    VO_VOID add_endtime_bitrate( VO_S64 end_time , VO_S64 bitrate );
    VO_VOID add_delaytime( VO_S64 delay_time , VO_S64 play_time );

    VO_VOID set_playlist( VO_S64 * ptr_playlist , VO_S32 size );
    VO_VOID set_default_streambandwidth( VO_S64 bandwidth );
	VO_VOID setCpuInfo(VO_VOID*  pCpuInfo);
	VO_VOID setCapInfo(VO_VOID*  pCapInfo);
	VO_VOID setStartCapInfo(VO_VOID*  pCapInfo);
	
    VO_VOID load_config( char * path );

    VO_VOID flush();
	VO_VOID reset_all();
    VO_VOID SetTheCPUAdaptionWorkMode(VO_U32  ulCPUAdaptionWorkMode);

protected:
    VO_S64 get_judgment( VO_S64 current_bandwidth , VO_S64 * ptr_next_bandwidth , VO_S64 * ptr_limit );
	VO_VOID get_machine_top_speed();
    VO_VOID calculate_top_speed();

    VO_BOOL get_isDualCore();

private:

	VO_S64 * m_ptr_judgelist;
	VO_S32 m_listsize;
	VO_S32 m_usedsize;
	VO_S32 m_pos;

    VO_S64 * m_ptr_playlist;
    VO_S32 m_playlist_count;

	VO_S32 m_total_number;

	VO_S64 m_average_speed;
	VO_S64 m_total_average_speed;

	VO_S64 m_machine_top_speed;

    TIME_BITRATE_ITEM m_timebitrate_array[TIMEBITRATE_ARRAYSIZE];
    VO_S32 m_array_start;

    VO_S64 m_suggest_bandwidth;

    VO_S64 m_last_timestamp;

    VO_BOOL m_bagressive;

    VO_S32 m_current_index;

    VO_S32 m_benable_cpuba;
    VO_S32 m_benable_cap;
    VO_S32 m_cap_singlecore;
    VO_S32 m_cap_dualcore;

    VO_S32 m_judgment_counter;

    VO_BOOL m_b_isPerformanceissue;
    VO_S32 m_afterPerformanceCount;
	VO_U32 m_ulProfile;

	VO_S64 m_last_downloadspeed;

	VO_SOURCE2_CPU_INFO    m_sCpuInfo;
    VO_SOURCE2_CAP_DATA    m_sCapInfo;
	VO_SOURCE2_CAP_DATA    m_sStartCapInfo;
	VO_U32                 m_ulCapFromOutSide;
};

#ifdef _VONAMESPACE
}
#endif


#endif
