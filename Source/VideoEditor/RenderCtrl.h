/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		RenderCtrl.h

Contains:	RenderCtrl header file

Written by:	Leon Huang

Change History (most recent first):
2011-01-16		Leon			Create file
*******************************************************************************/
#ifndef __RenderCtrl_H__
#define __RenderCtrl_H__
#pragma once

#include "CBaseAudioRender.h"
#include "CBaseVideoRender.h"
#include "videoEditorType.h"
class CRenderCtrl
{
public:
	CRenderCtrl(void);
	~CRenderCtrl(void);
private:
	CBaseAudioRender *		m_pAudioRender;
	CBaseVideoRender *		m_pVideoRender;
	int m_videoWidth;
	int m_videoHeight;

	int m_nAudioChannels;
	int m_nAudioSampleRate;
	int m_nAudioSampleBits;
public:
	VO_S32 CRenderCtrl::Init(VO_AUDIO_FORMAT *aformat);
	VO_S32 CRenderCtrl::Init(HWND hWnd,int videoWidth,int videoHeight);


	VO_S32 ToRender(VO_S32 isVideo,void* pBuffer,VO_S32 nSize,VO_U64 nStart,VO_BOOL bWait);
	//VO_S32 SetRenderParam(VO_S32 isVideo,void *param);
	VO_VOID UnInit();
};

#endif