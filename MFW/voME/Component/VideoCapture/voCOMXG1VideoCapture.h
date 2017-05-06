	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXG1VideoCapture.h

	Contains:	voCOMXG1VideoCapture header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXG1VideoCapture_H__
#define __voCOMXG1VideoCapture_H__

#include "voCOMXCompSource.h"
#include "voCOMXG1VideoCapPort.h"

class voCOMXG1VideoCapture : public voCOMXCompSource
{
public:
	voCOMXG1VideoCapture(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXG1VideoCapture(void);

	virtual OMX_ERRORTYPE GetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nParamIndex,  
										OMX_INOUT OMX_PTR pComponentParameterStructure);


	virtual OMX_ERRORTYPE SetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE GetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex, 
										OMX_INOUT OMX_PTR pComponentConfigStructure);


	virtual OMX_ERRORTYPE SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex, 
										OMX_IN  OMX_PTR pComponentConfigStructure);

protected:
	virtual OMX_ERRORTYPE	CreatePorts (void);
	virtual OMX_ERRORTYPE	InitPortType (void);


protected:
	voCOMXG1VideoCapPort *		m_pVideoPort;

};

#endif //__voCOMXG1VideoCapture_H__
