	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CTIPort.h

	Contains:	CTIPort header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-04		JBF			Create file

*******************************************************************************/
#ifndef __CTIPort_H__
#define __CTIPort_H__

#include "CBasePort.h"

#define ARM_4K_PAGE_SIZE 4096

typedef struct __VO_GRAPHICBUFFER_WRAPPER__{
	void**				ppHeader;
	void*				pBufferMeta;
	unsigned char*		pBuffer;
} VOGRAPHICBUFFERWRAPPER;

typedef struct __VO_NWBUFLOCKSTATE__{
	void* pAppPrivate;
	bool bLocked;
} VONWBUFLOCKSTATE;

class CTIPort : public CBasePort
{
public:
	CTIPort(CBaseComp * pComp, OMX_U32 nIndex);
	virtual ~CTIPort(void);

	//VO_U32					SetVideoInputData(VO_CODECBUFFER * pInput, VO_PBYTE pHeadData, VO_U32 nHeadSize, VO_BOOL bHeader);
	VO_U32						GetVideoOutputData(VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo);
	VO_U32						FillOutputBuffer();
	OMX_ERRORTYPE			FillBufferDone (OMX_BUFFERHEADERTYPE* pBuffer);
	OMX_ERRORTYPE			AllocBuffer (void);
	OMX_ERRORTYPE			FreeBuffer (void);
	OMX_ERRORTYPE			StartBuffer (void);
	OMX_ERRORTYPE			SetVideoInfo(VO_VIDEO_CODINGTYPE nCoding, VO_IV_COLORTYPE nColorType, OMX_U32  nWidth, OMX_U32 nHeight);
	OMX_ERRORTYPE			ResetCrop(void);
	OMX_ERRORTYPE			ReturnAllBuffers(void);

	static					OMX_S32 UseGraphicBuffer(OMX_PTR pUserData, OMX_PTR pparam);
protected:
	OMX_S32					useGraphicBuffer(OMX_PTR pUseAndroidNativeBufferParams);
	OMX_S32					nwAllocBuffers();
	OMX_PTR					nwDequeueBuffer();
	OMX_S32					nwCancelBuffer(OMX_PTR pNativeBuffer);
	OMX_S32					nwQueueBuffer(OMX_PTR pNativeBuffer);
	OMX_BUFFERHEADERTYPE*	nwPrepareBuffer();
	OMX_S32					nwLockBuffer(OMX_PTR pNativeBuffer);
	OMX_S32					nwUnlockBuffer(OMX_PTR pNativeBuffer);
	OMX_S32					nwSetCrop(OMX_S32 nLeft, OMX_S32 nTop, OMX_S32 nRight, OMX_S32 nBottom);
	OMX_S32					nwFreeBuffer(OMX_PTR pNativeBuffer);
	OMX_BOOL				nwIsBufferDequeued(OMX_PTR pNativeBuffer);
	OMX_BOOL				nwIsBufferLocked(OMX_PTR pNativeBuffer);
	OMX_U32					nwGetMinUndequeuedBuffers();

	inline int mycallback(int nID, void * pParam1, void * pParam2);

protected:
	voCOMXThreadMutex		m_tmNaviveWindowCommand;
	OMX_U32					m_nMinUndequeuedBuffers;
	OMX_U32					m_nOffset;
	VONWBUFLOCKSTATE* m_pBufferState;
};

#endif //__CTIPort_H__
