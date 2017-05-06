	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXRTSPVideoPort.h

	Contains:	voCOMXRTSPVideoPort header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXRTSPVideoPort_H__
#define __voCOMXRTSPVideoPort_H__

#include "voCOMXRTSPOutputPort.h"

class voCOMXPortClock;

class voCOMXRTSPVideoPort : public voCOMXRTSPOutputPort
{
public:
	voCOMXRTSPVideoPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex);
	virtual ~voCOMXRTSPVideoPort(void);

	virtual OMX_ERRORTYPE	Flush (void);


	virtual OMX_ERRORTYPE	SetTrack (CFileSource * pSource, OMX_S32 nTrackIndex);

	virtual OMX_ERRORTYPE	SetClockPort (voCOMXPortClock * pClock);

	virtual OMX_ERRORTYPE	GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);

	virtual OMX_ERRORTYPE	SetTrackPos (OMX_S64 * pPos);


protected:
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer);

protected:
	voCOMXPortClock *	m_pClockPort;

protected:
	VO_VIDEO_FORMAT		m_fmtVideo;

	VO_U32				m_nReadSamples;
	VO_U32				m_nReadResult;

	OMX_S64				m_nSeekPos;
};

#endif //__voCOMXRTSPVideoPort_H__
