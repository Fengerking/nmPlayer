#pragma once

#include <tchar.h>

#include <ddraw.h>

#include "CvoOnStreamMP.h"

class CPluginFunTest:public CvoOnStreamMP
{
public:
  CPluginFunTest(LPCTSTR szWorkingPath=_T(""));
  ~CPluginFunTest(void);

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
  int GetCurrTrackSelection(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);

  int HandleEvent (int nID, void * pParam1, void * pParam2);


private:
  bool			CreateDD (void);
  bool			ReleaseDD (void);
  static DWORD WINAPI TextOutTest(void * pm);

private:
  bool      m_bStoped;
  bool      m_bExit;

  HWND					m_hWnd;
  LPDIRECTDRAW           m_pDD;			// DirectDraw 对象指针
  LPDIRECTDRAWSURFACE    m_pDDSPrimary;  // DirectDraw 主表面指针
  LPDIRECTDRAWSURFACE    m_pDDSOffScr;	// DirectDraw 离屏表面指针
  DDSURFACEDESC			     m_ddsd;			// DirectDraw 表面描述
  HDC                    m_hdc;

  HANDLE                 m_hThread;

  VOOSMP_SRC_TRACK_PROPERTY m_srcTrackProperty;
  VOOSMP_SRC_TRACK_ITEM_PROPERTY* m_pItemProperties;

  LONG                   m_nDuration;
  LONG                   m_nPos;
  VOOSMP_SRC_CURR_TRACK_INDEX curPlayingIndex;
};
