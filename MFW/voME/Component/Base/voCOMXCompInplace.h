	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCompInplace.h

	Contains:	voCOMXCompInplace header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXCompInplace_H__
#define __voCOMXCompInplace_H__

#include "voCOMXCompFilter.h"
#include "voCOMXPortInplace.h"

class voCOMXCompInplace : public voCOMXCompFilter
{
friend class voCOMXBasePort;
friend class voCOMXPortInplace;

public:
	voCOMXCompInplace(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXCompInplace(void);

public:
	virtual OMX_ERRORTYPE	SetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE FillThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

protected:
	virtual OMX_ERRORTYPE	CreatePorts (void);

	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);
	virtual OMX_ERRORTYPE	Flush (OMX_U32	nPort);

	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled);

	virtual OMX_ERRORTYPE	ModifyBuffer (OMX_BUFFERHEADERTYPE * pBuffer) = 0;

protected:
	OMX_U32							m_nInputBufferCount;
	OMX_BUFFERHEADERTYPE **			m_ppInputBufferHead;
};

#endif //__voCOMXCompInplace_H__