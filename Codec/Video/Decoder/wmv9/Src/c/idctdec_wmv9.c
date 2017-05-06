//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) 2002  Microsoft Corporation

Module Name:

	idct_wmv3.cpp

Abstract:

	Inverse transform routines used by WMV3 codec

Author:

	Sridhar Srinivasan (sridhsri@microsoft.com)

Revision History:

*************************************************************************/
#include "xplatform_wmv.h"
#include "typedef.h"
#include "wmvdec_function.h"
#include "idctdec_wmv2.h"

#ifndef DISABLE_UNUSED_CODE_IDCT

#define  W0   12
#define  W1   16
#define  W3   15
#define  W5   9
#define  W7   4

#define  W1_W7   12
#define  W1pW7   20
#define  W3_W5   6
#define  W3pW5   24
 
#define  W1a   22
#define  W2a   17
#define  W3a   10

#define  W2   16
#define  W2A  16
#define  W6     6
#define  W2pW6   22
#define  W2_W6  10

#ifndef _EMB_SSIMD_IDCT_

#ifdef _12BITCOEFCLIP
    void clipDummy ()
    {
    }

    Void_WMV ClipCoef32 (U32_WMV b, U32_WMV n, I32_WMV *p)
    {
        int i;
    
        if (b>=32)
            return;
        
        for (i=0; i<n; i++)
        {
            I32_WMV pOrig = p[i];
            p[i] = (p[i]<<(32-b))>>(32-b);
            if (p[i] != pOrig)
                clipDummy ();
        }
    }

    Void_WMV ClipCoef16 (U32_WMV b, U32_WMV n, I16_WMV *p)
    {
        int i;
    
        if (b >= 16)
            return;
        
        for (i=0; i<n; i++)
        {
            I16_WMV pOrig = p[i];
            p[i] = (p[i]<<(16-b))>>(16-b);
            if (p[i] != pOrig)
                clipDummy ();
        }
    }
#endif

//#ifdef NEW_TRANSFORM

///////////////////////////////////////////////////////////////////////////
//
// Intra IDCT Functions
//
///////////////////////////////////////////////////////////////////////////
// C Version
// Input: 32-bit
// Output: 8-bit
#define SATURATE8(x)   ((x) < 0 ? 0 : ((x) > 255 ? 255 : (x)))

Void_WMV g_IDCTDec_WMV3 (U8_WMV __huge* piDst, I32_WMV iOffsetToNextRowForDCT, const I32_WMV __huge* rgiCoefRecon)
{
    I16_WMV x0, x1, x2, x3, x4, x5, x6, x7, x8;
    I16_WMV y3, y4, y5, y4a, x4a, x5a;
    I16_WMV  rgblk[64];
    I16_WMV* blk = rgblk;
    U8_WMV __huge * blk0 ;
	U8_WMV __huge * blk1 ;
	U8_WMV __huge * blk2 ;
	U8_WMV __huge * blk3 ;
	U8_WMV __huge * blk4 ;
	U8_WMV __huge * blk5 ;
	U8_WMV __huge * blk6 ;
	U8_WMV __huge * blk7 ;
	I16_WMV* pBlk0;
	I16_WMV* pBlk1;
	I16_WMV* pBlk2;
	I16_WMV* pBlk3;
	I16_WMV* pBlk4;
	I16_WMV* pBlk5;
	I16_WMV* pBlk6;
	I16_WMV* pBlk7;
    I32_WMV  i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV3);

    COVERAGE_COEF_INPUT_BITS (BLOCK_SIZE*BLOCK_SIZE, (U32_WMV*)(rgiCoefRecon));
    CLIPCOEF32 (12, BLOCK_SIZE*BLOCK_SIZE, (U32_WMV*)(rgiCoefRecon));

    for (i = 0; i < BLOCK_SIZE; i++, blk += BLOCK_SIZE) {
		x4 = (I16_WMV)rgiCoefRecon [1*8 + i];		
		x3 = (I16_WMV)rgiCoefRecon [2*8 + i];
		x7 = (I16_WMV)rgiCoefRecon [3*8 + i];
		x1 = (I16_WMV)rgiCoefRecon [4*8 + i] * W0;
		x6 = (I16_WMV)rgiCoefRecon [5*8 + i];
		x2 = (I16_WMV)rgiCoefRecon [6*8 + i];		
		x5 = (I16_WMV)rgiCoefRecon [7*8 + i];
        x0 = (I16_WMV)rgiCoefRecon [0*8 + i] * W0 + 4; /* for proper rounding */

		COVERAGE_COEF_INTERNAL_BITS (x4);
		COVERAGE_COEF_INTERNAL_BITS (x3);
		COVERAGE_COEF_INTERNAL_BITS (x7);
		COVERAGE_COEF_INTERNAL_BITS (x1);
		COVERAGE_COEF_INTERNAL_BITS (x6);
		COVERAGE_COEF_INTERNAL_BITS (x2);
		COVERAGE_COEF_INTERNAL_BITS (x5);
		COVERAGE_COEF_INTERNAL_BITS (x0);

        // zeroth stage
        y3 = x4 + x5;
		x8 = W3 * y3;
		x4a = x8 - W3pW5 * x5;
		x5a = x8 - W3_W5 * x4;
		
		COVERAGE_COEF_INTERNAL_BITS (y3);
		COVERAGE_COEF_INTERNAL_BITS (x8);
		COVERAGE_COEF_INTERNAL_BITS (x4a);
		COVERAGE_COEF_INTERNAL_BITS (x5a);
		
		x8 = W7 * y3;
		x4 = x8 + W1_W7 * x4;
		x5 = x8 - W1pW7 * x5;

		COVERAGE_COEF_INTERNAL_BITS (x8);
		COVERAGE_COEF_INTERNAL_BITS (x4);
		COVERAGE_COEF_INTERNAL_BITS (x5);

		// first stage
        y3 = x6 + x7;
		x8 = W7 * y3;
		x4a -= x8 + W1_W7 * x6;
		x5a += x8 - W1pW7 * x7;

		COVERAGE_COEF_INTERNAL_BITS (y3);
		COVERAGE_COEF_INTERNAL_BITS (x8);
		COVERAGE_COEF_INTERNAL_BITS (x4a);
		COVERAGE_COEF_INTERNAL_BITS (x5a);
		
		x8 = W3 * y3;
		x4 += x8 - W3_W5 * x6;
		x5 += x8 - W3pW5 * x7;

		COVERAGE_COEF_INTERNAL_BITS (x8);
		COVERAGE_COEF_INTERNAL_BITS (x4);
		COVERAGE_COEF_INTERNAL_BITS (x5);

		// second stage 
		x8 = x0 + x1;
		x0 -= x1;

		COVERAGE_COEF_INTERNAL_BITS (x8);
		COVERAGE_COEF_INTERNAL_BITS (x0);

		x1 = x2;
		x2 = W6 * x3 - W2 * x2;  // simplify?
		x3 = W6 * x1 + W2A * x3;

		COVERAGE_COEF_INTERNAL_BITS (x1);
		COVERAGE_COEF_INTERNAL_BITS (x2);
		COVERAGE_COEF_INTERNAL_BITS (x3);

		// third stage
		x7 = x8 + x3;
		x8 -= x3;
		x3 = x0 + x2;
		x0 -= x2;

		COVERAGE_COEF_INTERNAL_BITS (x7);
		COVERAGE_COEF_INTERNAL_BITS (x8);
		COVERAGE_COEF_INTERNAL_BITS (x3);
		COVERAGE_COEF_INTERNAL_BITS (x0);

		// fourth stage
		blk [0] = (I16_WMV) ((x7 + x4) >> 3);
		blk [1] = (I16_WMV) ((x3 + x4a) >> 3);
		blk [2] = (I16_WMV) ((x0 + x5a) >> 3);
		blk [3] = (I16_WMV) ((x8 + x5) >> 3);
		blk [4] = (I16_WMV) ((x8 - x5) >> 3);
		blk [5] = (I16_WMV) ((x0 - x5a) >> 3);
		blk [6] = (I16_WMV) ((x3 - x4a) >> 3);
		blk [7] = (I16_WMV) ((x7 - x4) >> 3);
		
		COVERAGE_COEF_INTERNAL_BITS (blk [0]);
		COVERAGE_COEF_INTERNAL_BITS (blk [1]);
		COVERAGE_COEF_INTERNAL_BITS (blk [2]);
		COVERAGE_COEF_INTERNAL_BITS (blk [3]);
		COVERAGE_COEF_INTERNAL_BITS (blk [4]);
		COVERAGE_COEF_INTERNAL_BITS (blk [5]);
		COVERAGE_COEF_INTERNAL_BITS (blk [6]);
		COVERAGE_COEF_INTERNAL_BITS (blk [7]);
		
	}

     blk0 = piDst;
	 blk1 = blk0 + iOffsetToNextRowForDCT;
	 blk2 = blk1 + iOffsetToNextRowForDCT;
	 blk3 = blk2 + iOffsetToNextRowForDCT;
	 blk4 = blk3 + iOffsetToNextRowForDCT;
	 blk5 = blk4 + iOffsetToNextRowForDCT;
	 blk6 = blk5 + iOffsetToNextRowForDCT;
	 blk7 = blk6 + iOffsetToNextRowForDCT;

	pBlk0 = rgblk;
	pBlk1 = pBlk0 + BLOCK_SIZE;
	pBlk2 = pBlk1 + BLOCK_SIZE;
	pBlk3 = pBlk2 + BLOCK_SIZE;
	pBlk4 = pBlk3 + BLOCK_SIZE;
	pBlk5 = pBlk4 + BLOCK_SIZE;
	pBlk6 = pBlk5 + BLOCK_SIZE;
	pBlk7 = pBlk6 + BLOCK_SIZE;

	for (i = 0; i < BLOCK_SIZE; i++){
        x0 = (*pBlk0++) * 6 + 32 /* rounding */;
		x1 = (*pBlk4++) * 6;
		x2 = (*pBlk6++);
		x3 = (*pBlk2++);
		x4 = (*pBlk1++);
		x5 = (*pBlk7++);
		x6 = (*pBlk5++);
		x7 = (*pBlk3++);

		COVERAGE_COEF_INTERNAL_BITS (x0);
		COVERAGE_COEF_INTERNAL_BITS (x1);
		COVERAGE_COEF_INTERNAL_BITS (x2);
		COVERAGE_COEF_INTERNAL_BITS (x3);
		COVERAGE_COEF_INTERNAL_BITS (x4);
		COVERAGE_COEF_INTERNAL_BITS (x5);
		COVERAGE_COEF_INTERNAL_BITS (x6);
		COVERAGE_COEF_INTERNAL_BITS (x7);

        // zeroth stage
        y4a = x4 + x5;
		x8 = 7 * y4a;
		x4a = x8 - 12 * x5;
		x5a = x8 - 3 * x4;

		COVERAGE_COEF_INTERNAL_BITS (y4a);
		COVERAGE_COEF_INTERNAL_BITS (x8);
		COVERAGE_COEF_INTERNAL_BITS (x4a);
		COVERAGE_COEF_INTERNAL_BITS (x5a);

		x8 = 2 * y4a;
		x4 = x8 + 6 * x4;
		x5 = x8 - 10 * x5;

		COVERAGE_COEF_INTERNAL_BITS (x8);
		COVERAGE_COEF_INTERNAL_BITS (x4);
		COVERAGE_COEF_INTERNAL_BITS (x5);

		// first stage
        y4 = x6 + x7;
		x8 = 2 * y4;
		x4a -= x8 + 6 * x6;
		x5a += x8 - 10 * x7;
		
		COVERAGE_COEF_INTERNAL_BITS (y4);
		COVERAGE_COEF_INTERNAL_BITS (x8);
		COVERAGE_COEF_INTERNAL_BITS (x4a);
		COVERAGE_COEF_INTERNAL_BITS (x5a);
		
		x8 = 7 * y4;
		x4 += x8 - 3 * x6;
		x5 += x8 - 12 * x7;

		COVERAGE_COEF_INTERNAL_BITS (x8);
		COVERAGE_COEF_INTERNAL_BITS (x4);
		COVERAGE_COEF_INTERNAL_BITS (x5);

		// second stage 
		x8 = x0 + x1;
		x0 -= x1;

		COVERAGE_COEF_INTERNAL_BITS (x8);
		COVERAGE_COEF_INTERNAL_BITS (x0);

// Change 1:

		x1 = 8 * (x2 + x3);
		x6 = x1 - 5 * x2;
		
		COVERAGE_COEF_INTERNAL_BITS (x1);
		COVERAGE_COEF_INTERNAL_BITS (x6);
		
		x1 -= 11 * x3;

		COVERAGE_COEF_INTERNAL_BITS (x1);


		// third stage
		x7 = x8 + x6;
		x8 -= x6;

		COVERAGE_COEF_INTERNAL_BITS (x7);
		COVERAGE_COEF_INTERNAL_BITS (x8);
		
// Change 2:

		x6 = x0 - x1;
		x0 += x1;

		COVERAGE_COEF_INTERNAL_BITS (x6);
		COVERAGE_COEF_INTERNAL_BITS (x0);

        // fractional part (guaranteed to have headroom)

// Change 3:

        y5 = y4 >> 1;
        y3 = y4a >> 1;

		COVERAGE_COEF_INTERNAL_BITS (y5);
		COVERAGE_COEF_INTERNAL_BITS (y3);

		// fourth stage
// Change 4:

        x4 += y5;
        x5 += y5;
        x4a += y3;
        x5a += y3;

		COVERAGE_COEF_INTERNAL_BITS (x4);
		COVERAGE_COEF_INTERNAL_BITS (x5);
		COVERAGE_COEF_INTERNAL_BITS (x4a);
		COVERAGE_COEF_INTERNAL_BITS (x5a);

//        if (iTest & ~255L) {
		    *blk0++ = SATURATE8((x7 + x4) >> 6);
		    *blk1++ = SATURATE8((x6 + x4a) >> 6);
		    *blk2++ = SATURATE8((x0 + x5a) >> 6);
		    *blk3++ = SATURATE8((x8 + x5) >> 6);
		    *blk4++ = SATURATE8((x8 - x5) >> 6);
		    *blk5++ = SATURATE8((x0 - x5a) >> 6);
		    *blk6++ = SATURATE8((x6 - x4a) >> 6);
		    *blk7++ = SATURATE8((x7 - x4) >> 6);
//        }

		COVERAGE_COEF_INTERNAL_BITS ((x7 + x4));
		COVERAGE_COEF_INTERNAL_BITS ((x6 + x4a));
		COVERAGE_COEF_INTERNAL_BITS ((x0 + x5a));
		COVERAGE_COEF_INTERNAL_BITS ((x8 + x5));
		COVERAGE_COEF_INTERNAL_BITS ((x8 - x5));
		COVERAGE_COEF_INTERNAL_BITS ((x6 - x4a));
		COVERAGE_COEF_INTERNAL_BITS ((x7 - x4));

    }
}


Void_WMV g_IDCTDec16_WMV3 (UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags)
{
    I16_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a;
    I16_WMV  y3, y4, y5, y4a;
    
    I16_WMV rgTempBlock[64];
    
    register I32_WMV iDCTVertFlag = 0;
    const I16_WMV __huge* rgiCoefRecon = piSrc->i16;
    I16_WMV __huge* blk = rgTempBlock;
    I32_WMV i;
    
    I16_WMV __huge *blk0 = piSrc->i16;
    I16_WMV __huge *blk1 = blk0 + 8;
    I16_WMV __huge *blk2 = blk1 + 8;
    I16_WMV __huge *blk3 = blk2 + 8;
    I16_WMV __huge *blk4 = blk3 + 8;
    I16_WMV __huge *blk5 = blk4 + 8;
    I16_WMV __huge *blk6 = blk5 + 8;
    I16_WMV __huge *blk7 = blk6 + 8;
    
    I16_WMV __huge *_blk0;
    I16_WMV __huge *_blk1;
    I16_WMV __huge *_blk2;
    I16_WMV __huge *_blk3;
    I16_WMV __huge *_blk4;
    I16_WMV __huge *_blk5;
    I16_WMV __huge *_blk6;
    I16_WMV __huge *_blk7;
    
    FUNCTION_PROFILE_DECL_START(fpDecode,IDCT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec16_WMV3);
    
    COVERAGE_COEF_INPUT_BITS16 (BLOCK_SIZE*BLOCK_SIZE, rgiCoefRecon);    
    CLIPCOEF16 (12, BLOCK_SIZE*BLOCK_SIZE, (I16_WMV __huge*)rgiCoefRecon);
    
    for ( i = 0; i < BLOCK_SIZE; i++) {
        // Check to see if this row has all zero coefficients in columns 1-7 (0 is first column)
        //          if (!(iDCTHorzFlags & (1 << i))) {
        //                // Columns 1-7 must be zero. Check to see if column zero coefficient is zero. 
        //                // If so, we don't have to do anything more for this row
        //                // Column zero cofficient is nonzero so all coefficients in this row are DC
        //                blk [0] = blk [1] = blk [2] = blk [3] = blk [4] = blk [5] = blk [6] = blk [7] = rgiCoefRecon [0] << 3;
        //                // If this is not the first row, tell the second stage that it can't use DC
        //                // shortcut - it has to do the full transform for all the columns
        //                iDCTVertFlag = i;
        //			continue;
        //		}
        
        // If this is not the first row, tell the second stage that it can't use DC shortcut 
        // - it has to do the full transform for all the columns
        iDCTVertFlag = i;
        x4 = blk1[i];		
        x3 = blk2[i];
        x7 = blk3[i];
        x1 = blk4[i] * W0;
        x6 = blk5[i];
        x2 = blk6[i];		
        x5 = blk7[i];
        x0 = blk0[i] * W0 + 4; /* for proper rounding */
        
        // zeroth stage
        y3 = x4 + x5;
        x8 = W3 * y3;
        x4a = x8 - W3pW5 * x5;
        x5a = x8 - W3_W5 * x4;
        x8 = W7 * y3;
        x4 = x8 + W1_W7 * x4;
        x5 = x8 - W1pW7 * x5;
        
        // first stage
        y3 = x6 + x7;
        x8 = W7 * y3;
        x4a -= x8 + W1_W7 * x6;
        x5a += x8 - W1pW7 * x7;
        x8 = W3 * y3;
        x4 += x8 - W3_W5 * x6;
        x5 += x8 - W3pW5 * x7;
        
        // second stage 
        x8 = x0 + x1;
        x0 -= x1;
        
        x1 = x2;
        x2 = W6 * x3 - W2 * x2;  // simplify?
        x3 = W6 * x1 + W2A * x3;
        
        // third stage
        x7 = x8 + x3;
        x8 -= x3;
        x3 = x0 + x2;
        x0 -= x2;
        
        // fourth stage
        *blk++ = (I16_WMV) ((x7 + x4) >> 3);
        *blk++ = (I16_WMV) ((x3 + x4a) >> 3);
        *blk++ = (I16_WMV) ((x0 + x5a) >> 3);
        *blk++ = (I16_WMV) ((x8 + x5) >> 3);
        *blk++ = (I16_WMV) ((x8 - x5) >> 3);
        *blk++ = (I16_WMV) ((x0 - x5a) >> 3);
        *blk++ = (I16_WMV) ((x3 - x4a) >> 3);
        *blk++ = (I16_WMV) ((x7 - x4) >> 3);
    }

    blk0 = rgTempBlock;//->i16;
    blk1 = blk0 + 8;
    blk2 = blk1 + 8;
    blk3 = blk2 + 8;
    blk4 = blk3 + 8;
    blk5 = blk4 + 8;
    blk6 = blk5 + 8;
    blk7 = blk6 + 8;
    
    _blk0 = piDst->i16;
    _blk1 = _blk0 + iOffsetToNextRowForDCT;
    _blk2 = _blk1 + iOffsetToNextRowForDCT;
    _blk3 = _blk2 + iOffsetToNextRowForDCT;
    _blk4 = _blk3 + iOffsetToNextRowForDCT;
    _blk5 = _blk4 + iOffsetToNextRowForDCT;
    _blk6 = _blk5 + iOffsetToNextRowForDCT;
    _blk7 = _blk6 + iOffsetToNextRowForDCT;
    
    for (i = 0; i < BLOCK_SIZE; i++){
        // If iDCTVertFlag is zero then we know that blk[1] thru blk[7] are zero.
        // Therefore, if blk[0] is zero we can skip to the next column. Otherwise we
        // can use the DC shortcut and just copy blk[0] to the rest of the column.
        //          if (iDCTVertFlag == 0){
        //                // If blk[0] is nonzero, copy the value to the other 7 rows in this column		
        //                if (blk0[i] != 0)
        //				blk0[i] = blk1[i] = blk2[i] = blk3[i] = blk4[i] = blk5[i] = blk6[i] = blk7[i] = (blk0[i] + 32) >> 6;
        //			continue;
        //		}
        x0 = blk0[i] * 6 + 32 /* rounding */;
        x1 = blk4[i] * 6;
        x2 = blk6[i];
        x3 = blk2[i];
        x4 = blk1[i];
        x5 = blk7[i];
        x6 = blk5[i];
        x7 = blk3[i];
        
        // zeroth stage
        y4a = x4 + x5;
        x8 = 7 * y4a;
        x4a = x8 - 12 * x5;
        x5a = x8 - 3 * x4;
        x8 = 2 * y4a;
        x4 = x8 + 6 * x4;
        x5 = x8 - 10 * x5;
        
        // first stage
        y4 = x6 + x7;
        x8 = 2 * y4;
        x4a -= x8 + 6 * x6;
        x5a += x8 - 10 * x7;
        x8 = 7 * y4;
        x4 += x8 - 3 * x6;
        x5 += x8 - 12 * x7;
        
        // second stage 
        x8 = x0 + x1;
        x0 -= x1;
        
        // Change 1:
        
        x1 = 8 * (x2 + x3);
        x6 = x1 - 5 * x2;
        x1 -= 11 * x3;
        
        // third stage
        x7 = x8 + x6;
        x8 -= x6;
        // Change 2:
        
        x6 = x0 - x1;
        x0 += x1;
        
        
        // fractional part (guaranteed to have headroom)
        // Change 3:
        
        y5 = y4 >> 1;
        y3 = y4a >> 1;
        
        // fourth stage
        // Change 4:
        
        x4 += y5;
        x5 += y5;
        x4a += y3;
        x5a += y3;
        
        _blk0 [i] = (x7 + x4) >> 6;
        _blk1 [i] = (x6 + x4a) >> 6;
        _blk2 [i] = (x0 + x5a) >> 6;
        _blk3 [i] = (x8 + x5) >> 6;
        _blk4 [i] = (x8 - x5) >> 6;
        _blk5 [i] = (x0 - x5a) >> 6;
        _blk6 [i] = (x6 - x4a) >> 6;
        _blk7 [i] = (x7 - x4) >> 6;
        
    }
    FUNCTION_PROFILE_STOP(&fpDecode);
    
}


Void_WMV g_8x4IDCTDec_WMV3 (UnionBuffer * piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer * rgiCoefReconBuf, I32_WMV iHalf)
{
    
    const I16_WMV __huge* rgiCoefRecon = rgiCoefReconBuf->i16;
    
    I16_WMV rgTemp[32];
    I16_WMV __huge* piDst = piDstBuf->i16 + (iHalf << 5);
    I16_WMV *blk = rgTemp;
    
    I16_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, y3, x4a, x5a;
    
    I16_WMV __huge *blk0 = (I16_WMV*)rgiCoefReconBuf->i16;
    I16_WMV __huge *blk1 = blk0 + 4;
    I16_WMV __huge *blk2 = blk1 + 4;
    I16_WMV __huge *blk3 = blk2 + 4;
    I16_WMV __huge *blk4 = blk3 + 4;
    I16_WMV __huge *blk5 = blk4 + 4;
    I16_WMV __huge *blk6 = blk5 + 4;
    I16_WMV __huge *blk7 = blk6 + 4;
    I32_WMV i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_8x4IDCTDec_WMV3);
    
    COVERAGE_COEF_INPUT_BITS16 (4*8, rgiCoefReconBuf->i16);
    CLIPCOEF16 (12, 4*8, (I16_WMV __huge*)rgiCoefReconBuf->i16);
    
    for ( i = 0; i < 4; i++, blk += 8)
        {
        x4 = blk1[i];		
        x3 = blk2[i];
        x7 = blk3[i];
        x1 = blk4[i] * W0;
        x6 = blk5[i];
        x2 = blk6[i];		
        x5 = blk7[i];
        x0 = blk0[i] * W0 + 4; /* for proper rounding */
        
        // zeroth stage
        y3 = x4 + x5;
        x8 = W3 * y3;
        x4a = x8 - W3pW5 * x5;
        x5a = x8 - W3_W5 * x4;
        x8 = W7 * y3;
        x4 = x8 + W1_W7 * x4;
        x5 = x8 - W1pW7 * x5;
        
        // first stage
        y3 = x6 + x7;
        x8 = W7 * y3;
        x4a -= x8 + W1_W7 * x6;
        x5a += x8 - W1pW7 * x7;
        x8 = W3 * y3;
        x4 += x8 - W3_W5 * x6;
        x5 += x8 - W3pW5 * x7;
        
        // second stage 
        x8 = x0 + x1;
        x0 -= x1;
        
        x1 = x2;
        x2 = W6 * x3 - W2 * x2;  // simplify?
        x3 = W6 * x1 + W2A * x3;
        
        // third stage
        x7 = x8 + x3;
        x8 -= x3;
        x3 = x0 + x2;
        x0 -= x2;
        
        // fourth stage
        blk [0] = (I16_WMV) ((x7 + x4) >> 3);
        blk [1] = (I16_WMV) ((x3 + x4a) >> 3);
        blk [2] = (I16_WMV) ((x0 + x5a) >> 3);
        blk [3] = (I16_WMV) ((x8 + x5) >> 3);
        blk [4] = (I16_WMV) ((x8 - x5) >> 3);
        blk [5] = (I16_WMV) ((x0 - x5a) >> 3);
        blk [6] = (I16_WMV) ((x3 - x4a) >> 3);
        blk [7] = (I16_WMV) ((x7 - x4) >> 3);
    }
    
    blk = rgTemp;
    
    blk0 = piDst;
    blk1 = blk0 + 8;
    blk2 = blk1 + 8;
    blk3 = blk2 + 8;
    
    for (i = 0; i < BLOCK_SIZE; i++,blk++) {
        x4 = blk[0];
        x5 = blk[8];
        x6 = blk[16];
        x7 = blk[24];
        
        x3 = (x4 - x6); 
        x6 += x4;
        
        x4 = 8 * x6 + 32; //rounding
        x8 = 8 * x3 + 32;   //rounding
        
        //simplify following to 3 multiplies
        x5a = 11 * x5 + 5 * x7;
        x5 = 5 * x5 - 11 * x7;
        
        x4 += (x6 >> 1); // guaranteed to have enough head room
        x8 += (x3 >> 1);
        
        blk0[i] = (I16_WMV) ((x4 + x5a) >> 6);
        blk1[i] = (I16_WMV) ((x8 + x5) >> 6);
        blk2[i] = (I16_WMV) ((x8 - x5) >> 6);
        blk3[i] = (I16_WMV) ((x4 - x5a) >> 6);
    } 
}


Void_WMV g_4x8IDCTDec_WMV3 (UnionBuffer * piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer * rgiCoefReconBuf, I32_WMV iHalf)
    {
    
    I32_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a;
    I32_WMV  y3, y4, y5, y4a;
    
    const I16_WMV __huge* rgiCoefRecon = rgiCoefReconBuf->i16;
    
    I16_WMV rgTemp[32];
    
    // const I32_WMV __huge* rgiCoefRecon = rgiCoefReconBuf->i32;
    I16_WMV __huge* piDst = piDstBuf->i16 + (iHalf*4);
    
    I16_WMV* blk = rgTemp;
    
    I16_WMV* blk0 = (short*)rgiCoefReconBuf;
    I16_WMV* blk1 = blk0 + 8;
    I16_WMV* blk2 = blk1 + 8;
    I16_WMV* blk3 = blk2 + 8;
    I32_WMV i;
    I16_WMV* blk4 ;
    I16_WMV* blk5 ;
    I16_WMV* blk6 ;
    I16_WMV* blk7 ;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_4x8IDCTDec_WMV3);
    COVERAGE_COEF_INPUT_BITS16 (8*4, rgiCoefReconBuf->i16);    

    CLIPCOEF16 (12, 8*4, (I16_WMV __huge*)rgiCoefReconBuf->i16);    
    
    for ( i = 0; i < BLOCK_SIZE; i++, blk += 4) {
        x4 = blk0[i];
        x5 = blk1[i];
        x6 = blk2[i];
        x7 = blk3[i];
        
        x0 = 17 * (x4 + x6) + 4; //rounding
        x1 = 17 * (x4 - x6) + 4; //rounding
        x8 = 10 * (x5 + x7);
        x2 = x8 + 12 * x5;
        x3 = x8 - 32 * x7;
        
        blk[0] = (I16_WMV)((x0 + x2) >> 3);
        blk[1] = (I16_WMV)((x1 + x3) >> 3);
        blk[2] = (I16_WMV)((x1 - x3) >> 3);
        blk[3] = (I16_WMV)((x0 - x2) >> 3);
    }
    
    blk = rgTemp;
    
    blk0 = piDst;
    blk1 = blk0 + iOffsetToNextRowForDCT;
    blk2 = blk1 + iOffsetToNextRowForDCT;
    blk3 = blk2 + iOffsetToNextRowForDCT;
    blk4 = blk3 + iOffsetToNextRowForDCT;
    blk5 = blk4 + iOffsetToNextRowForDCT;
    blk6 = blk5 + iOffsetToNextRowForDCT;
    blk7 = blk6 + iOffsetToNextRowForDCT;
    
    
    for (i = 0; i < 4; i++) {
        x0 = blk[i+(0*4)] * 6 + 32 /* rounding */;
        x1 = blk[i+(4*4)] * 6;
        x2 = blk[i+(6*4)];
        x3 = blk[i+(2*4)];
        x4 = blk[i+(1*4)];
        x5 = blk[i+(7*4)];
        x6 = blk[i+(5*4)];
        x7 = blk[i+(3*4)];
        
        // zeroth stage
        y4a = x4 + x5;
        x8 = 7 * y4a;
        x4a = x8 - 12 * x5;
        x5a = x8 - 3 * x4;
        x8 = 2 * y4a;
        x4 = x8 + 6 * x4;
        x5 = x8 - 10 * x5;
        
        // first stage
        y4 = x6 + x7;
        x8 = 2 * y4;
        x4a -= x8 + 6 * x6;
        x5a += x8 - 10 * x7;
        x8 = 7 * y4;
        x4 += x8 - 3 * x6;
        x5 += x8 - 12 * x7;
        
        // second stage 
        x8 = x0 + x1;
        x0 -= x1;
        
        // Change 1:
        
        x1 = 8 * (x2 + x3);
        x6 = x1 - 5 * x2;
        x1 -= 11 * x3;
        
        
        // third stage
        x7 = x8 + x6;
        x8 -= x6;
        // Change 2:
        
        x6 = x0 - x1;
        x0 += x1;
        
        
        // fractional part (guaranteed to have headroom)
        // Change 3:
        
        y5 = y4 >> 1;
        y3 = y4a >> 1;
        
        // fourth stage
        // Change 4:
        x4 += y5;
        x5 += y5;
        x4a += y3;
        x5a += y3;
        
        blk0 [i] = (x7 + x4) >> 6;
        blk1 [i] = (x6 + x4a) >> 6;
        blk2 [i] = (x0 + x5a) >> 6;
        blk3 [i] = (x8 + x5) >> 6;  
        blk4 [i] = (x8 - x5) >> 6;
        blk5 [i] = (x0 - x5a) >> 6;
        blk6 [i] = (x6 - x4a) >> 6;
        blk7 [i] = (x7 - x4) >> 6;
        
    }
}

#endif  //_EMB_SSIMD_IDCT_


#ifndef _EMB_SSIMD_IDCT_

// C Version
// Input:  32-bit (16 bits enough!)
// Output: 16-bit
//
// This transform uses MMX style transposed zigzag order the define _INPLACE_IDCT_ causes 
// this idct to use no intermediate buffer and uses the original zigzag order 
//
Void_WMV g_4x4IDCTDec_WMV3 (UnionBuffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iQuadrant)
{
    const I16_WMV* rgiCoefRecon = rgiCoefReconBuf->i16;
    I16_WMV* piDst = piDstBuf->i16 + (iQuadrant&2)*16 + (iQuadrant&1)*4;
//	I16_WMV * blk = piDst;
	I16_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x5a;
    I32_WMV i;
    I16_WMV rgTemp[32];
    I16_WMV* blk = rgTemp;   
    I16_WMV* blk0 = (short*)rgiCoefReconBuf;
    I16_WMV* blk1 = blk0 + 4;
    I16_WMV* blk2 = blk1 + 4;
    I16_WMV* blk3 = blk2 + 4;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_4x4IDCTDec_WMV3);

    COVERAGE_COEF_INPUT_BITS16 (4*4, rgiCoefRecon);    
    CLIPCOEF16 (12, 4*4, (I16_WMV __huge*)rgiCoefReconBuf->i16);    

	 assert(iOffsetToNextRowForDCT == 8);

    for ( i = 0; i < 4; i++, blk += 4){
        x4 = blk0[i];
        x5 = blk1[i];
        x6 = blk2[i];
        x7 = blk3[i];


        x0 = 17 * (x4 + x6) + 4; //rounding
        x1 = 17 * (x4 - x6) + 4; //rounding
        x8 = 10 * (x5 + x7);
        x2 = x8 + 12 * x5;
        x3 = x8 - 32 * x7;

        blk[0] = (I16_WMV)((x0 + x2) >> 3);
        blk[1] = (I16_WMV)((x1 + x3) >> 3);
        blk[2] = (I16_WMV)((x1 - x3) >> 3);
        blk[3] = (I16_WMV)((x0 - x2) >> 3);
    }


    blk = rgTemp;
    blk0 = piDst;
    blk1 = blk0 + 8;
    blk2 = blk1 + 8;
    blk3 = blk2 + 8;
    
    for (i = 0; i < 4; i++,blk++) {
        x4 = blk[0];
        x5 = blk[4];
        x6 = blk[8];
        x7 = blk[12];


        x3 = (x4 - x6); 
        x6 += x4;

        x4 = 8 * x6 + 32; //rounding
        x8 = 8 * x3 + 32;   //rounding
//simplify following to 3 multiplies
        x5a = 11 * x5 + 5 * x7;
        x5 = 5 * x5 - 11 * x7;

        x4 += (x6 >> 1); // guaranteed to have enough head room
        x8 += (x3 >> 1);
 
        blk0[i] = (I16_WMV) ((x4 + x5a) >> 6);
        blk1[i] = (I16_WMV) ((x8 + x5) >> 6);
        blk2[i] = (I16_WMV) ((x8 - x5) >> 6);
        blk3[i] = (I16_WMV) ((x4 - x5a) >> 6);
    } 
}

#endif

#endif //DISABLE_UNUSED_CODE_IDCT