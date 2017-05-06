
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/

/*!
************************************************************************
*
* \file		mmxShortInline.c
*
* \brief
*		defines short MMX inline function for local usage;
*
************************************************************************
*/

#include "global.h"
#include "image.h"

#if VOI_H264D_NON_BASELINE
void SILumaAvgBlock(avdNativeInt xIdx, avdNativeInt yIdx, avdUInt8 *tmp_block)
{
	StorablePicture	*dec_picture = img->dec_picture;
	avdUInt8 *refPt = &dec_picture->imgY[yIdx][xIdx];
	avdNativeInt	 width = img->width;
	_asm {
		mov			eax, refPt
		mov			ecx, width
		mov			edx, tmp_block  /*non-inline function use lea, __inline use mov*/
		lea			esi, [ecx + 2*ecx]
		cmp			idx8x8, 8
		jl			L_BLOCK4x4A
		
L_BLOCK16x8A:
		movq		mm0, [eax]
		movq		mm1, [edx]
		pavgb		mm0, mm1
		movq		[eax], mm0
		movq		mm2, [eax + ecx]
		movq		mm3, [edx + 16]
		pavgb		mm2, mm3
		movq		[eax + ecx], mm2

		movq		mm0, [eax + 2 * ecx]
		movq		mm1, [edx + 32]
		pavgb		mm0, mm1
		movq		[eax + 2 * ecx], mm0
		movq		mm2, [eax + esi]
		movq		mm3, [edx + 48]
		pavgb		mm2, mm3
		movq		[eax + esi], mm2

		lea			eax, [eax + 4 * ecx]
		movq		mm0, [eax]
		movq		mm1, [edx + 64]
		pavgb		mm0, mm1
		movq		[eax], mm0
		movq		mm2, [eax + ecx]
		movq		mm3, [edx + 80]
		pavgb		mm2, mm3
		movq		[eax + ecx], mm2

		movq		mm0, [eax + 2 * ecx]
		movq		mm1, [edx + 96]
		pavgb		mm0, mm1
		movq		[eax + 2 * ecx], mm0
		movq		mm2, [eax + esi]
		movq		mm3, [edx + 112]
		pavgb		mm2, mm3
		movq		[eax + esi], mm2
		cmp			idx8x8, 8
		je			L_BLOCKOUTA

		// another 8x8
		shl			ecx, 2
		sub			eax, ecx
		shr			ecx, 2
		add			eax, 8
		add			edx, 8

		movq		mm0, [eax]
		movq		mm1, [edx]
		pavgb		mm0, mm1
		movq		[eax], mm0
		movq		mm2, [eax + ecx]
		movq		mm3, [edx + 16]
		pavgb		mm2, mm3
		movq		[eax + ecx], mm2

		movq		mm0, [eax + 2 * ecx]
		movq		mm1, [edx + 32]
		pavgb		mm0, mm1
		movq		[eax + 2 * ecx], mm0
		movq		mm2, [eax + esi]
		movq		mm3, [edx + 48]
		pavgb		mm2, mm3
		movq		[eax + esi], mm2

		lea			eax, [eax + 4 * ecx]
		movq		mm0, [eax]
		movq		mm1, [edx + 64]
		pavgb		mm0, mm1
		movq		[eax], mm0
		movq		mm2, [eax + ecx]
		movq		mm3, [edx + 80]
		pavgb		mm2, mm3
		movq		[eax + ecx], mm2

		movq		mm0, [eax + 2 * ecx]
		movq		mm1, [edx + 96]
		pavgb		mm0, mm1
		movq		[eax + 2 * ecx], mm0
		movq		mm2, [eax + esi]
		movq		mm3, [edx + 112]
		pavgb		mm2, mm3
		movq		[eax + esi], mm2

		cmp			idx8x8, 0
		je			L_BLOCKOUTA

		mov			idx8x8, 0
		lea			eax, [eax + ecx]
		lea			eax, [eax + esi - 8]
		add			edx, 120

		jmp			L_BLOCK16x8A

L_BLOCK4x4A:
		movq		mm0, [eax]
		movq		mm1, [edx]
		pavgb		mm0, mm1
		movd		[eax], mm0
		movq		mm2, [eax + ecx]
		movq		mm3, [edx + 16]
		pavgb		mm2, mm3
		movd		[eax + ecx], mm2

		movq		mm0, [eax + 2 * ecx]
		movq		mm1, [edx + 32]
		pavgb		mm0, mm1
		movd		[eax + 2 * ecx], mm0
		movd		mm2, [eax + esi]
		movq		mm3, [edx + 48]
		pavgb		mm2, mm3
		movd		[eax + esi], mm2
L_BLOCKOUTA:
		emms
	}
/*
	avdNativeInt ii, jj;
	for(jj=0;jj<idx8x8;jj++)  
	for(ii=0;ii<idx8x8;ii++)
		dec_picture->imgY[yIdx+jj][xIdx+ii] = ((tmp_block[(jj<<TMP_BLOCK_SHIFT)+ii]
			+dec_picture->imgY[yIdx+jj][xIdx+ii]+1)>>1);
*/
}

void SIChromaAvgBlock(int xTmp, int yTmp, avdUInt8 **imgUV2D, 
						  avdUInt8 *tmp_block)
{
	avdUInt8 *refPt = &imgUV2D[yTmp][xTmp];
	avdNativeInt	 width = img->width_cr;
	_asm {
		mov			eax, refPt
		mov			esi, width
		mov			edx, tmp_block /*non-inline function use lea, __inline use mov*/
		cmp			idx8x8, 4
		jl			L_BLOCK2x2A
		lea			ecx, [esi + 2*esi]

L_BLOCK8x4B:
		movq		mm0, [eax]
		movq		mm1, [edx]
		pavgb		mm0, mm1
		movd		[eax], mm0
		movq		mm2, [eax + esi]
		movq		mm3, [edx + 16]
		pavgb		mm2, mm3
		movd		[eax + esi], mm2

		movq		mm0, [eax + 2 * esi]
		movq		mm1, [edx + 32]
		pavgb		mm0, mm1
		movd		[eax + 2 * esi], mm0
		movd		mm2, [eax + ecx]
		movq		mm3, [edx + 48]
		pavgb		mm2, mm3
		movd		[eax + ecx], mm2
		cmp			idx8x8, 4
		je			L_BLOCKOUTB

		// another 4x4
		add			eax, 4
		add			edx, 4

		movq		mm0, [eax]
		movq		mm1, [edx]
		pavgb		mm0, mm1
		movd		[eax], mm0
		movq		mm2, [eax + esi]
		movq		mm3, [edx + 16]
		pavgb		mm2, mm3
		movd		[eax + esi], mm2

		movq		mm0, [eax + 2 * esi]
		movq		mm1, [edx + 32]
		pavgb		mm0, mm1
		movd		[eax + 2 * esi], mm0
		movd		mm2, [eax + ecx]
		movq		mm3, [edx + 48]
		pavgb		mm2, mm3
		movd		[eax + ecx], mm2

		cmp			idx8x8, 0
		je			L_BLOCKOUTB

		mov			idx8x8, 0
		lea			eax, [eax + ecx]
		lea			eax, [eax + esi]
		sub			eax, 4
		add			edx, 60
		jmp			L_BLOCK8x4B

L_BLOCK2x2A:
		movq		mm0, [eax]
		movq		mm1, [edx]
		pavgb		mm0, mm1
		movd		ecx, mm0
		mov			[eax], cx

		movd		mm2, [eax + esi]
		movq		mm3, [edx + 16]
		pavgb		mm2, mm3
		movd		ecx, mm2
		mov			[eax + esi], cx
L_BLOCKOUTB:									
		emms
	}
/*
	avdNativeInt j1, i1;
	for (j1 = 0; j1 < idx8x8; j1++)
	for (i1 = 0; i1 < idx8x8; i1++)
		imgUV2D[yTmp+j1][xTmp+i1] = ((imgUV2D[yTmp+j1][xTmp+i1] 
			+ tmp_block[(j1<<TMP_BLOCK_SHIFT)+i1] + 1)>>1);
*/
}

#endif // VOI_H264D_NON_BASELINE
