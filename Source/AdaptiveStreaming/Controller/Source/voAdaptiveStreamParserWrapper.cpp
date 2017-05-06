#include "voAdaptiveStreamParserWrapper.h"
#include "voAdaptiveStreamingFileParser.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

typedef VO_S32 (VO_API * pvoGetAdaptiveStreamHLSAPI)(VO_ADAPTIVESTREAM_PARSER_API* pHandle);

voAdaptiveStreamParserWrapper::voAdaptiveStreamParserWrapper( VO_U32 adaptivestream_type , VO_SOURCE2_LIB_FUNC * pLibOp, VO_TCHAR *pWorkPath, VO_LOG_PRINT_CB * pVologCB )
:m_parserhandle(0)
{
	memset( &m_api , 0 , sizeof(VO_ADAPTIVESTREAM_PARSER_API) );

	if( pLibOp )
		SetLibOperator( ( VO_LIB_OPERATOR* )pLibOp );
	if( pWorkPath )
		SetWorkPath( pWorkPath );
	
	switch( adaptivestream_type )
	{
	case VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS:
		{
			vostrcpy( m_szDllFile , _T("voAdaptiveStreamHLS") );
			vostrcpy( m_szAPIName , _T("voGetAdaptiveStreamHLSAPI") );
		}
		break;
	case VO_ADAPTIVESTREAMPARSER_STREAMTYPE_ISS:
		{
			vostrcpy( m_szDllFile , _T("voAdaptiveStreamISS") );
			vostrcpy( m_szAPIName , _T("voGetAdaptiveStreamISSAPI") );
		}
		break;
	case VO_ADAPTIVESTREAMPARSER_STREAMTYPE_DASH:
		{
			vostrcpy( m_szDllFile , _T("voAdaptiveStreamDASH") );
			vostrcpy( m_szAPIName , _T("voGetAdaptiveStreamDASHAPI") );
		}
		break;
	}

	if( LoadLib(NULL) == 0 )
		return;

	pvoGetAdaptiveStreamHLSAPI pAPI = (pvoGetAdaptiveStreamHLSAPI) m_pAPIEntry;
	pAPI( &m_api );

	if( pVologCB)
		SetParam( VO_PID_COMMON_LOGFUNC, pVologCB );
}

voAdaptiveStreamParserWrapper::~voAdaptiveStreamParserWrapper(void)
{
}

VO_U32 voAdaptiveStreamParserWrapper::Init ( VO_ADAPTIVESTREAM_PLAYLISTDATA * pData , VO_SOURCE2_EVENTCALLBACK * pCallback )
{
	voCAutoLock lock( &m_lock );
	if (m_api.Init)
	{	
		VO_ADAPTIVESTREAMPARSER_INITPARAM initParam;
		initParam.uFlag = 0;
		initParam.pInitParam = NULL;
		initParam.strWorkPath =  m_pWorkPath;
		return m_api.Init( &m_parserhandle , pData , pCallback,&initParam );
	}
	else
		return VO_RET_SOURCE2_FORMATUNSUPPORT;

	
}

VO_U32 voAdaptiveStreamParserWrapper::UnInit ()
{
	voCAutoLock lock( &m_lock );
	if (m_api.UnInit)
		return m_api.UnInit( m_parserhandle );
	else
		return VO_RET_SOURCE2_OK;
}

VO_U32 voAdaptiveStreamParserWrapper::Open ()
{
	voCAutoLock lock( &m_lock );

	if (m_api.Open)
		return m_api.Open( m_parserhandle );
	else
		return VO_RET_SOURCE2_FORMATUNSUPPORT;
}

VO_U32 voAdaptiveStreamParserWrapper::Close ()
{
	voCAutoLock lock( &m_lock );

	if (m_api.Close)
		return m_api.Close( m_parserhandle );
	else
		return VO_RET_SOURCE2_FORMATUNSUPPORT;
}

VO_U32 voAdaptiveStreamParserWrapper::Start ()
{
	voCAutoLock lock( &m_lock );

	if (m_api.Start)
		return m_api.Start( m_parserhandle );
	else
		return VO_RET_SOURCE2_FORMATUNSUPPORT;
}

VO_U32 voAdaptiveStreamParserWrapper::Stop ()
{
	voCAutoLock lock( &m_lock );

	if (m_api.Stop)
		return m_api.Stop( m_parserhandle );
	else
		return VO_RET_SOURCE2_FORMATUNSUPPORT;
}

VO_U32 voAdaptiveStreamParserWrapper::Update ( VO_ADAPTIVESTREAM_PLAYLISTDATA * pData )
{
	voCAutoLock lock( &m_lock );

	if (m_api.Update)
		return m_api.Update( m_parserhandle , pData );
	else
		return VO_RET_SOURCE2_FORMATUNSUPPORT;
}

VO_U32 voAdaptiveStreamParserWrapper::GetChunk ( VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID ,  VO_ADAPTIVESTREAMPARSER_CHUNK **ppChunk )
{
	voCAutoLock lock( &m_lock );

	if (m_api.GetChunk)
		return m_api.GetChunk( m_parserhandle , uID , ppChunk );
	else
		return VO_RET_SOURCE2_FORMATUNSUPPORT;
}

VO_U32 voAdaptiveStreamParserWrapper::Seek ( VO_U64 * pTimeStamp, VO_ADAPTIVESTREAMPARSER_SEEKMODE sSeekMode )
{
	voCAutoLock lock( &m_lock );

	if (m_api.Seek)
		return m_api.Seek( m_parserhandle , pTimeStamp, sSeekMode );
	else
		return VO_RET_SOURCE2_FORMATUNSUPPORT;
}

VO_U32 voAdaptiveStreamParserWrapper::GetDuration ( VO_U64 * pDuration)
{
	voCAutoLock lock( &m_lock );

	if (m_api.GetDuration)
		return m_api.GetDuration( m_parserhandle , pDuration );
	else
		return VO_RET_SOURCE2_FORMATUNSUPPORT;
}

VO_U32 voAdaptiveStreamParserWrapper::GetProgramCount ( VO_U32 *pProgramCount )
{
	voCAutoLock lock( &m_lock );

	if (m_api.GetProgramCount)
		return m_api.GetProgramCount( m_parserhandle , pProgramCount );
	else
		return VO_RET_SOURCE2_FORMATUNSUPPORT;
}

VO_U32 voAdaptiveStreamParserWrapper::GetProgramInfo ( VO_U32 uProgram , _PROGRAM_INFO  **ppProgramInfo )
{
	voCAutoLock lock( &m_lock );

	if (m_api.GetProgramInfo)
		return m_api.GetProgramInfo( m_parserhandle , uProgram , ppProgramInfo );
	else
		return VO_RET_SOURCE2_FORMATUNSUPPORT;
}

VO_U32 voAdaptiveStreamParserWrapper::GetCurTrackInfo ( VO_SOURCE2_TRACK_TYPE sTrackType , _TRACK_INFO ** ppTrackInfo )
{
	voCAutoLock lock( &m_lock );

	if (m_api.GetCurTrackInfo)
		return m_api.GetCurTrackInfo( m_parserhandle , sTrackType , ppTrackInfo );
	else
		return VO_RET_SOURCE2_FORMATUNSUPPORT;
}

VO_U32 voAdaptiveStreamParserWrapper::SelectProgram ( VO_U32 uProgram)
{
	voCAutoLock lock( &m_lock );

	if (m_api.SelectProgram)
		return m_api.SelectProgram( m_parserhandle , uProgram );
	else
		return VO_RET_SOURCE2_FORMATUNSUPPORT;
}

VO_U32 voAdaptiveStreamParserWrapper::SelectStream ( VO_U32 uStream)
{
	voCAutoLock lock( &m_lock );

	if (m_api.SelectStream)
		return m_api.SelectStream( m_parserhandle , uStream,  VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS_PRESENT);
	else
		return VO_RET_SOURCE2_FORMATUNSUPPORT;
}
#ifdef _new_programinfo
VO_U32 voAdaptiveStreamParserWrapper::SelectTrack ( VO_U32 uTrack, VO_SOURCE2_TRACK_TYPE sTrackType)
{
	voCAutoLock lock( &m_lock );
	if (!m_api.SelectTrack)
		return VO_RET_SOURCE2_FORMATUNSUPPORT;
		
	VOLOGR(" SelectTrack.ID: %d, Type: %d", uTrack, sTrackType );
	return m_api.SelectTrack( m_parserhandle , uTrack, sTrackType );
}
#else
VO_U32 voAdaptiveStreamParserWrapper::SelectTrack ( VO_U32 uTrack )
{
	voCAutoLock lock( &m_lock );

	return m_api.SelectTrack( m_parserhandle , uTrack );
}
#endif

VO_U32 voAdaptiveStreamParserWrapper::GetDRMInfo ( VO_SOURCE2_DRM_INFO **ppDRMInfo )
{
	voCAutoLock lock( &m_lock );

	if (m_api.GetDRMInfo)
		return m_api.GetDRMInfo( m_parserhandle , ppDRMInfo );
	else
		return VO_RET_SOURCE2_FORMATUNSUPPORT;
}

VO_U32 voAdaptiveStreamParserWrapper::GetParam ( VO_U32 nParamID, VO_PTR pParam )
{
	voCAutoLock lock( &m_lock );

	if (m_api.GetParam)
		return m_api.GetParam( m_parserhandle , nParamID , pParam );
	else
		return VO_RET_SOURCE2_FORMATUNSUPPORT;
}

VO_U32 voAdaptiveStreamParserWrapper::SetParam ( VO_U32 nParamID, VO_PTR pParam )
{
	voCAutoLock lock( &m_lock );

	if (m_api.SetParam)
		return m_api.SetParam( m_parserhandle , nParamID , pParam );
	else
		return VO_RET_SOURCE2_FORMATUNSUPPORT;
}
