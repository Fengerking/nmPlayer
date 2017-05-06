//#include <string.h>
#include "defines.h"
#include "global.h"
#include "image.h"
//#include <assert.h>
#ifdef WMMX

	#define IPP_INTERPOLATION
	#ifdef IPP_INTERPOLATION
		#include "ippVC.h"
	#endif//IPP_INTERPOLATION

#ifdef NEW_YUV_MEMORY
#if !(AVD_PLATFORM & AVD_TI_C55x) 
	#define		AVD_6TAP(m2, m1, p0, p1, p2, p3) (m2+p3-5*(m1+p2)+20*(p0+p1)) 
#else //!(AVD_PLATFORM & AVD_TI_C55x) 
	// need to force it to 32 bits to avoid 16 bits int overflow;
	#define		AVD_6TAP(m2, m1, p0, p1, p2, p3) ((avdInt32)m2+(avdInt32)p3-5*((avdInt32)m1+(avdInt32)p2)+20*((avdInt32)p0+(avdInt32)p1)) 
#endif //!(AVD_PLATFORM & AVD_TI_C55x) 

#define  AVD_SAVE_TMPABCD_TO_OUT() \
			*outStart++ = tmpa; \
			*outStart++ = tmpb; \
			*outStart++ = tmpc; \
			*outStart++ = tmpd;

#define M7_WIDTH		24
void GetBlockLumaNxN(avdNativeInt x_pos, avdNativeInt y_pos, avdUInt8 *refStart[], avdUInt8 *outStart, avdNativeInt outLnLength)
{
	avdInt16 *xClip, *yClip;
	avdUInt8 *clp255, *srcY;
	avdNativeInt *tmpResJ;
	avdUInt8 *refTmp;
	avdNativeInt dxP, dyP, i, j;
	avdInt32 tmpa, tmpb, tmpc, tmpd;
	avdNativeInt pres_x, pres_y; 
#ifdef NEW_GET_BLOCK
	avdNativeInt blockSizeX = img->gbSizeX;
	avdNativeInt blockSizeY = img->gbSizeY;
#else
	avdNativeInt blockSizeX = img->getBlockSize;
	avdNativeInt blockSizeY = img->getBlockSize;
#endif
	avdInt32	 middle;
	avdNativeInt height, width, width1, width2, offset;
	StartRunTimeClock(DT_GETBLOCKLUMA);
	dxP = x_pos&3;
	dyP = y_pos&3;
	x_pos >>= 2;
	y_pos >>= 2;
	clp255 = img->clip255;
	width  = img->yPlnPitch;
	width1 = outLnLength - blockSizeX;
	width2  = width - blockSizeX;
#define ERR_OUTGETBLOCK	 64
#if ERROR_CHECK
	if((int)refStart<0x0fff)
		error("(int)refStart<1024",ERROR_InvalidBitstream);
	//if(x_pos<-ERR_OUTGETBLOCK||y_pos<-ERR_OUTGETBLOCK||x_pos>img->width+ERR_OUTGETBLOCK||y_pos>img->height+ERR_OUTGETBLOCK)
	//	error("GetBlockLumaNxN",110);
	x_pos = Clip3(-16,img->width+16,x_pos);
	y_pos = Clip3(-16,img->height+16,y_pos);
	
#endif
#if !Y_PADDING // no padding
	height = (!mbIsMbAffField(img->currMB) || img->structure!=FRAME) ? 
			img->height : img->height_cr;
	if (x_pos >= (dxP ? 2 : 0) && x_pos + (dxP ? blockSizeX + 2 : blockSizeX - 1) < img->width 
		&& y_pos >= (dyP ? 2 : 0) && y_pos + (dyP ? blockSizeY + 2 : blockSizeY - 1) < height){
#endif // !Y_PADDING

		srcY = (*refStart + y_pos * width + x_pos);
#ifdef IPP_INTERPOLATION
		{
			
			//if(img->number==1&&img->current_mb_nr==1) 
						

			j = blockSizeX>>4;
			if(j==0)
			{
				IppiSize  roi;//={blockSizeX,blockSizeY};
				roi.width 	= blockSizeX;
				roi.height 	= blockSizeY;
				
				 
				i = ippiInterpolateLuma_H264_8u_C1R(srcY ,width, outStart, outLnLength, dxP, dyP,roi);
			}
			else
			{
				do {
					IppiSize  roi;//={16,blockSizeY};
					roi.width 	= 16;
					roi.height 	= blockSizeY;
					i = ippiInterpolateLuma_H264_8u_C1R(srcY ,width, outStart, outLnLength, dxP, dyP,roi);
					srcY+=16;
					outStart+=16;
				} while(--j);
			}
			
				
				
			EndRunTimeClock(DT_GETBLOCKLUMA);	
			return;
			
		}
#else
		if (!dxP && !dyP) { //!dxP && !dyP (full pel);
			
			j = blockSizeY;
			do {
				i = blockSizeX;
				do {
					// srcY data may misaligned
					*outStart++ = *srcY++;
					*outStart++ = *srcY++;
					*outStart++ = *srcY++;
					*outStart++ = *srcY++;
				} while ((i -= 4) != 0);
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
					i = blockSizeX;
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
						AVD_SAVE_TMPABCD_TO_OUT();
						srcY += 4;
						refTmp += 4;
					} while ((i -= 4) != 0);
					outStart += width1;
					srcY += width2;
					refTmp += width2;
				} while (--j);
			}
			else
			{
				j = blockSizeY;
				do {
					i = blockSizeX;
					do {
						tmpa = clp255[(AVD_6TAP(srcY[-2], srcY[-1], srcY[0], 
							srcY[1], srcY[2], srcY[3])+16)>>5];
						tmpb = clp255[(AVD_6TAP(srcY[-1], srcY[0], srcY[1], 
							srcY[2], srcY[3], srcY[4])+16)>>5];
						tmpc = clp255[(AVD_6TAP(srcY[0], srcY[1], srcY[2], 
							srcY[3], srcY[4], srcY[5])+16)>>5];
						tmpd = clp255[(AVD_6TAP(srcY[1], srcY[2], srcY[3], 
							srcY[4], srcY[5], srcY[6])+16)>>5];
						AVD_SAVE_TMPABCD_TO_OUT();
						srcY += 4;						
					} while ((i -= 4) != 0);
					outStart += width1;
					srcY += width2;
				} while (--j);
			}
		}
		else if (!dxP) {  /* No horizontal interpolation */
		
			if (dyP&1) 
			{
				avdUInt8 *out;
				offset = (dyP>>1)*width;
				i = blockSizeX;
				do {
					out = outStart++;
					refTmp = srcY++;
					j = blockSizeY;
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
					} while ((j -= 4) != 0);
				} while (--i);
			}
			else
			{
				avdUInt8 *out;
				i = blockSizeX;
				do {
					out = outStart++;
					refTmp = srcY++;
					j = blockSizeY;
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
					} while ((j -= 4) != 0);
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
					i = blockSizeX;
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
					AVD_SAVE_TMPABCD_TO_OUT();
					srcY++;
					refTmp += 4;
				} while ((i -= 4) != 0);
				outStart += width1;
				refTmp += width2;
				srcY += width2;
			} while (--j);
		}
		else if (dxP == 2) {  // Vertical & horizontal interpolation 
			avdNativeInt *tmpM7 = img->m7[0];
			
			refTmp = srcY - (width<<1);
			j = blockSizeY + 5;
			do {
				tmpResJ = tmpM7;
				i = blockSizeX;
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
				} while ((i -= 4) != 0);
				refTmp += width2;
				tmpM7  += M7_WIDTH; 
			} while (--j);
			if (dyP&1) 
			{
				avdUInt8 *out;
				avdNativeInt *tmpM7 = img->m7[0];
				i = blockSizeX;
				do {
					tmpResJ = tmpM7++;
					out     = outStart++;
					pres_y  = 2 + (dyP>>1);
					j = blockSizeY;
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
					} while ((j -= 4) != 0);
				} while (--i);
			}
			else
			{
				avdUInt8 *out;
				avdNativeInt *tmpM7 = img->m7[0];
				i = blockSizeX;
				do {
					tmpResJ = tmpM7++;
					out     = outStart++;
					j = blockSizeY;
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
					} while ((j -= 4) != 0);
				} while (--i);
			}
		}
		else {  /* Horizontal & vertical interpolation */
			avdNativeInt *tmpM7 = img->m7[0];
		
			srcY -= 2;
			i = blockSizeX + 5;
			do {
				tmpResJ = tmpM7++;
				refTmp  = srcY++;
				j = blockSizeY;
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
				} while ((j -= 4) != 0);
			} while (--i);

			width1 = outLnLength - blockSizeX;
			width2 = M7_WIDTH  - blockSizeX;
			tmpResJ = img->m7[0];
			pres_x = 2+(dxP>>1);
			j = blockSizeY;
			do {
				i = blockSizeX;
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
					AVD_SAVE_TMPABCD_TO_OUT();					
					tmpResJ += 4;
				} while ((i -= 4) != 0);
				outStart += width1;
				tmpResJ += width2;				
			} while (--j);
		}
		EndRunTimeClock(DT_GETBLOCKLUMA);
		return;
#endif //USE_ARMV5E_ASM
#if !Y_PADDING // no padding

	}

	xClip = &img->clipWidth[x_pos];
	yClip  = (height == img->height) ? img->clipHeight : img->clipHeightCr;
	yClip = &yClip[y_pos];
	if (!dxP && !dyP) {  /* fullpel position */
		j = blockSizeY;
		do {
			refTmp = (*refStart + *yClip++ * width);
			i = blockSizeX;
			do {
				*outStart++ = refTmp[*xClip++];
				*outStart++ = refTmp[*xClip++];
				*outStart++ = refTmp[*xClip++];
				*outStart++ = refTmp[*xClip++];
			} while ((i -= 4) != 0);
			outStart += width1;
			xClip -= blockSizeX;
		} while (--j);
		EndRunTimeClock(DT_GETBLOCKLUMA);
		return;
	}

	if (!dyP) { /* No vertical interpolation */
		if (dxP&1) {
			pres_x = (dxP>>1);
			j = blockSizeY;
			do {
				refTmp = (*refStart + *yClip++ * width);
				i = blockSizeX;
				do {
					*outStart++ = ((clp255[(AVD_6TAP(refTmp[xClip[-2]], refTmp[xClip[-1]],
						refTmp[xClip[0]], refTmp[xClip[1]], refTmp[xClip[2]],
						refTmp[xClip[3]]) + 16)>>5] + refTmp[xClip[pres_x]] + 1)>>1);
					xClip++;
				} while (--i);
				outStart += width1;
				xClip -= blockSizeX;
			} while (--j);
		}
		else {
			j = blockSizeY;
			do {
				refTmp = (*refStart + *yClip++ * width);
				i = blockSizeX;
				do {
					*outStart++ = clp255[(AVD_6TAP(refTmp[xClip[-2]], refTmp[xClip[-1]],
						refTmp[xClip[0]], refTmp[xClip[1]], refTmp[xClip[2]],
						refTmp[xClip[3]]) + 16)>>5];
					xClip++;
				} while (--i);
				outStart += width1;
				xClip -= blockSizeX;
			} while (--j);
		}

	}
	else if (!dxP) {  /* No horizontal interpolation */
		refTmp = *refStart;
		if (dyP&1) {
			pres_y = (dyP>>1);
			j = blockSizeY;
			do {
				i = blockSizeX;
				do {
					pres_x = *xClip++;
					*outStart++ = ((clp255[(AVD_6TAP(refTmp[yClip[-2] * width + pres_x], 
						refTmp[yClip[-1] * width + pres_x], 
						refTmp[yClip[0] * width + pres_x], 
						refTmp[yClip[1] * width + pres_x], 
						refTmp[yClip[2] * width + pres_x], 
						refTmp[yClip[3] * width + pres_x]) + 16)>>5] + 
						refTmp[yClip[pres_y] * width + pres_x] + 1)>>1);
				} while (--i);
				outStart += width1;
				xClip -= blockSizeX;
				yClip++;
			} while (--j);
		}
		else{
			pres_y = (dyP>>1);
			j = blockSizeY;
			do {
				i = blockSizeX;
				do {
					pres_x = *xClip++;
					*outStart++ = clp255[(AVD_6TAP(refTmp[yClip[-2] * width + pres_x], 
						refTmp[yClip[-1] * width + pres_x], 
						refTmp[yClip[0] * width + pres_x], 
						refTmp[yClip[1] * width + pres_x], 
						refTmp[yClip[2] * width + pres_x], 
						refTmp[yClip[3] * width + pres_x]) + 16)>>5];
				} while (--i);
				outStart += width1;
				xClip -= blockSizeX;
				yClip++;
			} while (--j);
		}
	}
	else if (dxP == 2) {  /* Vertical & horizontal interpolation */
		yClip += blockSizeY + 2;
		xClip += blockSizeX - 1;
		j = blockSizeY + 4;
		do {
			refTmp = *refStart + *yClip-- * width;
			tmpResJ = img->m7[j];
			i = blockSizeX - 1;
			do {
				tmpResJ[i] = AVD_6TAP(refTmp[xClip[-2]], refTmp[xClip[-1]], 
					refTmp[xClip[0]], refTmp[xClip[1]], refTmp[xClip[2]], 
					refTmp[xClip[3]]);
				xClip--;
			} while (--i >= 0);
			xClip += blockSizeX;
		} while (--j >= 0);

		if (dyP&1) 
		{
			avdUInt8 *out;
			avdNativeInt *tmpM7 = img->m7[0];
			i = blockSizeX;
			do {
				tmpResJ = tmpM7++;
				out     = outStart++;
				pres_y  = 2 + (dyP>>1);
				j = blockSizeY;
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
				} while ((j -= 4) != 0);
			} while (--i);
		}
		else
		{
			avdUInt8 *out;
			avdNativeInt *tmpM7 = img->m7[0];
			i = blockSizeX;
			do {
				tmpResJ = tmpM7++;
				out     = outStart++;
				j = blockSizeY;
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
				} while ((j -= 4) != 0);
			} while (--i);
		}
	}
	else if (dyP == 2) {  /* Horizontal & vertical interpolation */
		yClip += blockSizeY - 1;
		xClip += blockSizeX + 2;
		tmpa = blockSizeX + 5;
		refTmp = *refStart;
		j = blockSizeY - 1;
		do {
			tmpResJ = img->m7[j];
			i = blockSizeX + 4;
			do {
				pres_x = *xClip--;
				tmpResJ[i] = AVD_6TAP(refTmp[yClip[-2] * width + pres_x], 
					refTmp[yClip[-1] * width + pres_x], 
					refTmp[yClip[0] * width + pres_x], 
					refTmp[yClip[1] * width + pres_x], 
					refTmp[yClip[2] * width + pres_x], 
					refTmp[yClip[3] * width + pres_x]);
			} while (--i >= 0);
			yClip--;
			xClip += tmpa;
		} while (--j >= 0);

		width1 = outLnLength - blockSizeX;
		width2 = M7_WIDTH  - blockSizeX;
		tmpResJ = img->m7[0];
		pres_x = 2+(dxP>>1);
		j = blockSizeY;
		do {
			i = blockSizeX;
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
				AVD_SAVE_TMPABCD_TO_OUT();					
				tmpResJ += 4;
			} while ((i -= 4) != 0);
			outStart += width1;
			tmpResJ += width2;				
		} while (--j);
	}
	else {  /* Diagonal interpolation */
		avdInt16 *yClipTmp = (dyP == 1) ? yClip : yClip + 1;
		avdUInt8 *out = outStart;
		j = blockSizeY;
		do {
			refTmp = *refStart + *yClipTmp++ * width;
			i = blockSizeX;
			do {
				*out++ = clp255[(AVD_6TAP(refTmp[xClip[-2]], refTmp[xClip[-1]], 
					refTmp[xClip[0]], refTmp[xClip[1]], refTmp[xClip[2]], 
					refTmp[xClip[3]]) + 16)>>5];
				xClip++;
			} while (--i);
			xClip -= blockSizeX;
			out += width1;
		} while (--j);

		if (dxP != 1)
			xClip++; 
		refTmp = *refStart;
		j = blockSizeY;
		do {
			i = blockSizeX;
			do {
				pres_x = *xClip++;
				*outStart++ = (*outStart + clp255[(AVD_6TAP(
					refTmp[yClip[-2] * width + pres_x], 
					refTmp[yClip[-1] * width + pres_x], 
					refTmp[yClip[0] * width + pres_x], 
					refTmp[yClip[1] * width + pres_x], 
					refTmp[yClip[2] * width + pres_x], 
					refTmp[yClip[3] * width + pres_x]) + 16)>>5] + 1)>>1;
			} while (--i);
			yClip++;
			xClip -= blockSizeX;
			outStart += width1;
		} while (--j);
	}
#endif //!Y_PADDING 
	EndRunTimeClock(DT_GETBLOCKLUMA);
}

#define  AVD_SAVE_TMPAB_TO_OUT() \
			*byteOutStart++ = tmpa; \
			*byteOutStart++ = tmpb;

void GetBlockChromaNxN(avdNativeInt xOffset, avdNativeInt yOffset, avdUInt8 *refStart[], 
					   avdUInt8 *byteOutStart, avdNativeInt outLnLength, avdInt16 *clpHt)
{
	avdUInt8 *ref0, *ref1;
	avdNativeInt x0, y0, tmpa, tmpb;
	avdNativeInt k, width, width2, width3,i,j;
#ifdef NEW_GET_BLOCK
	avdNativeInt blockSizeX = (img->gbSizeX>>1);
	avdNativeInt blockSizeY = (img->gbSizeY>>1);
#else
	avdNativeInt blockSizeX = (img->getBlockSize>>1);
	avdNativeInt blockSizeY = (img->getBlockSize>>1);
#endif
	avdNativeInt coefA1, coefA2, coefB1, coefB2;
	avdInt16 *xClip;

	StartRunTimeClock(DT_GETBLOCKCHROMA);	
	x0     = (xOffset & 7);
	y0     = (yOffset & 7);
	xOffset >>= 3; 
	yOffset >>= 3;
	width = (img->yPlnPitch>>1);
	width3 = outLnLength - blockSizeX;
#if ERROR_CHECK
	if((int)refStart<0x0fff)
		error("(int)refStart<1024",ERROR_InvalidBitstream);
	xOffset  = Clip3(-8,img->width_cr+8,xOffset);
	yOffset  = Clip3(-8,img->height_cr+8,yOffset);
	
#endif
#if !Y_PADDING 
	if (xOffset >= 0 && xOffset + blockSizeX < img->width_cr &&
		yOffset >= 0 && yOffset + blockSizeY < ((!mbIsMbAffField(img->currMB) 
		|| img->structure!=FRAME) ? 
		img->height_cr : (img->height_cr>>1))){
#endif //!Y_PADDING 
		//// inside the bounday;
		ref0 = *refStart + yOffset * width + xOffset;
#ifdef IPP_INTERPOLATION
		{
			
			j = blockSizeX>>3;
			if(j==0)
			{
				IppiSize  roi;//={blockSizeX,blockSizeY};
				roi.width 	= blockSizeX;
				roi.height 	= blockSizeY;
				i = ippiInterpolateChroma_H264_8u_C1R(ref0,width, byteOutStart, outLnLength, x0, y0,roi);
			}
			else
			{
				do {
					IppiSize  roi;//={8,blockSizeY};
					roi.width 	= 8;
					roi.height 	= blockSizeY;
					i = ippiInterpolateChroma_H264_8u_C1R(ref0,width, byteOutStart, outLnLength, x0, y0,roi);
					ref0+=8;
					byteOutStart+=8;
				} while(--j);
			}
				
				
				
			EndRunTimeClock(DT_GETBLOCKCHROMA);	
			return;
			
		}
#else//USE_ARMV5E_ASM

		width2 = width - blockSizeX;
		if (x0 && y0){
		
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
		}
		else if (!y0) {
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
		}
		else { //!x0
			// 20% cases; coefA2 = coefB2 = 0;
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
		}

#endif//USE_ARMV5E_ASM
		EndRunTimeClock(DT_GETBLOCKCHROMA);	
		return;
#if !Y_PADDING 
	}

	coefB2 = x0 * y0;
	coefA2 = (x0<<3) - coefB2; //x0 * (8 - y0j);
	coefB1 = (y0<<3) - coefB2; //(8 - x0) * y0;
	coefA1 = 64 - (coefB2 + coefB1 + coefA2); //(8 - x0) * (8 - y0);
	xClip = &img->clipWidthCr[xOffset];
	clpHt += yOffset;
	k = blockSizeY;
	do {
		ref0 = *refStart + *clpHt++ * width;
		ref1 = *refStart + *clpHt * width;
		x0 = blockSizeX;
		do {
#if 1
			*byteOutStart++ = (coefA1 * ref0[*xClip] + coefA2 * ref0[*(xClip+1)]
				+ coefB1 * ref1[*xClip] + coefB2 * ref1[*(xClip+1)] + 32)>>6;
			xClip++;
			*byteOutStart++ = (coefA1 * ref0[*xClip] + coefA2 * ref0[*(xClip+1)]
				+ coefB1 * ref1[*xClip] + coefB2 * ref1[*(xClip+1)] + 32)>>6;
			xClip++;
#else
			// intel compiler generate more code, when using tmpa and tmpb, since
			// alias only save two loads;
			tmpa = (coefA1 * ref0[*xClip] + coefA2 * ref0[*(xClip+1)]
				+ coefB1 * ref1[*xClip] + coefB2 * ref1[*(xClip+1)] + 32)>>6;
			tmpb = (coefA1 * ref0[*(xClip+1)] + coefA2 * ref0[*(xClip+2)]
				+ coefB1 * ref1[*(xClip+1)] + coefB2 * ref1[*(xClip+2)] + 32)>>6;
			AVD_SAVE_TMPAB_TO_OUT();
			xClip += 2;
#endif
		} while ((x0 -= 2) != 0);
		byteOutStart += width3;
		xClip -= blockSizeX;
	} while (--k);
	EndRunTimeClock(DT_GETBLOCKCHROMA);	
#endif //!Y_PADDING 
}

#else
#endif //NEW_YUV_MEMORY


#endif//WMMX










					
