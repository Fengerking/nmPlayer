	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXBaseComponent.h

	Contains:	voCOMXBaseComponent header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXBaseComponent_H__
#define __voCOMXBaseComponent_H__

#include "OMX_Component.h"
#include "voOMX_Types.h"
#include "voCOMXBaseObject.h"
#include "voCOMXBasePort.h"
#include "voCOMXThreadMutex.h"
#include "voCOMXThreadSemaphore.h"
#include "voCOMXTaskQueue.h"
#include "voOMXThread.h"
#include "CBaseConfig.h"

#define NUM_DOMAINS		4

typedef struct COMP_MESSAGE_TYPE {
	OMX_U32 uType;
	OMX_U32 uParam;
	OMX_PTR pData;
} COMP_MESSAGE_TYPE;

class voCOMXBaseComponent : public voCOMXBaseObject
{
friend class voCOMXBasePort;
friend class voCOMXPortInplace;

public:
	voCOMXBaseComponent(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXBaseComponent(void);

	virtual OMX_STATETYPE		GetCompState (void) {return m_sState;}
	virtual OMX_TRANS_STATE		GetTransState (void) {return m_sTrans;}
	virtual OMX_COMPONENTTYPE *	GetComponent (void) {return m_pComponent;}

	virtual OMX_BOOL			IsRunning (void);
	virtual OMX_BOOL			IsExecuting (void);
	virtual OMX_STRING			GetName (void) {return m_pName;}
	virtual OMX_BOOL			SetPosInPause (void) {return m_bSetPosInPause;}
	virtual OMX_BOOL			TranscodeMode (void) {return m_bTranscodeMode;}
	virtual OMX_S32				DumpRuntimeLog (void) {return m_nDumpRuntimeLog;}

	virtual OMX_ERRORTYPE		OnDisablePort (OMX_BOOL bFinished, OMX_U32 nPort) {return OMX_ErrorNone;}

protected:
	virtual OMX_ERRORTYPE		CheckParam (OMX_COMPONENTTYPE * pComp, OMX_U32 nPortIndex,
											OMX_PTR pParam, OMX_U32 nSize);

	virtual OMX_ERRORTYPE		TransState (OMX_STATETYPE newState);
	virtual OMX_ERRORTYPE		SetNewState (OMX_STATETYPE newState);

	virtual OMX_ERRORTYPE		Flush (OMX_U32 nPort);
	virtual OMX_ERRORTYPE		Enable (OMX_U32	nPort);
	virtual OMX_ERRORTYPE		Disable (OMX_U32 nPort);

	virtual OMX_ERRORTYPE		CreatePorts (void);
	virtual OMX_ERRORTYPE		ReleasePort (void);

	virtual OMX_ERRORTYPE		InitPortType (void);

	virtual OMX_ERRORTYPE		QuickReturnBuffer();

public:
	// standard the component function
	virtual OMX_ERRORTYPE GetComponentVersion (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_OUT OMX_STRING pComponentName,
												OMX_OUT OMX_VERSIONTYPE* pComponentVersion,
												OMX_OUT OMX_VERSIONTYPE* pSpecVersion,
												OMX_OUT OMX_UUIDTYPE* pComponentUUID);

	virtual OMX_ERRORTYPE SendCommand (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_COMMANDTYPE Cmd,
										OMX_IN  OMX_U32 nParam1,
										OMX_IN  OMX_PTR pCmdData);

	virtual OMX_ERRORTYPE GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nParamIndex,
										OMX_INOUT OMX_PTR pComponentParameterStructure);


	virtual OMX_ERRORTYPE SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentParameterStructure);


	virtual OMX_ERRORTYPE GetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_INOUT OMX_PTR pComponentConfigStructure);


	virtual OMX_ERRORTYPE SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentConfigStructure);


	virtual OMX_ERRORTYPE GetExtensionIndex (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_STRING cParameterName,
												OMX_OUT OMX_INDEXTYPE* pIndexType);

	virtual OMX_ERRORTYPE GetState (OMX_IN  OMX_HANDLETYPE hComponent,
									OMX_OUT OMX_STATETYPE* pState);

	virtual OMX_ERRORTYPE ComponentTunnelRequest (OMX_IN  OMX_HANDLETYPE hComp,
													OMX_IN  OMX_U32 nPort,
													OMX_IN  OMX_HANDLETYPE hTunneledComp,
													OMX_IN  OMX_U32 nTunneledPort,
													OMX_INOUT  OMX_TUNNELSETUPTYPE* pTunnelSetup);

	virtual OMX_ERRORTYPE UseBuffer (OMX_IN OMX_HANDLETYPE hComponent,
										OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
										OMX_IN OMX_U32 nPortIndex,
										OMX_IN OMX_PTR pAppPrivate,
										OMX_IN OMX_U32 nSizeBytes,
										OMX_IN OMX_U8* pBuffer);

	virtual OMX_ERRORTYPE AllocateBuffer (OMX_IN OMX_HANDLETYPE hComponent,
											OMX_INOUT OMX_BUFFERHEADERTYPE** ppBuffer,
											OMX_IN OMX_U32 nPortIndex,
											OMX_IN OMX_PTR pAppPrivate,
											OMX_IN OMX_U32 nSizeBytes);

	 virtual OMX_ERRORTYPE FreeBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_U32 nPortIndex,
										OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE FillThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

	 virtual OMX_ERRORTYPE SetCallbacks (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_CALLBACKTYPE* pCallbacks,
											OMX_IN  OMX_PTR pAppData);

	virtual OMX_ERRORTYPE UseEGLImage (OMX_IN OMX_HANDLETYPE hComponent,
										OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
										OMX_IN OMX_U32 nPortIndex,
										OMX_IN OMX_PTR pAppPrivate,
										OMX_IN void* eglImage);

	virtual OMX_ERRORTYPE ComponentRoleEnum (OMX_IN OMX_HANDLETYPE hComponent,
												OMX_OUT OMX_U8 *cRole,
												OMX_IN OMX_U32 nIndex);

protected:
	OMX_COMPONENTTYPE *				m_pComponent;
	OMX_STRING						m_pName;
	OMX_VERSIONTYPE					m_verMain;
	OMX_VERSIONTYPE					m_verSpec;
	OMX_UUIDTYPE					m_uuidType;

	OMX_U32							m_nGroupPriority;
	OMX_U32							m_nGroupID;

	OMX_PORT_PARAM_TYPE				m_portParam[NUM_DOMAINS];
	OMX_U32							m_uPorts;
	voCOMXBasePort **				m_ppPorts;

	OMX_MARKTYPE					m_sMarkType;
	OMX_CALLBACKTYPE *				m_pCallBack;
	OMX_PTR							m_pAppData;

	OMX_STATETYPE					m_sState;
	OMX_TRANS_STATE					m_sTrans;
	OMX_BOOL						m_bSetPosInPause;
	OMX_BOOL						m_bTranscodeMode;

	voCOMXTaskQueue					m_tqMessage;
	voCOMXThreadSemaphore			m_tsMessage;
	voCOMXThreadSemaphore			m_tsState;

	voCOMXThreadMutex				m_tmStatus;
	OMX_U32							m_nLogLevel;
	OMX_U32							m_nLogSysStartTime;
	OMX_U32							m_nLogSysCurrentTime;

	voOMXThreadHandle				m_hMsgThread;
	OMX_U32							m_nMsgThreadID;
	OMX_BOOL						m_bMsgThreadStop;

	voOMXThreadHandle				m_hBuffThread;
	OMX_U32							m_nBuffThreadID;
	OMX_BOOL						m_bBuffThreadStop;

	OMX_VO_LIB_OPERATOR *			m_pLibOP;
	OMX_STRING						m_pWorkPath;

	CBaseConfig *					m_pCfgComponent;


public:
	static OMX_U32	voCompMessageHandleProc (OMX_PTR pParam);
	static OMX_U32	voCompBufferHandleProc (OMX_PTR pParam);

protected:
	virtual OMX_U32	MessageHandle (COMP_MESSAGE_TYPE * pMsg);
	virtual OMX_U32	BufferHandle (void);

	// for dump performance information
protected:
	virtual OMX_U32	DumpPerformanceInfo (OMX_STRING pFile);

protected:
	OMX_U32	*	m_pPfmFrameTime;
	OMX_U32	*	m_pPfmCodecThreadTime;
	OMX_U32	*	m_pPfmCompnThreadTime;
	OMX_U32	*	m_pPfmMediaTime;
	OMX_U32	*	m_pPfmSystemTime;
	OMX_U32		m_nPfmFrameSize;
	OMX_U32		m_nPfmFrameIndex;
	OMX_S32		m_nDumpRuntimeLog;	// 0 means don't dump runtime log, otherwise dump
};

#endif //__voCOMXBaseComponent_H__
