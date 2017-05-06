// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "voAdsManager.h"
#include "stdlib.h"
#include "time.h"
#include "stdio.h"

typedef VO_S32 ( VO_API * pvoGetAdsManagerAPI)(VO_ADSMANAGER_API * pAPI);

void test( VO_ADSMANAGER_API * pAPI );

VO_S32 SendEvent(VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);

VO_U64 g_playingtime = 0;

int _tmain(int argc, _TCHAR* argv[])
{
	HMODULE h = LoadLibrary( _T("voAdsManager.dll") );

	pvoGetAdsManagerAPI pAPI = ( pvoGetAdsManagerAPI )GetProcAddress( h , "voGetAdsManagerAPI" );
	
	VO_ADSMANAGER_API api;
	memset( &api , 0 , sizeof( VO_ADSMANAGER_API ) );
	pAPI( &api );

	test( &api );

	FreeLibrary( h );

	return 0;
}

void test( VO_ADSMANAGER_API * pAPI )
{
	VO_HANDLE h = 0;

	VO_ADSMANAGER_INITPARAM param;
	param.ptr_workingpath = NULL;

	VO_ADSMANAGER_SOURCE_PID pid;
	pid.pPartnerString = 0;
	pid.pPID = "Ig46PiEOsrJD";
	//pid.pPID = "k_u42nbOWgZu";
	//pid.pPID = "gr_LWCCvh0kr";
	//pid.pPID = "WLkhc1EVWZvB";

	//pid.pPID = "TTjAcZ66PPl3";
	//pid.pPartnerString = "cbs_android_app";

	pAPI->Init( &h , &pid , 12 , VO_ADSMANAGER_OPENFLAG_PID , &param );

	VO_ADSMANAGER_EVENTCALLBACK cb;
	cb.pUserData = 0;
	cb.SendEvent = SendEvent;
	pAPI->SetParam( h , VO_ADSMANAGER_PID_EVENTCALLBACK , &cb );

	pAPI->Open( h );

	VO_ADSMANAGER_PLAYBACKINFO * ptr_info = 0;
	pAPI->GetPlaybackInfo( h , &ptr_info );

	for( VO_U32 i = 0 ; i < ptr_info->nCounts ; i++ )
	{
		printf( "%s\t%lld\t%lld\n" , ptr_info->pPeriods[i].nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT ? "Content" : "ADs    " ,  ptr_info->pPeriods[i].ullStartTime , ptr_info->pPeriods[i].ullEndTime );
	}

	g_playingtime = 0;

	
	pAPI->SetPlayingTime( h , 0 );

	pAPI->SetPlayingTime( h , 30000 );

	pAPI->SetPlayingTime( h , 40000 );

	pAPI->SetPlayingTime( h , 400000 );

	pAPI->SetPlayingTime( h , 30000 );

	pAPI->SetAction( h , VO_ADSMANAGER_ACTION_PLAYBACKCOMPLETE );

	getchar();
}

VO_S32 SendEvent(VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
	if( nID == VO_ADSMANAGER_EVENT_CONTENT_START )
		printf( "VO_ADSMANAGER_EVENT_CONTENT_START\t %lld\n" , g_playingtime );
	else if( nID == VO_ADSMANAGER_EVENT_CONTENT_END )
		printf( "VO_ADSMANAGER_EVENT_CONTENT_END\t %lld\n" , g_playingtime );
	else if( nID == VO_ADSMANAGER_EVENT_AD_START )
		printf( "VO_ADSMANAGER_EVENT_AD_START\t %lld\n" , g_playingtime );
	else if( nID == VO_ADSMANAGER_EVENT_AD_END )
		printf( "VO_ADSMANAGER_EVENT_AD_END\t %lld\n" , g_playingtime );

	return 0;
}

