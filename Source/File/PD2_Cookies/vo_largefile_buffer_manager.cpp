//#include "StdAfx.h"
#include "vo_largefile_buffer_manager.h"
#include "vo_largefile_buffer.h"
#include "vo_file_buffer.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

vo_largefile_buffer_manager::vo_largefile_buffer_manager(void)
	:m_ptr_buffer(0)
	,m_is_initmode(VO_FALSE)
{
}

vo_largefile_buffer_manager::~vo_largefile_buffer_manager(void)
{
	uninit();
}

BUFFER_CALLBACK_RET vo_largefile_buffer_manager::buffer_notify( BUFFER_CALLBACK_NOTIFY_ID notify_id , VO_PTR ptr_data )
{
	return m_ptr_buffer_callback->buffer_notify( notify_id , ptr_data );
}

VO_BOOL vo_largefile_buffer_manager::init( VOPDInitParam * ptr_param )
{
	uninit();

	m_headerdata_buffer.init( ptr_param );
	m_headerdata_buffer.set_buffer_callback(this);

	m_ptr_buffer = new vo_largefile_buffer();
	if( !m_ptr_buffer )
	{
		VOLOGE( "new obj failed" );
		return VO_FALSE;
	}

	m_ptr_buffer->set_buffer_callback(this);
	return m_ptr_buffer->init( ptr_param );
}

VO_VOID vo_largefile_buffer_manager::uninit()
{
	if( m_ptr_buffer )
	{
		m_ptr_buffer->uninit();
		delete m_ptr_buffer;
		m_ptr_buffer = 0;
	}
}

VO_S64 vo_largefile_buffer_manager::read( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread , VO_BOOL issync )
{
 	VO_S64 readed = 0;
	readed = m_headerdata_buffer.read( physical_pos , buffer , toread );
 
 	if( !m_is_initmode && m_ptr_buffer && readed < toread )
 		readed = readed + m_ptr_buffer->read( physical_pos + readed , buffer + readed , toread - readed );
 
	if(readed <= 0)
	{
		VOLOGE( "[moses]physical_pos: %lld , m_filesize: %lld,  toread %lld" , physical_pos , m_filesize , toread );
	}

 	return readed;
}

VO_S64 vo_largefile_buffer_manager::write( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 towrite )
{
	VO_S64 writen = m_headerdata_buffer.write( physical_pos , buffer , towrite );

	if(writen != towrite && m_is_initmode)
	{
		//header buffer maybe full, so we need to empty it #8493 pantech
		m_headerdata_buffer.reset();
		return write(physical_pos, buffer, towrite);
	}

	if( !m_is_initmode && m_ptr_buffer && writen < towrite )
		writen = writen + m_ptr_buffer->write( physical_pos + writen , buffer + writen , towrite - writen );

	return writen;
}

VO_S64 vo_largefile_buffer_manager::seek( VO_S64 physical_pos )
{
	return physical_pos;
}

VO_VOID vo_largefile_buffer_manager::reset()
{
	if( m_ptr_buffer )
		m_ptr_buffer->reset();
}

VO_VOID vo_largefile_buffer_manager::set_initmode( VO_BOOL isset )
{
	m_is_initmode = isset;
	m_headerdata_buffer.set_initmode( isset );
}

VO_VOID vo_largefile_buffer_manager::set_filesize( VO_S64 filesize )
{
	m_headerdata_buffer.set_filesize( filesize );
	m_ptr_buffer->set_filesize( filesize );

	vo_buffer::set_filesize( filesize );
}

VO_BOOL vo_largefile_buffer_manager::rerrange_cache( headerdata_element * ptr_info , VO_S32 info_number )
{
	return m_headerdata_buffer.rerrange_cache( ptr_info , info_number , m_ptr_buffer );
}

VO_VOID vo_largefile_buffer_manager::get_buffer_info( BUFFER_INFO * ptr_info )
{
	m_ptr_buffer->get_buffer_info( ptr_info );
}

VO_VOID vo_largefile_buffer_manager::get_buffering_start_info( BUFFER_INFO * ptr_info )
{
	m_ptr_buffer->get_buffering_start_info( ptr_info );
}

