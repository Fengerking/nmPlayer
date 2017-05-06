// AdsManager.cpp : Defines the exported functions for the DLL application.
//

#include "voAdsManager.h"
#include "vo_ads_manager.h"
#include "voLog.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

VO_U32 VO_API Init( VO_HANDLE * phHandle , VO_PTR pSource , VO_U32 nSize , VO_U32 nFlag , VO_ADSMANAGER_INITPARAM * ptr_param )
{
	if (NULL != ptr_param)
	{
		VOLOGINIT(ptr_param->ptr_workingpath);
	}
	VO_ADSMANAGER_GENERALINFO *ptr_info = NULL;

	VO_U32 ret = vo_ads_init( &ptr_info , pSource , nSize , nFlag , ptr_param );

	if( ret != VO_RET_SOURCE2_OK )
		return ret;

	*phHandle = ptr_info;

	return VO_RET_SOURCE2_OK;
}

VO_U32 VO_API Uninit( VO_HANDLE hHandle )
{
	VO_ADSMANAGER_GENERALINFO * ptr_info = ( VO_ADSMANAGER_GENERALINFO * )hHandle;

	VO_U32 ret = vo_ads_uninit( ptr_info );
	VOLOGUNINIT();
	return ret;
}

VO_U32 VO_API Open( VO_HANDLE hHandle )
{
	VO_ADSMANAGER_GENERALINFO * ptr_info = ( VO_ADSMANAGER_GENERALINFO * )hHandle;

	return vo_ads_open( ptr_info );
}

VO_U32 VO_API Close( VO_HANDLE hHandle )
{
	VO_ADSMANAGER_GENERALINFO * ptr_info = ( VO_ADSMANAGER_GENERALINFO * )hHandle;
	return vo_ads_close( ptr_info );
}

VO_U32 VO_API GetPlaybackInfo( VO_HANDLE hHandle , VO_ADSMANAGER_PLAYBACKINFO ** ppInfo )
{
	VO_ADSMANAGER_GENERALINFO * ptr_info = ( VO_ADSMANAGER_GENERALINFO * )hHandle;
	return vo_ads_get_playback_info( ptr_info , ppInfo );
}

VO_U32 VO_API ConvertTimeStamp( VO_HANDLE hHandle , VO_U64 * pOrgTime , VO_U64 ullPlayingTime )
{
	VO_ADSMANAGER_GENERALINFO * ptr_info = ( VO_ADSMANAGER_GENERALINFO * )hHandle;
	return vo_ads_convert_timestamp( ptr_info , pOrgTime , ullPlayingTime );
}

VO_U32 VO_API SetPlayingTime( VO_HANDLE hHandle , VO_U64 ullPlayingTime )
{
	VO_ADSMANAGER_GENERALINFO * ptr_info = ( VO_ADSMANAGER_GENERALINFO * )hHandle;
	return vo_ads_set_playingtime( ptr_info , ullPlayingTime );
}

VO_U32 VO_API SetAction( VO_HANDLE hHandle , VO_ADSMANAGER_ACTION nAction , VO_U64 ullPlayingTime )
{
	VO_ADSMANAGER_GENERALINFO * ptr_info = ( VO_ADSMANAGER_GENERALINFO * )hHandle;
	return vo_ads_set_action( ptr_info , nAction , ullPlayingTime );
}

VO_U32 VO_API SetActionSync( VO_HANDLE hHandle , VO_ADSMANAGER_ACTION nAction , VO_U64 ullPlayingTime , VO_VOID * pParam )
{
	VO_ADSMANAGER_GENERALINFO * ptr_info = ( VO_ADSMANAGER_GENERALINFO * )hHandle;
	return vo_ads_set_action_sync( ptr_info , nAction , ullPlayingTime , pParam );
}

VO_U32 VO_API GetContentDuration( VO_HANDLE hHandle , VO_U64 * pullDuration )
{
	VO_ADSMANAGER_GENERALINFO * ptr_info = ( VO_ADSMANAGER_GENERALINFO * )hHandle;
	return vo_ads_get_contentduration( ptr_info , pullDuration );
}

VO_BOOL VO_API IsSeekable( VO_HANDLE hHandle , VO_U64 ullPlayingTime )
{
	VO_ADSMANAGER_GENERALINFO * ptr_info = ( VO_ADSMANAGER_GENERALINFO * )hHandle;
	return vo_ads_is_seekable( ptr_info , ullPlayingTime );
}

VO_U32 VO_API Seek( VO_HANDLE hHandle , VO_U64 ullPlayingtTime , VO_U64 ullSeekPos , VO_ADSMANAGER_SEEKINFO * pInfo )
{
	VO_ADSMANAGER_GENERALINFO * ptr_info = ( VO_ADSMANAGER_GENERALINFO * )hHandle;
	return vo_ads_seek( ptr_info , ullPlayingtTime , ullSeekPos , pInfo );
}

VO_U32 VO_API GetParam(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
	VO_ADSMANAGER_GENERALINFO * ptr_info = ( VO_ADSMANAGER_GENERALINFO * )hHandle;
	return vo_ads_get_param( ptr_info , nParamID , pParam );
}

VO_U32 VO_API SetParam(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
	VO_ADSMANAGER_GENERALINFO * ptr_info = ( VO_ADSMANAGER_GENERALINFO * )hHandle;
	return vo_ads_set_param( ptr_info , nParamID , pParam );
}

VO_S32 VO_API voGetAdsManagerAPI(VO_ADSMANAGER_API * pAPI)
{
	pAPI->Init = Init;
	pAPI->Uninit = Uninit;
	pAPI->Open  = Open;
	pAPI->Close = Close;
	pAPI->GetPlaybackInfo = GetPlaybackInfo;
	pAPI->ConvertTimeStamp = ConvertTimeStamp;
	pAPI->SetPlayingTime = SetPlayingTime;
	pAPI->SetAction = SetAction;
	pAPI->SetActionSync = SetActionSync;
	pAPI->IsSeekable = IsSeekable;
	pAPI->GetContentDuration = GetContentDuration;
	pAPI->Seek = Seek;
	pAPI->GetParam = GetParam;
	pAPI->SetParam = SetParam;

	return VO_RET_SOURCE2_OK;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */