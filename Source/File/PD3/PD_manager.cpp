#include "PD_manager.h"
#include "fileformat_checker.h"
#include "log.h"

#define GUESSFILETYPE_BUFFERSIZE (32 * 1024)
typedef VO_S32 (VO_API *pvoGetReadAPI)(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag);

unsigned long g_dwFRModuleID = VO_INDEX_SRC_HTTP;
//log_client * g_ptr_logger = 0;

VO_PTR VO_API stream_open(VO_FILE_SOURCE * pSource)
{
	PD_manager * ptr_manager = ( PD_manager * )pSource->nReserve;
	return ptr_manager->get_io();
}

VO_S32 VO_API stream_read(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)
{
	interface_io * ptr_io = (interface_io*)pFile;
	return ptr_io->read( (VO_PBYTE)pBuffer , uSize );
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
	interface_io * ptr_io = (interface_io*)pFile;
	return ptr_io->set_pos( nPos , uFlag );
}

VO_S64 VO_API stream_size(VO_PTR pFile)
{
	interface_io * ptr_io = (interface_io*)pFile;
	return ptr_io->get_filesize();
}

VO_S64 VO_API stream_save(VO_PTR pFile)
{
	return -1;
}

VO_S32 VO_API stream_close(VO_PTR pFile)
{
	interface_io * ptr_io = (interface_io*)pFile;
	ptr_io->set_pos( 0 , VO_FILE_BEGIN );
	return 0;
}

PD_manager::PD_manager(void)
:m_firstseektrack(-1)
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

	if(!g_ptr_logger)
	{
		g_ptr_logger = new log_client;
	}
	g_ptr_logger->add_ref();

}

PD_manager::~PD_manager(void)
{
	if( g_ptr_logger )
		g_ptr_logger->release();
}

VO_BOOL PD_manager::open( VO_SOURCE_OPENPARAM * pParam )
{
	close();

	VO_TCHAR * url = (VO_TCHAR*)pParam->pSource;

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

	if( pParam->nReserve )
	{
		memcpy( &m_PD_param , (VO_PTR)pParam->nReserve , sizeof( m_PD_param ) );
	}
	else
	{
		return VO_FALSE;
	}

	m_PD_param.nMaxBuffer = 16 * 1024;
	m_PD_param.nBufferTime = 5;

	return VO_TRUE;
}

VO_U32 PD_manager::close()
{
	VO_S32 ret = 0;

	if( m_parser_api.Close )
	{
		ret = m_parser_api.Close( m_fileparser );
		m_parser_api.Close = 0;
	}

	return ret;
}

VO_U32 PD_manager::get_sourceinfo(VO_SOURCE_INFO * pSourceInfo)
{
	VO_U32 ret =  m_parser_api.GetSourceInfo( m_fileparser , pSourceInfo );

	return ret;
}

VO_U32 PD_manager::get_trackinfo(VO_U32 nTrack, VO_SOURCE_TRACKINFO * pTrackInfo)
{
	VO_U32 ret =  m_parser_api.GetTrackInfo( m_fileparser , nTrack , pTrackInfo );

	if( ret == 0 && pTrackInfo->Type == VO_SOURCE_TT_AUDIO )
	{
		PRINT_LOG( LOG_LEVEL_NORMAL , "%d is Audio!" , nTrack );
	}
	else if( ret == 0 && pTrackInfo->Type == VO_SOURCE_TT_VIDEO )
	{
		PRINT_LOG( LOG_LEVEL_NORMAL , "%d is Video!" , nTrack );
	}

	return ret;
}

VO_U32 PD_manager::get_sample(VO_U32 nTrack, VO_SOURCE_SAMPLE * pSample)
{
	PRINT_LOG( LOG_LEVEL_ALLINFO , "+Get Sample Track: %d" , nTrack );
	VO_U32 ret = m_parser_api.GetSample( m_fileparser , nTrack , pSample );
	PRINT_LOG( LOG_LEVEL_ALLINFO , "-Get Sample Track: %d" , nTrack );

	return ret;
}

VO_U32 PD_manager::set_pos(VO_U32 nTrack, VO_S64 * pPos)
{
	VO_U32 ret = m_parser_api.SetPos( m_fileparser , nTrack , pPos );

	return ret;
}

VO_VOID PD_manager::moveto( VO_S64 pos )
{
	;
}

VO_U32 PD_manager::set_sourceparam(VO_U32 uID, VO_PTR pParam)
{
	switch( uID )
	{
	case VOID_STREAMING_OPENURL:
		{
			VO_BOOL ret = delay_open();
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
	}
	return m_parser_api.SetSourceParam( m_fileparser , uID , pParam );
}

VO_U32 PD_manager::get_sourceparam(VO_U32 uID, VO_PTR pParam)
{
	return m_parser_api.GetSourceParam( m_fileparser , uID , pParam );
}

VO_U32 PD_manager::set_trackparam(VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
{
	return m_parser_api.SetTrackParam( m_fileparser , nTrack , uID , pParam );
}

VO_U32 PD_manager::get_trackparam(VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
{
	return m_parser_api.GetTrackParam( m_fileparser , nTrack , uID , pParam );
}

VO_VOID PD_manager::process_STREAMING_QUICKRESPONSE_COMMAND( VO_QUICKRESPONSE_COMMAND command )
{
	;
}

VO_BOOL PD_manager::delay_open()
{
	m_stream.set_initmode( VO_TRUE );
	if( !m_stream.open( m_url , &m_PD_param ) )
	{
		PRINT_LOG( LOG_LEVEL_CRITICAL , "Open Stream Fail!" );
		return VO_FALSE;
	}

	if( !perpare_parser() )
	{
		PRINT_LOG( LOG_LEVEL_CRITICAL , "File parser load Fail!" );
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
	param.pDrmCB = m_source_param.pDrmCB;
	param.pMemOP = m_source_param.pMemOP;

	PRINT_LOG( LOG_LEVEL_IMPORTANT , "+m_parser_api.Open" );
	VO_U32 ret = m_parser_api.Open( &m_fileparser , &param );
	PRINT_LOG( LOG_LEVEL_IMPORTANT , "-m_parser_api.Open ret: %d" , ret );

	if( ret != 0 )
	{
		m_parser_api.Close = 0;
		return VO_FALSE;
	}

	headerdata_info headerdata;
	memset( &headerdata , 0 , sizeof(headerdata) );
	ret = m_parser_api.GetSourceParam( m_fileparser , VO_PID_FILE_HEADDATAINFO , &headerdata );

	if( ret == VO_ERR_NONE && headerdata.arraysize )
	{
		headerdata.ptr_array = new headerdata_element[headerdata.arraysize];
		m_parser_api.GetSourceParam( m_fileparser , VO_PID_FILE_HEADDATAINFO , &headerdata );

		if( !m_stream.cache_headerdata( &headerdata ) )
		{
			PRINT_LOG( LOG_LEVEL_CRITICAL , "cache_headerdata fail!" );
			m_parser_api.Close( m_fileparser );
			m_parser_api.Close = 0;
			delete []headerdata.ptr_array;
			return VO_FALSE;
		}

		delete []headerdata.ptr_array;
	}

	m_stream.set_initmode( VO_FALSE );

	return VO_TRUE;
}

VO_BOOL PD_manager::perpare_parser()
{
	VO_BYTE *ptr_buffer = new VO_BYTE[GUESSFILETYPE_BUFFERSIZE];

	PRINT_LOG( LOG_LEVEL_DETAIL , "+Skip ID3 Data!" );
	while( SkipID3v2Data( &m_stream ) ){}
	PRINT_LOG( LOG_LEVEL_DETAIL , "-Skip ID3 Data!" );

	VO_S64 size = m_stream.read( ptr_buffer , GUESSFILETYPE_BUFFERSIZE );

	PRINT_LOG( LOG_LEVEL_IMPORTANT , "Read %d and Got %lld" , GUESSFILETYPE_BUFFERSIZE , size );

	m_stream.set_pos( 0 , VO_FILE_BEGIN );

	if( !get_rightlibrary( ptr_buffer , size , m_dllloader.m_szDllFile , m_dllloader.m_szAPIName ) )
	{
		PRINT_LOG( LOG_LEVEL_CRITICAL , "Get Library fail!" );
		delete []ptr_buffer;
		return VO_FALSE;
	}

	delete []ptr_buffer;

	if(m_dllloader.LoadLib(NULL) == 0)
	{
		return VO_FALSE;
	}

	pvoGetReadAPI getapi = (pvoGetReadAPI)m_dllloader.m_pAPIEntry;

	if( getapi )
		getapi( &m_parser_api , 0 );
	else
	{
		PRINT_LOG( LOG_LEVEL_CRITICAL , "Get API Fail!" );
		return VO_FALSE;
	}

	return VO_TRUE;
}

