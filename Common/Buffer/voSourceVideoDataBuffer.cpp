#include "voSourceVideoDataBuffer.h"
#include "voLog.h"

#ifndef LOG_TAG
#define LOG_TAG "voSourceVideoDataBuffer"
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voSourceVideoDataBuffer::voSourceVideoDataBuffer( vo_allocator< VO_BYTE >& allocator , VO_S32 nMaxBufferTime )
:voSourceDataBuffer( allocator , nMaxBufferTime )
,m_frameinterval(30)
,m_uflag(0)
{
}

voSourceVideoDataBuffer::~voSourceVideoDataBuffer(void)
{
}

VO_S32 voSourceVideoDataBuffer::GetBuffer (VO_SOURCE2_SAMPLE * pBuffer)
{
	voCAutoLock lock( &m_lock );

	if( m_list.empty() )
		return VO_RET_SOURCE2_NEEDRETRY;

	if( m_ptr_cache )
	{
		destroy_source2_sample( m_ptr_cache , m_allocator );
		m_ptr_cache = 0;
	}

	vo_singlelink_list< VO_SOURCE2_SAMPLE * >::iterator iter_drop = m_list.end();
	
	if( pBuffer->uTime > 0 && (*m_list.begin())->uTime + 500 < pBuffer->uTime )
		iter_drop = GetNextKeyFrame( pBuffer->uTime );

	VO_BOOL bDropFrame = VO_FALSE;

	if( iter_drop != m_list.end() )
	{
		vo_singlelink_list< VO_SOURCE2_SAMPLE * >::iterator iter = m_list.begin();

		while( iter != iter_drop )
		{
			(*iter)->uFlag |= VO_SOURCE2_FLAG_SAMPLE_FRAMESHOULDDROP;
			iter++;
		}

		bDropFrame = VO_TRUE;
	}

	m_ptr_cache = *m_list.begin();

	if( bDropFrame )
	{
		VOLOGR( "Drop to %lld Flag %d Size %d" , m_ptr_cache->uTime , m_ptr_cache->uFlag , m_ptr_cache->uSize );
	}

	memcpy( pBuffer , m_ptr_cache , sizeof( VO_SOURCE2_SAMPLE ) );

	m_list.pop_front();
	if(bDropFrame)
		pBuffer->pReserve1 = &(m_uflag = VO_SOURCE2_FLAG_SAMPLE_FRAMEDROPPED);

	return VO_RET_SOURCE2_OK;
}

VO_U64 voSourceVideoDataBuffer::GetBuffTime (void)
{
	voCAutoLock lock( &m_lock );

	//VOLOGR( "%d %s" , m_list.count() , m_list.empty()?"true":"false" );


	return m_list.count() * m_frameinterval;
}

vo_singlelink_list< VO_SOURCE2_SAMPLE * >::iterator voSourceVideoDataBuffer::GetNextKeyFrame( VO_U64 timestamp )
{
	vo_singlelink_list< VO_SOURCE2_SAMPLE * >::iterator iter = m_list.begin();
	vo_singlelink_list< VO_SOURCE2_SAMPLE * >::iterator itere = m_list.end();
	vo_singlelink_list< VO_SOURCE2_SAMPLE * >::iterator iter_pos = m_list.end();

	while( iter != itere )
	{
		if( (*iter)->uTime > timestamp )
			break;

		if( (*iter)->uFlag & VO_SOURCE2_FLAG_SAMPLE_KEYFRAME )
			iter_pos = iter;

		if( (*iter)->uFlag & 0xfffffffe )
		{
			iter_pos = iter;
			break;
		}

		iter++;
	}

	return iter_pos;
}

