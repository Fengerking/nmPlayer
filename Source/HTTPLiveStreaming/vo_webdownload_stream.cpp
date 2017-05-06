#include "vo_webdownload_stream.h"
#include "voLog.h"

#include "voString.h"
#if defined LINUX || defined _IOS_OPENSSL
#include "vo_https_stream.h"
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


vo_webdownload_stream::vo_webdownload_stream(void)
:m_ptr_stream(NULL)
{
    m_ulUserPwdSet = 0;
	memset(&m_sUserInfo, 0, sizeof(S_USER_INFO));
}

vo_webdownload_stream::~vo_webdownload_stream(void)
{
	delete m_ptr_stream;
}


VO_S64   vo_webdownload_stream::GetDownLoadTime()
{
    if(m_ptr_stream != NULL)
    {
        return m_ptr_stream->getdownloadtime();
    }

    return -1;
}


VO_BOOL vo_webdownload_stream::open( VO_CHAR * url , DownloadMode mode , VO_PBYTE ptr_key , VO_PBYTE ptr_iv , ProtectionType drm_type , DRM_Callback * ptr_drm_engine , void * drm_handle )
{
	if( m_ptr_stream )
		delete m_ptr_stream;

	m_ptr_stream = NULL;

	if( strstr( url , "http://" ) == url )
	{
		m_ptr_stream = new vo_http_stream();
		if(m_ulUserPwdSet == 1)
		{
		    m_ptr_stream->SetParamForHttp(1, m_sUserInfo.strUserName, m_sUserInfo.strPasswd);
		}
	}
#if defined LINUX || defined _IOS_OPENSSL
	else if( strstr( url , "https://" ) == url )
	{
		m_ptr_stream = new vo_https_stream();
	}
#endif
	else
	{
		return VO_FALSE;
	}

	if( !m_ptr_stream )
		return VO_FALSE;

	return m_ptr_stream->open( url , mode , ptr_key , ptr_iv , drm_type , ptr_drm_engine , drm_handle );

}

VO_BOOL vo_webdownload_stream::persist_open( VO_CHAR * url , DownloadMode mode , Persist_HTTP * ptr_persist , VO_PBYTE ptr_key , VO_PBYTE ptr_iv , ProtectionType drm_type , DRM_Callback * ptr_drm_engine , void * drm_handle )
{
    if( m_ptr_stream )
        delete m_ptr_stream;

    m_ptr_stream = NULL;

    if( strstr( url , "http://" ) == url )
    {
        m_ptr_stream = new vo_http_stream();
    }
#if defined LINUX || defined _IOS_OPENSSL
    else if( strstr( url , "https://" ) == url )
    {
        m_ptr_stream = new vo_https_stream();
    }
#endif
    else
    {
        return VO_FALSE;
    }

    if( !m_ptr_stream )
        return VO_FALSE;

    return m_ptr_stream->persist_open( url , mode , ptr_persist , ptr_key , ptr_iv , drm_type , ptr_drm_engine , drm_handle );
}

VO_VOID vo_webdownload_stream::close()
{
	if( m_ptr_stream )
		return m_ptr_stream->close();
	return;
}

VO_S64 vo_webdownload_stream::read( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	if( m_ptr_stream )
		return m_ptr_stream->read( ptr_buffer , buffer_size );
	return -1;
}

VO_S64 vo_webdownload_stream::write( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	if( m_ptr_stream )
		return m_ptr_stream->write( ptr_buffer , buffer_size );
	return -1;
}

VO_BOOL vo_webdownload_stream::seek( VO_S64 pos , vo_stream_pos stream_pos  )
{
	if( m_ptr_stream )
		return m_ptr_stream->seek( pos , stream_pos );
	return VO_FALSE;
}

VO_S64 vo_webdownload_stream::append( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	if( m_ptr_stream )
		return m_ptr_stream->append( ptr_buffer , buffer_size );
	return -1;
}

VO_S64 vo_webdownload_stream::get_content_length()
{
	if( m_ptr_stream )
		return m_ptr_stream->get_content_length();
	return -1;
}

VO_CHAR * vo_webdownload_stream::get_mime_type()
{
	if( m_ptr_stream )
		return m_ptr_stream->get_mime_type();
	return NULL;
}

VO_S64 vo_webdownload_stream::get_download_bitrate()
{
	if( m_ptr_stream )
		return m_ptr_stream->get_download_bitrate();
	return -1;
}

void vo_webdownload_stream::stop()
{
	if( m_ptr_stream )
		return m_ptr_stream->stop();
	return;
}

VO_S32 vo_webdownload_stream::get_lasterror()
{
	if( m_ptr_stream )
		return m_ptr_stream->get_lasterror();
	return -1;
}

VO_U32    vo_webdownload_stream::GetReDirectionURL(VO_CHAR*  pURLBuffer, VO_U32  ulLen)
{
    return m_ptr_stream->GetRedirectURL(pURLBuffer, ulLen);
}



VO_VOID vo_webdownload_stream::SetParamForHttp(VO_U32  uId, VO_VOID* pDataUser, VO_VOID*   pDataPwd)
{
    if((pDataUser == NULL) || (pDataPwd == NULL))
    {
        return;
    }

    m_ulUserPwdSet = 1;

	VOLOGI("user = %s, pwd= %s", (VO_CHAR*)pDataUser, (VO_CHAR*)pDataPwd);
	memcpy(m_sUserInfo.strUserName, (VO_CHAR*)pDataUser, strlen((VO_CHAR*)pDataUser));
	m_sUserInfo.ulUserNameLen = strlen((VO_CHAR*)pDataUser);
    memcpy(m_sUserInfo.strPasswd, (VO_CHAR*)pDataPwd, strlen((VO_CHAR*)pDataPwd));
	m_sUserInfo.ulUPasswdLen = strlen((VO_CHAR*)pDataPwd);	
}

