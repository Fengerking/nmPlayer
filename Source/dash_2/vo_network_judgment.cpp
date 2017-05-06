#include "vo_network_judgment.h"

#include "voLog.h"
#include "math.h"
#ifdef _IOS_TEST
using namespace _DASH;
#endif
vo_network_judgment::vo_network_judgment( VO_S32 judge_size )
:m_average_speed(0)
,m_listsize(0)
,m_usedsize(0)
,m_pos(0)
,m_ptr_judgelist(0)
,m_total_number(0)
{
	m_listsize = judge_size;
	m_ptr_judgelist = new VO_S64[ m_listsize ];

	m_machine_top_speed = 0x7fffffffffffffffll;

	VOLOGI("+get_machine_top_speed");
	//get_machine_top_speed();
	VOLOGI("-get_machine_top_speed");
}

vo_network_judgment::~vo_network_judgment()
{
	if( m_ptr_judgelist )
		delete []m_ptr_judgelist;

	m_ptr_judgelist = 0;
}

VO_VOID vo_network_judgment::add_item( VO_S64 speed )
{
	if( speed == -1 )
		return;

	VOLOGI("new speed = %lld",speed);
	m_ptr_judgelist[m_pos] = speed;

	m_pos++;

	if( m_pos >= m_listsize )
		m_pos = 0;

	if( m_usedsize < m_listsize )
		m_usedsize++;

	m_total_average_speed = ( ((float)m_total_average_speed) * m_total_number + speed ) / ( m_total_number + 1 );

	m_total_number++;
}

VO_S64 vo_network_judgment::get_judgment( VO_S64 current_bandwidth , VO_BOOL is_videodelay )
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

	m_average_speed = average_speed / m_usedsize;

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
		use_speed = min;
	}
	else
	{
		if( delta < 100000 ) // not change so frequently
		{
			use_speed = m_ptr_judgelist[pos];
		}
		else
		{
			//use_speed = m_average_speed < m_total_average_speed ? m_average_speed : m_total_average_speed;
			//use_speed = use_speed < m_ptr_judgelist[ m_pos - 1 ] ? use_speed : m_ptr_judgelist[ m_pos - 1 ];
			use_speed = m_average_speed < m_ptr_judgelist[ pos ] ? m_average_speed : m_ptr_judgelist[ pos ];
		}
	}

	//if( use_speed > current_bandwidth && is_videodelay )
		//use_speed = current_bandwidth - 1;

	if(  m_ptr_judgelist[ pos ] >= current_bandwidth && is_videodelay )
		m_machine_top_speed = current_bandwidth - 1;

 	if( use_speed > m_machine_top_speed )
 	{
 		use_speed = m_machine_top_speed;
 	}

	return use_speed;
}

VO_VOID vo_network_judgment::get_machine_top_speed()
{
#ifndef	_WIN32
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
#endif // _WIN32
}