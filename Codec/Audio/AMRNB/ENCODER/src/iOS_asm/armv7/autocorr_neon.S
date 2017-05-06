@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@****************************************************************
@Word16 Autocorr2 (
@    Word16 x[],            /* (i)    : Input signal (L_WINdOW)            */
@    Word16 r_h[],          /* (o)    : Autocorrelations  (msb)            */
@    Word16 r_l[],          /* (o)    : Autocorrelations  (lsb)            */
@    const Word16 wind[]    /* (i)    : window for LPC analysis (L_WINdOW) */
@)
        #include "voAMRNBEncID.h"
        .text   .align   4
	.globl   _Autocorr_asm

_Autocorr_asm:

        stmfd      r13!, {r4 - r12, r14}  
        sub        r13,  r13, #488                     @get the y[L_WINdOW] space
        mov        r6,   r13                            @store y[L_WINdOW] address
        mov        r8,   #240                           @L_WINdOW = 240      

@ for(i = 0@ i < L_WINdOW@ i++)
@{
@   y[i] = mult_r(x[i], wind[i])@
@}
LOOP1:
        vmov.s32        q10, #0x4000
        vmov.s32        q11, #0x4000
        vmov.s32        q12, #0x4000
        vmov.s32        q13, #0x4000
        vmov.s32        q14, #0x4000


        vld1.s16       {d0, d1, d2, d3}, [r0]!                         @load ten x[]
        vld1.s16       {d4, d5, d6, d7}, [r0]!
        vld1.s16       {d8, d9}, [r0]!
        
        vld1.s16       {d10, d11, d12, d13}, [r3]!                     @ load ten wind[]
        vld1.s16       {d14, d15, d16, d17}, [r3]!
        vld1.s16       {d18, d19}, [r3]!
        
                            
        vmlal.s16       q10, d0, d10
        vmlal.s16       q11, d1, d11
        vmlal.s16       q12, d2, d12
        vmlal.s16       q13, d3, d13
        vmlal.s16       q14, d4, d14

        vshrn.s32       d0, q10, #15                                    @get five y[i]
        vshrn.s32       d1, q11, #15
        vshrn.s32       d2, q12, #15
        vshrn.s32       d3, q13, #15
        vshrn.s32       d4, q14, #15

        vmov.s32        q10, #0x4000
        vmov.s32        q11, #0x4000
        vmov.s32        q12, #0x4000
        vmov.s32        q13, #0x4000
        vmov.s32        q14, #0x4000
        
        vmlal.s16       q10, d5, d15
        vmlal.s16       q11, d6, d16
        vmlal.s16       q12, d7, d17
        vmlal.s16       q13, d8, d18
        vmlal.s16       q14, d9, d19
        
        vshrn.s32       d5, q10, #15
        vshrn.s32       d6, q11, #15
        vshrn.s32       d7, q12, #15
        vshrn.s32       d8, q13, #15
        vshrn.s32       d9, q14, #15
        
        vst1.s16        {d0, d1, d2, d3}, [r6]!
        vst1.s16        {d4, d5, d6, d7}, [r6]!
        vst1.s16        {d8, d9}, [r6]!
        subs            r8, r8, #40 
        bgt             LOOP1                            
        
@ do 
@ {
@ }while(overf !=0)

LOOP2:
        vmov.s32        q10,  #0                            @sum = 0
        mov             r6,  r13                           @get y[i] first address
        mov             r8,  #240                          @L_WINdOW = 240 
        
LOOP3:
        vld1.s16        {d0, d1, d2, d3}, [r6]!
        vld1.s16        {d4, d5, d6, d7}, [r6]!
        vld1.s16        {d8, d9, d10, d11}, [r6]!
        vld1.s16        {d12, d13, d14, d15}, [r6]!
        vld1.s16        {d16, d17, d18, d19}, [r6]!
        
        vqdmlal.s16     q10, d0, d0
        vqdmlal.s16     q10, d1, d1
        vqdmlal.s16     q10, d2, d2
        vqdmlal.s16     q10, d3, d3
        vqdmlal.s16     q10, d4, d4
        vqdmlal.s16     q10, d5, d5
        vqdmlal.s16     q10, d6, d6
        vqdmlal.s16     q10, d7, d7
        vqdmlal.s16     q10, d8, d8
        vqdmlal.s16     q10, d9, d9
        vqdmlal.s16     q10, d10, d10
        vqdmlal.s16     q10, d11, d11
        vqdmlal.s16     q10, d12, d12
        vqdmlal.s16     q10, d13, d13
        vqdmlal.s16     q10, d14, d14
        vqdmlal.s16     q10, d15, d15
        vqdmlal.s16     q10, d16, d16
        vqdmlal.s16     q10, d17, d17
        vqdmlal.s16     q10, d18, d18
        vqdmlal.s16     q10, d19, d19
                
        subs            r8, r8, #80
        bgt             LOOP3
        vqadd.s32       d20, d20, d21       
        
        vmov.s32        r5, d20[0]
        vmov.s32        r10, d20[1]

        qadd            r5, r5, r10                       @ r5 --- sum
  
        cmp             r5, #0x7fffffff                   @ if(sum == MAX_32)
        bne             Lable

        mov             r6, r13                           @ get y[i] first address
        mov             r7, r13                           @ store the result
        mov             r8, #240
LOOP4:
        vld1.s16        {d0, d1, d2, d3}, [r6]!
        vld1.s16        {d4, d5, d6, d7}, [r6]!
        vld1.s16        {d8, d9, d10, d11}, [r6]!
        vld1.s16        {d12, d13, d14, d15}, [r6]!
        vld1.s16        {d16, d17, d18, d19}, [r6]!
                
        vshr.s16        q0, q0, #2
        vshr.s16        q1, q1, #2                        @ y[i] >>= 2
        vshr.s16        q2, q2, #2
        vshr.s16        q3, q3, #2
        vshr.s16        q4, q4, #2
        vshr.s16        q5, q5, #2
        vshr.s16        q6, q6, #2                        @ y[i] >>= 2
        vshr.s16        q7, q7, #2
        vshr.s16        q8, q8, #2
        vshr.s16        q9, q9, #2
        
        vst1.s16        {q0, q1}, [r7]!
        vst1.s16        {q2, q3}, [r7]!
        vst1.s16        {q4, q5}, [r7]!
        vst1.s16        {q6, q7}, [r7]!
        vst1.s16        {q8, q9}, [r7]!
        
        subs            r8, r8, #80
        bgt             LOOP4
        b               LOOP2
        
Lable:
        add             r5, r5, #1                        @ sum = L_add(sum, 1L)
        @norm = norm_l (sum)
        clz             r9, r5  
        sub             r4, r9, #1                             
        mov             r5, r5, lsl r4                    @ sum <<=norm

        mov             r10, r5, lsr #16
        strh            r10, [r1], #2                     @ r_h[0] = sum >>16
        sub             r11, r5, r10, lsl #16
        mov             r11, r11, lsr #1
        strh            r11, [r2], #2                     @r_l[0]=(sum - (r_h[0]<<16))>>1
 
        mov             r14, #240
        mov             r7, #1                             @ i = 1
LOOP5:                                                      @ for(i=1@ i <=m@ i++)
        mov             r8, #0                             @ sum = 0@
        mov             r6, r13                            @ get the y[] address
        sub             r10, r14, r7                       @ r10 --- L_WINdOW-i
        add             r12, r6, r7, lsl #1                @ get the y[i]
        mov             r11, #0                            @ j = 0
   
LOOP6:                                                      @ for(j=0@ j<L_WINdOW-i@ j++)
        ldrsh           r4, [r6], #2                            @ y[j]
        ldrsh           r5, [r12], #2                           @ y[j+i]
        add             r11, r11, #1
        mla             r8, r4, r5, r8                          @ sum += (y[j] * y[j+i])
        cmp             r11, r10
        blt             LOOP6

        mov             r8, r8, lsl r9                     @ sum <<=(norm +1)

        mov             r4, r8, lsr #16                    @ r_h[i] = sum >>16@
        strh            r4, [r1], #2
        sub             r5, r8, r4, lsl #16
        mov             r5, r5, lsr #1
        strh            r5, [r2], #2

        add             r7, r7, #1
        cmp             r7, #10
        ble             LOOP5
               
Autocorr_asm_end:  

        add             r13, r13, #488      
        ldmfd           r13!,{r4 - r12, r15}
    
        @.ENd
