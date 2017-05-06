/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#include "mem_align.h"

void* voMemMalloc(Jpeg_DecOBJ * decoder,UINT32 size, UINT8 alignment)
{
    UINT8 *mem_ptr;
    if(decoder->memoryOperator.Alloc)
	{
		VO_MEM_INFO MemInfo;

		MemInfo.Flag = 0;
		MemInfo.Size = size;
#ifdef VOJPEGFLAGE
        decoder->memoryOperator.Alloc(VO_INDEX_DEC_JPEG, &MemInfo);
#else
        decoder->memoryOperator.Alloc(VO_INDEX_DEC_MJPEG, &MemInfo);
#endif 
		mem_ptr = (UINT8 *)MemInfo.VBuffer;
	} 
	else
	{
		mem_ptr = (UINT8 *)mem_malloc(size,  alignment);
	}

    return (void*)mem_ptr;
}

void voMemFree(Jpeg_DecOBJ * decoder,void *mem_ptr)
{
    if(decoder->memoryOperator.Free)
    {
        decoder->memoryOperator.Free(VO_INDEX_DEC_MJPEG, mem_ptr);
		mem_ptr = NULL;
    }
    else
    {
        mem_free(mem_ptr);
        mem_ptr = NULL;
    }
}
void voMemSet(Jpeg_DecOBJ *decoder,void *mem_ptr, INT32 size, VO_U8 value)
{
    if(decoder->memoryOperator.Set)
	{
		decoder->memoryOperator.Set(VO_INDEX_DEC_MJPEG,mem_ptr, value, size);
	} 
	else
	{
		memset(mem_ptr, value, size);
	}
}

void *mem_malloc(VO_U32 size, VO_U8 alignment)
{
	VO_U8 *pMem;
	if (!alignment) {

		// We have not to satisfy any alignment
		if ((pMem = (VO_U8 *) malloc(size + 1)) != NULL) {

			//Store (pMem - "real allocated memory") in *(pMem-1)
			*pMem = (VO_U8)1;

			return ((void *)(pMem+1));
		}
	} else {
		VO_U8 *pTmp;

		if ((pTmp = (VO_U8 *) malloc(size + alignment)) != NULL) {

			pMem = 	(VO_U8 *) ((VO_U32) (pTmp + alignment - 1) &
							 (~(VO_U32) (alignment - 1)));

			// When pMem have already satisfied the alignment,We must add alignment to pMem
			if (pMem == pTmp)
				pMem += alignment;

			*(pMem - 1) = (VO_U8) (pMem - pTmp);
			return ((void *)pMem);
		}
	}

	return(NULL);
}

void mem_free(void *pMem)
{
	VO_U8 *ptr;
	if (pMem == NULL)
		return;
	ptr = pMem;
	// *(ptr - 1) holds the offset to the real allocated buffer
	ptr -= *(ptr - 1);
	free(ptr);
}
