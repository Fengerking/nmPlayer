/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#ifndef __voAdFormat_H__
#define __voAdFormat_H__

#include "voType.h"

typedef enum
{
	VOAdTimeType_Unknow = 0,
	VOAdTimeType_Time,///< ms
	VOAdTimeType_Percentage,///<a percentage n(¡°n¡± is a value from 0-100) 
	VOAdTimeType_StartEnd,///<0:indicate the start,1:indicate the end
	VOAdTimeType_Positon,///<an integer of 1 or greater and represents the ad break opportunity
	VOAdTimeType_Max
}VOAdTimeType;

typedef enum
{
	VOAdBreakType_Unknow = 0,
	VOAdBreakType_Linear = 0x01,
	VOAdBreakType_NonLinear = 0x02,
	VOAdBreakType_Companion = 0x04,
	VOAdBreakType_Skippable_Linear = 0x08,
	VOAdBreakType_Ad_Pods = 0x10
}VOAdBreakType;

typedef struct
{
	VOAdTimeType	TimeIndex;///<refer to VOVmapTimeType
	VO_S64			ullTime;///<value of time base on VOVmapTimeType
}VOAdTimeOffset;

typedef struct AdMediaFile
{
	VO_U32						uDeliveryType;///<0:progressive,1:streaming
	VO_PCHAR					strType;///< MIME type for the file container
	VO_U32						uWidth;
	VO_U32						uHeight;
	VO_PCHAR					strCodec;///<the codec used to encode the file which can take values as specified by RFC 4281
	VO_U32						uID;///<an identifier for the media file
	VO_U32						uBitrate;
	VO_U32						uMinBitrate;
	VO_U32						uMaxBitrate;
	VO_BOOL						bScalable;///<identifies whether the media file is meant to scale to larger dimensions
	VO_BOOL						bMaintainAspectRatio;///<a Boolean value that indicates whether aspect ratio for media file dimensions should be maintained when scaled to new dimensions
	VO_VOID*					pApiFramework;
	VO_PCHAR					strURI;///<the media file URI
	struct AdMediaFile*		pNext;
}VOAdMediaFile,*pVOAdMediaFile;

typedef struct AdClick
{
	VO_PCHAR			strID;
	VO_PCHAR			strURI;
	struct AdClick*		pNext;
}VOAdClick,*pVOAdClick;

typedef struct AdVideoClicks
{
	VO_U32					uClickThroughCount;
	VOAdClick*				pClickThrough;
	VO_U32					uClickTrackingCount;
	VOAdClick*				pClickTracking;
	VO_U32					uCustomClickCount;
	VOAdClick*				pCustomClick;

	struct AdVideoClicks*	pNext;
}VOAdVideoClicks,*pVOAdVideoClicks;

typedef struct AdParameter
{
	VO_BOOL						bXmlEncoded;
	struct AdParameter*			pNext;
}VOAdParameter,*pVOAdParameter;

typedef struct AdTrackingEvent
{
	VO_PCHAR					strEvent;
	VO_U32						uOffset;
	VO_PCHAR					strURI;
	struct AdTrackingEvent*		pNext;
}VOAdTrackingEvent,*pVOVastTrackingEvent;

typedef struct AdIcon
{
	VO_VOID*		pReserved;///<to be continued
	struct AdIcon*	pNext;
}VOAdIcon,*pVOAdIcon;

typedef struct
{
	VO_U64						ullDuration;///<ms
	VO_U32						uMediaFileCount;
	VOAdMediaFile*				pMediaFile;
	VO_U32						uVideoClicksCount;
	VOAdVideoClicks*			pVideoClicks;///< A Linear element may optionally contain a VideoClicks element
	VO_U32						uAdParameterCount;
	VOAdParameter*				pAdParameter;
	VO_U32						uTrackingEvenCount;
	VOAdTrackingEvent*			pTrackingEvents;
	VO_U32						uIconCount;
	VOAdIcon*					pIcons;
}VOAdLinear,*pVOVastLinear;

typedef struct
{
	VO_VOID*		pReserved;///<to be continued
}VOAdNonLinear,*pVOVastNonLinear;

typedef struct
{
	VO_VOID*		pReserved;///<to be continued
}VOAdCompanionAds,*pVOVastCompanionAds;

typedef struct AdCreative
{
	VO_PCHAR		strID;///<an ad server-defined identifier for the creative
	VO_U32			uSequence;///<the numerical order in which each sequenced creative should display 
	VO_PCHAR		strAdId;///<identifies the ad with which the creative is served
	VO_VOID*		pApiFramework;///<the technology used for any included API
	VOAdBreakType	CreativeType;
	union
	{
		VOAdLinear				Linear;
		VOAdNonLinear			NonLinear;
		VOAdCompanionAds		CompanionAds;
	};

	struct AdCreative*			pNext;
}VOAdCreative,*pVOAdCreative;

typedef struct VastImpression
{
	VO_PCHAR	strURI;///<a URI that directs the video player to a tracking resource file that the video player should request when the first frame of the ad is displayed
	struct VastImpression* pNext;
}VOAdImpression,*pVOAdImpression;

typedef struct
{
	VO_PCHAR			strAdSystem;///<the name of the ad server that returned the ad
	VO_PCHAR			strAdTitle;///<the common name of the ad
	VO_U32				uImpressionCount;
	pVOAdImpression		pImpression;			
	VO_PCHAR			strDescription;///<a string value that provides a longer description of the ad
	VO_PCHAR			strAdvertiser;///<the name of the advertiser as defined by the ad serving party
	VO_PCHAR			strSurvey;///<a URI to a survey vendor that could be the survey, a tracking pixel, or anything to do with the survey
	VO_PCHAR			strError;///<a URI representing an error-tracking pixel
	VO_VOID*			pExtensions;///<XML node for custom extensions
	VO_U32				uCreativeCount;
	VOAdCreative*		pCreative;
}VOAdInLine,*pVOAdInLine;

typedef struct AdData
{
	VO_PCHAR		strAdID;///<an ad server-defined identifier string for the ad
	VO_U32			uSequenceNum;///<a number greater than zero (0) that identifies the sequence in which an ad should play
	VOAdInLine*		pInLine;
	struct AdData*  pNext;
}VOAdData,*pVOAdData;

typedef struct
{
	VO_U32		uAdCount;
	pVOAdData	pVastAd;
}VOVastAdData,*pVOVastAdData;

typedef struct AdSource
{
	VO_U32				uID;
	VO_BOOL				bAllowMultipleAds;///< indicates whether a VAST ad pod or multiple buffet of ads can be served into an ad break
	VO_BOOL				bFollowRedirects;///< indicates whether the video player should honor redirects within an ad response
	VO_PCHAR			strAdTagURI;///< a URI that references an ad response from another system
	VO_PCHAR			strCustomAdData;///< Arbitrary string data that represents non-VAST ad response
	VO_U32				uAdDataCount;
	VOAdData*			pAdData;
}VOAdSource,*pVOAdSource;

typedef struct AdBreak
{
	VOAdTimeOffset		Time;
	VO_U64				ullDuration;///<ms
	VO_U32				BreakType;///<the composition of VOVmapBreakType
	VO_PCHAR			strBreakID;///< a string identifier for the ad break
	VO_U64				ullRepeatAfter;///indicates that the video player should repeat the same <AdBreak> break (using the same <AdSource>) at time offsets equal to the duration value of this attribute
	VOAdSource*			pAdSource;	
	struct AdBreak*		pNext;
}VOAdBreak,*pVOAdBreak;

typedef struct AdInfo
{
	VO_U32				uAdBreakCount;
	pVOAdBreak			pAdBreak;	
}VOAdInfo,*pVOAdInfo;



#endif // __voAdFormat_H__
