/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#include "mem_align.h"

void *
mem_malloc(UINT32 size, UINT8 alignment)
{
	UINT8 *mem_ptr;

	if (!alignment) {

		/* We have not to satisfy any alignment */
		if ((mem_ptr = (UINT8 *) malloc(size + 1)) != NULL) {

			/* Store (mem_ptr - "real allocated memory") in *(mem_ptr-1) */
			*mem_ptr = (UINT8)1;

			/* Return the mem_ptr pointer */
			return ((void *)(mem_ptr+1));
		}
	} else {
		UINT8 *tmp;

		/* Allocate the required size memory + alignment so we
		 * can realign the data if necessary */
		if ((tmp = (UINT8 *) malloc(size + alignment)) != NULL) {

			/* Align the tmp pointer */
			mem_ptr =
				(UINT8 *) ((UINT32) (tmp + alignment - 1) &
							 (~(UINT32) (alignment - 1)));

			/* Special case where malloc have already satisfied the alignment
			 * We must add alignment to mem_ptr because we must store
			 * (mem_ptr - tmp) in *(mem_ptr-1)
			 * If we do not add alignment to mem_ptr then *(mem_ptr-1) points
			 * to a forbidden memory space */
			if (mem_ptr == tmp)
				mem_ptr += alignment;

			/* (mem_ptr - tmp) is stored in *(mem_ptr-1) so we are able to retrieve
			 * the real malloc block allocated and free it in xvid_free */
			*(mem_ptr - 1) = (UINT8) (mem_ptr - tmp);

			/* Return the aligned pointer */
			return ((void *)mem_ptr);
		}
	}

	return(NULL);
}

/*****************************************************************************
 * xvid_free
 *
 * Free a previously 'xvid_malloc' allocated block. Does not free NULL
 * references.
 *
 * Returned value : None.
 *
 ****************************************************************************/

void
mem_free(void *mem_ptr)
{

	UINT8 *ptr;

	if (mem_ptr == NULL)
		return;

	/* Aligned pointer */
	ptr = mem_ptr;

	/* *(ptr - 1) holds the offset to the real allocated block
	 * we sub that offset os we free the real pointer */
	ptr -= *(ptr - 1);

	/* Free the memory */
	free(ptr);
}
