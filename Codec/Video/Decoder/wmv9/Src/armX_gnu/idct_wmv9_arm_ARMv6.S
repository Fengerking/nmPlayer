    #include "../c/voWMVDecID.h"
    .include "wmvdec_member_arm.inc"
    .include "xplatform_arm_asm.h" 

    @AREA |.text|, CODE, READONLY
    .section .text
    
    .IF WMV_OPT_IDCT_ARM == 1


    .global  ARMV6_g_IDCTDec16_WMV3_SSIMD
    .global  ARMV6_g_DecodeInterError8x8_SSIMD
    .global  ARMV6_g_DecodeInterError8x4_SSIMD
    .global  ARMV6_g_IDCTDec_WMV3_Pass1_Naked
    .global  ARMV6_g_IDCTDec_WMV3
    .global  ARMV6_SignPatch32

    .extern  DecodeInverseInterBlockQuantize16_EMB_Naked
    .extern  g_IDCTDec_WMV3_Pass2_Naked
    .extern  g_IDCTDec_WMV3_Pass4_Naked
    
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
    @AREA    |.text|, CODE
    WMV_LEAF_ENTRY ARMV6_g_IDCTDec16_WMV3_SSIMD
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ r0 == piDst
@ r1 == piSrc
@ r2 == iOffsetToNextRowForDCT
@ r3 == iDCTHorzFlags

.equ ST_piDst_4x4IDCTDec       , 0
.equ ST_tmpBuffer_4x4IDCTDec   , 4
.equ ST_SIZE_4x4IDCTDec        , 260

    stmdb     sp!, {r4 - r11, lr}
    FRAME_PROFILE_COUNT

    sub       sp, sp, #ST_SIZE_4x4IDCTDec
    str       r0, [sp, #ST_piDst_4x4IDCTDec]

@   g_IDCTDec_WMV3_Pass1(piSrc0, blk32, 4, iDCTHorzFlags)@
    mov       r0, r1
    mov       r2, #4
    add       r1, sp, #ST_tmpBuffer_4x4IDCTDec
    bl        ARMV6_g_IDCTDec_WMV3_Pass1_Naked

@   g_IDCTDec_WMV3_Pass2(piSrc0, blk16, 4)@
    mov       r2, #4
    ldr       r1, [sp, #ST_piDst_4x4IDCTDec]
    add       r0, sp, #ST_tmpBuffer_4x4IDCTDec
    bl        g_IDCTDec_WMV3_Pass2_Naked

    add       sp, sp, #ST_SIZE_4x4IDCTDec
    ldmia     sp!, {r4 - r11, pc}
    WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA    |.text|, CODE
    WMV_LEAF_ENTRY ARMV6_g_DecodeInterError8x8_SSIMD
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@r0 == pPMainLoop
@r1 == ppInterDCTTableInfo_Dec
@r2 == pDQ

.equ ST_result_8x8      , 0
.equ ST_blk16_8x8       , 4
.equ ST_tmpBuffer_8x8   , 8
.equ ST_SIZE_8x8        , 264

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
    bl        DecodeInverseInterBlockQuantize16_EMB_Naked
    str       r0, [sp, #ST_result_8x8]

@   UnionBuffer * ppxliErrorQMB == pPMainLoop->m_rgiCoefReconBuf@
@   I32_WMV *piSrc0 == ppxliErrorQMB->i32@
@   I16_WMV *blk16  == ppxliErrorQMB->i16@  
    	
@   g_IDCTDec_WMV3_Pass1(piSrc0, tmpBuffer, 4, pPMainLoop->m_iDCTHorzFlags)@
    ldr       r0, [sp, #ST_blk16_8x8]
    mov       r3, r8
    mov       r2, #4
    add       r1, sp, #ST_tmpBuffer_8x8
    bl        ARMV6_g_IDCTDec_WMV3_Pass1_Naked

@   g_IDCTDec_WMV3_Pass2(tmpBuffer, blk16, 4)@

    ldr       r1, [sp, #ST_blk16_8x8]
    mov       r2, #4
    add       r0, sp, #ST_tmpBuffer_8x8
    bl        g_IDCTDec_WMV3_Pass2_Naked
    
@   return result@
    ldr       r0, [sp, #ST_result_8x8]

    add       sp, sp, #ST_SIZE_8x8
    ldmia     sp!, {r4 - r11, pc}
    WMV_ENTRY_END

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA    |.text|, CODE
    WMV_LEAF_ENTRY ARMV6_g_DecodeInterError8x4_SSIMD
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@r0 == pPMainLoop
@r1 == ppInterDCTTableInfo_Dec
@r2 == pDQ
@r3 == i

.equ ST_result_8x4      , 0
.equ ST_blk16_8x4       , 4
.equ ST_tmpBuffer_8x4   , 8
.equ ST_SIZE_8x4        , 72

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
    bl        DecodeInverseInterBlockQuantize16_EMB_Naked
    str       r0, [sp, #ST_result_8x4]

@   I16_WMV  * blk16@
@   const I32_WMV  * piSrc0 == pPMainLoop->m_rgiCoefReconBuf->i32@
	
@   g_IDCTDec_WMV3_Pass1(piSrc0, tmpBuffer, 2, pPMainLoop->m_iDCTHorzFlags)@
    mov       r2, #2
    ldr       r0, [r3, #tagEMB_PBMainLoop_m_rgiCoefReconBuf]
    add       r1, sp, #ST_tmpBuffer_8x4
    mov       r3, r8
    bl        ARMV6_g_IDCTDec_WMV3_Pass1_Naked

@   blk16  == (I16_WMV *)piDst@
@   g_IDCTDec_WMV3_Pass4(tmpBuffer,  blk16, 4)@

    mov       r2, #4
    ldr       r1, [sp, #ST_blk16_8x4]
    add       r0, sp, #ST_tmpBuffer_8x4
    bl        g_IDCTDec_WMV3_Pass4_Naked

@   return result@
    ldr       r0, [sp, #ST_result_8x4]

    add       sp, sp, #ST_SIZE_8x4
    ldmia     sp!, {r4 - r11, pc}
    WMV_ENTRY_END

@
@ Void_WMV g_IDCTDec_WMV3_Pass1(const I32_WMV  *piSrc0, I32_WMV * blk32, const I32_WMV iNumLoops, I32_WMV iDCTHorzFlags)
@ 

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA    |.text|, CODE
    WMV_LEAF_ENTRY ARMV6_g_IDCTDec_WMV3_Pass1_Naked
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@   stmdb   sp!, {r4 - r11, lr}
@   stmdb   sp!, {lr}
    str     lr,  [sp, #-4]!

    FRAME_PROFILE_COUNT

@	pld		[r0]       
@	pld		[r0, #32]  
@	pld		[r0, #64]  
@	pld		[r0, #96]  
				
    mov     r4, #4

Pass1LoopStart:

    TST     r3, #3
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

	mov		r7, r6
	mov		r9, r8
    strd    r6, [r1], #8
    strd    r6, [r1], #8
    strd    r8, [r1], #8
    strd    r8, [r1], #8

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
    mov     r3,  r3,  asr #2        @ iDCTHorzFlags >>== 2
    bne     Pass1LoopStart

@   ldmia     sp!, {r4 - r11, pc}
@   ldmia     sp!, {pc}
    ldr       pc,  [sp], #4
    
    WMV_ENTRY_END	@ARMV6_g_IDCTDec_WMV3_Pass1_Naked

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
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    WMV_LEAF_ENTRY ARMV6_SignPatch32
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    stmdb     sp!, {r4-r9, lr}
    FRAME_PROFILE_COUNT

    mov     r14, r0

    mov   r12, #0xFF
    add   r12, r12, r12, LSL #8
	pld	  [r14, #32]

SignPatch32_loop:
	pld	  [r14, #64]
    ldrd     r2, [r14], #+8
    ldrd     r4, [r14], #+8
    ldrd     r6, [r14], #+8
    ldrd     r8, [r14], #+8

    add     r3, r3, r2, asr #16
    and     r2, r2, r12
    orr     r2, r2, r3, LSL #16

    add     r5, r5, r4, asr #16
    and     r4, r4, r12
    orr     r4, r4, r5, LSL #16

    add     r7, r7, r6, asr #16
    and     r6, r6, r12
    orr     r6, r6, r7, LSL #16

    add     r9, r9, r8, asr #16
    and     r8, r8, r12
    orr     r8, r8, r9, LSL #16

    str     r2, [r0], #+4
    str     r4, [r0], #+4
    str     r6, [r0], #+4
    str     r8, [r0], #+4

    subs    r1, r1, #16
    BGE    SignPatch32_loop

    ldmia     sp!, {r4-r9, pc}
    WMV_ENTRY_END
    @ENDP  @ |ARMV6_SignPatch32|

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @AREA |.text|, CODE, READONLY
    WMV_LEAF_ENTRY ARMV6_g_IDCTDec_WMV3
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
.equ Tmp_Buff_Size						, 32 * 4
.equ OFFSET_piDst						, Tmp_Buff_Size + 0
.equ OFFSET_iOffsetToNextRowForDCT		, Tmp_Buff_Size + 4
.equ Statck_Size						, Tmp_Buff_Size + 8

    stmdb     sp!, {r4 - r12, lr}
    FRAME_PROFILE_COUNT
		
    sub     sp, sp, #Statck_Size
    
    str		r0, [sp, #OFFSET_piDst]	
    str		r1, [sp, #OFFSET_iOffsetToNextRowForDCT]
    
	@ r2== rgiCoefRecon
    mov		r1, sp		@Tmp buff addr
    mov		r0, #4		@loop num
    mov     r4,  #4

Loop1_Start:

@for ( i == 0@ i < (BLOCK_SIZE>>1)@ i++,  blk32 +== 8){

@zeroth stage
@
@x4 == piSrc0[ i +1*4 ]
@x5 == piSrc0[ i +7*4 ]
@y3 == x4 + x5
@x8 == W3 * y3
@x4a == x8 - W3pW5 * x5
@x5a == x8 - W3_W5 * x4

    ldr     r8, [r2, #16]           @ x4 == [4]
    ldr     r9, [r2, #112]          @ x5 == [28*4]
    ldr     r6, [r2, #80]           @ x6
    ldr     r7, [r2, #48]           @ x7
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

@x0 == piSrc0[ i +0*4 ]@ /* for proper rounding */
@x1 == piSrc0[ i +4*4 ]@
@x1 == x1 * W0@  //12
@x0 == x0 * W0 + (4+(4<<16))@ /* for proper rounding */
@x8 == x0 + x1@
@x0 -== x1@

    ldr     lr, [r2], #4            @ x0 == r6
    sub     r8,  r8, r6, asl #1
    addne   r9,  r9, r10            @ x5 +== x8 - W3pW5 * x7
    add     r6, r4, lr, asl #2      @ x0*4+(4+(4<<16))
    add     r6, r6, r4, lsl #16
    add     r6, r6, lr, asl #3      @     +x0*8

    ldr     lr, [r2, #60]           @ x1 == r7
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

    ldr     lr, [r2, #28]           @ x3
    subne   r6, r6, r7              @ x0
    ldr     r7, [r2, #92]           @ x2

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
    mov     r7, r7, lsl #16
    
    mov     r4, r4, asr #19
    mov     r7, r7, asr #19
    
    mov     r4, r4, asl #16
    add     r4, r4, r5, asr #19     @ (c1<<16) + b1
    str     r4, [r1, #16]           @ blk32[4]

@ store blk32[3], blk32[7]
@   r11==b0  r8==c0  lr,r7,r5,r4 free

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

    adds    lr, r9, r4, lsl #13    
    add     r6, r6, r10, asl #16    @ blk32[3] == (c0<<16) + b0
    movne   lr, lr, asr #19         @ b1 == (b0 + 0x8000)>>19
    movs    r9, r9, lsl #16
    str     r5, [r1, #20]
    movne   r9, r9, asr #19       @ ((I16_WMV)b0)>>3
    add     r10, r12, r4, lsl #13    
    str     r6, [r1, #4]
    movs    r12, r12, lsl #16
    mov     r10, r10, asr #19         @ c1 == (c0 + 0x8000)>>19
    movne   r12, r12, asr #19         @ ((I16_WMV)c0)>>3
    add     r10, lr, r10, asl #16     @ blk32[7] == (c1<<16) + b1
    add     r9, r9, r12, asl #16    @ blk32[3] == (c0<<16) + b0
    str     r9, [r1, #8]
    str     r10, [r1, #24]

    subs    r0, r0, #1               @ if (i<iNumLoops)
    add     r1,  r1,  #32           @ blk32+==8
    bne     Loop1_Start
@}

    mov		r0, sp						@Tmp buff addr
    ldr		r1, [sp, #OFFSET_piDst]		@dst addr
    mov		r2, #4						@loop num
  
@piSrc0 == tmpBuffer@ //piDst->i32@    
@blk0 == piDst@
@blk1 == blk0 + iOffsetToNextRowForDCT@
@blk2 == blk1 + iOffsetToNextRowForDCT@
@blk3 == blk2 + iOffsetToNextRowForDCT@
@blk4 == blk3 + iOffsetToNextRowForDCT@
@blk5 == blk4 + iOffsetToNextRowForDCT@
@blk6 == blk5 + iOffsetToNextRowForDCT@
@blk7 == blk6 + iOffsetToNextRowForDCT@
    
Loop2_Start:

@for (i == 0@ i < (BLOCK_SIZE>>1)@ i++){

@x4 == piSrc0[i + 1*4 ]@
@x5 == piSrc0[i + 7*4 ]@
@y4a == x4 + x5@
@x8 == 7 * y4a@
@x4a == x8 - 12 * x5@
@x5a == x8 - 3 * x4@
@  zeroth stage

    ldr     r8,  [r0, #16]          @ x4
    ldr     r9,  [r0, #112]         @ x5
    adds    lr,  r8, r9             @ y4a
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

    and     r4,  lr, #0x80, 24		@0x8000
    rsb     lr,  r4, lr, asr #1
    bic     lr,  lr, #0x80, 24
    orrs    lr,  lr, r4             @ y4a

@x6 == piSrc0[i + 5*4 ]@
@x7 == piSrc0[i + 3*4 ]@
@y4 == x6 + x7@
@x8 == 2 * y4@
@x4a -== x8 + 6 * x6@
@x5a +== x8 - 10 * x7@
@  first stage

    ldr     r10, [r0, #80]          @ x6
    ldr     r11, [r0, #48]          @ x7
    addne   r6,  r6, lr             @ x4a
    addne   r7,  r7, lr             @ x5a
    adds    lr,  r10, r11           @ y4
    mov		r3, #32
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

    ands    r4,  lr, #0x80, 24
    rsb     lr,  r4, lr, asr #1
    bic     lr,  lr, #0x80, 24
    orrnes  lr,  lr, r4             @ y4
    adds    r12, r12, lr           @ y4+x8
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
    addne   r8,  r8, r12
    addne   r9,  r9, r12
    add     r4,  r3, r3, lsl #16
    add     r4,  r4, lr, asl #2
    add     r4,  r4, lr, asl #1     @ x0

    ldr     lr,  [r0, #60]          @ [4*4]
    sub     r8,  r8, r10, asl #1
    sub     r8,  r8, r10            @ x4
    sub     r9,  r9, r11, asl #3
    adds    lr,  lr, lr, asl #1      
    ldr     r5,  [r0, #28]          @ x3
    add     r12, r4, lr, asl #1     @ x8
    subne   r4,  r4, lr, asl #1     @ x0

    ldr     lr,  [r0, #92]          @ x2
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
@r3, r5, lr are free. 
@here set r3==iOffsetToNextRowForDCT. r5, lr are temp.
@

@j == i<<1@
	 
@// blk0

    adds		r5,  r11, r8            @b0 == (x7 + x4)
    addne		lr,  r5,  #0x80, 24		@b1 == (b0 + 0x8000)
    movne		r5,  r5,  lsl #16
    movne		lr,  lr,  asr #6            
	pkhtbne		r5, lr, r5, asr #22		@pack as |x|b1|x|b0|
	usat16ne	r5, #8, r5				@SATURATE8(b1), SATURATE8(b0)
	ldr			r3, [sp, #OFFSET_iOffsetToNextRowForDCT]
	orrne		r5, r5, r5, lsr	#8		@|0|b1|b1|b0|
	strh		r5, [r1]

@// blk7

    subs		r11, r11, r8			@b0
    addne		lr, r11, #0x80, 24		@b1
    movne		r11, r11, lsl #16
    movne		lr, lr, asr #6      
	pkhtbne		r5, lr, r11, asr #22	@pack as |x|b1|x|b0|
	usat16ne	r5, #8, r5				@SATURATE8(b1), SATURATE8(b0)
    rsb			r8, r3, r3, lsl #3		@dst + 7 * iOffsetToNextRowForDCT
	orrne		r5, r5, r5, lsr	#8		@|0|b1|b1|b0|
    add			r8, r8, r1
	strh		r5, [r8]
    
@// blk1

    adds		r11,  r10, r6            @ b0
    addne		lr,  r11,  #0x80, 24     @ b1
    movne		r11,  r11,  lsl #16
    movne	    lr,  lr,  asr #6  
	pkhtbne		r5, lr, r11, asr #22		@pack as |x|b1|x|b0|
	usat16ne	r5, #8, r5				@SATURATE8(b1), SATURATE8(b0)
    add			r8, r3, r1
	orrne		r5, r5, r5, lsr	#8		@|0|b1|b1|b0|
	strh		r5, [r8]

@// blk6

    subs		r11, r10, r6           @ b0 
    addne		lr,  r11, #0x80, 24    @ b1
    movne		r11,  r11,  lsl #16
    movne		lr,  lr,  asr #6
	pkhtbne		r5, lr, r11, asr #22	@pack as |x|b1|x|b0|
	usat16ne	r5, #8, r5				@SATURATE8(b1), SATURATE8(b0)
    add			r8, r1, r3, lsl #2		@dst + 6 * iOffsetToNextRowForDCT
	orrne		r5, r5, r5, lsr	#8		@|0|b1|b1|b0|
    add			r8, r8, r3, lsl #1
	strh		r5, [r8]

@// blk2

    adds		r11,  r4, r7            @ b0
    addne		lr,  r11,  #0x80, 24    @ b1
    movne		r11,  r11,  lsl #16
    movne       lr,  lr,  asr #6   
	pkhtbne		r5, lr, r11, asr #22	@pack as |x|b1|x|b0|
	usat16ne	r5, #8, r5				@SATURATE8(b1), SATURATE8(b0)
    add			r8, r1, r3, lsl #1		@dst + 2 * iOffsetToNextRowForDCT
	orrne		r5, r5, r5, lsr	#8		@|0|b1|b1|b0|
	strh		r5, [r8]

@// blk5

    subs		r11, r4, r7			    @ b0
    addne	    lr,  r11, #0x80, 24     @ b1
    movne		r11,  r11,  lsl #16
    movne       lr,  lr,  asr #6  
	pkhtbne		r5, lr, r11, asr #22	@pack as |x|b1|x|b0|
	usat16ne	r5, #8, r5				@SATURATE8(b1), SATURATE8(b0)
    add			r8, r3, r3, lsl #2		@dst + 5 * iOffsetToNextRowForDCT
	orrne		r5, r5, r5, lsr	#8		@|0|b1|b1|b0|   
    add			r8, r8, r1
	strh		r5, [r8]
    
@// blk3

    adds	    r11,  r12, r9            @ b0
    addne		lr,  r11,  #0x80, 24    @ b1
    movne		r11,  r11,  lsl #16
    movne       lr,  lr,  asr #6   
	pkhtbne		r5, lr, r11, asr #22	@pack as |x|b1|x|b0|
	usat16ne	r5, #8, r5				@SATURATE8(b1), SATURATE8(b0)
    rsb			r8, r3, r3, lsl #2		@dst + 3 * iOffsetToNextRowForDCT
	orrne		r5, r5, r5, lsr	#8		@|0|b1|b1|b0|   
    add			r8, r8, r1
	strh		r5, [r8]   

@// blk4

    subs	    r11, r12, r9            @ b0
    addne		lr,  r11, #0x80, 24     @ b1
    movne		r11,  r11,  lsl #16
    movne		lr,  lr,  asr #6 
	pkhtbne		r5, lr, r11, asr #22	@pack as |x|b1|x|b0|
	usat16ne	r5, #8, r5				@SATURATE8(b1), SATURATE8(b0)
    add			r8, r1, r3, lsl #2		    @dst + 4 * iOffsetToNextRowForDCT
	orrne		r5, r5, r5, lsr	#8		@|0|b1|b1|b0|   
	strh		r5, [r8]   

    subs    r2, r2, #1               @ if (i<iNumLoops)
    add     r1, r1, #2
    bne     Loop2_Start
@}

    add     sp, sp, #Statck_Size
    ldmia   sp!, {r4 - r12, pc}
	
    WMV_ENTRY_END
    @ENDP  @ |ARMV6_g_IDCTDec_WMV3|
    
    .ENDIF @ WMV_OPT_IDCT_ARM

    .END 
