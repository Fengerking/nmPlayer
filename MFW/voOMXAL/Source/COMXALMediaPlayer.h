/*
 *  COMXALMediaPlayer.h
 *
 *  Created by Lin Jun on 11/15/11.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#ifndef _COMXALMediaPlayer_H__
#define _COMXALMediaPlayer_H__

#include "voType.h"
#include "voNPWrap.h"
#include "COMXALWrapper.h"
#include "CMuxStream.h"
#include "voThread.h"
#include "voCMutex.h"
#include "CADecoder.h"
#include "CAEffect.h"
#include "voCSemaphore.h"
#include "CPtrList.h"
#include "CAVFrameTools.h"
#include "OpenMAXVideoFrameSplit.h"

typedef struct
{
    long long   llUpdateTimestamp;
    
    bool        bNeedVideoSizeUpdated;
    int         nVideoWidth;
    int         nVideoHeight;
    
    bool        bNeedAspectRatioUpdated;
    int         nAspectRatio;
}BA_CHANGE_INFO;

    
#define MAX_FORCE_READ_TRY_COUNT    120
//#define MAX_FORCE_READ_TRY_COUNT    0X7FFFFFFF
#define MAX_DISCONTINUOS_TIME       1000
#define MAX_AUDIO_INTERVAL_SCALE    1.5
    
typedef enum
{
    PEEK_TRIGGER_AUDIO_NEW_FORMAT        = 0,
    PEEK_TRIGGER_VIDEO_NEW_FORMAT        = 1,
    PEEK_TRIGGER_AUDIO_DISCONTINUOS      = 2,
    PEEK_TRIGGER_VIDEO_DISCONTINUOS      = 3,
    PEEK_TRIGGER_AUDIO_NOT_AVAILABLE     = 4,
    PEEK_TRIGGER_VIDEO_NOT_AVAILABLE     = 5,
    PEEK_TRIGGER_APP_RESUME              = 6,
    PEEK_TRIGGER_AUDIO_LARGE_INTERVAL    = 7
}PEEK_BUFFER_TRIGGER;
    
typedef struct
{
    bool                            bPeekBufAvailable;
    CObjectList<VONP_BUFFERTYPE>    sList;
}PEEK_BUFFER;
    
    
typedef struct
{
    bool                bEnable;
    bool                bPeeking;
    
    PEEK_BUFFER_TRIGGER nPeekTrigger;       // refer to PEEK_BUFFER_TRIGGER
    
    PEEK_BUFFER         sAudioBuf;
    PEEK_BUFFER         sVideoBuf;
    
    CObjectList<VONP_BUFFERTYPE>     sRecycleBuf;
}PEEK_BUFFER_INFO;
    
    
typedef enum
{
    APP_NORMAL          = 1,
    APP_ENTER_SUSPEND   = 2,
    APP_ENTER_RESUME    = 4,
}APP_STATUS;
 

class COMXALMediaPlayer
{
public:
	COMXALMediaPlayer(VONP_LIB_FUNC* pLibOP);
	virtual ~COMXALMediaPlayer(void);
	
public:
	virtual int Init();
	virtual int Uninit();
	virtual int SetView(void * pView);
	virtual int Open(void* pSource, int nFlag);
	virtual int Close();
	virtual int Run();
	virtual int Pause();
	virtual int Stop();
	virtual int GetPos();
	virtual int SetPos(int nPos);
	virtual int GetParam(int nParamID, void* pValue);
	virtual int SetParam(int nParamID, void* pValue);
	virtual int GetAudioBuffer(VONP_BUFFERTYPE** ppBuffer);
    virtual int GetVideoBuffer(VONP_BUFFERTYPE** ppBuffer);

private:
	void	Flush();
	int		SendEvent(int nEventID, void* pParam1, void*pParam2);
	bool	CheckCodecSupport(bool bAudio, int nCodec);
	
	int		CreateReadThread();
	int		DestroyReadThread();

	int		CreateEventNotifyThread();
	int		DestroyEventNotifyThread();
	
	bool	IsRunning(){return m_nPlayerStatus==VONP_STATUS_RUNNING;};
	bool	IsPause(){return m_nPlayerStatus==VONP_STATUS_PAUSED;};
	bool	IsStop(){return m_nPlayerStatus==VONP_STATUS_STOPPED;};
	bool	IsSeeking(){return m_bSeeking;};
	bool	IsSupportAudioCodec(){return m_bSupportAudioCodec;};
	bool	IsSupportVideoCodec(){return m_bSupportVideoCodec;};
	
	int		SendRawFrame(bool bAudio, VONP_BUFFERTYPE* pFrame);
	
	int		CreateAudioDecoder(int nCodec, VONP_AUDIO_FORMAT* pFmt);
	int		DoAudioDecoder(VONP_BUFFERTYPE* pBufer);
	int		DestroyAudioDecoder();
	
	int		CreateAuidoEffect();
	int		DoAudioEffect(VONP_BUFFERTYPE* pBufer);
	int		DestroyAudioEffect();

	bool	IsCanReadBuffer(bool bAudio);
	//void	OnFormatChanged(VONP_AUDIO_FORMAT* pAudioFormat);
	void	OnFormatChanged(void);
	
	int		GetPlayingTime();
	int		WaitBufferEmpty();
    int		WaitBufferEmptyAfterGoForeground();

	bool	isLegalADTSFrame(unsigned char * pData, int nSize, int * pnActualSize);

	bool	CheckEOS();
	void	CheckWrapRunning();
	bool	CheckBuffering();
	//bool    CheckFormatChanged(bool bAudio, VONP_BUFFER_FORMAT* pNewFormat);
	void    CheckFormatChanged();
    bool	CheckAudioDropFrame(VONP_BUFFERTYPE* pBuf);
	bool	CheckVideoDropFrame(VONP_BUFFERTYPE* pBuf);
	bool	CheckVideoKeyFrame(VONP_BUFFERTYPE* pBuf);
	bool	CheckH264BFrame(VONP_BUFFERTYPE* pBuf);
    bool	CheckValidADTSFrame(VONP_BUFFERTYPE* pBuf);
	bool    CheckSeekMode();
	void	CheckCPUInfo();
	void	CheckFlushBuffer(VONP_BUFFERTYPE* pBuf);
    bool    CheckH264Resolution(VONP_BUFFERTYPE* pBuf);
    bool    CheckFirstTimeStampViewChanged(int nTimeStamp);
    int     CheckAspectRatio(VONP_BUFFERTYPE* pHeadDataBuf);
    bool    CheckValidBA(int nBATime);
	
	void	NotifyBuffering(bool bEnable);
	void	CopyTrackInfo(bool bAudio, VONP_BUFFER_FORMAT* pDst, VONP_BUFFER_FORMAT* pSrc);
	bool	SendVideoHeadData(int nTimeStamp);
	bool	SendAudioHeadData(int nTimeStamp);
    
    int     PeekFlushBuffer(VONP_BUFFERTYPE* pBuffer, bool& bReadAudio);
    
    bool    CreateOMXAL();
    void    DestroyOMXAL();
    
    int    CreateMemStream();
    bool    DestroyMemStream();

    void    DestroyBAInfo();

	bool	getNativeWindow();
	void	releaseNativeWindow();

    bool    ProcessViewChanged(void * pNewView);
    
    int     CreatePCMBuffer();
    int     DestroyPCMBuffer();
    
    void    CreateFrameTools();
    void    DestroyFrameTools();
    
    void    SendEOS();
	void	InitVideoSplit(void);
    
private:
	static int		ReadBufQueryFromOMXAL(void * pUserData, char* pBuffer, int* pWantReadSize, int nReadType);
	int				doReadBufQueryFromOMXAL(char* pBuffer, int* pWantReadSize, int nReadType);
	
	static int		EventCallbackFromOMXAL(void* pUserData, int nEventID, void* pPram);
	int				doEventCallbackFromOMXAL(int nEventID, void* pPram);
	
	static VO_U32	ReadBufThreadProc(VO_PTR pParam);
	VO_U32			doReadBufThread();
	
	static int		SendDataBuf(void* pUserData, int nSSType, VONP_BUFFERTYPE* pBuffer);
	int				doSendDataBuf(int nSSType, VONP_BUFFERTYPE* pBuffer);
	
	static VO_U32	EventNotifyThreadProc(VO_PTR pParam);
	VO_U32			doEventNotifyThread();
	
	void DumpRawFrame(bool bAudio, unsigned char* pBuffer, int nSize, long long time);
    void DumpHeadData(bool bAudio, unsigned char* pBuffer, int nSize);

private:
    int     ReadBuffer(bool bAudio, VONP_BUFFERTYPE* pBuf);
    int     ForceReadBuffer(bool bAudio, VONP_BUFFERTYPE* pBuf);
    
    //peek functions
    bool    IsPeekBufAvailable();
    bool    PeekBuffer(VONP_BUFFERTYPE* pBuf);
    int     ReadPeekBuffer(bool bAudio, VONP_BUFFERTYPE* pBuf);
    void    ResetPeekBuffer();
    void    ReleasePeekBuffer(VONP_BUFFERTYPE* pBuf);
    void    ReleasePeekBuffer(CObjectList<VONP_BUFFERTYPE>* pList);
    bool    InsertPeekBuffer(CObjectList<VONP_BUFFERTYPE>* pList, VONP_BUFFERTYPE* pNewBuf);
    POSITION FindKeyFrame(CObjectList<VONP_BUFFERTYPE>* pList, long long llTimeBeforeKeyFrame);
    
    VONP_BUFFERTYPE*    CloneBuffer(VONP_BUFFERTYPE* pBuf);
    VONP_BUFFERTYPE*    CloneSilentBuffer(VONP_BUFFERTYPE* pBuf);

protected:
	bool    IsAppSuspend(){return m_nAppStatus&APP_ENTER_SUSPEND;};
	bool    IsAppResume(){return m_nAppStatus&APP_ENTER_RESUME;};
    virtual int     OnReadBufEOS();
    virtual bool    CheckBAInfoChanged();
    void    EnaleDumpRawFrame(bool bEnable);
    long long   GetLastReadTime();

protected:
	COMXALWrapper*				m_pWrap;
	VONP_READBUFFER_FUNC*       m_pReadBufFunc;
	OMXAL_READBUFFER_FUNC		m_ReadBufQueryFromOMXAL;
	OMXAL_EVENT_LISTENER		m_EventCallbackFromOMXAL;
	CMuxStream*					m_pMuxStream;
	
	voThreadHandle				m_hEventNotifyThread;
	voThreadHandle				m_hReadBufThread;
	bool						m_bStopReadThread;
	bool						m_bPauseReadBuf;
	
	int							m_nOpenFlag;
	VONP_LIB_FUNC*              m_pLibOP;
	
	int							m_nPlayerStatus;
	bool						m_bSeeking;
	char						m_szURL[1024];
	bool						m_bEnableAudioEffect;
	bool						m_bWaitKeyFrame;
	int							m_nStatusEOS;//0x01:audio EOS, 0x02:video EOS, 0x04:both EOS
	voCMutex					m_mtStatus;
	VONP_LISTENERINFO*          m_pNotifyEvent;

	VO_MEM_OPERATOR				m_MemOp;
	CADecoder*					m_pAudioDecoder;
	VO_CODECBUFFER				m_AudioDecOutputBuf;
	CAEffect*					m_pAudioEffect;
	
	int							m_nMaxPCMOutputBufSize;
	VONP_AUDIO_FORMAT           m_AudioFormat;

	void *						m_pJavaEnv;			// Java env
	void *						m_pJavaSurface;		// Java surface
	ANativeWindow *				m_pNativeWindow;	// Native window

	int							m_nLastVideoTime;
	int							m_nLastAudioTime;
	bool						m_bFormatChanged;
	bool						m_bFirstRun;
	
	voCSemaphore				m_semSeek;
	voCSemaphore                m_semBufQueue;
	int							m_nLastReadVideoTime;
	int							m_nLastReadAudioTime;
	int							m_nPlayingTime;
	int							m_nPosOffsetTime;
    int                         m_nNewFormatStartTime;
	int							m_nLastPosUpdateTime;
		
	bool						m_bAudioOnly;
	bool						m_bVideoOnly;
	bool						m_bBuffering;
	bool						m_bSeeked;
	bool						m_bNeedFlush;
	bool						m_bForceReadAudioFirst;
	
	VONP_BUFFER_FORMAT          m_AudioTrackInfo;
	VONP_BUFFER_FORMAT          m_VideoTrackInfo;
	
	VO_LOG_PRINT_CB*			m_pLogPrintCallback;
	bool						m_bSupportAudioCodec;
	bool						m_bSupportVideoCodec;
	bool						m_bHasBeenAudioOnly;
	int							m_nSeekPos;
	bool						m_bGotNewFormatBuf;
	VONP_CPU_INFO               m_sCPUInfo;
	bool						m_bReadFlushBuffer;
    int                         m_nTryReadBufCount;
    VONP_VIDEO_FORMAT           m_fmtVideo;
    int                         m_nLastBATime;
	int							m_nLastBAVideoTime;
    int                         m_nLastAudioNewFormatTime;
    bool                        m_bViewChanged;
    bool                        m_bReadAudioNewFormat;
    bool                        m_bReadVideoNewFormat;

	// force read Audio | Video new format
	// if > 0, we will force read, we use initialized value 60
    int							m_nForceReadAudioNewFmt;
    int							m_nForceReadVideoNewFmt;

    bool                        m_bCancelSendRawFrame;
    int                         m_nPlayingTimeViewChanged;
    bool                        m_bGetFirstTimeAfterViewChanged;
    bool                        m_bStopEventNotifyThread;
    bool                        m_bOpened;
    bool                        m_bEOSed;
    bool                        m_bUseOutsideRender;
    
    int                         m_nAudioStepTime;
	int                         m_nAudioStepSize;
	unsigned char*              m_pAudioOutBuff;
	int                         m_nAudioOutSize;
    int                         m_nAudioBufferSize;
    unsigned char*              m_pAudioPcmBuff;
    
    bool                        m_bSeekWhenPaused;
    bool                        m_bViewAvailable;
    
    CAVFrameTools*                m_pFrameTools;
    int                         m_nAspectRatio;

  bool                        m_bAudioStatusOK;
  bool                        m_bViewChangeing;
  VO_TCHAR                    m_pPlayerPath[1024];
#ifdef _VODEBUG
	bool						m_bPlayTsFile;
	int							m_nFileSuffix;		// -1 - all files finish working
	FILE *						m_hPlayTsFile;
#endif	// _VODEBUG

    PEEK_BUFFER_INFO            m_peekBufInfo;
    int                         m_nPlayingTimeOffset;
    bool                        m_bForceReadVideoNewFmt;
    bool                        m_bForceReadAudioNewFmt;
    bool                        m_bWaitFirstAudioFrame;
    int                         m_nLastVideoNewFormatTime;
    bool                        m_bForcePeekBuf;
    int                         m_nAppStatus;
	int							m_nLastWrapReadDataTime;
    CObjectList<BA_CHANGE_INFO> m_sBAChangeInfoList;
	char 						m_sAndroidVersion[64];
	CVideoFrameSplit*   		m_pVideoSplit;
	VONP_BUFFERTYPE				m_pSplitBuf;		
	int       					lastRunTime;
	bool						m_bNeedFormatChange;
};


#endif //_COMXALMediaPlayer_H_
