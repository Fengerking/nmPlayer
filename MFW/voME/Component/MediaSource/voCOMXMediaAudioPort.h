	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXMediaAudioPort.h

	Contains:	voCOMXMediaAudioPort header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXMediaAudioPort_H__
#define __voCOMXMediaAudioPort_H__

#include <voCOMXMediaOutputPort.h>

class voCOMXMediaAudioPort : public voCOMXMediaOutputPort
{
public:
	voCOMXMediaAudioPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex);
	virtual ~voCOMXMediaAudioPort(void);

	virtual OMX_ERRORTYPE	SetTrackInfo (OMX_S32 nTrackIndex, VO_SOURCE_TRACKINFO *pTrackInfo);

	virtual OMX_ERRORTYPE	GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);

protected:
	OMX_AUDIO_PARAM_PCMMODETYPE		m_pcmType;
	VO_AUDIO_FORMAT					m_fmtAudio;

};

#endif //__voCOMXMediaAudioPort_H__
