	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXVideoEnc.h

	Contains:	voCOMXVideoEnc header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#ifndef __voCOMXVideoEnc_H__
#define __voCOMXVideoEnc_H__

#include "voCOMXCompFilter.h"
#include "CVideoEncoder.h"

class voCOMXVideoEnc : public voCOMXCompFilter
{
public:
	voCOMXVideoEnc(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXVideoEnc(void);

	virtual OMX_ERRORTYPE	GetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nParamIndex,  
										OMX_INOUT OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE	SetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE	ComponentRoleEnum (OMX_IN OMX_HANDLETYPE hComponent,
												OMX_OUT OMX_U8 *cRole,
												OMX_IN OMX_U32 nIndex);

protected:
	virtual OMX_ERRORTYPE	InitPortType (void);
	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled);

protected:
	OMX_VIDEO_PORTDEFINITIONTYPE	m_videoType;
	CVideoEncoder *					m_pVideoEnc;
	VO_MEM_OPERATOR *				m_pMemOP;

	VO_VIDEO_BUFFER					m_inBuffer;
	VO_CODECBUFFER					m_outBuffer;

	VO_PBYTE						m_pInputData;

	VO_IV_QUALITY					m_nQuality;
	VO_U32							m_nKeyFrameInterval;
	VO_IV_RTTYPE					m_nRotateType;
};

#endif //__voCOMXVideoEnc_H__
