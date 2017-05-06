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

#ifndef __voedtType_H__
#define __voedtType_H__

#include "voType.h"
#include "voIVCommon.h"
#include "voAudio.h"
#include "voVideo.h"
#include "voFile.h"

/*clip pointer
* this pointer will be used in other operations.
*/
typedef void* VOEDT_CLIP_HANDLE;


/**
* Error code
*/
#define VOEDT_ERR_NONE				VO_ERR_NONE
#define VOEDT_ERR_BASE				0x96000000
#define VOEDT_ERR_FAILED				(VOEDT_ERR_BASE | 0x0001)             

#define VOEDT_ERR_OPEN_VIDEOENCODER_FAIL		(VOEDT_ERR_BASE | 0X0002)
#define VOEDT_ERR_OPEN_AUDIOENCODER_FAIL		(VOEDT_ERR_BASE | 0X0003)
#define VOEDT_ERR_OPEN_FILESOURCE_FAIL			(VOEDT_ERR_BASE | 0x0004)
#define VOEDT_ERR_NOT_SETEDITORPARAM			(VOEDT_ERR_BASE | 0x0005)
#define VOEDT_ERR_INVLAID_FILE					(VOEDT_ERR_BASE | 0x0006)
#define VOEDT_ERR_INVLAID_TRACK					(VOEDT_ERR_BASE | 0x0007)
#define VOEDT_ERR_END_OF_FILE					(VOEDT_ERR_BASE | 0x0008)
#define VOEDT_ERR_OPEN_FAIL						(VOEDT_ERR_BASE | 0x0009)
#define VOEDT_ERR_CLIP_NOTFOUND					(VOEDT_ERR_BASE | 0x000A)
#define VOEDT_ERR_NOT_INIT						(VOEDT_ERR_BASE | 0x000B)
#define VOEDT_ERR_BUFFER_INIT_FAILED			(VOEDT_ERR_BASE | 0x000C)
#define VOEDT_ERR_PID_NOTFOUND			(VOEDT_ERR_BASE | 0x000D)
#define VOEDT_ERR_OUTPUT_FORMAT			(VOEDT_ERR_BASE | 0x000E) 
#define VOEDT_ERR_NO_INIT_AUDIOVIDEO_PARAM_FIRST (VOEDT_ERR_BASE | 0x000F) /* for encoder , must set video/audio output param first*/
#define VOEDT_ERR_INVLAID_FORMAT							(VOEDT_ERR_BASE | 0x0010)
#define VOEDT_ERR_OPEN_FILESINK_FAIL			(VOEDT_ERR_BASE | 0x0011)
/*flag type*/
#define VOEDT_FLAG_SOURCE_URL				0X00000010	/*!<Indicate the source is URL */
#define VOEDT_FLAG_SOURCE_BUFFER		0X00000011	/*!<Indicate the source is Buffer Pointer */
#define VOEDT_FLAG_SOURCE_NULL			0X00000012	/*!<Indicate the source is a NULL Pointer,Only for text because Text source use SetTextContent func */

/*Clip type PID*/
/*if A clip is Video ,and it has a audio channel
*VOEDT_PID_CLIP_TYPE_IMAGE for picture
*/
#define  VOEDT_PID_CLIP_TYPE_VIDEO				0x0001
#define	  VOEDT_PID_CLIP_TYPE_AUDIO				0x0002
#define  VOEDT_PID_CLIP_TYPE_TEXT					0x0004
#define  VOEDT_PID_CLIP_TYPE_IMAGE				0x0008 |VOEDT_PID_CLIP_TYPE_VIDEO

/*Play MODE*/
/*if a video clip need operates with Video and Audio together
*you should set Play mode VOEDT_PLAY_WITH_VIDEO|VOEDT_PLAY_WITH_AUDIO
*/
#define VOEDT_PID_PLAYMODE_VIDEO				0x0001
#define VOEDT_PID_PLAYMODE_AUDIO				0x0002
#define VOEDT_PID_PLAYMODE_IMG					0x0004 |VOEDT_PID_PLAYMODE_VIDEO

/*VideoEditor action mode*/
#define VOEDT_ACTIONMODE_PREVIEW_VIDEO					0x000001
#define VOEDT_ACTIONMODE_PREVIEW_AUDIO				0x000002
#define	VOEDT_ACTIONMODE_TRANSCODE						0x000004

/*define image type*/
#define VOEDT_IMG_EXTENTION_BMP		"bmp"
#define VOEDT_IMG_EXTENTION_JPG			"jpg"
#define VOEDT_IMG_EXTENTION_PNG		"png"

/*VOEDT get /set Type(param ID) for SetParam/GetParam*/
//#define VOEDT_PID_THUMBNAIL_BUFFER				0X00000001	/*thumbnail buffer,VOEDT_VIDEO_BUFFER */
#define VOEDT_PID_THUMBNAIL_FORMAT				0X00000002	/*Get/Set, thumbnail FORMAT,VOEDT_VIDEO_FORMAT* */
//#define VOEDT_PID_OUTPUT_FORMAT					0X00000003	/*Get/Set, output Video FORMAT , VOEDT_VIDEO_FORMAT* ,the data will be modified to the mulitipile of 16, it is used for SetEditorPosition ,Preview & transcode Mode */
#define VOEDT_PID_CLIP_PARAM							0X00000004	/*Get/Set, CLIP FORMAT,VOEDT_CLIP_PARAM* */
#define VOEDT_PID_ACTION_MODE						0X00000005	/*Get/Set, ACTION mode,int*
																										* it must be set before 'PlayEditor'
																										*/
#define VOEDT_PID_OUTPUT_AUDIO_PARAM			0X00000006	/*Get/Set,OUTPUT FORMAT,VOEDT_OUTPUT_AUDIO_PARAM * */
#define VOEDT_PID_OUTPUT_VIDEO_PARAM			0X00000007	/*Get/Set,OUTPUT FORMAT,VOEDT_OUTPUT_VIDEO_PARAM * */
#define VOEDT_PID_OUTPUT_SINK_PARAM				0X00000008	/*Get/Set,OUTPUT FORMAT,VOEDT_OUTPUT_SINK_PARAM *
																										*  Set audio /video output param must earlier than sink param
																										*/

#define VOEDT_PID_ANY_POSITION						0X00000009	/*Set, the BeginTime to start Play,it is not support.  int * */
#define VOEDT_PID_DEFAULT_POSITION					0X0000000A	/*Set, the BeginTime to start Play, it is not support. (0, 1) int * */
//#define VOEDT_PID_TEXT_URL								0X0000000B	/*text string , only for text  VO_CHAR*  */
#define VOEDT_PID_AUDIO_FORMAT					0X0000000C	/*Get/Set, audio format , VOEDT_AUDIO_FORMAT */
#define VOEDT_PID_VIDEO_FORMAT					0X0000000D	/*Get/Set, video format , VOEDT_VIDEO_FORMAT 
																									* if editor, output is the present used video format
																									* set only for editor,not for clip
																									*/
//#define VOEDT_PID_DURATION							0X0000000E	/*clip duration , VO_U32 */
#define VOEDT_PID_CLIP_NAME							0X0000000F	/*Get, clip name , VO_CHAR* */
#define VOEDT_PID_CLIP_TYPE							0X00000011	/*Get, indicate the clip type (audio/video/audio*video/text) int*  */
#define VOEDT_PID_AUDIO_CODEC					0x00000012  /*Get ,indicate the audio codec int * */
#define VOEDT_PID_VIDEO_CODEC						0X00000013   /*Get, indicate the vidoe codec int * */
#define VOEDT_PID_DROP_BLACK_FRAME			0X00000014 /*Get/Set, int(0,1) */
#define VOEDT_PID_EDITOR_STATUS					0X00000015 /*Get, int(0,1) 0 STOP 1 PLAY*/
#define VOEDT_PID_IMG_BUFFER						0X00000016 /*Set, VOEDT_IMG_BUFFER* */
/*Sample type*/
#define VOEDT_PID_AUDIO_SAMPLE					0x00000017 /*Get , clip sample (VOMP_BUFFERTYPE**) */
#define VOEDT_PID_VIDEO_SAMPLE					0X00000018 /*Get , clip sample (VOMP_BUFFERTYPE**) */ 
#define VOEDT_PID_FLUSH_OP							0X00000020 /*Set,  flush all operation action from action thread, int * */
#define VOEDT_PID_RELOAD								0X00000021/*Set, reload clip, only for text now, int * */
#define VOEDT_PID_FRAMERATE							0X00000022/*Set/get, editor video framerate, for editor play & transcode , int *
																								(*100,eg 25f/s ,you need set 2500) */
#define VOEDT_PID_THUMBNAILCOUNTS_INLIST 0X00000023 /* define is 20,int* */
#define VOEDT_PID_SETPOSCOUNTS_INLIST         0X00000024 /* define is 2, int * */
/**
* callback data struct, All callback operations will return this struct 
*/ 
typedef struct 
{
	void				*pData;			/* the special data depend on the nPid*/
	void				*pUserData;
	VOEDT_CLIP_HANDLE		*pClipHandle;			/*!<	the pointer of clip, Add operation ,this pointer is new.
													* Others ,this pointer is used to check which is the caller.
													*/
	int					nReserve;		/*!< The reserve value */
}	VOEDT_CALLBACK_BUFFER;
/**
* Defination call back ID
*Each callback PID will return a same VOEDT_CALLBACK_BUFFER struct .
*but different PID has different param(pData) type
*/
#define VOEDT_CB_ADD_AUDIO							0X0000A001 /*!<a new clip has been add (VOEDT_CLIP_INFO *) ,the clip has been add on the tail*/
#define VOEDT_CB_ADD_TEXT								0X0000A002 /*!<a new clip has been add (VOEDT_CLIP_INFO *) ,the clip has been add on the tail*/
#define VOEDT_CB_ADD_VIDEO							0X0000A003 /*!<a new clip has been add (VOEDT_CLIP_INFO *) ,the clip has been add on the tail*/
#define VOEDT_CB_ADD_IMG								0X0000A004 /*!<a new clip has been add (VOEDT_CLIP_INFO *) ,the clip has been add on the tail*/

#define VOEDT_CB_CLIP_DELETE							0X0000A011 /*!<a clip has been deleted ,it will trigger VOEDT_CB_EDITOR_UPDATE callback*/
#define VOEDT_CB_CLIP_JUMP							0X0000A012 /*!<JUMP CLIP TO TARGET POSITION,it will trigger VOEDT_CB_EDITOR_UPDATE callback*/
#define VOEDT_CB_CLIP_POSITION						0X0000A013 /*!<CLIP POSITION NOW (int *) */
#define VOEDT_CB_CLIP_DURATION					0X0000A014 /*!<CLIP DURATION (int *) */
#define VOEDT_CB_CLIP_UPDATE						0X0000A015 /*!<CLIP UPDATA (VOEDT_CLIP_INFO *) */
#define VOEDT_CB_CLIP_PARAM							0X0000A016 /*!<CLIP PARAM  (VOEDT_CLIP_PARAM*) */
#define VOEDT_CB_TEXT_CONTENT						0X0000A017 /*!<TEXT CONTENT  (VO_CHAR*) */
#define VOEDT_CB_OUTPUT_VIDEO_PARAM		0X0000A018 /*!<VIDEO OUTPUT PARAM  (VOEDT_OUTPUT_AUDIO_PARAM*) */
#define VOEDT_CB_OUTPUT_AUDIO_PARAM		0X0000A019 /*!<AUDIO OUTPUT PARAM  (VOEDT_OUTPUT_VIDEO_PARAM*) */
#define VOEDT_CB_OUTPUT_SINK_PARAM			0X0000A01A /*!<FILESINK OUTPUT PARAM  (VOEDT_OUTPUT_SINK_PARAM*) */
#define VOEDT_CB_VIDEO_FORMAT				0X0000A01B /*!<VIDEO PREVIEW FORMAT (VOEDT_VIDEO_FORMAT*) */
#define VOEDT_CB_AUDIO_FORMAT				0X0000A01C /*!<VIDEO PREVIEW FORMAT (VOEDT_AUDIO_FORMAT*) */

#define VOEDT_CB_EDITOR_POSITION				0X0000A021 /*!<EDITOR POSITION NOW (int *) */	
#define VOEDT_CB_EDITOR_DURATION				0X0000A022 /*!<EDITOR DURATION  (int *) */	
#define VOEDT_CB_EDITOR_UPDATE					0X0000A023 /*!<EDITOR UPDATE   UI delete all clip,then all new clip will be returned */	
#define VOEDT_CB_EDITOR_DELETE						0X0000A024 /*!<EDITOR DELETE A CLIP ,return a deleted clip ID, int* */

#define VOEDT_CB_VIDEO_NOBLACK_THUMBNAIL				0X0000A030 /*!<One VIDEO thumbnail  of first noblack frame (VOMP_BUFFERTYPE*) ,nReserve for bufsize */	
#define VOEDT_CB_VIDEO_THUMBNAIL				0X0000A031 /*!<One VIDEO thumbnail   (VOMP_BUFFERTYPE*) ,nReserve for bufsize */	
#define VOEDT_CB_VIDEO_SAMPLE						0X0000A032 /*!<One VIDEO SAMPLE		(VOMP_BUFFERTYPE*) */	
#define VOEDT_CB_AUDIO_SAMPLE					0X0000A033/*!<One AUDIO SAMPLE		(VOMP_BUFFERTYPE*) */	
#define VOEDT_CB_AUDIO_CHUNK						0X0000A034 /*!<One AUDIO CHUNK , return this ID when get audio sample by GetEditorAudioSample,
																									* the data is equal to AUDIO SAMPLE , only differ from AUDIO SAMPLE,  (VOMP_BUFFERTYPE*)  */	

#define VOEDT_CB_TRANSCODE_ERROR					0X0000A041 /*!<When transcoding has occured error,Callback this ID */	
#define VOEDT_CB_TRANSCODE_SUCCESS				0X0000A042 /*!<When transcoding has completed,Callback this ID */	
#define VOEDT_CB_ERR_AUDIO_OUTPARAM					0X0000A043 /*!<ERR transcoding audio output param*/
/*transition id*/
#define VOEDT_TRANSITION_NONE					0x0000
/*vidoe*/
#define VOEDT_TRANSITION_VIDEO_BREADCIRCLE		0x0001
#define VOEDT_TRANSITION_VIDEO_CHESSBOARD		0x0002
#define VOEDT_TRANSITION_VIDEO_CONVOLUTE		0x0003
#define VOEDT_TRANSITION_VIDEO_KALEIDOSCOPE		0x0004
#define VOEDT_TRANSITION_VIDEO_STEEL			0x0005
#define VOEDT_TRANSITION_VIDEO_TREE				0x0006
#define VOEDT_TRANSITION_VIDEO_WINDOWS			0x0007
#define VOEDT_TRANSITION_VIDEO_ZOOMOUT			0x0008
/*audio*/
#define VOEDT_TRANSITION_AUDIO_FADE_IN_FADE_OUT	0x0001
#define VOEDT_TRANSITION_AUDIO_MIXING			0x0002

/*not support now*/
typedef enum VOEDT_CALLBACK_TYPE
{
	VOEDT_CALLBACK_AUTO,  //API will callback data by itself ,push mode
	VOEDT_CALLBACK_MANUAL  //Get callback data by caller ,pull mode
}VOEDT_CALLBACK_TYPE;
/**
* Video Coding type
*/
typedef enum VOEDT_VIDEO_CODINGTYPE {
	VOEDT_VIDEO_CodingUnused,     /*!< Value when coding is N/A */
	VOEDT_VIDEO_CodingMPEG2,      /*!< AKA: H.262 */
	VOEDT_VIDEO_CodingH263,       /*!< H.263 */
	VOEDT_VIDEO_CodingS263,       /*!< H.263 */
	VOEDT_VIDEO_CodingMPEG4,      /*!< MPEG-4 */
	VOEDT_VIDEO_CodingH264,       /*!< H.264/AVC */
	VOEDT_VIDEO_CodingWMV,        /*!< all versions of Windows Media Video */
	VOEDT_VIDEO_CodingRV,         /*!< all versions of Real Video */
	VOEDT_VIDEO_CodingMJPEG,      /*!< Motion JPEG */
	VOEDT_VIDEO_CodingDIVX,	   /*!< DIV3 */
	VOEDT_VIDEO_CodingVP6,		  /*!< VP6 */
	VOEDT_VIDEO_Coding_Max		= VO_MAX_ENUM_VALUE
} VOEDT_VIDEO_CODINGTYPE;


/**
*Enumeration used to define the possible audio codings.
*/
typedef enum VOEDT_AUDIO_CODINGTYPE {
	VOEDT_AUDIO_CodingUnused = 0,  /**< Placeholder value when coding is N/A  */
	VOEDT_AUDIO_CodingPCM,         /**< Any variant of PCM coding */
	VOEDT_AUDIO_CodingADPCM,       /**< Any variant of ADPCM encoded data */
	VOEDT_AUDIO_CodingAMRNB,       /**< Any variant of AMR encoded data */
	VOEDT_AUDIO_CodingAMRWB,       /**< Any variant of AMR encoded data */
	VOEDT_AUDIO_CodingAMRWBP,      /**< Any variant of AMR encoded data */
	VOEDT_AUDIO_CodingQCELP13,     /**< Any variant of QCELP 13kbps encoded data */
	VOEDT_AUDIO_CodingEVRC,        /**< Any variant of EVRC encoded data */
	VOEDT_AUDIO_CodingAAC,         /**< Any variant of AAC encoded data, 0xA106 - ISO/MPEG-4 AAC, 0xFF - AAC */
	VOEDT_AUDIO_CodingAC3,         /**< Any variant of AC3 encoded data */
	VOEDT_AUDIO_CodingFLAC,        /**< Any variant of FLAC encoded data */
	VOEDT_AUDIO_CodingMP1,		  /**< Any variant of MP1 encoded data */
	VOEDT_AUDIO_CodingMP3,         /**< Any variant of MP3 encoded data */
	VOEDT_AUDIO_CodingOGG,         /**< Any variant of OGG encoded data */
	VOEDT_AUDIO_CodingWMA,         /**< Any variant of WMA encoded data */
	VOEDT_AUDIO_CodingRA,          /**< Any variant of RA encoded data */
	VOEDT_AUDIO_CodingMIDI,        /**< Any variant of MIDI encoded data */
	VOEDT_AUDIO_CodingDRA,         /**< Any variant of dra encoded data */
	VOEDT_AUDIO_CodingG729,        /**< Any variant of dra encoded data */
	VOEDT_AUDIO_CodingEAC3,		  /**< Any variant of Enhanced AC3 encoded data */
	VOEDT_AUDIO_CodingAPE,		  /**< Any variant of APE encoded data */
	VOEDT_AUDIO_CodingALAC,		  /**< Any variant of ALAC encoded data */
	VOEDT_AUDIO_CodingDTS,		  /**< Any variant of DTS encoded data */
	VOEDT_AUDIO_Coding_MAX		= 0X7FFFFFFF
} VOEDT_AUDIO_CODINGTYPE;


/**
* Defination of color format
*/
typedef enum
{
	VOEDT_COLOR_YUV_PLANAR444			= 0,		/*!< YUV planar mode:444  vertical sample is 1, horizontal is 1  */
	VOEDT_COLOR_YUV_PLANAR422_12		= 1,		/*!< YUV planar mode:422, vertical sample is 1, horizontal is 2  */
	VOEDT_COLOR_YUV_PLANAR422_21		= 2,		/*!< YUV planar mode:422  vertical sample is 2, horizontal is 1  */
	VOEDT_COLOR_YUV_PLANAR420			= 3,		/*!< YUV planar mode:420  vertical sample is 2, horizontal is 2  */
	VOEDT_COLOR_YUV_PLANAR411			= 4,		/*!< YUV planar mode:411  vertical sample is 1, horizontal is 4  */
	VOEDT_COLOR_YUV_PLANAR411V			= 37,		/*!< YUV planar mode:411  vertical sample is 4, horizontal is 1  */
	VOEDT_COLOR_GRAY_PLANARGRAY			= 5,		/*!< gray planar mode, just Y value								 */
	VOEDT_COLOR_YUYV422_PACKED			= 6,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: Y0 U0 Y1 V0  */
	VOEDT_COLOR_YVYU422_PACKED			= 7,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: Y0 V0 Y1 U0  */
	VOEDT_COLOR_UYVY422_PACKED			= 8,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: U0 Y0 V0 Y1  */
	VOEDT_COLOR_VYUY422_PACKED			= 9,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: V0 Y0 U0 Y1  */
	VOEDT_COLOR_YUV444_PACKED			= 10,		/*!< YUV packed mode:444, vertical sample is 1, horizontal is 1, data: Y U V	*/
	VOEDT_COLOR_YUV_420_PACK				= 11, 		/*!< YUV planar mode:420  vertical sample is 2, horizontal is 2  , Y planar, UV Packed*/
	VOEDT_COLOR_YUV_420_PACK_2			= 35, 		/*!< YUV planar mode:420  vertical sample is 2, horizontal is 2  , Y planar, VU Packed*/
	VOEDT_COLOR_YVU_PLANAR420			= 12,
	VOEDT_COLOR_YVU_PLANAR422_12			= 13,
	VOEDT_COLOR_YUYV422_PACKED_2			= 14,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: Y1 U0 Y0 V0  */
	VOEDT_COLOR_YVYU422_PACKED_2			= 15,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: Y1 V0 Y0 U0  */
	VOEDT_COLOR_UYVY422_PACKED_2			= 16,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: U0 Y1 V0 Y0  */
	VOEDT_COLOR_VYUY422_PACKED_2			= 17,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: V0 Y1 U0 Y0  */
	VOEDT_COLOR_RGB565_PACKED			= 30,		/*!< RGB packed mode, data: B:5 G:6 R:5   						 */
	VOEDT_COLOR_RGB555_PACKED			= 31,		/*!< RGB packed mode, data: B:5 G:5 R:5   						 */
	VOEDT_COLOR_RGB888_PACKED			= 32,		/*!< RGB packed mode, data: B G R		 						 */
	VOEDT_COLOR_RGB32_PACKED				= 33,		/*!< RGB packed mode, data: B G R A								 */
	VOEDT_COLOR_RGB888_PLANAR			= 34,		/*!< RGB planar mode											 */
	VOEDT_COLOR_YUV_PLANAR420_NV12		= 36,		/*!< YUV planar mode:420  vertical sample is 2, horizontal is 2  */
	VOEDT_COLOR_ARGB32_PACKED			= 37,		/*!< ARGB packed mode, data: B G R A							 */
	VOEDT_COLOR_TYPE_MAX					= 0X7FFFFFF
}  VOEDT_COLORTYPE;


typedef struct
{
	VO_PBYTE 			pBuffer;			/*!< Buffer pointer */
	VO_U32				nWidth;			/*!<IMG nWidth*/
	VO_U32				nHeight;			/*!<IMG nHeight*/
	VO_IV_COLORTYPE		ColorType;			/*!< Color Type ,now support RGB888*/
} VOEDT_IMG_BUFFER;

typedef struct  
{
	VO_CHAR cFont[25];
	VO_S32 nColor;
	VO_U32 nSize;
	VO_S32 nColorBg;
}VOEDT_TEXT_FORMAT;


/**
* General audio format info
*/
typedef struct
{
	VO_U32		SampleRate;  /*!< Sample rate */
	VO_U32		Channels;    /*!< Channel count */
	VO_U32		SampleBits;  /*!< Bits per sample */
} VOEDT_AUDIO_FORMAT;

typedef struct  
{
	VO_U32 nWidth;
	VO_U32 nHeight;
	VO_U32 ColorType;
}VOEDT_VIDEO_FORMAT;

/*this struct indicate transition-effect buffer*/
typedef struct  
{
	/*transition id*/
	VO_S32 nType;	

	/*video: VOEDT_VIDEO_BUFFER struct
	* audio: a memory space
	*/
	VO_PBYTE effectBuffer; 
	VO_S32 nSize;
	/*transiton step,at present */
	float fTransitionValue;
}VOEDT_TRANSITION_BUFFER;

/*A struct used to set transition info
* set by user
*/
typedef struct 
{
	/*transiton type [1-8]*/
	VO_S32 nType;
	/*transiton duration 
	*Audio : if has video ,this param set the same as video for recommended
	*/
	VO_S32 nDuration;
	/*transition opertion Func pointer
	*This function can be set NULL,if use default transition function
	*[param] pBuf1 [in/out] (in)the first sample , (out) the blended sample of first and second
	*[param] pBuf2 [in]	the second sample
	* for default transition effcetion ,the first and second sample' size must the same. 
	* for default transition effcetion,the size equals to output size or preview size
	* for default transition effcetion,the input & output ColorType must be VO_COLOR_ARGB32_PACKED
	*/
	VO_S32 (*pEffectFunc)(void *pUser,VOEDT_TRANSITION_BUFFER *pBuf1,VOEDT_TRANSITION_BUFFER *pBuf2);
	/*set transition target size for video*/
	VO_S32 (*pSetSize)(void *pUser,int nWidth,int nHeight);
	void *	pUser;
}VOEDT_TRANSITION_EFFECT;
#define VOEDIT_PID_PERCENTVALUE 0x01
#define VOEDIT_PID_TRUEVALUE 0x02
typedef struct  
{
	/*to indicate nLeft & nTop type
	* if VOEDIT_PID_PERCENTVALUE, the value of nLeft & nTop is the percent of Target value 
	* if VOEDIT_PID_TRUEVALUE, the value  is nLeft & nTop real value
	*/
	int nType;

	/* clip target left position, but it does not support now*/
	int nLeft;
	/* clip target top position , but it does not support now*/
	int nTop;

	/* clip target Height*/
	int nHeight; 
	/* Clip target Width */
	int nWidth;

}VOEDT_CLIP_POSITION_FORMAT;

/*defination clip param*/
typedef struct  
{

	VO_S32 point_in;/*time where  start play.It is relative to the clip
								*for text & Img,this value will be set to 0
								*/
	VO_S32 point_out;/*time where  end play.It is relative to the clip*/
	VO_S32 beginAtTimeline;/*the starttime is the time to play the clip .It is relative to whole EDITOR time.
							*In VOEDT_PID_ANY_POSITION mode ,the clip will start to play at the postion be set. It is for PIP.
							*In VOEDT_PID_DEFAULT_POSITION mode ,the clip play one by one 
							*But for text ,it is  always in VOEDT_PID_ANY_POSITION mode
							*Now for video , caller donot set this value.
							*for text, this value must be set by caller
							*/
	VO_S32 bloop;    //need loop play file . {0, 1} ,1 for loop 

	/*audio volume [0-200],default 100 is Original Volume */
	VO_S32 nVolume;
	/*refer the pointer of video transition operation */
	VOEDT_TRANSITION_EFFECT *videoTransition;
	/*refer the pointer of audio transition operation*/
	VOEDT_TRANSITION_EFFECT *audioTransition;
	/*A/V  refer Play Type To Run a clip whether need play only audio \ only video \ Video&Audio together?*/
	VO_U32 nPlayMode;

	/* it's only for video \text
	* this param is support for PIP(picture in picture ) function,it will be supported in the future
	* Now user can not set this param
	*/
	VOEDT_CLIP_POSITION_FORMAT positionFormat;
} VOEDT_CLIP_PARAM;



/**
* Callback function. 
* it will return a VOEDT_CALLBACK_BUFFER pointer to AP.
* \param nPid [out]  indicate callback ID
* \param pCallbackData [out] indicate data pointer depend on the nPid
*/
typedef void (VO_API *VOEDTCALLBACK)(int nPid,void *pCallbackData);

typedef struct
{
	VOEDTCALLBACK		pCallback;
	VO_CHAR*			pWorkingPath;
	VO_VOID*			pLibOP;
	VO_VOID*			pUserData;
	VO_VOID*			pReserve;
}VOEDT_INIT_INFO;

/*Video Parameters for output*/
typedef struct 
{
	VOEDT_VIDEO_CODINGTYPE nCodecType;
	VO_U32				nWidth ;
	VO_U32				nHeight ;
	VO_U32				nBitRate;

	/*indicate frame per second, not be set by user
	use ID VOEDT_PID_FRAMERATE to Set FrameRate */
	VO_U32		        nFrameRate ;

	VO_U32				nKeyFrmInterval; 
	VO_IV_QUALITY		nEncQuality;
	VO_IV_COLORTYPE		nColorType;
	VO_IV_RTTYPE		nRotateType;
}VOEDT_OUTPUT_VIDEO_PARAM;
/*Audio Parameters for output*/
typedef struct  
{
	VOEDT_AUDIO_CODINGTYPE nCodecType;
	VO_U32	nBitRate;
	VO_U32	nChannels;
	VO_U32	nSampleRate;
	VO_U32   nSampleBits;
	VO_S16	nAdtsUsed;
}VOEDT_OUTPUT_AUDIO_PARAM;
/*Whole Parameters for output*/
typedef struct  
{
	VO_FILE_FORMAT			packetType;

	VO_CHAR				dumpFilePath[255];
	VO_CHAR				dumpTmpDir[255];
}VOEDT_OUTPUT_SINK_PARAM;

#endif
