	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXVideoResize.h

	Contains:	voCOMXVideoResize header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXVideoResize_H__
#define __voCOMXVideoResize_H__

#include "CVideoResize.h"
#include "voCOMXCompFilter.h"

class voCOMXVideoResize : public voCOMXCompFilter
{
public:
	voCOMXVideoResize(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXVideoResize(void);

public:
	virtual OMX_ERRORTYPE	GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE	SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure);

protected:
	virtual OMX_ERRORTYPE	InitPortType (void);
	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied, OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled);
	virtual OMX_ERRORTYPE	CreateVideoResize (void);

protected:
	CVideoResize *					m_pVideoRisize;
	VO_MEM_OPERATOR *				m_pMemOP;

	OMX_VIDEO_PORTDEFINITIONTYPE	m_inputPortDefinition;
	OMX_VIDEO_PORTDEFINITIONTYPE	m_outputPortDefinition;

	VO_VIDEO_FORMAT					m_inputFormat;
	VO_VIDEO_FORMAT					m_outputFormat;

	VO_VIDEO_BUFFER					m_inputBuffer;
	VO_VIDEO_BUFFER					m_outputBuffer;
};

#endif //__voCOMXVideoResize_H__
