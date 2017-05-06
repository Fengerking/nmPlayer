	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		androidVideoRender.cpp

	Contains:	androidVideoRender class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2011-06-08		JBF			Create file

*******************************************************************************/
#include "voAndroidVR.h"
#include "CSurfaceVideoRender.h"

#ifndef LOG_TAG
#define LOG_TAG "androidVideoRender"
#endif // LOG_TAG

VOOSMP_AUDIO_RENDER_FORMAT audioRenderFormat;
#include "voLog.h"
#include <media/AudioSystem.h>

using namespace android;

int voavrInit (void ** phRender, void * hSurface, int nFlag)
{
        VOLOGI("here into %s",__func__);
	if (phRender == NULL)
		return -1;

	*phRender = NULL;
	CSurfaceVideoRender * pRender = new CSurfaceVideoRender (hSurface, nFlag);
	if (pRender == NULL)
		return -1;

	*phRender = pRender;
        VOLOGI("here init yuvrender success+++++++++++++++++++++");

	return 0;
}

int voavrLock (void * hRender, VOMP_VIDEO_BUFFER * pVideoInfo, VOMP_RECT * pVideoSize)
{
	if (hRender == NULL)
		return -1;

	CSurfaceVideoRender * pRender = (CSurfaceVideoRender *)hRender;

	return pRender->Lock (pVideoInfo, pVideoSize);
}

int voavrUnLock (void * hRender)
{
	if (hRender == NULL)
		return -1;

	CSurfaceVideoRender * pRender = (CSurfaceVideoRender *)hRender;

	return pRender->UnLock ();
}

int voavrGetParam (void * hRender, int nID, void * pValue)
{
	if (hRender == NULL)
		return -1;

	CSurfaceVideoRender * pRender = (CSurfaceVideoRender *)hRender;

	return pRender->GetParam (nID, pValue);
}

int voavrSetParam (void * hRender, int nID, void * pValue)
{
	if (hRender == NULL)
		return -1;

	CSurfaceVideoRender * pRender = (CSurfaceVideoRender *)hRender;

	return pRender->SetParam (nID, pValue);
}

int voavrUninit (void * hRender)
{
	if (hRender == NULL)
		return -1;

	CSurfaceVideoRender * pRender = (CSurfaceVideoRender *)hRender;
	delete pRender;

	return 0;
}

int voavrSetAudioInfo(void * pValue)
{
	if(NULL == pValue)
		return -1;

	audioRenderFormat.SampleRate = ((VOOSMP_AUDIO_RENDER_FORMAT*)pValue)->SampleRate;
	audioRenderFormat.Channels = ((VOOSMP_AUDIO_RENDER_FORMAT*)pValue)->Channels;
	audioRenderFormat.SampleBits = ((VOOSMP_AUDIO_RENDER_FORMAT*)pValue)->SampleBits;
	audioRenderFormat.BufferSize = ((VOOSMP_AUDIO_RENDER_FORMAT*)pValue)->BufferSize;

	VOLOGI("Set audio info SampleRate %d Channels %d SampleBits %d BufferSize %d",
		audioRenderFormat.SampleRate, audioRenderFormat.Channels, audioRenderFormat.SampleBits, audioRenderFormat.BufferSize);

	return 0;
}

int voavrGetAudioBufTime (void * pValue)
{
	if(NULL == pValue)
		return -1;

	uint32_t afLatency = 0;
	int stream_type = -1;
#if defined __VONJ_CUPCAKE__
#else
	stream_type = AudioSystem::MUSIC;
#endif

	if (AudioSystem::getOutputLatency(&afLatency, stream_type) != NO_ERROR)
	{
		VOLOGE("Cannot get audio latency from audio system !");
		return -1;
	}
	else if(0 != audioRenderFormat.SampleRate && 0 != audioRenderFormat.Channels && 0 != audioRenderFormat.SampleBits)
	{
		*(int*)pValue = audioRenderFormat.BufferSize * 1000 / (audioRenderFormat.SampleRate * audioRenderFormat.Channels * audioRenderFormat.SampleBits / 8) + afLatency;
		VOLOGI("Get audio latency succeed latency %d buffer time %d!", afLatency, *(int*)pValue);
	}
	return 0;
}


int voavrGetFuncSet (VOAVR_FUNCSET * pFuncSet)
{
	VOLOGI("here into %s",__func__);

	memset(&audioRenderFormat, 0, sizeof(VOOSMP_AUDIO_RENDER_FORMAT));
	if (pFuncSet == NULL)
		return -1;
	
	pFuncSet->Init = voavrInit;
	pFuncSet->Lock = voavrLock;
	pFuncSet->UnLock = voavrUnLock;
	pFuncSet->GetParam = voavrGetParam;
	pFuncSet->SetParam = voavrSetParam;
	pFuncSet->Uninit = voavrUninit;
	pFuncSet->SetAudioInfo = voavrSetAudioInfo;
	pFuncSet->GetAudioBufTime = voavrGetAudioBufTime;
	
	VOLOGI("current load lib success");

	return 0;
}
