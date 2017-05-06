#include <string.h>
#include "global.h"
#include "image.h"
#include "macroblock.h"



void GetBlockLumaOutBound(ImageParameters *img,TMBsProcessor *info,avdUInt8* refStart, avdUInt8 *outStart,avdNativeInt outLnLength,
						  int dxP,int dyP,int x_pos,int y_pos,int blockSizeY,int blockSizeX,
						  avdNativeInt height,avdNativeInt width,avdNativeInt width2,avdNativeInt  width1,avdNativeInt quarterSizeX)
{

	//avdUInt8 *srcY;
	avdNativeInt *tmpResJ;
	avdNativeInt i,j,pres_x,pres_y;
	avdUInt8 *refTmp,*clp255;
	avdInt32 tmpa, tmpb, tmpc, tmpd;
	avdInt16 *xClip, *yClip;
	
	TCLIPInfo	*clipInfo = img->clipInfo;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	clp255 = clipInfo->clip255;
	xClip = &clipInfo->clipWidth[x_pos];
	yClip  = (height == sizeInfo->height) ? clipInfo->clipHeight : clipInfo->clipHeightCr;
	yClip = &yClip[y_pos];
	if (!dxP && !dyP) {  /* fullpel position */
		j = blockSizeY;
		do {
			refTmp = (refStart + *yClip++ * width);
			i = quarterSizeX;
			do {
				AVD_SAVE_4CHARS(outStart, refTmp[xClip[0]], refTmp[xClip[1]], 
					refTmp[xClip[2]], refTmp[xClip[3]]);
				xClip += 4;
			} while (--i);
			outStart += width1;
			xClip -= blockSizeX;
		} while (--j);
		return;
	}

	if (!dyP) { /* No vertical interpolation */
		if (dxP&1) {
			pres_x = (dxP>>1);
			j = blockSizeY;
			do {
				refTmp = (refStart + *yClip++ * width);
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
				refTmp = (refStart + *yClip++ * width);
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
		refTmp = refStart;
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
			refTmp = refStart + *yClip-- * width;
			tmpResJ = info->mbsParser->m7[j];
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
			avdNativeInt *tmpM7 = info->mbsParser->m7[0];
			i = blockSizeX;
			do {
				tmpResJ = tmpM7++;
				out     = outStart++;
				pres_y  = 2 + (dyP>>1);
				j = (blockSizeY>>2);
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
				j = (blockSizeY>>2);
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
	else if (dyP == 2) {  /* Horizontal & vertical interpolation */
		yClip += blockSizeY - 1;
		xClip += blockSizeX + 2;
		tmpa = blockSizeX + 5;
		refTmp = refStart;
		j = blockSizeY - 1;
		do {
			tmpResJ = info->mbsParser->m7[j];
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
		tmpResJ = info->mbsParser->m7[0];
		pres_x = 2+(dxP>>1);
		j = blockSizeY;
		do {
			i = (blockSizeX>>2);
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
	else {  /* Diagonal interpolation */
		avdInt16 *yClipTmp = (dyP == 1) ? yClip : yClip + 1;
		avdUInt8 *out = outStart;
		j = blockSizeY;
		do {
			refTmp = refStart + *yClipTmp++ * width;
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
		refTmp = refStart;
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

}









