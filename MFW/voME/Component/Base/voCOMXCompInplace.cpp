	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCompInplace.cpp

	Contains:	voCOMXCompInplace class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "voOMXBase.h"
#include "voOMXMemory.h"
#include "voCOMXCompInplace.h"

#define LOG_TAG "voCOMXCompInplace"
#include "voLog.h"

voCOMXCompInplace::voCOMXCompInplace(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompFilter (pComponent)
	, m_nInputBufferCount (0)
	, m_ppInputBufferHead (NULL)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
}

voCOMXCompInplace::~voCOMXCompInplace(void)
{
	if (m_ppInputBufferHead != NULL)
		delete []m_ppInputBufferHead;
}

OMX_ERRORTYPE voCOMXCompInplace::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	switch (nIndex)
	{
	case OMX_IndexParamPortDefinition:
		{
			OMX_PARAM_PORTDEFINITIONTYPE * pType = (OMX_PARAM_PORTDEFINITIONTYPE *)pComponentParameterStructure;
			errType = CheckParam ((OMX_COMPONENTTYPE *)hComponent, pType->nPortIndex, pType, sizeof (OMX_PARAM_PORTDEFINITIONTYPE));
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("The header of the parameter was wrong!");
				return errType;
			}

			OMX_PARAM_PORTDEFINITIONTYPE * pOtherType = NULL;
			if (pType->nPortIndex == 0)
				m_pOutput->GetPortType (&pOtherType);
			else
				m_pInput->GetPortType (&pOtherType);

			pOtherType->nBufferCountActual = pType->nBufferCountActual;
			pOtherType->nBufferCountMin = pType->nBufferCountMin;
			pOtherType->nBufferSize = pType->nBufferSize;
			pOtherType->nBufferAlignment = pType->nBufferAlignment;
			pOtherType->bBuffersContiguous = pType->bBuffersContiguous;

			errType = voCOMXCompFilter::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		}
		break;

	default:
		errType = voCOMXCompFilter::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXCompInplace::CreatePorts (void)
{
	if (m_uPorts == 0)
	{
		m_uPorts = 2;
		m_ppPorts = (voCOMXBasePort **)voOMXMemAlloc (m_uPorts * sizeof (voCOMXBasePort *));
		if (m_ppPorts == NULL)
			return OMX_ErrorInsufficientResources;
		m_pInput = m_ppPorts[0] = new voCOMXPortInplace (this, 0, OMX_DirInput);
		if (m_pInput == NULL)
			return OMX_ErrorInsufficientResources;
		m_pOutput = m_ppPorts[1] = new voCOMXPortInplace (this, 1, OMX_DirOutput);
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

OMX_ERRORTYPE voCOMXCompInplace::SetNewState (OMX_STATETYPE newState)
{
	if (newState == OMX_StateIdle && m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{
		m_nInputBufferCount = m_pInput->m_sType.nBufferCountActual;
		if (m_ppInputBufferHead == NULL)
		{
			m_ppInputBufferHead = new OMX_BUFFERHEADERTYPE * [m_nInputBufferCount];
			memset (m_ppInputBufferHead, 0, m_nInputBufferCount * 4);
		}
	}
	else if (newState == OMX_StateIdle && (m_sTrans == COMP_TRANSSTATE_PauseToIdle || m_sTrans == COMP_TRANSSTATE_ExecuteToIdle))
	{
		for (OMX_U32 i = 0; i < m_nInputBufferCount; i++)
		{
			if (m_ppInputBufferHead[i] != NULL)
			{
				m_pInput->ReturnBuffer (m_ppInputBufferHead[i]);
				m_ppInputBufferHead[i] = NULL;
			}
		}

		ResetPortBuffer ();
	}

	return voCOMXCompFilter::SetNewState (newState);
}

OMX_ERRORTYPE voCOMXCompInplace::Flush (OMX_U32	nPort)
{
	for (OMX_U32 i = 0; i < m_nInputBufferCount; i++)
	{
		if (m_ppInputBufferHead[i] != NULL)
		{
			m_pInput->ReturnBuffer (m_ppInputBufferHead[i]);
			m_ppInputBufferHead[i] = NULL;
		}
	}

	return voCOMXCompFilter::Flush (nPort);
}

OMX_ERRORTYPE voCOMXCompInplace::FillThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
	for (OMX_U32 i = 0; i < m_nInputBufferCount; i++)
	{
		if (m_ppInputBufferHead[i] != NULL)
		{
			if (m_ppInputBufferHead[i]->pBuffer == pBuffer->pBuffer)
			{
				m_pInput->ReturnBuffer (m_ppInputBufferHead[i]);
				m_ppInputBufferHead[i] = NULL;
				break;
			}
		}
	}

	return voCOMXCompFilter::FillThisBuffer (hComponent, pBuffer);
}

OMX_ERRORTYPE voCOMXCompInplace::FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
												OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled)
{
	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	*pFilled = OMX_TRUE;
	*pEmptied = OMX_FALSE;

	errType = ModifyBuffer (pInput);

	voOMXBase_CopyBufferHeader (pOutput, pInput);

	for (OMX_U32 i = 0; i < m_nInputBufferCount; i++)
	{
		if (m_ppInputBufferHead[i] == NULL)
		{
			m_ppInputBufferHead[i] = pInput;
			break;
		}
	}
	m_pInputBuffer = NULL;

	return errType;
}
