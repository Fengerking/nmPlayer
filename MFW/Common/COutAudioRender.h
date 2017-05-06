	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		COutAudioRender.h

	Contains:	COutAudioRender header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __COutAudioRender_H__
#define __COutAudioRender_H__

#include <voAudio.h>
#include "CAudioRender.h"

#include "voCMutex.h"

class COutAudioRender : public CAudioRender
{
public:
	COutAudioRender (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP);
	virtual ~COutAudioRender (void);

	virtual VO_U32		Init (VO_AUDIO_FORMAT * pFormat);
	virtual VO_U32		Uninit (void);

	virtual VO_U32		SetFormat(VO_AUDIO_FORMAT * pFormat);
	virtual VO_U32		Start(void);
	virtual VO_U32		Pause(void);
	virtual VO_U32		Stop(void);
	virtual VO_U32		Render(VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart);
	virtual VO_U32		Flush(void);
	virtual VO_S64		GetPlayingTime(void);
	virtual VO_S32		GetBufferTime(void);
	virtual VO_U32		SetCallBack(VOAUDIOCALLBACKPROC pCallBack, VO_PTR pUserData);
	virtual VO_U32		SetParam(VO_U32 nID, VO_PTR pValue);
	virtual VO_U32		GetParam(VO_U32 nID, VO_PTR pValue);

	virtual VO_U32		LoadLib (VO_HANDLE hInst);

protected:
	VO_AUDIO_FORMAT		m_fmtAudio;
	VO_U64				m_nMediaTime;
	VO_U64				m_nClockTime;
	VO_BOOL				m_bWallClock;

	VOAUDIOCALLBACKPROC m_fCallBack;
	VO_PTR				m_pUserData;

	VO_CODECBUFFER		m_Buffer;

	voCMutex			m_mutex;
};

#endif // __COutAudioRender_H__
