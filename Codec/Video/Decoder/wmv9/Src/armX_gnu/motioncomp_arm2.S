    #include "../c/voWMVDecID.h"
    .include "wmvdec_member_arm.inc"
    .include "xplatform_arm_asm.h" 

   .if WMV_OPT_MOTIONCOMP_ARM == 1

	@@AREA	.rdata, DATA, READONLY
	.section .rdata
	.extern	MotionCompMixed010Complete
    .extern  MotionCompMixed110Complete
    .extern  MotionCompMixed001Complete	
    .extern  MotionCompMixedAlignBlockComplete    
        
		.global MotionCompMixed110
    .global MotionCompMixed010	
    .global  MotionCompMixed001 
    .global  MotionCompMixedAlignBlock        
  
  
    .extern  MotionCompMixed011Complete  
    .extern  MotionCompWAddError10Complete
    .extern  MotionCompWAddError01Complete
                   
    .global  MotionCompMixed011     
    .global  MotionCompWAddError10
    .global  MotionCompWAddError01  
         
@PRESERVE8
	@@AREA	.text, CODE, READONLY
	.section .text

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@//Void MotionCompMixed010(PixelC*   ppxlcPredMB, const PixelC*  ppxlcRefMB, Int iWidthPrev, I32 * pErrorBuf )
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


    .if ARCH_V3 == 1
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        .macro             
        LOADONE16bitsLo $srcRN, $offset, $dstRN
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        LDR     $dstRN, [$srcRN, $offset]
        MOV     $dstRN, $dstRN, LSL #16
        MOV     $dstRN, $dstRN, ASR #16
        .endm
    .endif @ //ARCH_V3

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro AndAddError pErrorBuf, err_even, err_odd, u0, u1, err_overflow, scratch

@// err_even == pErrorBuf[0]@

    LDR \err_even, [\pErrorBuf], #0x10

@// err_odd  == pErrorBuf[0 + 32]@

    LDR \err_odd, [\pErrorBuf, #0x70]


@//pErrorBuf +== 4@



@//u0 == u0 + err_even-((err_even & 0x8000) << 1)@

    AND \scratch, \err_even, #0x8000
    SUB \scratch, \err_even, \scratch, LSL #1
    ADD \u0, \u0, \scratch

@//err_overflow  == u0@
    ORR \err_overflow, \err_overflow, \u0

@//u1 == u1 + err_odd -((err_odd  & 0x8000) << 1)@

    AND \scratch,\err_odd,#0x8000
    SUB \scratch,\err_odd,\scratch, LSL #1
    ADD \u1, \u1, \scratch

@//err_overflow  == u1@
    ORR \err_overflow, \err_overflow, \u1

    .endm

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    .macro AddErrorP0 err_even, err_odd, u0, u1, err_overflow, scratch
@//u0 == u0 + err_even-((err_even & 0x8000) << 1)@

    AND \scratch, \err_even, #0x8000
    SUB \scratch, \err_even, \scratch, LSL #1
    ADD \u0, \u0, \scratch

@//err_overflow  == u0@
    ORR \err_overflow, \err_overflow, \u0

@//u1 == u1 + err_odd -((err_odd  & 0x8000) << 1)@

    AND \scratch,\err_odd,#0x8000
    SUB \scratch,\err_odd,\scratch, LSL #1
    ADD \u1, \u1, \scratch

@//err_overflow  == u1@
    ORR \err_overflow, \err_overflow, \u1

    .endm
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro CubicFilter u0, t0, t1, t2, t3, const, scratch
@   //u0 == ((t1 + t2)*9-(t0 + t3) + 0x00080008)@
    
    ADD \scratch, \t1, \t2  
    ADD \scratch, \scratch, \scratch, LSL #3
    SUB \u0, \scratch, \t0
    SUB \u0, \u0, \t3
    ADD \u0, \u0, \const

    .endm
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro CubicFilterShort u0, t0, t1, t2, const, scratch
@   //u0 == ((t1 + t2)*9-(t0 ) + 0x00080008)@
    
    ADD \scratch, \t1, \t2  
    ADD \scratch, \scratch, \scratch, LSL #3
    SUB \u0, \scratch, \t0
    ADD \u0, \u0, \const

    .endm
    
    WMV_LEAF_ENTRY MotionCompMixed010

@t0==r4
@t1==r5
@t2==r6
@t3==r7
@t4==r8
@u0==r4
@u1==r5
@err_even==r6
@err_odd==r7
@y0==r4
@overflow==r9
@err_overflow==r10
@0x00080008==r11
@iy==r12
@ppxlcPredMB==r0
@ppxlcRefMB==r1
@pErrorBuf2==r3
@pErrorBuf==r2
@r14 scratch
@stack saved area
@   iWidthPrev


    STMFD   sp!, {r4 - r12, r14} @ //r0-r3 are preserved
    FRAME_PROFILE_COUNT


@// U32 err_overflow == 0@

    MOV r10, #0

@// U32 overflow == 0@

    MOV r9, #0

@//U32 mask == 0x00ff00ff@

@//0x00080008
    MOV r11,#0x00080000
    ORR r11, r11, #0x8

@//iWidthPrev  &== 0xffff@

    BIC r2, r2, #0x10000

    ADD r1, r1, #5

@//for(Int iz==0@iz<2@iz++)

MCM010_Loop0:
    
    
@//for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    MOVS r12, #7

MCM010_Loop1:

@//t0 == ppxlcRefMB[-1]  (ppxlcRefMB[-1 + 2] << 16)@

    LDRB r4, [r1, #-6]

    LDRB r14, [r1, #-4]
@@relo0    ORR r4, r4, r14, lsl #16

@// t1 == ppxlcRefMB[0]  (ppxlcRefMB[0 + 2] << 16)@
    LDRB r5, [r1, #-5]
    ORR r4, r4, r14, lsl #16 @@ relo0
    LDRB r14, [r1, #-3]
@@relo1 ORR r5, r5, r14, lsl #16

@// t2 == ppxlcRefMB[1]  (ppxlcRefMB[1 + 2] << 16)@
    LDRB r6, [r1, #-4]
    ORR r5, r5, r14, lsl #16 @@ relo1
    LDRB r14, [r1, #-2]
@@relo2 ORR r6, r6, r14, lsl #16

@// t3 == ppxlcRefMB[2]  (ppxlcRefMB[2 + 2] << 16)@
    LDRB r7, [r1, #-3]
    ORR r6, r6, r14, lsl #16 @@ relo2
    LDRB r14, [r1, #-1]
@@//relo3   ORR r7, r7, r14, lsl #16

@//t4 == ppxlcRefMB[3]  (ppxlcRefMB[3 + 2] << 16)@
@// ppxlcRefMB   +==  iWidthPrev@
    LDRB r8, [r1, #-2]
    ORR r7, r7, r14, lsl #16 @@relo3

    .if DYNAMIC_EDGEPAD==1
    LDRB r14, [r1], r2, LSR #17
    .else
    LDRB r14, [r1], r2
    .endif @.if DYNAMIC_EDGEPAD==1
    
@@relo4 ORR r8, r8, r14, lsl #16

@//u0 == ((t1 + t2)*9-(t0 + t3) + 0x00080008)@

@@macro@ CubicFilterShort $u0, $t0, $t1, $t2,  $const, $scratch

    ADD r4, r4, r7

    ORR r8, r8, r14, lsl #16@@ relo4

    CubicFilterShort r4, r4, r5, r6, r11, r14

@//overflow  == u0@ 
    ORR r9, r9, r4

@// u0 == u0>>4@
    MOV r4, r4, LSR #4

@//u1 == ((t2 + t3)*9-(t1 + t4) + 0x00080008)@
@@macro@ CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r5, r5, r6, r7, r8, r11, r14
    
@//overflow  == u1@ 
    ORR r9, r9, r5

@// u1 == u1>>4@
    MOV r5, r5, LSR #4

@// u0  &== mask@

    BIC r4, r4, #0xff00


@// u1  &== mask@

    BIC r5, r5, #0xff00

@ //   if(pErrorBuf2 != NULL)

    CMP r3, #0
 	BLT	AndAddError_010
 	CMP r3, #4
    BLE MCM010_L0


@@@Macro: AndAddError $pErrorBuf, $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch
AndAddError_010:
    AndAddError r3, r6, r7, r4, r5, r10, r14



MCM010_L0:
         

@//y0 == (u0)  ((u1) << 8)@

    ORR r4, r4, r5, LSL #8

@//* (U32 *) ppxlcPredMB== y0@
@// ppxlcPredMB   +==  iWidthPrev@

@@relo5 LDR r8, [sp]

@@relo6 STR r4, [ r0 ], r8 

    .if DYNAMIC_EDGEPAD==1
    AND r5, r2, #0x3fc @r5==iWidthPrev, r8>>17==iWidthPrefRef
    .endif @.if DYNAMIC_EDGEPAD==1

@ // } //for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    SUBS r12, r12, #1

    .if DYNAMIC_EDGEPAD==1
    STR r4, [ r0 ], r5 @@relo6
    .else
    STR r4, [ r0 ], r2 @@relo6
    .endif

    BGE MCM010_Loop1



@//     ppxlcRefMB==ppxlcRefMB2+4@

    .if DYNAMIC_EDGEPAD==1
    SUB r1, r1, r2, LSR #14
    .else
    SUB r1, r1, r2, LSL #3
    .endif

    ADD r1, r1, #4

@//     ppxlcPredMB==ppxlcPredMB2+4@

    .if DYNAMIC_EDGEPAD==1
    SUB r0, r0, r5, LSL #3
    .else
    SUB r0, r0, r2, LSL #3
    .endif
    ADD r0, r0, #4

		CMP 	r3, #0
    BLT		NEXT_POS_010
    CMP 	r3, #4
    BLE		TEST_NEXT_HALF_010

NEXT_POS_010:
    SUB		r3, r3, #128

TEST_NEXT_HALF_010:
    TST r3, #4

@//     pErrorBuf==pErrorBuf2+1@

    ADD r3, r3, #4


    BEQ MCM010_Loop0


@//   } //for(Int iz==0@iz<2@iz++)


@//    if((err_overflow & 0xff00ff00)  (overflow & 0xf000f000))

    TST r10, #0xff000000
    TSTEQ r10, #0xff00
    TSTEQ r9, #0xf0000000
    TSTEQ r9, #0xf000
    LDMEQFD   sp!, {r4 - r12, PC}
    

@//MotionCompMixed010Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2)@
    
    SUB r0, r0, #8
    SUB r1, r1, #13
    SUB r3, r3, #8

    BL MotionCompMixed010Complete

    LDMFD   sp!, {r4 - r12, PC}
    WMV_ENTRY_END
    
    WMV_LEAF_ENTRY MotionCompMixed110

@t0==r4
@t1==r5
@t2==r6
@t3==r7
@t4==r8
@u0==r4
@u1==r5
@err_even==r6
@err_odd==r7
@y0==r4
@overflow==r9
@err_overflow==r10
@0x00080008==r11
@iy==r12
@ppxlcPredMB==r0
@ppxlcRefMB==r1
@pErrorBuf2==r3
@pErrorBuf==r2
@r14 scratch
@stack saved area
@   iWidthPrev


    STMFD   sp!, {r4 - r12, r14} @ //r0-r3 are preserved
    FRAME_PROFILE_COUNT


@// U32 err_overflow == 0@

    MOV r10, #0

@// U32 overflow == 0@

    MOV r9, #0

@//U32 mask == 0x00ff00ff@

@//0x00080008
    MOV r11,#0x00080000
    ORR r11, r11, #0x8

@//iWidthPrev  &== 0xffff@

    BIC r2, r2, #0x10000

    ADD r1, r1, #5

@//for(Int iz==0@iz<2@iz++)

MCM110_Loop0:
    
    
@//for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    MOVS r12, #7

MCM110_Loop1:

@//t0 == ppxlcRefMB[-1]  (ppxlcRefMB[-1 + 2] << 16)@

    LDRB r4, [r1, #-6]

    LDRB r14, [r1, #-4]
@@relo0 ORR r4, r4, r14, lsl #16

@// t1 == ppxlcRefMB[0]  (ppxlcRefMB[0 + 2] << 16)@
    LDRB r5, [r1, #-5]
    ORR r4, r4, r14, lsl #16 @@relo0
    LDRB r14, [r1, #-3]
@@relo1 ORR r5, r5, r14, lsl #16

@// t2 == ppxlcRefMB[1]  (ppxlcRefMB[1 + 2] << 16)@
    LDRB r6, [r1, #-4]
    ORR r5, r5, r14, lsl #16 @@relo1
    LDRB r14, [r1, #-2]
@@relo2 ORR r6, r6, r14, lsl #16

@// t3 == ppxlcRefMB[2]  (ppxlcRefMB[2 + 2] << 16)@
    LDRB r7, [r1, #-3]
    ORR r6, r6, r14, lsl #16 @@relo2
    LDRB r14, [r1, #-1]
@@relo3 ORR r7, r7, r14, lsl #16

@//t4 == ppxlcRefMB[3]  (ppxlcRefMB[3 + 2] << 16)@
@// ppxlcRefMB   +==  iWidthPrev@
    LDRB r8, [r1, #-2]
    ORR r7, r7, r14, lsl #16 @@relo3
    .if DYNAMIC_EDGEPAD==1
    LDRB r14, [r1], r2, LSR #17
    .else
    LDRB r14, [r1], r2
    .endif
@@relo4 ORR r8, r8, r14, lsl #16

@//u0 == ((t1 + t2)*9-(t0 + t3) + 0x00080008)@

    ADD r4, r4, r7

    ORR r8, r8, r14, lsl #16 @@relo4

@@macro@ CubicFilterShort $u0, $t0, $t1, $t2,  $const, $scratch

    CubicFilterShort r4, r4, r5, r6, r11, r14

@//overflow  == u0@ 
    ORR r9, r9, r4

@// u0 == u0>>4@
@//    u0 == (u0 + t2 + 0x00010001)>>1@
@   MOV r4, r4, LSR #4
    ADD r4, r4, r6, LSL #4
    ADD r4, r4, r11, LSL #1
    MOV r4, r4, LSR #5

@//u1 == ((t2 + t3)*9-(t1 + t4) + 0x00080008)@
@@macro@ CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r5, r5, r6, r7, r8, r11, r14

@//overflow  == u1@ 
    ORR r9, r9, r5

@// u1 == u1>>4@
@//    u1 == (u1 + t3 + 0x00010001)>>1@
@   MOV r5, r5, LSR #4
    ADD r5, r5, r7, LSL #4
    ADD r5, r5, r11, LSL #1
    MOV r5, r5, LSR #5



@// u0  &== mask@

    BIC r4, r4, #0xff00


@// u1  &== mask@

    BIC r5, r5, #0xff00

@ //   if(pErrorBuf2 != NULL)
    CMP r3, #0
    BLT	AndAddError_110
	CMP r3, #4
    BLE MCM110_L0    



@@@Macro: AndAddError $pErrorBuf, $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch
AndAddError_110:
    AndAddError r3, r6, r7, r4, r5, r10, r14



MCM110_L0:
         

@//y0 == (u0)  ((u1) << 8)@

    ORR r4, r4, r5, LSL #8

@//* (U32 *) ppxlcPredMB== y0@
@// ppxlcPredMB   +==  iWidthPrev@

@@relo5 LDR r8, [sp]

@@relo6 STR r4, [ r0 ], r8 

    .if DYNAMIC_EDGEPAD==1
    AND r5, r2, #0x3fc @r5==iWidthPrev, r8>>17==iWidthPrefRef
    .endif @.if DYNAMIC_EDGEPAD==1

@ // } //for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    SUBS r12, r12, #1

    .if DYNAMIC_EDGEPAD==1
    STR r4, [ r0 ], r5 @@relo6
    .else
    STR r4, [ r0 ], r2 @@relo6
    .endif
    
    BGE MCM110_Loop1



@//     ppxlcRefMB==ppxlcRefMB2+4@

    .if DYNAMIC_EDGEPAD==1
    SUB r1, r1, r2, LSR #14
    .else
    SUB r1, r1, r2, LSL #3
    .endif
    
    ADD r1, r1, #4

@//     ppxlcPredMB==ppxlcPredMB2+4@

    .if DYNAMIC_EDGEPAD==1
    SUB r0, r0, r5, LSL #3
    .else
    SUB r0, r0, r2, LSL #3
    .endif

    ADD r0, r0, #4

    CMP 	r3, #0
    BLT		NEXT_POS_110
    CMP 	r3, #4
    BLE		TEST_NEXT_HALF_110

NEXT_POS_110:
    SUB		r3, r3, #128

TEST_NEXT_HALF_110:
    TST r3, #4

@//     pErrorBuf==pErrorBuf2+1@

    
    ADD r3, r3, #4


    BEQ MCM110_Loop0


@//   } //for(Int iz==0@iz<2@iz++)


@//    if((err_overflow & 0xff00ff00)  (overflow & 0xf000f000))

    TST r10, #0xff000000
    TSTEQ r10, #0xff00
    TSTEQ r9, #0xf0000000
    TSTEQ r9, #0xf000
    LDMEQFD   sp!, {r4 - r12, PC}
    

@//MotionCompMixed110Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2)@
    
    SUB r0, r0, #8
    SUB r1, r1, #13
    SUB r3, r3, #8
    

    BL MotionCompMixed110Complete

    LDMFD   sp!, {r4 - r12, PC}
    WMV_ENTRY_END


    WMV_LEAF_ENTRY MotionCompMixed001

@t_even_1==r4
@t_even_2==r5
@t_even_3==r6
@t_even_4==r7
@t_odd_1==r5
@t_odd_2==r6
@t_odd_3==r7
@t_odd_4==r8
@u0==r4
@u1==r5
@err_even==r6
@err_odd==r7
@y0==r4
@overflow==r9
@err_overflow==r10
@0x00080008==r11
@iy==r12
@ppxlcPredMB==r0
@ppxlcRefMB==r1
@pErrorBuf2==r3
@pErrorBuf==r2
@r14 scratch

@stack saved area
@   iWidthPrev
@   t_even_2,3,4
@   t_odd_2,3,4

    STMFD   sp!, {r4 - r12, r14} @ //r0-r3 are preserved
    FRAME_PROFILE_COUNT

    @// U32 err_overflow == 0@

    MOV r10, #0

@// U32 overflow == 0@

    MOV r9, #0

@//U32 mask == 0x00ff00ff@

@//0x00080008
    MOV r11,#0x00080000
    ORR r11, r11, #0x8

@//iWidthPrev  &== 0xffff@

    BIC r2, r2, #0x10000

    SUB sp, sp, #24 @ make room for t_even/odd_2,3,4

@//const PixelC* pLine  ==  ppxlcRefMB + 2*iWidthPrev+1@

    .if DYNAMIC_EDGEPAD==1
    ADD r1, r1, r2, LSR #16
    .else
    ADD r1, r1, r2, LSL #1
    .endif
    ADD r1, r1, #1

@//for(Int iz==0@iz<2@iz++)

MCM001_Loop0:

@ initially pLine==r1==ppxlcRefMB2+2*iWidthPrev+1, so odd first

@//t_odd_4 == pLine[1]  pLine[3] << 16@
@//pLine   -==  iWidthPrev@

    .if DYNAMIC_EDGEPAD==1
    LDRB r8, [r1, -r2, LSR #17 ]!   
    .else
    LDRB r8, [r1, -r2 ]!
    .endif   
    LDRB r14, [r1, #2]  
@@relo0 ORR r8, r8, r14, lsl #16

@//t_odd_3 == pLine[1]  pLine[3] << 16@
@//pLine   -==  iWidthPrev@

    .if DYNAMIC_EDGEPAD==1
    LDRB r7, [r1, -r2, LSR #17 ]!       
    .else
    LDRB r7, [r1, -r2 ]!    
    .endif
    ORR r8, r8, r14, lsl #16 @@ relo0
    LDRB r14, [r1, #2]  
@@relo1 ORR r7, r7, r14, lsl #16
        
@//t_odd_2 == pLine[1]  pLine[3] << 16@
@//pLine   -==  iWidthPrev@

    .if DYNAMIC_EDGEPAD==1
    LDRB r6, [r1, -r2, LSR #17]!
    .else
    LDRB r6, [r1, -r2 ]!
    .endif       
    ORR r7, r7, r14, lsl #16 @@relo1
    LDRB r14, [r1, #2]  

    SUB r1, r1, #1 @@relo2

    ORR r6, r6, r14, lsl #16

@ save t_odd_2,3,4 now

    STMIA sp, { r6 - r8 }

@ now deal with the t_even_2,3,4

@@relo2 SUB r1, r1, #1

@//t_even_2 == pLine[0]  pLine[2] << 16@
@//pLine   +==  iWidthPrev@

    LDRB r6, [r1, #2]
    .if DYNAMIC_EDGEPAD==1
    LDRB r5, [r1], r2, LSR #17
    .else
    LDRB r5, [r1], r2
    .endif
@@relo3 ORR r5, r5, r6, lsl #16

@//t_even_3 == pLine[0]  pLine[2] << 16@
@//pLine   +==  iWidthPrev@

        
    LDRB r7, [r1, #2]
    ORR r5, r5, r6, lsl #16 @@relo3
    .if DYNAMIC_EDGEPAD==1
    LDRB r6, [r1], r2, LSR #17
    .else
    LDRB r6, [r1], r2
    .endif
@@relo4 ORR r6, r6, r7, lsl #16
    

@//t_even_4 == pLine[0]  pLine[2] << 16@
@//pLine   +==  iWidthPrev@

        
    LDRB r14, [r1, #2]
    ORR r6, r6, r7, lsl #16 @@relo4
    .if DYNAMIC_EDGEPAD==1
    LDRB r7, [r1], r2, LSR #17
    .else
    LDRB r7, [r1], r2
    .endif
    ADD r8, sp, #12 @@relo5
    ORR r7, r7, r14, lsl #16

@@relo5 ADD r8, sp, #12
    STMIA r8, { r5 - r7 }

@//for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    MOVS r12, #7

MCM001_Loop1:

@ r1 starts offset by 0, so load even regs first, r8== sp+12

@@relo6 LDMIA r8, { r4 - r6 }

@//t_even_4 == pLine[0]  pLine[2] << 16@

    LDRB r7, [r1]

    LDRB r14, [r1, #2]
    LDMIA r8, { r4 - r6 } @@relo6

    ORR r7, r7, r14, lsl #16

@// u0 == ((t_even_2 + t_even_3)*9-(t_even_1 + t_even_4) + 0x00080008)@

@@macro@ CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r4, r4, r5, r6, r7, r11, r14

@ now save t_even_2,3,4

    STMIA r8, { r5 - r7 }

    LDRB r8, [r1, #1] @@relo7
    LDRB r14, [r1, #3] @@relo8

@//overflow  == u0@ 
    ORR r9, r9, r4

@// u0 == u0>>4@
    MOV r4, r4, LSR #4

@// u0  &== mask@

    BIC r4, r4, #0xff00

@ now load t_odd_2,3,4

    LDMIA sp, { r5 - r7 }
    
@//t_odd_4 == pLine[1]  pLine[3] << 16@

@@relo7 LDRB r8, [r1, #1]
@@relo8 LDRB r14, [r1, #3]
    ORR r8, r8, r14, lsl #16
    
@//u1 == ((t_odd_2 + t_odd_3)*9-(t_odd_1 + t_odd_4) + 0x00080008)@

@@macro@ CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r5, r5, r6, r7, r8, r11, r14

@ now save t_odd_2,3,4

    STMIA sp, { r6 - r8 }

@//overflow  == u1@ 
    ORR r9, r9, r5

@// u1 == u1>>4@
    MOV r5, r5, LSR #4


@// u1  &== mask@

    BIC r5, r5, #0xff00

@ //   if(pErrorBuf2 != NULL)

    CMP r3, #0
    BLT AndAddError_001
    CMP r3, #4
    BLE MCM001_L0    



@@@Macro: AndAddError $pErrorBuf, $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch
AndAddError_001:
    AndAddError r3, r6, r7, r4, r5, r10, r14

MCM001_L0:
         

@//y0 == (u0)  ((u1) << 8)@

    ORR r5, r4, r5, LSL #8

@//* (U32 *) ppxlcPredMB== y0@
@// ppxlcPredMB   +==  iWidthPrev@

@@relo9 STR r5, [ r0 ], r4 

            
@// ppxlcRefMB   +==  iWidthPrev@

@@relo10    ADD r1, r1, r4 @ r1 still even

    

@ // } //for (Int iy  ==  0@ iy < 8@ iy ++ ) 

@  prepare r8 for the loop start
    ADD r8, sp, #12

    .if DYNAMIC_EDGEPAD==1
    AND r7, r2, #0x3fc @r7==iWidthPrev, r4>>17==iWidthPrefRef
    ADD r1, r1, r2, LSR #17  @@relo10 @r1 still even
    .else
@// ppxlcRefMB   +==  iWidthPrev@
    ADD r1, r1, r2 @@relo10 @r1 still even
    .endif @.if DYNAMIC_EDGEPAD==1

    .if DYNAMIC_EDGEPAD==1
    STR r5, [ r0 ], r7 @@relo9
    .else
    STR r5, [ r0 ], r2 @@relo9
    .endif

    SUBS r12, r12, #1
    BGE MCM001_Loop1

@//     ppxlcRefMB==ppxlcRefMB2+4@

    .if DYNAMIC_EDGEPAD==1
    SUB r1, r1, r2, LSR #14  @ this brings r1 back to ppxlcRefMB2+2*iWidthPrev
    .else
    SUB r1, r1, r2, LSL #3  @ this brings r1 back to ppxlcRefMB2+2*iWidthPrev
    .endif
    ADD r1, r1, #5  @ 4+1 make sure r1 is odd now

@//     ppxlcPredMB==ppxlcPredMB2+4@

    .if DYNAMIC_EDGEPAD==1
    SUB r0, r0, r7, LSL #3
    .else
    SUB r0, r0, r2, LSL #3
    .endif
    ADD r0, r0, #4

    CMP 	r3, #0
    BLT		NEXT_POS_001
    CMP 	r3, #4
    BLE		TEST_NEXT_HALF_001

NEXT_POS_001:
    SUB		r3, r3, #128

TEST_NEXT_HALF_001:
    TST r3, #4

@//     pErrorBuf==pErrorBuf2+1@

    
    ADD r3, r3, #4


    BEQ MCM001_Loop0

@//   } //for(Int iz==0@iz<2@iz++)

    ADD sp, sp, #24 @ adjust the stack now

@//    if((err_overflow & 0xff00ff00)  (overflow & 0xf000f000))

    TST r10, #0xff000000
    TSTEQ r10, #0xff00
    TSTEQ r9, #0xf0000000
    TSTEQ r9, #0xf000
    LDMEQFD   sp!, {r4 - r12, PC}
    

@//MotionCompMixed001Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2)@
    
    SUB r0, r0, #8
    SUB r1, r1, #9
    .if DYNAMIC_EDGEPAD==1
    SUB r1, r1, r2, LSR #16
    .else
    SUB r1, r1, r2, LSL #1
    .endif
    SUB r3, r3, #8

    BL MotionCompMixed001Complete

    LDMFD   sp!, {r4 - r12, PC}
    WMV_ENTRY_END    
    
    WMV_LEAF_ENTRY MotionCompMixedAlignBlock

@t0==r4
@t1==r5
@t2==r6
@t3==r7
@t4==r8
@t5==r7
@t6==r6
@t7==r3
@t8==r10
@u0==r4
@u1==r5
@u2==r8
@u3==r5
@y0==r4
@y1==r5
@overflow==r9
@0x00080008==r11
@iy==r12
@pBlock==r0
@ppxlcRefMB==r1
@pLine==r1@
@iWidthPrev==r2
@r14==scratch



    STMFD   sp!, {r4 - r12, r14} @ //r0-r3 are preserved
    FRAME_PROFILE_COUNT

@// U32 overflow == 0@

    MOV r9, #0

@//U32 mask == 0x00ff00ff@

@//0x00080008
    MOV r11,#0x00080000
    ORR r11, r11, #0x8

@//const PixelC* pLine  ==  ppxlcRefMB - iWidthPrev@

    .if DYNAMIC_EDGEPAD==1
    SUB r1, r1, r2, LSR #16
    .else
    SUB r1, r1, r2, LSL #1
    .endif

@//for (Int iy  ==  0@ iy < 11@ iy ++ ) 

    MOVS r12, #10

MCMAlignBlock_Loop:




@//t1 == pLine[0]  (pLine[0 + 2] << 16)@

    .if DYNAMIC_EDGEPAD==1
    LDRB r5, [r1, r2, LSR #17 ]!
    .else
    LDRB r5, [r1, r2 ]!
    .endif

    LDRB r14, [r1, #2]
@@relo0   ORR r5, r5, r14, lsl #16

@//t0 == pLine[-1]  (pLine[-1 + 2] << 16)@

    LDRB r4, [r1, #-1]
    ORR r5, r5, r14, lsl #16@@relo0
    LDRB r14, [r1, #1]
@@relo1   ORR r4, r4, r14, lsl #16

@//t2 == pLine[1]  (pLine[1 + 2] << 16)@

    LDRB r6, [r1, #1]
    ORR r4, r4, r14, lsl #16 @@relo1
    LDRB r14, [r1, #3]
@@relo2   ORR r6, r6, r14, lsl #16

@//t3 == pLine[2]  (pLine[2 + 2] << 16)@

    LDRB r7, [r1, #2]
    ORR r6, r6, r14, lsl #16 @@relo2
    LDRB r14, [r1, #4]
@@relo3    ORR r7, r7, r14, lsl #16


@//t4 == pLine[3]  (pLine[3 + 2] << 16)@

    LDRB r8, [r1, #3]
    ORR r7, r7, r14, lsl #16 @@relo3
    LDRB r14, [r1, #5]


 @@relo4   ORR r8, r8, r14, lsl #16

@// u0 == ((t1 + t2)*9-(t0 + t3) + 0x00080008)@

    ADD r4, r4, r7

    ORR r8, r8, r14, lsl #16 @@relo4

@@macro@ CubicFilterShort $u0, $t0, $t1, $t2, $const, $scratch

    CubicFilterShort r4, r4, r5, r6,  r11, r14

@// overflow == u0@ 

    ORR r9, r9, r4
        
@// u0 == u0>>4@
    MOV r4, r4, LSR #4

@// u1 == ((t2 + t3)*9-(t1 + t4) + 0x00080008)@

@@macro@ CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r5, r5, r6, r7, r8, r11, r14

@// overflow == u1@

    ORR r9, r9, r5

@// u1   >>== 4@

    MOV r5, r5, LSR #4

@// y0 == (u0 & mask)  ((u1 & mask) << 8)@

    BIC r4, r4, #0xff00
    BIC r5, r5, #0xff00

@@relo8    STR r4, [ r0 ]
@@relo9   STR r5, [ r0, #44]

@// t5 == pLine[4]  (pLine[4 + 2] << 16)@

    LDRB r7, [r1, #4]
    LDRB r14, [r1, #6]
@@relo5    ORR r7, r7, r14, lsl #16

@// t6 == pLine[5]  (pLine[5 + 2] << 16)@
    LDRB r6, [r1, #5]
    ORR r7, r7, r14, lsl #16 @@relo5
    LDRB r14, [r1, #7]
@@relo6    ORR r6, r6, r14, lsl #16

@// t7 == pLine[6]  (pLine[6 + 2] << 16)@
    LDRB r3, [r1, #6]
    ORR r6, r6, r14, lsl #16 @@relo6
    LDRB r14, [r1, #8]
@@relo7    ORR r3, r3, r14, lsl #16

@// t8 == pLine[7]  (pLine[7 + 2] << 16)@

    LDRB r10, [r1, #7]
    ORR r3, r3, r14, lsl #16 @@relo7
    LDRB r14, [r1, #9]
@    STR r4, [ r0 ], #4      @@relo8
@    STR r5, [ r0, #40]  @@relo9
    ORR r10, r10, r14, lsl #16

@// u2 == ((t5 + t6)*9-(t4 + t7) + 0x00080008)@

@@macro@ CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r8, r8, r7, r6, r3, r11, r14

@// overflow == u2@ 

    ORR r9, r9, r8

@// u2   >>== 4@

    MOV r8, r8, LSR #4

@// u3 == ((t6 + t7)*9-(t5 + t8) + 0x00080008)@
@@macro@ CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r7, r7, r6, r3, r10, r11, r14

@// overflow == u3@

    ORR r9, r9, r7

@// u3   >>== 4@

    MOV r7, r7, LSR #4

@// y1 == (u2 & mask)  ((u3 & mask) << 8)@
    BIC r8, r8, #0xff00
    BIC r7, r7, #0xff00

    STR r4, [ r0 ], #4      @@relo8
    STR r5, [ r0, #40]  @@relo9
    STR r8, [ r0, #84 ]
    STR r7, [ r0, #128 ]


@ // } //for (Int iy  ==  0@ iy < 11@ iy ++ ) 

    SUBS r12, r12, #1
    BGE MCMAlignBlock_Loop

@// if(overflow & 0xf000f000)
    TST r9, #0xf0000000
    TSTEQ r9, #0xf000
    LDMEQFD   sp!, {r4 - r12, PC}

@//  MotionCompMixedAlignBlockComplete(pBlock2, ppxlcRefMB, iWidthPrev)@
        
    .if DYNAMIC_EDGEPAD==1
    SUB r1, r1, r2, LSR #14
    SUB r1, r1, r2, LSR #17
    .else
    SUB r1, r1, r2, LSL #3
    SUB r1, r1, r2 
    .endif

    SUB r0, r0, #44

    BL MotionCompMixedAlignBlockComplete

    LDMFD   sp!, {r4 - r12, PC}
    WMV_ENTRY_END    
    

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@




















@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  
    
    WMV_LEAF_ENTRY MotionCompMixed011

    

@//iWidthPrev  &== 0xffff@

    BIC r2, r2, #0x10000
    

    CMP r3, #0
    
    BNE MCM011_ADDERROR  

@   no add error here

@ref_offset==r3
@ref_offset2==r4
@ppxlcRef==r1
@ppxlcPredMB==r0
@iWidthPrev==r2
@data0==r5
@data1==r6
@data2==r7
@iy==r8@

    

@//U32 ref_offset==(((U32)ppxlcRefMB)&0x3)<<3@
@//ppxlcRef==ppxlcRef-(((U32)ppxlcRef)&0x3)@

    ANDS r3, r1, #3
    BEQ MCM011_NoErrorAlign

    .if DYNAMIC_EDGEPAD==1
    STMFD   sp!, {r4 - r12, r14} @ //r0-r3 are preserved
    .else
    STMFD   sp!, {r4 - r12, r14} @ //r0-r3 are preserved
    .endif
    FRAME_PROFILE_COUNT

    .if DYNAMIC_EDGEPAD==1
    AND r8, r2, #0x3fc
    .endif

    SUB r1, r1, r3
    MOV r3, r3, LSL #3

@//U32 ref_offset2==32-ref_offset@
    RSB r4, r3, #32

@//       for (Int iy  ==  0@ iy < 8@ iy ++ ) 
    MOV r14, #4
    .if DYNAMIC_EDGEPAD==1
    SUB r0, r0, r8
    .else
    SUB r0, r0, r2
    .endif

MCM011_NoErrorLoop:

    LDMIA r1, {r5 - r7}   
     
@//ppxlcRef  +==  iWidthPrev@
    .if DYNAMIC_EDGEPAD==1
    ADD r1, r1, r2, LSR #17
    ADD r0, r0, r8
    .else
    ADD r1, r1, r2
    ADD r0, r0, r2
    .endif

@// *(U32 *)ppxlcPredMB==(data0>>ref_offset)(data1<<ref_offset2)@
    MOV r5, r5, LSR r3
    ORR r5, r5, r6, LSL r4

@// *(U32 *)(ppxlcPredMB+4)==(data1>>ref_offset)(data2<<ref_offset2)@
    MOV r6, r6, LSR r3
    ORR r6, r6, r7, LSL r4

    STMIA r0, { r5, r6 }
@//ppxlcPredMB  +==  iWidthPrev@
    
    LDMIA r1, {r5 - r7}    

@//ppxlcRef  +==  iWidthPrev@
    .if DYNAMIC_EDGEPAD==1
    ADD r1, r1, r2, LSR #17
    ADD r0, r0, r8
    .else
    ADD r1, r1, r2
    ADD r0, r0, r2
    .endif

@// *(U32 *)ppxlcPredMB==(data0>>ref_offset)(data1<<ref_offset2)@
    MOV r5, r5, LSR r3
    ORR r5, r5, r6, LSL r4

@// *(U32 *)(ppxlcPredMB+4)==(data1>>ref_offset)(data2<<ref_offset2)@
    MOV r6, r6, LSR r3
    ORR r6, r6, r7, LSL r4

    STMIA r0, { r5, r6 }
@//ppxlcPredMB  +==  iWidthPrev@

    SUBS r14, r14, #1
    BGT MCM011_NoErrorLoop
    
    .if DYNAMIC_EDGEPAD==1
    LDMFD   sp!, {r4 - r12, PC}
    .else
    LDMFD   sp!, {r4 - r12, PC}
    .endif

MCM011_NoErrorAlign:

    .if DYNAMIC_EDGEPAD==1
    STMFD   sp!, {r4 - r12, r14} @ //r0-r3 are preserved
    .else
    STMFD   sp!, {r4 - r12, r14} @ //r0-r3 are preserved
    .endif
    FRAME_PROFILE_COUNT

    .if DYNAMIC_EDGEPAD==1
    AND r8, r2, #0x3fc
    .endif

    MOV r14, #2

    .if DYNAMIC_EDGEPAD==1
    SUB r1, r1, r2, LSR #17
    SUB r0, r0, r8
    .else
    SUB r1, r1, r2
    SUB r0, r0, r2
    .endif
MCM011_NoErrorAlignLoop:

    .if DYNAMIC_EDGEPAD==1
    LDR r3, [r1 , r2, LSR #17 ]!
    .else
    LDR r3, [r1 , r2 ]!
    .endif

    LDR r4, [r1, #4 ] 

    .if DYNAMIC_EDGEPAD==1
    ADD r0, r0, r8
    .else
    ADD r0, r0, r2
    .endif

    STMIA r0, { r3, r4 }

    .if DYNAMIC_EDGEPAD==1
    LDR r3, [r1 , r2, LSR #17 ]!
    .else
    LDR r3, [r1 , r2 ]!
    .endif
    
    LDR r4, [r1, #4 ]  

    .if DYNAMIC_EDGEPAD==1
    ADD r0, r0, r8
    .else
    ADD r0, r0, r2
    .endif

    STMIA r0, { r3, r4 }
 
    .if DYNAMIC_EDGEPAD==1
    LDR r3, [r1 , r2, LSR #17 ]!
    .else
    LDR r3, [r1 , r2 ]!
    .endif
    
    LDR r4, [r1, #4 ] 

    .if DYNAMIC_EDGEPAD==1
    ADD r0, r0, r8
    .else
    ADD r0, r0, r2
    .endif
    
    STMIA r0, { r3, r4 }

    .if DYNAMIC_EDGEPAD==1
    LDR r3, [r1 , r2, LSR #17 ]!
    .else
    LDR r3, [r1 , r2 ]!
    .endif

    LDR r4, [r1, #4 ]  

    .if DYNAMIC_EDGEPAD==1
    ADD r0, r0, r8
    .else
    ADD r0, r0, r2
    .endif
    
    STMIA r0, { r3, r4 }

    SUBS r14, r14, #1
    BGT MCM011_NoErrorAlignLoop

    .if DYNAMIC_EDGEPAD==1
    LDMFD   sp!, {r4 - r12, PC}
    .else
    LDMFD   sp!, {r4 - r12, PC}
    .endif

MCM011_ADDERROR:

@t0==r4
@t1==r5
@t2==r6
@t3==r7
@err_even==r8
@err_odd==r9
@err_overflow==r10
@ppxlcpredU32==r0
@ppxlcRefMB==r1
@iWidthPrev==r2
@pErrorBuf==r3
@iy==r12
@r14==scratch

    .if DYNAMIC_EDGEPAD==1
    STMFD   sp!, {r4 - r12, r14} @ //r0-r3 are preserved
    .else
    STMFD   sp!, {r4 - r12, r14} @ //r0-r3 are preserved
    .endif
    FRAME_PROFILE_COUNT

@// U32 err_overflow == 0@

    MOV r10, #0

@////for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    MOVS r12, #7

    .if DYNAMIC_EDGEPAD==1
    AND r11, r2, #0x3fc
    .endif

    .if DYNAMIC_EDGEPAD==1
    SUB r1, r1, r2, LSR #17
    SUB r0, r0, r11
    .else
    SUB r1, r1, r2
    SUB r0, r0, r2
    .endif

MCM011_ADDERROR_Loop:


@// t0 == ppxlcRefMB[0]  (ppxlcRefMB[2] << 16)@ 
    .if DYNAMIC_EDGEPAD==1
    LDRB r4, [r1, r2, LSR #17 ]!
    .else
    LDRB r4, [r1, r2]!
    .endif

    LDRB r14, [r1, #2]
@@relo0 ORR r4, r4, r14, lsl #16  

@// t1 == ppxlcRefMB[1]  (ppxlcRefMB[1 + 2] << 16)@
    LDRB r5, [r1, #1]
    ORR r4, r4, r14, lsl #16  @@relo0
    LDRB r14, [r1, #3]
@@relo1 ORR r5, r5, r14, lsl #16

@//t2 == ppxlcRefMB[4]  (ppxlcRefMB[4 + 2] << 16)@
    LDRB r6, [r1, #4]
    ORR r5, r5, r14, lsl #16@@relo1
    LDRB r14, [r1, #6]
@@relo2 ORR r6, r6, r14, lsl #16  

@//t3 == ppxlcRefMB[5]  (ppxlcRefMB[5 + 2] << 16)@
    LDRB r7, [r1, #5]
    ORR r6, r6, r14, lsl #16  @@relo2
    LDRB r14, [r1, #7]
@@relo3 ORR r7, r7, r14, lsl #16 

@// err_even == pErrorBuf[0]@

    LDR r8, [r3], #0x10

@//err_odd  == pErrorBuf[0 + 32]@

    LDR r9, [r3, #0x70]

    ORR r7, r7, r14, lsl #16 @@relo3

@@  AddErrorP0 $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch
    AddErrorP0 r8, r9, r4, r5, r10, r14

@// err_even == pErrorBuf[1]@
    LDR r8, [ r3, #-12 ]
            
@// err_odd  == pErrorBuf[1 + 32]@

    LDR r9, [r3, #0x74]

    ORR r4, r4, r5, LSL #8 @@relo4

@@  AddErrorP0 $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch
    AddErrorP0 r8, r9, r6, r7, r10, r14

@// y0 == (t0)  ((t1) << 8)@
@@relo4    ORR r4, r4, r5, LSL #8
    

@// y1 == (t2)  ((t3) << 8)@
    ORR r6, r6, r7, LSL #8

    
@    STR r4, [ r0, r2]!
@    STR r6, [ r0, #4]
    .if DYNAMIC_EDGEPAD==1
    ADD r0, r0, r11
    .else
    ADD r0, r0, r2
    .endif
    STMIA r0, { r4, r6 }

@//    ppxlcRefMB  +==  iWidthPrev@

@*    ADD r1, r1, r2

@ // } //for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    SUBS r12, r12, #1
    BGE MCM011_ADDERROR_Loop

@//         if(err_overflow & 0xff00ff00)

    TST r10, #0xff000000
    TSTEQ r10, #0xff00

    .if DYNAMIC_EDGEPAD==1
    LDMEQFD   sp!, {r4 - r12, PC}
    .else
    LDMEQFD   sp!, {r4 - r12, PC}
    .endif

@// MotionCompMixed011Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2)@

    .if DYNAMIC_EDGEPAD==1
    SUB r0, r0, r11, LSL #3
    ADD r0, r0, r11
    .else
    SUB r0, r0, r2, LSL #3
    ADD r0, r0, r2
    .endif

    .if DYNAMIC_EDGEPAD==1
    SUB r1, r1, r2, LSR #14
    ADD r1, r1, r2, LSR #17
    .else
    SUB r1, r1, r2, LSL #3
    ADD r1, r1, r2
    .endif

    SUB r3, r3, #0x80

    BL MotionCompMixed011Complete

    .if DYNAMIC_EDGEPAD==1
    LDMFD   sp!, {r4 - r12, PC}
    .else
    LDMFD   sp!, {r4 - r12, PC}
    .endif

    WMV_ENTRY_END
    	
    WMV_LEAF_ENTRY MotionCompWAddError10

@t1==r4
@t2==r5
@t3==r6
@u0==r4
@u1==r5
@err_even==r6
@err_odd==r7
@rndCtrl==r8
@pErrorBuf==r9
@y0==r4
@err_overflow==r10
@iy==r12
@ppxlcPredMB==r0
@ppxlcRefMB==r1
@pErrorBuf2==r3
@iWidthPrev
@r14 scratch


    .if DYNAMIC_EDGEPAD==1
    STMFD   sp!, {r4 - r12, r14} @
    .else
    STMFD   sp!, {r4 - r12, r14} @
    .endif 
    FRAME_PROFILE_COUNT

@// U32 err_overflow == 0@

    MOV r10, #0

@//   U32 rndCtrl == iWidthPrev>>16@
@    MOV r8, r2, LSR #16
    AND r8, r2, #0x10000

@// rndCtrl == rndCtrl << 16@
    ORR r8, r8, r8, LSR #16

@//iWidthPrev  &== 0xffff@

    BIC r2, r2, #0x10000

    .if DYNAMIC_EDGEPAD==1
    AND r11, r2, #0x3fc
    .endif

    MOV r9, r3 

    .if DYNAMIC_EDGEPAD==1
    SUB r1, r1, r2, LSR #17
    .else
    SUB r1, r1, r2
    .endif
    
@//for(Int iz==0@iz<2@iz++)

MCWAE10_Loop0:
    
    
@//for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    MOVS r12, #7



MCWAE10_Loop1:

@// t1 == ppxlcRefMB[0]  (ppxlcRefMB[0 + 2] << 16)@
    .if DYNAMIC_EDGEPAD==1
    LDRB r4, [r1, r2, LSR #17]!
    .else
    LDRB r4, [r1, r2]!
    .endif

    LDRB r14, [r1, #2]
@@relo0 ORR r4, r4, r14, lsl #16

@// t2 == ppxlcRefMB[1]  (ppxlcRefMB[1 + 2] << 16)@
    LDRB r5, [r1, #1]
    ORR r4, r4, r14, lsl #16 @@relo0
    LDRB r14, [r1, #3]
@@relo1 ORR r5, r5, r14, lsl #16

@// t3 == ppxlcRefMB[2]  (ppxlcRefMB[2 + 2] << 16)@
    LDRB r6, [r1, #2]
    ORR r5, r5, r14, lsl #16 @@relo1
    LDRB r14, [r1, #4]
@@relo2 ORR r6, r6, r14, lsl #16

@// u0 == (t1 + t2 + rndCtrl)@
    ADD r4, r4, r5
    ADD r4, r4, r8

@// u0 == u0>>1@
    MOV r4, r4, LSR #1

    ORR r6, r6, r14, lsl #16@@relo2

@// u1 == (t2 + t3 + rndCtrl)@
    ADD r5, r5, r6
    ADD r5, r5, r8

@// u1   >>== 1@
    MOV r5, r5, LSR #1

@// u0  &== mask@

    BIC r4, r4, #0xff00


@// u1  &== mask@

    BIC r5, r5, #0xff00

@ //   if(pErrorBuf2 != NULL)
    CMP r3, #0

    BEQ MCWAE10_L0   



@@@Macro: AndAddError $pErrorBuf, $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch

    AndAddError r9, r6, r7, r4, r5, r10, r14


MCWAE10_L0:
         
@//y0 == (u0)  ((u1) << 8)@

    ORR r4, r4, r5, LSL #8

@//* (U32 *) ppxlcPredMB== y0@
@// ppxlcPredMB   +==  iWidthPrev@

    .if DYNAMIC_EDGEPAD==1
    STR r4, [ r0 ], r11
    .else
    STR r4, [ r0 ], r2 
    .endif
            
@// ppxlcRefMB   +==  iWidthPrev@

@*  ADD r1, r1, r2

@ // } //for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    SUBS r12, r12, #1
    BGE MCWAE10_Loop1


@//     ppxlcRefMB==ppxlcRefMB2+4@

    .if DYNAMIC_EDGEPAD==1
    SUB r1, r1, r2, LSR #14
    .else
    SUB r1, r1, r2, LSL #3
    .endif
    ADD r1, r1, #4

@//     ppxlcPredMB==ppxlcPredMB2+4@

    .if DYNAMIC_EDGEPAD==1
    SUB r0, r0, r11, LSL #3
    .else
    SUB r0, r0, r2, LSL #3
    .endif
    ADD r0, r0, #4

    

    EOR r9, r9, r3
    TST r9, #4

@//     pErrorBuf==pErrorBuf2+1@

    
    ADD r9, r3, #4


    BEQ MCWAE10_Loop0

@//    if((err_overflow & 0xff00ff00) )

    TST r10, #0xff000000
    TSTEQ r10, #0xff00

    .if DYNAMIC_EDGEPAD==1
    LDMEQFD   sp!, {r4 - r12, PC}
    .else
    LDMEQFD   sp!, {r4 - r12, PC}
    .endif
    

@//MotionCompMixed010Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2)@
    
    SUB r0, r0, #8
    SUB r1, r1, #8

    .if DYNAMIC_EDGEPAD==1
    ADD r1, r1, r2, LSR #17
    .else
    ADD r1, r1, r2
    .endif

    ORR r2, r2, r8, LSL #16

    BL MotionCompWAddError10Complete

    .if DYNAMIC_EDGEPAD==1
    LDMFD   sp!, {r4 - r12, PC}
    .else
    LDMFD   sp!, {r4 - r12, PC}
    .endif

    WMV_ENTRY_END    	
    
    WMV_LEAF_ENTRY MotionCompWAddError01

@t_even_1==r4
@t_odd_1==r5
@t_even_2==r6
@t_odd_2==r7
@u0==r4
@u1==r5
@err_even==r8
@err_odd==r9
@rndCtrl==r11
@pErrorBuf==r2
@y0==r4
@err_overflow==r10
@iy==r12
@ppxlcPredMB==r0
@ppxlcRefMB==r1
@pLine==r1
@pErrorBuf2==r3
@r14 scratch
@stack save:
@  iWidthPrev

    STMFD   sp!, {r4 - r12, r14} @ 
    FRAME_PROFILE_COUNT

@// U32 err_overflow == 0@

    MOV r10, #0

@//   U32 rndCtrl == iWidthPrev>>16@
@    MOV r11, r2, LSR #16
    AND r11, r2, #0x10000

@// rndCtrl == rndCtrl << 16@
    ORR r11, r11, r11, LSR #16

@//iWidthPrev  &== 0xffff@

    BIC r2, r2, #0x10000

@//for(Int iz==0@iz<2@iz++)

MCWAE01_Loop0:

@// assume r8==iWidthPrev
    
@//     t_odd_2 == pLine[1]  pLine[3] << 16@
    LDRB r7, [r1, #1]
    LDRB r6, [r1, #3]
@@relo0 ORR r7, r7, r6, lsl #16

@//     t_even_2 == pLine[0]  pLine[2] << 16@

@//pLine   +==  iWidthPrev@
    LDRB r14, [r1, #2]
    ORR r7, r7, r6, lsl #16 @@relo0

    .if DYNAMIC_EDGEPAD==1
    LDRB r6, [r1], r2, LSR #17
    .else
    LDRB r6, [r1], r2
    .endif

    MOVS r12, #7 @@relo1
    ORR r6, r6, r14, lsl #16
    
@//for (Int iy  ==  0@ iy < 8@ iy ++ ) 

@@relo1 MOV r12, #8

MCWAE01_Loop1:

@//  t_even_1 == t_even_2@
    MOV r4, r6

@//  t_odd_1 == t_odd_2@
    MOV r5, r7

@//t_odd_2 == pLine[1]  pLine[3] << 16@
    LDRB r7, [r1, #1]
    LDRB r6, [r1, #3]
@@relo2 ORR r7, r7, r6, lsl #16
    
@//     t_even_2 == pLine[0]  pLine[2] << 16@

@//pLine   +==  iWidthPrev@
    LDRB r14, [r1, #2]
    ORR r7, r7, r6, lsl #16 @@relo2

    .if DYNAMIC_EDGEPAD==1
    LDRB r6, [r1], r2, LSR #17
    .else
    LDRB r6, [r1], r2
    .endif
    
@@relo3 ORR r6, r6, r14, lsl #16

@// u1 == ((t_odd_1 + t_odd_2 + rndCtrl))@
@// u1   >>== 1@
    ADD r5, r5, r7
    ADD r5, r5, r11
    MOV r5, r5, LSR #1

    ORR r6, r6, r14, lsl #16 @@relo3

@// u0 == (t_even_1 + t_even_2 + rndCtrl)@
@// u0 == u0>>1@
    ADD r4, r4, r6
    ADD r4, r4, r11
    MOV r4, r4, LSR #1

@// u0  &== mask@

    BIC r4, r4, #0xff00


@// u1  &== mask@

    BIC r5, r5, #0xff00

@ //   if(pErrorBuf2 != NULL)
    CMP r3, #0
		BLT	AndAddError_Done01
		CMP r3, #4
    BLE MCWAE01_L0   



@@@Macro: AndAddError $pErrorBuf, $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch
AndAddError_Done01:
    AndAddError r3, r8, r9, r4, r5, r10, r14

@ update r8 to iWidthPrev now


MCWAE01_L0:

@//y0 == (u0)  ((u1) << 8)@

    ORR r4, r4, r5, LSL #8

@//* (U32 *) ppxlcPredMB== y0@
@// ppxlcPredMB   +==  iWidthPrev@

    .if DYNAMIC_EDGEPAD==1
    AND r5, r2, #0x3fc @r5==iWidthPrev, r8>>17==iWidthPrefRef
    STR r4, [ r0 ], r5 
    .else
    STR r4, [ r0 ], r2 
    .endif @.if DYNAMIC_EDGEPAD==1

    

@ // } //for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    SUBS r12, r12, #1
    BGE MCWAE01_Loop1       
      
@//     ppxlcRefMB==ppxlcRefMB2+4@

    .if DYNAMIC_EDGEPAD==1
    SUB r1, r1, r2, LSR #14
    SUB r1, r1, r2, LSR #17
    .else
    SUB r1, r1, r2, LSL #3
    SUB r1, r1, r2
    .endif
    ADD r1, r1, #4

@//     ppxlcPredMB==ppxlcPredMB2+4@

    .if DYNAMIC_EDGEPAD==1
    SUB r0, r0, r5, LSL #3
    .else
    SUB r0, r0, r2, LSL #3
    .endif
    ADD r0, r0, #4

		CMP 	r3, #0
    BLT		NEXT_POS_01
    CMP 	r3, #4
    BLE		TEST_NEXT_HALF_01

NEXT_POS_01:
    SUB		r3, r3, #128

TEST_NEXT_HALF_01:
    TST 	r3, #4

@//     pErrorBuf==pErrorBuf2+1@

    
    ADD 	r3, r3, #4


    BEQ MCWAE01_Loop0

@//    if((err_overflow & 0xff00ff00) )

    TST r10, #0xff000000
    TSTEQ r10, #0xff00
    LDMEQFD   sp!, {r4 - r12, PC}
    

@//MotionCompMixed010Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2)@
    
    SUB r0, r0, #8
    SUB r1, r1, #8
    ORR r2, r2, r11, LSL #16
    SUB r3, r3, #8

    BL MotionCompWAddError01Complete

    LDMFD   sp!, {r4 - r12, PC}
    WMV_ENTRY_END

   .endif   @.if WMV_OPT_MOTIONCOMP_ARM == 1 
    
    .END     
    
