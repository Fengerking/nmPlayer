#include "vo_download_manager.h"
#include "vo_http_downloader.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

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
	VOLOGI("url is: %s" , str_url );
	if( strstr( str_url , "http://" ) == str_url ||
		strstr( str_url , "https://" ) == str_url)
	{
		VOLOGI( "+vo_download_manager::set_url" );
		m_ptr_downloader = new vo_http_downloader();
		if( !m_ptr_downloader )
		{
			VOLOGE( "new obj failed" );
			return VO_FALSE;
		}
		m_ptr_downloader->set_download_callback( this );
		return m_ptr_downloader->set_url( str_url , ptr_PD_param );
	}

	VOLOGI("-vo_download_manager::set_url" );
	return VO_FALSE;
}

VO_BOOL vo_download_manager::start_download( VO_S64 download_pos/* = 0 */, VO_S64 download_size/* = DOWNLOADTOEND*/ , VO_BOOL is_async/* = VO_TRUE*/ )
{
	stop_download();
	return m_ptr_downloader->start_download( download_pos , download_size , is_async );
}

VO_BOOL vo_download_manager::start_download_inthread( VO_S64 download_pos/* = 0 */, VO_S64 download_size/* = DOWNLOADTOEND*/ , VO_BOOL is_async/* = VO_TRUE*/  )
{
	stop_download();
	return m_ptr_downloader->start_download_inthread( download_pos , download_size , is_async );
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

VO_VOID vo_download_manager::set_to_close(VO_BOOL is_to_exit)
{
	if(m_ptr_downloader)
		return m_ptr_downloader->set_to_close(is_to_exit);
}

VO_BOOL vo_download_manager::is_downloading()
{
    if( !m_ptr_downloader )
        return VO_FALSE;

	return m_ptr_downloader->is_downloading();
}


VO_VOID vo_download_manager::get_dowloadinfo( VO_DOWNLOAD_INFO * ptr_info )
{
    if( !m_ptr_downloader )
        return;

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

//set data pointer and len for check whether the server can seek or not
VO_VOID vo_download_manager::set_comparedata(VO_PBYTE pdata , VO_S32 size)
{
	return m_ptr_downloader->set_comparedata(pdata, size);
}

VO_VOID vo_download_manager::slow_down(VO_U32 slowdown)
{
	if( m_ptr_downloader )
		m_ptr_downloader->slow_down( slowdown );
}

VO_VOID vo_download_manager::pause_connection(VO_BOOL recvpause)
{
	if( m_ptr_downloader )
		m_ptr_downloader->pause_connection( recvpause );
}

VO_VOID vo_download_manager::set_connectretry_times(VO_S32 retrytimes)
{
	if( m_ptr_downloader )
		m_ptr_downloader->set_connectretry_times( retrytimes );
}

VO_VOID vo_download_manager::set_workpath(VO_TCHAR * strWorkPath)
{
	if( m_ptr_downloader )
		m_ptr_downloader->set_workpath( strWorkPath );
}

VO_VOID vo_download_manager::set_dlna_param(VO_VOID* pobj)
{
	if( m_ptr_downloader )
		m_ptr_downloader->set_dlna_param( pobj );
}