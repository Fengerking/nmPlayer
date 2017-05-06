/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/
#if !BUILD_WITHOUT_C_LIB
#include <stdlib.h>
#include <stdio.h>
#endif
#include "voMpegMem.h"

#ifdef MEM_LEAKAGE
extern int malloc_count;
extern int free_count;
#endif // MEM_LEAKAGE

VO_VOID *MallocMem(VO_S32 size, VO_U8 alignment,VO_MEM_OPERATOR *pMemOperator)
{
	VO_U8 *mem_ptr;

#ifdef MEM_LEAKAGE
	malloc_count += 1;	
#endif // MEM_LEAKAGE

	if (!alignment) {
		/* We have not to satisfy any alignment */
		if(pMemOperator->Alloc){
			VO_MEM_INFO MemInfo;

			MemInfo.Flag = 0;
			MemInfo.Size = size + 1;
			pMemOperator->Alloc(VO_INDEX_DEC_MPEG2, &MemInfo);
			mem_ptr = (VO_U8 *)MemInfo.VBuffer;
		}else{
			mem_ptr = (VO_U8 *) malloc(size + 1);
		}

		if (NULL != mem_ptr) {
			/* Store (mem_ptr - "real allocated memory") in *(mem_ptr-1) */
			*mem_ptr = (VO_U8)1;

			/* Return the mem_ptr pointer */
			return ((VO_VOID *)(mem_ptr+1));
		}
	} else {
		VO_U8 *tmp;

		/* Allocate the required size memory + alignment so we
		* can realign the data if necessary */
		if(pMemOperator->Alloc){
			VO_MEM_INFO MemInfo;

			MemInfo.Flag = 0;
			MemInfo.Size = size + alignment;
			pMemOperator->Alloc(VO_INDEX_DEC_MPEG2, &MemInfo);
			tmp = (VO_U8 *)MemInfo.VBuffer;
		}else{
			tmp = (VO_U8 *) malloc(size + alignment);
		}

		if (NULL != tmp) {

			/* Align the tmp pointer */
			mem_ptr =
				(VO_U8 *) ((VO_U32) (tmp + alignment - 1) &
				(~(VO_U32) (alignment - 1)));

			/* Special case where malloc have already satisfied the alignment
			* We must add alignment to mem_ptr because we must store
			* (mem_ptr - tmp) in *(mem_ptr-1)
			* If we do not add alignment to mem_ptr then *(mem_ptr-1) points
			* to a forbidden memory space */
			if (mem_ptr == tmp)
				mem_ptr += alignment;

			/* (mem_ptr - tmp) is stored in *(mem_ptr-1) so we are able to retrieve
			* the real malloc block allocated and free it in xvid_free */
			*(mem_ptr - 1) = (VO_U8) (mem_ptr - tmp);

			/* Return the aligned pointer */
			return ((VO_VOID *)mem_ptr);
		}
	}

	return(NULL);
}

VO_VOID FreeMem(VO_VOID *mem_ptr,VO_MEM_OPERATOR *pMemOperator)
{
	VO_U8 *ptr;

	if (mem_ptr == NULL)
		return;

#ifdef MEM_LEAKAGE
	free_count += 1;
#endif // MEM_LEAKAGE

	/* Aligned pointer */
	ptr = (VO_U8 *)mem_ptr;

	/* *(ptr - 1) holds the offset to the real allocated block
	 * we sub that offset os we free the real pointer */
	ptr -= *(ptr - 1);

	/* Free the memory */
	if(pMemOperator->Free){
		pMemOperator->Free(VO_INDEX_DEC_MPEG2,ptr);
	}else{
		free(ptr);
	}
	
}

VO_VOID SetMem(VO_U8 *pDst, VO_U8 nValue, VO_U32 nSize,VO_MEM_OPERATOR *pMemOperator)
{
	if (pMemOperator->Set){
		pMemOperator->Set(VO_INDEX_DEC_MPEG2, pDst, nValue, nSize);
	}else{
		memset(pDst, nValue, nSize);
	}
}

VO_VOID MoveMem(VO_U8 *pDst, VO_U8* pSrc, VO_U32 nSize, VO_MEM_OPERATOR *pMemOperator)
{
	if(pMemOperator->Move){
		pMemOperator->Move(VO_INDEX_DEC_MPEG2, pDst, pSrc, nSize);
	}else{
		memmove(pDst, pSrc, nSize);
	}

}

VO_VOID CopyMem(VO_U8 *pDst, VO_U8* pSrc, VO_U32 nSize, VO_MEM_OPERATOR *pMemOperator)
{
	if(pMemOperator->Copy){
		pMemOperator->Copy(VO_INDEX_DEC_MPEG2, pDst, pSrc, nSize);
	}else{
		memcpy(pDst, pSrc, nSize);
	}
}