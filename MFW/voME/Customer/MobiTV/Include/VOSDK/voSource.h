	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voSource.h

	Contains:	data type define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

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

/**
* Parameter ID
* S - set; G - get; S/G - both set and get.
*/
#define	VO_PID_SOURCE_BASE				 0x43000000							/*!< the base param ID for source modules */
#define	VO_PID_SOURCE_SELTRACK			(VO_PID_SOURCE_BASE | 0x0001)		/*!< <S>select the track to playback */
#define VO_PID_SOURCE_FIRSTFRAME		(VO_PID_SOURCE_BASE | 0x0010)		/*!< <G>first frame data, VO_SOURCE_SAMPLE */
#define VO_PID_SOURCE_MAXSAMPLESIZE		(VO_PID_SOURCE_BASE | 0x0011)		/*!< <G>max sample size, VO_U32 */
#define VO_PID_SOURCE_CODECCC			(VO_PID_SOURCE_BASE | 0x0012)		/*!< <G>character code, video is four, audio is two, VO_U32 */
#define VO_PID_SOURCE_NEXTKEYFRAME		(VO_PID_SOURCE_BASE | 0x0013)		/*!< <G>next key frame timestamp, VO_SOURCE_SAMPLE */
#define VO_PID_SOURCE_PLAYMODE			(VO_PID_SOURCE_BASE | 0x0014)		/*!< <S>play mode, VO_SOURCE_PLAYMODE */
#define VO_PID_SOURCE_TRACKINUSE		(VO_PID_SOURCE_BASE | 0x0016)		/*!< <S>set track in use state, VO_BOOL */
#define VO_PID_SOURCE_BITRATE			(VO_PID_SOURCE_BASE | 0x0017)		/*!< <G>bitrate, VO_U32* */
#define VO_PID_SOURCE_FRAMENUM			(VO_PID_SOURCE_BASE | 0x0018)		/*!< <G>Get the total frame numbers of the "video" track, VO_U32* */
#define VO_PID_SOURCE_FRAMETIME			(VO_PID_SOURCE_BASE | 0x0019)		/*!< <G>Get the frame time of the "video" track, VO_U32*, <1/100ms> */
#define VO_PID_SOURCE_WAVEFORMATEX		(VO_PID_SOURCE_BASE | 0x001A)		/*!< <G>Get the pointer of VO_WAVEFORMATEX, VO_WAVEFORMATEX* */
#define VO_PID_SOURCE_BITMAPINFOHEADER	(VO_PID_SOURCE_BASE | 0x001B)		/*!< <G>Get the pointer of VO_BITMAPINFOHEADER, VO_BITMAPINFOHEADER* */
#define VO_PID_SOURCE_FILESIZE			(VO_PID_SOURCE_BASE | 0x001C)		/*!< <G>Get the file size. VO_S64* */
#define VO_PID_SOURCE_DRMTYPE			(VO_PID_SOURCE_BASE | 0x0021)		/*!< <G>Get DRM type, VO_SOURCE_DRMTYPE* */
#define VO_PID_SOURCE_DRMINFO			(VO_PID_SOURCE_BASE | 0x0022)		/*!< <G>Get DRM information, VO_SOURCE_DRMINFO* */
#define VO_PID_SOURCE_DRMCOMMIT			(VO_PID_SOURCE_BASE | 0x0023)		/*!< <S>Commit DRM usage */

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

#define VO_SOURCE_OPENPARAM_FLAG_FILEOPERATOR			0x100
#define VO_SOURCE_OPENPARAM_FLAG_RTSPCALLBACK			0x200

#define VO_SOURCE_OPENPARAM_FLAG_INFOONLY				0x10000		//only for get media information instead of playback
#define VO_SOURCE_OPENPARAM_FLAG_EXACTSCAN				0x20000		//scan exactly

typedef struct
{
	VO_U32				nFlag;				/*!< open flags  */
	VO_PTR				pSource;			/*!< source param. Local file is VO_FILE_SOURCE, RTSP is URL */
	VO_PTR				pSourceOP;			/*!< source operator functions. */
	VO_MEM_OPERATOR *	pMemOP;				/*!< memory operator functions. */
	VO_U32				nReserve;
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

/**
* Source DRM type
*/
typedef enum
{
	VO_SOURCE_DRM_UNKNOWN			= 0,
	VO_SOURCE_DRM_DIVX				= 1,			/*!< DivX DRM */
	VO_SOURCE_DRM_MAX				= VO_MAX_ENUM_VALUE
}VO_SOURCE_DRMTYPE;

/**
* Source DRM information
*/
/**
Flags: 0xDDDDDDDD CCCCCCCC BBBBBBBB AAAAAAAA (order: 87654321)
A1: set - rental; unset - purchase
A2: set - play count; unset - time restrict
D8: set - auth fail
*/
#define VO_SOURCE_DRMINFO_FLAG_RENTAL					0x01
#define VO_SOURCE_DRMINFO_FLAG_PLAYCOUNT				0x02
#define VO_SOURCE_DRMINFO_FLAG_NOAUTH					0x80000000
typedef struct
{
	VO_U32				Flags;
	VO_U32				Param1;
	VO_U32				Param2;
	VO_U32				Reserved[5];
}VO_SOURCE_DRMINFO;

typedef VO_U32 (VO_API * VOSOURCEDRMCALLBACKFUNC)(VO_U32 nDrmType, VO_PTR pDrmInfo, VO_PTR pUserData, VO_U32 nReserved);
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
