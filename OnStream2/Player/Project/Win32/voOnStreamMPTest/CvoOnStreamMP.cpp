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
#include "StdAfx.h"
#include "CvoOnStreamMP.h"
#include "voOSFunc.h"

CvoOnStreamMP::CvoOnStreamMP(LPCTSTR szWorkingPath)
: mpMPHandle(NULL)
, m_hDll(NULL)
, mpSource(NULL)
{
	_tcscpy(mszAppPath,szWorkingPath);

	memset(&mInitParam , 0 , sizeof(mInitParam));
	mLibFunc.pUserData = this;
	mLibFunc.FreeLib	= mmFreeLib;
	mLibFunc.GetAddress = mmGetAddress;
	mLibFunc.LoadLib	= mmLoadLib;

	mInitParam.pLibOP = &mLibFunc;
	mInitParam.pWorkingPath = mszAppPath;

	mListener.pUserData = this;
	mListener.pListener = OnListener;

	memset(&mMPAPI , 0 , sizeof(mMPAPI));
	LoadDll();
}

CvoOnStreamMP::~CvoOnStreamMP(void)
{
	if(m_hDll != NULL)
	{
		mmFreeLib(NULL , m_hDll , 0);
		m_hDll = NULL;
	}
}

int CvoOnStreamMP::Init()
{
	if(mMPAPI.Init == NULL)
		return VOOSMP_ERR_Pointer;

	int nRC = mMPAPI.Init((void **)&mpMPHandle , VOOSMP_VOME2_PLAYER , &mInitParam , VOOSMP_FLAG_INIT_LIBOP|VOOSMP_FLAG_INIT_WORKING_PATH);

	SetParam(VOOSMP_PID_LISTENER , &mListener);

	int nRC2 = nRC | GetParam(20130131 , &mpSource);
	return nRC;
}

int CvoOnStreamMP::Uninit()
{
	if(mpMPHandle == NULL || mMPAPI.Uninit == NULL)
		return VOOSMP_ERR_Pointer;

	mMPAPI.Uninit(mpMPHandle);
	mpMPHandle = NULL;

	return VOOSMP_ERR_None;
}

int CvoOnStreamMP::SetView( void* pView)
{
	if(mpMPHandle == NULL || mMPAPI.SetView == NULL)
		return VOOSMP_ERR_Pointer;

	return mMPAPI.SetView(mpMPHandle ,pView);
}

int CvoOnStreamMP::Open(void* pSource, int nFlag, int nSourceType)
{
	if(mpMPHandle == NULL || mMPAPI.Open == NULL)
		return VOOSMP_ERR_Pointer;

	return mMPAPI.Open(mpMPHandle , pSource , nFlag , nSourceType);
}

int CvoOnStreamMP::GetProgramCount(int* pProgramCount)
{
	if(mpMPHandle == NULL || mMPAPI.GetProgramCount == NULL)
		return VOOSMP_ERR_Pointer;

	return mMPAPI.GetProgramCount(mpMPHandle , pProgramCount);
}

int CvoOnStreamMP::GetProgramInfo(int nProgramIndex, VOOSMP_SRC_PROGRAM_INFO** ppProgramInfo)
{
	if(mpMPHandle == NULL || mMPAPI.GetProgramInfo == NULL)
		return VOOSMP_ERR_Pointer;

	return mMPAPI.GetProgramInfo(mpMPHandle , nProgramIndex , ppProgramInfo);
}

int CvoOnStreamMP::GetCurTrackInfo(int nTrackType, VOOSMP_SRC_TRACK_INFO** ppTrackInfo )
{
	if(mpMPHandle == NULL || mMPAPI.GetCurTrackInfo == NULL)
		return VOOSMP_ERR_Pointer;

	return mMPAPI.GetCurTrackInfo(mpMPHandle , nTrackType , ppTrackInfo);
}

int CvoOnStreamMP::GetSample(int nTrackType, void* pSample)
{
	if(mpMPHandle == NULL || mMPAPI.GetSample == NULL)
		return VOOSMP_ERR_Pointer;

	return mMPAPI.GetSample(mpMPHandle , nTrackType , pSample);
}

int CvoOnStreamMP::SelectProgram(int nProgram)
{
	if(mpMPHandle == NULL || mMPAPI.SelectProgram == NULL)
		return VOOSMP_ERR_Pointer;

	return mMPAPI.SelectProgram(mpMPHandle , nProgram);
}

int CvoOnStreamMP::SelectStream(int nStream)
{
	if(mpMPHandle == NULL || mMPAPI.SelectStream == NULL)
		return VOOSMP_ERR_Pointer;

	return mMPAPI.SelectStream(mpMPHandle , nStream);
}

int CvoOnStreamMP::SelectTrack( int nTrack)
{
	if(mpMPHandle == NULL || mMPAPI.SelectTrack == NULL)
		return VOOSMP_ERR_Pointer;

	return mMPAPI.SelectTrack(mpMPHandle , nTrack);
}

int CvoOnStreamMP::SelectLanguage( int nIndex)
{
	if(mpMPHandle == NULL || mMPAPI.SelectLanguage == NULL)
		return VOOSMP_ERR_Pointer;

	return mMPAPI.SelectLanguage(mpMPHandle , nIndex);
}

int CvoOnStreamMP::GetLanguage(VOOSMP_SUBTITLE_LANGUAGE_INFO** ppLangInfo)
{
	if(mpMPHandle == NULL || mMPAPI.GetLanguage == NULL)
		return VOOSMP_ERR_Pointer;

	return mMPAPI.GetLanguage(mpMPHandle ,ppLangInfo);
}

int CvoOnStreamMP::Close()
{
	if(mpMPHandle == NULL || mMPAPI.Close == NULL)
		return VOOSMP_ERR_Pointer;

	return mMPAPI.Close(mpMPHandle);
}

int CvoOnStreamMP::Run()
{
	if(mpMPHandle == NULL || mMPAPI.Run == NULL)
		return VOOSMP_ERR_Pointer;

	return mMPAPI.Run(mpMPHandle);
}

int CvoOnStreamMP::Pause()
{
	if(mpMPHandle == NULL || mMPAPI.Pause == NULL)
		return VOOSMP_ERR_Pointer;

	return mMPAPI.Pause(mpMPHandle);
}

int CvoOnStreamMP::Stop()
{
	if(mpMPHandle == NULL || mMPAPI.Stop == NULL)
		return VOOSMP_ERR_Pointer;

	return mMPAPI.Stop(mpMPHandle);
}

int CvoOnStreamMP::GetPos()
{
	if(mpMPHandle == NULL || mMPAPI.GetPos == NULL)
		return VOOSMP_ERR_Pointer;

	return mMPAPI.GetPos(mpMPHandle);
}

int CvoOnStreamMP::SetPos(int nPos)
{
	if(mpMPHandle == NULL || mMPAPI.SetPos == NULL)
		return VOOSMP_ERR_Pointer;

	return mMPAPI.SetPos(mpMPHandle , nPos);
}

int CvoOnStreamMP::GetDuration()
{
	if(mpMPHandle == NULL || mMPAPI.GetDuration == NULL)
		return VOOSMP_ERR_Pointer;

	long long llDuration = 0;

	mMPAPI.GetDuration(mpMPHandle , &llDuration);

	return (int)llDuration;
}

int CvoOnStreamMP::GetParam( int nParamID, void* pValue)
{
	if(mpMPHandle == NULL || mMPAPI.GetParam == NULL)
		return VOOSMP_ERR_Pointer;

	return mMPAPI.GetParam(mpMPHandle , nParamID , pValue);
}

int CvoOnStreamMP::SetParam(int nParamID, void* pValue)
{
	if(mpMPHandle == NULL || mMPAPI.SetParam == NULL)
		return VOOSMP_ERR_Pointer;

	return mMPAPI.SetParam(mpMPHandle , nParamID , pValue);
}

int	CvoOnStreamMP::OnListener (void * pUserData, int nID, void * pParam1, void * pParam2)
{
  if(NULL == pUserData)
    return VOOSMP_ERR_Pointer;

	CvoOnStreamMP * pPlayer = (CvoOnStreamMP *)pUserData;

	return pPlayer->HandleEvent (nID, pParam1, pParam2);
}

int	CvoOnStreamMP::HandleEvent (int nID, void * pParam1, void * pParam2)
{
	if (nID == VOOSMP_CB_PlayComplete)
	{
//		Stop();	
//		m_lsnUI (NULL, nID, 0, 0);
		return VOOSMP_ERR_None;
	}
	else if (nID == VOOSMP_CB_Error)
	{
//		Stop();
		return VOOSMP_ERR_None;
	}
	else if(nID == VOOSMP_CB_CodecNotSupport)
	{
//		Stop();
		return VOOSMP_ERR_None;
	}
	else if (nID == VOOSMP_CB_VideoStartBuff)
	{
		::SetCursor (LoadCursor (NULL, IDC_WAIT));
		return VOOSMP_ERR_None;
	}
	else if (nID == VOOSMP_CB_VideoStopBuff)
	{
		::SetCursor (LoadCursor (NULL, IDC_ARROW));
		return VOOSMP_ERR_None;
	}
	else if (nID == VOOSMP_CB_SourceBuffTime)
	{
		return VOOSMP_ERR_None;
	}
	else if (nID == VOOSMP_CB_BufferStatus)
	{
		return VOOSMP_ERR_None;
	}
	else if (nID == VOOSMP_CB_SeekComplete)
	{
		return VOOSMP_ERR_None;
	}
	else if(nID == VOOSMP_SRC_CB_Open_Finished)
	{
		return VOOSMP_ERR_None;
	}
	else if (nID == VOOSMP_CB_VideoDelayTime)
	{
		return VOOSMP_ERR_None;
	}
	else if (nID == VOOSMP_CB_VideoLastTime)
	{
		return VOOSMP_ERR_None;
	}
	else if (nID == VOOSMP_CB_VideoDropFrames)
	{
		return VOOSMP_ERR_None;
	}
	else if (nID == VOOSMP_CB_VideoFrameRate)
	{
		return VOOSMP_ERR_None;
	}

	return VOOSMP_ERR_Implement;
}

void CvoOnStreamMP::LoadDll()
{
	m_hDll = (HMODULE)mmLoadLib(this, "voOnStreamMP", 1);

	if (m_hDll == NULL)
		return ;

	VOGETONSTREAMPLAYERAPI pGetAPI = (VOGETONSTREAMPLAYERAPI) mmGetAddress (NULL , m_hDll, "voGetOnStreamMediaPlayerAPI", 0);

	if (pGetAPI == NULL)
		return  ;

	pGetAPI(&mMPAPI);

	if(mMPAPI.Init == NULL)
		return ;

}

//nFlag==1: pUserData is the workpath
void * CvoOnStreamMP::mmLoadLib (void * pUserData, char * pLibName, int nFlag)
{
	void* hDll = NULL;

	TCHAR	szDll[1024];
	TCHAR	szPath[1024];
	TCHAR		szLibName[1024];

  CvoOnStreamMP *pOnStream = (CvoOnStreamMP *)pUserData;

	memset (szLibName, 0, 1024);
	::MultiByteToWideChar (CP_ACP, 0, pLibName, -1, szLibName, sizeof (szLibName));

	if (hDll == NULL)
	{
      LPTSTR szAppPath = (LPTSTR)pOnStream->mszAppPath;
			_tcscpy (szDll, szAppPath);
			_tcscat (szDll, szLibName);
			hDll = LoadLibrary (szDll);
	}

	if (hDll == NULL)
	{
		GetModuleFileName (NULL, szPath, sizeof (szPath));

		TCHAR * pPos = _tcsrchr (szPath, _T('\\'));
		if(pPos != NULL)
		{
			*(pPos + 1) = 0;

			_tcscpy (szDll, szPath);
			_tcscat (szDll, szLibName);
			_tcscat (szDll, _T(".Dll"));
			hDll = LoadLibrary (szDll);
		}
	}

	if (hDll == NULL)
	{
		_tcscpy (szDll, szPath);
		_tcscat (szDll, _T("Dll\\"));
		_tcscat (szDll, szLibName);
		hDll = LoadLibrary (szDll);
	}

	if (hDll == NULL)
	{
		_tcscpy (szDll, szLibName);
		hDll = LoadLibrary (szDll);
	}

	return hDll;
}

void * CvoOnStreamMP::mmGetAddress (void * pUserData, void * hLib, char * pFuncName, int nFlag)
{
	if (hLib == NULL)
		return NULL;

	void* pFunc = NULL;

#ifndef WINCE
	pFunc = GetProcAddress ((HMODULE)hLib, pFuncName);
#else //WINCE
	TCHAR		szLibName[1024];

	memset (szLibName, 0, 1024 * sizeof(TCHAR));
	::MultiByteToWideChar (CP_ACP, 0, pFuncName, -1, szLibName, sizeof (szLibName));
	pFunc = GetProcAddress ((HMODULE)hLib, szLibName);
#endif //WINCE

	return pFunc;
}

int CvoOnStreamMP::mmFreeLib (void * pUserData, void * hLib, int nFlag)
{
	if (hLib == NULL)
		return VOOSMP_ERR_Pointer;

	FreeLibrary ((HMODULE)hLib);

	return VOOSMP_ERR_None;
}

int CvoOnStreamMP::GetVideoCount()
{
    if (NULL == mpSource)
    {
        return VOOSMP_ERR_Pointer;
    }

    return mpSource->GetVideoCount();
}

int CvoOnStreamMP::GetAudioCount()
{
    if (NULL == mpSource)
    {
        return VOOSMP_ERR_Pointer;
    }

    return mpSource->GetAudioCount();
}

int CvoOnStreamMP::GetSubtitleCount()
{
    if (NULL == mpSource)
    {
        return VOOSMP_ERR_Pointer;
    }

    return mpSource->GetSubtitleCount();
}

int CvoOnStreamMP::SelectVideo(int nIndex)
{
    if (NULL == mpSource)
    {
        return VOOSMP_ERR_Pointer;
    }

    return mpSource->SelectVideo(nIndex);
}

int CvoOnStreamMP::SelectAudio(int nIndex)
{
    if (NULL == mpSource)
    {
        return VOOSMP_ERR_Pointer;
    }

    return mpSource->SelectAudio(nIndex);
}

int CvoOnStreamMP::SelectSubtitle(int nIndex)
{
    if (NULL == mpSource)
    {
        return VOOSMP_ERR_Pointer;
    }

    return mpSource->SelectSubtitle(nIndex);
}

bool CvoOnStreamMP::IsVideoAvailable(int nIndex)
{
    if (NULL == mpSource)
    {
        return FALSE;
    }

    return mpSource->IsVideoAvailable(nIndex);
}

bool CvoOnStreamMP::IsAudioAvailable(int nIndex)
{
    if (NULL == mpSource)
    {
        return FALSE;
    }

    return mpSource->IsAudioAvailable(nIndex);
}

bool CvoOnStreamMP::IsSubtitleAvailable(int nIndex)
{
    if (NULL == mpSource)
    {
        return FALSE;
    }

    return mpSource->IsSubtitleAvailable(nIndex);
}

int CvoOnStreamMP::CommitSelection()
{
    if (NULL == mpSource)
    {
        return VOOSMP_ERR_Pointer;
    }

    return mpSource->CommitSelection();
}

int CvoOnStreamMP::ClearSelection()
{
    if (NULL == mpSource)
    {
        return VOOSMP_ERR_Pointer;
    }

    return mpSource->ClearSelection();
}

int CvoOnStreamMP::GetVideoProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
    if (NULL == mpSource)
    {
        return VOOSMP_ERR_Pointer;
    }

    return mpSource->GetVideoProperty(nIndex,ppProperty);
}

int CvoOnStreamMP::GetAudioProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
    if (NULL == mpSource)
    {
        return VOOSMP_ERR_Pointer;
    }

    return mpSource->GetAudioProperty(nIndex,ppProperty);
}

int CvoOnStreamMP::GetSubtitleProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
    if (NULL == mpSource)
    {
        return VOOSMP_ERR_Pointer;
    }

    return mpSource->GetSubtitleProperty(nIndex,ppProperty);
}

int CvoOnStreamMP::GetCurrTrackSelection(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
    if (NULL == mpSource)
    {
        return VOOSMP_ERR_Pointer;
    }

    return mpSource->GetCurrTrackSelection(pCurrIndex);
}


