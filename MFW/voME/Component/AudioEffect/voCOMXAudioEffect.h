	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXAudioEffect.h

	Contains:	voCOMXAudioEffect header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-06-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXAudioEffect_H__
#define __voCOMXAudioEffect_H__

#include <voCOMXCompInplace.h>

class voCOMXAudioEffect : public voCOMXCompInplace
{
public:
	voCOMXAudioEffect(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXAudioEffect(void);

	virtual OMX_ERRORTYPE	GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nParamIndex,
										OMX_INOUT OMX_PTR pComponentParameterStructure);
	virtual OMX_ERRORTYPE	SetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentParameterStructure);

protected:
	virtual OMX_ERRORTYPE	InitPortType (void);


protected:
	virtual OMX_ERRORTYPE	ModifyBuffer (OMX_BUFFERHEADERTYPE * pBuffer);

protected:
	OMX_AUDIO_PARAM_PCMMODETYPE		m_pcmType;
	OMX_U32							m_nSampleRate;
	OMX_U32							m_nChannels;
	OMX_U32							m_nBits;


};

#endif //__voCOMXAudioEffect_H__
