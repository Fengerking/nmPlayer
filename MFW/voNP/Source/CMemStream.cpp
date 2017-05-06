#include "CMemStream.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


VO_S64 CMemBlock::read( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	VO_U32 size = m_end - m_start;

	if( size > buffer_size )
	{
		memcpy( ptr_buffer , m_buffer + m_start , buffer_size );
		m_start = m_start + buffer_size;

		return buffer_size;
	}
	else if( size == buffer_size )
	{
		memcpy( ptr_buffer , m_buffer + m_start , buffer_size );
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

VO_S64 CMemBlock::write( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	VO_U32 size = BLOCKSIZE - m_end;

	if( size == 0 )
		return 0;
	else
	{
		if( size > buffer_size )
		{
			memcpy( m_buffer + m_end , ptr_buffer , buffer_size );
			m_end = m_end + buffer_size;
			return buffer_size;
		}
		else if( size == buffer_size )
		{
			memcpy( m_buffer + m_end , ptr_buffer , buffer_size );
			m_end = m_end + buffer_size;
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

CMemStream::CMemStream(void):
m_ptr_block_head(0)
,m_ptr_block_tail(0)
,m_ptr_recycle_block(0)
,m_bisend(VO_FALSE)
{
}

CMemStream::~CMemStream(void)
{
	destroy_memblock();
}

VO_BOOL CMemStream::open()
{
	voCAutoLock lock( &m_lock );

	close();
	create_memblock();

	return VO_TRUE;
}

VO_VOID CMemStream::close()
{
	voCAutoLock lock( &m_lock );

	destroy_memblock();
}

VO_S64 CMemStream::read( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	voCAutoLock lock( &m_lock );

	VO_S64 readsize = 0;

	CMemBlock * ptr_entry = m_ptr_block_head;

	while( (VO_U64)readsize != buffer_size && ptr_entry )
	{
		VO_S64 readed = ptr_entry->read( ptr_buffer , buffer_size - readsize );

		readsize += readed;
		ptr_buffer = ptr_buffer + readed;

		if( buffer_size  == (VO_U64)readsize )
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

    if( m_bisend && readsize == 0 )
        return -1;

    if( readsize == 0 )
        return -2;
	
	return readsize;
}

VO_S64 CMemStream::write( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	return -1;
}

VO_BOOL CMemStream::seek( VO_S64 pos , mux_stream_pos stream_pos  )
{
	return VO_FALSE;
}

VO_S64 CMemStream::size()
{
	voCAutoLock lock( &m_lock );

	if(!m_ptr_block_head && !m_ptr_block_tail)
		return 0;
	
	CMemBlock* ptr_curr = m_ptr_block_head;
	int total_size		= 0;
	
	while (ptr_curr)
	{
		total_size += (ptr_curr->m_end - ptr_curr->m_start);
		ptr_curr = ptr_curr->m_ptr_next;
	}
	
	return total_size;
}

VO_S64 CMemStream::append( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	voCAutoLock lock( &m_lock );
	VO_S64 writesize = 0;

	if( m_ptr_block_tail == NULL )
		create_memblock();

	while( (VO_U64)writesize != buffer_size )
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

VO_VOID CMemStream::destroy_memblock()
{
	voCAutoLock lock( &m_lock );
	CMemBlock * ptr = m_ptr_block_head;

	while( ptr )
	{
		CMemBlock * ptr_temp = ptr;
		ptr = ptr->m_ptr_next;

		delete ptr_temp;
	}

	m_ptr_block_head = m_ptr_block_tail = NULL;

	ptr = m_ptr_recycle_block;

	while( ptr )
	{
		CMemBlock * ptr_temp = ptr;
		ptr = ptr->m_ptr_next;

		delete ptr_temp;
	}

	m_ptr_recycle_block = NULL;
}

VO_VOID CMemStream::create_memblock()
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
		m_ptr_block_head = m_ptr_block_tail = new CMemBlock;
	}
	else
	{
		m_ptr_block_tail->m_ptr_next = new CMemBlock;
		m_ptr_block_tail = m_ptr_block_tail->m_ptr_next;
	}

	if( !m_ptr_block_tail )
		VOLOGE("[NPW]Critical Error!");
}

VO_VOID CMemStream::recycle_head_memblock()
{
	if( m_ptr_block_head )
	{
		CMemBlock * ptr = m_ptr_block_head;

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
