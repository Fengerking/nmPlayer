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


#ifndef __VOMTPSOURCE_CMMB_H__
#define __VOMTPSOURCE_CMMB_H__

#include "voLiveSource.h"
#include "voMTVBase.h"


#define VO_PID_LIVESRC_PROGRAMGUIDE	(VO_PID_LIVESRC_BASE_CMMB | 0x0001)		/*!< <G> Not used for now */
#define VO_PID_LIVESRC_EB				(VO_PID_LIVESRC_BASE_CMMB | 0x0002)		/*!< <G> Not used for now */
#define VO_PID_LIVESRC_CAID			(VO_PID_LIVESRC_BASE_CMMB | 0x0003)		/*!< <G> get the CA card ID, pValue is TCHAR pointer */
#define VO_PID_LIVESRC_KDAVERSION		(VO_PID_LIVESRC_BASE_CMMB | 0x0004)		/*!< <G> get the KDA version, pValue is TCHAR pointer */
#define	VO_PID_LIVESRC_CACARDNAME		(VO_PID_LIVESRC_BASE_CMMB | 0x0005)		/*!< <S> set the CA card device name, nValue is a TCHAR string pointer */
#define VO_PID_LIVESRC_CACARDREMOVED	(VO_PID_LIVESRC_BASE_CMMB | 0x0006)		/*!< <S> notify that CA card is removed */
#define VO_PID_LIVESRC_CACARDINSERTED	(VO_PID_LIVESRC_BASE_CMMB | 0x0007)		/*!< <S> notify that CA card is inserted */
#define VO_PID_LIVESRC_EBQUERY		(VO_PID_LIVESRC_BASE_CMMB | 0x0008)		/*!< <S> check EB status (only for two channel CMMB chips) */
#define VO_PID_LIVESRC_PROGRAMINFO	(VO_PID_LIVESRC_BASE_CMMB | 0x0009)		/*!< <G> get the program info array, pValue is pointer to VOENGPROGRAMEINFO_CMMB, return value is total program number */



#define VO_LIVESRC_STATUS_CA_ACCESS_CHANGE		(VO_LIVESRC_STATUS_BASE_CMMB | 0x0001)	 /*!< Abandoned */
#define VO_LIVESRC_STATUS_RECEIVING_ESG			(VO_LIVESRC_STATUS_BASE_CMMB | 0x0002)
#define VO_LIVESRC_STATUS_CMMB_CA_INVALIDCARD		(VO_LIVESRC_STATUS_BASE_CMMB | 0x0003)
#define VO_LIVESRC_STATUS_CMMB_CA_NOPERMIT		(VO_LIVESRC_STATUS_BASE_CMMB | 0x0004)
#define VO_LIVESRC_STATUS_EB_RECEIVED				(VO_LIVESRC_STATUS_BASE_CMMB | 0x0005)	 /*!< nParam1 is Update ID, nParam2 is struct pointer */
#define VO_LIVESRC_STATUS_PROGRAMGUIDE_RECEIVED	(VO_LIVESRC_STATUS_BASE_CMMB | 0x0006)	 /*!< nParam1 is Update ID, nParam2 is struct pointer */
#define VO_LIVESRC_STATUS_ESG_NEEDUPDATE			(VO_LIVESRC_STATUS_BASE_CMMB | 0x0007)	



/**
* Various maximum values
*/
#define	VO_LIVESRC_MAX_TITLE_LEN		128
#define	VO_LIVESRC_MAX_DIGEST_LEN		1024
#define	VO_LIVESRC_MAX_KEYWORD_LEN	256
#define	VO_LIVESRC_MAX_EB_LEN			4096



/**
* Language Type 
*/
typedef enum
{
	VO_LIVESRC_LANG_CHINESE			= 0,
	VO_LIVESRC_LANG_ENGLISH			= 1,
	VO_LIVESRC_LANG_MAX				= 10
}VO_LIVESRC_LANGTYPE;


/**
* CMMB Channel Extension Information
*/
typedef struct  
{
	VO_U32					nServiceID;
	VO_U32					nClass;
	VO_U32					nGenre;
	VO_U32					nParaID;
	VO_TCHAR				szName[VO_LIVESRC_MAX_TITLE_LEN];
	VO_BOOL					bForFree;
	VO_LIVESRC_LANGTYPE		nLang;
}VO_LIVESRC_CMMB_CHANNELINFOEX;





/**
* CMMB ESG Extension Information 
*/
typedef struct  
{
	VO_U32				nScheduleID;
	VO_U32				nServiceID;
	VO_U32				nContentID;
	VO_TCHAR			szTitle[VO_LIVESRC_MAX_TITLE_LEN];
	VO_LIVESRC_LANGTYPE		nLang;
	VODATETIME			nStartTime;
	VO_BOOL				bForFree;
	VO_BOOL				bLive;
	VO_BOOL				bRepeat;
}VO_LIVESRC_CMMB_ESGINFOEX;

/**
* CMMB Program Information
*/
typedef struct  
{
	VO_U32				nContentID;
	VO_U32				nClass;
	VO_U32				nGenre;
	VO_U32				nDuration;
	VO_TCHAR			szTitle[VO_LIVESRC_MAX_TITLE_LEN];
	VO_TCHAR			szDigestInfo[VO_LIVESRC_MAX_DIGEST_LEN];
	VO_TCHAR			szKeyword[VO_LIVESRC_MAX_KEYWORD_LEN];
	VO_LIVESRC_LANGTYPE	nAudioLang;
	VO_LIVESRC_LANGTYPE	nSubtitleLang;
	VO_LIVESRC_LANGTYPE	nLang;
	VO_U32				nServiceParaID;
	VO_TCHAR			szExtentInfo[128];
}VO_LIVESRC_CMMB_PROGRAMEINFO;

/**
* CMMB EB Information
*/
typedef struct
{
	VO_U32			nType;
	VO_U32			nLevel;
	VO_TCHAR 		szText[VO_LIVESRC_MAX_EB_LEN];
}VO_LIVESRC_CMMB_EBINFO;


/**
* DVB-T scan item
*/
typedef struct
{
	VO_U32 			nFrequency; /*!< in kHz */
	VO_U32 			nBandwidth; /*!< in kHz */
}VO_LIVESRC_DVBT_SCANITEM;

/**
* DVB-T scan parameters
*/
typedef struct
{
	VO_U32 					nCount;
	VO_LIVESRC_DVBT_SCANITEM *pItems;
}VO_LIVESRC_DVBT_SCANPARAM;

/**
* T-DMB scan item parameter
*/
typedef struct
{
	VO_U32 			nFrequency; /*!< in kHz */
}VO_LIVESRC_TDMB_SCANITEM;

/**
* T-DMB scan parameters
*/
typedef struct
{
	VO_U32 			nCount;
	VO_LIVESRC_TDMB_SCANITEM *pItems;
}VO_LIVESRC_TDMB_SCANPARAM;

/**
* ISDB-T scan parameters
*/
typedef struct
{
	VO_U32 			nStartFrequency;
	VO_U32 			nEndFrequency;
}VO_LIVESRC_ISDBT_SCANPARAM;

/**
* DVB-H scan parameters
*/
typedef struct
{
	VO_U32 			nSize;
	VO_PBYTE		pBuffer;	/* sdp content */
}VO_LIVESRC_DVBH_SCANPARAM;

/**
* File scan parameters
*/
typedef struct
{
	VO_TCHAR *			pFolder;
	VO_TCHAR *			pExt;
	VO_U32				nBitrate;
	VO_U32				nPackSize;
}VO_LIVESRC_FILE_SCANPARAM;

/**
* CMMB scan parameters
*/
typedef struct
{
	VO_U32				nStartFreq;
	VO_U32				nEndFreq;
}VO_LIVESRC_CMMB_SCANPARAM;

typedef struct
{
	VO_U32			nChannelID;     /*!< the channel ID */
	VO_U32			nIndex;			/*!< the index content */
	VO_TCHAR		szName[256];	/*!< the content Name */
	VODATETIME		nStartTime;
	VO_U32			nDuration;
	VO_PTR			pData;			/*!< if CMMB, pointer to VO_LIVESRC_ESGINFOEX_CMMB struct*/
}VO_CMMB_ESG_ITEM;

/**
* ESG Information
*/
typedef struct
{
	int nCount;
	VO_CMMB_ESG_ITEM* pItem;
}VO_CMMB_ESGINFO;

VO_S32 VO_API voGetCMMBSrcAPI(VO_LIVESRC_API* pHandle);


#endif // __VOMTPSOURCE_CMMB_H__