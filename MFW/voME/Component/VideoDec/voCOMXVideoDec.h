	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXVideoDec.h

	Contains:	voCOMXVideoDec header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#ifndef __voCOMXVideoDec_H__
#define __voCOMXVideoDec_H__

#include "voCOMXCompFilter.h"
#include "CVideoDecoder.h"
#include "voOMX_Drm.h"

class voCOMXVideoDec : public voCOMXCompFilter
{
public:
	voCOMXVideoDec(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXVideoDec(void);

	virtual OMX_ERRORTYPE	GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE	SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE ComponentRoleEnum (OMX_IN OMX_HANDLETYPE hComponent,
												OMX_OUT OMX_U8 *cRole,
												OMX_IN OMX_U32 nIndex);

	virtual OMX_ERRORTYPE EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
		OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

protected:
	virtual OMX_ERRORTYPE	InitPortType (void);

	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);
	virtual OMX_ERRORTYPE	Flush (OMX_U32	nPort);
	virtual OMX_ERRORTYPE	OnDisablePort (OMX_BOOL bFinished, OMX_U32 nPort);

protected:
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled);

	virtual VO_U32			GetOutputData (VO_VIDEO_BUFFER * pOutput, VO_VIDEO_FORMAT * pVideoFormat, VO_U32 nStart, VO_BOOL * pbContinuous);
	virtual OMX_ERRORTYPE	CreateVideoDec (void);

protected:
	OMX_VIDEO_CODINGTYPE			m_nCoding;
	OMX_VIDEO_PORTDEFINITIONTYPE	m_videoType;

	OMX_PTR							m_pFormatData;
	OMX_U32							m_nExtSize;
	OMX_S8 *						m_pExtData;

	OMX_U32							m_nFrameTime;
	OMX_S64							m_nPrevFrameTime;
	OMX_BOOL						m_bFrameDropped;

	VO_U32							m_nDropBFrames;
	VO_U32							m_nErrorFrames;

	OMX_BOOL						m_bUseTimeStampOfDecoder;
	OMX_S64							m_llPrevTimeStamp;

	OMX_U32							mnSourceType;

protected:
	CVideoDecoder *					m_pVideoDec;
	VO_MEM_OPERATOR *				m_pMemOP;

	VO_MEM_VIDEO_OPERATOR *			m_pVideoMemOP;

	VO_CODECBUFFER					m_inBuffer;
	VO_VIDEO_BUFFER					m_outBuffer;
	VO_S64							m_nStartTime;

	VO_VIDEO_FORMAT					m_fmtOutputVideo;

	CBaseConfig *					m_pCfgCodec;

	OMX_BOOL						m_bDualCore;
	
	OMX_VO_SOURCEDRM_CALLBACK *		m_pDrm;
};

#endif //__voCOMXVideoDec_H__
