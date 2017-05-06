#include "voCLiveDataSource.h"
#include "voOSFunc.h"

#define LOG_TAG "voCLiveDataSource"
#include "voLog.h"

VO_VOID VO_API status_callback (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);
VO_VOID VO_API data_callback (VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData);

voCLiveDataSource::voCLiveDataSource(VO_PTR hInst, VO_U32 nFormat, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP, VO_SOURCEDRM_CALLBACK * pDrmCB)
	: voCDataSource (hInst, nFormat, pMemOP, pFileOP, pDrmCB)
	,m_pLiveSource(NULL)
	,m_should_stop(VO_FALSE)
	,m_is_started(VO_FALSE)
	,m_nLiveSourceDuration(0)
{
	memset( m_url , 0 , sizeof( m_url ) );
}

voCLiveDataSource::~voCLiveDataSource(void)
{
}

VO_U32 voCLiveDataSource::LoadSource (const VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength)
{
#ifdef _WIN32
	memset (m_url, 0, sizeof (m_url));
	WideCharToMultiByte (CP_ACP, 0, (VO_TCHAR *)pSource, -1, m_url, 1024, NULL, NULL);
//	strcpy( m_url , (const VO_CHAR * )pSource );
#else
	strcpy( m_url , (const VO_CHAR * )pSource );
#endif // _WIN32
	m_should_stop = VO_FALSE;
	m_pLiveSource = new CBaseLiveSource( GetLiveType( m_nFormat ) );

	if (m_pLiveSource == NULL)
		return VO_ERR_OUTOF_MEMORY;

    if( m_pLibOP )
	{
		m_pLiveSource->SetLibOperator( m_pLibOP );
		m_pLiveSource->SetParam(VO_PID_LIVESRC_LIBOP, m_pLibOP);
	}

	m_pLiveSource->SetWorkPath(m_pWorkPath);

	int nRC;
	nRC = m_pLiveSource->Open( this , status_callback , data_callback );
	if(nRC)
		return nRC;

	nRC = m_pLiveSource->SetParam(VO_PID_LIVESRC_DRMCALLBACK, m_pDrmCB);	

	nRC = m_pLiveSource->SetDataSource( m_url , 0 );
	if(nRC)
		return nRC;
	m_is_started = VO_TRUE;

	nRC = m_pLiveSource->GetParam(VO_PID_LIVESRC_DURATION, &m_nLiveSourceDuration);
	if(VO_ERR_LIVESRC_OK == nRC)
	{
		m_filInfo.Duration = (VO_U32)m_nLiveSourceDuration;		
	}

	return 0;
}

VO_U32 voCLiveDataSource::CloseSource (void)
{
	if( m_pLiveSource )
		m_pLiveSource->Close();

	return voCDataSource::CloseSource();
}

VO_U32 voCLiveDataSource::Start(void)
{
	if( m_pLiveSource )
	{
		if( !m_is_started )
		{
			m_pLiveSource->Open( this , status_callback , data_callback );
			m_pLiveSource->SetParam(VO_PID_LIVESRC_DRMCALLBACK, m_pDrmCB);
			m_pLiveSource->SetDataSource( m_url , 0 );
			m_is_started = VO_TRUE;
		}
		m_pLiveSource->Start();
	}

	return voCDataSource::Start();
}

VO_U32 voCLiveDataSource::Pause(void)
{
	if( m_pLiveSource )
		m_pLiveSource->Pause();

	return voCDataSource::Pause();
}

VO_U32 voCLiveDataSource::Stop(void)
{
	m_should_stop = VO_TRUE;
	if( m_pLiveSource )
		m_pLiveSource->Stop();
	m_should_stop = VO_FALSE;

	return voCDataSource::Stop();
}

VO_U32 voCLiveDataSource::SetTrackPos (VO_U32 nTrack, VO_S64 * pPos)
{
//	voCAutoLock lock (&m_mtBuffer);
	int ret = 0;

	if (m_pVideoTrack != NULL && m_nVideoTrack == (int)nTrack)
	{
		m_pVideoTrack->Flush ();

		VOLOGI( "set position %d", (int)(*pPos));
		m_should_stop = VO_TRUE;
		ret = m_pLiveSource->Seek(*pPos);
		m_should_stop = VO_FALSE;

		VOLOGI( "set position end");
	}

	if (m_pAudioTrack != NULL && m_nAudioTrack == (int)nTrack)
	{
		m_pAudioTrack->Flush ();
		if(m_pVideoTrack == NULL)
		{
			m_should_stop = VO_TRUE;				
			ret = m_pLiveSource->Seek(*pPos);
			m_should_stop = VO_FALSE;
		}
	}

	return ret;
}

VO_LIVESRC_FORMATTYPE voCLiveDataSource::GetLiveType( VO_U32 nType )
{
	switch( nType )
	{
	case VO_FILE_FFSTREAMING_HLS:
		{
			VOLOGE( "Live Streaming!" );
		}
		return VO_LIVESRC_FT_HTTP_LS;
	case VO_FILE_FFSTREAMING_SSS:
		{
			VOLOGE( "Smooth Streaming!" );
		}
		return VO_LIVESRC_FT_IIS_SS;
	case VO_FILE_FFSTREAMING_DASH:
		{
			VOLOGE( "Dash Streaming!" );
		}
		return VO_LIVESRC_FT_DASH;
	default:
		VOLOGE( "What is this? %x" , (int)nType );
	}

	return VO_LIVESRC_FT_MAX;
}

VO_VOID voCLiveDataSource::NotifyStatus ( VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2 )
{
	;
}

VO_VOID voCLiveDataSource::NotifyData ( VO_U16 nOutputType, VO_PTR pData )
{
	int errType = VOMP_ERR_Unknown;

	VO_LIVESRC_SAMPLE * ptr_sample = ( VO_LIVESRC_SAMPLE * )pData;

	VOMP_BUFFERTYPE buffer;
	memset( &buffer , 0 , sizeof( VOMP_BUFFERTYPE ) );
	buffer.llTime = ptr_sample->Sample.Time;
	buffer.nFlag = ptr_sample->Sample.Flag;
	buffer.nSize = ptr_sample->Sample.Size;
	buffer.pBuffer = ptr_sample->Sample.Buffer;
	buffer.pData = (void *)ptr_sample->nCodecType;

	while (errType != VOMP_ERR_None)
	{
		if( m_should_stop )
			break;

		if( nOutputType == 1 )
		{
/*
			if (ptr_sample->nCodecType != m_sAudioTrackInfo.Codec)
			{
				m_sAudioTrackInfo.Codec = ptr_sample->nCodecType;
				buffer.nFlag = VOMP_FLAG_BUFFER_NEW_FORMAT;
			}
*/
			errType = AddBuffer (VOMP_SS_Audio, &buffer);

		}
		else if( nOutputType == 2 )
		{
/*
			if (ptr_sample->nCodecType != m_sAudioTrackInfo.Codec)
			{
				m_sAudioTrackInfo.Codec = ptr_sample->nCodecType;
				buffer.nFlag = VOMP_FLAG_BUFFER_NEW_FORMAT;
			}
*/
			errType = AddBuffer (VOMP_SS_Video, &buffer);
		}

		if (errType != VOMP_ERR_None)
			voOS_Sleep(20);
	}
}

void voCLiveDataSource::SetLibOperator (VO_LIB_OPERATOR * pLibOP)
{
    if( m_pLiveSource )
        m_pLiveSource->SetLibOperator( pLibOP );

    voCDataSource::SetLibOperator( pLibOP );
}

VO_VOID VO_API status_callback (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
	voCLiveDataSource * ptr_datasource = (voCLiveDataSource *)pUserData;

	ptr_datasource->NotifyStatus( nID , nParam1 , nParam2 );
}

VO_VOID VO_API data_callback (VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData)
{
	voCLiveDataSource * ptr_datasource = (voCLiveDataSource *)pUserData;

	ptr_datasource->NotifyData( nOutputType , pData );
}