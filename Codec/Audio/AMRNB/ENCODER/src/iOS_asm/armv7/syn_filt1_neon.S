@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@*************************************************************** 

@void Syn_filt_1 (
@    Word16 a[],
@    Word16 x[],
@    Word16 y[], 
@    Word16 mem[]    
@)
@ r0 --- a[]
@ r1 --- x[]
@ r2 --- y[]
@ r3 --- mem[]
@******************************************************************
        #include "voAMRNBEncID.h"
        .text   .align   4
        .globl   _Syn_filt_1
        
_Syn_filt_1:

        stmfd   r13!, {r4 - r11, r14} 
        sub     r13, r13, #116                   
        mov     r6, r0                           
        mov     r5, r1                           
        mov     r4, r2 
        mov     r8, #0
@memcpy(tmp, mem, 20)@
        str     r3, [r13, #4]                 
        mov         r0, r3  
        add         r1, r13, #8                      
        vld1.s16    {d0, d1}, [r0]!
        ldrsh       r10, [r0], #2
        ldrsh       r11, [r0], #2
        vst1.s16    {d0, d1}, [r1]!
        strh        r10, [r1], #2
        strh        r11, [r1], #2	
                        
        add         r9, r13, #28                      @yy = tmp +10      
        mov         r0, r4       
L10_4: 
                       
        ldrsh   r12, [r6]                         @tmpA = a[0]
        ldrsh   r10, [r5], #2                     @get x[i]

        ldrsh   r11, [r9, #-2]                    @yy[-1]
        ldrsh   r3,  [r6, #2]                     @*(tmpA++)
        rsb     r3,  r3, #0
        smulbb  r14, r10, r12                     @s = x[i] * (*(tmpA++))
        ldrsh   r12, [r6, #4]                     
        ldrsh   r10, [r9, #-4]                    @yy[-2]
        rsb     r12, r12, #0
        smlabb  r14, r3, r11, r14                 @s -=((*(tmpA++))* yy[-1])@
        ldrsh   r3,  [r6, #6]
        ldrsh   r11, [r9, #-6]  
        rsb     r3,  r3, #0        
        smlabb  r14, r12, r10, r14
        ldrsh   r12, [r6, #8]
        ldrsh   r10, [r9, #-8]
        rsb     r12, r12, #0
        smlabb  r14, r3, r11, r14
        ldrsh   r3,  [r6, #10]
        ldrsh   r11, [r9, #-10]
        rsb     r3,  r3, #0
        smlabb  r14, r12, r10, r14
        ldrsh   r12, [r6, #12]
        ldrsh   r10, [r9, #-12]
        rsb     r12, r12, #0
        smlabb  r14, r3, r11, r14
        ldrsh   r3,  [r6, #14]
        ldrsh   r11, [r9, #-14]
        rsb     r3,  r3, #0
        smlabb  r14, r12, r10, r14
        ldrsh   r12, [r6, #16]
        ldrsh   r10, [r9, #-16]
        rsb     r12, r12, #0
        smlabb  r14, r3, r11, r14
        ldrsh   r3,  [r6, #18]
        ldrsh   r11, [r9, #-18]
        rsb     r3,  r3, #0
        smlabb  r14, r12, r10, r14
        ldrsh   r12, [r6, #20]
        ldrsh   r10, [r9, #-20]
        rsb     r12, r12, #0
        smlabb  r14, r3, r11, r14
        smlabb  r14, r12, r10, r14

        @s = ASM_L_shl(s, 4)

        ssat     r12, #32, r14, lsl #4

        mov     r10, #0x8000
        qadd    r14, r12, r10
        add     r8, r8, #1
        mov     r14, r14, asr #16
        
        cmp     r8, #40
        strh    r14, [r9], #2
        strh    r14, [r0], #2
        blt     L10_4


        add         r0, r4, #60
        ldr         r1, [r13, #4]
        vld1.s16    {d0, d1}, [r0]!
        ldrsh       r10, [r0], #2
        ldrsh       r11, [r0], #2
        vst1.s16    {d0, d1}, [r1]!
        strh        r10, [r1], #2
        strh        r11, [r1], #2	 
        
  
        add         r13, r13, #116                    
        ldmfd       r13!, {r4 - r11, r15} 

        @ENdP
        @.ENd 



