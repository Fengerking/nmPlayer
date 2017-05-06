	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXBasePort.cpp

	Contains:	voCOMXBasePort class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "voOMXBase.h"
#include "voOMXOSFun.h"
#include "voOMXMemory.h"
#include "voCOMXBaseComponent.h"
#include "voOMXPortAudioConnect.h"
#include "voOMXPortVideoConnect.h"
#include "voCOMXBasePort.h"

#define LOG_TAG "voCOMXBasePort"
#include "voLog.h"

voCOMXBasePort::voCOMXBasePort(voCOMXBaseComponent * pParent, OMX_S32 nIndex, OMX_DIRTYPE dirType)
	: m_nStartDebugTime (0)
	, m_pParent (pParent)
	, m_pMIMEType (NULL)
	, m_nBufferSupplier (OMX_BufferSupplyOutput)
	, m_uBufferCount (0)
	, m_ppBufferHead (NULL)
	, m_ppBufferData (NULL)
	, m_uUsedBuffer (0)
	, m_hTunnelComp (NULL)
	, m_nTunnelPort (-1)
	, m_sStatus (PORT_TRANS_DONE)
	, m_bFlushing (OMX_FALSE)
	, m_bSettingsChanging (OMX_FALSE)
	, m_bResetBuffer (OMX_FALSE)
	, m_pCallBack (NULL)
	, m_pAppData (NULL)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
	voOMXBase_SetHeader (&m_sType, sizeof (OMX_PARAM_PORTDEFINITIONTYPE));
	m_sType.eDir = dirType;
	m_sType.nPortIndex = nIndex;
	m_sType.bEnabled = OMX_TRUE;
	m_sType.bPopulated = OMX_FALSE;
	m_sType.eDomain = OMX_PortDomainMax;

	m_pMIMEType = (OMX_STRING) voOMXMemAlloc (64);
	if (m_pMIMEType != NULL)
		voOMXMemSet (m_pMIMEType, 0, 64);
}

voCOMXBasePort::~voCOMXBasePort(void)
{
	ReleaseBuffer ();

	voOMXMemFree (m_pMIMEType);
}

OMX_ERRORTYPE voCOMXBasePort::GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam)
{
	return OMX_ErrorUnsupportedIndex;
}

OMX_ERRORTYPE voCOMXBasePort::SetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam)
{
	return OMX_ErrorUnsupportedIndex;
}

OMX_ERRORTYPE voCOMXBasePort::GetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig)
{
	return OMX_ErrorUnsupportedIndex;
}

OMX_ERRORTYPE voCOMXBasePort::SetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig)
{
	return OMX_ErrorUnsupportedIndex;
}

OMX_ERRORTYPE voCOMXBasePort::TunnelRequest (OMX_COMPONENTTYPE * hTunneledComp, OMX_U32 nTunneledPort,
											 OMX_TUNNELSETUPTYPE* pTunnelSetup)
{
	m_hTunnelComp = NULL;
	m_nTunnelPort = 0;

	if (hTunneledComp == NULL || pTunnelSetup == NULL)
		return OMX_ErrorNone;

	OMX_ERRORTYPE					errType = OMX_ErrorNone;
	OMX_PARAM_PORTDEFINITIONTYPE	sPortType;
	OMX_PARAM_BUFFERSUPPLIERTYPE	sSupplier;

	voOMXBase_SetHeader (&sPortType, sizeof (OMX_PARAM_PORTDEFINITIONTYPE));
	sPortType.nPortIndex = nTunneledPort;

	errType = hTunneledComp->GetParameter (hTunneledComp, OMX_IndexParamPortDefinition, &sPortType);
	if (errType != OMX_ErrorNone)
	{
		VOLOGW ("Comp %s, Index %d. hTunneledComp->GetParameter (OMX_IndexParamPortDefinition) was failed. 0X%08X",
					m_pParent->GetName (), (int)m_sType.nPortIndex, errType);
		return OMX_ErrorPortsNotCompatible;
	}

	if (sPortType.eDomain != m_sType.eDomain || sPortType.eDir == m_sType.eDir)
	{
		VOLOGW ("Comp %s, Index %d. The domain was not same.", m_pParent->GetName (), (int)m_sType.nPortIndex);
		return OMX_ErrorPortsNotCompatible;
	}

	if (sPortType.nBufferCountActual != m_sType.nBufferCountActual)
	{
		VOLOGW ("Comp %s, Index %d. The buffer count was not same!", m_pParent->GetName (), (int)m_sType.nPortIndex);
		return OMX_ErrorPortsNotCompatible;
	}

	voOMXBase_SetHeader (&sSupplier, sizeof (OMX_PARAM_BUFFERSUPPLIERTYPE));
	sSupplier.nPortIndex = nTunneledPort;
	errType = hTunneledComp->GetParameter (hTunneledComp, OMX_IndexParamCompBufferSupplier, &sSupplier);
	if (errType != OMX_ErrorNone)
	{
		VOLOGW ("Comp %s, Index %d. hTunneledComp->GetParameter (OMX_IndexParamCompBufferSupplier) was failed. 0X%08X",
					m_pParent->GetName (), (int)m_sType.nPortIndex, errType);
		return OMX_ErrorPortsNotCompatible;
	}

	if (m_sType.eDir == OMX_DirInput)
	{
		// Negotiation
		if (pTunnelSetup->nTunnelFlags & OMX_PORTTUNNELFLAG_READONLY)
		{
			pTunnelSetup->eSupplier = OMX_BufferSupplyInput;
			m_nBufferSupplier = OMX_BufferSupplyInput;
		}
		else
		{
			if (pTunnelSetup->eSupplier == OMX_BufferSupplyUnspecified)
			{
				pTunnelSetup->eSupplier = OMX_BufferSupplyInput;
				m_nBufferSupplier = OMX_BufferSupplyInput;
			}
			else
			{
				m_nBufferSupplier = pTunnelSetup->eSupplier;
			}
		}

		sSupplier.nPortIndex = nTunneledPort;
		sSupplier.eBufferSupplier = m_nBufferSupplier;
		errType = hTunneledComp->SetParameter (hTunneledComp, OMX_IndexParamCompBufferSupplier, &sSupplier);
		if (errType != OMX_ErrorNone)
		{
			VOLOGW ("Comp %s, Index %d. hTunneledComp->SetParameter (OMX_IndexParamCompBufferSupplier) was failed. 0X%08X",
						m_pParent->GetName (), (int)m_sType.nPortIndex, errType);
			return OMX_ErrorPortsNotCompatible;
		}

		if (m_sType.eDomain == OMX_PortDomainAudio)
		{
			if (m_sType.eDir == OMX_DirInput)
				errType = voOMXConnectAudioPort ((OMX_COMPONENTTYPE *)hTunneledComp, nTunneledPort, m_pParent->GetComponent (), m_sType.nPortIndex);
			else
				errType = voOMXConnectAudioPort (m_pParent->GetComponent (), m_sType.nPortIndex, (OMX_COMPONENTTYPE *)hTunneledComp, nTunneledPort);
		}
		else if (m_sType.eDomain == OMX_PortDomainVideo)
		{
			if (m_sType.eDir == OMX_DirInput)
				errType = voOMXConnectVideoPort ((OMX_COMPONENTTYPE *)hTunneledComp, nTunneledPort, m_pParent->GetComponent (), m_sType.nPortIndex);
			else
				errType = voOMXConnectVideoPort (m_pParent->GetComponent (), m_sType.nPortIndex, (OMX_COMPONENTTYPE *)hTunneledComp, nTunneledPort);
		}

		if (errType != OMX_ErrorNone)
		{
			VOLOGW ("Comp %s, Index %d. Connect ports was failed. 0X%08X", m_pParent->GetName (), (int)m_sType.nPortIndex, errType);
			return errType;
		}

	}
	else // output port
	{
		pTunnelSetup->eSupplier = OMX_BufferSupplyOutput;
		m_nBufferSupplier = OMX_BufferSupplyOutput;
	}

	m_hTunnelComp = hTunneledComp;
	m_nTunnelPort = nTunneledPort;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBasePort::SetPortType (OMX_PARAM_PORTDEFINITIONTYPE * pParamType)
{
	if (pParamType->nBufferCountActual < m_sType.nBufferCountMin)
	{
		VOLOGE ("Comp %s, Index %d, pParamType->nBufferCountActual < m_sType.nBufferCountMin!", m_pParent->GetName (), (int)m_sType.nPortIndex);
		return OMX_ErrorBadParameter;
	}

	ReleaseBuffer ();

	m_sType.nBufferCountActual = pParamType->nBufferCountActual;
	m_sType.eDomain = pParamType->eDomain;
	m_sType.nBufferSize = pParamType->nBufferSize;

	VOLOGI ("Comp %s, Port %d, Count %d, size %d ", m_pParent->GetName (), (int)m_sType.nPortIndex, (int)m_sType.nBufferCountActual, (int)m_sType.nBufferSize);

	switch (pParamType->eDomain)
	{
	case OMX_PortDomainAudio:
		voOMXMemCopy (&m_sType.format.audio, &pParamType->format.audio, sizeof(OMX_AUDIO_PORTDEFINITIONTYPE));
		break;

	case OMX_PortDomainVideo:
		voOMXMemCopy (&m_sType.format.video, &pParamType->format.video, sizeof(OMX_VIDEO_PORTDEFINITIONTYPE));
		break;

	case OMX_PortDomainImage:
		voOMXMemCopy (&m_sType.format.image, &pParamType->format.image, sizeof(OMX_IMAGE_PORTDEFINITIONTYPE));
		break;

	case OMX_PortDomainOther:
		voOMXMemCopy (&m_sType.format.other, &pParamType->format.other, sizeof(OMX_OTHER_PORTDEFINITIONTYPE));
		break;

	default:
		break;
	}

	if (m_ppBufferHead == NULL)
	{
		OMX_ERRORTYPE errType = AllocateBufferPointer ();
		if (errType != OMX_ErrorNone)
		{
			VOLOGW ("Comp %s, Index %d, AllocateBufferPointer was failed. 0X%08X", m_pParent->GetName (), (int)m_sType.nPortIndex, (int)errType);
			return errType;
		}
	}

	return OMX_ErrorNone;
}


OMX_ERRORTYPE voCOMXBasePort::GetSupplier (OMX_PARAM_BUFFERSUPPLIERTYPE * pSupplier)
{
	pSupplier->eBufferSupplier = m_nBufferSupplier;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBasePort::SetSupplier (OMX_PARAM_BUFFERSUPPLIERTYPE * pSupplier)
{
	if (IsTunnel ())
	{
		if (pSupplier->eBufferSupplier != m_nBufferSupplier)
			return OMX_ErrorNotImplemented;
		else
			return OMX_ErrorNone;
	}

	m_nBufferSupplier = pSupplier->eBufferSupplier;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBasePort::Flush (void)
{
	ResetBuffer (OMX_FALSE);

	OutputDebugStatus (m_pParent->GetCompState ());

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBasePort::Disable (void)
{
	if (m_sType.bEnabled == OMX_FALSE)
		return OMX_ErrorNone;

	m_sStatus = PORT_TRANS_ENA2DIS;

	if (m_pParent->m_sState != OMX_StateLoaded)
	{
		m_pParent->OnDisablePort (OMX_FALSE, m_sType.nPortIndex);

		ResetBuffer (OMX_FALSE);

		if (IsTunnel ())
		{
			if (!IsSupplier ())
			{
				// Wait till all buffers were freed.
				m_tsAlloc.Down ();
				m_tsBuffer.Reset ();
			}
			else
			{
				FreeTunnelBuffer (m_sType.nPortIndex);

				while (m_tqBuffer.Count () > 0)
				{
					m_tqBuffer.Remove ();
				}
				//must do reset
				//East 2009/11/23
				m_tsBuffer.Reset();
			}
		}
		else
		{
			m_tsAlloc.Down ();
			m_tsBuffer.Reset ();
		}

		m_pParent->OnDisablePort (OMX_TRUE, m_sType.nPortIndex);
	}

	m_sType.bEnabled = OMX_FALSE;
	m_sStatus = PORT_TRANS_DONE;
	m_uBufferCount = 0;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBasePort::Enable (void)
{
	if (m_sType.bEnabled == OMX_TRUE)
		return OMX_ErrorNone;

	m_sStatus = PORT_TRANS_DIS2ENA;

	if (m_pParent->m_sState != OMX_StateLoaded)
	{
		if (IsTunnel ())
		{
			if (IsSupplier ())
			{
				AllocTunnelBuffer (m_sType.nPortIndex, m_sType.nBufferSize);
			}
			else
			{
				m_tsAlloc.Down ();
			}
		}
		else
		{
			m_tsAlloc.Down ();
		}

		if (m_pParent->GetCompState () == OMX_StateExecuting || m_pParent->GetCompState () == OMX_StatePause)
		{
			if (IsTunnel () && IsSupplier ())
			{
				for (OMX_U32 i = 0; i < m_tqBuffer.Count (); i++)
					m_tsBuffer.Up ();
			}
		}
	}

	m_sType.bEnabled = OMX_TRUE;
	m_sStatus = PORT_TRANS_DONE;
	m_bSettingsChanging = OMX_FALSE;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBasePort::AllocBuffer (OMX_BUFFERHEADERTYPE** pBuffer, OMX_U32 nPortIndex,
										   OMX_PTR pAppPrivate, OMX_U32 nSizeBytes)
{
	if (nPortIndex != m_sType.nPortIndex)
	{
		VOLOGE ("Comp %s, Index %d, nPortIndex != m_sType.nPortIndex.", m_pParent->GetName (), (int)m_sType.nPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	if (!IsEnable () && m_sStatus != PORT_TRANS_DIS2ENA)
		return OMX_ErrorNone;

	if (IsTunnel () && IsSupplier ())
	{
		VOLOGE ("Comp %s, Index %d, IsTunnel () && IsSupplier ().", m_pParent->GetName (), (int)m_sType.nPortIndex);
		return OMX_ErrorNotImplemented;
	}

	if (!IsSupplier ())
	{
		if ( m_sType.eDir == OMX_DirInput )
			m_nBufferSupplier = OMX_BufferSupplyInput;
		else if (m_sType.eDir == OMX_DirOutput)
			m_nBufferSupplier = OMX_BufferSupplyOutput ;
	}

	if (m_pParent->m_sTrans != COMP_TRANSSTATE_LoadedToIdle && m_sStatus != PORT_TRANS_DIS2ENA)
	{
		VOLOGE ("Comp %s, Index %d, m_pParent->m_sTrans != COMP_TRANSSTATE_LoadedToIdle && m_sStatus != PORT_TRANS_DIS2ENA.",
					m_pParent->GetName (), (int)m_sType.nPortIndex);
		return OMX_ErrorIncorrectStateTransition;
	}

	if (nSizeBytes < m_sType.nBufferSize)
	{
		VOLOGE ("Comp %s, Index %d, nSizeBytes < m_sType.nBufferSize.", m_pParent->GetName (), (int)m_sType.nPortIndex);
		return OMX_ErrorIncorrectStateTransition;
	}

	if (m_ppBufferHead == NULL)
	{
		OMX_ERRORTYPE errType = AllocateBufferPointer ();
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("Comp %s, Index %d, AllocateBufferPointer was failed. 0X%08X.",
						m_pParent->GetName (), (int)m_sType.nPortIndex, errType);
			return errType;
		}
	}

	for (OMX_U32 i = 0; i < m_sType.nBufferCountActual; i++)
	{
		if (m_ppBufferHead[i] == NULL)
		{
			m_ppBufferHead[i] = (OMX_BUFFERHEADERTYPE *) voOMXMemAlloc (sizeof (OMX_BUFFERHEADERTYPE));
			if (m_ppBufferHead[i] == NULL)
			{
				VOLOGE ("Comp %s, Index %d, m_ppBufferHead[i] == NULL.", m_pParent->GetName (), (int)m_sType.nPortIndex);
				return OMX_ErrorInsufficientResources;
			}
			m_uBufferCount++;

			voOMXMemSet (m_ppBufferHead[i], 0, sizeof (OMX_BUFFERHEADERTYPE));
			voOMXBase_SetHeader (m_ppBufferHead[i], sizeof (OMX_BUFFERHEADERTYPE));

			m_ppBufferData[i] = (OMX_U8*) voOMXMemAlloc (nSizeBytes);
			if (m_ppBufferData[i] == NULL)
			{
				VOLOGE ("Comp %s, Index %d, m_ppBufferData[i] == NULL.", m_pParent->GetName (), (int)m_sType.nPortIndex);
				return OMX_ErrorInsufficientResources;
			}
			m_ppBufferHead[i]->pBuffer = m_ppBufferData[i];

			m_ppBufferHead[i]->nAllocLen = nSizeBytes;
			m_ppBufferHead[i]->pPlatformPrivate = this;
			m_ppBufferHead[i]->pAppPrivate = pAppPrivate;

			if (m_sType.eDir == OMX_DirInput)
				m_ppBufferHead[i]->nInputPortIndex = m_sType.nPortIndex;
			else
				m_ppBufferHead[i]->nOutputPortIndex = m_sType.nPortIndex;

			*pBuffer = m_ppBufferHead[i];
			m_uUsedBuffer++;

			if (m_sType.nBufferCountActual == m_uUsedBuffer)
			{
				m_sType.bPopulated = OMX_TRUE;
				m_tsAlloc.Up ();
			}

			return OMX_ErrorNone;
		}
	}

	return OMX_ErrorInsufficientResources;
}

OMX_ERRORTYPE voCOMXBasePort::UseBuffer (OMX_BUFFERHEADERTYPE** pBuffer, OMX_U32 nPortIndex,
										   OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8 * pData)
{
	if (nPortIndex != m_sType.nPortIndex)
	{
		VOLOGE ("Comp %s, Index %d, nPortIndex != m_sType.nPortIndex.", m_pParent->GetName (), (int)m_sType.nPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	if (!IsEnable () && m_sStatus != PORT_TRANS_DIS2ENA)
		return OMX_ErrorNone;

	if (IsSupplier ())
	{
		VOLOGE ("Comp %s, Index %d, IsSupplier.", m_pParent->GetName (), (int)m_sType.nPortIndex);
		return OMX_ErrorNotImplemented;
	}

	if (!IsTunnel ())
	{
		if (m_pParent->m_sTrans != COMP_TRANSSTATE_LoadedToIdle && m_sStatus != PORT_TRANS_DIS2ENA)
		{
			VOLOGE ("Comp %s, Index %d, m_pParent->m_sTrans != COMP_TRANSSTATE_LoadedToIdle && m_sStatus != PORT_TRANS_DIS2ENA.",
						m_pParent->GetName (), (int)m_sType.nPortIndex);
			return OMX_ErrorIncorrectStateTransition;
		}
	}

	if (m_ppBufferHead == NULL)
	{
		OMX_ERRORTYPE errType = AllocateBufferPointer ();
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("Comp %s, Index %d, AllocateBufferPointer.", m_pParent->GetName (), (int)m_sType.nPortIndex);
			return errType;
		}
	}

	for (OMX_U32 i = 0; i < m_sType.nBufferCountActual; i++)
	{
		if (m_ppBufferHead[i] == NULL)
		{
			m_ppBufferHead[i] = (OMX_BUFFERHEADERTYPE *) voOMXMemAlloc (sizeof (OMX_BUFFERHEADERTYPE));
			if (m_ppBufferHead[i] == NULL)
			{
				VOLOGE ("Comp %s, Index %d, m_ppBufferHead[i] == NULL.", m_pParent->GetName (), (int)m_sType.nPortIndex);
				return OMX_ErrorInsufficientResources;
			}

			m_uBufferCount++;
			voOMXMemSet (m_ppBufferHead[i], 0, sizeof (OMX_BUFFERHEADERTYPE));
			voOMXBase_SetHeader (m_ppBufferHead[i], sizeof (OMX_BUFFERHEADERTYPE));

			m_ppBufferHead[i]->pBuffer = pData;

			m_ppBufferHead[i]->nAllocLen = nSizeBytes;
			m_ppBufferHead[i]->pPlatformPrivate = this;
			m_ppBufferHead[i]->pAppPrivate = pAppPrivate;

			if (m_sType.eDir == OMX_DirInput)
				m_ppBufferHead[i]->nInputPortIndex = m_sType.nPortIndex;
			else
				m_ppBufferHead[i]->nOutputPortIndex = m_sType.nPortIndex;

			*pBuffer = m_ppBufferHead[i];
			m_uUsedBuffer++;

			if (m_sType.nBufferCountActual == m_uUsedBuffer)
			{
				m_sType.bPopulated = OMX_TRUE;
				m_tsAlloc.Up ();
			}

			return OMX_ErrorNone;
		}
	}

	return OMX_ErrorInsufficientResources;
}

OMX_ERRORTYPE voCOMXBasePort::FreeBuffer (OMX_IN  OMX_U32 nPortIndex,
										OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
	if (nPortIndex != m_sType.nPortIndex)
	{
		VOLOGE ("Comp %s, Index %d, nPortIndex != m_sType.nPortIndex.", m_pParent->GetName (), (int)m_sType.nPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	if (!IsEnable ())
		return OMX_ErrorNone;

	if (IsTunnel () && IsSupplier ())
	{
		VOLOGE ("Comp %s, Index %d, IsTunnel () && IsSupplier ().", m_pParent->GetName (), (int)m_sType.nPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	if (!IsTunnel ())
	{
		if (m_pParent->m_sTrans != COMP_TRANSSTATE_IdleToLoaded)
		{
			if (m_sStatus != PORT_TRANS_ENA2DIS)
			{
				m_pCallBack->EventHandler (m_pParent->m_pComponent, m_pAppData, OMX_EventError,
											OMX_ErrorPortUnpopulated, nPortIndex, NULL);

				VOLOGE ("Comp %s, Index %d, m_sStatus != PORT_TRANS_ENA2DIS.", m_pParent->GetName (), (int)m_sType.nPortIndex);
				return OMX_ErrorIncorrectStateTransition;
			}
		}
	}

//	for (OMX_U32 i = 0; i < m_sType.nBufferCountActual; i++)
	for (OMX_U32 i = 0; i < m_uBufferCount; i++)
	{
		if (m_ppBufferHead[i] == pBuffer)
		{
			if (m_ppBufferData[i] != NULL)
				voOMXMemFree (m_ppBufferData[i]);
			m_ppBufferData[i] = NULL;

			voOMXMemFree (m_ppBufferHead[i]);
			m_ppBufferHead[i] = NULL;
			m_uUsedBuffer--;

			if (m_uUsedBuffer == 0)
			{
				while (m_tqBuffer.Count () > 0)
					m_tqBuffer.Remove ();
				m_sType.bPopulated = OMX_FALSE;
				m_tsAlloc.Up ();
			}

			return OMX_ErrorNone;
		}
	}

	return OMX_ErrorIncorrectStateTransition;
}

OMX_ERRORTYPE voCOMXBasePort::AllocTunnelBuffer (OMX_U32 nPortIndex, OMX_U32 nSizeBytes)
{
	if (nPortIndex != m_sType.nPortIndex)
	{
		VOLOGE ("Comp %s, Index %d, nPortIndex != m_sType.nPortIndex.", m_pParent->GetName (), (int)m_sType.nPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	if (!IsEnable () && m_sStatus != PORT_TRANS_DIS2ENA)
		return OMX_ErrorNone;

	if (!IsTunnel () || !IsSupplier ())
	{
		VOLOGE ("Comp %s, Index %d, !IsTunnel () || !IsSupplier ().", m_pParent->GetName (), (int)m_sType.nPortIndex);
		return OMX_ErrorNotImplemented;
	}

	if (m_pParent->m_sTrans != COMP_TRANSSTATE_LoadedToIdle)
	{
		if (m_sStatus != PORT_TRANS_DIS2ENA)
		{
			VOLOGE ("Comp %s, Index %d, m_sStatus != PORT_TRANS_DIS2ENA.", m_pParent->GetName (), (int)m_sType.nPortIndex);
			return OMX_ErrorIncorrectStateTransition;
		}
	}

	if (nSizeBytes < m_sType.nBufferSize)
	{
		VOLOGE ("Comp %s, Index %d, nSizeBytes < m_sType.nBufferSize.", m_pParent->GetName (),(int)m_sType.nPortIndex);
		return OMX_ErrorIncorrectStateTransition;
	}

	if (m_ppBufferHead == NULL)
	{
		OMX_ERRORTYPE errType = AllocateBufferPointer ();
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("Comp %s, Index %d, AllocateBufferPointer was failed. 0X%08X.", m_pParent->GetName (), (int)m_sType.nPortIndex, errType);
			return errType;
		}
	}

	for (OMX_U32 i = 0; i < m_sType.nBufferCountActual; i++)
	{
		if (m_ppBufferHead[i] == NULL)
		{
			m_ppBufferData[i] = (OMX_U8*) voOMXMemAlloc (nSizeBytes);
			if (m_ppBufferData[i] == NULL)
				return OMX_ErrorInsufficientResources;
			
			m_uBufferCount++;

			OMX_ERRORTYPE errType = OMX_ErrorNone;
			OMX_U32 nTryTimes = 0;
			while (nTryTimes < 100)
			{
				errType = m_hTunnelComp->UseBuffer (m_hTunnelComp, &m_ppBufferHead[i], m_nTunnelPort,
													NULL, nSizeBytes, m_ppBufferData[i]);
				if (errType == OMX_ErrorIncorrectStateTransition)
				{
					voOMXOS_Sleep (10);
					nTryTimes++;
					continue;
				}
				else if (errType != OMX_ErrorNone)
				{
					voOMXMemFree (m_ppBufferData[i]);
					m_ppBufferData[i]  = NULL;

					VOLOGE ("Comp %s, Index %d, m_hTunnelComp->UseBuffer was failed. 0X%08X.", m_pParent->GetName (), (int)m_sType.nPortIndex, errType);
					return errType;
				}
				else
				{
					if (m_ppBufferHead[i] != NULL)
						break;
				}

				if (m_pParent->m_sTrans != COMP_TRANSSTATE_LoadedToIdle && m_sStatus != PORT_TRANS_DIS2ENA)
				{
					VOLOGE ("Comp %s, Index %d was failed in wrong status.", m_pParent->GetName (), (int)m_sType.nPortIndex);
					return OMX_ErrorInsufficientResources;
				}
			}
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Comp %s, Index %d, m_hTunnelComp->UseBuffer was failed. 0X%08X.", m_pParent->GetName (), (int)m_sType.nPortIndex, errType);
				return OMX_ErrorInsufficientResources;
			}

			if (m_ppBufferHead[i] == NULL)
			{
				VOLOGE ("Comp %s, Index %d, m_hTunnelComp->UseBuffer was failed. 0X%08X.", m_pParent->GetName (), (int)m_sType.nPortIndex, errType);
				return OMX_ErrorInsufficientResources;
			}

			if (m_sType.eDir == OMX_DirInput)
			{
				m_ppBufferHead[i]->nInputPortIndex = m_sType.nPortIndex;
				m_ppBufferHead[i]->nOutputPortIndex = m_nTunnelPort;
			}
			else
			{
				m_ppBufferHead[i]->nInputPortIndex = m_nTunnelPort;
				m_ppBufferHead[i]->nOutputPortIndex = m_sType.nPortIndex;
			}

			m_tqBuffer.Add (m_ppBufferHead[i]);
			m_uUsedBuffer++;

			if (m_sType.nBufferCountActual == m_uUsedBuffer)
				m_sType.bPopulated = OMX_TRUE;
		}
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBasePort::FreeTunnelBuffer (OMX_U32 nPortIndex)
{
	if (nPortIndex != m_sType.nPortIndex)
	{
		VOLOGE ("Comp %s, Index %d, nPortIndex != m_sType.nPortIndex.", m_pParent->GetName (), (int)m_sType.nPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	if (!IsEnable ())
		return OMX_ErrorNone;

	if (!IsTunnel () || !IsSupplier ())
	{
		VOLOGE ("Comp %s, Index %d, !IsTunnel () || !IsSupplier ().", m_pParent->GetName (), (int)m_sType.nPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	if (m_pParent->m_sTrans != COMP_TRANSSTATE_IdleToLoaded)
	{
		if (m_sStatus != PORT_TRANS_ENA2DIS)
		{
			m_pCallBack->EventHandler (m_pParent->m_pComponent, m_pAppData, OMX_EventError,
										OMX_ErrorPortUnpopulated, nPortIndex, NULL);

			VOLOGE ("Comp %s, Index %d, m_sStatus != PORT_TRANS_ENA2DIS.", m_pParent->GetName (), (int)m_sType.nPortIndex);
			return OMX_ErrorIncorrectStateTransition;
		}
	}

//	for (OMX_U32 i = 0; i < m_sType.nBufferCountActual; i++)
	for (OMX_U32 i = 0; i < m_uBufferCount; i++)
	{
		if (m_ppBufferHead[i] != NULL)
		{
			voOMXMemFree (m_ppBufferData[i]);
			m_ppBufferData[i] = NULL;

			OMX_ERRORTYPE errType = OMX_ErrorNone;
			OMX_U32 nTryTimes = 0;
			while (nTryTimes < 500)
			{
				errType = m_hTunnelComp->FreeBuffer (m_hTunnelComp, m_nTunnelPort, m_ppBufferHead[i]);
				if (errType == OMX_ErrorIncorrectStateTransition)
				{
					voOMXOS_Sleep (1);
					nTryTimes++;
					continue;
				}
				else
				{
					break;
				}
			}

			if (nTryTimes >= 500)
				VOLOGE ("Comp %s, Index %d, m_hTunnelComp->FreeBuffer was failed. 0X%08X.", m_pParent->GetName (), (int)m_sType.nPortIndex, errType);

			m_ppBufferHead[i] = NULL;
			m_uUsedBuffer--;

			if (m_uUsedBuffer == 0)
			{
				m_sType.bPopulated = OMX_FALSE;
			}
		}
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBasePort::HandleBuffer (OMX_BUFFERHEADERTYPE* pBuffer)
{
	if (!IsEnable () && m_sStatus != PORT_TRANS_DIS2ENA)
	{
		VOLOGE ("Comp %s, Index %d, !IsEnable () && m_sStatus != PORT_TRANS_DIS2ENA.", m_pParent->GetName (), (int)m_sType.nPortIndex);
		return OMX_ErrorIncorrectStateOperation;
	}

	OMX_U32	nPortIndex = (m_sType.eDir == OMX_DirInput) ? pBuffer->nInputPortIndex : pBuffer->nOutputPortIndex;
	if (nPortIndex != m_sType.nPortIndex)
	{
		VOLOGE ("Comp %s, Index %d, nPortIndex != m_sType.nPortIndex.", m_pParent->GetName (), (int)m_sType.nPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	if (IsTunnel () && IsSupplier ())
	{
		OMX_TRANS_STATE transType = m_pParent->GetTransState ();
		OMX_STATETYPE	stateType = m_pParent->GetCompState ();

		if (IsFlush () || m_sStatus == PORT_TRANS_ENA2DIS)
		{
			m_tqBuffer.Add (pBuffer);
			return OMX_ErrorNone;
		}

		if (transType == COMP_TRANSSTATE_ExecuteToIdle ||
			transType == COMP_TRANSSTATE_PauseToIdle)
		{
			m_tqBuffer.Add (pBuffer);
			return OMX_ErrorNone;
		}

		if (stateType == OMX_StateIdle &&
			(transType != COMP_TRANSSTATE_IdleToExecute && transType != COMP_TRANSSTATE_IdleToPause))
		{
			m_tqBuffer.Add (pBuffer);
			return OMX_ErrorNone;
		}
	}

	m_tqBuffer.Add (pBuffer);
	m_tsBuffer.Up ();

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBasePort::ReturnBuffer (OMX_BUFFERHEADERTYPE* pBuffer)
{
	if (!IsEnable () && m_sStatus != PORT_TRANS_DIS2ENA)
	{
		VOLOGE ("Comp %s, Index %d, !IsEnable () && m_sStatus != PORT_TRANS_DIS2ENA.", m_pParent->GetName (), (int)m_sType.nPortIndex);
		return OMX_ErrorIncorrectStateOperation;
	}

	OMX_U32	nPortIndex = (m_sType.eDir == OMX_DirInput) ? pBuffer->nInputPortIndex : pBuffer->nOutputPortIndex;
	if (nPortIndex != m_sType.nPortIndex)
	{
		VOLOGE ("Comp %s, Index %d, nPortIndex != m_sType.nPortIndex.", m_pParent->GetName (), (int)m_sType.nPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (IsTunnel ())
	{
		if (IsSupplier ())
		{
			OMX_TRANS_STATE transType = m_pParent->GetTransState ();
			OMX_STATETYPE	stateType = m_pParent->GetCompState ();

			if (IsFlush () || m_sStatus == PORT_TRANS_ENA2DIS)
			{
				m_tqBuffer.Add (pBuffer);
				return OMX_ErrorNone;
			}

			if (transType == COMP_TRANSSTATE_ExecuteToIdle ||
				transType == COMP_TRANSSTATE_PauseToIdle)
			{
				m_tqBuffer.Add (pBuffer);
				return OMX_ErrorNone;
			}

			if (stateType == OMX_StateIdle &&
				(transType != COMP_TRANSSTATE_IdleToExecute && transType != COMP_TRANSSTATE_IdleToPause))
			{
				m_tqBuffer.Add (pBuffer);
				return OMX_ErrorNone;
			}
		}

		OMX_U32 nTimes = 0;
		do 
		{
			if (m_sType.eDir == OMX_DirInput)
			{
				pBuffer->nOutputPortIndex = m_nTunnelPort;
				pBuffer->nInputPortIndex = m_sType.nPortIndex;

				errType = m_hTunnelComp->FillThisBuffer (m_hTunnelComp, pBuffer);
			}
			else
			{
				pBuffer->nInputPortIndex = m_nTunnelPort;
				pBuffer->nOutputPortIndex = m_sType.nPortIndex;

				errType = m_hTunnelComp->EmptyThisBuffer (m_hTunnelComp, pBuffer);
			}

			// try 100ms to avoid lost buffer when disable->enable, East 20111104
			if(OMX_ErrorNone != errType)
			{
				VOLOGW ("%s tunnel component return 0x%08X", m_pParent->GetName(), errType);

				if(OMX_ErrorIncorrectStateOperation == errType)
				{
					if(nTimes++ > 50)
					{
						// have to lost buffer, force return Buffer to avoid hang, East 20111104
						VOLOGW ("%s OMX_ErrorIncorrectStateOperation TimeOut", m_pParent->GetName());
						m_tqBuffer.Add (pBuffer);
						m_tsBuffer.Up ();
						break;
					}

					voOMXOS_Sleep(2);
				}
			}
		} while (OMX_ErrorIncorrectStateOperation == errType);
	}
	else
	{
		if (m_sType.eDir == OMX_DirInput)
			errType = m_pCallBack->EmptyBufferDone (m_pParent->m_pComponent, m_pAppData, pBuffer);
		else
			errType = m_pCallBack->FillBufferDone (m_pParent->m_pComponent, m_pAppData, pBuffer);
	}

	return errType;
}

OMX_ERRORTYPE voCOMXBasePort::SetNewCompState (OMX_STATETYPE sNew, OMX_TRANS_STATE sTrans)
{
	if (!IsEnable () && m_sStatus != PORT_TRANS_DIS2ENA)
	{
		VOLOGE ("Comp %s, Index %d, !IsEnable () && m_sStatus != PORT_TRANS_DIS2ENA.", m_pParent->GetName (), (int)m_sType.nPortIndex);
		return OMX_ErrorIncorrectStateOperation;
	}

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (sNew == OMX_StateIdle && sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{
		if (IsTunnel () && IsSupplier ())
		{
			errType = AllocTunnelBuffer (m_sType.nPortIndex, m_sType.nBufferSize);
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Comp %s, Index %d, AllocTunnelBuffer was failed. 0X%08X.", m_pParent->GetName (), (int)m_sType.nPortIndex, errType);
				return errType;
			}
		}
		else
		{
			m_tsAlloc.Down ();
		}
	}
	else if (sNew == OMX_StateIdle && (sTrans == COMP_TRANSSTATE_ExecuteToIdle || sTrans == COMP_TRANSSTATE_PauseToIdle))
	{
	}
	else if ((sNew == OMX_StateLoaded && sTrans == COMP_TRANSSTATE_IdleToLoaded) || sTrans == COMP_TRANSSTATE_AnyToInvalid)
	{
//		if (m_tsBuffer.Waiting ())
//			m_tsBuffer.Up ();

		if (IsTunnel () && IsSupplier ())
		{
			errType = FreeTunnelBuffer (m_sType.nPortIndex);
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Comp %s, Index %d, FreeTunnelBuffer was failed. 0X%08X.", m_pParent->GetName (), (int)m_sType.nPortIndex, errType);
				return errType;
			}
		}
		else
		{
			m_tsAlloc.Down ();
		}
	}
	else if (sNew == OMX_StateExecuting && (sTrans == COMP_TRANSSTATE_IdleToExecute || sTrans == COMP_TRANSSTATE_PauseToExecute))
	{
		if (IsTunnel () && IsSupplier ())
		{
			OMX_STATETYPE	nState = OMX_StateLoaded;
			OMX_U32			nTryTimes = 0;
			m_hTunnelComp->GetState (m_hTunnelComp, &nState);

			while (nState != OMX_StateExecuting && nState != OMX_StatePause)
			{
				voOMXOS_Sleep (1);
				m_hTunnelComp->GetState (m_hTunnelComp, &nState);

				nTryTimes++;
				if (nTryTimes > 500)
				{
					VOLOGE ("Comp %s, Index %d, m_hTunnelComp->GetState was not run", m_pParent->GetName (), (int)m_sType.nPortIndex);
					break;
				}
			}

			if (sTrans == COMP_TRANSSTATE_IdleToExecute || m_bResetBuffer)
			{
				m_bResetBuffer = OMX_FALSE;
				for (OMX_U32 i = 0; i < m_tqBuffer.Count (); i++)
					m_tsBuffer.Up ();
			}
		}
	}

	OutputDebugStatus (sNew);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBasePort::SetCallbacks (OMX_CALLBACKTYPE* pCallbacks, OMX_PTR pAppData)
{
	m_pCallBack = pCallbacks;
	m_pAppData = pAppData;

	return OMX_ErrorNone;
}

OMX_BOOL voCOMXBasePort::IsTunnel (void)
{
	if (m_hTunnelComp != NULL)
		return OMX_TRUE;
	else
		return OMX_FALSE;
}

OMX_BOOL voCOMXBasePort::IsSupplier (void)
{
	if (m_nBufferSupplier == OMX_BufferSupplyInput && m_sType.eDir == OMX_DirInput)
		return OMX_TRUE;
	else if (m_nBufferSupplier == OMX_BufferSupplyOutput && m_sType.eDir == OMX_DirOutput)
		return OMX_TRUE;
	else
		return OMX_FALSE;
}

OMX_BOOL voCOMXBasePort::IsEnable (void)
{
	return m_sType.bEnabled;
}

OMX_ERRORTYPE voCOMXBasePort::ResetBuffer (OMX_BOOL bRelease)
{
	if (m_sType.bPopulated == OMX_FALSE)
		return OMX_ErrorNone;

	if (!IsEnable () && m_sStatus != PORT_TRANS_DIS2ENA)
		return OMX_ErrorNone;

	OMX_U32	nResetBuffers = 0;

	if (IsTunnel () && IsSupplier ())
		nResetBuffers = m_sType.nBufferCountActual;

	OMX_U32 nTryTimes = 0;
	while (m_tqBuffer.Count () !=  nResetBuffers)
	{
		voOMXOS_Sleep (1);
		if (m_pParent->m_tsState.Waiting ())
			m_pParent->m_tsState.Up ();

		if (bRelease)
		{
			if (m_tsBuffer.Waiting ())
				m_tsBuffer.Up ();
		}

		nTryTimes++;
		if (nTryTimes > 1000)
		{
			VOLOGE ("Comp %s, Index %d, Time Out", m_pParent->GetName (), (int)m_sType.nPortIndex);
			break;
		}
	}

//	if (IsSupplier ()  && m_sType.eDomain != OMX_PortDomainOther)
	if (!bRelease)
	{
		if (m_ppBufferHead != NULL)
		{
//			for (OMX_U32 i = 0; i < m_sType.nBufferCountActual; i++)
			for (OMX_U32 i = 0; i < m_uBufferCount; i++)
			{
				if (m_ppBufferHead[i] != NULL)
				{
					m_ppBufferHead[i]->nFilledLen = 0;
					m_ppBufferHead[i]->nTimeStamp = 0;
					m_ppBufferHead[i]->hMarkTargetComponent = NULL;
					m_ppBufferHead[i]->pMarkData = NULL;
					m_ppBufferHead[i]->nFlags = 0;
				}
			}
		}
	}

	m_tsBuffer.Reset ();

	m_bResetBuffer = OMX_TRUE;

	return OMX_ErrorNone;
}

void voCOMXBasePort::ReleaseBuffer (void)
{
	if (m_ppBufferHead == NULL)
		return;

//	for (OMX_U32 i = 0; i < m_sType.nBufferCountActual; i++)
	for (OMX_U32 i = 0; i < m_uBufferCount; i++)
	{
		if (m_ppBufferData[i] != NULL)
		{
			voOMXMemFree (m_ppBufferData[i]);
			m_ppBufferData[i] = NULL;
		}

		if (m_ppBufferHead[i] != NULL)
		{
			if (!IsTunnel () || !IsSupplier ())
				voOMXMemFree (m_ppBufferHead[i]);

			m_ppBufferHead[i] = NULL;
		}
	}

	voOMXMemFree (m_ppBufferHead);
	m_ppBufferHead = NULL;

	voOMXMemFree (m_ppBufferData);
	m_ppBufferData = NULL;

	m_uBufferCount = 0;
}

OMX_ERRORTYPE voCOMXBasePort::AllocateBufferPointer (void)
{
	m_ppBufferHead = (OMX_BUFFERHEADERTYPE **)voOMXMemAlloc (m_sType.nBufferCountActual * 4);
	if (m_ppBufferHead == NULL)
		return OMX_ErrorInsufficientResources;

	voOMXMemSet (m_ppBufferHead, 0, m_sType.nBufferCountActual * 4);

	m_ppBufferData = (OMX_U8 **)voOMXMemAlloc (m_sType.nBufferCountActual * 4);
	if (m_ppBufferData == NULL)
		return OMX_ErrorInsufficientResources;

	voOMXMemSet (m_ppBufferData, 0, m_sType.nBufferCountActual * 4);

	return OMX_ErrorNone;
}

void voCOMXBasePort::OutputDebugStatus (OMX_STATETYPE nNewState)
{
	if (!IsEnable ())
		return;

	char szMsg[256];
	char szTmp[64];

	sprintf (szMsg, "***Comp: %s Port: %d", m_pParent->m_pName, (int)m_sType.nPortIndex);
	if (IsTunnel ())
		strcat (szMsg, " T+ ");
	else
		strcat (szMsg, " T- ");

	if (IsSupplier ())
		strcat (szMsg, " S+ ");
	else
		strcat (szMsg, " S- ");

	if (IsFlush ())
		strcat (szMsg, " F+ ");
	else
		strcat (szMsg, " F- ");

	switch (m_pParent->m_sState)
	{
	case OMX_StateLoaded:
		strcat (szMsg, " LOAD ");
		break;
	case OMX_StateIdle:
		strcat (szMsg, " IDLE ");
		break;
	case OMX_StateExecuting:
		strcat (szMsg, " EXEC ");
		break;
	case OMX_StatePause:
		strcat (szMsg, " PAUS ");
		break;
	default: break;
	}

	strcat (szMsg, " TO ");

	switch (nNewState)
	{
	case OMX_StateLoaded:
		strcat (szMsg, " LOAD ");
		break;
	case OMX_StateIdle:
		strcat (szMsg, " IDLE ");
		break;
	case OMX_StateExecuting:
		strcat (szMsg, " EXEC ");
		break;
	case OMX_StatePause:
		strcat (szMsg, " PAUS ");
		break;
	default: break;
	}

	if (m_nStartDebugTime == 0)
		m_nStartDebugTime = voOMXOS_GetSysTime ();

	sprintf (szTmp, "    Buff: %d / %d    Sem: %d    T:%d \r\n", (int)m_tqBuffer.Count (), (int)m_sType.nBufferCountActual,
																 (int)m_tsBuffer.Count (), (int)voOMXOS_GetSysTime () - (int)m_nStartDebugTime);
	strcat (szMsg,  szTmp);

//	voOS_Printf (szMsg);
}
