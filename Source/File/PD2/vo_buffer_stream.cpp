//#include "StdAfx.h"
#include "vo_buffer_stream.h"
#include "voLog.h"
#include "voOSFunc.h"

#define LOG_TAG "voCOMXAudioSink"

vo_buffer_stream::vo_buffer_stream( vo_buffer * ptr_buffer )
:m_ptr_buffer(ptr_buffer)
,m_filepos(0)
,m_is_closing(VO_FALSE)
{
}

vo_buffer_stream::~vo_buffer_stream(void)
{
}

VO_S32 vo_buffer_stream::read( VO_PBYTE buffer , VO_U32 toread )
{
	VO_U32 start = voOS_GetSysTime();
	if( m_is_closing )
	{
		VOLOGI("m_is_closing");
		return -1;
	}

	VO_S64 readed = m_ptr_buffer->read( m_filepos , buffer , toread );
	m_filepos = m_filepos + readed;

	VO_U32 end = voOS_GetSysTime();

	VOLOGE( " Read From %lld Size %lld Cost %u ms! " , m_filepos - readed , readed , end - start );

	return (VO_S32)readed;
}

VO_S64 vo_buffer_stream::seek( VO_S64 seekpos , VO_FILE_POS pos )
{
	VOLOGE( "vo_buffer_stream::seek: seekpos %lld , pos %d" , seekpos , (VO_S32)pos );
	switch ( pos )
	{
	case VO_FILE_BEGIN:
		m_filepos = m_ptr_buffer->seek( seekpos );
		break;
	case VO_FILE_CURRENT:
		m_filepos = m_ptr_buffer->seek( m_filepos + seekpos );
		break;
	case VO_FILE_END:
		m_filepos = m_ptr_buffer->seek( get_filesize() + seekpos );
		break;
	}

	return m_filepos;
}

VO_S64 vo_buffer_stream::get_filesize()
{
	VO_S64 size = m_ptr_buffer->get_filesize();
	VOLOGE( "FileSize: %lld" , size );
	return size;
}
