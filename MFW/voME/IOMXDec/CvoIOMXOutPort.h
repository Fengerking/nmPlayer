	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CvoIOMXOutPort.h

	Contains:	Android IOMX component output port header file

	Written by:	East Zhou

	Change History (most recent first):
	2012-04-20	East		Create file

*******************************************************************************/
#ifndef __CvoIOMXOutPort_H__
#define __CvoIOMXOutPort_H__

#include "CvoIOMXPort.h"
#include "OMX_Types.h"

class CvoIOMXOutPort : public CvoIOMXPort
{
public:
	CvoIOMXOutPort(CvoIOMXComponent * pComponent, OMX_U32 nPortIndex);
	virtual ~CvoIOMXOutPort();

public:
	virtual OMX_ERRORTYPE		SetPortSettings(OMX_U32 nWidth, OMX_U32 nHeight);
	virtual OMX_ERRORTYPE		AllocateBufferHandle();

	virtual OMX_ERRORTYPE		FillBuffer(IOMX::buffer_id hBuffer, OMX_BOOL bCancel = OMX_FALSE);
	virtual OMX_ERRORTYPE		StartBuffer();

	virtual	void				GetDecodeResolution(OMX_U32 * pnWidth, OMX_U32 * pnHeight);

	virtual OMX_ERRORTYPE		GetFilledBuffer(voIOMXPortBuffer ** ppBuffer);
	virtual void				SetFlushing(OMX_BOOL bFlushing);

	virtual void				OnFillBufferDone(IOMX::buffer_id hBuffer, OMX_U32 nOffset, OMX_U32 nLength, OMX_U32 nFlags, OMX_TICKS nTimeStamp, OMX_PTR pPlatformPrivate, OMX_PTR pData);

#ifdef _ICS
	inline OMX_U32				GetOutputBufferCount() { return (m_nBufferCount - m_nMinUndequeuedBuffers); }
#endif	// _ICS

#ifdef _ICS
protected:
	OMX_S32							m_nMinUndequeuedBuffers;
#endif	// _ICS
};

#endif	//__CvoIOMXOutPort_H__
