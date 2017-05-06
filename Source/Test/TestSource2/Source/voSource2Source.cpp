#include "voSource2Source.h"

voSource2Source::voSource2Source( const VO_TCHAR * str_modulename , const VO_TCHAR * str_apiname )
{
	vostrcpy( m_loader.m_szDllFile , str_modulename );
	vostrcpy( m_loader.m_szAPIName , str_apiname );
}

voSource2Source::~voSource2Source(void)
{
}

VO_U32 voSource2Source::Init ( VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pInitParam )
{
	return m_api.Init( &m_handle , pSource , nFlag , pInitParam );
}

VO_U32 voSource2Source::Uninit ()
{
	return m_api.Uninit( m_handle );
}

VO_U32 voSource2Source::Open ()
{
	return m_api.Open( m_handle );
}

VO_U32 voSource2Source::Close ()
{
	return m_api.Close( m_handle );
}

VO_U32 voSource2Source::Start ()
{
	return m_api.Start( m_handle );
}

VO_U32 voSource2Source::Pause ()
{
	return m_api.Pause( m_handle );
}

VO_U32 voSource2Source::Stop ()
{
	return m_api.Stop( m_handle );
}

VO_U32 voSource2Source::Seek ( VO_U64* pTimeStamp)
{
	return m_api.Seek( m_handle , pTimeStamp );
}

VO_U32 voSource2Source::GetDuration ( VO_U64 * pDuration)
{
	return m_api.GetDuration( m_handle , pDuration );
}

VO_U32 voSource2Source::GetSample ( VO_SOURCE2_TRACK_TYPE nTrackType , VO_PTR pSample )
{
	return m_api.GetSample( m_handle , nTrackType , pSample );
}

VO_U32 voSource2Source::GetProgramCount ( VO_U32 *pProgramCount)
{
	return m_api.GetProgramCount( m_handle , pProgramCount );
}

VO_U32 voSource2Source::GetProgramInfo ( VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **ppProgramInfo)
{
	return m_api.GetProgramInfo( m_handle , nProgram , ppProgramInfo );
}

VO_U32 voSource2Source::GetCurTrackInfo ( VO_SOURCE2_TRACK_TYPE nTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo )
{
	return m_api.GetCurTrackInfo( m_handle , nTrackType , ppTrackInfo );
}

VO_U32 voSource2Source::SelectProgram ( VO_U32 nProgram)
{
	return m_api.SelectProgram( m_handle , nProgram );
}

VO_U32 voSource2Source::SelectStream ( VO_U32 nStream)
{
	return m_api.SelectStream( m_handle , nStream );
}

VO_U32 voSource2Source::SelectTrack ( VO_U32 nTrack)
{
	return m_api.SelectTrack( m_handle , nTrack );
}

VO_U32 voSource2Source::GetDRMInfo ( VO_SOURCE2_DRM_INFO **ppDRMInfo)
{
	return m_api.GetDRMInfo( m_handle , ppDRMInfo );
}

VO_U32 voSource2Source::SendBuffer ( const VO_SOURCE2_SAMPLE& buffer )
{
	return m_api.SendBuffer( m_handle , buffer );
}

VO_U32 voSource2Source::GetParam ( VO_U32 nParamID, VO_PTR pParam)
{
	return m_api.GetParam( m_handle , nParamID , pParam );
}

VO_U32 voSource2Source::SetParam ( VO_U32 nParamID, VO_PTR pParam)
{
	return m_api.SetParam( m_handle , nParamID , pParam );
}

