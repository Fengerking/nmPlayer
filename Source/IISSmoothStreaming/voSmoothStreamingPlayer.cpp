#if defined LINUX
#include <dlfcn.h>
#include "voOSFunc.h"
#endif

#include "voSmoothStreamingPlayer.h"
#include "voLog.h"
#include "CCheckTsPacketSize.h"
#include "cmnMemory.h"
#include "voOSFunc.h"

#pragma warning (disable : 4996)

VO_VOID DataCallback( VO_VOID * ptr_obj , OMX_BUFFERHEADERTYPE * ptr_buffer );

extern VO_MEM_OPERATOR	g_memOP;

voSmoothStreamingPlayer::voSmoothStreamingPlayer(void)
	: COMXEngine ()
	, m_pCompSrc (NULL)
	, m_pCompAD (NULL)
	, m_pCompVD (NULL)
	, m_pCompAR (NULL)
	, m_pCompVR (NULL)
	, m_pCompClock (NULL)
	, m_hThreadMedia (NULL)
	, m_bMediaStop (false)
{
	cmnMemFillPointer(1);
}

voSmoothStreamingPlayer::~voSmoothStreamingPlayer ()
{
}

OMX_ERRORTYPE voSmoothStreamingPlayer::SetDisplayArea (OMX_VO_DISPLAYAREATYPE * pDisplay)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	memcpy (&m_dspArea, pDisplay, sizeof (OMX_VO_DISPLAYAREATYPE));

	return m_OMXEng.SetDisplayArea (m_hEngine, pDisplay);
}

OMX_ERRORTYPE voSmoothStreamingPlayer::Playback (VOME_SOURCECONTENTTYPE * pSource)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

//	return m_OMXEng.Playback (m_hEngine, pSource);

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	errType = AddComponent ("OMX.VisualOn.DataSource", &m_pCompSrc);
	if (errType != OMX_ErrorNone)
		return errType;
	errType = AddComponent ("OMX.VisualOn.Clock.Time", &m_pCompClock);

	errType = AddComponent ("OMX.VisualOn.Audio.Decoder.XXX", &m_pCompAD);
	errType = AddComponent ("OMX.VisualOn.Audio.Sink", &m_pCompAR);

	errType = AddComponent ("OMX.VisualOn.Video.Decoder.XXX", &m_pCompVD);
	errType = AddComponent ("OMX.VisualOn.Video.Sink", &m_pCompVR);

	errType = ConnectPorts (m_pCompSrc, 0, m_pCompAD, 0, OMX_TRUE);
	errType = ConnectPorts (m_pCompAD, 1, m_pCompAR, 0, OMX_TRUE);

	errType = ConnectPorts (m_pCompSrc, 1, m_pCompVD, 0, OMX_TRUE);
	errType = ConnectPorts (m_pCompVD, 1, m_pCompVR, 0, OMX_TRUE);

	errType = ConnectPorts (m_pCompClock, 0, m_pCompSrc, 2, OMX_TRUE);
	errType = ConnectPorts (m_pCompClock, 1, m_pCompVR, 1, OMX_TRUE);
	errType = ConnectPorts (m_pCompClock, 2, m_pCompAR, 1, OMX_TRUE);

	OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE clkRef;
	clkRef.eClock = OMX_TIME_RefClockAudio;
//	clkRef.eClock = OMX_TIME_RefClockVideo;
	m_pCompClock->SetConfig (m_pCompClock, OMX_IndexConfigTimeActiveRefClock, &clkRef);

	//errType = Run ();

	errType = SetDataSource();

	return errType;
}

OMX_ERRORTYPE voSmoothStreamingPlayer::Close (void)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	if( !m_is_reloading )
		m_streaming->close();

	return m_OMXEng.Close (m_hEngine);
}

OMX_ERRORTYPE voSmoothStreamingPlayer::Start()
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	OMX_ERRORTYPE errType = m_OMXEng.Run (m_hEngine);

	if( !m_streaming->is_live() )
	{
		m_streaming->run ();
	}
	
	return OMX_ErrorNone;
}


OMX_ERRORTYPE voSmoothStreamingPlayer::SetDataSource (void)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	OMX_ERRORTYPE errType = m_OMXEng.Run (m_hEngine);

	if (m_pCompVD != NULL && m_pCompAD != NULL )
	{
		m_bMediaStop = false;

		m_streaming = new vo_smooth_streaming ();

		m_streaming->close();
		change_buffer_time( 20 );
		m_streaming->set_datacallback( this , DataCallback );
		
		VO_BOOL bRC = m_streaming->open ("http://10.2.64.102/Beautiful_Girl.ism/Manifest" , VO_FALSE );
		//VO_BOOL bRC = m_streaming->open ("http://10.2.64.102/channel1.isml/manifest" , VO_FALSE );
	}

	return errType;
}

OMX_ERRORTYPE voSmoothStreamingPlayer::Pause (void)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	if( !m_streaming->is_live() )
	{
		m_streaming->pause();
	}

	return m_OMXEng.Pause (m_hEngine);
}

OMX_ERRORTYPE voSmoothStreamingPlayer::Stop (void)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	return m_OMXEng.Stop (m_hEngine);
}

OMX_ERRORTYPE voSmoothStreamingPlayer::GetDuration (OMX_S32 * pDuration)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	*pDuration = m_streaming->get_duration();

	//For debug
	*pDuration = 258920;

	VOLOGE( "Duration: %d" , *pDuration );

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voSmoothStreamingPlayer::GetCurPos (OMX_S32 * pCurPos)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	if( m_streaming->is_live() )
	{
		*pCurPos = 0;
		return OMX_ErrorNone;
	}
	else
	{
		OMX_ERRORTYPE type = COMXEngine::GetCurPos( pCurPos );
		//VOLOGE( "Duration: %d" , *pCurPos );
		return type;
	}
}

OMX_ERRORTYPE voSmoothStreamingPlayer::SetCurPos (OMX_S32 nCurPos)
{
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	if( !m_streaming->is_live())
	{
		COMXEngine::SetCurPos( m_streaming->set_pos( nCurPos ) );
	}
	return OMX_ErrorNone;
}

/*
int voSmoothStreamingPlayer::ReadMediaDataProc (OMX_PTR pParam)
{
	voSmoothStreamingPlayer * pPlayer = (voSmoothStreamingPlayer *) pParam;

	pPlayer->ReadMediaDataLoop ();

	return 0;
}

int voSmoothStreamingPlayer::ReadMediaDataLoop (void)
{
	m_streaming.close();
	change_buffer_time( 20 );
	m_streaming.set_datacallback( this , DataCallback );
	//m_streaming.open( "http://mediadl.microsoft.com/mediadl/iisnet/smoothmedia/Experience/BigBuckBunny_720p.ism/Manifest" , VO_TRUE );
	m_streaming.open( "http://10.2.64.102/Beautiful_Girl.ism/Manifest" , VO_TRUE );
	//m_streaming.open( "http://10.2.64.102/channel1.isml/manifest" , VO_TRUE );

	return 1;
}*/

VO_VOID voSmoothStreamingPlayer::data_callback( OMX_BUFFERHEADERTYPE * ptr_buffer )
{
	OMX_ERRORTYPE errType = OMX_ErrorUndefined;
	VO_S32 trycount = 0;

	while (errType != OMX_ErrorNone && trycount < 30 )
	{
		errType = m_pCompSrc->SetParameter (m_pCompSrc, (OMX_INDEXTYPE)OMX_VO_IndexSendBuffer, ptr_buffer);

		if (errType != OMX_ErrorNone)
		{
			voOS_Sleep (100);
			trycount++;
		}
	}

	if( trycount >= 30 )
	{
		if(ptr_buffer->nOutputPortIndex == 0)
		{
			VOLOGE( " Audio Frame Drop!" );
		}else
			VOLOGE( " video Frame Drop!" );
	}
}

VO_VOID voSmoothStreamingPlayer::change_buffer_time( VO_S32 time )
{
	VO_S32 playbackcache = 0;
	
	if( time <= 15 )
	{
		playbackcache = time < 5 ? 5000 : ( time + 5 ) * 1000;
	}
	else
	{
		playbackcache = time * 1000;
	}

	OMX_TIME_CONFIG_TIMESTAMPTYPE config;
	config.nSize = sizeof( OMX_TIME_CONFIG_TIMESTAMPTYPE );
	config.nPortIndex = 0;
	config.nTimestamp = playbackcache;
	m_pCompSrc->SetConfig( m_pCompSrc , (OMX_INDEXTYPE)OMX_VO_IndexConfigTimeDuration , &config );
	config.nPortIndex = 1;
	m_pCompSrc->SetConfig( m_pCompSrc , (OMX_INDEXTYPE)OMX_VO_IndexConfigTimeDuration , &config );
}

int	voSmoothStreamingPlayer::CallBackFunc (int nID, void * pParam1, void * pParam2)
{
	return 0;
}


VO_VOID DataCallback( VO_VOID * ptr_obj , OMX_BUFFERHEADERTYPE * ptr_buffer )
{
	voSmoothStreamingPlayer * ptr_player = ( voSmoothStreamingPlayer * )ptr_obj;

	ptr_player->data_callback( ptr_buffer );
}

