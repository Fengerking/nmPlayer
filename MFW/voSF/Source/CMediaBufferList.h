/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CMediaBufferList.cpp

Contains:	CMediaBufferList class file

Written by:	

Change History (most recent first):


*******************************************************************************/

#ifndef _CMEDIABUFFERLIST_H
#define _CMEDIABUFFERLIST_H

#include "voType.h"
#include "voSF.h"


typedef struct tagMemTrunk
{
	int				nOffset;
	int				nRecyleSize;
	tagMemTrunk*	pNextTrunk;
	tagMemTrunk*	pPrewTrunk;
	char *			pMem;
}MemTrunk;

typedef struct tagMemUnit
{
	int			nSize;
	MemTrunk*	pTrunk;
}MemUnit;

class CMemManger
{
public:
	CMemManger();
	~CMemManger();

	void* getMem(int nSize, MemUnit* pMemUnit);
	void  returnMem(MemUnit* pMemUnit);
	void  resetMem();

protected:
	void release();

private:
	MemTrunk*	m_pHeadTrunk;
	MemTrunk*	m_pTailTrunk;
	MemTrunk*	m_pCurrTrunk;
};

class CMediaBufferList
{
public:
	CMediaBufferList(VOSF_MEDIABUFFERTYPE * pMediaBufferOP, VOSF_METADATATYPE * pMetaDataOP);
	~CMediaBufferList();

	bool	addBuffer(void* pBuffer);
	bool    isBufferReady();
	bool	getSample(long long llTime, VO_CODECBUFFER* pBuffer);
	void	arrangeList();
	void	releaseList();
	void	setEOS(bool bEOS){m_bEOS = bEOS;}

	VO_U32	GetDropFrameCount(){return mnDropCount;}

protected:
	typedef struct tagBufferItem 
	{
		int				SyncFrame;
		tagBufferItem*  pNext;
		VO_CODECBUFFER	inputBuffer;
		MemUnit			memUnit;	

	}BufferItem;	

protected:	
	BufferItem*	getNextSyncItem();
	BufferItem* allocItem(int nSize);

protected:
	VOSF_MEDIABUFFERTYPE*	m_pMediaBufferOP;
	VOSF_METADATATYPE*		m_pMetaDataOP;
	CMemManger*				m_pMemanger;

	BufferItem*		m_pHead;
	BufferItem*		m_pCurr;
	BufferItem*		m_pTail;
	VO_S32			m_nSyncCnt;
	bool			m_bEOS;

	//add by Ma Tao for count drop frames.
	VO_U32			mnDropCount;
};
#endif