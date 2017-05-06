	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXBasePort.h

	Contains:	voCOMXBasePort header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXBasePort_H__
#define __voCOMXBasePort_H__

#include <OMX_Component.h>
#include <voOMX_Types.h>

#include "voCOMXBaseObject.h"

#include "voCOMXThreadMutex.h"
#include "voCOMXThreadSemaphore.h"
#include "voCOMXTaskQueue.h"

class voCOMXBaseComponent;

class voCOMXBasePort : public voCOMXBaseObject
{
friend class voCOMXBaseComponent;
friend class voCOMXCompFilter;
friend class voCOMXCompInplace;
friend class voCOMXCompSink;

public:
	voCOMXBasePort(voCOMXBaseComponent * pParent, OMX_S32 nIndex, OMX_DIRTYPE dirType);
	virtual ~voCOMXBasePort(void);

public:
	virtual OMX_ERRORTYPE	GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);
	virtual OMX_ERRORTYPE	SetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);

	virtual OMX_ERRORTYPE	GetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig);
	virtual OMX_ERRORTYPE	SetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig);

	virtual OMX_ERRORTYPE	TunnelRequest(OMX_COMPONENTTYPE * hTunneledComp, OMX_U32 nTunneledPort,
										  OMX_TUNNELSETUPTYPE* pTunnelSetup);

	virtual OMX_ERRORTYPE	SetPortType (OMX_PARAM_PORTDEFINITIONTYPE * pParamType);

	virtual OMX_ERRORTYPE	GetSupplier (OMX_PARAM_BUFFERSUPPLIERTYPE * pSupplier);
	virtual OMX_ERRORTYPE	SetSupplier (OMX_PARAM_BUFFERSUPPLIERTYPE * pSupplier);

	virtual OMX_ERRORTYPE	AllocBuffer (OMX_BUFFERHEADERTYPE** pBuffer, OMX_U32 nPortIndex,
										 OMX_PTR pAppPrivate,OMX_U32 nSizeBytes);
	virtual OMX_ERRORTYPE	UseBuffer (OMX_BUFFERHEADERTYPE** pBuffer, OMX_U32 nPortIndex,
										 OMX_PTR pAppPrivate,OMX_U32 nSizeBytes, OMX_U8 * pData);
	virtual OMX_ERRORTYPE	FreeBuffer (OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE	AllocTunnelBuffer (OMX_U32 nPortIndex, OMX_U32 nSizeBytes);
	virtual OMX_ERRORTYPE	FreeTunnelBuffer (OMX_U32 nPortIndex);

	virtual OMX_ERRORTYPE	HandleBuffer (OMX_BUFFERHEADERTYPE* pBuffer);
	virtual OMX_ERRORTYPE	ReturnBuffer (OMX_BUFFERHEADERTYPE* pBuffer);
	virtual OMX_ERRORTYPE	ResetBuffer (OMX_BOOL bRelease);

	virtual OMX_ERRORTYPE	SetNewCompState (OMX_STATETYPE sNewState, OMX_TRANS_STATE sTrans);
	virtual OMX_ERRORTYPE	Flush (void);

	virtual OMX_ERRORTYPE	Disable (void);
	virtual OMX_ERRORTYPE	Enable (void);

	virtual OMX_ERRORTYPE	SetCallbacks (OMX_CALLBACKTYPE* pCallbacks, OMX_PTR pAppData);

	virtual void			GetPortType (OMX_PARAM_PORTDEFINITIONTYPE ** ppType) {*ppType = &m_sType;}

	virtual voCOMXTaskQueue *		GetBufferQueue (void) {return &m_tqBuffer;}
	virtual voCOMXThreadSemaphore *	GetBufferSem (void) {return &m_tsBuffer;}

	virtual OMX_U32					GetBufferCount (void) {return m_uBufferCount;}
	virtual OMX_BUFFERHEADERTYPE **	GetBufferHeader (void) {return m_ppBufferHead;}

	virtual OMX_COMPONENTTYPE *		GetTunnelComp (void) {return m_hTunnelComp;}
	virtual OMX_S32 				GetTunnelPort (void) {return m_nTunnelPort;}

	virtual void	OutputDebugStatus (OMX_STATETYPE nNewState);
	OMX_U32			m_nStartDebugTime;

public:
	virtual OMX_BOOL		IsTunnel (void);
	virtual OMX_BOOL		IsSupplier (void);
	virtual OMX_BOOL		IsEnable (void);
	virtual OMX_BOOL		IsFlush (void);

protected:
	virtual void			ReleaseBuffer (void);

	virtual OMX_ERRORTYPE	AllocateBufferPointer (void);

protected:
	voCOMXBaseComponent *			m_pParent;
	OMX_PARAM_PORTDEFINITIONTYPE	m_sType;
	OMX_STRING						m_pMIMEType;

	OMX_BUFFERSUPPLIERTYPE			m_nBufferSupplier;

	OMX_U32							m_uBufferCount;
	OMX_BUFFERHEADERTYPE **			m_ppBufferHead;
	OMX_U8 **						m_ppBufferData;
	OMX_U32							m_uUsedBuffer;

	OMX_COMPONENTTYPE *				m_hTunnelComp;
	OMX_S32							m_nTunnelPort;

	PORT_TRANS_STATUS				m_sStatus;
	OMX_BOOL						m_bFlushing;
	OMX_BOOL						m_bResetBuffer;
	char							m_szLog[512];

	voCOMXTaskQueue					m_tqBuffer;
	voCOMXThreadSemaphore			m_tsAlloc;
	voCOMXThreadSemaphore			m_tsBuffer;

	OMX_CALLBACKTYPE *				m_pCallBack;
	OMX_PTR							m_pAppData;
};

#endif //__voCOMXBasePort_H__
