#include "voOSFunc.h"
#include "vo_http_session.h"
#include "vo_http_stream.h"
#include "vo_mem_stream.h"
#include "vo_http_utils.h"
#include "voLog.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

vo_http_sessions_info g_http_session_info;

#define IOFILENOTOPEN 0
#define IOFILEOPENING 1
#define IOFILEOPEN_SUCCESS 3
#define IOFILEOPEN_FAILED 4
#define IOFILEREAD_READY  5


#ifdef _VONAMESPACE
}
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
vo_http_session::vo_http_session()
{
	m_handle.download_handle = ( VO_HANDLE )0;
	m_handle.buffer_handle = ( VO_HANDLE )0;
	m_write2buffercb.HttpStreamCBFunc = 0;
	m_write2buffercb.handle = 0;
	m_iocb.hHandle = 0;
	m_iocb.IO_Callback = 0;
	m_append_datacnt = 0;
	m_IOFileOpenStatus = IOFILENOTOPEN;
	m_open_start_time = 0;
}

vo_http_session::~vo_http_session()
{
}

VO_U32 vo_http_session::Init( VO_PTR pSource , VO_U32 uFlag , VO_SOURCE2_IO_ASYNC_CALLBACK * pAsyncCallback )
{
	vo_http_stream * ptr_stream = 0;

	if( voiostrnicmp( (char *)pSource , "http://" , strlen("http://")) == 0 || 
		voiostrnicmp( (char *)pSource , "https://", strlen("https://") ) == 0 )
	{
		ptr_stream = new vo_http_stream;
	}
	else
	{
		return VO_SOURCE2_IO_PROTOCOL_NOTSUPPORT;
	}

	if( !ptr_stream )
	{
		return VO_SOURCE2_IO_FAIL;
	}

	VO_CHAR * ptr_redirect_url = g_http_session_info.get_redirecturl( (char *) pSource);

	if( ptr_redirect_url )
	{
		pSource = ptr_redirect_url;
	}

	ptr_stream->initial( (char *)pSource );

	vo_stream * ptr_buffer_stream = new vo_mem_stream();
	if( !ptr_buffer_stream )
	{
		return VO_SOURCE2_IO_FAIL;
	}
		
	m_handle.download_handle = ( VO_HANDLE )ptr_stream;
	m_handle.buffer_handle = ( VO_HANDLE )ptr_buffer_stream;

	//if the call back had been set outside , we should not overwrite it.
	if( !m_write2buffercb.HttpStreamCBFunc )
	{
		m_write2buffercb.handle = this;
		m_write2buffercb.HttpStreamCBFunc = vo_http_session::http_stream_callback;
		//set callback function to http stream
		ptr_stream->set_download2buff_callback( &m_write2buffercb );
		//set callback function to mem stream
		((vo_mem_stream*)ptr_buffer_stream)->set_buff2download_callback( &m_write2buffercb );
	}

	//we should protect module instance count, since it will decide vossl be freed or not.
	voCAutoLock lock(&m_sslock);
	g_http_session_info.m_module_instancecnt++;
	VOLOGI( "module_instance count of sourceio:%d" , g_http_session_info.m_module_instancecnt );

	return VO_SOURCE2_IO_OK;
}

VO_U32 vo_http_session::UnInit()
{
	vo_http_stream * ptr_stream = ( vo_http_stream * )m_handle.download_handle;

	if( !ptr_stream )
	{
	//	return VO_SOURCE2_IO_NULLPOINTOR;
		;
	}
	else
	{
		Persist_HTTP * ptr_persist = ptr_stream->get_persisthttp();
		delete ptr_stream;
		ptr_stream = 0;
		
		if( ptr_persist )
		{
			ptr_persist->unlock();
		}
	}

	vo_stream * ptr_buffer_stream = ( vo_stream * )m_handle.buffer_handle;
	if( !ptr_buffer_stream )
	{
	//	return VO_SOURCE2_IO_NULLPOINTOR;
		;
	}
	else
	{
		delete ptr_buffer_stream;
		ptr_buffer_stream = 0;
	}

	voCAutoLock lock(&m_sslock);
	g_http_session_info.m_module_instancecnt--;
	VOLOGI( "module_instance count of sourceio:%d" , g_http_session_info.m_module_instancecnt );

	if( g_http_session_info.m_module_instancecnt <= 0 )
	{
	//	g_http_session_info.m_sslload.FreeSSL();
	}

	return VO_SOURCE2_IO_OK;
}

VO_VOID vo_http_session::UpdateDNSCache()
{
	vo_http_stream * ptr_stream = ( vo_http_stream * )m_handle.download_handle;
	if( !ptr_stream )
	{
		VOLOGI("download stream pointer is null" );
		return;
	}

	if( ptr_stream->get_host() && ptr_stream->getdnscacheip() )
	{
		DNS_Record dnsrecord;
		memset( &dnsrecord , 0 , sizeof(DNS_Record) );
		strcpy( dnsrecord.host , ptr_stream->get_host() );
		strcpy( dnsrecord.ipaddr , ptr_stream->getdnscacheip() );
		dnsrecord.expiredtime = voOS_GetSysTime() + DNSCACHE_EXPIREDTIME;
		g_http_session_info.UpdateDnsCache( &dnsrecord );
	}
}


VO_VOID vo_http_session::UpdateCookieImmediately()
{
	vo_http_stream * ptr_stream = ( vo_http_stream * )m_handle.download_handle;
	if( !ptr_stream )
	{
		VOLOGI("download stream pointer is null" );
		return;
	}

	//update cookie info if necessary
	if( ptr_stream->Is_cookie_update() )
	{
		g_http_session_info.update_cookie_info( ptr_stream->get_cooki_node() );
	}

	VO_CHAR * ptr_url = ptr_stream->get_url();
	VO_BOOL ret = VO_FALSE;

	ret = g_http_session_info.generate_cookie_str( ptr_url );
	if( ret )
	{
		VOLOGI("cookies:%s" , g_http_session_info.get_cookie() );
		ptr_stream->set_cookies( g_http_session_info.get_cookie() );
	}
}


VO_VOID vo_http_session::QueryDNSRecord( VO_PBYTE ptrhost )
{
	vo_http_stream * ptr_stream = ( vo_http_stream * )m_handle.download_handle;
	if( !ptr_stream )
	{
		VOLOGI("download stream pointer is null" );
		return;
	}

	ptr_stream->setCachedDnsIP( g_http_session_info.QueryCachedIpbyhost( (VO_CHAR*)ptrhost ) );
}

VO_VOID vo_http_session::PostOpenprocess()
{
	vo_http_stream * ptr_stream = ( vo_http_stream * )m_handle.download_handle;
	if( !ptr_stream )
	{
		VOLOGE("download stream pointer is null" );
		return;
	}

	//if the redirect url is Permanently moved
	if( ptr_stream->Is_cache_redirect_url() )
	{
		g_http_session_info.add_redirecturl( ptr_stream->get_url() , ptr_stream->get_redirecturl() );
	}

	//update authorization info if necessary
	if( ptr_stream->Is_auth() )
	{
		g_http_session_info.add_authinfo( ptr_stream->get_url() , ptr_stream->get_authinfo() );
	}
}

VO_U32 vo_http_session::Open( VO_BOOL bIsAsyncOpen )
{
	vo_http_stream * ptr_stream = ( vo_http_stream * )m_handle.download_handle;
		
	if( !ptr_stream )
	{
		return VO_SOURCE2_IO_NULLPOINTOR;
	}

	vo_stream * ptr_buffer_stream = ( vo_stream * )m_handle.buffer_handle;

	if( !((vo_mem_stream*)ptr_buffer_stream)->open() )
	{
		return VO_SOURCE2_IO_FAIL;
	}

	VO_CHAR * ptr_url = ptr_stream->get_url();
	IONotify( VO_SOURCE2_IO_HTTP_STARTDOWNLOAD , ptr_url , 0 );

	m_open_start_time = voOS_GetSysTime();

	m_IOFileOpenStatus = IOFILEOPENING;
	if( ptr_stream->open( bIsAsyncOpen ) )
	{
		return VO_SOURCE2_IO_OK;
	}
	else
	{
		return VO_SOURCE2_IO_FAIL;
	}
}

VO_S64 vo_http_session::UpdateDownloadSetting( VO_CHAR* ptr_url )
{
	vo_http_stream * ptr_stream = ( vo_http_stream * )m_handle.download_handle;
	if( !ptr_stream )
	{
		VOLOGE("download stream pointer is null" );
		return 0;
	}

	VO_BOOL ret = VO_FALSE;

	//1,get cached cookie string and set it
	ret = g_http_session_info.generate_cookie_str( ptr_url );
	if( ret )
	{
		VOLOGI("cookies:%s" , g_http_session_info.get_cookie() );
		ptr_stream->set_cookies( g_http_session_info.get_cookie() );
	}

	//2,get cached authorization info and set it
	VO_CHAR str_authinfo[1024];
	memset( str_authinfo , 0 , sizeof(str_authinfo) );
	ret = g_http_session_info.get_authinfo( ptr_url , str_authinfo );
	if( ret )
	{
		ptr_stream->set_authinfo( str_authinfo );
	}

	//3,get proxy server info and set it
	if( strlen( g_http_session_info.getProxyHost() ) > 0 
		&& strlen( g_http_session_info.getProxyPort() ) > 0 )
	{
		ptr_stream->setProxyHost( g_http_session_info.getProxyHost() );
		ptr_stream->setProxyPort( g_http_session_info.getProxyPort() );
	}

	//4,get user agent info and set it
	if( strlen( g_http_session_info.getUserAgent() ) > 0 )
	{
		ptr_stream->setUserAgent( g_http_session_info.getUserAgent() );
	}

	Persist_HTTP * ptr_persist = g_http_session_info.get_persistHTTP( ptr_url );
	if( !ptr_persist )
	{
		VOLOGI( "failed to get persist socket instance" );
		return 0;
	}
	else
	{
		ptr_stream->setHandle( ptr_persist );
	}

	//we should load vossl if necessary
	if( Is_url_https( ptr_url ))
	{
		//for https link ,we should load openssl lib and set api 
		if( !setsslapi() )
		{
			return 0;
		}
	}

	return 1;
}

VO_U32 vo_http_session::Close()
{
	vo_http_stream * ptr_stream = ( vo_http_stream * )m_handle.download_handle;

	if( !ptr_stream )
	{
		return VO_SOURCE2_IO_NULLPOINTOR;
	}
	else
	{
		ptr_stream->close();
	}

	vo_stream * ptr_buffer_stream = ( vo_stream * )m_handle.buffer_handle;
	if( !ptr_buffer_stream )
	{
		return VO_SOURCE2_IO_NULLPOINTOR;
	}
	else
	{
		((vo_mem_stream*)ptr_buffer_stream)->close();
	}

	return VO_SOURCE2_IO_OK;
}

VO_U32 vo_http_session::Read( VO_VOID * ptr_buf , VO_U32 size , VO_U32 * ptr_size )
{
	vo_http_stream * ptr_stream = ( vo_http_stream * )m_handle.download_handle;

	if( !ptr_stream )
	{
		return VO_SOURCE2_IO_NULLPOINTOR;
	}

	vo_stream * ptr_buffer_stream = ( vo_stream * )m_handle.buffer_handle;
	if( !ptr_buffer_stream )
	{
		return VO_SOURCE2_IO_NULLPOINTOR;
	}
	else
	{
		VO_S32 status = GetIOFileOpenStatus();
		switch ( status )
		{
		case IOFILENOTOPEN:
			{
				return VO_SOURCE2_IO_NULLPOINTOR;
			}
			break;
		case IOFILEOPENING:
			{
				//we should make sure open is finished before any getsize operation.
				return VO_SOURCE2_IO_RETRY;
			}
			break;
		case IOFILEOPEN_SUCCESS:
			{
				return VO_SOURCE2_IO_RETRY;
			}
			break;
		case IOFILEOPEN_FAILED:
			{
				return VO_SOURCE2_IO_FAIL;
			}
			break;
		case IOFILEREAD_READY:
			{
				VO_BOOL bDownloadFinished = ptr_stream->IsDownloadFinished();
				VO_S64 readed = ((vo_mem_stream*)ptr_buffer_stream)->read( (VO_PBYTE)ptr_buf , size );
				if( readed == 0 && bDownloadFinished )
				{
					VO_BOOL bDownloadComplete= ptr_stream->IsDownloadComplete();
					if( bDownloadComplete )
					{
						VOLOGI("EOS detected");
						return VO_SOURCE2_IO_EOS;
					}
					else
					{
						VOLOGI("download not complete");
						return VO_SOURCE2_IO_FAIL;
					}
				}
				else if( readed == 0 && !bDownloadFinished )
				{
					return VO_SOURCE2_IO_RETRY;
				}
				else if( readed > 0 )
				{
					*ptr_size = (VO_U32)readed;
				}
			}
			break;
		default:
			{
				return VO_SOURCE2_IO_FAIL;
			}
		}
	}

	return VO_SOURCE2_IO_OK;
}

VO_U32 vo_http_session::Write (VO_VOID * pBuf , VO_U32 uSize , VO_U32 * pWrittenSize)
{
	return VO_SOURCE2_IO_OK;
}

VO_U32 vo_http_session::SetPos( VO_S64 pos , VO_SOURCE2_IO_POS relative_pos , VO_S64 *llActualPos )
{
	*llActualPos = pos;
	return VO_SOURCE2_IO_OK;
}

VO_U32 vo_http_session::Flush()
{
	return VO_SOURCE2_IO_OK;
}

VO_U32 vo_http_session::GetSize( VO_U64 * ptr_size )
{
	vo_http_stream * ptr_stream = ( vo_http_stream * )m_handle.download_handle;
		
	if( !ptr_stream )
	{
		return VO_SOURCE2_IO_NULLPOINTOR;
	}

	VO_S32 status = GetIOFileOpenStatus();
	switch ( status )
	{
	case IOFILENOTOPEN:
		{
			return VO_SOURCE2_IO_NULLPOINTOR;
		}
		break;
	case IOFILEOPENING:
		{
			//we should make sure open is finished before any getsize operation.
			return VO_SOURCE2_IO_RETRY;
		}
		break;
	case IOFILEOPEN_SUCCESS:
	case IOFILEREAD_READY:
		{
			VO_S64 size = ptr_stream->get_content_length();
			if( size < 0 )
			{
				return VO_SOURCE2_IO_RETRY;
			}

			*ptr_size = size;
			filesize2mem (size);
			return VO_SOURCE2_IO_OK;
		}
		break;
	case IOFILEOPEN_FAILED:
		{
			return VO_SOURCE2_IO_FAIL;
		}
		break;
	default:
		{
			return VO_SOURCE2_IO_FAIL;
		}
	}
}

VO_U32 vo_http_session::GetLastError()
{
	//both mem and download error is possible, but we just return download error
	vo_stream * ptr_buffer_stream = ( vo_stream * )m_handle.buffer_handle;
	if( !ptr_buffer_stream )
	{
		return VO_SOURCE2_IO_HTTP_LAST_ERRORNONE;
	}

	ptr_buffer_stream->get_lasterror();

	//
	vo_http_stream * ptr_stream = ( vo_http_stream * )m_handle.download_handle;
		
	if( !ptr_stream )
	{
		//return VO_SOURCE2_IO_NULLPOINTOR;
		return VO_SOURCE2_IO_HTTP_LAST_ERRORNONE;
	}

	return ptr_stream->GetLastError();
}

VO_U32 vo_http_session::GetParam( VO_U32 id , VO_PTR ptr_param )
{
	vo_http_stream * ptr_stream = ( vo_http_stream * )m_handle.download_handle;

	if( !ptr_stream )
		return VO_SOURCE2_IO_NULLPOINTOR;

	//remain for get param of buffer
	//vo_stream * ptr_buffer_stream = ( vo_stream * )m_handle.buffer_handle;

	switch( id )
	{
	case VO_SOURCE2_IO_PARAMID_HTTPDOWNLOADTIME:
		{
			VO_U64 * ptr_time = ( VO_U64 * )ptr_param;
			*ptr_time = (VO_U64)ptr_stream->get_download_time();

			return VO_SOURCE2_IO_OK;
		}
		break;
	case VO_SOURCE2_IO_PARAMID_HTTPREDIRECTURL:
		{
			VO_SOURCE2_IO_HTTP_REDIRECT_URL * ptr_redurl = ( VO_SOURCE2_IO_HTTP_REDIRECT_URL * )ptr_param;
			VO_CHAR * ptr_redirect_url = g_http_session_info.get_redirecturl(  ptr_redurl->ptr_url );
			if( ptr_redirect_url && strlen( ptr_redirect_url ) < ptr_redurl->redirect_size )
			{
				strcpy( ptr_redurl->ptr_redirect_url , ptr_redirect_url );
				ptr_redurl->redirect_size = strlen( ptr_redirect_url );
				return VO_SOURCE2_IO_OK;
			}
			else
			{
				return VO_SOURCE2_IO_FAIL;
			}
		}
		break;
	}

	return VO_SOURCE2_IO_OK;
}

VO_U32 vo_http_session::SetParam( VO_U32 id , VO_PTR ptr_param )
{
	//work path should be set before any init,so ptr_stream is not exist yet
	if( VO_PID_SOURCE2_WORKPATH == id )
	{
		VOLOGI( "ssload_path: %s " , (VO_PTCHAR)ptr_param );
		g_http_session_info.setworkpath( (VO_PTCHAR)ptr_param );
		return VO_SOURCE2_IO_OK;
	}

	vo_http_stream * ptr_stream = ( vo_http_stream * )m_handle.download_handle;

	if( !ptr_stream )
		return VO_SOURCE2_IO_NULLPOINTOR;

	//remain for set param of buffer
	//vo_stream * ptr_buffer_stream = ( vo_stream * )m_handle.buffer_handle;

	switch( id )
	{
	case VO_SOURCE2_IO_PARAMID_HTTPRANGE:
		{
			VO_SOURCE2_IO_HTTPRANGE * ptr = (VO_SOURCE2_IO_HTTPRANGE *)ptr_param;
			if( ptr )
			{
				ptr_stream->set_range( ptr->ullOffset , ptr->ullLength );
			}
		}
		break;
	case VO_SOURCE2_IO_PARAMID_HTTPMAXSPEED:
		{
			VO_U32* max_speed = ( VO_U32 *)ptr_param;
			ptr_stream->set_maxspeed( *max_speed );
		}
		break;
	case VO_SOURCE2_IO_PARAMID_DOHTTPVERIFICATION:
		{
			VO_SOURCE2_IO_VERIFICATIONINFO * ptr = (VO_SOURCE2_IO_VERIFICATIONINFO *)ptr_param;

			if( ptr )
			{
				VOLOGI( "http verify data: %s , size: %d" , ptr->pData , ptr->uDataSize );
				if( 1 == ptr->uDataFlag )
				{
					//username:password pair for authorization
					ptr_stream->set_userpass( (VO_CHAR*)ptr->pData , ptr->uDataSize );
				}
				else
				{
					//direct authorization information which will be append as http header
					g_http_session_info.add_authinfo( ptr_stream->get_url() , (VO_CHAR*)ptr->pData );
				}
			}

		}
		break;
	case VO_SOURCE2_IO_PARAMID_HTTPIOCALLBACK:
		{
			VO_SOURCE2_IO_HTTPCALLBACK * ptr = (VO_SOURCE2_IO_HTTPCALLBACK *)ptr_param;
			VOLOGI( "http io callback, handle: 0x%08x , func: 0x%08x" , ptr->hHandle , ptr->IO_Callback );
			if( ptr )
			{
				m_iocb.hHandle = ptr->hHandle;
				m_iocb.IO_Callback = ptr->IO_Callback;

				//set pointer for inner usage
				ptr_stream->set_IO_Callback( ptr );
			}
		}
		break;
	}

	return VO_SOURCE2_IO_OK;
}

VO_S32 vo_http_session::IONotify( VO_U32 uID ,  VO_PTR pParam1, VO_PTR pParam2 )
{
	if( m_iocb.IO_Callback )
	{
		return (VO_S32)m_iocb.IO_Callback( m_iocb.hHandle , uID , pParam1 , pParam2 );
	}
	else
	{
		VOLOGE("IO CallBack function pointer is not set yet.");
		return 0;
	}
}

VO_U32	vo_http_session::GetSource(VO_PTR* pSource)
{
	return VO_SOURCE2_IO_OK;
}

VO_U32	vo_http_session::GetCurPos(VO_U64* llPos)
{
	vo_stream * ptr_buffer_stream = ( vo_stream * )m_handle.buffer_handle;
	if( !ptr_buffer_stream )
	{
		return VO_SOURCE2_IO_NULLPOINTOR;
	}
	else
	{
		*llPos = ((vo_mem_stream*)ptr_buffer_stream)->GetCurPos();
		return VO_SOURCE2_IO_OK;
	}
}


VO_S64 vo_http_session::http_stream_callback(VO_HANDLE phandle, VO_PBYTE buffer, VO_U64 size , IO_HTTPDATA2BUFFER_DATA_TYPE type)
{
	vo_http_session* pobj = ( vo_http_session* )phandle;

	switch( type )
	{
	case VO_CONTENT_DATA:
		{
			VO_S64 ret = pobj->write_data( buffer , size );
			return ret;
		}
		break;
	case VO_CONTENT_LEN:
		{
			pobj->filesize2mem(size);
		}
		break;
	case VO_CONTENT_DOWNLOAD_SPEED:
		{
			pobj->control_downloadspeed( (VO_S32)size );
		}
		break;
	case VO_CONTENT_MEMINFO_PLUS:
		{
			//we need to update the meminfo of all sourceio instance.
			g_http_session_info.m_module_totalmem += size;
			VOLOGI( "module_mem of sourceio:%lld KB" , g_http_session_info.m_module_totalmem );
		}
		break;
	case VO_CONTENT_MEMINFO_MINUS:
		{
			//we need to update the meminfo of all sourceio instance.
			g_http_session_info.m_module_totalmem -= size;
			VOLOGI( "module_mem of sourceio:%lld KB" ,g_http_session_info.m_module_totalmem );
		}
		break;
	case VO_CONTENT_OPEN_SUCCESS:
		{
			VOLOGI("urlopen cost %d ms" , voOS_GetSysTime() - pobj->GetStartopenTime() );
			pobj->SetIOFileOpenStatus( IOFILEOPEN_SUCCESS );
			//3,post process after success open
			pobj->PostOpenprocess();
		}
		break;
	case VO_CONTENT_OPEN_FAILED:
		{
			pobj->SetIOFileOpenStatus( IOFILEOPEN_FAILED );
			VOLOGI("urlopen failed and cost %d ms" , voOS_GetSysTime() - pobj->GetStartopenTime() );
		}
		break;
	case VO_CONTENT_READAVAILABLE:
		{
			VOLOGI("data ready for read, cost %d ms" , voOS_GetSysTime() - pobj->GetStartopenTime() );
			pobj->SetIOFileOpenStatus( IOFILEREAD_READY );
		}
		break;
	case VO_CONTENT_COOKIE_UPDATED:
		{
			pobj->UpdateCookieImmediately();
		}
		break;
	case VO_CONTENT_DNSINFO_UPDATED:
		{
#ifdef ENABLE_ASYNCDNS
			pobj->UpdateDNSCache();
#endif
		}
		break;
	case VO_CONTENT_DNSINFO_QUERY:
		{
#ifdef ENABLE_ASYNCDNS
			pobj->QueryDNSRecord(buffer);
#endif
		}
		break;
	case VO_CONTENT_DOWNLOADSETTING_UPDATED:
		{
			return pobj->UpdateDownloadSetting( (VO_CHAR*)buffer );
		}
		break;
	default:
		break;
	}

	return 0;
}

VO_VOID vo_http_session::control_downloadspeed( VO_S32 speed )
{
	vo_http_stream * ptr_stream = ( vo_http_stream * )m_handle.download_handle;
	if( ptr_stream )
	{
		ptr_stream->control_speed( speed );
	}
}

//tell memory manager the size of file.
VO_VOID	vo_http_session::filesize2mem( VO_U64 size )
{ 
	vo_stream * ptr_buffer_stream = ( vo_stream * )m_handle.buffer_handle;
	if( !ptr_buffer_stream )
	{
		return;
	}
	else
	{
		return ((vo_mem_stream*)ptr_buffer_stream)->set_filesize( size );
	}
}

VO_S64 vo_http_session::write_data( VO_PBYTE buffer, VO_U64 size )
{
	vo_stream * ptr_buffer_stream = ( vo_stream * )m_handle.buffer_handle;
	if( !ptr_buffer_stream )
	{
		return -1;
	}
	else
	{
		if( m_append_datacnt++ > 100 )
		{
			m_append_datacnt = 0;
			((vo_mem_stream*)ptr_buffer_stream)->print_memInfo();
				
			vo_http_stream * ptr_stream = ( vo_http_stream * )m_handle.download_handle;
			if( ptr_stream )
			{
				ptr_stream->print_downloadInfo();
			}
		}

		VO_S64 ret = ((vo_mem_stream*)ptr_buffer_stream)->append( buffer , size );
		return ret;
	}
}

VO_BOOL vo_http_session::setsslapi()
{
	vo_http_stream * ptr_stream = ( vo_http_stream * )m_handle.download_handle;
	if( !ptr_stream )
	{
		VOLOGI("download stream pointer is null" );
		return VO_FALSE;
	}

#ifndef _IOS
	if( g_http_session_info.getworkpath() )
	{
		g_http_session_info.m_sslload.SetWorkpath( g_http_session_info.getworkpath() );
	}
	else
	{
		VOLOGE( "ssl work path not set" );
		return VO_FALSE;
	}
#endif		
	if( !g_http_session_info.m_sslload.LoadSSL() )
	{
		VOLOGE( "load ssl lib Failed" );
		return VO_FALSE;
	}
	
	ptr_stream->setsslapi( g_http_session_info.m_sslload.m_sslapi);
	return VO_TRUE;
}