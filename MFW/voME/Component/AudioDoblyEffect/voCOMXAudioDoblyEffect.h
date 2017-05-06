	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXAudioDoblyEffect.h

	Contains:	voCOMXAudioDec header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXAudioDoblyEffect_H__
#define __voCOMXAudioDoblyEffect_H__

#include <voOMX_Index.h>

#include <voCOMXCompFilter.h>
#include "CBaseConfig.h"

#include "CAudioDEffect.h"

class voCOMXAudioDobyEffect : public voCOMXCompFilter
{
public:
	voCOMXAudioDobyEffect(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXAudioDobyEffect(void);

	virtual OMX_ERRORTYPE	GetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nParamIndex,  
										OMX_INOUT OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE	SetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE GetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
									OMX_IN  OMX_INDEXTYPE nIndex,
									OMX_INOUT OMX_PTR pComponentConfigStructure);

	virtual OMX_ERRORTYPE SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
									OMX_IN  OMX_INDEXTYPE nIndex,
									OMX_IN  OMX_PTR pComponentConfigStructure);

	virtual OMX_ERRORTYPE ComponentRoleEnum (OMX_IN OMX_HANDLETYPE hComponent,
											OMX_OUT OMX_U8 *cRole,
											OMX_IN OMX_U32 nIndex);

protected:
	virtual OMX_ERRORTYPE	InitPortType (void);

	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);
	virtual OMX_ERRORTYPE	Flush (OMX_U32	nPort);

protected:
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled);

protected:
	OMX_PTR							m_pFormatData;
	OMX_U32							m_nExtSize;
	OMX_S8 *						m_pExtData;

	OMX_AUDIO_PARAM_PCMMODETYPE		m_pcmType;

	OMX_U32							m_nSampleRate;
	OMX_U32							m_nChannels;
	OMX_U32							m_nInSampleRate;
	OMX_U32							m_nInChannels;
	OMX_U32							m_nOutSampleRate;
	OMX_U32							m_nOutChannels;
	OMX_U32							m_nBits;

	OMX_BOOL						m_bSetThreadPriority;

protected:
	CAudioDEffect				   *pCAudioDoblyEffect; 

	VO_CODECBUFFER					m_inBuffer;
	VO_CODECBUFFER					m_outBuffer;
	VO_S64							m_nStartTime;

	VO_U32							m_nStepSize;
	VO_U32							m_nBuffSize;
};

#endif //__voCOMXAudioDoblyEffect_H__
