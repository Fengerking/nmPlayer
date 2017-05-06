@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
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
        #include "voAMRNBEncID.h"
        .section   .text
        .global    Vq_subvec_asm  

Vq_subvec_asm:
        STMFD       sp!, {r4-r11, r14}
        SUB         r13, r13, #12
        MOV         r12, #0x3fffffff       

        LDRSH       r6,  [r0]                          
        LDRSH       r7,  [r0, #2]
        LDRSH       r8,  [r1]                     
        LDRSH       r9,  [r1, #2]
        PKHBT       r4,  r6, r7, LSL #16
        LDR         r10, [r13,#48]                   
        PKHBT       r5,  r8, r9, LSL #16

        LDRSH       r6,  [r3]                        
        LDRSH       r8,  [r3, #2]   
        LDRSH       r7,  [r10]                      
        LDRSH       r9,  [r10, #2]
        PKHBT       r6,  r6, r8, LSL #16
        STR         r0,  [r13]            
        PKHBT       r7,  r7, r9, LSL #16

        STR         r1,  [r13, #4]           
        MOV         r14, r2                                      
        STR         r14, [r13, #8]             

        MOV         r0,  r14
        LDR         r3,  [r13, #52]              
                                                     
LOOP:
        LDRSH     r8, [r14], #2                         
        LDRSH     r9, [r14], #2
        PKHBT     r8, r8, r9, LSL #16        
        LDRSH     r9, [r14], #2                         
        LDRSH     r10,[r14], #2
        PKHBT     r9, r9, r10, LSL #16  
        @LDR       r8, [r14], #4
        @LDR       r9, [r14], #4

        QSUB16    r8, r4, r8                             
        QSUB16    r9, r5, r9                            
 
        SMULBB    r1, r6, r8                            
        SMULTT    r2, r6, r8                            
        SMULBB    r8, r7, r9                            
        MOV       r1, r1, ASR #15
        MOV       r2, r2, ASR #15

        MUL       r10, r1, r1
        MOV       r8, r8, ASR #15
        MLA       r10, r2, r2, r10

        SMULTT    r2, r7, r9
        MLA       r10, r8, r8, r10
        MOV       r2, r2, ASR #15
        MLA       r10, r2, r2, r10

        CMP       r10,r12                             
        MOVLT     r0, r14                             
        MOVLT     r12, r10                                                                        
        SUBS      r3,r3,#1
        BNE       LOOP


Vq_subvec_asm_end:

        LDR     r1,[r13,#8]                        
        LDR     r7,[r13]                      
        LDR     r8,[r13,#4]                       
        SUB     r0, r0, r1                          
        SUB     r0, r0, #8
        ADD     r10, r0, r1
        MOV     r0, r0, ASR #3
        

        LDRSH   r6,[r10], #2                          
        LDRSH   r5,[r10], #2                          
        STRH    r6,[r7],#2 
        LDRSH   r4,[r10], #2                          
        STRH    r5,[r7] 
        LDRSH   r2,[r10], #2                          
       
        STRH    r4,[r8],#2
         
        ADD     r13, r13, #12 
        STRH    r2, [r8] 
        LDMFD   sp!, {r4 - r11, r15}        
        
        .END




