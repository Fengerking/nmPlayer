//#include "StdAfx.h"
#include "vo_buffer_manager.h"
#include "vo_largefile_buffer_manager.h"
#include "vo_file_buffer.h"
#include "vo_smallfile_buffer_manager.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

vo_buffer_manager::vo_buffer_manager(void)
	:m_ptr_buffer(0)
{
	m_is_initmode = VO_FALSE;
	m_filesize = 0;
}

vo_buffer_manager::~vo_buffer_manager(void)
{
	uninit();
}

BUFFER_CALLBACK_RET vo_buffer_manager::buffer_notify( BUFFER_CALLBACK_NOTIFY_ID notify_id , VO_PTR ptr_data )
{
	return m_ptr_buffer_callback->buffer_notify( notify_id , ptr_data );
}

VO_BOOL vo_buffer_manager::init( VOPDInitParam * ptr_param )
{
	uninit();
	memcpy( &m_initparam , ptr_param , sizeof( m_initparam ) );

	return VO_TRUE;
}

VO_VOID vo_buffer_manager::uninit()
{
	if( m_ptr_buffer )
	{
		m_ptr_buffer->uninit();
		delete m_ptr_buffer;
		m_ptr_buffer = 0;
	}
}

VO_S64 vo_buffer_manager::read( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread , VO_BOOL issync )
{
 	VO_S64 readed = 0;

	if( m_filesize == -1 )
		return m_ptr_buffer->read( physical_pos + readed , buffer + readed , toread , issync );

	if( physical_pos >= m_filesize )
		return -1;

	if( m_filesize - physical_pos < toread )
		toread = m_filesize - physical_pos;
 
 	if( m_ptr_buffer && toread )
 		readed = m_ptr_buffer->read( physical_pos + readed , buffer + readed , toread , issync );
 
 	return readed;
}

VO_S64 vo_buffer_manager::write( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 towrite )
{
	VO_S64 writen = 0;

	if( m_ptr_buffer )
		writen = m_ptr_buffer->write( physical_pos + writen , buffer + writen , towrite );

	return writen;
}

VO_S64 vo_buffer_manager::seek( VO_S64 physical_pos )
{
	return m_ptr_buffer->seek( physical_pos );
}

VO_VOID vo_buffer_manager::reset()
{
	if( m_ptr_buffer )
		m_ptr_buffer->reset();
}

VO_VOID vo_buffer_manager::set_initmode( VO_BOOL isset )
{
	m_is_initmode = isset;
	if( m_ptr_buffer )
		m_ptr_buffer->set_initmode( isset );
}

VO_VOID vo_buffer_manager::set_filesize( VO_S64 filesize )
{
	if( m_filesize )
	{
		//we allow change file size when 
		//1,the size is unknow, it maybe in endless download mode, so after download, the size is ok
		//2,the size is small, so the previous size maybe shoutcast list, so we need to change it to media file actual size.
		if( m_filesize == -1 || m_filesize < 2048 )
		{
			m_ptr_buffer->set_filesize( filesize );
			vo_buffer::set_filesize( filesize );

			return;
		}
	}
	else
	{
		//the size info is not inited, so mem must be not allocated yet
		if ( m_initparam.nMaxBuffer * 1024 < filesize || filesize == -1 )
		{
			VOLOGE("Created Largefile Buffer" );
			m_ptr_buffer = new vo_largefile_buffer_manager();
		}
		else
		{
			VOLOGE( "Created Smallfile Buffer" );
			m_ptr_buffer = new vo_smallfile_buffer_manager();
		}

		if( !m_ptr_buffer )
		{
			VOLOGE( "new obj failed" );
			return;
		}

		m_ptr_buffer->init( &m_initparam );
		m_ptr_buffer->set_buffer_callback( this );
		m_ptr_buffer->set_filesize( filesize );
		m_ptr_buffer->set_initmode( m_is_initmode );

		vo_buffer::set_filesize( filesize );
	}
}

VO_BOOL vo_buffer_manager::rerrange_cache( headerdata_element * ptr_info , VO_S32 info_number )
{
	return m_ptr_buffer->rerrange_cache( ptr_info , info_number );
}

VO_VOID vo_buffer_manager::get_buffer_info( BUFFER_INFO * ptr_info )
{
	m_ptr_buffer->get_buffer_info( ptr_info );
}

VO_VOID vo_buffer_manager::get_buffering_start_info( BUFFER_INFO * ptr_info )
{
	m_ptr_buffer->get_buffering_start_info( ptr_info );
}

