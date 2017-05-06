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

	OMX_ERRORTYPE	SetNewCompState (OMX_STATETYPE sNewState, OMX_TRANS_STATE sTrans);

	OMX_ERRORTYPE	ReturnBuffer (OMX_BUFFERHEADERTYPE* pBuffer);

	OMX_ERRORTYPE RequestBuffer(const OMX_U32 type);  // 0: for HW;  1: for SW

	OMX_ERRORTYPE	Flush(void);

	OMX_ERRORTYPE SetFrameDropped() { m_bframedropped = true; }

	OMX_U32 GetBufferIndex(OMX_BUFFERHEADERTYPE *);
private:
	int m_nallocindex;
	bool m_bframedropped;
	voTIVideoRender*		m_pBufferAllocator;	
	void* m_pvideobuf[MAX_V4L2_BUFFER];
	voCOMXThreadMutex       m_mutlock;
};

#endif //__voCOMXTIOverlayPort_H__
