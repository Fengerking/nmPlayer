
#include "voBitrateAdaptationImp.h"
#include "voLog.h"
#include "voDelayTimeMap.h"
#include "voOSFunc.h"
#include "vo_thread.h"
#include "voCMutex.h"
#include "voDSType.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifndef LOG_TAG
#define LOG_TAG "voBitrateAdaptation"
#endif

#define LOWCPUUSAGE 50

#define BITRATECHANGEFREQUENTDURATION 60000

#define INTERVALOFFREQUENTCHANGE 10000
#define INTERVALAFTERFREQUENTCHANGE 30000

#define IGNOREDELAYRANGE 5000
#define IGNORECHUNKSTARTDELAYCOUNT 0


class voBitrateAdaptation
	:public vo_thread
{
public:
	voBitrateAdaptation()
		:m_bitratecount(0)
		,m_ptr_bitrate(0)
		,m_cap(0xffffffff)
		,m_lastdownloadspeed(0xffffffff)
		,m_lastdownloadtime(1)
		,m_bitrate_map( 30 )
		,m_performancegood( VO_TRUE )
		,m_lastchunktime(0)
		,m_is_stop( VO_FALSE )
		,m_delaytimebase( 0 )
		,m_lastBAchunktime(0)
		,m_lastchunkendtime(0)
		,m_delaycount(0)
		,m_hard_cap(0xffffffff)
		,m_upper_cap(0xffffffff)
		,m_lower_cap(0)
		,m_manualCPUUsage(VO_FALSE)
		,m_force(0xffffffff)
		,m_recommend(0xffffffff)
		,m_default(0xffffffff)
		,m_isfirstba(VO_TRUE)
		,m_is_docpuusage(VO_TRUE)
		,m_startcap(0xffffffff)
		,m_predictor(6)
		,m_cur_downloadchunk_bitrate(0xffffffff)
		,m_b_cpuba_disable(VO_FALSE)
		,m_b_pre_cpuba_disable(VO_FALSE)
		,m_currbitrate(0xffffffff)
		,m_average_count(0)
		,m_average_CPU(0)
		,m_average_Freq(100)
		,m_playback_speed(1)
		,m_buffer_duration(0)
	{
	}

	virtual ~voBitrateAdaptation(){}

	VO_U32 open( VO_SOURCE2_BITRATE_INFO * pBitrateInfo )
	{
		m_bitratecount = pBitrateInfo->uItemCount;
		
		if( m_ptr_bitrate )
			delete []m_ptr_bitrate;

		m_ptr_bitrate = new VO_SOURCE2_BITRATE_INFOITEM[m_bitratecount];

		for( VO_S32 i = 0 ; i < m_bitratecount ; i++ )
		{
			m_ptr_bitrate[i].uBitrate = pBitrateInfo->pItemList[i].uBitrate;
			m_ptr_bitrate[i].uSelInfo = pBitrateInfo->pItemList[i].uSelInfo;

			if( pBitrateInfo->pItemList[i].uSelInfo & VO_SOURCE2_SELECT_RECOMMEND )
				m_recommend = pBitrateInfo->pItemList[i].uBitrate;
			else if( pBitrateInfo->pItemList[i].uSelInfo & VO_SOURCE2_SELECT_DEFAULT )
				m_default = pBitrateInfo->pItemList[i].uBitrate;
			else if( pBitrateInfo->pItemList[i].uSelInfo & VO_SOURCE2_SELECT_FORCE )
				m_force = pBitrateInfo->pItemList[i].uBitrate;
		}

		VOLOGI( "BAINFO Bitrate Info Begin: " );

		for( VO_S32 i = 0 ; i < m_bitratecount ; i++ )
		{
			VO_U32 min_bitrate = 0xffffffff;
			VO_U32 min_bitrate_selinfo = 0;
			VO_U32 min_index = i;

			for( VO_S32 j = i ; j < m_bitratecount ; j++ )
			{
				if( m_ptr_bitrate[j].uBitrate < min_bitrate )
				{
					min_bitrate = m_ptr_bitrate[j].uBitrate;
					min_bitrate_selinfo = m_ptr_bitrate[j].uSelInfo;
					min_index = j;
				}
			}

			VOLOGI( "BAINFO Index %d Bitrate %d SelInfo %d" , i , min_bitrate , min_bitrate_selinfo );

			m_ptr_bitrate[min_index].uBitrate = m_ptr_bitrate[i].uBitrate;
			m_ptr_bitrate[min_index].uSelInfo = m_ptr_bitrate[i].uSelInfo;

			m_ptr_bitrate[i].uBitrate = min_bitrate;
			m_ptr_bitrate[i].uSelInfo = min_bitrate_selinfo;
		}

		VOLOGI( "BAINFO Bitrate Info End: " );

		m_is_stop = VO_FALSE;
		begin();

		return VO_RET_SOURCE2_OK;
	}

	VO_VOID close()
	{
		m_is_stop = VO_TRUE;
		m_is_docpuusage = VO_FALSE;

		stop();

		m_is_docpuusage = VO_TRUE;
		m_is_stop = VO_FALSE;

		if( m_ptr_bitrate )
			delete []m_ptr_bitrate;

		m_ptr_bitrate = 0;

		m_bitratecount = 0;
	}

	VO_VOID chunk_begin( VO_U32 bitrate , VO_U64 starttime  , VO_BOOL isFormatChanged )
	{
		if( m_bitratecount < 2 )
			return;

		m_lastchunktime = starttime;
		m_delaycount = 0;

		m_cur_downloadchunk_bitrate = bitrate;

		m_bitrate_map.add_starttime( starttime , bitrate , isFormatChanged );
	}

	VO_VOID chunk_end( VO_U32 downloadtime , VO_U32 downloadsize , VO_U64 endtime )
	{
		if( m_bitratecount < 2 )
			return;

		m_bitrate_map.add_endtime( endtime );

		m_lastchunkendtime = endtime;

		m_lastdownloadspeed = downloadsize * 8000. / downloadtime;
		m_lastdownloadtime = downloadtime;
		m_predictor.set_current_network( downloadtime , downloadsize );
	}

	VO_VOID add_delay( VO_S32 delay , VO_U64 playtime )
	{
		if( m_bitratecount < 2 )
			return;

		m_delaycount++;

		if( m_delaycount < IGNORECHUNKSTARTDELAYCOUNT )
			return;

		if( playtime <= m_delaytimebase + IGNOREDELAYRANGE )
			return;

		m_bitrate_map.add_delay( playtime , delay );
	}

	VO_U32 get_nextbitrate( VO_U32 * ptr_bitrate )
	{
		if( m_bitratecount <= 1 )
			return VO_RET_SOURCE2_FAIL;

		if( m_force != 0xffffffff && m_force <= m_cap )
		{
			*ptr_bitrate = m_force;
			VOLOGI( "BAINFO Next Bitrate: %d" , *ptr_bitrate );
			m_currbitrate = *ptr_bitrate;
			return VO_RET_SOURCE2_OK;
		}

		VO_U32 predict_next_downloadspeed = 0xffffffff;
		VO_U32 ret = m_predictor.get_predict_network( &predict_next_downloadspeed );

		VO_U32 network_cap = 0xffffffff;
		if(VO_RET_SOURCE2_OK == ret)
		{
			if(predict_next_downloadspeed <= m_cur_downloadchunk_bitrate)
				network_cap = predict_next_downloadspeed;
			else if(predict_next_downloadspeed < VO_U32(double(m_cur_downloadchunk_bitrate) * 1.3 + 0.5))
				network_cap = m_cur_downloadchunk_bitrate + 1;
			else
				network_cap = VO_U32(double(predict_next_downloadspeed) / 1.3 + 0.5);
			if(m_playback_speed > 1)
			{
				network_cap = (VO_U32)((float)network_cap / m_playback_speed);
			}
		}

		if( ret == VO_RET_SOURCE2_OK && network_cap < m_cur_downloadchunk_bitrate )
		{
			;
		}
		else
		{
			if( m_lastchunktime > BITRATECHANGEFREQUENTDURATION )
			{
				if( m_lastchunkendtime - m_lastBAchunktime < INTERVALAFTERFREQUENTCHANGE )
				{
					VOLOGI( "BAINFO Delay Change Bitrate!" );
					return VO_RET_SOURCE2_NEEDRETRY;
				}
			}
			else
			{
				if( m_lastchunkendtime != 0 && m_lastBAchunktime != 0 && m_lastchunkendtime - m_lastBAchunktime < INTERVALOFFREQUENTCHANGE )
				{
					VOLOGI( "BAINFO Delay Change Bitrate!" );
					return VO_RET_SOURCE2_NEEDRETRY;
				}
			}
		}

		{
			voCAutoLock lock( &m_average_lock );
			if( m_average_CPU < LOWCPUUSAGE && !m_b_cpuba_disable )
			{
				VO_S32 i = 0;

				VO_U32 avaliable_bitrate = 0;

				for( ; i < m_bitratecount ; i++ )
				{
					if( !( m_ptr_bitrate[i].uSelInfo & VO_SOURCE2_SELECT_DISABLE) )
						avaliable_bitrate = m_ptr_bitrate[i].uBitrate;

					if( m_ptr_bitrate[i].uBitrate >= m_bitrate_map.get_curbitrate() )
					{
						i++;
						break;
					}
				}

				while( i < m_bitratecount && ( m_ptr_bitrate[i].uSelInfo & VO_SOURCE2_SELECT_DISABLE ) )
					i++;

				if( i >= m_bitratecount )
					i = m_bitratecount - 1;

				if( !( m_ptr_bitrate[i].uSelInfo & VO_SOURCE2_SELECT_DISABLE) )
					avaliable_bitrate = m_ptr_bitrate[i].uBitrate;



				m_cap = avaliable_bitrate + 1 > m_cap ? avaliable_bitrate + 1 : m_cap ;

				VOLOGI( "BAINFO CPU usage looks good. Set CPU cap to: %d" , m_cap );
				m_bitrate_map.reset();
			}
			else if( m_average_Freq >= 50 )
			{
				VO_U32 possible_cap = m_bitrate_map.get_maxbitrate_withoutdelay();

				m_cap = m_cap < possible_cap ? m_cap : possible_cap;

				if( m_b_cpuba_disable )
					m_cap = 0xffffffff;

				VOLOGI( "BAINFO Set CPU cap to: %d" , m_cap );
			}
			else
			{
				VOLOGI( "BAINFO Stay CPU cap to: %d" , m_cap );
			}

			m_average_Freq = 100;
			m_average_CPU = 0;
			m_average_count = 0;
		}

		VO_U32 cap = (network_cap < m_cap) ? network_cap : m_cap;

		VOLOGI( "BAINFO CPU CAP %d BandWidth %d NETWORK CAP %d CAP Setting %d CPUBA Disable %s" , m_cap , predict_next_downloadspeed , network_cap, m_hard_cap , m_b_cpuba_disable ? "True" : "False" );
		VOLOGI( "BAINFO Upper is %d, Lower is %d",m_upper_cap,m_lower_cap);
		if( !m_b_cpuba_disable )
			cap = cap < m_hard_cap ? cap : m_hard_cap;

		cap = cap < m_upper_cap ? cap : m_upper_cap;
		
		m_performancegood = VO_TRUE;

		VO_U32 avaliable_bitrate = 0;

		if( m_isfirstba )
		{
			m_isfirstba = VO_FALSE;

			cap = cap < m_startcap ? cap : m_startcap;
			cap = cap < m_upper_cap ? cap : m_upper_cap;

			if( m_startcap == 0xffffffff )
			{
				if( m_recommend != 0xffffffff && cap >= m_recommend && m_lower_cap <= m_recommend)
				{
					*ptr_bitrate = m_recommend;

					if( m_lastbitrate != *ptr_bitrate )
					{
						m_lastBAchunktime = m_lastchunkendtime;
					}

					VOLOGI( "BAINFO Next Bitrate: %d" , *ptr_bitrate );
					m_currbitrate = *ptr_bitrate;
					return VO_RET_SOURCE2_OK;
				}
				else if( m_default != 0xffffffff && cap >= m_default && m_lower_cap <= m_default)
				{
					*ptr_bitrate = m_default;

					if( m_lastbitrate != *ptr_bitrate )
					{
						m_lastBAchunktime = m_lastchunkendtime;
					}

					VOLOGI( "BAINFO Next Bitrate: %d" , *ptr_bitrate );
					m_currbitrate = *ptr_bitrate;
					return VO_RET_SOURCE2_OK;
				}
			}
		}

		for( VO_S32 i = m_bitratecount - 1 ; i >= 0 ; i-- )
		{
			if( !( m_ptr_bitrate[i].uSelInfo & VO_SOURCE2_SELECT_DISABLE) )
				avaliable_bitrate = m_ptr_bitrate[i].uBitrate;

			if( m_ptr_bitrate[i].uBitrate <= cap && m_ptr_bitrate[i].uBitrate >= m_lower_cap && !( m_ptr_bitrate[i].uSelInfo & VO_SOURCE2_SELECT_DISABLE) )
			{
				*ptr_bitrate = m_ptr_bitrate[i].uBitrate;

				if( m_lastbitrate != *ptr_bitrate )
				{
					m_lastBAchunktime = m_lastchunkendtime;
				}
				if(check_needbitratechange(m_currbitrate,*ptr_bitrate))
				{
					VOLOGW("BAINFO delay bitrate change when referring to buffer");
					*ptr_bitrate = m_currbitrate;
					return VO_RET_SOURCE2_NEEDRETRY;
				}
				VOLOGI( "BAINFO Next Bitrate: %d" , *ptr_bitrate );
				m_currbitrate = *ptr_bitrate;
				return VO_RET_SOURCE2_OK;
			}
		}

		*ptr_bitrate = avaliable_bitrate;

		if( m_lastbitrate != *ptr_bitrate )
		{
			m_lastBAchunktime = m_lastchunkendtime;
		}
		if(check_needbitratechange(m_currbitrate,*ptr_bitrate))
		{
			VOLOGW("BAINFO delay bitrate change when referring to buffer");
			*ptr_bitrate = m_currbitrate;
			return VO_RET_SOURCE2_NEEDRETRY;
		}
		VOLOGI( "BAINFO Next Bitrate: %d" , *ptr_bitrate );
		m_currbitrate = *ptr_bitrate;
		return VO_RET_SOURCE2_OK;
	}
	VO_U32 get_minbitrate()
	{
		VO_U32 tmp_bitrate = 0xffffffff;
		for( VO_S32 i = m_bitratecount - 1 ; i >= 0 ; i-- )
		{
			if( !( m_ptr_bitrate[i].uSelInfo & VO_SOURCE2_SELECT_DISABLE) )
			{
				if(tmp_bitrate > m_ptr_bitrate[i].uBitrate)
					tmp_bitrate = m_ptr_bitrate[i].uBitrate;
			}
		}
		return tmp_bitrate;
	}
	VO_U32 get_param( VO_U32 id , VO_PTR ptr_param )
	{
		switch( id )
		{
		case VO_PID_SOURCE2_BA_MAXDOWNLOADBITRATE:
			{
				VO_U32 * ptr_maxdownloadbitrate = ( VO_U32 * )ptr_param;
				if( m_bitratecount > 1 )
				{
					*ptr_maxdownloadbitrate = m_ptr_bitrate[ m_bitratecount - 1 ].uBitrate < m_cap ? m_ptr_bitrate[ m_bitratecount - 1 ].uBitrate * 1.2 : m_cap * 1.2;
					*ptr_maxdownloadbitrate = *ptr_maxdownloadbitrate / 8;
				}
				else
				{
					*ptr_maxdownloadbitrate = m_cap;
				}
			}
			break;
		case VO_PID_SOURCE2_BA_MIN_BITRATE_PLAY:
			{
				VO_BOOL* min_bitrate_playback = (VO_BOOL *)ptr_param;
				if(m_currbitrate == get_minbitrate())
					*min_bitrate_playback = VO_TRUE;
				else
					*min_bitrate_playback = VO_FALSE;
					
			
				break;
			}
		}
		return VO_RET_SOURCE2_OK;
	}

	VO_U32 set_param( VO_U32 id , VO_PTR ptr_param )
	{
		switch( id )
		{
		case VO_PID_SOURCE2_BA_SEEK:
			{
				m_delaytimebase = *(VO_U64 *)ptr_param;
				m_performancegood = VO_FALSE;
			}
			break;
		case VO_PID_SOURCE2_BA_CAP:
			m_hard_cap = *(VO_U32*)ptr_param;
			if(m_hard_cap < m_lower_cap)
			{
				VOLOGW("m_lowe_cap set invalid value,m_hard_cap is %d,m_lower_cap is %d",m_hard_cap,m_lower_cap);
				m_lower_cap = 0;
			}
			break;
		case VO_PID_SOURCE2_BA_START_CAP:
			{
				VO_U32 tmp = *(VO_U32*)ptr_param;
				if(tmp < m_lower_cap)
				{
					VOLOGW("Wrong start cap,lower is %d, start cap is %d",m_lower_cap,tmp);
					return VO_RET_SOURCE2_INVALIDPARAM;
				}
				m_startcap = tmp;
				VOLOGI( "BAINFO Start Cap: %d" , m_startcap );
			}
			break;
		case VO_PID_SOURCE2_BA_CPUUSAGEFORTEST:
			{
				m_manualCPUUsage = VO_TRUE;

				VO_U32 usage = *(VO_U32*)ptr_param;
				if( usage > LOWCPUUSAGE )
				{
					m_performancegood = VO_FALSE;
				}
			}
			break;
		case VO_PID_SOURCE2_BA_DISABLECPUBA:
			{
				VO_BOOL * ptr_diable = ( VO_BOOL * )ptr_param;
				m_b_cpuba_disable = *ptr_diable;
				m_b_pre_cpuba_disable = m_b_cpuba_disable;
				VOLOGI( "BAINFO Diable CPUBA %s" , m_b_cpuba_disable ? "True" : "False" );
			}
			break;
		case VO_PID_SOURCE2_BA_SET_CURRENT_BITRATE:
			{
				m_currbitrate = *(VO_U32*)ptr_param;
			}
			break;
		case VO_PID_SOURCE2_BA_PLAYBACK_SPEED:
			{
				m_playback_speed = *(float*)ptr_param;
				if(m_playback_speed > 1)
					m_b_cpuba_disable = VO_TRUE;
				else
					m_b_cpuba_disable = m_b_pre_cpuba_disable;
			}
			break;
		case VO_PID_SOURCE2_BA_THRESHOLD:
			{
				VO_SOURCE2_BA_THRESHOLD* m_tmp_threshold = (VO_SOURCE2_BA_THRESHOLD*)ptr_param;
				if(m_tmp_threshold->nLower > m_hard_cap || m_tmp_threshold->nLower > m_startcap)
				{
					VOLOGW("Wrong threshold,lower is %d, cap is %d,start cap is %d",m_tmp_threshold->nLower,m_hard_cap,m_startcap);
					return VO_RET_SOURCE2_INVALIDPARAM;
				}
				m_upper_cap = m_tmp_threshold->nUpper;
				m_lower_cap = m_tmp_threshold->nLower;
			}
			break;
		case VO_PID_SOURCE2_BA_BUFFER_DURATION:
			{
				m_buffer_duration = *(VO_U32*)ptr_param;
			}
			break;
		}
		return VO_RET_SOURCE2_OK;
	}
	VO_U32 check_downloadslow( VO_U32 netspeed )
	{
		///???????????????
		VO_U32 ret = VO_DATASOURCE_RET_DOWNLOAD_SLOW;
		if(netspeed < m_cur_downloadchunk_bitrate && m_buffer_duration > 18000)
			ret = VO_DATASOURCE_RET_OK;
		if( netspeed > m_cur_downloadchunk_bitrate )
			ret = VO_DATASOURCE_RET_OK;
		VOLOGR("ret:%x, %d, %d , %d", ret,netspeed, m_cur_downloadchunk_bitrate, m_buffer_duration);
		return ret;
	}

	VO_U32 check_needbitratechange(VO_U32 prebitrate, VO_U32 nextbitrate)
	{
		if(prebitrate != 0)
		{
			if(prebitrate < nextbitrate)
			{
				VO_U32 ForcastDownloadTime = 1.1 * m_lastdownloadtime* nextbitrate / prebitrate;
				if(ForcastDownloadTime > m_buffer_duration)
				{
					VOLOGW("ForcastDownloadTime is larger than buffer duration, pre-bitrate is %d, next-bitrate is %d, last download time is %d,buffer duration is %d",prebitrate,nextbitrate,m_lastdownloadtime,m_buffer_duration);
					return VO_RET_SOURCE2_NEEDRETRY;
				}
			}
			else if(prebitrate > nextbitrate)
			{
				if(m_buffer_duration > 18000)
				{
					VOLOGW("Buffer is full enough for playing, duration is %d",m_buffer_duration);
					return VO_RET_SOURCE2_NEEDRETRY;
				}
			}
			else
			{
				VOLOGI("The same bitrate when checking");
			}
		}
		return VO_RET_SOURCE2_OK;
	}
	virtual void thread_function()
	{
		set_threadname( "CPU USAGE" );

		while( !m_is_stop && !m_manualCPUUsage )
		{
			VO_U32 sys = 0;
			VO_U32 usr = 0;
			voOS_GetCpuUsage( &sys , &usr , &m_is_docpuusage , 1 , 100 );

			if( m_is_stop )
				return;

			if( sys == -1 && usr == -1 )
				continue;

/*
			if( sys + usr > LOWCPUUSAGE )
			{
#ifdef _BAINFODETAIL
				VOLOGI( "BAINFO CPU Usage: %d" , sys + usr );
#endif
				m_performancegood = VO_FALSE;
			}*/
			VO_U32 freq = 0;
			VO_BOOL ret = voOS_GetCurrentCpuFrequencyPercent( &freq );
			freq = freq / 100;

			{
				voCAutoLock lock( &m_average_lock );

				m_average_CPU = ( m_average_CPU * m_average_count + sys + usr )/(m_average_count + 1);
				
				if( VO_TRUE == ret )
					m_average_Freq = ( m_average_Freq * m_average_count + freq )/(m_average_count + 1);

				m_average_count++;

				VOLOGI( "BAINFO Usage: %d Freq: %d AUsage: %d AFreq: %d" , sys + usr , freq , m_average_CPU , m_average_Freq );
			}

			voOS_SleepExitable( 100,&m_is_stop );
		}
	}

protected:

	VO_U32 m_bitratecount;
	VO_SOURCE2_BITRATE_INFOITEM * m_ptr_bitrate;

	VO_U32 m_cap;

	VO_U32 m_lastdownloadspeed;
	VO_U32 m_lastdownloadtime;

	voDelayTimeMap m_bitrate_map;

	VO_BOOL m_is_stop;
	VO_BOOL m_is_docpuusage;

	VO_BOOL m_performancegood;

	VO_U64 m_lastchunktime;
	VO_U64 m_lastchunkendtime;
	VO_U64 m_lastBAchunktime;

	VO_U32 m_lastbitrate;
	VO_U32 m_currbitrate;

	VO_U64 m_delaytimebase;
	VO_U32 m_delaycount;

	VO_U32 m_hard_cap;

	VO_U32 m_upper_cap;
	VO_U32 m_lower_cap;

	VO_BOOL m_manualCPUUsage;

	VO_U32 m_force;
	VO_U32 m_recommend;
	VO_U32 m_default;

	VO_BOOL m_isfirstba;

	VO_U32 m_startcap;

	VO_BOOL m_b_cpuba_disable;
	VO_BOOL	m_b_pre_cpuba_disable;

	voNetworkPredictor m_predictor;
	VO_U32 m_cur_downloadchunk_bitrate;

	VO_U32 m_average_CPU;
	VO_U32 m_average_Freq;
	VO_U32 m_average_count;
	voCMutex m_average_lock;

	VO_U32 m_buffer_duration;

	float	m_playback_speed;
};

VO_U32 BA_Init(VO_HANDLE* pHandle)
{
	voBitrateAdaptation * ptr_ba = new voBitrateAdaptation;

	if( !ptr_ba )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	*pHandle = ptr_ba;
	return VO_RET_SOURCE2_OK;
}

VO_U32 BA_Uninit(VO_HANDLE hHandle)
{
	voBitrateAdaptation * ptr_ba = (voBitrateAdaptation*)hHandle;
	if(ptr_ba)
	{
		delete ptr_ba;
		ptr_ba = NULL;
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 BA_Open( VO_HANDLE hHandle , VO_SOURCE2_BITRATE_INFO * pBitrateInfo )
{
	voBitrateAdaptation * ptr_ba = (voBitrateAdaptation*)hHandle;

	if( !ptr_ba )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	return ptr_ba->open( pBitrateInfo );
}

VO_VOID BA_Close( VO_HANDLE hHandle )
{
	voBitrateAdaptation * ptr_ba = (voBitrateAdaptation*)hHandle;

	if( !ptr_ba )
		return;

	return ptr_ba->close();
}

VO_VOID BA_StreamChunkBegin( VO_HANDLE hHandle , VO_U32 uBitrate , VO_U64 ullStartTimeStamp , VO_BOOL isFormatChanged )
{
	voBitrateAdaptation * ptr_ba = (voBitrateAdaptation*)hHandle;

	if( !ptr_ba )
		return;

	ptr_ba->chunk_begin( uBitrate , ullStartTimeStamp , isFormatChanged );
}

VO_VOID BA_StreamChunkEnd( VO_HANDLE hHandle , VO_U32 uDownloadTime , VO_U32 uDownloadSize , VO_U64 ullEndTimeStamp )
{
	voBitrateAdaptation * ptr_ba = (voBitrateAdaptation*)hHandle;

	if( !ptr_ba )
		return;

	ptr_ba->chunk_end( uDownloadTime , uDownloadSize , ullEndTimeStamp );
}

VO_VOID BA_AddVideoDelay( VO_HANDLE hHandle , VO_S32 uDelayTime , VO_U64 ullPlayTime )
{
	voBitrateAdaptation * ptr_ba = (voBitrateAdaptation*)hHandle;

	if( !ptr_ba )
		return;

	ptr_ba->add_delay( uDelayTime , ullPlayTime );
}

VO_U32 BA_GetNextBitrate( VO_HANDLE hHandle , VO_U32 * pBitrate )
{
	voBitrateAdaptation * ptr_ba = (voBitrateAdaptation*)hHandle;

	if( !ptr_ba )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	return ptr_ba->get_nextbitrate( pBitrate );
}
VO_U32 BA_CheckDownloadSlow( VO_HANDLE hHandle , VO_U32 nBitrate )
{
	voBitrateAdaptation * ptr_ba = (voBitrateAdaptation*)hHandle;

	if( !ptr_ba )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	return ptr_ba->check_downloadslow( nBitrate );
}
VO_U32 BA_GetParam( VO_HANDLE hHandle , VO_U32 uParamID , VO_PTR pParam )
{
	voBitrateAdaptation * ptr_ba = (voBitrateAdaptation*)hHandle;

	if( !ptr_ba )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	return ptr_ba->get_param( uParamID , pParam );
}

VO_U32 BA_SetParam( VO_HANDLE hHandle , VO_U32 uParamID , VO_PTR pParam )
{
	voBitrateAdaptation * ptr_ba = (voBitrateAdaptation*)hHandle;

	if( !ptr_ba )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	return ptr_ba->set_param( uParamID , pParam );
}

VO_VOID voGetBitrateAdaptationAPI( VO_BITRATE_ADAPTATION_API * pApi )
{
	pApi->Init = BA_Init;
	pApi->Uninit = BA_Uninit;
	pApi->Open = BA_Open;
	pApi->Close = BA_Close;
	pApi->StreamChunkBegin = BA_StreamChunkBegin;
	pApi->StreamChunkEnd = BA_StreamChunkEnd;
	pApi->AddVideoDelay = BA_AddVideoDelay;
	pApi->GetNextBitrate = BA_GetNextBitrate;
	pApi->CheckDownloadSlow = BA_CheckDownloadSlow;
	pApi->GetParam = BA_GetParam;
	pApi->SetParam = BA_SetParam;
}
