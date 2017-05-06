	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CMemPool.h

	Contains:	CMemPool header file

	Written by:	East

	Change History (most recent first):
	2008-11-05		East			Create file

*******************************************************************************/
#pragma once

#include "voYYDef_filcmn.h"
#include "voType.h"
#include "CvoBaseMemOpr.h"
#include "fMacros.h"
#if defined LINUX
#include <string.h>
#endif
#if defined _MAC_OS || defined(_IOS)
#include <string.h>
#endif


#ifdef __SYMBIAN32__
#include <prtypes.h>
#endif //__SYMBIAN32__

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


class CMemPool
	: public CvoBaseMemOpr
{
	typedef struct tagMemElem
	{
		tagMemElem*		next;
	} MemElem, *PMemElem;

	typedef struct tagMemBlock
	{
		VO_PTR			mem_ptr;
		tagMemBlock*	next;
	} MemBlock, *PMemBlock;

public:
	CMemPool(VO_MEM_OPERATOR* _mem_opr);
	virtual ~CMemPool();

public:
	VO_U32	getref() {return m_nRef;}
	VO_VOID	addref() {m_nRef++;}
	VO_VOID	release() {m_nRef--;};

	VO_PTR	alloc(size_t _size);
	VO_VOID	free(VO_PTR _p, size_t _size);

protected:
	VO_VOID	free_all();
	//base allocate function
	VO_PTR	alloc_b(size_t _size);

protected:
	PMemBlock	m_pHead;
	PMemBlock	m_pTail;
	PMemElem	m_pHeadOfFreeList;

	VO_U32		m_nRef;
};

#ifdef _VONAMESPACE
}
#endif
