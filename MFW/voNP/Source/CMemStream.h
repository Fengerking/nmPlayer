#pragma once
#include "CMuxerStream.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#define BLOCKSIZE (1024*500)

class CMemBlock
{
public:
	CMemBlock():m_start(0),m_end(0),is_end(VO_FALSE),is_full(VO_FALSE),m_ptr_next(0){}
	~CMemBlock(){}

	VO_S64 read( VO_PBYTE ptr_buffer , VO_U64 buffer_size );
	VO_S64 write( VO_PBYTE ptr_buffer , VO_U64 buffer_size );
	VO_VOID reset(){ m_start = m_end = 0 ; is_end = is_full = VO_FALSE;  m_ptr_next = NULL;}

	VO_BYTE m_buffer[BLOCKSIZE];
	VO_U32 m_start;
	VO_U32 m_end;
	VO_BOOL is_end;
	VO_BOOL is_full;	

	CMemBlock * m_ptr_next;
};

class CMemStream :
	public CMemBlock
{
public:
	CMemStream(void);
	virtual ~CMemStream(void);

	virtual VO_BOOL open();
	virtual VO_VOID close();

	virtual VO_S64 read( VO_PBYTE ptr_buffer , VO_U64 buffer_size );
	virtual VO_S64 write( VO_PBYTE ptr_buffer , VO_U64 buffer_size );
	virtual VO_BOOL seek( VO_S64 pos , mux_stream_pos stream_pos  );
	virtual VO_S64 append( VO_PBYTE ptr_buffer , VO_U64 buffer_size );

	virtual VO_VOID write_eos(){ m_bisend = VO_TRUE; }
	virtual VO_S32 get_lasterror(){ return 0; }
	virtual VO_S64 size();

protected:
	virtual VO_VOID destroy_memblock();
	virtual VO_VOID create_memblock();
	virtual VO_VOID recycle_head_memblock();

protected:
	CMemBlock * m_ptr_block_head;
	CMemBlock * m_ptr_block_tail;

	CMemBlock * m_ptr_recycle_block;

	voCMutex m_lock;

    VO_BOOL m_bisend;
};
    
#ifdef _VONAMESPACE
}
#endif // End _VONAMESPACE

