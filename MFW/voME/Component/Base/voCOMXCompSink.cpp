	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCompSink.cpp

	Contains:	voCOMXCompSink class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "voOMXMemory.h"
#include "voOMXOSFun.h"
#include "voCOMXCompSink.h"

#define LOG_TAG "voCOMXCompSink"
#include "voLog.h"

voCOMXCompSink::voCOMXCompSink(OMX_COMPONENTTYPE * pComponent)
	: voCOMXBaseComponent (pComponent)
	, m_pInput (NULL)
	, m_pBufferQueue (NULL)
	, m_pSemaphore (NULL)
	, m_llSeekTime (0)
	, m_llPlayTime (0)
	, m_nRenderFrames (0)
	, m_bEOS (OMX_FALSE)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
}

voCOMXCompSink::~voCOMXCompSink(void)
{
}

OMX_ERRORTYPE voCOMXCompSink::CreatePorts (void)
{
	if (m_uPorts == 0)
	{
		m_uPorts = 1;
		m_ppPorts = (voCOMXBasePort **)voOMXMemAlloc (m_uPorts * sizeof (voCOMXBasePort *));
		if (m_ppPorts == NULL)
			return OMX_ErrorInsufficientResources;
		m_pInput = m_ppPorts[0] = new voCOMXBasePort (this, 0, OMX_DirInput);
		if (m_pInput == NULL)
			return OMX_ErrorInsufficientResources;
		m_pInput->SetCallbacks (m_pCallBack, m_pAppData);

		m_pBufferQueue = m_pInput->GetBufferQueue ();
		m_pSemaphore = m_pInput->GetBufferSem ();

		InitPortType ();
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCompSink::GetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentConfigStructure)
{
	switch (nIndex)
	{
	case OMX_IndexConfigTimePosition:
		{
			OMX_TIME_CONFIG_TIMESTAMPTYPE * pTime = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)pComponentConfigStructure;
			pTime->nTimestamp = m_llPlayTime;
			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXBaseComponent::GetConfig (hComponent, nIndex, pComponentConfigStructure);
}

OMX_ERRORTYPE voCOMXCompSink::SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentConfigStructure)
{
	switch (nIndex)
	{
	case OMX_IndexConfigTimePosition:
		{
			OMX_TIME_CONFIG_TIMESTAMPTYPE * pTime = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)pComponentConfigStructure;
			m_llPlayTime = m_llSeekTime = pTime->nTimestamp;
			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXBaseComponent::SetConfig (hComponent, nIndex, pComponentConfigStructure);
}

OMX_ERRORTYPE voCOMXCompSink::SetNewState (OMX_STATETYPE newState)
{
	if (m_tsState.Waiting ())
		m_tsState.Up ();

	if (newState == OMX_StateIdle && (m_sTrans == COMP_TRANSSTATE_PauseToIdle || m_sTrans == COMP_TRANSSTATE_ExecuteToIdle))
	{
		m_pInput->ResetBuffer (OMX_FALSE);

		return OMX_ErrorNone;
	}

	if (newState == OMX_StateExecuting)
		m_bEOS = OMX_FALSE;

	return voCOMXBaseComponent::SetNewState (newState);
}

OMX_U32	voCOMXCompSink::BufferHandle (void)
{
	if (!m_pInput->m_bFlushing)
	{
		if ((m_sState == OMX_StatePause && m_sTrans == COMP_TRANSSTATE_None) || m_sTrans == COMP_TRANSSTATE_ExecuteToPause)
			m_tsState.Down ();
	}
	
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	m_pSemaphore->Down ();
	OMX_BUFFERHEADERTYPE * pBuffer = (OMX_BUFFERHEADERTYPE *) m_pBufferQueue->Remove ();
	if (pBuffer != NULL)
	{
		if (m_sState <= OMX_StateIdle)
		{
			m_pInput->ReturnBuffer (pBuffer);
			return OMX_ErrorInvalidState;
		}

		if (m_sTrans != COMP_TRANSSTATE_None)
		{
			while (m_sTrans == COMP_TRANSSTATE_IdleToExecute || m_sTrans == COMP_TRANSSTATE_PauseToExecute)
				voOMXOS_Sleep (2);
		}
		if (IsRunning () && !m_pInput->IsFlush ())
			 errType = FillBuffer (pBuffer);

		if ((pBuffer->nFlags & OMX_BUFFERFLAG_EOS) && !m_bEOS)
		{
			VOLOGI ("Name %s, Reach EOS!", m_pName);
	
			m_bEOS = OMX_TRUE;
			if (m_pCallBack != NULL)
			{
				m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventBufferFlag, 0, pBuffer->nFlags, NULL); 
			}
		}

		m_pInput->ReturnBuffer (pBuffer);
	}

	return errType;
}
