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

#include "OnStreamSrc.h"

OnStreamSrc::OnStreamSrc(int nPlayerType, void* pInitParam, int nInitParamFlag)
	: m_hDll (NULL)
	, m_hSource (NULL)
	, m_pLibop(NULL)
	, m_hThread(NULL)
	, m_dwThreadID(0)
	, m_bSubtitlePathSet(false)
{
	if(((nInitParamFlag | VOOSMP_FLAG_INIT_LIBOP) != 0) && pInitParam != NULL )
	{
		m_pLibop = (VOOSMP_LIB_FUNC *)((VOOSMP_INIT_PARAM *)pInitParam)->pLibOP;
	}

	memset(m_szFileName,0,sizeof(m_szFileName));
	memset(&m_pSourceAPI, 0, sizeof(voOSMPSourceAPI));
	LoadDll ();
}

OnStreamSrc::~OnStreamSrc ()
{
	if(m_hThread)
	{
		::TerminateThread(m_hThread,0);
		CloseHandle(m_hThread);
	}

	m_hThread = NULL;
	m_dwThreadID = 0;

	Uninit();

	if (m_hDll != NULL && m_pLibop != NULL && m_pLibop->FreeLib != NULL)
	{
		m_pLibop->FreeLib(m_pLibop->pUserData, m_hDll, 0);
	}
}

int	OnStreamSrc::Init(void* pSource, int nSourceFlag, int nSourceType, void* pInitParam, int nInitParamFlag)
{
	if (m_pSourceAPI.Init == NULL)
		return VOOSMP_ERR_Pointer;

	Uninit();
	m_pSourceAPI.Create( &m_hSource );
	int nRC = m_pSourceAPI.Init(m_hSource, pSource, nSourceFlag, nSourceType, pInitParam, nInitParamFlag);

   return nRC;
}

int	OnStreamSrc::Uninit()
{
	if (m_hSource == NULL || m_pSourceAPI.Uninit == NULL)
		return VOOSMP_ERR_Pointer;

	Close();

	 m_pSourceAPI.Uninit(m_hSource);
	 m_pSourceAPI.Destroy( m_hSource );
	 m_hSource = NULL;

	return 0;
}

int	OnStreamSrc::Open()
{
	if (m_hSource == NULL || m_pSourceAPI.Open == NULL)
		return VOOSMP_ERR_Pointer;
	m_bSubtitlePathSet = false;

	return m_pSourceAPI.Open (m_hSource);
}

int	OnStreamSrc::Close()
{
	if (m_hSource == NULL || m_pSourceAPI.Close == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.Close (m_hSource);
}

int OnStreamSrc::Run (void)
{
	if (m_hSource == NULL || m_pSourceAPI.Run == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.Run (m_hSource);
}

int OnStreamSrc::Pause (void)
{
	if (m_hSource == NULL || m_pSourceAPI.Pause == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.Pause (m_hSource);
}

int OnStreamSrc::Stop (void)
{
	if (m_hSource == NULL || m_pSourceAPI.Stop == NULL)
		return VOOSMP_ERR_None;

	return m_pSourceAPI.Stop(m_hSource);
}

int OnStreamSrc::GetDuration (long long* pDuration)
{
	if (m_hSource == NULL || m_pSourceAPI.GetDuration == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.GetDuration(m_hSource, pDuration);
}

int OnStreamSrc::SetPos(long long *pCurPos)
{
	if (m_hSource == NULL || m_pSourceAPI.SetPos == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.SetPos(m_hSource, pCurPos);
}

int OnStreamSrc::GetSample(int nTrackType, void* pSample)
{
	if (m_hSource == NULL || m_pSourceAPI.GetSample == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.GetSample(m_hSource, nTrackType, pSample);
}


int OnStreamSrc::GetProgramCount(int* pProgramCount)
{
	if (m_hSource == NULL || m_pSourceAPI.GetProgramCount == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.GetProgramCount(m_hSource, pProgramCount);
}

int OnStreamSrc::GetProgramInfo(int nProgram, VOOSMP_SRC_PROGRAM_INFO** ppProgramInfo)
{
	if (m_hSource == NULL || m_pSourceAPI.GetProgramInfo == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.GetProgramInfo(m_hSource, nProgram, ppProgramInfo);
}

int OnStreamSrc::GetCurTrackInfo(int nTrackType, VOOSMP_SRC_TRACK_INFO** ppTrackInfo)
{
	if (m_hSource == NULL || m_pSourceAPI.GetCurTrackInfo == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.GetCurTrackInfo(m_hSource, nTrackType, ppTrackInfo);
}

int OnStreamSrc::SelectProgram(int nProgram)
{
	if (m_hSource == NULL || m_pSourceAPI.SelectProgram == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.SelectProgram(m_hSource, nProgram);
}

int OnStreamSrc::SelectStream(int nStream)
{
	if (m_hSource == NULL || m_pSourceAPI.SelectStream == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.SelectStream(m_hSource, nStream);
}

int OnStreamSrc::SelectTrack(int nTrack)
{
	if (m_hSource == NULL || m_pSourceAPI.SelectTrack == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.SelectTrack(m_hSource, nTrack);
}

int OnStreamSrc::SelectLanguage( int nIndex)
{
	if (m_hSource == NULL || m_pSourceAPI.SelectLanguage == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.SelectLanguage(m_hSource, nIndex);
}

int OnStreamSrc::GetLanguage(  VOOSMP_SUBTITLE_LANGUAGE_INFO** ppLangInfo)
{
	if (m_hSource == NULL || m_pSourceAPI.GetLanguage == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.GetLanguage(m_hSource, ppLangInfo);
}

int OnStreamSrc::SendBuffer(const VOOSMP_BUFFERTYPE& buffer)
{
	if (m_hSource == NULL || m_pSourceAPI.SendBuffer == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.SendBuffer(m_hSource, buffer);
}

int OnStreamSrc::GetParam (int nID, void * pValue)
{
    if(nID == VOOSMP_PID_GET_ONSTREAMSRC_HANDLE && pValue != NULL)
    {
        *(int *)pValue = (int)this; 
        return VOOSMP_ERR_None;
    }

	if (m_hSource == NULL || m_pSourceAPI.GetParam == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.GetParam(m_hSource, nID, pValue);
}

int OnStreamSrc::SetParam (int nID, void * pValue)
{
	if (m_hSource == NULL || m_pSourceAPI.SetParam == NULL)
		return VOOSMP_ERR_Pointer;
	if(VOOSMP_PID_SUBTITLE_FILE_NAME == nID && pValue!=NULL)
	{
		HANDLE hFile = CreateFile((TCHAR *)pValue, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, (DWORD) 0, NULL);
		if(INVALID_HANDLE_VALUE == hFile && m_hThread==NULL)
		{//TO download it;
			memset(m_szFileName,0,sizeof(m_szFileName));
			_tcscpy(m_szFileName,(TCHAR *)pValue);
			m_hThread = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)GetHttpFileProc, (LPVOID)this, 0, &m_dwThreadID);
			 return 0;
		}
		else
		{
			 m_bSubtitlePathSet = true;
			::CloseHandle(hFile);
		}
	}

	return m_pSourceAPI.SetParam(m_hSource, nID, pValue);
}

void OnStreamSrc::GetHttpFile ()
{
	TCHAR szFileName[512];
	memset(szFileName,0,sizeof(szFileName));
#ifndef WINCE
	 if (S_OK == URLDownloadToCacheFile(NULL, (TCHAR *)m_szFileName, szFileName,512, NULL,NULL)) 
	 {
		 if(m_hSource!=NULL && m_pSourceAPI.SetParam != NULL)
		 {
			 m_pSourceAPI.SetParam(m_hSource, VOOSMP_PID_SUBTITLE_FILE_NAME, (void *)szFileName);
			 m_bSubtitlePathSet = true;
		 }
		 ::DeleteFile(szFileName);
	 }
#endif //WINCE
	if(m_hThread)
		CloseHandle(m_hThread);
	m_hThread = NULL;
	m_dwThreadID = 0;
}

DWORD OnStreamSrc::GetHttpFileProc (LPVOID pParam)
{
	OnStreamSrc * pSrc = (OnStreamSrc *)pParam;
	if(pSrc != NULL)
	{
		pSrc->GetHttpFile();
	}
	

	return 0;
}


int OnStreamSrc::GetVideoCount()
{
	if (m_hSource == NULL || m_pSourceAPI.GetVideoCount == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.GetVideoCount(m_hSource);
}

int OnStreamSrc::GetAudioCount()
{
	if (m_hSource == NULL || m_pSourceAPI.GetAudioCount == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.GetAudioCount(m_hSource);
}

int OnStreamSrc::GetSubtitleCount()
{
	if (m_hSource == NULL || m_pSourceAPI.GetSubtitleCount == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.GetSubtitleCount(m_hSource);
}

int OnStreamSrc::SelectVideo( int nIndex)
{
	if (m_hSource == NULL || m_pSourceAPI.SelectVideo == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.SelectVideo(m_hSource , nIndex);
}

int OnStreamSrc::SelectAudio(int nIndex)
{
	if (m_hSource == NULL || m_pSourceAPI.SelectAudio == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.SelectAudio(m_hSource , nIndex);
}

int OnStreamSrc::SelectSubtitle( int nIndex)
{
	if (m_hSource == NULL || m_pSourceAPI.SelectSubtitle == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.SelectSubtitle(m_hSource , nIndex);
}

bool OnStreamSrc::IsVideoAvailable( int nIndex)
{
	if (m_hSource == NULL || m_pSourceAPI.IsVideoAvailable == NULL)
		return false;

	return m_pSourceAPI.IsVideoAvailable(m_hSource , nIndex);
}

bool OnStreamSrc::IsAudioAvailable( int nIndex)
{
	if (m_hSource == NULL || m_pSourceAPI.IsAudioAvailable == NULL)
		return false;

	return m_pSourceAPI.IsAudioAvailable(m_hSource , nIndex);
}

bool OnStreamSrc::IsSubtitleAvailable( int nIndex)
{
	if (m_hSource == NULL || m_pSourceAPI.IsSubtitleAvailable == NULL)
		return false;

	return m_pSourceAPI.IsSubtitleAvailable(m_hSource , nIndex);
}

int OnStreamSrc::CommitSelection()
{
	if (m_hSource == NULL || m_pSourceAPI.CommitSelection == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.CommitSelection(m_hSource);
}

int OnStreamSrc::ClearSelection()
{
	if (m_hSource == NULL || m_pSourceAPI.ClearSelection == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.ClearSelection(m_hSource);
}

int OnStreamSrc::GetVideoProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
	if (m_hSource == NULL || m_pSourceAPI.GetVideoProperty == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.GetVideoProperty(m_hSource , nIndex , ppProperty);
}

int OnStreamSrc::GetAudioProperty( int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
	if (m_hSource == NULL || m_pSourceAPI.GetAudioProperty == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.GetAudioProperty(m_hSource , nIndex , ppProperty);
}

int OnStreamSrc::GetSubtitleProperty( int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
	if (m_hSource == NULL || m_pSourceAPI.GetSubtitleProperty == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.GetSubtitleProperty(m_hSource , nIndex , ppProperty);
}

int OnStreamSrc::GetCurrPlayingTrackIndex( VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
	if (m_hSource == NULL || m_pSourceAPI.GetCurrPlayingTrackIndex == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.GetCurrPlayingTrackIndex(m_hSource , pCurrIndex);
}

int OnStreamSrc::GetCurrTrackSelection( VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
	if (m_hSource == NULL || m_pSourceAPI.GetCurrTrackSelection == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.GetCurrTrackSelection(m_hSource , pCurrIndex);
}

int OnStreamSrc::GetCurrSelectedTrackIndex( VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
	if (m_hSource == NULL || m_pSourceAPI.GetCurrSelectedTrackIndex == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pSourceAPI.GetCurrSelectedTrackIndex(m_hSource , pCurrIndex);
}

int OnStreamSrc::LoadDll (void)
{
	if(m_pLibop == NULL || m_pLibop->FreeLib == NULL || m_pLibop->LoadLib == NULL || m_pLibop->GetAddress == NULL)
		return -1;
	
	if (m_hDll != NULL)
	{
		m_pLibop->FreeLib(m_pLibop->pUserData, m_hDll, 0);
		m_hDll = NULL;
	}

	m_hDll = (HMODULE)m_pLibop->LoadLib(m_pLibop->pUserData, "voOSSource", 0);

	if (m_hDll == NULL)
		return -1;

	VOGETONSTREAMSOURCEAPI pGetAPI = (VOGETONSTREAMSOURCEAPI) m_pLibop->GetAddress (m_pLibop->pUserData, m_hDll, "voGetOnStreamSourceAPI", 0);

	if (pGetAPI == NULL)
		return -1;

	pGetAPI(&m_pSourceAPI);

	if(m_pSourceAPI.Init == NULL)
		return -1;

	return 0;
}
