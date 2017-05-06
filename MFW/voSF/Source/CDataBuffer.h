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
#ifndef __CDataBuffer_H__
#define __CDataBuffer_H__

#include <stdlib.h>

#include "voIndex.h"
#include "voCMutex.h"

#include "vompType.h"

#define VO_ERR_SOURCE_NEW_PROGRAM		0X10000001
#define VO_ERR_SOURCE_NEW_FORMAT		0X10000002


class voCDataBufferData
{
public:
	voCDataBufferData(void)
	{
		m_pData = NULL;
		m_nDataSize = 0;
		m_nReadPos = 0;
		m_nWritePos = 0;

		m_pNext = NULL;
	}

	virtual ~voCDataBufferData(void)
	{
		if (m_pData != NULL)
			free (m_pData);
	}

	VO_S32	AllocData (VO_S32 nSize)
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

	VO_S32 WriteData (VO_BYTE * pData, VO_S32 nSize)
	{
		if (m_pData == NULL)
			return 0;

		if (m_nDataSize - m_nWritePos < nSize)
			return 0;

		memcpy (m_pData + m_nWritePos, pData, nSize);
		m_nWritePos += nSize;

		return nSize;
	}

	VO_S32 ReadData (VO_BYTE * pData, VO_S32 nSize)
	{
		if (m_pData == NULL)
			return 0;

		if (m_nWritePos - m_nReadPos < nSize)
			return 0;

		memcpy (pData, m_pData + m_nReadPos, nSize);
		m_nReadPos += nSize;

		return nSize;
	}

	VO_S32 GetData (VO_BYTE ** ppData, VO_S32 nSize)
	{
		if (m_pData == NULL)
			return 0;

		if (m_nWritePos - m_nReadPos < nSize)
			return 0;

		*ppData = m_pData + m_nReadPos;
		m_nReadPos += nSize;

		return nSize;
	}

	VO_S32	GetAvailableSize (void)
	{
		return m_nDataSize - m_nWritePos;
	}

	VO_S32	GetBufferSize (void)
	{
		return m_nWritePos - m_nReadPos;
	}

public:
	VO_BYTE *				m_pData;
	VO_S32					m_nDataSize;
	VO_S32					m_nReadPos;
	VO_S32					m_nWritePos;

	voCDataBufferData *	m_pNext;
};

class voCDataBufferItem
{
public:
	voCDataBufferItem(void)
	{
		m_pBufData = NULL;
		m_nBuffPos = 0;

		m_pNext = NULL;
	}

	virtual ~voCDataBufferItem(void)
	{
	}

public:
	VOMP_BUFFERTYPE				m_bufHead;

	voCDataBufferData *			m_pBufData;
	VO_S32						m_nBuffPos;

	voCDataBufferItem *			m_pNext;
};

class voCDataBufferList
{
public:
	voCDataBufferList(VO_S32 nItemSize, bool bVideoBuffer);
	virtual ~voCDataBufferList(void);

	VO_S32		AddBuffer (VOMP_BUFFERTYPE * pBuffer);
	VO_S32		GetBuffer (VOMP_BUFFERTYPE * pBufferr, VO_BOOL bCopy = VO_FALSE);

	VO_S32		GetBuffTime (void);

	VO_S32		Flush (void);
	VO_BOOL		IsEOS (void) {return m_bEOS;}


protected:
	voCDataBufferItem *	GetNextKeyFrame (VO_S64 llTime);
	VO_S32				AppendFreeItem (voCDataBufferItem * pItem);
	VO_S32				CheckBufferData (voCDataBufferData * pData, VO_S32 nReadSize);

protected:
	VO_S32					m_nItemSize;
	bool					m_bVideoBuffer;

	voCDataBufferItem *		m_pFirstItemFree;
	voCDataBufferItem *		m_pFirstItemFull;
	voCDataBufferItem *		m_pLastItemFull;

	voCDataBufferData *		m_pFirstDataFree;
	voCDataBufferData *		m_pFirstDataFull;
	voCDataBufferData *		m_pLastDataFull;

	voCMutex				m_mtxList;
	VO_BOOL					m_bEOS;

};

#endif //__voCDataBuffer_H__
