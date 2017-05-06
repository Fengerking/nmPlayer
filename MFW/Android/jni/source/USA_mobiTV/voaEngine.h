	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voaMediaPlayer.h

	Contains:	vo MultiMedia Manager Play Engine function define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-05-04		JBF			Create file

*******************************************************************************/
#ifndef __voaMediaPlayer_H__
#define __voaMediaPlayer_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef int64_t
typedef signed long long int64_t;
#endif // int64_t

#define VOA_MEDIAPLAYER_BASE		0X11000000							//!< The base ID of media player >
#define VOA_METADATA_BASE			0X12000000							//!< The base ID of metadata >
#define VOA_MEDIARECORDER_BASE		0X13000000							//!< The base ID of media recorder >

// media player ID
#define VOAP_IDF_onFirstRef			(VOA_MEDIAPLAYER_BASE | 0X0001)		//!< onFirstRef			Set.	Param: No >
#define VOAP_IDF_initCheck			(VOA_MEDIAPLAYER_BASE | 0X0002)		//!< initCheck			Set.	Param: No >
#define VOAP_IDF_setDataSourceURL	(VOA_MEDIAPLAYER_BASE | 0X0003)		//!< setDataSource		Set.	Param: const char * >
#define VOAP_IDF_setDataSourceID	(VOA_MEDIAPLAYER_BASE | 0X0004)		//!< setDataSource		Set.	Param: VOAP_SOURCE * >
#define VOAP_IDF_prepare			(VOA_MEDIAPLAYER_BASE | 0X0005)		//!< prepare			Set.	Param: No >
#define VOAP_IDF_prepareAsync		(VOA_MEDIAPLAYER_BASE | 0X0006)		//!< prepareAsync		Set.	Param: No >
#define VOAP_IDF_start				(VOA_MEDIAPLAYER_BASE | 0X0007)		//!< start				Set.	Param: No >
#define VOAP_IDF_stop				(VOA_MEDIAPLAYER_BASE | 0X0008)		//!< stop				Set.	Param: No >
#define VOAP_IDF_seekTo				(VOA_MEDIAPLAYER_BASE | 0X0009)		//!< seekTo				Set.	Param: int * >
#define VOAP_IDF_pause				(VOA_MEDIAPLAYER_BASE | 0X000A)		//!< pause				Set.	Param: No >
#define VOAP_IDF_isPlaying			(VOA_MEDIAPLAYER_BASE | 0X000B)		//!< isPlaying			Get.	Param: int * 1 playing, 0 No >
#define VOAP_IDF_getCurrentPosition	(VOA_MEDIAPLAYER_BASE | 0X000C)		//!< getCurrentPosition Get.	Param: int * (ms) >
#define VOAP_IDF_getDuration		(VOA_MEDIAPLAYER_BASE | 0X000D)		//!< getDuration		Get.	Param: int * (ms) >
#define VOAP_IDF_release			(VOA_MEDIAPLAYER_BASE | 0X000E)		//!< release			Set.	Param: No >
#define VOAP_IDF_reset				(VOA_MEDIAPLAYER_BASE | 0X000F)		//!< reset				Set.	Param: No >
#define VOAP_IDF_setLooping			(VOA_MEDIAPLAYER_BASE | 0X0010)		//!< setLooping			Set.	Param: int * >
#define VOAP_IDF_playerType			(VOA_MEDIAPLAYER_BASE | 0X0011)		//!< playerType			Get.	Param: int * >
#define VOAP_IDF_invoke				(VOA_MEDIAPLAYER_BASE | 0X0012)		//!< playerType			set.	Param: const Parcel& request, Parcel *reply >
#define VOAP_IDF_getMetadata		(VOA_MEDIAPLAYER_BASE | 0X0013)		//!< playerType			set.	Param: const media::Metadata::Filter& ids, Parcel *records >

#define VOAP_IDF_setCallBack		(VOA_MEDIAPLAYER_BASE | 0X0020)		//!< setCallBack		Set.	Param: 1 Callback, 2 Userdata >
#define VOAP_IDF_setFilePipe		(VOA_MEDIAPLAYER_BASE | 0X0021)		//!< setFilePipe		Set.	Param: 1 void * fFilePipe >

#define VOAP_IDC_notifyEvent		(VOA_MEDIAPLAYER_BASE | 0X0101)		//!< notifyEvent.	Param 1 VOA_NOTIFYEVENT * >
#define VOAP_IDC_displayText		(VOA_MEDIAPLAYER_BASE | 0X0102)		//!< notifyEvent.	Param 1 const char * >

#define VOAP_IDC_setVideoSize		(VOA_MEDIAPLAYER_BASE | 0X0111)		//!< setVideoSize.	Param 1 int W, 2 int H >
#define VOAP_IDC_getVideoBuffer		(VOA_MEDIAPLAYER_BASE | 0X0112)		//!< getVideoBuffer.Param 1 void ** ppBuffer >
#define VOAP_IDC_renderVideo		(VOA_MEDIAPLAYER_BASE | 0X0113)		//!< renderVideo.	Param 1 VOA_VIDEO_BUFFERTYPE* pBuffer >

#define VOAP_IDC_setAudioFormat		(VOA_MEDIAPLAYER_BASE | 0X0121)		//!< setAudioFormat.Param VOA_AUDIO_FORMAT *  >
#define VOAP_IDC_setAudioStart		(VOA_MEDIAPLAYER_BASE | 0X0122)		//!< setAudioStart.	Param no  >
#define VOAP_IDC_setAudioStop		(VOA_MEDIAPLAYER_BASE | 0X0123)		//!< setAudioStop.	Param no  >
#define VOAP_IDC_setAudioPause		(VOA_MEDIAPLAYER_BASE | 0X0124)		//!< setAudioPause.	Param no  >
#define VOAP_IDC_setAudioFlush		(VOA_MEDIAPLAYER_BASE | 0X0125)		//!< setAudioFlush.	Param no  >
#define VOAP_IDC_RenderAudio		(VOA_MEDIAPLAYER_BASE | 0X0126)		//!< renderAudio.	Param 1 unsigned char * pBuffer, 2. int nSize  >

// metadata retriever ID
#define VOAM_IDF_setDataSourceURL	(VOA_METADATA_BASE | 0X0001)		//!< setDataSource		Set.	Param: const char * >
#define VOAM_IDF_setDataSourceID	(VOA_METADATA_BASE | 0X0002)		//!< setDataSource		Set.	Param: VOAP_SOURCE * >
#define VOAM_IDF_setMode			(VOA_METADATA_BASE | 0X0003)		//!< setMode			Set.	Param: int >
#define VOAM_IDF_getMode			(VOA_METADATA_BASE | 0X0004)		//!< getMode			get.	Param: int * >
#define VOAM_IDF_captureFrame		(VOA_METADATA_BASE | 0X0005)		//!< captureFrame		get.	Param: void ** (VideoFrame **) >
#define VOAM_IDF_extractAlbumArt	(VOA_METADATA_BASE | 0X0006)		//!< extractAlbumArt	get.	Param: void ** (MediaAlbumArt **) >
#define VOAM_IDF_extractMetadata	(VOA_METADATA_BASE | 0X0007)		//!< extractMetadata	get.	Param: 1, int  2. char ** >


/**
 * The source structure.
 */
#if 0
typedef struct VOA_SOURCE {
    int			fd;			/*!< File ID  */
    int64_t		offset;		/*!< File begin pos  */
	int64_t		length;		/*!< File length  */
} VOA_SOURCE;
#endif

/**
 * The source structure.
 */
#if 0
typedef struct VOA_NOTIFYEVENT {
    int					msg;		/*!< event ID  */
    int					ext1;		/*!< event param1  */
    int					ext2;		/*!< event param2  */
} VOA_NOTIFYEVENT;
#endif

/**
 * Defination of color format
 */
#if 0
typedef enum
{
	VOA_COLOR_YUV420			= 0,		/*!< YUV planar mode:420  */
	VOA_COLOR_NV12				= 1,		/*!< YUV Y planar UV interlace */
	VOA_COLOR_RGB565			= 10,		/*!< RGB565  */
	VOA_COLOR_RGB888			= 11,		/*!< RGB888  */
	VOA_COLOR_TYPE_MAX			= 0X7FFFFFFF
} VOA_COLORTYPE;
#endif

/**
 * Video data buffer, usually used as iutput video information.
 */
#if 0
typedef struct
{
	unsigned char *			virBuffer[3];			/*!< virtual Buffer pointer */
	unsigned char *			phyBuffer[3];			/*!< physical Buffer pointer */
	int						nStride[3];				/*!< Buffer stride */
	VOA_COLORTYPE			nColorType;				/*!< color type  */
} VOA_VIDEO_BUFFERTYPE;
#endif


/**
 * Audio format
 */
#if 0
typedef struct
{
	int		nSampleRate;		/*!< Audio Samplerate */
	int		nChannels;			/*!< Audio Channels */
	int		nSampleBits;		/*!< Audio Sample Bits */
} VOA_AUDIO_FORMAT;
#endif

/**
 * VisualOn Android Engine Call Back function.
 * \param pUserData [IN] User data from caller.
 * \param nID [IN] The call back id.
 * \param pParam1 [IN/OUT] The call back param 1.
 * \param pParam2 [IN/OUT] The call back param 2.
 * \retval 0 succeeded..
 */
typedef int (* VOACallBack) (void * pUserData, int nID, void * pParam1, void * pParam2);


/**
 * Init the VisualOn Android Engine
 * \param nFlag [IN] 1 Player, 2 Recorder, 3 Metadata.
 * \retval The handle of engine. NULL was failed.
 */
void * voaInit (int nFlag);


/**
 * Set the param into engine
 * \param hEngine [IN] The handle of engine from voaInit.
 * \param nID [IN] The param ID.
 * \param pValue [IN] The param value. Depend of the ID.
 * \retval 0 successful.
 */
int voaSetParam (void * hEngine, int nID, void * pValue1= NULL, void * pValue2 = NULL);

/**
 * Get the param into engine
 * \param hEngine [IN] The handle of engine from voaInit.
 * \param nID [IN] The param ID.
 * \param pValue [OUT] The param value. Depend of the ID.
 * \retval 0 successful.
 */
int voaGetParam (void * hEngine, int nID, void * pValue1 = NULL, void * pValue2 = NULL);


/**
 * Uninit the engine
 * \param hEngine [IN] The handle of engine from voaInit.
 * \param nFlag [IN] 1 Player, 2 Recorder, 3 Metadata.
 * \retval 0 successful.
 */
int voaUninit (void * hEngine, int nFlag);


typedef int (* VOAINIT) (int nFlag);
typedef int (* VOASETPARAM) (void * hEngine, int nID, void * pValue);
typedef int (* VOAGETPARAM) (void * hEngine, int nID, void * pValue);
typedef int (* VOAUNINIT) (void * hEngine);

/**
 * voaEngine structure
 */
typedef struct
{
	VOAINIT			fInit;			/*!< Init function */
	VOASETPARAM		fSetParam;		/*!< SetParam function */
	VOAGETPARAM		fGetParam;		/*!< GetParam function */
	VOAUNINIT		fUninit;		/*!< Uninit function */
	void *			hModule;		/*!< The handle of module */
	void *			hEngine;		/*!< The handle of engine */
	void *			pFilePipe;		/*!< The File Pipe pointer */
	void *			pVideoRender;	/*!< The Video Render pointer */
	void *			pAudioRender;	/*!< The Audio Render pointer */
} VOA_ENGINETYPE;

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // __voaMediaPlayer_H__
