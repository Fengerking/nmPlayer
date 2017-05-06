@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@****************************************************************
@* File Name: 
@*            amrnb_vq_subvec_neon.s
@* description: 
@*            This module implements the Vq_subvec().
@* Functions Included:
@*            1. Word16 Vq_subvec()
@*
@***************************** Change History**************************
@* 
@*    dd/MMM/YYYY     Code Ver     description             Author
@*    -----------     --------     -----------             ------
@*    08-12-2008        1.0        File imported from      Huaping Liu
@*                                             
@**********************************************************************
@**************************
@ constant
@**************************
.set    LSF_R1,         0
.set    LSF_R2,         LSF_R1+4
.set    dICO,           LSF_R2+4
.set    STACK_uSEd ,    dICO+4
.set    WF2,            STACK_uSEd+4*9
.set    dICO_SIZE,      WF2+4

@**************************
@ARM Register
@**************************
@ *lsf_r1                RN               r0
@ *lsf_r2                RN               r1
@ *dico                  RN               r2
@ *wf1                   RN               r3
@ *f2                    RN               r4
@ *dico_size             RN               r5
        #include "voAMRNBEncID.h"  
        .text   .align  4
        .globl  _Vq_subvec_asm  

_Vq_subvec_asm:

        stmfd       sp!, {r4 - r11, r14}
        sub         r13, r13, #STACK_uSEd
        ldr         r10, [r13,#WF2]                                @ get wf2 address 
        str         r0,  [r13, #LSF_R1]                            @ push lsf_r1 
        str         r1,  [r13, #LSF_R2]                            @ push lsf_r2  

        vld1.s16    d4, [r0]!                                      @ lsf_r1[0] ~ lsf_r1[3]
        vld1.s16    d6, [r1]!                                      @ lsf_r2[0] ~ lsf_r2[3]
        vzip.32     d4, d6                                         @ generate d4: lsf_r1[0], lsf_r1[1]@ lsf_r2[0], lsf_r2[1]

        vld1.s16    d5, [r3]!                                      @ wf1[0] ~ wf1[3]
        vld1.s16    d6, [r10]!                                     @ wf2[0] ~ wf2[3]
        vzip.32     d5, d6                                        @ generate d5: wf1[0], wf1[1]@ wf2[0], wf2[1]
          
        mov         r12, r2                                        @p_dico = dico                  
        str         r12, [r13, #dICO]                              @push dico 

        ldr         r3,  [r13, #dICO_SIZE]                         @r3-->dico_size 

        ldr         r6, =0x3fffffff  
        vdup.s32    q8 , r6                                        @dist_min = MAX_32    
        
	mov         r7, #0
	vmov.s32    d12[0], r7
	mov         r7, #1
	vmov.s32    d12[1], r7
	mov         r7, #2
	vmov.s32    d13[0], r7
	mov         r7, #3
	vmov.s32    d13[1], r7

        @AdR         r7, start_index
        @vld1.s32    q6, [r7]!

        ldr         r8, =0x4
        vdup.s32    q7, r8
                                                              
LOOP:
        vld1.s16    {d0, d1, d2, d3}, [r12]!                   @load 16 p_dico[], p_dico[0] ~ p_dico[15]

        vqsub.s16   d6,  d4, d0                                @ sub(lsf_r1[0], *p_dico++) ~ sub(lsf_r2[1], *p_dico++)
        vmull.s16   q13, d5, d6                              
        vshrn.s32   d6, q13, #15                               @ mult(wf1[0], temp) ~ mult(wf2[1], temp)
        vmull.s16   q9, d6, d6

        vqsub.s16   d6,  d4, d1
        vmull.s16   q13, d5, d6
        vshrn.s32   d6, q13, #15
        vmull.s16   q10, d6, d6

        vqsub.s16   d6,  d4, d2
        vmull.s16   q13, d5, d6
        vshrn.s32   d6, q13, #15
        vmull.s16   q11, d6, d6

        vqsub.s16   d6,  d4, d3
        vmull.s16   q13, d5, d6
        vshrn.s32   d6, q13, #15
        vmull.s16   q12, d6, d6                                      @ get 4 dist
   
        vadd.s32    d18, d18, d19
        vpadd.s32   d18, d18, d18                              @ d18[0] is the sum of q9

        vadd.s32    d20, d20, d21
        vpadd.s32   d20, d20, d20                           @ d20[0] is the sum of q9 
        vmov.s32    r4, d20[0]        
      
        vadd.s32    d22, d22, d23
        vpadd.s32   d22, d22, d22                           @ d22[0] is the sum of q9 
        vmov.s32    r5, d22[0] 

	vadd.s32    d24, d24, d25
        vpadd.s32   d24, d24, d24                           @ d24[0] is the sum of q9
        vmov.s32    r6, d24[0]

        vmov.s32    d18[1], r4
        vmov.s32    d19[0], r5
        vmov.s32    d19[1], r6                                         @ q9 store 4 sum result

        vclt        q5.u32, q9.s32, q8.s32
        vmin.s32    q8, q8, q9                             @ update q8
        vbit        q15.s32, q6.s32, q5.u32                @ store min index
        vadd.s32    q6, q6, q7                             @ update q6
        subs        r3, r3, #4
        bgt         LOOP
        
Vq_subvec_asm_end:

        vext.s32    q9, q8, q8, #2
        vext.s32    q6, q15, q15, #2
        vclt        q5.u32, q9.s32, q8.s32
        vmin.s32    q8, q8, q9
        vbit        q15.s32, q6.s32, q5.u32
    
        vmov.s32    r4, d16[0]
        vmov.s32    r5, d16[1]
        vmov.s32    r6, d30[0]
        vmov.s32    r7, d30[1]      
 
        cmp         r4, r5
        movlt       r0, r6
        movgt       r0, r7
                 
        ldr         r7,[r13,#LSF_R1]                      @r7--> lsf_r1  store lsf_r1 
        ldr         r8,[r13,#LSF_R2]                      @r8--> lsf_r2 @pop lsf_r2
        add         r10, r2, r0, lsl #3                   @&dico[shl2(index, 2)]
        
        ldrsh       r6,[r10], #2                          @r6--> lsf_r1[0]  
        ldrsh       r5,[r10], #2                          @r5--> lsf_r1[1] 
        ldrsh       r4,[r10], #2                          @r4--> lsf_r2[0]  
        ldrsh       r2,[r10], #2                          @r2--> lsf_r2[1]       
        
        strh        r6,[r7],#2
        strh        r5,[r7]
        strh        r4,[r8],#2
        strh        r2,[r8]
               
        add         r13, r13, #STACK_uSEd  
        ldmfd       sp!, {r4 - r11, r15} 
 
@start_index:

@        .word         0x0000000100000000,0x0000000300000002                        @start_index[j] = j    
                         






