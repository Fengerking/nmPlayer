	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOMEPlayer.cpp

	Contains:	CVOMEPlayer class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-11-21		JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "CVOMEPlayer.h"
#include "voOMXOSFun.h"
#include "OMX_DolbyExt.h"
#include "voDivXDRM.h"
#include "voCOMXBaseConfig.h"

#undef LOG_TAG
#define LOG_TAG "CVOMEPlayer"
#include "voLog.h"

typedef OMX_ERRORTYPE (* VOOMXGetEngineAPI) (VOOMX_ENGINEAPI * pEngine, OMX_U32 uFlag);
typedef OMX_ERRORTYPE (* VOOMXGetComponentAPI) (VOOMX_COMPONENTAPI * pComponent, OMX_U32 uFlag);
typedef VO_S32 (VO_API *pvoGetPreHTTPAPI)(VO_PREHTTP_API * pHandle, VO_U32 uFlag);

int CVOMEPlayer::g_voAndroidStreamSource = 0;

CVOMEPlayer::CVOMEPlayer(void)
	: m_nStatus (-1)
	, m_nLoop (0)
	, m_bUninit (false)
	, m_pDM3 (NULL)
	, m_nVideoWidth (0)
	, m_nVideoHeight (0)
	, m_hPlay (NULL)
	, m_nFile (-1)
	, m_nOffset (0)
	, m_nLength (0)
	, m_nSourceType (0)
	, m_bSeeking (false)
	, m_bSeekable (true)
	, m_nStartPos (0)
	, m_nDuration (-1)
	, m_bPrepareAsync (false)
	, m_bAutoStart (false)
	, m_bStreamSourceCount (false)
	, m_bSourceLoading (false)
	, m_pFilePipe (NULL)
	, m_bNotifyVideoSize (false)
	, m_pCallBack (NULL)
	, m_pUserData (NULL)
	, m_hMsgThread (NULL)
	, m_nMsgThreadID (0)
	, m_bMsgThreadStop (OMX_FALSE)
	, m_nMsgType (0)
	, m_bLoading (false)
	, m_bNotifyError (false)
	, mpDRMAPI(NULL)
	, m_pDrmEngine (NULL)
	, m_bDrmEngineCommit (false)
	, m_nDivXDRMMode(0)
	, m_iRentalCountInfo(0)
	, m_nAudioCoding (OMX_AUDIO_CodingAAC)
	, m_nVideoCoding (OMX_VIDEO_CodingAVC)	
	, m_nSuspendStatus (-1)
	, m_pHeaders (NULL)
	, m_bAudioSinkToStart (true)
	, m_bBuffering (false)	
{
	strcpy (m_szFile, "");
	memset (&m_VideoBuff, 0, sizeof (VOA_VIDEO_BUFFERTYPE));
	memset (&m_dspArea, 0, sizeof (OMX_VO_DISPLAYAREATYPE));
	memset (&m_fmtAudio, 0, sizeof (VOA_AUDIO_FORMAT));

	m_drmCallback.pUserData = this;
	m_drmCallback.fCallback = vomeSourceDrmProc;
}

CVOMEPlayer::~CVOMEPlayer()
{
	Uninit ();

	if (m_pHeaders)
	{
		delete [] m_pHeaders;
		m_pHeaders = NULL;
	}
}

int CVOMEPlayer::SetCallBack (VOACallBack pCallBack, void * pUserData)
{
	m_pCallBack = pCallBack;
	m_pUserData = pUserData;
	return 0;
}

int CVOMEPlayer::SetFilePipe (void * pFilePipe)
{
	m_pFilePipe = (OMX_VO_FILE_OPERATOR *)pFilePipe;
	return 0;
}

int CVOMEPlayer::SetSurface (void * pSurface)
{
	if(m_dspArea.hView != pSurface)
	{
		VOLOGI ("surface is %08X", (unsigned int)pSurface);
		m_dspArea.hView = pSurface;
	}

	return 0;
}

int CVOMEPlayer::Init (void)
{
	voCOMXAutoLock lock (&m_mutStatus);

	m_bUninit = false;

	LoadModule ();
	if (m_hDll == NULL)
	{
		VOLOGE ("LoadModule Fail");
		return -1;
	}

	m_fAPI.Init (&m_hPlay, 0, vomeCallBack, this);
	if (m_hPlay == NULL)
	{
		VOLOGE ("Init Player Fail");
		return -1;
	}

	char szWorkPath[128];
	strcpy (szWorkPath, "/data/data/com.mobitv.vome/");
//	m_fAPI.SetParam (m_hPlay, VOME_PID_WorkingPath, szWorkPath);

	m_dspArea.nWidth = 480;
	m_dspArea.nHeight = 320;
//	m_fAPI.SetDisplayArea (m_hPlay, &m_dspArea);

	m_bMsgThreadStop = OMX_FALSE;
	voOMXThreadCreate (&m_hMsgThread, &m_nMsgThreadID, (voOMX_ThreadProc)vomeMessageHandleProc, this, 0);

	return 0;
}

int CVOMEPlayer::SetSource (const char * pSource, const char * pHeaders)
{
	voCOMXAutoLock lock (&m_mutStatus);

	VOLOGI ("Source: %s", pSource);

	if (m_bLoading)
		return -1;
	if (pSource == NULL)
		return -1;

	int nLen = strlen(pSource);
	if(nLen == 0 || nLen >= 1024)
		return -1;

	if(pHeaders)
	{
		if(m_pHeaders)
			delete [] m_pHeaders;

		nLen = strlen(pHeaders) + 1;
		m_pHeaders = new char[nLen];
		if(m_pHeaders == NULL)
			return -1;

		strcpy(m_pHeaders, pHeaders);
		VOLOGI("%s", m_pHeaders);
	}

	if (m_pHeaders && strstr(m_pHeaders, "x-htc-mimetype: application/sdp\r\n"))
		m_nSourceType = 1;
	else if (!strncmp (pSource, "rtsp://", 7))
		m_nSourceType = 1;
	else if (!strncmp (pSource, "mms://", 6))
		m_nSourceType = 1;
	else if (!strncmp (pSource, "http://", 7))
	{
		char* p = strstr((char*)pSource, ".sdp");
		if(p && ((p + 4 - pSource) == (int)strlen(pSource) || p[4] == '?'))
			m_nSourceType = 1;
		else
			m_nSourceType = 2;
	}
	else
		m_nSourceType = 0;

	if(m_nSourceType == 1)
	{
		char szCompCfg[128];
		strcpy (szCompCfg, "vomeComp_RTSP.cfg");
		m_fAPI.SetParam (m_hPlay, VOME_PID_CompConfigFile, szCompCfg);
	}

	strcpy (m_szFile, pSource);

	m_nOffset = 0;
	m_nLength = 0;

	m_nStatus = -1;
	m_bUninit = false;

	/*
	if (m_nSourceType == 1)
	{
		g_voAndroidStreamSource++;
		m_bStreamSourceCount = true;

		VOLOGI ("Count: %d", g_voAndroidStreamSource);

		if (g_voAndroidStreamSource > 1)
			return -1;
	}
	*/
	return 0;
}

int CVOMEPlayer::SetSource (int fd, int64_t offset, int64_t length)
{
	voCOMXAutoLock lock (&m_mutStatus);
//	VOLOGI ("File ID %d, %d, %d", fd, (int)offset, (int)length);

	if (m_bLoading)
		return -1;

#ifdef LINUX
	char szLink[1024];
	sprintf(szLink, "/proc/self/fd/%d", fd);
	memset(m_szFile, 0, sizeof(m_szFile));
	readlink(szLink, m_szFile, sizeof(m_szFile));
	m_nFile = dup(fd);
	if (m_nFile <= 0)
	{
		VOLOGE ("dup error. ID %d,  %s", errno, strerror(errno));
		m_nFile = fd;
	}
#endif // LINUX

	m_nOffset = offset;
	m_nLength = length;
	m_nSourceType = 0;

	m_nStatus = -1;
	m_bUninit = false;

	return 0;
}

OMX_U32 CVOMEPlayer::vomeMessageHandleProc (OMX_PTR pParam)
{
	CVOMEPlayer * pPlayer = (CVOMEPlayer *)pParam;

	return pPlayer->MessageHandleProc ();
}

OMX_U32 CVOMEPlayer::MessageHandleProc (void)
{
	while(!m_bMsgThreadStop)
	{
		{
			voCOMXAutoLock lMsg (&m_mutMsg);

			if(m_nMsgType == 1)
				vomeLoadSource();
			else if(m_nMsgType == 2)
				SetPosB(m_nStartPos);
			
			if(m_nMsgType > 0)
			{
				m_nMsgType = 0;
				continue;
			}
		}

		voOMXOS_Sleep(2);
	}

	voOMXThreadClose (m_hMsgThread, 0);
	m_hMsgThread = NULL;

	return 0;
}

OMX_U32 CVOMEPlayer::vomeLoadSource (void)
{
	int nRC = BuildGraph ();

	voCOMXAutoLock lock (&m_mutNotify);
	if (m_nSourceType == 0)
	{
		NotifyOpenSource ();
	}
	else
	{
		if (m_bUninit)
			return 0;
		
		// commited by Eric, 2011.09.28, fix htc issue 8261, should not run when prepare, some times need to seek before run
		// add back by Ferry,for htc`s request of playback order
		if (nRC >= 0)
		{
			Run (true);
			m_bAutoStart = true;
		}
		else
		{
			NotifyOpenSource ();
		}
	}

	m_bLoading = false;

	return 0;
}

int CVOMEPlayer::SetLoop (int nLoop)
{
	voCOMXAutoLock lock (&m_mutStatus);

	if (m_pDM3 != NULL)
	{
		int nEnable = 1;
		if (nLoop > 0)
		{
			m_pDM3->SetConfig (m_pDM3, (OMX_INDEXTYPE)OMX_IndexConfigDAK_DEFAULT, &nEnable);
			VOLOGI ("Enable DM3");
		}
		else
		{
			nEnable = 0;
			m_pDM3->SetConfig (m_pDM3, (OMX_INDEXTYPE)OMX_IndexConfigDAK_DEFAULT, &nEnable);
			VOLOGI ("Disable DM3");
		}
	}
	else
	{
		m_nLoop = nLoop;
	}

	return 0;
}

int CVOMEPlayer::Prepare (void)
{
	voCOMXAutoLock lock (&m_mutStatus);

	m_bNotifyError = false;

	int nRC = BuildGraph ();

	if (nRC >= 0)
		return 0;
	else
		return -1;
}

int CVOMEPlayer::PrepareAsync (void)
{
	voCOMXAutoLock lock (&m_mutStatus);

	m_bNotifyError = false;

	if (m_bLoading)
		return -1;

	m_bPrepareAsync = true;
	m_bLoading = true;

	voCOMXAutoLock lMsg (&m_mutMsg);
	m_nMsgType = 1;

	return 0;
}

void CVOMEPlayer::NotifyOpenSource (void)
{
	VOLOGI ("BuildGraph result %d", m_nStatus);

	if (!m_bUninit)
	{
		if (m_nStatus < 0)
#ifdef __VOPRJ_HTCTUBE__
			vomeNotifyEvent(MEDIA_ERROR, 400, -1);
#else	//__VOPRJ_HTCTUBE__
			vomeNotifyEvent(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, -1);
#endif	//__VOPRJ_HTCTUBE__
		else
		{
			if(m_bNotifyError)
				vomeNotifyEvent(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, -1);
			else
				vomeNotifyEvent(MEDIA_PREPARED);
		}
	}

	m_bPrepareAsync = false;
}

VO_U32 CVOMEPlayer::CheckHTTPType(VO_LIVESRC_FORMATTYPE *type)
{
	CDllLoad dlEngine;
	vostrcpy( dlEngine.m_szDllFile, _T("voPreHTTP.so") );
	vostrcpy( dlEngine.m_szAPIName, _T("voGetPreHTTPAPI") );

	if ( dlEngine.LoadLib(NULL) == 0 ) {
		VOLOGI("!LoadLib");
		return VO_ERR_BASE;
	}

	VO_PREHTTP_API apiPreHttp = {0};
	void * handlePreHttp = NULL;

	pvoGetPreHTTPAPI getapi = (pvoGetPreHTTPAPI)dlEngine.m_pAPIEntry;
	if (getapi)
	{
		getapi( &apiPreHttp , 0 );
	}
	else
	{
		VOLOGI("!getapi");
		return VO_ERR_BASE;
	}

	if ( VO_ERR_NONE != apiPreHttp.voPreHTTPInit(&handlePreHttp) ) {
		VOLOGI("!voPreHTTPInit");
		return VO_ERR_BASE;
	}

	do {
		*type = VO_LIVESRC_FT_MAX;

		if ( VO_ERR_NONE != apiPreHttp.voPreHTTPSetSrcURL(handlePreHttp, m_szFile) ) {
			VOLOGI("!voPreHTTPSetSrcURL");
			return VO_ERR_BASE;
		}

		VO_CHAR *pURL = NULL;
		int iRet = 0;
		do {
			iRet = apiPreHttp.voPreHTTPGetDesInfo(handlePreHttp, type, &pURL);
		} while (!m_bUninit && (OMX_U32)iRet == 0xfffffffe);

		if (*type == VO_LIVESRC_FT_FILE)
		{
			memset(m_szFile, 0, 1024);
			sscanf(pURL, "%s\r\n", m_szFile);
		}
	} while (!m_bUninit && *type == VO_LIVESRC_FT_FILE);

	apiPreHttp.voPreHTTPUninit(handlePreHttp);

	VOLOGI("szFile %s", m_szFile);

	return VO_ERR_NONE;
}

int CVOMEPlayer::BuildGraph (void)
{
	VO_LIVESRC_FORMATTYPE pretype = VO_LIVESRC_FT_MAX;
	
	CheckHTTPType(&pretype);

	VOLOGI("pretype %x", pretype);

	if (pretype == VO_LIVESRC_FT_RTSP)
	{
		char szCompCfg[128];
		strcpy (szCompCfg, "vomeComp_RTSP.cfg");
		m_fAPI.SetParam (m_hPlay, VOME_PID_CompConfigFile, szCompCfg);

		g_voAndroidStreamSource++;
		m_bStreamSourceCount = true;

		VOLOGI ("Count: %d", g_voAndroidStreamSource);

		if (g_voAndroidStreamSource > 1)
			return -1;
	}
	else if(VO_LIVESRC_FT_HTTP_LS == pretype)
	{
		char szCompCfg[128];
		strcpy (szCompCfg, "vomeComp_LS.cfg");
		m_fAPI.SetParam (m_hPlay, VOME_PID_CompConfigFile, szCompCfg);

		strcpy (szCompCfg, "vomeCore_LS.cfg");
		m_fAPI.SetParam (m_hPlay, VOME_PID_CoreConfigFile, szCompCfg);
	}
	//---------------------

	OMX_U32 nRC = OMX_ErrorNone;
	{
		voCOMXAutoLock lockBuildGraph (&m_mutBuildGraph);

		if (m_nStatus >= 0)
			return 0;

		if (m_hPlay == NULL)
			return -1;

		if (m_nFile == -1 && strlen (m_szFile) <= 0)
			return -1;

		OMX_VO_CHECKRENDERBUFFERTYPE	audioRender;
		audioRender.pUserData = this;
		audioRender.pCallBack = vomeAudioRenderBufferProc;
		m_fAPI.SetParam (m_hPlay, VOME_PID_AudioRenderBuffer, &audioRender);

		OMX_VO_CHECKRENDERBUFFERTYPE	videoRender;
		videoRender.pUserData = this;
		videoRender.pCallBack = vomeVideoRenderBufferProc;
		m_fAPI.SetParam (m_hPlay, VOME_PID_VideoRenderBuffer, &videoRender);

		m_bSourceLoading = true;
		VOLOGI ("start to build graph!");

		VOME_SOURCECONTENTTYPE Source;
		memset (&Source, 0, sizeof (VOME_SOURCECONTENTTYPE));
		Source.nSize = sizeof (VOME_SOURCECONTENTTYPE);
		if (m_nFile > 0)
		{
			Source.pSource = (OMX_PTR)m_nFile;
			Source.nType = VOME_SOURCE_ID;
		}
		else if(pretype == VO_LIVESRC_FT_HTTP_LS)
		{
			Source.pSource = (OMX_PTR)m_szFile;
			Source.nType = VOME_SOURCE_HLS;
		}
		else if(pretype == VO_LIVESRC_FT_IIS_SS)
		{
			Source.pSource = (OMX_PTR)m_szFile;
			Source.nType = VOME_SOURCE_ISS;
		}
		else
		{
			Source.pSource = (OMX_PTR)m_szFile;
			Source.nType = VOME_SOURCE_File;
			Source.pHeaders = m_pHeaders;
			Source.nReserved = pretype;
			VOLOGI("%s", (char*)Source.pSource);
		}

		Source.nOffset = m_nOffset;
		Source.nLength = m_nLength;
		Source.pPipe = m_pFilePipe;
#if defined _ENABLE_DRM || defined __VOPRJ_HTCTUBE__
		Source.pDrmCB = &m_drmCallback;
#else
		Source.pDrmCB = NULL;
#endif
		Source.nFlag = 3; // Audio and Video

		nRC = m_fAPI.Playback (m_hPlay, &Source);

		if (m_iRentalCountInfo)
			vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_VO_DRM_RENTAL, m_iRentalCountInfo);

		if (nRC != OMX_ErrorNone)
		{
			VOLOGE ("m_fAPI.Playback error \n");
			m_bSourceLoading = false;
			return -1;
		}
	}

	voCOMXAutoLock lock (&m_mutStatus);
	if (m_bUninit)
		return -1;

	if (m_hPlay == NULL)
		return -1;

	VOME_VIDEOSIZETYPE videoSize;
	nRC = m_fAPI.GetParam (m_hPlay, VOME_PID_VideoSize, &videoSize);
	if (nRC == OMX_ErrorNone)
	{
		m_nVideoWidth = videoSize.nWidth;
		m_nVideoHeight = videoSize.nHeight;
		VOLOGI ("Video: W %d, H %d \n", m_nVideoWidth, m_nVideoHeight);

#ifdef __VOPRJ_HTCTUBE__
		if (m_nVideoHeight > 720)
		{
			vomeNotifyEvent(MEDIA_ERROR, 300);
			return -1;
		}
#endif	//__VOPRJ_HTCTUBE__

		//turn on for fixing eclair(2.1) version can not displaying well
		vomeNotifyEvent(MEDIA_SET_VIDEO_SIZE, m_nVideoWidth, m_nVideoHeight);

		//m_dspArea.hView = NULL;
		m_dspArea.nWidth = m_nVideoWidth;
		m_dspArea.nHeight = m_nVideoHeight;
		m_fAPI.SetDisplayArea (m_hPlay, &m_dspArea);

		m_bNotifyVideoSize = false;
	}

	int nRotation = 0;
	nRC = m_fAPI.GetParam (m_hPlay, VOME_PID_VideoRotation, &nRotation);
	if (nRC == OMX_ErrorNone)
	{
		VOLOGI ("Source Video Rotation %d", nRotation);
		if(m_pCallBack)
			m_pCallBack(m_pUserData, VOAP_IDC_setVideoRotation, (void*)nRotation, NULL);
		vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_VO_MEDIA_VIDEO_ROTATION, nRotation);
	}

	/*
     * For DivxDRM Mobile profile1.1, any CGMSA/Macrovision/DHCP file, if it's resolution is
	 * greater than 320x240, the playback should be denied.
     */
	if (m_pDrmEngine && m_pDrmEngine->GetDrmType() == VO_DRMTYPE_DIVX) {
		nRC = m_pDrmEngine->CheckCopyRightResolution(videoSize.nWidth , videoSize.nHeight);
		if(OMX_VO_ERR_DRM_OK != nRC)
		{
			VOLOGE ("m_pDrmEngine->CheckCopyRightResolution fail: 0x%08X", (unsigned int)nRC);
			vomeNotifyEvent(MEDIA_ERROR, MEDIA_ERROR_VO_DRM_NOAUTH_RESLUTION);
			return nRC;
		}
	}

	VOME_AUDIOFORMATTYPE audioFormat;
	nRC = m_fAPI.GetParam (m_hPlay, VOME_PID_AudioFormat, &audioFormat);
	if (nRC == OMX_ErrorNone)
	{
		VOLOGI ("Audio: S %d, C %d \n",(int)audioFormat.nSampleRate, (int)audioFormat.nChannels);

		if (m_pCallBack != NULL)
		{
			m_fmtAudio.nSampleRate = audioFormat.nSampleRate;
			m_fmtAudio.nChannels = audioFormat.nChannels;
			m_fmtAudio.nSampleBits = audioFormat.nSampleBits;
			m_pCallBack (m_pUserData, VOAP_IDC_setAudioFormat, &m_fmtAudio, NULL);
		}
	}
	else
		vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_VO_AUDIO_UNSUPPORTED);	
		

	m_nDuration = 0;
 	m_fAPI.GetDuration (m_hPlay, (OMX_S32 *)&m_nDuration);

// commented by Eric, 2011.09.28, fix htc issue 8261, should not send SEEK_COMPLETE if not seeking
//	if (m_nSourceType > 0)
//		m_bSeeking = true;

	m_nStatus = 0;
	m_bSourceLoading = false;

	m_fComp.GetComponentByName (m_hPlay, (char*)"OMX.VisualOn.Audio.DoblyEffect.XXX", &m_pDM3);
	if (m_pDM3 != NULL)
	{
		int nEnable = 1;
		m_pDM3->SetConfig (m_pDM3, (OMX_INDEXTYPE)OMX_IndexConfigDAK_DEFAULT, &nEnable);
	}

	return 0;
}

int CVOMEPlayer::Run (bool bForceSeek /* = false */)
{
 	voCOMXAutoLock lock (&m_mutStatus);

	m_nSuspendStatus = -1;

	if (m_hPlay == NULL)
       	return -1;

	/*
		Before playing the rental DivxDRM, a dailog will popup to user for choosing playback or not. 
    	If choosing YES to play:
			Call "m_pDrmEngine->Commit()" to reduce the view count.
		If choosing No to exit:
			the "m_pDrmEngine->Commit()" should not be called, and don't change the view count.

    	In the previous version, the "m_pDrmEngine->Commit()" was called in "Prepare()" stage. 
		So, whatever the user choosed, the "m_pDrmEngine->Commit()" was always called and the view count was reduced.

		To fix this issue, we put "m_pDrmEngine->Commit()" here , and call it only when Start() is excuted.		
	*/

	if(m_bAfterEOSSetPos)
	{
		SetPosB(0);
		m_bAfterEOSSetPos = false;
	}

	if (m_pDrmEngine && m_pDrmEngine->GetDrmType() == VO_DRMTYPE_DIVX && !m_bDrmEngineCommit) 
	{
		VO_U32 result = m_pDrmEngine->Commit();
		if(OMX_VO_ERR_DRM_OK != result)
		{
			VOLOGE ("m_pDrmEngine->Commit fail: 0x%08X", (unsigned int)result);
			vomeNotifyEvent(MEDIA_ERROR, MEDIA_ERROR_VO_DRM_NOAUTH);
			return result;
		}
		m_bDrmEngineCommit = true;
	}

	if (m_nStartPos > 0 && bForceSeek == false)
	{
		if (m_nSourceType == 0)
		{
			m_fAPI.SetCurPos (m_hPlay, m_nStartPos);
			m_bSeeking  = false;
			vomeNotifyEvent(MEDIA_SEEK_COMPLETE);
		}
	}

    if (m_fAPI.Run (m_hPlay)== OMX_ErrorNone)
    {
		if (m_nStartPos > 0 && bForceSeek == false)
		{
			if (m_nSourceType > 0)
			{
				m_fAPI.SetCurPos (m_hPlay, m_nStartPos);
				m_nStartPos = 0;
			}
		}
    	m_nStatus = 1;
    	return 0;
    }
	if (m_nSourceType == 2)
		return 0;
	else
		return -1;
}

int CVOMEPlayer::Pause (void)
{
 	voCOMXAutoLock lock (&m_mutStatus);

    if (m_hPlay == NULL)
       	return -1;

    m_fAPI.Pause (m_hPlay);

	if (m_pCallBack != NULL)
		m_pCallBack (m_pUserData, VOAP_IDC_setAudioPause, NULL, NULL);
	m_bAudioSinkToStart = true;
	
	m_nStatus = 2;
//	m_nStartPos = 0;

	return 0;
}

int CVOMEPlayer::Stop (void)
{
  	voCOMXAutoLock lock (&m_mutStatus);

	if (m_hPlay == NULL)
       	return -1;

	if (m_nStatus < 1)
		return 0;

    m_fAPI.Stop (m_hPlay);

	// david added 2011-04-01 for moto solana
//	voOMXOS_Sleep(400);	
	if (m_pCallBack != NULL) {
		m_pCallBack (m_pUserData, VOAP_IDC_setAudioStop, NULL, NULL);
		// david added 2011-04-01 for moto solana
//		m_pCallBack (m_pUserData, VOAP_IDC_setAudioFlush, NULL, NULL);
	}
	m_bAudioSinkToStart = true;
	m_nStatus = 3;
	m_nStartPos = 0;

	return 0;
}

bool CVOMEPlayer::IsPlaying (void)
{
	voCOMXAutoLock lock (&m_mutStatus);

	// we should return correct value when both suspending and playing
	// East 2010/10/12
	if (m_nSuspendStatus == 1 || m_nStatus == 1)
		return true;

    return false;
}

int CVOMEPlayer::SetPosB (int nPos)
{
	voCOMXAutoLock lockBuildGraph (&m_mutBuildGraph);

	m_bAfterEOSSetPos = false;

	if (m_hPlay == NULL || m_nStatus < 0)
	{
		VOLOGW ("m_hPlay 0x%08X, m_nStatus %d", (unsigned int)m_hPlay, m_nStatus);
		m_bSeeking = true;
		return 0;
	}

	if (m_bSeeking)
	{
		VOLOGW ("m_bSeeking is true");
		m_bSeeking = false;
		vomeNotifyEvent(MEDIA_SEEK_COMPLETE);
	}

	m_fAPI.SetCurPos (m_hPlay, nPos);
	VOLOGI ("source type %d SetCurPos %d done", m_nSourceType, nPos);

	if (m_pCallBack != NULL)
	{
		m_pCallBack (m_pUserData, VOAP_IDC_setAudioPause, NULL, NULL);
		m_pCallBack (m_pUserData, VOAP_IDC_setAudioFlush, NULL, NULL);
	}
	m_bAudioSinkToStart = true;

	m_nStartPos = 0;
	if(m_nSourceType == 0 || (m_nSourceType == 2 && m_nStatus == 2))	// local file or PD in pause state
		vomeNotifyEvent(MEDIA_SEEK_COMPLETE);
	else	// streaming wait for buffer stop event
		m_bSeeking = true;

	return 0;
}

int CVOMEPlayer::SetPos (int nPos)
{
	voCOMXAutoLock lock (&m_mutStatus);

	if (m_hPlay == NULL || m_nStatus < 0)
	{
		VOLOGW ("m_hPlay 0x%08X, m_nStatus %d", (unsigned int)m_hPlay, m_nStatus);
		m_nStartPos = nPos;
		return 0;
	}

	//Modified by Ma Tao at 2011.4.11
	//if it is live streaming , the duration is 0 , we ignore seek.
	if (!m_bSeekable || m_nDuration <= 0)
	{
		VOLOGW ("m_bSeekable is false");
		vomeNotifyEvent(MEDIA_SEEK_COMPLETE);
		return 0;
	}

	if (nPos < 0)
		nPos = 0;
	else if (nPos > m_nDuration - 1500)
		nPos = m_nDuration - 1500;

	m_nStartPos = nPos;

	voCOMXAutoLock lMsg (&m_mutMsg);

	VOLOGI ("SetPos %d wait for processing", m_nStartPos);
	m_nMsgType = 2;

	return 0;
}

int CVOMEPlayer::GetPos (int * pPos)
{
	if(m_bPrepareAsync)
	{
		*pPos = 0;
		return 0;
	}

	voCOMXAutoLock lock (&m_mutStatus);

	if (m_nSuspendStatus != -1)
	{
		*pPos = m_nStartPos;
		return 0;
	}

    if (m_hPlay == NULL)
	{
		*pPos = 0;
    	return -1;
	}

	if (m_nStartPos > 0)
	{
		if (m_nStatus != 1 && m_nStatus != 2)
		{
			*pPos = m_nStartPos;
			return 0;
		}
	}

    m_fAPI.GetCurPos (m_hPlay, (OMX_S32 *)pPos);

	if (*pPos == 0 && m_nSourceType > 0 && m_bPrepareAsync)
		*pPos = 100;

	return 0;
}

int CVOMEPlayer::GetDuration (int * pPos)
{
	voCOMXAutoLock lock (&m_mutStatus);

	int nTryTimes = 0;
	while (m_bSourceLoading && nTryTimes < 100)
	{
		voOMXOS_Sleep (2);
		nTryTimes++;
	}

    if (m_hPlay == NULL)
    	return -1;

//	*pPos = m_nDuration;
 	m_fAPI.GetDuration (m_hPlay, (OMX_S32 *) pPos);

	return 0;
}

int CVOMEPlayer::Suspend (void)
{
	//FIXME: Now, we don't support Suspend for online stream
	if (m_nSourceType >= 1)
		return -1;

	int nStartPos = 0;
	GetPos (&nStartPos);

	if (m_nSuspendStatus == -1)
		m_nSuspendStatus = m_nStatus;

	Uninit (true);

	m_nStartPos = nStartPos;

	return 0;
}

int CVOMEPlayer::Resume (void)
{
	//FIXME: Now, we don't support Suspend for online stream
	if (m_nSourceType >= 1) 
		return -1;

	VOLOGI ("m_nSuspendStatus %d, m_nStartPos %d", m_nSuspendStatus, m_nStartPos);

	if (m_nSuspendStatus == -1)
		return -1;

	int nStartPos = m_nStartPos;
	m_nStartPos = 0;

	Init ();

	if (BuildGraph () < 0)
	{
		m_nSuspendStatus = -1;

		return -1;
	}

	m_nStartPos = nStartPos;

	if (m_nSuspendStatus != 2)
		Run ();

	return 0;
}

int CVOMEPlayer::ChangeSurface (VOAUpdateSurfaceCallBack fCallBack, void * pUserData)
{
	voCOMXAutoLock lock (&m_mutStatus);

	if(NULL == m_hPlay)
	{
		fCallBack(pUserData);
		return 0;
	}

	VOME_UPDATESURFACECALLBACK sUpdateSurface;
	sUpdateSurface.fCallBack = (VOMEUpdateSurfaceCallBack)fCallBack;
	sUpdateSurface.pUserData = pUserData;
	m_fAPI.SetParam(m_hPlay, VOME_PID_ChangeSurface, &sUpdateSurface);

	return 0;
}

int CVOMEPlayer::SendAudioData (VOA_DATA_BUFFERTYPE * pData, int nTime)
{
	return 0;
}

int CVOMEPlayer::SendVideoData (VOA_DATA_BUFFERTYPE * pData, int nTime)
{
	return 0;
}

#ifdef _LINUX_ANDROID
int CVOMEPlayer::Invoke (android::Parcel * request, android::Parcel *reply)
{
	return 0;
}
int CVOMEPlayer::GetSeekAble ()
{
    if (m_hPlay == NULL)
    	return 0;

	return m_nDuration > 0 ? 1 : 0;
}
#endif // _LINUX_ANDROID

int CVOMEPlayer::Reset (void)
{
	m_nLoop = 0;
	m_nStatus = -1;
	m_nSuspendStatus = -1;

	return 0;
}

int CVOMEPlayer::Uninit (bool bSuspend /* = false */)
{
	voCOMXAutoLock lock (&m_mutStatus);

	if (m_hPlay == NULL)
		return 0;

	m_bUninit = true;

	if (m_nStatus == 1 || m_nStatus == 2)
		Stop ();

	//force stop when source in loading state
	m_fAPI.Close (m_hPlay);

	m_bMsgThreadStop = OMX_TRUE;

	OMX_U32 nTryNum = 0;
	while (m_hMsgThread != NULL && nTryNum < 1000)
	{
		voOMXOS_Sleep (2);
		nTryNum++;
	}

	if (m_hMsgThread != NULL)
	{
		VOLOGE ("Exit the message process thread failed!");
	}

	{
		voCOMXAutoLock lockBuildGraph (&m_mutBuildGraph);
		m_fAPI.Uninit (m_hPlay);
		m_hPlay = NULL;
	}

	if (m_pDrmEngine != NULL)
	{
		delete m_pDrmEngine;
		m_pDrmEngine = NULL;
	}

	if (m_nSourceType == 1)
	{
		if (m_bStreamSourceCount)
		{
			g_voAndroidStreamSource--;
			m_bStreamSourceCount = false;
		}
		VOLOGI ("Stream Source Count: %d", g_voAndroidStreamSource);
	}

	m_nStatus = -1;

	if (!bSuspend && m_nFile != -1)
	{
#ifdef LINUX
		close (m_nFile);
#endif	//LINUX
		m_nFile = -1;
	}

	return 0;
}

int CVOMEPlayer::CallBackFunc (int nID, void * pParam1, void * pParam2)
{
	if (m_pCallBack == NULL)
		return 0;

	// native window callback should be valid after reset called because we need it to call FreeBuffer
	// it use my whole day to find out native buffer leak cause crash issue, East 2011/03/19
	if (nID == VOME_CID_PLAY_NATIVEWINDOWCOMMAND)
	{
		VOME_NATIVEWINDOWCOMMAND * pCommand = (VOME_NATIVEWINDOWCOMMAND *)pParam1;
		if(VOME_NATIVEWINDOW_IsUsable == pCommand->nCommand)
		{
			*((OMX_BOOL *)pCommand->pParam1) = (NULL == m_dspArea.hView) ? OMX_FALSE : OMX_TRUE;
			return 0;
		}

		m_pCallBack (m_pUserData, VOAP_IDC_NativeWindowCmd, pParam1, NULL);
		return 0;
	}

	if (m_bUninit)
	{
		VOLOGW ("Callback 0x%08X happen after reset called", nID);	// add warning
		return 0;
	}

	if (nID == VOME_CID_PLAY_DEBUGTEXT)
	{
		m_pCallBack (m_pUserData, VOAP_IDC_displayText, pParam1, pParam2);
		return 0;
	}
	else if (nID == VOME_CID_PLAY_FINISHED)
	{
		if (m_nLoop > 0)
			SetPosB (0);
		else
		{
//			Stop (); 
			//No Stop Call, but status need to set 3 for stop status.
			m_nStatus = 3;
			m_bAfterEOSSetPos = true;
			vomeNotifyEvent(MEDIA_PLAYBACK_COMPLETE);
		}

		return 0;
	}
	else if (nID == VOME_CID_PLAY_ERROR)
	{
        VOLOGI ("VOME Unknpwn Error!");
		m_bNotifyError = true;
		vomeNotifyEvent(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, -100);
		return 0;
	}

	if (nID == VOME_CID_PLAY_BUFFERSTART)
	{
		m_bBuffering = true;
		if (m_pCallBack != NULL)
			m_pCallBack (m_pUserData, VOAP_IDC_setAudioPause, NULL, NULL);
#ifdef __VOPRJ_HTCTUBE__
		vomeNotifyEvent(MEDIA_BUFFERING_UPDATE, VOME_CID_PLAY_BUFFERSTART);
#else   
		vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_BUFFERING_START);
#endif	//__VOPRJ_HTCTUBE__
	}
	else if (nID == VOME_CID_PLAY_BUFFERING)
	{
		if (m_nSourceType == 1)
#ifdef __VOPRJ_HTCTUBE__
			vomeNotifyEvent(MEDIA_BUFFERING_UPDATE, VOME_CID_PLAY_BUFFERING, *((int*)pParam1));
#else	//__VOPRJ_HTCTUBE__
			vomeNotifyEvent(MEDIA_BUFFERING_UPDATE, *((int*)pParam1));
#endif	//__VOPRJ_HTCTUBE__
	}
	else if (nID == VOME_CID_PLAY_BUFFERSTOP)
	{
		voCOMXAutoLock lock (&m_mutNotify);

		if (m_bAutoStart)
		{
			Pause();
			m_bAutoStart = false;
		}

		if (m_bPrepareAsync && m_bLoading == false)
		{
			VOLOGI ("VOME_CID_PLAY_BUFFERSTOP NotifyOpenSource");
			NotifyOpenSource ();
		}

		
#ifdef __VOPRJ_HTCTUBE__
		vomeNotifyEvent(MEDIA_BUFFERING_UPDATE, VOME_CID_PLAY_BUFFERSTART, 100);
#else  
		vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_BUFFERING_END);
#endif	//__VOPRJ_HTCTUBE__
		if(m_bSeeking)
		{
			m_bSeeking = false;
			vomeNotifyEvent(MEDIA_SEEK_COMPLETE);
		}
		if(m_bBuffering)
			m_bAudioSinkToStart = true;
		m_bBuffering = false;
	}
	else if (nID == VOME_CID_PLAY_DOWNLOADPOS)
		vomeNotifyEvent(MEDIA_BUFFERING_UPDATE, *((int*)pParam1));
	else if (nID == VOME_CID_PLAY_PACKETLOST)
		vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_VO_STREAMING_PACKETLOST);
	else if (nID == VOME_CID_PLAY_CODECPASSABLEERR)
		vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_VO_CODEC_PASSABLE_ERROR, (int)pParam1);
	else if (nID == VOME_CID_PLAY_MEDIACONTENTTYPE)
		vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_VO_MEDIA_CONTENT_TYPE, (int)pParam1);
	else if (nID == VOME_CID_PLAY_MEDIAFOURCC)
	{
		if((OMX_U32)pParam1 == 0)
			vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_VO_MEDIA_AUDIO_FOURCC, (int)pParam2);
		else if((OMX_U32)pParam1 == 1)
			vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_VO_MEDIA_VIDEO_FOURCC, (int)pParam2);
	}
	else if (nID == VOME_CID_PLAY_UNSUPPORTEDFORMAT)
		vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_VO_FORMAT_UNSUPPORTED);
	else if (nID == VOME_CID_PLAY_UNSUPPORTEDCODEC)
		vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_VO_CODEC_UNSUPPORTED);
	else if (nID == VOME_CID_PLAY_UNSUPPORTEDAUDIO)
		vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_VO_AUDIO_UNSUPPORTED);
	else if (nID == VOME_CID_PLAY_UNSUPPORTEDVIDEO)
		vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_VO_VIDEO_UNSUPPORTED);
	else if (nID == VOME_CID_PLAY_FRAMEDROPPED)
		vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_VO_SOURCE_FRAMEDROPPED);
	else if (nID == VOME_CID_PLAY_SERVERGOODBYE)
		vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_VO_STREAMING_SERVERGOODBYE);
	else if (nID == VOME_CID_PLAY_NOTSEEKABLE)
	{
		m_bSeekable = false;
		vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_NOT_SEEKABLE);
	}
	else if (nID == VOME_CID_PLAY_STREAMERROREVENT)
		vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_VO_STREAMING_ERROREVENT, (int)pParam1);
	else if (nID == VOME_CID_PLAY_STREAMBITRATETHROUGHPUT)
	{
		VOLOGI ("Streaming bitrate %d(kbps), throughput %d(kBps)", (int)pParam1, (int)pParam2);

		vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_VO_STREAMING_BITRATE, (int)pParam1);
		vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_VO_STREAMING_THROUGHPUT, (int)pParam2);
	}
	else if (nID == VOME_CID_PLAY_STREAMRTCPRR)
		vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_VO_STREAMING_RTCPRR, (int)pParam1);
	else if (nID == VOME_CID_PLAY_3DVIDEOTYPE)
		Set3DVideoType(pParam1);


	return 0;
}

int CVOMEPlayer::RenderVideo (OMX_VO_VIDEOBUFFERTYPE * pVideoBuffer)
{
	if(m_bSeeking)
	{
		m_bSeeking = false;
		vomeNotifyEvent(MEDIA_SEEK_COMPLETE);
	}

	m_VideoBuff.virBuffer[0] = pVideoBuffer->Buffer[0];
	m_VideoBuff.virBuffer[1] = pVideoBuffer->Buffer[1];
	m_VideoBuff.virBuffer[2] = pVideoBuffer->Buffer[2];

	m_VideoBuff.nStride[0] = pVideoBuffer->Stride[0];
	m_VideoBuff.nStride[1] = pVideoBuffer->Stride[1];
	m_VideoBuff.nStride[2] = pVideoBuffer->Stride[2];

	if (pVideoBuffer->Color == OMX_COLOR_Format_QcomHW ||  pVideoBuffer->Color == OMX_COLOR_Format_QcomHW2 || 
		pVideoBuffer->Color == OMX_COLOR_FormatYUV420SemiPlanar || pVideoBuffer->Color == OMX_COLOR_Format_TI_DUCATI1
		|| OMX_COLOR_Format_SamSuangFK == pVideoBuffer->Color)
		m_VideoBuff.nColorType = VOA_COLOR_NV12;
	else if (pVideoBuffer->Color == OMX_COLOR_Format16bitRGB565)
		m_VideoBuff.nColorType = VOA_COLOR_RGB565;
	else if (pVideoBuffer->Color == OMX_COLOR_Format_NvidiaHW)
		m_VideoBuff.nColorType = VOA_COLOR_NVIDIA;
	else if (pVideoBuffer->Color == OMX_COLOR_Format_STEHW)
		m_VideoBuff.nColorType = (VOA_COLORTYPE)(VOA_COLOR_TYPE_MAX -1);
	else
		m_VideoBuff.nColorType = VOA_COLOR_YUV420;

	if (!m_bNotifyVideoSize ||(OMX_U32) m_nVideoWidth != pVideoBuffer->Width || (OMX_U32)m_nVideoHeight != pVideoBuffer->Height)
	{
		// VOLOGI ("Video Color %d W %d, H %d, Disp W %d, H %d\n", pVideoBuffer->Color, pVideoBuffer->Width, pVideoBuffer->Height, m_dspArea.nWidth, m_dspArea.nHeight);

		m_nVideoWidth = pVideoBuffer->Width;
		m_nVideoHeight = pVideoBuffer->Height;

		vomeNotifyEvent(MEDIA_SET_VIDEO_SIZE, m_nVideoWidth, m_nVideoHeight);
		if (m_pCallBack != NULL)
			m_pCallBack (m_pUserData, VOAP_IDC_setVideoSize, &m_nVideoWidth, &m_nVideoHeight);
	}

	if (m_pCallBack == NULL)
	{
		m_bNotifyVideoSize = true;
		return OMX_ErrorNone;
	}

	int nRC = 0;
	if(m_dspArea.hView)
		nRC = m_pCallBack (m_pUserData, VOAP_IDC_renderVideo, &m_VideoBuff, NULL);

	if (nRC >= 0)
	{
		m_bNotifyVideoSize = true;
		return OMX_ErrorNone;
	}
	else
	{
		return OMX_ErrorNotImplemented;
	}
}

int CVOMEPlayer::Set3DVideoType(void* pParam)
{
	int n3DVideoType = *(int*)pParam;

	if(m_pCallBack)
		m_pCallBack (m_pUserData, VOAP_IDC_set3DVideoType, &n3DVideoType, NULL);

	return 0;
}

int CVOMEPlayer::RenderAudio (OMX_AUDIO_PARAM_PCMMODETYPE * pAudioFormat, OMX_BUFFERHEADERTYPE * pBuffer)
{
	if(m_bSeeking)
	{
		m_bSeeking = false;
		vomeNotifyEvent(MEDIA_SEEK_COMPLETE);
	}

	if (m_pCallBack == NULL)
		return OMX_ErrorNone;

	if(m_bAudioSinkToStart)
	{
		if (m_pCallBack != NULL)
			m_pCallBack (m_pUserData, VOAP_IDC_setAudioStart, NULL, NULL);
		m_bAudioSinkToStart = false;
	}

	if (pAudioFormat->nSamplingRate != (OMX_U32)m_fmtAudio.nSampleRate || pAudioFormat->nChannels != (OMX_U32) m_fmtAudio.nChannels ||
		pAudioFormat->nBitPerSample != (OMX_U32) m_fmtAudio.nSampleBits)
	{
		m_fmtAudio.nSampleRate = pAudioFormat->nSamplingRate;
		m_fmtAudio.nChannels = pAudioFormat->nChannels;
		m_fmtAudio.nSampleBits = pAudioFormat->nBitPerSample;
		m_pCallBack (m_pUserData, VOAP_IDC_setAudioFormat, &m_fmtAudio, NULL);
	}

	m_pCallBack (m_pUserData, VOAP_IDC_RenderAudio, pBuffer->pBuffer + pBuffer->nOffset, &pBuffer->nFilledLen);

	return OMX_ErrorNone;
}

OMX_S32 CVOMEPlayer::vomeCallBack (OMX_PTR pUserData, OMX_S32 nID, OMX_PTR pParam1, OMX_PTR pParam2)
{
	return ((CVOMEPlayer*) pUserData)->CallBackFunc (nID, pParam1, pParam2);
}

OMX_ERRORTYPE CVOMEPlayer::vomeVideoRenderBufferProc (OMX_HANDLETYPE hComponent,
										   OMX_PTR pAppData,
										   OMX_BUFFERHEADERTYPE* pBuffer,
										   OMX_IN OMX_INDEXTYPE nType,
										   OMX_IN OMX_PTR pFormat)
{
	if (nType != (OMX_INDEXTYPE)OMX_VO_IndexVideoBufferType)
		return OMX_ErrorNotImplemented;

	return (OMX_ERRORTYPE) ((CVOMEPlayer *)pAppData)->RenderVideo ((OMX_VO_VIDEOBUFFERTYPE *)pFormat);
}

OMX_ERRORTYPE CVOMEPlayer::vomeAudioRenderBufferProc (OMX_HANDLETYPE hComponent,
													   OMX_PTR pAppData,
													   OMX_BUFFERHEADERTYPE* pBuffer,
													   OMX_IN OMX_INDEXTYPE nType,
													   OMX_IN OMX_PTR pFormat)
{
	return (OMX_ERRORTYPE) ((CVOMEPlayer *)pAppData)->RenderAudio ((OMX_AUDIO_PARAM_PCMMODETYPE *)pFormat, pBuffer);
}

OMX_U32 CVOMEPlayer::vomeSourceDrmProc(OMX_PTR pUserData, OMX_U32 nFlag, OMX_PTR pParam, OMX_U32 nReserved)
{
	return ((CVOMEPlayer *)pUserData)->vomeOnSourceDrm (nFlag, pParam, nReserved);
}

OMX_U32 CVOMEPlayer::vomeOnSourceDrm(OMX_U32 nFlag, OMX_PTR pParam, OMX_U32 nReserved)
{
	if(OMX_VO_SOURCEDRM_FLAG_DRMINFO == nFlag)
	{
		OMX_VO_SOURCEDRM_INFO * pDrmInfo = (OMX_VO_SOURCEDRM_INFO *)pParam;

		if(mpDRMAPI != NULL)
		{
			if(m_pDrmEngine)
				delete m_pDrmEngine;

			m_pDrmEngine = new CDrmEngine(VO_DRMTYPE_UNKNOWN);
			if(!m_pDrmEngine)
				return VO_ERR_OUTOF_MEMORY;

			m_pDrmEngine->SetDrmAPI(mpDRMAPI);

			VO_U32 result = m_pDrmEngine->Open();
			if(OMX_VO_ERR_DRM_OK != result)
			{
				VOLOGE ("m_pDrmEngine->Open fail: 0x%08X", (unsigned int)result);
				return result;
			}

			result = m_pDrmEngine->SetDrmInfo(0, pDrmInfo->pDrmInfo);
			if(OMX_VO_ERR_DRM_OK != result)
			{
				VOLOGE ("m_pDrmEngine->SetDrmInfo fail: 0x%08X", (unsigned int)result);
				return result;
			}

			VO_DRM_FORMAT format;
			result = m_pDrmEngine->GetDrmFormat(&format);
			if(OMX_VO_ERR_DRM_OK != result)
			{
				VOLOGE ("m_pDrmEngine->GetDrmFormat fail: 0x%08X", (unsigned int)result);
				return result;
			}

			result = m_pDrmEngine->Commit();
			if(OMX_VO_ERR_DRM_OK != result)
			{
				VOLOGE ("m_pDrmEngine->Commit fail: 0x%08X", (unsigned int)result);
				return result;
			}

			return OMX_VO_ERR_DRM_OK;

		}
		else if(OMX_VO_DRMTYPE_DIVX == pDrmInfo->nType)
		{
#ifdef __VOPRJ_HTCTUBE__
			vomeNotifyEvent(MEDIA_ERROR, 500);
			return OMX_VO_ERR_DRM_MODULENOTFOUND;
#endif	//__VOPRJ_HTCTUBE__

			//initialize DivX DRM module
			//pDrmInfo->pDrmInfo is OMX_VO_DIVXDRM_INFO*
			if(m_pDrmEngine)
				delete m_pDrmEngine;

			m_pDrmEngine = new CDrmEngine(VO_DRMTYPE_DIVX);
			if(!m_pDrmEngine)
				return VO_ERR_OUTOF_MEMORY;

			VO_U32 result = m_pDrmEngine->LoadLib(NULL);
			if(1 != result)
			{
				VOLOGW ("Load DRM Engine fail");
				return OMX_VO_ERR_DRM_MODULENOTFOUND;
			}

			result = m_pDrmEngine->Open();
			if(OMX_VO_ERR_DRM_OK != result)
			{
				VOLOGE ("m_pDrmEngine->Open fail: 0x%08X", (unsigned int)result);
				vomeNotifyEvent(MEDIA_ERROR, MEDIA_ERROR_VO_DRM_NOAUTH);
				return result;
			}

			voCOMXBaseConfig cfg;
			if(OMX_TRUE == cfg.Open((OMX_STRING)_T("vomeplay.cfg")))
			{
				if (cfg.GetItemValue ((char*)"vomeMF", (char*)"DivXDRM_Version", 0) == 1)
					m_nDivXDRMMode = 1;
			}

			if (1 == m_nDivXDRMMode)
				m_pDrmEngine->SetParameter(VO_PID_DRM_DRMVersion, &m_nDivXDRMMode);

			result = m_pDrmEngine->SetDrmInfo(0, pDrmInfo->pDrmInfo);
			if(OMX_VO_ERR_DRM_OK != result)
			{
				VOLOGE ("m_pDrmEngine->SetDrmInfo fail: 0x%08X", (unsigned int)result);
				if (result == DIVX_DRM_RENTAL_EXPIRED)
					vomeNotifyEvent(MEDIA_ERROR, MEDIA_ERROR_VO_DRM_RENTAL_EXPIRED);
				else
					vomeNotifyEvent(MEDIA_ERROR, MEDIA_ERROR_VO_DRM_NOAUTH);
				return result;
			}

			VO_DRM_FORMAT format;
			memset(&format, 0, sizeof(format));
			result = m_pDrmEngine->GetDrmFormat(&format);
			if(OMX_VO_ERR_DRM_OK != result)
			{
				VOLOGE ("m_pDrmEngine->GetDrmFormat fail: 0x%08X", (unsigned int)result);
				vomeNotifyEvent(MEDIA_ERROR, MEDIA_ERROR_VO_DRM_NOAUTH);
				return result;
			}

			VOLOGI ("mode: %d, total: 0x%08X, used: 0x%08X", format.nMode, (unsigned int)format.uInfo.iRentalCount.nTotal, 
					(unsigned int)format.uInfo.iRentalCount.nUsed);

			if (format.nMode == VO_DRMMODE_PURCHASE)
			{
				vomeNotifyEvent(MEDIA_INFO, MEDIA_INFO_VO_DRM_PURCHASE);

				result = m_pDrmEngine->Commit();
				if(OMX_VO_ERR_DRM_OK != result)
				{
					VOLOGE ("m_pDrmEngine->Commit fail: 0x%08X", (unsigned int)result);
					vomeNotifyEvent(MEDIA_ERROR, MEDIA_ERROR_VO_DRM_NOAUTH);
					return result;
				}

				m_bDrmEngineCommit = true;
			}
			else if (format.nMode == VO_DRMMODE_RENTAL_COUNT) 
				m_iRentalCountInfo = (format.uInfo.iRentalCount.nUsed  & 0xff) << 8 | (format.uInfo.iRentalCount.nTotal & 0xff);

			return OMX_VO_ERR_DRM_OK;
		}
		else if(OMX_VO_DRMTYPE_WindowsMedia == pDrmInfo->nType)
		{
			//initialize WMDRM module
			//pDrmInfo->pDrmInfo is OMX_VO_WMDRM_INFO*
			if(m_pDrmEngine)
				delete m_pDrmEngine;

			m_pDrmEngine = new CDrmEngine(VO_DRMTYPE_WindowsMedia);
			if(!m_pDrmEngine)
				return VO_ERR_OUTOF_MEMORY;

			VO_U32 result = m_pDrmEngine->LoadLib(NULL);
			if(1 != result)
			{
				VOLOGW ("Load DRM Engine fail");
				return OMX_VO_ERR_DRM_MODULENOTFOUND;
			}

			result = m_pDrmEngine->Open();
			if(OMX_VO_ERR_DRM_OK != result)
			{
				VOLOGE ("m_pDrmEngine->Open fail: 0x%08X", (unsigned int)result);
				return result;
			}

			result = m_pDrmEngine->SetDrmInfo(0, pDrmInfo->pDrmInfo);
			if(OMX_VO_ERR_DRM_OK != result)
			{
				VOLOGE ("m_pDrmEngine->SetDrmInfo fail: 0x%08X", (unsigned int)result);
				return result;
			}

			VO_DRM_FORMAT format;
			result = m_pDrmEngine->GetDrmFormat(&format);
			if(OMX_VO_ERR_DRM_OK != result)
			{
				VOLOGE ("m_pDrmEngine->GetDrmFormat fail: 0x%08X", (unsigned int)result);
				return result;
			}

			result = m_pDrmEngine->Commit();
			if(OMX_VO_ERR_DRM_OK != result)
			{
				VOLOGE ("m_pDrmEngine->Commit fail: 0x%08X", (unsigned int)result);
				return result;
			}

			return OMX_VO_ERR_DRM_OK;
		}
		else
			return OMX_VO_ERR_DRM_MODULENOTFOUND;
	}
	else if(OMX_VO_SOURCEDRM_FLAG_DRMDATA == nFlag)
	{
		OMX_VO_SOURCEDRM_DATA * pDrmData = (OMX_VO_SOURCEDRM_DATA *)pParam;
		if(OMX_VO_DRMDATATYPE_MEDIASAMPLE == pDrmData->sDataInfo.nDataType)
		{
			return OMX_VO_ERR_DRM_OK;
		}
		else if(OMX_VO_DRMDATATYPE_VIDEOSAMPLE == pDrmData->sDataInfo.nDataType)
		{
			if(!m_pDrmEngine)
				return OMX_VO_ERR_DRM_ERRORSTATE;

			if (0 == m_nDivXDRMMode)
			{
				VO_U32 result = m_pDrmEngine->DecryptData((VO_DRM_DATAINFO*)&pDrmData->sDataInfo, pDrmData->pData, pDrmData->nSize);
				if(OMX_VO_ERR_DRM_OK != result)
					return result;
			}
			else
			{
				VO_U32 result = m_pDrmEngine->DecryptData2((VO_DRM_DATAINFO*)&pDrmData->sDataInfo, pDrmData->pData, pDrmData->nSize, pDrmData->ppDstData, pDrmData->pnDstSize);
				if(OMX_VO_ERR_DRM_OK != result)
					return result;
			}

			return OMX_VO_ERR_DRM_OK;
		}
		else if(OMX_VO_DRMDATATYPE_AUDIOSAMPLE == pDrmData->sDataInfo.nDataType)
		{
			//DivX DRM module process audio sample
			if(!m_pDrmEngine)
				return OMX_VO_ERR_DRM_ERRORSTATE;

			VO_U32 result = m_pDrmEngine->DecryptData((VO_DRM_DATAINFO*)&pDrmData->sDataInfo, pDrmData->pData, pDrmData->nSize);
			if(OMX_VO_ERR_DRM_OK != result)
				return result;

			return OMX_VO_ERR_DRM_OK;
		}
		else if(OMX_VO_DRMDATATYPE_PACKETDATA == pDrmData->sDataInfo.nDataType)
		{
			if(!m_pDrmEngine)
				return OMX_VO_ERR_DRM_ERRORSTATE;

			VO_U32 result = m_pDrmEngine->DecryptData((VO_DRM_DATAINFO*)&pDrmData->sDataInfo, pDrmData->pData, pDrmData->nSize);
			if(OMX_VO_ERR_DRM_OK != result)
				return result;

			return OMX_VO_ERR_DRM_OK;
		}
		else if(OMX_VO_DRMDATATYPE_JITSAMPLE == pDrmData->sDataInfo.nDataType)
		{
			if(!m_pDrmEngine)
				return OMX_VO_ERR_DRM_ERRORSTATE;

			//if (0 == m_nDivXDRMMode)
			//	return OMX_VO_ERR_DRM_OK;

			VO_U32 result = m_pDrmEngine->DecryptData((VO_DRM_DATAINFO*)&pDrmData->sDataInfo, pDrmData->pData, pDrmData->nSize);
			if(OMX_VO_ERR_DRM_OK != result)
				return result;

			return OMX_VO_ERR_DRM_OK;
		}
		else
			return OMX_VO_ERR_DRM_BADPARAMETER;
	}
	else if(OMX_VO_SOURCEDRM_FLAG_FLUSH == nFlag)
	{
		//reserved
		return OMX_VO_ERR_DRM_OK;
	}
	else if(OMX_VO_SOURCEDRM_FLAG_ISSUPPORTED == nFlag)
	{
		//check if DRM module occur
		return OMX_VO_ERR_DRM_OK;
	}
	else
		return OMX_VO_ERR_DRM_BADPARAMETER;
}

void CVOMEPlayer::vomeNotifyEvent(int nMsg, int nExt1 /* = 0 */, int nExt2 /* = 0 */)
{
	VOA_NOTIFYEVENT event;
	event.msg  = nMsg;
	event.ext1 = nExt1;
	event.ext2 = nExt2;

	if(m_pCallBack)
		m_pCallBack(m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
}

int CVOMEPlayer::LoadModule (void)
{
	vostrcpy(m_szDllFile, _T("voOMXME"));
	vostrcpy(m_szAPIName, _T("voOMXGetEngineAPI"));

#if defined _WIN32
	vostrcat(m_szDllFile, _T(".Dll"));
#elif defined LINUX
	vostrcat(m_szDllFile, _T(".so"));
#endif

	if(CDllLoad::LoadLib(NULL) == 0)
	{
		VOLOGI ("LoadLib fail");
		return -1;
	}

	VOOMXGetEngineAPI pAPI = (VOOMXGetEngineAPI) m_pAPIEntry;
	if (pAPI == NULL)
		return -1;
	pAPI (&m_fAPI, 0);

#ifdef _WIN32
#ifdef _WIN32_WCE
	VOOMXGetComponentAPI pComp = (VOOMXGetComponentAPI) GetProcAddress (m_hDll, _T("voOMXGetComponentAPI"));
#else
	VOOMXGetComponentAPI pComp = (VOOMXGetComponentAPI) GetProcAddress (m_hDll, "voOMXGetComponentAPI");
#endif // _WIN32_WCE
#elif defined _LINUX
	VOOMXGetComponentAPI pComp = (VOOMXGetComponentAPI) dlsym (m_hDll, "voOMXGetComponentAPI");
#endif // _WIN32

	if (pComp == NULL)
		return -1;

	pComp (&m_fComp, 0);

	return 0;
}
