
/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
	File:		vome2CAudioRender.cpp

	Contains:	vome2CAudioRender class file

	Written by:	Tom Yu Wei 

	Change History (most recent first):
	2010-09-25		Bang			Create file

*******************************************************************************/
/*
#include <utils/Log.h>
#include <sys/prctl.h>
#include <sys/resource.h>

#include <cutils/properties.h>
#include <media/MediaPlayerInterface.h>
*/

#include "vome2CAudioRender.h"
#include "CAudioUnitRender.h"

#define LOG_TAG "vome2CAudioRender"
#include "voLog.h"

vome2CAudioRender::vome2CAudioRender(vompCEngine * pEngine)
	: vome2CBaseRender (pEngine)
	, m_pAudioRender (NULL)
	, m_dLeftVolume (1.0)
	, m_dRightVolume (1.0)
	, m_pAudioBuffer (NULL)
	, m_uRenderSize (0)
	, m_bSetBufferTime (true)
{
	strcpy (m_szThreadName, "vome2AudioRender");
}

vome2CAudioRender::~vome2CAudioRender()
{
	if (m_pAudioRender != NULL)
	{
		m_pAudioRender->Stop ();
		delete m_pAudioRender;
		m_pAudioRender = NULL;
	}
}

int	vome2CAudioRender::Start (void)
{
	if (m_pAudioRender != NULL)
		m_pAudioRender->Start ();

	vome2CBaseRender::Start ();

	return 0;
}

int vome2CAudioRender::Pause (void)
{
	vome2CBaseRender::Pause ();

	if (m_pAudioRender != NULL)
		m_pAudioRender->Pause ();

	return 0;
}

int vome2CAudioRender::Stop (void)
{
	vome2CBaseRender::Stop ();

	if (m_pAudioRender != NULL)
	{
		m_pAudioRender->Stop ();
		delete m_pAudioRender;
		m_pAudioRender = NULL;
	}

	return 0;
}

int vome2CAudioRender::Flush (void)
{
	if (m_pAudioRender != NULL)
		m_pAudioRender->Flush ();

	m_uRenderSize = 0;
	return 0;
}

int vome2CAudioRender::SetVolume (float leftVolume, float rightVolume)
{
	m_dLeftVolume = leftVolume;
	m_dRightVolume = rightVolume;

	//if (m_pAudioRender != NULL)
		//m_pAudioRender->setVolume(m_dLeftVolume, m_dRightVolume);

	return 0;
}

void vome2CAudioRender::initAudioRender()
{
	VOMP_AUDIO_FORMAT		sAudioFormat;
	memset (&sAudioFormat, 0, sizeof (VOMP_AUDIO_FORMAT));
	m_pEngine->GetParam (VOMP_PID_AUDIO_FORMAT, &sAudioFormat);
	
	Open (sAudioFormat.SampleRate, sAudioFormat.Channels, sAudioFormat.SampleBits, 2);
	
	if (m_pAudioRender != NULL)
		m_pAudioRender->Start ();
	
	m_bSetBufferTime = true;
}

int vome2CAudioRender::Open (int sampleRate, int channelCount, int format, int bufferCount)
{
	VOLOGI("open samplerate: %d channelcount:%d format:%d buffercount%d ",	sampleRate, channelCount, format, bufferCount );

	if (m_pAudioRender != NULL)
	{
		m_pAudioRender->Stop ();
		delete m_pAudioRender;
	}
	
	m_pAudioRender = new CAudioUnitRender(NULL, NULL);
	VO_AUDIO_FORMAT		sAudioFormat;
	sAudioFormat.SampleBits = format;
	sAudioFormat.Channels	= channelCount;
	sAudioFormat.SampleRate	= sampleRate;
	m_pAudioRender->SetFormat (&sAudioFormat);
	return 0;
}

int vome2CAudioRender::vome2RenderThreadLoop (void)
{
	m_pAudioBuffer = NULL;
	int nRC = m_pEngine->GetParam (VOMP_PID_AUDIO_SAMPLE, &m_pAudioBuffer);
	if (nRC == VOMP_ERR_None && m_pAudioBuffer != NULL)
	{
		if (m_pAudioRender == NULL)
		{
			initAudioRender();

			if (m_pAudioRender != NULL)
				m_pAudioRender->Start ();
		}

		if (m_pAudioRender != NULL) {
			m_pAudioRender->Render (m_pAudioBuffer->pBuffer, m_pAudioBuffer->nSize, 0, VO_TRUE);
			
			if (m_bSetBufferTime && (NULL != m_pEngine)) {
				m_bSetBufferTime = false;
				VO_S32 buffTime = 0;
				m_pAudioRender->GetBufferTime(&buffTime);
				m_pEngine->SetParam(VOMP_PID_AUDIO_SINK_BUFFTIME, &buffTime);
			}
		}
	}
	else if (nRC == VOMP_ERR_FormatChange)
	{
		initAudioRender();
	}
	else if (nRC == VOMP_ERR_Retry)
	{
		usleep(5000);
	}
	else if(nRC == VOMP_ERR_WaitTime)
	{
		usleep(1000);
	}
	else
	{
		usleep (1000);
	}

	return 0;
}

void vome2CAudioRender::vome2RenderThreadStart (void)
{
	//int ret = setpriority(PRIO_PROCESS, 0, ANDROID_PRIORITY_AUDIO);	
	//int ret = setpriority(PRIO_DARWIN_THREAD, 0, 20);	
}

