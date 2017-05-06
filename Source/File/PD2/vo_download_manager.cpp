//#include "StdAfx.h"
#include "vo_download_manager.h"
#include "vo_http_downloader.h"
#include "voLog.h"

#define LOG_TAG "voCOMXAudioSink"

vo_download_manager::vo_download_manager(void):
	m_ptr_downloader(0)
{
}

vo_download_manager::~vo_download_manager(void)
{
	if( m_ptr_downloader )
	{
		delete m_ptr_downloader;
		m_ptr_downloader = 0;
	}
}

CALLBACK_RET vo_download_manager::received_data( VO_S64 physical_pos , VO_PBYTE ptr_buffer , VO_S32 size )
{
	return m_ptr_callback->received_data( physical_pos , ptr_buffer , size );
}

CALLBACK_RET vo_download_manager::download_notify( DOWNLOAD_CALLBACK_NOTIFY_ID id , VO_PTR ptr_data )
{
	return m_ptr_callback->download_notify( id , ptr_data );
}

VO_BOOL vo_download_manager::set_url( VO_CHAR * str_url , VOPDInitParam * ptr_PD_param )
{
	VOLOGI( "url is: %s" , str_url );
	if( strstr( str_url , "http://" ) == str_url )
	{
		VOLOGI( "+vo_download_manager::set_url" );
		m_ptr_downloader = new vo_http_downloader();
		m_ptr_downloader->set_download_callback( this );
		return m_ptr_downloader->set_url( str_url , ptr_PD_param );
	}

	VOLOGI( "-vo_download_manager::set_url" );
	return VO_FALSE;
}

VO_BOOL vo_download_manager::start_download( VO_S64 download_pos/* = 0 */, VO_S64 download_size/* = DOWNLOADTOEND*/ , VO_BOOL is_async/* = VO_TRUE*/ )
{
	stop_download();
	return m_ptr_downloader->start_download( download_pos , download_size , is_async );
}

VO_VOID vo_download_manager::stop_download()
{
	if( m_ptr_downloader )
		m_ptr_downloader->stop_download();
}

VO_S64 vo_download_manager::get_actual_filesize()
{
	return m_ptr_downloader->get_actual_filesize();
}

VO_BOOL vo_download_manager::is_downloading()
{
	return m_ptr_downloader->is_downloading();
}

VO_VOID vo_download_manager::get_dowloadinfo( VO_DOWNLOAD_INFO * ptr_info )
{
	m_ptr_downloader->get_dowloadinfo( ptr_info );
}

VO_BOOL vo_download_manager::is_support_seek()
{
	return m_ptr_downloader->is_support_seek();
}

VO_U32 vo_download_manager::get_duration()
{
	return m_ptr_downloader->get_duration();
}

