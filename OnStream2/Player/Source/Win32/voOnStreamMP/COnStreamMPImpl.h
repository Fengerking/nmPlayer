
/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
#pragma once

#include "windows.h"
#include "voOnStreamType.h"
#include "voOnStreamSourceType.h"
#include "voSubTitleManager.h"
#include "voString.h"

class OnStreamEngine;
class OnStreamSrc;
class voSubTitleManager;

class COnStreamMPImpl
{
public:
	COnStreamMPImpl(int nPlayerType, void* pInitParam, int nInitParamFlag);
	~COnStreamMPImpl(void);

	int SetView( void* pView);
	int Open(void* pSource, int nFlag, int nSourceType);
	int GetProgramCount(int* pProgramCount);
	int GetProgramInfo(int nProgramIndex, VOOSMP_SRC_PROGRAM_INFO** ppProgramInfo);
	int GetCurTrackInfo(int nTrackType, VOOSMP_SRC_TRACK_INFO** ppTrackInfo );
	int GetSample(int nTrackType, void* pSample);
	int SelectProgram(int nProgram);
	int SelectStream(int nStream);
	int SelectTrack( int nTrack);
	int SelectLanguage( int nIndex);
	int GetLanguage(VOOSMP_SUBTITLE_LANGUAGE_INFO** ppLangInfo);
	int Close();
	int Run();
	int Pause();
	int Stop();
	int GetPos();
	int SetPos(int nPos);
	int GetDuration( long long* pDuration);
	int GetParam( int nParamID, void* pValue);
	int SetParam(int nParamID, void* pValue);

protected:

	int Init();
	int Uninit();

	virtual int			HandleEvent (int nID, void * pParam1, void * pParam2);
	static	int			OnListener (void * pUserData, int nID, void * pParam1, void * pParam2);

	//subtitle
	void				CreateCloseCaptionUI();
	void				StartCloseCaptionTimer();
	void				CloseCloseCaptionTimer();
	void				HandleTimerEvent(int idEvent);
	int 				GetStatus (VOOSMP_STATUS * pStatus);
	void				DeleteSubtitleManager();
	float				ComputeXYRate();
	/* cap.xml parser function*/
	int					CapPerfDataExtractor();

private:

	OnStreamSrc *		mpSource;
	OnStreamEngine *	mpEngine;

	VOOSMP_INIT_PARAM   mInitParam;
	int					mnInitParamFlag;
	int					mnPlayerType;

	HWND				mhWnd;
	int					mnColorType;
	VOOSMP_RECT			mrcDraw;

	VOOSMP_LISTENERINFO		m_sListener;
	VOOSMP_LISTENERINFO		mCallbackFunc;

	//for subtitle
	bool					mEnableInnerCloseCaption;
	voSubTitleManager *		m_ccMan;

	VOOSMP_ASPECT_RATIO		m_aspectRatio;
	int						m_nVideoWidth;
	int						m_nVideoHeight;

	TCHAR					m_szDRMFile[1024];
	TCHAR					m_szDRMAPI[256];

	TCHAR					m_szAdapterDRMFile[1024];
	TCHAR					m_szAdapterDRMAPI[256];
	void*					m_pAapterDRMObject;

	VO_CHAR				m_pProxyhost[512];
	int						m_nProxyport;
	VO_CHAR				m_pUseragent[512];

	VOOSMP_SRC_HTTP_HEADER	m_sHttpHeader;
	VOOSMP_SRC_HTTP_PROXY		m_sHttpProxy;

	VO_CHAR       				m_pLiscenseText[512];
	TCHAR       				m_pLiscenseFilePath[1024];

	VOOSMP_RENDER_TYPE			m_renderType;
	bool						m_bID3Picture;
	bool						m_bDrawVideoDirectly;

	/* cap.xml for auto BA configuration*/
	VOOSMP_PERF_DATA			m_OSCapData;
};
