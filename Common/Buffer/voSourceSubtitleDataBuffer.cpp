#include "voSourceSubtitleDataBuffer.h"
#include "memory.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voSourceSubtitleDataBuffer::voSourceSubtitleDataBuffer( vo_allocator< VO_BYTE >& allocator )
:m_alloc(allocator)
,m_ptr_cache(0)

{
}

voSourceSubtitleDataBuffer::~voSourceSubtitleDataBuffer()
{
	Flush();
	if( m_ptr_cache )
	{
		destroy_subtitleinfo( m_ptr_cache , m_alloc );
		m_ptr_cache = 0;
	}
}

VO_S32 voSourceSubtitleDataBuffer::AddBuffer (voSubtitleInfo * pBuffer)
{
	voCAutoLock lock( &m_lock );

	if(pBuffer)
	{
		voSubtitleInfo * ptr_info = create_subtitleinfo( pBuffer , m_alloc );

		m_list.push_back( ptr_info );
	}

	return VO_RET_SOURCE2_OK;
}

VO_S32 voSourceSubtitleDataBuffer::GetBuffer (voSubtitleInfo * pBuffer)
{
	voCAutoLock lock( &m_lock );

	if( m_list.empty() )
		return VO_RET_SOURCE2_NEEDRETRY;

	if( m_ptr_cache )
	{
		destroy_subtitleinfo( m_ptr_cache , m_alloc );
		m_ptr_cache = 0;
	}

	if( pBuffer->nTimeStamp < (*m_list.begin())->nTimeStamp )
		return VO_RET_SOURCE2_NEEDRETRY;

	vo_singlelink_list< voSubtitleInfo * >::iterator iter_drop = m_list.end();

	iter_drop = FindNearestItem( pBuffer->nTimeStamp );

	if( iter_drop != m_list.end() )
	{
		vo_singlelink_list< voSubtitleInfo * >::iterator iter = m_list.begin();

		while( iter != iter_drop )
		{
			destroy_subtitleinfo( *iter , m_alloc );

			iter++;
		}

		m_list.removeto( iter_drop );
	}

	m_ptr_cache = *m_list.begin();

	memcpy( pBuffer , m_ptr_cache , sizeof( voSubtitleInfo ) );

	m_list.pop_front();

	return VO_RET_SOURCE2_OK;
}

VO_U64 voSourceSubtitleDataBuffer::GetBuffTime ()
{
	return 0;
}

VO_S32 voSourceSubtitleDataBuffer::GetBuffCount ()
{
	voCAutoLock lock( &m_lock );
	return m_list.count();
}

VO_S32 voSourceSubtitleDataBuffer::Flush ()
{
	voCAutoLock lock( &m_lock );

	vo_singlelink_list<voSubtitleInfo*>::iterator iter = m_list.begin();
	vo_singlelink_list<voSubtitleInfo*>::iterator itere = m_list.end();

	while( iter != itere )
	{
		destroy_subtitleinfo( *iter , m_alloc );
		iter++;
	}

	m_list.reset();
	return VO_RET_SOURCE2_OK;
}

vo_singlelink_list< voSubtitleInfo* >::iterator voSourceSubtitleDataBuffer::FindNearestItem( VO_U64 time )
{
	vo_singlelink_list< voSubtitleInfo * >::iterator iter = m_list.begin();
	vo_singlelink_list< voSubtitleInfo * >::iterator itere = m_list.end();
	vo_singlelink_list< voSubtitleInfo * >::iterator iter_pos = m_list.end();

	while( iter != itere )
	{
		if( (*iter)->nTimeStamp > time )
			break;

		iter_pos = iter;
		iter++;
	}

	return iter_pos;
}

VO_S32 voSourceSubtitleDataBuffer::RemoveTo( VO_U64 timestamp )
{
	voCAutoLock lock( &m_lock );

	vo_singlelink_list<voSubtitleInfo*>::iterator iter = m_list.begin();
	vo_singlelink_list<voSubtitleInfo*>::iterator itere = m_list.end();

	while( iter != itere )
	{
		if( (*iter)->nTimeStamp >= timestamp )
			break;

		destroy_subtitleinfo( *iter , m_alloc );
		iter++;
	}

	m_list.removeto( iter );

	return VO_RET_SOURCE2_OK;
}


