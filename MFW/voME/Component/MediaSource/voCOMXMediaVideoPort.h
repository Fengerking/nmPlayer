	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXMediaVideoPort.h

	Contains:	voCOMXMediaVideoPort header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXMediaVideoPort_H__
#define __voCOMXMediaVideoPort_H__

#include <voCOMXMediaOutputPort.h>

class voCOMXPortClock;

class voCOMXMediaVideoPort : public voCOMXMediaOutputPort
{
public:
	voCOMXMediaVideoPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex);
	virtual ~voCOMXMediaVideoPort(void);

	virtual OMX_ERRORTYPE	Flush (void);

	virtual OMX_ERRORTYPE	SetTrackInfo (OMX_S32 nTrackIndex, VO_SOURCE_TRACKINFO *pTrackInfo);

	virtual OMX_ERRORTYPE	SetClockPort (voCOMXPortClock * pClock);

	virtual OMX_ERRORTYPE	GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);

protected:
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer);

protected:
	voCOMXPortClock *	m_pClockPort;

protected:
	VO_VIDEO_FORMAT		m_fmtVideo;

	VO_U32				m_nReadSamples;


};

#endif //__voCOMXMediaVideoPort_H__
