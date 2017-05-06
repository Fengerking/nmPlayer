	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXTIOverlayPort.cpp

	Contains:	voCOMXTIOverlayPort class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#define LOG_TAG "voCOMXTIOverlayPort"

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
#include "voCOMXTIOverlayPort.h"

#include "voLog.h"
#include "voinfo.h"

voCOMXTIOverlayPort::voCOMXTIOverlayPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex, OMX_DIRTYPE dirType)
	: voCOMXBasePort (pParent, nIndex, dirType)
	, m_pheld(NULL)
	, m_pBufferAllocator(NULL)
	, m_nheld(0)
	, m_nframedropped(0)
{
	strcpy (m_pObjName, __FILE__);
}

voCOMXTIOverlayPort::~voCOMXTIOverlayPort(void)
{
	m_pBufferAllocator = NULL;
}

OMX_ERRORTYPE voCOMXTIOverlayPort::AllocBuffer (OMX_BUFFERHEADERTYPE** pBuffer, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	if (nPortIndex != m_sType.nPortIndex)
	{
		VOLOGE ("Comp %s, Index %d, nPortIndex != m_sType.nPortIndex.", m_pParent->GetName (), m_sType.nPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	if (!IsEnable () && m_sStatus != PORT_TRANS_DIS2ENA)
		return OMX_ErrorNone;

	if (IsTunnel () && IsSupplier ())
	{
		VOLOGE ("Comp %s, Index %d, IsTunnel () && IsSupplier ().", m_pParent->GetName (), m_sType.nPortIndex);
		return OMX_ErrorNotImplemented;
	}

	if (!IsSupplier ())
	{
		if ( m_sType.eDir == OMX_DirInput )
			m_nBufferSupplier = OMX_BufferSupplyInput;
		else if (m_sType.eDir == OMX_DirOutput)
			m_nBufferSupplier = OMX_BufferSupplyOutput ;
	}

	if (nSizeBytes < m_sType.nBufferSize)
	{
		VOLOGE ("Comp %s, Index %d, nSizeBytes < m_sType.nBufferSize.", m_pParent->GetName (), m_sType.nPortIndex);
		return OMX_ErrorIncorrectStateTransition;
	}

	if (m_ppBufferHead == NULL)
	{
		errType = AllocateBufferPointer ();
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("Comp %s, Index %d, AllocateBufferPointer was failed. 0X%08X.",
						m_pParent->GetName (), m_sType.nPortIndex, errType);
			return errType;
		}
	}

	// request buffer 
	void* pvideobuf[MAX_V4L2_BUFFER];
	memset(pvideobuf, 0, sizeof(void*) * MAX_V4L2_BUFFER);
	if (m_pBufferAllocator != NULL) {
		if (!m_pBufferAllocator->RequestBuffer(pvideobuf, m_sType.format.video.nFrameWidth, m_sType.format.video.nFrameHeight))
			return OMX_ErrorNotImplemented;
	}

	for (OMX_U32 i = 0; i < m_sType.nBufferCountActual; i++) {
		if (m_ppBufferHead[i] == NULL) {
			m_ppBufferHead[i] = (OMX_BUFFERHEADERTYPE *) voOMXMemAlloc(sizeof(OMX_BUFFERHEADERTYPE));
			//LOGI("%s->%d: actual buffer: %d, m_ppBufferHead[%d]: 0x%08x", __FILE__, __LINE__, m_sType.nBufferCountActual, i, m_ppBufferHead[i]);
			if (m_ppBufferHead[i] == NULL)
			{
				VOLOGE("Comp %s, Index %d, m_ppBufferHead[i] == NULL.", m_pParent->GetName(), m_sType.nPortIndex);
				return OMX_ErrorInsufficientResources;
			}

			voOMXMemSet(m_ppBufferHead[i], 0, sizeof(OMX_BUFFERHEADERTYPE));
			voOMXBase_SetHeader(m_ppBufferHead[i], sizeof(OMX_BUFFERHEADERTYPE));

			m_ppBufferData[i] = (OMX_U8*)(pvideobuf[i]);//
			if (m_ppBufferData[i] == NULL)
				return OMX_ErrorInsufficientResources;

			m_uBufferCount++;
			m_ppBufferHead[i]->pBuffer = m_ppBufferData[i];
			m_ppBufferHead[i]->nAllocLen = m_pBufferAllocator->GetBufferSize();//nSizeBytes;
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
				m_tsAlloc.Up();
			}

			return OMX_ErrorNone;
		}
	}

	return OMX_ErrorInsufficientResources;
}

OMX_ERRORTYPE voCOMXTIOverlayPort::AllocTunnelBuffer (OMX_U32 nPortIndex, OMX_U32 nSizeBytes)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	if (nPortIndex != m_sType.nPortIndex)
	{
		VOLOGE ("Comp %s, Index %d, nPortIndex != m_sType.nPortIndex.", m_pParent->GetName (), m_sType.nPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	if (!IsEnable () && m_sStatus != PORT_TRANS_DIS2ENA)
		return OMX_ErrorNone;

	if (!IsTunnel () || !IsSupplier ())
	{
		VOLOGE ("Comp %s, Index %d, !IsTunnel () || !IsSupplier ().", m_pParent->GetName (), m_sType.nPortIndex);
		return OMX_ErrorNotImplemented;
	}

	if (nSizeBytes < m_sType.nBufferSize)
	{
		VOLOGE ("Comp %s, Index %d, nSizeBytes < m_sType.nBufferSize.", m_pParent->GetName (), m_sType.nPortIndex);
		return OMX_ErrorIncorrectStateTransition;
	}

	if (m_ppBufferHead == NULL)
	{
		errType = AllocateBufferPointer ();
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("Comp %s, Index %d, AllocateBufferPointer was failed. 0X%08X.", m_pParent->GetName (), m_sType.nPortIndex, errType);
			return errType;
		}
	}

	// request the Overlay
	void* pvideobuf[MAX_V4L2_BUFFER];
	if (m_pBufferAllocator != NULL) {
		if (!m_pBufferAllocator->RequestBuffer(pvideobuf, m_sType.format.video.nFrameWidth, m_sType.format.video.nFrameHeight))
			return OMX_ErrorNotImplemented;
	}

	for (OMX_U32 i = 0; i < m_sType.nBufferCountActual; i++)
	{
		if (m_ppBufferHead[i] == NULL)
		{
			m_ppBufferData[i] = (OMX_U8*)pvideobuf[i];
			if (m_ppBufferData[i] == NULL)
				return OMX_ErrorInsufficientResources;

			OMX_ERRORTYPE errType = OMX_ErrorNone;
			OMX_U32 nTryTimes = 0;
			while(nTryTimes < 100)
			{
				errType = m_hTunnelComp->UseBuffer(m_hTunnelComp, &m_ppBufferHead[i], m_nTunnelPort,
													NULL, nSizeBytes, m_ppBufferData[i]);
				if (errType == OMX_ErrorIncorrectStateTransition)
				{
					voOMXOS_Sleep(10);
					nTryTimes++;
					continue;
				}
				else if (errType != OMX_ErrorNone)
				{
					//voOMXMemFree(m_ppBufferData[i]);
					m_ppBufferData[i]  = NULL;

					VOLOGE("Comp %s, Index %d, m_hTunnelComp->UseBuffer was failed. 0X%08X.", m_pParent->GetName(), m_sType.nPortIndex, errType);
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
				VOLOGE("Comp %s, Index %d, m_hTunnelComp->UseBuffer was failed. 0X%08X.", m_pParent->GetName(), m_sType.nPortIndex, errType);
				return OMX_ErrorInsufficientResources;
			}

			if (m_ppBufferHead[i] == NULL)
			{
				VOLOGE("Comp %s, Index %d, m_hTunnelComp->UseBuffer was failed. 0X%08X.", m_pParent->GetName(), m_sType.nPortIndex, errType);
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

			m_tqBuffer.Add(m_ppBufferHead[i]);
			m_uUsedBuffer++;

			if (m_sType.nBufferCountActual == m_uUsedBuffer)
				m_sType.bPopulated = OMX_TRUE;
		}
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXTIOverlayPort::SetNewCompState (OMX_STATETYPE sNew, OMX_TRANS_STATE sTrans)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	if (sNew == OMX_StateIdle && sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{
		if (IsTunnel () && IsSupplier ())
		{
			errType = AllocTunnelBuffer (m_sType.nPortIndex, m_sType.nBufferSize);
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Comp %s, Index %d, AllocTunnelBuffer was failed. 0X%08X.", m_pParent->GetName (), m_sType.nPortIndex, errType);
				return errType;
			}
		}
		else
		{
			m_tsAlloc.Down ();
		}

		return errType;
	}

	return voCOMXBasePort::SetNewCompState (sNew, sTrans);
}

OMX_ERRORTYPE voCOMXTIOverlayPort::FreeBuffer (OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE* pBuffer) {

	for (OMX_U32 i = 0; i < m_uBufferCount; i++)
	{
		if (m_ppBufferHead[i] == pBuffer)
		{
			if (m_ppBufferData[i] != NULL)
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

	if (m_pBufferAllocator) 
		m_pBufferAllocator->FreeBuffer();

	return OMX_ErrorNone;
}

OMX_ERRORTYPE	voCOMXTIOverlayPort::ReturnBuffer (OMX_BUFFERHEADERTYPE* pBuffer) {

	voCOMXAutoLock lock (&m_mutlock);

	if (m_nframedropped == 1) {
		m_nframedropped = 0;
#if defined __VODBG__
		int i = 0;
		for (; i < m_uBufferCount; i++)
			if (m_ppBufferHead[i] == pBuffer)
				break;
		VOINFO("this frame has been dropped, so return it directly [%d]: %p", i, pBuffer->pBuffer);
#endif
		
		return voCOMXBasePort::ReturnBuffer(pBuffer);
	} else if (m_nframedropped == 2) {
		m_nframedropped = 0;
		m_pheld = pBuffer;
		int pbuf[MAX_V4L2_BUFFER];
		m_nheld = m_pBufferAllocator->getQueuedBuffer(pbuf);
		VOINFO("<<<-------------------- marked buffer: %d", GetBufferIndex(m_pheld));
		for (int i = 0; i < m_nheld; ++i) {
			VOINFO("<<<<<<<<<<<<<<<<<<<<<<<<<<<<< return buffer: %d", pbuf[i]);
			voCOMXBasePort::ReturnBuffer(m_ppBufferHead[pbuf[i]]);
		}

		if (m_pheld != NULL) {
			VOINFO("<<<<<<<<<<<<<<<<<<<<<<<<<<<<< return buffer: %d", GetBufferIndex(m_pheld));
			voCOMXBasePort::ReturnBuffer(m_pheld);
			m_pheld = NULL;
		}
		return OMX_ErrorNone;
	}

	if (!IsFlush()) {

		int nbuf = m_pBufferAllocator->InquireIdleBuffer();
		if (nbuf >= 0) {
			if (m_nheld == 0) {
				voCOMXBasePort::ReturnBuffer(m_ppBufferHead[nbuf]);
			} else {  /* On OMAP3, buffer 0 must be returned to HW DEC after every flushing */
				--m_nheld;
				int skipped = m_pBufferAllocator->getSkippedBuffer();
				if (skipped > -1) {
					VOINFO("<<<<<<<<<<<<<<<<<<<<<<<<<<< return skipped buffer: %d", skipped);
					voCOMXBasePort::ReturnBuffer(m_ppBufferHead[skipped]);
				} else {
					VOINFO("---------------- ignore buffer: %d", nbuf);
				}
			}
		} else if (nbuf == -1) {

			/* have to return the skipped buffer */
			int skipped = m_pBufferAllocator->getSkippedBuffer();
			if (skipped > -1)
				voCOMXBasePort::ReturnBuffer(m_ppBufferHead[skipped]);

		} else if (nbuf == -2) {
			voCOMXBasePort::ReturnBuffer(pBuffer);
		}

	} else {
		VOINFO("<<<<<<<<<<<<<<<<<<<<<<<<<<<<< return buffer: %d", GetBufferIndex(pBuffer));
		voCOMXBasePort::ReturnBuffer(pBuffer);
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE	voCOMXTIOverlayPort::Flush(void) {

	voCOMXAutoLock lock (&m_mutlock);
	if (m_pBufferAllocator) {
		m_pBufferAllocator->Flush();
	}
	return voCOMXBasePort::Flush();
}

OMX_U32 voCOMXTIOverlayPort::GetBufferIndex(OMX_BUFFERHEADERTYPE* pbuf) {

	int i = 0;
	for (; i < m_uBufferCount; i++)
		if (pbuf == m_ppBufferHead[i])
			break;

	return i;
}

