	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CvoIOMXInPort.h

	Contains:	Android IOMX component input port header file

	Written by:	East Zhou

	Change History (most recent first):
	2012-04-20	East		Create file

*******************************************************************************/
#ifndef __CvoIOMXInPort_H__
#define __CvoIOMXInPort_H__

#include <binder/MemoryDealer.h>
#include "CvoIOMXPort.h"
#include "OMX_Types.h"
#include "voVideoParser.h"

class CvoIOMXInPort : public CvoIOMXPort
{
public:
	CvoIOMXInPort(CvoIOMXComponent * pComponent, OMX_U32 nPortIndex);
	virtual ~CvoIOMXInPort();

public:
	virtual OMX_ERRORTYPE		SetPortSettings(OMX_VIDEO_CODINGTYPE eCodec, OMX_U32 nWidth, OMX_U32 nHeight);
	virtual OMX_ERRORTYPE		AllocateBufferHandle();

	virtual OMX_ERRORTYPE		EmptyBuffer(VO_CODECBUFFER * pInput, VO_BOOL bEOS = VO_FALSE);

	virtual void				OnEmptyBufferDone(IOMX::buffer_id hBuffer);

	virtual VO_U32				GetAspectRatio() {return m_nAspectRatio;}
	virtual OMX_ERRORTYPE		SetEmptyBufferNeedRetry(OMX_BOOL bNeedRetry);

        OMX_BOOL                        m_bDropFrame;
        VO_VIDEO_CODINGTYPE             eVOCodec;
protected:
	sp<MemoryDealer>				m_pMemoryDealer;

	VO_VIDEO_PARSERAPI				m_sVideoParserAPI;
	VO_HANDLE						m_hVideoParser;

	VO_U32							m_nAspectRatio;
	OMX_BOOL						m_bEmptyBufferNeedRetry;	// if OMX_TRUE, we will retry in EmptyBuffer, otherwise we will return need retry immediately
};

#endif	//__CvoIOMXInPort_H__
