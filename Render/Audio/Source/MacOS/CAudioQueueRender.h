/*
 *  CAudioQueueRender.h
 *  Audio Render on iOS
 *
 *  Created by Lin Jun on 11/18/10.
 *  Copyright 2010 VisualOn. All rights reserved.
 *
 */

#include "CBaseAudioRender.h"

#ifndef _CAUDIO_QUEUE_RENDER_H__
#define _CAUDIO_QUEUE_RENDER_H__

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif
    
class CWaveOutAudio;

class CAudioQueueRender : public CBaseAudioRender
{
public:
	CAudioQueueRender (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP);
	virtual ~CAudioQueueRender (void);
	
	virtual VO_U32 		SetFormat (VO_AUDIO_FORMAT * pFormat);
	virtual VO_U32 		Start (void);
	virtual VO_U32 		Pause (void);
	virtual VO_U32 		Stop (void);
	virtual VO_U32 		Render (VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart, VO_BOOL bWait);
	virtual VO_U32 		Flush (void);
	virtual VO_U32 		GetPlayingTime (VO_S64	* pPlayingTime);
	virtual VO_U32 		GetBufferTime (VO_S32	* pBufferTime);

	
private:
	CWaveOutAudio*		m_pRender;
};

#ifdef _VONAMESPACE
}
#endif

#endif