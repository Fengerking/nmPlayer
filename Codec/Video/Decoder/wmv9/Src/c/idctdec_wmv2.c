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
#include "idctdec_wmv2.h"
#include "wmvdec_member.h"

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

#define _USE_C_IDCT_


///////////////////////////////////////////////////////////////////////////
//
// Intra IDCT Functions
//
///////////////////////////////////////////////////////////////////////////

#if !( defined(WMV_OPT_IDCT_ARM) || ( defined(OPT_IDCT_SHX) && (OPT_IDCT_SHX>=3)) || ( defined(SH3_DSP_ASM_IDCT)))

// C Version
// Input: 32-bit
// Output: 8-bit
Void_WMV g_IDCTDec_WMV2_Intra (U8_WMV* piDst, I32_WMV iOffsetToNextRowForDCT, const I32_WMV __huge* rgiCoefRecon)
{
    I32_WMV x0, x1, x2, x3, x4, x5, x6, x7, x8;
    I32_WMV rgblk [64];
    I32_WMV* blk = rgblk;
    I32_WMV iTest;
    I32_WMV  i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV2_Intra);

    COVERAGE_COEF_INPUT_BITS (BLOCK_SIZE*BLOCK_SIZE, rgiCoefRecon);
    CLIPCOEF32 (12, BLOCK_SIZE*BLOCK_SIZE, (I32_WMV __huge*)rgiCoefRecon);

    for (i = 0; i < BLOCK_SIZE; i++, blk += BLOCK_SIZE, rgiCoefRecon += BLOCK_SIZE) {
		x4 = rgiCoefRecon [1];
		x3 = rgiCoefRecon [2];
		x7 = rgiCoefRecon [3];
		x1 = (I32_WMV)rgiCoefRecon [4] << 11 ;
		x6 = rgiCoefRecon [5];
		x2 = rgiCoefRecon [6];
		x5 = rgiCoefRecon [7];
		x0 = ((I32_WMV)rgiCoefRecon [0] << 11) + 128L; /* for proper rounding in the fourth stage */
		
		COVERAGE_COEF_INTERNAL_BITS (x0);
		COVERAGE_COEF_INTERNAL_BITS (x1);
		COVERAGE_COEF_INTERNAL_BITS (x2);
		COVERAGE_COEF_INTERNAL_BITS (x3);
		COVERAGE_COEF_INTERNAL_BITS (x4);
		COVERAGE_COEF_INTERNAL_BITS (x5);
		COVERAGE_COEF_INTERNAL_BITS (x6);
		COVERAGE_COEF_INTERNAL_BITS (x7);
		
		/* first stage */
		x8 = W7 * (x4 + x5);
		x4 = x8 + (W1 - W7) * x4;
		x5 = x8 - (W1 + W7) * x5;
		
		COVERAGE_COEF_INTERNAL_BITS (x8);
		COVERAGE_COEF_INTERNAL_BITS (x4);
		COVERAGE_COEF_INTERNAL_BITS (x5);
		
		x8 = W3 * (x6 + x7);
		x6 = x8 - (W3 - W5) * x6;
		x7 = x8 - (W3 + W5) * x7;

		COVERAGE_COEF_INTERNAL_BITS (x8);
		COVERAGE_COEF_INTERNAL_BITS (x6);
		COVERAGE_COEF_INTERNAL_BITS (x7);

		/* second stage */
		x8 = x0 + x1;
		x0 -= x1;
		x1 = W6 * (x3 + x2);
		x2 = x1 - (W2 + W6) * x2;
		x3 = x1 + (W2 - W6) * x3;

		COVERAGE_COEF_INTERNAL_BITS (x8);
		COVERAGE_COEF_INTERNAL_BITS (x0);
		COVERAGE_COEF_INTERNAL_BITS (x1);
		COVERAGE_COEF_INTERNAL_BITS (x2);
		COVERAGE_COEF_INTERNAL_BITS (x3);
		
		
		x1 = x4 + x6;
		x4 -= x6;
		x6 = x5 + x7;
		x5 -= x7;

		COVERAGE_COEF_INTERNAL_BITS (x1);
		COVERAGE_COEF_INTERNAL_BITS (x4);
		COVERAGE_COEF_INTERNAL_BITS (x6);
		COVERAGE_COEF_INTERNAL_BITS (x5);

		/* third stage */
		x7 = x8 + x3;
		x8 -= x3;
		x3 = x0 + x2;
		x0 -= x2;
		x2 = (I32_WMV) (181L * (x4 + x5) + 128L) >> 8;
		x4 = (I32_WMV) (181L * (x4 - x5) + 128L) >> 8;

		COVERAGE_COEF_INTERNAL_BITS (x7);
		COVERAGE_COEF_INTERNAL_BITS (x8);
		COVERAGE_COEF_INTERNAL_BITS (x3);
		COVERAGE_COEF_INTERNAL_BITS (x0);
		COVERAGE_COEF_INTERNAL_BITS (x2);
		COVERAGE_COEF_INTERNAL_BITS (x4);

		/* fourth stage */
		blk [0] = (I32_WMV) ((x7 + x1) >> 8);
		blk [1] = (I32_WMV) ((x3 + x2) >> 8);
		blk [2] = (I32_WMV) ((x0 + x4) >> 8);
		blk [3] = (I32_WMV) ((x8 + x6) >> 8);
		blk [4] = (I32_WMV) ((x8 - x6) >> 8);
		blk [5] = (I32_WMV) ((x0 - x4) >> 8);
		blk [6] = (I32_WMV) ((x3 - x2) >> 8);
		blk [7] = (I32_WMV) ((x7 - x1) >> 8);
	
		COVERAGE_COEF_INTERNAL_BITS (blk [0]);
		COVERAGE_COEF_INTERNAL_BITS (blk [1]);
		COVERAGE_COEF_INTERNAL_BITS (blk [2]);
		COVERAGE_COEF_INTERNAL_BITS (blk [3]);
		COVERAGE_COEF_INTERNAL_BITS (blk [4]);
		COVERAGE_COEF_INTERNAL_BITS (blk [5]);
		COVERAGE_COEF_INTERNAL_BITS (blk [6]);
		COVERAGE_COEF_INTERNAL_BITS (blk [7]);
	
	}

    {
        U8_WMV * blk0 = piDst;
        U8_WMV * blk1 = blk0 + iOffsetToNextRowForDCT;
        U8_WMV * blk2 = blk1 + iOffsetToNextRowForDCT;
        U8_WMV * blk3 = blk2 + iOffsetToNextRowForDCT;
        U8_WMV * blk4 = blk3 + iOffsetToNextRowForDCT;
        U8_WMV * blk5 = blk4 + iOffsetToNextRowForDCT;
        U8_WMV * blk6 = blk5 + iOffsetToNextRowForDCT;
        U8_WMV * blk7 = blk6 + iOffsetToNextRowForDCT;
        
        I32_WMV* pBlk0 = rgblk;
        I32_WMV* pBlk1 = pBlk0 + BLOCK_SIZE;
        I32_WMV* pBlk2 = pBlk1 + BLOCK_SIZE;
        I32_WMV* pBlk3 = pBlk2 + BLOCK_SIZE;
        I32_WMV* pBlk4 = pBlk3 + BLOCK_SIZE;
        I32_WMV* pBlk5 = pBlk4 + BLOCK_SIZE;
        I32_WMV* pBlk6 = pBlk5 + BLOCK_SIZE;
        I32_WMV* pBlk7 = pBlk6 + BLOCK_SIZE;
        
        for (i = 0; i < BLOCK_SIZE; i++) {
            x0 = (I32_WMV) ((I32_WMV)*pBlk0++ << 8) + 8192L;
            x1 = (I32_WMV) *pBlk4++ << 8;
            x2 = *pBlk6++;
            x3 = *pBlk2++;
            x4 = *pBlk1++;
            x5 = *pBlk7++;
            x6 = *pBlk5++;
            x7 = *pBlk3++;

            COVERAGE_COEF_INTERNAL_BITS (x0);
            COVERAGE_COEF_INTERNAL_BITS (x1);
            COVERAGE_COEF_INTERNAL_BITS (x2);
            COVERAGE_COEF_INTERNAL_BITS (x3);
            COVERAGE_COEF_INTERNAL_BITS (x4);
            COVERAGE_COEF_INTERNAL_BITS (x5);
            COVERAGE_COEF_INTERNAL_BITS (x6);
            COVERAGE_COEF_INTERNAL_BITS (x7);
            
            /* first stage */
            x8 = W7 * (x4 + x5) + 4;
            x4 = (x8 + (W1 - W7) * x4) >> 3;
            x5 = (x8 - (W1 + W7) * x5) >> 3;

            COVERAGE_COEF_INTERNAL_BITS (x8);
            COVERAGE_COEF_INTERNAL_BITS (x4);
            COVERAGE_COEF_INTERNAL_BITS (x5);
            
            x8 = W3 * (x6 + x7) + 4;
            x6 = (x8 - (W3 - W5) * x6) >> 3;
            x7 = (x8 - (W3 + W5) * x7) >> 3;

            COVERAGE_COEF_INTERNAL_BITS (x8);
            COVERAGE_COEF_INTERNAL_BITS (x6);
            COVERAGE_COEF_INTERNAL_BITS (x7);
            
            /* second stage */
            x8 = x0 + x1;
            x0 -= x1;
            x1 = W6 * (x3 + x2) + 4;
            x2 = (x1 - (W2 + W6) * x2) >> 3;
            x3 = (x1 + (W2 - W6) * x3) >> 3;
            
            COVERAGE_COEF_INTERNAL_BITS (x8);
            COVERAGE_COEF_INTERNAL_BITS (x0);
            COVERAGE_COEF_INTERNAL_BITS (x1);
            COVERAGE_COEF_INTERNAL_BITS (x2);
            COVERAGE_COEF_INTERNAL_BITS (x3);
            
            x1 = x4 + x6;
            x4 -= x6;
            x6 = x5 + x7;
            x5 -= x7;
            
            COVERAGE_COEF_INTERNAL_BITS (x1);
            COVERAGE_COEF_INTERNAL_BITS (x4);
            COVERAGE_COEF_INTERNAL_BITS (x6);
            COVERAGE_COEF_INTERNAL_BITS (x5);
            
            /* third stage */
            x7 = x8 + x3;
            x8 -= x3;
            x3 = x0 + x2;
            x0 -= x2;
            x2 = (181 * (x4 + x5) + 128) >> 8;
            x4 = (181 * (x4 - x5) + 128) >> 8;
            
            COVERAGE_COEF_INTERNAL_BITS (x7);
            COVERAGE_COEF_INTERNAL_BITS (x8);
            COVERAGE_COEF_INTERNAL_BITS (x3);
            COVERAGE_COEF_INTERNAL_BITS (x0);
            COVERAGE_COEF_INTERNAL_BITS (x2);
            COVERAGE_COEF_INTERNAL_BITS (x4);
            
            /* fourth stage */
            x5 = (x7 + x1) >> 14;
            x1 = (x7 - x1) >> 14;
            iTest = x1 | x5;
            x7 = (x3 + x2) >> 14;
            iTest |= x7;
            x2 = (x3 - x2) >> 14;
            iTest |= x2;
            x3 = (x0 + x4) >> 14;
            iTest |= x3;
            x4 = (x0 - x4) >> 14;
            iTest |= x4;
            x0 = (x8 + x6) >> 14;
            iTest |= x0;
            x6 = (x8 - x6) >> 14;
            iTest |= x6;

            COVERAGE_COEF_INTERNAL_BITS (x5);
            COVERAGE_COEF_INTERNAL_BITS (x1);
            COVERAGE_COEF_INTERNAL_BITS (x7);
            COVERAGE_COEF_INTERNAL_BITS (x2);
            COVERAGE_COEF_INTERNAL_BITS (x3);
            COVERAGE_COEF_INTERNAL_BITS (x4);
            COVERAGE_COEF_INTERNAL_BITS (x0);
            COVERAGE_COEF_INTERNAL_BITS (x6);
            
            if (iTest & ~255L) {
                x0 = SATURATE8(x0);
                x1 = SATURATE8(x1);
                x2 = SATURATE8(x2);
                x3 = SATURATE8(x3);
                x4 = SATURATE8(x4);
                x5 = SATURATE8(x5);
                x6 = SATURATE8(x6);
                x7 = SATURATE8(x7);
            }
            
            *blk0++ = (U8_WMV) x5;
            *blk1++ = (U8_WMV) x7;
            *blk2++ = (U8_WMV) x3;
            *blk3++ = (U8_WMV) x0;
            *blk4++ = (U8_WMV) x6;
            *blk5++ = (U8_WMV) x4;
            *blk6++ = (U8_WMV) x2;
            *blk7++ = (U8_WMV) x1;
        }
    }
}

#endif //!( defined(WMV_OPT_IDCT_ARM) || ( defined(OPT_IDCT_SHX) && (OPT_IDCT_SHX>=3)))

#if !( defined(WMV_OPT_IDCT_ARM) || (defined(OPT_IDCT_SHX) && (OPT_IDCT_SHX>=3)) || defined(SH3_DSP_ASM_IDCT))

///////////////////////////////////////////////////////////////////////////
//
// Inter IDCT Functions
//
///////////////////////////////////////////////////////////////////////////
// C Version
// Input:  16-bit
// Output: 16-bit
Void_WMV g_IDCTDec16_WMV2 (UnionBuffer* piDst, UnionBuffer* piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags)
{
	I32_WMV x0, x1, x2, x3, x4, x5, x6, x7, x8;
    const I16_WMV __huge* rgiCoefRecon = piSrc->i16;
	I16_WMV __huge* blk = piDst->i16;
    register I32_WMV iDCTVertFlag = 0;
    I32_WMV  i;  

	FUNCTION_PROFILE_DECL_START(fpDecode,IDCT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec16_WMV2);
    
    CLIPCOEF16 (12, 64, piSrc->i16);

    iDCTHorzFlags = -1;

    for (i = 0; i < BLOCK_SIZE; i++, blk += iOffsetToNextRowForDCT, rgiCoefRecon += BLOCK_SIZE) {
        // Check to see if this row has all zero coefficients in columns 1-7 (0 is first column)
        if (!(iDCTHorzFlags & (1 << i))) {
            // Columns 1-7 must be zero. Check to see if column zero coefficient is zero. 
            // If so, we don't have to do anything more for this row
            if (rgiCoefRecon [0] != 0) {
                // Column zero cofficient is nonzero so all coefficients in this row are DC
                blk [0] = blk [1] = blk [2] = blk [3] = blk [4] = blk [5] = blk [6] = blk [7] = rgiCoefRecon [0] << 3;
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
		blk [0] = (I16_WMV) ((x7 + x1) >> 8);
		blk [1] = (I16_WMV) ((x3 + x2) >> 8);
		blk [2] = (I16_WMV) ((x0 + x4) >> 8);
		blk [3] = (I16_WMV) ((x8 + x6) >> 8);
		blk [4] = (I16_WMV) ((x8 - x6) >> 8);
		blk [5] = (I16_WMV) ((x0 - x4) >> 8);
		blk [6] = (I16_WMV) ((x3 - x2) >> 8);
		blk [7] = (I16_WMV) ((x7 - x1) >> 8);
	}

    {
        
        I16_WMV __huge *blk0 = piDst->i16;
        
        I16_WMV __huge *blk1 = blk0 + iOffsetToNextRowForDCT;
        I16_WMV __huge *blk2 = blk1 + iOffsetToNextRowForDCT;
        I16_WMV __huge *blk3 = blk2 + iOffsetToNextRowForDCT;
        I16_WMV __huge *blk4 = blk3 + iOffsetToNextRowForDCT;
        I16_WMV __huge *blk5 = blk4 + iOffsetToNextRowForDCT;
        I16_WMV __huge *blk6 = blk5 + iOffsetToNextRowForDCT;
        I16_WMV __huge *blk7 = blk6 + iOffsetToNextRowForDCT;
        
        for (i = 0; i < BLOCK_SIZE; i++) {
            // If iDCTVertFlag is zero then we know that blk[1] thru blk[7] are zero.
            // Therefore, if blk[0] is zero we can skip to the next column. Otherwise we
            // can use the DC shortcut and just copy blk[0] to the rest of the column.
            if (iDCTVertFlag == 0) {
                // If blk[0] is nonzero, copy the value to the other 7 rows in this column		
                if (blk0[i] != 0)
                    blk0[i] = blk1[i] = blk2[i] = blk3[i] = blk4[i] = blk5[i] = blk6[i] = blk7[i] = (blk0[i] + 32) >> 6;
                continue;
            }
            
            x0 = (I32_WMV)((I32_WMV)blk0[i] << 8) + 8192L;
            x1 = (I32_WMV)blk4[i] << 8;
            x2 = blk6[i];
            x3 = blk2[i];
            x4 = blk1[i];
            x5 = blk7[i];
            x6 = blk5[i];
            x7 = blk3[i];
            
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
            blk0[i] = (I16_WMV) ((x7 + x1) >> 14);
            blk1[i] = (I16_WMV) ((x3 + x2) >> 14);
            blk2[i] = (I16_WMV) ((x0 + x4) >> 14);
            blk3[i] = (I16_WMV) ((x8 + x6) >> 14);
            blk4[i] = (I16_WMV) ((x8 - x6) >> 14);
            blk5[i] = (I16_WMV) ((x0 - x4) >> 14);
            blk6[i] = (I16_WMV) ((x3 - x2) >> 14);
            blk7[i] = (I16_WMV) ((x7 - x1) >> 14); 
        }
    }
    FUNCTION_PROFILE_STOP(&fpDecode);

}
#endif // !( defined(WMV_OPT_IDCT_ARM) || ( defined(OPT_IDCT_SHX) && (OPT_IDCT_SHX>=3)) )


#ifdef _USE_C_IDCT_
// C Version
// Input:  32-bit
// Output: 32-bit
Void_WMV g_IDCTDec_WMV2_Inter (UnionBuffer* piDst, UnionBuffer* piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags)
{
	I32_WMV x0, x1, x2, x3, x4, x5, x6, x7, x8;
	const I32_WMV __huge* rgiCoefRecon = piSrc->i32;
	I32_WMV __huge* blk = piDst->i32;
    register I32_WMV iDCTVertFlag = 0;
    I32_WMV  i;  

	FUNCTION_PROFILE_DECL_START(fpDecode,IDCT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_IDCTDec_WMV2_Inter);
    
    CLIPCOEF32 (12, 64, piSrc->i32);

    iDCTHorzFlags = -1;

    for (i = 0; i < BLOCK_SIZE; i++, blk += iOffsetToNextRowForDCT, rgiCoefRecon += BLOCK_SIZE) {
        // Check to see if this row has all zero coefficients in columns 1-7 (0 is first column)
        if (!(iDCTHorzFlags & (1 << i))) {
            // Columns 1-7 must be zero. Check to see if column zero coefficient is zero. 
            // If so, we don't have to do anything more for this row
            if (rgiCoefRecon [0] != 0)
			{
                // Column zero cofficient is nonzero so all coefficients in this row are DC
                blk [0] = blk [1] = blk [2] = blk [3] = blk [4] = blk [5] = blk [6] = blk [7] = rgiCoefRecon [0] << 3;
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
		blk [0] =  ((x7 + x1) >> 8);
		blk [1] =  ((x3 + x2) >> 8);
		blk [2] =  ((x0 + x4) >> 8);
		blk [3] =  ((x8 + x6) >> 8);
		blk [4] =  ((x8 - x6) >> 8);
		blk [5] =  ((x0 - x4) >> 8);
		blk [6] =  ((x3 - x2) >> 8);
		blk [7] =  ((x7 - x1) >> 8);
	}


    {
        I32_WMV __huge *blk0 = piDst->i32;
        I32_WMV __huge *blk1 = blk0 + iOffsetToNextRowForDCT;
        I32_WMV __huge *blk2 = blk1 + iOffsetToNextRowForDCT;
        I32_WMV __huge *blk3 = blk2 + iOffsetToNextRowForDCT;
        I32_WMV __huge *blk4 = blk3 + iOffsetToNextRowForDCT;
        I32_WMV __huge *blk5 = blk4 + iOffsetToNextRowForDCT;
        I32_WMV __huge *blk6 = blk5 + iOffsetToNextRowForDCT;
        I32_WMV __huge *blk7 = blk6 + iOffsetToNextRowForDCT;
        
        
        
        for (i = 0; i < BLOCK_SIZE; i++) {
            // If iDCTVertFlag is zero then we know that blk[1] thru blk[7] are zero.
            // Therefore, if blk[0] is zero we can skip to the next column. Otherwise we
            // can use the DC shortcut and just copy blk[0] to the rest of the column.
            if (iDCTVertFlag == 0) {
                // If blk[0] is nonzero, copy the value to the other 7 rows in this column		
                if (blk0[i] != 0) {
                    blk0[i] = blk1[i] = blk2[i] = blk3[i] = blk4[i] = blk5[i] = blk6[i] = blk7[i] = (blk0[i] + 32) >> 6;
                }
                continue;
            }
            
            x0 = (I32_WMV)((I32_WMV)blk0[i] << 8) + 8192L;
            x1 = (I32_WMV)blk4[i] << 8;
            x2 = blk6[i];
            x3 = blk2[i];
            x4 = blk1[i];
            x5 = blk7[i];
            x6 = blk5[i];
            x7 = blk3[i];
            
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
            blk0[i] =  ((x7 + x1) >> 14);
            blk1[i] =  ((x3 + x2) >> 14);
            blk2[i] =  ((x0 + x4) >> 14);
            blk3[i] =  ((x8 + x6) >> 14);
            blk4[i] =  ((x8 - x6) >> 14);
            blk5[i] =  ((x0 - x4) >> 14);
            blk6[i] =  ((x3 - x2) >> 14);
            blk7[i] =  ((x7 - x1) >> 14); 
            
        }
    }
    FUNCTION_PROFILE_STOP(&fpDecode);

}
#endif



//#ifndef _EMB_WMV2_
// C Version
// Input:  32-bit
// Output: 32-bit
Void_WMV g_8x4IDCTDec_WMV2 (UnionBuffer* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer* rgiCoefReconBuf, I32_WMV iHalf)
{
    const I32_WMV __huge* rgiCoefRecon = rgiCoefReconBuf->i32;
	I32_WMV __huge* piDst = piDstBuf->i32 + (iHalf*32);
	I32_WMV * blk = piDst;

	I32_WMV x0, x1, x2, x3, x4, x5, x6, x7, x8;
	I32_WMV i;

#   if !defined(_SH4_)
	FUNCTION_PROFILE_DECL_START(fpDecode,IDCT_PROFILE);
#	endif
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_8x4IDCTDec_WMV2);
    
    CLIPCOEF32 (12, 64, (I32_WMV __huge*)rgiCoefReconBuf->i32);

    for (i = 0; i < 4; i++, blk += iOffsetToNextRowForDCT, rgiCoefRecon += BLOCK_SIZE) {
        x4 = rgiCoefRecon [1];		
        x3 = rgiCoefRecon [2];
	    x7 = rgiCoefRecon [3];
	    x1 = (I32_WMV)rgiCoefRecon [4] << 11;
	    x6 = rgiCoefRecon [5];
	    x2 = rgiCoefRecon [6];		
	    x5 = rgiCoefRecon [7];

        *blk = rgiCoefRecon [0];

		x0 = (I32_WMV)((I32_WMV)blk [0] << 11) + 128L; /* for proper rounding in the fourth stage */
		/* first stage */
		x8 = W7 * (x4 + x5);
		x4 = x8 + (W1 - W7) * x4;
		x5 = x8 - (W1 + W7) * x5;
		x8 = W3 * (x6 + x7);
		x6 = x8 - (W3 - W5) * x6;
		x7 = x8 - (W3 + W5) * x7;

		/* second stage */
		x8 = x0 + x1;
		x0 -= x1;
		x1 = W6 * (x3 + x2);
		x2 = x1 - (W2 + W6) * x2;
		x3 = x1 + (W2 - W6) * x3;
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
		blk [0] =  ((x7 + x1) >> 8);
		blk [1] =  ((x3 + x2) >> 8);
		blk [2] =  ((x0 + x4) >> 8);
		blk [3] =  ((x8 + x6) >> 8);
		blk [4] =  ((x8 - x6) >> 8);
		blk [5] =  ((x0 - x4) >> 8);
		blk [6] =  ((x3 - x2) >> 8);
		blk [7] =  ((x7 - x1) >> 8);
    }

    {
        I32_WMV* blk0 = piDst;
        I32_WMV* blk1 = blk0 + iOffsetToNextRowForDCT;
        I32_WMV* blk2 = blk1 + iOffsetToNextRowForDCT;
        I32_WMV* blk3 = blk2 + iOffsetToNextRowForDCT;
        
        for (i = 0; i < BLOCK_SIZE; i++) {
            x4 = blk0[i];
            x5 = blk1[i];
            x6 = blk2[i];
            x7 = blk3[i];
            
            x0 = (x4 + x6)*W2a;
            x1 = (x4 - x6)*W2a;
            x2 = x5*W1a + x7*W3a;
            x3 = x5*W3a - x7*W1a;
            
            blk0[i] = (I32_WMV)((x0 + x2 + 32768L)>>16);
            blk1[i] = (I32_WMV)((x1 + x3 + 32768L)>>16);
            blk2[i] = (I32_WMV)((x1 - x3 + 32768L)>>16);
            blk3[i] = (I32_WMV)((x0 - x2 + 32768L)>>16);
            
        }
    }
    FUNCTION_PROFILE_STOP(&fpDecode);

}

// C Version
// Input:  32-bit
// Output: 32-bit
Void_WMV g_4x8IDCTDec_WMV2 (UnionBuffer* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer* rgiCoefReconBuf, I32_WMV iHalf)
{
    const I32_WMV __huge* rgiCoefRecon = rgiCoefReconBuf->i32;
	I32_WMV __huge* piDst = piDstBuf->i32 + (iHalf*4);
	I32_WMV * blk = piDst;

	I32_WMV x0, x1, x2, x3, x4, x5, x6, x7, x8;
	I32_WMV i;

    FUNCTION_PROFILE_DECL_START(fpDecode,IDCT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_4x8IDCTDec_WMV2);
    
    CLIPCOEF32 (12, 64, (I32_WMV __huge*)rgiCoefReconBuf->i32);

    for (i = 0; i < BLOCK_SIZE; i++,  rgiCoefRecon += 4){

        x4 = rgiCoefRecon[0];
        x5 = rgiCoefRecon[1];
        x6 = rgiCoefRecon[2];
        x7 = rgiCoefRecon[3];

        x0 = (x4 + x6)*W2a;
        x1 = (x4 - x6)*W2a;
        x2 = x5*W1a + x7*W3a;
        x3 = x5*W3a - x7*W1a;

        blk[0] = (I32_WMV)((x0 + x2 + 64)>>7);
        blk[1] = (I32_WMV)((x1 + x3 + 64)>>7);
        blk[2] = (I32_WMV)((x1 - x3 + 64)>>7);
        blk[3] = (I32_WMV)((x0 - x2 + 64)>>7);
        blk += iOffsetToNextRowForDCT;

    }
    
    {
        I32_WMV* blk0 = piDst;
        I32_WMV* blk1 = blk0 + iOffsetToNextRowForDCT;
        I32_WMV* blk2 = blk1 + iOffsetToNextRowForDCT;
        I32_WMV* blk3 = blk2 + iOffsetToNextRowForDCT;
        I32_WMV* blk4 = blk3 + iOffsetToNextRowForDCT;
        I32_WMV* blk5 = blk4 + iOffsetToNextRowForDCT;
        I32_WMV* blk6 = blk5 + iOffsetToNextRowForDCT;
        I32_WMV* blk7 = blk6 + iOffsetToNextRowForDCT;
        
        
        for (i = 0; i < 4; i++) {
            
            x0 = (I32_WMV)((I32_WMV)blk0[i] << 8) + 8192L;
            x1 = (I32_WMV)blk4[i] << 8;
            x2 = blk6[i];
            x3 = blk2[i];
            x4 = blk1[i];
            x5 = blk7[i];
            x6 = blk5[i];
            x7 = blk3[i];
            
            
            /* first stage */
            x8 = W7 * (x4 + x5) + 4;
            x4 = (x8 + (W1 - W7) * x4) >> 3;
            x5 = (x8 - (W1 + W7) * x5) >> 3;
            x8 = W3 * (x6 + x7) + 4;
            x6 = (x8 - (W3 - W5) * x6) >> 3;
            x7 = (x8 - (W3 + W5) * x7) >> 3;
            
            /* second stage */
            x8 = x0 + x1;
            x0 -= x1;
            x1 = W6 * (x3 + x2) + 4;
            x2 = (x1 - (W2 + W6) * x2) >> 3;
            x3 = (x1 + (W2 - W6) * x3) >> 3;
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
            
            blk0[i] = (I32_WMV) ((x7 + x1) >> 14);
            blk1[i] = (I32_WMV) ((x3 + x2) >> 14);
            blk2[i] = (I32_WMV) ((x0 + x4) >> 14);
            blk3[i] = (I32_WMV) ((x8 + x6) >> 14);
            blk4[i] = (I32_WMV) ((x8 - x6) >> 14);
            blk5[i] = (I32_WMV) ((x0 - x4) >> 14);
            blk6[i] = (I32_WMV) ((x3 - x2) >> 14);
            blk7[i] = (I32_WMV) ((x7 - x1) >> 14);
            
        }
    }

    FUNCTION_PROFILE_STOP(&fpDecode);

}

// NOTE: These are not currently MMX versions of 8x4IDCT and 4x8IDCT. They are C versions
//  The only difference between these and the non-MMX versions are 
//  that these produce 16-bit output samples instead of 32-bit. This is to be compatible 
//  with the MMX motion comp routines which follow the IDCT. 



Void_WMV g_8x4IDCTDec16_WMV2 (UnionBuffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iHalf)
{
    const I16_WMV __huge* rgiCoefRecon =  (I16_WMV*)rgiCoefReconBuf;
    I16_WMV __huge* piDst = piDstBuf->i16 + (iHalf*32);
    I16_WMV * blk = piDst;
    I32_WMV x0, x1, x2, x3, x4, x5, x6, x7, x8;
    I32_WMV i;
    I16_WMV* blk0;
    I16_WMV* blk1;
    I16_WMV* blk2;
    I16_WMV* blk3;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_8x4IDCTDec16_WMV2);
    
    CLIPCOEF16 (12, 64, (I16_WMV __huge*)rgiCoefReconBuf->i16);

    for (i = 0; i < 4; i++, blk += iOffsetToNextRowForDCT, rgiCoefRecon += BLOCK_SIZE) {
        x4 = rgiCoefRecon [1];      
        x3 = rgiCoefRecon [2];
        x7 = rgiCoefRecon [3];
        x1 = (I32_WMV)rgiCoefRecon [4] << 11;
        x6 = rgiCoefRecon [5];
        x2 = rgiCoefRecon [6];      
        x5 = rgiCoefRecon [7];

        *blk = rgiCoefRecon [0];

        x0 = (I32_WMV)((I32_WMV)blk [0] << 11) + 128L; /* for proper rounding in the fourth stage */
        /* first stage */
        x8 = W7 * (x4 + x5);
        x4 = x8 + (W1 - W7) * x4;
        x5 = x8 - (W1 + W7) * x5;
        x8 = W3 * (x6 + x7);
        x6 = x8 - (W3 - W5) * x6;
        x7 = x8 - (W3 + W5) * x7;

        /* second stage */
        x8 = x0 + x1;
        x0 -= x1;
        x1 = W6 * (x3 + x2);
        x2 = x1 - (W2 + W6) * x2;
        x3 = x1 + (W2 - W6) * x3;
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
        blk [0] = (I16_WMV) ((x7 + x1) >> 8);
        blk [1] = (I16_WMV) ((x3 + x2) >> 8);
        blk [2] = (I16_WMV) ((x0 + x4) >> 8);
        blk [3] = (I16_WMV) ((x8 + x6) >> 8);
        blk [4] = (I16_WMV) ((x8 - x6) >> 8);
        blk [5] = (I16_WMV) ((x0 - x4) >> 8);
        blk [6] = (I16_WMV) ((x3 - x2) >> 8);
        blk [7] = (I16_WMV) ((x7 - x1) >> 8);
    }

    blk0 = piDst;
    blk1 = blk0 + iOffsetToNextRowForDCT;
    blk2 = blk1 + iOffsetToNextRowForDCT;
    blk3 = blk2 + iOffsetToNextRowForDCT;

    for (i = 0; i < BLOCK_SIZE; i++) {
        x4 = blk0[i];
        x5 = blk1[i];
        x6 = blk2[i];
        x7 = blk3[i];

        x0 = (x4 + x6)*W2a;
        x1 = (x4 - x6)*W2a;
        x2 = x5*W1a + x7*W3a;
        x3 = x5*W3a - x7*W1a;

        blk0[i] = (I16_WMV)((x0 + x2 + 32768L)>>16);
        blk1[i] = (I16_WMV)((x1 + x3 + 32768L)>>16);
        blk2[i] = (I16_WMV)((x1 - x3 + 32768L)>>16);
        blk3[i] = (I16_WMV)((x0 - x2 + 32768L)>>16);
    }
}

// Input:  16-bit
// Output: 16-bit
Void_WMV g_4x8IDCTDec16_WMV2 (UnionBuffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iHalf)
{
    const I16_WMV __huge* rgiCoefRecon = (I16_WMV*)rgiCoefReconBuf;
    I16_WMV __huge* piDst = piDstBuf->i16 + (iHalf*4);
    I16_WMV * blk = piDst;
    I32_WMV x0, x1, x2, x3, x4, x5, x6, x7, x8;
    I32_WMV i;
    I16_WMV* blk0;
    I16_WMV* blk1;
    I16_WMV* blk2;
    I16_WMV* blk3;
    I16_WMV* blk4;
    I16_WMV* blk5;
    I16_WMV* blk6;
    I16_WMV* blk7;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_4x8IDCTDec16_WMV2);
    
    CLIPCOEF16 (12, 64, (I16_WMV __huge*)rgiCoefReconBuf->i16);

    for (i = 0; i < BLOCK_SIZE; i++, blk += iOffsetToNextRowForDCT, rgiCoefRecon += 4) {

        x4 = rgiCoefRecon[0];
        x5 = rgiCoefRecon[1];
        x6 = rgiCoefRecon[2];
        x7 = rgiCoefRecon[3];

        x0 = (x4 + x6)*W2a;
        x1 = (x4 - x6)*W2a;
        x2 = x5*W1a + x7*W3a;
        x3 = x5*W3a - x7*W1a;

        blk[0] = (I16_WMV)((x0 + x2 + 64)>>7);
        blk[1] = (I16_WMV)((x1 + x3 + 64)>>7);
        blk[2] = (I16_WMV)((x1 - x3 + 64)>>7);
        blk[3] = (I16_WMV)((x0 - x2 + 64)>>7);
    }

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
        x0 = (I32_WMV)((I32_WMV)blk0[i] << 8) + 8192L;
        x1 = (I32_WMV)blk4[i] << 8;
        x2 = blk6[i];
        x3 = blk2[i];
        x4 = blk1[i];
        x5 = blk7[i];
        x6 = blk5[i];
        x7 = blk3[i];

        /* first stage */
        x8 = W7 * (x4 + x5) + 4;
        x4 = (x8 + (W1 - W7) * x4) >> 3;
        x5 = (x8 - (W1 + W7) * x5) >> 3;
        x8 = W3 * (x6 + x7) + 4;
        x6 = (x8 - (W3 - W5) * x6) >> 3;
        x7 = (x8 - (W3 + W5) * x7) >> 3;

        /* second stage */
        x8 = x0 + x1;
        x0 -= x1;
        x1 = W6 * (x3 + x2) + 4;
        x2 = (x1 - (W2 + W6) * x2) >> 3;
        x3 = (x1 + (W2 - W6) * x3) >> 3;
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
        blk0[i] = (I16_WMV) ((x7 + x1) >> 14);
        blk1[i] = (I16_WMV) ((x3 + x2) >> 14);
        blk2[i] = (I16_WMV) ((x0 + x4) >> 14);
        blk3[i] = (I16_WMV) ((x8 + x6) >> 14);
        blk4[i] = (I16_WMV) ((x8 - x6) >> 14);
        blk5[i] = (I16_WMV) ((x0 - x4) >> 14);
        blk6[i] = (I16_WMV) ((x3 - x2) >> 14);
        blk7[i] = (I16_WMV) ((x7 - x1) >> 14);
    }
}

// Input:  16-bit
// Output: 16-bit
Void_WMV g_4x4IDCTDec16_WMV2 (UnionBuffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iQuadrant)
{
    //const I16_WMV __huge* rgiCoefRecon =  (I16_WMV*)rgiCoefReconBuf;
    I16_WMV __huge* piDst = piDstBuf->i16 + (iQuadrant&2)*16 + (iQuadrant&1)*4;
    //I16_WMV * blk = piDst;
    I32_WMV x0, x1, x2, x3, x4, x5, x6, x7;
    I32_WMV i;
    I16_WMV rgTemp[32];
    I16_WMV* blk = rgTemp;   
    I16_WMV* blk0 = (short*)rgiCoefReconBuf;
    I16_WMV* blk1 = blk0 + 4;
    I16_WMV* blk2 = blk1 + 4;
    I16_WMV* blk3 = blk2 + 4;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_4x4IDCTDec16_WMV2);
    
    CLIPCOEF16 (12, 64, (I16_WMV __huge*)rgiCoefReconBuf->i16);

    for (i = 0; i < 4; i++, blk += 4) {
        x4 = blk0[i];
        x5 = blk1[i];
        x6 = blk2[i];
        x7 = blk3[i];

        x0 = (x4 + x6)*W2a;
        x1 = (x4 - x6)*W2a;
        x2 = x5*W1a + x7*W3a;
        x3 = x5*W3a - x7*W1a;

        blk[0] = (I16_WMV)((x0 + x2 +64)>>7);
        blk[1] = (I16_WMV)((x1 + x3 + 64)>>7);
        blk[2] = (I16_WMV)((x1 - x3 + 64)>>7);
        blk[3] = (I16_WMV)((x0 - x2 + 64)>>7);
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

        x0 = (x4 + x6)*W2a;
        x1 = (x4 - x6)*W2a;
        x2 = x5*W1a + x7*W3a;
        x3 = x5*W3a - x7*W1a;

        blk0[i] = (I16_WMV)((x0 + x2 + 32768L)>>16);
        blk1[i] = (I16_WMV)((x1 + x3 + 32768L)>>16);
        blk2[i] = (I16_WMV)((x1 - x3 + 32768L)>>16);
        blk3[i] = (I16_WMV)((x0 - x2 + 32768L)>>16);
    }
}


// C Version
// Input:  32-bit
// Output: 32-bit
Void_WMV g_4x4IDCTDec_WMV2 (UnionBuffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iQuadrant)
{
    const I32_WMV __huge* rgiCoefRecon = rgiCoefReconBuf->i32;
	I32_WMV __huge* piDst = piDstBuf->i32 + (iQuadrant&2)*16 + (iQuadrant&1)*4;
	I32_WMV * blk = piDst;
    I32_WMV x0, x1, x2, x3, x4, x5, x6, x7;
    I32_WMV* blk0, *blk1, *blk2, *blk3;
    I32_WMV i ;

    FUNCTION_PROFILE_DECL_START(fpDecode,IDCT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_4x4IDCTDec_WMV2);
    
    CLIPCOEF32 (12, 64, (I32_WMV __huge*)rgiCoefReconBuf->i32);

    for ( i = 0; i < 4; i++, blk += iOffsetToNextRowForDCT, rgiCoefRecon += 4) {

        x4 = rgiCoefRecon[0];
        x5 = rgiCoefRecon[1];
        x6 = rgiCoefRecon[2];
        x7 = rgiCoefRecon[3];

        x0 = (x4 + x6)*W2a;
        x1 = (x4 - x6)*W2a;
        x2 = x5*W1a + x7*W3a;
        x3 = x5*W3a - x7*W1a;

        blk[0] = (I32_WMV)((x0 + x2 +64)>>7);
        blk[1] = (I32_WMV)((x1 + x3 + 64)>>7);
        blk[2] = (I32_WMV)((x1 - x3 + 64)>>7);
        blk[3] = (I32_WMV)((x0 - x2 + 64)>>7);
    }

	blk0 = piDst;
	blk1 = blk0 + iOffsetToNextRowForDCT;
	blk2 = blk1 + iOffsetToNextRowForDCT;
    blk3 = blk2 + iOffsetToNextRowForDCT;

    for (i = 0; i < 4; i++) {
        x4 = blk0[i];
        x5 = blk1[i];
        x6 = blk2[i];
        x7 = blk3[i];

        x0 = (x4 + x6)*W2a;
        x1 = (x4 - x6)*W2a;
        x2 = x5*W1a + x7*W3a;
        x3 = x5*W3a - x7*W1a;

        blk0[i] = (I32_WMV)((x0 + x2 + 32768L)>>16);
        blk1[i] = (I32_WMV)((x1 + x3 + 32768L)>>16);
        blk2[i] = (I32_WMV)((x1 - x3 + 32768L)>>16);
        blk3[i] = (I32_WMV)((x0 - x2 + 32768L)>>16);
    }
    FUNCTION_PROFILE_STOP(&fpDecode);
}
