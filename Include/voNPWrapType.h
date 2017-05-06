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


#ifndef __voNPWrapType_h__
#define __voNPWrapType_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * Defination of return error code
 */
#define	VONP_ERR_None				0X00000000	/*!<No Error */
#define	VONP_ERR_EOS                0X00000001	/*!<Reach the end of source */
#define	VONP_ERR_Retry				0X00000002	/*!<The buffer was empty, try it again */
#define	VONP_ERR_VideoCodec			0X80000001	/*!<Error video codec */
#define	VONP_ERR_AudioCodec			0X80000002	/*!<Error audio codec */
#define	VONP_ERR_OutMemory			0X80000003	/*!<Error is out of memory */
#define	VONP_ERR_Pointer			0X80000004	/*!<The pointer is invoid */
#define	VONP_ERR_ParamID			0X80000005	/*!<The param id was not supported */
#define	VONP_ERR_Status				0X80000006	/*!<The status is wrong */
#define	VONP_ERR_Implement			0X80000007	/*!<This function was not impement */
#define	VONP_ERR_WaitTime			0X80000008	/*!<It is waiting reference time (elear)  */
#define	VONP_ERR_Unknown			0X80000009	/*!<Unknow error*/
#define	VONP_ERR_Format_NotSupport	0X8000000A	/*!<Not support codec*/
#define	VONP_ERR_Format_Changed		0X0000000B	/*!<Format changed*/
#define VONP_ERR_Audio_No_Now		0X0000000C  /*!<Indicate audio is not available now*/
#define VONP_ERR_Video_No_Now		0X0000000D  /*!<Indicate video is not available now*/
#define VONP_ERR_FLush_Buffer		0X0000000E  /*!<Indicate need flush buffers*/
#define VONP_ERR_SmallSize          0X8000000F  /*!<Indicate input buffer size is small*/
#define	VONP_ERR_SeekFailed         0X80000010	/*!<Indicate seek failed*/


/**
 * Defination parameter ID
 */
#define	VONP_PID_AUDIO_VOLUME		0X00000001	/*!<Set the audio volume, the volume is 0 - 100, 0 is mute and 100 is max*/
#define	VONP_PID_FUNC_LIB			0X00000002	/*!<Set lib func set.  VONP_LIB_FUNC * It will support next build. */	
#define VONP_PID_STATUS				0X00000003	/*!<Get player status. VONP_STATUS *    */
#define VONP_PID_LISTENER			0X00000004  /*!<Set event callback func,refer to VONP_LISTENERINFO *   */
#define VONP_PID_SENDBUFFERFUNC		0X00000005	/*!<Get send buffer callback func,refer to VONP_SENDBUFFER_FUNC *   */
#define VONP_PID_JAVAENV			0X00000006  /*!<Set Java Env> *     */
#define VONP_PID_VIDEO_ONLY			0X00000007  /*!<Set stream only video data */
#define VONP_PID_AUDIO_ONLY			0X00000008  /*!<Set stream only audio data */
#define VONP_PID_AUDIO_STREAM_ONOFF	0X00000009  /*!<Set enable/disable playback audio frame, int *, 0:disable,1:enable */
#define VONP_PID_VIDEO_STREAM_ONOFF	0X0000000A  /*!<Set enable/disable playback video frame, int *, 0:disable,1:enable */
#define VONP_PID_MFW_CFG_FILE		0X00000010  /*!<Set config file, char * */
#define VONP_PID_COMMON_LOGFUNC		0X00000011  /*!<Set log file, VO_LOG_PRINT_CB * */
#define	VONP_PID_COMMAND_STRING		0X00000012	/*!<Get/Set command string,module need parse this string to get the exact command.refer to  char *   */
#define VONP_PID_CPU_INFO			0X00000013	/*!<Get CPU info, VONP_CPU_INFO* */
#define VONP_PID_VIEW_ACTIVE        0X00000014  /*!<Video view go to forground, set view handle, void*  */
#define VONP_PID_APPLICATION_SUSPEND 0X00000015  /*!<Set application suspend,such as go backgroud */
#define VONP_PID_APPLICATION_RESUME 0X00000016  /*!<Set application resume,such as go foreground, it need set new view handle,void* */
#define	VONP_PID_AUDIO_BUFFER		0X00000017	/*!<Get audio sample buffer. VONP_BUFFERTYPE** */
#define	VONP_PID_VIDEO_BUFFER		0X00000018	/*!<Get video sample buffer. VONP_BUFFERTYPE** */
#define VONP_PID_AUDIO_FORMAT       0X00000019  /*!<Get PCM audio format. VONP_AUDIO_FORMAT* */
#define VONP_PID_PLAYER_PATH        0X0000001A  /*!<Set player work path. char *   */
    
/**
 * Defination call back ID
 */
#define	VONP_CB_PlayComplete			0X00000001	/*!<Play source was finished */
#define	VONP_CB_StartBuff               0X00000002	/*!<The stream start buffering  */
#define	VONP_CB_StopBuff				0X00000003	/*!<The stream stop buffering  */
#define	VONP_CB_Error                   0X80000004	/*!<Unknown error.  */
#define VONP_CB_SeekComplete			0X00000005  /*!<Seek was finished. param 1 should be the seek return timestamp long long* , param 2 should be the pointor of VONP_ERR_XXX value, VONP_ERR_SeekFailed or VONP_ERR_None*/
#define VONP_CB_Codec_NotSupport		0X80000006  /*!<Does not support the codec.  */	
#define VONP_CB_VideoSizeChanged		0X00000007	/*!<Video size changed, width int *, height int * */
#define VONP_CB_VideoAspectRatio		0X00000008	/*!<Video aspect ratio changed, int * ,refer to VONP_ASPECT_RATIO*/
#define VONP_CB_PlayerReadyRun          0X00000009	/*!<Native player is redy to run*/
#define VONP_CB_VideoRenderStart        0X0000000A  /*!<Notify video render started */
#define VONP_CB_NativePlayerError       0X0000000B  /*!<Notify native player occur error while playback */
    
    

/**
* Enumeration for open source flag, used in Open() API
*/
typedef enum
{
	VONP_FLAG_SOURCE_URL			= 0X00000001,		/*!<Indicate the source is URL */
	VONP_FLAG_SOURCE_READBUFFER     = 0X00000002,		/*!<Indicate the source is out of buffer. ReadBuffer */
	VONP_FLAG_SOURCE_SENDBUFFER     = 0X00000004,		/*!<Indicate the source is out of buffer. SendBuffer */
	VONP_FLAG_SOURCE_MAX			= 0X7FFFFFFF		/*!<Max value definition */
}VONP_OPEN_SOURCE_FLAG;


/**
* Enumeration for send/read buffer flag, usend in struct VONP_BUFFERTYPE
*/
typedef enum
{
	VONP_FLAG_BUFFER_UNKNOWN		= 0X80000000,		/*!<Indicate the buffer is unknow,need checking */
	VONP_FLAG_BUFFER_KEYFRAME		= 0X00000001,		/*!<Indicate the buffer is key frame */
	VONP_FLAG_BUFFER_NEW_PROGRAM	= 0X00000002,		/*!<Indicate the buffer start new program,codec is changed,
															 bitrate or samplerate is changed,timestamp is reset
															 pData point to VONP_BUFFER_FORMAT,pBuffer is NULL*/	
	VONP_FLAG_BUFFER_NEW_FORMAT		= 0X00000004,		/*!<Indicate the buffer start new format,codec is not changed,
															 pData point to VONP_BUFFER_FORMAT,pBuffer is NULL*/ 
	VONP_FLAG_BUFFER_HEADDATA		= 0X00000020,		/*!<Indicate the buffer is head data */
	VONP_FLAG_BUFFER_DROP_FRAME		= 0X00001000,		/*!<Indicate the previous buffer had dropped */
	VONP_FLAG_BUFFER_DELAY_TO_DROP	= 0x00020000,		/*!<Indicator buffer need be dropped by delay */
    VONP_FLAG_BUFFER_TIMESTAMP_RESET= 0X00040000,       /*!<Indicate the sample timestamp rollback */
    VONP_FLAG_BUFFER_DECODER_ONLY   = 0X00080000,       /*!<Indicate the sample just need decode,not need render */
	VONP_FLAG_BUFFER_MAX			= 0X7FFFFFFF		/*!<Max value definition */
}VONP_BUFFER_FLAG;
	
	
/**
 * General audio format info
 */
typedef struct
{
	int		nSampleRate;  /*!< Sample rate */
	int		nChannels;    /*!< Channel count */
	int		nSampleBits;  /*!< Bits per sample */
} VONP_AUDIO_FORMAT;

	
/**
 * General video format info
 */
typedef struct
{
	int					nWidth;		 /*!< Width */
	int					nHeight;	 /*!< Height */
	int					nType;		 /*!< Color type  */
} VONP_VIDEO_FORMAT;

	
/**
 * Defination of buffer format structure
 */
typedef struct
{
	int					nStreamType;		/*!< Stream type, refer to VONP_SOURCE_STREAMTYPE */
	int					nCodec;				/*!< Audio or Video format,refer to VONP_AUDIO_CODINGTYPE or VONP_VIDEO_CODINGTYPE  */
	int					nFourCC;			/*!< Audio or Video FourCC  */

    union 
	{
        VONP_AUDIO_FORMAT audio;			/*!< Audio format info */
        VONP_VIDEO_FORMAT video;			/*!< Video format info */
    } sFormat;
	
	int			nHeadDataSize;				/*!< Head data length */
	void*		pHeadData;					/*!< Head data buffer */
} VONP_BUFFER_FORMAT;

	
/**
 * General data buffer, used as input or output.
 */
typedef struct
{
	int				nSize;			/*!< Buffer size in byte */
	unsigned char *	pBuffer;		/*!< Buffer pointer */
	long long		llTime;			/*!< The time of the buffer */
	int				nFlag;			/*!< The flag of the buffer,refer to VONP_BUFFER_FLAG */
	void *			pData;			/*!< The special data pointer depend on the flag */
	long long		llReserve;		/*!< The reserve value */
} VONP_BUFFERTYPE;


/**
*the lib operator
*/
typedef struct
{
	void *	pUserData;																		/*!< User data  */
	void *	(* LoadLib) (void * pUserData, char * pLibName, int nFlag);						/*!< Address of load lib API */
	void *	(* GetAddress) (void * pUserData, void * hLib, char * pFuncName, int nFlag);	/*!< Address of get addr API  */
	int		(* FreeLib) (void * pUserData, void * hLib, int nFlag);							/*!< Address of free lib API  */
} VONP_LIB_FUNC;


/**
*the read buffer func
*/
typedef struct
{
	void *	pUserData;														/*!< User data  */
	int		(* ReadAudio) (void * pUserData, VONP_BUFFERTYPE * pBuffer);	/*!< Address of read audio API */
	int		(* ReadVideo) (void * pUserData, VONP_BUFFERTYPE * pBuffer);	/*!< Address of read video API */
} VONP_READBUFFER_FUNC;


/**
*the send buffer func
*/
typedef struct
{
	void *	pUserData;																	/*!< User data  */
	int		(* SendData) (void * pUserData, int nSSType, VONP_BUFFERTYPE * pBuffer);	/*!< Address of send buffer API,nSSType value is enum VONP_SOURCE_STREAMTYPE*/
} VONP_SENDBUFFER_FUNC;


/**
 *Enumeration used to define the possible source stream type.Used in VONP_SENDBUFFER_FUNC
 */
typedef enum 
{
	VONP_SS_Audio = 0,				/**< the source stream is Audio  */
	VONP_SS_Video,					/**< the source stream is Video  */
	VONP_SS_TS,						/**< the source stream is TS  */
	VONP_SS_MFS,					/**< the source stream is MFS  */
	VONP_SS_MAX		
} VONP_SOURCE_STREAMTYPE;

	
/**
 *Enumeration used to define the possible audio codings.
 */
typedef enum 
{
	VONP_AUDIO_CodingUnused = 0,  /**< Placeholder value when coding is N/A  */
	VONP_AUDIO_CodingPCM,         /**< Any variant of PCM coding */
	VONP_AUDIO_CodingADPCM,       /**< Any variant of ADPCM encoded data */
	VONP_AUDIO_CodingAMRNB,       /**< Any variant of AMR encoded data */
	VONP_AUDIO_CodingAMRWB,       /**< Any variant of AMR encoded data */
	VONP_AUDIO_CodingAMRWBP,      /**< Any variant of AMR encoded data */
	VONP_AUDIO_CodingQCELP13,     /**< Any variant of QCELP 13kbps encoded data */
	VONP_AUDIO_CodingEVRC,        /**< Any variant of EVRC encoded data */
	VONP_AUDIO_CodingAAC,         /**< Any variant of AAC encoded data, 0xA106 - ISO/MPEG-4 AAC, 0xFF - AAC */
	VONP_AUDIO_CodingAC3,         /**< Any variant of AC3 encoded data */
	VONP_AUDIO_CodingFLAC,        /**< Any variant of FLAC encoded data */
	VONP_AUDIO_CodingMP1,		 /**< Any variant of MP1 encoded data */
	VONP_AUDIO_CodingMP3,         /**< Any variant of MP3 encoded data */
	VONP_AUDIO_CodingOGG,         /**< Any variant of OGG encoded data */
	VONP_AUDIO_CodingWMA,         /**< Any variant of WMA encoded data */
	VONP_AUDIO_CodingRA,          /**< Any variant of RA encoded data */
	VONP_AUDIO_CodingMIDI,        /**< Any variant of MIDI encoded data */
	VONP_AUDIO_CodingDRA,         /**< Any variant of dra encoded data */
	VONP_AUDIO_CodingG729,        /**< Any variant of dra encoded data */
	VONP_AUDIO_CodingEAC3,		 /**< Any variant of Enhanced AC3 encoded data */
	VONP_AUDIO_CodingAPE,		 /**< Any variant of APE encoded data */
	VONP_AUDIO_CodingALAC,		 /**< Any variant of ALAC encoded data */
	VONP_AUDIO_CodingDTS,		 /**< Any variant of DTS encoded data */
	VONP_AUDIO_Coding_MAX		= 0X7FFFFFFF
} VONP_AUDIO_CODINGTYPE;

	
/**
 * Video Coding type
 */
typedef enum 
{
    VONP_VIDEO_CodingUnused,     /*!< Value when coding is N/A */
    VONP_VIDEO_CodingMPEG2,      /*!< AKA: H.262 */
    VONP_VIDEO_CodingH263,       /*!< H.263 */
    VONP_VIDEO_CodingS263,       /*!< H.263 */
    VONP_VIDEO_CodingMPEG4,      /*!< MPEG-4 */
    VONP_VIDEO_CodingH264,       /*!< H.264/AVC */
    VONP_VIDEO_CodingWMV,        /*!< all versions of Windows Media Video */
    VONP_VIDEO_CodingRV,         /*!< all versions of Real Video */
    VONP_VIDEO_CodingMJPEG,      /*!< Motion JPEG */
    VONP_VIDEO_CodingDIVX,	    /*!< DIV3 */
    VONP_VIDEO_CodingVP6,		/*!< VP6 */
    VONP_VIDEO_Coding_Max		= 0X7FFFFFFF
} VONP_VIDEO_CODINGTYPE;

	
/**
 *the status of the engine	
 */
typedef enum
{
	VONP_STATUS_INIT			= 0,			/*!<The status is init */
	VONP_STATUS_LOADING			= 1,			/*!<The status is loading */
	VONP_STATUS_RUNNING			= 2,			/*!<The status is running */
	VONP_STATUS_PAUSED			= 3,			/*!<The status is paused */
	VONP_STATUS_STOPPED			= 4,			/*!<The status is stopped */
	VONP_STATUS_MAX				= 0X7FFFFFFF
}VONP_STATUS;	
	
	
/**
 *Call back function. Update the status from OMX AL
 * \param nID [in] the id of special event
 * \param pParam1 [in/out] the first parameter
 * \param pParam2 [in/out] the second parameter
 */
typedef int (* VONPListener) (void * pUserData, int nID, void * pParam1, void * pParam2);
	
	
/*!
 * Definition of even callback
 */	
typedef struct 
{
	VONPListener    pListener;
	void*           pUserData;
}VONP_LISTENERINFO;
	
	
/**
 * CPU info
 */
typedef struct
{
	int			nCoreCount;		/*!<The CPU count */
	int			nCPUType;		/*!<The CPU type,0:neon not supoort, 1:support neon */
	int			nFrequency;		/*!<The frequency of CPU */
	long long	llReserved;		/*!<The reserved value */
}VONP_CPU_INFO;	
	

/*!
 * Definition of aspect ratio type
 */
typedef enum
{
    VONP_RATIO_00			= 0x00,	 /*!< Width and height is width: height */
    VONP_RATIO_11			= 0X01,  /*!< Width and Height is 1 : 1		*/
    VONP_RATIO_43			= 0X02,  /*!< Width and Height is 4 : 3		*/
    VONP_RATIO_169          = 0X03,  /*!< Width and Height is 16 : 9	*/
    VONP_RATIO_21			= 0X04,  /*!< Width and Height is 2 : 1		*/
    VONP_RATIO_MAX          = 0X7FFFFFF
}VONP_ASPECT_RATIO;

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif // __voNPWrapType_h__
