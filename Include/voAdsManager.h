/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#ifndef __VOADSMANAGER_H__

#define __VOADSMANAGER_H__

#include "voSource2.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VO_ADSMANAGER_OPENFLAG_PID					0x00000001	//Indicate the pSource will be VO_ADSMANAGER_SOURCE_PID* , nSize will be useless
#define VO_ADSMANAGER_OPENFLAG_ASYNCOPEN			0x00001000	//Indicate open should be async mode
#define VO_ADSMANAGER_OPENFLAG_DEBUG				0x80000000	//Indicate it is debug mode, we shoule use debug server

#define VO_ADSMANAGER_PID_EVENTCALLBACK				0x1230adcb	//Indicate that the Param will be VO_ADSMANAGER_EVENTCALLBACK* 
#define VO_ADSMANAGER_PID_DISCONTINUECHUNK			0x1230adcc	//Indicate that the Param will be VO_ADSMANAGER_CHUNKSAMPLEINFO*
#define VO_ADSMANAGER_PID_DISCONTINUESAMPLE			0x1230adcd	//Indicate that the Param will be VO_ADSMANAGER_CHUNKSAMPLEINFO*
#define VO_ADSMANAGER_PID_STARTSEEKADS				0x1230adce
#define VO_ADSMANAGER_PID_STARTSEEKCONTENT			0x1230adcf

#define VO_ADSMANAGER_EVENT_BASE					0xecb00000
#define VO_ADSMANAGER_EVENT_CONTENT_START			( VO_ADSMANAGER_EVENT_BASE | 0x00000001 )				//nParam1 is VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO*
#define VO_ADSMANAGER_EVENT_CONTENT_END				( VO_ADSMANAGER_EVENT_BASE | 0x00000002 )				//nParam1 is VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO*
#define VO_ADSMANAGER_EVENT_AD_START				( VO_ADSMANAGER_EVENT_BASE | 0x00000003 )				//nParam1 is VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO*
#define VO_ADSMANAGER_EVENT_AD_END					( VO_ADSMANAGER_EVENT_BASE | 0x00000004 )				//nParam1 is VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO*
#define VO_ADSMANAGER_EVENT_OPEN_COMPLETE			( VO_ADSMANAGER_EVENT_BASE | 0x00000005 )				//nParam1 is Open return value VO_U32 */
#define VO_ADSMANAGER_EVENT_WHOLECONTENT_START		( VO_ADSMANAGER_EVENT_BASE | 0x00000006 )				//nParam1 is VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO*
#define VO_ADSMANAGER_EVENT_WHOLECONTENT_END		( VO_ADSMANAGER_EVENT_BASE | 0x00000007 )				//nParam1 is VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO*

#define VO_ADSMANAGER_EVENT_TRACKING_BASE						( VO_ADSMANAGER_EVENT_BASE | 0x00010000 )
//#define VO_ADSMANAGER_EVENT_TRACKING_PRECENTAGE				( VO_ADSMANAGER_EVENT_TRACKING_BASE | 0x00000001 )		//this event show there is a percentage event, the nParam1 is VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO*
#define VO_ADSMANAGER_EVENT_TRACKING_ACTION						( VO_ADSMANAGER_EVENT_TRACKING_BASE | 0x00000002 )		//this event show there is an action event, the nParam1 is VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO*
#define VO_ADSMANAGER_EVENT_TRACKING_PERIODPRECENTAGE			( VO_ADSMANAGER_EVENT_TRACKING_BASE | 0x00000003 )		//this event show there is a period percentage event, the nParam1 is VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO*
#define VO_ADSMANAGER_EVENT_TRACKING_WHOLECONTENTPRECENTAGE		( VO_ADSMANAGER_EVENT_TRACKING_BASE | 0x00000004 )		//this event show there is a percentage event, the nParam1 is VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO*
#define VO_ADSMANAGER_EVENT_TRACKING_TIMEPASSED					( VO_ADSMANAGER_EVENT_TRACKING_BASE | 0x00000005 )		//this event show how much time passed since last same event, the nParam1 is VO_ADSMANAGER_EVENT_TRAKCING_TIMEPASSEDINFO*

#define VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE				( VO_ADSMANAGER_EVENT_TRACKING_BASE | 0x00001000 )
#define VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_AD_START			( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x00000001 )		//this event show there is an ad start event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
#define VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_FIRST_QUARTILE		( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x00000002 )		//this event show there is a first quartile event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
#define VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_MID_POINT			( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x00000003 )		//this event show there is a mid point event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
#define VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_THIRD_QUARTILE		( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x00000004 )		//this event show there is a third quartile event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
#define VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_COMPLETE			( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x00000005 )		//this event show there is a complete event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
#define VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_CREATIVE_VIEW		( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x00000006 )		//this event show there is a creative view event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
#define VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_MUTE				( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x00000007 )		//this event show there is a mute event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
#define VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_UNMUTE				( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x00000008 )		//this event show there is a unmute event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
#define VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_PAUSE				( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x00000009 )		//this event show there is a pause event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
#define VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_RESUME				( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x0000000a )		//this event show there is a resume event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
#define VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_FULLSCREEN			( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x0000000b )		//this event show there is a fullscreen event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
#define VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_IMPRESSION			( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x0000000c )		//this event show there is an impression event based on the vast , the nParam1 is VO_ADSMANAGER_TRACKINGEVENT_INFO*
#define VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_MAX					( VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE | 0x00000fff )

#define VO_ADSMANAGER_EVENT_ERROR_BASE				( VO_ADSMANAGER_EVENT_BASE | 0x000e0000 )				
#define VO_ADSMANAGER_EVENT_ERROR_GEOBLOCK			( VO_ADSMANAGER_EVENT_ERROR_BASE | 0x00000001 )			//indicate the PID is GEO blocked
#define VO_ADSMANAGER_EVENT_ERROR_PID_INVALID		( VO_ADSMANAGER_EVENT_ERROR_BASE | 0x00000002 )			//indicate the PID is invalid, it can not be downloaded
#define VO_ADSMANAGER_EVENT_ERROR_PID_EXPIRED		( VO_ADSMANAGER_EVENT_ERROR_BASE | 0x00000003 )			//indicate the PID has expired
#define VO_ADSMANAGER_EVENT_ERROR_PID_NOTAVALIBLE	( VO_ADSMANAGER_EVENT_ERROR_BASE | 0x00000004 )			//indicate the PID is not avalible right now, it will be avalible in the future
#define VO_ADSMANAGER_EVENT_ERROR_SMIL_DOWNLOADFAIL	( VO_ADSMANAGER_EVENT_ERROR_BASE | 0x00000005 )			//indicate the smil download fail
#define VO_ADSMANAGER_EVENT_ERROR_SMIL_PARSEFAIL	( VO_ADSMANAGER_EVENT_ERROR_BASE | 0x00000006 )			//indicate the smil parse fail
#define VO_ADSMANAGER_EVENT_ERROR_VMAP_AUTHFAIL		( VO_ADSMANAGER_EVENT_ERROR_BASE | 0x00000007 )			//indicate the vmap authentication fail
#define VO_ADSMANAGER_EVENT_ERROR_VMAP_DOWNLOADFAIL	( VO_ADSMANAGER_EVENT_ERROR_BASE | 0x00000008 )			//indicate the vmap download fail
#define VO_ADSMANAGER_EVENT_ERROR_VMAP_PARSEERROR	( VO_ADSMANAGER_EVENT_ERROR_BASE | 0x00000009 )			//indicate the vmap parse fail

enum VO_ADSMANAGER_PERIODTYPE
{
	VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT = 0,
	VO_ADSMANAGER_PERIODTYPE_ADS =1,
};

enum VO_ADSMANAGER_ACTION
{
	VO_ADSMANAGER_ACTION_CLICK = 0,
	VO_ADSMANAGER_ACTION_PLAYBACKSTART = 1,
	VO_ADSMANAGER_ACTION_PLAYBACKCOMPLETE = 2,
	VO_ADSMANAGER_ACTION_PAUSE = 3,
	VO_ADSMANAGER_ACTION_SEEK = 4,
	VO_ADSMANAGER_ACTION_DRAGSTART = 5,
	VO_ADSMANAGER_ACTION_RESUME = 6,
	VO_ADSMANAGER_ACTION_FORCESTOP = 7,
	VO_ADSMANAGER_ACTION_FULLSCREENON = 8,
	VO_ADSMANAGER_ACTION_FULLSCREENOFF = 9,
	VO_ADSMANAGER_ACTION_SUBTITLEON = 10,
	VO_ADSMANAGER_ACTION_SUBTITLEOFF = 11,
	VO_ADSMANAGER_ACTION_MUTEON = 12,
	VO_ADSMANAGER_ACTION_MUTEOFF = 13,

	VO_ADSMANAGER_ACTION_MAX = VO_MAX_ENUM_VALUE,
};

enum VO_ADSMANAGER_DEVICETYPE
{
	VO_ADSMANAGER_DEVICETYPE_NONE = 0,
	VO_ADSMANAGER_DEVICETYPE_PHONE = 1,
	VO_ADSMANAGER_DEVICETYPE_TABLET = 2,
};

typedef struct  
{
	VO_U32 uFlag;
	VO_U64 ullChunkStartTime;
	VO_U32	uPeriodSequenceNumber;
	VO_U64 ullPeriodFirstChunkStartTime;
	VO_U64 * pullSampleTime;
}VO_ADSMANAGER_CHUNKSAMPLEINFO;

typedef struct 
{
	VO_U32 uID;
	VO_ADSMANAGER_PERIODTYPE nPeriodType;	//the period type
	VO_CHAR strPeriodURL[MAXURLLEN];		//the period URL
	VO_U64 ullStartTime;					//the period start time
	VO_U64 ullEndTime;						//the period end time
	VO_CHAR strCaptionURL[MAXURLLEN];		//the caption/subtitle URL
	VO_CHAR strPeriodTitle[MAXURLLEN];
	VO_CHAR strPeriodID[64];
	VO_BOOL isLive;
	VO_BOOL isEpisode;
	
	VO_PTR pReserved1;
	VO_PTR pReserved2;
}VO_ADSMANAGER_PLAYBACKPERIOD;

typedef struct 
{
	VO_U32 nCounts;								//Indicate the counts of periods
	VO_ADSMANAGER_PLAYBACKPERIOD *pPeriods;		//Period list
}VO_ADSMANAGER_PLAYBACKINFO;

typedef struct  
{
	VO_TCHAR * ptr_workingpath;
	VO_ADSMANAGER_DEVICETYPE device_type;
}VO_ADSMANAGER_INITPARAM;

typedef struct 
{
	VO_CHAR * pPID;
	VO_CHAR * pPartnerString;
}VO_ADSMANAGER_SOURCE_PID;

typedef struct  
{
//callback instance
	VO_PTR pUserData;
/**
 * The source will notify client via this function for some events.
 * \param pUserData [in] The user data which was set by Open().
 * \param nID [in] The status type.
 * \param nParam1 [in] status specific parameter 1.
 * \param nParam2 [in] status specific parameter 2.
 */
	VO_S32 (VO_API * SendEvent) (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);
}VO_ADSMANAGER_EVENTCALLBACK;

typedef struct
{
	VO_U32 uPercentage;
	VO_U32 uPeriodID;
	VO_U64 ullElapsedTime;
}VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO;

typedef struct  
{
	VO_ADSMANAGER_ACTION nAction;
	VO_U32 uPeriodID;
	VO_U64 ullElapsedTime;
}VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO;

typedef struct  
{
	VO_U32 uPeriodID;
	VO_U64 ullTimePassed;
}VO_ADSMANAGER_EVENT_TRAKCING_TIMEPASSEDINFO;

typedef struct  
{
	VO_U32 uAdsSequnceNumber;
	VO_U64 ullContentSeekPos;
}VO_ADSMANAGER_SEEKINFO;

typedef struct  
{
	VO_U32 uPeriodID;
	VO_U32 uUrlCount;
	VO_CHAR ** ppUrl;
}VO_ADSMANAGER_TRACKINGEVENT_INFO;

typedef struct
{

/**
 * Init the session
 * this function will only check the param legal and store tha param for later use, it will never block
 * \param phHandle [out] handle.
 * \param pSource [in] input param, it depends on the nFlag.
 * \param nSize [in] the size of pSource.
 * \param nFlag [in] this flag indicate the type of the pSource.
 */
VO_U32 (VO_API * Init) ( VO_HANDLE * phHandle , VO_PTR pSource , VO_U32 nSize , VO_U32 nFlag , VO_ADSMANAGER_INITPARAM * ptr_param );

/**
 * Uninit the session
 * \param hHandle [in] handle.
 */
VO_U32 (VO_API * Uninit) ( VO_HANDLE hHandle );

/**
 * Open the session
 * \param hHandle [out] handle.
 */
VO_U32 (VO_API * Open) ( VO_HANDLE hHandle );

/**
 * Close the session
 * \param hHandle [in] handle.
 */
VO_U32 (VO_API * Close) ( VO_HANDLE hHandle );


/**
 * Get the playback period info
 * \param hHandle [in] handle.
 * \param ppInfo [out] it will contains all the playback period info, the memory will be maintained in AdsManager side, it will not be destroyed until next GetPlaybackInfo call
 */
VO_U32 (VO_API * GetPlaybackInfo) ( VO_HANDLE hHandle , VO_ADSMANAGER_PLAYBACKINFO ** ppInfo );

/**
 * Convert playing time to original time without ads
 * \param hHandle [in] handle.
 * \param ptr_orgtime [out] it will be the converted result, it is the org time without ads
 * \param ptr_playingtime [in] it is current playing time
 * \return value: 
 * \				VO_RET_SOURCE2_OK				means ok
 * \				VO_RET_SOURCE2_INVALIDPARAM		means the playingtime belongs to ads, so it can not be converted
 * \				others							means so special errors
 */
VO_U32 (VO_API * ConvertTimeStamp) ( VO_HANDLE hHandle , VO_U64 * pOrgTime , VO_U64 ullPlayingTime );

/**
 * Set current playing time
 * \param hHandle [in] handle.
 * \param ullPlayingTime [in] current playing time
 */
VO_U32 (VO_API * SetPlayingTime) ( VO_HANDLE hHandle , VO_U64 ullPlayingTime );


/**
 * Set action in the UI
 * \param hHandle [in] handle.
 * \param nAction [in] current action
 * \param ullPlayingTime [in] current playing time
 */
VO_U32 (VO_API * SetAction) ( VO_HANDLE hHandle , VO_ADSMANAGER_ACTION nAction , VO_U64 ullPlayingTime );

/**
 * Set action in the UI, it is sync mode
 * \param hHandle [in] handle.
 * \param nAction [in] current action
 * \param ullPlayingTime [in] current playing time
 * \param pParam [out] the return param of the action, it depends on the action
 *
 *	if nAction is VO_ADSMANAGER_ACTION_CLICK, pParam should be VO_CHAR* and the memory should be created by caller
 */
VO_U32 (VO_API * SetActionSync) ( VO_HANDLE hHandle , VO_ADSMANAGER_ACTION nAction , VO_U64 ullPlayingTime , VO_VOID * pParam );

/**
 * Get content duration
 * \param hHandle [in] handle.
 * \param pullDuration [out] content duration
 */
VO_U32 (VO_API * GetContentDuration) ( VO_HANDLE hHandle , VO_U64 * pullDuration );

/**
 * Check if we can seek at current playing time
 * \param hHandle [in] handle.
 * \param ullPlayingTime [in] this is the playing time from the player
 */
VO_BOOL (VO_API * IsSeekable) ( VO_HANDLE hHandle , VO_U64 ullPlayingTime );

/**
 * Get seek information
 * \param hHandle [in] handle.
 * \param ullPlayingTime [in] this is the playing time from the player
 * \param ullSeekPos[in] this is the pos that user wants to seek
 * \param pInfo[out] ad pos and seek pos
 */
VO_U32 (VO_API * Seek) ( VO_HANDLE hHandle , VO_U64 ullPlayingtTime , VO_U64 ullSeekPos , VO_ADSMANAGER_SEEKINFO * pInfo );

/**
 * Get the special value from param ID
 * \param hHandle [in] handle.
 * \param nParamID [in] The param ID
 * \
 */
VO_U32 (VO_API * GetParam) (VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam);

/**
 * Set the special value from param ID
 * \param hHandle [in] handle.
 * \param nParamID [in] The param ID
 * \param pParam [in] The set value depend on the param ID.
 */
VO_U32 (VO_API * SetParam) (VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam);

} VO_ADSMANAGER_API;

VO_S32 VO_API voGetAdsManagerAPI(VO_ADSMANAGER_API * pAPI);



//for internal use only


typedef struct 
{
	VO_U32 uID;
	VO_ADSMANAGER_PERIODTYPE nPeriodType;	//the period type
	VO_CHAR strPeriodURL[MAXURLLEN];		//the period URL
	VO_U64 ullStartTime;					//the period start time
	VO_U64 ullEndTime;						//the period end time
	VO_CHAR strCaptionURL[MAXURLLEN];		//the caption/subtitle URL
	VO_CHAR strGUID[64];
	VO_CHAR strPeriodTitle[MAXURLLEN];
	VO_CHAR strContentID[64];
	VO_BOOL isLive;
	VO_BOOL isEpisode;
	VO_PTR pReserved1;
	VO_PTR pReserved2;
}VO_ADSMANAGER_PLAYBACKPERIODEX;


typedef struct 
{
	VO_U32 nCounts;								//Indicate the counts of periods
	VO_ADSMANAGER_PLAYBACKPERIODEX *pPeriods;		//Period list
}VO_ADSMANAGER_PLAYBACKINFOEX;


//

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif