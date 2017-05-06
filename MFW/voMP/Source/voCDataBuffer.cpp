	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCDataBuffer.cpp

	Contains:	voCDataBuffer class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file
	2011-09-29  JBF			Disable drop frame if the time is later than 2 secs.

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "voSource.h"

#include "voOSFunc.h"
#include "voCDataBuffer.h"

#define LOG_TAG "voCDataBuffer"
#include "voLog.h"

voCDataBufferData::voCDataBufferData(void)
{
	m_pData = NULL;
	m_nDataSize = 0;
	m_nReadPos = 0;
	m_nWritePos = 0;

	m_pNext = NULL;
}

voCDataBufferData::~voCDataBufferData(void)
{
	if (m_pData != NULL)
	{
		free (m_pData);
		m_pData = NULL;
	}
}

VO_S32 voCDataBufferData::AllocData (VO_S32 nSize)
{
	if (m_pData != NULL)
	{
		free (m_pData);
		m_pData = NULL;
	}
	m_pData = (VO_BYTE *)malloc (nSize);
	if (m_pData == NULL)
		return 0;

	m_nDataSize = nSize;
	m_nReadPos = 0;
	m_nWritePos = 0;

	return m_nDataSize;
}

VO_S32 voCDataBufferData::WriteData (VO_BYTE * pData, VO_S32 nSize)
{
	if (m_pData == NULL || pData == NULL)
		return 0;

	if (m_nDataSize - m_nWritePos < nSize)
		return 0;

	memcpy (m_pData + m_nWritePos, pData, nSize);
	m_nWritePos += nSize;

	return nSize;
}

VO_S32 voCDataBufferData::ReadData (VO_BYTE * pData, VO_S32 nSize)
{
	if (m_pData == NULL || pData == NULL)
		return 0;

	if (m_nWritePos - m_nReadPos < nSize)
		return 0;

	memcpy (pData, m_pData + m_nReadPos, nSize);
	m_nReadPos += nSize;

	return nSize;
}

VO_S32 voCDataBufferData::GetData (VO_BYTE ** ppData, VO_S32 nSize)
{
	if (m_pData == NULL)
		return 0;

	if (m_nWritePos - m_nReadPos < nSize)
		return 0;

	*ppData = m_pData + m_nReadPos;
	m_nReadPos += nSize;

	return nSize;
}

VO_S32 voCDataBufferData::GetAvailableSize (void)
{
	return m_nDataSize - m_nWritePos;
}

VO_S32 voCDataBufferData::GetBufferSize (void)
{
	return m_nWritePos - m_nReadPos;
}

voCDataBufferItem::voCDataBufferItem(void)
{
	m_pBufData = NULL;
	m_nBuffPos = 0;

	m_pNext = NULL;
}

voCDataBufferItem::~voCDataBufferItem(void)
{
}

voCDataBufferList::voCDataBufferList(VO_S32 nItemSize, VO_S32 nVideoBuffer)
	: m_nItemSize (nItemSize)
	, m_nVideoBuffer (nVideoBuffer)
	, m_pCurBuffItem (NULL)
	, m_pFirstItemFree (NULL)
	, m_pFirstItemFull (NULL)
	, m_pLastItemFull (NULL)
	, m_pFirstDataFree (NULL)
	, m_pFirstDataFull (NULL)
	, m_pLastDataFull (NULL)
	, m_bEOS (VO_FALSE)
{
	if (m_nItemSize <= 0)
		m_nItemSize = 40960;
}

voCDataBufferList::~voCDataBufferList()
{
//	voCAutoLock lock (&m_mtxList);
	voCDataBufferData * pData = m_pFirstDataFull;
	voCDataBufferData * pNextData = NULL;
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

	voCDataBufferItem * pItem = m_pFirstItemFull;
	voCDataBufferItem * pNextItem = NULL;
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

VO_S32 voCDataBufferList::AddBuffer (VOMP_BUFFERTYPE * pBuffer)
{
	voCAutoLock lock (&m_mtxList);
	VO_S32 errType = VO_ERR_NONE;

	if(!pBuffer)
		return VO_ERR_INVALID_ARG;
	
	if (pBuffer->nFlag & VOMP_FLAG_BUFFER_FORCE_FLUSH)
	{
		Flush ();
		return 0;
	}

	if( !pBuffer->pBuffer )
		return VO_ERR_NONE;

	if (pBuffer->nFlag & VOMP_FLAG_BUFFER_EOS)
	{
		VOLOGI ("EOS arrived..");	
		m_bEOS = VO_TRUE;
	}

	int nItemCount = 0;
	// Check the Data
	voCDataBufferData * pData = m_pFirstDataFull;

	while (pData != NULL)
	{
		if (pData->m_pNext == NULL)
			break;
		pData = pData->m_pNext;

		// checking erro
		nItemCount++;
		if (nItemCount > 100000)
		{
			VOLOGE ("@@@@@@   11111  nItemCount > 100000");
			voOS_Sleep (1000);
		}
	}

	if (pData != NULL)
	{
		if (pData->GetAvailableSize () < pBuffer->nSize)
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
			pData = new voCDataBufferData ();
			if (pData == NULL)
				return VO_ERR_OUTOF_MEMORY;

			if (pData->AllocData (m_nItemSize * 30) <= 0)
				return VO_ERR_OUTOF_MEMORY;
		}

		if (m_pFirstDataFull == NULL)
		{
			m_pFirstDataFull = pData;
		}

//		m_pLastDataFull = pData;
		else
		{
			voCDataBufferData * pDataTemp = m_pFirstDataFull;
			nItemCount = 0;
			while (pDataTemp != NULL)
			{
				if (pDataTemp->m_pNext == NULL)
				{
					pDataTemp->m_pNext = pData;
					break;
				}

				pDataTemp = pDataTemp->m_pNext;

				// checking erro
				nItemCount++;
				if (nItemCount > 100000)
				{
					VOLOGE ("@@@@@@   22222  nItemCount > 100000");
					voOS_Sleep (1000);
				}
			}
		}
	}
	pData->m_pNext = NULL;

	// Check the item
	voCDataBufferItem * pItem = NULL;

	if (m_pFirstItemFree != NULL)
	{
		pItem = m_pFirstItemFree;
		m_pFirstItemFree = m_pFirstItemFree->m_pNext;
	}
	if (pItem == NULL)
		pItem = new voCDataBufferItem ();
	if (pItem == NULL)
		return VO_ERR_OUTOF_MEMORY;

	memcpy (&pItem->m_bufHead, pBuffer, sizeof (VOMP_BUFFERTYPE));
	pItem->m_bufHead.pBuffer = pData->m_pData + pData->m_nWritePos;

	pItem->m_nBuffPos = pData->m_nWritePos;
	pItem->m_pBufData = pData;
	pData->WriteData ((VO_PBYTE)pBuffer->pBuffer, pBuffer->nSize);

	pItem->m_pNext = NULL;
	if (m_pFirstItemFull == NULL)
	{
		m_pFirstItemFull = pItem;
	}
	else
	{
		voCDataBufferItem * pItemFull = m_pFirstItemFull;

		nItemCount = 0;

		while (pItemFull != NULL)
		{
			if (pItemFull->m_pNext == NULL)
			{
				pItemFull->m_pNext = pItem;
				break;
			}

			pItemFull = pItemFull->m_pNext;

			// checking erro
			nItemCount++;
			if (nItemCount > 100000)
			{
				VOLOGE ("@@@@@@   33333  nItemCount > 100000");
				voOS_Sleep (1000);
			}

		}
	}

//	VOLOGI ("Flag %08X", pItem->m_bufHead.nFlag);

	if (m_pLastItemFull != NULL)
		m_pLastItemFull->m_pNext = pItem;
//	m_pLastItemFull = pItem;

	return errType;
}

VO_S32 voCDataBufferList::GetBuffer (VOMP_BUFFERTYPE * pBuffer, VO_BOOL bCopy)
{
	voCAutoLock lock (&m_mtxList);
	if (m_bEOS && m_pFirstItemFull == NULL)
		return VO_ERR_SOURCE_END;

	if (m_pCurBuffItem != NULL)
	{
		AppendFreeItem (m_pCurBuffItem);
		m_pCurBuffItem = NULL;
	}

	VO_S32	errType = VO_ERR_NONE;
	VO_S32	nFlag = 0;

	// Check the item
	voCDataBufferItem * pItem = NULL;

	if (m_pFirstItemFull == NULL)
		return VO_ERR_SOURCE_NEEDRETRY;

	voCDataBufferItem * pKeyItem = NULL;
	if (m_nVideoBuffer > 0 && pBuffer->llTime > 0 && m_pFirstItemFull->m_bufHead.llTime + 500 < pBuffer->llTime)
		pKeyItem = GetNextKeyFrame (pBuffer->llTime);

	pItem = m_pFirstItemFull;

	if (pKeyItem == NULL)
	{
//		memcpy (pBuffer, &pItem->m_bufHead, sizeof (VOMP_BUFFERTYPE));
		pBuffer->nSize = pItem->m_bufHead.nSize;
		pBuffer->llTime = pItem->m_bufHead.llTime;
		pBuffer->pBuffer = pItem->m_bufHead.pBuffer;
		pBuffer->nFlag = pItem->m_bufHead.nFlag;
		pBuffer->pData  = pItem->m_bufHead.pData;

		nFlag = pItem->m_bufHead.nFlag;
		m_pCurBuffItem = pItem;
	}
	else
	{
		while (pItem != NULL)
		{
			if (pItem == pKeyItem)
			{
				// memcpy (pBuffer, &pItem->m_bufHead, sizeof (VOMP_BUFFERTYPE));
				pBuffer->nSize = pItem->m_bufHead.nSize;
				pBuffer->llTime = pItem->m_bufHead.llTime;
				pBuffer->pBuffer = pItem->m_bufHead.pBuffer;
				pBuffer->nFlag = pItem->m_bufHead.nFlag;
				pBuffer->pData  = pItem->m_bufHead.pData;

				nFlag = pItem->m_bufHead.nFlag;
				m_pCurBuffItem = pItem;
				break;
			}

			voCDataBufferItem * pTemp = pItem->m_pNext;
			//ignore the item, don't need keep in current item.
			errType = AppendFreeItem (pItem);
			pItem = pTemp;
		}
	}

	//if (nFlag & VOMP_FLAG_BUFFER_EOS)
	//	m_bEOS = VO_TRUE;

	if (nFlag & VOMP_FLAG_BUFFER_NEW_PROGRAM)
		return VO_ERR_SOURCE_NEW_PROGRAM;
	else if (nFlag & VOMP_FLAG_BUFFER_NEW_FORMAT)
		return VO_ERR_SOURCE_NEW_FORMAT;
	else if (nFlag & VOMP_FLAG_BUFFER_EOS)
		return VO_ERR_SOURCE_END;
	else if (nFlag & VOMP_FLAG_BUFFER_END)
	{
		if(m_nVideoBuffer > 0)
			return VOMP_ERR_Video_No_Now;
		else
			return VOMP_ERR_Audio_No_Now;
	}

	return errType;
}

voCDataBufferItem * voCDataBufferList::GetNextKeyFrame (VO_S64 llTime)
{
	voCDataBufferItem * pKeyItem = NULL;
	voCDataBufferItem * pItem = m_pFirstItemFull;

	while (pItem != NULL)
	{
		if (pItem->m_bufHead.llTime > llTime)
			break;

		if (pItem->m_bufHead.nFlag & 0xFFFFFFF0)
		{
			break;
		}

		if ((pItem->m_bufHead.nFlag & VOMP_FLAG_BUFFER_KEYFRAME) == VOMP_FLAG_BUFFER_KEYFRAME)
		{
			if (pItem->m_bufHead.llTime <= llTime)
			{
				pKeyItem = pItem;
				//break;
			}
		}

		pItem = pItem->m_pNext;
	}

	return pKeyItem;
}

VO_S32 voCDataBufferList::AppendFreeItem (voCDataBufferItem * pItem)
{
	if(!pItem)
		return VO_ERR_INVALID_ARG;
	
	VO_S32 errType = VO_ERR_NONE;

	CheckBufferData (pItem->m_pBufData, pItem->m_bufHead.nSize);

	m_pFirstItemFull = pItem->m_pNext;
	if (m_pFirstItemFull == NULL)
		m_pLastItemFull = NULL;

	if (m_pFirstItemFree == NULL)
	{
		m_pFirstItemFree = pItem;
	}
	else
	{
		voCDataBufferItem * pItemFree = m_pFirstItemFree;
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
	memset( &pItem->m_bufHead , 0 , sizeof( pItem->m_bufHead ) );

	return errType;
}

VO_S32 voCDataBufferList::CheckBufferData (voCDataBufferData * pData, VO_S32 nReadSize)
{
	VO_S32 errType = VO_ERR_NONE;
	if (pData == NULL)
		return VO_ERR_WRONG_PARAM_ID;

	VO_U8 * pDataBuffer = NULL;
	VO_S32 nRead = 0;
	nRead = pData->GetData (&pDataBuffer, nReadSize);

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
			voCDataBufferData * pDataFree = m_pFirstDataFree;
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

VO_S32 voCDataBufferList::GetBuffTime (void)
{
	voCAutoLock lock (&m_mtxList);

	if (m_pFirstItemFull == NULL)
		return 0;

	VO_S64 nBufferTime = 0;

	VO_S64 nTime = m_pFirstItemFull->m_bufHead.llTime;
	voCDataBufferItem * pItem = m_pFirstItemFull;
	while (pItem != NULL)
	{
		if (pItem->m_pNext == NULL)
		{
			nBufferTime = nBufferTime + (pItem->m_bufHead.llTime - nTime);
			break;
		}

		if (pItem->m_pNext->m_bufHead.llTime < pItem->m_bufHead.llTime)
		{
			nBufferTime = nBufferTime + (pItem->m_bufHead.llTime - nTime);
			nTime = pItem->m_pNext->m_bufHead.llTime;
		}

		pItem = pItem->m_pNext;
	}

	return (VO_S32)nBufferTime;
}

VO_S32 voCDataBufferList::GetBuffCount (void)
{
	voCAutoLock lock (&m_mtxList);

	if (m_pFirstItemFull == NULL)
		return 0;

	VO_S32 nBufferCount = 0;

	voCDataBufferItem * pItem = m_pFirstItemFull;
	while (pItem != NULL)
	{
		if (pItem->m_pNext == NULL)
		{
			break;
		}

		nBufferCount++;

		pItem = pItem->m_pNext;
	}

	return (VO_S32)nBufferCount;
}

VO_S32 voCDataBufferList::Flush (void)
{
	voCAutoLock lock (&m_mtxList);
	VO_S32 errType = VO_ERR_NONE;

	if (m_pCurBuffItem != NULL)
	{
		AppendFreeItem (m_pCurBuffItem);
		m_pCurBuffItem = NULL;
	}

	voCDataBufferData * pData = m_pFirstDataFull;
	voCDataBufferData * pNextData = NULL;

	voCDataBufferData * pFreeData = m_pFirstDataFree;
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

	voCDataBufferItem * pItem = m_pFirstItemFull;
	voCDataBufferItem * pNextItem = NULL;

	voCDataBufferItem * pFreeItem = m_pFirstItemFree;
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

	m_bEOS = VO_FALSE;

	VOLOGI("set m_bEOS false");

	return errType;
}
