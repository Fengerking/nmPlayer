    #include "../c/voWMVDecID.h"
    .include "wmvdec_member_arm.h"
    .include "xplatform_arm_asm.h" 

   .if WMV_OPT_MOTIONCOMP_ARM == 1

	@AREA	|.rdata|, DATA, READONLY
	@ .rdata
    .globl  _MotionCompMixedHQHIComplete
    .globl  _MotionCompMixedAlignBlock
    .globl  _MotionCompMixed000Complete
    .globl  _MotionCompWAddError00Complete
            
    .globl  _MotionCompMixedHQHI   
          
@PRESERVE8
	@AREA	|.text|, CODE, READONLY
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

    .macro AndAddErrorPartial @err_even, err_odd, u0, u1, err_overflow, scratch
@//u0 == u0 + err_even-((err_even & 0x8000) << 1)@

    and $5, $0, #0x8000
    sub $5, $0, $5, lsl #1
    add $2, $2, $5

@//err_overflow  |= u0@
    orr $4, $4, $2

@//u1 == u1 + err_odd -((err_odd  & 0x8000) << 1)@

    and $5,$1,#0x8000
    sub $5,$1,$5, lsl #1
    add $3, $3, $5

@//err_overflow  |= u1@
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
  
  
    WMV_LEAF_ENTRY MotionCompMixedHQHI

@at_even_1==r4
@at_even_2==r5
@at_even_3==r6
@at_even_4==r7
@at_odd_1==r5
@at_odd_2==r6
@at_odd_3==r7
@at_odd_4==r8
@t_even_1==r6
@t_even_2==r7
@t_even_3==r8
@t_even_4==r9
@t_odd_1==r6
@t_odd_2==r7
@t_odd_3==r8
@t_odd_4==r9
@u2==r4
@u3==r5
@err_even==r6
@err_odd==r7
@y0==r4
@overflow==r12
@err_overflow==r10
@0x00080008==r11
@iy==r2
@ppxlcPredMB==r0
@ppxlcRefMB==r1
@pErrorBuf2==r3
@pErrorBuf==r2
@r14 scratch

@stack saved area
@   err_overflow
@   iy
@   iWidthPrev
@   at_even_2,3,4
@   at_odd_2,3,4

    stmfd   sp!, {r4 - r12, r14} @ //r0-r3 are preserved
    FRAME_PROFILE_COUNT

    

@@  r10 is pBlock 
    ldr r10, [ sp, #40 ]@ 

@// U32 overflow == 0@

    mov r12, #0
    

@//U32 mask == 0x00ff00ff@

@//0x00080008
    mov r11,#0x00080000
    orr r11, r11, #0x8

    str r2, [sp, #-16]!@ save r2
    mov r4, r2

    sub sp, sp, #24 @ make room for t_even/odd_2,3,4

@//const PixelC* pLine  ==  ppxlcRefMB + 2*iWidthPrev+1@

    .if DYNAMIC_EDGEPAD==1
    add r1, r1, r4, lsr #16
    .else
    add r1, r1, r4, lsl #1
    .endif
    add r1, r1, #1

@// U32 err_overflow == 0@
    str r12, [ sp , #32 ]@ 

@//for(Int iz==0@iz<2@iz++)

MCMHQHI_Loop0:

@ initially pLine==r1==ppxlcRefMB2+2*iWidthPrev+1, so odd first
@ r4 == iWidthPrev

@//at_odd_4 == pLine[1] | pLine[3] << 16@
@//pLine   -==  iWidthPrev@
    .if DYNAMIC_EDGEPAD==1
    ldrb r8, [r1, -r4, lsr #17 ]!   
    .else
    ldrb r8, [r1, -r4 ]!
    .endif   
    ldrb r14, [r1, #2]  
@@relo0 orr r8, r8, r14, lsl #16

@//at_odd_3 == pLine[1] | pLine[3] << 16@
@//pLine   -==  iWidthPrev@
    .if DYNAMIC_EDGEPAD==1
    ldrb r7, [r1, -r4, lsr #17 ]!       
    .else
    ldrb r7, [r1, -r4 ]!    
    .endif
    orr r8, r8, r14, lsl #16 @@relo0
    ldrb r14, [r1, #2]  
@@relo1 orr r7, r7, r14, lsl #16
        
@//at_odd_2 == pLine[1] | pLine[3] << 16@
@//pLine   -==  iWidthPrev@
    .if DYNAMIC_EDGEPAD==1
    ldrb r6, [r1, -r4, lsr #17]!
    .else
    ldrb r6, [r1, -r4 ]!
    .endif       
    
    orr r7, r7, r14, lsl #16 @@relo1    
    ldrb r14, [r1, #2]  
    sub r1, r1, #1@@relo2
    orr r6, r6, r14, lsl #16

@ save at_odd_2,3,4 now

    stmia sp, { r6 - r8 }

@ now deal with the at_even_2,3,4

@@relo2 sub r1, r1, #1

@//at_even_2 == pLine[0] | pLine[2] << 16@
@//pLine   +==  iWidthPrev@

    ldrb r6, [r1, #2]

    .if DYNAMIC_EDGEPAD==1
    ldrb r5, [r1], r4, lsr #17
    .else
    ldrb r5, [r1], r4
    .endif
    
@@relo3 orr r5, r5, r6, lsl #16

@//at_even_3 == pLine[0] | pLine[2] << 16@
@//pLine   +==  iWidthPrev@

        
    ldrb r7, [r1, #2]
    orr r5, r5, r6, lsl #16@@relo3
    .if DYNAMIC_EDGEPAD==1
    ldrb r6, [r1], r4, lsr #17
    .else
    ldrb r6, [r1], r4
    .endif
@@relo4 orr r6, r6, r7, lsl #16
    

@//at_even_4 == pLine[0] | pLine[2] << 16@
@//pLine   +==  iWidthPrev@

        
    ldrb r14, [r1, #2]
    orr r6, r6, r7, lsl #16 @@relo4
    .if DYNAMIC_EDGEPAD==1
    ldrb r7, [r1], r4, lsr #17
    .else
    ldrb r7, [r1], r4
    .endif
    add r8, sp, #12 @@relo5
    orr r7, r7, r14, lsl #16

@@relo5 add r8, sp, #12
    stmia r8, { r5 - r7 }

@//for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    movs r2, #7

MCMHQHI_Loop1:

@ r1 starts offset by 0, so load even regs first, r8== sp+12

@@relo6 ldmia r8, { r4 - r6 }

@//at_even_4 == pLine[0] | pLine[2] << 16@

    ldrb r7, [r1]

    ldrb r14, [r1, #2]
    ldmia r8, { r4 - r6 } @@relo6
    orr r7, r7, r14, lsl #16

@// u2 == ((at_even_2 + at_even_3)*9-(at_even_1 + at_even_4) + 0x00080008)@

@@macro@ CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r4, r4, r5, r6, r7, r11, r14

@ now save at_even_2,3,4

    stmia r8, { r5 - r7 }

@//overflow  |== u2@ 
    orr r12, r12, r4

@// u2 == u2>>4@
    mov r4, r4, lsr #4


@ now load at_odd_2,3,4

@@relo7 ldmia sp, { r5 - r7 }
    
@//at_odd_4 == pLine[1] | pLine[3] << 16@

    ldrb r8, [r1, #1]
    ldrb r14, [r1, #3]
    ldmia sp, { r5 - r7 }@@relo7
    orr r8, r8, r14, lsl #16
    
    stmia sp, { r6 - r8 }@@relo10
@//u3 == ((at_odd_2 + at_odd_3)*9-(at_odd_1 + at_odd_4) + 0x00080008)@

@@macro@ CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r5, r5, r6, r7, r8, r11, r14

@ now save t_odd_2,3,4

@@relo10    stmia sp, { r6 - r8 }

@//overflow  |== u3@ 
@@relo9 orr r12, r12, r5

@// u3 == u3>>4@
@@relo8 mov r5, r5, lsr #4

    
@ loading t_even_1-4 now
    ldmia r10!, { r6 - r8 }
    ldr r9, [ r10 ], #32

    orr r12, r12, r5 @@relo9
    mov r5, r5, lsr #4@relo8

@// u0 == ((t_even_2 + t_even_3)*9-(t_even_1 + t_even_4) + 0x00080008)@
@@macro@ CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r6, r6, r7, r8, r9, r11, r14

@// overflow |== u0@ 
    orr r12, r12, r6

@// u0 == u0>>4@
@//u0 == ((u0 & mask) + u2 + 0x00010001)>>1@
    bic r6, r6, #0xff000
    add r4, r6, r4, lsl #4
@@relo11    add r4, r4, r11, lsl #1
@@relo12    mov r4, r4, lsr #5

@ loading t_odd_1-4 now

   ldmia r10!, { r6 - r8}
@ update pBlockU32 now
   ldr r9, [ r10 ], #-52

   add r4, r4, r11, lsl #1@@relo11 
   mov r4, r4, lsr #5@@relo12 


@// u1 == ((t_odd_2 + t_odd_3)*9-(t_odd_1 + t_odd_4) + 0x00080008)@
@@macro@ CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r6, r6, r7, r8, r9, r11, r14
            
@//overflow |== u1@
    orr r12, r12, r6
            
@//u1   >>== 4@
@//u1 == ((u1 & mask) + u3 + 0x00010001)>>1@
    bic r6, r6, #0xff000
    add r5, r6, r5, lsl #4
    add r5, r5, r11, lsl #1
    mov r5, r5, lsr #5

@// u0  &== mask@
    bic r4, r4, #0xff00

@// u1  &== mask@
    bic r5, r5, #0xff00

@ //   if(pErrorBuf2 != NULL)

    cmp r3, #4

    ble MCMHQHI_L0   

    str r10, [ sp, #80 ]@@

    ldr r10, [ sp, #32 ]@ restore err_overflow 
@@@Macro: AndAddError $pErrorBuf, $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch

    AndAddError r3, r6, r7, r4, r5, r10, r14

    str r10, [ sp, #32 ]@ save err_overflow 

    ldr r10, [ sp, #80 ]@@

MCMHQHI_L0:
         

@//y0 == (u0) | ((u1) << 8)@

    orr r5, r4, r5, lsl #8

@//* (U32 *) ppxlcPredMB== y0@
@// ppxlcPredMB   +==  iWidthPrev@

    ldr r4, [sp, #24] @ get the iWidthPrev now

    add r8, sp, #12 @@relo14
@@relo13    str r5, [ r0 ], r4 

    .if DYNAMIC_EDGEPAD==1
    and r7, r4, #0x3fc @r7==iWidthPrev, r4>>17==iWidthPrefRef
    add r1, r1, r4, lsr #17  @@relo10 @r1 still even
    .else
@// pLine  +==  iWidthPrev@
    add r1, r1, r4 @@relo10 @r1 still even
    .endif @.if DYNAMIC_EDGEPAD==1

@ // } //for (Int iy  ==  0@ iy < 8@ iy ++ ) 

@  prepare r8 for the loop start
@@relo14 add r8,    sp, #12

    .if DYNAMIC_EDGEPAD==1
    str r5, [ r0 ], r7 @@relo13
    .else
    str r5, [ r0 ], r4 @@relo13
    .endif

    subs r2, r2, #1
    bge MCMHQHI_Loop1

@//     ppxlcRefMB==ppxlcRefMB2+4@

    .if DYNAMIC_EDGEPAD==1
    sub r1, r1, r4, lsr #14  @ this brings r1 back to ppxlcRefMB2+2*iWidthPrev
    .else
    sub r1, r1, r4, lsl #3  @ this brings r1 back to ppxlcRefMB2+2*iWidthPrev
    .endif
    add r1, r1, #5  @ 4+1 make sure r1 is odd now

@//     ppxlcPredMB==ppxlcPredMB2+4@
    .if DYNAMIC_EDGEPAD==1
    sub r0, r0, r7, lsl #3
    .else
    sub r0, r0, r4, lsl #3
    .endif
    add r0, r0, #4

@//pBlockU32   +==  (3+11)@

    add r10, r10, #56 

    cmp r3, #4
    subgt r3, r3, #128

    tst r3, #4

@//     pErrorBuf==pErrorBuf2+1@

    
    add r3, r3, #4


    beq MCMHQHI_Loop0

@//   } //for(Int iz==0@iz<2@iz++)


    ldr r11, [ sp, #32 ]
    add sp, sp, #40 @ adjust the stack now

@//    if((err_overflow & 0xff00ff00) || (overflow & 0xf000f000))

    tst r11, #0xff000000
    tsteq r11, #0xff00
    tsteq r12, #0xf0000000
    tsteq r12, #0xf000
    ldmeqfd   sp!, {r4 - r12, PC}
    

@//MotionCompMixedHQHIComplete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2)@
    
    sub r0, r0, #8
    sub r1, r1, #9
    .if DYNAMIC_EDGEPAD==1
    sub r1, r1, r4, lsr #16
    .else
    sub r1, r1, r4, lsl #1
    .endif
    mov r2, r4
    sub r3, r3, #8

    sub r14, r10, #176

@    str r14, [sp, #-4]
    str r14, [sp, #-8]
    sub sp, sp, #8
    bl _MotionCompMixedHQHIComplete

    add sp, sp, #8

    ldmfd   sp!, {r4 - r12, PC}
    WMV_ENTRY_END

    .endif   @.if WMV_OPT_MOTIONCOMP_ARM == 1
    @@.end 
