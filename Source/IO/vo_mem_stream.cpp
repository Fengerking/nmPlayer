#include "vo_mem_stream.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define UPPER_MEMORY_USAGE	20 * 1024
#define BOTTOM_MEMORY_USAGE 10 * 1024
#define MAX_MEMORY_USAGE	30 * 1024
#define MAX_VOU64			0xffffffffffffffffULL

VO_S64 mem_block::read( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	VO_U32 size = m_end - m_start;

	if( size > buffer_size )
	{
		memcpy( ptr_buffer , m_buffer + m_start , (VO_S32)buffer_size );
		m_start = (VO_U32)(m_start + buffer_size);

		return buffer_size;
	}
	else if( size == buffer_size )
	{
		memcpy( ptr_buffer , m_buffer + m_start ,(VO_S32) buffer_size );
		m_start = m_end = 0;

		m_is_end = VO_TRUE;

		return buffer_size;
	}
	else
	{
		memcpy( ptr_buffer , m_buffer + m_start , size );
		m_start = m_end = 0;

		m_is_end = VO_TRUE;

		return size;
	}
}

VO_S64 mem_block::write( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	VO_U32 size = BLOCKSIZE - m_end;

	if( size == 0 )
	{
		return 0;
	}
	else
	{
		if( size > buffer_size )
		{
			memcpy( m_buffer + m_end , ptr_buffer , (VO_S32)buffer_size );
			m_end = (VO_U32)(m_end + buffer_size);
			return buffer_size;
		}
		else if( size == buffer_size )
		{
			memcpy( m_buffer + m_end , ptr_buffer , (VO_S32)buffer_size );
			m_end = (VO_U32)(m_end + buffer_size);
			m_is_full = VO_TRUE;
			return buffer_size;
		}
		else
		{
			memcpy( m_buffer + m_end , ptr_buffer , size );
			m_end = BLOCKSIZE;
			m_is_full = VO_TRUE;
			return size;
		}
	}
}

vo_mem_stream::vo_mem_stream(void):
m_ptr_block_head(0)
,m_ptr_block_tail(0)
,m_ptr_recycle_block(0)
,m_cur_download_pos(0)
,m_cur_read_pos(0)
,m_filesize(MAX_VOU64)
,m_total_allocmem(0)
,m_is_slowspeed(VO_FALSE)
{
	m_buffer2downloadCB.HttpStreamCBFunc = 0;
	m_buffer2downloadCB.handle = 0;
}

vo_mem_stream::~vo_mem_stream(void)
{
	destroy_memblock();
}

VO_BOOL vo_mem_stream::open()
{
	close();

	if(!create_memblock())
	{
		return VO_FALSE;
	}

	m_cur_download_pos = 0;
	m_cur_read_pos = 0;
	m_filesize = MAX_VOU64;

	return VO_TRUE;
}

VO_VOID vo_mem_stream::close()
{
	destroy_memblock();
}

VO_S64 vo_mem_stream::read( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	voCAutoLock lock( &m_lock );

	//if file size has been notified, we should check it
//	if( m_filesize == 0 )
//		return 0;

	VO_S64 readsize = 0;

	mem_block * ptr_entry = m_ptr_block_head;

	while( readsize != buffer_size && ptr_entry )
	{
		VO_S64 readed = ptr_entry->read( ptr_buffer , buffer_size - readsize );

		readsize += readed;
		ptr_buffer = ptr_buffer + readed;

		if( buffer_size  == readsize )
		{
			if( ptr_entry->m_is_end )
			{
				recycle_head_memblock();
			}
			break;
		}

		if( ptr_entry->m_is_end )
		{
			ptr_entry = ptr_entry->m_ptr_next;
			recycle_head_memblock();
		}
		else
		{
			//it is strange when we read data which did not meet requirement, and the block end flag is not true.
			VOLOGE("it is strange situation,please contact stony");
			break;
		}
	}
	
	m_cur_read_pos = (VO_U64)(m_cur_read_pos + readsize);

	//check the memory allocated for this instance
	if( m_is_slowspeed && get_cur_used_mem() < BOTTOM_MEMORY_USAGE )
	{
		m_is_slowspeed = VO_FALSE;
		Notify( 0 , 0 , VO_CONTENT_DOWNLOAD_SPEED );
		VOLOGI("resume Download_Speed");
	}

	//check the memory allocated for this instance
	if( !m_is_slowspeed && get_cur_used_mem() > UPPER_MEMORY_USAGE )
	{
		m_is_slowspeed = VO_TRUE;
		Notify( 0 , 100 , VO_CONTENT_DOWNLOAD_SPEED );
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
	{
		if(!create_memblock())
		{
			return -1;
		}
	}

	while( writesize != buffer_size )
	{
		VO_S64 writen = m_ptr_block_tail->write( ptr_buffer , buffer_size - writesize );

		writesize += writen;
		ptr_buffer = ptr_buffer + writen;

		if( writesize == buffer_size )
			break;

		if( m_ptr_block_tail->m_is_full )
		{
			if(!create_memblock())
			{
				break;
			}
		}
	}

	m_cur_download_pos = (VO_U64)(m_cur_download_pos + writesize);

	//VOLOGI( "dowload ");
	if( writesize != buffer_size )
	{
		return -1;
	}
	else
	{
		return writesize;
	}
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

	Notify( 0 , m_total_allocmem , VO_CONTENT_MEMINFO_MINUS );
	m_total_allocmem = 0;
}

VO_BOOL vo_mem_stream::create_memblock()
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

		mem_block * ptr_temp = m_ptr_recycle_block->m_ptr_next;
		m_ptr_recycle_block->reset();
		m_ptr_recycle_block = ptr_temp;

		return VO_TRUE;
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
	{
		VOLOGE("create_memblock Error!");
		return VO_FALSE;
	}
	else
	{
		m_total_allocmem += BLOCKSIZE/1024;
		Notify( 0 , BLOCKSIZE/1024 , VO_CONTENT_MEMINFO_PLUS );
	}

	//if already control the download speed, but the memory consumed exceed MAX_MEMORY_USAGE, then we should slow it more.
	//if( get_totalloc_mem() > MAX_MEMORY_USAGE && get_cur_used_mem() > UPPER_MEMORY_USAGE )
	//{
	//	m_is_slowspeed = VO_TRUE;
	//	Notify( 0 , 2000 , VO_CONTENT_DOWNLOAD_SPEED );
	//}

	return VO_TRUE;
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

VO_U64  vo_mem_stream::get_totalloc_mem()
{
	return m_total_allocmem;
}

VO_U64	vo_mem_stream::get_cur_used_mem()
{
	VO_U64 used_mem = 0;
	if( m_cur_download_pos >= m_cur_read_pos )
	{
		used_mem = (VO_U64)((( m_cur_download_pos - m_cur_read_pos ) / BLOCKSIZE) + 1) * ( BLOCKSIZE / 1024 );
	}

	return used_mem;
}

VO_S32 vo_mem_stream::get_lasterror()
{ 
	print_memInfo();
	return 0; 
}

VO_VOID vo_mem_stream::print_memInfo()
{
	VOLOGI("cur_download_pos: %lld , cur_read_pos: %lld , total_alloc_mem: %lld KBytes, cur_used_mem: %lld KBytes, clip size: %lld Bytes" , m_cur_download_pos , m_cur_read_pos ,  get_totalloc_mem() , get_cur_used_mem() , m_filesize );
}

VO_VOID vo_mem_stream::set_buff2download_callback( VO_PTR pParam )
{
	voCAutoLock lock(&m_lock_w2http);
	VO_HTTP_DOWNLOAD2BUFFERCALLBACK* ptr = ( VO_HTTP_DOWNLOAD2BUFFERCALLBACK* )pParam;
	m_buffer2downloadCB.HttpStreamCBFunc = ptr->HttpStreamCBFunc;
	m_buffer2downloadCB.handle = ptr->handle;
}

VO_S32 vo_mem_stream::Notify( VO_PBYTE param , VO_U64 size , IO_HTTPDATA2BUFFER_DATA_TYPE type )
{
	voCAutoLock lock(&m_lock_w2http);
	if( m_buffer2downloadCB.HttpStreamCBFunc )
	{
		return (VO_S32)m_buffer2downloadCB.HttpStreamCBFunc( m_buffer2downloadCB.handle , param , size , type );
	}
	return 0;
}