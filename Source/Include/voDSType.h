
//value indicate for DATA SOURCE TEAM 
#ifndef __VO_DSTYPE_H__
#define __VO_DSTYPE_H__

#include "voSource2.h"
//define all event func and flag for DATA SOURCE
#include "voDSEvent.h"

#include "voDSDRM.h"
//common function
#define VODS_MIN(a,b)      (((a) < (b)) ? (a) : (b))
#define VODS_MAX(a,b)      (((a) > (b)) ? (a) : (b))
#if 1
#define _VODS_CHECK_NULL_POINTER_ONE_(a, result)			if(!(a)){return (result);}
#define _VODS_CHECK_NULL_POINTER_TWO_(a, b,result)		if(!(a) ||!(b)){return (result);}
#define _VODS_CHECK_NULL_POINTER_THREE_(a , b, c, result)		if(!(a) ||!(b) ||!(c)){return (result);}

#define _VODS_CHECK_NULL_POINTER_VOID_ONE_(a)			if(!(a)){return;}
#define _VODS_CHECK_NULL_POINTER_VOID_TWO_(a,b)		if(!(a)||!(b)){return;}
#define _VODS_CHECK_NULL_POINTER_VOID_THREE_(a,b,c)		if(!(a)||!(b)||!(c)){return;}
#endif

/* 2013/08/14 Leon. each sample should be timestamp recalculate. So uncomment these define.*/
//#define _TIMERECALCULATE_LIVEandVOD
//#define _TIMERECALCULATE_DASH
//#define _TIMERECALCULATE_HLS
//#define _TIMERECALCULATE_ISS

//constant
#define _VODS_INT64_MAX 0xffffffffffffffffll
#define _VODS_INT32_MAX 0xffffffff
#define _VODS_INT16_MAX 0xffff
#define _VODS_INT8_MAX	0xff
#define _VODS_INT4_MAX	0xf
#define _VODS_SIZE_1M   1024 * 1024

#define __INTERNALSUBTITLE 12345678
#define INAVALIBLEU64 _VODS_INT64_MAX





//uDataSource_Flag for sample in VO_DATASOURCE_SAMPLE. It is a 64 bit parameter.
//chunk flag use 1 ~16 bit.
#define VO_DATASOURCE_FLAG_CHUNK_BEGIN						0x00000001 
#define VO_DATASOURCE_FLAG_CHUNK_END						0x00000002 
#define VO_DATASOURCE_FLAG_FORCEOUTPUT						0x00000004 
#define VO_DATASOURCE_FLAG_CHUNK_SWITCH						0x00000008 
#define VO_DATASOURCE_FLAG_CHUNK_TRACKTYPE_UNAVAILABLE		0x00000010  /*!< Indicate that this tracktype is not available. VO_SOURCE2_TT_AUDIO, VO_SOURCE2_TT_VIDEO, VO_SOURCE2_TT_SUBTITLE*/
#define VO_DATASOURCE_FLAG_EOS								0x00000020  /*!< Indicate that the stream has EOS.*/
#define VO_DATASOURCE_FLAG_DISCONTINUE					0x00000040 /*!<Indicate first sample of discontinue chunk. */
//track flag use 17 ~ 32 bit.
#define VO_DATASOURCE_FLAG_TRACK_LOCKED_FOLLOWING			0x00000001 <<16	/*!<Indicate the following sample of this track shall be locked,  do not be output. include this sample.*/
#define VO_DATASOURCE_FLAG_TRACK_UNLOCKED_PREVIOUS			0x00000002 <<16	/*!<Indicate the previous sample of this track shall be unlocked, include this sample.*/



#define VO_DATASOURCE_RET_OK					VO_RET_SOURCE2_OK
#define VO_DATASOURCE_RET_BASE					(VO_RET_SOURCE2_DS_BASE)
#define VO_DATASOURCE_RET_SHOULDSKIP			(VO_DATASOURCE_RET_BASE | 0x00000001)
#define VO_DATASOURCE_RET_DOWNLOAD_SLOW			(VO_DATASOURCE_RET_BASE | 0x00000002)


#define VO_DATASOURCE_PID_BASE 					(VO_PID_SOURCE2_BASE    | 0x00a00000)
#define VO_DATASOURCE_PID_SHARELIB_BASE			(VO_DATASOURCE_PID_BASE | 0x000a0000)
#define VO_DATASOURCE_PID_DRM_BASE				(VO_DATASOURCE_PID_BASE | 0x000b0000)

#define VO_DATASOURCE_PID_STREAMCOUNT			(VO_DATASOURCE_PID_BASE | 0x00000001)


//operation mark OP Type, 
enum VO_DATASOURCE_MARKOP_TYPE
{
	 VO_DATASOURCE_MARKOP_BASE								=	0X00000000,
	 VO_DATASOURCE_MARKOP_CONSTRUCTPROGRAMINFO_ADDBEGIN		=	0X00000001,
	 VO_DATASOURCE_MARKOP_CONSTRUCTPROGRAMINFO_ADD			=	0X00000002,
	 VO_DATASOURCE_MARKOP_CONSTRUCTPROGRAMINFO_ADDEND		=	0X00000003,
};


enum VO_DATASOURCE_BUFFER_TYPE
{
	VO_DATASOURCE_BUFFER_TYPE_DATA	= 1,		/*!< not implement now*/
	VO_DATASOURCE_BUFFER_TYPE_SAMPLE	= 2,	/*!< VO_DATASOURCE_SAMPLE* */
};

enum THREADTYPE
{
	THREADTYPE_MEDIA_AUDIO = 0,
	THREADTYPE_MEDIA_VIDEO = 1,
	THREADTYPE_MEDIA_SUBTITLE = 2,
	THREADTYPE_MEDIA_MAX = 0x7fffffff,
};

#define VO_DATASOURCE_MARKFLAG_RESET						0X00000001	/*!< it is for */
#define VO_DATASOURCE_MARKFLAG_ADD							0X00000002	
#define VO_DATASOURCE_MARKFLAG_PROGRAMINFO					0X00000004

//track type
enum VO_DATASOURCE_TRACK_TYPE
{
	VO_DATASOURCE_TT_AUDIO				= 0X00000001,	/*!< video track*/
	VO_DATASOURCE_TT_VIDEO				= 0X00000002,	/*!< audio track*/
	VO_DATASOURCE_TT_IMAGE				= 0X00000003,	/*!< image track*/
	VO_DATASOURCE_TT_STREAM				= 0X00000004,	/*!< stream track*/
	VO_DATASOURCE_TT_SCRIPT				= 0X00000005,	/*!< script track*/
	VO_DATASOURCE_TT_HINT				= 0X00000006,	/*!< hint track*/
	VO_DATASOURCE_TT_RTSP_VIDEO			= 0X00000007,   /*!< rtsp streaming video track*/
	VO_DATASOURCE_TT_RTSP_AUDIO			= 0X00000008,   /*!< rtsp streaming audio track*/
	VO_DATASOURCE_TT_SUBTITLE			= 0X00000009,   /*!< sub title track & closed caption*/
	VO_DATASOURCE_TT_RICHMEDIA			= 0X0000000B,   /*!< rich media track*/
	VO_DATASOURCE_TT_TRACKINFO			= 0X0000000C,	/*!< track info VO_DATASOURCE_TRACK_INFO* */
	VO_DATASOURCE_TT_CUSTOMTIMEDTAG		= 0X0000000D,   /*!< custom timed tag VO_DATASOURCE_CUSTOMERTAG_TIMEDTAG* */
	VO_DATASOURCE_TT_AUDIOGROUP			= 0X0000000E,	/*!< if donot know how many audio in, set it*/
	VO_DATASOURCE_TT_VIDEOGROUP		= 0X0000000F,	/*!< if donot know how many video in, set it*/
	VO_DATASOURCE_TT_SUBTITLEGROUP		= 0X00000010,	/*!< if donot know how many subtitle in ,set it*/
	VO_DATASOURCE_TT_MUXGROUP			= 0X00000011,	/*!< if you donot know anything ,set it*/

	VO_DATASOURCE_TT_CHUNKINFO			= _VODS_INT16_MAX-1, /*!< CHUNK INFORMATION */
	VO_DATASOURCE_TT_MAX				= VO_MAX_ENUM_VALUE
};



typedef struct
{
	VO_U32						uFPTrackID;				/*!< the track id created by our parser, it is unique in this source session*/
	VO_U32						uASTrackID;				/*!< the track id created by our adaptive streaming parser,it is unique in this source session*/
	VO_U32						uOutSideTrackID;		/*!< for outside, and it is set by ProgramInfoOp */

	VO_U32						uSelInfo;				/*!< Indicated if the track is selected and recommend or default */

	VO_BYTE						strFourCC[8];
	VO_U32						uTrackType;				/*!< video/audio/subtitle */
	VO_U32						uCodec;					/*!< codec type, VO_AUDIO_CODINGTYPE | VO_VIDEO_CODINGTYPE */

	VO_U64						uDuration;				/*!< duration of this track */
	VO_U32						uChunkCounts;			/*!< chunks in the track */

	VO_U32						uBitrate;				/*!< Bitrate of this track */

	union
	{
		VO_SOURCE2_AUDIO_INFO     sAudioInfo;			/*!< audio info */
		VO_SOURCE2_VIDEO_INFO     sVideoInfo;			/*!< video info */
		VO_SOURCE2_SUBTITLE_INFO  sSubtitleInfo;		/*!< subtitle info */
	};

	VO_U32						uHeadSize;				/*!< sequence data size*/
	VO_PBYTE					pHeadData;				/*!< Header data pointor */
}VO_DATASOURCE_TRACK_INFO;

typedef struct  
{
	VO_U32						uStreamID;				/*!< the stream id created by our parser, it is unique in this source session */

	VO_U32						uSelInfo;				/*!< Indicated if the Stream is selected and recommend or default */

	VO_U32						uBitrate;				/*!< the bitrate of the stream */

	VO_U32						uTrackCount;			/*!< track count if this sub stream */
	VO_DATASOURCE_TRACK_INFO		**ppTrackInfo;			/*!< Track info */
}VO_DATASOURCE_STREAM_INFO;

typedef struct  
{
	VO_U32						uProgramID;				/*!< the stream id created by our parser, it is unique in this source session */

	VO_U32						uSelInfo;				/*!< Indicated if the Program is selected and recommend or default */

	VO_SOURCE2_PROGRAM_TYPE		sProgramType;			/*!< Indicate if the Program is live or vod */
	VO_CHAR						strProgramName[256];	/*!< Name of the Program */

	VO_U32						uStreamCount;			/*!< Stream Count */
	VO_DATASOURCE_STREAM_INFO		**ppStreamInfo;			/*!< Stream info */	
}VO_DATASOURCE_PROGRAM_INFO;

/* it will be used in future*/
typedef struct
{	
	VO_U32			uFPTrackID;				/*!< the FP track ID*/
	VO_U32			uASTrackID;				/*!< the AS track ID*/
	VO_U32			uThreadID;				/*!< the sample thread ID*/
	VO_U64			uTime;					/*!< start time(MS)*/
	VO_U32			uDuration;				/*!< duration of sample(MS)*/

	VO_U32			uMarkOP;				/*!< this flag is marked how to implament/operator/deal this sample, it has drop/add/ and other infomation */
	VO_U32			uMarkFlag;				/*!< this flag is marked how to implament/operator/deal this sample, it has drop/add/ and other infomation */

	VO_U32			uFlag;			/*!< the flag that indicate the status of the sample,like key frame,codec change from this sample etc. it should use the flag define with VO_SOURCE2_FLAG_SAMPLE prefix*/
#ifdef _USE_BUFFERCOMMANDER
	VO_U64			uDataSource_Flag;		/*!< this is a flag only for source.*/
#endif
	VO_PTR			pFlagData;				/*!< If the the uFlag contains VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT, this field is pointed to VO_SOURCE2_TRACK_INFO* and pBuffer must be NULL */

	VO_U32			uSize;					/*!< buffer size.*/
	VO_PBYTE		pBuffer;				/*!< buffer address of sample, null indicate not need get buffer*/

	VO_U64			uChunkID;				/*!< unique ID of chunk in playlist */
	
	VO_VOID			*pChunkAgent;
	VO_VOID			*pReserve1;
	VO_VOID			*pReserve2;

}VO_DATASOURCE_SAMPLE;


#ifdef _new_programinfo
typedef		VO_DATASOURCE_PROGRAM_INFO		_PROGRAM_INFO;		
typedef		VO_DATASOURCE_TRACK_INFO		_TRACK_INFO;			
typedef		VO_DATASOURCE_STREAM_INFO		_STREAM_INFO;
typedef     VO_DATASOURCE_SAMPLE			_SAMPLE;
#else

#ifndef __PROGRAMINFO
#define __PROGRAMINFO
typedef		VO_SOURCE2_PROGRAM_INFO		_PROGRAM_INFO;		
typedef		VO_SOURCE2_TRACK_INFO		_TRACK_INFO;			
typedef		VO_SOURCE2_STREAM_INFO		_STREAM_INFO;
typedef     VO_SOURCE2_SAMPLE			_SAMPLE;
#endif
#endif

#endif