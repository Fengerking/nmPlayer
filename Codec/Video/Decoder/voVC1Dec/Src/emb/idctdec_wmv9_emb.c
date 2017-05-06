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

#   define PASS_8          4
#   define PASS_4          2
#   define HYBRID_PASS_8   4
#   define HYBRID_PASS_4   2


U32_WMV DCT_COEF1[6]= {0x00100004,0x0009000f,0x00100006,0x000c000c,0x00006000,0x00005555};
U32_WMV DCT_COEF2[2]= {0x00160011,0x000a000a};


///////////////////////////////////////////////////////////////////////////
//
// Intra IDCT Functions
//
///////////////////////////////////////////////////////////////////////////
// C Version
// Input: 32-bit
// Output: 8-bit
//#ifndef WMV_OPT_IDCT_ARM
#define SATURATE8(x)   ((x) < 0 ? 0 : ((x) > 255 ? 255 : (x)))       
Void_WMV g_IDCTDec_WMV3 (U8_WMV __huge* piDst, I32_WMV iOffsetToNextRowForDCT, const I32_WMV __huge* rgiCoefRecon)
{
    
    I32_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a;
    I32_WMV  y3, y4, y5, y4a, b0,c0,b1,c1,ls_signbit;
    I32_WMV  *piSrc0;
    I32_WMV i,j;
    //register I32_WMV iDCTVertFlag = 0;
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
    //register I32_WMV iDCTVertFlag = 0;
    //const I16_WMV * rgiCoefRecon = piSrc->i16;
    I16_WMV * blk16 = piDst->i16;
    I32_WMV * blk32 = piDst->i32;
    I32_WMV tmpBuffer[64];
   
    
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
    
    I32_WMV i;
    I32_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a;
    I32_WMV  y3,  b0,c0,b1,c1;
    
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
        //x4a = w3*x4 - w5*x5
        //x5a = w3*x5 + w5*x4
        //x4 = w7*x5 + w1*x4
        //x5 = w7*x4 - w1*x5
        y3 = x4 + x5;
        x8 = W3 * y3;           //15
        x4a = x8 - W3pW5 * x5;  //24
        x5a = x8 - W3_W5 * x4;  //6
        x8 = W7 * y3;           //4
        x4 = x8 + W1_W7 * x4;   //12
        x5 = x8 - W1pW7 * x5;   //20
        
        // first stage
        //
        //
        //
        //
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

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec_WMV3_Pass2(const I32_WMV  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops)
{
    I32_WMV i;
    I32_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a;
    I32_WMV  y4, y4a, b0,b1,ls_signbit;
    
    for (i = 0; i < iNumLoops; i++){
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


Void_WMV g_IDCTDec_WMV3_Pass1_new(const I32_WMV  *piSrc0, I32_WMV * blk32, const I32_WMV iNumLoops, I32_WMV iDCTHorzFlags)
{
    
    I32_WMV i;
    I32_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a;
    I32_WMV  y3, b0,b1;
    
    for ( i = 0; i < 2*iNumLoops; i++,  blk32 += 8, iDCTHorzFlags >>= 2 )
    {
        if(!(iDCTHorzFlags&3)) 
        {
            I32_WMV iCurr, iNext;
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
        
        x4 = piSrc0[ i +1*8 ];      
        x3 = piSrc0[ i +2*8 ];
        x7 = piSrc0[ i +3*8 ];
        x1 = piSrc0[ i +4*8 ];
        x6 = piSrc0[ i +5*8 ];
        x2 = piSrc0[ i +6*8 ];      
        x5 = piSrc0[ i +7*8 ];
        x0 = piSrc0[ i +0*8 ]; /* for proper rounding */
        
        
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
        blk32 [0] = (I16_WMV) ((x7 + x4) >> 3);
        blk32 [1] = (I16_WMV) ((x3 + x4a) >> 3);
        blk32 [2] = (I16_WMV) ((x0 + x5a) >> 3);
        blk32 [3] = (I16_WMV) ((x8 + x5) >> 3);
        blk32 [4] = (I16_WMV) ((x8 - x5) >> 3);
        blk32 [5] = (I16_WMV) ((x0 - x5a) >> 3);
        blk32 [6] = (I16_WMV) ((x3 - x4a) >> 3);
        blk32 [7] = (I16_WMV) ((x7 - x4) >> 3);     
        
        //// blk [0,1]
        //b0 = x7 + x4;	// sw: b0 = 12*x0 + 16*x4 + 16*x3 + 15*x7 + 12*x1 + 9*x6 + 6*x2 + 4*x5 + rounding
        //b1 = (b0 + 0x8000)>>19;
        //b0 = ((I16_WMV)b0)>>3;
        //
        //c0 = x3 + x4a;	// sw: c0 = 12*x0 + 15*x4 + 6*x3 + -4*x7 + -12*x1 + -16*x6 + -16*x2 + -9*x5 + rounding
        //c1 = (c0 + 0x8000)>>19;
        //c0 = ((I16_WMV)c0)>>3;
        //
        //blk32[0] = (c0<<16) + b0;
        //blk32[0+4] = (c1<<16) + b1;
        //
        //// blk [2,3]
        //b0 = x0 + x5a;
        //b1 = (b0 + 0x8000)>>19;
        //b0 = ((I16_WMV)b0)>>3;
        //
        //c0 = x8 + x5;
        //c1 = (c0 + 0x8000)>>19;
        //c0 = ((I16_WMV)c0)>>3;
        //
        //blk32[1] = (c0<<16) + b0;
        //blk32[1+4] = (c1<<16) + b1;
        //
        //// blk [4,5]
        //b0 = x8 - x5;
        //b1 = (b0 + 0x8000)>>19;
        //b0 = ((I16_WMV)b0)>>3;
        //
        //c0 = x0 - x5a;
        //c1 = (c0 + 0x8000)>>19;
        //c0 = ((I16_WMV)c0)>>3;
        //
        //blk32[2] = (c0<<16) + b0;
        //blk32[2+4] = (c1<<16) + b1;
        //
        //// blk [6,7]
        //b0 = x3 - x4a;
        //b1 = (b0 + 0x8000)>>19;
        //b0 = ((I16_WMV)b0)>>3;
        //
        //c0 = x7 - x4;
        //c1 = (c0 + 0x8000)>>19;
        //c0 = ((I16_WMV)c0)>>3;
        //
        //blk32[3] = (c0<<16) + b0;
        //blk32[3+4] = (c1<<16) + b1;
        
    }
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec_WMV3_Pass2_new(const I32_WMV  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops)
{
    I32_WMV i;
    I32_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a;
    I32_WMV  y4, y4a, b0,b1,ls_signbit;
    
    for (i = 0; i < iNumLoops; i++){
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

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec_WMV3_Pass3(const I32_WMV  *piSrc0, I32_WMV * blk32, const I32_WMV iNumLoops,  I32_WMV iDCTHorzFlags)
{
    I32_WMV i;
    I32_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8;
    I32_WMV  b0,c0,b1,c1;
    
    for ( i = 0; i < iNumLoops; i++, blk32 += 8, iDCTHorzFlags >>= 2)
    {
        if(!(iDCTHorzFlags&3))
        {
            I32_WMV iCurr, iNext;
            
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

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec_WMV3_Pass4(const I32_WMV  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops)
{
	//sw: the size of buffer blk16 is (8*8*2)*4.

    I32_WMV i;
    I32_WMV  x1, x3, x4, x5, x6, x7, x8,  x5a;
    I32_WMV  b0,b1,ls_signbit;
    
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
//#endif //WMV_OPT_IDCT_ARM

//#pragma code_seg (EMBSEC_PML)

void SignPatch_C(I16_WMV * rgiCoefRecon, int len)
{
    int i;
    I16_WMV *piSrcTmp = rgiCoefRecon;
    I16_WMV * dsttmp = rgiCoefRecon;
    
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
void SignPatch32_C(I32_WMV * rgiCoefRecon, int len)
{
    int i;
    I32_WMV v1, v2;
    
    for(i=0; i < (len >> 2); i++)
    {
        v1 = rgiCoefRecon[i*2];
        v2 = rgiCoefRecon[i*2+1];
        
        rgiCoefRecon[i] = (v1 & 0x0000ffff) | (((v1 >> 16) + v2) << 16);
    }
}

Void_WMV g_SubBlkIDCTClear_EMB_C (UnionBuffer * piDstBuf,  I32_WMV iIdx)
{
    I32_WMV iNumLoop = 8<<(iIdx>>7);
    I32_WMV iStep   =  1<< ((iIdx >>6)&0x1);
    I32_WMV iStep2 = iStep<<1;
    I32_WMV* piDst = piDstBuf->i32 + (iIdx&0x3f);
    I32_WMV i;
    for (i = 0; i < iNumLoop; i+=iStep2) 
    {
        piDst[i] = 0;
        piDst[i+16] = 0;
        piDst[i + iStep] = 0;
        piDst[i+16 + iStep] = 0;
    }
}

Void_WMV g_SubBlkIDCTClear_EMB_C_new (short * piDstBuf,I32_WMV iDststride,  I32_WMV iXformType,I32_WMV i)
{
    int j=0;
    if(iXformType == XFORMMODE_4x4)
    {
        for(j=0;j<4;j++)
         {
             piDstBuf[0] = 0; 
             piDstBuf[1] = 0;
             piDstBuf[2] = 0;
             piDstBuf[3] = 0;
             piDstBuf += iDststride;
         }
    }
    else if(iXformType == XFORMMODE_8x4)
    {
        for(j=0;j<4;j++)
         {
             piDstBuf[0] =  0; 
             piDstBuf[1] =  0; 
             piDstBuf[2] =  0;
             piDstBuf[3] =  0;
             piDstBuf[4] =  0; 
             piDstBuf[5] =  0; 
             piDstBuf[6] =  0; 
             piDstBuf[7] =  0;
             piDstBuf += iDststride;
         }  
    }
    else if(iXformType == XFORMMODE_4x8)
    {
        for(j=0;j<8;j++)
        {
            piDstBuf[0] = 0; 
            piDstBuf[1] = 0;
            piDstBuf[2] = 0; 
            piDstBuf[3] = 0;
            piDstBuf += iDststride;
        }   
    } 
    
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_IDCTDec16_WMV3_SSIMD_C (UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags)
{
    I32_WMV  *piSrc0;
    //const I16_WMV * rgiCoefRecon = piSrc->i16;
    I16_WMV * blk16 = piDst->i16;
    I32_WMV * blk32 = piDst->i32;    
    I32_WMV tmpBuffer[64];
    
    piSrc0 = piSrc->i32;
    blk32 = tmpBuffer;
    
    g_IDCTDec_WMV3_Pass1(piSrc0, blk32, HYBRID_PASS_8, iDCTHorzFlags);
    
    piSrc0 = tmpBuffer;
    blk16  = piDst->i16;
    
    g_IDCTDec_WMV3_Pass2(piSrc0, blk16, PASS_8); 
     
}
#if 0
static const I16_WMV T8[8][8] = 
{
    {  12,  12,  12,  12,  12,  12,  12,  12 },
    {  16,  15,   9,   4,  -4,  -9, -15, -16 },
    {  16,   6,  -6, -16, -16,  -6,   6,  16 },
    {  15,  -4, -16,  -9,   9,  16,   4, -15 },
    {  12, -12, -12,  12,  12, -12, -12,  12 },
    {   9, -16,   4,  15, -15,  -4,  16,  -9 },
    {   6, -16,  16,  -6,  -6,  16, -16,   6 },
    {   4,  -9,  15, -16,  16, -15,   9,  -4 }
};



/* 4x4 Inverse Transform matrix defined by the standard */

static const I16_WMV T4[4][4] =
{
    {  17,  17,  17,  17 },
    {  22,  10, -10, -22 },
    {  17, -17, -17,  17 },
    {  10, -22,  22, -10 }
};
static const I16_WMV C8[8] = 
{
    0,   0,   0,   0,   1,   1,   1,   1
};


/* C4 vector defined by the standard */


static const I16_WMV C4[4] = 
{
    0,   0,   0,   0
};

static const I16_WMV T8_test[8][8] = 
{

    {2048,  2048,   2048,   2048,   2048,   2048,   2048,   2048,   },

    {2841,  2408,   1609,   565,    -565,   -1609,  -2408,  -2841,  },

    {2676,  1108,   -1108,  -2676,  -2676,  -1108,  1108,   2676,   },

    {2408,  -565,   -2841,  -1609,  1609,   2841,   565,    -2408,  },

    {2048,  -2048,  -2048,  2048,   2048,   -2048,  -2048,  2048,   },

    {1609,  -2841,  565,    2408,   -2408,  -565,   2841,   -1609,  },

    {1108,  -2676,  2676,   -1108,  -1108,  2676,   -2676,  1108,   },

    {565,   -1609,  2408,   -2841,  2841,   -2408,  1609,   -565,   },

};


#define SAT(Value) (VO_U8)(Value < 0 ? 0: (Value > 255 ? 255: Value))
void vc1ITRANS_InverseTransform_AnnexA1_8x8test(I16_WMV *pD, I16_WMV *pR, int M, int N)
{
    I16_WMV pE[8*8];
    const I16_WMV *pT, *pC;
    U16_WMV i, j, k;
    I32_WMV acc;


    /* COL transform:
     *     Equation: E = (M * T + 4) >> 3
     */
    pT = T8[0];
    for (j = 0; j < N; j++)
    {
        for (i = 0; i < M; i++)
        {
            for (acc = 0, k = 0; k < M; k++)
            {
                acc += pR[k*8+j]*pT[k*M+i];
            }
            pE[i*8+j] = ((I16_WMV) ((acc + 4) >> 3));
        }
    }


    /*Row transform:
     *   Equation: R = (T' * E + C * 1 + 64) >> 7
     */
    pT =  T8[0];
    pC =  C8;
    for (i = 0; i < M; i++)
    {
        for (j = 0; j < N; j++)
        {
            for (acc = 0, k = 0; k < N; k++)
            {
                acc += pE[i*8+k]*pT[k*N+j];
            }
            //pD[i*8+j] = ((I16_WMV) ((acc + pC[j] + 64) >> 7));
            pD[j*8+i] = ((I16_WMV) ((acc + pC[j] + 64) >> 7));
        }
    }
}

void vc1ITRANS_InverseTransform_AnnexA1(I16_WMV *pD, I16_WMV *pR, int M, int N)
{
    I16_WMV pE[8*8];
    const I16_WMV *pT, *pC;
    U16_WMV i, j, k;
    I32_WMV acc;


    /* Row transform:
     *     Equation: E = (M * T + 4) >> 3
     */
    pT = (M == 8) ? T8[0] : T4[0];
    for (j = 0; j < N; j++)
    {
        for (i = 0; i < M; i++)
        {
            for (acc = 0, k = 0; k < M; k++)
            {
                acc += pR[j*8+k]*pT[k*M+i];
            }
            pE[j*8+i] = (I16_WMV) ((acc + 4) >> 3);
        }
    }


    /* Column transform:
     *   Equation: R = (T' * E + C * 1 + 64) >> 7
     */
    pT = (N == 8) ? T8[0] : T4[0];
    pC = (N == 8) ? C8 : C4;
    for (i = 0; i < M; i++)
    {
        for (j = 0; j < N; j++)
        {
            for (acc = 0, k = 0; k < N; k++)
            {
                acc += pE[i+k*8]*pT[k*N+j];
            }
            pD[i+j*8] = (I16_WMV) ((acc + pC[j] + 64) >> 7);
        }
    }
}
#endif
#define C1     16                 // 2048*sqrt(2)*cos(1*pi/16) 0.9808   Q4
#define C2     16                 // 2048*sqrt(2)*cos(2*pi/16) 0.9239
#define C3     15                 // 2048*sqrt(2)*cos(3*pi/16) 0.8315
#define C4     12                 // 2048*sqrt(2)*cos(4*pi/16) 2048   12                181 =  (2048/sqrt(2) )>>3   181 = (sqrt(2)/2)<<8
#define C5     9                  // 2048*sqrt(2)*cos(5*pi/16) 0.5556
#define C6     6                  // 2048*sqrt(2)*cos(6*pi/16) 0.3827
#define C7     4                  // 2048*sqrt(2)*cos(7*pi/16) 0.1951

void vc1_inv_trans_8x8_c_short(short* pDest,int dststride,short *block)
{
    int i;
    register int t1,t2,t3,t4,t5,t6,t7,t8;
    short *src, *dst;

    src = block;
    dst = block;
    for(i = 0; i < 8; i++){
        t1 = 12 * (src[0*8] + src[4*8]) + 4;
        t2 = 12 * (src[0*8] - src[4*8]) + 4;
        t3 = 16 * src[2*8] +  6 * src[6*8];
        t4 =  6 * src[2*8] - 16 * src[6*8];

        t5 = t1 + t3;
        t6 = t2 + t4;
        t7 = t2 - t4;
        t8 = t1 - t3;

        t1 = 16 * src[1*8] + 15 * src[3*8] +  9 * src[5*8] +  4 * src[7*8];
        t2 = 15 * src[1*8] -  4 * src[3*8] - 16 * src[5*8] -  9 * src[7*8];
        t3 =  9 * src[1*8] - 16 * src[3*8] +  4 * src[5*8] + 15 * src[7*8];
        t4 =  4 * src[1*8] -  9 * src[3*8] + 15 * src[5*8] - 16 * src[7*8];

        dst[0*8] = (t5 + t1) >> 3;
        dst[1*8] = (t6 + t2) >> 3;
        dst[2*8] = (t7 + t3) >> 3;
        dst[3*8] = (t8 + t4) >> 3;
        dst[4*8] = (t8 - t4) >> 3;
        dst[5*8] = (t7 - t3) >> 3;
        dst[6*8] = (t6 - t2) >> 3;
        dst[7*8] = (t5 - t1) >> 3;

        src ++;
        dst ++;
    }

    src = block;
    for(i = 0; i < 8; i++){
        t1 = 12 * (src[ 0] + src[4]) + 64;
        t2 = 12 * (src[ 0] - src[4]) + 64;
        t3 = 16 * src[2] +  6 * src[6];
        t4 =  6 * src[2] - 16 * src[6];

        t5 = t1 + t3;
        t6 = t2 + t4;
        t7 = t2 - t4;
        t8 = t1 - t3;

        t1 = 16 * src[ 1] + 15 * src[3] +  9 * src[5] +  4 * src[7];
        t2 = 15 * src[ 1] -  4 * src[3] - 16 * src[5] -  9 * src[7];
        t3 =  9 * src[ 1] - 16 * src[3] +  4 * src[5] + 15 * src[7];
        t4 =  4 * src[ 1] -  9 * src[3] + 15 * src[5] - 16 * src[7];

        pDest[ 0] = (t5 + t1) >> 7;
        pDest[1*dststride]= (t6 + t2) >> 7;
        pDest[2*dststride] = (t7 + t3) >> 7;
        pDest[3*dststride] = (t8 + t4) >> 7;
        pDest[4*dststride] = (t8 - t4 + 1) >> 7;
        pDest[5*dststride] = (t7 - t3 + 1) >> 7;
        pDest[6*dststride] = (t6 - t2 + 1) >> 7;
        pDest[7*dststride] = (t5 - t1 + 1) >> 7;
        src +=8;
        pDest++;
    }
}


void voVC1InvTrans_8x8_Overlap_C(unsigned char*pDest,
                                            int dststride,
                                            unsigned char* pRef,
                                            short* pDestOverlap,
                                            int refstride,
                                            short *block,
                                            int overlapstride,
											unsigned long *table)
{
    int i = 0;
    short *src, *dst;

    VO_S32 A, B, C, D, Ad, Bd, Cd, Dd, E, F, G, H;
	VO_S32 Ed, Gd, Add, Bdd, Fd, Hd;

    src = block;
    dst = block;
    for(i = 0; i < 8; i++){

        A = C7 * src[7*8] + C1 * src[1*8];
	    B = C7 * src[1*8] - C1 * src[7*8];
	    C = C3 * src[3*8] + C5 * src[5*8];
	    D = C3 * src[5*8] - C5 * src[3*8];
	    G = C6 * src[6*8] + C2 * src[2*8];
	    H = C6 * src[2*8] - C2 * src[6*8];

	    Ad = 24576*A - 21845*C;
	    Bd = 24576*B - 21845*D;
	    Add = ((Ad - Bd + 16*1024) >>15);
	    Bdd = ((Ad + Bd + 16*1024) >> 15);
	    Cd = A + C;
	    Dd = B + D;

        E = (src[0*8] + src[4*8]) *C4 + 4;
		F = (src[0*8] - src[4*8]) *C4 + 4;
		Ed = E - G;
		Fd = E + G;
		Gd = F - H;		
		Hd = F + H;

        dst[0*8] = (VO_S16)((Fd + Cd) >> 3);
        dst[7*8] = (VO_S16)((Fd - Cd) >> 3);
	    dst[1*8] = (VO_S16)((Hd + Bdd) >> 3);
	    dst[6*8] = (VO_S16)((Hd - Bdd) >> 3);
	    dst[3*8] = (VO_S16)((Ed + Dd) >> 3);
	    dst[4*8] = (VO_S16)((Ed - Dd) >> 3);
	    dst[2*8] = (VO_S16)((Gd + Add) >> 3);
	    dst[5*8] = (VO_S16)((Gd - Add) >> 3);

        src += 1;
        dst += 1;
    }

    src = block;
    for(i = 0; i < 8; i++){

        A = C7 * src[7] + C1 * src[1];
	    B = C7 * src[1] - C1 * src[7];
	    C = C3 * src[3] + C5 * src[5];
	    D = C3 * src[5] - C5 * src[3];
	    G = C6 * src[6] + C2 * src[2];
	    H = C6 * src[2] - C2 * src[6];

        Ad = 24576*A - 21845*C;
	    Bd = 24576*B - 21845*D;
	    Add = ((Ad - Bd + 16*1024) >>15);
	    Bdd = ((Ad + Bd + 16*1024) >> 15);
	    Cd = A + C;
	    Dd = B + D;

        E = (src[0] + src[4]) *C4 + 64;
		F = (src[0] - src[4]) *C4 + 64;
        Ed = E - G;
		Fd = E + G;
		Gd = F - H;		
		Hd = F + H;

        pDestOverlap[ 0] = (Fd + Cd) >> 7;
        pDestOverlap[1*overlapstride]= (Hd + Bdd) >> 7;
        pDestOverlap[2*overlapstride] = (Gd + Add) >> 7;
        pDestOverlap[3*overlapstride] = (Ed + Dd) >> 7;
        pDestOverlap[4*overlapstride] = (Ed - Dd + 1) >> 7;
        pDestOverlap[5*overlapstride] = (Gd - Add + 1) >> 7;
        pDestOverlap[6*overlapstride] = (Hd - Bdd + 1) >> 7;
        pDestOverlap[7*overlapstride] = (Fd - Cd + 1) >> 7;

        if(!pRef)
        {
            pDest[ 0] = (Fd + Cd) >> 7;
            pDest[1*dststride]= (Hd + Bdd) >> 7;
            pDest[2*dststride] = (Gd + Add) >> 7;
            pDest[3*dststride] = (Ed + Dd) >> 7;
            pDest[4*dststride] = (Ed - Dd + 1) >> 7;
            pDest[5*dststride] = (Gd - Add + 1) >> 7;
            pDest[6*dststride] = (Hd - Bdd + 1) >> 7;
            pDest[7*dststride] = (Fd - Cd + 1) >> 7;
        }
        else
        {
            pDest[ 0] =             SATURATE8((pRef[0])+((Fd + Cd) >> 7));
            pDest[1*dststride] = SATURATE8((pRef[1*refstride])+((Hd + Bdd) >> 7));
            pDest[2*dststride] = SATURATE8((pRef[2*refstride])+((Gd + Add) >> 7));
            pDest[3*dststride] = SATURATE8((pRef[3*refstride])+((Ed + Dd) >> 7));
            pDest[4*dststride] = SATURATE8((pRef[4*refstride])+((Ed - Dd + 1) >> 7));
            pDest[5*dststride] = SATURATE8((pRef[5*refstride])+((Gd - Add + 1) >> 7));
            pDest[6*dststride] = SATURATE8((pRef[6*refstride])+((Hd - Bdd + 1) >> 7));
            pDest[7*dststride] = SATURATE8((pRef[7*refstride])+((Fd - Cd + 1) >> 7));
            pRef++;    
        }

        src += 8;
        pDest++;
        pDestOverlap++;
        
    }
}


void voVC1InvTrans_8x8_C(unsigned char* pDest,
                                int dststride,
                                unsigned char* pRef0,
                                unsigned char* pRef1,
                                int refstride,
                                short *block,
								unsigned long *table0,
								unsigned long *table1)
{
    int i;
    short *src, *dst;

    VO_S32 A, B, C, D, Ad, Bd, Cd, Dd, E, F, G, H;
	VO_S32 Ed, Gd, Add, Bdd, Fd, Hd;

    src = block;
    dst = block;
    for(i = 0; i < 8; i++){

        A = C7 * src[7*8] + C1 * src[1*8];
	    B = C7 * src[1*8] - C1 * src[7*8];
	    C = C3 * src[3*8] + C5 * src[5*8];
	    D = C3 * src[5*8] - C5 * src[3*8];
	    G = C6 * src[6*8] + C2 * src[2*8];
	    H = C6 * src[2*8] - C2 * src[6*8];

	    Ad = 24576*A - 21845*C;
	    Bd = 24576*B - 21845*D;
	    Add = ((Ad - Bd + 16*1024) >>15);
	    Bdd = ((Ad + Bd + 16*1024) >> 15);
	    Cd = A + C;
	    Dd = B + D;

        E = (src[0*8] + src[4*8]) *C4 + 4;
		F = (src[0*8] - src[4*8]) *C4 + 4;
		Ed = E - G;
		Fd = E + G;
		Gd = F - H;		
		Hd = F + H;

        dst[0*8] = (VO_S16)((Fd + Cd) >> 3);
        dst[7*8] = (VO_S16)((Fd - Cd) >> 3);
	    dst[1*8] = (VO_S16)((Hd + Bdd) >> 3);
	    dst[6*8] = (VO_S16)((Hd - Bdd) >> 3);
	    dst[3*8] = (VO_S16)((Ed + Dd) >> 3);
	    dst[4*8] = (VO_S16)((Ed - Dd) >> 3);
	    dst[2*8] = (VO_S16)((Gd + Add) >> 3);
	    dst[5*8] = (VO_S16)((Gd - Add) >> 3);

        src += 1;
        dst += 1;
    }

    src = block;
    for(i = 0; i < 8; i++){

        A = C7 * src[7] + C1 * src[1];
	    B = C7 * src[1] - C1 * src[7];
	    C = C3 * src[3] + C5 * src[5];
	    D = C3 * src[5] - C5 * src[3];
	    G = C6 * src[6] + C2 * src[2];
	    H = C6 * src[2] - C2 * src[6];

        Ad = 24576*A - 21845*C;
	    Bd = 24576*B - 21845*D;
	    Add = ((Ad - Bd + 16*1024) >>15);
	    Bdd = ((Ad + Bd + 16*1024) >> 15);
	    Cd = A + C;
	    Dd = B + D;

        E = (src[0] + src[4]) *C4 + 64;
		F = (src[0] - src[4]) *C4 + 64;
        Ed = E - G;
		Fd = E + G;
		Gd = F - H;		
		Hd = F + H;

        if(!pRef0 && !pRef1)
        {
            pDest[ 0] = (Fd + Cd) >> 7;
            pDest[1*dststride]= (Hd + Bdd) >> 7;
            pDest[2*dststride] = (Gd + Add) >> 7;
            pDest[3*dststride] = (Ed + Dd) >> 7;
            pDest[4*dststride] = (Ed - Dd + 1) >> 7;
            pDest[5*dststride] = (Gd - Add + 1) >> 7;
            pDest[6*dststride] = (Hd - Bdd + 1) >> 7;
            pDest[7*dststride] = (Fd - Cd + 1) >> 7;
        }
        else if(pRef0 && !pRef1)
        {
            pDest[ 0] =             SATURATE8((pRef0[0])+((Fd + Cd) >> 7));
            pDest[1*dststride] = SATURATE8((pRef0[1*refstride])+((Hd + Bdd) >> 7));
            pDest[2*dststride] = SATURATE8((pRef0[2*refstride])+((Gd + Add) >> 7));
            pDest[3*dststride] = SATURATE8((pRef0[3*refstride])+((Ed + Dd) >> 7));
            pDest[4*dststride] = SATURATE8((pRef0[4*refstride])+((Ed - Dd + 1) >> 7));
            pDest[5*dststride] = SATURATE8((pRef0[5*refstride])+((Gd - Add + 1) >> 7));
            pDest[6*dststride] = SATURATE8((pRef0[6*refstride])+((Hd - Bdd + 1) >> 7));
            pDest[7*dststride] = SATURATE8((pRef0[7*refstride])+((Fd - Cd + 1) >> 7));
            pRef0++;    
        }
        else
        {
            pDest[ 0] =             SATURATE8(((pRef0[0]+pRef1[0]+1)>>1)+((Fd + Cd) >> 7));
            pDest[1*dststride] = SATURATE8(((pRef0[1*refstride]+pRef1[1*refstride]+1)>>1)+((Hd + Bdd) >> 7));
            pDest[2*dststride] = SATURATE8(((pRef0[2*refstride]+pRef1[2*refstride]+1)>>1)+((Gd + Add) >> 7));
            pDest[3*dststride] = SATURATE8(((pRef0[3*refstride]+pRef1[3*refstride]+1)>>1)+((Ed + Dd) >> 7));
            pDest[4*dststride] = SATURATE8(((pRef0[4*refstride]+pRef1[4*refstride]+1)>>1)+((Ed - Dd + 1) >> 7));
            pDest[5*dststride] = SATURATE8(((pRef0[5*refstride]+pRef1[5*refstride]+1)>>1)+((Gd - Add + 1) >> 7));
            pDest[6*dststride] = SATURATE8(((pRef0[6*refstride]+pRef1[6*refstride]+1)>>1)+((Hd - Bdd + 1) >> 7));
            pDest[7*dststride] = SATURATE8(((pRef0[7*refstride]+pRef1[7*refstride]+1)>>1)+((Fd - Cd + 1) >> 7));
            pRef0++;    
            pRef1++;
        }

        src += 8;
        pDest++;
        
    }
}

void voVC1InvTrans_8x8_DC_C(unsigned char* pDest,
                                int dststride,
                                unsigned char* pRef0,
                                unsigned char* pRef1,
                                int refstride,
                                short *block,
								unsigned long *table0,
								unsigned long *table1)
{
    int i;
    VO_S32 E;

	E = ((block[0]*12 + 4)>>3) *12 + 64;

    for(i = 0; i < 8; i++)
	{        
        if(!pRef0 && !pRef1)
        {
            pDest[0*dststride] = (E ) >> 7;
            pDest[1*dststride] = (E ) >> 7;
            pDest[2*dststride] = (E ) >> 7;
            pDest[3*dststride] = (E ) >> 7;
            pDest[4*dststride] = (E  + 1) >> 7;
            pDest[5*dststride] = (E  + 1) >> 7;
            pDest[6*dststride] = (E  + 1) >> 7;
            pDest[7*dststride] = (E  + 1) >> 7;
        }
        else if(pRef0 && !pRef1)
        {
            pDest[0*dststride] = SATURATE8((pRef0[0*refstride])+((E ) >> 7));
            pDest[1*dststride] = SATURATE8((pRef0[1*refstride])+((E ) >> 7));
            pDest[2*dststride] = SATURATE8((pRef0[2*refstride])+((E ) >> 7));
            pDest[3*dststride] = SATURATE8((pRef0[3*refstride])+((E ) >> 7));
            pDest[4*dststride] = SATURATE8((pRef0[4*refstride])+((E  + 1) >> 7));
            pDest[5*dststride] = SATURATE8((pRef0[5*refstride])+((E  + 1) >> 7));
            pDest[6*dststride] = SATURATE8((pRef0[6*refstride])+((E  + 1) >> 7));
            pDest[7*dststride] = SATURATE8((pRef0[7*refstride])+((E  + 1) >> 7));
            pRef0++;    
        }
        else
        {
            pDest[0*dststride] = SATURATE8(((pRef0[0*refstride]+pRef1[0*refstride]+1)>>1)+((E ) >> 7));
            pDest[1*dststride] = SATURATE8(((pRef0[1*refstride]+pRef1[1*refstride]+1)>>1)+((E ) >> 7));
            pDest[2*dststride] = SATURATE8(((pRef0[2*refstride]+pRef1[2*refstride]+1)>>1)+((E ) >> 7));
            pDest[3*dststride] = SATURATE8(((pRef0[3*refstride]+pRef1[3*refstride]+1)>>1)+((E ) >> 7));
            pDest[4*dststride] = SATURATE8(((pRef0[4*refstride]+pRef1[4*refstride]+1)>>1)+((E  + 1) >> 7));
            pDest[5*dststride] = SATURATE8(((pRef0[5*refstride]+pRef1[5*refstride]+1)>>1)+((E  + 1) >> 7));
            pDest[6*dststride] = SATURATE8(((pRef0[6*refstride]+pRef1[6*refstride]+1)>>1)+((E  + 1) >> 7));
            pDest[7*dststride] = SATURATE8(((pRef0[7*refstride]+pRef1[7*refstride]+1)>>1)+((E  + 1) >> 7));
            pRef0++;    
            pRef1++;
        }
        pDest++;
        
    }
}

void voVC1InvTrans_8x4_C(unsigned char* pDest,
						 int dststride,
						 unsigned char* pRef0,
						 unsigned char* pRef1,
						 int refstride,
						 short *block,
						 unsigned long *table0,
						 unsigned long *table1)
{
    int i;
    register int t1,t2,t3,t4;
    short *src, *dst;
    //const U8_CMV *cm = ff_cropTbl + MAX_NEG_CROP;
    VO_S32 A, B, C, D, Ad, Bd, Cd, Dd, E, F, G, H;
	VO_S32 Ed, Gd, Add, Bdd, Fd, Hd;

    src = block;
    dst = block;
    for(i = 0; i < 4; i++){

        A = C7 * src[7*8] + C1 * src[1*8];
	    B = C7 * src[1*8] - C1 * src[7*8];
	    C = C3 * src[3*8] + C5 * src[5*8];
	    D = C3 * src[5*8] - C5 * src[3*8];
	    G = C6 * src[6*8] + C2 * src[2*8];
	    H = C6 * src[2*8] - C2 * src[6*8];

	    Ad = 24576*A - 21845*C;
	    Bd = 24576*B - 21845*D;
	    Add = ((Ad - Bd + 16*1024) >>15);
	    Bdd = ((Ad + Bd + 16*1024) >> 15);
	    Cd = A + C;
	    Dd = B + D;

        E = (src[0*8] + src[4*8]) *C4 + 4;
		F = (src[0*8] - src[4*8]) *C4 + 4;
		Ed = E - G;
		Fd = E + G;
		Gd = F - H;		
		Hd = F + H;

        dst[0*8] = (VO_S16)((Fd + Cd) >> 3);
        dst[7*8] = (VO_S16)((Fd - Cd) >> 3);
	    dst[1*8] = (VO_S16)((Hd + Bdd) >> 3);
	    dst[6*8] = (VO_S16)((Hd - Bdd) >> 3);
	    dst[3*8] = (VO_S16)((Ed + Dd) >> 3);
	    dst[4*8] = (VO_S16)((Ed - Dd) >> 3);
	    dst[2*8] = (VO_S16)((Gd + Add) >> 3);
	    dst[5*8] = (VO_S16)((Gd - Add) >> 3);
        src += 1;
        dst += 1;
    }

    src = block;
    for(i = 0; i < 8; i++){           
        t1 = 17 * (src[ 0] + src[2]) + 64;
        t2 = 17 * (src[ 0] - src[2]) + 64;
        t3 = 22 * src[ 1] + 10 * src[3];
        t4 = 22 * src[3] - 10 * src[ 1];

        if(!pRef0 && !pRef1)
        {
            pDest[0*dststride] =  ((t1 + t3) >> 7);
            pDest[1*dststride] =  ((t2 - t4) >> 7);
            pDest[2*dststride] =  ((t2 + t4) >> 7);
            pDest[3*dststride] =  ((t1 - t3) >> 7);
        }
        else if(pRef0 && !pRef1)
        {
            pDest[0*dststride] =  SATURATE8((pRef0[0*refstride])+ ((t1 + t3) >> 7));
            pDest[1*dststride] =  SATURATE8((pRef0[1*refstride])+ ((t2 - t4) >> 7));
            pDest[2*dststride] =  SATURATE8((pRef0[2*refstride])+ ((t2 + t4) >> 7));
            pDest[3*dststride] =  SATURATE8((pRef0[3*refstride])+ ((t1 - t3) >> 7));
            pRef0++;
        }
        else
        {
            pDest[0*dststride] =  SATURATE8(((pRef0[0*refstride]+pRef1[0*refstride]+1)>>1)+ ((t1 + t3) >> 7));
            pDest[1*dststride] =  SATURATE8(((pRef0[1*refstride]+pRef1[1*refstride]+1)>>1)+ ((t2 - t4) >> 7));
            pDest[2*dststride] =  SATURATE8(((pRef0[2*refstride]+pRef1[2*refstride]+1)>>1)+ ((t2 + t4) >> 7));
            pDest[3*dststride] =  SATURATE8(((pRef0[3*refstride]+pRef1[3*refstride]+1)>>1)+ ((t1 - t3) >> 7));
            pRef0++;
            pRef1++;
        }
        src += 8;
        pDest++;        
    }
}

void voVC1InvTrans_8x4_DC_C(unsigned char* pDest,
						 int dststride,
						 unsigned char* pRef0,
						 unsigned char* pRef1,
						 int refstride,
						 short *block,
						 unsigned long *table0,
						 unsigned long *table1)
{
    int i;
    VO_S32 E;
	E = 17 * ((block[0] *12 + 4)>>3) + 64;

    for(i = 0; i < 8; i++)
	{ 
        if(!pRef0 && !pRef1)
        {
            pDest[0*dststride] =  ((E) >> 7);
            pDest[1*dststride] =  ((E) >> 7);
            pDest[2*dststride] =  ((E) >> 7);
            pDest[3*dststride] =  ((E) >> 7);
        }
        else if(pRef0 && !pRef1)
        {
            pDest[0*dststride] =  SATURATE8((pRef0[0*refstride])+ ((E) >> 7));
            pDest[1*dststride] =  SATURATE8((pRef0[1*refstride])+ ((E) >> 7));
            pDest[2*dststride] =  SATURATE8((pRef0[2*refstride])+ ((E) >> 7));
            pDest[3*dststride] =  SATURATE8((pRef0[3*refstride])+ ((E) >> 7));
            pRef0++;
        }
        else
        {
            pDest[0*dststride] =  SATURATE8(((pRef0[0*refstride]+pRef1[0*refstride]+1)>>1)+ ((E) >> 7));
            pDest[1*dststride] =  SATURATE8(((pRef0[1*refstride]+pRef1[1*refstride]+1)>>1)+ ((E) >> 7));
            pDest[2*dststride] =  SATURATE8(((pRef0[2*refstride]+pRef1[2*refstride]+1)>>1)+ ((E) >> 7));
            pDest[3*dststride] =  SATURATE8(((pRef0[3*refstride]+pRef1[3*refstride]+1)>>1)+ ((E) >> 7));
            pRef0++;
            pRef1++;
        }
        pDest++;        
    }
}

void voVC1InvTrans_4x8_C(unsigned char* pDest, 
						 int dststride, 
						 unsigned char *pRef0,
						 unsigned char *pRef1,
						 int refstride, 
						 short *block,
						 unsigned long *table0,
						 unsigned long *table1)
{
    int i;
    register int t1,t2,t3,t4;
    I16_WMV *src, *dst;
    VO_S32 A, B, C, D, Ad, Bd, Cd, Dd, E, F, G, H;
	VO_S32 Ed, Gd, Add, Bdd, Fd, Hd;

    src = block;
    dst = block;
    for(i = 0; i < 8; i++){
        t1 = 17 * (src[0*8] + src[2*8]) + 4;
        t2 = 17 * (src[0*8] - src[2*8]) + 4;
        t3 = 22 * src[1*8] + 10 * src[3*8];
        t4 = 22 * src[3*8] - 10 * src[1*8];

        dst[0*8] = (t1 + t3) >> 3;
        dst[1*8] = (t2 - t4) >> 3;
        dst[2*8] = (t2 + t4) >> 3;
        dst[3*8] = (t1 - t3) >> 3;

        src ++;
        dst ++;
    }

    src = block;
    for(i = 0; i < 4; i++){

        A = C7 * src[7] + C1 * src[1];
	    B = C7 * src[1] - C1 * src[7];
	    C = C3 * src[3] + C5 * src[5];
	    D = C3 * src[5] - C5 * src[3];
	    G = C6 * src[6] + C2 * src[2];
	    H = C6 * src[2] - C2 * src[6];

        Ad = 24576*A - 21845*C;
	    Bd = 24576*B - 21845*D;
	    Add = ((Ad - Bd + 16*1024) >>15);
	    Bdd = ((Ad + Bd + 16*1024) >> 15);
	    Cd = A + C;
	    Dd = B + D;

        E = (src[0] + src[4]) *C4 + 64;
		F = (src[0] - src[4]) *C4 + 64;
        Ed = E - G;
		Fd = E + G;
		Gd = F - H;		
		Hd = F + H;

        if(!pRef0 && !pRef1)
        {
            pDest[ 0] = (Fd + Cd) >> 7;
            pDest[1*dststride]= (Hd + Bdd) >> 7;
            pDest[2*dststride] = (Gd + Add) >> 7;
            pDest[3*dststride] = (Ed + Dd) >> 7;
            pDest[4*dststride] = (Ed - Dd + 1) >> 7;
            pDest[5*dststride] = (Gd - Add + 1) >> 7;
            pDest[6*dststride] = (Hd - Bdd + 1) >> 7;
            pDest[7*dststride] = (Fd - Cd + 1) >> 7;
        }
        else if(pRef0 && !pRef1)
        {
            pDest[0] =              SATURATE8((pRef0[0])+((Fd + Cd) >> 7));
            pDest[1*dststride] = SATURATE8((pRef0[1*refstride])+((Hd + Bdd) >> 7));
            pDest[2*dststride] = SATURATE8((pRef0[2*refstride])+((Gd + Add) >> 7));
            pDest[3*dststride] = SATURATE8((pRef0[3*refstride])+((Ed + Dd) >> 7));
            pDest[4*dststride] = SATURATE8((pRef0[4*refstride])+((Ed - Dd + 1) >> 7));
            pDest[5*dststride] = SATURATE8((pRef0[5*refstride])+((Gd - Add + 1) >> 7));
            pDest[6*dststride] = SATURATE8((pRef0[6*refstride])+((Hd - Bdd + 1) >> 7));
            pDest[7*dststride] = SATURATE8((pRef0[7*refstride])+((Fd - Cd + 1) >> 7));
            pRef0++;
        }
        else
        {
            pDest[0] =              SATURATE8(((pRef0[0]+pRef1[0]+1)>>1)+((Fd + Cd) >> 7));
            pDest[1*dststride] = SATURATE8(((pRef0[1*refstride]+pRef1[1*refstride]+1)>>1)+((Hd + Bdd) >> 7));
            pDest[2*dststride] = SATURATE8(((pRef0[2*refstride]+pRef1[2*refstride]+1)>>1)+((Gd + Add) >> 7));
            pDest[3*dststride] = SATURATE8(((pRef0[3*refstride]+pRef1[3*refstride]+1)>>1)+((Ed + Dd) >> 7));
            pDest[4*dststride] = SATURATE8(((pRef0[4*refstride]+pRef1[4*refstride]+1)>>1)+((Ed - Dd + 1) >> 7));
            pDest[5*dststride] = SATURATE8(((pRef0[5*refstride]+pRef1[5*refstride]+1)>>1)+((Gd - Add + 1) >> 7));
            pDest[6*dststride] = SATURATE8(((pRef0[6*refstride]+pRef1[6*refstride]+1)>>1)+((Hd - Bdd + 1) >> 7));
            pDest[7*dststride] = SATURATE8(((pRef0[7*refstride]+pRef1[7*refstride]+1)>>1)+((Fd - Cd + 1) >> 7));
            pRef0++;
            pRef1++;
        }
        src += 8;
        pDest++;         
    }
}

void voVC1InvTrans_4x8_DC_C(unsigned char* pDest, 
						 int dststride, 
						 unsigned char *pRef0,
						 unsigned char *pRef1,
						 int refstride, 
						 short *block,
						 unsigned long *table0,
						 unsigned long *table1)
{
    int i;
    VO_S32 E;

	E = ((17*block[0] + 4)>>3) *12 + 64;
    for(i = 0; i < 4; i++)
	{
        if(!pRef0 && !pRef1)
        {
            pDest[ 0] = (E) >> 7;
            pDest[1*dststride]= (E) >> 7;
            pDest[2*dststride] = (E) >> 7;
            pDest[3*dststride] = (E) >> 7;
            pDest[4*dststride] = (E + 1) >> 7;
            pDest[5*dststride] = (E + 1) >> 7;
            pDest[6*dststride] = (E + 1) >> 7;
            pDest[7*dststride] = (E + 1) >> 7;
        }
        else if(pRef0 && !pRef1)
        {
            pDest[0] =              SATURATE8((pRef0[0])+((E) >> 7));
            pDest[1*dststride] = SATURATE8((pRef0[1*refstride])+((E) >> 7));
            pDest[2*dststride] = SATURATE8((pRef0[2*refstride])+((E) >> 7));
            pDest[3*dststride] = SATURATE8((pRef0[3*refstride])+((E) >> 7));
            pDest[4*dststride] = SATURATE8((pRef0[4*refstride])+((E + 1) >> 7));
            pDest[5*dststride] = SATURATE8((pRef0[5*refstride])+((E + 1) >> 7));
            pDest[6*dststride] = SATURATE8((pRef0[6*refstride])+((E + 1) >> 7));
            pDest[7*dststride] = SATURATE8((pRef0[7*refstride])+((E + 1) >> 7));
            pRef0++;
        }
        else
        {
            pDest[0] =              SATURATE8(((pRef0[0]+pRef1[0]+1)>>1)+((E) >> 7));
            pDest[1*dststride] = SATURATE8(((pRef0[1*refstride]+pRef1[1*refstride]+1)>>1)+((E) >> 7));
            pDest[2*dststride] = SATURATE8(((pRef0[2*refstride]+pRef1[2*refstride]+1)>>1)+((E) >> 7));
            pDest[3*dststride] = SATURATE8(((pRef0[3*refstride]+pRef1[3*refstride]+1)>>1)+((E) >> 7));
            pDest[4*dststride] = SATURATE8(((pRef0[4*refstride]+pRef1[4*refstride]+1)>>1)+((E + 1) >> 7));
            pDest[5*dststride] = SATURATE8(((pRef0[5*refstride]+pRef1[5*refstride]+1)>>1)+((E + 1) >> 7));
            pDest[6*dststride] = SATURATE8(((pRef0[6*refstride]+pRef1[6*refstride]+1)>>1)+((E+ 1) >> 7));
            pDest[7*dststride] = SATURATE8(((pRef0[7*refstride]+pRef1[7*refstride]+1)>>1)+((E + 1) >> 7));
            pRef0++;
            pRef1++;
        }
        pDest++;         
    }
}

void voVC1InvTrans_4x4_C(unsigned char* pDest, 
                                int dststride, 
                                unsigned char *pRef0,
                                unsigned char *pRef1,
                                int refstride, 
                                short *block,
								unsigned long *table0,
								unsigned long *table1)
{
    int i;
    register int t1,t2,t3,t4;
    short *src, *dst;

    src = block;
    dst = block;
    for(i = 0; i < 4; i++){
        t1 = 17 * (src[0*8] + src[2*8]) + 4;
        t2 = 17 * (src[0*8] - src[2*8]) + 4;
        t3 = 22 * src[1*8] + 10 * src[3*8];
        t4 = 22 * src[3*8] - 10 * src[1*8];

        dst[0*8] = (t1 + t3) >> 3;
        dst[1*8] = (t2 - t4) >> 3;
        dst[2*8] = (t2 + t4) >> 3;
        dst[3*8] = (t1 - t3) >> 3;

        src ++;
        dst ++;
    }

    src = block;
    for(i = 0; i < 4; i++){
        t1 = 17 * (src[ 0] + src[2]) + 64;
        t2 = 17 * (src[ 0] - src[2]) + 64;
        t3 = 22 * src[ 1] + 10 * src[3];
        t4 = 22 * src[3] - 10 * src[1];
       

        if(!pRef0 && !pRef1)
        {
            pDest[0*dststride] = ((t1 + t3) >> 7);
            pDest[1*dststride] = ((t2 - t4) >> 7);
            pDest[2*dststride] = ((t2 + t4) >> 7);
            pDest[3*dststride] = ((t1 - t3) >> 7);
        }
        else if(pRef0 && !pRef1)
        {
            pDest[0*dststride] = SATURATE8(pRef0[0*refstride]+((t1 + t3) >> 7));
            pDest[1*dststride] = SATURATE8(pRef0[1*refstride]+((t2 - t4) >> 7));
            pDest[2*dststride] = SATURATE8(pRef0[2*refstride]+((t2 + t4) >> 7));
            pDest[3*dststride] = SATURATE8(pRef0[3*refstride]+((t1 - t3) >> 7));
            pRef0++;
        }
        else
        {
            pDest[0*dststride] = SATURATE8(((pRef0[0*refstride]+pRef1[0*refstride]+1)>>1)+((t1 + t3) >> 7));
            pDest[1*dststride] = SATURATE8(((pRef0[1*refstride]+pRef1[1*refstride]+1)>>1)+((t2 - t4) >> 7));
            pDest[2*dststride] = SATURATE8(((pRef0[2*refstride]+pRef1[2*refstride]+1)>>1)+((t2 + t4) >> 7));
            pDest[3*dststride] = SATURATE8(((pRef0[3*refstride]+pRef1[3*refstride]+1)>>1)+((t1 - t3) >> 7));
            pRef0++;
            pRef1++;
        }        

        src += 8;
        pDest++;        
    }
}
void voVC1InvTrans_4x4_DC_C(unsigned char* pDest, 
                                int dststride, 
                                unsigned char *pRef0,
                                unsigned char *pRef1,
                                int refstride, 
                                short *block,
								unsigned long *table0,
								unsigned long *table1)
{
    int i;
    register int t2;
	
	t2 = (17 * ((17*block[0] + 4)>>3) + 64)>>7;   

    for(i = 0; i < 4; i++)
	{      
        if(!pRef0 && !pRef1)
        {
            pDest[0*dststride] = t2;
            pDest[1*dststride] = t2;
            pDest[2*dststride] = t2;
            pDest[3*dststride] = t2;
        }
        else if(pRef0 && !pRef1)
        {
            pDest[0*dststride] = SATURATE8(pRef0[0*refstride]+t2);
            pDest[1*dststride] = SATURATE8(pRef0[1*refstride]+t2);
            pDest[2*dststride] = SATURATE8(pRef0[2*refstride]+t2);
            pDest[3*dststride] = SATURATE8(pRef0[3*refstride]+t2);
            pRef0++;
        }
        else
        {
            pDest[0*dststride] = SATURATE8(((pRef0[0*refstride]+pRef1[0*refstride]+1)>>1)+t2);
            pDest[1*dststride] = SATURATE8(((pRef0[1*refstride]+pRef1[1*refstride]+1)>>1)+t2);
            pDest[2*dststride] = SATURATE8(((pRef0[2*refstride]+pRef1[2*refstride]+1)>>1)+t2);
            pDest[3*dststride] = SATURATE8(((pRef0[3*refstride]+pRef1[3*refstride]+1)>>1)+t2);
            pRef0++;
            pRef1++;
        }        
        pDest++;        
    }
}

void voVC1Copy_8x8_C(unsigned char* pDst, int dststride, unsigned char *pRef0,unsigned char *pRef1,int refstride)
{
    int j=0;
    if(pRef1)
    {
        for(j=0;j<8;j++)
        {
            pDst[0] = (pRef0[0]+pRef1[0]+1)>>1; 
            pDst[1] = (pRef0[1]+pRef1[1]+1)>>1; 
            pDst[2] = (pRef0[2]+pRef1[2]+1)>>1;
            pDst[3] = (pRef0[3]+pRef1[3]+1)>>1;
            pDst[4] = (pRef0[4]+pRef1[4]+1)>>1; 
            pDst[5] = (pRef0[5]+pRef1[5]+1)>>1; 
            pDst[6] = (pRef0[6]+pRef1[6]+1)>>1; 
            pDst[7] = (pRef0[7]+pRef1[7]+1)>>1;
            pDst += dststride;
            pRef0 += refstride;
            pRef1 += refstride;
        } 
    }
    else
    {
        for(j=0;j<8;j++)
        {
            pDst[0] = pRef0[0]; 
            pDst[1] = pRef0[1]; 
            pDst[2] = pRef0[2];
            pDst[3] = pRef0[3];
            pDst[4] = pRef0[4]; 
            pDst[5] = pRef0[5]; 
            pDst[6] = pRef0[6]; 
            pDst[7] = pRef0[7];
            pDst += dststride;
            pRef0 += refstride;
        }   
    }
}

void voVC1Copy_4x8_C(unsigned char* pDst, int dststride, unsigned char *pRef0,unsigned char *pRef1,int refstride)
{
    int j=0;
    if(pRef1)
    {
        for(j=0;j<8;j++)
        {
            pDst[0] = (pRef0[0]+pRef1[0]+1)>>1; 
            pDst[1] = (pRef0[1]+pRef1[1]+1)>>1;
            pDst[2] = (pRef0[2]+pRef1[2]+1)>>1; 
            pDst[3] = (pRef0[3]+pRef1[3]+1)>>1;
            pDst += dststride;
            pRef0 += refstride;
            pRef1 += refstride;
        } 
    }
    else
    {
        for(j=0;j<8;j++)
        {
            pDst[0] = pRef0[0]; 
            pDst[1] = pRef0[1];
            pDst[2] = pRef0[2]; 
            pDst[3] = pRef0[3];
            pDst += dststride;
            pRef0 += refstride;
        }   
    }
}

void voVC1Copy_8x4_C(unsigned char* pDst, int dststride, unsigned char *pRef0,unsigned char *pRef1,int refstride)
{
    int j;
    if(pRef1)
    {    
        for(j=0;j<4;j++)
        {
                 pDst[0] = (pRef0[0]+pRef1[0]+1)>>1; 
                 pDst[1] = (pRef0[1]+pRef1[1]+1)>>1; 
                 pDst[2] = (pRef0[2]+pRef1[2]+1)>>1;
                 pDst[3] = (pRef0[3]+pRef1[3]+1)>>1;
                 pDst[4] = (pRef0[4]+pRef1[4]+1)>>1; 
                 pDst[5] = (pRef0[5]+pRef1[5]+1)>>1; 
                 pDst[6] = (pRef0[6]+pRef1[6]+1)>>1; 
                 pDst[7] = (pRef0[7]+pRef1[7]+1)>>1;
                 pDst += dststride;
                 pRef0 += refstride;  
                 pRef1 += refstride;
         }
    }
    else
    {
        for(j=0;j<4;j++)
        {
                 pDst[0] = pRef0[0]; 
                 pDst[1] = pRef0[1]; 
                 pDst[2] = pRef0[2];
                 pDst[3] = pRef0[3];
                 pDst[4] = pRef0[4]; 
                 pDst[5] = pRef0[5]; 
                 pDst[6] = pRef0[6]; 
                 pDst[7] = pRef0[7];
                 pDst += dststride;
                 pRef0 += refstride;  
         }
    }
}

void voVC1Copy_4x4_C(unsigned char* pDst, int dststride, unsigned char *pRef0,unsigned char *pRef1,int refstride)
{
    int j=0;
    if(pRef1)
    {
        for(j=0;j<4;j++)
         {
             pDst[0] = (pRef0[0]+pRef1[0] +1)>>1; 
             pDst[1] = (pRef0[1]+pRef1[1] +1)>>1;
             pDst[2] = (pRef0[2]+pRef1[2] +1)>>1;
             pDst[3] = (pRef0[3]+pRef1[3] +1)>>1;
             pDst += dststride;
             pRef0 += refstride;
             pRef1 += refstride;
         }
    }
    else
    {
        for(j=0;j<4;j++)
         {
             pDst[0] = pRef0[0]; 
             pDst[1] = pRef0[1];
             pDst[2] = pRef0[2];
             pDst[3] = pRef0[3];
             pDst += dststride;
             pRef0 += refstride;
         }
    }
}

I32_WMV g_DecodeIDCT8x4_SSIMD(UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags,I32_WMV i)
{
    I32_WMV result =0;
    I32_WMV * piDst0 = piDst->i32 + (i << 3);
    I16_WMV  * blk16;
    const I32_WMV  * piSrc0 = piSrc->i32;  
    blk16  = (I16_WMV *)piDst0;
#ifdef VOARMV7
     ARMV7_g_8x4IDCT(piSrc0,blk16,16,iDCTHorzFlags);
#elif VOARMV6
    {
        I32_WMV tmpBuffer[16];
        ARMV6_g_IDCTDec_WMV3_Pass1_Naked(piSrc0, tmpBuffer, HYBRID_PASS_4, iDCTHorzFlags);
        g_IDCTDec_WMV3_Pass4_ARMV4(tmpBuffer,  blk16, PASS_8);
    }
#else
    {
        I32_WMV tmpBuffer[16];
        g_IDCTDec_WMV3_Pass1(piSrc0, tmpBuffer, HYBRID_PASS_4, iDCTHorzFlags);
        g_IDCTDec_WMV3_Pass4(tmpBuffer,  blk16, PASS_8);
    }
 #endif   
    
    return result;
}

I32_WMV g_DecodeIDCT4x8_SSIMD(UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags,I32_WMV i)
{
    I32_WMV result =0;
    I32_WMV * piDst0 = piDst->i32 + (i);
    I16_WMV * blk16;
    const I32_WMV  * piSrc0 = piSrc->i32;
    blk16  = (I16_WMV *)piDst0;

#ifdef VOARMV7
    ARMV7_g_4x8IDCT(piSrc0,blk16,16,iDCTHorzFlags);
#elif VOARMV6
    {
        I32_WMV tmpBuffer[4*2*4];
        g_IDCTDec_WMV3_Pass3_ARMV4(piSrc0,  tmpBuffer, HYBRID_PASS_8, iDCTHorzFlags);    
        g_IDCTDec_WMV3_Pass2_ARMV4(tmpBuffer, blk16, PASS_4);    
    }
#else
    {
        I32_WMV tmpBuffer[4*2*4];
        g_IDCTDec_WMV3_Pass3(piSrc0,  tmpBuffer, HYBRID_PASS_8, iDCTHorzFlags);    
        g_IDCTDec_WMV3_Pass2(tmpBuffer, blk16, PASS_4);
    }
#endif
    
    return result;
}

I32_WMV g_DecodeIDCT4x4_SSIMD(UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags,I32_WMV i)
{
    I32_WMV result=0;
    I16_WMV  * blk16;
    I16_WMV* piDst0 = piDst->i16 + (i&2)*8 + (i&1)*2;
    I32_WMV  *piSrc0 = (I32_WMV *)piSrc->i32;  
    blk16  = (I16_WMV *)piDst0;

#ifdef VOARMV7
    ARMV7_g_4x4IDCT(piSrc0,blk16,16,iDCTHorzFlags);
#elif VOARMV6
    {
        I32_WMV rgTemp[16];
        g_IDCTDec_WMV3_Pass3_ARMV4(piSrc0,  rgTemp, HYBRID_PASS_4, iDCTHorzFlags);    
        g_IDCTDec_WMV3_Pass4_ARMV4(rgTemp,  piDst0, PASS_4);
    }
#else    
    {
        I32_WMV rgTemp[16];
        g_IDCTDec_WMV3_Pass3(piSrc0,  rgTemp, HYBRID_PASS_4, iDCTHorzFlags);   
        g_IDCTDec_WMV3_Pass4(rgTemp,  piDst0, PASS_4);
    }
#endif
    
    return result;
}