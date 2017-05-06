	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CAudioEffect.h

	Contains:	CAudioEffect header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CAudioEffect_H__
#define __CAudioEffect_H__

#include "voAudio.h"
#include "CBaseNode.h"

class CAudioEffect : public CBaseNode
{
public:
	CAudioEffect (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP);
	virtual ~CAudioEffect (void);

	virtual VO_U32		Init (VO_AUDIO_FORMAT * pFormat);
	virtual VO_U32		Uninit (void);

	virtual VO_U32		Start(void);
	virtual VO_U32		Pause(void);
	virtual VO_U32		Stop(void);

	virtual VO_U32		SetInputData (VO_CODECBUFFER * pInput);
	virtual VO_U32		GetOutputData (VO_CODECBUFFER * pOutput, VO_AUDIO_FORMAT * pAudioFormat);
	virtual VO_U32		Flush (void);
	virtual VO_U32		Enable (VO_BOOL bEnable);

	virtual VO_U32		SetFormat (VO_AUDIO_FORMAT * pFormat);

	virtual VO_U32		SetParam (VO_S32 uParamID, VO_PTR pData);
	virtual VO_U32		GetParam (VO_S32 uParamID, VO_PTR pData);

	virtual VO_U32		GetFormat (VO_AUDIO_FORMAT * pFormat);

	virtual VO_U32		LoadLib (VO_HANDLE hInst);

protected:
	VO_AUDIO_CODECAPI	m_funEffect;
	VO_HANDLE			m_hEffect;

	VO_AUDIO_FORMAT		m_sFormat;

	VO_CODECBUFFER *	m_pInputData;
	VO_U32				m_nInputUsed;

	VO_AUDIO_OUTPUTINFO	m_OutputInfo;
};

#endif // __CAudioEffect_H__
