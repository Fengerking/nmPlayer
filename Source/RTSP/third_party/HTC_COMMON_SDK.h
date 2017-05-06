#pragma once
#define STATIC_LIB
#ifndef STATIC_LIB
#ifdef STREAMINGCUSTOM_EXPORTS
#define STREAMINGCUSTOMAPI __declspec(dllexport)
#else
#define STREAMINGCUSTOMAPI __declspec(dllimport) 
#endif
#else
#define STREAMINGCUSTOMAPI 
#endif

/***************************************
 * Common Definition *
 ***************************************/

#define MAX_PROFILE_SIZE					0x04
#define MAX_SUPPORTED_FRAME_SIZE			0x0A
#define PORT_MIN							1024
#define PORT_MAX							65535
#define PORT_RANGE_MIN						20
#define MIN_BUFFER_TIME						4		// 4 sec
#define MAX_BUFFER_TIME						25		// 100 sec
#define DEFAULT_BUFFER_TIME					7		// 7 sec

#define DEFAULT_RTP_TIMEOUT					30000       
#define DEFAULT_RTSP_TIMEOUT				30000	// 30 seconds
#define DEFAULT_RTCP_KEEPALIVE              2500 

#define	APN_CONNECT_TIMEOUT				    30000	// 
#define FIND_NETWORK_TIMEOUT				600

#define BUFFER_LEADTIME						10		// second

#define NW_STATUS_CONNECT					(0x0001)
#define NW_STATUS_DISCONNECT				(0x0008)

/* Clip Type */

/* Clip Type */
enum CLIP_TYPE {UNKNOW, ON_DEMAND, PROGRESSIVE, LOCAL, LIVE};	

typedef struct 
{
	unsigned short int mFrameWidth;
	unsigned short int mFrameHeight;
	unsigned short int mFrameRate;
} tFrameSize;

typedef enum 
{
	kNetworkGPRS	= 64,
	kNetworkEDGE	= 128,
	kNetwork3G		= 256,
	kNetworkWiFi	= 1024,
	kNetworkUnknown	= 2048,
} eNetworkType;

typedef enum RTPBasedOnProtocol
{
	kBasedOnUDP,
	kBasedOnTCP,
	kBasedOnUDPTCP
}eRTPBasedProtocol;

typedef struct
{
	eNetworkType mNetWorkType;
	tFrameSize   mFrameSize[MAX_SUPPORTED_FRAME_SIZE];
}tClipRejInfo;
#ifndef MAX_PATH
#define MAX_PATH          260
#endif
typedef struct
{
	unsigned char  nNoOfProfile;
	unsigned char mProfile[MAX_PATH];
}tDeviceProfile;

/***************************************
 * An example for tDeviceProfile:      *
 * nNoOfProfile = 1 *
 * mpProfile = http://www.htcmms.com.tw/cingular/Hermes-1.0.xml *
 ***************************************/

/* Link rate Characteristic */
typedef struct
{
	unsigned int mGBR;	   /* link layer Guaranteed Bit rate */
	unsigned int mMBR;	   /* link layer Maximum Bit rate */
	unsigned int mMaxDelay; /* maximum possible transmission delay */
} tFrmmLinkChart;

/***************************************
 * Initialized Parameters *
 ***************************************/

struct StreamInitParamAdapter {
	int nMinUdpPort;
	int nMaxUdpPort;
	int nBufferTime;
	int nRTSPTimeOut;
	int nRTPTimeOut;
	int nReceiverReportInterval;
	int nBandWidth;
	int nExtraRTPPacket; // for Port Opener.
	int nSDKlogflag;   // 0: disable, 1: level1, 2: level2, 3: level3
	int nForceLiveSupportPAUSE; // force Live Support Pause.
	unsigned int unSupportCodecType;
	unsigned char mUserAgent[MAX_PATH];
	tClipRejInfo   mClipRejInfo;
	tDeviceProfile mDeviceProfile;
	tFrmmLinkChart LnkChart; // for 3GPP-Link-CHAR.

	int bEnablePlaylistSupport; // for PlaylistSupport
	int bDisableAVSyncNotification;//For quality control
	int bOpenOriginalUDPPorts;//Some proxies would change the UDP ports which client sends to server in SETUP
	int bOpenUAProEachRequest;//Some Customer hope that UAProfile can be included in each request
	int bUseRTSPProxy;
	char szProxyIP[MAX_PATH];
	int	nUseTCP;
	int	  nWaitForTeardown;
	int   nEnableNullAudFrameWhenPacketloss;
	int	  nForceFromKeyFrm;

	int nTimeMarker;   /*this marker is used to notify AP that we have received packets for more than nTimeMarker seconds.
					  Then AP can get throughput and bitrate to decide if the bandwidth is too low for this session*/
};

/***************************************
 * Common SDK errors *
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
 * Common Server errors *
 ***************************************/
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
 * CodeType definition *
 ***************************************/
typedef enum CodecType
{
	kCodecTypeUnknown   = 0x0000,
	kCodecTypeMPEG4SP   = 0x0001,
	kCodecTypeH263BL    = 0x0002,
	kCodecTypeGSMAMRNB  = 0x0004,
	kCodecTypeMPEG4AAC  = 0x0008,
	kCodecTypeH264BL    = 0x0010,
	kCodecTypeQCELP     = 0x0020,
	kCodecTypeGSMAMRWB  = 0x0040,
	kCodecTypeEVRC      = 0x0080,
	kCodecTypeSMV       = 0x0100,
}eCodecType;





/********************************************************
The following definitions are provided by VisualOn,Inc. 
Number Huang
2007.06.21
**********************************************************/

/***************************************
* Streaming info from SourceFilter *
***************************************/

typedef struct{
	char*			clip_title;
	eCodecType		codecType[2];//index 0:video,index 1:audio
	enum CLIP_TYPE		clipType;
	int			clip_bitrate;
	int             	clip_width;
	int             	clip_height;
	int				clip_SupportPAUSE; //1:support 0:not support
	int             clip_AvgFrameRate;
	long            clip_DownloadedSize;
	int             clip_audioSampleRate;
	int             clip_audioChannels;
	int             clip_audioBitRate;
	int             clip_videoBitRate;
}HS_StreamingInfo;

enum
{
	//RTSP EVENT
	HS_EVENT_CONNECT_FAIL		= 3000,
	HS_EVENT_DESCRIBE_FAIL		= 3001,
	HS_EVENT_SETUP_FAIL		= 3002,
	HS_EVENT_PLAY_FAIL		= 3003,
	HS_EVENT_PAUSE_FAIL		= 3004,
	HS_EVENT_OPTION_FAIL		= 3005,

	//RTP EVENT
	HS_EVENT_SOCKET_ERR		= 3006,

	//BUFFER EVENT
	HS_EVENT_BUFFERING_BEGIN	= 3007,
	HS_EVENT_BUFFERING_END		= 3008,

	//Support Server session
	HS_EVENT_PLAY_SEEK,


	//Other event
	HS_EVENT_DISABLE_SCAN		= 5000,
	HS_EVENT_FLUSH_BUFFER		= 5001,
	HS_EVENT_GET_REFCLOCK		= 5002,

	//HTC API EVENT
	HS_EVENT_INITNETWORK_FAIL	= 5100,
	HS_EVENT_GETINITPARAM_FAIL	= 5101,


	
};
/*

*/
enum
{
	//GetParam(ID,value);
	HSID_STREAMING_INFO							= 0x07ff,/*!<the parameter is a pointer of HS_StreamingInfo*/
	HSID_STREAMING_BUFFERING_PROGRESS			= 0x0800,/*!< the parameter is a LONG integer [0..100] */	
	HSID_STREAMING_GET_RECEIVING_DATA_RATE		= 0x0801,

	HSID_STREAMING_CURRENT_POS			= 0x1801,/*!< the parameter is a LONG integer  */

	//SetParam(ID,value);
	HSID_STREAMING_INIT_PARAM			= 0x1000,/*!< the parameter is a pointer of  StreamInitParam,it is only for inner use of VisualOn currently*/	
	HSID_STREAMING_FORCE_STOP			= 0x1001,/*!< the parameter is a bool*/
	HSID_STREAMING_SEND_PAUSE_FIRST		= 0x1002,
	HSID_STREAMING_BUFFERING_PAUSE		= 0x1003,/*!< the parameter is a bool*/
	HSID_STREAMING_FORCE_PAUSE			= 0x2005,/*!< the parameter is a bool*/
	HSID_STREAMING_DEBUG_MODE			= 0x2006,/*!< the parameter is a bool,internal use of VO*/

};

enum
{
	VOID_DEBUG_TEARDOWN			= 1,
	VOID_DEBUG_PAUSEFAIL		= 2,
};