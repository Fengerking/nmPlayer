#include "stdlib.h"
#include "string.h"
#include "voType.h"
#include "voMem.h"
#include "voVP8Memory.h"

void *MallocMem(VO_CODEC_INIT_USERDATA * pUserData, VO_U32 nCodecIdx, VO_U32 nSize, VO_U32 nAlignment)
{
	VO_U8 *pMem;

	if (!nAlignment) {

		/* We have not to satisfy any alignment */
		if(!pUserData){/* default memory*/
			pMem = (VO_U8*) malloc(nSize + 1);
		}else{
			if(pUserData->memflag == VO_IMF_USERMEMOPERATOR){  
				VO_MEM_OPERATOR *vMemOperator = (VO_MEM_OPERATOR *)pUserData->memData;
				VO_MEM_INFO vMemInfo;

				if(!vMemOperator || !vMemOperator->Alloc)
					return NULL;

				vMemInfo.VBuffer = NULL;
				vMemInfo.Size = nSize + 1;
				vMemOperator->Alloc(nCodecIdx, &vMemInfo); 
				pMem = (VO_U8*)vMemInfo.VBuffer;

			}else if(pUserData->memflag == VO_IMF_PREALLOCATEDBUFFER){ 
				VO_VIDEO_INNER_MEM * pInnerMem = (VO_VIDEO_INNER_MEM *)pUserData->memData;

				if(!pInnerMem || !pInnerMem->nPrivateMemSize || !pInnerMem->pPrivateMem)
					return NULL;

				pMem = (VO_U8*)pInnerMem->pPrivateMem;
				pInnerMem->pPrivateMem += (nSize + 1);
			}else{
				return NULL;
			}
		}

		if(pMem == NULL)
			return NULL;

		/* Store (mem_ptr - "real allocated memory") in *(mem_ptr-1) */
		*pMem = (VO_U8)1;

		/* Return the mem_ptr pointer */
		return ((void *)(pMem+1));


	}else{
		VO_U8 *pTmp;

		/* Allocate the required size memory + alignment so we
		* can realign the data if necessary */

		if(!pUserData){/* default memory*/
			pTmp = (VO_U8*) malloc(nSize + nAlignment);
		}else{
			if(pUserData->memflag == VO_IMF_USERMEMOPERATOR){  
				VO_MEM_OPERATOR *vMemOperator = (VO_MEM_OPERATOR *)pUserData->memData;
				VO_MEM_INFO vMemInfo;

				if(!vMemOperator || !vMemOperator->Alloc)
					return NULL;

				vMemInfo.VBuffer = NULL;
				vMemInfo.Size = nSize + nAlignment;
				vMemOperator->Alloc(nCodecIdx, &vMemInfo); 
				pTmp = (VO_U8*)vMemInfo.VBuffer;

			}else if(pUserData->memflag == VO_IMF_PREALLOCATEDBUFFER){ 
				VO_VIDEO_INNER_MEM * pInnerMem = (VO_VIDEO_INNER_MEM *)pUserData->memData;

				if(!pInnerMem || !pInnerMem->nPrivateMemSize || !pInnerMem->pPrivateMem)
					return NULL;

				pTmp = (VO_U8*)pInnerMem->pPrivateMem;
				pInnerMem->pPrivateMem += (nSize + nAlignment);
			}else{
				return NULL;
			}
		}

		if(pTmp == NULL)
			return NULL;

		/* Align the tmp pointer */
		pMem = (VO_U8 *) ((VO_U32) (pTmp + nAlignment - 1) &
				(~(VO_U32) (nAlignment - 1)));
			

		/* Special case where malloc have already satisfied the alignment
		* We must add alignment to mem_ptr because we must store
		* (mem_ptr - tmp) in *(mem_ptr-1)5
		* If we do not add alignment to mem_ptr then *(mem_ptr-1) points
		* to a forbidden memory space */
		if (pMem == pTmp){
			pMem += nAlignment;
		}

		/* (mem_ptr - tmp) is stored in *(mem_ptr-1) so we are able to retrieve
		* the real malloc block allocated and free it in xvid_free */
		*(pMem - 1) = (VO_U8) (pMem - pTmp);

		/* Return the aligned pointer */
		return ((void *)pMem);
	}

	return(NULL);
}

void
FreeMem(VO_CODEC_INIT_USERDATA * pUserData, VO_U32 nCodecIdx, VO_PTR pDst)
{

	VO_U8 *ptr;

	if (pDst == NULL)
		return;

	/* Aligned pointer */
	ptr = (VO_U8 *)pDst;

	/* *(ptr - 1) holds the nOffset to the real allocated block
	 * we sub that nOffset os we free the real pointer */
	ptr -= *(ptr - 1);

	/* Free the memory */

	if(!pUserData){/* default memory*/
		free(ptr);
		ptr =NULL;
	}else{
		if(pUserData->memflag == VO_IMF_USERMEMOPERATOR){  
			VO_MEM_OPERATOR *vMemOperator = (VO_MEM_OPERATOR *)pUserData->memData;

			if(vMemOperator&&vMemOperator->Free){
				vMemOperator->Free(nCodecIdx, ptr);//TBD

			}
		}
	}
}

void SetMem(VO_CODEC_INIT_USERDATA * pUserData, VO_U32 nCodecIdx, VO_U8 *pDst, VO_U8 nValue, VO_U32 nSize)
{
	if(!pUserData){/* default memory*/
		memset(pDst, nValue, nSize);
	}else{
		if(pUserData->memflag == VO_IMF_USERMEMOPERATOR){ 
			VO_MEM_OPERATOR *vMemOperator = (VO_MEM_OPERATOR *)pUserData->memData;

			if(vMemOperator&&vMemOperator->Set){
				vMemOperator->Set(nCodecIdx, pDst, nValue, nSize); 
			}
		}else{//TBD
			memset(pDst, nValue, nSize);
		}
	}
}

void CopyMem(VO_CODEC_INIT_USERDATA * pUserData, VO_U32 nCodecIdx, VO_U8 *pDst, VO_U8* pSrc, VO_U32 nSize)
{
	if(!pUserData){/* default memory*/
		memcpy(pDst, pSrc, nSize);
	}else{
		if(pUserData->memflag == VO_IMF_USERMEMOPERATOR){  
			VO_MEM_OPERATOR *vMemOperator = (VO_MEM_OPERATOR *)pUserData->memData;

			if(vMemOperator&&vMemOperator->Copy){
				vMemOperator->Copy(nCodecIdx, pDst, pSrc, nSize); 
			}
		}else{//TBD
			memcpy(pDst, pSrc, nSize);
		}
	}
}