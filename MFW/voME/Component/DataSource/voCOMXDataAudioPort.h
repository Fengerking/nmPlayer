	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXDataAudioPort.h

	Contains:	voCOMXDataAudioPort header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXDataAudioPort_H__
#define __voCOMXDataAudioPort_H__

#include "voCOMXDataOutputPort.h"

class voCOMXDataAudioPort : public voCOMXDataOutputPort
{
public:
	voCOMXDataAudioPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex);
	virtual ~voCOMXDataAudioPort(void);

	virtual	void			SetConfigFile (CBaseConfig * pCfgFile);

	virtual OMX_ERRORTYPE	GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);
	virtual OMX_ERRORTYPE	SetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);

	virtual OMX_ERRORTYPE	SendBuffer (OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE	Flush (void);
	virtual OMX_ERRORTYPE   FlushBuffer(void);

	void	GetPCMType(OMX_AUDIO_PARAM_PCMMODETYPE **ppPCMType ) {*ppPCMType = &m_pcmType;}

protected:
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer);
	virtual OMX_ERRORTYPE	ReconfigPort (VO_LIVESRC_TRACK_INFOEX * pInfoEx);

protected:
	OMX_AUDIO_PARAM_PCMMODETYPE		m_pcmType;

	OMX_BOOL						mbReceivedEOS;

	OMX_BOOL						m_bBuffering;
	OMX_BOOL						mbUnderflow;
	OMX_S32							mnBufPercent;
	OMX_S32							mnBufPosPercent;
	OMX_BOOL						mbSendPercent0;
	OMX_U32							mnOutLog;
};

#endif //__voCOMXDataAudioPort_H__
