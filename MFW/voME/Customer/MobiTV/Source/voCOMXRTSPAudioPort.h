	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXRTSPAudioPort.h

	Contains:	voCOMXRTSPAudioPort header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXRTSPAudioPort_H__
#define __voCOMXRTSPAudioPort_H__

#include "voCOMXRTSPOutputPort.h"

class voCOMXRTSPAudioPort : public voCOMXRTSPOutputPort
{
public:
	voCOMXRTSPAudioPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex);
	virtual ~voCOMXRTSPAudioPort(void);

	virtual OMX_ERRORTYPE	SetTrack (CFileSource * pSource, OMX_S32 nTrackIndex);

	virtual OMX_ERRORTYPE	GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);

protected:
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer);

protected:
	OMX_AUDIO_PARAM_PCMMODETYPE		m_pcmType;
	VO_AUDIO_FORMAT					m_fmtAudio;

};

#endif //__voCOMXRTSPAudioPort_H__
