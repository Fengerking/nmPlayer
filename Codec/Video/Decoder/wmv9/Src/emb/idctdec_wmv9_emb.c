//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
#include "xplatform_wmv.h"
#include "typedef.h"
#include "wmvdec_function.h"
#include "idctdec_wmv2.h"


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

#ifdef _EMB_HYBRID_16_32_IDCT_
#   define HYBRID_PASS_8   8
#   define HYBRID_PASS_4   4
#   define PASS_8          4
#   define PASS_4          2
#elif defined(_EMB_IDCT_SSIMD32_) || defined(_EMB_IDCT_SSIMD64_)
#   define PASS_8          4
#   define PASS_4          2
#   define HYBRID_PASS_8   4
#   define HYBRID_PASS_4   2
#else
#   define PASS_8          8
#   define PASS_4          4
#   define HYBRID_PASS_8   8
#   define HYBRID_PASS_4   4
#endif

///////////////////////////////////////////////////////////////////////////
//
// Intra IDCT Functions
//
///////////////////////////////////////////////////////////////////////////
// C Version
// Input: 32-bit
// Output: 8-bit

/* Try to use 16 bit implementation */
#ifdef _EMB_IDCT_SSIMD16_

#ifndef WMV_OPT_IDCT_ARM

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
    DEBUG_CACHE_READ_BYTES( rgiCoefRecon, BLOCK_SIZE*BLOCK_SIZE*4 );
    DEBUG_CACHE_WRITE_STRIDES( piDst, BLOCK_SIZE, iOffsetToNextRowForDCT, BLOCK_SIZE );

    for (i = 0; i < BLOCK_SIZE; i++, blk += BLOCK_SIZE) {
        x4 = (I16_WMV)rgiCoefRecon [1*8 + i];       
        x3 = (I16_WMV)rgiCoefRecon [2*8 + i];
        x7 = (I16_WMV)rgiCoefRecon [3*8 + i];
        x1 = (I16_WMV)rgiCoefRecon [4*8 + i] * W0;
        x6 = (I16_WMV)rgiCoefRecon [5*8 + i];
        x2 = (I16_WMV)rgiCoefRecon [6*8 + i];       
        x5 = (I16_WMV)rgiCoefRecon [7*8 + i];
        x0 = (I16_WMV)rgiCoefRecon [0*8 + i] * W0 + 4; /* for proper rounding */

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

        *blk0++ = SATURATE8((x7 + x4) >> 6);
        *blk1++ = SATURATE8((x6 + x4a) >> 6);
        *blk2++ = SATURATE8((x0 + x5a) >> 6);
        *blk3++ = SATURATE8((x8 + x5) >> 6);
        *blk4++ = SATURATE8((x8 - x5) >> 6);
        *blk5++ = SATURATE8((x0 - x5a) >> 6);
        *blk6++ = SATURATE8((x6 - x4a) >> 6);
        *blk7++ = SATURATE8((x7 - x4) >> 6);

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
    DEBUG_CACHE_READ_BYTES( piSrc->i16, BLOCK_SIZE*BLOCK_SIZE*2 );
    DEBUG_CACHE_WRITE_STRIDES( piDst->i16, BLOCK_SIZE*2, iOffsetToNextRowForDCT*2, BLOCK_SIZE );

    for ( i = 0; i < BLOCK_SIZE; i++){
        // Check to see if this row has all zero coefficients in columns 1-7 (0 is first column)
        //      if (!(iDCTHorzFlags & (1 << i))) {
        //          // Columns 1-7 must be zero. Check to see if column zero coefficient is zero. 
        //          // If so, we don't have to do anything more for this row
        //          // Column zero cofficient is nonzero so all coefficients in this row are DC
        //          blk [0] = blk [1] = blk [2] = blk [3] = blk [4] = blk [5] = blk [6] = blk [7] = rgiCoefRecon [0] << 3;
        //          // If this is not the first row, tell the second stage that it can't use DC
        //          // shortcut - it has to do the full transform for all the columns
        //          iDCTVertFlag = i;
        //          continue;
        //      }

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
        //  if (iDCTVertFlag == 0){
        //      // If blk[0] is nonzero, copy the value to the other 7 rows in this column      
        //      if (blk0[i] != 0)
        //          blk0[i] = blk1[i] = blk2[i] = blk3[i] = blk4[i] = blk5[i] = blk6[i] = blk7[i] = (blk0[i] + 32) >> 6;
        //      continue;
        //  }
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
    DEBUG_CACHE_READ_BYTES( rgiCoefRecon, BLOCK_SIZE*4*2 );
    DEBUG_CACHE_WRITE_STRIDES( piDst, 4*2, iOffsetToNextRowForDCT*2, BLOCK_SIZE );

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

        x4 = 8 * x6 + 32;   //rounding
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
    DEBUG_CACHE_READ_BYTES( rgiCoefRecon, BLOCK_SIZE*BLOCK_SIZE*2 );
    DEBUG_CACHE_WRITE_STRIDES( piDst, BLOCK_SIZE*2, iOffsetToNextRowForDCT*2, 4 );

    for ( i = 0; i < BLOCK_SIZE; i++, blk += 4){
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

    for (i = 0; i < 4; i++)
    {
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

Void_WMV g_4x4IDCTDec_WMV3 (UnionBuffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iQuadrant)
{
    const I16_WMV* rgiCoefRecon = rgiCoefReconBuf->i16;
    I16_WMV* piDst = piDstBuf->i16 + (iQuadrant&2)*16 + (iQuadrant&1)*4;
//  I16_WMV * blk = piDst;
    I16_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x5a;
    I32_WMV i;
    I16_WMV rgTemp[32];
    I16_WMV* blk = rgTemp;   
    I16_WMV* blk0 = (short*)rgiCoefReconBuf;
    I16_WMV* blk1 = blk0 + 4;
    I16_WMV* blk2 = blk1 + 4;
    I16_WMV* blk3 = blk2 + 4;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_4x4IDCTDec_WMV3);
    DEBUG_CACHE_READ_BYTES( rgiCoefRecon, 4*4*2 );
    DEBUG_CACHE_WRITE_STRIDES( piDst, 4*2, iOffsetToNextRowForDCT*2, 4 );

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

void SignPatch(I16_WMV * rgiCoefRecon, int len)
{
/*
    int i;
    I16_WMV *piSrcTmp = rgiCoefRecon;
    I16_WMV * dsttmp = rgiCoefRecon;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(SignPatch);
    
    for(i=0;i<(len/2);i+=2)
    {
        I16_WMV data_odd = piSrcTmp[i+1];
        
        *(I32_WMV *)(dsttmp + i) =  (I32_WMV) (piSrcTmp[i]);
        
        dsttmp[i+1]  +=  data_odd;
    }
*/
}



/* 
optimized C code against Ref code.
requirement: len mod 4 = 0
*/
void SignPatch32(I32_WMV * rgiCoefRecon, int len)
{
    /*
    int i;
    I32_WMV v1, v2;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(SignPatch32);
    
    for(i=0; i < (len >> 2); i++)
    {
        v1 = rgiCoefRecon[i*2];
        v2 = rgiCoefRecon[i*2+1];
        
        rgiCoefRecon[i] = (v1 & 0x0000ffff) | (((v1 >> 16) + v2) << 16);
    }
*/

}

#ifndef WMV_OPT_IDCT_SHX
///////////////////////////////////////////////////////////////////////////
// Inter IDCT Functions
// (this fn and one below are almost repeats of each other!!! - INEFFICIENT!)
///////////////////////////////////////////////////////////////////////////
// C Version
// Input:  16-bit
// Output: 16-bit

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec_WMV3_Pass1(const I32_WMV  *piSrc0, I32_WMV * blk32, const I32_WMV iNumLoops, I32_WMV iDCTHorzFlags)
{
    
    I16_WMV i;
    I16_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a;
    I16_WMV  y3,  b0;
    I16_WMV  *blk16 = (I16_WMV *)blk32;
    I16_WMV  *piSrc = (I16_WMV *)piSrc0;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV3_Pass1);
    DEBUG_CACHE_READ_BYTES( piSrc, BLOCK_SIZE*iNumLoops*2 );
    DEBUG_CACHE_WRITE_BYTES( blk16, BLOCK_SIZE*iNumLoops*2 );
    
    for ( i = 0; i < iNumLoops; i++,  blk16 += 8, iDCTHorzFlags >>= 1 ){
        // Check to see if this row has all zero coefficients in columns 1-7 (0 is first column)
        //        if (!(iDCTHorzFlags & (1 << i))) {
        //            // Columns 1-7 must be zero. Check to see if column zero coefficient is zero. 
        //            // If so, we don't have to do anything more for this row
        //            // Column zero cofficient is nonzero so all coefficients in this row are DC
        //            blk [0] = blk [1] = blk [2] = blk [3] = blk [4] = blk [5] = blk [6] = blk [7] = rgiCoefRecon [0] << 3;
        //            // If this is not the first row, tell the second stage that it can't use DC
        //            // shortcut - it has to do the full transform for all the columns
        //            iDCTVertFlag = i;
        //          continue;
        //      }
        
        if(!(iDCTHorzFlags&1))
        {
            assert(piSrc[ i +1*8 ]== 0);
            assert(piSrc[ i +2*8 ]== 0);
            assert(piSrc[ i +3*8 ]== 0);
            assert(piSrc[ i +4*8 ]== 0);
            assert(piSrc[ i +5*8 ]== 0);
            assert(piSrc[ i +6*8 ]== 0);
            assert(piSrc[ i +7*8 ]== 0);
            
            b0 = (piSrc[ i ]*W0 + 4)>>3; //12
            
            blk16[0] = b0;
            blk16[1] = b0;
            blk16[2] = b0;
            blk16[3] = b0;
            blk16[4] = b0;
            blk16[5] = b0;
            blk16[6] = b0;
            blk16[7] = b0;
            
            continue;
        }
        
        // If this is not the first row, tell the second stage that it can't use DC shortcut 
        // - it has to do the full transform for all the columns
        
        x4 = piSrc[ i +1*8 ];      
        x3 = piSrc[ i +2*8 ];
        x7 = piSrc[ i +3*8 ];
        x1 = piSrc[ i +4*8 ];
        x6 = piSrc[ i +5*8 ];
        x2 = piSrc[ i +6*8 ];      
        x5 = piSrc[ i +7*8 ];
        x0 = piSrc[ i +0*8 ]; /* for proper rounding */
        
        
        x1 = x1 * W0;   //12
        x0 = x0 * W0 + 4; /* for proper rounding */
        
        // zeroth stage
        y3 = x4 + x5;
        x8 = W3 * y3;           //15
        x4a = x8 - W3pW5 * x5;  //24
        x5a = x8 - W3_W5 * x4;  //6
        x8 = W7 * y3;           //4
        x4 = x8 + W1_W7 * x4;   //12
        x5 = x8 - W1pW7 * x5;   //20
        
        // first stage
        y3 = x6 + x7;
        x8 = W7 * y3;           //4
        x4a -= x8 + W1_W7 * x6; //12
        x5a += x8 - W1pW7 * x7; //20
        x8 = W3 * y3;           //15
        x4 += x8 - W3_W5 * x6;  //6
        x5 += x8 - W3pW5 * x7;  //24
        
        // second stage 
        x8 = x0 + x1;
        x0 -= x1;
        
        x1 = x2;
        x2 = W6 * x3 - W2 * x2;  //6,  16
        x3 = W6 * x1 + W2A * x3; //6,  16
        
        // third stage
        x7 = x8 + x3;
        x8 -= x3;
        x3 = x0 + x2;
        x0 -= x2;
        
        // fourth stage
        blk16 [0] = (I16_WMV) ((x7 + x4) >> 3);
        blk16 [1] = (I16_WMV) ((x3 + x4a) >> 3);
        blk16 [2] = (I16_WMV) ((x0 + x5a) >> 3);
        blk16 [3] = (I16_WMV) ((x8 + x5) >> 3);
        blk16 [4] = (I16_WMV) ((x8 - x5) >> 3);
        blk16 [5] = (I16_WMV) ((x0 - x5a) >> 3);
        blk16 [6] = (I16_WMV) ((x3 - x4a) >> 3);
        blk16 [7] = (I16_WMV) ((x7 - x4) >> 3);
    }
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec_WMV3_Pass2(const I32_WMV  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops)
{
    I16_WMV i, j;
    I16_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a;
    I16_WMV  y4, y4a;
    I16_WMV *piSrc = (I16_WMV *)piSrc0;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV3_Pass2);
    DEBUG_CACHE_READ_BYTES( piSrc, BLOCK_SIZE*iNumLoops*2 );
    DEBUG_CACHE_WRITE_BYTES( blk16, BLOCK_SIZE*iNumLoops*2 );

    for (i = 0; i < iNumLoops; i++){
        // If iDCTVertFlag is zero then we know that blk[1] thru blk[7] are zero.
        // Therefore, if blk[0] is zero we can skip to the next column. Otherwise we
        // can use the DC shortcut and just copy blk[0] to the rest of the column.
        //        if (iDCTVertFlag == 0){
        //            // If blk[0] is nonzero, copy the value to the other 7 rows in this column        
        //            if (blk0[i] != 0)
        //              blk0[i] = blk1[i] = blk2[i] = blk3[i] = blk4[i] = blk5[i] = blk6[i] = blk7[i] = (blk0[i] + 32) >> 6;
        //          continue;
        //      }
        x0 = piSrc[i + 0*8 ] * 6 + 32 /* rounding */;
        x1 = piSrc[i + 4*8 ] * 6;
        x2 = piSrc[i + 6*8 ];
        x3 = piSrc[i + 2*8 ];
        x4 = piSrc[i + 1*8 ];
        x5 = piSrc[i + 7*8 ];
        x6 = piSrc[i + 5*8 ];
        x7 = piSrc[i + 3*8 ];
        
        // zeroth stage
        y4a = x4 + x5;
        x8 = 7 * y4a;
        x4a = x8 - 12 * x5;
        x5a = x8 - 3 * x4;
        x8 = 2 * y4a;
        x4 = x8 + 6 * x4;
        x5 = x8 - 10 * x5;
        
        y4a = y4a >> 1;

        x4a += y4a;
        x5a += y4a;
        
        // first stage
        y4 = x6 + x7;
        x8 = 2 * y4;
        
        x4a -= x8 + 6 * x6;
        x5a += x8 - 10 * x7;
        x8 = 7 * y4;
        
        y4 = (y4 >> 1);
        //        x4 += y4;
        //        x5 += y4;
        x8 += y4;
        x4 += x8 - 3 * x6;
        x5 += x8 - 12 * x7;
        
        // second stage 
        x8 = x0 + x1;
        x0 -= x1;
        
        x1 = 8 * (x2 + x3);
        x6 = x1 - 5 * x2;
        x1 -= 11 * x3;
        
        // third stage
        x7 = x8 + x6;
        x8 -= x6;
        
        x6 = x0 - x1;
        x0 += x1;
        
        j = (i>>1) + ((i & 1) << 5);

        blk16 [j + 0*4] = (x7 + x4) >> 6;
        blk16 [j + 1*4] = (x6 + x4a) >> 6;
        blk16 [j + 2*4] = (x0 + x5a) >> 6;
        blk16 [j + 3*4] = (x8 + x5) >> 6;
        blk16 [j + 4*4] = (x8 - x5) >> 6;
        blk16 [j + 5*4] = (x0 - x5a) >> 6;
        blk16 [j + 6*4] = (x6 - x4a) >> 6;
        blk16 [j + 7*4] = (x7 - x4) >> 6;
    }
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec_WMV3_Pass3(const I32_WMV  *piSrc0, I32_WMV * blk32, const I32_WMV iNumLoops,  I32_WMV iDCTHorzFlags)
{
    I16_WMV i;
    I16_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8;
    I16_WMV  b0;
    I16_WMV *piSrc = (I16_WMV *)piSrc0;
    I16_WMV *blk16 = (I16_WMV *)blk32;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV3_Pass3);
    DEBUG_CACHE_READ_BYTES( piSrc, BLOCK_SIZE*iNumLoops*2 );
    DEBUG_CACHE_WRITE_BYTES( blk16, BLOCK_SIZE*iNumLoops*2 );
    
    for ( i = 0; i < iNumLoops; i++, blk16 += 8, iDCTHorzFlags >>= 1)
    {
        if(!(iDCTHorzFlags&1))
        {
            assert(piSrc[ i +1*8 ]== 0);
            assert(piSrc[ i +2*8 ]== 0);
            assert(piSrc[ i +3*8 ]== 0);
            
            
            b0 = (piSrc[ i ]*17 + 4) >> 3;
            
            blk16[0] = b0;
            blk16[1] = b0;
            blk16[2] = b0;
            blk16[3] = b0;
            
            continue;
            
        }
        
        x4 = piSrc[ i +0*8 ];      
        x5 = piSrc[ i +1*8 ];      
        x6 = piSrc[ i +2*8 ];
        x7 = piSrc[ i +3*8 ];
        
        
        x0 = 17 * (x4 + x6) + 4; //rounding
        x1 = 17 * (x4 - x6) + 4; //rounding
        x8 = 10 * (x5 + x7);
        x2 = x8 + 12 * x5;
        x3 = x8 - 32 * x7;
        
        blk16[0] = (I16_WMV)((x0 + x2) >> 3);
        blk16[1] = (I16_WMV)((x1 + x3) >> 3);
        blk16[2] = (I16_WMV)((x1 - x3) >> 3);
        blk16[3] = (I16_WMV)((x0 - x2) >> 3);
    }
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec_WMV3_Pass4(const I32_WMV  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops)
{
    I16_WMV  i, j;
    I16_WMV  x1, x3, x4, x5, x6, x7, x8,  x5a;

    I16_WMV  *piSrc = (I16_WMV  *)piSrc0;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV3_Pass4);
    DEBUG_CACHE_READ_BYTES( piSrc, BLOCK_SIZE*iNumLoops*2 );
    DEBUG_CACHE_WRITE_BYTES( blk16, BLOCK_SIZE*iNumLoops*2 );
    
    for (i = 0; i < iNumLoops; i++) 
    {
        x4 = piSrc[i + 0*8 ];
        x5 = piSrc[i + 1*8 ];
        x6 = piSrc[i + 2*8 ];
        x7 = piSrc[i + 3*8 ];
        
        
        x3 = (x4 - x6); 
        x6 += x4;
        
        x4 = 8 * x6 + 32; //rounding
        x8 = 8 * x3 + 32;   //rounding
        
        x6 = (x6 >> 1);
        x3 = (x3 >> 1);
        
        x4 += x6; // guaranteed to have enough head room
        x8 += x3 ;
        
        
        x1 = 5 * ( x5 + x7);
        x5a = x1 + 6 * x5;
        x5 =  x1 - 16 * x7;
        
        //simplify following to 3 multiplies
        //        x5a = 11 * x5 + 5 * x7;
        //        x5 = 5 * x5 - 11 * x7;
        
        j = (i>>1) + ((i&1)<<5);
        blk16[j+0*4] = (x4 + x5a) >> 6;
        blk16[j+1*4] = (x8 + x5) >> 6;
        blk16[j+2*4] = (x8 - x5) >> 6;
        blk16[j+3*4] = (x4 - x5a) >> 6;
    } 
}
#endif //WMV_OPT_IDCT_SHX

#endif //WMV_OPT_IDCT_ARM


#ifndef WMV_OPT_IDCT_ARM
//#pragma code_seg (EMBSEC_PML)
Void_WMV g_SubBlkIDCTClear_EMB (UnionBuffer * piDstBuf,  I32_WMV iIdx)
{
    I32_WMV iNumLoop = 8<<(iIdx>>7);
    I32_WMV iStep   =  1<< ((iIdx >>6)&0x1);
    I32_WMV iStep2 = iStep<<1;
    I32_WMV* piDst = piDstBuf->i32 + (iIdx&0x3f);
    I32_WMV i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_SubBlkIDCTClear_EMB);
    DEBUG_CACHE_WRITE_BYTES( piDst, BLOCK_SIZE*iNumLoops*4 );

    //(iNumLoop, iStep) = (8,1),(16,2), (8,2)
    for (i = 0; i < iNumLoop; i+=iStep2) 
    {
        piDst[i] = 0;
        piDst[i+16] = 0;
        piDst[i + iStep] = 0;
        piDst[i+16 + iStep] = 0;
    }
}
#endif //WMV_OPT_IDCT_ARM


#if !(defined(WMV_OPT_NAKED_ARM) && defined(WMV_OPT_DQUANT_ARM) && defined(WMV_OPT_IDCT_ARM))
#ifndef WMV_OPT_IDCT_SHX
//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec16_WMV3_SSIMD (UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags)
{
    I32_WMV  *piSrc0;
    const I16_WMV * rgiCoefRecon = piSrc->i16;
    I16_WMV * blk16 = piDst->i16;
    I32_WMV * blk32 = piDst->i32;    
    I32_WMV tmpBuffer[64];
    FUNCTION_PROFILE(fpDecode);
    FUNCTION_PROFILE_START(&fpDecode,IDCT_NT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec16_WMV3_SSIMD);
    
    piSrc0 = piSrc->i32;
    blk32 = tmpBuffer;
    
    g_IDCTDec_WMV3_Pass1(piSrc0, blk32, PASS_8, iDCTHorzFlags);
    
    piSrc0 = tmpBuffer;
    blk16  = piDst->i16;
    
    g_IDCTDec_WMV3_Pass2(piSrc0, blk16, PASS_8);
    
    FUNCTION_PROFILE_STOP(&fpDecode);
}
#endif //WMV_OPT_IDCT_SHX

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterError8x8_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ)
{
    UnionBuffer * ppxliErrorQMB = pMainLoop->m_rgiCoefReconBuf;
    I32_WMV result;
    I32_WMV *piSrc0 = ppxliErrorQMB->i32;
    I16_WMV *blk16  = ppxliErrorQMB->i16;  
    I32_WMV tmpBuffer[64];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterError8x8_SSIMD);
    
    result = DecodeInverseInterBlockQuantize16_EMB(pMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_8x8, 
        pDQ);
    
    g_IDCTDec_WMV3_Pass1(piSrc0, tmpBuffer, PASS_8, pMainLoop->m_iDCTHorzFlags);
    g_IDCTDec_WMV3_Pass2(tmpBuffer, blk16, PASS_8);
    
    return result;
}


//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterError8x4_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i)
{
    UnionBuffer * ppxliErrorQMB = pMainLoop->m_ppxliErrorQ;
    I32_WMV result;
    I32_WMV * piDst = ppxliErrorQMB->i32 + (i << 3);
    I16_WMV  * blk16;
    I32_WMV tmpBuffer[16];
    const I32_WMV  * piSrc0 = pMainLoop->m_rgiCoefReconBuf->i32;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterError8x4_SSIMD);
    
    result = DecodeInverseInterBlockQuantize16_EMB(pMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_8x4, 
        pDQ);
        
    g_IDCTDec_WMV3_Pass1(piSrc0, tmpBuffer, PASS_4, pMainLoop->m_iDCTHorzFlags);
        
    blk16  = (I16_WMV *)piDst;
    
    g_IDCTDec_WMV3_Pass4(tmpBuffer,  blk16, PASS_8);
    
    return result;
}


//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterError4x8_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i)
{
    UnionBuffer * ppxliErrorQMB = pMainLoop->m_ppxliErrorQ;
    I32_WMV result;
    I32_WMV * piDst = ppxliErrorQMB->i32 + (i);
    I16_WMV * blk16;
    I32_WMV tmpBuffer[4*2*4];
    const I32_WMV  * piSrc0 = pMainLoop->m_rgiCoefReconBuf->i32;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterError4x8_SSIMD);
    
    result = DecodeInverseInterBlockQuantize16_EMB(pMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_4x8, 
        pDQ);
    
    g_IDCTDec_WMV3_Pass3(piSrc0,  tmpBuffer, PASS_8, pMainLoop->m_iDCTHorzFlags);
    blk16  = (I16_WMV *)piDst;
    g_IDCTDec_WMV3_Pass2(tmpBuffer, blk16, PASS_4);
    
    return result;
}


//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterError4x4_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i)
{
    UnionBuffer * ppxliErrorQMB = pMainLoop->m_ppxliErrorQ;
    I32_WMV result;
    I16_WMV* piDst = ppxliErrorQMB->i16 + (i&2)*8 + (i&1)*2;
    I32_WMV  *piSrc0; 
    I32_WMV rgTemp[16];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterError4x4_SSIMD);
    
    result = DecodeInverseInterBlockQuantize16_EMB(pMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_4x4, 
        pDQ);
    
    piSrc0 = (I32_WMV *)pMainLoop->m_rgiCoefReconBuf->i32;
    g_IDCTDec_WMV3_Pass3(piSrc0,  rgTemp, PASS_4, pMainLoop->m_iDCTHorzFlags);
    
    //blk16  = piDst;
    g_IDCTDec_WMV3_Pass4(rgTemp,  piDst, PASS_4);
    
    return result;
}

#if 0
//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterErrorB8x8_SSIMD(EMB_BMainLoop  * pBMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ)
{
    UnionBuffer * ppxliErrorQMB = pBMainLoop->m_rgiCoefReconBuf;
    I32_WMV result;
    I32_WMV *piSrc0 = ppxliErrorQMB->i32;
    I16_WMV *blk16  = ppxliErrorQMB->i16;  
    I32_WMV tmpBuffer[64];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterErrorB8x8_SSIMD);
    
    result = DecodeInverseInterBlockQuantize16B_EMB(pBMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_8x8, 
        pDQ);
    
    g_IDCTDec_WMV3_Pass1(piSrc0, tmpBuffer, PASS_8, pBMainLoop->m_iDCTHorzFlags);
    g_IDCTDec_WMV3_Pass2(tmpBuffer, blk16, PASS_8);
    
    return result;
}


//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterErrorB8x4_SSIMD(EMB_BMainLoop  * pBMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i)
{
    UnionBuffer * ppxliErrorQMB = pBMainLoop->m_ppxliErrorQ;
    I32_WMV result;
    I32_WMV * piDst = ppxliErrorQMB->i32 + (i << 3);
    I16_WMV  * blk16;
    I32_WMV tmpBuffer[16];
    const I32_WMV  * piSrc0 = pBMainLoop->m_rgiCoefReconBuf->i32;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterErrorB8x4_SSIMD);
    
    result = DecodeInverseInterBlockQuantize16B_EMB(pBMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_8x4, 
        pDQ);
        
    g_IDCTDec_WMV3_Pass1(piSrc0, tmpBuffer, PASS_4, pBMainLoop->m_iDCTHorzFlags);
        
    blk16  = (I16_WMV *)piDst;
    
    g_IDCTDec_WMV3_Pass4(tmpBuffer,  blk16, PASS_8);
    
    return result;
}


//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterErrorB4x8_SSIMD(EMB_BMainLoop  * pBMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i)
{
    UnionBuffer * ppxliErrorQMB = pBMainLoop->m_ppxliErrorQ;
    I32_WMV result;
    I32_WMV * piDst = ppxliErrorQMB->i32 + (i);
    I16_WMV * blk16;
    I32_WMV tmpBuffer[4*2*4];
    const I32_WMV  * piSrc0 = pBMainLoop->m_rgiCoefReconBuf->i32;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterErrorB4x8_SSIMD);
    
    result = DecodeInverseInterBlockQuantize16B_EMB(pBMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_4x8, 
        pDQ);
    
    g_IDCTDec_WMV3_Pass3(piSrc0,  tmpBuffer, PASS_8, pBMainLoop->m_iDCTHorzFlags);
    blk16  = (I16_WMV *)piDst;
    g_IDCTDec_WMV3_Pass2(tmpBuffer, blk16, PASS_4);
    
    return result;
}


//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterErrorB4x4_SSIMD(EMB_BMainLoop  * pBMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i)
{
    UnionBuffer * ppxliErrorQMB = pBMainLoop->m_ppxliErrorQ;
    I32_WMV result;
    I16_WMV* piDst = ppxliErrorQMB->i16 + (i&2)*8 + (i&1)*2;
    I32_WMV  *piSrc0; 
    I32_WMV rgTemp[16];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterErrorB4x4_SSIMD);
    
    result = DecodeInverseInterBlockQuantize16B_EMB(pBMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_4x4, 
        pDQ);
    
    piSrc0 = (I32_WMV *)pBMainLoop->m_rgiCoefReconBuf->i32;
    g_IDCTDec_WMV3_Pass3(piSrc0,  rgTemp, PASS_4, pBMainLoop->m_iDCTHorzFlags);
    
    //blk16  = piDst;
    g_IDCTDec_WMV3_Pass4(rgTemp,  piDst, PASS_4);
    
    return result;
}
#endif
#endif  //!(defined(WMV_OPT_NAKED_ARM) && defined(WMV_OPT_DQUANT_ARM) && defined(WMV_OPT_IDCT_ARM))


#endif


#ifdef _EMB_IDCT_SSIMD32_

//I32_WMV g_dsttmp[256];

#ifndef WMV_OPT_IDCT_ARM

#define SATURATE8(x)   ((x) < 0 ? 0 : ((x) > 255 ? 255 : (x)))

Void_WMV g_IDCTDec_WMV3 (U8_WMV __huge* piDst, I32_WMV iOffsetToNextRowForDCT, const I32_WMV __huge* rgiCoefRecon)
{
    
    I32_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a;
    I32_WMV  y3, y4, y5, y4a, b0,c0,b1,c1,ls_signbit;
    I32_WMV  *piSrc0;
    I32_WMV i,j;
    register I32_WMV iDCTVertFlag = 0;
    //  const I16_WMV * rgiCoefRecon = piSrc->i16;
    //  I16_WMV * blk16 = piDst->i16;
    
    I32_WMV tmpBuffer[64];
    I32_WMV * blk32 = tmpBuffer; // piDst->i32;
    //  I16_WMV  dsttmp[64];
    
    U8_WMV __huge * blk0 ;
    U8_WMV __huge * blk1 ;
    U8_WMV __huge * blk2 ;
    U8_WMV __huge * blk3 ;
    U8_WMV __huge * blk4 ;
    U8_WMV __huge * blk5 ;
    U8_WMV __huge * blk6 ;
    U8_WMV __huge * blk7 ;
    
    FUNCTION_PROFILE(fpDecode);
    FUNCTION_PROFILE_START(&fpDecode,IDCT_NT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV3);
    DEBUG_CACHE_READ_BYTES( rgiCoefRecon, BLOCK_SIZE*BLOCK_SIZE*4 );
    DEBUG_CACHE_WRITE_STRIDES( piDst, BLOCK_SIZE, iOffsetToNextRowForDCT, BLOCK_SIZE );
  
    piSrc0 = (I32_WMV *) rgiCoefRecon;
    
    for ( i = 0; i < (BLOCK_SIZE>>1); i++,  blk32 += 8){
        // Check to see if this row has all zero coefficients in columns 1-7 (0 is first column)
        //        if (!(iDCTHorzFlags & (1 << i))) {
        //            // Columns 1-7 must be zero. Check to see if column zero coefficient is zero. 
        //            // If so, we don't have to do anything more for this row
        //            // Column zero cofficient is nonzero so all coefficients in this row are DC
        //            blk [0] = blk [1] = blk [2] = blk [3] = blk [4] = blk [5] = blk [6] = blk [7] = rgiCoefRecon [0] << 3;
        //            // If this is not the first row, tell the second stage that it can't use DC
        //            // shortcut - it has to do the full transform for all the columns
        //            iDCTVertFlag = i;
        //          continue;
        //      }
        
        // If this is not the first row, tell the second stage that it can't use DC shortcut 
        // - it has to do the full transform for all the columns
        
        x4 = piSrc0[ i +1*4 ];      
        x3 = piSrc0[ i +2*4 ];
        x7 = piSrc0[ i +3*4 ];
        x1 = piSrc0[ i +4*4 ];
        x6 = piSrc0[ i +5*4 ];
        x2 = piSrc0[ i +6*4 ];      
        x5 = piSrc0[ i +7*4 ];
        x0 = piSrc0[ i +0*4 ]; /* for proper rounding */
        
        
        x1 = x1 * W0;
        x0 = x0 * W0 + (4+(4<<16)); /* for proper rounding */
        
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
        /*
        blk [0] = (I16_WMV) ((x7 + x4) >> 3);
        blk [1] = (I16_WMV) ((x3 + x4a) >> 3);
        blk [2] = (I16_WMV) ((x0 + x5a) >> 3);
        blk [3] = (I16_WMV) ((x8 + x5) >> 3);
        blk [4] = (I16_WMV) ((x8 - x5) >> 3);
        blk [5] = (I16_WMV) ((x0 - x5a) >> 3);
        blk [6] = (I16_WMV) ((x3 - x4a) >> 3);
        blk [7] = (I16_WMV) ((x7 - x4) >> 3);
        */
        
        // blk [0,1]
        b0 = x7 + x4;
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x3 + x4a;
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[0] = (c0<<16) + b0;
        blk32[0+4] = (c1<<16) + b1;
        
        // blk [2,3]
        b0 = x0 + x5a;
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x8 + x5;
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[1] = (c0<<16) + b0;
        blk32[1+4] = (c1<<16) + b1;
        
        // blk [4,5]
        b0 = x8 - x5;
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x0 - x5a;
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[2] = (c0<<16) + b0;
        blk32[2+4] = (c1<<16) + b1;
        
        // blk [6,7]
        b0 = x3 - x4a;
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x7 - x4;
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[3] = (c0<<16) + b0;
        blk32[3+4] = (c1<<16) + b1;
        
    }
    
    piSrc0 = tmpBuffer; //piDst->i32;
    // blk16  = piDst->i16;
    
    blk0 = piDst;
    blk1 = blk0 + iOffsetToNextRowForDCT;
    blk2 = blk1 + iOffsetToNextRowForDCT;
    blk3 = blk2 + iOffsetToNextRowForDCT;
    blk4 = blk3 + iOffsetToNextRowForDCT;
    blk5 = blk4 + iOffsetToNextRowForDCT;
    blk6 = blk5 + iOffsetToNextRowForDCT;
    blk7 = blk6 + iOffsetToNextRowForDCT;
    
    for (i = 0; i < (BLOCK_SIZE>>1); i++){
        // If iDCTVertFlag is zero then we know that blk[1] thru blk[7] are zero.
        // Therefore, if blk[0] is zero we can skip to the next column. Otherwise we
        // can use the DC shortcut and just copy blk[0] to the rest of the column.
        //        if (iDCTVertFlag == 0){
        //            // If blk[0] is nonzero, copy the value to the other 7 rows in this column        
        //            if (blk0[i] != 0)
        //              blk0[i] = blk1[i] = blk2[i] = blk3[i] = blk4[i] = blk5[i] = blk6[i] = blk7[i] = (blk0[i] + 32) >> 6;
        //          continue;
        //      }
        x0 = piSrc0[i + 0*4 ] * 6 + 32 + (32<<16) /* rounding */;
        x1 = piSrc0[i + 4*4 ] * 6;
        x2 = piSrc0[i + 6*4 ];
        x3 = piSrc0[i + 2*4 ];
        x4 = piSrc0[i + 1*4 ];
        x5 = piSrc0[i + 7*4 ];
        x6 = piSrc0[i + 5*4 ];
        x7 = piSrc0[i + 3*4 ];
        
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
        
        x1 = 8 * (x2 + x3);
        x6 = x1 - 5 * x2;
        x1 -= 11 * x3;
        
        // third stage
        x7 = x8 + x6;
        x8 -= x6;
        
        x6 = x0 - x1;
        x0 += x1;
        
        /*
        y5 = y4 >> 1;
        y3 = y4a >> 1;
        */
        
        ls_signbit=y4&0x8000;
        y5 = (y4 >> 1) - ls_signbit;
        y5 = y5 & ~0x8000;
        y5 = y5 | ls_signbit;
        
        ls_signbit=y4a&0x8000;
        y3 = (y4a >> 1) - ls_signbit;
        y3 = y3 & ~0x8000;
        y3 = y3 | ls_signbit;
        
        // fourth stage
        // Change 4:
        
        x4 += y5;
        x5 += y5;
        x4a += y3;
        x5a += y3;
        
        /*
        
          _blk0 [i] = (x7 + x4) >> 6;
          _blk1 [i] = (x6 + x4a) >> 6;
          _blk2 [i] = (x0 + x5a) >> 6;
          _blk3 [i] = (x8 + x5) >> 6;
          _blk4 [i] = (x8 - x5) >> 6;
          _blk5 [i] = (x0 - x5a) >> 6;
          _blk6 [i] = (x6 - x4a) >> 6;
          _blk7 [i] = (x7 - x4) >> 6;
          
        */
        
        j = i<<1;
        
        // blk0
        b0 = (x7 + x4);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        //    blk16[ j + 0 + 0*8] = b0;
        //    blk16[ j + 1 + 0*8] = b1;
        blk0[ j ] = SATURATE8(b0);
        blk0[ j+1] = SATURATE8(b1);
        
        // blk1
        b0 = (x6 + x4a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        //   blk16[ j + 0 + 1*8] = b0;
        //   blk16[ j + 1 + 1*8] = b1;
        blk1[ j ] = SATURATE8(b0);
        blk1[ j+1] = SATURATE8(b1);
        
        // blk2
        b0 = (x0 + x5a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        //    blk16[ j + 0 + 2*8] = b0;
        //    blk16[ j + 1 + 2*8] = b1;
        blk2[ j ] = SATURATE8(b0);
        blk2[ j+1] = SATURATE8(b1);
        
        // blk3
        b0 = (x8 + x5);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        //      blk16[ j + 0 + 3*8] = b0;
        //      blk16[ j + 1 + 3*8] = b1;
        blk3[ j ] = SATURATE8(b0);
        blk3[ j+1] = SATURATE8(b1);
        
        // blk4
        b0 = (x8 - x5);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        //    blk16[ j + 0 + 4*8] = b0;
        //    blk16[ j + 1 + 4*8] = b1;
        blk4[ j ] = SATURATE8(b0);
        blk4[ j+1] = SATURATE8(b1);
        
        // blk5
        b0 = (x0 - x5a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        //   blk16[ j + 0 + 5*8] = b0;
        //    blk16[ j + 1 + 5*8] = b1;
        blk5[ j ] = SATURATE8(b0);
        blk5[ j+1] = SATURATE8(b1);
        
        // blk6
        b0 = (x6 - x4a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        //    blk16[ j + 0 + 6*8] = b0;
        //    blk16[ j + 1 + 6*8] = b1;
        blk6[ j ] = SATURATE8(b0);
        blk6[ j+1] = SATURATE8(b1);
        
        // blk7
        b0 = (x7 - x4);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        //    blk16[ j + 0 + 7*8] = b0;
        //    blk16[ j + 1 + 7*8] = b1;
        blk7[ j ] = SATURATE8(b0);
        blk7[ j+1] = SATURATE8(b1);
        
        
    }
    FUNCTION_PROFILE_STOP(&fpDecode);
    
    
    
}


///////////////////////////////////////////////////////////////////////////
// Inter IDCT Functions
// (this fn and one below are almost repeats of each other!!! - INEFFICIENT!)
///////////////////////////////////////////////////////////////////////////
// C Version
// Input:  16-bit
// Output: 16-bit
Void_WMV g_IDCTDec16_WMV3 (UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags)
{
    I32_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a;
    I32_WMV  y3, y4, y5, y4a, b0,c0,b1,c1,ls_signbit;
    I32_WMV  *piSrc0;
    I32_WMV i,j;
    register I32_WMV iDCTVertFlag = 0;
    const I16_WMV * rgiCoefRecon = piSrc->i16;
    I16_WMV * blk16 = piDst->i16;
    I32_WMV * blk32 = piDst->i32;
    I32_WMV tmpBuffer[64];
    
    FUNCTION_PROFILE(fpDecode);
    FUNCTION_PROFILE_START(&fpDecode,IDCT_NT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec16_WMV3);
    DEBUG_CACHE_READ_BYTES( rgiCoefRecon, BLOCK_SIZE*BLOCK_SIZE*2 );
    DEBUG_CACHE_WRITE_STRIDES( piDst, BLOCK_SIZE*2, iOffsetToNextRowForDCT*2, BLOCK_SIZE );
    
    piSrc0 = piSrc->i32;
    
    //  memcpy(dsttmp, piSrc->i32, 128);
    //  piSrc0 = (I32_WMV*) dsttmp;
    
    blk32 = tmpBuffer;
    
    for ( i = 0; i < (BLOCK_SIZE>>1); i++,  blk32 += 8){
        // Check to see if this row has all zero coefficients in columns 1-7 (0 is first column)
        //        if (!(iDCTHorzFlags & (1 << i))) {
        //            // Columns 1-7 must be zero. Check to see if column zero coefficient is zero. 
        //            // If so, we don't have to do anything more for this row
        //            // Column zero cofficient is nonzero so all coefficients in this row are DC
        //            blk [0] = blk [1] = blk [2] = blk [3] = blk [4] = blk [5] = blk [6] = blk [7] = rgiCoefRecon [0] << 3;
        //            // If this is not the first row, tell the second stage that it can't use DC
        //            // shortcut - it has to do the full transform for all the columns
        //            iDCTVertFlag = i;
        //          continue;
        //      }
        
        // If this is not the first row, tell the second stage that it can't use DC shortcut 
        // - it has to do the full transform for all the columns
        
        x4 = piSrc0[ i +1*4 ];      
        x3 = piSrc0[ i +2*4 ];
        x7 = piSrc0[ i +3*4 ];
        x1 = piSrc0[ i +4*4 ];
        x6 = piSrc0[ i +5*4 ];
        x2 = piSrc0[ i +6*4 ];      
        x5 = piSrc0[ i +7*4 ];
        x0 = piSrc0[ i +0*4 ]; /* for proper rounding */
        
        
        x1 = x1 * W0;
        x0 = x0 * W0 + (4+(4<<16)); /* for proper rounding */
        
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
        /*
        blk [0] = (I16_WMV) ((x7 + x4) >> 3);
        blk [1] = (I16_WMV) ((x3 + x4a) >> 3);
        blk [2] = (I16_WMV) ((x0 + x5a) >> 3);
        blk [3] = (I16_WMV) ((x8 + x5) >> 3);
        blk [4] = (I16_WMV) ((x8 - x5) >> 3);
        blk [5] = (I16_WMV) ((x0 - x5a) >> 3);
        blk [6] = (I16_WMV) ((x3 - x4a) >> 3);
        blk [7] = (I16_WMV) ((x7 - x4) >> 3);
        */
        
        // blk [0,1]
        b0 = x7 + x4;
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x3 + x4a;
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[0] = (c0<<16) + b0;
        blk32[0+4] = (c1<<16) + b1;
        
        // blk [2,3]
        b0 = x0 + x5a;
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x8 + x5;
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[1] = (c0<<16) + b0;
        blk32[1+4] = (c1<<16) + b1;
        
        // blk [4,5]
        b0 = x8 - x5;
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x0 - x5a;
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[2] = (c0<<16) + b0;
        blk32[2+4] = (c1<<16) + b1;
        
        // blk [6,7]
        b0 = x3 - x4a;
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x7 - x4;
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[3] = (c0<<16) + b0;
        blk32[3+4] = (c1<<16) + b1;
        
    }
    
    
    
    
    piSrc0 = tmpBuffer;
    
    blk16  = piDst->i16;
    
    for (i = 0; i < (BLOCK_SIZE>>1); i++){
        // If iDCTVertFlag is zero then we know that blk[1] thru blk[7] are zero.
        // Therefore, if blk[0] is zero we can skip to the next column. Otherwise we
        // can use the DC shortcut and just copy blk[0] to the rest of the column.
        //        if (iDCTVertFlag == 0){
        //            // If blk[0] is nonzero, copy the value to the other 7 rows in this column        
        //            if (blk0[i] != 0)
        //              blk0[i] = blk1[i] = blk2[i] = blk3[i] = blk4[i] = blk5[i] = blk6[i] = blk7[i] = (blk0[i] + 32) >> 6;
        //          continue;
        //      }
        x0 = piSrc0[i + 0*4 ] * 6 + 32 + (32<<16) /* rounding */;
        x1 = piSrc0[i + 4*4 ] * 6;
        x2 = piSrc0[i + 6*4 ];
        x3 = piSrc0[i + 2*4 ];
        x4 = piSrc0[i + 1*4 ];
        x5 = piSrc0[i + 7*4 ];
        x6 = piSrc0[i + 5*4 ];
        x7 = piSrc0[i + 3*4 ];
        
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
        
        x1 = 8 * (x2 + x3);
        x6 = x1 - 5 * x2;
        x1 -= 11 * x3;
        
        // third stage
        x7 = x8 + x6;
        x8 -= x6;
        
        x6 = x0 - x1;
        x0 += x1;
        
        /*
        y5 = y4 >> 1;
        y3 = y4a >> 1;
        */
        
        ls_signbit=y4&0x8000;
        y5 = (y4 >> 1) - ls_signbit;
        y5 = y5 & ~0x8000;
        y5 = y5 | ls_signbit;
        
        ls_signbit=y4a&0x8000;
        y3 = (y4a >> 1) - ls_signbit;
        y3 = y3 & ~0x8000;
        y3 = y3 | ls_signbit;
        
        // fourth stage
        // Change 4:
        
        x4 += y5;
        x5 += y5;
        x4a += y3;
        x5a += y3;
        
        /*
        
          _blk0 [i] = (x7 + x4) >> 6;
          _blk1 [i] = (x6 + x4a) >> 6;
          _blk2 [i] = (x0 + x5a) >> 6;
          _blk3 [i] = (x8 + x5) >> 6;
          _blk4 [i] = (x8 - x5) >> 6;
          _blk5 [i] = (x0 - x5a) >> 6;
          _blk6 [i] = (x6 - x4a) >> 6;
          _blk7 [i] = (x7 - x4) >> 6;
          
        */
        
        j = i<<1;
        
        // blk0
        b0 = (x7 + x4);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ j + 0 + 0*8] = b0;
        blk16[ j + 1 + 0*8] = b1;
        
        // blk1
        b0 = (x6 + x4a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ j + 0 + 1*8] = b0;
        blk16[ j + 1 + 1*8] = b1;
        
        // blk2
        b0 = (x0 + x5a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ j + 0 + 2*8] = b0;
        blk16[ j + 1 + 2*8] = b1;
        
        // blk3
        b0 = (x8 + x5);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ j + 0 + 3*8] = b0;
        blk16[ j + 1 + 3*8] = b1;
        
        // blk4
        b0 = (x8 - x5);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ j + 0 + 4*8] = b0;
        blk16[ j + 1 + 4*8] = b1;
        
        // blk5
        b0 = (x0 - x5a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ j + 0 + 5*8] = b0;
        blk16[ j + 1 + 5*8] = b1;
        
        // blk6
        b0 = (x6 - x4a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ j + 0 + 6*8] = b0;
        blk16[ j + 1 + 6*8] = b1;
        
        // blk7
        b0 = (x7 - x4);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ j + 0 + 7*8] = b0;
        blk16[ j + 1 + 7*8] = b1;
        
        
    }
    FUNCTION_PROFILE_STOP(&fpDecode);
    
}


// C Version
// Input:  32-bit (16 bits enough!)
// Output: 16-bit
Void_WMV g_8x4IDCTDec_WMV3 (UnionBuffer * piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer * rgiCoefReconBuf, I32_WMV iHalf)
{
    
    I32_WMV * piDst = piDstBuf->i32 + (iHalf << 4);
    I32_WMV *blk32 = piDst;
    I32_WMV i ;
    I32_WMV* blk0;
    I32_WMV* blk1;
    I32_WMV* blk2;
    I32_WMV* blk3;
    I32_WMV * blk32Next;
    const I32_WMV  * piSrc0 = rgiCoefReconBuf->i32;
    I32_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, y3, x4a, x5a, b0,c0,b1,c1,ls_signbit;
    I32_WMV iOffsetToNextRowForDCTHalf = iOffsetToNextRowForDCT>>1;
    FUNCTION_PROFILE(fpDecode);
    FUNCTION_PROFILE_START(&fpDecode,IDCT_NT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec16_WMV3);
    DEBUG_CACHE_READ_BYTES( piSrc0, BLOCK_SIZE*4*2 );
    DEBUG_CACHE_WRITE_STRIDES( piDst, BLOCK_SIZE*2, iOffsetToNextRowForDCT*2, 4 );
    
    piSrc0 = rgiCoefReconBuf->i32;

    for ( i = 0; i < 2; i++, blk32 += iOffsetToNextRowForDCT)
    {
        
        blk32Next = blk32 + iOffsetToNextRowForDCTHalf;
        
        x4 = piSrc0[ i +1*2 ];      
        x3 = piSrc0[ i +2*2 ];
        x7 = piSrc0[ i +3*2 ];
        x1 = piSrc0[ i +4*2 ];
        x6 = piSrc0[ i +5*2 ];
        x2 = piSrc0[ i +6*2 ];      
        x5 = piSrc0[ i +7*2 ];
        x0 = piSrc0[ i +0*2 ]; /* for proper rounding */
        
        x1 = x1 * W0;
        x0 = x0 * W0 + (4+(4<<16)); /* for proper rounding */
        
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
        /*
        blk [0] = (I16_WMV) ((x7 + x4) >> 3);
        blk [1] = (I16_WMV) ((x3 + x4a) >> 3);
        blk [2] = (I16_WMV) ((x0 + x5a) >> 3);
        blk [3] = (I16_WMV) ((x8 + x5) >> 3);
        blk [4] = (I16_WMV) ((x8 - x5) >> 3);
        blk [5] = (I16_WMV) ((x0 - x5a) >> 3);
        blk [6] = (I16_WMV) ((x3 - x4a) >> 3);
        blk [7] = (I16_WMV) ((x7 - x4) >> 3);
        */
        
        // blk [0,1]
        b0 = x7 + x4;
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x3 + x4a;
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[0] = (c0<<16) + b0;
        blk32Next[0] = (c1<<16) + b1;
        
        // blk [2,3]
        b0 = x0 + x5a;
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x8 + x5;
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[1] = (c0<<16) + b0;
        blk32Next[1] = (c1<<16) + b1;
        
        // blk [4,5]
        b0 = x8 - x5;
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x0 - x5a;
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[2] = (c0<<16) + b0;
        blk32Next[2] = (c1<<16) + b1;
        
        // blk [6,7]
        b0 = x3 - x4a;
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x7 - x4;
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[3] = (c0<<16) + b0;
        blk32Next[3] = (c1<<16) + b1;
    }
    
    blk0 = piDst;
    blk1 = blk0 + iOffsetToNextRowForDCTHalf;
    blk2 = blk1 + iOffsetToNextRowForDCTHalf;
    blk3 = blk2 + iOffsetToNextRowForDCTHalf;
    
    for (i = 0; i < 4; i++) {
        x4 = blk0[i];
        x5 = blk1[i];
        x6 = blk2[i];
        x7 = blk3[i];
        
        x3 = (x4 - x6); 
        x6 += x4;
        
        x4 = 8 * x6 + 32 + (32<<16); //rounding
        x8 = 8 * x3 + 32 + (32<<16);   //rounding
        //simplify following to 3 multiplies
        x5a = 11 * x5 + 5 * x7;
        x5 = 5 * x5 - 11 * x7;
        
        /*
        x4 += (x6 >> 1); // guaranteed to have enough head room
        x8 += (x3 >> 1);
        */
        ls_signbit=x6&0x8000;
        x6 = (x6 >> 1) - ls_signbit;
        x6 = x6 & ~0x8000;
        x6 = x6 | ls_signbit;
        
        ls_signbit=x3&0x8000;
        x3 = (x3 >> 1) - ls_signbit;
        x3 = x3 & ~0x8000;
        x3 = x3 | ls_signbit;
        
        x4 += x6;
        x8 += x3;
        
        /*
        blk0[i] = (I16_WMV) ((x4 + x5a) >> 6);
        blk1[i] = (I16_WMV) ((x8 + x5) >> 6);
        blk2[i] = (I16_WMV) ((x8 - x5) >> 6);
        blk3[i] = (I16_WMV) ((x4 - x5a) >> 6);
        */
        
        
        // blk0
        b0 = (x4 + x5a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        *(((I16_WMV *)(blk0 + i)) + 0 ) = b0;
        *(((I16_WMV *)(blk0 + i)) + 1 ) = b1;
        
        // blk1
        b0 = (x8 + x5);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        *(((I16_WMV *)(blk1 + i)) + 0 ) = b0;
        *(((I16_WMV *)(blk1 + i)) + 1 ) = b1;
        
        // blk2
        b0 = (x8 - x5);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        *(((I16_WMV *)(blk2 + i)) + 0 ) = b0;
        *(((I16_WMV *)(blk2 + i)) + 1 ) = b1;
        
        // blk3
        b0 = (x4 - x5a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        *(((I16_WMV *)(blk3 + i)) + 0 ) = b0;
        *(((I16_WMV *)(blk3 + i)) + 1 ) = b1;
        
    } 
    FUNCTION_PROFILE_STOP(&fpDecode);
}

Void_WMV g_4x8IDCTDec_WMV3 (UnionBuffer * piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer * rgiCoefReconBuf, I32_WMV iHalf)
{
    
    I32_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a;
    I32_WMV  y3, y4, y5, y4a, b0,c0,b1,c1,ls_signbit;
    
    I32_WMV * piDst = piDstBuf->i32 + (iHalf*2);
    I32_WMV * blk32 = piDst;
    I32_WMV * blk32Next;
    I32_WMV i ;
    I32_WMV* blk0;
    I32_WMV* blk1;
    I32_WMV* blk2;
    I32_WMV* blk3;
    I32_WMV* blk4;
    I32_WMV* blk5;
    I32_WMV* blk6;
    I32_WMV* blk7;
    const I32_WMV  * piSrc0 = rgiCoefReconBuf->i32;
    I32_WMV iOffsetToNextRowForDCTHalf = iOffsetToNextRowForDCT>>1;
    FUNCTION_PROFILE(fpDecode);
    FUNCTION_PROFILE_START(&fpDecode,IDCT_NT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_4x8IDCTDec_WMV3);
    DEBUG_CACHE_READ_BYTES( piSrc0, BLOCK_SIZE*4*2 );
    DEBUG_CACHE_WRITE_STRIDES( piDst, BLOCK_SIZE*2, iOffsetToNextRowForDCT*2, 4 );
    
    // memcpy(dsttmp, rgiCoefReconBuf->i32, 64);
    // piSrc0 = (I32_WMV*) dsttmp;
    
    piSrc0 = rgiCoefReconBuf->i32;
    
    for ( i = 0; i < 4; i++, blk32 += iOffsetToNextRowForDCT){
        
        blk32Next = blk32 + iOffsetToNextRowForDCTHalf;
        x4 = piSrc0[ i +0*4];
        x5 = piSrc0[ i +1*4];
        x6 = piSrc0[ i +2*4];
        x7 = piSrc0[ i +3*4];
        
        
        x0 = 17 * (x4 + x6) + 4 + (4<<16); //rounding
        x1 = 17 * (x4 - x6)  + 4 + (4<<16); //rounding
        x8 = 10 * (x5 + x7);
        x2 = x8 + 12 * x5;
        x3 = x8 - 32 * x7;
        
        /*
        blk[0] = (I16_WMV)((x0 + x2) >> 3);
        blk[1] = (I16_WMV)((x1 + x3) >> 3);
        blk[2] = (I16_WMV)((x1 - x3) >> 3);
        blk[3] = (I16_WMV)((x0 - x2) >> 3);
        */
        
        // blk [0,1]
        b0 = x0 + x2;
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x1 + x3;
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[0] = (c0<<16) + b0;
        blk32Next[0] = (c1<<16) + b1;
        
        // blk [2,3]
        b0 = x1 - x3;
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x0 - x2;
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[1] = (c0<<16) + b0;
        blk32Next[1] = (c1<<16) + b1;
    }
    
    blk0 = piDst;
    blk1 = blk0 + iOffsetToNextRowForDCTHalf;
    blk2 = blk1 + iOffsetToNextRowForDCTHalf;
    blk3 = blk2 + iOffsetToNextRowForDCTHalf;
    blk4 = blk3 + iOffsetToNextRowForDCTHalf;
    blk5 = blk4 + iOffsetToNextRowForDCTHalf;
    blk6 = blk5 + iOffsetToNextRowForDCTHalf;
    blk7 = blk6 + iOffsetToNextRowForDCTHalf;
    
    for (i = 0; i < 2; i++)
    {
        x0 = blk0[i] * 6 + 32 + (32<<16) /* rounding */;
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
        
        x1 = 8 * (x2 + x3);
        x6 = x1 - 5 * x2;
        x1 -= 11 * x3;
        
        // third stage
        x7 = x8 + x6;
        x8 -= x6;
        
        
        x6 = x0 - x1;
        x0 += x1;
        
        // fractional part (guaranteed to have headroom)
        
        // Change 3:
        
        /*
        y5 = y4 >> 1;
        y3 = y4a >> 1;
        */
        
        ls_signbit=y4&0x8000;
        y5 = (y4 >> 1) - ls_signbit;
        y5 = y5 & ~0x8000;
        y5 = y5 | ls_signbit;
        
        ls_signbit=y4a&0x8000;
        y3 = (y4a >> 1) - ls_signbit;
        y3 = y3 & ~0x8000;
        y3 = y3 | ls_signbit;
        
        // fourth stage
        
        x4 += y5;
        x5 += y5;
        x4a += y3;
        x5a += y3;
        
        /*
        blk0 [i] = (x7 + x4) >> 6;
        blk1 [i] = (x6 + x4a) >> 6;
        blk2 [i] = (x0 + x5a) >> 6;
        blk3 [i] = (x8 + x5) >> 6;  
        blk4 [i] = (x8 - x5) >> 6;
        blk5 [i] = (x0 - x5a) >> 6;
        blk6 [i] = (x6 - x4a) >> 6;
        blk7 [i] = (x7 - x4) >> 6;
        */
        
        // blk0
        b0 = (x7 + x4);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        *(((I16_WMV *)(blk0 + i)) + 0 ) = b0;
        *(((I16_WMV *)(blk0 + i)) + 1 ) = b1;
        
        // blk1
        b0 = (x6 + x4a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        *(((I16_WMV *)(blk1 + i)) + 0 ) = b0;
        *(((I16_WMV *)(blk1 + i)) + 1 ) = b1;
        
        // blk2
        b0 = (x0 + x5a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        *(((I16_WMV *)(blk2 + i)) + 0 ) = b0;
        *(((I16_WMV *)(blk2 + i)) + 1 ) = b1;
        
        // blk3
        b0 = (x8 + x5);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        *(((I16_WMV *)(blk3 + i)) + 0 ) = b0;
        *(((I16_WMV *)(blk3 + i)) + 1 ) = b1;
        
        // blk4
        b0 = (x8 - x5);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        *(((I16_WMV *)(blk4 + i)) + 0 ) = b0;
        *(((I16_WMV *)(blk4 + i)) + 1 ) = b1;
        
        // blk5
        b0 = (x0 - x5a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        *(((I16_WMV *)(blk5 + i)) + 0 ) = b0;
        *(((I16_WMV *)(blk5 + i)) + 1 ) = b1;
        
        // blk6
        b0 = (x6 - x4a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        *(((I16_WMV *)(blk6 + i)) + 0 ) = b0;
        *(((I16_WMV *)(blk6 + i)) + 1 ) = b1;
        
        // blk7
        b0 = (x7 - x4);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        *(((I16_WMV *)(blk7 + i)) + 0 ) = b0;
        *(((I16_WMV *)(blk7 + i)) + 1 ) = b1;
        
    }
    FUNCTION_PROFILE_STOP(&fpDecode);
    
}

Void_WMV g_4x4IDCTDec_WMV3 (UnionBuffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iQuadrant)
{
    I16_WMV* piDst = piDstBuf->i16 + (iQuadrant&2)*16 + (iQuadrant&1)*4;
    I32_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x5a;
    I32_WMV i;
    I32_WMV  *piSrc0; 
    I32_WMV j;
    I32_WMV b0,c0,b1,c1,ls_signbit;    
    I32_WMV rgTemp[8];
    I16_WMV * blk16 ;
    I32_WMV * blk32 = rgTemp;
    /*
    I16_WMV* blk = rgTemp;   
    I16_WMV* blk0 = (short*)rgiCoefReconBuf;
    I16_WMV* blk1 = blk0 + 4;
    I16_WMV* blk2 = blk1 + 4;
    I16_WMV* blk3 = blk2 + 4;
    */
    
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_4x4IDCTDec_WMV3);
    DEBUG_CACHE_READ_BYTES( rgiCoefReconBuf->i32, 4*4*4 );
    DEBUG_CACHE_WRITE_STRIDES( piDst, 4*2, iOffsetToNextRowForDCT*2, 4 );
    
    piSrc0 = (I32_WMV *)rgiCoefReconBuf->i32;
    
    for ( i = 0; i < 2; i++, blk32 += 4){
        x4 = piSrc0[ i +0*2 ];      
        x5 = piSrc0[ i +1*2 ];      
        x6 = piSrc0[ i +2*2 ];
        x7 = piSrc0[ i +3*2 ];
        
        
        x0 = 17 * (x4 + x6) + 4 + (4<<16); //rounding
        x1 = 17 * (x4 - x6) + 4 + (4<<16); //rounding
        x8 = 10 * (x5 + x7);
        x2 = x8 + 12 * x5;
        x3 = x8 - 32 * x7;
        
        /*
        blk[0] = (I16_WMV)((x0 + x2) >> 3);
        blk[1] = (I16_WMV)((x1 + x3) >> 3);
        blk[2] = (I16_WMV)((x1 - x3) >> 3);
        blk[3] = (I16_WMV)((x0 - x2) >> 3);
        */
        
        // blk [0,1]
        b0 = x0 + x2;
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x1 + x3;
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[0] = (c0<<16) + b0;
        blk32[0+2] = (c1<<16) + b1;
        
        // blk [2,3]
        b0 = x1 - x3;
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x0 - x2;
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[1] = (c0<<16) + b0;
        blk32[1+2] = (c1<<16) + b1;
        
    }
    
    
    piSrc0 = rgTemp; //piDst->i32;
    blk16  = piDst;
    
    
    for (i = 0; i < 2; i++) {
        
        x4 = piSrc0[i + 0*2 ];
        x5 = piSrc0[i + 1*2 ];
        x6 = piSrc0[i + 2*2 ];
        x7 = piSrc0[i + 3*2 ];
        
        
        x3 = (x4 - x6); 
        x6 += x4;
        
        x4 = 8 * x6 + 32 + (32<<16); //rounding
        x8 = 8 * x3 + 32 + (32<<16);   //rounding
        //simplify following to 3 multiplies
        x5a = 11 * x5 + 5 * x7;
        x5 = 5 * x5 - 11 * x7;
        
        /*
        x6 = (x6 >> 1);
        x3 = (x3 >> 1)
        */
        
        ls_signbit=x6&0x8000;
        x6 = (x6 >> 1) - ls_signbit;
        x6 = x6 & ~0x8000;
        x6 = x6 | ls_signbit;
        
        ls_signbit=x3&0x8000;
        x3 = (x3 >> 1) - ls_signbit;
        x3 = x3 & ~0x8000;
        x3 = x3 | ls_signbit;
        
        x4 += x6; // guaranteed to have enough head room
        x8 += x3 ;
        
        /*
        blk0[i] = (I16_WMV) ((x4 + x5a) >> 6);
        blk1[i] = (I16_WMV) ((x8 + x5) >> 6);
        blk2[i] = (I16_WMV) ((x8 - x5) >> 6);
        blk3[i] = (I16_WMV) ((x4 - x5a) >> 6);
        */
        
        j = i<<1;
        
        // blk0
        b0 = (x4 + x5a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ j + 0 + 0*8] = b0;
        blk16[ j + 1 + 0*8] = b1;
        
        // blk1
        b0 = (x8 + x5);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ j + 0 + 1*8] = b0;
        blk16[ j + 1 + 1*8] = b1;
        
        // blk2
        b0 = (x8 - x5);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ j + 0 + 2*8] = b0;
        blk16[ j + 1 + 2*8] = b1;
        
        // blk3
        b0 = (x4 - x5a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ j + 0 + 3*8] = b0;
        blk16[ j + 1 + 3*8] = b1;
        
    } 
}

void SignPatch(I16_WMV * rgiCoefRecon, int len)
{
    int i;
    I16_WMV *piSrcTmp = rgiCoefRecon;
    I16_WMV * dsttmp = rgiCoefRecon;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(SignPatch);
    
    for(i=0;i<(len/2);i+=2)
    {
        I16_WMV data_odd = piSrcTmp[i+1];
        
        *(I32_WMV *)(dsttmp + i) =  (I32_WMV) (piSrcTmp[i]);
        
        dsttmp[i+Y_INDEX]  +=  data_odd;
    }
}



/* 
optimized C code against Ref code.
requirement: len mod 4 = 0
*/
void SignPatch32(I32_WMV * rgiCoefRecon, int len)
{
    int i;
    I32_WMV v1, v2;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(SignPatch32);
    
    for(i=0; i < (len >> 2); i++)
    {
        v1 = rgiCoefRecon[i*2];
        v2 = rgiCoefRecon[i*2+1];
        
        rgiCoefRecon[i] = (v1 & 0x0000ffff) | (((v1 >> 16) + v2) << 16);
    }
}

#ifndef WMV_OPT_IDCT_SHX
///////////////////////////////////////////////////////////////////////////
// Inter IDCT Functions
// (this fn and one below are almost repeats of each other!!! - INEFFICIENT!)
///////////////////////////////////////////////////////////////////////////
// C Version
// Input:  16-bit
// Output: 16-bit
#ifndef _EMB_HYBRID_16_32_IDCT_

#if !defined(_MIPS_ASM_IDCTDEC_OPT_) || !defined(_MIPS64)

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec_WMV3_Pass1(const I32_WMV  *piSrc0, I32_WMV * blk32, const I32_WMV iNumLoops, I32_WMV iDCTHorzFlags)
{
    
    I32_WMV i;
    I32_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a;
    I32_WMV  y3,  b0,c0,b1,c1;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV3_Pass1);
    
    for ( i = 0; i < iNumLoops; i++,  blk32 += 8, iDCTHorzFlags >>= 2 ){
        // Check to see if this row has all zero coefficients in columns 1-7 (0 is first column)
        //        if (!(iDCTHorzFlags & (1 << i))) {
        //            // Columns 1-7 must be zero. Check to see if column zero coefficient is zero. 
        //            // If so, we don't have to do anything more for this row
        //            // Column zero cofficient is nonzero so all coefficients in this row are DC
        //            blk [0] = blk [1] = blk [2] = blk [3] = blk [4] = blk [5] = blk [6] = blk [7] = rgiCoefRecon [0] << 3;
        //            // If this is not the first row, tell the second stage that it can't use DC
        //            // shortcut - it has to do the full transform for all the columns
        //            iDCTVertFlag = i;
        //          continue;
        //      }
        
        if(!(iDCTHorzFlags&3)) 
        {
            I32_WMV iCurr, iNext;
            
            assert(piSrc0[ i +1*4 ]== 0);
            assert(piSrc0[ i +2*4 ]== 0);
            assert(piSrc0[ i +3*4 ]== 0);
            assert(piSrc0[ i +4*4 ]== 0);
            assert(piSrc0[ i +5*4 ]== 0);
            assert(piSrc0[ i +6*4 ]== 0);
            assert(piSrc0[ i +7*4 ]== 0);
            
            b0 = piSrc0[ i ]*W0 + (4+(4<<16)); //12
            b1 = (b0 + 0x8000)>>19;
            b0 = ((I16_WMV)b0)>>3;
            
            iCurr = (b0<<16) + b0;
            iNext = (b1<<16) + b1;
            
            blk32[0] = iCurr;
            blk32[0+4] = iNext;
            blk32[1] = iCurr;
            blk32[1+4] = iNext;
            blk32[2] = iCurr;
            blk32[2+4] = iNext;
            blk32[3] = iCurr;
            blk32[3+4] = iNext;
            
            continue;
            
        }
        
        // If this is not the first row, tell the second stage that it can't use DC shortcut 
        // - it has to do the full transform for all the columns
        
        x4 = piSrc0[ i +1*4 ];      
        x3 = piSrc0[ i +2*4 ];
        x7 = piSrc0[ i +3*4 ];
        x1 = piSrc0[ i +4*4 ];
        x6 = piSrc0[ i +5*4 ];
        x2 = piSrc0[ i +6*4 ];      
        x5 = piSrc0[ i +7*4 ];
        x0 = piSrc0[ i +0*4 ]; /* for proper rounding */
        
        
        x1 = x1 * W0;   //12
        x0 = x0 * W0 + (4+(4<<16)); /* for proper rounding */
        
        // zeroth stage
        y3 = x4 + x5;
        x8 = W3 * y3;           //15
        x4a = x8 - W3pW5 * x5;  //24
        x5a = x8 - W3_W5 * x4;  //6
        x8 = W7 * y3;           //4
        x4 = x8 + W1_W7 * x4;   //12
        x5 = x8 - W1pW7 * x5;   //20
        
        // first stage
        y3 = x6 + x7;
        x8 = W7 * y3;           //4
        x4a -= x8 + W1_W7 * x6; //12
        x5a += x8 - W1pW7 * x7; //20
        x8 = W3 * y3;           //15
        x4 += x8 - W3_W5 * x6;  //6
        x5 += x8 - W3pW5 * x7;  //24
        
        // second stage 
        x8 = x0 + x1;
        x0 -= x1;

        x1 = W6 * x3 - W2 * x2;  //6,  16
        x3 = W6 * x2 + W2A * x3; //6,  16

        // third stage
        x7 = x8 + x3;
        x8 -= x3;
        x3 = x0 + x1;
        x0 -= x1;      
        
        // fourth stage
        /*
        blk [0] = (I16_WMV) ((x7 + x4) >> 3);
        blk [1] = (I16_WMV) ((x3 + x4a) >> 3);
        blk [2] = (I16_WMV) ((x0 + x5a) >> 3);
        blk [3] = (I16_WMV) ((x8 + x5) >> 3);
        blk [4] = (I16_WMV) ((x8 - x5) >> 3);
        blk [5] = (I16_WMV) ((x0 - x5a) >> 3);
        blk [6] = (I16_WMV) ((x3 - x4a) >> 3);
        blk [7] = (I16_WMV) ((x7 - x4) >> 3);
        */
        
        // blk [0,1]
        b0 = x7 + x4;	// sw: b0 = 12*x0 + 16*x4 + 16*x3 + 15*x7 + 12*x1 + 9*x6 + 6*x2 + 4*x5 + rounding
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x3 + x4a;	// sw: c0 = 12*x0 + 15*x4 + 6*x3 + -4*x7 + -12*x1 + -16*x6 + -16*x2 + -9*x5 + rounding
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[0] = (c0<<16) + b0;
        blk32[0+4] = (c1<<16) + b1;
        
        // blk [2,3]
        b0 = x0 + x5a;
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x8 + x5;
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[1] = (c0<<16) + b0;
        blk32[1+4] = (c1<<16) + b1;
        
        // blk [4,5]
        b0 = x8 - x5;
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x0 - x5a;
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[2] = (c0<<16) + b0;
        blk32[2+4] = (c1<<16) + b1;
        
        // blk [6,7]
        b0 = x3 - x4a;
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x7 - x4;
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[3] = (c0<<16) + b0;
        blk32[3+4] = (c1<<16) + b1;
        
    }
}
#endif //_MIPS_ASM_IDCTDEC_OPT_

#else

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec_WMV3_Pass1(const I32_WMV  *piSrc0, I32_WMV * blk32, const I32_WMV iNumLoops, I32_WMV iDCTHorzFlags)
{
    
    I16_WMV i;
    I16_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a;
    I32_WMV  y3,  b0, c0;
    I16_WMV  *piSrc = (I16_WMV *)piSrc0;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV3_Pass1);
    DEBUG_CACHE_READ_BYTES( piSrc, BLOCK_SIZE*iNumLoops*2 );
    DEBUG_CACHE_WRITE_BYTES( blk32, 4*iNumLoops*4 );
    
    for ( i = 0; i < iNumLoops; i++,  blk32 += 4, iDCTHorzFlags >>= 1 ){
        // Check to see if this row has all zero coefficients in columns 1-7 (0 is first column)
        //        if (!(iDCTHorzFlags & (1 << i))) {
        //            // Columns 1-7 must be zero. Check to see if column zero coefficient is zero. 
        //            // If so, we don't have to do anything more for this row
        //            // Column zero cofficient is nonzero so all coefficients in this row are DC
        //            blk [0] = blk [1] = blk [2] = blk [3] = blk [4] = blk [5] = blk [6] = blk [7] = rgiCoefRecon [0] << 3;
        //            // If this is not the first row, tell the second stage that it can't use DC
        //            // shortcut - it has to do the full transform for all the columns
        //            iDCTVertFlag = i;
        //          continue;
        //      }
        
        if(!(iDCTHorzFlags&1))
        {
            assert(piSrc[ i +1*8 ]== 0);
            assert(piSrc[ i +2*8 ]== 0);
            assert(piSrc[ i +3*8 ]== 0);
            assert(piSrc[ i +4*8 ]== 0);
            assert(piSrc[ i +5*8 ]== 0);
            assert(piSrc[ i +6*8 ]== 0);
            assert(piSrc[ i +7*8 ]== 0);
            
            b0 = (piSrc[ i ]*W0 + 4)>>3; //12
            b0 += b0<<16;

            blk32[0] = b0;
            blk32[1] = b0;
            blk32[2] = b0;
            blk32[3] = b0;
            
            continue;
            
        }
        
        // If this is not the first row, tell the second stage that it can't use DC shortcut 
        // - it has to do the full transform for all the columns
        
        x4 = piSrc[ i +1*8 ];      
        x3 = piSrc[ i +2*8 ];
        x7 = piSrc[ i +3*8 ];
        x1 = piSrc[ i +4*8 ];
        x6 = piSrc[ i +5*8 ];
        x2 = piSrc[ i +6*8 ];      
        x5 = piSrc[ i +7*8 ];
        x0 = piSrc[ i +0*8 ]; /* for proper rounding */
        
        
        x1 = x1 * W0;   //12
        x0 = x0 * W0 + 4; /* for proper rounding */
        
        // zeroth stage
        y3 = x4 + x5;
        x8 = W3 * y3;           //15
        x4a = x8 - W3pW5 * x5;  //24
        x5a = x8 - W3_W5 * x4;  //6
        x8 = W7 * y3;           //4
        x4 = x8 + W1_W7 * x4;   //12
        x5 = x8 - W1pW7 * x5;   //20
        
        // first stage
        y3 = x6 + x7;
        x8 = W7 * y3;           //4
        x4a -= x8 + W1_W7 * x6; //12
        x5a += x8 - W1pW7 * x7; //20
        x8 = W3 * y3;           //15
        x4 += x8 - W3_W5 * x6;  //6
        x5 += x8 - W3pW5 * x7;  //24
        
        // second stage 
        x8 = x0 + x1;
        x0 -= x1;
        
        x1 = x2;
        x2 = W6 * x3 - W2 * x2;  //6,  16
        x3 = W6 * x1 + W2A * x3; //6,  16
        
        // third stage
        x7 = x8 + x3;
        x8 -= x3;
        x3 = x0 + x2;
        x0 -= x2;
        
        // fourth stage
        b0 = (x7 + x4) >> 3;
        c0 = (x3 + x4a) >> 3;
        b0 += c0 << 16;

        blk32 [0] = b0;

        b0 = (x0 + x5a) >> 3;
        c0 = (x8 + x5) >> 3;
        b0 += c0 << 16;

        blk32[1] = b0;

        b0 = (x8 - x5) >> 3;
        c0 = (x0 - x5a) >> 3;
        b0 += c0 << 16;

        blk32[2] = b0;

        b0 = (x3 - x4a) >> 3;
        c0 = (x7 - x4) >> 3;
        b0 += c0 << 16;

        blk32[3] = b0;
    }
}

#endif

#if !defined(_MIPS_ASM_IDCTDEC_OPT_) || !defined(_MIPS64)

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec_WMV3_Pass2(const I32_WMV  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops)
{
    I32_WMV i;
    I32_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a;
    I32_WMV  y4, y4a, b0,b1,ls_signbit;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV3_Pass2);
    DEBUG_CACHE_READ_BYTES( piSrc0, BLOCK_SIZE*iNumLoops*4 );
    DEBUG_CACHE_WRITE_STRIDES( blk16, BLOCK_SIZE*iNumLoops*2, 32, 2 );
    
    for (i = 0; i < iNumLoops; i++){
        // If iDCTVertFlag is zero then we know that blk[1] thru blk[7] are zero.
        // Therefore, if blk[0] is zero we can skip to the next column. Otherwise we
        // can use the DC shortcut and just copy blk[0] to the rest of the column.
        //        if (iDCTVertFlag == 0){
        //            // If blk[0] is nonzero, copy the value to the other 7 rows in this column        
        //            if (blk0[i] != 0)
        //              blk0[i] = blk1[i] = blk2[i] = blk3[i] = blk4[i] = blk5[i] = blk6[i] = blk7[i] = (blk0[i] + 32) >> 6;
        //          continue;
        //      }
        x0 = piSrc0[i + 0*4 ] * 6 + 32 + (32<<16) /* rounding */;
        x4 = piSrc0[i + 1*4 ];
        x3 = piSrc0[i + 2*4 ];
        x7 = piSrc0[i + 3*4 ];
        x1 = piSrc0[i + 4*4 ] * 6;
        x6 = piSrc0[i + 5*4 ];
        x2 = piSrc0[i + 6*4 ];
        x5 = piSrc0[i + 7*4 ];
        
        // zeroth stage
        y4a = x4 + x5;
        x8 = 7 * y4a;
        x4a = x8 - 12 * x5;
        x5a = x8 - 3 * x4;
        x8 = 2 * y4a;
        x4 = x8 + 6 * x4;
        x5 = x8 - 10 * x5;
        
        ls_signbit=y4a&0x8000;
        y4a = (y4a >> 1) - ls_signbit;
        y4a = y4a & ~0x8000;
        y4a = y4a | ls_signbit;
        x4a += y4a;
        x5a += y4a;
        
        // first stage
        y4 = x6 + x7;
        x8 = 2 * y4;
        
        x4a -= x8 + 6 * x6;
        x5a += x8 - 10 * x7;
        x8 = 7 * y4;
        
        ls_signbit=y4&0x8000;
        y4 = (y4 >> 1) - ls_signbit;
        y4 = y4 & ~0x8000;
        y4 = y4 | ls_signbit;
        //        x4 += y4;
        //        x5 += y4;
        x8 += y4;
        x4 += x8 - 3 * x6;
        x5 += x8 - 12 * x7;
        
        // second stage 
        x8 = x0 + x1;
        x0 -= x1;
        
        x1 = 8 * (x2 + x3);
        x6 = x1 - 5 * x2;
        x1 -= 11 * x3;
        
        // third stage
        x7 = x8 + x6;
        x8 -= x6;
        
        x6 = x0 - x1;
        x0 += x1;
        
        /*
        y5 = y4 >> 1;
        y3 = y4a >> 1;
        */
        /*
        ls_signbit=y4&0x8000;
        y5 = (y4 >> 1) - ls_signbit;
        y5 = y5 & ~0x8000;
        y5 = y5 | ls_signbit;
        
          ls_signbit=y4a&0x8000;
          y3 = (y4a >> 1) - ls_signbit;
          y3 = y3 & ~0x8000;
          y3 = y3 | ls_signbit;
          // fourth stage
          // Change 4:
          
            x4 += y4;
            x5 += y4;
            x4a += y3;
            x5a += y3;
        */
        
        /*
        
          _blk0 [i] = (x7 + x4) >> 6;
          _blk1 [i] = (x6 + x4a) >> 6;
          _blk2 [i] = (x0 + x5a) >> 6;
          _blk3 [i] = (x8 + x5) >> 6;
          _blk4 [i] = (x8 - x5) >> 6;
          _blk5 [i] = (x0 - x5a) >> 6;
          _blk6 [i] = (x6 - x4a) >> 6;
          _blk7 [i] = (x7 - x4) >> 6;
          
        */
        
        // j = i<<1;
        
        // blk0
        b0 = (x7 + x4);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ i + 0   + 0*4] = b0;
        blk16[ i + 32 + 0*4] = b1;
        
        // blk1
        b0 = (x6 + x4a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ i + 0 + 1*4] = b0;
        blk16[ i + 32 + 1*4] = b1;
        
        // blk2
        b0 = (x0 + x5a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ i + 0 + 2*4] = b0;
        blk16[ i + 32 + 2*4] = b1;
        
        // blk3
        b0 = (x8 + x5);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ i + 0 + 3*4] = b0;
        blk16[ i + 32 + 3*4] = b1;
        
        // blk4
        b0 = (x8 - x5);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ i + 0 + 4*4] = b0;
        blk16[ i + 32 + 4*4] = b1;
        
        // blk5
        b0 = (x0 - x5a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ i + 0 + 5*4] = b0;
        blk16[ i + 32 + 5*4] = b1;
        
        // blk6
        b0 = (x6 - x4a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ i + 0 + 6*4] = b0;
        blk16[ i + 32 + 6*4] = b1;
        
        // blk7
        b0 = (x7 - x4);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ i + 0 + 7*4] = b0;
        blk16[ i + 32 + 7*4] = b1;
        
        
    }
    
}
#endif //_MIPS_ASM_IDCTDEC_OPT_

#ifndef _EMB_HYBRID_16_32_IDCT_

#if !defined(_MIPS_ASM_IDCTDEC_OPT_) || !defined(_MIPS64)

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec_WMV3_Pass3(const I32_WMV  *piSrc0, I32_WMV * blk32, const I32_WMV iNumLoops,  I32_WMV iDCTHorzFlags)
{
    I32_WMV i;
    I32_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8;
    I32_WMV  b0,c0,b1,c1;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV3_Pass3);
    DEBUG_CACHE_READ_BYTES( piSrc0, BLOCK_SIZE*iNumLoops*4 );
    DEBUG_CACHE_WRITE_BYTES( blk32, BLOCK_SIZE*iNumLoops*4 );
    
    for ( i = 0; i < iNumLoops; i++, blk32 += 8, iDCTHorzFlags >>= 2)
    {
        if(!(iDCTHorzFlags&3))
        {
            I32_WMV iCurr, iNext;

            assert(piSrc0[ i +1*4 ]== 0);
            assert(piSrc0[ i +2*4 ]== 0);
            assert(piSrc0[ i +3*4 ]== 0);
            
            b0 = piSrc0[ i ]*17 + (4+(4<<16));
            b1 = (b0 + 0x8000)>>19;
            b0 = ((I16_WMV)b0)>>3;
            
            iCurr = (b0<<16) + b0;
            iNext = (b1<<16) + b1;
            
            blk32[0] = iCurr;
            blk32[0+4] = iNext;
            blk32[1] = iCurr;
            blk32[1+4] = iNext;
            
            continue;
        }
        
        x4 = piSrc0[ i +0*4 ];      
        x5 = piSrc0[ i +1*4 ];      
        x6 = piSrc0[ i +2*4 ];
        x7 = piSrc0[ i +3*4 ];
        
        
        x0 = 17 * (x4 + x6) + 4 + (4<<16); //rounding
        x1 = 17 * (x4 - x6) + 4 + (4<<16); //rounding
        x8 = 10 * (x5 + x7);
        x2 = x8 + 12 * x5;
        x3 = x8 - 32 * x7;
        
        /*
        blk[0] = (I16_WMV)((x0 + x2) >> 3);
        blk[1] = (I16_WMV)((x1 + x3) >> 3);
        blk[2] = (I16_WMV)((x1 - x3) >> 3);
        blk[3] = (I16_WMV)((x0 - x2) >> 3);
        */
        
        // blk [0,1]
		// b0 = 17*x4 + 22*x5 + 17*x6 + 10*x7
		// c0 = 17*x4 + 10*x5 + -17*x6 + -22*x7
        b0 = x0 + x2;
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x1 + x3;
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[0] = (c0<<16) + b0;
        blk32[0+4] = (c1<<16) + b1;
        
        // blk [2,3]
		// b0 = 17*x4 + -10*x5 + -17*x6 + 22*x7
		// c0 = 17*x4 + -22*x5 + 17*x6 + -10*x7
        b0 = x1 - x3;
        b1 = (b0 + 0x8000)>>19;
        b0 = ((I16_WMV)b0)>>3;
        
        c0 = x0 - x2;
        c1 = (c0 + 0x8000)>>19;
        c0 = ((I16_WMV)c0)>>3;
        
        blk32[1] = (c0<<16) + b0;
        blk32[1+4] = (c1<<16) + b1;
        
    }
}
#endif //_MIPS_ASM_IDCTDEC_OPT_

#else

/* 
 * Hybrid iDCT16_32 implementation: 16 bit => Pass1/3 => 32 bit => Pass2/4 SSIMD32 => 32 bit
 */

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec_WMV3_Pass3(const I32_WMV  *piSrc0, I32_WMV * blk32, const I32_WMV iNumLoops,  I32_WMV iDCTHorzFlags)
{
    I16_WMV i;
    I16_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8;
    I32_WMV  b0, c0;
    I16_WMV *piSrc = (I16_WMV *)piSrc0;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV3_Pass3);
    DEBUG_CACHE_READ_BYTES( piSrc, BLOCK_SIZE*iNumLoops*2 );
    DEBUG_CACHE_WRITE_BYTES( blk16, BLOCK_SIZE*iNumLoops*4 );
    
    for ( i = 0; i < iNumLoops; i++, blk32 += 4, iDCTHorzFlags >>= 1)
    {
        if(!(iDCTHorzFlags&1))
        {
            assert(piSrc[ i +1*8 ]== 0);
            assert(piSrc[ i +2*8 ]== 0);
            assert(piSrc[ i +3*8 ]== 0);
            
            
            b0 = (piSrc[ i ]*17 + 4) >> 3;
            b0 += b0<<16;

            blk32[0] = b0;
            blk32[1] = b0;
            
            continue;
            
        }
        
        x4 = piSrc[ i +0*8 ];      
        x5 = piSrc[ i +1*8 ];      
        x6 = piSrc[ i +2*8 ];
        x7 = piSrc[ i +3*8 ];
        
        
        x0 = 17 * (x4 + x6) + 4; //rounding
        x1 = 17 * (x4 - x6) + 4; //rounding
        x8 = 10 * (x5 + x7);
        x2 = x8 + 12 * x5;
        x3 = x8 - 32 * x7;
        
        b0 = (x0 + x2) >> 3;
        c0 = (x1 + x3) >> 3;
        b0+= c0<<16;

        blk32[0] = b0;

        b0 = (x1 - x3) >> 3;
        c0 = (x0 - x2) >> 3;
        b0+= c0<<16;

        blk32[1] = b0;
    }
}


#endif


#if !defined(_MIPS_ASM_IDCTDEC_OPT_) || !defined(_MIPS64)

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec_WMV3_Pass4(const I32_WMV  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops)
{
	//sw: the size of buffer blk16 is (8*8*2)*4.

    I32_WMV i;
    I32_WMV  x1, x3, x4, x5, x6, x7, x8,  x5a;
    I32_WMV  b0,b1,ls_signbit;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV3_Pass4);
    DEBUG_CACHE_READ_BYTES( piSrc0, BLOCK_SIZE*iNumLoops*4 );
    DEBUG_CACHE_WRITE_STRIDES( blk16, BLOCK_SIZE*iNumLoops*2, 32*2, 2 );
    
    for (i = 0; i < iNumLoops; i++) {
        
        x4 = piSrc0[i + 0*4 ];
        x5 = piSrc0[i + 1*4 ];
        x6 = piSrc0[i + 2*4 ];
        x7 = piSrc0[i + 3*4 ];
        
        
        x3 = (x4 - x6); 
        x6 += x4;
        
        x4 = 8 * x6 + 32 + (32<<16); //rounding
        x8 = 8 * x3 + 32 + (32<<16);   //rounding
        
        /*
        x6 = (x6 >> 1);
        x3 = (x3 >> 1)
        */
        
        ls_signbit=x6&0x8000;
        x6 = (x6 >> 1) - ls_signbit;
        x6 = x6 & ~0x8000;
        x6 = x6 | ls_signbit;
        
        ls_signbit=x3&0x8000;
        x3 = (x3 >> 1) - ls_signbit;
        x3 = x3 & ~0x8000;
        x3 = x3 | ls_signbit;
        
        x4 += x6; // guaranteed to have enough head room
        x8 += x3 ;
        
        
        x1 = 5 * ( x5 + x7);
        x5a = x1 + 6 * x5;
        x5 =  x1 - 16 * x7;
        
        //simplify following to 3 multiplies
        //        x5a = 11 * x5 + 5 * x7;
        //        x5 = 5 * x5 - 11 * x7;
        
        /*
        blk0[i] = (I16_WMV) ((x4 + x5a) >> 6);
        blk1[i] = (I16_WMV) ((x8 + x5) >> 6);
        blk2[i] = (I16_WMV) ((x8 - x5) >> 6);
        blk3[i] = (I16_WMV) ((x4 - x5a) >> 6);
        */
        
        // j = i<<1;
        
        // blk0
		// sw: b0 = 17*x4 + 22*x5 + 17*x6 + 10*x7
        b0 = (x4 + x5a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ i + 0   + 0*4] = b0;
        blk16[ i + 32 + 0*4] = b1;
        
        
        // blk1
		// sw: b0 = 17*x4 + 10*x5 + -17*x6 + -22*x7
        b0 = (x8 + x5);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ i + 0   + 1*4] = b0;
        blk16[ i + 32 + 1*4] = b1;
        
        
        // blk2
		// sw: b0 = 17*x4 + -10*x5 + -17*x6 + 22*x7
        b0 = (x8 - x5);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ i + 0   + 2*4] = b0;
        blk16[ i + 32 + 2*4] = b1;
        
        
        // blk3
		// sw: b0 = 17*x4 + -22*x5 + 17*x6 + -10*x7
        b0 = (x4 - x5a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ i + 0   + 3*4] = b0;
        blk16[ i + 32 + 3*4] = b1;
        
    } 
}

#endif //_MIPS_ASM_IDCTDEC_OPT_
#endif //WMV_OPT_IDCT_SHX
#endif //WMV_OPT_IDCT_ARM

#ifndef WMV_OPT_IDCT_ARM
//#pragma code_seg (EMBSEC_PML)
Void_WMV g_SubBlkIDCTClear_EMB (UnionBuffer * piDstBuf,  I32_WMV iIdx)
{
    I32_WMV iNumLoop = 8<<(iIdx>>7);
    I32_WMV iStep   =  1<< ((iIdx >>6)&0x1);
    I32_WMV iStep2 = iStep<<1;
    I32_WMV* piDst = piDstBuf->i32 + (iIdx&0x3f);
    I32_WMV i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_SubBlkIDCTClear_EMB);
    DEBUG_CACHE_WRITE_STRIDES( piDst, BLOCK_SIZE*iNumLoop*4, 16*4, 2 );

    //(iNumLoop, iStep) = (8,1),(16,2), (8,2)
    for (i = 0; i < iNumLoop; i+=iStep2) 
    {
        piDst[i] = 0;
        piDst[i+16] = 0;
        piDst[i + iStep] = 0;
        piDst[i+16 + iStep] = 0;
    }
}
#endif //WMV_OPT_IDCT_ARM

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_SubBlkIDCTClear_EMB_C (UnionBuffer * piDstBuf,  I32_WMV iIdx)
{
    I32_WMV iNumLoop = 8<<(iIdx>>7);
    I32_WMV iStep   =  1<< ((iIdx >>6)&0x1);
    I32_WMV iStep2 = iStep<<1;
    I32_WMV* piDst = piDstBuf->i32 + ((iIdx&0x3f)<<1);
    I32_WMV i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_SubBlkIDCTClear_EMB_C);
    DEBUG_CACHE_WRITE_BYTES( piDst, BLOCK_SIZE*iNumLoop*4 );

    //(iNumLoop, iStep) = (8,1),(16,2), (8,2)
    for (i = 0; i < iNumLoop; i+=iStep2) 
    {
        piDst[2*i] = 0;
        piDst[2*i+1] = 0;
        piDst[2*(i+iStep)] = 0;
        piDst[2*(i+iStep)+1] = 0;
    }
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec_WMV3_Pass2_C(const I32_WMV  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops)
{
    I32_WMV i;
    I32_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a;
    I32_WMV  y4, y4a, b0,b1,ls_signbit;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV3_Pass2_C);
    DEBUG_CACHE_READ_BYTES( piSrc0, BLOCK_SIZE*iNumLoops*4 );
    DEBUG_CACHE_WRITE_BYTES( blk16, BLOCK_SIZE*iNumLoops*2 );
    
    for (i = 0; i < iNumLoops; i++){
        // If iDCTVertFlag is zero then we know that blk[1] thru blk[7] are zero.
        // Therefore, if blk[0] is zero we can skip to the next column. Otherwise we
        // can use the DC shortcut and just copy blk[0] to the rest of the column.
        //        if (iDCTVertFlag == 0){
        //            // If blk[0] is nonzero, copy the value to the other 7 rows in this column        
        //            if (blk0[i] != 0)
        //              blk0[i] = blk1[i] = blk2[i] = blk3[i] = blk4[i] = blk5[i] = blk6[i] = blk7[i] = (blk0[i] + 32) >> 6;
        //          continue;
        //      }
        x0 = piSrc0[i + 0*4 ] * 6 + 32 + (32<<16) /* rounding */;
        x1 = piSrc0[i + 4*4 ] * 6;
        x2 = piSrc0[i + 6*4 ];
        x3 = piSrc0[i + 2*4 ];
        x4 = piSrc0[i + 1*4 ];
        x5 = piSrc0[i + 7*4 ];
        x6 = piSrc0[i + 5*4 ];
        x7 = piSrc0[i + 3*4 ];
        
        // zeroth stage
        y4a = x4 + x5;
        x8 = 7 * y4a;
        x4a = x8 - 12 * x5;
        x5a = x8 - 3 * x4;
        x8 = 2 * y4a;
        x4 = x8 + 6 * x4;
        x5 = x8 - 10 * x5;
        
        ls_signbit=y4a&0x8000;
        y4a = (y4a >> 1) - ls_signbit;
        y4a = y4a & ~0x8000;
        y4a = y4a | ls_signbit;
        x4a += y4a;
        x5a += y4a;
        
        // first stage
        y4 = x6 + x7;
        x8 = 2 * y4;
        
        x4a -= x8 + 6 * x6;
        x5a += x8 - 10 * x7;
        x8 = 7 * y4;
        
        ls_signbit=y4&0x8000;
        y4 = (y4 >> 1) - ls_signbit;
        y4 = y4 & ~0x8000;
        y4 = y4 | ls_signbit;
        //        x4 += y4;
        //        x5 += y4;
        x8 += y4;
        x4 += x8 - 3 * x6;
        x5 += x8 - 12 * x7;
        
        // second stage 
        x8 = x0 + x1;
        x0 -= x1;
        
        x1 = 8 * (x2 + x3);
        x6 = x1 - 5 * x2;
        x1 -= 11 * x3;
        
        // third stage
        x7 = x8 + x6;
        x8 -= x6;
        
        x6 = x0 - x1;
        x0 += x1;
        
        // blk0
        b0 = (x7 + x4);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ 2*i + 0   + 0*8] = b0;
        blk16[ 2*i + 1 + 0*8] = b1;
        
        // blk1
        b0 = (x6 + x4a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ 2*i + 0 + 1*8] = b0;
        blk16[ 2*i + 1 + 1*8] = b1;
        
        // blk2
        b0 = (x0 + x5a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ 2*i + 0 + 2*8] = b0;
        blk16[ 2*i + 1 + 2*8] = b1;
        
        // blk3
        b0 = (x8 + x5);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ 2*i + 0 + 3*8] = b0;
        blk16[ 2*i + 1 + 3*8] = b1;
        
        // blk4
        b0 = (x8 - x5);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ 2*i + 0 + 4*8] = b0;
        blk16[ 2*i + 1 + 4*8] = b1;
        
        // blk5
        b0 = (x0 - x5a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ 2*i + 0 + 5*8] = b0;
        blk16[ 2*i + 1 + 5*8] = b1;
        
        // blk6
        b0 = (x6 - x4a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ 2*i + 0 + 6*8] = b0;
        blk16[ 2*i + 1 + 6*8] = b1;
        
        // blk7
        b0 = (x7 - x4);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ 2*i + 0 + 7*8] = b0;
        blk16[ 2*i + 1 + 7*8] = b1;
    }
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec_WMV3_Pass4_C(const I32_WMV  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops)
{
    I32_WMV i;
    I32_WMV  x1, x3, x4, x5, x6, x7, x8,  x5a;
    I32_WMV  b0,b1,ls_signbit;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV3_Pass4_C);
    DEBUG_CACHE_READ_BYTES( piSrc0, BLOCK_SIZE*iNumLoops*4 );
    DEBUG_CACHE_WRITE_STRIDES( blk16, BLOCK_SIZE*iNumLoops*2, 32*2, 2 );
    
    for (i = 0; i < iNumLoops; i++) {
        
        x4 = piSrc0[i + 0*4 ];
        x5 = piSrc0[i + 1*4 ];
        x6 = piSrc0[i + 2*4 ];
        x7 = piSrc0[i + 3*4 ];
        
        
        x3 = (x4 - x6); 
        x6 += x4;
        
        x4 = 8 * x6 + 32 + (32<<16); //rounding
        x8 = 8 * x3 + 32 + (32<<16);   //rounding
        
        /*
        x6 = (x6 >> 1);
        x3 = (x3 >> 1)
        */
        
        ls_signbit=x6&0x8000;
        x6 = (x6 >> 1) - ls_signbit;
        x6 = x6 & ~0x8000;
        x6 = x6 | ls_signbit;
        
        ls_signbit=x3&0x8000;
        x3 = (x3 >> 1) - ls_signbit;
        x3 = x3 & ~0x8000;
        x3 = x3 | ls_signbit;
        
        x4 += x6; // guaranteed to have enough head room
        x8 += x3 ;
        
        
        x1 = 5 * ( x5 + x7);
        x5a = x1 + 6 * x5;
        x5 =  x1 - 16 * x7;
        
        //simplify following to 3 multiplies
        //        x5a = 11 * x5 + 5 * x7;
        //        x5 = 5 * x5 - 11 * x7;
        
        /*
        blk0[i] = (I16_WMV) ((x4 + x5a) >> 6);
        blk1[i] = (I16_WMV) ((x8 + x5) >> 6);
        blk2[i] = (I16_WMV) ((x8 - x5) >> 6);
        blk3[i] = (I16_WMV) ((x4 - x5a) >> 6);
        */
        
        // j = i<<1;
        
        // blk0
        b0 = (x4 + x5a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ 2*i + 0 + 0*8] = b0;
        blk16[ 2*i + 1 + 0*8] = b1;
        
        
        // blk1
        b0 = (x8 + x5);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ 2*i + 0 + 1*8] = b0;
        blk16[ 2*i + 1 + 1*8] = b1;
        
        
        // blk2
        b0 = (x8 - x5);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ 2*i + 0 + 2*8] = b0;
        blk16[ 2*i + 1 + 2*8] = b1;
        
        
        // blk3
        b0 = (x4 - x5a);
        b1 = (b0 + 0x8000)>>22;
        b0 = ((I16_WMV)b0)>>6;
        blk16[ 2*i + 0 + 3*8] = b0;
        blk16[ 2*i + 1 + 3*8] = b1;
        
    } 
}

#if 0  //not used for now
//#define 4 4
// C Version
// Input:  32-bit (16 bits enough!)
// Output: 16-bit
//#pragma code_seg (EMBSEC_PML)
Void_WMV g_8x4IDCTDec_WMV3_SSIMD (UnionBuffer * piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer * rgiCoefReconBuf, I32_WMV iHalf, I32_WMV iDCTHorzFlags)
{
    I32_WMV * piDst = piDstBuf->i32 + (iHalf << 3);
    I32_WMV *blk32 ;
    I16_WMV  * blk16;
    I32_WMV tmpBuffer[16];
    const I32_WMV  * piSrc0 = rgiCoefReconBuf->i32;
    FUNCTION_PROFILE(fpDecode);
    FUNCTION_PROFILE_START(&fpDecode,IDCT_NT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_8x4IDCTDec_WMV3_SSIMD);
    
    blk32 = tmpBuffer;
    g_IDCTDec_WMV3_Pass1(piSrc0, blk32, HYBRID_PASS_4, iDCTHorzFlags);
    
    piSrc0 = tmpBuffer;
    blk16  = (I16_WMV *)piDst;
    
    g_IDCTDec_WMV3_Pass4(piSrc0,  blk16, PASS_8);
    
    FUNCTION_PROFILE_STOP(&fpDecode);
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_4x8IDCTDec_WMV3_EMB (UnionBuffer * piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer * rgiCoefReconBuf, I32_WMV iHalf, I32_WMV iDCTHorzFlags)
{
    I32_WMV * piDst = piDstBuf->i32 + (iHalf);
    I32_WMV * blk32;
    I16_WMV * blk16;
    I32_WMV tmpBuffer[4*2*4];
    const I32_WMV  * piSrc0 = rgiCoefReconBuf->i32;   
    FUNCTION_PROFILE(fpDecode)
    FUNCTION_PROFILE_START(&fpDecode,IDCT_NT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_4x8IDCTDec_WMV3_EMB);
    
    blk32 = tmpBuffer;
    
    g_IDCTDec_WMV3_Pass3(piSrc0,  blk32, HYBRID_PASS_8, iDCTHorzFlags);
    
    piSrc0 = tmpBuffer;
    blk16  = (I16_WMV *)piDst;
    
    g_IDCTDec_WMV3_Pass2(piSrc0, blk16, PASS_4);
    
    FUNCTION_PROFILE_STOP(&fpDecode);
}


//#pragma code_seg (EMBSEC_PML)
Void_WMV g_4x4IDCTDec_WMV3_EMB (UnionBuffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iQuadrant, I32_WMV iDCTHorzFlags)
{
    I16_WMV* piDst = piDstBuf->i16 + (iQuadrant&2)*8 + (iQuadrant&1)*2;
    I32_WMV  *piSrc0; 
    I32_WMV rgTemp[16];
    I16_WMV * blk16 ;
    I32_WMV * blk32 = rgTemp;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_4x4IDCTDec_WMV3_EMB);
    
    piSrc0 = (I32_WMV *)rgiCoefReconBuf->i32;
    
    g_IDCTDec_WMV3_Pass3(piSrc0,  blk32, HYBRID_PASS_4, iDCTHorzFlags);
    
    piSrc0 = rgTemp; //piDst->i32;
    blk16  = piDst;
    g_IDCTDec_WMV3_Pass4(piSrc0,  blk16, PASS_4);
}
#endif //commend out


#if !(defined(WMV_OPT_NAKED_ARM) && defined(WMV_OPT_DQUANT_ARM) && defined(WMV_OPT_IDCT_ARM))
#ifndef WMV_OPT_IDCT_SHX
//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec16_WMV3_SSIMD (UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags)
{
    I32_WMV  *piSrc0;
    const I16_WMV * rgiCoefRecon = piSrc->i16;
    I16_WMV * blk16 = piDst->i16;
    I32_WMV * blk32 = piDst->i32;    
    I32_WMV tmpBuffer[64];
    FUNCTION_PROFILE(fpDecode);
    FUNCTION_PROFILE_START(&fpDecode,IDCT_NT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec16_WMV3_SSIMD);
    
    piSrc0 = piSrc->i32;
    blk32 = tmpBuffer;
    
    g_IDCTDec_WMV3_Pass1(piSrc0, blk32, HYBRID_PASS_8, iDCTHorzFlags);
    
    piSrc0 = tmpBuffer;
    blk16  = piDst->i16;
    
    g_IDCTDec_WMV3_Pass2(piSrc0, blk16, PASS_8);
    
    FUNCTION_PROFILE_STOP(&fpDecode);
}
#endif //WMV_OPT_IDCT_SHX

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterError8x8_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ)
{
    UnionBuffer * ppxliErrorQMB = pMainLoop->m_rgiCoefReconBuf;
    I32_WMV result;
    I32_WMV *piSrc0 = ppxliErrorQMB->i32;
    I16_WMV *blk16  = ppxliErrorQMB->i16;  
    I32_WMV tmpBuffer[64];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterError8x8_SSIMD);
    
   
    result = DecodeInverseInterBlockQuantize16_EMB(pMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_8x8, 
        pDQ);
    
    g_IDCTDec_WMV3_Pass1(piSrc0, tmpBuffer, HYBRID_PASS_8, pMainLoop->m_iDCTHorzFlags);
    g_IDCTDec_WMV3_Pass2(tmpBuffer, blk16, PASS_8);
    
    return result;
}

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterError8x4_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i)
{
    UnionBuffer * ppxliErrorQMB = pMainLoop->m_ppxliErrorQ;
    I32_WMV result;
    I32_WMV * piDst = ppxliErrorQMB->i32 + (i << 3);
    I16_WMV  * blk16;
    I32_WMV tmpBuffer[16];
    const I32_WMV  * piSrc0 = pMainLoop->m_rgiCoefReconBuf->i32;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterError8x4_SSIMD);
    
    result = DecodeInverseInterBlockQuantize16_EMB(pMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_8x4, 
        pDQ);
        
    g_IDCTDec_WMV3_Pass1(piSrc0, tmpBuffer, HYBRID_PASS_4, pMainLoop->m_iDCTHorzFlags);
        
    blk16  = (I16_WMV *)piDst;
    
    g_IDCTDec_WMV3_Pass4(tmpBuffer,  blk16, PASS_8);
    
    return result;
}



//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterError4x8_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i)
{
    UnionBuffer * ppxliErrorQMB = pMainLoop->m_ppxliErrorQ;
    I32_WMV result;
    I32_WMV * piDst = ppxliErrorQMB->i32 + (i);
    I16_WMV * blk16;
    I32_WMV tmpBuffer[4*2*4];
    const I32_WMV  * piSrc0 = pMainLoop->m_rgiCoefReconBuf->i32;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterError4x8_SSIMD);
    
    
    result = DecodeInverseInterBlockQuantize16_EMB(pMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_4x8, 
        pDQ);
    
    g_IDCTDec_WMV3_Pass3(piSrc0,  tmpBuffer, HYBRID_PASS_8, pMainLoop->m_iDCTHorzFlags);
    blk16  = (I16_WMV *)piDst;
    g_IDCTDec_WMV3_Pass2(tmpBuffer, blk16, PASS_4);
    
    return result;
}

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterError4x4_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i)
{
    UnionBuffer * ppxliErrorQMB = pMainLoop->m_ppxliErrorQ;
    I32_WMV result;
    I16_WMV* piDst = ppxliErrorQMB->i16 + (i&2)*8 + (i&1)*2;
    I32_WMV  *piSrc0; 
    I32_WMV rgTemp[16];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterError4x4_SSIMD);
    
    result = DecodeInverseInterBlockQuantize16_EMB(pMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_4x4, 
        pDQ);
    
    piSrc0 = (I32_WMV *)pMainLoop->m_rgiCoefReconBuf->i32;
    g_IDCTDec_WMV3_Pass3(piSrc0,  rgTemp, HYBRID_PASS_4, pMainLoop->m_iDCTHorzFlags);
    
    //blk16  = piDst;
    g_IDCTDec_WMV3_Pass4(rgTemp,  piDst, PASS_4);
    
    return result;
}

#if 0
//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterErrorB8x8_SSIMD(EMB_BMainLoop  * pBMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ)
{
    UnionBuffer * ppxliErrorQMB = pBMainLoop->m_rgiCoefReconBuf;
    I32_WMV result;
    I32_WMV *piSrc0 = ppxliErrorQMB->i32;
    I16_WMV *blk16  = ppxliErrorQMB->i16;  
    I32_WMV tmpBuffer[64];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterErrorB8x8_SSIMD);
    
   
    result = DecodeInverseInterBlockQuantize16B_EMB(pBMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_8x8, 
        pDQ);
    
    g_IDCTDec_WMV3_Pass1(piSrc0, tmpBuffer, HYBRID_PASS_8, pBMainLoop->m_iDCTHorzFlags);
    g_IDCTDec_WMV3_Pass2(tmpBuffer, blk16, PASS_8);
    
    return result;
}


//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterErrorB8x4_SSIMD(EMB_BMainLoop  * pBMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i)
{
    UnionBuffer * ppxliErrorQMB = pBMainLoop->m_ppxliErrorQ;
    I32_WMV result;
    I32_WMV * piDst = ppxliErrorQMB->i32 + (i << 3);
    I16_WMV  * blk16;
    I32_WMV tmpBuffer[16];
    const I32_WMV  * piSrc0 = pBMainLoop->m_rgiCoefReconBuf->i32;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterErrorB8x4_SSIMD);
    
    result = DecodeInverseInterBlockQuantize16B_EMB(pBMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_8x4, 
        pDQ);
        
    g_IDCTDec_WMV3_Pass1(piSrc0, tmpBuffer, HYBRID_PASS_4, pBMainLoop->m_iDCTHorzFlags);
        
    blk16  = (I16_WMV *)piDst;
    
    g_IDCTDec_WMV3_Pass4(tmpBuffer,  blk16, PASS_8);
    
    return result;
}


//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterErrorB4x8_SSIMD(EMB_BMainLoop  * pBMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i)
{
    UnionBuffer * ppxliErrorQMB = pBMainLoop->m_ppxliErrorQ;
    I32_WMV result;
    I32_WMV * piDst = ppxliErrorQMB->i32 + (i);
    I16_WMV * blk16;
    I32_WMV tmpBuffer[4*2*4];
    const I32_WMV  * piSrc0 = pBMainLoop->m_rgiCoefReconBuf->i32;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterErrorB4x8_SSIMD);
    
    result = DecodeInverseInterBlockQuantize16B_EMB(pBMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_4x8, 
        pDQ);
    
    g_IDCTDec_WMV3_Pass3(piSrc0,  tmpBuffer, HYBRID_PASS_8, pBMainLoop->m_iDCTHorzFlags);
    blk16  = (I16_WMV *)piDst;
    g_IDCTDec_WMV3_Pass2(tmpBuffer, blk16, PASS_4);
    
    return result;
}


//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterErrorB4x4_SSIMD(EMB_BMainLoop  * pBMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i)
{
    UnionBuffer * ppxliErrorQMB = pBMainLoop->m_ppxliErrorQ;
    I32_WMV result;
    I16_WMV* piDst = ppxliErrorQMB->i16 + (i&2)*8 + (i&1)*2;
    I32_WMV  *piSrc0; 
    I32_WMV rgTemp[16];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterErrorB4x4_SSIMD);
    
    result = DecodeInverseInterBlockQuantize16B_EMB(pBMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_4x4, 
        pDQ);
    
    piSrc0 = (I32_WMV *)pBMainLoop->m_rgiCoefReconBuf->i32;
    g_IDCTDec_WMV3_Pass3(piSrc0,  rgTemp, HYBRID_PASS_4, pBMainLoop->m_iDCTHorzFlags);
    
    //blk16  = piDst;
    g_IDCTDec_WMV3_Pass4(rgTemp,  piDst, PASS_4);
    
    return result;
}
#endif  //!(defined(WMV_OPT_NAKED_ARM) && defined(WMV_OPT_DQUANT_ARM) && defined(WMV_OPT_IDCT_ARM))
#endif
#endif  //_EMB_SSIMD32_

#ifndef _EMB_SSIMD_MC_ //sw: Not used for ARM version.

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterError8x8_C(EMB_PBMainLoop  * pPMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ)
{
    UnionBuffer * ppxliErrorQMB = pPMainLoop->m_rgiCoefReconBuf;
    I32_WMV result;
    I32_WMV *piSrc0 = ppxliErrorQMB->i32;
    I16_WMV *blk16  = ppxliErrorQMB->i16;  
    I32_WMV tmpBuffer[64];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterError8x8_C);
    
   
    result = DecodeInverseInterBlockQuantize16_EMB(pPMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_8x8, 
        pDQ);
    
    g_IDCTDec_WMV3_Pass1(piSrc0, tmpBuffer, HYBRID_PASS_8, pPMainLoop->m_iDCTHorzFlags);
    g_IDCTDec_WMV3_Pass2_C(tmpBuffer, blk16, PASS_8);
    
    return result;
}

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterError4x4_C(EMB_PBMainLoop  * pPMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i)
{
    UnionBuffer * ppxliErrorQMB = pPMainLoop->m_ppxliErrorQ;
    I32_WMV result;
    I16_WMV* piDst = ppxliErrorQMB->i16 + (i&2)*16 + (i&1)*4;
    I32_WMV  *piSrc0; 
    I32_WMV rgTemp[16];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterError4x4_C);
    
    result = DecodeInverseInterBlockQuantize16_EMB(pPMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_4x4, 
        pDQ);
    
    piSrc0 = (I32_WMV *)pPMainLoop->m_rgiCoefReconBuf->i32;
    g_IDCTDec_WMV3_Pass3(piSrc0,  rgTemp, HYBRID_PASS_4, pPMainLoop->m_iDCTHorzFlags);
    
    //blk16  = piDst;
    g_IDCTDec_WMV3_Pass4_C(rgTemp,  piDst, PASS_4);
    
    return result;
}

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterError4x8_C(EMB_PBMainLoop  * pPMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i)
{
    UnionBuffer * ppxliErrorQMB = pPMainLoop->m_ppxliErrorQ;
    I32_WMV result;
    I32_WMV * piDst = ppxliErrorQMB->i32 + (2*i);
    I16_WMV * blk16;
    I32_WMV tmpBuffer[4*2*4];
    const I32_WMV  * piSrc0 = pPMainLoop->m_rgiCoefReconBuf->i32;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterError4x8_C);
    
    result = DecodeInverseInterBlockQuantize16_EMB(pPMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_4x8, 
        pDQ);
    
    g_IDCTDec_WMV3_Pass3(piSrc0,  tmpBuffer, HYBRID_PASS_8, pPMainLoop->m_iDCTHorzFlags);
    blk16  = (I16_WMV *)piDst;
    g_IDCTDec_WMV3_Pass2_C(tmpBuffer, blk16, PASS_4);
    
    return result;
}

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterError8x4_C(EMB_PBMainLoop  * pPMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i)
{
    UnionBuffer * ppxliErrorQMB = pPMainLoop->m_ppxliErrorQ;
    I32_WMV result;
    I32_WMV * piDst = ppxliErrorQMB->i32 + (i << 4);
    I16_WMV  * blk16;
    I32_WMV tmpBuffer[16];
    const I32_WMV  * piSrc0 = pPMainLoop->m_rgiCoefReconBuf->i32;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterError8x4_C);
    
    result = DecodeInverseInterBlockQuantize16_EMB(pPMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_8x4, 
        pDQ);
        
    g_IDCTDec_WMV3_Pass1(piSrc0, tmpBuffer, HYBRID_PASS_4, pPMainLoop->m_iDCTHorzFlags);
        
    blk16  = (I16_WMV *)piDst;
    
    g_IDCTDec_WMV3_Pass4_C(tmpBuffer,  blk16, PASS_8);
    
    return result;
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec16_WMV3_C (UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags)
{
    I32_WMV  *piSrc0;
    const I16_WMV * rgiCoefRecon = piSrc->i16;
    I16_WMV * blk16 = piDst->i16;
    I32_WMV * blk32 = piDst->i32;    
    I32_WMV tmpBuffer[64];
    FUNCTION_PROFILE(fpDecode);
    FUNCTION_PROFILE_START(&fpDecode,IDCT_NT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec16_WMV3_C);
    
    piSrc0 = piSrc->i32;
    blk32 = tmpBuffer;
    
    g_IDCTDec_WMV3_Pass1(piSrc0, blk32, HYBRID_PASS_8, iDCTHorzFlags);
    
    piSrc0 = tmpBuffer;
    blk16  = piDst->i16;
    
    g_IDCTDec_WMV3_Pass2_C(piSrc0, blk16, PASS_8);
    
    FUNCTION_PROFILE_STOP(&fpDecode);
}

#endif



/*
 * This is to define _EMB_SSIMD64_ iDCT functions
 */
#if _EMB_IDCT_SSIMD64_

#define SATURATE8(x)   (I8_WMV)((x) < 0 ? 0 : ((x) > 255 ? 255 : (x)))

#define RESTORE16VAL(val) \
    if ((val) & 0x800080008000){ \
        (val) += ((val) & 0x8000) << 1; \
        (val) += ((val) & 0x80000000) << 1; \
        (val) += ((val) & 0x800000000000) << 1; \
    }


/*
 * rgiCoefRecon must be 8 bytes aligned.
 */

const I64_WMV   g_Round64_4  = 0x4000400040004;
const I64_WMV   g_Round64_32 = 0x20002000200020;


Void_WMV g_IDCTDec_WMV3 (U8_WMV __huge* piDst, I32_WMV iOffsetToNextRowForDCT, const I32_WMV __huge* rgiCoefRecon)
{
    I64_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a;
    I64_WMV  y3, y4, y5, y4a, b0,c0,d0,e0,b1,c1,d1,e1,ls_signbit;
    I64_WMV  *piSrc0;
    I64_WMV i,j;
    register I64_WMV iDCTVertFlag = 0;

    I64_WMV tmpBuffer[16];
    I64_WMV * blk64 = tmpBuffer; // piDst->i32;
    
    U8_WMV __huge * blk0 ;
    U8_WMV __huge * blk1 ;
    U8_WMV __huge * blk2 ;
    U8_WMV __huge * blk3 ;
    U8_WMV __huge * blk4 ;
    U8_WMV __huge * blk5 ;
    U8_WMV __huge * blk6 ;
    U8_WMV __huge * blk7 ;
    
    FUNCTION_PROFILE(fpDecode);
    FUNCTION_PROFILE_START(&fpDecode,IDCT_NT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV3);
    
    piSrc0 = (I64_WMV *) rgiCoefRecon;
    
    for ( i = 0; i < (BLOCK_SIZE>>2); i++,  blk64 += 8){
        // Check to see if this row has all zero coefficients in columns 1-7 (0 is first column)
        //        if (!(iDCTHorzFlags & (1 << i))) {
        //            // Columns 1-7 must be zero. Check to see if column zero coefficient is zero. 
        //            // If so, we don't have to do anything more for this row
        //            // Column zero cofficient is nonzero so all coefficients in this row are DC
        //            blk [0] = blk [1] = blk [2] = blk [3] = blk [4] = blk [5] = blk [6] = blk [7] = rgiCoefRecon [0] << 3;
        //            // If this is not the first row, tell the second stage that it can't use DC
        //            // shortcut - it has to do the full transform for all the columns
        //            iDCTVertFlag = i;
        //          continue;
        //      }
        
        // If this is not the first row, tell the second stage that it can't use DC shortcut 
        // - it has to do the full transform for all the columns
        
        x4 = piSrc0[ i +1*2 ];      
        x3 = piSrc0[ i +2*2 ];
        x7 = piSrc0[ i +3*2 ];
        x1 = piSrc0[ i +4*2 ];
        x6 = piSrc0[ i +5*2 ];
        x2 = piSrc0[ i +6*2 ];      
        x5 = piSrc0[ i +7*2 ];
        x0 = piSrc0[ i +0*2 ]; /* for proper rounding */
        
        x1 = x1 * W0;
        x0 = x0 * W0 + g_Round64_4; /* for proper rounding */
        
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
        
        b0 = x7 + x4;
        c0 = x3 + x4a;
        d0 = x0 + x5a;
        e0 = x8 + x5;

        RESTORE16VAL(b0);
        RESTORE16VAL(c0);
        RESTORE16VAL(d0);
        RESTORE16VAL(e0);

        b1 = (b0 << 48)>>51;
        c1 = (c0 << 48)>>51;
        d1 = (d0 << 48)>>51;
        e1 = (e0 << 48)>>51;
        blk64[0] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

        b1 = ((b0 << 32))>>51;
        c1 = ((c0 << 32))>>51;
        d1 = ((d0 << 32))>>51;
        e1 = ((e0 << 32))>>51;
        blk64[2] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

        b1 = ((b0 << 16))>>51;
        c1 = ((c0 << 16))>>51;
        d1 = ((d0 << 16))>>51;
        e1 = ((e0 << 16))>>51;
        blk64[4] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

        b1 = (b0)>>51;
        c1 = (c0)>>51;
        d1 = (d0)>>51;
        e1 = (e0)>>51;
        blk64[6] = (e1<<48) + (d1<<32) + (c1<<16) + b1;


        b0 = x8 - x5;
        c0 = x0 - x5a;
        d0 = x3 - x4a;
        e0 = x7 - x4;

        RESTORE16VAL(b0);
        RESTORE16VAL(c0);
        RESTORE16VAL(d0);
        RESTORE16VAL(e0);
        
        b1 = (b0 << 48)>>51;
        c1 = (c0 << 48)>>51;
        d1 = (d0 << 48)>>51;
        e1 = (e0 << 48)>>51;
        blk64[1] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

        b1 = ((b0 << 32))>>51;
        c1 = ((c0 << 32))>>51;
        d1 = ((d0 << 32))>>51;
        e1 = ((e0 << 32))>>51;
        blk64[3] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

        b1 = ((b0 << 16))>>51;
        c1 = ((c0 << 16))>>51;
        d1 = ((d0 << 16))>>51;
        e1 = ((e0 << 16))>>51;
        blk64[5] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

        b1 = (b0)>>51;
        c1 = (c0)>>51;
        d1 = (d0)>>51;
        e1 = (e0)>>51;
        blk64[7] = (e1<<48) + (d1<<32) + (c1<<16) + b1;
    }
    
    blk0 = piDst;
    blk1 = blk0 + iOffsetToNextRowForDCT;
    blk2 = blk1 + iOffsetToNextRowForDCT;
    blk3 = blk2 + iOffsetToNextRowForDCT;
    blk4 = blk3 + iOffsetToNextRowForDCT;
    blk5 = blk4 + iOffsetToNextRowForDCT;
    blk6 = blk5 + iOffsetToNextRowForDCT;
    blk7 = blk6 + iOffsetToNextRowForDCT;
    
    for (i = 0; i < (BLOCK_SIZE>>2); i++){
        // If iDCTVertFlag is zero then we know that blk[1] thru blk[7] are zero.
        // Therefore, if blk[0] is zero we can skip to the next column. Otherwise we
        // can use the DC shortcut and just copy blk[0] to the rest of the column.
        //        if (iDCTVertFlag == 0){
        //            // If blk[0] is nonzero, copy the value to the other 7 rows in this column        
        //            if (blk0[i] != 0)
        //              blk0[i] = blk1[i] = blk2[i] = blk3[i] = blk4[i] = blk5[i] = blk6[i] = blk7[i] = (blk0[i] + 32) >> 6;
        //          continue;
        //      }
        x0 = tmpBuffer[i + 0*2 ] * 6 + g_Round64_32 /* rounding */;
        x1 = tmpBuffer[i + 4*2 ] * 6;
        x2 = tmpBuffer[i + 6*2 ];
        x3 = tmpBuffer[i + 2*2 ];
        x4 = tmpBuffer[i + 1*2 ];
        x5 = tmpBuffer[i + 7*2 ];
        x6 = tmpBuffer[i + 5*2 ];
        x7 = tmpBuffer[i + 3*2 ];
        
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
        
        x1 = 8 * (x2 + x3);
        x6 = x1 - 5 * x2;
        x1 -= 11 * x3;
        
        // third stage
        x7 = x8 + x6;
        x8 -= x6;
        
        x6 = x0 - x1;
        x0 += x1;

        y5 = y4;
        RESTORE16VAL(y5);

        y4 -= y5;
        ls_signbit = y5 & 0x800080008000;
        y5 = (y5 & ~0x1000100010000) >> 1;
        y5 = y5 | ls_signbit;
        y5 += y4;

        y3 = y4a;
        RESTORE16VAL(y3);

        y4a -= y3;
        ls_signbit= y3 & 0x800080008000;
        y3 = (y3 & ~0x1000100010000) >> 1;
        y3 = y3 | ls_signbit;
        y3 += y4a;

/*
        ls_signbit=y4&0x800080008000;
        y5 = (y4 >> 1) - ls_signbit;
        y5 = y5 & ~0x800080008000;
        y5 = y5 | ls_signbit;
        
        ls_signbit=y4a&0x800080008000;
        y3 = (y4a >> 1) - ls_signbit;
        y3 = y3 & ~0x800080008000;
        y3 = y3 | ls_signbit;
*/        
        // fourth stage
        // Change 4:
        
        x4 += y5;
        x5 += y5;
        x4a += y3;
        x5a += y3;
        
        /*
        
          _blk0 [i] = (x7 + x4) >> 6;
          _blk1 [i] = (x6 + x4a) >> 6;
          _blk2 [i] = (x0 + x5a) >> 6;
          _blk3 [i] = (x8 + x5) >> 6;
          _blk4 [i] = (x8 - x5) >> 6;
          _blk5 [i] = (x0 - x5a) >> 6;
          _blk6 [i] = (x6 - x4a) >> 6;
          _blk7 [i] = (x7 - x4) >> 6;
          
        */
        
        j = i<<2;
        
        // blk0
        b0 = (x7 + x4);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk0[ j ] = SATURATE8(b1);
        b1 = ((b0 << 32))>>54;
        blk0[ j+1] = SATURATE8(b1);
        b1 = ((b0 << 16))>>54;
        blk0[ j+2] = SATURATE8(b1);
        b1 = (b0)>>54;
        blk0[ j+3] = SATURATE8(b1);
        
        // blk1
        b0 = (x6 + x4a);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk1[ j ] = SATURATE8(b1);
        b1 = ((b0 << 32))>>54;
        blk1[ j+1] = SATURATE8(b1);
        b1 = ((b0 << 16))>>54;
        blk1[ j+2] = SATURATE8(b1);
        b1 = (b0)>>54;
        blk1[ j+3] = SATURATE8(b1);
        
        // blk2
        b0 = (x0 + x5a);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk2[ j ] = SATURATE8(b1);
        b1 = ((b0 << 32))>>54;
        blk2[ j+1] = SATURATE8(b1);
        b1 = ((b0 << 16))>>54;
        blk2[ j+2] = SATURATE8(b1);
        b1 = (b0)>>54;
        blk2[ j+3] = SATURATE8(b1);
        
        // blk3
        b0 = (x8 + x5);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk3[ j ] = SATURATE8(b1);
        b1 = ((b0 << 32))>>54;
        blk3[ j+1] = SATURATE8(b1);
        b1 = ((b0 << 16))>>54;
        blk3[ j+2] = SATURATE8(b1);
        b1 = (b0)>>54;
        blk3[ j+3] = SATURATE8(b1);
        
        // blk4
        b0 = (x8 - x5);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk4[ j ] = SATURATE8(b1);
        b1 = ((b0 << 32))>>54;
        blk4[ j+1] = SATURATE8(b1);
        b1 = ((b0 << 16))>>54;
        blk4[ j+2] = SATURATE8(b1);
        b1 = (b0)>>54;
        blk4[ j+3] = SATURATE8(b1);
        
        // blk5
        b0 = (x0 - x5a);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk5[ j ] = SATURATE8(b1);
        b1 = ((b0 << 32))>>54;
        blk5[ j+1] = SATURATE8(b1);
        b1 = ((b0 << 16))>>54;
        blk5[ j+2] = SATURATE8(b1);
        b1 = (b0)>>54;
        blk5[ j+3] = SATURATE8(b1);
        
        // blk6
        b0 = (x6 - x4a);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk6[ j ] = SATURATE8(b1);
        b1 = ((b0 << 32))>>54;
        blk6[ j+1] = SATURATE8(b1);
        b1 = ((b0 << 16))>>54;
        blk6[ j+2] = SATURATE8(b1);
        b1 = (b0)>>54;
        blk6[ j+3] = SATURATE8(b1);
        
        // blk7
        b0 = (x7 - x4);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk7[ j ] = SATURATE8(b1);
        b1 = ((b0 << 32))>>54;
        blk7[ j+1] = SATURATE8(b1);
        b1 = ((b0 << 16))>>54;
        blk7[ j+2] = SATURATE8(b1);
        b1 = (b0)>>54;
        blk7[ j+3] = SATURATE8(b1);
    }
    FUNCTION_PROFILE_STOP(&fpDecode);
}


///////////////////////////////////////////////////////////////////////////
// Inter IDCT Functions
// (this fn and one below are almost repeats of each other!!! - INEFFICIENT!)
///////////////////////////////////////////////////////////////////////////
// C Version
// Input:  16-bit
// Output: 16-bit
Void_WMV g_IDCTDec16_WMV3 (UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags)
{
    I64_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a;
    I64_WMV  y3, y4, y5, y4a, b0,c0,d0,e0,b1,c1,d1,e1,ls_signbit;
    I64_WMV  *piSrc0;
    I64_WMV  i,j;
    I64_WMV tmpBuffer[16];
    I64_WMV * blk64;

    I16_WMV * blk16 = piDst->i16;
    
    FUNCTION_PROFILE(fpDecode);
    FUNCTION_PROFILE_START(&fpDecode,IDCT_NT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec16_WMV3);
    
    piSrc0 = (I64_WMV *)piSrc->i32;
    
    blk64 = tmpBuffer;
    
    for ( i = 0; i < (BLOCK_SIZE>>2); i++,  blk64 += 8){
        // Check to see if this row has all zero coefficients in columns 1-7 (0 is first column)
        //        if (!(iDCTHorzFlags & (1 << i))) {
        //            // Columns 1-7 must be zero. Check to see if column zero coefficient is zero. 
        //            // If so, we don't have to do anything more for this row
        //            // Column zero cofficient is nonzero so all coefficients in this row are DC
        //            blk [0] = blk [1] = blk [2] = blk [3] = blk [4] = blk [5] = blk [6] = blk [7] = rgiCoefRecon [0] << 3;
        //            // If this is not the first row, tell the second stage that it can't use DC
        //            // shortcut - it has to do the full transform for all the columns
        //            iDCTVertFlag = i;
        //          continue;
        //      }
        
        // If this is not the first row, tell the second stage that it can't use DC shortcut 
        // - it has to do the full transform for all the columns
        
        x4 = piSrc0[ i +1*2 ];      
        x3 = piSrc0[ i +2*2 ];
        x7 = piSrc0[ i +3*2 ];
        x1 = piSrc0[ i +4*2 ];
        x6 = piSrc0[ i +5*2 ];
        x2 = piSrc0[ i +6*2 ];      
        x5 = piSrc0[ i +7*2 ];
        x0 = piSrc0[ i +0*2 ]; /* for proper rounding */
        
        
        x1 = x1 * W0;
        x0 = x0 * W0 + g_Round64_4; /* for proper rounding */
        
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
        /*
        blk [0] = (I16_WMV) ((x7 + x4) >> 3);
        blk [1] = (I16_WMV) ((x3 + x4a) >> 3);
        blk [2] = (I16_WMV) ((x0 + x5a) >> 3);
        blk [3] = (I16_WMV) ((x8 + x5) >> 3);
        blk [4] = (I16_WMV) ((x8 - x5) >> 3);
        blk [5] = (I16_WMV) ((x0 - x5a) >> 3);
        blk [6] = (I16_WMV) ((x3 - x4a) >> 3);
        blk [7] = (I16_WMV) ((x7 - x4) >> 3);
        */
        
        // blk [0,1]
        b0 = x7 + x4;
        c0 = x3 + x4a;
        d0 = x0 + x5a;
        e0 = x8 + x5;

        RESTORE16VAL(b0);
        RESTORE16VAL(c0);
        RESTORE16VAL(d0);
        RESTORE16VAL(e0);
        
        b1 = (b0 << 48)>>51;
        c1 = (c0 << 48)>>51;
        d1 = (d0 << 48)>>51;
        e1 = (e0 << 48)>>51;
        blk64[0] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

        b1 = (b0 << 32)>>51;
        c1 = (c0 << 32)>>51;
        d1 = (d0 << 32)>>51;
        e1 = (e0 << 32)>>51;
        blk64[0+1*2] = (e1<<48) + (d1<<32) + (c1<<16) + b1;
        
        b1 = (b0 << 16)>>51;
        c1 = (c0 << 16)>>51;
        d1 = (d0 << 16)>>51;
        e1 = (e0 << 16)>>51;
        blk64[0+2*2] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

        b1 = b0>>51;
        c1 = c0>>51;
        d1 = d0>>51;
        e1 = e0>>51;
        blk64[0+3*2] = (e1<<48) + (d1<<32) + (c1<<16) + b1;


        b0 = x8 - x5;
        c0 = x0 - x5a;
        d0 = x3 - x4a;
        e0 = x7 - x4;
       
        RESTORE16VAL(b0);
        RESTORE16VAL(c0);
        RESTORE16VAL(d0);
        RESTORE16VAL(e0);

        b1 = (b0 << 48)>>51;
        c1 = (c0 << 48)>>51;
        d1 = (d0 << 48)>>51;
        e1 = (e0 << 48)>>51;
        blk64[1] = (e1<<48) + (d1<<32) + (c1<<16) + b1;
        
        b1 = (b0 << 32)>>51;
        c1 = (c0 << 32)>>51;
        d1 = (d0 << 32)>>51;
        e1 = (e0 << 32)>>51;
        blk64[1+1*2] = (e1<<48) + (d1<<32) + (c1<<16) + b1;
        
        b1 = (b0 << 16)>>51;
        c1 = (c0 << 16)>>51;
        d1 = (d0 << 16)>>51;
        e1 = (e0 << 16)>>51;
        blk64[1+2*2] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

        b1 = b0>>51;
        c1 = c0>>51;
        d1 = d0>>51;
        e1 = e0>>51;
        blk64[1+3*2] = (e1<<48) + (d1<<32) + (c1<<16) + b1;
    }
    
    blk16  = piDst->i16;
    
    for (i = 0; i < (BLOCK_SIZE>>2); i++)
    {
        // If iDCTVertFlag is zero then we know that blk[1] thru blk[7] are zero.
        // Therefore, if blk[0] is zero we can skip to the next column. Otherwise we
        // can use the DC shortcut and just copy blk[0] to the rest of the column.
        //        if (iDCTVertFlag == 0){
        //            // If blk[0] is nonzero, copy the value to the other 7 rows in this column        
        //            if (blk0[i] != 0)
        //              blk0[i] = blk1[i] = blk2[i] = blk3[i] = blk4[i] = blk5[i] = blk6[i] = blk7[i] = (blk0[i] + 32) >> 6;
        //          continue;
        //      }
        x0 = tmpBuffer[i + 0*2 ] * 6 + g_Round64_32 /* rounding */;
        x1 = tmpBuffer[i + 4*2 ] * 6;
        x2 = tmpBuffer[i + 6*2 ];
        x3 = tmpBuffer[i + 2*2 ];
        x4 = tmpBuffer[i + 1*2 ];
        x5 = tmpBuffer[i + 7*2 ];
        x6 = tmpBuffer[i + 5*2 ];
        x7 = tmpBuffer[i + 3*2 ];
        
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
        
        x1 = 8 * (x2 + x3);
        x6 = x1 - 5 * x2;
        x1 -= 11 * x3;
        
        // third stage
        x7 = x8 + x6;
        x8 -= x6;
        
        x6 = x0 - x1;
        x0 += x1;
        
        /*
        y5 = y4 >> 1;
        y3 = y4a >> 1;
        */
        
        y5 = y4;
        RESTORE16VAL(y5);
        y4 -= y5;
        ls_signbit = y5 & 0x800080008000;
        y5 = (y5 & ~0x1000100010000) >> 1;
        y5 = y5 | ls_signbit;
        y5 += y4;

        y3 = y4a;
        RESTORE16VAL(y3);
        y4a -= y3;

        ls_signbit= y3 & 0x800080008000;
        y3 = (y3 & ~0x1000100010000) >> 1;
        y3 = y3 | ls_signbit;
        y3 += y4a;
        
        // fourth stage
        // Change 4:
        
        x4 += y5;
        x5 += y5;
        x4a += y3;
        x5a += y3;
        
        /*
        
          _blk0 [i] = (x7 + x4) >> 6;
          _blk1 [i] = (x6 + x4a) >> 6;
          _blk2 [i] = (x0 + x5a) >> 6;
          _blk3 [i] = (x8 + x5) >> 6;
          _blk4 [i] = (x8 - x5) >> 6;
          _blk5 [i] = (x0 - x5a) >> 6;
          _blk6 [i] = (x6 - x4a) >> 6;
          _blk7 [i] = (x7 - x4) >> 6;
          
        */
        
        j = i<<2;
        
        // blk0
        b0 = (x7 + x4);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk16[ j + 0 + 0*8 ] = (I16_WMV)(b1);
        b1 = (b0 << 32)>>54;
        blk16[ j + 1 + 0*8 ] = (I16_WMV)(b1);
        b1 = (b0 << 16)>>54;
        blk16[ j + 2 + 0*8 ] = (I16_WMV)(b1);
        b1 = b0>>54;
        blk16[ j + 3 + 0*8 ] = (I16_WMV)(b1);

        // blk1
        b0 = (x6 + x4a);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk16[ j + 0 + 1*8 ] = (I16_WMV)(b1);
        b1 = (b0 << 32)>>54;
        blk16[ j + 1 + 1*8 ] = (I16_WMV)(b1);
        b1 = (b0 << 16)>>54;
        blk16[ j + 2 + 1*8 ] = (I16_WMV)(b1);
        b1 = b0>>54;
        blk16[ j + 3 + 1*8 ] = (I16_WMV)(b1);
        
        // blk2
        b0 = (x0 + x5a);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk16[ j + 0 + 2*8 ] = (I16_WMV)(b1);
        b1 = (b0 << 32)>>54;
        blk16[ j + 1 + 2*8 ] = (I16_WMV)(b1);
        b1 = (b0 << 16)>>54;
        blk16[ j + 2 + 2*8 ] = (I16_WMV)(b1);
        b1 = b0>>54;
        blk16[ j + 3 + 2*8 ] = (I16_WMV)(b1);
        
        // blk3
        b0 = (x8 + x5);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk16[ j + 0 + 3*8 ] = (I16_WMV)(b1);
        b1 = (b0 << 32)>>54;
        blk16[ j + 1 + 3*8 ] = (I16_WMV)(b1);
        b1 = (b0 << 16)>>54;
        blk16[ j + 2 + 3*8 ] = (I16_WMV)(b1);
        b1 = b0>>54;
        blk16[ j + 3 + 3*8 ] = (I16_WMV)(b1);
        
        // blk4
        b0 = (x8 - x5);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk16[ j + 0 + 4*8 ] = (I16_WMV)(b1);
        b1 = (b0 << 32)>>54;
        blk16[ j + 1 + 4*8 ] = (I16_WMV)(b1);
        b1 = (b0 << 16)>>54;
        blk16[ j + 2 + 4*8 ] = (I16_WMV)(b1);
        b1 = b0>>54;
        blk16[ j + 3 + 4*8 ] = (I16_WMV)(b1);
        
        // blk5
        b0 = (x0 - x5a);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk16[ j + 0 + 5*8 ] = (I16_WMV)(b1);
        b1 = (b0 << 32)>>54;
        blk16[ j + 1 + 5*8 ] = (I16_WMV)(b1);
        b1 = (b0 << 16)>>54;
        blk16[ j + 2 + 5*8 ] = (I16_WMV)(b1);
        b1 = b0>>54;
        blk16[ j + 3 + 5*8 ] = (I16_WMV)(b1);
        
        // blk6
        b0 = (x6 - x4a);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk16[ j + 0 + 6*8 ] = (I16_WMV)(b1);
        b1 = (b0 << 32)>>54;
        blk16[ j + 1 + 6*8 ] = (I16_WMV)(b1);
        b1 = (b0 << 16)>>54;
        blk16[ j + 2 + 6*8 ] = (I16_WMV)(b1);
        b1 = b0>>54;
        blk16[ j + 3 + 6*8 ] = (I16_WMV)(b1);
        
        // blk7
        b0 = (x7 - x4);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk16[ j + 0 + 7*8 ] = (I16_WMV)(b1);
        b1 = (b0 << 32)>>54;
        blk16[ j + 1 + 7*8 ] = (I16_WMV)(b1);
        b1 = (b0 << 16)>>54;
        blk16[ j + 2 + 7*8 ] = (I16_WMV)(b1);
        b1 = b0>>54;
        blk16[ j + 3 + 7*8 ] = (I16_WMV)(b1);
    }
    FUNCTION_PROFILE_STOP(&fpDecode);
}


// C Version
// Input:  32-bit (16 bits enough!)
// Output: 16-bit
Void_WMV g_8x4IDCTDec_WMV3 (UnionBuffer * piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer * rgiCoefReconBuf, I32_WMV iHalf)
{
    I64_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, y3, y5, x4a, x5a, b0,c0,d0,e0,b1,c1,d1,e1,ls_signbit;
    I64_WMV i, j;
    const I64_WMV  * piSrc0;
    I64_WMV tmpBuffer[8];

    I32_WMV *piDst = piDstBuf->i32 + (iHalf << 4);
    I16_WMV *blk0, *blk1, *blk2, *blk3;

    FUNCTION_PROFILE(fpDecode);
    FUNCTION_PROFILE_START(&fpDecode,IDCT_NT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec16_WMV3);
    
    piSrc0 = (I64_WMV *) rgiCoefReconBuf->i32;

    x4 = piSrc0[1];      
    x3 = piSrc0[2];
    x7 = piSrc0[3];
    x1 = piSrc0[4];
    x6 = piSrc0[5];
    x2 = piSrc0[6];      
    x5 = piSrc0[7];
    x0 = piSrc0[0]; /* for proper rounding */
    
    x1 = x1 * W0;
    x0 = x0 * W0 + g_Round64_4; /* for proper rounding */
    
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
    
    // blk [0,1]
    b0 = x7 + x4;
    c0 = x3 + x4a;
    d0 = x0 + x5a;
    e0 = x8 + x5;

    RESTORE16VAL(b0);
    RESTORE16VAL(c0);
    RESTORE16VAL(d0);
    RESTORE16VAL(e0);

    b1 = (b0 << 48)>>51;
    c1 = (c0 << 48)>>51;
    d1 = (d0 << 48)>>51;
    e1 = (e0 << 48)>>51;
    tmpBuffer[0] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

    b1 = (b0 << 32)>>51;
    c1 = (c0 << 32)>>51;
    d1 = (d0 << 32)>>51;
    e1 = (e0 << 32)>>51;
    tmpBuffer[0+1*2] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

    b1 = (b0 << 16)>>51;
    c1 = (c0 << 16)>>51;
    d1 = (d0 << 16)>>51;
    e1 = (e0 << 16)>>51;
    tmpBuffer[0+2*2] = (e1<<48) + (d1<<32) + (c1<<16) + b1;
    
    b1 = b0>>51;
    c1 = c0>>51;
    d1 = d0>>51;
    e1 = e0>>51;
    tmpBuffer[0+3*2] = (e1<<48) + (d1<<32) + (c1<<16) + b1;


    // blk [4,5]
    b0 = x8 - x5;
    c0 = x0 - x5a;
    d0 = x3 - x4a;
    e0 = x7 - x4;

    RESTORE16VAL(b0);
    RESTORE16VAL(c0);
    RESTORE16VAL(d0);
    RESTORE16VAL(e0);

    b1 = (b0 << 48)>>51;
    c1 = (c0 << 48)>>51;
    d1 = (d0 << 48)>>51;
    e1 = (e0 << 48)>>51;
    tmpBuffer[1] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

    b1 = (b0 << 32)>>51;
    c1 = (c0 << 32)>>51;
    d1 = (d0 << 32)>>51;
    e1 = (e0 << 32)>>51;
    tmpBuffer[1+1*2] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

    b1 = (b0 << 16)>>51;
    c1 = (c0 << 16)>>51;
    d1 = (d0 << 16)>>51;
    e1 = (e0 << 16)>>51;
    tmpBuffer[1+2*2] = (e1<<48) + (d1<<32) + (c1<<16) + b1;
    
    b1 = b0>>51;
    c1 = c0>>51;
    d1 = d0>>51;
    e1 = e0>>51;
    tmpBuffer[1+3*2] = (e1<<48) + (d1<<32) + (c1<<16) + b1;
    

    blk0 = (I16_WMV *)piDst;
    blk1 = blk0 + iOffsetToNextRowForDCT;
    blk2 = blk1 + iOffsetToNextRowForDCT;
    blk3 = blk2 + iOffsetToNextRowForDCT;
    
    for (i = 0; i < 2; i++) 
    {
        x4 = tmpBuffer[i+0*2];
        x5 = tmpBuffer[i+1*2];
        x6 = tmpBuffer[i+2*2];
        x7 = tmpBuffer[i+3*2];
        
        x3 = (x4 - x6); 
        x6 += x4;
        
        x4 = 8 * x6 + g_Round64_32; //rounding
        x8 = 8 * x3 + g_Round64_32;   //rounding

        //simplify following to 3 multiplies
        x5a = 11 * x5 + 5 * x7;
        x5 = 5 * x5 - 11 * x7;
        
        /*
        x4 += (x6 >> 1); // guaranteed to have enough head room
        x8 += (x3 >> 1);
        */


        y5 = x6;
        RESTORE16VAL(y5);
        x6 -= y5;
        ls_signbit = y5 & 0x800080008000;
        y5 = (y5 & ~0x1000100010000) >> 1;
        y5 = y5 | ls_signbit;
        x6 += y5;
        
        y5 = x3;
        RESTORE16VAL(y5);
        x3 -= y5;
        ls_signbit = y5 & 0x800080008000;
        y5 = (y5 & ~0x1000100010000) >> 1;
        y5 = y5 | ls_signbit;
        x3 += y5;
        
        x4 += x6;
        x8 += x3;
        
        /*
        blk0[i] = (I16_WMV) ((x4 + x5a) >> 6);
        blk1[i] = (I16_WMV) ((x8 + x5) >> 6);
        blk2[i] = (I16_WMV) ((x8 - x5) >> 6);
        blk3[i] = (I16_WMV) ((x4 - x5a) >> 6);
        */

        j = i << 2;

        // blk0
        b0 = (x4 + x5a);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk0[j+0] = (I16_WMV)b1;
        b1 = (b0 << 32)>>54;
        blk0[j+1] = (I16_WMV)b1;
        b1 = (b0 << 16)>>54;
        blk0[j+2] = (I16_WMV)b1;
        b1 = b0>>54;
        blk0[j+3] = (I16_WMV)b1;

        
        // blk1
        b0 = (x8 + x5);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk1[j+0] = (I16_WMV)b1;
        b1 = (b0 << 32)>>54;
        blk1[j+1] = (I16_WMV)b1;
        b1 = (b0 << 16)>>54;
        blk1[j+2] = (I16_WMV)b1;
        b1 = b0>>54;
        blk1[j+3] = (I16_WMV)b1;
        
        // blk2
        b0 = (x8 - x5);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk2[j+0] = (I16_WMV)b1;
        b1 = (b0 << 32)>>54;
        blk2[j+1] = (I16_WMV)b1;
        b1 = (b0 << 16)>>54;
        blk2[j+2] = (I16_WMV)b1;
        b1 = b0>>54;
        blk2[j+3] = (I16_WMV)b1;
        
        // blk3
        b0 = (x4 - x5a);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk3[j+0] = (I16_WMV)b1;
        b1 = (b0 << 32)>>54;
        blk3[j+1] = (I16_WMV)b1;
        b1 = (b0 << 16)>>54;
        blk3[j+2] = (I16_WMV)b1;
        b1 = b0>>54;
        blk3[j+3] = (I16_WMV)b1;
    } 

    FUNCTION_PROFILE_STOP(&fpDecode);
}

Void_WMV g_4x8IDCTDec_WMV3 (UnionBuffer * piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer * rgiCoefReconBuf, I32_WMV iHalf)
{
    I64_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a;
    I64_WMV  y3, y4, y5, y4a, b0,c0,d0,e0,b1,c1,d1,e1,ls_signbit;
    I64_WMV i ;
    const I64_WMV  *piSrc0;
    I64_WMV tmpBuffer[8];
    I64_WMV *blk64;
    
    I32_WMV * piDst = piDstBuf->i32 + (iHalf*2);
    I16_WMV* blk16;

    blk64 = tmpBuffer;

    FUNCTION_PROFILE(fpDecode);
    FUNCTION_PROFILE_START(&fpDecode,IDCT_NT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_4x8IDCTDec_WMV3);
    
    // memcpy(dsttmp, rgiCoefReconBuf->i32, 64);
    // piSrc0 = (I32_WMV*) dsttmp;
    
    piSrc0 = (I64_WMV *)rgiCoefReconBuf->i32;
    
    for ( i = 0; i < 2; i++, blk64 += 4)
    {
        x4 = piSrc0[ i +0*2];
        x5 = piSrc0[ i +1*2];
        x6 = piSrc0[ i +2*2];
        x7 = piSrc0[ i +3*2];
        
        x0 = 17 * (x4 + x6) + g_Round64_4; //rounding
        x1 = 17 * (x4 - x6)  + g_Round64_4; //rounding
        x8 = 10 * (x5 + x7);
        x2 = x8 + 12 * x5;
        x3 = x8 - 32 * x7;
        
        /*
        blk[0] = (I16_WMV)((x0 + x2) >> 3);
        blk[1] = (I16_WMV)((x1 + x3) >> 3);
        blk[2] = (I16_WMV)((x1 - x3) >> 3);
        blk[3] = (I16_WMV)((x0 - x2) >> 3);
        */
        
        // blk [0,1]
        b0 = x0 + x2;
        c0 = x1 + x3;
        d0 = x1 - x3;
        e0 = x0 - x2;

        RESTORE16VAL(b0);
        RESTORE16VAL(c0);
        RESTORE16VAL(d0);
        RESTORE16VAL(e0);

        b1 = (b0 << 48)>>51;
        c1 = (c0 << 48)>>51;
        d1 = (d0 << 48)>>51;
        e1 = (e0 << 48)>>51;
        blk64[0] = (e1<<48) + (d1<<32) + (c1<<16) + b1;


        b1 = (b0 << 32)>>51;
        c1 = (c0 << 32)>>51;
        d1 = (d0 << 32)>>51;
        e1 = (e0 << 32)>>51;
        blk64[1] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

        b1 = (b0 << 16)>>51;
        c1 = (c0 << 16)>>51;
        d1 = (d0 << 16)>>51;
        e1 = (e0 << 16)>>51;
        blk64[2] = (e1<<48) + (d1<<32) + (c1<<16) + b1;
    
        b1 = b0>>51;
        c1 = c0>>51;
        d1 = d0>>51;
        e1 = e0>>51;
        blk64[3] = (e1<<48) + (d1<<32) + (c1<<16) + b1;
    }
    
    blk16 = (I16_WMV *)piDst;
    
    x0 = tmpBuffer[0] * 6 + g_Round64_32; /* rounding */
    x1 = tmpBuffer[4] * 6;
    x2 = tmpBuffer[6];
    x3 = tmpBuffer[2];
    x4 = tmpBuffer[1];
    x5 = tmpBuffer[7];
    x6 = tmpBuffer[5];
    x7 = tmpBuffer[3];
    
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
    
    x1 = 8 * (x2 + x3);
    x6 = x1 - 5 * x2;
    x1 -= 11 * x3;
    
    // third stage
    x7 = x8 + x6;
    x8 -= x6;
    
    
    x6 = x0 - x1;
    x0 += x1;
    
    // fractional part (guaranteed to have headroom)
    
    // Change 3:
    
    /*
    y5 = y4 >> 1;
    y3 = y4a >> 1;
    */
    
    y5 = y4;
    RESTORE16VAL(y5);
    y4 -= y5;
    ls_signbit = y5 & 0x800080008000;
    y5 = (y5 & ~0x1000100010000) >> 1;
    y5 = y5 | ls_signbit;
    y5 += y4;

    
    y3 = y4a;
    RESTORE16VAL(y3);
    y4a -= y3;
    ls_signbit= y3 & 0x800080008000;
    y3 = (y3 & ~0x1000100010000) >> 1;
    y3 = y3 | ls_signbit;
    y3 += y4a;
    
    // fourth stage
    
    x4 += y5;
    x5 += y5;
    x4a += y3;
    x5a += y3;
    
    /*
    blk0 [i] = (x7 + x4) >> 6;
    blk1 [i] = (x6 + x4a) >> 6;
    blk2 [i] = (x0 + x5a) >> 6;
    blk3 [i] = (x8 + x5) >> 6;  
    blk4 [i] = (x8 - x5) >> 6;
    blk5 [i] = (x0 - x5a) >> 6;
    blk6 [i] = (x6 - x4a) >> 6;
    blk7 [i] = (x7 - x4) >> 6;
    */
    
    // blk0
    b0 = (x7 + x4);
    RESTORE16VAL(b0);

    blk16 = (I16_WMV *)piDst;

    b1 = (b0 << 48)>>54;
    *(blk16 + 0 ) = (I16_WMV)b1;
    b1 = (b0 << 32)>>54;
    *(blk16 + 1 ) = (I16_WMV)b1;
    b1 = (b0 << 16)>>54;
    *(blk16 + 2 ) = (I16_WMV)b1;
    b1 = b0>>54;
    *(blk16 + 3 ) = (I16_WMV)b1;

    // blk1
    blk16 += iOffsetToNextRowForDCT;

    b0 = (x6 + x4a);
    RESTORE16VAL(b0);

    b1 = (b0 << 48)>>54;
    *(blk16 + 0 ) = (I16_WMV)b1;
    b1 = (b0 << 32)>>54;
    *(blk16 + 1 ) = (I16_WMV)b1;
    b1 = (b0 << 16)>>54;
    *(blk16 + 2 ) = (I16_WMV)b1;
    b1 = b0>>54;
    *(blk16 + 3 ) = (I16_WMV)b1;
    
    // blk2
    blk16 += iOffsetToNextRowForDCT;

    b0 = (x0 + x5a);
    RESTORE16VAL(b0);

    b1 = (b0 << 48)>>54;
    *(blk16 + 0 ) = (I16_WMV)b1;
    b1 = (b0 << 32)>>54;
    *(blk16 + 1 ) = (I16_WMV)b1;
    b1 = (b0 << 16)>>54;
    *(blk16 + 2 ) = (I16_WMV)b1;
    b1 = b0>>54;
    *(blk16 + 3 ) = (I16_WMV)b1;
    
    // blk3
    blk16 += iOffsetToNextRowForDCT;

    b0 = (x8 + x5);
    RESTORE16VAL(b0);

    b1 = (b0 << 48)>>54;
    *(blk16 + 0 ) = (I16_WMV)b1;
    b1 = (b0 << 32)>>54;
    *(blk16 + 1 ) = (I16_WMV)b1;
    b1 = (b0 << 16)>>54;
    *(blk16 + 2 ) = (I16_WMV)b1;
    b1 = b0>>54;
    *(blk16 + 3 ) = (I16_WMV)b1;
    
    // blk4
    blk16 += iOffsetToNextRowForDCT;

    b0 = (x8 - x5);
    RESTORE16VAL(b0);

    b1 = (b0 << 48)>>54;
    *(blk16 + 0 ) = (I16_WMV)b1;
    b1 = (b0 << 32)>>54;
    *(blk16 + 1 ) = (I16_WMV)b1;
    b1 = (b0 << 16)>>54;
    *(blk16 + 2 ) = (I16_WMV)b1;
    b1 = b0>>54;
    *(blk16 + 3 ) = (I16_WMV)b1;
    
    // blk5
    blk16 += iOffsetToNextRowForDCT;

    b0 = (x0 - x5a);
    RESTORE16VAL(b0);

    b1 = (b0 << 48)>>54;
    *(blk16 + 0 ) = (I16_WMV)b1;
    b1 = (b0 << 32)>>54;
    *(blk16 + 1 ) = (I16_WMV)b1;
    b1 = (b0 << 16)>>54;
    *(blk16 + 2 ) = (I16_WMV)b1;
    b1 = b0>>54;
    *(blk16 + 3 ) = (I16_WMV)b1;
    
    // blk6
    blk16 += iOffsetToNextRowForDCT;

    b0 = (x6 - x4a);
    RESTORE16VAL(b0);

    b1 = (b0 << 48)>>54;
    *(blk16 + 0 ) = (I16_WMV)b1;
    b1 = (b0 << 32)>>54;
    *(blk16 + 1 ) = (I16_WMV)b1;
    b1 = (b0 << 16)>>54;
    *(blk16 + 2 ) = (I16_WMV)b1;
    b1 = b0>>54;
    *(blk16 + 3 ) = (I16_WMV)b1;
    
    // blk7
    blk16 += iOffsetToNextRowForDCT;

    b0 = (x7 - x4);
    RESTORE16VAL(b0);

    b1 = (b0 << 48)>>54;
    *(blk16 + 0 ) = (I16_WMV)b1;
    b1 = (b0 << 32)>>54;
    *(blk16 + 1 ) = (I16_WMV)b1;
    b1 = (b0 << 16)>>54;
    *(blk16 + 2 ) = (I16_WMV)b1;
    b1 = b0>>54;
    *(blk16 + 3 ) = (I16_WMV)b1;
    
    FUNCTION_PROFILE_STOP(&fpDecode);
}

Void_WMV g_4x4IDCTDec_WMV3 (UnionBuffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iQuadrant)
{
    I16_WMV* piDst = piDstBuf->i16 + (iQuadrant&2)*16 + (iQuadrant&1)*4;

    I64_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x5a, y5;
    I64_WMV  b0,c0,d0,e0, b1,c1, d1, e1,ls_signbit;    
    I64_WMV  rgTemp[4];
    I64_WMV  *piSrc0; 
    
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_4x4IDCTDec_WMV3);
    
    piSrc0 = (I64_WMV *)rgiCoefReconBuf->i32;
    
    x4 = piSrc0[0];      
    x5 = piSrc0[1];      
    x6 = piSrc0[2];
    x7 = piSrc0[3];
    
    
    x0 = 17 * (x4 + x6) + g_Round64_4; //rounding
    x1 = 17 * (x4 - x6) + g_Round64_4; //rounding
    x8 = 10 * (x5 + x7);
    x2 = x8 + 12 * x5;
    x3 = x8 - 32 * x7;
    
    // blk [0,1]
    b0 = x0 + x2;
    c0 = x1 + x3;
    d0 = x1 - x3;
    e0 = x0 - x2;
    
    RESTORE16VAL(b0);
    RESTORE16VAL(c0);
    RESTORE16VAL(d0);
    RESTORE16VAL(e0);

    b1 = (b0 << 48)>>51;
    c1 = (c0 << 48)>>51;
    d1 = (d0 << 48)>>51;
    e1 = (e0 << 48)>>51;
    rgTemp[0] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

    b1 = (b0 << 32)>>51;
    c1 = (c0 << 32)>>51;
    d1 = (d0 << 32)>>51;
    e1 = (e0 << 32)>>51;
    rgTemp[1] = (e1<<48) + (d1<<32) + (c1<<16) + b1;
 
    b1 = (b0 << 16)>>51;
    c1 = (c0 << 16)>>51;
    d1 = (d0 << 16)>>51;
    e1 = (e0 << 16)>>51;
    rgTemp[2] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

    b1 = b0>>51;
    c1 = c0>>51;
    d1 = d0>>51;
    e1 = e0>>51;
    rgTemp[3] = (e1<<48) + (d1<<32) + (c1<<16) + b1;
    
    
    x4 = rgTemp[0];
    x5 = rgTemp[1];
    x6 = rgTemp[2];
    x7 = rgTemp[3];
    
    
    x3 = (x4 - x6); 
    x6 += x4;
    
    x4 = 8 * x6 + g_Round64_32; //rounding
    x8 = 8 * x3 + g_Round64_32;   //rounding
    //simplify following to 3 multiplies
    x5a = 11 * x5 + 5 * x7;
    x5 = 5 * x5 - 11 * x7;
    
    /*
    x6 = (x6 >> 1);
    x3 = (x3 >> 1)
    */
    
    y5 = x6;
    RESTORE16VAL(y5);
    x6 -= y5;
    ls_signbit = y5 & 0x800080008000;
    y5 = (y5 & ~0x1000100010000) >> 1;
    y5 = y5 | ls_signbit;
    x6 += y5;
    
    y5 = x3;
    RESTORE16VAL(y5);
    x3 -= y5;
    ls_signbit = y5 & 0x800080008000;
    y5 = (y5 & ~0x1000100010000) >> 1;
    y5 = y5 | ls_signbit;
    x3 += y5;
    
    x4 += x6; // guaranteed to have enough head room
    x8 += x3 ;
    
    /*
    blk0[i] = (I16_WMV) ((x4 + x5a) >> 6);
    blk1[i] = (I16_WMV) ((x8 + x5) >> 6);
    blk2[i] = (I16_WMV) ((x8 - x5) >> 6);
    blk3[i] = (I16_WMV) ((x4 - x5a) >> 6);
    */
    
    // blk0
    b0 = (x4 + x5a);
    RESTORE16VAL(b0);

    b1 = (b0 << 48)>>54;
    piDst[ 0 + 0*8] = (I16_WMV)b1;
    b1 = (b0 << 32)>>54;
    piDst[ 1 + 0*8] = (I16_WMV)b1;
    b1 = (b0 << 16)>>54;
    piDst[ 2 + 0*8] = (I16_WMV)b1;
    b1 = b0>>54;
    piDst[ 3 + 0*8] = (I16_WMV)b1;
    
    // blk1
    b0 = (x8 + x5);
    RESTORE16VAL(b0);

    b1 = (b0 << 48)>>54;
    piDst[ 0 + 1*8] = (I16_WMV)b1;
    b1 = (b0 << 32)>>54;
    piDst[ 1 + 1*8] = (I16_WMV)b1;
    b1 = (b0 << 16)>>54;
    piDst[ 2 + 1*8] = (I16_WMV)b1;
    b1 = b0>>54;
    piDst[ 3 + 1*8] = (I16_WMV)b1;
    
    // blk2
    b0 = (x8 - x5);
    RESTORE16VAL(b0);

    b1 = (b0 << 48)>>54;
    piDst[ 0 + 2*8] = (I16_WMV)b1;
    b1 = (b0 << 32)>>54;
    piDst[ 1 + 2*8] = (I16_WMV)b1;
    b1 = (b0 << 16)>>54;
    piDst[ 2 + 2*8] = (I16_WMV)b1;
    b1 = b0>>54;
    piDst[ 3 + 2*8] = (I16_WMV)b1;
    
    // blk3
    b0 = (x4 - x5a);
    RESTORE16VAL(b0);

    b1 = (b0 << 48)>>54;
    piDst[ 0 + 3*8] = (I16_WMV)b1;
    b1 = (b0 << 32)>>54;
    piDst[ 1 + 3*8] = (I16_WMV)b1;
    b1 = (b0 << 16)>>54;
    piDst[ 2 + 3*8] = (I16_WMV)b1;
    b1 = b0>>54;
    piDst[ 3 + 3*8] = (I16_WMV)b1;
}


void SignPatch(I16_WMV * rgiCoefRecon, int len)
{
    int i;
    I16_WMV *piSrcTmp = rgiCoefRecon;
    I16_WMV * dsttmp = rgiCoefRecon;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(SignPatch);
    
    for(i=0;i<(len>>1);i+=4)
    {
        I64_WMV data1 = (I64_WMV)piSrcTmp[i];
        I64_WMV data2 = (I64_WMV)piSrcTmp[i+1];
        I64_WMV data3 = (I64_WMV)piSrcTmp[i+2];
        I64_WMV data4 = (I64_WMV)piSrcTmp[i+3];
        
        data1 += (data2<<16) + (data3<<32) + (data4<<48);
        dsttmp[i] = (I16_WMV)(data1 & 0xffff);
        dsttmp[i+1] = (I16_WMV)(data1>>16);// & 0xffff);
        dsttmp[i+2] = (I16_WMV)(data1>>32);// & 0xffff);
        dsttmp[i+3] = (I16_WMV)(data1>>48);// & 0xffff);
    }
}



/* 
optimized C code against Ref code.
requirement: len mod 4 = 0
*/

void SignPatch32(I32_WMV * rgiCoefRecon, int len)
{
    int i;
    I64_WMV v1, v2, v3, v4;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(SignPatch32);
    
    for(i=0; i < (len >> 3); i++)
    {
        v1 = rgiCoefRecon[i*4];
        v2 = rgiCoefRecon[i*4+1];
        v3 = rgiCoefRecon[i*4+2];
        v4 = rgiCoefRecon[i*4+3];

        v1 += (v2<<16) + (v3<<32) + (v4<<48);
        rgiCoefRecon[2*i] = (I32_WMV)v1;// & 0xffffffff);
        rgiCoefRecon[2*i+1] = (I32_WMV)(v1 >> 32);// & 0xffffffff);
    }
}


///////////////////////////////////////////////////////////////////////////
// Inter IDCT Functions
// (this fn and one below are almost repeats of each other!!! - INEFFICIENT!)
///////////////////////////////////////////////////////////////////////////
// C Version
// Input:  16-bit
// Output: 16-bit

/*
 * iDCTHorzFlags need more inspection
 * piSrc0 must be 8 bytes aligned.
 * blk32 must be 8 bytes aligned.
 * iNumLoops must be 4.
 */

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec_WMV3_Pass1(const I64_WMV  *piSrc, I64_WMV * blk64, const I32_WMV iNumLoops, I32_WMV iDCTHorzFlags)
{
    I64_WMV i;
    register I64_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a;
    register I64_WMV  y3,  b0, c0, d0, e0, b1, c1, d1, e1;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV3_Pass1);
    
    for ( i = 0; i < iNumLoops>>1; i++,  blk64 += 8, iDCTHorzFlags >>= 4 ){
        // Check to see if this row has all zero coefficients in columns 1-7 (0 is first column)
        //        if (!(iDCTHorzFlags & (1 << i))) {
        //            // Columns 1-7 must be zero. Check to see if column zero coefficient is zero. 
        //            // If so, we don't have to do anything more for this row
        //            // Column zero cofficient is nonzero so all coefficients in this row are DC
        //            blk [0] = blk [1] = blk [2] = blk [3] = blk [4] = blk [5] = blk [6] = blk [7] = rgiCoefRecon [0] << 3;
        //            // If this is not the first row, tell the second stage that it can't use DC
        //            // shortcut - it has to do the full transform for all the columns
        //            iDCTVertFlag = i;
        //          continue;
        //      }
        
        if(!(iDCTHorzFlags&15))
        {
            register I64_WMV b1;
            
            assert(piSrc[ i +1*2 ]== 0);
            assert(piSrc[ i +2*2 ]== 0);
            assert(piSrc[ i +3*2 ]== 0);
            assert(piSrc[ i +4*2 ]== 0);
            assert(piSrc[ i +5*2 ]== 0);
            assert(piSrc[ i +6*2 ]== 0);
            assert(piSrc[ i +7*2 ]== 0);
            
            b0 = piSrc[ i ]*W0 + g_Round64_4; //12
            RESTORE16VAL(b0);

            b1 = (b0 << 48)>>51;
            b1 = b1 + (b1 << 16) + (b1 << 32 ) + (b1<<48);
            blk64[0] = b1;
            blk64[1] = b1;

            b1 = (b0 << 32)>>51;
            b1 = b1 + (b1 << 16) + (b1 << 32 ) + (b1<<48);
            blk64[2] = b1;
            blk64[3] = b1;

            b1 = (b0 << 16)>>51;
            b1 = b1 + (b1 << 16) + (b1 << 32 ) + (b1<<48);
            blk64[4] = b1;
            blk64[5] = b1;

            b1 = b0>>51;
            b1 = b1 + (b1 << 16) + (b1 << 32 ) + (b1<<48);
            blk64[6] = b1;
            blk64[7] = b1;

            continue;
        }
        
        // If this is not the first row, tell the second stage that it can't use DC shortcut 
        // - it has to do the full transform for all the columns
        
        x4 = piSrc[ i +1*2 ];      
        x3 = piSrc[ i +2*2 ];
        x7 = piSrc[ i +3*2 ];
        x1 = piSrc[ i +4*2 ];
        x6 = piSrc[ i +5*2 ];
        x2 = piSrc[ i +6*2 ];      
        x5 = piSrc[ i +7*2 ];
        x0 = piSrc[ i +0*2 ]; /* for proper rounding */
        
        
        x1 = x1 * W0;   //12
        x0 = x0 * W0 + g_Round64_4; /* for proper rounding */
        
        // zeroth stage
        y3 = x4 + x5;
        x8 = W3 * y3;           //15
        x4a = x8 - W3pW5 * x5;  //24
        x5a = x8 - W3_W5 * x4;  //6
        x8 = W7 * y3;           //4
        x4 = x8 + W1_W7 * x4;   //12
        x5 = x8 - W1pW7 * x5;   //20
        
        // first stage
        y3 = x6 + x7;
        x8 = W7 * y3;           //4
        x4a -= x8 + W1_W7 * x6; //12
        x5a += x8 - W1pW7 * x7; //20
        x8 = W3 * y3;           //15
        x4 += x8 - W3_W5 * x6;  //6
        x5 += x8 - W3pW5 * x7;  //24
        
        // second stage 
        x8 = x0 + x1;
        x0 -= x1;
        
        x1 = x2;
        x2 = W6 * x3 - W2 * x2;  //6,  16
        x3 = W6 * x1 + W2A * x3; //6,  16
        
        // third stage
        x7 = x8 + x3;
        x8 -= x3;
        x3 = x0 + x2;
        x0 -= x2;
        
        // fourth stage
        /*
        blk [0] = (I16_WMV) ((x7 + x4) >> 3);
        blk [1] = (I16_WMV) ((x3 + x4a) >> 3);
        blk [2] = (I16_WMV) ((x0 + x5a) >> 3);
        blk [3] = (I16_WMV) ((x8 + x5) >> 3);
        blk [4] = (I16_WMV) ((x8 - x5) >> 3);
        blk [5] = (I16_WMV) ((x0 - x5a) >> 3);
        blk [6] = (I16_WMV) ((x3 - x4a) >> 3);
        blk [7] = (I16_WMV) ((x7 - x4) >> 3);
        */
        
        // blk [0,1]
        b0 = x7 + x4;
        c0 = x3 + x4a;
        d0 = x0 + x5a;
        e0 = x8 + x5;

        RESTORE16VAL(b0);
        RESTORE16VAL(c0);
        RESTORE16VAL(d0);
        RESTORE16VAL(e0);
        
        b1 = (b0 << 48)>>51;
        c1 = (c0 << 48)>>51;
        d1 = (d0 << 48)>>51;
        e1 = (e0 << 48)>>51;
        blk64[0] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

        b1 = (b0 << 32)>>51;
        c1 = (c0 << 32)>>51;
        d1 = (d0 << 32)>>51;
        e1 = (e0 << 32)>>51;
        blk64[2] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

        b1 = (b0 << 16)>>51;
        c1 = (c0 << 16)>>51;
        d1 = (d0 << 16)>>51;
        e1 = (e0 << 16)>>51;
        blk64[4] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

        b1 = b0>>51;
        c1 = c0>>51;
        d1 = d0>>51;
        e1 = e0>>51;
        blk64[6] = (e1<<48) + (d1<<32) + (c1<<16) + b1;


        // blk [4,5]
        b0 = x8 - x5;
        c0 = x0 - x5a;
        d0 = x3 - x4a;
        e0 = x7 - x4;

        RESTORE16VAL(b0);
        RESTORE16VAL(c0);
        RESTORE16VAL(d0);
        RESTORE16VAL(e0);
        

        b1 = (b0 << 48)>>51;
        c1 = (c0 << 48)>>51;
        d1 = (d0 << 48)>>51;
        e1 = (e0 << 48)>>51;
        blk64[1] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

        b1 = (b0 << 32)>>51;
        c1 = (c0 << 32)>>51;
        d1 = (d0 << 32)>>51;
        e1 = (e0 << 32)>>51;
        blk64[3] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

        b1 = (b0 << 16)>>51;
        c1 = (c0 << 16)>>51;
        d1 = (d0 << 16)>>51;
        e1 = (e0 << 16)>>51;
        blk64[5] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

        b1 = b0>>51;
        c1 = c0>>51;
        d1 = d0>>51;
        e1 = e0>>51;
        blk64[7] = (e1<<48) + (d1<<32) + (c1<<16) + b1;
    }
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec_WMV3_Pass2(const I64_WMV  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops)
{
    I64_WMV i, j;
    register I64_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a;
    I64_WMV  y4, y4a, b0,b1,ls_signbit, y5;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV3_Pass2);
    
    for (i = 0; i < iNumLoops; i+=2)
    {
        // If iDCTVertFlag is zero then we know that blk[1] thru blk[7] are zero.
        // Therefore, if blk[0] is zero we can skip to the next column. Otherwise we
        // can use the DC shortcut and just copy blk[0] to the rest of the column.
        //        if (iDCTVertFlag == 0){
        //            // If blk[0] is nonzero, copy the value to the other 7 rows in this column        
        //            if (blk0[i] != 0)
        //              blk0[i] = blk1[i] = blk2[i] = blk3[i] = blk4[i] = blk5[i] = blk6[i] = blk7[i] = (blk0[i] + 32) >> 6;
        //          continue;
        //      }

        j = i>>1;

        x0 = piSrc0[j + 0*2 ] * 6 + g_Round64_32 /* rounding */;
        x1 = piSrc0[j + 4*2 ] * 6;
        x2 = piSrc0[j + 6*2 ];
        x3 = piSrc0[j + 2*2 ];
        x4 = piSrc0[j + 1*2 ];
        x5 = piSrc0[j + 7*2 ];
        x6 = piSrc0[j + 5*2 ];
        x7 = piSrc0[j + 3*2 ];
        
        // zeroth stage
        y4a = x4 + x5;
        x8 = 7 * y4a;
        x4a = x8 - 12 * x5;
        x5a = x8 - 3 * x4;
        x8 = 2 * y4a;
        x4 = x8 + 6 * x4;
        x5 = x8 - 10 * x5;
        
        y5 = y4a;
        RESTORE16VAL(y5);
        y4a -= y5;
        ls_signbit = y5 & 0x800080008000;
        y5 = (y5 & ~0x1000100010000) >> 1;
        y5 = y5 | ls_signbit;
        y4a += y5;


        x4a += y4a;
        x5a += y4a;
        
        // first stage
        y4 = x6 + x7;
        x8 = 2 * y4;
        
        x4a -= x8 + 6 * x6;
        x5a += x8 - 10 * x7;
        x8 = 7 * y4;
        
        y5 = y4;
        RESTORE16VAL(y5);
        y4 -= y5;
        ls_signbit = y5 & 0x800080008000;
        y5 = (y5 & ~0x1000100010000) >> 1;
        y5 = y5 | ls_signbit;
        y4 += y5;

        //        x4 += y4;
        //        x5 += y4;
        x8 += y4;
        x4 += x8 - 3 * x6;
        x5 += x8 - 12 * x7;
        
        // second stage 
        x8 = x0 + x1;
        x0 -= x1;
        
        x1 = 8 * (x2 + x3);
        x6 = x1 - 5 * x2;
        x1 -= 11 * x3;
        
        // third stage
        x7 = x8 + x6;
        x8 -= x6;
        
        x6 = x0 - x1;
        x0 += x1;
        
        /*
        
          _blk0 [i] = (x7 + x4) >> 6;
          _blk1 [i] = (x6 + x4a) >> 6;
          _blk2 [i] = (x0 + x5a) >> 6;
          _blk3 [i] = (x8 + x5) >> 6;
          _blk4 [i] = (x8 - x5) >> 6;
          _blk5 [i] = (x0 - x5a) >> 6;
          _blk6 [i] = (x6 - x4a) >> 6;
          _blk7 [i] = (x7 - x4) >> 6;
          
        */
        
        // blk0
        b0 = (x7 + x4);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk16[ i + 0   + 0*2] = (I16_WMV)b1;
        b1 = (b0 << 32)>>54;
        blk16[ i + 32 + 0*4] = (I16_WMV)b1;
        b1 = (b0 << 16)>>54;
        blk16[ i + 1  + 0*4] = (I16_WMV)b1;
        b1 = b0>>54;
        blk16[i + 32 + 1 + 0*4] = (I16_WMV)b1;

        // blk1
        b0 = (x6 + x4a);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk16[ i + 0   + 1*4] = (I16_WMV)b1;
        b1 = (b0 << 32)>>54;
        blk16[ i + 32   + 1*4] = (I16_WMV)b1;
        b1 = (b0 << 16)>>54;
        blk16[ i + 1   + 1*4] = (I16_WMV)b1;
        b1 = b0>>54;
        blk16[i + 32 + 1 + 1*4] = (I16_WMV)b1;

        // blk2
        b0 = (x0 + x5a);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk16[ i + 0   + 2*4] = (I16_WMV)b1;
        b1 = (b0 << 32)>>54;
        blk16[ i + 32   + 2*4] = (I16_WMV)b1;
        b1 = (b0 << 16)>>54;
        blk16[ i + 1   + 2*4] = (I16_WMV)b1;
        b1 = b0>>54;
        blk16[i + 32 + 1 + 2*4] = (I16_WMV)b1;
        
        // blk3
        b0 = (x8 + x5);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk16[ i + 0   + 3*4] = (I16_WMV)b1;
        b1 = (b0 << 32)>>54;
        blk16[ i + 32   + 3*4] = (I16_WMV)b1;
        b1 = (b0 << 16)>>54;
        blk16[ i + 1   + 3*4] = (I16_WMV)b1;
        b1 = b0>>54;
        blk16[i + 32 + 1 + 3*4] = (I16_WMV)b1;
        
        // blk4
        b0 = (x8 - x5);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk16[ i + 0   + 4*4] = (I16_WMV)b1;
        b1 = (b0 << 32)>>54;
        blk16[ i + 32   + 4*4] = (I16_WMV)b1;
        b1 = (b0 << 16)>>54;
        blk16[ i + 1   + 4*4] = (I16_WMV)b1;
        b1 = b0>>54;
        blk16[i + 32 + 1 + 4*4] = (I16_WMV)b1;
        
        // blk5
        b0 = (x0 - x5a);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk16[ i + 0   + 5*4] = (I16_WMV)b1;
        b1 = (b0 << 32)>>54;
        blk16[ i + 32   + 5*4] = (I16_WMV)b1;
        b1 = (b0 << 16)>>54;
        blk16[ i + 1   + 5*4] = (I16_WMV)b1;
        b1 = b0>>54;
        blk16[i + 32 + 1 + 5*4] = (I16_WMV)b1;
        
        // blk6
        b0 = (x6 - x4a);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk16[ i + 0   + 6*4] = (I16_WMV)b1;
        b1 = (b0 << 32)>>54;
        blk16[ i + 32   + 6*4] = (I16_WMV)b1;
        b1 = (b0 << 16)>>54;
        blk16[ i + 1   + 6*4] = (I16_WMV)b1;
        b1 = b0>>54;
        blk16[i + 32 + 1 + 6*4] = (I16_WMV)b1;
        
        // blk7
        b0 = (x7 - x4);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk16[ i + 0   + 7*4] = (I16_WMV)b1;
        b1 = (b0 << 32)>>54;
        blk16[ i + 32   + 7*4] = (I16_WMV)b1;
        b1 = (b0 << 16)>>54;
        blk16[ i + 1   + 7*4] = (I16_WMV)b1;
        b1 = b0>>54;
        blk16[i + 32 + 1 + 7*4] = (I16_WMV)b1;
    }
}

/*
 * piSrc0, blk32 must be 8 bytes aligned.
 */

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec_WMV3_Pass3(const I64_WMV  *piSrc, I64_WMV * blk64, const I32_WMV iNumLoops,  I32_WMV iDCTHorzFlags)
{
    I64_WMV i;
    register I64_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8;
    register I64_WMV  b0, c0, d0, e0, b1, c1, d1, e1;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV3_Pass3);
    
    for ( i = 0; i < iNumLoops>>1; i++, blk64 += 8, iDCTHorzFlags >>= 4)
    {
        if(!(iDCTHorzFlags&15))
        {
            assert(piSrc[ i +1*2 ]== 0);
            assert(piSrc[ i +2*2 ]== 0);
            assert(piSrc[ i +3*2 ]== 0);
            
            
            b0 = piSrc[ i ]*17 + g_Round64_4;
            RESTORE16VAL(b0);

            b1 = (b0 << 48)>>51;
            b1 = b1 + (b1 << 16) + (b1 << 32 ) + (b1<<48);
            blk64[0] = b1;

            b1 = (b0 << 32)>>51;
            b1 = b1 + (b1 << 16) + (b1 << 32 ) + (b1<<48);
            blk64[2] = b1;

            b1 = (b0 << 16)>>51;
            b1 = b1 + (b1 << 16) + (b1 << 32 ) + (b1<<48);
            blk64[4] = b1;

            b1 = b0>>51;
            b1 = b1 + (b1 << 16) + (b1 << 32 ) + (b1<<48);
            blk64[6] = b1;
            
            continue;
            
        }
        
        x4 = piSrc[ i +0*2 ];      
        x5 = piSrc[ i +1*2 ];      
        x6 = piSrc[ i +2*2 ];
        x7 = piSrc[ i +3*2 ];
        
        
        x0 = 17 * (x4 + x6) + g_Round64_4;
        x1 = 17 * (x4 - x6) + g_Round64_4;
        x8 = 10 * (x5 + x7);
        x2 = x8 + 12 * x5;
        x3 = x8 - 32 * x7;
        
        /*
        blk[0] = (I16_WMV)((x0 + x2) >> 3);
        blk[1] = (I16_WMV)((x1 + x3) >> 3);
        blk[2] = (I16_WMV)((x1 - x3) >> 3);
        blk[3] = (I16_WMV)((x0 - x2) >> 3);
        */
        
        // blk [0,1]
        b0 = x0 + x2;
        c0 = x1 + x3;
        d0 = x1 - x3;
        e0 = x0 - x2;

        RESTORE16VAL(b0);
        RESTORE16VAL(c0);
        RESTORE16VAL(d0);
        RESTORE16VAL(e0);
        
        b1 = (b0 << 48)>>51;
        c1 = (c0 << 48)>>51;
        d1 = (d0 << 48)>>51;
        e1 = (e0 << 48)>>51;
        blk64[0] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

        b1 = (b0 << 32)>>51;
        c1 = (c0 << 32)>>51;
        d1 = (d0 << 32)>>51;
        e1 = (e0 << 32)>>51;
        blk64[2] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

        b1 = (b0 << 16)>>51;
        c1 = (c0 << 16)>>51;
        d1 = (d0 << 16)>>51;
        e1 = (e0 << 16)>>51;
        blk64[4] = (e1<<48) + (d1<<32) + (c1<<16) + b1;

        b1 = b0>>51;
        c1 = c0>>51;
        d1 = d0>>51;
        e1 = e0>>51;
        blk64[6] = (e1<<48) + (d1<<32) + (c1<<16) + b1;
    }
    
}

/*
 * piSrc must be 8 bytes aligned.
 */

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec_WMV3_Pass4(const I64_WMV  *piSrc, I16_WMV * blk16, const I32_WMV iNumLoops)
{
    I64_WMV i, j;
    register I64_WMV  x1, x3, x4, x5, y5, x6, x7, x8,  x5a;
    register I64_WMV  b0,b1,ls_signbit;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV3_Pass4);
    
    for (i = 0; i < iNumLoops; i+=2) 
    {
        j = i>>1;
        
        x4 = piSrc[j + 0*2 ];
        x5 = piSrc[j + 1*2 ];
        x6 = piSrc[j + 2*2 ];
        x7 = piSrc[j + 3*2 ];
        
        
        x3 = (x4 - x6); 
        x6 += x4;
        
        x4 = 8 * x6 + g_Round64_32; //rounding
        x8 = 8 * x3 + g_Round64_32; //rounding
        
        
        y5 = x6;
        RESTORE16VAL(y5);
        x6 -= y5;
        ls_signbit = y5 & 0x800080008000;
        y5 = (y5 & ~0x1000100010000) >> 1;
        y5 = y5 | ls_signbit;
        x6 += y5;
        
        y5 = x3;
        RESTORE16VAL(y5);
        x3 -= y5;
        ls_signbit = y5 & 0x800080008000;
        y5 = (y5 & ~0x1000100010000) >> 1;
        y5 = y5 | ls_signbit;
        x3 += y5;
        
        x4 += x6; // guaranteed to have enough head room
        x8 += x3 ;
        
        
        x1 = 5 * ( x5 + x7);
        x5a = x1 + 6 * x5;
        x5 =  x1 - 16 * x7;
        
        
        // blk0
        b0 = (x4 + x5a);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk16[i + 0   + 0*4] = (I16_WMV)b1;
        b1 = (b0 << 16)>>54;
        blk16[i + 1   + 0*4] = (I16_WMV)b1;
        b1 = (b0 << 32)>>54;
        blk16[i + 32   + 0*4] = (I16_WMV)b1;
        b1 = b0>>54;
        blk16[i + 32 +1 + 0*4] = (I16_WMV)b1;

        // blk1
        b0 = (x8 + x5);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk16[i + 0   + 1*4] = (I16_WMV)b1;
        b1 = (b0 << 16)>>54;
        blk16[i + 1   + 1*4] = (I16_WMV)b1;
        b1 = (b0 << 32)>>54;
        blk16[i + 32   + 1*4] = (I16_WMV)b1;
        b1 = b0>>54;
        blk16[i + 32 +1 + 1*4] = (I16_WMV)b1;
       
        
        // blk2
        b0 = (x8 - x5);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk16[i + 0   + 2*4] = (I16_WMV)b1;
        b1 = (b0 << 16)>>54;
        blk16[i + 1   + 2*4] = (I16_WMV)b1;
        b1 = (b0 << 32)>>54;
        blk16[i + 32   + 2*4] = (I16_WMV)b1;
        b1 = b0>>54;
        blk16[i + 32 +1 + 2*4] = (I16_WMV)b1;
        
        
        // blk3
        b0 = (x4 - x5a);
        RESTORE16VAL(b0);

        b1 = (b0 << 48)>>54;
        blk16[i + 0   + 3*4] = (I16_WMV)b1;
        b1 = (b0 << 16)>>54;
        blk16[i + 1   + 3*4] = (I16_WMV)b1;
        b1 = (b0 << 32)>>54;
        blk16[i + 32   + 3*4] = (I16_WMV)b1;
        b1 = b0>>54;
        blk16[i + 32 +1 + 3*4] = (I16_WMV)b1;
    } 
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec16_WMV3_SSIMD (UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags)
{
    I64_WMV  *piSrc0;
    I16_WMV * blk16;
    I64_WMV * blk64;    
    I64_WMV tmpBuffer[32];

    FUNCTION_PROFILE(fpDecode);
    FUNCTION_PROFILE_START(&fpDecode,IDCT_NT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec16_WMV3_SSIMD);
    
    piSrc0 = (I64_WMV  *)piSrc->i32;
    blk64 = tmpBuffer;
    
    g_IDCTDec_WMV3_Pass1(piSrc0, blk64, PASS_8, iDCTHorzFlags);
    
    piSrc0 = tmpBuffer;
    blk16  = piDst->i16;
    
    g_IDCTDec_WMV3_Pass2(piSrc0, blk16, PASS_8);
    
    FUNCTION_PROFILE_STOP(&fpDecode);
}


//#define 4 4
// C Version
// Input:  32-bit (16 bits enough!)
// Output: 16-bit
//#pragma code_seg (EMBSEC_PML)
Void_WMV g_8x4IDCTDec_WMV3_SSIMD (UnionBuffer * piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer * rgiCoefReconBuf, I32_WMV iHalf, I32_WMV iDCTHorzFlags)
{
    I64_WMV * piDst = (I64_WMV *)(piDstBuf->i32 + (iHalf << 3));
    I64_WMV *blk64 ;
    I16_WMV  * blk16;
    I64_WMV tmpBuffer[16];
    const I64_WMV  * piSrc0 = (I64_WMV *)rgiCoefReconBuf->i32;
    FUNCTION_PROFILE(fpDecode);
    FUNCTION_PROFILE_START(&fpDecode,IDCT_NT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_8x4IDCTDec_WMV3_SSIMD);
    
    blk64 = tmpBuffer;
    g_IDCTDec_WMV3_Pass1(piSrc0, blk64, PASS_4, iDCTHorzFlags);
    
    piSrc0 = tmpBuffer;
    blk16  = (I16_WMV *)piDst;
    
    g_IDCTDec_WMV3_Pass4(piSrc0,  blk16, PASS_8);
    
    FUNCTION_PROFILE_STOP(&fpDecode);
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_4x8IDCTDec_WMV3_EMB (UnionBuffer * piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer * rgiCoefReconBuf, I32_WMV iHalf, I32_WMV iDCTHorzFlags)
{
    I64_WMV * piDst = (I64_WMV *)(piDstBuf->i32 + (iHalf));
    I64_WMV * blk64;
    I64_WMV tmpBuffer[4*2*4];
    const I64_WMV  * piSrc0 = (I64_WMV *)rgiCoefReconBuf->i32;   
    I16_WMV * blk16;

    FUNCTION_PROFILE(fpDecode)
    FUNCTION_PROFILE_START(&fpDecode,IDCT_NT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_4x8IDCTDec_WMV3_EMB);
    
    blk64 = tmpBuffer;
    
    g_IDCTDec_WMV3_Pass3(piSrc0,  blk64, PASS_8, iDCTHorzFlags);
    
    piSrc0 = tmpBuffer;
    blk16  = (I16_WMV *)piDst;
    
    g_IDCTDec_WMV3_Pass2(piSrc0, blk16, PASS_4);
    
    FUNCTION_PROFILE_STOP(&fpDecode);
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_SubBlkIDCTClear_EMB (UnionBuffer * piDstBuf,  I32_WMV iIdx)
{
    I32_WMV iNumLoop = 8<<(iIdx>>7);
    I32_WMV iStep   =  1<< ((iIdx >>6)&0x1);
    I32_WMV iStep2 = iStep<<1;
    I32_WMV* piDst = piDstBuf->i32 + (iIdx&0x3f);
    I32_WMV i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_SubBlkIDCTClear_EMB);
    //(iNumLoop, iStep) = (8,1),(16,2), (8,2)
    for (i = 0; i < iNumLoop; i+=iStep2) 
    {
        piDst[i] = 0;
        piDst[i+16] = 0;
        piDst[i + iStep] = 0;
        piDst[i+16 + iStep] = 0;
    }
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_4x4IDCTDec_WMV3_EMB (UnionBuffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iQuadrant, I32_WMV iDCTHorzFlags)
{
    I16_WMV* piDst = piDstBuf->i16 + (iQuadrant&2)*8 + (iQuadrant&1)*2;
    I64_WMV  *piSrc0; 
    I64_WMV  rgTemp[16];
    I16_WMV * blk16 ;
    I64_WMV * blk64 = rgTemp;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_4x4IDCTDec_WMV3_EMB);
    
    piSrc0 = (I64_WMV *)rgiCoefReconBuf->i32;
    
    g_IDCTDec_WMV3_Pass3(piSrc0,  blk64, PASS_4, iDCTHorzFlags);
    
    piSrc0 = rgTemp; //piDst->i32;
    blk16  = piDst;
    g_IDCTDec_WMV3_Pass4(piSrc0,  blk16, PASS_4);
}

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterError8x8_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ)
{
    UnionBuffer * ppxliErrorQMB = pMainLoop->m_rgiCoefReconBuf;
    I32_WMV result;
    I64_WMV  *piSrc0 = (I64_WMV *)ppxliErrorQMB->i32;
    I16_WMV * blk16 = ppxliErrorQMB->i16;  
    I64_WMV tmpBuffer[32];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterError8x8_SSIMD);
    
    result = DecodeInverseInterBlockQuantize16_EMB(pMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_8x8, 
        pDQ);
    
    g_IDCTDec_WMV3_Pass1(piSrc0, tmpBuffer, PASS_8, pMainLoop->m_iDCTHorzFlags);
    g_IDCTDec_WMV3_Pass2(tmpBuffer, blk16, PASS_8);
    
    return result;
}

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterError8x4_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i)
{
    UnionBuffer * ppxliErrorQMB = pMainLoop->m_ppxliErrorQ;
    I32_WMV result;
    I32_WMV * piDst = ppxliErrorQMB->i32 + (i << 3);
    I16_WMV  * blk16;
    I64_WMV tmpBuffer[16];
    const I64_WMV  * piSrc0 = (I64_WMV *)pMainLoop->m_rgiCoefReconBuf->i32;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterError8x4_SSIMD);
    
    result = DecodeInverseInterBlockQuantize16_EMB(pMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_8x4, 
        pDQ);
    
    
    g_IDCTDec_WMV3_Pass1(piSrc0, tmpBuffer, PASS_4, pMainLoop->m_iDCTHorzFlags);
    
    blk16  = (I16_WMV *)piDst;
    
    g_IDCTDec_WMV3_Pass4(tmpBuffer,  blk16, PASS_8);
    
    return result;
}

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterError4x8_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i)
{
    UnionBuffer * ppxliErrorQMB = pMainLoop->m_ppxliErrorQ;
    I32_WMV result;
    I32_WMV * piDst = ppxliErrorQMB->i32 + (i);
    I16_WMV * blk16;
    I64_WMV tmpBuffer[4*2*4];
    const I64_WMV  * piSrc0 = (I64_WMV *)pMainLoop->m_rgiCoefReconBuf->i32;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterError4x8_SSIMD);
    
    result = DecodeInverseInterBlockQuantize16_EMB(pMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_4x8, 
        pDQ);
    
    g_IDCTDec_WMV3_Pass3(piSrc0,  tmpBuffer, PASS_8, pMainLoop->m_iDCTHorzFlags);
    blk16  = (I16_WMV *)piDst;
    g_IDCTDec_WMV3_Pass2(tmpBuffer, blk16, PASS_4);
    
    return result;
}

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_DecodeInterError4x4_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i)
{
    UnionBuffer * ppxliErrorQMB = pMainLoop->m_ppxliErrorQ;
    I32_WMV result;
    I16_WMV* piDst = ppxliErrorQMB->i16 + (i&2)*8 + (i&1)*2;
    I64_WMV  *piSrc0; 
    I64_WMV rgTemp[16];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DecodeInterError4x4_SSIMD);
    
    result = DecodeInverseInterBlockQuantize16_EMB(pMainLoop, 
        ppInterDCTTableInfo_Dec, 
        XFORMMODE_4x4, 
        pDQ);
    
    piSrc0 = (I64_WMV *)pMainLoop->m_rgiCoefReconBuf->i32;
    g_IDCTDec_WMV3_Pass3(piSrc0,  rgTemp, PASS_4, pMainLoop->m_iDCTHorzFlags);
    
    //blk16  = piDst;
    g_IDCTDec_WMV3_Pass4(rgTemp,  piDst, PASS_4);
    
    return result;
}

#endif  //_EMB_SSIMD64_
