
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/

/*!
************************************************************************
*
* \file		mmxGetBlock.h
*
* \date
*		March 19, 2004
*
************************************************************************
*/

#ifndef _MMXGETBLOCK_H_
#define _MMXGETBLOCK_H_

// common variable;
extern __int64 Mask16;
extern __int64 Mask32;
extern __int64 Mask512;
extern __int64 LastByteMaskQ;
extern __int64 FirstByteMaskQ;
extern __int64 LastDwMask;

// function;
void GetBlock8x8_InBound(avdNativeInt x_pos, avdNativeInt y_pos, avdUInt8 *refStart[], avdUInt8 *outStart, avdNativeInt outLnLength);

// common macro definition;
#define MacroGBLoadInput _asm \
{								\
	_asm mov			ecx, clpHt \
	_asm mov			eax, y_pos \
	_asm lea			eax, [ecx + 4*eax] /*&clpHt[y_pos]*/ \
	_asm mov			esi, refStart /*mref[ref_frame]*/ \
}

#define MacroGBCalcRef _asm \
{								\
	_asm mov			ecx, [eax] /*clpHt[y_pos]*/ \
	_asm mov			edx, [esi + 4*ecx] /*mref[ref_frame][clpHt[y_pos]]*/ \
	_asm add			edx, x_pos /*mref[ref_frame][clpHt[y_pos]] + x_pos*/ \
}

#define MacroGBByteOutLoadInput _asm \
{								\
	_asm MacroGBLoadInput \
	_asm mov			edi, byteOutStart \
	_asm pxor			mm7, mm7 \
}

#define MacroGBLd1Q8Byte(mmReg) _asm \
{								\
	_asm MacroGBCalcRef \
	_asm movq			mmReg, [edx] /*mref[ref_frame][clpHt[y_pos]][x_pos]*/ \
}

#define MacroGBRightPaddingQw(mmReg, shR, shL) _asm \
{												\
	/*left side padding*/ \
	_asm movq			mm3, mmReg \
	_asm pand			mm3, LastByteMaskQ \
	_asm pshufw			mm3, mm3, 00000000b /*0,a,0,a,0,a,0,a*/ \
	_asm movq			mm2, mm3 \
	_asm psllq			mm2, 8 /*a,0,a,0,a,0,a,0*/ \
	_asm por			mm3, mm2 /*a,a,a,a,a,a,a,a*/ \
	_asm psrlq			mm3, shR /*0,...,0,a,...,a*/ \
	_asm psllq			mmReg, shL \
	_asm por			mmReg, mm3 \
}

#define MacroGBLeftPaddingQw(mmReg, shR, shL) _asm \
{												\
	/*left side padding*/ \
	_asm movq			mm3, mmReg \
	_asm pand			mm3, FirstByteMaskQ \
	_asm pshufw			mm3, mm3, 11111111b /*a,0,a,0,a,0,a,0*/ \
	_asm movq			mm2, mm3 \
	_asm psrlq			mm2, 8 /*0,a,0,a,0,a,0,a*/ \
	_asm por			mm3, mm2 /*a,a,a,a,a,a,a,a*/ \
	_asm psllq			mm3, shL /*a,...,a,0,...,0*/ \
	_asm psrlq			mmReg, shR \
	_asm por			mmReg, mm3 \
}



#endif _MMXGETBLOCK_H_
