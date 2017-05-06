#ifndef __voLiveSrcDataBuffer_H__
#define __voLiveSrcDataBuffer_H__

#include <stdlib.h>
#include "voCMutex.h"
#include "voSource2.h"

class voSource2DataBufferData
{
public:
	voSource2DataBufferData(void);
	virtual ~voSource2DataBufferData(void);

	VO_S32	AllocData (VO_S32 nSize);
	VO_S32	WriteData (VO_BYTE * pData, VO_S32 nSize);
	VO_S32	ReadData (VO_BYTE * pData, VO_S32 nSize);
	VO_S32	GetData (VO_BYTE ** ppData, VO_S32 nSize);
	VO_S32	GetAvailableSize (void);
	VO_S32	GetBufferSize (void);

public:
	VO_BYTE *				m_pData;
	VO_S32					m_nDataSize;
	VO_S32					m_nReadPos;
	VO_S32					m_nWritePos;

	voSource2DataBufferData *		m_pNext;
};

class voSource2DataBufferItem
{
public:
	voSource2DataBufferItem(void);
	virtual ~voSource2DataBufferItem(void);

public:
	VO_SOURCE2_SAMPLE			m_bufHead;

	voSource2DataBufferData *			m_pBufData;
	VO_S32						m_nBuffPos;

	voSource2DataBufferItem *			m_pNext;
};

class voSource2DataBufferItemList
{
public:
	voSource2DataBufferItemList(VO_S32 nMaxSampleSize, VO_S32 nMaxBufferSize);
	virtual ~voSource2DataBufferItemList(void);

	VO_S32		AddBuffer (VO_SOURCE2_SAMPLE * pBuffer);
	VO_S32		GetBuffer (VO_SOURCE2_SAMPLE * pBuffer);

	VO_S32		GetBuffTime (void);
	VO_S32		GetBuffCount (void);

	VO_S32		Flush (void);
	VO_BOOL		IsEOS (void) {return m_bEOS;}


protected:
	VO_S32				AppendFreeItem (voSource2DataBufferItem * pItem);
	VO_S32				CheckBufferData (voSource2DataBufferData * pData, VO_S32 nReadSize);
	VO_S32				AddBufferLocked (VO_SOURCE2_SAMPLE * pBuffer);
	VO_S32				MergeLockedBuffer(VO_SOURCE2_SAMPLE * pBuffer);

protected:
	voCMutex				m_mtxList;

	VO_S32					m_nItemSize;
	VO_S32					m_nMaxBufferSize;

	voSource2DataBufferItem *		m_pCurBuffItem;	// use ptr to record the current item.

	voSource2DataBufferItem *		m_pFirstItemFree;
	voSource2DataBufferItem *		m_pFirstItemFull;
	voSource2DataBufferItem *		m_pLockedItem;

	voSource2DataBufferData *		m_pFirstDataFree;
	voSource2DataBufferData *		m_pFirstDataFull;
	voSource2DataBufferData *		m_pLockedData;

	VO_BOOL					m_bEOS;
	VO_BOOL					m_bLocked;
};

#endif //__voLiveSrcDataBuffer_H__
