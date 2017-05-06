
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
void GetBlockLumaNxN(ImageParameters *img,TMBsProcessor *info,avdNativeInt x_pos, avdNativeInt y_pos, StorablePicture* refPic, avdUInt8 *outStart,
					 avdNativeInt outLnLength,
					 int gbSizeX,int gbSizeY)
{
	avdInt16 *xClip, *yClip;
	avdUInt8 *clp255, *srcY;
	avdNativeInt *tmpResJ;
	avdUInt8 *refTmp;
	avdNativeInt dxP, dyP, i, j;
	avdInt32 tmpa, tmpb, tmpc, tmpd;
	avdNativeInt pres_x, pres_y, quarterSizeX; 
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
	avdNativeInt blockSizeX = gbSizeX;
	avdNativeInt blockSizeY = gbSizeY;
	TCLIPInfo	*clipInfo = img->clipInfo;
	avdInt32	 middle;
	avdNativeInt height, width, width1, width2, offset;
	avdUInt8* refStart = refPic->plnY;
	int start;
	TIME_BEGIN(start)
#if DISABLE_MC
	return;
#endif
#if USE_FULL_MC
	if( (img->vdLibPar->optFlag&OPT_FULLPIX_ALL)
		||((img->vdLibPar->optFlag&OPT_FULLPIX_NONREF)&&!img->dec_picture->used_for_reference)
		)
	{
		dxP = dyP = 0;
		//AvdLog2(LL_INFO,"@Interlace OPT_FULLPIX_NONREF\n");
	}
	else
#endif
	{
		dxP = x_pos&3;
		dyP = y_pos&3;
	}

#if (DUMP_VERSION & DUMP_SLICE)
	if(img->vdLibPar->rowNumOfMBs==1)
		AvdLog(DUMP_SLICE,DUMP_DCORE "\nGBL:%d:vec1(%d,%d),ref_poc=%d size(%d,%d)",GetCurPos(info,sizeInfo),x_pos, y_pos,gRefFrame->poc, blockSizeX,blockSizeY);
#endif
	x_pos >>= 2;
	y_pos >>= 2;
	clp255 = clipInfo->clip255;
	width  = sizeInfo->yPlnPitch;
	width1 = outLnLength - blockSizeX;
	width2  = width - blockSizeX;
	quarterSizeX = (blockSizeX>>2);

#define ERR_OUTGETBLOCK	 64

	if(!IS_VALID_POINTER(refStart))
		AVD_ERROR_CHECK2(img,"GetBlock:null refStart",ERROR_NULLPOINT);
	if(x_pos<-ERR_OUTGETBLOCK||y_pos<-ERR_OUTGETBLOCK||x_pos>sizeInfo->width+ERR_OUTGETBLOCK||y_pos>sizeInfo->height+ERR_OUTGETBLOCK)
	{
		x_pos = Clip3(-ERR_OUTGETBLOCK,sizeInfo->width+ERR_OUTGETBLOCK,x_pos);
		y_pos = Clip3(-ERR_OUTGETBLOCK,sizeInfo->height+ERR_OUTGETBLOCK,y_pos);
		AVD_ERROR_CHECK2(img,"GetBlockLumaNxN",ERROR_InvalidMV);
	}



	height = refPic->size_y;//sizeInfo->height;
	//sizeInfo->height : sizeInfo->height_cr;
	if (x_pos >= (dxP ? 2 : 0) && x_pos + (dxP ? blockSizeX + 2 : blockSizeX - 1) < sizeInfo->width 
		&& y_pos >= (dyP ? 2 : 0) && y_pos + (dyP ? blockSizeY + 2 : blockSizeY - 1) < height){


			srcY = (refStart + y_pos * width + x_pos);
#if MC_L_ASM 

			GetBlockLumaNxNInBound(srcY,outStart,outLnLength,dxP,dyP,info->mbsParser->m7,blockSizeX,blockSizeY,width,clp255);
			return;
#else
#if TEST_QCORE
			if (CHECK_THREAD_NEON)

			{
				GetBlockLumaNxNInBound(srcY,outStart,outLnLength,dxP,dyP,(avdInt32**)info->mbsParser->m7,blockSizeX,blockSizeY,width,clp255);
				return;
			}
#endif
			if (!dxP && !dyP) { //!dxP && !dyP (full pel);	
				j = blockSizeY;
				do {
					// blockSizeX could be from 4 to width;
					i = quarterSizeX;
					do {
						// srcY data may misaligned
						AVD_SAVE_4CHARS(outStart, srcY[0], srcY[1], srcY[2], srcY[3]);
						srcY += 4;
					} while (--i);
					srcY += width2;
					outStart += width1;
				} while (--j);
				EndRunTimeClock(DT_GETBLOCKLUMA);
				return;
			}

			refTmp = srcY;
			if (!dyP) { /* No vertical interpolation */
				if (dxP&1) 
				{
					refTmp = &srcY[dxP>>1];


					j = blockSizeY;
					do {
						i = quarterSizeX;
						do {
							middle = clp255[(AVD_6TAP(srcY[-2], srcY[-1], srcY[0], 
								srcY[1], srcY[2], srcY[3])+16)>>5];
							tmpa = (middle + refTmp[0] +1)>>1;
							middle = clp255[(AVD_6TAP(srcY[-1], srcY[0], srcY[1], 
								srcY[2], srcY[3], srcY[4])+16)>>5];
							tmpb = (middle + refTmp[1] +1)>>1;
							middle = clp255[(AVD_6TAP(srcY[0], srcY[1], srcY[2], 
								srcY[3], srcY[4], srcY[5])+16)>>5];
							tmpc = (middle + refTmp[2] +1)>>1;
							middle = clp255[(AVD_6TAP(srcY[1], srcY[2], srcY[3], 
								srcY[4], srcY[5], srcY[6])+16)>>5];
							tmpd = (middle + refTmp[3] +1)>>1;
							// use tmpa, tmpb, tmpc, tmpd to avoid alias;
							AVD_SAVE_4CHARS(outStart, tmpa, tmpb, tmpc, tmpd);
							srcY += 4;
							refTmp += 4;
						} while (--i);
						outStart += width1;
						srcY += width2;
						refTmp += width2;
					} while (--j);
				}
				else
				{

					j = blockSizeY;
					do {
						i = quarterSizeX;
						do {
							tmpa = clp255[(AVD_6TAP(srcY[-2], srcY[-1], srcY[0], 
								srcY[1], srcY[2], srcY[3])+16)>>5];
							tmpb = clp255[(AVD_6TAP(srcY[-1], srcY[0], srcY[1], 
								srcY[2], srcY[3], srcY[4])+16)>>5];
							tmpc = clp255[(AVD_6TAP(srcY[0], srcY[1], srcY[2], 
								srcY[3], srcY[4], srcY[5])+16)>>5];
							tmpd = clp255[(AVD_6TAP(srcY[1], srcY[2], srcY[3], 
								srcY[4], srcY[5], srcY[6])+16)>>5];
							AVD_SAVE_4CHARS(outStart, tmpa, tmpb, tmpc, tmpd);
							srcY += 4;						
						} while (--i);
						outStart += width1;
						srcY += width2;
					} while (--j);
				}
			}
			else if (!dxP) {  /* No horizontal interpolation */
				avdNativeInt quarterSizeY = (blockSizeY>>2);
				if (dyP&1) 
				{
					avdUInt8 *out;
					offset = (dyP>>1)*width;
					i = blockSizeX;
					do {
						out = outStart++;
						refTmp = srcY++;
						j = quarterSizeY;
						do {
							tmpa = clp255[(AVD_6TAP(refTmp[-(width<<1)], refTmp[-width],
								refTmp[0], refTmp[width], refTmp[width<<1],
								refTmp[3*width])+16)>>5];
							tmpb = clp255[(AVD_6TAP(refTmp[-width], refTmp[0],
								refTmp[width], refTmp[width<<1], refTmp[3*width],
								refTmp[width<<2])+16)>>5];
							tmpc = clp255[(AVD_6TAP(refTmp[0], refTmp[width],
								refTmp[width<<1], refTmp[3*width], refTmp[width<<2],
								refTmp[5*width])+16)>>5];
							tmpd = clp255[(AVD_6TAP(refTmp[width], refTmp[width<<1],
								refTmp[3*width], refTmp[width<<2], refTmp[5*width],
								refTmp[6*width])+16)>>5];

							out[0]        = ((tmpa + refTmp[offset] + 1)>>1);
							out[outLnLength]    = ((tmpb + refTmp[offset+width] + 1)>>1);
							out[outLnLength<<1] = ((tmpc + refTmp[offset+(width<<1)] + 1)>>1);
							out[3*outLnLength]  = ((tmpd + refTmp[offset+3*width] + 1)>>1);
							out    += (outLnLength<<2);
							refTmp += (width<<2);
						} while (--j);
					} while (--i);
				}
				else
				{
					avdUInt8 *out;
					i = blockSizeX;
					do {
						out = outStart++;
						refTmp = srcY++;
						j = quarterSizeY;
						do {
							tmpa = clp255[(AVD_6TAP(refTmp[-(width<<1)], refTmp[-width],
								refTmp[0], refTmp[width], refTmp[width<<1],
								refTmp[3*width])+16)>>5];
							tmpb = clp255[(AVD_6TAP(refTmp[-width], refTmp[0],
								refTmp[width], refTmp[width<<1], refTmp[3*width],
								refTmp[width<<2])+16)>>5];
							tmpc = clp255[(AVD_6TAP(refTmp[0], refTmp[width],
								refTmp[width<<1], refTmp[3*width], refTmp[width<<2],
								refTmp[5*width])+16)>>5];
							tmpd = clp255[(AVD_6TAP(refTmp[width], refTmp[width<<1],
								refTmp[3*width], refTmp[width<<2], refTmp[5*width],
								refTmp[6*width])+16)>>5];

							out[0]        = tmpa;
							out[outLnLength]    = tmpb;
							out[outLnLength<<1] = tmpc;
							out[3*outLnLength]  = tmpd;
							out    += (outLnLength<<2);
							refTmp += (width<<2);
						} while (--j);
					} while (--i);
				}
			}
			else if (dxP != 2 && dyP != 2) {  // Diagonal interpolation 
				avdNativeInt temp2;
				refTmp = srcY + (dyP == 1 ? 0 : width);
				if (dxP != 1)
					srcY++;
				j = blockSizeY;
				do {
					i = quarterSizeX;
					do {
						tmpa =  clp255[(AVD_6TAP(refTmp[-2], refTmp[-1], refTmp[0], 
							refTmp[1], refTmp[2], refTmp[3])+16)>>5];
						tmpb =  clp255[(AVD_6TAP(refTmp[-1], refTmp[0], refTmp[1], 
							refTmp[2], refTmp[3], refTmp[4])+16)>>5];
						tmpc =  clp255[(AVD_6TAP(refTmp[0], refTmp[1], refTmp[2], 
							refTmp[3], refTmp[4], refTmp[5])+16)>>5];
						tmpd =  clp255[(AVD_6TAP(refTmp[1], refTmp[2], refTmp[3], 
							refTmp[4], refTmp[5], refTmp[6])+16)>>5];

						temp2 = clp255[(AVD_6TAP(srcY[-(width<<1)], srcY[-width],
							srcY[0], srcY[width], srcY[width<<1],
							srcY[3*width])+16)>>5];
						tmpa = (tmpa +temp2 + 1)>>1;
						srcY++;
						temp2 = clp255[(AVD_6TAP(srcY[-(width<<1)], srcY[-width],
							srcY[0], srcY[width], srcY[width<<1],
							srcY[3*width])+16)>>5];
						tmpb = (tmpb +temp2 + 1)>>1;
						srcY++;
						temp2 = clp255[(AVD_6TAP(srcY[-(width<<1)], srcY[-width],
							srcY[0], srcY[width], srcY[width<<1],
							srcY[3*width])+16)>>5];
						tmpc = (tmpc +temp2 + 1)>>1;
						srcY++;
						temp2 = clp255[(AVD_6TAP(srcY[-(width<<1)], srcY[-width],
							srcY[0], srcY[width], srcY[width<<1],
							srcY[3*width])+16)>>5];
						tmpd = (tmpd +temp2 + 1)>>1;
						AVD_SAVE_4CHARS(outStart, tmpa, tmpb, tmpc, tmpd);
						srcY++;
						refTmp += 4;
					} while (--i);
					outStart += width1;
					refTmp += width2;
					srcY += width2;
				} while (--j);
			}
			else if (dxP == 2) {  // Vertical & horizontal interpolation 
				avdNativeInt *tmpM7 = info->mbsParser->m7[0];
				avdNativeInt quarterSizeY = (blockSizeY>>2);
				refTmp = srcY - (width<<1);
				j = blockSizeY + 5;
				do {
					tmpResJ = tmpM7;
					i = quarterSizeX;
					do {
						tmpa =  AVD_6TAP(refTmp[-2], refTmp[-1], refTmp[0], 
							refTmp[1], refTmp[2], refTmp[3]);
						tmpb =  AVD_6TAP(refTmp[-1], refTmp[0], refTmp[1], 
							refTmp[2], refTmp[3], refTmp[4]);
						tmpc =  AVD_6TAP(refTmp[0], refTmp[1], refTmp[2], 
							refTmp[3], refTmp[4], refTmp[5]);
						tmpd =  AVD_6TAP(refTmp[1], refTmp[2], refTmp[3], 
							refTmp[4], refTmp[5], refTmp[6]);

						*tmpResJ++ = tmpa;
						*tmpResJ++ = tmpb;
						*tmpResJ++ = tmpc;
						*tmpResJ++ = tmpd;
						refTmp += 4;					
					} while (--i);
					refTmp += width2;
					tmpM7  += M7_WIDTH; 
				} while (--j);
				if (dyP&1) 
				{
					avdUInt8 *out;
					tmpM7 = info->mbsParser->m7[0];
					i = blockSizeX;
					do {
						tmpResJ = tmpM7++;
						out     = outStart++;
						pres_y  = 2 + (dyP>>1);
						j = quarterSizeY;
						do {
							tmpa = clp255[(AVD_6TAP(tmpResJ[0], tmpResJ[M7_WIDTH], 
								tmpResJ[2*M7_WIDTH], tmpResJ[3*M7_WIDTH], tmpResJ[4*M7_WIDTH], 
								tmpResJ[5*M7_WIDTH]) + 512)>>10];
							tmpb = clp255[(AVD_6TAP(tmpResJ[M7_WIDTH], tmpResJ[2*M7_WIDTH], 
								tmpResJ[3*M7_WIDTH], tmpResJ[4*M7_WIDTH], tmpResJ[5*M7_WIDTH], 
								tmpResJ[6*M7_WIDTH]) + 512)>>10];
							tmpc = clp255[(AVD_6TAP(tmpResJ[2*M7_WIDTH], tmpResJ[3*M7_WIDTH], 
								tmpResJ[4*M7_WIDTH], tmpResJ[5*M7_WIDTH], tmpResJ[6*M7_WIDTH], 
								tmpResJ[7*M7_WIDTH]) + 512)>>10];
							tmpd = clp255[(AVD_6TAP(tmpResJ[3*M7_WIDTH], tmpResJ[4*M7_WIDTH], 
								tmpResJ[5*M7_WIDTH], tmpResJ[6*M7_WIDTH], tmpResJ[7*M7_WIDTH], 
								tmpResJ[8*M7_WIDTH]) + 512)>>10];
							out[0]              = ((tmpa + clp255[(tmpResJ[pres_y * M7_WIDTH]+16)>>5] + 1)>>1);
							out[outLnLength]    = ((tmpb + clp255[(tmpResJ[(pres_y+1) * M7_WIDTH]+16)>>5] + 1)>>1);
							out[outLnLength<<1] = ((tmpc + clp255[(tmpResJ[(pres_y+2) * M7_WIDTH]+16)>>5] + 1)>>1);
							out[3*outLnLength]  = ((tmpd + clp255[(tmpResJ[(pres_y+3) * M7_WIDTH]+16)>>5] + 1)>>1);
							out += (outLnLength<<2);
							tmpResJ += 4*M7_WIDTH;
						} while (--j);
					} while (--i);
				}
				else
				{
					avdUInt8 *out;
					avdNativeInt *tmpM7 = info->mbsParser->m7[0];
					i = blockSizeX;
					do {
						tmpResJ = tmpM7++;
						out     = outStart++;
						j = quarterSizeY;
						do {
							tmpa = clp255[(AVD_6TAP(tmpResJ[0], tmpResJ[M7_WIDTH], 
								tmpResJ[2*M7_WIDTH], tmpResJ[3*M7_WIDTH], tmpResJ[4*M7_WIDTH], 
								tmpResJ[5*M7_WIDTH]) + 512)>>10];
							tmpb = clp255[(AVD_6TAP(tmpResJ[M7_WIDTH], tmpResJ[2*M7_WIDTH], 
								tmpResJ[3*M7_WIDTH], tmpResJ[4*M7_WIDTH], tmpResJ[5*M7_WIDTH], 
								tmpResJ[6*M7_WIDTH]) + 512)>>10];
							tmpc = clp255[(AVD_6TAP(tmpResJ[2*M7_WIDTH], tmpResJ[3*M7_WIDTH], 
								tmpResJ[4*M7_WIDTH], tmpResJ[5*M7_WIDTH], tmpResJ[6*M7_WIDTH], 
								tmpResJ[7*M7_WIDTH]) + 512)>>10];
							tmpd = clp255[(AVD_6TAP(tmpResJ[3*M7_WIDTH], tmpResJ[4*M7_WIDTH], 
								tmpResJ[5*M7_WIDTH], tmpResJ[6*M7_WIDTH], tmpResJ[7*M7_WIDTH], 
								tmpResJ[8*M7_WIDTH]) + 512)>>10];
							out[0]              = tmpa;
							out[outLnLength]    = tmpb;
							out[outLnLength<<1] = tmpc;
							out[3*outLnLength]  = tmpd;
							out += (outLnLength<<2);
							tmpResJ += 4*M7_WIDTH;
						} while (--j);
					} while (--i);
				}
			}
			else {  /* Horizontal & vertical interpolation */
				avdNativeInt *tmpM7 = info->mbsParser->m7[0];
				avdNativeInt quarterSizeY = (blockSizeY>>2);
				srcY -= 2;
				i = blockSizeX + 5;
				do {
					tmpResJ = tmpM7++;
					refTmp  = srcY++;
					j = quarterSizeY;
					do {
						tmpa = AVD_6TAP(refTmp[-(width<<1)], refTmp[-width],
							refTmp[0], refTmp[width], refTmp[width<<1],
							refTmp[3*width]);
						tmpb = AVD_6TAP(refTmp[-width], refTmp[0],
							refTmp[width], refTmp[width<<1], refTmp[3*width],
							refTmp[width<<2]);
						tmpc = AVD_6TAP(refTmp[0], refTmp[width],
							refTmp[width<<1], refTmp[3*width], refTmp[width<<2],
							refTmp[5*width]);
						tmpd = AVD_6TAP(refTmp[width], refTmp[width<<1],
							refTmp[3*width], refTmp[width<<2], refTmp[5*width],
							refTmp[6*width]);

						tmpResJ[0]           = tmpa;
						tmpResJ[M7_WIDTH]    = tmpb;
						tmpResJ[M7_WIDTH<<1] = tmpc;
						tmpResJ[3*M7_WIDTH]  = tmpd;
						tmpResJ += (M7_WIDTH<<2);
						refTmp  += (width<<2);
					} while (--j);
				} while (--i);

				width1 = outLnLength - blockSizeX;
				width2 = M7_WIDTH  - blockSizeX;
				tmpResJ = info->mbsParser->m7[0];
				pres_x = 2+(dxP>>1);
				j = blockSizeY;
				do {
					i = quarterSizeX;
					do {
						tmpa = ((clp255[(AVD_6TAP(tmpResJ[0], tmpResJ[1], tmpResJ[2], 
							tmpResJ[3], tmpResJ[4], tmpResJ[5]) + 512)>>10]
						+ clp255[(tmpResJ[pres_x]+16)>>5] + 1)>>1);
						tmpb = ((clp255[(AVD_6TAP(tmpResJ[1], tmpResJ[2], tmpResJ[3], 
							tmpResJ[4], tmpResJ[5], tmpResJ[6]) + 512)>>10]
						+ clp255[(tmpResJ[pres_x+1]+16)>>5] + 1)>>1);
						tmpc = ((clp255[(AVD_6TAP(tmpResJ[2], tmpResJ[3], tmpResJ[4], 
							tmpResJ[5], tmpResJ[6], tmpResJ[7]) + 512)>>10]
						+ clp255[(tmpResJ[pres_x+2]+16)>>5] + 1)>>1);
						tmpd = ((clp255[(AVD_6TAP(tmpResJ[3], tmpResJ[4], tmpResJ[5], 
							tmpResJ[6], tmpResJ[7], tmpResJ[8]) + 512)>>10]
						+ clp255[(tmpResJ[pres_x+3]+16)>>5] + 1)>>1);
						AVD_SAVE_4CHARS(outStart, tmpa, tmpb, tmpc, tmpd);					
						tmpResJ += 4;
					} while (--i);
					outStart += width1;
					tmpResJ += width2;				
				} while (--j);
			}
			TIME_END(start,lmcSum)
			return;
#endif //USE_ARMV5E_ASM

			
	}
	else
	{
		TIME_BEGIN(start)
		GetBlockLumaOutBound(img,info,refStart, outStart,outLnLength,dxP,dyP,x_pos,y_pos,blockSizeY,blockSizeX,
			height,width,width2,width1,quarterSizeX);
		TIME_END(start,lmcSum2)
	}
}
#if FEATURE_BFRAME
void SILumaAvgBlock(ImageParameters *img,TMBsProcessor *info,avdNativeInt xIdx, avdNativeInt yIdx, avdUInt8 *tmp_block,
					int gbSizeX,int gbSizeY)
{
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
	avdNativeInt blockSizeX = gbSizeX;
	avdNativeInt blockSizeY = gbSizeY;


	avdNativeInt i, j;

	StorablePicture	*dec_picture = img->dec_picture;
	avdNativeInt yPitch = sizeInfo->yPlnPitch;
	avdUInt8 *dst = dec_picture->plnY + yIdx * yPitch + xIdx;
	SIAvgBlock_C(blockSizeY, blockSizeX,dst, tmp_block, yPitch);

}

#endif//FEATURE_BFRAME