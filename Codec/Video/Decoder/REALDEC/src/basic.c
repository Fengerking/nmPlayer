/********************************************************** BEGIN LICENSE BLOCK ******************************************** 
 * Version: RCSL 1.0 and Exhibits. 
 * REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM 
 * Portions Copyright (c) 1995-2002 RealNetworks, Inc. 
 * All Rights Reserved. 
 * 
 * The contents of this file, and the files included with this file, are 
 * subject to the current version of the RealNetworks Community Source 
 * License Version 1.0 (the "RCSL"), including Attachments A though H, 
 * all available at http://www.helixcommunity.org/content/rcsl. 
 * You may also obtain the license terms directly from RealNetworks. 
 * You may not use this file except in compliance with the RCSL and 
 * its Attachments. There are no redistribution rights for the source 
 * code of this file. Please see the applicable RCSL for the rights, 
 * obligations and limitations governing use of the contents of the file. 
 * 
 * This file is part of the Helix DNA Technology. RealNetworks is the 
 * developer of the Original Code and owns the copyrights in the portions 
 * it created. 
 * 
 * This file, and the files included with this file, is distributed and made 
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
 * 
 * Technology Compatibility Kit Test Suite(s) Location: 
 * https://rarvcode-tck.helixcommunity.org 
 * 
 * Contributor(s): 
 * 
 * ************************************************************ END LICENSE BLOCK ***********************************************/ 

/**************************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information.              
*    Copyright (c) 2001 RealNetworks, Inc.                                              
*    All Rights Reserved.                                                                       
*    Do not redistribute.                                                                          
*    DESCRIPTION:                                                                               
*	  RV89Combo optimized basic add, sub, and copy functions, C version. 
***************************************************************************************/

/***************************************************************************************
*    INTEL Corporation Proprietary Information
*    This listing is supplied under the terms of a license 
*    agreement with INTEL Corporation and may not be copied 
*    nor disclosed except in accordance with the terms of 
*    that agreement.
*    Copyright (c) 1999 - 2000 Intel Corporation. 
*    All Rights Reserved. 
*    DESCRIPTION: 
*	  RV89Combo optimized basic add, sub, and copy functions, C version.
*****************************************************************************************
************************************************************************
*																						  *
*	VisualOn, Inc. Confidential and Proprietary, 2008		          *
*								 														  *
************************************************************************/
/* $Header: /cvsroot/rarvcode-video/codec/rv89combo/platform/emuzed/cdeclib/basic.c,v 1.2 2006/10/05 10:18:58 pncbose Exp $ */
/*	
*******************************************************************************
						Emuzed India Private Limited
*******************************************************************************
Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date		    Author					Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 2nd May, 2005	Real Networks			Initial Code
16th May, 2005	M. Anjaneya Prasad		Optimized Copy, Add, Dequant Functions
******************************************************************************************************************************/
#include <string.h>
#include <stdlib.h>

#include "basic.h"

#ifdef MEM_LEAKAGE
extern I32 malloc_count;
extern I32 free_count;
#endif // MEM_LEAKAGE

void *MallocMem(VO_S32 size, VO_U8 alignment,VO_MEM_OPERATOR *pMemOperator)
{
	VO_U8 *pMem;

#ifdef MEM_LEAKAGE
	malloc_count += 1;	
#endif // MEM_LEAKAGE

	if (!alignment) {
		/* We have not to satisfy any alignment */
		if(pMemOperator->Alloc){
			VO_MEM_INFO MemInfo;

			MemInfo.Flag = 0;
			MemInfo.Size = size + 1;
			pMemOperator->Alloc(VO_INDEX_DEC_RV, &MemInfo);
			pMem = (VO_U8 *)MemInfo.VBuffer;
		}else{
			pMem = (VO_U8 *) malloc(size + 1);
		}

		if (NULL != pMem) {
			/* Store (pMem - "real allocated memory") in *(mem_ptr-1) */
			*pMem = (VO_U8)1;

			return ((void *)(pMem+1));
		}
	} else {
		VO_U8 *pTmp;

		if(pMemOperator->Alloc){
			VO_MEM_INFO MemInfo;

			MemInfo.Flag = 0;
			MemInfo.Size = size + alignment;
			pMemOperator->Alloc(VO_INDEX_DEC_RV, &MemInfo);
			pTmp = (VO_U8 *)MemInfo.VBuffer;
		}else{
			pTmp = (VO_U8 *) malloc(size + alignment);
		}

		if (NULL != pTmp) {

			pMem = (VO_U8 *) ((VO_U32) (pTmp + alignment - 1) &
				(~(VO_U32) (alignment - 1)));

			//When pMem satisfied the alignment,We must add alignment to pMem.
			//Since we must store * (pMem - pTmp) in *(pMem-1)
			if (pMem == pTmp) 
				pMem += alignment;

			*(pMem - 1) = (VO_U8) (pMem - pTmp);

			return ((void *)pMem);
		}
	}

	return(NULL);
}

void FreeMem(VO_VOID *pMem,VO_MEM_OPERATOR *pMemOperator)
{
	U8 *ptr;

	if (pMem == NULL)
		return;

#ifdef MEM_LEAKAGE
	free_count += 1;
#endif // MEM_LEAKAGE

	/* Aligned pointer */
	ptr = (VO_U8 *)pMem;

	/* *(ptr - 1) holds the offset to the real allocated block
	 * we sub that offset os we free the real pointer */
	ptr -= *(ptr - 1);

	/* Free the memory */
	if(pMemOperator->Free){
		pMemOperator->Free(VO_INDEX_DEC_RV,ptr);
	}else{
		free(ptr);
	}
	
}

void SetMem(VO_U8 *pDst, VO_U8 nValue, VO_U32 nSize,VO_MEM_OPERATOR *pMemOperator)
{
	if (pMemOperator->Set){
		pMemOperator->Set(VO_INDEX_DEC_RV, pDst, nValue, nSize);
	}else{
		memset(pDst, nValue, nSize);
	}
}

void MoveMem(VO_U8 *pDst, VO_U8* pSrc, VO_U32 nSize, VO_MEM_OPERATOR *pMemOperator)
{
	if(pMemOperator->Move){
		pMemOperator->Move(VO_INDEX_DEC_RV, pDst, pSrc, nSize);
	}else{
		memmove(pDst, pSrc, nSize);
	}

}

void CopyMem(VO_U8 *pDst, VO_U8* pSrc, VO_U32 nSize, VO_MEM_OPERATOR *pMemOperator)
{
	if(pMemOperator->Copy){
		pMemOperator->Copy(VO_INDEX_DEC_RV, pDst, pSrc, nSize);
	}else{
		memcpy(pDst, pSrc, nSize);
	}
}




