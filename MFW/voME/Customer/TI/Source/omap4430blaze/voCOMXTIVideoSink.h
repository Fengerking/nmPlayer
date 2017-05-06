	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXTIVideoSink.h

	Contains:	voCOMXTIVideoSink header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXTIVideoSink_H__
#define __voCOMXTIVideoSink_H__

#include <voOMX_Index.h>

#include <voCOMXCompSink.h>
#include <voCOMXPortClock.h>

#include "voCOMXTIOverlayPort.h"
#include "voTIVideoRender.h"

#define MAX_HELD 2

class voCOMXTIVideoSink : public voCOMXCompSink
{
public:
	voCOMXTIVideoSink(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXTIVideoSink(void);

	OMX_ERRORTYPE	GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nParamIndex,
										OMX_INOUT OMX_PTR pComponentParameterStructure);

	OMX_ERRORTYPE	SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentParameterStructure);

	OMX_ERRORTYPE	SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_INOUT OMX_PTR pComponentConfigStructure);

#if defined __VODBG__
	OMX_ERRORTYPE EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);
#endif
protected:
	OMX_ERRORTYPE	CreatePorts (void);
	OMX_U32			BufferHandle (void);
	OMX_ERRORTYPE	CreateRender (void);

	OMX_ERRORTYPE	Flush (OMX_U32	nPort);

	OMX_ERRORTYPE	InitPortType (void);

protected:
	OMX_BOOL		WaitForRenderTime(OMX_TICKS nTime, OMX_TICKS* pnCurrMediaTime);

	OMX_ERRORTYPE				FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer);
	inline int					RenderBuffer(OMX_BUFFERHEADERTYPE * pBuffer);

protected:
	voCOMXPortClock *				m_pClockPort;
	voCOMXTIOverlayPort*		m_pOverlayPort;
	void*										m_pview;
	voTIVideoRender*				m_pVideoRender;
	OMX_BUFFERHEADERTYPE*   m_pheldbuf;

	OMX_VIDEO_PORTDEFINITIONTYPE	m_videoType;
	OMX_VIDEO_PARAM_PORTFORMATTYPE 	m_videoFormat;

	VO_U32							m_nRendFrames;
	VO_U32							m_nRenderSpeed;
	VO_U32							m_nsleepcnt;
	VO_U32							m_nTotalTime;
	VO_U32							m_nlastsystime;
	OMX_TICKS						m_lllastframets;

	VO_U32							m_naveragedura;
	VO_U32							m_nDroppedFrame;
	VO_BOOL							m_bDropVideo;
	VO_BOOL							m_bDecodedSize;
	OMX_S64							m_llLastFrameTime;
#if defined __VODBG__
	VO_U32							m_nemptystart;
#endif
};


#endif //__voCOMXTIVideoSink_H__
