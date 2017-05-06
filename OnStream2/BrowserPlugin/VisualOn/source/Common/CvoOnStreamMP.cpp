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
#include "CvoOnStreamMP.h"
#include "voOSFunc.h"
#include "voLog.h"

CvoOnStreamMP::CvoOnStreamMP(LPCTSTR szWorkingPath)
: mpMPHandle(NULL)
, m_hDll(NULL)
, mpSource(NULL)
, m_bEnableSubtitle(FALSE)
, m_nIsLive(-1)
, m_nAudioTrackSelectedIdx(-1)
, m_nSubtitleTrackSelectedIdx(-1)
, m_bOpenedSucess(FALSE)
, m_bLiveStreamingDVRPosition(TRUE)
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
	Uninit();

	if(m_hDll != NULL)
	{
		mmFreeLib(NULL , m_hDll , 0);
		m_hDll = NULL;
	}
}

int CvoOnStreamMP::Init()
{
    voCAutoLock lock (&m_Mutex);

	if(mMPAPI.Init == NULL)
		return VOOSMP_ERR_Pointer;

	int nRC = mMPAPI.Init((void **)&mpMPHandle , VOOSMP_VOME2_PLAYER , &mInitParam , VOOSMP_FLAG_INIT_LIBOP|VOOSMP_FLAG_INIT_WORKING_PATH);

	SetParam(VOOSMP_PID_LISTENER , &mListener);
	m_nIsLive = -1;

	nRC = nRC | GetParam(VOOSMP_PID_GET_ONSTREAMSRC_HANDLE , &mpSource);
	return nRC;
}

int CvoOnStreamMP::Uninit()
{
    voCAutoLock lock (&m_Mutex);

	if(mpMPHandle == NULL || mMPAPI.Uninit == NULL)
		return VOOSMP_ERR_Pointer;

	mMPAPI.Uninit(mpMPHandle);
	mpMPHandle = NULL;

	return VOOSMP_ERR_None;
}

int CvoOnStreamMP::SetView( void* pView)
{
  voCAutoLock lock (&m_Mutex);

	if(mpMPHandle == NULL || mMPAPI.SetView == NULL)
		return VOOSMP_ERR_Pointer;

	return mMPAPI.SetView(mpMPHandle ,pView);
}

int CvoOnStreamMP::Open(void* pSource, int nFlag, int nSourceType)
{
  voCAutoLock lock (&m_Mutex);

	if(mpMPHandle == NULL || mMPAPI.Open == NULL)
		return VOOSMP_ERR_Pointer;

  m_bOpenedSucess = FALSE;
	int nRC = mMPAPI.Open(mpMPHandle , pSource , nFlag , nSourceType);

  if((nFlag & VO_OSMP_FLAG_SRC_OPEN_SYNC) != 0)
  {
    m_bOpenedSucess = TRUE;
  }

  return nRC;
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
  voCAutoLock lock (&m_Mutex);

	if(mpMPHandle == NULL || mMPAPI.SelectProgram == NULL)
		return VOOSMP_ERR_Pointer;

  if(FALSE == m_bOpenedSucess)
    return VOOSMP_ERR_Status;

	return mMPAPI.SelectProgram(mpMPHandle , nProgram);
}

int CvoOnStreamMP::SelectStream(int nStream)
{
  voCAutoLock lock (&m_Mutex);

	if(mpMPHandle == NULL || mMPAPI.SelectStream == NULL)
		return VOOSMP_ERR_Pointer;

  if(FALSE == m_bOpenedSucess)
    return VOOSMP_ERR_Status;

	return mMPAPI.SelectStream(mpMPHandle , nStream);
}

int CvoOnStreamMP::SelectTrack( int nTrack)
{
  voCAutoLock lock (&m_Mutex);

	if(mpMPHandle == NULL || mMPAPI.SelectTrack == NULL)
		return VOOSMP_ERR_Pointer;

  if(FALSE == m_bOpenedSucess)
    return VOOSMP_ERR_Status;

	return mMPAPI.SelectTrack(mpMPHandle , nTrack);
}

int CvoOnStreamMP::SelectLanguage( int nIndex)
{
  voCAutoLock lock (&m_Mutex);

	if(mpMPHandle == NULL || mMPAPI.SelectLanguage == NULL)
		return VOOSMP_ERR_Pointer;

  if(FALSE == m_bOpenedSucess)
    return VOOSMP_ERR_Status;

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
  voCAutoLock lock (&m_Mutex);

	if(mpMPHandle == NULL || mMPAPI.Close == NULL)
		return VOOSMP_ERR_Pointer;

	m_bOpenedSucess = FALSE;

	return mMPAPI.Close(mpMPHandle);
}

int CvoOnStreamMP::Run()
{
  voCAutoLock lock (&m_Mutex);

	if(mpMPHandle == NULL || mMPAPI.Run == NULL)
		return VOOSMP_ERR_Pointer; 

  if(FALSE == m_bOpenedSucess)
    return VOOSMP_ERR_Status;

  SetParam(VOOSMP_PID_COMMON_CCPARSER,&m_bEnableSubtitle);

	int nRC = mMPAPI.Run(mpMPHandle);

  if(nRC != VO_OSMP_ERR_NONE)
    m_bOpenedSucess = FALSE;

  return nRC;
}

int CvoOnStreamMP::Pause()
{
  voCAutoLock lock (&m_Mutex);

	if(mpMPHandle == NULL || mMPAPI.Pause == NULL)
		return VOOSMP_ERR_Pointer;

  if(FALSE == m_bOpenedSucess)
    return VOOSMP_ERR_Status;

	return mMPAPI.Pause(mpMPHandle);
}

int CvoOnStreamMP::Stop()
{
  voCAutoLock lock (&m_Mutex);

	if(mpMPHandle == NULL || mMPAPI.Stop == NULL)
		return VOOSMP_ERR_Pointer;

  m_bOpenedSucess = FALSE;

	return mMPAPI.Stop(mpMPHandle);
}

int CvoOnStreamMP::GetPos()
{
	if(mpMPHandle == NULL || mMPAPI.GetPos == NULL)
  {
    VOLOGE("point error!");
    return -1;	
  }

  if(FALSE == m_bOpenedSucess)
  {
  	VOLOGE("not open success!");
    return -1;
  }

  long pos = mMPAPI.GetPos(mpMPHandle);

  if(m_bLiveStreamingDVRPosition)
  {
    //do the same as ios: DX requirement
    if(IsLiveStreaming())
    {
      VOOSMP_SRC_DVRINFO voSrcDvrinfo;
      memset(&voSrcDvrinfo,0, sizeof(VOOSMP_SRC_DVRINFO));
      int nRC = GetParam(VOOSMP_SRC_PID_DVRINFO,&voSrcDvrinfo);
      if (VOOSMP_ERR_None != nRC) 
      {
        VOLOGE("getDVR Info Err:%d", nRC);
        return pos;
      }

      return pos-voSrcDvrinfo.uLiveTime;
    }
  }

  return pos;
}

int CvoOnStreamMP::SetPos(int nPos)
{
  voCAutoLock lock (&m_Mutex);

	if(mpMPHandle == NULL || mMPAPI.SetPos == NULL)
		return -1;

  if(FALSE == m_bOpenedSucess)
    return -1;

  if(m_bLiveStreamingDVRPosition)
  {
    //do the same as ios: DX requirement
    if(IsLiveStreaming())
    {
      VOOSMP_SRC_DVRINFO voSrcDvrinfo;
      memset(&voSrcDvrinfo,0, sizeof(VOOSMP_SRC_DVRINFO));
      int nRC = GetParam(VOOSMP_SRC_PID_DVRINFO,&voSrcDvrinfo);
      if (VOOSMP_ERR_None != nRC) 
      {
        VOLOGE("getDVR Info Err:%d", nRC);
        return -1;
      }

      long adjustPos = nPos + voSrcDvrinfo.uLiveTime;
      VOLOGI("[DVR]set pos %ld, pos %ld, max %lld", adjustPos, nPos, voSrcDvrinfo.uLiveTime);
      return mMPAPI.SetPos(mpMPHandle , adjustPos);
    }
  }

  VOLOGI("SetPos:  %d ", nPos);
  return mMPAPI.SetPos(mpMPHandle , nPos);
}

int CvoOnStreamMP::GetDuration()
{
	if(mpMPHandle == NULL || mMPAPI.GetDuration == NULL)
		return 0;

  if(FALSE == m_bOpenedSucess)
    return 0;

	long long llDuration = 0;

	mMPAPI.GetDuration(mpMPHandle , &llDuration);

	return (int)llDuration;
}

long CvoOnStreamMP::GetMinPosition()
{
  if(mpMPHandle == NULL || mMPAPI.GetParam == NULL)
    return 0;

  if(FALSE == m_bOpenedSucess)
    return 0;

  VOOSMP_SRC_DVRINFO voSrcDvrinfo;
  memset(&voSrcDvrinfo,0, sizeof(VOOSMP_SRC_DVRINFO));
  int nRC = GetParam(VOOSMP_SRC_PID_DVRINFO,&voSrcDvrinfo);
  if (VOOSMP_ERR_None != nRC) 
  {
    VOLOGE("getMinPosition Info Err:%d", nRC);
    return 0;
  }

  //do the same as ios: DX requirement
  if (m_bLiveStreamingDVRPosition)
  {
    if (IsLiveStreaming())
    {
      VOLOGI("[DVR]getMinPosition return %lld,  start %lld live %lld", voSrcDvrinfo.uStartTime - voSrcDvrinfo.uLiveTime, voSrcDvrinfo.uStartTime, voSrcDvrinfo.uLiveTime);
      return voSrcDvrinfo.uStartTime - voSrcDvrinfo.uLiveTime;
    }
  }

  return voSrcDvrinfo.uStartTime;
}

long CvoOnStreamMP::GetMaxPosition()
{
  if(mpMPHandle == NULL || mMPAPI.GetParam == NULL)
    return 0;

  if(FALSE == m_bOpenedSucess)
    return 0;

  VOOSMP_SRC_DVRINFO voSrcDvrinfo;
  memset(&voSrcDvrinfo,0, sizeof(VOOSMP_SRC_DVRINFO));
  int nRC = GetParam(VOOSMP_SRC_PID_DVRINFO,&voSrcDvrinfo);
  if (VOOSMP_ERR_None != nRC) 
  {
    VOLOGE("GetMaxPosition Info Err:%d", nRC);
    return 0;
  }

  if(m_bLiveStreamingDVRPosition)
  {
    if (IsLiveStreaming()) 
    {
      //do the same as ios: DX requirement
      return 0;
    }
  }
    
  return voSrcDvrinfo.uEndTime;
}

int CvoOnStreamMP::GetParam( int nParamID, void* pValue)
{
	if(mpMPHandle == NULL || mMPAPI.GetParam == NULL)
		return VOOSMP_ERR_Pointer;

  if (nParamID == VOOSMP_PID_COMMON_CCPARSER)
  {
    *((int*)pValue) = m_bEnableSubtitle;
    return VOOSMP_ERR_None;
  }

	return mMPAPI.GetParam(mpMPHandle , nParamID , pValue);
}

int CvoOnStreamMP::SetParam(int nParamID, void* pValue)
{
  voCAutoLock lock (&m_Mutex);

	if(mpMPHandle == NULL || mMPAPI.SetParam == NULL)
		return VOOSMP_ERR_Pointer;

  if (nParamID == VOOSMP_PID_COMMON_CCPARSER)
  {
    m_bEnableSubtitle = *((int*)pValue);

    if(FALSE == m_bOpenedSucess)
      return 0;
  }

  if (VOOSMP_PID_VIDEO_RENDER_TYPE == nParamID && NULL!=pValue)
  {
    VOLOGI("change render type to %d", *((int*)pValue));
  }

  VOLOGI("nParamID 0x%08x begin", nParamID);
	int iRet =  mMPAPI.SetParam(mpMPHandle , nParamID , pValue);
    VOLOGI("nParamID 0x%08x end", nParamID);

    return iRet;
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
  switch(nID)
  {
  case VOOSMP_SRC_CB_Open_Finished:
    {
      m_bOpenedSucess = TRUE;
    }
    return VOOSMP_ERR_None;
  default:
    break;
  }

  return VOOSMP_ERR_None;

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
		//::SetCursor (LoadCursor (NULL, IDC_WAIT));
		return VOOSMP_ERR_None;
	}
	else if (nID == VOOSMP_CB_VideoStopBuff)
	{
		//::SetCursor (LoadCursor (NULL, IDC_ARROW));
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
        return 0;
    }

    if(FALSE == m_bOpenedSucess)
      return 0;

    return mpSource->GetVideoCount();
}

int CvoOnStreamMP::GetAudioCount()
{
    if (NULL == mpSource)
    {
        return 0;
    }

    if(FALSE == m_bOpenedSucess)
      return 0;

    return mpSource->GetAudioCount();
}

int CvoOnStreamMP::GetSubtitleCount()
{
    if (NULL == mpSource)
    {
        return 0;
    }

    if(FALSE == m_bOpenedSucess)
      return 0;

    return mpSource->GetSubtitleCount();
}

int CvoOnStreamMP::SelectVideo(int nIndex)
{
  voCAutoLock lock (&m_Mutex);

    if (NULL == mpSource)
    {
        return VOOSMP_ERR_Pointer;
    }

    if(FALSE == m_bOpenedSucess)
      return VOOSMP_ERR_Status;

    return mpSource->SelectVideo(nIndex);
}

int CvoOnStreamMP::SelectAudio(int nIndex)
{
  voCAutoLock lock (&m_Mutex);

    if (NULL == mpSource)
    {
        return VOOSMP_ERR_Pointer;
    }

    if(FALSE == m_bOpenedSucess)
      return VOOSMP_ERR_Status;

	m_nAudioTrackSelectedIdx = nIndex;

    return mpSource->SelectAudio(nIndex);
}

int CvoOnStreamMP::SelectSubtitle(int nIndex)
{
  voCAutoLock lock (&m_Mutex);

    if (NULL == mpSource)
    {
        return VOOSMP_ERR_Pointer;
    }

    if(FALSE == m_bOpenedSucess)
      return VOOSMP_ERR_Status;

	m_nSubtitleTrackSelectedIdx = nIndex;

    return mpSource->SelectSubtitle(nIndex);
}

bool CvoOnStreamMP::IsVideoAvailable(int nIndex)
{
  if(FALSE == m_bOpenedSucess)
    return FALSE;

    if (NULL == mpSource)
    {
        return FALSE;
    }

    return mpSource->IsVideoAvailable(nIndex);
}

bool CvoOnStreamMP::IsAudioAvailable(int nIndex)
{
  if(FALSE == m_bOpenedSucess)
    return FALSE;

    if (NULL == mpSource)
    {
        return FALSE;
    }

    return mpSource->IsAudioAvailable(nIndex);
}

bool CvoOnStreamMP::IsSubtitleAvailable(int nIndex)
{
  if(FALSE == m_bOpenedSucess)
    return FALSE;

    if (NULL == mpSource)
    {
        return FALSE;
    }

    return mpSource->IsSubtitleAvailable(nIndex);
}

int CvoOnStreamMP::CommitSelection()
{
  voCAutoLock lock (&m_Mutex);

  if(FALSE == m_bOpenedSucess)
    return VOOSMP_ERR_Status;

    if (NULL == mpSource)
    {
        return VOOSMP_ERR_Pointer;
    }

	bool bNeedSeek = false;

	if(m_nAudioTrackSelectedIdx!=-1 || m_nSubtitleTrackSelectedIdx!=-1)
		bNeedSeek = true;

	m_nAudioTrackSelectedIdx	= -1;
	m_nSubtitleTrackSelectedIdx = -1;

    int nRet =  mpSource->CommitSelection();

	if(bNeedSeek)
	{
		SetPos(GetPos());
	}

	return nRet;
}

int CvoOnStreamMP::ClearSelection()
{
    voCAutoLock lock (&m_Mutex);

    if(FALSE == m_bOpenedSucess)
      return VOOSMP_ERR_Status;

    if (NULL == mpSource)
    {
        return VOOSMP_ERR_Pointer;
    }

	m_nAudioTrackSelectedIdx	= -1;
	m_nSubtitleTrackSelectedIdx = -1;

    return mpSource->ClearSelection();
}

int CvoOnStreamMP::GetVideoProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
  if(FALSE == m_bOpenedSucess)
    return VOOSMP_ERR_Status;

    if (NULL == mpSource)
    {
        return VOOSMP_ERR_Pointer;
    }

    return mpSource->GetVideoProperty(nIndex,ppProperty);
}

int CvoOnStreamMP::GetAudioProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
  if(FALSE == m_bOpenedSucess)
    return VOOSMP_ERR_Status;

    if (NULL == mpSource)
    {
        return VOOSMP_ERR_Pointer;
    }

    return mpSource->GetAudioProperty(nIndex,ppProperty);
}

int CvoOnStreamMP::GetSubtitleProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
  if(FALSE == m_bOpenedSucess)
    return VOOSMP_ERR_Status;

    if (NULL == mpSource)
    {
        return VOOSMP_ERR_Pointer;
    }

    return mpSource->GetSubtitleProperty(nIndex,ppProperty);
}

int CvoOnStreamMP::GetCurrPlayingTrackIndex(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
  if(FALSE == m_bOpenedSucess)
    return VOOSMP_ERR_Status;

	if (NULL == mpSource)
	{
		return VOOSMP_ERR_Pointer;
	}

	return mpSource->GetCurrPlayingTrackIndex(pCurrIndex);
}

int CvoOnStreamMP::GetCurrTrackSelection(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
  if(FALSE == m_bOpenedSucess)
    return VOOSMP_ERR_Status;

    if (NULL == mpSource)
    {
        return VOOSMP_ERR_Pointer;
    }

    return mpSource->GetCurrTrackSelection(pCurrIndex);
}


int CvoOnStreamMP::GetCurrSelectedTrackIndex(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
  if(FALSE == m_bOpenedSucess)
    return VOOSMP_ERR_Status;

	if (NULL == mpSource)
	{
		return VOOSMP_ERR_Pointer;
	}

	return mpSource->GetCurrSelectedTrackIndex(pCurrIndex);
}

int	CvoOnStreamMP::Redraw(BOOL bDraw)
{
  voCAutoLock lock (&m_Mutex);

  if(FALSE == m_bOpenedSucess)
    return VOOSMP_ERR_Status;

  VOLOGI("Redraw");

  int iTemp = 1;
  if (!bDraw)
	  iTemp = 0;
  int iRet = SetParam(VOOSMP_PID_VIDEO_REDRAW, &iTemp);
  return iRet;
}

int	CvoOnStreamMP::updateViewRegion(RECT& r) 
{
  voCAutoLock lock (&m_Mutex);

 // if(FALSE == m_bOpenedSucess)//must do not check this, rect maybe set rect before open success
 //   return VOOSMP_ERR_Status;

  VOLOGI("[view rect] l = %d, t = %d, r = %d, b = %d", r.left, r.top, r.right, r.bottom);

  return SetParam(VOOSMP_PID_DRAW_RECT, (void*)&r);
}

bool CvoOnStreamMP::IsLiveStreaming()
{
  if (-1 == m_nIsLive) 
  {
    VOOSMP_SRC_PROGRAM_INFO* pProgramInfo = NULL;
    int nRet = GetProgramInfo(0, &pProgramInfo);
    if ((VOOSMP_ERR_None != nRet) || (NULL == pProgramInfo))
    {
      return false;
    }

    if (VOOSMP_SRC_PROGRAM_TYPE_LIVE == pProgramInfo->nProgramType) 
    {
      m_nIsLive = 1;
    }
    else if (VOOSMP_SRC_PROGRAM_TYPE_VOD == pProgramInfo->nProgramType) 
    {
      m_nIsLive = 0;
    }
    else 
    {
      m_nIsLive = -1;

      return false;
    }
  }

  return m_nIsLive==1;
}

int CvoOnStreamMP::SetHTTPHeader(CHAR* headerName, CHAR* headerValue)
{
  if (headerName==NULL || headerValue==NULL ||  mMPAPI.SetParam==NULL)
  {
    return VO_OSMP_ERR_POINTER;
  }

  VOOSMP_SRC_HTTP_HEADER head;
  memset(&head, 0, sizeof(VOOSMP_SRC_HTTP_HEADER));

  head.pszHeaderName  = (unsigned char *)headerName;
  head.pszHeaderValue = (unsigned char *)headerValue;

  //there is lock in SetParam function, but this maybe call by a thread, so if directly use setParam, maybe locked
  return mMPAPI.SetParam(mpMPHandle , VOOSMP_SRC_PID_HTTP_HEADER , &head);
}

int CvoOnStreamMP::EnableLiveStreamingDVRPosition(BOOL bEnable)
{
  m_bLiveStreamingDVRPosition = bEnable;
  return 0;
}

int CvoOnStreamMP::GetVideoDecodingBitrate(char* pbuf) {

	if (pbuf == NULL)
		return VO_OSMP_ERR_POINTER;

	int nbuf[10];
	memset(nbuf, 0, sizeof(int) * 10);
	int ret = GetParam(VOOSMP_PID_VIDEO_DEC_BITRATE, nbuf);
	if (ret != VOOSMP_ERR_None)
		return ret;

	sprintf(pbuf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", nbuf[0], nbuf[1], nbuf[2], nbuf[3], nbuf[4], nbuf[5], nbuf[6], nbuf[7], nbuf[8], nbuf[9]);
	return VOOSMP_ERR_None;
}

int CvoOnStreamMP::GetAudioDecodingBitrate(char* pbuf) {

	if (pbuf == NULL)
		return VO_OSMP_ERR_POINTER;

	int nbuf[10];
	memset(nbuf, 0, sizeof(int) * 10);
	int ret = GetParam(VOOSMP_PID_AUDIO_DEC_BITRATE, nbuf);
	if (ret != VOOSMP_ERR_None)
		return ret;

	sprintf(pbuf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", nbuf[0], nbuf[1], nbuf[2], nbuf[3], nbuf[4], nbuf[5], nbuf[6], nbuf[7], nbuf[8], nbuf[9]);
	return VOOSMP_ERR_None;
}

BOOL CvoOnStreamMP::IsWin8OS()
{
    OSVERSIONINFOEX os; 
    os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX); 
    if(GetVersionEx((OSVERSIONINFO *)&os))
    {
        if ((6 < os.dwMajorVersion)|| (6 <= os.dwMajorVersion && 2<=os.dwMinorVersion))
        {
            VOLOGI("Win8 OS");
            return TRUE;
        }
    }

    return FALSE;
}
