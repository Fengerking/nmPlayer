	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXQcomVideoDec.h

	Contains:	voCOMXQcomVideoDec header file

	Written by:	East Zhou

	Change History (most recent first):
	2010-06-18	East		Create file

*******************************************************************************/

#ifndef __voCOMXQcomVideoDec_H__
#define __voCOMXQcomVideoDec_H__

#include <OMX_Video.h>
#include <voOMX_Index.h>

#include <voCOMXBaseComponent.h>

#include "CQcomVideoDec.h"

#ifdef _LINUX_ANDROID
#include <binder/MemoryHeapBase.h>
#endif	//_LINUX_ANDROID

#ifdef _LINUX_ANDROID
using namespace android;
// local pmem heap object
class VideoHeap : public MemoryHeapBase
{
public:
	VideoHeap(int fd, size_t size, void *base);
	virtual ~VideoHeap();
};
#endif	//_LINUX_ANDROID

class voCOMXQcomVideoDec : public voCOMXBaseComponent
{
public:
	voCOMXQcomVideoDec(OMX_COMPONENTTYPE* pComponent);
	virtual ~voCOMXQcomVideoDec();

public:
	virtual OMX_ERRORTYPE	EmptyThisBuffer(OMX_IN  OMX_HANDLETYPE hComponent, 
											OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE	FillThisBuffer(OMX_IN  OMX_HANDLETYPE hComponent, 
										   OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE	GetParameter(OMX_IN OMX_HANDLETYPE hComponent, 
										 OMX_IN OMX_INDEXTYPE nParamIndex, 
										 OMX_INOUT OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE	SetParameter(OMX_IN OMX_HANDLETYPE hComponent, 
										 OMX_IN OMX_INDEXTYPE nIndex, 
										 OMX_IN OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE	ComponentRoleEnum(OMX_IN OMX_HANDLETYPE hComponent, 
											  OMX_OUT OMX_U8* cRole, 
											  OMX_IN OMX_U32 nIndex);

protected:
	virtual OMX_ERRORTYPE	CreatePorts();
	virtual OMX_ERRORTYPE	Flush(OMX_U32 nPort);
	virtual OMX_ERRORTYPE	DisablePort(OMX_U32 nPort);

	virtual OMX_ERRORTYPE	ResetPortBuffer();
	virtual OMX_ERRORTYPE	InitPortType();

	virtual OMX_U32			BufferHandle();
	virtual OMX_ERRORTYPE	SetNewState(OMX_STATETYPE newState);

protected:
	virtual OMX_ERRORTYPE	CreateVideoDec();
	virtual OMX_ERRORTYPE	DestroyVideoDec();

	virtual OMX_ERRORTYPE	EndBufferHandleThread();

protected:
	voCOMXBasePort*					m_pInput;
	voCOMXBasePort*					m_pOutput;

	OMX_BUFFERHEADERTYPE*			m_pInputBuffer;
	OMX_BUFFERHEADERTYPE*			m_pOutputBuffer;

	voCOMXTaskQueue*				m_pInputQueue;
	voCOMXThreadSemaphore*			m_pInputSem;

	voCOMXTaskQueue*				m_pOutputQueue;
	voCOMXThreadSemaphore*			m_pOutputSem;

	OMX_S64							m_nOutBuffTime;
	OMX_U32							m_nOutBuffSize;

	OMX_BOOL						m_bInputEOS;
	OMX_BOOL						m_bResetBuffer;

	OMX_BOOL						m_bFlushed;
	voCOMXThreadMutex				m_mtxCodec;

#ifdef _LINUX_ANDROID
	// Heap pointer to frame buffers
	sp<MemoryHeapBase>				m_pHeap;
#endif	//_LINUX_ANDROID

protected:
	OMX_VIDEO_CODINGTYPE			m_nCoding;
	OMX_VIDEO_PORTDEFINITIONTYPE	m_videoType;

	OMX_U32							m_nExtSize;
	OMX_S8*							m_pExtData;

protected:
	CQcomVideoDec*					m_pVideoDec;
};

#endif //__voCOMXQcomVideoDec_H__
