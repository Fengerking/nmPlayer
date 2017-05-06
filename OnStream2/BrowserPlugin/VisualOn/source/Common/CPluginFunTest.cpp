#include "CPluginFunTest.h"

#define AUDIO_COUNT 3 // 2 is disable
#define DEFAULT_AUDIO_IDX 2
#define DISABLE_AUDIO_IDX (AUDIO_COUNT-2)

#define VIDEO_COUNT 3 // 1 is disable
#define DEFAULT_VIDEO_IDX 2
#define DISABLE_VIDEO_IDX (VIDEO_COUNT-2)

#define SUTITLE_COUNT 4 // 3 is disable
#define DEFAULT_SUTITLE_IDX 3
#define DISABLE_SUTITLE_IDX (SUTITLE_COUNT-2)

CPluginFunTest::CPluginFunTest(LPCTSTR szWorkingPath)
{
  m_hWnd = NULL;
  m_pDD = NULL;
  m_pDDSOffScr = NULL;
  m_pDDSPrimary = NULL;
  m_hdc = NULL;

  m_bExit = false;
  m_bStoped = true;
  m_hThread = NULL;

  m_pItemProperties = NULL;

  m_nDuration = 0;
  m_nPos = 0;
  curPlayingIndex.nCurrAudioIdx = DEFAULT_AUDIO_IDX;
  curPlayingIndex.nCurrSubtitleIdx = DEFAULT_SUTITLE_IDX;
  curPlayingIndex.nCurrVideoIdx = DEFAULT_VIDEO_IDX;
}

CPluginFunTest::~CPluginFunTest(void)
{
  Uninit();
}

int CPluginFunTest::Init()
{
  m_bExit = false;
  m_bStoped = true;

  m_nDuration = 0;

  m_srcTrackProperty.nPropertyCount = 1;
  m_pItemProperties = new VOOSMP_SRC_TRACK_ITEM_PROPERTY;
  strcpy(m_pItemProperties->szKey,"keyTest");
  m_pItemProperties->pszProperty = new char[MAX_PATH];
  strcpy(m_pItemProperties->pszProperty, "propertyTest");
  m_srcTrackProperty.ppItemProperties = &m_pItemProperties;

  return 0;
}

int CPluginFunTest::Uninit()
{
  if (NULL != m_pItemProperties)
  {
    if (m_pItemProperties->pszProperty)
    {
      delete []m_pItemProperties->pszProperty;
      m_pItemProperties->pszProperty = NULL;
    }

    delete m_pItemProperties;
    m_pItemProperties = NULL;
  }

  m_bExit = true;

  WaitForSingleObject(m_hThread, INFINITE);

  m_hThread = NULL;

  ReleaseDD();
  return 0;
}


int CPluginFunTest::SetView( void* pView)
{
  m_hWnd = (HWND)pView;
  return 0;
}


int CPluginFunTest::Open(void* pSource, int nFlag, int nSourceType)
{
  m_nDuration = 100000;
  m_nPos = 0;

  curPlayingIndex.nCurrAudioIdx = DEFAULT_AUDIO_IDX;
  curPlayingIndex.nCurrSubtitleIdx = DEFAULT_SUTITLE_IDX;
  curPlayingIndex.nCurrVideoIdx = DEFAULT_VIDEO_IDX;

  Run();
  return 0;
}


int CPluginFunTest::GetProgramCount(int* pProgramCount)
{
  OutputDebugString(_T("GetProgramCount"));
  return 0;
}


int CPluginFunTest::GetProgramInfo(int nProgramIndex, VOOSMP_SRC_PROGRAM_INFO** ppProgramInfo)
{
  OutputDebugString(_T("GetProgramInfo"));
  return 0;
}


int CPluginFunTest::GetCurTrackInfo(int nTrackType, VOOSMP_SRC_TRACK_INFO** ppTrackInfo )
{
  OutputDebugString(_T("GetCurTrackInfo"));
  return 0;
}


int CPluginFunTest::GetSample(int nTrackType, void* pSample)
{
  OutputDebugString(_T("GetSample"));

  return 0;
}


int CPluginFunTest::SelectProgram(int nProgram)
{
  OutputDebugString(_T("SelectProgram"));

  return 0;
}


int CPluginFunTest::SelectStream(int nStream)
{
  OutputDebugString(_T("SelectProgram"));

  return 0;
}


int CPluginFunTest::SelectTrack( int nTrack)
{
  OutputDebugString(_T("SelectProgram"));

  return 0;
}


int CPluginFunTest::SelectLanguage( int nIndex)
{
  OutputDebugString(_T("SelectProgram"));

  return 0;
}


int CPluginFunTest::GetLanguage(VOOSMP_SUBTITLE_LANGUAGE_INFO** ppLangInfo)
{
  OutputDebugString(_T("SelectProgram"));

  return 0;
}


int CPluginFunTest::Close()
{
  m_bStoped = true;

  ReleaseDD();
  return 0;
}


int CPluginFunTest::Run()
{
  m_bStoped = false;

  if (m_hdc==NULL)
  {
    CreateDD();
  }

  DWORD dwThreadID;
  if (m_hThread == NULL)
  {
    m_hThread = CreateThread(0, 0, TextOutTest, this, NULL, &dwThreadID);
  }

  return 0;
}


int CPluginFunTest::Pause()
{
  m_bStoped = true;
  return 0;
}


int CPluginFunTest::Stop()
{
  m_bStoped = true;

  ReleaseDD();
  return 0;
}


int CPluginFunTest::GetPos()
{
  OutputDebugString(_T("GetPos"));

  return m_nPos;
}


int CPluginFunTest::SetPos(int nPos)
{
  OutputDebugString(_T("SetPos"));

  m_nPos = nPos;

  return m_nPos;
}


int CPluginFunTest::GetDuration()
{
  OutputDebugString(_T("GetDuration"));

  return m_nDuration;
}


int CPluginFunTest::GetParam( int nParamID, void* pValue)
{
  OutputDebugString(_T("GetParam"));

  return 0;
}


int CPluginFunTest::SetParam(int nParamID, void* pValue)
{
  OutputDebugString(_T("SetParam"));

  return 0;
}



int CPluginFunTest::GetVideoCount()
{
  OutputDebugString(_T("GetVideoCount"));

  return VIDEO_COUNT;
}


int CPluginFunTest::GetAudioCount()
{
  OutputDebugString(_T("GetAudioCount"));

  return AUDIO_COUNT;
}


int CPluginFunTest::GetSubtitleCount()
{
  OutputDebugString(_T("GetSubtitleCount"));

  return SUTITLE_COUNT;
}


int CPluginFunTest::SelectVideo(int nIndex)
{
  OutputDebugString(_T("SelectVideo"));

  if (IsVideoAvailable(nIndex))
  {
    curPlayingIndex.nCurrVideoIdx = nIndex;
    return 0;
  }

  return -1;
}


int CPluginFunTest::SelectAudio(int nIndex)
{
  OutputDebugString(_T("SelectProgram"));

  if (IsAudioAvailable(nIndex))
  {
    curPlayingIndex.nCurrAudioIdx = nIndex;
    return 0;
  }

  return -1;
}


int CPluginFunTest::SelectSubtitle(int nIndex)
{
  OutputDebugString(_T("SelectSubtitle"));

  if (IsSubtitleAvailable(nIndex))
  {
    curPlayingIndex.nCurrSubtitleIdx = nIndex;
    return 0;
  }

  return -1;
}


bool CPluginFunTest::IsVideoAvailable(int nIndex)
{
  OutputDebugString(_T("IsVideoAvailable"));

  if (nIndex==DISABLE_VIDEO_IDX)
  {
    return 0;
  }

  if ((nIndex>=0&&nIndex<VIDEO_COUNT)  || nIndex==-1)
  {
    return 1;
  }

  return 0;
}


bool CPluginFunTest::IsAudioAvailable(int nIndex)
{
  OutputDebugString(_T("IsAudioAvailable"));

  if (nIndex==DISABLE_AUDIO_IDX)
  {
    return 0;
  }

  if (nIndex>=0&&nIndex<AUDIO_COUNT )
  {
    return 1;
  }

  return 0;
}


bool CPluginFunTest::IsSubtitleAvailable(int nIndex)
{
  OutputDebugString(_T("IsSubtitleAvailable"));

  if (nIndex==DISABLE_SUTITLE_IDX)
  {
    return 0;
  }

  if (nIndex>=0&&nIndex<SUTITLE_COUNT)
  {
    return 1;
  }

  return 0;
}


int CPluginFunTest::CommitSelection()
{
  OutputDebugString(_T("CommitSelection"));

  return 0;
}


int CPluginFunTest::ClearSelection()
{
  OutputDebugString(_T("ClearSelection"));

  curPlayingIndex.nCurrAudioIdx = DEFAULT_AUDIO_IDX;
  curPlayingIndex.nCurrSubtitleIdx = DEFAULT_SUTITLE_IDX;
  curPlayingIndex.nCurrVideoIdx = DEFAULT_VIDEO_IDX;

  return 0;
}


int CPluginFunTest::GetVideoProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
  OutputDebugString(_T("GetVideoProperty"));

  if((nIndex>=0&&nIndex<VIDEO_COUNT) || nIndex==-1)
    *ppProperty = &m_srcTrackProperty;

  return 0;
}


int CPluginFunTest::GetAudioProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
  OutputDebugString(_T("GetAudioProperty"));

  if(nIndex>=0&&nIndex<AUDIO_COUNT)
    *ppProperty = &m_srcTrackProperty;

  return 0;
}


int CPluginFunTest::GetSubtitleProperty(int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
  OutputDebugString(_T("GetSubtitleProperty"));

  if(nIndex>=0&&nIndex<SUTITLE_COUNT)
    *ppProperty = &m_srcTrackProperty;

  return 0;
}


int CPluginFunTest::GetCurrTrackSelection(VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
  OutputDebugString(_T("GetCurrTrackSelection"));

  if (pCurrIndex)
  {
    pCurrIndex->nCurrAudioIdx = curPlayingIndex.nCurrAudioIdx;
    pCurrIndex->nCurrSubtitleIdx = curPlayingIndex.nCurrSubtitleIdx;
    pCurrIndex->nCurrVideoIdx = curPlayingIndex.nCurrVideoIdx;
  }

  return 0;
}

 bool CPluginFunTest::CreateDD (void)
 {
   ReleaseDD ();

   if ( DirectDrawCreate( NULL, &m_pDD, NULL ) != DD_OK )
     return FALSE;

   if (m_pDD->SetCooperativeLevel(m_hWnd,DDSCL_NORMAL) != DD_OK)
     return FALSE;

   ZeroMemory(&m_ddsd, sizeof(m_ddsd));
   m_ddsd.dwSize = sizeof(m_ddsd);
   m_ddsd.dwFlags = DDSD_CAPS;
   m_ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
   if (m_pDD->CreateSurface(&m_ddsd, &m_pDDSPrimary, NULL) != DD_OK)
     return FALSE;

   LPDIRECTDRAWCLIPPER  pcClipper;   // Cliper
   if( m_pDD->CreateClipper( 0, &pcClipper, NULL ) != DD_OK )
     return FALSE;

   if( pcClipper->SetHWnd( 0, m_hWnd ) != DD_OK )
   {
     pcClipper->Release();
     return FALSE;
   }

   if( m_pDDSPrimary->SetClipper( pcClipper ) != DD_OK )
   {
     pcClipper->Release();
     return FALSE;
   }

   // Done with clipper
   pcClipper->Release();

   RECT rectWnd;
   GetWindowRect(m_hWnd,&rectWnd);

   // 创建YUV表面 
   ZeroMemory(&m_ddsd, sizeof(m_ddsd));
   m_ddsd.dwSize = sizeof(m_ddsd);
   m_ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
   m_ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
   m_ddsd.dwWidth = rectWnd.right-rectWnd.left;
   m_ddsd.dwHeight = rectWnd.bottom - rectWnd.top;
   m_ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
   m_ddsd.ddpfPixelFormat.dwFlags  = DDPF_FOURCC | DDPF_YUV ;
   m_ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('Y','V', '1', '2');
   m_ddsd.ddpfPixelFormat.dwYUVBitCount = 8;
   if (m_pDD->CreateSurface(&m_ddsd, &m_pDDSOffScr, NULL) != DD_OK)
     return FALSE;

   m_hdc = GetDC(m_hWnd);

   //if ( m_pDDSOffScr->GetDC(&m_hdc) != DD_OK)
   //  return FALSE;

   return TRUE;
 }

 bool CPluginFunTest::ReleaseDD(void)
 {
   if(m_pDD != NULL)
   {
     if(m_pDDSPrimary != NULL)
     {
       if (m_hdc)
       {
         m_pDDSPrimary->ReleaseDC(m_hdc);
       }

       m_pDDSPrimary->Release();
       m_pDDSPrimary = NULL;
     }
     if(m_pDDSOffScr != NULL)
     {
       m_pDDSOffScr->Release();
       m_pDDSOffScr = NULL;
     }
     m_pDD->Release();
     m_pDD = NULL;
   }

   return TRUE;
 }

 DWORD CPluginFunTest::TextOutTest(void * pm)
 {
   CPluginFunTest *pTest = (CPluginFunTest*)pm;

   if (pTest->m_hWnd==NULL)
   {
     MessageBox(NULL,_T("the m_hWnd is null!"),NULL,MB_OK);
     return -1;
   }

   if (pTest->m_hdc == NULL)
   {
       pTest->m_hdc = GetDC(pTest->m_hWnd);
   }
   if (pTest->m_hdc==NULL)
   {
     MessageBox(NULL,_T("the hdc is null!"),NULL,MB_OK);
     return -1;
   }

   while(!(pTest->m_bExit))
   {
     if (pTest->m_bStoped)
     {
       Sleep(1000);
       continue;
     }

     pTest->m_nPos += 1000;
     if (pTest->m_nPos>=pTest->m_nDuration)
     {
       pTest->m_bStoped = true;
       continue;
     }

     SetTextColor( pTest->m_hdc, RGB( 0, 255, 0 ) );

     RECT rectWnd;
     GetWindowRect(pTest->m_hWnd,&rectWnd);
     RECT rcClient;
     GetClientRect(pTest->m_hWnd, &rcClient);

     int nShowWidth = rectWnd.right-rectWnd.left;
     int nShowHeight = rectWnd.bottom-rectWnd.top;
     int x = rcClient.right - nShowWidth/2;
     int y = rcClient.bottom - nShowHeight/2;
     if (x<0 || y<0)
     {
       continue;
     }

     TCHAR szMsg[MAX_PATH] = _T("test ddraw for plugin!");
     BOOL bRet = TextOut( pTest->m_hdc, x, y, szMsg, lstrlen(szMsg));  

     _tcscpy(szMsg,_T(""));
     SYSTEMTIME systemTime;
     GetSystemTime(&systemTime);

     TCHAR szText[10] = _T("");
     _itot(systemTime.wYear,szText,10);
     _tcscat(szMsg, szText);
     _tcscat(szMsg,_T("-"));

     _itot(systemTime.wMonth,szText,10);
     _tcscat(szMsg, szText);
     _tcscat(szMsg,_T("-"));

     _itot(systemTime.wDay,szText,10);
     _tcscat(szMsg, szText);
     _tcscat(szMsg,_T("-"));

     _itot(systemTime.wHour,szText,10);
     _tcscat(szMsg, szText);
     _tcscat(szMsg,_T("-"));

     _itot(systemTime.wMinute,szText,10);
     _tcscat(szMsg, szText);
     _tcscat(szMsg,_T("-"));

     _itot(systemTime.wSecond,szText,10);
     _tcscat(szMsg, szText);

     BOOL bRet1 = TextOut( pTest->m_hdc, x, y+20, szMsg, lstrlen(szMsg));

     RECT					rctDest = rectWnd;		
     RECT					rctSour;	

     rctSour.left = 0;
     rctSour.top = 0;
     rctSour.right = nShowWidth;
     rctSour.bottom = nShowHeight;

 //    HRESULT ddRval = pTest->m_pDDSPrimary->Blt(&rctDest, pTest->m_pDDSOffScr, &rctSour, DDBLT_WAIT, NULL);

     Sleep(1000);
   }
 }

int	CPluginFunTest::HandleEvent (int nID, void * pParam1, void * pParam2)
{
  OutputDebugString(_T("HandleEvent"));

  return -1;
}