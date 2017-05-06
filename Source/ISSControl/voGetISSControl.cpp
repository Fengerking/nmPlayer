#include "voSource2.h"
#include "CISSControl.h"
//#include "voGetISSControl.h"
#if defined __cplusplus
extern "C" {
#endif
VO_U32 Init( VO_HANDLE * phHandle, VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_CALLBACK * pCallback )
{
	CISSControl * ptr_obj = new CISSControl;
	*phHandle = ptr_obj;
	return ptr_obj->Init( pSource , nFlag , pCallback );
}

VO_U32 Uninit( VO_HANDLE hHandle )
{
	CISSControl * ptr_obj = ( CISSControl * )hHandle;
	return ptr_obj->Uninit();
}

VO_U32 Open( VO_HANDLE hHandle )
{
	CISSControl * ptr_obj = ( CISSControl * )hHandle;
	return ptr_obj->Open();
}

VO_U32 Close( VO_HANDLE hHandle )
{
	CISSControl * ptr_obj = ( CISSControl * )hHandle;
	return ptr_obj->Close();
}

VO_U32 Start( VO_HANDLE hHandle )
{
	CISSControl * ptr_obj = ( CISSControl * )hHandle;
	return ptr_obj->Start();
}

VO_U32 Pause( VO_HANDLE hHandle )
{
	CISSControl * ptr_obj = ( CISSControl * )hHandle;
	return ptr_obj->Pause();
}

VO_U32 Stop( VO_HANDLE hHandle )
{
	CISSControl * ptr_obj = ( CISSControl * )hHandle;
	return ptr_obj->Stop();
}

VO_U32 Seek( VO_HANDLE hHandle, VO_U64* pTimeStamp )
{
	CISSControl * ptr_obj = ( CISSControl * )hHandle;
	return ptr_obj->Seek( pTimeStamp );
}

VO_U32 GetDuration( VO_PTR hHandle, VO_U64 * pDuration )
{
	CISSControl * ptr_obj = ( CISSControl * )hHandle;
	return ptr_obj->GetDuration( pDuration );
}

VO_U32 GetSample( VO_HANDLE hHandle , VO_U32 nOutPutType , VO_PTR pSample )
{
	CISSControl * ptr_obj = ( CISSControl * )hHandle;
	return ptr_obj->GetSample( nOutPutType , pSample );
}

VO_U32 GetStreamCount( VO_HANDLE hHandle, VO_U32 *pStreamCount )
{
	CISSControl * ptr_obj = ( CISSControl * )hHandle;
	return ptr_obj->GetStreamCount( pStreamCount );
}

VO_U32 GetStreamInfo( VO_HANDLE hHandle, VO_U32 nStream, VO_SOURCE2_STREAM_INFO **ppStreamInfo )
{
	CISSControl * ptr_obj = ( CISSControl * )hHandle;
	return ptr_obj->GetStreamInfo( nStream , ppStreamInfo );
}

VO_U32 GetCurSelTrackInfo( VO_HANDLE hHandle, VO_U32 nOutPutType , VO_SOURCE2_TRACK_INFO * pTrackInfo )
{
	CISSControl * ptr_obj = ( CISSControl * )hHandle;
	return ptr_obj->GetCurSelTrackInfo( nOutPutType , pTrackInfo );
}

VO_U32 SelectTrack( VO_HANDLE hHandle, VO_U32 nStreamID , VO_U32 nSubStreamID , VO_U32 nTrackID )
{
	CISSControl * ptr_obj = ( CISSControl * )hHandle;
	return ptr_obj->SelectTrack( nStreamID , nSubStreamID , nTrackID );
}

VO_U32 GetDRMInfo( VO_HANDLE hHandle, VO_SOURCE2_DRM_INFO **ppDRMInfo )
{
	CISSControl * ptr_obj = ( CISSControl * )hHandle;
	return ptr_obj->GetDRMInfo( ppDRMInfo );
}

VO_U32 GetParam( VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam )
{
	CISSControl * ptr_obj = ( CISSControl * )hHandle;
	return ptr_obj->GetParam( nParamID , pParam );
}

VO_U32 SetParam( VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam )
{
	CISSControl * ptr_obj = ( CISSControl * )hHandle;
	return ptr_obj->SetParam( nParamID , pParam );
}


VO_U32 voGetISSControlAPI( VO_SOURCE2_API * pAPI )
{
	if( !pAPI )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	pAPI->Init = Init;
	pAPI->Uninit = Uninit;
	pAPI->Open = Open;
	pAPI->Close = Close;
	pAPI->Start = Start;
	pAPI->Pause = Pause;
	pAPI->Stop = Stop;
	pAPI->Seek = Seek;
	pAPI->GetDuration = GetDuration;
	pAPI->GetSample = GetSample;
	pAPI->GetStreamCount = GetStreamCount;
	pAPI->GetStreamInfo = GetStreamInfo;
	pAPI->GetCurSelTrackInfo = GetCurSelTrackInfo;
	pAPI->SelectTrack = SelectTrack;
	pAPI->GetDRMInfo = GetDRMInfo;
	pAPI->GetParam = GetParam;
	pAPI->SetParam = SetParam;

	return VO_RET_SOURCE2_OK;
}

#if defined __cplusplus
}
#endif

