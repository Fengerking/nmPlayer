	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXPortSource.h

	Contains:	voCOMXPortSource header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXPortSource_H__
#define __voCOMXPortSource_H__

#include "voCOMXThreadSemaphore.h"
#include "voOMXThread.h"
#include "voCOMXBasePort.h"

class voCOMXPortSource : public voCOMXBasePort
{
public:
	voCOMXPortSource(voCOMXBaseComponent * pParent, OMX_S32 nIndex);
	virtual ~voCOMXPortSource(void);

public:
	static OMX_U32			voPortBufferHandleProc (OMX_PTR pParam);

public:
	virtual OMX_ERRORTYPE	SetNewCompState (OMX_STATETYPE sNew, OMX_TRANS_STATE sTrans);
	virtual OMX_ERRORTYPE	ResetBuffer (OMX_BOOL bRelease);

protected:
	virtual OMX_ERRORTYPE	BeforeHandleBuffer (void);
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer);
	virtual OMX_ERRORTYPE	AfterHandleBuffer (void);

	virtual OMX_ERRORTYPE	ExitBufferThread (void);
	virtual OMX_U32			BufferHandle (void);

protected:
	voOMXThreadHandle				m_hBuffThread;
	OMX_U32							m_nBuffThreadID;
	OMX_BOOL						m_bBuffThreadStop;

	voCOMXThreadSemaphore			m_tsState;
	OMX_BOOL						m_bResetingBuff;

	OMX_BUFFERHEADERTYPE *			m_pWorkBuffer;
};

#endif //__voCOMXPortSource_H__
