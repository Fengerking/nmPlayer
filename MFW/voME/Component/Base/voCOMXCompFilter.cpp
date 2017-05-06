	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCompFilter.cpp

	Contains:	voCOMXCompFilter class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "voOMX_Index.h"
#include "voOMXMemory.h"
#include "voOMXOSFun.h"
#include "voOMXFile.h"
#include "voCOMXCompFilter.h"

#define LOG_TAG "voCOMXCompFilter"
#include "voLog.h"

voCOMXCompFilter::voCOMXCompFilter(OMX_COMPONENTTYPE * pComponent)
	: voCOMXBaseComponent (pComponent)
	, m_pInput (NULL)
	, m_pOutput (NULL)
	, m_pInputBuffer (NULL)
	, m_pOutputBuffer (NULL)
	, m_pInputQueue (NULL)
	, m_pInputSem (NULL)
	, m_pOutputQueue (NULL)
	, m_pOutputSem (NULL)
	, m_nOutBuffTime (0)
	, m_nOutBuffSize (0)
	, m_bInputEOS (OMX_FALSE)
	, m_bResetBuffer (OMX_FALSE)
	, m_bFlushed (OMX_FALSE)
	, m_pDumpInputFile (NULL)
	, m_nDumpInputFormat (0)
	, m_pDumpOutputFile (NULL)
	, m_nDumpOutputFormat (0)
	, m_nPassCodec (0)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
}

voCOMXCompFilter::~voCOMXCompFilter(void)
{
	if (m_pDumpInputFile != NULL)
		voOMXFileClose (m_pDumpInputFile);
	if (m_pDumpOutputFile != NULL)
		voOMXFileClose (m_pDumpOutputFile);

	m_bBuffThreadStop = OMX_TRUE;
	ResetPortBuffer ();

	while (m_hBuffThread != NULL)
	{
		if (m_pInputSem->Waiting ())
			m_pInputSem->Up ();
		if (m_pOutputSem->Waiting ())
			m_pOutputSem->Up ();

		voOMXOS_Sleep (1);
	}
}

OMX_ERRORTYPE voCOMXCompFilter::EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent, OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
	m_bInputEOS = OMX_FALSE;

	return voCOMXBaseComponent::EmptyThisBuffer (hComponent, pBuffer);
}

OMX_ERRORTYPE voCOMXCompFilter::CreatePorts (void)
{
	if (m_uPorts == 0)
	{
		m_uPorts = 2;
		m_ppPorts = (voCOMXBasePort **)voOMXMemAlloc (m_uPorts * sizeof (voCOMXBasePort *));
		if (m_ppPorts == NULL)
			return OMX_ErrorInsufficientResources;
		m_pInput = m_ppPorts[0] = new voCOMXBasePort (this, 0, OMX_DirInput);
		if (m_pInput == NULL)
			return OMX_ErrorInsufficientResources;
		m_pOutput = m_ppPorts[1] = new voCOMXBasePort (this, 1, OMX_DirOutput);
		if (m_pOutput == NULL)
			return OMX_ErrorInsufficientResources;

		m_pInput->SetCallbacks (m_pCallBack, m_pAppData);
		m_pOutput->SetCallbacks (m_pCallBack, m_pAppData);

		m_pInputQueue = m_pInput->GetBufferQueue ();
		m_pInputSem = m_pInput->GetBufferSem ();

		m_pOutputQueue = m_pOutput->GetBufferQueue ();
		m_pOutputSem = m_pOutput->GetBufferSem ();

		InitPortType ();
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCompFilter::SetNewState (OMX_STATETYPE newState)
{
	if (newState == OMX_StateIdle && (m_sTrans == COMP_TRANSSTATE_PauseToIdle || m_sTrans == COMP_TRANSSTATE_ExecuteToIdle))
	{
//		ResetPortBuffer ();
		return OMX_ErrorNone;
	}
	else if (m_sTrans == COMP_TRANSSTATE_IdleToLoaded || m_sTrans == COMP_TRANSSTATE_AnyToInvalid)
	{
		m_bBuffThreadStop = OMX_TRUE;

		while (m_hBuffThread != NULL)
		{
			if (m_pInputSem->Waiting ())
				m_pInputSem->Up ();
			if (m_pOutputSem->Waiting ())
				m_pOutputSem->Up ();

			voOMXOS_Sleep (1);
		}

		return OMX_ErrorNone;
	}
	else if (newState == OMX_StateExecuting && m_sTrans == COMP_TRANSSTATE_IdleToExecute)
	{
		m_bInputEOS = OMX_FALSE;
	}

	return voCOMXBaseComponent::SetNewState (newState);
}

OMX_ERRORTYPE voCOMXCompFilter::Flush (OMX_U32	nPort)
{
	ResetPortBuffer ();

	m_pInput->OutputDebugStatus (m_sState);
	m_pOutput->OutputDebugStatus (m_sState);

	voCOMXAutoLock lokc (&m_tmStatus);

	m_pInput->m_bFlushing = OMX_FALSE;
	m_pOutput->m_bFlushing = OMX_FALSE;

	m_bFlushed = OMX_TRUE;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCompFilter::OnDisablePort (OMX_BOOL bFinished, OMX_U32 nPort)
{
	if (OMX_FALSE == bFinished && nPort == 1)
	{
		if (m_pInputSem->Waiting ())
			m_pInputSem->Up ();
	}
	return OMX_ErrorNone;
}

OMX_U32	voCOMXCompFilter::BufferHandle (void)
{
	while((m_pInput->IsSettingsChanging() || m_pOutput->IsSettingsChanging()) && m_sState > OMX_StateIdle)
		voOMXOS_Sleep(2);

	if (m_pInputBuffer == NULL)
	{
//		if (!m_bInputEOS)
		m_pInputSem->Down ();
		m_pInputBuffer = (OMX_BUFFERHEADERTYPE *) m_pInputQueue->Remove ();
		if (m_pDumpInputFile != NULL && m_pInputBuffer != NULL)
		{
			if (m_nDumpInputFormat >= 1)
				voOMXFileWrite (m_pDumpInputFile, (OMX_U8*)&m_pInputBuffer->nFilledLen, 4);
			if (m_nDumpInputFormat >= 2)
				voOMXFileWrite (m_pDumpInputFile, (OMX_U8*)&m_pInputBuffer->nTimeStamp, 8);
			voOMXFileWrite (m_pDumpInputFile, m_pInputBuffer->pBuffer, m_pInputBuffer->nFilledLen);
		}
	}
	VOLOGR ("Name %s. Input Done.", m_pName);

	if (m_pOutputBuffer == NULL)
	{
		m_pOutputSem->Down ();
		m_pOutputBuffer = (OMX_BUFFERHEADERTYPE *) m_pOutputQueue->Remove ();
		if (m_pOutputBuffer != NULL)
		{
			m_pOutputBuffer->nFilledLen = 0;
			m_pOutputBuffer->nFlags = 0;
			m_pOutputBuffer->nTimeStamp = 0;
		}
	}
	VOLOGR ("Name %s. Output Done.", m_pName);

	if (m_sState <= OMX_StateIdle)
	{
		// VOLOGE ("Name %s. The status was wrong.", m_pName);
		if (m_pInputBuffer != NULL)
		{
			m_pInput->ReturnBuffer (m_pInputBuffer);
			m_pInputBuffer = NULL;
		}
		if (m_pOutputBuffer != NULL)
		{
			m_pOutputBuffer->nFilledLen = 0;
			m_pOutputBuffer->nFlags = 0;
			m_pOutputBuffer->nTimeStamp = 0;

			m_pOutput->ReturnBuffer (m_pOutputBuffer);
			m_pOutputBuffer = NULL;
		}

		return OMX_ErrorInvalidState;
	}

	if (m_pInputBuffer == NULL || m_pOutputBuffer == NULL)
	{
		if (!m_bResetBuffer)
		{
			VOLOGW ("Name %s. m_pInputBuffer = 0X%08X || m_pOutputBuffer = 0X%08X.State: %d, Trans %d",
						m_pName,  (int)m_pInputBuffer, (int)m_pOutputBuffer, m_sState, m_sTrans);
		}
		if (m_pInputBuffer != NULL)
		{
			m_pInput->ReturnBuffer (m_pInputBuffer);
			m_pInputBuffer = NULL;
		}

		if (m_pOutputBuffer != NULL)
		{
			if (m_bInputEOS)
			{
				m_pOutputBuffer->nFilledLen = 0;
				m_pOutputBuffer->nFlags += OMX_BUFFERFLAG_EOS;

				voOMXOS_Sleep (10);
			}

			m_pOutputBuffer->nFilledLen = 0;
			m_pOutputBuffer->nTimeStamp = 0;

			m_pOutput->ReturnBuffer (m_pOutputBuffer);
			m_pOutputBuffer = NULL;
		}

		return OMX_ErrorNotImplemented;
	}

	if (m_pInputBuffer->nFilledLen == 0 && (m_pInputBuffer->nFlags & OMX_BUFFERFLAG_EOS) == 0)
	{
		m_pInput->ReturnBuffer (m_pInputBuffer);
		m_pInputBuffer = NULL;

		return OMX_ErrorNone;
	}

	if (!m_pInput->m_bFlushing && !m_pOutput->m_bFlushing)
	{
		if ((m_sState == OMX_StatePause && m_sTrans == COMP_TRANSSTATE_None) || m_sTrans == COMP_TRANSSTATE_ExecuteToPause)
			m_tsState.Down ();
	}

	OMX_BOOL		bEmpty = OMX_FALSE;
	OMX_BOOL		bFill = OMX_FALSE;
	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	m_pOutputBuffer->nFilledLen = 0;
	m_pOutputBuffer->hMarkTargetComponent = m_pInputBuffer->hMarkTargetComponent;
	m_pOutputBuffer->nFlags = (m_pOutputBuffer->nFlags  | m_pInputBuffer->nFlags);
	m_pOutputBuffer->nTimeStamp = m_pInputBuffer->nTimeStamp;
	m_pOutputBuffer->pMarkData = m_pInputBuffer->pMarkData;

	if (m_pInputBuffer->nFlags & OMX_BUFFERFLAG_EOS)
	{
		m_bInputEOS = OMX_TRUE;
		VOLOGI ("Name %s. OMX_BUFFERFLAG_EOS.", m_pName);
	}
	if ((m_pInputBuffer->nFlags & OMX_BUFFERFLAG_EOS) && m_pInputBuffer->nFilledLen == 0)
	{
		m_pInput->ReturnBuffer (m_pInputBuffer);
		m_pInputBuffer = NULL;

		m_pOutputBuffer->nFilledLen  = (OMX_U32)m_nOutBuffSize;
		m_pOutputBuffer->nTimeStamp = m_nOutBuffTime;

		m_pOutput->ReturnBuffer (m_pOutputBuffer);
		m_pOutputBuffer = NULL;
		m_nOutBuffSize = 0;

		VOLOGI ("Name %s. OMX_BUFFERFLAG_EOS.", m_pName);

		return OMX_ErrorNone;
	}

	if (m_sTrans != COMP_TRANSSTATE_None)
	{
		while (m_sTrans == COMP_TRANSSTATE_IdleToExecute || m_sTrans == COMP_TRANSSTATE_PauseToExecute)
			voOMXOS_Sleep (2);
	}

	if (IsRunning () && !m_bResetBuffer)
	{
		if (m_nPassCodec == 0)
		{
			errType = FillBuffer (m_pInputBuffer, &bEmpty, m_pOutputBuffer, &bFill);
		}
		else
		{
			errType = OMX_ErrorNone;
			bEmpty = OMX_TRUE;
			bFill = OMX_TRUE;
		}
	}
	else
	{
		bEmpty = OMX_TRUE;
		bFill = OMX_TRUE;
	}

	VOLOGR ("m_pInputBuffer 0x%08X, m_pOutputBuffer 0x%08X, errType 0x%08X, bEmpty %d, bFill %d", m_pInputBuffer, m_pOutputBuffer, errType, bEmpty, bFill);

	m_bFlushed = OMX_FALSE;

	if (errType != OMX_ErrorNone)
	{
		m_pInput->ReturnBuffer (m_pInputBuffer);
		m_pInputBuffer = NULL;

		m_pOutput->ReturnBuffer (m_pOutputBuffer);
		m_pOutputBuffer = NULL;

		VOLOGW ("Name %s. FillBuffer was failed.", m_pName);

		return errType;
	}

	if (bEmpty)
	{
		m_pInput->ReturnBuffer (m_pInputBuffer);
		m_pInputBuffer = NULL;
	}

	if (bFill)
	{
		if (m_pDumpOutputFile != NULL)
		{
			if (errType == OMX_ErrorNone && m_pOutputBuffer->nFilledLen > 0)
			{
				if (m_nDumpInputFormat >= 1)
					voOMXFileWrite (m_pDumpOutputFile, (OMX_U8*)&m_pOutputBuffer->nFilledLen, 4);
				if (m_nDumpInputFormat >= 2)
					voOMXFileWrite (m_pDumpOutputFile, (OMX_U8*)&m_pOutputBuffer->nTimeStamp, 8);
				voOMXFileWrite (m_pDumpOutputFile, m_pOutputBuffer->pBuffer, m_pOutputBuffer->nFilledLen);
			}
		}

		m_pOutput->ReturnBuffer (m_pOutputBuffer);
		m_pOutputBuffer = NULL;
	}

	return OMX_ErrorNone;
}

void voCOMXCompFilter::ResetPortBuffer (void)
{
	if (m_pInput == NULL || m_pOutput == NULL  || m_sState <= OMX_StateLoaded )
		return;

	m_bResetBuffer = OMX_TRUE;

	OMX_U32 nInputNum = 0;
	OMX_U32 nOutputNum = 0;

	if (m_pInput->IsTunnel () && m_pInput->IsSupplier ())
		nInputNum = m_pInput->m_sType.nBufferCountActual;
	if (m_pOutput->IsTunnel () && m_pOutput->IsSupplier ())
		nOutputNum = m_pOutput->m_sType.nBufferCountActual;

	OMX_U32 nTryTimes = 0;

	// try to wait the buffer reset
	while (m_pOutputQueue->Count () != nOutputNum || m_pOutputBuffer != NULL )
	{
		voOMXOS_Sleep (1);
		if (m_tsState.Waiting ())
			m_tsState.Up ();

		nTryTimes++;
		if (nTryTimes > 200)
			break;
	}

	// Force to rest the buffer
	nTryTimes = 0;
	while (m_pInputQueue->Count () != nInputNum || m_pInputBuffer != NULL ||
		   m_pOutputQueue->Count () != nOutputNum || m_pOutputBuffer != NULL )
	{
		if (m_pOutputBuffer == NULL && (m_pInputBuffer != NULL  || m_pInputQueue->Count () != nInputNum))
		{
			if (m_pOutputSem->Waiting ())
				m_pOutputSem->Up ();
			//Ferry add this for task #8862 time out issue
			 if(m_pOutput->m_bSettingsChanging)
				    m_pOutput->m_bSettingsChanging = OMX_FALSE;
		}
		if ((m_pOutputBuffer != NULL || m_pOutputQueue->Count () != nOutputNum) && m_pInputBuffer == NULL)
		{
			if (m_pInputSem->Waiting ())
				m_pInputSem->Up ();
		}

		if (m_hBuffThread == NULL)
			break;

		voOMXOS_Sleep (1);
		if (m_tsState.Waiting ())
			m_tsState.Up ();

		nTryTimes++;
		if (nTryTimes > 500)
		{
			VOLOGE ("Name %s. Buffer status: Input: %d, %d, Output: %d, %d.", m_pName,
						(int)m_pInputQueue->Count (), (int)nInputNum, (int)m_pOutputQueue->Count (), (int)nOutputNum);
			break;
		}
	}

	m_pOutputSem->Reset ();
	m_pInputSem->Reset ();

	m_pInput->ResetBuffer (OMX_FALSE);
	m_pOutput->ResetBuffer (OMX_FALSE);

	m_bResetBuffer = OMX_FALSE;
}
