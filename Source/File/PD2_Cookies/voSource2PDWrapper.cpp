
#include "voSource2PDWrapper.h"
#include "voLog.h"
#include "voIndex.h"
#include "voCheck.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voSource2PDWrapper::voSource2PDWrapper()
: m_handle(NULL)
, m_hCheck(NULL)
, m_nBufferTime(-1)
{
	memset( &m_PD_param , 0 , sizeof( m_PD_param ) );
}

voSource2PDWrapper::~voSource2PDWrapper()
{
	;
}

VO_U32 voSource2PDWrapper::Init(VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pParam )
{
	m_nFlag = nFlag;

	if (pParam != NULL && pParam->strWorkPath != NULL )
	{
		m_pstrWorkPath = new VO_TCHAR[vostrlen(pParam->strWorkPath) + 1];
		vostrcpy(m_pstrWorkPath,pParam->strWorkPath);
		m_pstrWorkPath[vostrlen(pParam->strWorkPath)] = '\0';
	}

	if( nFlag & VO_SOURCE2_FLAG_OPEN_URL )
	{
		vostrcpy( m_Url , (VO_TCHAR*)pSource );

		vo_PD_manager * ptr_PD = new vo_PD_manager();

		if(!ptr_PD)
		{
			VOLOGE("create PD manager fail!!");
			return VO_ERR_OUTOF_MEMORY;
		}

		ptr_PD->set_source2flag();
		ptr_PD->setWorkpath( m_pstrWorkPath );
		if( pParam && pParam->pInitParam )
		{
			memcpy( &m_PD_param , (VO_PTR)pParam->pInitParam , sizeof( m_PD_param ) );
		}
		else
		{
			VOLOGE("PD Init Param is null");
		}

		m_handle = ptr_PD;

		return VO_RET_SOURCE2_OK;
	}

	return VO_RET_SOURCE2_FAIL;
}

VO_U32 voSource2PDWrapper::Start()
{
	vo_PD_manager * ptr_PD = (vo_PD_manager *)m_handle;

	if( !ptr_PD )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 cmd = VO_QUICKRESPONSE_RUN;
	ptr_PD->set_sourceparam( VOID_STREAMING_QUICKRESPONSE_COMMAND , (VO_PTR)&cmd ); 
	return VO_RET_SOURCE2_OK;
}

VO_U32 voSource2PDWrapper::Pause()
{
	vo_PD_manager * ptr_PD = (vo_PD_manager *)m_handle;

	if( !ptr_PD )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 cmd = VO_QUICKRESPONSE_PAUSE;
	ptr_PD->set_sourceparam( VOID_STREAMING_QUICKRESPONSE_COMMAND , (VO_PTR)&cmd ); 
	return VO_RET_SOURCE2_OK;
}

VO_U32 voSource2PDWrapper::Stop()
{
	m_nCurrentTimeStamp = 0;

	vo_PD_manager * ptr_PD = (vo_PD_manager *)m_handle;

	if( !ptr_PD )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 cmd = VO_QUICKRESPONSE_STOP;
	ptr_PD->set_sourceparam( VOID_STREAMING_QUICKRESPONSE_COMMAND , (VO_PTR)&cmd ); 

	return VO_RET_SOURCE2_OK;
}

VO_U32 voSource2PDWrapper::Seek(VO_U64* pTimeStamp)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;

	vo_PD_manager * ptr_PD = (vo_PD_manager *)m_handle;

	voSourceBaseWrapper::Seek( pTimeStamp );

	if( !ptr_PD )
	{
		nResult = VO_RET_SOURCE2_FAIL;
	}
	else
	{
		ptr_PD->moveto( (VO_S64)*pTimeStamp );
	}

	//if(m_nFlag & VO_SOURCE2_FLAG_OPEN_ASYNC && m_ptr_EventCallback && m_ptr_EventCallback->SendEvent)
	//{
	//	VOLOGI("Seek---TimeStamp=%llu,nResult=%lu", *pTimeStamp,nResult);		
	//	m_ptr_EventCallback->SendEvent(m_ptr_EventCallback->pUserData, VO_EVENTID_SOURCE2_SEEKCOMPLETE, (VO_U32)pTimeStamp, (VO_U32)&nResult);
	//}

	return nResult;
}

VO_U32 voSource2PDWrapper::GetParam(VO_U32 nParamID, VO_PTR pParam)
{
	vo_PD_manager * ptr_PD = (vo_PD_manager *)m_handle;

	if( !ptr_PD )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	return ptr_PD->get_sourceparam( nParamID , pParam );
}

VO_U32 voSource2PDWrapper::SetParam(VO_U32 nParamID, VO_PTR pParam)
{
	switch( nParamID )
	{
	case VO_PID_SOURCE2_DRMCALLBACK:
		{
			if (pParam)
			{
				memcpy(m_ptr_drmcallback,pParam,sizeof(VO_SOURCEDRM_CALLBACK2));
			}
		}
		break;

	case VO_PID_COMMON_LOGFUNC:
		{
			VOLOGINIT(m_pstrWorkPath);

			return VO_RET_SOURCE2_OK;
		}
		break;
	case VO_PID_SOURCE2_LIBOP:
		{
			nParamID = VOID_STREAMING_LIBOP;
		}
		break;
	case VO_PID_SOURCE2_SAMPLECALLBACK:
		{
			return VO_RET_SOURCE2_FAIL;
		}
		break;
	case VO_PID_SOURCE2_HTTPVERIFICATIONCALLBACK:
		{
			nParamID = VO_PID_SOURCE_HTTPVERIFICATIONCALLBACK;
		}
		break;
	case VO_PID_SOURCE2_DOHTTPVERIFICATION:
		{
			nParamID = VO_PID_SOURCE_DOHTTPVERIFICATION;
		}
		break;
	case VO_PID_SOURCE2_EVENTCALLBACK:
		{
			m_ptr_EventCallback = (VO_SOURCE2_EVENTCALLBACK *)pParam;
			return VO_RET_SOURCE2_OK;
		}
		break;
	case VO_PID_SOURCE2_WORKPATH:
		{
			nParamID = VO_PID_SOURCE_WORKPATH;
		}
		break;
	case VO_PID_SOURCE2_BUFFER_STARTBUFFERINGTIME:
		{
			VO_U32* pvalue = ( VO_U32*)pParam;
			m_PD_param.nBufferTime = (VO_U32)(*pvalue);

			m_nBufferTime = (*pvalue);

			VOLOGI( "BufferTime: %d" , *pvalue );
		}
		break;
	}

	vo_PD_manager * ptr_PD = (vo_PD_manager *)m_handle;

	if( !ptr_PD )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	return ptr_PD->set_sourceparam( nParamID , pParam ); 
}

VO_VOID	voSource2PDWrapper::NotifyPDOpenEvent( VO_U32 nRet )
{
	VO_U32 nResult = nRet;
	if(m_nFlag & VO_SOURCE2_FLAG_OPEN_ASYNC && m_ptr_EventCallback && m_ptr_EventCallback->SendEvent )
	{
		VOLOGI("PDOnOpen---nResult=%lu", nResult);
		m_ptr_EventCallback->SendEvent(m_ptr_EventCallback->pUserData, VO_EVENTID_SOURCE2_OPENCOMPLETE, (VO_U32)&nResult, 0 );
	}
}

VO_U32	voSource2PDWrapper::OnOpen()
{
	VO_U32 nRet = voCheckLibInit(&m_hCheck,VO_INDEX_SRC_HTTP, VO_LCS_WORKPATH_FLAG, 0, (VO_PTR)m_pstrWorkPath);
	if( VO_ERR_NONE != nRet )
	{
		if(m_hCheck != NULL)
		{
			voCheckLibUninit(m_hCheck);
		}
		m_hCheck = NULL;
		VOLOGE("License check failed");
		return nRet;
	}

	VO_SOURCE_OPENPARAM param;
	memset( &param , 0 , sizeof( VO_SOURCE_OPENPARAM ) );
	param.pSource = ( VO_PTR )m_Url;
	param.pSourceOP = ( VO_PTR )m_ptr_EventCallback;
	param.pDrmCB = ( VO_SOURCEDRM_CALLBACK * )m_ptr_drmcallback;

	//work around to disable the set-in init PD parameters
	memset( &m_PD_param , 0 , sizeof(m_PD_param) );
	if( m_nBufferTime >= 0 )
	{
		m_PD_param.nBufferTime = m_nBufferTime / 1000;
	}
	param.nReserve = (VO_U32)&m_PD_param;

	vo_PD_manager * ptr_PD = (vo_PD_manager *)m_handle;

	if( !ptr_PD )
	{
		NotifyPDOpenEvent( VO_RET_SOURCE2_OPENFAIL );
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	VO_BOOL bret = ptr_PD->open( &param );

	if( !bret )
	{
		VOLOGE("PD open fail");
	    delete ptr_PD;
		m_handle = 0;

		NotifyPDOpenEvent( VO_RET_SOURCE2_OPENFAIL );

		return VO_RET_SOURCE2_OPENFAIL;
	}

	VO_U32 ret = ptr_PD->set_sourceparam( VOID_STREAMING_OPENURL , 0 );
	if( VO_ERR_NONE != ret )
	{
		VOLOGE("PD open url fail");
	    delete ptr_PD;
		m_handle = 0;

		NotifyPDOpenEvent( VO_RET_SOURCE2_OPENFAIL );

		return VO_RET_SOURCE2_OPENFAIL;
	}

	NotifyPDOpenEvent( VO_RET_SOURCE2_OK );

	return VO_RET_SOURCE2_OK;
}

VO_U32	voSource2PDWrapper::OnClose()
{
	if(m_hCheck != NULL)
	{
		voCheckLibUninit(m_hCheck);
		m_hCheck = NULL;
	}

	vo_PD_manager * ptr_PD = (vo_PD_manager *)m_handle;

	if( !ptr_PD )
		return VO_RET_SOURCE2_OK;

	ptr_PD->close();
	delete ptr_PD;

	m_handle = 0;
    return VO_RET_SOURCE2_OK;
}

VO_U32	voSource2PDWrapper::OnGetDuration(VO_U64 * pDuration)
{
	vo_PD_manager* ptr_PD = (vo_PD_manager*)m_handle;

	if( !ptr_PD )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_SOURCE_INFO source_info;
	memset( &source_info , 0 , sizeof( VO_SOURCE_INFO ) );
	ptr_PD->get_sourceinfo( &source_info );

	*pDuration = source_info.Duration;

	return VO_RET_SOURCE2_OK;
}

VO_U32	voSource2PDWrapper::OnGetTrackCnt(VO_U32 * pTraCnt)
{
	vo_PD_manager* ptr_PD = (vo_PD_manager*)m_handle;

	if( !ptr_PD )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_SOURCE_INFO source_info;
	memset( &source_info , 0 , sizeof( VO_SOURCE_INFO ) );
	ptr_PD->get_sourceinfo( &source_info );

	*pTraCnt = source_info.Tracks;

	return VO_RET_SOURCE2_OK;
}

VO_U32	voSource2PDWrapper::OnGetSample(VO_SOURCE2_TRACK_TYPE nOutPutType , VO_SOURCE_SAMPLE * pSample)
{
	if(!m_handle || !pSample)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	if( nOutPutType == VO_SOURCE2_TT_SUBTITLE )
		return VO_RET_SOURCE2_OUTPUTNOTFOUND;

	VO_U32 nTrack = 0;

	if( nOutPutType == VO_SOURCE2_TT_VIDEO && m_uVideoTrack != 0xffffffff )
	{
		nTrack = m_uVideoTrack;
	}
	else if( nOutPutType == VO_SOURCE2_TT_AUDIO && m_uAudioTrack != 0xffffffff )
	{
		nTrack = m_uAudioTrack;
	}
	else
	{
		return VO_RET_SOURCE2_OUTPUTNOTFOUND;
	}

	vo_PD_manager * ptr_PD = (vo_PD_manager *)m_handle;
	
	if( !ptr_PD )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 rc = ptr_PD->get_sample( nTrack , pSample);

	if(VO_ERR_SOURCE_OK != rc)
	{
		if(VO_ERR_SOURCE_END == rc)
			return VO_RET_SOURCE2_END;
		else if( VO_ERR_SOURCE_NEEDRETRY == rc )
			return VO_RET_SOURCE2_NEEDRETRY;
		else
			return VO_RET_SOURCE2_FAIL;
		
	}

	return VO_RET_SOURCE2_OK;
}

VO_U32	voSource2PDWrapper::OnGetTrackAvailable(VO_U32 uTrackIndex , VO_BOOL * beAlai)
{
	* beAlai = VO_TRUE;
	return VO_RET_SOURCE2_OK;
}

VO_U32	voSource2PDWrapper::OnGetTrackInfo(VO_U32 uTrackIndex , VO_SOURCE_TRACKINFO * pTrackInfo)
{
	vo_PD_manager *ptr_PD = ( vo_PD_manager *)m_handle;

	if( !ptr_PD || !pTrackInfo)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = VO_RET_SOURCE2_OK;
	ret = ptr_PD->get_trackinfo( uTrackIndex , pTrackInfo );
	return ret;
}

VO_U32	voSource2PDWrapper::OnGetTrackFourCC(VO_U32 uTrackIndex , VO_U32 * pFourCC)
{
	vo_PD_manager *ptr_PD = ( vo_PD_manager *)m_handle;

	if( !ptr_PD || !pFourCC)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = VO_RET_SOURCE2_OK;
	ret = ptr_PD->get_trackparam( uTrackIndex , VO_PID_SOURCE_CODECCC, pFourCC );
	return ret;
}

VO_U32	voSource2PDWrapper::OnGetTrackMaxSampleSize(VO_U32 uTrackIndex , VO_U32 * pMaxSampleSize)
{
	vo_PD_manager *ptr_PD = ( vo_PD_manager *)m_handle;

	if( !ptr_PD || !pMaxSampleSize)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = VO_RET_SOURCE2_OK;
	ret = ptr_PD->get_trackparam( uTrackIndex , VO_PID_SOURCE_MAXSAMPLESIZE, pMaxSampleSize );
	return ret;
}

VO_U32	voSource2PDWrapper::OnGetAudioFormat(VO_U32 uTrackIndex , VO_AUDIO_FORMAT * pAudioFormat)
{
	vo_PD_manager *ptr_PD = ( vo_PD_manager *)m_handle;

	if( !ptr_PD || !pAudioFormat)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = VO_RET_SOURCE2_OK;
	ret = ptr_PD->get_trackparam( uTrackIndex ,VO_PID_AUDIO_FORMAT, pAudioFormat );
	return ret;
}

VO_U32	voSource2PDWrapper::OnGetVideoFormat(VO_U32 uTrackIndex , VO_VIDEO_FORMAT * pVideoFormat)
{
	vo_PD_manager *ptr_PD = ( vo_PD_manager *)m_handle;

	if( !ptr_PD || !pVideoFormat)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = VO_RET_SOURCE2_OK;
	ret = ptr_PD->get_trackparam( uTrackIndex , VO_PID_VIDEO_FORMAT, pVideoFormat);
	return ret;
}

VO_U32	voSource2PDWrapper::OnGetWaveFormatEx(VO_U32 uTrackIndex , VO_PBYTE * pExData)
{
	vo_PD_manager *ptr_PD = ( vo_PD_manager *)m_handle;

	if( !ptr_PD || !pExData)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = VO_RET_SOURCE2_OK;
	ret = ptr_PD->get_trackparam( uTrackIndex , VO_PID_SOURCE_WAVEFORMATEX, pExData );
	return ret;
}

VO_U32	voSource2PDWrapper::OnGetVideoBitMapInfoHead(VO_U32 uTrackIndex , VO_PBYTE * pBitMap)
{
	vo_PD_manager *ptr_PD = ( vo_PD_manager *)m_handle;

	if( !ptr_PD || !pBitMap)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = VO_RET_SOURCE2_OK;
	ret = ptr_PD->get_trackparam( uTrackIndex , VO_PID_SOURCE_BITMAPINFOHEADER, pBitMap);
	return ret;
}

VO_U32	voSource2PDWrapper::OnGetVideoUpSideDown(VO_U32 uTrackIndex , VO_BOOL * pUpSideDown)
{
	vo_PD_manager *ptr_PD = ( vo_PD_manager *)m_handle;

	if( !ptr_PD || !pUpSideDown)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = VO_RET_SOURCE2_OK;
	ret = ptr_PD->get_trackparam( uTrackIndex, VO_PID_VIDEO_UPSIDEDOWN, pUpSideDown );
	return ret;
}

VO_U32	voSource2PDWrapper::OnGetLanguage(VO_U32 uTrackIndex , VO_CHAR ** pLanguage)
{
	vo_PD_manager *ptr_PD = ( vo_PD_manager *)m_handle;

	if( !ptr_PD || !pLanguage)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = VO_RET_SOURCE2_OK;
	ret = ptr_PD->get_trackparam( uTrackIndex, VO_PID_SOURCE_TRACK_LANGUAGE,pLanguage);
	return ret;
}