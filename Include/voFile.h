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

#ifndef __voFile_H__
#define __voFile_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voSource.h"

/**
* Parameter ID
* S - set; G - get; S/G - both set and get.
*/
#define	VO_PID_FILE_BASE						0x44000000						/*!< the base param ID for file parser */
#define	VO_PID_FILE_SCANLEVEL					(VO_PID_FILE_BASE | 0x0010)		/*!< <S>set the file scan level */
#define VO_PID_FILE_FILEPOS2MEDIATIME			(VO_PID_FILE_BASE | 0x0020)		/*!< <G>get the media time<MS> by input file position, VO_FILE_MEDIATIMEFILEPOS */
#define VO_PID_FILE_MEDIATIME2FILEPOS			(VO_PID_FILE_BASE | 0x0021)		/*!< <G>get the file position by input media time<MS>, VO_FILE_MEDIATIMEFILEPOS */
#define VO_PID_FILE_GetSEEKPOSBYTIME			(VO_PID_FILE_BASE | 0x0022)		/*!< <G>get the seek position by input media time<MS>, VO_FILE_MEDIATIMEFILEPOS */
#define VO_PID_FILE_HEADSIZE					(VO_PID_FILE_BASE | 0x0030)		/*!< <G>get the file head size, (only enough information for playback, usually used for PD), VO_U32 */
#define VO_PID_FILE_HEADDATAINFO				(VO_PID_FILE_BASE | 0x0031)		/*!< <G>get the file head data info, headerdata_info */
#define VO_PID_FILE_GETAVSTARTPOS				(VO_PID_FILE_BASE | 0x0032)		/*!< <G>get the file head data info, av_startpos */
#define VO_PID_FILE_GETVIDEOMATRIX				(VO_PID_FILE_BASE | 0x0033)		/*!< <G>get the movie matrix info, video_matrix */
#define VO_PID_FILE_GETDECSPECDATA				(VO_PID_FILE_BASE | 0x0034)		/*!< <G>get decoder specific data, dec_spec_data */
#define VO_FILE_TYPE_NAME						0x1
#define VO_FILE_TYPE_HANDLE						0x2
#define VO_FILE_TYPE_ID							0x3
#define VO_FILE_TYPE_IMFBYTESTREAM				0x4

/**
 * file type
 */
typedef enum
{
	VO_FILE_FT_UNKNOWN				= 0X00000000,   /*!< Unknown File type*/
	VO_FILE_FT_MOVIE				= 0X00000001,   /*!< Movie file type, it can contain video and (or) audio*/
	VO_FILE_FT_AUDIO				= 0X00000002,   /*!< Audio file type*/
	VO_FILE_FT_VIDEO				= 0X00000004,   /*!< Video file type*/
	VO_FILE_FT_IMAGE				= 0X00000008,   /*!< Image file type*/
	VO_FILE_FT_STREAM				= 0X00000010,   /*!< streaming file type*/
	VO_FILE_FT_TEXT					= 0X00000020,   /*!< text file type*/
	VO_FILE_FT_LIVE					= 0X00000040,   /*!< Live (TV...) file type*/
	VO_FILE_FT_MAX					= VO_MAX_ENUM_VALUE
}VO_FILE_TYPE;

/**
 * file format
 */
typedef enum
{
    VO_FILE_FFUNKNOWN			= 0X00000000,   /*!< Unknown File Format*/
    VO_FILE_FFMOVIE_MP4			= 0X00000001,   /*!< MP4 File Format */
    VO_FILE_FFMOVIE_ASF			= 0X00000002,   /*!< MS ASF File Format */
    VO_FILE_FFMOVIE_AVI			= 0X00000004,   /*!< AVI File Format */
    VO_FILE_FFMOVIE_MOV			= 0X00000008,   /*!< QuickTime Movie File Format */
    VO_FILE_FFMOVIE_REAL		= 0X00000010,   /*!< Real File Format */
    VO_FILE_FFMOVIE_MPG			= 0X00000020,   /*!< Mpeg File Format */
    VO_FILE_FFMOVIE_TS			= 0X00000040,   /*!< TS File Format */
    VO_FILE_FFMOVIE_DV			= 0X00000080,   /*!< DV File Format */
    VO_FILE_FFAUDIO_AAC			= 0X00000100,   /*!< Audio AAC File Format */
    VO_FILE_FFAUDIO_AMR			= 0X00000200,   /*!< Audio AMR NB File Format */
    VO_FILE_FFAUDIO_AWB			= 0X00000400,   /*!< Audio AMR WB File Format */
    VO_FILE_FFAUDIO_MP3			= 0X00000800,   /*!< Audio MP3 File Format */
    VO_FILE_FFAUDIO_QCP			= 0X00001000,   /*!< Audio QCP (QCELP or EVRC) File Format */
    VO_FILE_FFAUDIO_WAV			= 0X00002000,   /*!< Audio WAVE File Format */
    VO_FILE_FFAUDIO_WMA			= 0X00004000,   /*!< Audio WMA File Format */
    VO_FILE_FFAUDIO_MIDI		= 0X00008000,   /*!< Audio MIDI File Format */
    VO_FILE_FFVIDEO_H264		= 0X00010000,   /*!< Video H264 File Format */
    VO_FILE_FFVIDEO_H263		= 0X00020000,   /*!< Video H263 File Format */
    VO_FILE_FFVIDEO_H261		= 0X00040000,   /*!< Video H261 File Format */
    VO_FILE_FFVIDEO_MPEG4		= 0X00080000,   /*!< Video Mpeg4 File Format */
    VO_FILE_FFVIDEO_MPEG2		= 0X00100000,   /*!< Video Mpeg2 File Format */
    VO_FILE_FFVIDEO_MPEG1		= 0X00200000,   /*!< Video Mpeg1 File Format */
    VO_FILE_FFVIDEO_MJPEG		= 0X00400000,   /*!< Video Motion Jpeg File Format */
    VO_FILE_FFVIDEO_RAWDATA		= 0X00800000,   /*!< Video raw data (RGB, YUV) Format */
    VO_FILE_FFIMAGE_JPEG		= 0X01000000,   /*!< Image JPEG File Format */
    VO_FILE_FFIMAGE_BMP			= 0X02000000,   /*!< Image BMP File Format */
    VO_FILE_FFIMAGE_GIF			= 0X04000000,   /*!< Image GIF File Format */
    VO_FILE_FFIMAGE_PNG			= 0X08000000,   /*!< Image PNG File Format */
    VO_FILE_FFIMAGE_TIFF		= 0X10000000,   /*!< Image TIFF File Format */
    VO_FILE_FFIMAGE_PCX			= 0X20000000,   /*!< Image PCX File Format */
    VO_FILE_FFIMAGE_J2000		= 0X40000000,   /*!< Image JPEG 2000 File Format */
	VO_FILE_FFSTREAMING_RTSP    = 0X80000000,   /*!< RTSP Streaming */
	VO_FILE_FFAPPLICATION_SDP   = 0X80000001,   /*!< Session description */
	VO_FILE_FFAUDIO_OGG			= 0x80000002,	/*!< Audio OGG File Format */
	VO_FILE_FFAUDIO_FLAC		= 0x80000004,	/*!< Audio FLAC File Format */
	VO_FILE_FFMOVIE_FLV			= 0x80000008,   /*!< FLV File Format */
	VO_FILE_FFSTREAMING_HTTPPD  = 0X80000009,   /*!< PD Streaming */
	VO_FILE_FFAUDIO_AU			= 0X80000010,   /*!< Audio WAVE File Format */
	VO_FILE_FFMOVIE_CMMB		= 0X80000020,	/*!< CMMB dump file */	
	VO_FILE_FFMOVIE_MKV			= 0X80000040,	/*!< mkv file format */
	VO_FILE_FFAUDIO_APE			= 0X80000080,	/*!< Audio APE file format */
	VO_FILE_FFAUDIO_ALAC		= 0X80000100,	/*!< Audio ALAC file format */
	VO_FILE_FFSTREAMING_HLS		= 0X80000200,	/*!< Http Live Streaming */
	VO_FILE_FFSTREAMING_SSS		= 0X80000400,	/*!< Silverlight Smooth Streaming */
	VO_FILE_FFSTREAMING_DASH		= 0X80000600,	/*!< DASH Streaming */
	VO_FILE_FFAUDIO_AC3         = 0X80000800,   /*!< Audio AC3 file format        */
	VO_FILE_FFAUDIO_PCM         = 0X80001000,   /*!< Audio PCM file format        */
	VO_FILE_FFAUDIO_DTS         = 0X80001001,   /*!< Audio DTS file format        */
    VO_FILE_FFVIDEO_H265		= 0X80001002,   /*!< Video H265 File Format */
    VO_FILE_FFSUBTITLE_TTML		= 0X80001003,   /*!< Subtitle TTML File Format */
    VO_FILE_FFSUBTITLE_WEBVTT	= 0X80001004,	/*!< Subtitle WEBVTT File Format */
	VO_FILE_FFMOVIE_MAX			= VO_MAX_ENUM_VALUE
}
VO_FILE_FORMAT;


/**
 * File operate mode
 */
typedef enum
{
	VO_FILE_READ_ONLY				= 0X00000001,	/*!< Read file only*/
	VO_FILE_WRITE_ONLY				= 0X00000002,	/*!< Write file only*/
	VO_FILE_READ_WRITE				= 0X00000003,	/*!< Read and Write*/
	VO_FILE_MODE_MAX				= VO_MAX_ENUM_VALUE
}VO_FILE_MODE;

/**
 * File position
 */
typedef enum
{
	VO_FILE_BEGIN					= 0X00000000,	/*!< from begin position of file*/
	VO_FILE_CURRENT					= 0X00000001,	/*!< from current position of file*/
	VO_FILE_END						= 0X00000002,	/*!< form end position of file*/
	VO_FILE_POS_MAX					= VO_MAX_ENUM_VALUE
}VO_FILE_POS;


/**
* File Source
*/
typedef struct
{
	VO_U32			nFlag;			/*!< VO_FILE_TYPE_NAME, HANDLE, ID */
	VO_PTR			pSource;		/*!< File name, handle or id */
	VO_S64			nOffset;		/*!< The begin pos of the file */
	VO_S64			nLength;		/*!< The length of the file */
	VO_FILE_MODE	nMode;			/*!< The operate mode */
	VO_U32			nReserve;		/*!< Reserve param */
} VO_FILE_SOURCE;

/**
* File Handle
*/
typedef struct
{
	VO_PTR	hFile;			/*!< File handle */
	VO_U32	nFlag;			/*!< 0,opened by owner, 1 opened by caller */
} VO_FILE_HANDLE;

#define VO_FILE_MEDIATIMEFILEPOS_FLAG_START			0x1
typedef struct
{
	//last 1 bit: set, start = true, else, start = false
	VO_U32	nFlag;
	VO_S64	llMediaTime;
	VO_S64	llFilePos;
} VO_FILE_MEDIATIMEFILEPOS;

/**
* File IO functions set
* uID: module ID which call file IO actually
*/
typedef struct
{
	VO_PTR (VO_API * Open) (VO_FILE_SOURCE * pSource);						/*!< File open operation, return the file IO handle. failed return NULL*/
	VO_S32 (VO_API * Read) (VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize);	/*!< File read operation. return read number, failed return -1, retry return -2*/
	VO_S32 (VO_API * Write) (VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize);	/*!< File write operation. return write number, failed return -1*/
	VO_S32 (VO_API * Flush) (VO_PTR pFile);									/*!< File flush operation. return 0*/
	VO_S64 (VO_API * Seek) (VO_PTR pFile, VO_S64 nPos, VO_FILE_POS uFlag);	/*!< File seek operation. return current file position, failed return -1*/
	VO_S64 (VO_API * Size) (VO_PTR pFile);									/*!< File get size operation. return file size, failed return -1*/
	VO_S64 (VO_API * Save) (VO_PTR pFile);									/*!< File get saved operation. return file saved size, failed return -1*/
	VO_S32 (VO_API * Close) (VO_PTR pFile);									/*!< File close operation, failed return -1*/
} VO_FILE_OPERATOR;


typedef struct _headerdata_element
{
	VO_S64 physical_pos;
	VO_S64 size;
}headerdata_element;

typedef struct _headerdata_info
{
	VO_S32 arraysize;
	headerdata_element * ptr_array;
	VO_S32 reserved;
}headerdata_info;

typedef struct _av_startpos
{
	VO_S64 audio_startpos;
	VO_S64 video_startpos;
}av_startpos;

typedef struct _video_matrix
{
	VO_S32 global[9];
	VO_S32 track[9];
	VO_S32 rotation; //combine matrix global & track. 0, 90, 180, 270
}video_matrix;

typedef struct _dec_spec_data
{
	VO_S32 size;
	VO_PTR buf;
}dec_spec_data;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voFile_H__
