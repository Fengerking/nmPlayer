/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/


#ifndef __VOOSMP_SOURCE_TYPE_H__
#define __VOOSMP_SOURCE_TYPE_H__

#include "voOnStreamType.h"

/**
 * Static Value
 */
#define VOOSMP_SRC_MAX_URL_LEN	2048

/**
 * Definition of return error code
 */
#define VOOSMP_SRC_ERR_OPEN_SRC_FAIL		0X81000001	/*!<Open source fail */
#define VOOSMP_SRC_ERR_CONTENT_ENCRYPT		0X81000002	/*!<Content is encrypt, only can playback after some operations */
#define VOOSMP_SRC_ERR_PLAYMODE_UNSUPPORT	0X81000003	/*!<Play mode not support */
#define VOOSMP_SRC_ERR_ERROR_DATA			0X81000004	/*!<File has error data */
#define VOOSMP_SRC_ERR_SEEK_FAIL			0X81000005	/*!<Seek not support */
#define VOOSMP_SRC_ERR_FORMAT_UNSUPPORT		0X81000006  /*!<File format not support */
#define VOOSMP_SRC_ERR_TRACK_NOTFOUND		0X81000007  /*!<Track can not be found */
#define VOOSMP_SRC_ERR_NO_DOWNLOAD_OP		0X81000008  /*!<No set download pointer */
#define VOOSMP_SRC_ERR_NO_LIB_OP			0X81000009  /*!<No set lib op  pointer */
#define VOOSMP_SRC_ERR_OUTPUT_NOTFOUND		0X8100000A	/*!<Can not find such output type in this source */
#define VOOSMP_SRC_ERR_CHUNK_SKIP           0X8100000B  /*!<Indicate the chunk must be dropped */
#define VOOSMP_SRC_ERR_FORCETIMESTAMP       0X8100000C  /*!<Indicate source should use the timestamp in the event callback function's parameter */


/**
 * Definition event call back ID
 */
#define	VOOSMP_SRC_CB_Connecting			0X02000001	/*!<It's connectting source */
#define	VOOSMP_SRC_CB_Connection_Finished	0X02000002	/*!<Source connection is finished */
#define VOOSMP_SRC_CB_Connection_Timeout	0X82000003	/*!<Source connect timeout */
#define VOOSMP_SRC_CB_Connection_Loss		0X82000004	/*!<Source connection loss */
#define VOOSMP_SRC_CB_Download_Status		0X02000005	/*!<HTTP download status the param 1 is int *(0 - 100)  */
#define VOOSMP_SRC_CB_Connection_Fail		0X82000006	/*!<Notify connect fail */
#define VOOSMP_SRC_CB_Download_Fail			0X82000007	/*!<Notify download fail */
#define VOOSMP_SRC_CB_DRM_Fail				0X82000008	/*!<Notify drm engine err */
#define VOOSMP_SRC_CB_Playlist_Parse_Err	0X82000009	/*!<Notify playlist parse error */
#define VOOSMP_SRC_CB_Connection_Rejected	0X8200000A	/*!<Notify maximum number of connections reached,currently used in RTSP only */
#define VOOSMP_SRC_CB_BA_Happened           0X0200000B  /*!<Notify bitrate is changed, parameter is new bitrate value, int* */
#define VOOSMP_SRC_CB_DRM_Not_Secure	    0X0200000C  /*!<Notify device is rooted.*/
#define VOOSMP_SRC_CB_DRM_AV_Out_Fail       0X8200000D  /*!<Notify device uses a/v output device but the license doesnt allows it.*/
#define VOOSMP_SRC_CB_Download_Fail_Waiting_Recover     0X8200000E  /*!<Notify download failed,and is waitting recover*/
#define VOOSMP_SRC_CB_Download_Fail_Recover_Success     0X0200000F  /*!<Notify download recovered*/
#define	VOOSMP_SRC_CB_Open_Finished         0X02000010  /*!<Source open finished, param 1 should be the pointor of VOOSMP_SRC_ERR_XXX value, VOOSMP_SRC_ERR_OPEN_SRC_FAIL or VOOSMP_ERR_None*/
#define VOOSMP_SRC_CB_Customer_Tag          0X02000020  /*!<Notify customer tag information inside source, param 1 will be the value defined in VOOSMP_SRC_CUSTOMERTAGID and param 2 will depend on param 1 */
#define VOOSMP_SRC_CB_Adaptive_Streaming_Info   0X02000030	/*!<Notify streaming information , param 1 will be the value defined in  VOOSMP_SRC_SOURCE_ADAPTIVE_STREAMING_INFO_EVENT , param 2 will depend on param 1*/
#define VOOSMP_SRC_CB_Adaptive_Streaming_Error  0X02000040  /*!<Notify adaptive streaming error, param 1 will be the value defined in VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT , param 2 will depend one param 1*/
#define VOOSMP_SRC_CB_Adaptive_Stream_Warning   0X02000050	/*!<Notify adaptive streaming error warning, param 1 will be the value defined in VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT , param 2 will depend on param 1*/
#define VOOSMP_SRC_CB_RTSP_Error                0X02000060  /*!<Notify RTSP error, param 1 will be the value defined in VOOSMP_SRC_RTSP_ERROR_EVENT*/
#define VOOSMP_SRC_CB_Seek_Complete             0X02000070  /*!<Notify seek complete, param 1 should be the seek return timestamp long long* , param 2 should be the pointor of VOOSMP_SRC_ERR_XXX value, VOOSMP_SRC_ERR_SEEK_FAIL or VOOSMP_ERR_None*/
#define VOOSMP_SRC_CB_Program_Changed			0X02000071  /*!<Notify the program info has been changed in source */
#define VOOSMP_SRC_CB_Program_Reset				0X02000072  /*!<Notify the program info has been reset in source */
#define VOOSMP_SRC_CB_Adaptive_Stream_SEEK2LASTCHUNK    0X02000073  /*!< Notify when seek to the last chunk of play list (NTS link without END tag need change to live mode) */
#define VOOSMP_SRC_CB_Not_Applicable_Media      0X02000074  /*!<Network is not available now,but it still hase some buffering data which can playback */
#define VOOSMP_SRC_CB_PD_DOWNLOAD_POSITION      0X02000075  /*!<Notify the current media position downloaded by the progressive download module, param1 is the position, int* */
#define VOOSMP_SRC_CB_PD_BUFFERING_PERCENT      0X02000076  /*!<Notify the current buffering percent of the progressive download module, param1 is the percent, int* */
#define VOOSMP_SRC_CB_UPDATE_URL_COMPLETE       0X02000077  /*!<Notify update source URL complete, param 1 will be event error ID(VOOSMP_SRC_CB_Connection_Fail,VOOSMP_SRC_Download_Fail,VOOSMP_SRC_DRM_Fail,VOOSMP_SRC_Playlist_Parse_Err), int* */

/**
 * Definition event call back ID for VOOSMP_PID_ONREQUEST_LISTENER(sync event)
 */
#define VOOSMP_SRC_CB_Authentication_Request    0X03000001  /*!< Request authentication information, application need process it.
                                                                 first parameter value will be VOOSMP_SRC_AUTHENTICATION_REQUEST_TYPE */
#define VOOSMP_SRC_CB_IO_HTTP_Start_Download    0X03000002	/*!<HTTP IO has start download, pParam1 will be the related url(unsigned char*), pParam2 will be NULL
                                                                The callback function return code will be below:
                                                                1)VOOSMP_ERR_None: indicator process event successfully
                                                                2)VOOSMP_ERR_Unknow: indicator process event failed
                                                                3)VOOSMP_ERR_Retry: indicator it need re-send this event again*/
#define VOOSMP_SRC_CB_IO_HTTP_Download_Failed   0X03000003  /*!<HTTP IO failed to download, pParam1 will be the related url(unsigned char*) ,pParam2 will be the reason VOOSMP_SRC_IO_FAILED_REASON_DESCRIPTION* */
#define VOOSMP_SRC_CB_DRM_Init_Data             0X03000004  /*!<Event of DRM init data,param is VOOSMP_SRC_DRM_INIT_DATA */


/**
 * Definition parameter ID
 */
#define VOOSMP_SRC_PID_DRM_FILE_NAME					0X03000001	/*!<Set DRM file name*/
#define VOOSMP_SRC_PID_DRM_API_NAME						0X03000002	/*!<Set DRM API name*/
#define VOOSMP_SRC_PID_FUNC_IO							0X03000003	/*!<Set the io function, it can be local file or http file, VODOWNLOAD_FUNC* */
#define VOOSMP_SRC_PID_FUNC_READ_BUF					0X03000004	/*!<Get read buffer function pointer,refer to VOOSMP_READBUFFER_FUNC */
#define VOOSMP_SRC_PID_CC_AUTO_SWITCH_DURATION			0X03000005  /*!<Set the duration of close caption swith automatically between 608 and 708, int* */
#define VOOSMP_SRC_PID_DRM_THIRDPARTY_FUNC_SET			0X03000006	/*!<Set third party DRM function set */
#define VOOSMP_SRC_PID_DRM_CALLBACK_FUNC				0X03000007	/*!<Set DRM callback function,customer follow VisualOn's DRM API */
#define VOOSMP_SRC_PID_HTTPVERIFICATIONCALLBACK			0X03000008  /*!<Set the http verification callback, VOOSMP_SRC_HTTP_VERIFYCALLBACK* */
#define VOOSMP_SRC_PID_DOHTTPVERIFICATION				0X03000009  /*!<Set the verification information to start HTTP verification, VOOSMP_SRC_VERIFICATIONINFO* */
#define VOOSMP_SRC_PID_SOCKET_CONNECTION_TYPE			0X0300000A  /*!<Set RTSP connection type,0:automatically try UDP first then TCP,1:force to use TCP,-1:disable TCP, default 0, int* */
#define	VOOSMP_SRC_PID_COMMAND_STRING					0X0300000B	/*!<Get/Set command string,module need parse this string to get the exact command.refer to  char *   */
#define VOOSMP_SRC_PID_CAP_TABLE_PATH					0X0300000C	/*!<Set cap information table path,char* */
#define VOOSMP_SRC_PID_BA_STARTCAP						0X0300000D	/*!<Set start cap information, VOOSMP_PERF_DATA* */
#define VOOSMP_SRC_PID_DODRMVERIFICATION				0X0300000E  /*!<Set the verification information to start DRM verification, VOOSMP_SRC_VERIFICATIONINFO* */
#define VOOSMP_SRC_PID_DRM_UNIQUE_IDENTIFIER            0X0300000F  /*!<Get/Set IMEI or MAC of the device*/
#define VOOSMP_SRC_PID_DOWNLOAD_FAIL_MAX_TOLERANT_COUNT 0X03000010  /*!<Set max tolerant count if download fail, int* */
#define VOOSMP_SRC_PID_DISABLE_CPU_ADAPTION             0X03000011  /*!<Enable/disable CPU adaption when chose BA,1:disable,0:enable*/
#define VOOSMP_SRC_PID_DRM_FUNC_SET                     0X03000012	/*!<Set DRM function set */
#define VOOSMP_SRC_PID_BUFFER_START_BUFFERING_TIME		0X03000013  /*!<Set the buffering time of playback start, int*, unit is ms */
#define VOOSMP_SRC_PID_BUFFER_BUFFERING_TIME			0X03000014	/*!<Set the buffering time,for example after seeking int*, unit is ms */
#define VOOSMP_SRC_PID_DVRINFO                          0X03000015  /*!<Get the DVR Info, VOOSMP_SRC_DVRINFO** */
#define VOOSMP_SRC_PID_BA_WORKMODE						0X03000016	/*!<Get/Set Set/Get the BA work mode, VOOSMP_SRC_ADAPTIVESTREAMING_BA_MODE* */
#define VOOSMP_SRC_PID_RTSP_STATS						0X03000017	/*!<Get the rtsp module status value, VOOSMP_SRC_RTSP_STATS* */
#define VOOSMP_SRC_PID_LOW_LATENCY_MODE					0X03000018	/*!<Set source module into low latency mode */
#define VOOSMP_SRC_PID_BUFFER_MAX_SIZE                  0X03000019	/*!<Set source module max buffer size int*, unit is ms */
#define VOOSMP_SRC_PID_HTTP_HEADER						0X0300001A	/*!<Set the HTTP header, VOOSMP_SRC_HTTP_HEADER* */
#define VOOSMP_SRC_PID_RTSP_CONNECTION_PORT             0X0300001B  /*!<Set RTSP connection port, VOOSMP_SRC_RTSP_CONNECTION_PORT* */
#define VOOSMP_SRC_PID_HTTP_PROXY_INFO                  0X0300001C  /*!<Set proxy server information, VOOSMP_SRC_HTTP_PROXY* */
#define VOOSMP_SRC_PID_DRM_ADAPTER_FILE_NAME            0X0300001D	/*!<Set DRM adapter file name. char * */
#define VOOSMP_SRC_PID_DRM_ADAPTER_API_NAME             0X0300001E	/*!<Set DRM adapter API name. char * */
#define VOOSMP_SRC_PID_DRM_ADAPTER_OBJECT               0X0300001F	/*!<Set DRM adapter function set. void * */
#define VOOSMP_SRC_PID_PRESENTATION_DELAY               0X03000020	/*!<Set presentation delay time (milliseconds) for a live streaming source with respective to current time */
#define VOOSMP_SRC_PID_PD_CONNECTION_RETRY_TIMES        0X03000021  /*!<Set retry times when socket connect failed, the parameter is int *, the default value is 2, and if negative value is set, it will retry endless, until the Stop() be called.*/
#define VOOSMP_SRC_PID_PERIOD2TIME                      0X03000022  /*!<Convert from period sequence number to timestamp , VOOSMP_SRC_PERIODTIME_INFO* */
#define VOOSMP_SRC_PID_DRM_INIT_DATA_RESPONSE           0X03000023  /*!<Set DRM init data response to DRM module, VOOSMP_SRC_DRM_INIT_DATA_RESPONSE* */
#define VOOSMP_SRC_PID_APPLICATION_SUSPEND              0X03000024  /*!< Set application suspend */
#define VOOSMP_SRC_PID_APPLICATION_RESUME               0X03000025  /*!< Set application resume */
#define VOOSMP_SRC_PID_BITRATE_THRESHOLD                0X03000026  /*!< Set upper/lower bitrate threshold for adaptation, VOOSMP_SRC_BA_THRESHOLD* */
#define VOOSMP_SRC_PID_SEGMENT_DOWNLOAD_RETRY_COUNT     0X03000027  /*!< Set the maximum number of retry attempts to download a corresponding segment from a
 different bit rate due, default is 0,int* */
#define VOOSMP_SRC_PID_ENABLE_RTSP_HTTP_TUNNELING       0X03000028  /*!< Enable/Disable RTSP over HTTP tunneling. The default is disable(0). int* */
#define VOOSMP_SRC_PID_RTSP_OVER_HTTP_CONNECTION_PORT   0X03000029  /*!< Set port number for RTSP over HTTP tunneling. int* */
#define VOOSMP_SRC_PID_UPDATE_SOURCE_URL                0X0300002A  /*!< Set new source URL. char* */
#define VOOSMP_SRC_PID_HTTP_RETRY_TIMEOUT               0X0300002B  /*!< Set HTTP connection retry timeout, The default is 120 seconds. Setting the value to -1 disables the timeout so that the player will keep retrying, until the connection is established again. int* */
#define VOOSMP_SRC_PID_DEFAULT_AUDIO_LANGUAGE           0X0300002C  /*!< Set default audio language, char*, refer to ISO 639-2 code */
#define VOOSMP_SRC_PID_DEFAULT_SUBTITLE_LANGUAGE        0X0300002D  /*!< Set default subtitle language, char*, refer to ISO 639-2 code */

/**
 * Emuneration of source fromat
 */	
typedef enum
{
    VOOSMP_SRC_AUTO_DETECT			= 0X00000000,   /*!< Detect the source format automatically*/
    VOOSMP_SRC_FFMOVIE_MP4			= 0X00000001,   /*!< MP4 File Format */
    VOOSMP_SRC_FFMOVIE_ASF			= 0X00000002,   /*!< MS ASF File Format */
    VOOSMP_SRC_FFMOVIE_AVI			= 0X00000004,   /*!< AVI File Format */
    VOOSMP_SRC_FFMOVIE_MOV			= 0X00000008,   /*!< QuickTime Movie File Format */
    VOOSMP_SRC_FFMOVIE_REAL			= 0X00000010,   /*!< Real File Format */
    VOOSMP_SRC_FFMOVIE_MPG			= 0X00000020,   /*!< Mpeg File Format */
    VOOSMP_SRC_FFMOVIE_TS			= 0X00000040,   /*!< TS File Format */
    VOOSMP_SRC_FFMOVIE_DV			= 0X00000080,   /*!< DV File Format */
	VOOSMP_SRC_FFMOVIE_FLV			= 0x00000100,   /*!< FLV File Format */
	VOOSMP_SRC_FFMOVIE_CMMB			= 0X00000200,	/*!< CMMB dump file */	
	VOOSMP_SRC_FFMOVIE_MKV			= 0X00000400,	/*!< mkv file format */
	VOOSMP_SRC_FFVIDEO_H264			= 0X00000800,   /*!< Video H264 File Format */
    VOOSMP_SRC_FFVIDEO_H263			= 0X00001000,   /*!< Video H263 File Format */
    VOOSMP_SRC_FFVIDEO_H261			= 0X00002000,   /*!< Video H261 File Format */
    VOOSMP_SRC_FFVIDEO_MPEG4		= 0X00004000,   /*!< Video Mpeg4 File Format */
    VOOSMP_SRC_FFVIDEO_MPEG2		= 0X00008000,   /*!< Video Mpeg2 File Format */
    VOOSMP_SRC_FFVIDEO_MPEG1		= 0X00010000,   /*!< Video Mpeg1 File Format */
    VOOSMP_SRC_FFVIDEO_MJPEG		= 0X00020000,   /*!< Video Motion Jpeg File Format */
    VOOSMP_SRC_FFVIDEO_RAWDATA		= 0X00040000,   /*!< Video raw data (RGB, YUV) Format */	
    VOOSMP_SRC_FFAUDIO_AAC			= 0X00080000,   /*!< Audio AAC File Format */
    VOOSMP_SRC_FFAUDIO_AMR			= 0X00100000,   /*!< Audio AMR NB File Format */
    VOOSMP_SRC_FFAUDIO_AWB			= 0X00200000,   /*!< Audio AMR WB File Format */
    VOOSMP_SRC_FFAUDIO_MP3			= 0X00400000,   /*!< Audio MP3 File Format */
    VOOSMP_SRC_FFAUDIO_QCP			= 0X00800000,   /*!< Audio QCP (QCELP or EVRC) File Format */
    VOOSMP_SRC_FFAUDIO_WAV			= 0X01000000,   /*!< Audio WAVE File Format */
    VOOSMP_SRC_FFAUDIO_WMA			= 0X02000000,   /*!< Audio WMA File Format */
    VOOSMP_SRC_FFAUDIO_MIDI			= 0X04000000,   /*!< Audio MIDI File Format */
	VOOSMP_SRC_FFAUDIO_OGG			= 0x08000001,	/*!< Audio OGG File Format */
	VOOSMP_SRC_FFAUDIO_FLAC			= 0x08000002,	/*!< Audio FLAC File Format */
	VOOSMP_SRC_FFAUDIO_AU			= 0X08000004,   /*!< Audio WAVE File Format */
	VOOSMP_SRC_FFAUDIO_APE			= 0X08000008,	/*!< Audio APE file format */
	VOOSMP_SRC_FFAUDIO_ALAC			= 0X08000010,	/*!< Audio ALAC file format */
	VOOSMP_SRC_FFAUDIO_AC3			= 0X08000020,   /*!< Audio AC3 file format        */
	VOOSMP_SRC_FFAUDIO_PCM			= 0X08000040,   /*!< Audio PCM file format        */
	VOOSMP_SRC_FFAUDIO_DTS			= 0X08000080,   /*!< Audio DTS file format        */
	VOOSMP_SRC_FFSTREAMING_RTSP		= 0X08000100,   /*!< RTSP Streaming */
	VOOSMP_SRC_FFSTREAMING_SDP		= 0X08000200,   /*!< Session description */
	VOOSMP_SRC_FFSTREAMING_HLS		= 0X08000400,	/*!< Http Live Streaming */
	VOOSMP_SRC_FFSTREAMING_SSS		= 0X08000800,	/*!< Silverlight Smooth Streaming */
	VOOSMP_SRC_FFSTREAMING_DASH		= 0X08001000,	/*!< DASH Streaming */
	VOOSMP_SRC_FFSTREAMING_HTTPPD	= 0X08002000,   /*!< PD Streaming */
    VOOSMP_SRC_FFSTREAMING_CMMB     = 0X08004000,   /*!< CMMB Streaming */
	VOOSMP_SRC_FFVIDEO_H265			= 0X08008000,   /*!< Video H265 File Format */
	VOOSMP_SRC_FFMOVIE_MAX			= 0X7FFFFFFF	/*!< Max value definition        */

}VOOSMP_SRC_SOURCE_FORMAT;


/**
 * Emuneration of program type
 */	
typedef enum 
{
	VOOSMP_SRC_PROGRAM_TYPE_LIVE,
	VOOSMP_SRC_PROGRAM_TYPE_VOD,
	VOOSMP_SRC_PROGRAM_TYPE_UNKNOWN = 255,
}VOOSMP_SRC_PROGRAM_TYPE;


/**
 * Emuneration of track select type
 */	
typedef enum
{
	VOOSMP_SRC_TRACK_SELECT_SELECTABLE	= 0X00000000,
	VOOSMP_SRC_TRACK_SELECT_RECOMMEND	= 0X00000001,
	VOOSMP_SRC_TRACK_SELECT_SELECTED	= 0X00000002,
	VOOSMP_SRC_TRACK_SELECT_DISABLE		= 0X00000004,
	VOOSMP_SRC_TRACK_SELECT_DEFAULT		= 0X00000008,
	VOOSMP_SRC_TRACK_SELECT_FORCE		= 0X00000010,
	VOOSMP_SRC_TRACK_SELECT_MAX			= 0X7FFFFFFF
}VOOSMP_SRC_TRACK_SELECT;


/**
 * Structure for audio information
 */	
typedef struct
{
	VOOSMP_AUDIO_FORMAT sFormat;
	char				szLanguage[256];
} VOOSMP_SRC_AUDIO_INFO;


/**
 * Structure for video information
 */	
typedef struct
{
	VOOSMP_VIDEO_FORMAT sFormat;
	int					nAngle;
    char                szVideoDesc[252];
} VOOSMP_SRC_VIDEO_INFO;

/**
 * General subtitle info
 */
typedef struct
{
    char    szLanguage[256]; /*!< Language name */
    int     nCodingType;    /*!< Coding type */
    char    Align[8];       /*!< Byte align */
}VOOSMP_SRC_SUBTITLE_INFO;


/**
 * Structure for track information
 */
typedef struct
{
	int							nTrackID;				/*!< the track id created by our parser, it is unique in this source session */
	int                         nSelectInfo;			/*!< Indicated if the track is selected and recommend or default,refer to VOOSMP_SRC_TRACK_SELECT */

	char						szFourCC[8];
	VOOSMP_SOURCE_STREAMTYPE	nTrackType;				/*!< video/audio/subtitle */
	int							nCodec;					/*!< codec type,refer to VOOSMP_AUDIO_CODINGTYPE and VOOSMP_VIDEO_CODINGTYPE  */	
	long long					nDuration;				/*!< duration of this track */
	
	int							nChunkCounts;			/*!< chunks in the track */
	int							nBitrate;				/*!< bitrate of this track */
	
	union
	{
		VOOSMP_SRC_AUDIO_INFO       AudioInfo;				/*!< audio info */
		VOOSMP_SRC_VIDEO_INFO       VideoInfo;				/*!< video info */
        VOOSMP_SRC_SUBTITLE_INFO    SubtitleInfo;           /*!< subtitle info */
	};
	
	int							nHeadSize;				/*!< sequence head data size*/
	char*						pHeadData;				/*!< sequence head data */
}VOOSMP_SRC_TRACK_INFO;


/**
 * Structure for stream information
 */	
typedef struct  
{
	int							nStreamID;				/*!< the sub stream id created by our parser, it is unique in this source session */
	int							nSelInfo;				/*!< Indicated if the Stream is selected and recommend or default,refer to VOOSMP_SRC_TRACK_SELECT */
	int							nBitrate;				/*!< the bitrate of the stream */
	int							nTrackCount;			/*!< track count if this sub stream */
	VOOSMP_SRC_TRACK_INFO		**ppTrackInfo;			/*!< Track info */
}VOOSMP_SRC_STREAM_INFO;


/**
 * Structure for program information
 */		
typedef struct  
{
	int								nProgramID;				/*!< the stream id created by our parser, it is unique in this source session */
	int								nSelInfo;				/*!< Indicated if the Program is selected and recommend or default,refer to VOOSMP_SRC_TRACK_SELECT */
    VOOSMP_SRC_PROGRAM_TYPE			nProgramType;			/*!< Indicate if the Program is live or vod */
	char							szProgramName[256];		/*!< Name of the program */
    int								nStreamCount;			/*!< Stream Count */
	VOOSMP_SRC_STREAM_INFO**		pStreamInfo;			/*!< Stream info */	
}VOOSMP_SRC_PROGRAM_INFO;


/**
 *  Definition of HTTP status
 */
typedef enum 
{
	VOOSMP_SRC_HTTP_BEGIN = 0,
	VOOSMP_SRC_HTTP_SOCKETCONNECTED,
	VOOSMP_SRC_HTTP_REQUESTPREPARED,
	VOOSMP_SRC_HTTP_REQUESTSEND,
	VOOSMP_SRC_HTTP_RESPONSERECVED,
	VOOSMP_SRC_HTTP_RESPONSEANALYSED,
}VOOSMP_SRC_HTTP_STATUS;

/**
 * callback function for verification
 * \param hHandle [in] the pUserData member in VOOSMP_SRC_HTTP_VERIFYCALLBACK structure
 * \param nID [in] should use the value defined in VOOSMP_SRC_HTTP_STATUS
 * \param pSourceData [in] the data from source,it need pass it to source again when transfering verify information
 */
typedef int (* VOOSMP_HTTP_VERIFYCALLBACK_FUNC)(void* hHandle, int nID, void* pSourceData);


/**
 * Callback function structure for HTTP verify,used in VOOSMP_SRC_PID_HTTPVERIFICATIONCALLBACK
 */
struct VOOSMP_SRC_HTTP_VERIFYCALLBACK 
{
	void*							hHandle;	/*!< The user data */
	VOOSMP_HTTP_VERIFYCALLBACK_FUNC pCallback;  /*!< Callback function pointer */
};


/**
 * Definition of verification information
 */
typedef struct 
{
	void*	pUserData;				/*!<[in] The userdata recved in VOOSMP_SRC_HTTP_VERIFYCALLBACK*/
	void*	pData;					/*!<[in] The data that need to do verification*/
	int		nDataSize;				/*!<[in] The data size*/
	int		nDataFlag;				/*!<[in] The data flag, if it is 0, it is the request string, 1 is username:password,other value to not support currently */
	char*	pszResponse;			/*!<[out] Output the authentication response string*/
	int		nResponseSize;			/*!<[out] Output the response string size*/
}VOOSMP_SRC_VERIFICATIONINFO;


/**
 * Emuneration of source basic in
 */
typedef enum
{
	VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_BITRATE_CHANGE       = 1,		/*!<VOOSMPListener callabck param 2 will be the new bitrate, int* */
    VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_MEDIATYPE_CHANGE     = 2,      /*!<VOOSMPListener callabck param 2 will be the new media type defined in VOOSMP_AVAILABLE_TRACK_TYPE, int* */
    VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PROGRAM_TYPE         = 3,      /*!<VOOSMPListener callabck param 2 will be the program type defined in VOOSMP_SRC_PROGRAM_TYPE, int* */
    VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_BEGINDOWNLOAD  = 4,      /*!<VOOSMPListener callabck param 2 will be VOOSMP_SRC_CHUNK_INFO* , if the return value of this callback is VOOSMP_SRC_ERR_CHUNK_SKIP, source must drop this chunk */
    VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DROPPED        = 5 ,		/*!<VOOSMPListener callabck param 2 will be VOOSMP_SRC_CHUNK_INFO* */
    VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DOWNLOADOK     = 6,      /*!<VOOSMPListener callabck param 2 will be VOOSMP_SRC_CHUNK_INFO* */
    VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PLAYLIST_DOWNLOADOK  = 7,      /*!<VOOSMPListener callabck param 2 will be VOOSMP_SRC_CHUNK_INFO* */
    VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PROGRAM_CHANGE       = 8,		/*!<Indicates that Program has been changed in source. When this event is received, you should get program info again. Parameter 2 shall be ignored* */
    VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_FILE_FORMATSUPPORTED = 9,      /*!<Indicates chunk is supported* */
    VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_LIVESEEKABLE         = 10,     /*!<Indicates live clip can be seeked now* */
    VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_DISCONTINUE_SAMPLE   = 11,	//indicate this is the first sample from the discontinue chunk, param 2 will be VOOSMP_SRC_CHUNK_SAMPLE* */
	
    VOOSMP_SRC_ADAPTIVE_STREAMING_INFOEVENT_MAX = 0X7FFFFFFF,
}VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT;


/**
 * Emuneration of chunk type
 */
typedef enum
{
	VOOSMP_SRC_CHUNK_AUDIO,
	VOOSMP_SRC_CHUNK_VIDEO,
	VOOSMP_SRC_CHUNK_AUDIOVIDEO,
	VOOSMP_SRC_CHUNK_HEADDATA,
	VOOSMP_SRC_CHUNK_SUBTITLE,
	VOOSMP_SRC_CHUNK_UNKNOWN = 255,
	VOOSMP_SRC_CHUNK_TYPE_MAX = 0X7FFFFFFF,
}VOOSMP_SRC_CHUNK_TYPE;


/**
 * Structure of chunk
 */
typedef struct
{
	VOOSMP_SRC_CHUNK_TYPE                   nType;                          /*!<The type of this chunk */
	char									szRootUrl[VOOSMP_MAX_URL_LEN];	/*!<The URL of manifest. It must be filled by parser. */
	char									szUrl[VOOSMP_MAX_URL_LEN];		/*!<URL of this chunk , maybe relative URL */
	unsigned long long						ullStartTime;                   /*!<The start offset time of this chunk , the unit of ( ullStartTime / ullTimeScale * 1000 ) should be ms */
    unsigned int							uPeriodSequenceNumber;           /*!< The sequence number of this chunk */
	unsigned long long						ullDuration;                    /*!<Duration of this chunk , the unit of ( ullDuration / ullTimeScale * 1000 ) should be ms */
	unsigned long long						ullTimeScale;                   /*!<Time scale of this chunk */
	unsigned int							uReserved1;                     /*!<Reserved 1 */
	unsigned int							uReserved2;                     /*!< It is the Chunk flag */
}VOOSMP_SRC_CHUNK_INFO;


/**
 * Emuneration of adapter streaming error
 */
typedef enum
{
	VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_PLAYLIST_PARSEFAIL        = 1,     /*!<Notify playlist parse failed */
	VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_PLAYLIST_UNSUPPORTED      = 2,     /*!<Notify playlist not support */
	VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_UNSUPPORTED     = 3,     /*!<Notify stream not support */
	VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_DOWNLOADFAIL    = 4,     /*!<Notify download failed */
    VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_DRMLICENSEERROR = 5,     /*!<Notify DRM error */
	VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_STREAMING_VOLIBLICENSEERROR = 6,   /*!<Notify License error */
	VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT_MAX                       = 0X7FFFFFFF,
}VOOSMP_SRC_ADAPTIVE_STREAMING_ERROR_EVENT;


/**
 * Emuneration of RTSP error
 */
typedef enum
{
    VOOSMP_SRC_RTSP_ERROR_CONNECT_FAIL                                  = 1,          /*!<Notify connect failed in RTSP*/
    VOOSMP_SRC_RTSP_ERROR_DESCRIBE_FAIL                                 = 2,          /*!<Notify describle failed in RTSP*/
    VOOSMP_SRC_RTSP_ERROR_SETUP_FAIL                                    = 3,          /*!<Notify setup failed in RTSP*/
    VOOSMP_SRC_RTSP_ERROR_PLAY_FAIL                                     = 4,          /*!<Notify play failed in RTSP*/
    VOOSMP_SRC_RTSP_ERROR_PAUSE_FAIL                                    = 5,          /*!<Notify pause failed in RTSP*/
    VOOSMP_SRC_RTSP_ERROR_OPTION_FAIL                                   = 6,          /*!<Notify option failed in RTSP*/
    VOOSMP_SRC_RTSP_ERROR_SOCKET_ERROR                                  = 7,          /*!<Notify socket error in RTP channel of RTSP */
	VOOSMP_SRC_RTSP_ERROR_EVENT_MAX                                     = 0X7FFFFFFF,
}VOOSMP_SRC_RTSP_ERROR_EVENT;


/**
 * Emuneration of source warning
 */
typedef enum
{
	VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_DOWNLOADERROR             = 1,    /*!<Notify chunk download failed */
	VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_FILEFORMATUNSUPPORTED     = 2,    /*!<Notify chunk formatnot support */
	VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_DRMERROR                  = 3,    /*!<Notify DRM error */
    VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_PLAYLIST_DOWNLOADERROR          = 4,	/*!<Notify playlist download failed */

	VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_MAX = 0X7FFFFFFF,
}VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT;
	
/**
 * Enumeration of source customer tag
 */
typedef enum
{
	VOOSMP_SRC_CUSTOMERTAGID_TIMEDTAG = 1,					// param2 will be VO_SOURCE2_CUSTOMERTAG_TIMEDTAG* */
}VOOSMP_SRC_CUSTOMERTAGID;
 
 
 /**
 * Structure of source customer timed tag
 */
typedef struct  
{
	unsigned long long		ullTimeStamp;				/*!<Time stamp*/
	
	unsigned int			uSize;						/*!<Size */
	void*					pData;						/*!<Data pointor */
		
	unsigned int			uFlag;						/*!<For further use */
	void*					pReserve;					/*!<Data pointor */
}VOOSMP_SRC_CUSTOMERTAG_TIMEDTAG;


/**
 * Structure of item's property for each track
 */
typedef struct
{
	char    szKey[32];              /*!<Property key*/
	char*   pszProperty;            /*!<The property */
}VOOSMP_SRC_TRACK_ITEM_PROPERTY;


/**
 * Structure of track property
 */
typedef struct
{
	int                                 nPropertyCount;         /*!<Total property count*/
	VOOSMP_SRC_TRACK_ITEM_PROPERTY**    ppItemProperties;		/*!<Size */
}VOOSMP_SRC_TRACK_PROPERTY;

/**
 * Structure of current track index information
 */
typedef struct
{
    int nCurrAudioIdx;      /*!<Current audio index of track */
    int nCurrVideoIdx;      /*!<Current video index of track */
    int nCurrSubtitleIdx;   /*!<Current subtitle index of track */
    int nReserved;
}VOOSMP_SRC_CURR_TRACK_INDEX;


/**
 * Structure of DVR
 */
typedef struct
{
    unsigned long long  uStartTime;     /*!<Start time */
    unsigned long long  uEndTime;       /*!<End time */
    unsigned long long  uPlayingTime;   /*!<Current playing time */
    unsigned long long  uLiveTime;      /*!<Live time */
    void*               pReserve1;      /*!<Reserved */
    void*               pReserve2;      /*!<Reserved */
}VOOSMP_SRC_DVRINFO;


/**
 * Structure of playlist information
 */
typedef struct
{
	char            szRootUrl[VOOSMP_SRC_MAX_URL_LEN];				/*!<The URL of parent playlist */
	char            szUrl[VOOSMP_SRC_MAX_URL_LEN];					/*!<The URL of the playlist , maybe relative URL */
	char            szNewUrl[VOOSMP_SRC_MAX_URL_LEN];				/*!<The URL after download( maybe redirect forever ), you should always use this url after get this struct */
    
	unsigned char*  pData;											/*!<The data in the playlist */
	unsigned int    uDataSize;										/*!<Playlist size */
    
	unsigned int	uReserved1;										/*!<It is an Http ErrorCode for Warning & Error Event*/
	unsigned int	uReserved2;
    
}VOOSMP_SRC_ADAPTIVESTREAMING_PLAYLIST_DATA;


/**
 * Enum of BA mode
 */
typedef enum 
{
	VOOSMP_SRC_ADAPTIVE_STREAMING_BA_MODE_AUTO      = 0,
    VOOSMP_SRC_ADAPTIVE_STREAMING_BA_MODE_MANUAL    = 1,
}VOOSMP_SRC_ADAPTIVESTREAMING_BA_MODE;


/**
 * Structure of RTSP information
 */
typedef struct
{
	VOOSMP_SOURCE_STREAMTYPE        nTrackType;						/*!<[in] track type */
	unsigned int                    uPacketRecved;					/*!<[out] the cumulative number of RTP media packets received in this media track during this session */
	unsigned int                    uPacketDuplicated;				/*!<[out] cumulative number of RTP media packets previously received in this media track during this session */
	unsigned int                    uPacketLost;					/*!<[out] the cumulative number of RTP media packets lost for this media track type */
	unsigned int                    uPacketSent;					/*!<[out] the cumulative number of RTP media packets sent in this media track during this session */
	unsigned int                    uAverageJitter;					/*!<[out] the mean relative transit time between each two RTP packets of this media track type throughout the duration of the media session */
	unsigned int                    uAverageLatency;				/*!<[out] average accumulating latency values of all RTP media packets transmitted from the server to the client, !!now only reserved!! */
}VOOSMP_SRC_RTSP_STATS;


/**
 * Structure of HTTP head
 */
typedef struct
{
	unsigned char* 	pszHeaderName;      /*!<Name of the header, must not be null
                                         Value could be:"Set-Cookie" "User-Agent" */
	unsigned char* 	pszHeaderValue;     /*!<Value of the header,must not be null,it depends on pszHeaderName*/
        
	unsigned int    uFlag;              /*!<Indicates some property of this structure,not used right now */
	void*           pFlagData;          /*!<With some special flag, this field may use */
}VOOSMP_SRC_HTTP_HEADER;

/**
 * Structure of RTSP connection port
 */
typedef struct
{
	unsigned int    uAudioConnectionPort;	/*!<Indicates audio connection port (1024, 65535) */
	unsigned int    uVideoConnectionPort;	/*!<Indicates video connection port (1024, 65535) */
}VOOSMP_SRC_RTSP_CONNECTION_PORT;


typedef enum
{
	VOOSMP_IO_HTTP_CONNECT_FAILED,			/*!<HTTP IO connect failed*/
	VOOSMP_IO_HTTP_INVALID_RESPONSE,		/*!<HTTP IO failed to get response or response can't be parsed or response too large*/
	VOOSMP_IO_HTTP_CLIENT_ERROR,            /*!<HTTP IO 4xx error*/
	VOOSMP_IO_HTTP_SERVER_ERROR,			/*!<HTTP IO 5xx error*/
    VOOSMP_IO_HTTP_FAIL_REASON_MAX_VALUE  = 0x7FFFFFFF,   /*!<Max value*/
}VOOSMP_SRC_IO_DOWNLOAD_FAIL_REASON;

typedef struct
{
	VOOSMP_SRC_IO_DOWNLOAD_FAIL_REASON  reason;         /*!<Failed reason*/
	char*                               pszResponse;	/*!<When the reason is VOOSMP_IO_HTTP_CLIENT_ERROR or VOOSMP_IO_HTTP_SERVER_ERROR,it contain The response string, otherwise it will be NULL*/
	int                                 nResponseSize;			/*!<when the reason is VOOSMP_IO_HTTP_CLIENT_ERROR or VOOSMP_IO_HTTP_SERVER_ERROR,it contain The response string size, otherwise it will be 0*/
}VOOSMP_SRC_IO_FAILED_REASON_DESCRIPTION;


/**
 * Used for VOOSMP_SRC_CB_Authentication_Request
 */
#define VOOSMP_SRC_AUTHENTICATION_BASE_TYPE_SOURCE		0x10000000
#define VOOSMP_SRC_AUTHENTICATION_BASE_TYPE_DRM			0x20000000

typedef enum
{
	VOOSMP_SRC_AUTHENTICATION_HTTP_BEGIN                = VOOSMP_SRC_AUTHENTICATION_BASE_TYPE_SOURCE,
	VOOSMP_SRC_AUTHENTICATION_HTTP_SOCKET_CONNECTED,
	VOOSMP_SRC_AUTHENTICATION_HTTP_REQUEST_PREPARED,
	VOOSMP_SRC_AUTHENTICATION_HTTP_REQUEST_SEND,
	VOOSMP_SRC_AUTHENTICATION_HTTP_RESPONSE_RECVED,
	VOOSMP_SRC_AUTHENTICATION_HTTP_RESPONSE_ANALYSED,
    
    VOOSMP_SRC_AUTHENTICATION_DRM_REQUEST_SERVER_INFO   = VOOSMP_SRC_AUTHENTICATION_BASE_TYPE_DRM,
    
	VOOSMP_SRC_AUTHENTICATION_REQUEST_TYPE_MAX = 0x7FFFFFFF,
}VOOSMP_SRC_AUTHENTICATION_REQUEST_TYPE;


/**
 * Structure of proxy server information
 */
typedef struct
{
    unsigned char*  pszProxyHost;   /*!<Proxy server host name or ip address, must not be null */
    int             nProxyPort;     /*!<Proxy server port number,must not be null */
    unsigned int    uFlag;          /*!<Indicates some property of this structure, not used right now */
    void*           pFlagData;      /*!<With some special Flag, this field may use */
}VOOSMP_SRC_HTTP_PROXY;

#if 0 
/**
 * Structure of DRM init info.
 */
typedef struct
{
    char*               pData;      // Data of DRM init info
    unsigned int        nDataSize;  // Size of DRM init info
}VOOSMP_SRC_DRM_INIT_DATA;
#else
/**
* function pointer to allocate memory to be returned to DRM module
* \param uSize: wanted space
* \retval the pointer of allocated. if NULL, means not enough space
*/
typedef unsigned char* (*VOOSMP_DRM_ALLOCATEMEM)(unsigned uSize);

/**
 * Structure used to store DRM header info and receive some responses
 */
typedef struct
{
    char*       pData;     // data of DRM header info
    unsigned    uSize;     // size of DRM header info

    char*       pRetData;  // data of returned DRM header info
    unsigned    uRetSize;  // size of returned DRM header info

    VOOSMP_DRM_ALLOCATEMEM fAllocate;
} VOOSMP_SRC_DRM_INIT_DATA_RESPONSE;
#endif

/**
 * Structure of chunk sample info.
 */
typedef struct
{
	unsigned int			nChunkFlag;
	unsigned long long		ullChunkStartTime;
    unsigned int			uPeriodSequenceNumber;
	unsigned long long		ullPeriodFirstChunkStartTime;
	unsigned long long*		pullSampleTimeStamp;
    
	void*                   pReserve1;
	void*                   pReserve2;
}VOOSMP_SRC_CHUNK_SAMPLE;


/**
 * Structure of period info.
 */
typedef struct
{
	unsigned int        uPeriodSequenceNumber;
	unsigned long long  ullTimeStamp;
}VOOSMP_SRC_PERIODTIME_INFO;


/**
 * Threshold of bitrate adaptation.
 */
typedef struct
{
	int nUpper;
    int nLower;
}VOOSMP_SRC_BA_THRESHOLD;


#endif // end __VOOSMP_SOURCE_TYPE_H__
