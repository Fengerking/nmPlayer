/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		RenderCtrl.cpp

Contains:	RenderCtrl class file

Written by:	Leon Huang

Change History (most recent first):
2011-01-16		Leon			Create file
*******************************************************************************/
#include "RenderCtrl.h"

#include "CWaveOutRender.h"
#include "CGDIVideoRender.h"

CRenderCtrl::CRenderCtrl(void)
:m_pAudioRender(NULL)
,m_pVideoRender(NULL)
,m_videoHeight(0)
,m_videoWidth(0)
,m_nAudioChannels(0)
,m_nAudioSampleBits(0)
,m_nAudioSampleRate(0)
{
}

CRenderCtrl::~CRenderCtrl(void)
{
	UnInit();
}
VO_VOID CRenderCtrl::UnInit()
{
	if(m_pVideoRender) {delete m_pVideoRender; m_pVideoRender = NULL;m_videoWidth = 0;m_videoHeight = 0;}
	if(m_pAudioRender){m_pAudioRender->Stop(); delete m_pAudioRender; m_pAudioRender = NULL; m_nAudioChannels = 0;m_nAudioSampleBits = 0;m_nAudioSampleRate = 0;}

}

VO_S32 CRenderCtrl::ToRender(VO_S32 isVideo,void* pBuffer,VO_S32 nSize,VO_U64 nStart,VO_BOOL bWait)
{
	if(isVideo) 
	{	if(m_pVideoRender) m_pVideoRender->Render((VO_VIDEO_BUFFER*)pBuffer,nStart, bWait);}
	else
	{	if(m_pAudioRender) m_pAudioRender->Render((VO_PBYTE)pBuffer,nSize,nStart,bWait);}

	return 0;
}

VO_S32 CRenderCtrl::Init(HWND hWnd,int videoWidth,int videoHeight)
{
	if(m_videoWidth !=videoWidth ||  m_videoHeight!=videoHeight)
	{
		m_videoWidth = videoWidth;
		m_videoHeight = videoHeight;
		if(m_pVideoRender ){delete m_pVideoRender; m_pVideoRender = NULL;}
	
		VOMP_RECT rcDraw;
		GetClientRect (hWnd, (LPRECT)&rcDraw);
		rcDraw.bottom = rcDraw.bottom -26;
		
		m_pVideoRender = new CGDIVideoRender (NULL, hWnd, NULL);
		m_pVideoRender->SetVideoInfo (m_videoWidth,m_videoHeight, VO_COLOR_RGB565_PACKED);
		m_pVideoRender->SetDispRect (hWnd, (VO_RECT *)&rcDraw);
	}
	return 0;

}
VO_S32 CRenderCtrl::Init(VO_AUDIO_FORMAT *aformat)
{
	if(aformat->Channels != m_nAudioChannels || aformat->SampleBits != m_nAudioSampleBits || aformat->SampleRate != m_nAudioSampleRate)
	{
		m_nAudioChannels = aformat->Channels;
		m_nAudioSampleBits = aformat->SampleBits;
		m_nAudioSampleRate = aformat->SampleRate;

		if(m_pAudioRender){m_pAudioRender->Stop(); delete m_pAudioRender; m_pAudioRender = NULL;}

		m_pAudioRender = new CWaveOutRender (NULL, NULL);
		m_pAudioRender->SetFormat ((VO_AUDIO_FORMAT *)aformat);
		m_pAudioRender->Start ();
	}
	

	return 0;

}