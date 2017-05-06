#ifndef __VO_COM_SAMS_VIDEO_DEC_H
#define __VO_COM_SAMS_VIDEO_DEC_H

#include <OMX_Video.h>
#include <voVideo.h>
#include <voOMX_Index.h>
#include "voCOMXCompFilter.h"

class SamsVideoDec;

class voComSamsVideoDec : public voCOMXCompFilter
{
public:
	voComSamsVideoDec(OMX_COMPONENTTYPE * pComponent);
	virtual ~voComSamsVideoDec(void);

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
	virtual OMX_ERRORTYPE	Flush (OMX_U32	nPort);

protected:
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled);

	virtual VO_U32			GetOutputData (VO_VIDEO_BUFFER * pOutput, VO_VIDEO_FORMAT * pVideoFormat, VO_U32 nStart);

protected:
	OMX_VIDEO_CODINGTYPE			m_nCoding;
	OMX_VIDEO_PORTDEFINITIONTYPE	m_videoType;

	OMX_PTR							m_pFormatData;
	OMX_U32							m_nExtSize;
	OMX_S8 *						m_pExtData;

	OMX_U32							m_nFrameTime;
	VO_U32							m_nPrevSystemTime;

protected:
	VO_CODECBUFFER					m_inBuffer;
	VO_VIDEO_BUFFER					m_outBuffer;
	VO_S64							m_nStartTime;

	SamsVideoDec*                   m_pDec;
	
};
#endif
