@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************

@//************************************************************************
@//
@// Module Name:
@//
@//     idctARM.s
@//
@// Abstract:
@// 
@//     ARM specific transforms
@//     Optimized assembly routines to implement WMV9 8x8, 4x8, 8x4, 4x4 IDCT
@//
@//     Custom build with 
@//          armasm $(InputDir)\$(InputName).s $(OutDir)\$(InputName).obj
@//     and
@//          $(OutDir)\$(InputName).obj
@// 
@// Author:
@// 
@//     Chuang Gu (chuanggu@microsoft.com) Nov. 10, 2002
@//
@// Revision History:
@//
@//*************************************************************************
@//
@// r0 : x0, r1 : x1, ..., r8 : x8@
@// r14: blk[]
@// r9, r10, r11, r12 -> temporal registers
@//
@//*************************************************************************

    #include "../c/voWMVDecID.h"
    .include "wmvdec_member_arm.h"
    .include "xplatform_arm_asm.h"

    @AREA WMV9_IDCT, CODE, READONLY
     .text
     .align 4
    
    .if WMV_OPT_IDCT_ARM == 1

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@   WMV9 transform
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    
    .globl  _g_IDCTDec16_WMV3
    .globl  _g_8x4IDCTDec_WMV3
    .globl  _g_4x8IDCTDec_WMV3
    .globl  _g_4x4IDCTDec_WMV3
    .globl  _SignPatch
			
    .globl  _SignPatch32
@   .globl  _g_IDCTDec_WMV3_Pass1
@   .globl  _g_IDCTDec_WMV3_Pass2
@   .globl  _g_IDCTDec_WMV3_Pass3
@   .globl  _g_IDCTDec_WMV3_Pass4
			
    .globl  _SignPatch
    .globl  _SignPatch32
    .globl  _g_SubBlkIDCTClear_EMB
			
    .globl  _g_IDCTDec16_WMV3_SSIMD
    .globl  _g_DecodeInterError8x8_SSIMD
    .globl  _g_DecodeInterError8x4_SSIMD
    .globl  _g_DecodeInterError4x8_SSIMD
    .globl  _g_DecodeInterError4x4_SSIMD 

    .globl  _g_IDCTDec_WMV3_Pass1_Naked
    .globl  _g_IDCTDec_WMV3_Pass2_Naked
    .globl  _g_IDCTDec_WMV3_Pass3_Naked
    .globl  _g_IDCTDec_WMV3_Pass4_Naked
			
    .globl  _DecodeInverseInterBlockQuantize16_EMB_Naked
			

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA    |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY g_IDCTDec16_WMV3_SSIMD
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ r0 == piDst
@ r1 == piSrc
@ r2 == iOffsetToNextRowForDCT
@ r3 == iDCTHorzFlags

.set ST_piDst_4x4IDCTDec       ,0
.set ST_tmpBuffer_4x4IDCTDec   ,4
.set ST_SIZE_4x4IDCTDec        ,260

    stmdb     sp!, {r4 - r11, lr}
    FRAME_PROFILE_COUNT

    sub       sp, sp, #ST_SIZE_4x4IDCTDec
    str       r0, [sp, #ST_piDst_4x4IDCTDec]

@   g_IDCTDec_WMV3_Pass1(piSrc0, blk32, 4, iDCTHorzFlags)@
    mov       r0, r1
    mov       r2, #4
    add       r1, sp, #ST_tmpBuffer_4x4IDCTDec
    bl        _g_IDCTDec_WMV3_Pass1_Naked

@   g_IDCTDec_WMV3_Pass2(piSrc0, blk16, 4)@
    mov       r2, #4
    ldr       r1, [sp, #ST_piDst_4x4IDCTDec]
    add       r0, sp, #ST_tmpBuffer_4x4IDCTDec
    bl        _g_IDCTDec_WMV3_Pass2_Naked

    add       sp, sp, #ST_SIZE_4x4IDCTDec
    ldmia     sp!, {r4 - r11, pc}
    WMV_ENTRY_END


	.align 8	
Pass1_table:
		.word 15,12,24,20,6,4				
	.align 8	
Pass2_table:
		.word -3,6,-5,7,-11,-10,-12,-6				
	.align 8	
Pass3_table:	
		.word 10,17,-32,12		

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA    |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY g_DecodeInterError8x8_SSIMD
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@r0 == pPMainLoop
@r1 == ppInterDCTTableInfo_Dec
@r2 == pDQ

.set ST_result_8x8      ,0
.set ST_blk16_8x8       ,4
.set ST_tmpBuffer_8x8   ,8

.set ST_SIZE_8x8        ,264

    stmdb     sp!, {r4 - r11, lr}
    FRAME_PROFILE_COUNT
    
    sub       sp, sp, #ST_SIZE_8x8

@   result == DecodeInverseInterBlockQuantize16_EMB(pPMainLoop, 
@           ppInterDCTTableInfo_Dec, 
@           XFORMMODE_8x8, 
@           pDQ)@

    ldr       r4, [r0, #tagEMB_PBMainLoop_m_rgiCoefReconBuf]
    mov       r3, r2
    mov       r2, #0
    str       r4, [sp, #ST_blk16_8x8]

@   return value
@   r0 == result
@   r3 == pPMainLoop
@   r8 == pPMainLoop->m_iDCTHorzFlags 
    bl        _DecodeInverseInterBlockQuantize16_EMB_Naked
    str       r0, [sp, #ST_result_8x8]

@   UnionBuffer * ppxliErrorQMB == pPMainLoop->m_rgiCoefReconBuf@
@   I32_WMV *piSrc0 == ppxliErrorQMB->i32@
@   I16_WMV *blk16  == ppxliErrorQMB->i16@  
    
@   g_IDCTDec_WMV3_Pass1(piSrc0, tmpBuffer, 4, pPMainLoop->m_iDCTHorzFlags)@
    ldr       r0, [sp, #ST_blk16_8x8]
    mov       r3, r8
    mov       r2, #4
    add       r1, sp, #ST_tmpBuffer_8x8
    bl        _g_IDCTDec_WMV3_Pass1_Naked

@   g_IDCTDec_WMV3_Pass2(tmpBuffer, blk16, 4)@

    ldr       r1, [sp, #ST_blk16_8x8]
    mov       r2, #4
    add       r0, sp, #ST_tmpBuffer_8x8
    bl        _g_IDCTDec_WMV3_Pass2_Naked
    
@   return result@
    ldr       r0, [sp, #ST_result_8x8]

    add       sp, sp, #ST_SIZE_8x8
    ldmia     sp!, {r4 - r11, pc}
    WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA    |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY g_DecodeInterError8x4_SSIMD
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@r0 == pPMainLoop
@r1 == ppInterDCTTableInfo_Dec
@r2 == pDQ
@r3 == i

.set ST_result_8x4      ,   0
.set ST_blk16_8x4       ,   4
.set ST_tmpBuffer_8x4   ,   8

.set ST_SIZE_8x4        ,   72

    stmdb     sp!, {r4 - r11, lr}
    FRAME_PROFILE_COUNT

    ldr       r4, [r0, #tagEMB_PBMainLoop_m_ppxliErrorQ]
    sub       sp, sp, #ST_SIZE_8x4

@   UnionBuffer * ppxliErrorQMB == pPMainLoop->m_ppxliErrorQ@
@   I32_WMV * piDst == ppxliErrorQMB->i32 + (i << 3)@
    add       r5, r4, r3, lsl #5
    str       r5, [sp, #ST_blk16_8x4]

@   result == DecodeInverseInterBlockQuantize16_EMB(pPMainLoop, 
@       ppInterDCTTableInfo_Dec, 
@       XFORMMODE_8x4, 
@       pDQ)@

@   return value
@   r0 == result
@   r3 == pPMainLoop
@   r8 == pPMainLoop->m_iDCTHorzFlags 
    mov       r3, r2
    mov       r2, #1
    bl        _DecodeInverseInterBlockQuantize16_EMB_Naked
    str       r0, [sp, #ST_result_8x4]

@   I16_WMV  * blk16@
@   const I32_WMV  * piSrc0 == pPMainLoop->m_rgiCoefReconBuf->i32@
	
@   g_IDCTDec_WMV3_Pass1(piSrc0, tmpBuffer, 2, pPMainLoop->m_iDCTHorzFlags)@
    mov       r2, #2
    ldr       r0, [r3, #tagEMB_PBMainLoop_m_rgiCoefReconBuf]
    add       r1, sp, #ST_tmpBuffer_8x4
    mov       r3, r8
    bl        _g_IDCTDec_WMV3_Pass1_Naked

@   blk16  == (I16_WMV *)piDst@
@   g_IDCTDec_WMV3_Pass4(tmpBuffer,  blk16, 4)@

    mov       r2, #4
    ldr       r1, [sp, #ST_blk16_8x4]
    add       r0, sp, #ST_tmpBuffer_8x4
    bl        _g_IDCTDec_WMV3_Pass4_Naked

@   return result@
    ldr       r0, [sp, #ST_result_8x4]

    add       sp, sp, #ST_SIZE_8x4
    ldmia     sp!, {r4 - r11, pc}
    WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA    |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY g_DecodeInterError4x8_SSIMD
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@r0 == pPMainLoop
@r1 == ppInterDCTTableInfo_Dec
@r2 == pDQ
@r3 == i

.set ST_result_4x8      ,   0
.set ST_blk16_4x8       ,   4
.set ST_tmpBuffer_4x8   ,   8

.set ST_SIZE_4x8        ,   136


    stmdb     sp!, {r4 - r11, lr}  @ stmfd
    FRAME_PROFILE_COUNT

    ldr       r4, [r0, #tagEMB_PBMainLoop_m_ppxliErrorQ]
    sub       sp, sp, #ST_SIZE_4x8

@   UnionBuffer * ppxliErrorQMB == pPMainLoop->m_ppxliErrorQ@
@   I32_WMV * piDst == ppxliErrorQMB->i32 + (i)@
    add       r5, r4, r3, lsl #2
    str       r5, [sp, #ST_blk16_4x8]

@   result == DecodeInverseInterBlockQuantize16_EMB(pPMainLoop, 
@       ppInterDCTTableInfo_Dec, 
@       XFORMMODE_4x8, 
@       pDQ)@

@   return value
@   r0 == result
@   r3 == pPMainLoop
@   r8 == pPMainLoop->m_iDCTHorzFlags 

    mov       r3, r2
    mov       r2, #2
    bl        _DecodeInverseInterBlockQuantize16_EMB_Naked
    str       r0, [sp, #ST_result_4x8]

    mov       r2, #4
@   const I32_WMV  * piSrc0 == pPMainLoop->m_rgiCoefReconBuf->i32@
    ldr       r0, [r3, #tagEMB_PBMainLoop_m_rgiCoefReconBuf]
    
@   g_IDCTDec_WMV3_Pass3(piSrc0,  tmpBuffer, 4, pPMainLoop->m_iDCTHorzFlags)@
    add       r1, sp, #ST_tmpBuffer_4x8
    mov       r3, r8
    bl        _g_IDCTDec_WMV3_Pass3_Naked

@   blk16  == (I16_WMV *)piDst@
@   g_IDCTDec_WMV3_Pass2(tmpBuffer, blk16, 2)@

    ldr       r1, [sp, #ST_blk16_4x8]
    add       r0, sp, #ST_tmpBuffer_4x8
    mov       r2, #2
    bl        _g_IDCTDec_WMV3_Pass2_Naked
    
@   return result@

    ldr       r0, [sp, #ST_result_4x8]

    add       sp, sp, #ST_SIZE_4x8
    ldmia     sp!, {r4 - r11, pc}  @ ldmfd
    WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA    |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY g_DecodeInterError4x4_SSIMD
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@r0 == pPMainLoop
@r1 == ppInterDCTTableInfo_Dec
@r2 == pDQ
@r3 == i

.set ST_result_4x4      ,   0
.set ST_blk16_4x4       ,   4
.set ST_tmpBuffer_4x4   ,   8

.set ST_SIZE_4x4        ,   72

    stmdb     sp!, {r4 - r11, lr}

@   UnionBuffer * ppxliErrorQMB == pPMainLoop->m_ppxliErrorQ@
@   I16_WMV* piDst == ppxliErrorQMB->i16 + (i&2)*8 + (i&1)*2@

    and       r4, r3, #2
    and       r3, r3, #1
    add       r3, r3, r4, lsl #2

    ldr       r5, [r0, #tagEMB_PBMainLoop_m_ppxliErrorQ]
    sub       sp, sp, #ST_SIZE_4x4

    add       r5, r5, r3, lsl #2
    str       r5, [sp, #ST_blk16_4x4]

@   result == DecodeInverseInterBlockQuantize16_EMB(pPMainLoop, 
@       ppInterDCTTableInfo_Dec, 
@       XFORMMODE_4x4, 
@       pDQ)@

@   return value
@   r0 == result
@   r3 == pPMainLoop
@   r8 == pPMainLoop->m_iDCTHorzFlags

    mov       r3, r2
    mov       r2, #4
    bl        _DecodeInverseInterBlockQuantize16_EMB_Naked
    str       r0, [sp, #ST_result_4x4]

@   piSrc0 == (I32_WMV *)pPMainLoop->m_rgiCoefReconBuf->i32@
@   g_IDCTDec_WMV3_Pass3(piSrc0,  rgTemp, 2, pPMainLoop->m_iDCTHorzFlags)@

    mov       r2, #2
    ldr       r0, [r3, #tagEMB_PBMainLoop_m_rgiCoefReconBuf]
    add       r1, sp, #ST_tmpBuffer_4x4
    mov       r3, r8
    bl        _g_IDCTDec_WMV3_Pass3_Naked

@   g_IDCTDec_WMV3_Pass4(rgTemp,  piDst, 2)@
    ldr       r1, [sp, #ST_blk16_4x4]
    mov       r2, #2
    add       r0, sp, #ST_tmpBuffer_4x4
    bl        _g_IDCTDec_WMV3_Pass4_Naked
    
@   return result@
    ldr       r0, [sp, #ST_result_4x4]

    add       sp, sp, #ST_SIZE_4x4
    ldmia     sp!, {r4 - r11, pc}
    WMV_ENTRY_END

@Void_WMV g_SubBlkIDCTClear_EMB (UnionBuffer * piDstBuf,  I32_WMV iIdx)

    @AREA |.embsec_PMainLoopLvl1|, CODE, READONLY
    WMV_LEAF_ENTRY g_SubBlkIDCTClear_EMB
@r3  == iStep
@r12 == iNumLoop

    stmdb     sp!, {lr}  @ stmfd

    FRAME_PROFILE_COUNT

@I32_WMV iNumLoop == 8<<(iIdx>>7)@
    mov       r3, r1, asr #7
    mov       r2, #8
    mov       r12,r2, lsl r3

@I32_WMV iStep   ==  1<< ((iIdx >>6)&0x1)@
    mov       r3, r1, asr #6
    mov       r2, #1
    and       r3, r3, #1
    mov       r3, r2, lsl r3

@I32_WMV iStep2 == iStep<<1@
@I32_WMV* piDst == piDstBuf->i32 + (iIdx&0x3f)@
    and       r2, r1, #0x3F  @ 0x3F == 63
    mov       lr, #0
    add       r0, r0, r2, lsl #2
    mov       r2, r3, lsl #3
    add       r1, r0, r3, lsl #2

@DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_SubBlkIDCTClear_EMB)@
@for (i == 0@ i < iNumLoop@ i+==iStep2) 
g_SubBlkLoop:

@       piDst[i] == 0@
@       piDst[i+16] == 0@
    str       lr, [r0]
    subs      r12,r12,r3, lsl #1
    str       lr, [r0, #0x40]  @ 0x40 == 64

@       piDst[i + iStep] == 0@
@       piDst[i+16 + iStep] == 0@
    str       lr, [r1]
    add       r0, r0, r2
    str       lr, [r1, #0x40]

    add       r1, r1, r2
    bgt       g_SubBlkLoop

    ldmia     sp!, {pc}  @ ldmfd
    WMV_ENTRY_END	@g_DecodeInterError4x4_SSIMD

    @ENDP  @ |g_SubBlkIDCTClear_EMB|
    



@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    WMV_LEAF_ENTRY g_IDCTDec16_WMV3
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ 401  : {

    stmdb     sp!, {r4 - r12, lr}
    FRAME_PROFILE_COUNT
    sub       sp, sp, #0x4A, 30
M37421:
    str       r0, [sp, #0x20]
    mov       r6, r1

@ 402  :        I32_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a@
@ 403  :     I32_WMV  y3, y4, y5, y4a, b0,c0,b1,c1,ls_signbit@
@ 404  :     I32_WMV  *piSrc0@
@ 405  :     I32_WMV i,j@
@ 406  :     register I32_WMV iDCTVertFlag == 0@
@ 407  :    const I16_WMV * rgiCoefRecon == piSrc->i16@
@ 408  :    I16_WMV * blk16 == piDst->i16@
@ 409  :     I32_WMV * blk32 == piDst->i32@
@ 410  :  
@ 411  : 
@ 412  : 
@ 413  :      I32_WMV tmpBuffer[64]@
@ 414  : 
@ 415  :     FUNCTION_PROFILE(fpDecode)@
@ 416  : 
@ 417  : 
@ 418  :     piSrc0 == piSrc->i32@
@ 419  : 
@ 420  :   //  memcpy(dsttmp, piSrc->i32, 128)@
@ 421  :   //  piSrc0 == (I32_WMV*) dsttmp@
@ 422  : 
@ 423  :    blk32 == tmpBuffer@

    add       r0, sp, #0x28
    mov       r1, #4
    str       r0, [sp, #0x18]
    str       r1, [sp, #0x1C]
L36861:

@ 424  : 
@ 425  : 
@ 426  :     FUNCTION_PROFILE_START(&fpDecode,IDCT_NT_PROFILE)@
@ 427  : 
@ 428  :     for ( i == 0@ i < (BLOCK_SIZE>>1)@ i++,  blk32 +== 8){
@ 429  :         // Check to see if this row has all zero coefficients in columns 1-7 (0 is first column)
@ 430  : //        if (!(iDCTHorzFlags & (1 << i))) {
@ 431  : //            // Columns 1-7 must be zero. Check to see if column zero coefficient is zero. 
@ 432  : //            // If so, we don't have to do anything more for this row
@ 433  : //            // Column zero cofficient is nonzero so all coefficients in this row are DC
@ 434  : //            blk [0] == blk [1] == blk [2] == blk [3] == blk [4] == blk [5] == blk [6] == blk [7] == rgiCoefRecon [0] << 3@
@ 435  : //            // If this is not the first row, tell the second stage that it can't use DC
@ 436  : //            // shortcut - it has to do the full transform for all the columns
@ 437  : //            iDCTVertFlag == i@
@ 438  : //         continue@
@ 439  : //     }
@ 440  : 
@ 441  :         // If this is not the first row, tell the second stage that it can't use DC shortcut 
@ 442  :         // - it has to do the full transform for all the columns
@ 443  : 
@ 444  :        x4 == piSrc0[ i +1*4 ]@      
@ 445  :        x3 == piSrc0[ i +2*4 ]@

    ldr       r0, [r6, #0x20]
    ldr       r7, [r6, #0x10]
    ldr       r11, [r6, #0x60]
    str       r0, [sp, #0x10]

@ 446  :        x7 == piSrc0[ i +3*4 ]@
@ 447  :        x1 == piSrc0[ i +4*4 ]@
@ 448  :        x6 == piSrc0[ i +5*4 ]@
@ 449  :        x2 == piSrc0[ i +6*4 ]@      

    
    ldr       lr, [r6, #0x30]
    

@ 450  :        x5 == piSrc0[ i +7*4 ]@
@ 451  :         x0 == piSrc0[ i +0*4 ]@ /* for proper rounding */
@ 452  : 
@ 453  : 
@ 454  :         x1 == x1 * W0@

    ldr       r0, [r6, #0x40]
    str       r11, [sp, #0xC]
    ldr       r10, [r6, #0x50]
    add       r1, r0, r0, lsl #1
    ldr       r3, [r6, #0x70]

@ 455  :         x0 == x0 * W0 + (4+(4<<16))@ /* for proper rounding */

    ldr       r0, [r6], #4

    mov       r11, r1, lsl #2
    add       r1, r0, r0, lsl #1
    mov       r2, r1, lsl #2
    add       r0, r2, #1, 14
    

@ 456  : 
@ 457  :         // zeroth stage
@ 458  :         y3 == x4 + x5@

    add       r2, r3, r7
    add       r9, r0, #4

@ 459  :        x8 == W3 * y3@

    

@ 460  :        x4a == x8 - W3pW5 * x5@

    add       r0, r3, r3, lsl #1
    rsb       r1, r2, r2, lsl #4
    sub       r4, r1, r0, lsl #3

@ 461  :        x5a == x8 - W3_W5 * x4@

    add       r0, r7, r7, lsl #1
    mov       r2, r2, lsl #2
    sub       r5, r1, r0, lsl #1

@ 462  :        x8 == W7 * y3@

    

@ 463  :        x4 == x8 + W1_W7 * x4@

    add       r0, r7, r7, lsl #1

    add       r7, r2, r0, lsl #2

@ 464  :        x5 == x8 - W1pW7 * x5@

    add       r0, r3, r3, lsl #2
    add       r3, r10, lr
    sub       r8, r2, r0, lsl #2

@ 465  : 
@ 466  :        // first stage
@ 467  :         y3 == x6 + x7@

    

@ 468  :        x8 == W7 * y3@
@ 469  :        x4a -== x8 + W1_W7 * x6@

    add       r0, r10, r10, lsl #1
    mov       r2, r3, lsl #2
    sub       r1, r4, r0, lsl #2
    sub       r12, r1, r2
    

@ 470  :        x5a +== x8 - W1pW7 * x7@

    add       r0, lr, lr, lsl #2
    str       r12, [sp]
    sub       r1, r5, r0, lsl #2
    add       r12, r1, r2

@ 471  :        x8 == W3 * y3@

    

@ 472  :        x4 +== x8 - W3_W5 * x6@

    add       r0, r10, r10, lsl #1
    rsb       r2, r3, r3, lsl #4
    sub       r1, r2, r0, lsl #1

@ 473  :        x5 +== x8 - W3pW5 * x7@

    add       r0, lr, lr, lsl #1
    add       r10, r7, r1
    sub       r1, r2, r0, lsl #3

@ 474  : 
@ 475  :        // second stage 
@ 476  :        x8 == x0 + x1@
@ 477  :        x0 -== x1@
@ 478  : 
@ 479  :        x1 == x2@
@ 480  :        x2 == W6 * x3 - W2 * x2@  // simplify?

    ldr       r2, [sp, #0x10]
    add       lr, r8, r1
    str       r10, [sp, #0x14]
    add       r0, r2, r2, lsl #1

    mov       r1, r0, lsl #1
    ldr       r0, [sp, #0xC]
    add       r4, r9, r11
    sub       r5, r9, r11
    sub       r3, r1, r0, lsl #4

@ 481  :        x3 == W6 * x1 + W2A * x3@

    add       r0, r0, r0, lsl #1
    mov       r1, r0, lsl #1
    add       r2, r1, r2, lsl #4
    

@ 482  : 
@ 483  :        // third stage
@ 484  :        x7 == x8 + x3@

    add       r0, r4, r2

@ 485  :        x8 -== x3@
@ 486  :        x3 == x0 + x2@

    add       r11, r5, r3
    str       r0, [sp, #0xC]

@ 487  :        x0 -== x2@

    sub       r7, r5, r3

@ 488  : 
@ 489  :        // fourth stage
@ 490  : /*
@ 491  :        blk [0] == (I16_WMV) ((x7 + x4) >> 3)@
@ 492  :        blk [1] == (I16_WMV) ((x3 + x4a) >> 3)@
@ 493  :        blk [2] == (I16_WMV) ((x0 + x5a) >> 3)@
@ 494  :        blk [3] == (I16_WMV) ((x8 + x5) >> 3)@
@ 495  :        blk [4] == (I16_WMV) ((x8 - x5) >> 3)@
@ 496  :        blk [5] == (I16_WMV) ((x0 - x5a) >> 3)@
@ 497  :        blk [6] == (I16_WMV) ((x3 - x4a) >> 3)@
@ 498  :        blk [7] == (I16_WMV) ((x7 - x4) >> 3)@
@ 499  : */
@ 500  : 
@ 501  :         // blk [0,1]
@ 502  :         b0 == x7 + x4@

    add       r5, r0, r10

@ 503  :         b1 == (b0 + 0x8000)>>19@
@ 504  :         b0 == ((I16_WMV)b0)>>3@
@ 505  : 
@ 506  :         c0 == x3 + x4a@

    ldr       r0, [sp]
    sub       r8, r4, r2
    add       r4, r0, r11

@ 507  :         c1 == (c0 + 0x8000)>>19@
@ 508  :         c0 == ((I16_WMV)c0)>>3@
@ 509  : 
@ 510  :         blk32[0] == (c0<<16) + b0@

    mov       r0, r4, lsl #16
    mov       r2, r0, asr #19
    mov       r0, r5, lsl #16
    mov       r3, r2, lsl #16
    add       r0, r3, r0, asr #19
    add       r1, r4, #2, 18
    str       r0, [sp, #0x10]
    mov       r0, r1, asr #19

@ 511  :         blk32[0+4] == (c1<<16) + b1@

    mov       r2, r0, lsl #16
    add       r1, r5, #2, 18
    add       r4, r8, lr
    add       r0, r2, r1, asr #19
    str       r0, [sp, #8]

@ 512  : 
@ 513  :         // blk [2,3]
@ 514  :         b0 == x0 + x5a@
@ 515  :         b1 == (b0 + 0x8000)>>19@
@ 516  :         b0 == ((I16_WMV)b0)>>3@
@ 517  : 
@ 518  :         c0 == x8 + x5@

    
    

@ 519  :         c1 == (c0 + 0x8000)>>19@
@ 520  :         c0 == ((I16_WMV)c0)>>3@
@ 521  : 
@ 522  :         blk32[1] == (c0<<16) + b0@

    mov       r0, r4, lsl #16
    add       r5, r12, r7
    mov       r2, r0, asr #19
    mov       r0, r5, lsl #16
    mov       r3, r2, lsl #16
    add       r10, r3, r0, asr #19
    add       r1, r4, #2, 18
    mov       r0, r1, asr #19

@ 523  :         blk32[1+4] == (c1<<16) + b1@

    add       r1, r5, #2, 18
    mov       r2, r0, lsl #16

@ 524  : 
@ 525  :         // blk [4,5]
@ 526  :         b0 == x8 - x5@
@ 527  :         b1 == (b0 + 0x8000)>>19@
@ 528  :         b0 == ((I16_WMV)b0)>>3@
@ 529  : 
@ 530  :         c0 == x0 - x5a@

    
    sub       r4, r7, r12
    

@ 531  :         c1 == (c0 + 0x8000)>>19@
@ 532  :         c0 == ((I16_WMV)c0)>>3@
@ 533  : 
@ 534  :         blk32[2] == (c0<<16) + b0@

    mov       r0, r4, lsl #16
    add       r9, r2, r1, asr #19
    mov       r2, r0, asr #19
    sub       r5, r8, lr 
    mov       r0, r5, lsl #16
    mov       r3, r2, lsl #16
    add       r1, r4, #2, 18
    add       r8, r3, r0, asr #19
    mov       r0, r1, asr #19

@ 535  :         blk32[2+4] == (c1<<16) + b1@

    mov       r2, r0, lsl #16

@ 536  : 
@ 537  :         // blk [6,7]
@ 538  :         b0 == x3 - x4a@

    
    add       r1, r5, #2, 18
    ldr       r0, [sp]
    add       r7, r2, r1, asr #19

@ 539  :         b1 == (b0 + 0x8000)>>19@
@ 540  :         b0 == ((I16_WMV)b0)>>3@
@ 541  : 
@ 542  :         c0 == x7 - x4@

    ldr       r1, [sp, #0x14]
    sub       r5, r11, r0
    ldr       r0, [sp, #0xC]
    sub       r4, r0, r1

@ 543  :         c1 == (c0 + 0x8000)>>19@
@ 544  :         c0 == ((I16_WMV)c0)>>3@
@ 545  : 
@ 546  :         blk32[3] == (c0<<16) + b0@

    mov       r0, r4, lsl #16
    mov       r2, r0, asr #19
    mov       r0, r5, lsl #16
    mov       r3, r2, lsl #16
    add       r1, r4, #2, 18
    add       r3, r3, r0, asr #19
    mov       r0, r1, asr #19

@ 547  :         blk32[3+4] == (c1<<16) + b1@

    add       r1, r5, #2, 18
    mov       r2, r0, lsl #16
    add       r0, r2, r1, asr #19
    ldr       r2, [sp, #0x18]
    ldr       r1, [sp, #0x10]
    str       r8, [r2, #8]
    str       r0, [r2, #0x1C]
    str       r1, [r2]
    ldr       r1, [sp, #8]
    ldr       r0, [sp, #0x1C]
    str       r9, [r2, #0x14]
    str       r10, [r2, #4]
    subs      r0, r0, #1
    str       r3, [r2, #0xC]
    str       r1, [r2, #0x10]
    str       r7, [r2, #0x18]
    add       r2, r2, #0x20
    str       r2, [sp, #0x18]
    str       r0, [sp, #0x1C]
    bne       L36861

@ 548  : 
@ 549  :    }
@ 550  : 
@ 551  : 
@ 552  : 
@ 553  : 
@ 554  :     piSrc0 == tmpBuffer@
@ 555  : 
@ 556  :     blk16  == piDst->i16@
@ 557  : 
@ 558  :    for (i == 0@ i < (BLOCK_SIZE>>1)@ i++){

    mov       r12, #0
    add       r0, sp, #0x28
    str       r0, [sp]

@ 598  : 
@ 599  :        x1 == 8 * (x2 + x3)@

    b         L36872
L37420:
    ldr       r0, [sp]
L36872:

@ 559  :         // If iDCTVertFlag is zero then we know that blk[1] thru blk[7] are zero.
@ 560  :         // Therefore, if blk[0] is zero we can skip to the next column. Otherwise we
@ 561  :         // can use the DC shortcut and just copy blk[0] to the rest of the column.
@ 562  : //        if (iDCTVertFlag ==== 0){
@ 563  : //            // If blk[0] is nonzero, copy the value to the other 7 rows in this column       
@ 564  : //            if (blk0[i] != 0)
@ 565  : //             blk0[i] == blk1[i] == blk2[i] == blk3[i] == blk4[i] == blk5[i] == blk6[i] == blk7[i] == (blk0[i] + 32) >> 6@
@ 566  : //         continue@
@ 567  : //     }
@ 568  :         x0 == piSrc0[i + 0*4 ] * 6 + 32 + (32<<16) /* rounding */@

    ldr       r0, [r0]
    add       r1, r0, r0, lsl #1
    mov       r2, r1, lsl #1
    add       r0, r2, #2, 12

@ 569  :        x1 == piSrc0[i + 4*4 ] * 6@

    ldr       r2, [sp]
    add       r1, r0, #0x20
    str       r1, [sp, #8]
    ldr       r1, [r2, #0x40]

@ 570  :        x2 == piSrc0[i + 6*4 ]@
@ 571  :        x3 == piSrc0[i + 2*4 ]@
@ 572  :        x4 == piSrc0[i + 1*4 ]@

    ldr       r3, [r2, #0x10]

@ 573  :        x5 == piSrc0[i + 7*4 ]@

    
    add       r0, r1, r1, lsl #1
    ldr       r5, [r2, #0x70]
    mov       lr, r0, lsl #1

@ 574  :        x6 == piSrc0[i + 5*4 ]@

    ldr       r7, [r2, #0x50]

@ 575  :        x7 == piSrc0[i + 3*4 ]@
@ 576  : 
@ 577  :         // zeroth stage
@ 578  :         y4a == x4 + x5@

    add       r9, r5, r3
    ldr       r8, [r2, #0x30]

@ 579  :        x8 == 7 * y4a@

    rsb       r1, r9, r9, lsl #3
    

@ 580  :        x4a == x8 - 12 * x5@

    add       r0, r5, r5, lsl #1
    str       r9, [sp, #0x14]
    sub       r4, r1, r0, lsl #2

@ 581  :        x5a == x8 - 3 * x4@

    add       r0, r3, r3, lsl #1
    sub       r6, r1, r0

@ 582  :        x8 == 2 * y4a@

    

@ 583  :        x4 == x8 + 6 * x4@

    add       r0, r3, r3, lsl #1
    mov       r2, r9, lsl #1
    add       r3, r2, r0, lsl #1

@ 584  :        x5 == x8 - 10 * x5@

    add       r0, r5, r5, lsl #2
    add       r9, r7, r8
    sub       r5, r2, r0, lsl #1

@ 585  : 
@ 586  :        // first stage
@ 587  :         y4 == x6 + x7@

    

@ 588  :        x8 == 2 * y4@
@ 589  :        x4a -== x8 + 6 * x6@

    add       r0, r7, r7, lsl #1
    mov       r2, r9, lsl #1
    sub       r1, r4, r0, lsl #1
    

@ 590  :        x5a +== x8 - 10 * x7@

    add       r0, r8, r8, lsl #2
    sub       r11, r1, r2
    sub       r1, r6, r0, lsl #1
    add       r10, r1, r2

@ 591  :        x8 == 7 * y4@

    rsb       r2, r9, r9, lsl #3

@ 592  :        x4 +== x8 - 3 * x6@

    add       r0, r7, r7, lsl #1
    sub       r1, r2, r0

@ 593  :        x5 +== x8 - 12 * x7@

    add       r0, r8, r8, lsl #1
    add       r7, r3, r1
    sub       r1, r2, r0, lsl #2

@ 594  : 
@ 595  :        // second stage 
@ 596  :        x8 == x0 + x1@

    ldr       r0, [sp, #8]
    add       r6, r5, r1
    add       r4, r0, lr

@ 597  :        x0 -== x1@

    sub       r5, r0, lr

@ 598  : 
@ 599  :        x1 == 8 * (x2 + x3)@

    ldr       r0, [sp]
    ldr       r8, [r0, #0x20]
    ldr       r1, [r0, #0x60]
    add       r0, r1, r8
    mov       r2, r0, lsl #3

@ 600  :        x6 == x1 - 5 * x2@

    add       r0, r1, r1, lsl #2
    sub       r3, r2, r0

@ 601  :        x1 -== 11 * x3@

    add       r0, r8, r8, lsl #2
    add       r1, r8, r0, lsl #1

@ 602  : 
@ 603  :        // third stage
@ 604  :        x7 == x8 + x6@

    add       r0, r4, r3
    sub       r2, r2, r1
    str       r0, [sp, #8]

@ 605  :        x8 -== x6@
@ 606  : 
@ 607  :        x6 == x0 - x1@

    sub       r0, r5, r2

@ 608  :        x0 +== x1@

    add       lr, r5, r2
    str       r0, [sp, #0xC]

@ 609  : 
@ 610  :          /*
@ 611  :         y5 == y4 >> 1@
@ 612  :         y3 == y4a >> 1@
@ 613  :         */
@ 614  : 
@ 615  :         ls_signbit==y4&0x8000@

    and       r2, r9, #2, 18

@ 616  :         y5 == (y4 >> 1) - ls_signbit@

    rsb       r0, r2, r9, asr #1

@ 617  :         y5 == y5 & ~0x8000@

    bic       r1, r0, #2, 18

@ 618  :         y5 == y5 | ls_signbit@
@ 619  : 
@ 620  :         ls_signbit==y4a&0x8000@

    ldr       r0, [sp, #0x14]
    sub       r8, r4, r3
    and       r3, r0, #2, 18

@ 621  :         y3 == (y4a >> 1) - ls_signbit@

    rsb       r0, r3, r0, asr #1
    orr       r4, r1, r2

@ 622  :         y3 == y3 & ~0x8000@

    bic       r1, r0, #2, 18

@ 623  :         y3 == y3 | ls_signbit@
@ 624  : 
@ 625  :        // fourth stage
@ 626  : // Change 4:
@ 627  : 
@ 628  :         x4 +== y5@
@ 629  :         x5 +== y5@
@ 630  :         x4a +== y3@
@ 631  :         x5a +== y3@
@ 632  : 
@ 633  :         /*
@ 634  : 
@ 635  :         _blk0 [i] == (x7 + x4) >> 6@
@ 636  :        _blk1 [i] == (x6 + x4a) >> 6@
@ 637  :        _blk2 [i] == (x0 + x5a) >> 6@
@ 638  :        _blk3 [i] == (x8 + x5) >> 6@
@ 639  :        _blk4 [i] == (x8 - x5) >> 6@
@ 640  :        _blk5 [i] == (x0 - x5a) >> 6@
@ 641  :        _blk6 [i] == (x6 - x4a) >> 6@
@ 642  :        _blk7 [i] == (x7 - x4) >> 6@
@ 643  : 
@ 644  :   */
@ 645  : 
@ 646  :         j == i<<1@

    orr       r2, r1, r3
    ldr       r1, [sp, #0x20]
    add       r9, r7, r4
    add       r4, r6, r4
    add       r6, r11, r2

@ 647  : 
@ 648  :         // blk0
@ 649  :         b0 == (x7 + x4)@

    ldr       r11, [sp, #8]
    mov       r0, r12, lsl #1
    add       r5, r10, r2
    add       r3, r11, r9
    add       r10, r1, r0, lsl #1

@ 650  :         b1 == (b0 + 0x8000)>>22@
@ 651  :         b0 == ((I16_WMV)b0)>>6@
@ 652  :         blk16[ j + 0 + 0*8] == b0@

    mov       r0, r3, lsl #16
    ldr       r7, [sp, #0xC]
    mov       r2, r0, asr #22

@ 653  :         blk16[ j + 1 + 0*8] == b1@
@ 654  : 
@ 655  :        // blk1
@ 656  :         b0 == (x6 + x4a)@

    
    add       r0, r3, #2, 18
    strh      r2, [r10]
    mov       r1, r0, asr #22
    
    add       r3, r6, r7

@ 657  :         b1 == (b0 + 0x8000)>>22@
@ 658  :         b0 == ((I16_WMV)b0)>>6@
@ 659  :         blk16[ j + 0 + 1*8] == b0@

    mov       r0, r3, lsl #16
    strh      r1, [r10, #2]
    mov       r2, r0, asr #22
    add       r0, r3, #2, 18
    strh      r2, [r10, #0x10]
    mov       r1, r0, asr #22
    

@ 660  :         blk16[ j + 1 + 1*8] == b1@

    

@ 661  : 
@ 662  :         // blk2
@ 663  :         b0 == (x0 + x5a)@

    add       r3, r5, lr

@ 664  :         b1 == (b0 + 0x8000)>>22@
@ 665  :         b0 == ((I16_WMV)b0)>>6@
@ 666  :         blk16[ j + 0 + 2*8] == b0@

    mov       r0, r3, lsl #16
    strh      r1, [r10, #0x12]
    mov       r2, r0, asr #22
    add       r0, r3, #2, 18
    strh      r2, [r10, #0x20]
    mov       r1, r0, asr #22
    

@ 667  :         blk16[ j + 1 + 2*8] == b1@

    

@ 668  : 
@ 669  :         // blk3
@ 670  :         b0 == (x8 + x5)@

    add       r3, r8, r4

@ 671  :         b1 == (b0 + 0x8000)>>22@
@ 672  :         b0 == ((I16_WMV)b0)>>6@
@ 673  :         blk16[ j + 0 + 3*8] == b0@

    mov       r0, r3, lsl #16
    strh      r1, [r10, #0x22]
    mov       r2, r0, asr #22
    add       r0, r3, #2, 18
    strh      r2, [r10, #0x30]
    mov       r1, r0, asr #22
    

@ 674  :         blk16[ j + 1 + 3*8] == b1@

    

@ 675  : 
@ 676  :         // blk4
@ 677  :         b0 == (x8 - x5)@

    sub       r3, r8, r4

@ 678  :         b1 == (b0 + 0x8000)>>22@
@ 679  :         b0 == ((I16_WMV)b0)>>6@
@ 680  :         blk16[ j + 0 + 4*8] == b0@

    mov       r0, r3, lsl #16
    strh      r1, [r10, #0x32]
    mov       r2, r0, asr #22
    add       r0, r3, #2, 18
    strh      r2, [r10, #0x40]
    mov       r1, r0, asr #22
    

@ 681  :         blk16[ j + 1 + 4*8] == b1@

    

@ 682  : 
@ 683  :         // blk5
@ 684  :         b0 == (x0 - x5a)@

    sub       r3, lr, r5

@ 685  :         b1 == (b0 + 0x8000)>>22@
@ 686  :         b0 == ((I16_WMV)b0)>>6@
@ 687  :         blk16[ j + 0 + 5*8] == b0@

    mov       r0, r3, lsl #16
    strh      r1, [r10, #0x42]
    mov       r2, r0, asr #22
    add       r0, r3, #2, 18
    strh      r2, [r10, #0x50]
    mov       r1, r0, asr #22
    

@ 688  :         blk16[ j + 1 + 5*8] == b1@
@ 689  : 
@ 690  :        // blk6
@ 691  :         b0 == (x6 - x4a)@

    sub       r3, r7, r6
    

@ 692  :         b1 == (b0 + 0x8000)>>22@
@ 693  :         b0 == ((I16_WMV)b0)>>6@
@ 694  :         blk16[ j + 0 + 6*8] == b0@

    mov       r0, r3, lsl #16
    strh      r1, [r10, #0x52]
    mov       r2, r0, asr #22
    add       r0, r3, #2, 18
    strh      r2, [r10, #0x60]
    mov       r1, r0, asr #22
    

@ 695  :         blk16[ j + 1 + 6*8] == b1@
@ 696  : 
@ 697  :       // blk7
@ 698  :         b0 == (x7 - x4)@

    sub       r3, r11, r9
    

@ 699  :         b1 == (b0 + 0x8000)>>22@
@ 700  :         b0 == ((I16_WMV)b0)>>6@
@ 701  :         blk16[ j + 0 + 7*8] == b0@

    mov       r0, r3, lsl #16
    strh      r1, [r10, #0x62]
    mov       r2, r0, asr #22
    add       r0, r3, #2, 18
    strh      r2, [r10, #0x70]
    mov       r1, r0, asr #22
    

@ 702  :         blk16[ j + 1 + 7*8] == b1@

    strh      r1, [r10, #0x72]
    ldr       r2, [sp]
    add       r12, r12, #1
    cmp       r12, #4
    add       r2, r2, #4
    str       r2, [sp]
    blt       L37420

@ 703  : 
@ 704  : 
@ 705  :     }
@ 706  :     FUNCTION_PROFILE_STOP(&fpDecode)@
@ 707  : 
@ 708  : }

    add       sp, sp, #0x4A, 30
    ldmia     sp!, {r4 - r12, pc}
M37422:
    WMV_ENTRY_END
    @ENDP  @ g_IDCTDec16_WMV3

    

    WMV_LEAF_ENTRY g_8x4IDCTDec_WMV3

@ 720  : {

    stmdb     sp!, {r4 - r11, lr}
    FRAME_PROFILE_COUNT
    sub       sp, sp, #0x28
M37497:
    mov       r8, r2

@ 721  : 
@ 722  :    I32_WMV * piDst == piDstBuf->i32 + (iHalf * 16)@

    add       r2, r0, r3, lsl #6

@ 723  :    I32_WMV *blk32 == piDst@

    mov       r0, r2
    str       r2, [sp, #0x24]
    str       r0, [sp]

@ 724  :     I32_WMV i @
@ 725  :    I32_WMV* blk0@
@ 726  :    I32_WMV* blk1@
@ 727  :    I32_WMV* blk2@
@ 728  :    I32_WMV* blk3@
@ 729  :     I32_WMV * blk32Next@
@ 730  :   
@ 731  : 
@ 732  :     const I32_WMV  * piSrc0 == rgiCoefReconBuf->i32@
@ 733  :     I32_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, y3, x4a, x5a, b0,c0,b1,c1,ls_signbit@
@ 734  : 
@ 735  :     I32_WMV iOffsetToNextRowForDCTHalf == iOffsetToNextRowForDCT>>1@

    mov       r0, r1, asr #1
    mov       lr, r0, lsl #2
    mov       r0, r1, lsl #2
    str       lr, [sp, #0x14]
    mov       r1, #2
    str       r0, [sp, #0x20]
    str       r1, [sp, #0x10]
L36919:

@ 736  : 
@ 737  :        FUNCTION_PROFILE(fpDecode)@
@ 738  : 
@ 739  : 
@ 740  : 
@ 741  : 
@ 742  :     piSrc0 == rgiCoefReconBuf->i32@
@ 743  : 
@ 744  : 
@ 745  :    FUNCTION_PROFILE_START(&fpDecode,IDCT_NT_PROFILE)@
@ 746  :     for ( i == 0@ i < 2@ i++, blk32 +== iOffsetToNextRowForDCT)
@ 747  :     {
@ 748  : 
@ 749  :         blk32Next == blk32 + iOffsetToNextRowForDCTHalf@
@ 750  : 
@ 751  :        x4 == piSrc0[ i +1*2 ]@      
@ 752  :        x3 == piSrc0[ i +2*2 ]@

    ldr       r0, [r8, #0x10]
    ldr       r6, [r8, #8]
    str       r0, [sp, #0x18]

@ 753  :        x7 == piSrc0[ i +3*2 ]@
@ 754  :        x1 == piSrc0[ i +4*2 ]@
@ 755  :        x6 == piSrc0[ i +5*2 ]@
@ 756  :        x2 == piSrc0[ i +6*2 ]@      

    ldr       r0, [r8, #0x30]
    ldr       lr, [r8, #0x18]
    str       r0, [sp, #0x1C]

@ 757  :        x5 == piSrc0[ i +7*2 ]@
@ 758  :         x0 == piSrc0[ i +0*2 ]@ /* for proper rounding */
@ 759  : 
@ 760  :        x1 == x1 * W0@

    ldr       r0, [r8, #0x20]
    ldr       r10, [r8, #0x28]
    add       r1, r0, r0, lsl #1
    ldr       r3, [r8, #0x38]

@ 761  :         x0 == x0 * W0 + (4+(4<<16))@ /* for proper rounding */

    ldr       r0, [r8], #4
    mov       r11, r1, lsl #2
    add       r1, r0, r0, lsl #1
    mov       r2, r1, lsl #2
    add       r0, r2, #1, 14
    add       r9, r0, #4

@ 762  : 
@ 763  :         // zeroth stage
@ 764  :         y3 == x4 + x5@

    add       r2, r3, r6

@ 765  :        x8 == W3 * y3@

    rsb       r1, r2, r2, lsl #4

@ 766  :        x4a == x8 - W3pW5 * x5@

    add       r0, r3, r3, lsl #1
    sub       r4, r1, r0, lsl #3

@ 767  :        x5a == x8 - W3_W5 * x4@

    add       r0, r6, r6, lsl #1
    sub       r5, r1, r0, lsl #1

@ 768  :        x8 == W7 * y3@

    mov       r2, r2, lsl #2

@ 769  :        x4 == x8 + W1_W7 * x4@

    add       r0, r6, r6, lsl #1
    add       r6, r2, r0, lsl #2

@ 770  :        x5 == x8 - W1pW7 * x5@

    add       r0, r3, r3, lsl #2
    sub       r7, r2, r0, lsl #2

@ 771  : 
@ 772  :        // first stage
@ 773  :         y3 == x6 + x7@

    add       r3, r10, lr

@ 774  :        x8 == W7 * y3@
@ 775  :        x4a -== x8 + W1_W7 * x6@

    add       r0, r10, r10, lsl #1
    sub       r1, r4, r0, lsl #2
    mov       r2, r3, lsl #2
    sub       r0, r1, r2
    str       r0, [sp, #8]

@ 776  :        x5a +== x8 - W1pW7 * x7@

    add       r0, lr, lr, lsl #2
    sub       r1, r5, r0, lsl #2
    add       r0, r1, r2
    str       r0, [sp, #4]

@ 777  :        x8 == W3 * y3@

    rsb       r2, r3, r3, lsl #4

@ 778  :        x4 +== x8 - W3_W5 * x6@

    add       r0, r10, r10, lsl #1
    sub       r1, r2, r0, lsl #1
    add       r0, r6, r1
    str       r0, [sp, #0xC]

@ 779  :        x5 +== x8 - W3pW5 * x7@

    add       r0, lr, lr, lsl #1
    sub       r1, r2, r0, lsl #3

@ 780  : 
@ 781  :        // second stage 
@ 782  :        x8 == x0 + x1@
@ 783  :        x0 -== x1@
@ 784  : 
@ 785  :        x1 == x2@
@ 786  :        x2 == W6 * x3 - W2 * x2@  // simplify?

    ldr       r2, [sp, #0x18]
    add       r10, r7, r1
    add       r0, r2, r2, lsl #1
    mov       r1, r0, lsl #1
    ldr       r0, [sp, #0x1C]
    add       r4, r9, r11
    sub       r3, r1, r0, lsl #4

@ 787  :        x3 == W6 * x1 + W2A * x3@

    add       r0, r0, r0, lsl #1
    mov       r1, r0, lsl #1

@ 788  : 
@ 789  :        // third stage
@ 790  :        x7 == x8 + x3@
@ 791  :        x8 -== x3@
@ 792  :        x3 == x0 + x2@
@ 793  :        x0 -== x2@
@ 794  : 
@ 795  :        // fourth stage
@ 796  : /*
@ 797  :        blk [0] == (I16_WMV) ((x7 + x4) >> 3)@
@ 798  :        blk [1] == (I16_WMV) ((x3 + x4a) >> 3)@
@ 799  :        blk [2] == (I16_WMV) ((x0 + x5a) >> 3)@
@ 800  :        blk [3] == (I16_WMV) ((x8 + x5) >> 3)@
@ 801  :        blk [4] == (I16_WMV) ((x8 - x5) >> 3)@
@ 802  :        blk [5] == (I16_WMV) ((x0 - x5a) >> 3)@
@ 803  :        blk [6] == (I16_WMV) ((x3 - x4a) >> 3)@
@ 804  :        blk [7] == (I16_WMV) ((x7 - x4) >> 3)@
@ 805  : */
@ 806  : 
@ 807  :         // blk [0,1]
@ 808  :         b0 == x7 + x4@

    ldr       r0, [sp, #0xC]
    add       r2, r1, r2, lsl #4
    sub       r5, r9, r11
    add       r11, r4, r2
    add       r9, r5, r3
    sub       r6, r5, r3
    add       r5, r11, r0

@ 809  :         b1 == (b0 + 0x8000)>>19@
@ 810  :         b0 == ((I16_WMV)b0)>>3@
@ 811  : 
@ 812  :         c0 == x3 + x4a@

    ldr       r0, [sp, #8]
    sub       r7, r4, r2
    add       r4, r0, r9

@ 813  :         c1 == (c0 + 0x8000)>>19@
@ 814  :         c0 == ((I16_WMV)c0)>>3@
@ 815  : 
@ 816  :         blk32[0] == (c0<<16) + b0@

    mov       r0, r4, lsl #16
    mov       r1, r0, asr #16
    mov       r2, r1, asr #3
    mov       r3, r2, lsl #16
    mov       r0, r5, lsl #16
    add       r1, r3, r0, asr #19

@ 817  :         blk32Next[0] == (c1<<16) + b1@

    ldr       r0, [sp, #0x14]
    ldr       r3, [sp]
    mov       lr, r0
    add       r0, r4, #2, 18
    str       r1, [r3]
    mov       r1, r0, asr #19
    add       r0, r5, #2, 18
    mov       r2, r1, lsl #16
    add       r1, r2, r0, asr #19

@ 818  : 
@ 819  :         // blk [2,3]
@ 820  :         b0 == x0 + x5a@

    ldr       r0, [sp, #4]
    str       r1, [lr, +r3]!
    add       r5, r0, r6

@ 821  :         b1 == (b0 + 0x8000)>>19@
@ 822  :         b0 == ((I16_WMV)b0)>>3@
@ 823  : 
@ 824  :         c0 == x8 + x5@

    add       r4, r7, r10

@ 825  :         c1 == (c0 + 0x8000)>>19@
@ 826  :         c0 == ((I16_WMV)c0)>>3@
@ 827  : 
@ 828  :         blk32[1] == (c0<<16) + b0@

    mov       r0, r4, lsl #16
    mov       r1, r0, asr #16
    mov       r2, r1, asr #3
    mov       r3, r2, lsl #16
    mov       r0, r5, lsl #16
    add       r1, r3, r0, asr #19
    ldr       r0, [sp]
    str       r1, [r0, #4]
    add       r0, r4, #2, 18
    mov       r1, r0, asr #19

@ 829  :         blk32Next[1] == (c1<<16) + b1@

    mov       r2, r1, lsl #16
    add       r0, r5, #2, 18
    add       r1, r2, r0, asr #19

@ 830  : 
@ 831  :         // blk [4,5]
@ 832  :         b0 == x8 - x5@
@ 833  :         b1 == (b0 + 0x8000)>>19@
@ 834  :         b0 == ((I16_WMV)b0)>>3@
@ 835  : 
@ 836  :         c0 == x0 - x5a@

    ldr       r0, [sp, #4]
    str       r1, [lr, #4]
    sub       r5, r7, r10
    sub       r4, r6, r0

@ 837  :         c1 == (c0 + 0x8000)>>19@
@ 838  :         c0 == ((I16_WMV)c0)>>3@
@ 839  : 
@ 840  :         blk32[2] == (c0<<16) + b0@

    mov       r0, r4, lsl #16
    mov       r1, r0, asr #16
    mov       r2, r1, asr #3
    mov       r3, r2, lsl #16
    ldr       r2, [sp]
    mov       r0, r5, lsl #16
    add       r1, r3, r0, asr #19
    str       r1, [r2, #8]
    add       r0, r4, #2, 18
    mov       r1, r0, asr #19

@ 841  :         blk32Next[2] == (c1<<16) + b1@

    mov       r2, r1, lsl #16
    add       r0, r5, #2, 18
    add       r1, r2, r0, asr #19

@ 842  : 
@ 843  :         // blk [6,7]
@ 844  :         b0 == x3 - x4a@

    ldr       r0, [sp, #8]
    str       r1, [lr, #8]
    sub       r5, r9, r0

@ 845  :         b1 == (b0 + 0x8000)>>19@
@ 846  :         b0 == ((I16_WMV)b0)>>3@
@ 847  : 
@ 848  :         c0 == x7 - x4@

    ldr       r0, [sp, #0xC]
    sub       r4, r11, r0

@ 849  :         c1 == (c0 + 0x8000)>>19@
@ 850  :         c0 == ((I16_WMV)c0)>>3@
@ 851  : 
@ 852  :         blk32[3] == (c0<<16) + b0@

    mov       r0, r4, lsl #16
    mov       r1, r0, asr #16
    mov       r2, r1, asr #3
    mov       r3, r2, lsl #16
    mov       r0, r5, lsl #16
    add       r1, r3, r0, asr #19
    ldr       r3, [sp]
    add       r0, r4, #2, 18
    str       r1, [r3, #0xC]
    mov       r1, r0, asr #19
    add       r0, r5, #2, 18

@ 853  :         blk32Next[3] == (c1<<16) + b1@

    mov       r2, r1, lsl #16
    add       r1, r2, r0, asr #19
    ldr       r0, [sp, #0x20]
    str       r1, [lr, #0xC]
    add       r3, r3, r0
    ldr       r0, [sp, #0x10]
    str       r3, [sp]
    sub       r0, r0, #1
    str       r0, [sp, #0x10]
    cmp       r0, #0
    bhi       L36919

@ 854  :     }
@ 855  : 
@ 856  :     blk0 == piDst@
@ 857  :     blk1 == blk0 + iOffsetToNextRowForDCTHalf@

    ldr       r1, [sp, #0x14]
    ldr       r2, [sp, #0x24]
    add       r9, r1, r2

@ 858  :     blk2 == blk1 + iOffsetToNextRowForDCTHalf@

    add       r0, r1, r9
    str       r9, [sp, #0x14]

@ 859  :     blk3 == blk2 + iOffsetToNextRowForDCTHalf@

    add       r1, r1, r0

@ 860  : 
@ 861  :     for (i == 0@ i < 4@ i++) {

    sub       r5, r0, r9
    mov       r0, #4
    sub       r4, r2, r9
    str       r0, [sp, #0x10]
    sub       r6, r1, r9
L36930:

@ 862  :         x4 == blk0[i]@
@ 863  :         x5 == blk1[i]@
@ 864  :         x6 == blk2[i]@

    mov       r7, r5
    ldr       r2, [r9]
    ldr       r0, [r7, +r9]!
    mov       r11, r4
    ldr       r1, [r11, +r9]!
    str       r7, [sp, #0x24]

@ 865  :         x7 == blk3[i]@

    mov       r7, r6
    ldr       r3, [r7, +r9]!

@ 866  : 
@ 867  :         x3 == (x4 - x6)@ 

    sub       r9, r1, r0
    str       r7, [sp, #0x20]

@ 868  :         x6 +== x4@

    add       r7, r0, r1

@ 869  : 
@ 870  :         x4 == 8 * x6 + 32 + (32<<16)@ //rounding

    add       r0, r7, #1, 14
    add       r1, r0, #4

@ 871  :         x8 == 8 * x3 + 32 + (32<<16)@   //rounding
@ 872  : //simplify following to 3 multiplies
@ 873  :         x5a == 11 * x5 + 5 * x7@

    add       r0, r2, r2, lsl #2
    mov       r8, r1, lsl #3
    add       r1, r2, r0, lsl #1
    add       r0, r3, r3, lsl #2
    add       lr, r1, r0

@ 874  :         x5 == 5 * x5 - 11 * x7@

    add       r0, r3, r3, lsl #2
    add       r1, r3, r0, lsl #1
    add       r2, r2, r2, lsl #2
    sub       r10, r2, r1

@ 875  : 
@ 876  :     /*
@ 877  :         x4 +== (x6 >> 1)@ // guaranteed to have enough head room
@ 878  :         x8 +== (x3 >> 1)@
@ 879  :     */
@ 880  :         ls_signbit==x6&0x8000@

    and       r2, r7, #2, 18

@ 881  :         x6 == (x6 >> 1) - ls_signbit@

    rsb       r0, r2, r7, asr #1

@ 882  :         x6 == x6 & ~0x8000@

    bic       r1, r0, #2, 18

@ 883  :         x6 == x6 | ls_signbit@
@ 884  : 
@ 885  :         ls_signbit==x3&0x8000@

    and       r3, r9, #2, 18
    orr       r2, r1, r2

@ 886  :         x3 == (x3 >> 1) - ls_signbit@

    rsb       r0, r3, r9, asr #1

@ 887  :         x3 == x3 & ~0x8000@

    bic       r1, r0, #2, 18

@ 888  :         x3 == x3 | ls_signbit@
@ 889  : 
@ 890  :         x4 +== x6@

    add       r8, r8, r2
    orr       r2, r1, r3

@ 891  :         x8 +== x3@

    add       r0, r9, #1, 14

@ 892  :  
@ 893  : /*
@ 894  :         blk0[i] == (I16_WMV) ((x4 + x5a) >> 6)@
@ 895  :         blk1[i] == (I16_WMV) ((x8 + x5) >> 6)@
@ 896  :         blk2[i] == (I16_WMV) ((x8 - x5) >> 6)@
@ 897  :         blk3[i] == (I16_WMV) ((x4 - x5a) >> 6)@
@ 898  : */
@ 899  : 
@ 900  :  
@ 901  :         // blk0
@ 902  :         b0 == (x4 + x5a)@
@ 903  :         b1 == (b0 + 0x8000)>>22@
@ 904  :         b0 == ((I16_WMV)b0)>>6@
@ 905  :         *(((I16_WMV *)(blk0 + i)) + 0 ) == b0@
@ 906  :         *(((I16_WMV *)(blk0 + i)) + 1 ) == b1@
@ 907  : 
@ 908  :         // blk1
@ 909  :         b0 == (x8 + x5)@
@ 910  :         b1 == (b0 + 0x8000)>>22@
@ 911  :         b0 == ((I16_WMV)b0)>>6@
@ 912  :         *(((I16_WMV *)(blk1 + i)) + 0 ) == b0@

    ldr       r9, [sp, #0x14]
    add       r1, r0, #4
    add       r3, lr, r8
    add       r7, r2, r1, lsl #3
    mov       r0, r3, lsl #16
    mov       r1, r0, asr #16
    add       r0, r3, #2, 18
    mov       r2, r1, asr #6
    mov       r1, r0, asr #22
    strh      r2, [r11]
    mov       r2, r1, lsl #16
    mov       r0, r2, asr #16
    strh      r0, [r11, #2]
    add       r3, r7, r10
    mov       r0, r3, lsl #16
    mov       r1, r0, asr #16
    add       r0, r3, #2, 18
    mov       r2, r1, asr #6
    mov       r1, r0, asr #22
    strh      r2, [r9]

@ 913  :         *(((I16_WMV *)(blk1 + i)) + 1 ) == b1@

    mov       r2, r1, lsl #16
    mov       r0, r2, asr #16
    strh      r0, [r9, #2]

@ 914  : 
@ 915  :         // blk2
@ 916  :         b0 == (x8 - x5)@

    sub       r3, r7, r10

@ 917  :         b1 == (b0 + 0x8000)>>22@
@ 918  :         b0 == ((I16_WMV)b0)>>6@
@ 919  :         *(((I16_WMV *)(blk2 + i)) + 0 ) == b0@

    mov       r0, r3, lsl #16
    ldr       r7, [sp, #0x24]
    mov       r1, r0, asr #16
    add       r0, r3, #2, 18
    mov       r2, r1, asr #6
    mov       r1, r0, asr #22
    strh      r2, [r7]

@ 920  :         *(((I16_WMV *)(blk2 + i)) + 1 ) == b1@

    mov       r2, r1, lsl #16
    mov       r0, r2, asr #16

@ 921  : 
@ 922  :         // blk3
@ 923  :         b0 == (x4 - x5a)@

    sub       r3, r8, lr
    strh      r0, [r7, #2]

@ 924  :         b1 == (b0 + 0x8000)>>22@
@ 925  :         b0 == ((I16_WMV)b0)>>6@
@ 926  :         *(((I16_WMV *)(blk3 + i)) + 0 ) == b0@

    mov       r0, r3, lsl #16
    ldr       r7, [sp, #0x20]
    mov       r1, r0, asr #16
    add       r0, r3, #2, 18
    mov       r2, r1, asr #6
    mov       r1, r0, asr #22
    strh      r2, [r7]

@ 927  :         *(((I16_WMV *)(blk3 + i)) + 1 ) == b1@

    mov       r2, r1, lsl #16
    mov       r0, r2, asr #16
    strh      r0, [r7, #2]
    add       r9, r9, #4
    ldr       r0, [sp, #0x10]
    str       r9, [sp, #0x14]
    sub       r0, r0, #1
    str       r0, [sp, #0x10]
    cmp       r0, #0
    bhi       L36930

@ 928  : 
@ 929  :     } 
@ 930  :     FUNCTION_PROFILE_STOP(&fpDecode)@
@ 931  : }

    add       sp, sp, #0x28
    ldmia     sp!, {r4 - r11, pc}
M37498:
    WMV_ENTRY_END
    @ENDP  @ g_8x4IDCTDec_WMV3


    WMV_LEAF_ENTRY g_4x8IDCTDec_WMV3

@ 938  : {

    stmdb     sp!, {r4 - r11, lr}
    FRAME_PROFILE_COUNT
    sub       sp, sp, #0x54
M37580:
    mov       r7, r2
    str       r7, [sp, #4]

@ 939  :    
@ 940  :    I32_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x4a, x5a@
@ 941  :     I32_WMV  y3, y4, y5, y4a, b0,c0,b1,c1,ls_signbit@
@ 942  :     
@ 943  :    I32_WMV * piDst == piDstBuf->i32 + (iHalf*2)@

    add       r6, r0, r3, lsl #3

@ 944  :    I32_WMV * blk32 == piDst@
@ 945  :     I32_WMV * blk32Next@
@ 946  :     I32_WMV i @
@ 947  :    I32_WMV* blk0@
@ 948  :    I32_WMV* blk1@
@ 949  :    I32_WMV* blk2@
@ 950  :    I32_WMV* blk3@
@ 951  :    I32_WMV* blk4@
@ 952  :    I32_WMV* blk5@
@ 953  :    I32_WMV* blk6@
@ 954  :    I32_WMV* blk7@
@ 955  :   
@ 956  : 
@ 957  :      const I32_WMV  * piSrc0 == rgiCoefReconBuf->i32@
@ 958  :     I32_WMV iOffsetToNextRowForDCTHalf == iOffsetToNextRowForDCT>>1@

    mov       r0, r1, asr #1
    str       r6, [sp, #0x1C]
    mov       r2, r6
    mov       r8, r0, lsl #2
    str       r2, [sp]
    str       r8, [sp, #8]
    mov       r11, r1, lsl #2
    mov       lr, #4
L36988:

@ 959  : 
@ 960  : 
@ 961  :      FUNCTION_PROFILE(fpDecode)@
@ 962  : 
@ 963  : 
@ 964  :    // memcpy(dsttmp, rgiCoefReconBuf->i32, 64)@
@ 965  :    // piSrc0 == (I32_WMV*) dsttmp@
@ 966  : 
@ 967  : 
@ 968  :     piSrc0 == rgiCoefReconBuf->i32@
@ 969  : 
@ 970  : 
@ 971  :     FUNCTION_PROFILE_START(&fpDecode,IDCT_NT_PROFILE)@
@ 972  : 
@ 973  :     for ( i == 0@ i < 4@ i++, blk32 +== iOffsetToNextRowForDCT){
@ 974  : 
@ 975  :         blk32Next == blk32 + iOffsetToNextRowForDCTHalf@
@ 976  :         x4 == piSrc0[ i +0*4]@

    ldr       r4, [r7]
    sub       lr, lr, #1

@ 977  :         x5 == piSrc0[ i +1*4]@
@ 978  :         x6 == piSrc0[ i +2*4]@

    ldr       r3, [r7, #0x20]
    cmp       lr, #0
    ldr       r5, [r7, #0x10]

@ 979  :         x7 == piSrc0[ i +3*4]@
@ 980  : 
@ 981  : 
@ 982  :         x0 == 17 * (x4 + x6) + 4 + (4<<16)@ //rounding

    add       r0, r3, r4
    ldr       r6, [r7, #0x30]
    add       r1, r0, r0, lsl #4

@ 983  :         x1 == 17 * (x4 - x6)  + 4 + (4<<16)@ //rounding

    sub       r0, r4, r3
    add       r2, r1, #1, 14
    add       r1, r0, r0, lsl #4
    add       r10, r2, #4

@ 984  :         x8 == 10 * (x5 + x7)@

    add       r0, r6, r5
    add       r2, r1, #1, 14
    add       r1, r0, r0, lsl #2
    add       r9, r2, #4
    mov       r2, r1, lsl #1

@ 985  :         x2 == x8 + 12 * x5@
@ 986  :         x3 == x8 - 32 * x7@

    sub       r6, r2, r6, lsl #5
    add       r0, r5, r5, lsl #1
    add       r7, r2, r0, lsl #2

@ 987  : 
@ 988  : /*
@ 989  :         blk[0] == (I16_WMV)((x0 + x2) >> 3)@
@ 990  :         blk[1] == (I16_WMV)((x1 + x3) >> 3)@
@ 991  :         blk[2] == (I16_WMV)((x1 - x3) >> 3)@
@ 992  :         blk[3] == (I16_WMV)((x0 - x2) >> 3)@
@ 993  : */
@ 994  : 
@ 995  :         // blk [0,1]
@ 996  :         b0 == x0 + x2@
@ 997  :         b1 == (b0 + 0x8000)>>19@
@ 998  :         b0 == ((I16_WMV)b0)>>3@
@ 999  : 
@ 1000 :         c0 == x1 + x3@

    add       r4, r6, r9

@ 1001 :         c1 == (c0 + 0x8000)>>19@
@ 1002 :         c0 == ((I16_WMV)c0)>>3@
@ 1003 : 
@ 1004 :         blk32[0] == (c0<<16) + b0@

    mov       r0, r4, lsl #16
    mov       r1, r0, asr #16
    mov       r2, r1, asr #3
    add       r5, r7, r10
    mov       r3, r2, lsl #16
    mov       r0, r5, lsl #16
    add       r1, r3, r0, asr #19

@ 1005 :         blk32Next[0] == (c1<<16) + b1@

    ldr       r0, [sp, #8]
    ldr       r3, [sp]
    mov       r8, r0
    str       r1, [r3]
    add       r0, r4, #2, 18
    mov       r1, r0, asr #19
    mov       r2, r1, lsl #16
    add       r0, r5, #2, 18

@ 1006 : 
@ 1007 :         // blk [2,3]
@ 1008 :         b0 == x1 - x3@
@ 1009 :         b1 == (b0 + 0x8000)>>19@
@ 1010 :         b0 == ((I16_WMV)b0)>>3@
@ 1011 : 
@ 1012 :         c0 == x0 - x2@

    sub       r4, r10, r7
    ldr       r7, [sp, #4]
    add       r1, r2, r0, asr #19

@ 1013 :         c1 == (c0 + 0x8000)>>19@
@ 1014 :         c0 == ((I16_WMV)c0)>>3@
@ 1015 : 
@ 1016 :         blk32[1] == (c0<<16) + b0@

    mov       r0, r4, lsl #16
    str       r1, [r8, +r3]!
    mov       r1, r0, asr #16
    mov       r2, r1, asr #3
    sub       r5, r9, r6
    mov       r3, r2, lsl #16
    mov       r0, r5, lsl #16
    add       r1, r3, r0, asr #19
    ldr       r3, [sp]
    add       r0, r4, #2, 18
    str       r1, [r3, #4]
    mov       r1, r0, asr #19

@ 1017 :         blk32Next[1] == (c1<<16) + b1@

    mov       r2, r1, lsl #16
    add       r0, r5, #2, 18
    add       r1, r2, r0, asr #19
    add       r7, r7, #4
    str       r1, [r8, #4]
    add       r3, r3, r11
    str       r7, [sp, #4]
    str       r3, [sp]
    bhi       L36988

@ 1018 :     }
@ 1019 : 
@ 1020 :     blk0 == piDst@
@ 1021 :     blk1 == blk0 + iOffsetToNextRowForDCTHalf@

    ldr       r5, [sp, #8]
    ldr       r6, [sp, #0x1C]
    add       r9, r5, r6

@ 1022 :     blk2 == blk1 + iOffsetToNextRowForDCTHalf@

    add       r0, r5, r9
    str       r9, [sp]

@ 1023 :     blk3 == blk2 + iOffsetToNextRowForDCTHalf@

    add       r1, r5, r0

@ 1024 :     blk4 == blk3 + iOffsetToNextRowForDCTHalf@

    add       r2, r5, r1

@ 1025 :     blk5 == blk4 + iOffsetToNextRowForDCTHalf@

    add       r3, r5, r2

@ 1026 :     blk6 == blk5 + iOffsetToNextRowForDCTHalf@
@ 1027 :     blk7 == blk6 + iOffsetToNextRowForDCTHalf@
@ 1028 : 
@ 1029 :     for (i == 0@ i < 2@ i++)

    sub       r11, r0, r9
    sub       r0, r1, r9
    add       r4, r5, r3
    str       r0, [sp, #0x20]
    add       r5, r5, r4
    mov       r0, #2
    sub       r6, r6, r9
    str       r0, [sp, #0x1C]
    sub       r7, r2, r9
    sub       lr, r3, r9
    sub       r8, r4, r9
    sub       r10, r5, r9
L36995:

@ 1030 :     {
@ 1031 :         x0 == blk0[i] * 6 + 32 + (32<<16) /* rounding */@

    mov       r1, r6
    ldr       r0, [r1, +r9]!

@ 1032 :        x1 == blk4[i] * 6@

    mov       r4, r7
    str       r1, [sp, #0x14]
    add       r1, r0, r0, lsl #1
    mov       r2, r1, lsl #1
    add       r0, r2, #2, 12
    add       r1, r0, #0x20
    ldr       r0, [r4, +r9]!
    str       r1, [sp, #0x28]
    add       r1, r0, r0, lsl #1
    str       r4, [sp, #0x44]
    mov       r2, r1, lsl #1

@ 1033 :        x2 == blk6[i]@

    mov       r4, r8
    str       r2, [sp, #0x24]
    ldr       r0, [r4, +r9]!

@ 1034 :        x3 == blk2[i]@

    mov       r9, r11
    ldr       r2, [sp]
    str       r0, [sp, #0x2C]
    ldr       r0, [r9, +r2]!
    str       r4, [sp, #0x4C]

@ 1035 :        x4 == blk1[i]@
@ 1036 :        x5 == blk7[i]@

    mov       r4, r10
    ldr       r5, [r2]
    str       r9, [sp, #0x40]
    ldr       r9, [r4, +r2]!
    str       r0, [sp, #0xC]
    str       r4, [sp, #0x50]

@ 1037 :        x6 == blk5[i]@

    mov       r4, lr
    ldr       r0, [r4, +r2]!
    str       r0, [sp, #4]

@ 1038 :        x7 == blk3[i]@

    ldr       r0, [sp, #0x20]
    str       r4, [sp, #0x48]
    mov       r1, r0
    ldr       r0, [r1, +r2]!

@ 1039 : 
@ 1040 :         // zeroth stage
@ 1041 :         y4a == x4 + x5@
@ 1042 :        x8 == 7 * y4a@
@ 1043 :        x4a == x8 - 12 * x5@
@ 1044 :        x5a == x8 - 3 * x4@
@ 1045 :        x8 == 2 * y4a@

    add       r2, r9, r5
    mov       r2, r2, lsl #1
    str       r0, [sp, #8]
    add       r0, r9, r5
    str       r0, [sp, #0x30]
    str       r1, [sp, #0x18]
    rsb       r1, r0, r0, lsl #3
    add       r0, r9, r9, lsl #1
    sub       r3, r1, r0, lsl #2
    add       r0, r5, r5, lsl #1
    sub       r4, r1, r0

@ 1046 :        x4 == x8 + 6 * x4@
@ 1047 :        x5 == x8 - 10 * x5@
@ 1048 : 
@ 1049 :        // first stage
@ 1050 :         y4 == x6 + x7@

    ldr       r1, [sp, #4]
    add       r0, r5, r5, lsl #1
    add       r5, r2, r0, lsl #1
    add       r0, r9, r9, lsl #2
    sub       r9, r2, r0, lsl #1
    ldr       r0, [sp, #8]
    add       r0, r0, r1

@ 1051 :        x8 == 2 * y4@

    mov       r2, r0, lsl #1
    str       r0, [sp, #0x10]

@ 1052 :        x4a -== x8 + 6 * x6@

    add       r0, r1, r1, lsl #1
    sub       r1, r3, r0, lsl #1

@ 1053 :        x5a +== x8 - 10 * x7@

    ldr       r3, [sp, #8]
    sub       r0, r1, r2
    str       r0, [sp, #0x38]
    add       r0, r3, r3, lsl #2
    sub       r1, r4, r0, lsl #1
    add       r0, r1, r2
    str       r0, [sp, #0x3C]

@ 1054 :        x8 == 7 * y4@

    ldr       r0, [sp, #0x10]
    rsb       r2, r0, r0, lsl #3

@ 1055 :        x4 +== x8 - 3 * x6@

    ldr       r0, [sp, #4]
    add       r0, r0, r0, lsl #1
    sub       r1, r2, r0
    add       r0, r5, r1
    str       r0, [sp, #0x34]

@ 1056 :        x5 +== x8 - 12 * x7@

    add       r0, r3, r3, lsl #1
    sub       r1, r2, r0, lsl #2

@ 1057 : 
@ 1058 :        // second stage 
@ 1059 :        x8 == x0 + x1@

    ldr       r0, [sp, #0x24]
    add       r9, r9, r1
    ldr       r1, [sp, #0x28]

@ 1060 :        x0 -== x1@
@ 1061 : 
@ 1062 :        x1 == 8 * (x2 + x3)@

    ldr       r2, [sp, #0xC]
    add       r4, r0, r1
    sub       r5, r1, r0
    ldr       r1, [sp, #0x2C]
    add       r0, r2, r1
    mov       r2, r0, lsl #3

@ 1063 :        x6 == x1 - 5 * x2@

    add       r0, r1, r1, lsl #2

@ 1064 :        x1 -== 11 * x3@

    ldr       r1, [sp, #0xC]
    sub       r3, r2, r0
    add       r0, r1, r1, lsl #2
    add       r1, r1, r0, lsl #1

@ 1065 : 
@ 1066 :        // third stage
@ 1067 :        x7 == x8 + x6@

    add       r0, r4, r3
    str       r0, [sp, #4]
    sub       r2, r2, r1

@ 1068 :        x8 -== x6@

    sub       r0, r4, r3
    str       r0, [sp, #0x2C]

@ 1069 : 
@ 1070 : 
@ 1071 :        x6 == x0 - x1@

    sub       r0, r5, r2
    str       r0, [sp, #8]

@ 1072 :        x0 +== x1@

    add       r0, r5, r2
    str       r0, [sp, #0xC]

@ 1073 : 
@ 1074 :         // fractional part (guaranteed to have headroom)
@ 1075 : 
@ 1076 :         // Change 3:
@ 1077 : 
@ 1078 :         /*
@ 1079 :         y5 == y4 >> 1@
@ 1080 :         y3 == y4a >> 1@
@ 1081 :         */
@ 1082 : 
@ 1083 :        ls_signbit==y4&0x8000@

    ldr       r0, [sp, #0x10]
    and       r2, r0, #2, 18

@ 1084 :         y5 == (y4 >> 1) - ls_signbit@

    rsb       r0, r2, r0, asr #1

@ 1085 :         y5 == y5 & ~0x8000@

    bic       r1, r0, #2, 18

@ 1086 :         y5 == y5 | ls_signbit@
@ 1087 : 
@ 1088 :         ls_signbit==y4a&0x8000@

    ldr       r0, [sp, #0x30]
    orr       r4, r1, r2
    and       r3, r0, #2, 18

@ 1089 :         y3 == (y4a >> 1) - ls_signbit@

    rsb       r0, r3, r0, asr #1

@ 1090 :         y3 == y3 & ~0x8000@

    bic       r1, r0, #2, 18

@ 1091 :         y3 == y3 | ls_signbit@
@ 1092 : 
@ 1093 :        // fourth stage
@ 1094 : 
@ 1095 :         x4 +== y5@

    ldr       r0, [sp, #0x34]
    orr       r2, r1, r3
    add       r1, r0, r4

@ 1096 :         x5 +== y5@
@ 1097 :         x4a +== y3@

    ldr       r0, [sp, #0x38]
    add       r4, r9, r4
    str       r1, [sp, #0x34]
    add       r9, r0, r2

@ 1098 :         x5a +== y3@

    ldr       r0, [sp, #0x3C]
    str       r9, [sp, #0x38]
    add       r5, r0, r2

@ 1099 : 
@ 1100 :         /*
@ 1101 :         blk0 [i] == (x7 + x4) >> 6@
@ 1102 :        blk1 [i] == (x6 + x4a) >> 6@
@ 1103 :        blk2 [i] == (x0 + x5a) >> 6@
@ 1104 :        blk3 [i] == (x8 + x5) >> 6@  
@ 1105 :        blk4 [i] == (x8 - x5) >> 6@
@ 1106 :        blk5 [i] == (x0 - x5a) >> 6@
@ 1107 :        blk6 [i] == (x6 - x4a) >> 6@
@ 1108 :        blk7 [i] == (x7 - x4) >> 6@
@ 1109 :         */
@ 1110 : 
@ 1111 :                // blk0
@ 1112 :         b0 == (x7 + x4)@

    ldr       r0, [sp, #4]
    add       r3, r0, r1

@ 1113 :         b1 == (b0 + 0x8000)>>22@
@ 1114 :         b0 == ((I16_WMV)b0)>>6@
@ 1115 :         *(((I16_WMV *)(blk0 + i)) + 0 ) == b0@

    mov       r0, r3, lsl #16
    mov       r1, r0, asr #16
    ldr       r0, [sp, #0x14]
    mov       r2, r1, asr #6
    strh      r2, [r0]
    add       r0, r3, #2, 18
    mov       r1, r0, asr #22

@ 1116 :         *(((I16_WMV *)(blk0 + i)) + 1 ) == b1@

    mov       r2, r1, lsl #16
    ldr       r1, [sp, #0x14]
    mov       r0, r2, asr #16
    strh      r0, [r1, #2]

@ 1117 : 
@ 1118 :         // blk1
@ 1119 :         b0 == (x6 + x4a)@

    ldr       r0, [sp, #8]
    add       r3, r9, r0

@ 1120 :         b1 == (b0 + 0x8000)>>22@
@ 1121 :         b0 == ((I16_WMV)b0)>>6@
@ 1122 :         *(((I16_WMV *)(blk1 + i)) + 0 ) == b0@

    ldr       r9, [sp]
    mov       r0, r3, lsl #16
    mov       r1, r0, asr #16
    add       r0, r3, #2, 18
    mov       r2, r1, asr #6
    mov       r1, r0, asr #22
    strh      r2, [r9]

@ 1123 :         *(((I16_WMV *)(blk1 + i)) + 1 ) == b1@

    mov       r2, r1, lsl #16
    mov       r0, r2, asr #16
    strh      r0, [r9, #2]

@ 1124 : 
@ 1125 :         // blk2
@ 1126 :         b0 == (x0 + x5a)@

    ldr       r0, [sp, #0xC]

@ 1127 :         b1 == (b0 + 0x8000)>>22@
@ 1128 :         b0 == ((I16_WMV)b0)>>6@
@ 1129 :         *(((I16_WMV *)(blk2 + i)) + 0 ) == b0@

    ldr       r9, [sp, #0x40]
    add       r3, r5, r0
    mov       r0, r3, lsl #16
    mov       r1, r0, asr #16
    add       r0, r3, #2, 18
    mov       r2, r1, asr #6
    mov       r1, r0, asr #22
    strh      r2, [r9]

@ 1130 :         *(((I16_WMV *)(blk2 + i)) + 1 ) == b1@

    mov       r2, r1, lsl #16
    mov       r0, r2, asr #16
    strh      r0, [r9, #2]

@ 1131 : 
@ 1132 :         // blk3
@ 1133 :         b0 == (x8 + x5)@

    ldr       r9, [sp, #0x2C]
    add       r3, r9, r4

@ 1134 :         b1 == (b0 + 0x8000)>>22@
@ 1135 :         b0 == ((I16_WMV)b0)>>6@
@ 1136 :         *(((I16_WMV *)(blk3 + i)) + 0 ) == b0@

    mov       r0, r3, lsl #16
    mov       r1, r0, asr #16
    ldr       r0, [sp, #0x18]
    mov       r2, r1, asr #6
    strh      r2, [r0]
    add       r0, r3, #2, 18
    mov       r1, r0, asr #22

@ 1137 :         *(((I16_WMV *)(blk3 + i)) + 1 ) == b1@

    mov       r2, r1, lsl #16
    mov       r0, r2, asr #16
    ldr       r1, [sp, #0x18]

@ 1138 : 
@ 1139 :                 // blk4
@ 1140 :         b0 == (x8 - x5)@

    sub       r3, r9, r4

@ 1141 :         b1 == (b0 + 0x8000)>>22@
@ 1142 :         b0 == ((I16_WMV)b0)>>6@
@ 1143 :         *(((I16_WMV *)(blk4 + i)) + 0 ) == b0@

    ldr       r4, [sp, #0x44]
    strh      r0, [r1, #2]
    mov       r0, r3, lsl #16
    mov       r1, r0, asr #16
    ldr       r9, [sp]
    add       r0, r3, #2, 18
    mov       r2, r1, asr #6
    mov       r1, r0, asr #22
    strh      r2, [r4]

@ 1144 :         *(((I16_WMV *)(blk4 + i)) + 1 ) == b1@

    mov       r2, r1, lsl #16

@ 1145 : 
@ 1146 :         // blk5
@ 1147 :         b0 == (x0 - x5a)@

    ldr       r1, [sp, #0xC]
    mov       r0, r2, asr #16
    strh      r0, [r4, #2]
    sub       r3, r1, r5

@ 1148 :         b1 == (b0 + 0x8000)>>22@
@ 1149 :         b0 == ((I16_WMV)b0)>>6@
@ 1150 :         *(((I16_WMV *)(blk5 + i)) + 0 ) == b0@

    mov       r0, r3, lsl #16
    ldr       r4, [sp, #0x48]
    mov       r1, r0, asr #16
    add       r0, r3, #2, 18
    mov       r2, r1, asr #6
    mov       r1, r0, asr #22
    strh      r2, [r4]

@ 1151 :         *(((I16_WMV *)(blk5 + i)) + 1 ) == b1@

    mov       r2, r1, lsl #16

@ 1152 : 
@ 1153 :         // blk6
@ 1154 :         b0 == (x6 - x4a)@

    ldr       r1, [sp, #0x38]
    mov       r0, r2, asr #16
    ldr       r2, [sp, #8]
    strh      r0, [r4, #2]
    add       r9, r9, #4
    sub       r3, r2, r1

@ 1155 :         b1 == (b0 + 0x8000)>>22@
@ 1156 :         b0 == ((I16_WMV)b0)>>6@
@ 1157 :         *(((I16_WMV *)(blk6 + i)) + 0 ) == b0@

    ldr       r4, [sp, #0x4C]
    mov       r0, r3, lsl #16
    str       r9, [sp]
    mov       r1, r0, asr #16
    add       r0, r3, #2, 18
    mov       r2, r1, asr #6
    mov       r1, r0, asr #22
    strh      r2, [r4]

@ 1158 :         *(((I16_WMV *)(blk6 + i)) + 1 ) == b1@

    mov       r2, r1, lsl #16

@ 1159 : 
@ 1160 :         // blk7
@ 1161 :         b0 == (x7 - x4)@

    ldr       r1, [sp, #0x34]
    mov       r0, r2, asr #16
    ldr       r2, [sp, #4]
    strh      r0, [r4, #2]
    sub       r3, r2, r1

@ 1162 :         b1 == (b0 + 0x8000)>>22@
@ 1163 :         b0 == ((I16_WMV)b0)>>6@
@ 1164 :         *(((I16_WMV *)(blk7 + i)) + 0 ) == b0@

    ldr       r4, [sp, #0x50]
    mov       r0, r3, lsl #16
    mov       r1, r0, asr #16
    add       r0, r3, #2, 18
    mov       r2, r1, asr #6
    mov       r1, r0, asr #22
    strh      r2, [r4]

@ 1165 :         *(((I16_WMV *)(blk7 + i)) + 1 ) == b1@

    mov       r2, r1, lsl #16
    mov       r0, r2, asr #16
    strh      r0, [r4, #2]
    ldr       r0, [sp, #0x1C]
    sub       r0, r0, #1
    str       r0, [sp, #0x1C]
    cmp       r0, #0
    bhi       L36995

@ 1166 : 
@ 1167 :     }
@ 1168 :     FUNCTION_PROFILE_STOP(&fpDecode)@
@ 1169 : 
@ 1170 : }

    add       sp, sp, #0x54
    ldmia     sp!, {r4 - r11, pc}
M37581:
    WMV_ENTRY_END
    @ENDP  @ g_4x8IDCTDec_WMV3


    WMV_LEAF_ENTRY g_4x4IDCTDec_WMV3

@ 1178 : {

    stmdb     sp!, {r4 - r11, lr}
    FRAME_PROFILE_COUNT
    sub       sp, sp, #0x30
M37628:
    mov       r4, r0
    str       r2, [sp]

@ 1179 :   
@ 1180 :     I16_WMV* piDst == piDstBuf->i16 + (iQuadrant&2)*16 + (iQuadrant&1)*4@

    and       r0, r3, #1
    and       r1, r3, #2
    add       r1, r0, r1, lsl #2
    add       r0, r4, r1, lsl #3
    str       r0, [sp, #8]
    mov       r0, #2

@ 1181 : 
@ 1182 :    I32_WMV  x0, x1, x2, x3, x4, x5, x6, x7, x8, x5a@
@ 1183 :     I32_WMV i@
@ 1184 :     I32_WMV  *piSrc0@ 
@ 1185 :     I32_WMV j@
@ 1186 :     I32_WMV b0,c0,b1,c1,ls_signbit@
@ 1187 :    
@ 1188 :  
@ 1189 :     I32_WMV rgTemp[8]@
@ 1190 : 
@ 1191 :      I16_WMV * blk16 @
@ 1192 :     I32_WMV * blk32 == rgTemp@

    add       lr, sp, #0x10
    str       r0, [sp, #4]
L37054:

@ 1193 :     /*
@ 1194 :     I16_WMV* blk == rgTemp@   
@ 1195 :     I16_WMV* blk0 == (short*)rgiCoefReconBuf@
@ 1196 :     I16_WMV* blk1 == blk0 + 4@
@ 1197 :     I16_WMV* blk2 == blk1 + 4@
@ 1198 :     I16_WMV* blk3 == blk2 + 4@
@ 1199 : */
@ 1200 : 
@ 1201 : 
@ 1202 :     piSrc0 == (I32_WMV *)rgiCoefReconBuf->i32@
@ 1203 : 
@ 1204 : 
@ 1205 : 
@ 1206 : 
@ 1207 : 
@ 1208 :     for ( i == 0@ i < 2@ i++, blk32 +== 4){
@ 1209 :         x4 == piSrc0[ i +0*2 ]@     

    ldr       r4, [r2]

@ 1210 :        x5 == piSrc0[ i +1*2 ]@      
@ 1211 :        x6 == piSrc0[ i +2*2 ]@

    ldr       r3, [r2, #0x10]
    ldr       r5, [r2, #8]

@ 1212 :        x7 == piSrc0[ i +3*2 ]@
@ 1213 : 
@ 1214 : 
@ 1215 :         x0 == 17 * (x4 + x6) + 4 + (4<<16)@ //rounding

    add       r0, r3, r4
    ldr       r6, [r2, #0x18]
    add       r1, r0, r0, lsl #4

@ 1216 :         x1 == 17 * (x4 - x6) + 4 + (4<<16)@ //rounding

    sub       r0, r4, r3
    add       r2, r1, #1, 14
    add       r1, r0, r0, lsl #4
    add       r11, r2, #4

@ 1217 :         x8 == 10 * (x5 + x7)@

    add       r0, r6, r5
    add       r2, r1, #1, 14
    add       r1, r0, r0, lsl #2
    add       r10, r2, #4
    mov       r2, r1, lsl #1

@ 1218 :         x2 == x8 + 12 * x5@
@ 1219 :         x3 == x8 - 32 * x7@

    sub       r6, r2, r6, lsl #5
    add       r0, r5, r5, lsl #1
    add       r7, r2, r0, lsl #2

@ 1220 : 
@ 1221 :         /*
@ 1222 :         blk[0] == (I16_WMV)((x0 + x2) >> 3)@
@ 1223 :         blk[1] == (I16_WMV)((x1 + x3) >> 3)@
@ 1224 :         blk[2] == (I16_WMV)((x1 - x3) >> 3)@
@ 1225 :         blk[3] == (I16_WMV)((x0 - x2) >> 3)@
@ 1226 :         */
@ 1227 : 
@ 1228 :         // blk [0,1]
@ 1229 :         b0 == x0 + x2@
@ 1230 :         b1 == (b0 + 0x8000)>>19@
@ 1231 :         b0 == ((I16_WMV)b0)>>3@
@ 1232 : 
@ 1233 :         c0 == x1 + x3@

    add       r4, r6, r10

@ 1234 :         c1 == (c0 + 0x8000)>>19@
@ 1235 :         c0 == ((I16_WMV)c0)>>3@
@ 1236 : 
@ 1237 :         blk32[0] == (c0<<16) + b0@

    mov       r0, r4, lsl #16
    mov       r1, r0, asr #16
    mov       r2, r1, asr #3
    add       r5, r7, r11
    mov       r3, r2, lsl #16
    mov       r0, r5, lsl #16
    add       r1, r4, #2, 18
    add       r9, r3, r0, asr #19
    mov       r0, r1, asr #19
    str       r9, [lr]

@ 1238 :         blk32[0+2] == (c1<<16) + b1@
@ 1239 : 
@ 1240 :         // blk [2,3]
@ 1241 :         b0 == x1 - x3@
@ 1242 :         b1 == (b0 + 0x8000)>>19@
@ 1243 :         b0 == ((I16_WMV)b0)>>3@
@ 1244 : 
@ 1245 :         c0 == x0 - x2@

    sub       r4, r11, r7
    mov       r2, r0, lsl #16
    add       r1, r5, #2, 18

@ 1246 :         c1 == (c0 + 0x8000)>>19@
@ 1247 :         c0 == ((I16_WMV)c0)>>3@
@ 1248 : 
@ 1249 :         blk32[1] == (c0<<16) + b0@

    mov       r0, r4, lsl #16
    add       r8, r2, r1, asr #19
    mov       r1, r0, asr #16
    str       r8, [lr, #8]
    mov       r2, r1, asr #3
    sub       r5, r10, r6
    mov       r3, r2, lsl #16
    mov       r0, r5, lsl #16
    add       r1, r4, #2, 18
    add       r3, r3, r0, asr #19
    mov       r0, r1, asr #19
    str       r3, [lr, #4]

@ 1250 :         blk32[1+2] == (c1<<16) + b1@

    mov       r2, r0, lsl #16
    add       r1, r5, #2, 18
    add       r0, r2, r1, asr #19
    ldr       r2, [sp]
    str       r0, [lr, #0xC]
    add       lr, lr, #0x10
    ldr       r0, [sp, #4]
    add       r2, r2, #4
    str       r2, [sp]
    sub       r0, r0, #1
    str       r0, [sp, #4]
    cmp       r0, #0
    bhi       L37054

@ 1251 : 
@ 1252 :     }
@ 1253 : 
@ 1254 : 
@ 1255 :     piSrc0 == rgTemp@ //piDst->i32@
@ 1256 :     blk16  == piDst@
@ 1257 : 
@ 1258 :     
@ 1259 :     for (i == 0@ i < 2@ i++) {

    mov       r11, #0
    add       r10, sp, #0x10
L37061:

@ 1260 : 
@ 1261 :        x4 == piSrc0[i + 0*2 ]@

    ldr       r1, [r10]

@ 1262 :        x5 == piSrc0[i + 1*2 ]@
@ 1263 :        x6 == piSrc0[i + 2*2 ]@

    ldr       r0, [r10, #0x10]
    ldr       r5, [r10, #8]

@ 1264 :        x7 == piSrc0[i + 3*2 ]@
@ 1265 : 
@ 1266 : 
@ 1267 :         x3 == (x4 - x6)@ 
@ 1268 :         x6 +== x4@

    add       r4, r0, r1
    ldr       r3, [r10, #0x18]
    sub       r8, r1, r0

@ 1269 : 
@ 1270 :         x4 == 8 * x6 + 32 + (32<<16)@ //rounding

    add       r0, r4, #1, 14
    add       r1, r0, #4
    mov       r6, r1, lsl #3

@ 1271 :         x8 == 8 * x3 + 32 + (32<<16)@   //rounding

    add       r0, r8, #1, 14
    add       r1, r0, #4
    mov       r7, r1, lsl #3

@ 1272 : //simplify following to 3 multiplies
@ 1273 :         x5a == 11 * x5 + 5 * x7@

    add       r0, r5, r5, lsl #2
    add       r1, r5, r0, lsl #1
    add       r2, r3, r3, lsl #2
    add       r9, r2, r1

@ 1274 :         x5 == 5 * x5 - 11 * x7@

    add       r0, r3, r3, lsl #2
    add       r1, r3, r0, lsl #1
    add       r0, r5, r5, lsl #2
    sub       r5, r0, r1

@ 1275 : 
@ 1276 :         /*
@ 1277 :         x6 == (x6 >> 1)@
@ 1278 :         x3 == (x3 >> 1)
@ 1279 :         */
@ 1280 : 
@ 1281 :        ls_signbit==x6&0x8000@

    and       r2, r4, #2, 18

@ 1282 :         x6 == (x6 >> 1) - ls_signbit@

    rsb       r0, r2, r4, asr #1

@ 1283 :         x6 == x6 & ~0x8000@

    bic       r1, r0, #2, 18

@ 1284 :         x6 == x6 | ls_signbit@
@ 1285 : 
@ 1286 :         ls_signbit==x3&0x8000@

    and       r3, r8, #2, 18
    orr       r2, r1, r2

@ 1287 :         x3 == (x3 >> 1) - ls_signbit@

    rsb       r0, r3, r8, asr #1

@ 1288 :         x3 == x3 & ~0x8000@

    bic       r1, r0, #2, 18

@ 1289 :         x3 == x3 | ls_signbit@
@ 1290 : 
@ 1291 :         x4 +== x6@ // guaranteed to have enough head room

    add       r6, r6, r2
    orr       r2, r1, r3
    ldr       r1, [sp, #8]

@ 1292 :         x8 +== x3 @

    add       r4, r7, r2

@ 1293 :  
@ 1294 :         /*
@ 1295 :         blk0[i] == (I16_WMV) ((x4 + x5a) >> 6)@
@ 1296 :         blk1[i] == (I16_WMV) ((x8 + x5) >> 6)@
@ 1297 :         blk2[i] == (I16_WMV) ((x8 - x5) >> 6)@
@ 1298 :         blk3[i] == (I16_WMV) ((x4 - x5a) >> 6)@
@ 1299 :         */
@ 1300 : 
@ 1301 :         j == i<<1@

    mov       r0, r11, lsl #1
    add       r7, r1, r0, lsl #1

@ 1302 : 
@ 1303 :         // blk0
@ 1304 :         b0 == (x4 + x5a)@

    add       r3, r9, r6

@ 1305 :         b1 == (b0 + 0x8000)>>22@
@ 1306 :         b0 == ((I16_WMV)b0)>>6@
@ 1307 :         blk16[ j + 0 + 0*8] == b0@

    mov       r0, r3, lsl #16
    mov       r1, r0, asr #16
    add       r0, r3, #2, 18
    mov       r2, r1, asr #6
    mov       r1, r0, asr #22
    strh      r2, [r7]

@ 1308 :         blk16[ j + 1 + 0*8] == b1@

    mov       r2, r1, lsl #16
    mov       r0, r2, asr #16

@ 1309 : 
@ 1310 :        // blk1
@ 1311 :         b0 == (x8 + x5)@

    add       r3, r4, r5
    strh      r0, [r7, #2]

@ 1312 :         b1 == (b0 + 0x8000)>>22@
@ 1313 :         b0 == ((I16_WMV)b0)>>6@
@ 1314 :         blk16[ j + 0 + 1*8] == b0@

    mov       r0, r3, lsl #16
    mov       r1, r0, asr #16
    add       r0, r3, #2, 18
    mov       r2, r1, asr #6
    mov       r1, r0, asr #22
    strh      r2, [r7, #0x10]

@ 1315 :         blk16[ j + 1 + 1*8] == b1@

    mov       r2, r1, lsl #16
    mov       r0, r2, asr #16

@ 1316 : 
@ 1317 :         // blk2
@ 1318 :         b0 == (x8 - x5)@

    sub       r3, r4, r5
    strh      r0, [r7, #0x12]

@ 1319 :         b1 == (b0 + 0x8000)>>22@
@ 1320 :         b0 == ((I16_WMV)b0)>>6@
@ 1321 :         blk16[ j + 0 + 2*8] == b0@

    mov       r0, r3, lsl #16
    mov       r1, r0, asr #16
    add       r0, r3, #2, 18
    mov       r2, r1, asr #6
    mov       r1, r0, asr #22
    strh      r2, [r7, #0x20]

@ 1322 :         blk16[ j + 1 + 2*8] == b1@

    mov       r2, r1, lsl #16
    mov       r0, r2, asr #16

@ 1323 : 
@ 1324 :         // blk3
@ 1325 :         b0 == (x4 - x5a)@

    sub       r3, r6, r9
    strh      r0, [r7, #0x22]

@ 1326 :         b1 == (b0 + 0x8000)>>22@
@ 1327 :         b0 == ((I16_WMV)b0)>>6@
@ 1328 :         blk16[ j + 0 + 3*8] == b0@

    mov       r0, r3, lsl #16
    mov       r1, r0, asr #16
    add       r0, r3, #2, 18
    mov       r2, r1, asr #6
    mov       r1, r0, asr #22
    strh      r2, [r7, #0x30]

@ 1329 :         blk16[ j + 1 + 3*8] == b1@

    mov       r2, r1, lsl #16
    mov       r0, r2, asr #16
    add       r11, r11, #1
    strh      r0, [r7, #0x32]
    add       r10, r10, #4
    cmp       r11, #2
    blt       L37061

@ 1330 : 
@ 1331 :     } 
@ 1332 : }

    add       sp, sp, #0x30
    ldmia     sp!, {r4 - r11, pc}
M37629:
    WMV_ENTRY_END
    @ENDP  @ g_4x4IDCTDec_WMV3

    
    WMV_LEAF_ENTRY SignPatch

@ 1341 : {

    stmdb     sp!, {r4 - r6, lr}
    FRAME_PROFILE_COUNT
M37638:
    mov       r4, r0

@ 1342 :     
@ 1343 :     int i@
@ 1344 : 
@ 1345 :     I16_WMV *piSrcTmp == rgiCoefRecon@
@ 1346 :   //  I16_WMV * dsttmp == g_dsttmp@
@ 1347 : 
@ 1348 : 
@ 1349 :      I16_WMV * dsttmp == rgiCoefRecon@
@ 1350 : 
@ 1351 : //    memset(dsttmp, 0, 128)@
@ 1352 : 
@ 1353 :     for(i==0@i<(len/2)@i+==2)

    mov       r3, r1, asr #2
    mov       r3, r3, asr #2
    sub       r4, r4, #16

L37076:

@ 1354 :     {
@ 1355 :         I16_WMV data_odd == piSrcTmp[i+1]@
@ 1356 :        
@ 1357 :             *(I32_WMV *)(dsttmp + i) ==  (I32_WMV) (piSrcTmp[i])@

    ldrsh     r0, [r4, #16]!
    ldrsh     r2, [r4, #2]
    ldrsh     r5, [r4, #4]
    ldrsh     r6, [r4, #6]
    

@ 1358 :         
@ 1359 :             dsttmp[i+1]  +==  data_odd@

    add       r1, r2, r0, asr #31 
    strh      r1, [r4, #2]

@ 1358 :         
@ 1359 :             dsttmp[i+1]  +==  data_odd@

    add       r1, r6, r5, asr #31 
    strh      r1, [r4, #6]

    ldrsh     r0, [r4, #8]
    ldrsh     r2, [r4, #10]
    ldrsh     r5, [r4, #12]
    ldrsh     r6, [r4, #14]

@ 1358 :         
@ 1359 :             dsttmp[i+1]  +==  data_odd@

    add       r1, r2, r0, asr #31 
    strh      r1, [r4, #10]

@ 1358 :         
@ 1359 :             dsttmp[i+1]  +==  data_odd@

    add       r1, r6, r5, asr #31 
    strh      r1, [r4, #14]
    
    subs      r3, r3, #1
    bhi       L37076
L37078:

@ 1360 :     }
@ 1361 : 
@ 1362 :    
@ 1363 : }

    ldmia     sp!, {r4 - r6, pc}
M37639:
    WMV_ENTRY_END
    @ENDP  @ SignPatch

    WMV_LEAF_ENTRY SignPatch32
@void SignPatch32(I32_WMV * rgiCoefRecon, int len)
@{
@    int i@
@    I32_WMV v1, v2@
@    
@    for(i==0@ i < (len >> 2)@ i++)
@    {
@        v1 == rgiCoefRecon[i*2]@
@        v2 == rgiCoefRecon[i*2+1]@
@        
@        rgiCoefRecon[i] == (v1 & 0x0000ffff) | (((v1 >> 16) + v2) << 16)@
@    }
@}
    stmdb     sp!, {r4-r9, lr}
    FRAME_PROFILE_COUNT

    mov     r14, r0

    mov   r12, #0xFF
    add   r12, r12, r12, lsl #8
	.if PLD_ENABLE == 1
	pld	  [r14, #32]
	.endif

SignPatch32_loop:
	.if PLD_ENABLE == 1
	pld	  [r14, #64]
	.endif
    ldr      r2, [r14], #+4
    ldr      r3, [r14], #+4
    ldr      r4, [r14], #+4
    ldr      r5, [r14], #+4
    ldr      r6, [r14], #+4
    ldr      r7, [r14], #+4
    ldr      r8, [r14], #+4
    ldr      r9, [r14], #+4

    add     r3, r3, r2, asr #16
    and     r2, r2, r12
    orr     r2, r2, r3, lsl #16

    add     r5, r5, r4, asr #16
    and     r4, r4, r12
    orr     r4, r4, r5, lsl #16

    add     r7, r7, r6, asr #16
    and     r6, r6, r12
    orr     r6, r6, r7, lsl #16

    add     r9, r9, r8, asr #16
    and     r8, r8, r12
    orr     r8, r8, r9, lsl #16

    str     r2, [r0], #+4
    str     r4, [r0], #+4
    str     r6, [r0], #+4
    str     r8, [r0], #+4

    subs    r1, r1, #16
    bge    SignPatch32_loop

    ldmia     sp!, {r4-r9, pc}
    WMV_ENTRY_END
    @ENDP  @ SignPatch32

@
@ Void_WMV g_IDCTDec_WMV3_Pass1(const I32_WMV  *piSrc0, I32_WMV * blk32, const I32_WMV iNumLoops, I32_WMV iDCTHorzFlags)
@ r0 == piSrc0   r1== blk32   r2 == iNumLoops  r3 == iDCTHorzFlags
@ 

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA WMV9_IDCT, CODE, READONLY
    WMV_LEAF_ENTRY g_IDCTDec_WMV3_Pass1
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    stmdb   sp!, {r4 - r11, lr}
    FRAME_PROFILE_COUNT

    bl   _g_IDCTDec_WMV3_Pass1_Naked

    ldmia     sp!, {r4 - r11, pc}
    WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA    |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY g_IDCTDec_WMV3_Pass1_Naked
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@   stmdb   sp!, {r4 - r11, lr}
@   stmdb   sp!, {lr}
    str     lr,  [sp, #-4]!

    FRAME_PROFILE_COUNT

	.if PLD_ENABLE == 1
@	pld		[r0]       
	.endif
				
    mov     r4, #4

Pass1LoopStart:

    tst     r3, #3
    bne     Pass1FullTransform

@   r6 == b0
    ldr     r7, [r0], #4            @ piSrc0[i]
    add     r6, r4, r4, lsl #16
    add     r6, r6, r7, asl #2      @ piSrc0[i] * 4 + (4+(4<<16))
    adds    r6, r6, r7, asl #3      @ +piSrc0[i] * 8

@   r8 == b1
    add     r8, r6, r4, lsl #13     @ b0 + 0x8000
    movne   r6, r6, lsl #16
    
    movne   r6, r6, asr #19         @ ((I16_WMV)b0)>>3

@   r6 == iCurr  r8 == iNext
    addne   r6, r6, r6, asl #16
    movs    r8, r8, asr #19

    addne   r8, r8, r8, asl #16

@   blk32[0-7]

    str     r6, [r1], #4
    str     r6, [r1], #4
    str     r6, [r1], #4
    str     r6, [r1], #4
    str     r8, [r1], #4
    str     r8, [r1], #4
    str     r8, [r1], #4
    str     r8, [r1], #4

    b       Pass1BeforeEnd

Pass1FullTransform:
@
@zeroth stage
@
@x4 == piSrc0[ i +1*4 ]
@x5 == piSrc0[ i +7*4 ]
@y3 == x4 + x5
@x8 == W3 * y3
@x4a == x8 - W3pW5 * x5
@x5a == x8 - W3_W5 * x4

    ldr     r8, [r0, #16]           @ x4 == [4]
    ldr     r9, [r0, #112]          @ x5 == [28*4]
    ldr     r6, [r0, #80]           @ x6
    ldr     r7, [r0, #48]           @ x7
    add     lr, r8, r9              @ y3
   
    rsb     r10, lr, lr, asl #4     @ x8 == 15*y3
    sub     r11, r10, r9, asl #4    @ x4a == x8 - 16 * x5
    sub     r11, r11, r9, asl #3    @          - 8 * x5
    sub     r12, r10, r8, asl #2    @ x5a == x8 - 4 * x4
    sub     r12, r12, r8, asl #1    @          - 2 * x4
@
@x8 == W7 * y3@          //4
@x4 == x8 + W1_W7 * x4@  //12
@x5 == x8 - W1pW7 * x5@  //20

    mov     r10, lr, asl #2         @ x8 == W7(4) * y3
    add     lr,  r10, r8, asl #3    @ x4 == x8 + 8 * x4
    add     r8,  lr,  r8, asl #2    @         + 4 * x4
    
@
@first stage
@   r8 == x4     r9 == x5     r11 == x4a   r12 == x5a

@x7 == piSrc0[ i +3*4 ]
@x6 == piSrc0[ i +5*4 ]

    
    sub     lr,  r10, r9, asl #4    @ x5 == x8 - 16 * x5
    sub     r9,  lr,  r9 ,asl #2    @         -  4 * x5

@y3 == x6 + x7@
@x8 == W7 * y3@          //4
@x4a -== x8 + W1_W7 * x6@ //12
@x5a +== x8 - W1pW7 * x7@    //20

    adds    lr, r6, r7              @ x6+x7
@    mov     r10, lr, asl #2         @ x8 == 4 * y3
    addne   r12, r12, lr, asl #2
    subne   r11, r11, lr, asl #2
    sub     r11, r11, r6, asl #3
    sub     r11, r11, r6, asl #2    @ x4a -== x8 + W1_W7 * x6
    sub     r12, r12, r7, asl #4
    sub     r12, r12, r7, asl #2    @ x5a +== x8 - W1pW7 * x7

@x8 == W3 * y3@          //15
@x4 +== x8 - W3_W5 * x6@ //6
@x5 +== x8 - W3pW5 * x7@ //24

    rsbs    r10, lr, lr, asl #4     @ x8 == 15 * y3
    addne   r8,  r8, r10            @ x4 +== x8 - W3_W5 * x6
    sub     r8,  r8, r6, asl #2
@
@second stage
@
@
@x0 == piSrc0[ i +0*4 ]@ /* for proper rounding */
@x1 == piSrc0[ i +4*4 ]@
@x1 == x1 * W0@  //12
@x0 == x0 * W0 + (4+(4<<16))@ /* for proper rounding */
@x8 == x0 + x1@
@x0 -== x1@

    ldr     lr, [r0], #4            @ x0 == r6
    sub     r8,  r8, r6, asl #1
    addne   r9,  r9, r10            @ x5 +== x8 - W3pW5 * x7
    add     r6, r4, lr, asl #2      @ x0*4+(4+(4<<16))
    add     r6, r6, r4, lsl #16
    add     r6, r6, lr, asl #3      @     +x0*8

    ldr     lr, [r0, #60]           @ x1 == r7
    sub     r9,  r9, r7, asl #4
    sub     r9,  r9, r7, asl #3
    movs    r7, lr, asl #2          
    addnes  r7, r7, lr, asl #3

@r6==x0  r10 == x8
    add     r10, r6, r7             @ x8 == x0 + x1

@x3 == piSrc0[ i +2*4 ]@
@x2 == piSrc0[ i +6*4 ]@     
@x1 == x2@
@x2 == W6 * x3 - W2 * x2@  //6,  16
@x3 == W6 * x1 + W2A * x3@ //6,  16

    ldr     lr, [r0, #28]           @ x3
    subne   r6, r6, r7              @ x0
    ldr     r7, [r0, #92]           @ x2

@r5 == x2
    movs    r5, lr, asl #2          @ 4*x3
    addne   r5, r5, lr, asl #1      @ 6 * x3
    sub     r5, r5, r7, asl #4      @  - 16 * x2

@r7 == x3
    movne   lr, lr, asl #4          @ 16*x3
    add     lr, lr, r7, asl #2      @ 4*x2+16*x3
    adds    r7, lr, r7, asl #1      @ 6 * x2 + 16 * x3
@
@third stage
@  lr==x7   r10==x8  r7==x3      r6==x0
@
@x7 == x8 + x3@
@x8 -== x3@
@x3 == x0 + x2@
@x0 -== x2@

    add     lr, r10, r7             @ x7 == x8 + x3
    subne   r10, r10, r7
    add     r7, r6, r5
    sub     r6, r6, r5
@
@ store blk32[0], blk32[4]
@  lr==x7   r10==x8  r7==x3      r6==x0
@  r8==x4   r9==x5   r11==x4a    r12==x5a
@

@b0 == x7 + x4@
@b1 == (b0 + 0x8000)>>19@
@b0 == ((I16_WMV)b0)>>3@

@c0 == x3 + x4a@
@c1 == (c0 + 0x8000)>>19@
@c0 == ((I16_WMV)c0)>>3@

    add     lr, lr, r8              @ b0 == x7 + x4
    sub     r8, lr, r8, asl #1      @ cn0 == x7 - x4
    add     r7, r7, r11             @ c0 == x3 + x4a
    
    add     r5, lr, r4, lsl #13     @ b1 == b0+0x8000
    movs    lr, lr, lsl #16
    sub     r11, r7, r11, asl #1    @ bn0 == x3 - x4a
    movne   lr, lr, asr #19
    add     r4, r7, r4, lsl #13     @ c1 == c0+0x8000
    movs    r7, r7, lsl #16
    
    movne   r7, r7, asr #19
    movs    r4, r4, asr #19
    
    movne   r4, r4, asl #16
    add     r4, r4, r5, asr #19     @ (c1<<16) + b1
    str     r4, [r1, #16]           @ blk32[4]

@ store blk32[3], blk32[7]
@   r11==b0  r8==c0  lr,r7,r5,r4 free
@
    mov     r4, #4
    add     r7, lr, r7, asl #16     @ (c0<<16) + b0
    adds    lr, r11, r4, lsl #13    
    str     r7, [r1]                @ blk32[0]
    movne   lr, lr, asr #19         @ b1 == (b0 + 0x8000)>>19
    movs    r11, r11, lsl #16
    add     r6,  r6, r12            @ b0 == x0+x5a
    movne   r11, r11, asr #19       @ ((I16_WMV)b0)>>3

    adds    r7, r8, r4, lsl #13    
    sub     r12, r6, r12, asl #1    @ cn0== x0-x5a
    movne   r7, r7, asr #19         @ c1 == (c0 + 0x8000)>>19
    movs    r8, r8, lsl #16
    add     r7, lr, r7, asl #16     @ blk32[7] == (c1<<16) + b1
    movne   r8, r8, asr #19         @ ((I16_WMV)c0)>>3
    
    
@ store blk32[1], blk32[5]
@   r6==x0   r12==x5a r10==x8  r9==x5
@
    
    adds    lr, r6, r4, lsl #13    
    add     r10, r10, r9            @ c0 == x8+x5
    movne   lr, lr, asr #19         @ b1 == (b0 + 0x8000)>>19
    movs    r6, r6, lsl #16
    sub     r9,  r10, r9, asl #1    @ bn0== x8-x5
    movne   r6, r6, asr #19       @ ((I16_WMV)b0)>>3
    
    adds    r5, r10, r4, lsl #13    
    add     r8, r11, r8, asl #16    @ blk32[3] == (c0<<16) + b0
    movne   r5, r5, asr #19         @ c1 == (c0 + 0x8000)>>19
    movs    r10, r10, lsl #16
    str     r8, [r1, #12]
    movne   r10, r10, asr #19         @ ((I16_WMV)c0)>>3
    str     r7, [r1, #28]
    add     r5, lr, r5, asl #16     @ blk32[7] == (c1<<16) + b1
    
            
@ store blk32[2], blk32[6]
@

    adds    lr, r9, r4, lsl #13    
    add     r6, r6, r10, asl #16    @ blk32[3] == (c0<<16) + b0
    movne   lr, lr, asr #19         @ b1 == (b0 + 0x8000)>>19
    movs    r9, r9, lsl #16
    str     r5, [r1, #20]
    movne   r9, r9, asr #19       @ ((I16_WMV)b0)>>3
    adds    r10, r12, r4, lsl #13    
    str     r6, [r1, #4]
    movne   r10, r10, asr #19         @ c1 == (c0 + 0x8000)>>19
    movs    r12, r12, lsl #16
    movne   r12, r12, asr #19         @ ((I16_WMV)c0)>>3
    add     r10, lr, r10, asl #16     @ blk32[7] == (c1<<16) + b1
    add     r9, r9, r12, asl #16    @ blk32[3] == (c0<<16) + b0
    str     r9, [r1, #8]
    str     r10, [r1, #24]

    add     r1,  r1,  #32           @ blk32+==8
    
Pass1BeforeEnd:
    subs    r2, r2, #1               @ if (i<iNumLoops)
    mov     r3,  r3,  asr #2        @ iDCTHorzFlags >>= 2
    bne     Pass1LoopStart

@   ldmia     sp!, {r4 - r11, pc}
@   ldmia     sp!, {pc}
    ldr       pc,  [sp], #4
    
    WMV_ENTRY_END	@g_IDCTDec_WMV3_Pass1_Naked

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA WMV9_IDCT, CODE, READONLY
    WMV_LEAF_ENTRY g_IDCTDec_WMV3_Pass2
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    stmdb   sp!, {r4 - r11, lr}
    FRAME_PROFILE_COUNT

    bl   _g_IDCTDec_WMV3_Pass2_Naked

    ldmia     sp!, {r4 - r11, pc}
    WMV_ENTRY_END
@
@Void_WMV g_IDCTDec_WMV3_Pass2(const I32_WMV  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops)
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA    |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY g_IDCTDec_WMV3_Pass2_Naked
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@ r0==piSrc0   r1==blk16   r2==iNumLoops
@
@   stmdb   sp!, {r4 - r11, lr}
@   stmdb   sp!, {lr}
    str     lr,  [sp, #-4]!  

    FRAME_PROFILE_COUNT

	.if PLD_ENABLE == 1
@	pld		[r0]       
	.endif
				
    ldr     r8,  [r0, #16]          @ x4
    ldr     r9,  [r0, #112]         @ x5
    mov     r3, #32

Pass2LoopStart:
@x4 == piSrc0[i + 1*4 ]@
@x5 == piSrc0[i + 7*4 ]@
@y4a == x4 + x5@
@x8 == 7 * y4a@
@x4a == x8 - 12 * x5@
@x5a == x8 - 3 * x4@
@  zeroth stage

    ldr     r10, [r0, #80]          @ x6
    adds    lr,  r8, r9             @ y4a
    ldr     r11, [r0, #48]          @ x7
    rsb     r12, lr, lr, asl #3     @ x8
    sub     r6,  r12, r9, asl #3
    sub     r6,  r6, r9, asl #2     @ x4a
    sub     r7,  r12, r8, asl #1    
    sub     r7,  r7,  r8            @ x5a

@x8 == 2 * y4a@
@x4 == x8 + 6 * x4@
@x5 == x8 - 10 * x5@

    movne   r12, lr, asl #1         @ x8
    add     r4,  r12, r8, asl #2
    add     r8,  r4, r8, asl #1     @ x4
    sub     r4,  r12, r9, asl #3
    sub     r9,  r4,  r9, asl #1    @ x5

@ls_signbit==y4a&0x8000@
@y4a == (y4a >> 1) - ls_signbit@
@y4a == y4a & ~0x8000@
@y4a == y4a | ls_signbit@
@x4a +== y4a@
@x5a +== y4a@

    and     r4,  lr, r3, lsl #10
    rsb     lr,  r4, lr, asr #1
    bic     lr,  lr, r3, lsl #10
    orrs    lr,  lr, r4             @ y4a



@x6 == piSrc0[i + 5*4 ]@
@x7 == piSrc0[i + 3*4 ]@
@y4 == x6 + x7@
@x8 == 2 * y4@
@x4a -== x8 + 6 * x6@
@x5a +== x8 - 10 * x7@
@  first stage

    addne   r6,  r6, lr             @ x4a
    addne   r7,  r7, lr             @ x5a
    adds    lr,  r10, r11           @ y4
@    mov     r12, lr, asl #1         @ x8
    rsb     r12, lr, lr, asl #3     @ x8
    subne   r6,  r6, lr, asl #1
    sub     r6,  r6, r10, asl #2
    sub     r6,  r6, r10, asl #1    @ x4a
    addne   r7,  r7, lr, asl #1
    sub     r7,  r7, r11, asl #3
    sub     r7,  r7, r11, asl #1    @ x5a

@x8 == 7 * y4@
@ls_signbit==y4&0x8000@
@y4 == (y4 >> 1) - ls_signbit@
@y4 == y4 & ~0x8000@
@y4 == y4 | ls_signbit@
@x4 +== y4@
@x5 +== y4@
@x4 +== x8 - 3 * x6@
@x5 +== x8 - 12 * x7@

    ands    r4,  lr, r3, lsl #10
    rsb     lr,  r4, lr, asr #1
    bic     lr,  lr, r3, lsl #10
    orrnes  lr,  lr, r4             @ y4
    adds    r12, r12, lr           @ y4+x8
    addne   r8,  r8, r12
@
@ second stage
@
@x0 == piSrc0[i + 0*4 ] * 6 + 32 + (32<<16) /* rounding */@
@x1 == piSrc0[i + 4*4 ] * 6@
@x8 == x0 + x1@
@x0 -== x1@
@x2 == piSrc0[i + 6*4 ]@
@x3 == piSrc0[i + 2*4 ]@
@x1 == 8 * (x2 + x3)@
@x6 == x1 - 5 * x2@
@x1 -== 11 * x3@

    ldr     lr,  [r0], #4
    addne   r9,  r9, r12
    add     r4,  r3, r3, lsl #16
    add     r4,  r4, lr, asl #2
    add     r4,  r4, lr, asl #1     @ x0

    ldr     lr,  [r0, #60]          @ [4*4]
    sub     r8,  r8, r10, asl #1
    sub     r8,  r8, r10            @ x4
    adds    lr,  lr, lr, asl #1      
    sub     r9,  r9, r11, asl #3
    add     r12, r4, lr, asl #1     @ x8
    subne   r4,  r4, lr, asl #1     @ x0

    ldr     lr,  [r0, #92]          @ x2
    ldr     r5,  [r0, #28]          @ x3
    sub     r9,  r9, r11, asl #2    @ x5
    add     r11, lr, lr, asl #1     @ 3*x2
    adds    r10, r11, r5, asl #3    @ x6
    sub     r11, r5,  r5, asl #2     @ -3*x3
    add     r5,  r11, lr, asl #3    @ x1

@ third stage
@x7 == x8 + x6@
@x8 -== x6@
@x6 == x0 - x1@
@x0 +== x1@

    add     r11,  r12, r10          @ x7
    subne   r12,  r12, r10          @ x8
    sub     r10,  r4, r5            @ x6
    add     r4,   r4, r5            @ x0
@
@r4==x0 r6==x4a r7==x5a r8==x4 r9==x5 r10==x6 r11==x7 x12==x8 
@ r5, lr are free

@// blk0
@b0 == (x7 + x4)@
@b1 == (b0 + 0x8000)>>22@
@b0 == ((I16_WMV)b0)>>6@
@blk16[ i + 0   + 0*4] == b0@
@blk16[ i + 32 + 0*4] == b1@

    adds    r5,  r11, r8            @ b0
    add     lr,  r5,  r3, lsl #10
    movne   r5,  r5,  asl #16
    mov     lr,  lr,  asr #22       @ b1
    strh    lr,  [r1, #64]
    movne   r5,  r5,  asr #22       @ b0
    

@// blk7
@b0 == (x7 - x4)@
@b1 == (b0 + 0x8000)>>22@
@b0 == ((I16_WMV)b0)>>6@
@blk16[ i + 0 + 7*4] == b0@
@blk16[ i + 32 + 7*4] == b1@

    subs    r11, r11,  r8          @ b0
    add     lr,  r11,  r3, lsl #10
    strh    r5,  [r1]
    mov     lr,  lr,  asr #22      @ b1
    
    movne   r11,  r11,  asl #16
    strh    lr,  [r1, #120]
    movne   r11,  r11,  asr #22    @ b0
    
@// blk1
@b0 == (x6 + x4a)@
@b1 == (b0 + 0x8000)>>22@
@b0 == ((I16_WMV)b0)>>6@
@blk16[ i + 0 + 1*4] == b0@
@blk16[ i + 32 + 1*4] == b1@

    adds    r5,  r10, r6            @ b0
    add     lr,  r5,  r3, lsl #10
    strh    r11,  [r1, #56]
    mov     lr,  lr,  asr #22       @ b1
    movne   r5,  r5,  asl #16
    strh    lr,  [r1, #72]
    movne   r5,  r5,  asr #22       @ b0
    

@// blk6
@b0 == (x6 - x4a)@
@b1 == (b0 + 0x8000)>>22@
@b0 == ((I16_WMV)b0)>>6@
@blk16[ i + 0 + 6*4] == b0@
@blk16[ i + 32 + 6*4] == b1@

    subs    r11, r10, r6     
    add     lr,  r11, r3, lsl #10
    strh    r5,  [r1, #8]
    mov     lr,  lr,  asr #22      @ b1
    
    movne   r11,  r11,  asl #16
    strh    lr,  [r1, #112]
    movne   r11,  r11,  asr #22    @ b0
    

@// blk2
@b0 == (x0 + x5a)@
@b1 == (b0 + 0x8000)>>22@
@b0 == ((I16_WMV)b0)>>6@
@blk16[ i + 0 + 2*4] == b0@
@blk16[ i + 32 + 2*4] == b1@

    adds    r5,  r4, r7            @ b0
    add     lr,  r5,  r3, lsl #10
    strh    r11,  [r1, #48]
    mov     lr,  lr,  asr #22       @ b1

    movne   r5,  r5,  asl #16
    strh    lr,  [r1, #80]
    movne   r5,  r5,  asr #22       @ b0

@// blk5
@b0 == (x0 - x5a)@
@b1 == (b0 + 0x8000)>>22@
@b0 == ((I16_WMV)b0)>>6@
@blk16[ i + 0 + 5*4] == b0@
@blk16[ i + 32 + 5*4] == b1@

    subs    r11, r4, r7     
    add     lr,  r11, r3, lsl #10
    strh    r5,  [r1, #16]
    mov     lr,  lr,  asr #22      @ b1
    
    movne   r11,  r11,  asl #16
    strh    lr,  [r1, #104]
    movne   r11,  r11,  asr #22    @ b0
    
@// blk3
@b0 == (x8 + x5)@
@b1 == (b0 + 0x8000)>>22@
@b0 == ((I16_WMV)b0)>>6@
@blk16[ i + 0 + 3*4] == b0@
@blk16[ i + 32 + 3*4] == b1@

    adds    r5,  r12, r9            @ b0
    add     lr,  r5,  r3, lsl #10
    strh    r11,  [r1, #40]
    mov     lr,  lr,  asr #22       @ b1

    movne   r5,  r5,  asl #16
    strh    lr,  [r1, #88]
    movne   r5,  r5,  asr #22       @ b0
    

@// blk4
@b0 == (x8 - x5)@
@b1 == (b0 + 0x8000)>>22@
@b0 == ((I16_WMV)b0)>>6@
@blk16[ i + 0 + 4*4] == b0@
@blk16[ i + 32 + 4*4] == b1@

    subs    r11, r12, r9     
    add     lr,  r11, r3, lsl #10
    strh    r5,  [r1, #24]
    mov     lr,  lr,  asr #22      @ b1
    
    movne   r11,  r11,  asl #16
    strh    lr,  [r1, #96]
    movne   r11,  r11,  asr #22    @ b0
    strh    r11,  [r1, #32]

    subs    r2, r2, #1               @ if (i<iNumLoops)
    add     r1, r1, #2
    ldrne   r8,  [r0, #16]          @ x4
    ldrne   r9,  [r0, #112]         @ x5
    bne     Pass2LoopStart

@   ldmia     sp!, {r4 - r11, pc}
@   ldmia     sp!, {pc}
    ldr       pc,  [sp], #4
    
    WMV_ENTRY_END	@g_IDCTDec_WMV3_Pass2_Naked

@
@Void_WMV g_IDCTDec_WMV3_Pass3(const I32_WMV  *piSrc0, I32_WMV * blk32, const I32_WMV iNumLoops,  I32_WMV iDCTHorzFlags)
@
@ r0 == piSrc0   r1== blk32   r2 == iNumLoops  r3 == iDCTHorzFlags
@ 

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA WMV9_IDCT, CODE, READONLY
    WMV_LEAF_ENTRY g_IDCTDec_WMV3_Pass3
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    stmdb   sp!, {r4 - r9, lr}
    FRAME_PROFILE_COUNT

    bl   _g_IDCTDec_WMV3_Pass3_Naked

    ldmia     sp!, {r4 - r9, pc}
    WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA    |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY g_IDCTDec_WMV3_Pass3_Naked
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
@   stmdb   sp!, {r4 - r9, lr}
@   stmdb   sp!, {lr}
    str     lr,  [sp, #-4]!

    FRAME_PROFILE_COUNT

	.if PLD_ENABLE == 1
@	pld		[r0]
	.endif

    mov     r4, #4
    add     r5, r4, r4, lsl #16

Pass3LoopStart:
    
    tst     r3, #3
    bne     Pass3FullTransform

    ldr     r7, [r0], #4            @ piSrc0[i]
    
    
    add     r6, r5, r7, asl #4      @ piSrc0[i] * 16 + (4+(4<<16))
    adds    r6, r6, r7              @ +piSrc0[i]

@   r7 == b1
    add     r7, r6, r4, lsl #13     @ b0 + 0x8000
    movne   r6, r6, lsl #16
    
    movne   r6, r6, asr #19         @ ((I16_WMV)b0)>>3

@   r6 == iCurr  r7 == iNext
    
    addne   r6, r6, r6, asl #16
    movs    r7, r7, asr #19
    str     r6, [r1], #32
    addne   r7, r7, r7, asl #16

@   blk32[0-7]
    
    str     r6, [r1, #-28]
    str     r7, [r1, #-16]
    str     r7, [r1, #-12]

    b       Pass3BeforeEnd

Pass3FullTransform:
@x4 == piSrc0[ i +0*4 ]@     
@x6 == piSrc0[ i +2*4 ]@

    ldr     r8, [r0], #4            @ x4
    ldr     r9, [r0, #28]           @ x6
    ldr     r10, [r0, #12]           @ x5

    add     lr, r8, r9
    ldr     r11, [r0, #44]           @ x7

    add     r6, lr, lr, asl #4
    sub     lr, r8, r9
    
@x5 == piSrc0[ i +1*4 ]@     
@x7 == piSrc0[ i +3*4 ]@
@x8 == 10 * (x5 + x7)@
@x2 == x8 + 12 * x5@
@x3 == x8 - 32 * x7@

    add     r7, lr, lr, asl #4
    add     r6, r6, r5              @ x0
    adds    r12, r10, r11
    add     r7, r7, r5              @ x1
    addne   r12, r12, r12, asl #2
    
    movne   r12, r12, asl #1        @ x8

    add     lr, r12, r10, asl #3
    adds    r8, lr, r10, asl #2      @ x2
    sub     r9, r12, r11, asl #5     @ x3
@
@ store blk32[0], blk32[4]
@
@b0 == x0 + x2@
@b1 == (b0 + 0x8000)>>19@
@b0 == ((I16_WMV)b0)>>3@

    addne   r6, r6, r8              @ b0 ==  x0 + x2@
    sub     r8, r6, r8, asl #1      @ cn0 == x0 - x2@
    add     r7, r7, r9              @ c0 == x1 + x3
    
    add     lr, r6, r4, lsl #13     @ b1 == b0+0x8000
    movs    r6, r6, lsl #16
    sub     r9, r7, r9, asl #1      @ bn0 == x1 - x3
    movne   r6, r6, asr #19         @ b0
    add     r12,r7, r4, lsl #13     @ c1
    movs    r7, r7, lsl #16
    
    movne   r7, r7, asr #19         @ c0
    movs    r12, r12, asr #19       @ c1 >> 19
    add     r7, r6, r7, asl #16     @ (c0<<16) + b0
    str     r7, [r1], #32           @ blk32[0]
    movne   r12, r12, asl #16       @ c1 << 16
    add     r12,  r12, lr, asr #19  @ c1 + b1>>19
    
@ store  blk[1,5]
@
    add     lr, r9, r4, lsl #13     @ b1
    movs    r9, r9, lsl #16
    str     r12,  [r1, #-16]         @ blk32[4]
    movne   r9, r9, asr #19         @ b0

    add     r12, r8, r4, lsl #13    @ c1
    movs    r8, r8, lsl #16
    
    movne   r8, r8, asr #19         @ c0
    movs    r12, r12, asr #19       @ c1 >> 19
    add     r8, r9, r8, asl #16     @ (c0<<16) + b0
    str     r8, [r1, #-28]      
    movne   r12, r12, asl #16       @ c1 << 16
    add     r12, r12, lr, asr #19   @ c1 + b1>>19
    str     r12, [r1, #-12]        

@    add     r1,  r1,  #32           @ blk32+==8
    
Pass3BeforeEnd:
    subs    r2, r2, #1               @ if (i<iNumLoops)
    mov     r3,  r3,  asr #2        @ iDCTHorzFlags >>= 2
    bne     Pass3LoopStart


@   ldmia   sp!, {r4 - r9, pc}
@   ldmia   sp!, {pc}
    ldr     pc,  [sp], #4
    	
    WMV_ENTRY_END	@g_IDCTDec_WMV3_Pass3_Naked

@
@Void_WMV g_IDCTDec_WMV3_Pass4(const I32_WMV  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops)
@

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA WMV9_IDCT, CODE, READONLY
    WMV_LEAF_ENTRY g_IDCTDec_WMV3_Pass4
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    stmdb   sp!, {r4 - r8, lr}
    FRAME_PROFILE_COUNT

    bl   _g_IDCTDec_WMV3_Pass4_Naked

    ldmia     sp!, {r4 - r8, pc}
    WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA    |.embsec_PMainLoopLvl1|, CODE
    WMV_LEAF_ENTRY g_IDCTDec_WMV3_Pass4_Naked
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ r0==piSrc0   r1==blk16   r2==iNumLoops
@
@   stmdb   sp!, {r4 - r8, lr}
@   stmdb   sp!, {lr}
    str     lr,  [sp, #-4]!


    FRAME_PROFILE_COUNT

	.if PLD_ENABLE == 1
@	pld		[r0]
	.endif

    ldr     r5,  [r0], #4           @ x4
    ldr     r12, [r0, #28]          @ x6

    mov     r3, #32
    add     r4, r3, r3, lsl #16

Pass4LoopStart:

@x4 == piSrc0[i + 0*4 ]@
@x6 == piSrc0[i + 2*4 ]@
@x3 == (x4 - x6)@ 
@x6 +== x4@
@x4 == 8 * x6 + 32 + (32<<16)@ //rounding
@x8 == 8 * x3 + 32 + (32<<16)@   //rounding

    sub     lr,  r5, r12            @ x3
    add     r12,  r12, r5           @ x6
    ldr     r6,  [r0, #12]          @ x5
    add     r5,  r4, r12, asl #3
    add     r7,  r4, lr, asl #3     @ x8

@ls_signbit==x6&0x8000@
@x6 == (x6 >> 1) - ls_signbit@
@x6 == x6 & ~0x8000@
@x6 == x6 | ls_signbit@
@x4 +== x6@ // guaranteed to have enough head room

    and     r8, r12,  r3, lsl #10
    rsb     r12, r8,  r12, asr #1
    bic     r12, r12,  r3, lsl #10
    orrs    r12, r12,  r8
    addne   r5,  r5,  r12

@ls_signbit==lr&0x8000@
@x3 == (x3 >> 1) - ls_signbit@
@x3 == x3 & ~0x8000@
@x3 == x3 | ls_signbit@
@x8 +== x3 @

    and     r8,  lr,  r3, lsl #10
    rsb     lr,  r8,  lr, asr #1
    ldr     r12, [r0, #44]          @ x7
    bic     lr,  lr,  r3, lsl #10
    orrs    lr,  lr,  r8
    addne   r7,  r7,  lr

@x5 == piSrc0[i + 1*4 ]@
@x7 == piSrc0[i + 3*4 ]@
@x1 == 5 * ( x5 + x7)@
@x5a == x1 + 6 * x5@
@x5 ==  x1 - x7 >> 4@

    adds    lr, r6, r12
    
    addne   lr, lr, lr, asl #2      @ 5*(x5+x7)
    add     r8, lr, r6, asl #2
    adds    r8, r8, r6, asl #1      @ x5a
    sub     r6, lr, r12, asl #4     @ x5

@blk0, blk3
@b0 == (x4 + x5a)@
@b0 == (x4 - x5a)@

    addne   r5,  r5, r8             @ b0
    adds    r12, r5, r3, lsl #10   
    sub     r8,  r5, r8, asl #1     @ bn0
    movne   r12, r12, asr #22      @ b1
    movs    r5,  r5, asl #16
    strh    r12, [r1, #64]
    movne   r5,  r5, asr #22       @ b0
    adds    r12, r8, r3, lsl #10   
    strh    r5,  [r1]
    movne   r12, r12, asr #22      @ b1
    movs    r8,  r8, asl #16
    strh    r12, [r1, #88]
    movne   r8,  r8, asr #22       @ b0
    

@blk1 blk2
@b0 == (x8 + x5)@
@b0 == (x8 - x5)@

    add     r7,  r7, r6             @ b0
    sub     r6,  r7, r6, asl #1     @ bn0

    adds    r12, r7, r3, lsl #10   
    strh    r8,  [r1, #24]
    movne   r12, r12, asr #22      @ b1
    movs    r7,  r7, asl #16
    strh    r12, [r1, #72]
    movne   r7,  r7, asr #22       @ b0
    adds    r12, r6, r3, lsl #10   
    strh    r7,  [r1, #8]
    movne   r12, r12, asr #22      @ b1
    movs    r6,  r6, asl #16
    strh    r12, [r1, #80]
    movne   r6,  r6, asr #22       @ b0
    strh    r6,  [r1, #16]
    add     r1, r1, #2

    subs    r2, r2, #1               @ if (i<iNumLoops)
    ldrne   r5,  [r0], #4           @ x4
    ldrne   r12, [r0, #28]          @ x6
    bne     Pass4LoopStart

@   ldmia     sp!, {r4 - r8, pc}
@   ldmia     sp!, {pc}
    ldr       pc,  [sp], #4
    
    WMV_ENTRY_END	@g_IDCTDec_WMV3_Pass4_Naked

    .endif @ WMV_OPT_IDCT_ARM

    @@.end 

