@**************************************************************
@* Copyright 2003 ~ 2010 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************

@static Word16 Vq_subvec (/* o : quantization index,            Q0  */
@    Word16 *lsf_r1,      /* i : 1st LSF residual vector,       Q15 */
@    Word16 *lsf_r2,      /* i : 2nd LSF residual vector,       Q15 */
@    const Word16 *dico,  /* i : quantization codebook,         Q15 */
@    Word16 *wf1,         /* i : 1st LSF weighting factors      Q13 */
@    Word16 *wf2,         /* i : 2nd LSF weighting factors      Q13 */  
@    Word16 dico_size     /* i : size of quantization codebook, Q0  */
@)
@*********************************************************************

@***************************** Change History**************************
@* 
@*    DD/MMM/YYYY     Code Ver     Description             Author
@*    -----------     --------     -----------             ------
@*    08-12-2008        1.0        File imported from      Huaping Liu
@*                                             
@**********************************************************************
        #include "voAMRNBEncID.h"
        .text     .align    4
        .globl    _Vq_subvec_asm  

_Vq_subvec_asm:

        stmfd       sp!, {r4-r11, r14}
        sub         r13, r13, #12
        mov         r12, #0x3fffffff       
                    
        ldrsh       r6,  [r0]                              @r4 = lsf_r1[1]<<16+lsf_r1[0]
        ldrsh       r7,  [r0, #2]
        ldrsh       r8,  [r1]                              @r5 = lsf_r2[1]<<16+lsf_r2[0]
        ldrsh       r9,  [r1, #2]
        pkhbt       r4,  r6, r7, lsl #16
        ldr         r10, [r13,#48]                         @load wf2 address     
        pkhbt       r5,  r8, r9, lsl #16

        ldrsh       r6,  [r3]                              @r6 = wf1[1]<<16+wf1[0] 
        ldrsh       r8,  [r3, #2]   
        ldrsh       r7,  [r10]                             @r7 = wf2[1]<<16+wf2[0] 
        ldrsh       r9,  [r10, #2]
        pkhbt       r6,  r6, r8, lsl #16
        str         r0,  [r13]                             @store lsf_r1 
        pkhbt       r7,  r7, r9, lsl #16

        str         r1,  [r13, #4]                         @store lsf_r2 
        mov         r14, r2                                @p_dico = dico                  
        str         r14, [r13, #8]                         @store dico 

        mov         r0,  r14
        ldr         r3,  [r13, #52]                       @load r3 = dico_size 
                                                           @dist_min = MAX_32        
LOOP:
        ldrsh     r8, [r14], #2                   
        ldrsh     r9, [r14], #2
        pkhbt     r8, r8, r9, lsl #16        
        ldrsh     r9, [r14], #2                      
        ldrsh     r10,[r14], #2
        pkhbt     r9, r9, r10, lsl #16  

        qsub16    r8, r4, r8                            
        qsub16    r9, r5, r9                           
 
        smulbb    r1, r6, r8                           
        smultt    r2, r6, r8                           
        smulbb    r8, r7, r9                         
        mov       r1, r1, asr #15
        mov       r2, r2, asr #15

        mul       r10, r1, r1
        mov       r8, r8, asr #15
        mla       r10, r2, r2, r10

        smultt    r2, r7, r9
        mla       r10, r8, r8, r10
        mov       r2, r2, asr #15
        mla       r10, r2, r2, r10

        cmp       r10, r12                             @if (dist < dist_min) 
        movlt     r0, r14                             @p_dico_index = p_dico 
        movlt     r12, r10                            @dist_min = dist                                                        
        subs      r3, r3, #1
        bne       LOOP

Vq_subvec_asm_end:

        ldr     r1,[r13,#8]  
        ldr     r7,[r13]                               
        ldr     r8,[r13,#4]       
        sub     r0, r0, r1      
        sub     r0, r0, #8
        add     r10, r0, r1
        mov     r0, r0, asr #3
        

        ldrsh   r6, [r10],  #2                          @r6--> lsf_r1[0]  
        ldrsh   r5, [r10],  #2                          @r5--> lsf_r1[1]
        strh    r6, [r7], #2 
        ldrsh   r4, [r10], #2                          @r4--> lsf_r2[0] 
        strh    r5, [r7] 
        ldrsh   r2, [r10],  #2                          @r2--> lsf_r2[1]  
       
        strh    r4, [r8], #2
         
        add     r13, r13, #12 
        strh    r2, [r8] 
        ldmfd   sp!, {r4 - r11, r15}        
        
        @.END




