#include "utility.h"
#include "RTSPSource.h"

#include "MediaStream.h"
#include "TaskSchedulerEngine.h"
#include "RTSPMediaStreamSyncEngine.h"
#include "RTSPClientEngine.h"

#include "mswitch.h"

#include "voLog.h"
#include "DxManager.h"
#include "mylog.h"
#define LOG_TAG "FLOEngine"
void  CRTSPSource::SetSessionStatus(Session_Status status)
{
	if(CUserOptions::UserOptions.status != status)
	{
		CUserOptions::UserOptions.prevStatus = CUserOptions::UserOptions.status;
		CUserOptions::UserOptions.status = status;
	}
}
const char* strVer = "versionNum:1133.2010.0930.1745";
CRTSPSource::CRTSPSource()
: m_hRecvThread(NULL)
, m_nRecvThreadID(0)
, m_bStopRecv(false)
, m_bRTPTimeOut(false)
, m_RTSPClientEngine(NULL)
, m_nChannelChangeMode(VO_FLOENGINE_CCMODE_NORMAL)
{
	VOLOGI("@!@CRTSPSource Constructor,%s",strVer);
}

CRTSPSource::~CRTSPSource(void)
{
	VOLOGI("@!@~CRTSPSource ENTER");
	Stop(); //ShutdownRTSPSession();
	voCAutoLock lock(&m_OpenMutex);
	SAFE_DELETE(m_RTSPClientEngine);
	VOLOGI("@!@~CRTSPSource EXIT");
}

VO_U32 CRTSPSource::Open(const char * rtspURL)
{
#define OPEN_RETURN {\
	VOLOGI("@!@OPEN_EXIT,%d",ret);\
	if(ret==0)\
		SetSessionStatus(Session_Playing);\
	else \
		SetSessionStatus(Session_Stopped);\
	return ret;}

	VO_U32 ret = VO_ERR_FLOENGINE_NOTIMPLEMENT;
	VOLOGI("@!@OPEN_ENTER_%s,ver=%s",rtspURL,strVer);
    if(_strnicmp(rtspURL, "rtsp://", 7) != 0)
        OPEN_RETURN

    /*
	while(m_bStopRecv)
	{
		SLOG1(LL_RTP_ERR,"rtsp.txt","waiting for prev session (%s) exit\n",m_rtspURL);
		VOLOGI("waiting for prev session exit");
		voOS_Sleep(5);
	}*/
	voCAutoLock lock(&m_OpenMutex);
	SetSessionStatus(Session_Connectting);
	CUserOptions::UserOptions.m_exit = false;
	strcpy(m_rtspURL, rtspURL);
    VOLOGI("CCMode: %d", m_nChannelChangeMode);
    if (m_nChannelChangeMode == VO_FLOENGINE_CCMODE_OCC)
    {
        VOLOGI("@!@HandleOCC");
        ret = HandleOCC(rtspURL);
        VOLOGI("@!@HandleOCC... %x", ret);
		OPEN_RETURN
    }
    // if fastCC failed, try normal CC
#ifdef FORCE_RTSP_LOG
	CUserOptions::UserOptions.m_bMakeLog = 2;
#endif//FORCE_RTSP_LOG
    VOLOGI("StartupRTSPSession_begin");
    ret = StartupRTSPSession();
    VOLOGI("StartupRTSPSession... %d,log=%d", ret,CUserOptions::UserOptions.m_bMakeLog);
	
    OPEN_RETURN
}

VO_U32 CRTSPSource::Close()
{
	VOLOGI("@!@CLOSE_ENTER");
	CUserOptions::UserOptions.m_exit = true;
	m_bStopRecv = true;
	Stop();
	voCAutoLock lock(&m_OpenMutex);
	ShutdownRTSPSession();

	VOLOGI("@!@CLOSE_EXIT");
	return VO_ERR_FLOENGINE_OK;
}

VO_U32 CRTSPSource::Start()
{
	if(m_hRecvThread != NULL)
		return VO_ERR_FLOENGINE_OK;
	VOLOGI("@!@START_ENTER");
	voCAutoLock lock(&m_OpenMutex);

	m_bStopRecv = false;
	m_bRTPTimeOut = false;

	voThreadCreate(&m_hRecvThread, &m_nRecvThreadID, (voThreadProc)RecvRTSPStreamsProc, this, 0);
	if(m_nRecvThreadID==NULL)
		m_nRecvThreadID = 1;
	VOLOGI("@!@START_EXIT,%X",m_nRecvThreadID);
	return VO_ERR_FLOENGINE_OK;
}

VO_U32 CRTSPSource::Stop()
{
	if (0)//m_nChannelChangeMode == VO_FLOENGINE_CCMODE_OCC&&m_bStopRecv==false)
		return true;
	if(m_hRecvThread==NULL)
		return true;
	VOLOGI("@!@STOP_ENTER");
	CUserOptions::UserOptions.m_exit = true;
	m_bStopRecv = true;
	voCAutoLock lock(&m_OpenMutex);

	VO_U32 nTryNum = 0;
	while(m_hRecvThread != NULL)
	{
		voOS_Sleep(10);
		SLOG1(LL_SOCKET_ERR,"flow.txt","trying to stop recv_thread_%d",nTryNum);
		nTryNum++;
	}
	CTaskSchedulerEngine::CreateTaskSchedulerEngine()->UnscheduleAllTask();

	SetSessionStatus(Session_Stopped);
	
	VOLOGI("@!@STOP_EXIT");
	return true;
}

VO_U32 CRTSPSource::GetParam(VO_U32 nParamID, VO_S32 * pParam)
{
	switch(nParamID)
	{
	case VO_PID_FLOENGINE_H264HEADDATA:
		{
			VO_CODECBUFFER * pHeadData = (VO_CODECBUFFER *)pParam;
			CMediaStream * pVideoMediaStream = GetMediaStream();
			if(pVideoMediaStream == NULL)
				return VO_ERR_FLOENGINE_NOTIMPLEMENT;
			pHeadData->Length = pVideoMediaStream->GetCodecHeadData(pHeadData->Buffer);
			return VO_ERR_FLOENGINE_OK;
		}

	default:
	    break;
	}

	return VO_ERR_FLOENGINE_NOTIMPLEMENT;
}

VO_U32 CRTSPSource::SetParam(VO_U32 nParamID, VO_PTR pParam)
{
	//VOLOGI("@!@SETPARAM_ENTER_%X",nParamID);
#define SETPARAM_RETURN_OK \
	return VO_ERR_FLOENGINE_OK; //VOLOGI("@!@SETPARAM_EXIT");\

	voCAutoLock lock(&m_OpenMutex);
	switch(nParamID)
	{
	case VO_PID_FLOENGINE_CALLBACK:
		{
			VO_FLOENGINE_CALLBACK * pCallback = (VO_FLOENGINE_CALLBACK *)pParam;
			memcpy(&CUserOptions::UserOptions.m_mfCallBackFuncSet, pCallback, sizeof(VO_FLOENGINE_CALLBACK));
			SETPARAM_RETURN_OK
		}

	case VO_PID_FLOENGINE_CCMODE:
		m_nChannelChangeMode = (VO_U32)pParam;
		VOLOGI("VO_PID_FLOENGINE_CCMODE: %d", m_nChannelChangeMode);
		SETPARAM_RETURN_OK

	case VO_PID_FLOENGINE_H264ERROR:
		{
			HandleH264BitError();
			SETPARAM_RETURN_OK
		}
	
#ifdef ENABLE_LOG
	case VO_PID_FLOENGINE_DATAPATH:
		VOLOGI("VO_PID_FLOENGINE_DATAPATH");
#if ENABLE_DX
	//	TheDxManager.SetDataPath((const char*)pParam);
#endif //ENABLE_DX
        CDumper::SetFolder((const char*)pParam);
        SETPARAM_RETURN_OK
	case VO_PID_FLOENGINE_LOGLEVEL:
		VOLOGI("VO_PID_FLOENGINE_LOGLEVEL");
		CUserOptions::UserOptions.m_bMakeLog = pParam?2:0;
#if ENABLE_DX
//		TheDxManager.SetLogLevel((VO_U32)pParam);
#endif //ENABLE_DX
		CDumper::SetMask((VO_U32)pParam);
        SETPARAM_RETURN_OK
#endif //ENABLE_LOG

	case VO_PID_FLOENGINE_ERRORRETRY:
		VOLOGI("VO_PID_FLOENGINE_ERRORRETRY");
		CUserOptions::UserOptions.m_nMaxErrorRetry = (VO_U32)pParam;
		SETPARAM_RETURN_OK

	case VO_PID_FLOENGINE_NETTIMEOUT:
		
		CUserOptions::UserOptions.m_nRTPTimeOut = (VO_U32)pParam;
		CUserOptions::UserOptions.m_nRTSPTimeOut = (VO_U32)pParam;
		CUserOptions::UserOptions.m_nConnectTimeOut = ((VO_U32)pParam) / 2;
		VOLOGI("VO_PID_FLOENGINE_NETTIMEOUT %d",(VO_U32)pParam);
		SETPARAM_RETURN_OK

#if ENABLE_DX
	case VO_PID_FLOENGINE_LIBPATH:
		VOLOGI("VO_PID_FLOENGINE_LIBPATH");
		TheDxManager.SetLibPath((const char*)pParam);
        SETPARAM_RETURN_OK
        
	case VO_PID_FLOENGINE_DXDRM:
		VOLOGI("VO_PID_FLOENGINE_DXDRM");
		TheDxManager.SetDxDRM((VO_U32)pParam);
        SETPARAM_RETURN_OK

#endif //ENABLE_DX

	default:
	    break;
	}
	return VO_ERR_FLOENGINE_NOTIMPLEMENT;
}

VO_U32 CRTSPSource::StartupRTSPSession()
{
	int ret = -1;
	do
	{
		VOLOGI("ClientEngine: %p", m_RTSPClientEngine);
		if(m_RTSPClientEngine!=NULL)
			break;
		m_RTSPClientEngine = new CRTSPClientEngine();
		if(m_RTSPClientEngine == NULL)
			break;
		m_RTSPClientEngine->SetRTSPSource(this);

		m_RTSPClientEngine->Init();

		VOLOGI("Connect server");
		if(!m_RTSPClientEngine->ConnectServer(m_rtspURL))
		{
			VOLOGE("Connect Server Failure");
			ret = HS_EVENT_CONNECT_FAIL;
			break;
		}

		VOLOGI("Send DESCRIBE command");
		if(!m_RTSPClientEngine->DescribeCmd(m_rtspURL))
		{
			VOLOGE("DESCRIBE Command Failure");
			ret = HS_EVENT_DESCRIBE_FAIL;
			break;
		}
		
		VOLOGI("Send SETUP command");
		if(!m_RTSPClientEngine->SetupCmd())
		{
			VOLOGE("SETUP Command Failure");
			ret = HS_EVENT_SETUP_FAIL;
			break;
		}
		
		CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
		VOLOGI("SyncEngine: %p", pRTSPMediaStreamSyncEngine);
		
		VOLOGI("Send PLAY command");
		if(!m_RTSPClientEngine->PlayCmd())
		{
			VOLOGE("PLAY Command Failure");
			ret = HS_EVENT_PLAY_FAIL;
			break;
		}
		
		VOLOGI("Startup RTSP Session Success");

		return VO_ERR_FLOENGINE_OK;

	}while(0);

	ShutdownRTSPSession();

	VOLOGE("Startup RTSP Session Failure");

	return ret;
}

VO_U32 CRTSPSource::ShutdownRTSPSession()
{
	if(m_RTSPClientEngine == NULL)
		return VO_ERR_FLOENGINE_OK;
	
    VOLOGI("Shutdown RTSP Session...");
	m_RTSPClientEngine->TeardownCmd();
	m_RTSPClientEngine->Reset();

	SAFE_DELETE(m_RTSPClientEngine);

	return VO_ERR_FLOENGINE_OK;
}

VO_U32 CRTSPSource::RecvRTSPStreamsProc(void * pParam)
{
	CRTSPSource * pRTSPSource = (CRTSPSource *)pParam;
	return pRTSPSource->RecvRTSPStreams();
}

VO_U32 CRTSPSource::RecvRTSPStreams(void)
{
	VO_U32 ret = VO_ERR_FLOENGINE_FAIL;

	m_timeoutNum = 0;

	m_bRTPTimeOut = false;

	int m_beginTime = voOS_GetSysTime();

	bool rtpTimeout = false;
	bool workAround = true;
	VOLOGI("Recv_Thread,Enter");
	while(!m_bStopRecv)
	{
		if(m_timeoutNum > CUserOptions::UserOptions.m_nRTPTimeOut && rtpTimeout == false)
		{
			rtpTimeout = true;
			NotifyEvent(HS_EVENT_RTP_TIMEOUT, 0);
		}
		else if(rtpTimeout && m_timeoutNum == 0)
		{
			rtpTimeout = false;
		}
#if 0//workaround:send fake packet for JNI to avoid timeout
		if(m_timeoutNum==1&&workAround)
		{
			workAround = false;
			static const unsigned char fakeData[]={0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
			VO_FLOENGINE_FRAME frame;
			frame.pData = (VO_PBYTE)fakeData;
			frame.nSize = 8;
			frame.nStartTime = 0;
			frame.nFrameType = VO_FLOENGINE_FRAMETYPE_VIDEO;
			CUserOptions::UserOptions.m_mfCallBackFuncSet.OnFrame(&frame);
			frame.nFrameType =VO_FLOENGINE_FRAMETYPE_AUDIO;
			CUserOptions::UserOptions.m_mfCallBackFuncSet.OnFrame(&frame);
			VOLOGI("workaround:send fake packet for JNI to avoid timeout");
		}
#endif
		voCAutoLock lock(&m_SessionMutex);
	
		
		ret = m_RTSPClientEngine->ReadRTSPStreams();

		if(ret == VEC_SOCKET_ERROR)
		{
			NotifyEvent(HS_EVENT_SOCKET_ERR, CUserOptions::UserOptions.errorID);
			m_bStopRecv = true;

			VOLOGE("Network Socket Error: code = %d", ::GetSocketError());
		}
		else if(ret == 0) // timeout
		{
			m_timeoutNum = (voOS_GetSysTime() - m_beginTime) / 1000;
		}
		else // recv data
		{
			if((ret & PACKET_RTP) == PACKET_RTP)
			{
				m_timeoutNum = 0;
				m_beginTime = voOS_GetSysTime();
				m_bRTPTimeOut = false;
			}
		}
	}
	VOLOGI("Recv_Thread,Exit");
	m_hRecvThread = NULL;
	m_nRecvThreadID = 0;
	return ret;
}

VO_U32 CRTSPSource::HandleOCC(const char * media_url2)
{
	voCAutoLock lock(&m_SessionMutex);
	
	if(m_RTSPClientEngine == NULL)
		return VO_ERR_FLOENGINE_FAIL;
#if ENABLE_DX
	TheDxManager.Close();
	if(TheDxManager.Open()!=EDX_SUCCESS)
	{
		VOLOGE("TheDxManager.Open()!=EDX_SUCCESS,reset fails");
		return VO_ERR_FLOENGINE_FAIL;
	}
	VOLOGE("Reset DxManager");
#endif ENABLE_DX	
	m_timeoutNum = 0;
	strcpy(m_rtspURL, media_url2);
	if(!m_RTSPClientEngine->HandleOCC(media_url2))
	{

		return VO_ERR_FLOENGINE_FAIL;
	}
	

	return VO_ERR_FLOENGINE_OK;
}

/*
VO_U32 CRTSPSource::HandleOCCIncompatibleTransport(char * media_url2)
{	
	CTaskSchedulerEngine::CreateTaskSchedulerEngine()->UnscheduleAllTask();
	
	// 1) stop receiving data thread
	m_bStopReceivingData = true;	

	while(m_dwThreadId != 0)
	{
		voOS_Sleep (1);
	}
	
	if(m_socketThread != NULL)
		VOCPFactory::DeleteOneThread(m_socketThread);
	m_socketThread = NULL;
	
	//voOS_Sleep(10);
	
	CUserOptions::UserOptions.m_exit = true;
	
	// 2) destroy rtsp tracks of media_url1
	for(int i=0; i<m_nRTSPTrackNum; ++i)
	{
		SAFE_DELETE(m_rtspTracks[i]);
	}
	m_nRTSPTrackNum = 0;
	
	// 3) do NOT send TEARDOWN command to rtsp server
	//m_RTSPClientEngine->TeardownCmd();
	
	// 4) reset rtsp client engine
	m_RTSPClientEngine->ResetForOCC();
	m_RTSPClientEngine->Init();
	
	
	// 5) talk to server for media_url2
	strcpy(m_rtspURL, media_url2);
	if(!StartupRTSPSessionForOCC(media_url2))
	{
#if _TEST_OCC_TIME
		{
			COCCTimeInfo * pOCCTimeInfo = COCCTimeInfo::CreateInstance();
			pOCCTimeInfo->Reset();
		}
#endif //_TEST_OCC_TIME
		
		return VORC_FILEREAD_ENTER_FAILED;
	}
		
#if _TEST_OCC_TIME
	{
		COCCTimeInfo * pOCCTimeInfo = COCCTimeInfo::CreateInstance();
		pOCCTimeInfo->nOCCStopTime = voOS_GetSysTime();
		pOCCTimeInfo->GetTimeofDay(pOCCTimeInfo->nOCCStopTime_sec, pOCCTimeInfo->nOCCStopTime_usec);
	}
#endif //_TEST_OCC_TIME
	
	return VORC_FILEREAD_OK;
}

VO_U32 CRTSPSource::StartupRTSPSessionForOCC(char * media_url2)
{
	CUserOptions::UserOptions.eventCode = 0;
	
	do{
		CUserOptions::UserOptions.m_exit = false;
		
		if(!m_RTSPClientEngine->DescribeCmd(m_rtspURL) || CUserOptions::UserOptions.eventCode == HS_EVENT_DESCRIBE_FAIL)
		{
			CUserOptions::UserOptions.eventCode = 0;
			CLog::Log.MakeLog(LL_RTSP_ERR, "rtsp.txt", "@!@@!@describe fail@!@@!@\n");
			break;
		}
		
		if(!m_RTSPClientEngine->SetupCmd())
		{
			CLog::Log.MakeLog(LL_RTSP_ERR, "rtsp.txt", "@!@@!@setup fail@!@@!@\n");
			NotifyEvent(HS_EVENT_SETUP_FAIL, CUserOptions::UserOptions.errorID);
			break;
		}

		
		CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
		
#if !TEST_ONE_TRACK
		if(CUserOptions::UserOptions.m_bMakeLog < LOG_DATA)
			if(m_RTSPClientEngine->MediaStreamCount() > 1)
				pRTSPMediaStreamSyncEngine->EnableWaitIFrame(true); //m_bPermitSeeking==false);//waitting for I frame is only for live mode
#endif//
		
		
#define  CHECK_MAX_DURATION 1800 //seconds
		
		if(0)//m_bPermitSeeking&&duaration>CHECK_MAX_DURATION)
		{
			//if(CheckScanSupport()==false)
			{
				NotifyEvent(HS_EVENT_DISABLE_SCAN, 1);
			}
		}
		
		//if(!InitRTSPTracks())
		//{
		//	CLog::Log.MakeLog(LL_RTSP_ERR, "rtsp.txt", "@!@@!@InitRTSPTracks fail@!@@!@\n");
		//	break;
		//}
		
		if(!m_RTSPClientEngine->PlayCmd())//,254.489
		{
			CLog::Log.MakeLog(LL_RTSP_ERR, "rtsp.txt", "@!@@!@Play fail@!@@!@\n");
			break;
		}
		
		CLog::Log.MakeLog(LL_RTSP_ERR, "rtsp.txt", "$$$$Wait I frame@!@@!@\n");
		//m_scaleBufferTime = 5000;//set the scale time for first buffering that needs to wait for the I frame
		
		
		return true;
		
	}while(1);
	
	//ShutdownRTSPSession();
	
	CLog::Log.MakeLog(LL_RTSP_ERR,"rtsp.txt","@!@@!@StartupRTSPSession fail@!@@!@\n");
	return false;	
}


*/
CMediaStream*	CRTSPSource::		GetMediaStream(int id)
{
	CMediaStream * pVideoMediaStream = NULL;
	for(int i=0; i<m_RTSPClientEngine->MediaStreamCount(); ++i)
	{
		CMediaStream * pMediaStream = m_RTSPClientEngine->MediaStream(i);
		if((id==VIDEO_TRACK&&pMediaStream->IsVideoStream()) ||(id==AUDIO_TRACK&&pMediaStream->IsAudioStream()))
		{
			pVideoMediaStream = pMediaStream;
			break;
		}

	}
	return pVideoMediaStream;
}
VO_U32 CRTSPSource::HandleH264BitError()
{
	CMediaStream * pVideoMediaStream = GetMediaStream();
	if(pVideoMediaStream == NULL)
		return 0;

	pVideoMediaStream->HandleH264BitError();

	return 0;	
}