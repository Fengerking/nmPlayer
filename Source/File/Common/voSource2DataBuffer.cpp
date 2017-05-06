#ifndef _NEW_SOURCEBUFFER

#include <stdio.h>
#include <string.h>
#include "voOSFunc.h"
#include "voSource2DataBuffer.h"
#include "voLog.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

voSource2DataBufferData::voSource2DataBufferData(void)
{
	m_pData = NULL;
	m_nDataSize = 0;
	m_nReadPos = 0;
	m_nWritePos = 0;

	m_pNext = NULL;
}

voSource2DataBufferData::~voSource2DataBufferData(void)
{
	if (m_pData != NULL)
		free (m_pData);
}

VO_S32 voSource2DataBufferData::AllocData (VO_S32 nSize)
{
	if (m_pData != NULL)
		free (m_pData);
	m_pData = (VO_BYTE *)malloc (nSize);
	if (m_pData == NULL)
		return 0;

	m_nDataSize = nSize;
	m_nReadPos = 0;
	m_nWritePos = 0;

	return m_nDataSize;
}

VO_S32 voSource2DataBufferData::WriteData (VO_BYTE * pData, VO_S32 nSize)
{
	if (m_pData == NULL)
		return 0;

	if (m_nDataSize - m_nWritePos < nSize)
		return 0;

	memcpy (m_pData + m_nWritePos, pData, nSize);
	m_nWritePos += nSize;

	return nSize;
}

VO_S32 voSource2DataBufferData::ReadData (VO_BYTE * pData, VO_S32 nSize)
{
	if (m_pData == NULL)
		return 0;

	if (m_nWritePos - m_nReadPos < nSize)
		return 0;

	memcpy (pData, m_pData + m_nReadPos, nSize);
	m_nReadPos += nSize;

	return nSize;
}

VO_S32 voSource2DataBufferData::GetData (VO_BYTE ** ppData, VO_S32 nSize)
{
	if (m_pData == NULL)
		return 0;

	if (m_nWritePos - m_nReadPos < nSize)
		return 0;

	*ppData = m_pData + m_nReadPos;
	m_nReadPos += nSize;

	return nSize;
}

VO_S32 voSource2DataBufferData::GetAvailableSize (void)
{
	return m_nDataSize - m_nWritePos;
}

VO_S32 voSource2DataBufferData::GetBufferSize (void)
{
	return m_nWritePos - m_nReadPos;
}

voSource2DataBufferItem::voSource2DataBufferItem(void)
{
	m_pBufData = NULL;
	m_nBuffPos = 0;

	m_pNext = NULL;
}

voSource2DataBufferItem::~voSource2DataBufferItem(void)
{
}

voSource2DataBufferItemList::voSource2DataBufferItemList(VO_S32 nMaxSampleSize, VO_S32 nMaxBufferTime)
	: m_nMaxBufferTime (nMaxBufferTime)
	, m_pCurBuffItem (NULL)
	, m_pFirstItemFree (NULL)
	, m_pFirstItemFull (NULL)
//	, m_pLastItemFull (NULL)
	, m_pLockedItem(NULL)
	, m_pFirstDataFree (NULL)
	, m_pFirstDataFull (NULL)
//	, m_pLastDataFull (NULL)
	, m_pLockedData(NULL)
	, m_bEOS (VO_FALSE)
	, m_bLocked(VO_FALSE)
{
	if (nMaxSampleSize <= 0)
		m_nItemSize = 4*1024*1024;
	else
		m_nItemSize = nMaxSampleSize*2;

	if( m_nMaxBufferTime <= 0 )
		m_nMaxBufferTime = 10 * 1000;
}

voSource2DataBufferItemList::~voSource2DataBufferItemList()
{
//	voCAutoLock lock (&m_mtxList);
	voSource2DataBufferData * pData = m_pFirstDataFull;
	voSource2DataBufferData * pNextData = NULL;
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

	pData = m_pLockedData;
	while (pData != NULL)
	{
		pNextData = pData->m_pNext;
		delete pData;
		pData = pNextData;
	}

	voSource2DataBufferItem * pItem = m_pFirstItemFull;
	voSource2DataBufferItem * pNextItem = NULL;
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

	pItem = m_pLockedItem;
	while (pItem != NULL)
	{
		pNextItem = pItem->m_pNext;
		delete pItem;
		pItem = pNextItem;
	}
}

VO_S32 voSource2DataBufferItemList::AddBuffer (VO_SOURCE2_SAMPLE * pBuffer)
{
	voCAutoLock lock (&m_mtxList);
	VO_S32 errType = VO_ERR_NONE;

	if ( pBuffer->Flag & VO_SOURCE2_FLAG_SAMPLE_FORCE_FLUSH )
	{
		Flush ();
		return 0;
	}

	if(GetBuffCount()> 2500)
		return VO_ERR_RETRY;
// 	if( GetBuffTime() > m_nMaxBufferTime )
// 		return VO_ERR_RETRY;

	//if( !pBuffer->pBuffer )
		//return VO_ERR_NONE;

	if ( pBuffer->Flag & VO_SOURCE2_FLAG_SAMPLE_EOS )
	{
		VOLOGI ("EOS arrived..");	
		m_bEOS = VO_TRUE;
	}
	/*if( pBuffer->Flag & VOLIVESRC_FLAG_BUFFER_LOCKED )
	{
		m_bLocked = VO_TRUE;
		return AddBufferLocked(pBuffer);
	}
	if( m_bLocked && !( pBuffer->Flag & VOLIVESRC_FLAG_BUFFER_LOCKED ) )
	{
		
		VO_U32 ret = MergeLockedBuffer(pBuffer);
		if (ret == VO_ERR_FINISH)
		{
			m_bLocked = VO_FALSE;
		}
		return ret;
		
	}*/
	// Check the Data
	voSource2DataBufferData * pData = m_pFirstDataFull;

	while (pData != NULL)
	{
		if (pData->m_pNext == NULL)
			break;
		pData = pData->m_pNext;
	}

	if (pData != NULL)
	{
		if (pData->GetAvailableSize () < pBuffer->Size)
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
			pData = new voSource2DataBufferData ();
			if (pData == NULL)
				return VO_ERR_OUTOF_MEMORY;

			if (pData->AllocData (m_nItemSize) <= 0)
				return VO_ERR_OUTOF_MEMORY;
		}

		if (m_pFirstDataFull == NULL)
		{
			m_pFirstDataFull = pData;
		}
		else
		{
			voSource2DataBufferData * pDataTemp = m_pFirstDataFull;
			while (pDataTemp != NULL)
			{
				if (pDataTemp->m_pNext == NULL)
				{
					pDataTemp->m_pNext = pData;
					break;
				}
				pDataTemp = pDataTemp->m_pNext;
			}
		}
	}
	pData->m_pNext = NULL;

	// Check the item
	voSource2DataBufferItem * pItem = NULL;

	if (m_pFirstItemFree != NULL)
	{
		pItem = m_pFirstItemFree;
		m_pFirstItemFree = m_pFirstItemFree->m_pNext;
	}
	if (pItem == NULL)
		pItem = new voSource2DataBufferItem ();
	if (pItem == NULL)
		return VO_ERR_OUTOF_MEMORY;

	memcpy (&pItem->m_bufHead, pBuffer, sizeof (VO_SOURCE2_SAMPLE));

	pItem->m_nBuffPos = pData->m_nWritePos;

	if( pBuffer->pBuffer )
	{
		pItem->m_bufHead.pBuffer = pData->m_pData + pData->m_nWritePos;
		pItem->m_pBufData = pData;
		pData->WriteData ((VO_PBYTE)pBuffer->pBuffer, pBuffer->Size);
	}
	else
	{
		pItem->m_pBufData = pData;
		pItem->m_bufHead.pBuffer = 0;
	}

	pItem->m_pNext = NULL;
	if (m_pFirstItemFull == NULL)
	{
		m_pFirstItemFull = pItem;
	}
	else
	{
		voSource2DataBufferItem * pItemFull = m_pFirstItemFull;
		while (pItemFull != NULL)
		{
			if (pItemFull->m_pNext == NULL)
			{
				pItemFull->m_pNext = pItem;
				break;
			}
			pItemFull = pItemFull->m_pNext;
		}
	}
	return errType;
}

VO_S32 voSource2DataBufferItemList::GetBuffer (VO_SOURCE2_SAMPLE * pBuffer)
{
	voCAutoLock lock (&m_mtxList);
	if (m_bEOS && m_pFirstItemFull == NULL)
		return VO_RET_SOURCE2_END;

	if (m_pCurBuffItem != NULL)
	{
		AppendFreeItem (m_pCurBuffItem);
		m_pCurBuffItem = NULL;
	}

	VO_S32	errType = VO_ERR_NONE;
	VO_S32	nFlag = 0;

	if (m_pFirstItemFull == NULL)
		return VO_RET_SOURCE2_NEEDRETRY;

	// Check the item
	voSource2DataBufferItem * pItem = NULL;
	voSource2DataBufferItem * pKeyItem = NULL;
	if ( pBuffer->Time > 0 && m_pFirstItemFull->m_bufHead.Time + 500 < pBuffer->Time)
		pKeyItem = GetNextKeyFrame (pBuffer->Time);

	pItem = m_pFirstItemFull;

	if (pKeyItem == NULL)
	{
		pBuffer->Size = pItem->m_bufHead.Size;
		pBuffer->Time = pItem->m_bufHead.Time;
		pBuffer->pBuffer = pItem->m_bufHead.pBuffer;
		pBuffer->Flag = pItem->m_bufHead.Flag;
		m_pCurBuffItem = pItem;
	}
	else
	{
		while (pItem != NULL)
		{
			if (pItem == pKeyItem)
			{
				pBuffer->Size = pItem->m_bufHead.Size;
				pBuffer->Time = pItem->m_bufHead.Time;
				pBuffer->pBuffer = pItem->m_bufHead.pBuffer;
				pBuffer->Flag = pItem->m_bufHead.Flag;
				m_pCurBuffItem = pItem;
				break;
			}

			voSource2DataBufferItem * pTemp = pItem->m_pNext;
			//ignore the item, don't need keep in current item.
			errType = AppendFreeItem (pItem);
			pItem = pTemp;
		}
	}

	return errType;
}

VO_S32 voSource2DataBufferItemList::AppendFreeItem (voSource2DataBufferItem * pItem)
{
	VO_S32 errType = VO_ERR_NONE;

	CheckBufferData (pItem->m_pBufData, pItem->m_bufHead.Size);

	m_pFirstItemFull = pItem->m_pNext;

	if (m_pFirstItemFree == NULL)
	{
		m_pFirstItemFree = pItem;
	}
	else
	{
		voSource2DataBufferItem * pItemFree = m_pFirstItemFree;
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

VO_S32 voSource2DataBufferItemList::CheckBufferData (voSource2DataBufferData * pData, VO_S32 nReadSize)
{
	VO_S32 errType = VO_ERR_NONE;
	if (pData == NULL)
		return VO_ERR_WRONG_PARAM_ID;

	VO_U8 * pDataBuffer = NULL;
	VO_S32 nRead = pData->GetData (&pDataBuffer, nReadSize);

	if (pData->m_nReadPos == pData->m_nWritePos)
	{
		m_pFirstDataFull = pData->m_pNext;
		if (m_pFirstDataFree == NULL)
		{
			m_pFirstDataFree = pData;
		}
		else
		{
			voSource2DataBufferData * pDataFree = m_pFirstDataFree;
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

VO_S32 voSource2DataBufferItemList::GetBuffTime (void)
{
	voCAutoLock lock (&m_mtxList);

	if (m_pFirstItemFull == NULL)
		return 0;

	VO_S64 nBufferTime = 0;
    VO_S64 nBufferFirst = 0;
    VO_S64 nBufferLast = 0;    
    
	voSource2DataBufferItem * pItemFirst = m_pFirstItemFull;
    voSource2DataBufferItem * pItemLast = m_pFirstItemFull;

    while(pItemFirst != NULL)
    {
        if((pItemFirst->m_bufHead.Flag == VO_SOURCE2_FLAG_SAMPLE_KEYFRAME) ||
           (pItemFirst->m_bufHead.Flag == 0))
        {
            nBufferFirst = m_pFirstItemFull->m_bufHead.Time;
            break;
        }
        pItemFirst = pItemFirst->m_pNext;
    }

    while(pItemLast != NULL)
    {
        if((pItemLast->m_bufHead.Flag == VO_SOURCE2_FLAG_SAMPLE_KEYFRAME) ||
           (pItemLast->m_bufHead.Flag == 0))
        {
            nBufferLast = pItemLast->m_bufHead.Time;
        }

        pItemLast = pItemLast->m_pNext;
    }

    nBufferTime = (nBufferFirst<nBufferLast)?(nBufferLast-nBufferFirst):(nBufferFirst-nBufferLast);

	return (VO_S32)nBufferTime;
}

VO_S32 voSource2DataBufferItemList::GetBuffCount (void)
{
	voCAutoLock lock (&m_mtxList);

	if (m_pFirstItemFull == NULL)
		return 0;

	VO_S32 nBufferCount = 0;

	voSource2DataBufferItem * pItem = m_pFirstItemFull;
	while (pItem != NULL)
	{
		nBufferCount++;
		if (pItem->m_pNext == NULL)
		{
			break;
		}
		pItem = pItem->m_pNext;
	}
	return (VO_S32)nBufferCount;
}

VO_S32 voSource2DataBufferItemList::Flush (void)
{
	voCAutoLock lock (&m_mtxList);
	VO_S32 errType = VO_ERR_NONE;

	if (m_pCurBuffItem != NULL)
	{
		AppendFreeItem (m_pCurBuffItem);
		m_pCurBuffItem = NULL;
	}

	voSource2DataBufferData * pData = m_pLockedData;
	voSource2DataBufferData * pNextData = NULL;
	while (pData != NULL)
	{
		pNextData = pData->m_pNext;
		delete pData;
		pData = pNextData;
	}
	m_pLockedData = NULL;

	pData = m_pFirstDataFull;
	pNextData = NULL;

	voSource2DataBufferData * pFreeData = m_pFirstDataFree;
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

	voSource2DataBufferItem * pItem = m_pFirstItemFull;
	voSource2DataBufferItem * pNextItem = NULL;

	voSource2DataBufferItem * pFreeItem = m_pFirstItemFree;
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
	m_bEOS = VO_FALSE;
	return errType;
}

VO_S32 voSource2DataBufferItemList::AddBufferLocked (VO_SOURCE2_SAMPLE * pBuffer)
{
	VO_S32 errType = VO_ERR_NONE;

	if( !pBuffer->pBuffer )
		return VO_ERR_NONE;

	voSource2DataBufferData * pData = m_pLockedData;

	while (pData != NULL)
	{
		if (pData->m_pNext == NULL)
			break;
		pData = pData->m_pNext;
	}

	if (pData != NULL)
	{
		if (pData->GetAvailableSize () < pBuffer->Size)
			pData = NULL;
	}

	if (pData == NULL)
	{
		pData = new voSource2DataBufferData ();
		if (pData == NULL)
			return VO_ERR_OUTOF_MEMORY;

		if (pData->AllocData (m_nItemSize) <= 0)
			return VO_ERR_OUTOF_MEMORY;

		if (m_pLockedData == NULL)
		{
			m_pLockedData = pData;
		}
		else
		{
			voSource2DataBufferData * pDataTemp = m_pLockedData;
			while (pDataTemp != NULL)
			{
				if (pDataTemp->m_pNext == NULL)
				{
					pDataTemp->m_pNext = pData;
					break;
				}

				pDataTemp = pDataTemp->m_pNext;
			}
		}
	}
	pData->m_pNext = NULL;

	// Check the item
	voSource2DataBufferItem * pItem = new voSource2DataBufferItem ();
	if (pItem == NULL)
		return VO_ERR_OUTOF_MEMORY;

	memcpy (&pItem->m_bufHead, pBuffer, sizeof (VO_SOURCE2_SAMPLE));
	pItem->m_bufHead.pBuffer = pData->m_pData + pData->m_nWritePos;

	pItem->m_nBuffPos = pData->m_nWritePos;
	pItem->m_pBufData = pData;
	pData->WriteData ((VO_PBYTE)pBuffer->pBuffer, pBuffer->Size);

	pItem->m_pNext = NULL;
	if (m_pLockedItem == NULL)
	{
		m_pLockedItem = pItem;
	}
	else
	{
		voSource2DataBufferItem * pItemFull = m_pLockedItem;

		while (pItemFull != NULL)
		{
			if (pItemFull->m_pNext == NULL)
			{
				pItemFull->m_pNext = pItem;
				break;
			}
			pItemFull = pItemFull->m_pNext;
		}
	}
	return errType;
}

VO_S32 voSource2DataBufferItemList::MergeLockedBuffer(VO_SOURCE2_SAMPLE * pBuffer)
{
	if (!m_pLockedItem)
	{
		return VO_ERR_NONE;
	}

	voSource2DataBufferItem * pItem = m_pLockedItem;
	if (!(pBuffer->Size & 0x80000000))
	{
		return VO_ERR_NONE;
	}
	
	voSource2DataBufferData * pData = m_pFirstDataFull;

	while (pData != NULL)
	{
		if (pData->m_pNext == NULL)
		{
			pData->m_pNext =  m_pLockedData; 
			break;
		}
		pData = pData->m_pNext;
	}
	if (!pData)
	{
		pData =  m_pLockedData; 
	}
	voSource2DataBufferItem * pItemFull = m_pFirstItemFull;
	while (pItemFull != NULL)
	{
		if (pItemFull->m_pNext == NULL)
		{
			pItemFull->m_pNext = m_pLockedItem;
			break;
		}

		pItemFull = pItemFull->m_pNext;
	}
	if (!pItemFull)
	{
		pItemFull = m_pLockedItem;
	}
	return VO_ERR_NONE;
}

voSource2DataBufferItem *	voSource2DataBufferItemList::GetNextKeyFrame (VO_S64 llTime)
{
	voSource2DataBufferItem * pKeyItem = NULL;
	voSource2DataBufferItem * pItem = m_pFirstItemFull;

	while (pItem != NULL)
	{
		if (pItem->m_bufHead.Time > llTime)
			break;

		if((pItem->m_bufHead.Flag & VO_SOURCE2_FLAG_SAMPLE_KEYFRAME) == VO_SOURCE2_FLAG_SAMPLE_KEYFRAME)
		{
			if (pItem->m_bufHead.Time <= llTime)
			{
				pKeyItem = pItem;
				//break;
			}
		}

		if((pItem->m_bufHead.Flag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT) == VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT)
		{
			if (pItem->m_bufHead.Time <= llTime)
			{
				pKeyItem = pItem;
				break;
			}
		}

		pItem = pItem->m_pNext;
	}

	return pKeyItem;
}

#endif