	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXG1AudioCapPort.h

	Contains:	voCOMXG1AudioCapPort header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXG1AudioCapPort_H__
#define __voCOMXG1AudioCapPort_H__

#include "voCOMXPortSource.h"

class voCOMXG1AudioCapPort : public voCOMXPortSource
{
public:
	voCOMXG1AudioCapPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex);
	virtual ~voCOMXG1AudioCapPort(void);

	virtual OMX_ERRORTYPE	GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);
	virtual OMX_ERRORTYPE	SetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);

	virtual OMX_ERRORTYPE	SetNewCompState (OMX_STATETYPE sNew, OMX_TRANS_STATE sTrans);
	virtual OMX_ERRORTYPE	Flush (void);

protected:
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer);

protected:
	OMX_AUDIO_PARAM_PCMMODETYPE		m_pcmType;

	OMX_BUFFERHEADERTYPE *			m_pSendBuffer;
	voCOMXThreadSemaphore			m_tsSendBuffer;
	voCOMXThreadMutex				m_tmSendBuffer;

};

#endif //__voCOMXG1AudioCapPort_H__
