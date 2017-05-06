//#include "StdAfx.h"
#include "vo_PD_manager.h"
#include "voOSFunc.h"
#include "voLog.h"
#include "fileformat_checker.h"

#define LOG_TAG "voCOMXAudioSink"

#define GUESSFILETYPE_BUFFERSIZE (32 * 1024)

typedef VO_S32 (VO_API *pvoGetReadAPI)(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag);

#ifndef _LIB
unsigned long g_dwFRModuleID = VO_INDEX_SRC_HTTP;
#endif

VO_PTR VO_API stream_open(VO_FILE_SOURCE * pSource)
{
	vo_PD_manager * ptr_manager = ( vo_PD_manager * )pSource->nReserve;
	return ptr_manager->get_stream();
}

VO_S32 VO_API stream_read(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)
{
	vo_buffer_stream * ptr_buffer = (vo_buffer_stream*)pFile;
	return ptr_buffer->read( (VO_PBYTE)pBuffer , uSize );
}

VO_S32 VO_API stream_write(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)
{
	return -1;
}

VO_S32 VO_API stream_flush(VO_PTR pFile)
{
	return -1;
}

VO_S64 VO_API stream_seek(VO_PTR pFile, VO_S64 nPos, VO_FILE_POS uFlag)
{
	vo_buffer_stream * ptr_buffer = (vo_buffer_stream*)pFile;
	return ptr_buffer->seek( nPos , uFlag );
}

VO_S64 VO_API stream_size(VO_PTR pFile)
{
	vo_buffer_stream * ptr_buffer = (vo_buffer_stream*)pFile;
	return ptr_buffer->get_filesize();
}

VO_S64 VO_API stream_save(VO_PTR pFile)
{
	return -1;
}

VO_S32 VO_API stream_close(VO_PTR pFile)
{
	vo_buffer_stream * ptr_buffer = (vo_buffer_stream*)pFile;
	ptr_buffer->seek( 0 , VO_FILE_BEGIN );
	return 0;
}


vo_PD_manager::vo_PD_manager(void)
:m_is_init( VO_FALSE )
,m_is_to_exit( VO_FALSE )
,m_duration(0)
,m_firstseektrack(-1)
,m_is_pause( VO_FALSE )
,m_buffer_stream(0)
,m_is_to_break_buffering( VO_FALSE )
,m_is_start_buffering( VO_FALSE )
,m_seek_status( SEEKSTATUS_SEEKEND )
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

	m_framegot[0] = 0;
	m_framegot[1] = 0;
}

vo_PD_manager::~vo_PD_manager(void)
{
	close();
}

CALLBACK_RET vo_PD_manager::received_data( VO_S64 physical_pos , VO_PBYTE ptr_buffer , VO_S32 size )
{
	VO_S64 writesize = m_buffer.write( physical_pos , ptr_buffer , size );

	if( writesize == size )
		return CALLBACK_OK;
	else
		return CALLBACK_BUFFER_FULL;
}

CALLBACK_RET vo_PD_manager::download_notify( DOWNLOAD_CALLBACK_NOTIFY_ID id , VO_PTR ptr_data )
{
	switch( id )
	{
	case DOWNLOAD_END:
		m_buffer.set_download_stoped( VO_TRUE );
		break;
	case DOWNLOAD_START:
		m_buffer.set_download_stoped( VO_FALSE );
		break;
	case DOWNLOAD_DEBUG_STRING:
		{
			char * ptr_str = (char*)ptr_data;
			VOLOGE( "DOWNLOAD_DEBUG_STRING : %s" , ptr_str );
		}
		break;
	case DOWNLOAD_FILESIZE:
	case DOWNLOAD_TO_FILEEND:
		{
			VO_S64 * ptr_filesize = ( VO_S64 * )ptr_data;
			m_buffer.set_filesize( *ptr_filesize );
		}
		break;
	case DOWNLOAD_ERROR:
		{
			VOLOGE("Download Error!");
			notify( VO_EVENT_SOCKET_ERR , (long *)ptr_data );
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
			VOLOGI( "cache buffer need download  pos: %d   size: %d" , (VO_S32)ptr_getcache->want_physical_pos , (VO_S32)ptr_getcache->want_size );
			m_downloader.start_download( ptr_getcache->want_physical_pos , ptr_getcache->want_size , VO_FALSE );
		}
		break;
	case BUFFER_LOW:
		{
			VO_BUFFER_NEED * ptr_need = (VO_BUFFER_NEED *)ptr_data;

			VOLOGI( "Buffer low! download start: %d\r\n" , (VO_S32)ptr_need->want_physical_pos );

			if( !m_downloader.is_downloading() )
			{
				m_downloader.start_download( ptr_need->want_physical_pos );
			}
		}
		break;
	case BUFFER_LACK:
		{
// 			if( m_is_to_exit || m_is_pause || m_is_to_break_buffering )
// 			{
// 				if( m_is_to_exit )
// 					VOLOGE("m_is_to_exit");
// 				
// 				if( m_is_pause )
// 					VOLOGE( "m_is_pause" );
// 
// 				if( m_is_to_break_buffering )
// 					VOLOGE( "m_is_to_break_buffering" );
// 
// 				//m_is_pause = VO_FALSE;
// 				VOLOGI("BUFFER_CALLBACK_SHOULD_STOP");
// 				return BUFFER_CALLBACK_SHOULD_STOP;
// 			}

			VO_BUFFER_NEED * ptr_need = (VO_BUFFER_NEED *)ptr_data;

			return buffer_notify_bufferlack( ptr_need );
		}
		break;
	}

	return BUFFER_CALLBACK_OK;
}

BUFFER_CALLBACK_RET vo_PD_manager::buffer_notify_bufferlack( VO_BUFFER_NEED * ptr_need )
{
	if( m_downloader.is_downloading() )
	{ 
		VO_DOWNLOAD_INFO info;
		m_downloader.get_dowloadinfo( &info );

		if( m_downloader.is_support_seek() )
		{
			VOLOGI( "Buffer lack: lack_pos %lld lack_size %lld ; Current Downloading: start_pos %lld current_pos %lld SEEKStatue: %d" , 
				ptr_need->want_physical_pos , ptr_need->want_size , info.start_download_pos , info.cur_download_pos , (VO_S32)m_seek_status );

			if( info.start_download_pos <= ptr_need->want_physical_pos && ( info.download_size == DOWNLOADTOEND || info.start_download_pos + info.download_size >= ptr_need->want_physical_pos || m_seek_status <= SEEKSTATUS_TWOTRACKGOT ) )
			{
				VO_S64 delta = ptr_need->want_physical_pos + ptr_need->want_size - info.cur_download_pos;

				if( m_is_init )
				{
					VOLOGI("m_is_init");
					if( 0/*info.is_speed_reliable*/ )
					{
						VO_S32 timecost;
						
						if( info.average_speed ) 
							timecost = (VO_S32)(delta / info.average_speed);
						else
							timecost = 0;

						VOLOGI( "Buffer Lack Time Cost: %d" , timecost );
						if( timecost >= 20 )
						{
							m_downloader.stop_download();
							if( !m_downloader.start_download( ptr_need->want_physical_pos ) )
								return BUFFER_CALLBACK_ERROR;
						}
					}
					else
					{
						if( delta >= 1024 * 2048/* || delta <= -1024 * 2048*/ )
						{
							m_downloader.stop_download();
							if( !m_downloader.start_download( ptr_need->want_physical_pos ) )
								return BUFFER_CALLBACK_ERROR;
						}
					}
				}
				else
				{
					if( 0/*info.is_speed_reliable*/ )
					{
						VO_S32 timecost;

						if( info.average_speed )
							timecost = (VO_S32)(delta / info.average_speed);
						else
							timecost = 0;

						VOLOGI( "Buffer Lack Time Cost: %d" , timecost );
						if( timecost >= 5 )
						{
							m_downloader.stop_download();
							if( !m_downloader.start_download( ptr_need->want_physical_pos ) )
								return BUFFER_CALLBACK_ERROR;
						}
						else
						{
							m_downloader.get_dowloadinfo( &info );

							VO_S64 buffer_end_pos;
							
							if( m_bitrate > 0 )
								buffer_end_pos = ptr_need->want_physical_pos + m_bitrate / 8. * m_PD_param.nBufferTime;
							else
								buffer_end_pos = ptr_need->want_physical_pos + m_buffertimewithoutbitrate;

							while( m_downloader.is_downloading() && info.cur_download_pos < buffer_end_pos && !m_is_pause && m_is_start_buffering )
							{
								VO_S32 sleeptime;

								VO_S64 distance = buffer_end_pos - info.cur_download_pos;

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
									voOS_Sleep( 200 );
								}
								else
								{
									for( VO_S32 i = 0 ; i < sleeptime / 100 && i < 20 && m_downloader.is_downloading() && !m_is_pause ; i++ )
									{
										voOS_Sleep( 100 );
									}
								}
								m_downloader.get_dowloadinfo( &info );
							}
						}
					}
					else
					{
						if( delta >= 1024 * 500  && m_seek_status > SEEKSTATUS_TWOTRACKGOT/* || delta <= -1024 * 500*/ )
						{
							m_downloader.stop_download();
							if( !m_downloader.start_download( ptr_need->want_physical_pos ) )
								return BUFFER_CALLBACK_ERROR;
						}
						else
						{
							m_downloader.get_dowloadinfo( &info );

							VO_S64 buffer_end_pos;

							if( m_bitrate > 0 )
								buffer_end_pos = ptr_need->want_physical_pos + m_bitrate / 8. * m_PD_param.nBufferTime;
							else
								buffer_end_pos = ptr_need->want_physical_pos + m_buffertimewithoutbitrate;

							VO_S64 buffer_start_org_pos = info.cur_download_pos;

							if( m_is_start_buffering )
							{
								VOLOGE( "+VO_EVENT_BUFFERING_BEGIN" );
								notify( VO_EVENT_BUFFERING_BEGIN , 0 );
								VOLOGE( "-VO_EVENT_BUFFERING_BEGIN" );

								int percent = 0;
								notify( VO_EVENT_BUFFERING_PERCENT , (long*)&percent );
							}

							while( m_downloader.is_downloading() && info.cur_download_pos < buffer_end_pos && !m_is_pause && m_is_start_buffering )
							{
								VO_S32 sleeptime;

								VO_S64 distance = buffer_end_pos - info.cur_download_pos;

								if( buffer_end_pos - buffer_start_org_pos )
								{
									int percent = (int)((float)( info.cur_download_pos - buffer_start_org_pos ) / (float)( buffer_end_pos - buffer_start_org_pos ) * 100. );
									VOLOGE( "VO_EVENT_BUFFERING_PERCENT %d" , percent );
									notify( VO_EVENT_BUFFERING_PERCENT , (long*)&percent );
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

								if( sleeptime <= 200 )
								{
									voOS_Sleep( 200 );
								}
								else
								{
									for( VO_S32 i = 0 ; i < sleeptime / 100 && i < 20 && m_downloader.is_downloading() && !m_is_pause ; i++ )
									{
										voOS_Sleep( 100 );
									}
								}

								m_downloader.get_dowloadinfo( &info );
							}

							if( m_is_start_buffering )
							{
								int percent = 100;
								notify( VO_EVENT_BUFFERING_PERCENT , (long*)&percent );
								VOLOGE("VO_EVENT_BUFFERING_END");
								notify( VO_EVENT_BUFFERING_END , 0 );
							}
						}
					}
				}
			}
			else
			{
				VOLOGI( "Buffer lack: lack_pos %d lack_size %d" , (VO_S32)ptr_need->want_physical_pos , (VO_S32)ptr_need->want_size );
				m_downloader.stop_download();
				if( !m_downloader.start_download( ptr_need->want_physical_pos ) )
					return BUFFER_CALLBACK_ERROR;
			}
		}
		else	//do not support seek
		{
			VOLOGI( "Do not support seek! Current Download Pos: %d" , (VO_S32)info.start_download_pos );
			if( info.start_download_pos <= ptr_need->want_physical_pos )
			{
				voOS_Sleep( 1000 );
			}
			else
			{
				m_downloader.stop_download();
				if( !m_downloader.start_download( ptr_need->want_physical_pos ) )
					return BUFFER_CALLBACK_ERROR;
			}
		}
	}
	else
	{
		VOLOGI( "Buffer lack: lack_pos %lld lack_size %lld" , ptr_need->want_physical_pos , ptr_need->want_size );
		if( !m_downloader.start_download( ptr_need->want_physical_pos ) )
			return BUFFER_CALLBACK_ERROR;

		if( m_seek_status < SEEKSTATUS_SEEKEND )
		{
			m_framegot[0] = 0;
			m_framegot[1] = 0;
			m_seek_status = SEEKSTATUS_SEEKSTART;
		}

		voOS_Sleep( 1000 );
	}

	if( m_is_to_exit || m_is_pause || m_is_to_break_buffering )
	{
		VOLOGI("BUFFER_CALLBACK_SHOULD_STOP");
		return BUFFER_CALLBACK_SHOULD_STOP;
	}

	return BUFFER_CALLBACK_OK;
}

VO_BOOL vo_PD_manager::open( VO_SOURCE_OPENPARAM * pParam )
{
	VOLOGE("vo_PD_manager::open");
	close();

//for htc test
	FILE * fp = fopen( "/system/etc/wait.cfg" , "r" );

	if( fp )
	{
		fseek( fp , 0 , SEEK_END );

		VO_S32 size = ftell( fp );

		VO_CHAR temp[10];
		memset( temp , 0 , 10 );

		if( size > 9 )
		{
			m_waitbuffertime = 1000;
		}
		else
		{
			fseek( fp , 0 , SEEK_SET );
			fread( temp , 1 , size , fp );
			m_waitbuffertime = atoi( temp );
		}

		fclose( fp );
	}
	else
		m_waitbuffertime = 1000;

	fp = fopen( "/system/etc/lackbuffer.cfg" , "r" );

	if( fp )
	{
		fseek( fp , 0 , SEEK_END );

		VO_S32 size = ftell( fp );

		VO_CHAR temp[10];
		memset( temp , 0 , 10 );

		if( size > 9 )
		{
			m_buffertimewithoutbitrate = 5 * 1024 * 1024;
		}
		else
		{
			fseek( fp , 0 , SEEK_SET );
			fread( temp , 1 , size , fp );
			m_buffertimewithoutbitrate = atoi( temp );
		}

		fclose( fp );
	}
	else
		m_buffertimewithoutbitrate = 5 * 1024 * 1024;

//

	VO_TCHAR * url = (VO_TCHAR*)pParam->pSource;
	if( m_buffer_stream )
		m_buffer_stream->set_to_close( VO_FALSE );
	m_is_to_exit = VO_FALSE;

	memcpy( &m_source_param , pParam , sizeof(VO_SOURCE_OPENPARAM) );

	if( m_source_param.pSourceOP )
	{
		StreamingNotifyEventFunc * ptr_func = ( StreamingNotifyEventFunc * )m_source_param.pSourceOP;

		m_notifier.funtcion = ptr_func->funtcion;
		m_notifier.userdata = ptr_func->userdata;
	}
	else
	{
		m_notifier.funtcion = 0;
		m_notifier.userdata = 0;
	}

	if( sizeof(VO_TCHAR) != 1 )
	{
		wcstombs(m_url,(wchar_t*)url,sizeof(m_url));
	}
	else
	{
		strcpy( m_url , (char *)url );
	}

	//strcpy( m_url , "http://10.2.64.229/share/Jolin.3gp" );

	if( pParam->nReserve )
	{
		memcpy( &m_PD_param , (VO_PTR)pParam->nReserve , sizeof( m_PD_param ) );
	}
	else
	{
		return VO_FALSE;
	}

	//m_PD_param.nMaxBuffer = 15000;
	m_PD_param.nBufferTime = 12;
	m_PD_param.nMaxBuffer = 16 * 1024;
	//return delay_open();

	return VO_TRUE;
}

VO_U32 vo_PD_manager::close()
{
	//voCAutoLock lock(&m_testlock);
	VO_U32 ret = 0;

	if( m_buffer_stream )
		m_buffer_stream->set_to_close( VO_TRUE );
	m_is_to_exit = VO_TRUE;

	VOLOGI( "+wait delay open finish!" );
	while( m_is_init )
	{
		voOS_Sleep( 20 );
	}
	VOLOGI( "-wait delay open finish!" );

	m_downloader.stop_download();

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

	return ret;
}

VO_U32 vo_PD_manager::get_sourceinfo(VO_SOURCE_INFO * pSourceInfo)
{
	return m_parser_api.GetSourceInfo( m_fileparser , pSourceInfo );
}

VO_U32 vo_PD_manager::get_trackinfo(VO_U32 nTrack, VO_SOURCE_TRACKINFO * pTrackInfo)
{
	VO_U32 ret = m_parser_api.GetTrackInfo( m_fileparser , nTrack , pTrackInfo );

	if( m_duration )
		pTrackInfo->Duration = m_duration;

	if( ret == 0 )
	{
		VOLOGE( "Track Num: %d , Track Type: %s" , nTrack , pTrackInfo->Type == 1 ? "Video" : "Audio" );
	}

	return ret;
}

VO_U32 vo_PD_manager::get_sample(VO_U32 nTrack, VO_SOURCE_SAMPLE * pSample)
{
	//VOLOGI( "+vo_PD_manager::get_sample Track: %d" , nTrack );
	VO_U32 ret = m_parser_api.GetSample( m_fileparser , nTrack , pSample );

	m_framegot[nTrack]++;

	VOLOGI( "vo_PD_manager::get_sample Track: %d , TimeStamp: %lld , ret: %u" , nTrack , pSample->Time , ret );

	if( m_framegot[0] >= 3 && m_framegot[1] >= 3 )
	{
		m_is_start_buffering = VO_TRUE;
	}

	if( m_framegot[0] == 1 || m_framegot[1] == 1  )
	{
		m_seek_status = (SEEKSTATUS)((VO_S32)m_seek_status + 1);
	}

	return ret;
}

VO_U32 vo_PD_manager::set_pos(VO_U32 nTrack, VO_S64 * pPos)
{
	if( m_is_init )
		return VO_ERR_FAILED;

	return m_parser_api.SetPos( m_fileparser , nTrack , pPos );
}

VO_VOID vo_PD_manager::moveto( VO_S64 pos )
{
	if( m_is_init )
		return;

	VOLOGI( "++++++++++++++++Seek to : %d" , (VO_S32)pos );

	VO_FILE_MEDIATIMEFILEPOS mediapos;
	mediapos.nFlag = 1;
	mediapos.llMediaTime = pos;

	VO_U32 ret = m_parser_api.GetSourceParam( m_fileparser , VO_PID_FILE_GetSEEKPOSBYTIME , &mediapos );

	if( ret == 0 )
	{
		VOLOGI( "Got Accurate Seek Pos: %lld" , mediapos.llFilePos );
		VO_BYTE b;
		//m_is_start_buffering = VO_FALSE;

		m_buffer.read( mediapos.llFilePos , &b , 1 );
	}

	m_framegot[0] = 0;
	m_framegot[1] = 0;
	m_seek_status = SEEKSTATUS_SEEKSTART;
}

VO_U32 vo_PD_manager::set_sourceparam(VO_U32 uID, VO_PTR pParam)
{
	switch( uID )
	{
	case VOID_STREAMING_OPENURL:
		{
			//return VO_ERR_NONE;
			if( delay_open() )
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
	}
	return m_parser_api.SetSourceParam( m_fileparser , uID , pParam );
}

VO_VOID vo_PD_manager::process_STREAMING_QUICKRESPONSE_COMMAND( VO_QUICKRESPONSE_COMMAND command )
{
	switch( command )
	{
	case VO_QUICKRESPONSE_RUN:
		{
			VOLOGI("+++++++++++++++++++++VO_QUICKRESPONSE_RUN+++++++++++++++++++");
			//m_is_to_break_buffering = VO_FALSE;
			m_is_pause = VO_FALSE;
		}
		break;
	case VO_QUICKRESPONSE_PAUSE:
		{
			VOLOGI("+++++++++++++++++++++VO_QUICKRESPONSE_PAUSE+++++++++++++++++++");
			m_is_pause = VO_TRUE;
		}
		break;
	case VO_QUICKRESPONSE_STOP:
		{
			VOLOGI("+++++++++++++++++++++VO_QUICKRESPONSE_STOP+++++++++++++++++++");
			//close();
			//m_is_to_break_buffering = VO_TRUE;
			m_is_pause = VO_TRUE;
		}
		break;
	case VO_QUICKRESPONSE_FLUSH:
		{
			VOLOGI("+++++++++++++++++++++VO_QUICKRESPONSE_FLUSH+++++++++++++++++++");
			m_is_pause = VO_FALSE;
		}
		break;
	case VO_QUICKRESPONSE_RETURNBUFFER:
		{
			VOLOGI("+++++++++++++++++++++VO_QUICKRESPONSE_RETURNBUFFER+++++++++++++++++++");
			//m_is_to_break_buffering = VO_TRUE;
			m_is_pause = VO_TRUE;
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
				VOLOGI( "Use Media Time to determine: Media Time %d  Duration %d" , (VO_S32)pos.llMediaTime , (VO_S32)m_duration );
				if( m_duration )
				{
					*((VO_S32*)pParam) = (float)( pos.llMediaTime ) * 100. / (float)m_duration;
				}
				else
					*((VO_S32*)pParam) = 0;
			}
			else
			{
				*((VO_S32*)pParam) = (VO_S32)(( info.physical_start + info.usedsize ) * 100. /(float)m_buffer.get_filesize());
			}

			VOLOGI( "Current Buffering Duration %d" , *((VO_S32*)pParam) );
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
	return m_parser_api.GetTrackParam( m_fileparser , nTrack , uID , pParam );
}

VO_BOOL vo_PD_manager::delay_open()
{
	m_buffer.init( &m_PD_param );

	m_downloader.set_download_callback( this );
	m_buffer.set_buffer_callback( this );

	m_is_init = VO_TRUE;
	m_buffer.set_initmode( m_is_init );

	if( !m_downloader.set_url( m_url , &m_PD_param ) )
	{
		VOLOGI( "set download url failed!" );
		m_is_init = VO_FALSE;
		return VO_FALSE;
	}

	VOLOGI( "+m_downloader.start_download()" );
	if( !m_downloader.start_download() )
	{
		VOLOGI( "download failed!" );
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

	if( !perpare_parser() )
	{
		m_is_init = VO_FALSE;
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
	source.nFlag = param.nFlag;
	source.pSource = m_PD_param.mFilePath;
	source.nReserve = ( VO_U32 )this;
	param.pSource		= &source;
	param.pSourceOP= &m_opFile;
	param.pMemOP = m_source_param.pMemOP;

	VOLOGI( "+m_parser_api.Open( &m_fileparser , &param )" );
	VO_U32 ret = m_parser_api.Open( &m_fileparser , &param );
	VOLOGI( "-m_parser_api.Open( &m_fileparser , &param )" );

	if( ret != 0 )
	{
		VOLOGI( "file parser open failed!" );
		m_parser_api.Close = 0;
		m_is_init = VO_FALSE;
		return VO_FALSE;
	}

	if( m_is_to_exit )
	{
		m_is_init = VO_FALSE;
		return VO_FALSE;
	}

	VO_BOOL need_scan = VO_FALSE;
	ret = m_parser_api.GetSourceParam( m_fileparser , VO_PID_SOURCE_NEEDSCAN , &need_scan );

	//if( ret == VO_ERR_NONE && need_scan )
		//notify( VO_PD_EVENT_DURATIONMAYWRONG , 0 );

	VO_SOURCE_INFO source_info;
	m_parser_api.GetSourceInfo( m_fileparser , &source_info );
	if( m_downloader.get_duration() )
	{
		source_info.Duration = m_downloader.get_duration();
		VOLOGE( "Duration get from downloader: %u" , source_info.Duration );
	}
	m_bitrate = m_downloader.get_actual_filesize() * 8000. / source_info.Duration;
	m_duration = source_info.Duration;

	if( need_scan )
		m_duration = -1;

	headerdata_info headerdata;
	memset( &headerdata , 0 , sizeof(headerdata) );
	ret = m_parser_api.GetSourceParam( m_fileparser , VO_PID_FILE_HEADDATAINFO , &headerdata );

	if( ret == VO_ERR_NONE && headerdata.arraysize )
	{
		headerdata.ptr_array = new headerdata_element[headerdata.arraysize];
		m_parser_api.GetSourceParam( m_fileparser , VO_PID_FILE_HEADDATAINFO , &headerdata );

#if 1
		VOLOGI( "Got cache buffers! Size: %d Reserved: %d" , headerdata.arraysize , headerdata.reserved );
		VOLOGI( "List Below: " );
		for( VO_S32 i = 0 ; i < headerdata.arraysize ; i++ )
		{
			VOLOGI( " Pos : %d    Size : %d " , (VO_S32)headerdata.ptr_array[i].physical_pos , (VO_S32)headerdata.ptr_array[i].size );
		}
		VOLOGI( "List End!" );
#endif

		VOLOGI( "+arrange_headdata" );
		arrange_headdata( &headerdata );
		VOLOGI( "-arrange_headdata" );
	}
	else
	{
		memset( &headerdata , 0 , sizeof(headerdata) );
	}

	VOLOGI( "+m_downloader.stop_download" );
	m_downloader.stop_download();
	VOLOGI( "-m_downloader.stop_download" );

#if 1
	VOLOGI( "We need to cache %d buffers!" , headerdata.arraysize );
	VOLOGI( "List Below: " );
	for( VO_S32 i = 0 ; i < headerdata.arraysize ; i++ )
	{
		VOLOGI( " Pos : %d    Size : %d " , (VO_S32)headerdata.ptr_array[i].physical_pos , (VO_S32)headerdata.ptr_array[i].size );
	}
	VOLOGI( "Trick!" );
	VOLOGI( "List End!" );
#endif

	VOLOGI( "0" );
	if( m_is_to_exit )
	{
		m_is_init = VO_FALSE;
		return VO_FALSE;
	}

	VOLOGI( "+m_buffer.rerrange_cache" );
	if( !m_buffer.rerrange_cache( headerdata.ptr_array , headerdata.arraysize ) )
	{
		VOLOGI( "cache buffer failed!" );
		if( headerdata.ptr_array )
		{
			delete []headerdata.ptr_array;
		}
		m_is_init = VO_FALSE;
		return VO_FALSE;
	}

	VOLOGI( "1" );
	if( headerdata.ptr_array )
	{
		delete []headerdata.ptr_array;
	}

	VOLOGI( "2" );
	if( m_is_to_exit )
	{
		m_is_init = VO_FALSE;
		return VO_FALSE;
	}

	VOLOGI( "3" );
	m_buffer.set_initmode( VO_FALSE );

	VOLOGI( "4" );
	BUFFER_INFO info;
	m_buffer.get_buffer_info( &info );

	VOLOGI( "cached buffer: start %d , size %d" , (VO_S32)info.physical_start , (VO_S32)info.usedsize );

	if( info.usedsize != m_buffer.get_filesize() )
		m_downloader.start_download( info.physical_start + info.usedsize );

	m_is_init = VO_FALSE;

	av_startpos startpos;
	memset( &startpos , 0 , sizeof( av_startpos ) );
	if( VO_ERR_NONE == m_parser_api.GetSourceParam( m_fileparser , VO_PID_FILE_GETAVSTARTPOS , &startpos ) )
	{
		VOLOGE( "Audio Start: %lld , Video Start: %lld" , startpos.audio_startpos , startpos.video_startpos );
		VO_S64 delta = startpos.audio_startpos - startpos.video_startpos;

		if( delta < -5 * 1024 * 1024 || delta > 5 * 1024 * 1024 )
		{
			VOLOGE("**************notify VO_PD_ENENT_NOTGOODFORSTREAMING***************");
			notify( VO_PD_ENENT_NOTGOODFORSTREAMING , 0 );
		}
	}

	VOLOGE( "Begin Sleep! %d" , m_waitbuffertime );
	voOS_Sleep( m_waitbuffertime );
	VOLOGE( "End Sleep!" );

	return VO_TRUE;
}

VO_BOOL vo_PD_manager::perpare_parser()
{
	VO_BYTE *ptr_buffer = new VO_BYTE[GUESSFILETYPE_BUFFERSIZE];

	VOLOGE( "+Skip ID3" );
	while( SkipID3v2Data( m_buffer_stream ) ){}
	VOLOGE( "-Skip ID3" );

	VOLOGI( "+get buffer to perpare parser!" );
	VO_S64 size = m_buffer_stream->read( ptr_buffer , GUESSFILETYPE_BUFFERSIZE );
	VOLOGI( "-get buffer to perpare parser!%d   %d" , GUESSFILETYPE_BUFFERSIZE , (VO_S32)size );

	m_buffer_stream->seek( 0 , VO_FILE_BEGIN );

#ifdef _LIB
	if( !get_api( ptr_buffer , GUESSFILETYPE_BUFFERSIZE , &m_parser_api ) )
	{
		VOLOGI("can not find right file parser for this file!");
		delete []ptr_buffer;
		return VO_FALSE;
	}
#else
	if( !get_rightlibrary( ptr_buffer , size , m_dllloader.m_szDllFile , m_dllloader.m_szAPIName ) )
	{
		VOLOGI("can not find right file parser for this file!");
		delete []ptr_buffer;
		return VO_FALSE;
	}

	delete []ptr_buffer;

	if(m_dllloader.LoadLib(NULL) == 0)
	{
		VOLOGI("load library %s fail!" , m_dllloader.m_szDllFile );
		return VO_FALSE;
	}

	pvoGetReadAPI getapi = (pvoGetReadAPI)m_dllloader.m_pAPIEntry;

	if( getapi )
		getapi( &m_parser_api , 0 );
	else
		return VO_FALSE;
#endif

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

VO_VOID vo_PD_manager::notify( long EventCode, long * EventParam )
{
	if( m_notifier.funtcion )
	{
		m_notifier.funtcion( EventCode , EventParam , (long	*)m_notifier.userdata );
	}
}


