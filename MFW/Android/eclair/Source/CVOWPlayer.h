	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOWPlayer.h

	Contains:	CVOWPlayer header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-11-21		JBF			Create file

*******************************************************************************/

#ifndef __CVOWPlayer_H__
#define __CVOWPlayer_H__

#include "voMMPlay.h"
#include "CVOMEPlayer.h"

class CVOWPlayer : public CVOMEPlayer
{
public:
	CVOWPlayer (void);
	virtual ~CVOWPlayer (void);

	virtual int			Init (void);
	virtual int			PrepareAsync (void);

	virtual int			BuildGraph (void);
	virtual int			Run (bool bForceSeek = false);
	virtual int			Pause (void);
	virtual int			Stop (void);

	virtual int			SetPos (int	nPos);
	virtual int			GetPos (int * pPos);
	virtual int			GetDuration (int * pPos);

	virtual int			Reset (void);
	virtual int			Uninit (bool bSuspend = false);

protected:
	virtual VO_U32		MessageHandler (VO_U32 nID, VO_PTR pValue1, VO_PTR pValue2);
	virtual int			RenderVideo (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_FORMAT * pVideoSize);
	virtual int			RenderAudio (VO_CODECBUFFER * pAudioBuffer, VO_AUDIO_FORMAT * pAudioFormat);
	virtual int			LoadModule (void);

protected:
	VO_PTR				m_hVOWPlay;
	VOMM_PLAYAPI		m_fVOWAPI;

public:
	static VO_U32	vowMessageHandlerProc (VO_PTR pUserData, VO_U32 nID, VO_PTR pValue1, VO_PTR pValue2);
	static VO_S32	vowVideoRenderProc (VO_PTR pUserData, VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_FORMAT * pVideoSize, VO_S32 nStart);
	static VO_S32	vowAudioRenderProc (VO_PTR pUserData, VO_CODECBUFFER * pAudioBuffer, VO_AUDIO_FORMAT * pAudioFormat, VO_S32 nStart);
};

#endif // __CVOWPlayer_H__
