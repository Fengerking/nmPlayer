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


#ifndef __voOnStreamType_h__
#define __voOnStreamType_h__

#include "voSubtitleType.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VOOSMP_MAX_URL_LEN          2048

/**
 * Enumeration of return codes
 */
#define VOOSMP_ERR_None             0X00000000  /*!< No Error */
#define VOOSMP_ERR_EOS              0X00000001  /*!< Reached the end of source */
#define VOOSMP_ERR_Retry            0X00000002  /*!< Buffer was empty, retry */
#define VOOSMP_ERR_FormatChange     0X00000003  /*!< Format was changed */
#define VOOSMP_ERR_Audio_No_Now     0X00000010  /*!< Audio is not currently available */
#define VOOSMP_ERR_Video_No_Now     0X00000011  /*!< Video is not currently available */
#define VOOSMP_ERR_FLush_Buffer     0X00000012  /*!< Buffer needs to be flushed */
#define VOOSMP_ERR_Video            0X80000004  /*!< Error from video codec */
#define VOOSMP_ERR_Audio            0X80000005  /*!< Error from audio codec */
#define VOOSMP_ERR_OutMemory        0X80000006  /*!< Out of memory */
#define VOOSMP_ERR_Pointer          0X80000007  /*!< Pointer is invalid */
#define VOOSMP_ERR_ParamID          0X80000008  /*!< Parameter ID is not supported or parameter value is invalid*/
#define VOOSMP_ERR_Status           0X80000009  /*!< Status is incorrect */
#define VOOSMP_ERR_Implement        0X8000000A  /*!< This function is not implemented */
#define VOOSMP_ERR_SmallSize        0X8000000B  /*!< Input buffer is too small */
#define VOOSMP_ERR_OutOfTime        0X8000000C  /*!< Out of time (late) */
#define VOOSMP_ERR_WaitTime         0X8000000D  /*!< Waiting reference time (early) */
#define VOOSMP_ERR_Unknown          0X8000000E  /*!< Unknown error */
#define VOOSMP_ERR_JNI              0X8000000F  /*!< JNI error. Not used for iOS platforms */
#define VOOSMP_ERR_SeekFailed       0X80000010  /*!< Seek failed */
#define VOOSMP_ERR_LicenseFailed    0X80000011  /*!< License check failed */
#define VOOSMP_ERR_Args             0X80000013  /*!< Invalid argument */
#define VOOSMP_ERR_MultipleInstancesNotSupport 0X80000014  /** Multiple player instances are not supported */
    
   
   

   
/**
 * Enumeration of callback IDs
 */
#define VOOSMP_CB_PlayComplete          0X00000001  /*!< Playback of source was finished */
#define VOOSMP_CB_BufferStatus          0X00000002  /*!< Buffer status. Parameter 1 is int *. Range is (0 - 100) */
#define VOOSMP_CB_VideoStartBuff        0X00000003  /*!< Video stream started buffering */
#define VOOSMP_CB_VideoStopBuff         0X00000004  /*!< Video stream stopped buffering */
#define VOOSMP_CB_AudioStartBuff        0X00000005  /*!< Audio stream started buffering */
#define VOOSMP_CB_AudioStopBuff         0X00000006  /*!< Audio stream stopped buffering */
#define VOOSMP_CB_SourceBuffTime        0X00000007  /*!< Buffer time in source, int * A, int * V */
#define VOOSMP_CB_VideoDelayTime        0X00000008  /*!< Video delay time, int * ms */
#define VOOSMP_CB_VideoLastTime         0X00000009  /*!< Video last render time, int * ms */
#define VOOSMP_CB_VideoDropFrames       0X0000000A  /*!< Video dropped frames, int * */
#define VOOSMP_CB_VideoFrameRate        0X0000000B  /*!< Video playback frame rate last 5 seconds, int * */
#define VOOSMP_CB_Error                 0X8000000C  /*!< Unknown error */
#define VOOSMP_CB_SeekComplete          0X0000000D  /*!< Seek is finished. Parameter 1 should be the seek return timestamp, long long* , parameter 2 should be the pointer of VOOSMP_ERR_XXX value, VOOSMP_ERR_SeekFailed or VOOSMP_ERR_None */
#define VOOSMP_CB_VideoAspectRatio      0X0000000E  /*!< Video aspect ratio changed, int * , refer to VOOSMP_ASPECT_RATIO */
#define VOOSMP_CB_VideoSizeChanged      0X0000000F  /*!< Video size changed, width int *, height int * */
#define VOOSMP_CB_CodecNotSupport       0X80000010  /*!< Codec is not supported */
#define VOOSMP_CB_Deblock               0X00000011  /*!< Video codec deblock event, int *, 0:disable, 1:enable */
#define VOOSMP_CB_MediaTypeChanged      0X00000012  /*!< Notify if video/audio is available, int *, refer to VOOSMP_AVAILABLE_TRACK_TYPE */
#define VOOSMP_CB_HWDecoderStatus       0X00000013  /*!< Notify hardware decoder available event, int *, 0:error, 1:available */
#define VOOSMP_CB_LanguageInfoAvailable 0X00000015   /*!< Notify subtitle language info is parsed */
#define VOOSMP_CB_VideoRenderStart      0X00000016   /*!< Notify video render started */
#define VOOSMP_CB_OpenSource            0X00000017   /*!< Notify Open Source completed */
#define VOOSMP_CB_Metadata_Arrive       0X00000018   /*!< Metadata found, pass the type of metadata, int *, refer to VOOSMP_SOURCE_STREAMTYPE */
#define VOOSMP_CB_SEI_INFO              0X00000019   /*!< Notify SEI info, parameter is VOOSMP_SEI_INFO */
#define VOOSMP_CB_Audio_Render_Failed   0X8000001A   /*!< Notify audio render initialization failed */
#define VOOSMP_CB_Video_Render_Complete 0X0000001B   /*!< Notify rendering finished after each frame,and pass the frame's timestamp, long long* */
#define VOOSMP_CB_PCM_OUTPUT            0X0000001C   /*!< This event will be issued only when PCM data output is enabled. Parameter 1 should be VOOSMP_PCMBUFFER */
#define VOOSMP_CB_LicenseFailed         0X8000001D   /*!< Notify license check failed */
#define VOOSMP_CB_FullScreen            0X0000001E   /*!< Notify enter fullscreen or not,int *, 0:exit fullscreen, 1:enter fullscreen */
#define VOOSMP_CB_VR_USERCALLBACK		0X0000001F   /*!< Notify before render, user can draw something in it, Parameter 1 is VOOSMP_VR_USERCALLBACK_TYPE * */
#define VOOSMP_CB_VideoRenderCreateFail 0X80000020   /*!< Notify create video render failed */
#define VOOSMP_CB_AudioRenderStart		0X80000021   /*!< Notify audio render started */
#define VOOSMP_CB_SubtitleRenderStart	0X80000022   /*!< Notify subtitle render started */
#define VOOSMP_CB_NativePlayerFailed	0X80000023   /*!< Notify native player occurs fatal error while playabck */


/**
 * Enumeration of parameter IDs
 */
#define VOOSMP_PID_AUDIO_VOLUME             0X00000001  /*!< Set the audio volume, the volume range is 0-100, 0 is mute and 100 is max */
#define VOOSMP_PID_DRAW_RECT                0X00000002  /*!< Set the draw area, or get the real area from engine, VOOSMP_RECT * */
#define VOOSMP_PID_DRAW_COLOR               0X00000003  /*!< Set the draw color type, VOOSMP_COLORTYPE * */
#define VOOSMP_PID_FUNC_LIB                 0X00000004  /*!< Set Lib function set. VOOSMP_LIB_FUNC * It will be supported in next build */
#define VOOSMP_PID_MAX_BUFFTIME             0X00000005  /*!< Set data source max buffer time. int * */
#define VOOSMP_PID_MIN_BUFFTIME             0X00000006  /*!< Set data source min buffer time. int * */
#define VOOSMP_PID_VIDEO_REDRAW             0X00000007  /*!< Set Redraw the video in render */
#define VOOSMP_PID_AUDIO_EFFECT_ENABLE      0X00000008  /*!< Set audio effect enable int *, >0:enable, 0:disable */
#define VOOSMP_PID_LICENSE_TEXT             0X00000009  /*!< Set check license text. char * */
#define VOOSMP_PID_LICENSE_FILE_PATH        0X0000000A  /*!< Set license file path. char * */
#define VOOSMP_PID_PLAYER_PATH              0X0000000B  /*!< Set player work path. char * */
#define VOOSMP_PID_FUNC_DRM                 0X0000000C  /*!< Set DRM function set. Refer to VOOSMP_DRM_CALLBACK */
#define VOOSMP_PID_AUDIO_REND_ADJUST_TIME   0X0000000D  /*!< Set audio render adjust time. int * */
#define VOOSMP_PID_STATUS                   0X0000000E  /*!< Get engine status. VOOSMP_STATUS * */
#define VOOSMP_PID_ZOOM_MODE                0X0000000F  /*!< Override automatic screen width and height adjustment according to zoom mode. VOOSMP_ZOOM_MODE * */
#define VOOSMP_PID_CPU_NUMBER               0X00000010  /*!< Set device CPU number * */
#define VOOSMP_PID_LISTENER                 0X00000011  /*!< Set synchronous event callback function, refer to VOOSMP_LISTENERINFO* */
#define VOOSMP_PID_AUDIO_REND_BUFFER_TIME   0X00000012  /*!< Set audio render buffer time * */
#define VOOSMP_PID_SENDBUFFERFUNC           0X00000013  /*!< Get send buffer callback function, refer to VOOSMP_SENDBUFFER_FUNC * */
#define VOOSMP_PID_AUDIO_FORMAT             0X00000014  /*!< Get audio format, VOOSMP_AUDIO_FORMAT * */
#define VOOSMP_PID_VIDEO_FORMAT             0X00000015  /*!< Get video format, VOOSMP_VIDEO_FORMAT * */
#define VOOSMP_PID_VIDEO_RENDER_TYPE        0X00000016  /*!< Set video render type, int *, refer to VOOSMP_RENDER_TYPE */
#define VOOSMP_PID_PLAYER_TYPE              0X00000017  /*!< Get media framework type */
#define VOOSMP_PID_AUDIO_DECODER_FILE_NAME  0X00180000  /*!< Set audio decoder module file name. char * , VOOSMP_PID_AUDIO_DECODER_FILE_NAME is the base ID value, decoder module ID should be (VOOSMP_PID_AUDIO_DECODER_FILE_NAME+code type) value. For instance, if you want to set AAC decoder module name, it should be (VOOSMP_PID_AUDIO_DECODER_FILE_NAME+VOOSMP_AUDIO_CodingAAC) */
#define VOOSMP_PID_VIDEO_DECODER_FILE_NAME  0X00190000  /*!< Set video decoder module file name. char * , same as VOOSMP_PID_AUDIO_DECODER_FILE_NAME */
#define VOOSMP_PID_PERFORMANCE_DATA         0X0000001A  /*!< Get/Set codec performance data, VOOSMP_PERF_DATA* */
#define VOOSMP_PID_FUNC_READ_IO             0X0000001B  /*!< Set the function address of read data, VOOSMP_READDATA_FUNC* */
#define VOOSMP_PID_AUDIO_STEP_BUFFTIME      0X0000001C  /*!< Set audio sink step time, int * ms */
#define VOOSMP_PID_AUDIO_SINK_BUFFTIME      0X0000001D  /*!< Set audio sink buffer time, int * ms */
#define VOOSMP_PID_AUDIO_STREAM_ONOFF       0X0000001E  /*!< Set enable/disable playback audio frame, int *, 0:disable, 1:enable */
#define VOOSMP_PID_VIDEO_STREAM_ONOFF       0X0000001F  /*!< Set enable/disable playback video frame, int *, 0:disable, 1:enable */
#define VOOSMP_PID_AUDIO_DEC_BITRATE        0X00000020  /*!< Get bitrate of audio codec, int * */
#define VOOSMP_PID_VIDEO_DEC_BITRATE        0X00000021  /*!< Get bitrate of video codec, int * */
#define VOOSMP_PID_MFW_CFG_FILE             0X00000022  /*!< Set media framework configuration file, char * */
#define VOOSMP_PID_AUDIO_DECODER_API_NAME   0X00230000  /*!< Set audio decoder module API name. char * , VOOSMP_PID_AUDIO_DECODER_API_NAME is the base ID value, decoder API ID should be (VOOSMP_PID_AUDIO_DECODER_API_NAME + code type) value, for instance, if you want to set AAC decoder API name, it should be (VOOSMP_PID_AUDIO_DECODER_API_NAME+VOOSMP_AUDIO_CodingAAC) */
#define VOOSMP_PID_VIDEO_DECODER_API_NAME   0X00240000  /*!< Set video decoder API name. char * , same as VOOSMP_PID_AUDIO_DECODER_API_NAME */
#define VOOSMP_PID_DEBLOCK_ONOFF            0X00000025  /*!< Set enable/disable deblock, int *, 0:disable, 1:enable */
#define VOOSMP_PID_CPU_INFO                 0X00000026  /*!< Get/Set CPU information, refer to VOOSMP_CPU_INFO */
#define VOOSMP_PID_LOAD_AUDIO_EFFECT_MODULE 0X00000027  /*!< Set enable/disable load audio effect library, 0:disable, 1:enable, default is 0 */
#define VOOSMP_PID_AUTHENTICATION_INFO      0X00000028  /*!< Set authentication information, for instance user name, char* */
#define VOOSMP_PID_COMMON_LOGFUNC           0X00000029  /*!< Set Log function pointer */
#define VOOSMP_PID_COMMON_CCPARSER          0X0000002A  /*!< Set CC enable or disable */
#define VOOSMP_PID_LICENSE_CONTENT          0X0000002B  /*!< Set license content, char * */
#define VOOSMP_PID_SOURCE_FILENAME          0X0000002C  /*!< Set File Source name. Find relational smi, srt subtitle file. char * */
#define VOOSMP_PID_LIB_NAME_PREFIX          0X0000002D  /*!< Set prefix of library name. char * */
#define VOOSMP_PID_LIB_NAME_SUFFIX          0X0000002E  /*!< Set suffix of library name. char * */
#define VOOSMP_PID_COMMAND_STRING           0X0000002F  /*!< Get/Set command string, module needs to parse this string to get the exact command. Refer to  char * */
#define VOOSMP_PID_VIDEO_ASPECT_RATIO       0X00000030  /*!< Set aspect ratio. int *, refer to VOOSMP_ASPECT_RATIO */
#define VOOSMP_PID_CAP_TABLE_PATH           0X00000031  /*!< Set cap information table path, char* */
#define VOOSMP_PID_LOW_LATENCY              0X00000032  /*!< Enable/disable audio/video output immediately for rendering, int *, 1:enable, 0:disable */
#define VOOSMP_PID_VIEW_ACTIVE              0X00000033  /*!< Video view go to foreground, set view handle, void* */  
#define VOOSMP_PID_VIDEO_PERFORMANCE_CACHE  0X00000034  /*!< Set the performance data buffer time, in seconds, int * */
#define VOOSMP_PID_VIDEO_PERFORMANCE_OPTION 0X00000035  /*!< Get the performance data statistics, refer to VOOSMP_PERFORMANCEDATA* */    
#define VOOSMP_PID_SUBTITLE_SETTINGS        0X00000036  /*!< Get/Set subtitle settings, refer to VOOSMP_SUBTITLE_SETTINGS */
#define VOOSMP_PID_SUBTITLE_FILE_NAME       0X00000037  /*!< Set Subtitle file full path */
#define VOOSMP_PID_VIDEO_PERFORMANCE_ONOFF  0X00000038  /*!< Set enable/disable the performance data statistics, int 0:disable, 1:enable, default is 0 * */    
#define VOOSMP_PID_SUBTITLE_SETTINGS_ONOFF  0X00000039  /*!< Enable/disable subtitle settings, 1:enable, 0:disable, default is 0 */
#define VOOSMP_PID_APPLICATION_SUSPEND      0X0000003A  /*!< Set application suspend, such as go background, 0:pause playback, 1:only audio continue playback,video stopped */
#define VOOSMP_PID_APPLICATION_RESUME       0X0000003B  /*!< Set application resume, such as go foreground, new view handle will need to be set, void* */
#define VOOSMP_PID_READ_SUBTITLE_CALLBACK   0X0000003C  /*!< Set read closed captions or subtitle buffer callback, refer to VOOSMP_READ_SUBTITLE_FUNC */
#define VOOSMP_PID_AUDIO_SINK               0X0000003D  /*!< Set Audio sink void* */    
#define VOOSMP_PID_BITMAP_HANDLE            0x0000003E  /*!< Set optimized bitmap object */
#define VOOSMP_PID_RETRIEVE_SEI_INFO        0x0000003F  /*!< For set, the flag which you like to retrieve SEI, int*, parameter is combination of VOOSMP_SEI_FLAG, VOOSMP_FLAG_SEI_NONE indicates it is disabled, VOOSMP_FLAG_SEI_MAX indicates retrieve all SEI info. For get, parameter is VOOSMP_SEI_INFO, nFlag in VOOSMP_SEI_INFO indicates which info you like to retrieve */
#define VOOSMP_PID_SEI_EVENT_INTERVAL       0x00000040  /*!< Set SEI event interval. If want to disable event notification, then the corresponding int value is set to -1. int *, in ms */
#define VOOSMP_PID_VIDEO_OVERLAY_CONTROL    0x00000041  /*!< Set video render overlay control, int *, 0:disable, 1:enable */
#define VOOSMP_PID_ONREQUEST_LISTENER       0x00000042  /*!< Set asynchronous event callback function, refer to VOOSMP_LISTENERINFO* */
#define VOOSMP_PID_SCREEN_BRIGHTNESS_MODE   0x00000043  /*!< Get/Set screen screen brightness management mode, int*, refer to VOOSMP_SCREEN_BRIGHTNESS_MODE,only Android support */
#define VOOSMP_PID_SCREEN_BRIGHTNESS_VALUE  0x00000044  /*!< Get/Set screen screen brightness value, int*, 0~100 */
#define VOOSMP_PID_IOMX_PROBE  				0x00000045  /*!< Hardware codec probe */
#define VOOSMP_PID_GET_ONSTREAMSRC_HANDLE  	0x00000046  /*!< Get the OnStreamSrc Handler */
#define VOOSMP_PID_AUDIO_PCM_OUTPUT         0x00000047  /*!< Set enable/disable the audio pcm output, param is int type. 1 to enable, 0 to disable. Default is 0. */
#define VOOSMP_PID_AUDIO_DECODER_TYPE       0x00000048  /*!< Set audio decoder type, refer to VOOSMP_AUDIO_DECODER_TYPE.Default is VOOSMP_AUDIO_DECODER_TYPE_SOFTWARE */
#define VOOSMP_PID_MODULE_VERSION           0x00000049  /*!< Get module version, refer to VOOSMP_MODULE_VERSION */
#define VOOSMP_PID_AUDIO_PLAYBACK_SPEED     0x00000050  /*!< Get/Set audio playback speed, float*, range is 0.5 ~ 2.0, default is 1.0 */
#define VOOSMP_PID_VIDEO_DC                 0x00000051  /*!< Set DC to video render */
#define VOOSMP_PID_CLEAR_VIDEO_AREA         0x00000052  /*!< Redraw video area using black color */
#define VOOSMP_PID_DROP_VIDEO_RENDER_FRAME  0x00000053  /*!< Notify video render to drop the current frame */
#define VOOSMP_PID_DRM_UNIQUE_IDENTIFIER    0X00000054  /*!< Set DRM unique identifier */
#define VOOSMP_PID_VR_USERCALLBACK			0X00000055	/*!< Enable/Disable video render user callback, bool *, DirectDraw render implement only currently */
#define VOOSMP_PID_DRAW_VIDEO_DIRECTLY      0X00000056	/*!< Enable/Disable draw video directly, bool * */
#define VOOSMP_PID_CAPTURE_VIDEO_IMAGE      0x00000057  /*!< Get function. Capture current video image, refer to VOOSMP_IMAGE_DATA*. */
#define VOOSMP_PID_AUDIO_RENDER_DATA_FORMAT 0x00000058  /*!< Get audio render data format after decoding, int* */
#define VOOSMP_PID_AUDIO_DSP_CLOCK          0x00000059  /*!< set the interface IAudioDSPClock interface */    
#define VOOSMP_PID_PAUSE_REFERENCE_COLOCK	0x0000005A	/*!< Set pause reference clock or not. bool * */
#define VOOSMP_PID_AUDIO_RENDER_LATENCY     0x0000005B	/*!< Get audio render latency from hardware. int* ms */
#define VOOSMP_PID_AUDIO_RENDER_FORMAT      0x0000005C  /*!< Set audio render format, VOOSMP_AUDIO_RENDER_FORMAT * */
#define VOOSMP_PID_LOAD_DOLBY_DECODER_MODULE 0x0000005D /*!< Set enable/disable loading Dolby decoder library, 0:disable, 1:enable, default is 1 */
#define VOOSMP_PID_PREVIEW_SUBTITLE         0x0000005E  /*!< Preview subtitle by sending some sample text to be rendered, VOOSMP_PREVIEW_SUBTITLE_INFO * */

/**
* Enumeration of open source flags, used in Open() API
 */
typedef enum
{
    VOOSMP_FLAG_SOURCE_URL          = 0X00000001,       /*!< Source is URL */
    VOOSMP_FLAG_SOURCE_READBUFFER   = 0X00000002,       /*!< Source is out of read buffer */
    VOOSMP_FLAG_SOURCE_SENDBUFFER   = 0X00000004,       /*!< Source is out of send buffer */
    VOOSMP_FLAG_SOURCE_HANDLE       = 0X00000008,       /*!< Source is Handle */
    VOOSMP_FLAG_SOURCE_OPEN_SYNC    = 0X00000010,       /*!< Indicates the source was opened in synchronous mode.
                                                         The function will not return until the open operation is complete. */
    VOOSMP_FLAG_SOURCE_OPEN_ASYNC   = 0X00000020,       /*!< Indicates the source was opened in asynchronous mode.
                                                         The function will return immediately after the call. The completion of 
                                                         the open will be indicated by the VOOSMP_SRC_CB_Open_Finished event. */
    VOOSMP_FLAG_SOURCE_MAX          = 0X7FFFFFFF        /*!< Max value definition */
}VOOSMP_OPEN_SOURCE_FLAG;


/**
 * Enumeration of send/read buffer flags, used in struct VOOSMP_BUFFERTYPE
 */
typedef enum
{
    VOOSMP_FLAG_BUFFER_UNKNOWN              = 0X80000000,       /*!< Buffer is unknown, needs checking */
    VOOSMP_FLAG_BUFFER_KEYFRAME             = 0X00000001,       /*!< Buffer is key frame */
    VOOSMP_FLAG_BUFFER_NEW_PROGRAM          = 0X00000002,       /*!< Buffer starts new program, codec is changed, bitrate or sample rate is changed, timestamp is reset, pData point to VOOSMP_BUFFER_FORMAT, pBuffer is NULL */
    VOOSMP_FLAG_BUFFER_NEW_FORMAT           = 0X00000004,       /*!< Buffer starts new format, codec is not changed, pData point to VOOSMP_BUFFER_FORMAT, pBuffer is NULL */
    VOOSMP_FLAG_BUFFER_HEADDATA             = 0X00000020,       /*!< Buffer is header data */
    VOOSMP_FLAG_BUFFER_DROP_FRAME           = 0X00001000,       /*!< Previous buffer was dropped */
    VOOSMP_FLAG_BUFFER_DELAY_TO_DROP        = 0x00020000,       /*!< Buffer needed to be dropped due to delay */
    VOOSMP_FLAG_BUFFER_TIMESTAMP_RESET      = 0X00080000,       /*!< Sample timestamp rollback */
	VOOSMP_FLAG_BUFFER_ADD_DRMDATA          = 0x00100000,       /*!< Buffer has been appended by drm information */
    VOOSMP_FLAG_BUFFER_FRAME_DECODE_ONLY	= 0x00200000,       /*!< Buffer shall only be decoded, but not be rendered */

    VOOSMP_FLAG_BUFFER_MAX                  = 0X7FFFFFFF        /*!< Max value definition */
}VOOSMP_BUFFER_FLAG;
   
   
/**
 * General audio format info
 */
typedef struct
{
    int     SampleRate;  /*!< Sample rate */
    int     Channels;    /*!< Channel count */
    int     SampleBits;  /*!< Bits per sample */
}VOOSMP_AUDIO_FORMAT;

   
/**
 * General video format info
 */
typedef struct
{
    int     Width;       /*!< Width */
    int     Height;      /*!< Height */
    int     Type;        /*!< Color type */
}VOOSMP_VIDEO_FORMAT;
   
   
/**
 * Buffer format structure
 */
typedef struct
{
    int                 nStreamtype;        /*!< Stream type, refer to VOOSMP_SOURCE_STREAMTYPE */
    int                 nCodec;             /*!< Audio or Video format, refer to VOOSMP_AUDIO_CODINGTYPE or VOOSMP_VIDEO_CODINGTYPE */
    int                 nFourCC;            /*!< Audio or Video FourCC */
   
    union
    {
        VOOSMP_AUDIO_FORMAT     audio;      /*!< Audio format info */
        VOOSMP_VIDEO_FORMAT     video;      /*!< Video format info */
    } sFormat;

    int                 nHeadDataLen;       /*!< Header data length */
    char*               pHeadData;          /*!< Header data buffer */
}VOOSMP_BUFFER_FORMAT;


/**
 * Call back function. Update the status from SDK
 * \param nID [in] the id of special event
 * \param pParam1 [in/out] the first parameter
 * \param pParam2 [in/out] the second parameter
 */
typedef int (* VOOSMPListener) (void * pUserData, int nID, void * pParam1, void * pParam2);



/**
 * General data buffer, used as input or output
 */
typedef struct
{
    int             nSize;          /*!< Buffer size in byte */
    unsigned char * pBuffer;        /*!< Buffer pointer */
    long long       llTime;         /*!< [in/out] The time of the buffer */
    int             nDuration;      /*!< [In]AV offset, [out]Duration of buffer(MS) */  
    int             nFlag;          /*!< Flag of the buffer, refer to VOOSMP_BUFFER_FLAG
                                        VOOSMP_FLAG_BUFFER_KEYFRAME:    pBuffer is video key frame
                                        VOOSMP_FLAG_BUFFER_NEW_PROGRAM: pBuffer is NULL, pData is VOOSMP_BUFFER_FORMAT
                                        VOOSMP_FLAG_BUFFER_NEW_FORMAT:  pBuffer is NULL, pData is VOOSMP_BUFFER_FORMAT
                                        VOOSMP_FLAG_BUFFER_HEADDATA:    pBuffer is head data
                                        VOOSMP_FLAG_BUFFER_SUBTITLE:    pBuffer can be NULL or not NULL, pData is VOOSMP_SUBTITLE_INFO */
    void *          pData;          /*!< Special data pointer, depends on the flag */
    long long       llReserve;      /*!< Reserve value */
}VOOSMP_BUFFERTYPE;

    
/**
 * PCM data buffer
 */
typedef struct
{
    /*!< the timestamp of buffer */
    long long nTimestamp;
    
    /*!< the size of buffer in bytes */
    int  nBufferSize;
    
    /*!< return buffer array */
    unsigned char * pBuffer;
}VOOSMP_PCMBUFFER;


/**
 * Definition of rect structure
 */
typedef struct
{
    int                 nLeft;      /*!< Left */
    int                 nTop;       /*!< top */
    int                 nRight;     /*!< right */
    int                 nBottom;    /*!< bottom */
}VOOSMP_RECT;


/**
* The lib operator
 */
typedef struct
{
    void *  pUserData;                                                                      /*!< User data */
    void *  (* LoadLib) (void * pUserData, char * pLibName, int nFlag);                     /*!< Address of load lib API */
    void *  (* GetAddress) (void * pUserData, void * hLib, char * pFuncName, int nFlag);    /*!< Address of get addr API */
    int     (* FreeLib) (void * pUserData, void * hLib, int nFlag);                         /*!< Address of free lib API */
}VOOSMP_LIB_FUNC;


/**
* The read buffer function
 */
typedef struct
{
    void *  pUserData;                                                      /*!< User data */
    int     (* ReadAudio) (void * pUserData, VOOSMP_BUFFERTYPE * pBuffer);  /*!< Address of read audio API */
    int     (* ReadVideo) (void * pUserData, VOOSMP_BUFFERTYPE * pBuffer);  /*!< Address of read video API */
}VOOSMP_READBUFFER_FUNC;


/**
* The send buffer function
 */
typedef struct
{
    void *  pUserData;                                                                  /*!< User data */
    int     (* SendData) (void * pUserData, int nSSType, VOOSMP_BUFFERTYPE * pBuffer);  /*!< Address of send buffer API, nSSType value is enum VOOSMP_SOURCE_STREAMTYPE */
}VOOSMP_SENDBUFFER_FUNC;


/**
 * Enumeration used to define the possible source stream types
 */
typedef enum
{
    VOOSMP_SS_AUDIO             = 0X00000001,   /**< Source stream is Audio */
    VOOSMP_SS_VIDEO             = 0X00000002,   /**< Source stream is Video */
    VOOSMP_SS_IMAGE             = 0X00000003,   /*!< Image track */
    VOOSMP_SS_STREAM            = 0X00000004,   /*!< Stream track */
    VOOSMP_SS_SCRIPT            = 0X00000005,   /*!< Script track */
    VOOSMP_SS_HINT              = 0X00000006,   /*!< Hint track */
    VOOSMP_SS_RTSP_VIDEO        = 0X00000007,   /*!< RTSP streaming video track */
    VOOSMP_SS_RTSP_AUDIO        = 0X00000008,   /*!< RTSP streaming audio track */
    VOOSMP_SS_SUBTITLE          = 0X00000009,   /*!< Subtitle track & closed captions */
    VOOSMP_SS_RICHMEDIA         = 0X0000000A,   /*!< Rich media track */
    VOOSMP_SS_TRACKINFO         = 0X0000000B,   /*!< Track info, refer to VOOSMP_TRACK_INFO* */
    VOOSMP_SS_TS                = 0X0000000C,   /*!< Source stream is TS */
    VOOSMP_SS_MFS               = 0X0000000D,   /*!< Source stream is MFS */
    VOOSMP_SS_AUDIO_GROUP		= 0X0000000E,	/*!< Source stream is audio,number of track is unknown */
	VOOSMP_SS_VIDEO_GROUP		= 0X0000000F,	/*!< Source stream is video,number of track is unknown */
	VOOSMP_SS_SUBTITLE_GROUP	= 0X00000010,	/*!< Source stream is subtitle,number of track is unknown */
	VOOSMP_SS_MUX_GROUP			= 0X00000011,	/*!< Source format of stream is unknown */
    VOOSMP_SS_MAX               = 0X7FFFFFFF    /*!< Max value definition */
}VOOSMP_SOURCE_STREAMTYPE;

/**
 * Enumeration used to define the possible audio codings
 */
typedef enum
{
    VOOSMP_AUDIO_CodingUnused = 0,  /**< Placeholder value when coding is N/A */
    VOOSMP_AUDIO_CodingPCM,         /**< Any variant of PCM coding */
    VOOSMP_AUDIO_CodingADPCM,       /**< Any variant of ADPCM encoded data */
    VOOSMP_AUDIO_CodingAMRNB,       /**< Any variant of AMR encoded data */
    VOOSMP_AUDIO_CodingAMRWB,       /**< Any variant of AMR encoded data */
    VOOSMP_AUDIO_CodingAMRWBP,      /**< Any variant of AMR encoded data */
    VOOSMP_AUDIO_CodingQCELP13,     /**< Any variant of QCELP 13kbps encoded data */
    VOOSMP_AUDIO_CodingEVRC,        /**< Any variant of EVRC encoded data */
    VOOSMP_AUDIO_CodingAAC,         /**< Any variant of AAC encoded data, 0xA106 - ISO/MPEG-4 AAC, 0xFF - AAC */
    VOOSMP_AUDIO_CodingAC3,         /**< Any variant of AC3 encoded data */
    VOOSMP_AUDIO_CodingFLAC,        /**< Any variant of FLAC encoded data */
    VOOSMP_AUDIO_CodingMP1,         /**< Any variant of MP1 encoded data */
    VOOSMP_AUDIO_CodingMP3,         /**< Any variant of MP3 encoded data */
    VOOSMP_AUDIO_CodingOGG,         /**< Any variant of OGG encoded data */
    VOOSMP_AUDIO_CodingWMA,         /**< Any variant of WMA encoded data */
    VOOSMP_AUDIO_CodingRA,          /**< Any variant of RA encoded data */
    VOOSMP_AUDIO_CodingMIDI,        /**< Any variant of MIDI encoded data */
    VOOSMP_AUDIO_CodingDRA,         /**< Any variant of DRA encoded data */
    VOOSMP_AUDIO_CodingG729,        /**< Any variant of G729 encoded data */
    VOOSMP_AUDIO_CodingEAC3,        /**< Any variant of Enhanced AC3 encoded data */
    VOOSMP_AUDIO_CodingAPE,         /**< Any variant of APE encoded data */
    VOOSMP_AUDIO_CodingALAC,        /**< Any variant of ALAC encoded data */
    VOOSMP_AUDIO_CodingDTS,         /**< Any variant of DTS encoded data */
    VOOSMP_AUDIO_Coding_MAX     = 0X7FFFFFFF
}VOOSMP_AUDIO_CODINGTYPE;

   
/**
 * Video Coding types
 */
typedef enum
{
    VOOSMP_VIDEO_CodingUnused = 0, /*!< Value when coding is N/A */
    VOOSMP_VIDEO_CodingMPEG2,      /*!< A.K.A. H.262 */
    VOOSMP_VIDEO_CodingH263,       /*!< H.263 */
    VOOSMP_VIDEO_CodingS263,       /*!< S.263 */
    VOOSMP_VIDEO_CodingMPEG4,      /*!< MPEG-4 */
    VOOSMP_VIDEO_CodingH264,       /*!< H.264/AVC */
    VOOSMP_VIDEO_CodingWMV,        /*!< All versions of Windows Media Video */
    VOOSMP_VIDEO_CodingRV,         /*!< All versions of Real Video */
    VOOSMP_VIDEO_CodingMJPEG,      /*!< Motion JPEG */
    VOOSMP_VIDEO_CodingDIVX,       /*!< DIV3 */
    VOOSMP_VIDEO_CodingVP6,        /*!< VP6 */
    VOOSMP_VIDEO_CodingVP8,        /*!< VP8 */
    VOOSMP_VIDEO_CodingVP7,        /*!< VP7 */
    VOOSMP_VIDEO_CodingVC1,        /*VC1: WMV3, WMVA, WVC1 */
    VOOSMP_VIDEO_CodingH265,       /*!< H265 */
    VOOSMP_VIDEO_Coding_Max     = 0X7FFFFFFF
}VOOSMP_VIDEO_CODINGTYPE;
   

/**
 * Enumeration of color formats
 */
typedef enum
{
    VOOSMP_COLOR_YUV_PLANAR444          = 0,        /*!< YUV planar mode:444  vertical sample is 1, horizontal is 1 */
    VOOSMP_COLOR_YUV_PLANAR422_12       = 1,        /*!< YUV planar mode:422, vertical sample is 1, horizontal is 2 */
    VOOSMP_COLOR_YUV_PLANAR422_21       = 2,        /*!< YUV planar mode:422  vertical sample is 2, horizontal is 1 */
    VOOSMP_COLOR_YUV_PLANAR420          = 3,        /*!< YUV planar mode:420  vertical sample is 2, horizontal is 2 */
    VOOSMP_COLOR_YUV_PLANAR411          = 4,        /*!< YUV planar mode:411  vertical sample is 1, horizontal is 4 */
    VOOSMP_COLOR_YUV_PLANAR411V         = 5,        /*!< YUV planar mode:411  vertical sample is 4, horizontal is 1 */
    VOOSMP_COLOR_GRAY_PLANARGRAY        = 6,        /*!< gray planar mode, just Y value */
    VOOSMP_COLOR_YUYV422_PACKED         = 7,        /*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: Y0 U0 Y1 V0 */
    VOOSMP_COLOR_YVYU422_PACKED         = 8,        /*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: Y0 V0 Y1 U0 */
    VOOSMP_COLOR_UYVY422_PACKED         = 9,        /*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: U0 Y0 V0 Y1 */
    VOOSMP_COLOR_VYUY422_PACKED         = 10,       /*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: V0 Y0 U0 Y1 */
    VOOSMP_COLOR_YUV444_PACKED          = 11,       /*!< YUV packed mode:444, vertical sample is 1, horizontal is 1, data: Y U V */
    VOOSMP_COLOR_YUV_420_PACK           = 12,       /*!< YUV planar mode:420  vertical sample is 2, horizontal is 2  , Y planar, UV Packed */
    VOOSMP_COLOR_YUV_420_PACK_2         = 13,       /*!< YUV planar mode:420  vertical sample is 2, horizontal is 2  , Y planar, VU Packed */
    VOOSMP_COLOR_YVU_PLANAR420          = 14,       /*!< YUV planar mode:420 vertical sample is 2, horizontal is 2 , Y planar, V planar, U planar */
    VOOSMP_COLOR_YVU_PLANAR422_12       = 15,       /*!< YUV planar mode:422 vertical sample is 1, horizontal is 2 , Y planar, V planar, U planar */
    VOOSMP_COLOR_YUYV422_PACKED_2       = 16,       /*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: Y1 U0 Y0 V0 */
    VOOSMP_COLOR_YVYU422_PACKED_2       = 17,       /*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: Y1 V0 Y0 U0 */
    VOOSMP_COLOR_UYVY422_PACKED_2       = 18,       /*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: U0 Y1 V0 Y0 */
    VOOSMP_COLOR_VYUY422_PACKED_2       = 19,       /*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: V0 Y1 U0 Y0 */
    VOOSMP_COLOR_RGB565_PACKED          = 20,       /*!< RGB packed mode, data: B:5 G:6 R:5 */
    VOOSMP_COLOR_RGB555_PACKED          = 21,       /*!< RGB packed mode, data: B:5 G:5 R:5 */
    VOOSMP_COLOR_RGB888_PACKED          = 22,       /*!< RGB packed mode, data: B G R */
    VOOSMP_COLOR_RGB32_PACKED           = 23,       /*!< RGB packed mode, data: B G R A */
    VOOSMP_COLOR_RGB888_PLANAR          = 24,       /*!< RGB planar mode */
    VOOSMP_COLOR_YUV_PLANAR420_NV12     = 25,       /*!< YUV planar mode:420  vertical sample is 2, horizontal is 2 */
    VOOSMP_COLOR_ARGB32_PACKED          = 26,       /*!< ARGB packed mode, data: B G R A */
    VOOSMP_COLOR_TYPE_MAX               = 0X7FFFFFF
}VOOSMP_COLORTYPE;
   
   
/**
 * Definition of aspect ratio type
 */
typedef enum
{
    VOOSMP_RATIO_00         = 0X00,  /*!< Width and height is width: height,deprecated,it's same with VOOSMP_RATIO_ORIGINAL,and will be removed in the future.Prefer to use VOOSMP_RATIO_ORIGINAL */
    VOOSMP_RATIO_11         = 0X01,  /*!< Width and Height is 1 : 1 */
    VOOSMP_RATIO_43         = 0X02,  /*!< Width and Height is 4 : 3 */
    VOOSMP_RATIO_169        = 0X03,  /*!< Width and Height is 16 : 9 */
    VOOSMP_RATIO_21         = 0X04,  /*!< Width and Height is 2 : 1 */
    VOOSMP_RATIO_2331       = 0X05,  /*!< Width and Height is 2.33 : 1 */
    VOOSMP_RATIO_AUTO       = 0X06,  /*!< Use aspect ratio value from video frame.If frame does not contain this value,behavior is same as VOOSMP_RATIO_ORIGINAL*/
    VOOSMP_RATIO_ORIGINAL   = 0X00,  /*!< Use video original size, width: height, same as VOOSMP_RATIO_00*/

    VOOSMP_RATIO_MAX        = 0X7FFFFFF
}VOOSMP_ASPECT_RATIO;
   
   
/**
 * The status of the engine 
 */
typedef enum
{
    VOOSMP_STATUS_INIT              = 0,            /*!< Status is init */
    VOOSMP_STATUS_LOADING           = 1,            /*!< Status is loading */
    VOOSMP_STATUS_RUNNING           = 2,            /*!< Status is running */
    VOOSMP_STATUS_PAUSED            = 3,            /*!< Status is paused */
    VOOSMP_STATUS_STOPPED           = 4,            /*!< Status is stopped */
    VOOSMP_STATUS_MAX               = 0X7FFFFFFF
}VOOSMP_STATUS;
   
   
/**
 * Definition of zoom modes
 */
typedef enum
{
    VOOSMP_ZOOM_LETTERBOX     = 0X01,  /*!< Zoom mode is letter box */
    VOOSMP_ZOOM_PANSCAN       = 0X02,  /*!< Zoom mode is pan scan */
    VOOSMP_ZOOM_FITWINDOW     = 0X03,  /*!< Zoom mode is fit to window */
    VOOSMP_ZOOM_ORIGINAL      = 0X04,  /*!< Zoom mode is original size */
    VOOSMP_ZOOM_ZOOMIN        = 0x05,  /*!< Zoom mode is zoom in with the window */
    VOOSMP_ZOOM_MAX           = 0X7FFFFFFF
}VOOSMP_ZOOM_MODE;
   
   
/*!
 * Definition of event callback
 */
typedef struct
{
    VOOSMPListener  pListener;
    void*           pUserData;
}VOOSMP_LISTENERINFO;
   
   
/**
 * Enumeration of render types
 */
typedef enum
{
    VOOSMP_RENDER_TYPE_JAVA,                    /*!< Using Java render,for Android platform only */
    VOOSMP_RENDER_TYPE_NATIVE,                  /*!< Using native window render,for Android platform only */
    VOOSMP_RENDER_TYPE_BITMAP,                  /*!< Using bitmap render,for Android platform only */
    VOOSMP_RENDER_TYPE_OPENGLES,                /*!< Using OpenGL ES,for Android platform only */
    VOOSMP_RENDER_TYPE_NATIVE_C,                /*!< Using native C render,for Android platform only */
    VOOSMP_RENDER_TYPE_HW_RENDER,               /*!< Using native HW render,for Android platform only */
	VOOSMP_RENDER_TYPE_JMHW_RENDER,             /*!< Using Java MediaCodec HW render,for Android platform only */
    VOOSMP_RENDER_TYPE_DDRAW,                   /*!< Using DirectDraw render,for Windows platform only */
    VOOSMP_RENDER_TYPE_GDI,                     /*!< Using GDI render,for Windows platform only */
    VOOSMP_RENDER_TYPE_DC,                      /*!< Using DC(Device Context) render,for Windows platform only */
    VOOSMP_RENDER_TYPE_MAX = 0X7FFFFFFF         /*!< Max value definition */
}VOOSMP_RENDER_TYPE;
   
   
/**
 * Performance data
 */
typedef struct
{
    int     nCodecType;     /*!< Codec you want to query performance data */
    int     nBitRate;       /*!< Bitrate */
    int     nVideoWidth;    /*!< Video width */
    int     nVideoHeight;   /*!< Video height */
    int     nProfileLevel;  /*!< Level of the profile, 0:baseline, 1:main profile, 2:high profile */
    int     nFPS;           /*!< Frame rate */
}VOOSMP_PERF_DATA;
   
   
/**
 * CPU info
 */
typedef struct
{
    int         nCoreCount;     /*!< CPU count */
    int         nCPUType;       /*!< CPU type, 0:no neon support, 1:neon support */
    int         nFrequency;     /*!< CPU frequency */
    long long   llReserved;     /*!< Reserved value */
}VOOSMP_CPU_INFO;  
   
   
/**
 * The read buffer function
 */
typedef struct
{
    void*   pUserData;                                                              /*!< User data */
    int     (*ReadData)(void* pUserData, int nPos, void* pBuffer, int nSize);       /*!< Address read io buffer API */
}VOOSMP_READDATA_FUNC;
   
   
/**
 * Enumeration for init parameter flags, used in Init() API
 */
typedef enum
{
    VOOSMP_FLAG_INIT_NOUSE              = 0X00000000,   /*!< Init PARAM is NULL */
    VOOSMP_FLAG_INIT_LIBOP              = 0X00000001,   /*!< Init PARAM is load library function address */
    VOOSMP_FLAG_INIT_IO_FILE_NAME       = 0X00000002,   /*!< Init PARAM is IO module file name */
    VOOSMP_FLAG_INIT_IO_API_NAME        = 0X00000004,   /*!< Init PARAM is IO function name */
    VOOSMP_FLAG_INIT_ACTUAL_FILE_SIZE   = 0X00000008,   /*!< Init PARAM is actual file size */
    VOOSMP_FLAG_INIT_WORKING_PATH       = 0X00000010,   /*!< Init PARAM is full path of libraries */
    VOOSMP_FLAG_INIT_MAX                = 0X7FFFFFFF    /*!< Max value definition */
}VOOSMP_INIT_PARAM_FLAG;
   
   
/**
 * Enumeration for init parameters, used in Init() API
 */
typedef struct
{
    void*       pLibOP;             /*!< Init PARAM is load library operation API when VOOSMP_FLAG_INIT_LIBOP is available */
    char*       pszIOFileName;      /*!< Init PARAM is IO module file name when VOOSMP_FLAG_INIT_IO_FILE_NAME is available */
    char*       pszIOApiName;       /*!< Init PARAM is IO API name when VOOSMP_FLAG_INIT_IO_API_NAME is available */
    unsigned long long llFileSize;  /*!< Init PARAM is actual file size when VOOSMP_FLAG_INIT_IO_API_NAME is available */
    void*       pWorkingPath;       /*!< Init PARAM is full path of libraries when VOOSMP_FLAG_INIT_WORKING_PATH is available */
}VOOSMP_INIT_PARAM;    
   
   
/**
 * Enumeration of available tracks, used in VOOSMP_CB_MediaTypeChanged callback
 */
typedef enum
{
    VOOSMP_AVAILABLE_PUREAUDIO,             /*!< Only audio is available */
    VOOSMP_AVAILABLE_PUREVIDEO,             /*!< Only video is available */
    VOOSMP_AVAILABLE_AUDIOVIDEO,            /*!< Both audio and video are available */
    VOOSMP_AVAILABLE_SUBTITLE,              /*!< Subtitle is available */
    VOOSMP_AVAILABLE_MAX    = 0X7FFFFFFF    /*!< Max value definition */
}VOOSMP_AVAILABLE_TRACK_TYPE;
   
   
/**
 * Enumeration of select media framework types, used in Init() API
 */
typedef enum
{
    VOOSMP_VOME2_PLAYER,                /*!< VOME2 media framework is being used */
    VOOSMP_OMXAL_PLAYER,                /*!< OpenMAX AL media framework is being used (Android only) */
    VOOSMP_AV_PLAYER,                   /*!< AVPlayer is being used (iOS only) */
    VOOSMP_PLAYER_TYPE_MAX = 0X7FFFFFFF /*!< Max value definition */
}VOOSMP_PLAYER_TYPE;
   
   
/**
 * Structure for performance info
 */  
typedef struct
{
    int nLastTime;                  /*!< Time to look back in */
    int nSourceDropNum;             /*!< Source dropped frame number */
    int nCodecDropNum;              /*!< Codec dropped frame number */
    int nRenderDropNum;             /*!< Render dropped frame number */
    int nDecodedNum;                /*!< Decoded frame number */
    int nRenderNum;                 /*!< Rendered frame number */
    int nSourceTimeNum;             /*!< Number source exceeds the time (I / ms) */
    int nCodecTimeNum;              /*!< Number codec exceeds the time  (I /ms) */
    int nRenderTimeNum;             /*!< Number render exceeds the time (I /ms) */
    int nJitterNum;                 /*!< Number jitter exceeds the time (I /ms) */
    int nCodecErrorsNum;            /*!< Number codec dropped with encounter errors */
    int *nCodecErrors;              /*!< Codec errors */
    int nCPULoad;                   /*!< Current CPU load in percent */
    int nFrequency;                 /*!< Current frequency CPU is scaled to */
    int nMaxFrequency;              /*!< Maximum frequency CPU */
    int nWorstDecodeTime;           /*!< Worst codec decode time (ms) */
    int nWorstRenderTime;           /*!< Worst render time (ms) */
    int nAverageDecodeTime;         /*!< Average codec decode time (ms) */
    int nAverageRenderTime;         /*!< Average render time (ms) */
    int nTotalCPULoad;              /*!< Current total CPU load in percent */
    int nTotalPlaybackDuration;     /*!< The playback duration from the beginning of playback to the time analytics is queried */
    int nTotalSourceDropNum;        /*!< Source dropped frame number from the beginning of playback to the time analytics is queried */
    int nTotalCodecDropNum;         /*!< Codec dropped frame number from the beginning of playback to the time analytics is queried */
    int nTotalRenderDropNum;        /*!< Render dropped frame number from the beginning of playback to the time analytics is queried */
    int nTotalDecodedNum;           /*!< Decoded frame number from the beginning of playback to the time analytics is queried */
    int nTotalRenderNum;            /*!< Rendered frame number from the beginning of playback to the time analytics is queried */
}VOOSMP_PERFORMANCE_DATA;
   
   
   
/**
 * Enumeration of subtitle language types
 */
typedef enum
{
    VOOSMP_LANG_TYPE_UNKNOWN,                   /*!< Unknown subtitles */
    VOOSMP_LANG_TYPE_SIMPLE_CHINESE,            /*!< Simple Chinese subtitles */
    VOOSMP_LANG_TYPE_TRADITIONAL_CHINESE,       /*!< Traditional Chinese subtitles */
    VOOSMP_LANG_TYPE_ENGLISH,                   /*!< English subtitles */
    VOOSMP_LANG_TYPE_FRENCH,                    /*!< French subtitles */
    VOOSMP_LANG_TYPE_KOREAN,                    /*!< Korean subtitles */
    VOOSMP_LANG_TYPE_JAPANESE,                  /*!< Japanese subtitles */
    VOOSMP_LANG_TYPE_GERMANY,                   /*!< Germany subtitles */
    VOOSMP_LANG_TYPE_ITALIAN,                   /*!< Italian subtitles */
    VOOSMP_LANG_TYPE_SPANISH,                   /*!< Spanish subtitles */

    VOOSMP_LANG_TYPE_UNKNOWN_MAX = 0X7FFFFFFF   /*!< Max value definition */
}VOOSMP_SUBTITLE_LANGUAGE_TYPE;
   
   
   
/**
 * Structure for subtitle language
 */  
typedef struct
{
    unsigned char   szLangName[128];    /*!< Subtitle language name */
    int             nLanguageType;      /*!< Subtitle language type, refer to VOOSMP_SUBTITLE_LANGUAGE_TYPE */
    int             nFlag;              /*!< Not currently used */
    int             nReserved1;         /*!< Reserved */
    int             nReserved2;         /*!< Reserved */
}VOOSMP_SUBTITLE_LANGUAGE;
   
   
/**
 * Structure for subtitle language info
 */  
typedef struct
{
    int                         nLanguageCount;     /*!< Subtitle language total count */
    VOOSMP_SUBTITLE_LANGUAGE**  ppLanguage;         /*!< Subtitle language info list */
}VOOSMP_SUBTITLE_LANGUAGE_INFO;  
   
   
   
/**
 * Enumeration of character edges
 */
typedef enum
{
    VOOSMP_EDGE_NO_EDGE,                /*!< No edges */
    VOOSMP_EDGE_RAISED,                 /*!< Raised edges */
    VOOSMP_EDGE_DEPRESSED,              /*!< Depressed edges */
    VOOSMP_EDGE_UNIFORM,                /*!< Uniform edges */
    VOOSMP_EDGE_DROP_SHADOWED,          /*!< Drop shadowed edges */
    VOOSMP_EDGE_TYPE_MAX = 0X7FFFFFFF   /*!< Max value definition */
}VOOSMP_SUBTITLE_CHARACTER_EDGE_ATTRIBUTES;

   
/**
 * Enumeration of masks for setting items
 */
typedef enum
{
    VOOSMP_SUBTITLE_MASK_DISABLE_ALL                = 0x00000000,   /*!< All items are disabled */
    VOOSMP_SUBTITLE_MASK_FONT_NAME                  = 0x00000001,   /*!< Font name item is enabled */
    VOOSMP_SUBTITLE_MASK_FONT_COLOR                 = 0x00000002,   /*!< Font color item is enabled */
    VOOSMP_SUBTITLE_MASK_FONT_OPACITY               = 0x00000004,   /*!< Font opacity item is enabled */
    VOOSMP_SUBTITLE_MASK_FONT_SIZE_SCALE            = 0x00000008,   /*!< Font size scale item is enabled */
    VOOSMP_SUBTITLE_MASK_FONT_BOLD                  = 0x00000010,   /*!< Font bold item is enabled */
    VOOSMP_SUBTITLE_MASK_FONT_ITALIC                = 0x00000020,   /*!< Font italic item is enabled */
    VOOSMP_SUBTITLE_MASK_FONT_UNDERLINE             = 0x00000040,   /*!< Font underline item is enabled */
    VOOSMP_SUBTITLE_MASK_FONT_EDGE_TYPE             = 0x00000080,   /*!< Font edge type item is enabled */
    VOOSMP_SUBTITLE_MASK_FONT_EDGE_COLOR            = 0x00000100,   /*!< Font edge color item is enabled */
    VOOSMP_SUBTITLE_MASK_FONT_EDGE_OPACITY          = 0x00000200,   /*!< Font edge opacity item is enabled */
    VOOSMP_SUBTITLE_MASK_FONT_BACKGROUND_COLOR      = 0x00000400,   /*!< Font background color item is enabled */
    VOOSMP_SUBTITLE_MASK_FONT_BACKGROUND_OPACITY    = 0x00000800,   /*!< Font background opacity item is enabled */
    VOOSMP_SUBTITLE_MASK_WINDOW_BACKGROUND_COLOR    = 0x00001000,   /*!< Font window background color item is enabled */
    VOOSMP_SUBTITLE_MASK_WINDOW_BACKGROUND_OPACITY  = 0x00002000,   /*!< Font window background OPACITY item is enabled */
    VOOSMP_SUBTITLE_MASK_MAX                        = 0X7FFFFFFF    /*!< Max value definition */
}VOOSMP_SUBTITLE_ITEMS_MASK;
   

/**
 * Structure for subtitle settings
 */  
typedef struct
{
    int     nItemMask;                  /*!< Mask for setting items, default is 0, refer to VOOSMP_SUBTITLE_ITEMS_MASK */
    char    szFontName[128];            /*!< Font name */
    int     nFontColor;                 /*!< Font color, int, such as 0x00RRGGBB */    
    int     nFontOpacity;               /*!< Font opacity between semi-transparent and opaque, percent range is 50-100, default is 100(opaque) */
    int     nFontSizeScale;             /*!< Font size scale, range is 50-200, default is 100, from 50% of the default character size to 200% */
    bool    bFontBold;                  /*!< Font bold or not */
    bool    bFontItalic;                /*!< Font italic or not */
    bool    bFontUnderline;             /*!< Font underlined or not */
    int     nBackgroundColor;           /*!< Font background color, int, such as 0x00RRGGBB */
    int     nBackgroundOpacity;         /*!< Font background opacity between transparent and opaque, percent range is 0-100, default is 100(opaque) */
    int     nEdgeType;                  /*!< Font edge attributes, refer to VOOSMP_SUBTITLE_CHARACTER_EDGE_ATTRIBUTES */
    int     nEdgeColor;                 /*!< Font edge color, int, such as 0x00RRGGBB */
    int     nEdgeOpacity;               /*!< Font edge opacity between transparent and opaque, percent range is 0-100, default is 100(opaque) */
    int     nWindowBackgroundColor;     /*!< Window backgroud color, int, such as 0x00RRGGBB */
    int     nWindowBackgroundOpacity;   /*!< Window opacity between transpatent and opaque, percent range is 0-100, default is 100(opaque) */
}VOOSMP_SUBTITLE_SETTINGS;
   
   
/**
 * The read close captions or subtitles buffer function
 * \param pUserData user data
 * \param ReadSubtitle callback of read subtitle
 * \param voSubtitleInfo [in/out] subtitle buffer
 */
typedef struct
{
    void*   pUserData;                                                         
    int     (*ReadSubtitle)(void* pUserData, voSubtitleInfo* pSubtitle);
}VOOSMP_READ_SUBTITLE_FUNC;
 
/**
 * Structure of picture timing ISO/IEC 14496-10:2005 (E) Annex D 2.2
 */  
typedef struct
{
    int nClockTimestampFlag;
    int nCtType;
    int nNuitFieldBasedFlag;
    int nCountingType;
    int nFullTimestampFlag;
    int nDiscontinuityFlag;
    int nCntDroppedFlag;
    int nFrames;
    int nSecondsValue;
    int nMinutesValue;
    int nHoursValue;
    int nSecondsFlag;
    int nMinutesFlag;
    int nHoursFlag;
    int nTimeOffset;
}VOOSMP_CLOCK_TIME_STAMP;


/**
 * Structure of picture timing SEI message ISO/IEC 14496-10:2005 (E) Annex D 2.2
 */  
typedef struct
{
    int nCpbDpbDelaysPresentFlag;
    int nCpbRemovalDelay;
    int nDpbOutputDelay;
    int nPictureStructurePresentFlag;
    int nPictureStructure;
    int nNumClockTs;
    VOOSMP_CLOCK_TIME_STAMP sClock[3];
}VOOSMP_SEI_PIC_TIMING;

   
/**
 * Structure of SEI user data
 */
typedef struct
{
    unsigned int     nCount;
    unsigned int     nSize[255];
    void*            pBuffer;
}VOOSMP_SEI_USER_DATA_UNREGISTERED;
   
/**
 * Enumeration of SEI event type flags
 */
typedef enum
{
    VOOSMP_FLAG_SEI_NONE                        = 0X00000000,    /*!< No SEI info */
    VOOSMP_FLAG_SEI_PIC_TIMING                  = 0X00000001,    /*!< Picture info, refer to VOOSMP_SEI_PIC_TIMING */
    VOOSMP_FLAG_SEI_USER_DATA_UNREGISTERED      = 0X00000002,    /*!< User data,refer to VOOSMP_SEI_USER_DATA_UNREGISTERED*/
    VOOSMP_FLAG_SEI_MAX                         = 0X7FFFFFFF     /*!< Max value definition */
}VOOSMP_SEI_INFO_FLAG;
   
   
/**
 * Structure for SEI information
 */
typedef struct
{
    long long   llTime;         /*!< In, the current time */
    int         nFlag;          /*!< In/Out, SEI info flag, refer to VOOSMP_SEI_INFO_FLAG */
    void*       pInfo;          /*!< SEI info, value depends on nFlag */
}VOOSMP_SEI_INFO;


/**
 * Enumeration of image type
 */
typedef enum
{
    VOOSMP_IMAGE_RGBA32 = 0,
    VOOSMP_IMAGE_MAX    = 0x7fffffff
}VOOSMP_IMAGE_TYPE;


/**
 * Definition of image data
 */
typedef struct
{
    VOOSMP_IMAGE_TYPE nType;
    int     nWidth;
    int     nHeight;
    int     nSize;
    void *  pPicData; ///<picture data, will be delete in next call, or when Stop() be called
} VOOSMP_IMAGE_DATA;


/**
 * Enumeration of screen brightness adjust mode
 */
typedef enum
{
    VOOSMP_SCREEN_BRIGHTNESS_MODE_MANUAL    = 0X00000000,    /*!< Adjust manually */
    VOOSMP_SCREEN_BRIGHTNESS_MODE_AUTO      = 0X00000001,    /*!< Adjust automatically*/
    VOOSMP_SCREEN_BRIGHTNESS_MODE_MAX       = 0X7FFFFFFF     /*!< Max value definition */
}VOOSMP_SCREEN_BRIGHTNESS_MODE;
    
    
/**
 * Enumeration of audio decoder types
 */
typedef enum
{
    VOOSMP_AUDIO_DECODER_TYPE_SOFTWARE      = 0,                 /*!< Using VisualOn software decoder */
    VOOSMP_AUDIO_DECODER_TYPE_MEDIACODEC    = 1,                 /*!< Using MediaCodec audio decoder,only available on Android */
    VOOSMP_AUDIO_DECODER_TYPE_MAX           = 0X7FFFFFFF         /*!< Max value definition */
}VOOSMP_AUDIO_DECODER_TYPE;
    

/**
 * Enumeration of module type
 */
typedef enum
{
    VOOSMP_MODULE_TYPE_SDK         = 0X00000000,        /*!< The whole OSMP+ SDK */
    VOOSMP_MODULE_TYPE_DRM_VENDOR_A= 0X00000001,        /*!< The version of DRM vendor A */
    VOOSMP_MODULE_TYPE_MAX         = 0X7FFFFFFF         /*!< Max value definition */
}VOOSMP_MODULE_TYPE;

    
/**
 * Structure of module version info
 */
typedef struct
{
    int     nModuleType;    /*!< [in]Indicator module type,refer to VOOSMP_MODULE_TYPE*/
    char*   pszVersion;     /*!< [out]Output the version information */
}VOOSMP_MODULE_VERSION;
 
/**
 * Structure of video render callback
 */
typedef struct
{
	void*       pDC;			/*! device context, HDC for Windows platform	*/
	void*       pRect;			/*! rectangle, LPRECT for Windows platform	*/
	long long	llVideoTime;	/*! time stamp of video	*/
}VOOSMP_VR_USERCALLBACK_TYPE;
    
    
/**
 * General audio render format info
 */
typedef struct
{
    int     SampleRate;  /*!< Sample rate */
    int     Channels;    /*!< Channel count */
    int     SampleBits;  /*!< Bits per sample */
    int     BufferSize;  /*!< Audio render buffer size */
}VOOSMP_AUDIO_RENDER_FORMAT;

    
/**
 * Struct for previewing subtitle
 */
typedef struct
{
    char*   pszSampleText;  /*!< Text for previewing current subtitle settings */
    void*   pView;          /*!< View to which the sample text is being rendered */
}VOOSMP_PREVIEW_SUBTITLE_INFO;
    
       
   
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif // __voOnStreamType_h__
  
