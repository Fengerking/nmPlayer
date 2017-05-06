	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXPortInplace.cpp

	Contains:	voCOMXPortInplace class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(_WIN32)
#  include <tchar.h>
#endif

#include <voOMXMemory.h>

#include "voOMXBase.h"
#include "voOMXOSFun.h"

#include "voCOMXCompInplace.h"
#include "voCOMXPortInplace.h"

#include "voLog.h"

voCOMXPortInplace::voCOMXPortInplace(voCOMXBaseComponent * pParent, OMX_S32 nIndex, OMX_DIRTYPE dirType)
	: voCOMXBasePort (pParent, nIndex, dirType)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);

}

voCOMXPortInplace::~voCOMXPortInplace(void)
{

}

OMX_ERRORTYPE voCOMXPortInplace::AllocBuffer (OMX_BUFFERHEADERTYPE** pBuffer, OMX_U32 nPortIndex, 
										   OMX_PTR pAppPrivate, OMX_U32 nSizeBytes)
{
	VOLOGI ("Comp %s, Index %d", m_pParent->GetName (),(int)m_sType.nPortIndex);

	OMX_ERRORTYPE errType = OMX_ErrorNone;

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
		errType = AllocateBufferPointer ();
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("Comp %s, Index %d, AllocateBufferPointer was failed. 0X%08X.",
						m_pParent->GetName (),(int)m_sType.nPortIndex, errType);
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

			voOMXMemSet (m_ppBufferHead[i], 0, sizeof (OMX_BUFFERHEADERTYPE));
			voOMXBase_SetHeader (m_ppBufferHead[i], sizeof (OMX_BUFFERHEADERTYPE));

			OMX_U32					nTryTimes = 0;
			OMX_BUFFERHEADERTYPE ** ppBufferHead =  m_pParent->m_ppPorts[0]->GetBufferHeader ();
			if (m_sType.eDir == OMX_DirInput)
				ppBufferHead =  m_pParent->m_ppPorts[1]->GetBufferHeader ();

			while (nTryTimes < 10)
			{
				if (ppBufferHead != NULL && ppBufferHead[i]->pBuffer != NULL)
					break;

				voOMXOS_Sleep (50);
				ppBufferHead =  m_pParent->m_ppPorts[0]->GetBufferHeader ();
				nTryTimes++;
			}

			if (ppBufferHead == NULL)
				return OMX_ErrorIncorrectStateTransition;

			if (ppBufferHead[i]->pBuffer == NULL)
				return OMX_ErrorIncorrectStateTransition;

			m_ppBufferHead[i]->pBuffer = ppBufferHead[i]->pBuffer;

			m_ppBufferHead[i]->nAllocLen = ppBufferHead[i]->nAllocLen;
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

OMX_ERRORTYPE voCOMXPortInplace::AllocTunnelBuffer (OMX_U32 nPortIndex, OMX_U32 nSizeBytes)
{


	OMX_ERRORTYPE errType = OMX_ErrorNone;

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
		VOLOGE ("Comp %s, Index %d, nSizeBytes < m_sType.nBufferSize.", m_pParent->GetName (), (int)m_sType.nPortIndex);
		return OMX_ErrorIncorrectStateTransition;
	}

	if (m_ppBufferHead == NULL)
	{
		errType = AllocateBufferPointer ();
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
			OMX_U32					nTryTimes = 0;
			OMX_U32					nPortIndex = 0;

			if (m_sType.eDir == OMX_DirInput)
				nPortIndex =  1;
			OMX_BUFFERHEADERTYPE ** ppBufferHead =  m_pParent->m_ppPorts[nPortIndex]->GetBufferHeader ();
			while (nTryTimes < 10)
			{
				if (ppBufferHead != NULL && ppBufferHead[i]->pBuffer != NULL)
					break;

				voOMXOS_Sleep (50);
				ppBufferHead =  m_pParent->m_ppPorts[nPortIndex]->GetBufferHeader ();
				nTryTimes++;
			}

			if (ppBufferHead == NULL)
				return OMX_ErrorIncorrectStateTransition;

			if (ppBufferHead[i]->pBuffer == NULL)
				return OMX_ErrorIncorrectStateTransition;

			OMX_ERRORTYPE errType = OMX_ErrorNone;
			nTryTimes = 0;
			while (nTryTimes < 100)
			{
				errType = m_hTunnelComp->UseBuffer (m_hTunnelComp, &m_ppBufferHead[i], m_nTunnelPort,
													NULL,  ppBufferHead[i]->nAllocLen, ppBufferHead[i]->pBuffer);
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
			}
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Comp %s, Index %d, m_hTunnelComp->UseBuffer was failed. 0X%08X.", m_pParent->GetName (), (int)m_sType.nPortIndex, errType);
				return OMX_ErrorInsufficientResources;
			}

			if (m_ppBufferHead[i] == NULL)
			{
				VOLOGE ("Comp %s, Index %d, m_hTunnelComp->UseBuffer was failed. 0X%08X.", m_pParent->GetName (),(int)m_sType.nPortIndex, errType);
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

