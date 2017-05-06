#ifndef __RTSPSOURCE_H__
#define __RTSPSOURCE_H__

#include "filebase.h"
#include "network.h"
#include "AutoCriticalSec.h"
#include "MediaStream.h"
#include "RTSPTrack.h"
#include "RTSPClientEngine.h"
#include "utility.h"
#include "voSource2.h"
#include "RTCPParser.h"
#include "RTPParser.h"

#ifdef HTC_PLAYLIST
#include "PlayListInterface_Vo.h"
#include "list_T.h"
#endif //HTC_PLAYLIST

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

/**
\brief the C++ wrapper of the C style SDK of session

The CRTSPSource wraps the session relative SDK,such as voRTSPSessionCreate,voRTSPSessionOpenURL,
voRTSPSessionClose,etc.
*/
class CRTSPSourceEX
{
public:
	CRTSPSourceEX();
	~CRTSPSourceEX();
public:
	/**	create the rtspClientEngine,and open the URL,the session is startup after creation*/
	bool Open(const char * rtspURL);
	/**stop and destroy the session*/
	bool Stop();
	/**create the session if the session does not exist*/
	bool Start(int startTimeByMS=0);

	bool Restart();
	/**pause the session*/
	bool Pause();
	/**play the session*/
	bool Run();
	/**get the media from a specialized position */
	bool SetPosition(DWORD dwStart);

	bool IsPrebufferingCompleted();

	bool IsRebufferingNeeded();
	/**Get the buffering status
	/return the percentage [0,100],the percentage=100*(hasBufferedTime/expectedBuftime)
	*/
	int	 GetBufPercent(int expectBufTimeBySec=0);
private:
	int m_bufferingPercent;
	bool m_bDisableGetData;
protected:
	/**implementation of initializing a RTSP session*/
	bool StartupRTSPSession(DWORD dwRestartTime);
	bool PauseRTSPSession(bool forcePause=false);
	bool PlayRTSPSession();
	bool PlayRTSPSession(DWORD dwStartTime);
	void ShutdownRTSPSession();

protected:
	/**the main function of the socket thread */
	static	DWORD	RecvRTSPStreamsProc(LPVOID pParam);

	bool            InitRTSPTracks();
protected:
	bool            GetRTSPLinkFromSDPFile(const char * pSDPFilePath);
	bool            GetRTSPLinkFromSDPLink(const char * pSDPFilePath);
public:
	int	 			SetDiffTimeInPlayResp(float dwStart);
	bool			CheckDataEnd(int limit=2000);
	bool 			IsAudioBufferEmpty();

	DWORD			RecvRTSPStreams(void);
	/**warpper of voRTSPSessionSetParameter*/
	VOFILEREADRETURNCODE SetParameter(LONG lID, LONG lValue);
	/**warpper of voRTSPSessionGetParameter*/
	VOFILEREADRETURNCODE GetParameter(LONG lID, LONG * plValue);
	/**warpper of voRTSPTrackOpen*/
	VOFILEREADRETURNCODE OpenRTSPTrack(int nIndex, CRTSPTrack ** ppRTSPTrack);
	/**warpper of voRTSPTrackClose*/
	VOFILEREADRETURNCODE CloseRTSPTrack(CRTSPTrack * pRTSPTrack);
	/**warpper of voRTSPTrackGetSampleByIndex*/
	VOFILEREADRETURNCODE GetRTSPTrackSampleByIndex(CRTSPTrack * pRTSPTrack, VOSAMPLEINFO * pSampleInfo);

private:
	IVOMutex*			    m_csSessionLock;//CRITICAL_SECTION
	//HANDLE					m_hThread;
	IVOThread*				m_socketThread;
	IVOThread*				m_restartThread;
	IVOCommand*				m_threadFuncCMD;
	IVOCommand*				m_restartFuncCMD;
	DWORD					m_dwThreadId; 
	
private:
	bool					m_bExit;
	bool					m_bStopReceivingData;
	Session_Status          m_sessionStatus;
	bool					m_livePaused;
	bool					m_sourceEnd;
	bool					m_seekToEnd;
	int						m_tryTime;
	int						m_scaleBufferTime;
	static bool					m_speedupGetLeftSocketData;
public:
	/**get the session status*/
	void  SetSessionStatus(Session_Status status);
	Session_Status GetSessionStatus(){return m_sessionStatus;}
private:
	static void					SetSpeedupState(bool state){m_speedupGetLeftSocketData=state;};
private:
	bool                    m_bNeedBuffering;
	DWORD                   m_dwStartBufferingTime;
	long					UpdatePauseTask(float timeBySec);
	
	void					RemovePauseTask();
	static int				CheckTimeToPause(void * param);
	static int				CheckTimeToPlay(void * param);
	static int				CheckTimeToStop(void * param);
	static int				CheckTimeToSeek(void * param);
	static int				CheckTimeToReportPercent(void * param);
	long						UpdateReportPercentTask(int timeByMS);
	void						DoTimeToReportPercentTask();
	long						m_ReportPercentTaskToken;
	void					DoTimeToPlay();
	void					DoTimeToStop();
	void					DoTimeToSeek();
	void					DoTimeToPause();

	long				    m_pauseTaskToken;
	long				    m_seekTaskToken;
	long					m_stopTaskToken;
	long					SetSeekTask(float timeBySec,int dwStart);
	long					UpdateSeekTask(float timeBySec);
	void					RemoveSeekTask();
	long					m_nextSeekTime;
	long					SetStopTask(float timeBySec);
	long					UpdateStopTask(float timeBySec);
	void					RemoveStopTask();
	void					RemoveAllTask();
	bool					IsDataEnd(int limit=2000);

	
	long					UpdatePlayTask(float timeBySec);
	void					RemovePlayTask();
	long				    m_playTaskToken;
	long					m_playTaskPos;
	bool					ParseDigestInfo(VO_SOURCE2_VERIFICATIONINFO *info, DigestAuthParam *param);

public:
	long					SetPauseTask(float timeBySec,bool delay=false);
	long					SetPlayTask(float timeBySec,int dwStart=0);
	long					GetPauseTask(){return m_pauseTaskToken;}
	long					GetPlayTask(){return m_playTaskToken;}
	long					GetSeekTask(){return m_seekTaskToken;}
private:
	char 				    *m_rtspURL;
	
	bool				    m_bStreamUsingTCP;
	bool                    m_bPermitSeeking;
	CRTSPClientEngine       *m_RTSPClientEngine;

private:
	int                     m_nRTSPTrackNum;
	CRTSPTrack            * m_rtspTracks[32];
private:
	CALLBACKNOTIFY          m_pNotifyFunction;
	bool ResumeLiveStream();
	bool PauseLiveStream();
	bool ResumeStreamAfterPauseFail();
	//bool AllTrackAreEnd();
	int GetExpectBufferingTime();
	bool CheckNetStatus(int percent);
	void EnableBuffering(bool buffering);
	bool CheckScanSupport();
	int  m_prePercent;
	int  m_pauseTime;
private:
	bool					m_bRTPTimeOut;
	char*					m_sdpFileString;
	
	int						m_livePauseTime;
	bool					m_isBegining;
	bool					m_bRestart;
	bool					IsBothBufferEmpty();
	bool					IsOneBufferEmptyWhenPlaying();
	bool					IsBothBufferLessThan(int lowseconds);

	bool					EitherBufferEmpty();
	bool					CreateClientEngine(int urlType=ST_3GPP);
//	bool					IsNearEnd();
	bool					m_bStreamOver;
	bool					m_playRespForSeek;

	void					RetrieveAllPacketsFromRTPBuffer();
	void					AdaptToRefClock();
	int						GetBufPercent2();
	int						m_beginTime;
	
public:
	void					SetSourceEnd(bool end);
	list_T<RTSPFieldParam *> m_listRTSPFieldParam;
	public:
		void					SetBeginTime(int time){m_beginTime=time;}
		void					SetTransportProtocol(bool isTCP);
public:
	void HandleCrash();
};

#ifdef _VONAMESPACE
}
#endif

#endif //__RTSPSOURCE_H__
