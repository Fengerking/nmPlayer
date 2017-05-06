#pragma once
#include "vo_stream.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define BLOCKSIZE (1024*500)

class mem_block
{
public:
	mem_block();
	~mem_block(){}

	VO_S64 read( VO_PBYTE ptr_buffer , VO_U64 buffer_size );
	VO_S64 write( VO_PBYTE ptr_buffer , VO_U64 buffer_size );
	VO_VOID reset(){ m_start = m_end = 0 ; is_end = is_full = VO_FALSE;  m_ptr_next = NULL;}

	VO_BOOL is_end;
	VO_BOOL is_full;

	VO_BYTE m_buffer[BLOCKSIZE];
	VO_U32 m_start;
	VO_U32 m_end;

	mem_block * m_ptr_next;
};

class vo_mem_stream :
	public vo_stream
{
public:
	vo_mem_stream(void);
	virtual ~vo_mem_stream(void);

	virtual VO_BOOL open();
	virtual VO_VOID close();

	virtual VO_S64 read( VO_PBYTE ptr_buffer , VO_U64 buffer_size );
	virtual VO_S64 write( VO_PBYTE ptr_buffer , VO_U64 buffer_size );
	virtual VO_BOOL seek( VO_S64 pos , vo_stream_pos stream_pos  );
	virtual VO_S64 append( VO_PBYTE ptr_buffer , VO_U64 buffer_size );

protected:
	virtual VO_VOID destroy_memblock();
	virtual VO_VOID create_memblock();

protected:
	mem_block * m_ptr_block_head;
	mem_block * m_ptr_block_tail;

	mem_block * m_ptr_recycle_block;

	voCMutex m_lock;
};

#ifdef _VONAMESPACE
}
#endif