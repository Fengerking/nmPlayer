/*
 *  Copyright 2008 by Visualon software Incorporated.
 *  All rights reserved. Property of Visualon software Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */

/***************************** Change History**************************
;* 
;*    DD/MMM/YYYY     Code Ver         Description             Author
;*    -----------     --------     ------------------        -----------
;*    11-25-2008        1.0        File imported from        Huaping Liu
;*                                             
;**********************************************************************/
#include "basop.h"


void *AlignedMalloc(int size)
{
	Word8 advance;
	Word8 *realPt, *alignedPt;
	size += G723_MEM_ALIGN;
	if ((realPt = calloc(size,1)) == NULL)
		return NULL;
	advance = (Word8)(G723_MEM_ALIGN - ((Word32)realPt & G723_MEM_ALIGN_MASK));
	alignedPt = realPt + advance; // to aligned location;
	*(alignedPt - 1) = advance; // save real malloc pt at alignedPt[-1] location for free;
	return alignedPt;
}

void AlignedFree(void *alignedPt)
{
	if (alignedPt)
		free((Word8 *)((Word8 *)alignedPt - *((Word8 *)alignedPt - 1))); // free real malloc location;
}

