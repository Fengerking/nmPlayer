#include "global.h"
#include "image.h"
#include "mmxGetBlock.h"
__int64 LastByteMaskQ  = 0x00000000000000ff;
__int64 FirstByteMaskQ = 0xff00000000000000;
__int64 LastDwMask     = 0x00000000ffffffff;

/*!
 ************************************************************************
 * 
 * GetBlock: Chroma 4x4
 *    
 ************************************************************************
 */
#define MacroBroadcastLastW(varInt64) _asm \
{										\
	_asm movq			mm0, varInt64 \
	_asm pshufw			mm0, mm0, 0 \
	_asm movq			varInt64, mm0 \
}

#define MacroGBChromaInit _asm \
{										\
	_asm MacroGBByteOutLoadInput \
	/* line 1 */  \
	_asm MacroGBLd1Q8Byte(mm6) \
	_asm add				eax, 4 /*next line*/ \
}

#define MacroGBChroma4x4ProcFirstLine _asm \
{										\
	_asm movq				mm4, mm6 \
	_asm punpcklbw			mm6, mm7 /*0-4*/ \
	_asm pmullw				mm6, coefA1 \
	_asm psrlq				mm4, 8 \
	_asm punpcklbw			mm4, mm7 /*1-5*/ \
	/* line 2 */  \
	_asm pmullw				mm4, coefA2 \
	_asm paddusw			mm6, mm4 \
	_asm MacroGBLd1Q8Byte(mm0) \
	_asm add				eax, 4 /*next line*/ \
}

#define MacroGBChroma4x4ProcOneLine _asm \
{										\
	_asm movq			mm1, mm0 \
	_asm punpcklbw		mm0, mm7 /*0-4*/ \
	_asm psrlq			mm1, 8 \
	_asm punpcklbw		mm1, mm7 /*1-5*/ \
	_asm movq			mm4, mm1 \
	_asm pmullw			mm1, coefB2 \
	_asm paddusw		mm1, mm6 \
	_asm movq			mm6, mm0 \
	_asm pmullw			mm0, coefB1 \
	_asm paddusw		mm1, mm0 \
	_asm paddusw		mm1, Mask32 \
	/* line 3 */ \
	_asm pmullw			mm6, coefA1 \
	_asm psraw			mm1, 6 \
	_asm packuswb		mm1, mm7 \
	_asm movd			[edi], mm1 \
	_asm pmullw			mm4, coefA2 \
	_asm add			edi, outLnLength \
	_asm paddusw		mm6, mm4 \
	_asm MacroGBLd1Q8Byte(mm0) \
	_asm add			eax, 4 /*next line*/ \
}

#define MacroGBChroma4x4ProcLastLine _asm \
{										\
	_asm movq			mm1, mm0 \
	_asm psrlq			mm1, 8 \
	_asm punpcklbw		mm1, mm7 /*1-5*/ \
	_asm pmullw			mm1, coefB2 \
	_asm punpcklbw		mm0, mm7 /*0-4*/ \
	_asm paddusw		mm1, mm6 \
	_asm pmullw			mm0, coefB1 \
	_asm paddusw		mm1, mm0 \
	_asm paddusw		mm1, Mask32 \
	_asm psraw			mm1, 6 \
	_asm packuswb		mm1, mm7 \
	_asm movd			[edi], mm1 \
}

#define MacroGBChroma4x4InBoundProcOneLine _asm \
{				\
	/* line 2 */  \
	_asm pmullw				mm6, mm3 \
	/* load data*/ \
	_asm movq				mm0, [eax] \
	_asm movq				mm1, mm0 \
	_asm pmullw				mm4, mm2 \
	_asm punpcklbw			mm0, mm7 /*0-4*/ \
	_asm psrlq				mm1, 8 \
	_asm paddusw			mm6, mm4 \
	_asm punpcklbw			mm1, mm7 /*1-5*/ \
	_asm movq				mm4, mm1 \
	_asm pmullw				mm1, coefB2 \
	_asm paddusw			mm1, mm6 \
	_asm movq				mm6, mm0 \
	_asm pmullw				mm0, mm5 \
	_asm add				eax, ecx \
	_asm paddusw			mm1, mm0 \
	_asm paddusw			mm1, Mask32 \
	_asm psraw				mm1, 6 \
	_asm packuswb			mm1, mm7 \
	_asm movd				[edi], mm1 \
	_asm add				edi, edx \
}

// xOffset = x_pos * 8 + mv_array[x];
void GetBlockChroma4x4(avdNativeInt xOffset, avdNativeInt yOffset, avdUInt8 *refStart[], 
					   avdUInt8 *byteOutStart, avdNativeInt outLnLength, avdNativeInt *clpHt)
{
	__int64 shL0, shR0;
	avdNativeInt x_pos, y_pos, i, j, k;
	__int64 coefA1, coefA2, coefB1, coefB2;
	x_pos  = (xOffset>>3); // xOffset could be negative;
	y_pos  = (yOffset>>3);
	i      = (xOffset & 7);
	j      = (yOffset & 7);
	k      = i * j;
	coefA1 = 64 - ((i+j)<<3) + k; //(8 - i) * (8 - j);
	coefA2 = (i<<3) - k; //i * (8 - j);
	coefB1 = (j<<3) - k; //(8 - i) * j;
	coefB2 =  k; //i * j;
	if (x_pos >= 0 && x_pos + 5 <= img->width_cr &&
		y_pos >= 0 && y_pos + 5 <= ((!mbIsMbAffField(img->currMB) 
		|| img->structure!=FRAME) ? 
		img->height_cr : (img->height_cr>>1))){
		// inside boundary;
		avdUInt8 *ref = &refStart[y_pos][x_pos];
		int width = img->width_cr;
		_asm {
			mov				eax, ref
			movq			mm6, [eax]
			mov				ecx, width
			add				eax, ecx
			mov				edi, byteOutStart
			mov				edx, outLnLength
			pxor			mm7, mm7
			// first line;
			MacroBroadcastLastW(coefB2)
			movq			mm4, mm6
			punpcklbw		mm6, mm7 /*0-4*/
			psrlq			mm4, 8
			punpcklbw		mm4, mm7
			/* broadcast coeff */ 
			movq			mm3, coefA1
			pshufw			mm3, mm3, 0
			movq			mm2, coefA2
			pshufw			mm2, mm2, 0
			movq			mm5, coefB1
			pshufw			mm5, mm5, 0

			MacroGBChroma4x4InBoundProcOneLine
			MacroGBChroma4x4InBoundProcOneLine
			MacroGBChroma4x4InBoundProcOneLine
			MacroGBChroma4x4InBoundProcOneLine
			emms
		}
		return;
	}

	_asm {
		MacroBroadcastLastW(coefA1)
		MacroBroadcastLastW(coefA2)
		MacroBroadcastLastW(coefB1)
		MacroBroadcastLastW(coefB2)
	}
	if (x_pos >= 0 && x_pos + 5 <= img->width_cr){
		// no padding;
		_asm {
			MacroGBChromaInit
			MacroGBChroma4x4ProcFirstLine
			MacroGBChroma4x4ProcOneLine /*line 1*/
			MacroGBChroma4x4ProcOneLine /*line 2*/
			MacroGBChroma4x4ProcOneLine /*line 3*/
			MacroGBChroma4x4ProcLastLine /*line 4*/
		}
	}
	else if (x_pos < 0){
		// right padding;
		shL0 = (x_pos > -8) ? ((-x_pos)<<3) : 64;
		shR0 = 64 - shL0;
		x_pos = 0;
		_asm {
			MacroGBChromaInit
			MacroGBRightPaddingQw(mm6, shR0, shL0)
			MacroGBChroma4x4ProcFirstLine
			MacroGBRightPaddingQw(mm0, shR0, shL0)
			MacroGBChroma4x4ProcOneLine /*line 1*/
			MacroGBRightPaddingQw(mm0, shR0, shL0)
			MacroGBChroma4x4ProcOneLine /*line 2*/
			MacroGBRightPaddingQw(mm0, shR0, shL0)
			MacroGBChroma4x4ProcOneLine /*line 3*/
			MacroGBRightPaddingQw(mm0, shR0, shL0)
			MacroGBChroma4x4ProcLastLine /*line 4*/
		}
	}
	else {
		// left padding;
		//adding unused 3 bytes, so that low 5 bytes are x_pos to x_pos+4 after shift;
		i = x_pos + 8 - img->width_cr;
		shR0 = (i < 8) ? (i<<3) : 64;
		shL0 = 64 - shR0;
		x_pos = img->width_cr - 8;
		_asm {
			MacroGBChromaInit
			MacroGBLeftPaddingQw(mm6, shR0, shL0)
			MacroGBChroma4x4ProcFirstLine
			MacroGBLeftPaddingQw(mm0, shR0, shL0)
			MacroGBChroma4x4ProcOneLine /*line 1*/
			MacroGBLeftPaddingQw(mm0, shR0, shL0)
			MacroGBChroma4x4ProcOneLine /*line 2*/
			MacroGBLeftPaddingQw(mm0, shR0, shL0)
			MacroGBChroma4x4ProcOneLine /*line 3*/
			MacroGBLeftPaddingQw(mm0, shR0, shL0)
			MacroGBChroma4x4ProcLastLine /*line 4*/
		}
	}
	_asm emms
}

/*!
 ************************************************************************
 * 
 * GetBlock: Chroma 2x2
 *    
 ************************************************************************
 */
// mmx code is not much faster in this case, so turn it off for now;
//#define AVD_CHROMA2x2_MMX
#ifndef AVD_CHROMA2x2_MMX
void GetBlockChroma2x2(avdNativeInt xOffset, avdNativeInt yOffset, avdUInt8 *refStart[], 
					   avdUInt8 *byteOutStart, avdNativeInt outLnLength, avdNativeInt *clpHt)
{
	avdNativeInt x0, x1, x2, y0, y1, y2;
	avdNativeInt coefA1, coefA2, coefB1, coefB2;
	x0     = (xOffset & 7);
	y0     = (yOffset & 7);
	coefA1 = (8 - x0) * (8 - y0);
	coefA2 =       x0 * (8 - y0);
	coefB1 = (8 - x0) * y0;
	coefB2 =       x0 * y0;
	xOffset >>= 3; // xOffset could be negative;
	yOffset >>= 3;
	x0 = img->clipWidthCr[xOffset];
	x1 = img->clipWidthCr[xOffset+1];
	x2 = img->clipWidthCr[xOffset+2];
	y0 = clpHt[yOffset];
	y1 = clpHt[yOffset+1];
	y2 = clpHt[yOffset+2];
	byteOutStart[0] = (coefA1 * refStart[y0][x0] + coefA2 * refStart[y0][x1]
		+  coefB1 * refStart[y1][x0] + coefB2 * refStart[y1][x1] + 32)>>6;
	byteOutStart[1] = (coefA1 * refStart[y0][x1] + coefA2 * refStart[y0][x2]
		+  coefB1 * refStart[y1][x1] + coefB2 * refStart[y1][x2] + 32)>>6;	
	byteOutStart += outLnLength;
	byteOutStart[0] = (coefA1 * refStart[y1][x0] + coefA2 * refStart[y1][x1]
		+  coefB1 * refStart[y2][x0] + coefB2 * refStart[y2][x1] + 32)>>6;
	byteOutStart[1] = (coefA1 * refStart[y1][x1] + coefA2 * refStart[y1][x2]
		+  coefB1 * refStart[y2][x1] + coefB2 * refStart[y2][x2] + 32)>>6;
}
#else AVD_CHROMA2x2_MMX
#define MacroGBChroma2x2Step1 _asm \
{										\
	_asm punpcklbw			mm6, mm7 /*0-4*/ \
	_asm movq				mm5, mm6 \
	_asm pand				mm5, LastDwMask /*0-1*/ \
	_asm psllq				mm6, 16 \
	_asm psrlq				mm6, 32 /*1-2*/ \
	/*second lines*/ \
	_asm MacroGBLd1Q8Byte(mm0) \
	_asm add				eax, 4 /*next line*/ \
}

#define MacroGBChroma2x2Step2 _asm \
{										\
	_asm punpcklbw			mm0, mm7 /*0-4*/ \
	_asm movq				mm1, mm0 \
	_asm psllq				mm0, 32 /*0-1 to high DW*/ \
	_asm por				mm0, mm5 /*0-1 of two lines*/ \
	_asm psrlq				mm1, 16 \
	_asm psllq				mm1, 32 /*1-2 to high DW*/ \
	_asm por				mm1, mm6 /*1-2 of two lines*/ \
	_asm movq				mm5, mm0 \
	_asm movq				mm6, mm1 \
	_asm pmullw				mm0, coefA1 \
	_asm pmullw				mm1, coefA2 \
	_asm paddusw			mm0, mm1 \
	_asm psrlq				mm5, 32 /*0-1 2nd line to low DW*/ \
	_asm psrlq				mm6, 32 /*1-2 2nd line to low DW*/ \
	/*third lines*/ \
	_asm MacroGBLd1Q8Byte(mm1) \
}

#define MacroGBChroma2x2Step3 _asm \
{										\
	_asm punpcklbw			mm1, mm7 /*0-4*/ \
	_asm movq				mm4, mm1 \
	_asm psllq				mm1, 32 /*0-1 to hight DW*/ \
	_asm por				mm5, mm1 /*0-1 of two lines*/ \
	_asm psrlq				mm4, 16 \
	_asm psllq				mm4, 32 /*1-2 to hight DW*/ \
	_asm por				mm6, mm4 /*1-2 of two lines*/ \
	_asm pmullw				mm5, coefB1 \
	_asm pmullw				mm6, coefB2 \
	_asm paddusw			mm0, mm5 \
	_asm paddusw			mm0, mm6 \
	_asm paddusw			mm0, Mask32 \
	_asm psraw				mm0, 6 \
	_asm packuswb			mm0, mm7 \
	_asm movd				eax, mm0 \
	_asm mov				[edi], ax \
	_asm shr				eax, 16 \
	_asm add				edi, outLnLength \
	_asm mov				[edi], ax \
}

void GetBlockChroma2x2(avdNativeInt xOffset, avdNativeInt yOffset, avdUInt8 *refStart[], 
					   avdUInt8 *byteOutStart, avdNativeInt outLnLength, avdNativeInt *clpHt)
{
	__int64 shL0, shR0;
	avdNativeInt x_pos, y_pos, i, j;
	__int64 coefA1, coefA2, coefB1, coefB2;
	x_pos  = (xOffset>>3); // xOffset could be negative;
	y_pos  = (yOffset>>3);
	i      = (xOffset & 7);
	j      = (yOffset & 7);
	coefA1 = (8 - i) * (8 - j);
	coefA2 =       i * (8 - j);
	coefB1 = (8 - i) * j;
	coefB2 =       i * j;
	_asm {
		MacroBroadcastLastW(coefA1)
		MacroBroadcastLastW(coefA2)
		MacroBroadcastLastW(coefB1)
		MacroBroadcastLastW(coefB2)
	}

	if (x_pos >= 0 && x_pos + 3 <= img->width_cr){
		// no padding;
		_asm {
			MacroGBChromaInit
			MacroGBChroma2x2Step1
			MacroGBChroma2x2Step2
			MacroGBChroma2x2Step3
		}
	}
	else if (x_pos < 0){
		// right padding;
		shL0 = (x_pos > -8) ? ((-x_pos)<<3) : 64;
		shR0 = 64 - shL0;
		x_pos = 0;
		_asm {
			MacroGBChromaInit
			MacroGBRightPaddingQw(mm6, shR0, shL0)
			MacroGBChroma2x2Step1
			MacroGBRightPaddingQw(mm0, shR0, shL0)
			MacroGBChroma2x2Step2 /*line 1*/
			MacroGBRightPaddingQw(mm1, shR0, shL0)
			MacroGBChroma2x2Step3 /*line 2*/
		}
	}
	else {
		// left padding;
		//adding unused 5 bytes, so that low 3 bytes are x_pos to x_pos+2 after shift;
		i = x_pos + 8 - img->width_cr;
		shR0 = (i < 8) ? (i<<3) : 64;
		shL0 = 64 - shR0;
		x_pos = img->width_cr - 8;
		_asm {
			MacroGBChromaInit
			MacroGBLeftPaddingQw(mm6, shR0, shL0)
			MacroGBChroma2x2Step1
			MacroGBLeftPaddingQw(mm0, shR0, shL0)
			MacroGBChroma2x2Step2 /*line 1*/
			MacroGBLeftPaddingQw(mm1, shR0, shL0)
			MacroGBChroma2x2Step3 /*line 2*/
		}
	}
	_asm emms
}

#endif AVD_CHROMA2x2_MMX


