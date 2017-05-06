	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CTIPort.h

	Contains:	CTIPort header file

	Written by:	Bangfei Jin

	Change History(most recent first):
	2009-04-04		JBF			Create file

*******************************************************************************/
#ifndef __CTIPort_H__
#define __CTIPort_H__

#include <binder/MemoryDealer.h>
#include "CBasePort.h"
#include "vonwdefs.h"

#define ARM_4K_PAGE_SIZE 4096


class CTIPort : public CBasePort
{

public:
	CTIPort(CBaseComp * pComp, OMX_U32 nIndex, sp<IOMX>& omx, void* pn, sp<ANativeWindow>& nw);
	virtual ~CTIPort(void);

	VO_U32					SetVideoInputData(VO_CODECBUFFER * pInput, VO_PBYTE pHeadData, VO_U32 nHeadSize, VO_BOOL bHeader);
	VO_U32					GetVideoOutputData(VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo);
	VO_U32					FillOutputBuffer();

	OMX_ERRORTYPE			EmptyBuffer(OMX_BUFFERHEADERTYPE* pBuffer, OMX_U32 rangeOffset, OMX_U32 rangeLength, OMX_U32 flags, OMX_TICKS timestamp);
	OMX_ERRORTYPE			FillBuffer(OMX_BUFFERHEADERTYPE* pBuffer);
	OMX_ERRORTYPE			EmptyBufferDone (OMX_BUFFERHEADERTYPE* pBuffer);
	OMX_ERRORTYPE			FillBufferDone(OMX_BUFFERHEADERTYPE* pBuffer);
	OMX_ERRORTYPE			AllocBuffer(OMX_PARAM_PORTDEFINITIONTYPE* pPortDef = NULL);
	OMX_ERRORTYPE			FreeBuffer(void);
	OMX_ERRORTYPE			StartBuffer(void);
	OMX_ERRORTYPE			SetVideoInfo(VO_VIDEO_CODINGTYPE nCoding, VO_IV_COLORTYPE nColorType, OMX_U32  nWidth, OMX_U32 nHeight);
	OMX_ERRORTYPE			ResetCrop(void);
	OMX_ERRORTYPE			ReturnAllBuffers(void);

	OMX_S32					ReconnectNativeWindow(OMX_PARAM_PORTDEFINITIONTYPE* pPortDef = NULL);
	void					InitOutputFormat(OMX_PARAM_PORTDEFINITIONTYPE* pPortDef = NULL);
	void					SetCrop(OMX_PARAM_PORTDEFINITIONTYPE* pPortDef = NULL);
	void					SetFlag(OMX_U32 flags = 0) { m_nflags = flags; }
protected:

	OMX_S32									EnableGraphicBuffers();
	OMX_S32									nwInitNativeWindow(OMX_PARAM_PORTDEFINITIONTYPE* pPortDef = NULL);
	OMX_S32									nwAllocBuffers();
	OMX_S32									nwCancelBuffer(OMX_PTR);
	OMX_S32									nwQueueBuffer(OMX_PTR);
	OMX_S32									nwLockBuffer(OMX_PTR);
	OMX_S32									nwFreeBuffer(OMX_PTR);
	OMX_S32									nwSetCrop();
	OMX_BUFFERHEADERTYPE*					nwDequeueBuffer();
	OMX_BUFFERHEADERTYPE*					nwPrepareBuffer();

	status_t setVideoPortFormatType(OMX_U32 portIndex, OMX_VIDEO_CODINGTYPE compressionFormat, OMX_COLOR_FORMATTYPE colorFormat); 
	status_t applyRotation();
	void getFlags();

	inline int mycallback(int nID, void * pParam1, void * pParam2);

protected:
	voCOMXThreadMutex				m_tmNaviveWindowCommand;
	OMX_U32							m_nMinUndequeuedBufs;
	OMX_U32							m_nOffset;
	OMX_U32							m_nflags;

	sp<IOMX>						m_pomx;
	void*							m_pnode;
	sp<ANativeWindow>				m_pnativewindow;
	sp<MemoryDealer>				m_pmemdealer;
	bool							m_bLivesLocally;

	void*							m_pdrmengine;
	VOBUFFERINFO*					m_pBuffInfo;

private:
	enum {
		kPortIndexInput  = 0,
		kPortIndexOutput = 1
	};

	int findInputBuffer(OMX_BUFFERHEADERTYPE*);
};

#endif //__CTIPort_H__
