	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXDataBuffer.cpp

	Contains:	voCOMXDataBuffer class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "voOMXOSFun.h"
#include "voCOMXDataBuffer.h"

#define LOG_TAG "voCOMXDataBuffer"
#include "voLog.h"

voCOMXDataBufferList::voCOMXDataBufferList(OMX_S32 nItemSize, bool bVideoBuffer)
	: m_nItemSize (nItemSize)
	, m_bVideoBuffer (bVideoBuffer)
	, m_pFirstItemFree (NULL)
	, m_pFirstItemFull (NULL)
	, m_pLastItemFull (NULL)
	, m_pFirstDataFree (NULL)
	, m_pFirstDataFull (NULL)
	, m_pLastDataFull (NULL)
{
	if (m_nItemSize <= 0)
		m_nItemSize = 40960;
}

voCOMXDataBufferList::~voCOMXDataBufferList()
{
	voCOMXAutoLock lock (&m_mtxList);

	voCOMXDataBufferData * pData = m_pFirstDataFull;
	voCOMXDataBufferData * pNextData = NULL;

	while (pData != NULL)
	{
		pNextData = pData->m_pNext;
		delete pData;
		pData = pNextData;
	}

	pData = m_pFirstDataFree;
	while (pData != NULL)
	{
		pNextData = pData->m_pNext;
		delete pData;
		pData = pNextData;
	}

	voCOMXDataBufferItem * pItem = m_pFirstItemFull;
	voCOMXDataBufferItem * pNextItem = NULL;

	while (pItem != NULL)
	{
		pNextItem = pItem->m_pNext;
		delete pItem;
		pItem = pNextItem;
	}

	pItem = m_pFirstItemFree;
	while (pItem != NULL)
	{
		pNextItem = pItem->m_pNext;
		delete pItem;
		pItem = pNextItem;
	}
}

OMX_ERRORTYPE voCOMXDataBufferList::AddBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	voCOMXAutoLock lock (&m_mtxList);
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	int nItemCount = 0;
	// Check the Data
	voCOMXDataBufferData * pData = m_pFirstDataFull;

	while (pData != NULL)
	{
		if (pData->m_pNext == NULL)
			break;
		pData = pData->m_pNext;

		// checking error
		nItemCount++;
		if (nItemCount > 100000)
		{
			VOLOGE ("@@@@@@   11111  nItemCount > 100000");
			voOMXOS_Sleep (1000);
		}
	}

	if (pData != NULL)
	{
		if (pData->GetAvailableSize () < (OMX_S32)pBuffer->nFilledLen)
			pData = NULL;
	}

	if (pData == NULL)
	{
		if (m_pFirstDataFree != NULL)
		{
			pData = m_pFirstDataFree; 
			m_pFirstDataFree = m_pFirstDataFree->m_pNext;
		}
		else
		{
			pData = new voCOMXDataBufferData ();
			if (pData == NULL)
				return OMX_ErrorInsufficientResources;

			if (pData->AllocData (m_nItemSize * 30) <= 0)
				return OMX_ErrorInsufficientResources;
		}

		if (m_pFirstDataFull == NULL)
		{
			m_pFirstDataFull = pData;
		}

//		m_pLastDataFull = pData;
		else
		{
			voCOMXDataBufferData * pDataTemp = m_pFirstDataFull;
			nItemCount = 0;
			while (pDataTemp != NULL)
			{
				if (pDataTemp->m_pNext == NULL)
				{
					pDataTemp->m_pNext = pData;
					break;
				}

				pDataTemp = pDataTemp->m_pNext;

				// checking error
				nItemCount++;
				if (nItemCount > 100000)
				{
					VOLOGE ("@@@@@@   22222  nItemCount > 100000");
					voOMXOS_Sleep (1000);
				}
			}
		}
	}
	pData->m_pNext = NULL;

	// Check the item
	voCOMXDataBufferItem * pItem = NULL;

	if (m_pFirstItemFree != NULL)
	{
		pItem = m_pFirstItemFree;
		m_pFirstItemFree = m_pFirstItemFree->m_pNext;
	}
	if (pItem == NULL)
		pItem = new voCOMXDataBufferItem ();
	if (pItem == NULL)
		return OMX_ErrorInsufficientResources;

	memcpy (&pItem->m_bufHead, pBuffer, sizeof (OMX_BUFFERHEADERTYPE));
	pItem->m_bufHead.pBuffer = pData->m_pData + pData->m_nWritePos;

	pItem->m_nBuffPos = pData->m_nWritePos;
	pItem->m_pBufData = pData;
	pData->WriteData (pBuffer->pBuffer, pBuffer->nFilledLen);

	pItem->m_pNext = NULL;
	if (m_pFirstItemFull == NULL)
	{
		m_pFirstItemFull = pItem;
	}
	else
	{
		voCOMXDataBufferItem * pItemFull = m_pFirstItemFull;

		nItemCount = 0;

		while (pItemFull != NULL)
		{
			if (pItemFull->m_pNext == NULL)
			{
				pItemFull->m_pNext = pItem;
				break;
			}

			pItemFull = pItemFull->m_pNext;

			// checking error
			nItemCount++;
			if (nItemCount > 100000)
			{
				VOLOGE ("@@@@@@   33333  nItemCount > 100000");
				voOMXOS_Sleep (1000);
			}

		}
	}

	if (m_pLastItemFull != NULL)
		m_pLastItemFull->m_pNext = pItem;
//	m_pLastItemFull = pItem;

	return errType;
}

OMX_ERRORTYPE voCOMXDataBufferList::GetBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	voCOMXAutoLock lock (&m_mtxList);

	// Check the item
	if (m_pFirstItemFull == NULL)
		return OMX_ErrorUnderflow;

	voCOMXDataBufferItem * pKeyItem = NULL;
	if (m_bVideoBuffer && pBuffer->nTimeStamp > 0)
		pKeyItem = GetNextKeyFrame (pBuffer->nTimeStamp);

	voCOMXDataBufferItem * pCurrItem = pKeyItem ? pKeyItem : m_pFirstItemFull;
	voCOMXDataBufferItem * pItem = m_pFirstItemFull;
	while (pItem != NULL)
	{
		VOLOGR ("m_bVideoBuffer %d Time %d, Flag 0x%08X, Size %d, Buffer 0x%08X", 
			m_bVideoBuffer, (VO_S32)pItem->m_bufHead.nTimeStamp, pItem->m_bufHead.nFlags, pItem->m_bufHead.nFilledLen, pItem->m_bufHead.pBuffer);

		if((pItem->m_bufHead.nFlags & OMX_BUFFERFLAG_EXTRADATA) && ((pItem->m_bufHead.nFlags & OMX_VO_BUFFERFLAG_NEWSTREAM) == OMX_VO_BUFFERFLAG_NEWSTREAM))
		{
			// PortSettingsChanged
			pBuffer->nFlags = 0;
			pBuffer->nFilledLen = pItem->m_bufHead.nFilledLen;
//			pBuffer->pBuffer = pItem->m_bufHead.pBuffer;
			memcpy(pBuffer->pBuffer + pBuffer->nOffset, pItem->m_bufHead.pBuffer + pItem->m_bufHead.nOffset, pItem->m_bufHead.nFilledLen);

			AppendFreeItem (pItem);
			return OMX_ErrorStreamCorrupt;	// return special value
		}

		if (pItem == pCurrItem)	// reach here
		{
			pBuffer->nFilledLen = pItem->m_bufHead.nFilledLen;
			pBuffer->nFlags = pItem->m_bufHead.nFlags;
			pBuffer->nOffset = pItem->m_bufHead.nOffset;
			pBuffer->nTickCount = pItem->m_bufHead.nTickCount;
			pBuffer->nTimeStamp = pItem->m_bufHead.nTimeStamp;
	//		pBuffer->pBuffer = pItem->m_bufHead.pBuffer;
			// fix smooth streaming green frame issue,Eric, 2011.11.02
			memcpy(pBuffer->pBuffer + pBuffer->nOffset, pItem->m_bufHead.pBuffer + pItem->m_bufHead.nOffset, pItem->m_bufHead.nFilledLen);

			return AppendFreeItem (pItem);
		}

		AppendFreeItem (pItem);
		pItem = pItem->m_pNext;
	}

	return OMX_ErrorUnderflow;
}

voCOMXDataBufferItem * voCOMXDataBufferList::GetNextKeyFrame (OMX_S64 llTime)
{
	voCOMXDataBufferItem * pKeyItem = NULL;
	voCOMXDataBufferItem * pItem = m_pFirstItemFull;

	while (pItem != NULL)
	{
		if (pItem->m_bufHead.nTimeStamp > llTime)
			return NULL;

		if ((pItem->m_bufHead.nFlags & OMX_BUFFERFLAG_SYNCFRAME) == OMX_BUFFERFLAG_SYNCFRAME)
		{
			if (pItem->m_bufHead.nTimeStamp <= llTime)
			{
				pKeyItem = pItem;
				break;
			}
		}

		pItem = pItem->m_pNext;
	}

	return pKeyItem;
}

OMX_ERRORTYPE voCOMXDataBufferList::AppendFreeItem (voCOMXDataBufferItem * pItem)
{
	CheckBufferData (pItem->m_pBufData, pItem->m_bufHead.nFilledLen);

	m_pFirstItemFull = pItem->m_pNext;
	if (m_pFirstItemFull == NULL)
		m_pLastItemFull = NULL;

	if (m_pFirstItemFree == NULL)
	{
		m_pFirstItemFree = pItem;
	}
	else
	{
		voCOMXDataBufferItem * pItemFree = m_pFirstItemFree;
		while (pItemFree != NULL)
		{
			if (pItemFree->m_pNext == NULL)
			{
				pItemFree->m_pNext = pItem;
				break;
			}
			pItemFree = pItemFree->m_pNext;
		}
	}

	pItem->m_pBufData = NULL;
	pItem->m_nBuffPos = 0;
	pItem->m_pNext = NULL;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXDataBufferList::CheckBufferData (voCOMXDataBufferData * pData, OMX_S32 nReadSize)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (pData == NULL)
		return OMX_ErrorBadParameter;

	OMX_U8 * pDataBuffer = NULL;
	//OMX_S32 nRead = 
	pData->GetData (&pDataBuffer, nReadSize);

	if (pData->m_nReadPos == pData->m_nWritePos)
	{
		m_pFirstDataFull = pData->m_pNext;
		if (m_pFirstDataFull == NULL)
			m_pLastDataFull = NULL;

		if (m_pFirstDataFree == NULL)
		{
			m_pFirstDataFree = pData;
		}
		else
		{
			voCOMXDataBufferData * pDataFree = m_pFirstDataFree;
			while (pDataFree != NULL)
			{
				if (pDataFree->m_pNext == NULL)
				{
					pDataFree->m_pNext = pData;
					break;
				}
				pDataFree = pDataFree->m_pNext;
			}
		}

		pData->m_nReadPos = 0;
		pData->m_nWritePos = 0;
		pData->m_pNext = NULL;
	}

	return errType;
}

OMX_S32 voCOMXDataBufferList::GetBuffTime (void)
{
	voCOMXAutoLock lock (&m_mtxList);

	if (m_pFirstItemFull == NULL)
		return 0;

	OMX_S64 nBufferTime = 0;

	OMX_S64 nTime = m_pFirstItemFull->m_bufHead.nTimeStamp;
	voCOMXDataBufferItem * pItem = m_pFirstItemFull;
	while (pItem != NULL)
	{
		if (pItem->m_pNext == NULL)
		{
			nBufferTime = nBufferTime + (pItem->m_bufHead.nTimeStamp - nTime);
			break;
		}

		if (pItem->m_pNext->m_bufHead.nTimeStamp < pItem->m_bufHead.nTimeStamp)
		{
			nBufferTime = nBufferTime + (pItem->m_bufHead.nTimeStamp - nTime);
			nTime = pItem->m_pNext->m_bufHead.nTimeStamp;
		}

		pItem = pItem->m_pNext;
	}

	return (OMX_S32)nBufferTime;
}

OMX_ERRORTYPE voCOMXDataBufferList::Flush (void)
{
	voCOMXAutoLock lock (&m_mtxList);
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	voCOMXDataBufferData * pData = m_pFirstDataFull;
	voCOMXDataBufferData * pNextData = NULL;

	voCOMXDataBufferData * pFreeData = m_pFirstDataFree;
	while (pFreeData != NULL)
	{
		if (pFreeData->m_pNext == NULL)
			break;
		pFreeData = pFreeData->m_pNext;
	}

	while (pData != NULL)
	{
		pNextData = pData->m_pNext;

		if(pFreeData == NULL)
		{
			pFreeData = m_pFirstDataFree = pData;
		}
		else
		{
			pFreeData->m_pNext = pData;
			pFreeData = pData;
		}

		pFreeData->m_nReadPos = 0;
		pFreeData->m_nWritePos = 0;
		pFreeData->m_pNext = NULL;

		pData = pNextData;
	}

	m_pFirstDataFull = NULL;

	voCOMXDataBufferItem * pItem = m_pFirstItemFull;
	voCOMXDataBufferItem * pNextItem = NULL;

	voCOMXDataBufferItem * pFreeItem = m_pFirstItemFree;
	while (pFreeItem != NULL)
	{
		if (pFreeItem->m_pNext == NULL)
			break;
		pFreeItem = pFreeItem->m_pNext;
	}

	while (pItem != NULL)
	{
		pNextItem = pItem->m_pNext;

		if (pFreeItem == NULL)
		{
			m_pFirstItemFree = pItem;
			pFreeItem = m_pFirstItemFree;
		}
		else
		{
			pFreeItem->m_pNext = pItem;
			pFreeItem = pItem;
		}

		pFreeItem->m_nBuffPos = 0;
		pFreeItem->m_pBufData = NULL;
		pFreeItem->m_pNext = NULL;

		pItem = pNextItem;
	}

	m_pFirstItemFull = NULL;
	m_pLastDataFull = NULL;
	m_pLastItemFull = NULL;

	return errType;
}

OMX_ERRORTYPE voCOMXDataBufferList::GetSeekKeyFrame(OMX_TICKS * pnTimeStamp)
{
	voCOMXAutoLock lock (&m_mtxList);

	OMX_TICKS nTimeStamp = *pnTimeStamp;
	voCOMXDataBufferItem * pKeyItem = NULL;
	voCOMXDataBufferItem * pItem = m_pFirstItemFull;
	while (pItem != NULL)
	{
		if (pItem->m_bufHead.nTimeStamp > nTimeStamp)
		{
			if(pKeyItem)
			{
				*pnTimeStamp = pKeyItem->m_bufHead.nTimeStamp;
				return OMX_ErrorNone;
			}
			else if(pItem->m_bufHead.nFlags & OMX_BUFFERFLAG_SYNCFRAME)
			{
				*pnTimeStamp = pItem->m_bufHead.nTimeStamp;
				return OMX_ErrorNone;
			}
			else
				return OMX_ErrorNoMore;
		}

		if(pItem->m_bufHead.nFlags & OMX_BUFFERFLAG_SYNCFRAME)	// key frame
			pKeyItem = pItem;

		pItem = pItem->m_pNext;
	}

	if(pKeyItem)
	{
		*pnTimeStamp = pKeyItem->m_bufHead.nTimeStamp;
		return OMX_ErrorOverflow;
	}
	else
		return OMX_ErrorNotReady;
}
