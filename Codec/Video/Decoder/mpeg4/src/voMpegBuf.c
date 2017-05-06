/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#include "voMpeg4Dec.h"
#include "voMpegBuf.h"
#include "voMpegMem.h"

void * MallocMem(VO_CODEC_INIT_USERDATA * pUserData, VO_U32 nCodecIdx, VO_U32 nSize, VO_U32 nAlignment)
{
	VO_U8 *pMem;

	if (!nAlignment) {

		//We have not to satisfy any alignment 
		if(!pUserData){		//default memory
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

		//Store (mem_ptr - "real allocated memory") in *(mem_ptr-1) 
		*pMem = (VO_U8)1;

		//Return the mem_ptr pointer 
		return ((void *)(pMem+1));


	}else{
		VO_U8 *pTmp;

		//Allocate the required (size memory + alignment) so we can realign the data if necessary 
		if(!pUserData){		//default memory
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

		//Align the tmp pointer 
		pMem = (VO_U8 *) ((VO_U32) (pTmp + nAlignment) & (~(nAlignment - 1)));
			
		//(mem_ptr - tmp) is stored in *(mem_ptr-1) so we are able to retrieve the real malloc block allocated and free it in FreeMem 
		*(pMem - 1) = (VO_U8) (pMem - pTmp);

		//Return the aligned pointer 
		return ((void *)pMem);
	}

	return(NULL);
}

void FreeMem(VO_CODEC_INIT_USERDATA * pUserData, VO_U32 nCodecIdx, VO_PTR pDst)
{

	VO_U8 *ptr;

	if (pDst == NULL)
		return;

	//Aligned pointer 
	ptr = (VO_U8 *)pDst;

	// *(ptr - 1) holds the nOffset to the real allocated block 
	ptr -= *(ptr - 1);

	//Free the memory 

	if(!pUserData){		//default memory
		free(ptr);
	}else{
		if(pUserData->memflag == VO_IMF_USERMEMOPERATOR){  
			VO_MEM_OPERATOR *vMemOperator = (VO_MEM_OPERATOR *)pUserData->memData;

			if(vMemOperator&&vMemOperator->Free){
				vMemOperator->Free(nCodecIdx, ptr);//TBD

			}
		}
	}
}

#if 0
void *MallocMem(VO_CODEC_INIT_USERDATA * pUserData, VO_U32 nCodecIdx, VO_U32 nSize, VO_U32 nAlignment)
{
	VO_U8 *pMem = NULL;

	if(!pUserData){/* default memory*/
		pMem = (void*)alignMalloc(nSize, nAlignment);
	}else{
		if(pUserData->memflag == VO_IMF_USERMEMOPERATOR){  
			VO_MEM_OPERATOR *vMemOperator = (VO_MEM_OPERATOR *)pUserData->memData;
			VO_MEM_INFO vMemInfo;
			VO_U8 *pTmp;

			if(!vMemOperator || !vMemOperator->Alloc)
				return NULL;

			vMemInfo.VBuffer = NULL;
			vMemInfo.Size = nSize;
			vMemOperator->Alloc(nCodecIdx, &vMemInfo); 
			pTmp = vMemInfo.VBuffer;

			pMem = (VO_U8 *) ((VO_U32) (pTmp + nAlignment - 1) &
							 (~(VO_U32) (nAlignment - 1)));
				

			if (pMem == pTmp)
				pMem += nAlignment;

			*(pMem - 1) = (VO_U8) (pMem - pTmp);

		}else if(pUserData->memflag == VO_IMF_PREALLOCATEDBUFFER){ 
			VO_VIDEO_INNER_MEM * pInnerMem = (VO_VIDEO_INNER_MEM *)pUserData->memData;

			if(!pInnerMem || !pInnerMem->nPrivateMemSize || !pInnerMem->pPrivateMem)
				return NULL;
			
			pMem = (void*)pInnerMem->pPrivateMem;
		}else{
			return NULL;
		}
	}

	return pMem;
}
#endif


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
#if 0
void FreeMem(VO_CODEC_INIT_USERDATA * pUserData, VO_U32 nCodecIdx, VO_PTR pDst)
{
	if(pDst){
		if(!pUserData){/* default memory*/
			alignFree(pDst);
		}else{
			if(pUserData->memflag == VO_IMF_USERMEMOPERATOR){  
				VO_MEM_OPERATOR *vMemOperator = (VO_MEM_OPERATOR *)pUserData->memData;

				if(vMemOperator&&vMemOperator->Free){
					VO_U8 *pTmp;

					pTmp = (VO_U8 *)pDst;

					pTmp -= *(pTmp - 1);
					vMemOperator->Free(nCodecIdx, pTmp);//TBD

				}
			}
		}
	}
}
#endif

void
SwapImage(VO_IMGYUV ** iMgDst,VO_IMGYUV ** iMgSrc)
{
	VO_IMGYUV *tmp;
	
	tmp = *iMgDst;
	*iMgDst = *iMgSrc;
	*iMgSrc = tmp;
}


void
ColoneImage(VO_CODEC_INIT_USERDATA * pUserData,
		  VO_U32 nCodecIdx,
		  VO_IMGYUV * iMgDst,
		   VO_IMGYUV * iMgSrc,
		   VO_U32 ExWidth,
		   VO_U32 nHeight)
{
	CopyMem(pUserData, nCodecIdx, iMgDst->y, iMgSrc->y, ExWidth * nHeight);
	CopyMem(pUserData, nCodecIdx, iMgDst->u, iMgSrc->u, ExWidth * nHeight / 4);
	CopyMem(pUserData, nCodecIdx, iMgDst->v, iMgSrc->v, ExWidth * nHeight / 4);
}

#if defined(VOARMV7) || defined(VOARMV6)
extern void fill_edge_y_armv6(VO_U8 *src_in, const VO_U32 ExWidth, VO_U32 width, VO_U32 height, VO_U32 edge);
extern void fill_edge_uv_armv6(VO_U8 *src_in, const VO_U32 ExWidth, VO_U32 width, VO_U32 height, VO_U32 edge);
#endif
void
SetImageEdge(VO_CODEC_INIT_USERDATA * pUserData,
			 VO_U32 nCodecIdx,
			 VO_IMGYUV * image,
			   const VO_U32 ExWidthY,
			   const VO_U32 ExWidthUV,
			   const VO_U32 ExHeight,
			   VO_U32 nWidth,
			   VO_U32 nHeight)
{
#if defined(VOARMV7) || defined(VOARMV6)
	fill_edge_y_armv6(image->y, ExWidthY, nWidth, nHeight, EDGE_SIZE);
	fill_edge_uv_armv6(image->u, ExWidthUV, nWidth/2, nHeight/2, EDGE_SIZE2);
	fill_edge_uv_armv6(image->v, ExWidthUV, nWidth/2, nHeight/2, EDGE_SIZE2);		
#else
// 	const VO_U32 edged_width2 = ExWidthUV;
	VO_U32 nWidth2, nHeight2;
	VO_U32 i;
	VO_U8 *dst;
	VO_U8 *src;

	dst = image->y - (EDGE_SIZE + EDGE_SIZE * ExWidthY);
	src = image->y;

	nWidth  = (nWidth+15)&~15;
	nHeight = (nHeight+15)&~15;


	nWidth2 = nWidth>>1;
	nHeight2 = nHeight>>1;

	for (i = 0; i < EDGE_SIZE; i++) {
		SetMem(pUserData, nCodecIdx, dst, *src, EDGE_SIZE);
		CopyMem(pUserData, nCodecIdx, dst + EDGE_SIZE, src, nWidth);
		SetMem(pUserData, nCodecIdx, dst + EDGE_SIZE + nWidth, *(src + nWidth - 1),
			   EDGE_SIZE);
		dst += ExWidthY;
	}

	for (i = 0; i < nHeight; i++) {
		SetMem(pUserData, nCodecIdx, dst, *src, EDGE_SIZE);
		SetMem(pUserData, nCodecIdx, dst + nWidth + EDGE_SIZE, src[nWidth - 1], EDGE_SIZE);
		dst += ExWidthY;
		src += ExWidthY;
	}

	src -= ExWidthY;
	for (i = 0; i < EDGE_SIZE; i++) {
		SetMem(pUserData, nCodecIdx, dst, *src, EDGE_SIZE);
		CopyMem(pUserData, nCodecIdx, dst + EDGE_SIZE, src, nWidth);
		SetMem(pUserData, nCodecIdx, dst + nWidth + EDGE_SIZE, *(src + nWidth - 1),
				   EDGE_SIZE);
		dst += ExWidthY;
	}


	/* U */
	dst = image->u - (EDGE_SIZE2 + EDGE_SIZE2 * ExWidthUV);
	src = image->u;
	
	for (i = 0; i < EDGE_SIZE2; i++) {
		SetMem(pUserData, nCodecIdx, dst, *src, EDGE_SIZE2);
		CopyMem(pUserData, nCodecIdx, dst + EDGE_SIZE2, src, nWidth2);
		SetMem(pUserData, nCodecIdx, dst + nWidth2 + EDGE_SIZE2, *(src + nWidth2 - 1),
			EDGE_SIZE2);
		dst += ExWidthUV;
	}
	
	for (i = 0; i < nHeight2; i++) {
		SetMem(pUserData, nCodecIdx, dst, *src, EDGE_SIZE2);
		SetMem(pUserData, nCodecIdx, dst + nWidth2 + EDGE_SIZE2, src[nWidth2 - 1], EDGE_SIZE2);
		dst += ExWidthUV;
		src += ExWidthUV;
	}
	src -= ExWidthUV;
	for (i = 0; i < EDGE_SIZE2; i++) {
		SetMem(pUserData, nCodecIdx, dst, *src, EDGE_SIZE2);
		CopyMem(pUserData, nCodecIdx, dst + EDGE_SIZE2, src, nWidth2);
		SetMem(pUserData, nCodecIdx, dst + nWidth2 + EDGE_SIZE2, *(src + nWidth2 - 1),
			EDGE_SIZE2);
		dst += ExWidthUV;
	}
	
	
	/* V */
	dst = image->v - (EDGE_SIZE2 + EDGE_SIZE2 * ExWidthUV);
	src = image->v;
	
	for (i = 0; i < EDGE_SIZE2; i++) {
		SetMem(pUserData, nCodecIdx, dst, *src, EDGE_SIZE2);
		CopyMem(pUserData, nCodecIdx, dst + EDGE_SIZE2, src, nWidth2);
		SetMem(pUserData, nCodecIdx, dst + nWidth2 + EDGE_SIZE2, *(src + nWidth2 - 1),
			EDGE_SIZE2);
		dst += ExWidthUV;
	}
	
	for (i = 0; i < nHeight2; i++) {
		SetMem(pUserData, nCodecIdx, dst, *src, EDGE_SIZE2);
		SetMem(pUserData, nCodecIdx, dst + nWidth2 + EDGE_SIZE2, src[nWidth2 - 1], EDGE_SIZE2);
		dst += ExWidthUV;
		src += ExWidthUV;
	}
	src -= ExWidthUV;
	for (i = 0; i < EDGE_SIZE2; i++) {
		SetMem(pUserData, nCodecIdx, dst, *src, EDGE_SIZE2);
		CopyMem(pUserData, nCodecIdx, dst + EDGE_SIZE2, src, nWidth2);
		SetMem(pUserData, nCodecIdx, dst + nWidth2 + EDGE_SIZE2, *(src + nWidth2 - 1),
			EDGE_SIZE2);
		dst += ExWidthUV;
	}
#endif
}

VO_S32 GetFrameBufIdx(VO_IMGYUV* img, VO_IMGYUV* img_seq)
{	
	VO_S32 i;

	for(i = 0; i < MAXFRAMES; i++){
		if(img == (img_seq + i)){
			return i;
		}
	}
	return 0;
}


VO_IMGYUV*  
FrameBufCtl(FIFOTYPE *priv, VO_IMGYUV* img , const VO_U32 flag)
{
    VO_IMGYUV *temp = NULL;
    VO_U32 w_idx = 0;

    w_idx = priv->w_idx;    /* Save the old index before proceeding */
    if (flag == FIFO_WRITE){ /*write data*/

        /* Save it to buffer */
        if (((priv->w_idx + 1) % FIFO_NUM) == priv->r_idx) {
            /* Adjust read index since buffer is full */
            /* Keep the latest one and drop the oldest one */
            priv->r_idx = (priv->r_idx + 1) % FIFO_NUM;
        }
        priv->img_seq[priv->w_idx] = img;
        priv->w_idx = (priv->w_idx + 1) % FIFO_NUM;

    }else{/*read data*/
		if ((priv->r_idx == w_idx)){
            return NULL;/*there is no free buffer*/
		}else{
            temp = priv->img_seq[priv->r_idx];
            priv->r_idx = (priv->r_idx + 1) % FIFO_NUM;
            }
        }
     return temp;

}

