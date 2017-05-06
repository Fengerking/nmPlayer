	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXAudioEQ.h

	Contains:	voCOMXAudioEQ header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXAudioEQ_H__
#define __voCOMXAudioEQ_H__

#include <voOMX_Index.h>

#include <voCOMXCompInplace.h>


class voCOMXAudioEQ : public voCOMXCompInplace
{
public:
	voCOMXAudioEQ(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXAudioEQ(void);

	virtual OMX_ERRORTYPE GetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nParamIndex,  
										OMX_INOUT OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE SetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentParameterStructure);

protected:
	virtual OMX_ERRORTYPE	InitPortType (void);

	virtual OMX_ERRORTYPE	ModifyBuffer (OMX_BUFFERHEADERTYPE * pBuffer);

protected:
	OMX_AUDIO_PARAM_PCMMODETYPE		m_pcmType;

	OMX_U32							m_nSampleRate;
	OMX_U32							m_nChannels;
	OMX_U32							m_nBits;
};

#endif //__voCOMXAudioEQ_H__