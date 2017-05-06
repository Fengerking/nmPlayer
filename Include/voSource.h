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

#ifndef __voSource_H__
#define __voSource_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voIndex.h"
#include "voString.h"
#include "voMem.h"

#include "voAudio.h"
#include "voVideo.h"
#include "voDRM.h"

/**
* Parameter ID
* S - set; G - get; S/G - both set and get.
*/
#define	VO_PID_SOURCE_BASE				 0x43000000							/*!< the base param ID for source modules */
#define	VO_PID_SOURCE_SELTRACK			(VO_PID_SOURCE_BASE | 0x0001)		/*!< <G/S>get selected track information/select the track to playback, VO_SOURCE_SELTRACK */
#define VO_PID_SOURCE_PREPARETRACKS		(VO_PID_SOURCE_BASE | 0x0002)		/*!< <S>prepare/unprepare tracks for playback after call/before VO_PID_SOURCE_SELTRACK, VO_BOOL */
#define VO_PID_SOURCE_FIRSTFRAME		(VO_PID_SOURCE_BASE | 0x0010)		/*!< <G>first frame data, VO_SOURCE_SAMPLE */
#define VO_PID_SOURCE_MAXSAMPLESIZE		(VO_PID_SOURCE_BASE | 0x0011)		/*!< <G>max sample size, VO_U32 */
#define VO_PID_SOURCE_CODECCC			(VO_PID_SOURCE_BASE | 0x0012)		/*!< <G>character code, video is four, audio is two, VO_U32 */
#define VO_PID_SOURCE_NEXTKEYFRAME		(VO_PID_SOURCE_BASE | 0x0013)		/*!< <G>next key frame timestamp, VO_SOURCE_SAMPLE */
#define VO_PID_SOURCE_PLAYMODE			(VO_PID_SOURCE_BASE | 0x0014)		/*!< <S>play mode, VO_SOURCE_PLAYMODE */
#define VO_PID_SOURCE_BITRATE			(VO_PID_SOURCE_BASE | 0x0017)		/*!< <G>bitrate, VO_U32* */
#define VO_PID_SOURCE_FRAMENUM			(VO_PID_SOURCE_BASE | 0x0018)		/*!< <G>Get the total frame numbers of the "video" track, VO_U32* */
#define VO_PID_SOURCE_FRAMETIME			(VO_PID_SOURCE_BASE | 0x0019)		/*!< <G>Get the frame time of the "video" track, VO_U32*, <1/100ms> */
#define VO_PID_SOURCE_WAVEFORMATEX		(VO_PID_SOURCE_BASE | 0x001A)		/*!< <G>Get the pointer of VO_WAVEFORMATEX, VO_WAVEFORMATEX* */
#define VO_PID_SOURCE_BITMAPINFOHEADER	(VO_PID_SOURCE_BASE | 0x001B)		/*!< <G>Get the pointer of VO_BITMAPINFOHEADER, VO_BITMAPINFOHEADER* */
#define VO_PID_SOURCE_FILESIZE			(VO_PID_SOURCE_BASE | 0x001C)		/*!< <G/S>Get/Set the file size. VO_S64* */
#define VO_PID_SOURCE_SEEKABLE			(VO_PID_SOURCE_BASE | 0x001D)		/*!< <G>If the file can seek. VO_BOOL* */
#define VO_PID_SOURCE_NEEDSCAN			(VO_PID_SOURCE_BASE | 0x001E)		/*!< <G>If the file need scan to get duration, seek point, ... VO_BOOL* */
#define VO_PID_SOURCE_NEEDLOOPPLAY		(VO_PID_SOURCE_BASE | 0x001F)		/*!< <G>for ogg, google add one special comment to let player loop playback,... VO_BOOL* */
#define VO_PID_SOURCE_NEARKEYFRAME		(VO_PID_SOURCE_BASE | 0x0020)		/*!< <G>get previous and next key frame time of input time, VO_SOURCE_NEARKEYFRAME* */
#define VO_PID_SOURCE_VIDEOPROFILELEVEL	(VO_PID_SOURCE_BASE | 0x0021)		/*!< <G>get video profile&level, VO_VIDEO_PROFILELEVEL* */
#define VO_PID_SOURCE_ISINTERLACE		(VO_PID_SOURCE_BASE | 0x0022)		/*!< <G>check is interlace */
#define VO_PID_SOURCE_SEEKMODE			(VO_PID_SOURCE_BASE | 0x0023)		/*!< <S>tell source seek mode of framework, VO_U32*, 0 - key frame; 1 - accurate */
#define VO_PID_SOURCE_ACTUALFILESIZE    (VO_PID_SOURCE_BASE | 0x0024)       /*!< <G>Get the actual file size when it is push play mode. VO_S64*  ret value has three conditions. VO_ERR_NONE means ok, VO_ERR_BASE means can not support seek , VO_ERR_NOT_IMPLEMENT means it is not push play*/
#define VO_PID_SOURCE_SEEKABLETIME      (VO_PID_SOURCE_BASE | 0x0025)       /*!< <G>Get the max seekable time in ms. VO_S64* */
#define VO_PID_SOURCE_GETSEEKTIMEBYPOS  (VO_PID_SOURCE_BASE | 0x0026)       /*!< <G>Get the seekable time with certain file size. VO_FILE_MEDIATIMEFILEPOS* */
#define VO_PID_SOURCE_THUMBNAILSCANRANGE (VO_PID_SOURCE_BASE | 0x0027)       /*!< <S>Set the key frame scan range, right now it is how many key frame to scan , VO_S32* */
#define VO_PID_SOURCE_GETTHUMBNAIL       (VO_PID_SOURCE_BASE | 0x0028)       /*!< <G>Get the thumbnail key frame , VO_SOURCE_SAMPLE* */
#define VO_PID_SOURCE_DATE              (VO_PID_SOURCE_BASE | 0x0029)       /*!< <G>Get the source encoded date. char* */
#define VO_PID_SOURCE_NUM_TRACKS        (VO_PID_SOURCE_BASE | 0x0030)       /*!< <G>Get the source track number. VO_U32* */
#define VO_PID_SOURCE_LOCATION		    (VO_PID_SOURCE_BASE | 0x0031)		/*!< <G>Get GPS information : latitude, logitude, char* */
#define VO_PID_SOURCE_MINORVERSION		(VO_PID_SOURCE_BASE | 0x0032)		/*!< <G>Get  MinorVersion information. VO_U32* */
#define VO_PID_SOURCE_MPLAYERID			(VO_PID_SOURCE_BASE | 0x0033)		/*!< <G>Get  Audio track format profile.Layer 1 is 1, Layer 2 is 2  */
#define VO_PID_SOURCE_TRACK_LANGUAGE	(VO_PID_SOURCE_BASE | 0x0034)		/*!< <G>Get  Audio track format profile.Layer 1 is 1, Layer 2 is 2  */
#define VO_PID_SOURCE_WIDEVINE			(VO_PID_SOURCE_BASE | 0xF005)		/*!< <G>Get if the content is widevine or not */
#define	VO_PID_SOURCE_WORKPATH			(VO_PID_SOURCE_BASE | 0xF006)		/*!< <S>Set the lib load work path */
#define	VO_PID_SOURCE_HTTPVERIFICATIONCALLBACK (VO_PID_SOURCE_BASE | 0xF007)/*!< <S>the parameter is a pointer to struct VO_HTTP_VERIFYCALLBACK */
#define	VO_PID_SOURCE_DOHTTPVERIFICATION (VO_PID_SOURCE_BASE | 0xF008)		/*!< <S>the parameter is a pointer to struct VO_SOURCE_VERIFICATIONINFO */
#define	VO_PID_SOURCE_EVENTCALLBACK	(VO_PID_SOURCE_BASE | 0xF009)			/*!< <S>the parameter is a pointer to struct VO_SOURCE_EVENTCALLBACK */
#define	VO_PID_SOURCE_NEXT_FRAME_INFO	(VO_PID_SOURCE_BASE | 0xF00A)			/*!< <G>the parameter is a pointer to struct VO_SOURCE_SAMPLE */
#define	VO_PID_SOURCE_SECOND_IO			(VO_PID_SOURCE_BASE | 0xF00B)			/*!< <G>the parameter is a pointer to struct VO_FILE_SOURCE */
#define	VO_PID_SOURCE_CURRENT_FILE_INFO	(VO_PID_SOURCE_BASE | 0xF00C)			/*!< <G>the parameter is a pointer to struct VO_PUSH_TRACK_INFO */
#define 	VO_PID_SOURCE_FLAG_OPEN_THUMBNAIL		(VO_PID_SOURCE_BASE | 0xF00D)		/*!<Indicate this instance is for getting the thumbnail */

#define VO_PID_SOURCE_LGSPECIAL			(VO_PID_SOURCE_BASE | 0x1000)
#define VO_PID_SOURCE_LGE_VIDEO_CONTENT	(VO_PID_SOURCE_LGSPECIAL | 0x0033)		/*!< <G>Get whether this is LG content, !!!FOR LG ONLY!!!! . VO_U32* */



/**
* Error code
*/
#define VO_ERR_SOURCE_OK				VO_ERR_NONE
#define VO_ERR_SOURCE_BASE				0x86000000
#define VO_ERR_SOURCE_OPENFAIL			(VO_ERR_SOURCE_BASE | 0x0001)		/*!< open fail */
#define VO_ERR_SOURCE_NEEDRETRY			(VO_ERR_SOURCE_BASE | 0x0002)		/*!< can not finish operation, but maybe you can finish it next time */
#define VO_ERR_SOURCE_END				(VO_ERR_SOURCE_BASE | 0x0003)		/*!< play end */
#define VO_ERR_SOURCE_CONTENTENCRYPT	(VO_ERR_SOURCE_BASE | 0x0004)		/*!< content is encrypt, only can playback after some operations */
#define VO_ERR_SOURCE_CODECUNSUPPORT	(VO_ERR_SOURCE_BASE | 0x0005)		/*!< codec not support */
#define VO_ERR_SOURCE_PLAYMODEUNSUPPORT	(VO_ERR_SOURCE_BASE | 0x0006)		/*!< play mode not support */
#define VO_ERR_SOURCE_ERRORDATA			(VO_ERR_SOURCE_BASE | 0x0007)		/*!< file has error data */
#define VO_ERR_SOURCE_SEEKFAIL			(VO_ERR_SOURCE_BASE | 0x0008)		/*!< seek not support */
#define VO_ERR_SOURCE_FORMATUNSUPPORT	(VO_ERR_SOURCE_BASE | 0x0009)		/*!< file format not support */
#define VO_ERR_SOURCE_TRACKNOTFOUND		(VO_ERR_SOURCE_BASE | 0x000A)		/*!< track can not be found */
#define VO_ERR_SOURCE_NEEDPROXY			(VO_ERR_SOURCE_BASE | 0x000B)		/*!< need proxy setting, mainly for HTTP&RTSP streaming */
#define VO_ERR_SOURCE_AUDIOUNSUPPORT	(VO_ERR_SOURCE_BASE | 0x000C)		/*!< audio not support */
#define VO_ERR_SOURCE_VIDEOUNSUPPORT	(VO_ERR_SOURCE_BASE | 0x000D)		/*!< video not support */

/**
 * Track type
 */
typedef enum
{
	VO_SOURCE_TT_VIDEO				= 0X00000001,	/*!< video track*/
	VO_SOURCE_TT_AUDIO				= 0X00000002,	/*!< audio track*/
	VO_SOURCE_TT_IMAGE				= 0X00000003,	/*!< image track*/
	VO_SOURCE_TT_STRAM				= 0X00000004,	/*!< stream track*/
	VO_SOURCE_TT_SCRIPT				= 0X00000005,	/*!< script track*/
	VO_SOURCE_TT_HINT				= 0X00000006,	/*!< hint track*/
	VO_SOURCE_TT_RTSP_VIDEO         = 0X00000007,   /*!< rtsp streaming video track*/
	VO_SOURCE_TT_RTSP_AUDIO         = 0X00000008,   /*!< rtsp streaming audio track*/
	VO_SOURCE_TT_SUBTITLE			= 0X00000009,   /*!< sub title track*/
	VO_SOURCE_TT_TELETEXT			= 0X0000000A,   /*!< tele text track*/
	VO_SOURCE_TT_RICHMEDIA			= 0X0000000B,   /*!< rich media track*/
	VO_SOURCE_TT_MAX				= VO_MAX_ENUM_VALUE
}VO_SOURCE_TRACKTYPE;

/**
 * Source information
 */
typedef struct
{
	VO_U32				Duration;					/*!< duration of file(MS)*/
	VO_U32				Tracks;						/*!< number of track*/
}VO_SOURCE_INFO;

/**
 * Track information
 */
typedef struct
{
	VO_SOURCE_TRACKTYPE	Type;						/*!< the type of track*/
	VO_U32				Codec;						/*!< codec type, VO_AUDIO_CODINGTYPE | VO_VIDEO_CODINGTYPE*/
	VO_S64				Start;						/*!< start time(MS)*/
	VO_U32				Duration;					/*!< duration of track(MS)*/
	VO_PBYTE			HeadData;					/*!< sequence data*/
	VO_U32				HeadSize;					/*!< sequence data size*/
}VO_SOURCE_TRACKINFO;

#define VO_SOURCE_SAMPLE_FLAG_FRAMEDROPPED	0x01	// only for Video I frame, when some previous frames are dropped
#define VO_SOURCE_SAMPLE_FLAG_CODECCONFIG	0x02	// the Buffer is configure data
#define VO_SOURCE_SAMPLE_FLAG_NEWPROGRAM	0x10
#define VO_SOURCE_SAMPLE_FLAG_NEWVIDEOSIZE	0x20
#define VO_SOURCE_SAMPLE_FLAG_DivXDRM		0x40	//indicate the frame is encrypted in JIT mode
#define VO_SOURCE_SAMPLE_FLAG_NOFRAMESKIPED	0x80	// only for Video I frame, when some previous frames are dropped
/**
 * Sample information
 */
typedef struct
{
	VO_PBYTE		Buffer;					/*!< buffer address of sample, null indicate not need get buffer*/
	VO_U32			Size;					/*!< buffer size. the highest bit is key sample or not*/
	VO_S64			Time;					/*!< start time(MS)*/
	VO_U32			Duration;				/*!< duration of sample(MS)*/
	VO_U32			Flag;
}VO_SOURCE_SAMPLE;

/**
* DRM callback
*/
#define VO_SOURCEDRM_FLAG_DRMINFO			0x1
#define VO_SOURCEDRM_FLAG_DRMDATA			0x2
#define VO_SOURCEDRM_FLAG_FLUSH				0x10
#define VO_SOURCEDRM_FLAG_ISSUPPORTED		0x20

typedef struct
{
	VO_DRM_TYPE			nType;			//DRM type
	VO_PTR				pDrmInfo;		//DRM information in file, the format is decided by nType
	VO_U32				nReserved[6];	//reserved for future
} VO_SOURCEDRM_INFO;

typedef struct
{
	VO_DRM_DATAINFO		sDataInfo;		//information needed by DecryptDataXXX
	VO_PBYTE			pData;			//encrypted data buffer
	VO_U32				nSize;			//encrypted data size
	VO_PBYTE*			ppDstData;		//reserved for size-changed DRM
	VO_U32*				pnDstSize;		//reserved for size-changed DRM
	VO_U32				nReserved[4];	//reserved for future
} VO_SOURCEDRM_DATA;

/**
* VisualOn file parser DRM callback.
* \param pUserData: user data set in.
* \param nFlag: VO_SOURCEDRM_FLAG_XXX.
* \param pParam: VO_SOURCEDRM_FLAG_DRMINFO - VO_SOURCEDRM_INFO*; VO_SOURCEDRM_FLAG_DRMDATA - VO_SOURCEDRM_DATA*; VO_SOURCEDRM_FLAG_ISSUPPORTED - VO_DRM_TYPE*.
* \param nReserved: reserved.
* \retval VO_ERR_NONE Succeeded, VO_ERR_DRM_XXX.
*/
typedef VO_U32 (VO_API * VOSOURCEDRMCALLBACK)(VO_PTR pUserData, VO_U32 nFlag, VO_PTR pParam, VO_U32 eSourceFormat);// !eSourceFormat used in DRM2.h

// Some source module need request response itself, so please implement this callback internally
// Framework will call it if necessary, East 20110802
// [IN] pChallenge: Challenge Data
// [IN] nChallenge: Length of pChallenge
// [IN] pcUrl: Server URL to process HTTP POST / HTTP GET
// [IN] nUrl: Length of pUrlData
// [IN] pcHttpHeader: Additional HTTP header about "SOAP Action"
// [IN] nHttpHeader: Length of pcHttpHeader
// [OUT] ppcMimetype: pointer of mime type of Response Data
// [OUT] ppResponse: pointer of Response Data from PlayReady Server
// [OUT] nResponse: Length of pResponse
// [IN] pbCancel: reserved cancel flag, you need check (*pbCancel == VO_TRUE) any time, exit as quickly as possible.
typedef VO_U32 (VO_API * VO_GET_LICENSE_RESPONSE)(VO_PBYTE pChallenge, VO_U32 nChallenge, VO_PCHAR pcUrl, VO_U32 nUrl, VO_PCHAR pcHttpHeader, VO_U32 nHttpHeader, 
												  VO_PCHAR* ppcMimetype, VO_PBYTE* ppResponse, VO_U32* pnResponse, VO_BOOL* pbCancel, VO_PTR pUserData);

typedef struct
{
	VOSOURCEDRMCALLBACK	fCallback;		//callback function pointer
	VO_PTR				pUserData;		//user data
} VO_SOURCEDRM_CALLBACK;

/**
 * Source open parameters
 */
/**
 nFlag: 0xRRRRRRRR RRRRRRRR FFFFFFFF OOOOOOOO (order: 87654321)
 O: source open flags(local file, PD, RTSP...)

 F: source operator flags(file operation, callback...)

 R: reserved flags
 */
#define VO_SOURCE_OPENPARAM_FLAG_OPENLOCALFILE			0x1
#define VO_SOURCE_OPENPARAM_FLAG_OPENPD					0x2
#define VO_SOURCE_OPENPARAM_FLAG_OPENRTSP				0x3
#define VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL        0x4         //this flag is used for speeding up get thumbnail, this flag should not set for playback
#define VO_SOURCE_OPENPARAM_FLAG_OPENFORSTREAMING        0x8        //this flag is used for streaming, this flag should not set for local playback

#define VO_SOURCE_OPENPARAM_FLAG_FILEOPERATOR			0x100
#define VO_SOURCE_OPENPARAM_FLAG_RTSPCALLBACK			0x200

#define VO_SOURCE_OPENPARAM_FLAG_INFOONLY				0x10000		//only for get media information instead of playback
#define VO_SOURCE_OPENPARAM_FLAG_EXACTSCAN				0x20000		//scan exactly

#define VO_SOURCE_OPENPARAM_FLAG_PUSHPLAY               0x40000     //it is push play

typedef struct
{
	VO_U32					nFlag;				/*!< open flags  */
	VO_PTR					pSource;			/*!< source param. Local file is VO_FILE_SOURCE, RTSP is URL */
	VO_PTR					pSourceOP;			/*!< source operator functions. */
	VO_MEM_OPERATOR *		pMemOP;				/*!< memory operator functions. */
	VO_SOURCEDRM_CALLBACK *	pDrmCB;				/*!< DRM callback function. */
	VO_LIB_OPERATOR *		pLibOP;				/*!< Library operator function set. */
	VO_U32					nReserve;			/*!< reserved. */
	VO_TCHAR *				pstrWorkpath;		/*!< work path for license check. */
}VO_SOURCE_OPENPARAM;

/**
* Source play mode
*/
typedef enum
{
	VO_SOURCE_PM_PLAY				= 0X00000001,	/*!< normal play */
	VO_SOURCE_PM_FF					= 0X00000002,	/*!< fast forward */
	VO_SOURCE_PM_BF					= 0X00000003,	/*!< fast backward */
	VO_SOURCE_PM_MAX				= VO_MAX_ENUM_VALUE
}VO_SOURCE_PLAYMODE;

#define VO_ALL 0x7FFFFFFF
typedef struct
{
	VO_U32					nIndex;		//I
	VO_BOOL					bInUsed;	//I/O
	VO_U64					llTs;		//TimeStamp
}VO_SOURCE_SELTRACK;

typedef struct VO_SOURCE_NEARKEYFRAME
{
	VO_S64					Time;					// <I> specified time stamp, can > end but must > start, otherwise return VO_ERR_INVALID_ARG
	VO_S64					PreviousKeyframeTime;	// <O> time stamp of previous key frame, <= Time, VO_MAXS64 mean no key frame(commonly no such case)
	VO_S64					NextKeyframeTime;		// <O> time stamp of next key frame, > Time, VO_MAXS64 mean no key frame(commonly reach end)
} VO_SOURCE_NEARKEYFRAME;

typedef struct 
{
	VO_PTR pUserData;				/*!<The userdata recved the VO_SOURCE2_IO_HTTP_VERIFYCALLBACK*/
	VO_PTR pData;					/*!<The data that need to do verification*/
	VO_U32 uDataSize;				/*!<The data size*/
	VO_U32 uDataFlag;				/*!<The data flag, if it is 0, it is the request string, other value to not support currently */
	VO_CHAR * szResponse;			/*!<The response string*/
	VO_U32 uResponseSize;			/*!<The response string size*/
}VO_SOURCE_VERIFICATIONINFO;

/**
 * callback function for verification
 * \param hHandle [in] the hHandle member in VO_SOURCE2_IO_HTTP_VERIFYCALLBACK structure
 * \param uID [in] should use the value defined in VO_SOURCE2_IO_HTTP_STATUS
 * \param pData [in] the data need to send out
 */
typedef VO_U32 (VO_API * HTTP_VERIFYCALLBACK_FUNC)( VO_PTR hHandle , VO_U32 uID , VO_PTR pUserData );

struct VO_HTTP_VERIFYCALLBACK 
{
	VO_PTR hHandle;
	HTTP_VERIFYCALLBACK_FUNC HTTP_Callback;
};

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

}VO_SOURCE_EVENTCALLBACK;


typedef enum
{
	VO_SOURCE_THUMBNAILMODE_INFOONLY = 0,		/*!< only need the thumbnail info, do not need the thumbnail sample data */
	VO_SOURCE_THUMBNAILMODE_DATA = 1,			/*!< need the thumbanil sample data */
}VO_SOURCE_THUMBNAILMODE;

typedef struct  
{
	VO_SOURCE_THUMBNAILMODE		uFlag;			/*!< [in] Should use the value defined in VO_SOURCE2_THUMBNAILMODE */

	VO_PBYTE						pSampleData;	/*!< [out] Sample data */
	VO_U32							uSampleSize;	/*!< [out] Sample size */

	VO_S64							ullTimeStamp;	/*!< [out] Sample time stamp */

	VO_U64							ullFilePos;		/*!< [out] Sample file pos in the media file */
	VO_U32							uPrivateData;	/*!< [out] Private Data for source use internally only */
}VO_SOURCE_THUMBNAILINFO;

/**
 * Source reader function set
 */
typedef struct
{
	/**
	 * Open the source and return source handle
	 * \param ppHandle [OUT] Return the source operator handle
	 * \param pName	[IN] The source name
	 * \param pParam [IN] The source open param
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * Open) (VO_PTR * ppHandle, VO_SOURCE_OPENPARAM * pParam);

	/**
	 * Close the opened source.
	 * \param pHandle [IN] The handle which was create by open function.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * Close) (VO_PTR pHandle);

	/**
	 * Get the source information
	 * \param pHandle [IN] The handle which was create by open function.
	 * \param pSourceInfo [OUT] The structure of source info to filled.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * GetSourceInfo) (VO_PTR pHandle, VO_SOURCE_INFO * pSourceInfo);

	/**
	 * Get the track information
	 * \param pHandle [IN] The handle which was create by open function.
	 * \param nTrack [IN] The index of the track in source
	 * \param pTrackInfo [OUT] The track info to filled..
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * GetTrackInfo) (VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_TRACKINFO * pTrackInfo);

	/**
	 * Get the track buffer
	 * \param pHandle [IN] The handle which was create by open function.
	 * \param nTrack [IN] The index of the track in source
	 * \param pSample [OUT] The sample info was filled.
	 *		  Audio. It will fill the next frame audio buffer automatically.
	 *		  Video  It will fill the frame data depend on the sample time. if the next key frmae time
	 *				 was less than the time, it will fill the next key frame data, other, it will fill
	 *				 the next frame data.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * GetSample) (VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_SAMPLE * pSample);

	/**
	 * Set the track read position.
	 * \param pHandle [IN] The handle which was create by open function.
	 * \param nTrack [IN] The index of the track in source
	 * \param pPos [IN/OUT] The new pos will be set, and it will reset with previous key frame time if it is video track.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * SetPos) (VO_PTR pHandle, VO_U32 nTrack, VO_S64 * pPos);

	/**
	 * Set source param for special target.
	 * \param pHandle [IN] The handle which was create by open function.
	 * \param uID [IN] The param ID.
	 * \param pParam [IN] The param value depend on the ID>
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * SetSourceParam) (VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);

	/**
	 * Get source param for special target.
	 * \param pHandle [IN] The handle which was create by open function.
	 * \param uID [IN] The param ID.
	 * \param pParam [Out] The param value depend on the ID>
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * GetSourceParam) (VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);

	/**
	 * Set track param for special target.
	 * \param pHandle [IN] The handle which was create by open function.
	 * \param nTrack [IN] The index of the track in source
	 * \param uID [IN] The param ID.
	 * \param pParam [IN] The param value depend on the ID>
	 * \retval VO_ERR_NONE Succeeded. 
	 *         VO_ERR_SOURCE_END out of the duration
	 */
	VO_U32 (VO_API * SetTrackParam) (VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam);

	/**
	 * Get track param for special target.
	 * \param pHandle [IN] The handle which was create by open function.
	 * \param nTrack [IN] The index of the track in source
	 * \param uID [IN] The param ID.
	 * \param pParam [IN] The param value depend on the ID>
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * GetTrackParam) (VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam);
} VO_SOURCE_READAPI;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voSource_H__
