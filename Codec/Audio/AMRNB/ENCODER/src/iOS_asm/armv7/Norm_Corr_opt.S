@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@****************************************************************

@static void Norm_Corr (Word16 exc[], 
@                       Word16 xn[], 
@                       Word16 h[],
@                       Word16 L_subfr,
@                       Word16 t_min, 
@                       Word16 t_max,
@                       Word16 corr_norm[])
@
        #include "voAMRNBEncID.h"
        .text   .align   4
	.globl   _Norm_corr_asm
	.globl   _Convolve_asm
	.globl   _Inv_sqrt1

@******************************
@ constant
@******************************
.set    EXC, 0
.set    XN , EXC+ 4
.set    H  ,  XN + 4
.set    L_subFR  , H + 4
.set    STACK_USEd , L_subFR + 100
.set    T_MIN  , STACK_USEd + 4*10
.set    T_MAX  , T_MIN + 4
.set    CORR_NORM , T_MAX + 4
                  
_Norm_corr_asm:

        stmfd      r13!, {r4 - r12, r14}  
        sub        r13, r13, #STACK_USEd
  
        add        r8, r13, #20                 @get the excf[L_subFR]
        ldr        r4, [r13, #T_MIN]            @get t_min
        rsb        r11, r4, #0                  @k = -t_min
        add        r5, r0, r11, lsl #1          @get the &exc[k]   
        
        @transfer Convolve function
        stmfd       sp!, {r0 - r3}
        mov         r0, r5
        mov         r1, r2
        mov         r2, r8               @r2 --- excf[]
        bl          _Convolve_asm
        @mov        r12, r2              @get the excf[]
        ldmfd       sp!, {r0 - r3}
        @ get sum result
        @vmov.s32       q10, #0              @s = 0
        mov        r12, r8              @copy the excf[] address

LOOP1:
        vld1.s16       {q0, q1}, [r12]!
        vld1.s16       {q2, q3}, [r12]!
        vld1.s16       {q4}, [r12]!
             
        vqdmull.s16    q10, d0, d0
        vqdmlal.s16    q10, d1, d1
        vqdmlal.s16    q10, d2, d2
        vqdmlal.s16    q10, d3, d3
        vqdmlal.s16    q10, d4, d4
        vqdmlal.s16    q10, d5, d5
        vqdmlal.s16    q10, d6, d6
        vqdmlal.s16    q10, d7, d7
        vqdmlal.s16    q10, d8, d8
        vqdmlal.s16    q10, d9, d9
        vqadd.s32      d20, d20, d21
        vmov.s32       r9,  d20[0]
        vmov.s32       r10, d20[1]
        qadd           r9, r9, r10            @get the excf[] sum
         
        cmp            r9, #0x4000000
        @r8 ---  excf[] or s_excf[]
        
        bgt            B_ELSE
        mov            r12, #3
        mov            r10, #0
        b              LOOP_FOR

B_ELSE:
        mov            r12, r8                @load data address
        vshr.s16       q0, q0, #2
        vshr.s16       q1, q1, #2    
        vshr.s16       q2, q2, #2
        vshr.s16       q3, q3, #2
        vshr.s16       q4, q4, #2                
        vst1.s16       {d0, d1, d2, d3}, [r12]!
        vst1.s16       {d4, d5, d6, d7}, [r12]!
        vst1.s16       {d8, d9}, [r12]!
        mov            r12, #1                @h_fac = 1
        mov            r10, #2                @scaling = 2
        
@*********
@ need store register
@ r0 -- exc[]@ r1 -- xn[]@ r2 -- h[]@ r4 -- t_min@ r5 -- t_max
@ r6 -- corr_norm[]@ r8 --- excf[] or s_excf[]@
@ r12 -- h_fac@ r10 -- scaling@ r14 --- k--

LOOP_FOR:

        vdup.s32       q13, r12
        @ldr        r5, [r13, #T_MAX]                           @ get = t_max
        mov            r7, r4                                      @ i = t_min
        rsb            r14, r4, #0
        sub            r14, r14, #1                                @ k--   r14 to k-- 
LOOP:
        @vmov.s32       q10,  #0                                    @ s = 0
        @vmov.s32       q11, #0                                    @ s1 = 0
        mov            r3, r8                                      @ r3 --- s_excf[j]
        mov            r11, r3                                     @ copy s_excf address
        mov            r9, r1                                      @ r9 --- xn[j]
        vld1.s16       {d0, d1, d2, d3}, [r3]!                     @ load 4*4 s_excf[]
        vld1.s16       {d4, d5, d6, d7}, [r3]!                     @ load 4*4 s_excf[]       
        vld1.s16       {d8, d9}, [r3]!                             @ load 4 s_excf[]

        vld1.s16       {d10, d11, d12, d13}, [r9]!                 @ load 4*4 x[]
        vld1.s16       {d14, d15, d16, d17}, [r9]!                 @ load 4*4 x[]
        vld1.s16       {d18, d19}, [r9]!                           @ load 4 x[]

        vqdmull.s16    q10, d0, d0
        vqdmull.s16    q11, d0, d10        
        vqdmlal.s16    q10, d1, d1
        vqdmlal.s16    q11, d1, d11
        vqdmlal.s16    q10, d2, d2
        vqdmlal.s16    q11, d2, d12        
        vqdmlal.s16    q10, d3, d3
        vqdmlal.s16    q11, d3, d13
        vqdmlal.s16    q10, d4, d4
        vqdmlal.s16    q11, d4, d14
        vqdmlal.s16    q10, d5, d5
        vqdmlal.s16    q11, d5, d15
        vqdmlal.s16    q10, d6, d6
        vqdmlal.s16    q11, d6, d16
        vqdmlal.s16    q10, d7, d7
        vqdmlal.s16    q11, d7, d17
        vqdmlal.s16    q10, d8, d8
        vqdmlal.s16    q11, d8, d18
        vqdmlal.s16    q10, d9, d9        
        vqdmlal.s16    q11, d9, d19 
        
        vqadd.s32      d20, d20, d21
        vqadd.s32      d22, d22, d23
        
        mov            r3, r2                                      @r3 --- h[] address
        vld1.s16       {d10, d11, d12, d13}, [r3]!
        vld1.s16       {d14, d15, d16, d17}, [r3]!
        vld1.s16       {d18, d19}, [r3]!                           @load all h[] elements

        @ exc[k]
        add            r3, r0, r14, lsl #1                         @get exc[k] address
        ldrsh          r9, [r3] 
        
        vdup.s16       d28, r9                                     @bl0 ---- exc[k]
        
        
        vqdmull.s16    q12, d10, d28                              @L_mult(exc[k], h[j])
        vqshl.s32      q12, q12, q13                              @L_shl(s2, h_fac)
        vshrn.s32      d10, q12, #16                              @extract_h(s)
        
         
        vqdmull.s16    q12, d11, d28                              @L_mult(exc[k], h[j])
        vqshl.s32      q12, q12, q13                              @L_shl(s2, h_fac)
        vshrn.s32      d11, q12, #16                         


        vqdmull.s16    q12, d12, d28                             @L_mult(exc[k], h[j])
        vqshl.s32      q12, q12, q13                             @L_shl(s2, h_fac)
        vshrn.s32      d12, q12, #16


        vqdmull.s16    q12, d13, d28                              @L_mult(exc[k], h[j])
        vqshl.s32      q12, q12, q13                             @L_shl(s2, h_fac)
        vshrn.s32      d13, q12, #16


        vqdmull.s16    q12, d14, d28                              @L_mult(exc[k], h[j])
        vqshl.s32      q12, q12, q13                             @L_shl(s2, h_fac)
        vshrn.s32      d14, q12, #16


        vqdmull.s16    q12, d15, d28                              @L_mult(exc[k], h[j])
        vqshl.s32      q12, q12, q13                             @L_shl(s2, h_fac)
        vshrn.s32      d15, q12, #16


        vqdmull.s16    q12, d16, d28                             @L_mult(exc[k], h[j])
        vqshl.s32      q12, q12, q13                             @L_shl(s2, h_fac)
        vshrn.s32      d16, q12, #16



        vqdmull.s16    q12, d17, d28                              @L_mult(exc[k], h[j])
        vqshl.s32      q12, q12, q13                             @L_shl(s2, h_fac)
        vshrn.s32      d17, q12, #16


        vqdmull.s16    q12, d18, d28                             @L_mult(exc[k], h[j])
        vqshl.s32      q12, q12, q13                             @L_shl(s2, h_fac)
        vshrn.s32      d18, q12, #16


        vqdmull.s16    q12, d19, d28                              @L_mult(exc[k], h[j])
        vqshl.s32      q12, q12, q13                             @L_shl(s2, h_fac)
        vshrn.s32      d19, q12, #16

        @b10---b19
        @L_mult(exc[k], h[0])----- L_mult(exc[k], h[39]) 
            
        sub            r3, r8, #2                                  @get the s_excf[-1] address
        
        @a10---a19
        @a10-a19 ---- s_excf[-1]----s_excf[38] 

        vld1.s16       {q0, q1}, [r3]!
        vld1.s16       {q2, q3}, [r3]!
        vld1.s16       {q4}, [r3]!          
        
        vqadd.s16      q0, q0, q5
                               
	@s_excf[3] = add (extract_h(s2), s_excf[2])@
	@s_excf[2] = add (extract_h(s3), s_excf[1])@
	@s_excf[1] = add (extract_h(s4), s_excf[0])@
	@s_excf[0] = shr (exc[k], scaling)@
	
	
        vqadd.s16      q1, q1, q6
        vqadd.s16      q2, q2, q7
        vqadd.s16      q3, q3, q8
        vqadd.s16      q4, q4, q9
        
        mov            r11, r8
        mov            r9, r9, asr r10                             @s_excf[0]= shr(exc[k], scaling)
        strh           r9, [r11], #2
        vmov.s16       r3, d0[1]
        vmov.s16       r9, d0[2]
        strh           r3, [r11], #2
        strh           r9, [r11], #2
        vmov.s16       r3, d0[3]
        strh           r3, [r11], #2
        vst1.s16       d1, [r11]!
        vst1.s16       {q1, q2}, [r11]!
        vst1.s16       {q3, q4}, [r11]!

        @ store s_excf[0]----s_excf[39]
        
        vmov.s32       r3, d20[0]
        vmov.s32       r9, d20[1]
        qadd           r3, r3, r9

        stmfd          sp!, {r0 - r2, r14}
        mov            r0, r3
        bl             _Inv_sqrt1
        mov            r3, r0
        ldmfd          sp!, {r0 - r2, r14}

   
        vmov.s32       r9, d22[0]
        vmov.s32       r11, d22[1]
        qadd           r9, r9, r11
 
        @ r3 --- s = Inv_sqrt (s)@
        @ r9 --- s1
           
        smultt         r11, r3, r9                                      @ s = (corr_h * norm_h)@
        mov            r6, r3, lsr #16                     @ r6 --- norm_h
        sub            r5, r3, r6, lsl #16                
        mov            r5, r5, asr #1                      @ r5 --- norm_l

        smultb         r5, r9, r5
        mov            r5, r5, asr #15
        add            r11, r11, r5

        mov            r6, r9, lsr #16                     @ r6 --- corr_h
        sub            r5, r9, r6, lsl #16
        mov            r5, r5, asr #1                      @ r5 --- corr_l

        smultb         r5, r3, r5
        mov            r5, r5, asr #15
        add            r11, r11, r5
 
  
        @SSAT       r11, #32, r11, lsl #17                      @ L_shl(s, 17)
              
        cmp            r11, #0
        beq            HERE
        eor            r5, r11, r11, lsl #1
        clz            r3, r5
        cmp            r3, #17
        movge          r11, r11, lsl #17
        bge            HERE
        cmp            r11, #0
        movlt          r11, #0x80000000
        movgt          r11, #0x7fffffff
        
HERE:
        mov            r11, r11, lsr #16                           @ extract_h()
        ldr            r3, [r13, #CORR_NORM]                       @ get corr_norm address
        ldr            r5, [r13, #T_MAX]                           @ get = t_max
        add            r3, r3, r7, lsl #1                          @ get corr_norm[i] address
        strh           r11, [r3]
        add            r7, r7, #1
        sub            r14, r14, #1
        cmp            r7, r5
        ble            LOOP  
            

Norm_corr_asm_end: 
        
        add            r13, r13, #STACK_USEd      
        ldmfd          r13!, {r4 - r12, r15}
    
        @.ENd
