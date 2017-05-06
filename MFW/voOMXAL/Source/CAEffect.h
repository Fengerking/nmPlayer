	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CAEffect.h

	Contains:	CAEffect header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CAudioEffect_H__
#define __CAudioEffect_H__

#include "voAudio.h"
#include "CBaseNode.h"
#include "voNPWrap.h"

class CAEffect : public CBaseNode
{
public:
	CAEffect (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP);
	virtual ~CAEffect (void);

	virtual VO_U32		Init (VO_AUDIO_FORMAT * pFormat);
	virtual VO_U32		Uninit (void);

	virtual VO_U32		Start(void);
	virtual VO_U32		Pause(void);
	virtual VO_U32		Stop(void);

	virtual VO_U32		SetInputData (VO_CODECBUFFER * pInput);
	virtual VO_U32		GetOutputData (VO_CODECBUFFER * pOutput, VO_AUDIO_FORMAT * pAudioFormat);
	virtual VO_U32		Flush (void);
	virtual VO_U32		Enable (VO_BOOL bEnable);
	virtual VO_BOOL		IsEnable();

	virtual VO_U32		SetFormat (VO_AUDIO_FORMAT * pFormat);

	virtual VO_U32		SetParam (VO_S32 uParamID, VO_PTR pData);
	virtual VO_U32		GetParam (VO_S32 uParamID, VO_PTR pData);

	virtual void		GetInputBuffer (VO_CODECBUFFER ** ppBuffer) {*ppBuffer = &m_sBuffer;}
	virtual VO_U32		GetFormat (VO_AUDIO_FORMAT * pFormat);

	virtual VO_U32		LoadLib (VO_HANDLE hInst);
	
	void				SetLib(VONP_LIB_FUNC* pLib){m_pLibFunc = pLib;};

protected:
	VO_AUDIO_CODECAPI	m_funEffect;
	VO_HANDLE			m_hEffect;

	VO_AUDIO_FORMAT		m_sFormat;
	VO_CODECBUFFER		m_sBuffer;

	VO_CODECBUFFER *	m_pInputData;
	VO_U32				m_nInputUsed;

	VO_AUDIO_OUTPUTINFO	m_OutputInfo;
	
	VONP_LIB_FUNC*      m_pLibFunc;
	
	VO_BOOL				m_bEnable;
};

#endif // __CAudioEffect_H__
