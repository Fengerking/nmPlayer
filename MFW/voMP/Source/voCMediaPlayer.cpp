	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCMediaPlayer.cpp

	Contains:	voCMediaPlayer class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-18		JBF			Create file

*******************************************************************************/
#ifdef __SYMBIAN32__
#include <stdlib.h>
#include <aknnotewrappers.h>
#elif defined _IOS
#include <stdlib.h>
#elif defined _MAC_OS
#include <stdlib.h>
#else
#include <malloc.h>
#endif // __SYMBIAN32__
#include <string.h>
#include <stdio.h>

#ifdef _LINUX_ANDROID
#ifndef __VO_NDK__
#include <cutils/properties.h>
#include <utils/threads.h>
#endif  //__VO_NDK__
#include <sys/prctl.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif // _LINUX_ANDROID

#include "voOSFunc.h"
#include "cmnFile.h"
#include "CFileFormatCheck.h"

#include "CRTSPSource.h"
#include "CHTTPPDSource.h"

#include "voCDataSource.h"

#include "voCMediaPlayer.h"

#include "voAMediaCodec.h"

#define LOG_TAG "voCMediaPlayer"
#include "voLog.h"


#if defined (_IOS)
#include "CiOSVideoRender.h"
#endif

#if defined (_MAC_OS)
#include "CMacOSVideoRender.h"
#endif

#ifdef _MAC_OS_CG
#include "CCGRender.h"
#endif

voCMediaPlayer::voCMediaPlayer(VOMPListener pListener, void * pUserData)
	: voCBasePlayer (pListener, pUserData)
	, m_pMsgThread (NULL)
	, m_pFileOP (NULL)
	, m_pSource (NULL)
	, m_nAudioBuffTime (0)
	, m_nStatus (VOMP_STATUS_INIT)
	, m_nStatusChanging(0)
 	, m_nOpenFlag (VOMP_FLAG_SOURCE_SYNC)
	, m_nSeekMode (0)
	, m_pLibFunc (NULL)
	, m_pDrmCB (NULL)
	, m_pFileSource (NULL)
	, m_pDataSource (NULL)
	, m_bAudioOnly (false)
	, m_bVideoOnly (false)
	, m_nAudioPlayMode (VOMP_PUSH_MODE)
	, m_nVideoPlayMode (VOMP_PUSH_MODE)
	, m_nDisableDropVideoFrame (0)
	, m_nDisableDeblockVideo (1)
	, m_nDeblockVideo(1)
	, m_nCheckFastPerformance (0)
	, m_nRenderSuccess(0)
	, m_nLowLatence(0)
	, m_nAudioOnOff(1)
	, m_nVideoOnOff(1)
	, m_nAudioReaded(0)
	, m_nVideoReaded(0)
	, m_nVideoPlayBack(1)
	, m_nAudioVideoType(VOMP_AVAILABLE_AUDIOVIDEO)
	, m_pAudioDec (NULL)
	, m_nAudioRenderBufferCount(1)
	, m_pAudioDecThread(NULL)
	, m_pARBManager(NULL)
	, m_nAudioVolume (100)
	, m_nAudioAACPlus(0)
	, m_bOnlyGetAudio (VO_FALSE)
	, m_pAudioEffect (NULL)
	, m_bEffectEnable (VO_TRUE)
	, m_bAddAudioEffect (VO_FALSE)
	, m_bAllowDolbyAudio (VO_TRUE)
	, m_pAudioEftOutPcmBuff(NULL)
	, m_pAudioRender (NULL)
	, m_nAudioStepTime (100)
	, m_pAudioDecOutPcmBuff(NULL)
	, m_nAudioDateCount(0)
	, m_nAudioCountStartTime(0)
	, m_nRCGetVideoTrackDataAfterPureAudio(VO_ERR_SOURCE_NEEDRETRY)
	, m_pVideoDec (NULL)
	, m_bVideoDecInputRetry (VO_FALSE)
	, m_bVideoDecHungry (VO_FALSE)
	, m_eRecreateDecoderState (VOMP_RECREATEDECODERSTATE_NONE)
#if !defined __VOPRJ_MOTOPASTEUR__
	, m_nVideoDropRender (1)
#else
	, m_nVideoDropRender (0)
#endif
	, m_nVideoDateCount (0)
	, m_nVideoCountStartTime(0)
	, m_nGetThumbnail (0)
	, m_bSetDispRect (VO_FALSE)
	, m_pVidDecCB (NULL)
	, m_nAspectRatio(VOMP_RATIO_00)
	, m_bOnlyGetVideo(VO_FALSE)
	, m_bUsingHWCodec(VO_FALSE)
	, m_bFirstFrmRnd(VO_FALSE)
	, m_pVideoEffect (NULL)
	, m_pVideoEffectType(VOMP_VE_Null)
	, m_bVideoEffectOn (VO_FALSE)
	, m_nVideoPerfOnOff(0)
	, m_pVideoRender (NULL)
	, m_bAudioNewTime(VO_FALSE)
	, m_bVideoNewTime(VO_FALSE)
	, m_nNewTimeBegin(0)
	, m_nNewTimeVideo(0)
	, m_nIFrameSkip(0)
	, m_hView (NULL)
	, m_nColorType (VOMP_COLOR_YUV_PLANAR420)
	, m_nZoomType(VO_ZM_LETTERBOX)
	, m_llRenderAdjustTime (100)
	, m_llDSPRefTime(-1)
	, m_llDSPTimeOffset(0)
	, m_nVideoEOSArrived(0)
	, m_nAudioEOSArrived(0)
	, m_nAudioDecError(0)
	, m_nVideoDecError(0)
	, m_pAudioThread (NULL)
	, m_pVideoThread (NULL)
	, m_pVideoDecThread (NULL)
	, m_pVideoOutputBuff (NULL)
	, m_nVideoRenderCount (1)
	, m_nVideoRenderAdd(0)
	, m_nVideoRndCopy (1)
	, m_nVideoStartTime(0)
	, m_ppVideoOutBuffers (NULL)
	, m_ppVideoOutBufShared(NULL)
	, m_pVideoOutWidth (NULL)
	, m_pVideoOutHeight (NULL)
	, m_nVideoOutFillIndex (0)
	, m_nVideoOutPlayIndex (0)
	, m_nVideoRndWidth (320)
	, m_nVideoRndHeight (240)
	, m_nLastFrameTime(0)
	, m_nOnlyVideoDropCount(0)
	, m_nOnlyVideoDropTime(0)
	, m_nReceiveFrameTime(0)
	, m_nReceiveFrameNum(0)
	, m_nFrameRate(33)
	, m_nDecStartSysTime(0)
	, m_nDecSucess(1)
	, m_hCheckLib (NULL)
	, m_pLicenseData(NULL)
	, m_nCPUNumber (1)
	, m_nCPUVersion (6)
	, m_pCPULoadInfo(NULL)
	, m_pFileSink (NULL)
	, m_nSocketConnectionType(0)
	, m_nNotifyBuffingTime (1500)
	, m_nStatusCheckTime (500)
	, m_nShowLogLevel (0)
	, m_nIdleThdPriority (2011)
	, m_bIdleStop (true)
	, m_pIdleThread (NULL)
	, m_nSetAudioNameType(0)
	, m_nSetAudioAPIType(0)
	, m_nSetVideoNameType(0)
	, m_nSetVideoAPIType(0)
#if defined(_IOS) || defined(_MAC_OS)
	, m_pLogoEffect(NULL)
	, m_bAudioBufFull(VO_FALSE)
#endif
	, m_pVideoLogoInfo(NULL)
	, m_pAudioLogoInfo(NULL)
	, mJavaVM(NULL)
	, mAudioDecInputRetry(VO_FALSE)
	, m_bSendAudioBuffer(VO_FALSE)
	, m_bRedrawVideo(VO_FALSE)
	, m_pAudioSpeed(NULL)
	, m_bASpeedEnable(VO_FALSE)
	, m_nAudioPlaySpeed(10)
	, m_bRenderAudioOnOff(VO_TRUE)
	, m_fDumpVideoinput(NULL)
	, mADSPClock(NULL)
	, mOutputType(VO_AUDIO_CodingPCM)
	, m_bPauseRefClock(VO_FALSE)
	, preVideoRenderTimeStamp(0)
	, m_nAudioVideoUnsupport(0)
	, mIsSuspend(VO_FALSE)
	, m_nStatusRenderFlag (0)
	, m_bNewVOVideoDec(VO_FALSE)
{
	vostrcpy (m_szWorkPath, _T(""));
	vostrcpy (m_szLicensePath, _T(""));
	//strcpy (m_szLicenseText, "VisualOn");
	memset(m_szLicenseText, 0, sizeof(m_szLicenseText));

//#ifdef _LINUX_ANDROID
//		char	szPackageName[1024];
//		FILE * hFile = fopen("/proc/self/cmdline", "rb");
//		if (hFile != NULL)
//		{  
//			fgets(szPackageName, 1024, hFile);
//			fclose(hFile);
//			if (strstr (szPackageName, "com.") != NULL)
//				sprintf(m_szWorkPath, "/data/data/%s/", szPackageName);
//		}
//#endif //_LINUX_ANDROID
	
	cmnMemFillPointer(0x53465053);//'SFPS'0x53465053

	m_sLibFunc.pUserData = this;
	m_sLibFunc.LoadLib = vomtLoadLib;
	m_sLibFunc.GetAddress = vomtGetAddress;
	m_sLibFunc.FreeLib = vomtFreeLib;

	m_pLibFunc = &m_sLibFunc;

	memset (&m_rcDraw, 0, sizeof (m_rcDraw));
	memset (&m_sAudioTrackInfo, 0, sizeof (m_sAudioTrackInfo));
	memset (&m_sVideoTrackInfo, 0, sizeof (m_sVideoTrackInfo));

	memset (&m_sSetAudioName, 0, sizeof(VO_TCHAR)*256);
	memset (&m_sSetAudioAPI, 0, sizeof(VO_TCHAR)*256);
	memset(&m_sSetVideoName, 0, sizeof(VO_TCHAR)*256);	
	memset(&m_sSetVideoAPI, 0, sizeof(VO_TCHAR)*256);

	memset(m_nAudioBitrate, 0, sizeof(int)*10);
	memset(m_nVideoBitrate, 0, sizeof(int)*10);

	m_sCPUInfo.nCoreCount = m_nCPUNumber;
	m_sCPUInfo.nCPUType = 0;
	m_sCPUInfo.nFrequency = 500;
	m_sCPUInfo.lReserved = 0;
	
	Reset ();

	CheckCPUInfo ();

	OpenPlayConfigFile (NULL);

	OpenVOSDKConfigFile (NULL);

#ifndef __SYMBIAN32__
	m_pMsgThread = new voCBaseThread ("vomp Message");
#endif //__SYMBIAN32__

	memset (&m_Source, 0, sizeof (m_Source));
	memset (&m_smpSinkVideo, 0, sizeof (m_smpSinkVideo));
	memset (&m_smpSinkAudio, 0, sizeof (m_smpSinkAudio));

	memset(m_sDigestName, 0, 256*sizeof(char));
	memset(m_sDigestPass, 0, 256*sizeof(char));
	memset ( &m_szUUID, 0, 256 *sizeof (char));

	m_pAudioLogoInfo = new voCPlayerLogoInfo(0);
	m_pVideoLogoInfo = new voCPlayerLogoInfo(1);
	m_pCPULoadInfo =  new CCPULoadInfo();

#if defined(_IOS) || defined(_MAC_OS)
#ifdef _USE_VO_LOGO
	m_pLogoEffect = new voLogoEffect;
#elif defined _USE_VO_LOGOEXT
    m_pLogoEffect = new voLogoEffectExt;
#endif
#endif

#if defined DUMP_VIDEO_HEAD
	if (!m_fDumpVideoinput)
	{
		VOLOGI("Open file /sdcard/dump/dump.h265");
		m_fDumpVideoinput = fopen("/sdcard/dump/dump.h265", "wb");
	}	
#endif
}

voCMediaPlayer::~voCMediaPlayer(void)
{
	Reset ();

	if (m_pMsgThread != NULL)
	{
		m_pMsgThread->Stop ();
		delete m_pMsgThread;
		m_pMsgThread = NULL;
	}

	if (m_hCheckLib != NULL) 
	{
#if !defined (__SYMBIAN32__) && !defined (_METRO)
		voCheckLibUninit (m_hCheckLib);
#else
		VOLOGE("m_hCheckLib is not NULL");
#endif //_METRO __SYMBIAN32__
	}

	if (m_pFileSink != NULL)
	{
		delete m_pFileSink;
		m_pFileSink = NULL;
	}
	if (m_smpSinkVideo.Buffer != NULL)
	{
		delete []m_smpSinkVideo.Buffer;
		m_smpSinkVideo.Buffer = NULL;
	}

	if(m_pLicenseData)
	{
		delete []m_pLicenseData;
		m_pLicenseData = NULL;
	}

	if(m_pVideoLogoInfo)
	{
		delete m_pVideoLogoInfo;
		m_pVideoLogoInfo = NULL;
	}

	if(m_pAudioLogoInfo)
	{
		delete m_pAudioLogoInfo;
		m_pAudioLogoInfo = NULL;
	}

	if(m_pCPULoadInfo)
	{
		delete m_pCPULoadInfo;
		m_pCPULoadInfo = NULL;
	}

#if defined(_IOS) || defined(_MAC_OS)
	if(m_pLogoEffect)
	{
		delete m_pLogoEffect;
		m_pLogoEffect = NULL;
	}
#endif

#if defined DUMP_VIDEO_HEAD
	if(m_fDumpVideoinput)
	{
		delete m_fDumpVideoinput;
		m_fDumpVideoinput = NULL;
	}
#endif
}

int voCMediaPlayer::SetDataSource (void * pSource, int nFlag)
{
	voCAutoLock lock (&m_mtStatus);
#ifndef _MAC_OS
#ifndef _IOS
	VOLOGI ("pSource %d, nFlag %d", (int)pSource, nFlag);
#endif
#endif
    
#ifndef _MAC_OS
#ifndef _IOS
#ifndef _METRO
	if(m_pLibFunc)
	{
		VO_PTR hCheckLic = m_pLibFunc->LoadLib(m_pLibFunc->pUserData, (char *)("voVidDec"), 0);

		if(hCheckLic == NULL)
		{
			VOLOGI ("Could not check the license file");
			return VOMP_ERR_Status;
		}
		else
		{			
			m_pLibFunc->FreeLib(m_pLibFunc->pUserData, hCheckLic, 0);
		}
	}
	else
	{
		return VOMP_ERR_Status;
	}
#endif //_METRO
#endif
#endif

#ifndef __SYMBIAN32__
#ifndef _METRO
	if (m_hCheckLib == NULL)
	{
		int nRC = voCheckLibInit (&m_hCheckLib, 0,VO_LCS_LIBOP_FLAG, m_hInst, (VO_LIB_OPERATOR  *)m_pLibFunc);
		if (m_hCheckLib != NULL)
		{
			VO_VIDEO_BUFFER videoBuff;
			if(m_pLicenseData)
			{
				videoBuff.ColorType = (VO_IV_COLORTYPE)VOLICENSE_SET_DATA;
				videoBuff.Buffer[0] = (VO_PBYTE)m_pLicenseData;
				nRC = voCheckLibCheckImage (m_hCheckLib, &videoBuff, NULL);
			}

			if(vostrlen(m_szLicensePath))
			{
				videoBuff.ColorType = (VO_IV_COLORTYPE)VOLICENSE_SET_PATH;
				videoBuff.Buffer[0] = (VO_PBYTE)m_szLicensePath;
				nRC = voCheckLibCheckImage (m_hCheckLib, &videoBuff, NULL);			
			}
			
			if(strlen(m_szLicenseText) > 0)
			{
				videoBuff.ColorType = (VO_IV_COLORTYPE)VOLICENSE_SET_TEXT;
				videoBuff.Buffer[0] = (VO_PBYTE)m_szLicenseText;
				nRC = voCheckLibCheckImage (m_hCheckLib, &videoBuff, NULL);
			}
			

			videoBuff.ColorType = (VO_IV_COLORTYPE)VOLICENSE_SET_VOLOG;
			videoBuff.Buffer[0] = (VO_PBYTE)&m_cbVOLOG;
			nRC = voCheckLibCheckImage (m_hCheckLib, &videoBuff, NULL);			
		}
	}
#endif //_METRO
#endif // __SYMBIAN32__
    
	if (m_pMsgThread != NULL)
		m_pMsgThread->Start (m_fThreadCreate);

	if (m_nStatus == VOMP_STATUS_LOADING)
		return VOMP_ERR_Status;

	Reset ();

	m_pSource = pSource;
	m_nOpenFlag = nFlag;
	if ((m_nOpenFlag & VOMP_FLAG_SOURCE_URL) == VOMP_FLAG_SOURCE_URL)
	{
		vostrcpy (m_szURL, (VO_PTCHAR) pSource);
		CFileFormatCheck filCheck (NULL , &g_memOP);
		m_nFF = filCheck.GetFileFormat(m_szURL, ~FLAG_CHECK_AUDIOREADER);
	}

	if ((m_nOpenFlag & VOMP_FLAG_SOURCE_SYNC) == VOMP_FLAG_SOURCE_SYNC)
	{
		int nRC = LoadSource ();
		// added by Lin Jun 20110115, next SetDatasource will fail if not reset status.
		if(nRC != VOMP_ERR_None)
			m_nStatus = VOMP_STATUS_STOPPED;
		// add end
		
		return nRC;
	}
	else
	{
		if (m_pMsgThread != NULL)
		{
			voCBaseEventItem * pEvent = m_pMsgThread->GetEventByType (VOMP_EVENT_LOAD);
			if (pEvent == NULL)
				pEvent = new voCMediaPlayerEvent (this, &voCMediaPlayer::onLoadSource, VOMP_EVENT_LOAD);
			m_pMsgThread->PostEventWithRealTime (pEvent, -1);

			return VOMP_ERR_None;
		}
		else
		{
			return VOMP_ERR_Status;
		}
	}

	return VOMP_ERR_None;
}

int voCMediaPlayer::SendBuffer (int nSSType, VOMP_BUFFERTYPE * pBuffer)
{
//	voCAutoLock lock (&m_mtStatus);
	voCAutoLock lockSetpos (&m_mtSetPos);

	if (vostrlen (m_szURL) > 0)
		return VOMP_ERR_Status;

	//VOLOGI ("media type: %d, nflag: %d, media size: %d, media time: %d", nSSType, pBuffer->nFlag, pBuffer->nSize, (int)pBuffer->llTime);

	voCDataSource * pSource = (voCDataSource *)m_pFileSource;
	if (pSource == NULL)
		return VOMP_ERR_Status;

	return pSource->AddBuffer (nSSType, pBuffer);
}

int voCMediaPlayer::GetVideoBuffer (VOMP_BUFFERTYPE ** ppBuffer)
{
	if(VO_TRUE == m_bRedrawVideo)
		return redraw(ppBuffer);

	if (m_bVideoEOS)
	{
		if (m_nVideoOutPlayIndex  == m_nVideoOutFillIndex)
		{
			voOS_Sleep (10);
			return VOMP_ERR_EOS;
		}
	}

	voCAutoLock lockVideo (&m_mtVideoFlush);

	if (m_nVideoPlayMode == VOMP_PUSH_MODE)
		return VOMP_ERR_Status;

	if (m_nStatus == VOMP_STATUS_PAUSED && m_nVideoRenderNum > 0)
		return VOMP_ERR_Retry;

	// !!!!!!we need drop one frame for full state so that we can call GetTrackData to judge if source need VOMP_ERR_FLush_Buffer
	// !!!!!!currently, we block it and will re-fix in future, East 20131120
// 	if(m_nStatusChanging && m_nVideoRenderCount > 1)
// 	{
// 		voCAutoLock lockVideoDec (&m_mtVideoRender);
// 		if (m_nVideoOutFillIndex >= m_nVideoOutPlayIndex + m_nVideoRenderCount - 1)
// 		{
// 			VO_VIDEO_BUFFER * pVideoBuffer = m_ppVideoOutBuffers[m_nVideoOutPlayIndex % m_nVideoRenderCount];
// 			if (pVideoBuffer == NULL)
// 				return VOMP_ERR_Retry;
// 
// 			m_pVideoOutputBuff = pVideoBuffer;
//			cancelBufferFromVideoDec(m_pVideoOutputBuff);
// 			m_nVideoOutPlayIndex++;
// 		}		
// 		
// 		return VOMP_ERR_Retry;
// 	}

	int nRC = DoVideoRender (ppBuffer);

	return nRC;
}

int voCMediaPlayer::GetAudioBuffer (VOMP_BUFFERTYPE ** ppBuffer)
{
	if (m_pVideoLogoInfo)
		m_pVideoLogoInfo->UpdataPerformanceData();

	if (m_bAudioEOS)
	{
		voOS_Sleep (10);
		return VOMP_ERR_EOS;
	}

	voCAutoLock lockAudio (&m_mtAudioFlush);

	if (m_nAudioPlayMode == VOMP_PUSH_MODE)
		return VOMP_ERR_Status;

	if (m_nStatus == VOMP_STATUS_PAUSED || m_nStatus <= VOMP_STATUS_LOADING)
		return VOMP_ERR_Retry;

	int nRC = DoAudioRender (ppBuffer);

	if((VOMP_ERR_None == nRC || VOMP_ERR_FormatChange == nRC) && VO_TRUE == m_bSendAudioBuffer && NULL != m_pListener)
	{
		int nV1 = 0;
		if(VOMP_ERR_FormatChange == nRC)
			nV1 = VOMP_FLAG_BUFFER_NEW_FORMAT;

		m_pListener (m_pUserData, VOMP_CB_Pcm_Output, &nV1, *ppBuffer);
	}

	if (VOMP_ERR_Retry == nRC && m_bOnlyGetVideo)
	{
		voOS_Sleep (2);
	}	

	return nRC;
}

int voCMediaPlayer::Run (void)
{
	voCAutoLock lock (&m_mtStatus);

	if (m_pFileSource == NULL || m_nStatus <= VOMP_STATUS_LOADING || m_nVideoTrack < 0)
	{
		VOLOGI("m_pFileSource 0x%08X, m_nStatus %d, m_nVideoTrack %d", m_pFileSource, m_nStatus, m_nVideoTrack);
		return VOMP_ERR_Status;
	}

	if (m_nStatus == VOMP_STATUS_RUNNING)
		return VOMP_ERR_None;

	m_nStatus = VOMP_STATUS_RUNNING;
	m_nStatusChanging = 1;
	m_nAudioRndNuming = 0;
	m_nVideoRndNuming = 0;
	
#if defined _IOS || defined(_MAC_OS)
	m_nStartSysTime = 0;
#endif	

	// start audio stream
	if (m_nAudioPlayMode == VOMP_PUSH_MODE && m_nAudioTrack >= 0)
	{
		if (m_pAudioThread == NULL)
		{
			m_pAudioThread = new voCBaseThread ("vomp Audio Render");
			if(!m_pAudioThread)
				return VOMP_ERR_OutMemory;
		}
		m_pAudioThread->Start (m_fThreadCreate);

		postAudioRenderEvent (-1);
	}

	// start audio decoder thread for audio render buffering mechanism
	if(m_nAudioRenderBufferCount > 1 && m_nAudioTrack >= 0)
	{
		if(m_pAudioDecThread == NULL)
		{
			m_pAudioDecThread = new voCBaseThread("vomp Audio Decoder");
			if(!m_pAudioDecThread)
				return VOMP_ERR_OutMemory;
		}
		m_pAudioDecThread->Start(m_fThreadCreate);

		postAudioDecoderEvent(-1);
	}

	// Start video stream
	if (m_nVideoPlayMode == VOMP_PUSH_MODE && m_nVideoTrack >= 0)
	{
#if defined _IOS || defined(_MAC_OS)
		// It seems that need to create video render in main thread on iOS.
		if(m_pVideoRender == NULL)
		{			
#ifdef _IOS
			m_pVideoRender = new CiOSVideoRender (NULL, m_hView, NULL);
#else
			
#ifdef _MAC_OS_CG
			m_pVideoRender = new CCGRender(NULL, m_hView, NULL);
#else
			m_pVideoRender = new CMacOSVideoRender (NULL, m_hView, NULL);
#endif
			
#endif
			if(!m_pVideoRender)
				return VOMP_ERR_OutMemory;
			m_pVideoRender->Start();
			VO_RECT r;
			r.left=m_rcDraw.left;r.top=m_rcDraw.top;r.right=m_rcDraw.right;r.bottom=m_rcDraw.bottom;
			m_pVideoRender->SetDispRect (m_hView, &r, (VO_IV_COLORTYPE)m_nColorType);
			
			m_pVideoRender->SetVideoInfo (m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
			m_pVideoRender->SetParam (VO_PID_COMMON_LIBOP, (VO_LIB_OPERATOR *)m_pLibFunc);
		}
#endif	
		if (m_pVideoThread == NULL)
		{
			m_pVideoThread = new voCBaseThread ("vomp Video Render");
			if(!m_pVideoThread)
				return VOMP_ERR_OutMemory;
		}
		m_pVideoThread->Start (m_fThreadCreate);

		postVideoRenderEvent (-1);
	}

	// Start video stream
	if (m_nVideoRenderCount > 1 && m_nVideoTrack >= 0)
	{
		if (m_pVideoDecThread == NULL)
		{
			m_pVideoDecThread = new voCBaseThread ("vomp Video Decoder");
			if(!m_pVideoDecThread)
				return VOMP_ERR_OutMemory;
		}
		m_pVideoDecThread->Start (m_fThreadCreate);

		postVideoDecoderEvent (-1);
	}

	if(m_pVideoLogoInfo)
	{
		m_pVideoLogoInfo->Flush();
		m_pVideoLogoInfo->Start();
	}

	if (m_nVideoTrack >= 0)
	{		
//#ifndef _WIN32
		if(!m_bOnlyGetAudio) // if pure audio, we should not post VOMP_CB_VideoStartBuff.
		{
			m_bSendStartVideoBuff = VO_TRUE;
			postCheckStatusEvent (VOMP_CB_VideoStartBuff);
		}
//#endif
	}

	if (m_pAudioRender != NULL)
		m_pAudioRender->Start ();
	m_pFileSource->Start ();

	{
		VOLOGI("start decoder");
		voCAutoLock lockCodec (&m_mtCodec);
		VOLOGI("start decoder enter m_mtCodec m_pVideoDec = %p", m_pVideoDec);
		if (m_pAudioDec != NULL)
			m_pAudioDec->Start ();
		if (m_pVideoDec != NULL)
			m_pVideoDec->Start ();
	}

#ifndef _MAC_OS
#ifndef _IOS
	m_nStartSysTime = 0;
#endif
#endif
	
	m_nLastVideoSysTime = 0;
	if (m_nStartPlayTime == 0)
		m_nStartPlayTime = (VO_S64)voOS_GetSysTime () - (VO_S64)m_nSeekPos;

	if (m_pMsgThread != NULL)
	{
		m_pMsgThread->Start ();
		// m_pMsgThread->CancelEventByType (VOMP_EVENT_STATUS);
		m_pMsgThread->CancelEventByMsg (VOMP_CB_CheckStatus);
	}

	postCheckStatusEvent (VOMP_CB_CheckStatus, 0, 0, m_nStatusCheckTime);

	if (m_nIdleThdPriority != 2011)
	{
		if (m_pIdleThread == NULL)
		{
			m_bIdleStop = false;
			voThreadCreate (&m_pIdleThread, &m_nIdleThdID, IdleThreadProc, this, 0);
		}
	}

	return VOMP_ERR_None;
}

int voCMediaPlayer::Pause (void)
{
	if (m_pFileSource == NULL || m_nStatus != VOMP_STATUS_RUNNING)
		return VOMP_ERR_Status;

	if (m_pVideoDecThread != NULL)
		m_pVideoDecThread->Pause ();
	if (m_pVideoThread != NULL)
		m_pVideoThread->Pause ();
	if (m_pAudioDecThread != NULL)
		m_pAudioDecThread->Pause ();
	if (m_pAudioThread != NULL)
		m_pAudioThread->Pause ();

	voCAutoLock lock (&m_mtStatus);

	m_nStatus = VOMP_STATUS_PAUSED;
	m_pFileSource->Pause ();
	if (m_pAudioRender != NULL)
		m_pAudioRender->Pause ();

	{
		VOLOGI("pause decoder");
		voCAutoLock lockCodec (&m_mtCodec);
		VOLOGI("pause decoder enter m_mtCodec");
		if (m_pAudioDec != NULL)
		{
			// use the multi thread in jni level, delete it make the JVM crash.
			if(mIsSuspend && m_nAudioRenderBufferCount == 1)
			{
				delete m_pAudioDec;
				m_pAudioDec = NULL;
			}
			else		
				m_pAudioDec->Pause ();
		}
			
		if (m_pVideoDec != NULL)
		{
			if(mIsSuspend)
			{
				bool isSDec = !vostrcmp(m_sSetVideoName, _T(""));
				if(!isSDec)
				{
					delete m_pVideoDec;
					m_nIFrameSkip = 1;
					m_pVideoDec = NULL;
					m_hView = NULL;				
				}
				/*
				//If software decoder want to delete too, the follows code have to be called 
				if(isSDec)
				{
				ReleaseVideoRenderBuffers();
				m_pVideoOutputBuff = NULL;
				}
				*/
			}
			else
				m_pVideoDec->Pause ();
		}

		if(mIsSuspend)
			mIsSuspend = VO_FALSE;
	}

	return VOMP_ERR_None;
}

int voCMediaPlayer::Stop (void)
{
	if (m_pFileSource == NULL || m_nStatus <= VOMP_STATUS_LOADING)
		return VOMP_ERR_Status;

	if (m_nStatus == VOMP_STATUS_STOPPED)
	{
		m_bNotifyCompleted = false;

		if (m_pMsgThread != NULL)
			m_pMsgThread->Stop ();		

		return VOMP_ERR_None;
	}

	ShowResult ();

	if (m_pVideoDecThread != NULL)
		m_pVideoDecThread->Stop ();
	if (m_pVideoThread != NULL)
		m_pVideoThread->Stop ();
	if (m_pAudioDecThread != NULL)
		m_pAudioDecThread->Stop ();
	if (m_pAudioThread != NULL)
		m_pAudioThread->Stop ();

	voCAutoLock lock (&m_mtStatus);
	m_nStatusChanging = 1;	
	VOMP_STATUS nOldStatus = m_nStatus;
	m_nStatus = VOMP_STATUS_STOPPED;
	m_bSeekingPos = VO_FALSE;
	m_nAspectRatio = VOMP_RATIO_00;
	m_bFirstFrmRnd = VO_FALSE;

	cancelBuffersFromVOVideoDec();

	{
		VOLOGI("stop decoder");
		voCAutoLock lockCodec (&m_mtCodec);
		VOLOGI("stop decoder enter m_mtCodec");
		if (m_pAudioDec != NULL)
			m_pAudioDec->Stop ();
		if (m_pVideoDec != NULL)
			m_pVideoDec->Stop ();
	}

	m_pFileSource->Stop ();
	if (m_pAudioRender != NULL)
		m_pAudioRender->Stop ();

#ifndef __SYMBIAN32__
	if (m_pVideoRender != NULL)
		delete m_pVideoRender;
	m_pVideoRender = NULL;
#endif // __SYMBIAN32__

#if defined(_IOS) || defined(_MAC_OS)
	m_bAudioBufFull = VO_FALSE;
#endif

	if (m_nFF != VO_FILE_FFUNKNOWN && m_nFF != VO_FILE_FFSTREAMING_RTSP && m_nFF != VO_FILE_FFAPPLICATION_SDP && m_nFF != VO_FILE_FFSTREAMING_HTTPPD)
	{
		if (nOldStatus != VOMP_STATUS_STOPPED && m_nGetThumbnail == 0)
		{
			SetCurPos (0);
		}
	}
	else
	{
		m_nSeekPos = 0;
		m_sVideoDecOutBuf.Time = 0;
		m_llLastVideoDecInTime = 0;
		m_llLastVideoDecOutTime = 0;
		m_sAudioDecOutBuf.Time = 0;
		m_nStartSysTime = 0;
		m_nStartMediaTime = 0;
		m_nRCGetVideoTrackDataAfterPureAudio = VO_ERR_SOURCE_NEEDRETRY;
		m_nVideoRenderNum = 0;
		m_nVideoRndNuming = 0;
		m_nVideoDecoderNum = 0;
		m_nAudioRenderNum = 0;
		m_nAudioRndNuming = 0;
		m_llAudioRenderTime = 0;
		m_llLastAudioRenderTime = 0;
		m_llRenderBufferTime = 0;
		m_llRenderSystemTime = 0;

		if(m_nAudioRenderBufferCount > 1)
		{
			voCAutoLock lockAudioDec(&m_mtAudioRender);
			if(m_pARBManager)
				m_pARBManager->flush();
		}

		m_bSendStartVideoBuff = VO_FALSE;
		m_bSendStartAudioBuff = VO_FALSE;

		m_bVideoEOS = VO_FALSE;
		m_bAudioEOS = VO_FALSE;

		m_nVideoEOSArrived = 0;
		m_nAudioEOSArrived = 0;

		m_nVideoDcdTimeIndex = 0;
		m_nVideoRndTimeIndex = 0;
		m_nDroppedVidooFrames = 0;

		m_nVideoOutFillIndex = 0;
		m_nVideoOutPlayIndex = 0;

		m_sVideoSample.Size = 0;
		m_sVideoSample.Time = 0;

		m_sAudioSample.Size = 0;
		m_sAudioSample.Time = 0;
		m_nAudioDecOutLength = 0;
		m_nAudioRndLength = 0;

		{
			VOLOGI("flush decoder");
			voCAutoLock lockCodec (&m_mtCodec);
			VOLOGI("flush decoder enter m_mtCodec");
			if (m_pVideoDec != NULL)
				m_pVideoDec->Flush();
			if (m_pAudioDec != NULL)
				m_pAudioDec->Flush();
		}
	}
	
	ResetVideoSample(NULL);

	m_bNotifyCompleted = false;

	if (m_pMsgThread != NULL)
		m_pMsgThread->Stop ();

	if (m_pFileSink != NULL)
	{
		m_pFileSink->Stop ();
	}

	m_bIdleStop = true;
	while (m_pIdleThread != NULL)
		voOS_Sleep (10);

	if (m_pVideoLogoInfo)
	{
		m_pVideoLogoInfo->RestStasticPerfoData();
	}

	return VOMP_ERR_None;
}

int voCMediaPlayer::Flush (void)
{
	voCAutoLock lockAudio (&m_mtAudioFlush);
	voCAutoLock lockVideo (&m_mtVideoFlush);

	if (m_pDataSource == NULL)
		return VOMP_ERR_Status;

	if (m_nStatus <= VOMP_STATUS_LOADING)
		return VOMP_ERR_Status;

	VOMP_STATUS oldStatus = m_nStatus;
	if (m_nStatus == VOMP_STATUS_RUNNING)
		Pause ();

	voCAutoLock lock (&m_mtStatus);

	m_pDataSource->Flush ();
	cancelBuffersFromVOVideoDec();

	{
		VOLOGI("flush decoder");
		voCAutoLock lockCodec (&m_mtCodec);
		VOLOGI("flush decoder enter m_mtCodec");
		if (m_pVideoDec != NULL)
			m_pVideoDec->Flush();
		if (m_pAudioDec != NULL)
			m_pAudioDec->Flush();
	}

	if (m_pAudioRender != NULL)
		m_pAudioRender->Flush ();

	m_sVideoDecOutBuf.Time = 0;
	m_llLastVideoDecInTime = 0;
	m_llLastVideoDecOutTime = 0;
	m_sAudioDecOutBuf.Time = 0;
	m_nStartSysTime = 0;
	m_nStartMediaTime = 0;
	m_nRCGetVideoTrackDataAfterPureAudio = VO_ERR_SOURCE_NEEDRETRY;
	m_nVideoRenderNum = 0;
	m_nVideoRndNuming = 0;
	m_nVideoDecoderNum = 0;
	m_bVideoDecInputRetry = VO_FALSE;
	m_bVideoDecHungry = VO_FALSE;
	m_nAudioRenderNum = 0;
	m_nAudioRndNuming = 0;
	m_nAudioRndLength = 0;
	m_nAudioDecOutLength = 0;
	m_nStartPlayTime = 0;
	m_llAudioRenderTime = 0;
	m_llLastAudioRenderTime = 0;
	m_llRenderBufferTime = 0;
	m_llRenderSystemTime = 0;
	m_llDSPRefTime = -1; // set to -1 to pick up the next audio decode time

	if(m_nAudioRenderBufferCount > 1)
	{
		voCAutoLock lockAudioDec(&m_mtAudioRender);
		if(m_pARBManager)
			m_pARBManager->flush();
	}

	m_bSendStartVideoBuff = VO_FALSE;
	m_bSendStartAudioBuff = VO_FALSE;

	m_nStartPlayTime = (VO_S64)voOS_GetSysTime ();
	m_nVideoDcdTimeIndex = 0;
	m_nVideoRndTimeIndex = 0;
	m_nDroppedVidooFrames = 0;
	m_nVideoDateCount = 0;
	m_nVideoCountStartTime = 0;

	m_nVideoOutFillIndex = 0;
	m_nVideoOutPlayIndex = 0;

	if (m_nVideoTrack >= 0)
	{
		m_nVideoEOSArrived = 0;
		m_bVideoEOS = VO_FALSE;
	}
	if (m_nAudioTrack >= 0)
	{
		m_nAudioEOSArrived = 0;
		m_bAudioEOS = VO_FALSE;
	}

	m_bNotifyCompleted = false;

	mAudioDecInputRetry = VO_FALSE;	
	if (oldStatus == VOMP_STATUS_RUNNING)
	{
		Run ();
	}

	return 0;
}

int voCMediaPlayer::GetStatus (VOMP_STATUS * pStatus)
{
	voCAutoLock lock (&m_mtStatus);

	*pStatus = m_nStatus;

	return VOMP_ERR_None;
}

int voCMediaPlayer::GetDuration (int * pDuration)
{
	voCAutoLock lock (&m_mtStatus);

	if (m_nStatus <= VOMP_STATUS_LOADING)
		return VOMP_ERR_Status;

	if (m_nPlayStopPos > m_nPlayStartPos)
		*pDuration = (int)(m_nPlayStopPos - m_nPlayStartPos);
	else
		*pDuration = (int)m_sFilInfo.Duration;

	return VOMP_ERR_None;
}

int voCMediaPlayer::GetCurPos (int * pCurPos)
{
	voCAutoLock lock (&m_mtStatus);

	if (m_nGetThumbnail > 0)
	{
		*pCurPos = (int)m_nSeekPos;
	}
	else
	{
		if (m_nStatus <= VOMP_STATUS_LOADING)
		{
			// VOLOGW ("m_nStatus %d was wrong!", m_nStatus);
			return VOMP_ERR_Status;
		}

		*pCurPos = (int)GetPlayingTime (VO_TRUE);
	}

	if (m_nPlayStopPos > m_nPlayStartPos)
		*pCurPos = *pCurPos - (int)m_nPlayStartPos;

	return VOMP_ERR_None;
}

int voCMediaPlayer::SetCurPos (int nCurPos)
{
	voCAutoLock lockAudio (&m_mtAudioFlush);
	voCAutoLock lockVideo (&m_mtVideoFlush);
	voCAutoLock lockSetpos (&m_mtSetPos);

	VOLOGI ("Set Pos is %d", nCurPos);
	
	if (nCurPos < 0)
		nCurPos = 0;

	preVideoRenderTimeStamp = 0;
	if (m_nStatus <= VOMP_STATUS_LOADING)
		return VOMP_ERR_Status;

	if(m_pFileSource == NULL)
		return VOMP_ERR_Status;

	m_pFileSource->SetSourceParam(VO_PID_SOURCE_SEEKMODE, &m_nSeekMode); 

	if (m_nPlayStopPos > m_nPlayStartPos)
		nCurPos = nCurPos + (int)m_nPlayStartPos;

	VOMP_STATUS oldStatus = m_nStatus;

	if (m_nStatus == VOMP_STATUS_RUNNING)
		Pause ();

	voCAutoLock lock (&m_mtStatus);

	m_nSeekPos = (VO_S64)nCurPos;
	m_sVideoDecOutBuf.Time = 0;
	m_llLastVideoDecInTime = 0;
	m_llLastVideoDecOutTime = 0;
	m_sAudioDecOutBuf.Time = 0;

	cancelBuffersFromVOVideoDec();

	VO_S64	nSeekVideoPos = (VO_S64)nCurPos;
	VO_S64	nSeekAudioPos = (VO_S64)nCurPos;
	VO_U32	nRC = 0;
	if (m_nVideoTrack >= 0)
	{
		nRC = m_pFileSource->SetTrackPos(m_nVideoTrack , &nSeekVideoPos);
		
		if(nRC == VO_ERR_SOURCE_SEEKFAIL)
		{
			VO_S64 nSeekVideoPos0 = 0;
			nRC = m_pFileSource->SetTrackPos(m_nVideoTrack , &nSeekVideoPos0);
		}

		m_sVideoSample.Size = 0;
		m_sVideoSample.Time = nSeekVideoPos;
		
		m_bVideoEOS = VO_FALSE;
		m_nVideoEOSArrived = 0;

		{
			VOLOGI("flush decoder");
			voCAutoLock lockCodec (&m_mtCodec);
			VOLOGI("flush decoder enter m_mtCodec");
			if (m_pVideoDec != NULL)
				m_pVideoDec->Flush();
		}

		if(m_pVideoLogoInfo)
			m_pVideoLogoInfo->Flush();
	}

	if (m_nSeekMode > 0 || (m_nStatus != VOMP_STATUS_RUNNING && m_nStatus != VOMP_STATUS_PAUSED))
	{
		nSeekAudioPos = nSeekVideoPos;
		nCurPos = (int)nSeekVideoPos;
		m_nSeekPos = (int)nSeekVideoPos;

		//VOLOGI ("Audio Pos %d, Video Pos %d", (int)nSeekAudioPos, (int)nSeekVideoPos);
	}


	if (m_nAudioTrack >= 0)
	{
		nRC = m_pFileSource->SetTrackPos(m_nAudioTrack , &nSeekAudioPos);

		if(nRC == VO_ERR_SOURCE_SEEKFAIL)
		{
			VO_S64 nSeekPos0 = 0;
			if (m_nVideoTrack >= 0)
			{
				nRC = m_pFileSource->SetTrackPos(m_nVideoTrack , &nSeekPos0);

				{
					VOLOGI("flush decoder");
					voCAutoLock lockCodec (&m_mtCodec);
					VOLOGI("flush decoder enter m_mtCodec");
					if (m_pVideoDec != NULL)
						m_pVideoDec->Flush();
				}
			}

			nSeekPos0 = 0;			
			nRC = m_pFileSource->SetTrackPos(m_nAudioTrack , &nSeekPos0);
		}

		m_sAudioSample.Size = 0;
		m_sAudioSample.Time = nSeekAudioPos;
		m_nAudioDecOutLength = 0;
		m_nAudioRndLength = 0;
		m_bAudioEOS = VO_FALSE;
		m_nAudioEOSArrived = 0;

		{
			VOLOGI("flush decoder");
			voCAutoLock lockCodec (&m_mtCodec);
			VOLOGI("flush decoder enter m_mtCodec");
			if (m_pAudioDec != NULL)
				m_pAudioDec->Flush();
		}

		m_nSeekPos = (int)nSeekAudioPos;
	}

	if (m_pAudioRender != NULL)
		m_pAudioRender->Flush ();

	if(m_pVideoEffect != NULL)
		m_pVideoEffect->Flush ();

	m_bSeekingPos = VO_TRUE;
	m_bAudioSeekingPos  = VO_TRUE;

	m_bVideoNewTime = VO_FALSE;
	m_bAudioNewTime = VO_FALSE;
	m_nNewTimeBegin = 0;

	m_nVideoRenderAdd = 0;

	if(m_nVideoRenderCount > 1)
	{
		m_pVideoOutputBuff = NULL;
	}

	m_sVideoDecOutBuf.Time = m_nSeekPos;
	m_sAudioDecOutBuf.Time = -1;
	m_nStartSysTime = 0;
	m_nStartMediaTime = m_nSeekPos;
	m_nRCGetVideoTrackDataAfterPureAudio = VO_ERR_SOURCE_NEEDRETRY;
	m_nVideoRenderNum = 0;
	m_nVideoRndNuming = 0;
	m_nVideoDecoderNum = 0;
	m_nAudioRenderNum = 0;
	m_nAudioRndNuming = 0;
	m_llAudioRenderTime = m_nSeekPos;
	m_llLastAudioRenderTime = m_nSeekPos;
	m_llRenderBufferTime = 0;
	m_llRenderSystemTime = 0;
	m_llDSPRefTime = -1; // set to -1 to pick up the next audio decode time

	if(m_nAudioRenderBufferCount > 1)
	{
		voCAutoLock lockAudioDec(&m_mtAudioRender);
		if(m_pARBManager)
			m_pARBManager->flush();
	}

	m_bSendStartVideoBuff = VO_FALSE;
	m_bSendStartAudioBuff = VO_FALSE;

	m_nStartPlayTime = (VO_S64)voOS_GetSysTime () - m_nSeekPos;
	m_nVideoDcdTimeIndex = 0;
	m_nVideoRndTimeIndex = 0;
	m_nDroppedVidooFrames = 0;

	m_nVideoOutFillIndex = 0;
	m_nVideoOutPlayIndex = 0;

	m_bNotifyCompleted = false;

	m_nStatusRenderFlag = 1;

	if (oldStatus == VOMP_STATUS_RUNNING)
	{
		Run ();
	}
	else if (oldStatus == VOMP_STATUS_PAUSED)
	{
		// pause -> run
		if(m_nVideoTrack >= 0)
		{
			if (m_pVideoDecThread != NULL)
			{
				m_pVideoDecThread->Start (m_fThreadCreate);
				postVideoDecoderEvent (-1);
			}
			if (m_pVideoThread != NULL)
			{
				m_pVideoThread->Start (m_fThreadCreate);
				postVideoRenderEvent (-1);
			}
		}

		if(m_nAudioTrack >= 0)
		{
			if (m_pAudioDecThread != NULL)
			{
				m_pAudioDecThread->Start (m_fThreadCreate);
				postAudioDecoderEvent (-1);
			}
		}
	}

	return VOMP_ERR_None;
}

int voCMediaPlayer::checkAudioEffectFormat()
{
	if (m_pAudioEffect)
	{
		VO_AUDIO_FORMAT	fmtAudio;
		m_pAudioEffect->GetFormat (&fmtAudio);
		if(fmtAudio.SampleRate != m_sAudioEftOutFormat.SampleRate || fmtAudio.Channels != m_sAudioEftOutFormat.Channels || 
			fmtAudio.SampleBits != m_sAudioEftOutFormat.SampleBits)
		{
			memcpy(&m_sAudioEftOutFormat, &fmtAudio, sizeof(VO_AUDIO_FORMAT));

			VOLOGI ("The audio format was changed to SampleRate %d, Channels %d, Bits %d", 
				fmtAudio.SampleRate, fmtAudio.Channels, fmtAudio.SampleBits);

			onAudioOutFormatChanged(&m_sAudioEftOutFormat);

			return VOMP_ERR_FormatChange;
		}
	}

	return VOMP_ERR_None;
}

void voCMediaPlayer::onAudioOutFormatChanged(VO_AUDIO_FORMAT * pOutFormat)
{
	UpdateAudioFormat(pOutFormat);

	if(m_pAudioSpeed != NULL && m_bASpeedEnable)
		m_pAudioSpeed->SetFormat(pOutFormat);

	if (m_pAudioRender != NULL)
		m_pAudioRender->SetFormat(pOutFormat);
}

void voCMediaPlayer::onAudioDecOutFormatChanged(VO_AUDIO_FORMAT * pDecOutFormat)
{
	VO_AUDIO_FORMAT * pAudioOutFormat = pDecOutFormat;
	if(m_pAudioEffect)
	{
		// reset audio effect and use its out format
		m_pAudioEffect->SetFormat (pDecOutFormat);
		m_pAudioEffect->Enable (m_bEffectEnable);
		m_pAudioEffect->GetFormat (&m_sAudioEftOutFormat);

		pAudioOutFormat = &m_sAudioEftOutFormat;
	}

	VOLOGI ("The audio format was changed to SampleRate %d, Channels %d, Bits %d", 
		pAudioOutFormat->SampleRate, pAudioOutFormat->Channels, pAudioOutFormat->SampleBits);

	onAudioOutFormatChanged(pAudioOutFormat);
}

int voCMediaPlayer::doAudioRender(VO_PBYTE pPCMdata, VO_U32 nPCMLength, VO_S64 llTimeStamp, VOMP_BUFFERTYPE ** ppBuffer)
{
	if (llTimeStamp + (VO_S64)m_nAudioStepTime * 2 + (VO_S64)m_nAudioBuffTime < m_nSeekPos)
		return VOMP_ERR_Retry;

	effectAudioSample_Volume(pPCMdata, nPCMLength);

	m_nAudioRndLength = nPCMLength;

	VO_U32 nRC = VO_ERR_NONE;
	VOMP_BUFFERTYPE * pBuffer = *ppBuffer;
	if(m_pAudioEffect)
	{
		if (pBuffer != NULL && pBuffer->pBuffer != NULL)
		{
			if (pBuffer->nSize < (int)m_nAudioDecOutStepSize)
				return VOMP_ERR_SmallSize;

			m_pAudioRndBuff = pBuffer->pBuffer;
			m_nAudioRndSize = pBuffer->nSize;
		}
		else
		{
			if(m_pAudioEftOutPcmBuff == NULL)
			{
				m_pAudioEftOutPcmBuff = new VO_BYTE[m_nAudioEftOutFrameSize];
				if(!m_pAudioEftOutPcmBuff)
					return VOMP_ERR_OutMemory;
			}
			m_pAudioRndBuff = m_pAudioEftOutPcmBuff;
			m_nAudioRndSize = m_nAudioEftOutFrameSize;
		}

		nRC = effectAudioSample_Dolby(pPCMdata, nPCMLength);
		if(VOMP_ERR_FormatChange == nRC)
			return nRC;
	}
	else
		m_pAudioRndBuff = pPCMdata;

	if (m_pAudioSpeed != NULL && m_bASpeedEnable)
	{
		VO_U8 *pTempBuf = new VO_U8[m_nAudioRndLength];
		memcpy(pTempBuf, m_pAudioRndBuff, m_nAudioRndLength);

		VO_U32 nNewFilledLen = m_nAudioRndLength;
		VOLOGI("@@@####rndAs m_pAudioSpeed->Convert ====> m_nAudioPlaySpeed: %d", m_nAudioPlaySpeed);
		nRC = m_pAudioSpeed->Convert(pTempBuf, m_nAudioRndLength, m_pAudioRndBuff, &nNewFilledLen);
		//VOLOGI("@@@####rndAs m_pAudioSpeed->Convert <====");

		delete [] pTempBuf;

		if (VO_ERR_NONE == nRC && nNewFilledLen > 0)
		{
			VOLOGI("@@@####rndAs m_pAudioSpeed->Convert <==== nRC: OK m_nAudioRndLength: %d nNewFilledLen: %d", m_nAudioRndLength, nNewFilledLen);
			m_nAudioRndLength = nNewFilledLen;
		}
		else
		{
			VOLOGI("@@@####rndAs m_pAudioSpeed->Convert <==== nRC: 0x%08X nNewFilledLen: %d", nRC, nNewFilledLen);
			m_nAudioRndLength = 0;
			return VOMP_ERR_Retry;
		}
	}

	if (m_llDSPRefTime == -1)
	{
		// Get DSP audio clock reference time
		m_llDSPRefTime = llTimeStamp;
		m_llDSPTimeOffset = 0;
		m_llDSPEOSSystemStartTime = 0;
		m_llDSPLastAudioRenderTime = -1;
		m_llDSPLastAudioRenderDuration = 0;
		VOLOGI("@@@####rndAs m_llDSPRefTime is %d",(int)m_llDSPRefTime);
	}

	// set the audio parameters
	{
		voCAutoLock lock (&m_mtPlayTime);
		m_nStartSysTime = 0;
		if (m_llAudioRenderTime < llTimeStamp || m_llAudioRenderTime > llTimeStamp + 15000 || m_bAudioNewTime)
		{
			m_llAudioRenderTime = llTimeStamp;
		}

		m_nLastAudioSysTime = (VO_U64)voOS_GetSysTime ();
		m_nAudioRenderNum++;
		m_nAudioRndNuming++;
	}

	// if current render time is close to last render time, we regard time jump finish
	if(abs((int)llTimeStamp - (int)m_llLastAudioRenderTime) < (int)m_nAudioStepTime * 2)
	{
		voCAutoLock lock(&m_mtNewTime);
		if(m_bAudioNewTime)
		{
			VOLOGI("m_nNewTimeBegin %d, m_bOnlyGetAudio %d", (int)m_nNewTimeBegin, m_bOnlyGetAudio);

			m_bAudioNewTime = VO_FALSE;

			if(m_nNewTimeBegin > 0)
				m_nNewTimeBegin--;

			if(m_bOnlyGetAudio && m_nNewTimeBegin > 0)
				m_nNewTimeBegin--;

			if(m_nNewTimeBegin < 0)
				m_nNewTimeBegin = 0;
		}
	}

	m_llLastAudioRenderTime = llTimeStamp;

	if (m_bSeekingPos && (m_nVideoTrack < 0 || m_bVideoEOS || m_bOnlyGetAudio))
	{
		m_bSeekingPos = VO_FALSE;
		postCheckStatusEvent (VOMP_CB_SeekComplete);
	}

	if (ppBuffer == NULL)
	{
		RenderAudioSample ();
	}
	else
	{
		if (!m_bRenderAudioOnOff)
			memset (m_pAudioRndBuff, 0 , m_nAudioRndLength);

		if (pBuffer == NULL)
		{
			m_bufAudio.pBuffer = m_pAudioRndBuff;
			m_bufAudio.nSize = m_nAudioRndLength;
			m_bufAudio.llTime = m_llAudioRenderTime;
			if (m_nPlayStopPos > m_nPlayStartPos)
				m_bufAudio.llTime = m_bufAudio.llTime - m_nPlayStartPos;
			*ppBuffer = (VOMP_BUFFERTYPE *)&m_bufAudio;
		}
		else
		{
			pBuffer->nSize = m_nAudioRndLength;
			pBuffer->llTime = m_llAudioRenderTime;
			if (m_nPlayStopPos > m_nPlayStartPos)
				pBuffer->llTime = pBuffer->llTime - m_nPlayStartPos;
		}

		if (mADSPClock != NULL || m_pVideoLogoInfo) 
		{
			VO_AUDIO_FORMAT * pAudioOutFormat = m_pAudioEffect ? &m_sAudioEftOutFormat : &m_sAudioDecOutFormat;
			double nDuration = (double)(m_nAudioRndLength * 8000) /(double) (pAudioOutFormat->Channels * pAudioOutFormat->SampleBits * pAudioOutFormat->SampleRate);
			if (mADSPClock && m_sAudioTrackInfo.Codec != VO_AUDIO_CodingAC3 && m_sAudioTrackInfo.Codec != VO_AUDIO_CodingEAC3)
			{
				if (m_llDSPLastAudioRenderTime != -1)
				{
					int diff = (int)(m_llAudioRenderTime - m_llDSPLastAudioRenderTime - m_llDSPLastAudioRenderDuration);
					if (diff > 10)
					{
						// NOTE: hard code to 10 ms for now
						VOLOGI ("Audio Timestamp discontinuity: LastTimeStamp %d Duration %d CurrTimeStamp %d Diff %d", (int)m_llDSPLastAudioRenderTime, (int)m_llDSPLastAudioRenderDuration, (int)m_llAudioRenderTime, diff);
						m_llDSPTimeOffset += diff;
					}
				}
				m_llDSPLastAudioRenderTime = m_llAudioRenderTime;
				m_llDSPLastAudioRenderDuration = (int)nDuration;
			}
			if (m_pVideoLogoInfo)
				m_pVideoLogoInfo->m_nStatisPlaybackDurationTime += nDuration;
		}
	}

	if ((m_nShowLogLevel & 0X02) == 2)
	{
		if (m_nLastAudioFrmTime == 0)
			m_nLastAudioFrmTime = m_llAudioRenderTime;
		if (m_nLastAudioRndTime == 0)
			m_nLastAudioRndTime = (VO_S64)voOS_GetSysTime ();

		VOLOGI ("A% 6d  P% 6d, S% 6d Frm% 4d  S-A% 4d  S-P% 4d Rnd% 4d, Diff% 5d, Size % 5d", (int)m_llAudioRenderTime, GetPlayingTime (), (int)(voOS_GetSysTime () - m_nStartPlayTime), (int)(m_llAudioRenderTime - m_nLastAudioFrmTime), 
			(int)(voOS_GetSysTime () - m_nStartPlayTime - (int)m_llAudioRenderTime), (int)(voOS_GetSysTime () - m_nStartPlayTime - GetPlayingTime ()), (int)(voOS_GetSysTime () - m_nLastAudioRndTime), (int)(voOS_GetSysTime () - m_nLastAudioRndTime - m_nLastAudioRndDifTime), (int)m_nAudioRndLength);

		m_nLastAudioFrmTime = m_llAudioRenderTime;
		m_nLastAudioRndDifTime = (VO_S64)voOS_GetSysTime () - m_nLastAudioRndTime;
		m_nLastAudioRndTime = (VO_S64)voOS_GetSysTime ();
	}

	m_bAudioSeekingPos = VO_FALSE;

#ifdef _VOLOG_INFO
	static int audiorendertime = 0;
	VOLOGI ("@@@####rndA Audio Render TimeStamp is %d, Size: %d, PlayTime: %d, System Time: %d, Diff: %d",(int)m_llAudioRenderTime, m_nAudioRndLength, (int)GetPlayingTime(), voOS_GetSysTime (), voOS_GetSysTime () - audiorendertime);	
	audiorendertime = voOS_GetSysTime ();
#endif	

	return VOMP_ERR_None;
}

int voCMediaPlayer::DoAudioRender (VOMP_BUFFERTYPE ** ppBuffer)
{
	if (!m_bSetAudioRendererThreadPriority)
	{
		m_bSetAudioRendererThreadPriority = true;
#ifdef _LINUX_ANDROID
//		setpriority(PRIO_PROCESS, 0, ANDROID_PRIORITY_AUDIO);
#endif //_LINUX_ANDROID
#ifdef _WIN32
		voThreadSetPriority (GetCurrentThread (), VO_THREAD_PRIORITY_ABOVE_NORMAL);
#endif // _WIN32
#ifdef _IOS
//		voThreadSetPriority((void *)voThreadGetCurrentID(), VO_THREAD_PRIORITY_ABOVE_NORMAL);
#endif
	}

	if (m_bAudioEOS)
	{
		voOS_Sleep (10);
		return VOMP_ERR_EOS;
	}

	if (m_nPlayStopPos > m_nPlayStartPos)
	{
		if (m_sAudioSample.Time >= (VO_S64)m_nPlayStopPos)
		{
			m_bAudioEOS = VO_TRUE;
			if (mADSPClock != NULL)
			{
				// switch to use system time as reference clock
				VOLOGI ("Switching reference time from DSP clock to system clock");
				m_llDSPRefTime = m_nPlayingTime;
				m_llDSPEOSSystemStartTime =(VO_S64)voOS_GetSysTime();
			}
			return VOMP_ERR_EOS;
		}
	}

	int nRC = 0;
	VOMP_BUFFERTYPE * pBuffer = *ppBuffer;
	if(m_nAudioRenderBufferCount > 1)
	{
		VO_PBYTE pAudioDecOutBuff = NULL;
		if(pBuffer != NULL && pBuffer->pBuffer != NULL)
		{
			if (pBuffer->nSize < (int)m_nAudioDecOutStepSize)
				return VOMP_ERR_SmallSize;

			pAudioDecOutBuff = pBuffer->pBuffer;
		}
		else
		{
			if(m_pAudioDecOutPcmBuff == NULL)
			{
				m_pAudioDecOutPcmBuff = new VO_BYTE[m_nAudioDecOutFrameSize];
				if(!m_pAudioDecOutPcmBuff)
					return VOMP_ERR_OutMemory;
			}
			pAudioDecOutBuff = m_pAudioDecOutPcmBuff;
		}

		VO_U32 nAudioDecOutLength = 0;
		VO_S64 llTimeStamp = 0;
		VO_AUDIO_FORMAT fmtAudio;
		memcpy(&fmtAudio, &m_sARBFormat, sizeof(VO_AUDIO_FORMAT));
		{
			voCAutoLock lockAudioDec(&m_mtAudioRender);
			if(!m_pARBManager)
				return VOMP_ERR_Retry;

			nRC = m_pARBManager->getPCMData(pAudioDecOutBuff, -1, &nAudioDecOutLength, &llTimeStamp, &fmtAudio);
		}

		if(VO_ERR_RETRY == nRC)
		{
			if(m_nAudioEOSArrived == 2)
			{
				VOLOGI ("Set m_bAudioEOS True %d, video %d", m_bAudioEOS, m_bVideoEOS);

				voCAutoLock lockSetpos (&m_mtSetPos);
				m_bAudioEOS = VO_TRUE;

				if (mADSPClock != NULL)
				{ 
					// switch to use system time as reference clock
					VOLOGI ("Switching reference time from DSP clock to system clock");

					m_llDSPRefTime = m_nPlayingTime;
					m_llDSPEOSSystemStartTime =(VO_S64)voOS_GetSysTime();
				}

				if (m_bVideoEOS)
					postCheckStatusEvent (VOMP_CB_PlayComplete);

				return VOMP_ERR_EOS;
			}

			return VOMP_ERR_Retry;
		}
		else if(VO_ERR_FINISH == nRC)
		{
			// format changed detected
			memcpy(&m_sARBFormat, &fmtAudio, sizeof(VO_AUDIO_FORMAT));

			onAudioDecOutFormatChanged(&m_sARBFormat);

			return VOMP_ERR_FormatChange;
		}
		else if(VO_ERR_NONE != nRC)
			return nRC;

		nRC = checkAudioEffectFormat();
		if(VOMP_ERR_None != nRC)
			return nRC;

		return doAudioRender(pAudioDecOutBuff, nAudioDecOutLength, llTimeStamp, ppBuffer);
	}
	else
	{
		if(pBuffer != NULL && pBuffer->pBuffer != NULL)
		{
			if (pBuffer->nSize < (int)m_nAudioDecOutStepSize)
				return VOMP_ERR_SmallSize;

			m_pAudioDecOutBuff = pBuffer->pBuffer;
			m_nAudioDecOutSize = pBuffer->nSize;
		}
		else
		{
			if(m_pAudioDecOutPcmBuff == NULL)
			{
				m_pAudioDecOutPcmBuff = new VO_BYTE[m_nAudioDecOutFrameSize];
				if(!m_pAudioDecOutPcmBuff)
					return VOMP_ERR_OutMemory;
			}
			m_pAudioDecOutBuff = m_pAudioDecOutPcmBuff;
			m_nAudioDecOutSize = m_nAudioDecOutFrameSize;
		}

		nRC = ReadAudioSample ();
		if (nRC == VOMP_ERR_EOS)
		{
			VOLOGI ("Set m_bAudioEOS True %d, video %d", m_bAudioEOS, m_bVideoEOS);

			voCAutoLock lockSetpos (&m_mtSetPos);
			m_bAudioEOS = VO_TRUE;

			if (mADSPClock != NULL)
			{ 
				// switch to use system time as reference clock
				VOLOGI ("Switching reference time from DSP clock to system clock");

				m_llDSPRefTime = m_nPlayingTime;
				m_llDSPEOSSystemStartTime =(VO_S64)voOS_GetSysTime();
			}

			if (m_bVideoEOS)
				postCheckStatusEvent (VOMP_CB_PlayComplete);
		}
		else if (nRC == VOMP_ERR_FormatChange)
		{
			m_nAudioDecOutLength = 0;

			onAudioDecOutFormatChanged(&m_sAudioDecOutFormat);
		}

		if (nRC != VOMP_ERR_None)
			return nRC;

		if (m_nAudioDecOutLength < m_nAudioDecOutStepSize)
			return VOMP_ERR_Retry;

		nRC = checkAudioEffectFormat();
		if(VOMP_ERR_None != nRC)
			return nRC;

		return doAudioRender(m_pAudioDecOutBuff, m_nAudioDecOutLength, m_sAudioDecOutBuf.Time, ppBuffer);
	}
}

int voCMediaPlayer::DoVideoRender (VOMP_BUFFERTYPE ** ppBuffer)
{
	if (m_pVideoLogoInfo)
	{
		m_pVideoLogoInfo->m_nVideoRenderEndTime = voOS_GetSysTime();
		if (m_pVideoLogoInfo->m_nVideoRenderStartTime > 0)
		{
			int nTime = m_pVideoLogoInfo->m_nVideoRenderEndTime - m_pVideoLogoInfo->m_nVideoRenderStartTime;
			//m_pVideoLogoInfo->m_nSumRenderTime += nTime;

			//if (m_pVideoLogoInfo->m_nMaxRenderTime < nTime)
			//	m_pVideoLogoInfo->m_nMaxRenderTime = nTime;

			m_pVideoLogoInfo->m_nVideoRenderStartTime = 0;
		}
	}
	
	if (m_nVideoRndLTime > 0)
	{
		m_nVideoRndTimes = m_nVideoRndTimes + ((VO_U64)voOS_GetSysTime () - m_nVideoRndLTime);
		m_nVideoRndLTime = 0;
	}

	if(m_nVideoRenderCount > 1 && m_nRenderSuccess > 0)
	{
        ResetVideoSample(NULL);

		voCAutoLock lockVideoDec (&m_mtVideoRender);
		if(m_nVideoOutPlayIndex < m_nVideoOutFillIndex)
		{
			if(m_pVideoOutputBuff)
				cancelBufferFromVideoDec(m_pVideoOutputBuff);

			m_nVideoOutPlayIndex++;
		}
		m_nRenderSuccess = 0;
	}

	VOMP_BUFFERTYPE * pBuffer = NULL;
	if(ppBuffer != NULL && *ppBuffer != NULL)
	{
		pBuffer = *ppBuffer;

		// seek by GetVideoBuffer??
		if(pBuffer->llTime > 0 && m_outVideoBuffer.llTime != pBuffer->llTime)
			SetCurPos((int)pBuffer->llTime);

		m_outVideoBuffer.llTime = pBuffer->llTime;
	}

	if (m_nVideoRenderCount > 1 && m_nVideoOutPlayIndex  == m_nVideoOutFillIndex)
	{
		if(m_nVideoEOSArrived == 2)
		{
			voCAutoLock lockSetpos (&m_mtSetPos);
			if (!m_bVideoEOS)
			{
				VOLOGI ("Read at end of track!");
				ShowResult ();
			}

			m_bVideoEOS = VO_TRUE;

			if(m_bOnlyGetVideo == VO_TRUE)
				m_bAudioEOS = VO_TRUE;

			// added by David to ensure player stop correctly if audio ends up firstly
			if(m_bAudioEOS)
				postCheckStatusEvent (VOMP_CB_PlayComplete);

			voOS_Sleep (10);
			return VOMP_ERR_EOS;
		}
	}

	if (m_nPlayStopPos > m_nPlayStartPos)
	{
		if (m_pVideoOutputBuff != NULL && m_pVideoOutputBuff->Time >= (VO_S64)m_nPlayStopPos)
		{
			m_bVideoEOS = VO_TRUE;
			if (m_bSeekingPos)
			{
				m_bSeekingPos = VO_FALSE;
				postCheckStatusEvent (VOMP_CB_SeekComplete);
			}
			if (m_bAudioEOS)
				postCheckStatusEvent (VOMP_CB_PlayComplete);

			return VOMP_ERR_EOS;
		}
	}

	int nRC = 0;
	if (m_nCheckFastPerformance == 0 && m_nGetThumbnail == 0 && !m_nVideoRenderAdd)// && m_nVideoRenderNum > 0)
	{
		nRC = CheckVideoRenderTime ();
		if (nRC != VOMP_ERR_None)
		{
			if (nRC == VOMP_ERR_Retry)	// video is overtime, we need drop it
			{
				if(m_pVideoLogoInfo && m_pVideoOutputBuff)
					m_pVideoLogoInfo->RenderSourceStart(m_pVideoOutputBuff, 0);

				if (m_nVideoRenderCount == 1)
				{
					cancelBufferFromVideoDec(m_pVideoOutputBuff);

					nRC = ReadVideoSample ();
					if(nRC && m_nVideoEOSArrived == 2)
					{
						voCAutoLock lockSetpos (&m_mtSetPos);
						if (!m_bVideoEOS)
						{
							VOLOGI ("Read at end of track!");
							ShowResult ();
						}

						m_bVideoEOS = VO_TRUE;

						if(m_bOnlyGetVideo == VO_TRUE)
							m_bAudioEOS = VO_TRUE;

						// added by David to ensure player stop correctly if audio ends up firstly
						if (m_bAudioEOS)
							postCheckStatusEvent (VOMP_CB_PlayComplete);

						return VOMP_ERR_EOS;
					}

					cancelBufferFromVideoDec(m_pVideoOutputBuff);
					
#if defined __VOPRJ_MOTOPASTEUR__
					return nRC;
#endif
				}
				else
				{
					voCAutoLock lockVideoDec (&m_mtVideoRender);
					if (m_nVideoOutPlayIndex >= m_nVideoOutFillIndex)
						return VOMP_ERR_Retry;

					VO_VIDEO_BUFFER * pVideoBuffer = m_ppVideoOutBuffers[m_nVideoOutPlayIndex % m_nVideoRenderCount];
					if (pVideoBuffer == NULL)
						return VOMP_ERR_Retry;

					m_pVideoOutputBuff = pVideoBuffer;
					cancelBufferFromVideoDec(m_pVideoOutputBuff);
					m_nVideoOutPlayIndex++;

					if(m_pVideoLogoInfo)
					{
						//m_pVideoLogoInfo->m_nStatisTotalRenderDropFrame ++;
						//m_pVideoLogoInfo->m_nRnnderDropFrame ++;
						if (m_nAudioTrack < 0 || m_bOnlyGetVideo)
						{
							m_pVideoLogoInfo->m_nStatisPlaybackDurationTime += m_pVideoLogoInfo->m_nFrameRateValue;
						}
					}
				}

				return VOMP_ERR_Retry;				
			}
			else if( nRC == VOMP_ERR_Pointer)
			{
				if (m_nVideoRenderCount > 1)
				{
					voCAutoLock lockVideoDec (&m_mtVideoRender);
					if (m_nVideoOutPlayIndex >= m_nVideoOutFillIndex)
						return VOMP_ERR_Retry;

					VO_VIDEO_BUFFER * pVideoBuffer = m_ppVideoOutBuffers[m_nVideoOutPlayIndex % m_nVideoRenderCount];
					if (pVideoBuffer == NULL)
						return VOMP_ERR_Retry;

					m_pVideoOutputBuff = pVideoBuffer;
				}
			}
			else
			{
				return nRC;
			}
		}
	}

	VO_VIDEO_BUFFER	* pVideoOutputBuff = m_pVideoOutputBuff;
	VO_U32	nVideoOutPlayIndex = m_nVideoOutPlayIndex;


	if (m_nVideoRenderCount == 1)
	{
		nRC = ReadVideoSample ();

		if(nRC && m_nVideoEOSArrived == 2)
		{
			voCAutoLock lockSetpos (&m_mtSetPos);
			if (!m_bVideoEOS)
			{
				VOLOGI ("Read at end of track!");
				ShowResult ();
			}

			m_bVideoEOS = VO_TRUE;

			if(m_bOnlyGetVideo == VO_TRUE)
				m_bAudioEOS = VO_TRUE;

			// added by David to ensure player stop correctly if audio ends up firstly
			if (m_bAudioEOS)
				postCheckStatusEvent (VOMP_CB_PlayComplete);

			return VOMP_ERR_EOS;
		}
	}
	else
	{
		//if (m_nVideoRenderNum == 0)
		//{
		//	if (m_nVideoOutFillIndex + 1 < m_nVideoRenderCount && !m_nLowLatence)
		//	{
		//		//VOLOGI ("Fill %d, Render %d", m_nVideoOutFillIndex, m_nVideoRenderCount);
		//		voOS_Sleep (5);
		//		return VOMP_ERR_Retry;
		//	}
		//}

		if (m_nVideoOutPlayIndex >= m_nVideoOutFillIndex)
		{
			voOS_Sleep(2);
			return VOMP_ERR_Retry;
		}

		voCAutoLock lockVideoDec (&m_mtVideoRender);
		if (m_ppVideoOutBuffers == NULL)
			return VOMP_ERR_Retry;

		if(!m_nVideoPlayBack && !m_nVideoRenderAdd)
		{
			if(m_pVideoOutputBuff)
				cancelBufferFromVideoDec(m_pVideoOutputBuff);

			m_nVideoOutPlayIndex++; //jump decoder out frame
			return VOMP_ERR_Retry;
		}

		nVideoOutPlayIndex = m_nVideoOutPlayIndex;
		VO_U32 nIndex = nVideoOutPlayIndex % m_nVideoRenderCount;
		VO_VIDEO_BUFFER * pVideoBuffer = m_ppVideoOutBuffers[nIndex];
		if (pVideoBuffer == NULL)
			return VOMP_ERR_Retry;

		if (m_pVideoOutWidth[nIndex] != m_nVideoRndWidth || m_pVideoOutHeight[nIndex] != m_nVideoRndHeight)
		{
			m_nVideoRndWidth = m_pVideoOutWidth[nIndex];
			m_nVideoRndHeight = m_pVideoOutHeight[nIndex];
			nRC = VOMP_ERR_FormatChange;
		}
		else
		{
			m_pVideoOutputBuff = pVideoBuffer;
			nVideoOutPlayIndex++;
		}

		if (m_bVideoEOS  && nVideoOutPlayIndex >= m_nVideoOutFillIndex)
		{
			m_nVideoOutPlayIndex = nVideoOutPlayIndex;
			nRC = VOMP_ERR_EOS;
		}
	}

	if (nRC == VOMP_ERR_FormatChange)
	{
		cancelBufferFromVideoDec(m_pVideoOutputBuff);
		
		if (m_pVideoRender != NULL)
		{
			if (!m_bSetDispRect)
			{
				m_rcDraw.left = 0; m_rcDraw.right = m_nVideoRndWidth;
				m_rcDraw.top = 0; m_rcDraw.bottom = m_nVideoRndHeight;
			}

			m_rgbBuffer.Stride[0] = 0;

#ifdef _IOS
			VO_RECT r = {m_rcDraw.left, m_rcDraw.top, m_rcDraw.right, m_rcDraw.bottom};
			m_pVideoRender->SetDispRect (m_hView, &r, (VO_IV_COLORTYPE)m_nColorType);
			m_pVideoRender->SetVideoInfo (m_nVideoRndWidth, m_nVideoRndHeight, VO_COLOR_YUV_PLANAR420);
#else
			m_pVideoRender->SetVideoInfo (m_nVideoRndWidth, m_nVideoRndHeight, VO_COLOR_YUV_PLANAR420);
			m_pVideoRender->SetDispRect (m_hView, (VO_RECT *)&m_rcDraw, (VO_IV_COLORTYPE)m_nColorType);
			m_pVideoRender->SetDispType(m_nZoomType, VO_RATIO_00);
#endif
		}

#ifdef _LINUX_ANDROID
		if(m_nColorType == VOMP_COLOR_YUV_PLANAR420 || m_nColorType == VOMP_COLOR_YUV_420_PACK || m_nColorType == VOMP_COLOR_YUV_PLANAR420_NV12)
		{
			if(!m_bSetDispRect)
			{
				m_rcDraw.left = 0;
				m_rcDraw.right = m_nVideoRndWidth;
				m_rcDraw.top = 0;
				m_rcDraw.bottom = m_nVideoRndHeight;
			}
		}
#endif	// _LINUX_ANDROID

		return nRC;
	}
	else if (nRC != VOMP_ERR_None)
	{
		if (nRC == VOMP_ERR_EOS)
		{
			if (m_bSeekingPos)
			{
				m_bSeekingPos = VO_FALSE;
				postCheckStatusEvent (VOMP_CB_SeekComplete);
			}

			if (m_bAudioEOS)
				postCheckStatusEvent (VOMP_CB_PlayComplete);
		}

		return nRC;
	}

	if (m_nGetThumbnail == 0)
	{		
		VO_BOOL bCanDropFrame = (m_nAudioTrack > 0 && !m_bOnlyGetVideo && !m_bAudioEOS && (m_nAudioRndNuming > 2 && m_nVideoRndNuming > 0 || m_nStatusRenderFlag == 1)) ? VO_TRUE : VO_FALSE;
		//VOLOGI("@@@####rndV m_nStatusRenderFlag: %d  bCanDropFrame: %d", m_nStatusRenderFlag, (int)bCanDropFrame);
		VO_S64	nPlayTime = (VO_S64)GetPlayingTime ();
		if (m_nVideoDropRender > 0 && nPlayTime > 0 && !m_nVideoRenderAdd && !m_nNewTimeBegin)
		{
			VO_S64 nTolTime = 100;
			if (m_nAudioPlaySpeed > 10)
			{
				nTolTime *= (VO_S64)(m_nAudioPlaySpeed/10);
				VOLOGI("@@@####rndV m_nAudioPlaySpeed: %d	nTolTime: %lld", m_nAudioPlaySpeed, nTolTime);
			}
			
#if defined(_IOS) || defined(_MAC_OS)
			if (nPlayTime > m_pVideoOutputBuff->Time + nTolTime && m_bAudioBufFull)
#else
			if (nPlayTime > m_pVideoOutputBuff->Time + nTolTime)
#endif			
			{
				if (!m_bFrameDropped && VO_TRUE == bCanDropFrame)
				{
					m_bFrameDropped = VO_TRUE;
					m_nDroppedVidooFrames++;

					VOLOGI("@@@####rndV Drop video frame: video delayed..., playing time = %d, video dec time = %d, diff = %d", (int)nPlayTime, (int)(m_pVideoOutputBuff->Time), (int)(m_pVideoOutputBuff->Time - nPlayTime));

					cancelBufferFromVideoDec(m_pVideoOutputBuff);
					
					if(m_pVideoLogoInfo)
						m_pVideoLogoInfo->RenderSourceStart(m_pVideoOutputBuff, 0);

					voCAutoLock lockVideoDec (&m_mtVideoRender);
					m_nVideoOutPlayIndex = nVideoOutPlayIndex;

					if(m_pVideoLogoInfo)
					{
						//m_pVideoLogoInfo->m_nStatisTotalRenderDropFrame ++;
						m_pVideoLogoInfo->m_nRnnderDropFrame ++;
						if (m_nAudioTrack < 0 || m_bOnlyGetVideo)
						{
							m_pVideoLogoInfo->m_nStatisPlaybackDurationTime += m_pVideoLogoInfo->m_nFrameRateValue;
						}
					}

					return VOMP_ERR_OutOfTime;
				}
			}
		}
	}
	m_bFrameDropped = VO_FALSE;

	if (m_nVideoRenderNum > 1) //can drop at least 2 frame after seek
	{		
		m_nStatusRenderFlag = 0;
	}
	
	if (m_bSendStartVideoBuff)
	{
		m_bSendStartVideoBuff = VO_FALSE;
		postCheckStatusEvent (VOMP_CB_VideoStopBuff);
	}

	m_aVideoRndTime[m_nVideoRndTimeIndex] = m_pVideoOutputBuff->Time;
	if (m_nVideoRndTimeIndex > 1000)
	{
		int nFirstFrame = m_nVideoRndTimeIndex;
		while (m_aVideoRndTime[m_nVideoRndTimeIndex] - m_aVideoRndTime[nFirstFrame] < 5000)
		{
			nFirstFrame--;
			if (nFirstFrame <= 0)
				break;
		}

		memcpy (&m_aVideoRndTime[0], &m_aVideoRndTime[nFirstFrame], (m_nVideoRndTimeIndex - nFirstFrame) * sizeof (VO_S64));
		m_nVideoRndTimeIndex = m_nVideoRndTimeIndex - nFirstFrame;
	}
	else
	{
		m_nVideoRndTimeIndex++;
	}

	if(m_pVideoLogoInfo)
	{
		if(m_nVideoRenderCount == 1)
			m_pVideoLogoInfo->RenderSourceStart(m_pVideoOutputBuff, m_nVideoRenderCount);
		else
			m_pVideoLogoInfo->RenderSourceStart(m_pVideoOutputBuff, m_nVideoOutFillIndex - m_nVideoOutPlayIndex);
	}

	if (ppBuffer == NULL)
	{
		nRC = RenderVideoSample ();
	}
	else
	{
		if (pBuffer == NULL)
		{
			memset (&m_outVideoBuffer, 0, sizeof (m_outVideoBuffer));
			*ppBuffer = &m_outVideoBuffer;
			pBuffer = &m_outVideoBuffer;
		}

		if (m_nColorType == VOMP_COLOR_YUV_PLANAR420 || m_nColorType == VOMP_COLOR_YUV_420_PACK || m_nColorType == VOMP_COLOR_YUV_PLANAR420_NV12)
		{
			if (pBuffer != NULL)
			{
				if(pBuffer->nFlag & VOMP_FLAG_BUFFER_TRY_ONLY)
				{
					m_pVideoOutputBuff = pVideoOutputBuff;
					m_nVideoRenderAdd = 1;
					return VOMP_ERR_None;
				}
			}

			m_nVideoRenderAdd = 0;
#ifdef _LINUX_ANDROID
			if(m_nVideoRenderNum == 0 && !m_bSetDispRect)
			{
				m_rcDraw.left = 0;
				m_rcDraw.right = m_nVideoRndWidth;
				m_rcDraw.top = 0;
				m_rcDraw.bottom = m_nVideoRndHeight;
			}

//			VOLOGI("GetVideoData %d %d %d %d W %d H %d", m_rcDraw.left, m_rcDraw.right, m_rcDraw.top, m_rcDraw.bottom, m_nVideoRndWidth, m_nVideoRndHeight);

			if(pBuffer->nFlag & VOMP_FLAG_BUFFER_YUVBUFFER)
			{
				VOMP_RECT rcDraw;
				if(VOMP_ERR_None != getDrawRect(rcDraw))
					memcpy(&rcDraw, &m_rcDraw, sizeof(VOMP_RECT));

				int nDrawWidth = (rcDraw.right - rcDraw.left + 1) & ~1;
				int nDrawHeight = (rcDraw.bottom - rcDraw.top + 1) & ~1;

				VOMP_VIDEO_BUFFER * pYUVBufferInfo = (VOMP_VIDEO_BUFFER *)pBuffer->pData;

				int nX = nDrawWidth;
				int nY = nDrawHeight;
				int nXOffset = rcDraw.left;
				int nYOffset = rcDraw.top;

				// copy Y data
				for(int i = 0; i < nY; i++)
					memcpy(pYUVBufferInfo->Buffer[0] + pYUVBufferInfo->Stride[0] * i, m_pVideoOutputBuff->Buffer[0] + m_pVideoOutputBuff->Stride[0] * (i + nYOffset) + nXOffset, nX);

				nX = nDrawWidth / 2;
				nY = nDrawHeight / 2;
				nXOffset = rcDraw.left / 2;
				nYOffset = rcDraw.top / 2;

				// copy U data
				for(int i = 0; i < nY; i++)
					memcpy(pYUVBufferInfo->Buffer[1] + pYUVBufferInfo->Stride[1] * i, m_pVideoOutputBuff->Buffer[1] + m_pVideoOutputBuff->Stride[1] * (i + nYOffset) + nXOffset, nX);

				// copy V data
				for(int i = 0; i < nY; i++)
					memcpy(pYUVBufferInfo->Buffer[2] + pYUVBufferInfo->Stride[2] * i, m_pVideoOutputBuff->Buffer[2] + m_pVideoOutputBuff->Stride[2] * (i + nYOffset) + nXOffset, nX);
			}
			else
			{
				VOLOGI("illegal buffer flag 0x%08X", pBuffer->nFlag);
			}
#endif	// _LINUX_ANDROID		
			pBuffer->pBuffer = (unsigned char *)m_pVideoOutputBuff;
			pBuffer->nSize = sizeof (VO_VIDEO_BUFFER);
			pBuffer->llTime = m_pVideoOutputBuff->Time;

			m_nRenderSuccess = 1;
            
            EffectVideoSample (m_pVideoOutputBuff);

			//voCAutoLock lockVideoDec (&m_mtVideoRender);
			//m_nVideoOutPlayIndex = nVideoOutPlayIndex;
		}
		else if (m_nColorType == VOMP_COLOR_RGB565_PACKED || m_nColorType == VOMP_COLOR_RGB888_PACKED ||
				 m_nColorType == VOMP_COLOR_RGB32_PACKED || m_nColorType == VOMP_COLOR_ARGB32_PACKED)
		{
			VO_U64 nStartTime = (VO_U64)voOS_GetSysTime ();
	
			if (m_pVideoRender == NULL)
			{
				m_pVideoRender = new CBaseVideoRender (NULL, m_hView, NULL);
				if (m_pVideoRender == NULL)
					return VOMP_ERR_OutMemory;
				m_pVideoRender->SetWorkPath (m_szWorkPath);
				m_pVideoRender->SetConfigFile (&m_cfgVOSDK);
				m_pVideoRender->SetCPUVersion (m_nCPUVersion);
				m_pVideoRender->SetParam (VO_PID_COMMON_LIBOP, (VO_LIB_OPERATOR *)m_pLibFunc);

				if(m_sVideoTrackInfo.Codec == VO_VIDEO_CodingVP6)
				{
					VO_BOOL bUpSideDown = VO_FALSE;

					if(m_pDataSource)
					{
						m_pDataSource->GetTrackParam(m_nVideoTrack, VO_PID_VIDEO_UPSIDEDOWN, &bUpSideDown);
						m_pVideoRender->SetParam(VO_PID_VIDEO_UPSIDEDOWN, &bUpSideDown);
					}
				}
			}

			if (pBuffer != NULL)
			{
				if(pBuffer->pBuffer == NULL || pBuffer->nSize == 0)
				{
					if(pBuffer->nFlag & VOMP_FLAG_BUFFER_TRY_ONLY)
					{
						m_pVideoOutputBuff = pVideoOutputBuff;
						m_nVideoRenderAdd = 1;
						return VOMP_ERR_None;
					}
					else
					{
						return VOMP_ERR_Pointer;
					}
				}
			}

			m_nVideoRenderAdd = 0;		

			if (m_nVideoRenderNum == 0)
			{
				if (!m_bSetDispRect)
				{
					m_rcDraw.left = 0; m_rcDraw.right = m_nVideoRndWidth;
					m_rcDraw.top = 0;  m_rcDraw.bottom = m_nVideoRndHeight;
				}
				m_pVideoRender->SetVideoInfo (m_nVideoRndWidth, m_nVideoRndHeight, VO_COLOR_YUV_PLANAR420);
				m_pVideoRender->SetDispRect (m_hView, (VO_RECT *)&m_rcDraw, (VO_IV_COLORTYPE)m_nColorType);
				m_pVideoRender->SetDispType(m_nZoomType, VO_RATIO_00);
			}

			m_rgbBuffer.Buffer[0] = pBuffer->pBuffer;
			if (pBuffer->pData != NULL)
				m_rgbBuffer.Stride[0] = *(int *)pBuffer->pData;
			m_rgbBuffer.ColorType = (VO_IV_COLORTYPE)m_nColorType;
			if (m_pVideoRender != NULL)
			{
				voCAutoLock lockVideoDec (&m_mtVideoRender);
				m_pVideoRender->RenderRGBBuffer (m_pVideoOutputBuff, &m_rgbBuffer);
				if (pBuffer->pBuffer == NULL)
				{
					pBuffer->nSize = m_rgbBuffer.Stride[1];
					pBuffer->pBuffer = m_rgbBuffer.Buffer[0];
				}
				pBuffer->llTime = m_pVideoOutputBuff->Time;
				pBuffer->nReserve = m_nVideoRndHeight * m_rgbBuffer.Stride[0];
			}
			else
			{
				m_pVideoOutputBuff = pVideoOutputBuff;
				return VOMP_ERR_OutMemory;
			}

			m_nVideoCCRTimes = m_nVideoCCRTimes + ((VO_U64)voOS_GetSysTime () - nStartTime);
			//VOLOGI ("Video color convert use system time %d", voOS_GetSysTime () - nStartTime);
		}
		else if(m_nColorType == VO_COLOR_IOMX_PRIVATE)
		{
			if(m_pVideoDec)
			{
				if(m_nVideoPlayBack && (preVideoRenderTimeStamp <= m_pVideoOutputBuff->Time || abs(m_pVideoOutputBuff->Time - preVideoRenderTimeStamp) > 1000))
				{
					preVideoRenderTimeStamp = m_pVideoOutputBuff->Time;
					m_pVideoDec->SetParam(VO_PID_IOMXDEC_RenderData, m_pVideoOutputBuff);
				}
				else
					m_pVideoDec->SetParam(VO_PID_IOMXDEC_CancelData, m_pVideoOutputBuff);
			}
		}
		else if (VO_ACODEC_COLOR_TYPE == m_nColorType)  // added by gtxia for dealing with MediaCodec
		{
			if(m_pVideoDec)
			{
				if(m_nVideoPlayBack && (preVideoRenderTimeStamp <= m_pVideoOutputBuff->Time || abs(m_pVideoOutputBuff->Time - preVideoRenderTimeStamp) > 1000))
				{
					preVideoRenderTimeStamp = m_pVideoOutputBuff->Time;
					m_pVideoDec->SetParam(VO_ACODEC_RENDER, m_pVideoOutputBuff);
				}
				else
					m_pVideoDec->SetParam(VO_ACODEC_DROP, m_pVideoOutputBuff);
			}
		}

		if (m_nPlayStopPos > m_nPlayStartPos)
			pBuffer->llTime = pBuffer->llTime - m_nPlayStartPos;

		m_nRenderSuccess = 1;
		//voCAutoLock lockVideoDec (&m_mtVideoRender);
		//m_nVideoOutPlayIndex = nVideoOutPlayIndex;
	}

	if(m_pVideoLogoInfo)
	{
		int nPlayingTime = (int)GetPlayingTime ();
		m_pVideoLogoInfo->RenderSourceEnd(m_pVideoOutputBuff, nPlayingTime, 0);
		m_pVideoLogoInfo->UpdateJitterTimeNumber( abs(nPlayingTime - m_pVideoOutputBuff->Time) );
	}

	if (m_pVideoLogoInfo)
		m_pVideoLogoInfo->UpdataPerformanceData();

//	VOLOGI ("m_nAudioRenderNum is %d", m_nAudioRenderNum);
	if ((m_nShowLogLevel & 0X01) == 1)
	{
		if (m_nLastVideoFrmTime == 0)
			m_nLastVideoFrmTime = (int)m_pVideoOutputBuff->Time;

		VOLOGI ("% 4d, V% 6d  P% 6d, S% 6d S-P % 4d  Frm% 3d  Disp% 3d", (int)m_nVideoRenderNum, (int)m_pVideoOutputBuff->Time, GetPlayingTime (), 
				(int)(voOS_GetSysTime () - (int)m_nStartPlayTime), (int)(voOS_GetSysTime () - (int)m_nStartPlayTime - (int)GetPlayingTime ()), (int)(m_pVideoOutputBuff->Time - (int)m_nLastVideoFrmTime), (int)(voOS_GetSysTime () - (int)m_nLastVideoSysTime));

		m_nLastVideoFrmTime = m_pVideoOutputBuff->Time;
	}

	m_nVideoRenderNum++;
	m_nVideoRndNuming++;

	if(m_pVideoLogoInfo)
	{
		if (m_nAudioTrack < 0 || m_bOnlyGetVideo)
		{
			m_pVideoLogoInfo->m_nStatisPlaybackDurationTime += m_pVideoLogoInfo->m_nFrameRateValue;
		}
	}

	m_nLastVideoSysTime = (VO_U64)voOS_GetSysTime ();

	if (m_bSeekingPos)
	{
		m_bSeekingPos = VO_FALSE;
		postCheckStatusEvent (VOMP_CB_SeekComplete);
	}

	if(abs((int)m_pVideoOutputBuff->Time - (int)GetPlayingTime ()) < 200 && abs((int)m_sVideoSample.Time - (int)GetPlayingTime ()) < 1000)
	{
		voCAutoLock lock(&m_mtNewTime);
		if(m_bVideoNewTime)
		{
			VOLOGI("m_nNewTimeBegin %d, m_bOnlyGetVideo %d", (int)m_nNewTimeBegin, m_bOnlyGetVideo);

			m_bVideoNewTime = VO_FALSE;
			m_nNewTimeVideo = 0;

			if(m_nNewTimeBegin > 0)
				m_nNewTimeBegin--;

			if(m_bOnlyGetVideo && m_nNewTimeBegin > 0)
				m_nNewTimeBegin--;

			if(m_nNewTimeBegin < 0)
				m_nNewTimeBegin = 0;
		}
	}

	{
		voCAutoLock lock(&m_mtNewTime);
		if(m_bVideoNewTime)
		{
			m_nNewTimeVideo++;
			if(m_nNewTimeVideo > 60)
			{
				VOLOGI("m_nNewTimeBegin %d, m_bOnlyGetVideo %d", (int)m_nNewTimeBegin, m_bOnlyGetVideo);

				m_bVideoNewTime = VO_FALSE;
				m_nNewTimeVideo = 0;

				if(m_nNewTimeBegin > 0)
					m_nNewTimeBegin--;

				if(m_bOnlyGetVideo && m_nNewTimeBegin > 0)
					m_nNewTimeBegin--;

				if(m_nNewTimeBegin < 0)
					m_nNewTimeBegin = 0;
			}
		}
	}

	if(m_bFirstFrmRnd == VO_FALSE)
	{
		m_bFirstFrmRnd = VO_TRUE;
		postCheckStatusEvent (VOMP_CB_FirstFrameRender);
	}

	if(m_nOnlyVideoDropCount == 0)
	{
		m_nOnlyVideoDropTime = 0;
	}

#ifdef _VOLOG_INFO
	static int nVideoRndLTime = 0;
	static int TimeStamp = 0;
	VOLOGI ("@@@####rndV Render TimeStamp is %d, Diff: %d, PlayTime: %d, Sync Diff: %d, System Diff: %d",(int)m_pVideoOutputBuff->Time, (int)m_pVideoOutputBuff->Time - TimeStamp, (int)GetPlayingTime (), (int)(m_pVideoOutputBuff->Time - (VO_S64)GetPlayingTime ()), (int)voOS_GetSysTime () - nVideoRndLTime);
	nVideoRndLTime = voOS_GetSysTime ();
	TimeStamp = (int)m_pVideoOutputBuff->Time;
#endif

	m_nVideoRndLTime = (VO_U64)voOS_GetSysTime ();

	if (m_pVideoLogoInfo)
	{
		m_pVideoLogoInfo->m_nVideoRenderStartTime = voOS_GetSysTime();
	}
	return nRC;
}

void voCMediaPlayer::cancelBufferFromVideoDec(VO_VIDEO_BUFFER * pVideoBuffer)
{
	voCAutoLock lockCodec (&m_mtCodec);
	if(m_pVideoDec && pVideoBuffer)
	{
		if(VO_COLOR_IOMX_PRIVATE == m_nColorType)
			m_pVideoDec->SetParam(VO_PID_IOMXDEC_CancelData, pVideoBuffer);
		else if(VO_ACODEC_COLOR_TYPE == m_nColorType)
			m_pVideoDec->SetParam(VO_ACODEC_DROP, pVideoBuffer);
		else if(VO_TRUE == m_bNewVOVideoDec)
		{
			VOLOGI("codec data 0x%08X", pVideoBuffer->CodecData);
			if(pVideoBuffer->CodecData)
				m_pVideoDec->SetParam(VO_PID_COMMON_FRAME_BUF_BACK, pVideoBuffer);
		}
	}
}

void voCMediaPlayer::cancelBuffersFromVOVideoDec(VO_BOOL bResetCodecData /* = VO_FALSE */)
{
	if(m_bNewVOVideoDec && m_nVideoRenderCount > 1)
	{
		voCAutoLock lockVideoDec (&m_mtVideoRender);

		for(VO_U32 nVideoIndex = m_nVideoOutPlayIndex; nVideoIndex < m_nVideoOutFillIndex; nVideoIndex++)
		{
			VO_U32 nIndex = nVideoIndex % m_nVideoRenderCount;
			VO_VIDEO_BUFFER * pVideoBuffer = m_ppVideoOutBuffers[nIndex];
			if(pVideoBuffer == NULL)
				continue;

			{
				voCAutoLock lockCodec (&m_mtCodec);
				VOLOGI("m_pVideoDec 0x%08X, codec data 0x%08X", m_pVideoDec, pVideoBuffer->CodecData);

				if(m_pVideoDec && pVideoBuffer->CodecData)
					m_pVideoDec->SetParam(VO_PID_COMMON_FRAME_BUF_BACK, pVideoBuffer);
			}

			if(VO_TRUE == bResetCodecData)
				pVideoBuffer->CodecData = NULL;	// avoid return old buffers to new video decoder!!
		}
	}
}

int voCMediaPlayer::onLoadSource (int nMsg, int nV1, int nV2)
{
	int nRC = LoadSource ();
	VOLOGI ("The result of load source is %d", nRC);
	if (nRC < 0)
	{
		m_nStatus = VOMP_STATUS_INIT;
		postCheckStatusEvent (VOMP_CB_Error);
		return -1;
	}

	m_nStatus = VOMP_STATUS_STOPPED;
	postCheckStatusEvent (VOMP_CB_OpenSource);

	return 0;
}

int voCMediaPlayer::onCloseSource (int nMsg, int nV1, int nV2)
{
	Reset ();

	postCheckStatusEvent (VOMP_CB_CloseSource);

	return 0;
}

int voCMediaPlayer::onNotifyEvent (int nMsg, int nV1, int nV2)
{
//	VOLOGI ("%08X", nMsg);
	if (m_pListener != NULL)
	{
		voCAutoLock	lockListen (&m_mtListen);
		if ((VO_U32)nMsg != VOMP_CB_CheckStatus)
		{
			if (nMsg == VOMP_CB_PlayComplete)
			{
				if (!m_bNotifyCompleted)
				{
					if (m_pVideoLogoInfo)
					{
						m_pVideoLogoInfo->RestStasticPerfoData();
						m_pVideoLogoInfo->Stop();
					}
					ShowResult ();
					m_bNotifyCompleted = true;
					m_pListener (m_pUserData, nMsg, &nV1, &nV2);
				}
			}
			else
			{
				m_pListener (m_pUserData, nMsg, &nV1, &nV2);
			}
		}
		else
		{
//			if (m_bVideoEOS && m_bAudioEOS)
//				postCheckStatusEvent (VOMP_CB_PlayComplete);

			if (m_nStatus == VOMP_STATUS_RUNNING) 
			{
				if (m_nLastVideoSysTime > 0 && !m_bVideoEOS && !m_bOnlyGetAudio)
				{
					if ((VO_U64)voOS_GetSysTime () - m_nLastVideoSysTime > (VO_U32)m_nFrameRate*50)
					{
						if (!m_bSendStartVideoBuff)
						{
							m_bSendStartVideoBuff = VO_TRUE;
							postCheckStatusEvent (VOMP_CB_VideoStartBuff);
						}
					}
					else
					{
						if (m_bSendStartVideoBuff)
						{
							m_bSendStartVideoBuff = VO_FALSE;
							postCheckStatusEvent (VOMP_CB_VideoStopBuff);
						}
					}
				}

				if((m_bOnlyGetAudio || m_nVideoDecError > 100) && m_bSendStartVideoBuff)
				{
					m_bSendStartVideoBuff = VO_FALSE;
					postCheckStatusEvent (VOMP_CB_VideoStopBuff);
				}
				if (m_nLastAudioSysTime > 0 && !m_bAudioEOS && !m_bOnlyGetVideo)
				{
					if (((VO_U64)voOS_GetSysTime () - m_nLastAudioSysTime > m_nNotifyBuffingTime) && m_nAudioDecError <= 200)
					{
						if (!m_bSendStartAudioBuff)
						{
							m_bSendStartAudioBuff = VO_TRUE;
							postCheckStatusEvent (VOMP_CB_AudioStartBuff);
						}
					}
					else
					{
						if (m_bSendStartAudioBuff)
						{
							m_bSendStartAudioBuff = VO_FALSE;
							postCheckStatusEvent (VOMP_CB_AudioStopBuff);
						}
					}					
				}

				if((m_bOnlyGetVideo || m_nAudioDecError > 200)  && m_bSendStartAudioBuff)
				{
					m_bSendStartAudioBuff = VO_FALSE;
					postCheckStatusEvent (VOMP_CB_AudioStopBuff);
				}

				if (m_nOnEventTimes % 4 == 0 && m_nVideoPerfOnOff && m_pCPULoadInfo)
				{
					int nOutSys = 0;
					int nOutUsr = 0;
					int	nOutAP = 0;

					int rc = m_pCPULoadInfo->InitCPUProc();
					if(rc == 0)
					{
						rc = m_pCPULoadInfo->GetCPULoad(&nOutSys, &nOutUsr, &nOutAP);

						if(m_pVideoLogoInfo)
						{
							m_pVideoLogoInfo->SetParam(LOGO_CPU_USER_LOAD_VALUE, &nOutUsr);
							m_pVideoLogoInfo->SetParam(LOGO_CPU_SYS_LOAD_VALUE, &nOutSys);
							m_pVideoLogoInfo->SetParam(LOGO_CPU_APP_LOAD_VALUE, &nOutAP);
						}

						m_pCPULoadInfo->UpdateCPUProc();
					}
				}

//				if (m_nOnEventTimes % 4 == 0 && !m_bOnlyGetAudio)
//				{
//					int nDelayTime, nLastTime;
//					if (m_pVideoOutputBuff != NULL)
//					{
//						nLastTime = (int)m_pVideoOutputBuff->Time;
//						nDelayTime = GetPlayingTime () - nLastTime;
//					}
//					else
//					{
//						nLastTime = (int)m_sVideoSample.Time;
//						nDelayTime = GetPlayingTime () - nLastTime;
//					}
//
//					postCheckStatusEvent (VOMP_CB_VideoDelayTime, nDelayTime, 0);
//
//					postCheckStatusEvent (VOMP_CB_VideoLastTime, nLastTime, 0);
//
//					postCheckStatusEvent (VOMP_CB_VideoDropFrames, m_nDroppedVidooFrames, 0);
//
//					if (m_nVideoRndTimeIndex >= 100 && m_nVideoDcdTimeIndex >= 100)
//					{
//						voCAutoLock lockVideo (&m_mtVideoFlush);
//
//						int nFirstFrame = m_nVideoRndTimeIndex - 1;
//						while (m_aVideoRndTime[m_nVideoRndTimeIndex - 1] - m_aVideoRndTime[nFirstFrame] < 5000)
//						{
//							nFirstFrame--;
//							if (nFirstFrame <= 0)
//								break;
//						}
//
//						int nPlayTime = m_aVideoRndTime[m_nVideoRndTimeIndex - 1] - m_aVideoRndTime[nFirstFrame];
//						if (nPlayTime > 0)
//						{
//							int nRndFrameRate = ((m_nVideoRndTimeIndex - nFirstFrame - 1) * 1000 + 500) / nPlayTime;
//
//							nFirstFrame = m_nVideoDcdTimeIndex - 1;
//							while (m_aVideoDcdTime[m_nVideoDcdTimeIndex - 1] - m_aVideoDcdTime[nFirstFrame] < 5000)
//							{
//								nFirstFrame--;
//								if (nFirstFrame <= 0)
//									break;
//							}
//							nPlayTime = m_aVideoDcdTime[m_nVideoDcdTimeIndex - 1] - m_aVideoDcdTime[nFirstFrame];
//							
//#if defined _IOS || defined(_MAC_OS)							
//							// added by linjun
//							// tag:20110308
//							if(nPlayTime <= 0)
//								return 0;
//							//end
//#endif
//							
//							if (nPlayTime > 0)
//							{
//								int nDcdFrameRate = ((m_nVideoDcdTimeIndex - nFirstFrame - 1) * 1000 + 500) / nPlayTime;
//
//								postCheckStatusEvent (VOMP_CB_VideoFrameRate, nDcdFrameRate, nRndFrameRate);
//							}
//						}
//					}
//
//				}
			}

			m_nOnEventTimes++;
			postCheckStatusEvent (VOMP_CB_CheckStatus, 0, 0, m_nStatusCheckTime);
		}
	}

	return 0;
}

int voCMediaPlayer::postCheckStatusEvent (int nMsg, int nV1, int nV2, int nDelayTime)
{
	if (m_pMsgThread == NULL)
		return -1;

//	VOLOGI ("delay time %d", nDelayTime);

	voCBaseEventItem * pEvent = m_pMsgThread->GetEventByType (VOMP_EVENT_STATUS);
	if (pEvent == NULL)
		pEvent = new voCMediaPlayerEvent (this, &voCMediaPlayer::onNotifyEvent, VOMP_EVENT_STATUS, nMsg, nV1, nV2);
	else
		pEvent->SetEventMsg (nMsg, nV1, nV2);

	m_pMsgThread->PostEventWithDelayTime (pEvent, nDelayTime);

	return 0;
}

int voCMediaPlayer::onRenderAudio (int nMsg, int nV1, int nV2)
{
//	voCAutoLock lockAudio (&m_mtAudioFlush);
	if(m_pAudioDecOutPcmBuff == NULL)
	{
		m_pAudioDecOutPcmBuff = new VO_BYTE[m_nAudioDecOutFrameSize];
		if(!m_pAudioDecOutPcmBuff)
			return VOMP_ERR_OutMemory;
	}
	m_pAudioDecOutBuff = m_pAudioDecOutPcmBuff;
	m_nAudioDecOutSize = m_nAudioDecOutFrameSize;

	int nDelayTime = -1;
	VO_U32 nRC = DoAudioRender (NULL);
	if (nRC == VOMP_ERR_Retry)
	{
		nDelayTime = -1;
	}
	else if (nRC == VOMP_ERR_FormatChange)
		nDelayTime = -1;

	if (!m_bAudioEOS)
		postAudioRenderEvent (nDelayTime);

	return 0;
}

int voCMediaPlayer::postAudioRenderEvent (int nDelayTime)
{
	if (m_pAudioThread == NULL)
		return -1;

	int nFullNum = m_pAudioThread->GetFullEventNum (VOMP_EVENT_AUDIO);
	if (nFullNum > 0)
		m_pAudioThread->CancelEventByType (VOMP_EVENT_AUDIO);

	voCBaseEventItem * pEvent = m_pAudioThread->GetEventByType (VOMP_EVENT_AUDIO);
	if (pEvent == NULL)
		pEvent = new voCMediaPlayerEvent (this, &voCMediaPlayer::onRenderAudio, VOMP_EVENT_AUDIO);
	m_pAudioThread->PostEventWithDelayTime (pEvent, nDelayTime);

	return 0;
}

int voCMediaPlayer::onDecoderAudio (int nMsg, int nV1, int nV2)
{
	{
		voCAutoLock lockAudioDec(&m_mtAudioRender);
		if(!m_pARBManager)
			m_pARBManager = new voCAudioRenderBufferMgr(m_nAudioRenderBufferCount);

		if (m_pARBManager->isFull())
		{
			// if buffer is not ready or full
			voCAutoLock lock (&m_mtStatus);
			postAudioDecoderEvent (2);
			return 0;
		}
	}

	if (!m_bSetAudioDecoderThreadPriority)
	{
		m_bSetAudioDecoderThreadPriority = true;
#ifdef _LINUX_ANDROID
//		setpriority(PRIO_PROCESS, 0, ANDROID_PRIORITY_AUDIO);
#endif //_LINUX_ANDROID
#ifdef _WIN32
		voThreadSetPriority (GetCurrentThread (), VO_THREAD_PRIORITY_ABOVE_NORMAL);
#endif // _WIN32
#ifdef _IOS
//		voThreadSetPriority((void *)voThreadGetCurrentID(), VO_THREAD_PRIORITY_ABOVE_NORMAL);
#endif // _IOS
	}

	int nDelayTime = -1;
	VOMP_AUDIORENDERBUFFER * pARB = NULL;
	VO_U32 nRC = VOMP_ERR_None;
	{
		voCAutoLock lockAudioDec(&m_mtAudioRender);
		nRC = m_pARBManager->getWritableBuffer(&pARB, &m_sAudioDecOutFormat);
	}
	if(VOMP_ERR_None == nRC)
	{
		m_pAudioDecOutBuff = pARB->pData;
		m_nAudioDecOutSize = pARB->nSize;

		int nRC = ReadAudioSample ();

		if (nRC == VOMP_ERR_EOS)
		{
			VOLOGI("VOMP_ERR_EOS of ReadAudioSample, just set EOS arrived flag");
			if(m_nAudioEOSArrived == 0)
				m_nAudioEOSArrived = 2;
		}
		else if (nRC == VOMP_ERR_FormatChange)
			m_nAudioDecOutLength = 0;

		if (VOMP_ERR_None == nRC && m_nAudioDecOutLength >= m_nAudioDecOutStepSize)
		{
			// here we got PCM data from audio decoder
			pARB->nLength = m_nAudioDecOutLength;
			pARB->llTime = m_sAudioDecOutBuf.Time;
			memcpy(&pARB->sFormat, &m_sAudioDecOutFormat, sizeof(VO_AUDIO_FORMAT));

			voCAutoLock lockAudioDec(&m_mtAudioRender);
			m_pARBManager->finishWriting(pARB);
		}
	}
	else if(VOMP_ERR_Retry == nRC)
	{
		nDelayTime = 5;
	}
	else
		return 0;

	if (!m_bAudioEOS)
	{
		voCAutoLock lock (&m_mtStatus);
		postAudioDecoderEvent (nDelayTime);
	}

	return 0;
}

int voCMediaPlayer::postAudioDecoderEvent (int nDelayTime)
{
	if (m_pAudioDecThread == NULL)
		return -1;

	int nFullNum = m_pAudioDecThread->GetFullEventNum (VOMP_EVENT_AUDIO);
	if (nFullNum > 0)
		m_pAudioDecThread->CancelEventByType (VOMP_EVENT_AUDIO);

	voCBaseEventItem * pEvent = m_pAudioDecThread->GetEventByType (VOMP_EVENT_AUDIO);
	if (pEvent == NULL)
		pEvent = new voCMediaPlayerEvent (this, &voCMediaPlayer::onDecoderAudio, VOMP_EVENT_AUDIO);
	m_pAudioDecThread->PostEventWithDelayTime (pEvent, nDelayTime);

	return 0;
}

int voCMediaPlayer::onRenderVideo (int nMsg, int nV1, int nV2)
{
//	voCAutoLock lockVideo (&m_mtVideoFlush);
	int nDelayTime = -1;
	VO_U32 nRC = DoVideoRender (NULL);

	if (nRC == VOMP_ERR_Retry)
		nDelayTime = 5;
	else if (nRC == VOMP_ERR_OutOfTime)
		nDelayTime = -1;
	else if (nRC == VOMP_ERR_FormatChange)
		nDelayTime = -1;
	else if (nRC == VOMP_ERR_EOS)
		m_bVideoEOS = VO_TRUE;

	if (!m_bVideoEOS)
	{
		voCAutoLock lock (&m_mtStatus);

		if (m_nStatus == VOMP_STATUS_RUNNING)
		{
			postVideoRenderEvent (nDelayTime);
		}
		else
		{
			if (m_nVideoRenderNum <= 0)
				postVideoRenderEvent (nDelayTime);
		}
	}
	else if (m_nVideoRenderCount > 1)
	{
		if (m_nVideoOutPlayIndex < m_nVideoOutFillIndex)	
			postVideoRenderEvent (nDelayTime);
	}

	return 0;
}

int voCMediaPlayer::onDecoderVideo (int nMsg, int nV1, int nV2)
{
	{
		voCAutoLock lockVideoDec (&m_mtVideoRender);
		if (m_nVideoOutFillIndex >= m_nVideoOutPlayIndex + m_nVideoRenderCount - 1)
		{
			voCAutoLock lock (&m_mtStatus);
			postVideoDecoderEvent (1);
			return 0;
		}
	}

#ifdef _WIN32
	//Set decode thread priority at below normal for quick render video frame
	//Because decode thread hold long time for decode a frame some time , this cause render thread waiting longer .
	//We should render have decoded frames quickly.
	if(m_nVideoDecoderNum == 0)
	{
		voThreadSetPriority(GetCurrentThread (), VO_THREAD_PRIORITY_BELOW_NORMAL);
	}
#endif // _WIN32

	int nDelayTime = -1;
	VO_U32 nRC = ReadVideoSample ();
	if (nRC == VOMP_ERR_Retry)
		nDelayTime = m_bVideoDecHungry ? -1 : 5;	// if video decoder is hungry, we need input data ASAP
	else if (nRC == VOMP_ERR_OutOfTime)
		nDelayTime = -1;
	else if (nRC == VOMP_ERR_FormatChange)
		nDelayTime = -1;
	else if (nRC == VOMP_ERR_EOS)
		m_bVideoEOS = VO_TRUE;

	if (!m_bVideoEOS)
	{
		voCAutoLock lock (&m_mtStatus);
		postVideoDecoderEvent (nDelayTime);
	}

	return 0;
}

int voCMediaPlayer::postVideoRenderEvent (int nDelayTime)
{
	if (m_pVideoThread == NULL)
		return -1;

	int nFullNum = m_pVideoThread->GetFullEventNum (VOMP_EVENT_VIDEO);
	if (nFullNum > 0)
		m_pVideoThread->CancelEventByType (VOMP_EVENT_VIDEO);

	voCBaseEventItem * pEvent = m_pVideoThread->GetEventByType (VOMP_EVENT_VIDEO);
	if (pEvent == NULL)
		pEvent = new voCMediaPlayerEvent (this, &voCMediaPlayer::onRenderVideo, VOMP_EVENT_VIDEO);
	m_pVideoThread->PostEventWithDelayTime (pEvent, nDelayTime);

	return 0;
}

int voCMediaPlayer::postVideoDecoderEvent (int nDelayTime)
{
	if (m_pVideoDecThread == NULL)
		return -1;

	int nFullNum = m_pVideoDecThread->GetFullEventNum (VOMP_EVENT_VIDEO);
	if (nFullNum > 0)
		m_pVideoDecThread->CancelEventByType (VOMP_EVENT_VIDEO);

	voCBaseEventItem * pEvent = m_pVideoDecThread->GetEventByType (VOMP_EVENT_VIDEO);
	if (pEvent == NULL)
		pEvent = new voCMediaPlayerEvent (this, &voCMediaPlayer::onDecoderVideo, VOMP_EVENT_VIDEO);
	m_pVideoDecThread->PostEventWithDelayTime (pEvent, nDelayTime);

	return 0;
}

int voCMediaPlayer::vompStreamCallBack (long EventCode, long * EventParam1, long * userData)
{
	voCMediaPlayer *	pPlayer = (voCMediaPlayer *)userData;

	if (EventCode == VO_STREAM_BUFFERSTART)
	{
		VOLOGI ("VO_STREAM_BUFFERSTART");
		//pPlayer->m_bDataBuffering = VO_TRUE;
		if (pPlayer->m_nFF == VO_FILE_FFSTREAMING_RTSP)
			pPlayer->postCheckStatusEvent (VOMP_CB_BufferStatus, 0);
	}
	else if (EventCode == VO_STREAM_BUFFERSTATUS)
	{
		VOLOGI ("VO_STREAM_BUFFERSTATUS  %d ", (int)(*EventParam1));
		//pPlayer->m_bDataBuffering = VO_TRUE;
		if (pPlayer->m_nFF == VO_FILE_FFSTREAMING_RTSP)
			pPlayer->postCheckStatusEvent (VOMP_CB_BufferStatus, *EventParam1);
	}
	else if (EventCode == VO_STREAM_BUFFERSTOP)
	{
		VOLOGI ("VO_STREAM_BUFFERSTOP");
		//pPlayer->m_bDataBuffering = VO_FALSE;
		if (pPlayer->m_nFF == VO_FILE_FFSTREAMING_RTSP)
			pPlayer->postCheckStatusEvent (VOMP_CB_BufferStatus, 100);
	}
	else if (EventCode == VO_STREAM_DOWNLODPOS)
	{
		VOLOGI ("VO_STREAM_DOWNLODPOS  %d ", (int)(*EventParam1));
		pPlayer->postCheckStatusEvent (VOMP_CB_DownloadPos, *EventParam1);
	}
	else if(EventCode == VO_STREAM_SEEKFINALPOS)
	{
		VOLOGI ("VO_STREAM_SEEKFINALPOS  %d ", (int)(*EventParam1));
		pPlayer->m_nSeekPos = (VO_S64)(*((VO_U32 *)EventParam1));
		pPlayer->m_sAudioSample.Time = pPlayer->m_nSeekPos;
		pPlayer->m_sVideoSample.Time = pPlayer->m_nSeekPos;

		pPlayer->m_sVideoDecOutBuf.Time = pPlayer->m_nSeekPos;
		pPlayer->m_sAudioDecOutBuf.Time = -1;
		pPlayer->m_nStartMediaTime = pPlayer->m_nSeekPos;
		pPlayer->m_llAudioRenderTime = pPlayer->m_nSeekPos;
		pPlayer->m_llLastAudioRenderTime = pPlayer->m_nSeekPos;
	}
	else if (EventCode == VO_STREAM_ERROR)
	{
		VOLOGI ("VO_STREAM_ERROR  %d ", (int)(*EventParam1));

		pPlayer->postCheckStatusEvent (VOMP_CB_Error, *EventParam1);
	}
	else if (EventCode == VO_STREAM_RUNTIMEERROR)
	{
		VOLOGI ("VO_STREAM_RUNTIMEERROR  %d ", (int)(*EventParam1));

		pPlayer->postCheckStatusEvent (VOMP_CB_Error, *EventParam1);
	}
	else if(EventCode == VO_STREAM_DESCRIBE_FAIL)
	{
		VOLOGI ("VOMP_CB_DescribeFail  %d ", (int)(*EventParam1));

		pPlayer->postCheckStatusEvent (VOMP_CB_DescribeFail, *EventParam1);
	}
	else if (EventCode == VO_STREAM_PACKETLOST)
	{
		VOLOGI ("VO_STREAM_PACKETLOST");

		pPlayer->postCheckStatusEvent (VOMP_CB_PacketLost, *EventParam1);
	}

	return 0;
}

int voCMediaPlayer::vompDrmCallBack (void * pUserData, int nFlag, void * pParam, int nReserved)
{
	voCMediaPlayer * pPlayer = (voCMediaPlayer *)pUserData;

	pPlayer->m_bDrmCrippled = VO_TRUE;

	return 0;
}


int voCMediaPlayer::redraw (VOMP_BUFFERTYPE ** ppBuffer)
{
	m_bRedrawVideo = VO_FALSE;

	if(NULL != (*ppBuffer)){
		if(NULL != m_pVideoOutputBuff){
			if (m_nColorType == VOMP_COLOR_RGB565_PACKED || m_nColorType == VOMP_COLOR_RGB888_PACKED ||
				m_nColorType == VOMP_COLOR_RGB32_PACKED || m_nColorType == VOMP_COLOR_ARGB32_PACKED)
			{
				VO_VIDEO_BUFFER * pVideoOutputBuff = m_pVideoOutputBuff; 
				VOMP_BUFFERTYPE * pBuffer = *ppBuffer;
				if (pBuffer != NULL)  
				{
					if(pBuffer->pBuffer == NULL || pBuffer->nSize == 0)
					{
						if(pBuffer->nFlag & VOMP_FLAG_BUFFER_TRY_ONLY)
						{
							m_pVideoOutputBuff = pVideoOutputBuff;
							m_nVideoRenderAdd = 1;         
							return VOMP_ERR_None;        
						}
						else
						{
							return VOMP_ERR_Pointer;     
						}
					}
				}
				if (m_nVideoRenderNum == 0)    
				{
					if (!m_bSetDispRect)
					{
						m_rcDraw.left = 0; m_rcDraw.right = m_nVideoRndWidth;
						m_rcDraw.top = 0;  m_rcDraw.bottom = m_nVideoRndHeight;
					}
					m_pVideoRender->SetVideoInfo (m_nVideoRndWidth, m_nVideoRndHeight, VO_COLOR_YUV_PLANAR420);
					m_pVideoRender->SetDispRect (m_hView, (VO_RECT *)&m_rcDraw, (VO_IV_COLORTYPE)m_nColorType);
					m_pVideoRender->SetDispType(m_nZoomType, VO_RATIO_00);
				}
				m_rgbBuffer.Buffer[0] = pBuffer->pBuffer;
				if (pBuffer->pData != NULL)      
					m_rgbBuffer.Stride[0] = *(int *)pBuffer->pData;
				m_rgbBuffer.ColorType = (VO_IV_COLORTYPE)m_nColorType;
				if (m_pVideoRender != NULL)    
				{
					voCAutoLock lockVideoDec (&m_mtVideoRender);
					m_pVideoRender->RenderRGBBuffer (m_pVideoOutputBuff, &m_rgbBuffer);
					if (pBuffer->pBuffer == NULL)  
					{
						pBuffer->nSize = m_rgbBuffer.Stride[1];
						pBuffer->pBuffer = m_rgbBuffer.Buffer[0];
					}
					pBuffer->llTime = m_pVideoOutputBuff->Time;
					pBuffer->nReserve = m_nVideoRndHeight * m_rgbBuffer.Stride[0];
				}
				else
				{
					return VOMP_ERR_OutMemory;   
				}
			}
			else if (m_nColorType == VOMP_COLOR_YUV_PLANAR420 || m_nColorType == VOMP_COLOR_YUV_420_PACK || m_nColorType == VOMP_COLOR_YUV_PLANAR420_NV12)
			{
				VO_VIDEO_BUFFER * pVideoOutputBuff = m_pVideoOutputBuff; 
				VOMP_BUFFERTYPE * pBuffer = *ppBuffer;
				if (pBuffer != NULL)
				{
					if(pBuffer->nFlag & VOMP_FLAG_BUFFER_TRY_ONLY)
					{
						m_pVideoOutputBuff = pVideoOutputBuff;
						m_nVideoRenderAdd = 1;
						return VOMP_ERR_None;
					}
				}

				m_nVideoRenderAdd = 0;
#ifdef _LINUX_ANDROID
				if(m_nVideoRenderNum == 0 && !m_bSetDispRect)
				{
					m_rcDraw.left = 0;
					m_rcDraw.right = m_nVideoRndWidth;
					m_rcDraw.top = 0;
					m_rcDraw.bottom = m_nVideoRndHeight;
				}

//				VOLOGI("GetVideoData %d %d %d %d W %d H %d", m_rcDraw.left, m_rcDraw.right, m_rcDraw.top, m_rcDraw.bottom, m_nVideoRndWidth, m_nVideoRndHeight);

				if(pBuffer->nFlag & VOMP_FLAG_BUFFER_YUVBUFFER)
				{
					VOMP_RECT rcDraw;
					if(VOMP_ERR_None != getDrawRect(rcDraw))
						memcpy(&rcDraw, &m_rcDraw, sizeof(VOMP_RECT));

					int nDrawWidth = (rcDraw.right - rcDraw.left + 1) & ~1;
					int nDrawHeight = (rcDraw.bottom - rcDraw.top + 1) & ~1;

					VOMP_VIDEO_BUFFER * pYUVBufferInfo = (VOMP_VIDEO_BUFFER *)pBuffer->pData;

					int nX = nDrawWidth;
					int nY = nDrawHeight;
					int nXOffset = rcDraw.left;
					int nYOffset = rcDraw.top;

					// copy Y data
					for(int i = 0; i < nY; i++)
						memcpy(pYUVBufferInfo->Buffer[0] + pYUVBufferInfo->Stride[0] * i, m_pVideoOutputBuff->Buffer[0] + m_pVideoOutputBuff->Stride[0] * (i + nYOffset) + nXOffset, nX);

					nX = nDrawWidth / 2;
					nY = nDrawHeight / 2;
					nXOffset = rcDraw.left / 2;
					nYOffset = rcDraw.top / 2;

					// copy U data
					for(int i = 0; i < nY; i++)
						memcpy(pYUVBufferInfo->Buffer[1] + pYUVBufferInfo->Stride[1] * i, m_pVideoOutputBuff->Buffer[1] + m_pVideoOutputBuff->Stride[1] * (i + nYOffset) + nXOffset, nX);

					// copy V data
					for(int i = 0; i < nY; i++)
						memcpy(pYUVBufferInfo->Buffer[2] + pYUVBufferInfo->Stride[2] * i, m_pVideoOutputBuff->Buffer[2] + m_pVideoOutputBuff->Stride[2] * (i + nYOffset) + nXOffset, nX);
				}
				else
				{
					VOLOGI("illegal buffer flag 0x%08X", pBuffer->nFlag);
				}
#endif	// _LINUX_ANDROID		
				pBuffer->pBuffer = (unsigned char *)m_pVideoOutputBuff;
				pBuffer->nSize = sizeof (VO_VIDEO_BUFFER);
				pBuffer->llTime = m_pVideoOutputBuff->Time;

				m_nRenderSuccess = 1;

				EffectVideoSample (m_pVideoOutputBuff);
			}
		}
		return VOMP_ERR_None;
	}
	else 
	{    
		if(NULL != m_pVideoOutputBuff){
			if (m_nColorType == VOMP_COLOR_YUV_PLANAR420 || m_nColorType == VOMP_COLOR_YUV_420_PACK || m_nColorType == VOMP_COLOR_YUV_PLANAR420_NV12)
			{
				VO_VIDEO_BUFFER * pVideoOutputBuff = m_pVideoOutputBuff; 
				VOMP_BUFFERTYPE * pBuffer = NULL;
				memset (&m_outVideoBuffer, 0, sizeof (m_outVideoBuffer));
				*ppBuffer = &m_outVideoBuffer; 
				pBuffer = &m_outVideoBuffer;

				if (pBuffer != NULL)
				{
					if(pBuffer->nFlag & VOMP_FLAG_BUFFER_TRY_ONLY)
					{
						m_pVideoOutputBuff = pVideoOutputBuff;
						m_nVideoRenderAdd = 1;         
						return VOMP_ERR_None;        
					}
				}

				m_nVideoRenderAdd = 0;

				pBuffer->pBuffer = (unsigned char *)m_pVideoOutputBuff;
				pBuffer->nSize = sizeof (VO_VIDEO_BUFFER);
				pBuffer->llTime = m_pVideoOutputBuff->Time;

				m_nRenderSuccess = 1;

#ifndef _IOS
				EffectVideoSample (m_pVideoOutputBuff);
#endif
			}
		}
		return VOMP_ERR_None;
	}
	return VOMP_ERR_None;
}
