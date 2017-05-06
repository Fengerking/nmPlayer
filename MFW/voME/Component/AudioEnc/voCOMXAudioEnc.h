	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXAudioEnc.h

	Contains:	voCOMXAudioEnc header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#ifndef __voCOMXAudioEnc_H__
#define __voCOMXAudioEnc_H__

#include "voCOMXCompFilter.h"
#include "CAudioEncoder.h"

class voCOMXAudioEnc : public voCOMXCompFilter
{
public:
	voCOMXAudioEnc(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXAudioEnc(void);

public:
	virtual OMX_ERRORTYPE	GetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nParamIndex,  
										OMX_INOUT OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE	SetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE	ComponentRoleEnum (OMX_IN OMX_HANDLETYPE hComponent,
												OMX_OUT OMX_U8 *cRole,
												OMX_IN OMX_U32 nIndex);

protected:
	virtual OMX_ERRORTYPE	InitPortType (void);
	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled);

protected:
	OMX_AUDIO_CODINGTYPE			m_nCoding;
	OMX_PTR							m_pFormatData;
	OMX_AUDIO_PARAM_PCMMODETYPE		m_pcmType;

	CAudioEncoder *					m_pAudioEnc;
	VO_MEM_OPERATOR *				m_pMemOP;

	VO_CODECBUFFER					m_inBuffer;
	VO_CODECBUFFER					m_outBuffer;
	VO_S64							m_nStartTime;
	VO_AUDIO_OUTPUTINFO				m_outInfo;

};

#endif //__voCOMXAudioEnc_H__
