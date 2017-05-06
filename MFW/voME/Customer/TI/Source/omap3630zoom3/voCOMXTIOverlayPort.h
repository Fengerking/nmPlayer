	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXTIOverlayPort.h

	Contains:	voCOMXTIOverlayPort header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXTIOverlayPort_H__
#define __voCOMXTIOverlayPort_H__

#include "voCOMXBasePort.h"
#include "voTIVideoRender.h"

class voCOMXVideoSink;

class voCOMXTIOverlayPort : public voCOMXBasePort
{
friend class voCOMXTIVideoSink;

public:
	voCOMXTIOverlayPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex, OMX_DIRTYPE dirType);
	virtual ~voCOMXTIOverlayPort(void);

	OMX_ERRORTYPE	AllocBuffer (OMX_BUFFERHEADERTYPE** pBuffer, OMX_U32 nPortIndex,
										 OMX_PTR pAppPrivate,OMX_U32 nSizeBytes);

	OMX_ERRORTYPE FreeBuffer (OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE* pBuffer);

	OMX_ERRORTYPE	AllocTunnelBuffer (OMX_U32 nPortIndex, OMX_U32 nSizeBytes);

	OMX_ERRORTYPE	SetNewCompState (OMX_STATETYPE sNewState, OMX_TRANS_STATE sTrans);

	OMX_ERRORTYPE	ReturnBuffer (OMX_BUFFERHEADERTYPE* pBuffer);

	OMX_ERRORTYPE	Flush(void);

	OMX_ERRORTYPE SetFrameDropped(const int type = 1) { m_nframedropped = type; }

	OMX_U32 GetBufferIndex(OMX_BUFFERHEADERTYPE *);
private:
	OMX_BUFFERHEADERTYPE* m_pheld;
	voTIVideoRender*		m_pBufferAllocator;	
	int m_nheld;
	int m_nframedropped;
	voCOMXThreadMutex       m_mutlock;
};

#endif //__voCOMXTIOverlayPort_H__
