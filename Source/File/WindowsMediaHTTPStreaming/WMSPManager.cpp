#include "WMSPManager.h"
#include "voLog.h"
#include "voOSFunc.h"


typedef VO_S32 (VO_API *pvoGetReadAPI)(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag);


static VO_PTR VO_API stream_open(VO_FILE_SOURCE * pSource)
{
	CWMSPManager * pManager = (CWMSPManager *)pSource->nReserve;
	return pManager->getStream();
}

static VO_S32 VO_API stream_read(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)
{
	CStreamCache * pCachebuffer = (CStreamCache*)pFile;
	return pCachebuffer->read( (VO_PBYTE)pBuffer, uSize );
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
	CStreamCache * pCachebuffer = (CStreamCache*)pFile;
	return pCachebuffer->seek( nPos , uFlag );
}

static VO_S64 VO_API stream_size(VO_PTR pFile)
{
	CStreamCache * pCachebuffer = (CStreamCache*)pFile;
	return pCachebuffer->FileSize();
}

static VO_S64 VO_API stream_save(VO_PTR pFile)
{
	return -1;
}

static VO_S32 VO_API stream_close(VO_PTR pFile)
{
	CStreamCache * pCachebuffer = (CStreamCache*)pFile;
	pCachebuffer->seek(0 , VO_FILE_BEGIN);
	return 0;
}


CWMSPManager::CWMSPManager(void)
	: CThread("CWMSPManager")
	, m_StateManager(Manager_Stop)
	, m_fileparser(0)
	, m_duration(0)
	, m_llSeekTime(0)
	, m_iOverall_Kbitrate(0)
	, m_bBuffering(VO_FALSE)
	, m_bNeedCBSeekPos(VO_FALSE)
	, m_bStopedLastWhileSeeking(VO_FALSE)
{
	m_opFile.Close	= stream_close;
	m_opFile.Flush	= stream_flush;
	m_opFile.Open	= stream_open;
	m_opFile.Read	= stream_read;
	m_opFile.Save	= stream_save;
	m_opFile.Seek	= stream_seek;
	m_opFile.Size	= stream_size;
	m_opFile.Write	= stream_write;

	memset( &m_parser_api, 0, sizeof(m_parser_api) );
	memset( &m_sourcedrm_callbcak, 0, sizeof(VO_SOURCEDRM_CALLBACK) );

	m_WMHTTP.setDelegate(this);
}

CWMSPManager::~CWMSPManager(void)
{
	Close();
}

VO_BOOL CWMSPManager::Open(VO_SOURCE_OPENPARAM * pParam)
{
	//PRINT_LOG(LOG_LEVEL_CRITICAL, "open");
	VO_TCHAR *url = (VO_TCHAR*)pParam->pSource;

	memcpy( &m_source_param, pParam, sizeof(VO_SOURCE_OPENPARAM) );
	if (m_source_param.pDrmCB)
		memcpy( &m_sourcedrm_callbcak, m_source_param.pDrmCB, sizeof(VO_SOURCEDRM_CALLBACK) );

	m_dllloader.SetLibOperator(m_source_param.pLibOP);

	if (m_source_param.pSourceOP)
	{
		StreamingNotifyEventFunc *ptr_func = (StreamingNotifyEventFunc *)m_source_param.pSourceOP;

		m_notifier.funtcion = ptr_func->funtcion;
		m_notifier.userdata = ptr_func->userdata;
	}
	else
	{
		m_notifier.funtcion = 0;
		m_notifier.userdata = 0;
	}

	if (sizeof(VO_TCHAR) != 1)
		wcstombs( m_url, (wchar_t*)url, sizeof(m_url) );
	else
		strcpy(m_url , (char*)url);

	if (pParam->nReserve)
		memcpy( &m_PD_param, (VO_PTR)pParam->nReserve, sizeof(m_PD_param) );
	else
		return VO_FALSE;
	//---
	vostrcpy( m_dllloader.m_szDllFile, _T("voASFFR") );
	vostrcpy( m_dllloader.m_szAPIName, _T("voGetASFReadAPI") );

#ifdef _WIN32
	vostrcat( m_dllloader.m_szDllFile, _T(".Dll") );
#else
	vostrcat( m_dllloader.m_szDllFile, _T(".so") );
#endif

	if (m_dllloader.LoadLib(NULL) == 0)
	{
		VOLOGE("!LoadLib");
		return VO_FALSE;
	}

	pvoGetReadAPI getapi = (pvoGetReadAPI)m_dllloader.m_pAPIEntry;
	if ( getapi )
	{
		getapi (&m_parser_api , 0);
	}
	else
	{
		VOLOGE("!getapi");
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_S32 CWMSPManager::Play()
{
	m_Stream.start();
	m_WMHTTP.play(m_url, m_llSeekTime);

	VO_FILE_SOURCE source;
	source.nFlag	= 0x102;//0x00020101;
	source.pSource	= m_PD_param.mFilePath;
	source.nReserve	= (VO_U32)this;

	VO_SOURCE_OPENPARAM param={0};//TODO
	param.nFlag			= source.nFlag;
	param.pSource		= &source;
	param.pSourceOP		= &m_opFile;
	param.pMemOP		= m_source_param.pMemOP;
	if (m_sourcedrm_callbcak.fCallback)
		param.pDrmCB	= &m_sourcedrm_callbcak;

	if (m_parser_api.Open(&m_fileparser, &param) != 0)
	{
		m_parser_api.Close = 0;
		VOLOGE("!m_parser_api.Open");
		return VO_ERR_BASE;
	}

	//voOS_Sleep(3000);//4500

	VO_SOURCE_INFO sourceinfo = {0};
	if (m_parser_api.GetSourceInfo(m_fileparser, &sourceinfo) != 0)
	{
		m_parser_api.Close = 0;
		VOLOGE("!m_parser_api.GetSourceInfo");
		return VO_ERR_BASE;
	}
	if (m_parser_api.GetSourceParam(m_fileparser, VO_PID_SOURCE_BITRATE, &m_iOverall_Kbitrate) != 0)
	{
		m_parser_api.Close = 0;
		VOLOGE("!VO_PID_SOURCE_BITRATE");
		return VO_ERR_BASE;
	}

	m_StateManager = Manager_Running;

	return VO_ERR_NONE;
}

VO_U32 CWMSPManager::Close()
{
	//PRINT_LOG(LOG_LEVEL_CRITICAL, "-----CWMSPManager::close-----");
	VO_U32 iRet = 0;

	m_WMHTTP.stop();

	if (m_parser_api.Close)
	{
		iRet = m_parser_api.Close(m_fileparser);
		m_parser_api.Close = 0;
	}
	//PRINT_LOG(LOG_LEVEL_CRITICAL, "-----CWMSPManager::close %d-----", ret);

	return iRet;
}

VO_U32 CWMSPManager::GetSourceInfo(VO_SOURCE_INFO * pSourceInfo)
{
	//PRINT_LOG(LOG_LEVEL_CRITICAL, "get_sourceinfo");

	VO_U32 ret =  m_parser_api.GetSourceInfo( m_fileparser , pSourceInfo );

	if (ret == 0 && m_duration)
		pSourceInfo->Duration = m_duration;

	return ret;
}

VO_U32 CWMSPManager::GetTrackInfo(VO_U32 nTrack, VO_SOURCE_TRACKINFO * pTrackInfo)
{
	//PRINT_LOG(LOG_LEVEL_CRITICAL, "get_trackinfo");
	VO_U32 ret =  m_parser_api.GetTrackInfo( m_fileparser , nTrack , pTrackInfo );

	if (m_duration)
		pTrackInfo->Duration = m_duration;
	
	if (nTrack < 2)
		m_TrackType[nTrack] = pTrackInfo->Type;

	return ret;
}

VO_U32 CWMSPManager::GetSample(VO_U32 nTrack, VO_SOURCE_SAMPLE * pSample)
{
	//PRINT_LOG(LOG_LEVEL_CRITICAL, "UseableSize %d", 1024 * 1024 * 7 - m_Stream.freeBufSize());
	//if (m_Stream.WrittenSize() < m_Stream.FileSize() && (1024 * 1024 * 7 - m_Stream.freeBufSize()) < (m_iOverall_Kbitrate / 8 * 1024 / 2) )
	if (m_bBuffering || Manager_Running != m_StateManager)
	{
		//PRINT_LOG(LOG_LEVEL_CRITICAL , "RRR VO_ERR_SOURCE_NEEDRETRY");
		return VO_ERR_SOURCE_NEEDRETRY;
	}
	else
	{
		//PRINT_LOG(LOG_LEVEL_CRITICAL , "RRR m_parser_api.GetSample nTrack %d", nTrack);
		VO_U32 iRet = m_parser_api.GetSample(m_fileparser , nTrack , pSample);

		if (m_bNeedCBSeekPos)
		{
			if (VO_SOURCE_TT_VIDEO == m_TrackType[nTrack])
			{
				notify( VO_PD_EVENT_SEEKFINALPOS , (long *)&pSample->Time );
				VOLOGR("set_pos final %lld", pSample->Time);
				m_bNeedCBSeekPos = VO_FALSE;
			} 
			else if (VO_SOURCE_TT_AUDIO == m_TrackType[nTrack])
			{
				//if ()
				//{
				//}
			}
		}

		//PRINT_LOG(LOG_LEVEL_CRITICAL , "RRR m_parser_api.GetSample iRet %x, nTrack %x, Time %lld", iRet, nTrack, pSample->Time);

		return iRet;
	}
}

VO_U32 CWMSPManager::SetPos(VO_U32 nTrack, VO_S64 * pPos)
{
	VOLOGR("set_pos %lld", *pPos);
	m_llSeekTime = *pPos;
	if (Manager_Seeking == m_StateManager)
		return VO_ERR_NONE;

	m_StateManager = Manager_Seeking;

	m_bBuffering = VO_TRUE;

	long temp = 0;
	notify(VO_EVENT_BUFFERING_BEGIN, &temp);
	m_bBuffering = VO_TRUE;
	VOLOGR("RRR iUseableSize VO_EVENT_BUFFERING_BEGIN");

	m_bNeedCBSeekPos = VO_TRUE;

	CThread::ThreadStart();

	return VO_ERR_NONE;
}

VO_U32 CWMSPManager::SetSourceParam(VO_U32 uID, VO_PTR pParam)
{
	if (Manager_Seeking == m_StateManager)
		return VO_ERR_SOURCE_OK;

	VOLOGR("set_sourceparam %x", uID);
	switch( uID )
	{
	case VOID_STREAMING_OPENURL:
		{
			if ( 0 == Play() )
				return VO_ERR_SOURCE_OK;
			else
				return VO_ERR_SOURCE_OPENFAIL;
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

VO_VOID CWMSPManager::process_STREAMING_QUICKRESPONSE_COMMAND( VO_QUICKRESPONSE_COMMAND command )
{
	VOLOGR("%d", command);
	
	switch (command)
	{
	case VO_QUICKRESPONSE_RUN:
		{
			if (Manager_Stop == m_StateManager)
			{
				m_Stream.close();
				m_Stream.start();

				Play();
				//doOpen();
			}
			else if (Manager_Paused == m_StateManager)
			{
				m_Stream.start();

				m_WMHTTP.play( m_url, 0, m_Stream.WrittenSize() );
			}
			else if (Manager_Pausing == m_StateManager)
			{
				m_Stream.start();
			}

			m_StateManager = Manager_Running;
		}

		break;

	case VO_QUICKRESPONSE_PAUSE:
		{
			m_StateManager = Manager_Pausing;
			m_Stream.pause();
		}

		break;

	case VO_QUICKRESPONSE_STOP:
		{
			m_Stream.close();

			m_StateManager = Manager_Stop;
		}

		break;

	case VO_QUICKRESPONSE_FLUSH:
		{
/*
			if (Manager_Running != m_StateManager)
			{
				//PRINT_LOG(LOG_LEVEL_CRITICAL, "VO_QUICKRESPONSE_FLUSH");

				m_WMHTTP.stop();
				m_Stream.close();

				m_parser_api.Close(m_fileparser);
				m_fileparser = NULL;

				m_StateManager = Manager_Stop;
			}
*/
		}

		break;

	case VO_QUICKRESPONSE_RETURNBUFFER:
		{
			m_Stream.close();
		}

		break;
	}
}

VO_U32 CWMSPManager::GetSourceParam(VO_U32 uID, VO_PTR pParam)
{
	return m_parser_api.GetSourceParam( m_fileparser , uID , pParam );
}

VO_U32 CWMSPManager::SetTrackParam(VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
{
	return m_parser_api.SetTrackParam( m_fileparser , nTrack , uID , pParam );
}

VO_U32 CWMSPManager::GetTrackParam(VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
{
	return m_parser_api.GetTrackParam( m_fileparser , nTrack , uID , pParam );
}

VO_VOID CWMSPManager::notify( long EventCode, long * EventParam )
{
	if( m_notifier.funtcion )
		m_notifier.funtcion( EventCode , EventParam , (long	*)m_notifier.userdata );
}

VO_VOID CWMSPManager::some (VO_CHAR *buf, VO_S32 iLen)
{
	//PRINT_LOG(LOG_LEVEL_CRITICAL, "m_StateManager %x, iLen %d", m_StateManager, iLen);
	if (Manager_Pausing == m_StateManager && m_Stream.freeBufSize() < 1024 * 1024)
	{
		m_StateManager = Manager_Paused;

		m_WMHTTP.stop(3000);
	}
	else if ( Manager_Paused == m_StateManager && (0 == iLen || m_Stream.freeBufSize() < iLen) )
	{
		return;
	}
	else if (Manager_Running == m_StateManager)
	{
		int iUseableSize = 1024 * 1024 * 12 - m_Stream.freeBufSize();
		if (VO_FALSE == m_bBuffering)
		{
			if (iLen && iUseableSize < (m_iOverall_Kbitrate / 8 * 1024 / 2) )
			{
				long temp = 0;
				notify(VO_EVENT_BUFFERING_BEGIN, &temp);

				m_bBuffering = VO_TRUE;

				VOLOGR("RRR iUseableSize VO_EVENT_BUFFERING_BEGIN");
			}
		} 
		else //VO_TRUE == m_bBuffering
		{
			if (iLen && m_Stream.WrittenSize() != m_Stream.FileSize() && iUseableSize < (m_iOverall_Kbitrate / 8 * 1024 * 8) && m_Stream.freeBufSize() > 1024 * 300)
			{
				int iPercent = 100 * iUseableSize / (m_iOverall_Kbitrate *1024 / 8 * 8);
				notify(VO_EVENT_BUFFERING_PERCENT, (long*)&iPercent);

				//PRINT_LOG(LOG_LEVEL_CRITICAL , "RRR iUseableSize %d, VO_EVENT_BUFFERING_PERCENT %d", iUseableSize, iPercent);
			}
			else
			{
/*
				PRINT_LOG(LOG_LEVEL_CRITICAL, "m_StateManager %x, iLen %d", m_StateManager, iLen);

				if (m_Stream.WrittenSize() != m_Stream.FileSize())
					PRINT_LOG(LOG_LEVEL_CRITICAL , "true");
				else
					PRINT_LOG(LOG_LEVEL_CRITICAL , "false");
				PRINT_LOG(LOG_LEVEL_CRITICAL , "m_Stream.WrittenSize() %lld, m_Stream.FileSize() %lld", m_Stream.WrittenSize(), m_Stream.FileSize());

				if (iUseableSize < (m_iOverall_Kbitrate / 8 * 1024 * 8))
					PRINT_LOG(LOG_LEVEL_CRITICAL , "true");
				else
					PRINT_LOG(LOG_LEVEL_CRITICAL , "false");
				PRINT_LOG(LOG_LEVEL_CRITICAL , "iUseableSize %d, m_iOverall_Kbitrate %d", iUseableSize, m_iOverall_Kbitrate);

				if (m_Stream.freeBufSize() > 1024 * 300)
					PRINT_LOG(LOG_LEVEL_CRITICAL , "true");
				else
					PRINT_LOG(LOG_LEVEL_CRITICAL , "false");
				PRINT_LOG(LOG_LEVEL_CRITICAL , "m_Stream.freeBufSize()  %d", m_Stream.freeBufSize() );
*/

				long temp = 100;
				notify(VO_EVENT_BUFFERING_END, &temp);

				VOLOGR("RRR iUseableSize  VO_EVENT_BUFFERING_END");
				
				m_bBuffering = VO_FALSE;
			}
		}
	}
	else if (Manager_Seeking == m_StateManager && 0 == iLen && m_bStopedLastWhileSeeking)
	{
		m_bBuffering = VO_FALSE;

		long temp = 100;
		notify(VO_EVENT_BUFFERING_END, &temp);

		VOLOGR("RRR iUseableSize  VO_EVENT_BUFFERING_END");
	}

	if (m_Stream.write((VO_BYTE *)buf, iLen) != iLen)
	{
		switch (m_StateManager)
		{
		case Manager_Running:
			{
				;//
			}
			break;

		case Manager_Pausing:
			{
				m_StateManager = Manager_Paused;

				m_WMHTTP.stop(3000);
			}
			break;
		default:
			break;
		}
	}
}


VO_VOID CWMSPManager::ThreadMain()
{
	VO_S64 llSeekTime = 0;

	do 
	{
		VOLOGR("Seek Loop");
		m_bStopedLastWhileSeeking = VO_FALSE;
		m_WMHTTP.stop();
		m_bStopedLastWhileSeeking = VO_TRUE;
		m_Stream.close();
		m_parser_api.Close(m_fileparser);

		m_Stream.start();
		m_WMHTTP.play(m_url, m_llSeekTime);
		llSeekTime = m_llSeekTime;

		VO_FILE_SOURCE source;
		source.nFlag	= 0x102;//0x00020101;
		source.pSource	= m_PD_param.mFilePath;
		source.nReserve	= (VO_U32)this;

		VO_SOURCE_OPENPARAM param={0};//TODO
		param.nFlag			= source.nFlag;
		param.pSource		= &source;
		param.pSourceOP		= &m_opFile;
		param.pMemOP		= m_source_param.pMemOP;
		if (m_sourcedrm_callbcak.fCallback)
			param.pDrmCB	= &m_sourcedrm_callbcak;

		if (m_parser_api.Open(&m_fileparser, &param) != 0)
		{
			VOLOGR("Seek Fail");
			m_parser_api.Close = 0;
			m_StateManager = Manager_Stop;

			notify(VO_PD_EVENT_SERVER_WRONGRESPONSE, 0);
		}

	} while (llSeekTime != m_llSeekTime);

	m_bNeedCBSeekPos = VO_TRUE;
	m_StateManager = Manager_Running;

/*
	m_bBuffering = VO_FALSE;

	long temp = 100;
	notify(VO_EVENT_BUFFERING_END, &temp);

	PRINT_LOG(LOG_LEVEL_CRITICAL , "RRR iUseableSize  VO_EVENT_BUFFERING_END");
*/
}
