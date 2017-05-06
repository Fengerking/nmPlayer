/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
File:		CBaseEdit.h

Contains:	CBaseEdit header file

Written by:	 Yu Wei based on Bangfei's code

Change History (most recent first):
2010-04-26		YW			Create file

*******************************************************************************/

#ifndef __CBaseEdit_H__
#define __CBaseEdit_H__

#include "voFile.h"

#include "CFileSink.h"

#include "CBaseController.h"


#include "CBaseSource.h"
#include "CAudioDecoder.h"
#include "CVideoDecoder.h"
#include "CAudioRender.h"
#include "CVideoRender.h"

#include "voThread.h"


class CBaseEdit 
{
protected:
	VO_FILE_SOURCE			m_Source;
	VO_TCHAR                m_szDumpFilePath[2048];
	VO_TCHAR                m_szDumpTmpDir[2048];

	VO_U32                  m_nVideoFirstSample;
	VO_U32                  m_nAudioFirstSample;

	VO_BYTE*                m_pAudioHeader;
	VO_BYTE*                m_pVideoHeader;
	VO_U32                  m_nAudioHeaderSize;
	VO_U32                  m_nVideoHeaderSize;

	VO_S64                  m_nDumpVideoTime;
	VO_S64                  m_nDumpAudioTime;

	CBaseController         m_Controller;

	VOMMPlAYSTATUS		    m_status;

	voCMutex				m_csEdit;

	VO_PTR					m_hInst;
	VO_MEM_OPERATOR *		m_pMemOP;
	VO_FILE_OPERATOR *		m_pFileOP;

	CBaseConfig *		    m_pCodecCfg;
	CBaseSource *		    m_pSource;
	VO_S32				    m_nAudioTrack;
	VO_S32				    m_nVideoTrack;

	CAudioDecoder *		    m_pAudioDec;
	CVideoDecoder *		    m_pVideoDec;
	CAudioRender *		    m_pAudioRender;
	CVideoRender *		    m_pVideoRender;

	VO_PTR						m_pUserData;

	VO_LIB_OPERATOR *			m_pLibOP;

	VO_SOURCEDRM_CALLBACK *		m_pDrmCallBack;
	VO_VOMM_CB_VIDEORENDER	    m_cbVideoRender;
	VO_VOMM_CB_AUDIORENDER		m_cbAudioRender;
	VOMMPlayCallBack			m_pCallBack;

	VO_VOMM_CB_STREAMING    	m_cbStreaming;


	VO_AUDIO_FORMAT		m_fmtAudio;
	VO_VIDEO_FORMAT		m_fmtVideo;	
	VO_BOOL				m_bVOSDK;
	VO_BOOL				m_bForThumbnail;

	VO_U32				m_nDuration;
	VO_S32				m_nCurPos;

	VO_S64				m_nStartTime;
	VO_S64				m_nPlayedTime;

	VO_S32				m_nAudioBufferTime;

	VO_PTR				m_hView;
	VO_RECT				m_rcView;

	VO_IV_ZOOM_MODE		m_nZoomMode;
	VO_IV_ASPECT_RATIO	m_nRatio;
	VO_RECT				m_rcDisp;
	VO_U32				m_nSeekMode;

	VO_U32				m_nSeekTime;
	VO_U32				m_nSeekPos;

	VO_S64				m_llVideoStartPos;

	VO_BOOL					m_bAudioEndOfStream;
	VO_BOOL					m_bVideoEndOfStream;

	VO_SOURCE_SAMPLE		m_videoSourceSample;
	VO_SOURCE_SAMPLE		m_audioSourceSample;

	VO_U32					m_nThreadNum;
	voThreadHandle			m_hThreadAudio;
	VO_BOOL					m_bStopAudio;
	VO_BOOL					m_bAudioPlaying;

	voThreadHandle			m_hThreadVideo;
	VO_BOOL					m_bStopVideo;
	VO_BOOL					m_bVideoPlaying;

	VO_S64					m_nFirstAudioPos;
	VO_S64					m_nFirstVideoPos;

	VO_CODECBUFFER			m_videoSourceBuffer;

	VO_BOOL					m_bReleased;

	VO_PBYTE				m_pAudioRenderData;

	VO_SOURCE_SAMPLE		m_videoNextKeySample;
	VO_BOOL					m_bVideoPlayNextFrame;

	VO_TCHAR			    m_szError[128];

	CBaseConfig *			m_pConfig;
	
	 

public:
	// Used to control the image drawing
	CBaseEdit (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP);
	virtual ~CBaseEdit (void);

	virtual VO_U32 		Create (VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength) = 0;

	virtual VO_U32	PlaybackAudioFrame () = 0;
	virtual VO_U32	PlaybackVideoFrame (void) = 0;


	static	VO_U32	PlaybackAudioLoopProc (VO_PTR pParam);
	static	VO_U32	PlaybackVideoLoopProc (VO_PTR pParam);

	virtual VO_U32	PlaybackAudioLoop (void) = 0 ;

	virtual VO_U32	PlaybackVideoLoop (void) = 0;

	virtual VO_U32 	Run (void) = 0;
	virtual VO_U32 	Pause (void);
	virtual VO_U32 	Stop (void);

	virtual VO_U32 	GetDuration (VO_U32 * pDuration);
	virtual VO_U32 	GetCurPos (VO_S32 * pPos);
	virtual VO_U32 	SetCurPos (VO_S32 nPos) = 0;
	virtual VO_U32 	GetParam (VO_U32 nID, VO_PTR pValue);

	virtual VO_U32	SetConfig (CBaseConfig * pConfig);

	virtual void	SetDrmCB (VO_SOURCEDRM_CALLBACK * pDrmCB) {m_pDrmCallBack = pDrmCB;}
	virtual VO_U32  SetParam (VO_U32 nID, VO_PTR pValue);

	virtual VO_U32	SetViewInfo (VO_PTR hView, VO_RECT * pRect);
	virtual VO_U32	SetCallBack (VOMMPlayCallBack pCallBack, VO_PTR pUserData);


protected:

	virtual VO_U32		Release (void) = 0;

	virtual VO_S64		GetPlayingTime (void);

	virtual VO_U32		ReadVideoData (VO_U32 nTrack, VO_PTR pTrackData);
	virtual VO_U32		ReadAudioData (VO_U32 nTrack, VO_PTR pTrackData);

	virtual VO_U32		EndOfSource (void);
	virtual VO_U32      UpdateWithConfig (void);

	virtual VO_U32		DecoderVideoData (VO_VIDEO_BUFFER * pOutput, VO_VIDEO_FORMAT * pVideoFormat);
	

};

#endif // __CBaseEdit_H__
