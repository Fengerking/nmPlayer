#ifdef LINUX
#include <dlfcn.h>
#endif //LINUX

#include "voLog.h"
#include "RTSPMediaStreamSyncEngine.h"
#include "utility.h"
#include "RTSPSource.h"
#include "RealRTSPClientEngine.h"
#include "ASFRTSPClientEngine.h"
#include "TaskSchedulerEngine.h"

#ifdef _IOS
#include <sys/stat.h>
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define IS_FULL(a) ((a)>95)

class CVOSocketThreadCMD:public IVOCommand
{
	CRTSPSourceEX* m_source;
public:
	CVOSocketThreadCMD(CRTSPSourceEX* source):m_source(source){};
	void Execute(){
		m_source->RecvRTSPStreams();
	}
};

class CVORestartThreadCMD:public IVOCommand
{
	CRTSPSourceEX* m_source;
public:
	CVORestartThreadCMD(CRTSPSourceEX* source):m_source(source){};
	void Execute(){
		m_source->Restart();
	}
};

#define MAX_URL_LEN 1024
CRTSPSourceEX::CRTSPSourceEX()
: m_bufferingPercent(0)
, m_bDisableGetData(false)
, m_socketThread(NULL)
, m_restartThread(NULL)
, m_dwThreadId(0)
, m_bExit(false)
, m_bStopReceivingData(false)
, m_sessionStatus(Session_Shutdown)
, m_bNeedBuffering(false)
, m_dwStartBufferingTime(0)
, m_ReportPercentTaskToken(INVALID_TASK_TOKEN)
, m_pauseTaskToken(INVALID_TASK_TOKEN)
, m_seekTaskToken(INVALID_TASK_TOKEN)
, m_stopTaskToken(INVALID_TASK_TOKEN)
, m_nextSeekTime(0)
, m_playTaskPos(0)
, m_bStreamUsingTCP(false)
, m_bPermitSeeking(true)
, m_RTSPClientEngine(NULL)
, m_pNotifyFunction(NULL)
, m_prePercent(0)
, m_pauseTime(0)
, m_bRTPTimeOut(false)
, m_sdpFileString(NULL)
{
	m_csSessionLock = NULL;
	m_nRTSPTrackNum = 0;
	SetSourceEnd(false);
	m_seekToEnd = false;
	m_livePaused = false;
	m_tryTime  = 0;
	m_scaleBufferTime = 0;
	memset(&m_rtspTracks, 0, sizeof(m_rtspTracks));
	m_rtspURL = new char[MAX_URL_LEN];TRACE_NEW("rtss_m_rtspURL",m_rtspURL);
	m_livePauseTime = 0;
	m_isBegining = true;
	m_bRestart = false;
	m_playRespForSeek = false;

	m_threadFuncCMD = new CVOSocketThreadCMD(this);TRACE_NEW("rtss_m_threadFuncCMD",m_threadFuncCMD);
	m_restartFuncCMD = new CVORestartThreadCMD(this);TRACE_NEW("rtss_mRestartThreadCMD",m_restartFuncCMD);

}

CRTSPSourceEX::~CRTSPSourceEX(void)
{
    
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$RTSP Source destruct begin!!!\n");
#ifdef LINUX
	if(CUserOptions::UserOptions.m_pRTSPTrans)
	{
		dlclose(CUserOptions::UserOptions.m_pRTSPTrans);
		CUserOptions::UserOptions.m_pRTSPTrans = NULL;
	}
#endif //LINUX
	SAFE_DELETE_ARRAY(m_rtspURL);
	SAFE_DELETE(m_threadFuncCMD);
	SAFE_DELETE(m_restartFuncCMD);
	for(int i=0; i<m_nRTSPTrackNum; ++i)
	{
		SAFE_DELETE(m_rtspTracks[i]);
	}
	if(m_sdpFileString)
		SAFE_DELETE_ARRAY(m_sdpFileString);
	Stop();
	while (m_bRestart)
	{
		IVOThread::Sleep(100);
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$RTSP Source destruct:try to exit the m_bRestart thread\n");

	}
	SAFE_DELETE(m_RTSPClientEngine);

	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$RTSP Source destruct:delete client engine\n");
	if(m_socketThread)
	{
		VOCPFactory::DeleteOneThread(m_socketThread);
		TRACE_DELETE(m_socketThread);
	}
	m_socketThread  = NULL;
	if(m_restartThread)
	{
		VOCPFactory::DeleteOneThread(m_restartThread);
		TRACE_DELETE(m_restartThread);
	}
	m_restartThread=NULL;
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$RTSP Source destruct:delete the thread\n");
	if(m_csSessionLock)
	{
		VOCPFactory::DeleteOneMutex(m_csSessionLock);
		TRACE_DELETE(m_csSessionLock);
	}
	m_csSessionLock = NULL;
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$RTSP Source destruct end!!!\n");

	if(CUserOptions::UserOptions.m_nResetLog)
		CLog::Log.Reset();

}

bool CRTSPSourceEX::CreateClientEngine(int urlType)
{
	int len = strlen(m_rtspURL)-5;
	if(len<0)
		len = 0;
	CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "@@@@SDP:before check the postfix\n");
	const char * pExtName= strstr(m_rtspURL+len, ".");
	bool IsReal =pExtName&&(_strnicmp(pExtName,".rm",3)==0||_strnicmp(pExtName,".rv",3)==0||_strnicmp(pExtName,".ra",3)==0);//||strstr(pExtName,".ram"); 
	bool IsASF  =(urlType==ST_ASF)||(pExtName&&((_strnicmp(pExtName,".wma",4)==0||_strnicmp(pExtName,".wmv",4)==0||_strnicmp(pExtName,".asf",4)==0)));//||strstr(pExtName,".ram");  
	CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "@@@@SDP:after check the postfix\n");
	if(m_RTSPClientEngine)
	{
		SAFE_DELETE(m_RTSPClientEngine);
		m_RTSPClientEngine = NULL;
	}
	if(IsReal)
	{
#if !ENABLE_REAL
		CUserOptions::UserOptions.errorID = E_UNSUPPORTED_FORMAT;
		CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt", "@@@@The version does not support the format\n");
		NotifyEvent(VO_EVENT_DESCRIBE_FAIL,CUserOptions::UserOptions.errorID);
		return false;
#else
		m_RTSPClientEngine = new CRealRTSPClientEngine();TRACE_NEW("rtss_m_RTSPClientEngine",m_RTSPClientEngine);
		SetTransportProtocol(true);
		CUserOptions::UserOptions.streamType=ST_REAL;
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "this is real RDT rtsp\n");
#endif

	}
	else if(IsASF)
	{
#if !ENABLE_WMV
		CUserOptions::UserOptions.errorID = E_UNSUPPORTED_FORMAT;
		CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt", "@@@@The version does not support the format\n");
		NotifyEvent(VO_EVENT_DESCRIBE_FAIL,CUserOptions::UserOptions.errorID);
		return false;
#else

		m_RTSPClientEngine = new CASFRTSPClientEngine();TRACE_NEW("rtss_m_RTSPClientEngine",m_RTSPClientEngine);
		SetTransportProtocol(true);
		CUserOptions::UserOptions.streamType=ST_ASF;
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "this is ASF rtsp\n");
#endif//
	}
	else
	{
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt", "@@@@SDP:create engine\n");
		m_RTSPClientEngine = new CRTSPClientEngine();TRACE_NEW("rtss_m_RTSPClientEngine2",m_RTSPClientEngine);
		CUserOptions::UserOptions.streamType=ST_3GPP;
	}
#if 1//def HTC_PLAYLIST
	m_RTSPClientEngine->SetRTSPSource(this);
#endif //HTC_PLAYLIST
	return true;
}
//extern const char* sgVersion;

bool CRTSPSourceEX::Open(const char * rtspURL)
{
	if(CUserOptions::UserOptions.m_nResetLog)
		CLog::Log.Reset();

	FILE *fp = NULL;

#ifndef _IOS
	if((fp = fopen(CFG_DIRECTORY"rtsp.cfg","r")) == NULL)
#else
	if((fp = fopen(CUserOptions::UserOptions.cfgPath,"r")) == NULL)
#endif
	{
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","RTSP: Could not open rtsp.cfg file!\n");
		CUserOptions::UserOptions.m_bMakeLog = 0;
	}
	else
	{
		char buf[100];
		char *log = NULL;
		memset(buf, 0, 100);
		fgets(buf, 100, fp);
		log = strstr(buf,"log=");
		if(log)
		{
			log+=strlen("log=");
			CUserOptions::UserOptions.m_bMakeLog = atoi(log);
            #ifdef _IOS
            if(CUserOptions::UserOptions.m_bMakeLog>0)
                mkdir(CUserOptions::UserOptions.logDir,0777);
            #endif
		}
		else
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","RTSP: log level can not find in rtsp.cfg file!\n");
		}
	}
	
	if(fp)
		fclose(fp);

	sprintf(CLog::formatString,"CUserOptions::UserOptions.m_bMakeLog = %d\n", CUserOptions::UserOptions.m_bMakeLog);
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
	CLog::Log.UpdateLogLevel();

#ifdef LINUX
	//if(CUserOptions::UserOptions.m_pRTSPTrans=dlopen("/sdcard/libvoSrcRTSPTrans.so",RTLD_LAZY))
	//if(CUserOptions::UserOptions.m_pRTSPTrans=dlopen(CUserOptions::UserOptions.m_apkworkpath,RTLD_LAZY))
    //strcat(CUserOptions::UserOptions.m_apkworkpath,"libvoSrcRTSPTrans.so");
	//VOLOGI("************************libvoSrcRTSPTrans.so path(%s)",CUserOptions::UserOptions.m_apkworkpath);
	//CUserOptions::UserOptions.m_pRTSPTrans=dlopen(CUserOptions::UserOptions.m_apkworkpath,RTLD_LAZY);

	CUserOptions::UserOptions.m_pRTSPTrans=dlopen("/data/data/com.philips.cl.insight/lib/libvoSrcRTSPTrans.so",RTLD_LAZY);
	if(CUserOptions::UserOptions.m_pRTSPTrans)
	{
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","libvoSrcRTSPTrans loaded!!!\n");
		CUserOptions::UserOptions.m_pRTSPTransFunc = (SetRTSPPacket)dlsym(CUserOptions::UserOptions.m_pRTSPTrans,"SetRTSPPacket");
		if(CUserOptions::UserOptions.m_pRTSPTransFunc)
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","SetRTSPPacket loaded!!!\n");
		}
		else
		{
			sprintf(CLog::formatString,"SetRTSPPacket load failed, %s!!!\n",(const unsigned char *)dlerror());
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
			
			dlclose(CUserOptions::UserOptions.m_pRTSPTrans);
			CUserOptions::UserOptions.m_pRTSPTrans = NULL;
		}
	}
	else
	{
		sprintf(CLog::formatString,"libvoSrcRTSPTrans load failed, %s!!!\n",(const unsigned char *)dlerror());
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
	}
#endif

	if(strstr(rtspURL,"live/co/icable/news/ipad/xyz/icable_news.3gp"))
	{
		CUserOptions::UserOptions.m_nHKIcableAVOutOfSync = 1;
	}
	if(strstr(rtspURL,"rtsp://v2.cache7.c.youtube.com/video.3gp?cid=0xc80658495af60617&fmt=17&user=android-device-test"))
	{
		CUserOptions::UserOptions.m_nSourceEndTime = 3000;
	}
	if(strstr(rtspURL,"h8f_test_sdp2"))
	{
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","This is SDP2 test\n");
		rtspURL= "c:/visualon/RTSPStream.sdp2";
		
#ifdef _LINUX_ANDROID
		rtspURL= "/data/local/RTSPStream.sdp2";
#endif
	}

	if(strlen(rtspURL)>MAX_URL_LEN)
	{
		CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt","(rtspURL)>MAX_URL_LEN\n");
		return false;
	}
	TRACE_NEW("rtss_m_rtspURL",m_rtspURL);
#if DELAY_STOP
	if(m_RTSPClientEngine)
	{
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","DELAY_STOP:restart\n");
		return true;
	}
#endif//
	const char* sgVersion1="2012.0418.1732: add voSource2 support  ";
	if(CUserOptions::UserOptions.m_bMakeLog)
	{
		sprintf(CLog::formatString,"version=%s\nurl=%s\n,tcp=%d,Log=%d,livepaused=%d,BuffTime=%d,RTP=%d,RTSP=%d,bs=%d\n"
			"buflow=%d,3GPPAdapt=%d,connect=%d,firstI=%d,rtcpInterval=%d\n"
			"portopen=%d,proxy=%d,\nUAmust=%d,nullframe=%d,tearDown=%d,port(%d-%d) ,liveFirstFrame=%d\nusa=%s\nup=%s\nproxyURL=%s\n,ext=%s\n",
			sgVersion1,
			rtspURL,
			CUserOptions::UserOptions.m_useTCP,
			CUserOptions::UserOptions.m_bMakeLog,
			CUserOptions::UserOptions.m_nForceLivePause,
			CUserOptions::UserOptions.m_nMediaBuffTime,
			CUserOptions::UserOptions.m_nRTPTimeOut,
			CUserOptions::UserOptions.m_nRTSPTimeOut,
			CUserOptions::UserOptions.m_nBandWidth,
			CUserOptions::UserOptions.m_nBufferLow,
			CUserOptions::UserOptions.m_n3GPPAdaptive,
			CUserOptions::UserOptions.m_nConnectTimeOut,
			CUserOptions::UserOptions.m_nFirstIFrame,
			CUserOptions::UserOptions.m_nRTCPInterval,
			CUserOptions::UserOptions.m_nPortOpener,
			CUserOptions::UserOptions.m_bUseRTSPProxy,
			CUserOptions::UserOptions.m_bOpenUAProEachRequest,
			CUserOptions::UserOptions.m_nEnableNullAudFrameWhenPacketloss,
			CUserOptions::UserOptions.m_nWaitForTeardown,
			CUserOptions::UserOptions.m_nUDPPortFrom,
			CUserOptions::UserOptions.m_nUDPPortTo,
			CUserOptions::UserOptions.m_nSinkFlushed,
			CUserOptions::UserOptions.m_sUserAgent,
			CUserOptions::UserOptions.m_sUAProfile,
			CUserOptions::UserOptions.m_szProxyIP,
			CUserOptions::UserOptions.m_extraFields
			);
		CLog::Log.MakeLog(LL_RTSP_ERR,"config.txt",CLog::formatString);
	}

	
	CUserOptions::UserOptions.eventCode = 0;
	if(m_csSessionLock==NULL)
	{
		m_csSessionLock = VOCPFactory::CreateOneMutex();TRACE_NEW("m_criticalSec2",m_csSessionLock);
	}
		//CUserOptions::UserOptions.m_bMakeLog = LOG_RTP;
	bool isValidURL=false;
	do 
	{
		bool ishttpSDP=false;
 		ishttpSDP=_strnicmp(rtspURL,"http://",7)==0;

  		if(!ishttpSDP&&(_strnicmp(rtspURL, "rtsp://", 7) == 0
			||_strnicmp(rtspURL, "mobitv://", 9) == 0
			||_strnicmp(rtspURL, "mms://", 6) == 0
			||_strnicmp(rtspURL, "voStream://", 11) == 0))
		{
			int urlType=ST_3GPP;
			if(_strnicmp(rtspURL, "rtsp://", 7) == 0)
			{
				strcpy(m_rtspURL, rtspURL);
			}
			else if(_strnicmp(rtspURL, "mobitv://", 9) == 0)//rename the protocol name
			{
				strcpy(m_rtspURL,"rtsp://");
				strcpy(m_rtspURL+7,rtspURL+9);
			}
			else if(_strnicmp(rtspURL, "mms://", 6) == 0)//rename the protocol name
			{
				strcpy(m_rtspURL,"rtsp://");
				strcpy(m_rtspURL+7,rtspURL+6);
				urlType=ST_ASF;
			}
			else if(_strnicmp(rtspURL, "voStream://", 11) == 0)//rename the protocol name
			{
				strcpy(m_rtspURL,"rtsp://");
				strcpy(m_rtspURL+7,rtspURL+11);
			}
			
				

			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$rtsp link:begin to open the url@@@@\n");
			CUserOptions::UserOptions.m_URLType = UT_RTSP_URL;
			if(CreateClientEngine(urlType)==false)
				break;
#if defined(TCP_FIRST1) || defined(WIN_X86)
			//SetTransportProtocol(true);;
			//CUserOptions::UserOptions.m_useTCP=-1;
#endif//_DEBUG
#ifdef DISABLE_TCP
			//CUserOptions::UserOptions.m_useTCP=-1;
#endif//
			isValidURL = true;
		}
		else
		{
			int len = strlen(rtspURL)-5;
			if(len<0)
				len = 0;
			const char * pExtName = (const char*)strstr(rtspURL+len, ".");
 
			if(pExtName == NULL)
				pExtName = ".sdp";

 			if(ishttpSDP||_stricmp(pExtName, ".sdp") == 0
				||_stricmp(pExtName, ".ram") == 0
				||_stricmp(pExtName, ".sdp1") == 0
				||_stricmp(pExtName, ".smil") == 0
				)
			{
				CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt","$$$$$It is a sdp file\n");
				if (ishttpSDP==false&&_strnicmp(rtspURL, "http://", 7))//if it is local sdp
				{
					
					if(!GetRTSPLinkFromSDPFile(rtspURL))
					{
						CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$GetRTSPLinkFromSDPFile fail@@@@\n");
						break;
					}
					if (_stricmp(pExtName, ".smil") == 0)
					{
						char* modify=strchr(m_rtspURL,'\"');
						if(modify)
							*modify='\0';
					}
				}
				else//if it is http...sdp
				{
					char* tmpURL=(char*)rtspURL;
#ifndef LINUX
					if (strstr(tmpURL,".sdp1"))//for WMPP
					{
						strcpy(m_rtspURL, "http://");
						strcat(m_rtspURL,rtspURL+7);
						m_rtspURL[strlen(m_rtspURL)-1]='\0';
						tmpURL = m_rtspURL;
					}
#endif//LINUX
					if(!GetRTSPLinkFromSDPLink(tmpURL))
					{
						CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$GetRTSPLinkFromSDPFile fail@@@@\n");
						break;
					}
				}
				
				CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt","$$$$$Get the link of sdp\n");
				if(CreateClientEngine()==false)
					break;
				CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt","$$$$$Create Engine Done\n");

				if(CUserOptions::UserOptions.streamType==ST_REAL||_stricmp(pExtName, ".smil") == 0)
				{
					CUserOptions::UserOptions.m_URLType = UT_RTSP_URL;
				}
				else
				{
					m_RTSPClientEngine->SetSDPFileString(m_sdpFileString);
					CUserOptions::UserOptions.m_URLType = UT_SDP_URL;
				}
				isValidURL = true;
			}
			else if(_stricmp(pExtName, ".sdp2") == 0)
			{
				CUserOptions::UserOptions.m_URLType = UT_SDP2_LOCAL;
				//if(CUserOptions::UserOptions.m_bMakeLog < LOG_RTP)
					CUserOptions::UserOptions.m_bMakeLog = 4;//LOG_RTP;
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","This is LOCAL dump file!!\n");
				if(CreateClientEngine()==false)
					break;
				bool ok =m_RTSPClientEngine->ParseSDP2(rtspURL);
				if(!ok)
				{
					CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@SDP2:ParseSDP2 fail@@@@\n");
					return false;
				}
				if(!InitRTSPTracks())
				{
					CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@SDP2:InitRTSPTracks fail@@@@\n");
					return false;
				}

				//m_sessionStatus = Session_Playing;
				SetSessionStatus(Session_Playing);
				isValidURL=true;
				return ok;
			}
			
		}
		if(!isValidURL)
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@Invalid URL\n");
			return false;
		}
		bool startOK=StartupRTSPSession(0);
		if(startOK)
		{
		     if(CUserOptions::UserOptions.m_exit==false)//it is possible because the ForceStop may be in another thread
			 {
 				return true;
			 }
			 else
			 {
				 CLog::Log.MakeLog(LL_RTSP_ERR,"socketerr.txt","user force stop the connection\n");
				 break;
			 }
		}
		else
		{
			break;
		}
	} while(0);
	
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$fail to open the url@@@@\n");
	return false;			
}
void  CRTSPSourceEX::SetSessionStatus(Session_Status status)
{
	m_sessionStatus = status;
	if(CUserOptions::UserOptions.status != status)
	{
		CUserOptions::UserOptions.prevStatus = CUserOptions::UserOptions.status;
		CUserOptions::UserOptions.status = status;
	}
	
}
int		CRTSPSourceEX::CheckTimeToStop(void * param)
{
	CRTSPSourceEX* source=(CRTSPSourceEX*)param;
	source->DoTimeToStop();
	return 0;
}
void	CRTSPSourceEX::DoTimeToStop()
{
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","DoTimeToStop\n");
	//Stop();
	NotifyEvent(VOID_INNER_EVENT_STOP,1);
}
long CRTSPSourceEX::SetStopTask(float timeBySec)
{
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","SetStopTask\n");
	UpdateStopTask(timeBySec);
	
	return 1;
}
long CRTSPSourceEX::UpdateStopTask(float timeBySec)
{
	CTaskSchedulerEngine * engine=CTaskSchedulerEngine::CreateTaskSchedulerEngine();
	if(engine)
		m_stopTaskToken=engine->ScheduleTask(timeBySec*1000, (TaskFunction *)CRTSPSourceEX::CheckTimeToStop, this,m_stopTaskToken);
	return m_stopTaskToken;
}
void	CRTSPSourceEX::RemoveStopTask()
{
	
	if(m_stopTaskToken>0)
	{
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","RemoveStopTask\n");
		UpdateStopTask(30000);//forward 30,000 seconds
	}
}

bool CRTSPSourceEX::Stop()
{
#if TRACE_FLOW
	sprintf(CLog::formatString,"$$$$$Stop() begin status=%d@@@@\n",m_sessionStatus);
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
#endif//TRACE_FLOW
	if(m_sessionStatus == Session_Shutdown)
	{ 		
		return true;
	}
	m_bStopReceivingData = true;	
	m_bExit = true;
	
	while(m_dwThreadId != 0)
	{
		IVOThread::Sleep (1);
	}
	CAutoCriticalSec cAutoLock(m_csSessionLock);
	CTaskSchedulerEngine::CreateTaskSchedulerEngine()->UnscheduleAllTask();

	if(!m_bPermitSeeking)
	{
		m_RTSPClientEngine->ResetPauseTime();
	}
	IVOThread::Sleep (10);
	ShutdownRTSPSession();
#if TRACE_FLOW
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$Stop() end@@@@\n");
#endif//TRACE_FLOW
	if(m_socketThread)
	{
		VOCPFactory::DeleteOneThread(m_socketThread);
		TRACE_DELETE(m_socketThread);
	}
	m_socketThread  = NULL;
	//if(m_csSessionLock)
	//	VOCPFactory::DeleteOneMutex(m_csSessionLock);
	//m_csSessionLock = NULL;
	m_bufferingPercent = 0;
	m_prePercent=0;
	return true;
}

bool CRTSPSourceEX::Start(int startTimeByMS)
{
	//CAutoCriticalSec cAutoLock(m_csSessionLock);
	RemoveAllTask();
	if(m_socketThread != NULL)
	{
#if TRACE_FLOW
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$Start() m_hThread != NULL @@@@\n");
#endif//TRACE_FLOW
		return true;
	}
	
#if TRACE_FLOW
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$Start() begin@@@@\n");
#endif//TRACE_FLOW
	if(m_sessionStatus == Session_Shutdown)
	{
#if TRACE_FLOW
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$Start()_StartupRTSPSession(0) begin@@@@\n");
#endif//TRACE_FLOW
		if(!StartupRTSPSession(startTimeByMS))
			return false;
	}
	if(!m_bPermitSeeking)//live
	{
		m_RTSPClientEngine->ResetPauseTime();
	}
	int flushFlag=-1;
	if(startTimeByMS>0&&CUserOptions::UserOptions.m_CheckSessionAlive)//m_sessionStatus==Session_PauseFail)
	{
		flushFlag=0;
	}
	/*
	if(CUserOptions::UserOptions.initPlayPos==0)//Do not SetPos again if the initPlaypos exists
		for(int i=0; i<m_nRTSPTrackNum; ++i)
		{
			m_rtspTracks[i]->SetPosition(startTimeByMS,flushFlag);
		}
	else
		CUserOptions::UserOptions.initPlayPos=0;
	*/
	//init the status flag
	SetSourceEnd(false);
	m_seekToEnd = false;
	m_livePaused = false;
	m_bStopReceivingData = false;
	m_bRTPTimeOut = false;
	m_bExit = false;
	m_bStreamOver = false;
	CUserOptions::UserOptions.m_exit = false;
	//EnableBuffering(true);
	m_dwStartBufferingTime = 0;
	//m_hThread = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)RecvRTSPStreamsProc, this, 0, &m_dwThreadId);
	m_socketThread=VOCPFactory::CreateOneThread();TRACE_NEW("m_socketThread",m_socketThread);
	m_socketThread->Create(m_threadFuncCMD);

	m_dwThreadId=m_socketThread?m_socketThread->GetThreadID():0;
	if(m_dwThreadId==0)
	{
		sprintf(CLog::formatString,"create socket thread fails=%d,m_dwThreadId=%ld\n",::GetSocketError(),m_dwThreadId);
		CLog::Log.MakeLog(LL_RTSP_ERR,"socketerr.txt",CLog::formatString);
	}
	
#if TRACE_FLOW
	sprintf(CLog::formatString,"$$$$$Start() end,threadID=%ld@@@@\n",m_dwThreadId);
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
#endif//TRACE_FLOW
	
	return true;
}
bool CRTSPSourceEX::PauseLiveStream()
{
	if(m_sessionStatus == Session_LivePaused)
		return true;
#if TRACE_FLOW
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$PauseLiveStream() begin@@@@\n");
#endif//TRACE_FLOW
	//check if server supports Pause for Live :
	if(CUserOptions::UserOptions.m_nForceLivePause==0)//&&CUserOptions::UserOptions.m_nSupportPause==0)
		return false;

	bool paused = m_RTSPClientEngine->PauseCmd();
	if(paused)//||CUserOptions::UserOptions.m_nSupportPause)
	{
		//m_livePauseTime = timeGetTime();
		//m_sessionStatus = Session_LivePaused;
		SetSessionStatus(Session_LivePaused);
		IVOThread::Sleep(1);
		//m_RTSPClientEngine->FlushRTSPStreams();
		m_livePaused = true;
		NotifyEvent(HS_EVENT_FLUSH_BUFFER,0);
		int max_pos = 0;
		for(int i=0; i<m_nRTSPTrackNum; ++i)
		{
			int pos = m_rtspTracks[i]->GetTrackPlayPos();
			if(pos>max_pos)
				max_pos = pos;
		}
		for(int i=0; i<m_nRTSPTrackNum; ++i)
			m_rtspTracks[i]->SetTrackLivePausePos(max_pos);
	}
#if TRACE_FLOW
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$PauseLiveStream() end@@@@\n");
#endif//TRACE_FLOW
	return true;
}
bool CRTSPSourceEX::ResumeLiveStream()
{
	if(m_sessionStatus == Session_Playing)
		return true;
#if TRACE_FLOW
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$ResumeLiveStream() begin@@@@\n");
#endif//TRACE_FLOW

	if(m_livePaused)
	{
		m_dwStartBufferingTime = 0;
		m_livePaused = false;
		m_RTSPClientEngine->FlushRTSPStreams();

		if(PlayRTSPSession())
		{
			CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
			pRTSPMediaStreamSyncEngine->EnableWaitIFrame(true);
			pRTSPMediaStreamSyncEngine->SetMaxWaitFrames(25);
			
			EnableBuffering(true);
			m_livePauseTime = -1;//timeGetTime()-m_livePauseTime+1000;
			for(int i=0; i<m_nRTSPTrackNum; ++i)
			{
				int pos = m_rtspTracks[i]->GetTrackLivePausePos();
				m_rtspTracks[i]->SetStartTime(pos);
				//m_rtspTracks[i]->SetTrackLivePausePos(pos+m_livePauseTime);

			}
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$ResumeLiveStream() successfullly!@@@@\n");
		}
		else
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$ResumeLiveStream() fail!@@@@\n");
			return false;
		}

	}
#if TRACE_FLOW
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$ResumeLiveStream() end@@@@\n");
#endif//TRACE_FLOW	
	return true;
}
int		CRTSPSourceEX::CheckTimeToSeek(void * param)
{
	CRTSPSourceEX* source=(CRTSPSourceEX*)param;
	source->DoTimeToSeek();
	return 0;
}
void	CRTSPSourceEX::DoTimeToSeek()
{
	RemoveAllTask();
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","TimeToSeek\n");
	SetPosition(m_nextSeekTime);
}
long CRTSPSourceEX::SetSeekTask(float timeBySec,int dwStart)
{
	m_nextSeekTime = dwStart;
	UpdateSeekTask(timeBySec);
	
	return 1;
}
long CRTSPSourceEX::UpdateSeekTask(float timeBySec)
{
	return m_seekTaskToken=CTaskSchedulerEngine::CreateTaskSchedulerEngine()->ScheduleTask(timeBySec*1000, (TaskFunction *)CRTSPSourceEX::CheckTimeToSeek, this,m_seekTaskToken);
}
void	CRTSPSourceEX::RemoveSeekTask()
{
	if(m_seekTaskToken>0)
		UpdateSeekTask(20000);//forward 20,000 seconds
}
int		CRTSPSourceEX::CheckTimeToPause(void * param)
{
	CRTSPSourceEX* source=(CRTSPSourceEX*)param;
	source->DoTimeToPause();
	return 0;
}
void	CRTSPSourceEX::RemoveAllTask()
{
	RemovePauseTask();
	RemoveSeekTask();
	RemoveStopTask();
	RemovePlayTask();
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","remove-all-tasks\n");
}
int	CRTSPSourceEX::			CheckTimeToReportPercent(void * param)
{
	CRTSPSourceEX* source=(CRTSPSourceEX*)param;
	source->DoTimeToReportPercentTask();
	return 0;
}

long		CRTSPSourceEX::				UpdateReportPercentTask(int timeByMS)
{
	return m_ReportPercentTaskToken=CTaskSchedulerEngine::CreateTaskSchedulerEngine()->ScheduleTask(timeByMS, (TaskFunction *)CRTSPSourceEX::CheckTimeToReportPercent, this,m_ReportPercentTaskToken);
}
void		CRTSPSourceEX::				DoTimeToReportPercentTask()
{
#define DEFAULT_REPORT_TIME 200
#define PAUSE_REPORT 1000000
	if(m_bNeedBuffering)
	{
		long percent;
		GetParameter(VOID_STREAMING_BUFFERING_PROGRESS,&percent);
		NotifyEvent(VO_EVENT_BUFFERING_PERCENT,percent);
		//sprintf(CLog::formatString,"buf:%d\%\n",percent);
		//CLog::Log.MakeLog(LL_RTSP_ERR,"buffering.txt",CLog::formatString);
		UpdateReportPercentTask(DEFAULT_REPORT_TIME);
	}
	else
	{
		UpdateReportPercentTask(PAUSE_REPORT);
	}
}
int		CRTSPSourceEX::CheckTimeToPlay(void * param)
{
	CRTSPSourceEX* source=(CRTSPSourceEX*)param;
	source->DoTimeToPlay();
	return 0;
}
void	CRTSPSourceEX::DoTimeToPlay()
{
	RemoveAllTask();
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","TimeToPlay\n");

	if(m_playTaskPos>=0)
		PlayRTSPSession(m_playTaskPos);
	else
		PlayRTSPSession();
}
long CRTSPSourceEX::SetPlayTask(float timeBySec,int dwStart)
{
	m_playTaskPos=dwStart;
	UpdatePlayTask(timeBySec);
	return 1;
}
long CRTSPSourceEX::UpdatePlayTask(float timeBySec)
{
	return m_playTaskToken=CTaskSchedulerEngine::CreateTaskSchedulerEngine()->ScheduleTask(timeBySec*1000, (TaskFunction *)CRTSPSourceEX::CheckTimeToPlay, this,m_playTaskToken);
}
void	CRTSPSourceEX::RemovePlayTask()
{
	if(m_playTaskToken>0)
		UpdatePlayTask(20000);//forward 20,000 seconds
}
void	CRTSPSourceEX::DoTimeToPause()
{
#define TIME_TO_PAUSE_CHECK 2
#define EXPECT_BUF_TIME_FOR_PAUSE_SEC 10
	int percent=GetBufPercent(CUserOptions::UserOptions.m_nMediaBuffTime+TIME_TO_PAUSE_CHECK);
	int isEnd=IsDataEnd();

	if(percent==100&&!isEnd)
	{
		PauseRTSPSession();
	}
	else if(!isEnd)
	{
		UpdatePauseTask(TIME_TO_PAUSE_CHECK);
	}
		
	sprintf(CLog::formatString,"pause=%d,end=%d\n",percent,isEnd);
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
}

long CRTSPSourceEX::SetPauseTask(float timeBySec,bool delay)
{
	DoTimeToPause();
	return 1;
}

long CRTSPSourceEX::UpdatePauseTask(float timeBySec)
{
	return m_pauseTaskToken=CTaskSchedulerEngine::CreateTaskSchedulerEngine()->ScheduleTask(timeBySec*1000, (TaskFunction *)CRTSPSourceEX::CheckTimeToPause, this,m_pauseTaskToken);
}
void	CRTSPSourceEX::RemovePauseTask()
{
	if(m_pauseTaskToken>0)
		UpdatePauseTask(10000);//forward 20,000 seconds
}

bool CRTSPSourceEX::Pause()
{
	CAutoCriticalSec cAutoLock(m_csSessionLock);
#if TRACE_FLOW
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$Pause() begin@@@@\n");
#endif//TRACE_FLOW
	do
	{
		if(m_bPermitSeeking)
		{
			//if there is pause task in queue,do nothing
			//this may happen when user click pause quickly
			if(CUserOptions::UserOptions.forcePause==false)
			{
				//if(m_pauseTaskToken==INVALID_TASK_TOKEN)
				SetPauseTask(2);
			}
			else//pause immediately
			{
				//if(m_pauseTaskToken!=INVALID_TASK_TOKEN)
				PauseRTSPSession();

				CUserOptions::UserOptions.forcePause=false;
			}
			

			//if(!PauseRTSPSession())
			//	break;
		}
		else
		{
			//if(!m_RTSPClientEngine->TeardownCmd())
			//	break;
			//m_dwPausedTime = ???
			//m_sessionStatus = Session_Paused;
#ifdef BRCM_WMPP//FUCK BRCM,a abnormal requirement
			m_RTSPClientEngine->PauseCmd();
			break;
#endif//BRCM_WMPP
			if(!PauseLiveStream())
				break;
		}
#if TRACE_FLOW
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$Pause() end@@@@\n");
#endif//TRACE_FLOW
		return true;

	}while(0);
#if TRACE_FLOW
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$Pause() fail@@@@\n");
#endif//TRACE_FLOW	
	return false;
}
bool CRTSPSourceEX::ResumeStreamAfterPauseFail()
{
	do 
	{
		
		if(m_RTSPClientEngine->PauseCmd(3))
		{
			SetSessionStatus(Session_Paused);
			m_pauseTime=0;
			for(int i=0; i<m_nRTSPTrackNum; ++i)
			{
				int time1=m_rtspTracks[i]->GetLastTimeInBuffer();
				m_pauseTime = max(m_pauseTime,time1);
			}
			m_pauseTime+=500;//forward one second
			sprintf(CLog::formatString,"ResumeStreamAfterPauseFail: the session is still pos=%d\n",m_pauseTime);
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
			if(!PlayRTSPSession(m_pauseTime))
				break;
		}
		else//the session is over, restart it
		{
			/*
			m_pauseTime=0;
			for(int i=0; i<m_nRTSPTrackNum; ++i)
			{
				int time1=m_rtspTracks[i]->GetTrackPlayPos();
				m_pauseTime = max(m_pauseTime,time1);
			}
			m_pauseTime-=1000;//forward one second
			if(m_pauseTime<0)
				m_pauseTime = 0;
			sprintf(CLog::formatString,"ResumeStreamAfterPauseFail: the session is still pos=%d\n",m_pauseTime);
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
			if(!Stop())
				break;
			if(!Start(m_pauseTime))
				break;
			SetPosition(m_pauseTime);
			m_livePauseTime = -1;*/
			NotifyEvent(VO_EVENT_PAUSE_FAIL,E_METHOD_INVALID);
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","ResumeStreamAfterPauseFail:Session Over\n");
		}
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","ResumeStreamAfterPauseFail:Done!\n");
		return true;
	} while(0);
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","ResumeStreamAfterPauseFail:Fail Again\n");

	return false;
}
bool CRTSPSourceEX::Run()
{
	CAutoCriticalSec cAutoLock(m_csSessionLock);
	//m_livePauseTime = -1;
#if TRACE_FLOW
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$Run() begin@@@@\n");
#endif//TRACE_FLOW
	if(m_socketThread == NULL)
	{
 		Start();
	}
	do
	{
		if(m_bPermitSeeking)
		{
			RemoveStopTask();
			RemovePauseTask();
			if(m_sessionStatus==Session_NewPos)
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$Run() Session_NewPos\n");
				break;
			}

 			if(m_sessionStatus == Session_PauseFail)
			{
				if(!ResumeStreamAfterPauseFail())
					break;
			}
			else 
			{
				if(!PlayRTSPSession())
					break;
 			}
		}
		else//below is live stream
		{
			if(m_livePaused)
			{
				//m_RTSPClientEngine->FlushRTSPStreams();
				//if(!StartupRTSPSession(0))
				//	break;
				ResumeLiveStream();
				
			}
		}
#if TRACE_FLOW
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$Run() end@@@@\n");
#endif//TRACE_FLOW
		return true;

	}while(0);
#if TRACE_FLOW
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$Run() fail@@@@\n");
#endif//TRACE_FLOW
	return false;
}
bool	CRTSPSourceEX::m_speedupGetLeftSocketData = false;
int	CRTSPSourceEX::GetBufPercent2()
{
	CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
	if(pRTSPMediaStreamSyncEngine->GetPlayResponse())
	{
		return GetBufPercent();
	}
	else
		return 0;

}
bool CRTSPSourceEX::SetPosition(DWORD dwStart)
{
	if(!m_bPermitSeeking) 	
		return true;
	CAutoCriticalSec cAutoLock(m_csSessionLock);

#if TRACE_FLOW
	sprintf(CLog::formatString,"$$$$$SetPosition(%ld) begin\n",dwStart);
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
#endif//TRACE_FLOW
	if(m_bExit)
	{
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","the socket thread is closed,seek fail\n");
		return false;
		//CUserOptions::UserOptions.errorID=E_DATA_INACTIVITY;
		//NotifyEvent(VO_EVENT_PLAY_FAIL,CUserOptions::UserOptions.errorID);
	}

	if(dwStart>m_RTSPClientEngine->StreamingDuration()*1000)
	{
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$Invalid Position\n");
		return true;
	}

	SetSourceEnd(false);
	for(int i=0; i<m_nRTSPTrackNum; ++i)
	{
		m_rtspTracks[i]->SetEndOfStream(false);
	}

	bool isNeedToRequestServer=true;

	do
	{	
		CUserOptions::UserOptions.isSeeking = true;
		//EnableBuffering(false);

		if(isNeedToRequestServer&&!PauseRTSPSession())
			break;
		IVOThread::Sleep(0);//Make sure that the GetSample Thread is hang;
		for(int i=0; i<m_nRTSPTrackNum; ++i)
		{
			if(m_rtspTracks[i]->SetPosition(dwStart,isNeedToRequestServer)==false)
			{
				m_seekToEnd = true;
#if TRACE_FLOW
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$Track is seek to end@@@@\n");
#endif//TRACE_FLOW
			}
		}
		SetSessionStatus(Session_NewPos);

		if(isNeedToRequestServer&&!PlayRTSPSession(dwStart))//once the play request is done,the seq_number is the sync flag
			break;
		
		CUserOptions::UserOptions.isSeeking = false;
		//EnableBuffering(true);
		m_dwStartBufferingTime = 0;
		
		CUserOptions::UserOptions.preBufferingLocalTime = timeGetTime();
#if TRACE_FLOW
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$SetPosition() end@@@@\n");
#endif//TRACE_FLOW
		return true;

	}while(0);

#if TRACE_FLOW
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$SetPosition() fail@@@@\n");
#endif//TRACE_FLOW
	CUserOptions::UserOptions.isSeeking = false;
	return false;

}
void CRTSPSourceEX::EnableBuffering(bool buffering)
{
		return;
	if(m_sessionStatus != Session_LivePaused)
	{
		m_bNeedBuffering = buffering;
		if(m_bNeedBuffering)
		{
			m_bufferingPercent = 0;
			m_prePercent=0;
			NotifyEvent(VO_EVENT_BUFFERING_BEGIN,0);
			CUserOptions::UserOptions.isBuffering=true;
			UpdateReportPercentTask(0);
		}
	}
}


#define MAX_REPEAT_COUNT 100
#define WAIT_COMPLETE_TIME 20
bool CRTSPSourceEX::IsPrebufferingCompleted()
{
	m_bufferingPercent = GetBufPercent2();
 
	CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();

	if(IS_FULL(m_bufferingPercent)&&pRTSPMediaStreamSyncEngine->GetPlayResponse()==false)
	{
		//this case may happen when the RTP data is much quicker than RTSP,when the buffer is full,the RTSP play response
		//still has not arrive,so wait it
		//but if the percent is over 150,then try another sync way
#ifdef BRCM_WMPP
#define MAX_PERCENT 100
#else//BRCM_WMPP
#define MAX_PERCENT 100
#endif//BRCM_WMPP
		if(m_bufferingPercent>MAX_PERCENT||IsDataEnd())
		{
			pRTSPMediaStreamSyncEngine->SetSyncByRTPInfo(false);
			m_RTSPClientEngine->CalculateNPTAfterPlayResponse();
			CLog::Log.MakeLog(LL_RTSP_ERR,"buffering.txt","transfer to another sync way\n");
		}
		else
		{
			m_bufferingPercent = 95;
			CLog::Log.MakeLog(LL_RTSP_ERR,"buffering.txt","error:isFull but no play response\n");
		}
		
	}

	return IS_FULL(m_bufferingPercent);
}

bool  CRTSPSourceEX::EitherBufferEmpty()
{
	int mediaCount=m_RTSPClientEngine->MediaStreamCount();
	if(mediaCount>1)
	{
		CMediaStream * pMediaStream1 = m_RTSPClientEngine->MediaStream(0);
		CMediaStream * pMediaStream2 = m_RTSPClientEngine->MediaStream(1);
		unsigned long time1=(int)pMediaStream1->HaveBufferedMediaStreamTime();
		unsigned long time2=(int)pMediaStream2->HaveBufferedMediaStreamTime();
		if(time1<0)
			time1 = 0;
		if(time2<0)
			time2 = 0;
		
		if(!time1 || !time2)//time3<limit2 is used for handle the case that the INVALID_TIMESTAMP
		{
			sprintf(CLog::formatString,"one buffer is always empty, time1 %ld, time2 %ld\n",time1,time2);
			CLog::Log.MakeLog(LL_RTSP_ERR,"buffer.txt",CLog::formatString);
			return true;
		}
	}
	return false;	
}
bool  CRTSPSourceEX::IsBothBufferLessThan(int lowseconds)
{
	if(lowseconds<=0)
		return false;
	unsigned int lowestTime=lowseconds*1000;
	

	for(int streamIndex=0; streamIndex<m_RTSPClientEngine->MediaStreamCount(); ++streamIndex)
	{
		CMediaStream * pMediaStream = m_RTSPClientEngine->MediaStream(streamIndex);
		if(pMediaStream->HaveBufferedMediaStreamTime()>lowestTime)
				return false;
	
	}
	return true;
}
bool  CRTSPSourceEX::IsBothBufferEmpty()
{
	for(int streamIndex=0; streamIndex<m_RTSPClientEngine->MediaStreamCount(); ++streamIndex)
	{
		CMediaStream * pMediaStream = m_RTSPClientEngine->MediaStream(streamIndex);
		
		if(pMediaStream->HaveBufferedMediaFrameCount() > 0)
		{
			return false;
		}
	}

	CLog::Log.MakeLog(LL_BUF_STATUS,"buffering.txt","###No Frame in Buffer,Rebuffering!\n");

	return true;
}
void CRTSPSourceEX::RetrieveAllPacketsFromRTPBuffer()
{
	CAutoCriticalSec cAutoLock(m_csSessionLock);

	for(int i=0; i<m_nRTSPTrackNum; ++i)
	{
		m_rtspTracks[i]->RetrieveAllPacketsFromRTPBuffer();
	}
}
bool CRTSPSourceEX::IsOneBufferEmptyWhenPlaying()
{
	static int isChecking=false;//sync lock
	bool	result=false;

	if(!isChecking)//CUserOptions::UserOptions.m_nEnableNullAudFrameWhenPacketloss)
	{
		isChecking=true;
		for(int i=0; i<m_nRTSPTrackNum; i++)
		{
			CMediaStream * pMediaStream = m_RTSPClientEngine->MediaStream(i);
			if(pMediaStream)//->IsAudioStream())
			{
				
				if(pMediaStream->HaveBufferedMediaFrameCount() <2
					&&(m_rtspTracks[i]->GetTrackDuration()==0||m_rtspTracks[i]->GetTrackDuration()-m_rtspTracks[i]->GetTrackPlayPos()>15*1000))
				{
					sprintf(CLog::formatString,"###IsOneBufferEmptyWhenPlaying(left %d)!,isAudio=%d,len=%ld,pos=%ld\n",
						pMediaStream->HaveBufferedMediaFrameCount(),
						pMediaStream->IsAudioStream(),
						m_rtspTracks[i]->GetTrackDuration(),
						m_rtspTracks[i]->GetTrackPlayPos());
					CLog::Log.MakeLog(LL_BUF_STATUS,"buffering.txt",CLog::formatString);
					result=true;
					break;
				}

			}
			
		}
		isChecking=false;
	}
	return result;
}
bool CRTSPSourceEX::IsAudioBufferEmpty()
{
	for(int i=0; i<m_nRTSPTrackNum; i++)
	{
		if(m_rtspTracks[i]->GetMediaStream()->IsAudioStream())
		{
			if(m_rtspTracks[i]->GetMediaStream()->HaveBufferedMediaFrameCount()==0)
			{
				SIMPLE_LOG("buffering.txt","AudioBuffer Empty\n");
				return true;
			}
		}
	}
	
	return false;
}
bool CRTSPSourceEX::IsRebufferingNeeded()
{	
	return false;
	if(CheckDataEnd())
		return false;
	else if(IsBothBufferEmpty()
		||IsAudioBufferEmpty()//this is a option, in most of cases, we prefer bothempty
		)
	{
		if(!m_bNeedBuffering)
		{

			EnableBuffering(true);
		}

	}	
	return true;
}
//some server may not support scan well,check it,if failed,disable the scan
bool CRTSPSourceEX::CheckScanSupport()
{
	float duration = m_RTSPClientEngine->StreamingDuration();
	float begin = duration - 20.0;
	float end   = begin+0.3;
	bool result = m_RTSPClientEngine->PlayCmd(begin,end);
	IVOThread::Sleep(500);//sleep 0.5 second
	return result;
}
int ShowWinInfo(char* info,int handle=NULL);
bool CRTSPSourceEX::StartupRTSPSession(DWORD dwStartTime)
{
	do
	{
		ShutdownRTSPSession();
		
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$StartupRTSPSession begine@@@@\n");
		CUserOptions::UserOptions.m_exit = false;

		if(!m_RTSPClientEngine->DescribeCmd(m_rtspURL)||CUserOptions::UserOptions.eventCode==VO_EVENT_DESCRIBE_FAIL)
		{
			CUserOptions::UserOptions.eventCode = 0;
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@describe fail@@@@\n");
			break;
		}
		
#if TEST_DIMS//
		InitRTSPTracks();
		m_bPermitSeeking=false;
		SetSessionStatus(Session_Playing);
		return true;//TEST_DIMS
#endif//
#if TRACE_FLOW
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$describe ok@@@@\n");
#endif//TRACE_FLOW
		if(!m_RTSPClientEngine->SetupCmd(m_bStreamUsingTCP))
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@setup fail@@@@\n");
			bool setupOK=false;
			if(CUserOptions::UserOptions.errorID==E_UNSUPPORTED_TRANSPORT)//try again
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@setup try again\n");
				CUserOptions::UserOptions.errorID=0;
				
				setupOK=m_RTSPClientEngine->ReSetupCmd(m_bStreamUsingTCP);
			}
			if(!setupOK)
			{
				NotifyEvent(VO_EVENT_SETUP_FAIL,CUserOptions::UserOptions.errorID);
				break;
			
			}
		}
		
		
		int duaration = m_RTSPClientEngine->StreamingDuration();
		m_bPermitSeeking = (duaration != 0);
		CUserOptions::UserOptions.m_isLiveStream = !m_bPermitSeeking;

		sprintf(CLog::formatString,"Is Live Stream=%d\n", CUserOptions::UserOptions.m_isLiveStream);
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);

		CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
#if !TEST_ONE_TRACK
		if(CUserOptions::UserOptions.m_bMakeLog<LOG_DATA)
			if(m_RTSPClientEngine->MediaStreamCount()>1)
				pRTSPMediaStreamSyncEngine->EnableWaitIFrame(true);//m_bPermitSeeking==false);//waitting for I frame is only for live mode
#endif//
		if(m_bPermitSeeking)
			pRTSPMediaStreamSyncEngine->SetMaxWaitFrames(50);
		else
			pRTSPMediaStreamSyncEngine->SetMaxWaitFrames(50);
#define  CHECK_MAX_DURATION 1800 //seconds
		if(0)//m_bPermitSeeking&&duaration>CHECK_MAX_DURATION)
		{
			//if(CheckScanSupport()==false)
			{
				NotifyEvent(HS_EVENT_DISABLE_SCAN,1);
			}
		}
		if(CUserOptions::UserOptions.streamType==ST_REAL)
		{
			m_RTSPClientEngine->Set_Parameter_Cmd(SPT_REAL_STREAM_RULE,NULL);
			//m_RTSPClientEngine->Set_Parameter_Cmd(SPT_REAL_STREAM_BANDWIDTH,NULL);
		}
		
		if(!InitRTSPTracks())
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@InitRTSPTracks fail@@@@\n");
			break;
		}
		
		if(!m_RTSPClientEngine->PlayCmd(CUserOptions::UserOptions.initPlayPos/1000))//,254.489
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@Play fail@@@@\n");
			break;
		}

		if(CUserOptions::UserOptions.initPlayPos)
		{
			m_playRespForSeek = true;
		}

		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$Wait I frame@@@@\n");
		//m_scaleBufferTime = 5000;//set the scale time for first buffering that needs to wait for the I frame

		if(CUserOptions::UserOptions.m_exit)
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","exit! user cancel it\n");
			break;
		}
#if TRACE_FLOW
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$StartupRTSPSession end@@@@\n");
#endif//TRACE_FLOW
		
		SetSessionStatus(Session_Playing);
		return true;

	}while(1);

	ShutdownRTSPSession();

	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@StartupRTSPSession fail@@@@\n");
	return false;
}

bool CRTSPSourceEX::PauseRTSPSession(bool forcePause)
{
	RemovePauseTask();

	if(m_sessionStatus==Session_NewPos)
	{
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$Pause: Session_NewPos\n");
		return true;
	}

	if(m_sessionStatus == Session_Paused||(m_bNeedBuffering&&CUserOptions::UserOptions.forcePause==false))
		return true;

	if(m_RTSPClientEngine->PauseCmd())
	{
		//m_sessionStatus = Session_Paused;
		SetSessionStatus(Session_Paused);
		return true;
	}
	else
	{
		if(CUserOptions::UserOptions.m_CheckSessionAlive)
		{
			SetSessionStatus(Session_PauseFail);
			m_RTSPClientEngine->FlushRTSPStreams(FLUSH_RTPBUFFER|FLUSH_H264TEMPBUFFER);
		}
	}
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@PauseRTSPSessionfail@@@@\n");
	return false;
}

bool CRTSPSourceEX::PlayRTSPSession()
{

	if(m_sessionStatus == Session_Playing)
		return true;

	m_playRespForSeek = false;

	if(m_RTSPClientEngine->PlayCmd((float)-1.000))
	{
		SetSessionStatus(Session_Playing);
		//some server resume from the beginning if client pause at the end
		//We take it as error
		if(m_sourceEnd)
		{
			if(m_RTSPClientEngine->GetPlayRangeBegin()<1)
			{
				CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt","error:server resume from the beginning if client pause at the end\n");
				CUserOptions::UserOptions.errorID=E_DATA_INACTIVITY;
				NotifyEvent(VO_EVENT_PLAY_FAIL,CUserOptions::UserOptions.errorID);
			}
		}
		AdaptToRefClock();
		return true;

	}
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@PlayRTSPSessionfail@@@@\n");
	return false;
}
void CRTSPSourceEX::AdaptToRefClock()
{
#if 1
	int refClock;
	NotifyEvent(HS_EVENT_GET_REFCLOCK,(int)(&refClock));
	int playPos=0;
	int curPos=0;
	for(int i=0; i<m_nRTSPTrackNum; ++i)
	{
		playPos=max(m_rtspTracks[i]->GetTrackPlayPos(),playPos);
		curPos=max(m_rtspTracks[i]->GetTrackPlayPos()-m_rtspTracks[i]->GetStartTime(),curPos);
	}
	int diff=abs(refClock-curPos);

#define MIN_ADJUST_REF  1000
#define MAX_ADJUST_REF  1000*1000
	if(playPos>0&&diff>MIN_ADJUST_REF&&MAX_ADJUST_REF<MAX_ADJUST_REF)
	{
		sprintf(CLog::formatString,"AdaptToRefClock:playPos=%d,refClock=%d,curPos=%d\n",playPos,refClock,curPos);
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
		m_livePauseTime = -1;
	}
#endif
}
int	 CRTSPSourceEX::SetDiffTimeInPlayResp(float dwStart)
{
	if(m_playRespForSeek)
	{
		for(int i=0; i<m_nRTSPTrackNum; ++i)
		{
			m_rtspTracks[i]->SetDiffTimeInPlayResp(dwStart);	
		}
	}
	return 0;
}
bool CRTSPSourceEX::PlayRTSPSession(DWORD dwStartTime)
{

	if(m_sessionStatus == Session_Playing)
		return true;

	m_playRespForSeek = true;

	float fStartTime = (float)dwStartTime / (float)1000;
	if(m_RTSPClientEngine->PlayCmd(fStartTime))
	{
		//m_sessionStatus = Session_Playing;
		SetSessionStatus(Session_Playing);
		AdaptToRefClock();
		return true;
	}
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@PlayRTSPSessionfail@@@@\n");
	return false;
}

void CRTSPSourceEX::ShutdownRTSPSession()
{
	if(m_sessionStatus == Session_Shutdown)
		return;

	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","@@@@ShutdownRTSPSession() !!!\n");
	CUserOptions::UserOptions.m_exit = true;
	for(int index=0; index<m_nRTSPTrackNum; ++index)
	{
		CRTSPTrack * pRTSPTrack = m_rtspTracks[index];
		if(pRTSPTrack)
			pRTSPTrack->ResetTrack();
	}
	m_RTSPClientEngine->TeardownCmd();
	m_RTSPClientEngine->Reset();
	//m_sessionStatus = Session_Shutdown;
	SetSessionStatus(Session_Shutdown);
	
}


DWORD CRTSPSourceEX::RecvRTSPStreamsProc(LPVOID pParam)
{
	CRTSPSourceEX * pRTSPSource = (CRTSPSourceEX *)pParam;
	pRTSPSource->RecvRTSPStreams();
	return 0;
}
void CRTSPSourceEX::HandleCrash(){
	NotifyEvent(VO_EVENT_SOCKET_ERR,1234);
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "****************ReadNetData****************\n");
	m_bStopReceivingData=true;m_bExit=true;
	CLog::Log.MakeLog(LL_RTP_ERR,"error.txt","crash exit\n");
}
DWORD CRTSPSourceEX::RecvRTSPStreams(void)
{
	int ret = -1;
	int timeout_num = 0;
	m_bRTPTimeOut = false;
	m_beginTime=timeGetTime();

	while((!m_bStopReceivingData)&&(!m_bExit))//if dumping data,do not exit
	{
		IVOThread::Sleep(0);
		if(m_bNeedBuffering)//
		{
			if(IsPrebufferingCompleted())
			{
				m_bNeedBuffering = false;
				m_dwStartBufferingTime = 0;
				if(!m_isBegining)//if it is the beginning,it may try the TCP alternatively
				{
					NotifyEvent(VO_EVENT_BUFFERING_PERCENT,100);
					NotifyEvent(VO_EVENT_BUFFERING_END,0);
					CUserOptions::UserOptions.isBuffering = false;
					CLog::Log.MakeLog(LL_BUF_STATUS,"buffering.txt","!!!preBuffering Completed\n");
				}
			}
		}
		if(m_sessionStatus!=Session_PauseFail)//CUserOptions::UserOptions.debugFlag!=VOID_DEBUG_PAUSEFAIL)//!m_livePaused)
		{
			CAutoCriticalSec cAutoLock(m_csSessionLock);
			if(m_bStreamUsingTCP)
				m_isBegining = false;
			int timeout_set= m_isBegining&&CUserOptions::UserOptions.m_useTCP>=0&&(!m_bStreamUsingTCP)?CUserOptions::UserOptions.m_nRTPTimeOut/2:CUserOptions::UserOptions.m_nRTPTimeOut;

			if(m_bPermitSeeking&&CheckDataEnd(GetExpectBufferingTime()))
				RetrieveAllPacketsFromRTPBuffer();
			
			ret = m_RTSPClientEngine->ReadRTSPStreams();

			//check whether the play response is received
 			if(ret ==SOCKET_ERROR) // socket error
			{
				CUserOptions::UserOptions.errorID = E_DATA_INACTIVITY;
				NotifyEvent(VO_EVENT_SOCKET_ERR,CUserOptions::UserOptions.errorID);
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "****************RecvRTSPStreams(SOCKET_ERROR)****************\n");
				m_bStopReceivingData  = true;
				sprintf(CLog::formatString,"@@@@socket error=%d happen or the connection is closed,stop it!\n",::GetSocketError());
				CLog::Log.MakeLog(LL_SOCKET_ERR,"socketErr.txt",CLog::formatString);
				break;
			}
			else if(ret == 0) // time out,or the connection is closed
			{
				if(m_sessionStatus == Session_Playing)
				{
					//++timeout_num;
					timeout_num=(timeGetTime()-m_beginTime)/1000;

					/*
					if(m_bPermitSeeking && !IsDataEnd((CUserOptions::UserOptions.m_nMediaBuffTime+3)*1000)&&timeout_num>CUserOptions::UserOptions.m_nMediaBuffTime)
					{
						CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","Do not receive data at last, quit!\n");
						SetSourceEnd(true);
						break;
					}
					*/
					if(CUserOptions::UserOptions.endOfStreamFromServer == E_CONNECTION_GOODBYE&&
						!IsDataEnd((CUserOptions::UserOptions.m_nMediaBuffTime+3)*1000)&&timeout_num>CUserOptions::UserOptions.m_nMediaBuffTime)
					{
						//NotifyEvent(VO_EVENT_PLAY_FAIL,E_CONNECTION_GOODBYE);
						CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","Goodbye received, quit!\n");
						SetSourceEnd(true);
					}
					if(timeout_num>timeout_set)
					{
						if(IsBothBufferEmpty())//if there is data in buffer,reset the timeout_num
						{
							//if((!m_bStreamUsingTCP)&&CUserOptions::UserOptions.m_useTCP>=0)		//change to TCP mode if time out	doncy 1214
							if(m_isBegining&& (!m_bStreamUsingTCP)&&CUserOptions::UserOptions.m_useTCP>=0)
							{
								m_bStreamUsingTCP = true;
								CLog::Log.MakeLog(LL_SOCKET_ERR,"tcp.txt","$$$$$UDP can not work,try TCP\n");
								//StartupRTSPSession(0);
								m_bRestart = true;
								
								SetTransportProtocol(m_bStreamUsingTCP);
								if(CUserOptions::UserOptions.m_URLType==UT_SDP_URL)
								{
									m_RTSPClientEngine->SetSDPFileString(m_sdpFileString);
								}
								m_restartThread=VOCPFactory::CreateOneThread();TRACE_NEW("m_restartThread",m_restartThread);
								m_restartThread->Create(m_restartFuncCMD);
								break;//exit the socket thread;
							}
							else
							{
								CUserOptions::UserOptions.errorID = E_DATA_INACTIVITY;
								NotifyEvent(VO_EVENT_SOCKET_ERR,CUserOptions::UserOptions.errorID);
								CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "****************RecvRTSPStreams2222****************\n");
								m_bStopReceivingData  = true;
								sprintf(CLog::formatString,"$$$$$RTP timeout(%d)!!!!!,exit\n",CUserOptions::UserOptions.m_nRTPTimeOut);
								CLog::Log.MakeLog(LL_SOCKET_ERR,"socket_timeout.txt",CLog::formatString);
								break;
							}
						
						}
						else
						{
								CUserOptions::UserOptions.errorID = E_DATA_INACTIVITY;
								NotifyEvent(VO_EVENT_SOCKET_ERR,CUserOptions::UserOptions.errorID);
								CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "****************RecvRTSPStreams****************\n");
								m_bStopReceivingData  = true;
								sprintf(CLog::formatString,"$$$$$isBuffering timeout(%d=%d-%d)!!!!!,exit\n",CUserOptions::UserOptions.m_nRTPTimeOut,timeGetTime(),m_beginTime);
								CLog::Log.MakeLog(LL_SOCKET_ERR,"socket_timeout.txt",CLog::formatString);
								break;
						}
						//m_bRTPTimeOut = true;
						
					}
					
					
				}

			}
			else // recv data
			{
				if((ret&PACKET_RTP)==PACKET_RTP)//||m_bStreamUsingTCP)
				{
					timeout_num = 0;
					m_beginTime=timeGetTime();
					m_bRTPTimeOut = false;
					m_isBegining = false;
				}
			}
		}
		
	}

	sprintf(CLog::formatString,"$$$$$socket exit,begin=%d\n",m_beginTime);
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
	m_bExit = true;	
	m_dwThreadId = 0;
	return ret;
}
void CRTSPSourceEX::SetTransportProtocol(bool isTCP)
{
	CUserOptions::UserOptions.m_useTCP = m_bStreamUsingTCP=isTCP;
}

bool CRTSPSourceEX::InitRTSPTracks()
{
	CMediaStream * pMediaStream = NULL;
	CRTSPTrack * pRTSPTrack = NULL;
	
	if(m_nRTSPTrackNum == 0)
	{
		for(int nIndex=0; nIndex<m_RTSPClientEngine->MediaStreamCount(); ++nIndex)
		{
			pMediaStream = m_RTSPClientEngine->MediaStream(nIndex);
			pRTSPTrack = new CRTSPTrack(this, m_RTSPClientEngine);TRACE_NEW("rtss_pRTSPTrack",pRTSPTrack);
			if(pRTSPTrack == NULL)
				return false;

			pRTSPTrack->SetTrackName(pMediaStream->MediaName());
			pRTSPTrack->SetMediaStream(pMediaStream);
			m_rtspTracks[nIndex] = pRTSPTrack;
			++m_nRTSPTrackNum;
		}
	}

	for(int nTrackIndex=0; nTrackIndex<m_nRTSPTrackNum; ++nTrackIndex)
	{
		pRTSPTrack = m_rtspTracks[nTrackIndex];
		for(int nStreamIndex=0; nStreamIndex<m_RTSPClientEngine->MediaStreamCount(); ++nStreamIndex)
		{
			pMediaStream = m_RTSPClientEngine->MediaStream(nStreamIndex);
			if(_stricmp(pRTSPTrack->GetTrackName(), pMediaStream->MediaName()) == 0)
			{
				pRTSPTrack->ResetTrack();
				pRTSPTrack->SetMediaStream(pMediaStream);
				break;
			}
		}
	}

	return true;
}
bool  CRTSPSourceEX::GetRTSPLinkFromSDPLink(const char * pSDPFilePath)
{
	
		char* rtspURL=(char*)pSDPFilePath;
		char* requestContent;
		IVOSocket* httpSock=INVALID_SOCKET;
		do
		{
			const char * protocolName = "http://";
			unsigned protocolNameLen = 7;
			if (_strnicmp(rtspURL, protocolName, protocolNameLen) != 0) 
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "invalid url syntax3\r\n");
				CUserOptions::UserOptions.errorID = E_HOST_UNKNOWN;
				break;
			}
			char * p = rtspURL + protocolNameLen;
			char * p1 = strstr(p, "/");
			if(p1 == NULL )
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "invalid url syntax23\r\n");
				CUserOptions::UserOptions.errorID = E_HOST_UNKNOWN;
				break;
			}
			requestContent = p1;
			// parse streaming server address
			char httpServerHostName[MAX_PATH] = {0};
			char * to = httpServerHostName;
			while(p < p1)
			{
				if(*p == ':') 
					break;

				*to++ = *p++;
			}
			*to = '\0';

			// parse streaming server port
			//unsigned short serverPort = 80;
			//unsigned short serverPort = 32000;
			unsigned short serverPort =  CUserOptions::UserOptions.m_rtspOverHttpConnectionPort;
			int port;
			if(*p == ':')                 
			{

				if(sscanf(++p, "%u", &port) != 1)
				{
					CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "can not parse port\r\n");
					CUserOptions::UserOptions.errorID =E_PORT_ALLOC;
					break;
				}
				if(port < 1 || port > 65535) 
				{
					CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", "invalid port\r\n");
					CUserOptions::UserOptions.errorID =E_PORT_ALLOC;
					break;
				}
				serverPort = (unsigned short )port;
			}
			voSocketAddress httpServerAddr2={0};
			voSocketAddress* httpServerAddr=&httpServerAddr2;
			httpServerAddr->family = VAF_UNSPEC;
			httpServerAddr->port   = HostToNetShort(serverPort);
			httpServerAddr->protocol = VPT_TCP;
			httpServerAddr->sockType = VST_SOCK_STREAM;
			int ret=IVOSocket::GetPeerAddressByURL(httpServerHostName,strlen(httpServerHostName),httpServerAddr);
			if(ret)
			{
				SLOG1(LL_SOCKET_ERR, "socketErr.txt", "getaddrinfo fail_err=%d\r\n",IVOSocket::GetSockLastError());
				CUserOptions::UserOptions.errorID = E_HOST_UNKNOWN;
				break;
			}
			httpSock = VOCPFactory::CreateOneSocket( VST_SOCK_STREAM,httpServerAddr->protocol,httpServerAddr->family);
			TRACE_NEW("httpSock",httpSock);
			if(httpSock==NULL||httpSock->IsValidSocket()==false)
			{
				sprintf(CLog::formatString," StreamSocket fail err=%d\n",GetSocketError());
				CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", CLog::formatString);
				break;
			}
			//CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", "CreateOneSocket\n");
			int timeout=CUserOptions::UserOptions.m_nConnectTimeOut;
			if(CUserOptions::UserOptions.m_bMakeLog)
			{
				sprintf(CLog::formatString,"Address:%d,%ld,%d,%d,%d,%d,%d\n",
					httpServerAddr->family,
					httpServerAddr->ipAddress,
					httpServerAddr->port,
					httpServerAddr->protocol,
					httpServerAddr->sockType,
					httpServerAddr->userData1,
					httpServerAddr->userData2);
				CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", CLog::formatString);
			}
			ret=0;
			do 
			{
 				ret=httpSock->Connect(*httpServerAddr,1000);
 
 				if(ret== SOCKET_ERROR) 
				{
					int err = GetSocketError();
					char* format = CLog::formatString;
					sprintf(format,"@@@@connect function fail err=%d\r\n",err);
					CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", format);
					CUserOptions::UserOptions.errorID = E_CONNECTION_RESET;
					break;
				}
				if(CUserOptions::UserOptions.m_exit)//user stop it
				{
					CLog::Log.MakeLog(LL_SOCKET_ERR, "rtsp.txt", "user stops the socket\n");
					timeout=0;
					break;//return false;
				}
			} while(--timeout>0&&ret==1);

			if(timeout==0||ret== SOCKET_ERROR)//timeout
			{
				
				char* format = CLog::formatString;
				sprintf(format,"@@@@connect function timeout=%d,%d,ret=%d,err=%d\r\n",timeout,CUserOptions::UserOptions.m_nRTSPTimeOut,ret,GetSocketError());
				CLog::Log.MakeLog(LL_SOCKET_ERR, "socketErr.txt", format);
				break;
			}
			CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", "GetRTSPLinkFromSDPLink connect Done\n");
			//
#define MAX_SDP_SIZE2  10240 //10K
			m_sdpFileString = new char[MAX_SDP_SIZE2];TRACE_NEW("rtss_m_sdpFileString2",m_sdpFileString);
			const char* request= "GET %s HTTP/1.1\r\n"
									"Accept: application/sdp,*/*\r\n"
									"Accept-Encoding: identity\r\n"
									"User-Agent: Mozilla/4.0.2009\r\n"
									"Host: %s:%d\r\n"
									"Connection: Keep-Alive\r\n"
									"\r\n";
			sprintf(m_sdpFileString,request,requestContent,httpServerHostName,serverPort);
			CLog::Log.MakeLog(LL_SOCKET_ERR, "rtsp.txt",m_sdpFileString);
			int size=strlen(m_sdpFileString);
			int sentSize=httpSock->Send(m_sdpFileString,size);
			if (sentSize!=size)
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", "sentSize!=size\n");
			}
			int max_wait_read = 10;
			while (!httpSock->Select(VSE_FD_READ,1000)&&--max_wait_read>0)
			{
			}
			IVOThread::Sleep(200);
			size=httpSock->Recv(m_sdpFileString,MAX_SDP_SIZE2-1);
			m_sdpFileString[size]='\0';
			CLog::Log.MakeLog(LL_SOCKET_ERR, "rtsp.txt",m_sdpFileString);
			if (size<=0||strstr(m_sdpFileString,"Content-Type: application/sdp")==NULL)
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR, "rtsp.txt","it is not sdp\n");
				break;
			}
			char _httpVer[32];
			long  statusCode=200;
			CLog::Log.MakeLog(LL_SOCKET_ERR, "flow.txt", "HTTP SDP is received\n");
			if(sscanf(m_sdpFileString, "%[^ \r\n]%ld", _httpVer, &statusCode) != 2)
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR, "rtsp.txt","it is not 200 code\n");
				break;
			}
			if(statusCode == 200 || statusCode == 206)
			{
				
				char* _attrLine = strstr(m_sdpFileString,"Content-Length:");
				int totalLen=0;
				if (_attrLine)
				{
					_attrLine += 14;
					if(1 != sscanf(_attrLine, ": %u", &totalLen))
					{	
					
					}
				}
				
				//TODO:the flexible way is to get possible unreached data by content length
				
				char* sdp=strstr(m_sdpFileString,"\r\n\r\n");
				
				if (sdp==NULL)
				{
					CLog::Log.MakeLog(LL_SOCKET_ERR, "rtsp.txt","there is no sdp\n");
					break;
				}
				sdp+=4;
				int sdpLen = size - (sdp - m_sdpFileString);
				SLOG3(LL_SOCKET_ERR, "flow.txt", "Response Size=%d,SDP size=%d,total=%d\n",size,sdpLen,totalLen);
				memmove(m_sdpFileString,sdp,sdpLen);
				while(sdpLen<totalLen)
				{
					size=httpSock->Recv(m_sdpFileString+sdpLen,MAX_SDP_SIZE2-1);
					sdpLen+=size;
					SLOG2(LL_SOCKET_ERR, "flow.txt", "Get next fragment: Size=%d,SDP size=%d\n",size,sdpLen);
				}
				m_sdpFileString[sdpLen-1]='\0';
				char*  pRTSPLink=strstr(m_sdpFileString,"rtsp://");
				if (pRTSPLink==NULL)
				{
					pRTSPLink=strstr(m_sdpFileString,"RTSP://");
					if (pRTSPLink==NULL)
					{
						CLog::Log.MakeLog(LL_SOCKET_ERR, "rtsp.txt","there is no rtsp link\n");
						break;
					}
				}
				if(sscanf(pRTSPLink, "%1000s[^\r\n]", m_rtspURL) != 1)
					//if(_sscanf_l(pRTSPLink, "%[^\r\n]", m_rtspURL) != 1)
				{
					m_rtspURL[1023]='\0';
					CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt",m_rtspURL);
					break;
				}
     			CUserOptions::UserOptions.sdpURLLength = strlen(m_rtspURL);
				CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt","$$$$$After Get the HTTP link@@@@\n");
			}
			else if(statusCode >= 300 && statusCode < 400)
			{
				SLOG1(LL_SOCKET_ERR,"rtsp.txt","redirect not supported retCode=%ld\n",statusCode);
			}
			else
			{
				break;
			}
			VOCPFactory::DeleteOneSocket(httpSock);TRACE_DELETE(httpSock);
			httpSock=INVALID_SOCKET;
			return true;

		}while(0);
		if(httpSock!=INVALID_SOCKET)
		{
			VOCPFactory::DeleteOneSocket(httpSock);TRACE_DELETE(httpSock);
			httpSock=INVALID_SOCKET;
		}
	
		CLog::Log.MakeLog(LL_SOCKET_ERR, "rtsp.txt", "@@@@@GetRTSPLinkFromSDPLink fail\r\n");
		return false;
}
bool CRTSPSourceEX::GetRTSPLinkFromSDPFile(const char * pSDPFilePath)
{
	FILE * pSDPFile = NULL;
	char * pSDPData = NULL;
	bool  isValid = true;
	int fileSize;
	do
	{
		pSDPFile = fopen(pSDPFilePath, "rb");
		if(pSDPFile == NULL)
		{
			isValid = false;
			break;
		}
		else
		{
//somehow,sometimes the filesize in this way has some issues,so constraint the size between[4K,64K]
#define MAX_SDP_SIZE 64*1024
#define MIN_SDP_SIZE 4*1024
			fseek( pSDPFile, 0L, SEEK_END );
			fileSize = ftell(pSDPFile);
			fileSize*=2;
			if(fileSize<MIN_SDP_SIZE)
				fileSize = MIN_SDP_SIZE;
			if(fileSize>MAX_SDP_SIZE )
				fileSize=MAX_SDP_SIZE ;
			fseek(pSDPFile,0,SEEK_SET);

		}
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt","$$$$$After open SDP file@@@@\n");
		if(m_sdpFileString)
			SAFE_DELETE_ARRAY(m_sdpFileString);

		m_sdpFileString = new char[fileSize];TRACE_NEW("rtss_m_sdpFileString",m_sdpFileString);
#if 1
		pSDPData = m_sdpFileString;
		if(pSDPData == NULL)
		{
			isValid = false;
			break;
		}
		int sdpDataSize = fread(pSDPData, 1, fileSize, pSDPFile);
		pSDPData[sdpDataSize] = '\0';
		sprintf(CLog::formatString,"file size=%d,bufferSize=%d\n",sdpDataSize,fileSize);
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt",CLog::formatString);
		char * pRTSPLink = strstr(pSDPData, "rtsp://");
		if(pRTSPLink == NULL)
		{
			pRTSPLink = strstr(pSDPData, "RTSP://");
			if(pRTSPLink==NULL)
			{
				isValid = false;
				break;
			}
		
		}
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt","$$$$$Get the link@@@@\n");
	
		if(sscanf(pRTSPLink, "%1000s[^\r\n]", m_rtspURL) != 1)
		//if(_sscanf_l(pRTSPLink, "%[^\r\n]", m_rtspURL) != 1)
		{
			m_rtspURL[1023]='\0';
			CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt",m_rtspURL);
			isValid = false;
			break;
		}
#else
		
		strcpy(m_rtspURL, "rtsp://ug.rtsp-youtube.l.google.com/CiULENy73wIaHAnA_H4Bw7o54BMYDSANFEIJbXYtZ29vZ2xlSAQM/0/0/0/video.3gp");
#endif
		
		CUserOptions::UserOptions.sdpURLLength = strlen(m_rtspURL);
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt","$$$$$After Get the link@@@@\n");

	}while(0);
	if(pSDPFile != NULL)
		fclose(pSDPFile);
	CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt","$$$$$close file@@@@\n");
	
	return isValid;
}

VOFILEREADRETURNCODE CRTSPSourceEX::SetParameter(LONG lID, LONG value1)
{
#define SDK_VERSION_CHECK	\
	long		lValue=value1;\
	if(CUserOptions::UserOptions.sdkVersion)\
		lValue=*((long*)value1);

	switch(lID)
	{
	case VOID_STREAMING_SDK_VERSION:
		if((value1&0x0fffffff)>10)
		{
			CUserOptions::UserOptions.sdkVersion = *((long*)value1);
		}
		else
			CUserOptions::UserOptions.sdkVersion =value1;	
		return VORC_FILEREAD_OK;
	case VOID_FILEREAD_ACTION_STOP:
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","!!!filter stop\n");
#if	DELAY_STOP
			if(m_bExit==false&&m_sessionStatus!=Session_Shutdown&&m_rtspTracks[0]->GetOutPutFrames()==0)
				SetStopTask(2);
			else
#endif
			if(!Stop())
				return VORC_FILEREAD_NEED_RETRY;

			return VORC_FILEREAD_OK;
		}
	case VOID_FILEREAD_ACTION_START:
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","!!!filter start\n");
			if(!Start())
				return VORC_FILEREAD_NEED_RETRY;

			return VORC_FILEREAD_OK;
		}			
	case VOID_STREAMING_BUFFERING_PAUSE:
		{
			SDK_VERSION_CHECK
			if(lValue)
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$Pause when buffering@@@@\n");
				CUserOptions::UserOptions.forcePause= true;
				if(!Pause())
					return VORC_FILEREAD_NEED_RETRY;
				CUserOptions::UserOptions.forcePause= false;
			}
			else
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","$$$$$resume after buffering_pause@@@@\n");
				if(!Run())
					return VORC_FILEREAD_NEED_RETRY;
			}
			return VORC_FILEREAD_OK;
		}
	case VOID_FILEREAD_ACTION_PAUSE:
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","!!!filter pause\n");
            if(!Pause())
				return VORC_FILEREAD_NEED_RETRY;
            
			return VORC_FILEREAD_OK;
		}
	case VOID_STREAMING_FORCE_RUN:
	case VOID_FILEREAD_ACTION_RUN:
		{
			if(lID==VOID_STREAMING_FORCE_RUN)
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","!!!Force run\n");
			else
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","!!!filter run\n");

			if(!Run())
				return VORC_FILEREAD_NEED_RETRY;

			return VORC_FILEREAD_OK;
		}
	case VOID_FILEREAD_ACTION_SEEK:
		{
			SDK_VERSION_CHECK
			if(CUserOptions::UserOptions.m_bMakeLog)
			{
				sprintf(CLog::formatString,"!!!filter seek(%ld)\n",lValue);
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
			}
	
			//m_RTSPClientEngine->FlushRTSPStreams();
			//EnableBuffering(true);
			//m_bNeedBuffering=true;
			SetSeekTask(0.2,lValue);
			m_prePercent=-1;

			return VORC_FILEREAD_OK;
		}
	case VOID_STREAMING_INITPLAY_POS:
		{
			SDK_VERSION_CHECK
			CUserOptions::UserOptions.initPlayPos = lValue;
			return VORC_FILEREAD_OK;
		}
	case VOID_FILEREAD_CALLBACK_NOTIFY:
		{
			NotifyEventFunc* value= (NotifyEventFunc*)value1;
			if(value==NULL)
				break;
			CUserOptions::UserOptions.notifyFunc.funtcion = value->funtcion;
			CUserOptions::UserOptions.notifyFunc.parent	 = value->parent;
			return VORC_FILEREAD_OK;
		}
	case VOID_STREAMING_FORCE_STOP:
		{
			m_bStopReceivingData = true;	
			m_bExit = true;
			CUserOptions::UserOptions.m_exit = true;
			
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","____________Force Stop_________________\n");
			IVOThread::Sleep (0);
            //Stop();
			return VORC_FILEREAD_OK;
		}
	case VOID_STREAMING_INIT_PARAM:
		{
			VOStreamInitParam* params = (VOStreamInitParam*)value1;
			if(params==NULL)
				break;
			CUserOptions::UserOptions.m_bMakeLog		= params->nSDKlogflag;
			if(CUserOptions::UserOptions.m_bMakeLog&0x00f0)
				CUserOptions::UserOptions.m_nAppendMode = 1;
			
			if((CUserOptions::UserOptions.m_bMakeLog&0x0f)<2)
				CUserOptions::UserOptions.m_bMakeLog		= 2;
			
			CUserOptions::UserOptions.m_nMediaBuffTime	= params->nBufferTime;

			if(params->nRTSPTimeout>0)
				CUserOptions::UserOptions.m_nRTSPTimeOut	= params->nRTSPTimeout;
			if(params->nRTPTimeOut)
				CUserOptions::UserOptions.m_nRTPTimeOut		= params->nRTPTimeOut;
			if(params->nConnectTimeout)
				CUserOptions::UserOptions.m_nConnectTimeOut = params->nConnectTimeout;
			if(params->nMinUdpPort>=1024)
				CUserOptions::UserOptions.m_nUDPPortFrom	= params->nMinUdpPort;
			if(params->nMaxUdpPort>params->nMinUdpPort+4&&params->nMaxUdpPort<65535)
				CUserOptions::UserOptions.m_nUDPPortTo		= params->nMaxUdpPort;
			CUserOptions::UserOptions.m_useTCP			= params->nUseTCP;
			//if(params->nRTCPInterval>1000)
			CUserOptions::UserOptions.m_nRTCPInterval   = params->nRTCPInterval;
			if(params->nUseTCP>0)
			{
				SetTransportProtocol(true);
			}
			CUserOptions::UserOptions.m_bUseRTSPProxy			= params->bUseRTSPProxy==1;
			if(params->bUseRTSPProxy)
			{
				strcpy(CUserOptions::UserOptions.m_szProxyIP,params->szProxyIP);
			}
			CUserOptions::UserOptions.m_nFirstIFrame	= params->nFirstIFrame;
			if(CUserOptions::UserOptions.m_n3GPPAdaptive==0)
			CUserOptions::UserOptions.m_n3GPPAdaptive	= params->nEnable3GPPRateAdaptive;
			CUserOptions::UserOptions.m_nPortOpener		= params->nExtraRTPPacket;
			CUserOptions::UserOptions.m_nBandWidth		= params->nBandWidth;
			CUserOptions::UserOptions.m_nBufferLow		= params->nBufferLow;
			//it is safer to use memcpy than to use strcpy
			if(strlen(params->szUserAgent)>3)
			{
				memcpy(CUserOptions::UserOptions.m_sUserAgent,(char*)params->szUserAgent,MAX_PATH);
			}
			if(strlen(params->szProfile)>3)
			{
				memcpy(CUserOptions::UserOptions.m_sUAProfile,(char*)(params->szProfile),MAX_PATH);
				//printf("%s\n",params->szProfile);
			}
			//SLOG1(LL_RTSP_ERR,"flow.txt","be careful the ExtraFields=%d\n",params->bUseExtraFields);
			if(params->bUseExtraFields==1)
			{
				//CUserOptions::UserOptions.m_extraFields =strdup(params->szExtraFields); //contented by doncy 0813
#if defined	UNDER_CE
				CUserOptions::UserOptions.m_extraFields =_strdup(params->szExtraFields);
#else
				CUserOptions::UserOptions.m_extraFields =strdup(params->szExtraFields);
#endif	//UNDER_CE
				//SLOG1(LL_RTSP_ERR,"flow.txt","be careful the ExtraFields is set,%s\n",CUserOptions::UserOptions.m_extraFields);
			}

			PDigestAuthParam digestParam = params->digestAuth;

			if(digestParam)
			{
				sprintf(CLog::formatString,"Digest User name: %s, len %d; passwd %s, len %d\n",
							digestParam->user,digestParam->userLen,digestParam->passwd,digestParam->passwdLen);
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
				
				if((!digestParam->passwdLen) || (!digestParam->userLen) || (!digestParam->user) || (!digestParam->passwd))
				{
					sprintf(CLog::formatString,"Invalid digest user name and password, ignore\n");
					CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
				}
				else
				{
					CUserOptions::UserOptions.digestIsSet = true;
					CUserOptions::UserOptions.digestAuth.passwdLen = digestParam->passwdLen;
					CUserOptions::UserOptions.digestAuth.userLen = digestParam->userLen;

					memcpy(CUserOptions::UserOptions.digestAuth.user,digestParam->user,digestParam->userLen);
					memcpy(CUserOptions::UserOptions.digestAuth.passwd,digestParam->passwd,digestParam->passwdLen);
				}
			}
			return VORC_FILEREAD_OK;
		}
	case VOID_STREAMING_MINIMUM_LATENCY:
			VOLOGI("***************************uplayer has set latency param********************");
			CUserOptions::UserOptions.m_minimumLatency = true;
			return VORC_FILEREAD_OK;
				
	case VOID_STREAMING_ADD_RTSP_FIELD:
		{
			m_listRTSPFieldParam.push_back((RTSPFieldParam *)value1);
			return VORC_FILEREAD_OK;
		}		
	case VOID_STREAMING_NOT_RESET_FLAG:
		{
			CUserOptions::UserOptions.m_nResetLog = 0;
			break;
		}
	case VOID_STREAMING_TIMEMARKER:
		{
			int nTimerMarker = *((long*)value1);
			
			if(nTimerMarker >0 && nTimerMarker <=60)
				CUserOptions::UserOptions.m_nTimeMarker = nTimerMarker;
			else
				CUserOptions::UserOptions.m_nTimeMarker = 10; //10s is default value

			return VORC_FILEREAD_OK;
		}
	case VO_PID_SOURCE2_DOHTTPVERIFICATION:
		{
			VO_SOURCE2_VERIFICATIONINFO *info = (VO_SOURCE2_VERIFICATIONINFO *)value1;
			
			if(!info)
				return VORC_FILEREAD_NULL_POINTER;

			sprintf(CLog::formatString,"Digest info %s(%lu),flag %lu\n",(char *)info->pData,
					info->uDataSize,info->uDataFlag);
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
			
			if(info->uDataFlag == 1)
			{
				DigestAuthParam *digest = &CUserOptions::UserOptions.digestAuth;
				
				if(!ParseDigestInfo(info,digest))
				{
					sprintf(CLog::formatString,"Invalid digest info %s\n",(char *)info->pData);
					CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
					break; 
				}
					
				sprintf(CLog::formatString,"Digest User name: %s, len %d; passwd %s, len %d\n",
							digest->user,digest->userLen,digest->passwd,digest->passwdLen);
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
				
				if((!digest->passwdLen) || (!digest->userLen) || (!digest->user) || (!digest->passwd))
				{
					sprintf(CLog::formatString,"Invalid digest user name and password, ignore\n");
					CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
				}
				else
				{
					CUserOptions::UserOptions.digestIsSet = true;
				}
				
			}
			
			return VORC_FILEREAD_OK;
		}
			
	case VO_PID_SOURCE2_RTSPINITPARAM:
		{
			VO_SOURCE2_RTSP_INIT_PARAM *param = (VO_SOURCE2_RTSP_INIT_PARAM *)value1;

			if(!param)
				return VORC_FILEREAD_NULL_POINTER;

			sprintf(CLog::formatString,"voSource2:useTCP %lu, buffer time %lu , video connection port %lu , audio connection port %lu \n",param->uRTPChan,param->uBufferTime, param->uVideoConnectionPort, param->uAudioConnectionPort);
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
			
			if ((param->uVideoConnectionPort!=0)&&(param->uVideoConnectionPort==param->uAudioConnectionPort))
			{
				return (VOFILEREADRETURNCODE)VO_RET_SOURCE2_INVALIDPARAM;
			}

			if (param->uVideoConnectionPort>1024&&param->uVideoConnectionPort<65535)
			{
				CUserOptions::UserOptions.m_videoConnectionPort= param->uVideoConnectionPort;
			}
			else 
			{
				if (param->uVideoConnectionPort!=0)
				{
					return (VOFILEREADRETURNCODE)VO_RET_SOURCE2_INVALIDPARAM;
				}
			}

			if (param->uAudioConnectionPort>1024&&param->uAudioConnectionPort<65535)
			{
				CUserOptions::UserOptions.m_audioConnectionPort= param->uAudioConnectionPort;
			}
			else 
			{
				if (param->uAudioConnectionPort!=0)
				{
					return (VOFILEREADRETURNCODE)VO_RET_SOURCE2_INVALIDPARAM;
				}
			}

			sprintf(CLog::formatString,"RTSP+++++param->uRTPChan=%d\n",param->uRTPChan);
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);

			if(param->uRTPChan>0)
			{
				SetTransportProtocol(true);
			}
			
			CUserOptions::UserOptions.m_nMediaBuffTime = param->uBufferTime/1000;
			
			if(!CUserOptions::UserOptions.m_nMediaBuffTime)
				CUserOptions::UserOptions.m_nMediaBuffTime = 1;

			return VORC_FILEREAD_OK;	
		}

	case VO_PID_SOURCE2_ENABLE_RTSP_HTTP_TUNNELING:
		{
			CUserOptions::UserOptions.m_isOverHttpTunnel = *((int *)value1);
			if (CUserOptions::UserOptions.m_isOverHttpTunnel)
			{
				SetTransportProtocol(true);
			}
			return VORC_FILEREAD_OK;
		}

	case VO_PID_SOURCE2_RTSP_OVER_HTTP_CONNECTION_PORT:
		{
			CUserOptions::UserOptions.m_rtspOverHttpConnectionPort = *((int *)value1);
			return VORC_FILEREAD_OK;
		}

	default:
	    break;
	}
	return VORC_FILEREAD_NOT_IMPLEMENT;
}
bool CRTSPSourceEX::IsDataEnd(int limit)
{
	bool isEnd = false;
	for(int index=0; index<m_nRTSPTrackNum; ++index)
	{
		CRTSPTrack * pRTSPTrack = m_rtspTracks[index];
		if(pRTSPTrack==NULL)
		{
			VOLOGI("IsDataEnd()_____pRTSPTrack is NULL");
			break;
		}
		CMediaStream * pMediaStream = pRTSPTrack->GetMediaStream();
		if(pMediaStream==NULL)
		{
			VOLOGI("IsDataEnd()_____pMediaStream is NULL");
			break;
		}
		DWORD dwTrackDuration = pRTSPTrack->GetTrackDuration();
		DWORD dwCurrPos = pRTSPTrack->GetStartTime()+pMediaStream->GetLastTimeInBuffer();
		DWORD dwMaxTime = abs(dwTrackDuration-dwCurrPos);
		if(dwMaxTime<limit)//if it is need to the last 2 seconds,regard it as end
		{
			isEnd= true;
			break;
		}
	}
	return isEnd;
}
int CRTSPSourceEX::GetExpectBufferingTime()
{
	int time = CUserOptions::UserOptions.GetMediaBufferTime();

	return time;
}
bool CRTSPSourceEX::CheckNetStatus(int percent)
{
	return true;
}
bool CRTSPSourceEX::CheckDataEnd(int limit)
{
	if(m_sourceEnd)
		return true;

	bool isEnd = false;
	for(int index=0; index<m_nRTSPTrackNum; ++index)
	{
		CRTSPTrack * pRTSPTrack = m_rtspTracks[index];
		CMediaStream * pMediaStream = pRTSPTrack->GetMediaStream();
		if(pMediaStream==NULL)
			break;
		isEnd = pRTSPTrack->IsNearEnd(limit);
		if(isEnd)//if it is need to the last 2 seconds,regard it as end
		{
			SLOG1(LL_RTP_ERR,"buffering.txt","IsNearEnd_%d\n",limit);
			break;
		}
	}
	
	return isEnd;
}

int	CRTSPSourceEX:: GetBufPercent(int expectBufTimeBySec)
{
	int  isAddAntiJitterBuff = CheckDataEnd(8000);//at last 8 seconds, plus the antijitterBufTime
	long dwBufferedDuration = m_RTSPClientEngine->HaveBufferedRTSPStreamDuration(isAddAntiJitterBuff);
	long totalExpectBufTime = expectBufTimeBySec>0?expectBufTimeBySec*1000:GetExpectBufferingTime();
	long percent ;

	if(CUserOptions::UserOptions.m_minimumLatency)
		totalExpectBufTime = 500;
	
	if(CheckDataEnd(totalExpectBufTime))
	{
		totalExpectBufTime /= 5;

		if(totalExpectBufTime<1000)
			totalExpectBufTime = 1000;
	}
	
	percent = (LONG)(dwBufferedDuration*100 /totalExpectBufTime); 
	
	if(percent>100||CheckDataEnd())
		percent=100;

	return percent;
}
VOFILEREADRETURNCODE CRTSPSourceEX::GetParameter(LONG lID, LONG * plValue)
{
	switch(lID)
	{
	case VOID_COM_DURATION:
		{
			*plValue = (LONG)(m_RTSPClientEngine->StreamingDuration()*1000);
			return VORC_FILEREAD_OK;
		}
	case VOID_FILEREAD_TRACKS:
		{
			*plValue = (LONG)m_RTSPClientEngine->MediaStreamCount();
			return VORC_FILEREAD_OK;
		}
	case VOID_STREAMING_BUFFERING_PROGRESS:
	case VOID_FILEREAD_BUFFERING_PROGRESS:
	case HSID_STREAMING_BUFFERING_PROGRESS:
		{
			if(m_prePercent==-1)
			{
				*plValue = 0;
				return VORC_FILEREAD_OK;
			}

			//sprintf(CLog::formatString,"pre=%d,cur=%d\n",m_prePercent,m_bufferingPercent);
			//CLog::Log.MakeLog(LL_RTSP_ERR,"bufing2.txt",CLog::formatString);
			int percent =m_bufferingPercent;// GetBufPercentForUI();//GetBufPercent();
			if(percent<m_prePercent)//make sure that the user can see the increasing buf time
				percent = m_prePercent;
			//percent+=5;
			if(IS_FULL(percent))
			{
				*plValue=percent = 100;
				m_bufferingPercent = 0;
				m_prePercent=0;
			}
			else
			{

				*plValue = m_prePercent=percent;
			}
			return VORC_FILEREAD_OK;

		}
	case HSID_STREAMING_CURRENT_POS:
		{
			int posmax=0;
			int posmin=0x7ffff;
			//choose the longer track
			for(int i=0; i<m_nRTSPTrackNum; ++i)
			{
				int pos2=m_rtspTracks[i]->GetTrackPlayPos();
				//int trackDuration=m_rtspTracks[i]->GetTrackDuration();
				//if(trackDuration>0&&pos2>trackDuration)//avoid beyond the duration
				//	pos2=trackDuration;
				if(pos2<posmin)
					posmin = pos2;
				if(pos2>posmax)
					posmax = pos2;
			}
			int pos;
			if(m_sourceEnd)
			{
				pos = posmax;
				pos+= 500;//sometimes,the last several frames can not arrive on time,adjust it by 0.5s
			}
			else
			{
				pos = posmin;
			}
						
			*plValue=pos;
			return VORC_FILEREAD_OK;
		}
	case HSID_STREAMING_INFO:
		{
			HS_StreamingInfo* parms = (HS_StreamingInfo*)plValue;
			if(parms==NULL)
				break;
			//memcpy(parms,&(CUserOptions::UserOptions.outInfo),sizeof(VO_StreamingInfo));ß
			parms->clipType		= m_bPermitSeeking?ON_DEMAND:LIVE;
			parms->clip_title	= (m_RTSPClientEngine&& m_RTSPClientEngine->IsInited())?CUserOptions::UserOptions.outInfo.clip_title:(char*)"No Title2\n";
			parms->codecType[0]	= CUserOptions::UserOptions.outInfo.codecType[0];
			parms->codecType[1]	= CUserOptions::UserOptions.outInfo.codecType[1];
			parms->clip_bitrate = CUserOptions::UserOptions.outInfo.clip_bitrate;
			parms->clip_width	= CUserOptions::UserOptions.outInfo.clip_width;
			parms->clip_height  = CUserOptions::UserOptions.outInfo.clip_height;
			parms->clip_SupportPAUSE	= CUserOptions::UserOptions.m_nSupportPause;
			parms->clip_AvgFrameRate	= CUserOptions::UserOptions.outInfo.clip_AvgFrameRate;
			parms->clip_DownloadedSize = CUserOptions::UserOptions.outInfo.clip_DownloadedSize;
			parms->clip_audioChannels  = CUserOptions::UserOptions.outInfo.clip_audioChannels;
			parms->clip_audioSampleRate	= CUserOptions::UserOptions.outInfo.clip_audioSampleRate;
			parms->clip_videoBitRate	= CUserOptions::UserOptions.outInfo.clip_videoBitRate;
			parms->clip_audioBitRate	= CUserOptions::UserOptions.outInfo.clip_audioBitRate;
			sprintf(CLog::formatString,"\nStreamInfo..%dx%d,codec(%d,%d),bitrate=%d(%d+%d),title=%s,\nframerate=clip_AvgFrameRate=%d,clip_DownloadedSize=%ld,audio(%d,%d)\n",parms->clip_width,parms->clip_height,
				parms->codecType[0],
				parms->codecType[1],
				parms->clip_bitrate,
				parms->clip_videoBitRate,
				parms->clip_audioBitRate,
				parms->clip_title,
				parms->clip_AvgFrameRate,
				parms->clip_DownloadedSize,
				parms->clip_audioChannels,
				parms->clip_audioSampleRate);
			CLog::Log.MakeLog(LL_RTSP_ERR,"config.txt",CLog::formatString);
			return VORC_FILEREAD_OK;
		}
	case HSID_STREAMING_GET_RECEIVING_DATA_RATE:
		{
			if(m_RTSPClientEngine)
			{
				*plValue=m_RTSPClientEngine->GetActualBitrate();
			}
			else
			{
				*plValue=0;
			}
			return VORC_FILEREAD_OK;
		}
	case VOID_STREAMING_THROUGHPUT:
		{
			if(m_RTSPClientEngine)
			{
				*plValue=m_RTSPClientEngine->GetActualBitrate()/(8*1000);
			}
			else
			{
				*plValue=0;
			}
			return VORC_FILEREAD_OK;
		}
	case VOID_STREAMING_INFO:
		{
			VOStreamingInfo* parms = (VOStreamingInfo*)plValue;
			if(parms==NULL)
				break;
			//memcpy(parms,&(CUserOptions::UserOptions.outInfo),sizeof(VO_StreamingInfo));
			parms->clipType		= m_bPermitSeeking?ST_ON_DEMAND:ST_LIVE;
			parms->clip_title	= CUserOptions::UserOptions.outInfo.clip_title;
			parms->codecType[0]	= VOCodecType(CUserOptions::UserOptions.outInfo.codecType[0]);
			parms->codecType[1]	= VOCodecType(CUserOptions::UserOptions.outInfo.codecType[1]);
			parms->clip_bitrate = CUserOptions::UserOptions.outInfo.clip_bitrate;
			parms->clip_width	= CUserOptions::UserOptions.outInfo.clip_width;
			parms->clip_height  = CUserOptions::UserOptions.outInfo.clip_height;
			sprintf(CLog::formatString,"GetParam..%dx%d\n",parms->clip_width,parms->clip_height);
			CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt",CLog::formatString);
			return VORC_FILEREAD_OK;
		}
	case VOID_INNER_BUFFERING_TIME:
		{
			if(m_RTSPClientEngine)
				*plValue=m_RTSPClientEngine->HaveBufferedRTSPStreamDuration();
			else
				*plValue=0;
			return VORC_FILEREAD_OK; 
		}
		break;

	case VO_PID_SOURCE2_RTSP_STATS:
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "-----------------------VO_PID_SOURCE2_RTSP_STATS--------------------------\n");

			VO_SOURCE2_RTSP_STATS* statusInfo = (VO_SOURCE2_RTSP_STATS*)plValue;

			statusInfo->nTrackType = VO_SOURCE2_TT_VIDEO;
			statusInfo->uAverageJitter=0;
			statusInfo->uAverageLatency=0;
			statusInfo->uPacketDuplicated=0;
			statusInfo->uPacketLost=0;
			statusInfo->uPacketRecved=0;
			statusInfo->uPacketSent = 0;
			
			for(int i=0;i<m_nRTSPTrackNum;i++)
			{
				statusInfo->uAverageJitter=statusInfo->uAverageJitter +m_rtspTracks[i]->GetMediaStream()->RTCPParser()->m_sRReport.interarrivalJitter;
				statusInfo->uAverageLatency  =statusInfo->uAverageLatency  +m_rtspTracks[i]->GetMediaStream()->RTCPParser()->m_latencyAvg;
				statusInfo->uPacketLost =statusInfo->uPacketLost +m_rtspTracks[i]->GetMediaStream()->RTCPParser()->m_sRReport.cumulativeLost;
				statusInfo->uPacketDuplicated =statusInfo->uPacketDuplicated +m_rtspTracks[i]->GetMediaStream()->RTPParser()->GetDuplicatePacketNum();
				statusInfo->uPacketRecved =statusInfo->uPacketRecved +m_rtspTracks[i]->GetMediaStream()->RTCPParser()->m_sRReport.extendedHighestSeqReceived;
				statusInfo->uPacketSent =statusInfo->uPacketSent +m_rtspTracks[i]->GetMediaStream()->RTCPParser()->m_sSReport.packCnt;

				//sprintf(CLog::formatString,"RTSP session %d uAverageJitter, uAverageLatency %u, uPacketLost %u,uPacketDuplicated %u,uPacketRecved  %u,uPacketSent %u\n",
				//	i,statusInfo[i].uAverageJitter,statusInfo[i].uAverageLatency,statusInfo[i].uPacketLost,	statusInfo[i].uPacketDuplicated,statusInfo[i].uPacketRecved,statusInfo[i].uPacketSent);
				//CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
			}

		return VORC_FILEREAD_OK;
		}
		break;

	default:
	    break;
	}
	return VORC_FILEREAD_NOT_IMPLEMENT;
}

VOFILEREADRETURNCODE CRTSPSourceEX::OpenRTSPTrack(int nIndex, CRTSPTrack ** ppRTSPTrack)
{
	if(ppRTSPTrack == NULL)
		return VORC_FILEREAD_NULL_POINTER;

	if(nIndex < 0 || nIndex > 31)
		return VORC_FILEREAD_INDEX_NOT_EXIST;

	if(m_rtspTracks[nIndex] != NULL)
	{
		*ppRTSPTrack = m_rtspTracks[nIndex];
		return VORC_FILEREAD_OK;
	}

	return VORC_FILEREAD_INDEX_NOT_EXIST;
}

VOFILEREADRETURNCODE CRTSPSourceEX::CloseRTSPTrack(CRTSPTrack * pRTSPTrack)
{
	return VORC_FILEREAD_OK;
}
bool CRTSPSourceEX::Restart()
{
	IVOThread::Sleep(10);
	if(m_bRestart)//This only happen when UDP is switched to TCP
	{
		if(m_bStreamUsingTCP)
		{	
			if(1)//m_rtspTracks[0]==pRTSPTrack)//make sure,only the first GetSample thread invoke the restart
			{
				 //int prevPos=pRTSPTrack->GetTrackPlayPos();
				CLog::Log.MakeLog(LL_RTSP_ERR,"tcp.txt","restart!This only happen when UDP is switched to TCP\n");
				Stop();
				//if the server does not support TCP,try RTP again
				bool startOK = Start();
				if(!startOK&&CUserOptions::UserOptions.m_exit==false)
				{
					if(CUserOptions::UserOptions.errorID ==E_UNSUPPORTED_TRANSPORT)
					{
						CLog::Log.MakeLog(LL_RTSP_ERR,"tcp.txt","restart!the server does not support TCP,try RTP again\n");
						SetTransportProtocol(false);
						CUserOptions::UserOptions.m_useTCP=-1;//the server does not support TCP,disable it.
						SetSessionStatus(Session_Playing);//it is must,otherwise the stop will not execute
						Stop();
						startOK= Start();
						if(!startOK)
						{
							CLog::Log.MakeLog(LL_RTSP_ERR,"tcp.txt","try UDP fails again,exit\n");
						}
					}
					if(!startOK)
					{
						CLog::Log.MakeLog(LL_RTSP_ERR,"tcp.txt","try TCP fails,exit\n");
						m_bStopReceivingData=true;
						CUserOptions::UserOptions.m_exit=true;
					}

				}
				m_bRestart = false;
			}

			return VORC_FILEREAD_NEED_RETRY;
		}
		else//if the TCP is not supported,stop it,and exit
		{
			m_bStopReceivingData=true;
			m_bRestart = false;
		}
	}
	return VORC_FILEREAD_OK;
}
VOFILEREADRETURNCODE CRTSPSourceEX::GetRTSPTrackSampleByIndex(CRTSPTrack * pRTSPTrack, VOSAMPLEINFO * pSampleInfo)
{
	if(m_sessionStatus!=Session_Playing)
		return VORC_FILEREAD_NEED_RETRY;
	
	if(m_bRestart||Session_LivePaused==m_sessionStatus)//Session_Paused==m_sessionStatus||pRTSPMediaStreamSyncEngine->GetPlayResponse()==false||
	{
		return VORC_FILEREAD_NEED_RETRY;
	}

	bool IsRTSPClosed = m_RTSPClientEngine->IsRTSPStreamingClosed();
	if(IsRTSPClosed)
	{
		//NotifyEvent(VO_EVENT_SOCKET_ERR,CUserOptions::UserOptions.errorID);
		m_bStopReceivingData = true;
	}
	//if(CUserOptions::UserOptions.m_bMakeLog<LOG_DATA)
	if(m_bStopReceivingData||CUserOptions::UserOptions.m_exit)//m_bStopReceivingData has been set somwhere,or it can not connect to RTSP server somehow,or the socket for data exited
	{
		sprintf(CLog::formatString,"exit!!!receiveing data stop=%d,IsRTSPClosed =%d\n",m_bStopReceivingData,IsRTSPClosed); 
		CLog::Log.MakeLog(LL_BUF_STATUS,"buffer.txt",CLog::formatString);
		return VORC_FILEREAD_INDEX_END;
 	}

	if(CUserOptions::UserOptions.isSeeking)
		return VORC_FILEREAD_NEED_RETRY;
	
	if(m_bNeedBuffering)//
	{
		return VORC_FILEREAD_NEED_RETRY;
	}
	
	//there is buffer,check whether buffering end
	if(pRTSPTrack->IsTrackPlayedComplete()&&m_bPermitSeeking)
	{
		if(!m_sourceEnd)
			CLog::Log.MakeLog(LL_BUF_STATUS,"buffer.txt","!!!TrackPlayedComplete,set the m_sourceEnd = true\n");
		SetSourceEnd(true);
	}
	VOFILEREADRETURNCODE rc = pRTSPTrack->GetSampleByIndex(pSampleInfo);
	if(rc == VORC_FILEREAD_NEED_RETRY)
	{
#define MAX_TRYTIME 20
		if(m_sourceEnd)
		{
			//RetrieveAllPacketsFromRTPBuffer();
			if(m_tryTime>MAX_TRYTIME)
			{
				//if(pRTSPTrack->IsBufEmpty())
						//RetrieveAllPacketsFromRTPBuffer();
				if(pRTSPTrack->IsBufEmpty())
 					 pRTSPTrack->SetEndOfStream(true);
				else//it is possible that last frame arrive slowly,so reset it. 
					 m_tryTime=0;
			}
			else
				m_tryTime++;
		}
		else if(IsRebufferingNeeded())//pSampleInfo->pBuffer&&
		{
			m_dwStartBufferingTime = 0;
		}
	}
	else if(rc == VORC_FILEREAD_OK)
	{
		if(m_sourceEnd)
			m_tryTime = 0;
	}
	else if(rc == VORC_FILEREAD_INDEX_END)
	{
		//CLog::Log.MakeLog(LL_BUF_STATUS,"buffer.txt","!!!exit normally due to the return value of track is end\n");
		int isEnd=0;
		for(int i=0; i<m_nRTSPTrackNum; ++i)
		{
			isEnd += m_rtspTracks[i]->GetEndofStream()?1:0;
		}

		CLog::Log.MakeLog(LL_BUF_STATUS,"buffer.txt","end of stream,exit the current track\n");

		if(isEnd==m_nRTSPTrackNum)
		{
			m_bStreamOver=m_bStopReceivingData = true;//all is end,stop the socket thread
			sprintf(CLog::formatString,"!!!exit normally due to the return value of track is end,num=%d\n",m_nRTSPTrackNum);
			CLog::Log.MakeLog(LL_BUF_STATUS,"buffer.txt",CLog::formatString);
		}
	}
	
	return rc;
}
void	CRTSPSourceEX::SetSourceEnd(bool end)
{
	m_sourceEnd = end;
	CUserOptions::UserOptions.streamEnd =end;
	
	sprintf(CLog::formatString,"set source end to %d\n",end);
	CLog::Log.MakeLog(LL_BUF_STATUS,"buffer.txt",CLog::formatString);
}

bool CRTSPSourceEX::ParseDigestInfo(VO_SOURCE2_VERIFICATIONINFO *info, DigestAuthParam *param)
{
	const char *start = (const char *)info->pData;
	int userLen = 0;
	int	passwdLen = 0;
	
	const char *divide = strchr(start,':');

	if(divide>(start+info->uDataSize))
		return false;

	userLen = divide - start;

	memcpy(param->user,start,userLen);
	param->userLen = userLen;

	passwdLen = info->uDataSize-userLen-1;
	memcpy(param->passwd,divide+1,passwdLen);
	param->passwdLen = passwdLen;

	return true;
}

