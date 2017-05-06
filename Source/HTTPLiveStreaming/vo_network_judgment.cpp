#include "vo_network_judgment.h"

#include "voLog.h"
#include "math.h"
#include "CBaseConfig.h"
#include "voHalInfo.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


vo_network_judgment::vo_network_judgment( VO_S32 judge_size )
:m_ptr_judgelist(NULL)
,m_listsize(0)
,m_usedsize(0)
,m_pos(0)
,m_ptr_playlist(0)
,m_playlist_count(0)
,m_total_number(0)
,m_average_speed(0)
,m_total_average_speed(0)
,m_machine_top_speed(0x7fffffffffffffffll)
{
	m_listsize = judge_size;
	m_ptr_judgelist = new VO_S64[ m_listsize ];
	m_array_start = 0;
	 
	m_current_index = 0;
#if defined _WIN32
    m_benable_cpuba = 0;
    m_benable_cap = 0;
#else
    m_benable_cpuba = 1;
    m_benable_cap = 0;
#endif
    m_cap_singlecore = 600000;
    m_cap_dualcore = 2000000;
	m_judgment_counter = 0;
	m_ulProfile = 0;
	VOLOGI("+get_machine_top_speed");
	//get_machine_top_speed();
	VOLOGI("-get_machine_top_speed");
    m_bagressive = get_isDualCore();

    if( m_bagressive )
    {
        VOLOGI( "Aggressive mode!!!" );
    }
    else
    {
        VOLOGI( "None Aggressive mode!!!" );
    }

    m_ulCapFromOutSide = 0xffffffff;

    m_b_isPerformanceissue = VO_FALSE;
    memset( m_timebitrate_array , 0 , TIMEBITRATE_ARRAYSIZE * sizeof( TIME_BITRATE_ITEM ) );
	memset( &m_sStartCapInfo , 0 , sizeof(VO_SOURCE2_CAP_DATA) );
}

vo_network_judgment::~vo_network_judgment()
{
	if( m_ptr_judgelist )
		delete []m_ptr_judgelist;

    if( m_ptr_playlist )
        delete []m_ptr_playlist;

	m_ptr_judgelist = 0;
}

VO_VOID vo_network_judgment::load_config( char * path )
{
    if( strlen( path ) == 0 )
        return;

    char temp[1024];
    memset( temp , 0 , 1024 );
    strcpy( temp , path );
    strcat( temp , "/voHLScfg.cfg" );


#ifdef _WIN32
	m_benable_cpuba = 0;
	m_benable_cap = 0;
#else
    CBaseConfig cfg;
    if( cfg.Open( (TCHAR *)temp ) )
    {
        m_benable_cpuba = cfg.GetItemValue( (char *)"CPUAdaptation" , (char *)"Enable" , 1 );
        m_benable_cap = cfg.GetItemValue( (char *)"AdaptationCap" , (char *)"Enable" , 1 );
		m_cap_singlecore = cfg.GetItemValue( (char *)"Cap" , (char *)"SingleCore" , 600000 );
		m_cap_dualcore = cfg.GetItemValue((char *) "Cap" , (char *)"DualCore" , 2000000 );
    }
    
#endif
}

VO_VOID vo_network_judgment::add_item( VO_S64 speed )
{
	if( speed == -1 )
		return;

    if( m_playlist_count == 0 )
        return;

	m_ptr_judgelist[m_pos] = speed;

	m_last_downloadspeed = speed;

	m_pos++;

	if( m_pos >= m_listsize )
		m_pos = 0;

	if( m_usedsize < m_listsize )
		m_usedsize++;

	m_total_average_speed = (VO_S64) (( ((float)m_total_average_speed) * m_total_number + speed ) / ( m_total_number + 1 ));

	m_total_number++;
}

VO_S64 vo_network_judgment::get_judgment( VO_S64 current_bandwidth , VO_S64 * ptr_next_bandwidth , VO_S64 * ptr_limit )
{
	VO_S64 max = 0;
	VO_S64 min = 0xffffffff;

	float average_speed = 0;

	for( VO_S32 i = 0 ; i < m_usedsize ; i++ )
	{
		if( m_ptr_judgelist[i] > max )
			max = m_ptr_judgelist[i];

		if( m_ptr_judgelist[i] < min )
			min = m_ptr_judgelist[i];

		average_speed = average_speed + m_ptr_judgelist[i];
	}

	m_average_speed = (VO_S64)(average_speed / m_usedsize);

	float delta = 0;

	for( VO_S32 i = 0 ; i < m_usedsize ; i++ )
	{
		delta += abs( (float)m_ptr_judgelist[i] - (float)m_average_speed );
	}

	delta = delta / m_usedsize;

	float max_delta = 0;

	max_delta = abs( (float)max - (float)m_average_speed ) + abs( (float)min - (float)m_average_speed ) ;
	max_delta = max_delta / 2;

	VO_U32 use_speed;

	VO_S32 pos = m_pos;

	if( pos == 0 )
		pos = m_usedsize - 1;
	else
		pos = pos - 1;

	if( m_usedsize < 5 )
	{
		use_speed = (VO_U32)min;
	}
	else
	{
		if( delta < 100000 ) // not change so frequently
		{
			use_speed = (VO_U32)(m_ptr_judgelist[pos]);
		}
		else
		{
			//use_speed = m_average_speed < m_total_average_speed ? m_average_speed : m_total_average_speed;
			//use_speed = use_speed < m_ptr_judgelist[ m_pos - 1 ] ? use_speed : m_ptr_judgelist[ m_pos - 1 ];
			use_speed = (VO_U32)(m_average_speed < m_ptr_judgelist[ pos ] ? m_average_speed : m_ptr_judgelist[ pos ]);
		}
	}

	//if( use_speed > current_bandwidth && is_videodelay )
		//use_speed = current_bandwidth - 1;

    calculate_top_speed();

    VOLOGR( "+++++++++++++Guess speed: %u" , use_speed );

    *ptr_next_bandwidth = use_speed;
    *ptr_limit = m_machine_top_speed;

    if( m_suggest_bandwidth > 0 )
    {
        if( m_machine_top_speed > m_suggest_bandwidth )
            *ptr_limit = m_suggest_bandwidth;
    }

    VOLOGR( "+++++++++++++Limit speed: %lld" , *ptr_limit );

	return 0;
}

VO_VOID vo_network_judgment::get_machine_top_speed()
{
#if !defined _WIN32 && !defined _IOS
	FILE * fp = popen( "cat /proc/cpuinfo" , "r" );

	if( fp )
	{

		VO_CHAR ptr_cpu[1024];
		memset( ptr_cpu , 0 , 1024 );

		fread( ptr_cpu , 1024 , 1 , fp );

		VOLOGI( "%s" , ptr_cpu );

		VO_CHAR * ptr = strstr( ptr_cpu , "BogoMIPS" );

		if( ptr )
		{
			ptr = strchr( ptr , ':' );

			if( ptr )
			{
				while( *ptr < '0' || *ptr > '9' )
					ptr++;

				float freq;

				sscanf( ptr , "%f\r\n" , &freq );

				VOLOGI("*************CPUINFO: %f" , freq);

				if( freq < 550 )
					m_machine_top_speed = 900000;
			}
		}
		pclose( fp );
	}
#endif // _WIN32 && _IOS
}

VO_VOID vo_network_judgment::add_starttime( VO_S64 start_time )
{
    if( m_array_start == -1 )
        return;

    if( m_playlist_count == 0 )
        return;

    m_timebitrate_array[m_array_start].start_time = start_time;
    m_timebitrate_array[m_array_start].end_time = -1;
    m_timebitrate_array[m_array_start].delaytimes = 0;
    m_timebitrate_array[m_array_start].smalldelaytimes = 0;
}

VO_VOID vo_network_judgment::add_endtime_bitrate( VO_S64 end_time , VO_S64 bitrate )
{
    if( m_playlist_count == 0 )
        return;

    if( m_array_start == -1 )
    {
        m_array_start++;
        return;
    }

    m_timebitrate_array[m_array_start].end_time = end_time;
    m_timebitrate_array[m_array_start].bitrate = bitrate;

    m_array_start++;

    if( m_array_start >= TIMEBITRATE_ARRAYSIZE )
        m_array_start = 0;
}

VO_VOID vo_network_judgment::add_delaytime( VO_S64 delay_time , VO_S64 play_time )
{
    if( m_playlist_count == 0 )
        return;

    if( m_b_isPerformanceissue )
    {
        m_afterPerformanceCount++;

        if( m_afterPerformanceCount >= 2 )
            m_b_isPerformanceissue = VO_FALSE;

        VOLOGI( "Drop this delay!" );
        return;
    }
    m_last_timestamp = play_time;

    for( VO_S32 i = 0 ; i < TIMEBITRATE_ARRAYSIZE ; i++ )
    {
        if( m_timebitrate_array[i].end_time != 0 )
        {
            if( m_timebitrate_array[i].start_time <= play_time && ( m_timebitrate_array[i].end_time == -1 || m_timebitrate_array[i].end_time >= play_time ) )
            {
                if( delay_time > 120 )
                    m_timebitrate_array[i].delaytimes++;

                if( delay_time >= 50 )
                    m_timebitrate_array[i].smalldelaytimes++;

                //m_fuzzy.add( m_timebitrate_array[i].bitrate , delay_time );

                return;
            }
        }
    }
}

VO_VOID vo_network_judgment::calculate_top_speed()
{
    if( m_playlist_count == 0 )
        return;

    VO_S64 cal_top_speed = 0x7fffffffffffffffll;
    m_suggest_bandwidth = -1;
    VO_S64 perhapsright_bandwidth = 0x7fffffffffffffffll;

    BITRATE_DELAYTIME_ITEM array[TIMEBITRATE_ARRAYSIZE];
    memset( array , 0 , TIMEBITRATE_ARRAYSIZE * sizeof( BITRATE_DELAYTIME_ITEM ) );
    VO_S32 usedsize = 0;

    for( VO_S32 i = 0 ; i < TIMEBITRATE_ARRAYSIZE ; i++ )
    {
        if( m_timebitrate_array[i].end_time != 0 )
        {
            VOLOGR( "+++++++++++++%d Start time: %lld , End time: %lld , Bitrate: %lld , Delay Times: %d , Small Delay Times: %d" ,
                i , m_timebitrate_array[i].start_time , m_timebitrate_array[i].end_time , m_timebitrate_array[i].bitrate , m_timebitrate_array[i].delaytimes , m_timebitrate_array[i].smalldelaytimes );
            VO_BOOL isfind = VO_FALSE;

            for( VO_S32 j = 0 ; j < usedsize ; j++ )
            {
                if( array[j].bitrate == m_timebitrate_array[i].bitrate )
                {
                    array[j].delaytime = array[j].delaytime + m_timebitrate_array[i].delaytimes;
                    array[j].smalldelaytime = array[j].smalldelaytime + m_timebitrate_array[i].smalldelaytimes;
                    isfind = VO_TRUE;
                    break;
                }
            }

            if( !isfind )
            {
                array[usedsize].bitrate = m_timebitrate_array[i].bitrate;
                array[usedsize].delaytime = m_timebitrate_array[i].delaytimes;
                array[usedsize].smalldelaytime = m_timebitrate_array[i].smalldelaytimes;
                usedsize++;
            }
        }
    }

    for( VO_S32 i = 0 ; i < usedsize ; i++ )
    {
        VOLOGR( "+++++++++++++%d Bitrate: %lld , DelayTimes: %d" , i , array[i].bitrate , array[i].delaytime );
        if( array[i].delaytime >= 4 /*|| array[i].smalldelaytime >= 7*/ )
        {
            if( cal_top_speed >= array[i].bitrate )
            {
                cal_top_speed = array[i].bitrate - 1;
            }
        }

        if( array[i].smalldelaytime > 2 )
        {
            if( perhapsright_bandwidth > array[i].bitrate )
                perhapsright_bandwidth = array[i].bitrate;
        }
    }

    VOLOGR( "+++++++++++++Cal Top Speed: %lld" , cal_top_speed );

    if( cal_top_speed != 0x7fffffffffffffffll )
    {
        VO_S32 i = 0;

        for( ; i < m_playlist_count ; i++ )
        {
            if( m_ptr_playlist[i] == cal_top_speed + 1 )
                break;
        }

        if( i < 1 )
            m_suggest_bandwidth = m_ptr_playlist[0];
        else
            m_suggest_bandwidth = m_ptr_playlist[i-1] + 1;
    }

    if( m_suggest_bandwidth > perhapsright_bandwidth || m_suggest_bandwidth == -1 )
        m_suggest_bandwidth = perhapsright_bandwidth + 1;

    /*if( m_last_timestamp < 60000 )
    {
        VO_S64 test_bitrate = m_playlist_count > 1 ? m_ptr_playlist[m_playlist_count - 2] : m_ptr_playlist[0];
        if( m_suggest_bandwidth > test_bitrate || m_suggest_bandwidth == -1  )
            m_suggest_bandwidth = test_bitrate + 1;
    }*/

    if( cal_top_speed < m_machine_top_speed )
    {
        m_machine_top_speed = cal_top_speed;
        VOLOGR("m_machine_top_speed = 0x%lld, cal_top_speed = 0x%lld", m_machine_top_speed, cal_top_speed);
        VOLOGW( "We need choose small bitrate!" );
        m_b_isPerformanceissue = VO_TRUE;
        m_afterPerformanceCount = 0;

        //memset( m_timebitrate_array , 0 , TIMEBITRATE_ARRAYSIZE * sizeof( TIME_BITRATE_ITEM ) );
        //m_array_start = 0;
    }

    VOLOGR( "+++++++++++++Top Speed: %lld , Suggest BandWidth: %lld" , m_machine_top_speed , m_suggest_bandwidth );

    //m_fuzzy.get_suitable_bitrate();
    //VOLOGI( "Cal Fuzzy done!" );
}

VO_VOID vo_network_judgment::flush()
{
    if( m_playlist_count == 0 )
        return;

    VO_S64 bitrate = m_ptr_playlist[m_current_index];
    if( m_array_start != -1 )
    {
        if( m_array_start > 0 )
            bitrate = m_timebitrate_array[m_array_start-1].bitrate;
    }

    VOLOGI( "Should use bitrate %lld" , bitrate );

    for( VO_S32 i = 0 ; i < m_playlist_count ; i++ )
    {
        if( m_ptr_playlist[i] == bitrate )
        {
            m_current_index = i;
            break;
        }
    }

    VOLOGI( "After flush the bitrate should be %lld" , m_ptr_playlist[m_current_index] );

    memset( m_timebitrate_array , 0 , TIMEBITRATE_ARRAYSIZE * sizeof( TIME_BITRATE_ITEM ) );
    m_array_start = -1;
}

VO_VOID vo_network_judgment::reset_all()
{
    if(m_ptr_judgelist != NULL)
    {
        memset(m_ptr_judgelist, 0, sizeof(VO_S64)*m_listsize);
    }

	m_array_start = 0;	 
	m_current_index = 0;
	m_usedsize = 0;
    m_pos = 0;
    m_total_number = 0;
    m_average_speed = 0;
    m_total_average_speed = 0;
    m_machine_top_speed = 0x7fffffffffffffffll;

    memset( m_timebitrate_array , 0 , TIMEBITRATE_ARRAYSIZE * sizeof( TIME_BITRATE_ITEM ) );

    if(m_ptr_playlist != NULL)
    {
        delete m_ptr_playlist;
		m_ptr_playlist = NULL;
		m_playlist_count = 0;
    }


#if defined _WIN32
    m_benable_cpuba = 0;
    m_benable_cap = 0;
#else
    m_benable_cpuba = 1;
    m_benable_cap = 0;
#endif
    m_cap_singlecore = 600000;
    m_cap_dualcore = 2000000;
	m_judgment_counter = 0;
	m_ulProfile = 0;
}


VO_VOID  vo_network_judgment::SetTheCPUAdaptionWorkMode(VO_U32  ulCPUAdaptionWorkMode)
{
    if(ulCPUAdaptionWorkMode == 0)
    {
        m_benable_cpuba = 0;
    }
    else
    {
        m_benable_cpuba = 1;
    }
}


VO_VOID vo_network_judgment::set_playlist( VO_S64 * ptr_playlist , VO_S32 size )
{
    if( size == 0 )
    {
        m_playlist_count = 0;
        return;
    }

    m_ptr_playlist = new VO_S64[size];
    memcpy( m_ptr_playlist , ptr_playlist , size * sizeof( VO_S64 ) );
    m_playlist_count = size;

    //m_fuzzy.init( ptr_playlist , size );
}

VO_BOOL vo_network_judgment::get_isDualCore()
{
#ifdef _IOS
	if (voOS_GetCPUNum() < 2)
        return VO_FALSE;
    else
        return VO_TRUE;
#else
	VO_CPU_Info info;
    get_cpu_info( &info );

    if( info.mCount < 2 )
        return VO_FALSE;
    else
        return VO_TRUE;
#endif
}

VO_VOID vo_network_judgment::set_default_streambandwidth( VO_S64 bandwidth )
{
    if( m_playlist_count == 0 )
        return;

    for( VO_S32 i = 0 ; i < m_playlist_count ; i++ )
    {
        if( bandwidth == m_ptr_playlist[i] )
        {
            m_current_index = i;
            break;
        }
    }
}

VO_VOID vo_network_judgment::get_right_bandwidth( VO_S64 current_bandwidth , VO_S64* ptr_bandwidth )
{
    VO_S64 next_download_speed;
    VO_S64 limit_bandwidth;
    VO_U32   ulCapBitrate = 0;


    if( m_playlist_count == 0 )
    {
        *ptr_bandwidth = -1;
        return;
    }



  /*
    if(m_sStartCapInfo.nBitRate == 0)
    {
        m_sStartCapInfo.nBitRate = m_sCapInfo.nBitRate;    
    }

	if( m_judgment_counter == 0 && m_sStartCapInfo.nBitRate != 0 )
	{
		m_judgment_counter++;
        ulCapBitrate = m_sCapInfo.nBitRate;

		VO_U32 start_bitrate = (m_sStartCapInfo.nBitRate<m_sCapInfo.nBitRate?m_sStartCapInfo.nBitRate:m_sCapInfo.nBitRate);
        VOLOGI("the start bitrate:%d", start_bitrate);

		if( start_bitrate == 0 )
		{
			*ptr_bandwidth = -1;
			return;
		}

		VO_S32 i = 0;
		for(  ; i < m_playlist_count ; i++ )
		{
			if( m_ptr_playlist[i] > start_bitrate )
			{
				if( i > 0 )
					i--;
				break;
			}
		}

		if( i == m_playlist_count && i != 0 )
			i--;

		*ptr_bandwidth = m_ptr_playlist[i];
		m_current_index = i;
		return;
	}


    if( m_judgment_counter == 0 && !( m_benable_cpuba == 0 && m_benable_cap == 1 ) )
    {
        m_judgment_counter++;
        *ptr_bandwidth = -1;
        return;
    }
   */

    m_judgment_counter++;

    if( m_array_start == -1 )
    {
       *ptr_bandwidth = m_ptr_playlist[m_current_index];
       return;
    }

    get_judgment( current_bandwidth , &next_download_speed , &limit_bandwidth );



	VOLOGR( "Current Play Bitrate: %lld    Current Download Speed: %lld    Next Download Speed: %d   CPU CAP: %lld" , current_bandwidth , m_last_downloadspeed , next_download_speed , limit_bandwidth );

    if( m_benable_cpuba == 0 )
        limit_bandwidth = 0x7fffffffffffffffll;

    if( m_benable_cap == 1 )
    {
        VO_S32 cap = m_bagressive? m_cap_dualcore : m_cap_singlecore;

        if( limit_bandwidth > cap + 1 )
            limit_bandwidth = cap + 1;
    }


    if( next_download_speed >= m_ptr_playlist[m_current_index] && m_ptr_playlist[m_current_index] <= limit_bandwidth )
    {
        VO_S32 i = m_current_index;
        VO_S32 count = 0;
        VO_S32 max_count = m_bagressive ? 2 : 1;

        while( 1 )
        {
            i++;
            count++;

            //
			VOLOGR( "next_download_speed: %lld , m_ptr_playlist[i]: %lld, m_ptr_playlist[m_current_index]: %lld, m_playlist_count:%d, i: %d,m_current_index:%d, limit_bandwidth:%lld" , next_download_speed,m_ptr_playlist[i] , m_ptr_playlist[m_current_index], m_playlist_count, i, m_current_index, limit_bandwidth);
            //
            if( next_download_speed < ( m_ptr_playlist[i] + m_ptr_playlist[m_current_index] ) * 1.01 || 
                m_ptr_playlist[i] > limit_bandwidth || i >= m_playlist_count )
            {
                i--;
                break;
            }
            else if( count >= max_count )
                break;
        }

        m_current_index = i;
		
        /*if( m_current_index < m_playlist_count - 1 )
        {
            VOLOGE( "Current Bandwidth: %lld" , m_ptr_playlist[m_current_index] );
            VO_S32 i = m_current_index;
            VO_S32 count = 0;
            VO_S32 max_count = m_bagressive ? 2 : 1;

            for( ; i < m_playlist_count && count < max_count ; i++ )
            {
                if( next_download_speed < ( m_ptr_playlist[i] + m_ptr_playlist[m_current_index] ) * 1.01 || 
                    m_ptr_playlist[i] > limit_bandwidth )
                {
                    i--;
                    break;
                }
                count++;
            }

            VOLOGE("%d",i);

            if( next_download_speed < ( m_ptr_playlist[i] + m_ptr_playlist[m_current_index] ) * 1.01 || 
                m_ptr_playlist[i] > limit_bandwidth )
            {
                i--;
            }

            if( i >= m_playlist_count )
                i = m_playlist_count - 1;

            if( i < m_current_index )
                i = m_current_index;

            m_current_index = i;
        }*/
    }
    else
    {
        VO_S32 i = m_current_index;

        while( ( next_download_speed < m_ptr_playlist[i] || limit_bandwidth < m_ptr_playlist[i] ) && i > 0 )
            i--;

        m_current_index = i;


        /*if( m_current_index > 0 )
        {
            VO_S32 i = m_current_index - 1;

            for( ; i > -1 ; i-- )
            {
                if( next_download_speed > m_ptr_playlist[i] && limit_bandwidth >= m_ptr_playlist[i] )
                    break;
            }

            if( i < 0 )
                i = 0;

            m_current_index = i;
        }*/
    }

    *ptr_bandwidth = m_ptr_playlist[m_current_index];


    if(m_ulCapFromOutSide == 0xffffffff)
    {
        VOLOGI("The MaxCap is not set from outside! The current RunCap:%u", (VO_U32)m_machine_top_speed);
    }
    else
    {
        VOLOGI( "BAInfo***-MaxCap %d , RunCap %u", m_ulCapFromOutSide , (VO_U32)m_machine_top_speed);
    }
}


VO_VOID vo_network_judgment::setCpuInfo(VO_VOID*  pCpuInfo)
{
    if(pCpuInfo == NULL)
    {
        return;
    }

    memset(&m_sCpuInfo, 0, sizeof(VO_SOURCE2_CPU_INFO));
    memcpy(&m_sCpuInfo, pCpuInfo, sizeof(VO_SOURCE2_CPU_INFO));
    if(m_sCpuInfo.nCoreCount > 1)
    {
        m_bagressive = VO_TRUE;
    }
}
VO_VOID vo_network_judgment::setCapInfo(VO_VOID*  pCapInfo)
{
    if(pCapInfo == NULL)
    {
        return;
    }

    memset(&m_sCapInfo, 0, sizeof(VO_SOURCE2_CAP_DATA));
    memcpy(&m_sCapInfo, pCapInfo, sizeof(VO_SOURCE2_CAP_DATA));
    m_ulCapFromOutSide = m_sCapInfo.nBitRate;
    m_machine_top_speed = m_ulCapFromOutSide;
    VOLOGI("set the Max bitrate:%d", m_sCapInfo.nBitRate);
}

VO_VOID vo_network_judgment::setStartCapInfo(VO_VOID*  pCapInfo)
{
	if(pCapInfo == NULL)
	{
		return;
	}

	memset( &m_sStartCapInfo , 0 , sizeof(VO_SOURCE2_CAP_DATA) );
	memcpy(&m_sStartCapInfo, pCapInfo, sizeof(VO_SOURCE2_CAP_DATA));
    VOLOGI("the startcap bitrate:%d", m_sStartCapInfo.nBitRate);
}
