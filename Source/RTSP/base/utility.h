#ifndef __BASEUTILITY_H__
#define __BASEUTILITY_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macro.h"
#include "voRTSPSDK.h"
#include "HTC_COMMON_SDK.h"
#include "rtsptvparam.h"
#include "list_T.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

using namespace vo_cross_platform_streaming;

#ifdef LINUX
#define __cdecl
#endif//LINUX

#define MAX_LOG_STRING_LEN 8192

#undef LOG_TAG
#define  LOG_TAG "VORTSP"

#if MEMORY_TRACE
void trace_new(char* name,int address);
void trace_delete(int address);
void trace_memLeak();
#define TRACE_NEW(name,address) {trace_new((char*)(name),int(address));}
#define TRACE_DELETE(address)	{trace_delete(int(address));}
#else //MEMORY_TRACE
#define TRACE_NEW(name,address)
#define TRACE_DELETE(address)
#define trace_memLeak()
#endif //MEMORY_TRACE

int EncodeBase64(char * pInput, char * pOutput);
int voBaseSixFourDecodeEx(unsigned char *src, unsigned char *dst, int *dlen);
unsigned char * voBaseSixFourDecode(char * in, unsigned int & resultSize, bool trimTrailingZeros = true);
unsigned int Random32();
typedef int (* __cdecl CALLBACKNOTIFY2) (long EventCode, long * EventParam1, long * EventParam2);

typedef struct  
{
	CALLBACKNOTIFY2 funtcion;
	void* parent;
}NotifyEventFunc;

//typedef void (*SetRTSPPacket)(unsigned char flag, unsigned int size, unsigned char *buf);	//doncy 0118
typedef void (*SetRTSPPacket)(unsigned char isTCP, unsigned char flag, unsigned int size, unsigned char *buf, unsigned short rtpSeqNum, unsigned int rtpTimestamp);

int  	Value(const char c);
void 	HexStrToBytes(const char * hexString, unsigned char ** buffer, int * bufLen);
void 	SpropParameterSetsData(const char * spropParamSets, unsigned char ** buffer, int * bufLen);
void 	ParseMPEG4LatmAudioFmtpConfig(const char * fmtpConfig, unsigned char ** buffer, int * bufLen);

//-------------------------------------------------------------

int 	IsH264IntraFrame(unsigned char * buffer, int /* bufLen */);
int 	IsMPEG4IntraFrame(unsigned char * buffer, int bufLen);
int 	IsH263IntraFrame(unsigned char * buffer, int bufLen);

//------------------  class CUserOptions ------------------

typedef enum{
	LL_NONE			= 0,
	LL_RTSP_MESSAGE	= 1,
	LL_VIDEO_TIME	= 1<<1,
	LL_AUDIO_TIME	= 1<<2,
	LL_RTSP_ERR		= 1<<3,
	LL_RTP_ERR		= 1<<5,
	LL_SDP_ERR		= 1<<7,
	LL_SOCKET_ERR	= 1<<9,
	LL_SOCKET_WARN  = 1<<10,
	LL_RTCP_ERR		= 1<<11,
	LL_CODEC_ERR	= 1<<13,
	LL_VIDEO_DATA	= 1<<14,
	LL_AUDIO_DATA	= 1<<15,
	LL_BUF_STATUS	= 1<<16,
	LL_VIDEO_SAMPLE = 1<<17,
	LL_AUDIO_SAMPLE = 1<<18,
	LL_ALL			= 0xffffffff,
}LOG_Level;

#define LOG_NONE 0
#define LOG_RTSP 1
#define LOG_RTP	 2
#define LOG_DATA 3

#define UT_RTSP_URL		0
#define UT_SDP_URL		1
#define UT_SDP2_LOCAL	2
#define UT_META_URL	    3

#define LOCAL_VIDEO_RTP  0
#define LOCAL_VIDEO_RTCP 1
#define LOCAL_AUDIO_RTP  2
#define LOCAL_AUDIO_RTCP 3

typedef enum{
	LT_FILE		=1,
	LT_STDOUT	=2,
}LOG_Target;

#ifdef DIMS_DATA
#define LOCAL_DIMS_RTP  4
#define LOCAL_DIMS_RTCP 5
#endif//DIMS_DATA

#define MAX_USER_INFO_SIZE 256

#define ST_3GPP 0
#define ST_REAL 1
#define ST_ASF	2

int NotifyEvent(int eventCode,int param);

typedef enum 
{
	Session_Shutdown = 0,
	Session_Paused,
	Session_NewPos,
	Session_Playing,
	Session_LivePaused,
	Session_PauseFail,
	Session_Start,
	Session_Seek_BeforeRun,
	Session_SwitchChannel,//only for MediaFlo
}Session_Status;

typedef enum
{
	VOS_CMD_UNKNOWN		=	0,
	VOS_CMD_DESCRIBE		,
	VOS_CMD_SETUP			,
	VOS_CMD_PAUSE			,
	VOS_CMD_PLAY			,
	VOS_CMD_TEARDOWN		,
	VOS_CMD_SET_PARAMETER		,
	VOS_CMD_GET_PARAMETER,
	VOS_CMD_OPTION,
	VOS_CMD_PLAY_LIST,
	VOS_CMD_KEEPALIVE,
	VOS_CMD_GET,
	VOS_CMD_POST,
}VOS_RTSP_CMD;

typedef enum
{
	VOS_STREAM_ALL		= 0xffffffff,
	VOS_STREAM_VIDEO	= 1,
	VOS_STREAM_AUDIO	= 1<<1,
	VOS_STREAM_VIDEO_RTCP = 1<<2,
	VOS_STREAM_AUDIO_RTCP = 1<<3,
	VOS_STREAM_RTSP		= 1<<10,
}VOS_RTSP_STREAMTYPE;

typedef struct
{
	unsigned int mGBR;	   /* link layer Guaranteed Bit rate */
	unsigned int mMBR;	   /* link layer Maximum Bit rate */
	unsigned int mMaxDelay; /* maximum possible transmission delay */
}T3GPPLinkChart;

class CUserOptions
{
public:
	static CUserOptions UserOptions;

public:
	CUserOptions(void);
	~CUserOptions(void);

public:
	bool 			LoadUserOptionsFromReg();
	int  			GetMediaBufferTime() { return m_nMediaBuffTime * 1000;}
	void 			GetUDPPortRange(int & nUDPPortFrom, int & nUDPPortTo);
	void 			SetSessionStatus(Session_Status status1){status=status1;}
	Session_Status 	GetSessionStatus(){return status;}
public:
	int 			m_nUDPPortFrom;
	int 			m_nUDPPortTo;
	int 			m_nMediaBuffTime;
	int 			m_nRTPTimeOut;
	int 			m_nRTSPTimeOut;
	int 			m_nConnectTimeOut;
	int 			m_nReceiverReportInterval;
	int 			m_nBandWidth;
	char 			m_sUserAgent[MAX_PATH];
	char 			m_sUAProfile[MAX_PATH];
	int 			m_nPortOpener;
	int 			m_nSupportPause;
	int 			m_nForceLivePause;
	int 			m_bMakeLog;
	int 			m_URLType;
	bool 			m_bEnablePlaylistSupport;
	int				m_timeoutCounter;
	bool 			m_exit;
	char* 			m_currURL;
	int 			sdpURLLength;
	int 			m_useTCP;
	HS_StreamingInfo outInfo;
	NotifyEventFunc notifyFunc;
	int				errorID;
	int 			eventCode;
	bool 			streamEnd;
	int  			debugFlag;
	Session_Status 	status;
	Session_Status 	prevStatus;
	bool 			isSeeking;
	bool 			isBuffering;
	int	 			preBufferingLocalTime;
	int  			streamType;
	//new HTC request
	bool 			m_bDisableAVSyncNotification;//For quality control
	bool 			m_bOpenOriginalUDPPorts;//Some proxies would change the UDP ports which client sends to server in SETUP
	bool 			m_bOpenUAProEachRequest;//Some Customer hope that UAProfile can be included in each request
	bool 			m_bUseRTSPProxy;
	char 			m_szProxyIP[MAX_PATH];
	int  			m_CheckSessionAlive;
	bool 			forcePause;
	float 			portScale;
	int	  			m_nResetLog;
	int  			m_nAppendMode;
	int	  			m_nRTCPInterval;
	int	  			m_nWaitForTeardown;
	int	  			m_nEnableNullAudFrameWhenPacketloss;
	int	  			m_nSinkFlushed;
	int	  			m_n3GPPAdaptive;
	int	  			m_nFirstIFrame;
	int	  			nReceivdPacks;
	int	  			nLostPacks;
	int	  			m_nBufferLow;	
	T3GPPLinkChart 	m_3GPPLinkChart;
	int	  			m_nHKIcableAVOutOfSync;
	int	  			sdkVersion;
	int  			antiJitterBufferSize;
	char* 			m_extraFields;
	int  			endOfStreamFromServer;
	long 			initPlayPos; //count by msec
	bool			m_minimumLatency;
	int				m_nSourceEndTime;
	char			m_apkworkpath[512];
	int				m_skipPos;
	int				m_videoConnectionPort;
	int				m_audioConnectionPort;
	int				m_isOverHttpTunnel;
	int				m_rtspOverHttpConnectionPort;
	bool			m_isLiveStream;
	
	unsigned int 	m_nTimeMarker;
	int m_nSeekToNextIFlag;

	DigestAuthParam digestAuth;
	bool			digestIsSet;

	void *			m_pRTSPTrans;
	SetRTSPPacket	m_pRTSPTransFunc;
#ifdef _IOS
	char 			logDir[1024];
	char			cfgPath[1024];
#endif //_IOS
};

#define TRACE_FLOW 1

#ifdef LINUX
//#define LOG_DIRECTORY    ("/data/local/visualon/rtspLog/")
#define LOG_DIRECTORY    ("/sdcard/rtspLog/")
#define CFG_DIRECTORY	 "/sdcard/"
#elif defined WIN32
#define LOG_DIRECTORY    ("c:\\visualon\\")
#define CFG_DIRECTORY	 "c:\\"
#endif

#ifdef  _LINUX_ANDROID
#include "voLog_android.h"
#else//_LINUX_ANDROID
#define voLog_android(a,b)
#endif//_LINUX_ANDROID

class CLog
{
	class CLogFile
	{
	public:
		CLogFile(const char * logFileName)
			: m_logFile(NULL)

		{
#ifdef _IOS
			strcpy(m_logFileName, CUserOptions::UserOptions.logDir);
#else
			strcpy(m_logFileName, LOG_DIRECTORY);
#endif //_IOS
			
			strcat(m_logFileName, logFileName);
			

			m_logFile = fopen(m_logFileName, "w");
			if(m_logFile==NULL)
			{
			  //VOLOGI("Failed to create %s!!!",m_logFileName);
			}

			strcpy(m_logFileName, logFileName);
		}
		~CLogFile()
		{
			if(m_logFile != NULL)
			{
				fclose(m_logFile);
				m_logFile = NULL;
				TRACE_DELETE(this);
			}
		}

		char m_logFileName[MAX_PATH];
		FILE * m_logFile;
	};
	static bool m_isPrintTime;
public:
	static CLog Log;
	static char formatString[MAX_LOG_STRING_LEN];
	static void EnablePrintTime(bool isPrint){m_isPrintTime=isPrint;};
public:
	CLog(void);
	~CLog(void);
	void UpdateLogLevel();
	void Reset();
public:
	void MakeLog2(const char * logFileName, const char * logText);
	void MakeLog3(int target,const char * logFileName, const char* oriLogFile,const char * logText);
	void MakeLog(LOG_Level level,const char * logFileName, const char * logData);
	void MakeLog2(const char * logFileName, const unsigned char * logData, int len);
	void MakeLog(LOG_Level level,const char * logFileName, const unsigned char * logData, int len);
private:
	list_T<CLogFile *> m_logFileSet;
	LOG_Level m_logLevel;
	int	m_lastTime;
	int m_actualFile;
};
enum{

	PACKET_RTP=1, 
	PACKET_RTCP=1<<1,
	PACKET_RTSP=1<<2,
};
#define ROBUST_SYNC_TIME 1
#define FORCE_LIVE_SMOOTH	1
#define BIG2L_2(a) (((a[0]<<8 )&0x0000ff00)|a[1])
#define BIG2L_3(a) (((a[0]<<16)&0x00ff0000)|((a[1]<<8 )&0x0000ff00)|a[2])
#define BIG2L_4(a) (((a[0]<<24)&0xff000000)|((a[1]<<16)&0x00ff0000)|((a[2]<<8)&0x0000ff00)|a[3])
#define BITL_2(a) (((a[1]<<8 )&0x0000ff00)|a[0])
#define BITL_3(a) (((a[2]<<16)&0x00ff0000)|((a[1]<<8 )&0x0000ff00)|a[0])
#define BITL_4(a) (((a[3]<<24)&0xff000000)|((a[2]<<16)&0x00ff0000)|((a[1]<<8)&0x0000ff00)|a[0])
const long million = 1000000;
#define NEW_WAIT_I_FRAME 0 //new way still receive data and flush it if I frame arrives,else flush nothing
#define TEST_ONE_TRACK 0
#define TRACE_FLOW_RTPPARSER 0
#define TRACE_CRASH 0

#if defined(DIMS_DATA)
#define TEST_DIMS 0
#endif


#define DISABLE_OPTION_FIRST 1
#define ENABLE_WMV	1
#define ENABLE_REAL	1
#define ASYNC_TCP 1

enum{
	VOID_STREAMING_NOT_RESET_FLAG=0x5120,
	VOID_INNER_EVENT_STOP=0x5150,
	VOID_INNER_EVENT_PAUSE=0x5151,
	VOID_INNER_FRAME_RATE=0x5152,
	VOID_INNER_BUFFERING_TIME=0x5153,
};

#ifdef LINUX
#define __int64 long long
#include <string.h>
#define _strnicmp strncasecmp
#define _stricmp	strcasecmp
#define _strcmp strcmp
#endif//

#ifdef _IOS
#define __int64 long long
#include <string.h>
#define _strnicmp strncasecmp
#define _stricmp	strcasecmp
#define _strcmp strcmp
#endif//_IOS


#define SIMPLE_LOG(target,s) CLog::Log.MakeLog(LL_RTSP_ERR,target, (s));
#define SLOG0(level,target,str) CLog::Log.MakeLog(level,target,str);
#define SLOG1(level,target,str,param1)\
{sprintf(CLog::formatString,str,param1);\
	CLog::Log.MakeLog(level,target, CLog::formatString);}

#define SLOG2(level,target,str,p1,p2)\
{sprintf(CLog::formatString,str,p1,p2);\
	CLog::Log.MakeLog(level,target, CLog::formatString);}

#define SLOG3(level,target,str,p1,p2,p3)\
{sprintf(CLog::formatString,str,p1,p2,p3);\
	CLog::Log.MakeLog(level,target, CLog::formatString);}

#define SLOG4(level,target,str,p1,p2,p3,p4)\
{	sprintf(CLog::formatString,str,p1,p2,p3,p4);\
	CLog::Log.MakeLog(level,target, CLog::formatString);}

#define SLOG5(level,target,str,p1,p2,p3,p4,p5)\
{	sprintf(CLog::formatString,str,p1,p2,p3,p4,p5);\
	CLog::Log.MakeLog(level,target, CLog::formatString);}

#define SLOG6(level,target,str,p1,p2,p3,p4,p5,p6)\
{	sprintf(CLog::formatString,str,p1,p2,p3,p4,p5,p6);\
	CLog::Log.MakeLog(level,target, CLog::formatString);}

#ifdef _VONAMESPACE
}
#endif

#endif //__BASEUTILITY_H__
