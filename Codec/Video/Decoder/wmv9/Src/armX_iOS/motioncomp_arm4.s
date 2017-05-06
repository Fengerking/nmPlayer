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
            
    .globl  _MotionCompMixed000    
    .globl  _MotionCompWAddError00    
      
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
    


    WMV_LEAF_ENTRY MotionCompMixed000

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
@pBlockU32==r1
@pErrorBuf2==r3
@pErrorBuf==r2
@r14 scratch
@stack saved area
@   iWidthPrev


    stmfd   sp!, {r4 - r12, r14} @ 
    FRAME_PROFILE_COUNT

    sub sp, sp, #176 @ sizeof(tempBlock)

    @//iWidthPrev  &== 0xffff@

    bic r2, r2, #0x10000

@    str r1, [sp, #-4]!
@    str r2, [sp, #-4]!

    str r1, [sp, #-4]
    str r2, [sp, #-8]
    sub	sp, sp, #8

    stmfd   sp!, {r0, r3} @ //r0-r3 are preserved

    add r0, sp, #16

    bl _MotionCompMixedAlignBlock

    ldmfd   sp!, {r0, r3}

@   now sp+0 is iWidthPrev, sp+4 is original ppxlcRefMB

    add r1, sp, #8@  r1 == pBlockU32

    .if DYNAMIC_EDGEPAD==1
        .if ARCH_V3 == 1
            LOADONE16bitsLo sp, #0, r2 @ sp is always word aligned.
        .else
            ldrh r2, [sp]@@relo2
        .endif
    .else
        ldr r2, [sp]@@relo2 
    .endif

@// U32 err_overflow == 0@

    mov r10, #0

@// U32 overflow == 0@

    mov r9, #0

@//U32 mask == 0x00ff00ff@

@//0x00080008
    mov r11,#0x00080000
    orr r11, r11, #0x8

@//for(Int iz==0@iz<2@iz++)

MCM000_Loop0:
    
    
@//for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    mov r12, #8

MCM000_Loop1:

@ load t_even_1-4 now

    ldmia r1!, { r4 - r6 }
    ldr r7, [ r1 ], #32

@// u0 == ((t_even_2 + t_even_3)*9-(t_even_1 + t_even_4) + 0x00080008)@
 

@@macro@ CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r4, r4, r5, r6, r7, r11, r14

    ldmia r1!, { r5 - r7 }@@relo1
    ldr r8, [ r1 ], #-52
@//overflow  |== u0@ 
    orr r9, r9, r4

@// u0 == u0>>4@
    mov r4, r4, lsr #4

@ load t_odd_1-4 now

@@relo0    add r1, r1, #44
@@relo1    ldmia r1, { r5 - r8 }

@// u1 == ((t_odd_2 + t_odd_3)*9-(t_odd_1 + t_odd_4) + 0x00080008)@

@@macro@ CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r5, r5, r6, r7, r8, r11, r14
    


@//overflow  |== u1@ 
    orr r9, r9, r5

@// u1 == u1>>4@
    mov r5, r5, lsr #4

@// u0  &== mask@

    bic r4, r4, #0xff00


@// u1  &== mask@

    bic r5, r5, #0xff00

@ //   if(pErrorBuf2 != NULL)
    cmp r3, #4

    ble MCM000_L0    



@@@Macro: AndAddError $pErrorBuf, $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch

    AndAddError r3, r6, r7, r4, r5, r10, r14



MCM000_L0:
         

@//y0 == (u0) | ((u1) << 8)@

    orr r4, r4, r5, lsl #8

@//* (U32 *) ppxlcPredMB== y0@
@// ppxlcPredMB   +==  iWidthPrev@

@@relo2 ldr r8, [sp]

    str r4, [ r0 ], r2 

            
@// ppxlcRefMB   +==  iWidthPrev@


@ // } //for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    subs r12, r12, #1
    bgt MCM000_Loop1



@//     pBlockU32+==3+11@

    add r1, r1, #56

@//     ppxlcPredMB==ppxlcPredMB2+4@

    sub r0, r0, r2, lsl #3
    add r0, r0, #4

    cmp r3, #4
    subgt r3, r3, #128
    

    tst r3, #4

@//     pErrorBuf==pErrorBuf2+1@

    
    add r3, r3, #4


    beq MCM000_Loop0


@//   } //for(Int iz==0@iz<2@iz++)

    .if DYNAMIC_EDGEPAD==1
    ldr r2, [sp]
    .endif

    ldr r1, [ sp, #4 ]
    add sp, sp, #184

@//    if((err_overflow & 0xff00ff00) || (overflow & 0xf000f000))

    tst r10, #0xff000000
    tsteq r10, #0xff00
    tsteq r9, #0xf0000000
    tsteq r9, #0xf000
    ldmeqfd   sp!, {r4 - r12, PC}
    

@//MotionCompMixed000Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2)@
    
    sub r0, r0, #8
    sub r3, r3, #8

    bl _MotionCompMixed000Complete

    ldmfd   sp!, {r4 - r12, PC}
    WMV_ENTRY_END
        
        
          
    WMV_LEAF_ENTRY MotionCompWAddError00

@t_even_1==r4
@t_odd_1==r5
@ta1==r6
@t_even_2==r7
@t_odd_2==r8
@ta2==r9
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
@ t_even_2, t_odd_2, ta2

    stmfd   sp!, {r4 - r12, r14} @ 
    FRAME_PROFILE_COUNT

@// U32 err_overflow == 0@

    mov r10, #0

@//   U32 rndCtrl == iWidthPrev>>16@
@   mov r11, r2, lsr #16
    and r11, r2, #0x10000

@// rndCtrl ++ @
    add r11, r11, #0x10000

@// rndCtrl |== rndCtrl << 16@
    orr r11, r11, r11, lsr #16

@//iWidthPrev  &== 0xffff@

    bic r2, r2, #0x10000

    sub sp, sp, #16 @ make room for t_even_2, t_odd_2, ta2

@//for(Int iz==0@iz<2@iz++)

MCWAE00_Loop0:

@// assume r6==iWidthPrev

@// ta2 == pLine[2] | pLine[4] << 16@
    ldrb r9, [r1, #2]
    ldrb r14, [r1, #4]
@@relo0 orr r9, r9, r14, lsl #16
    
@//     t_odd_2 == pLine[1] | pLine[3] << 16@
    ldrb r8, [r1, #1]
    orr r9, r9, r14, lsl #16 @@relo0
    ldrb r7, [r1, #3]
@@relo1 orr r8, r8, r7, lsl #16

@// t_even_2 == pLine[0] | pLine[2] << 16@

@//pLine   +==  iWidthPrev@
    ldrb r14, [r1, #2]
    orr r8, r8, r7, lsl #16 @@relo1

    .if DYNAMIC_EDGEPAD==1
    ldrb r7, [r1], r2, lsr #17
    .else
    ldrb r7, [r1], r2
    .endif
    
    movs r12, #7 @@relo2
    orr r7, r7, r14, lsl #16

    stmia sp, { r7 - r9}
    
@//for (Int iy  ==  0@ iy < 8@ iy ++ ) 

@@relo2 mov r12, #8

MCWAE00_Loop1:


@// t_even_2 == pLine[0] | pLine[2] << 16@
    
    ldrb r7, [r1]

    ldrb r14, [r1, #2]
    ldmia sp, { r4 - r6}@@relo3
    orr r7, r7, r14, lsl #16

@//     t_odd_2 == pLine[1] | pLine[3] << 16@
    ldrb r8, [r1, #1]
    ldrb r14, [r1, #3]
@@relo4 orr r8, r8, r14, lsl #16

@// ta2 == pLine[2] | pLine[4] << 16@
    ldrb r9, [r1, #2]
    orr r8, r8, r14, lsl #16 @@relo4
    ldrb r14, [r1, #4]
@@relo5 orr r9, r9, r14, lsl #16

@//   u0 == (t_even_1 + t_even_2 + t_odd_1 + t_odd_2 + rndCtrl)@
@//  u0 == u0>>2@

    add r4, r4, r5
    add r4, r4, r7
    add r4, r4, r8
    add r4, r4, r11
    mov r4, r4, lsr #2

    orr r9, r9, r14, lsl #16@@relo5

   stmia sp, { r7 - r9 }@@relo6 


@// u1 == (t_odd_1 + t_odd_2 + ta1 + ta2 + rndCtrl)@
@//u1   >>== 2@
    add r5, r5, r6
    add r5, r5, r8
    add r5, r5, r9
    add r5, r5, r11
    mov r5, r5, lsr #2

@// u0  &== mask@

    bic r4, r4, #0xff00


@// u1  &== mask@

    bic r5, r5, #0xff00

@ //   if(pErrorBuf2 != NULL)
    cmp r3, #4

    ble MCWAE00_L0   



@@@Macro: AndAddError $pErrorBuf, $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch

    AndAddError r3, r8, r9, r4, r5, r10, r14

MCWAE00_L0:

@   picking up iWidthPrev
    
@//y0 == (u0) | ((u1) << 8)@

    orr r4, r4, r5, lsl #8

@//* (U32 *) ppxlcPredMB== y0@
@// ppxlcPredMB   +==  iWidthPrev@

    .if DYNAMIC_EDGEPAD==1
    and r5, r2, #0x3fc @r5==iWidthPrev, r8>>17==iWidthPrefRef
    str r4, [ r0 ], r5 
    add r1, r1, r2, lsr #17 @@relo7
    .else
    str r4, [ r0 ], r2 
    add r1, r1, r2 @@relo7
    .endif @.if DYNAMIC_EDGEPAD==1
    

@//pLine   +==  iWidthPrev@

@@relo7   add r1, r1, r6


@ // } //for (Int iy  ==  0@ iy < 8@ iy ++ ) 

    subs r12, r12, #1
    bge MCWAE00_Loop1       
      
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


    beq MCWAE00_Loop0



@//    if((err_overflow & 0xff00ff00) )

    add sp, sp, #16

    tst r10, #0xff000000
    tsteq r10, #0xff00
    ldmeqfd   sp!, {r4 - r12, PC}
    

@//MotionCompMixed010Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2)@
    
    sub r0, r0, #8
    sub r1, r1, #8

    sub r11, r11, #1
    orr r2, r2, r11, lsl #16
    sub r3, r3, #8

    bl _MotionCompWAddError00Complete

    ldmfd   sp!, {r4 - r12, PC}
    WMV_ENTRY_END
    
		.endif  @.if WMV_OPT_MOTIONCOMP_ARM == 1

    @@.end 
