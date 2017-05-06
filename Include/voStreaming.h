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

#ifndef __voStreaming_H__
#define __voStreaming_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef _WIN32
//#include <Windows.h>
#endif // _WIN32
#include "voString.h"

/***************************************
* Common Definition *
***************************************/
#define DEFAULT_PORT_MIN					1024
#define DEFAULT_PORT_MAX					65535
#define DEFAULT_BUFFERTIME					7
#define DEFAULT_RTPTIMEOUT					30
#define ENUM_MAX32  0x7fffffff
/* Stream Type */
typedef enum
{
	ST_UNKNOW,
	ST_ON_DEMAND,
	ST_PROGRESSIVE,
	ST_LOCAL,
	ST_LIVE,
	ST_MAX=ENUM_MAX32
} STREAM_TYPE;

#ifndef VOS_MAX_PATH
#define VOS_MAX_PATH          260
#endif

#if defined(_WIN32)||defined(WIN32)
#define VO_STREAMING_API __cdecl
#else
#define VO_STREAMING_API
typedef char TCHAR;
#endif
typedef struct tagDigestAuthParam
{
	char * user;
	unsigned int userLen;

	char * passwd;
	unsigned int passwdLen;
} DigestAuthParam, *PDigestAuthParam;

typedef enum 
{
	VO_SOURCE_STREAMING_SOCKET_USAGE_HTTP = 0x00000001,
	VO_SOURCE_STREAMING_SOCKET_USAGE_RTSP = 0x00000002,
}VO_SOURCE_STREAMING_SOCKET_USAGE;

typedef struct  
{
	VO_PTR pUserData;
	VO_U32 (VO_API * RegSocket)( VO_PTR pUserData , VO_S32 nSocket , VO_SOURCE_STREAMING_SOCKET_USAGE nUsage );
	VO_U32 (VO_API * UnRegSocket)( VO_PTR pUserData , VO_S32 nSocket );
}VO_SOURCE_STREAMING_SOCKETREG;

typedef struct  
{
	VO_PTR pUserData;
	VO_BOOL (VO_API * CheckCanUseProxy)( VO_PTR pUserData , VO_CHAR * pUrl );
}VO_SOURCE_STREAMING_PROXYCHECK;

/***************************************
* Initialized Parameters *
***************************************/
typedef struct tagVOStreamInitParam
{
	int		nMinUdpPort;	// default 1024
	int		nMaxUdpPort;	// default 65535
	int		nBufferTime;	// <S> default 7s
	int		nRTPTimeOut;	// <S> default 30s
	int		nSDKlogflag;	// 0: disable, 1: level1, 2: level2, 3: level3, default 0
	int		nUseTCP;		// 0:automatically try UDP first then TCP,1:force to use TCP,-1:disable TCP, default 0
	int		bUseRTSPProxy;	//default 0
	char	szProxyIP[VOS_MAX_PATH];//if bUseRTSPProxy==1,set it as a proper proxy IP=address:port
	int		nRTCPInterval;		 //default 5000 MS
	int		nRTSPTimeout;		//default 30s
	int		nConnectTimeout;	//default 30s
	char	szUserAgent[VOS_MAX_PATH];//default "VisualOn Streaming Player2.0"
	char	szProfile[VOS_MAX_PATH];//default '\0'
	int		nBandWidth;		 // default 0
	int		nExtraRTPPacket; // default 1
	int		nFirstIFrame;	 // default 1
	int		nEnable3GPPRateAdaptive;	//default 0
	int		nBufferLow;		//default 0s,when the buffer time is lower than the value,rebuffering will be triggered
	int		bUseExtraFields;//set it as 1 to use extra fields, otherwise 0
	char* 	szExtraFields;
	PDigestAuthParam	digestAuth;
	int		nReserved;
	char*	szProxyBlacklist;
	VO_SOURCE_STREAMING_SOCKETREG * pSocketReg;
	VO_SOURCE_STREAMING_PROXYCHECK * pProxyCheck;
} VOStreamInitParam, *PVOStreamInitParam;

typedef struct tagRTSPFieldParam
{
	unsigned int uMethods;
	char * _field;
} RTSPFieldParam, *PRTSPFieldParam;

typedef struct tagVOPDInitParam
{
	int nBufferTime;                    // unit: sec									default 5
	int nPlayBufferTime;                // unit: sec									default 90
	int nHTTPDataTimeOut;               // unit:MS										default 15000
	int nBandWidth;						//												default 0
	int nSDKlogflag;                    // 0: disable, 1: level1, 2: level2, 3: level3  default 0
	int nTempFileLocation;              // 0: memory, 1: storage, 2: ext.storage		default 1
	int nMaxBuffer;                     // unit: KB										default 5120
	int nPacketLength;					// unit: KB										defualt 5
	int nHttpProtocol;					// 0: http 1.0, 1: http 1.1						default 0
	unsigned int unSupportCodecType;	//												default 0
	unsigned char mUserAgent[VOS_MAX_PATH]; //default "VisualOn PD Player"
	TCHAR mFilePath[VOS_MAX_PATH];			//if nTempFileLocation,set the path and file name of tempFile,like /tempPD.mp4
	TCHAR mProxyName[VOS_MAX_PATH];			//if proxy is needed,set the proxy name,otherwise,set it as null string
	int nSeekUnsupported;				// 0: support seek; 1: block seek				default 0
	char * ptr_cookie;
	char*	szProxyBlacklist;
	VO_SOURCE_STREAMING_SOCKETREG * pSocketReg;
	VO_SOURCE_STREAMING_PROXYCHECK * pProxyCheck;
} VOPDInitParam, *PVOPDInitParam;

typedef struct tagVORTCPReceiverReport
{
    unsigned int senderSSRC;
    unsigned int sourceSSRC;

    unsigned int fractionLost;
    unsigned int cumulativeLost;

    unsigned int extendedHighestSeqReceived;
    unsigned int interarrivalJitter;

    unsigned int lastSR;
    unsigned int delayLastSR;
} VORTCPReceiverReport, *PVORTCPReceiverReport;

typedef struct tagVORTCPSenderReport
{
	unsigned int packCnt;
	unsigned int octetCnt;
	
    unsigned int fractionLost;
    unsigned int cumulativeLost;

    unsigned int extendedHighestSeqReceived;
    unsigned int interarrivalJitter;

    unsigned int lastSR;
    unsigned int delayLastSR;
} VORTCPSenderReport, *PVORTCPSenderReport;

/***************************************
* Common SDK errors code*
***************************************/
#define E_USER_ERROR_BASE  			(-1000)
#define E_INVALID_HANDLE			(E_USER_ERROR_BASE - 1)
#define E_NULL_PARAM				(E_USER_ERROR_BASE - 2)
#define E_INVALID_STATE				(E_USER_ERROR_BASE - 3)
#define E_NULL_CALLBACK				(E_USER_ERROR_BASE - 4)
#define E_UNSUPPORTED				(E_USER_ERROR_BASE - 5)
#define E_FILE_OPEN_FAIL			(E_USER_ERROR_BASE - 6)
#define E_FILE_READ_FAIL			(E_USER_ERROR_BASE - 7)
#define E_FILE_WRITE_FAIL			(E_USER_ERROR_BASE - 8)
#define E_NO_AUDIO_FRAMEOUT			(E_USER_ERROR_BASE - 9)
#define E_NO_VIDEO_FRAMEOUT			(E_USER_ERROR_BASE - 10)
#define E_CODEC_CREATION_FAIL		(E_USER_ERROR_BASE - 11)
#define E_CORRUPT_MEDIA				(E_USER_ERROR_BASE - 12)
#define E_SMALL_BUFFER				(E_USER_ERROR_BASE - 13)
#define E_CDI_FAILURE				(E_USER_ERROR_BASE - 14)
#define	E_THREAD_CREATE_FAIL		(E_USER_ERROR_BASE - 15)
#define	E_QUEUE_CREATE_FAIL			(E_USER_ERROR_BASE - 16)
#define	E_EVENT_CREATE_FAIL			(E_USER_ERROR_BASE - 17)
#define E_CLOCK_CREATE_FAIL			(E_USER_ERROR_BASE - 18)
#define E_INVALID_URL				(E_USER_ERROR_BASE - 19)
#define E_PARSER_FAILED				(E_USER_ERROR_BASE - 20)
#define E_CONNECTION_FAILED			(E_USER_ERROR_BASE - 21)
#define E_HOST_UNKNOWN				(E_USER_ERROR_BASE - 22)
#define E_NETWORK_INIT_ERROR		(E_USER_ERROR_BASE - 23)
#define E_DUPLICATE_TRANS_ID		(E_USER_ERROR_BASE - 24)
#define E_INVALID_TRANS_ID			(E_USER_ERROR_BASE - 25)
#define E_UNCLASSIFIED_ERROR		(E_USER_ERROR_BASE - 26)
#define E_CONNECTION_LOST			(E_USER_ERROR_BASE - 27)
#define E_INFINITE_REDIRECTS		(E_USER_ERROR_BASE - 28)
#define E_NETWORK_ERROR				(E_USER_ERROR_BASE - 29)
#define E_UNSUPPORTED_FORMAT		(E_USER_ERROR_BASE - 30)
#define E_INFO_NOT_AVAILABLE		(E_USER_ERROR_BASE - 31)
#define E_DATA_INACTIVITY			(E_USER_ERROR_BASE - 32)
#define E_UDP_BLOCKED				(E_USER_ERROR_BASE - 33)
#define E_RTSP_BAD_RESPONSE			(E_USER_ERROR_BASE - 34)
#define E_PORT_ALLOC				(E_USER_ERROR_BASE - 35)
#define E_SDP_PARSE_FAILED			(E_USER_ERROR_BASE - 36)
#define E_SDP_FILE_OPEN_FAILED		(E_USER_ERROR_BASE - 37)
#define E_NULL_SDP					(E_USER_ERROR_BASE - 38)
#define E_DATA_GET_ERROR			(E_USER_ERROR_BASE - 39)
#define E_NETWORK_TIMEOUT			(E_USER_ERROR_BASE - 40)
#define E_CONNECTION_RESET			(E_USER_ERROR_BASE - 41)
#define E_UNSUPPORTED_CODEC			(E_USER_ERROR_BASE - 42)
#define E_VIDEO_FRAME_DROPPED		(E_USER_ERROR_BASE - 43)
#define E_RTSP_SEND_FAILED			(E_USER_ERROR_BASE - 44)
#define E_HTTP_RESP_FAILED			(E_USER_ERROR_BASE - 45)
#define E_NW_BW_NOT_SUFFICIENT		(E_USER_ERROR_BASE - 46)
#define E_RES_NOT_SUFFICIENT		(E_USER_ERROR_BASE - 47)
#define E_INCONSISTANT_BUF_LEVEL_VID (E_USER_ERROR_BASE - 48)
#define E_INCONSISTANT_BUF_LEVEL_AUD (E_USER_ERROR_BASE - 49)
#define E_HARDWARE_FAILURE			(E_USER_ERROR_BASE - 50)
#define E_RTSP_REDIRECT_RESPONSE	(E_USER_ERROR_BASE - 51)

/***************************************
* Common Server errors code*
***************************************/
#define E_PROXY_NEEDED				(E_USER_ERROR_BASE - 305)
#define E_BAD_REQUEST				(E_USER_ERROR_BASE - 400)
#define	E_UNAUTHORISED				(E_USER_ERROR_BASE - 401)
#define	E_PAYMENT_REQUIRED			(E_USER_ERROR_BASE - 402)
#define	E_FORBIDDEN					(E_USER_ERROR_BASE - 403)
#define	E_NOT_FOUND					(E_USER_ERROR_BASE - 404)
#define	E_METHOD_NOT_ALLOWED		(E_USER_ERROR_BASE - 405)
#define	E_NOT_ACCEPTABLE			(E_USER_ERROR_BASE - 406)
#define	E_PROXY_AUTH_REQUIRED		(E_USER_ERROR_BASE - 407)
#define	E_REQUEST_TIMEOUT			(E_USER_ERROR_BASE - 408)
#define E_CONFLICT					(E_USER_ERROR_BASE - 409)
#define E_GONE						(E_USER_ERROR_BASE - 410)
#define E_LENGTH_REQUIRED			(E_USER_ERROR_BASE - 411)
#define E_PRECONDITION_FAILED		(E_USER_ERROR_BASE - 412)
#define E_REQUEST_ENTITY_TOO_LARGE	(E_USER_ERROR_BASE - 413)
#define E_REQUEST_URI_TOO_LONG		(E_USER_ERROR_BASE - 414)
#define E_UNSUPPORTED_MEDIA_TYPE	(E_USER_ERROR_BASE - 415)
#define E_RANGE_OVERFLOW			(E_USER_ERROR_BASE - 416)
#define E_EXPECTATION_FAILED		(E_USER_ERROR_BASE - 417)
#define E_PARAMETER_NOT_UNDERSTOOD	(E_USER_ERROR_BASE - 451)
#define E_CONFERENCE_NOT_FOUND		(E_USER_ERROR_BASE - 452)
#define E_NOT_ENOUGH_BANDWIDTH		(E_USER_ERROR_BASE - 453)
#define E_SESSION_NOT_FOUND			(E_USER_ERROR_BASE - 454)
#define E_METHOD_INVALID			(E_USER_ERROR_BASE - 455)
#define E_HEADER_INVALID			(E_USER_ERROR_BASE - 456)
#define E_RANGE_INVALID				(E_USER_ERROR_BASE - 457)
#define E_PARAMETER_READ_ONLY		(E_USER_ERROR_BASE - 458)
#define E_AGGR_OP_NOT_ALLOWED		(E_USER_ERROR_BASE - 459)
#define E_ONLY_AGGR_OP_ALLOWED		(E_USER_ERROR_BASE - 460)
#define E_UNSUPPORTED_TRANSPORT		(E_USER_ERROR_BASE - 461)
#define E_DESTINATION_UNREACHABLE	(E_USER_ERROR_BASE - 462)
#define E_INTERNAL_SERVER_ERROR		(E_USER_ERROR_BASE - 500)
#define E_NOT_IMPLEMENTED			(E_USER_ERROR_BASE - 501)
#define E_BAD_GATEWAY				(E_USER_ERROR_BASE - 502)
#define E_SERVICE_UNAVAILABLE		(E_USER_ERROR_BASE - 503)
#define E_GATEWAY_TIMEOUT			(E_USER_ERROR_BASE - 504)
#define E_VERSION_NOT_SUPPORTED		(E_USER_ERROR_BASE - 505)
#define E_OPTION_NOT_SUPPORTED		(E_USER_ERROR_BASE - 551)
#define E_CONNECTION_GOODBYE	 (E_USER_ERROR_BASE - 600)
/***************************************
* SDK specific  error base
**************************************/
#define E_SDK_ERROR_BASE			(E_USER_ERROR_BASE - 1000)

/***************************************
* Sub Module specific  error base
**************************************/
#define E_MP_ERRORBASE				(E_SDK_ERROR_BASE - 1000) /* Mp4 Player */
#define E_MR_ERRORBASE				(E_SDK_ERROR_BASE - 2000) /* Mp4 Recorder */
#define E_ST_ERRORBASE				(E_SDK_ERROR_BASE - 3000) /* Streaming Player */
#define E_IV_ERRORBASE				(E_SDK_ERROR_BASE - 4000) /* Image Viewer*/
#define E_TR_ERRORBASE				(E_SDK_ERROR_BASE - 5000) /* Transition*/
#define E_HTCL_ERRORBASE			(E_SDK_ERROR_BASE - 6000) /* HTTP Stk Error Base */
#define E_STMR_ERRORBASE			(E_SDK_ERROR_BASE - 7000) /* State Mgr Error Base */
#define E_LSTM_ERRORBASE			(E_SDK_ERROR_BASE - 8000) /* List Mgr Error Base */
#define E_CDI_ERRORBASE				(E_SDK_ERROR_BASE - 9000) /* CDI Error Base */
#define E_CM_ERRORBASE				(E_SDK_ERROR_BASE - 10000) /* CM Error Base */
#define E_NT_ERRORBASE				(E_SDK_ERROR_BASE - 11000) /* NT Error Base */
#define E_ISDK_ERRORBASE			(E_SDK_ERROR_BASE - 12000) /* Integrated PD/RTSP Strm/StoredPB */

/***************************************
* Http PD module error code
****************************************/
#define E_PD_MEDIATYPE_UNKNOWN			(E_ISDK_ERRORBASE - 500) // PD file format check,but type is unknown 
#define E_PD_FILE_PARSER_LIB_LOADFAIL	(E_ISDK_ERRORBASE - 501) // PD Load file parser so file but failed 
#define E_PD_PARAM_MISS					(E_ISDK_ERRORBASE - 502) // PD Param are supposed from player but missed 
#define E_PD_SET_DOWNLOAD_URL_FAILED	(E_ISDK_ERRORBASE - 503) // PD set download url failed
#define E_PD_DOWNLOAD_FAILED			(E_ISDK_ERRORBASE - 504) // PD download failed! 
#define E_PD_FILE_PARSER_OPEN_FAILED	(E_ISDK_ERRORBASE - 505) // PD file parser open failed
#define E_PD_SERVER_FILE_NOT_EXIST		(E_ISDK_ERRORBASE - 506) // PD request data but server-side response 4xx error, file not exist 
#define E_PD_SERVER_DCON_ON_PREPARE		(E_ISDK_ERRORBASE - 507) // server disconnected on prepare  
#define E_PD_SERVER_DCON_ON_PLAY		(E_ISDK_ERRORBASE - 508) // server disconnected on playing


/***************************************
* MediaType definition *
***************************************/
typedef enum
{
	VOMediaTypeUnknown   = 0x0000,
	VOMediaTypeMP4		 = 0x0001,
	VOMediaTypeAudio     = 0x0002,
	VOMediaTypeReal		 = 0x0003,
	VOMediaTypeAsf		 = 0x0004,
	VOMediaTypeFLV		 = 0x0005,
	VOMediaType_MAX=ENUM_MAX32
} VOMediaType;

/***************************************
* CodeType definition *
***************************************/
typedef enum
{
	VOCodecTypeUnknown   = 0x0000,
	VOCodecTypeMPEG4SP   = 0x0001,
	VOCodecTypeH263BL    = 0x0002,
	VOCodecTypeGSMAMRNB  = 0x0004,
	VOCodecTypeMPEG4AAC  = 0x0008,
	VOCodecTypeH264BL    = 0x0010,
	VOCodecTypeQCELP     = 0x0020,
	VOCodecTypeGSMAMRWB  = 0x0040,
	VOCodecTypeEVRC      = 0x0080,
	VOCodecTypeSMV       = 0x0100,
	VOCodecType_MAX=ENUM_MAX32
} VOCodecType;

/***************************************
* Streaming info from SourceFilter *
***************************************/
typedef struct tagVOStreamingInfo
{
	char*			clip_title;
	VOCodecType		codecType[2];			//index 0:video,index 1:audio
	STREAM_TYPE		clipType;
	int				clip_bitrate;
	int             clip_width;
	int             clip_height;
	int				clip_SupportPAUSE;      //1:support 0:not support
} VOStreamingInfo, *PVOStreamingInfo;

typedef struct
{
	VO_U32			size;				// the size of the media track <KByte>
	VO_U32			packet_received;	// the cumulative number of RTP media packets received in this media track during this session
	VO_U32			packet_duplicated;	// the cumulative number of RTP media packets previously received in this media track during this session
	VO_U32			packet_lost;		// the cumulative number of RTP media packets lost for this media track type
	VO_U32			packet_sent;		// the cumulative number of RTP media packets sent in this media track during this session
	VO_U32			average_jitter;		// the mean relative transit time between each two RTP packets of this media track type throughout the duration of the media session
	VO_U32			average_latency;	// average accumulating latency values of all RTP media packets transmitted from the server to the client, !!now only reserved!!
	VO_U8			codec;				// codec type for the media track. (iq_sdp_media_codec_t)
} VORTSPMediaStats;

typedef struct
{
	VO_U16			error_code;			// 200-no error, the protocol error code(describe, setup, play fails)
	VO_U8			track_number;		// number of media tracks in the session and the number of elements in tTrackInfo array
	VORTSPMediaStats*	track_stats;	// statistical information from each media track streamed during the session
} VORTSPSessionEndInfo;

/***************************************
* EventCode *
SourceFilter will call NotifyEvent(EventCode,ErrorCode,0)
if some events happen.Please refer the ErrorCode defined above
and EventCode defined below.
***************************************/
enum
{
	//RTSP EVENT
	VO_EVENT_CONNECT_FAIL		= 3000,
	VO_EVENT_DESCRIBE_FAIL		= 3001,
	VO_EVENT_SETUP_FAIL			= 3002,
	VO_EVENT_PLAY_FAIL			= 3003,
	VO_EVENT_PAUSE_FAIL			= 3004,
	VO_EVENT_OPTION_FAIL		= 3005,

	//RTP EVENT
	VO_EVENT_SOCKET_ERR			= 3006,

	//BUFFER EVENT
	VO_EVENT_BUFFERING_BEGIN	= 3007,
	VO_EVENT_BUFFERING_END		= 3008,
	VO_EVENT_BUFFERING_PERCENT	= 3100,
	VO_EVENT_BUFFERING_FAIL		= 3101,


	//Other event
	VO_EVENT_DISABLE_SCAN		= 5000,
	VO_EVENT_FLUSH_BUFFER		= 5001,
	VO_EVENT_PACKET_LOST		= 5012,
	VO_EVENT_TIMEMARKER			= 5020,	// if network get data reach time marker, notify it.

	//Session event
	VO_RTSP_EVENT_SESSION_START			= 5030,	// char*, string containing the complete URL for the multimedia request, as transmitted, including protocol identifier and host name/port
	VO_RTSP_EVENT_SESSION_END			= 5031,	// VORTSPSessionEndInfo*
	VO_RTSP_EVENT_SESSION_RECVSTART		= 5032,	// the first RTP packet for one of the media tracks in the session is placed in the playback jitter buffer
	VO_RTSP_EVENT_SESSION_RECVFINISH	= 5033,	// all RTP packets for the media session have been placed into the playback jitter buffer
	VO_RTSP_EVENT_SESSION_TEARDOWN		= 5034,	// event marking the beginning of the tear down process to end the media playback session and release the connection to the media server

	//PD EVENT
	VO_PD_EVENT_INSUFFICIENT_SPACE		= 0x4000, 
	VO_PD_EVENT_SERVER_WRONGRESPONSE	= 0x4001, 
	VO_PD_EVENT_GET_ASX					= 0x4002,	//when receive the event,the param1 is the pointer to the ASX buffer

	VO_PD_EVENT_DURATIONMAYWRONG		= 0x4020, 
	VO_PD_ENENT_NOTGOODFORSTREAMING		= 0x4021, 
	VO_PD_EVENT_DOWNLOADPOS				= 0x4022, 
    VO_PD_EVENT_SEEKFINALPOS            = 0x4023,   //VO_S64* ,the real timestamp after async setpos
	VO_PD_EVENT_NOT_APPLICABLE_MEDIA	= 0x4024,	//The requested operation is not applicable to the current media type.

	//RTCP EVENT
	VO_EVENT_RTCP_RR					= 0x5000,	// parameter is VORTCPReceiverReport*

	VO_EVENT_STREAMING_MAX		= ENUM_MAX32
};

typedef enum
{
	VO_QUICKRESPONSE_RUN, 
	VO_QUICKRESPONSE_PAUSE, 
	VO_QUICKRESPONSE_STOP, 
	VO_QUICKRESPONSE_FLUSH, 
	VO_QUICKRESPONSE_RETURNBUFFER, 
	VO_QUICKRESPONSE_PAUSE_MANUAL,
} VO_QUICKRESPONSE_COMMAND;

/***************************************
* Parameter ID for IVORTSPSource/IVOPDSource interface
Please refer the definition of IVORTSPSource/IVOPDSource
***************************************/
enum
{
	//GetParam(ID,value);
	VOID_STREAMING_INFO						= 0x37ff,	/*!<the parameter is a pointer of VOStreamingInfo*/
	VOID_STREAMING_BUFFERING_PROGRESS		= 0x3801,	/*!< the parameter is a LONG integer [0..100] */
	VOID_STREAMING_DOWNLOADING_TIME			= 0x3802,	/*!<the parameter is a integer [ms] */

	//SetParam(ID,value);
	VOID_STREAMING_INIT_PARAM				= 0x4001,	/*!< the parameter is a pointer of VOStreamInitParam/VOPDInitParam*/
	VOID_STREAMING_FORCE_STOP				= 0x4002,	/*!< the parameter is a bool*/
	VOID_STREAMING_BUFFERING_PAUSE			= 0x4005,	/*!< the parameter is a bool*/
	VOID_STREAMING_ADD_RTSP_FIELD			= 0x4006,	/*!< the parameter is a pointer of struct RTSPFieldParam */
	VOID_STREAMING_PLAYLIST_PLAY			= 0x4007,	/*!< the parameter is the playlist string */
	VOID_STREAMING_ADD_CUSTOM_NOTIFY_SINK	= 0x4008, 	/*!< the parameter is a pointer of ICustomNotifyInterface interface */
	VOID_STREAMING_PAUSE_DOWNLOAD			= 0x4009,	/*!< the parameter is a bool */
	VOID_STREAMING_THROUGHPUT				= 0x400A,	/*!< the parameter is throughput threshold (kbytes) */
	VOID_STREAMING_LOAD_CALLBACK			= 0x400B,	/*!< the parameter is load callback function pointer*/
	VOID_STREAMING_MEDIA_TYPE				= 0x400C,	/*!< the parameter is VOMediaTypeXXX, must set before load*/
	VOID_STREAMING_FORCE_RUN				= 0x400D,	/*!< the parameter is boolean*/
	VOID_STREAMING_SDK_VERSION				= 0x400E,	/*!< the parameter is long,currently only [0,1],0 is default*/
	VOID_STREAMING_OPENURL					= 0x400F,	/*!< the parameter is a pointer, is default*/
	VOID_STREAMING_AUTO_EXTEND_BUFTIME		= 0x4010,	/*!< the parameter is long,currently only [0,1],0 is default*/
	VOID_STREAMING_QUICKRESPONSE_COMMAND	= 0x4020,	/*!< the parameter is VO_QUICKRESPONSE_COMMAND* */
	VOID_STREAMING_INITPLAY_POS				= 0x4100,	/*!< the parameter is long,million second,the init play position*/
	VOID_STREAMING_TIMEMARKER				= 0x4101,	/*!< the parameter is long, time(second), default 10s, please refer VO_EVENT_TIMEMARKER */
	VOID_STREAMING_MINIMUM_LATENCY			= 0x4102,	/*!< the parameter has no parameter, it will ask streaming to minimum latency  */
	VOID_STREAMING_DIGEST_AUTHEN			= 0x4103,	/*!< the parameter is a PDigestAuthParam *>*/
	VOID_STREAMING_LIBOP					= 0x4104,	/*!< the parameter is used to set lib op */
	VOID_HTTP_ADD_HEADER					= 0x4105,	/*!< the parameter is the HTTP request header string, Giving or updating header name: value */
	VOID_HTTP_REMOVE_HEADER					= 0x4106,   /*!< the parameter is the HTTP request header string, Delete given name of header */
	VOID_HTTP_REQUEST_OPTION_KEEPCONNECTION_ON_PLAY = 0x4107, /*!< the parameter is unsigned long,currently only [0,1],0 is default*/
	VOID_HTTP_REQUEST_OPTION_KEEPCONNECTION_ON_PAUSE = 0x4108, /*!< the parameter is unsigned long,currently only [0,1],1 is default*/
	VOID_HTTP_REQUEST_OPTION_ENABLE_HTTPRANGE = 0x4109, /*!< the parameter is unsigned long,currently only [0,1],1 is default*/
	VOID_HTTP_REQUEST_OPTION_CONNECTION_TIMEOUT = 0x4110, /*!< the parameter is unsigned long ,microseconds */
	VOID_HTTP_REQUEST_OPTION_READ_TIMEOUT = 0x4111, /*!< the parameter is unsigned long ,microseconds*/
	VOID_HTTP_GET_RESPONSE	= 0x4112,  /*!< the parameter is char* ,we will copy response string to it*/
	VOID_STREAMING_PDCONNECTRETRY_TIMES		= 0x4120,	/*!< the parameter is long,socket connect failed and retry times*/
	VOID_STREAMING_PARAM_MAX				= ENUM_MAX32
};

// return true to make Load() continuously run, or false to make Load() return E_NOT_ENOUGH_BANDWIDTH immediately
// nClipBitrate, nNetThroughput <Bytes/Second>



typedef bool (VO_STREAMING_API* AUTOSELECTCALLBACK)(int nClipBitrate, int nNetThroughput);
typedef int (VO_STREAMING_API* STREAMING_CALLBACK_NOTIFY) (long EventCode, long * EventParam1, long * userData);

typedef struct  
{
	STREAMING_CALLBACK_NOTIFY funtcion;
	void* userdata;
}StreamingNotifyEventFunc;

#ifdef __cplusplus
}
#endif

#endif	//__voStreaming_H__
