	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CPlayGraph.h

	Contains:	CPlayGraph header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CPlayGraph_H__
#define __CPlayGraph_H__

#include "CBasePlay.h"

#include "CFileSource.h"
#include "CAudioDecoder.h"
#include "CVideoDecoder.h"
#include "CAudioRender.h"
#include "CVideoRender.h"

#include "voThread.h"

class CPlayGraph : public CBasePlay
{
public:
	// Used to control the image drawing
	CPlayGraph (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP);
	virtual ~CPlayGraph (void);

	virtual VO_U32 		Create (VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength);
	virtual VO_U32		SetViewInfo (VO_PTR hView, VO_RECT * pRect);

	virtual VO_U32 		Run (void);
	virtual VO_U32 		Pause (void);
	virtual VO_U32 		Stop (void);

	virtual VO_U32 		GetDuration (VO_U32 * pDuration);
	virtual VO_U32 		GetCurPos (VO_S32 * pPos);
	virtual VO_U32 		SetCurPos (VO_S32 nPos);

	virtual VO_U32 		SetParam (VO_U32 nID, VO_PTR pValue);
	virtual VO_U32 		GetParam (VO_U32 nID, VO_PTR pValue);

	virtual VO_U32		SetConfig (CBaseConfig * pConfig);

protected:
	virtual VO_U32		AllocAudioRenderBuffer (void);
	virtual VO_S64		GetPlayingTime (void);
	virtual VO_U32		VideoWaitForPlayingTime (VO_S64 nVideoTime);

	virtual VO_U32		Release (void);
	virtual VO_U32		UpdateWithConfig (void);

	virtual VO_U32		ReadAudioData (VO_U32 nTrack, VO_PTR pTrackData);
	virtual VO_U32		DecoderAudioData (VO_CODECBUFFER * pOutput, VO_AUDIO_FORMAT * pAudioFormat);
	virtual VO_U32		RenderAudioData (VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart);

	virtual VO_U32		ReadVideoData (VO_U32 nTrack, VO_PTR pTrackData);
	virtual VO_U32		SetInputVideoData (VO_CODECBUFFER * pInput);
	virtual VO_U32		DecoderVideoData (VO_VIDEO_BUFFER * pOutput, VO_VIDEO_FORMAT * pVideoFormat);
	virtual VO_U32		RenderVideoData (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait);

	virtual VO_U32		EndOfSource (void);

protected:
	CBaseConfig *		m_pCodecCfg;
	CBaseSource *		m_pSource;
	VO_S32				m_nAudioTrack;
	VO_S32				m_nVideoTrack;

	CAudioDecoder *		m_pAudioDec;
	CVideoDecoder *		m_pVideoDec;
	CAudioRender *		m_pAudioRender;
	CVideoRender *		m_pVideoRender;

	VO_AUDIO_FORMAT		m_fmtAudio;
	VO_VIDEO_FORMAT		m_fmtVideo;
	VO_IV_ZOOM_MODE		m_nZoomMode;
	VO_IV_ASPECT_RATIO	m_nRatio;
	VO_RECT				m_rcDisp;
	VO_BOOL				m_bVOSDK;
	VO_BOOL				m_bForThumbnail;

	// Playvack status
	VOMMPlAYSTATUS		m_status;
	VO_U32				m_nPlaySpeed;
	VO_U32				m_nPlayFlag;
	VO_U32				m_nSeekMode;
	VO_S64				m_llVideoStartPos;

	// playback time
	VO_S64				m_nStartTime;
	VO_S64				m_nPlayedTime;

	// position parameter
	VO_U32				m_nDuration;
	VO_S32				m_nCurPos;

	// first sample position
	VO_S64				m_nFirstAudioPos;
	VO_S64				m_nFirstVideoPos;

	// end of stream
	VO_BOOL				m_bAudioEndOfStream;
	VO_BOOL				m_bVideoEndOfStream;

	VO_U32				m_nSeekTime;
	VO_U32				m_nSeekPos;

	VO_BOOL				m_bReleased;
	VO_S32				m_nBuffering;
	VO_TCHAR			m_szError[128];

protected:
	VO_U32					m_nThreadNum;
	voThreadHandle			m_hThreadAudio;
	VO_BOOL					m_bStopAudio;
	VO_BOOL					m_bAudioPlaying;

	VO_SOURCE_SAMPLE		m_audioSourceSample;
	VO_CODECBUFFER			m_audioSourceBuffer;

	VO_CODECBUFFER			m_audioRenderBuffer;
	VO_PBYTE				m_pAudioRenderData;
	VO_U32					m_nAudioRenderSize;
	VO_U32					m_nAudioRenderStep;
	VO_U32					m_nAudioBytesPerSec;
	VO_S32					m_nAudioBufferTime;

	VO_S64					m_nAudioRenderStart;
	VO_U32					m_nAudioRenderFill;
	VO_U32					m_nAudioRenderCount;

	voThreadHandle			m_hThreadVideo;
	VO_BOOL					m_bStopVideo;
	VO_BOOL					m_bVideoPlaying;

	VO_SOURCE_SAMPLE		m_videoSourceSample;
	VO_CODECBUFFER			m_videoSourceBuffer;
	VO_VIDEO_BUFFER			m_videoRenderBuffer;

	VO_S64					m_nVideoRenderTime;
	VO_U32					m_nVideoRenderCount;
	VO_U32					m_nVideoRenderSpeed;
	VO_U32					m_nVideoRenderDropTime;
	VO_BOOL					m_bVideoRenderDrop;


	VO_SOURCE_SAMPLE		m_videoNextKeySample;
	VO_BOOL					m_bVideoPlayNextFrame;

	VO_VOMM_CB_STREAMING	m_cbStreaming;
	VO_VOMM_CB_VIDEORENDER	m_cbVideoRender;
	VO_VOMM_CB_AUDIORENDER	m_cbAudioRender;

public:
	static	VO_U32	PlaybackAudioLoopProc (VO_PTR pParam);
	virtual VO_U32	PlaybackAudioLoop (void);
	virtual VO_U32	PlaybackAudioFrame (void);

	static	VO_U32	PlaybackVideoLoopProc (VO_PTR pParam);
	virtual VO_U32	PlaybackVideoLoop (void);
	virtual VO_U32	PlaybackVideoFrame (void);

	virtual VO_U32	PlaybackAudioVideoLoop (void);
	virtual VO_U32	PlaybackAudioVideo (VO_U32 nFlag);

	static	int		voSourceStreamCallBack (long EventCode, long * EventParam1, long * userData);

#ifdef _WIN32
	voThreadHandle	m_hThreadIdle;
	VO_BOOL			m_bStopIdle;
	VO_U32			m_nThreadIdle;

	static	VO_U32	PlaybackIdleLoopProc (VO_PTR pParam);
	virtual VO_U32	PlaybackIdleLoop (void);
#endif // _WIN32

};

#endif // __CPlayGraph_H__
