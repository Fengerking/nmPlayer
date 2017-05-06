    #include "../c/voWMVDecID.h"
    .include "wmvdec_member_arm.h"
    .include "xplatform_arm_asm.h" 

   .if WMV_OPT_MOTIONCOMP_ARM == 1

	@@AREA	.rdata, DATA, READONLY
	@ .rdata
	.globl	_MotionCompMixed010Complete
    .globl  _MotionCompMixed110Complete
    .globl  _MotionCompMixed001Complete	
    .globl  _MotionCompMixedAlignBlockComplete    
        
	.globl  _MotionCompMixed110
    .globl  _MotionCompMixed010	
    .globl  _MotionCompMixed001 
    .globl  _MotionCompMixedAlignBlock        
  
  
    .globl  _MotionCompMixed011Complete  
    .globl  _MotionCompWAddError10Complete
    .globl  _MotionCompWAddError01Complete
                   
    .globl  _MotionCompMixed011     
    .globl  _MotionCompWAddError10
    .globl  _MotionCompWAddError01  
         
@PRESERVE8
	@@AREA	.text, CODE, READONLY
	 .text
	 .align 4

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@//Void MotionCompMixed010(PixelC*   ppxlcPredMB, const PixelC*  ppxlcRefMB, Int iWidthPrev, I32 * pErrorBuf )
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


    .if ARCH_V3 == 1
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        .macro LOADONE16bitsLo @srcRN, offset, dstRN
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        ldr     $2, [$0, $1]
        mov     $2, $2, lsl #16
        mov     $2, $2, asr #16
        .endmacro
    .endif @ //ARCH_V3

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro AndAddError @pErrorBuf, err_even, err_odd, u0, u1, err_overflow, scratch

@// err_even == pErrorBuf[0]@

    ldr $1, [$0], #0x10

@// err_odd  == pErrorBuf[0 + 32]@

    ldr $2, [$0, #0x70]


@//pErrorBuf += 4@


@//u0 == u0 + err_even-((err_even & 0x8000) << 1)@

    and $6, $1, #0x8000
    sub $6, $1, $6, lsl #1
    add $3, $3, $6

@//err_overflow  |= u0@
    orr $5, $5, $3

@//u1 == u1 + err_odd -((err_odd  & 0x8000) << 1)@

    and $6,$2,#0x8000
    sub $6,$2,$6, lsl #1
    add $4, $4, $6

@//err_overflow  |= u1@
    orr $5, $5, $4

    .endmacro

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  .macro AddErrorP0 @err_even, err_odd, u0, u1, err_overflow, scratch
@//u0 == u0 + err_even-((err_even & 0x8000) << 1)@

   and $5, $0, #0x8000
   sub $5, $0, $5, LSL #1
   add $2, $2, $5

@//err_overflow  == u0@
   orr $4, $4, $2

@//u1 == u1 + err_odd -((err_odd  & 0x8000) << 1)@

   and $5,$1,#0x8000
   sub $5,$1,$5, LSL #1
   add $3, $3, $5

@//err_overflow  == u1@
   orr $4, $4, $3

   .endmacro
   
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro CubicFilter @u0, t0, t1, t2, t3, const, scratch
@   //u0 == ((t1 + t2)*9-(t0 + t3) + 0x00080008)@
    
    add $6, $2, $3  
    add $6, $6, $6, lsl #3
    sub $0, $6, $1
    sub $0, $0, $4
    add $0, $0, $5

    .endmacro
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro CubicFilterShort @u0, t0, t1, t2, const, scratch
@   //u0 == ((t1 + t2)*9-(t0 ) + 0x00080008)@
    
    add $5, $2, $3  
    add $5, $5, $5, lsl #3
    sub $0, $5, $1
    add $0, $0, $4

    .endmacro
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  

    
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


    stmfd   sp!, {r4 - r12, r14} @ //r0-r3 are preserved
    FRAME_PROFILE_COUNT


@// U32 err_overflow == 0@

    mov r10, #0

@// U32 overflow == 0@

    mov r9, #0

@//U32 mask == 0x00ff00ff@

@//0x00080008
    mov r11,#0x00080000
    orr r11, r11, #0x8

@//iWidthPrev  &== 0xffff@

    bic r2, r2, #0x10000

    add r1, r1, #5

@//for(Int iz==0@iz<2@iz++)

MCM010_Loop0:
    
    
@//for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    movs r12, #7

MCM010_Loop1:

@//t0 == ppxlcRefMB[-1]  (ppxlcRefMB[-1 + 2] << 16)@

    ldrb r4, [r1, #-6]

    ldrb r14, [r1, #-4]
@@relo0    orr r4, r4, r14, lsl #16

@// t1 == ppxlcRefMB[0]  (ppxlcRefMB[0 + 2] << 16)@
    ldrb r5, [r1, #-5]
    orr r4, r4, r14, lsl #16 @@ relo0
    ldrb r14, [r1, #-3]
@@relo1 orr r5, r5, r14, lsl #16

@// t2 == ppxlcRefMB[1]  (ppxlcRefMB[1 + 2] << 16)@
    ldrb r6, [r1, #-4]
    orr r5, r5, r14, lsl #16 @@ relo1
    ldrb r14, [r1, #-2]
@@relo2 orr r6, r6, r14, lsl #16

@// t3 == ppxlcRefMB[2]  (ppxlcRefMB[2 + 2] << 16)@
    ldrb r7, [r1, #-3]
    orr r6, r6, r14, lsl #16 @@ relo2
    ldrb r14, [r1, #-1]
@@//relo3   orr r7, r7, r14, lsl #16

@//t4 == ppxlcRefMB[3]  (ppxlcRefMB[3 + 2] << 16)@
@// ppxlcRefMB   +==  iWidthPrev@
    ldrb r8, [r1, #-2]
    orr r7, r7, r14, lsl #16 @@relo3

    .if DYNAMIC_EDGEPAD==1
    ldrb r14, [r1], r2, lsr #17
    .else
    ldrb r14, [r1], r2
    .endif @.if DYNAMIC_EDGEPAD==1
    
@@relo4 orr r8, r8, r14, lsl #16

@//u0 == ((t1 + t2)*9-(t0 + t3) + 0x00080008)@

@@macro@ CubicFilterShort $u0, $t0, $t1, $t2,  $const, $scratch

    add r4, r4, r7

    orr r8, r8, r14, lsl #16@@ relo4

    CubicFilterShort r4, r4, r5, r6, r11, r14

@//overflow  == u0@ 
    orr r9, r9, r4

@// u0 == u0>>4@
    mov r4, r4, lsr #4

@//u1 == ((t2 + t3)*9-(t1 + t4) + 0x00080008)@
@@macro@ CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r5, r5, r6, r7, r8, r11, r14
    
@//overflow  == u1@ 
    orr r9, r9, r5

@// u1 == u1>>4@
    mov r5, r5, lsr #4

@// u0  &== mask@

    bic r4, r4, #0xff00


@// u1  &== mask@

    bic r5, r5, #0xff00

@ //   if(pErrorBuf2 != NULL)

    cmp r3, #4
 
    ble MCM010_L0


@@@Macro: AndAddError $pErrorBuf, $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch

    AndAddError r3, r6, r7, r4, r5, r10, r14



MCM010_L0:
         

@//y0 == (u0)  ((u1) << 8)@

    orr r4, r4, r5, lsl #8

@//* (U32 *) ppxlcPredMB== y0@
@// ppxlcPredMB   +==  iWidthPrev@

@@relo5 ldr r8, [sp]

@@relo6 str r4, [ r0 ], r8 

    .if DYNAMIC_EDGEPAD==1
    and r5, r2, #0x3fc @r5==iWidthPrev, r8>>17==iWidthPrefRef
    .endif @.if DYNAMIC_EDGEPAD==1

@ // } //for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    subs r12, r12, #1

    .if DYNAMIC_EDGEPAD==1
    str r4, [ r0 ], r5 @@relo6
    .else
    str r4, [ r0 ], r2 @@relo6
    .endif

    bge MCM010_Loop1



@//     ppxlcRefMB==ppxlcRefMB2+4@

    .if DYNAMIC_EDGEPAD==1
    sub r1, r1, r2, lsr #14
    .else
    sub r1, r1, r2, lsl #3
    .endif

    add r1, r1, #4

@//     ppxlcPredMB==ppxlcPredMB2+4@

    .if DYNAMIC_EDGEPAD==1
    sub r0, r0, r5, lsl #3
    .else
    sub r0, r0, r2, lsl #3
    .endif
    add r0, r0, #4

    cmp r3, #4
    subgt r3, r3, #128

    tst r3, #4

@//     pErrorBuf==pErrorBuf2+1@

    add r3, r3, #4


    beq MCM010_Loop0


@//   } //for(Int iz==0@iz<2@iz++)


@//    if((err_overflow & 0xff00ff00)  (overflow & 0xf000f000))

    tst r10, #0xff000000
    tsteq r10, #0xff00
    tsteq r9, #0xf0000000
    tsteq r9, #0xf000
    ldmeqfd   sp!, {r4 - r12, PC}
    

@//MotionCompMixed010Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2)@
    
    sub r0, r0, #8
    sub r1, r1, #13
    sub r3, r3, #8

    bl _MotionCompMixed010Complete

    ldmfd   sp!, {r4 - r12, PC}
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


    stmfd   sp!, {r4 - r12, r14} @ //r0-r3 are preserved
    FRAME_PROFILE_COUNT


@// U32 err_overflow == 0@

    mov r10, #0

@// U32 overflow == 0@

    mov r9, #0

@//U32 mask == 0x00ff00ff@

@//0x00080008
    mov r11,#0x00080000
    orr r11, r11, #0x8

@//iWidthPrev  &== 0xffff@

    bic r2, r2, #0x10000

    add r1, r1, #5

@//for(Int iz==0@iz<2@iz++)

MCM110_Loop0:
    
    
@//for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    movs r12, #7

MCM110_Loop1:

@//t0 == ppxlcRefMB[-1]  (ppxlcRefMB[-1 + 2] << 16)@

    ldrb r4, [r1, #-6]

    ldrb r14, [r1, #-4]
@@relo0 orr r4, r4, r14, lsl #16

@// t1 == ppxlcRefMB[0]  (ppxlcRefMB[0 + 2] << 16)@
    ldrb r5, [r1, #-5]
    orr r4, r4, r14, lsl #16 @@relo0
    ldrb r14, [r1, #-3]
@@relo1 orr r5, r5, r14, lsl #16

@// t2 == ppxlcRefMB[1]  (ppxlcRefMB[1 + 2] << 16)@
    ldrb r6, [r1, #-4]
    orr r5, r5, r14, lsl #16 @@relo1
    ldrb r14, [r1, #-2]
@@relo2 orr r6, r6, r14, lsl #16

@// t3 == ppxlcRefMB[2]  (ppxlcRefMB[2 + 2] << 16)@
    ldrb r7, [r1, #-3]
    orr r6, r6, r14, lsl #16 @@relo2
    ldrb r14, [r1, #-1]
@@relo3 orr r7, r7, r14, lsl #16

@//t4 == ppxlcRefMB[3]  (ppxlcRefMB[3 + 2] << 16)@
@// ppxlcRefMB   +==  iWidthPrev@
    ldrb r8, [r1, #-2]
    orr r7, r7, r14, lsl #16 @@relo3
    .if DYNAMIC_EDGEPAD==1
    ldrb r14, [r1], r2, lsr #17
    .else
    ldrb r14, [r1], r2
    .endif
@@relo4 orr r8, r8, r14, lsl #16

@//u0 == ((t1 + t2)*9-(t0 + t3) + 0x00080008)@

    add r4, r4, r7

    orr r8, r8, r14, lsl #16 @@relo4

@@macro@ CubicFilterShort $u0, $t0, $t1, $t2,  $const, $scratch

    CubicFilterShort r4, r4, r5, r6, r11, r14

@//overflow  == u0@ 
    orr r9, r9, r4

@// u0 == u0>>4@
@//    u0 == (u0 + t2 + 0x00010001)>>1@
@   mov r4, r4, lsr #4
    add r4, r4, r6, lsl #4
    add r4, r4, r11, lsl #1
    mov r4, r4, lsr #5

@//u1 == ((t2 + t3)*9-(t1 + t4) + 0x00080008)@
@@macro@ CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r5, r5, r6, r7, r8, r11, r14

@//overflow  == u1@ 
    orr r9, r9, r5

@// u1 == u1>>4@
@//    u1 == (u1 + t3 + 0x00010001)>>1@
@   mov r5, r5, lsr #4
    add r5, r5, r7, lsl #4
    add r5, r5, r11, lsl #1
    mov r5, r5, lsr #5



@// u0  &== mask@

    bic r4, r4, #0xff00


@// u1  &== mask@

    bic r5, r5, #0xff00

@ //   if(pErrorBuf2 != NULL)
    cmp r3, #4

    ble MCM110_L0    



@@@Macro: AndAddError $pErrorBuf, $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch

    AndAddError r3, r6, r7, r4, r5, r10, r14



MCM110_L0:
         

@//y0 == (u0)  ((u1) << 8)@

    orr r4, r4, r5, lsl #8

@//* (U32 *) ppxlcPredMB== y0@
@// ppxlcPredMB   +==  iWidthPrev@

@@relo5 ldr r8, [sp]

@@relo6 str r4, [ r0 ], r8 

    .if DYNAMIC_EDGEPAD==1
    and r5, r2, #0x3fc @r5==iWidthPrev, r8>>17==iWidthPrefRef
    .endif @.if DYNAMIC_EDGEPAD==1

@ // } //for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    subs r12, r12, #1

    .if DYNAMIC_EDGEPAD==1
    str r4, [ r0 ], r5 @@relo6
    .else
    str r4, [ r0 ], r2 @@relo6
    .endif
    
    bge MCM110_Loop1



@//     ppxlcRefMB==ppxlcRefMB2+4@

    .if DYNAMIC_EDGEPAD==1
    sub r1, r1, r2, lsr #14
    .else
    sub r1, r1, r2, lsl #3
    .endif
    
    add r1, r1, #4

@//     ppxlcPredMB==ppxlcPredMB2+4@

    .if DYNAMIC_EDGEPAD==1
    sub r0, r0, r5, lsl #3
    .else
    sub r0, r0, r2, lsl #3
    .endif

    add r0, r0, #4

    cmp r3, #4
    subgt r3, r3, #128

    tst r3, #4

@//     pErrorBuf==pErrorBuf2+1@

    
    add r3, r3, #4


    beq MCM110_Loop0


@//   } //for(Int iz==0@iz<2@iz++)


@//    if((err_overflow & 0xff00ff00)  (overflow & 0xf000f000))

    tst r10, #0xff000000
    tsteq r10, #0xff00
    tsteq r9, #0xf0000000
    tsteq r9, #0xf000
    ldmeqfd   sp!, {r4 - r12, PC}
    

@//MotionCompMixed110Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2)@
    
    sub r0, r0, #8
    sub r1, r1, #13
    sub r3, r3, #8
    

    bl _MotionCompMixed110Complete

    ldmfd   sp!, {r4 - r12, PC}
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

    stmfd   sp!, {r4 - r12, r14} @ //r0-r3 are preserved
    FRAME_PROFILE_COUNT

    @// U32 err_overflow == 0@

    mov r10, #0

@// U32 overflow == 0@

    mov r9, #0

@//U32 mask == 0x00ff00ff@

@//0x00080008
    mov r11,#0x00080000
    orr r11, r11, #0x8

@//iWidthPrev  &== 0xffff@

    bic r2, r2, #0x10000

    sub sp, sp, #24 @ make room for t_even/odd_2,3,4

@//const PixelC* pLine  ==  ppxlcRefMB + 2*iWidthPrev+1@

    .if DYNAMIC_EDGEPAD==1
    add r1, r1, r2, lsr #16
    .else
    add r1, r1, r2, lsl #1
    .endif
    add r1, r1, #1

@//for(Int iz==0@iz<2@iz++)

MCM001_Loop0:

@ initially pLine==r1==ppxlcRefMB2+2*iWidthPrev+1, so odd first

@//t_odd_4 == pLine[1]  pLine[3] << 16@
@//pLine   -==  iWidthPrev@

    .if DYNAMIC_EDGEPAD==1
    ldrb r8, [r1, -r2, lsr #17 ]!   
    .else
    ldrb r8, [r1, -r2 ]!
    .endif   
    ldrb r14, [r1, #2]  
@@relo0 orr r8, r8, r14, lsl #16

@//t_odd_3 == pLine[1]  pLine[3] << 16@
@//pLine   -==  iWidthPrev@

    .if DYNAMIC_EDGEPAD==1
    ldrb r7, [r1, -r2, lsr #17 ]!       
    .else
    ldrb r7, [r1, -r2 ]!    
    .endif
    orr r8, r8, r14, lsl #16 @@ relo0
    ldrb r14, [r1, #2]  
@@relo1 orr r7, r7, r14, lsl #16
        
@//t_odd_2 == pLine[1]  pLine[3] << 16@
@//pLine   -==  iWidthPrev@

    .if DYNAMIC_EDGEPAD==1
    ldrb r6, [r1, -r2, lsr #17]!
    .else
    ldrb r6, [r1, -r2 ]!
    .endif       
    orr r7, r7, r14, lsl #16 @@relo1
    ldrb r14, [r1, #2]  

    sub r1, r1, #1 @@relo2

    orr r6, r6, r14, lsl #16

@ save t_odd_2,3,4 now

    stmia sp, { r6 - r8 }

@ now deal with the t_even_2,3,4

@@relo2 sub r1, r1, #1

@//t_even_2 == pLine[0]  pLine[2] << 16@
@//pLine   +==  iWidthPrev@

    ldrb r6, [r1, #2]
    .if DYNAMIC_EDGEPAD==1
    ldrb r5, [r1], r2, lsr #17
    .else
    ldrb r5, [r1], r2
    .endif
@@relo3 orr r5, r5, r6, lsl #16

@//t_even_3 == pLine[0]  pLine[2] << 16@
@//pLine   +==  iWidthPrev@

        
    ldrb r7, [r1, #2]
    orr r5, r5, r6, lsl #16 @@relo3
    .if DYNAMIC_EDGEPAD==1
    ldrb r6, [r1], r2, lsr #17
    .else
    ldrb r6, [r1], r2
    .endif
@@relo4 orr r6, r6, r7, lsl #16
    

@//t_even_4 == pLine[0]  pLine[2] << 16@
@//pLine   +==  iWidthPrev@

        
    ldrb r14, [r1, #2]
    orr r6, r6, r7, lsl #16 @@relo4
    .if DYNAMIC_EDGEPAD==1
    ldrb r7, [r1], r2, lsr #17
    .else
    ldrb r7, [r1], r2
    .endif
    add r8, sp, #12 @@relo5
    orr r7, r7, r14, lsl #16

@@relo5 add r8, sp, #12
    stmia r8, { r5 - r7 }

@//for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    movs r12, #7

MCM001_Loop1:

@ r1 starts offset by 0, so load even regs first, r8== sp+12

@@relo6 ldmia r8, { r4 - r6 }

@//t_even_4 == pLine[0]  pLine[2] << 16@

    ldrb r7, [r1]

    ldrb r14, [r1, #2]
    ldmia r8, { r4 - r6 } @@relo6

    orr r7, r7, r14, lsl #16

@// u0 == ((t_even_2 + t_even_3)*9-(t_even_1 + t_even_4) + 0x00080008)@

@@macro@ CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r4, r4, r5, r6, r7, r11, r14

@ now save t_even_2,3,4

    stmia r8, { r5 - r7 }

    ldrb r8, [r1, #1] @@relo7
    ldrb r14, [r1, #3] @@relo8

@//overflow  == u0@ 
    orr r9, r9, r4

@// u0 == u0>>4@
    mov r4, r4, lsr #4

@// u0  &== mask@

    bic r4, r4, #0xff00

@ now load t_odd_2,3,4

    ldmia sp, { r5 - r7 }
    
@//t_odd_4 == pLine[1]  pLine[3] << 16@

@@relo7 ldrb r8, [r1, #1]
@@relo8 ldrb r14, [r1, #3]
    orr r8, r8, r14, lsl #16
    
@//u1 == ((t_odd_2 + t_odd_3)*9-(t_odd_1 + t_odd_4) + 0x00080008)@

@@macro@ CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r5, r5, r6, r7, r8, r11, r14

@ now save t_odd_2,3,4

    stmia sp, { r6 - r8 }

@//overflow  == u1@ 
    orr r9, r9, r5

@// u1 == u1>>4@
    mov r5, r5, lsr #4


@// u1  &== mask@

    bic r5, r5, #0xff00

@ //   if(pErrorBuf2 != NULL)

    cmp r3, #4

    ble MCM001_L0    



@@@Macro: AndAddError $pErrorBuf, $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch

    AndAddError r3, r6, r7, r4, r5, r10, r14

MCM001_L0:
         

@//y0 == (u0)  ((u1) << 8)@

    orr r5, r4, r5, lsl #8

@//* (U32 *) ppxlcPredMB== y0@
@// ppxlcPredMB   +==  iWidthPrev@

@@relo9 str r5, [ r0 ], r4 

            
@// ppxlcRefMB   +==  iWidthPrev@

@@relo10    add r1, r1, r4 @ r1 still even

    

@ // } //for (Int iy  ==  0@ iy < 8@ iy ++ ) 

@  prepare r8 for the loop start
    add r8, sp, #12

    .if DYNAMIC_EDGEPAD==1
    and r7, r2, #0x3fc @r7==iWidthPrev, r4>>17==iWidthPrefRef
    add r1, r1, r2, lsr #17  @@relo10 @r1 still even
    .else
@// ppxlcRefMB   +==  iWidthPrev@
    add r1, r1, r2 @@relo10 @r1 still even
    .endif @.if DYNAMIC_EDGEPAD==1

    .if DYNAMIC_EDGEPAD==1
    str r5, [ r0 ], r7 @@relo9
    .else
    str r5, [ r0 ], r2 @@relo9
    .endif

    subs r12, r12, #1
    bge MCM001_Loop1

@//     ppxlcRefMB==ppxlcRefMB2+4@

    .if DYNAMIC_EDGEPAD==1
    sub r1, r1, r2, lsr #14  @ this brings r1 back to ppxlcRefMB2+2*iWidthPrev
    .else
    sub r1, r1, r2, lsl #3  @ this brings r1 back to ppxlcRefMB2+2*iWidthPrev
    .endif
    add r1, r1, #5  @ 4+1 make sure r1 is odd now

@//     ppxlcPredMB==ppxlcPredMB2+4@

    .if DYNAMIC_EDGEPAD==1
    sub r0, r0, r7, lsl #3
    .else
    sub r0, r0, r2, lsl #3
    .endif
    add r0, r0, #4

    cmp r3, #4
    subgt r3, r3, #128


    tst r3, #4

@//     pErrorBuf==pErrorBuf2+1@

    
    add r3, r3, #4


    beq MCM001_Loop0

@//   } //for(Int iz==0@iz<2@iz++)

    add sp, sp, #24 @ adjust the stack now

@//    if((err_overflow & 0xff00ff00)  (overflow & 0xf000f000))

    tst r10, #0xff000000
    tsteq r10, #0xff00
    tsteq r9, #0xf0000000
    tsteq r9, #0xf000
    ldmeqfd   sp!, {r4 - r12, PC}
    

@//MotionCompMixed001Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2)@
    
    sub r0, r0, #8
    sub r1, r1, #9
    .if DYNAMIC_EDGEPAD==1
    sub r1, r1, r2, lsr #16
    .else
    sub r1, r1, r2, lsl #1
    .endif
    sub r3, r3, #8

    bl _MotionCompMixed001Complete

    ldmfd   sp!, {r4 - r12, PC}
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



    stmfd   sp!, {r4 - r12, r14} @ //r0-r3 are preserved
    FRAME_PROFILE_COUNT

@// U32 overflow == 0@

    mov r9, #0

@//U32 mask == 0x00ff00ff@

@//0x00080008
    mov r11,#0x00080000
    orr r11, r11, #0x8

@//const PixelC* pLine  ==  ppxlcRefMB - iWidthPrev@

    .if DYNAMIC_EDGEPAD==1
    sub r1, r1, r2, lsr #16
    .else
    sub r1, r1, r2, lsl #1
    .endif

@//for (Int iy  ==  0@ iy < 11@ iy ++ ) 

    movs r12, #10

MCMAlignBlock_Loop:




@//t1 == pLine[0]  (pLine[0 + 2] << 16)@

    .if DYNAMIC_EDGEPAD==1
    ldrb r5, [r1, r2, lsr #17 ]!
    .else
    ldrb r5, [r1, r2 ]!
    .endif

    ldrb r14, [r1, #2]
@@relo0   orr r5, r5, r14, lsl #16

@//t0 == pLine[-1]  (pLine[-1 + 2] << 16)@

    ldrb r4, [r1, #-1]
    orr r5, r5, r14, lsl #16@@relo0
    ldrb r14, [r1, #1]
@@relo1   orr r4, r4, r14, lsl #16

@//t2 == pLine[1]  (pLine[1 + 2] << 16)@

    ldrb r6, [r1, #1]
    orr r4, r4, r14, lsl #16 @@relo1
    ldrb r14, [r1, #3]
@@relo2   orr r6, r6, r14, lsl #16

@//t3 == pLine[2]  (pLine[2 + 2] << 16)@

    ldrb r7, [r1, #2]
    orr r6, r6, r14, lsl #16 @@relo2
    ldrb r14, [r1, #4]
@@relo3    orr r7, r7, r14, lsl #16


@//t4 == pLine[3]  (pLine[3 + 2] << 16)@

    ldrb r8, [r1, #3]
    orr r7, r7, r14, lsl #16 @@relo3
    ldrb r14, [r1, #5]


 @@relo4   orr r8, r8, r14, lsl #16

@// u0 == ((t1 + t2)*9-(t0 + t3) + 0x00080008)@

    add r4, r4, r7

    orr r8, r8, r14, lsl #16 @@relo4

@@macro@ CubicFilterShort $u0, $t0, $t1, $t2, $const, $scratch

    CubicFilterShort r4, r4, r5, r6,  r11, r14

@// overflow == u0@ 

    orr r9, r9, r4
        
@// u0 == u0>>4@
    mov r4, r4, lsr #4

@// u1 == ((t2 + t3)*9-(t1 + t4) + 0x00080008)@

@@macro@ CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r5, r5, r6, r7, r8, r11, r14

@// overflow == u1@

    orr r9, r9, r5

@// u1   >>== 4@

    mov r5, r5, lsr #4

@// y0 == (u0 & mask)  ((u1 & mask) << 8)@

    bic r4, r4, #0xff00
    bic r5, r5, #0xff00

@@relo8    str r4, [ r0 ]
@@relo9   str r5, [ r0, #44]

@// t5 == pLine[4]  (pLine[4 + 2] << 16)@

    ldrb r7, [r1, #4]
    ldrb r14, [r1, #6]
@@relo5    orr r7, r7, r14, lsl #16

@// t6 == pLine[5]  (pLine[5 + 2] << 16)@
    ldrb r6, [r1, #5]
    orr r7, r7, r14, lsl #16 @@relo5
    ldrb r14, [r1, #7]
@@relo6    orr r6, r6, r14, lsl #16

@// t7 == pLine[6]  (pLine[6 + 2] << 16)@
    ldrb r3, [r1, #6]
    orr r6, r6, r14, lsl #16 @@relo6
    ldrb r14, [r1, #8]
@@relo7    orr r3, r3, r14, lsl #16

@// t8 == pLine[7]  (pLine[7 + 2] << 16)@

    ldrb r10, [r1, #7]
    orr r3, r3, r14, lsl #16 @@relo7
    ldrb r14, [r1, #9]
@    str r4, [ r0 ], #4      @@relo8
@    str r5, [ r0, #40]  @@relo9
    orr r10, r10, r14, lsl #16

@// u2 == ((t5 + t6)*9-(t4 + t7) + 0x00080008)@

@@macro@ CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r8, r8, r7, r6, r3, r11, r14

@// overflow == u2@ 

    orr r9, r9, r8

@// u2   >>== 4@

    mov r8, r8, lsr #4

@// u3 == ((t6 + t7)*9-(t5 + t8) + 0x00080008)@
@@macro@ CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r7, r7, r6, r3, r10, r11, r14

@// overflow == u3@

    orr r9, r9, r7

@// u3   >>== 4@

    mov r7, r7, lsr #4

@// y1 == (u2 & mask)  ((u3 & mask) << 8)@
    bic r8, r8, #0xff00
    bic r7, r7, #0xff00

    str r4, [ r0 ], #4      @@relo8
    str r5, [ r0, #40]  @@relo9
    str r8, [ r0, #84 ]
    str r7, [ r0, #128 ]


@ // } //for (Int iy  ==  0@ iy < 11@ iy ++ ) 

    subs r12, r12, #1
    bge MCMAlignBlock_Loop

@// if(overflow & 0xf000f000)
    tst r9, #0xf0000000
    tsteq r9, #0xf000
    ldmeqfd   sp!, {r4 - r12, PC}

@//  MotionCompMixedAlignBlockComplete(pBlock2, ppxlcRefMB, iWidthPrev)@
        
    .if DYNAMIC_EDGEPAD==1
    sub r1, r1, r2, lsr #14
    sub r1, r1, r2, lsr #17
    .else
    sub r1, r1, r2, lsl #3
    sub r1, r1, r2 
    .endif

    sub r0, r0, #44

    bl _MotionCompMixedAlignBlockComplete

    ldmfd   sp!, {r4 - r12, PC}
    WMV_ENTRY_END    
    

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@




















@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  
    
    WMV_LEAF_ENTRY MotionCompMixed011

    

@//iWidthPrev  &== 0xffff@

    bic r2, r2, #0x10000
    

    cmp r3, #0
    
    bne MCM011_ADDERROR  

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

    ands r3, r1, #3
    beq MCM011_NoErrorAlign

    .if DYNAMIC_EDGEPAD==1
    stmfd   sp!, {r4 - r12, r14} @ //r0-r3 are preserved
    .else
    stmfd   sp!, {r4 - r12, r14} @ //r0-r3 are preserved
    .endif
    FRAME_PROFILE_COUNT

    .if DYNAMIC_EDGEPAD==1
    and r8, r2, #0x3fc
    .endif

    sub r1, r1, r3
    mov r3, r3, lsl #3

@//U32 ref_offset2==32-ref_offset@
    rsb r4, r3, #32

@//       for (Int iy  ==  0@ iy < 8@ iy ++ ) 
    mov r14, #4
    .if DYNAMIC_EDGEPAD==1
    sub r0, r0, r8
    .else
    sub r0, r0, r2
    .endif

MCM011_NoErrorLoop:

    ldmia r1, {r5 - r7}   
     
@//ppxlcRef  +==  iWidthPrev@
    .if DYNAMIC_EDGEPAD==1
    add r1, r1, r2, lsr #17
    add r0, r0, r8
    .else
    add r1, r1, r2
    add r0, r0, r2
    .endif

@// *(U32 *)ppxlcPredMB==(data0>>ref_offset)(data1<<ref_offset2)@
    mov r5, r5, lsr r3
    orr r5, r5, r6, lsl r4

@// *(U32 *)(ppxlcPredMB+4)==(data1>>ref_offset)(data2<<ref_offset2)@
    mov r6, r6, lsr r3
    orr r6, r6, r7, lsl r4

    stmia r0, { r5, r6 }
@//ppxlcPredMB  +==  iWidthPrev@
    
    ldmia r1, {r5 - r7}    

@//ppxlcRef  +==  iWidthPrev@
    .if DYNAMIC_EDGEPAD==1
    add r1, r1, r2, lsr #17
    add r0, r0, r8
    .else
    add r1, r1, r2
    add r0, r0, r2
    .endif

@// *(U32 *)ppxlcPredMB==(data0>>ref_offset)(data1<<ref_offset2)@
    mov r5, r5, lsr r3
    orr r5, r5, r6, lsl r4

@// *(U32 *)(ppxlcPredMB+4)==(data1>>ref_offset)(data2<<ref_offset2)@
    mov r6, r6, lsr r3
    orr r6, r6, r7, lsl r4

    stmia r0, { r5, r6 }
@//ppxlcPredMB  +==  iWidthPrev@

    subs r14, r14, #1
    bgt MCM011_NoErrorLoop
    
    .if DYNAMIC_EDGEPAD==1
    ldmfd   sp!, {r4 - r12, PC}
    .else
    ldmfd   sp!, {r4 - r12, PC}
    .endif

MCM011_NoErrorAlign:

    .if DYNAMIC_EDGEPAD==1
    stmfd   sp!, {r4 - r12, r14} @ //r0-r3 are preserved
    .else
    stmfd   sp!, {r4 - r12, r14} @ //r0-r3 are preserved
    .endif
    FRAME_PROFILE_COUNT

    .if DYNAMIC_EDGEPAD==1
    and r8, r2, #0x3fc
    .endif

    mov r14, #2

    .if DYNAMIC_EDGEPAD==1
    sub r1, r1, r2, lsr #17
    sub r0, r0, r8
    .else
    sub r1, r1, r2
    sub r0, r0, r2
    .endif
MCM011_NoErrorAlignLoop:

    .if DYNAMIC_EDGEPAD==1
    ldr r3, [r1 , r2, lsr #17 ]!
    .else
    ldr r3, [r1 , r2 ]!
    .endif

    ldr r4, [r1, #4 ] 

    .if DYNAMIC_EDGEPAD==1
    add r0, r0, r8
    .else
    add r0, r0, r2
    .endif

    stmia r0, { r3, r4 }

    .if DYNAMIC_EDGEPAD==1
    ldr r3, [r1 , r2, lsr #17 ]!
    .else
    ldr r3, [r1 , r2 ]!
    .endif
    
    ldr r4, [r1, #4 ]  

    .if DYNAMIC_EDGEPAD==1
    add r0, r0, r8
    .else
    add r0, r0, r2
    .endif

    stmia r0, { r3, r4 }
 
    .if DYNAMIC_EDGEPAD==1
    ldr r3, [r1 , r2, lsr #17 ]!
    .else
    ldr r3, [r1 , r2 ]!
    .endif
    
    ldr r4, [r1, #4 ] 

    .if DYNAMIC_EDGEPAD==1
    add r0, r0, r8
    .else
    add r0, r0, r2
    .endif
    
    stmia r0, { r3, r4 }

    .if DYNAMIC_EDGEPAD==1
    ldr r3, [r1 , r2, lsr #17 ]!
    .else
    ldr r3, [r1 , r2 ]!
    .endif

    ldr r4, [r1, #4 ]  

    .if DYNAMIC_EDGEPAD==1
    add r0, r0, r8
    .else
    add r0, r0, r2
    .endif
    
    stmia r0, { r3, r4 }

    subs r14, r14, #1
    bgt MCM011_NoErrorAlignLoop

    .if DYNAMIC_EDGEPAD==1
    ldmfd   sp!, {r4 - r12, PC}
    .else
    ldmfd   sp!, {r4 - r12, PC}
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
    stmfd   sp!, {r4 - r12, r14} @ //r0-r3 are preserved
    .else
    stmfd   sp!, {r4 - r12, r14} @ //r0-r3 are preserved
    .endif
    FRAME_PROFILE_COUNT

@// U32 err_overflow == 0@

    mov r10, #0

@////for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    movs r12, #7

    .if DYNAMIC_EDGEPAD==1
    and r11, r2, #0x3fc
    .endif

    .if DYNAMIC_EDGEPAD==1
    sub r1, r1, r2, lsr #17
    sub r0, r0, r11
    .else
    sub r1, r1, r2
    sub r0, r0, r2
    .endif

MCM011_ADDERROR_Loop:


@// t0 == ppxlcRefMB[0]  (ppxlcRefMB[2] << 16)@ 
    .if DYNAMIC_EDGEPAD==1
    ldrb r4, [r1, r2, lsr #17 ]!
    .else
    ldrb r4, [r1, r2]!
    .endif

    ldrb r14, [r1, #2]
@@relo0 orr r4, r4, r14, lsl #16  

@// t1 == ppxlcRefMB[1]  (ppxlcRefMB[1 + 2] << 16)@
    ldrb r5, [r1, #1]
    orr r4, r4, r14, lsl #16  @@relo0
    ldrb r14, [r1, #3]
@@relo1 orr r5, r5, r14, lsl #16

@//t2 == ppxlcRefMB[4]  (ppxlcRefMB[4 + 2] << 16)@
    ldrb r6, [r1, #4]
    orr r5, r5, r14, lsl #16@@relo1
    ldrb r14, [r1, #6]
@@relo2 orr r6, r6, r14, lsl #16  

@//t3 == ppxlcRefMB[5]  (ppxlcRefMB[5 + 2] << 16)@
    ldrb r7, [r1, #5]
    orr r6, r6, r14, lsl #16  @@relo2
    ldrb r14, [r1, #7]
@@relo3 orr r7, r7, r14, lsl #16 

@// err_even == pErrorBuf[0]@

    ldr r8, [r3], #0x10

@//err_odd  == pErrorBuf[0 + 32]@

    ldr r9, [r3, #0x70]

    orr r7, r7, r14, lsl #16 @@relo3

@@  AddErrorP0 $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch
    AddErrorP0 r8, r9, r4, r5, r10, r14

@// err_even == pErrorBuf[1]@
    ldr r8, [ r3, #-12 ]
            
@// err_odd  == pErrorBuf[1 + 32]@

    ldr r9, [r3, #0x74]

    orr r4, r4, r5, lsl #8 @@relo4

@@  AddErrorP0 $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch
    AddErrorP0 r8, r9, r6, r7, r10, r14

@// y0 == (t0)  ((t1) << 8)@
@@relo4    orr r4, r4, r5, lsl #8
    

@// y1 == (t2)  ((t3) << 8)@
    orr r6, r6, r7, lsl #8

    
@    str r4, [ r0, r2]!
@    str r6, [ r0, #4]
    .if DYNAMIC_EDGEPAD==1
    add r0, r0, r11
    .else
    add r0, r0, r2
    .endif
    stmia r0, { r4, r6 }

@//    ppxlcRefMB  +==  iWidthPrev@

@*    add r1, r1, r2

@ // } //for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    subs r12, r12, #1
    bge MCM011_ADDERROR_Loop

@//         if(err_overflow & 0xff00ff00)

    tst r10, #0xff000000
    tsteq r10, #0xff00

    .if DYNAMIC_EDGEPAD==1
    ldmeqfd   sp!, {r4 - r12, PC}
    .else
    ldmeqfd   sp!, {r4 - r12, PC}
    .endif

@// MotionCompMixed011Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2)@

    .if DYNAMIC_EDGEPAD==1
    sub r0, r0, r11, lsl #3
    add r0, r0, r11
    .else
    sub r0, r0, r2, lsl #3
    add r0, r0, r2
    .endif

    .if DYNAMIC_EDGEPAD==1
    sub r1, r1, r2, lsr #14
    add r1, r1, r2, lsr #17
    .else
    sub r1, r1, r2, lsl #3
    add r1, r1, r2
    .endif

    sub r3, r3, #0x80

    bl _MotionCompMixed011Complete

    .if DYNAMIC_EDGEPAD==1
    ldmfd   sp!, {r4 - r12, PC}
    .else
    ldmfd   sp!, {r4 - r12, PC}
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
    stmfd   sp!, {r4 - r12, r14} @
    .else
    stmfd   sp!, {r4 - r12, r14} @
    .endif 
    FRAME_PROFILE_COUNT

@// U32 err_overflow == 0@

    mov r10, #0

@//   U32 rndCtrl == iWidthPrev>>16@
@    mov r8, r2, lsr #16
    and r8, r2, #0x10000

@// rndCtrl == rndCtrl << 16@
    orr r8, r8, r8, lsr #16

@//iWidthPrev  &== 0xffff@

    bic r2, r2, #0x10000

    .if DYNAMIC_EDGEPAD==1
    and r11, r2, #0x3fc
    .endif

    mov r9, r3 

    .if DYNAMIC_EDGEPAD==1
    sub r1, r1, r2, lsr #17
    .else
    sub r1, r1, r2
    .endif
    
@//for(Int iz==0@iz<2@iz++)

MCWAE10_Loop0:
    
    
@//for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    movs r12, #7



MCWAE10_Loop1:

@// t1 == ppxlcRefMB[0]  (ppxlcRefMB[0 + 2] << 16)@
    .if DYNAMIC_EDGEPAD==1
    ldrb r4, [r1, r2, lsr #17]!
    .else
    ldrb r4, [r1, r2]!
    .endif

    ldrb r14, [r1, #2]
@@relo0 orr r4, r4, r14, lsl #16

@// t2 == ppxlcRefMB[1]  (ppxlcRefMB[1 + 2] << 16)@
    ldrb r5, [r1, #1]
    orr r4, r4, r14, lsl #16 @@relo0
    ldrb r14, [r1, #3]
@@relo1 orr r5, r5, r14, lsl #16

@// t3 == ppxlcRefMB[2]  (ppxlcRefMB[2 + 2] << 16)@
    ldrb r6, [r1, #2]
    orr r5, r5, r14, lsl #16 @@relo1
    ldrb r14, [r1, #4]
@@relo2 orr r6, r6, r14, lsl #16

@// u0 == (t1 + t2 + rndCtrl)@
    add r4, r4, r5
    add r4, r4, r8

@// u0 == u0>>1@
    mov r4, r4, lsr #1

    orr r6, r6, r14, lsl #16@@relo2

@// u1 == (t2 + t3 + rndCtrl)@
    add r5, r5, r6
    add r5, r5, r8

@// u1   >>== 1@
    mov r5, r5, lsr #1

@// u0  &== mask@

    bic r4, r4, #0xff00


@// u1  &== mask@

    bic r5, r5, #0xff00

@ //   if(pErrorBuf2 != NULL)
    cmp r3, #0

    beq MCWAE10_L0   



@@@Macro: AndAddError $pErrorBuf, $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch

    AndAddError r9, r6, r7, r4, r5, r10, r14


MCWAE10_L0:
         
@//y0 == (u0)  ((u1) << 8)@

    orr r4, r4, r5, lsl #8

@//* (U32 *) ppxlcPredMB== y0@
@// ppxlcPredMB   +==  iWidthPrev@

    .if DYNAMIC_EDGEPAD==1
    str r4, [ r0 ], r11
    .else
    str r4, [ r0 ], r2 
    .endif
            
@// ppxlcRefMB   +==  iWidthPrev@

@*  add r1, r1, r2

@ // } //for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    subs r12, r12, #1
    bge MCWAE10_Loop1


@//     ppxlcRefMB==ppxlcRefMB2+4@

    .if DYNAMIC_EDGEPAD==1
    sub r1, r1, r2, lsr #14
    .else
    sub r1, r1, r2, lsl #3
    .endif
    add r1, r1, #4

@//     ppxlcPredMB==ppxlcPredMB2+4@

    .if DYNAMIC_EDGEPAD==1
    sub r0, r0, r11, lsl #3
    .else
    sub r0, r0, r2, lsl #3
    .endif
    add r0, r0, #4

    

    eor r9, r9, r3
    tst r9, #4

@//     pErrorBuf==pErrorBuf2+1@

    
    add r9, r3, #4


    beq MCWAE10_Loop0

@//    if((err_overflow & 0xff00ff00) )

    tst r10, #0xff000000
    tsteq r10, #0xff00

    .if DYNAMIC_EDGEPAD==1
    ldmeqfd   sp!, {r4 - r12, PC}
    .else
    ldmeqfd   sp!, {r4 - r12, PC}
    .endif
    

@//MotionCompMixed010Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2)@
    
    sub r0, r0, #8
    sub r1, r1, #8

    .if DYNAMIC_EDGEPAD==1
    add r1, r1, r2, lsr #17
    .else
    add r1, r1, r2
    .endif

    orr r2, r2, r8, lsl #16

    bl _MotionCompWAddError10Complete

    .if DYNAMIC_EDGEPAD==1
    ldmfd   sp!, {r4 - r12, PC}
    .else
    ldmfd   sp!, {r4 - r12, PC}
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

    stmfd   sp!, {r4 - r12, r14} @ 
    FRAME_PROFILE_COUNT

@// U32 err_overflow == 0@

    mov r10, #0

@//   U32 rndCtrl == iWidthPrev>>16@
@    mov r11, r2, lsr #16
    and r11, r2, #0x10000

@// rndCtrl == rndCtrl << 16@
    orr r11, r11, r11, lsr #16

@//iWidthPrev  &== 0xffff@

    bic r2, r2, #0x10000

@//for(Int iz==0@iz<2@iz++)

MCWAE01_Loop0:

@// assume r8==iWidthPrev
    
@//     t_odd_2 == pLine[1]  pLine[3] << 16@
    ldrb r7, [r1, #1]
    ldrb r6, [r1, #3]
@@relo0 orr r7, r7, r6, lsl #16

@//     t_even_2 == pLine[0]  pLine[2] << 16@

@//pLine   +==  iWidthPrev@
    ldrb r14, [r1, #2]
    orr r7, r7, r6, lsl #16 @@relo0

    .if DYNAMIC_EDGEPAD==1
    ldrb r6, [r1], r2, lsr #17
    .else
    ldrb r6, [r1], r2
    .endif

    movs r12, #7 @@relo1
    orr r6, r6, r14, lsl #16
    
@//for (Int iy  ==  0@ iy < 8@ iy ++ ) 

@@relo1 mov r12, #8

MCWAE01_Loop1:

@//  t_even_1 == t_even_2@
    mov r4, r6

@//  t_odd_1 == t_odd_2@
    mov r5, r7

@//t_odd_2 == pLine[1]  pLine[3] << 16@
    ldrb r7, [r1, #1]
    ldrb r6, [r1, #3]
@@relo2 orr r7, r7, r6, lsl #16
    
@//     t_even_2 == pLine[0]  pLine[2] << 16@

@//pLine   +==  iWidthPrev@
    ldrb r14, [r1, #2]
    orr r7, r7, r6, lsl #16 @@relo2

    .if DYNAMIC_EDGEPAD==1
    ldrb r6, [r1], r2, lsr #17
    .else
    ldrb r6, [r1], r2
    .endif
    
@@relo3 orr r6, r6, r14, lsl #16

@// u1 == ((t_odd_1 + t_odd_2 + rndCtrl))@
@// u1   >>== 1@
    add r5, r5, r7
    add r5, r5, r11
    mov r5, r5, lsr #1

    orr r6, r6, r14, lsl #16 @@relo3

@// u0 == (t_even_1 + t_even_2 + rndCtrl)@
@// u0 == u0>>1@
    add r4, r4, r6
    add r4, r4, r11
    mov r4, r4, lsr #1

@// u0  &== mask@

    bic r4, r4, #0xff00


@// u1  &== mask@

    bic r5, r5, #0xff00

@ //   if(pErrorBuf2 != NULL)
    cmp r3, #4

    ble MCWAE01_L0   



@@@Macro: AndAddError $pErrorBuf, $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch

    AndAddError r3, r8, r9, r4, r5, r10, r14

@ update r8 to iWidthPrev now


MCWAE01_L0:

@//y0 == (u0)  ((u1) << 8)@

    orr r4, r4, r5, lsl #8

@//* (U32 *) ppxlcPredMB== y0@
@// ppxlcPredMB   +==  iWidthPrev@

    .if DYNAMIC_EDGEPAD==1
    and r5, r2, #0x3fc @r5==iWidthPrev, r8>>17==iWidthPrefRef
    str r4, [ r0 ], r5 
    .else
    str r4, [ r0 ], r2 
    .endif @.if DYNAMIC_EDGEPAD==1

    

@ // } //for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    subs r12, r12, #1
    bge MCWAE01_Loop1       
      
@//     ppxlcRefMB==ppxlcRefMB2+4@

    .if DYNAMIC_EDGEPAD==1
    sub r1, r1, r2, lsr #14
    sub r1, r1, r2, lsr #17
    .else
    sub r1, r1, r2, lsl #3
    sub r1, r1, r2
    .endif
    add r1, r1, #4

@//     ppxlcPredMB==ppxlcPredMB2+4@

    .if DYNAMIC_EDGEPAD==1
    sub r0, r0, r5, lsl #3
    .else
    sub r0, r0, r2, lsl #3
    .endif
    add r0, r0, #4

    cmp r3, #4
    subgt r3, r3, #128

    tst r3, #4

@//     pErrorBuf==pErrorBuf2+1@

    
    add r3, r3, #4


    beq MCWAE01_Loop0

@//    if((err_overflow & 0xff00ff00) )

    tst r10, #0xff000000
    tsteq r10, #0xff00
    ldmeqfd   sp!, {r4 - r12, PC}
    

@//MotionCompMixed010Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2)@
    
    sub r0, r0, #8
    sub r1, r1, #8
    orr r2, r2, r11, lsl #16
    sub r3, r3, #8

    bl _MotionCompWAddError01Complete

    ldmfd   sp!, {r4 - r12, PC}
    WMV_ENTRY_END

   .endif   @.if WMV_OPT_MOTIONCOMP_ARM == 1 
    
    @@.end     
    
