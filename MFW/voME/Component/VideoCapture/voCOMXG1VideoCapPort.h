	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXG1VideoCapPort.h

	Contains:	voCOMXG1VideoCapPort header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXG1VideoCapPort_H__
#define __voCOMXG1VideoCapPort_H__

#include "voCOMXPortSource.h"
#include "CBaseConfig.h"

class voCOMXG1VideoCapPort : public voCOMXPortSource
{
public:
	voCOMXG1VideoCapPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex);
	virtual ~voCOMXG1VideoCapPort(void);

	virtual OMX_ERRORTYPE	GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);
	virtual OMX_ERRORTYPE	SetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);

	virtual OMX_ERRORTYPE	SetNewCompState (OMX_STATETYPE sNew, OMX_TRANS_STATE sTrans);
	virtual OMX_ERRORTYPE	Flush (void);

protected:
	virtual OMX_U32			BufferHandle (void);
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer);

protected:
	OMX_VIDEO_PORTDEFINITIONTYPE	m_videoType;
	OMX_U32							m_nSourceType;

	OMX_BUFFERHEADERTYPE *			m_pSendBuffer;
	voCOMXThreadSemaphore			m_tsSendBuffer;
	voCOMXThreadMutex				m_tmSendBuffer;

	OMX_U32							m_nEncFrames;
	OMX_S32							m_nFrameRate;
	OMX_S64							m_llFirstTime;
	OMX_S64							m_llFrameTime;

	CBaseConfig *					m_pCfgComponent;
};

#endif //__voCOMXG1VideoCapPort_H__
