
#ifndef __VODELAYTIMEMAP_H__

#define __VODELAYTIMEMAP_H__

#include "voType.h"

#define MAX_U64			0xffffffffffffffffll
#define MAX_U32			0xffffffff
#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif
struct voBitrateMap
{
	VO_U64 starttime;
	VO_U64 endtime;
	VO_U32 bitrate;
	VO_BOOL bisformatchanged;

	VO_U32 delaycount;
};

class voDelayTimeMap
{
public:
	voDelayTimeMap( VO_U32 map_size );
	~voDelayTimeMap();

	VO_VOID add_starttime( VO_U64 starttime , VO_U64 bitrate , VO_BOOL isFormatChanged );
	VO_VOID add_endtime( VO_U64 endtime );

	VO_VOID add_delay( VO_U64 playtime , VO_S32 delay_time );

	VO_U32 get_maxbitrate_withoutdelay();

	VO_U32 get_curbitrate(){ return m_curbitrate; }

	VO_VOID reset();

protected:
	VO_VOID print_map();

protected:

	voBitrateMap * m_ptr_bitratemap;
	VO_U32 m_startindex;
	VO_U32 m_map_size;

	VO_U32 m_curbitrate;
};


class voNetworkPredictor
{
public:
	voNetworkPredictor( VO_U32 cache_size );
	~voNetworkPredictor();

	VO_VOID set_current_network( VO_U32 uDownloadTime , VO_U32 uDownloadSize );
	VO_U32 get_predict_network( VO_U32 * ptr_bitrate );

protected:
	VO_U32 m_cachesize;
	VO_U32 * m_ptr_cache;
	VO_U32 m_cur_cachesize;
	VO_U32 m_next_cachepos;

};

#ifdef _VONAMESPACE
}
#endif
#endif