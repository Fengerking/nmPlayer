#include "buffer_controller.h"
#include "log.h"

#define JUDGE_SIZE 6

buffer_controller::buffer_controller( page_allocator * ptr_allocator , VO_S32 page_size , VO_S64 physical_start )
:m_page_size(page_size)
,m_ptr_allocator( ptr_allocator )
,m_endboundary( -1 )
,m_physical_start( physical_start )
,m_ptr_cur_page(0)
,m_cur_page_physical_start(-1)
,m_cur_page_end(0)
,m_pagelist_header(0)
,m_pagelist_tail(0)
,m_pagecount(0)
,m_judger(JUDGE_SIZE)
,m_is_buffer_fulled( VO_FALSE )
,m_is_recyclebuffer( VO_TRUE )
,m_is_small_file( VO_FALSE )
,m_filesize(-1)
{
	m_pagetoken = m_ptr_allocator->init();
}

buffer_controller::~buffer_controller()
{
	m_ptr_allocator->uninit( m_pagetoken );
	free_allpage();
}

VO_S64 buffer_controller::read( VO_S64 pos , VO_PBYTE ptr_buffer , VO_S64 size )
{
	voCAutoLock lock( &m_lock );

	VO_S64 max_readable = m_physical_start + m_page_size * m_pagecount + m_cur_page_end;

	if( pos < m_physical_start || ( m_endboundary != -1 && pos >= m_endboundary ) )
		return 0;

	if( m_endboundary != -1 && pos + size > m_endboundary )
		size = m_endboundary - pos;

	if( m_filesize != -1 && pos + size > m_filesize )
		size = m_filesize - pos;

	while( max_readable < pos + size )
	{
		VO_S64 lack_size = pos > max_readable ? size : pos + size - max_readable;

		m_lock.Unlock();
		BUFFER_CALLBACK_RET ret = m_ptr_callback->buffer_lack( pos > max_readable ? pos : max_readable , lack_size );
		m_lock.Lock();

		max_readable = m_physical_start + m_page_size * m_pagecount + m_cur_page_end;

		switch( ret )
		{
		case BUFFER_CALLBACK_OK:
			break;
		case BUFFER_CALLBACK_SHOULD_STOP:
			return -2;
			break;
		case BUFFER_CALLBACK_ERROR:
			return -1;
			break;
		case BUFFER_CALLBACK_DATA_LACKTOOMUCH:
			return 0;
			break;
		}
	}

	VO_S32 page_index = (pos - m_physical_start) / m_page_size;
	PAGE_LIST * ptr_page = m_pagelist_header;
	VO_S64 cur_page_start = m_physical_start + page_index * m_page_size;

	for( VO_S32 index = 0 ; index < page_index ; index++ )
		ptr_page = ptr_page->ptr_next;

	VO_S64 readed = 0;

	while( ptr_page && readed < size )
	{
		VO_S64 cpysize = 0;
		VO_S64 offset = 0;

		if( pos >= cur_page_start )
		{
			if( pos <= cur_page_start + m_page_size )
			{
				offset = pos - cur_page_start;
				cpysize = ((pos + size) < (cur_page_start + m_page_size)) ? size : m_page_size - offset;
			}
			else
			{
				ptr_page = ptr_page->ptr_next;
				cur_page_start = cur_page_start + m_page_size;
				continue;
			}
		}
		else
		{
			cpysize = ( (pos + size) < (cur_page_start + m_page_size) ) ? pos + size - cur_page_start : m_page_size;
		}

		memcpy( ptr_buffer + readed , ptr_page->ptr_page + offset , cpysize );

		//PRINT_LOG( LOG_LEVEL_DETAIL , "Read Page 0X%p , offset %lld , size %lld" , ptr_page->ptr_page , offset , cpysize );

		readed = readed + cpysize;

		ptr_page = ptr_page->ptr_next;
		cur_page_start = cur_page_start + m_page_size;
	}

	if( !ptr_page && readed < size )
	{
		memcpy( ptr_buffer + readed , m_ptr_cur_page + ( pos + readed - m_cur_page_physical_start ) , size - readed );
		//PRINT_LOG( LOG_LEVEL_DETAIL , "Read Page 0X%p , offset %lld , size %lld" , m_ptr_cur_page , pos + readed - m_cur_page_physical_start , size - readed );
		readed = size;
	}

	m_judger.push_pos( pos );

	recycle_buffer();

	return readed;
}

VO_VOID buffer_controller::recycle_buffer()
{
	VO_S64 del_pos;
	if( m_judger.get_judgement( &del_pos ) )
	{
		free_page_until( del_pos );


		VO_S32 used_page = m_ptr_allocator->get_usedpage_count( m_pagetoken );
		VO_S32 total_page = m_ptr_allocator->get_totalpage_count( m_pagetoken );

		if( m_is_buffer_fulled && used_page < 2 * total_page / 3 )
		{
			m_is_buffer_fulled = VO_FALSE;
			m_lock.Unlock();
			m_ptr_callback->buffer_low( m_physical_start + m_page_size * m_pagecount + m_cur_page_end );
			m_lock.Lock();
		}
	}
}

VO_S64 buffer_controller::write( VO_S64 pos , VO_PBYTE ptr_buffer , VO_S64 size )
{
	voCAutoLock lock( &m_lock );

	if( m_endboundary != -1 && pos + size > m_endboundary )
		size = m_endboundary - pos;

	if( m_cur_page_physical_start == -1 )
	{
		if( pos != m_physical_start )
			return -1;

		m_cur_page_physical_start = m_physical_start;
		m_cur_page_end = 0;
	}


	if( pos + size < m_cur_page_physical_start + m_cur_page_end )
		return -2;

	if( pos > m_cur_page_physical_start + m_cur_page_end )
		return -1;

	VO_S64 leftsize = size;

	ptr_buffer = ptr_buffer + m_cur_page_physical_start + m_cur_page_end - pos;

	while( leftsize > 0 )
	{
		if( !m_ptr_cur_page )
		{
			if( PAGEALLOC_OK != m_ptr_allocator->alloc_page( m_pagetoken , &m_ptr_cur_page ) )
			{
				m_is_buffer_fulled = VO_TRUE;
				return size - leftsize;
			}

			m_cur_page_end = 0;
		}

		VO_S64 canwrite_size = m_page_size - m_cur_page_end;
		VO_S64 write_size = canwrite_size < leftsize ? canwrite_size : leftsize;

		memcpy( m_ptr_cur_page + m_cur_page_end , ptr_buffer , write_size );
		m_cur_page_end = m_cur_page_end + write_size;
		leftsize = leftsize - write_size;
		ptr_buffer = ptr_buffer + write_size;

		if( m_cur_page_end == m_page_size )
		{
			add_page( m_ptr_cur_page );
			m_ptr_cur_page = NULL;
			m_cur_page_end = 0;
			m_cur_page_physical_start = m_cur_page_physical_start + m_page_size;
		}
	}

	return size;
}

void buffer_controller::add_page( VO_PBYTE ptr_page )
{
	PAGE_LIST * ptr_element = new PAGE_LIST;
	ptr_element->ptr_page = ptr_page;
	ptr_element->ptr_next = NULL;

	if( m_pagelist_header == NULL )
		m_pagelist_header = m_pagelist_tail = ptr_element;
	else
	{
		m_pagelist_tail->ptr_next = ptr_element;
		m_pagelist_tail = ptr_element;
	}

	m_pagecount++;
}

void buffer_controller::free_allpage()
{
	voCAutoLock lock( &m_lock );

	PAGE_LIST * ptr_element = m_pagelist_header;
	while( ptr_element )
	{
		PAGE_LIST * ptr_temp = ptr_element;
		m_ptr_allocator->free_page( m_pagetoken , ptr_element->ptr_page );
		ptr_element = ptr_element->ptr_next;

		delete ptr_temp;
	}

	if( m_ptr_cur_page )
		m_ptr_allocator->free_page( m_pagetoken , m_ptr_cur_page );

	m_cur_page_end = 0;
}

void buffer_controller::set_endboundary( VO_S64 end_pos )
{
	voCAutoLock lock( &m_lock );
	m_endboundary = end_pos;
}

VO_BOOL buffer_controller::is_buffer_full()
{
	VO_S32 used_page = m_ptr_allocator->get_usedpage_count( m_pagetoken );
	VO_S32 total_page = m_ptr_allocator->get_totalpage_count( m_pagetoken );

	return used_page >= total_page ? VO_TRUE : VO_FALSE;
}

void buffer_controller::free_page_until( VO_S64 pos )
{
	if( m_is_small_file )
		return;

	if( !m_is_recyclebuffer )
		return;

	PAGE_LIST * ptr_page = m_pagelist_header;

	while( ptr_page && pos > m_physical_start + m_page_size )
	{
		m_ptr_allocator->free_page( m_pagetoken , ptr_page->ptr_page );

		PAGE_LIST * ptr_temp = ptr_page;
		ptr_page = ptr_page->ptr_next;
		delete ptr_temp;

		m_pagelist_header = ptr_page;
		m_physical_start = m_physical_start + m_page_size;
		m_pagecount--;
	}

	PRINT_LOG( LOG_LEVEL_DETAIL , "Recycle Page to %lld , After Recycle Start %lld" , pos , m_physical_start );
	m_ptr_callback->buffer_newstart( m_physical_start );
}

void buffer_controller::set_filesize( VO_S64 filesize )
{
	m_filesize = filesize;
	if( filesize > 0 && filesize + 8 * 1024 <= m_ptr_allocator->get_mempool_size() )
	{
		PRINT_LOG( LOG_LEVEL_IMPORTANT , "Small File! File Size: %lld , Buffer Size: %lld" , filesize , m_ptr_allocator->get_mempool_size() );
		m_is_small_file = VO_TRUE;
	}
}