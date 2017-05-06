/*
 *  CNativePlayerWrap.h
 *
 *  Created by Lin Jun on 09/10/12.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#ifndef _CBaseNativePlayerWrap_H__
#define _CBaseNativePlayerWrap_H__


#include "voNPWrap.h"
#include "CBaseNativePlayer.h"
#include "voType.h"
#include "voThread.h"
#include "voCMutex.h"
#include "voCSemaphore.h"
#include "CTsMuxStream.h"
#include "CAVFrameTools.h"
#include "CAVFrameRW.h"
#include "CPtrList.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

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
    

class CBaseWrap
{
public:
	CBaseWrap(VONP_LIB_FUNC* pLibOP);
	virtual ~CBaseWrap(void);
	
public:
	virtual int Init();
	virtual int Uninit();
	virtual int SetView(void* pView);
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
    
protected:
    virtual bool    IsCanReadBuffer(bool bAudio);
    virtual bool    CheckEOS();
    virtual bool    CheckCodecSupport(bool bAudio, int nCodec);
    virtual bool    CheckValidBA(int nBATime);
    virtual bool    CheckAudioDropFrame(VONP_BUFFERTYPE* pBuf);
    virtual bool    CheckValidADTSFrame(VONP_BUFFERTYPE* pBuf);
    virtual bool    CheckVideoKeyFrame(VONP_BUFFERTYPE* pBuf);
    virtual bool    CheckH264Resolution(VONP_BUFFERTYPE* pBuf);
    virtual bool    CheckVideoDropFrame(VONP_BUFFERTYPE* pBuf);
    //virtual bool    CheckBuffering();
    virtual bool    CheckBufferReady(){return false;};
    virtual int     CheckAspectRatio(VONP_BUFFERTYPE* pBuf);
    virtual bool    CheckBAInfoChanged();
    virtual int     SendRawFrame(bool bAudio, VONP_BUFFERTYPE* pFrame);
    virtual int     OnFormatChanged();
    virtual int     OnReadBufEOS();
    virtual void    Flush();

    virtual int     CreateNativePlayer();
    virtual void    DestroyNativePlayer();
    
    virtual void    doProcessPlayerReadyRun();
    virtual void    doProcessSuspend();
    virtual void    doProcessResume(void* hView);

protected:
    int             SendEvent(int nEventID, void* pParam1, void*pParam2);
    static int      OnNativePlayerEvent(void * pUserData, int nID, void * pParam1, void * pParam2);
    virtual int     doOnNativePlayerEvent(int nID, void * pParam1, void * pParam2);
    
    static VO_U32	ReadBufThreadProc(VO_PTR pParam);
	VO_U32			doReadBufThread();
	
//	static int		SendDataBuf(void* pUserData, int nSSType, VONP_BUFFERTYPE* pBuffer);
//	int				doSendDataBuf(int nSSType, VONP_BUFFERTYPE* pBuffer);
//	
	static VO_U32	EventNotifyThreadProc(VO_PTR pParam);
	VO_U32			doEventNotifyThread();
    
    bool	IsRunning(){return m_nPlayerStatus==VONP_STATUS_RUNNING;};
	bool	IsPause(){return m_nPlayerStatus==VONP_STATUS_PAUSED;};
	bool	IsStop(){return m_nPlayerStatus==VONP_STATUS_STOPPED;};
    bool	IsLoading(){return m_nPlayerStatus==VONP_STATUS_LOADING;};
	bool	IsSeeking(){return m_bSeeking;};
	bool	IsSupportAudioCodec(){return m_bSupportAudioCodec;};
	bool	IsSupportVideoCodec(){return m_bSupportVideoCodec;};
    bool    IsAppSuspend(){return m_nAppStatus&APP_ENTER_SUSPEND;};
    bool    IsAppResume(){return m_nAppStatus&APP_ENTER_RESUME;};
    bool    IsAppNormal(){return m_nAppStatus&APP_NORMAL;};
    
    void    Reset();
    void    SendEOS();
    void    NotifyBuffering(bool bEnable);
    bool    AdjustTimestamp(VO_SINK_SAMPLE* pMuxSample, VONP_BUFFERTYPE* pBuf);
    
    long long   GetLastReadTime();

protected:
    int		CreateReadThread();
	int		DestroyReadThread();
    
    int		CreateEventNotifyThread();
	int		DestroyEventNotifyThread();
    
    bool    CreateMemStream();
    bool    DestroyMemStream();
    
    void    DestroyHeadData();
    void    DestroyBAInfo();
        
    void    CreateFrameTools();
    void    DestroyFrameTools();

    int     GetPlayingTime();
    
    bool    SendAudioHeadData(int nTimeStamp);
    bool    SendVideoHeadData(int nTimeStamp);
    void    CopyTrackInfo(bool bAudio, VONP_BUFFER_FORMAT* pDst, VONP_BUFFER_FORMAT* pSrc);
    
    void    DumpRawFrame(bool bAudio, unsigned char* pBuffer, int nSize);
    void    EnaleDumpRawFrame(bool bEnable);
    
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
    VONP_LIB_FUNC*              m_pLibOP;
    void*                       m_pView;
    
    CBaseNativePlayer*          m_pNativePlayer;
    CBaseMuxStream*				m_pMuxStream;
    
    VONP_LISTENERINFO           m_NotifyEventInfo;
    VONP_BUFFER_FORMAT          m_AudioTrackInfo;
	VONP_BUFFER_FORMAT          m_VideoTrackInfo;
    VONP_VIDEO_FORMAT           m_fmtVideo;
    VONP_AUDIO_FORMAT           m_fmtAudio;
    
    //read buff
    VONP_READBUFFER_FUNC        m_ReadBufFunc;
    voThreadHandle				m_hEventNotifyThread;
	voThreadHandle				m_hReadBufThread;
	bool						m_bStopReadThread;
	bool						m_bPauseReadBuf;
    voCMutex					m_mtStatus;
    voCSemaphore				m_semSeek;
    bool                        m_bForceReadAudioNewFmt;
    bool                        m_bForceReadVideoNewFmt;
    bool                        m_bReadAudioNewFormat;
    bool                        m_bReadVideoNewFormat;
    bool                        m_bAudioOnly;
    bool                        m_bVideoOnly;
    int                         m_nPosOffsetTime;
    int                         m_nLastReadAudioTime;
    int                         m_nLastReadVideoTime;
    int                         m_nLastAudioNewFormatTime;
    int                         m_nLastVideoNewFormatTime;
    int                         m_nLastBATime;
    int                         m_nTryReadBufCount;
    int                         m_nPlayerStatus;
    int							m_nStatusEOS;//0x01:audio EOS, 0x02:video EOS, 0x04:both EOS
    int                         m_bSupportAudioCodec;
    int                         m_bSupportVideoCodec;
    bool                        m_bWaitKeyFrame;
    bool                        m_bWaitFirstAudioFrame;

    bool                        m_bSeeking;
    bool                        m_bCancelSendRawFrame;
    int                         m_nOpenFlag;
    char                        m_szPlaybackURL[1024];
    bool                        m_bStopEventNotifyThread;
    bool                        m_bFormatChanged;
    bool                        m_bFirstRun;
    
    int                         m_nSeekPos;
    bool                        m_bSeeked;
    int                         m_nPlayingTimeOffset;
    int                         m_nLastPosUpdateTime;
    int                         m_nLastPlayingTime;
    int                         m_nCurrPos;
    bool                        m_bEOSed;
    bool                        m_bBuffering;
    
    CAVFrameTools*              m_pFrameTools;
    int                         m_nAspectRatio;
    
    CAVFrameWriter              m_AVFrameWriter;
    CAVFrameReader              m_AVFrameReader;
    
    PEEK_BUFFER_INFO            m_peekBufInfo;
    
    int                         m_nReadyRunUseTime;
    int                         m_nAppStatus;
    
    CObjectList<BA_CHANGE_INFO> m_sBAChangeInfoList;
    VONP_CPU_INFO				m_sCPUInfo;
    
    bool                        m_bForcePeekBuf;
    bool                        m_bBACasuedByVideoTimeNotDiscontinuo;
    long long                   m_llPauseSysTime;
};

#ifdef _VONAMESPACE
}
#endif // End _VONAMESPACE


#endif //_CNativePlayerWrap_H__
