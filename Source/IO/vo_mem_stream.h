#pragma once
#include "voYYDef_SourceIO.h"
#include "vo_stream.h"
#include "voCMutex.h"
#include "vo_http_struct.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#define BLOCKSIZE (1024*500)

class mem_block
{
public:
	mem_block():m_start(0),m_end(0),m_is_end(VO_FALSE),m_is_full(VO_FALSE),m_ptr_next(0){}
	~mem_block(){}

	VO_S64 read( VO_PBYTE ptr_buffer , VO_U64 buffer_size );
	VO_S64 write( VO_PBYTE ptr_buffer , VO_U64 buffer_size );
	VO_VOID reset(){ m_start = m_end = 0 ; m_is_end = m_is_full = VO_FALSE;  m_ptr_next = NULL;}

	VO_BOOL m_is_end;
	VO_BOOL m_is_full;

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

	virtual VO_S32 get_lasterror();


	VO_U64	GetCurPos(){ return m_cur_read_pos; }
	VO_U64	get_cur_download_pos(){ return m_cur_download_pos; }
	VO_VOID set_filesize( VO_U64 size ){ m_filesize = size; }
	VO_U64	get_cur_used_mem();
	VO_U64  get_totalloc_mem();
	VO_VOID print_memInfo();
	VO_VOID set_buff2download_callback( VO_PTR pParam );

protected:
	virtual VO_VOID destroy_memblock();
	virtual VO_BOOL create_memblock();
	virtual VO_VOID recycle_head_memblock();
	VO_S32 Notify( VO_PBYTE param , VO_U64 size , IO_HTTPDATA2BUFFER_DATA_TYPE type );
protected:
	mem_block * m_ptr_block_head;
	mem_block * m_ptr_block_tail;

	mem_block * m_ptr_recycle_block;

	voCMutex m_lock;

	VO_U64	 m_cur_download_pos;
	VO_U64	 m_cur_read_pos;
	VO_U64	 m_filesize;
	VO_U64	 m_total_allocmem;
	VO_BOOL  m_is_slowspeed;
	voCMutex m_lock_w2http;
	VO_HTTP_DOWNLOAD2BUFFERCALLBACK  m_buffer2downloadCB;
};

#ifdef _VONAMESPACE
}
#endif