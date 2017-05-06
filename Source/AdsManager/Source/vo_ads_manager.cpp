
#include "vo_ads_manager.h"
#include "vo_smil.h"
#include "vo_ads_utility.h"
#include "vo_vmap.h"
#include "voLog.h"
#include "vo_message_thread.h"
#include "voHMAC_SHA1.h"
#include "voOSFunc.h"
#include "base64.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#define INVALIDTIMESTAMP 0xffffffffffffffffll

//step 1
VO_U32 generate_smil_url( VO_CHAR * ptr_id , VO_CHAR * ptr_url , VO_U32 size );
VO_VOID create_playbackinfo( VO_ADSMANAGER_GENERALINFO * ptr_dst , VO_ADSMANAGER_PLAYBACKINFOEX * ptr_src );
VO_U32 convert_periodurl( VO_ADSMANAGER_GENERALINFO * ptr_info );
VO_VOID insert_playbackperiod_internallist( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item );
VO_VOID destroy_playbackperiod_internallist( VO_ADSMANAGER_GENERALINFO * ptr_info );
VO_VOID destory_periodinfo( VO_ADSMANAGER_GENERALINFO * ptr_info );
VO_VOID create_periodinfo( VO_ADSMANAGER_GENERALINFO * ptr_info );
VO_BOOL merge_ads_into_periodinfo( VO_ADSMANAGER_GENERALINFO * ptr_info , VOAdInfo * ptr_adsinfo );
VO_VOID insert_playbackperiod_internallist( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_insertpos , VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item );
VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * divid_playbackperiod_internallistitem( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_dividitem , VO_U64 divide_time );
VO_VOID recalculate_periodinfo_timestamp( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_start_recalculate_item , VO_U64 offset );
unsigned int message_func( void * ptr_userdata , unsigned int msg_id , void * ptr_param1 , void * ptr_param2 );
VO_VOID detect_period_percentage( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item , VO_U64 timestamp );
VO_VOID detect_wholecontent_percentage( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item , VO_U64 timestamp );
VO_VOID detect_period_timepass( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item , VO_U64 timestamp );
VO_VOID append_verification( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_CHAR * ptr_url );
VO_BOOL urlencode( VO_CHAR * ptr_url );
unsigned char toHex(const unsigned char x);
VO_BOOL isxnumalpha( char x );
VO_BOOL isxhex( char x );
VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * get_periodbyid( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U32 id );
VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * get_periodbyplayingtime( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U64 playingtime );
VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * get_periodbyorgtime( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U64 orgtime );
VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * get_periodbyidealtime( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U64 idealtime );
VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * get_periodbyidealtime_fuzzy( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U64 idealtime );
VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * get_periodbyperiodid( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U32 period_id );

VO_U32 open_internal( VO_ADSMANAGER_GENERALINFO * ptr_info  );
unsigned int begin_open_thread( VO_ADSMANAGER_GENERALINFO * ptr_info );
unsigned int end_open_thread( VO_ADSMANAGER_GENERALINFO * ptr_info );
unsigned int open_threadfunc( void * ptr_obj );

VO_VOID merge_internallist( VO_ADSMANAGER_GENERALINFO * ptr_info );

VO_VOID post_trackingevents( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U32 eventid , VO_U32 periodid , VO_ADSMANAGER_URLLIST * ptr_list , VO_U64 playingtime );
VO_VOID destroy_urllist( VO_ADSMANAGER_URLLIST * ptr_list );

VO_BOOL is_nocontent_before( VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item );
VO_BOOL is_nocontent_behind( VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item );


VO_U32 vo_ads_init( VO_ADSMANAGER_GENERALINFO ** pptr_info , VO_PTR ptr_source , VO_U32 size , VO_U32 flag , VO_ADSMANAGER_INITPARAM * ptr_param )
{
	if( !( flag & VO_ADSMANAGER_OPENFLAG_PID ) )
		return VO_RET_SOURCE2_OPENFAIL;

	VO_ADSMANAGER_GENERALINFO * ptr_info = new VO_ADSMANAGER_GENERALINFO;
	memset( ptr_info , 0 , sizeof( VO_ADSMANAGER_GENERALINFO ) );

	ptr_info->is_stop = VO_FALSE;
	ptr_info->is_seek_ads = VO_FALSE;

	ptr_info->last_playingtime = INVALIDTIMESTAMP;
	ptr_info->last_org_time = INVALIDTIMESTAMP;

	ptr_info->is_wholecontent_endsent = VO_FALSE;
	ptr_info->is_wholecontent_startsent = VO_FALSE;

	ptr_info->status = VO_ADSMANAGER_STATUS_RUNNING;

	ptr_info->flag = flag;

	ptr_info->device_type = ptr_param->device_type;

	VO_ADSMANAGER_SOURCE_PID * ptr_source_pid = ( VO_ADSMANAGER_SOURCE_PID * )ptr_source;

	size = strlen( ptr_source_pid->pPID );
	VO_CHAR * ptr_pid = new VO_CHAR[size + 2];
	memset( ptr_pid , 0 , size + 2 );
	memcpy( ptr_pid , ptr_source_pid->pPID , size );

	generate_smil_url( ptr_pid , ptr_info->url , sizeof( ptr_info->url ) );

	delete []ptr_pid;

	if( ptr_source_pid->pPartnerString )
	{
		size = strlen( ptr_source_pid->pPartnerString );

		if( size > MAXURLLEN - 1 )
		{
			VOLOGE( "PartnerString too big!!! %s" , ptr_source_pid->pPartnerString );
			return VO_RET_SOURCE2_FAIL;
		}

		strcpy( ptr_info->str_partner , ptr_source_pid->pPartnerString );
	}

	ptr_info->ptr_workingpath = ptr_param->ptr_workingpath;

	*pptr_info = ptr_info;

	return VO_RET_SOURCE2_OK;
}

VO_U32 vo_ads_uninit( VO_ADSMANAGER_GENERALINFO * ptr_info )
{
	destroy_playbackperiod_internallist( ptr_info );

	destory_periodinfo( ptr_info );

	delete ptr_info;

	return VO_RET_SOURCE2_OK;
}

VO_U32 vo_ads_open( VO_ADSMANAGER_GENERALINFO * ptr_info  )
{
	if( ptr_info->flag & VO_ADSMANAGER_OPENFLAG_ASYNCOPEN )
	{
		begin_open_thread( ptr_info );
	}
	else
		return open_internal( ptr_info );
}

VO_U32 vo_ads_close( VO_ADSMANAGER_GENERALINFO * ptr_info )
{
	VOLOGI( "+vo_ads_close" );
#ifdef _VOLOG_INFO
	if( ptr_info->log_fp )
	{
		fclose( ptr_info->log_fp );
		ptr_info->log_fp = 0;
	}
#endif

	ptr_info->is_stop = VO_TRUE;

	VOLOGI( "end_open_thread" );
	end_open_thread( ptr_info );
	VOLOGI( "end_message_thread" );
	end_message_thread( ptr_info->ptr_msg_thread );
	ptr_info->ptr_msg_thread = 0;

	destroy_playbackperiod_internallist( ptr_info );

	VOLOGI( "destory_periodinfo" );
	destory_periodinfo( ptr_info );

	VOLOGI( "-vo_ads_close" );
	return VO_RET_SOURCE2_OK;
}

VO_U32 vo_ads_get_playback_info( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_ADSMANAGER_PLAYBACKINFO ** pptr_info )
{
	destory_periodinfo( ptr_info );

	create_periodinfo( ptr_info );

	*pptr_info = ptr_info->pInfo;

	for( VO_U32 i = 0 ; i < ptr_info->pInfo->nCounts ; i++ )
	{
		VOLOGI( "PeriodInfo: %s\t%lld\t%lld\n" , ptr_info->pInfo->pPeriods[i].nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT ? "Content" : "ADs    " ,  ptr_info->pInfo->pPeriods[i].ullStartTime , ptr_info->pInfo->pPeriods[i].ullEndTime );
	}

	return VO_RET_SOURCE2_OK;
}

VO_U32 vo_ads_convert_timestamp( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U64 * pOrgTime , VO_U64 ullPlayingTime )
{
	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = ptr_info->ptr_internal_begin;

	VO_U64 lastcontenttimestamp = 0;

	while( ptr_item )
	{
		if( ullPlayingTime >= ptr_item->ullStartTime && ullPlayingTime <= ptr_item->ullEndTime )
		{
			if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT )
			{
				*pOrgTime = ullPlayingTime - ( ptr_item->ullStartTime - ptr_item->ullOrgStartTime );
				//VOLOGI( "PlayingTime: %lld SubtitleTime %lld" , ullPlayingTime , *pOrgTime );
				return VO_RET_SOURCE2_OK;
			}
			else if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS )
			{
				*pOrgTime = lastcontenttimestamp;
				return VO_RET_SOURCE2_OK;
			}
		}

		if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT )
		{
			lastcontenttimestamp = ptr_item->ullEndTime;
		}

		ptr_item = ptr_item->ptr_next;
	}

	return VO_RET_SOURCE2_INVALIDPARAM;
}

VO_U32 vo_ads_set_playingtime( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U64 playingtime )
{
	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = ptr_info->ptr_internal_begin;

	static VO_U32 systime = voOS_GetSysTime();

	if( voOS_GetSysTime() - systime > 1000 )
	{
		VOLOGI( "PlayingTime: %lld" , playingtime );
		systime = voOS_GetSysTime();
	}

	if( ptr_info->is_seek_ads && 
		playingtime < ptr_info->seek_ads_starttime )
	{
		VOLOGI( "!!!!!!!Ignore this playing time" );
		return 0;
	}

	while( ptr_item )
	{
		if( playingtime >= ptr_item->ullStartTime && playingtime <= ptr_item->ullEndTime )
		{
			if( ptr_info->last_playingtime == INVALIDTIMESTAMP )
			{
				VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO info;
				memset( &info , 0 , sizeof( VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO ) );
				info.uPeriodID = ptr_item->uID;

				post_message( ptr_info->ptr_msg_thread , ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS ? VO_ADSMANAGER_EVENT_AD_START : VO_ADSMANAGER_EVENT_CONTENT_START , &info , (void *)playingtime );
				ptr_info->ping_time = playingtime;

				if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS )
				{
					post_trackingevents( ptr_info , VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_AD_START , ptr_item->uID , ptr_item->ptr_trackingevent_ad_start , playingtime );
					post_trackingevents( ptr_info , VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_CREATIVE_VIEW , ptr_item->uID , ptr_item->ptr_trackingevent_ad_creative_view , playingtime );
					post_trackingevents( ptr_info , VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_IMPRESSION , ptr_item->uID , ptr_item->ptr_trackingevent_ad_impression , playingtime );
				}
				else if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT )
				{
					if( (!ptr_info->is_wholecontent_startsent) && is_nocontent_before( ptr_item ) )
					{
						post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_WHOLECONTENT_START , &info , (void *)playingtime );
						ptr_info->is_wholecontent_startsent = VO_TRUE;
					}
				}

				VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_iter = ptr_info->ptr_internal_begin;
				while( ptr_iter )
				{
					VOLOGI( "PeriodInfo: %s\t%d\t%lld\t%lld\t Ideal %lld\t%lld\t%s  %s\n" , ptr_iter->nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT ? "Content" : "ADs    " , ptr_iter->uSequecnNumber , ptr_iter->ullStartTime , ptr_iter->ullEndTime , ptr_iter->ullIdealStartTime , ptr_iter->ullIdealEndTime , ptr_iter->isPlayed ? "True":"False" , ptr_iter == ptr_item ? "<-----" : "" );
					ptr_iter = ptr_iter->ptr_next;
				}
			}
			else if( ptr_info->last_playingtime < ptr_item->ullStartTime || ptr_info->last_playingtime > ptr_item->ullEndTime )
			{
				VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO info;
				memset( &info , 0 , sizeof( VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO ) );
				info.uPeriodID = ptr_info->last_periodid;
				info.ullElapsedTime = playingtime - ptr_info->ping_time;

				VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_period = get_periodbyid( ptr_info , ptr_info->last_periodid );
				//ptr_period->isPlayed = VO_TRUE;

				if( ptr_period->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS && ptr_period->uLastPercentage != 0xffffffff )
				{
					VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO perinfo;
					perinfo.uPercentage = 100;
					perinfo.uPeriodID = ptr_item->uID;
					perinfo.ullElapsedTime = playingtime - ptr_info->ping_time;
					post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_TRACKING_PERIODPRECENTAGE , &perinfo , (void *)playingtime );
					ptr_info->ping_time = playingtime;

					ptr_item->uLastPercentage = 0xffffffff;

					VOLOGI( "Add 100% event manualy!" );
				}

				if( ptr_period->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS && ptr_period->isPlayed )
				{
					;
				}
				else
				{
					post_message( ptr_info->ptr_msg_thread , ptr_info->last_periodtype == VO_ADSMANAGER_PERIODTYPE_ADS ? VO_ADSMANAGER_EVENT_AD_END : VO_ADSMANAGER_EVENT_CONTENT_END , &info , (void *)playingtime );
					ptr_info->ping_time = playingtime;

					ptr_period->isPlayed = VO_TRUE;

					info.ullElapsedTime = 0;

					if( ptr_info->last_periodtype == VO_ADSMANAGER_PERIODTYPE_ADS )
					{
						post_trackingevents( ptr_info , VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_COMPLETE , ptr_info->last_periodid , ptr_period->ptr_trackingevent_ad_complete , playingtime );
						ptr_info->is_seek_ads = VO_FALSE;
					}
					else if( ptr_info->last_periodtype == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT )
					{
						if( (!ptr_info->is_wholecontent_endsent) && is_nocontent_behind( ptr_period ) )
						{
							info.uPeriodID = ptr_info->last_period_content_id;
							post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_WHOLECONTENT_END , &info , (void *)playingtime );
							ptr_info->is_wholecontent_endsent = VO_TRUE;
						}
					}
				}

				info.uPeriodID = ptr_item->uID;
				info.ullElapsedTime = playingtime - ptr_info->ping_time;

				if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS && ptr_item->isPlayed )
				{
					;
				}
				else
				{
					if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS )
					{
						if( (!ptr_info->is_wholecontent_endsent) && is_nocontent_behind( ptr_item ) )
						{
							info.uPeriodID = ptr_info->last_period_content_id;
							post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_WHOLECONTENT_END , &info , (void *)playingtime );
							ptr_info->is_wholecontent_endsent = VO_TRUE;

							info.ullElapsedTime = 0;
						}
					}

					info.uPeriodID = ptr_item->uID;
					post_message( ptr_info->ptr_msg_thread , ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS ? VO_ADSMANAGER_EVENT_AD_START : VO_ADSMANAGER_EVENT_CONTENT_START , &info , (void *)playingtime );

					if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS )
					{
						post_trackingevents( ptr_info , VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_AD_START , ptr_item->uID , ptr_item->ptr_trackingevent_ad_start , playingtime );
						post_trackingevents( ptr_info , VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_CREATIVE_VIEW , ptr_item->uID , ptr_item->ptr_trackingevent_ad_creative_view , playingtime );
						post_trackingevents( ptr_info , VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_IMPRESSION , ptr_item->uID , ptr_item->ptr_trackingevent_ad_impression , playingtime );
					}
					else if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT )
					{
						if( (!ptr_info->is_wholecontent_startsent) && is_nocontent_before( ptr_item ) )
						{
							post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_WHOLECONTENT_START , &info , (void *)playingtime );
							ptr_info->is_wholecontent_startsent = VO_TRUE;
						}
					}

					ptr_info->ping_time = playingtime;
				}

				VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_iter = ptr_info->ptr_internal_begin;
				while( ptr_iter )
				{
					VOLOGI( "PeriodInfo: %s\t%d\t%lld\t%lld\t Ideal %lld\t%lld\t%s  %s\n" , ptr_iter->nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT ? "Content" : "ADs    " , ptr_iter->uSequecnNumber , ptr_iter->ullStartTime , ptr_iter->ullEndTime , ptr_iter->ullIdealStartTime , ptr_iter->ullIdealEndTime , ptr_iter->isPlayed ? "True":"False" , ptr_iter == ptr_item ? "<-----" : "" );
					ptr_iter = ptr_iter->ptr_next;
				}
			}
			else
			{
				if( ptr_info->last_periodtype != ptr_item->nPeriodType )
				{
					VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO info;
					memset( &info , 0 , sizeof( VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO ) );
					info.uPeriodID = ptr_info->last_periodid;
					info.ullElapsedTime = playingtime - ptr_info->ping_time;

					VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_period = get_periodbyid( ptr_info , ptr_info->last_periodid );

					if( ptr_period->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS && ptr_period->uLastPercentage != 0xffffffff )
					{
						VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO perinfo;
						perinfo.uPercentage = 100;
						perinfo.uPeriodID = ptr_item->uID;
						perinfo.ullElapsedTime = playingtime - ptr_info->ping_time;
						post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_TRACKING_PERIODPRECENTAGE , &perinfo , (void *)playingtime );
						ptr_info->ping_time = playingtime;

						ptr_item->uLastPercentage = 0xffffffff;

						VOLOGI( "Add 100% event manualy!" );
					}

					if( ptr_period->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS && ptr_period->isPlayed )
					{
						;
					}
					else
					{
						post_message( ptr_info->ptr_msg_thread , ptr_info->last_periodtype == VO_ADSMANAGER_PERIODTYPE_ADS ? VO_ADSMANAGER_EVENT_AD_END : VO_ADSMANAGER_EVENT_CONTENT_END , &info , (void *)playingtime );
						ptr_info->ping_time = playingtime;
						ptr_period->isPlayed = VO_TRUE;
						info.ullElapsedTime = 0;

						if( ptr_info->last_periodtype == VO_ADSMANAGER_PERIODTYPE_ADS )
						{
							post_trackingevents( ptr_info , VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_COMPLETE , ptr_info->last_periodid , ptr_period->ptr_trackingevent_ad_complete , playingtime );
						}
						else if( ptr_info->last_periodtype == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT )
						{
							if( (!ptr_info->is_wholecontent_endsent) && is_nocontent_behind( ptr_period ) )
							{
								info.uPeriodID = ptr_info->last_period_content_id;
								post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_WHOLECONTENT_END , &info , (void *)playingtime );
								ptr_info->is_wholecontent_endsent = VO_TRUE;
							}
						}
					}

					if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS )
					{
						if( (!ptr_info->is_wholecontent_endsent) && is_nocontent_behind( ptr_item ) )
						{
							info.uPeriodID = ptr_info->last_period_content_id;
							post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_WHOLECONTENT_END , &info , (void *)playingtime );
							ptr_info->is_wholecontent_endsent = VO_TRUE;
							info.ullElapsedTime = 0;
						}
					}

					info.uPeriodID = ptr_item->uID;
					info.ullElapsedTime = playingtime - ptr_info->ping_time;
					post_message( ptr_info->ptr_msg_thread , ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS ? VO_ADSMANAGER_EVENT_AD_START : VO_ADSMANAGER_EVENT_CONTENT_START , &info , (void *)playingtime );

					if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS )
					{
						post_trackingevents( ptr_info , VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_AD_START , ptr_item->uID , ptr_item->ptr_trackingevent_ad_start , playingtime );
						post_trackingevents( ptr_info , VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_CREATIVE_VIEW , ptr_item->uID , ptr_item->ptr_trackingevent_ad_creative_view , playingtime );
						post_trackingevents( ptr_info , VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_IMPRESSION , ptr_item->uID , ptr_item->ptr_trackingevent_ad_impression , playingtime );
					}
					else if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT )
					{
						if( (!ptr_info->is_wholecontent_startsent) && is_nocontent_before( ptr_item ) )
						{
							post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_WHOLECONTENT_START , &info , (void *)playingtime );
							ptr_info->is_wholecontent_startsent = VO_TRUE;
						}
					}

					ptr_info->ping_time = playingtime;
				}
			}

			detect_period_percentage( ptr_info , ptr_item , playingtime );
			detect_period_timepass( ptr_info , ptr_item , playingtime );

			if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT )
			{
				detect_wholecontent_percentage( ptr_info , ptr_item , playingtime );
			}

			ptr_info->last_periodtype = ptr_item->nPeriodType;
			ptr_info->last_periodid = ptr_item->uID;

			break;
		}

		ptr_item = ptr_item->ptr_next;
	}

	ptr_info->last_playingtime = playingtime;
	return VO_RET_SOURCE2_OK;
}

VO_U32 vo_ads_set_action( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_ADSMANAGER_ACTION action , VO_U64 ullPlayingTime )
{
	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = get_periodbyplayingtime( ptr_info , ullPlayingTime );

	if( !ptr_item )
		return VO_RET_SOURCE2_INVALIDPARAM;

	VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO actioninfo;
	actioninfo.nAction = action;
	actioninfo.uPeriodID = ptr_item->uID;
	actioninfo.ullElapsedTime = ullPlayingTime - ptr_info->ping_time;

	switch( action )
	{
	case VO_ADSMANAGER_ACTION_PLAYBACKCOMPLETE:
		{
			actioninfo.ullElapsedTime = ptr_info->last_playingtime - ptr_info->ping_time;

			VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO info;
			memset( &info , 0 , sizeof( VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO ) );
			info.uPeriodID = ptr_info->last_periodid;
			info.ullElapsedTime = ptr_info->last_playingtime - ptr_info->ping_time;

			post_message( ptr_info->ptr_msg_thread , ptr_info->last_periodtype == VO_ADSMANAGER_PERIODTYPE_ADS ? VO_ADSMANAGER_EVENT_AD_END : VO_ADSMANAGER_EVENT_CONTENT_END , &info , (void *)ullPlayingTime );
			ptr_info->ping_time = ptr_info->last_playingtime;
			info.ullElapsedTime = 0;

			VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_period = get_periodbyplayingtime( ptr_info , ptr_info->last_playingtime );
			if( ptr_info->last_periodtype == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT )
			{
				if( (!ptr_info->is_wholecontent_endsent) && is_nocontent_behind( ptr_period ) )
				{
					info.uPeriodID = ptr_info->last_period_content_id;
					post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_WHOLECONTENT_END , &info , (void *)ullPlayingTime );
					ptr_info->is_wholecontent_endsent = VO_TRUE;
				}
			}
		}
		break;
	case VO_ADSMANAGER_ACTION_SEEK:
		{
			ptr_info->ping_time = ullPlayingTime;
		}
		break;
	case VO_ADSMANAGER_ACTION_PAUSE:
		{
			ptr_info->ping_time = ullPlayingTime;
			VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_period = get_periodbyplayingtime( ptr_info , ullPlayingTime );
			if( ptr_period && ptr_period->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS )
				post_trackingevents( ptr_info , VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_PAUSE , ptr_period->uID , ptr_period->ptr_trackingevent_ad_pause , ullPlayingTime );
		}
		break;
	case VO_ADSMANAGER_ACTION_RESUME:
		{
			ptr_info->ping_time = ullPlayingTime;
			VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_period = get_periodbyplayingtime( ptr_info , ullPlayingTime );
			if( ptr_period && ptr_period->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS )
				post_trackingevents( ptr_info , VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_RESUME , ptr_period->uID , ptr_period->ptr_trackingevent_ad_resume , ullPlayingTime );
		}
		break;
	case VO_ADSMANAGER_ACTION_MUTEON:
		{
			VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_period = get_periodbyplayingtime( ptr_info , ullPlayingTime );
			if( ptr_period && ptr_period->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS )
				post_trackingevents( ptr_info , VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_MUTE , ptr_period->uID , ptr_period->ptr_trackingevent_ad_mute , ullPlayingTime );
		}
		break;
	case VO_ADSMANAGER_ACTION_MUTEOFF:
		{
			VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_period = get_periodbyplayingtime( ptr_info , ullPlayingTime );
			if( ptr_period && ptr_period->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS )
				post_trackingevents( ptr_info , VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_UNMUTE , ptr_period->uID , ptr_period->ptr_trackingevent_ad_unmute , ullPlayingTime );
		}
		break;
	case VO_ADSMANAGER_ACTION_FULLSCREENON:
		{
			VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_period = get_periodbyplayingtime( ptr_info , ullPlayingTime );
			if( ptr_period && ptr_period->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS )
				post_trackingevents( ptr_info , VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_FULLSCREEN , ptr_period->uID , ptr_period->ptr_trackingevent_ad_fullscreen , ullPlayingTime );
		}
		break;
	}

	post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_TRACKING_ACTION , &actioninfo , (void *)ullPlayingTime );

	return VO_RET_SOURCE2_OK;
}

VO_U32 vo_ads_set_action_sync( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_ADSMANAGER_ACTION action , VO_U64 ullPlayingTime , VO_VOID * pParam )
{
	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = get_periodbyplayingtime( ptr_info , ullPlayingTime );

	if( action == VO_ADSMANAGER_ACTION_CLICK )
	{
		if( !ptr_item )
			return VO_RET_SOURCE2_OK;

		strcpy( (VO_CHAR*)pParam , ptr_item->strClickURL );
		return VO_RET_SOURCE2_OK;
	}

	return VO_RET_SOURCE2_NOIMPLEMENT;
}

VO_U32 vo_ads_get_contentduration( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U64 * pullDuration )
{
	*pullDuration = ptr_info->org_duration;
	VOLOGI( "Content Duration: %lld" , ptr_info->org_duration );
	return VO_RET_SOURCE2_OK;
}

VO_BOOL vo_ads_is_seekable( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U64 ullPlayingTime )
{
	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = ptr_info->ptr_internal_begin;

	VO_BOOL ret = VO_FALSE;

	while( ptr_item )
	{
		if( ullPlayingTime >= ptr_item->ullStartTime && ullPlayingTime <= ptr_item->ullEndTime )
		{
			if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS )
				ret = VO_FALSE;
			else
				ret = VO_TRUE;
		}

		ptr_item = ptr_item->ptr_next;
	}

	VOLOGI( "Can Seek: %s" , ret ? "True" : "False" );

	return ret;
}

VO_U32 vo_ads_seek( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U64 ullPlayingtTime , VO_U64 ullSeekPos , VO_ADSMANAGER_SEEKINFO * pInfo )
{
	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_last_period = get_periodbyplayingtime( ptr_info , ullPlayingtTime );

	if( !ptr_last_period )
		return VO_RET_SOURCE2_INVALIDPARAM;

	if( ptr_last_period->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS )
		return VO_RET_SOURCE2_INVALIDPARAM;

	if( ullSeekPos >= ptr_last_period->ullOrgStartTime && ullSeekPos <= ptr_last_period->ullOrgEndTime )
	{
		pInfo->uAdsSequnceNumber = 0xffffffff;
		pInfo->ullContentSeekPos = ullSeekPos - ptr_last_period->ullOrgStartTime + ptr_last_period->ullIdealStartTime;
	}
	else
	{
		VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_seekitem = get_periodbyorgtime( ptr_info , ullSeekPos );
		VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_ad_needplay = 0;
		VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_cur = ptr_last_period;

		if( !ptr_seekitem )
			return VO_RET_SOURCE2_INVALIDPARAM;

		VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = ptr_cur;

		if( ptr_seekitem->uSequecnNumber < ptr_cur->uSequecnNumber )
		{
			ptr_item = ptr_seekitem->ptr_pre;
			while( ptr_item )
			{
				if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT )
					break;
				ptr_item = ptr_item->ptr_pre;
			}
		}

		while( ptr_item && ptr_item != ptr_seekitem )
		{
			if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS &&
				( ptr_item->ptr_pre == 0 || ( ptr_item->ptr_pre && ptr_item->ptr_pre->nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT ) ) &&
				ptr_item->isPlayed == VO_FALSE )
				ptr_ad_needplay = ptr_item;

			ptr_item = ptr_item->ptr_next;
		}

		if( ptr_ad_needplay )
		{
			pInfo->uAdsSequnceNumber = ptr_ad_needplay->uSequecnNumber;
			ptr_info->is_seek_ads = VO_TRUE;
			ptr_info->seek_ads_starttime = ptr_ad_needplay->ullStartTime;
			ptr_info->status = VO_ADSMANAGER_STATUS_SEEKADS;
		}
		else
			pInfo->uAdsSequnceNumber = 0xffffffff;

		pInfo->ullContentSeekPos = ullSeekPos - ptr_seekitem->ullOrgStartTime + ptr_seekitem->ullIdealStartTime;
	}

	VOLOGI( "Seek Finished: Pos %lld Ads %d Content %lld" , ullSeekPos , pInfo->uAdsSequnceNumber , pInfo->ullContentSeekPos );

	return 0;
}

VO_U32 vo_ads_set_param( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U32 id , VO_PTR param )
{
	//VOLOGI( "ParamID: %d PTR: %p PTR_INFO %p" , id , param , ptr_info );
	if( id == VO_ADSMANAGER_PID_EVENTCALLBACK )
	{
		VO_ADSMANAGER_EVENTCALLBACK * ptr_cb = ( VO_ADSMANAGER_EVENTCALLBACK * )param;

		ptr_info->eventcallback.SendEvent = ptr_cb->SendEvent;
		ptr_info->eventcallback.pUserData = ptr_cb->pUserData;

		return VO_RET_SOURCE2_OK;
	}
	else if( id == VO_ADSMANAGER_PID_DISCONTINUECHUNK )
	{
		VO_ADSMANAGER_CHUNKSAMPLEINFO * ptr_sampleinfo = ( VO_ADSMANAGER_CHUNKSAMPLEINFO * )param;
		VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = get_periodbyperiodid( ptr_info , ptr_sampleinfo->uPeriodSequenceNumber );

		if( !ptr_item )
			return VO_RET_SOURCE2_INVALIDPARAM;

		VO_U32 ret;

		if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS && ptr_info->status == VO_ADSMANAGER_STATUS_SEEKADS )
			ptr_info->status = VO_ADSMANAGER_STATUS_PLAYING_SEEKEDADS;

		if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT && ptr_info->status == VO_ADSMANAGER_STATUS_PLAYING_SEEKEDADS )
			ptr_info->status = VO_ADSMANAGER_STATUS_DROP;

		if( ptr_info->status == VO_ADSMANAGER_STATUS_DROP )
			ret = VO_RET_SOURCE2_ADAPTIVESTREAMING_CHUNK_SKIP;
		else
		{
			if( ptr_item && ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS && ptr_item->isPlayed )
				ret = VO_RET_SOURCE2_ADAPTIVESTREAMING_CHUNK_SKIP;
			else
				ret = VO_RET_SOURCE2_OK;
		}

		VOLOGI( "VO_ADSMANAGER_PID_DISCONTINUECHUNK %d , StartTime: %lld Flag: %08x Period Start: %lld End: %lld Status: %d Ret: %08x" , ptr_sampleinfo->uPeriodSequenceNumber , ptr_sampleinfo->ullChunkStartTime , ptr_sampleinfo->uFlag , ptr_item->ullIdealStartTime , ptr_item->ullIdealEndTime , ptr_info->status , ret );

		return ret;
	}
	else if( id == VO_ADSMANAGER_PID_DISCONTINUESAMPLE )
	{
		VO_ADSMANAGER_CHUNKSAMPLEINFO * ptr_sampleinfo = ( VO_ADSMANAGER_CHUNKSAMPLEINFO * )param;
		VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = get_periodbyperiodid( ptr_info , ptr_sampleinfo->uPeriodSequenceNumber );

		if( !ptr_item )
			return VO_RET_SOURCE2_OK;

		VOLOGI( "VO_ADSMANAGER_PID_DISCONTINUESAMPLE %d StartTime: %lld Period First Chunk StartTime: %lld Period Start: %lld End: %lld" , ptr_sampleinfo->uPeriodSequenceNumber , ptr_sampleinfo->ullChunkStartTime , ptr_sampleinfo->ullPeriodFirstChunkStartTime , ptr_item->ullIdealStartTime , ptr_item->ullIdealEndTime );

		if( ptr_sampleinfo->uFlag & VO_SOURCE2_ADAPTIVESTREAMING_CHUNKFLAG_DISCONTINUE )
		{
			if( ptr_item->ullPeriodStartTsTimeStamp == UNAVALIABLETIME )
			{
				ptr_item->ullPeriodStartTsTimeStamp = *( ptr_sampleinfo->pullSampleTime );
			}
			*( ptr_sampleinfo->pullSampleTime ) = ptr_item->ullStartTime;
		}
		else
		{
			if( ptr_sampleinfo->uFlag & VO_SOURCE2_ADAPTIVESTREAMING_CHUNKFLAG_NEEDREF )
			{
				VOLOGI( "VO_SOURCE2_ADAPTIVESTREAMING_CHUNKFLAG_NEEDREF" );
				if( ptr_item->ullPeriodStartTsTimeStamp == UNAVALIABLETIME )
					*( ptr_sampleinfo->pullSampleTime ) = ( ptr_sampleinfo->ullChunkStartTime - ptr_sampleinfo->ullPeriodFirstChunkStartTime ) + ptr_item->ullStartTime;
				else
					*( ptr_sampleinfo->pullSampleTime ) = ( *( ptr_sampleinfo->pullSampleTime ) - ptr_item->ullPeriodStartTsTimeStamp ) + ptr_item->ullStartTime;
			}
			else
			{
				return VO_RET_SOURCE2_OK;
			}
		}

		VOLOGI( "Force Timestamp to: %lld Period Start Ts TimeStamp: %lld" , *( ptr_sampleinfo->pullSampleTime ) , ptr_item->ullPeriodStartTsTimeStamp );

		return VO_RET_SOURCE2_ADAPTIVESTREAMING_FORCETIMESTAMP;
	}
	else if( id == VO_ADSMANAGER_PID_STARTSEEKCONTENT )
	{
		ptr_info->status = VO_ADSMANAGER_STATUS_RUNNING;
	}
	else
	{
		VOLOGE( "Error ID: %d" , id );
	}

	return VO_RET_SOURCE2_NOIMPLEMENT;
}

VO_U32 vo_ads_get_param( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U32 id , VO_PTR param )
{
	return VO_RET_SOURCE2_NOIMPLEMENT;
}

VO_U32 generate_smil_url( VO_CHAR * ptr_id , VO_CHAR * ptr_url , VO_U32 size )
{
	sprintf( ptr_url , "http://link.theplatform.com/s/dJ5BDC/%s?format=SMIL&Tracking=true&assetType=OnceURL" , ptr_id );
	return VO_RET_SOURCE2_OK;
}


VO_VOID create_playbackinfo( VO_ADSMANAGER_GENERALINFO * ptr_dst , VO_ADSMANAGER_PLAYBACKINFOEX * ptr_src )
{
	for( VO_U32 i = 0 ; i < ptr_src->nCounts ; i++ )
	{
		VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = new VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL;
		memset( ptr_item , 0 , sizeof( VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL ) );

		ptr_item->uID = 0x80000000 | ( i * 100 );
		ptr_item->nPeriodType = VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT;
		strcpy( ptr_item->strCaptionURL , ptr_src->pPeriods[i].strCaptionURL );
		strcpy( ptr_item->strPeriodURL , ptr_src->pPeriods[i].strPeriodURL );
		ptr_item->ullEndTime = ptr_item->ullOrgEndTime = ptr_src->pPeriods[i].ullEndTime;
		ptr_item->ullStartTime = ptr_item->ullOrgStartTime = ptr_src->pPeriods[i].ullStartTime;
		strcpy( ptr_item->strGUID , ptr_src->pPeriods[i].strGUID );

		strcpy( ptr_item->strContentID , ptr_src->pPeriods[i].strContentID );
		strcpy( ptr_item->strTitle , ptr_src->pPeriods[i].strPeriodTitle );
		ptr_item->isLive = ptr_src->pPeriods[i].isLive;
		ptr_item->isPlayed = VO_FALSE;
		ptr_item->isEpisode = ptr_src->pPeriods[i].isEpisode;

		insert_playbackperiod_internallist( ptr_dst , ptr_item );
	}
}

VO_U32 convert_periodurl( VO_ADSMANAGER_GENERALINFO * ptr_info )
{
	VO_PTR h = 0;
	VO_U32 ret = vo_vmap_open( &h , ptr_info->ptr_workingpath );

	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = ptr_info->ptr_internal_begin;

	if( ret != VO_RET_SOURCE2_OK )
		return ret;

	VO_CHAR previous_url[MAXURLLEN];
	memset( previous_url , 0 , sizeof( previous_url ) );

	VO_CHAR previous_converted_url[MAXURLLEN];
	memset( previous_converted_url , 0 , sizeof( previous_converted_url ) );

	while( ptr_item )
	{
		if( strcmp( previous_url , ptr_item->strPeriodURL ) == 0 )
		{
			memset( ptr_item->strPeriodURL , 0 , sizeof( ptr_item->strPeriodURL ) );
			strcpy( ptr_item->strPeriodURL , previous_converted_url );
		}
		else
		{
			VO_CHAR url[MAXURLLEN];
			memset( url , 0 , sizeof(url) );

			strcpy( url , ptr_item->strPeriodURL );
			strcpy( previous_url , url );

#ifdef _VOLOG_INFO
			if( ptr_info->log_fp )
			{
				const VO_CHAR * ptr_temp = "Period URL:";
				fwrite( ptr_temp , strlen( ptr_temp ) , 1 , ptr_info->log_fp );
				fwrite( previous_url , strlen( previous_url ) , 1 , ptr_info->log_fp );

				ptr_temp = "\r\n\r\n";
				fwrite( ptr_temp , strlen( ptr_temp ) , 1 , ptr_info->log_fp );
			}
#endif

			VO_CHAR * ptr_domainguid = NULL;
			VO_CHAR * ptr_applicationguid = NULL;
			VO_CHAR * ptr_mediaitemguid = NULL;

			VO_CHAR * ptr_pos = strrchr( url , '/' );
			*ptr_pos = 0;

			ptr_pos = strrchr( url , '/' );
			*ptr_pos = 0;
			ptr_mediaitemguid = ptr_pos + 1;

			ptr_pos = strrchr( url , '/' );
			*ptr_pos = 0;
			ptr_applicationguid = ptr_pos + 1;

			ptr_pos = strrchr( url , '/' );
			*ptr_pos = 0;
			ptr_domainguid = ptr_pos + 1;

			VO_CHAR tempurl[MAXURLLEN];
			memset( tempurl , 0 , sizeof(tempurl) );

			VO_BOOL is_secure = VO_FALSE;

			if( strcmp( ptr_domainguid , "bb0b18ba-64f5-4b1b-a29f-0ac252f06b68" ) == 0 &&
				( strcmp( ptr_applicationguid , "77a785f3-5188-4806-b788-0893a61634ed" ) == 0 ||
				strcmp( ptr_applicationguid , "bbec2d35-e8ad-4d35-ae85-1a3c8b5e5df2" ) == 0 ||
				strcmp( ptr_applicationguid , "b9079cb5-f902-4451-8bed-dbfb5e3c2be7" ) == 0 ) )
				is_secure = VO_TRUE;

			VO_CHAR * domainguid = ptr_domainguid;
			VO_CHAR * appguid = ptr_applicationguid;

			if( ptr_info->device_type == VO_ADSMANAGER_DEVICETYPE_TABLET )
			{
				domainguid = (VO_CHAR*) (( is_secure == VO_TRUE ) ? SECURE_DOMAINGUID_TABLET : DOMAINGUID_TABLET);
				appguid = (VO_CHAR*) (( is_secure == VO_TRUE ) ? SECURE_APPLICATIONGUID_TABLET : APPLICATIONGUID_TABLET);
			}
			else if( ptr_info->device_type == VO_ADSMANAGER_DEVICETYPE_PHONE )
			{
				domainguid = (VO_CHAR*) (( is_secure == VO_TRUE ) ? SECURE_DOMAINGUID_PHONE : DOMAINGUID_PHONE);
				appguid = (VO_CHAR*) (( is_secure == VO_TRUE ) ? SECURE_APPLICATIONGUID_PHONE : APPLICATIONGUID_PHONE );
			}

			if( ptr_info->flag & VO_ADSMANAGER_OPENFLAG_DEBUG )
			{
				appguid = (VO_CHAR*)(( is_secure == VO_TRUE ) ? DEBUG_SECURE_APPLICATIONGUID : DEBUG_APPLICATIONGUID );
			}

			if( ptr_info->str_partner && strlen( ptr_info->str_partner ) )
			{
				if( ptr_info->flag & VO_ADSMANAGER_OPENFLAG_DEBUG )
					sprintf( tempurl , "http://oas25.phx.unicornapp.com/now/ads/vmap/adaptive/m3u8/%s/%s/%s?UMPTPARAMcust_params=vid%%3D%s%%26partner%%3D%s&UMTP=0" , domainguid , appguid , ptr_mediaitemguid , ptr_item->strGUID , ptr_info->str_partner );
				else
					sprintf( tempurl , "http://onceux.unicornmedia.com/now/ads/vmap/adaptive/m3u8/%s/%s/%s?UMPTPARAMcust_params=vid%%3D%s%%26partner%%3D%s&UMTP=0" , domainguid , appguid , ptr_mediaitemguid , ptr_item->strGUID , ptr_info->str_partner );
			}
			else
			{
				if( ptr_info->flag & VO_ADSMANAGER_OPENFLAG_DEBUG )
					sprintf( tempurl , "http://oas25.phx.unicornapp.com/now/ads/vmap/adaptive/m3u8/%s/%s/%s?UMTP=0" , domainguid , appguid , ptr_mediaitemguid );
				else
					sprintf( tempurl , "http://onceux.unicornmedia.com/now/ads/vmap/adaptive/m3u8/%s/%s/%s?UMTP=0" , domainguid , appguid , ptr_mediaitemguid );
			}

			if( is_secure )
				append_verification( ptr_info , tempurl );

			VO_CHAR * ptr_vmap = 0;
			VO_U32 size = 0;

			ret = vo_download_by_url( tempurl , (VO_PBYTE *)&ptr_vmap , &size , ptr_info->ptr_workingpath , &( ptr_info->is_stop ) );

			/*FILE * fp = fopen( "D:\\Data\\6\\VMAP.xml" , "r" );
			fseek( fp , 0 , SEEK_END );
			size = ftell( fp );
			ptr_vmap = new VO_CHAR[size + 1];
			memset( ptr_vmap , 0 , size + 1 );
			fseek( fp , 0 , SEEK_SET );
			fread( ptr_vmap , size , 1 , fp );
			fclose( fp );*/


			if( ret != VO_RET_SOURCE2_OK )
			{
				post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_ERROR_VMAP_DOWNLOADFAIL , 0 , 0 );
				break;
			}

#ifdef _VOLOG_INFO
			FILE * fp = fopen( "/mnt/sdcard/CBSi/VMAP.xml" , "wb+" );

			if( fp )
			{
				fwrite( ptr_vmap , size , 1 , fp );
				fclose( fp );
			}

			if( ptr_info->log_fp )
			{
				const VO_CHAR * ptr_temp = "VMAP URL:";
				fwrite( ptr_temp , strlen( ptr_temp ) , 1 , ptr_info->log_fp );
				fwrite( tempurl , strlen( tempurl ) , 1 , ptr_info->log_fp );

				ptr_temp = "\r\n--------->VMAP.xml\r\n\r\n";
				fwrite( ptr_temp , strlen( ptr_temp ) , 1 , ptr_info->log_fp );
			}
#endif

			ret = vo_vmap_process( h , (VO_PBYTE)ptr_vmap , size );

			delete []ptr_vmap;

			if( ret != VO_RET_SOURCE2_OK )
			{
				post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_ERROR_VMAP_PARSEERROR , 0 , 0 );
				break;
			}

			memset( ptr_item->strPeriodURL , 0 , sizeof( ptr_item->strPeriodURL ) );

			ret = vo_vmap_get_contenturl( h , ptr_item->strPeriodURL );

			if( ret != VO_RET_SOURCE2_OK )
				break;

			strcpy( previous_converted_url , ptr_item->strPeriodURL );

#ifdef _VOLOG_INFO
			if( ptr_info->log_fp )
			{
				const VO_CHAR * ptr_temp = "Master M3U8 URL:";
				fwrite( ptr_temp , strlen( ptr_temp ) , 1 , ptr_info->log_fp );
				fwrite( previous_converted_url , strlen( previous_converted_url ) , 1 , ptr_info->log_fp );

				ptr_temp = "\r\n\r\n";
				fwrite( ptr_temp , strlen( ptr_temp ) , 1 , ptr_info->log_fp );
			}
#endif
		}

		ptr_item = ptr_item->ptr_next;
	}

	if( ret == VO_RET_SOURCE2_OK )
	{
		VOAdInfo * ptr_adinfo;

		ret = vo_vmap_get_data( h , &ptr_adinfo );

		if( ret == VO_RET_SOURCE2_OK )
		{
			VOLOGI( "%d" , ptr_adinfo->uAdBreakCount );
			VO_BOOL result = merge_ads_into_periodinfo( ptr_info , ptr_adinfo );

			if( !result )
			{
				ret = VO_RET_SOURCE2_FAIL;
				post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_ERROR_VMAP_PARSEERROR , 0 , 0 );
				return ret;
			}
		}
	}

	vo_vmap_close( h );

	return ret;
}

VO_VOID insert_playbackperiod_internallist( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item )
{
	if( ptr_info->ptr_internal_begin == 0 && ptr_info->ptr_internal_end == 0 )
	{
		ptr_info->ptr_internal_begin = ptr_info->ptr_internal_end = ptr_item;
	}
	else
	{
		ptr_item->ptr_pre = ptr_info->ptr_internal_end;
		ptr_info->ptr_internal_end->ptr_next = ptr_item;
		ptr_info->ptr_internal_end = ptr_item;
	}
}

VO_VOID destroy_playbackperiod_internallist( VO_ADSMANAGER_GENERALINFO * ptr_info )
{
	if( !ptr_info )
		return;

	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = ptr_info->ptr_internal_begin;

	while( ptr_item )
	{
		VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_temp = ptr_item;
		ptr_item = ptr_item->ptr_next;

		destroy_urllist( ptr_temp->ptr_trackingevent_ad_complete );
		destroy_urllist( ptr_temp->ptr_trackingevent_ad_creative_view );
		destroy_urllist( ptr_temp->ptr_trackingevent_ad_first_quartile );
		destroy_urllist( ptr_temp->ptr_trackingevent_ad_mid_point );
		destroy_urllist( ptr_temp->ptr_trackingevent_ad_third_quartile );
		destroy_urllist( ptr_temp->ptr_trackingevent_ad_fullscreen );
		destroy_urllist( ptr_temp->ptr_trackingevent_ad_impression );
		destroy_urllist( ptr_temp->ptr_trackingevent_ad_mute );
		destroy_urllist( ptr_temp->ptr_trackingevent_ad_unmute );
		destroy_urllist( ptr_temp->ptr_trackingevent_ad_pause );
		destroy_urllist( ptr_temp->ptr_trackingevent_ad_resume );
		destroy_urllist( ptr_temp->ptr_trackingevent_ad_start );
		delete ptr_temp;
	}

	ptr_info->ptr_internal_begin = ptr_info->ptr_internal_end = 0;
}

VO_VOID destory_periodinfo( VO_ADSMANAGER_GENERALINFO * ptr_info )
{
	if( !ptr_info )
		return;

	if( ptr_info->pInfo && ptr_info->pInfo->pPeriods )
		delete []ptr_info->pInfo->pPeriods;

	if( ptr_info->pInfo )
		delete ptr_info->pInfo;

	ptr_info->pInfo = 0;
}

VO_VOID create_periodinfo( VO_ADSMANAGER_GENERALINFO * ptr_info )
{
	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = ptr_info->ptr_internal_begin;

	VO_U32 count = 0;
	while( ptr_item )
	{
		count++;
		ptr_item = ptr_item->ptr_next;
	}

	ptr_info->pInfo = new VO_ADSMANAGER_PLAYBACKINFO;

	ptr_info->pInfo->nCounts = count;
	ptr_info->pInfo->pPeriods = new VO_ADSMANAGER_PLAYBACKPERIOD[count];

	count = 0;
	ptr_item = ptr_info->ptr_internal_begin;
	while( ptr_item )
	{
		ptr_info->pInfo->pPeriods[count].uID = ptr_item->uID;
		strcpy( ptr_info->pInfo->pPeriods[count].strCaptionURL , ptr_item->strCaptionURL );
		strcpy( ptr_info->pInfo->pPeriods[count].strPeriodURL , ptr_item->strPeriodURL );
		ptr_info->pInfo->pPeriods[count].nPeriodType = ptr_item->nPeriodType;
		ptr_info->pInfo->pPeriods[count].ullStartTime = ptr_item->ullStartTime;
		ptr_info->pInfo->pPeriods[count].ullEndTime = ptr_item->ullEndTime;
		strcpy( ptr_info->pInfo->pPeriods[count].strPeriodID , ptr_item->strContentID );
		strcpy( ptr_info->pInfo->pPeriods[count].strPeriodTitle , ptr_item->strTitle );
		ptr_info->pInfo->pPeriods[count].isLive = ptr_item->isLive;
		ptr_info->pInfo->pPeriods[count].pReserved1 = ptr_info->pInfo->pPeriods[count].pReserved2 = 0;
		ptr_info->pInfo->pPeriods[count].isEpisode = ptr_item->isEpisode;

		ptr_item = ptr_item->ptr_next;
		count++;
	}

}

VO_BOOL merge_ads_into_periodinfo( VO_ADSMANAGER_GENERALINFO * ptr_info , VOAdInfo * ptr_adsinfo )
{
	VO_U64 duration = ptr_info->ptr_internal_end->ullOrgEndTime;

	pVOAdBreak ptr_ad = ptr_adsinfo->pAdBreak;

	for( VO_U32 i = 0 ; i < ptr_adsinfo->uAdBreakCount ; i++ )
	{
		VO_U64 inserttime = ptr_ad->Time.ullTime;

		if( ptr_ad->Time.TimeIndex == VOAdTimeType_StartEnd )
		{
			if( ptr_ad->Time.ullTime == 0 )
				inserttime = 0;
			else
				inserttime = duration;
		}
		else if( ptr_ad->Time.TimeIndex == VOAdTimeType_Percentage )
		{
			inserttime = (VO_U64)(duration * ptr_ad->Time.ullTime / 100.);
		}
		else if( ptr_ad->Time.TimeIndex == VOAdTimeType_Positon )
		{
			VOLOGE( "Do not support right now!!!" );
			//continue;
			return VO_FALSE;
		}
		else if( ptr_ad->Time.TimeIndex == VOAdTimeType_Unknow )
		{
			VOLOGE( "Do not support!!!" );
			return VO_FALSE;
		}

		VO_U64 ad_duration = 0;

		if( ptr_ad->pAdSource &&
			ptr_ad->pAdSource->pAdData && 
			ptr_ad->pAdSource->pAdData->pInLine )
		{
			VOAdInLine * ptr_inline = ptr_ad->pAdSource->pAdData->pInLine;

			VOAdCreative * ptr_creative = ptr_inline->pCreative;

			for( VO_S32 i = 0 ; i < ptr_inline->uCreativeCount ; i++ )
			{
				if( ptr_creative->CreativeType == VOAdBreakType_Linear )
				{
					if( ptr_inline->pCreative->Linear.ullDuration > ad_duration )
						ad_duration = ptr_inline->pCreative->Linear.ullDuration;
				}
				ptr_creative = ptr_creative->pNext;
			}
		}


		VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = new VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL;
		memset( ptr_item , 0 , sizeof( VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL ) );
		ptr_item->uID = i;
		ptr_item->nPeriodType = VO_ADSMANAGER_PERIODTYPE_ADS;
		ptr_item->ullStartTime = inserttime;
		ptr_item->ullEndTime = ptr_item->ullStartTime + ad_duration;
		ptr_item->isPlayed = VO_FALSE;

		if( ptr_ad->pAdSource && 
			ptr_ad->pAdSource->pAdData &&
			ptr_ad->pAdSource->pAdData->pInLine &&
			ptr_ad->pAdSource->pAdData->pInLine->strAdTitle )
			strcpy( ptr_item->strTitle , ptr_ad->pAdSource->pAdData->pInLine->strAdTitle );

		if( ptr_ad->pAdSource && 
			ptr_ad->pAdSource->pAdData &&
			ptr_ad->pAdSource->pAdData->pInLine &&
			ptr_ad->pAdSource->pAdData->pInLine->pImpression )
		{
			VOAdImpression * ptr_impression_item = ptr_ad->pAdSource->pAdData->pInLine->pImpression;

			while( ptr_impression_item )
			{
				VO_ADSMANAGER_URLLIST * ptr_url = new VO_ADSMANAGER_URLLIST;
				strcpy( ptr_url->url , ptr_impression_item->strURI );
				ptr_url->ptr_next = ptr_item->ptr_trackingevent_ad_impression;
				ptr_item->ptr_trackingevent_ad_impression = ptr_url;

				ptr_impression_item = ptr_impression_item->pNext;
			}
		}

		if( ptr_ad->pAdSource && 
			ptr_ad->pAdSource->pAdData &&
			ptr_ad->pAdSource->pAdData->pInLine &&
			ptr_ad->pAdSource->pAdData->pInLine->pCreative &&
			ptr_ad->pAdSource->pAdData->pInLine->pCreative->CreativeType == VOAdBreakType_Linear &&
			ptr_ad->pAdSource->pAdData->pInLine->pCreative->Linear.uTrackingEvenCount > 0 && 
			ptr_ad->pAdSource->pAdData->pInLine->pCreative->Linear.pTrackingEvents )
		{
			VOAdTrackingEvent * ptr_event = ptr_ad->pAdSource->pAdData->pInLine->pCreative->Linear.pTrackingEvents;

			while( ptr_event )
			{
				VO_ADSMANAGER_URLLIST ** ptr_class = 0;

				if( strcmp( ptr_event->strEvent , "start" ) == 0 )
					ptr_class = &(ptr_item->ptr_trackingevent_ad_start);
				else if( strcmp( ptr_event->strEvent , "firstQuartile" ) == 0 )
					ptr_class = &(ptr_item->ptr_trackingevent_ad_first_quartile);
				else if( strcmp( ptr_event->strEvent , "midpoint" ) == 0 )
					ptr_class = &(ptr_item->ptr_trackingevent_ad_mid_point);
				else if( strcmp( ptr_event->strEvent , "thirdQuartile" ) == 0 )
					ptr_class = &(ptr_item->ptr_trackingevent_ad_third_quartile);
				else if( strcmp( ptr_event->strEvent , "complete" ) == 0 )
					ptr_class = &(ptr_item->ptr_trackingevent_ad_complete);
				else if( strcmp( ptr_event->strEvent , "mute" ) == 0 )
					ptr_class = &(ptr_item->ptr_trackingevent_ad_mute);
				else if( strcmp( ptr_event->strEvent , "unmute" ) == 0 )
					ptr_class = &(ptr_item->ptr_trackingevent_ad_unmute);
				else if( strcmp( ptr_event->strEvent , "pause" ) == 0 )
					ptr_class = &(ptr_item->ptr_trackingevent_ad_pause);
				else if( strcmp( ptr_event->strEvent , "resume" ) == 0 )
					ptr_class = &(ptr_item->ptr_trackingevent_ad_resume);
				else if( strcmp( ptr_event->strEvent , "fullscreen" ) == 0 )
					ptr_class = &(ptr_item->ptr_trackingevent_ad_fullscreen);
				else if( strcmp( ptr_event->strEvent , "creativeView" ) == 0 )
					ptr_class = &(ptr_item->ptr_trackingevent_ad_creative_view);
				else
					ptr_class = 0;

				if( ptr_class )
				{
					VO_ADSMANAGER_URLLIST * ptr_url = new VO_ADSMANAGER_URLLIST;
					strcpy( ptr_url->url , ptr_event->strURI );
					ptr_url->ptr_next = *ptr_class;
					*ptr_class = ptr_url;
				}

				ptr_event = ptr_event->pNext;
			}
		}

		if( ptr_ad->pAdSource && 
			ptr_ad->pAdSource->pAdData &&
			ptr_ad->pAdSource->pAdData->pInLine &&
			ptr_ad->pAdSource->pAdData->pInLine->pCreative &&
			ptr_ad->pAdSource->pAdData->pInLine->pCreative->CreativeType == VOAdBreakType_Linear &&
			ptr_ad->pAdSource->pAdData->pInLine->pCreative->Linear.uVideoClicksCount > 0 &&
			ptr_ad->pAdSource->pAdData->pInLine->pCreative->Linear.pVideoClicks && 
			ptr_ad->pAdSource->pAdData->pInLine->pCreative->Linear.pVideoClicks->uClickThroughCount > 0 &&
			ptr_ad->pAdSource->pAdData->pInLine->pCreative->Linear.pVideoClicks->pClickThrough &&
			ptr_ad->pAdSource->pAdData->pInLine->pCreative->Linear.pVideoClicks->pClickThrough->strURI )
			strcpy( ptr_item->strClickURL , ptr_ad->pAdSource->pAdData->pInLine->pCreative->Linear.pVideoClicks->pClickThrough->strURI );

		if( ptr_ad->pAdSource && 
			ptr_ad->pAdSource->pAdData &&
			ptr_ad->pAdSource->pAdData->strAdID )
			strcpy( ptr_item->strContentID , ptr_ad->pAdSource->pAdData->strAdID );

		VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_period = ptr_info->ptr_internal_begin;

		while( ptr_period )
		{
			if( ptr_period->nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT )
			{
				if( ptr_item->ullStartTime <= ptr_period->ullOrgStartTime )
				{
					break;
				}
				else if( ptr_item->ullStartTime < ptr_period->ullOrgEndTime )
				{
					ptr_period = divid_playbackperiod_internallistitem( ptr_info , ptr_period , ptr_item->ullStartTime );
					//insert_playbackperiod_internallist( ptr_info , ptr_period , ptr_item );
					break;
				}
			}

			ptr_period = ptr_period->ptr_next;
		}

		insert_playbackperiod_internallist( ptr_info , ptr_period , ptr_item );

		VO_U64 offset;
		/*if( ptr_period )
			offset = ( ptr_item->ullEndTime > ptr_period->ullStartTime ? ptr_item->ullEndTime : ptr_period->ullStartTime - ptr_period->ullEndTime ) + 1;
		else*/
			offset = ad_duration + 1;

		recalculate_periodinfo_timestamp( ptr_info , ptr_period , offset );

		ptr_ad = ptr_ad->pNext;
	}

	return VO_TRUE;
}

VO_VOID insert_playbackperiod_internallist( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_insertpos , VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item )
{
	if( ptr_info->ptr_internal_begin == ptr_insertpos )
	{
		ptr_item->ptr_next = ptr_insertpos;
		ptr_insertpos->ptr_pre = ptr_item;
		ptr_info->ptr_internal_begin = ptr_item;
	}
	else if( ptr_insertpos == 0 )
	{
		ptr_info->ptr_internal_end->ptr_next = ptr_item;
		ptr_item->ptr_pre = ptr_info->ptr_internal_end;
		ptr_info->ptr_internal_end = ptr_item;
		
		VO_U64 duration = ptr_item->ullEndTime - ptr_item->ullStartTime;
		ptr_item->ullStartTime = ptr_item->ptr_pre->ullEndTime + 1;
		ptr_item->ullEndTime = ptr_item->ullStartTime + duration;
	}
	else
	{
		ptr_item->ptr_pre = ptr_insertpos->ptr_pre;
		ptr_item->ptr_next = ptr_insertpos;
		ptr_insertpos->ptr_pre->ptr_next = ptr_item;
		ptr_insertpos->ptr_pre = ptr_item;

		VO_U64 duration = ptr_item->ullEndTime - ptr_item->ullStartTime;
		ptr_item->ullStartTime = ptr_item->ptr_pre->ullEndTime + 1;
		ptr_item->ullEndTime = ptr_item->ullStartTime + duration;
	}
}

VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * divid_playbackperiod_internallistitem( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_dividitem , VO_U64 divide_time )
{
	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = new VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL;
	memcpy( ptr_item , ptr_dividitem , sizeof( VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL ) );
	ptr_item->ptr_next = ptr_item->ptr_pre = 0;

	ptr_item->ullOrgEndTime = divide_time - 1;
	ptr_item->ullEndTime = ptr_item->ullOrgEndTime - ptr_item->ullOrgStartTime + ptr_item->ullStartTime;

	ptr_dividitem->uID++;
	ptr_dividitem->ullOrgStartTime = divide_time;
	ptr_dividitem->ullStartTime = ptr_dividitem->ullEndTime - ( ptr_dividitem->ullOrgEndTime - ptr_dividitem->ullOrgStartTime );

	insert_playbackperiod_internallist( ptr_info , ptr_dividitem , ptr_item );

	return ptr_dividitem;
}

VO_VOID recalculate_periodinfo_timestamp( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_start_recalculate_item , VO_U64 offset )
{
	while( ptr_start_recalculate_item )
	{
		ptr_start_recalculate_item->ullStartTime  = ptr_start_recalculate_item->ullStartTime + offset;
		ptr_start_recalculate_item->ullEndTime = ptr_start_recalculate_item->ullEndTime + offset;

		ptr_start_recalculate_item = ptr_start_recalculate_item->ptr_next;
	}
}

unsigned int message_func( void * ptr_userdata , unsigned int msg_id , void * ptr_param1 , void * ptr_param2 )
{
	VO_ADSMANAGER_GENERALINFO * ptr_info = ( VO_ADSMANAGER_GENERALINFO * )ptr_userdata;

	if( msg_id < VO_MESSAGE_BASE || msg_id > VO_MESSAGE_USER )
	{
		if( msg_id == VO_ADSMANAGER_EVENT_CONTENT_START )
		{
			VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO * ptr_info = ( VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO * ) ptr_param1;

			VOLOGI( "VO_ADSMANAGER_EVENT_CONTENT_START\t%d\tID: %08x\tElapsedTime: %lld\n" , (VO_U32)ptr_param2 , ptr_info->uPeriodID , ptr_info->ullElapsedTime );

		}
		else if( msg_id == VO_ADSMANAGER_EVENT_CONTENT_END )
		{
			VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO * ptr_info = ( VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO * ) ptr_param1;

			VOLOGI( "VO_ADSMANAGER_EVENT_CONTENT_END\t%d\tID: %08x\tElapsedTime: %lld\n" , (VO_U32)ptr_param2 , ptr_info->uPeriodID , ptr_info->ullElapsedTime );
		}
		else if( msg_id == VO_ADSMANAGER_EVENT_AD_START )
		{
			VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO * ptr_info = ( VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO * ) ptr_param1;

			VOLOGI( "VO_ADSMANAGER_EVENT_AD_START\t%d\tID: %08x\tElapsedTime: %lld\n" , (VO_U32)ptr_param2 , ptr_info->uPeriodID , ptr_info->ullElapsedTime );
		}
		else if( msg_id == VO_ADSMANAGER_EVENT_AD_END )
		{
			VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO * ptr_info = ( VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO * ) ptr_param1;

			VOLOGI( "VO_ADSMANAGER_EVENT_AD_END\t%d\tID: %08x\tElapsedTime: %lld\n" , (VO_U32)ptr_param2 , ptr_info->uPeriodID , ptr_info->ullElapsedTime );
		}
		else if( msg_id == VO_ADSMANAGER_EVENT_WHOLECONTENT_START )
		{
			VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO * ptr_info = ( VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO * ) ptr_param1;

			VOLOGI( "VO_ADSMANAGER_EVENT_WHOLECONTENT_START\t%d\tID: %08x\tElapsedTime: %lld\n" , (VO_U32)ptr_param2 , ptr_info->uPeriodID , ptr_info->ullElapsedTime );
		}
		else if( msg_id == VO_ADSMANAGER_EVENT_WHOLECONTENT_END )
		{
			VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO * ptr_info = ( VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO * ) ptr_param1;

			VOLOGI( "VO_ADSMANAGER_EVENT_WHOLECONTENT_END\t%d\tID: %08x\tElapsedTime: %lld\n" , (VO_U32)ptr_param2 , ptr_info->uPeriodID , ptr_info->ullElapsedTime );
		}
		else if( msg_id == VO_ADSMANAGER_EVENT_TRACKING_PERIODPRECENTAGE )
		{
			VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO * ptr_info = ( VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO * ) ptr_param1;

			VOLOGI( "VO_ADSMANAGER_EVENT_TRACKING_PERIODPRECENTAGE\t%d\t%d%%\tID: %08x\tElapsedTime: %lld\n" , (VO_U32)ptr_param2 , ptr_info->uPercentage , ptr_info->uPeriodID , ptr_info->ullElapsedTime );
		}
		else if( msg_id == VO_ADSMANAGER_EVENT_TRACKING_WHOLECONTENTPRECENTAGE )
		{
			VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO * ptr_info = ( VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO * ) ptr_param1;

			VOLOGI( "VO_ADSMANAGER_EVENT_TRACKING_WHOLECONTENTPRECENTAGE\t%d\t%d%%\tID: %08x\tElapsedTime: %lld\n" , (VO_U32)ptr_param2 , ptr_info->uPercentage , ptr_info->uPeriodID , ptr_info->ullElapsedTime );
		}
		else if( msg_id == VO_ADSMANAGER_EVENT_TRACKING_TIMEPASSED )
		{
			VO_ADSMANAGER_EVENT_TRAKCING_TIMEPASSEDINFO * ptr_info = ( VO_ADSMANAGER_EVENT_TRAKCING_TIMEPASSEDINFO * )ptr_param1;
			VOLOGI( "VO_ADSMANAGER_EVENT_TRACKING_TIMEPASSED\t%d\tID: %08x\tPassedTime: %lld\n" , (VO_U32)ptr_param2 , ptr_info->uPeriodID , ptr_info->ullTimePassed );
		}
		else if( msg_id == VO_ADSMANAGER_EVENT_TRACKING_ACTION )
		{
			VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO * ptr_info = ( VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO * ) ptr_param1;

			switch( ptr_info->nAction )
			{
			case VO_ADSMANAGER_ACTION_CLICK:
				{
					VOLOGI( "VO_ADSMANAGER_ACTION_CLICK\t%d\tID: %08x\tElapsedTime: %lld\n" , (VO_U32)ptr_param2 , ptr_info->uPeriodID , ptr_info->ullElapsedTime );
				}
				break;
			case VO_ADSMANAGER_ACTION_PLAYBACKSTART:
				{
					VOLOGI( "VO_ADSMANAGER_ACTION_PLAYBACKSTART\t%d\tID: %08x\tElapsedTime: %lld\n" , (VO_U32)ptr_param2 , ptr_info->uPeriodID , ptr_info->ullElapsedTime );
				}
				break;
			case VO_ADSMANAGER_ACTION_PLAYBACKCOMPLETE:
				{
					VOLOGI( "VO_ADSMANAGER_ACTION_PLAYBACKCOMPLETE\t%d\tID: %08x\tElapsedTime: %lld\n" , (VO_U32)ptr_param2 , ptr_info->uPeriodID , ptr_info->ullElapsedTime );
				}
				break;
			case VO_ADSMANAGER_ACTION_PAUSE:
				{
					VOLOGI( "VO_ADSMANAGER_ACTION_PAUSE\t%d\tID: %08x\tElapsedTime: %lld\n" , (VO_U32)ptr_param2 , ptr_info->uPeriodID , ptr_info->ullElapsedTime );
				}
				break;
			case VO_ADSMANAGER_ACTION_SEEK:
				{
					VOLOGI( "VO_ADSMANAGER_ACTION_SEEK\t%d\tID: %08x\tElapsedTime: %lld\n" , (VO_U32)ptr_param2 , ptr_info->uPeriodID , ptr_info->ullElapsedTime );
				}
				break;
			case VO_ADSMANAGER_ACTION_DRAGSTART:
				{
					VOLOGI( "VO_ADSMANAGER_ACTION_DRAGSTART\t%d\tID: %08x\tElapsedTime: %lld\n" , (VO_U32)ptr_param2 , ptr_info->uPeriodID , ptr_info->ullElapsedTime );
				}
				break;
			case VO_ADSMANAGER_ACTION_RESUME:
				{
					VOLOGI( "VO_ADSMANAGER_ACTION_RESUME\t%d\tID: %08x\tElapsedTime: %lld\n" , (VO_U32)ptr_param2 , ptr_info->uPeriodID , ptr_info->ullElapsedTime );
				}
				break;
			case VO_ADSMANAGER_ACTION_FORCESTOP:
				{
					VOLOGI( "VO_ADSMANAGER_ACTION_FORCESTOP\t%d\tID: %08x\tElapsedTime: %lld\n" , (VO_U32)ptr_param2 , ptr_info->uPeriodID , ptr_info->ullElapsedTime );
				}
				break;
			}
		}
		else if( msg_id == VO_ADSMANAGER_EVENT_OPEN_COMPLETE )
		{
			VOLOGI( "VO_ADSMANAGER_EVENT_OPEN_COMPLETE\t%d" , (VO_U32)ptr_param1 );
		}
		else if ( msg_id == VO_ADSMANAGER_EVENT_ERROR_GEOBLOCK )
		{
			VOLOGI( "VO_ADSMANAGER_EVENT_ERROR_GEOBLOCK");
		}
		else if( msg_id == VO_ADSMANAGER_EVENT_ERROR_PID_INVALID )
		{
			VOLOGI( "VO_ADSMANAGER_EVENT_ERROR_PID_INVALID");
		}
		else if( msg_id == VO_ADSMANAGER_EVENT_ERROR_PID_EXPIRED )
		{
			VOLOGI( "VO_ADSMANAGER_EVENT_ERROR_PID_EXPIRED");
		}
		else if( msg_id == VO_ADSMANAGER_EVENT_ERROR_PID_NOTAVALIBLE )
		{
			VOLOGI( "VO_ADSMANAGER_EVENT_ERROR_PID_NOTAVALIBLE");
		}
		else if( msg_id == VO_ADSMANAGER_EVENT_ERROR_SMIL_DOWNLOADFAIL )
		{
			VOLOGI( "VO_ADSMANAGER_EVENT_ERROR_SMIL_DOWNLOADFAIL");
		}
		else if( msg_id == VO_ADSMANAGER_EVENT_ERROR_SMIL_PARSEFAIL )
		{
			VOLOGI( "VO_ADSMANAGER_EVENT_ERROR_SMIL_PARSEFAIL");
		}
		else if( msg_id == VO_ADSMANAGER_EVENT_ERROR_VMAP_AUTHFAIL )
		{
			VOLOGI( "VO_ADSMANAGER_EVENT_ERROR_VMAP_AUTHFAIL");
		}
		else if( msg_id == VO_ADSMANAGER_EVENT_ERROR_VMAP_DOWNLOADFAIL )
		{
			VOLOGI( "VO_ADSMANAGER_EVENT_ERROR_VMAP_DOWNLOADFAIL");
		}
		else if( msg_id == VO_ADSMANAGER_EVENT_ERROR_VMAP_PARSEERROR )
		{
			VOLOGI( "VO_ADSMANAGER_EVENT_ERROR_VMAP_PARSEERROR");
		}
		else if( msg_id >= VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE && msg_id <= VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_MAX )
		{
			switch ( msg_id )
			{
			case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_AD_START:
				VOLOGI( "VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_AD_START:");
				break;
			case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_FIRST_QUARTILE:
				VOLOGI( "VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_FIRST_QUARTILE:");
				break;
			case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_MID_POINT:
				VOLOGI( "VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_MID_POINT:");
				break;
			case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_THIRD_QUARTILE:
				VOLOGI( "VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_THIRD_QUARTILE:");
				break;
			case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_COMPLETE:
				VOLOGI( "VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_COMPLETE:");
				break;
			case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_CREATIVE_VIEW:
				VOLOGI( "VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_CREATIVE_VIEW:");
				break;
			case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_IMPRESSION:
				VOLOGI( "VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_IMPRESSION:");
				break;
			case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_MUTE:
				VOLOGI( "VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_MUTE:");
				break;
			case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_UNMUTE:
				VOLOGI( "VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_UNMUTE:");
				break;
			case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_PAUSE:
				VOLOGI( "VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_PAUSE:");
				break;
			case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_RESUME:
				VOLOGI( "VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_RESUME:");
				break;
			case VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_FULLSCREEN:
				VOLOGI( "VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_FULLSCREEN:");
				break;
			default:
				return VO_RET_SOURCE2_OK;
			}
			VO_ADSMANAGER_TRACKINGEVENT_INFO * ptr_list = ( VO_ADSMANAGER_TRACKINGEVENT_INFO *)ptr_param1;

			for( VO_U32 i = 0 ; i < ptr_list->uUrlCount ; i++ )
			{
				VOLOGI( "\t%s\n" , ptr_list->ppUrl[i] );
			}
		}

		//VOLOGI( "+SendEvent" );
		ptr_info->eventcallback.SendEvent( ptr_info->eventcallback.pUserData , msg_id , (VO_U32)ptr_param1 , (VO_U32)ptr_param2 );
		//VOLOGI( "-SendEvent" );
	}

	return VO_RET_SOURCE2_OK;
}

VO_VOID detect_period_percentage( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item , VO_U64 timestamp )
{
	if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT )
		return;

	if( ( ( ptr_info->last_playingtime < ptr_item->ullStartTime || ptr_info->last_playingtime > ptr_item->ullStartTime + 300 ) && timestamp < ptr_item->ullStartTime + 300 ) || 
		ptr_info->last_playingtime == UNAVALIABLETIME )
	{
		VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO info;
		info.uPercentage = 0;
		info.uPeriodID = ptr_item->uID;
		info.ullElapsedTime = timestamp - ptr_info->ping_time;
		post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_TRACKING_PERIODPRECENTAGE , &info , (void *)timestamp );
		ptr_info->ping_time = timestamp;

		ptr_item->uLastPercentage = 0;
	}
	else if( ptr_info->last_playingtime < ptr_item->ullEndTime - 300 &&
		timestamp < ptr_item->ullEndTime - 300 )
	{
		if( ptr_item->ullEndTime == ptr_item->ullStartTime )
		{
			return;
		}

		VO_U32 pre_percent = ( ptr_info->last_playingtime - ptr_item->ullStartTime ) * 100 / ( ptr_item->ullEndTime - ptr_item->ullStartTime );
		VO_U32 next_percent = ( pre_percent / 5 + 1 ) * 5;

		VO_U32 cur_precent = ( timestamp - ptr_item->ullStartTime ) * 100 / ( ptr_item->ullEndTime - ptr_item->ullStartTime );

		if( cur_precent >= next_percent )
		{
			VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO info;
			info.uPercentage = next_percent;
			info.uPeriodID = ptr_item->uID;
			info.ullElapsedTime = timestamp - ptr_info->ping_time;
			post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_TRACKING_PERIODPRECENTAGE , &info , (void *)timestamp );

			//if( next_percent == 0 || next_percent == 25 || next_percent == 50 || next_percent == 75 || next_percent == 100 )
			if( next_percent == 25 )
				post_trackingevents( ptr_info , VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_FIRST_QUARTILE , ptr_item->uID , ptr_item->ptr_trackingevent_ad_first_quartile , timestamp );
			if( next_percent == 50 )
				post_trackingevents( ptr_info , VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_MID_POINT , ptr_item->uID , ptr_item->ptr_trackingevent_ad_mid_point , timestamp );
			if( next_percent == 75 )
				post_trackingevents( ptr_info , VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_THIRD_QUARTILE , ptr_item->uID , ptr_item->ptr_trackingevent_ad_third_quartile , timestamp );
			ptr_info->ping_time = timestamp;

			ptr_item->uLastPercentage = next_percent;
		}
	}
	else if( ptr_info->last_playingtime < ptr_item->ullEndTime - 300 && 
		timestamp >= ptr_item->ullEndTime - 300 )
	{
		VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO info;
		info.uPercentage = 100;
		info.uPeriodID = ptr_item->uID;
		info.ullElapsedTime = timestamp - ptr_info->ping_time;
		post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_TRACKING_PERIODPRECENTAGE , &info , (void *)timestamp );
		ptr_info->ping_time = timestamp;

		ptr_item->uLastPercentage = 0xffffffff;
	}
}

VO_VOID detect_period_timepass( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item , VO_U64 timestamp )
{
	if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_ADS )
		return;

	VO_U64 cur_passed = 0;
	vo_ads_convert_timestamp( ptr_info , &cur_passed , timestamp );

	VO_U64 pre_passed = 0;
	vo_ads_convert_timestamp( ptr_info , &pre_passed , ptr_info->last_playingtime );

	VO_U32 next_passed = ( pre_passed / 15000 + 1 ) * 15000;

	if( (VO_U64)((cur_passed/15000)*15000) >= next_passed )
	{
		VO_ADSMANAGER_EVENT_TRAKCING_TIMEPASSEDINFO info;
		info.uPeriodID = ptr_item->uID;
		info.ullTimePassed = cur_passed;
		post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_TRACKING_TIMEPASSED , &info , (void *)timestamp );
	}
}

VO_VOID detect_wholecontent_percentage( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item , VO_U64 timestamp )
{
	VO_U64 org_pos = timestamp - ptr_item->ullStartTime + ptr_item->ullOrgStartTime;

	if( ptr_info->last_org_time == UNAVALIABLETIME )
	{
		VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO info;
		info.uPercentage = 0;
		info.uPeriodID = ptr_item->uID;
		info.ullElapsedTime = timestamp - ptr_info->ping_time;
		post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_TRACKING_WHOLECONTENTPRECENTAGE , &info , (void *)timestamp );
		ptr_info->ping_time = timestamp;
	}
	else if( ptr_info->last_org_time < ptr_info->org_duration - 300 &&
		org_pos >= ptr_info->org_duration - 300 )
	{
		VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO info;
		info.uPercentage = 100;
		info.uPeriodID = ptr_item->uID;
		info.ullElapsedTime = timestamp - ptr_info->ping_time;
		post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_TRACKING_WHOLECONTENTPRECENTAGE , &info , (void *)timestamp );
		ptr_info->ping_time = timestamp;
	}
	else
	{
		VO_U32 pre_percent = ptr_info->last_org_time * 100 / ptr_info->org_duration;
		VO_U32 next_percent = ( pre_percent / 5 + 1 ) * 5;

		VO_U32 cur_precent = org_pos * 100 / ptr_info->org_duration;
		cur_precent = ( cur_precent / 5 ) * 5;

		if( cur_precent == next_percent )
		{
			VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO info;
			info.uPercentage = next_percent;
			info.uPeriodID = ptr_item->uID;
			info.ullElapsedTime = timestamp - ptr_info->ping_time;
			post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_TRACKING_WHOLECONTENTPRECENTAGE , &info , (void *)timestamp );
			ptr_info->ping_time = timestamp;
		}
	}

	ptr_info->last_org_time = org_pos;
}

VO_VOID append_verification( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_CHAR * ptr_url )
{
	VO_U64 utc = voOS_GetUTC() - 5 * 60;

	VO_CHAR temp[200];
	memset( temp , 0 , sizeof( temp ) );

	VO_BOOL is_contain_param = VO_FALSE;

	VO_CHAR * ptr_temp = strrchr( ptr_url , '/' );

	if( ptr_temp )
	{
		if( strchr( ptr_temp , '?' ) != NULL )
			is_contain_param = VO_TRUE;
	}

	if( is_contain_param )
		sprintf( temp , "&umsstime=%lld&umsttl=7200" , utc );
	else
		sprintf( temp , "?umsstime=%lld&umsttl=7200" , utc );

	strcat( ptr_url , temp );

	VO_CHAR key[20];
	memset( key , 0 , sizeof(key) );
	strcpy( key , "cbsiuvpoctaveum" );

	VO_CHAR digest[20];
	memset( digest , 0 , sizeof( digest ) );

	voHMAC_SHA1 hash;
	hash.HMAC_SHA1( ( BYTE * )ptr_url , strlen( ptr_url ) , ( BYTE * )key , strlen( key ) , ( BYTE * ) digest );

	VO_CHAR * ptr_hash = Base64Encode( digest , 20 );

	memset( temp , 0 , sizeof( temp ) );
	strcpy( temp , ptr_hash );

	delete []ptr_hash;

	urlencode( temp );

	strcat( ptr_url , "&umshash=" );
	strcat( ptr_url , temp );
}

VO_BOOL urlencode( VO_CHAR * ptr_url ) 
{
	VO_BOOL encodeurl = VO_FALSE;
	char *pstr = ptr_url;
	char *buf = (char *)malloc(strlen(pstr) * 3 + 1);
	char *pbuf = buf;
	while (*pstr) 
	{
		if ( isxnumalpha(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~') 
		{
			*pbuf++ = *pstr;
		}
		else if (*pstr == '%' && isxhex(*(pstr+1)) && isxhex(*(pstr+2)) ) 
		{
			//the url may contain url encoded char, %20 for example, we should not encode it again.
			*pbuf++ = *pstr++;
			*pbuf++ = *pstr++;
			*pbuf++ = *pstr;
		}
		else 
		{
			encodeurl = VO_TRUE;
			*pbuf++ = '%'; 
			*pbuf++ = toHex(*pstr >> 4); 
			*pbuf++ = toHex(*pstr & 15);
		}

		pstr++;
	}
	*pbuf = '\0';

	//if it is same as before encode
	if( !strcmp( ptr_url , buf) )
	{
		encodeurl = VO_FALSE;
	}

	memset( ptr_url , 0 , sizeof(ptr_url) );
	strcpy( ptr_url , buf );
	free(buf);

	return encodeurl;
}

/* Converts x to its hex character*/
unsigned char toHex(const unsigned char x) 
{
	static char desthex[] = "0123456789abcdef";
	return desthex[x & 15];
}

VO_BOOL isxnumalpha( char x )
{
	if(  ('0' <= x && x <= '9') ||//0-9
		('a' <= x && x <= 'z') ||//abc...xyz
		('A' <= x && x <= 'Z')   //ABC...XYZ
		)
	{
		return VO_TRUE;
	}
	else
	{
		return VO_FALSE;
	}
}

VO_BOOL isxhex( char x )
{
	if(  ('0' <= x && x <= '9') ||//0-9
		('a' <= x && x <= 'f') ||//abcdef
		('A' <= x && x <= 'F')   //ABCDEF
		)
	{
		return VO_TRUE;
	}
	else
	{
		return VO_FALSE;
	}
}

VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * get_periodbyid( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U32 id )
{
	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = ptr_info->ptr_internal_begin;

	while( ptr_item )
	{
		if( ptr_item->uID == id )
			return ptr_item;

		ptr_item = ptr_item->ptr_next;
	}

	return 0;
}

VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * get_periodbyplayingtime( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U64 playingtime )
{
	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = ptr_info->ptr_internal_begin;

	while( ptr_item )
	{
		if( ptr_item->ullStartTime <= playingtime && ptr_item->ullEndTime >= playingtime )
			return ptr_item;

		ptr_item = ptr_item->ptr_next;
	}

	return 0;
}

VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * get_periodbyorgtime( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U64 orgtime )
{
	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = ptr_info->ptr_internal_begin;

	while( ptr_item )
	{
		if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT && ptr_item->ullOrgStartTime <= orgtime && ptr_item->ullOrgEndTime >= orgtime )
			return ptr_item;

		ptr_item = ptr_item->ptr_next;
	}

	return 0;
}

VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * get_periodbyidealtime( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U64 idealtime )
{
	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = ptr_info->ptr_internal_begin;

	while( ptr_item )
	{
		if( ptr_item->ullIdealStartTime <= idealtime && ptr_item->ullIdealEndTime >= idealtime )
			return ptr_item;

		ptr_item = ptr_item->ptr_next;
	}

	return 0;
}

VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * get_periodbyidealtime_fuzzy( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U64 idealtime )
{
	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = ptr_info->ptr_internal_begin;

	while( ptr_item )
	{
		if( ptr_item->ullIdealStartTime <= idealtime && ptr_item->ullIdealEndTime - 1000 >= idealtime )
		{
			return ptr_item;
		}
		else if( idealtime >= ptr_item->ullIdealEndTime - ( ptr_item->ullIdealEndTime - ptr_item->ullIdealStartTime ) / 2 && idealtime <= ptr_item->ullIdealEndTime )
		{
			if( ptr_item->ptr_next )
				return ptr_item->ptr_next;
			else
				return ptr_item;
		}

		ptr_item = ptr_item->ptr_next;
	}

	return 0;
}

VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * get_periodbyperiodid( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U32 period_id )
{
	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = ptr_info->ptr_internal_begin;

	while( ptr_item )
	{
		if( ptr_item->uSequecnNumber == period_id )
		{
			return ptr_item;
		}

		ptr_item = ptr_item->ptr_next;
	}

	return 0;
}

VO_U32 open_internal( VO_ADSMANAGER_GENERALINFO * ptr_info  )
{
	VOLOGI("1");
	begin_message_thread( &( ptr_info->ptr_msg_thread ) , message_func , ptr_info );

	VO_PTR h_smil = 0;

#ifdef _VOLOG_INFO
	ptr_info->log_fp = fopen( "/mnt/sdcard/CBSi/log.txt" , "wb+" );

	if( ptr_info->log_fp )
	{
		const VO_CHAR * ptr_temp = "SMIL URL: ";
		fwrite( ptr_temp , strlen( ptr_temp ) , 1 , ptr_info->log_fp );
		fwrite( ptr_info->url , strlen( ptr_info->url ) , 1 , ptr_info->log_fp );

		ptr_temp = "\r\n--------->SMIL.xml\r\n\r\n";
		fwrite( ptr_temp , strlen( ptr_temp ) , 1 , ptr_info->log_fp );
	}
#endif

	VOLOGI("2");
	VO_U32 ret = vo_smil_open( &h_smil , ptr_info->ptr_workingpath );
	VOLOGI("3");

	if( ret != VO_RET_SOURCE2_OK )
		return ret;

	VO_CHAR * ptr_smil = 0;
	VO_U32 size = 0;

	VOLOGI("4");
	ret = vo_download_by_url( ptr_info->url , (VO_PBYTE *)&ptr_smil , &size , ptr_info->ptr_workingpath , &( ptr_info->is_stop ) );
	VOLOGI("5");

	if( ret != VO_RET_SOURCE2_OK || !ptr_smil )
	{
		if( ptr_smil )
			delete []ptr_smil;

		post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_ERROR_SMIL_DOWNLOADFAIL , 0 , 0 );

		return VO_RET_SOURCE2_FAIL;
	}

#ifdef _VOLOG_INFO
	FILE * fp = fopen( "/mnt/sdcard/CBSi/SMIL.xml" , "wb+" );

	if( fp )
	{
		fwrite( ptr_smil , size , 1 , fp );
		fclose( fp );
	}
#endif

	VO_ADSMANAGER_PLAYBACKINFOEX * ptr_playbackinfo = NULL;

	VOLOGI("6");
	ret = vo_smil_parse_smilinfo( h_smil , ptr_smil , size , &ptr_playbackinfo );
	VOLOGI("7");

	VOLOGI( "Parse Smil %d Period Count %d" , ret , ptr_playbackinfo->nCounts );

#ifdef _VOLOG_INFO
	if( ptr_info->log_fp )
	{
		VO_CHAR temp[255];
		memset( temp , 0 , 255 );

		int str_size = sprintf( temp , "Parse Smil %d Period Count %d\r\n" , ret , ptr_playbackinfo->nCounts );

		fwrite( temp , str_size , 0 , ptr_info->log_fp );
	}
#endif

	if( ret != VO_RET_SOURCE2_OK )
		post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_ERROR_SMIL_PARSEFAIL , 0 , 0 );

	if( ptr_playbackinfo->nCounts == 0 )
	{
		if( strstr( ptr_smil , "This content expired on" ) )
		{
			post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_ERROR_PID_EXPIRED , 0 , 0 );
		}
		else if( strstr( ptr_smil , "This content is not available in your location" ) )
		{
			post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_ERROR_GEOBLOCK , 0 , 0 );
		}

		delete []ptr_smil;
		return VO_RET_SOURCE2_FAIL;
	}

	delete []ptr_smil;

	VOLOGI("8");
	if( ret == VO_RET_SOURCE2_OK )
	{
		create_playbackinfo( ptr_info , ptr_playbackinfo );
		ptr_info->org_duration = ptr_info->ptr_internal_end->ullEndTime;
	}
	VOLOGI("9");

	vo_smil_close( h_smil );
	VOLOGI("10");

	if( ret != VO_RET_SOURCE2_OK )
		return ret;

	ret = convert_periodurl( ptr_info );

	VOLOGI("11");

	if( ret != VO_RET_SOURCE2_OK )
		return ret;

	/*VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_testitem = ptr_info->ptr_internal_begin;

	while( ptr_testitem )
	{
		printf( "%s\tID: %08x\tFrom: %lld\tTo:%lld\tcid: %s\tTitle: %s\n" , ptr_testitem->nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT ? "Content" : "ADs    " , ptr_testitem->uID ,  ptr_testitem->ullStartTime , ptr_testitem->ullEndTime , ptr_testitem->strContentID , ptr_testitem->strTitle );
		ptr_testitem = ptr_testitem->ptr_next;
	}*/


	merge_internallist( ptr_info );

	VOLOGI("12");

	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = ptr_info->ptr_internal_begin;

	VO_U32 count = 0;
	while( ptr_item )
	{
		ptr_item->uSequecnNumber = count;
		ptr_item->ullIdealStartTime = ptr_item->ullStartTime;
		ptr_item->ullIdealEndTime = ptr_item->ullEndTime;
		ptr_item->ullPeriodStartTsTimeStamp = UNAVALIABLETIME;
		ptr_item->uLastPercentage = 0xffffffff;

		if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT )
			ptr_info->last_period_content_id = ptr_item->uID;

#ifdef _VOLOG_INFO
		if( ptr_info->log_fp )
		{
			VO_CHAR temp[512];
			memset( temp , 0 , 512 );

			int str_size = sprintf( temp , "PeriodInfo: %s\t%d\t%d\t%lld\t%lld\r\n" , ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT ? "Content" : "ADs    " , ptr_item->uSequecnNumber , ptr_item->uID ,  ptr_item->ullStartTime , ptr_item->ullEndTime );

			fwrite( temp , str_size , 0 , ptr_info->log_fp );
		}
#endif

		count++;
		ptr_item = ptr_item->ptr_next;
	}

	VOLOGI("13");

	return VO_RET_SOURCE2_OK;
}

unsigned int begin_open_thread( VO_ADSMANAGER_GENERALINFO * ptr_info )
{
	unsigned int thread_id = 0;
	create_thread( &( ptr_info->open_thread_handle ) , &thread_id , open_threadfunc , ptr_info , 0 );

	return 0;
}

unsigned int end_open_thread( VO_ADSMANAGER_GENERALINFO * ptr_info )
{
	wait_thread_exit( ptr_info->open_thread_handle );
	ptr_info->open_thread_handle = 0;

	return 0;
}

unsigned int open_threadfunc( void * ptr_obj )
{
	VO_ADSMANAGER_GENERALINFO * ptr_info = ( VO_ADSMANAGER_GENERALINFO * )ptr_obj;

	VO_U32 ret = open_internal( ptr_info );

	post_message( ptr_info->ptr_msg_thread , VO_ADSMANAGER_EVENT_OPEN_COMPLETE , (void *)ret , 0 );

	return ret;
}

VO_VOID merge_internallist( VO_ADSMANAGER_GENERALINFO * ptr_info )
{
	VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item = ptr_info->ptr_internal_begin;

	while( ptr_item )
	{
		ptr_info->ptr_internal_end = ptr_item;

		if( ptr_item->ptr_next && ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT && ptr_item->ptr_next->nPeriodType == ptr_item->nPeriodType )
		{
			VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_next = ptr_item->ptr_next;

			ptr_item->ullEndTime = ptr_next->ullEndTime;
			ptr_item->ullOrgEndTime = ptr_next->ullOrgEndTime;
			ptr_item->ptr_next = ptr_next->ptr_next;

			if( ptr_next->ptr_next )
				ptr_next->ptr_next->ptr_pre = ptr_item;

			delete ptr_next;

			continue;
		}
		ptr_item = ptr_item->ptr_next;
	}
}

VO_VOID post_trackingevents( VO_ADSMANAGER_GENERALINFO * ptr_info , VO_U32 eventid , VO_U32 periodid , VO_ADSMANAGER_URLLIST * ptr_list , VO_U64 playingtime )
{
	VO_U32 count = 0;
	VO_ADSMANAGER_URLLIST * ptr_item = ptr_list;

	while( ptr_item )
	{
		count++;
		ptr_item = ptr_item->ptr_next;
	}

	VO_ADSMANAGER_TRACKINGEVENT_INFO * ptr_event = new VO_ADSMANAGER_TRACKINGEVENT_INFO;
	ptr_event->ppUrl = new VO_CHAR *[ count ];
	ptr_event->uUrlCount = count;
	ptr_event->uPeriodID = periodid;

	count = 0;
	while( ptr_list )
	{
		ptr_event->ppUrl[count] = new VO_CHAR[MAXURLLEN];
		strcpy( ptr_event->ppUrl[count] , ptr_list->url );
		ptr_list = ptr_list->ptr_next;
		count++;
	}

	post_message( ptr_info->ptr_msg_thread , eventid , ptr_event , (void *)playingtime );
}

VO_VOID destroy_urllist( VO_ADSMANAGER_URLLIST * ptr_list )
{
	while( ptr_list )
	{
		VO_ADSMANAGER_URLLIST * ptr_item = ptr_list;
		ptr_list = ptr_list->ptr_next;

		delete ptr_item;
	}
}

VO_BOOL is_nocontent_before( VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item )
{
	ptr_item = ptr_item->ptr_pre;
	while( ptr_item )
	{
		if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT )
			return VO_FALSE;
		ptr_item = ptr_item->ptr_pre;
	}

	return VO_TRUE;
}

VO_BOOL is_nocontent_behind( VO_ADSMANAGER_PLAYBACKPERIOD_INTERNAL * ptr_item )
{
	ptr_item = ptr_item->ptr_next;
	while( ptr_item )
	{
		if( ptr_item->nPeriodType == VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT )
			return VO_FALSE;
		ptr_item = ptr_item->ptr_next;
	}

	return VO_TRUE;
}

#ifdef _VONAMESPACE
}
#endif