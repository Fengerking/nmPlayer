@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
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
        .section  .text
	.global   Norm_corr_asm
	.extern   Convolve_asm
	.extern   Inv_sqrt1

@******************************
@ constant
@******************************
.equ    EXC, 0
.equ    XN , EXC+ 4
.equ    H  ,  XN + 4
.equ    L_SUBFR  , H + 4
.equ    STACK_USED , L_SUBFR + 100
.equ    T_MIN  , STACK_USED + 4*10
.equ    T_MAX  , T_MIN + 4
.equ    CORR_NORM , T_MAX + 4
                  
Norm_corr_asm:
        STMFD      r13!, {r4 - r12, r14}  
        SUB        r13, r13, #STACK_USED
  
        ADD        r8, r13, #20                 @get the excf[L_SUBFR]
        LDR        r4, [r13, #T_MIN]            @get t_min
        RSB        r11, r4, #0                  @k = -t_min
        ADD        r5, r0, r11, LSL #1          @get the &exc[k]   
        
        @transfer Convolve function
        STMFD       sp!, {r0 - r3}
        MOV         r0, r5
        MOV         r1, r2
        MOV         r2, r8               @r2 --- excf[]
        BL          Convolve_asm
        @MOV        r12, r2              @get the excf[]
        LDMFD       sp!, {r0 - r3}
        @ get sum result
        @VMOV.S32       Q10, #0              @s = 0
        MOV        r12, r8              @copy the excf[] address

LOOP1:
        VLD1.S16       {Q0, Q1}, [r12]!
        VLD1.S16       {Q2, Q3}, [r12]!
        VLD1.S16       {Q4}, [r12]!
             
        VQDMULL.S16    Q10, D0, D0
        VQDMLAL.S16    Q10, D1, D1
        VQDMLAL.S16    Q10, D2, D2
        VQDMLAL.S16    Q10, D3, D3
        VQDMLAL.S16    Q10, D4, D4
        VQDMLAL.S16    Q10, D5, D5
        VQDMLAL.S16    Q10, D6, D6
        VQDMLAL.S16    Q10, D7, D7
        VQDMLAL.S16    Q10, D8, D8
        VQDMLAL.S16    Q10, D9, D9
        VQADD.S32      D20, D20, D21
        VMOV.S32       r9,  D20[0]
        VMOV.S32       r10, D20[1]
        QADD           r9, r9, r10            @get the excf[] sum
         
        CMP            r9, #0x4000000
        @r8 ---  excf[] or s_excf[]
        
        BGT            B_ELSE
        MOV            r12, #3
        MOV            r10, #0
        B              LOOP_FOR

B_ELSE:
        MOV            r12, r8                @load data address
        VSHR.S16       Q0, Q0, #2
        VSHR.S16       Q1, Q1, #2    
        VSHR.S16       Q2, Q2, #2
        VSHR.S16       Q3, Q3, #2
        VSHR.S16       Q4, Q4, #2                
        VST1.S16       {D0, D1, D2, D3}, [r12]!
        VST1.S16       {D4, D5, D6, D7}, [r12]!
        VST1.S16       {D8, D9}, [r12]!
        MOV            r12, #1                @h_fac = 1
        MOV            r10, #2                @scaling = 2
        
@*********
@ need store register
@ r0 -- exc[]@ r1 -- xn[]@ r2 -- h[]@ r4 -- t_min@ r5 -- t_max
@ r6 -- corr_norm[]@ r8 --- excf[] or s_excf[]@
@ r12 -- h_fac@ r10 -- scaling@ r14 --- k--

LOOP_FOR:

        VDUP.S32       Q13, r12
        @LDR        r5, [r13, #T_MAX]                           @ get = t_max
        MOV            r7, r4                                      @ i = t_min
        RSB            r14, r4, #0
        SUB            r14, r14, #1                                @ k--   r14 to k-- 
LOOP:
        @VMOV.S32       Q10,  #0                                    @ s = 0
        @VMOV.S32       Q11, #0                                    @ s1 = 0
        MOV            r3, r8                                      @ r3 --- s_excf[j]
        MOV            r11, r3                                     @ copy s_excf address
        MOV            r9, r1                                      @ r9 --- xn[j]
        VLD1.S16       {D0, D1, D2, D3}, [r3]!                     @ load 4*4 s_excf[]
        VLD1.S16       {D4, D5, D6, D7}, [r3]!                     @ load 4*4 s_excf[]       
        VLD1.S16       {D8, D9}, [r3]!                             @ load 4 s_excf[]

        VLD1.S16       {D10, D11, D12, D13}, [r9]!                 @ load 4*4 x[]
        VLD1.S16       {D14, D15, D16, D17}, [r9]!                 @ load 4*4 x[]
        VLD1.S16       {D18, D19}, [r9]!                           @ load 4 x[]

        VQDMULL.S16    Q10, D0, D0
        VQDMULL.S16    Q11, D0, D10        
        VQDMLAL.S16    Q10, D1, D1
        VQDMLAL.S16    Q11, D1, D11
        VQDMLAL.S16    Q10, D2, D2
        VQDMLAL.S16    Q11, D2, D12        
        VQDMLAL.S16    Q10, D3, D3
        VQDMLAL.S16    Q11, D3, D13
        VQDMLAL.S16    Q10, D4, D4
        VQDMLAL.S16    Q11, D4, D14
        VQDMLAL.S16    Q10, D5, D5
        VQDMLAL.S16    Q11, D5, D15
        VQDMLAL.S16    Q10, D6, D6
        VQDMLAL.S16    Q11, D6, D16
        VQDMLAL.S16    Q10, D7, D7
        VQDMLAL.S16    Q11, D7, D17
        VQDMLAL.S16    Q10, D8, D8
        VQDMLAL.S16    Q11, D8, D18
        VQDMLAL.S16    Q10, D9, D9        
        VQDMLAL.S16    Q11, D9, D19 
        
        VQADD.S32      D20, D20, D21
        VQADD.S32      D22, D22, D23
        
        MOV            r3, r2                                      @r3 --- h[] address
        VLD1.S16       {D10, D11, D12, D13}, [r3]!
        VLD1.S16       {D14, D15, D16, D17}, [r3]!
        VLD1.S16       {D18, D19}, [r3]!                           @load all h[] elements

        @ exc[k]
        ADD            r3, r0, r14, LSL #1                         @get exc[k] address
        LDRSH          r9, [r3] 
        
        VDUP.S16       D28, r9                                     @bl0 ---- exc[k]
        
        
        VQDMULL.S16    Q12, D10, D28                              @L_mult(exc[k], h[j])
        VQSHL.S32      Q12, Q12, Q13                              @L_shl(s2, h_fac)
        VSHRN.S32      D10, Q12, #16                              @extract_h(s)
        
         
        VQDMULL.S16    Q12, D11, D28                              @L_mult(exc[k], h[j])
        VQSHL.S32      Q12, Q12, Q13                              @L_shl(s2, h_fac)
        VSHRN.S32      D11, Q12, #16                         


        VQDMULL.S16    Q12, D12, D28                             @L_mult(exc[k], h[j])
        VQSHL.S32      Q12, Q12, Q13                             @L_shl(s2, h_fac)
        VSHRN.S32      D12, Q12, #16


        VQDMULL.S16    Q12, D13, D28                              @L_mult(exc[k], h[j])
        VQSHL.S32      Q12, Q12, Q13                             @L_shl(s2, h_fac)
        VSHRN.S32      D13, Q12, #16


        VQDMULL.S16    Q12, D14, D28                              @L_mult(exc[k], h[j])
        VQSHL.S32      Q12, Q12, Q13                             @L_shl(s2, h_fac)
        VSHRN.S32      D14, Q12, #16


        VQDMULL.S16    Q12, D15, D28                              @L_mult(exc[k], h[j])
        VQSHL.S32      Q12, Q12, Q13                             @L_shl(s2, h_fac)
        VSHRN.S32      D15, Q12, #16


        VQDMULL.S16    Q12, D16, D28                             @L_mult(exc[k], h[j])
        VQSHL.S32      Q12, Q12, Q13                             @L_shl(s2, h_fac)
        VSHRN.S32      D16, Q12, #16



        VQDMULL.S16    Q12, D17, D28                              @L_mult(exc[k], h[j])
        VQSHL.S32      Q12, Q12, Q13                             @L_shl(s2, h_fac)
        VSHRN.S32      D17, Q12, #16


        VQDMULL.S16    Q12, D18, D28                             @L_mult(exc[k], h[j])
        VQSHL.S32      Q12, Q12, Q13                             @L_shl(s2, h_fac)
        VSHRN.S32      D18, Q12, #16


        VQDMULL.S16    Q12, D19, D28                              @L_mult(exc[k], h[j])
        VQSHL.S32      Q12, Q12, Q13                             @L_shl(s2, h_fac)
        VSHRN.S32      D19, Q12, #16

        @b10---b19
        @L_mult(exc[k], h[0])----- L_mult(exc[k], h[39]) 
            
        SUB            r3, r8, #2                                  @get the s_excf[-1] address
        
        @a10---a19
        @a10-a19 ---- s_excf[-1]----s_excf[38] 

        VLD1.S16       {Q0, Q1}, [r3]!
        VLD1.S16       {Q2, Q3}, [r3]!
        VLD1.S16       {Q4}, [r3]!          
        
        VQADD.S16      Q0, Q0, Q5
                               
	@s_excf[3] = add (extract_h(s2), s_excf[2])@
	@s_excf[2] = add (extract_h(s3), s_excf[1])@
	@s_excf[1] = add (extract_h(s4), s_excf[0])@
	@s_excf[0] = shr (exc[k], scaling)@
	
	
        VQADD.S16      Q1, Q1, Q6
        VQADD.S16      Q2, Q2, Q7
        VQADD.S16      Q3, Q3, Q8
        VQADD.S16      Q4, Q4, Q9
        
        MOV            r11, r8
        MOV            r9, r9, ASR r10                             @s_excf[0]= shr(exc[k], scaling)
        STRH           r9, [r11], #2
        VMOV.S16       r3, D0[1]
        VMOV.S16       r9, D0[2]
        STRH           r3, [r11], #2
        STRH           r9, [r11], #2
        VMOV.S16       r3, D0[3]
        STRH           r3, [r11], #2
        VST1.S16       D1, [r11]!
        VST1.S16       {Q1, Q2}, [r11]!
        VST1.S16       {Q3, Q4}, [r11]!

        @ store s_excf[0]----s_excf[39]
        
        VMOV.S32       r3, D20[0]
        VMOV.S32       r9, D20[1]
        QADD           r3, r3, r9

        STMFD          sp!, {r0 - r2, r14}
        MOV            r0, r3
        BL             Inv_sqrt1
        MOV            r3, r0
        LDMFD          sp!, {r0 - r2, r14}

   
        VMOV.S32       r9, D22[0]
        VMOV.S32       r11, D22[1]
        QADD           r9, r9, r11
 
        @ r3 --- s = Inv_sqrt (s)@
        @ r9 --- s1
           
        SMULTT         r11, r3, r9                                      @ s = (corr_h * norm_h)@
        MOV            r6, r3, LSR #16                     @ r6 --- norm_h
        SUB            r5, r3, r6, LSL #16                
        MOV            r5, r5, ASR #1                      @ r5 --- norm_l

        SMULTB         r5, r9, r5
        MOV            r5, r5, ASR #15
        ADD            r11, r11, r5

        MOV            r6, r9, LSR #16                     @ r6 --- corr_h
        SUB            r5, r9, r6, LSL #16
        MOV            r5, r5, ASR #1                      @ r5 --- corr_l

        SMULTB         r5, r3, r5
        MOV            r5, r5, ASR #15
        ADD            r11, r11, r5
 
  
        @SSAT       r11, #32, r11, LSL #17                      @ L_shl(s, 17)
              
        CMP            r11, #0
        BEQ            HERE
        EOR            r5, r11, r11, LSL #1
        CLZ            r3, r5
        CMP            r3, #17
        MOVGE          r11, r11, LSL #17
        BGE            HERE
        CMP            r11, #0
        MOVLT          r11, #0x80000000
        MOVGT          r11, #0x7fffffff
        
HERE:
        MOV            r11, r11, LSR #16                           @ extract_h()
        LDR            r3, [r13, #CORR_NORM]                       @ get corr_norm address
        LDR            r5, [r13, #T_MAX]                           @ get = t_max
        ADD            r3, r3, r7, LSL #1                          @ get corr_norm[i] address
        STRH           r11, [r3]
        ADD            r7, r7, #1
        SUB            r14, r14, #1
        CMP            r7, r5
        BLE            LOOP  
            

Norm_corr_asm_end: 
        
        ADD            r13, r13, #STACK_USED      
        LDMFD          r13!, {r4 - r12, r15}
    
        .END
