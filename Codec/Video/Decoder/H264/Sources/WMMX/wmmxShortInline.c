
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/

/*!
************************************************************************
*
* \file		avc_neighbor.h
*
* \brief
*		defines store neighboring infomation encoding/decoding
*
************************************************************************
*/
#include "global.h"
#include "image.h"
#if VOI_H264D_NON_BASELINE

#ifdef WMMX
#define AVGBLOCK
#endif

//#define BLOCKAVG
void WMMXSIAvgBlock(avdUInt8 *img,						
					avdUInt8 *tmp_block,				
					avdNativeInt idx8x8,				
					avdNativeInt imgwidth);
void SILumaAvgBlock(avdNativeInt xIdx, avdNativeInt yIdx, avdUInt8 *tmp_block)
{
	
	//avdNativeInt ii, jj;
#ifdef AVGBLOCK
	StorablePicture	*dec_picture = img->dec_picture;

#ifdef NEW_YUV_MEMORY
	avdUInt8 * OriImg = &(dec_picture->imgY[yIdx][xIdx]);
#else //NEW_YUV_MEMORY
	avdUInt8 * OriImg = dec_picture->plnY + yIdx * img->yPlnPitch + xIdx;
#endif //NEW_YUV_MEMORY

#ifdef BLOCKAVG
	return;
#endif
	//if(idx8x8==4)
	{
		WMMXSIAvgBlock(OriImg,tmp_block,idx8x8,img->width);
		return;
	}
	
#else	
#ifdef NEW_GET_BLOCK
	avdNativeInt blockSizeX = img->gbSizeX;
	avdNativeInt blockSizeY = img->gbSizeY;
#else
	avdNativeInt blockSizeX = img->getBlockSize;
	avdNativeInt blockSizeY = img->getBlockSize;
#endif
	avdNativeInt ii, jj;
	for(jj=0;jj<blockSizeY;jj++)  
	for(ii=0;ii<blockSizeX;ii++)
		dec_picture->imgY[yIdx+jj][xIdx+ii] = ((tmp_block[(jj<<TMP_BLOCK_SHIFT)+ii]
			+dec_picture->imgY[yIdx+jj][xIdx+ii]+1)>>1);
	
#endif
}

void SIChromaAvgBlock(avdNativeInt xTmp, avdNativeInt yTmp, avdUInt8 **imgUV2D, avdUInt8 *tmp_block)
{
	//avdNativeInt j1, i1;
#ifdef AVGBLOCK
#ifdef NEW_YUV_MEMORY
	avdUInt8 * OriImg = &(imgUV2D[yTmp][xTmp]);
#else //NEW_YUV_MEMORY
	avdUInt8 * OriImg = *imgUV2D;
#endif //NEW_YUV_MEMORY

#ifdef BLOCKAVG
	return;
#endif
	//if(idx8x8!=2)
	{
		WMMXSIAvgBlock(OriImg,tmp_block,idx8x8,img->width>>1);
		return;
	}
#else
	
#ifdef NEW_GET_BLOCK
	avdNativeInt blockSizeX = (img->gbSizeX>>1);
	avdNativeInt blockSizeY = (img->gbSizeY>>1);
#else
	avdNativeInt blockSizeX = (img->getBlockSize>>1);
	avdNativeInt blockSizeY = (img->getBlockSize>>1);
#endif
	avdNativeInt j1, i1;
	for (j1 = 0; j1 < blockSizeY; j1++)
	for (i1 = 0; i1 < blockSizeX; i1++)
		imgUV2D[yTmp+j1][xTmp+i1] = ((imgUV2D[yTmp+j1][xTmp+i1] 
			+ tmp_block[(j1<<TMP_BLOCK_SHIFT)+i1] + 1)>>1);
#endif
}

#endif // VOI_H264D_NON_BASELINE
