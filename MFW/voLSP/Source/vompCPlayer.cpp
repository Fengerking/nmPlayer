	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		vompCPlayer.cpp

	Contains:	vompCPlayer class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include "vompCPlayer.h"

#include "voOSFunc.h"
#include "voLog.h"
#include "CCTSLiveSource.h"

#include "voParser.h"
#include "vome2CAudioRender.h"
#include "vome2CVideoRender.h"

#ifdef _IOS
#include "CAudioUnitRender.h"
#include "CiOSVideoRender.h"
#include "CBaseVideoRender.h"
#endif

vompCPlayer::vompCPlayer(VO_PLAYER_OPEN_PARAM* pParam)
	: vompCEngine ()
	//, m_hWnd (hWnd)
	, m_bOpenSync (true)
	, m_nPlayMode (VOMP_PULL_MODE)
	, m_pLiveSource (NULL)
	, m_pAudioRender (NULL)
	, m_pVideoRender (NULL)
	, m_nColorType (VOMP_COLOR_RGB565_PACKED)
	, m_llLastVideoMediaTime (0)
	, m_nLastVideoSystemTime (0)
	, m_bStopPlay (true)
	, m_bSeeking (false)
	, m_bRenderOut (false)
	, m_bIsSourceModule (true)
	, m_bUseLiveSourceModule(true)
{
	memset (&m_sAudioFormat, 0, sizeof (m_sAudioFormat));
	memset (&m_sVideoFormat, 0, sizeof (m_sVideoFormat));
	memset (&m_rcDraw, 0, sizeof (m_rcDraw));
	
	memcpy(&m_openParam, pParam, sizeof(VO_PLAYER_OPEN_PARAM));
	
	memset(m_szDataSource, 0, sizeof(m_szDataSource));
}	

int vompCPlayer::init() {
	
	vompCEngine::init();
	
	m_sDataSource.pUserData = &m_opFile;
	m_sDataSource.ReadAt = vompReadAt;
	m_sDataSource.WriteAt = vompWriteAt;
	m_sDataSource.GetSize = vompGetSize;
	
	m_sLibFunc.pUserData = this;
	m_sLibFunc.LoadLib = mmLoadLib;
	m_sLibFunc.GetAddress = mmGetAddress;
	m_sLibFunc.FreeLib = mmFreeLib;
	
	SetParam (VOMP_PID_FUNC_LIB, &m_sLibFunc);
	
	m_sDrmCB.fCallback = vompSourceDrmCallBack;
	m_sDrmCB.pUserData = this;
	
	SetParam (VOMP_PID_FUNC_DRM, &m_sDrmCB);
	
	m_sReadBuffer.pUserData = &m_sSourceBuffer;
	m_sReadBuffer.ReadAudio = vompReadAudio;
	m_sReadBuffer.ReadVideo = vompReadVideo;	
	
#ifdef TARA
    CreateVideoRender();
#endif
    
	return 1;
}

vompCPlayer::~vompCPlayer ()
{
	if (m_pAudioRender != NULL)
	{
		m_pAudioRender->Stop ();
		delete m_pAudioRender;
		m_pAudioRender = NULL;
	}

	if (m_pVideoRender != NULL)
	{
		delete m_pVideoRender;
		m_pVideoRender = NULL;
	}

	if (m_pLiveSource != NULL)
	{
		delete m_pLiveSource;
		m_pLiveSource = NULL;
	}
}

int vompCPlayer::SendBuffer (VO_BUFFER_INPUT_TYPE eInputType, VOMP_BUFFERTYPE * pBuffer)
{	
	if (VO_BUFFER_INPUT_STREAM == eInputType) {
		return m_pLiveSource->SendBuffer(pBuffer);
	}
	
	voCAutoLock lock(&m_mutexSendBuf);

	if(m_bStopPlay)
	{
		VOLOGW("VOME is not ready.");
		return -1;
	}
				
	pBuffer->nSize = pBuffer->nSize & 0X7FFFFFFF;
	
	int nRet = -1;
	nRet = vompCEngine::SendBuffer(eInputType, pBuffer);
	
	if(pBuffer->nFlag & VOMP_FLAG_BUFFER_NEW_FORMAT)
		VOLOGI("VOMP_FLAG_BUFFER_NEW_FORMAT sent...........................");
	
// Dump buffer for testing	
#if 0
	if(VO_BUFFER_INPUT_AUDIO == eInputType)
	{
		static FILE* aduio_file = NULL;
		if(!aduio_file)
		{
			char szTmp[1024];
			voOS_GetAppFolder(szTmp, 1024);
			strcat(szTmp, "audio_dump.mp3");
			aduio_file = fopen(szTmp, "wb");
		}
		
		if(aduio_file)
		{
			fwrite(pBuffer->pBuffer, 1, pBuffer->nSize & 0X7FFFFFFF, aduio_file);
		}
	}
	else if (VO_BUFFER_INPUT_VIDEO == eInputType)
	{
		static FILE* video_file = NULL;
		if(!video_file)
		{
			char szTmp[1024];
			voOS_GetAppFolder(szTmp, 1024);
			strcat(szTmp, "video_dump.mpeg");
			video_file = fopen(szTmp, "wb");
		}
		
		if(video_file)
		{
			fwrite(pBuffer->pBuffer, 1, pBuffer->nSize & 0X7FFFFFFF, video_file);
		}		
	}
	else {}

#endif
	// End dump buffer
	
	return nRet;
}

int vompCPlayer::SetDataSource (void * pSource, int nFlag, int nMode)
{
	Stop ();

	if (nMode == 1)
	{
		m_nPlayMode = VOMP_PUSH_MODE;
		m_bRenderOut = false;
	}
	else if (nMode == 2)
	{
		m_nPlayMode = VOMP_PUSH_MODE;
		m_bRenderOut = true;
	}
	else if (nMode == 3)
	{
		m_nPlayMode = VOMP_PULL_MODE;
		m_bRenderOut = false;
	}
	else
	{
		m_nPlayMode = VOMP_PUSH_MODE;
		m_bRenderOut = false;
	}

	SetParam (VOMP_PID_AUDIO_PLAYMODE, &m_nPlayMode);
	SetParam (VOMP_PID_VIDEO_PLAYMODE, &m_nPlayMode);

//	int nOneTrack = 1;
//	SetParam (VOMP_PID_AUDIO_ONLY, &nOneTrack);
//	SetParam (VOMP_PID_VIDEO_ONLY, &nOneTrack);

	SetParam (VOMP_PID_DRAW_COLOR, &m_nColorType);
	SetParam (VOMP_PID_DRAW_RECT, &m_rcDraw);

	memset(m_szDataSource, 0, sizeof(m_szDataSource));
	
	if (pSource)
	{						
#ifdef _WIN32
		WideCharToMultiByte (CP_ACP, 0, (LPWSTR) pSource, -1, m_szDataSource, 1024, NULL, NULL);
#else
		strcpy(m_szDataSource , (const VO_CHAR * )pSource );
#endif 
	}
	
	if (m_pLiveSource != NULL) {
		m_pLiveSource->Stop ();
		delete m_pLiveSource;
		m_pLiveSource = NULL;
	}
	
	int nOpenFlag = m_bOpenSync ? VOMP_FLAG_SOURCE_SYNC : VOMP_FLAG_SOURCE_ASYNC;
	
	if (m_bIsSourceModule)
	{
		if (m_bUseLiveSourceModule) {
			m_pLiveSource = new CCTSLiveSource (VO_LIVESRC_FT_TS);
			m_pLiveSource->Open (this, NotifyStatus, SendData);
			nOpenFlag |= VOMP_FLAG_SOURCE_SENDBUFFER;
		}
		else {
			nOpenFlag |= VOMP_FLAG_SOURCE_SENDBUFFER_TS;
		}
		
		int nCodec   = VOMP_VIDEO_CodingMPEG2;//VOMP_VIDEO_CodingH264;//VOMP_VIDEO_CodingMPEG2;
		SetParam (VOMP_PID_VIDEO_TRACK, &nCodec);
		
		nCodec	= VOMP_AUDIO_CodingMP3;//VOMP_AUDIO_CodingAAC;//VOMP_AUDIO_CodingMP3;
		SetParam (VOMP_PID_AUDIO_TRACK, &nCodec);
		
		
		vompCEngine::SetDataSource (NULL, nOpenFlag, 0);
		
	}
	else {
		if (m_opFile.Open (m_szDataSource) < 0)
			return -1;
		
		nOpenFlag |= VOMP_FLAG_SOURCE_URL;
		vompCEngine::SetDataSource (m_szDataSource, nOpenFlag, 0);
		
//		if (m_bOpenSync)
//			Run ();
	}
	
	SetParam (VOMP_PID_DRAW_RECT, &m_rcDraw);
	
	return 0;
}

int vompCPlayer::Run (void)
{
	int nRC = vompCEngine::Run ();

	if (m_pLiveSource != NULL)
		m_pLiveSource->Start ();

	if (VOMP_PULL_MODE == m_nPlayMode) {
		CreateAudioRender();
		CreateVideoRender();
	}
	
	if (m_pVideoRender)
	{
        m_pVideoRender->Start();
		m_pVideoRender->SetDrawRect(m_rcDraw.left, m_rcDraw.top, m_rcDraw.right, m_rcDraw.bottom);
	}
	
	if (m_pAudioRender != NULL)
		m_pAudioRender->Start ();
	
	m_bStopPlay = false;
	
	return nRC;
}

int vompCPlayer::CreateAudioRender (void)
{
	if (m_pAudioRender != NULL)
		return 0;
	
	m_pAudioRender = new vome2CAudioRender(this);
	
	return 0;
}

int vompCPlayer::CreateVideoRender (void)
{
	if (m_pVideoRender != NULL)
		return 0;
	
	m_pVideoRender = new vome2CVideoRender(this);
	
	if (m_pVideoRender != NULL)
	{
		m_pVideoRender->SetColorType((VOMP_COLORTYPE)m_nColorType);
		if (m_openParam.hDrawWnd != NULL)
			m_pVideoRender->SetSurface (m_openParam.hDrawWnd);
	}
	
	return 0;
}

int vompCPlayer::Pause (void)
{
	int nRC = vompCEngine::Pause ();

	if (m_pLiveSource != NULL)
		m_pLiveSource->Pause ();

	if (m_pAudioRender != NULL)
		m_pAudioRender->Pause ();
	
	if (m_pVideoRender != NULL)
		m_pVideoRender->Pause ();
	
	return nRC;
}

int vompCPlayer::Stop (void)
{
	m_bStopPlay = true;

	if (m_pLiveSource != NULL)
		m_pLiveSource->Stop ();

	if (m_pAudioRender != NULL)
		m_pAudioRender->Stop ();
	
	if (m_pVideoRender != NULL)
		m_pVideoRender->Stop ();
	
	int nRC = vompCEngine::Stop ();

	return nRC;
}


int vompCPlayer::SetCurPos (int nCurPos)
{
//	if (m_bSeeking)
//		return VOMP_ERR_Status;

	m_bSeeking = true;

	return vompCEngine::SetCurPos (nCurPos);
}

int vompCPlayer::GetParam (int nID, void * pValue)
{
	if (VOMP_PID_VIDEO_RENDER_CONTEXT == nID) {
        if ((VOMP_PULL_MODE == m_nPlayMode) && (NULL != m_pVideoRender)) {
            return m_pVideoRender->GetParam(nID, pValue);
        }
    }
    
	return vompCEngine::GetParam(nID, pValue);
}

int vompCPlayer::SetParam (int nID, void * pValue)
{
	if (nID == VOMP_PID_DRAW_VIEW) {
		m_openParam.hDrawWnd = pValue;
        if ((VOMP_PULL_MODE == m_nPlayMode) && (NULL != m_pVideoRender)) {
			m_pVideoRender->SetSurface (m_openParam.hDrawWnd);
        }
	}
	else if (nID == VOMP_PID_DRAW_COLOR)
	{
		m_nColorType = *((int*)pValue);
        
        if (VOMP_PULL_MODE == m_nPlayMode) {
            int iPullColor = VOMP_COLOR_YUV_PLANAR420;
            vompCEngine::SetParam (VOMP_PID_DRAW_COLOR, &iPullColor);
            
            if (NULL != m_pVideoRender) {
                m_pVideoRender->SetColorType((VOMP_COLORTYPE)m_nColorType);
            }
            
            return VOMP_ERR_None;
        }
	}
	else if (nID == VOMP_PID_DRAW_RECT)
	{
        return SetDrawRect((VOMP_RECT*) pValue);
	}
    
    return vompCEngine::SetParam(nID, pValue);
}

int vompCPlayer::SendEvent (int nID, void * pParam1, void * pParam2)
{
	if(m_openParam.pUserData && m_openParam.pListener)
		/*return*/ m_openParam.pListener(m_openParam.pUserData, nID, pParam1, pParam2);
	
	return VOMP_ERR_None;
}

int	vompCPlayer::HandleEvent (int nID, void * pParam1, void * pParam2)
{
	if (nID == VOMP_CB_OpenSource)
	{
		// 20110506
		//Run ();
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_CB_PlayComplete)
	{
		VOLOGI ("VOMP_CB_PlayComplete");
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_CB_Error)
	{
		VOLOGE ("The error is %d", *(int *)pParam1);
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_CB_VideoStartBuff)
	{
		return SendEvent(nID, pParam1, pParam2);
	}
	else if (nID == VOMP_CB_VideoStopBuff)
	{
		return SendEvent(nID, pParam1, pParam2);
	}
	else if (nID == VOMP_CB_SourceBuffTime)
	{
		_stprintf (m_szStatus, _T("Buffer Time A% 8d        V% 8d"), *(int*)pParam1, *(int*)pParam2);
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_CB_BufferStatus)
	{
		_stprintf (m_szStatus, _T("Buffer status %d\n"), *(int*)pParam1);
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_CB_DownloadPos)
	{
		_stprintf (m_szStatus, _T("Download pos %d"), *(int*)pParam1);
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_CB_SeekComplete)
	{
		VOLOGI ("VOMP_CB_SeekComplete  222");
		m_bSeeking = false;

		return VOMP_ERR_None;
	}
	else if (nID == VOMP_CB_VideoDelayTime)
	{
//		VOLOGI ("VOMP_CB_VideoDelayTime  %d", *(int*)pParam1);
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_CB_VideoLastTime)
	{
//		VOLOGI ("VOMP_CB_VideoLastTime  %d", *(int*)pParam1);
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_CB_VideoDropFrames)
	{
//		VOLOGI ("VOMP_CB_VideoDropFrames  %d", *(int*)pParam1);
		return SendEvent(nID, pParam1, pParam2);
	}
	else if (nID == VOMP_CB_VideoFrameRate)
	{
//		VOLOGI ("VOMP_CB_VideoFrameRate  %d    %d", *(int*)pParam1, *(int*)pParam2);
		return SendEvent(nID, pParam1, pParam2);
	}
	else if (nID == VOMP_CB_RenderAudio)
	{
		SendEvent(nID, pParam1, pParam2);
		
		if (!m_bRenderOut)
			return VOMP_ERR_Implement;

/*it is sample code
		VOMP_AUDIO_FORMAT * pAudioFormat = (VOMP_AUDIO_FORMAT *)pParam2;
		VOMP_BUFFERTYPE * pAudioBuffer = (VOMP_BUFFERTYPE *)pParam1;
		static CBaseAudioRender *pAudioRender = NULL;
		
		if (pAudioRender == NULL)
		{
			memcpy (&m_sAudioFormat, pAudioFormat, sizeof (m_sAudioFormat));
#ifdef _WIN32
			pAudioRender = new CWaveOutRender (NULL, NULL);
#elif defined _IOS
			pAudioRender = new CAudioUnitRender (NULL, NULL);
#endif
			pAudioRender->SetFormat ((VO_AUDIO_FORMAT *)pAudioFormat);
			pAudioRender->Start ();
		}

		if (pAudioRender != NULL)
		{
			if (m_sAudioFormat.SampleRate != pAudioFormat->SampleRate || m_sAudioFormat.Channels != pAudioFormat->Channels)
			{
				memcpy (&m_sAudioFormat, pAudioFormat, sizeof (m_sAudioFormat));
				pAudioRender->SetFormat ((VO_AUDIO_FORMAT *)pAudioFormat);
			}

			pAudioRender->Render ((VO_PBYTE)pAudioBuffer->pBuffer, pAudioBuffer->nSize, pAudioBuffer->llTime, VO_TRUE);
			return VOMP_ERR_None;
		}*/

		return VOMP_ERR_Implement;
	}
	else if (nID == VOMP_CB_RenderVideo)
	{
		SendEvent(nID, pParam1, pParam2);
		
		if (!m_bRenderOut)
			return VOMP_ERR_Implement;

/*it is sample code, if you want to render in lsp with push mode, you need confirm the sequence of create video render 
		VOMP_VIDEO_BUFFER * pVideoBuffer = (VOMP_VIDEO_BUFFER *)pParam1;
		VOMP_VIDEO_FORMAT * pVideoFormat = (VOMP_VIDEO_FORMAT *)pParam2;
		static CBaseVideoRender *pVideoRender;
		
		if (pVideoRender == NULL)
		{
#ifdef _WIN32
			pVideoRender = new CGDIVideoRender (NULL, m_openParam.hDrawWnd, NULL);
#elif defined _IOS
			pVideoRender = new CiOSVideoRender (NULL, m_openParam.hDrawWnd, NULL);
#endif
			VO_RECT r;
			r.left=m_rcDraw.left;r.top=m_rcDraw.top;r.right=m_rcDraw.right;r.bottom=m_rcDraw.bottom;
			pVideoRender->SetDispRect  (m_openParam.hDrawWnd, &r, (VO_IV_COLORTYPE)m_nColorType);
			pVideoRender->SetVideoInfo (pVideoFormat->Width, pVideoFormat->Height, VO_COLOR_RGB565_PACKED);
			pVideoRender->SetParam (VO_PID_COMMON_LIBOP, (VO_LIB_OPERATOR *)NULL);
			
			memcpy (&m_sVideoFormat, pVideoFormat, sizeof (m_sVideoFormat));
		}

		if (pVideoRender != NULL)
		{
			if (m_sVideoFormat.Width != pVideoFormat->Width || m_sVideoFormat.Height != pVideoFormat->Height)
			{
				memcpy (&m_sVideoFormat, pVideoFormat, sizeof (m_sVideoFormat));
				pVideoRender->SetVideoInfo (pVideoFormat->Width, pVideoFormat->Height, VO_COLOR_RGB565_PACKED);
			}

			if (m_llLastVideoMediaTime == 0)
				m_llLastVideoMediaTime = pVideoBuffer->Time;

			VOLOGI ("Video Render %d	%d	%d", (int)pVideoBuffer->Time, voOS_GetSysTime () - m_nLastVideoSystemTime, 
										(int)(pVideoBuffer->Time - m_llLastVideoMediaTime));

			pVideoRender->Render ((VO_VIDEO_BUFFER *)pVideoBuffer, pVideoBuffer->Time, VO_TRUE);

			m_nLastVideoSystemTime = voOS_GetSysTime ();
			m_llLastVideoMediaTime = pVideoBuffer->Time;
			return VOMP_ERR_None;
		}*/
		
		return VOMP_ERR_Implement;
	}


	return VOMP_ERR_Implement;
}

void * vompCPlayer::mmLoadLib (void * pUserData, char * pLibName, int nFlag)
{
//	vompCPlayer * pGraph = (vompCPlayer *)pUserData;
//	VO_TCHAR* pWorkPath = pGraph->m_pWorkingPath;
	VO_PTR hDll = NULL;

#ifdef _WIN32
	VO_TCHAR	szDll[MAX_PATH];
	VO_TCHAR	szPath[MAX_PATH];
	TCHAR		szLibName[256];

	memset (szLibName, 0, 256);
	::MultiByteToWideChar (CP_ACP, 0, pLibName, -1, szLibName, sizeof (szLibName));

	if (hDll == NULL)
	{
		GetModuleFileName (NULL, szPath, sizeof (szPath));

		VO_TCHAR * pPos = vostrrchr (szPath, _T('\\'));
		if(pPos != NULL)
		{
			*(pPos + 1) = 0;

			vostrcpy (szDll, szPath);
			vostrcat (szDll, szLibName);
			hDll = LoadLibrary (szDll);
		}
	}

	if (hDll == NULL)
	{
		::GetModuleFileName (NULL, szPath, sizeof (szPath));
		VO_TCHAR * pPos = vostrrchr (szPath, _T('\\'));
		if(pPos != NULL)
		{
			*(pPos + 1) = 0;

			vostrcpy (szDll, szPath);
			vostrcat (szDll, szLibName);
			hDll = LoadLibrary (szDll);
		}
	}

	if (hDll == NULL)
	{
		vostrcpy (szDll, szPath);
		vostrcat (szDll, _T("Dll\\"));
		vostrcat (szDll, szLibName);
		hDll = LoadLibrary (szDll);
	}

	if (hDll == NULL)
	{
		vostrcpy (szDll, szLibName);
		hDll = LoadLibrary (szDll);
	}

	VOLOGI ("%s", szDll);

#elif defined _LINUX_X86
	VO_TCHAR libname[256];
	VO_TCHAR szPath[256];

	vostrcpy(libname, pLibName);
	voOS_GetAppFolder(szPath, sizeof(szPath));
	vostrcat(szPath, libname);
	hDll = dlopen (szPath, RTLD_FLAGS);
	if (hDll == NULL)
	{
		voOS_GetAppFolder(szPath, sizeof(szPath));
		vostrcat(szPath, "lib/");
		vostrcat(szPath, libname);
		hDll = dlopen (szPath, RTLD_FLAGS);
	}

#elif defined _LINUX_ANDROID
	VO_TCHAR libname[256];
	VO_TCHAR szLibPath[256];

	if (strncmp (pLibName, "lib", 3))
		vostrcpy (libname, pLibName);
	else
		vostrcpy(libname, pLibName + 3);

	FILE * hFile = fopen ("/data/local/voOMXPlayer/lib/debugvolib.txt", "rb");
	if (hFile != NULL)
	{
		fclose (hFile);

		vostrcpy(szLibPath, "/data/local/voOMXPlayer/lib/lib");
		vostrcat(szLibPath, libname);
		hDll = dlopen (szLibPath, RTLD_FLAGS);
		if (hDll == NULL)
		{
			vostrcpy(szLibPath, "/data/local/voOMXPlayer/lib/");
			vostrcat(szLibPath, libname);
			hDll = dlopen (szLibPath, RTLD_FLAGS);
		}
	}

	if (hDll == NULL)
	{
		if (pWorkPath != NULL)
		{
			vostrcpy(szLibPath, pWorkPath);
			vostrcat(szLibPath, "/lib/lib");
			vostrcat(szLibPath, libname);

			hDll = dlopen (szLibPath, RTLD_FLAGS);

			if (hDll == NULL)
			{
				vostrcpy(szLibPath, pWorkPath);
				vostrcat(szLibPath, "/lib/");
				vostrcat(szLibPath, libname);
				hDll = dlopen (szLibPath, RTLD_FLAGS);
			}

			if (hDll == NULL && strncmp (libname, "libvo", 5))
			{
				vostrcpy (szLibPath, libname);
				hDll = dlopen (szLibPath, RTLD_FLAGS);
			}
		}
		else
		{
			vostrcpy(szLibPath, "lib");
			vostrcat(szLibPath, libname);
			hDll = dlopen (szLibPath, RTLD_FLAGS);
			if (hDll == NULL)
			{
				vostrcpy (szLibPath, libname);
				hDll = dlopen (szLibPath, RTLD_FLAGS);
			}
		}
	}

	dlerror ();

	if (hDll != NULL)
	{
		VOLOGI ("Load %s ... %p", szLibPath, hDll);
	}
	else
	{
		VOLOGE ("Load %s failed!", pLibName);
	}
#endif // _WIN32

	return hDll;
}

void * vompCPlayer::mmGetAddress (void * pUserData, void * hLib, char * pFuncName, int nFlag)
{
	if (hLib == NULL)
		return NULL;

	VO_PTR pFunc = NULL;

#ifdef _LINUX
	pFunc = dlsym (hLib, pFuncName);
#endif // _LINUX

#ifdef _WIN32
#ifdef _WIN32_WCE
	TCHAR szFuncName[128];
	memset (szFuncName, 0, 256);
	::MultiByteToWideChar (CP_ACP, 0, pFuncName, -1, szFuncName, sizeof (szFuncName));

	pFunc = GetProcAddress ((HMODULE)hLib, szFuncName);
#else
	pFunc = GetProcAddress ((HMODULE)hLib, pFuncName);
#endif //_WIN32_WCE
#endif // _WIN32

	return pFunc;
}

int vompCPlayer::mmFreeLib (void * pUserData, void * hLib, int nFlag)
{
	if (hLib == NULL)
		return -1;

#ifdef _LINUX
	dlclose (hLib);
#endif // _LINUX

#ifdef _WIN32
	FreeLibrary ((HMODULE)hLib);
#endif // _WIN32

	return 0;
}

int vompCPlayer::vompSourceDrmCallBack (void * pUserData, int nFlag, void * pParam, int nReserved)
{
	return 0;
}

VO_VOID vompCPlayer::NotifyStatus (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{

}

VO_VOID vompCPlayer::SendData  (VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData)
{	
	int errType = VOMP_ERR_Unknown;
	vompCPlayer * ptr_player = ( vompCPlayer * )pUserData;
	
	VO_LIVESRC_SAMPLE * ptr_sample = ( VO_LIVESRC_SAMPLE * )pData;
	
	if (ptr_sample->nTrackID == VO_LIVESRC_OUTPUT_TRACKINFO)
	{
		//return 0;
		VO_PARSER_STREAMINFO* pStream = (VO_PARSER_STREAMINFO*)ptr_sample->Sample.Buffer;
		if(pStream->pVideoExtraData)
		{
			VO_VIDEO_FORMAT fmt;
			memset(&fmt, 0, sizeof(VO_VIDEO_FORMAT));
			fmt.Width	= pStream->VideoFormat.width;
			fmt.Height	= pStream->VideoFormat.height;
			ptr_player->SetParam(VOMP_PID_VIDEO_FORMAT, &fmt);
			//VOLOGI("tara ***** vompCPlayer SendData pVideoExtraData bRet:%d\n", bRet);
		}
		//VOLOGI("tara ***** vompCPlayer SendData VO_LIVESRC_OUTPUT_TRACKINFO end\n");
		return;
	}
	
	//voCAutoLock alock(&ptr_player->m_mutexSendBuf);
	
	VOMP_BUFFERTYPE buffer;
	memset( &buffer , 0 , sizeof( VOMP_BUFFERTYPE ) );
	buffer.llTime = ptr_sample->Sample.Time;
	buffer.nFlag = ptr_sample->Sample.Flag;
	buffer.nSize = ptr_sample->Sample.Size;
	buffer.pBuffer = ptr_sample->Sample.Buffer;
	
	if( buffer.nFlag & VOMP_FLAG_BUFFER_NEW_FORMAT )
	{
		VOLOGE( "New codec type: %d" , ptr_sample->nCodecType );
		buffer.pData = (void *)(ptr_sample->nCodecType);
	}
	
	int retrynums = 10;
	while ((errType != VOMP_ERR_None ) && (0 < retrynums))
	{
		if (VO_LIVESRC_OUTPUT_AUDIO == nOutputType) {
			errType = ptr_player->SendBuffer (VO_BUFFER_INPUT_AUDIO, &buffer);
		}
		else if(VO_LIVESRC_OUTPUT_VIDEO == nOutputType) {
			errType = ptr_player->SendBuffer (VO_BUFFER_INPUT_VIDEO, &buffer);
		}
		else {
			return;
		}

		if (errType != VOMP_ERR_None)
		{
			voOS_Sleep (500);
			retrynums--;
		}
	}
}

int vompCPlayer::SetDrawRect(VOMP_RECT* pRect)
{
	memcpy(&m_rcDraw, pRect, sizeof(VOMP_RECT));
    
    if (m_pVideoRender)
	{
		m_pVideoRender->SetDrawRect(m_rcDraw.left, m_rcDraw.top, m_rcDraw.right, m_rcDraw.bottom);
	}
    
	return vompCEngine::SetParam (VOMP_PID_DRAW_RECT, &m_rcDraw);;
}

int vompCPlayer::GetAudioBufferTime (void)
{
	int nAudioBufTime = 0;
	
	GetParam (VOMP_PID_AUDIO_BUFFTIME, &nAudioBufTime);
	
	return nAudioBufTime;
	
}

int vompCPlayer::GetVideoBufferTime (void)
{
	int nVideoBufTime = 0;
	
	GetParam (VOMP_PID_VIDEO_BUFFTIME, &nVideoBufTime);
	
	return nVideoBufTime;
	
}
