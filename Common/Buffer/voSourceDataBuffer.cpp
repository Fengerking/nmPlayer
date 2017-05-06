#include "voSourceDataBuffer.h"
#include "voLog.h"

#ifndef LOG_TAG
#define LOG_TAG "voSourceDataBuffer"
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define SAMPLEMAXDURATION 2000

voSourceDataBuffer::voSourceDataBuffer( vo_allocator< VO_BYTE >& allocator , VO_S32 nMaxBufferTime )
:m_ptr_cache(0)
,m_nMaxBufferTime( nMaxBufferTime )
,m_allocator(allocator)
{
}

voSourceDataBuffer::~voSourceDataBuffer(void)
{
	Flush ();
	if( m_ptr_cache )
	{
		destroy_source2_sample( m_ptr_cache , m_allocator );
		m_ptr_cache = 0;
	}
}

VO_S32 voSourceDataBuffer::AddBuffer (VO_SOURCE2_SAMPLE * pBuffer)
{
	voCAutoLock lock( &m_lock );
	
	if( GetBuffTime() > m_nMaxBufferTime )
		return VO_RET_SOURCE2_NEEDRETRY;

	VO_SOURCE2_SAMPLE * ptr_obj = create_source2_sample( pBuffer , m_allocator );

	VOLOGR( "%p" , ptr_obj );
	VOLOGR( "+Push Buffer : %lld" , ptr_obj->uTime );
	m_list.push_back( ptr_obj );
	VOLOGR( "-Push Buffer" );

	return VO_RET_SOURCE2_OK;
}

VO_S32 voSourceDataBuffer::GetBuffer (VO_SOURCE2_SAMPLE * pBuffer)
{
	voCAutoLock lock( &m_lock );

	if( m_list.empty() )
		return VO_RET_SOURCE2_NEEDRETRY;

	if( m_ptr_cache )
	{
		destroy_source2_sample( m_ptr_cache , m_allocator );
		m_ptr_cache = 0;
	}

	m_ptr_cache = *m_list.begin();

	memcpy( pBuffer , m_ptr_cache , sizeof( VO_SOURCE2_SAMPLE ) );

	m_list.pop_front();

	return VO_RET_SOURCE2_OK;
}

VO_U64 voSourceDataBuffer::GetBuffTime (void)
{
	voCAutoLock lock( &m_lock );

	VOLOGR( "%d %s" , m_list.count() , m_list.empty()?"true":"false" );

	if( m_list.empty() )
		return 0;
	
	vo_singlelink_list< VO_SOURCE2_SAMPLE * >::iterator iter = m_list.begin();
	vo_singlelink_list< VO_SOURCE2_SAMPLE * >::iterator itere = m_list.end();

	VO_U64 buffer_duration = 0;

	while( iter != itere )
	{
		VO_U64 group_duration = 0;
		VO_U64 group_first_timestamp = (*iter)->uTime;
		VO_U64 group_last_timestamp = (*iter)->uTime;

		while( iter != itere )
		{
			//if (iter->uFlag & 0xFFFFFFFE)
			//{
			//	iter++;
			//	continue;
			//}
			
			if( ( (*iter)->uTime > group_last_timestamp ? (*iter)->uTime - group_last_timestamp : group_last_timestamp - (*iter)->uTime )> SAMPLEMAXDURATION )
			{
				group_duration = group_first_timestamp < group_last_timestamp ? group_last_timestamp - group_first_timestamp : group_first_timestamp - group_last_timestamp;
				buffer_duration += group_duration;
				break;
			}
			else
			{
				group_last_timestamp = (*iter)->uTime;
				iter++;
			}
		}

		if( iter == itere )
		{
			buffer_duration = buffer_duration + group_first_timestamp < group_last_timestamp ? group_last_timestamp - group_first_timestamp : group_first_timestamp - group_last_timestamp;
		}
	}

	return buffer_duration;
}

VO_S32 voSourceDataBuffer::GetBuffCount (void)
{
	voCAutoLock lock( &m_lock );

	return m_list.count();
}

VO_S32 voSourceDataBuffer::Flush (void)
{
	voCAutoLock lock( &m_lock );

	vo_singlelink_list<VO_SOURCE2_SAMPLE*>::iterator iter = m_list.begin();
	vo_singlelink_list<VO_SOURCE2_SAMPLE*>::iterator itere = m_list.end();

	while( iter != itere )
	{
		destroy_source2_sample( *iter , m_allocator );
		iter++;
	}

	m_list.reset();

	return VO_RET_SOURCE2_OK;
}

VO_S32 voSourceDataBuffer::GetFirstBufferTimeStamp( VO_U64 * ptr_timestamp )
{
	if( m_list.empty() )
		return VO_RET_SOURCE2_NEEDRETRY;

	vo_singlelink_list<VO_SOURCE2_SAMPLE*>::iterator iter = m_list.begin();
	*ptr_timestamp = (*iter)->uTime;

	return VO_RET_SOURCE2_OK;
}

VO_S32 voSourceDataBuffer::RemoveTo( VO_U64 timestamp )
{
	voCAutoLock lock( &m_lock );

	vo_singlelink_list<VO_SOURCE2_SAMPLE*>::iterator iter = m_list.begin();
	vo_singlelink_list<VO_SOURCE2_SAMPLE*>::iterator itere = m_list.end();

	VO_SOURCE2_SAMPLE * ptr_trackinfo = 0;

	while( iter != itere )
	{
		if( (*iter)->uTime >= timestamp )
			break;

		if( (*iter)->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT )
		{
			if( ptr_trackinfo )
				destroy_source2_sample( ptr_trackinfo , m_allocator );

			ptr_trackinfo = *iter;
		}
		else
		{
			destroy_source2_sample( *iter , m_allocator );
		}

		iter++;
	}

	m_list.removeto( iter );

	if( ptr_trackinfo )
	{
		if( !m_list.empty() )
		{
			vo_singlelink_list<VO_SOURCE2_SAMPLE*>::iterator iter = m_list.begin();
			if( ( (*iter)->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT ) == 0 )
			{
				ptr_trackinfo->uTime = (*iter)->uTime - 1 > 0 ? (*iter)->uTime - 1 : 0;
				m_list.push_front( ptr_trackinfo );
			}
		}
		else
		{
			ptr_trackinfo->uTime = timestamp - 1;
			m_list.push_front( ptr_trackinfo );
		}
	}

	return VO_RET_SOURCE2_OK;
}
