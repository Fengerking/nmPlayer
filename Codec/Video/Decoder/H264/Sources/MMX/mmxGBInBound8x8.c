#include "global.h"
#include "image.h"

__int64 Mask16  = 0x0010001000100010;
__int64 Mask32  = 0x0020002000200020;
__int64 Mask512 = 0x0000020000000200;

/* tmp64A0(p-2), mmReg1(p-1), mmReg4(p0), mmReg6(p1), mmReg0(p2)*/ \
#define MacroGBInBoundVertInterp8WordAtMM2MM3(mmReg1, mmReg4, mmReg6, mmReg0, tmp64A0, tmp64B0) _asm \
{								\
	/* for low 4 bytes*/ \
	_asm movq			mm2, mmReg4 \
	_asm movq			mm5, mmReg6 \
	_asm punpcklbw		mm2, mm7 /*0L*/ \
	_asm punpcklbw		mm5, mm7 /*1L*/ \
	_asm paddw			mm2, mm5 /*L: p0+p1*/ \
	_asm psllq			mm2, 2   /*L: 4*(p0+p1)*/ \
	_asm movq			mm5, mmReg1 \
	_asm punpcklbw		mm5, mm7 /*-1L*/ \
	_asm movq			mm3, mmReg0 \
	_asm punpcklbw		mm3, mm7 /*2L*/ \
	_asm paddw			mm3, mm5 \
	_asm psubsw			mm2, mm3 /*L: -p(-1) + 4*(p0+p1) - p2*/ \
	_asm movq			mm3, mm2 \
	_asm psllw			mm3, 2 \
	_asm paddsw			mm2, mm3 /*L: -5*p(-1) + 20*(p0+p1) - 5*p2*/ \
	/* for high 4 bytes;*/ \
	_asm movq			tmp64B0, mmReg1 /*save p(-1) at tmp64B*/ \
	_asm movq			mm3, mmReg4 \
	_asm movq			mm5, mmReg6 \
	_asm punpckhbw		mm3, mm7 /*0H*/ \
	_asm punpckhbw		mm5, mm7 /*1H*/ \
	_asm paddw			mm3, mm5 /*H: p0+p1*/ \
	_asm psllq			mm3, 2   /*H: 4*(p0+p1)*/ \
	_asm punpckhbw		mmReg1, mm7 /*-1H*/ \
	_asm movq			mm5, mmReg0 \
	_asm punpckhbw		mm5, mm7 /*2H*/ \
	_asm paddw			mm5, mmReg1 \
	_asm psubsw			mm3, mm5 /*H: -p(-1) + 4*(p0+p1) - p2*/ \
	_asm movq			mm5, mm3 \
	_asm psllw			mm5, 2 \
	_asm paddsw			mm3, mm5 /*H: -5*p(-1) + 20*(p0+p1) - 5*p2*/ \
	/* merge high and low*/ \
	_asm movq			mmReg1, tmp64A0 /*read back p(-2) at tmp64A*/ \
	_asm movq			mm5, mmReg1 \
	_asm punpcklbw		mmReg1, mm7 /*-2L*/ \
	_asm punpckhbw		mm5, mm7 /*-2H*/ \
	_asm paddsw			mm2, mmReg1 /*L: p(-2) - 5*p(-1) + 20*(p0+p1) - 5*p2*/ \
	_asm paddsw			mm3, mm5 /*H: p(-2) - 5*p(-1) + 20*(p0+p1) - 5*p2*/ \
	_asm movq			mmReg1, [eax] /*load p3;*/ \
	_asm movq			mm5, mmReg1 \
	_asm punpcklbw		mm5, mm7 /*+3L*/ \
	_asm paddsw			mm2, mm5 /*L: p(-2) - 5*p(-1) + 20*(p0+p1) - 5*p2 + p3*/ \
	_asm movq			mm5, mmReg1 \
	_asm punpckhbw		mm5, mm7 /*+3H*/ \
	_asm paddsw			mm3, mm5 /*H: p(-2) - 5*p(-1) + 20*(p0+p1) - 5*p2 + p3*/ \
}

#define MacroGBInBoundAdd16Div32NPackMM2MM3 _asm \
{								\
	/* ((x+16)>>5)*/ \
	_asm paddsw			mm2, Mask16 \
	_asm paddsw			mm3, Mask16 \
	_asm psraw			mm2, 5 \
	_asm psraw			mm3, 5 \
	_asm packuswb		mm2, mm3 /*8 bytes*/ \
}


#define MacroGBInBoundVertInterp8ByteAvgSaveNInc _asm \
{			\
	_asm movq			mm5, [edx] \
	_asm MacroGBInBoundAdd16Div32NPackMM2MM3 \
	_asm pavgb			mm5, mm2 \
	_asm movq			[edx], mm5 \
	_asm add			edx, edi \
	_asm add			eax, ecx \
}
  
#define MacroGB8x8InBoundVertInterp1Ln(mmReg1, mmReg4, mmReg6, mmReg0, tmp64A0, tmp64B0, LabelGB8x8VertInterp1LnOut) _asm \
{								\
	_asm MacroGBInBoundVertInterp8WordAtMM2MM3(mmReg1, mmReg4, mmReg6, mmReg0, tmp64A0, tmp64B0) \
	_asm MacroGBInBoundAdd16Div32NPackMM2MM3 \
	_asm test			dyP, 1 \
	_asm jz				LabelGB8x8VertInterp1LnOut \
	_asm movq			mm5, mmReg4 \
	_asm pand			mm5, avg1 /*for dxP == 1;*/ \
	_asm movq			mm3, mmReg6 \
	_asm pand			mm3, avg3 /*for dxP == 3;*/ \
	_asm por			mm5, mm3 \
	_asm pavgb			mm2, mm5 \
_asm LabelGB8x8VertInterp1LnOut: \
	_asm movq			[edx], mm2 /*save the first row;*/ \
	_asm add			edx, edi \
	_asm add			eax, ecx \
}

#define MacroGBInBoundHoriInterp8WordAtMM1MM2 _asm \
{								\
	_asm movq			mm0, [eax] /*-2 to 5*/ \
	_asm movq			mm6, [eax + 8] /*6  to 13*/ \
	/* do first 4 bytes;*/ \
	_asm movq			mm1, mm0 \
	_asm movq			mm2, mm0 \
	_asm psrlq			mm1, 16  /*get 0-3*/ \
	_asm psrlq			mm2, 24  /*get 1-4*/ \
	_asm punpcklbw		mm1, mm7 /*p(0),0-3;*/ \
	_asm punpcklbw		mm2, mm7 /*p(1),1-4;*/ \
	_asm paddw			mm1, mm2 /*p(0) + p(1)*/ \
	_asm psllq			mm1, 2   /*4*(p(0)+p(1))*/ \
	_asm movq			mm2, mm0 \
	_asm movq			mm3, mm0 \
	_asm psrlq			mm2, 8   /*get -1 - 2*/ \
	_asm psrlq			mm3, 32  /*get 2-5*/ \
	_asm punpcklbw		mm2, mm7 /*p(-1);*/ \
	_asm punpcklbw		mm3, mm7 /*p(2);*/ \
	_asm paddw			mm2, mm3 /*p(-1) + p(2);*/ \
	_asm psubsw			mm1, mm2 /*-p(-1) - p(2) + 4*(p(0)+p(1))*/ \
	_asm movq			mm2, mm1 \
	_asm psllw			mm2, 2 \
	_asm paddsw			mm1, mm2 /*-5*(p(-1)+p(2)) + 20*(p(0)+p(1))*/ \
	_asm movq			mm2, mm0 /*-2 to 5*/ \
	_asm movq			mm3, mm6 /*6 to 13  */ \
	_asm movq			mm4, mm0 /*-2 to 5*/ \
	_asm psllq			mm3, 24  /*000 + 6-10, from LSB*/ \
	_asm psrlq			mm4, 40  /*3-5*/ \
	_asm por			mm3, mm4 /*3-10*/ \
	_asm punpcklbw		mm2, mm7 /*p(-2);*/ \
	_asm punpcklbw		mm3, mm7 /*p(3);*/ \
	_asm paddw			mm2, mm3 /*p(-2)+p(3)*/ \
	_asm paddsw			mm1, mm2 /*p(-2)+p(3) - 5*(p(-1)+p(2)) + 20*(p(0)+p(1))*/ \
	/* do 2nd 4 bytes;*/ \
	_asm movq			mm2, mm6 /*6-13*/ \
	_asm psllq			mm2, 16  /*00 + 6-11*/ \
	_asm movq			mm3, mm0 \
	_asm psrlq			mm3, 48  /*4-5*/ \
	_asm por			mm3, mm2 /*4-11*/ \
	_asm punpcklbw		mm3, mm7 /*p(0);*/ \
	_asm movq			mm5, mm0 /*-2-5*/ \
	_asm psrlq			mm5, 56  /*5*/ \
	_asm psrlq			mm2, 8   /*0 + 6-11 + 0, from LSB*/ \
	_asm por			mm5, mm2 /*5-11*/ \
	_asm punpcklbw		mm5, mm7 /*p(1);*/ \
	_asm paddw			mm5, mm3 /*p(0)+p(1)*/ \
	_asm psllq			mm5, 2   /*4*(p(0)+p(1))*/ \
	_asm psllq			mm2, 16  /*000 + 6-10, from LSB*/ \
	_asm por			mm4, mm2 /*3-10*/ \
	_asm punpcklbw		mm4, mm7 /*p(-1);*/ \
	_asm movq			mm3, mm6 /*6-13*/ \
	_asm punpcklbw		mm3, mm7 /*p(2);*/ \
	_asm paddw			mm4, mm3 /*p(-1) + p(2)*/ \
	_asm psubsw			mm5, mm4 /*-p(-1) + 4*(p(0)+p(1)) - p(2)*/ \
	_asm movq			mm3, mm5 \
	_asm psllw			mm5, 2 \
	_asm paddsw			mm3, mm5 /*-5*p(-1) + 20*(p(0)+p(1)) + -5*p(2)*/ \
	_asm psrlq			mm2, 32  /*7-10*/ \
	_asm punpcklbw		mm2, mm7 /*p(3);*/ \
	_asm movq			mm4, mm0  \
	_asm psrlq			mm4, 32  /*2-5*/ \
	_asm punpcklbw		mm4, mm7 /*p(-2)*/ \
	_asm paddw			mm2, mm4 /*p(-2) + p(3)*/ \
	_asm paddsw			mm2, mm3 /*p(-2) - 5*p(-1) + 20*(p(0)+p(1)) - 5*p(2) + p(3)*/ \
}

/*mm5(p(-2)), mm0(p(-1)), mm1(p0), mm4(p1), mm6(p2)*/
#define MacroInBoundVertInterp4WordIn(mmReg5, mmReg0, mmReg1, mmReg4, mmReg6) _asm \
{								\
	/* 0-1 byte */ \
	_asm movq			mm2, mmReg1 \
	_asm movq			mmReg5, mmReg4 \
	_asm punpcklwd		mm2, mm7 /* 0L */ \
	_asm punpcklwd		mmReg5, mm7 /* 1L */ \
	_asm pslld			mm2, 16 \
	_asm psrad			mm2, 16 \
	_asm pslld			mmReg5, 16 \
	_asm psrad			mmReg5, 16 \
	_asm paddd			mm2, mmReg5 /* L: p0+p1 */ \
	_asm pslld			mm2, 2   /* L: 4*(p0+p1) */ \
	_asm movq			mmReg5, mmReg0 \
	_asm punpcklwd		mmReg5, mm7 /* -1L */ \
	_asm pslld			mmReg5, 16 \
	_asm psrad			mmReg5, 16 \
	_asm movq			mm3, mmReg6 \
	_asm punpcklwd		mm3, mm7 /* 2L */ \
	_asm pslld			mm3, 16 \
	_asm psrad			mm3, 16 \
	_asm paddd			mm3, mmReg5 \
	_asm psubd			mm2, mm3 /* L: -p(-1) + 4*(p0+p1) - p2 */ \
	_asm movq			mm3, mm2 \
	_asm pslld			mm3, 2 \
	_asm paddd			mm2, mm3 /* L: -5*p(-1) + 20*(p0+p1) - 5*p2 */ \
	_asm movq			mm3, [eax] /* -2L */ \
	_asm movq			mmReg5, [eax + 80] /* 3L */ \
	_asm punpcklwd		mm3, mm7 /* -2L */ \
	_asm punpcklwd		mmReg5, mm7 /* 3L */ \
	_asm pslld			mm3, 16 \
	_asm psrad			mm3, 16 \
	_asm pslld			mmReg5, 16 \
	_asm psrad			mmReg5, 16 \
	_asm paddd			mm3, mmReg5 /* p(-2) + p2 */ \
	_asm paddd			mm3, mm2 /* p(-2) - 5*p(-1) + 20*(p0+p1) - 5*p2 + p2 */ \
	_asm paddd			mm3, Mask512 \
	_asm psrad			mm3, 10 \
	/* 2-3 byte */ \
	_asm movq			mm2, mmReg1 \
	_asm movq			mmReg5, mmReg4 \
	_asm punpckhwd		mm2, mm7 /* 0L */ \
	_asm punpckhwd		mmReg5, mm7 /* 1L */ \
	_asm pslld			mm2, 16 \
	_asm psrad			mm2, 16 \
	_asm pslld			mmReg5, 16 \
	_asm psrad			mmReg5, 16 \
	_asm paddd			mm2, mmReg5 /* L: p0+p1 */ \
	_asm pslld			mm2, 2   /* L: 4*(p0+p1) */ \
	_asm movq			mmReg5, mmReg0 \
	_asm punpckhwd		mmReg5, mm7 /* -1L */ \
	_asm pslld			mmReg5, 16 \
	_asm psrad			mmReg5, 16 \
	_asm psubd			mm2, mmReg5 /* L: -p(-1) + 4*(p0+p1) */ \
	_asm movq			mmReg5, mmReg6 \
	_asm punpckhwd		mmReg5, mm7 /* 2L */ \
	_asm pslld			mmReg5, 16 \
	_asm psrad			mmReg5, 16 \
	_asm psubd			mm2, mmReg5 /* L: -p(-1) + 4*(p0+p1) - p2 */ \
	_asm movq			mmReg5, mm2 \
	_asm pslld			mmReg5, 2 \
	_asm paddd			mm2, mmReg5 /* L: -5*p(-1) + 20*(p0+p1) - 5*p2 */ \
	_asm movq			mmReg5, [eax] /* -2L */ \
	_asm punpckhwd		mmReg5, mm7 /* -2L */ \
	_asm pslld			mmReg5, 16 \
	_asm psrad			mmReg5, 16 \
	_asm paddd			mm2, mmReg5 /* p(-2) - 5*p(-1) + 20*(p0+p1) - 5*p2*/ \
	_asm movq			mmReg5, [eax + 80] /* 3L */ \
	_asm punpckhwd		mm7, mmReg5 /* 3L */ \
	_asm psrad			mm7, 16 \
	_asm paddd			mm2, mm7 /* p(-2) - 5*p(-1) + 20*(p0+p1) - 5*p2 + p3 */ \
	_asm pxor			mm7, mm7 \
	_asm paddd			mm2, Mask512 \
	_asm psrad			mm2, 10 \
	_asm packssdw		mm3, mm2 \
	_asm packuswb		mm3, mm7 /* first 8 bytes */ \
	_asm movd			esi, mm3 \
	_asm mov			[edx], esi /* save first 4 bytes; */ \
	_asm add			edx, edi \
	_asm add			eax, 16 \
}

#define MacroGBInBoundVertInterp8WordSaveNInc _asm \
{			\
	_asm movq			[edx], mm2 \
	_asm movq			[edx + 8], mm3 \
	_asm add			edx, 32 \
	_asm add			eax, ecx \
}

#define MacroGBInBoundVertInterp4WordOut(mmReg5, mmReg0, mmReg1, mmReg4, mmReg6) _asm \
{			\
	_asm add			edx, 32 \
	_asm add			eax, ecx \
	_asm movq			mm2, mmReg1 \
	_asm paddw			mm2, mmReg4 /* L: p0+p1 */ \
	_asm psllq			mm2, 2   /* L: 4*(p0+p1) */ \
	_asm movq			mm3, mmReg0 \
	_asm paddw			mm3, mmReg6 \
	_asm psubsw			mm2, mm3 /* L: -p(-1) + 4*(p0+p1) - p2 */ \
	_asm movq			mm3, mm2 \
	_asm psllw			mm3, 2 \
	_asm paddsw			mm2, mm3 /* L: -5*p(-1) + 20*(p0+p1) - 5*p2 */ \
	_asm paddsw			mm2, mmReg5 /* L: p(-2) - 5*p(-1) + 20*(p0+p1) - 5*p2 */ \
	_asm movd			mmReg5, [eax] /* load p3; */ \
	_asm punpcklbw		mmReg5, mm7 /* +3L */ \
	_asm paddsw			mm2, mmReg5 /* L: p(-2) - 5*p(-1) + 20*(p0+p1) - 5*p2 + p3 */ \
	_asm movq			[edx], mm2 \
}

#define MacroGBInBoundVertInterp8WordAtMM2MM3FirstRow _asm \
{				\
	_asm mov			ecx, refWidth \
	_asm lea			esi, [ecx + 2*ecx] /* 3*refWidth; */ \
	_asm pxor			mm7, mm7 \
	/* first row; */ \
	_asm movq			mm1, [eax + 2*ecx] /* p0 */ \
	_asm movq			mm4, [eax + esi]   /* p1 */ \
	/* for low 4 bytes; */ \
	_asm movq			mm2, mm1 \
	_asm movq			mm5, mm4 \
	_asm punpcklbw		mm2, mm7 /* 0L */ \
	_asm punpcklbw		mm5, mm7 /* 1L */ \
	_asm paddw			mm2, mm5 /* L: p0+p1 */ \
	_asm psllq			mm2, 2   /* L: 4*(p0+p1) */ \
	_asm movq			mm0, [eax + ecx]/* p(-1) */ \
	_asm movq			mm5, mm0 \
	_asm punpcklbw		mm5, mm7 /* -1L */ \
	_asm movq			mm6, [eax + 4*ecx]/* p(2) */ \
	_asm movq			mm3, mm6 \
	_asm punpcklbw		mm3, mm7 /* 2L */ \
	_asm paddw			mm3, mm5 \
	_asm psubsw			mm2, mm3 /* L: -p(-1) + 4*(p0+p1) - p2 */ \
	_asm movq			mm3, mm2 \
	_asm psllw			mm3, 2 \
	_asm paddsw			mm2, mm3 /* L: -5*p(-1) + 20*(p0+p1) - 5*p2 */ \
	/* for high 4 bytes; */ \
	_asm movq			tmp64A, mm0 /* save p(-1) at tmp64B */ \
	_asm movq			mm3, mm1 \
	_asm movq			mm5, mm4 \
	_asm punpckhbw		mm3, mm7 /* 0H */ \
	_asm punpckhbw		mm5, mm7 /* 1H */ \
	_asm paddw			mm3, mm5 /* H: p0+p1 */ \
	_asm psllq			mm3, 2   /* H: 4*(p0+p1) */ \
	_asm punpckhbw		mm0, mm7 /* -1H */ \
	_asm movq			mm5, mm6 \
	_asm punpckhbw		mm5, mm7 /* 2H */ \
	_asm paddw			mm5, mm0 \
	_asm psubsw			mm3, mm5 /* H: -p(-1) + 4*(p0+p1) - p2 */ \
	_asm movq			mm5, mm3 \
	_asm psllw			mm5, 2 \
	_asm paddsw			mm3, mm5 /* H: -5*p(-1) + 20*(p0+p1) - 5*p2 */ \
	_asm movq			mm0, [eax] /* p(-2) */ \
	_asm lea			eax, [eax + 4*ecx] /* eax at y_pos+2 row; */ \
	_asm movq			mm5, mm0 \
	_asm punpcklbw		mm0, mm7 /* -2L */ \
	_asm punpckhbw		mm5, mm7 /* -2H */ \
	_asm paddsw			mm2, mm0 /* L: p(-2) - 5*p(-1) + 20*(p0+p1) - 5*p2 */ \
	_asm paddsw			mm3, mm5 /* H: p(-2) - 5*p(-1) + 20*(p0+p1) - 5*p2 */ \
	_asm movq			mm0, [eax + ecx] /* load p3; */ \
	_asm movq			mm5, mm0 \
	_asm punpcklbw		mm5, mm7 /* +3L */ \
	_asm paddsw			mm2, mm5 /* L: p(-2) - 5*p(-1) + 20*(p0+p1) - 5*p2 + p3 */ \
	_asm movq			mm5, mm0 \
	_asm punpckhbw		mm5, mm7 /* +3H */ \
	_asm paddsw			mm3, mm5 /* H: p(-2) - 5*p(-1) + 20*(p0+p1) - 5*p2 + p3 */ \
}


void GetBlock8x8_InBound(avdNativeInt x_pos, avdNativeInt y_pos, avdUInt8 *refStart[], 
						 avdUInt8 *outStart, avdNativeInt outLnLength)
{
	avdInt16 *tmpResJ;
	avdUInt8 *out, *refPt, *clp255;
	avdNativeInt dxP, dyP, i, j, pres_x, pres_y, refWidth;
	avdInt32	 middle;

	refWidth = img->width;
	dxP = x_pos&3;
	dyP = y_pos&3;
	x_pos >>= 2;
	y_pos >>= 2;
	if (!dxP && !dyP) { //!dxP && !dyP (full pel);
		refPt = &refStart[y_pos][x_pos];
		_asm {
			mov				eax, refPt
			mov				ecx, refWidth
			mov				edx, outStart
			mov				edi, outLnLength
			lea				esi, [ecx + 2*ecx]
			movq			mm0, [eax] //0
			movq			[edx], mm0
			movq			mm1, [eax + ecx] //1
			movq			[edx + edi], mm1
			lea				edx, [edx + 2 * edi] // edx = outStart + 2*outLnLength;
			movq			mm2, [eax + 2 * ecx] //2
			movq			[edx], mm2
			movq			mm3, [eax + esi] //3
			movq			[edx + edi], mm3
			lea				eax, [eax + 4 * ecx] // eax = refPt + 4*refWidth;
			lea				edx, [edx + 2 * edi]
			movq			mm4, [eax] //4
			movq			[edx], mm4
			movq			mm5, [eax + ecx] //5
			movq			[edx + edi], mm5
			lea				edx, [edx + 2 * edi]
			movq			mm6, [eax + 2 * ecx] //6
			movq			[edx], mm6
			movq			mm7, [eax + esi] //7
			movq			[edx + edi], mm7
		}
		return;
	}

	clp255 = img->clip255;
	out = outStart;
	if (!dyP) { /* No vertical interpolation */
		refPt = &refStart[y_pos][x_pos - 2];
		_asm {
			mov				eax, refPt
			mov				ecx, refWidth
			mov				edx, outStart
			mov				edi, outLnLength
			mov				esi, 8
			pxor			mm7, mm7
GB8x8InBoundLnStartA:
			// start: same as MacroGBInBoundHoriInterp8WordAtMM1MM2;
			//MacroGBInBoundHoriInterp8WordAtMM1MM2
			movq			mm0, [eax] //-2 to 5
			movq			mm6, [eax + 8] //6  to 13
			// do first 4 bytes;
			movq			mm1, mm0
			movq			mm2, mm0
			psrlq			mm1, 16  // get 0-3
			psrlq			mm2, 24  // get 1-4
			punpcklbw		mm1, mm7 // p(0),0-3;
			punpcklbw		mm2, mm7 // p(1),1-4;
			paddw			mm1, mm2 // p(0) + p(1)
			psllq			mm1, 2   // 4*(p(0)+p(1))
			movq			mm2, mm0
			movq			mm3, mm0
			psrlq			mm2, 8   // get -1 - 2
			psrlq			mm3, 32  // get 2-5
			punpcklbw		mm2, mm7 // p(-1);
			punpcklbw		mm3, mm7 // p(2);
			paddw			mm2, mm3 // p(-1) + p(2);
			psubsw			mm1, mm2 // -p(-1) - p(2) + 4*(p(0)+p(1))
			movq			mm2, mm1
			psllw			mm2, 2
			paddsw			mm1, mm2 // -5*(p(-1)+p(2)) + 20*(p(0)+p(1))
			movq			mm2, mm0 // -2 to 5
			movq			mm3, mm6 // 6 to 13  
			movq			mm4, mm0 // -2 to 5
			psllq			mm3, 24  // 000 + 6-10, from LSB
			psrlq			mm4, 40  // 3-5
			por				mm3, mm4 // 3-10
			punpcklbw		mm2, mm7 // p(-2);
			punpcklbw		mm3, mm7 // p(3);
			paddw			mm2, mm3 // p(-2)+p(3)
			paddsw			mm1, mm2 // p(-2)+p(3) - 5*(p(-1)+p(2)) + 20*(p(0)+p(1))
			// do 2nd 4 bytes;
			movq			mm2, mm6 // 6-13
			psllq			mm2, 16  // 00 + 6-11
			movq			mm3, mm0
			psrlq			mm3, 48  // 4-5
			por				mm3, mm2 // 4-11
			punpcklbw		mm3, mm7 // p(0);
			movq			mm5, mm0 // -2-5
			psrlq			mm5, 56  // 5
			psrlq			mm2, 8   // 0 + 6-11 + 0, from LSB
			por				mm5, mm2 // 5-11
			punpcklbw		mm5, mm7 // p(1);
			paddw			mm5, mm3 // p(0)+p(1)
			psllq			mm5, 2   // 4*(p(0)+p(1))
			psllq			mm2, 16  // 000 + 6-10, from LSB
			por				mm4, mm2 // 3-10
			punpcklbw		mm4, mm7 // p(-1);
			movq			mm3, mm6 // 6-13
			punpcklbw		mm3, mm7 // p(2);
			paddw			mm4, mm3 // p(-1) + p(2)
			psubsw			mm5, mm4 // -p(-1) + 4*(p(0)+p(1)) - p(2)
			movq			mm3, mm5
			psllw			mm5, 2
			paddsw			mm3, mm5 // -5*p(-1) + 20*(p(0)+p(1)) + -5*p(2)
			psrlq			mm2, 32  // 7-10
			punpcklbw		mm2, mm7 // p(3);
			movq			mm4, mm0 
			psrlq			mm4, 32  // 2-5
			punpcklbw		mm4, mm7 // p(-2)
			paddw			mm2, mm4 // p(-2) + p(3)
			paddsw			mm2, mm3 // p(-2) - 5*p(-1) + 20*(p(0)+p(1)) - 5*p(2) + p(3)
			// end: same as MacroGBInBoundHoriInterp8WordAtMM1MM2;

			// add 16 and shift 5, then pack it;
			paddsw			mm1, Mask16
			paddsw			mm2, Mask16
			psraw			mm1, 5
			psraw			mm2, 5
			packuswb		mm1, mm2
			test			dxP, 1
			jz				GB8x8InBoundWrite8BytesA
			// do average;
			test			dxP, 2
			jz				GB8x8InBoundAvgdxP1
			// dxp = 3;
			psrlq			mm0, 24  //1-5 + 000, from LSB
			psllq			mm6, 40  //00000 + 6-8, from LSB
			por				mm0, mm6 //1-8
			pavgb			mm1, mm0
			jmp				GB8x8InBoundWrite8BytesA
GB8x8InBoundAvgdxP1:
			// dxp = 1;
			psrlq			mm0, 16  //0-5 + 00, from LSB
			psllq			mm6, 48  //000000 + 6-7, from LSB
			por				mm0, mm6 //0-7, from LSB
			pavgb			mm1, mm0
GB8x8InBoundWrite8BytesA:
			// slower in Celeron;
			//movntq			[edx], mm1
			movq			[edx], mm1
			add				edx, edi
			add				eax, ecx
			sub				esi, 1
			jnz				GB8x8InBoundLnStartA;
			//sfence
		}
#if 0
		for (j = 0; j < 8; j++) {
			refTmp = refStart[y_pos + j];
			for (i = 0; i < 8; i++) {
				pres_x = x_pos + i;
				middle = refTmp[pres_x-1] + refTmp[pres_x+2] -
					((refTmp[pres_x] + refTmp[pres_x+1])<<2);
				test = clp255[(refTmp[pres_x-2] + refTmp[pres_x+3] -
					middle - (middle<<2) + 16)>>5];
				if (dxP&1) {
					test = (test + refTmp[pres_x+(dxP>>1)] + 1)>>1;
				}
				if (test != outStart[i])
					printf("\nerror: ccc = %d, test= %d, outStart[i]= %d, i = %d, j = %d, x_pos = %d, y_pos = %d",
						ccc, test, outStart[i], i, j, x_pos, y_pos);
			}
			outStart += outLnLength;
		}
#endif 0
	}
	else if (!dxP) {  /* No horizontal interpolation */
		__int64 avg1, avg3, tmp64A, tmp64B;
		avg1 = (dyP == 1) ? ~0 : 0;
		avg3 = (dyP == 3) ? ~0 : 0;
		refPt = &refStart[y_pos - 2][x_pos];
		_asm {
			mov				edx, outStart
			mov				edi, outLnLength
			mov				eax, refPt
			/****** Start: same code as MacroGBInBoundVertInterp8WordAtMM2MM3FirstRow *************/
			mov				ecx, refWidth
			lea				esi, [ecx + 2*ecx] // 3*refWidth;
			pxor			mm7, mm7

			// first row;
			movq			mm1, [eax + 2*ecx] // p0
			movq			mm4, [eax + esi]   // p1
			// for low 4 bytes;
			movq			mm2, mm1
			movq			mm5, mm4
			punpcklbw		mm2, mm7 // 0L
			punpcklbw		mm5, mm7 // 1L
			paddw			mm2, mm5 // L: p0+p1
			psllq			mm2, 2   // L: 4*(p0+p1)
			movq			mm0, [eax + ecx]// p(-1)
			movq			mm5, mm0
			punpcklbw		mm5, mm7 // -1L
			movq			mm6, [eax + 4*ecx]// p(2)
			movq			mm3, mm6
			punpcklbw		mm3, mm7 // 2L
			paddw			mm3, mm5
			psubsw			mm2, mm3 // L: -p(-1) + 4*(p0+p1) - p2
			movq			mm3, mm2
			psllw			mm3, 2
			paddsw			mm2, mm3 // L: -5*p(-1) + 20*(p0+p1) - 5*p2

			// for high 4 bytes;
			movq			tmp64A, mm0 // save p(-1) at tmp64B
			movq			mm3, mm1
			movq			mm5, mm4
			punpckhbw		mm3, mm7 // 0H
			punpckhbw		mm5, mm7 // 1H
			paddw			mm3, mm5 // H: p0+p1
			psllq			mm3, 2   // H: 4*(p0+p1)
			punpckhbw		mm0, mm7 // -1H
			movq			mm5, mm6
			punpckhbw		mm5, mm7 // 2H
			paddw			mm5, mm0
			psubsw			mm3, mm5 // H: -p(-1) + 4*(p0+p1) - p2
			movq			mm5, mm3
			psllw			mm5, 2
			paddsw			mm3, mm5 // H: -5*p(-1) + 20*(p0+p1) - 5*p2

			movq			mm0, [eax] // p(-2)
			lea				eax, [eax + 4*ecx] // eax at y_pos+2 row;
			movq			mm5, mm0
			punpcklbw		mm0, mm7 // -2L
			punpckhbw		mm5, mm7 // -2H
			paddsw			mm2, mm0 // L: p(-2) - 5*p(-1) + 20*(p0+p1) - 5*p2
			paddsw			mm3, mm5 // H: p(-2) - 5*p(-1) + 20*(p0+p1) - 5*p2
			movq			mm0, [eax + ecx] // load p3;
			movq			mm5, mm0
			punpcklbw		mm5, mm7 // +3L
			paddsw			mm2, mm5 // L: p(-2) - 5*p(-1) + 20*(p0+p1) - 5*p2 + p3
			movq			mm5, mm0
			punpckhbw		mm5, mm7 // +3H
			paddsw			mm3, mm5 // H: p(-2) - 5*p(-1) + 20*(p0+p1) - 5*p2 + p3
			/****** End: same code as MacroGBInBoundVertInterp8WordAtMM2MM3FirstRow *************/

			/*** Start: same code as MacroGBInBoundAdd16Div32NPackMM2MM3 ***/
			paddsw			mm2, Mask16
			paddsw			mm3, Mask16
			psraw			mm2, 5
			psraw			mm3, 5
			packuswb		mm2, mm3 // first 8 bytes
			/*** End: same code as MacroGBInBoundAdd16Div32NPackMM2MM3 ***/
			test			dyP, 1
			jz				LabelGB8x8VI1LnOut0
			movq			mm5, mm1
			pand			mm5, avg1 // for dxP == 1;
			movq			mm3, mm4
			pand			mm3, avg3 // for dxP == 3;
			por				mm5, mm3
			pavgb			mm2, mm5
LabelGB8x8VI1LnOut0:
			movq			[edx], mm2 // save the first row;
			add				edx, edi
			lea				eax, [eax + 2*ecx] // eax at y_pos+4 row;

			// the code in the macro is the same as the first line above, except data loading;
			// To debug the macro, we can either debug the first line code above or copy and
			// roll out macro one by one;
			//2th row: tmp64A(p-2), mm1(p-1), mm4(p0), mm6(p1), mm0(p2); eax at y_pos+4 row;
			MacroGB8x8InBoundVertInterp1Ln(mm1, mm4, mm6, mm0, tmp64A, tmp64B, LabelGB8x8VI1LnOut1)
			//3rd row: tmp64B(p-2), mm4(p-1), mm6(p0), mm0(p1), mm1(p2); eax at y_pos+5 row;
			MacroGB8x8InBoundVertInterp1Ln(mm4, mm6, mm0, mm1, tmp64B, tmp64A, LabelGB8x8VI1LnOut2)
			//4th row: tmp64A(p-2), mm6(p-1), mm0(p0), mm1(p1), mm4(p2); eax at y_pos+6 row;
			MacroGB8x8InBoundVertInterp1Ln(mm6, mm0, mm1, mm4, tmp64A, tmp64B, LabelGB8x8VI1LnOut3)
			//5th row: tmp64B(p-2), mm0(p-1), mm1(p0), mm4(p1), mm6(p2); eax at y_pos+7 row;
			MacroGB8x8InBoundVertInterp1Ln(mm0, mm1, mm4, mm6, tmp64B, tmp64A, LabelGB8x8VI1LnOut4)
			//6th row: tmp64A(p-2), mm1(p-1), mm4(p0), mm6(p1), mm0(p2); eax at y_pos+8 row;
			MacroGB8x8InBoundVertInterp1Ln(mm1, mm4, mm6, mm0, tmp64A, tmp64B, LabelGB8x8VI1LnOut5)
			//7rd row: tmp64B(p-2), mm4(p-1), mm6(p0), mm0(p1), mm1(p2); eax at y_pos+9 row;
			MacroGB8x8InBoundVertInterp1Ln(mm4, mm6, mm0, mm1, tmp64B, tmp64A, LabelGB8x8VI1LnOut6)
			//8th row: tmp64A(p-2), mm6(p-1), mm0(p0), mm1(p1), mm4(p2); eax at y_pos+10 row;
			MacroGB8x8InBoundVertInterp1Ln(mm6, mm0, mm1, mm4, tmp64A, tmp64B, LabelGB8x8VI1LnOut7)
		}
#if 0
		for (j = 0; j < 8; j++) {
			pres_y = y_pos + j;
			for (i = 0; i < 8; i++) {
				pres_x = x_pos + i;
				middle = refStart[pres_y-1][pres_x] + refStart[pres_y+2][pres_x] -
					((refStart[pres_y][pres_x] + refStart[pres_y+1][pres_x])<<2);
				test = clp255[(refStart[pres_y-2][pres_x] + 
					refStart[pres_y+3][pres_x] - middle - (middle<<2) + 16)>>5];
				if (dyP&1) 
					test = ((test + refStart[y_pos + j + (dyP>>1)][pres_x] + 1)>>1);
				if (test != outStart[i])
					printf("stop");
			}
			outStart += outLnLength;
		}
#endif 0
	}
	else if (dxP != 2 && dyP != 2) {  /* Diagonal interpolation */
		__int64 tmp64A, tmp64B;
		refPt = &refStart[dyP == 1 ? y_pos : y_pos+1][x_pos - 2];
		_asm {
			mov				eax, refPt
			mov				ecx, refWidth
			mov				edx, outStart
			mov				edi, outLnLength
			mov				esi, 8
			pxor			mm7, mm7
GB8x8InBoundLnStartE:
			MacroGBInBoundHoriInterp8WordAtMM1MM2
			// add 16 and shift 5, then pack it;
			paddsw			mm1, Mask16
			paddsw			mm2, Mask16
			psraw			mm1, 5
			psraw			mm2, 5
			packuswb		mm1, mm2
			movq			[edx], mm1
			add				edx, edi
			add				eax, ecx
			sub				esi, 1
			jnz				GB8x8InBoundLnStartE
			/* do vertical refPt = &refStart[y_pos-2][x_pos + (dxP == 1 ? 0 : 1)];*/
			mov				esi, refStart
			mov				eax, y_pos
			sub				eax, 2 /*y_pos - 2*/
			mov				esi, [esi + 4*eax] /*&refStart[y_pos-2]*/
			xor				edx, edx
			mov				eax, 1
			cmp				dxP, 1
			cmove			eax, edx /*(dxP == 1 ? 0 : 1)*/
			add				eax, x_pos /*x_pos + (dxP == 1 ? 0 : 1)*/
			add				eax, esi /*&refStart[y_pos-2] + x_pos + (dxP == 1 ? 0 : 1)*/
			mov				edx, outStart
			/*first line*/
			MacroGBInBoundVertInterp8WordAtMM2MM3FirstRow
			MacroGBInBoundVertInterp8ByteAvgSaveNInc /* eax at y_pos + 3 */
			add				eax, ecx /* eax at y_pos + 4 */
			//2th row: tmp64A(p-2), mm1(p-1), mm4(p0), mm6(p1), mm0(p2); eax at y_pos+4 row;
			MacroGBInBoundVertInterp8WordAtMM2MM3(mm1, mm4, mm6, mm0, tmp64A, tmp64B)
			MacroGBInBoundVertInterp8ByteAvgSaveNInc
			//3rd row: tmp64B(p-2), mm4(p-1), mm6(p0), mm0(p1), mm1(p2); eax at y_pos+5 row;
			MacroGBInBoundVertInterp8WordAtMM2MM3(mm4, mm6, mm0, mm1, tmp64B, tmp64A)
			MacroGBInBoundVertInterp8ByteAvgSaveNInc
			//4th row: tmp64A(p-2), mm6(p-1), mm0(p0), mm1(p1), mm4(p2); eax at y_pos+6 row;
			MacroGBInBoundVertInterp8WordAtMM2MM3(mm6, mm0, mm1, mm4, tmp64A, tmp64B)
			MacroGBInBoundVertInterp8ByteAvgSaveNInc
			//5th row: tmp64B(p-2), mm0(p-1), mm1(p0), mm4(p1), mm6(p2); eax at y_pos+7 row;
			MacroGBInBoundVertInterp8WordAtMM2MM3(mm0, mm1, mm4, mm6, tmp64B, tmp64A)
			MacroGBInBoundVertInterp8ByteAvgSaveNInc
			//6th row: tmp64A(p-2), mm1(p-1), mm4(p0), mm6(p1), mm0(p2); eax at y_pos+8 row;
			MacroGBInBoundVertInterp8WordAtMM2MM3(mm1, mm4, mm6, mm0, tmp64A, tmp64B)
			MacroGBInBoundVertInterp8ByteAvgSaveNInc
			//7rd row: tmp64B(p-2), mm4(p-1), mm6(p0), mm0(p1), mm1(p2); eax at y_pos+9 row;
			MacroGBInBoundVertInterp8WordAtMM2MM3(mm4, mm6, mm0, mm1, tmp64B, tmp64A)
			MacroGBInBoundVertInterp8ByteAvgSaveNInc
			//8th row: tmp64A(p-2), mm6(p-1), mm0(p0), mm1(p1), mm4(p2); eax at y_pos+10 row;
			MacroGBInBoundVertInterp8WordAtMM2MM3(mm6, mm0, mm1, mm4, tmp64A, tmp64B)
			MacroGBInBoundVertInterp8ByteAvgSaveNInc
		}
	}
	else if (dxP == 2) {  /* Vertical & horizontal interpolation */
		// borrow int img->m7[16][16], use as a short pointer for faster MMX store and load;
		avdInt16 *tmp_res = (avdInt16 *)&img->m7[0][0];
		__int64 avg1, avg3;
		avg1 = (dyP == 1) ? ~0 : 0;
		avg3 = (dyP == 3) ? ~0 : 0;
		refPt = &refStart[y_pos-2][x_pos - 2];
		_asm{
			mov				edx, 13
			mov				esi, tmp_res
			mov				eax, refPt
			mov				ecx, refWidth
			pxor			mm7, mm7
GB8x8InBoundLnStartB:
			MacroGBInBoundHoriInterp8WordAtMM1MM2
			movq			[esi], mm1
			movq			[esi + 8], mm2
			add				esi, 16 /*every 8*sizeof(short) = 16 bytes as a row;*/
			add				eax, ecx
			sub				edx, 1
			jnz				GB8x8InBoundLnStartB;
		}

		// Notice that due the possible overflow, mmx could only process two data at once,
		// this MMX code is not faster than C code using 32 registers;
#if 1
		for (j = 0; j < 8; j++) {
			for (i = 0; i < 8; i++) {
				middle = tmp_res[((j+1)<<3)+i] + tmp_res[((j+4)<<3)+i] -
					((tmp_res[((j+2)<<3)+i] + tmp_res[((j+3)<<3)+i])<<2);
				out[i] = clp255[(tmp_res[(j<<3)+i] + tmp_res[((j+5)<<3)+i] -
					middle - (middle<<2) + 512)>>10];
			} 
			out += outLnLength;
		}
#else 1		
		_asm {
			mov				edi, outLnLength
			// start vertical;
			mov				edx, outStart
			mov				eax, tmp_res

			// for low 4 bytes; first row;
			movq			mm1, [eax + 32] // p0
			movq			mm4, [eax + 48] // p1
			movq			mm0, [eax + 16]// p(-1)
			movq			mm6, [eax + 64]// p(2)
			MacroInBoundVertInterp4WordIn(mm5, mm0, mm1, mm4, mm6)
			/*mm0(p(-2)), mm1(p(-1)), mm4(p0), mm6(p1), mm5(p2)*/
			MacroInBoundVertInterp4WordIn(mm0, mm1, mm4, mm6, mm5)
			MacroInBoundVertInterp4WordIn(mm1, mm4, mm6, mm5, mm0)
			MacroInBoundVertInterp4WordIn(mm4, mm6, mm5, mm0, mm1)
			
			MacroInBoundVertInterp4WordIn(mm6, mm5, mm0, mm1, mm4)
			MacroInBoundVertInterp4WordIn(mm5, mm0, mm1, mm4, mm6)
			MacroInBoundVertInterp4WordIn(mm0, mm1, mm4, mm6, mm5)
			MacroInBoundVertInterp4WordIn(mm1, mm4, mm6, mm5, mm0)

			mov				edx, outStart
			mov				eax, tmp_res
			add				edx, 4
			add				eax, 8
			// for low 4 bytes; first row;
			movq			mm1, [eax + 32] // p0
			movq			mm4, [eax + 48] // p1
			movq			mm0, [eax + 16]// p(-1)
			movq			mm6, [eax + 64]// p(2)
			MacroInBoundVertInterp4WordIn(mm5, mm0, mm1, mm4, mm6)
			MacroInBoundVertInterp4WordIn(mm0, mm1, mm4, mm6, mm5)
			MacroInBoundVertInterp4WordIn(mm1, mm4, mm6, mm5, mm0)
			MacroInBoundVertInterp4WordIn(mm4, mm6, mm5, mm0, mm1)
			
			MacroInBoundVertInterp4WordIn(mm6, mm5, mm0, mm1, mm4)
			MacroInBoundVertInterp4WordIn(mm5, mm0, mm1, mm4, mm6)
			MacroInBoundVertInterp4WordIn(mm0, mm1, mm4, mm6, mm5)
			MacroInBoundVertInterp4WordIn(mm1, mm4, mm6, mm5, mm0)
		}
#endif 1

		if (dyP&1) {
			tmpResJ = &tmp_res[(2 + (dyP>>1))<<3];
			_asm{
				mov				eax, tmpResJ
				mov				ecx, 4
				mov				edx, outStart
				mov				edi, outLnLength
GB8x8InBoundLnStartC:
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
				jnz				GB8x8InBoundLnStartC
			}
		}
	}
	else {  /* Horizontal & vertical interpolation */
		// borrow int img->m7[16][16], use as a short pointer for faster MMX store and load;
		avdInt16 *tmp_res = (avdInt16 *)&img->m7[0][0];
		__int64 tmp64A, tmp64B;
		refPt = &refStart[y_pos - 2][x_pos - 2];
		_asm {
			mov				edx, tmp_res
			mov				eax, refPt
			MacroGBInBoundVertInterp8WordAtMM2MM3FirstRow
			movq			[edx], mm2
			movq			[edx + 8], mm3
			add				edx, 32 /* 16 * sizeof(short) as a row*/
			lea				eax, [eax + 2*ecx] // eax at y_pos+4 row;
			// the code in the macro is the same as the first line above, except data loading;
			// To debug the macro, we can either debug the first line code above or copy and
			// roll out macro one by one; Or it may be able to get into macro in debug mode;
			//2th row: tmp64A(p-2), mm1(p-1), mm4(p0), mm6(p1), mm0(p2); eax at y_pos+4 row;
			MacroGBInBoundVertInterp8WordAtMM2MM3(mm1, mm4, mm6, mm0, tmp64A, tmp64B)
			MacroGBInBoundVertInterp8WordSaveNInc
			//3rd row: tmp64B(p-2), mm4(p-1), mm6(p0), mm0(p1), mm1(p2); eax at y_pos+5 row;
			MacroGBInBoundVertInterp8WordAtMM2MM3(mm4, mm6, mm0, mm1, tmp64B, tmp64A)
			MacroGBInBoundVertInterp8WordSaveNInc
			//4th row: tmp64A(p-2), mm6(p-1), mm0(p0), mm1(p1), mm4(p2); eax at y_pos+6 row;
			MacroGBInBoundVertInterp8WordAtMM2MM3(mm6, mm0, mm1, mm4, tmp64A, tmp64B)
			MacroGBInBoundVertInterp8WordSaveNInc
			//5th row: tmp64B(p-2), mm0(p-1), mm1(p0), mm4(p1), mm6(p2); eax at y_pos+7 row;
			MacroGBInBoundVertInterp8WordAtMM2MM3(mm0, mm1, mm4, mm6, tmp64B, tmp64A)
			MacroGBInBoundVertInterp8WordSaveNInc
			//6th row: tmp64A(p-2), mm1(p-1), mm4(p0), mm6(p1), mm0(p2); eax at y_pos+8 row;
			MacroGBInBoundVertInterp8WordAtMM2MM3(mm1, mm4, mm6, mm0, tmp64A, tmp64B)
			MacroGBInBoundVertInterp8WordSaveNInc
			//7rd row: tmp64B(p-2), mm4(p-1), mm6(p0), mm0(p1), mm1(p2); eax at y_pos+9 row;
			MacroGBInBoundVertInterp8WordAtMM2MM3(mm4, mm6, mm0, mm1, tmp64B, tmp64A)
			MacroGBInBoundVertInterp8WordSaveNInc
			//8th row: tmp64A(p-2), mm6(p-1), mm0(p0), mm1(p1), mm4(p2); eax at y_pos+10 row;
			MacroGBInBoundVertInterp8WordAtMM2MM3(mm6, mm0, mm1, mm4, tmp64A, tmp64B)
			movq			[edx], mm2
			movq			[edx + 8], mm3

			/* be careful, load 4 bytes only; when loading 8 bytes, refPt may go out of bound*/
			mov				edx, tmp_res
			mov				eax, refPt
			add				edx, 16
			add				eax, 8
			// first row;
			movd			mm1, [eax + 2*ecx] // p0
			movd			mm4, [eax + esi]   // p1
			punpcklbw		mm1, mm7 // 0L
			punpcklbw		mm4, mm7 // 1L
			movq			mm2, mm1
			paddw			mm2, mm4 // L: p0+p1
			psllq			mm2, 2   // L: 4*(p0+p1)
			movd			mm0, [eax + ecx]// p(-1)
			movd			mm6, [eax + 4*ecx]// p(2)
			punpcklbw		mm0, mm7 // -1L
			punpcklbw		mm6, mm7 // 2L
			movq			mm3, mm0
			paddw			mm3, mm6
			psubsw			mm2, mm3 // L: -p(-1) + 4*(p0+p1) - p2
			movq			mm3, mm2
			psllw			mm3, 2
			paddsw			mm2, mm3 // L: -5*p(-1) + 20*(p0+p1) - 5*p2
			movd			mm5, [eax] // p(-2)
			lea				eax, [eax + 4*ecx] // eax at y_pos+2 row;
			punpcklbw		mm5, mm7 // -2L
			paddsw			mm2, mm5 // L: p(-2) - 5*p(-1) + 20*(p0+p1) - 5*p2
			movd			mm5, [eax + ecx] // load p3;
			punpcklbw		mm5, mm7 // +3L
			paddsw			mm2, mm5 // L: p(-2) - 5*p(-1) + 20*(p0+p1) - 5*p2 + p3
			movq			[edx], mm2
			add				eax, ecx // eax at y_pos+3 row; 

			/*m0(p(-2)), m1(p-1), m4(p0), m6(p1), m5(p3) */
			MacroGBInBoundVertInterp4WordOut(mm0, mm1, mm4, mm6, mm5)
			MacroGBInBoundVertInterp4WordOut(mm1, mm4, mm6, mm5, mm0)
			MacroGBInBoundVertInterp4WordOut(mm4, mm6, mm5, mm0, mm1)

			MacroGBInBoundVertInterp4WordOut(mm6, mm5, mm0, mm1, mm4)
			MacroGBInBoundVertInterp4WordOut(mm5, mm0, mm1, mm4, mm6)
			MacroGBInBoundVertInterp4WordOut(mm0, mm1, mm4, mm6, mm5)
			MacroGBInBoundVertInterp4WordOut(mm1, mm4, mm6, mm5, mm0)

		}
		// last one use C code to do it;
		pres_y = y_pos;
		pres_x = x_pos + 10;
		for (j = 0; j < 8; j++) {
			middle = refStart[pres_y-1][pres_x] + refStart[pres_y+2][pres_x] -
				((refStart[pres_y][pres_x] + refStart[pres_y+1][pres_x])<<2);
			tmp_res[(j<<4)+12] = refStart[pres_y-2][pres_x] + 
				refStart[pres_y+3][pres_x] - middle - (middle<<2);
			pres_y++;
		}
		// TBD: try MMX assembly;
		for (j = 0; j < 8; j++) {
			tmpResJ = &tmp_res[j<<4];
			for (i = 0; i < 8; i++) {
				middle = tmpResJ[i+4] + tmpResJ[i+1] -
					((tmpResJ[i+2] + tmpResJ[i+3])<<2);
				out[i] = clp255[(tmpResJ[i] + tmpResJ[i+5] -
					middle - (middle<<2) + 512)>>10];
			}
			out += outLnLength;
		}
#if 1
		if (dxP&1) {
			pres_x = 2+(dxP>>1);
			for (j = 0; j < 8; j++){
				tmpResJ = &tmp_res[j<<4];
				for (i = 0; i < 8; i++)
					outStart[i] = (outStart[i] + clp255[(tmpResJ[pres_x+i]+16)>>5] + 1)>>1;
				outStart += outLnLength;
			}
		}
#else 1
		if (dxP&1) {
			// TBD: tmp_res[2+(dxP>>1)] is never 8 bytes aligned;
			// need a better way to do this;
			tmpResJ = &tmp_res[2+(dxP>>1)];
			_asm{
				mov				eax, tmpResJ
				mov				ecx, 4
				mov				edx, outStart
				mov				edi, outLnLength
GB8x8InBoundLnStartD:
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
				movq			mm0, [eax + 32] //0-3
				movq			mm1, [eax + 40] //4-7
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
				jnz				GB8x8InBoundLnStartD
			}
		}
#endif 1
	}
}	

