	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CMemPool.cpp

	Contains:	CMemPool class file

	Written by:	East

	Change History (most recent first):
	2008-11-05		East			Create file

*******************************************************************************/
#include "CMemPool.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define MEM_POOL_BLOCK_SIZE				1024

CMemPool::CMemPool(VO_MEM_OPERATOR* _mem_opr)
	: CvoBaseMemOpr(_mem_opr)
	, m_pHead(VO_NULL)
	, m_pTail(VO_NULL)
	, m_pHeadOfFreeList(VO_NULL)
	, m_nRef(0)
{
}

CMemPool::~CMemPool()
{
	free_all();
}

VO_PTR CMemPool::alloc(size_t _size)
{
	if(m_pHeadOfFreeList)
	{
		PMemElem _p = m_pHeadOfFreeList;
		m_pHeadOfFreeList = _p->next;
		return _p;
	}
	else
	{
		VO_PTR pNewBlock = alloc_b(MEM_POOL_BLOCK_SIZE * _size);
		if(!pNewBlock)
			return VO_NULL;

		VO_PBYTE p = VO_PBYTE(pNewBlock) + _size;
		m_pHeadOfFreeList = (PMemElem)p;

		for(VO_U32 i = 1; i < MEM_POOL_BLOCK_SIZE - 1; ++i)
		{
			PMemElem(p)->next = PMemElem(p + _size);
			p += _size;
		}

		PMemElem(p)->next = VO_NULL;
		return pNewBlock;
	}
}

VO_VOID CMemPool::free(VO_PTR _p, size_t _size)
{
	PMemElem carcass = static_cast<PMemElem>(_p);

	carcass->next = m_pHeadOfFreeList;
	m_pHeadOfFreeList = carcass;
}

VO_PTR CMemPool::alloc_b(size_t _size)
{
	//allocate memory
	VO_PTR p = MemAlloc(_size);
	if(!p)
		return VO_NULL;

	//new a block to record the memory pointer, so that we can free the memory!!
	PMemBlock pNew = NEW_OBJ(MemBlock);
	if(!pNew)
	{
		MemFree(p);
		return VO_NULL;
	}

	pNew->mem_ptr = p;
	pNew->next = VO_NULL;

	if(m_pTail)
	{
		m_pTail->next = pNew;
		m_pTail = pNew;
	}
	else
		m_pHead = m_pTail = pNew;

	return p;
}

VO_VOID CMemPool::free_all()
{
	PMemBlock pTmp = m_pHead;
	PMemBlock pDel = VO_NULL;
	while(pTmp)
	{
		pDel = pTmp;
		pTmp = pTmp->next;
		//free memory
		MemFree(pDel->mem_ptr);
		//free block
		MemFree(pDel);
	}
}