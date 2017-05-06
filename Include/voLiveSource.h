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


#ifndef __VOLIVESRC_H__
#define __VOLIVESRC_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voString.h"
#include "voSource.h"
#include "vompType.h"
#include "voAudio.h"
#include "voVideo.h"

/**
* Parameter ID
* S - set; G - get; S/G - both set and get.
*/
#define	VO_PID_LIVESRC_BASE				0x4A000000									/*!< the base param ID for source modules */
#define	VO_PID_LIVESRC_SIGNAL			(VO_PID_LIVESRC_BASE | 0x0001)				/*!< <G> get the signal strength. The value is 0 - 100 */
#define VO_PID_LIVESRC_TRACKINFO		(VO_PID_LIVESRC_BASE | 0x0002)				/*!< <G> get the track information and codec type, see VO_LIVESRC_TRACK_INFO */
#define VO_PID_LIVESRC_STARTRECORD		(VO_PID_LIVESRC_BASE | 0x0003)				/*!< <S> start stream raw data recording */
#define VO_PID_LIVESRC_ENDRECORD		(VO_PID_LIVESRC_BASE | 0x0004)				/*!< <S> end stream raw data recording */
#define VO_PID_LIVESRC_CHDATASAVEPATH	(VO_PID_LIVESRC_BASE | 0x0005)				/*!< <S> set the file path (w/ name) to save channel info, return channel number, Should be called immediately after Open */
#define VO_PID_LIVESRC_SOURCE_TYPE		(VO_PID_LIVESRC_BASE | 0x0006)				/*!< <S> get the source type. int * 0, Live, 1 File */
#define VO_PID_LIVESRC_DURATION			(VO_PID_LIVESRC_BASE | 0x0007)				/*!< <S> get the source duration int * */
#define VO_PID_LIVESRC_VIDEODELAY		(VO_PID_LIVESRC_BASE | 0x0008)				/*!< <S> set the video delay time the param will be int * to indicate the delay time in ms */
#define VO_PID_LIVESRC_AUDIO_FOURCC		(VO_PID_LIVESRC_BASE | 0x0009)				/*!< <G> get the audio track FOURCC. unsigned int* */
#define VO_PID_LIVESRC_VIDEO_FOURCC		(VO_PID_LIVESRC_BASE | 0x000a)				/*!< <G> get the video track FOURCC. unsigned int* */
#define VO_PID_LIVESRC_DRMCALLBACK		(VO_PID_LIVESRC_BASE | 0x000b)				/*!< <S> set the DRM callback after open and before set url, VO_SOURCEDRM_CALLBACK* */
#define VO_PID_LIVESRC_POSITION			(VO_PID_LIVESRC_BASE | 0x000c)				/*!< <S> set the source position, int* in second */
#define VO_PID_LIVESRC_TRACKNUMBER      (VO_PID_LIVESRC_BASE | 0x000e)              /*!< <G> get the track number of this file, parem is int* */
#define VO_PID_LIVESRC_LIBOP			(VO_PID_LIVESRC_BASE | 0x000f)              /*!< <G> Set the lib operator pointer, parem is VO_LIB_OPERATOR* */
#define VO_PID_LIVESRC_WORKPATH         (VO_PID_LIVESRC_BASE | 0x0010)              /*!< <S> set the work path char* */
#define VO_PID_LIVESRC_CPU_ABILITY      (VO_PID_LIVESRC_BASE | 0x0011)              /*!< <S> set the cpu work mode, single core or dual core* */
#define VO_PID_LIVESRC_CAP_PROFILE      (VO_PID_LIVESRC_BASE | 0x0012)              /*!< <S> set the bitrate cap and the h264 profile* */
#define VO_PID_LIVESRC_SAMPLE_DATA      (VO_PID_LIVESRC_BASE | 0x0013)
#define VO_PID_LIVESRC_USERNAME         (VO_PID_LIVESRC_BASE | 0x0014)              /*!< <S> set the user name for hls* */
#define VO_PID_LIVESRC_PASSWD           (VO_PID_LIVESRC_BASE | 0x0015)              /*!< <S> set the passwd name for hls* */


#define VO_PID_LIVESRC_BASE_CMMB		(VO_PID_LIVESRC_BASE | 0x0100)				/*!< the base param ID for CMMB modules */
#define VO_PID_LIVESRC_BASE_DVBH		(VO_PID_LIVESRC_BASE | 0x0200)				/*!< the base param ID for DVBH modules */
#define VO_PID_LIVESRC_BASE_DVBT		(VO_PID_LIVESRC_BASE | 0x0300)				/*!< the base param ID for DVBT modules */
#define VO_PID_LIVESRC_BASE_ATSCMH		(VO_PID_LIVESRC_BASE | 0x0400)				/*!< the base param ID for ATSC-MH modules */
#define VO_PID_LIVESRC_BASE_ISDBT		(VO_PID_LIVESRC_BASE | 0x0500)				/*!< the base param ID for ISDBT modules */
#define VO_PID_LIVESRC_BASE_TDMB		(VO_PID_LIVESRC_BASE | 0x0600)				/*!< the base param ID for TDMB modules */
#define VO_PID_LIVESRC_BASE_DAB			(VO_PID_LIVESRC_BASE | 0x0700)				/*!< the base param ID for DAB modules */
#define VO_PID_LIVESRC_BASE_DMBT		(VO_PID_LIVESRC_BASE | 0x0800)				/*!< the base param ID for DMBT modules */
#define VO_PID_LIVESRC_BASE_HTTP_LS		(VO_PID_LIVESRC_BASE | 0x0900)				/*!< the base param ID for Http Live Streaming modules */
#define VO_PID_LIVESRC_BASE_IIS_SS		(VO_PID_LIVESRC_BASE | 0x0A00)				/*!< the base param ID for IIS Smooth Streaming modules */

#define VO_PID_LIVESRC_HLS_DRM			(VO_PID_LIVESRC_BASE_HTTP_LS | 0x0001)		/*!< Set the DRM pointer */
#define VO_PID_LIVESRC_HLS_AD_FILTER		(VO_PID_LIVESRC_BASE_HTTP_LS | 0x0002)		/*!< Set the Advertisement pointer */


/**
* Error code
*/
#define VO_ERR_LIVESRC_OK				VO_ERR_NONE
#define VO_ERR_LIVESRC_BASE				0x80000000
#define VO_ERR_LIVESRC_FAIL				VO_ERR_LIVESRC_BASE
#define VO_ERR_LIVESRC_OUTOFMEMORY		(VO_ERR_LIVESRC_BASE | 0x0001)		/*!< out of memory */
#define VO_ERR_LIVESRC_NULLPOINTER		(VO_ERR_LIVESRC_BASE | 0x0002)		/*!< null pointer */
#define VO_ERR_LIVESRC_INVALIDARG		(VO_ERR_LIVESRC_BASE | 0x0003)		/*!< invalid argument */
#define VO_ERR_LIVESRC_LOADDRIVERFAILED	(VO_ERR_LIVESRC_BASE | 0x0004)		/*!< load driver failed */
#define VO_ERR_LIVESRC_NO_CHANNEL		(VO_ERR_LIVESRC_BASE | 0x0005)		/*!< no channel */
#define VO_ERR_LIVESRC_NO_DATA			(VO_ERR_LIVESRC_BASE | 0x0006)		/*!< no data */
#define VO_ERR_LIVESRC_NOIMPLEMENT		(VO_ERR_LIVESRC_BASE | 0x0007)		/*!< feature not implemented */
#define VO_ERR_LIVESRC_WRONG_STATUS		(VO_ERR_LIVESRC_BASE | 0x0008)		/*!< the status was wrong */


/**
* Status update code
*/
#define VO_LIVESRC_STATUS_BASE					0x41000000							/*!< the base status code for source modules */
#define	VO_LIVESRC_STATUS_SCAN_PROCESS			(VO_LIVESRC_STATUS_BASE | 0x0001)	/*!< nParam1 is scanned frequence percent, 0 - 100 */
#define	VO_LIVESRC_STATUS_CHANNEL_STOP			(VO_LIVESRC_STATUS_BASE | 0x0002)	/*!< nParam1 is old type, nParam2 is new type */
#define	VO_LIVESRC_STATUS_CHANNEL_START			(VO_LIVESRC_STATUS_BASE | 0x0003)	/*!< nParam1 is old type, nParam2 is new type */
#define VO_LIVESRC_STATUS_DEVICE_NOTREADY		(VO_LIVESRC_STATUS_BASE | 0x0004)	/*!< device is not ready to open */
#define VO_LIVESRC_STATUS_DEVICE_ERROR			(VO_LIVESRC_STATUS_BASE | 0x0005)	/*!< device error */
#define VO_LIVESRC_STATUS_NO_SIGNAL				(VO_LIVESRC_STATUS_BASE | 0x0006)	/*!< no signal */ 
#define VO_LIVESRC_STATUS_DEVICE_INIT_OK		(VO_LIVESRC_STATUS_BASE | 0x0007)	/*!< init device successfully */
#define VO_LIVESRC_STATUS_CODEC_CHANGED			(VO_LIVESRC_STATUS_BASE | 0x0008)	/*!< track codec changed , if nParam1 = 0 it is audio, 1 is video, nParam2 is VOMP_AUDIO_CODINGTYPE or VOMP_VIDEO_CODINGTYPE defined in vompType.h  */
#define VO_LIVESRC_STATUS_BITRATE_CHANGE		(VO_LIVESRC_STATUS_BASE | 0x0009)	/*!< track bitrate changed */
#define VO_LIVESRC_STATUS_NEED_FLUSH			(VO_LIVESRC_STATUS_BASE | 0x000A)	/*!< request media framework flush */
#define VO_LIVESRC_STATUS_MEDIATYPE_CHANGE		(VO_LIVESRC_STATUS_BASE | 0x000B)	/*!< mediatype update, just use nParam1 as VO_LIVESRC_MEDIA_TYPE*/
#define VO_LIVESRC_STATUS_DRM_ERROR				(VO_LIVESRC_STATUS_BASE | 0x000C)	/*!< DRM error callback, nParam1 is the DRM error code*/
#define VO_LIVESRC_STATUS_NETWORKBROKEN         (VO_LIVESRC_STATUS_BASE | 0x000D)   /*!< Network broken issue */
#define VO_LIVESRC_STATUS_HSL_AD_APPLICATION    (VO_LIVESRC_STATUS_BASE | 0x000E)   /*!< For Advertisement Application */
#define VO_LIVESRC_STATUS_HSL_FRAME_SCRUB       (VO_LIVESRC_STATUS_BASE | 0x000F)   /*!< For Frame Scrubbing Application */
#define VO_LIVESRC_STATUS_HSL_CHANGE_BITRATE    (VO_LIVESRC_STATUS_BASE | 0x0010)   /*!< Change the Bitrate Application */
#define VO_LIVESRC_STATUS_HSL_EVENT_THUMBNAIL   (VO_LIVESRC_STATUS_BASE | 0x0011)   /*!< Event Thumbnail */



#define VO_LIVESRC_STATUS_BASE_CMMB				(VO_LIVESRC_STATUS_BASE | 0x0100)	/*!< the base status code for CMMB modules */
#define VO_LIVESRC_STATUS_BASE_DVBH				(VO_LIVESRC_STATUS_BASE | 0x0200)	/*!< the base status code for DVBH modules */
#define VO_LIVESRC_STATUS_BASE_DVBT				(VO_LIVESRC_STATUS_BASE | 0x0300)	/*!< the base status code for DVBT modules */
#define VO_LIVESRC_STATUS_BASE_ATSCMH			(VO_LIVESRC_STATUS_BASE | 0x0400)	/*!< the base status code for ATSC-MH modules */
#define VO_LIVESRC_STATUS_BASE_ISDBT			(VO_LIVESRC_STATUS_BASE | 0x0500)	/*!< the base status code for ISDBT modules */
#define VO_LIVESRC_STATUS_BASE_TDMB				(VO_LIVESRC_STATUS_BASE | 0x0600)	/*!< the base status code for TDMB modules */
#define VO_LIVESRC_STATUS_BASE_DAB				(VO_LIVESRC_STATUS_BASE | 0x0700)	/*!< the base status code for DAB modules */
#define VO_LIVESRC_STATUS_BASE_DMBT				(VO_LIVESRC_STATUS_BASE | 0x0800)	/*!< the base status code for DMBT modules */
#define VO_LIVESRC_STATUS_BASE_HTTP_LS			(VO_LIVESRC_STATUS_BASE | 0x0900)	/*!< the base status code for Http Live Streaming modules */
#define VO_LIVESRC_STATUS_BASE_IIS_SS			(VO_LIVESRC_STATUS_BASE | 0x0A00)	/*!< the base status code for IIS Smooth Streaming modules */


	/*SendData buffer  flag*/
	/*the flag is defined for VO_LIVESRC_SAMPLE.VO_SUORCE_SAMPLE.Flag*/
#define VO_LIVESRC_FLAG_BUFFER_KEYFRAME		0X00000001	/*!<Indicate the buffer is key frame */
#define VO_LIVESRC_FLAG_BUFFER_NEW_PROGRAM	0X00000010	/*!<Indicate the buffer start new program */
#define VO_LIVESRC_FLAG_BUFFER_NEW_FORMAT		0X00000020	/*!<Indicate the buffer start new format, the buffer must parse as VO_LIVESRC_TRACK_INFOEX* */
#define VO_LIVESRC_FLAG_BUFFER_FORCE_FLUSH	0X00000100	/*!<Indicate the buffer flush the previous buffers */
#define VO_LIVESRC_FLAG_BUFFER_EOS			0X00000200	/*!<Indicate the buffer reach EOS */
#define VO_LIVESRC_FLAG_BUFFER_HEADDATA		0X00001000	/*!<Indicate the buffer is head data */

/**
* Task code, use in Cancel()
*/
#define VO_LIVESRC_TASK_BASE		0x42000000
#define	VO_LIVESRC_TASK_SCAN		(VO_LIVESRC_TASK_BASE | 0x0001)	 /*!< the scan task ID */


/**
* Live source output type,  
*/
typedef enum
{
	VO_LIVESRC_OUTPUT_AUDIO		=	1,	/*!<  Output audio frame, see VO_LIVESRC_SAMPLE */
	VO_LIVESRC_OUTPUT_VIDEO		=	2,	/*!<  Output audio frame, see VO_LIVESRC_SAMPLE */
	VO_LIVESRC_OUTPUT_DATABLOCK	=	3,	/*!<  Output data service,just like stock info,traffic info..., see VO_LIVESRC_DATA_SERVICE */
	VO_LIVESRC_OUTPUT_TRACKINFO	=	4	/*!<  Output live source source, just like codec,head data..., see VO_LIVESRC_TRACK_INFO */
}VO_LIVESRC_OUTPUT_TYPE;


/**
* Audio Video sample
*
*/
typedef struct  
{
	VO_U32				nTrackID;	/*!< live source track ID */
	VO_U32				nCodecType;	/*!< live source codec type */
	VO_SOURCE_SAMPLE	Sample;		/*!< sample info */
}VO_LIVESRC_SAMPLE;


/**
* Track info
*
*/
typedef struct  
{
	VO_U32					nTrackCount;	/*!< live source track count */
	VO_U32*					pTrackID;		/*!< live source track ID list */
	VO_SOURCE_TRACKINFO*	pInfo;			/*!< each track info list */
}VO_LIVESRC_TRACK_INFO;

/**
* Track info for VOME
*
*/
typedef struct
{
    VO_SOURCE_TRACKTYPE     Type;           /*!< the type of track*/
    VO_U32                  Codec;          /*!< codec type, VO_AUDIO_CODINGTYPE | VO_VIDEO_CODINGTYPE*/
    union
    {
        VO_AUDIO_FORMAT     audio_info;     // channel, bits, samplerate
        VO_VIDEO_FORMAT     video_info;     // width, height
    };
    VO_PBYTE                HeadData;       /*!< sequence data , if the sequence data exist, it should always equal to 'Padding' */
    VO_U32                  HeadSize;       /*!< sequence data size*/
    VO_BYTE                 Padding[12];    /*!< 12 is the fake size, this Padding length should be equal to HeadSize. If HeadSize is 0, you should not parse this element */
}VO_LIVESRC_TRACK_INFOEX;



/**
* Data service
*
*/
typedef struct  
{
	VO_PTR pData;						/*!< pointer to raw data,just like stock info,traffic info... */
	VO_U32 nDataLen;					/*!< raw data length */
	VO_U32 nDataType;					/*!< raw data type */
	VO_U32 nReserved;					/*!< reserved */
}VO_LIVESRC_DATA_SERVICE;


/**
 * Live source format type
 */
typedef enum
{
	VO_LIVESRC_FT_FILE		= 0x00000000,	/*!< Unknown format */
	VO_LIVESRC_FT_TDMB		= 0x00000001,	/*!< T-DMB */
	VO_LIVESRC_FT_DAB		= 0x00000002,	/*!< DAB */
	VO_LIVESRC_FT_DVBH		= 0x00000003,	/*!< DVB-H stream */
	VO_LIVESRC_FT_ISDBT		= 0x00000004,	/*!< ISDB-T stream */
	VO_LIVESRC_FT_DVBT		= 0x00000005,	/*!< DVB-T stream */
	VO_LIVESRC_FT_DMBT		= 0x00000006,	/*!< DMB-T stream */
	VO_LIVESRC_FT_DVBHTS	= 0x00000007,	/*!< DVB-H stream with TS payload */
	VO_LIVESRC_FT_ATSCMH	= 0x00000008,	/*!< ATSC-MH stream */
	VO_LIVESRC_FT_CMMB		= 0x00000009,	/*!< CMMB stream */
	VO_LIVESRC_FT_HTTP_LS	= 0x0000000A,	/*!< Http Live Streaming */
	VO_LIVESRC_FT_IIS_SS	= 0x0000000B,	/*!< IIS Smooth Streaming */
	VO_LIVESRC_FT_PD		= 0x0000000C,	/*!< HTTP Progressive Download */
	VO_LIVESRC_FT_RTSP		= 0x0000000D,	/*!< RTSP */
	VO_LIVESRC_FT_WMS		= 0x0000000E,	/*!< Windows Media HTTP Streaming */
	VO_LIVESRC_FT_TS    	= 0x0000000F,	/*!< TS Streaming */
	VO_LIVESRC_FT_DASH    	= 0x00000010,	/*!< Dash Streaming */
	VO_LIVESRC_FT_MAX		= VO_MAX_ENUM_VALUE
}VO_LIVESRC_FORMATTYPE;


/**
 * ESG information
 *
 */
typedef struct  
{
	VO_PTR pEsgInfo;					/*!< pointer to VO_CMMB_ESGINFO or other... */
	VO_LIVESRC_FORMATTYPE tEngine;		/*!< live source format */
}VO_LIVESRC_ESG_INFO;


/**
 * For Advertisement Application
 *
 */
struct S_FilterForAdvertisementIn
{
    VO_CHAR strFilterString[256];
	VO_S32  iFilterId;
};

/**
 * For Bitrate Cap and Profile
 *
 */
struct S_AbilityCap
{
    VO_U32  iBitRateCap;
	VO_U32	iProfile;
};

/**
 * For Thumbnail Item
 *
 */
struct S_Thumbnail_Item
{
    VO_CHAR strThumbnailURL[1024];
	VO_U32  ulDuration;
};


/**
* Scan parameter
*
*/
typedef struct  
{
	VO_PTR pParam;						/*!< pointer to VO_LIVESRC_CMMB_SCANPARAM,VO_LIVESRC_DVBT_SCANITEM,VO_LIVESRC_ISDBT_SCANPARAM...     */
	VO_LIVESRC_FORMATTYPE tEngine;		/*!< live source format */
}VO_LIVESRC_SCAN_PARAMEPTERS;


/**
 * Channel Information
 */
typedef struct
{
	VO_U32					nChannelID;			/*!< the channel ID */
	VO_TCHAR				szName[256];		/*!< the channel Name */
	VO_PTR					hIcon;				/*!< the channel Icon */
	VO_U32					nCurrent;			/*!< 1 is current channel 0 is not*/
	VO_LIVESRC_FORMATTYPE	nType;				/*!< live source type (CMMB, DVB-T, T-DMB, DVB-H..... */
	VO_PTR					pData;				/*!< if CMMB, pointer to VO_LIVESRC_CHANNELINFOEX_CMMB struct */
}VO_LIVESRC_CHANNELINFO;

typedef enum
{
	VO_LIVESRC_MEDIA_PURE_AUDIO,
	VO_LIVESRC_MEDIA_PURE_VIDEO,
	VO_LIVESRC_MEDIA_AUDIOVIDEO,
}VO_LIVESRC_MEDIA_TYPE;
	
typedef struct
{
    VO_S32 delaytime;							//video delay time
    VO_S64 playtime;							//current playing time
}VO_LIVESRC_DELAYTIME;


/**
 * Callback function. The source will notify client via this function for some events.
 * \param pUserData [in] The user data which was set by Open().
 * \param nID [in] The status type.
 * \param nParam1 [in] status specific parameter 1.
 * \param nParam2 [in] status specific parameter 2.
 */
typedef VO_VOID (VO_API * VOSRCSTATUS) (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);

/**
 * Callback function. The source will send the data out..
 * \param pUserData [in] The user data which was set by Open().
 * \param nOutputType [in] The output data type, see VO_LIVESRC_OUTPUT_TYPE
 * \param pData [in] The data pointer, it MUST be converted to correct struct pointer according nType, see VO_LIVESRC_OUTPUT_TYPE
 */
typedef VO_VOID (VO_API * VOSRCSENDDATA) (VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData);



/**
* Live source function set
*/
typedef struct
{
/**
 * Initialize a live source session.
 * This function should be called first for a session.
 * \param phLiveSrc [out] live source handle.
 * \param pUserData [in] The user data. It will use in call back function
 * \param fStatus [in] status notify call back function pointer.
 * \param fSendData [in] send data call back function pointer.
 * Return Value: 0 it didn't scan before, > 0 it scanned before and return the channels
 */
VO_U32 (VO_API * Open) (VO_HANDLE * phLiveSrc, VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData);

/**
 * Close the live source session.
 * This function should be called last of the session.
 * \param hLiveSrc [in] The live source handle. Opened by Open().
 */
VO_U32 (VO_API * Close) (VO_HANDLE hLiveSrc);

/**
 * Scan the channels
 * \param hLiveSrc [in] The live source engine handle. Opened by Open().
  * \param pParam [in] It was structure depend on the live source type... 
 */
VO_U32 (VO_API * Scan) (VO_HANDLE hLiveSrc, VO_LIVESRC_SCAN_PARAMEPTERS* pParam);


/**
 * Cancel the current task
 * \param hLiveSrc [in] The live source handle. Opened by Open().
  * \param nID [in] The task ID. It only support scan task now..
*/
VO_U32 (VO_API * Cancel) (VO_HANDLE hLiveSrc, VO_U32 nID);

/**
 * start to send data out
 * \param hLiveSrc [in] The live source handle. Opened by Open().
 */
VO_U32 (VO_API * Start) (VO_HANDLE hLiveSrc);

/**
 * pause to send data out
 * \param hLiveSrc [in] The live source handle. Opened by Open().
 */
VO_U32 (VO_API * Pause) (VO_HANDLE hLiveSrc);

/**
 * stop to send data out
 * \param hLiveSrc [in] The live source handle. Opened by Open().
 */
VO_U32 (VO_API * Stop) (VO_HANDLE hLiveSrc);

/**
* Set data source type
* \param hLiveSrc [in] The live source handle. Opened by Open().
* \param pSource [in] The source. Normally it should be URL for HTTP Live Streaming.
* \param nFlag [in]. It can indicate the source type..
*/
VO_U32 (VO_API * SetDataSource) (VO_HANDLE hLiveSrc, VO_PTR pSource, VO_U32 nFlag);

/**
* Data source seek, Normally it should be used by HTTP Live Streaming
* \param hLiveSrc [in] The live source handle. Opened by Open().
* \param pSource [in] The source.
* \param nTimeStamp [in]. Seek to this timestamp.
*/
VO_U32 (VO_API * Seek) (VO_HANDLE hLiveSrc, VO_U64 nTimeStamp);

/**
 * Get Channel information
 * \param hLiveSrc [in] The live source handle. Opened by Open().
 * \param pCount [out] The channel count
 * \param ppChannelInfo [out] The channel info pointer
 */
VO_U32 (VO_API * GetChannel) (VO_HANDLE hLiveSrc, VO_U32* pCount, VO_LIVESRC_CHANNELINFO** ppChannelInfo);

/**
 * select the channel
 * \param hLiveSrc [in] The live source handle. Opened by Open().
 * \param nChannelID [in] The channel ID
 */
VO_U32 (VO_API * SetChannel) (VO_HANDLE hLiveSrc, VO_S32 nChannelID);

/**
 * Get ESG information
 * \param hLiveSrc [in] The live source handle. Opened by Open().
 * \param nChannelID [in] The channel ID
 * \param ppESGInfo [out] The ESG info pointer
 */
VO_U32 (VO_API * GetESG) (VO_HANDLE hLiveSrc, VO_S32 nChannelID, VO_LIVESRC_ESG_INFO** ppESGInfo);

/**
 * Get the special value from param ID
 * \param hLiveSrc [in] The live source handle. Opened by Open().
 * \param nParamID [in] The param ID
 * \param pParam [out] The return value depend on the param ID.
 */
VO_U32 (VO_API * GetParam) (VO_HANDLE hLiveSrc, VO_U32 nParamID, VO_PTR pParam);

/**
 * Set the special value from param ID
 * param hLiveSrc [in] The live source handle. Opened by Open().
 * \param nParamID [in] The param ID
 * \param pParam [in] The set value depend on the param ID.
 */
VO_U32 (VO_API * SetParam) (VO_HANDLE hLiveSrc, VO_U32 nParamID, VO_PTR pParam);

/**
* Send the buffer into player
* \param hLiveSrc [in] The live source handle. Opened by Open().
* \param pBuffer [in] It is ts stream buffer.
* \retval VO_ERR_LIVESRC_OK Succeeded
*/
VO_U32 (VO_API * SendBuffer) (VO_HANDLE hLiveSrc, VOMP_BUFFERTYPE * pBuffer);
} VO_LIVESRC_API;

/**
* Get Live Source API interface
* \param pHandle [IN/OUT] Return the live source API handle.
* \retval VO_ERR_LIVESRC_OK Succeeded.
*/
//#ifdef _HLS_SOURCE_
	VO_S32 VO_API voGetHLSLiveSrcAPI(VO_LIVESRC_API* pHandle);
//#elif defined  _ISS_SOURCE_
	VO_S32 VO_API voGetISSLiveSrcAPI(VO_LIVESRC_API* pHandle);
//#elif defined _LIVE_TS_SOURCE_
	VO_S32 VO_API voGetTSLiveSrcAPI(VO_LIVESRC_API* pHandle);
//#elif defined _CMMB_SOURCE_	
	VO_S32 VO_API voGetCMMBLiveSrcAPI(VO_LIVESRC_API* pHandle);
//#else 
	VO_S32 VO_API voGetLiveSrcAPI(VO_LIVESRC_API* pHandle);
//#endif // _HLS_SOURCE_

typedef VO_U32 (VO_API * VOGETLIVESRCAPI) (VO_LIVESRC_API* pHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __VOLIVESRC_H__
