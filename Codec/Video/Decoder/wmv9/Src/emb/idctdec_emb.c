//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) 1996 -- 1998  Microsoft Corporation

Module Name:

    idct.cpp

Abstract:

    Inverse DCT routines

Author:

    Marc Holder (marchold@microsoft.com)    05/98
    Bruce Lin (blin@microsoft.com)          06/98
    Ming-Chieh Lee (mingcl@microsoft.com)   06/98

Revision History:

*************************************************************************/


#include "xplatform_wmv.h"
#include "typedef.h"

#ifndef __DCT_COEF_INT_
#   define __DCT_COEF_INT_

#   define W1 2841L /* 2048*sqrt(2)*cos(1*pi/16) */
#   define W2 2676L /* 2048*sqrt(2)*cos(2*pi/16) */
#   define W3 2408L /* 2048*sqrt(2)*cos(3*pi/16) */
#   define W5 1609L /* 2048*sqrt(2)*cos(5*pi/16) */
#   define W6 1108L /* 2048*sqrt(2)*cos(6*pi/16) */
#   define W7 565L  /* 2048*sqrt(2)*cos(7*pi/16) */

#   define W1a 1892L /* 2048*cos(1*pi/8) */
#   define W2a 1448L /* 2048*cos(2*pi/8) */
#   define W3a 784L  /* 2048*cos(3*pi/8) */

#   define W1_W7 2276L //(W1 - W7)
#   define W1pW7 3406L //(W1 + W7)
#   define W3_W5 799L  //(W3 - W5)
#   define W3pW5 4017L //(W3 + W5)
#   define W2_W6 1568L //(W2 - W6)
#   define W2pW6 3784L //(W2 + W6)

#endif // __DCT_COEF_INT_

#define SATURATE8(x)   (x < 0 ? 0 : (x > 255 ? 255 : x))



#if defined( _EMB_WMV2_) && !defined(_MIPS_ASM_IDCTDEC_OPT_) && !defined(WMV_OPT_IDCT_ARM) && !defined(SH3_DSP_ASM_IDCT)
#if (((defined(OPT_IDCT_SHX)) && (OPT_IDCT_SHX==0)) || (!defined(OPT_IDCT_SHX)))


#ifdef IDCTDEC_32BIT_INPUT
    I32_WMV g_IDCTPass1_WMV2 (I32_WMV __huge* rgiCoefRecon, I32_WMV number, I32_WMV iDCTHorzFlags)
#else
    I32_WMV g_IDCTPass1_WMV2 (I16_WMV __huge* rgiCoefRecon, I32_WMV number, I32_WMV iDCTHorzFlags)
#endif
{
    I32_WMV  i;  
    I32_WMV x0, x1, x2, x3, x4, x5, x6, x7, x8;
    I32_WMV iDCTVertFlag=0;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTPass1_WMV2);

    for (i = 0; i < number; i++,  rgiCoefRecon += BLOCK_SIZE){
        // Check to see if this row has all zero coefficients in columns 1-7 (0 is first column)
        if (!(iDCTHorzFlags & (1 << i))) 
        {
            // Columns 1-7 must be zero. Check to see if column zero coefficient is zero. 
            // If so, we don't have to do anything more for this row
            if (rgiCoefRecon [0] != 0)
            {
                // Column zero cofficient is nonzero so all coefficients in this row are DC
                rgiCoefRecon [0] = rgiCoefRecon [1] = rgiCoefRecon [2] = rgiCoefRecon [3] 
                = rgiCoefRecon [4] = rgiCoefRecon [5] = rgiCoefRecon [6] = rgiCoefRecon [7] = rgiCoefRecon [0] << 3;
                // If this is not the first row, tell the second stage that it can't use DC
                // shortcut - it has to do the full transform for all the columns
                iDCTVertFlag = i;
            }
            continue;
        }

        // If this is not the first row, tell the second stage that it can't use DC shortcut 
        // - it has to do the full transform for all the columns
        iDCTVertFlag = i;

        x4 = rgiCoefRecon [1];      
        x3 = rgiCoefRecon [2];
        x7 = rgiCoefRecon [3];
        x1 = (I32_WMV)rgiCoefRecon [4] << 11;
        x6 = rgiCoefRecon [5];
        x2 = rgiCoefRecon [6];      
        x5 = rgiCoefRecon [7];

        x0 = (I32_WMV)((I32_WMV)rgiCoefRecon [0] << 11) + 128L; /* for proper rounding in the fourth stage */

        // first stage
        x8 = W7 * (x4 + x5);
        x4 = x8 + W1_W7 * x4;
        x5 = x8 - W1pW7 * x5;
        x8 = W3 * (x6 + x7);
        x6 = x8 - W3_W5 * x6;
        x7 = x8 - W3pW5 * x7;

        // second stage 
        x8 = x0 + x1;
        x0 -= x1;
        x1 = W6 * (x3 + x2);
        x2 = x1 - W2pW6 * x2;
        x3 = x1 + W2_W6 * x3;
        x1 = x4 + x6;
        x4 -= x6;
        x6 = x5 + x7;
        x5 -= x7;

        /* third stage */
        x7 = x8 + x3;
        x8 -= x3;
        x3 = x0 + x2;
        x0 -= x2;
        x2 = (I32_WMV)(181L * (x4 + x5) + 128L) >> 8;
        x4 = (I32_WMV)(181L * (x4 - x5) + 128L) >> 8;

        /* fourth stage */
        rgiCoefRecon [0] =  ((x7 + x1) >> 8);
        rgiCoefRecon [1] =  ((x3 + x2) >> 8);
        rgiCoefRecon [2] =  ((x0 + x4) >> 8);
        rgiCoefRecon [3] =  ((x8 + x6) >> 8);
        rgiCoefRecon [4] =  ((x8 - x6) >> 8);
        rgiCoefRecon [5] =  ((x0 - x4) >> 8);
        rgiCoefRecon [6] =  ((x3 - x2) >> 8);
        rgiCoefRecon [7] =  ((x7 - x1) >> 8);
    }

    return iDCTVertFlag;
}


#ifdef IDCTDEC_32BIT_INPUT
Void_WMV g_IDCTPass2_WMV2 (I16_WMV __huge *blk, I32_WMV __huge* rgiCoefRecon, I32_WMV number)
#else
Void_WMV g_IDCTPass2_WMV2 (I16_WMV __huge *blk, I16_WMV __huge* rgiCoefRecon, I32_WMV number)
#endif
{
    I32_WMV x0, x1, x2, x3, x4, x5, x6, x7, x8;
    I32_WMV i;

    I16_WMV __huge *blk0 = blk;
    I16_WMV __huge *blk1 = blk0 + 8;
    I16_WMV __huge *blk2 = blk1 + 8;
    I16_WMV __huge *blk3 = blk2 + 8;
    I16_WMV __huge *blk4 = blk3 + 8;
    I16_WMV __huge *blk5 = blk4 + 8;
    I16_WMV __huge *blk6 = blk5 + 8;
    I16_WMV __huge *blk7 = blk6 + 8;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTPass2_WMV2);

    for ( i = 0; i < number; i++){
 
        int idex;

        x0 = (I32_WMV)((I32_WMV)rgiCoefRecon[i] << 8) + 8192L;
        x1 = (I32_WMV)rgiCoefRecon[i+(4*number)] << 8;
        x2 = rgiCoefRecon[i+(6*number)];
        x3 = rgiCoefRecon[i+(2*number)];
        x4 = rgiCoefRecon[i+(1*number)];
        x5 = rgiCoefRecon[i+(7*number)];
        x6 = rgiCoefRecon[i+(5*number)];
        x7 = rgiCoefRecon[i+(3*number)];

        // first stage 
        x8 = W7 * (x4 + x5) + 4;
        x4 = (x8 + W1_W7 * x4) >> 3;
        x5 = (x8 - W1pW7 * x5) >> 3;
        x8 = W3 * (x6 + x7) + 4;
        x6 = (x8 - W3_W5 * x6) >> 3;
        x7 = (x8 - W3pW5 * x7) >> 3;

        // second stage 
        x8 = x0 + x1;
        x0 -= x1;
        x1 = W6 * (x3 + x2) + 4;
        x2 = (x1 - W2pW6 * x2) >> 3;
        x3 = (x1 + W2_W6 * x3) >> 3;
        x1 = x4 + x6;
        x4 -= x6;
        x6 = x5 + x7;
        x5 -= x7;

        /* third stage */
        x7 = x8 + x3;
        x8 -= x3;
        x3 = x0 + x2;
        x0 -= x2;
        x2 = (I32_WMV) (181L * (x4 + x5) + 128L) >> 8;
        x4 = (I32_WMV) (181L * (x4 - x5) + 128L) >> 8;


                /* fourth stage */

        idex=(i>>1)+((i&1)<<6);

        blk0[idex] =  ((x7 + x1) >> 14);
        blk1[idex] =  ((x3 + x2) >> 14);
        blk2[idex] =  ((x0 + x4) >> 14);
        blk3[idex] =  ((x8 + x6) >> 14);
        blk4[idex] =  ((x8 - x6) >> 14);
        blk5[idex] =  ((x0 - x4) >> 14);
        blk6[idex] =  ((x3 - x2) >> 14);
        blk7[idex] =  ((x7 - x1) >> 14); 



    }

}



// C Version
// Input:  32-bit
// Output: 32-bit
Void_WMV g_IDCTDec_WMV2_16bit (UnionBuffer __huge * piDst, UnionBuffer __huge * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags)
{
    // I32_WMV x0, x1, x2, x3, x4, x5, x6, x7, x8;
#ifdef IDCTDEC_32BIT_INPUT
    I32_WMV __huge* rgiCoefRecon = (I32_WMV __huge* )((UnionBuffer __huge * )piSrc)->i32;
#else
    I16_WMV __huge* rgiCoefRecon = (I16_WMV __huge* )((UnionBuffer __huge * )piSrc)->i16;
#endif
    I16_WMV __huge* blk = piDst->i16;
    register I32_WMV iDCTVertFlag;
    I32_WMV  i;  
#   if !defined(_SH4_)
    FUNCTION_PROFILE_DECL_START(fpDecode,IDCT_PROFILE);
#   endif
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV2_16bit);

    iDCTVertFlag=g_IDCTPass1_WMV2 (rgiCoefRecon, 8, iDCTHorzFlags);

    if (iDCTVertFlag == 0)
    {
        I16_WMV __huge *blk0 = piDst->i16;
        I16_WMV __huge *blk1 = blk0 + iOffsetToNextRowForDCT;
        I16_WMV __huge *blk2 = blk1 + iOffsetToNextRowForDCT;
        I16_WMV __huge *blk3 = blk2 + iOffsetToNextRowForDCT;
        I16_WMV __huge *blk4 = blk3 + iOffsetToNextRowForDCT;
        I16_WMV __huge *blk5 = blk4 + iOffsetToNextRowForDCT;
        I16_WMV __huge *blk6 = blk5 + iOffsetToNextRowForDCT;
        I16_WMV __huge *blk7 = blk6 + iOffsetToNextRowForDCT;
        // If iDCTVertFlag is zero then we know that blk[1] thru blk[7] are zero.
        // Therefore, if blk[0] is zero we can skip to the next column. Otherwise we
        // can use the DC shortcut and just copy blk[0] to the rest of the column.
        for(i=0;i<BLOCK_SIZE;i++)
        {

            int idex=(i>>1)+((i&1)<<6);
            blk0[idex] = blk1[idex] = blk2[idex] = blk3[idex] = blk4[idex] = blk5[idex] = blk6[idex] = blk7[idex] = (rgiCoefRecon[i] + 32) >> 6;

        }
#   if !defined(_SH4_)
        FUNCTION_PROFILE_STOP(&fpDecode);
#   endif

        return;
    }


    g_IDCTPass2_WMV2 (piDst->i16, rgiCoefRecon, 8);

#   if !defined(_SH4_)
        FUNCTION_PROFILE_STOP(&fpDecode);
#   endif

}


// C Version
// Input:  32-bit
// Output: 32-bit
Void_WMV g_8x4IDCTDec_WMV2_16bit (UnionBuffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iHalfAndiDCTHorzFlags)
{
#ifdef IDCTDEC_32BIT_INPUT
    I32_WMV __huge* rgiCoefRecon = (I32_WMV __huge* )rgiCoefReconBuf->i32;
#else
    I16_WMV __huge* rgiCoefRecon = (I16_WMV __huge* )rgiCoefReconBuf->i16;
#endif
    I32_WMV iDCTHorzFlags=iHalfAndiDCTHorzFlags&0x7fffffff;
    I32_WMV iHalf=((U32_WMV)iHalfAndiDCTHorzFlags)>>31;

    I16_WMV __huge* piDst = piDstBuf->i16 + (iHalf*32);
    I16_WMV * blk = piDst;

    I32_WMV x0, x1, x2, x3, x4, x5, x6, x7;
    I32_WMV i;
    I32_WMV iDCTVertFlag;
    I16_WMV* blk0 = piDst;
    I16_WMV* blk1 = blk0 + iOffsetToNextRowForDCT;
    I16_WMV* blk2 = blk1 + iOffsetToNextRowForDCT;
    I16_WMV* blk3 = blk2 + iOffsetToNextRowForDCT;

#   if !defined(_SH4_)
    FUNCTION_PROFILE_DECL_START(fpDecode,IDCT_PROFILE);
#   endif
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_8x4IDCTDec_WMV2_16bit);
  
    iDCTVertFlag=g_IDCTPass1_WMV2 (rgiCoefRecon, 4, iDCTHorzFlags);

    if (iDCTVertFlag == 0)
    {
        // If iDCTVertFlag is zero then we know that blk[1] thru blk[7] are zero.
        // Therefore, if blk[0] is zero we can skip to the next column. Otherwise we
        // can use the DC shortcut and just copy blk[0] to the rest of the column.
        for(i=0;i<BLOCK_SIZE;i++)
        {

            int idex=(i>>1)+((i&1)<<6);
            //blk0[idex] = blk1[idex] = blk2[idex] = blk3[idex]  = (rgiCoefRecon[i] + 32) >> 6;
            blk0[idex] = blk1[idex] = blk2[idex] = blk3[idex]  = (I16_WMV)( (rgiCoefRecon[i]*W2a + 32768L) >> 16 );
        }
#   if !defined(_SH4_)
        FUNCTION_PROFILE_STOP(&fpDecode);
#   endif

        return;
    }

    for (i = 0; i < BLOCK_SIZE; i++)
    {
        int idex;

        x4 = rgiCoefRecon[i+(0*8)];
        x5 = rgiCoefRecon[i+(1*8)];
        x6 = rgiCoefRecon[i+(2*8)];
        x7 = rgiCoefRecon[i+(3*8)];

        x0 = (x4 + x6)*W2a;
        x1 = (x4 - x6)*W2a;
        x2 = x5*W1a + x7*W3a;
        x3 = x5*W3a - x7*W1a;



       idex=(i>>1)+((i&1)<<6);

        blk0[idex]    = (I16_WMV)((x0 + x2 + 32768L)>>16);
        blk1[idex]    = (I16_WMV)((x1 + x3 + 32768L)>>16);
        blk2[idex]    = (I16_WMV)((x1 - x3 + 32768L)>>16);
        blk3[idex]    = (I16_WMV)((x0 - x2 + 32768L)>>16);


    } 
#   if !defined(_SH4_)
        FUNCTION_PROFILE_STOP(&fpDecode);
#   endif
}


// C Version
// Input:  32-bit
// Output: 32-bit
Void_WMV g_4x8IDCTDec_WMV2_16bit (UnionBuffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iHalfAndiDCTHorzFlags)
{
#   ifdef IDCTDEC_32BIT_INPUT
        I32_WMV __huge* rgiCoefRecon = (I32_WMV __huge*)rgiCoefReconBuf->i32;
#   else
        I16_WMV __huge* rgiCoefRecon = (I16_WMV __huge*)rgiCoefReconBuf->i16;
#   endif

    I32_WMV iHalf=((U32_WMV)iHalfAndiDCTHorzFlags)>>31;
    I16_WMV __huge* piDst = piDstBuf->i16 + (iHalf*2);

    I32_WMV x0, x1, x2, x3, x4, x5, x6, x7; 
    I32_WMV i;
    FUNCTION_PROFILE_DECL_START(fpDecode,IDCT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_4x8IDCTDec_WMV2_16bit);

    for (i = 0; i < BLOCK_SIZE; i++,  rgiCoefRecon += 4) {

        x4 = rgiCoefRecon[0];
        x5 = rgiCoefRecon[1];
        x6 = rgiCoefRecon[2];
        x7 = rgiCoefRecon[3];

        x0 = (x4 + x6)*W2a;
        x1 = (x4 - x6)*W2a;
        x2 = x5*W1a + x7*W3a;
        x3 = x5*W3a - x7*W1a;

        rgiCoefRecon[0] = ((x0 + x2 + 64)>>7);
        rgiCoefRecon[1] = ((x1 + x3 + 64)>>7);
        rgiCoefRecon[2] = ((x1 - x3 + 64)>>7);
        rgiCoefRecon[3] = ((x0 - x2 + 64)>>7);

    }

    rgiCoefRecon-=32;

    g_IDCTPass2_WMV2 (piDst, rgiCoefRecon, 4);

    FUNCTION_PROFILE_STOP(&fpDecode);
}

#endif
#endif


