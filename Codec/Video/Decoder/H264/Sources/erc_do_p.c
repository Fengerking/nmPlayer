#if !BUILD_WITHOUT_C_LIB
#include <assert.h>
#endif//BUILD_WITHOUT_C_LIB
#include "global.h"
#include "defines.h"
#include "loopFilter.h"
#include "global.h"
//#include "H264_C_Type.h"
#include "avd_neighbor.h"
#include "mbuffer.h"
#include "elements.h"
#include "errorconcealment.h"
#include "macroblock.h"
#include "fmo.h"
#include "cabac.h"
#include "vlc.h"
#include "image.h"
#include "mb_access.h"
#include "biaridecod.h"
#include "block.h"
#define  AVD_SAVE_TMPAB_TO_OUT() \
	*byteOutStart++ = tmpa; \
	*byteOutStart++ = tmpb;
#ifdef ARM_ASM
#if 1//ndef DISABLE_GBC
#define  USE_ARM_ASM_GBC 1
#endif
#endif//ARM_ASM
void GetBlockChromaNxN(ImageParameters *img,TMBsProcessor *info,avdNativeInt xOffset, avdNativeInt yOffset, StorablePicture* refPic,int uv, 
					   avdUInt8 *byteOutStart, avdNativeInt outLnLength, avdInt16 *clpHt,
					   int gbSizeX,int gbSizeY)
{
	avdUInt8 *ref0, *ref1;
	avdNativeInt x0, y0, tmpa, tmpb;
	avdNativeInt k, width, width2, width3;

	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
	avdNativeInt blockSizeX = (gbSizeX>>1);
	avdNativeInt blockSizeY = (gbSizeY>>1);

	avdNativeInt coefA1, coefA2, coefB1, coefB2;
	avdInt16 *xClip;
	avdUInt8 *refStart = uv==0?(refPic->plnU):(refPic->plnV);
	int start;
	TIME_BEGIN(start)
#if DISABLE_MC
	return;
#endif
#if (DUMP_VERSION & DUMP_SLICE)
	//if(img->vdLibPar->rowNumOfMBs==1)
	//AvdLog(DUMP_SLICE,DUMP_DCORE "\nGBLC:%d:vec1(%d,%d),ref_poc=%d size(%d,%d)",GetCurPos(info,sizeInfo),xOffset, yOffset,refPic->poc, gbSizeX,gbSizeY);
#endif
#define ERR_OUTGETBLOCK2	 32
#if USE_FULL_MC
	if((img->vdLibPar->optFlag&OPT_FULLPIX_ALL)
		||((img->vdLibPar->optFlag&OPT_FULLPIX_NONREF)&&!img->dec_picture->used_for_reference))
	{
		x0 = y0 = 0;
	}
	else
#endif
	{
		x0     = (xOffset & 7);
		y0     = (yOffset & 7);
	}
	xOffset >>= 3; 
	yOffset >>= 3;
	width = (sizeInfo->yPlnPitch>>1);
	width3 = outLnLength - blockSizeX;

	if(xOffset<-ERR_OUTGETBLOCK2||
		yOffset<-ERR_OUTGETBLOCK2||
		xOffset>sizeInfo->width_cr+ERR_OUTGETBLOCK2||
		yOffset>sizeInfo->height_cr+ERR_OUTGETBLOCK2)
	{
		xOffset = Clip3(-ERR_OUTGETBLOCK2,sizeInfo->width_cr+ERR_OUTGETBLOCK2,xOffset);
		yOffset = Clip3(-ERR_OUTGETBLOCK2,sizeInfo->height_cr+ERR_OUTGETBLOCK2,yOffset);
		//error(img,"GetBlockChromaNxN",ERROR_InvalidMV);
	}



	if (xOffset >= 0 && xOffset + blockSizeX < sizeInfo->width_cr &&
		yOffset >= 0 && yOffset + blockSizeY < refPic->size_y_cr){
			// inside the bounday;
			ref0 = refStart + yOffset * width + xOffset;
			width2 = width - blockSizeX;
			if (x0 && y0){
#if MC_C_ASM 
				GetBlockChromaNxN_asm_case0(width, ref0, x0, y0, byteOutStart, width3, blockSizeX, blockSizeY);
				return;
#else//NEW_ARM_ASM
#if TEST_QCORE
				if (CHECK_THREAD_NEON)
				{
					GetBlockChromaNxN_asm_case0(width, ref0, x0, y0, byteOutStart, width3, blockSizeX, blockSizeY);
					return;
				}
#endif
				coefB2 = x0 * y0;
				coefA2 = (x0<<3) - coefB2; //x0 * (8 - y0j);
				coefB1 = (y0<<3) - coefB2; //(8 - x0) * y0;
				coefA1 = 64 - (coefB2 + coefB1 + coefA2); //(8 - x0) * (8 - y0);
				//50% cases;
				ref1 = ref0 + width;
				y0 = blockSizeY;
				do {
					x0 = blockSizeX;
					do {
						tmpa = (coefA1 * ref0[0] + coefA2 * ref0[1]
						+ coefB1 * ref1[0] + coefB2 * ref1[1] + 32)>>6;
						tmpb = (coefA1 * ref0[1] + coefA2 * ref0[2]
						+ coefB1 * ref1[1] + coefB2 * ref1[2] + 32)>>6;
						ref1 += 2;
						ref0 += 2;
						AVD_SAVE_TMPAB_TO_OUT();
					} while ((x0 -= 2) != 0);
					byteOutStart += width3;
					ref0 += width2;
					ref1 += width2;
				} while (--y0);
#endif//NEW_ARM_ASM
			}
			else if (!y0) {
#if USE_ARM_ASM_GBC
				GetBlockChromaNxN_asm_case1(width, ref0, x0, y0, byteOutStart, width3, blockSizeX, blockSizeY);
				return;
#else
				if (x0){
					// 20% cases; coefB1 = coefB2 = 0;
					y0 = blockSizeY;
					do {
						k = blockSizeX;
						do {
							tmpa = (((ref0[1] - ref0[0]) * x0 + 4)>>3) + ref0[0];
							tmpb = (((ref0[2] - ref0[1]) * x0 + 4)>>3) + ref0[1];
							ref0 += 2;
							AVD_SAVE_TMPAB_TO_OUT();
						} while ((k -= 2) != 0);
						byteOutStart += width3;
						ref0 += width2;
					} while (--y0);
				}
				else { //x0=y0=0;
					// 10% cases; coefA2 = coefB1 = coefB2 = 0;
					y0 = blockSizeY;
					do {
						x0 = blockSizeX;
						do {
							*byteOutStart++ = *ref0++;
							*byteOutStart++ = *ref0++;
						} while ((x0 -= 2) != 0);
						byteOutStart += width3;
						ref0 += width2;
					} while (--y0);
				}
#endif//NEW_ARM_ASM
			}
			else { //!x0
				// 20% cases; coefA2 = coefB2 = 0;
#if USE_ARM_ASM_GBC
				GetBlockChromaNxN_asm_case2(width, ref0, x0, y0, byteOutStart, width3, blockSizeX, blockSizeY);
				return;
#else//NEW_ARM_ASM
				ref1 = ref0 + width;
				k = blockSizeY;
				do {
					x0 = blockSizeX;
					do {
						tmpa = (((ref1[0] - ref0[0]) * y0 + 4)>>3) + ref0[0];
						tmpb = (((ref1[1] - ref0[1]) * y0 + 4)>>3) + ref0[1];
						ref0 += 2;
						ref1 += 2;
						AVD_SAVE_TMPAB_TO_OUT();
					} while ((x0 -= 2) != 0);
					byteOutStart += width3;
					ref0 += width2;
					ref1 += width2;
				} while (--k);
#endif//NEW_ARM_ASM
			}
			TIME_END(start,cmcSum)
			return;
	}
	TIME_BEGIN(start)
	coefB2 = x0 * y0;
	coefA2 = (x0<<3) - coefB2; //x0 * (8 - y0j);
	coefB1 = (y0<<3) - coefB2; //(8 - x0) * y0;
	coefA1 = 64 - (coefB2 + coefB1 + coefA2); //(8 - x0) * (8 - y0);
	xClip = &img->clipInfo->clipWidthCr[xOffset];
	clpHt += yOffset;
	k = blockSizeY;
	if((*clpHt)>sizeInfo->height_cr+8)
	{
		AVD_ERROR_CHECK2(img,"!!!(*clpHt)>sizeInfo->height_cr+8",ERROR_NULLPOINT);
	}
	do {
		ref0 = refStart + *clpHt++ * width;
		ref1 = refStart + *clpHt * width;
		x0 = blockSizeX;
		do {

			*byteOutStart++ = (coefA1 * ref0[*xClip] + coefA2 * ref0[*(xClip+1)]
			+ coefB1 * ref1[*xClip] + coefB2 * ref1[*(xClip+1)] + 32)>>6;
			xClip++;
			*byteOutStart++ = (coefA1 * ref0[*xClip] + coefA2 * ref0[*(xClip+1)]
			+ coefB1 * ref1[*xClip] + coefB2 * ref1[*(xClip+1)] + 32)>>6;
			xClip++;

		} while ((x0 -= 2) != 0);
		byteOutStart += width3;
		xClip -= blockSizeX;
	} while (--k);
	TIME_END(start,cmcSum2)
}

#if FEATURE_BFRAME
#if !USE_AVG_ASM
void SIAvgBlock_C(int blockSizeY, int blockSizeX,avdUInt8 * src, avdUInt8 *mpr, int Pitch)
{
	int offset1,offset2;
	offset1 = Pitch - blockSizeX;
	offset2 = 16 - blockSizeX;
	if(blockSizeX==16)
	{
		offset1 = Pitch - 12;
		offset2 = 4;
		do
		{
			avdUInt32* src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2 = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src += offset1;	mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2 = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src += offset1;	mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2 = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src += offset1;	mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2 = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src += offset1;	mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2 = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src += offset1;	mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2 = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src += offset1;	mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2 = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src += offset1;	mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2 = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src += offset1;	mpr += offset2;
			blockSizeY -=8;
		}while(blockSizeY>0);
	}
	else if(blockSizeX==8)
	{
		offset1 = Pitch - 4;
		offset2 = 12;
		do
		{
			avdUInt32* src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2 = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src += offset1;	mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2 = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src += offset1;	mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2 = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src += offset1;	mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src+=4;mpr+=4;
			*src2 = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src += offset1;	mpr += offset2;
			blockSizeY-=4;
		}while(blockSizeY>0);
	}
	else if(blockSizeX==4)
	{
		offset1 = Pitch;
		offset2 = 16;
		do
		{
			avdUInt32* src2 = (avdUInt32*)src;
			*src2 = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src += offset1;	mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2 = COMBINE_4_BYTES_TO32((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1,(mpr[2]+src[2]+1)>>1,(mpr[3]+src[3]+1)>>1);
			src += offset1;	mpr += offset2;
			blockSizeY-=2;
		}while(blockSizeY>0);
	}
	else//if(blockSizeX==2)
	{
		offset1 = Pitch;
		offset2 = 16;
		do
		{
			avdUInt16* src2 = (avdUInt32*)src;
			*src2 = COMBINE_2_BYTES_TO16((mpr[0]+src[0]+1)>>1,(mpr[1]+src[1]+1)>>1);
			src += offset1;	mpr += offset2;
		}while(--blockSizeY);
	}

}
#endif//USE_AVG_ASM

void SIChromaAvgBlock(ImageParameters *img,TMBsProcessor *info,avdNativeInt xIdx, avdNativeInt yIdx, 
					  avdUInt8 **imgUV2D, avdUInt8 *tmp_block,
					  int gbSizeX,int gbSizeY)
{
	TSizeInfo	*sizeInfo = img->sizeInfo;
	avdNativeInt blockSizeX = (gbSizeX>>1);
	avdNativeInt blockSizeY = (gbSizeY>>1);



	avdNativeInt uvPicth = (sizeInfo->yPlnPitch>>1);

	avdUInt8 *dst = *imgUV2D;
	avdNativeInt j, i;
	SIAvgBlock_C(blockSizeY, blockSizeX,dst, tmp_block,uvPicth);

}
#endif //FEATURE_BFRAME