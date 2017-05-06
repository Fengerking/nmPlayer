	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXZoom2VideoSink.h

	Contains:	voCOMXZoom2VideoSink header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXZoom2VideoSink_H__
#define __voCOMXZoom2VideoSink_H__

#include <voOMX_Index.h>
#include "CBaseConfig.h"

#include <voCOMXCompSink.h>
#include <voCOMXPortClock.h>

#include <CVideoRender.h>

typedef void (VO_API * VOPACKUVAPI) (unsigned char* dstUV, unsigned char* srcU, unsigned char* srcV,
									 int strideU, int strideV, int rows, int width, int stridedest);

class voCOMXZoom2VideoSink : public voCOMXCompSink
{
public:
	voCOMXZoom2VideoSink(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXZoom2VideoSink(void);

	virtual OMX_ERRORTYPE	GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nParamIndex,
										OMX_INOUT OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE	SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE	SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_INOUT OMX_PTR pComponentConfigStructure);

	virtual OMX_ERRORTYPE	EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
											 OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);


protected:
	virtual OMX_ERRORTYPE	CreatePorts (void);
	virtual OMX_ERRORTYPE	CreateRender (void);

	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);
	virtual OMX_ERRORTYPE	Flush (OMX_U32	nPort);

	virtual OMX_ERRORTYPE	InitPortType (void);

protected:
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer);
	virtual OMX_ERRORTYPE	RenderVideo (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait);

protected:
	voCOMXPortClock *				m_pClockPort;
	OMX_VIDEO_PORTDEFINITIONTYPE	m_videoType;
	OMX_VIDEO_PARAM_PORTFORMATTYPE 	m_videoFormat;


protected:
	OMX_VO_CHECKRENDERBUFFERTYPE	m_cbRenderBuffer;
	OMX_VO_VIDEOBUFFERTYPE			m_cbVideoBuffer;
	OMX_BUFFERHEADERTYPE *			m_pRenderBuffer;

	CVideoRender *					m_pVideoRender;
	VO_MEM_OPERATOR *				m_pMemOP;

	OMX_PTR							m_hView;
	VO_RECT							m_rcDisp;

	VO_VIDEO_BUFFER					m_videoBuffer;
	VO_U32							m_nYUVWidth;
	VO_U32							m_nYUVHeight;

	VO_PBYTE						m_pLogBuffer;

	VO_U32							m_nRendFrames;
	VO_U32							m_nRenderSpeed;
	VO_BOOL							m_nDroppedFrame;
	VO_U32							m_nPrevSystemTime;

	CBaseConfig *					m_pCfgComponent;
	CBaseConfig *					m_pCfgCodec;
	OMX_BOOL						m_bThread;

	VO_U32							m_nSysTime;

protected:
	static VO_S32	videosinkVideoRenderProc (VO_PTR pUserData, VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_FORMAT * pVideoFormat, VO_S32 nStart);
		   VO_S32	videosinkVideoRender (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_FORMAT * pVideoFormat, VO_S32 nStart);
};

#endif //__voCOMXZoom2VideoSink_H__
