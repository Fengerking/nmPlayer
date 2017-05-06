#include <assert.h>
#include "voString.h"
#include "COSBaseSource.h"
#include "voOSFunc.h"
#include "voProgramInfo.h"
#include "voCheck.h"
#include "voStreaming.h"

#define  LOG_TAG    "COSBaseSource"
#include "voLog.h"

COSBaseSource::COSBaseSource(VO_SOURCE2_LIB_FUNC *pLibop)
	: m_hDll(NULL)	
	, m_hSource (NULL)
	, m_nFlag(0)
	, m_nProgramNum(0)
	, m_nStreamIndex(0)
	, m_nAudioCodec(VOOSMP_AUDIO_CodingAAC)
	, m_nVideoCodec(VOOSMP_VIDEO_CodingH264)
	, m_nJavaENV(0)
	, m_nSeeking(0)
	, m_nSeekTime(0)
	, m_nStatus(VOOSSRC_STATUS_CLOSED)
	, m_nChanging(0)
	, m_pInProgramInfo(NULL)
	, m_pOutProgramInfo(NULL)
	, m_pInternalProgramInfo(NULL)
	, m_ullDuration(0)
	, m_pAudioCurTrack(NULL)
	, m_pVideoCurTrack(NULL)
	, m_pSubtitleCurTrack(NULL)
	, m_pStreamList(NULL)
	, m_pLibop(pLibop)
	, m_pSendBufFunc(NULL)
	, m_pSourceListener(NULL)
	, m_pSourceOnQest(NULL)
	, m_nSyncMsg(0)
	, m_nDelayTime(-1)
	, m_nOpenFinished(0)
	, m_pFileOP(NULL)
	, m_pbVOLOG(NULL)
	, m_pCSubtitle(NULL)
	, m_nSubtitleTrackNum(0)
	, m_nSubtitleCodecType(0)
	, m_nSubtitleSelect(0)
	, m_nLogAudioLevel(0)
	, m_nLogVideoLevel(0)
	, m_pAudioDumpFile(0)
	, m_pVideoDumpFile(0)
	, m_nDumpAudioLevel(0)
	, m_nDumpVideoLevel(0)
	, m_nStartSystemTime(0)
	, m_hAudioDumpFile(0)
	, m_hVideoDumpFile(0)
	, mCheckLib(NULL)
	, m_nAudioCount(0)
	, m_nVideoCount(0)
	, m_nSubtitleCount(0)
	, m_nExtendSubtitle(0)
	, m_nAudioSelected(-2)
	, m_nVideoSelected(-2)
	, m_nSubtitleSelected(-2)
	, m_nBAStreamEnable(1)
	, m_nAudioCurTrack(-1)
	, m_nVideoCurTrack(-1)
	, m_nSubtitleCurTrack(-1)
	, m_bBuffering(false)
{
	memset(&m_SourceAPI, 0, sizeof(m_SourceAPI));

	m_cReadBufFunc.pUserData = this;
	m_cReadBufFunc.ReadAudio = voSrcReadAudio;
	m_cReadBufFunc.ReadVideo = voSrcReadVideo;
	

	m_cEventCallBack.pUserData =  this;
	m_cEventCallBack.SendEvent =  vomtEventCallBack;

	m_cSampleCallBack.pUserData = this;
	m_cSampleCallBack.SendData = vomtSendCallBack;

	memset(&m_sAudioFormat, 0, sizeof(VOOSMP_BUFFER_FORMAT));
	memset(&m_sVideoFormat, 0, sizeof(VOOSMP_BUFFER_FORMAT));
	memset(&m_sSubtitleFormat, 0, sizeof(VOOSMP_BUFFER_FORMAT));
	memset(&m_sCCLanguageDescription, 0, sizeof(VO_CAPTION_LANGUAGE_DESCRIPTION));

	memset(m_szPathLib, 0, sizeof(VO_TCHAR)*1024);

	memset(m_pAudioTrackIndex, 0, sizeof(char)*128);
	memset(m_pVideoTrackIndex, 0, sizeof(char)*32);
	memset(m_pSubtitleTrackIndex, 0, sizeof(char)*128);
	memset(&m_szAudioProtery, 0, sizeof(VOOSMP_SRC_TRACK_PROPERTY));
	memset(&m_szVideoProtery, 0, sizeof(VOOSMP_SRC_TRACK_PROPERTY));
	memset(&m_szSubtitleProtery, 0, sizeof(VOOSMP_SRC_TRACK_PROPERTY));
	ClearPropertyItem(&m_szAudioProtery);
	ClearPropertyItem(&m_szVideoProtery);
	ClearPropertyItem(&m_szSubtitleProtery);

	m_pMsgThread = new voCBaseThread ("vo source Message");

	OpenConfig();

	if (g_fileOP.Open == NULL)
		cmnFileFillPointer ();
	m_pFileOP = &g_fileOP;

	// added by gtxia
	assert(mCheckLib == NULL);
	if(!mCheckLib)
		voCheckLibInit(&mCheckLib, 0, VO_LCS_LIBOP_FLAG, NULL, (VO_LIB_OPERATOR  *)m_pLibop);
}

COSBaseSource::~COSBaseSource ()
{
	Uninit();

	if(m_hAudioDumpFile && m_pFileOP)
	{
		m_pFileOP->Close(m_hAudioDumpFile);
	}

	if(m_hVideoDumpFile && m_pFileOP)
	{
		m_pFileOP->Close (m_hVideoDumpFile);
	}

	if(m_pCSubtitle)
	{
		delete m_pCSubtitle;
		m_pCSubtitle = NULL;
	}

	if (m_pMsgThread != NULL)
	{
		m_pMsgThread->Stop ();
		delete m_pMsgThread;
		m_pMsgThread = NULL;
	}
	
	if(m_pInProgramInfo)
	{
		ReleaseProgramInfoOP_T(m_pInProgramInfo);
		m_pInProgramInfo = NULL;
	}
	if(m_pOutProgramInfo)
	{
		ReleaseProgramInfoOP_T(m_pOutProgramInfo);
		m_pOutProgramInfo = NULL;
	}
	if(m_pInternalProgramInfo)
	{
		ReleaseProgramInfoOP_T(m_pInternalProgramInfo);
		m_pInternalProgramInfo = NULL;
	}

	if(m_pAudioCurTrack)
	{
		ReleaseTrackInfoOP_T(m_pAudioCurTrack);
		m_pAudioCurTrack = NULL;
	}

	if(m_pVideoCurTrack)
	{
		ReleaseTrackInfoOP_T(m_pVideoCurTrack);
		m_pVideoCurTrack = NULL;
	}

	if(m_pSubtitleCurTrack)
	{
		ReleaseTrackInfoOP_T(m_pSubtitleCurTrack);
		m_pSubtitleCurTrack = NULL;
	}

	ClearPropertyItem(&m_szAudioProtery);
	ClearPropertyItem(&m_szVideoProtery);
	ClearPropertyItem(&m_szSubtitleProtery);

	unLoadDll();
}

int	COSBaseSource::Init(void * pSource, int nFlag, void* pInitparam, int nInitFlag)
{
	if(m_SourceAPI.Init == NULL)
	{
		VOLOGE("m_SourceAPI.Init 0x%08X", m_SourceAPI.Init);
		return VOOSMP_ERR_Implement;
	}

	VOLOGI("Init+++ %d", voOS_GetSysTime());

	if(m_pMsgThread)
		m_pMsgThread->Start();

	if(m_hSource)
		Uninit();

	VO_SOURCE2_INITPARAM sInit;
	memset(&sInit, 0, sizeof(VO_SOURCE2_INITPARAM));

	sInit.pInitParam = pInitparam;
	sInit.uFlag = nInitFlag;

	m_nFlag = nFlag;

	m_nOpenFinished = 0;

	if(m_SourceAPI.SetParam != NULL)
		m_SourceAPI.SetParam(m_hSource, VO_PID_COMMON_LOGFUNC, m_pbVOLOG);

	nFlag |= VO_LCS_WORKPATH_FLAG;
	sInit.strWorkPath = m_szPathLib;

	int nRC = m_SourceAPI.Init(&m_hSource, pSource, nFlag, &sInit);
	if(VO_RET_SOURCE2_OK != nRC)
	{
		VOLOGE("failed to m_SourceAPI.Init 0x%08X", nRC);
		return ConvertReturnValue(nRC);
	}

	if(m_SourceAPI.SetParam == NULL  || m_hSource == NULL)
	{
		VOLOGE("m_SourceAPI.SetParam 0x%08X, m_hSource 0x%08X", m_SourceAPI.SetParam, m_hSource);
		return VOOSMP_ERR_Implement;
	}

	m_SourceAPI.SetParam(m_hSource, VO_PID_COMMON_LOGFUNC, m_pbVOLOG);
//	m_SourceAPI.SetParam(m_hSource, VO_PID_SOURCE2_LIBOP, m_pLibop);
	m_SourceAPI.SetParam(m_hSource, VO_PID_SOURCE2_WORKPATH, m_szPathLib);
	m_SourceAPI.SetParam(m_hSource, VO_PID_SOURCE2_EVENTCALLBACK, &m_cEventCallBack);
	m_SourceAPI.SetParam(m_hSource, VO_PID_SOURCE2_SAMPLECALLBACK, &m_cSampleCallBack);	

	VO_FILE_SOURCE filSource;
	memset (&filSource, 0, sizeof (VO_FILE_SOURCE));
	filSource.nFlag = VO_FILE_TYPE_NAME;
	filSource.nMode = VO_FILE_WRITE_ONLY;

	if(m_pAudioDumpFile && m_pFileOP)
	{
#ifdef _WIN32			
		VO_TCHAR szFile[1024];
		memset (szFile, 0, sizeof (szFile));
		MultiByteToWideChar (CP_ACP, 0, m_pAudioDumpFile, -1, szFile, sizeof (szFile)); 
		filSource.pSource = szFile;		
#elif defined _IOS
        char szFile[1024];
        voOS_GetAppFolder(szFile, 1024);
        vostrcat(szFile, m_pAudioDumpFile);
		filSource.pSource = szFile;
#else
		filSource.pSource = m_pAudioDumpFile;
#endif
		m_hAudioDumpFile = m_pFileOP->Open (&filSource);
	}

	if(m_pVideoDumpFile && m_pFileOP)
	{
#ifdef _WIN32
		VO_TCHAR szFile[1024];
		memset (szFile, 0, sizeof (szFile));
		MultiByteToWideChar (CP_ACP, 0, m_pVideoDumpFile, -1, szFile, sizeof (szFile)); 
		filSource.pSource = szFile;	
#elif defined _IOS
        char szFile[1024];
        voOS_GetAppFolder(szFile, 1024);
        vostrcat(szFile, m_pVideoDumpFile);
		filSource.pSource = szFile;
#else
		filSource.pSource = m_pVideoDumpFile;
#endif
		m_hVideoDumpFile = m_pFileOP->Open (&filSource);
	}

	if(m_pCSubtitle)
	{
		m_pCSubtitle->Init(0, 0, 0);
		m_pCSubtitle->SetParam (ID_SET_JAVA_ENV, (void *)m_nJavaENV);
	}

	VOLOGI("Init--- 0x%08X %d", nRC, voOS_GetSysTime());
	return ConvertReturnValue(nRC);
}

int	COSBaseSource::Uninit()
{
	if(m_SourceAPI.Uninit == NULL  || m_hSource == NULL)
	{
		VOLOGE("m_SourceAPI.Uninit 0x%08X, m_hSource 0x%08X", m_SourceAPI.Uninit, m_hSource);
		return VOOSMP_ERR_Implement;
	}

	// added by gtxia
	if(mCheckLib)
		voCheckLibUninit(mCheckLib);

	VOLOGI("Uninit+++ %d", voOS_GetSysTime());

	int nRC = m_SourceAPI.Uninit(m_hSource);
	m_hSource = NULL;

	if(m_pCSubtitle)
		m_pCSubtitle->Uninit();

	if(m_pMsgThread)
		m_pMsgThread->Stop();

	VOLOGI("Uninit--- %d", voOS_GetSysTime());
	return ConvertReturnValue(nRC);
}

void* COSBaseSource::GetReadBufPtr()
{
	return &m_cReadBufFunc;
}

int COSBaseSource::Open (void)
{
	if(m_SourceAPI.Open == NULL || m_hSource == NULL)
	{
		VOLOGE("m_SourceAPI.Open 0x%08X, m_hSource 0x%08X", m_SourceAPI.Open, m_hSource);
		return VOOSMP_ERR_Implement;
	}

	VOLOGI("Open+++ %d", voOS_GetSysTime());
	voCAutoLock lockReadSrc (&m_MutexReadData);
	VOLOGI("Open enter m_MutexReadData %d", voOS_GetSysTime());

	CheckingStatus(100);

	if(m_nChanging)
	{
		VOLOGW("m_nChanging %d", m_nChanging);
		return VOOSMP_ERR_Retry;
	}

	m_nOpenFinished = 0;

	m_nChanging = 1;
	int nRC = m_SourceAPI.Open(m_hSource);
	if(VO_RET_SOURCE2_OK == nRC)
		m_nStatus = VOOSSRC_STATUS_OPENED;
	m_nChanging = 0;

	if ((nRC & VO_ERR_LICENSE_ERROR) == VO_ERR_LICENSE_ERROR)
	{
		VOLOGE("m_SourceAPI.Open failed due to license check error 0x%08X", nRC);
		return  VOOSMP_ERR_LicenseFailed;
	}

	if(VO_RET_SOURCE2_OK != nRC)
	{
		VOLOGE("failed to m_SourceAPI.Open 0x%08X", nRC);
		return ConvertReturnValue(nRC);
	}

	if((m_nFlag& VO_SOURCE2_FLAG_OPEN_ASYNC) == 0)
	{
		m_nOpenFinished = 1;
		InitSrcParam();
	}

	VOLOGI("Open--- %d", voOS_GetSysTime());
	return ConvertReturnValue(nRC);
}

int COSBaseSource::Run (void)
{
	if(m_SourceAPI.Start == NULL  || m_hSource == NULL)
	{
		VOLOGE("m_SourceAPI.Start 0x%08X, m_hSource 0x%08X", m_SourceAPI.Start, m_hSource);
		return VOOSMP_ERR_Implement;
	}

	VOLOGI("Run+++ %d", voOS_GetSysTime());
	voCAutoLock lockReadSrc (&m_MutexReadData);
	VOLOGI("Run enter m_MutexReadData %d", voOS_GetSysTime());

	CheckingStatus(500);

	if(m_nChanging || m_nOpenFinished == 0)
	{
		VOLOGW("m_nChanging %d, m_nOpenFinished %d, need retry", m_nChanging, m_nOpenFinished);
		return VOOSMP_ERR_Retry;
	}

	if(VOOSSRC_STATUS_RUNNING == m_nStatus)
	{
		VOLOGW("OS source is running, just retun OK!");
		return VOOSMP_ERR_None;
	}
	
	m_nChanging = 1;
	int nRC = m_SourceAPI.Start(m_hSource);
	if(VO_RET_SOURCE2_OK == nRC)
		m_nStatus = VOOSSRC_STATUS_RUNNING;
	m_nChanging = 0;

	if(VO_RET_SOURCE2_OK != nRC)
	{
		VOLOGE("failed to m_SourceAPI.Start 0x%08X", nRC);
		return ConvertReturnValue(nRC);
	}

	VOLOGI("Run--- %d", voOS_GetSysTime());
	return VOOSMP_ERR_None;
}

int COSBaseSource::Pause (void)
{	
	if(m_SourceAPI.Pause == NULL  || m_hSource == NULL)
	{
		VOLOGE("m_SourceAPI.Pause 0x%08X, m_hSource 0x%08X", m_SourceAPI.Pause, m_hSource);
		return VOOSMP_ERR_Implement;
	}

	VOLOGI("Pause+++ %d", voOS_GetSysTime());
	voCAutoLock lockReadSrc (&m_MutexReadData);
	VOLOGI("Pause enter m_MutexReadData %d", voOS_GetSysTime());

	CheckingStatus(100);

	if(m_nChanging)
	{
		VOLOGW("m_nChanging %d", m_nChanging);
		return VOOSMP_ERR_Retry;
	}

	if(VOOSSRC_STATUS_RUNNING != m_nStatus)
	{
		VOLOGW("OS source isn't running, just retun status error!");
		return VOOSMP_ERR_Status;
	}

	m_nChanging = 1;
	int nRC = m_SourceAPI.Pause(m_hSource);
	if(VO_RET_SOURCE2_OK == nRC)
		m_nStatus = VOOSSRC_STATUS_PAUSED;
	m_nChanging = 0;

	VOLOGI("Pause--- 0x%08X %d", nRC, voOS_GetSysTime());
	return ConvertReturnValue(nRC);
}

int COSBaseSource::Stop (void)
{
	if(m_SourceAPI.Stop == NULL || m_hSource == NULL)
	{
		VOLOGE("m_SourceAPI.Stop 0x%08X, m_hSource 0x%08X", m_SourceAPI.Stop, m_hSource);
		return VOOSMP_ERR_Implement;
	}

	VOLOGI("Stop+++ %d", voOS_GetSysTime());
	voCAutoLock lockReadSrc (&m_MutexReadData);
	VOLOGI("Stop enter m_MutexReadData %d", voOS_GetSysTime());

	CheckingStatus(1000);

	if(m_nChanging)
	{
		VOLOGW("m_nChanging %d", m_nChanging);
		return VOOSMP_ERR_Retry;
	}

	m_nChanging = 1;
	int nRC = m_SourceAPI.Stop(m_hSource);
	if(VO_RET_SOURCE2_OK == nRC)
		m_nStatus = VOOSSRC_STATUS_STOPPED;
	m_nChanging = 0;

	VOLOGI("Stop--- 0x%08X %d", nRC, voOS_GetSysTime());
	return ConvertReturnValue(nRC);
}

int COSBaseSource::Close (void)
{
	if(m_SourceAPI.Close == NULL  || m_hSource == NULL)
	{
		VOLOGE("m_SourceAPI.Close 0x%08X, m_hSource 0x%08X", m_SourceAPI.Close, m_hSource);
		return VOOSMP_ERR_Implement;
	}

	VOLOGI("Close+++ %d", voOS_GetSysTime());
	voCAutoLock lockReadSrc (&m_MutexReadData);
	VOLOGI("Close enter m_MutexReadData %d", voOS_GetSysTime());

	CheckingStatus(1000);

	if(m_nChanging)
	{
		VOLOGW("m_nChanging %d", m_nChanging);
		return VOOSMP_ERR_Retry;
	}

	m_nChanging = 1;
	int nRC = m_SourceAPI.Close(m_hSource);
	if(VO_RET_SOURCE2_OK == nRC)
		m_nStatus = VOOSSRC_STATUS_CLOSED;
	m_nChanging = 0;

	m_nOpenFinished = 0;

	VOLOGI("Close--- 0x%08X %d", nRC, voOS_GetSysTime());
	return ConvertReturnValue(nRC);
}

int COSBaseSource::GetDuration (long long * pDuration)
{
	if(m_SourceAPI.GetDuration == NULL  || m_hSource == NULL || m_nOpenFinished == 0)
	{
		VOLOGE("m_SourceAPI.GetDuration 0x%08X, m_hSource 0x%08X, m_nOpenFinished %d", m_SourceAPI.GetDuration, m_hSource, m_nOpenFinished);
		return VOOSMP_ERR_Implement;
	}

	VOLOGI("GetDuration+++ %d", voOS_GetSysTime());
	voCAutoLock lockProgramInfo (&m_MutexProgramInfo);
	VOLOGI("GetDuration enter m_MutexProgramInfo %d", voOS_GetSysTime());

	*pDuration = m_ullDuration;

	VOLOGI("GetDuration--- %d", voOS_GetSysTime());
	return ConvertReturnValue(VO_RET_SOURCE2_OK);
}

int COSBaseSource::GetSample(VO_SOURCE2_TRACK_TYPE nTrackType , void* pSample)
{
	if(m_SourceAPI.GetSample == NULL  || m_hSource == NULL || pSample == NULL)
	{
		VOLOGE("m_SourceAPI.GetSample 0x%08X, m_hSource 0x%08X, pSample 0x%08X", m_SourceAPI.GetSample, m_hSource, pSample);
		return VOOSMP_ERR_Implement;
	}

	voCAutoLock lockReadSrc (&m_MutexReadData);

	int nRC = 0;
	if(nTrackType == VO_SOURCE2_TT_SUBTITLE)
	{
		if(m_pCSubtitle == NULL)
			return VOOSMP_ERR_Implement;

		voSubtitleInfo *pSubSample = NULL;

		int nSubtileNum = m_pCSubtitle->GetSubTitleNum();
		VOLOGI("@@@### GetSubTileNum = %d", nSubtileNum);
		if(nSubtileNum > 0)
		{
			nRC = m_pCSubtitle->GetSubtitleInfo((long)((voSubtitleInfo *)pSample)->nTimeStamp, &pSubSample);
			VOLOGI("@@@### GetSubtitleInfo nRC = 0x%08X", nRC);
			if (nRC == VOOSMP_ERR_None)
				memcpy(pSample,pSubSample, sizeof(voSubtitleInfo));
			return nRC;
		}
		
		if(m_pCSubtitle->GetSubtitleType() != VOOS_SUBTITLE_CLOSECAPTION)
		{
			VO_SOURCE2_SAMPLE nSample;
			memset(&nSample, 0, sizeof(VO_SOURCE2_SAMPLE));
			nSample.uTime = ((voSubtitleInfo *)pSample)->nTimeStamp;
			nRC = m_SourceAPI.GetSample(m_hSource, nTrackType, &nSample);
			VOLOGI("@@@### Subtitle [not closecaption]   m_SourceAPI.GetSample nRC = 0x%08X, Time %d, size %d, nType %d", nRC, (int)nSample.uTime, (int)nSample.uSize, (int)nSample.pReserve1);
			if(nRC == 0)
			{
				VOLOGI("m_SourceAPI.GetSample return %x, Time %d, size %d, nType %d", nRC, (int)nSample.uTime, (int)nSample.uSize, (int)nSample.pReserve1);
			}

			if(nRC == VOOSMP_ERR_None && nSample.uSize > 0)
			{
				int nType = 0;
				nType = (int)nSample.pReserve1;
#if 0
				FILE* pFile = fopen("/sdcard/sbIn.xml", "ab+");
				{
					if (pFile)
					{
						fwrite (nSample.pBuffer, nSample.uSize, 1, pFile);
						fclose (pFile);
					}
				}
#endif
				nRC = m_pCSubtitle->SetInputData((VO_CHAR *)nSample.pBuffer, nSample.uSize, (int)nSample.uTime, nType);
				VOLOGI("@@@### SubtitleDec SetInputData nRC = 0x%08X, uTime = %d", nRC, (int)(nSample.uTime));
			}

			nRC = m_pCSubtitle->GetSubtitleInfo((long)((voSubtitleInfo *)pSample)->nTimeStamp, &pSubSample);
			VOLOGI("@@@### SubtitleDec GetOutputData nRC = 0x%08X, Time = %d", nRC, (int)((voSubtitleInfo *)pSample)->nTimeStamp);

			if(nRC == 0)
			{
				VOLOGI("@@@### SubtitleDec GetOutputData return 0, Time %d", nRC, (int)((voSubtitleInfo *)pSample)->nTimeStamp);
#if 0
				FILE* pFile = fopen("/sdcard/sbOut.png", "wb");
				{
					if (pFile)
					{
						if(pSubSample->pSubtitleEntry)
						{
							if (pSubSample->pSubtitleEntry->stSubtitleDispInfo.pImageInfo)
							{
								if (pSubSample->pSubtitleEntry->stSubtitleDispInfo.pImageInfo->stImageData.pPicData)
								{
									VOLOGI("@@@### dumpSubTitle PNG image size = %d", pSubSample->pSubtitleEntry->stSubtitleDispInfo.pImageInfo->stImageData.nSize);
									fwrite(pSubSample->pSubtitleEntry->stSubtitleDispInfo.pImageInfo->stImageData.pPicData, pSubSample->pSubtitleEntry->stSubtitleDispInfo.pImageInfo->stImageData.nSize, 1, pFile);
								}
								else
								{
									VOLOGI ("@@@### dumpSubTitle pSubSample->pSubtitleEntry->stSubtitleDispInfo.pImageInfo->stImageData.pPicData = NULL");
								}
							}
							else
							{
								VOLOGI("@@@### dumpSubTitle pSubSample->pSubtitleEntry->stSubtitleDispInfo.pImageInfo = NULL");
							}
						}
						else
						{
							VOLOGI("@@@###  dumpSubTitle pSubSample->pSubtitleEntry = NULL");
						}
						fclose (pFile);
					}
				}
#endif
			}

			if (nRC == VOOSMP_ERR_None)
				memcpy(pSample,pSubSample, sizeof(voSubtitleInfo));
		}
		else
		{
			VO_SOURCE2_SAMPLE nSample;
			memset(&nSample, 0, sizeof(VO_SOURCE2_SAMPLE));
			nSample.uTime = ((voSubtitleInfo *)pSample)->nTimeStamp;

			nRC = m_pCSubtitle->GetSubtitleInfo((long)((voSubtitleInfo *)pSample)->nTimeStamp, &pSubSample);

			
			if(nRC == 0)
				VOLOGI("m_pCSubtitle Get Subtitle return %d, Time %d", nRC,(int)pSubSample->nTimeStamp);

			if (nRC == VOOSMP_ERR_None)
				memcpy(pSample,pSubSample, sizeof(voSubtitleInfo));
		}

		return ConvertReturnValue(nRC);
	}

	nRC = m_SourceAPI.GetSample(m_hSource, nTrackType, pSample);

	return ConvertReturnValue(nRC);
}

int COSBaseSource::GetProgramCount(unsigned int  *pStreamCount)
{
	if(m_SourceAPI.GetProgramCount == NULL  || m_hSource == NULL || m_nOpenFinished == 0)
	{
		VOLOGE("m_SourceAPI.GetProgramCount 0x%08X, m_hSource 0x%08X, m_nOpenFinished %d", m_SourceAPI.GetProgramCount, m_hSource, m_nOpenFinished);
		return VOOSMP_ERR_Implement;
	}

	VOLOGI("GetProgramCount+++ %d", voOS_GetSysTime());
	voCAutoLock lockProgramInfo (&m_MutexProgramInfo);
	VOLOGI("GetProgramCount enter m_MutexProgramInfo %d", voOS_GetSysTime());

//	int nRC = m_SourceAPI.GetProgramCount(m_hSource, (VO_U32 *)pStreamCount);

	*((VO_U32 *)pStreamCount) = 1;

	VOLOGI("GetProgramCount--- %d", voOS_GetSysTime());
	return ConvertReturnValue(VO_RET_SOURCE2_OK);
}

int COSBaseSource::GetProgramInfo(unsigned int nProgram, VO_SOURCE2_PROGRAM_INFO ** ppProgramInfo)
{
	if(m_SourceAPI.GetProgramInfo == NULL  || m_hSource == NULL || m_nOpenFinished == 0)
	{
		VOLOGE("m_SourceAPI.GetProgramInfo 0x%08X, m_hSource 0x%08X, m_nOpenFinished %d", m_SourceAPI.GetProgramInfo, m_hSource, m_nOpenFinished);
		return VOOSMP_ERR_Implement;
	}

	VOLOGI("GetProgramInfo+++ %d", voOS_GetSysTime());
	voCAutoLock lockProgramInfo (&m_MutexProgramInfo);
	VOLOGI("GetProgramInfo enter m_MutexProgramInfo %d", voOS_GetSysTime());
	
	if(NULL == m_pInternalProgramInfo)
	{
		VOLOGE("GetProgramInfo--- m_pInternalProgramInfo %p", m_pInternalProgramInfo);
		return ConvertReturnValue(VO_RET_SOURCE2_FAIL);
	}

	//below code add for copy program info
	if(m_pOutProgramInfo)
	{
		ReleaseProgramInfoOP_T(m_pOutProgramInfo);
		m_pOutProgramInfo = NULL;
	}

	// copy program info here in case of shared program info changed when do parse
	CopyProgramInfoOP_T(m_pInternalProgramInfo, &m_pOutProgramInfo);
	*ppProgramInfo = m_pOutProgramInfo;
	
	VOLOGI("GetProgramInfo--- %d", voOS_GetSysTime());
	return ConvertReturnValue(VO_RET_SOURCE2_OK);
}

int COSBaseSource::GetCurTrackInfo(VO_SOURCE2_TRACK_TYPE nTrackType, VO_SOURCE2_TRACK_INFO ** ppTrackInfo)
{
	if(m_SourceAPI.GetCurTrackInfo == NULL  || m_hSource == NULL || ppTrackInfo == NULL || m_nOpenFinished == 0)
	{
		VOLOGE("m_SourceAPI.GetCurTrackInfo 0x%08X, m_hSource 0x%08X, ppTrackInfo 0x%08X, m_nOpenFinished %d", m_SourceAPI.GetCurTrackInfo, m_hSource, ppTrackInfo, m_nOpenFinished);
		return VOOSMP_ERR_Implement;
	}

	VOLOGI("GetCurTrackInfo+++ Type %d %d", nTrackType, voOS_GetSysTime());
	voCAutoLock lockReadSrc (&m_MutexReadData);
	VOLOGI("GetCurTrackInfo enter m_MutexReadData Type %d %d", nTrackType, voOS_GetSysTime());
	voCAutoLock lockProgramInfo (&m_MutexProgramInfo);
	VOLOGI("GetCurTrackInfo enter m_MutexProgramInfo Type %d %d", nTrackType, voOS_GetSysTime());

	int nRC =  VOOSMP_ERR_Implement;
	if(nTrackType == VO_SOURCE2_TT_SUBTITLE)
	{
		if(m_pInternalProgramInfo == NULL || m_pInternalProgramInfo->ppStreamInfo == NULL)
		{
			VOLOGE("m_pInternalProgramInfo 0x%08X", m_pInternalProgramInfo);
			return nRC;
		}

		VO_SOURCE2_STREAM_INFO* pStreamInfo = NULL;
		VO_SOURCE2_TRACK_INFO* pTrackInfo = NULL;
		unsigned int n, m;

		for(n = 0; n < m_pInternalProgramInfo->uStreamCount; n++)
		{
			pStreamInfo = m_pInternalProgramInfo->ppStreamInfo[n];

			if(pStreamInfo == NULL || pStreamInfo->ppTrackInfo == NULL)
				continue;

			if(!(pStreamInfo->uSelInfo&VO_SOURCE2_SELECT_SELECTED))
				continue;

			for(m = 0; m < pStreamInfo->uTrackCount; m++)
			{
				pTrackInfo = pStreamInfo->ppTrackInfo[m];

				if(pTrackInfo == NULL)
					continue;				

				if(pTrackInfo->uTrackType == VO_SOURCE2_TT_SUBTITLE)
				{
					if(pTrackInfo->uSelInfo & VO_SOURCE2_SELECT_SELECTED)
					{
						if(m_pSubtitleCurTrack)
						{
							ReleaseTrackInfoOP_T(m_pSubtitleCurTrack);
							m_pSubtitleCurTrack = NULL;
						}

						CopyTrackInfoOP_T(pTrackInfo, &m_pSubtitleCurTrack);

						*ppTrackInfo = m_pSubtitleCurTrack;

						VOLOGI("GetCurTrackInfo--- Type %d %d", nTrackType, voOS_GetSysTime());
						return VOOSMP_ERR_None;
					}					
				}
			}
		}	
	}
	else
	{
		nRC = m_SourceAPI.GetCurTrackInfo(m_hSource, nTrackType, ppTrackInfo);
	}

	VOLOGI("GetCurTrackInfo--- Type %d 0x%08X %d", nTrackType, nRC, voOS_GetSysTime());
	return ConvertReturnValue(nRC);
}

int COSBaseSource::SelectProgram(unsigned int nProgram)
{
	if(m_SourceAPI.SelectProgram == NULL  || m_hSource == NULL)
	{
		VOLOGE("m_SourceAPI.SelectProgram 0x%08X, m_hSource 0x%08X", m_SourceAPI.SelectProgram, m_hSource);
		return VOOSMP_ERR_Implement;
	}

	VOLOGI("SelectProgram+++ Program %d %d", nProgram, voOS_GetSysTime());
	voCAutoLock lockReadSrc (&m_MutexReadData);
	VOLOGI("SelectProgram Program %d enter m_MutexReadData %d", nProgram, voOS_GetSysTime());
	voCAutoLock lockProgramInfo (&m_MutexProgramInfo);
	VOLOGI("SelectProgram Program %d enter m_MutexProgramInfo %d", nProgram, voOS_GetSysTime());

	int nRC = m_SourceAPI.SelectProgram(m_hSource, nProgram);

	VOLOGI("SelectProgram--- Program %d 0x%08X %d", nProgram, nRC, voOS_GetSysTime());
	return ConvertReturnValue(nRC);
}

int COSBaseSource::SelectStream(unsigned int nStream)
{
	if(m_SourceAPI.SelectStream == NULL  || m_hSource == NULL)
	{
		VOLOGE("m_SourceAPI.SelectStream 0x%08X, m_hSource 0x%08X", m_SourceAPI.SelectStream, m_hSource);
		return VOOSMP_ERR_Implement;
	}

	VOLOGI("SelectStream+++ Stream %d %d", nStream, voOS_GetSysTime());
	voCAutoLock lockReadSrc (&m_MutexReadData);
	VOLOGI("SelectStream Stream %d enter m_MutexReadData %d", nStream, voOS_GetSysTime());
	voCAutoLock lockProgramInfo (&m_MutexProgramInfo);
	VOLOGI("SelectStream Stream %d enter m_MutexProgramInfo %d", nStream, voOS_GetSysTime());

	int nRC = m_SourceAPI.SelectStream(m_hSource, nStream);

	VOLOGI("SelectStream--- Stream %d 0x%08X %d", nStream, nRC, voOS_GetSysTime());
	return ConvertReturnValue(nRC);
}

int COSBaseSource::SelectTrack(unsigned int nTrack)
{
	if(m_SourceAPI.SelectTrack == NULL  || m_hSource == NULL)
	{
		VOLOGE("m_SourceAPI.SelectTrack 0x%08X, m_hSource 0x%08X", m_SourceAPI.SelectTrack, m_hSource);
		return VOOSMP_ERR_Implement;
	}

	VOLOGI("SelectTrack+++ Track %d %d", nTrack, voOS_GetSysTime());
	voCAutoLock lockReadSrc (&m_MutexReadData);
	VOLOGI("SelectTrack enter m_MutexReadData Track %d %d", nTrack, voOS_GetSysTime());
	m_MutexProgramInfo.Lock();
	VOLOGI("SelectTrack enter m_MutexProgramInfo Track %d m_pCSubtitle 0x%08X %d", nTrack, m_pCSubtitle, voOS_GetSysTime());

	int nRC = VOOSMP_ERR_Implement;
	if(nTrack >= 0x10000 && m_pCSubtitle)
	{
		if(m_nSubtitleSelect == (int)(nTrack&0xffff))
		{
			VOLOGE("m_nSubtitleSelect %d, nTrack %d", m_nSubtitleSelect, nTrack);
			m_MutexProgramInfo.Unlock();
			return VOOSMP_ERR_Implement;
		}
		else
			nRC = m_pCSubtitle->SelectTrackByIndex(nTrack&0xffff);

		m_nSubtitleSelect = nTrack&0xffff;

		voCAutoLock lockpost (&m_MutexPostEvent);
		VOLOGI("SelectTrack enter m_MutexPostEvent Track %d %d", nTrack, voOS_GetSysTime());
		postMsgEvent(VOOSMP_SRC_CB_Program_Changed, 0, 0, 0);
		m_MutexProgramInfo.Unlock();
	}
	else
	{
		m_MutexProgramInfo.Unlock();
		nRC = m_SourceAPI.SelectTrack(m_hSource, nTrack);

		if(nRC == 0)
			InitSrcParam();
	}

	VOLOGI("SelectTrack-- 0x%08X %d", nRC, voOS_GetSysTime());
	return ConvertReturnValue(nRC);
}

int COSBaseSource::SelectSubLang(char *pLan)
{
	if(pLan == NULL)
	{
		VOLOGE("pLan 0x%08X", pLan);
		return VOOSMP_ERR_Implement;
	}

	VOLOGI("SelectSubLang+++ %d", voOS_GetSysTime());
	voCAutoLock lockProgramInfo (&m_MutexProgramInfo);
	VOLOGI("SelectSubLang enter m_MutexProgramInfo %d", voOS_GetSysTime());

	int nRC = VOOSMP_ERR_Implement;
	if(m_pCSubtitle)
	{
		unsigned int n, m;	
		VO_SOURCE2_STREAM_INFO* pStreamInfo = NULL;
		VO_SOURCE2_TRACK_INFO* pTrackInfo = NULL;

		if(m_pInternalProgramInfo == NULL)
		{
			VOLOGE("m_pInternalProgramInfo 0x%08X", m_pInternalProgramInfo);
			return nRC;
		}

		for(n = 0; n < m_pInternalProgramInfo->uStreamCount; n++)
		{
			pStreamInfo = m_pInternalProgramInfo->ppStreamInfo[n];

			if(pStreamInfo == NULL)
				continue;

			if(!(pStreamInfo->uSelInfo&VO_SOURCE2_SELECT_SELECTED))
				continue;

			for(m = 0; m < pStreamInfo->uTrackCount; m++)
			{
				pTrackInfo = pStreamInfo->ppTrackInfo[m];

				if(pTrackInfo == NULL)
					continue;

				if(pTrackInfo->uTrackType == VO_SOURCE2_TT_SUBTITLE)
				{
					if(strcmp(pTrackInfo->sSubtitleInfo.chLanguage, pLan) == 0)
					{
						if(pTrackInfo->uSelInfo&VO_SOURCE2_SELECT_SELECTED)
							return VOOSMP_ERR_Implement;

						pTrackInfo->uSelInfo |= VO_SOURCE2_SELECT_SELECTED;

						m_nSubtitleSelect = pTrackInfo->uTrackID&0xffff;
						nRC = m_pCSubtitle->SelectTrackByIndex(pTrackInfo->uTrackID&0xffff);
					}
					else
					{
						pTrackInfo->uSelInfo = 0;
					}
				}
			}
		}
	}

	VOLOGI("SelectSubLang--- 0x%08X %d", nRC, voOS_GetSysTime());
	return nRC;
}

int COSBaseSource::GetDRMInfo(VO_SOURCE2_DRM_INFO **ppDRMInfo)
{
	if(m_SourceAPI.GetDRMInfo == NULL  || m_hSource == NULL)
	{
		VOLOGE("m_SourceAPI.GetDRMInfo 0x%08X, m_hSource 0x%08X", m_SourceAPI.GetDRMInfo, m_hSource);
		return VOOSMP_ERR_Implement;
	}

	int nRC = m_SourceAPI.GetDRMInfo(m_hSource, ppDRMInfo);

	VOLOGI("@@@### m_SourceAPI.GetDRMInfo() ret Convert");
	return ConvertReturnValue(nRC);
}

int COSBaseSource::SendBuffer(const VO_SOURCE2_SAMPLE& buffer )
{
	if(m_SourceAPI.SendBuffer == NULL  || m_hSource == NULL)
	{
		VOLOGE("m_SourceAPI.SendBuffer 0x%08X, m_hSource 0x%08X", m_SourceAPI.SendBuffer, m_hSource);
		return VOOSMP_ERR_Implement;
	}

	int nRC = m_SourceAPI.SendBuffer(m_hSource, buffer);

	VOLOGI("@@@### m_SourceAPI.SendBuffer() ret Convert");
	return ConvertReturnValue(nRC);
}

int COSBaseSource::SetCurPos (long long *pCurPos)
{
	if(m_SourceAPI.Seek == NULL  || m_hSource == NULL || pCurPos == NULL)
	{
		VOLOGE("m_SourceAPI.Seek 0x%08X, m_hSource 0x%08X, pCurPos 0x%08X", m_SourceAPI.Seek, m_hSource, pCurPos);
		return VOOSMP_ERR_Implement;
	}

	VOLOGI("SetCurPos+++ Position %d %d", (int)(*pCurPos), voOS_GetSysTime());
	voCAutoLock lockReadSrc (&m_MutexReadData);
	VOLOGI("SetCurPos enter m_MutexReadData Position %d %d %d", (int)(*pCurPos), m_nOpenFinished, voOS_GetSysTime());

	if(m_nOpenFinished == 0)
	{
		VOLOGW("m_nOpenFinished %d", m_nOpenFinished);
		return VOOSMP_ERR_Retry;
	}

	m_nSeekTime = voOS_GetSysTime();
	m_nSeeking = 1;

	int nRC = m_SourceAPI.Seek(m_hSource, (VO_U64 *)pCurPos);

	VOLOGI("SetCurPos finish m_SourceAPI.Seek Position %d 0x%08X m_pCSubtitle 0x%08X %d", (int)(*pCurPos), nRC, m_pCSubtitle, voOS_GetSysTime());

	if(m_pCSubtitle && nRC == VO_RET_SOURCE2_OK)
		m_pCSubtitle->SetCurPos(pCurPos);

	VOLOGI("SetCurPos--- Position %d %d", (int)(*pCurPos), voOS_GetSysTime());
	return ConvertReturnValue(nRC);
}

int COSBaseSource::GetParam (int nID, void * pValue)
{
	if(m_SourceAPI.GetParam == NULL  || m_hSource == NULL)
	{
		VOLOGE("m_SourceAPI.GetParam 0x%08X, m_hSource 0x%08X", m_SourceAPI.GetParam, m_hSource);
		return VOOSMP_ERR_Implement;
	}

	int mID = nID;
	if(nID == VOOSMP_SRC_PID_DVRINFO)
		mID = VO_PID_SOURCE2_SEEKRANGEINFO;

	int nRC = m_SourceAPI.GetParam(m_hSource, mID, pValue);

	VOLOGI("@@@### m_SourceAPI.GetParam() ret Convert");
	return ConvertReturnValue(nRC);
}

int COSBaseSource::SetParam (int nID, void * pValue)
{
	if(nID == VO_PID_SOURCE2_HTTPHEADER)
	{
		if(m_SourceAPI.SetParam == NULL  || m_hSource == NULL)
		{
			VOLOGE("m_SourceAPI.SetParam 0x%08X, m_hSource 0x%08X", m_SourceAPI.SetParam, m_hSource);
			return VOOSMP_ERR_Implement;
		}

		int nRC = m_SourceAPI.SetParam(m_hSource, nID, pValue);

		VOLOGI("@@@### m_SourceAPI.SetParam() ret Convert");
		return ConvertReturnValue(nRC);
	}

	VOLOGI("SetParam+++ ID 0x%08X %d", nID, voOS_GetSysTime());
	voCAutoLock lockReadSrc (&m_MutexReadData);
	VOLOGI("SetParam enter m_MutexReadData ID 0x%08X %d", nID, voOS_GetSysTime());

	// added by gtxia 
	onSetParam(nID, pValue);
	
	if(nID == VOOSMP_PID_COMMON_LOGFUNC)
	{
		m_pbVOLOG = (VO_LOG_PRINT_CB*)pValue;

		if(m_pCSubtitle)
			m_pCSubtitle->SetParam (nID, pValue);

		return VOOSMP_ERR_None;
	}
	else if(nID == VOOSMP_PID_PLAYER_PATH)
	{
		if(pValue)
			vostrcpy(m_szPathLib, (VO_TCHAR *)pValue);

		return VOOSMP_ERR_None;
	}
	else if(nID == VOOSMP_PID_LISTENER)
	{
		m_pSourceListener = (VOOSMP_LISTENERINFO *)pValue;
		return VOOSMP_ERR_None;
	}
	else if(nID == VOOSMP_PID_ONREQUEST_LISTENER)
	{
		m_pSourceOnQest = (VOOSMP_LISTENERINFO *)pValue;
		return VOOSMP_ERR_None;
	}
	else if(nID == VOOSMP_PID_SUBTITLE_FILE_NAME)
	{
		int nRC = VOOSMP_ERR_Pointer;

		if(pValue)
		{
			if(m_pCSubtitle)
			{
				delete m_pCSubtitle;
				m_pCSubtitle = NULL;
			}

			m_pCSubtitle = new COSTimeText(m_pLibop, m_szPathLib);
			if(m_pCSubtitle == NULL)
				return VOOSMP_ERR_OutMemory;

			m_pCSubtitle->Init(0, 0, 0);
			m_pCSubtitle->SetParam (ID_SET_JAVA_ENV, (void *)m_nJavaENV);

			nRC = m_pCSubtitle->SetTimedTextFilePath((VO_TCHAR *)pValue, SUBTITLE_PARSER_TYPE_UNKNOWN);
			VOLOGI("SetTimedTextFilePath File %s, return %x", (VO_TCHAR *)pValue, nRC);

			voCAutoLock lockpost (&m_MutexPostEvent);
			postMsgEvent(VOOSMP_SRC_CB_Program_Changed, 0, 0, 0);
		}

		return nRC;
	}
	else if(nID == ID_SET_JAVA_ENV)
	{
		if(m_pCSubtitle)
			m_pCSubtitle->SetParam (ID_SET_JAVA_ENV, pValue);

		m_nJavaENV = (int)pValue;
		
		return VOOSMP_ERR_None;
	}
	else if(nID == VO_PID_SOURCE2_SUBTITLELANGUAGE)
	{
		if(m_pCSubtitle)
		{
			if(m_pCSubtitle->GetSubtitleType() == VOOS_SUBTITLE_CLOSECAPTION)
				return SelectSubLang((char *)pValue);

			if(m_pCSubtitle->GetSubtitleType() == VOOS_SUBTITLE_TIMETEXT && m_pCSubtitle->GetSubTitleNum())
				return SelectSubLang((char *)pValue); 
		}
	}

	if(m_SourceAPI.SetParam == NULL  || m_hSource == NULL)
	{
		VOLOGE("m_SourceAPI.SetParam 0x%08X, m_hSource 0x%08X", m_SourceAPI.SetParam, m_hSource);
		return VOOSMP_ERR_Implement;
	}

	int nRC = m_SourceAPI.SetParam(m_hSource, nID, pValue);

	VOLOGI("SetParam--- ID 0x%08X 0x%08X %d", nID, nRC, voOS_GetSysTime());
	return ConvertReturnValue(nRC);
}

int COSBaseSource::onNotifyEvent (int nMsg, int nV1, int nV2)
{
	VOLOGI("onNotifyEvent nMsg %x, nV1 %d, nV2 %d %d", nMsg, nV1, nV2, voOS_GetSysTime());

	if(nMsg == VOOSMP_SRC_CB_Program_Changed || nMsg == VOOSMP_SRC_CB_Program_Reset || nMsg == VOOSMP_SRC_CB_Open_Finished)
	{
		VOLOGI("onNotifyEvent+++ %d", voOS_GetSysTime());
		voCAutoLock lockReadSrc (&m_MutexReadData);
		VOLOGI("onNotifyEvent enter m_MutexReadData %d", voOS_GetSysTime());
		InitSrcParam();
	}

	VOLOGI("onNotifyEvent+++ %d", voOS_GetSysTime());
	voCAutoLock	lockListen (&m_MutexSendEvent);
	VOLOGI("onNotifyEvent enter m_MutexSendEvent %d", voOS_GetSysTime());

	int nRC = VOOSMP_ERR_Implement;
	if(m_pSourceListener)
		nRC = m_pSourceListener->pListener(m_pSourceListener->pUserData, nMsg, &nV1, &nV2);

	VOLOGI("onNotifyEvent--- 0x%08X %d", nRC, voOS_GetSysTime());
	return nRC;
}

int COSBaseSource::callBackEvent ()
{
	int nRC = 0;	
	if(m_nSyncMsg == 0)
	{
		voCAutoLock lockpost (&m_MutexPostEvent);
		nRC = postMsgEvent(m_nEventID, m_nParam1, m_nParam2, m_nDelayTime);
		if(nRC == 0)
			return nRC;
	}

	if(m_pSourceListener)
	{
		VOLOGI("callBackEvent+++ %d", voOS_GetSysTime());
		voCAutoLock	lockListen (&m_MutexSendEvent);
		VOLOGI("callBackEvent enter m_MutexSendEvent %d", voOS_GetSysTime());

#ifdef _WIN32
		if (m_nParam1 == VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_BEGINDOWNLOAD ||
			m_nParam1 == VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DROPPED || 
			m_nParam1 == VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DOWNLOADOK )
		{
			nRC = m_pSourceListener->pListener(m_pSourceListener->pUserData, m_nEventID, &m_nParam1, (void*)m_nParam2);
		}
		else
		{
			nRC = m_pSourceListener->pListener(m_pSourceListener->pUserData, m_nEventID, &m_nParam1, &m_nParam2);
		}
#else
		nRC = m_pSourceListener->pListener(m_pSourceListener->pUserData, m_nEventID, &m_nParam1, &m_nParam2);
#endif
		VOLOGI("callBackEvent--- 0x%08X %d", nRC, voOS_GetSysTime());
	}
	else
		nRC = VOOSMP_ERR_Implement;

	return nRC;
}

int COSBaseSource::postMsgEvent (int nMsg, int nV1, int nV2, int nDelayTime)
{
	if (m_pMsgThread == NULL)
		return -1;

	voCBaseEventItem * pEvent = m_pMsgThread->GetEventByType (VOOS_EVENT_MESSAGE);
	if (pEvent == NULL)
		pEvent = new voCBaseSourceEvent (this, &COSBaseSource::onNotifyEvent, VOOS_EVENT_MESSAGE, nMsg, nV1, nV2);
	else
		pEvent->SetEventMsg (nMsg, nV1, nV2);

	m_pMsgThread->PostEventWithDelayTime (pEvent, nDelayTime);

	return 0;
}

VO_S32 COSBaseSource::vomtEventCallBack (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
	int nRC = 0;
	COSBaseSource *pSource = (COSBaseSource *)pUserData;
	
	if(pSource == NULL)
		return VOOSMP_ERR_Implement;
	VOLOGI("vomtEventCallBack+++ %d", voOS_GetSysTime());
	voCAutoLock lockEventCallBack (&pSource->m_MutexEvent);
	VOLOGI("vomtEventCallBack enter m_MutexEvent %d", voOS_GetSysTime());
	pSource->m_nDelayTime = -1;
	pSource->m_nSyncMsg = 0;
	nRC = pSource->HandleEvent(nID, nParam1, nParam2);
	VOLOGI("vomtEventCallBack--- %d,return is 0x%08x", voOS_GetSysTime(),nRC);
	return nRC;
}

int COSBaseSource::HandleEvent (int nID, int nParam1, int nParam2)
{
	if((unsigned int)nID == VO_EVENTID_SOURCE2_MEDIATYPECHANGE)
	{
		m_nEventID = VOOSMP_CB_MediaTypeChanged;

		if(nParam1 == VO_SOURCE2_MT_PUREAUDIO)
		{
			m_nParam1 = VOOSMP_AVAILABLE_PUREAUDIO;
		}
		else if(nParam1 == VO_SOURCE2_MT_PUREVIDEO)
		{
			m_nParam1 = VOOSMP_AVAILABLE_PUREVIDEO;
		}
		else if(nParam1 == VO_SOURCE2_MT_AUDIOVIDEO)
		{
			m_nParam1 = VOOSMP_AVAILABLE_AUDIOVIDEO;
		}

		m_nParam2 =	nParam2;
	}
	else if((unsigned int)nID == VO_EVENTID_SOURCE2_SEEKCOMPLETE)
	{
		m_nEventID = VOOSMP_SRC_CB_Seek_Complete;
		if(nParam1)
			m_nParam1 = (int)(*((VO_U64*)nParam1));
		else
			m_nParam1 = 0;

		if(nParam2)
			m_nParam2 =	(int)(*((VO_S32*)nParam2));
		else 
			m_nParam2 = 0;
	}
	else if((unsigned int)nID == VO_EVENTID_SOURCE2_OPENCOMPLETE)
	{
		m_nEventID = VOOSMP_SRC_CB_Open_Finished;
		if(nParam1)
		{
			int nTempParam =  (int)(*((VO_S32*)nParam1));
			if ( (nTempParam & VO_ERR_LICENSE_ERROR) == VO_ERR_LICENSE_ERROR )
			{
				m_nParam1 = VOOSMP_CB_LicenseFailed; 
			}
			else
				m_nParam1 = (int)(*((VO_S32*)nParam1));
		}
		else
			m_nParam1 = 0;

		m_nParam2 =	nParam2;

		m_nOpenFinished = 1;

		m_nDelayTime = 50;
	}
	else if((unsigned int)nID == VO_EVENTID_SOURCE2_ERR_RTSP_DESCRIBE_FAIL)
	{
		m_nEventID = VOOSMP_SRC_CB_Connection_Rejected;
		m_nParam1 = nParam1;
		m_nParam2 =	nParam2;
	}
	else if((unsigned int)nID == VO_EVENTID_SOURCE2_ERR_CONNECTFAIL)
	{
		m_nEventID = VOOSMP_SRC_CB_Connection_Fail;
		m_nParam1 = nParam1;
		m_nParam2 =	nParam2;
	}
	else if((unsigned int)nID == VO_EVENTID_SOURCE2_ERR_DOWNLOADFAIL)
	{
		m_nEventID = VOOSMP_SRC_CB_Download_Fail;
		m_nParam1 = nParam1;
		m_nParam2 =	nParam2;
	}
	else if((unsigned int)nID == VO_EVENTID_SOURCE2_ERR_DRMFAIL)
	{
		m_nEventID = VOOSMP_SRC_CB_DRM_Fail;
		m_nParam1 = nParam1;
		m_nParam2 =	nParam2;
	}
	else if((unsigned int)nID == VO_EVENTID_SOURCE2_ERR_PLAYLISTPARSEERR)
	{
		m_nEventID = VOOSMP_SRC_CB_Playlist_Parse_Err;
		m_nParam1 = nParam1;
		m_nParam2 =	nParam2;
	}
	else if((unsigned int)nID == VO_EVENTID_SOURCE2_HLS_BITRATE_NOTIFY)
	{
		m_nEventID = VOOSMP_SRC_CB_BA_Happened;
		m_nParam1 = nParam1;
		m_nParam2 =	nParam2;
	}
	else if((unsigned int)nID == VO_EVENTID_SOURCE2_START_DOWNLOAD_FAIL_WAITING_RECOVER)
	{
		m_nEventID = VOOSMP_SRC_CB_Download_Fail_Waiting_Recover;
		m_nParam1 = nParam1;
		m_nParam2 =	nParam2;
	}
	else if((unsigned int)nID == VO_EVENTID_SOURCE2_DOWNLOAD_FAIL_RECOVER_SUCCESS)
	{
		m_nEventID = VOOSMP_SRC_CB_Download_Fail_Recover_Success;
		m_nParam1 = nParam1;
		m_nParam2 =	nParam2;
	}
	else if((unsigned int)nID == VO_EVENTID_SOURCE2_CUSTOMTAG)
	{
		m_nEventID = VOOSMP_SRC_CB_Customer_Tag;
		
		if(nParam1 == VO_SOURCE2_CUSTOMERTAGID_TIMEDTAG)
		{
			m_nParam1 = VOOSMP_SRC_CUSTOMERTAGID_TIMEDTAG;
		}
		else
		{
			m_nParam1 = nParam1;
		}

		m_nParam2 =	nParam2;
		m_nSyncMsg = 1;
	}
	else if((unsigned int)nID == VO_EVENTID_SOURCE2_PROGRAMCHANGED)
	{
		m_nEventID = VOOSMP_SRC_CB_Program_Changed;
		
		m_nParam1 = nParam1;
		m_nParam2 =	nParam2;
	}
	else if((unsigned int)nID == VO_EVENTID_SOURCE2_PROGRAMRESET)
	{
		m_nEventID = VOOSMP_SRC_CB_Program_Reset;
		
		m_nParam1 = nParam1;
		m_nParam2 =	nParam2;
	}
	else if((unsigned int)nID == VO_EVENTID_SOURCE2_SEEK2LASTCHUNK)
	{
		m_nEventID = VOOSMP_SRC_CB_Adaptive_Stream_SEEK2LASTCHUNK;

		m_nParam1 = nParam1;
		m_nParam2 =	nParam2;
	}
	else if((unsigned int)nID == VO_EVENT_SOCKET_ERR)
	{
		m_nEventID = VOOSMP_SRC_CB_Connection_Fail;

		m_nParam1 = nParam1;
		m_nParam2 =	nParam2;
	}
	else if((unsigned int)nID == VO_PD_EVENT_NOT_APPLICABLE_MEDIA)
	{
		m_nEventID = VOOSMP_SRC_CB_Not_Applicable_Media;

		m_nParam1 = nParam1;
		m_nParam2 =	nParam2;
	}
	else if((unsigned int)nID == VO_PD_EVENT_DOWNLOADPOS)
	{
		m_nEventID = VOOSMP_SRC_CB_PD_DOWNLOAD_POSITION;
		m_nParam1 = (int)(*((VO_S32*)nParam1));
		m_nParam2 =	nParam2;
		VOLOGI("@@@###  recived VO_PD_EVENT_DOWNLOADPOS, nParam1 = %d, nParam2 = %d", m_nParam1, nParam2);

	}
	else if((unsigned int)nID == VO_EVENT_BUFFERING_PERCENT)
	{
		m_nEventID = VOOSMP_SRC_CB_PD_BUFFERING_PERCENT;
		m_nParam1 = (int)(*((VO_S32*)nParam1));
		m_nParam2 =	nParam2;
		VOLOGI("@@@###  recived VO_EVENT_BUFFERING_PERCENT, nParam1 = %d, nParam2 = %d", m_nParam1, nParam2);
	}
	else if ((unsigned int)nID == VO_EVENT_BUFFERING_BEGIN)
	{
		VOLOGI("@@@@### PD Start Bufferring....");
		m_nEventID = VOOSMP_CB_VideoStartBuff;
		m_nParam1 = nParam1;
		m_nParam2 =	nParam2;
	}
	else if ((unsigned int)nID == VO_EVENT_BUFFERING_END )
	{
		VOLOGI("@@@### PD Stop Buffering....");
		m_nEventID = VOOSMP_CB_VideoStopBuff;
		m_nParam1 = nParam1;
		m_nParam2 =	nParam2;
	}
	else if ((unsigned int)nID == VO_EVENTID_SOURCE2_UPDATEURLCOMPLETE )
	{
		m_nEventID = VOOSMP_SRC_CB_UPDATE_URL_COMPLETE;
		if (nParam1 == 0)
			m_nParam1 = 0;
		else if ((unsigned int)nParam1 == VO_EVENTID_SOURCE2_ERR_DOWNLOADFAIL)
			m_nParam1 = VOOSMP_SRC_CB_Download_Fail;
		else if ((unsigned int)nParam1 == VO_EVENTID_SOURCE2_ERR_DRMFAIL)
			m_nParam1 =	VOOSMP_SRC_CB_DRM_Fail;
		else if ((unsigned int)nParam1 == VO_EVENTID_SOURCE2_ERR_PLAYLISTPARSEERR)
			m_nParam1 = VOOSMP_SRC_CB_Playlist_Parse_Err;
		else
		{
			VOLOGW("@@@### Low layer callback set Unknow nParamID: 0x%08X", nParam1);
			m_nParam1 = nParam1;
			m_nParam2 =	nParam2;
		}
		m_nParam2 =	nParam2;
		VOLOGI("@@@### Update URL Complite event send to OSSource....nParam1 = 0x%08X, nParam2 = 0x%08X", nParam1, nParam2);
	}
	else 
	{
		return VOOSMP_ERR_Implement; 
	}

	return VOOSMP_ERR_None;
}

VO_S32 COSBaseSource::vomtSendCallBack (VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData)
{
	COSBaseSource *pSource = (COSBaseSource *)pUserData;

	if(pSource == NULL)
		return VOOSMP_ERR_Implement;

	return pSource->doSendData(nOutputType, pData);
}

int COSBaseSource::doSendData(unsigned int nOutputType, void* pData)
{
	if(m_pSendBufFunc == NULL)
		return VOOSMP_ERR_Implement;

	if(pData == NULL)
		return  VOOSMP_ERR_Pointer;
	
	voCAutoLock lockReadSrc (&m_MutexReadData);
	
	int nRC = VOOSMP_ERR_None;
	VOOSMP_BUFFERTYPE nSample;
	memset(&nSample, 0, sizeof(VOOSMP_BUFFERTYPE));

	if(nOutputType == VO_SOURCE2_TT_VIDEO)
	{
		ConvertBufSrc2OS((VO_SOURCE2_SAMPLE*)pData, &nSample, VOOSMP_SS_VIDEO);
		nRC = m_pSendBufFunc->SendData(m_pSendBufFunc->pUserData, VOOSMP_SS_VIDEO, &nSample);
	}
	else if(nOutputType == VO_SOURCE2_TT_VIDEO)
	{
		ConvertBufSrc2OS((VO_SOURCE2_SAMPLE*)pData, &nSample, VOOSMP_SS_AUDIO);
		nRC = m_pSendBufFunc->SendData(m_pSendBufFunc->pUserData, VOOSMP_SS_AUDIO, &nSample);
	}

	if(nRC == VOOSMP_ERR_Retry)
	{
		return VO_RET_SOURCE2_NEEDRETRY;
	}
	else if(nRC == VOOSMP_ERR_None)
	{
		return VO_RET_SOURCE2_OK;
	}
	else
	{
		return nRC;
	}
}

int COSBaseSource::voSrcReadAudio(void * pUserData, VOOSMP_BUFFERTYPE * pBuffer)
{
	if(pBuffer == NULL)
		return VOOSMP_ERR_Pointer;
	COSBaseSource *pSource = (COSBaseSource *)pUserData;

	if(pSource == NULL)
		return VOOSMP_ERR_Retry;

	int nRC = pSource->doReadAudio(pBuffer);

	pSource->DumpInfo(VOOSMP_SS_AUDIO, nRC, pBuffer);

	return nRC;
}

int COSBaseSource::doReadAudio(VOOSMP_BUFFERTYPE * pBuffer)
{
	if(m_SourceAPI.GetSample == NULL  || m_hSource == NULL || pBuffer == NULL)
		return VOOSMP_ERR_Retry;

	if(pBuffer == NULL)
		return VOOSMP_ERR_Pointer;

	if (m_bBuffering)
	{
		VOLOGI("@@@### PD Audio Still buffering, no Sample to be get return  VOOSMP_ERR_Retry....");
		return VOOSMP_ERR_Retry;
	}

	voCAutoLock lockReadSrc (&m_MutexReadData);

	VO_SOURCE2_SAMPLE nSample;
	memset(&nSample, 0, sizeof(VO_SOURCE2_SAMPLE));
	nSample.uTime = pBuffer->llTime;

	int nRC = m_SourceAPI.GetSample(m_hSource, VO_SOURCE2_TT_AUDIO, &nSample);
	VOLOGI("@@@### m_SourceAPI.GetSample( Audio ) nRC = 0x%08X, nSample.Time = %d, buffer.Flag = 0x%08X", nRC, (int)nSample.uTime, nSample.uFlag);

	if(nRC == VO_RET_SOURCE2_OK)
	{
		return	ConvertBufSrc2OS(&nSample, pBuffer, VOOSMP_SS_AUDIO);	
	}
	else if(nRC == (int)VO_RET_SOURCE2_NEEDRETRY)
	{
		return VOOSMP_ERR_Retry;
	}
	else if(nRC == (int)VO_RET_SOURCE2_END)
	{
		return VOOSMP_ERR_EOS;
	}
	else
	{
		return VOOSMP_ERR_Retry;
	}
}

int COSBaseSource::voSrcReadVideo(void * pUserData, VOOSMP_BUFFERTYPE * pBuffer)
{
	if(pBuffer == NULL)
		return VOOSMP_ERR_Pointer;
	COSBaseSource *pSource = (COSBaseSource *)pUserData;

	if(pSource == NULL)
		return VOOSMP_ERR_Retry;

	int nRC = pSource->doReadVideo(pBuffer);

	pSource->DumpInfo(VOOSMP_SS_VIDEO, nRC, pBuffer);

	return nRC;
}

int COSBaseSource::doReadVideo(VOOSMP_BUFFERTYPE * pBuffer)
{
	if(m_SourceAPI.GetSample == NULL || m_hSource == NULL || pBuffer == NULL)
		return VOOSMP_ERR_Retry;

	if(pBuffer == NULL)
		return VOOSMP_ERR_Pointer;

	if (m_bBuffering)
	{
		VOLOGI("@@@### PD Video Still buffering, no Sample to be get return  VOOSMP_ERR_Retry....");
		return VOOSMP_ERR_Retry;
	}

	voCAutoLock lockReadSrc (&m_MutexReadData);
	
	VO_SOURCE2_SAMPLE nSample;
	memset(&nSample, 0, sizeof(VO_SOURCE2_SAMPLE));
	nSample.uTime = pBuffer->llTime;
	nSample.uDuration = pBuffer->nDuration;

	if(m_nLogVideoLevel&2)
	{
		VOLOGI("GetVideo Sample enter TimeStamp %d, Duration %d", (int)pBuffer->llTime, pBuffer->nDuration);
	}

	int nRC = m_SourceAPI.GetSample(m_hSource, VO_SOURCE2_TT_VIDEO, &nSample);
	VOLOGI("@@@### m_SourceAPI.GetSample( Video ) nRC = 0x%08X, nSample.Time = %d, buffer.Flag = 0x%08X", nRC, (int)nSample.uTime, nSample.uFlag);
	if(nRC == VO_RET_SOURCE2_OK)
	{
		if(m_nSeeking)
		{
			m_nSeeking = 0;
			VOLOGI("Seeking use time %d", (int)(voOS_GetSysTime() - m_nSeekTime));
		}

		nRC = ConvertBufSrc2OS(&nSample, pBuffer, VOOSMP_SS_VIDEO);	

		HandleCC(pBuffer, nRC);

		return nRC;
	}
	else if(nRC == (int)VO_RET_SOURCE2_NEEDRETRY)
	{
		return VOOSMP_ERR_Retry;
	}
	else if(nRC == (int)VO_RET_SOURCE2_END)
	{
		HandleCC(pBuffer, VOOSMP_ERR_EOS);
		return VOOSMP_ERR_EOS;
	}
	else
	{
		return VOOSMP_ERR_Retry;
	}
}

int COSBaseSource::HandleCC(VOOSMP_BUFFERTYPE* pBuffer, int nRC)
{
	if(pBuffer == NULL)
		return VOOSMP_ERR_None;

	voCAutoLock lockProgramInfo (&m_MutexProgramInfo);
	if(m_pCSubtitle && m_pCSubtitle->GetSubtitleType() != VOOS_SUBTITLE_CLOSECAPTION)
	{
		return VOOSMP_ERR_None;
	}
	if(pBuffer == NULL)
		return VOOSMP_ERR_Pointer;

	if((pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_PROGRAM)||(pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_FORMAT))
	{
		VOOSMP_BUFFER_FORMAT *pFormat = (VOOSMP_BUFFER_FORMAT *)pBuffer->pData;
		if(pFormat == NULL)
			return VOOSMP_ERR_None;
		
		if(pFormat->nCodec != VOOSMP_VIDEO_CodingH264)
		{
			return VOOSMP_ERR_None;
		}

		if(m_pCSubtitle == NULL)
		{
			m_pCSubtitle = new COSCloseCaption(m_pLibop, m_szPathLib);
			if(m_pCSubtitle == NULL)
				return VOOSMP_ERR_OutMemory;
			if(pFormat && pFormat->nHeadDataLen > 0)
				m_pCSubtitle->Init((unsigned char *)pFormat->pHeadData, pFormat->nHeadDataLen, 0);
			else
				m_pCSubtitle->Init(pBuffer->pBuffer, pBuffer->nSize, 0);

			if (m_sCCLanguageDescription.pData && m_sCCLanguageDescription.uDataLen > 0)
			{
				VOLOGI("@@@### Set CC Description: uDataLen = %d", m_sCCLanguageDescription.uDataLen);
				m_pCSubtitle->SetParam(VO_PARAMETER_ID_708LANGUAGEDes, &m_sCCLanguageDescription);
			}
		}
		else
		{
			int nFlag = VOCC_FLAG_NEW_STREAM;

			if(pFormat && pFormat->nHeadDataLen > 0)
				m_pCSubtitle->SetInputData((VO_CHAR *)pFormat->pHeadData, pFormat->nHeadDataLen, (int)pBuffer->llTime, nFlag);
			else
				m_pCSubtitle->SetInputData((VO_CHAR *)pBuffer->pBuffer, pBuffer->nSize, (int)pBuffer->llTime, nFlag);
		}
	}

	if(m_pCSubtitle == NULL)
	{
		return VOOSMP_ERR_Retry;
	}

	int nFlag = 0;
	if(nRC == VOOSMP_ERR_EOS)
		nFlag |= VOCC_FLAG_STREAM_EOS;

	if(pBuffer->pBuffer != NULL && pBuffer->nSize > 0)
		m_pCSubtitle->SetInputData((VO_CHAR *)pBuffer->pBuffer, pBuffer->nSize, (int)pBuffer->llTime, nFlag);

	if(m_nSubtitleTrackNum != m_pCSubtitle->GetTrackCount())
	{
		voCAutoLock lockpost (&m_MutexPostEvent);
		postMsgEvent(VOOSMP_SRC_CB_Program_Changed, (int)pBuffer->llTime, 0, 0);
		m_nSubtitleTrackNum = m_pCSubtitle->GetTrackCount();
	}

	return 0;
}

int COSBaseSource::ConvertBufSrc2OS(VO_SOURCE2_SAMPLE* pSampleBuf, VOOSMP_BUFFERTYPE* pBuffer, int ssType)
{
	if(pBuffer == NULL || pSampleBuf == NULL)
		return VOOSMP_ERR_Pointer;

	pBuffer->pBuffer = pSampleBuf->pBuffer;
	pBuffer->nSize	 = pSampleBuf->uSize;
	pBuffer->llTime	 = pSampleBuf->uTime;
	pBuffer->llReserve = ((long long)pSampleBuf->pReserve2) << 32;
	pBuffer->llReserve |= ((long long)pSampleBuf->pReserve1);
	pBuffer->nFlag = 0;

	if(pSampleBuf->uFlag&VO_SOURCE2_FLAG_SAMPLE_KEYFRAME)
	{
		pBuffer->nFlag |= VOOSMP_FLAG_BUFFER_KEYFRAME;
	}

	if(pSampleBuf->uFlag&VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT)
	{
		if(pSampleBuf->uFlag&VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET)
			pBuffer->nFlag |= VOOSMP_FLAG_BUFFER_NEW_PROGRAM;
		else
			pBuffer->nFlag |= VOOSMP_FLAG_BUFFER_NEW_FORMAT;

		if((pSampleBuf->uFlag & VO_SOURCE2_FLAG_SAMPLE_DRMHEADER) == VO_SOURCE2_FLAG_SAMPLE_DRMHEADER)
		{
			//VOLOGE("HUI_FA  pSampleBuf->uFlag & VO_SOURCE2_FLAG_SAMPLE_DRMHEADER\n");
			pBuffer->nFlag |= VOOSMP_FLAG_BUFFER_ADD_DRMDATA;
		}

		if(pSampleBuf->pFlagData)
		{
			VO_SOURCE2_TRACK_INFO* pTrackInfo = (VO_SOURCE2_TRACK_INFO *)pSampleBuf->pFlagData;

			if(ssType == VOOSMP_SS_AUDIO)
			{
				memset(&m_sAudioFormat, 0, sizeof(VOOSMP_BUFFER_FORMAT));
				m_nAudioCodec = ConvertAudioCodecSrc2OS(pTrackInfo->uCodec);
				m_sAudioFormat.nStreamtype = VOOSMP_SS_AUDIO;
				m_sAudioFormat.nCodec = m_nAudioCodec;
				m_sAudioFormat.nFourCC = *((int *)pTrackInfo->strFourCC);
				
				m_sAudioFormat.sFormat.audio.SampleRate = pTrackInfo->sAudioInfo.sFormat.SampleRate;
				m_sAudioFormat.sFormat.audio.Channels = pTrackInfo->sAudioInfo.sFormat.Channels;
				m_sAudioFormat.sFormat.audio.SampleBits = pTrackInfo->sAudioInfo.sFormat.SampleBits;

				m_sAudioFormat.pHeadData = (char *)pTrackInfo->pHeadData;
				m_sAudioFormat.nHeadDataLen	 = pTrackInfo->uHeadSize;

				pBuffer->pData = &m_sAudioFormat;
			}
			else if(ssType == VOOSMP_SS_VIDEO)
			{
				memset(&m_sVideoFormat, 0, sizeof(VOOSMP_BUFFER_FORMAT));
				m_nVideoCodec = ConvertVideoCodecSrc2OS(pTrackInfo->uCodec);
				m_sVideoFormat.nStreamtype = VOOSMP_SS_VIDEO;
				m_sVideoFormat.nCodec = m_nVideoCodec;
				m_sVideoFormat.nFourCC = *((int *)pTrackInfo->strFourCC);
				
				m_sVideoFormat.sFormat.video.Width = pTrackInfo->sVideoInfo.sFormat.Width;
				m_sVideoFormat.sFormat.video.Height = pTrackInfo->sVideoInfo.sFormat.Height;
				m_sVideoFormat.sFormat.video.Type = pTrackInfo->sVideoInfo.sFormat.Type;
				m_sVideoFormat.pHeadData = (char *)pTrackInfo->pHeadData;
				m_sVideoFormat.nHeadDataLen	 = pTrackInfo->uHeadSize;

				m_sCCLanguageDescription.uDataLen = pTrackInfo->nVideoClosedCaptionDescDataLen;
				m_sCCLanguageDescription.pData = pTrackInfo->pVideoClosedCaptionDescData;
				VOLOGI("@@@### CC-Descripiton data length = %d", m_sCCLanguageDescription.uDataLen);
				if(m_pCSubtitle && m_pCSubtitle->GetSubtitleType() == VOOS_SUBTITLE_CLOSECAPTION)
				{
					VOLOGI("@@@### Set CC Description");
					m_pCSubtitle->SetParam(VO_PARAMETER_ID_708LANGUAGEDes, &m_sCCLanguageDescription);
				}

				if(m_sVideoFormat.nCodec == VOOSMP_VIDEO_CodingVP6)
					m_sVideoFormat.nFourCC = pTrackInfo->sVideoInfo.uAngle;

				pBuffer->pData = &m_sVideoFormat;
			}
		}
		else
		{
			pBuffer->pData = NULL;
		}
	}

	if(pSampleBuf->uFlag&VO_SOURCE2_FLAG_SAMPLE_NEW_PROGRAM)
	{
		pBuffer->nFlag |= VOOSMP_FLAG_BUFFER_NEW_PROGRAM;

		if(pSampleBuf->pFlagData)
		{
			VO_SOURCE2_TRACK_INFO* pTrackInfo = (VO_SOURCE2_TRACK_INFO *)pSampleBuf->pFlagData;
			
			if(ssType == VOOSMP_SS_AUDIO)
			{
				memset(&m_sAudioFormat, 0, sizeof(VOOSMP_BUFFER_FORMAT));
				m_nAudioCodec = ConvertAudioCodecSrc2OS(pTrackInfo->uCodec);
				m_sAudioFormat.nStreamtype = VOOSMP_SS_AUDIO;
				m_sAudioFormat.nCodec = m_nAudioCodec;
				m_sAudioFormat.nFourCC = *((int *)pTrackInfo->strFourCC);
				
				m_sAudioFormat.sFormat.audio.SampleRate = pTrackInfo->sAudioInfo.sFormat.SampleRate;
				m_sAudioFormat.sFormat.audio.Channels = pTrackInfo->sAudioInfo.sFormat.Channels;
				m_sAudioFormat.sFormat.audio.SampleBits = pTrackInfo->sAudioInfo.sFormat.SampleBits;

				m_sAudioFormat.pHeadData = (char *)pTrackInfo->pHeadData;
				m_sAudioFormat.nHeadDataLen	 = pTrackInfo->uHeadSize;

				pBuffer->pData = &m_sAudioFormat;
			}
			else if(ssType == VOOSMP_SS_VIDEO)
			{
				memset(&m_sVideoFormat, 0, sizeof(VOOSMP_BUFFER_FORMAT));
				m_nVideoCodec = ConvertVideoCodecSrc2OS(pTrackInfo->uCodec);
				m_sVideoFormat.nStreamtype = VOOSMP_SS_VIDEO;
				m_sVideoFormat.nCodec = m_nVideoCodec;
				m_sVideoFormat.nFourCC = *((int *)pTrackInfo->strFourCC);
				
				m_sVideoFormat.sFormat.video.Width = pTrackInfo->sVideoInfo.sFormat.Width;
				m_sVideoFormat.sFormat.video.Height = pTrackInfo->sVideoInfo.sFormat.Height;
				m_sVideoFormat.sFormat.video.Type = pTrackInfo->sVideoInfo.sFormat.Type;
				m_sVideoFormat.pHeadData = (char *)pTrackInfo->pHeadData;
				m_sVideoFormat.nHeadDataLen	 = pTrackInfo->uHeadSize;

				m_sCCLanguageDescription.uDataLen = pTrackInfo->nVideoClosedCaptionDescDataLen;
				m_sCCLanguageDescription.pData = pTrackInfo->pVideoClosedCaptionDescData;
				VOLOGI("@@@### CC-Descripiton data length = %d", m_sCCLanguageDescription.uDataLen);
				if(m_pCSubtitle && m_pCSubtitle->GetSubtitleType() == VOOS_SUBTITLE_CLOSECAPTION)
				{
					VOLOGI("@@@### Set CC Description");
					m_pCSubtitle->SetParam(VO_PARAMETER_ID_708LANGUAGEDes, &m_sCCLanguageDescription);
				}

				if(m_sVideoFormat.nCodec == VOOSMP_VIDEO_CodingVP6)
					m_sVideoFormat.nFourCC = pTrackInfo->sVideoInfo.uAngle;

				pBuffer->pData = &m_sVideoFormat;
			}
		}
		else
		{
			pBuffer->pData = NULL;
		}
	}

	if(pSampleBuf->uFlag&VO_SOURCE2_FLAG_SAMPLE_FORCE_FLUSH)
	{
		return VOOSMP_ERR_FLush_Buffer;
	}

	if(pSampleBuf->uFlag&VO_SOURCE2_FLAG_SAMPLE_EOS)
	{
		return VOOSMP_ERR_EOS;
	}

	if(pSampleBuf->uFlag&VO_SOURCE2_FLAG_SAMPLE_FRAMESHOULDDROP)
	{
		pBuffer->nFlag |= VOOSMP_FLAG_BUFFER_DELAY_TO_DROP;
	}

	if(pSampleBuf->uFlag&VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET)
	{
		if(!(pSampleBuf->uFlag&VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT) && !(pSampleBuf->uFlag&VO_SOURCE2_FLAG_SAMPLE_NEW_PROGRAM))
			pBuffer->nFlag |= VOOSMP_FLAG_BUFFER_TIMESTAMP_RESET;
	}

	if(pSampleBuf->uFlag&VO_SOURCE2_FLAG_SAMPLE_FRAMEDROPPED)
	{
		pBuffer->nFlag |= VOOSMP_FLAG_BUFFER_DROP_FRAME;
	}

	if(pSampleBuf->uFlag&VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE)
	{
		if(ssType == VOOSMP_SS_AUDIO)
			return VOOSMP_ERR_Audio_No_Now;
		else if(ssType == VOOSMP_SS_VIDEO)
			return VOOSMP_ERR_Video_No_Now;
	}

	if(pSampleBuf->uFlag&VO_SOURCE2_FLAG_SAMPLE_FRAMETYPEUNKNOWN)
	{
		pBuffer->nFlag |= VOOSMP_FLAG_BUFFER_UNKNOWN;
	}

	if(pSampleBuf->uFlag&VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE)
	{
		memset(&m_sSubtitleFormat, 0, sizeof(VOOSMP_BUFFER_FORMAT));
		m_sSubtitleFormat.nStreamtype = VOOSMP_SS_SUBTITLE;
		m_sSubtitleFormat.nFourCC = VOOSMP_SS_IMAGE;

		m_sSubtitleFormat.pHeadData = (char *)pSampleBuf->pBuffer;
		m_sSubtitleFormat.nHeadDataLen = pSampleBuf->uSize;

		pBuffer->pData = &m_sSubtitleFormat;

		//VOLOGI("Set Image subtitle %x, Duration %d", (int)pBuffer->pData, pBuffer->nDuration);
		
		pBuffer->nFlag |= VOOSMP_FLAG_BUFFER_NEW_PROGRAM;
	}
	else if (pSampleBuf->uFlag & VO_SOURCE2_FLAG_SAMPLE_FRAMEDECODEONLY)
	{
		pBuffer->nFlag |= VOOSMP_FLAG_BUFFER_FRAME_DECODE_ONLY;
	}

	return VOOSMP_ERR_None;
}

int COSBaseSource::ConvertAudioCodecSrc2OS(int nCodec)
{
	int dCodec = VOOSMP_AUDIO_CodingUnused;

	switch(nCodec)
	{
	case VO_AUDIO_CodingPCM:
		dCodec = VOOSMP_AUDIO_CodingPCM;
		break;
	case VO_AUDIO_CodingADPCM:
		dCodec = VOOSMP_AUDIO_CodingADPCM;
		break;
	case VO_AUDIO_CodingAMRNB:
		dCodec = VOOSMP_AUDIO_CodingAMRNB;
		break;
	case VO_AUDIO_CodingAMRWB:
		dCodec = VOOSMP_AUDIO_CodingAMRWB;
		break;
	case VO_AUDIO_CodingAMRWBP:
		dCodec = VOOSMP_AUDIO_CodingAMRWBP;
		break;
	case VO_AUDIO_CodingQCELP13:
		dCodec = VOOSMP_AUDIO_CodingQCELP13;
		break;
	case VO_AUDIO_CodingEVRC:
		dCodec = VOOSMP_AUDIO_CodingEVRC;
		break;
	case VO_AUDIO_CodingAAC:
		dCodec = VOOSMP_AUDIO_CodingAAC;
		break;
	case VO_AUDIO_CodingAC3:
		dCodec = VOOSMP_AUDIO_CodingAC3;
		break;
	case VO_AUDIO_CodingFLAC:
		dCodec = VOOSMP_AUDIO_CodingFLAC;
		break;
	case VO_AUDIO_CodingMP1:
		dCodec = VOOSMP_AUDIO_CodingMP1;
		break;
	case VO_AUDIO_CodingMP3:
		dCodec = VOOSMP_AUDIO_CodingMP3;
		break;
	case VO_AUDIO_CodingOGG:
		dCodec = VOOSMP_AUDIO_CodingOGG;
		break;
	case VO_AUDIO_CodingWMA:
		dCodec = VOOSMP_AUDIO_CodingWMA;
		break;
	case VO_AUDIO_CodingRA:
		dCodec = VOOSMP_AUDIO_CodingRA;
		break;
	case VO_AUDIO_CodingMIDI:
		dCodec = VOOSMP_AUDIO_CodingMIDI;
		break;
	case VO_AUDIO_CodingDRA:
		dCodec = VOOSMP_AUDIO_CodingDRA;
		break;
	case VO_AUDIO_CodingG729:
		dCodec = VOOSMP_AUDIO_CodingG729;
		break;
	case VO_AUDIO_CodingEAC3:
		dCodec = VOOSMP_AUDIO_CodingEAC3;
		break;
	case VO_AUDIO_CodingAPE:
		dCodec = VOOSMP_AUDIO_CodingAPE;
		break;
	case VO_AUDIO_CodingALAC:
		dCodec = VOOSMP_AUDIO_CodingALAC;
		break;
	case VO_AUDIO_CodingDTS:
		dCodec = VOOSMP_AUDIO_CodingDTS;
		break;
	}

	return dCodec;
}

int COSBaseSource::ConvertVideoCodecSrc2OS(int nCodec)
{
	int dCodec = VOOSMP_VIDEO_CodingUnused;

	switch(nCodec)
	{
   case VO_VIDEO_CodingMPEG2:
	   dCodec = VOOSMP_VIDEO_CodingMPEG2;
	   break;
   case VO_VIDEO_CodingH263:
		dCodec = VOOSMP_VIDEO_CodingH263;
	   break;
   case VO_VIDEO_CodingS263:
		dCodec = VOOSMP_VIDEO_CodingS263;
	   break;
   case VO_VIDEO_CodingMPEG4:
		dCodec = VOOSMP_VIDEO_CodingMPEG4;
	   break;
   case VO_VIDEO_CodingH264:
	   dCodec = VOOSMP_VIDEO_CodingH264;
	   break;
   case VO_VIDEO_CodingH265:
	   dCodec = VOOSMP_VIDEO_CodingH265;
	   break;
   case VO_VIDEO_CodingWMV:
		dCodec = VOOSMP_VIDEO_CodingWMV;
	   break;
   case VO_VIDEO_CodingRV:
		dCodec = VOOSMP_VIDEO_CodingRV;
	   break;
   case VO_VIDEO_CodingMJPEG:
		dCodec = VOOSMP_VIDEO_CodingMJPEG;
	   break;
   case VO_VIDEO_CodingDIVX:
		dCodec = VOOSMP_VIDEO_CodingDIVX;
	   break;
   case VO_VIDEO_CodingVP6:
		dCodec = VOOSMP_VIDEO_CodingVP6;
	   break;
   case VO_VIDEO_CodingVP8:
		dCodec = VOOSMP_VIDEO_CodingVP8;
	   break;
   case VO_VIDEO_CodingVP7:
		dCodec = VOOSMP_VIDEO_CodingVP7;
	   break;
   case VO_VIDEO_CodingVC1:
		dCodec = VOOSMP_VIDEO_CodingVC1;
	   break;
	}

	return dCodec;
}

unsigned int COSBaseSource::ConvertReturnValue(unsigned int nRC)
{
	unsigned int osRC = nRC;

	switch(nRC)
	{
	case VO_RET_SOURCE2_OK:
		osRC = VOOSMP_ERR_None;
		break;
	case VO_RET_SOURCE2_FAIL:
		osRC = VOOSMP_ERR_Unknown;
		break;
	case VO_RET_SOURCE2_OPENFAIL:
		osRC = VOOSMP_SRC_ERR_OPEN_SRC_FAIL;
		break;
	case VO_RET_SOURCE2_NEEDRETRY:
		osRC = VOOSMP_ERR_Retry;
		break;
	case VO_RET_SOURCE2_END:
		osRC = VOOSMP_ERR_EOS;
		break;
	case VO_RET_SOURCE2_CONTENTENCRYPT:
		osRC = VOOSMP_SRC_ERR_CONTENT_ENCRYPT;
		break;
	case VO_RET_SOURCE2_CODECUNSUPPORT:
		osRC = VOOSMP_SRC_ERR_FORMAT_UNSUPPORT;
		break;
	case VO_RET_SOURCE2_PLAYMODEUNSUPPORT:
		osRC = VOOSMP_SRC_ERR_PLAYMODE_UNSUPPORT;
		break;
	case VO_RET_SOURCE2_ERRORDATA:
		osRC = VOOSMP_SRC_ERR_ERROR_DATA;
		break;
	case VO_RET_SOURCE2_SEEKFAIL:
		osRC = VOOSMP_SRC_ERR_SEEK_FAIL;
		break;
	case VO_RET_SOURCE2_FORMATUNSUPPORT:
		osRC = VOOSMP_SRC_ERR_FORMAT_UNSUPPORT;
		break;
	case VO_RET_SOURCE2_TRACKNOTFOUND:
		osRC = VOOSMP_SRC_ERR_TRACK_NOTFOUND;
		break;
	case VO_RET_SOURCE2_EMPTYPOINTOR:
		osRC = VOOSMP_ERR_Pointer;
		break;
	case VO_RET_SOURCE2_NOIMPLEMENT:
		osRC = VOOSMP_ERR_Implement;
		break;
	case VO_RET_SOURCE2_NODOWNLOADOP:
		osRC = VOOSMP_SRC_ERR_NO_DOWNLOAD_OP;
		break;
	case VO_RET_SOURCE2_INPUTDATASMALL:
		osRC = VOOSMP_ERR_SmallSize;
		break;
	case VO_RET_SOURCE2_OUTPUTNOTAVALIBLE:
		osRC = VOOSMP_SRC_ERR_FORMAT_UNSUPPORT;
		break;
	case VO_RET_SOURCE2_CHUNKSKIPPED:
		osRC = VOOSMP_SRC_ERR_CHUNK_SKIP;
		break;
	case VO_RET_SOURCE2_DRMERROR:
		osRC = VOOSMP_ERR_Unknown;
		break;
	case VO_RET_SOURCE2_NOLIBOP:
		osRC = VOOSMP_SRC_ERR_CONTENT_ENCRYPT;
		break;
	case VO_RET_SOURCE2_OUTPUTNOTFOUND:
		osRC = VOOSMP_SRC_ERR_OUTPUT_NOTFOUND;
		break;
	case VO_RET_SOURCE2_ONECHUNKFAIL:
		osRC =  VOOSMP_SRC_ERR_CHUNK_SKIP;
		break;
	case VO_RET_SOURCE2_LINKFAIL:
		osRC = VOOSMP_ERR_Unknown;
		break;
	case VO_RET_SOURCE2_CHUNKDROPPED:
		osRC = VOOSMP_SRC_ERR_CHUNK_SKIP;
		break;
	case VO_RET_SOURCE2_OUTPUTDATASMALL:
		osRC = VOOSMP_ERR_SmallSize;
		break;
	case VO_RET_SOURCE2_CHUNKPARTERROR:
		osRC = VOOSMP_SRC_ERR_CHUNK_SKIP;
		break;
	case VO_RET_SOURCE2_INVALIDPARAM:
		osRC = VOOSMP_ERR_ParamID;
		break;
	case VO_RET_SOURCE2_SEEKMODEUNSUPPORTED:
		osRC = VOOSMP_SRC_ERR_SEEK_FAIL;
		break;
	case VO_RET_SOURCE2_ONELINKFAIL:
		osRC = VOOSMP_SRC_ERR_OPEN_SRC_FAIL;
		break;
	case VO_RET_SOURCE2_RESOLUTIONFORBIDDEN:
		osRC = VOOSMP_SRC_ERR_FORMAT_UNSUPPORT;
		break;
	case VO_RET_SOURCE2_ADAPTIVESTREAMING_CHUNK_SKIP:
		osRC = VOOSMP_SRC_ERR_CHUNK_SKIP;
		break;
	case VO_RET_SOURCE2_ADAPTIVESTREAMING_FORCETIMESTAMP:
		osRC = VOOSMP_SRC_ERR_FORCETIMESTAMP;
		break;
	default:
		{
			VOLOGI("@@@### Return not Source2 defined code:  retValue = 0x%08X", nRC);
		}
	}

	return osRC;
}

int COSBaseSource::InitSrcParam()
{
	if(m_SourceAPI.GetDuration == NULL || m_SourceAPI.GetProgramCount == NULL || m_hSource == NULL)
	{
		VOLOGE("GetDuration 0x%08X, GetProgramCount 0x%08X, m_hSource 0x%08X", m_SourceAPI.GetDuration, m_SourceAPI.GetProgramCount, m_hSource);
		return VOOSMP_ERR_Implement;
	}

	if(m_nOpenFinished == 0)
	{
		VOLOGW("m_nOpenFinished %d", m_nOpenFinished);
		return VOOSMP_ERR_Implement;
	}

	VOLOGI("InitSrcParam+++ %d", voOS_GetSysTime());

	VO_U64 ullDuration = 0;
	int nRC = m_SourceAPI.GetDuration(m_hSource, &ullDuration);
	VOLOGI("GetDuration nRC 0x%08X, duration %lld", nRC, ullDuration);

	unsigned int nProgramNum = 0;
	nRC = m_SourceAPI.GetProgramCount(m_hSource, (VO_U32*)&nProgramNum);
	if(nRC != VO_RET_SOURCE2_OK)
	{
		VOLOGE("failed to m_SourceAPI.GetProgramCount", nRC);
		return ConvertReturnValue(nRC);
	}

	if(m_SourceAPI.GetProgramInfo == NULL)
	{
		VOLOGE("m_SourceAPI.GetProgramInfo 0x%08X", m_SourceAPI.GetProgramInfo);
		return VOOSMP_ERR_Implement;
	}
	unsigned int m, n, l;
	VO_SOURCE2_PROGRAM_INFO* pProgramInfo = NULL;
	
	if(m_pCSubtitle && m_pCSubtitle->GetSubTitleNum())
	{
		if(m_pCSubtitle->GetSubtitleType() == VOOS_SUBTITLE_TIMETEXT)
		{
			for(m = 0; m < nProgramNum; m++)
			{
				nRC = m_SourceAPI.GetProgramInfo(m_hSource, m, &pProgramInfo);
				if(nRC) continue;

				if(pProgramInfo == NULL || pProgramInfo->ppStreamInfo == NULL)
					continue;
			}
			UpdateSrcParam(pProgramInfo);
		}

		voCAutoLock lockProgramInfo (&m_MutexProgramInfo);
		VOLOGI("InitSrcParam enter m_MutexProgramInfo %d", voOS_GetSysTime());

		if(m_pInternalProgramInfo)
		{
			ReleaseProgramInfoOP_T(m_pInternalProgramInfo);
			m_pInternalProgramInfo = NULL;
		}

		CopyProgramInfoOP_T(m_pInProgramInfo, &m_pInternalProgramInfo);

		m_ullDuration = ullDuration;

		VOLOGI ("InitSrcParam--- 0x%08X %d", nRC, voOS_GetSysTime());
		return ConvertReturnValue(nRC);
	}

	VO_SOURCE2_STREAM_INFO* pStreamInfo = NULL;
	VO_SOURCE2_TRACK_INFO* pTrackInfo = NULL;
	
	for(m = 0; m < nProgramNum; m++)
	{
		nRC = m_SourceAPI.GetProgramInfo(m_hSource, m, &pProgramInfo);
		if(nRC) continue;

		if(pProgramInfo == NULL || pProgramInfo->ppStreamInfo == NULL)
			continue;

		// update subtitle type
		for(n = 0; n < pProgramInfo->uStreamCount; n++)
		{
			pStreamInfo = pProgramInfo->ppStreamInfo[n];

			if(pStreamInfo == NULL || pStreamInfo->ppTrackInfo == NULL)
				continue;

			for(l = 0; l < pStreamInfo->uTrackCount; l++)
			{
				pTrackInfo = pStreamInfo->ppTrackInfo[l];

				if(pTrackInfo == NULL)
					continue;

				VOLOGI ("pTrackInfo->uTrackType: %d, pTrackInfo->uTrackID %d, pTrackInfo->uSelInfo %d, pTrackInfo->uCodec %d", pTrackInfo->uTrackType, pTrackInfo->uTrackID, pTrackInfo->uSelInfo, pTrackInfo->uCodec);

				if(pTrackInfo->uTrackType == VO_SOURCE2_TT_SUBTITLE)
				{
					if(pTrackInfo->uSelInfo & VO_SOURCE2_SELECT_SELECTED)
					{
						if(m_pCSubtitle)
						{
							int nSubtitleType = m_pCSubtitle->GetSubtitleType();
							if(nSubtitleType == VOOS_SUBTITLE_CLOSECAPTION)
							{
								delete m_pCSubtitle;
								m_pCSubtitle = NULL;
							}
							else if(nSubtitleType == VOOS_SUBTITLE_TIMETEXT)
							{
								if(pTrackInfo->uCodec == VO_SUBTITLE_CodingDVBTeleText || pTrackInfo->uCodec == VO_SUBTITLE_CodingDVBSystem)
								{
									delete m_pCSubtitle;
									m_pCSubtitle = NULL;
								}
							}
							else if(nSubtitleType == VOOS_SUBTITLE_DVBT)
							{
								if( m_nSubtitleCodecType != pTrackInfo->uCodec)
								{
									delete m_pCSubtitle;
									m_pCSubtitle = NULL;
								}
							}
						}

						if(m_pCSubtitle == NULL)
						{
							if(pTrackInfo->uCodec == VO_SUBTITLE_CodingDVBTeleText || pTrackInfo->uCodec == VO_SUBTITLE_CodingDVBSystem)
							{
								m_pCSubtitle = new COSDVBSubtitle(m_pLibop, m_szPathLib);
								if(m_pCSubtitle == NULL)
								{
									VOLOGE("m_pCSubtitle 0x%08X", m_pCSubtitle);
									return VOOSMP_ERR_OutMemory;
								}
								m_pCSubtitle->Init(0, 0, pTrackInfo->uCodec);								
							}
							else
							{
								m_pCSubtitle = new COSTimeText(m_pLibop, m_szPathLib);
								if(m_pCSubtitle == NULL)
								{
									VOLOGE("m_pCSubtitle 0x%08X", m_pCSubtitle);
									return VOOSMP_ERR_OutMemory;
								}
								VOLOGI("@@@### Subtitle: pTrackInfo->uCodec = %d", (int)pTrackInfo->uCodec);
								m_pCSubtitle->Init(0, 0, pTrackInfo->uCodec);
								m_pCSubtitle->SetParam (ID_SET_JAVA_ENV, (void *)m_nJavaENV);
							}

							m_nSubtitleCodecType = pTrackInfo->uCodec;
						}
					}					
				}
			}
		}

		UpdateSrcParam(pProgramInfo);
	}

	voCAutoLock lockProgramInfo (&m_MutexProgramInfo);
	VOLOGI("InitSrcParam enter m_MutexProgramInfo %d", voOS_GetSysTime());

	if(m_pInternalProgramInfo)
	{
		ReleaseProgramInfoOP_T(m_pInternalProgramInfo);
		m_pInternalProgramInfo = NULL;
	}

	CopyProgramInfoOP_T(m_pInProgramInfo, &m_pInternalProgramInfo);

	m_ullDuration = ullDuration;

	VOLOGI ("InitSrcParam--- %d", voOS_GetSysTime());
	return 0;
}

int COSBaseSource::UpdateSrcParam(VO_SOURCE2_PROGRAM_INFO * pProgramInfo)
{
	int nRC = VOOSMP_ERR_Implement;
	if(pProgramInfo == NULL)
		return VOOSMP_ERR_Implement;

//	voCAutoLock lockProgramInfo (&m_MutexProgramInfo);

	if(pProgramInfo->ppStreamInfo == NULL)
		return VOOSMP_ERR_Implement;

	if(m_pInProgramInfo)
	{
		ReleaseProgramInfoOP_T(m_pInProgramInfo);
		m_pInProgramInfo = NULL;
	}

	CopyProgramInfoOP_T(pProgramInfo, &m_pInProgramInfo);

	// reorder
	unsigned int n, m, k;	
	VO_SOURCE2_STREAM_INFO* pStreamInfo = NULL;
	VO_SOURCE2_TRACK_INFO* pTrackInfo = NULL;

	for(n = 0; n < m_pInProgramInfo->uStreamCount; n++)
	{
		pStreamInfo = m_pInProgramInfo->ppStreamInfo[n];

		if(pStreamInfo == NULL || pStreamInfo->ppTrackInfo == NULL)
			continue;

		for(m = 0; m < pStreamInfo->uTrackCount; m++)
		{
			pTrackInfo = pStreamInfo->ppTrackInfo[m];

			if(pTrackInfo == NULL)
				continue;

			VOLOGI ("pTrackInfo->uTrackType: %d, pTrackInfo->uTrackID %d, pTrackInfo->uSelInfo %d, pTrackInfo->uCodec %d", pTrackInfo->uTrackType, pTrackInfo->uTrackID, pTrackInfo->uSelInfo, pTrackInfo->uCodec);
			//if(pTrackInfo->uTrackType == VO_SOURCE2_TT_MUXGROUP)
			//{
			//	RemoveTrackFromStreamOP_T(pStreamInfo, pTrackInfo);
			//	continue;
			//}

	   	 	if(pTrackInfo->uTrackType != VO_SOURCE2_TT_AUDIO && pTrackInfo->uTrackType != VO_SOURCE2_TT_AUDIOGROUP)
				continue;
			
			for(k = m + 1; k < pStreamInfo->uTrackCount; k++)
			{
				VO_SOURCE2_TRACK_INFO* pTrackInfoTmp = NULL;
				VO_SOURCE2_TRACK_INFO* pTrackInfo2 = pStreamInfo->ppTrackInfo[k];

				if(pTrackInfo2 == NULL)
					continue;

				if(pTrackInfo2->uTrackType == VO_SOURCE2_TT_AUDIO || pTrackInfo2->uTrackType == VO_SOURCE2_TT_AUDIOGROUP)
				{
					if(strcmp(pTrackInfo2->sAudioInfo.chLanguage, pTrackInfo->sAudioInfo.chLanguage) < 0)
					{
						pTrackInfoTmp = pStreamInfo->ppTrackInfo[m];
						pStreamInfo->ppTrackInfo[m] = pStreamInfo->ppTrackInfo[k];
						pStreamInfo->ppTrackInfo[k] = pTrackInfoTmp;
						pTrackInfo = pStreamInfo->ppTrackInfo[m];
					}
				}
			}
		}
	}

	if(m_pCSubtitle == NULL)
		return VOOSMP_ERR_None;
	
	if(m_pCSubtitle->GetSubTitleNum() && m_pCSubtitle->GetTrackCount())
	{
		for(n = 0; n < m_pInProgramInfo->uStreamCount; n++)
		{
			pStreamInfo = m_pInProgramInfo->ppStreamInfo[n];

			if(pStreamInfo == NULL || pStreamInfo->ppTrackInfo == NULL)
				continue;

			for(m = 0; m < pStreamInfo->uTrackCount; m++)
			{
				pTrackInfo = pStreamInfo->ppTrackInfo[m];

				if(pTrackInfo == NULL)
					continue;

				if(pTrackInfo->uTrackType == VO_SOURCE2_TT_SUBTITLE || pTrackInfo->uTrackType == VO_SOURCE2_TT_SUBTITLEGROUP)
				{
					VOLOGI("@@@### Remove internal subtile--> %s", pTrackInfo->sSubtitleInfo.chLanguage);
					RemoveTrackFromStreamOP_T(pStreamInfo, pTrackInfo);
					m = 0;
				}
			}
		}
	}

	for(n = 0; n < m_pInProgramInfo->uStreamCount; n++)
	{
		pStreamInfo = m_pInProgramInfo->ppStreamInfo[n];

		if(pStreamInfo == NULL || pStreamInfo->ppTrackInfo == NULL)
			continue;

		for(m = 0; m < pStreamInfo->uTrackCount; m++)
		{
			pTrackInfo = pStreamInfo->ppTrackInfo[m];

			if(pTrackInfo == NULL)
				continue;

			if(pTrackInfo->uTrackType != VO_SOURCE2_TT_SUBTITLE && pTrackInfo->uTrackType != VO_SOURCE2_TT_SUBTITLEGROUP)
				continue;
			
			for(k = m + 1; k < pStreamInfo->uTrackCount; k++)
			{
				VO_SOURCE2_TRACK_INFO* pTrackInfoTmp = NULL;
				VO_SOURCE2_TRACK_INFO* pTrackInfo2 = pStreamInfo->ppTrackInfo[k];

				if(pTrackInfo2 == NULL)
					continue;

				if(pTrackInfo2->uTrackType == VO_SOURCE2_TT_SUBTITLE || pTrackInfo2->uTrackType == VO_SOURCE2_TT_SUBTITLEGROUP)
				{
					if(strcmp(pTrackInfo2->sSubtitleInfo.chLanguage, pTrackInfo->sSubtitleInfo.chLanguage) < 0)
					{
						pTrackInfoTmp = pStreamInfo->ppTrackInfo[m];
						pStreamInfo->ppTrackInfo[m] = pStreamInfo->ppTrackInfo[k];
						pStreamInfo->ppTrackInfo[k] = pTrackInfoTmp;
						pTrackInfo = pStreamInfo->ppTrackInfo[m];
					}
				}
			}
		}
	}

	if(m_pCSubtitle->GetSubTitleNum() || m_pCSubtitle->GetSubtitleType() == VOOS_SUBTITLE_CLOSECAPTION)
	{
		int nTrackNum = m_pCSubtitle->GetTrackCount();
		if(nTrackNum <= 0)
			return VOOSMP_ERR_None;

		for(m = 0; m < (unsigned int)nTrackNum; m++)
		{
			pTrackInfo = new VO_SOURCE2_TRACK_INFO();
			if(pTrackInfo == NULL)
				return VOOSMP_ERR_OutMemory;
			memset(pTrackInfo, 0, sizeof(VO_SOURCE2_TRACK_INFO));
			pTrackInfo->uTrackType = VO_SOURCE2_TT_SUBTITLE;
			pTrackInfo->uTrackID = 0x10000|m;
			if(m_pCSubtitle->GetSubtitleType() == VOOS_SUBTITLE_CLOSECAPTION)
				pTrackInfo->uCodec = VO_SUBTITLE_CodingCC608;
			else if(m_pCSubtitle->GetSubtitleType() == VOOS_SUBTITLE_TIMETEXT)
				pTrackInfo->uCodec = VO_SUBTITLE_CodingUnused;

			VOOSMP_SUBTITLE_LANGUAGE *pSubLan = NULL;

			nRC = m_pCSubtitle->GetTrackInfo(m, &pSubLan);

			if(nRC == 0 && pSubLan)
			{
				memset(pTrackInfo->sSubtitleInfo.chLanguage, 0, sizeof(char)*256);
				strcpy((char *)pTrackInfo->sSubtitleInfo.chLanguage, (char *)pSubLan->szLangName);
			}

			for(n = 0; n < m_pInProgramInfo->uStreamCount; n++)
			{
				pStreamInfo = m_pInProgramInfo->ppStreamInfo[n];

				if(pStreamInfo == NULL)
					continue;

				if((pStreamInfo->uSelInfo&VO_SOURCE2_SELECT_SELECTED) && (unsigned int)m_nSubtitleSelect == (pTrackInfo->uTrackID&0xffff))
				{
					pTrackInfo->uSelInfo |= VO_SOURCE2_SELECT_SELECTED;
				}
				else
				{
					pTrackInfo->uSelInfo = 0;
				}		

				InsertTrackInfoOP_T(m_pInProgramInfo, pStreamInfo->uStreamID, pTrackInfo);
			}

			delete pTrackInfo;
			pTrackInfo = NULL;
		}		
	}

	return VOOSMP_ERR_None;
}

int COSBaseSource::GetSubLangNum(int *pNum)
{
	int nNum  = 0;

	voCAutoLock lockReadSrc (&m_MutexReadData);

	if(m_pCSubtitle)
	{
		nNum = m_pCSubtitle->GetTrackCount();
	}

	*pNum = nNum;

	return VOOSMP_ERR_None; 
}

int COSBaseSource::GetSubLangItem(int Index, VOOSMP_SUBTITLE_LANGUAGE** ppSubLangItem)
{
	/*int nRC = VOOSMP_ERR_Retry;*/

	voCAutoLock lockReadSrc (&m_MutexReadData);

	if(m_pCSubtitle)
		m_pCSubtitle->GetTrackInfo(Index, ppSubLangItem);

	return VOOSMP_ERR_None;
}

int COSBaseSource::GetSubLangInfo(VOOSMP_SUBTITLE_LANGUAGE_INFO** pSubLangInfo)
{
	int nRC = VOOSMP_ERR_Implement;

	voCAutoLock lockReadSrc (&m_MutexReadData);

	if(m_pCSubtitle)
		nRC = m_pCSubtitle->GetSubLangInfo(pSubLangInfo);

	return nRC;
}

int COSBaseSource::SelectLanguage(int Index)
{
	int nRC = VOOSMP_ERR_Implement;

	voCAutoLock lockReadSrc (&m_MutexReadData);

	if(m_pCSubtitle)
	{
		nRC = m_pCSubtitle->SelectTrackByIndex(Index);
		m_nSubtitleSelect = Index;
	}

	return nRC;
}


int COSBaseSource::LoadDll()
{
	return VOOSMP_ERR_Implement; 
}


int COSBaseSource::unLoadDll()
{
	if (m_hDll != NULL && m_pLibop != NULL)
	{
		m_pLibop->FreeLib(m_pLibop->pUserData, m_hDll, 0);
		m_hDll = NULL;
	}

	memset(&m_SourceAPI, 0, sizeof(VO_SOURCE2_API));

	return 0; 
}

int COSBaseSource::OpenConfig()
{
	VO_TCHAR	szCfgFile[1024];	

#ifdef LINUX
	vostrcpy (szCfgFile, _T("/data/local/tmp/OnStreamPlayer/osmpDemoPlay.cfg"));
#else
	voOS_GetAppFolder (szCfgFile, 1024);
	vostrcat (szCfgFile, _T("osmpDemoPlay.cfg"));
#endif // LINUX

	if (!m_cfgSource.Open (szCfgFile))
	{
		return 0;
	}

	m_nLogAudioLevel = m_cfgSource.GetItemValue("OSMP_SOURRCE", "LogAudioLevel", m_nLogAudioLevel);
	m_nLogVideoLevel = m_cfgSource.GetItemValue("OSMP_SOURRCE", "LogVideoLevel", m_nLogVideoLevel);

	m_pAudioDumpFile = m_cfgSource.GetItemText("OSMP_SOURRCE", "AudioDumpFile", m_pAudioDumpFile);
	m_pVideoDumpFile = m_cfgSource.GetItemText("OSMP_SOURRCE", "VideoDumpFile", m_pVideoDumpFile);
	
	m_nDumpAudioLevel = m_cfgSource.GetItemValue("OSMP_SOURRCE", "DumpAudioLevel", m_nDumpAudioLevel);
	m_nDumpVideoLevel = m_cfgSource.GetItemValue("OSMP_SOURRCE", "DumpVideoLevel", m_nDumpVideoLevel);
	
	return 0;
}


int COSBaseSource::DumpInfo(int ssType, int nRC, VOOSMP_BUFFERTYPE * pBuffer)
{
	if(ssType == VOOSMP_SS_AUDIO)
	{
		if(m_nLogAudioLevel&1)
		{
			if(m_nStartSystemTime == 0)
				m_nStartSystemTime = voOS_GetSysTime();

#ifdef _VOLOG_INFO
			int Diff = 0;
			Diff = (int)pBuffer->llTime - (voOS_GetSysTime() - m_nStartSystemTime);
			VOLOGI("GetAudio Sample return %x, TimeStamp %d, SystemTime %d, Diff %d, BufferSize %d, nFlag %d", nRC, (int)pBuffer->llTime, (int)voOS_GetSysTime() - m_nStartSystemTime, Diff, pBuffer->nSize, pBuffer->nFlag);
#endif
		}

		if(nRC == VOOSMP_ERR_None)
		{
			if((m_nDumpAudioLevel & 8) && m_hAudioDumpFile)
			{
				int nFlag = (int)(pBuffer->nFlag);
				m_pFileOP->Write(m_hAudioDumpFile,(VO_PTR)(&nFlag), 4);
			}
			
			if((m_nDumpAudioLevel & 4) && m_hAudioDumpFile)
			{
				int nTime = (int)(pBuffer->llTime);
				m_pFileOP->Write(m_hAudioDumpFile,(VO_PTR)(&nTime), 4);
			}

			if((m_nDumpAudioLevel & 2) && m_hAudioDumpFile)
			{
				int nSize = (int)(pBuffer->nSize);
				if((pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_FORMAT) || (pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_PROGRAM))
				{
					nSize = (int)(pBuffer->nSize);
					m_pFileOP->Write(m_hAudioDumpFile, (VO_PTR)(&m_sAudioFormat.nHeadDataLen) , 4);
				}
				else
				{
					m_pFileOP->Write(m_hAudioDumpFile, (VO_PTR)(&nSize), 4);
				}

			}

			if((m_nDumpAudioLevel & 1) && m_hAudioDumpFile)
			{
				if((pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_FORMAT) || (pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_PROGRAM))
				{
					if(m_sAudioFormat.pHeadData && m_sAudioFormat.nHeadDataLen > 0)
						m_pFileOP->Write(m_hAudioDumpFile, m_sAudioFormat.pHeadData , m_sAudioFormat.nHeadDataLen);
				}
				else
				{
					if(pBuffer->pBuffer && pBuffer->nSize > 0)
						m_pFileOP->Write(m_hAudioDumpFile, pBuffer->pBuffer , pBuffer->nSize);
				}
			}
		}
	}
	else if(ssType == VOOSMP_SS_VIDEO)
	{
		if(m_nLogVideoLevel&1)
		{
			if(m_nStartSystemTime == 0)
				m_nStartSystemTime = voOS_GetSysTime();

#ifdef _VOLOG_INFO
			int Diff = 0;
			Diff = (int)pBuffer->llTime - (voOS_GetSysTime() - m_nStartSystemTime);
			VOLOGI("GetVideo Sample return %x, TimeStamp %d, SystemTime %d, Diff %d, BufferSize %d, nFlag %d", nRC, (int)pBuffer->llTime, (int)voOS_GetSysTime() - m_nStartSystemTime, Diff, pBuffer->nSize, pBuffer->nFlag);
#endif
		}

		if(nRC == VOOSMP_ERR_None)
		{		
			if((m_nDumpVideoLevel & 8) && m_hVideoDumpFile)
			{
				int nFlag = (int)(pBuffer->nFlag);
				m_pFileOP->Write(m_hVideoDumpFile,(VO_PTR)(&nFlag), 4);
			}
			
			if((m_nDumpVideoLevel & 4) && m_hVideoDumpFile)
			{
				int nTime = (int)(pBuffer->llTime);
				m_pFileOP->Write(m_hVideoDumpFile,(VO_PTR)(&nTime), 4);
			}

			if((m_nDumpVideoLevel & 2) && m_hVideoDumpFile)
			{
				int nSize = (int)(pBuffer->nSize);
				if((pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_FORMAT) || (pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_PROGRAM))
				{
					nSize = (int)(pBuffer->nSize);
					m_pFileOP->Write(m_hVideoDumpFile, (VO_PTR)(&m_sVideoFormat.nHeadDataLen) , 4);
				}
				else
				{
					m_pFileOP->Write(m_hVideoDumpFile, (VO_PTR)(&nSize), 4);
				}
			}

			if((m_nDumpVideoLevel & 1) && m_hVideoDumpFile)
			{
				if((pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_FORMAT) || (pBuffer->nFlag & VOOSMP_FLAG_BUFFER_NEW_PROGRAM))
				{
					if(m_hVideoDumpFile)
						m_pFileOP->Close(m_hVideoDumpFile);

					VO_FILE_SOURCE filSource;
					memset (&filSource, 0, sizeof (VO_FILE_SOURCE));
					filSource.nFlag = VO_FILE_TYPE_NAME;
					filSource.nMode = VO_FILE_WRITE_ONLY;

					if(m_pVideoDumpFile && m_pFileOP)
					{
#ifdef _WIN32
						VO_TCHAR szFile[1024];
						memset (szFile, 0, sizeof (szFile));
						MultiByteToWideChar (CP_ACP, 0, m_pVideoDumpFile, -1, szFile, sizeof (szFile)); 
						filSource.pSource = szFile;	
#elif defined _IOS
						char szFile[1024];
						voOS_GetAppFolder(szFile, 1024);
						vostrcat(szFile, m_pVideoDumpFile);
						filSource.pSource = szFile;
#else
						filSource.pSource = m_pVideoDumpFile;
#endif
						m_hVideoDumpFile = m_pFileOP->Open (&filSource);
					}				

					if(m_sVideoFormat.pHeadData && m_sVideoFormat.nHeadDataLen > 0)
						m_pFileOP->Write(m_hVideoDumpFile, m_sVideoFormat.pHeadData , m_sVideoFormat.nHeadDataLen);
				}
				else
				{
					if(pBuffer->pBuffer && pBuffer->nSize > 0)
						m_pFileOP->Write(m_hVideoDumpFile, pBuffer->pBuffer , pBuffer->nSize);
				}
			}
		}
	}

	return 0;

}


int	COSBaseSource::CheckingStatus(int nCount)
{
	int n = 0;

	while(m_nChanging)
	{
		voOS_Sleep(2);
		n++;

		if(n > nCount)
			break;
	}

	return 0;
}
int COSBaseSource::onSetParam(int nID, void * pValue)
{
	if(!mCheckLib)
		return VOOSMP_ERR_Pointer;

	VO_VIDEO_BUFFER videoBuff;
	int nRC = VOOSMP_ERR_None;
	switch(nID)
	{
	case VOOSMP_PID_PLAYER_PATH:
	case VOOSMP_PID_LICENSE_FILE_PATH:
		{
			videoBuff.ColorType = (VO_IV_COLORTYPE)VOLICENSE_SET_PATH;
			videoBuff.Buffer[0] = (VO_PBYTE)pValue;
			nRC = voCheckLibCheckImage (mCheckLib, &videoBuff, NULL);	
		}
		break;
	case VOOSMP_PID_LICENSE_TEXT:
		{
			videoBuff.ColorType = (VO_IV_COLORTYPE)VOLICENSE_SET_TEXT;
			videoBuff.Buffer[0] = (VO_PBYTE)pValue;
			nRC = voCheckLibCheckImage (mCheckLib, &videoBuff, NULL);
		}
		break;
	case VOOSMP_PID_LICENSE_CONTENT:
		{
			videoBuff.ColorType = (VO_IV_COLORTYPE)VOLICENSE_SET_DATA;
			videoBuff.Buffer[0] = (VO_PBYTE)pValue;
			nRC = voCheckLibCheckImage (mCheckLib, &videoBuff, NULL);
		}
		break;
	case VOOSMP_PID_COMMON_LOGFUNC:
		{
			videoBuff.ColorType = (VO_IV_COLORTYPE)VOLICENSE_SET_VOLOG;
			videoBuff.Buffer[0] = (VO_PBYTE)pValue;
			nRC = voCheckLibCheckImage (mCheckLib, &videoBuff, NULL);
		}
		break;
	default:
		break;
	}
	return nRC;
}

int COSBaseSource::GetMediaCount(int ssType)
{
	int nRC = UpdateTrackInfo();

	if(nRC) return 0;

	if(ssType == VOOSMP_SS_AUDIO)
	{
		return m_nAudioCount;
	}
	else if(ssType == VOOSMP_SS_VIDEO)
	{
		return m_nVideoCount;
	}
	else if(ssType == VOOSMP_SS_SUBTITLE)
	{
		return m_nSubtitleCount;
	}
	else
	{
		return 0;
	}
}

int COSBaseSource::SelectMediaIndex(int ssType, int nIndex)
{
	VOLOGI("@@@### ssType = %d, nIndex = %d", ssType, nIndex);
	if(ssType == VOOSMP_SS_AUDIO)
	{
		m_nAudioSelected = nIndex;
	}
	else if(ssType == VOOSMP_SS_VIDEO)
	{
		m_nVideoSelected = nIndex;
	}
	else if(ssType == VOOSMP_SS_SUBTITLE)
	{
		m_nSubtitleSelected = nIndex;
	}
	else
	{
		return VOOSMP_ERR_Implement;
	}

	return VOOSMP_ERR_None;
}
	
int COSBaseSource::IsMediaIndexAvailable(int ssType, int nIndex)
{
	return 1;
}

int COSBaseSource::GetMediaProperty(int ssType, int nIndex, VOOSMP_SRC_TRACK_PROPERTY **pProperty)
{
	if(nIndex < 0)
		return VOOSMP_ERR_Implement;

	int nRC = UpdateTrackInfo();
	if(nRC) return nRC;

	int nTrackIndex = -1;

	if(ssType == VOOSMP_SS_AUDIO)
	{
		nTrackIndex = m_pAudioTrackIndex[nIndex];
		ClearPropertyItem(&m_szAudioProtery);
	}
	else if(ssType == VOOSMP_SS_VIDEO)
	{
		nTrackIndex = m_pVideoTrackIndex[nIndex];
		ClearPropertyItem(&m_szVideoProtery);
	}
	else if(ssType == VOOSMP_SS_SUBTITLE)
	{
		nTrackIndex = m_pSubtitleTrackIndex[nIndex];

		ClearPropertyItem(&m_szSubtitleProtery);
	}
	else
	{
		return VOOSMP_ERR_Implement;
	}


	VO_SOURCE2_STREAM_INFO* pStreamInfo = NULL;
	VO_SOURCE2_TRACK_INFO* pTrackInfo = NULL;	
	
	unsigned int i, j;

	if(m_pInternalProgramInfo == NULL || m_pInternalProgramInfo->ppStreamInfo == NULL)
		return VOOSMP_ERR_Implement;

	for(i = 0; i < m_pInternalProgramInfo->uStreamCount; i++)
	{
		pStreamInfo = m_pInternalProgramInfo->ppStreamInfo[i];

		if(pStreamInfo == NULL || pStreamInfo->ppTrackInfo == NULL)
			continue;

		if(ssType == VOOSMP_SS_VIDEO)
		{
			/*if(nTrackIndex != (int)pStreamInfo->uStreamID)
				continue;*/
			/* Local file using current playback video track */
		}

		for(j = 0; j < pStreamInfo->uTrackCount; j++)
		{
			pTrackInfo = pStreamInfo->ppTrackInfo[j];

			if(pTrackInfo == NULL)
				continue;

			if(ssType == VOOSMP_SS_VIDEO)
			{
				if(pTrackInfo->uTrackType == VOOSMP_SS_VIDEO || pTrackInfo->uTrackType == VOOSMP_SS_VIDEO_GROUP)
				{
					m_szVideoProtery.nPropertyCount = 5;
					m_szVideoProtery.ppItemProperties = new VOOSMP_SRC_TRACK_ITEM_PROPERTY*[5];
					if(m_szVideoProtery.ppItemProperties == NULL)
						return VOOSMP_ERR_OutMemory;

					m_szVideoProtery.ppItemProperties[0] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szVideoProtery.ppItemProperties[0] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szVideoProtery.ppItemProperties[0], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szVideoProtery.ppItemProperties[0]->szKey, "description");

					m_szVideoProtery.ppItemProperties[0]->pszProperty = new char[32];
					if(m_szVideoProtery.ppItemProperties[0]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szVideoProtery.ppItemProperties[0]->pszProperty, 0, sizeof(char)*32);
					if(strlen(pTrackInfo->sVideoInfo.strVideoDesc) > 0)
					{
						strcpy(m_szVideoProtery.ppItemProperties[0]->pszProperty, pTrackInfo->sVideoInfo.strVideoDesc);

						VOLOGI("video description: %s, strlen %d, pTrackInfo->sVideoInfo.strVideoDesc[0] %d", m_szVideoProtery.ppItemProperties[0]->pszProperty, strlen(pTrackInfo->sVideoInfo.strVideoDesc), 
							pTrackInfo->sVideoInfo.strVideoDesc[0]);
					}
					else
						sprintf(m_szVideoProtery.ppItemProperties[0]->pszProperty, "Video %d", nIndex);


					m_szVideoProtery.ppItemProperties[1] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szVideoProtery.ppItemProperties[1] == NULL)
						return VOOSMP_ERR_OutMemory;
						
					memset(m_szVideoProtery.ppItemProperties[1], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szVideoProtery.ppItemProperties[1]->szKey, "codec");

					m_szVideoProtery.ppItemProperties[1]->pszProperty = new char[32];
					if(m_szVideoProtery.ppItemProperties[1]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szVideoProtery.ppItemProperties[1]->pszProperty, 0, sizeof(char)*32);
					strcpy(m_szVideoProtery.ppItemProperties[1]->pszProperty, VideoCodec[pTrackInfo->uCodec]);

					m_szVideoProtery.ppItemProperties[2] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szVideoProtery.ppItemProperties[2] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szVideoProtery.ppItemProperties[2], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szVideoProtery.ppItemProperties[2]->szKey, "bitrate");

					m_szVideoProtery.ppItemProperties[2]->pszProperty = new char[32];
					if(m_szVideoProtery.ppItemProperties[2]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szVideoProtery.ppItemProperties[2]->pszProperty, 0, sizeof(char)*32);

					if(pTrackInfo->uBitrate)
						sprintf(m_szVideoProtery.ppItemProperties[2]->pszProperty, "%d bps", (int)pTrackInfo->uBitrate);
					else
						sprintf(m_szVideoProtery.ppItemProperties[2]->pszProperty, "%d bps", (int)pStreamInfo->uBitrate);
					VOLOGI("@@@### bps = %s", m_szVideoProtery.ppItemProperties[2]->pszProperty);


					m_szVideoProtery.ppItemProperties[3] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szVideoProtery.ppItemProperties[3] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szVideoProtery.ppItemProperties[3], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szVideoProtery.ppItemProperties[3]->szKey, "width");

					m_szVideoProtery.ppItemProperties[3]->pszProperty = new char[32];
					if(m_szVideoProtery.ppItemProperties[3]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szVideoProtery.ppItemProperties[3]->pszProperty, 0, sizeof(char)*32);
					sprintf(m_szVideoProtery.ppItemProperties[3]->pszProperty, "%d", (int)pTrackInfo->sVideoInfo.sFormat.Width);

					m_szVideoProtery.ppItemProperties[4] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szVideoProtery.ppItemProperties[4] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szVideoProtery.ppItemProperties[4], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szVideoProtery.ppItemProperties[4]->szKey, "height");

					m_szVideoProtery.ppItemProperties[4]->pszProperty = new char[32];
					if(m_szVideoProtery.ppItemProperties[4]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szVideoProtery.ppItemProperties[4]->pszProperty, 0, sizeof(char)*32);
					sprintf(m_szVideoProtery.ppItemProperties[4]->pszProperty, "%d", (int)pTrackInfo->sVideoInfo.sFormat.Height);

					*pProperty = &m_szVideoProtery;

					return 0;
				}

				continue;
			}

			if((int)pTrackInfo->uTrackID == nTrackIndex)
			{
				if(pTrackInfo->uTrackType == VOOSMP_SS_AUDIO || pTrackInfo->uTrackType == VOOSMP_SS_AUDIO_GROUP)
				{
					m_szAudioProtery.nPropertyCount = 5;

					m_szAudioProtery.ppItemProperties = new VOOSMP_SRC_TRACK_ITEM_PROPERTY*[5];
					if(m_szAudioProtery.ppItemProperties == NULL)
						return VOOSMP_ERR_OutMemory;

					m_szAudioProtery.ppItemProperties[0] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szAudioProtery.ppItemProperties[0] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szAudioProtery.ppItemProperties[0], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szAudioProtery.ppItemProperties[0]->szKey, "description");

					m_szAudioProtery.ppItemProperties[0]->pszProperty = new char[32];
					if(m_szAudioProtery.ppItemProperties[0]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szAudioProtery.ppItemProperties[0]->pszProperty, 0, sizeof(char)*32);
					sprintf(m_szAudioProtery.ppItemProperties[0]->pszProperty, "Audio %d", nIndex);


					m_szAudioProtery.ppItemProperties[1] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szAudioProtery.ppItemProperties[1] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szAudioProtery.ppItemProperties[1], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szAudioProtery.ppItemProperties[1]->szKey, "language");

					m_szAudioProtery.ppItemProperties[1]->pszProperty = new char[32];
					if(m_szAudioProtery.ppItemProperties[1]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szAudioProtery.ppItemProperties[1]->pszProperty, 0, sizeof(char)*32);

					if(strlen(pTrackInfo->sAudioInfo.chLanguage) > 0)
						strcpy(m_szAudioProtery.ppItemProperties[1]->pszProperty, (const char *)pTrackInfo->sAudioInfo.chLanguage);
					else
						strcpy(m_szAudioProtery.ppItemProperties[1]->pszProperty, "unknown");

					m_szAudioProtery.ppItemProperties[2] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szAudioProtery.ppItemProperties[2] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szAudioProtery.ppItemProperties[2], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szAudioProtery.ppItemProperties[2]->szKey, "codec");

					m_szAudioProtery.ppItemProperties[2]->pszProperty = new char[32];
					if(m_szAudioProtery.ppItemProperties[2]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szAudioProtery.ppItemProperties[2]->pszProperty, 0, sizeof(char)*32);
					strcpy(m_szAudioProtery.ppItemProperties[2]->pszProperty, AudioCodec[pTrackInfo->uCodec]);

					/* bitrate */
					m_szAudioProtery.ppItemProperties[3] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szAudioProtery.ppItemProperties[3] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szAudioProtery.ppItemProperties[3], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));
					strcpy(m_szAudioProtery.ppItemProperties[3]->szKey, "bitrate");
					m_szAudioProtery.ppItemProperties[3]->pszProperty = new char[32];
					if(m_szAudioProtery.ppItemProperties[3]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szAudioProtery.ppItemProperties[3]->pszProperty, 0, sizeof(char)*32);
					sprintf(m_szAudioProtery.ppItemProperties[3]->pszProperty, "%d", (int)pTrackInfo->uBitrate);

					/* channel count */
					m_szAudioProtery.ppItemProperties[4] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szAudioProtery.ppItemProperties[4] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szAudioProtery.ppItemProperties[4], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));
					strcpy(m_szAudioProtery.ppItemProperties[4]->szKey, "channelcount");
					m_szAudioProtery.ppItemProperties[4]->pszProperty = new char[32];
					if(m_szAudioProtery.ppItemProperties[4]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szAudioProtery.ppItemProperties[4]->pszProperty, 0, sizeof(char)*32);
					sprintf(m_szAudioProtery.ppItemProperties[4]->pszProperty, "%d", (int)pTrackInfo->sAudioInfo.sFormat.Channels);

					*pProperty = &m_szAudioProtery;

					return 0;
				}
				else if(pTrackInfo->uTrackType == VOOSMP_SS_SUBTITLE || pTrackInfo->uTrackType == VOOSMP_SS_SUBTITLE_GROUP)
				{
					m_szSubtitleProtery.nPropertyCount = 3;

					m_szSubtitleProtery.ppItemProperties = new VOOSMP_SRC_TRACK_ITEM_PROPERTY*[3];
					if(m_szSubtitleProtery.ppItemProperties == NULL)
						return VOOSMP_ERR_OutMemory;

					m_szSubtitleProtery.ppItemProperties[0] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szSubtitleProtery.ppItemProperties[0] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szSubtitleProtery.ppItemProperties[0], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szSubtitleProtery.ppItemProperties[0]->szKey, "description");

					m_szSubtitleProtery.ppItemProperties[0]->pszProperty = new char[32];
					if(m_szSubtitleProtery.ppItemProperties[0]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szSubtitleProtery.ppItemProperties[0]->pszProperty, 0, sizeof(char)*32);
					sprintf(m_szSubtitleProtery.ppItemProperties[0]->pszProperty, "SubTitle %d", nIndex);


					m_szSubtitleProtery.ppItemProperties[1] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szSubtitleProtery.ppItemProperties[1] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szSubtitleProtery.ppItemProperties[1], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szSubtitleProtery.ppItemProperties[1]->szKey, "language");

					m_szSubtitleProtery.ppItemProperties[1]->pszProperty = new char[32];
					if(m_szSubtitleProtery.ppItemProperties[1]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szSubtitleProtery.ppItemProperties[1]->pszProperty, 0, sizeof(char)*32);

					if(strlen(pTrackInfo->sSubtitleInfo.chLanguage) > 0)
						strcpy(m_szSubtitleProtery.ppItemProperties[1]->pszProperty, (const char *)pTrackInfo->sSubtitleInfo.chLanguage);
					else
						strcpy(m_szSubtitleProtery.ppItemProperties[1]->pszProperty, "unknown");

					m_szSubtitleProtery.ppItemProperties[2] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szSubtitleProtery.ppItemProperties[2] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szSubtitleProtery.ppItemProperties[2], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szSubtitleProtery.ppItemProperties[2]->szKey, "codec");

					m_szSubtitleProtery.ppItemProperties[2]->pszProperty = new char[32];
					if(m_szSubtitleProtery.ppItemProperties[2]->pszProperty == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_szSubtitleProtery.ppItemProperties[2]->pszProperty, 0, sizeof(char)*32);
					strcpy(m_szSubtitleProtery.ppItemProperties[2]->pszProperty, "unknown");

					*pProperty = &m_szSubtitleProtery;

					return 0;
				}
			}

		}

		if(ssType == VOOSMP_SS_VIDEO)
		{
			if(nTrackIndex == (int)pStreamInfo->uStreamID)
			{
				m_szVideoProtery.nPropertyCount = 5;
				m_szVideoProtery.ppItemProperties = new VOOSMP_SRC_TRACK_ITEM_PROPERTY*[5];
				if(m_szVideoProtery.ppItemProperties == NULL)
					return VOOSMP_ERR_OutMemory;

				m_szVideoProtery.ppItemProperties[0] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
				if(m_szVideoProtery.ppItemProperties[0] == NULL)
					return VOOSMP_ERR_OutMemory;
				memset(m_szVideoProtery.ppItemProperties[0], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

				strcpy(m_szVideoProtery.ppItemProperties[0]->szKey, "description");

				m_szVideoProtery.ppItemProperties[0]->pszProperty = new char[32];
				if(m_szVideoProtery.ppItemProperties[0]->pszProperty == NULL)
					return VOOSMP_ERR_OutMemory;
				memset(m_szVideoProtery.ppItemProperties[0]->pszProperty, 0, sizeof(char)*32);
				strcpy(m_szVideoProtery.ppItemProperties[0]->pszProperty, "unknown");

				m_szVideoProtery.ppItemProperties[1] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
				if(m_szVideoProtery.ppItemProperties[1] == NULL)
					return VOOSMP_ERR_OutMemory;
				memset(m_szVideoProtery.ppItemProperties[1], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

				strcpy(m_szVideoProtery.ppItemProperties[1]->szKey, "codec");

				m_szVideoProtery.ppItemProperties[1]->pszProperty = new char[32];
				if(m_szVideoProtery.ppItemProperties[1]->pszProperty == NULL)
					return VOOSMP_ERR_OutMemory;
				memset(m_szVideoProtery.ppItemProperties[1]->pszProperty, 0, sizeof(char)*32);
				strcpy(m_szVideoProtery.ppItemProperties[1]->pszProperty, VideoCodec[0]);

				m_szVideoProtery.ppItemProperties[2] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
				if(m_szVideoProtery.ppItemProperties[2] == NULL)
					return VOOSMP_ERR_OutMemory;
				memset(m_szVideoProtery.ppItemProperties[2], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

				strcpy(m_szVideoProtery.ppItemProperties[2]->szKey, "bitrate");

				m_szVideoProtery.ppItemProperties[2]->pszProperty = new char[32];
				if(m_szVideoProtery.ppItemProperties[2]->pszProperty == NULL)
					return VOOSMP_ERR_OutMemory;
				memset(m_szVideoProtery.ppItemProperties[2]->pszProperty, 0, sizeof(char)*32);
				sprintf(m_szVideoProtery.ppItemProperties[2]->pszProperty, "%d bps", (int)pStreamInfo->uBitrate);
				VOLOGI("@@@### bps = %s", m_szVideoProtery.ppItemProperties[2]->pszProperty);

				m_szVideoProtery.ppItemProperties[3] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
				if(m_szVideoProtery.ppItemProperties[3] == NULL)
					return VOOSMP_ERR_OutMemory;
				memset(m_szVideoProtery.ppItemProperties[3], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

				strcpy(m_szVideoProtery.ppItemProperties[3]->szKey, "width");
				m_szVideoProtery.ppItemProperties[3]->pszProperty = new char[32];
				if(m_szVideoProtery.ppItemProperties[3]->pszProperty == NULL)
					return VOOSMP_ERR_OutMemory;
				memset(m_szVideoProtery.ppItemProperties[3]->pszProperty, 0, sizeof(char)*32);
				strcpy(m_szVideoProtery.ppItemProperties[3]->pszProperty, "unknown");

				m_szVideoProtery.ppItemProperties[4] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
				if(m_szVideoProtery.ppItemProperties[4] == NULL)
					return VOOSMP_ERR_OutMemory;
				memset(m_szVideoProtery.ppItemProperties[4], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

				strcpy(m_szVideoProtery.ppItemProperties[4]->szKey, "height");
				m_szVideoProtery.ppItemProperties[4]->pszProperty = new char[32];
				if(m_szVideoProtery.ppItemProperties[4]->pszProperty == NULL)
					return VOOSMP_ERR_OutMemory;
				memset(m_szVideoProtery.ppItemProperties[4]->pszProperty, 0, sizeof(char)*32);
				strcpy(m_szVideoProtery.ppItemProperties[4]->pszProperty, "unknown");

				*pProperty = &m_szVideoProtery;

				return 0;
			}
		}
	}

	return VOOSMP_ERR_Implement;
}

int COSBaseSource::CommetSelection(int bSelect)
{
	if(bSelect)
	{
		int nRC = UpdateTrackInfo();
		if(nRC) return nRC;

		VOOSMP_SRC_CURR_TRACK_INDEX mCurIndex;		
		nRC = GetCurMediaTrack(&mCurIndex);
		if(nRC) return nRC;
		
		int nAudioSelect = -1;
		int nVideoSelect = -1;
		int nSubTitleSelect = -1;
		if(m_nAudioSelected == -2)
		{
			nAudioSelect = mCurIndex.nCurrAudioIdx;
		}
		else
		{
			nAudioSelect = m_nAudioSelected;
		}

		if(m_nVideoSelected == -2)
		{
			nVideoSelect = mCurIndex.nCurrVideoIdx;
		}
		else
		{
			nVideoSelect = m_nVideoSelected;
		}

		if(m_nSubtitleSelected == -2)
		{
			nSubTitleSelect = mCurIndex.nCurrSubtitleIdx;
		}
		else
		{
			nSubTitleSelect = m_nSubtitleSelected;
		}
		
		if(IsMediaIndexAvailable(VOOSMP_SS_AUDIO, nAudioSelect) 
			&& IsMediaIndexAvailable(VOOSMP_SS_VIDEO, nVideoSelect) 
			&& IsMediaIndexAvailable(VOOSMP_SS_SUBTITLE, nSubTitleSelect))
		{
			if(nAudioSelect >= 0)
			{
				nAudioSelect = m_pAudioTrackIndex[nAudioSelect];

				nRC = SelectTrack(nAudioSelect);
				if(nRC) return nRC;
			}

			if(nVideoSelect >= 0)
			{
				nVideoSelect = m_pVideoTrackIndex[nVideoSelect];

				nRC = SelectTrack(nVideoSelect);
				if(nRC) return nRC;

				m_nBAStreamEnable = 0;
			}
			else
			{
				m_nBAStreamEnable = 1;
			}

			if(nSubTitleSelect >= 0)
			{
				if(m_nExtendSubtitle > 0)
				{
					nRC = SelectLanguage(nSubTitleSelect);
				}	
				else
				{
					nSubTitleSelect = m_pSubtitleTrackIndex[nSubTitleSelect];

					nRC = SelectTrack(nSubTitleSelect);
					if(nRC) return nRC;
				}
			}
		}
		else
		{
			return VOOSMP_ERR_Implement;
		}
	}

	m_nAudioSelected = -2;
	m_nVideoSelected = -2;
	m_nSubtitleSelected = -2;
	return VOOSMP_ERR_None;
}

int COSBaseSource::GetCurMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex)
{
	if(pCurIndex == NULL)
		return VOOSMP_ERR_Pointer;

	int nRC = VOOSMP_ERR_Implement;
	int n;

	nRC = UpdateTrackInfo();

	pCurIndex->nCurrAudioIdx = -1;
	m_nAudioCurTrack = -1;
	VO_SOURCE2_TRACK_INFO *pTrackInfo = NULL;
	nRC = GetCurTrackInfo(VO_SOURCE2_TT_AUDIO, &pTrackInfo);
	if(nRC == 0 && pTrackInfo)
	{
		for(n = 0; n < m_nAudioCount; n++)
		{
			if((int)pTrackInfo->uTrackID == m_pAudioTrackIndex[n])
			{
				m_nAudioCurTrack = pTrackInfo->uTrackID;
				pCurIndex->nCurrAudioIdx = n;
				break;
			}
		}
	}
	
	pCurIndex->nCurrVideoIdx = -1;
	m_nVideoCurTrack = -1;
	pTrackInfo = NULL;
	nRC = GetCurTrackInfo(VO_SOURCE2_TT_VIDEO, &pTrackInfo);
	if(nRC == 0 && pTrackInfo && m_nBAStreamEnable == 0)
	{
		for(n = 0; n < m_nVideoCount; n++)
		{
			if((int)pTrackInfo->uTrackID == m_pVideoTrackIndex[n])
			{
				m_nVideoCurTrack = pTrackInfo->uTrackID;
				pCurIndex->nCurrVideoIdx = n;
				break;
			}
		}
	}

	pCurIndex->nCurrSubtitleIdx = -1;
	m_nSubtitleCurTrack = -1;
	if(m_nExtendSubtitle > 0)
	{
		if(m_nSubtitleCurTrack == -1)
			m_nSubtitleCurTrack = 0;
		pCurIndex->nCurrSubtitleIdx = m_nSubtitleCurTrack;
		m_nSubtitleCurTrack = m_nSubtitleCurTrack;
	}
	else
	{
		pTrackInfo = NULL;
		nRC = GetCurTrackInfo(VO_SOURCE2_TT_SUBTITLE, &pTrackInfo);
		if(nRC == 0 && pTrackInfo)
		{
			for(n = 0; n < m_nSubtitleCount; n++)
			{
				if((int)pTrackInfo->uTrackID == m_pSubtitleTrackIndex[n])
				{
					m_nSubtitleCurTrack = pTrackInfo->uTrackID;
					pCurIndex->nCurrSubtitleIdx = n;
					break;
				}
			}
		}
	}

	return VOOSMP_ERR_None;
}

int COSBaseSource::GetCurPlayingMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex)
{
	/*API3.0 using this function, implementation in class COSAdaptiveStrSource*/
	return VOOSMP_ERR_Implement;
}

int COSBaseSource::GetCurSelectedMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex)
{
	/*API3.0 using this function, implementation in class COSAdaptiveStrSource*/
	return VOOSMP_ERR_Implement;
}

int COSBaseSource::UpdateTrackInfo()
{
	int nCount = 0;
	int n;

	m_nAudioCount = 0;
	m_nVideoCount = 0;
	m_nSubtitleCount = 0;
	m_nExtendSubtitle = 0;

	int nRC = GetSubLangNum(&nCount);
	if(nRC == 0 && nCount > 0)
	{
		m_nSubtitleCount = nCount;

		for(n =0; n < nCount; n++)
		{
			m_pSubtitleTrackIndex[n] = n;
		}

		m_nExtendSubtitle = nCount;
	}

	unsigned int nProgramcount = 0;
	VO_SOURCE2_PROGRAM_INFO *pProgramInfo = NULL;
	VO_SOURCE2_STREAM_INFO* pStreamInfo = NULL;
	VO_SOURCE2_TRACK_INFO* pTrackInfo = NULL;

	nRC = GetProgramCount(&nProgramcount);
	if(nRC) return nRC;
	
	unsigned int i, j;
	int IndexAudioAnd = 0;
	int IndexVideoAnd = 0;
	int IndexSubAnd = 0;

	for(n = 0; n < (int)nProgramcount; n++)
	{
		nRC = GetProgramInfo(n, &pProgramInfo);
		if(nRC) continue;
		
		if(pProgramInfo->ppStreamInfo == NULL)
			continue;

		for(i = 0; i < pProgramInfo->uStreamCount; i++)
		{
			pStreamInfo = pProgramInfo->ppStreamInfo[i];
			
			if(pStreamInfo->ppTrackInfo == NULL)
				continue;

			for(j = 0; j < pStreamInfo->uTrackCount; j++)
			{
				pTrackInfo = pStreamInfo->ppTrackInfo[j];

				if(pTrackInfo == NULL)
					continue;

				if(pTrackInfo->uTrackType == VOOSMP_SS_AUDIO)
				{
					if((IndexAudioAnd & (1 << pTrackInfo->uTrackID)) == 0)
					{
						IndexAudioAnd |= (1 << pTrackInfo->uTrackID);
						m_pAudioTrackIndex[m_nAudioCount] = pTrackInfo->uTrackID;
						m_nAudioCount += 1;
					}
				}
				else if(pTrackInfo->uTrackType == VOOSMP_SS_VIDEO)
				{
					if((IndexVideoAnd & (1 << pTrackInfo->uTrackID)) == 0)
					{
						IndexVideoAnd |= (1 << pTrackInfo->uTrackID);
						m_pVideoTrackIndex[m_nVideoCount] = pTrackInfo->uTrackID;
						m_nVideoCount += 1;
					}
				}
				else if(pTrackInfo->uTrackType == VOOSMP_SS_SUBTITLE && m_nExtendSubtitle == 0)
				{
					if((IndexSubAnd & (1 << pTrackInfo->uTrackID)) == 0)
					{
						IndexSubAnd |= (1 << pTrackInfo->uTrackID);
						m_pSubtitleTrackIndex[m_nSubtitleCount] = pTrackInfo->uTrackID;
						m_nSubtitleCount += 1;
					}
				}
			}
		}
	}

	return VOOSMP_ERR_None;
}

int COSBaseSource::ClearPropertyItem(VOOSMP_SRC_TRACK_PROPERTY *pItemProperty)
{
	if(pItemProperty == NULL)
		return VOOSMP_ERR_None;

	if(pItemProperty->nPropertyCount > 0 && pItemProperty->ppItemProperties)
	{
		int n;

		for(n = 0; n < pItemProperty->nPropertyCount; n++)
		{
			if(pItemProperty->ppItemProperties[n])
			{
				if(pItemProperty->ppItemProperties[n]->pszProperty)
				{
					delete []pItemProperty->ppItemProperties[n]->pszProperty;
					pItemProperty->ppItemProperties[n]->pszProperty = NULL;
				}

				delete []pItemProperty->ppItemProperties[n];

				pItemProperty->ppItemProperties[n] = NULL;
			}
		}

		delete []pItemProperty->ppItemProperties;
	}

	memset(pItemProperty, 0, sizeof(VOOSMP_SRC_TRACK_PROPERTY));

	return 0;
}
