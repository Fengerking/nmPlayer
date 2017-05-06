/** 
 * \file filebase.h
 * \brief Common constants, types, strucs of VisualOn file read/write SDK.
 * \version 0.1
 * \date 4/18/2007 moved VOAVCODEC & VOIMAGECODEC to sdkbase.h
 * \date 11/01/2006 created
 * \author VisualOn
 */


#ifndef __VO_FILEBASE_H_
#define __VO_FILEBASE_H_

#if defined(LINUX)
#	define __cdecl 
#endif

#include "sdkbase.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VOFILEAPI __cdecl

typedef void* HVOFILEREAD;
typedef void* HVOFILEWRITE;
typedef void* HVOFILETRACK;

/**
 * file type
 */
typedef enum
{
        FT_UNKNOWN				= 0X00000000,   /*!< Unknown File type*/
        FT_MOVIE				= 0X00000001,   /*!< Movie file type, it can contain video and (or) audio*/
        FT_AUDIO				= 0X00000002,   /*!< Audio file type*/
        FT_VIDEO				= 0X00000004,   /*!< Video file typet*/
        FT_IMAGE				= 0X00000008,   /*!< Video file typet*/
        FT_STREAM				= 0X00000010,   /*!< streaming file type*/
        FT_TEXT					= 0X00000020,   /*!< text file typet*/
        FT_LIVE					= 0X00000040,   /*!< Live (TV...) file type*/
}VOFILETYPE;

/**
 * file format
 */
typedef enum
{
        FF_UNKNOWN				= 0X00000000,   /*!< Unknown File Format*/
        FF_MOVIE_MP4			= 0X00000001,   /*!< MP4 File Format */
        FF_MOVIE_ASF			= 0X00000002,   /*!< MS ASF File Format */
        FF_MOVIE_AVI			= 0X00000004,   /*!< AVI File Format */
        FF_MOVIE_MOV			= 0X00000008,   /*!< QuickTime Movie File Format */
        FF_MOVIE_REAL			= 0X00000010,   /*!< Real File Format */
        FF_MOVIE_MPG			= 0X00000020,   /*!< Mpeg File Format */
        FF_MOVIE_TS				= 0X00000040,   /*!< TS File Format */
        FF_MOVIE_DV				= 0X00000080,   /*!< DV File Format */
        FF_AUDIO_AAC			= 0X00000100,   /*!< Audio AAC File Format */
        FF_AUDIO_AMR			= 0X00000200,   /*!< Audio AMR NB File Format */
        FF_AUDIO_AWB			= 0X00000400,   /*!< Audio AMR WB File Format */
        FF_AUDIO_MP3			= 0X00000800,   /*!< Audio MP3 File Format */
        FF_AUDIO_QCP			= 0X00001000,   /*!< Audio QCP (QCELP or EVRC) File Format */
        FF_AUDIO_WAV			= 0X00002000,   /*!< Audio WAVE File Format */
        FF_AUDIO_WMA			= 0X00004000,   /*!< Audio WMA File Format */
        FF_AUDIO_MIDI			= 0X00008000,   /*!< Audio MIDI File Format */
        FF_VIDEO_H264			= 0X00010000,   /*!< Video H264 File Format */
        FF_VIDEO_H263			= 0X00020000,   /*!< Video H263 File Format */
        FF_VIDEO_H261			= 0X00040000,   /*!< Video H261 File Format */
        FF_VIDEO_MPEG4			= 0X00080000,   /*!< Video Mpeg4 File Format */
        FF_VIDEO_MPEG2			= 0X00100000,   /*!< Video Mpeg2 File Format */
        FF_VIDEO_MPEG1			= 0X00200000,   /*!< Video Mpeg1 File Format */
        FF_VIDEO_MJPEG			= 0X00400000,   /*!< Video Motion Jpeg File Format */
        FF_VIDEO_RAWDATA		= 0X00800000,   /*!< Video raw data (RGB, YUV) Format */
        FF_IMAGE_JPEG			= 0X01000000,   /*!< Image JPEG File Format */
        FF_IMAGE_BMP			= 0X02000000,   /*!< Image BMP File Format */
        FF_IMAGE_GIF			= 0X04000000,   /*!< Image GIF File Format */
        FF_IMAGE_PNG			= 0X08000000,   /*!< Image PNG File Format */
        FF_IMAGE_TIFF			= 0X10000000,   /*!< Image TIFF File Format */
        FF_IMAGE_PCX			= 0X20000000,   /*!< Image PCX File Format */
        FF_IMAGE_J2000			= 0X40000000,   /*!< Image JPEG 2000 File Format */
		FF_STREAMING_RTSP       = 0X80000000,   /*!< RTSP Streaming */   
		FF_APPLICATION_SDP      = 0X80000001,   /*!< Session description */   
		FF_AUDIO_OGG			= 0x80000002,	/*!< Audio OGG File Format */ 
		FF_AUDIO_FLAC			= 0x80000004,	/*!< Audio FLAC File Format */ 
		FF_MOVIE_FLV			= 0x80000008,   /*!< FLV File Format */
}
VOFILEFORMAT;

/**
 * file read return code
 */
typedef enum
{
	VORC_FILEREAD_OK					= VORC_OK,					/*!< Successful    */
	VORC_FILEREAD_NULL_POINTER			= VORC_FILEREAD_BASE | 0x0001,	/*!< Null Pointer  */
	VORC_FILEREAD_NOT_IMPLEMENT			= VORC_FILEREAD_BASE | 0x0002,	/*!< this function is not implemented */
	VORC_FILEREAD_INDEX_NOT_EXIST		= VORC_FILEREAD_BASE | 0x0003,	/*!< media sample index is not exist*/
	VORC_FILEREAD_INDEX_END				= VORC_FILEREAD_BASE | 0x0004,	/*!< media sample reach end*/
	VORC_FILEREAD_NEED_RETRY			= VORC_FILEREAD_BASE | 0x0005,	/*!< not get media sample, but if you retry, it probably will be gotten*/
	VORC_FILEREAD_FILE_NOTEXIST			= VORC_FILEREAD_BASE | 0x0006,   /*!< Not Implement */
	VORC_FILEREAD_OPEN_FAILED			= VORC_FILEREAD_BASE | 0x0007,   /*!< Not Implement */
	VORC_FILEREAD_READ_FAILED			= VORC_FILEREAD_BASE | 0x0008,   /*!< Not Implement */
	VORC_FILEREAD_WRITE_FAILED			= VORC_FILEREAD_BASE | 0x0009,   /*!< Not Implement */
	VORC_FILEREAD_SEEK_FAILED			= VORC_FILEREAD_BASE | 0x000A,   /*!< Not Implement */
	VORC_FILEREAD_UNKNOWN_FILE_FORMAT   = VORC_FILEREAD_BASE | 0x000B,   /*!< Not Implement */
	VORC_FILEREAD_UNKNOWN_VIDEO_CODEC   = VORC_FILEREAD_BASE | 0x000C,   /*!< Not Implement */
	VORC_FILEREAD_UNKNOWN_AUDIO_CODEC   = VORC_FILEREAD_BASE | 0x000D,   /*!< Not Implement */
	VORC_FILEREAD_SAMPLE_DISCARDED		= VORC_FILEREAD_BASE | 0x000E,	/*!< Cannot get media sample because the media sample is discarded by reader */
	VORC_FILEREAD_CONTENT_ENCRYPT		= VORC_FILEREAD_BASE | 0x000F,	/*!< media content is encrypt, so we can not play this file*/
	VORC_FILEREAD_UNSUPPORT_FF_BF		= VORC_FILEREAD_BASE | 0x0010,	/*!< not support FF/BF */
	VORC_FILEREAD_UNKNOWN_FAILED		= VORC_FILEREAD_BASE | 0xFFFF,	/*!< Unknown error */
}
VOFILEREADRETURNCODE;

/**
 * Return value from file write functions
 */

typedef enum
{
	VORC_FILEWRITE_OK					= VORC_OK,					/*!< Successful    */
	VORC_FILEWRITE_NULL_POINTER			= VORC_FILEWRITE_BASE | 0x0001,	/*!< Null Pointer  */
	VORC_FILEWRITE_NOT_IMPLEMENT		= VORC_FILEWRITE_BASE | 0x0002,	/*!< this function is not implemented */
	VORC_FILEWRITE_OUT_OF_MEMORY        = VORC_FILEWRITE_BASE | 0x0003,  /*!< Out of memory */
	VORC_FILEWRITE_INDEX_NOT_EXIST		= VORC_FILEWRITE_BASE | 0x0004,	/*!< media sample index is not exist*/
    VORC_FILEWRITE_INTERNAL_BUILD       = VORC_FILEWRITE_BASE | 0x0005,    /*!< Internal build failed */ 
	VORC_FILEWRITE_OPEN_FAILED			= VORC_FILEWRITE_BASE | 0x0006,   /*!< Cannot open file, maybe no such file, or file in use etc. */
	VORC_FILEWRITE_WRITE_FAILED			= VORC_FILEWRITE_BASE | 0x0007,   /*!< General write file failed */
	VORC_FILEWRITE_DISK_FULL			= VORC_FILEWRITE_BASE | 0x000A,   /*!< Disk full */
	VORC_FILEWRITE_UNKNOWN_VIDEO_CODEC  = VORC_FILEWRITE_BASE | 0x0008,   /*!< Not support such video codec */
	VORC_FILEWRITE_UNKNOWN_AUDIO_CODEC  = VORC_FILEWRITE_BASE | 0x0009,   /*!< Not support such audio codec */
	VORC_FILEWRITE_UNKNOWN_FAILED		= VORC_FILEWRITE_BASE | 0xFFFF,	/*!< Unknown error */
}
VOFILEWRITERETURNCODE;



/**
 * file read state
*/
typedef enum
{
	VOS_RUN		= 0x0001,	/*!< run*/
	VOS_PAUSE	= 0x0002,	/*!< pause*/ 
	VOS_STOP	= 0x0003,	/*!< stop*/
}
VOFILEREADERSTATE;

/**
 * File Read parameter ID
 */
typedef enum
{
	VOID_FILEREAD_TRACKS				= VOID_FILEREAD_BASE | 0x0001,  /*!< track number in file */
	VOID_FILEREAD_SCANLEVEL				= VOID_FILEREAD_BASE | 0x0002,  /*!< scan file level when load file (0-100)*/
	VOID_FILEREAD_ID3					= VOID_FILEREAD_BASE | 0x0003,  /*!< ID3 info. VOID3INFO structure(voAudioFR.dll voID3Info**)*/
	VOID_FILEREAD_CODEC					= VOID_FILEREAD_BASE | 0x0004,  /*!< Codec id */
	VOID_FILEREAD_CODEC_NAME			= VOID_FILEREAD_BASE | 0x0005,  /*!< Codec FOUR CC */
	VOID_FILEREAD_STATE					= VOID_FILEREAD_BASE | 0x0006,	/*!< file reader's state, VOS_XXX*/
	VOID_FILEREAD_AVC_CONFIG			= VOID_FILEREAD_BASE | 0x0007,	/*!< avc decoder config information*/
	VOID_FILEREAD_BLOCKALIGN			= VOID_FILEREAD_BASE | 0x0008, 
	VOID_FILEREAD_AVGBYTESPERSEC		= VOID_FILEREAD_BASE | 0x0009, 
	VOID_FILEREAD_ACTION_STOP			= VOID_FILEREAD_BASE | 0x0010, 
	VOID_FILEREAD_ACTION_START			= VOID_FILEREAD_BASE | 0x0011,
	VOID_FILEREAD_ACTION_PAUSE			= VOID_FILEREAD_BASE | 0x0012, 
	VOID_FILEREAD_ACTION_RUN			= VOID_FILEREAD_BASE | 0x0013, 
	VOID_FILEREAD_ACTION_SEEK			= VOID_FILEREAD_BASE | 0x0014, 
	VOID_FILEREAD_CALLBACK_NOTIFY		= VOID_FILEREAD_BASE | 0x0015, 
	VOID_FILEREAD_BUFFERING_PROGRESS	= VOID_FILEREAD_BASE | 0x0016,
	VOID_FILEREAD_HEADER_SIZE			= VOID_FILEREAD_BASE | 0x0017, 
	VOID_FILEREAD_SEEKABLE_POS			= VOID_FILEREAD_BASE | 0x0018,	/*!< return current generated index position, LONGLONG*/
	VOID_FILEREAD_JUMP_KEYFRAME			= VOID_FILEREAD_BASE | 0x0019,
	VOID_FILEREAD_CURR_MAX_TIME			= VOID_FILEREAD_BASE | 0x0020,	/*!< <MS> return current generated index time, DWORD*/
	VOID_FILEREAD_UNVALID				= VOID_FILEREAD_BASE | 0x0021,  
	VOID_FILEREAD_BUFFER_LACK			= VOID_FILEREAD_BASE | 0x0022, 
	VOID_FILEREAD_FILE_FORMAT			= VOID_FILEREAD_BASE | 0x0023,  /*!< set file format, VOFILEFORMAT*/
	VOID_FILEREAD_ACTION_FF				= VOID_FILEREAD_BASE | 0x0024,  /*!< start/stop FF, DWORD 0-stop, else-start */
	VOID_FILEREAD_ACTION_BF				= VOID_FILEREAD_BASE | 0x0025,  /*!< start/stop BF, DWORD 0-stop, else-start */
	VOID_FILEREAD_FASTPLAY_TIME2INDEX	= VOID_FILEREAD_BASE | 0x0026,  /*!< fast play time to index, VOSAMPLEINFO* */
	VOID_FILEREAD_FIRST_FRAME			= VOID_FILEREAD_BASE | 0x0028,	/*!< get first frame data */
}
VOFILEREADPARAMETERID;

typedef int (* CALLBACKNOTIFY) (long EventCode, long * EventParam1, long * EventParam2);

/**
 * file operate handle.
 */
typedef void* HVOFILEOPERATE;

/**
 * File operate mode
 */
typedef enum
{
	FO_READ_ONLY				= 0X00000001,	/*!< Read file only*/
	FO_WRITE_ONLY				= 0X00000002,	/*!< Write file only*/
	FO_READ_WRITE				= 0X00000003,	/*!< Read and Write*/
	FO_READ_ONLY_NOBUF			= 0X00000004,	/*!< Read file only force to no buffer reader*/
	FO_WRITE_ONLY_NOBUF			= 0X00000005,	/*!< Write file only force to no buffer reader*/
	FO_READ_WRITE_NOBUF			= 0X00000006,	/*!< Read and Write force to no buffer reader*/
}VOFILEOPMODE;

/**
 * File seek position
 */
typedef enum
{
	FS_BEGIN					= 0X00000001,	/*!< seek the file pos from begin*/
	FO_CURRENT					= 0X00000002,	/*!< seek the file pos from current*/
	FO_END						= 0X00000003,	/*!< seek the file pos from end*/
}VOFILESEEKPOS;

/**
 * File Operations
 */
typedef struct
{
	HVOFILEOPERATE (__cdecl *voOpenFile)( const char *pFileName, VOFILEOPMODE opMode );					/*!< File open operation, return the file read handle. 0 failed */
	int (__cdecl *voSeekFile)( HVOFILEOPERATE hFile, long offset, VOFILESEEKPOS fsPos );				/*!< File seek operation. return -1 failed */
	int (__cdecl *voReadFile)( HVOFILEOPERATE hFile, void *buffer, int nSize, int * pReadSize );		/*!< File read operation. return 0 failed */
	int (__cdecl *voWriteFile)( HVOFILEOPERATE hFile, void *buffer, int nSize, int * pWriteWize );		/*!< File write operation. return 0 failed */
	int (__cdecl *voGetFileSize)( HVOFILEOPERATE hFile, int * pHighSize );								/*!< File get size operation. return file size. */
	int (__cdecl *voCloseFile)( HVOFILEOPERATE hFile );													/*!< File close operation. return 0 failed.*/
	int	(__cdecl *voGetFileActualSize) (HVOFILEOPERATE hFile, int* pHighSize);							/*!< File get size operation. return file actual size. */
}VOFILEOP;

/**
 * ID3 information
 */
typedef struct
{
	wchar_t * pTitle;										/*!< title */
	wchar_t * pArtist;										/*!< artist */
	wchar_t * pAlbum;										/*!< album */
	wchar_t * pYear;										/*!< year */
	wchar_t * pComment;										/*!< comment */
	wchar_t * pGenre;										/*!< genre */
	wchar_t * pTrack;										/*!< track */
	wchar_t * pComposer;									/*!< composer */
	wchar_t * pOriginal_artist;								/*!< original artist */
	wchar_t * pURL;											/*!< url */
}VOID3INFO;


/**
 * Track type
 */
typedef enum
{
	TT_VIDEO				= 0X00000001,	/*!< video track*/
	TT_AUDIO				= 0X00000002,	/*!< audio track*/
	TT_STRAM				= 0X00000003,	/*!< stream track*/
	TT_SCRIPT				= 0X00000004,	/*!< script track*/
	TT_HINT					= 0X00000005,	/*!< hint track*/
	TT_RTSP_VIDEO           = 0X00000006,   /*!< rtsp streaming video track*/
	TT_RTSP_AUDIO           = 0X00000007,   /*!< rtsp streaming audio track*/
	TT_SUBTITLE				= 0X00000008,   /*!< sub title track*/
	TT_TELETEXT				= 0X00000009,   /*!< tele text track*/
	TT_RICHMEDIA			= 0X0000000A,   /*!< rich media track*/
}VOTRACKTYPE;

/**
 * Track information
 */
typedef struct
{
	VOTRACKTYPE	  uType;						/*!< the type of track */
	VOAVCODEC	  uCodec;
	unsigned long uSampleCount;					/*!< samples in track */
	unsigned long uDuration;					/*!< duration of track (MS) */
	unsigned long uMaxSampleSize;				/*!< max size of sample */
}VOTRACKINFO;

/**
 * Sample information
 */
typedef struct
{
	unsigned char *	pBuffer;					/*!< buffer address of sample, null indicate not need get buffer*/
	unsigned long	uSize;						/*!< buffer size. the highest bit is key sample or not*/
	unsigned long	uTime;						/*!< start time (MS)*/
	unsigned long	uDuration;					/*!< Duration (MS) */
	unsigned long	uIndex;						/*!< index in track */
}VOSAMPLEINFO;

/**
 AVC configuration information
*/
typedef struct  
{
	unsigned char AVCProfileIndication;
	unsigned char AVCLevelIndication;
	unsigned char lengthSizeMinusOne;
}VOAVCCFGINFO;


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __VO_FILEBASE_H_ */

