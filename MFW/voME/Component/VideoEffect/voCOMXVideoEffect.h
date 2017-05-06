	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXVideoEffect.h

	Contains:	voCOMXVideoEffect header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-06-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXVideoEffect_H__
#define __voCOMXVideoEffect_H__

#include <voCOMXCompInplace.h>

class voCOMXVideoEffect : public voCOMXCompInplace
{
public:
	voCOMXVideoEffect(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXVideoEffect(void);

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
	OMX_VIDEO_PORTDEFINITIONTYPE	m_videoType;
	OMX_VIDEO_PARAM_PORTFORMATTYPE 	m_videoFormat;
};

#endif //__voCOMXVideoEffect_H__
