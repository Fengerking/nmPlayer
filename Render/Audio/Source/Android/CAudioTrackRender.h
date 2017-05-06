	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CAudioTrackRender.h

	Contains:	CAudioTrackRender header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2012-05-13		JBF			Create file

*******************************************************************************/

#ifndef __CAudioTrackRender_H__
#define __CAudioTrackRender_H__

#include <stdio.h>

#include <sys/types.h>
#include <utils/Errors.h>

#include <sys/types.h>
#include <utils/Errors.h>
#include <media/AudioSystem.h>
#include <media/AudioTrack.h>

#include "CPtrList.h"
#include "voCMutex.h"

#include "../CBaseAudioRender.h"


class CAudioTrackRender : public CBaseAudioRender
{
public:
	CAudioTrackRender (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP);
	virtual ~CAudioTrackRender (void);

	virtual VO_U32 		SetFormat (VO_AUDIO_FORMAT * pFormat);
	virtual VO_U32 		Start (void);
	virtual VO_U32 		Pause (void);
	virtual VO_U32 		Stop (void);
	virtual VO_U32 		Render (VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart, VO_BOOL bWait);
	virtual VO_U32 		Flush (void);
	virtual VO_U32 		GetPlayingTime (VO_S64	* pPlayingTime);
	virtual VO_U32 		GetBufferTime (VO_S32	* pBufferTime);
	virtual VO_U32 		SetParam (VO_U32 nID, VO_PTR pValue);
	virtual VO_U32 		GetParam (VO_U32 nID, VO_PTR pValue);
	
protected:
	android::AudioTrack *		m_pTrack;
	voCMutex					m_mtTrack;
	
	int							m_nStreamType;
	int							m_lFrameSize;
	
	VO_AUDIO_FORMAT				m_fmtAudio;
	
	voCMutex					m_mtTime;	
	long long					m_llPlayingTime;
	long long					m_llBufferTime;
	long long					m_llSystemTime;
	
};


#endif // __CAudioTrackRender_H__