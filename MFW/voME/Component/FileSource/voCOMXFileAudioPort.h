	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXFileAudioPort.h

	Contains:	voCOMXFileAudioPort header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXFileAudioPort_H__
#define __voCOMXFileAudioPort_H__

#include "voCOMXFileOutputPort.h"

class voCOMXFileAudioPort : public voCOMXFileOutputPort
{
public:
	voCOMXFileAudioPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex);
	virtual ~voCOMXFileAudioPort(void);

public:
	virtual OMX_ERRORTYPE	SetTrack (CBaseSource * pSource, OMX_S32 nTrackIndex);

	virtual OMX_ERRORTYPE	GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);
	virtual OMX_ERRORTYPE	SetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);

protected:
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer);
	inline OMX_ERRORTYPE	DownMixBuffer (OMX_BUFFERHEADERTYPE * pBuffer);

protected:
	OMX_AUDIO_PARAM_PCMMODETYPE		m_pcmType;
	VO_AUDIO_FORMAT					m_fmtAudio;
	OMX_BOOL						m_bDownMix;
	OMX_U32							m_nBitrate;
};

#endif //__voCOMXFileAudioPort_H__
