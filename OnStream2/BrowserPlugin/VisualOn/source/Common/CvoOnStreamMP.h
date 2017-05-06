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

#include "voOnStreamMP.h"
#include "OnStreamSrc.h"
#include "voCMutex.h"
#include "VOOSMPType.h"

typedef int	(* ONUILISTENER) (void * pUserData, int nID, void * pParam1, void * pParam2);

class CvoOnStreamMP
{
public:
	CvoOnStreamMP(LPCTSTR szWorkingPath=_T(""));
	~CvoOnStreamMP(void);

	int Init();
	int Uninit();
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
	int GetDuration();
	int GetParam( int nParamID, void* pValue);
	int SetParam(int nParamID, void* pValue);

  int GetVideoCount();
  int GetAudioCount();
  int GetSubtitleCount();
  int SelectVideo(int nIndex);
  int SelectAudio(int nIndex);
  int SelectSubtitle(int nIndex);
  bool IsVideoAvailable(int nIndex);
  bool IsAudioAvailable(int nIndex);
  bool IsSubtitleAvailable(int nIndex);
  int CommitSelection();
  int ClearSelection();
  int GetVideoProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
  int GetAudioProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
  int GetSubtitleProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
  int GetCurrPlayingTrackIndex(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);
  int GetCurrTrackSelection(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);
  int GetCurrSelectedTrackIndex(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);

  long GetMinPosition();
  long GetMaxPosition();

  bool IsLiveStreaming();

  int EnableLiveStreamingDVRPosition(BOOL bEnable);

  int SetHTTPHeader(CHAR* headerName, CHAR* headerValue);

  int GetVideoDecodingBitrate(char* pbuf);
  int GetAudioDecodingBitrate(char* pbuf);

	int					HandleEvent (int nID, void * pParam1, void * pParam2);
	static	int			OnListener (void * pUserData, int nID, void * pParam1, void * pParam2);

  //nFlag==1: pUserData is the workpath
	static void *		mmLoadLib (void * pUserData, char * pLibName, int nFlag);
	static void *		mmGetAddress (void * pUserData, void * hLib, char * pFuncName, int nFlag);
	static int			mmFreeLib (void * pUserData, void * hLib, int nFlag);

	void				SetListener (void * pUserData, ONUILISTENER listen) {m_lsnUI = listen;}
	ONUILISTENER		m_lsnUI;

  int       			Redraw(BOOL bDraw = TRUE);
  int					updateViewRegion(RECT&);

  BOOL                  IsWin8OS();

private:	
	void LoadDll();

private:
	voOnStreamMediaPlayerAPI	mMPAPI;
	void *						        mpMPHandle;

	HMODULE						        m_hDll;

	VOOSMP_LIB_FUNC				    mLibFunc;
	VOOSMP_INIT_PARAM			    mInitParam;

	TCHAR						          mszAppPath[MAX_PATH];

	VOOSMP_LISTENERINFO			  mListener;

  OnStreamSrc *             mpSource;

  voCMutex                  m_Mutex;

  BOOL                      m_bEnableSubtitle;

  int                       m_nIsLive;
  int                       m_nAudioTrackSelectedIdx;
  int                       m_nSubtitleTrackSelectedIdx;

  BOOL                      m_bOpenedSucess;

  BOOL                      m_bLiveStreamingDVRPosition;
};
