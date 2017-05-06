#ifndef __BASEUTILITY_H__
#define __BASEUTILITY_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macro.h"

#include "voFLOEngine.h"

#include "vo_COMMON_SDK.h"


#include "list_T.h"
#include "mswitch.h"

#include "voRTSPCrossPlatform.h"
#include "voFLOEngine.h"
using namespace vo_cross_platform_streaming;



#define TRACE_NEW(name,address)
#define TRACE_DELETE(address)
#define trace_memLeak()

typedef int (* CALLBACKNOTIFY2) (long EventCode, long * EventParam1, long * EventParam2);

typedef struct  
{
	CALLBACKNOTIFY2 funtcion;
	void* parent;
}NotifyEventFunc;
int base64_decode(unsigned char *src, unsigned char *dst, int *dlen);
unsigned char * Base64Decode(char * in, unsigned int & resultSize, bool trimTrailingZeros = true);
unsigned int Random32();


int  Value(const char c);
void HexStrToBytes(const char * hexString, unsigned char ** buffer, int * bufLen);
void SpropParameterSetsData(const char * spropParamSets, unsigned char ** buffer, int * bufLen);


//-------------------------------------------------------------

int IsH264IntraFrame(unsigned char * buffer, int /* bufLen */);
int IsValidH264NALU(unsigned char * buffer,int size);
//------------------  class CUserOptions ------------------

typedef enum{
	LL_NONE			= 0,
	LL_RTSP_MESSAGE	= 1,
	LL_RTP_ERR	= 1<<1,
	//LL_RTP_ERR	= 1<<2,
	LL_RTSP_ERR		= 1<<3,
	//LL_RTP_ERR		= 1<<5,
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

#define LOCAL_VIDEO_RTP  0
#define LOCAL_VIDEO_RTCP 1
#define LOCAL_AUDIO_RTP  2
#define LOCAL_AUDIO_RTCP 3


#define MAX_USER_INFO_SIZE 256


int NotifyEvent(int eventCode,int param);

typedef enum 
{
	Session_Stopped,
	Session_Connectting,
	Session_Playing,
	Session_SwitchChannel,
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
}VOS_RTSP_CMD;

typedef enum
{
	VOS_STREAM_ALL		= 0xffffffff,
	VOS_STREAM_VIDEO	= 1,
	VOS_STREAM_AUDIO	= 1<<1,
	VOS_STREAM_VIDEO_RTCP = 1<<2,
	VOS_STREAM_AUDIO_RTCP = 1<<3,
	VOS_STREAM_RTSP		= 1<<10,

};

class CUserOptions
{
public:
	static CUserOptions UserOptions;

public:
	CUserOptions(void);
	~CUserOptions(void);

public:
	int m_nUDPPortFrom;
	int m_nUDPPortTo;
	int m_nMediaBuffTime;
	int m_nRTPTimeOut;
	int m_nRTSPTimeOut;
	int m_nConnectTimeOut;
	int m_nMaxErrorRetry;
	int m_nReceiverReportInterval;

	char m_sUserAgent[MAX_PATH];

	Session_Status status;
	Session_Status prevStatus;

	int m_bMakeLog;
	int m_URLType;
	
	int	m_timeoutCounter;

	bool m_exit;

	char* m_currURL;

	int sdpURLLength;

	int m_useTCP;

	HS_StreamingInfo outInfo;

	NotifyEventFunc notifyFunc;

	int	errorID;
	int eventCode;
	bool streamEnd;
	int  debugFlag;


	int  streamType;


	
	float portScale;
	int	  m_nResetLog;

	int	  m_nRTCPInterval;


	int	  nLostPacks;

	bool   disableNotify;

#ifdef MEDIA_FLO
	VO_FLOENGINE_CALLBACK m_mfCallBackFuncSet;
#endif//MEDIA_FLO
};


//-------------------  class CLog --------------

#ifdef LINUX//_WIN32_WINCE
//#define LOG_DIRECTORY    ("/voLog/")
#endif

#ifdef WINCE
#define LOG_DIRECTORY    ("\\My Documents\\VisualOn\\")
#endif//_WIN32_WINCE
const __inline char * GetLogDir2()
{
#ifdef _WIN32
	return "C:/visualon/";
#elif defined _LINUX
	return "/sdcard/visualonLog_rtsp/";
#endif // _WIN32
}
class CLogFile
{
public:
	CLogFile(const char * logFileName);
	~CLogFile();

	char m_logFileName[MAX_PATH];
	FILE * m_logFile;

};

class CLog
{
	
	static bool m_isPrintTime;

public:
	static CLog Log;
	static char formatString[1024];
	static void EnablePrintTime(bool isPrint){m_isPrintTime=isPrint;};

public:
	CLog(void);
	~CLog(void);
	void UpdateLogLevel();
	void Reset();

public:
	void MakeLog2(const char * logFileName, const char * logText);
	void MakeLog3(int target, const char * logFileName, const char* oriLogFile, const char * logText);
	void MakeLog(LOG_Level level, const char * logFileName, const char * logData);
	void MakeLog2(const char * logFileName, const unsigned char * logData, int len);
	void MakeLog(LOG_Level level, const char * logFileName, const unsigned char * logData, int len);

private:
	list_T<CLogFile *> m_logFileSet;

	int	m_lastTime;
	int m_actualFile;
	LOG_Level m_logLevel;
};

enum{

	PACKET_RTP=1, 
	PACKET_RTCP=1<<1,
	PACKET_RTSP=1<<2,
};


void ParseMPEG4LatmAudioFmtpConfig(const char * fmtpConfig, unsigned char ** buffer, int * bufLen);


const unsigned int million = 1000000;

#if 1//def R_TEST
#define SUPPORT_ADAPTATION	1
#endif//


#ifdef LINUX
#define __int64 long long
//int _strnicmp(const char *dst,const char *src,int count);
//int _stricmp(const char *dst,const char *src);
#include <string.h>
#define _strnicmp strncasecmp
#define _stricmp	strcasecmp
#define _strcmp strcmp
#define _strdup strdup
#endif//
#define SIMPLE_LOG(target,s) if(CUserOptions::UserOptions.m_bMakeLog) CLog::Log.MakeLog(LL_RTSP_ERR,target, (s));
#define SLOG1(level,target,str,param1)\
if(CUserOptions::UserOptions.m_bMakeLog)\
{sprintf(CLog::formatString,str,param1);\
	CLog::Log.MakeLog(level,target, CLog::formatString);}

#define SLOG2(level,target,str,p1,p2)\
	if(CUserOptions::UserOptions.m_bMakeLog)\
{sprintf(CLog::formatString,str,p1,p2);\
	CLog::Log.MakeLog(level,target, CLog::formatString);}

#define SLOG3(level,target,str,p1,p2,p3)\
	if(CUserOptions::UserOptions.m_bMakeLog)\
{sprintf(CLog::formatString,str,p1,p2,p3);\
	CLog::Log.MakeLog(level,target, CLog::formatString);}

#define SLOG4(level,target,str,p1,p2,p3,p4)\
	if(CUserOptions::UserOptions.m_bMakeLog)\
{	sprintf(CLog::formatString,str,p1,p2,p3,p4);\
	CLog::Log.MakeLog(level,target, CLog::formatString);}

#define SLOG5(level,target,str,p1,p2,p3,p4,p5)\
	if(CUserOptions::UserOptions.m_bMakeLog)\
{	sprintf(CLog::formatString,str,p1,p2,p3,p4,p5);\
	CLog::Log.MakeLog(level,target, CLog::formatString);}

#define SLOG6(level,target,str,p1,p2,p3,p4,p5,p6)\
	if(CUserOptions::UserOptions.m_bMakeLog)\
{	sprintf(CLog::formatString,str,p1,p2,p3,p4,p5,p6);\
	CLog::Log.MakeLog(level,target, CLog::formatString);}

#endif //__BASEUTILITY_H__