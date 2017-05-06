/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "voPNGMemmory.h"

/*****************************************************************************
 * voMpegMalloc
 *
 * This function allocates 'size' bytes (usable by the user) on the heap and
 * takes care of the requested 'alignment'.
 * In order to align the allocated memory block, the voMpegMalloc allocates
 * 'size' bytes + 'alignment' bytes. So try to keep alignment very small
 * when allocating small pieces of memory.
 *
 * NB : a block allocated by voMpegMalloc _must_ be freed with voMpegFree
 *      (the libc free will return an error)
 *
 * Returned value : - NULL on error
 *                  - Pointer to the allocated aligned block
 *
 ****************************************************************************/

#ifdef MAX_RAM_USED_CHECK
extern int g_RamUsed;
#endif

VO_VOID * voMpegMalloc(PNGDEC *pDecHnd, VO_U32 size, VO_U8 alignment)	//The alignment must be multiple of 4.
{
	VO_U8 *pMem;

#ifdef MAX_RAM_USED_CHECK
	g_RamUsed += size + alignment;
#endif

	if(pDecHnd)
    {
		if(pDecHnd->memOperater)
        {
			if (!alignment) 
            {
				voMemAlloc(pMem, pDecHnd->memOperater, pDecHnd->codec_id, size+1);
				if(pMem == NULL) {
					return(NULL);
				}
				*pMem = (VO_U8)1;
				return ((VO_VOID *)(pMem+1));
			} 
            else 
            {
				VO_U8 *tmp;
				voMemAlloc(tmp, pDecHnd->memOperater, pDecHnd->codec_id, size+alignment);
				if(tmp == NULL)
					return(NULL);
				pMem = (VO_U8 *) ((VO_U32) (tmp + alignment - 1) & (~(VO_U32) (alignment - 1)));
				if (pMem == tmp)
					pMem += alignment;
				*(pMem - 1) = (VO_U8) (pMem - tmp);
				return ((VO_VOID *)pMem);
			}
		}
	}

	if (!alignment) 
    {
        pMem = (VO_U8 *) malloc(size + 1);
        if(pMem == NULL)
            return (NULL);
        *pMem = (VO_U8)1;
        return ((VO_VOID *)(pMem+1));
    }
    else
    {
        VO_U8 *tmp = (VO_U8 *) malloc(size + alignment);
        if(tmp == NULL )
            return(NULL);

        pMem = 	(VO_U8 *) ((VO_U32) (tmp + alignment - 1) & (~(VO_U32) (alignment - 1)));
        if (pMem == tmp)
				pMem += alignment;
        *(pMem - 1) = (VO_U8) (pMem - tmp);
			return ((VO_VOID *)pMem);
    }

	return(NULL);
}

VO_VOID voMpegFree(PNGDEC *pDecHnd, VO_VOID *pMem)
{

	VO_U8 *ptr;

	if (pMem == NULL)
		return;

	if(pDecHnd)
    {
		if(pDecHnd->memOperater)
        {
			ptr = pMem;
			ptr -= *(ptr - 1);
			pDecHnd->memOperater->Free(pDecHnd->codec_id, ptr); 
			return;
		}
	}

	ptr = pMem;
	ptr -= *(ptr - 1);
	free(ptr);
    return;
}
