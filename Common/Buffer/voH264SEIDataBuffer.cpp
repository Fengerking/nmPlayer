#include "voH264SEIDataBuffer.h"
#include "memory.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voH264SEIDataBuffer::voH264SEIDataBuffer(int nCount)
:m_nSETCount(nCount)
{
	m_ptr_cache = NULL;
}

voH264SEIDataBuffer::~voH264SEIDataBuffer()
{
	if(m_ptr_cache)
	{
		DestrpySEIInfo( m_ptr_cache);
		m_ptr_cache = NULL;
	}

	Flush();
}

VO_S32 voH264SEIDataBuffer::AddBuffer (VO_PARSER_SEI_INFO * pBuffer)
{
	voCAutoLock lock( &m_lock );

	if(pBuffer)
	{
		VO_PARSER_SEI_INFO * ptr_info = CreateSEIInfo( pBuffer);

		if(GetBuffCount() > m_nSETCount)
		{
			DestrpySEIInfo( *m_list.begin());
			m_list.pop_front();
		}

		m_list.insert_back_sort( ptr_info, TimeStampComp() );
	}

	return VO_RET_SOURCE2_OK;
}

VO_S32 voH264SEIDataBuffer::GetBuffer (VO_PARSER_SEI_INFO * pBuffer)
{
	voCAutoLock lock( &m_lock );

	if( m_list.empty() )
		return VO_RET_SOURCE2_NEEDRETRY;

	if(m_ptr_cache)
	{
		DestrpySEIInfo( m_ptr_cache);
		m_ptr_cache = NULL;
	}

	if( pBuffer->time < (*m_list.begin())->time )
		return VO_RET_SOURCE2_NEEDRETRY;

	vo_doublelink_list< VO_PARSER_SEI_INFO * >::iterator iter_cur = m_list.end();

	iter_cur = FindNearestItem( pBuffer->time );

	m_ptr_cache = CreateSEIInfo( *iter_cur);

	memcpy( pBuffer , m_ptr_cache , sizeof( VO_PARSER_SEI_INFO ) );

	return VO_RET_SOURCE2_OK;
}

VO_U64 voH264SEIDataBuffer::GetBuffTime ()
{
	return 0;
}

VO_S32 voH264SEIDataBuffer::GetBuffCount ()
{
	voCAutoLock lock( &m_lock );
	return m_list.count();
}

VO_S32 voH264SEIDataBuffer::Flush ()
{
	voCAutoLock lock( &m_lock );

	vo_doublelink_list<VO_PARSER_SEI_INFO*>::iterator iter = m_list.begin();
	vo_doublelink_list<VO_PARSER_SEI_INFO*>::iterator itere = m_list.end();

	while( iter != itere )
	{
		DestrpySEIInfo( *iter);
		iter++;
	}

	m_list.reset();
	return VO_RET_SOURCE2_OK;
}

vo_doublelink_list< VO_PARSER_SEI_INFO* >::iterator voH264SEIDataBuffer::FindNearestItem( VO_U64 time )
{
	vo_doublelink_list< VO_PARSER_SEI_INFO * >::iterator iter = m_list.begin();
	vo_doublelink_list< VO_PARSER_SEI_INFO * >::iterator itere = m_list.end();
	vo_doublelink_list< VO_PARSER_SEI_INFO * >::iterator iter_pos = m_list.end();

	while( iter != itere )
	{
		if( (*iter)->time > (VO_S64)time )
			break;

		iter_pos = iter;
		iter++;
	}

	return iter_pos;
}

VO_S32 voH264SEIDataBuffer::RemoveTo( VO_U64 timestamp )
{
	voCAutoLock lock( &m_lock );

	vo_doublelink_list<VO_PARSER_SEI_INFO*>::iterator iter = m_list.begin();
	vo_doublelink_list<VO_PARSER_SEI_INFO*>::iterator itere = m_list.end();
/*
	while( iter != itere )
	{
		if( (*iter)->time >= (VO_S64)timestamp )
			break;

		DestrpySEIInfo( *iter);
		m_list.pop_front();

		iter++;
	}
*/
	m_list.reset();
	return VO_RET_SOURCE2_OK;
}


VO_PARSER_SEI_INFO* voH264SEIDataBuffer::CreateSEIInfo( const VO_PARSER_SEI_INFO* ptr_info)
{
	if(ptr_info == NULL)
		return NULL;

	VO_PARSER_SEI_INFO* pSEIInfo = new VO_PARSER_SEI_INFO();

	memcpy(pSEIInfo, ptr_info, sizeof(VO_PARSER_SEI_INFO));

	if(ptr_info->buffersize > 0)
	{
		pSEIInfo->buffer = new VO_U8[ptr_info->buffersize];
		memcpy(pSEIInfo->buffer, ptr_info->buffer, ptr_info->buffersize);
	}

	return pSEIInfo;
}

int voH264SEIDataBuffer::DestrpySEIInfo( const VO_PARSER_SEI_INFO* ptr_info)
{
	if(ptr_info == NULL)
		return 0;

	if(ptr_info->buffer)
	{
		delete []ptr_info->buffer;
	}

	delete ptr_info;

	return 0;
}