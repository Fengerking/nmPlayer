	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCMediaPlayer.h

	Contains:	voCMediaPlayer header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-18		JBF			Create file

*******************************************************************************/
#ifndef __voCMediaPlayer_H__
#define __voCMediaPlayer_H__

#include "vompDrm.h"
#include "voMetaData.h"
#include "CAndroidUtilities.h"

#include "CFileSource.h"
#include "CVideoDecoder.h"
#include "CAudioDecoder.h"
#include "voIOMXDec.h"

#include "voCAudioRenderBufferMgr.h"

#include "CAudioEffect.h"
#include "CVideoEffect.h"

#include "CAudioSpeed.h"
#include "CBaseAudioRender.h"
#include "CBaseVideoRender.h"
#include "voCPlayerLogo.h"

#include "CFileSink.h"

#include "voCBaseThread.h"
#include "voCBasePlayer.h"

#if defined(_IOS) || defined(_MAC_OS)
#include "voLicenseMng.h"
#include "voLogoEffect.h"
#endif

// 'vomp' = 0x766f6d70

#define		VOMP_EVENT_STATUS		0x766f6d70 + 1
#define		VOMP_EVENT_AUDIO		0x766f6d70 + 2
#define		VOMP_EVENT_VIDEO		0x766f6d70 + 3
#define		VOMP_EVENT_LOAD			0x766f6d70 + 4
#define		VOMP_EVENT_CLOSE		0x766f6d70 + 5
#define		VOMP_EVENT_CPULOAD		0x766f6d70 + 6

#define		VOMP_CB_CheckStatus				0X82100006
#define		VOMP_PID_CHECK_PERFORMANCE		0X09000001	/*!<Internal check performance  */

typedef enum 
{
	VOMP_RECREATEDECODERSTATE_NONE, 
	VOMP_RECREATEDECODERSTATE_NEEDRECREATE, 
	VOMP_RECREATEDECODERSTATE_FINISHSETFORCEOUTPUT,
	VOMP_RECREATEDECODERSTATE_SENDEOS,
} VOMP_RECREATEDECODERSTATE;
	
class voCDataSource;
struct IAudioDSPClock;
class voCMediaPlayer : public voCBasePlayer
{
public:
	voCMediaPlayer(VOMPListener pListener, void * pUserData);
	virtual ~voCMediaPlayer(void);

	virtual int 		SetDataSource (void * pSource, int nFlag);
	virtual int 		SendBuffer (int nSSType, VOMP_BUFFERTYPE * pBuffer);
	virtual int			GetVideoBuffer (VOMP_BUFFERTYPE ** ppBuffer);
	virtual int			GetAudioBuffer (VOMP_BUFFERTYPE ** ppBuffer);
	virtual int 		Run (void);
	virtual int 		Pause (void);
	virtual int 		Stop (void);
	virtual int 		Flush (void);
	virtual int 		GetStatus (VOMP_STATUS * pStatus);
	virtual int 		GetDuration (int * pDuration);
	virtual int 		GetCurPos (int * pCurPos);
	virtual int 		SetCurPos (int nCurPos);
	virtual int 		GetParam (int nID, void * pValue);
	virtual int 		SetParam (int nID, void * pValue);
	virtual int			CloseSource (bool bSync);

	virtual int			onLoadSource (int nMsg, int nV1, int nV2);
	virtual int			onCloseSource (int nMsg, int nV1, int nV2);
	virtual int			onNotifyEvent (int nMsg, int nV1, int nV2);
	virtual int			onRenderAudio (int nMsg, int nV1, int nV2);
	virtual int			onDecoderAudio (int nMsg, int nV1, int nV2);
	virtual int			onRenderVideo (int nMsg, int nV1, int nV2);
	virtual int			onDecoderVideo (int nMsg, int nV1, int nV2);
	virtual int			redraw (VOMP_BUFFERTYPE ** ppBuffer);

protected:
			int			OpenPlayConfigFile (VO_TCHAR * pCfgFile);
			int			OpenVOSDKConfigFile (VO_TCHAR * pCfgFile);

			int			gcd(int m, int n);

	virtual int			Reset (void);
	virtual int			ReleaseVideoRenderBuffers (void);

	virtual int			LoadSource (void);
	virtual bool		CreateAudioDec (void);
	virtual bool		CreateVideoDec (void);
	virtual int			GetMetaData (int nIndex, void * pValue);

	virtual int			ReadAudioSample (void);
	virtual int			DecAudioSample (void);
	virtual int			effectAudioSample_Volume (VO_PBYTE pBuffer, VO_S32 nSize);	// effect to buffer itself
	virtual int			effectAudioSample_Dolby (VO_PBYTE pBuffer, VO_S32 nSize);	// effect to m_pAudioRndBuff
	virtual int			RenderAudioSample (void);
	virtual int			checkAudioEffectFormat();
	virtual void		onAudioOutFormatChanged(VO_AUDIO_FORMAT * pOutFormat);
	virtual void		onAudioDecOutFormatChanged(VO_AUDIO_FORMAT * pDecOutFormat);
	// render PCM data from decoder
	// mainly post process (volume, Dolby effect), update reference clock
	virtual int			doAudioRender (VO_PBYTE pPCMdata, VO_U32 nPCMLength, VO_S64 llTimeStamp, VOMP_BUFFERTYPE ** ppBuffer);
	virtual int			DoAudioRender (VOMP_BUFFERTYPE ** ppBuffer);

	virtual int			ReadVideoSample (void);
	virtual int			NewVideoCodec (void);
	virtual int			DecVideoSample (void);
	virtual int			EffectVideoSample (VO_VIDEO_BUFFER * pVideoBuffer);
	virtual int			RenderVideoSample (void);
	virtual int			CheckVideoRenderTime (void);
	virtual int			DoVideoRender (VOMP_BUFFERTYPE ** ppBuffer);
	virtual void		cancelBufferFromVideoDec(VO_VIDEO_BUFFER * pVideoBuffer);
	virtual void		cancelBuffersFromVOVideoDec(VO_BOOL bResetCodecData = VO_FALSE);
	virtual int			flushPicturesOfVOVideoDec();

	virtual int			UpdateAudioFormat (VO_AUDIO_FORMAT * pAudioOutFormat);
	virtual VO_U64		GetPlayingTime (VO_BOOL bForUser = VO_FALSE);
	virtual void		ShowResult (void);

	virtual int			postAudioRenderEvent (int nDelayTime);
	virtual int			postAudioDecoderEvent (int nDelayTime);
	virtual int			postVideoRenderEvent (int nDelayTime);
	virtual int			postVideoDecoderEvent (int nDelayTime);
	virtual int			postCheckStatusEvent (int nMsg, int nV1 = 0, int nV2 = 0, int nDelayTime = -1);

	static int			vompStreamCallBack (long EventCode, long * EventParam1, long * userData);
	static int			vompDrmCallBack (void * pUserData, int nFlag, void * pParam, int nReserved);

	static void*		vomtGetAddress (void * pUserData, void * hLib, char * pFuncName, int nFlag);
	static int			vomtFreeLib (void * pUserData, void * hLib, int nFlag);
	static void*		vomtLoadLib (void * pUserData, char * pLibName, int nFlag);

			int			ResetVideoSample (VO_VIDEO_BUFFER * pOutBuffer);
			void		CheckCPUInfo (void);
			bool		CheckVideoCodecLegal(VOMP_VIDEO_CODINGTYPE nCodec , VO_U32 nFourCC , int nWidth , int nHeight );
	
#if defined(_IOS) || defined(_MAC_OS)
			bool		CheckLicense(VO_TCHAR* pLicenseFilePath);
#endif

	inline int			getDrawRect(VOMP_RECT& rectDraw)
	{
		int nDrawWidth = m_rcDraw.right - m_rcDraw.left;
		int nDrawHeight = m_rcDraw.bottom - m_rcDraw.top;
		if(nDrawWidth > m_nVideoRndWidth || nDrawHeight > m_nVideoRndHeight)
			return VOMP_ERR_Status;

		memcpy(&rectDraw, &m_rcDraw, sizeof(VOMP_RECT));

		if(m_nZoomType == VO_ZM_PANSCAN)
		{
			int nShowLeft = 0;
			int nShowTop = 0;

			if (m_nVideoRndWidth > nDrawWidth)
				nShowLeft = ((m_nVideoRndWidth - nDrawWidth ) / 2) & ~7;

			if (m_nVideoRndHeight > nDrawHeight)
				nShowTop = ((m_nVideoRndHeight - nDrawHeight) / 2) & ~7;

			rectDraw.right += nShowLeft;
			rectDraw.left += nShowLeft;
			rectDraw.top += nShowTop;
			rectDraw.bottom += nShowTop;
		}

		return VOMP_ERR_None;
	}
	
protected:
	CBaseConfig					m_cfgPlay;
	CBaseConfig					m_cfgVOSDK;
	CAndroidUtilities			m_utProp;
	voCMutex					m_mtStatus;
	voCBaseThread *				m_pMsgThread;
	VO_FILE_OPERATOR *			m_pFileOP;
	void *						m_pSource;
	VO_TCHAR 					m_szURL[1024*2];
	VO_U64						m_nAudioBuffTime;
	VOMP_STATUS					m_nStatus;
	VO_U32						m_nStatusChanging;
	int							m_nOpenFlag;
	int							m_nSeekMode;
	unsigned int				m_nOnEventTimes;
	unsigned int				m_nDroppedVidooFrames;
	VOMP_LIB_FUNC *				m_pLibFunc;
	VOMP_SOURCEDRM_CALLBACK *	m_pDrmCB;
	VO_BOOL						m_bDrmCrippled;
	MetaDataString				m_msMetaInfo;
	char						m_szMetaText[1024];
	VO_U64						m_nPlayStartPos;
	VO_U64						m_nPlayStopPos;
	voCMutex					m_mtSetPos;
	voCMutex					m_mtCodec;

	voCMutex					m_mtSource;
	CBaseSource *				m_pFileSource;
	voCDataSource *				m_pDataSource;
	VO_SOURCE_INFO				m_sFilInfo;
	int							m_nTrackCount;
	VO_FILE_FORMAT				m_nFF;
	bool						m_bAudioOnly;
	bool						m_bVideoOnly;
	int							m_nAudioPlayMode;
	int							m_nVideoPlayMode;
	int							m_nDisableDropVideoFrame;
	int							m_nDisableDeblockVideo;
	int							m_nDeblockVideo;
	int							m_nCheckFastPerformance;
	bool						m_bNotifyCompleted;

	int							m_nRenderSuccess;
	int							m_nLowLatence;

	int							m_nAudioOnOff;
	int							m_nVideoOnOff;
	int							m_nAudioReaded;
	int							m_nVideoReaded;
	int							m_nVideoPlayBack;

	int							m_nAudioVideoType;
	
	int							m_nAudioBitrate[10];
	int							m_nVideoBitrate[10];

	voCMutex					m_mtAudioFlush;
	bool						m_bSetAudioDecoderThreadPriority;
	bool						m_bSetAudioRendererThreadPriority;
	int							m_nAudioTrack;
	VO_SOURCE_TRACKINFO			m_sAudioTrackInfo;
	VO_SOURCE_SAMPLE			m_sAudioSample;
	CAudioDecoder *				m_pAudioDec;
	VO_CODECBUFFER				m_sAudioDecInBuf;
	VO_CODECBUFFER				m_sAudioDecOutBuf;
	VO_U32						m_nAudioDecSize;
	VO_U32						m_nAudioDecOutFrameSize;	// bytes per second of audio decoder output

	voCMutex					m_mtAudioRender;
	VO_U32						m_nAudioRenderBufferCount;
	voCBaseThread *				m_pAudioDecThread;

	// audio render buffer manager
	voCAudioRenderBufferMgr	*	m_pARBManager;
	VO_AUDIO_FORMAT				m_sARBFormat;

	VO_U32						m_nAudioRenderNum;
	VO_U32						m_nAudioRndNuming;
	VO_S32						m_nAudioVolume;
	VO_S32						m_nAudioAACPlus;
	VO_BOOL						m_bOnlyGetAudio;

	CAudioEffect *				m_pAudioEffect;
	VO_AUDIO_FORMAT				m_sAudioEftOutFormat;	// audio format of effect output buffer
	VO_CODECBUFFER				m_sAudioEftInBuf;
	VO_CODECBUFFER				m_sAudioEftOutBuf;
	VO_BOOL						m_bEffectEnable;
	VO_BOOL						m_bAddAudioEffect;
	VO_PBYTE					m_pAudioEftOutPcmBuff;		// actual PCM buffer for audio effect output
	VO_U32						m_nAudioEftOutFrameSize;	// bytes per second of audio effect output
	VO_BOOL						m_bAllowDolbyAudio;  // allow to load dolby module and AC3,eAC3 module

	CBaseAudioRender *			m_pAudioRender;
	VOMP_BUFFERTYPE				m_bufAudio;
	VO_S64						m_llAudioRenderTime;
	VO_S64						m_llLastAudioRenderTime;
	VO_U32						m_nAudioStepTime;
	VO_U32						m_nAudioDecOutStepSize;
	VO_PBYTE					m_pAudioDecOutPcmBuff;		// actual PCM buffer for audio decoder output

	VO_PBYTE					m_pAudioDecOutBuff;			// audio decoder output buffer
	VO_U32						m_nAudioDecOutSize;			// total size of m_pAudioDecOutBuff
	VO_U32						m_nAudioDecOutLength;		// current length of audio decoder output buffer
	VO_AUDIO_FORMAT				m_sAudioDecOutFormat;		// audio format of decoder output buffer

	VO_PBYTE					m_pAudioRndBuff;			// audio output buffer
	VO_U32						m_nAudioRndSize;			// total size of m_pAudioRndBuff
	VO_U32						m_nAudioRndLength;			// current length of audio output buffer

	VO_U32						m_nAudioDateCount;
	VO_U64						m_nAudioCountStartTime;

	voCMutex					m_mtVideoFlush;
	bool						m_bSetVideoThreadPri;
	int							m_nVideoTrack;
	VO_SOURCE_TRACKINFO			m_sVideoTrackInfo;
	VO_VIDEO_FORMAT				m_sVideoFormat;
	VO_S32						m_nRCGetVideoTrackDataAfterPureAudio;
	VO_SOURCE_SAMPLE			m_sVideoSample;
	CVideoDecoder *				m_pVideoDec;
	VO_CODECBUFFER				m_sVideoDecInBuf;
	VO_VIDEO_BUFFER				m_sVideoDecOutBuf;
	VO_S64						m_llLastVideoDecInTime;		// last time stamp of sample input to video decoder
	VO_S64						m_llLastVideoDecOutTime;	// last time stamp of sample output from video decoder
	VO_BOOL						m_bMoreVideoBuff;
	VO_BOOL						m_bNeedInputData;
	VO_BOOL						m_bVideoDecInputRetry;
	VO_BOOL						m_bVideoDecHungry;			// if the video decoder need input data ASAP or not
	VOMP_RECREATEDECODERSTATE	m_eRecreateDecoderState;
	VO_BOOL						m_bFileDropFrame;
	VO_BOOL						m_bFrameDropped;
	VO_U32						m_nBFrameDropped;
	VO_U32						m_nVideoRenderNum;
	VO_U32						m_nVideoRndNuming;
	VO_U32						m_nVideoDecoderNum;
	VO_S32						m_nVideoDropRender;
	VO_U32						m_nVideoDateCount;
	VO_S64						m_nVideoCountStartTime;
	VO_S32						m_nGetThumbnail;
	VO_BOOL						m_bSetDispRect;
	VO_PTR						m_pVidDecCB;
	VOMP_ASPECT_RATIO			m_nAspectRatio;
	VO_BOOL						m_bOnlyGetVideo;
	VO_BOOL						m_bUsingHWCodec;
	VO_BOOL						m_bFirstFrmRnd;

	CVideoEffect *				m_pVideoEffect;
	VOMP_VIDEO_EFFECTTYPE		m_pVideoEffectType;
	VO_BOOL						m_bVideoEffectOn;	

	VO_S32						m_nVideoPerfOnOff;

	CBaseVideoRender *			m_pVideoRender;
	VO_BOOL						m_bOutVideoRender;

	voCMutex					m_mtNewTime;
	VO_BOOL						m_bAudioNewTime;
	VO_BOOL						m_bVideoNewTime;
	VO_S64						m_nNewTimeBegin;
	VO_S64						m_nNewTimeVideo;
	VO_S32						m_nIFrameSkip;


	VO_PTR						m_hView;
	VOMP_RECT					m_rcDraw;
	int							m_nColorType;
	VO_IV_ZOOM_MODE				m_nZoomType;
	VO_VIDEO_BUFFER				m_rgbBuffer;
	VOMP_BUFFERTYPE				m_outVideoBuffer;

	VO_VIDEO_FORMAT				m_fmtVideo;
	VO_VIDEO_BUFFER				m_bufVideo;

	voCMutex					m_mtPlayTime;
	VO_U64						m_nStartSysTime;
	VO_S64						m_nStartMediaTime;
	VO_S64						m_nSeekPos;
	VO_BOOL						m_bSeekingPos;
	VO_S64						m_llRenderBufferTime;
	VO_S64						m_llRenderSystemTime;
	VO_S64						m_llRenderAdjustTime;
	VO_S64						m_llDSPRefTime; // DSP audio clock reference time
	VO_S64						m_llDSPTimeOffset; 
	VO_S64						m_llDSPEOSSystemStartTime; 
	VO_S64						m_llDSPLastAudioRenderTime; 
	int							m_llDSPLastAudioRenderDuration; 
	VO_S64						m_nPlayingTime;
	VO_BOOL						m_bDataBuffering;
	VO_BOOL						m_bAudioSeekingPos;

	VO_BOOL						m_bVideoEOS;
	VO_BOOL						m_bAudioEOS;
	int							m_nVideoEOSArrived;
	int							m_nAudioEOSArrived;
	int							m_nAudioDecError;
	int							m_nVideoDecError;

	voCBaseThread *				m_pAudioThread;
	voCBaseThread *				m_pVideoThread;

	// for multi video frames output
	voCMutex					m_mtVideoRender;
	voCBaseThread *				m_pVideoDecThread;
	VO_VIDEO_BUFFER	*			m_pVideoOutputBuff;
	VO_U32						m_nVideoRenderCount;
	VO_U32						m_nVideoRenderAdd;
	VO_S32						m_nVideoRndCopy;
	VO_S64						m_nVideoStartTime;
	VO_VIDEO_BUFFER **			m_ppVideoOutBuffers;
	VO_VIDEO_BUFFER **			m_ppVideoOutBufShared;
	VO_S32 *					m_pVideoOutWidth;
	VO_S32 *					m_pVideoOutHeight;
	VO_U32						m_nVideoOutFillIndex;
	VO_U32						m_nVideoOutPlayIndex;
	VO_S32						m_nVideoRndWidth;
	VO_S32						m_nVideoRndHeight;
	VO_S64						m_nLastFrameTime;
	VO_U32						m_nOnlyVideoDropCount;
	VO_U64						m_nOnlyVideoDropTime;
	VO_S64						m_nReceiveFrameTime;
	VO_S32						m_nReceiveFrameNum;
	VO_S32						m_nFrameRate;
	VO_S64						m_nDecStartSysTime;
	VO_S32						m_nDecSucess;

	VO_PTR						m_hCheckLib;
	char*						m_pLicenseData;
	char						m_szLicenseText[256];
	VO_TCHAR					m_szLicensePath[1024];

	int							m_nCPUNumber;	
	int							m_nCPUVersion;
	VOMP_CPU_INFO				m_sCPUInfo;
	CCPULoadInfo*				m_pCPULoadInfo;

	CFileSink *					m_pFileSink;
	VO_FILE_SOURCE				m_Source;
	VO_TCHAR					m_szSinkName[256];
	VO_SINK_SAMPLE				m_smpSinkAudio;
	VO_SINK_SAMPLE				m_smpSinkVideo;
	
	// added by Lin Jun,currently only for RTSP
	// 0:automatically try UDP first then TCP,1:force to use TCP,-1:disable TCP, default 0*/
	VO_U32						m_nSocketConnectionType;
	char						m_sDigestName[256];
	char						m_sDigestPass[256];

	// for log out
	VO_U64						m_nNotifyBuffingTime;
	VO_U64						m_nLastVideoSysTime;
	VO_BOOL						m_bSendStartVideoBuff;
	VO_U64						m_nLastAudioSysTime;
	VO_BOOL						m_bSendStartAudioBuff;

	VO_S32						m_nStatusCheckTime;
	VO_TCHAR					m_szWorkPath[1024];
	
	VOMP_LIB_FUNC				m_sLibFunc;


	int							m_nShowLogLevel;
	VO_S64						m_nStartPlayTime;
	VO_S64						m_nLastVideoFrmTime;

	VO_S64						m_nLastAudioFrmTime;
	VO_S64						m_nLastAudioRndTime;
	VO_S64						m_nLastAudioRndDifTime;

	VO_S64						m_aVideoDcdTime[1024];
	int							m_nVideoDcdTimeIndex;
	VO_S64						m_aVideoRndTime[1024];
	int							m_nVideoRndTimeIndex;

	VO_U64						m_nVideoDecTimes;
	VO_U64						m_nVideoCCRTimes;
	VO_U64						m_nVideoRndTimes;
	VO_U64						m_nVideoRndLTime;
	VO_U64						m_nAudioDecTimes;

	VO_U32						m_nSetAudioNameType;
	VO_U32						m_nSetAudioAPIType;
	VO_TCHAR					m_sSetAudioName[256];
	VO_TCHAR					m_sSetAudioAPI[256];

	VO_U32						m_nSetVideoNameType;
	VO_U32						m_nSetVideoAPIType;
	VO_TCHAR					m_sSetVideoName[256];	
	VO_TCHAR					m_sSetVideoAPI[256];

	voCPlayerLogoInfo*			m_pVideoLogoInfo;
	voCPlayerLogoInfo*			m_pAudioLogoInfo;

	CAudioSpeed*				m_pAudioSpeed;
	VO_BOOL						m_bASpeedEnable;
	VO_S32						m_nAudioPlaySpeed;
	VO_AUDIO_FORMAT				m_sLastAudioFmt;

	char						m_nVideoBuffHead[128];

#if defined(_IOS) || defined(_MAC_OS)
	voLicenseMng				m_LicenseCheck;
	voLogoEffect*				m_pLogoEffect;
	VO_BOOL						m_bAudioBufFull; //changed with VOMP_PID_AUDIO_SINK_BUFFTIME when IOS audio sink get 4 buffer
#endif	

	VO_PTR                      mJavaVM;
	VO_BOOL                     mAudioDecInputRetry;
	VO_BOOL                     m_bSendAudioBuffer;
	VO_BOOL                     m_bRedrawVideo;
	
	char		                m_szUUID[256];
	VO_BOOL		                m_bRenderAudioOnOff;

	FILE*		                m_fDumpVideoinput;
    IAudioDSPClock*             mADSPClock;

	VO_AUDIO_CODINGTYPE         mOutputType;
	VO_BOOL						m_bPauseRefClock;
	VO_U32						m_nAudioVideoUnsupport;/* 0x01 : Audio, 0x02 : Video*/
	VO_BOOL                     mIsSuspend;
	
	int							m_nStatusRenderFlag; /* 1: have seeked */

	// VisualOn use new buffer mechanism for video decoder, so vome2 need adjust to fit, East 20131113
	// implemented: HEVC
	VO_BOOL						m_bNewVOVideoDec;

public:
	static	VO_U32	IdleThreadProc (VO_PTR pParam);
	virtual int		IdleThreadLoop (void);
			int		m_nIdleThdPriority;
			bool	m_bIdleStop;
			void *	m_pIdleThread;
			VO_U32	m_nIdleThdID;
			VO_S64  preVideoRenderTimeStamp;
};

class voCMediaPlayerEvent : public voCBaseEventItem
{
public:
    voCMediaPlayerEvent(voCMediaPlayer * pPlayer, int (voCMediaPlayer::* method)(int, int, int),
					    int nType, int nMsg = 0, int nV1 = 0, int nV2 = 0)
		: voCBaseEventItem (nType, nMsg, nV1, nV2)
	{
		m_pPlayer = pPlayer;
		m_fMethod = method;
    }

    virtual ~voCMediaPlayerEvent()
	{
	}

    virtual void Fire (void) 
	{
        (m_pPlayer->*m_fMethod)(m_nMsg, m_nV1, m_nV2);
    }

protected:
    voCMediaPlayer *		m_pPlayer;
    int (voCMediaPlayer::*	m_fMethod) (int, int, int);
};

#endif // __voCMediaPlayer_H__

