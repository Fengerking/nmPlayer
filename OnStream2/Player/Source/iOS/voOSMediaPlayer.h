    /************************************************************************
    *                                                                        *
    *        VisualOn, Inc. Confidential and Proprietary, 2003 - 2011        *
    *                                                                        *
    ************************************************************************/
/*******************************************************************************
    File:        voOSMediaPlayer.h

    Contains:    voOSMediaPlayer header file

    Written by:  Jeff

    Change History (most recent first):
    2011-11-15        Jeff            Create file

*******************************************************************************/
#ifndef __VO_OS_MEDIA_PLAYER_H__
#define __VO_OS_MEDIA_PLAYER_H__

#include <map>
#include "voOSEngWrapper.h"
#include "voOnStreamType.h"
#include "voOSSourceWrapper.h"
#include "voOSLog.h"
#include "voRunRequestOnMain.h"
#include "voNSRecursiveLock.h"

using namespace std;

class voOSMediaPlayer :private voRunRequestOnMain
{
public:
    voOSMediaPlayer();
    virtual ~voOSMediaPlayer();
    
    virtual int         Init(int nPlayerType, void* pInitParam, int nInitParamFlag);
    virtual int         UnInit();
    virtual int         SetView(void * hDrawWnd);
    virtual int         Open(void* pSource, int nFlag, int nSourceType, void* pInitParam, int nInitParamFlag);
    
    virtual int			GetProgramCount (int* pStreamCount);
    virtual int			GetProgramInfo (int nStream, VOOSMP_SRC_PROGRAM_INFO** ppStreamInfo);
    virtual int         GetCurTrackInfo (int nOutPutType, VOOSMP_SRC_TRACK_INFO** ppTrackInfo);
    virtual int			GetSample (int nOutPutType, void* pSample);
    virtual int         SelectProgram (int nProgram);
	virtual int         SelectStream (int nStream);
	virtual int         SelectTrack (int nTrack);
	virtual int         SelectLanguage (int nIndex);
	virtual int         GetLanguage (VOOSMP_SUBTITLE_LANGUAGE_INFO** ppLangInfo);
    
    virtual int			Close();
    
    virtual int         Run(void);
    virtual int         Pause(void);
    virtual int         Stop(void);
    
    virtual int         Suspend(bool bPlayAudio);
    virtual int         Resume(void);
    
    virtual int         GetPos();
    virtual int         SetPos(int nCurPos);
    
    virtual int         GetDuration(long long* pDuration);
    
    virtual int         GetParam(int nParamID, void* pValue);
	virtual int         SetParam(int nParamID, void* pValue);

    virtual int			GetVideoCount();
    virtual int			GetAudioCount();
    virtual int			GetSubtitleCount();
    virtual int			SelectVideo(int nIndex);
    virtual int			SelectAudio(int nIndex);
    virtual int			SelectSubtitle(int nIndex);
    virtual bool        IsVideoAvailable(int nIndex);
    virtual bool        IsAudioAvailable(int nIndex);
    virtual bool        IsSubtitleAvailable(int nIndex);
    
    virtual int			CommitSelection();
    virtual int			ClearSelection();
    
    virtual int			GetVideoProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
    virtual int			GetAudioProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
    virtual int			GetSubtitleProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
    virtual int			GetCurrTrackSelection(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);
    virtual int			GetCurrPlayingTrackIndex(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);
    virtual int			GetCurrSelectedTrackIndex(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);
    
    virtual bool        IsLive();
    
protected:
    virtual int         SetPosInner(int nCurPos);
    
    virtual int         SetSrcParam(int nParamID, void* pValue);
    
    virtual void        RunningRequestOnMain(int nID, void *pParam1, void *pParam2);
    
    virtual int         DoReadSubtitle(voSubtitleInfo* pSubtitle);
    static  int         OnReadSubtitle(void* pUserData, voSubtitleInfo* pSubtitle);
    
    virtual int			HandleEvent (int nID, void * pParam1, void * pParam2);
	static	int			OnListener (void * pUserData, int nID, void * pParam1, void * pParam2);
    
    virtual int			HandleRequestEvent (int nID, void * pParam1, void * pParam2);
	static	int			OnRequestListener (void * pUserData, int nID, void * pParam1, void * pParam2);
    
    virtual int         ParserCap();
    
    virtual int         SetSubtitlePath(const char *pPath);
    static  void        voStreamDownloadRespondCB(void * pUserData, int nCode, const char * strError);
    
    virtual int         GetSelectStream(int *pStream);
    virtual int         ResetStr(char **pTo, const char *pFrom);
    
protected:
    bool					m_bSeeking;
    bool					m_bNativeRecoveryBlockEvent;
    bool                    m_bSuspendPlayAudio;
    bool                    m_bIsAppActive;
    int                     m_nIsLive;
    int                     m_nOpenFlag;
    int                     m_nSelectedAudio;
    int                     m_nSelectedSubtitle;
    int                     m_nlastStatus;
    
    void*                   m_pCabXml;
    void*                   m_pStreamDownload;
    void*                   m_pDownloadFile;
    void*                   m_pDRMAdapter;
    
    VO_U64                  m_nPosition;
    
    char                    m_szCap[1024];
    
    char*                   m_pUserAgent;
    char*                   m_pDrmName;
    char*                   m_pDrmAPI;
    
    char*                   m_pLicensePath;
    char*                   m_pLicenseText;
    char*                   m_pLicenseContent;
    char*                   m_pPathLib;
    
    VOOSMP_SRC_VERIFICATIONINFO* m_pVerificationInfo;
    VOOSMP_SRC_RTSP_CONNECTION_PORT* m_pRTSP_Port;
    VOOSMP_SRC_HTTP_PROXY*  m_pProxy;
    
	voOSEngWrapper*         m_pOSEng;
    voOSSourceWrapper*      m_pSource;
    
	VOOSMP_LISTENERINFO		m_cSendListener;
    VOOSMP_LISTENERINFO     m_cThisListener;
    
	VOOSMP_LISTENERINFO		m_cRequest_SendListener;
    VOOSMP_LISTENERINFO     m_cRequest_ThisListener;
    
    VOOSMP_PERF_DATA        m_cStartCap;
    
    voNSRecursiveLock       m_cLock;
    
    std::map<VO_U32, int>   m_cParamMap;
    
    static voOSLog          m_cLog;
};

#endif // __VO_PLAYER_SDK_H__
