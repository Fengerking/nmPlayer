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



#ifndef _VOADAPTIVESTREAMPARSER_H__

#define _VOADAPTIVESTREAMPARSER_H__

#include "voSource2.h"
#include "voDSType.h"
#include "voSource2_ProgramInfo.h"

#define VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_NEEDPARSEITEM	(VO_EVENTID_SOURCE2_BASE | VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_BASE | 0x0001)	/*!< Notify there is some item need to be parsed , Param1 will be VO_ADAPTIVESTREAM_PLAYLISTDATA* */
#define VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_PROGRAMCHANGED	(VO_EVENTID_SOURCE2_BASE | VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_BASE | 0x0002)	/*!< Notify program has been changed. Param1 shall be VO_SOURCE2_PROGRAM_INFO* */
#define VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_PROGRAMRESET	(VO_EVENTID_SOURCE2_BASE | VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_BASE | 0x0003)	/*!< Notify program has been Reset. Param1 shall be VO_SOURCE2_PROGRAM_INFO* */
#define VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_SEEK2LASTCHUNK	(VO_EVENTID_SOURCE2_BASE | VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_BASE | 0x0004)	/*!< Notify when seek to the last chunk of play list (NTS link without END tag need change to live mode) */

#define VO_PID_ADAPTIVESTREAMING_BASE							0x47100000
#define VO_PID_ADAPTIVESTREAMING_UTC							( VO_PID_ADAPTIVESTREAMING_BASE | 0x0001 )	/*!< <S> set the UTC time to adaptive stream parser VO_U64* */
#define VO_PID_ADAPTIVESTREAMING_STREAMTYPE						( VO_PID_ADAPTIVESTREAMING_BASE | 0x0002 )  /*!< <S> set the stream type  VO_ADAPTIVESTREAMPARSER_STREAMTYPE */
#define VO_PID_ADAPTIVESTREAMING_DVR_ENDTIME					( VO_PID_ADAPTIVESTREAMING_BASE | 0x0003 )	/*!< <G> Get the EndTime for DVR, VO_U64* */
#define VO_PID_ADAPTIVESTREAMING_DVR_WINDOWLENGTH				( VO_PID_ADAPTIVESTREAMING_BASE | 0x0004 )	/*!< <G> Get the DVRWindowLength for DVR which defined in spec, VO_U64* */
#define VO_PID_ADAPTIVESTREAMING_DVR_LIVETIME					( VO_PID_ADAPTIVESTREAMING_BASE | 0x0005 )	/*!< <G> Get the LiveTime for DVR, VO_U64* */
#define VO_PID_ADAPTIVESTREAMING_DVR_ENDLENGTH					( VO_PID_ADAPTIVESTREAMING_BASE | 0x0006 )	/*!< <G> Get the EndLength for DVR, it is a duration which is relative to the current downloading chunk.  VO_U64* */
#define VO_PID_ADAPTIVESTREAMING_DVR_LIVELENGTH					( VO_PID_ADAPTIVESTREAMING_BASE | 0x0007 )	/*!< <G> Get the LiveLength for DVR, it is a duration which is relative to the current downloading chunk. VO_U64* */
#define VO_PID_ADAPTIVESTREAMING_SWITCH_BACKWARDTIME			( VO_PID_ADAPTIVESTREAMING_BASE | 0x0008 )	/*!< <S> set the backward time before doing bitrate or track switch, VO_U64* the unit should be ms */
#define VO_PID_ADAPTIVESTREAMING_DVR_CHUNKWINDOWPOS				( VO_PID_ADAPTIVESTREAMING_BASE | 0x0009 )	/*!< <G> get the chunk's window position (ms) VO_ADAPTIVESTREAMPARSER_CHUNKWINDOWPOS*	*/
#define VO_PID_ADAPTIVESTREAMING_DVR_PROGRAMSTREAMTYPE			( VO_PID_ADAPTIVESTREAMING_BASE | 0x000A )	/*!< <G> get current program stream type for DVR, commonly VO_SOURCE2_STREAM_TYPE_LIVE, 
																													 VO_SOURCE2_STREAM_TYPE_VOD if converted to VOD, VO_SOURCE2_PROGRAM_TYPE*	*/
#define VO_PID_ADAPTIVESTREAMING_LIVELATENCY						( VO_PID_ADAPTIVESTREAMING_BASE | 0x000B )	/*!< <S> set the live latency value VO_U64* the unit is ms */

#define VO_PID_ADAPTIVESTREAMING_SEGMENTINDEX_INFO				( VO_PID_ADAPTIVESTREAMING_BASE | 0x000C )	/*!< identify the data is segment index information, VODS_SEG_INDEX_INFO* */
#define VO_PID_ADAPTIVESTREAMING_PERIOD2TIME					( VO_PID_ADAPTIVESTREAMING_BASE | 0x000D )	/*!< <S> convert the period sequence number to timestamp, VO_SOURCE2_PERIODTIMEINFO* */
#define VO_PID_ADAPTIVESTREAMING_INITDATA_INFO 					(VO_PID_ADAPTIVESTREAMING_BASE | 0x000E)    /*!< identify the data is Init data information, it contains ¡®Moov¡¯ & ¡®Sidx¡¯ offset, VODS_INITDATA_INFO* */
#define VO_PID_ADAPTIVESTREAMING_SWITCH_REF_ADUIO_THREAD			(VO_PID_ADAPTIVESTREAMING_BASE | 0x000F)   /*!<S> switch to audio thread as a reference for get chunk*/
#define VO_PID_ADAPTIVESTREAMING_UPDATEURL						(VO_PID_ADAPTIVESTREAMING_BASE | 0x0010) 		/*!<S> set updated URL into stream parser, now only for hls, VO_ADAPTIVESTREAM_PLAYLISTDATA* */
#define VO_PID_ADAPTIVESTREAMING_CHUNK_INFO						(VO_PID_ADAPTIVESTREAMING_BASE | 0x0011)		 /*!<S> set chunk info into stream parser when update url happen, now only for hls */

/**
*Param
*/
enum VO_ADAPTIVESTREAMPARSER_CHUNKFLAG
{
	VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE = 0x00000001,
	VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_SMOOTH_ADAPTION = 0x00000002,  /*!<This is obsolete, if not necssary please do not use this flag */
	VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_PROGRAMCHANGE = 0x00000004,	 /*!<Instead of VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_TIMESTAMPRECALCULATE, it indicates that Program maybe changed, it should reset fileparser*/
	VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_CHUNKDROPBEFORE = 0x00000008,		/*!add this flay if the trunk was drop before.*/
	//VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_TIMESTAMPRECALCULATE = 0x00000004,	/*!<Instead of VO_ADAPTIVESTREAMPARSER_STARTEX_FLAG_TIMESTAMPERECALCULATE, Indicate the timestamp has been changed */
	VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_CHUNKSWITCHED = 0x00000010,	/*!< Indicated that the track or bitrate switched started from this chunk */
	VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_SMOOTH_ADAPTIONEX = 0x00000020,	/*!< Indicated that start from this we will do smooth BA */
	VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_DISCONTINUE = 0x00000080,			/*!< Indicated that before this chunk there is a discontinue tag */
	VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_NEEDREF	= 0x00000100,			/*!< Indicated that this chunk may need the ref timestamp to recalculate the timestamp */

	VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_MAX = 0X7FFFFFFF,
};

enum VO_ADAPTIVESTREAMPARSER_STREAMTYPE
{
	VO_ADAPTIVESTREAMPARSER_STREAMTYPE_UNKOWN,
	VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS = 0x4701,
	VO_ADAPTIVESTREAMPARSER_STREAMTYPE_ISS,
	VO_ADAPTIVESTREAMPARSER_STREAMTYPE_DASH,

	VO_ADAPTIVESTREAMPARSER_STREAMTYPE_MAX = 0X7FFFFFFF,
};

/*enum VO_ADAPTIVESTREAMPARSER_CHUNKTYPE
{
	VO_ADAPTIVESTREAMPARSER_CHUNKTYPE_AUDIO,
	VO_ADAPTIVESTREAMPARSER_CHUNKTYPE_VIDEO,
	VO_ADAPTIVESTREAMPARSER_CHUNKTYPE_AUDIOVIDEO,
	VO_ADAPTIVESTREAMPARSER_CHUNKTYPE_HEADDATA,
	VO_ADAPTIVESTREAMPARSER_CHUNKTYPE_SUBTITLE,	
	VO_ADAPTIVESTREAMPARSER_CHUNKTYPE_UNKNOWN = 255,
}*/

enum VO_ADAPTIVESTREAMPARSER_SEEKMODE
{
	VO_ADAPTIVESTREAMPARSER_SEEKMODE_BACKWARD		= 1, 
	VO_ADAPTIVESTREAMPARSER_SEEKMODE_FORWARD		= 2, 
	VO_ADAPTIVESTREAMPARSER_SEEKMODE_OBSOLUTE		= 3, 
	VO_ADAPTIVESTREAMPARSER_SEEKMODE_DVRWINDOWPOS	= 4,	// position (ms) in DVR window
	VO_ADAPTIVESTREAMPARSER_SEEKMODE_TRACK_ONLY	    = 5,	// position (ms) within PlayList Duration
};

enum VO_ADAPTIVESTREAMPARSER_STARTEX_FLAG
{
	VO_ADAPTIVESTREAMPARSER_STARTEX_FLAG_TIMESTAMPERECALCULATE = 0x47470001,		/*!< Indicates start from this chunk, the timestamp has been changed */

	VO_ADAPTIVESTREAMPARSER_STARTEX_FLAG_MAX = 0X7FFFFFFF,
};

typedef struct  
{
	VO_CHAR		szRootUrl[MAXURLLEN];							/*!< The URL of parent playlist */
	VO_CHAR		szUrl[MAXURLLEN];								/*!< The URL of the playlist , maybe relative URL */
	VO_CHAR		szNewUrl[MAXURLLEN];							/*!< The URL after download( maybe redirect forever ), you should always use this url after get this struct */

	VO_PBYTE	pData;											/*!< The data in the playlist */
	VO_U32		uDataSize;										/*!< Playlist size */
	VO_U32		uFullDataSize;									/*!< Full Data size */

	VO_PTR					pReserve;
	VOS2_ProgramInfo_Func *	pProgFunc;
}VO_ADAPTIVESTREAM_PLAYLISTDATA;


struct VO_ADAPTIVESTREAMPARSER_STARTEXT
{
	VO_ADAPTIVESTREAMPARSER_STARTEX_FLAG	uFlag;
};

typedef struct
{
	VO_U32 uProgramID;
	VO_U32 uStreamID;
	VO_U32 uTrackID;
}VO_CHUNK_KEYID;

struct  VO_ADAPTIVESTREAMPARSER_CHUNK
{
	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE	Type;					/*!< The type of this chunk */	

	VO_CHAR								szRootUrl[MAXURLLEN];	/*!< The URL of manifest. It must be filled by parser. */
	VO_CHAR								szUrl[MAXURLLEN];		/*!< URL of this chunk , maybe relative URL */

	VO_U64								ullChunkOffset;			/*!< The download offset of the url, if this info is no avalible, please use INAVALIBLEU64 */
	VO_U64								ullChunkSize;			/*!< The download size of the url, if this info is no avalible, please use INAVALIBLEU64 */

	VO_U64								ullChunkLiveTime;		/*!< It is UTC time, it indicates if the chunk is live playback, when should this chunk play , if no such info the value should be INAVALIBLEU64 */
	VO_U64								ullChunkDeadTime;		/*!< It is UTC time, it indicates when the chunk is not avalible, if no such info the value should be INAVALIBLEU64 */

	VO_U64								ullStartTime;			/*!< The start offset time of this chunk , the unit of ( ullStartTime / ullTimeScale * 1000 ) should be ms */
	VO_ADAPTIVESTREAMPARSER_STARTEXT *	pStartExtInfo;			/*!< The extension info of the start time */

	VO_U64								ullDuration;			/*!< Duration of this chunk , the unit of ( ullDuration / ullTimeScale * 1000 ) should be ms */
	VO_U64								ullTimeScale;			/*!< TimeScale of this chunk */

	VO_U32								uFlag;					/*!< Flag to describe pFlagData */
	VO_PTR								pFlagData;				/*!< The Data based on uFlag */

	VO_PTR								pPrivateData;			/*!< The private data that needed to set back to parser */

	VO_PTR								pChunkDRMInfo;			/*!< DRM info */

	VO_CHUNK_KEYID						sKeyID;					/*!< The ID collection to identfy one chunk */
	VO_U64								uChunkID;				/*!< unique ID of chunk in playlist */

	VO_U32								uPeriodSequenceNumber;	/*!< the period sequence number */

	VO_PTR								pReserved;				/*!< Reseved */
};

struct  VO_ADAPTIVESTREAMPARSER_CHUNKWINDOWPOS
{
	VO_U32								uFlags;					/*!< <I> flags, reserved currently */
	VO_U64								uChunkID;				/*!< <I> unique ID of chunk in play list */
	VO_U64								ullWindowPosition;		/*!< <O> window position (ms) of this chunk */
};

struct VO_ADAPTIVESTREAMPARSER_INITPARAM
{
	VO_U32		uFlag;			/*not use right now*/
	VO_PTR 		pInitParam;		/*not use right now*/
	VO_TCHAR *   	strWorkPath;     /*!< Load library work path */
};

struct VO_ADAPTIVESTREAMPARSER_CHUNK_INFO
{
	VO_U32 uTrackID;
	VO_U64 uChunkID;
};


enum VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS
{
	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS_PRESENT = 0, /*!< sepcify adaptiveStreaming parser shall give the present Chunk*/
	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS_NEXT = 1,		/*!< sepcify adaptiveStreaming parser shall give the next Chunk*/	
};


struct VO_ADAPTIVESTREAMPARSER_TRACK_SEEK_PARAM
{
	VO_U32 uASTrackID;           /*!< sepcify the AS TrackId for Seeking*/
	VO_U64 ullOffsetTime;         /*!< sepcify position (ms) within PlayList Duration*/
};

typedef struct
{
/**
 * Initial a parser session
 * For this function please only copy the pData param, and cache the callback, do not do anything else
 * This function should be called first for a session.
 * \param phAdaptiveParser [out] parser handle.
 * \param pData [in] The playlist data..
 * \param pCallback [in] notify call back function pointer.
 */
	VO_U32 (VO_API * Init) ( VO_HANDLE * phAdaptiveParser , VO_ADAPTIVESTREAM_PLAYLISTDATA * pData , VO_SOURCE2_EVENTCALLBACK * pCallback , VO_ADAPTIVESTREAMPARSER_INITPARAM * pInitParam );

/**
 * Uninitial a parser session
 * \param hAdaptiveParser [in] parser handle.
 */
	VO_U32 (VO_API * UnInit) ( VO_HANDLE hAdaptiveParser );

/**
 * Parser should open and parse root playlist
 * \param hAdaptiveParser [in] parser handle.
 */
	VO_U32 (VO_API * Open) ( VO_HANDLE hAdaptiveParser );

/**
 * Destroy all the info with the playlist provided by open
 * \param hAdaptiveParser [in] parser handle.
 */
	VO_U32 (VO_API * Close) ( VO_HANDLE hAdaptiveParser );

/**
 * Parser should start to Parse sub playlist and start update thread if it is needed
 * \param hAdaptiveParser [in] parser handle.
 */
	VO_U32 (VO_API * Start) ( VO_HANDLE hAdaptiveParser );

/**
 * Stop update thread if it has
 * \param hAdaptiveParser [in] parser handle.
 */
	VO_U32 (VO_API * Stop) ( VO_HANDLE hAdaptiveParser );

/**
 * Update info with the playlist data provided
 * \param hAdaptiveParser [in] parser handle.
 * \param pData [in] playlist data
 */
	VO_U32 (VO_API * Update) ( VO_HANDLE hAdaptiveParser , VO_ADAPTIVESTREAM_PLAYLISTDATA * pData );

/**
 * GetChunk 
 * \param hAdaptiveParser [in] parser handle.
 * \param uID [in] it defines which media type i want to get, it only can be VO_SOURCE2_ADAPTIVESTREAMING_AUDIO( it must be audio ) or VO_SOURCE2_ADAPTIVESTREAMING_VIDEO( it can be video only or both video and audio )
 * \param ppItem [out] the Chunk info
 */
	VO_U32 (VO_API * GetChunk) ( VO_HANDLE hAdaptiveParser , VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID ,  VO_ADAPTIVESTREAMPARSER_CHUNK **ppChunk );

/**
 * Seek
 * \param hAdaptiveParser [in] The parser handle. Opened by Init().
 * \param pTimeStamp [in/out] The seek pos and return the available pos after seek
 */
	VO_U32 (VO_API * Seek) ( VO_HANDLE hAdaptiveParser , VO_U64 * pTimeStamp, VO_ADAPTIVESTREAMPARSER_SEEKMODE sSeekMode );

/**
 * Get Duration
 * \param hAdaptiveParser [in] The parser handle. Opened by Init().
 * \param pDuration [out] The duration of the stream
 */
	VO_U32 (VO_API * GetDuration) (VO_HANDLE hAdaptiveParser, VO_U64 * pDuration);

/**
 * It retrieve how many programs in the live stream
 * \param hAdaptiveParser [in] The parser handle. Opened by Init().
 * \param pProgramCount [out] The number of the streams
 */
	VO_U32 (VO_API * GetProgramCount) ( VO_HANDLE hAdaptiveParser, VO_U32 *pProgramCount );

/**
 * Enum all the program info based on program count
 * \param hAdaptiveParser [in] The parser handle. Opened by Init().
 * \param nProgram  [in] The program sequence based on the program counts
 * \param ppProgramInfo [out] return the info of the program, parser will keep the info structure available until you call close
 */
	VO_U32 (VO_API * GetProgramInfo) ( VO_HANDLE hAdaptiveParser, VO_U32 uProgram , VO_DATASOURCE_PROGRAM_INFO  **ppProgramInfo );

/**
 * Get the track info we selected
 * \param hAdaptiveParser [in] The parser handle. Opened by Init().
 * \param sTrackType [in] The sample type of the stream, it will be audio/video/closed caption
 * \param ppTrackInfo [out] The trackinfo of the selected track. The trackinfo memory will be maintained in source until you call close
 */

	VO_U32 (VO_API * GetCurTrackInfo) ( VO_HANDLE hAdaptiveParser, VO_SOURCE2_TRACK_TYPE sTrackType , VO_DATASOURCE_TRACK_INFO ** ppTrackInfo );

/**
 * Select the Program
 * \param hAdaptiveParser [in] The parser handle. Opened by Init().
 * \param nProgram [in] Program ID
 */
	VO_U32 (VO_API * SelectProgram) (VO_HANDLE hAdaptiveParser, VO_U32 uProgram);

/**
 * Select the Stream
 * \param hAdaptiveParser [in] The parser handle. Opened by Init().
 * \param nStream [in] Stream ID
 * \param sPrepareChunkPos [in] it defines which chunk parser should prepare for the stream change, next or current.
 */
	VO_U32 (VO_API * SelectStream) (VO_HANDLE hAdaptiveParser, VO_U32 uStream  , VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS sPrepareChunkPos );

/**
 * Select the Track
 * \param hAdaptiveParser [in] The parser handle. Opened by Init().
 * \param nTrack [in] Track ID
 */

	VO_U32 (VO_API * SelectTrack) (VO_HANDLE hAdaptiveParser, VO_U32 uTrack, VO_SOURCE2_TRACK_TYPE sTrackType);

/**
 * Get the DRM info
 * \param hAdaptiveParser [in] The parser handle. Opened by Init().
 * \param VO_SOURCE2_DRM_INFO [out] return the drm info, source will keep the info structure available until you call close
 */
	VO_U32 (VO_API * GetDRMInfo) ( VO_HANDLE hAdaptiveParser, VO_SOURCE2_DRM_INFO **ppDRMInfo );

/**
 * Get the special value from param ID
 * \param hAdaptiveParser [in] The parser handle. Opened by Init().
 * \param nParamID [in] The param ID
 * \param pParam [out] The get value depend on the param ID.
 */
	VO_U32 (VO_API * GetParam) ( VO_HANDLE hAdaptiveParser, VO_U32 nParamID, VO_PTR pParam );

/**
 * Set the special value from param ID
 * \param hAdaptiveParser [in] The parser handle. Opened by Init().
 * \param nParamID [in] The param ID
 * \param pParam [in] The set value depend on the param ID.
 */
	VO_U32 (VO_API * SetParam) ( VO_HANDLE hAdaptiveParser, VO_U32 nParamID, VO_PTR pParam );

} VO_ADAPTIVESTREAM_PARSER_API;


#endif
