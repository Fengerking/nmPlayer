/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/
#include "voMpegMem.h"

void *
alignMalloc(VO_U32 size, VO_U32 alignment)
{
	VO_U8 *mem_ptr;

	if (!alignment) {

		//We have not to satisfy any alignment 
		if ((mem_ptr = (VO_U8 *) malloc(size + 1)) != NULL) {

			//Store (mem_ptr - "real allocated memory") in *(mem_ptr-1) 
			*mem_ptr++ = (VO_U8)1;

			//Return the mem_ptr pointer 
			return ((void *)mem_ptr);
		}
	} else {
		VO_U8 *tmp;

		//Allocate the required size memory + alignment so we can realign the data if necessary 
		if ((tmp = (VO_U8 *)malloc(size + alignment)) != NULL) {

			//Align the tmp pointer 
			mem_ptr = (VO_U8 *) ((VO_U32) (tmp + alignment) & (~(alignment - 1)));

			//(mem_ptr - tmp) is stored in *(mem_ptr-1) so we are able to retrieve the real malloc block allocated and free it in alignFree 
			*(mem_ptr - 1) = (VO_U8) (mem_ptr - tmp);

			//Return the aligned pointer 
			return ((void *)mem_ptr);
		}
	}

	return(NULL);
}

void
alignFree(void *mem_ptr)
{

	VO_U8 *ptr;

	if (mem_ptr == NULL)
		return;

	//Aligned pointer 
	ptr = (VO_U8 *)mem_ptr;

	// *(ptr - 1) holds the nOffset to the real allocated block
	ptr -= *(ptr - 1);

	//Free the memory 
	free(ptr);
}

