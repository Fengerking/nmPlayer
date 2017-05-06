	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voLiveSource.h

	Contains:	data type define header file

	Written by:	Leon Huang

*******************************************************************************/

#ifndef __VOLIVESRCPLUS_H__
#define __VOLIVESRCPLUS_H__

#include "voYYDef_filcmn.h"
#include "voString.h"
#include "voSource.h"
#include "voAudio.h"
#include "voVideo.h"
#include "voParser.h"
#include "voSource2.h"

#ifdef _VONAMESPACE
	namespace _VONAMESPACE{
#else
	#ifdef __cplusplus
		extern "C" {
	#endif /* __cplusplus */
#endif // _VONAMESPACE

/**
* Parameter ID
* S - set; G - get; S/G - both set and get.
*/
#define	VO_PID_LIVESRC_PLUS_BASE						0x4A000000									/*!< the base param ID for source modules */
#define VO_PID_LIVESRC_PLUS_LIBOP						(VO_PID_LIVESRC_PLUS_BASE | 0x000f)              /*!< <G> Set the lib operator pointer, parem is VO_LIB_OPERATOR* */

/**
* Error code
*/
#define VO_ERR_LIVESRC_PLUS_OK				VO_ERR_NONE
#define VO_ERR_LIVESRC_PLUS_BASE				0x80000000
#define VO_ERR_LIVESRC_PLUS_FAIL				VO_ERR_LIVESRC_PLUS_BASE
#define VO_ERR_LIVESRC_PLUS_OUTOFMEMORY		(VO_ERR_LIVESRC_PLUS_BASE | 0x0001)		/*!< out of memory */
#define VO_ERR_LIVESRC_PLUS_NULLPOINTER		(VO_ERR_LIVESRC_PLUS_BASE | 0x0002)		/*!< null pointer */
#define VO_ERR_LIVESRC_PLUS_INVALIDARG		(VO_ERR_LIVESRC_PLUS_BASE | 0x0003)		/*!< invalid argument */
#define VO_ERR_LIVESRC_PLUS_LOADDRIVERFAILED	(VO_ERR_LIVESRC_PLUS_BASE | 0x0004)		/*!< load driver failed */
#define VO_ERR_LIVESRC_PLUS_NO_CHANNEL		(VO_ERR_LIVESRC_PLUS_BASE | 0x0005)		/*!< no channel */
#define VO_ERR_LIVESRC_PLUS_NO_DATA			(VO_ERR_LIVESRC_PLUS_BASE | 0x0006)		/*!< no data */
#define VO_ERR_LIVESRC_PLUS_NOIMPLEMENT		(VO_ERR_LIVESRC_PLUS_BASE | 0x0007)		/*!< feature not implemented */
#define VO_ERR_LIVESRC_PLUS_WRONG_STATUS		(VO_ERR_LIVESRC_PLUS_BASE | 0x0008)		/*!< the status was wrong */
#define VO_ERR_LIVESRC_PLUS_RETRY					(VO_ERR_LIVESRC_PLUS_BASE | 0x0009)		/*!< retry.   */


/**
* Status update code
*/
#define VO_LIVESRC_PLUS_STATUS_BASE					0x41000000							/*!< the base status code for source modules */
#define VO_LIVESRC_PLUS_STATUS_BASE_IIS_SS			(VO_LIVESRC_PLUS_STATUS_BASE | 0x0A00)	/*!< the base status code for IIS Smooth Streaming modules */

/**
* Live source output type,  
*/
typedef enum
{
	VO_LIVESRC_PLUS_OUTPUT_AUDIO		=	1,	/*!<  Output audio frame, see VO_LIVESRC_PLUS_SAMPLE */
	VO_LIVESRC_PLUS_OUTPUT_VIDEO		=	2,	/*!<  Output audio frame, see VO_LIVESRC_PLUS_SAMPLE */
	VO_LIVESRC_PLUS_OUTPUT_TRACKINFO	=	4,	/*!<  Output live source source, just like codec,head data..., see VO_LIVESRC_PLUS_TRACK_INFO */
}VO_LIVESRC_PLUS_OUTPUT_TYPE;

/**
 * Chunk type
 */
typedef enum
{
	VO_LIVESRC_PLUS_TT_VIDEO				= 0X00000001,	/*!< video chunk*/
	VO_LIVESRC_PLUS_TT_AUDIO				= 0X00000002,	/*!< audio chunk*/
	VO_LIVESRC_PLUS_TT_STREAM			= 0X00000003,   /*!< stream chunk(audio&video)*/
	VO_LIVESRC_PLUS_TT_SUBTITLE			= 0X00000004,   /*!< sub title track*/
	VO_LIVESRC_PLUS_TT_MAX					= VO_MAX_ENUM_VALUE
}VO_LIVESRC_PLUS_STREAMTYPE;



typedef struct
{
	VO_U32	nSampleRate;  /*!< Sample rate */
	VO_U32	nChannels;    /*!< Channel count */
	VO_U32	nSampleBits;  /*!< Bits per sample */
} VO_LIVESRC_PLUS_AUDIO_FORMAT;

typedef struct
{
	VO_U32				nWidth;		 /*!< Width */
	VO_U32				nHeight;		 /*!< Height */
} VO_LIVESRC_PLUS_VIDEO_FORMAT;

typedef struct
{
	VO_U32								nTrackID;				/*!<*/	
	VO_SOURCE_TRACKTYPE     nTrackType;          /*!< video/audio/subtitle*/
	VO_U32								nBitRate;
	VO_BYTE								strFourCC[8];
	VO_U32								nCodec;				/*!< codec type, VO_AUDIO_CODINGTYPE | VO_VIDEO_CODINGTYPE*/
	union
	{
		VO_LIVESRC_PLUS_AUDIO_FORMAT    stAudio_info;     // channel, bits, samplerate
		VO_LIVESRC_PLUS_VIDEO_FORMAT     stVideo_info;     // width, height
	};
	VO_PBYTE						 HeadData;       /*!< sequence data.*/
	VO_U32							 nHeadSize;       /*!< sequence data size*/
}VO_LIVESRC_PLUS_TRACK_INFO;

typedef struct  
{
	VO_LIVESRC_PLUS_STREAMTYPE	nStreamType; /*audio/video/audio&video*/
	VO_BYTE										strSubType[8];
	VO_BYTE										strBaseUrl[1024];
	VO_BYTE										strStreamName[256];
	VO_U32										nBandWidth;					//this is only used when streamtype is VO_LIVESRC_PLUS_TT_STREAM
	VO_U32										nDuration;
	VO_U32										nChunkCounts; 

	VO_U32										nTrackCounts;
	VO_LIVESRC_PLUS_TRACK_INFO **ppTrackInfo; /* count indicated by nTrackNumber*/
}VO_LIVESRC_PLUS_STREAM_INFO;


/*playready sample enc box*/
typedef struct 
{
	VO_PBYTE	pEncBoxBuffer;
	VO_U32		nDwSampleEncBoxLen;
}VO_PLAYREADY_SAMPLEENC_BOX;

/*playready sample enc box*/
#ifndef _ENTRIES_ST
#define _ENTRIES_ST
typedef struct 
{
	VO_U16    nBytesOfClearData;
	VO_U32	 nBytesOfEncrytedData;
}ENTRIES_ST;
#endif

typedef struct 
{
	VO_U32		nNumberOfEntries;
	ENTRIES_ST *pEntries;

	VO_PBYTE pIV;//InitializationVector
	VO_U32    nIVsize;
	VO_U32    nBlockOffset;
	VO_U32	 btByteOffset;
}VO_PLAYREADY_ENCRYPTION;

/**
* Audio Video sample
* Send out by SendData callback
*/
#define  VO_PID_FLAG_PIFF			0x	00000001 /*<to indicate pReserve is VO_PLAYREADY_ENCRYPTION* */
typedef struct
{
	VO_PBYTE					pData;			/*!< the frame data pointer */
	VO_U32						nSize;			/*!< the frame data size */
	VO_U64						nStartTime;		/*!< the frame start time */
	VO_U64						nEndTime;		/*!< the frame end time */
	VO_U32						nFrameType;		/*!< the frame type, 0 key frame, others normal frame , refer to VO_VIDEO_FRAMETYPE*/
	VO_U32						nCodecType;		/*!< the frame codec type, refer to VO_VIDEO_CODINGTYPE, VO_AUDIO_CODINGTYPE */

	VO_U32						nFlag; /*!< to indicate pReserve type */
	VO_VOID						*pReserve;
	VO_VOID						*pReserve1;
	VO_VOID						*pReserve2;
}VO_LIVESRC_FRAMEBUFFER;


typedef enum
{
	VO_LIVESRC_PLUS_MEDIA_PURE_AUDIO,
	VO_LIVESRC_PLUS_MEDIA_PURE_VIDEO,
	VO_LIVESRC_PLUS_MEDIA_AUDIOVIDEO,
}VO_LIVESRC_PLUS_MEDIA_TYPE;

	
struct  FragmentItem
{
	VO_S32	index;	
	VO_S32	streamindex;
	VO_U64	duration;
	VO_U64	starttime;
	VO_U32    nIsVideo;//0 audio ,1 video
	FragmentItem * ptr_next;
};

typedef struct
{
	VO_PTR pUserData;
	VO_S32 (VO_API *SendEvent) (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);
	VO_S32 (VO_API *SendData) (VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData);
}VO_LIVESRC_CALLBACK;

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
VO_U32 (VO_API * Init) (VO_HANDLE * phLiveSrc, VO_LIVESRC_CALLBACK *pCallback);

VO_U32 (VO_API * Uninit) (VO_HANDLE  hLiveSrc);

/**
* PARSER Manifest buffer. It is used for PUSH mode.  Live(ISS,DASH,etc) module change to a parser module
* \param hLiveSrc [in] The live source handle. Opened by Init().
* \param pData [in] HeadInfo data pushed this buffer from outside(Manifest for ISS, It must be a complete xml buffer).
* \param uSize [in]. indicate the length of pData
*/
VO_U32 (VO_API * ManifestParser) (VO_HANDLE hLiveSrc, VO_PBYTE pData, VO_U32 uSize);
/**
* PARSER Header buffer. It is used for PUSH mode.  Live(ISS,DASH,etc) module change to a parser module
* \param hLiveSrc [in] The live source handle. Opened by Init().
* \param pData [in] HeadInfo data pushed this buffer from outside(Manifest for ISS, It must be a complete xml buffer).
* \param uSize [in]. indicate the length of pData
*/
VO_U32 (VO_API * HeaderParser) (VO_HANDLE hLiveSrc, VO_PBYTE pData, VO_U32 uSize);
/**
* PARSER one Chunk(Fragment & Segment & chunk) . It is used for PUSH mode.  Live(ISS,HLS,DASH,etc) module change to a parser module
* \param hLiveSrc [in] The live source handle. Opened by Init().
* \param type [in] indicate streamtype of buffer. 
* \param pData [in] Manifest data pushed this buffer from outside(fragment for ISS, It must be a complete fragment buffer now).
* \param uSize [in]. indicate the length of pData
*/
VO_U32 (VO_API * ChunkParser) (VO_HANDLE hLiveSrc, VO_SOURCE_TRACKTYPE trackType,VO_PBYTE pData, VO_U32 uSize);

/**
 * Get the special value from param ID
 * This func must be called by the end of SendDataSource.
 * \param hLiveSrc [in] The live source handle. Opened by Init().
 * \param pStreamCounts [out] The return of the number of streams in base manifest.
 */
VO_U32 (VO_API * GetStreamCounts) (VO_HANDLE hLiveSrc, VO_U32 *pStreamCounts);

/**
 * Get the special value from param ID
 * This func must be called by the end of SendDataSource.
 * \param hLiveSrc [in] The live source handle. Opened by Init().
 * \param nStreamID [in] The Stream ID, the number of stream is obtained by GetStreamCounts()
 * \param ppStreamInfo [out] The return value depend on the Stream ID.
 */
VO_U32 (VO_API * GetStreamInfo) (VO_HANDLE hLiveSrc, VO_U32 nStreamID, VO_SOURCE2_STREAM_INFO  **ppStreamInfo);
VO_U32 (VO_API * GetPlayList) (VO_HANDLE hLiveSrc, FragmentItem **ppPlaylist);

/**
 * Get the special value from param ID
 * This func must be called by the end of SendDataSource.
 * \param hLiveSrc [in] The live source handle. Opened by Init().
 * \param ppDRMInfo [out] The DRMInfo struct. If no DRMInfo , it will be returned NULL.
 */
VO_U32 (VO_API * GetDRMInfo) (VO_HANDLE hLiveSrc, VO_SOURCE2_DRM_INFO **ppDRMInfo);

/**
 * Get the special value from param ID
 * \param hLiveSrc [in] The live source handle. Opened by Init().
 * \param nParamID [in] The param ID
 * \param pParam [out] The return value depend on the param ID.
 */
VO_U32 (VO_API * GetParam) (VO_HANDLE hLiveSrc, VO_U32 nParamID, VO_PTR pParam);

/**
 * Set the special value from param ID
 * param hLiveSrc [in] The live source handle. Opened by Init().
 * \param nParamID [in] The param ID
 * \param pParam [in] The set value depend on the param ID.
 */
VO_U32 (VO_API * SetParam) (VO_HANDLE hLiveSrc, VO_U32 nParamID, VO_PTR pParam);

} VO_LIVESRC_PLUS_API;

/**
* Get Live Source API interface
* \param pHandle [IN/OUT] Return the live source API handle.
* \retval VO_ERR_LIVESRC_PLUS_OK Succeeded.
*/
#ifdef _ISS_SOURCE
VO_S32 VO_API voGetISSLiveSrcPlusAPI(VO_LIVESRC_PLUS_API* pHandle);
#else
VO_S32 VO_API voGetLiveSrcPlusAPI(VO_LIVESRC_PLUS_API* pHandle);
#endif
//#endif // _HLS_SOURCE_

typedef VO_U32 (VO_API * VOGETLIVESRCPLUSAPI) (VO_LIVESRC_PLUS_API* pHandle);


#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // _VONAMESPACE

#endif // __VOLIVESRC_H__
