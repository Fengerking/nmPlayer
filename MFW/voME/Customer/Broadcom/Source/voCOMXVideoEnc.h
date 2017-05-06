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

#include <voOMX_Index.h>
#include <voCOMXCompFilter.h>

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

	virtual OMX_ERRORTYPE ComponentRoleEnum (OMX_IN OMX_HANDLETYPE hComponent,
												OMX_OUT OMX_U8 *cRole,
												OMX_IN OMX_U32 nIndex);

protected:
	virtual OMX_ERRORTYPE	InitPortType (void);
	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);

protected:
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled);

protected:
	OMX_VIDEO_PORTDEFINITIONTYPE	m_videoType;
	OMX_PTR							m_pFormatData;

protected:
};

#endif //__voCOMXVideoEnc_H__
