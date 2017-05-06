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
#include "voOMXMemory.h"
#include "voOMXBase.h"
#include "voOMXOSFun.h"
#include "voCOMXCompInplace.h"
#include "voCOMXTIOverlayPort.h"
#include "voLog.h"
#include "voinfo.h"


voCOMXTIOverlayPort::voCOMXTIOverlayPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex, OMX_DIRTYPE dirType)
	: voCOMXBasePort (pParent, nIndex, dirType)
	, m_nallocindex(0)
	, m_bframedropped(false)
	, m_pBufferAllocator(NULL)
{
	strcpy (m_pObjName, __FILE__);
}

voCOMXTIOverlayPort::~voCOMXTIOverlayPort(void)
{
	m_pBufferAllocator = NULL;
}

OMX_ERRORTYPE voCOMXTIOverlayPort::AllocBuffer(OMX_BUFFERHEADERTYPE** pBuffer, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes)
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

	if (m_sType.bPopulated == OMX_TRUE) {

		if (m_nallocindex < m_sType.nBufferCountActual)
			*pBuffer = m_ppBufferHead[m_nallocindex++];

		VOINFO("======================= bufferhead[%d]: %p ==========================", m_nallocindex, m_ppBufferHead[m_nallocindex]);
		if (m_nallocindex == m_sType.nBufferCountActual)
			m_nallocindex %= m_sType.nBufferCountActual;

		return OMX_ErrorNone;
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

	//VOINFO("video w: %d, h: %d, buffer count: %d", m_sType.format.video.nFrameWidth, m_sType.format.video.nFrameHeight, m_sType.nBufferCountActual);
	OMX_U32 i = 0;
	OMX_U32 bufsize = m_pBufferAllocator->GetBufferSize();//nSizeBytes;
	//VOINFO("input size: %d, m_sType.nBufferSize: %d, actual size: %d", nSizeBytes, m_sType.nBufferSize, bufsize);
	if (nSizeBytes > bufsize) {
		VOINFO("error: too small memory from overlay!");
	}

	for (i = 0; i < m_sType.nBufferCountActual; i++) {

		if (m_ppBufferHead[i] == NULL) {

			m_ppBufferHead[i] = (OMX_BUFFERHEADERTYPE *) voOMXMemAlloc(sizeof(OMX_BUFFERHEADERTYPE));
			if (m_ppBufferHead[i] == NULL)
			{
				VOINFO("Comp %s, Index %d, m_ppBufferHead[i] == NULL.", m_pParent->GetName(), m_sType.nPortIndex);
				return OMX_ErrorInsufficientResources;
			}

			voOMXMemSet(m_ppBufferHead[i], 0, sizeof(OMX_BUFFERHEADERTYPE));
			voOMXBase_SetHeader(m_ppBufferHead[i], sizeof(OMX_BUFFERHEADERTYPE));

			m_ppBufferData[i] = (OMX_U8*)(m_pvideobuf[i]);//
			//VOLOGI("m_ppBufferData[%d]: 0x%08x", i, m_ppBufferData[i]);
			VOINFO("bufferhead[%d]: %p, buffer: %p, size: %d, required size: %d", i, m_ppBufferHead[i], m_ppBufferData[i], bufsize, nSizeBytes);
			if (m_ppBufferData[i] == NULL) 
				return OMX_ErrorInsufficientResources;

			m_uBufferCount++;
			m_ppBufferHead[i]->pBuffer = m_ppBufferData[i];
			m_ppBufferHead[i]->nAllocLen = bufsize;//nSizeBytes;
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

OMX_ERRORTYPE voCOMXTIOverlayPort::SetNewCompState (OMX_STATETYPE sNew, OMX_TRANS_STATE sTrans)
{
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

	//voCOMXAutoLock lock (&m_mutlock);

	if (m_bframedropped) {
		m_bframedropped = false;
#if defined __VODBG__
		int i = GetBufferIndex(pBuffer);
		VOINFO("this frame has been dropped, so return it directly [%d]: %p", i, pBuffer->pBuffer);
#endif
		
		return voCOMXBasePort::ReturnBuffer(pBuffer);
	}

	OMX_BUFFERHEADERTYPE * pbuf = NULL;
	OMX_ERRORTYPE	errType = OMX_ErrorNone;
	if (!IsFlush()) {
		int nbuf = m_pBufferAllocator->InquireIdleBuffer();
		if (nbuf >= 0) {   /* normally return buffer */
			pbuf = m_ppBufferHead[nbuf];
			VOINFO("@@@@@@@@ normal return buffer[%d]: %p", nbuf, m_ppBufferHead[nbuf]->pBuffer);
			errType = voCOMXBasePort::ReturnBuffer(pbuf);
		} else if (nbuf == -1) {  /* buffer dropped, so return it directly */
			int skipped = m_pBufferAllocator->getSkippedBuffer();
			if (skipped > -1)
			{
#if defined __VODBG__
				VOINFO("<<<<<<<<<<<<<< have to return skipped buffer[%d]: %p", skipped, m_ppBufferHead[skipped]->pBuffer);
#endif
				voCOMXBasePort::ReturnBuffer(m_ppBufferHead[skipped]);
			}
		} else if (nbuf == -2) {
#if defined __VODBG__
			VOINFO("[SW] <<<<<<<<<<<<<<<<<< %p", pBuffer);
#endif
			errType = voCOMXBasePort::ReturnBuffer(pBuffer);
		} else if (nbuf == -3) {  /* screen mode swiched, overlay buffers been all popped up, so return them one by one */
			int nb = m_pBufferAllocator->getPoppedBuffer();
			if (nb > 0) 
				voCOMXBasePort::ReturnBuffer(m_ppBufferHead[nb]);
		}
	} else {
#if defined __VODBG__
		pbuf = pBuffer;
		int i = GetBufferIndex(pbuf);
		VOINFO("-------------flushing..... return buffer[%d]: %p", i, pbuf->pBuffer);
#endif
		voCOMXBasePort::ReturnBuffer(pBuffer);
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXTIOverlayPort::RequestBuffer(const OMX_U32 type) {

	memset(m_pvideobuf, 0, sizeof(void*) * MAX_V4L2_BUFFER);
	int bufcnt = -1;

	if (m_pBufferAllocator != NULL) 
		bufcnt = m_pBufferAllocator->RequestBuffer(m_pvideobuf, type);

	if (m_sType.nBufferCountActual < 0)
		m_sType.nBufferCountActual = 10;

	return OMX_ErrorNone;
}

OMX_U32 voCOMXTIOverlayPort::GetBufferIndex(OMX_BUFFERHEADERTYPE* pbuf) {

	int i = 0;
	for (; i < m_uBufferCount; i++)
		if (pbuf == m_ppBufferHead[i])
			break;

	return i;
}

OMX_ERRORTYPE	voCOMXTIOverlayPort::Flush(void) {

	voCOMXAutoLock lock (&m_mutlock);
	if (m_pBufferAllocator) 
		m_pBufferAllocator->Flush();

	return voCOMXBasePort::Flush();
}

