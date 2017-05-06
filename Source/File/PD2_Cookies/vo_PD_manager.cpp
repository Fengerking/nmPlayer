#include "vo_PD_manager.h"
#include "voOSFunc.h"
#include "voThread.h"
#include "voLog.h"
#include "fileformat_checker.h"
#include "vo_playlist_pls_parser.h"
#include "vo_playlist_m3u_parser.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


#define GUESSFILETYPE_BUFFERSIZE (64 * 1024)
#define DEFAULTBUFFERINGSIZE     (100 * 1024)
#define DEFAULT_CONNECT_RETRYTIMES	2

typedef VO_S32 (VO_API *pvoGetReadAPI)(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag);

#ifdef _IOS
	#define g_dwFRModuleID		VO_INDEX_SRC_HTTP
#else
	unsigned long g_dwFRModuleID = VO_INDEX_SRC_HTTP;
#endif


VO_U32 LoadParserLib(int nParserType, void** ppParserApi)
{
	return 0;
}

static VO_PTR VO_API stream_open(VO_FILE_SOURCE * pSource)
{
	vo_PD_manager * ptr_manager = ( vo_PD_manager * )pSource->nReserve;
	return ptr_manager->get_stream();
}

static VO_S32 VO_API stream_read(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)
{
	vo_buffer_stream * ptr_buffer = (vo_buffer_stream*)pFile;
	return ptr_buffer->read( (VO_PBYTE)pBuffer , uSize );
}

static VO_S32 VO_API stream_write(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)
{
	return -1;
}

static VO_S32 VO_API stream_flush(VO_PTR pFile)
{
	return -1;
}

static VO_S64 VO_API stream_seek(VO_PTR pFile, VO_S64 nPos, VO_FILE_POS uFlag)
{
	vo_buffer_stream * ptr_buffer = (vo_buffer_stream*)pFile;
	return ptr_buffer->seek( nPos , uFlag );
}

static VO_S64 VO_API stream_size(VO_PTR pFile)
{
	vo_buffer_stream * ptr_buffer = (vo_buffer_stream*)pFile;
	return ptr_buffer->get_filesize();
}

static VO_S64 VO_API stream_save(VO_PTR pFile)
{
	return -1;
}

static VO_S32 VO_API stream_close(VO_PTR pFile)
{
	vo_buffer_stream * ptr_buffer = (vo_buffer_stream*)pFile;
	ptr_buffer->seek( 0 , VO_FILE_BEGIN );
	return 0;
}


vo_PD_manager::vo_PD_manager(void)
:m_buffer_stream(0)
,m_fileparser(0)
,m_is_init( VO_FALSE )
,m_is_to_exit( VO_FALSE )
,m_bitrate(0)
,m_duration(0)
,m_firstseektrack(-1)
,m_is_pause( VO_FALSE )
,m_streamtype(StreamType_Normal)
,m_is_networkerror(VO_FALSE)
,m_tracks(2)
,m_ptr_sample(0)
,m_ptr_bottom_sample(0)
,m_ptr_buffer(0)
,m_ptr_bottom_buffer(0)
,m_getsample_ret(0)
,m_bsourcend(0)
,m_ptr_timestamp(0)
,m_max_sampsize(0)
,m_getsample_flag(0)
,m_bdrop_frame(0)
,m_b_stop_getsample(VO_FALSE)
,m_getsamplelock(0)
,m_sem_getsample(0)
,m_seek_threadhandle(0)
,m_startbuffering_handle(0)
,m_seekpos( -1 )
,m_videotrack(-1)
,m_is_seeking(VO_FALSE)
,m_is_stopseek(VO_FALSE)
,m_ptrpool(0)
,m_poolsize(0)
,m_seekmode(TIME_SeekModeFast)
,m_cur_downl_percent(0)
,m_is_buffering_on_start(VO_TRUE)
,m_is_shoutcast(VO_FALSE)
,m_ptr_playlist_parser(0)
,m_is_stopbuffering_thread(VO_FALSE)
,m_buffering_percent(0)
,m_is_buffering(VO_FALSE)
,m_filesize_byserver(-1)
,m_is_source2(VO_FALSE)
,m_retry_download_pos(-1)
,m_pstrWorkPath(0)
,m_connectretry_times(DEFAULT_CONNECT_RETRYTIMES)
,m_last_download_retry_time(0)
{
	m_opFile.Close		=	stream_close;
	m_opFile.Flush		=	stream_flush;
	m_opFile.Open		=	stream_open;
	m_opFile.Read		=	stream_read;
	m_opFile.Save		=	stream_save;
	m_opFile.Seek		=	stream_seek;
	m_opFile.Size		=	stream_size;
	m_opFile.Write		=	stream_write;

	memset( &m_parser_api , 0 , sizeof( m_parser_api ) );
	memset( &m_url , 0 , sizeof( m_url ) );
	memset( &m_source_param , 0 , sizeof(VO_SOURCE_OPENPARAM) );
	memset( &m_PD_param , 0 , sizeof( m_PD_param ) );
	memset( &m_dwango_cookies , 0 , sizeof( m_dwango_cookies ) );
	m_http_verifycb.HTTP_Callback = 0;
	m_http_verifycb.hHandle = 0;
	m_http_eventcb.pUserData = 0;
	m_http_eventcb.SendEvent = 0;
}

vo_PD_manager::~vo_PD_manager(void)
{
	close();
}

CALLBACK_RET vo_PD_manager::received_data( VO_S64 physical_pos , VO_PBYTE ptr_buffer , VO_S32 size )
{
	VO_S64 writesize = m_buffer.write( physical_pos , ptr_buffer , size );

	if( writesize == size )
	{
		if( -1 != m_buffer.get_filesize() )
		{
			VO_S32 new_downl_pos = (VO_S32)(( physical_pos + size ) * 100 /m_buffer.get_filesize());
			//VOLOGI( "VO_PD_EVENT_DOWNLOADPOS : physical_pos %lld , size %d , filesize %lld" , physical_pos , size , m_buffer.get_filesize() );

			if( new_downl_pos != m_cur_downl_percent )
			{
				m_cur_downl_percent = new_downl_pos;

				//we should try to notify timestamp based percentage info , it is more reasonable
				if( m_duration && new_downl_pos != 100)
				{
					VO_FILE_MEDIATIMEFILEPOS pos;
					pos.nFlag = 1;
					pos.llFilePos = physical_pos + size;

					VO_U32 ret = m_parser_api.GetSourceParam( m_fileparser , VO_PID_FILE_FILEPOS2MEDIATIME , &pos );

					if( ret == 0 )
					{
						VO_S32 timepos = (VO_S32)((float)( pos.llMediaTime ) * 100. / (float)m_duration);
						notify( VO_PD_EVENT_DOWNLOADPOS , &timepos );
						VOLOGI( "VO_PD_EVENT_DOWNLOADPOS ontimestamp %ld" , timepos);
					}
					else
					{
						notify( VO_PD_EVENT_DOWNLOADPOS , &new_downl_pos );
						VOLOGI("VO_PD_EVENT_DOWNLOADPOS %ld" , m_cur_downl_percent );
					}
				}
				else
				{
					notify( VO_PD_EVENT_DOWNLOADPOS , &new_downl_pos );
					VOLOGI("VO_PD_EVENT_DOWNLOADPOS %ld" , m_cur_downl_percent );
				}


				//add for print download speed data
				if(m_downloader.is_downloading())
				{
					VO_DOWNLOAD_INFO info;
					m_downloader.get_dowloadinfo(&info);
				}
			}
		}
		return CALLBACK_OK;
	}
	else
	{
		return CALLBACK_BUFFER_FULL;
	}
}

CALLBACK_RET vo_PD_manager::download_notify( DOWNLOAD_CALLBACK_NOTIFY_ID id , VO_PTR ptr_data )
{
	switch( id )
	{
	case DOWNLOAD_END:
		{
			VOLOGI( "http_download end" );
		}
		break;
	case DOWNLOAD_START:
		{
			VOLOGI( "http_download start" );
			if( m_is_networkerror )
			{
				VOLOGI( "+VO_EVENT_BUFFERING_END");
				long temp = 100;
				notify( VO_EVENT_BUFFERING_END , &temp );
				VOLOGI( "-VO_EVENT_BUFFERING_END");
			}

			m_is_networkerror = VO_FALSE;
			m_retry_download_pos = -1;
		}
		break;
	case DOWNLOAD_FILESIZE:
		{
			VO_S64 * ptr_filesize = ( VO_S64 * )ptr_data;
						
			if( *ptr_filesize > 0 && *ptr_filesize <= 4096 )
			{
				//the size maybe value of shoutcast playlist,and we should not create small memory mode in this case.
				m_buffer.set_filesize( -1 );
			}
			else
			{
				m_buffer.set_filesize( *ptr_filesize );
			}
			
			m_filesize_byserver = *ptr_filesize;

			//for small file size case ,we use nonestop download mode
			if( (*ptr_filesize) < m_PD_param.nMaxBuffer * 1024 
				&& (*ptr_filesize) > 4096 )
			{
				VOLOGE( "StreamType_NoneStop for small file size case" );
				m_streamtype = StreamType_NoneStop;
			}
		}
		break;
	case DOWNLOAD_TO_FILEEND:
		{
			VO_S64 * ptr_filesize = ( VO_S64 * )ptr_data;
			VOLOGI( "the file size got in trunk or endless mode :%lld" ,*ptr_filesize );
			m_buffer.set_filesize( *ptr_filesize );
		}
		break;
	case DOWNLOAD_ERROR:
		{
			VOLOGE( "Download Fatal Error!" );
			m_is_networkerror = VO_TRUE;
			if(m_downloader.get_net_error_type() == E_PD_DOWNLOAD_FAILED)
			{
				long err_type = m_is_init ? E_PD_SERVER_DCON_ON_PREPARE : E_PD_SERVER_DCON_ON_PLAY;
				notify( VO_PD_EVENT_NOT_APPLICABLE_MEDIA ,&err_type);
							
				m_retry_download_pos = (*( VO_S64 * )ptr_data);
				VOLOGI( "retry download pos: %lld " , m_retry_download_pos );
			}
			else
			{
				long err_type = m_downloader.get_net_error_type();
				notify( VO_PD_EVENT_NOT_APPLICABLE_MEDIA ,&err_type);
			}
		}
		break;
	case DOWNLOAD_SERVER_RESPONSEERROR:
		{
			VOLOGE("server response error" );
			long err_code = 0;
			notify( VO_PD_EVENT_SERVER_WRONGRESPONSE , &err_code );
		}
		break;
	case DOWNLOAD_NEED_PROXY:
		{
			long id = E_PROXY_AUTH_REQUIRED;
			notify( VO_EVENT_OPTION_FAIL , &id );
		}
		break;
	case DOWNLOAD_USE_PROXY:
		{
			long id = E_PROXY_NEEDED;
			notify( VO_EVENT_OPTION_FAIL , &id );
		}
		break;
	case DOWNLOAD_SERVER_RESPONSE:
		{
			VO_CHAR * response = ( VO_CHAR * )ptr_data;
			VO_SOURCE2_VERIFICATIONINFO *ptr = new VO_SOURCE2_VERIFICATIONINFO;
			if( !ptr )
			{
				VOLOGE( "new obj failed" );
				return CALLBACK_OK;
			}
			memset(ptr , 0, sizeof(VO_SOURCE2_VERIFICATIONINFO) );
			ptr->szResponse = response;
			ptr->uResponseSize = strlen( response );

			http_verify_callback( VO_SOURCE2_IO_HTTP_RESPONSERECVED , (void*)ptr);

			delete ptr;
		}
		break;
	case DOWNLOAD_SHOUTCAST_METADATA:
		{
			VO_CHAR * metadata = ( VO_CHAR * )ptr_data;
			VO_SOURCE2_CUSTOMERTAG_TIMEDTAG *ptr = new VO_SOURCE2_CUSTOMERTAG_TIMEDTAG;
			if( !ptr )
			{
				VOLOGE( "new obj failed" );
				return CALLBACK_OK;
			}
			ptr->ullTimeStamp = 0;
			ptr->uSize = strlen(metadata);
			ptr->pData = (VO_VOID*)metadata;
			ptr->uFlag = VO_SOURCE2_CUSTOMERTAG_TIMEDTAG_FLAG_STRINGTAG;
			ptr->pReserved = 0;

			//event callback
			http_event_callback( VO_EVENTID_SOURCE2_CUSTOMTAG , VO_SOURCE2_CUSTOMERTAGID_TIMEDTAG , (VO_U32)ptr );
			VOLOGE( "shoutcast: %s" , (VO_CHAR*)ptr->pData );

			delete ptr;
		}
		break;
	default:
		{
			;
		}
		break;
	}
	return CALLBACK_OK;
}

BUFFER_CALLBACK_RET vo_PD_manager::buffer_notify( BUFFER_CALLBACK_NOTIFY_ID notify_id , VO_PTR ptr_data )
{
	switch( notify_id )
	{
	case BUFFER_FULL:
		break;
	case BUFFER_GET_CACHE:
		{
			VO_BUFFER_NEED * ptr_getcache = ( VO_BUFFER_NEED* )ptr_data;
			VOLOGE( "cache buffer need download  pos: %lld   size: %lld" , ptr_getcache->want_physical_pos , ptr_getcache->want_size );
			m_downloader.start_download( ptr_getcache->want_physical_pos , ptr_getcache->want_size , VO_FALSE );
		}
		break;
	case BUFFER_LOW:
		{
			VO_BUFFER_NEED * ptr_need = (VO_BUFFER_NEED *)ptr_data;
						
			//only when not in init mode, then we will resume the download speed
			if( !m_is_init )
			{
				m_downloader.slow_down( 0 );
			}

			VOLOGI("Buffer low! download start: %lld" , ptr_need->want_physical_pos );

			if( !m_downloader.is_downloading() && !m_is_init)
			{
				m_downloader.start_download_inthread( ptr_need->want_physical_pos );
			}
		}
		break;
	case BUFFER_LACK:
		{
			if( m_is_networkerror && m_connectretry_times == DEFAULT_CONNECT_RETRYTIMES )
			{
				notify( VO_EVENT_SOCKET_ERR , (long *)ptr_data );
				return BUFFER_CALLBACK_ERROR;
			}
	
			if( m_is_networkerror )
			{
				VOLOGI( "+VO_EVENT_BUFFERING_BEGIN");
				long temp = 0;
				notify( VO_EVENT_BUFFERING_BEGIN , &temp );
				VOLOGI( "-VO_EVENT_BUFFERING_BEGIN");
			}

			VOLOGI("Buffer Lack!" );

			//only when not in init mode, then we will resume the download speed
			if( !m_is_init )
			{
				m_downloader.slow_down( 0 );
			}

			VO_BUFFER_NEED * ptr_need = (VO_BUFFER_NEED *)ptr_data;

			BUFFER_CALLBACK_RET ret = buffer_notify_bufferlack( ptr_need );

			if( ret == BUFFER_CALLBACK_ERROR && m_connectretry_times == DEFAULT_CONNECT_RETRYTIMES )
			{
				notify( VO_EVENT_SOCKET_ERR , (long *)ptr_data );
			}

			return ret;
		}
		break;
	case BUFFER_HEADER_CONTROL:
		{
			long * slowdown = (long *)ptr_data;
			VOLOGI( "Buffer_Control! :%ld " , *slowdown);
			if( m_dlna_protocolinfo.is_dlna() )
			{
				m_downloader.slow_down(*slowdown);
			}
		}
		break;
	case BUFFER_HIGH:
		{
			long * slowdown = (long *)ptr_data;
			VOLOGI( "Buffer_High! :%ld " , *slowdown);

			//we apply it only in dlna case and connection stalling on play is true
			if( m_dlna_protocolinfo.is_dlna() && m_dlna_protocolinfo.is_connection_stalling_on_play() )
			{
				m_downloader.slow_down(*slowdown);
			}
		}
		break;
	}

	return BUFFER_CALLBACK_OK;
}

BUFFER_CALLBACK_RET vo_PD_manager::buffer_notify_bufferlack( VO_BUFFER_NEED * ptr_need )
{
	if( m_dlna_protocolinfo.is_dlna() )
	{
		return dlna_buffer_notify_bufferlack( ptr_need );
	}
	else
	{
		return normal_buffer_notify_bufferlack( ptr_need );
	}
}

BUFFER_CALLBACK_RET vo_PD_manager::dlna_buffer_notify_bufferlack( VO_BUFFER_NEED * ptr_need )
{
	if( m_downloader.is_downloading() )
	{ 
		VO_DOWNLOAD_INFO info;
		m_downloader.get_dowloadinfo( &info );

		BUFFER_INFO buf_info;
		m_buffer.get_buffer_info( &buf_info );

		if( m_downloader.is_support_seek() )
		{
			VOLOGI( "Buffer lack: lack_pos %lld lack_size %lld ; Current Downloading: start_pos %lld current_pos %lld buf_start %lld" , 
				ptr_need->want_physical_pos , ptr_need->want_size , info.start_download_pos , info.cur_download_pos , buf_info.physical_start );

			if( info.start_download_pos <= ptr_need->want_physical_pos && ( info.is_update_buffer ? ( buf_info.physical_start <= ptr_need->want_physical_pos ) : VO_TRUE )  && ( info.download_size == DOWNLOADTOEND || info.start_download_pos + info.download_size >= ptr_need->want_physical_pos ) )
			{
				VO_S64 delta = ptr_need->want_physical_pos + ptr_need->want_size - info.cur_download_pos;

				if( m_is_init )
				{
					VOLOGI("m_is_init");
					if( delta >= 1024 * 2048/* || delta <= -1024 * 2048*/ && !m_dlna_protocolinfo.is_connection_stalling_on_play() )
					{
						m_downloader.stop_download();
						if( !m_downloader.start_download( ptr_need->want_physical_pos ) )
							return BUFFER_CALLBACK_ERROR;
					}
				}
				else
				{
					if( delta >= 1024 * 500 /* || delta <= -1024 * 500*/ && !m_dlna_protocolinfo.is_connection_stalling_on_play() )
					{
						m_downloader.stop_download();
						if( !m_downloader.start_download_inthread( ptr_need->want_physical_pos ) )
							return BUFFER_CALLBACK_ERROR;
					}
					else
					{
						m_downloader.get_dowloadinfo( &info );

						VO_S64 buffer_end_pos;

						if( m_bitrate > 0 )
						{
							buffer_end_pos = (VO_S64)(ptr_need->want_physical_pos + m_bitrate / 8. * m_PD_param.nBufferTime);
						}
						else
						{
							buffer_end_pos = ptr_need->want_physical_pos + DEFAULTBUFFERINGSIZE;
						}

						if( m_filesize_byserver > 0 )
						{
							buffer_end_pos = ( buffer_end_pos > m_filesize_byserver ) ? m_filesize_byserver : buffer_end_pos;
						}

						VOLOGI( "Try to Buffer from %lld to %lld , Current download pos: %lld" , ptr_need->want_physical_pos , buffer_end_pos , info.cur_download_pos );

						VO_S64 buffer_start_org_pos = info.cur_download_pos;

						VOLOGI( "+VO_EVENT_BUFFERING_BEGIN");
						long temp = 0;
						notify( VO_EVENT_BUFFERING_BEGIN , &temp );
						VOLOGI( "-VO_EVENT_BUFFERING_BEGIN");

						while( m_downloader.is_downloading() && info.cur_download_pos < buffer_end_pos && !m_is_pause )
						{
							VO_S32 sleeptime;

							VO_S64 distance = buffer_end_pos - info.cur_download_pos;

							if( buffer_end_pos - buffer_start_org_pos )
							{
								int percent = (int)((float)( info.cur_download_pos - buffer_start_org_pos ) / (float)( buffer_end_pos - buffer_start_org_pos ) * 100. );
								VOLOGI( "+VO_EVENT_BUFFERING_PERCENT : %d" , percent);
								notify( VO_EVENT_BUFFERING_PERCENT , (long*)&percent );
								VOLOGI( "-VO_EVENT_BUFFERING_PERCENT : %d" , percent);
							}

							if( info.is_speed_reliable )
							{
								sleeptime = (VO_S32)(distance / (float)info.average_speed * 1000.);
								VOLOGI( "Buffering............. Need Buffer Time: %d    Download Speed: %d KBps Download pos: %lld" , sleeptime , (VO_S32)(info.average_speed/1024.) , info.cur_download_pos );
							}
							else
							{
								if( m_bitrate > 0 )
									sleeptime = (VO_S32)(distance / (m_bitrate / 8.) / 2. * 1000.);
								else
									sleeptime = 1000;
								VOLOGI( "Buffering............. Need Buffer Time: %d Download pos: %lld" , sleeptime , info.cur_download_pos );
							}

							voOS_Sleep( 50 );
							m_downloader.get_dowloadinfo( &info );
						}

						VOLOGI( "+VO_EVENT_BUFFERING_END");
						temp = 100;
						notify( VO_EVENT_BUFFERING_END , &temp );
						VOLOGI( "-VO_EVENT_BUFFERING_END");
					}
				}
			}
			else
			{
				VOLOGI( "Buffer lack: lack_pos %lld lack_size %lld" , ptr_need->want_physical_pos , ptr_need->want_size );
				m_downloader.stop_download();
				if( m_is_init )
				{
					if( !m_downloader.start_download( ptr_need->want_physical_pos ) )
						return BUFFER_CALLBACK_ERROR;
				}
				else
				{
					if( !m_downloader.start_download_inthread( ptr_need->want_physical_pos ) )
						return BUFFER_CALLBACK_ERROR;
				}
			}
		}
		else	//do not support seek
		{
			VOLOGI( "Do not support seek! Current Download Pos: %lld" , info.start_download_pos );
			if( info.start_download_pos <= ptr_need->want_physical_pos )
			{
				//we should pause the player for a while
				long temp = 0;
				VOLOGI( "+VO_EVENT_BUFFERING_BEGIN");
				notify( VO_EVENT_BUFFERING_BEGIN , &temp );
				VOLOGI( "-VO_EVENT_BUFFERING_BEGIN");

				VO_BOOL bdownload_finish = VO_FALSE;

				//in a waiting loop for data download completed.
				do
				{
					voOS_Sleep( 50 );

					//get the latest download info
					m_downloader.get_dowloadinfo( &info );

					//check if the download data has meet the data request or not
					VO_S64 delta = (ptr_need->want_physical_pos + ptr_need->want_size) - info.cur_download_pos;

					if( delta > 0)
					{
						int percent = (int)((float)( info.cur_download_pos - info.start_download_pos ) / (float)( (ptr_need->want_physical_pos + ptr_need->want_size) - info.start_download_pos ) * 100. );
						VOLOGI( "+VO_EVENT_BUFFERING_PERCENT");
						notify( VO_EVENT_BUFFERING_PERCENT , (long*)&percent );
						VOLOGI( "-VO_EVENT_BUFFERING_PERCENT");
					}
					else
					{
						bdownload_finish = VO_TRUE;
					}

				}while( m_downloader.is_downloading() && !bdownload_finish   && !m_is_pause);

				//the download process should be finished
				VOLOGI( "+VO_EVENT_BUFFERING_END");
				temp = 100;
				notify( VO_EVENT_BUFFERING_END , &temp );
				VOLOGI( "-VO_EVENT_BUFFERING_END");
			}
			else
			{
				m_downloader.stop_download();
				if( m_is_init )
				{
					if( !m_downloader.start_download( ptr_need->want_physical_pos ) )
						return BUFFER_CALLBACK_ERROR;
				}
				else
				{
					if( !m_downloader.start_download_inthread( ptr_need->want_physical_pos ) )
						return BUFFER_CALLBACK_ERROR;
				}
			}
		}
	}
	else
	{
		VOLOGI( "Buffer lack: lack_pos %lld lack_size %lld" , ptr_need->want_physical_pos , ptr_need->want_size );
		if( m_is_init )
		{
			if( !m_downloader.start_download( ptr_need->want_physical_pos ) )
				return BUFFER_CALLBACK_ERROR;
		}
		else
		{
			if( !m_downloader.start_download_inthread( ptr_need->want_physical_pos ) )
				return BUFFER_CALLBACK_ERROR;
		}

		voOS_Sleep( 100 );
	}

	if( m_is_to_exit || m_is_pause )
	{
		VOLOGI( "BUFFER_CALLBACK_SHOULD_STOP");
		return BUFFER_CALLBACK_SHOULD_STOP;
	}

	return BUFFER_CALLBACK_OK;
}

BUFFER_CALLBACK_RET vo_PD_manager::normal_buffer_notify_bufferlack( VO_BUFFER_NEED * ptr_need )
{
	if( m_downloader.is_downloading() )
	{ 
		VO_DOWNLOAD_INFO info;
		m_downloader.get_dowloadinfo( &info );

		BUFFER_INFO buf_info;
		m_buffer.get_buffer_info( &buf_info );

		if( m_downloader.is_support_seek() )
		{
			VOLOGI( "Buffer lack: lack_pos %lld lack_size %lld ; Current Downloading: start_pos %lld current_pos %lld  buf_start %lld" , 
				ptr_need->want_physical_pos , ptr_need->want_size , info.start_download_pos , info.cur_download_pos  , buf_info.physical_start );

			if( info.start_download_pos <= ptr_need->want_physical_pos && ( info.is_update_buffer ? ( buf_info.physical_start <= ptr_need->want_physical_pos ) : VO_TRUE )  && ( info.download_size == DOWNLOADTOEND || info.start_download_pos + info.download_size >= ptr_need->want_physical_pos ) )
			{
				VO_S64 delta = ptr_need->want_physical_pos + ptr_need->want_size - info.cur_download_pos;

				if( m_is_init )
				{
					VOLOGI( "m_is_init");
					if( delta >= 1024 * 2048 || delta <= -1024 * 2048 )
					{
						m_downloader.stop_download();
						if( !m_downloader.start_download( ptr_need->want_physical_pos ) )
							return BUFFER_CALLBACK_ERROR;
					}
				}
				else
				{
					//VOLOGI(  "Status Info: Delta %lld , Tracks %d" , delta , m_tracks );
					if( delta >= 1024 * 2048 /* || delta <= -1024 * 500*/ )
					{
						m_downloader.stop_download();
						if( !m_downloader.start_download_inthread( ptr_need->want_physical_pos ) )
							return BUFFER_CALLBACK_ERROR;
					}
					else
					{
						m_downloader.get_dowloadinfo( &info );

						VO_S64 buffer_end_pos;
						
						if( m_bitrate > 0 )
						{
							buffer_end_pos = (VO_S64)(ptr_need->want_physical_pos + m_bitrate / 8. * m_PD_param.nBufferTime);
						}
						else
						{
							buffer_end_pos = ptr_need->want_physical_pos + DEFAULTBUFFERINGSIZE;
						}

						if( m_filesize_byserver > 0 )
						{
							buffer_end_pos = ( buffer_end_pos > m_filesize_byserver ) ? m_filesize_byserver : buffer_end_pos;
						}
						

						VOLOGI( "Try to Buffer from %lld to %lld , Current download pos: %lld" , ptr_need->want_physical_pos , buffer_end_pos , info.cur_download_pos );

						VO_S64 buffer_start_org_pos = info.cur_download_pos;
						
						VOLOGI("+VO_EVENT_BUFFERING_BEGIN");
						long temp = 0;
						notify( VO_EVENT_BUFFERING_BEGIN , &temp );
						m_is_buffering = VO_TRUE;
						VOLOGI("-VO_EVENT_BUFFERING_BEGIN");
						

						while( m_downloader.is_downloading() && info.cur_download_pos < buffer_end_pos && !m_is_pause )
						{
							VO_S32 sleeptime;

							VO_S64 distance = buffer_end_pos - info.cur_download_pos;

							if( buffer_end_pos - buffer_start_org_pos )
							{
								int percent = (int)((float)( info.cur_download_pos - buffer_start_org_pos ) / (float)( buffer_end_pos - buffer_start_org_pos ) * 100. );
								VOLOGI("+VO_EVENT_BUFFERING_PERCENT : %d" , percent);
								notify( VO_EVENT_BUFFERING_PERCENT , (long*)&percent );
								VOLOGI("-VO_EVENT_BUFFERING_PERCENT : %d" , percent);
							}

							if( info.is_speed_reliable )
							{
								sleeptime = (VO_S32)(distance / (float)info.average_speed * 1000.);
								VOLOGI( "Buffering............. Need Buffer Time: %d    Download Speed: %d KBps Download pos: %lld" , sleeptime , (VO_S32)(info.average_speed/1024.) , info.cur_download_pos );
							}
							else
							{
								if( m_bitrate > 0 )
									sleeptime = (VO_S32)(distance / (m_bitrate / 8.) / 2. * 1000.);
								else
									sleeptime = 1000;
								VOLOGI( "Buffering............. Need Buffer Time: %d Download pos: %lld" , sleeptime , info.cur_download_pos );
							}

							voOS_Sleep( 50 );

							m_downloader.get_dowloadinfo( &info );
						}			
						
						VOLOGI("+VO_EVENT_BUFFERING_END");
						temp = 100;
						notify( VO_EVENT_BUFFERING_END , &temp );
						m_is_buffering = VO_FALSE;
						VOLOGI("-VO_EVENT_BUFFERING_END");
						
					}
				}
			}
			else
			{
				VOLOGI( "Buffer lack: lack_pos %lld lack_size %lld" , ptr_need->want_physical_pos , ptr_need->want_size );
				m_downloader.stop_download();
				if( m_is_init )
				{
					if( !m_downloader.start_download( ptr_need->want_physical_pos ) )
						return BUFFER_CALLBACK_ERROR;
				}
				else
				{
					if( !m_downloader.start_download_inthread( ptr_need->want_physical_pos ) )
						return BUFFER_CALLBACK_ERROR;
				}
			}
		}
		else	//do not support seek
		{
			VOLOGI( "Do not support seek! Current Download Pos: %lld" , info.start_download_pos );
			if( info.start_download_pos <= ptr_need->want_physical_pos )
			{
				//we should pause the player for a while
				long temp = 0;
				VOLOGI("+VO_EVENT_BUFFERING_BEGIN");
				notify( VO_EVENT_BUFFERING_BEGIN , &temp );
				m_is_buffering = VO_TRUE;
				VOLOGI("-VO_EVENT_BUFFERING_BEGIN");

				VO_BOOL bdownload_finish = VO_FALSE;

				//in a waiting loop for data download completed.
				do
				{
					voOS_Sleep( 50 );

					//get the latest download info
					m_downloader.get_dowloadinfo( &info );

					//check if the download data has meet the data request or not
					VO_S64 delta = (ptr_need->want_physical_pos + ptr_need->want_size) - info.cur_download_pos;

					if( delta > 0)
					{
						int percent = (int)((float)( info.cur_download_pos - info.start_download_pos ) / (float)( (ptr_need->want_physical_pos + ptr_need->want_size) - info.start_download_pos ) * 100. );
						VOLOGI("+VO_EVENT_BUFFERING_PERCENT : %d" , percent);
						notify( VO_EVENT_BUFFERING_PERCENT , (long*)&percent );
						VOLOGI("-VO_EVENT_BUFFERING_PERCENT : %d" , percent);
					}
					else
					{
						bdownload_finish = VO_TRUE;
					}
				}while( m_downloader.is_downloading() && !bdownload_finish   && !m_is_pause);

				//the download process should be finished
				VOLOGI("+VO_EVENT_BUFFERING_END");
				temp = 100;
				notify( VO_EVENT_BUFFERING_END , &temp );
				m_is_buffering = VO_FALSE;
				VOLOGI("-VO_EVENT_BUFFERING_END");
			}
			else
			{
				m_downloader.stop_download();
				if( m_is_init )
				{
					if( !m_downloader.start_download( ptr_need->want_physical_pos ) )
						return BUFFER_CALLBACK_ERROR;
				}
				else
				{
					if( !m_downloader.start_download_inthread( ptr_need->want_physical_pos ) )
						return BUFFER_CALLBACK_ERROR;
				}
			}
		}
	}
	else
	{
		VOLOGI( "Buffer lack: lack_pos %lld lack_size %lld" , ptr_need->want_physical_pos , ptr_need->want_size );
		if( m_buffer.get_filesize() > 0 && ( ptr_need->want_physical_pos + ptr_need->want_size > m_buffer.get_filesize() ) )
		{
			VOLOGE("the data requested is out of range");
			return BUFFER_CALLBACK_ERROR;
		}

		if( m_is_init )
		{
			if( !m_downloader.start_download( ptr_need->want_physical_pos ) )
				return BUFFER_CALLBACK_ERROR;
		}
		else
		{
			if( !m_downloader.start_download_inthread( ptr_need->want_physical_pos ) )
				return BUFFER_CALLBACK_ERROR;
		}

		voOS_Sleep( 100 );
	}

	if( m_is_to_exit || m_is_pause )
	{
		VOLOGI("BUFFER_CALLBACK_SHOULD_STOP");
		return BUFFER_CALLBACK_SHOULD_STOP;
	}

	return BUFFER_CALLBACK_OK;
}

VO_BOOL vo_PD_manager::open( VO_SOURCE_OPENPARAM * pParam )
{
	VO_TCHAR * url = (VO_TCHAR*)pParam->pSource;
	if( m_buffer_stream )
		m_buffer_stream->set_to_close( VO_FALSE );
	m_is_to_exit = VO_FALSE;

	memcpy( &m_source_param , pParam , sizeof(VO_SOURCE_OPENPARAM) );

	if( m_source_param.pSourceOP )
	{
		if( m_is_source2 )
		{
			VO_SOURCE2_EVENTCALLBACK * ptr_func = ( VO_SOURCE2_EVENTCALLBACK * )m_source_param.pSourceOP;
			m_notifier2.SendEvent = ptr_func->SendEvent;
			m_notifier2.pUserData = ptr_func->pUserData;
		}
		else
		{
			StreamingNotifyEventFunc * ptr_func = ( StreamingNotifyEventFunc * )m_source_param.pSourceOP;

			m_notifier.funtcion = ptr_func->funtcion;
			m_notifier.userdata = ptr_func->userdata;
		}
	}
	else
	{
		VOLOGI("notify-callback won't working" );
		m_notifier.funtcion = 0;
		m_notifier.userdata = 0;
	}
	
	if(m_source_param.pLibOP)
	{
		m_dllloader.SetLibOperator(m_source_param.pLibOP);
	}

	if( sizeof(VO_TCHAR) != 1 )
	{
		wcstombs(m_url,(wchar_t*)url,sizeof(m_url));
	}
	else
	{
		strcpy( m_url , (char *)url );
	}

	VOLOGI("Check Blacklist!" );
	if( strstr( m_url , "play.last.fm" ) || strstr( m_url , "0.0.0.0:8081" ) )
	{
		VOLOGI( "StreamType_NoneStop" );
		m_streamtype = StreamType_NoneStop;
	}

	//strcpy( m_url , "http://10.2.64.229/share/Jolin.3gp" );

	if( pParam->nReserve )
	{
		memcpy( &m_PD_param , (VO_PTR)pParam->nReserve , sizeof( m_PD_param ) );
		
		//set default buffertime to 3 seconds if not set.
		if( m_PD_param.nBufferTime <= 0 )
		{
			m_PD_param.nBufferTime = 3;
		}
	}
	else
	{
		long err_type = E_PD_PARAM_MISS;
		notify( VO_PD_EVENT_NOT_APPLICABLE_MEDIA ,&err_type);
		return VO_FALSE;
	}

	//set fixed 40MB buffer size
	m_PD_param.nMaxBuffer = 40960;
	VOLOGI( "Buffer Time: %d" , m_PD_param.nBufferTime );

	return VO_TRUE;
}

VO_U32 vo_PD_manager::close()
{
	VO_U32 ret = 0;
	
	m_buffer.close();

	if( m_buffer_stream )
		m_buffer_stream->set_to_close( VO_TRUE );
	m_is_to_exit = VO_TRUE;

	m_downloader.set_to_close(m_is_to_exit);

	VOLOGI("+wait delay open finish!" );
	while( m_is_init )
	{
		if(m_downloader.is_downloading())
		{
			VOLOGI("[moses]stop download thread!" );
			m_downloader.stop_download();
		}
		voOS_Sleep( 20 );
	}
	VOLOGI("-wait delay open finish!" );

	stop_getsample_thread();

	VOLOGI( "stop_getsample_thread done!" );

	stop_seek_thread();

	if(m_downloader.is_downloading())
	{
		VOLOGI("[moses]stop download thread!" );
		m_downloader.stop_download();
	}
	

	VOLOGI( "stop_seek_thread done!" );

	stop_startbuffering_thread();

	VOLOGI( "stop_startbuffering_thread done!" );

	if( m_parser_api.Close )
	{
		ret = m_parser_api.Close( m_fileparser );
		m_parser_api.Close = 0;
	}

	if( m_buffer_stream )
	{
		delete m_buffer_stream;
		m_buffer_stream = 0;
	}

	if( m_ptr_sample )
	{
		delete []m_ptr_sample;
	}
	m_ptr_sample = 0;

	if( m_ptr_bottom_sample )
	{
		delete []m_ptr_bottom_sample;
	}
	m_ptr_bottom_sample = 0;

	if( m_ptr_buffer )
	{
		for( VO_S32 i = 0 ; i < m_tracks ; i++ )
		{
			delete []m_ptr_buffer[i];
		}
		delete []m_ptr_buffer;
	}
	m_ptr_buffer = 0;

	if( m_ptr_bottom_buffer )
	{
		for( VO_S32 i = 0 ; i < m_tracks ; i++ )
		{
			delete []m_ptr_bottom_buffer[i];
		}
		delete []m_ptr_bottom_buffer;
	}
	m_ptr_bottom_buffer = 0;

	if( m_getsample_ret )
		delete []m_getsample_ret;
	m_getsample_ret = 0;

	if( m_bsourcend )
		delete []m_bsourcend;
	m_bsourcend = 0;

	if( m_max_sampsize )
		delete []m_max_sampsize;
	m_max_sampsize = 0;

	if( m_getsample_flag )
		delete []m_getsample_flag;
	m_getsample_flag = 0;

	if( m_bdrop_frame )
		delete []m_bdrop_frame;
	m_bdrop_frame = 0;

	if( m_ptr_timestamp )
		delete []m_ptr_timestamp;
	m_ptr_timestamp = 0;

	if( m_getsamplelock )
		delete []m_getsamplelock;
	m_getsamplelock = 0;

	if( m_sem_getsample )
		delete []m_sem_getsample;
	m_sem_getsample = 0;

	if(m_ptrpool)
	{
		delete []m_ptrpool;
		m_ptrpool = 0;
		m_poolsize = 0;
		m_downloader.set_comparedata(0,0);
	}

	if(m_ptr_playlist_parser)
	{
		delete m_ptr_playlist_parser;
		m_ptr_playlist_parser = 0;
	}

	return ret;
}

VO_U32 vo_PD_manager::get_sourceinfo(VO_SOURCE_INFO * pSourceInfo)
{
	if(m_is_buffering_on_start == VO_TRUE)
	{
		m_is_buffering_on_start = VO_FALSE;
		do_startbuffering();
	}

	VO_U32 ret =  m_parser_api.GetSourceInfo( m_fileparser , pSourceInfo );

	if( ret == 0 )
	{
		m_tracks = pSourceInfo->Tracks;

		if( m_duration )
			pSourceInfo->Duration = m_duration;
	}

	return ret;
}

VO_U32 vo_PD_manager::get_trackinfo(VO_U32 nTrack, VO_SOURCE_TRACKINFO * pTrackInfo)
{
	if(m_is_buffering_on_start == VO_TRUE)
	{
		m_is_buffering_on_start = VO_FALSE;
		do_startbuffering();
	}

	VO_U32 ret =  m_parser_api.GetTrackInfo( m_fileparser , nTrack , pTrackInfo );

	if( m_duration )
		pTrackInfo->Duration = m_duration;

	VOLOGI("%u is %s" , nTrack , pTrackInfo->Type == VO_SOURCE_TT_VIDEO ? "Video" : "Audio" );

	if( pTrackInfo->Type == VO_SOURCE_TT_VIDEO )
	{
		m_videotrack = nTrack;
	}

	return ret;
}

//when network adapter switch between wifi and 3G, we should retry download data.
VO_VOID vo_PD_manager::ProcessNetworkError()
{
	if( m_retry_download_pos >= 0 && m_is_networkerror )
	{
		VO_BOOL bRetry = VO_FALSE;

		if( 0 == m_last_download_retry_time )
		{
			m_last_download_retry_time = voOS_GetSysTime();
			bRetry = VO_TRUE;
		}
		else if( voOS_GetSysTime() - m_last_download_retry_time > 3000 )
		{
			m_last_download_retry_time = voOS_GetSysTime();
			bRetry = VO_TRUE;
		}
		else
		{
			bRetry = VO_FALSE;
		}

		if( bRetry )
		{
			if( !m_downloader.is_downloading() && !m_is_to_exit )
			{
				VOLOGI( "network adapter changed ,so retry download : %lld" , m_retry_download_pos );
				m_downloader.start_download_inthread( m_retry_download_pos );
			}
			else
			{
				;
			}
		}
	}
}

VO_U32 vo_PD_manager::get_sample(VO_U32 nTrack, VO_SOURCE_SAMPLE * pSample)
{
	//use getsample routine as a timer.
	ProcessNetworkError();

	//if the track reach end, we should return directly
	if( m_bsourcend[nTrack] )
	{
		return VO_ERR_SOURCE_END;
	}

	//if the track reach end, or user tell exit ,we should return directly
	if( m_is_to_exit )
	{
		return VO_ERR_SOURCE_END;
	}

	//if is buffering or seeking now ,then we should not allow getsample etc.
    if( m_is_seeking || m_is_buffering )
    {
        //VOLOGI( "In seeking mode, return retry!" );
        return VO_ERR_SOURCE_NEEDRETRY;
    }

	//set the flag to true, so get sample thread will work for this track.
	m_getsample_flag[nTrack] = 1;

	//VOLOGI( "+vo_PD_manager::get_sample %d" , nTrack );
	if( m_thread_handle == 0 )
	{
		m_b_stop_getsample = VO_FALSE;
		begin((char *)"PD_Getsample");
	}

//	if( !m_ptr_timestamp )
//	{
//		//VOLOGI(  "-vo_PD_manager::get_sample %d" , nTrack );
//		return VO_ERR_SOURCE_NEEDRETRY;
//	}

    VOLOGI("+m_sem_getsample[%d].Down" , nTrack );
	VO_U32 ret = m_sem_getsample[nTrack].Down( 1000 );
    VOLOGI("-m_sem_getsample[%d].Down" , nTrack );

	//thread should be running when not seek, if not running ,it means end of file has been reached
	if(is_thread_running() == VO_FALSE && m_is_seeking == VO_FALSE)
	{
		VOLOGE( "thread not running and not seeking, so EOS" );
		return VO_ERR_SOURCE_END;
	}

	if( ret == VOVO_SEM_TIMEOUT )
	{
		//VOLOGI( "-vo_PD_manager::get_sample %d" , nTrack );
		return VO_ERR_SOURCE_NEEDRETRY;
	}
	else
	{
		VOLOGI( "+m_getsamplelock" );
		voCAutoLock lock( &m_getsamplelock[nTrack] );
		VOLOGI( "-m_getsamplelock" );

		memcpy( &m_ptr_sample[nTrack] , &m_ptr_bottom_sample[nTrack] , sizeof(VO_SOURCE_SAMPLE) );
		if( m_ptr_sample[nTrack].Size != 0 && m_getsample_ret[nTrack] == VO_ERR_NONE )
		{
			//enable drop frame mechanism		
//			if( pSample->Time > m_ptr_sample[nTrack].Time + 100 && (VO_U32)m_videotrack == nTrack )
//			{
//				VOLOGI( "drop frame, ask:%lld , current:%lld " , pSample->Time , m_ptr_sample[nTrack].Time );
//				m_ptr_timestamp[nTrack] = pSample->Time;
//				m_bdrop_frame[nTrack] = 1;
				//m_ptr_sample[nTrack].Size = 0;
                //m_getsample_ret[nTrack] = VO_ERR_NONE;
                //m_sem_emptysample.Up();
				//return VO_ERR_SOURCE_NEEDRETRY;
//			}

			VOLOGI("+vo_PD_manager::get_sample Track: %d Time %lld" , nTrack , pSample->Time );
			memcpy( pSample , &m_ptr_sample[nTrack] , sizeof(VO_SOURCE_SAMPLE) );
			pSample->Buffer = m_ptr_buffer[nTrack];

			//the sample length get from parser should not exceed the max sample length, else out of range will happen.	
			if( m_max_sampsize[nTrack] && ( (pSample->Size & 0x7FFFFFFF) > m_max_sampsize[nTrack]) )
			{
				VOLOGI("[parser error]Track: %d , size:%u  ,maxsize: %u" , nTrack , pSample->Size & 0x7FFFFFFF , m_max_sampsize[nTrack]);
				pSample->Size = 0;
				pSample->Buffer = 0;

				//PD should not set return value , just transfer value
			//	m_getsample_ret[nTrack] = VO_ERR_SOURCE_END;
			}
			else
			{
				memcpy( m_ptr_buffer[nTrack] , m_ptr_bottom_buffer[nTrack] , m_ptr_sample[nTrack].Size & 0x7FFFFFFF );
				//VOLOGI( "%p %p %u" , pSample->Buffer , m_ptr_sample[nTrack].Buffer , pSample->Size & 0x7FFFFFFF );
				//memcpy( pSample->Buffer , m_ptr_sample[nTrack].Buffer , pSample->Size & 0x7FFFFFFF );
			}

			VO_U32 ret = m_getsample_ret[nTrack];

			m_getsample_ret[nTrack] = VO_ERR_NONE;
			m_ptr_sample[nTrack].Size = 0;
            m_sem_emptysample.Up();

			VOLOGI("-vo_PD_manager::get_sample Track: %d , TimeStamp: %lld , Size: %u , IsKeyFrame: %s , ret: %u" , nTrack , pSample->Time , pSample->Size & 0x7FFFFFFF , pSample->Size & 0x80000000 ? "TRUE" : "FALSE"  , ret );

			return ret;
		}
		else if( m_getsample_ret[nTrack] != VO_ERR_NONE )
		{
			VO_U32 ret = m_getsample_ret[nTrack];

			m_getsample_ret[nTrack] = VO_ERR_NONE;
            m_ptr_sample[nTrack].Size = 0;
            m_sem_emptysample.Up();

			VOLOGI("-vo_PD_manager::get_sample %d" , nTrack );
			return ret;
		}
		else
		{
            m_getsample_ret[nTrack] = VO_ERR_NONE;
            m_ptr_sample[nTrack].Size = 0;
            m_sem_emptysample.Up();

			VOLOGI("-vo_PD_manager::get_sample %d" , nTrack );
			return VO_ERR_SOURCE_NEEDRETRY;
		}
	}
}

VO_U32 vo_PD_manager::get_sample_internal(VO_U32 nTrack, VO_SOURCE_SAMPLE * pSample)
{
	if( !m_sem_seekcount.Waiting() || m_is_seeking )
		return VO_ERR_SOURCE_NEEDRETRY;

	VOLOGI("+vo_PD_manager::get_sample_internal Track: %d , %lld" , nTrack , pSample->Time );
	VO_U32 ret = m_parser_api.GetSample( m_fileparser , nTrack , pSample );
	VOLOGI("-vo_PD_manager::get_sample_internal Track: %d , TimeStamp: %lld , ret: %u , %p" , nTrack , pSample->Time , ret , pSample->Buffer );

	return ret;
}

VO_U32 vo_PD_manager::set_pos(VO_U32 nTrack, VO_S64 * pPos)
{
	//VOLOGI( "m_parser_api.SetPos track: %u , Pos: %lld" , nTrack , *pPos );
	//if( m_is_init )
	//	return VO_ERR_FAILED;

	//VO_U32 ret = m_parser_api.SetPos( m_fileparser , nTrack , pPos );
	//m_ptr_timestamp[nTrack] = *pPos;
	//VOLOGI( "set pos ret: %u" , ret );

	VOLOGI("setpos to %lld" , *pPos );

	return 0;
}

VO_VOID vo_PD_manager::moveto( VO_S64 pos )
{
	VOLOGI( "+vo_PD_manager::moveto : %lld " , pos );
	if( m_is_init )
	{
		VOLOGI( "-vo_PD_manager::moveto" );
		return;
	}

	//we should reset flag when seek
	memset( m_bsourcend , 0 , m_tracks * sizeof( VO_U32 ) );

	voCAutoLock lock(&m_seekpos_lock);

	m_seekpos = pos;
	m_sem_seekcount.Up();

	VOLOGI( "-vo_PD_manager::moveto" );
}

VO_U32 vo_PD_manager::set_sourceparam(VO_U32 uID, VO_PTR pParam)
{
	VOLOGE( "vo_PD_manager::set_sourceparam uID: %d " , uID );
	switch( uID )
	{
	case VOID_STREAMING_OPENURL:
		{
			//return VO_ERR_NONE;
			VO_BOOL ret;
			ret = delay_open();
			if( ret )
			{
				return VO_ERR_NONE;
			}
			else
			{
				return VO_ERR_SOURCE_OPENFAIL;
			}
		}
		break;
	case VOID_STREAMING_QUICKRESPONSE_COMMAND:
		{
			VO_QUICKRESPONSE_COMMAND* ptr_command = (VO_QUICKRESPONSE_COMMAND*)pParam;
			process_STREAMING_QUICKRESPONSE_COMMAND(*ptr_command);
			return VO_ERR_NONE;
		}
		break;
	case VO_PID_SOURCE_SEEKMODE:
		{
			TIME_SEEKMODETYPE* seekmode = (TIME_SEEKMODETYPE*)pParam;

			//process the command of seekmode notification
			m_seekmode = *seekmode;

			return VO_ERR_NONE;
		}
		break;
	case VOID_STREAMING_LIBOP:
		{
			VO_LIB_OPERATOR * ptr = (VO_LIB_OPERATOR *)pParam;
			m_dllloader.SetLibOperator( ptr );

			return VO_ERR_NONE;
		}
		break;
	case VO_PID_SOURCE_HTTPVERIFICATIONCALLBACK:
		{
			VO_HTTP_VERIFYCALLBACK * ptr = (VO_HTTP_VERIFYCALLBACK *)pParam;

			if( ptr )
			{
				m_http_verifycb.HTTP_Callback = ptr->HTTP_Callback;
				m_http_verifycb.hHandle = ptr->hHandle;
			}
			else
			{
				m_http_verifycb.HTTP_Callback = 0;
				m_http_verifycb.hHandle = 0;
			}

			VOLOGI("http verify callback: 0x%08x , userdata: 0x%08x" , m_http_verifycb.HTTP_Callback , m_http_verifycb.hHandle);
			
			return VO_ERR_NONE;
		}
		break;
	case VO_PID_SOURCE_DOHTTPVERIFICATION:
		{
			VO_SOURCE_VERIFICATIONINFO * ptr = (VO_SOURCE_VERIFICATIONINFO *)pParam;

			VOLOGI("http verify data: %s , size: %d" , ptr->pData , ptr->uDataSize );

			//replace old cookies with new input string.
			strncpy( m_dwango_cookies , (const char*)ptr->pData , ptr->uDataSize );

			//we should append "\r\n" if it not exist in the original cookies
			if( m_dwango_cookies[ptr->uDataSize - 1] != '\n' || m_dwango_cookies[ptr->uDataSize - 2] != '\r')
			{
				strcat(m_dwango_cookies , (char *)  "\r\n" );
			}

			m_PD_param.ptr_cookie = m_dwango_cookies;

			return VO_ERR_NONE;
		}
		break;
	case VO_PID_SOURCE_EVENTCALLBACK:
		{
			VO_SOURCE_EVENTCALLBACK * ptr = (VO_SOURCE_EVENTCALLBACK *)pParam;
						
			if( ptr )
			{
				m_http_eventcb.SendEvent = ptr->SendEvent;
				m_http_eventcb.pUserData = ptr->pUserData;
			}
			else
			{
				m_http_eventcb.SendEvent = 0;
				m_http_eventcb.pUserData = 0;
			}

			VOLOGI("http event callback: 0x%08x , userdata: 0x%08x" , m_http_eventcb.SendEvent , m_http_eventcb.pUserData );
			
			return VO_ERR_NONE;
		}
		break;
	case VO_PID_SOURCE_WORKPATH:
		{
			VOLOGI("dll load path: %s " , (VO_PTCHAR)pParam );
			if( pParam )
			{
				//does the work path string will be available all the time?
				//should we make a copy of it?
				m_dllloader.SetWorkPath( (VO_PTCHAR)pParam );
			}
		}
		break;
	case VOID_HTTP_ADD_HEADER:
		{
			VO_CHAR* pheader = ( VO_CHAR*)pParam;

			VOLOGI( "[dlna] ADD_HEADER %s" , pheader);

			m_dlna_protocolinfo.append_reqitem( pheader );

			return VO_ERR_NONE;
		}
		break;
	case VOID_HTTP_REMOVE_HEADER:
		{
			VO_CHAR* pheader = 0;
			pheader = ( VO_CHAR*)pParam;

			VOLOGI( "[dlna] REMOVE_HEADER %s" , pheader);

			return VO_ERR_NONE;
		}
		break;
	case VOID_HTTP_REQUEST_OPTION_KEEPCONNECTION_ON_PLAY:
		{
			VO_U32* pvalue = ( VO_U32*)pParam;

			VOLOGI( "[dlna] KEEPCONNECTION_ON_PLAY: %d" , *pvalue );

			m_dlna_protocolinfo.set_connection_stalling_on_play( *pvalue );
			if( *pvalue > 0)
			{
				VOLOGI( "StreamType_NoneStop for dlna keep connection on play case" );
				m_streamtype = StreamType_NoneStop;
			}

			return VO_ERR_NONE;
		}
		break;
	case VOID_HTTP_REQUEST_OPTION_KEEPCONNECTION_ON_PAUSE:
		{
			VO_U32* pvalue = ( VO_U32*)pParam;

			VOLOGI( "[dlna] KEEPCONNECTION_ON_PAUSE: %d" , *pvalue );

			m_dlna_protocolinfo.set_connection_stalling_on_pause( *pvalue );
			if( *pvalue > 0)
			{
				VOLOGI( "StreamType_NoneStop for dlna keep connection on pause case" );
				m_streamtype = StreamType_NoneStop;
			}

			return VO_ERR_NONE;
		}
		break;
	case VOID_HTTP_REQUEST_OPTION_ENABLE_HTTPRANGE:
		{
			VO_U32* pvalue = ( VO_U32*)pParam;

			VOLOGI( "[dlna] ENABLE_HTTPRANGE: %d" , *pvalue );

			m_dlna_protocolinfo.set_byterange_support( *pvalue );

			return VO_ERR_NONE;
		}
		break;
	case VOID_HTTP_REQUEST_OPTION_CONNECTION_TIMEOUT:
		{
			VO_U32* pvalue = ( VO_U32*)pParam;

			VOLOGI( "[dlna] CONNECTION_TIMEOUT: %d" , *pvalue );

			m_dlna_protocolinfo.set_connection_timeout( *pvalue );

			return VO_ERR_NONE;
		}
		break;
	case VOID_HTTP_REQUEST_OPTION_READ_TIMEOUT:
		{
			VO_U32* pvalue = ( VO_U32*)pParam;

			VOLOGI( "[dlna] READ_TIMEOUT: %d" , *pvalue );

			m_dlna_protocolinfo.set_read_timeout( *pvalue );

			return VO_ERR_NONE;
		}
		break;
	case VOID_STREAMING_PDCONNECTRETRY_TIMES:
		{
			VO_U32* pvalue = ( VO_U32*)pParam;

			VOLOGI( "connect retry times: %d" , *pvalue );

			//store the max connect retry times value
			m_connectretry_times = *pvalue;

			m_downloader.set_connectretry_times( *pvalue );
		}
		break;
	case VO_PID_SOURCE2_BUFFER_STARTBUFFERINGTIME:
		{
			VO_U32* pvalue = ( VO_U32*)pParam;
			m_PD_param.nBufferTime = (VO_U32)(*pvalue);

			return VO_ERR_NONE;
		}
		break;
	}

	if( m_parser_api.SetSourceParam )
	{
		return m_parser_api.SetSourceParam( m_fileparser , uID , pParam );
	}
	else
	{
		return VO_ERR_FAILED;
	}
}

VO_VOID vo_PD_manager::process_STREAMING_QUICKRESPONSE_COMMAND( VO_QUICKRESPONSE_COMMAND command )
{
	switch( command )
	{
	case VO_QUICKRESPONSE_RUN:
		{
			VOLOGE( "+++++++++++++++++++++VO_QUICKRESPONSE_RUN+++++++++++++++++++");
			m_is_pause = VO_FALSE;

			if( m_dlna_protocolinfo.is_dlna() )
			{
				m_downloader.pause_connection(VO_FALSE);
			}
		}
		break;
	case VO_QUICKRESPONSE_PAUSE:
		{
			VOLOGE( "+++++++++++++++++++++VO_QUICKRESPONSE_PAUSE+++++++++++++++++++");
			//m_is_pause = VO_TRUE;
		}
		break;
	case VO_QUICKRESPONSE_PAUSE_MANUAL:
		{
			VOLOGE( "+++++++++++++++++++++VO_QUICKRESPONSE_PAUSE_MANUAL+++++++++++++++++++");
			if( m_dlna_protocolinfo.is_dlna() )
			{
				m_downloader.pause_connection(VO_TRUE);
			}
		}
		break;
	case VO_QUICKRESPONSE_STOP:
		{
			VOLOGE( "+++++++++++++++++++++VO_QUICKRESPONSE_STOP+++++++++++++++++++");
			m_is_pause = VO_TRUE;
			if( m_buffer_stream )
				m_buffer_stream->set_to_close( VO_TRUE );
		}
		break;
	case VO_QUICKRESPONSE_FLUSH:
		{
			VOLOGE("+++++++++++++++++++++VO_QUICKRESPONSE_FLUSH+++++++++++++++++++");
			m_is_pause = VO_FALSE;
		}
		break;
	case VO_QUICKRESPONSE_RETURNBUFFER:
		{
			VOLOGE( "+++++++++++++++++++++VO_QUICKRESPONSE_RETURNBUFFER+++++++++++++++++++");
			//m_is_pause = VO_TRUE;
		}
		break;
	}
}

VO_U32 vo_PD_manager::get_sourceparam(VO_U32 uID, VO_PTR pParam)
{
	switch( uID )
	{
	case 0x4300016:
	case 0x3802:
		{
			BUFFER_INFO info;
			m_buffer.get_buffer_info( &info );

			VO_FILE_MEDIATIMEFILEPOS pos;
			pos.nFlag = 1;
			pos.llFilePos = info.physical_start + info.usedsize;

			VO_U32 ret = m_parser_api.GetSourceParam( m_fileparser , VO_PID_FILE_FILEPOS2MEDIATIME , &pos );

			if( ret == 0 )
			{
				VOLOGI("Use Media Time to determine: Media Time %lld  Duration %u" , pos.llMediaTime , m_duration );
				if( m_duration )
				{
					*((VO_S32*)pParam) = (VO_S32)((float)( pos.llMediaTime ) * 100. / (float)m_duration);
				}
				else
					*((VO_S32*)pParam) = 0;
			}
			else
			{
				*((VO_S32*)pParam) = (VO_S32)(( info.physical_start + info.usedsize ) * 100. /(float)m_buffer.get_filesize());
			}

			VOLOGI("Current Buffering Duration %d" , *((VO_S32*)pParam) );
		}
		break;
	case VOID_HTTP_GET_RESPONSE:
		{
			VO_S32 size = strlen( m_dlna_protocolinfo.get_server_response() );
			strncpy( (VO_CHAR*) pParam , m_dlna_protocolinfo.get_server_response() , size >= 2048 ? 2048 - 1 : size );
			VOLOGI( "[dlna]----get http response: %s" , ((VO_CHAR*)pParam) );
			return VO_ERR_NONE;
		}
		break;
	default:
		return m_parser_api.GetSourceParam( m_fileparser , uID , pParam );
	}

	return 0;
}

VO_U32 vo_PD_manager::set_trackparam(VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
{
	return m_parser_api.SetTrackParam( m_fileparser , nTrack , uID , pParam );
}

VO_U32 vo_PD_manager::get_trackparam(VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
{
	if(m_is_buffering_on_start == VO_TRUE)
	{
		m_is_buffering_on_start = VO_FALSE;
		do_startbuffering();
	}
	return m_parser_api.GetTrackParam( m_fileparser , nTrack , uID , pParam );
}

VO_BOOL vo_PD_manager::ProcessShoutCast()
{
	//we should set file size info to unknow, 
	//since the file size info before is not media file size info ,but shoutcast playlist size info
	m_buffer.set_filesize( -1 );

	m_is_shoutcast = VO_FALSE;
	m_buffer.reset();

	m_downloader.set_download_callback( this );
	m_buffer.set_buffer_callback( this );

	m_is_init = VO_TRUE;
	m_buffer.set_initmode( m_is_init );
//	m_downloader.set_initmode( m_is_init );

	VOLOGI("[shoutCast]try the new url:%s ", m_url);

	if( !m_downloader.set_url( m_url , &m_PD_param ) )
	{
		VOLOGE( "set download url failed!" );
		m_is_init = VO_FALSE;
		return VO_FALSE;
	}

	VOLOGI( "+m_downloader.start_download()" );
	if( !m_downloader.start_download() )
	{
		m_is_init = VO_FALSE;
		return VO_FALSE;
	}


	VOLOGI( "-m_downloader.start_download()" );

	if( m_is_to_exit )
	{
		m_is_init = VO_FALSE;
		return VO_FALSE;
	}

	//		if(m_buffer_stream)
	//		{
	//			delete m_buffer_stream;
	//			m_buffer_stream = 0;
	//		}
	//
	//			m_buffer_stream = new vo_buffer_stream( &m_buffer );

	return VO_TRUE;
}

//download partial data and load related parser.
VO_BOOL vo_PD_manager::LoadParser()
{
	VO_BOOL bpre = perpare_parser();
	while( !bpre )
	{
		if(m_is_shoutcast)
		{
			//for shoutcast , we should download in nonestop mode.
			m_streamtype = StreamType_NoneStop;
			if(!ProcessShoutCast())
			{
				return VO_FALSE;
			}
			else
			{
				bpre = perpare_parser();
			}
		}
		else
		{
			m_is_init = VO_FALSE;
			return VO_FALSE;
		}
	}

	return VO_TRUE;
}


VO_BOOL vo_PD_manager::delay_open()
{
	m_buffer.init( &m_PD_param );

	m_downloader.set_download_callback( this );
	m_buffer.set_buffer_callback( this );

	m_is_init = VO_TRUE;
	m_buffer.set_initmode( m_is_init );

		
	//for dlna ctt test purpose
	//m_dlna_protocolinfo.set_dlna();
	//m_dlna_protocolinfo.set_connection_stalling_on_play( 1 );
	//m_dlna_protocolinfo.set_connection_stalling_on_pause( 1 );
	//m_dlna_protocolinfo.set_byterange_support( 0 );
	//m_dlna_protocolinfo.append_reqitem( "transferMode.dlna.org: Streaming" );
	//m_dlna_protocolinfo.append_reqitem( "getcontentFeatures.dlna.org: 1" );
	//m_dlna_protocolinfo.append_reqitem( "User-Agent: DLNA" );
	//m_dlna_protocolinfo.set_connection_timeout( 60000000 );
	//m_dlna_protocolinfo.set_read_timeout( 60000000 );

	if( !m_downloader.set_url( m_url , &m_PD_param ) )
	{
		long err_type = E_PD_SET_DOWNLOAD_URL_FAILED;
		notify( VO_PD_EVENT_NOT_APPLICABLE_MEDIA ,&err_type);
		VOLOGE( "set download url failed!" );
		m_is_init = VO_FALSE;
		return VO_FALSE;
	}

	m_downloader.set_workpath(m_pstrWorkPath);
	m_downloader.set_connectretry_times( m_connectretry_times );

	//for dlna ctt test purpose
	//m_downloader.pause_connection(VO_TRUE);

	m_downloader.set_dlna_param((VO_VOID*) &m_dlna_protocolinfo);

	VOLOGI( "+m_downloader.start_download()" );
	if( !m_downloader.start_download() )
	{
		VOLOGE( "download failed!" );
		m_is_init = VO_FALSE;
		return VO_FALSE;
	}
	VOLOGI( "-m_downloader.start_download()" );

	if( m_is_to_exit )
	{
		m_is_init = VO_FALSE;
		return VO_FALSE;
	}

	m_buffer_stream = new vo_buffer_stream( &m_buffer );
	if( !m_buffer_stream )
	{
		VOLOGE( "new obj failed" );
		return VO_FALSE;
	}

	if( !LoadParser() )
	{
		return VO_FALSE;
	}

	if( m_is_to_exit )
	{
		m_parser_api.Close = 0;
		m_is_init = VO_FALSE;
		return VO_FALSE;
	}

	VO_SOURCE_OPENPARAM param={0};//TODO
	param.nFlag			= 0x102;//0x00020101;

	VO_FILE_SOURCE source;
	memset(&source , 0 , sizeof(VO_FILE_SOURCE));
	source.nFlag = param.nFlag;
	source.pSource = m_PD_param.mFilePath;
	source.nReserve = ( VO_U32 )this;
	param.pSource		= &source;
	param.pSourceOP= &m_opFile;
	param.pDrmCB = m_source_param.pDrmCB;
	VOLOGI("DRM Engine Ptr: %p" , param.pDrmCB );
	param.pMemOP = m_source_param.pMemOP;
	param.pstrWorkpath = m_pstrWorkPath;

	VOLOGI( "+m_parser_api.Open( &m_fileparser , &param )" );
	VO_U32 ret = m_parser_api.Open( &m_fileparser , &param );
	VOLOGI( "-m_parser_api.Open( &m_fileparser , &param )" );

	if( ret != 0 )
	{
		long err_type = E_PD_FILE_PARSER_OPEN_FAILED;
		notify( VO_PD_EVENT_NOT_APPLICABLE_MEDIA ,&err_type);
		VOLOGE( "file parser open failed!" );
		m_parser_api.Close = 0;
		m_is_init = VO_FALSE;
		return VO_FALSE;
	}

	if( m_is_to_exit )
	{
		m_is_init = VO_FALSE;
		return VO_FALSE;
	}

	VO_SOURCE_INFO source_info;
	m_parser_api.GetSourceInfo( m_fileparser , &source_info );

	if( m_downloader.get_duration() )
	{
		source_info.Duration = m_downloader.get_duration();
		VOLOGE( "Duration get from downloader: %u" , source_info.Duration );
	}

    if( source_info.Duration )
	{
		m_bitrate = (VO_S32)(m_downloader.get_actual_filesize() * 8000 / source_info.Duration);
	}
	else
	{
		m_bitrate = 0;
	}
	
	m_duration = source_info.Duration;

	if( source_info.Tracks > 0)
	{
		m_tracks = source_info.Tracks;
	}
	else
	{
		VOLOGE( "track count is: %d" , source_info.Tracks );
		m_is_init = VO_FALSE;
		return VO_FALSE;
	}

	VOLOGE( "Bitrate of the file: %d" , m_bitrate );

	headerdata_info headerdata;
	memset( &headerdata , 0 , sizeof(headerdata) );
	ret = m_parser_api.GetSourceParam( m_fileparser , VO_PID_FILE_HEADDATAINFO , &headerdata );

	if( ret == VO_ERR_NONE && headerdata.arraysize )
	{
		headerdata.ptr_array = new headerdata_element[headerdata.arraysize];
		if( !headerdata.ptr_array )
		{
			VOLOGE( "new obj failed" );
			return VO_FALSE;
		}
		m_parser_api.GetSourceParam( m_fileparser , VO_PID_FILE_HEADDATAINFO , &headerdata );

#if 1
		VOLOGI("Got cache buffers! Size: %d Reserved: %d" , headerdata.arraysize , headerdata.reserved );
		VOLOGI( "List Below: " );
		for( VO_S32 i = 0 ; i < headerdata.arraysize ; i++ )
		{
			VOLOGI( " Pos : %lld    Size : %lld " , headerdata.ptr_array[i].physical_pos , headerdata.ptr_array[i].size );
		}
		VOLOGI( "List End!" );
#endif

		arrange_headdata( &headerdata );
	}
	else
	{
		memset( &headerdata , 0 , sizeof(headerdata) );
	}

	if( m_streamtype != StreamType_NoneStop )
		m_downloader.stop_download();
	//m_downloader.stop_download();

#if 1
	VOLOGI("We need to cache %d buffers!" , headerdata.arraysize );
	VOLOGI( "List Below: " );
	for( VO_S32 i = 0 ; i < headerdata.arraysize ; i++ )
	{
		VOLOGI( " Pos : %lld    Size : %lld " , headerdata.ptr_array[i].physical_pos , headerdata.ptr_array[i].size );
	}
	VOLOGI( "List End!" );
#endif

	if( m_is_to_exit )
	{
		m_is_init = VO_FALSE;
		return VO_FALSE;
	}

	if(m_dlna_protocolinfo.is_dlna() && m_dlna_protocolinfo.is_connection_stalling_on_play())
	{
		VOLOGE("dlna and connect stall on play, so disable header buffer destroy" );
	}
	else 
	if( !m_buffer.rerrange_cache( headerdata.ptr_array , headerdata.arraysize ) )
	{
		VOLOGE("cache buffer failed!" );
		if( headerdata.ptr_array )
		{
			delete []headerdata.ptr_array;
		}
		m_is_init = VO_FALSE;
		return VO_FALSE;
	}

	if( headerdata.ptr_array )
	{
		delete []headerdata.ptr_array;
	}

	if( m_is_to_exit )
	{
		m_is_init = VO_FALSE;
		return VO_FALSE;
	}

	m_buffer.set_initmode( VO_FALSE );

	if( m_streamtype != StreamType_NoneStop || !m_downloader.is_downloading() )
	{
		BUFFER_INFO info;

		m_buffer.get_buffering_start_info( &info );

		VOLOGE( "cached buffer: start %d , size %d" , (VO_S32)info.physical_start , (VO_S32)info.usedsize );

		if( info.usedsize != m_buffer.get_filesize() )
			m_downloader.start_download( info.physical_start + info.usedsize );
	}

	m_ptr_sample = new VO_SOURCE_SAMPLE[m_tracks];
	if( !m_ptr_sample )
	{
		VOLOGE( "new obj failed" );
		return VO_FALSE;
	}
	memset( m_ptr_sample , 0 , m_tracks * sizeof( VO_SOURCE_SAMPLE ) );
	m_ptr_bottom_sample = new VO_SOURCE_SAMPLE[m_tracks];
	if( !m_ptr_bottom_sample )
	{
		VOLOGE( "new obj failed" );
		return VO_FALSE;
	}
	memset( m_ptr_bottom_sample , 0 , m_tracks * sizeof( VO_SOURCE_SAMPLE ) );

	m_getsample_ret = new VO_U32[m_tracks];
	if( !m_getsample_ret )
	{
		VOLOGE( "new obj failed" );
		return VO_FALSE;
	}
	memset( m_getsample_ret , 0 , m_tracks * sizeof( VO_U32 ) );

	m_bsourcend = new VO_U32[m_tracks];
	if( !m_bsourcend )
	{
		VOLOGE( "new obj failed" );
		return VO_FALSE;
	}
	memset( m_bsourcend , 0 , m_tracks * sizeof( VO_U32 ) );

	m_getsample_flag = new VO_U32[m_tracks];
	if( !m_getsample_flag )
	{
		VOLOGE( "new obj failed" );
		return VO_FALSE;
	}
	memset( m_getsample_flag , 0 , m_tracks * sizeof( VO_U32 ) );

	m_bdrop_frame = new VO_U32[m_tracks];
	if( !m_bdrop_frame )
	{
		VOLOGE( "new obj failed" );
		return VO_FALSE;
	}
	memset( m_bdrop_frame , 0 , m_tracks * sizeof( VO_U32 ) );
	
	m_max_sampsize = new VO_U32[m_tracks];
	if( !m_max_sampsize )
	{
		VOLOGE( "new obj failed" );
		return VO_FALSE;
	}
	memset( m_max_sampsize , 0 , m_tracks * sizeof( VO_U32 ) );

	m_ptr_buffer = new VO_PBYTE[m_tracks];
	if( !m_ptr_buffer )
	{
		VOLOGE( "new obj failed" );
		return VO_FALSE;
	}
	memset( m_ptr_buffer , 0 , m_tracks * sizeof(VO_PBYTE) );

	m_ptr_bottom_buffer = new VO_PBYTE[m_tracks];
	if( !m_ptr_bottom_buffer )
	{
		VOLOGE( "new obj failed" );
		return VO_FALSE;
	}
	memset( m_ptr_bottom_buffer , 0 , m_tracks * sizeof(VO_PBYTE) );

	m_ptr_timestamp = new VO_S64[m_tracks];
	if( !m_ptr_timestamp )
	{
		VOLOGE( "new obj failed" );
		return VO_FALSE;
	}
	memset( m_ptr_timestamp , 0 , m_tracks * sizeof(VO_S64) );

	m_getsamplelock = new voCMutex[m_tracks];
	if( !m_getsamplelock )
	{
		VOLOGE( "new obj failed" );
		return VO_FALSE;
	}
	m_sem_getsample = new voCSemaphore[m_tracks];
	if( !m_sem_getsample )
	{
		VOLOGE( "new obj failed" );
		return VO_FALSE;
	}

    m_sem_emptysample.Reset();

    for( VO_S32 i = 0 ; i < m_tracks ; i++ )
	{
		m_sem_emptysample.Up();
	}

	for( VO_S32 i = 0 ; i < m_tracks ; i++ )
	{
		m_parser_api.GetTrackParam( m_fileparser , i , VO_PID_SOURCE_MAXSAMPLESIZE , (VO_PTR)&m_max_sampsize[i] );

		//the maxsamplesize maybe wrong, so we need to adjust the value.
		if( m_max_sampsize[i] < 10 * 1024 )
		{
			VOLOGI("the max sample size of track : %d is : %d, so adjust it to 10k" , i , m_max_sampsize[i] );
			m_max_sampsize[i] = 10 * 1024;
		}
		m_ptr_buffer[i] = new VO_BYTE[m_max_sampsize[i]];
		m_ptr_bottom_buffer[i] = new VO_BYTE[m_max_sampsize[i]];
		if( !m_ptr_buffer[i] || !m_ptr_bottom_buffer[i] )
		{
			VOLOGE( "new obj failed" );
			return VO_FALSE;
		}
	}

	//voOS_Sleep( 500 );
//	do_startbuffering();


//	VO_U32 threadid;
//	voThreadCreate( &m_seek_threadhandle , &threadid , (voThreadProc)seekto , this , 0 );

	unsigned int threadid = 0;
	create_thread( &m_seek_threadhandle , &threadid , seekto , this , 0 );
	VOLOGI( "seek thread_id is : %d" , threadid);

    m_is_init = VO_FALSE;

	return VO_TRUE;
}

VO_BOOL vo_PD_manager::perpare_parser()
{
	if(m_ptrpool)
	{
		delete []m_ptrpool;
		m_ptrpool = 0;
		m_poolsize = 0;
		m_downloader.set_comparedata(0,0);
	}
	
		
	//if file size is small than 64K, then we should not try to read 64k data.
	VO_U32 format_check_size = GUESSFILETYPE_BUFFERSIZE;
	if( m_filesize_byserver > 32 * 1024 )
	{
		format_check_size = ( m_filesize_byserver >= GUESSFILETYPE_BUFFERSIZE ? GUESSFILETYPE_BUFFERSIZE : 32 * 1024 );
	}
	else if ( m_filesize_byserver > 0 )
	{
		format_check_size = (VO_U32)m_filesize_byserver;
	}
	m_ptrpool = new VO_BYTE[format_check_size];
	if( !m_ptrpool )
	{
		VOLOGE( "new obj failed" );
		return VO_FALSE;
	}

	VOLOGI( "+Skip ID3" );
	while( SkipID3v2Data( m_buffer_stream ) ){}
	VOLOGI( "-Skip ID3" );

	VOLOGI( "+get buffer to perpare parser!" );
	VO_S64 size = m_buffer_stream->read( m_ptrpool , format_check_size );
	VOLOGI( "-get buffer to perpare parser!%d   %lld" , format_check_size , size );

	m_poolsize = (int)size;

	m_buffer_stream->seek( 0 , VO_FILE_BEGIN );

    
	PD_MEDIA_FORMAT mf = get_rightlibrary( m_ptrpool , (VO_S32)size , m_dllloader.m_szDllFile , m_dllloader.m_szAPIName );
	if( mf == PD_MEDIA_UNKNOWN )
	{
		long err_type = E_PD_MEDIATYPE_UNKNOWN;
		notify( VO_PD_EVENT_NOT_APPLICABLE_MEDIA ,&err_type);
		VOLOGE("can not find right file parser for this file!");
		return VO_FALSE;
	}

	if(mf == PD_MEDIA_PLS || mf == PD_MEDIA_M3U)
	{
		m_is_shoutcast = VO_TRUE;
		//try to parse the data follow it
		if(m_ptr_playlist_parser)
		{
			delete m_ptr_playlist_parser;
			m_ptr_playlist_parser = 0;
		}

		if(mf == PD_MEDIA_PLS)
		{
			m_ptr_playlist_parser =  new vo_playlist_pls_parser;
		}
		else
		{
			m_ptr_playlist_parser = new vo_playlist_m3u_parser;
		}
		if( !m_ptr_playlist_parser )
		{
			VOLOGE( "new obj failed" );
			return VO_FALSE;
		}
		m_ptr_playlist_parser->set_source(m_ptrpool);
		VO_BOOL res = m_ptr_playlist_parser->parse();

		//get the new url is what we need.
		if(res)
		{
			if(!m_ptr_playlist_parser->get_url(1, m_url))
			{
				//we have been failed to retrieve a new url from playlist, so it is invalid shoutcast playlist.
				m_is_shoutcast = VO_FALSE;
			}
		}
		else
		{
			m_is_shoutcast = VO_FALSE;
		}
			
		//we should tell the caller that the lib is not successfully load in this loop
		return VO_FALSE;
	}

	//ensure we read data from the begin of the file
	size = m_buffer_stream->read( m_ptrpool , format_check_size );
	m_poolsize = (int)size;
	//set data to be referenced for later range request support check.
	m_downloader.set_comparedata( m_ptrpool, m_poolsize);

	m_buffer_stream->seek( 0 , VO_FILE_BEGIN );

	//processing with audio file
	if( mf == PD_MEDIA_MP3 )
		m_streamtype = StreamType_NoneStop;
	
	if(m_dllloader.LoadLib(NULL) == 0)
	{
		long err_type = E_PD_FILE_PARSER_LIB_LOADFAIL;
		notify( VO_PD_EVENT_NOT_APPLICABLE_MEDIA ,&err_type);
		VOLOGE( "load library %s fail!" , m_dllloader.m_szDllFile );
		return VO_FALSE;
	}

	pvoGetReadAPI getapi = (pvoGetReadAPI)m_dllloader.m_pAPIEntry;

	if( getapi )
		getapi( &m_parser_api , 0 );
	else
		return VO_FALSE;
//#endif

	return VO_TRUE;
}

VO_VOID vo_PD_manager::arrange_headdata( headerdata_info * ptr_info )
{
	for( VO_S32 i = 0 ; i < ptr_info->arraysize ; i++ )
	{
		ptr_info->ptr_array[i].size += ptr_info->reserved;

		if( m_buffer.get_filesize() != -1 && ptr_info->ptr_array[i].size + ptr_info->ptr_array[i].physical_pos > m_buffer.get_filesize() )
			ptr_info->ptr_array[i].size = m_buffer.get_filesize() - ptr_info->ptr_array[i].physical_pos;
	}

	for( VO_S32 i = 0 ; i < ptr_info->arraysize ; i++ )
	{
		VO_S64 minpos = ptr_info->ptr_array[i].physical_pos;
		VO_S32 index = i;

		for( VO_S32 j = i + 1 ; j < ptr_info->arraysize ; j++ )
		{
			if( ptr_info->ptr_array[j].physical_pos < minpos )
			{
				minpos = ptr_info->ptr_array[j].physical_pos;
				index = j;
			}
		}

		if( i != index )
		{
			headerdata_element temp;
			temp = ptr_info->ptr_array[i];
			ptr_info->ptr_array[i] = ptr_info->ptr_array[index];
			ptr_info->ptr_array[index] = temp;
		}
	}

	volatile VO_S32 array_size = ptr_info->arraysize;

	for( VO_S32 i = 0 ; i < array_size ; i++ )
	{
		VO_S64 end_pos = ptr_info->ptr_array[i].physical_pos + ptr_info->ptr_array[i].size;

		VO_S32 j = i + 1;
		for(  ; j < array_size ; j++ )
		{
			if( end_pos < ptr_info->ptr_array[j].physical_pos )
				break;
		}

		if( j != i + 1 )
		{
			VO_S64 max_end_pos = end_pos;

			for( VO_S32 k = i + 1 ; k < j ; k++ )
			{
				if( ptr_info->ptr_array[k].physical_pos + ptr_info->ptr_array[k].size > max_end_pos )
					max_end_pos = ptr_info->ptr_array[k].physical_pos + ptr_info->ptr_array[k].size;
			}

			ptr_info->ptr_array[i].size = max_end_pos - ptr_info->ptr_array[i].physical_pos;

			for( VO_S32 k = j ; k < array_size ; k++ )
			{
				ptr_info->ptr_array[ i + k - j + 1 ] = ptr_info->ptr_array[k];
			}

			array_size = array_size - ( j - 1 - i );
		}
	}

	ptr_info->arraysize = array_size;
}


VO_VOID vo_PD_manager::http_verify_callback( int id, void* pParam)
{
	if( m_http_verifycb.HTTP_Callback )
	{
		m_http_verifycb.HTTP_Callback( m_http_verifycb.hHandle , id, pParam );
	}
}

VO_VOID vo_PD_manager::http_event_callback( int id, VO_U32 pParam1 , VO_U32 pParam2)
{
	if( m_http_eventcb.SendEvent )
	{
		m_http_eventcb.SendEvent( m_http_eventcb.pUserData , id, pParam1 , pParam2 );
	}
}

VO_VOID vo_PD_manager::notify( long EventCode, long * EventParam )
{
	if( m_is_source2 )
	{
		if( m_notifier2.SendEvent )
		{
			m_notifier2.SendEvent( m_notifier2.pUserData, (VO_U32)EventCode , (VO_U32)EventParam , 0 );
		}
	}
	else
	{
		if( m_notifier.funtcion )
		{
			m_notifier.funtcion( EventCode , EventParam , (long	*)m_notifier.userdata );
		}
	}
}

void vo_PD_manager::thread_function()
{
	while( !m_b_stop_getsample )
	{
		m_sem_emptysample.Down();

		VO_BOOL bgotdata = VO_FALSE;
		while( !bgotdata )
		{
			if( m_b_stop_getsample || m_is_seeking )
			{
				return;
			}

			for( VO_S32 i = 0 ; i < m_tracks && m_ptr_sample ; i++ )
			{
				//we will try to ask get sample in thread only when it is request by VOME. else we will skip the process to save time.
				if( !m_getsample_flag[i] || m_bsourcend[i] )
				{
					continue;
				}

				if( m_ptr_sample[i].Size == 0)
				{
					voCAutoLock lock( &m_getsamplelock[i] );
					memset( &m_ptr_sample[i] , 0 , sizeof( VO_SOURCE_SAMPLE ) );
					memset( &m_ptr_bottom_sample[i] , 0 , sizeof( VO_SOURCE_SAMPLE ) );
					
					if( m_bdrop_frame[i] )
					{
						m_ptr_sample[i].Time = m_ptr_timestamp[i];
						m_bdrop_frame[i] = 0;
					}
					else
					{
						//when the set in timestamp is 0 , it will just get next sample, not sample will be droped.
						m_ptr_sample[i].Time = 0;
					}
					
					m_getsample_ret[i] = get_sample_internal( i , &(m_ptr_sample[i]) );
					memcpy( &m_ptr_bottom_sample[i] , &m_ptr_sample[i] , sizeof(VO_SOURCE_SAMPLE) );

					if( m_getsample_ret[i] == VO_ERR_SOURCE_OK )
						m_ptr_timestamp[i] = m_ptr_sample[i].Time;

					//we have to make sure got track data from source,else when one track size == 0 twice, it will cause problem of dead lock
					if(m_ptr_sample[i].Size > 0)
					{
						if( m_max_sampsize[i] && ( (m_ptr_sample[i].Size & 0x7FFFFFFF) > m_max_sampsize[i]) )
						{
							//do nothing , since the size is invalid at all.
							;
						}
						else
						{
							//ensure we will handle the buffer, since parser won't be responsible to the buffer after the call of getsample on parser.
							memcpy( m_ptr_bottom_buffer[i] , m_ptr_sample[i].Buffer , m_ptr_sample[i].Size & 0x7FFFFFFF );
						}
						bgotdata = VO_TRUE;
						m_sem_getsample[i].Up();
						break;
					}
					else if(m_getsample_ret[i] != VO_ERR_SOURCE_OK)
					{
						m_getsample_flag[i] = 0;
						m_sem_getsample[i].Up();
												
						//mark source end flag
						m_bsourcend[i] = 1;
					//	return;
					}
				}
				else
				{
					//it means the sample has not been read by consumer
#ifdef WINCE
					voOS_Sleep( 2 );
#endif
				}
			}
		}

	}
}

void vo_PD_manager::stop_getsample_thread()
{
	m_b_stop_getsample = VO_TRUE;
    m_sem_emptysample.Up();
	stop();
    m_sem_emptysample.Reset();

    for( VO_S32 i = 0 ; i < m_tracks && m_ptr_sample ; i++ )
    {
        voCAutoLock lock( &m_getsamplelock[i] );
        m_getsample_ret[i] = VO_ERR_NONE;
        m_ptr_sample[i].Size = 0;
		m_ptr_bottom_sample[i].Size = 0;
        m_sem_getsample[i].Reset();
    }

    m_sem_emptysample.Reset();
    for( VO_S32 i = 0 ; i < m_tracks ; i++ )
        m_sem_emptysample.Up();
}

void vo_PD_manager::stop_seek_thread()
{
	m_is_stopseek = VO_TRUE;
	m_sem_seekcount.Up();

	if( m_seek_threadhandle )
	{
		wait_thread_exit( m_seek_threadhandle );
	}

	m_sem_seekcount.Reset();
	m_seek_threadhandle = 0;
	m_is_stopseek = VO_FALSE;
}

void vo_PD_manager::stop_startbuffering_thread()
{
	m_is_stopbuffering_thread = VO_TRUE;
	if( m_startbuffering_handle )
	{
		wait_thread_exit( m_startbuffering_handle );
	}

	m_startbuffering_handle = 0;
	m_is_stopbuffering_thread = VO_FALSE;
}


void vo_PD_manager::do_startbuffering()
{
	VOLOGI( "+VO_EVENT_BUFFERING_BEGIN");
	m_buffering_percent = 0;
	notify( VO_EVENT_BUFFERING_BEGIN , &m_buffering_percent );
	m_is_buffering = VO_TRUE;
	VOLOGI( "-VO_EVENT_BUFFERING_BEGIN");

	//unsigned int threadid = 0;
	//create_thread( &m_startbuffering_handle , &threadid , buffering_func , this , 0 );
	//VOLOGI( "start buffering thread_id is : %d" , threadid);

	VO_DOWNLOAD_INFO info;
	m_downloader.get_dowloadinfo( &info );

	VO_S64 buffer_end_pos;

	if( m_bitrate > 0 )
		buffer_end_pos = (VO_S64)(m_bitrate / 8. * m_PD_param.nBufferTime);
	else
		buffer_end_pos = 50000;

	VOLOGI("Try to Buffer from %lld to %lld , Current download pos: %lld" , (VO_S64)0 , buffer_end_pos , info.cur_download_pos );

	VO_S64 buffer_start_org_pos = info.cur_download_pos;


	while( m_downloader.is_downloading() && info.cur_download_pos < buffer_end_pos && !m_is_pause && !m_is_seeking && !m_is_to_exit && !m_is_stopbuffering_thread)
	{
		VO_S32 sleeptime;

		VO_S64 distance = buffer_end_pos - info.cur_download_pos;

		if( buffer_end_pos - buffer_start_org_pos )
		{
			m_buffering_percent = (int)((float)( info.cur_download_pos - buffer_start_org_pos ) / (float)( buffer_end_pos - buffer_start_org_pos ) * 100. );
		}
		else
			break;

		if( info.is_speed_reliable )
		{
			sleeptime = (VO_S32)(distance / (float)info.average_speed * 1000.);
			VOLOGI("Buffering............. Need Buffer Time: %d    Download Speed: %d KBps Download pos: %lld" , sleeptime , (VO_S32)(info.average_speed/1024.) , info.cur_download_pos );
		}
		else
		{
			if( m_bitrate > 0 )
				sleeptime = (VO_S32)(distance / (m_bitrate / 8.) / 2. * 1000.);
			else
				sleeptime = 1000;
			VOLOGI( "Buffering............. Need Buffer Time: %d Download pos: %lld" , sleeptime , info.cur_download_pos );
		}

		
		if( sleeptime <= 200 )
		{
			VO_S32 i = 4;
			do
			{
				voOS_Sleep( 50 );
			}while( i-- > 0 && m_downloader.is_downloading() && !m_is_pause && !m_is_stopbuffering_thread);
		}
		else
		{
			for( VO_S32 i = 0 ; i < sleeptime / 50 && i < 20 && m_downloader.is_downloading() && !m_is_pause && !m_is_stopbuffering_thread; i++ )
			{
				voOS_Sleep( 50 );
			}
		}

		m_downloader.get_dowloadinfo( &info );
	}

	VOLOGI( "+VO_EVENT_BUFFERING_END");
	m_buffering_percent = 100;
	notify( VO_EVENT_BUFFERING_END , &m_buffering_percent );
	m_is_buffering = VO_FALSE;
	VOLOGI( "-VO_EVENT_BUFFERING_END");
	
}

unsigned int vo_PD_manager::buffering_func(void * ptr_obj)
{
	VOLOGE("**********Thread_Created! name: PD_StartBuffering");
	set_threadname( (char *) "PD_SBuf" );
	vo_PD_manager* ptr = ( vo_PD_manager*)ptr_obj;
	ptr->dobuffering_internal();
	VOLOGE("**********Thread_End! name:PD_StartBuffering");
	exit_thread();
	return 0;
}


VO_VOID vo_PD_manager::dobuffering_internal()
{
	VO_DOWNLOAD_INFO info;
	m_downloader.get_dowloadinfo( &info );

	VO_S64 buffer_end_pos;

	if( m_bitrate > 0 )
	{
		buffer_end_pos = (VO_S64)(m_bitrate / 8. * m_PD_param.nBufferTime);
	}
	else
	{
		buffer_end_pos = 50000;
	}

	if( m_filesize_byserver > 0 )
	{
		buffer_end_pos = ( buffer_end_pos > m_filesize_byserver ) ? m_filesize_byserver : buffer_end_pos;
	}

	VOLOGI("Try to Buffer from %lld to %lld , Current download pos: %lld" , (VO_S64)0 , buffer_end_pos , info.cur_download_pos );

	VO_S64 buffer_start_org_pos = info.cur_download_pos;


	while( m_downloader.is_downloading() && info.cur_download_pos < buffer_end_pos && !m_is_pause && !m_is_seeking && !m_is_to_exit && !m_is_stopbuffering_thread)
	{
		VO_S32 sleeptime;

		VO_S64 distance = buffer_end_pos - info.cur_download_pos;

		if( buffer_end_pos - buffer_start_org_pos )
		{
			m_buffering_percent = (int)((float)( info.cur_download_pos - buffer_start_org_pos ) / (float)( buffer_end_pos - buffer_start_org_pos ) * 100. );
		}
		else
			break;

		if( info.is_speed_reliable )
		{
			sleeptime = (VO_S32)(distance / (float)info.average_speed * 1000.);
			VOLOGI( "Buffering............. Need Buffer Time: %d    Download Speed: %d KBps Download pos: %lld" , sleeptime , (VO_S32)(info.average_speed/1024.) , info.cur_download_pos );
		}
		else
		{
			if( m_bitrate > 0 )
				sleeptime = (VO_S32)(distance / (m_bitrate / 8.) / 2. * 1000.);
			else
				sleeptime = 1000;
			VOLOGI( "Buffering............. Need Buffer Time: %d Download pos: %lld" , sleeptime , info.cur_download_pos );
		}

		
		if( sleeptime <= 200 )
		{
			VO_S32 i = 4;
			do
			{
				voOS_Sleep( 50 );
			}while( i-- > 0 && m_downloader.is_downloading() && !m_is_pause && !m_is_stopbuffering_thread);
		}
		else
		{
			for( VO_S32 i = 0 ; i < sleeptime / 50 && i < 20 && m_downloader.is_downloading() && !m_is_pause && !m_is_stopbuffering_thread; i++ )
			{
				voOS_Sleep( 50 );
			}
		}

		m_downloader.get_dowloadinfo( &info );
	}

	VOLOGI( "+VO_EVENT_BUFFERING_END");
	m_buffering_percent = 100;
	notify( VO_EVENT_BUFFERING_END , &m_buffering_percent );
	m_is_buffering = VO_FALSE;
	VOLOGI( "-VO_EVENT_BUFFERING_END");
}

unsigned int vo_PD_manager::seekto( void * ptr_obj )
{
	VOLOGE( "**********Thread_Created! name: PD_Seek");
	set_threadname( (char *) "PD_Seek" );
	vo_PD_manager* ptr = ( vo_PD_manager*)ptr_obj;
	ptr->moveto_internal();
	VOLOGE( "**********Thread_End! name:PD_Seek");
	exit_thread();
	return 0;
}

VO_VOID vo_PD_manager::moveto_internal()
{
	while( 1 )
	{
		VO_S64 pos;

		VOLOGI( "+m_sem_seekcount.Down()" );
		m_sem_seekcount.Down();
		VOLOGI( "-m_sem_seekcount.Down()" );
		m_is_seeking = VO_TRUE;

		{
			voCAutoLock lock(&m_seekpos_lock);
			pos = m_seekpos;
			m_sem_seekcount.Reset();
		}

		if( m_is_stopseek )
		{
			m_is_seeking = VO_FALSE;
			break;
		}

		VOLOGI("++++++++++++++++Seek to : %lld" , pos );

		VOLOGI( "+stop_getsample_thread" );
		stop_getsample_thread();
		VOLOGI( "-stop_getsample_thread" );

		VO_FILE_MEDIATIMEFILEPOS mediapos;
		mediapos.nFlag = 1;
		mediapos.llMediaTime = pos;

		VO_U32 ret = m_parser_api.GetSourceParam( m_fileparser , VO_PID_FILE_GetSEEKPOSBYTIME , &mediapos );

		if( ret == 0 )
		{
			VOLOGE( "Got Accurate Seek Pos: %lld" , mediapos.llFilePos );
			VO_BYTE b;

			m_buffer.read( mediapos.llFilePos , &b , 1 , VO_FALSE );
		}

		
		if(m_videotrack != -1)
		{
			m_parser_api.SetPos( m_fileparser , m_videotrack , &pos );
			m_ptr_timestamp[m_videotrack] = pos;

			VOLOGI("After setpos to videotrack %d %lld" , m_videotrack , pos );

			for( VO_S32 i = 0 ; i < m_tracks ; i++ )
			{
				if( i != m_videotrack )
				{
					if( m_seekmode == TIME_SeekModeFast )
					{
						if(m_seekpos == 0)
						{
							//if it is just at the begin of playback, we should not sync audio with video , else first audio sample may be droped, issue 9533
							VO_S64 audiopos = 0;
							m_parser_api.SetPos( m_fileparser , i , &audiopos );
							m_ptr_timestamp[i] = audiopos;
							VOLOGI("Fast seek mode After setpos to audiotrack %d apos: %lld" , i , audiopos );
						}
						else
						{
							//set audio pos sync with video pos
							m_parser_api.SetPos( m_fileparser , i , &pos );
							m_ptr_timestamp[i] = pos;
							VOLOGI("Fast seek mode After setpos to audiotrack %d pos: %lld" , i , pos );
						}
					}
					else if(m_seekmode == TIME_SeekModeAccurate) 
					{
						//set audio pos with the accurate pos user specified.
						pos = m_seekpos;
						m_parser_api.SetPos( m_fileparser , i , &pos );
						m_ptr_timestamp[i] = pos;
						VOLOGI("Accurate seek mode After setpos to audiotrack %d %lld" , i , pos );
					}
				}
			}
		}
		else
		{
			//it tell us that only audio track is available
			m_parser_api.SetPos( m_fileparser , 0 , &pos );
			m_ptr_timestamp[0] = pos;
		}

		if(m_seekmode == TIME_SeekModeFast)
		{
			//we will tell vome the key frame pos
			VOLOGI( "notify final seekpos to VOME fast" );
		}
		else if(m_seekmode == TIME_SeekModeAccurate)
		{
			//we will tell vome the accurate frame pos it specified
			VOLOGI( "notify final seekpos to VOME accurate" );
			pos = m_seekpos;
		}

		//notify final seek position to the caller
		if( m_is_source2 )
		{
			if( m_notifier2.SendEvent )
			{
				VO_U32 nResult = VO_RET_SOURCE2_OK;
				m_notifier2.SendEvent( m_notifier2.pUserData, (VO_U32)VO_EVENTID_SOURCE2_SEEKCOMPLETE , (VO_U32)&pos , (VO_U32)&nResult );
			}
		}
		else
		{
			notify( VO_PD_EVENT_SEEKFINALPOS ,(long*) &pos );
		}

		for( VO_S32 i = 0 ; i < m_tracks ; i++ )
		{
			m_ptr_sample[i].Size = 0;
			m_ptr_bottom_sample[i].Size = 0;
			m_getsample_ret[i] = 0;
		}

		m_is_seeking = VO_FALSE;
	}
	VOLOGI("Seek thread exit!" );
}

VO_BOOL vo_PD_manager::load_static_parser_lib(int nStreamType, void** ppParserApi)
{
#ifdef _IOS
	int nParserType = VO_FILE_FFUNKNOWN;
	
	if(PD_MEDIA_MP4 == nStreamType)
	{
		nParserType = VO_FILE_FFMOVIE_MP4;
	}
	else if(PD_MEDIA_ASF == nStreamType)
	{
		nParserType = VO_FILE_FFMOVIE_ASF;
	}
	else if(PD_MEDIA_AVI == nStreamType)
	{
		nParserType = VO_FILE_FFMOVIE_AVI;
	}
	else if(PD_MEDIA_AWB == nStreamType)
	{
		nParserType = VO_FILE_FFAUDIO_AWB;
	}
	else if(PD_MEDIA_AMR == nStreamType)
	{
		nParserType = VO_FILE_FFAUDIO_AMR;
	}
	else if(PD_MEDIA_MP3 == nStreamType)
	{
		nParserType = VO_FILE_FFAUDIO_MP3;
	}
	else if(PD_MEDIA_AAC == nStreamType)
	{
		nParserType = VO_FILE_FFAUDIO_AAC;
	}
	else if(PD_MEDIA_WAV == nStreamType)
	{
		nParserType = VO_FILE_FFAUDIO_WAV;
	}
	else if(PD_MEDIA_FLV == nStreamType)
	{
		nParserType = VO_FILE_FFMOVIE_FLV;
	}
	else if(PD_MEDIA_MKV == nStreamType)
	{
		nParserType = VO_FILE_FFMOVIE_MKV;
	}
	else if(PD_MEDIA_OGG == nStreamType)
	{
		nParserType = VO_FILE_FFAUDIO_OGG;
	}
	else if(PD_MEDIA_REAL == nStreamType)
	{
		nParserType = VO_FILE_FFMOVIE_REAL;
	}
	else
		return VO_FALSE;
	
	return (1==LoadParserLib(nParserType, ppParserApi))?VO_TRUE:VO_FALSE;
#else
	return VO_FALSE;
#endif
}

