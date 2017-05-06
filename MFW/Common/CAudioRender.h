	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CAudioRender.h

	Contains:	CAudioRender header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CAudioRender_H__
#define __CAudioRender_H__

#include <voAudio.h>
#include <voAudioRender.h>

#include "CBaseNode.h"

class CAudioRender : public CBaseNode
{
public:
	CAudioRender (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP);
	virtual ~CAudioRender (void);

	virtual VO_U32		Init (VO_AUDIO_FORMAT * pFormat);
	virtual VO_U32		Uninit (void);

	virtual void		SetLibOperator (VO_LIB_OPERATOR * pLibOP);

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
	VO_AUDIO_RENDERAPI	m_funRender;
	VO_HANDLE			m_hRender;

	VO_S32				m_nBuffTime;
	VO_S64				m_nPlayTime;
};

#endif // __CAudioRender_H__
