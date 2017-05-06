	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXFileVideoPort.h

	Contains:	voCOMXFileVideoPort header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXFileVideoPort_H__
#define __voCOMXFileVideoPort_H__

#include "voCOMXFileOutputPort.h"
#include "fVideoHeadDataInfo.h"
#include "voVideo.h"

class voCOMXPortClock;
class voCOMXFileVideoPort : public voCOMXFileOutputPort
{
public:
	voCOMXFileVideoPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex);
	virtual ~voCOMXFileVideoPort(void);

	virtual OMX_ERRORTYPE	Flush (void);

	virtual OMX_ERRORTYPE	SetTrack (CBaseSource * pSource, OMX_S32 nTrackIndex);

	virtual OMX_ERRORTYPE	SetClockPort (voCOMXPortClock * pClock);

	virtual OMX_ERRORTYPE	GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);
	virtual OMX_ERRORTYPE	SetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);

	virtual OMX_ERRORTYPE	SetTrackPos (OMX_S64 * pPos);

	virtual void			SetSourceConfig (CBaseConfig * pConfig);

protected:
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer);
	virtual VO_VIDEO_FRAMETYPE	GetSampleFrameType(VO_PBYTE pSample, OMX_U32 nSize);
	virtual VO_VIDEO_FRAMETYPE	GetMpeg2FrameType(VO_PBYTE pSample, OMX_U32 nSize);
	virtual VO_VIDEO_FRAMETYPE	GetWMVFrameType(VO_PBYTE pSample, OMX_U32 nSize);

protected:
	voCOMXPortClock *	m_pClockPort;
	VO_VIDEO_FORMAT		m_fmtVideo;
	VO_U32				m_nReadSamples;
	VO_U32				m_nReadResult;

	OMX_U8*				m_pFrameHeader;
	OMX_U32				m_nFrameHeaderSize;

	// recode the format of I frame in a GOP
	VO_VIDEO_FORMAT		m_frameFmt;
	OMX_U32				m_nJumpState;
};

#endif //__voCOMXFileVideoPort_H__
