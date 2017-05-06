#if !defined(_IOS)
	#ifdef _PUSHPLAYPARSER

#include "vo_file_io.h"
#include "vo_file_parser.h"
#include "voOSFunc.h"
#include "voLog.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

#define MAX_WAIT_DISTANCE (5 * 1024 * 1024)
#define MAX_IO_TIMEOUT 5000


VO_PTR file_open(VO_FILE_SOURCE * pSource)
{
    vo_file_parser * ptr_fileparser = ( vo_file_parser * )pSource->nReserve;
    return ptr_fileparser->get_stream();
}

VO_S32 file_read(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)
{
    vo_file_io * ptr_io = (vo_file_io*)pFile;
    VO_S32 ret = ptr_io->read( (VO_PBYTE)pBuffer , uSize );

    VOLOGI( "Read size %u ret %d " , uSize , ret );

    return ret;
}

VO_S32 file_write(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)
{
    return -1;
}

VO_S32 file_flush(VO_PTR pFile)
{
    return -1;
}

VO_S64 file_seek(VO_PTR pFile, VO_S64 nPos, VO_FILE_POS uFlag)
{
    vo_file_io * ptr_io = (vo_file_io*)pFile;
    return ptr_io->seek( nPos , uFlag );
}

VO_S64 file_size(VO_PTR pFile)
{
    vo_file_io * ptr_io = (vo_file_io*)pFile;
    VO_S64 llSize = ptr_io->getRealFileSize();

	VOLOGI("file_size %lld", llSize);
	return llSize;
}

VO_S64 file_save(VO_PTR pFile)
{
    return -1;
}

VO_S32 file_close(VO_PTR pFile)
{
    vo_file_io * ptr_io = (vo_file_io*)pFile;
    ptr_io->seek( 0 , VO_FILE_BEGIN );
    return 0;
}

vo_file_io::vo_file_io()
:m_filepos(0)
,m_is_closing(VO_FALSE)
,m_ptr_org_op(0)
,m_org_filehandle(0)
,start_time(0xffffffff)
,m_filesize(0)
{
}

vo_file_io::~vo_file_io(void)
{
}

VO_VOID vo_file_io::init( vo_file_parser * ptr_org_op , VO_PTR org_filehandle  )
{
    m_ptr_org_op = ptr_org_op;
    m_org_filehandle = org_filehandle;
}

VO_S32 vo_file_io::read( VO_PBYTE buffer , VO_U32 toread )
{
    if( m_is_closing )
    {
        return -1;
    }

    VO_S64 readed = 0;

    if( m_mode == IO_NORMAL )
    {
        readed = m_ptr_org_op->FileRead( m_org_filehandle , buffer , toread );
        m_filepos = m_filepos + readed;
    }
    else if( m_mode == IO_CHECK )
    {
        VO_S64 readend_pos = m_filepos + toread;
        VO_S64 cur_pos = get_filesize();

        if( readend_pos > get_filesize() )
        {
            VO_U32 start = voOS_GetSysTime();

            while( readend_pos > get_filesize() )
            {
                voOS_Sleep( 30 );
                if( voOS_GetSysTime() - start >= MAX_IO_TIMEOUT )
                    return -1;
            }
        }

        readed = m_ptr_org_op->FileRead( m_org_filehandle , buffer , toread );
        m_filepos = m_filepos + readed;
    }

    return (VO_S32)readed;
}

VO_S64 vo_file_io::seek( VO_S64 seekpos , VO_FILE_POS pos )
{
    VO_S64 want_pos = 0;

    switch ( pos )
    {
    case VO_FILE_BEGIN:
        want_pos = seekpos;
        break;
    case VO_FILE_CURRENT:
        want_pos = m_filepos + seekpos;
        break;
    case VO_FILE_END:
        return -1;
        break;
    }

    if( m_mode == IO_NORMAL )
    {
        m_filepos = m_ptr_org_op->FileSeek( m_org_filehandle , want_pos , VO_FILE_BEGIN );
        return m_filepos;
    }
    else if( m_mode == IO_CHECK )
    {
        VO_S64 cur_pos = get_filesize();

        if( want_pos - cur_pos >= MAX_WAIT_DISTANCE )
            return -1;
        else if( want_pos - cur_pos >= 0 && want_pos - cur_pos < MAX_WAIT_DISTANCE )
        {
            while( want_pos > get_filesize() )
            {
                voOS_Sleep( 30 );
            }
        }
        
        m_filepos = m_ptr_org_op->FileSeek( m_org_filehandle , want_pos , VO_FILE_BEGIN );
        return m_filepos;
    }
}

VO_S64 vo_file_io::get_filesize()
{
/*
#ifdef _WIN32
#ifdef _DEBUG
    if( start_time == 0xffffffff )
        start_time = voOS_GetSysTime();

    VO_U32 passed_time = voOS_GetSysTime() - start_time;

    m_filesize = passed_time / 1000. * 500 * 1024;

    return m_filesize;
#endif
#endif
*/
    return m_ptr_org_op->FileSize( m_org_filehandle );
}

VO_VOID vo_file_io::setRealFileSize(VO_S64 llSize)
{
	m_RealFileSize = llSize ? llSize : 0x7FFFFFFFFFFFFFFF;

	VOLOGI("setRealFileSize inputparam %lld, m_RealFileSize %lld", llSize, m_RealFileSize);
}

VO_S64 vo_file_io::getRealFileSize()
{
	return (NORMALFILEPARSE == m_ptr_org_op->m_mode) ? get_filesize(): m_RealFileSize;
}

#endif
#endif
