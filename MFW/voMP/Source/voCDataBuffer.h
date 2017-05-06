	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCDataBuffer.h

	Contains:	voCDataBuffer header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-09-20	JBF			Create file

*******************************************************************************/
#ifndef __voCDataBuffer_H__
#define __voCDataBuffer_H__

#include <stdlib.h>

#include "voIndex.h"
#include "voCMutex.h"

#include "vompType.h"

#define VO_ERR_SOURCE_NEW_PROGRAM		0X10000001
#define VO_ERR_SOURCE_NEW_FORMAT		0X10000002
#define VO_ERR_SOURCE_DROP_FRAME		0X10000003
#define VO_ERR_SOURCE_TIME_RESET		0X10000004


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

class voCDataBufferData
{
public:
	voCDataBufferData(void);
	virtual ~voCDataBufferData(void);

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

	voCDataBufferData *		m_pNext;
};

class voCDataBufferItem
{
public:
	voCDataBufferItem(void);
	virtual ~voCDataBufferItem(void);

public:
	VOMP_BUFFERTYPE				m_bufHead;

	voCDataBufferData *			m_pBufData;
	VO_S32						m_nBuffPos;

	voCDataBufferItem *			m_pNext;
};

class voCDataBufferList
{
public:
	voCDataBufferList(VO_S32 nItemSize, VO_S32 nVideoBuffer);
	virtual ~voCDataBufferList(void);

	VO_S32		AddBuffer (VOMP_BUFFERTYPE * pBuffer);
	VO_S32		GetBuffer (VOMP_BUFFERTYPE * pBufferr, VO_BOOL bCopy = VO_FALSE);

	VO_S32		GetBuffTime (void);
	VO_S32		GetBuffCount (void);

	VO_S32		Flush (void);
	VO_BOOL		IsEOS (void) {return m_bEOS;}


protected:
	voCDataBufferItem *	GetNextKeyFrame (VO_S64 llTime);
	VO_S32				AppendFreeItem (voCDataBufferItem * pItem);
	VO_S32				CheckBufferData (voCDataBufferData * pData, VO_S32 nReadSize);

protected:
	voCMutex				m_mtxList;

	VO_S32					m_nItemSize;
	VO_S32					m_nVideoBuffer;

	voCDataBufferItem *		m_pCurBuffItem;	// use ptr to record the current item.

	voCDataBufferItem *		m_pFirstItemFree;
	voCDataBufferItem *		m_pFirstItemFull;
	voCDataBufferItem *		m_pLastItemFull;

	voCDataBufferData *		m_pFirstDataFree;
	voCDataBufferData *		m_pFirstDataFull;
	voCDataBufferData *		m_pLastDataFull;

	VO_BOOL					m_bEOS;
};

#endif //__voCDataBuffer_H__
