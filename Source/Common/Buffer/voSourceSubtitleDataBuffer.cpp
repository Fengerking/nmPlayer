#include "voSourceSubtitleDataBuffer.h"
#include "memory.h"
#include "voLog.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voSourceSubtitleDataBuffer::voSourceSubtitleDataBuffer( vo_allocator< VO_BYTE >& allocator )
: voSourceDataBuffer(allocator, 0x7FFFFFFF)
{
}

voSourceSubtitleDataBuffer::~voSourceSubtitleDataBuffer()
{
}

VO_S32 voSourceSubtitleDataBuffer::AddBuffer (_SAMPLE * pBuffer)
{
	if (NULL == pBuffer) {
		VOLOGE("empty pointor");
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	voCAutoLock lock( &m_lock );

	//if( GetBuffCount() >= 2 )
		//return VO_RET_SOURCE2_NEEDRETRY;

	_SAMPLE * ptr_obj = create_source2_sample( pBuffer , m_allocator );
	m_list.push_back( ptr_obj );

	return VO_RET_SOURCE2_OK;
}

VO_S32 voSourceSubtitleDataBuffer::GetBuffer (_SAMPLE * pBuffer)
{
	if (NULL == pBuffer) {
		VOLOGE("empty pointor");
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	voCAutoLock lock( &m_lock );

	if (m_list.empty())
		return VO_RET_SOURCE2_NEEDRETRY;

	if (m_ptr_cache)
	{
		destroy_source2_sample( m_ptr_cache , m_allocator );
		m_ptr_cache = 0;
	}

	if (pBuffer->uTime < (*m_list.begin())->uTime)
		return VO_RET_SOURCE2_NEEDRETRY;

	if( VO_U32((*m_list.begin())->pReserve2 ) == __INTERNALSUBTITLE )
	{
		vo_singlelink_list< _SAMPLE * >::iterator iter_drop = m_list.end();

		iter_drop = FindNearestItem(pBuffer->uTime);

		if( iter_drop != m_list.end() )
		{
			vo_singlelink_list< _SAMPLE * >::iterator iter = m_list.begin();

			while( iter != iter_drop )
			{
				destroy_source2_sample( *iter , m_allocator );

				iter++;
			}

			m_list.removeto( iter_drop );
		}
	}

	m_ptr_cache = *m_list.begin();

	memcpy( pBuffer , m_ptr_cache , sizeof( _SAMPLE ) );

	m_list.pop_front();

	return VO_RET_SOURCE2_OK;
}

vo_singlelink_list< _SAMPLE* >::iterator voSourceSubtitleDataBuffer::FindNearestItem(VO_U64 uTime)
{
	vo_singlelink_list< _SAMPLE * >::iterator iter = m_list.begin();
	vo_singlelink_list< _SAMPLE * >::iterator itere = m_list.end();
	vo_singlelink_list< _SAMPLE * >::iterator iter_pos = m_list.end();

	while( iter != itere )
	{
		if ( (*iter)->uTime > uTime )
			break;

		iter_pos = iter;
		iter++;
	}

	return iter_pos;
}
