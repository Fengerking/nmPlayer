#include <memory.h>
#include "global.h"
#include "image.h"
#include "mmxGetBlock.h"

static void GetBlock8x8_OutOfBound(avdNativeInt x_pos, avdNativeInt y_pos, avdUInt8 *refStart[], avdUInt8 *outStart, 
							avdNativeInt outLnLength);

void GetBlock8x8(avdNativeInt x_pos, avdNativeInt y_pos, avdUInt8 *refStart[], avdUInt8 *outStart, avdNativeInt outLnLength)
{
	avdNativeInt x = (x_pos>>2);
	avdNativeInt y = (y_pos>>2);
	avdNativeInt dxP = x_pos&3;
	avdNativeInt dyP = y_pos&3;
	avdNativeInt height = (!mbIsMbAffField(img->currMB) || img->structure!=FRAME) ? 
		img->height : img->height_cr;
	if (x >= (dxP ? 2 : 0) && x + (dxP ? 10 : 7) < img->width 
		&& y >= (dyP ? 2 : 0) && y + (dyP ? 10 : 7) < height)
		GetBlock8x8_InBound(x_pos, y_pos, refStart, outStart, outLnLength);
	else
		GetBlock8x8_OutOfBound(x_pos, y_pos, refStart, outStart, outLnLength);

	_asm emms
}

static void GetBlock8x8_OutOfBound(avdNativeInt x_pos, avdNativeInt y_pos, avdUInt8 *refStart[], avdUInt8 *outStart, 
							avdNativeInt outLnLength)
{
	avdUInt8 *clp255;
	avdInt16 *tmpResJ;
	avdUInt8 *refTmp, *out;
	avdInt16 *xClip, *yClip;
	avdNativeInt pres_x;
	avdNativeInt dxP, dyP, i, j;
	avdInt32	 middle;

	dxP = x_pos&3;
	dyP = y_pos&3;
	clp255 = img->clip255;
	xClip = &img->clipWidth[x_pos>>2];
	yClip = (!mbIsMbAffField(img->currMB) || img->structure!=FRAME) ? 
		img->clipHeight : img->clipHeightCr;
	yClip = &yClip[y_pos>>2];

	if (!dxP && !dyP) { //!dxP && !dyP (full pel);
		for (j = 0; j < 8; j++){
			refTmp = refStart[yClip[j]];
			for (i = 0; i < 8; i++)
				outStart[i] = refTmp[xClip[i]];
			outStart += outLnLength;
		}
		return;
	}

	/* non-fullpel position */
	out = outStart;
	if (!dyP) { /* No vertical interpolation */
		for (j = 0; j < 8; j++) {
			refTmp = refStart[yClip[j]];
			for (i = 0; i < 8; i++) {
				middle = refTmp[xClip[i-1]] + refTmp[xClip[i+2]] -
					((refTmp[xClip[i]] + refTmp[xClip[i+1]])<<2);
				out[i] = clp255[(refTmp[xClip[i-2]] + refTmp[xClip[i+3]] -
					middle - (middle<<2) + 16)>>5];
			}
			out += outLnLength;
		}

		if (dxP&1) {
			pres_x = (dxP>>1);
			for (j = 0; j < 8; j++){
				refTmp = refStart[yClip[j]];
				for (i = 0; i < 8; i++)
					outStart[i] = (outStart[i] + refTmp[xClip[pres_x+i]] + 1)>>1;
				outStart += outLnLength;
			}
		}
	}
	else if (!dxP) {  /* No horizontal interpolation */
		for (j = 0; j < 8; j++) {
			for (i = 0; i < 8; i++) {
				pres_x = xClip[i];
				middle = refStart[yClip[j-1]][pres_x] + refStart[yClip[j+2]][pres_x] -
					((refStart[yClip[j]][pres_x] + refStart[yClip[j+1]][pres_x])<<2);
				out[i] = clp255[(refStart[yClip[j-2]][pres_x] +
					refStart[yClip[j+3]][pres_x] - middle - (middle<<2) + 16)>>5];
			}
			out += outLnLength;
		}

		if (dyP&1) {
			for (j = 0; j < 8; j++){
				refTmp = refStart[yClip[j + (dyP>>1)]];
				for (i = 0; i < 8; i++)
					outStart[i] = (outStart[i] + refTmp[xClip[i]] + 1)>>1;
				outStart += outLnLength;
			}
		}
	}
	else if (dxP == 2) {  /* Vertical & horizontal interpolation */
		// borrow int img->m7[16][16] for tmp_res[13][8], use as a short pointer for faster MMX store and load;
		avdInt16 *tmp_res = (avdInt16 *)&img->m7[0][0];
		for (j = -2; j < 11; j++) {
			refTmp = refStart[yClip[j]];
			tmpResJ = &tmp_res[(j+2)<<3]; /*8 shorts a row;*/
			for (i = 0; i < 8; i++){
				middle = refTmp[xClip[i-1]] + refTmp[xClip[i+2]] -
					((refTmp[xClip[i]] + refTmp[xClip[i+1]])<<2);
				tmpResJ[i] = refTmp[xClip[i-2]] +
					refTmp[xClip[i+3]] - middle - (middle<<2);
			}
		}
		for (j = 0; j < 8; j++) {
			for (i = 0; i < 8; i++) {
				middle = tmp_res[((j+1)<<3)+i] + tmp_res[((j+4)<<3)+i] -
					((tmp_res[((j+2)<<3)+i] + tmp_res[((j+3)<<3)+i])<<2);
				out[i] = clp255[(tmp_res[(j<<3)+i] + tmp_res[((j+5)<<3)+i] -
					middle - (middle<<2) + 512)>>10];
			} 
			out += outLnLength;
		}
		if (dyP&1) {
			tmpResJ = &tmp_res[(2 + (dyP>>1))<<3];
			_asm{
				mov				eax, tmpResJ
				mov				ecx, 4
				mov				edx, outStart
				mov				edi, outLnLength
GB8x8OutOfBoundLnStartB:
				movq			mm2, [edx]
				movq			mm0, [eax] //0-3
				movq			mm1, [eax + 8] //4-7
				paddsw			mm0, Mask16
				paddsw			mm1, Mask16
				psraw			mm0, 5
				psraw			mm1, 5
				packuswb		mm0, mm1
				pavgb			mm2, mm0
				movq			[edx], mm2
				
				movq			mm2, [edx + edi]
				movq			mm1, [eax + 24] //4-7
				movq			mm0, [eax + 16] //0-3
				paddsw			mm0, Mask16
				paddsw			mm1, Mask16
				psraw			mm0, 5
				psraw			mm1, 5
				packuswb		mm0, mm1
				pavgb			mm2, mm0
				movq			[edx + edi], mm2

				add				eax, 32
				lea				edx, [edx + 2*edi]
				sub				ecx, 1
				jnz				GB8x8OutOfBoundLnStartB
			}
		}
	}
	else if (dyP == 2) {  /* Horizontal & vertical interpolation */
		// borrow int img->m7[16][16] for tmp_res[8][16], use as a short pointer for faster MMX store and load;
		avdInt16 *tmp_res = (avdInt16 *)&img->m7[0][0];
		avdInt16 *resPt;
		for (j = 0; j < 8; j++) {
			resPt = &tmp_res[j<<4];
			for (i = -2; i < 11; i++){
				pres_x = xClip[i];
				middle = refStart[yClip[j-1]][pres_x] + refStart[yClip[j+2]][pres_x] -
					((refStart[yClip[j]][pres_x] + refStart[yClip[j+1]][pres_x])<<2);
				resPt[i+2] = refStart[yClip[j-2]][pres_x] +
					refStart[yClip[j+3]][pres_x] - middle - (middle<<2);
			}
		}

		for (j = 0; j < 8; j++) {
			resPt = &tmp_res[j<<4];
			for (i = 0; i < 8; i++) {
				middle = resPt[i+4] + resPt[i+1] - ((resPt[i+2] + resPt[i+3])<<2);
				out[i] = clp255[(resPt[i] + resPt[i+5] -
					middle - (middle<<2) + 512)>>10];
			}
			out += outLnLength;
		}

		if (dxP&1) {
			// TBD: tmp_res[2+(dxP>>1)] is never 8 bytes aligned;
			// need a better way to do this;
			resPt = &tmp_res[2+(dxP>>1)];
			_asm{
				mov				eax, resPt
				mov				ecx, 4
				mov				edx, outStart
				mov				edi, outLnLength
GB8x8OutOfBoundLnStartA:
				movq			mm2, [edx]
				movq			mm0, [eax] //0-1
				movq			mm1, [eax + 8] //4-5
				paddsw			mm0, Mask16
				paddsw			mm1, Mask16
				psraw			mm0, 5
				psraw			mm1, 5
				packuswb		mm0, mm1
				pavgb			mm2, mm0
				movq			[edx], mm2
				
				movq			mm2, [edx + edi]
				movq			mm0, [eax + 32] //0-1
				movq			mm1, [eax + 40] //4-5
				paddsw			mm0, Mask16
				paddsw			mm1, Mask16
				psraw			mm0, 5
				psraw			mm1, 5
				packuswb		mm0, mm1
				pavgb			mm2, mm0
				movq			[edx + edi], mm2

				lea				edx, [edx + 2*edi]
				add				eax, 64
				sub				ecx, 1
				jnz				GB8x8OutOfBoundLnStartA
			}
		}
/*
		if (dxP&1) {
			pres_x = 2+(dxP>>1);
			for (j = 0; j < 8; j++){
				resPt = img->m7[j];
				for (i = 0; i < 8; i++)
					outStart[i] = (outStart[i] + clp255[(resPt[pres_x+i]+16)>>5] + 1)>>1;
				outStart += outLnLength;
			}
		}
*/
	}
	else {  /* Diagonal interpolation */
		for (j = 0; j < 8; j++) {
			refTmp =  refStart[yClip[dyP == 1 ? j : j+1]];
			for (i = 0; i < 8; i++) {
				middle = refTmp[xClip[i-1]] + refTmp[xClip[i+2]] -
					((refTmp[xClip[i]] + refTmp[xClip[i+1]])<<2);
				out[i] = clp255[(refTmp[xClip[i-2]] + refTmp[xClip[i+3]] -
					middle - (middle<<2) + 16)>>5];
			}
			out += outLnLength;
		}

		for (j = 0; j < 8; j++) {
			for (i = 0; i < 8; i++) {
				pres_x = xClip[(dxP == 1 ? i : i+1)];
				middle = refStart[yClip[j-1]][pres_x] + refStart[yClip[j+2]][pres_x] -
					((refStart[yClip[j]][pres_x] + refStart[yClip[j+1]][pres_x])<<2);
				outStart[i] = (outStart[i] + clp255[( refStart[yClip[j-2]][pres_x] +
					refStart[yClip[j+3]][pres_x] - middle - (middle<<2) + 16)>>5] + 1)>>1;
			}
			outStart += outLnLength;
		}
	}
}	

void GetBlock4x4(avdNativeInt x_pos, avdNativeInt y_pos, avdUInt8 *refStart[], avdUInt8 *outStart, avdNativeInt outLnLength)
{
	avdInt16 *xClip, *yClip;
	avdUInt8 *clp255;
	avdUInt8 *refTmp, *out;
	avdNativeInt pres_x; 
	avdInt16 *tmpResJ;
	avdNativeInt dxP, dyP, i, j, tmp;
	avdNativeInt numLn = 4;
	avdInt32	 middle;

	dxP = x_pos&3;
	dyP = y_pos&3;
	clp255 = img->clip255;
	xClip = &img->clipWidth[x_pos>>2];
	yClip = (!mbIsMbAffField(img->currMB) || img->structure!=FRAME) ? 
		img->clipHeight : img->clipHeightCr;
	yClip = &yClip[y_pos>>2];

	if (!dxP && !dyP) {  /* fullpel position */
		for (j = 0; j < BLOCK_SIZE; j++){
			refTmp = refStart[yClip[j]];
			for (i = 0; i < BLOCK_SIZE; i++)
				outStart[i] = refTmp[xClip[i]];
			outStart += outLnLength;
		}
		return;
	}

	out = outStart;
	if (!dyP) { /* No vertical interpolation */
		for (j = 0; j < BLOCK_SIZE; j++) {
			refTmp = refStart[yClip[j]];
			for (i = 0; i < BLOCK_SIZE; i++) {
				middle = refTmp[xClip[i-1]] + refTmp[xClip[i+2]] -
					((refTmp[xClip[i]] + refTmp[xClip[i+1]])<<2);
				out[i] = clp255[(refTmp[xClip[i-2]] + refTmp[xClip[i+3]] -
					middle - (middle<<2) + 16)>>5];
			}
			out += outLnLength;
		}

		if (dxP&1) {
			pres_x = (dxP>>1);
			for (j = 0; j < BLOCK_SIZE; j++){
				refTmp = refStart[yClip[j]];
				for (i = 0; i < BLOCK_SIZE; i++)
					outStart[i] = (outStart[i] + refTmp[xClip[pres_x+i]] + 1)>>1;

				outStart += outLnLength;
			}
		}
	}
	else if (!dxP) {  /* No horizontal interpolation */
		for (j = 0; j < BLOCK_SIZE; j++) {
			for (i = 0; i < BLOCK_SIZE; i++) {
				pres_x = xClip[i];
				middle = refStart[yClip[j-1]][pres_x] +
					refStart[yClip[j+2]][pres_x] - ((refStart[yClip[j]][pres_x] +
					refStart[yClip[j+1]][pres_x])<<2);
				out[i] = clp255[(refStart[yClip[j-2]][pres_x] +
					refStart[yClip[j+3]][pres_x] - middle - (middle<<2) + 16)>>5];
			}
			out += outLnLength;
		}

		if (dyP&1) {
			for (j = 0; j < BLOCK_SIZE; j++){
				refTmp = refStart[yClip[j + (dyP>>1)]];
				for (i = 0; i < BLOCK_SIZE; i++)
					outStart[i] = (outStart[i] + refTmp[xClip[i]] + 1)>>1;

				outStart += outLnLength;
			}
		}
	}
	else if (dxP == 2) {  /* Vertical & horizontal interpolation */
		// borrow int img->m7[16][16] for tmp_res[9][4], use as a short pointer for faster MMX store and load;
		avdInt16 *tmp_res = (avdInt16 *)&img->m7[0][0];
		tmp = BLOCK_SIZE + 3;
		for (j = -2; j < tmp; j++) {
			refTmp = refStart[yClip[j]];
			tmpResJ = &tmp_res[(j+2)<<2];
			for (i = 0; i < BLOCK_SIZE; i++){
				middle = refTmp[xClip[i-1]] + refTmp[xClip[i+2]] -
					((refTmp[xClip[i]] + refTmp[xClip[i+1]])<<2);
				tmpResJ[i] = refTmp[xClip[i-2]] +
					refTmp[xClip[i+3]] - middle - (middle<<2);
			}
		}

		for (j = 0; j < BLOCK_SIZE; j++) {
			for (i = 0; i < BLOCK_SIZE; i++) {
				middle = tmp_res[((j+1)<<2)+i] + tmp_res[((j+4)<<2)+i] -
					((tmp_res[((j+2)<<2)+i] + tmp_res[((j+3)<<2)+i])<<2);
				out[i] = img->clip255[(tmp_res[(j<<2)+i] + tmp_res[((j+5)<<2)+i] -
					middle - (middle<<2) + 512)>>10];
			} 
			out += outLnLength;
		}
		if (dyP&1) {
			tmpResJ = &tmp_res[(2 + (dyP>>1))<<2];
			_asm{
				mov				eax, tmpResJ
				mov				edx, outStart
				mov				edi, outLnLength
				pxor			mm1, mm1
				// 1st line;
				movd			mm2, [edx]
				movq			mm0, [eax] //0-3
				paddsw			mm0, Mask16
				psraw			mm0, 5
				packuswb		mm0, mm1
				pavgb			mm2, mm0
				movd			[edx], mm2
				// 2nd line;
				movd			mm2, [edx + edi]
				movq			mm0, [eax + 8] //0-3
				paddsw			mm0, Mask16
				psraw			mm0, 5
				packuswb		mm0, mm1
				pavgb			mm2, mm0
				movd			[edx + edi], mm2
				// 3rd line;
				movd			mm2, [edx + 2*edi]
				movq			mm0, [eax + 16] //0-3
				paddsw			mm0, Mask16
				psraw			mm0, 5
				packuswb		mm0, mm1
				pavgb			mm2, mm0
				movd			[edx + 2*edi], mm2
				// 4th line;
				lea				edx, [edx + 2*edi]
				movd			mm2, [edx + edi]
				movq			mm0, [eax + 24] //0-3
				paddsw			mm0, Mask16
				psraw			mm0, 5
				packuswb		mm0, mm1
				pavgb			mm2, mm0
				movd			[edx + edi], mm2
			}
		}
	}
	else if (dyP == 2) {  /* Horizontal & vertical interpolation */
		// borrow int img->m7[16][16] for tmp_res[4][16], use as a short pointer for faster MMX store and load;
		avdInt16 *tmp_res = (avdInt16 *)&img->m7[0][0];
		tmp = BLOCK_SIZE + 3;
		for (j = 0; j < BLOCK_SIZE; j++) {
			tmpResJ = &tmp_res[j<<4];
			for (i = -2; i < tmp; i++){
				pres_x = xClip[i];
				middle = refStart[yClip[j-1]][pres_x] + refStart[yClip[j+2]][pres_x] -
					((refStart[yClip[j]][pres_x] + refStart[yClip[j+1]][pres_x])<<2);
				tmpResJ[i+2] = refStart[yClip[j-2]][pres_x] +
					refStart[yClip[j+3]][pres_x] - middle - (middle<<2);
			}
		}
		for (j = 0; j < BLOCK_SIZE; j++) {
			tmpResJ = &tmp_res[j<<4];
			for (i = 0; i < BLOCK_SIZE; i++) {
				middle = tmpResJ[i+4] + tmpResJ[i+1] - ((tmpResJ[i+2] + tmpResJ[i+3])<<2);
				out[i] = img->clip255[(tmpResJ[i] + tmpResJ[i+5] -
					middle - (middle<<2) + 512)>>10];
			}
			out += outLnLength;
		}

		if (dxP&1) {
			tmpResJ = &tmp_res[2+(dxP>>1)];
			// TBD: tmp_res[2+(dxP>>1)] is never 8 bytes aligned;
			// need a better way to do this;
			_asm{
				mov				eax, tmpResJ
				mov				edx, outStart
				mov				edi, outLnLength
				pxor			mm1, mm1
				// 1st line;
				movd			mm2, [edx]
				movq			mm0, [eax] //0-3
				paddsw			mm0, Mask16
				psraw			mm0, 5
				packuswb		mm0, mm1
				pavgb			mm2, mm0
				movd			[edx], mm2
				// 2nd line;
				movd			mm2, [edx + edi]
				movq			mm0, [eax + 32] //0-3
				paddsw			mm0, Mask16
				psraw			mm0, 5
				packuswb		mm0, mm1
				pavgb			mm2, mm0
				movd			[edx + edi], mm2
				// 3rd line;
				movd			mm2, [edx + 2*edi]
				movq			mm0, [eax + 64] //0-3
				paddsw			mm0, Mask16
				psraw			mm0, 5
				packuswb		mm0, mm1
				pavgb			mm2, mm0
				movd			[edx + 2*edi], mm2
				// 4th line;
				lea				edx, [edx + 2*edi] 
				movd			mm2, [edx + edi]
				movq			mm0, [eax + 96] //0-3
				paddsw			mm0, Mask16
				psraw			mm0, 5
				packuswb		mm0, mm1
				pavgb			mm2, mm0
				movd			[edx + edi], mm2
			}
		}
	}
	else {  /* Diagonal interpolation */
		for (j = 0; j < BLOCK_SIZE; j++) {
			refTmp = refStart[yClip[dyP == 1 ? j : j+1]];
			for (i = 0; i < BLOCK_SIZE; i++) {
				middle = refTmp[xClip[i-1]] + refTmp[xClip[i+2]] -
					((refTmp[xClip[i]] + refTmp[xClip[i+1]])<<2);
				out[i] = clp255[(refTmp[xClip[i-2]] +
					refTmp[xClip[i+3]] - middle - (middle<<2) + 16)>>5];
			}
			out += outLnLength;
		}

		for (j = 0; j < BLOCK_SIZE; j++) {
			for (i = 0; i < BLOCK_SIZE; i++) {
				pres_x = xClip[dxP == 1 ? i : i+1];
				middle = refStart[yClip[j-1]][pres_x] + refStart[yClip[j+2]][pres_x] -
					((refStart[yClip[j]][pres_x] + refStart[yClip[j+1]][pres_x])<<2);
				outStart[i] = (outStart[i] + clp255[(refStart[yClip[j-2]][pres_x] +
					refStart[yClip[j+3]][pres_x] - middle - (middle<<2) + 16)>>5] + 1)>>1;
			}
			outStart += outLnLength;
		}
	}
	_asm	emms
}

// The MMX code above is from the old code, and only use P3 MMX and do up to 8x8 block;
// For now, this is a back up version for P3;
// TBD: should do 16x16 block and use P4 MMX;
void GetBlockLumaNxN(avdNativeInt x_pos, avdNativeInt y_pos, avdUInt8 *refStart[], avdUInt8 *outStart, avdNativeInt outLnLength)
{
	avdNativeInt i, j;
#ifdef NEW_GET_BLOCK
	avdNativeInt blockSizeX = img->gbSizeX;
	avdNativeInt blockSizeY = img->gbSizeY;
#else
	avdNativeInt blockSizeX = img->getBlockSize;
	avdNativeInt blockSizeY = img->getBlockSize;
#endif
	StartRunTimeClock(DT_GETBLOCKLUMA);	
	if (blockSizeY > 4){
		for (j = 0; j < blockSizeY; j += 8){
			for (i = 0; i < blockSizeX; i += 8){
				GetBlock8x8(x_pos + (i<<2), y_pos + (j<<2), refStart, outStart + i, 
					outLnLength);
			}
			outStart += (outLnLength<<3);
		}
	}
	else
		GetBlock4x4(x_pos, y_pos, refStart, outStart, outLnLength);

	EndRunTimeClock(DT_GETBLOCKLUMA);	
}

void GetBlockChromaNxN(avdNativeInt xOffset, avdNativeInt yOffset, avdUInt8 *refStart[], 
					   avdUInt8 *byteOutStart, avdNativeInt outLnLength, avdInt16 *clpHt)
{
	avdNativeInt i, j;
#ifdef NEW_GET_BLOCK
	avdNativeInt blockSizeX = (img->gbSizeX>>1);
	avdNativeInt blockSizeY = (img->gbSizeY>>1);
#else
	avdNativeInt blockSizeX = (img->getBlockSize>>1);
	avdNativeInt blockSizeY = (img->getBlockSize>>1);
#endif
	StartRunTimeClock(DT_GETBLOCKCHROMA);	
	if (blockSizeY > 2){
		for (j = 0; j < blockSizeY; j += 4){
			for (i = 0; i < blockSizeX; i += 4){
				GetBlockChroma4x4(xOffset + (i<<3), yOffset + (j<<3), refStart, 
					byteOutStart + i, outLnLength, clpHt);
			}
			byteOutStart += (outLnLength<<2);
		}
	}
	else 
		GetBlockChroma2x2(xOffset, yOffset, refStart, byteOutStart, outLnLength, clpHt);
	EndRunTimeClock(DT_GETBLOCKCHROMA);	
}









