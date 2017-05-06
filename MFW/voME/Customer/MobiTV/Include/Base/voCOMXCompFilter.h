	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCompFilter.h

	Contains:	voCOMXCompFilter header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXCompFilter_H__
#define __voCOMXCompFilter_H__

#include "voCOMXBaseComponent.h"

class voCOMXCompFilter : public voCOMXBaseComponent
{
friend class voCOMXBasePort;

public:
	voCOMXCompFilter(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXCompFilter(void);

	virtual OMX_ERRORTYPE EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);
	virtual OMX_ERRORTYPE FillThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);
protected:
	virtual OMX_ERRORTYPE	CreatePorts (void);

	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);
	virtual OMX_ERRORTYPE	Flush (OMX_U32	nPort);
	virtual OMX_ERRORTYPE	DisablePort (OMX_U32 nPort);

	virtual OMX_U32			BufferHandle (void);

	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled) = 0;

protected:
	virtual void			ResetPortBuffer (void);

protected:
	voCOMXBasePort *		m_pInput;
	voCOMXBasePort *		m_pOutput;

	OMX_BUFFERHEADERTYPE *	m_pInputBuffer;
	OMX_BUFFERHEADERTYPE *	m_pOutputBuffer;

	voCOMXTaskQueue *		m_pInputQueue;
	voCOMXThreadSemaphore * m_pInputSem;

	voCOMXTaskQueue *		m_pOutputQueue;
	voCOMXThreadSemaphore * m_pOutputSem;

	OMX_S64					m_nOutBuffTime;
	OMX_U32					m_nOutBuffSize;

	OMX_BOOL				m_bInputEOS;
	OMX_BOOL				m_bResetBuffer;

	OMX_BOOL				m_bFlushed;

};

#endif //__voCOMXCompFilter_H__
