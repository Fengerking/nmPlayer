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
#elif defined _IOS
#include <stdlib.h>
#ifdef USE_HW_AUDIO_DEC
#include "CAudioMockDecoder.h"
#endif
#elif defined _MAC_OS
#include <stdlib.h>
#else
#include <malloc.h>
#endif // __SYMBIAN32__
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "vompMeta.h"

#include "voOSFunc.h"
#include "cmnFile.h"
#include "CFileFormatCheck.h"

#include "CRTSPSource.h"
#include "CHTTPPDSource.h"

#include "voH264.h"
#include "voH265.h"

#include "voCDataSource.h"
#include "voCDataSourceOP.h"

//#include "voCMixDataSource.h"
#include "voCLiveDataSource.h"

#include "voCheck.h"
#include "voHalInfo.h"

#include "voAMediaCodec.h"

#ifdef _METRO
#elif defined _WIN32
#include "CWaveOutRender.h"
#include "CGDIVideoRender.h"
#endif  //_METRO


#ifdef _IOS
#include <mach/machine.h>
#include "CAudioQueueRender.h"
#include "CAudioUnitRender.h"
#include "CiOSVideoRender.h"
#include "voLogoEffect.h"
#endif

#ifdef _MAC_OS

#ifdef _MAC_OS_CG
#include "CCGRender.h"
#else
#include "CMacOSVideoRender.h"
#endif // _MAC_OS_CG

#include "CAudioQueueRender.h"
#endif // _MAC_OS

#ifdef _LINUX_ANDROID
#ifndef __VO_NDK__
#include <cutils/properties.h>
#include <sys/resource.h>
#else
//#include <sys/linux-unistd.h>
#include <linux/resource.h>
#include <sys/resource.h>
#include <sys/sysconf.h>
#endif  //__VO_NDK__
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif // _LINUX_ANDROID

#include "IAudioDSPClock.h"
#include "voCMediaPlayer.h"

#define LOG_TAG "voCMediaPlayerInit"
#include "voLog.h"

#define BA_DEBUG

int voCMediaPlayer::OpenPlayConfigFile (VO_TCHAR * pCfgFile)
{
#ifndef __SYMBIAN32__
#ifndef _MAC_OS
	VO_TCHAR szCfgFile[256];
	VO_BOOL	bOpen = VO_TRUE;
	if(pCfgFile)
	{
		vostrcpy (szCfgFile, pCfgFile);
		bOpen = m_cfgPlay.Open (szCfgFile);
	}
#endif // _MAC_OS
#endif // __SYMBIAN32__

	VOLOGI("Before configure file ABT %lld, RAT %lld, DDrpV %d, DDbkV %d, SM %d, NBT %lld, SCT %d, SLL %d, AST %d, VRCnt %d, VRCpy %d, ITP %d, AAP %d", 
		m_nAudioBuffTime, m_llRenderAdjustTime, m_nDisableDropVideoFrame, m_nDisableDeblockVideo, m_nSeekMode, m_nNotifyBuffingTime, 
		m_nStatusCheckTime, m_nShowLogLevel, m_nAudioStepTime, m_nVideoRenderCount, m_nVideoRndCopy, m_nIdleThdPriority, 
		m_nAudioAACPlus);

	if(m_nAudioBuffTime == 0)
#ifdef _WIN32
		m_nAudioBuffTime = 400;
#elif defined __SYMBIAN32__
		m_nAudioBuffTime = 300;
#elif defined _IOS
		m_nAudioBuffTime = 150;
#elif defined _MAC_OS
		m_nAudioBuffTime = 1100;
#endif // _WIN32
	m_nAudioBuffTime = (VO_U64)m_cfgPlay.GetItemValue ("Playback", "AudioSinkBuffTime", (VO_S32)m_nAudioBuffTime);

#ifdef _WIN32
	// wave out renderer use 1/6 s as once buffer time, so we must use it to avoid video dropped and A/V sync, East 20130510
	m_llRenderAdjustTime = 200;
#endif //UNDER_CE

#ifdef UNDER_CE
	m_llRenderAdjustTime = 300;
#endif //UNDER_CE

#ifdef _IOS
	m_llRenderAdjustTime = 200;
	m_nAudioStepTime = 50;
#endif
	m_llRenderAdjustTime = (VO_S64)m_cfgPlay.GetItemValue ("Playback", "AudioSinkAdjustTime", (VO_S32)m_llRenderAdjustTime);
	m_nDisableDropVideoFrame = m_cfgPlay.GetItemValue ("Playback", "DisableDropVideoFrame", m_nDisableDropVideoFrame);
	m_nDisableDeblockVideo = m_cfgPlay.GetItemValue ("Playback", "DisableDeblockVideo", m_nDisableDeblockVideo);
	m_nSeekMode = m_cfgPlay.GetItemValue ("Playback", "SeekMode", m_nSeekMode);
	m_nNotifyBuffingTime = (VO_U64)m_cfgPlay.GetItemValue ("Playback", "RenderNotifyTime", (VO_S32)m_nNotifyBuffingTime);
	m_nStatusCheckTime = m_cfgPlay.GetItemValue ("Playback", "StatusCheckTime", m_nStatusCheckTime);
	m_nShowLogLevel = m_cfgPlay.GetItemValue ("Playback", "ShowLogLevel", m_nShowLogLevel);
	m_nAudioStepTime = m_cfgPlay.GetItemValue ("Playback", "AudioSinkStepTime", m_nAudioStepTime);
	m_nVideoRenderCount = m_cfgPlay.GetItemValue ("Playback", "VideoRenderBuffers", m_nVideoRenderCount);
	m_nVideoRndCopy = m_cfgPlay.GetItemValue ("Playback", "VideoRndBufCopy", m_nVideoRndCopy);
	m_nIdleThdPriority = m_cfgPlay.GetItemValue ("Playback", "IDLEThread", m_nIdleThdPriority);
	m_nAudioAACPlus = m_cfgPlay.GetItemValue("Playback", "AudioEfficient", m_nAudioAACPlus);

#ifdef _LINUX_ANDROID
	char	szPropName[256];
	char	szPropProduct[256];
	char	szPropModel[256];
	char	szPropText[1024];

	memset (szPropName, 0, sizeof (szPropName));
	m_utProp.GetProperty("ro.product.manufacturer", szPropName, "VisualOn");

	memset (szPropProduct, 0, sizeof (szPropProduct));
	m_utProp.GetProperty("ro.product.name", szPropProduct, "VisualOn");

	memset (szPropModel, 0, sizeof (szPropModel));
	m_utProp.GetProperty("ro.product.model", szPropModel, "VisualOn");

	sprintf (szPropText, "%s_%s_%s", szPropName, szPropProduct, szPropModel);
	m_nAudioBuffTime = (VO_U64)m_cfgPlay.GetItemValue (szPropText, (char *)"AudioSinkBuffTime", (VO_S32)m_nAudioBuffTime);
#endif // _LINUX_ANDROID

	VOLOGI("After configure file ABT %lld, RAT %lld, DDrpV %d, DDbkV %d, SM %d, NBT %lld, SCT %d, SLL %d, AST %d, VRCnt %d, VRCpy %d, ITP %d, AAP %d", 
		m_nAudioBuffTime, m_llRenderAdjustTime, m_nDisableDropVideoFrame, m_nDisableDeblockVideo, m_nSeekMode, m_nNotifyBuffingTime, 
		m_nStatusCheckTime, m_nShowLogLevel, m_nAudioStepTime, m_nVideoRenderCount, m_nVideoRndCopy, m_nIdleThdPriority, 
		m_nAudioAACPlus);

	return 0;
}

int voCMediaPlayer::OpenVOSDKConfigFile (VO_TCHAR * pCfgFile)
{
#ifndef __SYMBIAN32__
#ifndef _MAC_OS	
#ifndef _IOS
	VO_TCHAR szCfgFile[256];
	if (pCfgFile == NULL)
	{
		//vostrcpy (szCfgFile, _T("/data/local/voOMXPlayer/vommcodec.cfg"));
		//if (!m_cfgVOSDK.Open (szCfgFile))
		//{
		//	if (vostrlen (m_szWorkPath) > 0)
		//	{
		//		vostrcpy (szCfgFile, m_szWorkPath);
		//		vostrcat (szCfgFile, _T("/vommcodec.cfg"));
		//		m_cfgVOSDK.Open (szCfgFile);
		//	}
		//	else
		//	{
		//		vostrcpy (szCfgFile, _T("vommcodec.cfg"));
		//		m_cfgVOSDK.Open (szCfgFile);
		//	}
		//}
	}
	else
	{
		vostrcpy (szCfgFile, pCfgFile);
		m_cfgVOSDK.Open (szCfgFile);
	}
#endif // _IOS
#endif // _MAC_OS
#endif // __SYMBIAN32__
	
	return 0;
}

void voCMediaPlayer::CheckCPUInfo (void)
{
#if defined(_LINUX_ANDROID) && defined (__VO_NDK__)	
    VO_CPU_Info info;
    get_cpu_info(&info);
	if (info.mType == CPU_FAMILY_ARM) 
	{
		if ((info.mFeatures & CPU_ARM_FEATURE_NEON) != 0) 
		{
			m_nCPUVersion = 7;
			m_sCPUInfo.nCPUType = 1;
		}
	}

    m_nCPUNumber = info.mCount;
	m_sCPUInfo.nCoreCount = info.mCount;
	m_sCPUInfo.nFrequency = info.mMaxCpuSpeed;
	m_sCPUInfo.lReserved = info.mMinCpuSpeed;

	//VOLOGI("CPU Online Number is %d!", m_nCPUNumber);
#elif defined (_IOS)
    VO_CPU_Info info;
    get_cpu_info(&info);
    
    m_nCPUNumber = info.mCount;
    if (CPU_SUBTYPE_ARM_V7 <= info.mFeatures) {
        m_sCPUInfo.nCPUType = 1;
    }
    else {
        m_sCPUInfo.nCPUType = 0;
    }
    m_sCPUInfo.nCoreCount = info.mCount;
    m_sCPUInfo.nFrequency = info.mMaxCpuSpeed;
    m_sCPUInfo.lReserved = info.mMinCpuSpeed;
#endif // _LINUX_ANDROID
}

int voCMediaPlayer::Reset (void)
{
	Stop ();

	if (m_pVideoDecThread != NULL)
	{
		m_pVideoDecThread->Stop ();
		delete m_pVideoDecThread;
		m_pVideoDecThread = NULL;
	}

	if (m_pVideoThread != NULL)
	{
		m_pVideoThread->Stop ();
		delete m_pVideoThread;
		m_pVideoThread = NULL;
	}

	if (m_pAudioDecThread != NULL)
	{
		m_pAudioDecThread->Stop ();
		delete m_pAudioDecThread;
		m_pAudioDecThread = NULL;
	}

	if (m_pAudioThread != NULL)
	{
		m_pAudioThread->Stop ();
		delete m_pAudioThread;
		m_pAudioThread = NULL;
	}

#ifdef _WIN32
	if (m_pVideoRender != NULL && m_nVideoPlayMode == VOMP_PUSH_MODE)
		delete m_pVideoRender;
	m_pVideoRender = NULL;
#endif // _WIN32

	if (m_pVideoDec != NULL)
	{
		m_pVideoDec->Uninit ();
		delete m_pVideoDec;
		m_pVideoDec = NULL;
	}

	ReleaseVideoRenderBuffers ();

	if(m_pARBManager)
	{
		delete m_pARBManager;
		m_pARBManager = NULL;
	}

	if (m_pAudioRender != NULL)
	{
		delete m_pAudioRender;
		m_pAudioRender = NULL;
	}

	if (m_pAudioEffect != NULL)
	{
		m_pAudioEffect->Uninit ();
		delete m_pAudioEffect;
		m_pAudioEffect = NULL;
	}

	if (m_pAudioSpeed)
	{
		delete m_pAudioSpeed;
		m_pAudioSpeed = NULL;
	}	

	if (m_pVideoEffect != NULL)
	{
		m_pVideoEffect->Uninit ();
		delete m_pVideoEffect;
		m_pVideoEffect = NULL;
	}

	if (m_pAudioDec != NULL)
	{
		m_pAudioDec->Uninit ();
		delete m_pAudioDec;
		m_pAudioDec = NULL;
	}

	if (m_pDataSource != NULL)
	{
		m_pDataSource->CloseSource ();
		delete m_pDataSource;
		m_pDataSource = NULL;
	}
	else if (m_pFileSource != NULL)
	{
		m_pFileSource->CloseSource ();
		delete m_pFileSource;
		m_pFileSource = NULL;
	}
	if(m_pAudioDecOutPcmBuff != NULL)
	{
		delete [] m_pAudioDecOutPcmBuff;
		m_pAudioDecOutPcmBuff = NULL;
	}
	if(m_pAudioEftOutPcmBuff != NULL)
	{
		delete [] m_pAudioEftOutPcmBuff;
		m_pAudioEftOutPcmBuff = NULL;
	}

	vostrcpy (m_szURL, _T(""));
	m_nStatus = VOMP_STATUS_INIT;
	m_nOnEventTimes = 0;
	m_nDroppedVidooFrames = 0;

	m_pFileSource = NULL;
	m_pDataSource = NULL;
	memset (&m_sFilInfo, 0, sizeof (m_sFilInfo));
	m_nTrackCount = 0;
	m_nFF = VO_FILE_FFUNKNOWN;
	m_bDrmCrippled = VO_FALSE;
	m_nPlayStartPos = 0;
	m_nPlayStopPos = 0;

	m_bSetAudioDecoderThreadPriority = false;
	m_bSetAudioRendererThreadPriority = false;
	m_nAudioTrack = -1;
	m_sAudioDecOutFormat.SampleRate = 44100;
	m_sAudioDecOutFormat.Channels = 2;
	m_sAudioDecOutFormat.SampleBits = 16;
	m_sAudioEftOutFormat.SampleRate = 44100;
	m_sAudioEftOutFormat.Channels = 2;
	m_sAudioEftOutFormat.SampleBits = 16;
	m_sARBFormat.SampleRate = 44100;
	m_sARBFormat.Channels = 2;
	m_sARBFormat.SampleBits = 16;
	memset (&m_sLastAudioFmt, 0, sizeof (m_sLastAudioFmt));
	memset (&m_sAudioSample, 0, sizeof (m_sAudioSample));
	m_pAudioDec = NULL;
	memset (&m_sAudioDecInBuf, 0, sizeof (m_sAudioDecInBuf));
	memset (&m_sAudioDecOutBuf, 0, sizeof (m_sAudioDecOutBuf));
	m_nAudioDecSize = 0;
	m_nAudioDecOutFrameSize = 44100 * 4;
	m_nAudioEftOutFrameSize = 44100 * 4;
	m_nAudioRenderNum = 0;
	m_nAudioRndNuming = 0;

	m_pAudioRender = NULL;
	memset (&m_bufAudio, 9, sizeof (m_bufAudio));
	m_nStartPlayTime = 0;
	m_llAudioRenderTime = 0;
	m_llLastAudioRenderTime = 0;
	m_nAudioDecOutStepSize = 0;

	m_pAudioDecOutBuff = NULL;
	m_nAudioDecOutSize = 0;
	m_nAudioDecOutLength = 0;

	m_pAudioRndBuff = NULL;
	m_nAudioRndSize = 0;
	m_nAudioRndLength = 0;

	m_bSetVideoThreadPri = false;
	m_nVideoTrack = -1;
	memset (&m_sVideoFormat, 0, sizeof (m_sVideoFormat));
	m_sVideoFormat.Width = 0;
	m_sVideoFormat.Height = 0;
	memset (&m_sVideoSample, 0, sizeof (m_sVideoSample));
	m_pVideoDec = NULL;
	memset (&m_sVideoDecInBuf, 0, sizeof (m_sVideoDecInBuf));
	memset (&m_sVideoDecOutBuf, 0, sizeof (m_sVideoDecOutBuf));
	m_llLastVideoDecInTime = 0;
	m_llLastVideoDecOutTime = 0;
	memset (&m_bufVideo, 0, sizeof (m_bufVideo));
	memset (&m_fmtVideo, 0, sizeof (m_fmtVideo));
	m_nRCGetVideoTrackDataAfterPureAudio = VO_ERR_SOURCE_NEEDRETRY;
	m_bMoreVideoBuff = VO_FALSE;
	m_bNeedInputData = VO_FALSE;
	m_bVideoDecInputRetry = VO_FALSE;
	m_bVideoDecHungry = VO_FALSE;
	m_eRecreateDecoderState = VOMP_RECREATEDECODERSTATE_NONE;
	m_bFileDropFrame = VO_FALSE;
	m_bFrameDropped = VO_FALSE;
	m_nBFrameDropped = 0;
	m_nVideoRenderNum = 0;
	m_nVideoRndNuming = 0;
	m_nLastFrameTime = 0;
	m_nReceiveFrameTime = 0;
	m_nReceiveFrameNum = 0;
	m_nVideoDecoderNum = 0;
	m_bSetDispRect = VO_FALSE;
	m_bOnlyGetVideo = VO_FALSE;

    // delete the render before set it to null
    if (NULL != m_pVideoRender) {
        delete m_pVideoRender;
        m_pVideoRender = NULL;
    }
    
	m_bOutVideoRender = VO_TRUE;
	memset (&m_rgbBuffer, 0, sizeof (m_rgbBuffer));
	memset (&m_outVideoBuffer, 0, sizeof (m_outVideoBuffer));

	m_nStartSysTime = 0;
	m_nStartMediaTime = 0;
	m_nSeekPos = 0;
	m_bSeekingPos = VO_FALSE;
	m_llRenderBufferTime = 0;
	m_llRenderSystemTime = 0;
	m_bDataBuffering = VO_FALSE;

	m_bVideoEOS = VO_TRUE;
	m_bAudioEOS = VO_TRUE;

	if(m_nVideoRenderCount > 1)
		m_pVideoOutputBuff = NULL;

	m_nVideoOutFillIndex = 0;
	m_nVideoOutPlayIndex = 0;
	m_nRenderSuccess = 0;
	m_nVideoRndWidth = 0;
	m_nVideoRndHeight = 0;

	m_nLastVideoSysTime = 0;
	m_bSendStartVideoBuff = VO_FALSE;
	m_nLastAudioSysTime = 0;
	m_bSendStartAudioBuff = VO_FALSE;

	m_nStartPlayTime = 0;
	m_nLastVideoFrmTime = 0;
	m_nLastAudioFrmTime = 0;
	m_nLastAudioRndTime = 0;
	m_nLastAudioRndDifTime = 0;

	m_nVideoDcdTimeIndex = 0;
	m_nVideoRndTimeIndex = 0;
	m_nVideoDateCount = 0;
	m_nVideoCountStartTime = 0;

	m_nVideoDecTimes = 0;
	m_nVideoCCRTimes = 0;
	m_nVideoRndTimes = 0;
	m_nVideoRndLTime = 0;
	m_nAudioDecTimes = 0;

	m_bNotifyCompleted = false;
	m_bPauseRefClock = VO_FALSE;

	m_nStatus = VOMP_STATUS_INIT;
	
#if defined(_IOS) || defined(_MAC_OS)
	if(m_pLogoEffect)
		m_pLogoEffect->Flush();
#endif	
	
	// LinJun:to fix loading fail if streaming(HLS,SS) will be failed after local file playback failed last time
	m_sAudioTrackInfo.Codec = 0;
	m_sVideoTrackInfo.Codec = 0;

	if (m_pVideoLogoInfo)
	{
		m_pVideoLogoInfo->RestStasticPerfoData();
	}
	
	return 0;
}

int voCMediaPlayer::ReleaseVideoRenderBuffers (void)
{
	voCAutoLock lokc (&m_mtVideoRender);

	VO_U32 i;

	if (m_ppVideoOutBuffers != NULL)
	{
		for (i = 0; i < m_nVideoRenderCount; i++)
		{
			if (m_ppVideoOutBuffers[i] != NULL)
			{
				if (m_nVideoRndCopy > 0)
				{
					if (m_ppVideoOutBuffers[i]->Buffer[0] != NULL)
					{
						delete []m_ppVideoOutBuffers[i]->Buffer[0];
						m_ppVideoOutBuffers[i]->Buffer[0] = NULL;
					}
				}

				delete m_ppVideoOutBuffers[i];
				m_ppVideoOutBuffers[i] = NULL;
			}
		}

		delete []m_ppVideoOutBuffers;
		m_ppVideoOutBuffers = NULL;
	}

	m_ppVideoOutBuffers = NULL;

	if (m_ppVideoOutBufShared != NULL)
	{
		for (i = 0; i < m_nVideoRenderCount; i++)
		{
			if (m_ppVideoOutBufShared[i] != NULL)
			{

				if (m_ppVideoOutBufShared[i]->Buffer[0] != NULL)
				{
					delete []m_ppVideoOutBufShared[i]->Buffer[0];
					m_ppVideoOutBufShared[i]->Buffer[0] = NULL;
				}

				delete m_ppVideoOutBufShared[i];
				m_ppVideoOutBufShared[i] = NULL;
			}
		}

		delete []m_ppVideoOutBufShared;
		m_ppVideoOutBufShared = NULL;
	}

	m_ppVideoOutBufShared = NULL;

	if (m_pVideoOutWidth != NULL)
	{
		delete []m_pVideoOutWidth;
		m_pVideoOutWidth = NULL;
	}
	if (m_pVideoOutHeight != NULL)
	{
		delete []m_pVideoOutHeight;
		m_pVideoOutHeight = NULL;
	}
	m_pVideoOutWidth = NULL;
	m_pVideoOutHeight = NULL;


	m_nVideoOutFillIndex = 0;
	m_nVideoOutPlayIndex = 0;

	mAudioDecInputRetry = VO_FALSE;
	return 0;
}

int voCMediaPlayer::GetParam (int nID, void * pValue)
{
	if (nID == VOMP_PID_AUDIO_SAMPLE)
		return GetAudioBuffer ((VOMP_BUFFERTYPE **)pValue);
	else if (nID == VOMP_PID_VIDEO_SAMPLE)
		return GetVideoBuffer ((VOMP_BUFFERTYPE **)pValue);

	voCAutoLock lock (&m_mtStatus);
	if (pValue == NULL)
		return VOMP_ERR_Pointer;
	
	if (nID == VOMP_PID_VIDEO_RENDER_CONTEXT)
	{
#ifdef _IOS
        if (m_pVideoRender != NULL) {
			return m_pVideoRender->GetParam(VO_VR_PMID_RENDER_CONTEXT, pValue);
		}
        
		return VOMP_ERR_None;
#else
		return VOMP_ERR_Implement;
#endif
	}
	else if(nID == VOMP_PID_CPU_INFO)
	{
		VOMP_CPU_INFO *pCPUInfo = (VOMP_CPU_INFO *)pValue;

		pCPUInfo->nCoreCount   = m_sCPUInfo.nCoreCount;
		pCPUInfo->nCPUType   = m_sCPUInfo.nCPUType;
		pCPUInfo->nFrequency   = m_sCPUInfo.nFrequency;
		pCPUInfo->lReserved   = m_sCPUInfo.lReserved;

		return VOMP_ERR_None;
	}
	
	if (m_nStatus <= VOMP_STATUS_LOADING)
		return VOMP_ERR_Status;

	if (nID == VOMP_PID_AUDIO_FORMAT)
	{
		if(m_pAudioEffect)
		{
			memcpy(pValue, &m_sAudioEftOutFormat, sizeof(VO_AUDIO_FORMAT));
			VOLOGI("VOMP_PID_AUDIO_FORMAT Eft %d %d %d", m_sAudioEftOutFormat.SampleRate, m_sAudioEftOutFormat.Channels, m_sAudioEftOutFormat.SampleBits);
		}
		else
		{
			memcpy(pValue, &m_sAudioDecOutFormat, sizeof(VO_AUDIO_FORMAT));
			VOLOGI("VOMP_PID_AUDIO_FORMAT Dec %d %d %d", m_sAudioDecOutFormat.SampleRate, m_sAudioDecOutFormat.Channels, m_sAudioDecOutFormat.SampleBits);
		}

		return VOMP_ERR_None;
	}
	else if(VOMP_PID_AUDIO_RENDER_DATA_FORMAT == nID)
	{
		// added by gtxia 2013-9-14
		*(int*)pValue = VO_AUDIO_CodingPCM;
		if(m_pAudioDec)
			*(int*) pValue = (int)m_pAudioDec->getOutputType();		
	}
	else if (nID == VOMP_PID_VIDEO_FORMAT)
	{
		((VO_VIDEO_FORMAT *)pValue)->Width = m_nVideoRndWidth;
		((VO_VIDEO_FORMAT *)pValue)->Height = m_nVideoRndHeight;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_VIDEO_DIMENSION) 
	{
		if (m_pVideoDec) 
			m_pVideoDec->GetParam(VO_PID_VIDEO_DIMENSION, pValue);
	}
	else if (nID == VOMP_PID_AUDIO_VOLUME)
	{
		*(int *)pValue = m_nAudioVolume;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_AUDIO_PLAYBACK_SPEED)
	{
		*(float *)pValue = (float)(m_nAudioPlaySpeed / 10);
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_MAX_BUFFTIME)
	{
		if (m_pDataSource == NULL)
			return VOMP_ERR_Status;
		*(int *)pValue = (int)m_pDataSource->GetMaxBuffTime ();
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_MIN_BUFFTIME)
	{
		if (m_pDataSource == NULL)
			return VOMP_ERR_Status;
		*(int *)pValue = (int)m_pDataSource->GetMinBuffTime ();
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_AUDIO_BUFFTIME)
	{
		if (m_pDataSource == NULL)
			return VOMP_ERR_Status;
		*(int *)pValue = (int)m_pDataSource->GetBufferTime (true);
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_VIDEO_BUFFTIME)
	{
		if (m_pDataSource == NULL)
			return VOMP_ERR_Status;
		*(int *)pValue = (int)m_pDataSource->GetBufferTime (false);
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_SEEK_MODE)
	{
		*(int *)pValue = m_nSeekMode;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_AUDIO_SINK_BUFFTIME)
	{
		*(int *)pValue = (int)m_nAudioBuffTime;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_AUDIO_STEP_BUFFTIME)
	{
		*(int *)pValue = m_nAudioStepTime;
		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_AUDIO_REND_ADJUTIME)
	{
		*(int *)pValue = (int)m_llRenderAdjustTime;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_AUDIO_CODEC)
	{
		if (m_pAudioDec == NULL)
			*(int *)pValue = VOMP_AUDIO_CodingUnused;
		else
			*(int *)pValue = m_sAudioTrackInfo.Codec;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_VIDEO_CODEC)
	{
		if (m_pVideoDec == NULL)
			*(int *)pValue = VOMP_VIDEO_CodingUnused;
		else
			*(int *)pValue = m_sVideoTrackInfo.Codec;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_START_POS)
	{
		*(VO_U32 *)pValue = (VO_U32)m_nPlayStartPos;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_STOP_POS)
	{
		*(VO_U32 *)pValue = (VO_U32)m_nPlayStopPos;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_VIDEO_RNDNUM)
	{
		*(VO_S32 *)pValue = m_nVideoRenderCount;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_VIDEO_RNDCOPY)
	{
		*(VO_S32 *)pValue = m_nVideoRndCopy;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_AUDIO_EFFECT_ENABLE)
	{
		*(VO_S32 *)pValue = m_bEffectEnable;
		return VOMP_ERR_None;
	}
	else if (nID >= VOMP_PID_METADATA_TITLE && nID <= VOMP_PID_METADATA_MAX)
	{
		int nRC = GetMetaData (nID, pValue);
		if (nRC > 0)
		{
			if (nID == VOMP_PID_METADATA_FRONTCOVER)
				return nRC;

			*(char **)pValue = m_szMetaText;
			return nRC;
		}
		else
		{
			return VOMP_ERR_ParamID;
		}
	}
	else if(nID == VOMP_PID_VIDEO_EFFECT_TYPE)
	{
		m_pVideoEffectType = *((VOMP_VIDEO_EFFECTTYPE *)pValue) = m_pVideoEffectType;
		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_VIDEO_EFFECT_ONOFF)
	{
		*(VO_BOOL *)pValue = m_bVideoEffectOn;
		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_VIDEO_VIDEO_UPSIDE)
	{
		*(VO_BOOL *)pValue = VO_FALSE;

		if(m_sVideoTrackInfo.Codec == VO_VIDEO_CodingVP6 && m_pDataSource)
		{
			m_pDataSource->GetTrackParam(m_nVideoTrack, VO_PID_VIDEO_UPSIDEDOWN, pValue);
		}

		return VOMP_ERR_None;		
	}
	else if(nID == VOMP_PID_AUDIO_DEC_BITRATE)
	{
		memcpy((int *)pValue, m_nAudioBitrate, sizeof(int)*10);
		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_VIDEO_DEC_BITRATE)
	{
		memcpy((int *)pValue, m_nVideoBitrate, sizeof(int)*10);
		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_VIDEO_PERFORMANCE_OPTION)
	{
		if(m_pVideoLogoInfo)
			return m_pVideoLogoInfo->GetParam(nID, pValue);

		return VOMP_ERR_Implement;
	}

	return VOMP_ERR_ParamID;
}

int voCMediaPlayer::SetParam (int nID, void * pValue)
{
	voCAutoLock lock (&m_mtStatus);

	if (nID == VOMP_PID_DRAW_VIEW)
	{
#ifndef _MAC_OS
		m_hView = pValue;
		if (m_pVideoRender != NULL)
			m_pVideoRender->SetDispRect (m_hView, (VO_RECT *)&m_rcDraw, (VO_IV_COLORTYPE)m_nColorType);

		if(m_pVideoDec != NULL && pValue != NULL)
		{
			//New view we need destroy decoder and recreate it.
			CreateVideoDec();
			if(m_nLastVideoFrmTime >= 0)
			{
				m_bSeekingPos = VO_TRUE;
				m_nSeekPos = m_nLastVideoFrmTime;
			}
//			m_pVideoDec->SetParam(VO_PID_IOMXDEC_SetSurface, m_hView);
		}
#else
		bool bChanged = (m_hView != pValue && m_hView);
		m_hView = pValue;
		if(bChanged)
		{
			if (m_pVideoRender != NULL)
				delete m_pVideoRender;
			m_pVideoRender = NULL;
			
			if(!m_pVideoRender)
			{
#ifdef _MAC_OS_CG
				m_pVideoRender = new CCGRender(NULL, m_hView, NULL);
#else
				m_pVideoRender = new CMacOSVideoRender (NULL, m_hView, NULL);
#endif
			}

			// tag:20110210 added by lin jun
			if (m_pVideoRender != NULL)
			{
				m_pVideoRender->SetVideoInfo (m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
				m_pVideoRender->SetParam (VO_PID_COMMON_LIBOP, (VO_LIB_OPERATOR *)m_pLibFunc);
				VO_RECT r;
				r.left=m_rcDraw.left;r.top=m_rcDraw.top;r.right=m_rcDraw.right;r.bottom=m_rcDraw.bottom;
				m_pVideoRender->SetDispRect (m_hView, &r, (VO_IV_COLORTYPE)m_nColorType);
			}
			
			m_hView = pValue;
			//printf("render thread start use time = %d\n", voOS_GetSysTime() - t);
		}
#endif
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_IOMX_PROBE)
	{
		if(pValue != NULL)
		{
			m_pVideoDec = new CVideoDecoder (m_hInst, VO_VIDEO_CodingH264, &g_memOP);

#ifdef _WIN32
			m_pVideoDec->SetVideoLibFile(VO_VIDEO_CodingH264, _T("voIOMXDec_ics"));
			m_pVideoDec->SetVideoAPIName(VO_VIDEO_CodingH264, _T("voGetIOMXDecAPI"));
#elif defined LINUX
			VO_TCHAR szTmp[256];
			vostrcpy(szTmp, (VO_PTCHAR)"voIOMXDec_ics");
			m_pVideoDec->SetVideoLibFile(VO_VIDEO_CodingH264, szTmp);
			vostrcpy(szTmp, (VO_PTCHAR)"voGetIOMXDecAPI");
			m_pVideoDec->SetVideoAPIName(VO_VIDEO_CodingH264, szTmp);
#endif // _WIN32

			m_pVideoDec->SetWorkPath(m_szWorkPath);
			VO_VIDEO_FORMAT t_format;
			t_format.Height = 240;
			t_format.Width  = 320;
			m_pVideoDec->Init(NULL, 0, &t_format);
			int ret = m_pVideoDec->SetParam(VO_PID_IOMXDEC_PROBE, pValue);
			return ret;
		}
	}
	else if (nID == VOMP_PID_DRAW_RECT)
	{
		if(pValue)
			memcpy (&m_rcDraw, pValue, sizeof (m_rcDraw));
		if (m_pVideoRender != NULL)
			m_pVideoRender->SetDispRect (m_hView, (VO_RECT *)&m_rcDraw, (VO_IV_COLORTYPE)m_nColorType);

		if(m_pVideoDec != NULL && m_nColorType == VO_COLOR_IOMX_PRIVATE)
		{
			VOMP_RECT rcDraw;
			int nRC = getDrawRect(rcDraw);
			if(VOMP_ERR_None != nRC)
				return nRC;

			m_pVideoDec->SetParam(VO_PID_IOMXDEC_SetCrop, &rcDraw);
		}
		m_bSetDispRect = VO_TRUE;
		
		VOLOGI("VOMP_PID_DRAW_RECT l: %d, t: %d, r: %d, b: %d", m_rcDraw.left, m_rcDraw.top, m_rcDraw.right, m_rcDraw.bottom);

		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_ZOOM_MODE)
	{
		if(pValue)
		{
			m_nZoomType = *((VO_IV_ZOOM_MODE *)pValue);

			if (m_pVideoRender != NULL)
				m_pVideoRender->SetDispType(m_nZoomType, VO_RATIO_00);

			VOLOGI("VOMP_PID_ZOOM_MODE %d", m_nZoomType);
			if(m_pVideoDec &&  m_nColorType == VO_ACODEC_COLOR_TYPE)
			{
				int nMode = 1;
				if(VO_ZM_ZOOMIN == m_nZoomType || VO_ZM_PANSCAN == m_nZoomType)
					nMode = 2;
				m_pVideoDec->SetParam(VO_ACODEC_SMODE, &nMode);
			}
		}

		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_DRAW_COLOR)
	{
		// when new color type was set and current type is HW, we should erase decoder info.
		if(NULL != pValue && m_nColorType != *(int *)pValue && (m_nColorType == VO_COLOR_IOMX_PRIVATE || m_nColorType == VO_ACODEC_COLOR_TYPE))
		{
			memset(m_sSetVideoName, 0, sizeof(VO_TCHAR)*256);
			memset(m_sSetVideoAPI, 0, sizeof(VO_TCHAR)*256);
			m_nSetVideoNameType = m_nSetVideoAPIType = 0;                                                                                                                                                       
		}

		if(pValue)     
			m_nColorType = *(int *)pValue;
#ifdef _IOS
        if (m_pVideoRender != NULL) {
            m_pVideoRender->SetDispRect (m_hView, (VO_RECT *)&m_rcDraw, (VO_IV_COLORTYPE)m_nColorType);
        }
#endif
		VOLOGI ("m_nColorType is %d", m_nColorType);

#ifdef __SYMBIAN32__
		if (m_pVideoRender == NULL && m_nColorType >= VOMP_COLOR_RGB565_PACKED)
		{
			m_pVideoRender = new CBaseVideoRender (m_hInst, m_hView, NULL);
			if (m_pVideoRender != NULL)
			{
				m_pVideoRender->SetWorkPath (m_szWorkPath);
				m_pVideoRender->SetParam (VO_PID_COMMON_LIBOP, (VO_LIB_OPERATOR *)m_pLibFunc);
				if (m_rcDraw.bottom == 0 || m_rcDraw.right == 0)
				{
					m_rcDraw.left = 0; m_rcDraw.right = m_sVideoFormat.Width;
					m_rcDraw.top = 0; m_rcDraw.bottom = m_sVideoFormat.Height;
				}
				m_pVideoRender->SetVideoInfo (m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
				m_pVideoRender->SetDispRect (m_hView, (VO_RECT *)&m_rcDraw, (VO_IV_COLORTYPE)m_nColorType);
			}
		}
#endif // __SYMBIAN32__
		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_VIDEO_REDRAW || nID == VOMP_PID_APPLICATION_RESUME || nID == VOMP_PID_VIDEO_ONDRAW) 
	{
		if(m_pVideoRender)
			m_pVideoRender->Redraw();

        m_bRedrawVideo = VO_TRUE;

		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_LOW_LATENCE)
	{
		if(pValue)
			m_nLowLatence = *(int *)pValue;

		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_VIDEO_TRACK)
	{
		m_sVideoTrackInfo.Codec = *(int*)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_AUDIO_TRACK)
	{
		m_sAudioTrackInfo.Codec = *(int*)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_AUDIO_PLAYMODE)
	{
		m_nAudioPlayMode = *(int*)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_VIDEO_PLAYMODE)
	{
		m_nVideoPlayMode = *(int*)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_AUDIO_ONLY)
	{
		m_bAudioOnly = *(int*)pValue > 0;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_VIDEO_ONLY)
	{
		m_bVideoOnly = *(int*)pValue > 0;
		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_AUDIO_STREAM_ONOFF)
	{
		if(*(int*)pValue == 0 && m_nVideoOnOff == 0)
			return VOMP_ERR_Implement;

		//m_nAudioOnOff = *(int*)pValue;
		int nAudioOnOFF = *(int*)pValue;
		if (nAudioOnOFF)
			m_bRenderAudioOnOff = VO_TRUE;
		else
			m_bRenderAudioOnOff = VO_FALSE;

		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_VIDEO_STREAM_ONOFF)
	{
		if(*(int*)pValue == 0 && m_nAudioOnOff == 0)
			return VOMP_ERR_Implement;

		m_nVideoOnOff = *(int*)pValue;
		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_VIDEO_PLAY_BACK)
	{
		VOLOGI ("m_nVideoPlayBack is %d", (int)pValue);
		if(m_nAudioOnOff == 0 && (int)pValue == 0)
			return VOMP_ERR_Implement;

		if(m_nVideoPlayBack == 0 && pValue)
			m_nIFrameSkip = 1;

		m_nVideoPlayBack = (int)pValue;

		if(m_nVideoPlayBack == 0)
		{
			cancelBuffersFromVOVideoDec();
			if(m_pVideoDec)
			{
				m_pVideoDec->Flush();
			}
		}

		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_AUDIO_VOLUME)
	{
		m_nAudioVolume = *(int *)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_MAX_BUFFTIME)
	{
		if (m_pDataSource == NULL)
			return VOMP_ERR_Status;
		 m_pDataSource->SetMaxBuffTime (*(int *)pValue);
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_MIN_BUFFTIME)
	{
		if (m_pDataSource == NULL)
			return VOMP_ERR_Status;
		m_pDataSource->SetMinBuffTime (*(int *)pValue);
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_SEEK_MODE)
	{
		m_nSeekMode = *(int *)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_THREAD_CREATE)
	{
		m_fThreadCreate = (VOTDThreadCreate)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_FUNC_LIB)
	{
		m_pLibFunc = (VOMP_LIB_FUNC *)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_FUNC_FILE)
	{
		VOLOGI("VOMP_PID_FUNC_FILE %x", VOMP_PID_FUNC_FILE);
		m_pFileOP = (VO_FILE_OPERATOR *)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_FUNC_DRM)
	{
		VOLOGI("VOMP_PID_FUNC_DRM %x", VOMP_PID_FUNC_DRM);
		m_pDrmCB = (VOMP_SOURCEDRM_CALLBACK *)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_DISABLE_DROPFRAME)
	{
		m_nDisableDropVideoFrame = *(int *)pValue;
		if (m_nDisableDropVideoFrame > 0)
			m_nVideoDropRender = 0;
		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_VIDEO_PERFORMANCE_OPTION)
	{
		if(m_pVideoLogoInfo)
			return m_pVideoLogoInfo->SetParam(nID, pValue);

		return VOMP_ERR_Implement;
	}
	else if(nID == VOMP_PID_VIDEO_PERFORMANCE_ONOFF)
	{
		m_nVideoPerfOnOff = *((int *)pValue);

		if(m_pVideoLogoInfo)
			return m_pVideoLogoInfo->SetParam(nID, pValue);	

		return VOMP_ERR_Implement;
	}
	else if (nID == VOMP_PID_VIDEO_DISABLE_DEBLOCK)
	{
		m_nDisableDeblockVideo = *(int *)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_AUDIO_EFFECT_ENABLE)
	{
		if (*(int *)pValue > 0)
			m_bEffectEnable = VO_TRUE;
		else
			m_bEffectEnable = VO_FALSE;

		if (m_pAudioEffect != NULL)
			m_pAudioEffect->Enable (m_bEffectEnable);

		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_AUDIO_EFFECT_MODULE)
	{
		if (*(int *)pValue > 0)
			m_bAddAudioEffect = VO_TRUE;
		else
			m_bAddAudioEffect = VO_FALSE;

		return VOMP_ERR_None;
	}
	//rogine add for #31875 from CJ
	else if(nID == VOMP_PID_LOAD_DOLBY_DECODER_MODULE)
	{
		if (*(int *)pValue > 0)
			m_bAllowDolbyAudio = VO_TRUE;
		else
			m_bAllowDolbyAudio = VO_FALSE;

		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_DISABLE_DROPRENDER)
	{
		m_nVideoDropRender = *(int *)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_CHECK_PERFORMANCE)
	{
		m_nCheckFastPerformance = *(int *)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_GET_THUMBNAIL)
	{
		m_nGetThumbnail = *(int *)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_START_POS)
	{
		m_nPlayStartPos = (VO_U64)(*(VO_U32 *)pValue);
		if (m_pFileSource != NULL)
		{
			if (m_nPlayStartPos > (VO_U64)m_pFileSource->GetDuration ())
				m_nPlayStartPos = (VO_U64)m_pFileSource->GetDuration ();

			if (m_nPlayStopPos == 0)
				m_nPlayStopPos = (VO_U64)m_pFileSource->GetDuration ();

			if (m_nPlayStartPos > 0)
				SetCurPos (0);
		}
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_STOP_POS)
	{
		m_nPlayStopPos = (VO_U64)(*(VO_U32 *)pValue);
		if (m_pFileSource != NULL)
		{
			if (m_nPlayStartPos > (VO_U64)m_pFileSource->GetDuration ())
				m_nPlayStopPos = (VO_U64)m_pFileSource->GetDuration ();
		}
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_AUDIO_RNDNUM)
	{
		VO_U32 nAudioRenderBufferCount = *((VO_U32 *)pValue);
		VOLOGI("VOMP_PID_AUDIO_RNDNUM %d", nAudioRenderBufferCount);

		if(nAudioRenderBufferCount <= 0 || nAudioRenderBufferCount > 16)
			return VOMP_ERR_ParamID;

		if(m_nAudioRenderBufferCount == nAudioRenderBufferCount)
			return VOMP_ERR_None;

		if(m_pARBManager)
		{
			delete m_pARBManager;
			m_pARBManager = NULL;
		}

		m_nAudioRenderBufferCount = nAudioRenderBufferCount;

		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_VIDEO_RNDNUM)
	{
		if (*(VO_S32 *)pValue <= 0 || *(VO_S32 *)pValue > 32)
			return VOMP_ERR_ParamID;

		if (m_nVideoRenderCount == *(VO_U32 *)pValue)
			return VOMP_ERR_None;

		ReleaseVideoRenderBuffers ();

		m_nVideoRenderCount = *(VO_U32 *)pValue;

		VOLOGI("VOMP_PID_VIDEO_RNDNUM %d", m_nVideoRenderCount);

		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_VIDEO_RNDCOPY)
	{
		m_nVideoRndCopy = *(int *)pValue;
		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_VIDEO_REDRAW || nID == VOMP_PID_APPLICATION_RESUME || nID == VOMP_PID_VIDEO_ONDRAW) 
	{
		if (m_pVideoRender != NULL)
			m_pVideoRender->Redraw ();

        m_bRedrawVideo = VO_TRUE;

		return VOMP_ERR_None;
	}
	else if(VOMP_PID_VIDEO_RENDER_TYPE == nID)
	{
		return VOMP_ERR_Implement;
	}
	else if (nID == VOMP_PID_AUDIO_SINK_BUFFTIME)
	{
		m_nAudioBuffTime = (VO_U64)(*(int *)pValue);
#if defined(_IOS) || defined(_MAC_OS)
		m_bAudioBufFull = VO_TRUE;
#endif
		VOLOGI("VOMP_PID_AUDIO_SINK_BUFFTIME %lld", m_nAudioBuffTime);
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_AUDIO_STEP_BUFFTIME)
	{
		m_nAudioStepTime = *(int *)pValue;

		if(m_pAudioEffect)
			UpdateAudioFormat(&m_sAudioEftOutFormat);
		else
			UpdateAudioFormat(&m_sAudioDecOutFormat);

		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_AUDIO_REND_ADJUTIME)
	{
		m_llRenderAdjustTime = (VO_S64)(*(int *)pValue);
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_AUDIO_GETONLY)
	{
		m_bOnlyGetAudio = *(VO_BOOL *)pValue;
		return VOMP_ERR_None;
	}	
	else if(nID == VOMP_PID_AUDIO_PLAYBACK_SPEED)
	{
		if (pValue)
		{
			VOLOGI("@@@####spd Set audio playback speed %1.3f from APP", *(float*)pValue);
			VO_S32 nSpeedVal = (VO_S32)(((float)(*(float*)pValue) + 1.0e-4) * 10);
			if (nSpeedVal <= 0)
			{
				//if the value over the define range, the speed will keep the original
				nSpeedVal = m_nAudioPlaySpeed;
			}
			else if (nSpeedVal != 10)
			{			
				if (m_pAudioSpeed == NULL || m_pAudioDec == NULL)
				{
					//m_nAudioPlaySpeed = nSpeedVal;
					VOLOGE("@@@####spd Set audio speed %d not enable! Wait for reset after create audio decoder", nSpeedVal);
					return VOMP_ERR_Retry;
				}

				VO_AUDIO_FORMAT * pAudioOutFormat = m_pAudioEffect ? &m_sAudioEftOutFormat : &m_sAudioDecOutFormat;
				m_pAudioSpeed->SetFormat(pAudioOutFormat);
				int nChannel = pAudioOutFormat->Channels;
				if (nChannel > 2)
					nChannel = 2;
				VOLOGI("@@@####spd m_pAudioDec->SetParam (VO_PID_AUDIO_CHANNELS, %d)", nChannel);
				m_pAudioDec->SetParam (VO_PID_AUDIO_CHANNELS, &nChannel);				
				m_bASpeedEnable = VO_TRUE;
				VOLOGI("@@@####spd m_bASpeedEnable: %d", (int)m_bASpeedEnable);
			}
			else
			{
				if(m_bASpeedEnable == VO_TRUE && m_pAudioDec != NULL)
				{
					int nChannel = (int)m_sLastAudioFmt.Channels;
					VOLOGI("@@@####spd m_pAudioDec->SetParam (VO_PID_AUDIO_CHANNELS, %d)", nChannel);
					m_pAudioDec->SetParam (VO_PID_AUDIO_CHANNELS, &nChannel);
				}				
				m_bASpeedEnable = VO_FALSE;
				VOLOGI("@@@####spd m_bASpeedEnable: %d", (int)m_bASpeedEnable);
			}						
			
			VOLOGI("@@@####spd Set audio playback speed %d in vompEng", nSpeedVal);
			m_nAudioPlaySpeed = nSpeedVal;
			if (m_pAudioSpeed != NULL)
			{
				m_pAudioSpeed->SetParam(VO_PID_AUDIOSPEED_RATE, &m_nAudioPlaySpeed);
			}
			
		}
		
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_PLAY_CONFIG_FILE)
	{
		OpenPlayConfigFile ((VO_TCHAR *)pValue);
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_VOSDK_CONFIG_FILE)
	{
		OpenVOSDKConfigFile ((VO_TCHAR *)pValue);
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_LICENSE_TEXT)
	{
		if(pValue)
        {
            strcpy (m_szLicenseText, (char *)pValue);
#if defined(_IOS) || defined(_MAC_OS)
            
#ifdef _USE_VO_LOGO
            CheckLicense((VO_TCHAR*)m_szLicensePath);
#elif defined _USE_VO_LOGOEXT
            if (m_pLogoEffect) {
                m_pLogoEffect->SetKey(m_szLicenseText);
            }
#endif
            
#endif
        }
		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_LICENSE_FILE_PATH)
	{
		if (pValue)
        {
			vostrcpy (m_szLicensePath, (VO_TCHAR *)pValue);
            
#if defined(_IOS) || defined(_MAC_OS)
            
#ifdef _USE_VO_LOGO
            CheckLicense((VO_TCHAR*)m_szLicensePath);
#elif defined _USE_VO_LOGOEXT
            if (m_pLogoEffect) {
                m_pLogoEffect->SetLicenseFilePath(m_szLicensePath);
            }
#endif
            
#endif
        }
        
		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_PLAYER_PATH)
	{
		if(pValue)
			vostrcpy (m_szWorkPath, (VO_TCHAR *)pValue);

		VOLOGI("m_szWorkPath %s", (int)m_szWorkPath);

		if ( vostrlen(m_szWorkPath) > 0 && m_pVideoLogoInfo )
			m_pVideoLogoInfo->SetWorkPath(m_szWorkPath);

		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_LICENSE_DATA)
	{
		if(pValue)
		{
			if(m_pLicenseData)
			{
				delete []m_pLicenseData;
				m_pLicenseData = NULL;
			}

			m_pLicenseData = new char[32768+32];
			if(m_pLicenseData == NULL)
				return VOMP_ERR_OutMemory;
			memset(m_pLicenseData, 0, 32768+32);
			memcpy (m_pLicenseData, (char *)pValue, 32768*sizeof(char));

#if defined(_IOS) || defined(_MAC_OS)
#ifdef _USE_VO_LOGO
#elif defined _USE_VO_LOGOEXT
            if (m_pLogoEffect) {
                m_pLogoEffect->SetLicenseContent(m_pLicenseData);
            }
#endif
            
#endif
		}

		return VOMP_ERR_None;		
	}
	else if (nID == VOMP_PID_IDLE_THREAD)
	{
		m_nIdleThdPriority = *(int *)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_CPU_NUMBER)
	{
		m_nCPUNumber = *(int *)pValue;
		
		//if(m_nCPUNumber >= 4)
		//	m_nCPUVersion = 7;

		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_VIDEO_DRM_FUNC)
	{
		m_pVidDecCB = pValue;
		if (m_pVideoDec != NULL)
			m_pVideoDec->SetParam (VO_PID_VIDEO_DRM_FUNC, m_pVidDecCB);
		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_VIDEO_EFFECT_TYPE)
	{
		m_pVideoEffectType = *((VOMP_VIDEO_EFFECTTYPE *)pValue);
		if (m_pVideoEffect != NULL)
			m_pVideoEffect->SetVideoEffect ((VO_VIDEO_EFFECTTYPE)m_pVideoEffectType);

		if(m_pVideoEffectType == VOMP_VE_CloseCaptionOn)
		{
			VO_H264_SEIFLAG val = VHSF_GET_T35_USERDDATA;
			if(m_pVideoDec != NULL)
				m_pVideoDec->SetParam(VO_ID_H264_ENABLE_SEI, &val);
		}

		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_VIDEO_EFFECT_ONOFF)
	{
		m_bVideoEffectOn = *(VO_BOOL *)pValue;
		if (m_pVideoEffect != NULL)
			m_pVideoEffect->Enable (m_bVideoEffectOn);

		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_SOCKET_CONNECTION_TYPE)
	{
		m_nSocketConnectionType = (*(int*)pValue);
		if(m_pFileSource)
			m_pFileSource->SetSourceParam(VOMP_PID_SOCKET_CONNECTION_TYPE, &m_nSocketConnectionType);
		
		VOLOGI("Socket connection type is %d", (int)m_nSocketConnectionType);

		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_STREAMDIGEST_AUTHEN_N)
	{
		if(pValue)
			strcpy(m_sDigestName, (char *)pValue);

		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_STREAMDIGEST_AUTHEN_P)
	{
		if(pValue)
			strcpy(m_sDigestPass, (char *)pValue);

		return VOMP_ERR_None;
	}
	else if(nID == VO_PID_COMMON_LOGFUNC)
	{
		if(pValue)
			memcpy(&m_cbVOLOG, pValue, sizeof(VO_LOG_PRINT_CB));

		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_VIDEO_PERFORMANCE_CACHE)
	{
		if(m_pVideoLogoInfo)
			m_pVideoLogoInfo->SetParam(nID, pValue);

		return VOMP_ERR_None;
	}
	else if((nID & 0xFFFF0000) == VOMP_PID_AUDIO_CODEC_FILENAME)
	{
		m_nSetAudioNameType = nID&0xFFFF;
		if(pValue)
		{
			vostrcpy(m_sSetAudioName, (VO_PTCHAR)pValue);
			bool isAMCDec = !vostrcmp(m_sSetAudioName, _T("voAudioMCDec"));
			{
				if(isAMCDec)
					m_nAudioRenderBufferCount = 6;
			}
			VOLOGI("m_nAudioRenderBufferCount = %d\n", m_nAudioRenderBufferCount);
		}

		return VOMP_ERR_None;
	}
	else if((nID & 0xFFFF0000) == VOMP_PID_AUDIO_CODEC_APINAME)
	{
		m_nSetAudioAPIType = nID&0xFFFF;
		if(pValue)
			vostrcpy(m_sSetAudioAPI, (VO_PTCHAR)pValue);

		return VOMP_ERR_None;
	}
	else if((nID & 0xFFFF0000) == VOMP_PID_VIDEO_CODEC_FILENAME)
	{
		m_nSetVideoNameType = (nID&0xFFFF);
		if(pValue)
			vostrcpy(m_sSetVideoName, (VO_PTCHAR)pValue);

		return VOMP_ERR_None;
	}
	else if((nID & 0xFFFF0000) == VOMP_PID_VIDEO_CODEC_APINAME)
	{
		m_nSetVideoAPIType = nID&0xFFFF;
		if(pValue)
			vostrcpy(m_sSetVideoAPI, (VO_PTCHAR)pValue);

		return VOMP_ERR_None;
	}
	else if(nID == VOMP_PID_JAVA_VM)
	{
		mJavaVM = pValue;
		return VOMP_ERR_None;
	}
	else if(VOMP_PID_AUDIO_PCM_OUTPUT == nID)
	{
		m_bSendAudioBuffer = 1 == (*(int*)pValue) ? VO_TRUE : VO_FALSE;
		return VOMP_ERR_None;
	}
	else if(VOMP_PID_DRM_UNIQUE_IDENTIFIER == nID) 
	{
		strcpy (m_szUUID, (char*)pValue);
		VOLOGI("@@@###, SetUUID :  %s  to VOME2", m_szUUID);
		return VOMP_ERR_None;
	}
	// added by gtxia for support ing Audio_DSP_Clock
	else if(VOMP_PID_AUDIO_DSP_CLOCK == nID)
	{
		mADSPClock = (IAudioDSPClock*)pValue;
		m_llDSPRefTime = -1; 
		return VOMP_ERR_None;
	}
	else if(VOMP_PID_PAUSE_REFCOLOK == nID)
	{
		voCAutoLock lockGetPlayTime(&m_mtPlayTime);

		m_bPauseRefClock = *((VO_BOOL *)pValue);
		VOLOGI("VOMP_PID_PAUSE_REFCOLOK %d", m_bPauseRefClock);

		return VOMP_ERR_None;
	}
	else if(VOMP_PID_APPLICATION_SUSPEND == nID)
	{
		mIsSuspend = VO_TRUE;
	}
	else if(VOMP_PID_SIGNAL_BUFFER_RETURNED == nID)
	{
		// just handle for new video decoder API
/*		if(!m_bNewVOVideoDec)
			return VOMP_ERR_None;

		VOMP_SIGNALBUFFERRETURNED * pSbr = (VOMP_SIGNALBUFFERRETURNED *)pValue;
		VOLOGI("VOMP_PID_SIGNAL_BUFFER_RETURNED type %d, rendered %d, buffer 0x%08X", pSbr->nTrackType, pSbr->nRendered, pSbr->pBuffer);
		if(VOMP_SS_Video == pSbr->nTrackType && pSbr->pBuffer)
		{
			if(m_pVideoDec)
			{
				VO_VIDEO_BUFFER * pVideoBuffer = (VO_VIDEO_BUFFER *)pSbr->pBuffer->pBuffer;
				if(pVideoBuffer)
				{
					VOLOGI("codec data 0x%08X", pVideoBuffer->CodecData);
					if(pVideoBuffer->CodecData)
						m_pVideoDec->SetParam(VO_PID_COMMON_FRAME_BUF_BACK, pVideoBuffer);
				}
			}
		}
*/
		return VOMP_ERR_None;
	}

	return VOMP_ERR_ParamID;
}

int voCMediaPlayer::LoadSource (void)
{	
	m_nStatus = VOMP_STATUS_LOADING;

	VO_U32	i = 0;
	if (m_nVideoRenderCount == 1)
		m_pVideoOutputBuff = &m_sVideoDecOutBuf;

	if (m_pFileOP == NULL)
	{
		cmnFileFillPointer ();
		m_pFileOP = (VO_FILE_OPERATOR *)&g_fileOP;
	}

	VO_PTCHAR			pURL = NULL;

	m_nAudioVideoUnsupport = 0;
	VO_FILE_SOURCE filSource;
	memset (&filSource, 0, sizeof (VO_FILE_SOURCE));
	filSource.nMode = VO_FILE_READ_ONLY;

	VO_SOURCEDRM_CALLBACK cbSourceDrm;
	if(m_pDrmCB)
	{
		cbSourceDrm.fCallback = (VOSOURCEDRMCALLBACK)m_pDrmCB->fCallback;
		cbSourceDrm.pUserData = m_pDrmCB->pUserData;
	}
	else
	{
		cbSourceDrm.fCallback = NULL;
		cbSourceDrm.pUserData = NULL;
	}

	m_pFileSource = NULL;
	if (m_nOpenFlag & VOMP_FLAG_SOURCE_URL)
	{
		filSource.nFlag = VO_FILE_TYPE_NAME;
		pURL = vostrstr (m_szURL, _T("http://"));
		if (pURL == NULL)
			pURL = vostrstr (m_szURL, _T("rtsp://"));
		filSource.pSource = m_szURL;

		VO_TCHAR szURL[1024*2];
		vostrcpy (szURL, (VO_PTCHAR)m_szURL);

		if (!vostrncmp (szURL, _T("RTSP://"), 6) || !vostrncmp (szURL, _T("rtsp://"), 6) || 
			!vostrncmp (szURL, _T("MMS://"), 5) || !vostrncmp (szURL, _T("mms://"), 5))
			m_nFF = VO_FILE_FFSTREAMING_RTSP;
		else if (!vostrncmp (szURL, _T("HTTP://"), 6) || !vostrncmp (szURL, _T("http://"), 6))
		{
			VO_TCHAR* p = vostrstr(szURL, _T(".sdp"));
			if(!p)
				p = vostrstr(szURL, _T(".SDP"));

			if(p && ((unsigned int)(p + 4 - szURL) == vostrlen(szURL) || p[4] == _T('?')))
				m_nFF = VO_FILE_FFSTREAMING_RTSP;
			else
			{
				if( vostrstr(szURL , _T(".m3u")) || vostrstr(szURL , _T(".M3U")) )
				{
					VOLOGE( "It is HTTP Live Streaming!" );
					m_nFF = VO_FILE_FFSTREAMING_HLS;
				}				
				else if( vostrstr(szURL , _T(".mpd")) || vostrstr(szURL , _T(".MPD")))
				{
					VOLOGE( "It is Silverlight DASH Streaming!" );
					m_nFF = VO_FILE_FFSTREAMING_DASH;
				}
				else if( vostrstr(szURL , _T("/manifest")) || vostrstr (szURL , _T("/MANIFEST")) || vostrstr (szURL , _T("/Manifest")))
				{
					VOLOGE( "It is Silverlight Smooth Streaming!" );
					m_nFF = VO_FILE_FFSTREAMING_SSS;
				}				
				else
				{
					VOLOGE( "It is HTTP Streaming!" );
					m_nFF = VO_FILE_FFSTREAMING_HTTPPD;
				}
			}
		}

		if(pURL == NULL)
		{
			CFileFormatCheck filCheck (m_pFileOP , &g_memOP);
			VO_FILE_FORMAT nFF = filCheck.GetFileFormat(&filSource , ~FLAG_CHECK_AUDIOREADER);
			if (nFF != VO_FILE_FFUNKNOWN)
				m_nFF = nFF;
		}

		if (m_nFF == VO_FILE_FFSTREAMING_RTSP || m_nFF == VO_FILE_FFAPPLICATION_SDP)
		{
			m_pFileSource = new CRTSPSource (m_hInst, m_nFF, &g_memOP, m_pFileOP, NULL);
			if(!m_pFileSource)
				return VOMP_ERR_OutMemory;
			// end

			m_pFileSource->SetSourceParam(VOMP_PID_SOCKET_CONNECTION_TYPE, &m_nSocketConnectionType);

			DigestAuthParam sDigestAuthParam;
			memset(&sDigestAuthParam, 0, sizeof(sDigestAuthParam));

			sDigestAuthParam.user = m_sDigestName;
			sDigestAuthParam.userLen = strlen(m_sDigestName);

			sDigestAuthParam.passwd = m_sDigestPass;	
			sDigestAuthParam.passwdLen = strlen(m_sDigestPass);
			m_pFileSource->SetSourceParam(VOID_STREAMING_DIGEST_AUTHEN, &sDigestAuthParam);
			int nMinBufTime = m_cfgPlay.GetItemValue ("Playback", "SourceMinBuffTime", 1000);
			m_pFileSource->SetSourceParam(VOMP_PID_MIN_BUFFTIME, &nMinBufTime);

		}
		else if (m_nFF == VO_FILE_FFSTREAMING_HTTPPD)
		{
			m_pFileSource = new CHTTPPDSource (m_hInst, m_nFF, &g_memOP, m_pFileOP, &cbSourceDrm);
			if(!m_pFileSource)
				return VOMP_ERR_OutMemory;
		}
		else if ( m_nFF == VO_FILE_FFSTREAMING_HLS)
		{
			m_pDataSource = new voCLiveDataSource (m_hInst, m_nFF, &g_memOP, m_pFileOP, NULL);
			if(!m_pDataSource)
				return VOMP_ERR_OutMemory;
			m_pFileSource = m_pDataSource;

			if (m_sVideoTrackInfo.Codec <= 0)
				m_sVideoTrackInfo.Codec = VOMP_VIDEO_CodingH264;
			m_pDataSource->AddTrack (NULL, m_sVideoTrackInfo.Codec, false);
			if (m_sAudioTrackInfo.Codec <= 0)
				m_sAudioTrackInfo.Codec = VOMP_AUDIO_CodingAAC;
			m_pDataSource->AddTrack (NULL, m_sAudioTrackInfo.Codec, true);

			m_pDataSource->SetMaxBuffTime (m_cfgPlay.GetItemValue ("Playback", "SourceMaxBuffTime", 10000));
			m_pDataSource->SetMinBuffTime (m_cfgPlay.GetItemValue ("Playback", "SourceMinBuffTime", 1000));
		}
		else if ( m_nFF == VO_FILE_FFSTREAMING_SSS )
		{
			m_pDataSource = new voCLiveDataSource (m_hInst, m_nFF, &g_memOP, m_pFileOP, NULL);
			if(!m_pDataSource)
				return VOMP_ERR_OutMemory;
			m_pFileSource = m_pDataSource;

			if (m_sVideoTrackInfo.Codec <= 0)
//				m_sVideoTrackInfo.Codec = VOMP_VIDEO_CodingWMV;
				m_sVideoTrackInfo.Codec = VOMP_VIDEO_Coding_Max;
			m_pDataSource->AddTrack (NULL, m_sVideoTrackInfo.Codec, false);
			if (m_sAudioTrackInfo.Codec <= 0)
//				m_sAudioTrackInfo.Codec = VOMP_AUDIO_CodingWMA;
				m_sAudioTrackInfo.Codec = VOMP_AUDIO_Coding_MAX;
			m_pDataSource->AddTrack (NULL, m_sAudioTrackInfo.Codec, true);

			m_pDataSource->SetMaxBuffTime (m_cfgPlay.GetItemValue ("Playback", "SourceMaxBuffTime", 10000));
			m_pDataSource->SetMinBuffTime (m_cfgPlay.GetItemValue ("Playback", "SourceMinBuffTime", 1000));
		}
		else if(m_nFF == VO_FILE_FFSTREAMING_DASH){
			m_pDataSource = new voCLiveDataSource (m_hInst, m_nFF, &g_memOP, m_pFileOP, NULL);
			if(!m_pDataSource)
				return VOMP_ERR_OutMemory;
			m_pFileSource = m_pDataSource;
			if (m_sVideoTrackInfo.Codec <= 0)
				//				m_sVideoTrackInfo.Codec = VOMP_VIDEO_CodingWMV;
			m_sVideoTrackInfo.Codec = VOMP_VIDEO_Coding_Max;
			m_pDataSource->AddTrack (NULL, m_sVideoTrackInfo.Codec, false);
			if (m_sAudioTrackInfo.Codec <= 0)
				//				m_sAudioTrackInfo.Codec = VOMP_AUDIO_CodingWMA;
			   m_sAudioTrackInfo.Codec = VOMP_AUDIO_Coding_MAX;
			m_pDataSource->AddTrack (NULL, m_sAudioTrackInfo.Codec, true);

			m_pDataSource->SetMaxBuffTime (m_cfgPlay.GetItemValue ("Playback", "SourceMaxBuffTime", 10000));
			m_pDataSource->SetMinBuffTime (m_cfgPlay.GetItemValue ("Playback", "SourceMinBuffTime", 1000));
		}
		else
		{
			m_pFileSource = new CFileSource (m_hInst, m_nFF, &g_memOP, m_pFileOP, &cbSourceDrm);
			if(!m_pFileSource)
				return VOMP_ERR_OutMemory;
		}
	}
	else if (m_nOpenFlag & VOMP_FLAG_SOURCE_DATASOURCE)
	{
		filSource.nFlag = VO_FILE_TYPE_HANDLE;
		filSource.pSource = m_pSource;

		voSourceOPFillPointer ();
		CFileFormatCheck filCheck (&g_vompSourceOP , &g_memOP);
		
#ifdef _IOS
		m_nFF = filCheck.GetFileFormat(&filSource , FLAG_CHECK_AUDIOREADER|FLAG_CHECK_REAL|FLAG_CHECK_ASF|FLAG_CHECK_MKV|FLAG_CHECK_MPEG4|FLAG_CHECK_TS|FLAG_CHECK_MP4);
#else
		m_nFF = filCheck.GetFileFormat(&filSource , ~FLAG_CHECK_AUDIOREADER);
#endif	

		VOLOGI ("File format is %d", m_nFF);

		m_pFileSource = new CFileSource (m_hInst, m_nFF, &g_memOP, &g_vompSourceOP, &cbSourceDrm);
		if(!m_pFileSource)
			return VOMP_ERR_OutMemory;
	}
	else
	{
		if ((m_nOpenFlag & 0X000F0000) == 0)
		{
			m_pDataSource = new voCDataSource (m_hInst, m_nFF, &g_memOP, m_pFileOP, NULL);
			if(!m_pDataSource)
				return VOMP_ERR_OutMemory;
		}
		else
		{
			m_nFF = VO_FILE_FFMOVIE_TS;
			if (m_nOpenFlag & VOMP_FLAG_SOURCE_SENDBUFFER_TS)
				m_nFF = VO_FILE_FFMOVIE_TS;
			else if (m_nOpenFlag & VOMP_FLAG_SOURCE_SENDBUFFER_MFS)
				m_nFF = VO_FILE_FFMOVIE_CMMB;

//			m_pDataSource = new voCMixDataSource (m_hInst, m_nFF, &g_memOP, m_pFileOP, NULL);
			if(!m_pDataSource)
				return VOMP_ERR_OutMemory;
		}

		m_pFileSource = m_pDataSource;

		if (m_sVideoTrackInfo.Codec <= 0)
		{
			if (m_nFF == VO_FILE_FFMOVIE_TS)
				m_sVideoTrackInfo.Codec = VOMP_VIDEO_Coding_Max;
			else
				m_sVideoTrackInfo.Codec = VOMP_VIDEO_CodingH264;
		}
		m_pDataSource->AddTrack (m_pSource, m_sVideoTrackInfo.Codec, false);
		if (m_sAudioTrackInfo.Codec <= 0)
		{
			if (m_nFF == VO_FILE_FFMOVIE_TS)
				m_sAudioTrackInfo.Codec = VOMP_AUDIO_Coding_MAX;
			else
				m_sAudioTrackInfo.Codec = VOMP_AUDIO_CodingAAC;
		}
		m_pDataSource->AddTrack (m_pSource, m_sAudioTrackInfo.Codec, true);

		m_pDataSource->SetMaxBuffTime (m_cfgPlay.GetItemValue ("Playback", "SourceMaxBuffTime", 10000));
		m_pDataSource->SetMinBuffTime (m_cfgPlay.GetItemValue ("Playback", "SourceMinBuffTime", 1000));
	}

	if(m_pFileSource == NULL)
	{
		VOLOGE ("It could not create CFileSource");
		return VOMP_ERR_OutMemory;
	}

	if(m_pLibFunc)
		m_pFileSource->SetLibOperator ((VO_LIB_OPERATOR *)m_pLibFunc);
	m_pFileSource->SetWorkPath(m_szWorkPath);

#ifndef __SYMBIAN32__
#ifndef _MAC_OS
#ifndef _IOS
	m_pFileSource->SetConfig (&m_cfgVOSDK);
#endif // _IOS
#endif // _MAC_OS
#endif // __SYMBIAN32__
	m_pFileSource->SetCallBack ((void *)vompStreamCallBack, this);

	VO_U32 nRC = VO_ERR_NONE;
	if (m_nOpenFlag & VOMP_FLAG_SOURCE_READBUFFER)
		nRC = m_pFileSource->LoadSource (m_pSource, 0, 0, 0);
	else
		nRC = m_pFileSource->LoadSource(filSource.pSource, VO_FILE_TYPE_NAME , 0 , 0);

	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("The file could not load!");
		// added by Lin Jun 20101218, next SetDatasource will fail if not reset status.
		m_nStatus = VOMP_STATUS_STOPPED;
		// add end	
		return VOMP_ERR_File;
	}

	m_pFileSource->GetSourceInfo (&m_sFilInfo);
	if (m_sFilInfo.Tracks == 0)
	{
		VOLOGE ("The track number is 0");
		return VOMP_ERR_File;
	}

	if ((m_nOpenFlag & VOMP_FLAG_SOURCE_METADATA) == VOMP_FLAG_SOURCE_METADATA)
	{
		m_nTrackCount = m_sFilInfo.Tracks;
		m_nStatus = VOMP_STATUS_STOPPED;
		return VOMP_ERR_None;
	}

	m_nTrackCount = 0;

	VO_SOURCE_TRACKINFO	trkInfo;
	for (i = 0; i < m_sFilInfo.Tracks; i++)
	{
		m_pFileSource->GetTrackInfo (i, &trkInfo);

		if ((trkInfo.Type == VO_SOURCE_TT_AUDIO || trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO))
		{
			if (m_bVideoOnly || m_nAudioTrack != -1 || m_nGetThumbnail > 0)
				continue;

			m_nAudioTrack = i;
			m_pFileSource->GetTrackInfo (m_nAudioTrack, &m_sAudioTrackInfo);
			// double confirm??
			m_pFileSource->GetAudioFormat (m_nAudioTrack, &m_sAudioDecOutFormat);
			m_bAudioEOS = VO_FALSE;
			m_nAudioEOSArrived = 0;
			m_nTrackCount++;

//#ifndef  WINCE
//			bool bAudio = CreateAudioDec ();
//			if(bAudio == false)
//			{
//				if(m_pAudioDec)
//				{
//					delete m_pAudioDec;
//					m_pAudioDec = NULL;
//				}
//			}
//#endif //WINCE
		}
		else if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))
		{
			if (m_bAudioOnly || m_nVideoTrack != -1)
				continue;

			m_nVideoTrack = i;
			m_pFileSource->GetTrackInfo (m_nVideoTrack, &m_sVideoTrackInfo);
			m_pFileSource->GetVideoFormat (m_nVideoTrack, &m_sVideoFormat);

			m_nVideoRndWidth = m_sVideoFormat.Width;
			m_nVideoRndHeight = m_sVideoFormat.Height;

			m_bVideoEOS = VO_FALSE;
			m_nTrackCount++;

//#ifndef  WINCE
//			bool bVideo = CreateVideoDec ();
//			if(bVideo == false)
//			{
//				if(m_pVideoDec)
//				{
//					delete m_pVideoDec;
//					m_pVideoDec = NULL;
//				}
//			}
//#endif //WINCE
		}
		
		// added by Lin Jun 20101218, some WMV files have more than one video tracks, we only support one.
		//if(i > 0)
		{
			if(m_bVideoOnly && m_nVideoTrack>=0)
				break;
			if(m_bAudioOnly && m_nAudioTrack>=0)
				break;
			if(m_nAudioTrack>=0 && m_nVideoTrack>=0)
				break;
			
		}
		// added end
	}

	if (m_nAudioTrack > 0 && !(m_nOpenFlag & VOMP_FLAG_SOURCE_SENDBUFFER) && m_pAudioDec != NULL)
	{
		int nTryTimes = 0;
		nRC = VO_ERR_FAILED;
		while (nRC != VO_ERR_NONE)
		{
			nRC = ReadAudioSample ();
			if (nTryTimes++ > 50)
				break;
		}
		long long llSeekPos = 0;
		if (m_nVideoTrack >= 0)
			nRC = m_pFileSource->SetTrackPos(m_nVideoTrack , &llSeekPos);
		nRC = m_pFileSource->SetTrackPos(m_nAudioTrack , &llSeekPos);
		m_pAudioDec->Flush();
		memset (&m_sAudioSample, 0, sizeof (m_sAudioSample));
	}

	if (m_nSeekPos > 0)
		SetCurPos ((int)m_nSeekPos);

	m_nStatus = VOMP_STATUS_STOPPED;

	// Add for file sink
	if (m_pFileSink == NULL)
	{		
#ifdef _LINUX_ANDROID
		if (strlen (m_szWorkPath) <= 0)
			return VOMP_ERR_None;
		strcpy (m_szSinkName, "/mnt/sdcard/vomeDumpData.mp4");
		char szLibSink[256]; 
		strcpy (szLibSink, "/data/local/OSMP+Debug/lib/libvoMP4FW.so");
		int nOk = access(szLibSink, F_OK);
		if (nOk != 0)
			return VOMP_ERR_None;		
		VOLOGI ("The MP4 dump file is %s", szLibSink);			
#elif defined _WIN32
		//vostrcpy (m_szSinkName, _T("e:\\vomeDump.mp4"));
		return VOMP_ERR_None;
#else
		return VOMP_ERR_None;
#endif // _LINUX_ANDROID
		
		m_pFileSink = new CFileSink (NULL, VO_FILE_FFMOVIE_MP4, &g_memOP, m_pFileOP);
		if(!m_pFileSink)
			return VOMP_ERR_OutMemory;
		m_pFileSink->SetLibOperator ((VO_LIB_OPERATOR *)m_pLibFunc);
		m_pFileSink->SetWorkPath ((VO_TCHAR *)m_szWorkPath);

		m_Source.pSource = (VO_PTR) m_szSinkName;
		m_Source.nFlag   = VO_FILE_TYPE_NAME;
		m_Source.nMode    = VO_FILE_WRITE_ONLY;
#ifdef _MAC_OS
		m_Source.nReserve = (unsigned long)m_szSinkName;
#else
		m_Source.nReserve = (VO_U32)m_szSinkName;
#endif
		
		VO_U32 nRC = m_pFileSink->Init (&m_Source, m_sAudioTrackInfo.Codec, VOMP_VIDEO_CodingH264);
		if (nRC != VO_ERR_NONE) 
		{
			delete m_pFileSink;
			m_pFileSink = NULL;
			VOLOGW ("It could not init the sink file!");
			return VOMP_ERR_None;
		}

		if (m_pAudioDec != NULL)
			nRC = m_pFileSink->SetParam (VO_PID_AUDIO_FORMAT, &m_sAudioDecOutFormat);
		if (m_pVideoDec != NULL)
			nRC = m_pFileSink->SetParam (VO_PID_VIDEO_FORMAT, &m_sVideoFormat);

		m_pFileSink->Start ();
	}

	return VOMP_ERR_None;
}

bool voCMediaPlayer::CreateAudioDec (void)
{
	VOLOGI("create audio decoder");
	voCAutoLock lockCodec (&m_mtCodec);
	VOLOGI("create audio decoder enter m_mtCodec");

	if (m_pFileSource == NULL || m_nAudioTrack < 0)
	{
		VOLOGE ("The source is NULL or there was no audio track!");
		return false;
	}

	//Rogine add for issue #27473, 2013/8/16
	//This condition may be happened with audio init failed, which may be 
	//caused by lack of corresponding audio decoder module  
	if (m_bOnlyGetVideo)
	{
		VOLOGI ("Only get video, audio create disable!");
		return false;
	}

	VO_SOURCE_TRACKINFO	trkInfo;
	m_pFileSource->GetTrackInfo (m_nAudioTrack, &trkInfo);
	
	bool isAMCDec = !vostrcmp(m_sSetAudioName, _T("voAudioMCDec"));
	
	if (trkInfo.Codec == VOMP_AUDIO_Coding_MAX)
	{
		return true;
	}
	// modified by gtxia 2013-9-4
	// since the mediacodec can not support only send the head data so I have to reset it again 
	else if(trkInfo.Codec == m_sAudioTrackInfo.Codec)
	{
		// added by gtxia 2013-9-13 
		if (isAMCDec && m_pAudioDec != NULL && (trkInfo.Codec == VO_AUDIO_CodingEAC3 || trkInfo.Codec == VO_AUDIO_CodingAC3))
		{
			m_pAudioDec->SetParam(VO_ACODEC_RESET, &trkInfo);
			return true;
		}
		
		if (m_pAudioDec != NULL && (trkInfo.Codec == VO_AUDIO_CodingAAC || trkInfo.Codec == VO_AUDIO_CodingMP3))
		{
			if(trkInfo.Codec == VO_AUDIO_CodingAAC && isAMCDec)
			{
				m_pAudioDec->SetParam(VO_ACODEC_RESET, &trkInfo);
			}
			else
			{
				m_pAudioDec->Flush();
			}
			return true; 
		}
	}

	if (m_pAudioDec != NULL)
	{
		delete m_pAudioDec;
		m_pAudioDec = NULL;
	}
	VO_AUDIO_FORMAT				sAudioFormat;

	m_pFileSource->GetAudioFormat (m_nAudioTrack, &sAudioFormat);

	if(sAudioFormat.SampleRate < 8000)
		sAudioFormat.SampleRate = 44100;

	if(sAudioFormat.Channels == 0)
		sAudioFormat.Channels = 2;

	m_sAudioTrackInfo.Codec = trkInfo.Codec;
	//rogine add for #31875 from CJ
	if ((trkInfo.Codec == VO_AUDIO_CodingAC3 || trkInfo.Codec == VO_AUDIO_CodingEAC3) 
			&& !m_bAllowDolbyAudio)
	{
		VOLOGE ("Since it not allowed to play dolby audio, framework didn't create audio dec!");
		return false;
	}	

#if defined(_IOS) && defined(USE_HW_AUDIO_DEC)
	m_pAudioDec = new CAudioMockDecoder (m_hInst , trkInfo.Codec, &g_memOP);
#else
	m_pAudioDec = new CAudioDecoder (m_hInst , trkInfo.Codec, &g_memOP);
#endif	
	if (m_pAudioDec == NULL)
	{
		VOLOGE ("It could not create audio dec class!");
		return false;
	}

	m_pAudioDec->SetParam(VO_PID_COMMON_WORKPATH, &m_szWorkPath);
	m_pAudioDec->SetParam(VO_PID_COMMON_LOGFUNC, &m_cbVOLOG);
	m_pAudioDec->SetLibOperator ((VO_LIB_OPERATOR *)m_pLibFunc);
	m_pAudioDec->SetWorkPath(m_szWorkPath);
	m_pAudioDec->SetAACPlus(m_nAudioAACPlus);

	m_pAudioDec->SetAudioLibFile(m_nSetAudioNameType, m_sSetAudioName);
	m_pAudioDec->SetAudioAPIName(m_nSetAudioAPIType, m_sSetAudioAPI);

	// added by gtxia
	m_pAudioDec->setSampleDrmInfo(m_pFileSource->isADrmDataAppended());
#ifndef __SYMBIAN32__
#ifndef _MAC_OS
#ifndef _IOS
	m_pAudioDec->SetConfig (&m_cfgVOSDK);
#endif // _IOS
#endif // _MAC_OS
#endif  //__SYMBIAN32__
	VO_S32	nRC = 0;
	VO_U32	nFourCC = 0;
	nRC = m_pFileSource->GetTrackParam (m_nAudioTrack, VO_PID_SOURCE_CODECCC, &nFourCC);
	m_pAudioDec->SetFourCC (nFourCC);

	// the head data for WMA is different
	if (trkInfo.Codec == VO_AUDIO_CodingWMA || trkInfo.Codec == VO_AUDIO_CodingADPCM)
	{
		nRC = m_pFileSource->GetTrackParam (m_nAudioTrack, VO_PID_SOURCE_WAVEFORMATEX, &trkInfo.HeadData);
		if (nRC != VO_ERR_NONE)
		{
			VOLOGE ("It could not get VO_PID_SOURCE_WAVEFORMATEX!");
			return false;
		}

		if (trkInfo.HeadData != NULL)
		{
			VO_WAVEFORMATEX * pWaveFormat = (VO_WAVEFORMATEX *)trkInfo.HeadData;
			trkInfo.HeadSize = VO_WAVEFORMATEX_STRUCTLEN + pWaveFormat->cbSize;
		}
		else
		{
			m_pFileSource->GetTrackInfo (m_nAudioTrack, &trkInfo);
		}
	}
	//rogine modify for #31875 from CJ
	if (m_pAudioEffect == NULL && m_bAddAudioEffect && m_bAllowDolbyAudio)
	{
		m_pAudioEffect = new CAudioEffect (m_hInst , &g_memOP);
		if (m_pAudioEffect != NULL)
		{
			m_pAudioEffect->SetWorkPath(m_szWorkPath);
			m_pAudioEffect->SetLibOperator ((VO_LIB_OPERATOR *)m_pLibFunc);
#ifndef __SYMBIAN32__
			m_pAudioEffect->SetConfig (&m_cfgPlay);
#endif  //__SYMBIAN32__

			if (m_pAudioEffect->Init (&sAudioFormat) != VO_ERR_NONE)
			{
				delete m_pAudioEffect;
				m_pAudioEffect = NULL;
			}
		}
	}
	else if (!m_bAllowDolbyAudio)
	{
		if (m_pAudioDec != NULL)
		{
			delete m_pAudioDec;
			m_pAudioDec = NULL;
		}

		return false;
	}	

	if (m_pAudioEffect != NULL)
	{
		m_pAudioEffect->Enable (m_bEffectEnable);
		if(trkInfo.Codec == VO_AUDIO_CodingAC3 || trkInfo.Codec == VO_AUDIO_CodingEAC3)
			sAudioFormat.Channels = 6;
	}
	else
	{
		if((trkInfo.Codec != VO_AUDIO_CodingPCM && trkInfo.Codec != VO_AUDIO_CodingAAC) && sAudioFormat.Channels > 2)
			sAudioFormat.Channels = 2;
	}

	if (trkInfo.HeadData != NULL && trkInfo.HeadSize > 0)
	{
		nRC = m_pAudioDec->Init (trkInfo.HeadData, trkInfo.HeadSize, &sAudioFormat);
	}
	else
	{
		VO_SOURCE_SAMPLE	sample;
		memset (&sample, 0, sizeof (VO_SOURCE_SAMPLE));

		nRC = m_pFileSource->GetTrackParam (m_nAudioTrack, VO_PID_SOURCE_FIRSTFRAME, &sample);
		if (nRC != VO_ERR_NONE)
		{
			sample.Buffer = NULL;
			sample.Size = 0;
		}
		else
		{
			sample.Size = sample.Size & 0X7FFFFFFF;
		}

		nRC = m_pAudioDec->Init (sample.Buffer, sample.Size, &sAudioFormat);
	}

	if ((nRC & VO_ERR_LICENSE_ERROR) == VO_ERR_LICENSE_ERROR)
	{
		VOLOGE("m_pAudioDec->Init failed due to license check error 0x%08X", nRC);
		postCheckStatusEvent (VOMP_CB_LicenseFailed, nRC);
		return false;
	}

	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("Init audio dec error. %08X", (unsigned int)nRC);
		//Rogine add for issue #27473, 2013/8/16
		//if init failed, audio dec should not be init again unless source meet new format
		m_bOnlyGetVideo = VO_TRUE;
		m_nAudioOnOff = 0;
#ifdef _METRO
		return true;
#else  //_METRO
		return false;
#endif //_METRO
	}
	//else
	//{
	//	int tmp = VO_AUDIO_CHAN_MULDOWNMIX2;
	//	m_pAudioDec->SetParam(VO_PID_AAC_CHANNELSPEC, &tmp);
	//	//sAudioFormat.Channels = 2;
	//}

	// added by gtxia for dealing with the hardware decoder
	if(mJavaVM)
	{
		m_pAudioDec->SetParam(VO_ACODEC_JVM, mJavaVM);
	}

	if (m_pAudioEffect != NULL)
	{
		// we need use audio format of decoder output to sync up format change
		m_pAudioEffect->SetFormat (&m_sAudioDecOutFormat);
		m_pAudioEffect->Enable (m_bEffectEnable);
		m_pAudioEffect->GetFormat (&m_sAudioEftOutFormat);
	}

	UpdateAudioFormat (&sAudioFormat);
	m_sLastAudioFmt.Channels = sAudioFormat.Channels;
	VOLOGI("@@@####fmtCh m_sLastAudioFmt.Channels = sAudioFormat.Channels: %d", sAudioFormat.Channels);
	
	if (m_pAudioSpeed == NULL)
	{		
		m_pAudioSpeed = new CAudioSpeed(m_hInst, &g_memOP);
		if (m_pAudioSpeed != NULL)
		{
			m_pAudioSpeed->SetWorkPath(m_szWorkPath);

		VO_AUDIO_FORMAT	fmtAudio;
		fmtAudio.Channels   = sAudioFormat.Channels;
		fmtAudio.SampleRate = sAudioFormat.SampleRate;
		fmtAudio.SampleBits = sAudioFormat.SampleBits;

			if (m_pAudioSpeed->Init(&fmtAudio) != 0)
			{
				VOLOGI ("Audio speed init failed");
				m_bASpeedEnable = VO_FALSE;
			}
			else
			{
				VOLOGI ("Audio speed init successed");
				VO_S32 nPlaySpeed = 10;
				if (m_nAudioPlaySpeed > 0)
					nPlaySpeed = m_nAudioPlaySpeed;
				VOLOGI("@@@####spd Set audio playback speed %d in vompEng", nPlaySpeed);
				m_pAudioSpeed->SetParam(VO_PID_AUDIOSPEED_RATE, &nPlaySpeed);
			}
		}	
		else
			VOLOGE("Create the audio speed failed");
	}	
	else
	{
		if (m_bASpeedEnable == VO_TRUE)
		{
			int nChannel = sAudioFormat.Channels;
			if (nChannel > 2)
				nChannel = 2;
			VOLOGI("@@@####spd m_pAudioDec->SetParam (VO_PID_AUDIO_CHANNELS, %d)", nChannel);
			m_pAudioDec->SetParam (VO_PID_AUDIO_CHANNELS, &nChannel);
		}					
	}

	return true;
}

int voCMediaPlayer::UpdateAudioFormat (VO_AUDIO_FORMAT * pAudioOutFormat)
{
	if(pAudioOutFormat->SampleRate < 8000)
	{
		VOLOGI("output sample rate %d too small, force to 8000", pAudioOutFormat->SampleRate);
		pAudioOutFormat->SampleRate = 8000;
	}

	VO_U32 nAudioFrameSize = pAudioOutFormat->SampleRate * pAudioOutFormat->Channels * pAudioOutFormat->SampleBits / 8;
	VO_U32 nPreAudioFrameSize = m_pAudioEffect ? m_nAudioEftOutFrameSize : m_nAudioDecOutFrameSize;
	if(nAudioFrameSize > nPreAudioFrameSize)
	{
		// previous PCM buffers is not enough, we need re-allocate
		if(m_pAudioDecOutPcmBuff != NULL)
		{
			delete [] m_pAudioDecOutPcmBuff;
			m_pAudioDecOutPcmBuff = NULL;
		}

		if(m_pAudioEftOutPcmBuff != NULL)
		{
			delete [] m_pAudioEftOutPcmBuff;
			m_pAudioEftOutPcmBuff = NULL;
		}
	}

	if(m_pAudioEffect)
	{
		m_nAudioDecOutFrameSize = m_sAudioDecOutFormat.SampleRate * m_sAudioDecOutFormat.Channels * m_sAudioDecOutFormat.SampleBits / 8;
		m_nAudioEftOutFrameSize = nAudioFrameSize;
	}
	else
	{
		m_nAudioDecOutFrameSize = nAudioFrameSize;
		m_nAudioEftOutFrameSize = 0;
	}

	if(m_nAudioStepTime == 0)
		m_nAudioStepTime = m_cfgPlay.GetItemValue ("Playback", "AudioSinkStepTime", 100);

	if(pAudioOutFormat->SampleRate < 16000)
	{
		VOLOGI("output sample rate %d too small, force step time from %d to 200", pAudioOutFormat->SampleRate, m_nAudioStepTime);
		m_nAudioStepTime = 200;
	}

	m_nAudioDecOutStepSize = m_nAudioDecOutFrameSize  * m_nAudioStepTime / 1000;

#ifndef _WIN32
	if(m_pAudioDecOutPcmBuff == NULL)
	{
		m_pAudioDecOutPcmBuff = new VO_BYTE[m_nAudioDecOutFrameSize];
		if(!m_pAudioDecOutPcmBuff)
			return VOMP_ERR_OutMemory;
	}
	if(m_pAudioEftOutPcmBuff == NULL)
	{
		m_pAudioEftOutPcmBuff = new VO_BYTE[m_nAudioEftOutFrameSize];
		if(!m_pAudioEftOutPcmBuff)
			return VOMP_ERR_OutMemory;
	}
#endif

	return 0;
}

bool voCMediaPlayer::CreateVideoDec (void)
{
	VOLOGI("create video decoder");
	voCAutoLock lockCodec (&m_mtCodec);
	VOLOGI("create video decoder enter m_mtCodec");

	if (m_pFileSource == NULL || m_nVideoTrack < 0)
	{
		VOLOGE ("The source is NULL or there was no video track!");
		return false;
	}

	if (m_pVideoDec != NULL)
	{
		delete m_pVideoDec;
		m_pVideoDec = NULL;
	}
	VO_SOURCE_TRACKINFO	trkInfo;
	m_pFileSource->GetTrackInfo (m_nVideoTrack, &trkInfo);

	if (trkInfo.HeadSize < 128)
	{
		memcpy(m_nVideoBuffHead, trkInfo.HeadData, trkInfo.HeadSize);
	}
	else
	{
		VOLOGE("@@@####srcV trkInfo.HeadSize: %d, this value is too big!", trkInfo.HeadSize);
	}
	
	if (trkInfo.Codec == VOMP_VIDEO_Coding_Max)
		return true;

	m_pFileSource->GetVideoFormat(m_nVideoTrack, &m_sVideoFormat);
	m_sVideoTrackInfo.Codec = trkInfo.Codec;

	VO_S32	nRC = 0;
	VO_U32	nFourCC = 0;
	nRC = m_pFileSource->GetTrackParam (m_nVideoTrack, VO_PID_SOURCE_CODECCC, &nFourCC);

#ifdef WINCE
	if(!CheckVideoCodecLegal((VOMP_VIDEO_CODINGTYPE)trkInfo.Codec , nFourCC , m_sVideoFormat.Width , m_sVideoFormat.Height))
		return false;

#endif // WINCE
	m_pVideoDec = new CVideoDecoder (m_hInst, trkInfo.Codec, &g_memOP);

	// added by gtxia for fixing the problem that make the VC1 crashing. the issue id 28067
	// in fact the code NewVideoCodec has been adde this logic, seems that our VC1 can not eat any error data 
	// so I have to move this logic here 
	if(trkInfo.Codec != VOMP_VIDEO_CodingH264 && (VO_ACODEC_COLOR_TYPE == m_nColorType || VO_COLOR_IOMX_PRIVATE == m_nColorType))
	{
		m_pListener(m_pUserData, VOMP_CB_CodecNotSupport, &trkInfo.Codec, NULL);
		return true;
	}
	
	if (m_pVideoDec == NULL)
	{
		VOLOGE ("It could not create CVideoDecoder");
		return false;
	}
	
#if defined(_IOS) || defined(_MAC_OS)
	if(m_pLogoEffect)
		m_pLogoEffect->Flush();
#endif

	m_pVideoDec->SetParam(VO_PID_COMMON_WORKPATH, &m_szWorkPath);
	// david 2011/11/30
	m_pVideoDec->SetParam(VO_PID_COMMON_LOGFUNC, &m_cbVOLOG);
	m_pVideoDec->SetSourcePtr((void*)m_pFileSource);

	VOLOGI("@@@###  m_nVideoRenderCount = %d", m_nVideoRenderCount);
	// if using single core, there is video flash issue, do workaround here
	// we need remove it in future, East 20131009
	if(m_nCPUNumber <= 1)
		m_pVideoDec->SetVideoCount(m_nVideoRenderCount + 2);
	else
		m_pVideoDec->SetVideoCount(m_nVideoRenderCount);

	m_pVideoDec->SetVideoLibFile(m_nSetVideoNameType, m_sSetVideoName);
	m_pVideoDec->SetVideoAPIName(m_nSetVideoAPIType, m_sSetVideoAPI);
	
	// added by gtxia 
	m_pVideoDec->setSampleDrmInfo(m_pFileSource->isVDrmDataAppended());
	
	if (m_nGetThumbnail > 0)
		m_pVideoDec->SetVOSDK (VO_TRUE);
	if(m_pLibFunc)
		m_pVideoDec->SetLibOperator ((VO_LIB_OPERATOR *)m_pLibFunc);
	m_pVideoDec->SetWorkPath(m_szWorkPath);
	m_pVideoDec->SetCPUVersion (m_nCPUVersion);
	if (m_nCPUNumber > 0)
		m_pVideoDec->SetCPUNumber (m_nCPUNumber);
#ifndef __SYMBIAN32__
#ifndef _MAC_OS
#ifndef _IOS
	m_pVideoDec->SetConfig (&m_cfgVOSDK);
#endif // _IOS
#endif // _MAC_OS
#endif // __SYMBIAN32__

	if(m_nColorType == VO_COLOR_IOMX_PRIVATE && (m_nZoomType == VO_ZM_PANSCAN || m_nZoomType == VO_ZM_ZOOMIN))
	{
		VOMP_RECT rcDraw;
		if(VOMP_ERR_None == getDrawRect(rcDraw))
			m_pVideoDec->SetParam(VO_PID_IOMXDEC_SetCrop, &rcDraw);
		else
			m_pVideoDec->SetParam(VO_PID_IOMXDEC_SetCrop, &m_rcDraw);
	}

	if(m_nColorType == VO_ACODEC_COLOR_TYPE && (m_nZoomType == VO_ZM_PANSCAN || m_nZoomType == VO_ZM_ZOOMIN))
	{
		int nMode = 2;
		m_pVideoDec->SetParam(VO_ACODEC_SMODE, &nMode);		
	}
	m_pVideoDec->SetFourCC (nFourCC);

	// the head data for WMV is different
	if (trkInfo.Codec == VO_VIDEO_CodingWMV)
	{
		if (memcmp((char*)&nFourCC, (char*)"WVC1", 4) == 0) // WVC1
		{
			m_pFileSource->GetTrackInfo (m_nVideoTrack, &trkInfo);
		}
		else
		{
			nRC = m_pFileSource->GetTrackParam (m_nVideoTrack, VO_PID_SOURCE_BITMAPINFOHEADER, &trkInfo.HeadData);
			if (nRC != VO_ERR_NONE)
			{
				VOLOGE ("WMV VO_PID_SOURCE_BITMAPINFOHEADER error %08X", (unsigned int)nRC);
				return false;
			}

			if (trkInfo.HeadData != NULL)
			{
				VO_BITMAPINFOHEADER * pBmpInfo = (VO_BITMAPINFOHEADER *)trkInfo.HeadData;
				trkInfo.HeadSize = pBmpInfo->biSize;
			}
			else
			{
				m_pFileSource->GetTrackInfo (m_nVideoTrack, &trkInfo);
			}
		}
	}

	// Pass the listener to decoder then down to OMXWrap for sharing memory between HW DEC & NativeWindow
	// david@2011/12/2
	VOMP_LISTENERINFO li;
	li.pListener = m_pListener;
	li.pUserData = m_pUserData;
	m_pVideoDec->SetListenerInfo(&li);

	if (trkInfo.HeadData != NULL && trkInfo.HeadSize > 0)
	{
		nRC = m_pVideoDec->Init (trkInfo.HeadData, trkInfo.HeadSize, &m_sVideoFormat);
#if defined DUMP_VIDEO_HEAD
		if(m_fDumpVideoinput)
		{
			VOLOGI("Open file successed");
			fwrite(trkInfo.HeadData, trkInfo.HeadSize, 1, m_fDumpVideoinput);
		}
		else
		{
			VOLOGI("Open file failed");
		}
#endif
		if (nRC != VO_ERR_NONE)
		{
			VOLOGE("failed to init video decoder 0x%08X", nRC);
			return false;
		}
	}
	else
	{
		VO_SOURCE_SAMPLE	sample;
		memset (&sample, 0, sizeof (VO_SOURCE_SAMPLE));

		nRC = m_pFileSource->GetTrackParam (m_nVideoTrack, VO_PID_SOURCE_FIRSTFRAME, &sample);

		if (nRC != VO_ERR_NONE)
		{
			sample.Buffer = NULL;
			sample.Size = 0;
		}
		else
		{
			sample.Size = sample.Size & 0X7FFFFFFF;
		}

		nRC = m_pVideoDec->Init (sample.Buffer, sample.Size, &m_sVideoFormat);
	}

	if ((nRC & VO_ERR_LICENSE_ERROR) == VO_ERR_LICENSE_ERROR)
	{
		VOLOGE("m_pVideoDec->Init failed due to license check error 0x%08X", nRC);
		postCheckStatusEvent (VOMP_CB_LicenseFailed, nRC);
		return false;
	}

	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("m_pVideoDec->Init error %08X", (unsigned int)nRC);
#ifdef _METRO
		return true;
#else  //_METRO
		return false;
#endif //_METRO
	}
	
	if(m_pVidDecCB)
		m_pVideoDec->SetParam (VO_PID_VIDEO_DRM_FUNC, m_pVidDecCB);

	if(m_nVideoRenderCount > 1)
	{
		int MemShared = 0;
		MemShared = m_pVideoDec->GetMemShared();
		if(MemShared)
		{
			if (m_nVideoRndCopy > 0)
			{
				if (m_ppVideoOutBuffers != NULL)
				{
					VO_U32 i = 0;
					for (i = 0; i < m_nVideoRenderCount; i++)
					{
						if (m_ppVideoOutBuffers[i] != NULL)
						{
							if (m_ppVideoOutBuffers[i]->Buffer[0] != NULL)
							{
								delete []m_ppVideoOutBuffers[i]->Buffer[0];
								m_ppVideoOutBuffers[i]->Buffer[0] = NULL;
							}
							m_ppVideoOutBuffers[i]->Buffer[0] = NULL;
						}
					}
				}
			}

			m_nVideoRndCopy = 0;
		}
		else
		{
			m_nVideoRndCopy = 1;
		}
	}

	
	if(mJavaVM)
		m_pVideoDec->SetParam(VO_ACODEC_JVM, mJavaVM);
	
	m_pVideoDec->SetParam(VO_ACODEC_SURFACE, m_hView);
	
	m_pVideoDec->SetParam(VO_PID_IOMXDEC_SetSurface, m_hView);

	if (m_pVideoEffect != NULL)
	{
		delete m_pVideoEffect;
		m_pVideoEffect = NULL;
	}

	if( m_pVideoEffectType == VOMP_VE_Null )
		return true;

	m_pVideoEffect = new CVideoEffect (m_hInst , &g_memOP);
	if (m_pVideoEffect != NULL)
	{
		m_pVideoEffect->SetLibOperator ((VO_LIB_OPERATOR *)m_pLibFunc);
		m_pVideoEffect->SetWorkPath(m_szWorkPath);
#ifndef __SYMBIAN32__
		m_pVideoEffect->SetConfig (&m_cfgPlay);
#endif  //__SYMBIAN32__

		m_pVideoEffect->SetVideoEffect((VO_VIDEO_EFFECTTYPE)m_pVideoEffectType);
		m_pVideoEffect->Enable(m_bVideoEffectOn);

		m_pVideoEffect->Init (&m_sVideoFormat);   		

		if(m_pVideoEffectType == VOMP_VE_CloseCaptionOn)
		{
			VO_H264_SEIFLAG val = VHSF_GET_T35_USERDDATA;
			m_pVideoDec->SetParam(VO_ID_H264_ENABLE_SEI, &val);
		}
	}

	m_bUsingHWCodec = m_pVideoDec->GetVOSDK() ? VO_FALSE : VO_TRUE;
	
	if(m_nSetVideoNameType || m_nSetVideoAPIType)
		postCheckStatusEvent (VOMP_CB_HWStatus, m_bUsingHWCodec);

	return true;
}

bool voCMediaPlayer::CheckVideoCodecLegal(VOMP_VIDEO_CODINGTYPE nCodec , VO_U32 nFourCC , int nWidth , int nHeight )
{
	VO_TCHAR strCfgFile[1024];
	memset(strCfgFile , 0 , 1024 * sizeof(VO_TCHAR));

	if(vostrlen(m_szWorkPath) > 0)
	{
		vostrcpy(strCfgFile , m_szWorkPath);
	}

	vostrcat(strCfgFile , _T("vommcodec.cfg"));

	CBaseConfig *pCfg = new CBaseConfig();
	if(pCfg == NULL)
		return true;

	if(pCfg->Open(strCfgFile) == VO_FALSE)
	{
		delete pCfg ;
		pCfg = NULL;

		return true;
	}

	char szCfgItem[128];
	if (nCodec == VOMP_VIDEO_CodingMPEG4)
		strcpy (szCfgItem, "Video_Dec_MPEG4");
	else if (nCodec == VOMP_VIDEO_CodingH263)
		strcpy (szCfgItem, "Video_Dec_H263");
	else if (nCodec == VOMP_VIDEO_CodingS263)
		strcpy (szCfgItem, "Video_Dec_S263");
	else if (nCodec == VOMP_VIDEO_CodingH264)
		strcpy (szCfgItem, "Video_Dec_H264");
	else if (nCodec == VOMP_VIDEO_CodingWMV)
		strcpy (szCfgItem, "Video_Dec_WMV");
	else if (nCodec == VOMP_VIDEO_CodingVC1)
		strcpy (szCfgItem, "Video_Dec_VC1");
	else if (nCodec == VOMP_VIDEO_CodingRV)
		strcpy (szCfgItem, "Video_Dec_REAL");
	else if (nCodec == VOMP_VIDEO_CodingMPEG2)
		strcpy (szCfgItem, "Video_Dec_MPEG2");
	else if (nCodec == VOMP_VIDEO_CodingMJPEG)
		strcpy (szCfgItem, "Video_Dec_MJPEG");
	else if (nCodec == VOMP_VIDEO_CodingDIVX)
		strcpy (szCfgItem, "Video_Dec_DIVX");
	else if (nCodec == VOMP_VIDEO_CodingVP6)
		strcpy (szCfgItem, "Video_Dec_VP6");
	else if (nCodec == VOMP_VIDEO_CodingVP8)
		strcpy (szCfgItem, "Video_Dec_VP8");
	else if(nCodec == VOMP_VIDEO_CodingH265)
		strcpy (szCfgItem, "Video_Dec_H265");

	// check the max video size.
	VO_S32 nMaxWidth = pCfg->GetItemValue (szCfgItem, (char*)"MaxWidth", 1920);
	VO_S32 nMaxHeight = pCfg->GetItemValue (szCfgItem,(char*)"MaxHeight", 1080);

	if(pCfg != NULL)
	{
		delete pCfg;
		pCfg = NULL;
	}

	if(nWidth > nMaxWidth || nHeight > nMaxHeight)
		return false;

	return true;
}

int voCMediaPlayer::ReadAudioSample (void)
{
	VO_S32 nRC = 0;
	if (m_nVideoTrack >= 0 && !m_bVideoEOS && m_nStatus >= VOMP_STATUS_LOADING && !m_bOnlyGetAudio)
	{
		if (m_nVideoRenderNum <= 0) //change to render num for iomx not have filled index
		{
			voOS_Sleep (10);
			return VOMP_ERR_Retry;
		}
	}

	if (m_sAudioSample.Size > 0 && m_pAudioDec != NULL)
	{
		// tag: 20101228
#ifndef USE_HW_AUDIO_DEC	
		nRC = DecAudioSample ();
		if ( nRC == VO_ERR_NONE || nRC == VOMP_ERR_FormatChange )
			return nRC;
		if(mAudioDecInputRetry)
		{ 
			m_sAudioDecInBuf.Buffer = m_sAudioSample.Buffer;
			m_sAudioDecInBuf.Length = m_sAudioSample.Size & 0x7FFFFFFF;
			m_sAudioDecInBuf.Time   = m_sAudioSample.Time;

			nRC = m_pAudioDec->SetInputData(&m_sAudioDecInBuf);

			mAudioDecInputRetry = (VO_ERR_IOMXDEC_NeedRetry == nRC) ? VO_TRUE : VO_FALSE;

			if(nRC != VO_ERR_NONE && VO_ERR_IOMXDEC_NeedRetry != nRC)
				return VOMP_ERR_Retry;
			nRC = DecAudioSample ();
			return nRC;	
		}
#endif
	}

	if(m_nAudioOnOff == 0 && m_nAudioReaded)
	{
		if(m_sAudioSample.Time > (VO_S64)GetPlayingTime () + 50)
		{
			voOS_Sleep((int)(m_sAudioSample.Time - (VO_S64)GetPlayingTime () - 20));
		}
	}

	m_mtSource.Lock ();
	m_sAudioSample.Flag = 0;
	m_sAudioSample.Time = GetPlayingTime();
	nRC = m_pFileSource->GetTrackData(m_nAudioTrack , &m_sAudioSample);
	if ((VO_U32)nRC == VO_ERR_SOURCE_NEEDRETRY && (m_aVideoRndTime[m_nVideoRndTimeIndex - 1] == m_llLastVideoDecOutTime))
		m_bDataBuffering = VO_TRUE;	
	else if ((VO_U32)nRC != VO_ERR_SOURCE_END && (VO_U32)nRC != VOMP_ERR_EOS) //Rogine update this condition 2014/01/13
		m_bDataBuffering = VO_FALSE;
	m_mtSource.Unlock ();
	m_nAudioDecSize = 0;
#ifdef _VOLOG_INFO
	static int nSrcAudioGetTime = 0;
	static int nTimeStamp = 0;
	if (nRC == VO_ERR_SOURCE_NEEDRETRY)
	{
		VOLOGI("@@@####srcA m_pFileSource->GetTrackData() nRC: VO_ERR_SOURCE_NEEDRETRY");
	}
	else if (nRC == VO_ERR_SOURCE_OK)
	{		
		VOLOGI("@@@####srcA m_pFileSource->GetTrackData() nRC: OK  m_sAudioSample.Time: %lld  diff: %d  sysdiff: %d", m_sAudioSample.Time, (int)m_sAudioSample.Time - nTimeStamp, voOS_GetSysTime() - nSrcAudioGetTime);
		nTimeStamp = (int)m_sAudioSample.Time;
		nSrcAudioGetTime = voOS_GetSysTime();
	}
	else if (nRC == VO_ERR_SOURCE_DROP_FRAME)
	{
		VOLOGI("@@@####srcA m_pFileSource->GetTrackData() nRC: VO_ERR_SOURCE_DROP_FRAME  m_sAudioSample.Time: %lld  diff: %d  sysdiff: %d", m_sAudioSample.Time, (int)m_sAudioSample.Time - nTimeStamp, voOS_GetSysTime() - nSrcAudioGetTime);
		nTimeStamp = (int)m_sAudioSample.Time;
		nSrcAudioGetTime = voOS_GetSysTime();
	}
	else if (nRC == VO_ERR_SOURCE_END)
	{
		VOLOGI("@@@####srcA m_pFileSource->GetTrackData() nRC: VO_ERR_SOURCE_END");
	}
	else
	{
		VOLOGI("@@@####srcA m_pFileSource->GetTrackData() nRC: 0x%08x", nRC);
	}
#endif
	//VOLOGI ("Get audio data return %x, Size %d, Time %d, playtime %d, system  %d", nRC, m_sAudioSample.Size & 0X7FFFFFFF, (int)m_sAudioSample.Time,GetPlayingTime (), voOS_GetSysTime ());

	if(m_nAudioOnOff == 0)
	{
		if(nRC == VO_ERR_NONE)
		{
			nRC = VOMP_ERR_Audio_No_Now;
			m_nAudioReaded = 1;
		}
		else
		{
			m_nAudioReaded = 0;
		}
	}

	//if(m_sAudioSample.Time < 100)
	//	nRC = VO_ERR_SOURCE_NEW_PROGRAM;

	if(nRC != VO_ERR_NONE)
	{
		if((VO_U32)nRC == VO_ERR_SOURCE_END || (VO_U32)nRC == VOMP_ERR_EOS)
		{
			if (!m_bAudioEOS)
			{
				VOLOGI ("Read at end of track!");
			}

			{
				voCAutoLock	lockListen (&m_mtListen);
				if (m_bSendStartAudioBuff)
				{
					m_bSendStartAudioBuff = VO_FALSE;
					postCheckStatusEvent (VOMP_CB_AudioStopBuff);
				}
			}

			if(m_bOnlyGetAudio == VO_TRUE)
				m_bVideoEOS = VO_TRUE;

			return VOMP_ERR_EOS;
		}
		else if ((VO_U32)nRC == VO_ERR_SOURCE_NEEDRETRY || (VO_U32)nRC == VOMP_ERR_Retry)
		{
			// Added by Lin Jun 20110703(Jeff Move here 20111101)
			// Need to sleep if no data(Prevent CPU overload)
			if(m_nAudioPlayMode == VOMP_PUSH_MODE)
				voOS_Sleep(1);
			// end

			if(m_nAudioOnOff == 0)
				m_bOnlyGetVideo = VO_TRUE;

			if (m_bOnlyGetVideo == VO_TRUE)
			{
				memset(m_nAudioBitrate, 0, sizeof(int)*10);
			}
			
			return VOMP_ERR_Retry;
		}
		else if (nRC == VO_ERR_SOURCE_NEW_PROGRAM)
		{
			m_bOnlyGetVideo = VO_FALSE;
			VO_U64 nPlayingTime = GetPlayingTime();
			{
				voCAutoLock lock(&m_mtNewTime);
				if(nPlayingTime > 500 || m_bVideoNewTime || m_nNewTimeBegin > 0)
				{
					VOLOGI("nPlayingTime %lld, m_bVideoNewTime %d, m_nNewTimeBegin %d", nPlayingTime, m_bVideoNewTime, (int)m_nNewTimeBegin);

					m_bAudioNewTime = VO_TRUE;

					if(!m_nNewTimeBegin)
						m_nNewTimeBegin = 2;
				}
			}

			m_nSeekPos = 0;
			bool bAudio = CreateAudioDec ();
			if(bAudio == false)
			{
				if(m_pAudioDec)
				{
					delete m_pAudioDec;
					m_pAudioDec = NULL;
				}
				postCheckStatusEvent (VOMP_CB_CodecNotSupport, m_sAudioTrackInfo.Codec);
				voOS_Sleep(2);

				return VOMP_ERR_Audio;
			}

			if(m_bOnlyGetAudio == VO_FALSE)
			{
				if(m_nAudioVideoType != VOMP_AVAILABLE_AUDIOVIDEO)
				{
					postCheckStatusEvent (VOMP_CB_MediaChanged , (int)VOMP_AVAILABLE_AUDIOVIDEO);
					m_nAudioVideoType = VOMP_AVAILABLE_AUDIOVIDEO;
				}
			}
			else 
			{
				if(m_nAudioVideoType != VOMP_AVAILABLE_PUREAUDIO)
				{
					postCheckStatusEvent (VOMP_CB_MediaChanged , (int)VOMP_AVAILABLE_PUREAUDIO);
					m_nAudioVideoType = VOMP_AVAILABLE_PUREAUDIO;
				}
			}
		}
		else if (nRC == VO_ERR_SOURCE_NEW_FORMAT)
		{
			m_bOnlyGetVideo = VO_FALSE;

			bool bAudio = CreateAudioDec ();
			
			if(bAudio == false)
			{
				if(m_pAudioDec)
				{
					delete m_pAudioDec;
					m_pAudioDec = NULL;
				}
				postCheckStatusEvent (VOMP_CB_CodecNotSupport, m_sAudioTrackInfo.Codec);
				voOS_Sleep(2);

				return VOMP_ERR_Audio;
			}

			if(m_bOnlyGetAudio == VO_FALSE)
			{
				if(m_nAudioVideoType != VOMP_AVAILABLE_AUDIOVIDEO)
				{
					postCheckStatusEvent (VOMP_CB_MediaChanged , (int)VOMP_AVAILABLE_AUDIOVIDEO);
					m_nAudioVideoType = VOMP_AVAILABLE_AUDIOVIDEO;
				}
			}
			else 
			{
				if(m_nAudioVideoType != VOMP_AVAILABLE_PUREAUDIO)
				{
					postCheckStatusEvent (VOMP_CB_MediaChanged , (int)VOMP_AVAILABLE_PUREAUDIO);
					m_nAudioVideoType = VOMP_AVAILABLE_PUREAUDIO;
				}
			}

			//if(!(m_sAudioSample.Size & 0x80000000))
			//	return VOMP_ERR_Retry;
		}
		else if(nRC == VOMP_ERR_Audio_No_Now)
		{
			if(!m_bOnlyGetVideo)
			{
				m_nStartSysTime = 0;
			}

			{
				voCAutoLock lock(&m_mtNewTime);

				VOLOGI("m_nNewTimeBegin %d", (int)m_nNewTimeBegin);

				if(m_nNewTimeBegin > 0)
					m_nNewTimeBegin--;

				if(m_nNewTimeBegin < 0)
					m_nNewTimeBegin = 0;

				m_bOnlyGetVideo = VO_TRUE;
			}
			
			if(m_bOnlyGetAudio == VO_FALSE)
			{
				if(m_nAudioVideoType != VOMP_AVAILABLE_PUREVIDEO)
				{
					postCheckStatusEvent (VOMP_CB_MediaChanged , (int)VOMP_AVAILABLE_PUREVIDEO);
					m_nAudioVideoType = VOMP_AVAILABLE_PUREVIDEO;
				}
			}

			voOS_Sleep(5);
			return VOMP_ERR_Retry;
		}
		else if (nRC == VO_ERR_SOURCE_TIME_RESET)
		{
			VO_U64 nPlayingTime = GetPlayingTime();
			{
				voCAutoLock lock(&m_mtNewTime);
				if(nPlayingTime > 500 || m_bVideoNewTime || m_nNewTimeBegin > 0)
				{
					VOLOGI("nPlayingTime %d, m_bVideoNewTime %d, m_nNewTimeBegin %d", (int)nPlayingTime, m_bVideoNewTime, (int)m_nNewTimeBegin);

					m_bAudioNewTime = VO_TRUE;

					if(!m_nNewTimeBegin)
						m_nNewTimeBegin = 2;
				}
			}

			m_nSeekPos = 0;
		}
		else if(nRC == VOMP_ERR_FLush_Buffer)
		{
			if(m_pAudioDec)
				m_pAudioDec->Flush();

			m_nAudioDecOutLength = 0;
			return VOMP_ERR_Retry;
		}
		else
		{
			VOLOGE ("Read data from track error %08X!", (unsigned int)nRC);
			return VOMP_ERR_File;
		}
	}

	m_bOnlyGetVideo = VO_FALSE;

	if(m_bOnlyGetAudio && m_nAudioVideoType != VOMP_AVAILABLE_PUREAUDIO)
	{
		postCheckStatusEvent (VOMP_CB_MediaChanged , (int)VOMP_AVAILABLE_PUREAUDIO);
		m_nAudioVideoType = VOMP_AVAILABLE_PUREAUDIO;
	}

	if(m_bOnlyGetAudio == VO_FALSE && m_nAudioVideoType != VOMP_AVAILABLE_AUDIOVIDEO)
	{
		postCheckStatusEvent (VOMP_CB_MediaChanged , (int)VOMP_AVAILABLE_AUDIOVIDEO);
		m_nAudioVideoType = VOMP_AVAILABLE_AUDIOVIDEO;
	}

	if((m_sAudioSample.Flag & VOMP_FLAG_VIDEO_EFFECT_ON) == VOMP_FLAG_VIDEO_EFFECT_ON)
	{
		postCheckStatusEvent (VO_LIVESRC_STATUS_HSL_EVENT_THUMBNAIL , (int)m_sAudioSample.Buffer, m_sAudioSample.Size & 0x7FFFFFFF);
		return VOMP_ERR_Retry;
	}

	if(m_sAudioSample.Size <= 0)
	{
		return VOMP_ERR_Retry;
	}

	if(m_bAudioSeekingPos && m_sAudioSample.Time - m_nSeekPos > 2000)
	{
		m_nSeekPos = m_sAudioSample.Time;
		m_nVideoRenderNum = 0;
		m_nVideoRndNuming = 0;
	}

	if (m_pFileSink != NULL && m_smpSinkAudio.Buffer != NULL)
	{
		m_smpSinkAudio.Buffer   = m_sAudioSample.Buffer;
		m_smpSinkAudio.Size     = m_sAudioSample.Size & 0x7FFFFFFF;
		m_smpSinkAudio.Time     = m_sAudioSample.Time;
		m_smpSinkAudio.Duration = 1;
		m_smpSinkAudio.nAV		= 0; // audio
		
		m_pFileSink->AddSample (&m_smpSinkAudio);
	}

	m_sAudioDecInBuf.Buffer = m_sAudioSample.Buffer;
	m_sAudioDecInBuf.Length = m_sAudioSample.Size & 0x7FFFFFFF;
	m_sAudioDecInBuf.Time   = m_sAudioSample.Time;

	VO_U64 nAudioStartTime = (VO_U64)voOS_GetSysTime ();

	if(m_nAudioDateCount == 0)
	{
		m_nAudioCountStartTime = nAudioStartTime;
	}

	m_nAudioDateCount += m_sAudioDecInBuf.Length;

	if(nAudioStartTime - m_nAudioCountStartTime > 1000)
	{
		int i = 0;

		for(i = 0; i < 9; i++)
			m_nAudioBitrate[i] = m_nAudioBitrate[i + 1];

		m_nAudioBitrate [9] = m_nAudioDateCount;

		m_nAudioDateCount = 0;
	}

	if (m_pAudioDec == NULL)
	{
		bool bAudio = CreateAudioDec ();
		if(bAudio == false)
		{
			if(m_pAudioDec)
			{
				delete m_pAudioDec;
				m_pAudioDec = NULL;
			}
			postCheckStatusEvent (VOMP_CB_CodecNotSupport, m_sAudioTrackInfo.Codec);
			voOS_Sleep(2);
			
			return VOMP_ERR_Audio;
		}
	}

	if (m_pAudioDec == NULL)
		return VOMP_ERR_Audio;

#if 0
	static FILE* faac = fopen("/data/local/dump/dump.aac", "wb");
	if(faac)
	{
		//VOLOGI("The input length is %d", m_sAudioDecInBuf.Length);		
		int time = (int)m_sAudioDecInBuf.Time;
		//fwrite(&m_sAudioDecInBuf.Length, 1, 4, faac);
		//fwrite(&time, 1, 4, faac);
		fwrite(m_sAudioDecInBuf.Buffer, 1, m_sAudioDecInBuf.Length, faac);
	}
	else
	{
		VOLOGI("Open file failed");
	}
#endif

	nRC = m_pAudioDec->SetInputData(&m_sAudioDecInBuf);
	mAudioDecInputRetry = (VO_ERR_IOMXDEC_NeedRetry == nRC) ? VO_TRUE : VO_FALSE;

	if(VO_ERR_CODEC_UNSUPPORTED == nRC)
	{
		if(!(m_nAudioVideoUnsupport & 0x01))
		{
			m_nAudioVideoUnsupport |= 0x01;
			postCheckStatusEvent (VOMP_CB_CodecNotSupport, m_sAudioTrackInfo.Codec);
			VOLOGI("Audio codec not support !");
		}
		voOS_Sleep(2);
		return VOMP_ERR_Audio;
	}
	else if(nRC != VO_ERR_NONE && VO_ERR_IOMXDEC_NeedRetry != nRC)
	{
		VOLOGW ("m_pAudioDec->SetInputData result is %08X", (unsigned int)nRC);
		return VOMP_ERR_Retry;
	}

	nRC = DecAudioSample ();
	if (nRC >= 0)
		return nRC;

	return VOMP_ERR_Unknown;
}

int voCMediaPlayer::DecAudioSample (void)
{
	if (m_pAudioDec == NULL)
		return VOMP_ERR_Status;

	// after seek, we use first audio sample time
	if (m_sAudioDecOutBuf.Time == -1)
		m_sAudioDecOutBuf.Time = m_sAudioSample.Time;

	if (m_nAudioDecOutLength >= m_nAudioDecOutStepSize)
	{
		m_sAudioDecOutBuf.Time = m_sAudioSample.Time + m_nAudioDecSize * 1000 / m_nAudioDecOutFrameSize;
		m_nAudioDecOutLength = 0;
	}

	m_sAudioDecOutBuf.Buffer = m_pAudioDecOutBuff + m_nAudioDecOutLength;
	m_sAudioDecOutBuf.Length = m_nAudioDecOutSize - m_nAudioDecOutLength;

	VO_U64	nStartTime = (VO_U64)voOS_GetSysTime ();
	VO_AUDIO_FORMAT	fmtAudio;

#ifdef _VOLOG_INFO
	VO_U64	nStartDecTime = (VO_U64)voOS_GetSysTime ();
	VOLOGI("@@@####decA m_pAudioDec->GetOutputData ===>");
	int nRC = m_pAudioDec->GetOutputData(&m_sAudioDecOutBuf , &fmtAudio);
	//VOLOGI("@@@####decA m_pAudioDec->GetOutputData <===");
	VO_U64 nEndDecTime = (VO_U64)voOS_GetSysTime();

	if (nRC == VO_ERR_NONE)
	{
		VOLOGI("@@@####decA m_pAudioDec->GetOutputData <=== nRC: OK  usetime: %d  m_sAudioDecOutBuf.Time: %lld S: %d B: %d C: %d", (int)(nEndDecTime - nStartDecTime), m_sAudioDecOutBuf.Time, (int)fmtAudio.SampleRate, (int)fmtAudio.SampleBits, (int)fmtAudio.Channels);
	} 
	else if ((nRC & 0X8000000F) == VO_ERR_INPUT_BUFFER_SMALL)
	{
		VOLOGI("@@@####decA m_pAudioDec->GetOutputData <=== nRC: VO_ERR_INPUT_BUFFER_SMALL  usetime: %d", (int)(nEndDecTime - nStartDecTime));
	}
	else if ((nRC & 0X8000000F) == VO_ERR_RETRY)
	{
		VOLOGI("@@@####decA m_pAudioDec->GetOutputData <=== nRC: VO_ERR_RETRY  usetime: %d", (int)(nEndDecTime - nStartDecTime));
	}
	else
	{
		VOLOGI("@@@####decA m_pAudioDec->GetOutputData <=== nRC: 0x%08x  usetime: %d", nRC, (int)(nEndDecTime - nStartDecTime));
	}
#else
	int nRC = m_pAudioDec->GetOutputData(&m_sAudioDecOutBuf , &fmtAudio);
#endif	

	if(VO_ERR_CODEC_UNSUPPORTED == nRC)
	{
		if(!(m_nAudioVideoUnsupport & 0x01))
		{
			m_nAudioVideoUnsupport |= 0x01;
			postCheckStatusEvent (VOMP_CB_CodecNotSupport, m_sAudioTrackInfo.Codec);
			VOLOGI("Audio codec not support !");
		}
		voOS_Sleep(2);
		return VOMP_ERR_Audio;
	}

	if(nRC == VOMP_ERR_None)
	{
		if(mOutputType != m_pAudioDec->getOutputType())
		{
			mOutputType = m_pAudioDec->getOutputType();

			return VOMP_ERR_FormatChange;
		}

		if(fmtAudio.SampleRate != m_sAudioDecOutFormat.SampleRate || fmtAudio.Channels != m_sAudioDecOutFormat.Channels || 
			fmtAudio.SampleBits != m_sAudioDecOutFormat.SampleBits)
		{
			memcpy(&m_sAudioDecOutFormat, &fmtAudio, sizeof(VO_AUDIO_FORMAT));

			return VOMP_ERR_FormatChange;
		}
		
		if (m_pFileSink != NULL && m_smpSinkAudio.Buffer == NULL)
		{
			nRC = m_pFileSink->SetParam (VO_PID_AUDIO_FORMAT, &m_sAudioDecOutFormat);

			m_smpSinkAudio.Buffer   = m_sAudioSample.Buffer;
			m_smpSinkAudio.Size     = m_sAudioSample.Size;
			m_smpSinkAudio.Time     = m_sAudioSample.Time;
			m_smpSinkAudio.Duration = 1;
			m_smpSinkAudio.nAV		= 0; // audio
			
			m_pFileSink->AddSample (&m_smpSinkAudio);
		}

		m_nAudioDecOutLength = m_nAudioDecOutLength + m_sAudioDecOutBuf.Length;
		m_nAudioDecSize = m_nAudioDecSize + m_sAudioDecOutBuf.Length;
		m_nAudioDecTimes = m_nAudioDecTimes + ((VO_U64)voOS_GetSysTime () - nStartTime);

#if 0
		static FILE* fpcm = fopen("/data/local/dump/dump.pcm", "wb");
		if(fpcm)
		{
			fwrite(m_sAudioDecOutBuf.Buffer, 1, m_sAudioDecOutBuf.Length, fpcm);
		}
		else
		{
			VOLOGI("Open file failed");
		}
#endif

		m_nAudioDecError = 0;

		return VOMP_ERR_None;
	}
	else if ((nRC & 0X8000000F) == VO_ERR_INPUT_BUFFER_SMALL)
	{
		if(m_bSeekingPos)
		{
			m_nAudioDecError++;
			if (m_nAudioDecError > 500)
			{
				m_nAudioRenderNum = 2;
				m_nAudioRndNuming = 3;
				m_bSeekingPos = VO_FALSE;
				postCheckStatusEvent (VOMP_CB_SeekComplete);
			}
		}

		return VOMP_ERR_Retry;
	}
	else if ((nRC & 0X8000000F) == VO_ERR_OUTPUT_BUFFER_SMALL)
	{
		if (m_nAudioDecOutLength > 4096 && m_nAudioDecOutStepSize > m_nAudioDecOutLength)
		{
			m_nAudioDecOutStepSize = m_nAudioDecOutLength;
			return VOMP_ERR_None;
		}
	}

	m_nAudioDecError++;
	if (m_nAudioDecError > 500)
	{
		if(m_bSeekingPos)
		{
			m_nAudioRenderNum = 2;
			m_nAudioRndNuming = 3;
			m_bSeekingPos = VO_FALSE;
			postCheckStatusEvent (VOMP_CB_SeekComplete);
		}

		voOS_Sleep(2);
	}

	VOLOGE ("Audio Dec error: %08X", nRC);

	return VOMP_ERR_Retry;
}

int voCMediaPlayer::effectAudioSample_Volume (VO_PBYTE pBuffer, VO_S32 nSize)
{
	if ((m_nAudioVolume >= 0 && m_nAudioVolume < 100) || (m_nAudioVolume > 100 && m_nAudioVolume <= 200))
	{
		if (m_nAudioVolume == 0)
		{
			memset (pBuffer, 0, nSize);
		}
		else
		{
			if (m_sAudioDecOutFormat.SampleBits == 16)
			{
				int nTmp;
				short * pSData;
				pSData = (short *)pBuffer;
				for (int i = 0; i < nSize; i+=2)
				{
					nTmp = ((*pSData) * m_nAudioVolume / 100);
					
					if(nTmp >= -32768 && nTmp <= 32767)
					{
						*pSData = (short)nTmp;
					}
					else if(nTmp < -32768)
					{
						*pSData = -32768;
					}
					else if(nTmp > 32767)
					{
						*pSData = 32767;
					}

					pSData++;
				}
			}
			else if (m_sAudioDecOutFormat.SampleBits == 8)
			{
				int nTmp;
				char * pCData;
				pCData = (char *)pBuffer;
				for (int i = 0; i < nSize; i++)
				{
					nTmp = (*pCData) * m_nAudioVolume / 100;

					if(nTmp >= -256 && nTmp <= 255)
					{
						*pCData = (char)nTmp;
					}
					else if(nTmp < -256)
					{
						*pCData = (char)-256;
					}
					else if(nTmp > 255)
					{
						*pCData = (char)255;
					}

					pCData++;
				}
			}
		}
	}

	return VOMP_ERR_None;
}

int voCMediaPlayer::effectAudioSample_Dolby(VO_PBYTE pBuffer, VO_S32 nSize)
{
	if (m_pAudioEffect != NULL)
	{
		int nRC = VOMP_ERR_None;
		VO_AUDIO_FORMAT	fmtAudio;

		m_sAudioEftInBuf.Buffer = pBuffer;
		m_sAudioEftInBuf.Length = nSize;
		m_pAudioEffect->SetInputData (&m_sAudioEftInBuf);

		m_nAudioRndLength = 0;
		while (nRC == VOMP_ERR_None)
		{
			m_sAudioEftOutBuf.Buffer = m_pAudioRndBuff + m_nAudioRndLength;
			m_sAudioEftOutBuf.Length = m_nAudioRndSize - m_nAudioRndLength;

			memcpy(&fmtAudio, &m_sAudioEftOutFormat, sizeof(VO_AUDIO_FORMAT));
			nRC = m_pAudioEffect->GetOutputData(&m_sAudioEftOutBuf, &fmtAudio);

			if(fmtAudio.SampleRate != m_sAudioEftOutFormat.SampleRate || fmtAudio.Channels != m_sAudioEftOutFormat.Channels || 
				fmtAudio.SampleBits != m_sAudioEftOutFormat.SampleBits)
			{
				memcpy(&m_sAudioEftOutFormat, &fmtAudio, sizeof(VO_AUDIO_FORMAT));

				VOLOGI ("The audio format was changed to SampleRate %d, Channels %d, Bits %d", 
					fmtAudio.SampleRate, fmtAudio.Channels, fmtAudio.SampleBits);

				onAudioOutFormatChanged(&m_sAudioEftOutFormat);

				return VOMP_ERR_FormatChange;
			}

			if (nRC == VOMP_ERR_None)
				m_nAudioRndLength = m_nAudioRndLength + m_sAudioEftOutBuf.Length;
		}
	}

	return VOMP_ERR_None;
}

int voCMediaPlayer::RenderAudioSample (void)
{
	if (m_nAudioDecOutLength < m_nAudioDecOutStepSize)
		return -1;

	VO_AUDIO_FORMAT * pAudioOutFormat = m_pAudioEffect ? &m_sAudioEftOutFormat : &m_sAudioDecOutFormat;

	if (m_pListener != NULL)
	{
		m_bufAudio.pBuffer = m_pAudioRndBuff;
		m_bufAudio.nSize = m_nAudioRndLength;
		m_bufAudio.llTime = m_llAudioRenderTime;

		if (m_pListener (m_pUserData, VOMP_CB_RenderAudio, &m_bufAudio, pAudioOutFormat) == VOMP_ERR_None)
		{
			if (m_pVideoLogoInfo)
			{
				double nDuration = (double)(m_bufAudio.nSize * 8000) / (double)(pAudioOutFormat->Channels * pAudioOutFormat->SampleBits * pAudioOutFormat->SampleRate);
				m_pVideoLogoInfo->m_nStatisPlaybackDurationTime += nDuration;
			}
			return 0;
		}
	}

	if (m_pAudioRender == NULL)
	{
#ifdef _METRO
#elif defined  _WIN32
		m_pAudioRender = new CWaveOutRender (NULL, NULL);
		if(!m_pAudioRender)
			return VOMP_ERR_OutMemory;
		m_pAudioRender->SetFormat (pAudioOutFormat);
		m_pAudioRender->Start ();
#elif defined _IOS
		//m_pAudioRender = new CAudioQueueRender (NULL, NULL);
		m_pAudioRender = new CAudioUnitRender (NULL, NULL);
		if(!m_pAudioRender)
			return VOMP_ERR_OutMemory;
		
#ifdef USE_HW_AUDIO_DEC
		if(m_sAudioTrackInfo.Codec == VOMP_AUDIO_CodingAAC)
			m_pAudioRender->SetInputFormat(INPUT_AAC);
		else if(m_sAudioTrackInfo.Codec == VOMP_AUDIO_CodingMP3)
			m_pAudioRender->SetInputFormat(INPUT_MP3);
#endif
		m_pAudioRender->SetFormat (pAudioOutFormat);
		m_pAudioRender->Start ();
#elif defined _MAC_OS
		m_pAudioRender = new CAudioQueueRender (NULL, NULL);
		if(!m_pAudioRender)
			return VOMP_ERR_OutMemory;
		m_pAudioRender->SetFormat (pAudioOutFormat);
		m_pAudioRender->Start ();
#endif // _WIN32
	}

	if (m_pAudioRender != NULL)
	{
		m_pAudioRender->Render (m_pAudioRndBuff, m_nAudioRndLength, (VO_U64)m_llAudioRenderTime, VO_TRUE);

		if (m_pVideoLogoInfo)
		{
			double nDuration = (double)(m_bufAudio.nSize * 8000) / (double)(pAudioOutFormat->Channels * pAudioOutFormat->SampleBits * pAudioOutFormat->SampleRate);
			m_pVideoLogoInfo->m_nStatisPlaybackDurationTime += nDuration;
		}

#if defined _MAC_OS || defined _IOS
		VO_S32 buffTime = 0;
		m_pAudioRender->GetBufferTime(&buffTime);
		// removed by Lin Jun 20110425
		// by audiounit
		m_nAudioBuffTime = (VO_U64)buffTime;
		//printf("Audio buffer time = %d\n", buffTime);
#endif
	}

	return 0;
}

int voCMediaPlayer::NewVideoCodec (void)
{
	VOLOGI("new video decoder");

	m_bOnlyGetAudio = VO_FALSE;
	if(m_nColorType == VO_COLOR_IOMX_PRIVATE)
	{
		voCAutoLock lockCodec (&m_mtCodec);
		VOLOGI("new video decoder enter m_mtCodec");
		if(m_pVideoDec)
		{
			VO_SOURCE_TRACKINFO	trkInfo;
			m_pFileSource->GetTrackInfo (m_nVideoTrack, &trkInfo);

			if (trkInfo.HeadSize < 128)
			{
				memcpy(m_nVideoBuffHead, trkInfo.HeadData, trkInfo.HeadSize);
			}
			else
			{
				VOLOGE("@@@####srcV trkInfo.HeadSize: %d, this value is too big!", trkInfo.HeadSize);
			}

			if(trkInfo.Codec != VOMP_VIDEO_CodingH264)
			{	
				postCheckStatusEvent (VOMP_CB_CodecNotSupport, trkInfo.Codec);
				voOS_Sleep(2);
				return VOMP_ERR_Video;
			}

			if(VO_TRUE == m_pVideoDec->IsIOMXWorking())
			{
				// if IOMX decoder is working, we need re-create it
				VOLOGI("if IOMX decoder is working, we need re-create it");
#if 0
				// kindle fire HD needn't recreate IOMX decoder, following source code can work well
				VO_CODECBUFFER sBuffer;
				memset(&sBuffer, 0, sizeof(sBuffer));
				sBuffer.Buffer = trkInfo.HeadData;
				sBuffer.Length = trkInfo.HeadSize;
				m_pVideoDec->SetParam(VO_PID_COMMON_HEADDATA, &sBuffer);
				m_pVideoDec->SetInputData(&sBuffer);
#else	// 0
				m_pVideoDec->ResetDecoder(trkInfo.HeadData, trkInfo.HeadSize);
				m_pVideoDec->SetParam(VO_PID_IOMXDEC_SetSurface, m_hView);
#endif	// 0
			}
			else
			{
				// if IOMX decoder isn't working, that means just after init, we just need set head data
				VOLOGI("if IOMX decoder isn't working, that means just after init, we just need set head data");
				if(trkInfo.HeadSize > 0 && NULL != trkInfo.HeadData)
				{
					VO_CODECBUFFER sBuffer;
					memset(&sBuffer, 0, sizeof(sBuffer));
					sBuffer.Buffer = trkInfo.HeadData;
					sBuffer.Length = trkInfo.HeadSize;
					m_pVideoDec->SetParam(VO_PID_COMMON_HEADDATA, &sBuffer);
				}
			}
		}
		else if (m_pVideoDec == NULL)
		{	
			bool bVideo = CreateVideoDec ();
			if(bVideo == false)
			{
				if(m_pVideoDec)
				{
					delete m_pVideoDec;
					m_pVideoDec = NULL;
				}
				postCheckStatusEvent (VOMP_CB_CodecNotSupport, m_sVideoTrackInfo.Codec);
				voOS_Sleep(2);
				return VOMP_ERR_Video;
			}
		}
	}
	else if(m_nColorType == VO_ACODEC_COLOR_TYPE)			
	{
		voCAutoLock lockCodec (&m_mtCodec);
		VOLOGI("new video decoder enter m_mtCodec");
		if(m_pVideoDec)
		{
			VO_SOURCE_TRACKINFO	trkInfo;
			m_pFileSource->GetTrackInfo (m_nVideoTrack, &trkInfo);

			if(trkInfo.Codec != VOMP_VIDEO_CodingH264)
			{	
				postCheckStatusEvent (VOMP_CB_CodecNotSupport, trkInfo.Codec);
				voOS_Sleep(2);
				return VOMP_ERR_Video;
			}
			VO_BOOL bLive = VO_FALSE;
			m_pVideoDec->GetParam(VO_ACODEC_LIVE, &bLive);
			
			if(bLive)
			{
				// at first we use this method to destroy and recreate the video decoder
				// since Amazon suggests that NOT destroy and create. So I optimize it.
#ifdef _LINUX_ANDROID
				if(access("/data/local/acodec_recreate_.txt", F_OK) == 0)
#else	// _LINUX_ANDROID
				if(0)
#endif	// _LINUX_ANDROID
				{
					// if IOMX decoder is working, we need re-create it
					VOLOGI("if Java Media decoder is working, we need re-create it");
					m_pVideoDec->ResetDecoder(trkInfo.HeadData, trkInfo.HeadSize);
					m_pVideoDec->SetParam(VO_ACODEC_SURFACE, m_hView);
					if(mJavaVM)
						m_pVideoDec->SetParam(VO_ACODEC_JVM, mJavaVM);
					m_pVideoOutputBuff->Buffer[0] = (VO_PBYTE)-1; 
				}
				else
				{
					// It is used the default scheme 
					m_pVideoDec->SetParam(VO_ACODEC_RESET, &trkInfo);
				}
			}
			else
			{
				// if IOMX decoder isn't working, that means just after init, we just need set head data
				VOLOGI("if IOMX decoder isn't working, that means just after init, we just need set head data");
				if(trkInfo.HeadSize > 0 && NULL != trkInfo.HeadData)
				{
					VO_CODECBUFFER sBuffer;
					memset(&sBuffer, 0, sizeof(sBuffer));
					sBuffer.Buffer = trkInfo.HeadData;
					sBuffer.Length = trkInfo.HeadSize;
					m_pVideoDec->SetParam(VO_PID_COMMON_HEADDATA, &sBuffer);
				}
			}
		}
		else if (m_pVideoDec == NULL)
		{	
			bool bVideo = CreateVideoDec ();
			if(bVideo == false)
			{
				if(m_pVideoDec)
				{
					delete m_pVideoDec;
					m_pVideoDec = NULL;
				}
				postCheckStatusEvent (VOMP_CB_CodecNotSupport, m_sVideoTrackInfo.Codec);
				voOS_Sleep(2);
				return VOMP_ERR_Video;
			}
		}
	}
	else
	{
		if(!m_nVideoRndCopy && m_ppVideoOutBuffers)
		{
			VO_U32 nVideoIndex, i;
			if (m_ppVideoOutBufShared == NULL)
			{
				m_ppVideoOutBufShared = new VO_VIDEO_BUFFER * [m_nVideoRenderCount];
				if(!m_ppVideoOutBufShared)
					return VOMP_ERR_OutMemory;
				for (i = 0; i < m_nVideoRenderCount; i++)
				{
					m_ppVideoOutBufShared[i] = new VO_VIDEO_BUFFER ();
					if(!m_ppVideoOutBufShared[i])
						return VOMP_ERR_OutMemory;
					memset (m_ppVideoOutBufShared[i], 0, sizeof (VO_VIDEO_BUFFER));
					m_ppVideoOutBufShared[i]->ColorType = VO_COLOR_YUV_PLANAR420;
				}
			}

			for(nVideoIndex = m_nVideoOutPlayIndex; nVideoIndex < m_nVideoOutFillIndex;  nVideoIndex++)
			{
				int nIndex = nVideoIndex % m_nVideoRenderCount;
				VO_VIDEO_BUFFER * pVideoBuffer = m_ppVideoOutBuffers[nIndex];
				if (pVideoBuffer == NULL || pVideoBuffer->Buffer[0] == NULL || pVideoBuffer->Buffer[0] == m_ppVideoOutBufShared[nIndex]->Buffer[0])
					continue;

				if (m_ppVideoOutBufShared[nIndex]->Buffer[0] != NULL)
				{
					delete []m_ppVideoOutBufShared[nIndex]->Buffer[0];
					m_ppVideoOutBufShared[nIndex]->Buffer[0] = NULL;
				}
				m_ppVideoOutBufShared[nIndex]->Buffer[0] = new VO_BYTE[pVideoBuffer->Stride[0] * m_pVideoOutHeight[nIndex] * 3 / 2];
				m_ppVideoOutBufShared[nIndex]->Buffer[1] = m_ppVideoOutBufShared[nIndex]->Buffer[0] + pVideoBuffer->Stride[0] * m_pVideoOutHeight[nIndex];
				m_ppVideoOutBufShared[nIndex]->Buffer[2] = m_ppVideoOutBufShared[nIndex]->Buffer[0] + pVideoBuffer->Stride[0] * m_pVideoOutHeight[nIndex] * 5 / 4;

				m_ppVideoOutBufShared[nIndex]->Stride[0] = pVideoBuffer->Stride[0];
				m_ppVideoOutBufShared[nIndex]->Stride[1] = pVideoBuffer->Stride[0] / 2;
				m_ppVideoOutBufShared[nIndex]->Stride[2] = pVideoBuffer->Stride[0] / 2;

				memcpy(m_ppVideoOutBufShared[nIndex]->Buffer[0], pVideoBuffer->Buffer[0], pVideoBuffer->Stride[0]*m_pVideoOutHeight[nIndex]);
				memcpy(m_ppVideoOutBufShared[nIndex]->Buffer[1], pVideoBuffer->Buffer[1], pVideoBuffer->Stride[1]*m_pVideoOutHeight[nIndex]/2);
				memcpy(m_ppVideoOutBufShared[nIndex]->Buffer[2], pVideoBuffer->Buffer[2], pVideoBuffer->Stride[2]*m_pVideoOutHeight[nIndex]/2);

				m_ppVideoOutBufShared[nIndex]->Time = pVideoBuffer->Time;
				m_ppVideoOutBufShared[nIndex]->CodecData = pVideoBuffer->CodecData;

				voCAutoLock lockVideoDec (&m_mtVideoRender);
				memcpy(pVideoBuffer, m_ppVideoOutBufShared[nIndex], sizeof(VO_VIDEO_BUFFER));

				voOS_Sleep(2);
			}

#ifndef _LINUX_ANDROID
			// after memcpy of output buffers, we need wait at least one shared buffers output
			m_mtVideoRender.Lock();
			VO_U32 nVideoIndexOut = m_nVideoOutPlayIndex;
			m_mtVideoRender.Unlock();

			for(VO_U32 n = 0; n < 50; n++)
			{
				voCAutoLock lockVideoDec(&m_mtVideoRender);
				if(m_nVideoOutPlayIndex > nVideoIndexOut || m_nVideoOutPlayIndex >= m_nVideoOutFillIndex)
				{
					VOLOGI("nVideoIndexOut %d, m_nVideoOutPlayIndex %d, m_nVideoOutFillIndex %d n %d", 
						nVideoIndexOut, m_nVideoOutPlayIndex, m_nVideoOutFillIndex, n);
					break;
				}

				voOS_Sleep(2);
			}
#endif

			// all buffers finish copying, so cancel them to old decoder which will be destroyed soon after
			cancelBuffersFromVOVideoDec(VO_TRUE);
			{
				voCAutoLock lockVideoDec (&m_mtVideoRender);
				if(m_pVideoOutputBuff)
					m_pVideoOutputBuff->CodecData = NULL;
			}
		}

		voCAutoLock lockCodec (&m_mtCodec);
		VOLOGI("new video decoder enter m_mtCodec");

		VO_SOURCE_TRACKINFO	trkInfo;
		m_pFileSource->GetTrackInfo(m_nVideoTrack, &trkInfo);
		if(VOMP_VIDEO_CodingH265 == trkInfo.Codec && (VO_COLOR_IOMX_PRIVATE != m_nColorType && VO_ACODEC_COLOR_TYPE != m_nColorType))
			m_bNewVOVideoDec = VO_TRUE;
		else
			m_bNewVOVideoDec = VO_FALSE;

		VOLOGI("Codec %d, m_bNewVOVideoDec %d", trkInfo.Codec, m_bNewVOVideoDec);

		bool bVideo = CreateVideoDec ();
		if(bVideo == false)
		{
			if(m_pVideoDec)
			{
				delete m_pVideoDec;
				m_pVideoDec = NULL;
			}
			postCheckStatusEvent (VOMP_CB_CodecNotSupport, m_sVideoTrackInfo.Codec);
			voOS_Sleep(2);
			return VOMP_ERR_Video;
		}
	}

	if(m_bOnlyGetVideo == VO_FALSE)
	{
		if(m_nAudioVideoType != VOMP_AVAILABLE_AUDIOVIDEO)
		{
			postCheckStatusEvent (VOMP_CB_MediaChanged , (int)VOMP_AVAILABLE_AUDIOVIDEO);
			m_nAudioVideoType = VOMP_AVAILABLE_AUDIOVIDEO;
		}
	}
	else
	{
		if(m_nAudioVideoType != VOMP_AVAILABLE_PUREVIDEO)
		{
			postCheckStatusEvent (VOMP_CB_MediaChanged , (int)VOMP_AVAILABLE_PUREVIDEO);
			m_nAudioVideoType = VOMP_AVAILABLE_PUREVIDEO;
		}
	}

	return 0;
}

int voCMediaPlayer::ReadVideoSample (void)
{
	VO_S32 nRC = VO_ERR_NONE;
	if (m_pVideoDec != NULL)
	{
		if(VOMP_RECREATEDECODERSTATE_NONE != m_eRecreateDecoderState)
		{
			if(VOMP_RECREATEDECODERSTATE_NEEDRECREATE == m_eRecreateDecoderState)
			{
				if(m_nColorType == VO_COLOR_IOMX_PRIVATE)
				{
					if(VO_ERR_NONE == m_pVideoDec->SetParam(VO_PID_IOMXDEC_ForceOutputAll, NULL))
					{
						voOS_Sleep(5);	// sleep 5 ms to wait IOMX handle EOS

						VOLOGI("VOMP_RECREATEDECODERSTATE_FINISHSETFORCEOUTPUT");
						m_eRecreateDecoderState = VOMP_RECREATEDECODERSTATE_FINISHSETFORCEOUTPUT;
					}
				}
				else if(VO_ACODEC_COLOR_TYPE == m_nColorType)
				{
					if(VO_ERR_NONE == m_pVideoDec->SetParam(VO_ACODEC_EOS, NULL))
					{
						voOS_Sleep(5);	// sleep 5 ms to wait IOMX handle EOS

						VOLOGI("VOMP_RECREATEDECODERSTATE_SENDEOS");
						m_eRecreateDecoderState = VOMP_RECREATEDECODERSTATE_SENDEOS;
					}
				}
			}

			nRC = DecVideoSample ();
			if(VOMP_RECREATEDECODERSTATE_FINISHSETFORCEOUTPUT == m_eRecreateDecoderState)
			{
				if(VO_ERR_NONE != nRC)
				{
					if(m_nColorType != VO_COLOR_IOMX_PRIVATE && m_bMoreVideoBuff)
					{
						VOLOGI("m_eRecreateDecoderState %d nRC 0x%08X", m_eRecreateDecoderState, nRC);
						return nRC;
					}

					// all output buffer are finished
					VOLOGI("VOMP_RECREATEDECODERSTATE_NONE, nRC 0x%08X, m_bMoreVideoBuff %d", nRC, m_bMoreVideoBuff);
					m_eRecreateDecoderState = VOMP_RECREATEDECODERSTATE_NONE;
					NewVideoCodec();
				}
				else
					return nRC;
			}
			else if(VOMP_RECREATEDECODERSTATE_SENDEOS == m_eRecreateDecoderState)
			{
				if(VO_ERR_NONE != nRC)
				{
					if(m_nColorType != VO_ACODEC_COLOR_TYPE && m_bMoreVideoBuff)
					{
						VOLOGI("m_eRecreateDecoderState %d nRC 0x%08X", m_eRecreateDecoderState, nRC);
						return nRC;
					}
						
					// all output buffer are finished
					VOLOGI("VOMP_RECREATEDECODERSTATE_NONE, nRC 0x%08X, m_bMoreVideoBuff %d", nRC, m_bMoreVideoBuff);
					m_eRecreateDecoderState = VOMP_RECREATEDECODERSTATE_NONE;
					NewVideoCodec();
				}
				else
					return nRC;
			}
			else
				return nRC;
		}
		else if(m_sVideoSample.Size > 0 || m_nVideoEOSArrived == 1)
		{
			if(m_bMoreVideoBuff && !m_bNeedInputData)
			{
				nRC = DecVideoSample ();
				if(nRC == VOMP_ERR_None || nRC == VOMP_ERR_Retry || nRC == VOMP_ERR_FormatChange)
					return nRC;
			}
			else if(m_bVideoDecInputRetry)
			{
				m_sVideoDecInBuf.Buffer = m_sVideoSample.Buffer;
				m_sVideoDecInBuf.Length = m_sVideoSample.Size & 0x7FFFFFFF;
				m_sVideoDecInBuf.Time   = m_sVideoSample.Time;
				m_sVideoDecInBuf.UserData = (VO_PTR)(&m_sVideoSample.Flag);

				nRC = m_pVideoDec->SetInputData(&m_sVideoDecInBuf);
				VOLOGI("@@@####decV SetInputData time %lld, len %d, nRC 0x%08X", m_sVideoDecInBuf.Time, m_sVideoDecInBuf.Length, nRC);

				m_bVideoDecInputRetry = VO_FALSE;
				m_bVideoDecHungry = VO_FALSE;
				if(nRC != VO_ERR_NONE)
				{
					if(VO_ERR_IOMXDEC_NeedRetry == nRC || VO_ERR_RETRY == nRC)
						m_bVideoDecInputRetry = VO_TRUE;
					else
					{
						if(VO_ERR_INPUT_BUFFER_SMALL == nRC)
							m_bVideoDecHungry = VO_TRUE;

						return VOMP_ERR_Retry;
					}
				}
				else
					m_llLastVideoDecInTime = m_sVideoDecInBuf.Time;

				nRC = DecVideoSample ();

				return nRC;
			}
		}
	}

	VO_U64 ullPlayingTime = GetPlayingTime();
	if(VO_ERR_SOURCE_NEEDRETRY == m_nRCGetVideoTrackDataAfterPureAudio)
	{
		if (m_nDisableDropVideoFrame == 0 && m_nGetThumbnail == 0)
		{
			if(m_nVideoOnOff == 0 || m_nVideoPlayBack == 0 )
			{
				if(m_sVideoSample.Time > (VO_S64)ullPlayingTime + 50 && m_nVideoReaded)
				{
					voOS_Sleep((int)(m_sVideoSample.Time - (VO_S64)ullPlayingTime - 20));
				}
			}

			VO_S64 nCurrVideoDecBufferTime = 0;
			if(m_bNewVOVideoDec && m_llLastVideoDecOutTime > 0 && m_llLastVideoDecInTime > m_llLastVideoDecOutTime)
				nCurrVideoDecBufferTime = m_llLastVideoDecInTime - m_llLastVideoDecOutTime;

// 			VOLOGI("nCurrVideoDecBufferTime %lld, m_llLastVideoDecInTime %lld, m_llLastVideoDecOutTime %lld, ullPlayingTime %lld", 
// 				nCurrVideoDecBufferTime, m_llLastVideoDecInTime, m_llLastVideoDecOutTime, ullPlayingTime);

#if defined(_IOS) || defined(_MAC_OS)
			if (m_nDisableDropVideoFrame == 0 && ullPlayingTime > 1000 && m_nVideoRenderNum > 0 && m_bAudioBufFull)
#else
			if (m_nDisableDropVideoFrame == 0 && ullPlayingTime > 1000 && m_nVideoRenderNum > 0)
#endif	
				m_sVideoSample.Time = ullPlayingTime + nCurrVideoDecBufferTime - 90;
			else
				m_sVideoSample.Time = 0;

			if(m_bSeekingPos)
			{
				if(ullPlayingTime >= 90)
					m_sVideoSample.Time = ullPlayingTime + nCurrVideoDecBufferTime - 90;
				else
					m_sVideoSample.Time = 0;
			}

			if(m_nNewTimeBegin)
				m_sVideoSample.Time = 0;

			int nLastTime, nDelayTime;
			if (m_pVideoOutputBuff != NULL && m_nVideoRndNuming > 60 )
			{
				nLastTime = (int)m_pVideoOutputBuff->Time;
				nDelayTime = (int)ullPlayingTime - nLastTime;
			}
			else
			{
				nLastTime = (int)m_sVideoSample.Time;
				nDelayTime = 0;
			}

			if((VO_U64)voOS_GetSysTime () - m_nLastAudioSysTime > 2 * m_nAudioStepTime)
			{
				nDelayTime = 0;
			}

			if(m_bOnlyGetAudio || m_bSeekingPos || m_nNewTimeBegin)
				m_sVideoSample.Duration = 0;
			else
				m_sVideoSample.Duration = nDelayTime;

			//VOLOGI ("Read data from source Delay time %d", nDelayTime);

			m_sVideoSample.Flag = 0;
		}

		if(m_pVideoLogoInfo)
			m_pVideoLogoInfo->GetSourceStart(&m_sVideoSample);

		m_mtSource.Lock ();
		//VO_U32 usetime = voOS_GetSysTime();
		VOLOGI("@@@####srcV m_pFileSource->GetTrackData =====> m_sVideoSample.Time: %d", (int)m_sVideoSample.Time);
		nRC = m_pFileSource->GetTrackData(m_nVideoTrack , &m_sVideoSample);
		//VOLOGI("@@@####srcV m_pFileSource->GetTrackData <=====	usetime: %u", voOS_GetSysTime()-usetime);
		m_mtSource.Unlock ();

		if(m_pVideoLogoInfo)
			m_pVideoLogoInfo->GetSourceEnd(&m_sVideoSample, nRC);

#ifdef _VOLOG_INFO	
		static int nsrcVideoGetTime = 0;
		static int nTimeStamp = 0;
		if (nRC == VO_ERR_SOURCE_NEEDRETRY)
		{
			VOLOGI("@@@####srcV m_pFileSource->GetTrackData <===== nRC: VO_ERR_SOURCE_NEEDRETRY");
		}
		else if (nRC == VO_ERR_SOURCE_OK)
		{		
			VOLOGI("@@@####srcV m_pFileSource->GetTrackData <===== nRC: OK  m_sVideoSample.Time: %lld  diff: %d  sysdiff: %d", m_sVideoSample.Time, (int)m_sVideoSample.Time - nTimeStamp, voOS_GetSysTime() - nsrcVideoGetTime);
			nTimeStamp = (int)m_sVideoSample.Time;
			nsrcVideoGetTime = voOS_GetSysTime();
		}
		else if (nRC == VO_ERR_SOURCE_DROP_FRAME)
		{
			VOLOGI("@@@####srcV m_pFileSource->GetTrackData <===== nRC: VO_ERR_SOURCE_DROP_FRAME  m_sVideoSample.Time: %lld  diff: %d  sysdiff: %d", m_sVideoSample.Time, (int)m_sVideoSample.Time - nTimeStamp, voOS_GetSysTime() - nsrcVideoGetTime);
			nTimeStamp = (int)m_sVideoSample.Time;
			nsrcVideoGetTime = voOS_GetSysTime();
		}
		else if (nRC == VO_ERR_SOURCE_END)
		{
			VOLOGI("@@@####srcV m_pFileSource->GetTrackData <===== nRC: VO_ERR_SOURCE_END");
		}
		else
		{
			VOLOGI("@@@####srcV m_pFileSource->GetTrackData <===== nRC: 0x%08x", nRC);
		}
#endif
		if(VOMP_AVAILABLE_PUREAUDIO == m_nAudioVideoType && VO_ERR_SOURCE_NEEDRETRY != nRC)
		{
			VOLOGI("not VO_ERR_SOURCE_NEEDRETRY after pure audio nRC 0x%08X, time %lld", nRC, m_sVideoSample.Time);
			m_nRCGetVideoTrackDataAfterPureAudio = nRC;
		}
	}

	if(VO_ERR_SOURCE_NEEDRETRY != m_nRCGetVideoTrackDataAfterPureAudio)
	{
		if(m_sVideoSample.Time < ullPlayingTime + 500)
		{
			VOLOGI("hold video buffer end after pure audio time %lld, playing time %lld, sys time %d", m_sVideoSample.Time, ullPlayingTime, voOS_GetSysTime());
			nRC = m_nRCGetVideoTrackDataAfterPureAudio;
			m_nRCGetVideoTrackDataAfterPureAudio = VO_ERR_SOURCE_NEEDRETRY;
		}
		else
			nRC = VO_ERR_SOURCE_NEEDRETRY;
	}

	if (m_nAudioTrack < 0)
	{
		if ((VO_U32)nRC == VO_ERR_SOURCE_NEEDRETRY)
		{
			if(m_bDataBuffering == VO_FALSE)
				m_nVideoStartTime = (VO_S64)voOS_GetSysTime ();
			m_bDataBuffering = VO_TRUE;
		}
		else
		{
			if(m_bDataBuffering && (VO_S64)voOS_GetSysTime ()  - m_nVideoStartTime > 40)
			{
				VOLOGI ("Read data from source using time %d", voOS_GetSysTime ()  - (int)m_nVideoStartTime);
			}	
			m_bDataBuffering = VO_FALSE;
		}
	}

//	VOLOGI ("Get video data return %x, Size %d, Time %lld, playtime %lld, system  %d", nRC, m_sVideoSample.Size & 0X7FFFFFFF, m_sVideoSample.Time, ullPlayingTime, voOS_GetSysTime());

	if(m_nVideoOnOff == 0 || m_nVideoPlayBack == 0)
	{
		if(nRC == VO_ERR_NONE)
		{
			nRC = VOMP_ERR_Video_No_Now;
			m_nVideoReaded = 1;

			if(m_nColorType == VO_COLOR_IOMX_PRIVATE && m_pVideoDec)
			{
				delete m_pVideoDec;
				m_pVideoDec = NULL;
			}
			else if(VO_ACODEC_COLOR_TYPE == m_nColorType && m_pVideoDec)
			{
				delete m_pVideoDec;
				m_pVideoDec = NULL;
			}
		}
		else
		{
			m_nVideoReaded = 0;
		}
	}

	if(nRC != VO_ERR_NONE)
	{
		if((VO_U32)nRC == VO_ERR_SOURCE_END || (VO_U32)nRC == VOMP_ERR_EOS)
		{
			if(m_pVideoDec)
			{
				if(m_nColorType == VO_COLOR_IOMX_PRIVATE)
					m_pVideoDec->SetParam(VO_PID_IOMXDEC_ForceOutputAll, NULL);
				else if(VO_ACODEC_COLOR_TYPE == m_nColorType)
					m_pVideoDec->SetParam(VO_ACODEC_EOS, NULL);
				else
					flushPicturesOfVOVideoDec();
			}

			m_bMoreVideoBuff = VO_TRUE;
			m_bNeedInputData = VO_FALSE;

			{
				voCAutoLock	lockListen (&m_mtListen);
				if(m_bSendStartVideoBuff)
				{
					m_bSendStartVideoBuff = VO_FALSE;
					postCheckStatusEvent (VOMP_CB_VideoStopBuff);
				}
			}

			if(m_nVideoEOSArrived == 0)
				m_nVideoEOSArrived = 1;

			if(m_nVideoRenderNum == 0)
				m_nVideoEOSArrived = 2;

			m_nStatusChanging = 0;

			voOS_Sleep (10);
			return VOMP_ERR_Retry;
		}
		else if ((VO_U32)nRC == VO_ERR_SOURCE_NEEDRETRY || nRC == VOMP_ERR_Retry)
		{
			if(m_nVideoOnOff == 0)
				m_bOnlyGetAudio = VO_TRUE;

			if(m_nVideoEOSArrived)
			{
				m_bMoreVideoBuff = VO_TRUE;
				m_bNeedInputData = VO_FALSE;
			}

			if (m_bOnlyGetAudio == VO_TRUE)
			{
				memset(m_nVideoBitrate, 0, sizeof(int)*10);
			}			
			
			return VOMP_ERR_Retry;
		}
		else if (nRC == VO_ERR_SOURCE_NEW_PROGRAM)
		{
			m_nSeekPos = 0;
			m_nNewTimeVideo = 0;
			m_nStatusChanging = 0;

			if(VO_COLOR_IOMX_PRIVATE == m_nColorType)
			{
				if(m_pVideoDec && VO_TRUE == m_pVideoDec->IsIOMXWorking())
				{
					VO_SOURCE_TRACKINFO	trkInfo;
					m_pFileSource->GetTrackInfo (m_nVideoTrack, &trkInfo);
					VO_S32 ret = cmnMemCompare(0, m_nVideoBuffHead, trkInfo.HeadData, trkInfo.HeadSize);
					if (trkInfo.HeadSize < 128)
					{
						memcpy(m_nVideoBuffHead, trkInfo.HeadData, trkInfo.HeadSize);
					}
					else
					{
						VOLOGE("@@@####srcV trkInfo.HeadSize: %d, this value is too big!", trkInfo.HeadSize);
					}
					if (ret != 0)
					{
						VOLOGI("VOMP_RECREATEDECODERSTATE_NEEDRECREATE");
						m_eRecreateDecoderState = VOMP_RECREATEDECODERSTATE_NEEDRECREATE;

						if(VO_ERR_NONE == m_pVideoDec->SetParam(VO_PID_IOMXDEC_ForceOutputAll, NULL))
						{
							voOS_Sleep(5);	// sleep 5 ms to wait IOMX handle EOS

							VOLOGI("VOMP_RECREATEDECODERSTATE_FINISHSETFORCEOUTPUT");
							m_eRecreateDecoderState = VOMP_RECREATEDECODERSTATE_FINISHSETFORCEOUTPUT;
						}
					}
				}
				else
					NewVideoCodec();
			}
			else if (m_nColorType == VO_ACODEC_COLOR_TYPE) // added by gtxia 
			{
				if(m_pVideoDec)
				{
					VO_BOOL bLive = VO_FALSE;
					m_pVideoDec->GetParam(VO_ACODEC_LIVE, &bLive);
					if(bLive)
					{
						// should I do ????
						VOLOGI("VOMP_RECREATEDECODERSTATE_NEEDRECREATE");
						m_eRecreateDecoderState = VOMP_RECREATEDECODERSTATE_NEEDRECREATE;
						if(VO_ERR_NONE == m_pVideoDec->SetParam(VO_ACODEC_EOS, NULL))
						{
							voOS_Sleep(5);	// sleep 5 ms to wait IOMX handle EOS

							VOLOGI("VOMP_RECREATEDECODERSTATE_SENDEOS");
							m_eRecreateDecoderState = VOMP_RECREATEDECODERSTATE_SENDEOS;
						}
					}
				}
				else
				{
					NewVideoCodec();	
				}

			}
			else
			{
				if(VOMP_ERR_None == flushPicturesOfVOVideoDec())
				{
					m_bMoreVideoBuff = VO_TRUE;
					m_bNeedInputData = VO_FALSE;
					VOLOGI("VOMP_RECREATEDECODERSTATE_FINISHSETFORCEOUTPUT");
					m_eRecreateDecoderState = VOMP_RECREATEDECODERSTATE_FINISHSETFORCEOUTPUT;
				}
				else
					NewVideoCodec();
			}

			{
				voCAutoLock lock(&m_mtNewTime);

				if(ullPlayingTime > 500 || m_bAudioNewTime || m_nNewTimeBegin > 0)
				{
					VOLOGI("nPlayingTime %lld, m_bAudioNewTime %d, m_nNewTimeBegin %d", ullPlayingTime, m_bAudioNewTime, (int)m_nNewTimeBegin);

					m_bVideoNewTime = VO_TRUE;

					if(!m_nNewTimeBegin)
						m_nNewTimeBegin = 2;
				}
			}
		}
		else if (nRC == VO_ERR_SOURCE_NEW_FORMAT)
		{
			m_nStatusChanging = 0;
			if(VO_COLOR_IOMX_PRIVATE == m_nColorType)
			{
				if(m_pVideoDec && VO_TRUE == m_pVideoDec->IsIOMXWorking())
				{
					VO_SOURCE_TRACKINFO	trkInfo;
					m_pFileSource->GetTrackInfo (m_nVideoTrack, &trkInfo);
					VO_S32 ret = cmnMemCompare(0, m_nVideoBuffHead, trkInfo.HeadData, trkInfo.HeadSize);
					if (trkInfo.HeadSize < 128)
					{
						memcpy(m_nVideoBuffHead, trkInfo.HeadData, trkInfo.HeadSize);
					}
					else
					{
						VOLOGE("@@@####srcV trkInfo.HeadSize: %d, this value is too big!", trkInfo.HeadSize);
					}
					if (ret != 0)
					{
						VOLOGI("VOMP_RECREATEDECODERSTATE_NEEDRECREATE");
						m_eRecreateDecoderState = VOMP_RECREATEDECODERSTATE_NEEDRECREATE;

						if(VO_ERR_NONE == m_pVideoDec->SetParam(VO_PID_IOMXDEC_ForceOutputAll, NULL))
						{
							voOS_Sleep(5);	// sleep 5 ms to wait IOMX handle EOS

							VOLOGI("VOMP_RECREATEDECODERSTATE_FINISHSETFORCEOUTPUT");
							m_eRecreateDecoderState = VOMP_RECREATEDECODERSTATE_FINISHSETFORCEOUTPUT;
						}
					}
				}
				else
					NewVideoCodec();
			}
			else if (m_nColorType == VO_ACODEC_COLOR_TYPE) // added by gtxia 
			{
				if(m_pVideoDec)
				{
					VO_BOOL bLive = VO_FALSE;
					m_pVideoDec->GetParam(VO_ACODEC_LIVE, &bLive);
					if(bLive)
					{
					// should I do ????
						VOLOGI("VOMP_RECREATEDECODERSTATE_NEEDRECREATE");
						m_eRecreateDecoderState = VOMP_RECREATEDECODERSTATE_NEEDRECREATE;
						if(VO_ERR_NONE == m_pVideoDec->SetParam(VO_ACODEC_EOS, NULL))
						{
							voOS_Sleep(5);	// sleep 5 ms to wait IOMX handle EOS

							VOLOGI("VOMP_RECREATEDECODERSTATE_SENDEOS");
							m_eRecreateDecoderState = VOMP_RECREATEDECODERSTATE_SENDEOS;
						}
					}
				}
				else
					NewVideoCodec();
			}
			else
			{
				if(VOMP_ERR_None == flushPicturesOfVOVideoDec())
				{
					m_bMoreVideoBuff = VO_TRUE;
					m_bNeedInputData = VO_FALSE;
					VOLOGI("VOMP_RECREATEDECODERSTATE_FINISHSETFORCEOUTPUT");
					m_eRecreateDecoderState = VOMP_RECREATEDECODERSTATE_FINISHSETFORCEOUTPUT;
				}
				else
					NewVideoCodec();
			}
		}
		else if (nRC == VO_ERR_SOURCE_TIME_RESET)
		{
			m_nSeekPos = 0;
			{
				voCAutoLock lock(&m_mtNewTime);

				m_nNewTimeVideo = 0;
				if(ullPlayingTime > 500 || m_bAudioNewTime || m_nNewTimeBegin > 0)
				{
					VOLOGI("nPlayingTime %lld, m_bAudioNewTime %d, m_nNewTimeBegin %d", ullPlayingTime, m_bAudioNewTime, (int)m_nNewTimeBegin);

					m_bVideoNewTime = VO_TRUE;
					if(!m_nNewTimeBegin)
						m_nNewTimeBegin = 2;
				}
			}
		}
		else if(nRC == VO_ERR_SOURCE_DROP_FRAME)
		{
			VOLOGI("@@@####srcV  VO_ERR_SOURCE_DROP_FRAME");
			m_nStatusChanging = 0;
			return VOMP_ERR_Retry;
		}
		else if(nRC == VOMP_ERR_Video_No_Now)
		{
			m_nVideoRenderNum = 0;
			m_nVideoRndNuming = 0;
			m_nLastFrameTime = 0;
			m_nReceiveFrameTime = 0;
			m_nReceiveFrameNum = 0;
			if(m_nColorType != VO_COLOR_IOMX_PRIVATE && m_nColorType != VO_ACODEC_COLOR_TYPE)
			{
				cancelBuffersFromVOVideoDec();
				if(m_pVideoDec)
					m_pVideoDec->Flush();
			}

			{
				voCAutoLock lock(&m_mtNewTime);

				VOLOGI("m_nNewTimeBegin %d", (int)m_nNewTimeBegin);

				if(m_nNewTimeBegin > 0)
					m_nNewTimeBegin--;

				if(m_nNewTimeBegin < 0)
					m_nNewTimeBegin = 0;

				m_bOnlyGetAudio = VO_TRUE;
			}


			if(m_bOnlyGetVideo == VO_FALSE)
			{
				if(m_nAudioVideoType != VOMP_AVAILABLE_PUREAUDIO)
				{
					postCheckStatusEvent (VOMP_CB_MediaChanged , (int)VOMP_AVAILABLE_PUREAUDIO);
					m_nAudioVideoType = VOMP_AVAILABLE_PUREAUDIO;
				}
			}

			m_nStatusChanging = 0;

			voCAutoLock lockVideoDec (&m_mtVideoRender);
			if(m_nVideoOutFillIndex > m_nVideoOutPlayIndex)
			{
				m_nVideoOutFillIndex = m_nVideoOutPlayIndex+1;
			}

			voOS_Sleep(5);

			return VOMP_ERR_Retry;
		}
		else if(nRC == VOMP_ERR_FLush_Buffer)
		{
			voCAutoLock lockVideoDec (&m_mtVideoRender);
			cancelBuffersFromVOVideoDec();

			m_nVideoOutPlayIndex = 0;
			m_nVideoOutFillIndex = 0;
			if(m_pVideoDec)
				m_pVideoDec->Flush();			
			return VOMP_ERR_Retry;
		}
		else
		{
			VOLOGE ("Read data from track error %08X!", (unsigned int)nRC);
			return VOMP_ERR_Unknown;
		}
	}

	m_bOnlyGetAudio = VO_FALSE;
	m_nStatusChanging = 0;

	if(m_bOnlyGetVideo && m_nAudioVideoType != VOMP_AVAILABLE_PUREVIDEO)
	{
		postCheckStatusEvent (VOMP_CB_MediaChanged , (int)VOMP_AVAILABLE_PUREVIDEO);
		m_nAudioVideoType = VOMP_AVAILABLE_PUREVIDEO;
	}

	if(m_bOnlyGetVideo == VO_FALSE && m_nAudioVideoType != VOMP_AVAILABLE_AUDIOVIDEO)
	{
		postCheckStatusEvent (VOMP_CB_MediaChanged , (int)VOMP_AVAILABLE_AUDIOVIDEO);
		m_nAudioVideoType = VOMP_AVAILABLE_AUDIOVIDEO;
	}

	if(m_nLastFrameTime != 0)
	{
		if(m_sVideoSample.Time > m_nLastFrameTime && abs((VO_S32)m_sVideoSample.Time - (VO_S32)m_nLastFrameTime) < 30000 && !m_bSeekingPos)
		{
			m_nReceiveFrameTime += m_sVideoSample.Time - m_nLastFrameTime;
		}
		else if(abs((VO_S32)m_sVideoSample.Time - (VO_S32)m_nLastFrameTime) >= 30000 || m_bSeekingPos)
		{
			m_nReceiveFrameTime = m_nFrameRate;
			m_nReceiveFrameNum = 0;
		}

		if(m_sVideoSample.Time > m_nLastFrameTime || m_nLastFrameTime - m_sVideoSample.Time > 300)
		{
			m_nLastFrameTime =  m_sVideoSample.Time;
		}

		m_nReceiveFrameNum++;
		
		m_nFrameRate = (VO_S32)m_nReceiveFrameTime/m_nReceiveFrameNum;

		if(m_nFrameRate == 0)
			m_nFrameRate = 33;
	}
	else
	{
		m_nLastFrameTime = m_sVideoSample.Time;
	}

	if(m_nIFrameSkip)
	{
		if(m_sVideoSample.Size&0x80000000)
			m_nIFrameSkip = 0;
		else
			return VOMP_ERR_Retry;
	}

	m_sVideoDecInBuf.Buffer = m_sVideoSample.Buffer;
	m_sVideoDecInBuf.Length = m_sVideoSample.Size & 0x7FFFFFFF;
	m_sVideoDecInBuf.Time   = m_sVideoSample.Time;
	m_sVideoDecInBuf.UserData = (VO_PTR)(&m_sVideoSample.Flag);

	if(m_bSeekingPos && m_nSeekPos - m_sVideoDecInBuf.Time > 5000 && nRC == 0)
	{
		m_nSeekPos = m_sVideoDecInBuf.Time;
		m_sAudioSample.Time = m_nSeekPos;
		m_sVideoSample.Time = m_nSeekPos;

		m_sVideoDecOutBuf.Time = m_nSeekPos;
		m_sAudioDecOutBuf.Time = -1;
		m_nStartMediaTime = m_nSeekPos;
		m_llAudioRenderTime = m_nSeekPos;
		m_llLastAudioRenderTime = m_nSeekPos;
	}

#if 0
	static FILE* f264 = fopen("/data/local/dump/dump.H264", "wb");
	static int BeginWrite = 0;
	if(f264)
	{
		fwrite(m_sVideoDecInBuf.Buffer, 1, m_sVideoDecInBuf.Length, f264);
	}
	else
	{
		VOLOGI("Open file failed");
	}
#endif

#if defined DUMP_VIDEO_HEAD
	if(m_fDumpVideoinput)
	{
		fwrite(m_sVideoDecInBuf.Buffer, m_sVideoDecInBuf.Length, 1, m_fDumpVideoinput);
	}
	else
	{
		VOLOGI("Open file failed");
	}
#endif

	VO_S64 nVideoStartTime = (VO_S64)voOS_GetSysTime ();

	if(m_nVideoDateCount == 0)
	{
		m_nVideoCountStartTime = nVideoStartTime;
	}

	m_nVideoDateCount += m_sVideoDecInBuf.Length;

	if(nVideoStartTime - m_nVideoCountStartTime > 1000)
	{
		int i = 0;

		for(i = 0; i < 9; i++)
			m_nVideoBitrate[i] = m_nVideoBitrate[i + 1];

		m_nVideoBitrate [9] = m_nVideoDateCount;

		m_nVideoDateCount = 0;
	}

	if (m_pVideoDec == NULL)
	{
		bool bVideo = CreateVideoDec ();
		if(bVideo == false)
		{
			if(m_pVideoDec)
			{
				delete m_pVideoDec;
				m_pVideoDec = NULL;
			}
			postCheckStatusEvent (VOMP_CB_CodecNotSupport, m_sVideoTrackInfo.Codec);
			voOS_Sleep(2);
			return VOMP_ERR_Video;
		}
	}

	if ( m_sVideoSample.Flag & VO_SOURCE_SAMPLE_FLAG_FRAMEDROPPED )
	{
		if (m_pVideoLogoInfo)
		{
			//m_pVideoLogoInfo->m_nStatisTotalSourceDropSample ++;
			m_pVideoLogoInfo->m_nSourceDropSample ++;
		}
	}

	if (m_pVideoDec == NULL)
		return VOMP_ERR_Video;

	if ((m_sVideoSample.Flag & VO_SOURCE_SAMPLE_FLAG_FRAMEDROPPED) || m_sVideoSample.Time == m_nSeekPos)
	{
		m_bFileDropFrame = VO_TRUE;
		m_bFrameDropped = VO_TRUE;
		m_nDroppedVidooFrames++;
	}
	else if (m_bFileDropFrame)
	{
		VO_VIDEO_FRAMETYPE nFrameType = m_pVideoDec->GetFrameType (&m_sVideoDecInBuf);
		if (nFrameType == VO_VIDEO_FRAME_B && !m_pVideoDec->IsRefFrame(&m_sVideoDecInBuf))
		{
			if(m_pVideoLogoInfo)
				m_pVideoLogoInfo->DecSourceStart(&m_sVideoDecInBuf, 1);
			m_nDroppedVidooFrames++;
			return VOMP_ERR_OutOfTime;
		}
		m_bFileDropFrame = VO_FALSE;
	}

	if (m_sVideoDecInBuf.Buffer == NULL)
		return VOMP_ERR_Retry;

	VO_S64 llVideoRndTime = m_sVideoDecInBuf.Time;

	if ((m_nDisableDropVideoFrame == 0) && ((VO_S64)ullPlayingTime > llVideoRndTime + 80))
	{
#if defined(_IOS) || defined(_MAC_OS)
		if (m_pVideoDec->GetFrameType (&m_sVideoDecInBuf) == VO_VIDEO_FRAME_B && m_nBFrameDropped <= 4 && !m_pVideoDec->IsRefFrame(&m_sVideoDecInBuf) && m_bAudioBufFull)
#else
		if (m_pVideoDec->GetFrameType (&m_sVideoDecInBuf) == VO_VIDEO_FRAME_B && m_nBFrameDropped <= 4 && !m_pVideoDec->IsRefFrame(&m_sVideoDecInBuf))
#endif	
		{
			VOLOGI("@@@####srcV Drop video frame: B frame, PlayingTime: %lld, llVideoRndTime = m_sVideoDecInBuf.Time: %d", ullPlayingTime, (int)llVideoRndTime);
			m_bFrameDropped = VO_TRUE;
			m_nBFrameDropped++;
			m_nDroppedVidooFrames++;
			
			if(m_pVideoLogoInfo)
			{
				//m_pVideoLogoInfo->m_nStatisTotalDecoderDropFrame ++;
				m_pVideoLogoInfo->m_nDecoderDropFrame ++;
			}

			if(m_pVideoLogoInfo)
				m_pVideoLogoInfo->DecSourceStart(&m_sVideoDecInBuf, 1);

			return VOMP_ERR_OutOfTime;
		}
	}

	nRC = m_pVideoDec->SetInputData(&m_sVideoDecInBuf);
	VOLOGI("@@@####decV SetInputData time %lld, len %d, nRC 0x%08X", m_sVideoDecInBuf.Time, m_sVideoDecInBuf.Length, nRC);

	m_bVideoDecInputRetry = VO_FALSE;
	m_bVideoDecHungry = VO_FALSE;
	if(VO_ERR_CODEC_UNSUPPORTED == nRC)
	{
		if(!(m_nAudioVideoUnsupport & 0x02))
		{
			m_nAudioVideoUnsupport |= 0x02;
			postCheckStatusEvent (VOMP_CB_CodecNotSupport, m_sVideoTrackInfo.Codec);
			VOLOGI("Video codec not support !");
		}

		voOS_Sleep(2);
		return VOMP_ERR_Video;
	}
	else if(nRC != VO_ERR_NONE)
	{
		if(VO_ERR_IOMXDEC_NeedRetry == nRC || VO_ERR_RETRY == nRC)
			m_bVideoDecInputRetry = VO_TRUE;
		else
		{
			if(VO_ERR_INPUT_BUFFER_SMALL == nRC)
				m_bVideoDecHungry = VO_TRUE;

			return VOMP_ERR_Retry;
		}
	}
	else
		m_llLastVideoDecInTime = m_sVideoDecInBuf.Time;

	nRC = DecVideoSample ();

	if (m_pFileSink != NULL)
	{
		if (m_smpSinkVideo.Buffer == NULL)
		{			
			m_smpSinkVideo.Buffer = new unsigned char[64 * 1024];
			if (m_pVideoDec != NULL)
				nRC = m_pFileSink->SetParam (VO_PID_VIDEO_FORMAT, &m_sVideoFormat);			
		}
		memcpy (m_smpSinkVideo.Buffer, m_sVideoSample.Buffer, m_sVideoSample.Size  & 0X7FFFFFFF);
		m_smpSinkVideo.Size     = m_sVideoSample.Size & 0x7FFFFFFF;
		m_smpSinkVideo.Time     = m_sVideoSample.Time;
		m_smpSinkVideo.Duration = 1;
		m_smpSinkVideo.nAV		= 1; // video
		
		nRC = m_pFileSink->AddSample (&m_smpSinkVideo);
	}
		
	if (nRC >= 0)
		return nRC;

	return VOMP_ERR_Unknown;
}

int  voCMediaPlayer::gcd(int m, int n)
{
	if (m == 0)
		return n;
	if (n == 0)
		return m;
	if (m < n)
	{
		int tmp = m;
		m = n;
		n = tmp;
	}
	while (n != 0)
	{
		int tmp = m % n;
		m = n;
		n = tmp;
	}

	return m;
}

int voCMediaPlayer::DecVideoSample (void)
{
	if (m_pVideoDec == NULL)
		return VOMP_ERR_Status;

	if (m_nDisableDeblockVideo > 0)
	{
		if (m_nDisableDropVideoFrame == 0)
			m_pVideoDec->SetDelayTime ((VO_S32)(GetPlayingTime () - m_sVideoDecInBuf.Time));

		int nDeblock = m_pVideoDec->GetDeblock();
		
		if(nDeblock != m_nDeblockVideo)
		{
			m_nDeblockVideo = nDeblock;
			postCheckStatusEvent (VOMP_CB_Deblock, m_nDeblockVideo);
		}
	}

	if(m_nDecSucess)
	{
		m_nDecStartSysTime = (VO_S64)voOS_GetSysTime ();
		m_nDecSucess = 0;
	}

	m_bufVideo.Time = -1;
	m_bufVideo.UserData = NULL;
	m_bMoreVideoBuff = VO_FALSE;
	m_bNeedInputData = VO_FALSE;

#if 1

	if(m_pVideoLogoInfo)
		m_pVideoLogoInfo->DecSourceStart(&m_sVideoDecInBuf, 0);

#ifdef _VOLOG_INFO
	VO_U64 nStartDecTime = (VO_U64)voOS_GetSysTime();
	VOLOGI("@@@####decV m_pVideoDec->GetOutputData ===> morebuff: %d", (int)m_bMoreVideoBuff);
	int nRC = m_pVideoDec->GetOutputData (&m_bufVideo , &m_fmtVideo , &m_bMoreVideoBuff);
	//VOLOGI("@@@####decV m_pVideoDec->GetOutputData <=== morebuff: %d", m_bMoreVideoBuff);
	VO_U64 nEndDecTime = (VO_U64)voOS_GetSysTime();	

	if (nRC == VO_ERR_NONE)
	{
		VOLOGI("@@@####decV m_pVideoDec->GetOutputData <=== Dec_nRC: OK  usetime: %d morebuff: %d m_bufVideo.Time: %lld codec data 0x%08X", (int)(nEndDecTime - nStartDecTime), (int)m_bMoreVideoBuff, m_bufVideo.Time, m_bufVideo.CodecData);
	} 
	else if ((nRC & 0X8000000F) == VO_ERR_INPUT_BUFFER_SMALL)
	{
		VOLOGI("@@@####decV m_pVideoDec->GetOutputData <=== Dec_nRC: VO_ERR_INPUT_BUFFER_SMALL  usetime: %d", (int)(nEndDecTime - nStartDecTime));
	}
	else if ((nRC & 0X8000000F) == VO_ERR_RETRY)
	{
		VOLOGI("@@@####decV m_pVideoDec->GetOutputData <=== Dec_nRC: VO_ERR_RETRY  usetime: %d", (int)(nEndDecTime - nStartDecTime));
	}
	else
	{
		VOLOGI("@@@####decV m_pVideoDec->GetOutputData <=== Dec_nRC: 0x%08x  usetime: %d", nRC, (int)(nEndDecTime - nStartDecTime));
	}
#else
	VO_U64 nStartDecTime = (VO_U64)voOS_GetSysTime();
	int nRC = m_pVideoDec->GetOutputData (&m_bufVideo , &m_fmtVideo , &m_bMoreVideoBuff);
	VO_U64 nEndDecTime = (VO_U64)voOS_GetSysTime();	
#endif

	if(VO_ERR_CODEC_UNSUPPORTED == nRC)
	{
		if(!(m_nAudioVideoUnsupport & 0x02))
		{
			m_nAudioVideoUnsupport |= 0x02;
			postCheckStatusEvent (VOMP_CB_CodecNotSupport, m_sVideoTrackInfo.Codec);
			VOLOGI("Video codec not support !");
		}
		voOS_Sleep(2);
		return VOMP_ERR_Video;
	}

	if(m_pVideoLogoInfo && m_pVideoLogoInfo->m_nFrameRateCount < 11 && (nRC == VO_ERR_NONE))
	{
		if (m_pVideoLogoInfo->m_nFrameRateCount == 0)
			m_pVideoLogoInfo->m_nFrameRateStartTime += (int)m_bufVideo.Time;
		if (m_pVideoLogoInfo->m_nFrameRateCount == 10)
		{
			m_pVideoLogoInfo->m_nFrameRateValue = ((int)m_bufVideo.Time - m_pVideoLogoInfo->m_nFrameRateStartTime) / 10;
		}

		m_pVideoLogoInfo->m_nFrameRateCount ++;
	}

	if(m_pVideoLogoInfo)
		m_pVideoLogoInfo->DecSourceEnd(&m_bufVideo, nRC);

#else
	int nRC = VO_ERR_INPUT_BUFFER_SMALL;
#endif

	if(m_nVideoEOSArrived == 1)
	{
		if(m_nColorType == VO_COLOR_IOMX_PRIVATE || VO_ACODEC_COLOR_TYPE == m_nColorType)
		{
			if(nRC)
			{
				m_nVideoEOSArrived = 2;
			}
		}
		else
		{
			if(m_bMoreVideoBuff == VO_FALSE)
				m_nVideoEOSArrived = 2;
		}
	}

	// for new video decoder API, we need set input data every time even decoder has more buffer
	if(m_bMoreVideoBuff && m_bNewVOVideoDec)
		m_bNeedInputData = VO_TRUE;

	if(nRC == VO_ERR_NONE)
	{
		if (m_bufVideo.Buffer[0] == NULL)
		{
			if(m_bMoreVideoBuff)
				m_bNeedInputData = VO_TRUE;

			return VOMP_ERR_SmallSize;
		}

#ifdef BA_DEBUG
		int nOutBuffFlag = 0;
		if (m_bufVideo.UserData != NULL)
		{
			nOutBuffFlag = (int) (*(int*)m_bufVideo.UserData);
		}
		else
		{
			VOLOGI("@@@####decV m_bufVideo.UserData == NULL");
		}		
		
		if ((nOutBuffFlag & VOMP_FLAG_BUFFER_FRAME_DECODE_ONLY) == VOMP_FLAG_BUFFER_FRAME_DECODE_ONLY)
		{
			VOLOGI("@@@####decV Meet buffer flag FRAME_DECODE_ONLY! Video Dec output TimeStamp: %d", (int)m_bufVideo.Time);
			return VOMP_ERR_SmallSize;
		}	
#endif
		//VOLOGI ("@@@####decV Video Dec TimeStamp is %d",(int)m_bufVideo.Time);
		//VOLOGI ("Video Dec TimeStamp is %d, System Time: %d, Playing Time %d, Using Time %d",(int)m_bufVideo.Time, voOS_GetSysTime (),GetPlayingTime (), voOS_GetSysTime () - m_nDecStartSysTime);
		// using the system time more reasonable.
		m_nVideoDecTimes = m_nVideoDecTimes + ((VO_S64)voOS_GetSysTime () - m_nDecStartSysTime);
		m_nDecSucess = 1;

		if (m_bufVideo.Time == -1)
		{
			m_bufVideo.Time = m_sVideoSample.Time;
		}
		else
		{
			if (m_nFF == VO_FILE_FFMOVIE_REAL)
				m_bufVideo.Time = m_sVideoSample.Time;
		}
		memcpy (&m_sVideoDecOutBuf, &m_bufVideo, sizeof (VO_VIDEO_BUFFER));

		m_llLastVideoDecOutTime = m_bufVideo.Time;

		int nRDec = VO_ERR_NONE;
		if(m_fmtVideo.Height != m_sVideoFormat.Height || m_fmtVideo.Width != m_sVideoFormat.Width)
		{			
			m_sVideoFormat.Height = m_fmtVideo.Height;
			m_sVideoFormat.Width  = m_fmtVideo.Width;
			m_sVideoFormat.Type   = m_fmtVideo.Type;
			
			nRDec = VOMP_ERR_FormatChange;
		}

		VO_U32	mAspectRatio = 0;
		VO_U32 ret = m_pVideoDec->GetParam(VO_PID_VIDEO_ASPECTRATIO, &mAspectRatio);

		if(m_sVideoTrackInfo.Codec == VOMP_VIDEO_CodingH264)
		{
			TH264VideoUsebilityInfo info;
			memset( &info , 0 , sizeof( TH264VideoUsebilityInfo ) );
			int nRC1 = m_pVideoDec->GetParam(VO_ID_H264_VUI,&info);
			if(nRC1 == 0 && m_nColorType != VO_COLOR_IOMX_PRIVATE && VO_ACODEC_COLOR_TYPE != m_nColorType)
			{
				//VOLOGI ("info.aspect_ratio_idc %d, info.sar_width %d, info.sar_height %d, Video Width %d, Video Height %d, mAspectRatio %d", info.aspect_ratio_idc, info.sar_width, info.sar_height, m_sVideoFormat.Width, m_sVideoFormat.Height, mAspectRatio);
				const static int aspectInfo[16][2]={
					{1,1}, {12,11}, {10,11}, {16,11}, {40,33},{24,11},{20,11},{32,11},{80,33},{18,11},{15,11},{64,33},{160,99}, {4, 3}, {3, 2}, {2, 1}
				};

				if( info.aspect_ratio_idc == 0 || info.aspect_ratio_idc == 1)
					mAspectRatio = 0;

				int width = 0;
				int height = 0;
				if(info.aspect_ratio_idc > 1 && info.aspect_ratio_idc < 17)
				{
					width = m_sVideoFormat.Width*aspectInfo[info.aspect_ratio_idc - 1][0];
					height = m_sVideoFormat.Height*aspectInfo[info.aspect_ratio_idc - 1][1];
				}
				else if( info.aspect_ratio_idc == 255 )
				{
					width = m_sVideoFormat.Width * info.sar_width;
					height = m_sVideoFormat.Height * info.sar_height;
				}

				int ratio = 0;

				if(height > 0)
					ratio = width*10/height;

				mAspectRatio = 0;
				if( ratio == 0 )
					mAspectRatio = 0;
				else if( ratio <= 10 )
					mAspectRatio = 1;
				else if( ratio <= 42/3 )
					mAspectRatio = 2;
				else if( ratio <= 168 / 9 )
					mAspectRatio = 3;
				else if( ratio <= 21)
					mAspectRatio = 4;
				else 
				{
					int m = gcd(width, height);
					if(m){
						width = width/m;
						height = height/m;
					}

					if(width > 0x7fff || height > 0x7fff)
					{
						width = (width + 512) >> 10;
						height = (height + 512) >> 10;
					}

					mAspectRatio = (width << 16) | height;
				}
			}
		}

		if(ret == 0)
		{
			if((VOMP_ASPECT_RATIO)mAspectRatio != m_nAspectRatio)
			{
				m_nAspectRatio = (VOMP_ASPECT_RATIO)mAspectRatio;

				// added by gtxia for fixing the issue that aspect ratio switch twice(the issue id is 29346)
				if( VO_ACODEC_COLOR_TYPE == m_nColorType)
					m_pListener(m_pUserData, VOMP_CB_VideoAspectRatio, &mAspectRatio, NULL);
				else
					postCheckStatusEvent (VOMP_CB_VideoAspectRatio, mAspectRatio);

				VOLOGI ("m_nAspectRatio %d", m_nAspectRatio);
			}			
		}

		if (m_nVideoRenderCount > 1)
		{
			VO_U32 i = 0;
			
			if (m_ppVideoOutBuffers == NULL)
			{
				m_ppVideoOutBuffers = new VO_VIDEO_BUFFER * [m_nVideoRenderCount];
				if(!m_ppVideoOutBuffers)
					return VOMP_ERR_OutMemory;
				m_pVideoOutWidth = new VO_S32[m_nVideoRenderCount];
				if(!m_pVideoOutWidth)
					return VOMP_ERR_OutMemory;
				m_pVideoOutHeight = new VO_S32[m_nVideoRenderCount];
				if(!m_pVideoOutHeight)
					return VOMP_ERR_OutMemory;

				for (i = 0; i < m_nVideoRenderCount; i++)
				{
					m_ppVideoOutBuffers[i] = new VO_VIDEO_BUFFER ();
					if(!m_ppVideoOutBuffers[i])
						return VOMP_ERR_OutMemory;
					memset (m_ppVideoOutBuffers[i], 0, sizeof (VO_VIDEO_BUFFER));
					m_ppVideoOutBuffers[i]->ColorType = VO_COLOR_YUV_PLANAR420;

					m_pVideoOutWidth[i] = 0;
					m_pVideoOutHeight[i] = 0;
				}
			}

			int nIndex = m_nVideoOutFillIndex % m_nVideoRenderCount;

			//int nStartTime = voOS_GetSysTime ();

			if (m_nVideoRndCopy > 0)
			{				
				if (m_ppVideoOutBuffers[nIndex]->Stride[0] != m_sVideoFormat.Width || m_fmtVideo.Height != m_pVideoOutHeight[nIndex])
				{
					if (m_ppVideoOutBuffers[nIndex]->Buffer[0] != NULL)
					{
						delete []m_ppVideoOutBuffers[nIndex]->Buffer[0];
						m_ppVideoOutBuffers[nIndex]->Buffer[0] = NULL;
					}
					m_ppVideoOutBuffers[nIndex]->Buffer[0] = new VO_BYTE[m_sVideoFormat.Width * m_sVideoFormat.Height * 3 / 2];
					m_ppVideoOutBuffers[nIndex]->Buffer[1] = m_ppVideoOutBuffers[nIndex]->Buffer[0] + m_sVideoFormat.Width * m_sVideoFormat.Height;
					m_ppVideoOutBuffers[nIndex]->Buffer[2] = m_ppVideoOutBuffers[nIndex]->Buffer[0] + m_sVideoFormat.Width * m_sVideoFormat.Height * 5 / 4;

					m_ppVideoOutBuffers[nIndex]->Stride[0] = m_sVideoFormat.Width;
					m_ppVideoOutBuffers[nIndex]->Stride[1] = m_sVideoFormat.Width / 2;
					m_ppVideoOutBuffers[nIndex]->Stride[2] = m_sVideoFormat.Width / 2;
				}

				int i = 0;
				for (i = 0; i < m_sVideoFormat.Height; i++)
					memcpy (m_ppVideoOutBuffers[nIndex]->Buffer[0] + m_sVideoFormat.Width * i, m_sVideoDecOutBuf.Buffer[0] + m_sVideoDecOutBuf.Stride[0] * i,  m_sVideoFormat.Width);
				for (i = 0; i < m_sVideoFormat.Height / 2; i++)
					memcpy (m_ppVideoOutBuffers[nIndex]->Buffer[1] + (m_sVideoFormat.Width / 2) * i, m_sVideoDecOutBuf.Buffer[1] + m_sVideoDecOutBuf.Stride[1] * i,  m_sVideoFormat.Width / 2);
				for (i = 0; i < m_sVideoFormat.Height / 2; i++)
					memcpy (m_ppVideoOutBuffers[nIndex]->Buffer[2] + (m_sVideoFormat.Width / 2) * i, m_sVideoDecOutBuf.Buffer[2] + m_sVideoDecOutBuf.Stride[2] * i,  m_sVideoFormat.Width / 2);
			}
			else
			{
				memcpy (m_ppVideoOutBuffers[nIndex], &m_sVideoDecOutBuf, sizeof (VO_VIDEO_BUFFER));
			}

			//VOLOGI("Copy YUV useTime %d", voOS_GetSysTime () - nStartTime);
			m_ppVideoOutBuffers[nIndex]->UserData = m_sVideoDecOutBuf.UserData;
			m_ppVideoOutBuffers[nIndex]->Time = m_sVideoDecOutBuf.Time;
			m_pVideoOutWidth[nIndex] = m_fmtVideo.Width;
			m_pVideoOutHeight[nIndex] = m_fmtVideo.Height;

			m_nVideoOutFillIndex++;
		}
		else
		{
			m_nVideoRndWidth = m_fmtVideo.Width;
			m_nVideoRndHeight = m_fmtVideo.Height;
		}

		m_nBFrameDropped = 0;
		m_nVideoDecoderNum++;


		if(m_pVideoLogoInfo)
		{
			//m_pVideoLogoInfo->m_nStatisTotalDecoderFrame ++;
			m_pVideoLogoInfo->m_nDecoderFrameNumber ++;

			int nDecUseTime =  (int)(nEndDecTime - nStartDecTime);
			m_pVideoLogoInfo->UpdateDecTimeNumber( nDecUseTime );
			m_pVideoLogoInfo->m_nSumDecoderTime = m_pVideoLogoInfo->m_nSumDecoderTime + nDecUseTime;
			if (m_pVideoLogoInfo->m_nMaxDecoderTime < nDecUseTime) m_pVideoLogoInfo->m_nMaxDecoderTime = nDecUseTime;

			m_pVideoLogoInfo->UpdataPerformanceData();
		}

		m_aVideoDcdTime[m_nVideoDcdTimeIndex] = m_sVideoSample.Time;
		if (m_nVideoDcdTimeIndex > 1000)
		{
			int nFirstFrame = m_nVideoDcdTimeIndex;
			while (m_aVideoDcdTime[m_nVideoDcdTimeIndex] - m_aVideoDcdTime[nFirstFrame] < 5000)
			{
				nFirstFrame--;
				if (nFirstFrame <= 0)
					break;
			}

			memcpy (&m_aVideoDcdTime[0], &m_aVideoDcdTime[nFirstFrame], (m_nVideoDcdTimeIndex - nFirstFrame) * sizeof (VO_S64));
			m_nVideoDcdTimeIndex = m_nVideoDcdTimeIndex - nFirstFrame;
		}
		else
		{
			m_nVideoDcdTimeIndex++;
		}

#if 0
		static FILE* f = fopen("/sdcard/osmp/dump.yuv", "wb");
		if(f)
		{
			int i, j;
			VO_PBYTE pBuffer;

			VOLOGI("The video Height is %d, width is %d, m_sVideoDecOutBuf.Stride[0] %d", m_sVideoFormat.Height, m_sVideoFormat.Width, m_sVideoDecOutBuf.Stride[0]);

			pBuffer = m_sVideoDecOutBuf.Buffer[0] ;
			for(i = 0; i < m_sVideoFormat.Height; i++)
			{
				fwrite(pBuffer, m_sVideoFormat.Width, 1, f);
				pBuffer += m_sVideoDecOutBuf.Stride[0];
			}

			pBuffer = m_sVideoDecOutBuf.Buffer[1] ;
			for(i = 0; i < m_sVideoFormat.Height/2; i++)
			{
				fwrite(pBuffer, m_sVideoFormat.Width/2, 1, f);
				pBuffer += m_sVideoDecOutBuf.Stride[1];
			}

			pBuffer = m_sVideoDecOutBuf.Buffer[2] ;
			for(i = 0; i < m_sVideoFormat.Height/2; i++)
			{
				fwrite(pBuffer, m_sVideoFormat.Width/2, 1, f);
				pBuffer += m_sVideoDecOutBuf.Stride[2];
			}

		}
		else
		{
			VOLOGI("Open file failed");
		}
#endif
		m_nVideoDecError = 0;

		return nRDec;
	}
	else if(VO_ERR_IOMXDEC_NeedRetry == nRC)
		return nRC;
	else if ((nRC & 0X8000000F) == VO_ERR_INPUT_BUFFER_SMALL)
	{
		if(m_bSeekingPos)
		{
			m_nVideoDecError++;
			if (m_nVideoDecError > 500)
			{
				m_nAudioRenderNum = 2;
				m_nAudioRndNuming = 3;
				m_bSeekingPos = VO_FALSE;
				postCheckStatusEvent (VOMP_CB_SeekComplete);
			}
		}

		return VOMP_ERR_SmallSize;
	}
	else if ((nRC & 0X8000000F) == VO_ERR_RETRY)
	{
		m_bMoreVideoBuff = VO_TRUE;
		return VOMP_ERR_Retry;
	}

	m_nVideoDecError++;
	if (m_nVideoDecError > 500)
	{
		if(m_bSeekingPos)
		{
			m_nAudioRenderNum = 2;
			m_nAudioRndNuming = 3;
			m_bSeekingPos = VO_FALSE;
			postCheckStatusEvent (VOMP_CB_SeekComplete);
		}

		voOS_Sleep(2);
	}

	VOLOGW ("Video Dec error: %08X", nRC);
	return VOMP_ERR_Unknown;
}

int voCMediaPlayer::flushPicturesOfVOVideoDec()
{
	if(!m_pVideoDec)
		return VOMP_ERR_Pointer;

	if(VO_VIDEO_CodingH264 == m_sVideoTrackInfo.Codec)
	{
		VO_U32 nFlushPictures = 1;
		m_pVideoDec->SetParam(VO_ID_H264_FLUSH_PICS, &nFlushPictures);
	}
	else if(VO_VIDEO_CodingH265 == m_sVideoTrackInfo.Codec)
	{
		VO_U32 nFlushPictures = 1;
		m_pVideoDec->SetParam(VO_PID_DEC_H265_FLUSH_PICS, &nFlushPictures);
	}
	else	// not implemented for other decoders
		return VOMP_ERR_Implement;

	return VOMP_ERR_None;
}

int voCMediaPlayer::ResetVideoSample (VO_VIDEO_BUFFER * pOutBuffer)
{
#if defined(_IOS) || defined(_MAC_OS)
	if(m_pLogoEffect)
		m_pLogoEffect->ResetVideo(pOutBuffer);
#endif
	return VOMP_ERR_None;
}

int voCMediaPlayer::EffectVideoSample (VO_VIDEO_BUFFER * pVideoBuffer)
{
#if defined(_IOS) || defined(_MAC_OS)
	if(m_pLogoEffect)
	{
		m_pLogoEffect->CheckVideo(pVideoBuffer, &m_fmtVideo);

		if(m_LicenseCheck.IsVideoFrameBlack())
		{
			memset(pVideoBuffer->Buffer[0], 0, pVideoBuffer->Stride[0]*m_fmtVideo.Height);
			memset(pVideoBuffer->Buffer[1], 128, pVideoBuffer->Stride[1]*m_fmtVideo.Height);
			memset(pVideoBuffer->Buffer[2], 128, pVideoBuffer->Stride[2]*m_fmtVideo.Height);
		}
	}
#endif

	int nRC = VOMP_ERR_None;

	if(m_pVideoEffect == NULL)
		return VOMP_ERR_None;

	if(!m_bVideoEffectOn)
	{
		return VOMP_ERR_None;
	}

	VO_VIDEO_OUTPUTINFO mVideoInfo;
	VO_CODECBUFFER UserBuf;
	memset( &UserBuf , 0 , sizeof(VO_CODECBUFFER));
	
	if(m_pVideoEffectType == VOMP_VE_CloseCaptionOn)
	{
		m_pVideoDec->GetParam(VO_ID_H264_T35_USERDATA, &UserBuf);
	}

	m_pVideoEffect->SetInputData (&m_bufVideo);
	nRC = m_pVideoEffect->GetOutputData (pVideoBuffer, &mVideoInfo, &UserBuf);

	return VOMP_ERR_None;
}

int voCMediaPlayer::RenderVideoSample (void)
{
#if defined _IOS
	if (m_pListener (m_pUserData, VOMP_CB_RenderVideo, m_pVideoOutputBuff, &m_sVideoFormat) == VOMP_ERR_None)
		return 0;
	
	if (m_pVideoRender == NULL)
	{
		m_pVideoRender = new CiOSVideoRender (NULL, m_hView, NULL);
		if(!m_pVideoRender)
			return VOMP_ERR_OutMemory;
        m_pVideoRender->Start();
		m_pVideoRender->SetVideoInfo (m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
		m_pVideoRender->SetParam (VO_PID_COMMON_LIBOP, (VO_LIB_OPERATOR *)m_pLibFunc);
		m_pVideoRender->SetDispRect (m_hView, (VO_RECT *)&m_rcDraw, (VO_IV_COLORTYPE)m_nColorType);
	}
	
	if ((NULL != m_pVideoRender) && (NULL != m_pVideoOutputBuff))
	{
		m_pVideoRender->Render (m_pVideoOutputBuff, m_pVideoOutputBuff->Time, VO_TRUE);
		//VOLOGI("RENDER video ts = %d", m_sVideoDecOutBuf.Time);
	}
	
	return 0;
#endif // _IOS
	
	if (m_pListener != NULL && m_bOutVideoRender)
	{
		if (m_nColorType == VOMP_COLOR_YUV_PLANAR420)
		{
			if (m_pListener (m_pUserData, VOMP_CB_RenderVideo, m_pVideoOutputBuff, &m_sVideoFormat) == VOMP_ERR_None)
				return 0;
		}
		else if (m_nColorType == VOMP_COLOR_RGB565_PACKED || m_nColorType == VOMP_COLOR_RGB888_PACKED ||
				 m_nColorType == VOMP_COLOR_RGB32_PACKED || m_nColorType == VOMP_COLOR_ARGB32_PACKED)
		{
			if (m_pVideoRender == NULL)
			{
#ifdef _METRO
#elif defined _WIN32 
				m_pVideoRender = new CGDIVideoRender (m_hInst, m_hView, NULL);
#else
				m_pVideoRender = new CBaseVideoRender (m_hInst, m_hView, NULL);
#endif // _WIN32
				if (m_pVideoRender == NULL)
					return VOMP_ERR_OutMemory;
				m_pVideoRender->SetWorkPath (m_szWorkPath);
				m_pVideoRender->SetConfigFile (&m_cfgVOSDK);
				m_pVideoRender->SetCPUVersion(m_nCPUVersion);

				m_pVideoRender->SetParam (VO_PID_COMMON_LIBOP, (VO_LIB_OPERATOR *)m_pLibFunc);
			}

			if (m_nVideoRenderNum == 0)
			{				
				if(!m_bSetDispRect)
				{
					m_rcDraw.left = 0; m_rcDraw.right = m_sVideoFormat.Width;
					m_rcDraw.top = 0; m_rcDraw.bottom = m_sVideoFormat.Height;
				}

				m_pVideoRender->SetVideoInfo (m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
				m_pVideoRender->SetDispRect (m_hView, (VO_RECT *)&m_rcDraw, (VO_IV_COLORTYPE)m_nColorType);
				m_pVideoRender->SetDispType(m_nZoomType, VO_RATIO_00);
			}

#ifndef _WIN32
			m_rgbBuffer.Buffer[0] = NULL;
			m_rgbBuffer.ColorType = VO_COLOR_RGB565_PACKED;
			m_pVideoRender->RenderRGBBuffer (m_pVideoOutputBuff, &m_rgbBuffer);
#endif // _WIN32

			VO_RECT rcDraw;
			m_pVideoRender->GetParam (VO_VR_PMID_DrawRect, &rcDraw);
			m_sVideoFormat.Width = rcDraw.right - rcDraw.left;
			m_sVideoFormat.Height = rcDraw.bottom - rcDraw.top;
			m_sVideoFormat.Type = (VO_VIDEO_FRAMETYPE)VOMP_COLOR_RGB565_PACKED;

			if (m_pListener (m_pUserData, VOMP_CB_RenderVideo, &m_rgbBuffer, &m_sVideoFormat) == VOMP_ERR_None)
				return 0;
			else
				m_bOutVideoRender = VO_FALSE;
		}
	}

	if (m_pVideoRender == NULL)
	{
#ifdef _WIN32
#ifdef _METRO
#else  //_METRO
		m_pVideoRender = new CGDIVideoRender (m_hInst, m_hView, NULL);
#endif //_METRO
		if(!m_pVideoRender)
			return VOMP_ERR_OutMemory;
		m_pVideoRender->SetWorkPath (m_szWorkPath);
		m_pVideoRender->SetConfigFile (&m_cfgVOSDK);
		m_pVideoRender->SetParam (VO_PID_COMMON_LIBOP, (VO_LIB_OPERATOR *)m_pLibFunc);
		m_pVideoRender->SetVideoInfo (m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
		m_pVideoRender->SetDispType(m_nZoomType, VO_RATIO_00);
		m_pVideoRender->SetDispRect (m_hView, (VO_RECT *)&m_rcDraw, VO_COLOR_RGB565_PACKED);
		
#endif // _WIN32
	}

	if (m_pVideoRender != NULL)
		m_pVideoRender->Render (m_pVideoOutputBuff, m_pVideoOutputBuff->Time, VO_TRUE);

	return 0;
}

int voCMediaPlayer::CheckVideoRenderTime (void)
{
	if (m_pVideoOutputBuff == NULL)
		return VOMP_ERR_Pointer;

	// wait the seek frame
#if !defined __VOPRJ_MOTOPASTEUR__
	if (m_nSeekPos > 0 && m_pVideoOutputBuff->Time < m_nSeekPos)
	{
		cancelBufferFromVideoDec(m_pVideoOutputBuff);

		return VOMP_ERR_Retry;
	}
#endif

	//Rogine add to fix issue #32138, it should not be in buffering mode if there is video buffer to be render, 
	//this will influence the playing time
	if (m_aVideoRndTime[m_nVideoRndTimeIndex - 1] != m_llLastVideoDecOutTime)
	{
		m_bDataBuffering = VO_FALSE;
	}
	
	if (m_nVideoRenderNum > 0)
	{
		VO_S64 nPlayingTime = (VO_S64)GetPlayingTime ();
		if (nPlayingTime <= 0 && !m_nNewTimeBegin)
		{
			voOS_Sleep(2);
			return VOMP_ERR_WaitTime;
		}

		bool bAudioRendered = true;
		if(m_nAudioTrack >= 0 && !m_bOnlyGetVideo && !m_bAudioEOS && 0 == m_nAudioRenderNum)
			bAudioRendered = false;

		if (m_pVideoOutputBuff->Time > 0 && bAudioRendered && ((abs((int)(nPlayingTime - m_pVideoOutputBuff->Time)) > 30000) || m_nNewTimeBegin))
		{
			VOLOGI("AV sync out of control V %d P %d ARed %d m_nNewTimeBegin %d", (int)m_pVideoOutputBuff->Time, (int)nPlayingTime, bAudioRendered, (int)m_nNewTimeBegin);
			voOS_Sleep (25);
		}
		else
		{
			// remove the video render after seek if the audio is faster than video
			if (nPlayingTime > m_pVideoOutputBuff->Time + 300 && m_nVideoRenderNum <= 1)
			{
				cancelBufferFromVideoDec(m_pVideoOutputBuff);

				return VOMP_ERR_Retry;
			}

			if(m_nLowLatence && m_bOnlyGetVideo && m_nVideoRenderNum > 1 )
			{
				if(nPlayingTime > m_pVideoOutputBuff->Time + 100 && m_nVideoOutPlayIndex  < m_nVideoOutFillIndex)
				{
					if(m_nOnlyVideoDropTime > 600)
					{
						m_nStartSysTime  = 0;
					}

					m_nOnlyVideoDropCount++;
					if(m_nOnlyVideoDropCount < 15)
					{
						cancelBufferFromVideoDec(m_pVideoOutputBuff);

						return VOMP_ERR_Retry;
					}
					else if(m_nOnlyVideoDropCount == 15)
					{
						m_bFrameDropped = VO_TRUE;
						m_nOnlyVideoDropTime++;
					}
					else
					{
						m_nOnlyVideoDropCount = 0;
						voOS_Sleep (25);

						cancelBufferFromVideoDec(m_pVideoOutputBuff);

						return VOMP_ERR_Retry;
					}
				}
				else
				{
					m_nOnlyVideoDropCount = 0;
				}
			}
			
			if (nPlayingTime < m_pVideoOutputBuff->Time && m_nVideoRenderNum > 1)
			{
				voOS_Sleep (2);
				return VOMP_ERR_WaitTime;
			}
		}
	}

	return VOMP_ERR_None;
}

VO_U64 voCMediaPlayer::GetPlayingTime (VO_BOOL bForUser /* = VO_FALSE */)
{
	voCAutoLock lock (&m_mtPlayTime);
	if (m_nAudioTrack >= 0 && !m_bOnlyGetVideo)
	{
		if (m_nStatus == VOMP_STATUS_STOPPED || m_nStatus <= VOMP_STATUS_LOADING)
			return 0;

		if (!m_bAudioEOS)
		{
			if (m_nAudioPlayMode == VOMP_PUSH_MODE && (m_nStatus == VOMP_STATUS_PAUSED || m_nAudioRenderNum == 0))
				return (VO_U64)(m_llAudioRenderTime - m_nAudioBuffTime);

			if ((m_llAudioRenderTime <= (VO_S64)m_nAudioBuffTime) && (VO_FALSE == bForUser))
				return 0;

			if(m_nAudioRenderNum <= 1 || m_bDataBuffering)
			{
				//VOLOGI("@@@####pt pos-1.0");
				m_nPlayingTime = (VO_S32)m_llAudioRenderTime;
				return (VO_U64)m_nPlayingTime;
			}
		}
		else
		{
			//Rogine modified to fix issue #31225
			if(m_bVideoEOS || m_bDataBuffering)
			{
				//VOLOGI("@@@####pt pos-2.0");
				return (VO_U64)m_nPlayingTime;
			}
		}

		if (m_nStartSysTime == 0)
			m_nStartSysTime = (VO_U64)voOS_GetSysTime ();

		if (m_llRenderSystemTime == 0)
		{
			m_llRenderSystemTime = (VO_S64)voOS_GetSysTime ();
			m_llRenderBufferTime = m_llAudioRenderTime;

			VOLOGI("Need reset: m_llRenderSystemTime %lld, m_llRenderBufferTime %lld", m_llRenderSystemTime, m_llRenderBufferTime);
		}

		if (m_nStatus == VOMP_STATUS_RUNNING)
		{
			if ((m_nShowLogLevel & 8) == 8)
				VOLOGI ("Audio Time %lld, %lld, %lld", m_llAudioRenderTime, m_sAudioSample.Time, m_sAudioSample.Time - m_llAudioRenderTime);

			if((m_bSendStartVideoBuff || m_bSendStartAudioBuff || m_bPauseRefClock) && !m_bAudioEOS && !bForUser)
			{
				if(m_bPauseRefClock)
				{
					//VOLOGI("@@@####pt pos-3.0");
					m_nPlayingTime = (m_llAudioRenderTime < (VO_S64)m_nAudioBuffTime) ? 0 : (m_llAudioRenderTime - (VO_S64)m_nAudioBuffTime);

					m_llRenderBufferTime = 0;
					m_llRenderSystemTime = 0;
				}
				else
				{
					//VOLOGI("@@@####pt pos-3.1");
//					m_nPlayingTime = m_llAudioRenderTime;
					VO_AUDIO_FORMAT * pAudioOutFormat = m_pAudioEffect ? &m_sAudioEftOutFormat : &m_sAudioDecOutFormat;
					VO_U32 nDuration = (m_nAudioRndLength * 8000) / (pAudioOutFormat->Channels * pAudioOutFormat->SampleBits * pAudioOutFormat->SampleRate);
					if(m_llAudioRenderTime + nDuration > m_nAudioBuffTime)
						m_nPlayingTime = m_llAudioRenderTime + nDuration - m_nAudioBuffTime;
					else
						m_nPlayingTime = 0;

					VOLOGI("m_nPlayingTime %lld, m_llAudioRenderTime %lld, nDuration %d, m_nAudioBuffTime %lld", 
						m_nPlayingTime, m_llAudioRenderTime, nDuration, m_nAudioBuffTime);
				}
				//VOLOGI("@@@####pt pos-3");

				return (VO_U64)m_nPlayingTime;
			}

                if (mADSPClock != NULL) 
				{
                    m_nPlayingTime = (m_llDSPEOSSystemStartTime == 0) ? 
                                        (VO_S64)(mADSPClock->getDSPTimeStamp() + m_llDSPRefTime + m_llDSPTimeOffset) :
                                        (VO_S64)(m_llDSPRefTime + ((VO_S64)voOS_GetSysTime() - m_llDSPEOSSystemStartTime));
		            //VOLOGI ("DSP PlayTime %d DSP Offset %ld", (int)(m_nPlayingTime), (long)(m_llDSPTimeOffset));
                }
                else if (abs ((VO_S32)((m_llAudioRenderTime - m_llRenderBufferTime + ((VO_U64)voOS_GetSysTime () - m_nStartSysTime)) - (voOS_GetSysTime () - m_llRenderSystemTime))) > (VO_S32)m_llRenderAdjustTime)
			{
				VOLOGI("Need re-adjust: m_llAudioRenderTime %lld, m_llRenderBufferTime %lld, sys time %d, m_nStartSysTime %lld, m_llRenderSystemTime %lld, m_llRenderAdjustTime %lld, m_nAudioBuffTime %lld", 
					m_llAudioRenderTime, m_llRenderBufferTime, voOS_GetSysTime(), m_nStartSysTime, m_llRenderSystemTime, m_llRenderAdjustTime, m_nAudioBuffTime);

				m_llRenderBufferTime = 0;
				m_llRenderSystemTime = 0;

				m_nPlayingTime = (int)(m_llAudioRenderTime - m_nAudioBuffTime + ((VO_U64)voOS_GetSysTime () - m_nStartSysTime));			
			}
			else
			{
#if 0
                VO_S64 DSPPlayTime = (m_llDSPEOSSystemStartTime == 0) ?
                                      (VO_S64)(mADSPClock->getDSPTimeStamp() + m_llDSPRefTime + m_llDSPTimeOffset) :
                                      (VO_S64)(m_llDSPRefTime + ((VO_S64)voOS_GetSysTime() - m_llDSPEOSSystemStartTime));
				m_nPlayingTime = (int)((voOS_GetSysTime () - m_llRenderSystemTime) + m_llRenderBufferTime - m_nAudioBuffTime);
		        VOLOGI ("DSP PlayTime %d DSP Offset %ld VO PlayTime %ld, RenderBufferTime %ld", (int)(DSPPlayTime), (long)(m_llDSPTimeOffset), (long)(m_nPlayingTime), (long)(m_llRenderBufferTime));
#else
				//VOLOGI("@@@####pt pos-6");
				m_nPlayingTime = (int)((voOS_GetSysTime () - m_llRenderSystemTime) + m_llRenderBufferTime - m_nAudioBuffTime);
#endif
			}

			// sometimes m_nAudioBuffTime is bigger than m_llAudioRenderTime, we need protect it, East 20130803
			if(m_nPlayingTime < 0)
				m_nPlayingTime = 0;

			return (VO_U64)m_nPlayingTime;

		}
		else if (m_nStatus == VOMP_STATUS_PAUSED)
		{
			// we need also concern used time of renderer in future
			return (m_llAudioRenderTime < (VO_S64)m_nAudioBuffTime) ? 0 : (m_llAudioRenderTime - (VO_S64)m_nAudioBuffTime);
		}
		else
			return 0;
	}

	if (m_nStatus == VOMP_STATUS_RUNNING && m_nVideoOnOff == 0)
	{
		return (VO_U64)voOS_GetSysTime () - m_nStartSysTime;
	}

	if (m_nVideoRenderNum == 0)
	{
		if(m_bSeekingPos)
			return (VO_U64)m_nSeekPos;
		return 0;
	}

	if (m_nStartSysTime == 0)
	{
		VO_S64 nVideoNowTime = 0;	
		if (m_pVideoOutputBuff != NULL)
		{
			m_nStartSysTime = (VO_S64)(voOS_GetSysTime () - m_pVideoOutputBuff->Time);
			nVideoNowTime = m_pVideoOutputBuff->Time;
		}
		else
		{
			m_nStartSysTime = (VO_S64)(voOS_GetSysTime () - m_sVideoSample.Time);
			nVideoNowTime = m_sVideoSample.Time;
		}

		if (abs(m_llAudioRenderTime - nVideoNowTime) > 3000 && m_llAudioRenderTime > 0 && m_nAudioPlaySpeed > 10)
			m_nStartSysTime = (VO_S64)(voOS_GetSysTime () - m_llAudioRenderTime);
	}

	if(m_bVideoEOS)
	{
		return (VO_U64)m_nPlayingTime;
	}

	if (m_nStatus == VOMP_STATUS_RUNNING)
	{
		if(m_bSendStartVideoBuff && !m_nLowLatence)
		{
			if (m_pVideoOutputBuff != NULL)
			{
				m_nStartSysTime = (VO_S64)(voOS_GetSysTime () - m_pVideoOutputBuff->Time);
				m_nPlayingTime = m_pVideoOutputBuff->Time;
			}
			else
			{
				m_nStartSysTime = (VO_S64)(voOS_GetSysTime () - m_sVideoSample.Time);
				m_nPlayingTime = m_sVideoSample.Time;
			}

			return (VO_U64)m_nPlayingTime;
		}
		//Rogine add to fix issue #31225
		m_nPlayingTime = (VO_U64)voOS_GetSysTime () - m_nStartSysTime;
		return (VO_U64)m_nPlayingTime;
	}
	else if (m_nStatus == VOMP_STATUS_PAUSED)
	{
		if (m_pVideoOutputBuff == NULL)
			return 0;

		return (VO_U64)(m_pVideoOutputBuff->Time - m_nAudioBuffTime);
	}
	else
		return 0;
}

void voCMediaPlayer::ShowResult (void)
{
	if (m_llAudioRenderTime > 0 || m_nVideoRenderNum > 0)
	{
		VO_S64 nPlayTime = ((VO_S64)voOS_GetSysTime () - m_nStartPlayTime) / 1000;
		if (nPlayTime <= 0)
			nPlayTime = 1;

		VO_TCHAR     strPath[1024];
		VO_TCHAR     strFileName[1024];
		memset(strPath , 0 , sizeof(VO_TCHAR) * 1024);
		memset(strFileName , 0 , sizeof(VO_TCHAR) * 1024);

		voOS_GetAppFolder(strPath , 1024);

		vostrcpy(strFileName , strPath);
		vostrcat(strFileName , _T("tp.txt"));

#ifdef _WIN32
		vostrcpy(strPath , m_szWorkPath);
		VO_TCHAR strTempName[1024] = {0};
		vostrcat(strTempName , strPath);
		
		//WideCharToMultiByte (CP_ACP, 0, strFileName, -1, strTempName, 1024 , NULL, NULL);
		vostrcat(strTempName ,  _T("tp.txt"));
#endif //
		
		FILE *file = NULL;
#if 0
#ifdef _WIN32
		FILE *tmp = fopen((char *)strTempName , "wb+");
#else
		FILE *tmp = fopen((char *)strFileName , "r");
#endif

		if(tmp != NULL)
		{
			fclose(tmp);

			vostrcpy(strFileName , strPath);
			vostrcat(strFileName , _T("perf.txt"));

#ifdef _WIN32
			char strTempName[1024];
			memset(strTempName , 0 , 1024);

			WideCharToMultiByte (CP_ACP, 0, strFileName, -1, strTempName, 1024 , NULL, NULL);
			memcpy(strFileName , strTempName , 1024);
#endif //

			file = fopen((char *)strFileName , "a");
			if(file != NULL)
			{
				fprintf(file, "\r\nPlayed URL: %s", m_szURL);
				fprintf(file , "\rPlaying Time %d Video Frames %d,	%d	%d	%d \r\n", (int)nPlayTime, (int)m_nVideoDecoderNum, (int)m_nVideoRenderNum, (int)(m_nVideoDecoderNum * 100 / nPlayTime), (int)(m_nVideoRenderNum * 100 / nPlayTime));
			}
		}
#endif

		VOLOGI ("Playing Time %d Video Frames %d,	%d	%d	%d", (int)nPlayTime, (int)m_nVideoDecoderNum, (int)m_nVideoRenderNum, (int)(m_nVideoDecoderNum * 100 / nPlayTime), (int)(m_nVideoRenderNum * 100 / nPlayTime));
		if (m_nVideoDecTimes == 0)
			m_nVideoDecTimes = 1;
		if (m_nVideoCCRTimes == 0)
			m_nVideoCCRTimes = 1;
		if (m_nVideoRndTimes == 0)
			m_nVideoRndTimes = 1;
		VOLOGI ("Video: Dec %d, %d, CCR %d, %d, Rnd %d, %d, Audio: Dec %d (ms)", (int)m_nVideoDecTimes, (int)(m_nVideoDecoderNum * 100000 / m_nVideoDecTimes), (int)m_nVideoCCRTimes, (int)(m_nVideoRenderNum * 100000 / m_nVideoCCRTimes),
																				(int)m_nVideoRndTimes, (int)(m_nVideoRenderNum * 100000 / m_nVideoRndTimes), (int)m_nAudioDecTimes);
		if(file != NULL)
		{
			fprintf(file , "Video: Dec %d, %d, CCR %d, %d, Rnd %d, %d, Audio: Dec %d (ms) \r\n ", (int)m_nVideoDecTimes, (int)(m_nVideoDecoderNum * 100000 / m_nVideoDecTimes), (int)m_nVideoCCRTimes, (int)(m_nVideoRenderNum * 100000 / m_nVideoCCRTimes),
				(int)m_nVideoRndTimes, (int)(m_nVideoRenderNum * 100000 / m_nVideoRndTimes), (int)m_nAudioDecTimes);

			fclose(file);
		}

		
	}
}

int voCMediaPlayer::CloseSource (bool bSync)
{
	if (!bSync)
	{
		if (m_pMsgThread != NULL)
		{
			voCBaseEventItem * pEvent = m_pMsgThread->GetEventByType (VOMP_EVENT_CLOSE);
			if (pEvent == NULL)
				pEvent = new voCMediaPlayerEvent (this, &voCMediaPlayer::onCloseSource, VOMP_EVENT_CLOSE);
			m_pMsgThread->PostEventWithRealTime (pEvent, -1);
		}
	}
	else
	{
		Reset ();

		postCheckStatusEvent (VOMP_CB_CloseSource);
	}

	return VOMP_ERR_None;
}

VO_U32 voCMediaPlayer::IdleThreadProc (VO_PTR pParam)
{
	voCMediaPlayer * pPlayer = (voCMediaPlayer *)pParam;
	return pPlayer->IdleThreadLoop ();
}

int voCMediaPlayer::IdleThreadLoop (void)
{
	voThreadSetName (m_nIdleThdID, "VOME2 IDLE Thread");

#ifdef _LINUX_ANDROID
	int ret = 0;
	ret = setpriority(PRIO_PROCESS, 0, m_nIdleThdPriority);	
#else
	voThreadSetPriority (m_pIdleThread, VO_THREAD_PRIORITY_IDLE);
#endif // _LINUX_ANDROID

	int nStartTime = voOS_GetSysTime ();
	while (!m_bIdleStop)
	{
		if (voOS_GetSysTime () - nStartTime > 50)
		{
			voOS_Sleep (1);
			nStartTime = voOS_GetSysTime ();
		}
	}

	m_pIdleThread = NULL;

	VOLOGI ("Exit IDLE Thread!");

	return 0;
}

#if defined(_IOS) || defined(_MAC_OS)
bool voCMediaPlayer::CheckLicense(VO_TCHAR* pLicenseFilePath)
{
	bool bPass = false;
#ifdef _USE_VO_LOGO
	if (m_pLogoEffect)
	{
		delete m_pLogoEffect;
		m_pLogoEffect = NULL;
	}
	
	/*
	char szBuf[1024];
	memset(szBuf, 0, 1024);
	FILE* hFile = fopen(pLicenseFilePath, "rb");
	
	if(hFile)
	{
		fread(szBuf, 1, 1024, hFile);
		fclose(hFile);
		bPass = true;
		printf("%s\n", szBuf);
	}
	 */
	
	m_LicenseCheck.SetKey(m_szLicenseText);
	
	int nRet = m_LicenseCheck.CheckLicense(pLicenseFilePath);
	bPass = (nRet==0);

	if(!bPass)
	{
		m_pLogoEffect = new voLogoEffect;
	}
#else
    bPass = true;
#endif
	return bPass;
}
#endif

void * voCMediaPlayer::vomtGetAddress (void * pUserData, void * hLib, char * pFuncName, int nFlag)
{
	void * pFunc = NULL;
#ifdef _WIN32
#ifndef WINCE
	pFunc = GetProcAddress ((HMODULE)hLib, pFuncName);
#else //WINCE
	TCHAR		szLibName[1024];

	memset (szLibName, 0, 1024 * sizeof(TCHAR));
	::MultiByteToWideChar (CP_ACP, 0, pFuncName, -1, szLibName, sizeof (szLibName));
	pFunc = GetProcAddress ((HMODULE)hLib, szLibName);
#endif //WINCE
#elif defined _LINUX_ANDROID
	pFunc = voGetAddressSymbol (hLib, pFuncName);
#elif defined _MAC_OS
	pFunc = dlsym (hLib, pFuncName);
#endif
	//VOLOGI( "Func Name: %s  %p" , pFuncName , pFunc );
	return pFunc;
}

int voCMediaPlayer::vomtFreeLib (void * pUserData, void * hLib, int nFlag)
{
	if (hLib != NULL)
#ifdef _WIN32
		FreeLibrary ((HMODULE)hLib);
#elif defined _LINUX_ANDROID
		voFreeLibrary (hLib);
#elif defined _MAC_OS
		dlclose (hLib);
#endif // _WIN32

	hLib = NULL;

	return VO_ERR_NONE;
}

void * voCMediaPlayer::vomtLoadLib (void * pUserData, char * pLibName, int nFlag)
{
	voCMediaPlayer *pPlayer = (voCMediaPlayer *)pUserData;

	void * hDll = NULL;

#ifdef _METRO
	TCHAR	szDll[1024];
	memset (szDll, 0, 1024 * sizeof(TCHAR));
	if (m_hDll == NULL)
	{
		_tcscat (szDll, pLibName);
		m_hDll = LoadPackagedLibrary(szDll , 0);

		if (m_hDll == NULL)
		{
			_tcscat (szDll, _T(".Dll"));
			m_hDll = LoadPackagedLibrary (szDll , 0);
		}
	}

#elif defined _WIN32
	TCHAR	szDll[1024];
	TCHAR	szPath[1024];
	TCHAR	szLibName[1024];

	memset (szLibName, 0, 1024);
	::MultiByteToWideChar (CP_ACP, 0, pLibName, -1, szLibName, sizeof (szLibName));

	vostrcpy (szDll, pPlayer->m_szWorkPath);
	vostrcat (szDll, szLibName);
	hDll = LoadLibrary (szDll);

	if (hDll == NULL)
	{
		GetModuleFileName (NULL, szPath, sizeof (szPath));

		TCHAR * pPos = _tcsrchr (szPath, _T('\\'));
		if(pPos != NULL)
		{
			*(pPos + 1) = 0;

			_tcscpy (szDll, szPath);
			_tcscat (szDll, szLibName);
			hDll = LoadLibrary (szDll);
		}
	}

	if (hDll == NULL)
	{
		::GetModuleFileName (NULL, szPath, sizeof (szPath));
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

#elif defined _LINUX_ANDROID 

	char szFile[1024];
	char tempFile[1024];
	uint64_t nCpuFeature;

	strcpy (szFile, pPlayer->m_szWorkPath);
//	strcat (szFile, "lib/");

	memset(tempFile, 0, sizeof(tempFile));
	strcpy(tempFile, "/data/local/tmp/OnStreamPlayer/lib/");
	if (strstr(pLibName, "lib") == 0)
	{
		strcat(szFile, "lib");
		strcat(tempFile, "lib");
	}

	strcat(szFile, pLibName); 
	strcat(tempFile, pLibName);

	if( strstr( szFile , ".so" ) == 0 )
	{
		strcat( szFile , ".so" );
		strcat( tempFile, ".so" );
	}

	VOLOGI("Load lib name: %s!!!",szFile);

	if(voOS_EnableDebugMode(0))
	{
		hDll = voLoadLibrary (tempFile, RTLD_NOW);
		VOLOGI("Load Debug lib name: %s",tempFile);
	}

	if (hDll == NULL)
	{
		hDll = voLoadLibrary (szFile, RTLD_NOW);

		if (hDll == NULL)
		{
			strcpy (szFile, pPlayer->m_szWorkPath);
			strcat (szFile, "lib/");
			strcat (szFile, pLibName);

	        if( strstr( szFile , ".so" ) == 0 )
	            strcat( szFile , ".so" );

			hDll = voLoadLibrary (szFile, RTLD_NOW);
		}
		VOLOGI ("Load Library File %s, Handle %p", szFile, hDll);
	}else
	{
		VOLOGI ("Load Library File %s, Handle %p", tempFile, hDll);
	}
#elif defined _MAC_OS
  #if 0
	char szFile[1024];
	strcpy (szFile, pPlayer->m_szWorkPath);

	if (strstr(pLibName, "lib") == 0)
	{
		strcat(szFile, "lib");
		strcat(tempFile, "lib");
	}

	strcat(szFile, pLibName); 

	if(strstr(szFile , ".dylib") == 0)
	{
		strcat(szFile , ".dylib");
	}

	VOLOGI("Load lib name: %s!!!",szFile);

	hDll = voLoadLibrary (szFile, RTLD_NOW);

	if (hDll == NULL)
	{
		strcpy (szFile, pPlayer->m_szWorkPath);
		strcat (szFile, "lib/");
		strcat (szFile, pLibName);

		if( strstr( szFile , ".dylib" ) == 0 )
			strcat( szFile , ".dylib" );

		hDll = voLoadLibrary (szFile, RTLD_NOW);
	}

	VOLOGI ("Load Library File %s, Handle %p", szFile, hDll);

  #else

	VO_TCHAR libname[256] = {0};
	VO_TCHAR PathAndName [1024] = {0};

	if (0 != strncasecmp (pLibName, "lib", 3))
	{
		vostrcat(libname, "lib");
	}
	vostrcat(libname, pLibName);
	if (vostrstr(libname, ".dylib") == 0)
	{
		vostrcat(libname, ".dylib");
	}

	bool bAPP = false;
	VOLOGI ("dllfile: %s, workpath: %s!", libname, pPlayer->m_szWorkPath);

	if (hDll == NULL && pPlayer->m_szWorkPath != NULL && strcmp(pPlayer->m_szWorkPath, ""))
	{
		bAPP = true;

		vostrcpy(PathAndName, pPlayer->m_szWorkPath);

		if (pPlayer->m_szWorkPath[strlen(pPlayer->m_szWorkPath) - 1] != '/') 
		{
			vostrcat(PathAndName, "/");
		}

		vostrcat(PathAndName, libname);

		hDll = dlopen (PathAndName, RTLD_NOW);
	}

	VOLOGI ("bAPP: %d", bAPP);

	if (!bAPP && hDll == NULL)
	{        
		voOS_GetAppFolder(PathAndName, sizeof(PathAndName));
		vostrcat(PathAndName, libname);
		hDll = dlopen (PathAndName, RTLD_NOW);
		if (NULL == hDll)
		{
			voOS_GetAppFolder(PathAndName, sizeof(PathAndName));
			vostrcat(PathAndName, "lib/");
			vostrcat(PathAndName, libname);
			hDll = dlopen (PathAndName, RTLD_NOW);
		}

		if (NULL == hDll)
		{
			voOS_GetPluginModuleFolder(PathAndName, sizeof(PathAndName));
			vostrcat(PathAndName, libname);
			hDll = dlopen (PathAndName, RTLD_NOW);
		}

		if (NULL == hDll)
		{
			vostrcpy(PathAndName, "/Library/Internet Plug-Ins/");
			vostrcat(PathAndName, libname);
			hDll = dlopen (PathAndName, RTLD_NOW);
		}

		if (NULL == hDll)
		{
			hDll = dlopen (PathAndName, RTLD_NOW);
		}

		if (NULL == hDll)
		{
			if (strrchr(libname, '/') == NULL)
				vostrcpy(PathAndName, "/usr/lib/");

			vostrcat (PathAndName, libname);
			hDll = dlopen (PathAndName, RTLD_NOW);
		}
	}

	if (hDll)
	{
		VOLOGI("load: %s ok", PathAndName);
	}
	else {
		VOLOGI("load: %s error", libname);
	}
  #endif
#endif

	return hDll;
}
