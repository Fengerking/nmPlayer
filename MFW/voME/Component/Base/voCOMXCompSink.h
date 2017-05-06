	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCompSink.h

	Contains:	voCOMXCompSink header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXCompSink_H__
#define __voCOMXCompSink_H__

#include "voCOMXBaseComponent.h"

class voCOMXCompSink : public voCOMXBaseComponent
{
public:
	voCOMXCompSink(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXCompSink(void);

	virtual OMX_ERRORTYPE GetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_INOUT OMX_PTR pComponentConfigStructure);

	virtual OMX_ERRORTYPE SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentConfigStructure);

protected:
	virtual OMX_ERRORTYPE	CreatePorts (void);
	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);

	virtual OMX_U32			BufferHandle (void);

	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer) = 0;

protected:
	voCOMXBasePort *		m_pInput;

	voCOMXTaskQueue *		m_pBufferQueue;
	voCOMXThreadSemaphore * m_pSemaphore;

	OMX_S64					m_llSeekTime;
	OMX_S64					m_llPlayTime;
	OMX_U32					m_nRenderFrames;

	voCOMXThreadMutex		m_mutRender;

	OMX_BOOL				m_bEOS;
};

#endif //__voCOMXCompSink_H__

