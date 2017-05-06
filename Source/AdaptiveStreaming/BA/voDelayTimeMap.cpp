
#include "voDelayTimeMap.h"
#include "voString.h"
#include "voLog.h"
#include "voSource2.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifndef LOG_TAG
#define LOG_TAG "voDelayTimeMap"
#endif

#define MINDELAYLEVEL 140

voDelayTimeMap::voDelayTimeMap( VO_U32 map_size )
:m_map_size( map_size )
,m_startindex(0)
,m_curbitrate(0)
{
	m_ptr_bitratemap = new voBitrateMap[ m_map_size ];

	memset( m_ptr_bitratemap , 0 , sizeof( voBitrateMap ) * m_map_size );
}

voDelayTimeMap::~voDelayTimeMap()
{
	if( m_ptr_bitratemap )
		delete []m_ptr_bitratemap;

	m_ptr_bitratemap = 0;

	m_map_size = 0;
}

VO_VOID voDelayTimeMap::add_starttime( VO_U64 starttime , VO_U64 bitrate , VO_BOOL isFormatChanged )
{
	if( !m_ptr_bitratemap )
		return;

	m_ptr_bitratemap[m_startindex].bitrate = bitrate;
	m_ptr_bitratemap[m_startindex].starttime = starttime;
	m_ptr_bitratemap[m_startindex].endtime = MAX_U64;
	m_ptr_bitratemap[m_startindex].bisformatchanged = isFormatChanged;
	m_ptr_bitratemap[m_startindex].delaycount = 0;
}

VO_VOID voDelayTimeMap::add_endtime( VO_U64 endtime )
{
	if( !m_ptr_bitratemap )
		return;

	m_ptr_bitratemap[m_startindex].endtime = endtime;

	VOLOGI( "BAINFO StartTime: %lld EndTime %lld Bitrate %d" , m_ptr_bitratemap[m_startindex].starttime , m_ptr_bitratemap[m_startindex].endtime , m_ptr_bitratemap[m_startindex].bitrate );

	m_startindex++;

	if( m_startindex >= m_map_size )
		m_startindex = 0;

	print_map();
}

VO_VOID voDelayTimeMap::add_delay( VO_U64 playtime , VO_S32 delay_time )
{
	if( !m_ptr_bitratemap )
		return;

	for( VO_S32 i = 0 ; i < m_map_size ; i++ )
	{
		if( m_ptr_bitratemap[i].endtime != 0 && playtime >= m_ptr_bitratemap[i].starttime && playtime <= m_ptr_bitratemap[i].endtime )
		{
#ifdef _BAINFODETAIL
			if( m_curbitrate != m_ptr_bitratemap[i].bitrate )
			{
				VOLOGI( "BAINFO Video Delay: %d %lld" , delay_time , playtime );
			}
#endif

			m_curbitrate = m_ptr_bitratemap[i].bitrate;
			if( delay_time >= MINDELAYLEVEL )
			{
				if( m_ptr_bitratemap[i].bisformatchanged )
				{
					/*if( playtime >= m_ptr_bitratemap[i].starttime + 1000 )
					{
						VOLOGI( "BAINFO Big Delay! Delay Time: %d Delay Bitrate %d" , delay_time , m_ptr_bitratemap[i].bitrate );
						m_ptr_bitratemap[i].delaycount++;
					}
					else*/
					{
						VOLOGI( "BAINFO Drop this delay since format changed!" );
					}
				}
				else
				{
					//to avoid drop before new format of next chunk
					VO_S32 next_chunk = i+1;

					if( next_chunk >= m_map_size )
						next_chunk = 0;

					if( m_ptr_bitratemap[i].endtime != 0 &&
						m_ptr_bitratemap[next_chunk].starttime > m_ptr_bitratemap[i].endtime && 
						m_ptr_bitratemap[next_chunk].bisformatchanged /*&& 
						playtime + 1000 >= m_ptr_bitratemap[i].endtime*/ )
					{
						VOLOGI( "BAINFO Drop this delay since format changed will happen!" );
					}
					//
					else
					{
						VOLOGI( "BAINFO Big Delay! Delay Time: %d Delay Bitrate %d" , delay_time , m_ptr_bitratemap[i].bitrate );
						m_ptr_bitratemap[i].delaycount++;
					}
				}

				return;
			}
		}
	}

	if( delay_time > MINDELAYLEVEL )
	{
		VOLOGI( "BAINFO Can not find the delay bitrate in the map!" );
	}
}

struct BitrateDelayCount
{
	VO_U32 bitrate;
	VO_U32 delaycount;
};

VO_U32 voDelayTimeMap::get_maxbitrate_withoutdelay()
{
	if( !m_ptr_bitratemap )
		return MAX_U32;

	BitrateDelayCount * ptr_counts = new BitrateDelayCount[ m_map_size ];
	VO_U32 size = 0;

	memset( ptr_counts , 0 , sizeof( BitrateDelayCount ) * m_map_size );

	for( VO_S32 i = 0 ;  i < m_map_size ; i++ )
	{
		if( m_ptr_bitratemap[i].endtime != 0 )
		{
			VO_U32 j = 0;

			for (  ; j < size ; j++ )
			{
				if( ptr_counts[j].bitrate == m_ptr_bitratemap[i].bitrate )
				{
					ptr_counts[j].delaycount += m_ptr_bitratemap[i].delaycount;
					break;
				}
			}

			if( j == size )
			{
				ptr_counts[size].bitrate = m_ptr_bitratemap[i].bitrate;
				ptr_counts[size].delaycount += m_ptr_bitratemap[i].delaycount;
				size++;
			}
		}
	}

	VO_U32 min_delaybitrate = MAX_U32;

	for( VO_S32 i = 0 ; i < size ; i++ )
	{
		if( ptr_counts[ i ].delaycount > 0 )
		{
			if( ptr_counts[ i ].bitrate < min_delaybitrate )
				min_delaybitrate = ptr_counts[ i ].bitrate;
		}
	}

	delete []ptr_counts;

	VO_U32 bitrate = min_delaybitrate - 1;

	return bitrate;
}

VO_VOID voDelayTimeMap::reset()
{
	for( VO_S32 i = 0 ;  i < m_map_size ; i++ )
	{
		m_ptr_bitratemap[i].delaycount = 0;
	}
}

VO_VOID voDelayTimeMap::print_map()
{
	VOLOGI( "BAINFO BAMAP START**************************" );
	for( VO_S32 i = 0 ; i < m_map_size ; i++ )
	{
		if( m_ptr_bitratemap[i].endtime != 0 )
		{
			VOLOGI( "BAINFO BAMAP Start Time: %lld End Time: %lld Bitrate: %d FormatChange: %s DelayCount: %d" , m_ptr_bitratemap[i].starttime , m_ptr_bitratemap[i].endtime , m_ptr_bitratemap[i].bitrate , m_ptr_bitratemap[i].bisformatchanged ? "Ture":"False" , m_ptr_bitratemap[i].delaycount );
		}
	}
	VOLOGI( "BAINFO BAMAP   END**************************" );
}

voNetworkPredictor::voNetworkPredictor( VO_U32 cache_size )
{
	m_ptr_cache = new VO_U32[cache_size];
	m_cachesize = cache_size;
	m_cur_cachesize = 0;
	m_next_cachepos = 0;
}

voNetworkPredictor::~voNetworkPredictor()
{
	delete []m_ptr_cache;
}

VO_VOID voNetworkPredictor::set_current_network( VO_U32 uDownloadTime , VO_U32 uDownloadSize )
{
	m_ptr_cache[m_next_cachepos] = ( VO_U32 )( uDownloadSize * 8000. / uDownloadTime );

	m_next_cachepos++;

	if( m_next_cachepos >= m_cachesize )
		m_next_cachepos = 0;

	if( m_cur_cachesize < m_cachesize )
		m_cur_cachesize++;
}

VO_U32 voNetworkPredictor::get_predict_network( VO_U32 * ptr_bitrate )
{
	VO_U32 max = 0;
	VO_U32 min = 0xffffffff;

	float average_speed = 0;

	for( VO_S32 i = 0 ; i < m_cur_cachesize ; i++ )
	{
		if( m_ptr_cache[i] > max )
			max = m_ptr_cache[i];

		if( m_ptr_cache[i] < min )
			min = m_ptr_cache[i];

		average_speed = average_speed + m_ptr_cache[i];
	}

	average_speed = average_speed / m_cur_cachesize;

	float delta = 0;

	for( VO_S32 i = 0 ; i < m_cur_cachesize ; i++ )
	{
		delta += m_ptr_cache[i] > average_speed ? m_ptr_cache[i] - average_speed : average_speed - m_ptr_cache[i];
	}

	delta = delta / m_cur_cachesize;

	VO_U32 pos = m_next_cachepos;

	if( m_next_cachepos == 0 )
		pos = m_cur_cachesize - 1;
	else
		pos = pos - 1;

	if( m_cur_cachesize < 1 )
		return VO_RET_SOURCE2_NEEDRETRY;

	
	if( delta < 100000 ) // not change so frequently
	{
		*ptr_bitrate = m_ptr_cache[pos];
	}
	else
	{
		//use_speed = m_average_speed < m_total_average_speed ? m_average_speed : m_total_average_speed;
		//use_speed = use_speed < m_ptr_judgelist[ m_pos - 1 ] ? use_speed : m_ptr_judgelist[ m_pos - 1 ];
		*ptr_bitrate = average_speed < m_ptr_cache[ pos ] ? average_speed : m_ptr_cache[ pos ];
	}

	return VO_RET_SOURCE2_OK;
}