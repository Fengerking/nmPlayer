#include "vo_mem_stream.h"
#include "voLog.h"

VO_S64 mem_block::read( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	VO_U32 size = m_end - m_start;

	if( size > buffer_size )
	{
		memcpy( ptr_buffer , m_buffer + m_start ,(VO_U32) buffer_size );
		m_start = m_start + (VO_U32)buffer_size;

		return buffer_size;
	}
	else if( size == buffer_size )
	{
		memcpy( ptr_buffer , m_buffer + m_start , (VO_U32)buffer_size );
		m_start = m_end = 0;

		is_end = VO_TRUE;

		return buffer_size;
	}
	else
	{
		memcpy( ptr_buffer , m_buffer + m_start , size );
		m_start = m_end = 0;

		is_end = VO_TRUE;

		return size;
	}
}

VO_S64 mem_block::write( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	VO_U32 size = BLOCKSIZE - m_end;

	if( size == 0 )
		return 0;
	else
	{
		if( size > buffer_size )
		{
			memcpy( m_buffer + m_end , ptr_buffer , (VO_U32)buffer_size );
			m_end = m_end + (VO_U32)buffer_size;
			return buffer_size;
		}
		else if( size == buffer_size )
		{
			memcpy( m_buffer + m_end , ptr_buffer , (VO_U32)buffer_size );
			m_end = m_end + (VO_U32)buffer_size;
			is_full = VO_TRUE;
			return buffer_size;
		}
		else
		{
			memcpy( m_buffer + m_end , ptr_buffer , size );
			m_end = BLOCKSIZE;
			is_full = VO_TRUE;
			return size;
		}
	}
}

vo_mem_stream::vo_mem_stream(void):
m_ptr_block_head(0)
,m_ptr_block_tail(0)
,m_ptr_recycle_block(0)
{
}

vo_mem_stream::~vo_mem_stream(void)
{
	destroy_memblock();
}

VO_BOOL vo_mem_stream::open()
{
	close();
	create_memblock();

	return VO_TRUE;
}

VO_VOID vo_mem_stream::close()
{
	destroy_memblock();
}

VO_S64 vo_mem_stream::read( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	voCAutoLock lock( &m_lock );

	VO_S64 readsize = 0;

	mem_block * ptr_entry = m_ptr_block_head;

	while( readsize != (VO_S64)buffer_size && ptr_entry )
	{
		VO_S64 readed = ptr_entry->read( ptr_buffer , buffer_size - readsize );

		readsize += readed;
		ptr_buffer = ptr_buffer + readed;

		if( (VO_S64)buffer_size  == readsize )
		{
			if( ptr_entry->is_end )
			{
				recycle_head_memblock();
			}
			break;
		}

		//if( ptr_entry->is_end )
		{
			ptr_entry = ptr_entry->m_ptr_next;
			recycle_head_memblock();
		}
	}
	
	return readsize;
}

VO_S64 vo_mem_stream::write( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	return -1;
}

VO_BOOL vo_mem_stream::seek( VO_S64 pos , vo_stream_pos stream_pos  )
{
	return VO_FALSE;
}

VO_S64 vo_mem_stream::append( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	voCAutoLock lock( &m_lock );
	VO_S64 writesize = 0;

	if( m_ptr_block_tail == NULL )
		create_memblock();

	while( writesize != (VO_S64)buffer_size )
	{
		VO_S64 writen = m_ptr_block_tail->write( ptr_buffer , buffer_size - writesize );

		writesize += writen;
		ptr_buffer = ptr_buffer + writen;

		if( (VO_U64)writesize == buffer_size )
			break;

		if( m_ptr_block_tail->is_full )
			create_memblock();
	}

	//VOLOGI( "dowload ");
	return writesize;
}

VO_VOID vo_mem_stream::destroy_memblock()
{
	voCAutoLock lock( &m_lock );
	mem_block * ptr = m_ptr_block_head;

	while( ptr )
	{
		mem_block * ptr_temp = ptr;
		ptr = ptr->m_ptr_next;

		delete ptr_temp;
	}

	m_ptr_block_head = m_ptr_block_tail = NULL;

	ptr = m_ptr_recycle_block;

	while( ptr )
	{
		mem_block * ptr_temp = ptr;
		ptr = ptr->m_ptr_next;

		delete ptr_temp;
	}

	m_ptr_recycle_block = NULL;
}

VO_VOID vo_mem_stream::create_memblock()
{
	if( m_ptr_recycle_block )
	{
		if( m_ptr_block_head == NULL && m_ptr_block_tail == NULL )
		{
			m_ptr_block_head = m_ptr_block_tail = m_ptr_recycle_block;
		}
		else
		{
			m_ptr_block_tail->m_ptr_next = m_ptr_recycle_block;
			m_ptr_block_tail = m_ptr_block_tail->m_ptr_next;
		}

		m_ptr_recycle_block->reset();
		m_ptr_recycle_block = m_ptr_recycle_block->m_ptr_next;

		return;
	}

	if( m_ptr_block_head == NULL && m_ptr_block_tail == NULL )
	{
		m_ptr_block_head = m_ptr_block_tail = new mem_block;
	}
	else
	{
		m_ptr_block_tail->m_ptr_next = new mem_block;
		m_ptr_block_tail = m_ptr_block_tail->m_ptr_next;
	}

	if( !m_ptr_block_tail )
		VOLOGE("Critical Error!");
}

VO_VOID vo_mem_stream::recycle_head_memblock()
{
	if( m_ptr_block_head )
	{
		mem_block * ptr = m_ptr_block_head;

		if( m_ptr_block_head == m_ptr_block_tail )
		{
			m_ptr_block_head = m_ptr_block_tail = NULL;
		}
		else
		{
			m_ptr_block_head = m_ptr_block_head->m_ptr_next;
		}

		ptr->reset();
		ptr->m_ptr_next = m_ptr_recycle_block;
		m_ptr_recycle_block = ptr;
	}
}
