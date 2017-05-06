	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CvoIOMXPort.h

	Contains:	Android IOMX component port header file

	Written by:	East Zhou

	Change History (most recent first):
	2012-04-20	East		Create file

*******************************************************************************/
#ifndef __CvoIOMXPort_H__
#define __CvoIOMXPort_H__

#include <media/IOMX.h>
#include "OMX_Component.h"

using namespace android;

typedef enum PORT_TRANS_STATUS
{
	PORT_TRANS_None		= 0x0,
	PORT_TRANS_Dis2Ena, 
	PORT_TRANS_Ena2Dis, 
	PORT_TRANS_MAX		= 0x7FFFFFFF
} PORT_TRANS_STATUS;

struct voIOMXPortBuffer
{
	// both input port & output port
	IOMX::buffer_id		hBuffer;
	void *				pData;

	// output port only
	OMX_BOOL			bFilled;
	OMX_U32				nOffset;
	OMX_U32				nLength;
	OMX_U32				nFlags;
	OMX_TICKS			nTimeStamp;
	OMX_PTR				pPlatformPrivate;

#ifdef _ICS
	sp<GraphicBuffer>	piGraphicBuffer;
	OMX_BOOL			bDequeued;
#endif	// _ICS
};

class CvoIOMXBufferList
{
public:
	CvoIOMXBufferList();
	virtual ~CvoIOMXBufferList();

public:
	OMX_BOOL				Create(OMX_S32 nBuffers);
	OMX_BOOL				Add(voIOMXPortBuffer * pBuffer);
	OMX_BOOL				Push(voIOMXPortBuffer * pBuffer);
	OMX_BOOL				Remove(voIOMXPortBuffer * pBuffer);
	voIOMXPortBuffer *		GetHead() { if(m_ppBuffers) return m_ppBuffers[0]; else return NULL;}
	voIOMXPortBuffer *		GetByBuffer(IOMX::buffer_id hBuffer);
	voIOMXPortBuffer *		GetByData(void * pData);
	voIOMXPortBuffer *		GetByIndex(OMX_S32 nIndex) {return m_ppBuffers[nIndex];}
	void					RemoveAll();
	OMX_U32					Count() {return m_nCount;}

protected:
	voIOMXPortBuffer **				m_ppBuffers;
	OMX_S32							m_nBuffers;
	OMX_S32							m_nCount;
};

class CvoIOMXComponent;
class CvoIOMXPort
{
public:
	CvoIOMXPort(CvoIOMXComponent * pComponent, OMX_U32 nPortIndex);
	virtual ~CvoIOMXPort();

public:
	virtual OMX_ERRORTYPE			AllocateBufferHandle() = 0;
	virtual OMX_ERRORTYPE			FreeBufferHandle();

	virtual OMX_ERRORTYPE			StartBuffer();

	virtual OMX_ERRORTYPE			UpdatePortDefinition();
	virtual OMX_ERRORTYPE			UpdatePortFormat();
	virtual OMX_BOOL				IsEnable();
	virtual OMX_COLOR_FORMATTYPE	GetColorFormat();
	virtual	void					GetResolution(OMX_U32 * pnWidth, OMX_U32 * pnHeight);

	virtual PORT_TRANS_STATUS		GetTransStatus();
	virtual OMX_ERRORTYPE			Enable();
	virtual OMX_ERRORTYPE			Disable();

	virtual OMX_ERRORTYPE			Reconfigure();

	virtual void					SetFlushing(OMX_BOOL bFlushing);
	OMX_BOOL						IsFlushing();

	virtual void					SetChanging2IdleState(OMX_BOOL bChanging2IdleState);

	virtual OMX_BOOL				CanBuffersWork();

	virtual OMX_BOOL				IsLegalBuffer(IOMX::buffer_id hBuffer);
	virtual OMX_BOOL				GetBufferByData(void * pData, voIOMXPortBuffer ** ppBuffer);

	OMX_U32							GetIndex() {return m_nIndex;}

protected:
	CvoIOMXComponent *				m_pComponent;
	OMX_U32							m_nIndex;

	OMX_PARAM_PORTDEFINITIONTYPE	m_sPortDefinition;
	OMX_VIDEO_PARAM_PORTFORMATTYPE	m_sPortFormat;
	PORT_TRANS_STATUS				m_eTransStatus;

	OMX_BOOL						m_bFlushing;
	OMX_BOOL						m_bChanging2IdleState;

	voCOMXThreadMutex				m_tmPortDefinition;
	voCOMXThreadMutex				m_tmBuffer;
	voCOMXThreadMutex				m_tmFlush;

	OMX_U32							m_nBufferCount;
	voIOMXPortBuffer *				m_pBuffers;
	CvoIOMXBufferList				m_listBufferEmpty;
	OMX_BOOL						m_bGraphicBufferAvaible;
};

#endif	//__CvoIOMXPort_H__
