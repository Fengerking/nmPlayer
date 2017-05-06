	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXBaseHole.h

	Contains:	voCOMXBaseHole header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-04		JBF			Create file

*******************************************************************************/
#ifndef __voCOMXBaseHole_H__
#define __voCOMXBaseHole_H__

#include "voCOMXBaseObject.h"
#include "voCOMXThreadMutex.h"

// TODO: we will use list struct for it, now only keep old style
class voCOMXBufferHeaderList : public voCOMXBaseObject
{
public:
	voCOMXBufferHeaderList();
	virtual ~voCOMXBufferHeaderList();

	virtual	OMX_BOOL				Create(OMX_S32 nBufferHeaders);
	inline OMX_BOOL					Add(OMX_BUFFERHEADERTYPE* pBufferHeader);
	inline OMX_BOOL					Remove(OMX_BUFFERHEADERTYPE* pBufferHeader);
	inline OMX_BUFFERHEADERTYPE*	GetHead() {return m_ppBufferHeaders[0];}
	inline OMX_BUFFERHEADERTYPE*	GetByBuffer(OMX_U8* pBuffer);
	inline void						RemoveAll();
	virtual	OMX_U32					Count() {return m_nCount;}

protected:
	OMX_BUFFERHEADERTYPE**			m_ppBufferHeaders;
	OMX_S32							m_nBufferHeaders;
	OMX_S32							m_nCount;
};

class voCOMXBaseBox;
class voCOMXBaseHole : public voCOMXBaseObject
{
public:
    voCOMXBaseHole(voCOMXBaseBox * pBox, OMX_U32 nIndex);
    virtual ~voCOMXBaseHole(void);

	virtual void				SetCallBack (VOMECallBack pCallBack, OMX_PTR pUserData);

	virtual OMX_ERRORTYPE		ConnectedTo (voCOMXBaseHole* pInputHole, OMX_BOOL bTunnel);
	virtual OMX_ERRORTYPE		SetConnected (voCOMXBaseHole* pOutputHole, OMX_BOOL bTunnel);
	virtual voCOMXBaseHole *	GetConnected (void);

	virtual OMX_ERRORTYPE		AllocBuffer (void);
	virtual OMX_ERRORTYPE		FreeBuffer (void);

	virtual OMX_ERRORTYPE		StartBuffer (void);

	virtual OMX_ERRORTYPE		EmptyBufferDone (OMX_BUFFERHEADERTYPE* pBuffer);
	virtual OMX_ERRORTYPE		FillBufferDone (OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE		FillBuffer (OMX_BUFFERHEADERTYPE* pBuffer);
	virtual OMX_ERRORTYPE		EmptyBuffer (OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE		Disable (void);
	virtual OMX_ERRORTYPE		Enable (void);

	virtual void				SetFlush (OMX_BOOL	bFlush);
	virtual OMX_BOOL			GetFlush (void);
	virtual void				SetChanging2IdleState (OMX_BOOL bChanging2IdleState);
	virtual OMX_BOOL			IsStartBuffer (void);
	virtual OMX_ERRORTYPE		FillPortType (void);

	virtual	voCOMXBaseBox *		GetParentBox (void) {return m_pParent;}
	virtual OMX_U32				GetPortIndex (void) {return m_nIndex;}
	virtual voCOMXThreadMutex *	GetBufferMutex (void) {return &m_tmBuffer;}

	virtual OMX_BOOL			IsInput (void);
	virtual OMX_BOOL			IsSupplier (void);
	virtual OMX_BOOL			IsTunnel (void);
	virtual OMX_BOOL			IsEnable (void);
	virtual PORT_TRANS_STATUS	GetTrans (void);

	virtual OMX_U32						GetBufferCount (void) {return m_nBuffCount;}
	virtual OMX_U32						GetBufferEmpty (void) {return m_listBuffEmpty.Count();}
	virtual OMX_BUFFERHEADERTYPE **		GetBufferHead (void) {return m_ppBuffHead;}
	OMX_PARAM_PORTDEFINITIONTYPE *		GetPortType (void) {return &m_portType;}

	OMX_ERRORTYPE 				ConnectVideoPort (OMX_COMPONENTTYPE * pPrev, OMX_U32 nOutputPort,
												  OMX_COMPONENTTYPE * pNext, OMX_U32 nInputPort);
	OMX_ERRORTYPE				ConnectAudioPort (OMX_COMPONENTTYPE * pPrev, OMX_U32 nOutputPort,
												  OMX_COMPONENTTYPE * pNext, OMX_U32 nInputPort);

	virtual OMX_S32				UseGraphicBuffer(OMX_PTR pUseAndroidNativeBufferParams);
	virtual OMX_S32				NativeWindow_AllocBuffers();
	virtual OMX_PTR				NativeWindow_DequeueBuffer();
	virtual OMX_S32				NativeWindow_CancelBuffer(OMX_PTR pNativeBuffer);
	virtual OMX_S32				NativeWindow_QueueBuffer(OMX_PTR pNativeBuffer);
	virtual OMX_S32				NativeWindow_LockBuffer(OMX_PTR pNativeBuffer);
	virtual OMX_S32				NativeWindow_SetCrop(OMX_S32 nLeft, OMX_S32 nTop, OMX_S32 nRight, OMX_S32 nBottom);
	virtual OMX_S32				NativeWindow_FreeBuffer(OMX_PTR pNativeBuffer);
	virtual OMX_BOOL			NativeWindow_IsBufferDequeued(OMX_PTR pNativeBuffer);
	virtual	OMX_BUFFERHEADERTYPE*	NativeWindow_PrepareBuffer();
	virtual OMX_U32				NativeWindow_GetMinUndequeuedBuffers();
	OMX_ERRORTYPE               getGraphicBufferUsage(OMX_U32& inUsage);
	OMX_ERRORTYPE               getAndroidNativeBufferVer(OMX_U32& inVer);
	virtual OMX_BOOL			IsUseNativeWindow() {return m_bUseNativeWindow;}

protected:
	virtual OMX_ERRORTYPE		CreateBufferHead (void);
	virtual OMX_ERRORTYPE		CopyBuffer (OMX_BUFFERHEADERTYPE * pDest, OMX_BUFFERHEADERTYPE * pSour);
	virtual OMX_U32				ProcessSEIData(OMX_BUFFERHEADERTYPE *pBuffer, OMX_U32 flags);

protected:
	voCOMXBaseBox *					m_pParent;
	OMX_COMPONENTTYPE *				m_pComponent;
	OMX_U32							m_nIndex;

	OMX_PARAM_PORTDEFINITIONTYPE	m_portType;
	OMX_BOOL						m_bSupplier;
	voCOMXThreadMutex				m_tmPortType;

	voCOMXBaseHole *				m_pConnected;
	OMX_BOOL						m_bTunnel;

	voCOMXThreadMutex				m_tmBuffer;
	PORT_TRANS_STATUS				m_nTrans;
	OMX_BOOL						m_bFlush;
	voCOMXThreadMutex				m_tmFlush;

	OMX_BOOL						m_bStartBuffer;
	OMX_BOOL						m_bEOS;

	OMX_S32							m_nBuffCount;
	OMX_BUFFERHEADERTYPE **			m_ppBuffHead;
	OMX_U8 **						m_ppBuffData;

	voCOMXBufferHeaderList			m_listBuffEmpty;

	OMX_BOOL						m_bGetHeadData;
	OMX_U8	*						m_pHeadData;
	OMX_S32							m_nHeadSize;

	OMX_U32							m_nTimeDivision;
	OMX_U32							m_nHardwareComponent;

	VOMECallBack					m_pCallBack;
	OMX_PTR							m_pUserData;
	
	voCOMXThreadMutex				m_tmNaviveWindowCommand;
	OMX_U32							m_nMinUndequeuedBuffers;
	OMX_BOOL						m_bChanging2IdleState;

	OMX_BOOL						m_b3DVideoDetected;
	OMX_U32                         mUANBVer;				// OMX.google.android.index.useAndroidNativeBuffer
	OMX_BOOL						m_bUseNativeWindow;		// if use native window
};

#endif //__voCOMXBaseHole_H__
