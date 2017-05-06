
//for parse download url and cc we have following steps:
//1. Use PID to combine a smil request URL
//2. Parse smil
//2.1 Download smil
//2.2 Send smil to smil parser and get the parsed result
//2.3 Re org the parsed result and get nessccary info
//3. Combine VMAP URL
//4. Parse VMAP
//4.1 Download VMAP
//4.2 Send VMAP to VMAP parser and get the parsed result
//5. Fill all the info and return

#ifndef __VO_ADS_MANAGER_H__

#define __VO_ADS_MANAGER_H__

#include "voAdsManager.h"
#include "vo_thread.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#define DOMAINGUID_PHONE "c9cd5ffb-e591-4566-8141-d79c05703b80"
#define DOMAINGUID_TABLET "c9cd5ffb-e591-4566-8141-d79c05703b80"
#define APPLICATIONGUID_PHONE "ce4713f9-c3bc-4cc2-bd06-4dfc74ce97f3"
#define APPLICATIONGUID_TABLET "a43b4194-1ef3-43bc-82ac-b132951060e0"
#define SECURE_DOMAINGUID_PHONE "bb0b18ba-64f5-4b1b-a29f-0ac252f06b68"
#define SECURE_DOMAINGUID_TABLET "bb0b18ba-64f5-4b1b-a29f-0ac252f06b68"
#define SECURE_APPLICATIONGUID_PHONE "fd8345f4-b034-4192-a30a-712542d3ef99"
#define SECURE_APPLICATIONGUID_TABLET "5bd36573-84c7-407c-936b-b6065842cbe6"
#define DEBUG_APPLICATIONGUID "de3616e9-6af2-43d5-beb5-156579d993a5"
#define DEBUG_SECURE_APPLICATIONGUID "06082b31-3e1a-4739-b8b5-b6cd640a1f92"

typedef enum
{
	VO_ADSMANAGER_STATUS_RUNNING,
	VO_ADSMANAGER_STATUS_SEEKADS,
	VO_ADSMANAGER_STATUS_PLAYING_SEEKEDADS,
	VO_ADSMANAGER_STATUS_DROP,
}VO_ADSMANAGER_STATUS;

typedef struct VO_ADSMANAGER_URLLIST
{
	VO_CHAR url[MAXURLLEN];
	VO_ADSMANAGER_URLLIST * ptr_next;
}VO_ADSMANAGER_URLLIST;

typedef struct VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL
{
	VO_U32 uID;
	VO_U32 uSequecnNumber;

	VO_ADSMANAGER_PERIODTYPE nPeriodType;	//the period type

	VO_CHAR strPeriodURL[MAXURLLEN];		//the period URL
	VO_CHAR strCaptionURL[MAXURLLEN];		//the caption/subtitle URL

	VO_U64 ullStartTime;					//the period start time
	VO_U64 ullEndTime;						//the period end time

	VO_U64 ullIdealStartTime;
	VO_U64 ullIdealEndTime;

	VO_U64 ullOrgStartTime;
	VO_U64 ullOrgEndTime;

	VO_CHAR strGUID[64];

	VO_CHAR strTitle[MAXURLLEN];
	VO_BOOL isLive;
	VO_BOOL isEpisode;
	VO_CHAR strContentID[64];

	VO_BOOL isPlayed;

	VO_CHAR strClickURL[MAXURLLEN];

	VO_ADSMANAGER_URLLIST * ptr_trackingevent_ad_start;
	VO_ADSMANAGER_URLLIST * ptr_trackingevent_ad_first_quartile;
	VO_ADSMANAGER_URLLIST * ptr_trackingevent_ad_mid_point;
	VO_ADSMANAGER_URLLIST * ptr_trackingevent_ad_third_quartile;
	VO_ADSMANAGER_URLLIST * ptr_trackingevent_ad_complete;
	VO_ADSMANAGER_URLLIST * ptr_trackingevent_ad_creative_view;
	VO_ADSMANAGER_URLLIST * ptr_trackingevent_ad_mute;
	VO_ADSMANAGER_URLLIST * ptr_trackingevent_ad_unmute;
	VO_ADSMANAGER_URLLIST * ptr_trackingevent_ad_pause;
	VO_ADSMANAGER_URLLIST * ptr_trackingevent_ad_resume;
	VO_ADSMANAGER_URLLIST * ptr_trackingevent_ad_fullscreen;
	VO_ADSMANAGER_URLLIST * ptr_trackingevent_ad_impression;

	VO_U64 ullPeriodStartTsTimeStamp;

	VO_U32 uLastPercentage;

	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_pre;
	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_next;

}VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL;

typedef struct  
{
	VO_U32 flag;
	VO_CHAR url[MAXURLLEN];
	VO_CHAR str_partner[MAXURLLEN];
	VO_ADSMANAGER_PLAYBACKINFO * pInfo;

	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_internal_begin;
	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_internal_end;

	VO_TCHAR * ptr_workingpath;

	void * ptr_msg_thread;

	THREAD_HANDLE open_thread_handle;

	VO_BOOL is_stop;

	VO_ADSMANAGER_EVENTCALLBACK eventcallback;

	VO_U64 last_playingtime;
	VO_ADSMANAGER_PERIODTYPE last_periodtype;
	VO_U32 last_periodid;

	VO_U64 org_duration;
	VO_U64 ping_time;
	VO_U64 last_org_time;

	VO_BOOL is_seek_ads;
	VO_U64 seek_ads_starttime;

	VO_ADSMANAGER_STATUS status;

	VO_BOOL is_wholecontent_startsent;
	VO_BOOL is_wholecontent_endsent;

	VO_U32 last_period_content_id;

	VO_ADSMANAGER_DEVICETYPE device_type;

#ifdef _VOLOG_INFO
	FILE * log_fp;
#endif

}VO_ADSMANAGER_GENERALINFO;

VO_U32 vo_ads_init( VO_ADSMANAGER_GENERALINFO ** pptr_info , VO_PTR ptr_source , VO_U32 size , VO_U32 flag , VO_ADSMANAGER_INITPARAM * ptr_param );
VO_U32 vo_ads_uninit( VO_ADSMANAGER_GENERALINFO * ptr_info );
VO_U32 vo_ads_open( VO_ADSMANAGER_GENERALINFO * ptr_info );
VO_U32 vo_ads_close( VO_ADSMANAGER_GENERALINFO * ptr_info );
VO_U32 vo_ads_get_playback_info( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_ADSMANAGER_PLAYBACKINFO ** pptr_info );
VO_U32 vo_ads_convert_timestamp( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U64 * pOrgTime , VO_U64 ullPlayingTime );
VO_U32 vo_ads_set_playingtime( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U64 playingtime );
VO_U32 vo_ads_set_action( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_ADSMANAGER_ACTION action , VO_U64 ullPlayingTime );
VO_U32 vo_ads_set_action_sync( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_ADSMANAGER_ACTION action , VO_U64 ullPlayingTime , VO_VOID * pParam );
VO_U32 vo_ads_get_contentduration( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U64 * pullDuration );
VO_BOOL vo_ads_is_seekable( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U64 ullPlayingTime );
VO_U32 vo_ads_seek( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U64 ullPlayingtTime , VO_U64 ullSeekPos , VO_ADSMANAGER_SEEKINFO * pInfo );
VO_U32 vo_ads_set_param( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U32 id , VO_PTR param );
VO_U32 vo_ads_get_param( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U32 id , VO_PTR param );

#ifdef _VONAMESPACE
}
#endif


#endif