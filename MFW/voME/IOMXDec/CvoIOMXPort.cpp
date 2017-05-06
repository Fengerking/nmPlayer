	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CvoIOMXPort.cpp

	Contains:	Android IOMX component port source file

	Written by:	East Zhou

	Change History (most recent first):
	2012-04-19	East		Create file

*******************************************************************************/
#include "voOMXOSFun.h"
#include "CvoIOMXComponent.h"
#include "CvoIOMXPort.h"

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "CvoIOMXPort"
#endif

#include "voLog.h"

CvoIOMXBufferList::CvoIOMXBufferList()
	: m_ppBuffers(NULL)
	, m_nBuffers(0)
	, m_nCount(0)
{
}

CvoIOMXBufferList::~CvoIOMXBufferList()
{
	if(m_ppBuffers)
		delete [] m_ppBuffers;
}

OMX_BOOL CvoIOMXBufferList::Create(OMX_S32 nBuffers)
{
	if(m_ppBuffers)
		delete [] m_ppBuffers;

	m_nBuffers = 0;
	m_nCount = 0;

	if(nBuffers > 0)
	{
		m_ppBuffers = new voIOMXPortBuffer *[nBuffers];
		if(NULL == m_ppBuffers)
			return OMX_FALSE;

		memset(m_ppBuffers, 0, nBuffers * sizeof(voIOMXPortBuffer *));
	}

	m_nBuffers = nBuffers;

	return OMX_TRUE;
}

OMX_BOOL CvoIOMXBufferList::Add(voIOMXPortBuffer * pBuffer)
{
	for(OMX_S32 i = 0; i < m_nBuffers; i++)
	{
		if(m_ppBuffers[i] == NULL)
		{
			m_ppBuffers[i] = pBuffer;
			m_nCount++;

			return OMX_TRUE;
		}
	}

	return OMX_FALSE;
}

OMX_BOOL CvoIOMXBufferList::Push(voIOMXPortBuffer * pBuffer)
{
	OMX_S32 nLast = -1;
	OMX_S32 i = 0;
	for(i = 0; i < m_nBuffers; i++)
	{
		if(!m_ppBuffers[i])
			break;

		nLast = i;
	}

	if(nLast == m_nBuffers - 1)	// full
		return OMX_FALSE;

	for(i = nLast; i >= 0; i--)
		m_ppBuffers[i + 1] = m_ppBuffers[i];

	m_ppBuffers[0] = pBuffer;
	m_nCount++;
	return OMX_TRUE;
}

OMX_BOOL CvoIOMXBufferList::Remove(voIOMXPortBuffer * pBuffer)
{
	OMX_S32 nDelete = -1;
	OMX_S32 i = 0;
	for(i = 0; i < m_nBuffers; i++)
	{
		if(m_ppBuffers[i] == pBuffer)
		{
			nDelete = i;
			break;
		}
	}

	if(nDelete < 0)
	{
		VOLOGW("the buffer 0x%08X which we want to remove is not in list!!!", pBuffer);
		return OMX_FALSE;
	}

	for(i = nDelete; i < m_nBuffers - 1; i++)
	{
		m_ppBuffers[i] = m_ppBuffers[i + 1];
		if(m_ppBuffers[i + 1] == NULL)
			break;
	}

	if(i == m_nBuffers - 1)
		m_ppBuffers[i] = NULL;

	m_nCount--;
	return OMX_TRUE;
}

voIOMXPortBuffer * CvoIOMXBufferList::GetByBuffer(IOMX::buffer_id hBuffer)
{
	for(OMX_S32 i = 0; i < m_nBuffers && m_ppBuffers[i]; i++)
	{
		if(m_ppBuffers[i]->hBuffer == hBuffer)
			return m_ppBuffers[i];
	}

	return NULL;
}

voIOMXPortBuffer * CvoIOMXBufferList::GetByData(void * pData)
{
	for(OMX_S32 i = 0; i < m_nBuffers && m_ppBuffers[i]; i++)
	{
		if(m_ppBuffers[i]->pData == pData)
			return m_ppBuffers[i];
	}

	return NULL;
}

void CvoIOMXBufferList::RemoveAll()
{
	for(OMX_S32 i = 0; i < m_nBuffers; i++)
		m_ppBuffers[i] = NULL;
	m_nCount = 0;
}

CvoIOMXPort::CvoIOMXPort(CvoIOMXComponent * pComponent, OMX_U32 nPortIndex)
	: m_pComponent(pComponent)
	, m_nIndex(nPortIndex)
	, m_eTransStatus(PORT_TRANS_None)
	, m_bFlushing(OMX_FALSE)
	, m_bChanging2IdleState(OMX_FALSE)
	, m_nBufferCount(0)
	, m_pBuffers(NULL)
	, m_bGraphicBufferAvaible(OMX_TRUE)
{
	m_pComponent->SetHeader(&m_sPortDefinition, sizeof(m_sPortDefinition));
	m_pComponent->SetHeader(&m_sPortFormat, sizeof(m_sPortFormat));
}

CvoIOMXPort::~CvoIOMXPort()
{
	if(m_pBuffers)
	{
		delete [] m_pBuffers;
		m_pBuffers = NULL;
	}
}

OMX_ERRORTYPE CvoIOMXPort::FreeBufferHandle()
{
	if(NULL == m_pBuffers)
	{
		VOLOGE("NULL == m_pBuffers");
		return OMX_ErrorNotReady;
	}

	VO_U32 nFreed = 0;
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	while(OMX_TRUE == m_bGraphicBufferAvaible && nFreed < m_nBufferCount)
	{
		voCOMXAutoLock lock(&m_tmBuffer);
		voIOMXPortBuffer * pBuffer = m_listBufferEmpty.GetHead();
		if(pBuffer)
		{
#ifdef _ICS
			if(OMX_DirOutput == m_sPortDefinition.eDir && pBuffer->hBuffer) // maybe we should free buffer first and then cancel buffer
				m_pComponent->NativeWindow_CancelBuffer(pBuffer);
#endif	// _ICS
			VOLOGR("FreeBuffer 0x%08X", pBuffer->hBuffer);
			errType = m_pComponent->FreeBuffer(m_nIndex, pBuffer->hBuffer);
			if(OMX_ErrorNone != errType)
			{
				VOLOGE("failed to free buffer 0x%08X", errType);
				return errType;
			}

			m_listBufferEmpty.Remove(pBuffer);
			nFreed++;
		}

		VOLOGI("nFreed %d, m_nBufferCount %d", nFreed, m_nBufferCount);
	}

	delete [] m_pBuffers;
	m_pBuffers = NULL;

	return errType;
}

OMX_ERRORTYPE CvoIOMXPort::StartBuffer()
{
	return OMX_ErrorNone;
}

OMX_ERRORTYPE CvoIOMXPort::UpdatePortDefinition()
{
	voCOMXAutoLock lock(&m_tmPortDefinition);

	m_sPortDefinition.nPortIndex = m_nIndex;
	OMX_ERRORTYPE errType = m_pComponent->GetParameter(OMX_IndexParamPortDefinition, &m_sPortDefinition, sizeof(m_sPortDefinition));
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to get OMX_IndexParamPortDefinition 0x%08X", errType);
		return errType;
	}

	VOLOGR("updated port definition index %d, enable %d, buffer count %d, buffer size %d, width %d, height %d, stride %d, slice height %d", 
		m_nIndex, m_sPortDefinition.bEnabled, m_sPortDefinition.nBufferCountActual, m_sPortDefinition.nBufferSize, 
		m_sPortDefinition.format.video.nFrameWidth, m_sPortDefinition.format.video.nFrameHeight, m_sPortDefinition.format.video.nStride, m_sPortDefinition.format.video.nSliceHeight);

	return errType;
}

OMX_ERRORTYPE CvoIOMXPort::UpdatePortFormat()
{
	voCOMXAutoLock lock(&m_tmPortDefinition);

	m_sPortFormat.nPortIndex = m_nIndex;
	OMX_ERRORTYPE errType = m_pComponent->GetParameter(OMX_IndexParamVideoPortFormat, &m_sPortFormat, sizeof(m_sPortFormat));
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to get OMX_IndexParamVideoPortFormat 0x%08X", errType);
		return errType;
	}

	VOLOGR("updated port format index %d, color %d, compression format %d", m_nIndex, m_sPortFormat.eColorFormat, m_sPortFormat.eCompressionFormat);
	return errType;
}

OMX_BOOL CvoIOMXPort::IsEnable()
{
	voCOMXAutoLock lock(&m_tmPortDefinition);

	return m_sPortDefinition.bEnabled;
}

OMX_COLOR_FORMATTYPE CvoIOMXPort::GetColorFormat()
{
	voCOMXAutoLock lock(&m_tmPortDefinition);

	return m_sPortFormat.eColorFormat;
}

void CvoIOMXPort::GetResolution(OMX_U32 * pnWidth, OMX_U32 * pnHeight)
{
	voCOMXAutoLock lock(&m_tmPortDefinition);

	if(pnWidth)
		*pnWidth = m_sPortDefinition.format.video.nFrameWidth;
	if(pnHeight)
		*pnHeight = m_sPortDefinition.format.video.nFrameHeight;
}

PORT_TRANS_STATUS CvoIOMXPort::GetTransStatus()
{
	return m_eTransStatus;
}

OMX_ERRORTYPE CvoIOMXPort::Enable()
{
	m_eTransStatus = PORT_TRANS_Dis2Ena;

	OMX_ERRORTYPE errType = m_pComponent->SendCommand(OMX_CommandPortEnable, m_nIndex);
	if(OMX_ErrorNone != errType)
	{
		VOLOGE ("failed to set OMX_CommandPortEnable %d 0x%08X", m_nIndex, errType);
		return errType;
	}

	errType = AllocateBufferHandle();
	if(OMX_ErrorNone != errType)
	{
		VOLOGE ("failed to AllocateBufferHandle %d 0x%08X", m_nIndex, errType);
		return errType;
	}

	VO_U32 nTryTimes = 0;
	while(!IsEnable())
	{
		voOMXOS_Sleep(2);
		if(++nTryTimes > 2500)
		{
			VOLOGE("OMX_ErrorTimeout");
			return OMX_ErrorTimeout;
		}
	}

	return errType;
}

OMX_ERRORTYPE CvoIOMXPort::Disable()
{
	m_eTransStatus = PORT_TRANS_Ena2Dis;

	OMX_ERRORTYPE errType = m_pComponent->SendCommand(OMX_CommandPortDisable, m_nIndex);
	if(OMX_ErrorNone != errType)
	{
		VOLOGE ("failed to set OMX_CommandPortDisable %d 0x%08X", m_nIndex, errType);
		return errType;
	}

	errType = FreeBufferHandle();
	if(OMX_ErrorNone != errType)
	{
		VOLOGE ("failed to FreeBufferHandle %d 0x%08X", m_nIndex, errType);
		return errType;
	}

	VO_U32 nTryTimes = 0;
	while(IsEnable())
	{
		voOMXOS_Sleep(2);
		if(++nTryTimes > 2500)
		{
			VOLOGE("OMX_ErrorTimeout");
			return OMX_ErrorTimeout;
		}
	}

	return errType;
}

OMX_ERRORTYPE CvoIOMXPort::Reconfigure()
{
	OMX_ERRORTYPE errType = Disable();
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to disable 0x%08X", errType);
		return errType;
	}

	errType = Enable();
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to enable 0x%08X", errType);
		return errType;
	}

	// before StartBuffer to avoid lost buffer from FillBufferDone, East 20130101
	m_pComponent->m_bPortSettingsChanging = OMX_FALSE;

	errType = StartBuffer();

	return OMX_ErrorNone;
}

void CvoIOMXPort::SetFlushing(OMX_BOOL bFlushing)
{
	if(!bFlushing)
	{
		// wait for all buffer return
		VO_U32	nTryTimes = 0;
		while(nTryTimes < 500)
		{
			m_tmBuffer.Lock();
			OMX_U32 nEmptyBuffers = m_listBufferEmpty.Count();
			m_tmBuffer.Unlock();

			if(m_nBufferCount == nEmptyBuffers)
				break;

			voOMXOS_Sleep(2);
			nTryTimes++;
		}
		if(nTryTimes >= 500)
		{
			VOLOGE("port %d timeout when flush", m_nIndex);
		}
	}

	voCOMXAutoLock lock(&m_tmFlush);

	m_bFlushing = bFlushing;
}

OMX_BOOL CvoIOMXPort::IsFlushing()
{
	voCOMXAutoLock lock(&m_tmFlush);

	return m_bFlushing;
}

void CvoIOMXPort::SetChanging2IdleState(OMX_BOOL bChanging2IdleState)
{
	voCOMXAutoLock lock(&m_tmFlush);

	m_bChanging2IdleState = bChanging2IdleState;
}

OMX_BOOL CvoIOMXPort::CanBuffersWork()
{
	if(PORT_TRANS_Ena2Dis == m_eTransStatus)
	{
		VOLOGW("index %d  PORT_TRANS_Ena2Dis", m_nIndex);
		return OMX_FALSE;
	}

	// make sure when flush, no FillThisBuffer and EmptyThisBuffer
	// fix QCM HoneyComb seek hang issue, East 20110903
	voCOMXAutoLock lock(&m_tmFlush);
	if(m_bFlushing)
	{
		VOLOGW("index %d is flushing", m_nIndex);
		return OMX_FALSE;
	}

	if(m_bChanging2IdleState)
	{
		VOLOGW("index %d is changing to idle state", m_nIndex);
		return OMX_FALSE;
	}

	return OMX_TRUE;
}

OMX_BOOL CvoIOMXPort::IsLegalBuffer(IOMX::buffer_id hBuffer)
{
	voCOMXAutoLock lock(&m_tmBuffer);

	return (NULL != m_listBufferEmpty.GetByBuffer(hBuffer)) ? OMX_TRUE : OMX_FALSE;
}

OMX_BOOL CvoIOMXPort::GetBufferByData(void * pData, voIOMXPortBuffer ** ppBuffer)
{
	voCOMXAutoLock lock(&m_tmBuffer);

	voIOMXPortBuffer * pBuffer = m_listBufferEmpty.GetByData(pData);
	if(!pBuffer)
		return OMX_FALSE;

	if(ppBuffer)
		*ppBuffer = pBuffer;

	return OMX_TRUE;
}
