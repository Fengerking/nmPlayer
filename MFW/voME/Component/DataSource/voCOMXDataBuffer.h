	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXDataBuffer.h

	Contains:	voCOMXDataBuffer header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXDataBuffer_H__
#define __voCOMXDataBuffer_H__

#include <stdlib.h>
#include <OMX_ContentPipe.h>

#include <voOMX_Index.h>

#include <voCOMXCompSource.h>
#include <voCOMXThreadMutex.h>

class voCOMXDataBufferData
{
public:
	voCOMXDataBufferData(void)
	{
		m_pData = NULL;
		m_nDataSize = 0;
		m_nReadPos = 0;
		m_nWritePos = 0;

		m_pNext = NULL;
	}

	virtual ~voCOMXDataBufferData(void)
	{
		if (m_pData != NULL)
			free (m_pData);
	}

	OMX_S32	AllocData (OMX_S32 nSize)
	{
		if (m_pData != NULL)
			free (m_pData);
		m_pData = (OMX_U8 *)malloc (nSize);
		if (m_pData == NULL)
			return 0;

		m_nDataSize = nSize;
		m_nReadPos = 0;
		m_nWritePos = 0;

		return m_nDataSize;
	}

	OMX_S32 WriteData (OMX_U8 * pData, OMX_S32 nSize)
	{
		if (m_pData == NULL)
			return 0;

		if (m_nDataSize - m_nWritePos < nSize)
			return 0;

		memcpy (m_pData + m_nWritePos, pData, nSize);
		m_nWritePos += nSize;

		return nSize;
	}

	OMX_S32 ReadData (OMX_U8 * pData, OMX_S32 nSize)
	{
		if (m_pData == NULL)
			return 0;

		if (m_nWritePos - m_nReadPos < nSize)
			return 0;

		memcpy (pData, m_pData + m_nReadPos, nSize);
		m_nReadPos += nSize;

		return nSize;
	}

	OMX_S32 GetData (OMX_U8 ** ppData, OMX_S32 nSize)
	{
		if (m_pData == NULL)
			return 0;

		if (m_nWritePos - m_nReadPos < nSize)
			return 0;

		*ppData = m_pData + m_nReadPos;
		m_nReadPos += nSize;

		return nSize;
	}

	OMX_S32	GetAvailableSize (void)
	{
		return m_nDataSize - m_nWritePos;
	}

	OMX_S32	GetBufferSize (void)
	{
		return m_nWritePos - m_nReadPos;
	}

public:
	OMX_U8 *				m_pData;
	OMX_S32					m_nDataSize;
	OMX_S32					m_nReadPos;
	OMX_S32					m_nWritePos;

	voCOMXDataBufferData *	m_pNext;
};

class voCOMXDataBufferItem
{
public:
	voCOMXDataBufferItem(void)
	{
		m_pBufData = NULL;
		m_nBuffPos = 0;

		m_pNext = NULL;
	}

	virtual ~voCOMXDataBufferItem(void)
	{
	}

public:
	OMX_BUFFERHEADERTYPE		m_bufHead;

	voCOMXDataBufferData *		m_pBufData;
	OMX_S32						m_nBuffPos;

	voCOMXDataBufferItem *		m_pNext;
};

class voCOMXDataBufferList
{
public:
	voCOMXDataBufferList(OMX_S32 nItemSize, bool bVideoBuffer);
	virtual ~voCOMXDataBufferList(void);

	OMX_ERRORTYPE	AddBuffer (OMX_BUFFERHEADERTYPE * pBuffer);
	OMX_ERRORTYPE	GetBuffer (OMX_BUFFERHEADERTYPE * pBuffer);

	OMX_S32			GetBuffTime (void);

	OMX_ERRORTYPE	Flush (void);
	OMX_ERRORTYPE	GetSeekKeyFrame(OMX_TICKS * pnTimeStamp);

protected:
	voCOMXDataBufferItem *	GetNextKeyFrame (OMX_S64 llTime);
	OMX_ERRORTYPE			AppendFreeItem (voCOMXDataBufferItem * pItem);
	OMX_ERRORTYPE			CheckBufferData (voCOMXDataBufferData * pData, OMX_S32 nReadSize);

protected:
	OMX_S32						m_nItemSize;
	bool						m_bVideoBuffer;

	voCOMXDataBufferItem *		m_pFirstItemFree;
	voCOMXDataBufferItem *		m_pFirstItemFull;
	voCOMXDataBufferItem *		m_pLastItemFull;

	voCOMXDataBufferData *		m_pFirstDataFree;
	voCOMXDataBufferData *		m_pFirstDataFull;
	voCOMXDataBufferData *		m_pLastDataFull;

	voCOMXThreadMutex			m_mtxList;
};

#endif //__voCOMXDataBuffer_H__
