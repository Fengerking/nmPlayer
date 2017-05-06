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

#ifndef __OnStreamSrc_H__
#define __OnStreamSrc_H__

#include <windows.h>
#include <tchar.h>

#include "voOnStreamType.h"
#include "voOnStreamSource.h"

class OnStreamSrc
{
public:
	// Used to control the image drawing
	OnStreamSrc (int nPlayerType, void* pInitParam, int nInitParamFlag);
	~OnStreamSrc (void);

	int Init(void* pSource, int nSourceFlag, int nSourceType, void* pInitParam, int nInitParamFlag);
	int Uninit();

	int Open();
	int Close();
	int Run();
	int Pause();
	int Stop();
	int SetPos(long long* pTimeStamp);
	int GetDuration(long long* pDuration);
	int GetSample(int nTrackType, void* pSample);
	int GetProgramCount(int* pProgramCount);
	int GetProgramInfo(int nProgram, VOOSMP_SRC_PROGRAM_INFO** ppProgramInfo);
	int GetCurTrackInfo(int nTrackType, VOOSMP_SRC_TRACK_INFO** ppTrackInfo);
	int SelectProgram(int nProgram);
	int SelectStream(int nStream);
	int SelectTrack(int nTrack);
	int SelectLanguage( int nIndex);
    int GetLanguage( VOOSMP_SUBTITLE_LANGUAGE_INFO** ppLangInfo);
	int SendBuffer(const VOOSMP_BUFFERTYPE& buffer);
	int GetParam(int nParamID, void* pParam);
	int SetParam(int nParamID, void* pParam);
	int GetVideoCount();
    int GetAudioCount();
    int GetSubtitleCount();
    int SelectVideo( int nIndex);
    int SelectAudio(int nIndex);
    int SelectSubtitle(int nIndex);
    bool IsVideoAvailable(int nIndex);
    bool IsAudioAvailable( int nIndex);
    bool IsSubtitleAvailable( int nIndex);
    int CommitSelection();
    int ClearSelection();
    int GetVideoProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
    int GetAudioProperty( int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
    int GetSubtitleProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
	int GetCurrPlayingTrackIndex(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);
    int GetCurrTrackSelection(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);
	int GetCurrSelectedTrackIndex(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);
	bool IsSubtitlePathSet(){return m_bSubtitlePathSet;};

	bool IsInit(){return m_hSource == NULL ? false : true; };

protected:
	int					LoadDll (void);
	static	DWORD		GetHttpFileProc (LPVOID pParam);
	void				GetHttpFile ();

protected:
	HMODULE				m_hDll;
	void *				m_hSource;
	VOOSMP_LIB_FUNC *	m_pLibop;

	TCHAR				m_szFileName[512];
	HANDLE				m_hThread;
	DWORD				m_dwThreadID;

	bool				m_bSubtitlePathSet;

	voOSMPSourceAPI		m_pSourceAPI;
};

#endif // __OnStreamEngine_H__
