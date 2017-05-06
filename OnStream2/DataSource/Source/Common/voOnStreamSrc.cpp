#include "voOnStreamType.h"
#include "voOnStreamSource.h"
#include "COSSourceBase.h"
#include "voLog.h"

VO_LOG_PRINT_CB		g_OnStreamSourceVOLOGCB;

int voOSSourceCreate(void** phHandle)
{
	COSSourceBase * pSrc = 0;

	pSrc = new COSSourceBase();

	if(pSrc == 0)
		return VOOSMP_ERR_OutMemory;

	pSrc->SetParam (VOOSMP_PID_COMMON_LOGFUNC, &g_OnStreamSourceVOLOGCB);

	*phHandle = pSrc;

	return VOOSMP_ERR_None;
}

int voOSSourceInit(void* phHandle, void* pSource, int nSoureFlag, int nSourceType, void* pInitParam, int nInitParamFlag)
{
	COSSourceBase * pSrc = (COSSourceBase *)phHandle;
	if(pSrc == 0)
		return VOOSMP_ERR_Pointer;
    
	return pSrc->Init(pSource, nSoureFlag, nSourceType, pInitParam, nInitParamFlag);
}

int voOSSourceUninit(void* pHandle)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->Uninit();
}

int voOSSourceDestroy(void* pHandle)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	delete pSource;
	pHandle = 0;

	return VOOSMP_ERR_None;
}

int voOSSourceOpen(void* pHandle)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->Open ();
}

int voOSSourceClose(void* pHandle)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->Close ();
}

int voOSSourceRun (void* pHandle)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->Run ();
}

int voOSSourcePause (void* pHandle)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->Pause ();
}

int voOSSourceStop (void* pHandle)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->Stop();
}

int voOSSourceGetDuration (void* pHandle, long long* pDuration)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->GetDuration(pDuration);
}

int voOSSourceSetPos(void* pHandle, long long *pCurPos)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->SetCurPos(pCurPos);
}

int voOSSourceGetSample(void* pHandle, int nTrackType, void* pSample)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->GetSample(nTrackType, pSample);
}

int voOSSourceGetProgramCount(void* pHandle, int* pProgramCount)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->GetProgramCount((unsigned int *)pProgramCount);
}

int voOSSourceGetProgramInfo(void* pHandle, int nProgram, VOOSMP_SRC_PROGRAM_INFO** ppProgramInfo)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->GetProgramInfo(nProgram, ppProgramInfo);
}

int voOSSourceGetCurTrackInfo(void* pHandle, int nTrackType, VOOSMP_SRC_TRACK_INFO** ppTrackInfo)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->GetCurTrackInfo(nTrackType, ppTrackInfo);
}

int voOSSourceSelectProgram(void* pHandle, int nProgram)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->SelectProgram(nProgram);
}

int voOSSourceSelectStream(void* pHandle, int nStream)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->SelectStream(nStream);
}

int voOSSourceSelectTrack(void* pHandle, int nTrack)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->SelectTrack(nTrack);
}


int voOSSourceSendBuffer(void* pHandle, const VOOSMP_BUFFERTYPE& buffer)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->SendBuffer(buffer);
}

int voOSSourceGetParam (void* pHandle, int nID, void * pValue)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->GetParam(nID, pValue);
}

int voOSSourceSetParam (void* pHandle, int nID, void * pValue)
{
    if (nID == VOOSMP_PID_COMMON_LOGFUNC && pValue)
	{
		memcpy (&g_OnStreamSourceVOLOGCB, pValue, sizeof (g_OnStreamSourceVOLOGCB));
		
		VO_LOG_PRINT_CB * pVologCB = (VO_LOG_PRINT_CB *)pValue;
		//vologInit (pVologCB->pUserData, pVologCB->fCallBack);
	}
    
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->SetParam(nID, pValue);
}

int voOSSourceSelectLanguage (void* pHandle, int nIndex)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->SelectLanguage(nIndex);
}

int voOSSourceGetLanguage(void* pHandle, VOOSMP_SUBTITLE_LANGUAGE_INFO** ppLangInfo)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->GetSubLangInfo(ppLangInfo);
}

int voOSSourceGetVideoCount(void* pHandle)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->GetMediaCount(VOOSMP_SS_VIDEO);
}

int voOSSourceGetAudioCount (void* pHandle)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->GetMediaCount(VOOSMP_SS_AUDIO);
}
    

int voOSSourceGetSubtitleCount(void* pHandle)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->GetMediaCount(VOOSMP_SS_SUBTITLE);
}
    

int voOSSourceSelectVideo(void* pHandle, int nIndex)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->SelectMediaIndex(VOOSMP_SS_VIDEO, nIndex);
}
    
int voOSSourceSelectAudio (void* pHandle, int nIndex)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->SelectMediaIndex(VOOSMP_SS_AUDIO, nIndex);
}
    
int voOSSourceSelectSubtitle(void* pHandle, int nIndex)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->SelectMediaIndex(VOOSMP_SS_SUBTITLE, nIndex);
}
   
bool voOSSourceIsVideoAvailable (void* pHandle, int nIndex)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return false;

	if(pSource->IsMediaIndexAvailable(VOOSMP_SS_VIDEO, nIndex))
		return true;

	return false;
}   

bool voOSSourceIsAudioAvailable (void* pHandle, int nIndex)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return false;

	if(pSource->IsMediaIndexAvailable(VOOSMP_SS_AUDIO, nIndex))
		return true;

	return false;
}

bool voOSSourceIsSubtitleAvailable (void* pHandle, int nIndex)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return false;

	if(pSource->IsMediaIndexAvailable(VOOSMP_SS_SUBTITLE, nIndex))
		return true;

	return false;
}
    
int voOSSourceCommitSelection (void* pHandle)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->CommetSelection(1);
}  

int voOSSourceClearSelection (void* pHandle)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->CommetSelection(0);
}

int voOSSourceGetVideoProperty (void* pHandle, int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->GetMediaProperty(VOOSMP_SS_VIDEO, nIndex, ppProperty);
}

int voOSSourceGetAudioProperty (void* pHandle, int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->GetMediaProperty(VOOSMP_SS_AUDIO, nIndex, ppProperty);
}
    
int voOSSourceGetSubtitleProperty (void* pHandle, int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->GetMediaProperty(VOOSMP_SS_SUBTITLE, nIndex, ppProperty);
}    

int voOSSourceGetCurrTrackSelection (void* pHandle, VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->GetCurMediaTrack(pCurrIndex);
}

int voOSSourceGetCurrPlayingTrackIndex (void* pHandle, VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->GetCurPlayingMediaTrack(pCurrIndex);
}

int voOSSourceGetCurrSelectedTrackIndex (void* pHandle, VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
	COSSourceBase * pSource = (COSSourceBase *)pHandle;
	if(pSource == 0)
		return VOOSMP_ERR_Pointer;

	return pSource->GetCurSelectedMediaTrack(pCurrIndex);
}

int voGetOnStreamSourceAPI(voOSMPSourceAPI* pFuncSet)
{
	if(pFuncSet == 0)
		return VOOSMP_ERR_Pointer;

	pFuncSet->Create = voOSSourceCreate;
	pFuncSet->Init = voOSSourceInit;
	pFuncSet->Uninit = voOSSourceUninit;
	pFuncSet->Destroy = voOSSourceDestroy;
	pFuncSet->Open = voOSSourceOpen;
	pFuncSet->Close = voOSSourceClose;
	pFuncSet->Run = voOSSourceRun;
	pFuncSet->Pause = voOSSourcePause;
	pFuncSet->Stop = voOSSourceStop;
	pFuncSet->SetPos = voOSSourceSetPos;
	pFuncSet->GetDuration = voOSSourceGetDuration;
	pFuncSet->GetSample = voOSSourceGetSample;
	pFuncSet->GetProgramCount = voOSSourceGetProgramCount;
	pFuncSet->GetProgramInfo = voOSSourceGetProgramInfo;
	pFuncSet->GetCurTrackInfo = voOSSourceGetCurTrackInfo;
	pFuncSet->SelectProgram = voOSSourceSelectStream;
	pFuncSet->SelectStream = voOSSourceSelectStream;
	pFuncSet->SelectTrack = voOSSourceSelectTrack;
	pFuncSet->SendBuffer = voOSSourceSendBuffer;
	pFuncSet->SelectLanguage = voOSSourceSelectLanguage;
	pFuncSet->GetLanguage = voOSSourceGetLanguage;
	pFuncSet->GetParam = voOSSourceGetParam;
	pFuncSet->SetParam = voOSSourceSetParam;
	pFuncSet->GetVideoCount = voOSSourceGetVideoCount;
	pFuncSet->GetAudioCount = voOSSourceGetAudioCount;
	pFuncSet->GetSubtitleCount = voOSSourceGetSubtitleCount;
	pFuncSet->SelectVideo = voOSSourceSelectVideo;
	pFuncSet->SelectAudio = voOSSourceSelectAudio;
	pFuncSet->SelectSubtitle = voOSSourceSelectSubtitle;
	pFuncSet->IsVideoAvailable = voOSSourceIsVideoAvailable;
	pFuncSet->IsAudioAvailable = voOSSourceIsAudioAvailable;
	pFuncSet->IsSubtitleAvailable = voOSSourceIsSubtitleAvailable;
	pFuncSet->CommitSelection = voOSSourceCommitSelection;
	pFuncSet->ClearSelection = voOSSourceClearSelection;
	pFuncSet->GetVideoProperty = voOSSourceGetVideoProperty;
	pFuncSet->GetAudioProperty = voOSSourceGetAudioProperty;
	pFuncSet->GetSubtitleProperty = voOSSourceGetSubtitleProperty;
	pFuncSet->GetCurrTrackSelection = voOSSourceGetCurrTrackSelection;
	pFuncSet->GetCurrPlayingTrackIndex = voOSSourceGetCurrPlayingTrackIndex;
	pFuncSet->GetCurrSelectedTrackIndex = voOSSourceGetCurrSelectedTrackIndex;

	return VOOSMP_ERR_None;
}
