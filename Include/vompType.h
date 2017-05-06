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
#ifndef __vompType_h__
#define __vompType_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define	VOMP_PLAYER_MEDIA		0
#define VOMP_PLAYER_EDITOR		1
#define VOMP_PLAYER_LIVE		2

#define	VOMP_PUSH_MODE		0		/*!<Push Mode.The AV play thread will create in engine */
#define	VOMP_PULL_MODE		1		/*!<Pull Mode.The client call GetParam to get AV data. */

/**
 * Definition of return error code
 */
#define	VOMP_ERR_None					0X00000000	/*!<No Error */
#define	VOMP_ERR_EOS					0X00000001	/*!<Reach the end of source */
#define	VOMP_ERR_Retry					0X00000003	/*!<The buffer was empty, try it again */
#define	VOMP_ERR_FormatChange			0X00000004	/*!<Format was changed */
#define	VOMP_ERR_File					0X80000005	/*!<File read or write error. */
#define	VOMP_ERR_Video					0X80000006	/*!<Error from video codec */
#define	VOMP_ERR_Audio					0X80000007	/*!<Error from audio codec */
#define	VOMP_ERR_OutMemory				0X80000008	/*!<Error is out of memory */
#define	VOMP_ERR_Pointer				0X80000009	/*!<The pointer is invoid */
#define	VOMP_ERR_ParamID				0X8000000A	/*!<The param id was not supported */
#define	VOMP_ERR_Status					0X8000000B	/*!<The status is wrong */
#define	VOMP_ERR_Implement				0X8000000C	/*!<This function was not impement */
#define	VOMP_ERR_SmallSize				0X8000000D	/*!<The input buffer was too small */
#define	VOMP_ERR_OutOfTime				0X8000000E	/*!<It is out of time (late)  */
#define	VOMP_ERR_WaitTime				0X8000000F	/*!<It is waiting reference time (elear)  */
#define	VOMP_ERR_Unknown				0X80000010	/*!<Unknow error*/
#define VOMP_ERR_Audio_No_Now			0X00000011  /*!<Indicate audio is not available now*/
#define VOMP_ERR_Video_No_Now			0X00000012  /*!<Indicate video is not available now*/
#define VOMP_ERR_FLush_Buffer			0X00000013  /*!<Indicate need flush buffers*/



/**
 * Definition call back ID
 */
#define	VOMP_CB_OpenSource				0X01000001	/*!<The source was opened */
#define	VOMP_CB_PlayComplete			0X01000002	/*!<Play source was finished */
#define	VOMP_CB_BufferStatus			0X01000003	/*!<The buffer status. the param 1 is int *.(0 - 100)  */
#define	VOMP_CB_RenderVideo				0X01000004	/*!<Render video. the param 1 is VO_BUFFERTYPE *.  */
#define	VOMP_CB_RenderAudio				0X01000005	/*!<Render audio. the param 1 is VO_BUFFERTYPE *.  */
#define	VOMP_CB_VideoStartBuff			0X01000006	/*!<The video stream start buffering  */
#define	VOMP_CB_VideoStopBuff			0X01000007	/*!<The video stream stop buffering   */
#define	VOMP_CB_AudioStartBuff			0X01000008	/*!<The audio stream start buffering  */
#define	VOMP_CB_AudioStopBuff			0X01000009	/*!<The audio stream stop buffering  */
#define	VOMP_CB_SourceBuffTime			0X0100000A	/*!<The buffer time in source  int * A, int * V  */
#define	VOMP_CB_DownloadPos				0X0100000C	/*!<The download position int *  */
#define	VOMP_CB_PacketLost				0X0100000D	/*!<The Packet lost   */
#define	VOMP_CB_SeekComplete			0X0100000E	/*!<Play source was finished */
#define	VOMP_CB_VideoDelayTime			0X0100000F	/*!<Video delay time int * ms */
#define	VOMP_CB_VideoLastTime			0X01000010	/*!<Video last render time int * ms */
#define	VOMP_CB_VideoDropFrames			0X01000011	/*!<Video dropped frames int *  */
#define	VOMP_CB_VideoFrameRate			0X01000012	/*!<Video playback frame rate last 5 seconds int *  */
#define	VOMP_CB_CloseSource				0X01000013	/*!<The source was closed */
#define	VOMP_CB_Error					0X81100006	/*!<There was unknown in player.  */
#define VOMP_CB_CodecNotSupport			0X81100007	/*!<There was unsupport Codec in player.  */
#define VOMP_CB_NATIVEWINDOW			0X01000014  /*!<The info of native window> */
#define VOMP_CB_VideoAspectRatio		0X01000015	/*!<Video aspect ratio changed*/
#define VOMP_CB_DescribeFail			0X01000016  /*!<send RTSP describel failed. */
#define VOMP_CB_Deblock					0X01000017  /*!<send deblock event. */
#define VOMP_CB_HWStatus				0X01000018  /*!<send hw status event, error 0, ok 1 */
#define VOMP_CB_MediaChanged			0X01000019  /*!<if video/audio is available,int *,refer to VOMP_AVAILABLE_TRACK_TYPE */
#define VOMP_CB_FirstFrameRender		0X0100001A  /*!<if the first video frame was rendered. */
#define VOMP_CB_Pcm_Output				0X0100001B  /*!<This event will be issued only when PCM data output is enabled.*/
#define VOMP_CB_VideoReadyToRender		0X0100001C  /*!< Notify application that video is ready to render */
#define VOMP_CB_LicenseFailed			0X0100001D	/*!< Notify license check failed */


/**
 * Definition parameter ID
 */
#define	VOMP_PID_PLAYER_PATH			0X00000001	/*!<Set the audio volume, the volume is 0 - 100, 0 is mute and 100 is max*/
#define	VOMP_PID_AUDIO_VOLUME			0X00000002	/*!<Set the audio volume, the volume is 0 - 100, 0 is mute and 100 is max*/
#define	VOMP_PID_DRAW_VIEW				0X00000003	/*!<Set the draw view handle  void * */
#define	VOMP_PID_DRAW_RECT				0X00000004	/*!<Set the draw area  VOMP_RECT * */
#define	VOMP_PID_DRAW_COLOR				0X00000005	/*!<Set the draw color type  VOMP_COLORTYPE * */
#define	VOMP_PID_AUDIO_FORMAT			0X00000006	/*!<Get audio format  VOMP_AUDIO_FORMAT * */
#define	VOMP_PID_AUDIO_SAMPLE			0X00000007	/*!<Get audio sample buffer. VOMP_BUFFERTYPE ** It can null */
#define	VOMP_PID_VIDEO_FORMAT			0X00000008	/*!<Get video format  VOMP_VIDEO_FORMAT * */
#define	VOMP_PID_VIDEO_SAMPLE			0X00000009	/*!<Get video sample buffer. VOMP_BUFFERTYPE ** It can null */
#define	VOMP_PID_VIDEO_TRACK			0X0000000A	/*!<Set the video track codec  VOMP_VIDEO_CODINGTYPE * */
#define	VOMP_PID_AUDIO_TRACK			0X0000000B	/*!<Set the audio track codec. VOMP_AUDIO_CODINGTYPE * */
#define	VOMP_PID_FUNC_LIB				0X0000000C	/*!<Set Lib func set.  VOMP_LIB_FUNC * It will support next build. */
#define	VOMP_PID_FUNC_FILE				0X0000000D	/*!<Set File func set.  void * It will support next build. */
#define	VOMP_PID_FUNC_DRM				0X0000000E	/*!<Set DRM func set.  void * It will support next build. */
#define	VOMP_PID_AUDIO_PLAYMODE			0X00000010	/*!<Set the audio stream playback mode.  int * */
#define	VOMP_PID_VIDEO_PLAYMODE			0X00000011	/*!<Set the video stream playback mode.  int * */
#define	VOMP_PID_AUDIO_ONLY				0X00000012	/*!<Set create audio track only.  int * (0, 1) */
#define	VOMP_PID_VIDEO_ONLY				0X00000013	/*!<Set create video track only.  int * (0, 1) */
#define	VOMP_PID_MAX_BUFFTIME			0X00000014	/*!<Set data source max buffer time.  int *  */
#define	VOMP_PID_MIN_BUFFTIME			0X00000015	/*!<Set data source min buffer time.  int *  */
#define	VOMP_PID_SEEK_MODE				0X00000016	/*!<Set seek mode. 0 key frame pos. 1 any pos,  int *  */
#define	VOMP_PID_AUDIO_BUFFTIME			0X00000017	/*!<Get audio buffer time in source.  int *  */ 
#define	VOMP_PID_VIDEO_BUFFTIME			0X00000018	/*!<Get video buffer time in source   int *  */
#define	VOMP_PID_THREAD_CREATE			0X00000019	/*!<Set thread create function.  VOMPThreadCreate *  */
#define	VOMP_PID_SOURCE_HEADER			0X0000001A	/*!<Set header of source parameter void * */
#define	VOMP_PID_DISABLE_DROPFRAME		0X00000020	/*!<Set disable drop video frame int * 1 disable */
#define	VOMP_PID_DISABLE_DROPRENDER		0X00000021	/*!<Set disable drop video render int * 1 disable */
#define	VOMP_PID_AUDIO_CODEC			0X00000022	/*!<Get the audio codec.  int * */
#define	VOMP_PID_VIDEO_CODEC			0X00000023	/*!<Get the video codec.  int * */
#define	VOMP_PID_GET_THUMBNAIL			0X00000024	/*!<Set graph to get the thumbnail int * 1 for thumbnail */
#define	VOMP_PID_START_POS				0X00000025	/*!<Set the start position  int *  */
#define	VOMP_PID_STOP_POS				0X00000026	/*!<Set the stop position  int *  */
#define	VOMP_PID_VIDEO_RNDNUM			0X00000027	/*!<Set/Get the video render frames number  int * (1 - 32)  */
#define	VOMP_PID_VIDEO_RNDCOPY			0X00000028	/*!<Set/Get the video render frames number  int * (1, 0)  */
#define	VOMP_PID_VIDEO_REDRAW			0X00000029	/*!<Set Redraw the video in render   */
#define	VOMP_PID_AUDIO_SINK_BUFFTIME	0X00000030	/*!<Set audio sink buffer time int * ms */
#define	VOMP_PID_AUDIO_STEP_BUFFTIME	0X00000031	/*!<Set audio step buffer time int * ms */
#define	VOMP_PID_PLAY_CONFIG_FILE		0X00000032	/*!<Set vomp play config file VO_TCHAR * */
#define	VOMP_PID_VOSDK_CONFIG_FILE		0X00000033	/*!<Set vomp vosdk config file VO_TCHAR * */
#define	VOMP_PID_VIDEO_DISABLE_DEBLOCK	0X00000034	/*!<Set Video deblock or not (1 disable deblock is performance is not good. 0 Force enable deblock). int * */
#define	VOMP_PID_AUDIO_EFFECT_ENABLE	0X00000035	/*!<Set audio effect enable int * > 0 enable, 0 disable */
#define	VOMP_PID_AUDIO_EFFECT_MODULE	0X00000036	/*!<Set audio effect load or not */
#define VOMP_PID_APPLICATION_SUSPEND	0X0000003A  /*!<Set application suspend, such as go background, 0:pause playback, 1:only audio continue playback,video stopped */
#define VOMP_PID_APPLICATION_RESUME		0X0000003B  /*!<Set application resume, such as go foreground, new view handle will need to be set, void* */
#define VOMP_PID_AUDIO_PCM_OUTPUT		0X0000003C  /*!<Set post audio pcm data to application*/
#define	VOMP_PID_LICENSE_TEXT			0X00000040	/*!<Set check license text. char *   */
#define	VOMP_PID_LICENSE_DATA			0X00000041	/*!<Set check license data. char *   */
#define	VOMP_PID_VIDEO_RENDER_CONTEXT	0X00000045	/*!<Get video render EAGLContext*/
#define	VOMP_PID_VIDEO_EFFECT_TYPE		0X00000046	/*!<set video effect type */
#define	VOMP_PID_VIDEO_EFFECT_ONOFF		0X00000047	/*!<set video effect disable or enable*/
#define	VOMP_PID_VIDEO_VIDEO_UPSIDE		0X00000048	/*!<get video upside down*/
#define	VOMP_PID_VIDEO_PLAY_BACK		0X00000049	/*!<set video play back*/
#define	VOMP_PID_IDLE_THREAD			0X00000050	/*!<Set Create the IDLE thread to keep CPU speed.. int * (-100 100)   */
#define	VOMP_PID_CPU_NUMBER				0X00000051	/*!<Set the cpu number. . int * (1 100)   */
#define VOMP_PID_ZOOM_MODE				0X00000052  /*< Set the draw area mode 1. letter box, 2. pan scan, 3. VO_ZM_FITWINDOW, 4. original size*/
#define VOMP_PID_AUDIO_GETONLY			0X00000053  /*< Set only get audio data or not, for video edit. default is false.*/
#define VOMP_PID_AUDIO_REND_ADJUTIME	0X00000054  /*< Set audio render Adjust Time time int * ms */
#define VOMP_PID_VIDEO_DIMENSION		0X00000055  /* TO get the decoded width & height from decoder */
#define VOMP_PID_DEVICE_RESOLUTION		0X00000056  /*< Set device resolution for the video panscan level */
#define	VOMP_PID_LICENSE_FILE_PATH		0X00000057	/*!<Set license file path. char *   */
#define	VOMP_PID_SOCKET_CONNECTION_TYPE 0X00000058  /*!<Set RTSP default connection type,0:automatically try UDP first then TCP,1:force to use TCP,-1:disable TCP, default 0*/
#define VOMP_PID_AUDIO_STREAM_ONOFF		0X00000059  /*!<Set enable/disable playback audio frame, int *, 0:disable,1:enable */
#define VOMP_PID_VIDEO_STREAM_ONOFF		0X0000005A  /*!<Set enable/disable playback video frame, int *, 0:disable,1:enable */
#define VOMP_PID_AUDIO_DEC_BITRATE		0X0000005B  /*!<Get bitrate of audio codec, int * */
#define VOMP_PID_VIDEO_DEC_BITRATE		0X0000005C  /*!<Get bitrate of video codec, int * */
#define	VOMP_PID_CPU_INFO				0X0000005D	/*!<Get the cpu information. . VOMP_CPU_INFO *  */
#define	VOMP_PID_COMMON_STRING			0X0000005f	/*!<set the common string *  */
#define	VOMP_PID_LOW_LATENCE			0X00000060	/*!<set the player low latence *  */
#define	VOMP_PID_VIDEO_PERFORMANCE_CACHE	0X00000061   /*!<set the performance data buffer , in/out int *  */
#define	VOMP_PID_VIDEO_PERFORMANCE_OPTION	0X00000062   /*!<get the performance data Statistics, in/out  PerformanceData * */
#define	VOMP_PID_VIDEO_PERFORMANCE_ONOFF	0X00000063   /*!<set enable/disable the performance data Statistics, default disable, int 0 disable, 1 enable * */
#define	VOMP_PID_IOMX_PROBE				0X00000064	/*!<probe IOMX hardware decoder *  */
#define	VOMP_PID_VIDEO_RENDER_TYPE			0X00000065	/*!<set video render type, for windows only *  */
#define	VOMP_PID_VIDEO_ONDRAW				0X00000066	/*!< App ondraw call*  */
#define	VOMP_PID_AUDIO_PLAYBACK_SPEED		0X00000067	/*!< set audio playback speed*  */
#define	VOMP_PID_DRM_UNIQUE_IDENTIFIER		0X00000068	/*!< set vomp2 UUID*  */
#define VOMP_PID_AUDIO_RENDER_DATA_FORMAT   0x00000069  /*!< Get audio render data format after decoding, int* added for amazon project for passthrough ac3/eac3*/
#define VOMP_PID_AUDIO_DSP_CLOCK            0x0000006A  /*!< set the interface IAudioDSPClock interface */
#define VOMP_PID_PAUSE_REFCOLOK				0x00000070	/*!< pause reference clock or not. VO_BOOL * */
#define	VOMP_PID_AUDIO_RNDNUM			0X00000071		/*!<Set/Get the audio render buffers' number  VO_U32 * (1 - 16)  */

#define	VOMP_PID_VIDEO_DRM_FUNC			0X00000080	/*!< Set the DRM function pointer into video codec Param void * depend the DRM type. */

#define VOMP_PID_STREAMDIGEST_AUTHEN_N	0X00000100  /*!<Set RTSP default connection type,authen test name*/
#define VOMP_PID_STREAMDIGEST_AUTHEN_P	0X00000101  /*!<Set RTSP default connection type,authen test password*/

#define	VOMP_PID_OUTPUTFILE				0X00000104	/*!<Set the output file name for recorder. char * */
#define	VOMP_PID_REC_START				0X00000105	/*!<Start recorder  It will support next build */
#define	VOMP_PID_REC_PAUSE				0X00000106	/*!<Pause recorder  It will support next build */
#define	VOMP_PID_REC_STOP				0X00000107	/*!<Stop recorder  It will support next build */

#define VOMP_PID_JAVA_VM				0X00000500	/*!<Used by Java MediaCodec  set it from jni level it refers to JavaVM * */

#define VOMP_PID_SIGNAL_BUFFER_RETURNED	0X00001000	/*!<Set signal buffer returned, VOMP_SIGNALBUFFERRETURNED * */

#define VOMP_PID_AUDIO_CODEC_FILENAME	0X00110000  /*!<Set audio decoder library name*/
#define VOMP_PID_AUDIO_CODEC_APINAME	0X00220000  /*!<Set audio decoder API name*/
#define VOMP_PID_VIDEO_CODEC_FILENAME	0X00440000  /*!<Set video decoder library name*/
#define VOMP_PID_VIDEO_CODEC_APINAME	0X00880000  /*!<Set video decoder API name*/
#define VOMP_PID_LOAD_DOLBY_DECODER_MODULE 0X00880001 /*!<Enable/Disable loading Dolby libraries*/



#define VOMP_FLAG_SOURCE_SYNC			0X00000001	/*!<Indicate load source sync mode  */
#define VOMP_FLAG_SOURCE_ASYNC			0X00000002	/*!<Indicate load source async mode  */
#define VOMP_FLAG_SOURCE_URL			0X00000010	/*!<Indicate the source is URL */
#define VOMP_FLAG_SOURCE_DATASOURCE		0X00000020	/*!<Indicate the source is out data source */
#define VOMP_FLAG_SOURCE_SENDBUFFER		0X00000040	/*!<Indicate the source is out of buffer. SendBuffer */
#define VOMP_FLAG_SOURCE_READBUFFER		0X00000080	/*!<Indicate the source is out of buffer. EaadBuffer */
#define VOMP_FLAG_SOURCE_METADATA		0X00000100	/*!<Indicate the source only open file sourcer */

#define VOMP_FLAG_SOURCE_SENDBUFFER_TS	0X00010040	/*!<Indicate the source is out of buffer. SendBuffer, the stream format is TS */
#define VOMP_FLAG_SOURCE_SENDBUFFER_MFS	0X00020040	/*!<Indicate the source is out of buffer. SendBuffer, the stream format is MFS */

#define VOMP_FLAG_BUFFER_KEYFRAME			0X00000001	/*!<Indicate the buffer is key frame */
#define VOMP_FLAG_BUFFER_NEW_PROGRAM		0X00000010	/*!<Indicate the buffer start new program */
#define VOMP_FLAG_BUFFER_NEW_FORMAT			0X00000020	/*!<Indicate the buffer start new format */
#define VOMP_FLAG_BUFFER_TIME_RESET			0X00000040	/*!<Indicate the sample timestamp rollback */
#define VOMP_FLAG_BUFFER_FORCE_FLUSH		0X00000100	/*!<Indicate the buffer flush the previous buffers */
#define VOMP_FLAG_BUFFER_EOS				0X00000200	/*!<Indicate the buffer reach EOS */
#define VOMP_FLAG_BUFFER_END				0X00000400	/*!<Indicate the buffer is drop frame before */
#define VOMP_FLAG_BUFFER_HEADDATA			0X00001000	/*!<Indicate the buffer is head data */
#define VOMP_FLAG_BUFFER_DROP_FRAME			0X00002000	/*!<Indicate the buffer is drop frame before */
#define	VOMP_FLAG_BUFFER_TRY_ONLY   		0X00004000	/*!<Indicate only try to get video the buffer */
#define	VOMP_FLAG_BUFFER_DELAY_TO_DROP		0X00008000	/*!<Indicate only try to get video the buffer */
#define	VOMP_FLAG_BUFFER_SAMPLEAFTERSEEK	0X00010000	/*!<Indicate this sample is the the first sample after seek, this flag is used in the case that app seek time is not reachable and the real seek pos can not return immediatly 
															When you are using this flag, the timestamp shows the reachable seek pos after seek	*/
#define VOMP_FLAG_BUFFER_ADD_DRMDATA		0X00020000	/*!<Indicate the buffer has been added the drm information*/
#define	VOMP_FLAG_BUFFER_FRAME_DECODE_ONLY	0X00040000	/*!<Indicate only decoder it not render this frame */
#define VOMP_FLAG_BUFFER_YUVBUFFER			0X00080000	/*!<Indicate the buffer is YUV buffer, pData should be VOMP_VIDEO_BUFFER *	*/

#define VOMP_FLAG_VIDEO_EFFECT_ON		0X10000000	/*!<Indicate the video effect on */
#define VOMP_FLAG_VIDEO_EFFECT_CCP		0X00000001	/*!<Indicate the video effect close caption */

/**
 *Call back function. Update the status from player
 * \param nID [in] the id of special event
 * \param pParam1 [in/out] the first parameter
 * \param pParam2 [in/out] the second parameter
 */
typedef int (* VOMPListener) (void * pUserData, int nID, void * pParam1, void * pParam2);


/**
*the status of the player	
*/
typedef enum{
	VOMP_STATUS_INIT				= 0,			/*!<The status is init */
	VOMP_STATUS_LOADING				= 1,			/*!<The status is loading */
	VOMP_STATUS_RUNNING				= 2,			/*!<The status is running */
	VOMP_STATUS_PAUSED				= 3,			/*!<The status is paused */
	VOMP_STATUS_STOPPED				= 4,			/*!<The status is stopped */
	VOMP_STATUS_MAX					= 0X7FFFFFFF
}VOMP_STATUS;

/**
 * General data buffer, used as input or output.
 */
typedef struct {
	unsigned char *	pBuffer;		/*!< Buffer pointer */
	int				nSize;			/*!< Buffer size in byte */
	long long		llTime;			/*!< The time of the buffer */
	int				nFlag;			/*!< The flag of the buffer */
	void *			pData;			/*!< The special data pointer depend on the flag */
	int				nReserve;		/*!< The reserve value */
} VOMP_BUFFERTYPE;

/**
 * General audio format info
 */
typedef struct
{
	int		SampleRate;  /*!< Sample rate */
	int		Channels;    /*!< Channel count */
	int		SampleBits;  /*!< Bits per sample */
} VOMP_AUDIO_FORMAT;

/**
 * General video format info
 */
typedef struct
{
	int					Width;		 /*!< Width */
	int					Height;		 /*!< Height */
	int					Type;		 /*!< Color type  */
} VOMP_VIDEO_FORMAT;


typedef struct
{
	int							nTrackType;				/*!< video/audio/subtitle */
	int							nCodec;					/*!< codec type, VO_AUDIO_CODINGTYPE | VO_VIDEO_CODINGTYPE */
	int							nFourCC;

	union
	{
		VOMP_AUDIO_FORMAT     sAudioInfo;				/*!< audio info */
		VOMP_VIDEO_FORMAT     sVideoInfo;				/*!< video info */
	};

	int							nHeadSize;				/*!< sequence data size*/
	char*						pHeadData;				/*!< Header data pointor */
}VOMP_TRACK_INFO;

/**
 * General video dimension info
 */
typedef struct
{
	int					nTop;		 /*!< top */
	int					nLeft;		 /*!< left */
	unsigned int		nWidth;		 /*!< width */
	unsigned int		nHeight;	 /*!< height> */
} VOMP_VIDEO_DECDIMENSION;

/**
 * Video data buffer, usually used as input or output of video codec.
 */
typedef struct
{
	char *	 			Buffer[3];			/*!< Buffer pointer */
	int					Stride[3];			/*!< Buffer stride */
	int					ColorType;			/*!< Color Type */
	long long			Time;				/*!< The time of the buffer */
} VOMP_VIDEO_BUFFER;

/**
 * Definition of rect structure
 */
typedef struct
{
	int					left;		/*!< Left  */
	int					top;		/*!< top  */
	int					right;		/*!< right  */
	int					bottom;		/*!< bottom  */
} VOMP_RECT;


/**
 * CPU info
 */
typedef struct
{
	int			nCoreCount;		/*!<The codec you want to query performance data */
	int			nCPUType;		/*!<The CPU type,0:neon not supoort, 1:support neon */
	int			nFrequency;		/*!<The frequency of CPU */
	int			lReserved;		/*!<The reserved value */
}VOMP_CPU_INFO;	

/**
*the data source operator
*/
typedef struct
{
	void *	pUserData;
	int		(* ReadAt) (void * pUserData, long long llPos, unsigned char * pBuffer, int nSize);
	int		(* GetSize) (void * pUserData, long long  * pPos);
	int		(* WriteAt) (void * pUserData, long long llPos, unsigned char * pBuffer, int nSize);
} VOMP_DATASOURCE_FUNC;


/**
*the lib operator
*/
typedef struct
{
	void *	pUserData;
	void *	(* LoadLib) (void * pUserData, char * pLibName, int nFlag);
	void *	(* GetAddress) (void * pUserData, void * hLib, char * pFuncName, int nFlag);
	int		(* FreeLib) (void * pUserData, void * hLib, int nFlag);
} VOMP_LIB_FUNC;

/**
*the read buffer func
*/
typedef struct
{
	void *	pUserData;
	int		(* ReadAudio) (void * pUserData, VOMP_BUFFERTYPE * pBuffer);
	int		(* ReadVideo) (void * pUserData, VOMP_BUFFERTYPE * pBuffer);
} VOMP_READBUFFER_FUNC;

typedef struct
{
	void *	pUserData;
	int		(* SendData) (void * pUserData, int nSSType, VOMP_BUFFERTYPE * pBuffer);
} VOMP_SENDBUFFER_FUNC;

/**
 *Enumeration used to define the possible source stream type.
 */
typedef enum VOMP_VIDEO_EFFECTTYPE {
	VOMP_VE_Null		  = 0,
	VOMP_VE_CloseCaptionOn,				/**< the closeCaptionOn  */
	VOMP_VE_MAX		= 0X7FFFFFFF
} VOMP_VIDEO_EFFECTTYPE;

/**
 *Enumeration used to define the possible source stream type.
 */
typedef enum VOMP_SOURCE_STREAMTYPE {
	VOMP_SS_Audio = 0,				/**< the source stream is Audio  */
	VOMP_SS_Video,					/**< the source stream is Video  */
	VOMP_SS_TS,						/**< the source stream is TS  */
	VOMP_SS_MFS,					/**< the source stream is MFS  */
	VOMP_SS_MAX		= 0X7FFFFFFF
} VOMP_SOURCE_STREAMTYPE;

/**
 *Enumeration used to define the possible audio codings.
 */
typedef enum VOMP_AUDIO_CODINGTYPE {
	VOMP_AUDIO_CodingUnused = 0,  /**< Placeholder value when coding is N/A  */
	VOMP_AUDIO_CodingPCM,         /**< Any variant of PCM coding */
	VOMP_AUDIO_CodingADPCM,       /**< Any variant of ADPCM encoded data */
	VOMP_AUDIO_CodingAMRNB,       /**< Any variant of AMR encoded data */
	VOMP_AUDIO_CodingAMRWB,       /**< Any variant of AMR encoded data */
	VOMP_AUDIO_CodingAMRWBP,      /**< Any variant of AMR encoded data */
	VOMP_AUDIO_CodingQCELP13,     /**< Any variant of QCELP 13kbps encoded data */
	VOMP_AUDIO_CodingEVRC,        /**< Any variant of EVRC encoded data */
	VOMP_AUDIO_CodingAAC,         /**< Any variant of AAC encoded data, 0xA106 - ISO/MPEG-4 AAC, 0xFF - AAC */
	VOMP_AUDIO_CodingAC3,         /**< Any variant of AC3 encoded data */
	VOMP_AUDIO_CodingFLAC,        /**< Any variant of FLAC encoded data */
	VOMP_AUDIO_CodingMP1,		  /**< Any variant of MP1 encoded data */
	VOMP_AUDIO_CodingMP3,         /**< Any variant of MP3 encoded data */
	VOMP_AUDIO_CodingOGG,         /**< Any variant of OGG encoded data */
	VOMP_AUDIO_CodingWMA,         /**< Any variant of WMA encoded data */
	VOMP_AUDIO_CodingRA,          /**< Any variant of RA encoded data */
	VOMP_AUDIO_CodingMIDI,        /**< Any variant of MIDI encoded data */
	VOMP_AUDIO_CodingDRA,         /**< Any variant of dra encoded data */
	VOMP_AUDIO_CodingG729,        /**< Any variant of dra encoded data */
	VOMP_AUDIO_CodingEAC3,		  /**< Any variant of Enhanced AC3 encoded data */
	VOMP_AUDIO_CodingAPE,		  /**< Any variant of APE encoded data */
	VOMP_AUDIO_CodingALAC,		  /**< Any variant of ALAC encoded data */
	VOMP_AUDIO_CodingDTS,		  /**< Any variant of DTS encoded data */
	VOMP_AUDIO_Coding_MAX		= 0X7FFFFFFF
} VOMP_AUDIO_CODINGTYPE;

/**
 * Video Coding type
 */
typedef enum VOMP_VIDEO_CODINGTYPE {
	VOMP_VIDEO_CodingUnused,     /*!< Value when coding is N/A */
	VOMP_VIDEO_CodingMPEG2,      /*!< AKA: H.262 */
	VOMP_VIDEO_CodingH263,       /*!< H.263 */
	VOMP_VIDEO_CodingS263,       /*!< H.263 */
	VOMP_VIDEO_CodingMPEG4,      /*!< MPEG-4 */
	VOMP_VIDEO_CodingH264,       /*!< H.264/AVC */
	VOMP_VIDEO_CodingWMV,        /*!< all versions of Windows Media Video */
	VOMP_VIDEO_CodingRV,         /*!< all versions of Real Video */
	VOMP_VIDEO_CodingMJPEG,      /*!< Motion JPEG */
	VOMP_VIDEO_CodingDIVX,	     /*!< DIV3 */
	VOMP_VIDEO_CodingVP6,		  /*!< VP6 */
	VOMP_VIDEO_CodingVP8,		   /*!< VP8 */
	VOMP_VIDEO_CodingVP7,        /*!< VP7 */
	VOMP_VIDEO_CodingVC1,         /*VC1:WMV3,WMVA,WVC1*/
	VOMP_VIDEO_CodingH265,         /*H265*/
	VOMP_VIDEO_Coding_Max		= 0X7FFFFFFF
} VOMP_VIDEO_CODINGTYPE;

/**
 * Definition of color format
 */
typedef enum
{
	VOMP_COLOR_YUV_PLANAR444			= 0,		/*!< YUV planar mode:444  vertical sample is 1, horizontal is 1  */
	VOMP_COLOR_YUV_PLANAR422_12			= 1,		/*!< YUV planar mode:422, vertical sample is 1, horizontal is 2  */
	VOMP_COLOR_YUV_PLANAR422_21			= 2,		/*!< YUV planar mode:422  vertical sample is 2, horizontal is 1  */
	VOMP_COLOR_YUV_PLANAR420			= 3,		/*!< YUV planar mode:420  vertical sample is 2, horizontal is 2  */
	VOMP_COLOR_YUV_PLANAR411			= 4,		/*!< YUV planar mode:411  vertical sample is 1, horizontal is 4  */
	VOMP_COLOR_YUV_PLANAR411V			= 44,		/*!< YUV planar mode:411  vertical sample is 4, horizontal is 1  */
	VOMP_COLOR_GRAY_PLANARGRAY			= 5,		/*!< gray planar mode, just Y value								 */
	VOMP_COLOR_YUYV422_PACKED			= 6,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: Y0 U0 Y1 V0  */
	VOMP_COLOR_YVYU422_PACKED			= 7,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: Y0 V0 Y1 U0  */
	VOMP_COLOR_UYVY422_PACKED			= 8,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: U0 Y0 V0 Y1  */
	VOMP_COLOR_VYUY422_PACKED			= 9,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: V0 Y0 U0 Y1  */
	VOMP_COLOR_YUV444_PACKED			= 10,		/*!< YUV packed mode:444, vertical sample is 1, horizontal is 1, data: Y U V	*/
	VOMP_COLOR_YUV_420_PACK				= 11, 		/*!< YUV planar mode:420  vertical sample is 2, horizontal is 2  , Y planar, UV Packed*/
	VOMP_COLOR_YUV_420_PACK_2			= 35, 		/*!< YUV planar mode:420  vertical sample is 2, horizontal is 2  , Y planar, VU Packed*/
	VOMP_COLOR_YVU_PLANAR420			= 12,
	VOMP_COLOR_YVU_PLANAR422_12			= 13,
	VOMP_COLOR_YUYV422_PACKED_2			= 14,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: Y1 U0 Y0 V0  */
	VOMP_COLOR_YVYU422_PACKED_2			= 15,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: Y1 V0 Y0 U0  */
	VOMP_COLOR_UYVY422_PACKED_2			= 16,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: U0 Y1 V0 Y0  */
	VOMP_COLOR_VYUY422_PACKED_2			= 17,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: V0 Y1 U0 Y0  */
	VOMP_COLOR_RGB565_PACKED			= 30,		/*!< RGB packed mode, data: B:5 G:6 R:5   						 */
	VOMP_COLOR_RGB555_PACKED			= 31,		/*!< RGB packed mode, data: B:5 G:5 R:5   						 */
	VOMP_COLOR_RGB888_PACKED			= 32,		/*!< RGB packed mode, data: B G R		 						 */
	VOMP_COLOR_RGB32_PACKED				= 33,		/*!< RGB packed mode, data: B G R A								 */
	VOMP_COLOR_RGB888_PLANAR			= 34,		/*!< RGB planar mode											 */
	VOMP_COLOR_YUV_PLANAR420_NV12		= 36,		/*!< YUV planar mode:420  vertical sample is 2, horizontal is 2  */
	VOMP_COLOR_ARGB32_PACKED			= 37,		/*!< ARGB packed mode, data: B G R A							 */
	VOMP_COLOR_TYPE_MAX					= 0X7FFFFFF
}  VOMP_COLORTYPE;

/*!
* Definition of aspect ratio type
*/
typedef enum
{
	VOMP_RATIO_00			= 0x00,	 /*!< Width and height is width: height */
	VOMP_RATIO_11			= 0X01,  /*!< Width and Height is 1 : 1		*/
	VOMP_RATIO_43			= 0X02,  /*!< Width and Height is 4 : 3		*/
	VOMP_RATIO_169			= 0X03,  /*!< Width and Height is 16 : 9	*/
	VOMP_RATIO_21			= 0X04,  /*!< Width and Height is 2 : 1		*/
	VOMP_RATIO_MAX			= 0X7FFFFFF
}VOMP_ASPECT_RATIO;

/**
 * Enumeration for available track, used in VOMP_CB_MediaTypeChanged callback
 */	
typedef enum
{
	VOMP_AVAILABLE_PUREAUDIO,				/*!<Only audio is available */
	VOMP_AVAILABLE_PUREVIDEO,				/*!<Only video is available */
	VOMP_AVAILABLE_AUDIOVIDEO,				/*!<Both audio and video are available */
	VOMP_AVAILABLE_MAX	= 0X7FFFFFFF		/*!<Max value definition */
}VOMP_AVAILABLE_TRACK_TYPE;

/**
 * Structure of performance data.
 */	
typedef struct 
{
	int nLastTime;
	int nSourceDropNum;					// O=    source Drop frame number
	int nCodecDropNum;					// O=    codec dropped frame number
	int nRenderDropNum;					// O=    render dropped frame number
	int nDecodedNum;					// O=    decoded frame number
	int nRenderNum;						// O=    rendered frame number
	int nSourceTimeNum;					// I/O=  number source exceeds the time (I / ms)
	int nCodecTimeNum;					// I/O=  number codec exceeds the time  (I /ms)
	int nRenderTimeNum;					// I/O=  number render exceeds the time (I /ms) 
	int nJitterNum;						// I/O=  number jitter exceeds the time (I /ms)
	int nCodecErrorsNum;				// O=    number codec dropped with encounter errors
	int *nCodecErrors;					// O=    codec errors
	int nCPULoad;						// I/O=  current CPU load in percent
	int nFrequency;						// O=    current frequency CPU is scaled to
	int nMaxFrequency;					// O=    maximum frequency CPU 
	int nWorstDecodeTime;				// O=    worst codec decode time (ms)
	int nWorstRenderTime;				// O=    worst render time (ms)
	int nAverageDecodeTime;				// O=    average codec decode time (ms) 
	int nAverageRenderTime;				// O=    average render time (ms) 
	int nTotalCPULoad;					// I/O=  current total CPU load in percent
	int nTotalPlaybackDuration;			/*!< The playback duration from the beginning of playback to the time analytics is queried */
	int nTotalSourceDropNum;			/*!< Source dropped frame number from the beginning of playback to the time analytics is queried */
	int nTotalCodecDropNum;				/*!< Codec dropped frame number from the beginning of playback to the time analytics is queried */
	int nTotalRenderDropNum;			/*!< Render dropped frame number from the beginning of playback to the time analytics is queried */
	int nTotalDecodedNum;				/*!< Decoded frame number from the beginning of playback to the time analytics is queried */
	int nTotalRenderNum;				/*!< Rendered frame number from the beginning of playback to the time analytics is queried */
}VOMP_PERFORMANCEDATA;

/* below is for NativeWindow */
typedef enum
{
	// Allocate native buffers and set them to Nvidia Component by VOMPUSEGRAPHICBUFFER callback
	// pParam1: VOMP_NATIVEWINDOWALLOCBUFFER*
	// pParam2: not specified
	VOMP_NATIVEWINDOW_AllocBuffers		= 1, 

	// Dequeue native buffer from native window
	// pParam1: native buffer pointer pointer, void**
	// pParam2: VOMP_NATIVEWINDOWBUFFERHEADERS*
	VOMP_NATIVEWINDOW_DequeueBuffer		= 2, 

	// Cancel native buffer to native window, must thread sync with Dequeue
	// We will do nothing if the native buffer is not dequeued
	// pParam1: native buffer pointer, void*
	// pParam2: not specified
	VOMP_NATIVEWINDOW_CancelBuffer		= 3, 

	// Queue native buffer to native window
	// Actually we do not call it because VOAP_IDC_renderVideo will do it in video sink callback
	// pParam1: native buffer pointer, void*
	// pParam2: not specified
	VOMP_NATIVEWINDOW_QueueBuffer		= 4, 

	// Lock native buffer so that Nvidia component can write it
	// pParam1: native buffer pointer, void*
	// pParam2: not specified
	VOMP_NATIVEWINDOW_LockBuffer		= 5, 

	// Prepare native buffer before send it to render: dequeue & lock
	// pParam1: [In] lock type, 1: Only lock NativeWindow; 2: Only lock Graphic Buffer; 3: both lock, int*; [Out] index of the returned buffer header, int*
	// pParam2: pParam2: VOMP_NATIVEWINDOWBUFFERHEADERS*
	VOMP_NATIVEWINDOW_PrepareBuffer = 6, 


	// Set crop to native window when receive OMX_IndexConfigCommonOutputCrop
	// pParam1: VOMP_NATIVEWINDOWCROP*
	// pParam2: not specified
	VOMP_NATIVEWINDOW_SetCrop			= 7, 

	// Free resource of native buffer
	// if the native buffer dequeued, we will cancel it to native window, must thread sync with Dequeue
	// pParam1: native buffer pointer, void*
	// pParam2: not specified
	VOMP_NATIVEWINDOW_FreeBuffer		= 8, 

	// Check if native buffer dequeued from native window
	// pParam1: native buffer pointer, void*
	// pParam2: unsigned int*, 1 - dequeued; 0 - not dequeued
	VOMP_NATIVEWINDOW_IsBufferDequeued	= 9, 

	// Get minimum number of buffers that must remain un-dequeued after a buffer has been queued
	// pParam1: unsigned int*, minimum number of buffers that must remain un-dequeued after a buffer has been queued
	// pParam2: not specified
	VOMP_NATIVEWINDOW_GetMinUndequeuedBuffers	= 10
} VOMP_NATIVEWINDOW_COMMANDID;

typedef struct VOMP_NATIVEWINDOWCOMMAND {
	VOMP_NATIVEWINDOW_COMMANDID		nCommand;	// Command ID
	void*							pParam1;	// Parameter 1
	void*							pParam2;	// Parameter 2
	unsigned int							nReserved;	// Reserved for future
} VOMP_NATIVEWINDOWCOMMAND;

typedef int (* VOMPGETGRAPHICBUFFERUSAGE) (void* pUserData, void* pGBUsage);
typedef int (* VOMPUSEGRAPHICBUFFER) (void* pUserData, void* pUseAndroidNativeBufferParams);

typedef struct VOMP_NATIVEWINDOWALLOCBUFFER {
	unsigned int					nPortIndex;
	unsigned int					nWidth;
	unsigned int					nHeight;
	unsigned int					nColor;
	void**							ppBufferHeader;
	unsigned int					nBufferCount;
	//VOMPGETGRAPHICBUFFERUSAGE fGetGraphicBufferUsage;
	VOMPUSEGRAPHICBUFFER			fUseGraphicBuffer;
	void*							pUserData;
	unsigned int					nBufferSize;
} VOMP_NATIVEWINDOWALLOCBUFFER;

typedef struct VOMP_NATIVEWINDOWCROP {
	int					nLeft;
	int					nTop;
	int					nRight;
	int					nBottom;
} VOMP_NATIVEWINDOWCROP;

typedef struct VOMP_NATIVEWINDOWBUFFERHEADERS {
	unsigned int					nBufferCount;		// buffer header count
	void**							ppBufferHeader;		// buffer headers' pointer
} VOMP_NATIVEWINDOWBUFFERHEADERS;

typedef struct __VOMP_LISTENERINFO {

	VOMPListener		pListener;
	void*				pUserData;
}VOMP_LISTENERINFO;

#ifdef _WIN32
typedef enum
{
	VOMP_RENDER_TYPE_DDRAW,					/*!< Using Direct Draw render, for windows platform only */
	VOMP_RENDER_TYPE_GDI,					/*!< Using GDI window render, for windows platform only */
	VOMP_RENDER_TYPE_DC,					/*!< Using DC(Device Context) render, for windows platform only */
	VOMP_RENDER_TYPE_MAX = 0X7FFFFFFF		/*!< Max value definition */
}VOMP_RENDER_TYPE;
#endif

/**
 * Structure of signal buffer returned.
 */	
typedef struct 
{
	int					nTrackType;			/*!< video/audio/subtitle, VOMP_SS_XXX	*/
	int					nRendered;			/*!< if the buffer has been rendered, 0 means not, others mean yes	*/
	VOMP_BUFFERTYPE *	pBuffer;			/*!< buffer pointer	*/
} VOMP_SIGNALBUFFERRETURNED;

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif // __vompType_h__
