	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXBaseComponent.cpp

	Contains:	voCOMXBaseComponent class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "voOMX_Index.h"
#include "voOMXBase.h"
#include "voOMXOSFun.h"
#include "voOMXMemory.h"
#include "voOMXFile.h"
#include "voCOMXBaseComponent.h"

#define LOG_TAG "voCOMXBaseComponent"
#include "voLog.h"

#define CHECK_PORT_INDEX(x) \
	if (x != OMX_ALL && x >= (OMX_U32)m_uPorts) return OMX_ErrorBadPortIndex;

voCOMXBaseComponent::voCOMXBaseComponent(OMX_COMPONENTTYPE * pComponent)
	: m_pComponent (pComponent)
	, m_nGroupPriority (0)
	, m_nGroupID (0)
	, m_uPorts (0)
	, m_ppPorts (NULL)
	, m_pCallBack (NULL)
	, m_pAppData (NULL)
	, m_sState (OMX_StateInvalid)
	, m_sTrans (COMP_TRANSSTATE_None)
	, m_bSetPosInPause (OMX_FALSE)
	, m_bTranscodeMode (OMX_FALSE)
	, m_nLogLevel (0)
	, m_nLogSysStartTime (0)
	, m_nLogSysCurrentTime (0)
	, m_hMsgThread (NULL)
	, m_nMsgThreadID (0)
	, m_bMsgThreadStop (OMX_TRUE)
	, m_hBuffThread (NULL)
	, m_nBuffThreadID (0)
	, m_bBuffThreadStop (OMX_TRUE)
	, m_pLibOP (NULL)
	, m_pWorkPath (NULL)
	, m_pCfgComponent (NULL)
	, m_pPfmFrameTime (NULL)
	, m_pPfmCodecThreadTime (NULL)
	, m_pPfmCompnThreadTime (NULL)
	, m_pPfmMediaTime (NULL)
	, m_pPfmSystemTime (NULL)
	, m_nPfmFrameSize (0)
	, m_nPfmFrameIndex (0)
	, m_nDumpRuntimeLog (0)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
	m_pName = (OMX_STRING) voOMXMemAlloc (128);
	if (m_pName != NULL)
		strcpy (m_pName, "OMX_VisusalOn.base component");

	m_verMain.s.nVersionMajor = OMX_VERSION_MAJOR;
	m_verMain.s.nVersionMinor = OMX_VERSION_MINOR;
	m_verMain.s.nRevision = OMX_VERSION_REVISION;
	m_verMain.s.nStep = OMX_VERSION_STEP;

	m_verSpec.s.nVersionMajor = OMX_VERSION_MAJOR;
	m_verSpec.s.nVersionMinor = OMX_VERSION_MINOR;
	m_verSpec.s.nRevision = OMX_VERSION_REVISION;
	m_verSpec.s.nStep = OMX_VERSION_STEP;

	OMX_S32 i = 0;
	for (i = 0; i < 128; i++)
		m_uuidType[i] = rand () % 256;

	for (i = 0; i < NUM_DOMAINS; i++)
	{
		voOMXMemSet (&m_portParam[i], 0, sizeof (OMX_PORT_PARAM_TYPE));
		voOMXBase_SetHeader (&m_portParam[i], sizeof (OMX_PORT_PARAM_TYPE));
	}
	voOMXMemSet (&m_sMarkType, 0, sizeof (OMX_MARKTYPE));
	voOMXBase_SetHeader (&m_sMarkType, sizeof (OMX_MARKTYPE));

	m_sState = OMX_StateLoaded;
	m_bMsgThreadStop = OMX_FALSE;
	voOMXThreadCreate (&m_hMsgThread, &m_nMsgThreadID, (voOMX_ThreadProc)voCompMessageHandleProc, this, 0);
}

voCOMXBaseComponent::~voCOMXBaseComponent(void)
{
	m_bBuffThreadStop = OMX_TRUE;
	for (OMX_U32 i = 0; i < m_uPorts; i++)
	{
		if (m_ppPorts[i] != NULL)
		{
			m_ppPorts[i]->ResetBuffer (OMX_TRUE);
		}
	}

	while (m_hBuffThread != NULL)
	{
		for (OMX_U32 i = 0; i < m_uPorts; i++)
		{
			if (m_ppPorts[i] != NULL)
			{
				if (m_ppPorts[i]->m_tsBuffer.Waiting ())
					m_ppPorts[i]->m_tsBuffer.Up ();
			}
		}

		voOMXOS_Sleep (1);
	}

	ReleasePort ();

	m_bMsgThreadStop = OMX_TRUE;
	if (m_tsMessage.Count () == 0)
		m_tsMessage.Up ();

	OMX_U32 nTryNum = 0;
	while (m_hMsgThread != NULL && nTryNum < 100)
	{
		voOMXOS_Sleep (1);
		nTryNum++;
	}

	if (m_hMsgThread != NULL)
		VOLOGE ("Exit the message proc failed!");

	DumpPerformanceInfo (NULL);
	if (m_pPfmFrameTime != NULL)
		delete []m_pPfmFrameTime;
	if (m_pPfmCodecThreadTime != NULL)
		delete []m_pPfmCodecThreadTime;
	if (m_pPfmCompnThreadTime != NULL)
		delete []m_pPfmCompnThreadTime;
	if (m_pPfmMediaTime != NULL)
		delete []m_pPfmMediaTime;
	if (m_pPfmSystemTime != NULL)
		delete []m_pPfmSystemTime;

	voOMXMemFree (m_pName);

	if (m_pCfgComponent != NULL)
		delete m_pCfgComponent;
}

OMX_BOOL voCOMXBaseComponent::IsRunning (void)
{
//	if (m_sState == OMX_StateIdle || m_sState == OMX_StateExecuting || m_sState == OMX_StatePause)
	if ((m_sState == OMX_StateExecuting || m_sState == OMX_StatePause) && m_sTrans == COMP_TRANSSTATE_None)
		return OMX_TRUE;
	else
		return OMX_FALSE;
}

OMX_BOOL voCOMXBaseComponent::IsExecuting (void)
{
	if (m_sTrans == COMP_TRANSSTATE_None && m_sState == OMX_StateExecuting)
		return OMX_TRUE;
//	else if (m_sTrans == COMP_TRANSSTATE_IdleToExecute || m_sTrans == COMP_TRANSSTATE_PauseToExecute)
//		return OMX_TRUE;
	else
		return OMX_FALSE;
}

OMX_ERRORTYPE voCOMXBaseComponent::GetComponentVersion(OMX_IN  OMX_HANDLETYPE hComponent,
									OMX_OUT OMX_STRING pComponentName,
									OMX_OUT OMX_VERSIONTYPE* pComponentVersion,
									OMX_OUT OMX_VERSIONTYPE* pSpecVersion,
									OMX_OUT OMX_UUIDTYPE* pComponentUUID)
{
	strcpy (pComponentName, m_pName);
	memcpy (pComponentVersion, &m_verMain, sizeof (OMX_VERSIONTYPE));
	memcpy (pSpecVersion, &m_verSpec, sizeof (OMX_VERSIONTYPE));
	memcpy (pComponentUUID, &m_uuidType, 128);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBaseComponent::CheckParam (OMX_COMPONENTTYPE * pComp, OMX_U32 nPortIndex,
												OMX_PTR pParam, OMX_U32 nSize)
{
	if (nPortIndex > m_uPorts)
		return OMX_ErrorBadPortIndex;
/*
	if (m_sState != OMX_StateLoaded && m_sState != OMX_StateWaitForResources)
	{
		if (m_ppPorts[nPortIndex]->m_sType.bEnabled)
			return OMX_ErrorIncorrectStateOperation;
	}
*/
	return voOMXBase_CheckHeader (pParam, nSize);
}

OMX_ERRORTYPE voCOMXBaseComponent::TransState (OMX_STATETYPE newState)
{
	if (m_tsState.Waiting ())
		m_tsState.Up ();

	if (m_sState == OMX_StateInvalid)
		return OMX_ErrorInvalidState;

	if (m_sState == newState)
		return OMX_ErrorSameState;

	if (m_sTrans == COMP_TRANSSTATE_IdleToLoaded || m_sTrans == COMP_TRANSSTATE_AnyToInvalid)
	{
		m_bBuffThreadStop = OMX_TRUE;

		while (m_hBuffThread != NULL)
		{
			for (OMX_U32 i = 0; i < m_uPorts; i++)
			{
				if (m_ppPorts[i]->GetBufferSem ()->Waiting ())
					m_ppPorts[i]->GetBufferSem ()->Up ();
			}

			if (m_tsState.Waiting ())
				m_tsState.Up ();

			voOMXOS_Sleep (1);
		}
	}

	if (newState == OMX_StateIdle && (m_sTrans == COMP_TRANSSTATE_PauseToIdle || m_sTrans == COMP_TRANSSTATE_ExecuteToIdle))
		QuickReturnBuffer();

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	OMX_U32	i = 0;
	for (i = 0; i < m_uPorts; i++)
	{
		if ( m_ppPorts[i]->IsEnable ())
		{
			errType = m_ppPorts[i]->SetNewCompState (newState, m_sTrans);
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("m_ppPorts[i]->SetNewCompState was failed. The port is %d, result is 0X%08X",(int)i, errType);
				return errType;
			}
		}
	}

	errType = SetNewState (newState);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("SetNewState was failed. The result is 0X%08X", errType);
		return errType;
	}

	for (i = 0; i < m_uPorts; i++)
		m_ppPorts[i]->OutputDebugStatus (newState);

	m_sTrans = COMP_TRANSSTATE_None;
	m_sState = newState;

	return errType;
}

OMX_ERRORTYPE voCOMXBaseComponent::SetNewState (OMX_STATETYPE newState)
{
	if (m_tsState.Waiting ())
		m_tsState.Up ();

	if (newState == OMX_StateIdle && m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{
		if (m_hBuffThread == NULL)
		{
			m_bBuffThreadStop = OMX_FALSE;
			voOMXThreadCreate (&m_hBuffThread, &m_nBuffThreadID, (voOMX_ThreadProc) voCompBufferHandleProc, this, 0);
			if (m_hBuffThread == NULL)
			{
				VOLOGE ("voOMXThreadCreate was failed!");
				return OMX_ErrorUndefined;
			}
		}
	}
	else if (m_sTrans == COMP_TRANSSTATE_IdleToLoaded || m_sTrans == COMP_TRANSSTATE_AnyToInvalid)
	{
		m_bBuffThreadStop = OMX_TRUE;

		while (m_hBuffThread != NULL)
		{
			for (OMX_U32 i = 0; i < m_uPorts; i++)
			{
				if (m_ppPorts[i]->GetBufferSem ()->Waiting ())
					m_ppPorts[i]->GetBufferSem ()->Up ();
			}

			voOMXOS_Sleep (1);
		}
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBaseComponent::Flush (OMX_U32 nPort)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (nPort == OMX_ALL)
	{
		for (OMX_U32 i = 0; i < m_uPorts; i++)
		{
			errType = m_ppPorts[i]->Flush ();
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("m_ppPorts[i]->Flush was failed. The port is %d, result is 0X%08X", (int)i, errType);
				break;
			}
		}
	}
	else
	{
		errType = m_ppPorts[nPort]->Flush ();
	}

	voCOMXAutoLock lokc (&m_tmStatus);

	if (nPort == OMX_ALL)
	{
		for (OMX_U32 i = 0; i < m_uPorts; i++)
		{
			//if (m_ppPorts[i]->m_sType.bEnabled)
			m_ppPorts[i]->m_bFlushing = OMX_FALSE;
		}
	}
	else
	{
		//if (m_ppPorts[nParam1]->m_sType.bEnabled)
		m_ppPorts[nPort]->m_bFlushing = OMX_FALSE;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXBaseComponent::Enable(OMX_U32 nPort)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (nPort == OMX_ALL)
	{
		for (OMX_U32 i = 0; i < m_uPorts; i++)
			errType = m_ppPorts[i]->Enable ();
	}
	else
		errType = m_ppPorts[nPort]->Enable ();
	return errType;
}

OMX_ERRORTYPE voCOMXBaseComponent::Disable(OMX_U32 nPort)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	OMX_U32 i = 0;
	if (nPort == OMX_ALL)
	{
		if (m_sState != OMX_StateLoaded)
		{
			for (i = 0; i < m_uPorts; i++)
				errType = m_ppPorts[i]->Flush ();
		}
		for (i = 0; i < m_uPorts; i++)
			errType = m_ppPorts[i]->Disable ();
	}
	else
	{
		if (m_sState != OMX_StateLoaded)
			errType = m_ppPorts[nPort]->Flush ();
		errType = m_ppPorts[nPort]->Disable ();
	}
	return errType;
}

OMX_ERRORTYPE voCOMXBaseComponent::CreatePorts (void)
{
	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBaseComponent::ReleasePort (void)
{
	if (m_uPorts > 0)
	{
		for (OMX_U32 i = 0; i < m_uPorts; i++)
		{
			if (m_ppPorts[i] != NULL)
				delete m_ppPorts[i];
		}
		voOMXMemFree (m_ppPorts);

		m_uPorts = 0;
		m_ppPorts = NULL;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBaseComponent::InitPortType (void)
{
	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBaseComponent::QuickReturnBuffer()
{
	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBaseComponent::SendCommand (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_COMMANDTYPE Cmd,
											OMX_IN  OMX_U32 nParam1,
											OMX_IN  OMX_PTR pCmdData)
{
	if (m_sState == OMX_StateInvalid)
		return OMX_ErrorInvalidState;

	switch (Cmd)
	{
	case OMX_CommandStateSet:
		if ((int)m_sState == (int)nParam1)
			return OMX_ErrorSameState;

		if (nParam1 == OMX_StateLoaded)
		{
			if (m_sState == OMX_StateIdle)
				m_sTrans = COMP_TRANSSTATE_IdleToLoaded;
			else if (m_sState == OMX_StateWaitForResources)
				m_sTrans = COMP_TRANSSTATE_WaitResToLoaded;
			else
				return OMX_ErrorIncorrectStateTransition;
		}
		else if (nParam1 == OMX_StateWaitForResources)
		{
			if (m_sState == OMX_StateLoaded)
				m_sTrans = COMP_TRANSSTATE_LoadedToWaitRes;
			else
				return OMX_ErrorIncorrectStateTransition;
		}
		else if (nParam1 == OMX_StateIdle)
		{
			if (m_sState == OMX_StateLoaded)
				m_sTrans = COMP_TRANSSTATE_LoadedToIdle;
			else if (m_sState == OMX_StateWaitForResources)
				m_sTrans = COMP_TRANSSTATE_WaitResToIdle;
			else if (m_sState == OMX_StateExecuting)
			{
				DumpPerformanceInfo (NULL);
				m_sTrans = COMP_TRANSSTATE_ExecuteToIdle;
			}
			else if (m_sState == OMX_StatePause)
				m_sTrans = COMP_TRANSSTATE_PauseToIdle;
			else
				return OMX_ErrorIncorrectStateTransition;
		}
		else if (nParam1 == OMX_StatePause)
		{
			if (m_sState == OMX_StateIdle)
				m_sTrans = COMP_TRANSSTATE_IdleToPause;
			else if (m_sState == OMX_StateExecuting)
				m_sTrans = COMP_TRANSSTATE_ExecuteToPause;
			else
				return OMX_ErrorIncorrectStateTransition;
		}
		else if (nParam1 == OMX_StateExecuting)
		{
			if (m_sState == OMX_StateIdle)
				m_sTrans = COMP_TRANSSTATE_IdleToExecute;
			else if (m_sState == OMX_StatePause)
				m_sTrans = COMP_TRANSSTATE_PauseToExecute;
			else
				return OMX_ErrorIncorrectStateTransition;
		}
		else if (nParam1 == OMX_StateInvalid)
		{
			m_sTrans = COMP_TRANSSTATE_AnyToInvalid;
		}
		break;

	case OMX_CommandFlush:
		CHECK_PORT_INDEX (nParam1)
		if (nParam1 == OMX_ALL)
		{
			for (OMX_U32 i = 0; i < m_uPorts; i++)
			{
				//if (m_ppPorts[i]->m_sType.bEnabled)
				m_ppPorts[i]->m_bFlushing = OMX_TRUE;
			}
		}
		else
		{
			//if (m_ppPorts[nParam1]->m_sType.bEnabled)
			m_ppPorts[nParam1]->m_bFlushing = OMX_TRUE;
		}

		if (m_tsState.Waiting ())
			m_tsState.Up ();

		break;

	case OMX_CommandPortDisable:
		CHECK_PORT_INDEX (nParam1)
		if (nParam1 == OMX_ALL)
		{
			for (OMX_U32 i = 0; i < m_uPorts; i++)
			{
				if (m_ppPorts[i]->m_sType.bEnabled)
					m_ppPorts[i]->m_sStatus = PORT_TRANS_ENA2DIS;
			}
		}
		else
		{
			if (m_ppPorts[nParam1]->m_sType.bEnabled)
				m_ppPorts[nParam1]->m_sStatus = PORT_TRANS_ENA2DIS;
		}
		break;

	case OMX_CommandPortEnable:
		CHECK_PORT_INDEX (nParam1)
		if (nParam1 == OMX_ALL)
		{
			for (OMX_U32 i = 0; i < m_uPorts; i++)
			{
				if (!m_ppPorts[i]->m_sType.bEnabled)
					m_ppPorts[i]->m_sStatus = PORT_TRANS_DIS2ENA;
			}
		}
		else
		{
			if (!m_ppPorts[nParam1]->m_sType.bEnabled)
				m_ppPorts[nParam1]->m_sStatus = PORT_TRANS_DIS2ENA;
		}
		break;

	case OMX_CommandMarkBuffer:
		CHECK_PORT_INDEX (nParam1)
		break;

	default:
		return OMX_ErrorNotImplemented;
	}

	COMP_MESSAGE_TYPE * pMsg = (COMP_MESSAGE_TYPE *) voOMXMemAlloc (sizeof (COMP_MESSAGE_TYPE));
	if (pMsg == NULL)
		return OMX_ErrorInsufficientResources;

	pMsg->uType = Cmd;
	pMsg->uParam = nParam1;
	pMsg->pData = pCmdData;

	m_tqMessage.Add (pMsg);
	m_tsMessage.Up ();

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBaseComponent::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
	if (hComponent == NULL)
		return OMX_ErrorBadParameter;

	OMX_ERRORTYPE errType = OMX_ErrorUnsupportedIndex;
	switch (nParamIndex)
	{
	case OMX_IndexParamPriorityMgmt:
		{
			OMX_PRIORITYMGMTTYPE * pPriority = (OMX_PRIORITYMGMTTYPE*)pComponentParameterStructure;
			errType = voOMXBase_CheckHeader (pComponentParameterStructure, sizeof (OMX_PRIORITYMGMTTYPE));
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("The header of the parameter was wrong!");
				return errType;
			}

			pPriority->nGroupPriority = m_nGroupPriority;
			pPriority->nGroupID = m_nGroupID;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioInit:
		CreatePorts ();
		memcpy (pComponentParameterStructure, &m_portParam[OMX_PortDomainAudio], sizeof (OMX_PORT_PARAM_TYPE));
		errType = OMX_ErrorNone;
		break;

	case OMX_IndexParamVideoInit:
		CreatePorts ();
		memcpy (pComponentParameterStructure, &m_portParam[OMX_PortDomainVideo], sizeof (OMX_PORT_PARAM_TYPE));
		errType = OMX_ErrorNone;
		break;

	case OMX_IndexParamImageInit:
		CreatePorts ();
		memcpy (pComponentParameterStructure, &m_portParam[OMX_PortDomainImage], sizeof (OMX_PORT_PARAM_TYPE));
		errType = OMX_ErrorNone;
		break;

	case OMX_IndexParamOtherInit:
		CreatePorts ();
		memcpy (pComponentParameterStructure, &m_portParam[OMX_PortDomainOther], sizeof (OMX_PORT_PARAM_TYPE));
		errType = OMX_ErrorNone;
		break;

	case OMX_IndexParamSuspensionPolicy:
		{
			OMX_PARAM_SUSPENSIONPOLICYTYPE * pSuspendPolicy = (OMX_PARAM_SUSPENSIONPOLICYTYPE *)pComponentParameterStructure;
			pSuspendPolicy->ePolicy = OMX_SuspensionEnabled;
			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamComponentSuspended:
		break;

	case OMX_IndexParamPortDefinition:
		{
			CreatePorts ();
			OMX_PARAM_PORTDEFINITIONTYPE * pType = (OMX_PARAM_PORTDEFINITIONTYPE *)pComponentParameterStructure;
			errType = CheckParam ((OMX_COMPONENTTYPE *)hComponent, pType->nPortIndex, pType, sizeof (OMX_PARAM_PORTDEFINITIONTYPE));
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("The header of the parameter was wrong!");
				return errType;
			}

			voOMXMemCopy (pComponentParameterStructure, &m_ppPorts[pType->nPortIndex]->m_sType, sizeof (OMX_PARAM_PORTDEFINITIONTYPE));
			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamCompBufferSupplier:
		{
			OMX_PARAM_BUFFERSUPPLIERTYPE * pSupplier = (OMX_PARAM_BUFFERSUPPLIERTYPE*)pComponentParameterStructure;

			errType = voOMXBase_CheckHeader (pComponentParameterStructure, sizeof (OMX_PRIORITYMGMTTYPE));
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("The header of the parameter was wrong!");
				return errType;
			}

			CHECK_PORT_INDEX (pSupplier->nPortIndex)

			errType = m_ppPorts[pSupplier->nPortIndex]->GetSupplier (pSupplier);
		}
		break;

	case OMX_IndexParamVideoProfileLevelCurrent:
		{
			OMX_VIDEO_PARAM_PROFILELEVELTYPE* pProfileLevelType = (OMX_VIDEO_PARAM_PROFILELEVELTYPE*)pComponentParameterStructure;
			CHECK_PORT_INDEX(pProfileLevelType->nPortIndex)
			errType = m_ppPorts[pProfileLevelType->nPortIndex]->GetParameter(nParamIndex, pComponentParameterStructure);
		}
		break;

	default:
		{
			OMX_ERRORTYPE errPort = OMX_ErrorNone;
			for (OMX_U32 i = 0; i < m_uPorts; i++)
			{
				errPort = m_ppPorts[i]->GetParameter (nParamIndex, pComponentParameterStructure);
				if (errPort == OMX_ErrorNone)
					return errPort;
			}
		}
		break;
	}

	return errType;
}


OMX_ERRORTYPE voCOMXBaseComponent::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{
	OMX_ERRORTYPE errType = OMX_ErrorUnsupportedIndex;
	switch (nIndex)
	{
	case OMX_IndexParamAudioInit:
	case OMX_IndexParamVideoInit:
	case OMX_IndexParamImageInit:
	case OMX_IndexParamOtherInit:
		if (m_sState != OMX_StateLoaded && m_sState != OMX_StateWaitForResources)
			return OMX_ErrorIncorrectStateOperation;

		errType = voOMXBase_CheckHeader (pComponentParameterStructure, sizeof (OMX_PRIORITYMGMTTYPE));
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("The header of the parameter was wrong!");
			return errType;
		}

		errType = OMX_ErrorUndefined;
		break;

	case OMX_IndexParamPortDefinition:
		{
			OMX_PARAM_PORTDEFINITIONTYPE * pType = (OMX_PARAM_PORTDEFINITIONTYPE *)pComponentParameterStructure;
			errType = CheckParam ((OMX_COMPONENTTYPE *)hComponent, pType->nPortIndex, pType, sizeof (OMX_PARAM_PORTDEFINITIONTYPE));
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("The header of the parameter was wrong!");
				return errType;
			}

			errType = m_ppPorts[pType->nPortIndex]->SetPortType (pType);
		}
		break;

	case OMX_IndexParamPriorityMgmt:
		{
			if (m_sState != OMX_StateLoaded && m_sState != OMX_StateWaitForResources)
				return OMX_ErrorIncorrectStateOperation;

			OMX_PRIORITYMGMTTYPE * pPrioMgmt = (OMX_PRIORITYMGMTTYPE*)pComponentParameterStructure;

			errType = voOMXBase_CheckHeader (pComponentParameterStructure, sizeof (OMX_PRIORITYMGMTTYPE));
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("The header of the parameter was wrong!");
				return errType;
			}

			m_nGroupPriority = pPrioMgmt->nGroupPriority;
			m_nGroupID = pPrioMgmt->nGroupID;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamCompBufferSupplier:
		{
			OMX_PARAM_BUFFERSUPPLIERTYPE * pSupplier = (OMX_PARAM_BUFFERSUPPLIERTYPE *)pComponentParameterStructure;
			errType = CheckParam ((OMX_COMPONENTTYPE *)hComponent, pSupplier->nPortIndex, pSupplier, sizeof (OMX_PARAM_BUFFERSUPPLIERTYPE));
			if (errType == OMX_ErrorIncorrectStateOperation)
				return OMX_ErrorIncorrectStateOperation;
			else if (errType != OMX_ErrorNone)
				break;

			if (pSupplier->eBufferSupplier == OMX_BufferSupplyUnspecified)
				return OMX_ErrorNone;

			errType = m_ppPorts[pSupplier->nPortIndex]->SetSupplier (pSupplier);
		}
		break;

	case OMX_IndexParamStandardComponentRole:
		{
			OMX_PARAM_COMPONENTROLETYPE * pRoleType = (OMX_PARAM_COMPONENTROLETYPE *)pComponentParameterStructure;
			pRoleType = pRoleType;
			errType = OMX_ErrorNone;
		}
		break;

	case OMX_VO_IndexLibraryOperator:
		m_pLibOP = (OMX_VO_LIB_OPERATOR *)pComponentParameterStructure;
		errType = OMX_ErrorNone;
		break;

	case OMX_VO_IndexWorkPath:
		m_pWorkPath = (OMX_STRING)pComponentParameterStructure;
		errType = OMX_ErrorNone;
		break;

	case OMX_VO_IndexSetPosInPause:
		m_bSetPosInPause = *(OMX_BOOL*)pComponentParameterStructure;
		errType = OMX_ErrorNone;
		break;

	case OMX_VO_IndexSetTranscodeMode:
		m_bTranscodeMode = *(OMX_BOOL*)pComponentParameterStructure;
		errType = OMX_ErrorNone;
		break;

	default:
		{
			OMX_ERRORTYPE errPort = OMX_ErrorNone;
			for (OMX_U32 i = 0; i < m_uPorts; i++)
			{
				errPort = m_ppPorts[i]->SetParameter (nIndex, pComponentParameterStructure);
				if (errPort == OMX_ErrorNone)
					return errPort;
			}
		}
		break;
	}

	return errType;
}


OMX_ERRORTYPE voCOMXBaseComponent::GetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
						OMX_IN  OMX_INDEXTYPE nIndex,
						OMX_INOUT OMX_PTR pComponentConfigStructure)
{
	OMX_ERRORTYPE errType = OMX_ErrorUnsupportedIndex;
	switch (nIndex)
	{
	case OMX_IndexConfigPriorityMgmt:
		{
			OMX_PRIORITYMGMTTYPE * pPriority = (OMX_PRIORITYMGMTTYPE*)pComponentConfigStructure;
			errType = voOMXBase_CheckHeader (pComponentConfigStructure, sizeof (OMX_PRIORITYMGMTTYPE));
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("The header of the parameter was wrong!");
				return errType;
			}

			pPriority->nGroupPriority = m_nGroupPriority;
			pPriority->nGroupID = m_nGroupID;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexConfigCapturing:
		{
			OMX_CONFIG_BOOLEANTYPE * pParam = (OMX_CONFIG_BOOLEANTYPE *)pComponentConfigStructure;
			pParam->bEnabled = OMX_FALSE;

			errType = OMX_ErrorNone;
		}
		break;

	default:
		{
			OMX_ERRORTYPE errPort = OMX_ErrorNone;
			for (OMX_U32 i = 0; i < m_uPorts; i++)
			{
				errPort = m_ppPorts[i]->GetConfig (nIndex, pComponentConfigStructure);
				if (errPort == OMX_ErrorNone)
					return errPort;
			}
		}
		break;
	}

	return errType;
}


OMX_ERRORTYPE voCOMXBaseComponent::SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
													OMX_IN  OMX_INDEXTYPE nIndex,
													OMX_IN  OMX_PTR pComponentConfigStructure)
{
	OMX_ERRORTYPE errPort = OMX_ErrorUnsupportedIndex;
	for (OMX_U32 i = 0; i < m_uPorts; i++)
	{
		errPort = m_ppPorts[i]->SetConfig (nIndex, pComponentConfigStructure);
		if (errPort == OMX_ErrorNone)
			return errPort;
	}

	return OMX_ErrorUnsupportedIndex;
}


OMX_ERRORTYPE voCOMXBaseComponent::GetExtensionIndex (OMX_IN  OMX_HANDLETYPE hComponent,
													OMX_IN  OMX_STRING cParameterName,
													OMX_OUT OMX_INDEXTYPE* pIndexType)
{
	return OMX_ErrorBadParameter;
}

OMX_ERRORTYPE voCOMXBaseComponent::GetState (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_OUT OMX_STATETYPE* pState)
{
	*pState = m_sState;

	return OMX_ErrorNone;
}


OMX_ERRORTYPE voCOMXBaseComponent::ComponentTunnelRequest (OMX_IN  OMX_HANDLETYPE hComp,
														OMX_IN  OMX_U32 nPort,
														OMX_IN  OMX_HANDLETYPE hTunneledComp,
														OMX_IN  OMX_U32 nTunneledPort,
														OMX_INOUT  OMX_TUNNELSETUPTYPE* pTunnelSetup)
{
	CHECK_PORT_INDEX (nPort)

	return m_ppPorts[nPort]->TunnelRequest ((OMX_COMPONENTTYPE *)hTunneledComp, nTunneledPort, pTunnelSetup);
}

OMX_ERRORTYPE voCOMXBaseComponent::UseBuffer (OMX_IN OMX_HANDLETYPE hComponent,
											OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
											OMX_IN OMX_U32 nPortIndex,
											OMX_IN OMX_PTR pAppPrivate,
											OMX_IN OMX_U32 nSizeBytes,
											OMX_IN OMX_U8* pBuffer)
{
	CHECK_PORT_INDEX (nPortIndex)

	return m_ppPorts[nPortIndex]->UseBuffer (ppBufferHdr, nPortIndex, pAppPrivate, nSizeBytes, pBuffer);
}

OMX_ERRORTYPE voCOMXBaseComponent::AllocateBuffer (OMX_IN OMX_HANDLETYPE hComponent,
												OMX_INOUT OMX_BUFFERHEADERTYPE** ppBuffer,
												OMX_IN OMX_U32 nPortIndex,
												OMX_IN OMX_PTR pAppPrivate,
												OMX_IN OMX_U32 nSizeBytes)
{
	CHECK_PORT_INDEX (nPortIndex)

	return m_ppPorts[nPortIndex]->AllocBuffer (ppBuffer, nPortIndex, pAppPrivate, nSizeBytes);
}

OMX_ERRORTYPE voCOMXBaseComponent::FreeBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_U32 nPortIndex,
											OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
	CHECK_PORT_INDEX (nPortIndex)

	return m_ppPorts[nPortIndex]->FreeBuffer (nPortIndex, pBuffer);
}

OMX_ERRORTYPE voCOMXBaseComponent::EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
	voCOMXAutoLock lokc (&m_tmStatus);

	if (pBuffer == NULL)
		return OMX_ErrorBadParameter;

	OMX_U32 nPortIndex = pBuffer->nInputPortIndex;
	CHECK_PORT_INDEX (nPortIndex)

	if (m_ppPorts[nPortIndex]->m_sType.eDir != OMX_DirInput)
		return OMX_ErrorBadPortIndex;
	return m_ppPorts[nPortIndex]->HandleBuffer (pBuffer);
}

OMX_ERRORTYPE voCOMXBaseComponent::FillThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
	voCOMXAutoLock lokc (&m_tmStatus);

	if (pBuffer == NULL)
		return OMX_ErrorBadParameter;

	OMX_U32 nPortIndex = pBuffer->nOutputPortIndex;
	CHECK_PORT_INDEX (nPortIndex)

	if (m_ppPorts[nPortIndex]->m_sType.eDir != OMX_DirOutput)
		return OMX_ErrorBadPortIndex;

	return m_ppPorts[nPortIndex]->HandleBuffer (pBuffer);
}

OMX_ERRORTYPE voCOMXBaseComponent::SetCallbacks (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_CALLBACKTYPE* pCallbacks,
											OMX_IN  OMX_PTR pAppData)
{
	m_pCallBack = pCallbacks;
	m_pAppData = pAppData;

	for (OMX_U32 i = 0; i < m_uPorts; i++)
	{
		if (m_ppPorts[i] != NULL)
			m_ppPorts[i]->SetCallbacks (m_pCallBack, m_pAppData);
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBaseComponent::UseEGLImage (OMX_IN OMX_HANDLETYPE hComponent,
											OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
											OMX_IN OMX_U32 nPortIndex,
											OMX_IN OMX_PTR pAppPrivate,
											OMX_IN void* eglImage)
{
	return OMX_ErrorNotImplemented;
}

OMX_ERRORTYPE voCOMXBaseComponent::ComponentRoleEnum (OMX_IN OMX_HANDLETYPE hComponent,
													OMX_OUT OMX_U8 *cRole,
													OMX_IN OMX_U32 nIndex)
{
	strcat((char*)cRole, "\0");

	return OMX_ErrorNoMore;
}

OMX_U32 voCOMXBaseComponent::voCompMessageHandleProc (OMX_PTR pParam)
{
	voCOMXBaseComponent * pComp = (voCOMXBaseComponent *)pParam;
	COMP_MESSAGE_TYPE *	pMsg = NULL;

#ifdef _LINUX
	char threadname[255];
	memset( threadname , 0 , sizeof(threadname) );
	strcpy( threadname , pComp->GetName() );
	
	if( strstr( threadname , "VisualOn.Video.Sink" ) )
		strcpy( threadname , "VO.VS." );
	else if( strstr( threadname , "VisualOn.Audio.Sink" ) )
		strcpy( threadname , "VO.AS." );
	else if( strstr( threadname , "VisualOn.Video.Decoder" ) )
		strcpy( threadname , "VO.VD." );
	else if( strstr( threadname , "VisualOn.Audio.Decoder" ) )
		strcpy( threadname , "VO.AD." );
	else if( strstr( threadname , "VisualOn.FileSource" ) )
		strcpy( threadname , "VO.FS." );
	else if( strstr( threadname , "VisualOn.Clock.Time" ) )
		strcpy( threadname , "VO.CT." );

	strcat( threadname , "MsgHandle" );

	voSetThreadName( threadname );
#endif

	while (!pComp->m_bMsgThreadStop)
	{
		pComp->m_tsMessage.Down ();

		if (pComp->m_sState == OMX_StateInvalid)
			break;

		pMsg = (COMP_MESSAGE_TYPE *)pComp->m_tqMessage.Remove ();
		if (pMsg != NULL)
		{

			pComp->MessageHandle (pMsg);

			voOMXMemFree (pMsg);
			pMsg = NULL;
		}
	}

	voOMXThreadClose (pComp->m_hMsgThread, 0);
	pComp->m_hMsgThread = NULL;

	return 0;
}

OMX_U32 voCOMXBaseComponent::MessageHandle (COMP_MESSAGE_TYPE * pMsg)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	OMX_U32	i = 0;

	if (m_pCallBack == NULL)
		return 0;

//	voCOMXAutoLock lokc (&m_tmStatus);

	switch (pMsg->uType)
	{
	case OMX_CommandStateSet:
		errType = TransState ((OMX_STATETYPE)pMsg->uParam);
		if (errType != OMX_ErrorNone)
			m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventError, errType, 0, NULL);
		else
			m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventCmdComplete, OMX_CommandStateSet, pMsg->uParam, NULL);
		break;

	case OMX_CommandFlush:
		errType = Flush (pMsg->uParam);
		if (errType != OMX_ErrorNone)
		{
			m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventError, errType, 0, NULL);
		}
		else
		{
			if (pMsg->uParam == OMX_ALL)
			{
				for (i = 0; i < m_uPorts; i++)
				{
					m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventCmdComplete, OMX_CommandFlush, i, NULL);
				}
			}
			else
			{
				m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventCmdComplete, OMX_CommandFlush, pMsg->uParam, NULL);
			}
		}
		break;

	case OMX_CommandPortDisable:
		errType = Disable (pMsg->uParam);
		if (errType != OMX_ErrorNone)
		{
			m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventError, errType, 0, NULL);
		}
		else
		{
			if (pMsg->uParam == OMX_ALL)
			{
				for (i = 0; i < m_uPorts; i++)
				{
					m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventCmdComplete, OMX_CommandPortDisable, i, NULL);
				}
			}
			else
			{
				m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventCmdComplete, OMX_CommandPortDisable, pMsg->uParam, NULL);
			}
		}
		break;

	case OMX_CommandPortEnable:
		errType = Enable (pMsg->uParam);
		if (errType != OMX_ErrorNone)
		{
			m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventError, errType, 0, NULL);
		}
		else
		{
			//don't do repeat work with voCOMXBasePort::Enable
			//East 2009/11/23
			if (pMsg->uParam == OMX_ALL)
			{
				for (i = 0; i < m_uPorts; i++)
					m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventCmdComplete, OMX_CommandPortEnable, i, NULL);
			}
			else
			{
				m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventCmdComplete, OMX_CommandPortEnable, pMsg->uParam, NULL);
			}
		}

		break;

	case OMX_CommandMarkBuffer:
		{
			OMX_MARKTYPE * pMarkData = (OMX_MARKTYPE *)pMsg->pData;
			m_sMarkType.hMarkTargetComponent = pMarkData->hMarkTargetComponent;
			m_sMarkType.pMarkData = pMarkData->pMarkData;
		}
		break;

	default:
		break;
	}

	return 0;
}

OMX_U32 voCOMXBaseComponent::voCompBufferHandleProc (OMX_PTR pParam)
{
	voCOMXBaseComponent *	pComp = (voCOMXBaseComponent *)pParam;

#ifdef _LINUX
	char threadname[255];
	memset( threadname , 0 , sizeof(threadname) );
	strcpy( threadname , pComp->GetName() );

	if( strstr( threadname , "VisualOn.Video.Sink" ) )
		strcpy( threadname , "VO.VS." );
	else if( strstr( threadname , "VisualOn.Audio.Sink" ) )
		strcpy( threadname , "VO.AS." );
	else if( strstr( threadname , "VisualOn.Video.Decoder" ) )
		strcpy( threadname , "VO.VD." );
	else if( strstr( threadname , "VisualOn.Audio.Decoder" ) )
		strcpy( threadname , "VO.AD." );
	else if( strstr( threadname , "VisualOn.FileSource" ) )
		strcpy( threadname , "VO.FS." );
	else if( strstr( threadname , "VisualOn.Clock.Time" ) )
		strcpy( threadname , "VO.CT." );

	strcat( threadname , "BufHandle" );
	
	voSetThreadName( threadname );
#endif

	OMX_STATETYPE	nState = pComp->GetCompState ();
	OMX_U32			nTryTimes = 0;
	while (nState != OMX_StateExecuting && nState != OMX_StatePause)
	{
		voOMXOS_Sleep (2);
		nState = pComp->GetCompState ();

		if (nTryTimes > 500)
		{
			VOLOGE ("The Component %s could not switch to run statue!", pComp->GetName ());
			break;
		}
		nTryTimes++;

		if (pComp->m_bBuffThreadStop)
			break;

		if (nState == OMX_StateLoaded || nState == OMX_StateInvalid)
			break;

		if (pComp->GetTransState () == COMP_TRANSSTATE_IdleToLoaded)
			break;
	}

	while (!pComp->m_bBuffThreadStop)
	{
		pComp->BufferHandle ();
	}

	voOMXThreadClose (pComp->m_hBuffThread, 0);
	pComp->m_hBuffThread = NULL;

	return 0;
}

OMX_U32 voCOMXBaseComponent::BufferHandle (void)
{
	voOMXOS_Sleep (10);

	return 0;
}

OMX_U32 voCOMXBaseComponent::DumpPerformanceInfo (OMX_STRING pFile)
{
	if (m_nPfmFrameIndex == 0 || m_pPfmFrameTime == NULL)
		return 0;

	OMX_PTR hFile = NULL;
	OMX_PTR hOMXFile = NULL;
	if (pFile != NULL)
		hFile = voOMXFileOpen (pFile, VOOMX_FILE_WRITE_ONLY);
	else
	{
#ifdef _WIN32
		TCHAR szFile[256];
		TCHAR szOMXFile[256];
#ifndef _WIN32_WCE
		_tcscpy (szFile, _T("C:\\"));
#else
		_tcscpy (szFile, _T("\\"));
#endif // _WIN32_WCE
		_tcscpy (szOMXFile, szFile);
		_tcscat (szOMXFile, _T("voomx.txt"));

		TCHAR szName[128];
		memset (szName, 0, 256);
		MultiByteToWideChar (CP_ACP, 0, m_pName, -1, szName,  128);

		_tcscat (szFile, szName);
		_tcscat (szFile, _T(".txt"));
#elif defined _LINUX
		char szFile[256];
		char szOMXFile[256];
		strcpy (szFile, _T("/sdcard/"));
		strcat (szFile, m_pName);
		strcat (szFile, ".txt");

		if (m_pCfgComponent != NULL)
		{
			char * pDumpFile = m_pCfgComponent->GetItemText (m_pName, (char*)"DumpLogFile");
			if (pDumpFile != NULL)
				strcpy (szFile, pDumpFile);
		}

		strcpy (szOMXFile, _T("/sdcard/voomx.txt"));
#endif // _WIN32

		hFile = voOMXFileOpen ((OMX_STRING)szFile, VOOMX_FILE_WRITE_ONLY);
		hOMXFile = voOMXFileOpen ((OMX_STRING)szOMXFile, VOOMX_FILE_READ_WRITE);
	}

//	if (hFile == NULL || hOMXFile == NULL)
//		return 0;
	if (hOMXFile != NULL)
		voOMXFileSeek (hOMXFile, 0, VOOMX_FILE_END);

	if (m_nPfmFrameIndex > m_nPfmFrameSize)
		m_nPfmFrameIndex = m_nPfmFrameSize;

	OMX_U32 nTotalSysTime = 0;
	OMX_U32 nTotalThrTime = 0;
	OMX_U32 i = 0;
	for (i = 0; i < m_nPfmFrameIndex; i++)
	{
		nTotalSysTime = nTotalSysTime + m_pPfmFrameTime[i];
		nTotalThrTime = nTotalThrTime + m_pPfmCodecThreadTime[i];
	}
	if (nTotalSysTime == 0)
		nTotalSysTime = 1;
	if (nTotalThrTime == 0)
		nTotalThrTime = 1;

	OMX_U32 nUsedSysTime = m_pPfmSystemTime[m_nPfmFrameIndex - 1] - m_pPfmSystemTime[0];
	if (nUsedSysTime == 0)
		nUsedSysTime = 1;

	char szLine[256];
	sprintf (szLine, "Number: %d, Time: T- %d, S-%d		Speed: System: %d	Thread: %d	Total: %dF/S \r\n\r\n\r\n",
			 (int)m_nPfmFrameIndex, (int)nTotalThrTime, (int)nTotalSysTime,
			 (int)(m_nPfmFrameIndex * 100000 / nTotalSysTime),
			 (int)(m_nPfmFrameIndex * 100000 / nTotalThrTime),
			 (int)(m_nPfmFrameIndex * 100000 / nUsedSysTime));
	if (hFile != NULL)
		voOMXFileWrite (hFile, (OMX_U8 *)szLine, strlen (szLine));

	if (hOMXFile != NULL)
	{
		voOMXFileWrite (hOMXFile, (OMX_U8 *)m_pName, strlen (m_pName));
		voOMXFileWrite (hOMXFile, (OMX_U8 *)"\r\n", 2);
		voOMXFileWrite (hOMXFile, (OMX_U8 *)szLine, strlen (szLine));
		voOMXFileClose (hOMXFile);
	}
	char szDumpLog[1024];
	strcpy (szDumpLog, m_pName);
	strcat (szDumpLog, "   ");
	strcat (szDumpLog, szLine);
	VOLOGE ("%s", szDumpLog);
	// m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_EventComponentLog, 0, 0, (OMX_PTR)szLog);

	strcpy (szLine, "Index	TSpd	SSpd	Sys	Step	Thread	Step	Media	Step	Diff	\r\n");
	if (hFile != NULL)
		voOMXFileWrite (hFile, (OMX_U8 *)szLine, strlen (szLine));

	OMX_U32 nSysTime  = 0;
	OMX_U32 nThdTime  = 0;
	for (i = 0; i < m_nPfmFrameIndex; i++)
	{
		nSysTime = m_pPfmSystemTime[i] - m_pPfmSystemTime[0];
		nThdTime = m_pPfmCompnThreadTime[i] - m_pPfmCompnThreadTime[0];

		if (i == 0)
			sprintf (szLine, "%d,	%d	%d	%d	%d	%d	%d	%d	%d	%d\r\n",
					 (int)i, (int)m_pPfmCodecThreadTime[i], (int)m_pPfmFrameTime[i], (int)nSysTime, 0,
					 (int)nThdTime, 0, (int)m_pPfmMediaTime[i], 0,
					 (int)nSysTime - (int)m_pPfmMediaTime[i]);
		else
			sprintf (szLine, "%d,	%d	%d	%d	%d	%d	%d	%d	%d	%d\r\n",
					 (int)i, (int)m_pPfmCodecThreadTime[i], (int)m_pPfmFrameTime[i], (int)nSysTime,
					 (int)m_pPfmSystemTime[i] - (int)m_pPfmSystemTime[i-1],
					 (int)nThdTime, (int)m_pPfmCompnThreadTime[i] - (int)m_pPfmCompnThreadTime[i-1],
					 (int)m_pPfmMediaTime[i], (int)m_pPfmMediaTime[i] - (int)m_pPfmMediaTime[i-1],
					 (int)(nSysTime - m_pPfmMediaTime[i]));
			if (hFile != NULL)
				voOMXFileWrite (hFile, (OMX_U8 *)szLine, strlen (szLine));
	}

	if (hFile != NULL)
		voOMXFileClose (hFile);
	m_nPfmFrameIndex = 0;

	return 1;
}
