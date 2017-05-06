	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXVideoSink.h

	Contains:	voCOMXVideoSink header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXVideoSink_H__
#define __voCOMXVideoSink_H__

#include <voOMX_Index.h>

#include <voCOMXCompSink.h>
#include <voCOMXPortClock.h>


class voCOMXVideoSink : public voCOMXCompSink
{
public:
	voCOMXVideoSink(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXVideoSink(void);

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

protected:
	voCOMXPortClock *				m_pClockPort;
	OMX_VIDEO_PORTDEFINITIONTYPE	m_videoType;
	OMX_VIDEO_PARAM_PORTFORMATTYPE 	m_videoFormat;


protected:
	OMX_VO_CHECKRENDERBUFFERTYPE	m_cbRenderBuffer;
	OMX_VO_VIDEOBUFFERTYPE			m_cbVideoBuffer;
	OMX_BUFFERHEADERTYPE *			m_pRenderBuffer;

	OMX_PTR							m_hView;
};

#endif //__voCOMXVideoSink_H__
